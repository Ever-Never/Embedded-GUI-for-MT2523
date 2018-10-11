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

#include "hal_spi_master.h"

#ifdef HAL_SPI_MASTER_MODULE_ENABLED

#include "hal_spi_master_internal.h"
#include "hal_log.h"
#include "hal_sleep_manager.h"
//#include "hal_dvfs_internal.h"

SPI_MASTER_REGISTER_T *const spi_master_register[HAL_SPI_MASTER_MAX] = {SPI_MASTER_0, SPI_MASTER_1, SPI_MASTER_2, SPI_MASTER_3};
spi_direction_t g_spi_direction[HAL_SPI_MASTER_MAX] = {SPI_TX};

static uint32_t g_pause_count[HAL_SPI_MASTER_MAX] = {0};
static uint16_t g_packet_length[HAL_SPI_MASTER_MAX] = {0};
static uint16_t g_packet_count[HAL_SPI_MASTER_MAX]  = {0};
/* enhancement for spim driver to support any size transfer in DMA mode */
static uint32_t g_small_size_pause_count[HAL_SPI_MASTER_MAX] = {0};


/*
#ifdef HAL_DVFS_MODULE_ENABLED
extern dvfs_lock_t spim_dvfs_lock;
#endif
*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
extern uint8_t spim_sleep_handler[HAL_SPI_MASTER_MAX];
#endif
void spi_isr_handler(hal_spi_master_port_t master_port, hal_spi_master_callback_t user_callback, void *user_data)
{
    hal_spi_master_callback_event_t event;
    uint32_t interrupt_status = 0;

    interrupt_status = spi_master_register[master_port]->SPI_STATUS0;

    if (interrupt_status & SPI_STATUS0_FINISH_MASK) {
        switch (g_spi_direction[master_port]) {
            case SPI_TX:
                event = HAL_SPI_MASTER_EVENT_SEND_FINISHED;
                break;
            case SPI_RX:
                event = HAL_SPI_MASTER_EVENT_RECEIVE_FINISHED;
                break;
            default:
                event = HAL_SPI_MASTER_EVENT_RECEIVE_FINISHED;
                break;
        }
        /* This is just for data corruption check */
        if (NULL != user_callback) {
            user_callback(event, user_data);
        }
#ifdef HAL_SLEEP_MANAGER_ENABLED
        hal_sleep_manager_unlock_sleep(spim_sleep_handler[master_port]);
#endif
        /*
        #ifdef HAL_DVFS_MODULE_ENABLED
                dvfs_unlock(&spim_dvfs_lock);
        #endif
        */
    } else if (interrupt_status & SPI_STATUS0_PAUSE_MASK) {
        spi_start_transfer(master_port, SPI_MODE_DMA);
    }
}

