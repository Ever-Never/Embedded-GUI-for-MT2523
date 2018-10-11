/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "hal.h"
#include "hal_msdc.h"
#include "hal_sdio_internal.h"
#include "hal_sdio.h"

#ifdef HAL_SDIO_MODULE_ENABLED
#include "hal_log.h"
#include "assert.h"
#include "hal_platform.h"
#include "hal_gpt.h"
#include <string.h>
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#endif


/* This MACRO return 0 if condtion is meet, otherwise timeout happens */
#define MSDC_TIMEOUT_WAIT(cond, time) \
   do{\
        uint32_t start_count = 0;\
            uint32_t end_count = 0;\
        uint32_t count = 0;\
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &start_count);\
        do {\
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);\
            hal_gpt_get_duration_count(start_count, end_count, &count);\
            if ((cond))\
                break;\
        } while(count < time);\
    } while(0)

#define MSDC_STOP_DMA() \
    do { \
        MSDC_REG->DMA_CTRL |= MSDC_DMA_CTRL_STOP; \
        MSDC_TIMEOUT_WAIT(((!(MSDC_REG->DMA_CTRL & MSDC_DMA_CTRL_STOP)) || (!(MSDC_REG->DMA_CFG & MSDC_DMA_CFG_STS))), 500); \
    } while(0)


extern volatile sdio_information_t sdio_information;
extern msdc_config_t msdc_config;
gpd_t gpd[2];
bd_t bd;

extern void restore_interrupt_mask(uint32_t mask);
extern uint32_t save_and_set_interrupt_mask(void);
hal_sdio_status_t hal_sdio_register_callback(hal_sdio_port_t sdio_port, hal_sdio_callback_t sdio_callback,void *user_data)
{
    if (!sdio_callback) {
        return HAL_SDIO_STATUS_ERROR;
    }

    msdc_config.msdc_sdio_dma_interrupt_callback = sdio_callback;
    return HAL_SDIO_STATUS_OK;
}

hal_sdio_status_t hal_sdio_set_bus_width(hal_sdio_port_t sdio_port, hal_sdio_bus_width_t bus_width)
{
    sdio_internal_status_t status;
    hal_sdio_command52_config_t command52;

    command52.direction = HAL_SDIO_DIRECTION_READ;
    command52.function = HAL_SDIO_FUNCTION_0;
    command52.read_after_write = false;
    command52.stop = false;
    command52.address = 0x07;

    status = sdio_command52(&command52);
    if (status != NO_ERROR) {
        goto error;
    }

    command52.data = command52.data & (~(SDIO_CCCR_BUS0_BIT_MASK | SDIO_CCCR_BUS1_BIT_MASK));
    /*set bus width and disable card detection*/
    command52.data =  command52.data | SDIO_CCCR_CD_BIT_MASK |
                      ((bus_width == HAL_SDIO_BUS_WIDTH_4) ? SDIO_CCCR_BUS1_BIT_MASK : 0);

    command52.direction = HAL_SDIO_DIRECTION_WRITE;
    status = sdio_command52(&command52);
    if (status != NO_ERROR) {
        goto error;
    }

    if (bus_width == HAL_SDIO_BUS_WIDTH_4) {
        MSDC_REG->SDC_CFG = (MSDC_REG->SDC_CFG & (~SDC_CFG_BUSWIDTH)) | (MSDC_SDIO_BUS_WIDTH_4BITS << SDC_CFG_BUSWIDTH_OFFSET);
    } else {
        MSDC_REG->SDC_CFG = MSDC_REG->SDC_CFG & (~SDC_CFG_BUSWIDTH);
    }

    sdio_information.bus_width = bus_width;

    return HAL_SDIO_STATUS_OK;
error:

    log_hal_error("sdio error status = %d \r\n", status);

    return HAL_SDIO_STATUS_ERROR;
}


