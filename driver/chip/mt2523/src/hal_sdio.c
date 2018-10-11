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

#include "hal_msdc.h"
#include "hal_sdio.h"
#include "hal_sdio_define.h"

#ifdef HAL_SDIO_MODULE_ENABLED
#include "hal_log.h"
#include "hal_dvfs_internal.h"
#include "assert.h"


#ifdef HAL_DVFS_MODULE_ENABLED
dvfs_lock_t sdio_dvfs_lock = {
    .domain = "VCORE",
    .addressee = "MSDC_DVFS",
};
#endif

extern volatile sdio_information_t sdio_information;
extern msdc_config_t msdc_config[];

extern void restore_interrupt_mask(uint32_t mask);
extern uint32_t save_and_set_interrupt_mask(void);
hal_sdio_status_t hal_sdio_register_dma_interrupt_callback(hal_sdio_port_t sdio_port, hal_sdio_callback_t sdio_callback,void *user_data)
{
    msdc_config[sdio_port].msdc_sdio_dma_interrupt_callback = sdio_callback;
    return HAL_SDIO_STATUS_OK;
}

hal_sdio_status_t hal_sdio_set_bus_width(hal_sdio_port_t sdio_port, hal_sdio_bus_width_t bus_width)
{
    sdio_internal_status_t status;
    hal_sdio_command52_config_t command52;
    msdc_register_t *msdc_register_base;

    msdc_register_base = (HAL_SDIO_PORT_0 == sdio_port) ? MSDC0_REG : MSDC1_REG;

    command52.direction = HAL_SDIO_DIRECTION_READ;
    command52.function = HAL_SDIO_FUNCTION_0;
    command52.read_after_write = false;
    command52.stop = false;
    command52.address = 0x07;

    status = sdio_command52(sdio_port, &command52);
    if (status != NO_ERROR) {
        goto error;
    }

    command52.data = command52.data & (~(SDIO_CCCR_BUS0_BIT_MASK | SDIO_CCCR_BUS1_BIT_MASK));
    /*set bus width and disable card detection*/
    command52.data =  command52.data | SDIO_CCCR_CD_BIT_MASK |
                      ((bus_width == HAL_SDIO_BUS_WIDTH_4) ? SDIO_CCCR_BUS1_BIT_MASK : 0);

    command52.direction = HAL_SDIO_DIRECTION_WRITE;
    status = sdio_command52(sdio_port, &command52);
    if (status != NO_ERROR) {
        goto error;
    }

    if (bus_width == HAL_SDIO_BUS_WIDTH_4) {
        msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG | SDC_CFG_MDLEN_MASK;
    } else {
        msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG & (~SDC_CFG_MDLEN_MASK);
    }

    sdio_information.bus_width = bus_width;

    return HAL_SDIO_STATUS_OK;
error:
    sdio_record_error_status(status);

    log_hal_error("sdio error status = %d \r\n", status);

    return HAL_SDIO_STATUS_ERROR;
}