static void spi_master_pad_macro_select(hal_spi_master_port_t master_port)
{
    switch (master_port) {
        case HAL_SPI_MASTER_0:
            if (HAL_GPIO_5_MA_SPI0_B_SCK == ((*(volatile uint32_t *)SPI0_CLK_PAD_B_GPIO5_MODE_ADDRESS & SPI0_CLK_PAD_B_GPIO5_MODE_MASK) >> SPI0_CLK_PAD_B_GPIO5_MODE_SHIFT)) {
                spi_master_register[master_port]->SPI_PAD_MACRO_SEL = (1 << SPI_PAD_MACRO_SEL_OFFSET);
            }
            break;
        case HAL_SPI_MASTER_1:
            if (HAL_GPIO_12_MA_SPI1_B_SCK == ((*(volatile uint32_t *)SPI1_CLK_PAD_B_GPIO12_MODE_ADDRESS & SPI1_CLK_PAD_B_GPIO12_MODE_MASK) >> SPI1_CLK_PAD_B_GPIO12_MODE_SHIFT)) {
                spi_master_register[master_port]->SPI_PAD_MACRO_SEL = (2 << SPI_PAD_MACRO_SEL_OFFSET);
            }
            break;
        case HAL_SPI_MASTER_2:
            if (HAL_GPIO_40_MA_SPI2_B_SCK == ((*(volatile uint32_t *)SPI2_CLK_PAD_B_GPIO40_MODE_ADDRESS & SPI2_CLK_PAD_B_GPIO40_MODE_MASK) >> SPI2_CLK_PAD_B_GPIO40_MODE_SHIFT)) {
                spi_master_register[master_port]->SPI_PAD_MACRO_SEL = (2 << SPI_PAD_MACRO_SEL_OFFSET);
            }
            break;
        case HAL_SPI_MASTER_3:
            if (HAL_GPIO_33_MA_SPI3_B_SCK == ((*(volatile uint32_t *)SPI3_CLK_PAD_B_GPIO33_MODE_ADDRESS & SPI3_CLK_PAD_B_GPIO33_MODE_MASK) >> SPI3_CLK_PAD_B_GPIO33_MODE_SHIFT)) {
                spi_master_register[master_port]->SPI_PAD_MACRO_SEL = (1 << SPI_PAD_MACRO_SEL_OFFSET);
            }
            break;
        default:
            break;
    }
}

void spi_master_init(hal_spi_master_port_t master_port, const hal_spi_master_config_t *spi_config)
{
    uint32_t sck_count;

    /* set default register value here */
    spi_master_register[master_port]->SPI_CFG0 = SPI_DEFAULT_VALUE;
    spi_master_register[master_port]->SPI_CFG1 = SPI_DEFAULT_VALUE;
    spi_master_register[master_port]->SPI_CMD  = SPI_DEFAULT_VALUE;

    sck_count = ((CLOCK_SOURCE_DIVIDE_2 / (spi_config->clock_frequency)) - 1);
    spi_master_register[master_port]->SPI_CFG2 = ((sck_count << SPI_CFG2_SCK_LOW_COUNT_OFFSET) | sck_count);

    switch (spi_config->slave_port) {
        case  HAL_SPI_MASTER_SLAVE_0:
            spi_master_register[master_port]->SPI_CFG1 &= (~SPI_CFG1_DEVICE_SEL_MASK);
            break;
        case  HAL_SPI_MASTER_SLAVE_1:
            spi_master_register[master_port]->SPI_CFG1 |= SPI_CFG1_DEVICE_SEL_MASK;
            break;
        default:
            break;
    }

    switch (spi_config->bit_order) {
        case HAL_SPI_MASTER_LSB_FIRST:
            spi_master_register[master_port]->SPI_CMD &= (~(SPI_CMD_TXMSBF_MASK | SPI_CMD_RXMSBF_MASK));
            break;
        case HAL_SPI_MASTER_MSB_FIRST:
            spi_master_register[master_port]->SPI_CMD |= (SPI_CMD_TXMSBF_MASK | SPI_CMD_RXMSBF_MASK);
            break;
    }

    switch (spi_config->polarity) {
        case HAL_SPI_MASTER_CLOCK_POLARITY0:
            spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_CPOL_MASK);
            break;
        case HAL_SPI_MASTER_CLOCK_POLARITY1:
            spi_master_register[master_port]->SPI_CMD |= (SPI_CMD_CPOL_MASK);
            break;
    }

    switch (spi_config->phase) {
        case HAL_SPI_MASTER_CLOCK_PHASE0:
            spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_CPHA_MASK);
            break;
        case HAL_SPI_MASTER_CLOCK_PHASE1:
            spi_master_register[master_port]->SPI_CMD |= (SPI_CMD_CPHA_MASK);
            break;
    }

    /* default use non-paused mode*/
    spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_PAUSE_EN_MASK);

    /* Set pad macro value if PINs of SPIx isn't the default group. */
    spi_master_pad_macro_select(master_port);
}

#ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG
void spi_master_set_advanced_config(hal_spi_master_port_t master_port, const hal_spi_master_advanced_config_t *advanced_config)
{
    switch (advanced_config->byte_order) {
        case HAL_SPI_MASTER_LITTLE_ENDIAN:
            spi_master_register[master_port]->SPI_CMD &= (~(SPI_CMD_TX_ENDIAN_MASK | SPI_CMD_RX_ENDIAN_MASK));
            break;
        case HAL_SPI_MASTER_BIG_ENDIAN:
            spi_master_register[master_port]->SPI_CMD |= (SPI_CMD_TX_ENDIAN_MASK | SPI_CMD_RX_ENDIAN_MASK);
            break;
    }

    switch (advanced_config->chip_polarity) {
        case HAL_SPI_MASTER_CHIP_SELECT_LOW:
            spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_CS_POL_MASK);
            break;
        case HAL_SPI_MASTER_CHIP_SELECT_HIGH:
            spi_master_register[master_port]->SPI_CMD |= SPI_CMD_CS_POL_MASK;
            break;
    }

    /* clear former get_tick setting */
    spi_master_register[master_port]->SPI_CFG1 &= (uint32_t)(~SPI_CFG1_GET_TICK_DLY_MASK);
    spi_master_register[master_port]->SPI_CFG1 |= (uint32_t)((advanced_config->get_tick) << SPI_CFG1_GET_TICK_DLY_OFFSET);

    switch (advanced_config->sample_select) {
        case HAL_SPI_MASTER_SAMPLE_POSITIVE:
            spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_SAMPLE_SEL_MASK);
            break;
        case HAL_SPI_MASTER_SAMPLE_NEGATIVE:
            spi_master_register[master_port]->SPI_CMD |= SPI_CMD_SAMPLE_SEL_MASK;
            break;
    }

}
#endif

uint32_t spi_get_status(hal_spi_master_port_t master_port)
{
    volatile uint32_t status;

    status = (spi_master_register[master_port]->SPI_STATUS1);

    return status;
}

void spi_set_rwaddr(hal_spi_master_port_t master_port, spi_direction_t type, const uint8_t *addr)
{
    switch (type) {
        case SPI_TX:
            spi_master_register[master_port]->SPI_TX_SRC = (uint32_t)addr;
            break;
        case SPI_RX:
            spi_master_register[master_port]->SPI_RX_DST = (uint32_t)addr;
            break;
    }
}

hal_spi_master_status_t spi_push_data(hal_spi_master_port_t master_port, const uint8_t *data, uint32_t size)
{
    uint32_t spi_data = 0;
    uint32_t i = 0;
    uint32_t j = 0;
    const uint8_t *temp_addr = data;
    uint8_t temp_data = 0;
    uint32_t quotient = 0;
    uint32_t remainder = 0;
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;

    /* if byte_order setting is big_endian, return error */
    if ((spi_master_register[master_port]->SPI_CMD) & SPI_CMD_TX_ENDIAN_MASK) {
        log_hal_error("[SPIM%d][push_data]:big_endian error.\r\n", master_port);
        return HAL_SPI_MASTER_STATUS_ERROR;
    }

    /* clear and configure packet length and count register */
    /* HW limitation: When using FIFO mode, need to configure transfer size before push data to Tx FIFO */
    spi_master_register[master_port]->SPI_CFG1 &= (~(SPI_CFG1_PACKET_LENGTH_MASK | SPI_CFG1_PACKET_LOOP_COUNT_MASK));
    spi_master_register[master_port]->SPI_CFG1 |= (g_packet_count[master_port] << SPI_CFG1_PACKET_LOOP_COUNT_OFFSET);
    spi_master_register[master_port]->SPI_CFG1 |= ((g_packet_length[master_port] - 1) << SPI_CFG1_PACKET_LENGTH_OFFSET);

    quotient = size / sizeof(uint32_t);
    remainder = size % sizeof(uint32_t);

    for (i = 0; i < quotient; i++) {
        spi_data = 0;
        for (j = 0; j < 4; j++) {
            temp_data = (*temp_addr);
            spi_data |= (temp_data << (8 * j));
            temp_addr++;
        }
        spi_master_register[master_port]->SPI_TX_DATA = spi_data;
    }
    if (remainder > 0) {
        spi_data = 0;
        for (j = 0; j < 4; j++) {
            temp_data = (*temp_addr);
            spi_data |= (temp_data << (8 * j));
            temp_addr++;
        }
        switch (remainder) {
            case 3:
                spi_master_register[master_port]->SPI_TX_DATA = (spi_data & 0x00FFFFFF);
                break;
            case 2:
                spi_master_register[master_port]->SPI_TX_DATA = (spi_data & 0x0000FFFF);
                break;
            case 1:
                spi_master_register[master_port]->SPI_TX_DATA = (spi_data & 0x000000FF);
                break;
        }
    }
    return status;
}