hal_sdio_status_t hal_sdio_init(hal_sdio_port_t sdio_port, hal_sdio_config_t *sdio_config)
{
    sdio_internal_status_t  status;
    uint32_t retry = 0;
    bool is_io_ready = false;
    uint32_t irq_status;
    msdc_bus_width_t bus_width;

    if (NULL == sdio_config) {
        log_hal_error("parameter error");
        return HAL_SDIO_STATUS_ERROR;
    }

    bus_width = (sdio_config->bus_width == HAL_SDIO_BUS_WIDTH_1) ? MSDC_BUS_WIDTH_1BITS : MSDC_SDIO_BUS_WIDTH_4BITS;

    irq_status = save_and_set_interrupt_mask();
    if (sdio_information.is_busy) {
        restore_interrupt_mask(irq_status);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    restore_interrupt_mask(irq_status);

    sdio_information.is_initialized = false;

    if (MSDC_INITIALIZED == msdc_init(bus_width)) {
        if (MSDC_OWNER_SD == msdc_get_owner()) {
            sdio_information.is_busy = false;
            return HAL_SDIO_STATUS_ERROR;
        }
    }
#ifdef HAL_SLEEP_MANAGER_ENABLED
    sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_SDIO_MST, (sleep_management_suspend_callback_t)sdio_backup_all, NULL);
    sleep_management_register_resume_callback(SLEEP_BACKUP_RESTORE_SDIO_MST, (sleep_management_resume_callback_t)sdio_restore_all, NULL);
#endif

#ifdef MSDC_USE_INT
    msdc_nvic_set(true);
#else
    msdc_nvic_set(false);
#endif

    msdc_reset();

    /*disable 4bit*/
    MSDC_REG->SDC_CFG = MSDC_REG->SDC_CFG & (~SDC_CFG_BUSWIDTH);

    msdc_set_output_clock(240); /*setting bus clock to 240KHz.*/

    /*reset data timeout conter to 65536*256 cycles*/
    MSDC_REG->SDC_CFG = MSDC_REG->SDC_CFG | 0xFF000000;

    /*enable SDIO mode*/
    MSDC_REG->SDC_CFG |= SDC_CFG_SDIO;

    /*enable 74 serial clock*/
    MSDC_REG->MSDC_CFG = MSDC_REG->MSDC_CFG | MSDC_CFG_CKPDN;

    /*enable SDIO interrupt*/
    MSDC_REG->SDC_CFG |= SDC_CFG_SDIOIDE;

    msdc_wait(10);

    /*disable serial clock*/
    MSDC_REG->MSDC_CFG = MSDC_REG->MSDC_CFG & (~MSDC_CFG_CKPDN);

    status = sdio_send_command5(COMMAND_NO_ARGUMENT);
    if (status != NO_ERROR) {
        goto error;
    }

    do {
        retry ++;

        status = sdio_send_command5(sdio_information.ocr);

        if (100 < retry) {
            goto error;
        }

    } while (!sdio_information.is_io_ready);

    msdc_set_output_clock(MSDC_OUTPUT_CLOCK);

    status = sdio_get_rca();
    if (status != NO_ERROR) {
        goto error;
    }

    status = sdio_select_card(sdio_information.rca);
    if (status != NO_ERROR) {
        goto error;
    }

    status = sdio_get_cccr();
    if (status != NO_ERROR) {
        goto error;
    }

    sdio_get_capacity();

    sdio_get_power_control();

    if (!(sdio_is_support_lsc())) {
        if (sdio_is_support_s4mi()) {
            status = sdio_set_e4mi(true);
            if (status != NO_ERROR) {
                goto error;
            }
        }

        if (HAL_SDIO_STATUS_OK != hal_sdio_set_bus_width(HAL_SDIO_PORT_0, sdio_config->bus_width)) {
            msdc_deinit();
            return HAL_SDIO_STATUS_ERROR;
        }

    } else if (sdio_is_support_4bls() && (sdio_config->bus_width == HAL_SDIO_BUS_WIDTH_4)) {
        if (HAL_SDIO_STATUS_OK != hal_sdio_set_bus_width(HAL_SDIO_PORT_0, sdio_config->bus_width)) {
            msdc_deinit();
            return HAL_SDIO_STATUS_ERROR;
        }
    }

    status =  sdio_get_cis(HAL_SDIO_FUNCTION_0);
    if (status < NO_ERROR) {
        goto error;
    }

    for (retry = 0; retry < 100; retry++) {
        status = sdio_set_io(HAL_SDIO_FUNCTION_1, true);
        if (status < NO_ERROR) {
            goto error;
        }

        status = sdio_check_io_ready(HAL_SDIO_FUNCTION_1, &is_io_ready);
        if (status < NO_ERROR) {
            goto error;
        }

        if (is_io_ready) {
            break;
        }
    }

    status = sdio_get_fbr(HAL_SDIO_FUNCTION_1);
    if (status < NO_ERROR) {
        goto error;
    }

    if (sdio_is_support_mps()) {
        status = sdio_set_mps(true);
        if (status < NO_ERROR) {
            goto error;
        }
    }

    if (sdio_config->clock >= SDIO_DEFAULT_MAX_SPEED) {
        sdio_get_high_speed();
        if (sdio_is_support_shs()) {
            status = sdio_set_ehs(true);
            if (status < NO_ERROR) {
                goto error;
            }
            msdc_set_output_clock(sdio_config->clock);
        } else {
            msdc_set_output_clock(SDIO_DEFAULT_MAX_SPEED);
        }
    } else {
        msdc_set_output_clock(sdio_config->clock);
    }

    status = sdio_get_cccr();
    if (status != NO_ERROR) {
        goto error;
    }

    msdc_nvic_set(true);

    sdio_information.is_initialized = true ;
    sdio_information.is_busy = false;
    return HAL_SDIO_STATUS_OK;

error:
    if (status != NO_ERROR) {
        sdio_information.is_initialized = false;
    }
    sdio_information.is_busy = false;

    log_hal_error("sdio error status = %d \r\n", status);
    msdc_deinit();

    return HAL_SDIO_STATUS_ERROR;
}


hal_sdio_status_t hal_sdio_deinit(hal_sdio_port_t sdio_port)
{
    msdc_reset();
    memset((void *)(&sdio_information), 0, sizeof(sdio_information_t));

    msdc_deinit();

    return HAL_SDIO_STATUS_OK;
}


