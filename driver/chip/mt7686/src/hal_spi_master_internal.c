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
#include "hal_sleep_manager_internal.h"

SPIM_REGISTER_T *const g_spi_master_register[HAL_SPI_MASTER_MAX] = {SPI_MASTER_0};
#ifdef HAL_SLEEP_MANAGER_ENABLED
static uint32_t g_spi_master_ctrl_0_reg[HAL_SPI_MASTER_MAX] = {0};
static uint32_t g_spi_master_ctrl_1_reg[HAL_SPI_MASTER_MAX] = {0};
static uint32_t g_spi_master_cfg_0_reg[HAL_SPI_MASTER_MAX] = {0};
static uint32_t g_spi_master_cfg_1_reg[HAL_SPI_MASTER_MAX] = {0};
static uint32_t g_spi_master_cfg_2_reg[HAL_SPI_MASTER_MAX] = {0};
static uint32_t g_spi_master_cfg_3_reg[HAL_SPI_MASTER_MAX] = {0};
static uint32_t g_spi_master_dlysel0_reg[HAL_SPI_MASTER_MAX] = {0};
static uint32_t g_spi_master_dlysel1_reg[HAL_SPI_MASTER_MAX] = {0};
static uint32_t g_spi_master_dlysel2_reg[HAL_SPI_MASTER_MAX] = {0};
#endif

extern spi_master_direction_t g_spi_master_direction[HAL_SPI_MASTER_MAX];