hal_spi_master_status_t spi_pop_data(hal_spi_master_port_t master_port, uint8_t *buffer, uint32_t size)
{
    uint32_t spi_data = 0;
    uint32_t i = 0;
    uint32_t j = 0;
    uint8_t *temp_addr = buffer;
    uint8_t temp_data = 0;
    uint32_t quotient = 0;
    uint32_t remainder = 0;
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;

    /* if byte_order setting is big_endian, return error */
    if ((spi_master_register[master_port]->SPI_CMD) & SPI_CMD_RX_ENDIAN_MASK) {
        log_hal_error("[SPIM%d][pop_data]:big_endian error.\r\n", master_port);
        return HAL_SPI_MASTER_STATUS_ERROR;
    }

    quotient = size / sizeof(uint32_t);
    remainder = size % sizeof(uint32_t);

    for (i = 0; i < quotient; i++) {
        spi_data = spi_master_register[master_port]->SPI_RX_DATA;
        for (j = 0; j < 4; j++) {
            temp_data = ((spi_data & (0xff << (8 * j))) >> (8 * j));
            *temp_addr = temp_data;
            temp_addr++;
        }
    }
    if (remainder > 0) {
        spi_data = spi_master_register[master_port]->SPI_RX_DATA;
        switch (remainder) {
            case 3:
                spi_data &= 0x00FFFFFF;
                break;
            case 2:
                spi_data &= 0x0000FFFF;
                break;
            case 1:
                spi_data &= 0x000000FF;
                break;
        }

        for (j = 0; j < remainder; j++) {
            *temp_addr = (spi_data >> (8 * j));
            temp_addr++;
        }
    }
    return status;
}

void spi_set_interrupt(hal_spi_master_port_t master_port, bool status)
{
    switch (status) {
        case false:
            spi_master_register[master_port]->SPI_CMD &= (~(SPI_CMD_FINISH_IE_MASK | SPI_CMD_PAUSE_IE_MASK));
            break;
        case true:
            spi_master_register[master_port]->SPI_CMD |= (SPI_CMD_FINISH_IE_MASK | SPI_CMD_PAUSE_IE_MASK);
            break;
    }
}

void spi_clear_fifo(hal_spi_master_port_t master_port)
{
    spi_master_register[master_port]->SPI_CMD |= SPI_CMD_RST_MASK;
    spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_RST_MASK);
}

void spi_set_mode(hal_spi_master_port_t master_port, spi_direction_t type, spi_mode_t mode)
{
    if (SPI_TX == type) {
        switch (mode) {
            case SPI_MODE_DMA:
                spi_master_register[master_port]->SPI_CMD |= SPI_CMD_TX_DMA_EN_MASK;
                break;
            case SPI_MODE_FIFO:
                spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_TX_DMA_EN_MASK);
        }
    } else {
        switch (mode) {
            case SPI_MODE_DMA:
                spi_master_register[master_port]->SPI_CMD |= SPI_CMD_RX_DMA_EN_MASK;
                break;
            case SPI_MODE_FIFO:
                spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_RX_DMA_EN_MASK);
        }
    }
}