hal_sdio_status_t hal_sdio_execute_command52(hal_sdio_port_t sdio_port, hal_sdio_command52_config_t *command52_config)
{
    uint32_t irq_status;
    sdio_internal_status_t status;

    if (NULL == command52_config) {
        log_hal_error("parameter error");
        return HAL_SDIO_STATUS_ERROR;
    }

    log_hal_info("command52, r/w = %d, is_stop = %x, addr = %x \r\n", command52_config->direction, command52_config->stop, (unsigned int)command52_config->address);

    irq_status = save_and_set_interrupt_mask();
    if (sdio_information.is_busy) {
        restore_interrupt_mask(irq_status);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    restore_interrupt_mask(irq_status);

    status = sdio_command52(command52_config);

    if (NO_ERROR != status) {
        sdio_information.is_busy = false;

        log_hal_error("sdio error status = %d \r\n", (unsigned int)status);
        return HAL_SDIO_STATUS_ERROR;
    }

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_OK;
}

void msdc_dump_debug_reg(void)
{
    uint32_t i = 0;
    uint32_t reg;

    for (i = 0; i < 0x19; i++) {
        MSDC_REG->SW_DBG_DEL = i;
        msdc_wait(1);
        reg = MSDC_REG->SW_DBG_OUT;
        log_hal_info("i = %x, reg = %x \r\n", (unsigned int)i, (unsigned int)reg);
    }
}


/*********************************************************************************************************************************/
/************************************************CMD53 ***************************************************************************/
/*********************************************************************************************************************************/
/*| 1 | 1 |       6       |  1  |           3            |      1     |    1    |         17       |         9        |  7  | 1 |*/
/*| S | D | Command index | R/W | Numberof I/O functions | Block mode | OP code | register address | byte/block count | CRC | E |*/
/*********************************************************************************************************************************/
hal_sdio_status_t hal_sdio_execute_command53(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config)
{
    sdio_internal_status_t status;
    uint32_t argument = 0;
    uint32_t command;
    uint32_t response;
    uint32_t word_count;
    uint32_t *buffer_pointer;
    uint32_t irq_status;
    uint32_t block_size;
    uint32_t count;
    uint32_t fifo_count;
    uint32_t interrupte_status;

    if (NULL == command53_config) {
        log_hal_error("parameter error");
        return HAL_SDIO_STATUS_ERROR;
    }

    irq_status = save_and_set_interrupt_mask();
    if (sdio_information.is_busy) {
        restore_interrupt_mask(irq_status);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    restore_interrupt_mask(irq_status);
#ifdef MSDC_USE_INT
    msdc_nvic_set(true);
#else
    msdc_nvic_set(false);
#endif

    msdc_reset();
    msdc_clear_fifo();

    interrupte_status = MSDC_REG->MSDC_INT;
    if (0 != interrupte_status) {
        log_hal_info("interrupt status = %x before send cmd53 \r\n", (unsigned int)interrupte_status);
        MSDC_REG->MSDC_INT |= interrupte_status;
    }

    /*use pio mode*/
    MSDC_REG->MSDC_CFG |= MSDC_CFG_PIO;

    /***************************************************************/
    /******************config command53 argument********************/
    /***************************************************************/
    /*set R/W bit*/
    argument = (command53_config->direction == HAL_SDIO_DIRECTION_WRITE) ? (argument | COMMAND53_DIRECTION_BIT_MASK) :
               (argument & (~COMMAND53_DIRECTION_BIT_MASK));
    /*set Numberof I/O functions bit*/
    argument = (argument & (~COMMAND53_FUNCTION_NUMBER_BIT_MASK)) |
               ((uint32_t)command53_config->function << COMMAND53_FUNCTION_NUMBER_BIT_SHIFT);
    /*set block mode bit*/
    argument = command53_config->block ? (argument | COMMAND53_BLOCK_MODE_BIT_MASK) : (argument & (~COMMAND53_BLOCK_MODE_BIT_MASK));
    /*set OP code bit*/
    argument = command53_config->operation ? (argument | COMMAND53_OP_BIT_MASK) : (argument & (~COMMAND53_OP_BIT_MASK));
    /*set register address bit*/
    argument = (argument & (~COMMAND53_REGISTER_ADDRESS_BIT_MASK)) |
               ((uint32_t)command53_config->address << COMMAND53_REGISTER_ADDRESS_BIT_SHIFT);
    /*set byte/block count bit*/
    argument = (argument & (~COMMAND53_COUNT_BIT_MASK)) |
               ((uint32_t)command53_config->count << COMMAND53_COUNT_BIT_SHIFT);


    /***************************************************************/
    /******************config command53 command********************/
    /***************************************************************/
    block_size = sdio_information.block_size[command53_config->function];

    command = MSDC_COMMAND53_SDIO;
    command = command53_config->direction ? (command | SDC_CMD_RW) : (command & (~SDC_CMD_RW));
    if (command53_config->block) {
        command = (command & (~SDC_CMD_DTYP)) | (MULTI_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN)) | (block_size << 16);
    } else {
        command = (command & (~SDC_CMD_DTYP)) | (SINGLE_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN)) | (command53_config->count << 16);
    }

    /*set block number.*/
    if (command53_config->block) {
        MSDC_REG->SDC_BLK_NUM = command53_config->count;
    } else {
        MSDC_REG->SDC_BLK_NUM = 1;
    }


    status =  sdio_send_command(command, argument);
    if (status != NO_ERROR) {
        goto error;
    }

    response = MSDC_REG->SDC_RESP0;

    sdio_information.response = (response & SDIO_R5_RESPONSE_FLAG_BIT_MASK) >> SDIO_R5_RESPONSE_FLAG_BIT_SHIFT;

    if (sdio_information.response & SDIO_R5_RESPONSE_FLAG_ERROR_BIT_MASK) {
        goto error;
    }


#ifdef MSDC_USE_INT
    MSDC_REG->MSDC_INTEN |= MSDC_DAT_INTS_EN | MSDC_INTEN_DXFER_DONE;
#endif

    buffer_pointer = (uint32_t *)command53_config->buffer;

    if (command53_config->direction) {
        /*write data*/
        if (command53_config->block) {
            word_count = (uint32_t)(command53_config->count * sdio_information.block_size[command53_config->function] + 3) >> 2;
        } else {
            word_count = (uint32_t)(command53_config->count + 3) >> 2;
        }

        while (word_count) {
            if ((word_count < (MSDC_FIFO_SIZE >> 2)) && (0 == MSDC_TXFIFOCNT())) {
                do {
                    MSDC_REG->MSDC_TXDATA = *buffer_pointer++;
                } while (--word_count);
            } else if (word_count >= (MSDC_FIFO_SIZE >> 2) && (0 == MSDC_TXFIFOCNT())) {
                count = MSDC_FIFO_SIZE >> 2;
                do {
                    MSDC_REG->MSDC_TXDATA = *buffer_pointer++;
                } while (--count);
                word_count = word_count - (MSDC_FIFO_SIZE >> 2);
            }
        }

    } else {
        /*read data*/
        if (command53_config->block) {
            word_count = (uint32_t)(command53_config->count * sdio_information.block_size[command53_config->function] + 3) >> 2;
        } else {
            word_count = (uint32_t)(command53_config->count + 3) >> 2;
        }

        while (word_count) {
            fifo_count = ((MSDC_REG->MSDC_FIFOCS & MSDC_FIFOCS_RXCNT) >> 0);
            if ((word_count < (MSDC_FIFO_SIZE >> 2)) && (word_count <= (fifo_count >> 2))) {
                do {
                    *buffer_pointer++ = MSDC_REG->MSDC_RXDATA;
                } while (--word_count);
            } else if ((word_count >= (MSDC_FIFO_SIZE >> 2)) && (MSDC_FIFO_SIZE <= fifo_count)) {
                count = MSDC_FIFO_SIZE >> 2;
                do {
                    *buffer_pointer++ = MSDC_REG->MSDC_RXDATA;
                } while (--count);
                word_count = word_count - (MSDC_FIFO_SIZE >> 2);
            }
        }
    }

    status = sdio_wait_data_ready();
    if (status != NO_ERROR) {
        goto error;
    }

    msdc_nvic_set(true);
    msdc_reset();

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_OK;

error:
    *(volatile uint32_t *)0xA2010060 = *(volatile uint32_t *)0xA2010060 | 0x8000;

    log_hal_error("sdio error status = %d \r\n", (unsigned int)status);
    if (command53_config->block) {
        sdio_command12_stop();
    }
    sdio_stop();
    msdc_reset();
    msdc_nvic_set(true);

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_ERROR;
}