void spi_master_isr_handler(hal_spi_master_port_t master_port, hal_spi_master_callback_t user_callback, void *user_data)
{
    hal_spi_master_callback_event_t event;
    uint32_t interrupt_status = 0;

    interrupt_status = g_spi_master_register[master_port]->INT;

    if (interrupt_status & SPIM_INT_FINISH_INT_MASK) {
        switch (g_spi_master_direction[master_port]) {
            case SPI_MASTER_TX:
                event = HAL_SPI_MASTER_EVENT_SEND_FINISHED;
                break;
            case SPI_MASTER_RX:
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
        hal_sleep_manager_unlock_sleep(SLEEP_LOCK_SPI_MASTER);
#endif
    } else if (interrupt_status & SPIM_INT_PAUSE_INT_MASK) {
        spi_master_start_transfer_dma(master_port, true, false);
    }
}

void spi_master_init(hal_spi_master_port_t master_port, const hal_spi_master_config_t *spi_config)
{
    uint32_t sck_count;

    /* TODO: need to change to use API to get frequency of SPI master. */
    sck_count = SPI_MASTER_INPUT_CLOCK_FREQUENCY / (spi_config->clock_frequency * 2) - 1;
    g_spi_master_register[master_port]->CFG1_UNION.CFG1 = ((sck_count << SPIM_CFG1_SCK_LOW_COUNT_OFFSET) | sck_count);

    switch (spi_config->bit_order) {
        case HAL_SPI_MASTER_LSB_FIRST:
            g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 &= (~(SPIM_CTRL0_TXMSBF_MASK | SPIM_CTRL0_RXMSBF_MASK));
            break;
        case HAL_SPI_MASTER_MSB_FIRST:
            g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 |= (SPIM_CTRL0_TXMSBF_MASK | SPIM_CTRL0_RXMSBF_MASK);
            break;
    }

    switch (spi_config->polarity) {
        case HAL_SPI_MASTER_CLOCK_POLARITY0:
            g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 &= (~SPIM_CTRL0_CPOL_MASK);
            break;
        case HAL_SPI_MASTER_CLOCK_POLARITY1:
            g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 |= (SPIM_CTRL0_CPOL_MASK);
            break;
    }

    switch (spi_config->phase) {
        case HAL_SPI_MASTER_CLOCK_PHASE0:
            g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 &= (~SPIM_CTRL0_CPHA_MASK);
            break;
        case HAL_SPI_MASTER_CLOCK_PHASE1:
            g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 |= (SPIM_CTRL0_CPHA_MASK);
            break;
    }

    /* default use non-paused mode*/
    g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.PAUSE_EN = 0;

    g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 |= HAL_SPI_MASTER_SINGLE_MODE << SPIM_CTRL0_TYPE_OFFSET;
}

#ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG
void spi_master_set_advanced_config(hal_spi_master_port_t master_port, const hal_spi_master_advanced_config_t *advanced_config)
{
    switch (advanced_config->byte_order) {
        case HAL_SPI_MASTER_LITTLE_ENDIAN:
            g_spi_master_register[master_port]->CTRL1_UNION.CTRL1_CELLS.CTRL1 &= ~(SPIM_CTRL1_ENDIAN_MASK);
            break;
        case HAL_SPI_MASTER_BIG_ENDIAN:
            g_spi_master_register[master_port]->CTRL1_UNION.CTRL1_CELLS.CTRL1 |= SPIM_CTRL1_ENDIAN_MASK;
            break;
    }

    switch (advanced_config->chip_polarity) {
        case HAL_SPI_MASTER_CHIP_SELECT_LOW:
            g_spi_master_register[master_port]->CTRL1_UNION.CTRL1_CELLS.CTRL1 &= (~SPIM_CTRL1_CS_POL_MASK);
            break;
        case HAL_SPI_MASTER_CHIP_SELECT_HIGH:
            g_spi_master_register[master_port]->CTRL1_UNION.CTRL1_CELLS.CTRL1 |= SPIM_CTRL1_CS_POL_MASK;
            break;
    }

    switch (advanced_config->sample_select) {
        case HAL_SPI_MASTER_SAMPLE_POSITIVE:
            g_spi_master_register[master_port]->CTRL1_UNION.CTRL1_CELLS.CTRL1 &= (~SPIM_CTRL1_SAMPLE_SEL_MASK);
            break;
        case HAL_SPI_MASTER_SAMPLE_NEGATIVE:
            g_spi_master_register[master_port]->CTRL1_UNION.CTRL1_CELLS.CTRL1 |= SPIM_CTRL1_SAMPLE_SEL_MASK;
            break;
    }

    g_spi_master_register[master_port]->CTRL1_UNION.CTRL1_CELLS.GET_DLY = (uint8_t)(advanced_config->get_tick);
}
#endif

uint32_t spi_master_get_status(hal_spi_master_port_t master_port)
{
    volatile uint32_t status;

    status = (g_spi_master_register[master_port]->STA);

    if (status == 1) {
        status = 0;
    } else {
        status = 1;
    }

    return status;
}

void spi_master_set_rwaddr(hal_spi_master_port_t master_port, spi_master_direction_t type, const uint8_t *addr)
{
    switch (type) {
        case SPI_MASTER_TX:
            g_spi_master_register[master_port]->TX_SRC = (uint32_t)addr;
            break;
        case SPI_MASTER_RX:
            g_spi_master_register[master_port]->RX_DST = (uint32_t)addr;
            break;
    }
}

hal_spi_master_status_t spi_master_push_data(hal_spi_master_port_t master_port, const uint8_t *data, uint32_t size, uint32_t total_size)
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
    if ((g_spi_master_register[master_port]->CTRL1_UNION.CTRL1_CELLS.CTRL1) & SPIM_CTRL1_TX_ENDIAN_MASK) {
        log_hal_error("[SPIM%d][push_data]:big_endian error.\r\n", master_port);
        return HAL_SPI_MASTER_STATUS_ERROR;
    }

    /* clear and configure packet length and count register */
    /* HW limitation: When using FIFO mode, need to configure transfer size before push data to Tx FIFO */
    g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = 0;
    g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = (total_size - 1);

    quotient = size / sizeof(uint32_t);
    remainder = size % sizeof(uint32_t);

    for (i = 0; i < quotient; i++) {
        spi_data = 0;
        for (j = 0; j < 4; j++) {
            temp_data = (*temp_addr);
            spi_data |= (temp_data << (8 * j));
            temp_addr++;
        }
        g_spi_master_register[master_port]->TX_DATA = spi_data;
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
                g_spi_master_register[master_port]->TX_DATA = (spi_data & 0x00FFFFFF);
                break;
            case 2:
                g_spi_master_register[master_port]->TX_DATA = (spi_data & 0x0000FFFF);
                break;
            case 1:
                g_spi_master_register[master_port]->TX_DATA = (spi_data & 0x000000FF);
                break;
        }
    }

    return status;
}