void spi_start_transfer(hal_spi_master_port_t master_port, spi_mode_t mode)
{
    volatile uint32_t status;
    bool pause_mode_status;

    /* check controller's status because the first command should be CMD_ACT, even pause mode will be used */
    if ((spi_master_register[master_port]->SPI_CMD) & SPI_CMD_PAUSE_EN_MASK) {
        pause_mode_status = true;
    } else {
        pause_mode_status = false;
    }

    switch (mode) {
        case SPI_MODE_DMA:
            /* clear and configure packet length and count register */
            spi_master_register[master_port]->SPI_CFG1 &= (~(SPI_CFG1_PACKET_LENGTH_MASK | SPI_CFG1_PACKET_LOOP_COUNT_MASK));
            /* Already in pause mode and g_pause_count[master_port] > 0, using resume command and
               transfer size is equal to SPI_MAX_SIZE_FOR_NON_PAUSE */
            if ((g_pause_count[master_port] > 0) && (pause_mode_status == true)) {
                g_pause_count[master_port]--;
                spi_master_register[master_port]->SPI_CFG1 |= (SPI_CFG1_PACKET_LOOP_COUNT_MASK | SPI_CFG1_PACKET_LENGTH_MASK);
                if ((g_pause_count[master_port] == 0) && (g_small_size_pause_count[master_port] == 0) && (g_packet_length[master_port] == 0)) {
                    spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_PAUSE_EN_MASK);
                }
                spi_master_register[master_port]->SPI_CMD |= SPI_CMD_RESUME_MASK;
            } else if ((g_small_size_pause_count[master_port] > 0) && (pause_mode_status == true)) {
                spi_master_register[master_port]->SPI_CFG1 |= (SPI_CFG1_PACKET_LENGTH_MASK | ((g_small_size_pause_count[master_port] - 1) << SPI_CFG1_PACKET_LOOP_COUNT_OFFSET));
                g_small_size_pause_count[master_port] = 0;
                if (g_packet_length[master_port] == 0) {
                    spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_PAUSE_EN_MASK);
                }
                spi_master_register[master_port]->SPI_CMD |= SPI_CMD_RESUME_MASK;
            } else if (((g_packet_length[master_port] % 4) > 0) && (g_pause_count[master_port] > 0)) {
                /* Workaround for the size not as multiple 4 bytes but also need to use the pause mode */
                g_pause_count[master_port]--;
                spi_master_register[master_port]->SPI_CFG1 |= (SPI_CFG1_PACKET_LOOP_COUNT_MASK | SPI_CFG1_PACKET_LENGTH_MASK);
                if ((g_pause_count[master_port] == 0) && (g_small_size_pause_count[master_port] == 0)) {
                    spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_PAUSE_EN_MASK);
                }
                spi_master_register[master_port]->SPI_CMD |= (SPI_CMD_CMD_ACT_MASK | SPI_CMD_PAUSE_EN_MASK);
            } else if (((g_packet_length[master_port] % 4) > 0) && (g_small_size_pause_count[master_port] > 0)) {
                /* Workaround for the size not as multiple 4 bytes but also need to use the pause mode */
                spi_master_register[master_port]->SPI_CFG1 |= (SPI_CFG1_PACKET_LENGTH_MASK | ((g_small_size_pause_count[master_port] - 1) << SPI_CFG1_PACKET_LOOP_COUNT_OFFSET));
                g_small_size_pause_count[master_port] = 0;
                spi_master_register[master_port]->SPI_CMD |= (SPI_CMD_CMD_ACT_MASK | SPI_CMD_PAUSE_EN_MASK);
            } else if ((g_pause_count[master_port] > 0) || (g_small_size_pause_count[master_port] > 0)) {
                spi_master_register[master_port]->SPI_CFG1 |= (g_packet_count[master_port] << SPI_CFG1_PACKET_LOOP_COUNT_OFFSET);
                spi_master_register[master_port]->SPI_CFG1 |= ((g_packet_length[master_port] - 1) << SPI_CFG1_PACKET_LENGTH_OFFSET);
                g_packet_length[master_port] = 0;
                /* enable spi controller's pause mode*/
                spi_master_register[master_port]->SPI_CMD |= SPI_CMD_PAUSE_EN_MASK;

                spi_master_register[master_port]->SPI_CMD |= SPI_CMD_CMD_ACT_MASK;
            } else {
                spi_master_register[master_port]->SPI_CFG1 |= (g_packet_count[master_port] << SPI_CFG1_PACKET_LOOP_COUNT_OFFSET);
                spi_master_register[master_port]->SPI_CFG1 |= ((g_packet_length[master_port] - 1) << SPI_CFG1_PACKET_LENGTH_OFFSET);
                if (pause_mode_status == true) {
                    spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_PAUSE_EN_MASK);
                    spi_master_register[master_port]->SPI_CMD |= SPI_CMD_RESUME_MASK;
                } else {
                    /* normal mode is used, and transfer size is contained in g_packet_count[master_port] and g_packet_length[master_port] */
                    spi_master_register[master_port]->SPI_CMD |= SPI_CMD_CMD_ACT_MASK;
                }
            }
            break;
        case SPI_MODE_FIFO:
            /* HW limitation: When using FIFO mode, need to configure transfer size before push data to Tx FIFO */
            spi_master_register[master_port]->SPI_CMD |= SPI_CMD_CMD_ACT_MASK;
            do {
                status = spi_master_register[master_port]->SPI_STATUS1;
            } while ((status != SPI_STATUS1_BUSY_MASK));
            /* read clear finish flag to prevent dummy interrupt when call dma API to enable irq */
            status = (spi_master_register[master_port]->SPI_STATUS0);
            status = status;
            break;
    }
}