void msdc_dma_stop(void)
{
    uint32_t wints = MSDC_INTEN_XFER_COMPL |    /* To detect one GPD data transfer complete */
                     MSDC_INTEN_DATTMO |        /* To detect data transfer timdout */
                     MSDC_INTEN_DATCRCERR;      /* To detect data transfer error */

    log_hal_info("***dma status: 0x%08x", (unsigned int)MSDC_REG->DMA_CFG);
    MSDC_STOP_DMA();

    MSDC_REG->MSDC_INTEN &= ~wints;
    MSDC_REG->MSDC_INT |= wints;

    log_hal_info("***dma stop...");
}


hal_sdio_status_t hal_sdio_execute_command53_dma_blocking(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config)
{
    sdio_internal_status_t status;
    uint32_t argument = 0;
    uint32_t command;
    uint32_t response;
    uint32_t *buffer_pointer;
    uint32_t irq_status;
    uint32_t block_size;
    uint32_t interrupte_status;

    if (NULL == command53_config) {
        log_hal_error("parameter error");
        return HAL_SDIO_STATUS_ERROR;
    }

    log_hal_info("command53_dma, r/w = %d, addr = %x, buffer = %x, is_block = %d, count = %x \r\n",
                 command53_config->direction, (unsigned int)command53_config->address, (unsigned int)command53_config->buffer, command53_config->block, (unsigned int)command53_config->count);

    irq_status = save_and_set_interrupt_mask();
    if (sdio_information.is_busy) {
        restore_interrupt_mask(irq_status);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    restore_interrupt_mask(irq_status);

#ifdef MSDC_USE_INT
    msdc_nvic_set(true);
#else
    msdc_nvic_set(false);
#endif

    msdc_reset();
    msdc_clear_fifo();

    interrupte_status = MSDC_REG->MSDC_INT;
    if (0 != interrupte_status) {
        log_hal_info("interrupt status = %x before send cmd53 \r\n", (unsigned int)interrupte_status);
        MSDC_REG->MSDC_INT |= interrupte_status;
    }

    /*enable DMA*/
    MSDC_REG->MSDC_CFG &= ~MSDC_CFG_PIO;

    /***************************************************************/
    /******************config command53 argument********************/
    /***************************************************************/
    /*set R/W bit*/
    argument = (command53_config->direction == HAL_SDIO_DIRECTION_WRITE) ? (argument | COMMAND53_DIRECTION_BIT_MASK) :
               (argument & (~COMMAND53_DIRECTION_BIT_MASK));
    /*set Numberof I/O functions bit*/
    argument = (argument & (~COMMAND53_FUNCTION_NUMBER_BIT_MASK)) |
               ((uint32_t)command53_config->function << COMMAND53_FUNCTION_NUMBER_BIT_SHIFT);
    /*set block mode bit*/
    argument = command53_config->block ? (argument | COMMAND53_BLOCK_MODE_BIT_MASK) : (argument & (~COMMAND53_BLOCK_MODE_BIT_MASK));
    /*set OP code bit*/
    argument = command53_config->operation ? (argument | COMMAND53_OP_BIT_MASK) : (argument & (~COMMAND53_OP_BIT_MASK));
    /*set register address bit*/
    argument = (argument & (~COMMAND53_REGISTER_ADDRESS_BIT_MASK)) |
               ((uint32_t)command53_config->address << COMMAND53_REGISTER_ADDRESS_BIT_SHIFT);
    /*set byte/block count bit*/
    argument = (argument & (~COMMAND53_COUNT_BIT_MASK)) |
               ((uint32_t)command53_config->count << COMMAND53_COUNT_BIT_SHIFT);


    /***************************************************************/
    /******************config command53 command********************/
    /***************************************************************/
    block_size = sdio_information.block_size[command53_config->function];

    command = MSDC_COMMAND53_SDIO;
    command = command53_config->direction ? (command | SDC_CMD_RW) : (command & (~SDC_CMD_RW));
    if (command53_config->block) {
        command = (command & (~SDC_CMD_DTYP)) | (MULTI_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN)) | (block_size << 16);
    } else {
        command = (command & (~SDC_CMD_DTYP)) | (SINGLE_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN)) | (command53_config->count << 16);
    }

    /*set block number.*/
    if (command53_config->block) {
        MSDC_REG->SDC_BLK_NUM = command53_config->count;
    } else {
        MSDC_REG->SDC_BLK_NUM = 1;
    }

    buffer_pointer = (uint32_t *)command53_config->buffer;

    status =  sdio_send_command(command, argument);
    if (status != NO_ERROR) {
        goto error;
    }

    response = MSDC_REG->SDC_RESP0;

    sdio_information.response = (response & SDIO_R5_RESPONSE_FLAG_BIT_MASK) >> SDIO_R5_RESPONSE_FLAG_BIT_SHIFT;

    if (sdio_information.response & SDIO_R5_RESPONSE_FLAG_ERROR_BIT_MASK) {
        goto error;
    }

    /*use basic DMA mode.*/
    MSDC_REG->DMA_CTRL &= ~MSDC_DMA_CTRL_MODE;

    MSDC_REG->DMA_CTRL = (MSDC_REG->DMA_CTRL & (~MSDC_DMA_CTRL_BRUSTSZ)) | (MSDC_DMA_BURST_8_BYTES << 12);
    MSDC_REG->DMA_CTRL |= MSDC_DMA_CTRL_LASTBUF;

    if (command53_config->block) {
        MSDC_REG->DMA_LENGTH = command53_config->count * sdio_information.block_size[command53_config->function];
    } else {
        MSDC_REG->DMA_LENGTH = command53_config->count;
    }

    MSDC_REG->DMA_SA = (uint32_t)buffer_pointer;