hal_spi_master_status_t spi_master_pop_data(hal_spi_master_port_t master_port, uint8_t *buffer, uint32_t size)
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
    if ((g_spi_master_register[master_port]->CTRL1_UNION.CTRL1_CELLS.CTRL1) & SPIM_CTRL1_RX_ENDIAN_MASK) {
        log_hal_error("[SPIM%d][pop_data]:big_endian error.\r\n", master_port);
        return HAL_SPI_MASTER_STATUS_ERROR;
    }

    quotient = size / sizeof(uint32_t);
    remainder = size % sizeof(uint32_t);

    for (i = 0; i < quotient; i++) {
        spi_data = g_spi_master_register[master_port]->RX_DATA;
        for (j = 0; j < 4; j++) {
            temp_data = ((spi_data & (0xff << (8 * j))) >> (8 * j));
            *temp_addr = temp_data;
            temp_addr++;
        }
    }
    if (remainder > 0) {
        spi_data = g_spi_master_register[master_port]->RX_DATA;
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

void spi_master_set_interrupt(hal_spi_master_port_t master_port, bool status)
{
    switch (status) {
        case false:
            g_spi_master_register[master_port]->IE = (~(SPIM_IE_FINISH_IE_MASK | SPIM_IE_PAUSE_IE_MASK));
            break;
        case true:
            g_spi_master_register[master_port]->IE = (SPIM_IE_FINISH_IE_MASK | SPIM_IE_PAUSE_IE_MASK);
            break;
    }
}

void spi_master_clear_fifo(hal_spi_master_port_t master_port)
{
    g_spi_master_register[master_port]->TRIG_UNION.TRIG_CELLS.RST = 1;
    g_spi_master_register[master_port]->TRIG_UNION.TRIG_CELLS.RST = 0;
}

void spi_master_set_mode(hal_spi_master_port_t master_port, spi_master_direction_t type, spi_master_mode_t mode)
{
    if (SPI_MASTER_TX == type) {
        switch (mode) {
            case SPI_MASTER_MODE_DMA:
                g_spi_master_register[master_port]->CTRL1_UNION.CTRL1_CELLS.TXDMA_EN = 1;
                break;
            case SPI_MASTER_MODE_FIFO:
                g_spi_master_register[master_port]->CTRL1_UNION.CTRL1_CELLS.TXDMA_EN = 0;
        }
    } else {
        switch (mode) {
            case SPI_MASTER_MODE_DMA:
                g_spi_master_register[master_port]->CTRL1_UNION.CTRL1_CELLS.RXDMA_EN = 1;
                break;
            case SPI_MASTER_MODE_FIFO:
                g_spi_master_register[master_port]->CTRL1_UNION.CTRL1_CELLS.RXDMA_EN = 0;
        }
    }
}

/*
 * g_full_packet_count = x, g_partial_packet_count = y, g_remainder_count = z
 *
 *   index     transfer_start               transfer_end       pause_mode            condition
 *     0             x                           x                  no               x==1,y==0,z==0
 *     1             x                           x                 yes               x>1, y==0,z==0
 *     2             x                           y                 yes               x>=1,y!=0,z==0
 *     3             x                           z                 yes               x>=1,y>=0,z!=0
 *     4             y                           y                  no               x==0,y!=0,z==0
 *     5             y                           z                 yes               x==0,y!=0,z!=0
 *     6             z                           z                  no               x==0,y==0,z!=0
 */

typedef enum {
    PAUSE_END_NONE,
    PAUSE_END_FULL,
    PAUSE_END_PARTIAL,
    PAUSE_END_REMAINDER,
} pause_end_t;
static const bool g_pause_mode_on[7] = {false, true, true, true, false, true, false};
static const pause_end_t g_pause_mode_off[7] = {PAUSE_END_NONE, PAUSE_END_FULL, PAUSE_END_PARTIAL, PAUSE_END_REMAINDER, PAUSE_END_NONE, PAUSE_END_REMAINDER, PAUSE_END_NONE};
static uint32_t g_pause_mode_index[HAL_SPI_MASTER_MAX] = {0};
static uint32_t g_full_packet_count[HAL_SPI_MASTER_MAX] = {0};
static uint32_t g_partial_packet_count[HAL_SPI_MASTER_MAX] = {0};
static uint32_t g_remainder_count[HAL_SPI_MASTER_MAX] = {0};

void spi_master_start_transfer_fifo(hal_spi_master_port_t master_port, bool is_write)
{
    uint32_t status;

    if (is_write == true) {
        g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 |= (1 << SPIM_CTRL0_RW_MODE_OFFSET);
    } else {
        g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 &= ~(1 << SPIM_CTRL0_RW_MODE_OFFSET);
    }

    /* HW limitation: When using FIFO mode, need to configure transfer size before push data to Tx FIFO */
    g_spi_master_register[master_port]->TRIG_UNION.TRIG_CELLS.CMD_ACT = 1;
    do {
        status = g_spi_master_register[master_port]->STA;
    } while ((status == SPIM_STATUS_BUSY_MASK));
    /* read clear the finish flag after transfer complete */
    status = (g_spi_master_register[master_port]->INT);
    status = status;
}

void spi_master_start_transfer_dma(hal_spi_master_port_t master_port, bool is_continue, bool is_write)
{
    bool continue_pause_mode = true;

    if (is_continue == true) {
        if (!((g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.PAUSE_EN) & 1)) {
            log_hal_error("[spi-%d]: pause status error.\r\n", master_port);
            return;
        }
    }

    if (g_full_packet_count[master_port] > 0) {
        g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = (uint8_t)SPI_MASTER_MAX_PACKET_COUNT_MASK;
        g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = SPI_MASTER_MAX_PACKET_LENGTH_MASK;
        g_full_packet_count[master_port]--;
        if ((g_pause_mode_off[g_pause_mode_index[master_port]] == PAUSE_END_FULL) &&
                (g_full_packet_count[master_port] == 0)) {
            continue_pause_mode = false;
        }
    } else if (g_partial_packet_count[master_port] > 0) {
        /* only need one transfer for g_partial_packet_count */
        g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = (uint8_t)(g_partial_packet_count[master_port] - 1);
        g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = SPI_MASTER_MAX_PACKET_LENGTH_MASK;
        g_partial_packet_count[master_port] = 0;
        if (g_pause_mode_off[g_pause_mode_index[master_port]] == PAUSE_END_PARTIAL) {
            continue_pause_mode = false;
        }
    } else if (g_remainder_count[master_port] > 0) {
        /* packet_loop_cnt = 0 */
        g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = 0;
        g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = g_remainder_count[master_port] - 1;
        if (g_pause_mode_off[g_pause_mode_index[master_port]] == PAUSE_END_REMAINDER) {
            continue_pause_mode = false;
        }
    } else {
        log_hal_error("[spi-%d]: machine status error.\r\n", master_port);
        return;
    }

    if (is_continue == false) {
        /* set direction for dual mode and quad mode. */
        if (is_write == true) {
            g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 |= (1 << SPIM_CTRL0_RW_MODE_OFFSET);
        } else {
            g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 &= ~(1 << SPIM_CTRL0_RW_MODE_OFFSET);
        }
        if (g_pause_mode_on[g_pause_mode_index[master_port]] == true) {
            g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.PAUSE_EN = 1;
        }
        g_spi_master_register[master_port]->TRIG_UNION.TRIG_CELLS.CMD_ACT = 1;
    } else {
        if (continue_pause_mode == false) {
            g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.PAUSE_EN = 0;
            g_spi_master_register[master_port]->TRIG_UNION.TRIG_CELLS.RESUME = 1;
        } else {
            g_spi_master_register[master_port]->TRIG_UNION.TRIG_CELLS.RESUME = 1;
        }
    }
}

void spi_master_start_transfer_dma_blocking(hal_spi_master_port_t master_port, bool is_write)
{
    bool continue_pause_mode, is_continue, loop_end;
    uint32_t irq_status;

    is_continue = false;
    continue_pause_mode = true;
    do {
        if (g_full_packet_count[master_port] > 0) {
            g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = (uint8_t)SPI_MASTER_MAX_PACKET_COUNT_MASK;
            g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = SPI_MASTER_MAX_PACKET_LENGTH_MASK;
            g_full_packet_count[master_port]--;
            if ((g_pause_mode_off[g_pause_mode_index[master_port]] == PAUSE_END_FULL) &&
                    (g_full_packet_count[master_port] == 0)) {
                continue_pause_mode = false;
            }
        } else if (g_partial_packet_count[master_port] > 0) {
            /* only need one transfer for g_partial_packet_count */
            g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = (uint8_t)(g_partial_packet_count[master_port] - 1);
            g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = SPI_MASTER_MAX_PACKET_LENGTH_MASK;
            g_partial_packet_count[master_port] = 0;
            if (g_pause_mode_off[g_pause_mode_index[master_port]] == PAUSE_END_PARTIAL) {
                continue_pause_mode = false;
            }
        } else if (g_remainder_count[master_port] > 0) {
            /* packet_loop_cnt = 0 */
            g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.PACKET_LOOP_CNT = 0;
            g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.PACKET_LENGTH_CNT = g_remainder_count[master_port] - 1;
            if (g_pause_mode_off[g_pause_mode_index[master_port]] == PAUSE_END_REMAINDER) {
                continue_pause_mode = false;
            }
        } else {
            log_hal_error("[spi-%d]: machine status error.\r\n", master_port);
            return;
        }

        if (is_continue == false) {
            /* set direction for dual mode and quad mode. */
            if (is_write == true) {
                g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 |= (1 << SPIM_CTRL0_RW_MODE_OFFSET);
            } else {
                g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 &= ~(1 << SPIM_CTRL0_RW_MODE_OFFSET);
            }
            loop_end = true;
            if (g_pause_mode_on[g_pause_mode_index[master_port]] == true) {
                g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.PAUSE_EN = 1;
                loop_end = false;
            }
            g_spi_master_register[master_port]->TRIG_UNION.TRIG_CELLS.CMD_ACT = 1;
            is_continue = true;
        } else {
            if (continue_pause_mode == false) {
                g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.PAUSE_EN = 0;
                g_spi_master_register[master_port]->TRIG_UNION.TRIG_CELLS.RESUME = 1;
                loop_end = true;
            } else {
                g_spi_master_register[master_port]->TRIG_UNION.TRIG_CELLS.RESUME = 1;
                loop_end = false;
            }
        }

        /* Wait current part of transfer finish. */
        while ((g_spi_master_register[master_port]->STA) == SPIM_STATUS_BUSY_MASK);
        do {
            irq_status = g_spi_master_register[master_port]->INT;
        } while ((irq_status != SPIM_INT_FINISH_INT_MASK) && (irq_status != SPIM_INT_PAUSE_INT_MASK));
    } while (loop_end == false);
}

hal_spi_master_status_t spi_master_analyse_transfer_size(hal_spi_master_port_t master_port, uint32_t size)
{
    uint32_t remainder;

    g_full_packet_count[master_port] = (size / SPI_MASTER_MAX_SIZE_FOR_NON_PAUSE);
    remainder = size % SPI_MASTER_MAX_SIZE_FOR_NON_PAUSE;
    g_partial_packet_count[master_port] = remainder / SPI_MASTER_MAX_PACKET_LENGTH;
    g_remainder_count[master_port] = remainder % SPI_MASTER_MAX_PACKET_LENGTH;

    /*
     * 1. decide whether we need use pause mode.
     * 2. decide where should we stop pause mode.
     * 3. Refer comment above about how to decide index here.
     */
    if (g_full_packet_count[master_port] > 0) {
        if (g_remainder_count[master_port] > 0) {
            g_pause_mode_index[master_port] = 3;
        } else if (g_partial_packet_count[master_port] > 0) {
            g_pause_mode_index[master_port] = 2;
        } else if (g_full_packet_count[master_port] > 1) {
            g_pause_mode_index[master_port] = 1;
        } else {
            g_pause_mode_index[master_port] = 0;
        }
    } else {
        if (g_remainder_count[master_port] == 0) {
            g_pause_mode_index[master_port] = 4;
        } else if (g_partial_packet_count[master_port] > 0) {
            g_pause_mode_index[master_port] = 5;
        } else {
            g_pause_mode_index[master_port] = 6;
        }
    }

    /* When we need pause mode, de-assert must NOT be enabled. */
    if (g_pause_mode_on[g_pause_mode_index[master_port]] == true) {
        if ((g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.DEASSERT_EN & 1) != 0) {
            log_hal_error("[SPIM%d]:pause deassert mode error.\r\n", master_port);
            return HAL_SPI_MASTER_STATUS_ERROR;
        }
    }

    return HAL_SPI_MASTER_STATUS_OK;
}

#ifdef HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING
void spi_master_set_chip_select_timing(hal_spi_master_port_t master_port, hal_spi_master_chip_select_timing_t chip_select_timing)
{
    g_spi_master_register[master_port]->CFG0_UNION.CFG0 = (chip_select_timing.chip_select_setup_count << 16) | chip_select_timing.chip_select_hold_count;

    g_spi_master_register[master_port]->CFG2_UNION.CFG2_CELLS.CS_IDLE_CNT = chip_select_timing.chip_select_idle_count;
}
#endif

#ifdef HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG
void spi_master_set_deassert(hal_spi_master_port_t master_port, hal_spi_master_deassert_t deassert)
{
    switch (deassert) {
        case HAL_SPI_MASTER_DEASSERT_DISABLE:
            g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.DEASSERT_EN = 0;
            break;
        case HAL_SPI_MASTER_DEASSERT_ENABLE:
            g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.DEASSERT_EN = 1;
            break;
    }
}
#endif

#ifdef HAL_SPI_MASTER_FEATURE_DUAL_QUAD_MODE
void spi_master_set_type(hal_spi_master_port_t master_port, hal_spi_master_mode_t mode)
{
    g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 &= ~(SPIM_CTRL0_TYPE_MASK);
    g_spi_master_register[master_port]->CTRL0_UNION.CTRL0_CELLS.CTRL0 |= mode << SPIM_CTRL0_TYPE_OFFSET;
}

void spi_master_set_dummy_bits(hal_spi_master_port_t master_port, uint8_t dummy_bits)
{
    g_spi_master_register[master_port]->CFG3_UNION.CFG3_CELLS.DUMMY_CNT = dummy_bits;
}

void spi_master_set_command_bytes(hal_spi_master_port_t master_port, uint8_t command_bytes)
{
    g_spi_master_register[master_port]->CFG3_UNION.CFG3_CELLS.COMMAND_CNT = command_bytes;
}
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
void spi_master_backup_register_callback(void *data)
{
    hal_spi_master_port_t master_port;

    for (master_port = HAL_SPI_MASTER_0; master_port < HAL_SPI_MASTER_MAX; master_port++) {
        /* backup related spi_master register values */
        g_spi_master_ctrl_0_reg[master_port] = g_spi_master_register[master_port]->CTRL0_UNION.CTRL0;
        g_spi_master_ctrl_1_reg[master_port] = g_spi_master_register[master_port]->CTRL1_UNION.CTRL1;
        g_spi_master_cfg_0_reg[master_port] = g_spi_master_register[master_port]->CFG0_UNION.CFG0;
        g_spi_master_cfg_1_reg[master_port] = g_spi_master_register[master_port]->CFG1_UNION.CFG1;
        g_spi_master_cfg_2_reg[master_port] = g_spi_master_register[master_port]->CFG2_UNION.CFG2;
        g_spi_master_cfg_3_reg[master_port] = g_spi_master_register[master_port]->CFG3_UNION.CFG3;
        g_spi_master_dlysel0_reg[master_port] = g_spi_master_register[master_port]->DLYSEL0_UNION.DLYSEL0;
        g_spi_master_dlysel1_reg[master_port] = g_spi_master_register[master_port]->DLYSEL1_UNION.DLYSEL1;
        g_spi_master_dlysel2_reg[master_port] = g_spi_master_register[master_port]->DLYSEL2_UNION.DLYSEL2;
    }
}

void spi_master_restore_register_callback(void *data)
{
    hal_spi_master_port_t master_port;

    for (master_port = HAL_SPI_MASTER_0; master_port < HAL_SPI_MASTER_MAX; master_port++) {
        /* restore related spi_master register values */
        g_spi_master_register[master_port]->CTRL0_UNION.CTRL0 = g_spi_master_ctrl_0_reg[master_port];
        g_spi_master_register[master_port]->CTRL1_UNION.CTRL1 = g_spi_master_ctrl_1_reg[master_port];
        g_spi_master_register[master_port]->CFG0_UNION.CFG0 = g_spi_master_cfg_0_reg[master_port];
        g_spi_master_register[master_port]->CFG1_UNION.CFG1 = g_spi_master_cfg_1_reg[master_port];
        g_spi_master_register[master_port]->CFG2_UNION.CFG2 = g_spi_master_cfg_2_reg[master_port];
        g_spi_master_register[master_port]->CFG3_UNION.CFG3 = g_spi_master_cfg_3_reg[master_port];
        g_spi_master_register[master_port]->DLYSEL0_UNION.DLYSEL0 = g_spi_master_dlysel0_reg[master_port];
        g_spi_master_register[master_port]->DLYSEL1_UNION.DLYSEL1 = g_spi_master_dlysel1_reg[master_port];
        g_spi_master_register[master_port]->DLYSEL2_UNION.DLYSEL2 = g_spi_master_dlysel2_reg[master_port];
    }
}
#endif

void spi_master_reset_default_value(hal_spi_master_port_t master_port)
{
    uint32_t REG_INT;

    g_spi_master_register[master_port]->IE = 0;

    g_spi_master_register[master_port]->CTRL0_UNION.CTRL0 = 0;
    g_spi_master_register[master_port]->CTRL1_UNION.CTRL1 = 0;
    g_spi_master_register[master_port]->TX_SRC = 0;
    g_spi_master_register[master_port]->RX_DST = 0;
    g_spi_master_register[master_port]->CFG0_UNION.CFG0 = 0;
    g_spi_master_register[master_port]->CFG1_UNION.CFG1 = 0;
    g_spi_master_register[master_port]->CFG2_UNION.CFG2 = 0;
    g_spi_master_register[master_port]->CFG3_UNION.CFG3 = 0;
    g_spi_master_register[master_port]->DLYSEL0_UNION.DLYSEL0 = 0;
    g_spi_master_register[master_port]->DLYSEL1_UNION.DLYSEL1 = 0;
    g_spi_master_register[master_port]->DLYSEL2_UNION.DLYSEL2 = 0;

    g_spi_master_register[master_port]->TRIG_UNION.TRIG_CELLS.RST = 1;
    g_spi_master_register[master_port]->TRIG_UNION.TRIG_CELLS.RST = 0;

    REG_INT = g_spi_master_register[master_port]->INT;
    REG_INT = REG_INT;
}

#endif /* HAL_SPI_MASTER_MODULE_ENABLED */