void spi_start_transfer_dma_blocking(hal_spi_master_port_t master_port)
{
    bool pause_mode_status;
    uint32_t irq_status;

    do {
        /* check controller's status because the first command should be CMD_ACT, even pause mode will be used */
        if ((spi_master_register[master_port]->SPI_CMD) & SPI_CMD_PAUSE_EN_MASK) {
            pause_mode_status = true;
        } else {
            pause_mode_status = false;
        }

        /* clear and configure packet length and count register */
        spi_master_register[master_port]->SPI_CFG1 &= (~(SPI_CFG1_PACKET_LENGTH_MASK | SPI_CFG1_PACKET_LOOP_COUNT_MASK));
        /* Already in pause mode and g_pause_count[master_port] > 0, using resume command and
           transfer size is equal to SPI_MAX_SIZE_FOR_NON_PAUSE */
        if ((g_pause_count[master_port] > 0) && (pause_mode_status == true)) {
            g_pause_count[master_port]--;
            spi_master_register[master_port]->SPI_CFG1 |= (SPI_CFG1_PACKET_LOOP_COUNT_MASK | SPI_CFG1_PACKET_LENGTH_MASK);
            if ((g_pause_count[master_port] == 0) && (g_small_size_pause_count[master_port] == 0) && (g_packet_length[master_port] == 0)) {
                spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_PAUSE_EN_MASK);
            }
            spi_master_register[master_port]->SPI_CMD |= SPI_CMD_RESUME_MASK;
        } else if ((g_small_size_pause_count[master_port] > 0) && (pause_mode_status == true)) {
            spi_master_register[master_port]->SPI_CFG1 |= (SPI_CFG1_PACKET_LENGTH_MASK | ((g_small_size_pause_count[master_port] - 1) << SPI_CFG1_PACKET_LOOP_COUNT_OFFSET));
            g_small_size_pause_count[master_port] = 0;
            if (g_packet_length[master_port] == 0) {
                spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_PAUSE_EN_MASK);
            }
            spi_master_register[master_port]->SPI_CMD |= SPI_CMD_RESUME_MASK;
        } else if (((g_packet_length[master_port] % 4) > 0) && (g_pause_count[master_port] > 0)) {
            /* Workaround for the size not as multiple 4 bytes but also need to use the pause mode */
            g_pause_count[master_port]--;
            spi_master_register[master_port]->SPI_CFG1 |= (SPI_CFG1_PACKET_LOOP_COUNT_MASK | SPI_CFG1_PACKET_LENGTH_MASK);
            if ((g_pause_count[master_port] == 0) && (g_small_size_pause_count[master_port] == 0)) {
                spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_PAUSE_EN_MASK);
            }
            spi_master_register[master_port]->SPI_CMD |= (SPI_CMD_CMD_ACT_MASK | SPI_CMD_PAUSE_EN_MASK);
        } else if (((g_packet_length[master_port] % 4) > 0) && (g_small_size_pause_count[master_port] > 0)) {
            /* Workaround for the size not as multiple 4 bytes but also need to use the pause mode */
            spi_master_register[master_port]->SPI_CFG1 |= (SPI_CFG1_PACKET_LENGTH_MASK | ((g_small_size_pause_count[master_port] - 1) << SPI_CFG1_PACKET_LOOP_COUNT_OFFSET));
            g_small_size_pause_count[master_port] = 0;
            spi_master_register[master_port]->SPI_CMD |= (SPI_CMD_CMD_ACT_MASK | SPI_CMD_PAUSE_EN_MASK);
        } else if ((g_pause_count[master_port] > 0) || (g_small_size_pause_count[master_port] > 0)) {
            spi_master_register[master_port]->SPI_CFG1 |= (g_packet_count[master_port] << SPI_CFG1_PACKET_LOOP_COUNT_OFFSET);
            spi_master_register[master_port]->SPI_CFG1 |= ((g_packet_length[master_port] - 1) << SPI_CFG1_PACKET_LENGTH_OFFSET);
            g_packet_length[master_port] = 0;
            /* enable spi controller's pause mode*/
            spi_master_register[master_port]->SPI_CMD |= SPI_CMD_PAUSE_EN_MASK;
            spi_master_register[master_port]->SPI_CMD |= SPI_CMD_CMD_ACT_MASK;
        } else {
            spi_master_register[master_port]->SPI_CFG1 |= (g_packet_count[master_port] << SPI_CFG1_PACKET_LOOP_COUNT_OFFSET);
            spi_master_register[master_port]->SPI_CFG1 |= ((g_packet_length[master_port] - 1) << SPI_CFG1_PACKET_LENGTH_OFFSET);
            if (pause_mode_status == true) {
                spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_PAUSE_EN_MASK);
                spi_master_register[master_port]->SPI_CMD |= SPI_CMD_RESUME_MASK;
            } else {
                /* normal mode is used, and transfer size is contained in g_packet_count[master_port] and g_packet_length[master_port] */
                spi_master_register[master_port]->SPI_CMD |= SPI_CMD_CMD_ACT_MASK;
            }
            g_packet_length[master_port] = 0;
        }
        while ((spi_master_register[master_port]->SPI_STATUS1) != SPI_STATUS1_BUSY_MASK);
        do {
            irq_status = spi_master_register[master_port]->SPI_STATUS0;
        } while ((irq_status != SPI_STATUS0_FINISH_MASK) && (irq_status != SPI_STATUS0_PAUSE_MASK));
    } while ((g_pause_count[master_port] > 0) || (g_small_size_pause_count[master_port] > 0) || (g_packet_length[master_port] > 0));
}