#ifdef MSDC_USE_INT
    MSDC_REG->MSDC_INTEN |= MSDC_DAT_INTS_EN | MSDC_INTEN_DXFER_DONE;
#endif

    MSDC_REG->DMA_CTRL |= MSDC_DMA_CTRL_START;

    status = sdio_wait_data_ready();
    if (status != NO_ERROR) {
        msdc_dump_debug_reg();
        goto error;
    }

    msdc_dma_stop();
    msdc_reset();

    msdc_nvic_set(true);
    msdc_reset();

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_OK;

error:
    *(volatile uint32_t *)0xA2010060 = *(volatile uint32_t *)0xA2010060 | 0x8000;

    log_hal_error("sdio error status = %d \r\n", status);

    if (command53_config->block) {
        sdio_command12_stop();
    }

    sdio_stop();
    msdc_reset();
    msdc_nvic_set(true);

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_ERROR;
}


hal_sdio_status_t hal_sdio_execute_command53_dma(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config)
{
    sdio_internal_status_t status;
    uint32_t argument = 0;
    uint32_t command;
    uint32_t response;
    uint32_t *buffer_pointer;
    uint32_t irq_status;
    uint32_t block_size;
    uint32_t interrupte_status;

    if (NULL == msdc_config.msdc_sdio_dma_interrupt_callback) {
        log_hal_error("sdio dma interrupt callback is null\r\n");
        return HAL_SDIO_STATUS_ERROR;
    }

    if (NULL == command53_config) {
        log_hal_error("parameter error\r\n");
        return HAL_SDIO_STATUS_ERROR;
    }

    irq_status = save_and_set_interrupt_mask();
    if (sdio_information.is_busy) {
        restore_interrupt_mask(irq_status);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    restore_interrupt_mask(irq_status);

    msdc_reset();
    msdc_clear_fifo();

    interrupte_status = MSDC_REG->MSDC_INT;
    if (0 != interrupte_status) {
        log_hal_error("error, interrupt status = %x before send cmd53 \r\n", (unsigned int)interrupte_status);
        MSDC_REG->MSDC_INT |= interrupte_status;
    }

    /*enable DMA*/
    MSDC_REG->MSDC_CFG &= ~MSDC_CFG_PIO;

    /***************************************************************/
    /******************config command53 argument********************/
    /***************************************************************/
    /*set R/W bit*/
    argument = (command53_config->direction == HAL_SDIO_DIRECTION_WRITE) ? (argument | COMMAND53_DIRECTION_BIT_MASK) :
               (argument & (~COMMAND53_DIRECTION_BIT_MASK));
    /*set Numberof I/O functions bit*/
    argument = (argument & (~COMMAND53_FUNCTION_NUMBER_BIT_MASK)) |
               ((uint32_t)command53_config->function << COMMAND53_FUNCTION_NUMBER_BIT_SHIFT);
    /*set block mode bit*/
    argument = command53_config->block ? (argument | COMMAND53_BLOCK_MODE_BIT_MASK) : (argument & (~COMMAND53_BLOCK_MODE_BIT_MASK));
    /*set OP code bit*/
    argument = command53_config->operation ? (argument | COMMAND53_OP_BIT_MASK) : (argument & (~COMMAND53_OP_BIT_MASK));
    /*set register address bit*/
    argument = (argument & (~COMMAND53_REGISTER_ADDRESS_BIT_MASK)) |
               ((uint32_t)command53_config->address << COMMAND53_REGISTER_ADDRESS_BIT_SHIFT);
    /*set byte/block count bit*/
    argument = (argument & (~COMMAND53_COUNT_BIT_MASK)) |
               ((uint32_t)command53_config->count << COMMAND53_COUNT_BIT_SHIFT);


    /***************************************************************/
    /******************config command53 command********************/
    /***************************************************************/
    block_size = sdio_information.block_size[command53_config->function];

    command = MSDC_COMMAND53_SDIO;
    command = command53_config->direction ? (command | SDC_CMD_RW) : (command & (~SDC_CMD_RW));
    if (command53_config->block) {
        command = (command & (~SDC_CMD_DTYP)) | (MULTI_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN)) | (block_size << 16);
    } else {
        command = (command & (~SDC_CMD_DTYP)) | (SINGLE_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN)) | (command53_config->count << 16);
    }

    /*set block number.*/
    if (command53_config->block) {
        MSDC_REG->SDC_BLK_NUM = command53_config->count;
    } else {
        MSDC_REG->SDC_BLK_NUM = 1;
    }

    buffer_pointer = (uint32_t *)command53_config->buffer;

    status =  sdio_send_command(command, argument);
    if (status != NO_ERROR) {
        goto error;
    }

    response = MSDC_REG->SDC_RESP0;

    sdio_information.response = (response & SDIO_R5_RESPONSE_FLAG_BIT_MASK) >> SDIO_R5_RESPONSE_FLAG_BIT_SHIFT;

    if (sdio_information.response & SDIO_R5_RESPONSE_FLAG_ERROR_BIT_MASK) {
        goto error;
    }

    /*use basic DMA mode.*/
    MSDC_REG->DMA_CTRL &= ~MSDC_DMA_CTRL_MODE;
    MSDC_REG->DMA_CTRL = (MSDC_REG->DMA_CTRL & (~MSDC_DMA_CTRL_BRUSTSZ)) | (MSDC_DMA_BURST_8_BYTES << 12);
    MSDC_REG->DMA_CTRL |= MSDC_DMA_CTRL_LASTBUF;
    if (command53_config->block) {
        MSDC_REG->DMA_LENGTH = command53_config->count * sdio_information.block_size[command53_config->function];
    } else {
        MSDC_REG->DMA_LENGTH = command53_config->count;
    }
    MSDC_REG->DMA_SA = (uint32_t)buffer_pointer;

    /*MSDC interrupt config*/
    MSDC_REG->MSDC_INTEN |= MSDC_DAT_INTS_EN | MSDC_INTEN_DXFER_DONE;
    msdc_nvic_set(true);

    MSDC_REG->DMA_CTRL |= MSDC_DMA_CTRL_START;

    return HAL_SDIO_STATUS_OK;