hal_sdio_status_t hal_sdio_init(hal_sdio_port_t sdio_port, hal_sdio_config_t *sdio_config)
{
    sdio_internal_status_t  status;
    uint32_t retry = 0;
    msdc_register_t *msdc_register_base;
    bool is_io_ready = false;
    uint32_t irq_status;

    if (NULL == sdio_config) {
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

    sdio_information.is_initialized = false;

    msdc_register_base = (HAL_SDIO_PORT_0 == sdio_port) ? MSDC0_REG : MSDC1_REG;

    /*cofirm whether MSDC have used by SD*/
    if (MSDC_INITIALIZED == msdc_init((msdc_port_t)sdio_port)) {
        if (MSDC_OWNER_SD == msdc_get_owner((msdc_port_t)sdio_port)) {
            sdio_information.is_busy = false;
            return HAL_SDIO_STATUS_ERROR;
        }
    }

#ifdef HAL_DVFS_MODULE_ENABLED
    /*lock dvfs befor dma start transfer.*/
    dvfs_lock(&sdio_dvfs_lock);
#endif

    /*save MSDC owner*/
    msdc_set_owner((msdc_port_t)sdio_port, MSDC_OWNER_SDIO);

    msdc_reset((msdc_port_t)sdio_port);

    /*disable 4bit*/
    msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG & (~SDC_CFG_MDLEN_MASK);

    msdc_clock_init((msdc_port_t)sdio_port);

    /*reset data timeout conter to 65536*256 cycles and write data timeout delay 15 clock cycles*/
    msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG | 0xFFF00000;

    /*enable SDIO mode*/
    msdc_register_base->SDC_CFG |= SDC_CFG_SDIO_MASK;

    /*enable 74 serial clock*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG | MSDC_CFG_SCLKON_MASK;

    /*enable SDIO interrupt*/
    msdc_register_base->SDIO_CFG |= SDIO_CFG_INTEN_MASK;

    msdc_wait(15);

    /*disable serial clock*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG & (~MSDC_CFG_SCLKON_MASK);


    status = sdio_send_command5(sdio_port, COMMAND_NO_ARGUMENT);
    if (status != NO_ERROR) {
        goto error;
    }

    do {
        retry ++;

        status = sdio_send_command5(sdio_port, sdio_information.ocr);

        if (100 < retry) {
            goto error;
        }

    } while (!sdio_information.is_io_ready);

    msdc_set_output_clock((msdc_port_t)sdio_port, MSDC_OUTPUT_CLOCK);

    status = sdio_get_rca(sdio_port);
    if (status != NO_ERROR) {
        goto error;
    }

    status = sdio_select_card(sdio_port, sdio_information.rca);
    if (status != NO_ERROR) {
        goto error;
    }

    status = sdio_get_cccr(sdio_port);
    if (status != NO_ERROR) {
        goto error;
    }

    sdio_get_capacity(sdio_port);

    sdio_get_power_control(sdio_port);

    if (!(sdio_is_support_lsc(sdio_port))) {
        if (sdio_is_support_s4mi(sdio_port)) {
            status = sdio_set_e4mi(sdio_port, true);
            if (status != NO_ERROR) {
                goto error;
            }
        }

        if (HAL_SDIO_STATUS_OK != hal_sdio_set_bus_width(sdio_port, sdio_config->bus_width)) {
            msdc_deinit((msdc_port_t)sdio_port);
#ifdef HAL_DVFS_MODULE_ENABLED
            /*unlock dvfs.*/
            dvfs_unlock(&sdio_dvfs_lock);
#endif
            return HAL_SDIO_STATUS_ERROR;
        }

    } else if (sdio_is_support_4bls(sdio_port) && (sdio_config->bus_width == HAL_SDIO_BUS_WIDTH_4)) {
        if (HAL_SDIO_STATUS_OK != hal_sdio_set_bus_width(sdio_port, sdio_config->bus_width)) {
            msdc_deinit((msdc_port_t)sdio_port);
#ifdef HAL_DVFS_MODULE_ENABLED
            /*unlock dvfs.*/
            dvfs_unlock(&sdio_dvfs_lock);
#endif
            return HAL_SDIO_STATUS_ERROR;
        }
    }

    status =  sdio_get_cis(sdio_port, HAL_SDIO_FUNCTION_0);
    if (status < NO_ERROR) {
        goto error;
    }

    for (retry = 0; retry < 100; retry++) {
        status = sdio_set_io(sdio_port, HAL_SDIO_FUNCTION_1, true);
        if (status < NO_ERROR) {
            goto error;
        }

        status = sdio_check_io_ready(sdio_port, HAL_SDIO_FUNCTION_1, &is_io_ready);
        if (status < NO_ERROR) {
            goto error;
        }

        if (is_io_ready) {
            break;
        }
    }

    status = sdio_get_fbr(sdio_port, HAL_SDIO_FUNCTION_1);
    if (status < NO_ERROR) {
        goto error;
    }

    if (sdio_is_support_mps(sdio_port)) {
        status = sdio_set_mps(sdio_port, true);
        if (status < NO_ERROR) {
            goto error;
        }
    }

    if (sdio_config->clock >= SDIO_DEFAULT_MAX_SPEED) {
        sdio_get_high_speed(sdio_port);
        if (sdio_is_support_shs(sdio_port)) {
            status = sdio_set_ehs(sdio_port, true);
            if (status < NO_ERROR) {
                goto error;
            }
            msdc_set_output_clock((msdc_port_t)sdio_port, sdio_config->clock);
        } else {
            msdc_set_output_clock((msdc_port_t)sdio_port, SDIO_DEFAULT_MAX_SPEED);
        }
    } else {
        msdc_set_output_clock((msdc_port_t)sdio_port, sdio_config->clock);
    }

    status = sdio_get_cccr(sdio_port);
    if (status != NO_ERROR) {
        goto error;
    }

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs after transfer done.*/
    dvfs_unlock(&sdio_dvfs_lock);