hal_spi_master_status_t spi_analyse_transfer_size(hal_spi_master_port_t master_port, uint32_t size)
{
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;
    uint32_t temp_size = size;

    if (temp_size > SPI_MAX_SIZE_FOR_NON_PAUSE) {
        /* should use pause mode now */
        g_pause_count[master_port] = (temp_size / SPI_MAX_SIZE_FOR_NON_PAUSE);
        /* pause mode is conflicted with de-assert feature */
        if ((spi_master_register[master_port]->SPI_CMD & SPI_CMD_CS_DEASSERT_EN_MASK) == SPI_CMD_CS_DEASSERT_EN_MASK) {
            log_hal_error("[SPIM%d]:pause deassert mode error.\r\n", master_port);
            return HAL_SPI_MASTER_STATUS_ERROR;
        }
        temp_size = (temp_size % SPI_MAX_SIZE_FOR_NON_PAUSE);
        if (temp_size == 0) {
            g_pause_count[master_port] -= 1;
            temp_size = SPI_MAX_SIZE_FOR_NON_PAUSE;
        }
    }

    if (temp_size > SPI_MAX_PACKET_LENGTH) {
        /* if size > 1kB but not as multiple of 1kB, using g_small_size_pause_count[master_port] to support it */
        if ((temp_size % SPI_MAX_PACKET_LENGTH) > 0) {
            g_small_size_pause_count[master_port] = (temp_size / SPI_MAX_PACKET_LENGTH);
            g_packet_count[master_port] = 0;
            g_packet_length[master_port] = (temp_size % SPI_MAX_PACKET_LENGTH);
        } else {
            g_packet_count[master_port] = ((temp_size / SPI_MAX_PACKET_LENGTH) - 1);
            g_packet_length[master_port] = SPI_MAX_PACKET_LENGTH;
        }
    } else if (temp_size > 0) {
        g_packet_count[master_port] = 0;
        g_packet_length[master_port] = temp_size;
    } else {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    return status;
}

#ifdef HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING
void spi_master_set_chip_select_timing(hal_spi_master_port_t master_port, hal_spi_master_chip_select_timing_t chip_select_timing)
{
    /* set parameters to CFG0 register */
    spi_master_register[master_port]->SPI_CFG0 &= (uint32_t)(~(SPI_CFG0_CS_HOLD_COUNT_MASK | SPI_CFG0_CS_SETUP_COUNT_MASK));
    spi_master_register[master_port]->SPI_CFG0 |= (chip_select_timing.chip_select_setup_count << SPI_CFG0_CS_SETUP_COUNT_OFFSET);
    spi_master_register[master_port]->SPI_CFG0 |= (chip_select_timing.chip_select_hold_count << SPI_CFG0_CS_HOLD_COUNT_OFFSET);

    /* set idle parameter to CFG1 register low 8 bits*/
    spi_master_register[master_port]->SPI_CFG1 &= (~SPI_CFG1_CS_IDLE_COUNT_MASK);
    spi_master_register[master_port]->SPI_CFG1 |= (chip_select_timing.chip_select_idle_count << SPI_CFG1_CS_IDLE_COUNT_OFFSET);
}
#endif

#ifdef HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG
void spi_master_set_deassert(hal_spi_master_port_t master_port, hal_spi_master_deassert_t deassert)
{
    switch (deassert) {
        case HAL_SPI_MASTER_DEASSERT_DISABLE:
            spi_master_register[master_port]->SPI_CMD &= (~SPI_CMD_CS_DEASSERT_EN_MASK);
            break;
        case HAL_SPI_MASTER_DEASSERT_ENABLE:
            spi_master_register[master_port]->SPI_CMD |= SPI_CMD_CS_DEASSERT_EN_MASK;
            break;
    }
}
#endif

#endif /* HAL_SPI_MASTER_MODULE_ENABLED */