error:
  
    *(volatile uint32_t *)0xA2010060 = *(volatile uint32_t *)0xA2010060 | 0x8000;

    log_hal_error("sdio error status = %d \r\n", status);

    if (command53_config->block) {
        sdio_command12_stop();
    }

    sdio_stop();
    msdc_reset();
    msdc_nvic_set(true);

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_ERROR;

}
static uint8_t msdc_cal_checksum(const void *buf, uint32_t len)
{
    uint32_t i = 0;
    uint32_t sum = 0;

    char *data = (char *)buf;

    for (i = 0; i < len; i++) {
        sum += *data++;
    }
    return 0xFF - (uint8_t)sum;
}


hal_sdio_status_t hal_sdio_execute_command53_dma_gpd(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config)
{
    sdio_internal_status_t status;
    uint32_t argument = 0;
    uint32_t command;
    uint32_t response;
    uint32_t irq_status;
    uint32_t block_size;

    if (NULL == command53_config) {
        log_hal_error("parameter error");
        return HAL_SDIO_STATUS_ERROR;
    }

    log_hal_info("command53_dma, r/w = %d, addr = %x, buffer = %x, is_block = %d, count = %x \r\n",
                 command53_config->direction, (unsigned int)command53_config->address, (unsigned int)command53_config->buffer, command53_config->block, (unsigned int)command53_config->count);

    irq_status = save_and_set_interrupt_mask();
    if (sdio_information.is_busy) {
        restore_interrupt_mask(irq_status);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    restore_interrupt_mask(irq_status);

#ifdef MSDC_USE_INT
    msdc_nvic_set(true);
#else
    msdc_nvic_set(false);
#endif

    msdc_reset();
    msdc_clear_fifo();

    MSDC_REG->DMA_CTRL = (MSDC_REG->DMA_CTRL & (~MSDC_DMA_CTRL_BRUSTSZ)) | (MSDC_DMA_BURST_8_BYTES << 12);

    /*enable DMA*/
    MSDC_REG->MSDC_CFG &= ~MSDC_CFG_PIO;

    /***************************************************************/
    /******************config command53 argument********************/
    /***************************************************************/
    /*set R/W bit*/
    argument = (command53_config->direction == HAL_SDIO_DIRECTION_WRITE) ? (argument | COMMAND53_DIRECTION_BIT_MASK) :
               (argument & (~COMMAND53_DIRECTION_BIT_MASK));
    /*set Numberof I/O functions bit*/
    argument = (argument & (~COMMAND53_FUNCTION_NUMBER_BIT_MASK)) |
               ((uint32_t)command53_config->function << COMMAND53_FUNCTION_NUMBER_BIT_SHIFT);
    /*set block mode bit*/
    argument = command53_config->block ? (argument | COMMAND53_BLOCK_MODE_BIT_MASK) : (argument & (~COMMAND53_BLOCK_MODE_BIT_MASK));
    /*set OP code bit*/
    argument = command53_config->operation ? (argument | COMMAND53_OP_BIT_MASK) : (argument & (~COMMAND53_OP_BIT_MASK));
    /*set register address bit*/
    argument = (argument & (~COMMAND53_REGISTER_ADDRESS_BIT_MASK)) |
               ((uint32_t)command53_config->address << COMMAND53_REGISTER_ADDRESS_BIT_SHIFT);
    /*set byte/block count bit*/
    argument = (argument & (~COMMAND53_COUNT_BIT_MASK)) |
               ((uint32_t)command53_config->count << COMMAND53_COUNT_BIT_SHIFT);


    /***************************************************************/
    /******************config command53 command********************/
    /***************************************************************/
    block_size = sdio_information.block_size[command53_config->function];

    command = MSDC_COMMAND53_SDIO;
    command = command53_config->direction ? (command | SDC_CMD_RW) : (command & (~SDC_CMD_RW));
    if (command53_config->block) {
        command = (command & (~SDC_CMD_DTYP)) | (MULTI_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN)) | (block_size << 16);
    } else {
        command = (command & (~SDC_CMD_DTYP)) | (SINGLE_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN)) | (command53_config->count << 16);
    }

    /*set block number.*/
    if (command53_config->block) {
        MSDC_REG->SDC_BLK_NUM = command53_config->count;
    } else {
        MSDC_REG->SDC_BLK_NUM = 1;
    }

    status =  sdio_send_command(command, argument);
    if (status != NO_ERROR) {
        goto error;
    }

    response = MSDC_REG->SDC_RESP0;

    sdio_information.response = (response & SDIO_R5_RESPONSE_FLAG_BIT_MASK) >> SDIO_R5_RESPONSE_FLAG_BIT_SHIFT;

    if (sdio_information.response & SDIO_R5_RESPONSE_FLAG_ERROR_BIT_MASK) {
        goto error;
    }

    memset(&gpd[0], 0, (sizeof(gpd_t) * 2));
    memset(&bd, 0, sizeof(bd_t));

    /* Config GPD & BD descriptors */
    gpd[0].intr     = 0;   /* donnot generate interrupt */
    gpd[0].extlen   = 0;   /* ignore cmd,arg,etc. */
    gpd[0].hwo      = 1;   /* config hardward owner */
    gpd[0].bdp      = 1;   /* use buffer descriptor list */
    gpd[0].ptr      = (void *)(&bd); /* physical addr */
    gpd[0].next     = NULL;
    gpd[0].chksum   = msdc_cal_checksum(&gpd[0], 16);

    bd.eol     = 1;
    bd.next    = NULL;
    bd.ptr     = (void *)command53_config->buffer;
    bd.buflen   = command53_config->block ? (command53_config->count * block_size) : command53_config->count;
    bd.blkpad   = 0;
    bd.dwpad    = 0;
    bd.chksum    = msdc_cal_checksum(&bd, 16);

    /* enable Descriptor's checksum */
    MSDC_REG->DMA_CFG |= MSDC_DMA_CFG_DECSEN;

    /* descriptor mode and burst size=64Bytes */
    MSDC_REG->DMA_CTRL = (MSDC_REG->DMA_CTRL & (~MSDC_DMA_CTRL_BRUSTSZ)) | (MSDC_DMA_BURST_64_BYTES << 12);
    MSDC_REG->DMA_CTRL |= MSDC_DMA_CTRL_MODE;

    /* write DMA start address to GPD's physical address */
    MSDC_REG->DMA_SA = (uint32_t)(&gpd[0]);