#endif

    sdio_information.is_initialized = true ;
    sdio_information.is_busy = false;
    return HAL_SDIO_STATUS_OK;

error:
    sdio_record_error_status(status);
    if (status != NO_ERROR) {
        sdio_information.is_initialized = false;
    }
    sdio_information.is_busy = false;

    log_hal_error("sdio error status = %d \r\n", status);
    msdc_deinit((msdc_port_t)sdio_port);

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs.*/
    dvfs_unlock(&sdio_dvfs_lock);
#endif

    return HAL_SDIO_STATUS_ERROR;
}


hal_sdio_status_t hal_sdio_deinit(hal_sdio_port_t sdio_port)
{
    msdc_reset((msdc_port_t)sdio_port);
    memset((void *)(&sdio_information), 0, sizeof(sdio_information_t));

    msdc_deinit((msdc_port_t)sdio_port);

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

    irq_status = save_and_set_interrupt_mask();
    if (sdio_information.is_busy) {
        restore_interrupt_mask(irq_status);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    restore_interrupt_mask(irq_status);

#ifdef HAL_DVFS_MODULE_ENABLED
    /*lock dvfs befor start transfer.*/
    dvfs_lock(&sdio_dvfs_lock);
#endif

    status = sdio_command52(sdio_port, command52_config);

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs after transfer done.*/
    dvfs_unlock(&sdio_dvfs_lock);
#endif

    if (NO_ERROR != status) {
        sdio_information.is_busy = false;

        log_hal_error("sdio error status = %d \r\n", status);
        return HAL_SDIO_STATUS_ERROR;
    }

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_OK;
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
    uint32_t index = 0;
    uint32_t *buffer_pointer;
    msdc_register_t *msdc_register_base;
    uint32_t irq_status;
    uint32_t block_size;

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

    msdc_register_base = (HAL_SDIO_PORT_0 == sdio_port) ? MSDC0_REG : MSDC1_REG;
    msdc_nvic_set((msdc_port_t)sdio_port, false);

    msdc_reset((msdc_port_t)sdio_port);

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
    command = command53_config->direction ? (command | COMMAND_WRITE) : (command & (~COMMAND_WRITE));
    if (command53_config->block) {
        command = command | COMMAND_MULTIPLE_BLOCK_TRANSACTION;
        msdc_register_base->SDC_CFG = (msdc_register_base->SDC_CFG & (~SDC_CFG_BLKEN_MASK)) | block_size;
    } else {
        command = command | COMMAND_SINGLE_BLOCK_TRANSACTION;
        msdc_register_base->SDC_CFG = (msdc_register_base->SDC_CFG & (~SDC_CFG_BLKEN_MASK)) | command53_config->count;
    }

#ifdef HAL_DVFS_MODULE_ENABLED
    /*lock dvfs befor start transfer.*/
    dvfs_lock(&sdio_dvfs_lock);
#endif

    status =  sdio_send_command(sdio_port, command, argument);
    if (status != NO_ERROR) {
        goto error;
    }

    response = msdc_register_base->SDC_RESP0;

    sdio_information.response = (response & SDIO_R5_RESPONSE_FLAG_BIT_MASK) >> SDIO_R5_RESPONSE_FLAG_BIT_SHIFT;

    if (sdio_information.response & SDIO_R5_RESPONSE_FLAG_ERROR_BIT_MASK) {
        goto error;
    }

    buffer_pointer = (uint32_t *)command53_config->buffer;
    /*set fifo threshold*/
    msdc_register_base->MSDC_CFG = (msdc_register_base->MSDC_CFG & (~MSDC_CFG_FIFOTHD_MASK)) |
                                   (MSDC_FIFO_THRESHOLD_1 << MSDC_CFG_FIFOTHD_OFFSET);
    /*set MSDC burst type*/
    msdc_register_base->MSDC_IOCON = msdc_register_base->MSDC_IOCON & (~MSDC_IOCON_DMABURST_MASK);

    if (command53_config->direction) {
        /*write data*/
        if (command53_config->block) {
            word_count = (uint32_t)(command53_config->count * sdio_information.block_size[command53_config->function] + 3) >> 2;
            while (index < word_count) {
                if (msdc_register_base->MSDC_STA & MSDC_STA_DRQ_MASK) {
                    msdc_register_base->MSDC_DAT = *(buffer_pointer + index);
                    index++;
                }
            }
        } else {
            word_count = (uint32_t)(command53_config->count + 3) >> 2;
            while (index < word_count) {
                if (msdc_register_base->MSDC_STA & MSDC_STA_DRQ_MASK) {
                    msdc_register_base->MSDC_DAT = *(buffer_pointer + index);
                    index++;
                }
            }
        }
    } else {
        /*read data*/
        if (command53_config->block) {
            word_count = (uint32_t)(command53_config->count * sdio_information.block_size[command53_config->function] + 3) >> 2;
            while (index < word_count) {
                if (msdc_register_base->MSDC_STA & MSDC_STA_DRQ_MASK) {
                    *(buffer_pointer + index) = msdc_register_base->MSDC_DAT ;
                    index++;
                }
            }
        } else {
            word_count = (uint32_t)(command53_config->count + 3) >> 2;
            while (index < word_count) {
                if (msdc_register_base->MSDC_STA & MSDC_STA_DRQ_MASK) {
                    *(buffer_pointer + index) = msdc_register_base->MSDC_DAT ;
                    index++;
                }
            }
        }
    }

    status = sdio_wait_data_ready(sdio_port);
    if (status != NO_ERROR) {
        goto error;
    }

    /*for mutil block write, this API must be called for check data write to card done*/
    if (command53_config->direction &&
            command53_config->block  &&
            (command53_config->count > 1)) {

        status = sdio_wait_last_block_complete(sdio_port);
        if (status != NO_ERROR) {
            goto error;
        }
    }

    msdc_nvic_set((msdc_port_t)sdio_port, true);
    msdc_reset((msdc_port_t)sdio_port);

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs after transfer done.*/
    dvfs_unlock(&sdio_dvfs_lock);
#endif

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_OK;

error:
    sdio_record_error_status(status);
    if (command53_config->block) {
        sdio_command12_stop(sdio_port);
    }
    sdio_stop(sdio_port);
    msdc_register_base->SDC_CMDSTA;
    msdc_reset((msdc_port_t)sdio_port);
    msdc_nvic_set((msdc_port_t)sdio_port, true);

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs.*/
    dvfs_unlock(&sdio_dvfs_lock);
#endif

    sdio_information.is_busy = false;

    log_hal_error("sdio error status = %d \r\n", status);

    return HAL_SDIO_STATUS_ERROR;
}



hal_sdio_status_t hal_sdio_execute_command53_dma_blocking(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config)
{
    sdio_internal_status_t status;
    uint32_t argument = 0;
    uint32_t command;
    uint32_t response;
    uint32_t *buffer_pointer;
    msdc_register_t *msdc_register_base;
    uint32_t irq_status;
    pdma_channel_t dma_channel;
    uint32_t block_size;

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

    dma_channel = (sdio_port == HAL_SDIO_PORT_0) ? PDMA_MSDC1 : PDMA_MSDC2;

    msdc_register_base = (HAL_SDIO_PORT_0 == sdio_port) ? MSDC0_REG : MSDC1_REG;
    msdc_nvic_set((msdc_port_t)sdio_port, false);

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
    command = command53_config->direction ? (command | COMMAND_WRITE) : (command & (~COMMAND_WRITE));
    if (command53_config->block) {
        command = command | COMMAND_MULTIPLE_BLOCK_TRANSACTION;
        msdc_register_base->SDC_CFG = (msdc_register_base->SDC_CFG & (~SDC_CFG_BLKEN_MASK)) | block_size;
    } else {
        command = command | COMMAND_SINGLE_BLOCK_TRANSACTION;
        msdc_register_base->SDC_CFG = (msdc_register_base->SDC_CFG & (~SDC_CFG_BLKEN_MASK)) | command53_config->count;
    }

    /*enable DMA*/
    msdc_dma_enable((msdc_port_t)sdio_port);
    msdc_dma_init((msdc_port_t)sdio_port);

    buffer_pointer = (uint32_t *)command53_config->buffer;

#ifdef HAL_DVFS_MODULE_ENABLED
    /*lock dvfs befor dma start transfer.*/
    dvfs_lock(&sdio_dvfs_lock);
#endif

    if (command53_config->block) {
        msdc_dma_config((msdc_port_t)sdio_port, (uint32_t)buffer_pointer, sdio_information.block_size[command53_config->function] * command53_config->count, command53_config->direction);
    } else {
        msdc_dma_config((msdc_port_t)sdio_port, (uint32_t)buffer_pointer, command53_config->count, command53_config->direction);
    }
    status =  sdio_send_command(sdio_port, command, argument);
    if (status != NO_ERROR) {
        goto error;
    }

    response = msdc_register_base->SDC_RESP0;

    sdio_information.response = (response & SDIO_R5_RESPONSE_FLAG_BIT_MASK) >> SDIO_R5_RESPONSE_FLAG_BIT_SHIFT;

    if (sdio_information.response & SDIO_R5_RESPONSE_FLAG_ERROR_BIT_MASK) {
        goto error;
    }

    msdc_dma_wait_transfer_complete((msdc_port_t)sdio_port,(uint32_t)buffer_pointer);

    status = sdio_wait_data_ready(sdio_port);
    if (status != NO_ERROR) {
        goto error;
    }

    /*for mutil block write, this API must be called for check data write to card done*/
    if (command53_config->direction &&
            command53_config->block  &&
            (command53_config->count > 1)) {
        status = sdio_wait_last_block_complete(sdio_port);
        if (status != NO_ERROR) {
            goto error;
        }
    }

    msdc_dma_disable((msdc_port_t)sdio_port);
    msdc_reset((msdc_port_t)sdio_port);
    pdma_stop(dma_channel);
    pdma_deinit(dma_channel);

    msdc_nvic_set((msdc_port_t)sdio_port, true);
    msdc_reset((msdc_port_t)sdio_port);

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs after dma transfer done.*/
    dvfs_unlock(&sdio_dvfs_lock);
#endif

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_OK;

error:
    sdio_record_error_status(status);
    if (command53_config->block) {
        sdio_command12_stop(sdio_port);
    }
    sdio_stop(sdio_port);
    pdma_stop(dma_channel);
    pdma_deinit(dma_channel);
    msdc_register_base->SDC_CMDSTA;
    msdc_reset((msdc_port_t)sdio_port);
    msdc_nvic_set((msdc_port_t)sdio_port, true);

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs.*/
    dvfs_unlock(&sdio_dvfs_lock);
#endif

    sdio_information.is_busy = false;

    log_hal_error("sdio error status = %d \r\n", status);

    return HAL_SDIO_STATUS_ERROR;
}

hal_sdio_status_t hal_sdio_execute_command53_dma(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config)
{
    sdio_internal_status_t status;
    uint32_t argument = 0;
    uint32_t command;
    uint32_t response;
    uint32_t *buffer_pointer;
    msdc_register_t *msdc_register_base;
    uint32_t irq_status;
    pdma_channel_t dma_channel;
    uint32_t block_size;
    sdio_dma_interrupt_context_t context;

    if (!(msdc_config[sdio_port].msdc_sdio_dma_interrupt_callback)) {
        log_hal_error("msdc_sdio_dma_interrupt_callback is null!\r\n");
        return HAL_SDIO_STATUS_ERROR;
    }

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

    dma_channel = (sdio_port == HAL_SDIO_PORT_0) ? PDMA_MSDC1 : PDMA_MSDC2;

    msdc_register_base = (HAL_SDIO_PORT_0 == sdio_port) ? MSDC0_REG : MSDC1_REG;
    msdc_nvic_set((msdc_port_t)sdio_port, false);

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
    command = command53_config->direction ? (command | COMMAND_WRITE) : (command & (~COMMAND_WRITE));
    if (command53_config->block) {
        command = command | COMMAND_MULTIPLE_BLOCK_TRANSACTION;
        msdc_register_base->SDC_CFG = (msdc_register_base->SDC_CFG & (~SDC_CFG_BLKEN_MASK)) | block_size;
    } else {
        command = command | COMMAND_SINGLE_BLOCK_TRANSACTION;
        msdc_register_base->SDC_CFG = (msdc_register_base->SDC_CFG & (~SDC_CFG_BLKEN_MASK)) | command53_config->count;
    }

    /*enable DMA*/
    msdc_dma_enable((msdc_port_t)sdio_port);
    msdc_dma_init((msdc_port_t)sdio_port);

    buffer_pointer = (uint32_t *)command53_config->buffer;

#ifdef HAL_DVFS_MODULE_ENABLED
    /*lock dvfs befor dma start transfer.*/
    dvfs_lock(&sdio_dvfs_lock);
#endif

    if (command53_config->block) {
        msdc_dma_config((msdc_port_t)sdio_port, (uint32_t)buffer_pointer, sdio_information.block_size[command53_config->function] * command53_config->count, command53_config->direction);
    } else {
        msdc_dma_config((msdc_port_t)sdio_port, (uint32_t)buffer_pointer, command53_config->count, command53_config->direction);
    }
    msdc_set_dma_interrupt((msdc_port_t)sdio_port, (uint32_t)buffer_pointer);
    status =  sdio_send_command(sdio_port, command, argument);
    if (status != NO_ERROR) {
        goto error;
    }

    response = msdc_register_base->SDC_RESP0;

    sdio_information.response = (response & SDIO_R5_RESPONSE_FLAG_BIT_MASK) >> SDIO_R5_RESPONSE_FLAG_BIT_SHIFT;

    if (sdio_information.response & SDIO_R5_RESPONSE_FLAG_ERROR_BIT_MASK) {
        goto error;
    }

    /*save the dma interrupt context*/
    context.sdio_command53_config = *command53_config;
    #ifdef HAL_DVFS_MODULE_ENABLED
        context.sd_dvfs_lock = sdio_dvfs_lock;
    #endif
    sdio_save_dma_interrupt_context(sdio_port,&context);

    return HAL_SDIO_STATUS_OK;

error:
    sdio_record_error_status(status);
    if (command53_config->block) {
        sdio_command12_stop(sdio_port);
    }
    sdio_stop(sdio_port);
    pdma_stop(dma_channel);
    pdma_deinit(dma_channel);
    msdc_register_base->SDC_CMDSTA;
    msdc_reset((msdc_port_t)sdio_port);
    msdc_nvic_set((msdc_port_t)sdio_port, true);

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs.*/
    dvfs_unlock(&sdio_dvfs_lock);
#endif

    sdio_information.is_busy = false;

    log_hal_error("sdio error status = %d \r\n", status);

    return HAL_SDIO_STATUS_ERROR;

}

hal_sdio_status_t hal_sdio_set_block_size(hal_sdio_port_t sdio_port, hal_sdio_function_id_t function, uint32_t block_size)
{
    sdio_internal_status_t status;
    hal_sdio_command52_config_t command52;
    msdc_register_t *msdc_register_base;
    uint32_t irq_status;

    irq_status = save_and_set_interrupt_mask();
    if (sdio_information.is_busy) {
        restore_interrupt_mask(irq_status);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    restore_interrupt_mask(irq_status);

    msdc_register_base = (HAL_SDIO_PORT_0 == sdio_port) ? MSDC0_REG : MSDC1_REG;
    command52.direction = HAL_SDIO_DIRECTION_WRITE;
    command52.function = HAL_SDIO_FUNCTION_0;
    command52.read_after_write = false;
    command52.stop = false;
    command52.address = 0x10 + function * 0x100;
    command52.data = block_size & 0xff;
    status = sdio_command52(sdio_port, &command52);
    if (status != NO_ERROR) {
        goto error;
    }

    if (block_size >= 256) {
        command52.address = 0x11 + function * 0x100;
        command52.data = (block_size & 0xff00) >> 8;
        status = sdio_command52(sdio_port, &command52);
        if (status != NO_ERROR) {
            goto error;
        }
    }

    sdio_information.block_size[function] = block_size;
    msdc_register_base->SDC_CFG = (msdc_register_base->SDC_CFG & (~SDC_CFG_BLKEN_MASK)) | (block_size);
    sdio_information.is_busy = false;
    return HAL_SDIO_STATUS_OK;
error:
    sdio_record_error_status(status);
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
    msdc_register_t *msdc_register_base;


    if (NULL == block_size) {
        log_hal_error("parameter error");
        return HAL_SDIO_STATUS_ERROR;
    }

    msdc_register_base = (HAL_SDIO_PORT_0 == sdio_port) ? MSDC0_REG : MSDC1_REG;
    command52.direction = HAL_SDIO_DIRECTION_READ;
    command52.function = HAL_SDIO_FUNCTION_0;
    command52.read_after_write = false;
    command52.stop = false;
    command52.address = 0x10 + function * 0x100;
    status = sdio_command52(sdio_port, &command52);
    if (status != NO_ERROR) {
        goto error;
    }

    temp = command52.data;

    command52.address = 0x11 + function * 0x100;
    status = sdio_command52(sdio_port, &command52);
    if (status != NO_ERROR) {
        goto error;
    }
    temp1 = command52.data;
    temp = temp | (temp1 << 8);

    if (temp == (msdc_register_base->SDC_CFG & SDC_CFG_BLKEN_MASK) &&
            (temp == sdio_information.block_size[function])) {
        *block_size = temp;
    } else {
        status = ERROR_INVALID_BLOCK_LENGTH;
        goto error;
    }

    return HAL_SDIO_STATUS_OK;
error:

    sdio_record_error_status(status);

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

    msdc_set_output_clock((msdc_port_t)sdio_port, clock);

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_OK;
}


hal_sdio_status_t hal_sdio_get_clock(hal_sdio_port_t sdio_port, uint32_t *clock)
{
    if (NULL == clock) {
        log_hal_error("parameter error");
        return HAL_SDIO_STATUS_ERROR;
    }

    *clock = msdc_get_output_clock((msdc_port_t)sdio_port);

    return HAL_SDIO_STATUS_OK;
}

#endif /*HAL_SDIO_MODULE_ENABLED*/