#ifdef MSDC_USE_INT
    MSDC_REG->MSDC_INTEN |= MSDC_DAT_INTS_EN | MSDC_INTEN_DXFER_DONE;
#endif

    MSDC_REG->DMA_CTRL |= MSDC_DMA_CTRL_START;

    status = sdio_wait_data_ready();
    if (status != NO_ERROR) {
        msdc_dump_debug_reg();
        goto error;
    }

    msdc_dma_stop();
    msdc_reset();

    msdc_nvic_set(true);
    msdc_reset();

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_OK;

error:
    *(volatile uint32_t *)0xA2010060 = *(volatile uint32_t *)0xA2010060 | 0x8000;

    log_hal_error("sdio error status = %d \r\n", status);

    if (command53_config->block) {
        sdio_command12_stop();
    }

    sdio_stop();
    msdc_reset();
    msdc_nvic_set(true);

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_ERROR;
}

hal_sdio_status_t hal_sdio_set_block_size(hal_sdio_port_t sdio_port, hal_sdio_function_id_t function, uint32_t block_size)
{
    sdio_internal_status_t status;
    hal_sdio_command52_config_t command52;
    uint32_t irq_status;

    irq_status = save_and_set_interrupt_mask();
    if (sdio_information.is_busy) {
        restore_interrupt_mask(irq_status);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    restore_interrupt_mask(irq_status);

    command52.direction = HAL_SDIO_DIRECTION_WRITE;
    command52.function = HAL_SDIO_FUNCTION_0;
    command52.read_after_write = false;
    command52.stop = false;
    command52.address = 0x10 + function * 0x100;
    command52.data = block_size & 0xff;
    status = sdio_command52(&command52);
    if (status != NO_ERROR) {
        goto error;
    }

    if (block_size >= 256) {
        command52.address = 0x11 + function * 0x100;
        command52.data = (block_size & 0xff00) >> 8;
        status = sdio_command52(&command52);
        if (status != NO_ERROR) {
            goto error;
        }
    }

    sdio_information.block_size[function] = block_size;

    sdio_information.is_busy = false;
    return HAL_SDIO_STATUS_OK;
error:
    sdio_information.is_busy = false;

    log_hal_error("sdio error status = %d \r\n", status);

    return HAL_SDIO_STATUS_ERROR;
}


hal_sdio_status_t hal_sdio_get_block_size(hal_sdio_port_t sdio_port, hal_sdio_function_id_t function, uint32_t *block_size)
{
    sdio_internal_status_t status;
    hal_sdio_command52_config_t command52;
    uint32_t temp = 0;
    uint32_t temp1 = 0;


    if (NULL == block_size) {
        log_hal_error("parameter error");
        return HAL_SDIO_STATUS_ERROR;
    }

    command52.direction = HAL_SDIO_DIRECTION_READ;
    command52.function = HAL_SDIO_FUNCTION_0;
    command52.read_after_write = false;
    command52.stop = false;
    command52.address = 0x10 + function * 0x100;
    status = sdio_command52(&command52);
    if (status != NO_ERROR) {
        goto error;
    }

    temp = command52.data;

    command52.address = 0x11 + function * 0x100;
    status = sdio_command52(&command52);
    if (status != NO_ERROR) {
        goto error;
    }
    temp1 = command52.data;
    temp = temp | (temp1 << 8);

    if (temp == sdio_information.block_size[function]) {
        *block_size = temp;
    } else {
        status = ERROR_INVALID_BLOCK_LENGTH;
        goto error;
    }

    return HAL_SDIO_STATUS_OK;
error:

    log_hal_error("sdio error status = %d \r\n", status);

    return HAL_SDIO_STATUS_ERROR;
}

hal_sdio_status_t hal_sdio_set_clock(hal_sdio_port_t sdio_port, uint32_t clock)
{
    uint32_t irq_status;

    irq_status = save_and_set_interrupt_mask();
    if (sdio_information.is_busy) {
        restore_interrupt_mask(irq_status);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    restore_interrupt_mask(irq_status);

    msdc_set_output_clock(clock);

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_OK;
}


hal_sdio_status_t hal_sdio_get_clock(hal_sdio_port_t sdio_port, uint32_t *clock)
{
    if (NULL == clock) {
        log_hal_error("parameter error");
        return HAL_SDIO_STATUS_ERROR;
    }

    *clock = msdc_get_output_clock();

    return HAL_SDIO_STATUS_OK;
}

#endif /*HAL_SDIO_MODULE_ENABLED*/

