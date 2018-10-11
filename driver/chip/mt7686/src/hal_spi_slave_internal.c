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

#include "hal_spi_slave.h"

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED

#include "hal_spi_slave_internal.h"
#include "hal_clock.h"
#include "hal_log.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"

static SPIS_REGISTER_T *const g_spi_slave_register[HAL_SPI_SLAVE_MAX] = {SPI_SLAVE_0};
static hal_spi_slave_fsm_status_t g_spi_slave_fsm[MAX_STATUS][MAX_OPERATION_CMD] = {
    {HAL_SPI_SLAVE_FSM_INVALID_OPERATION,     HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION, HAL_SPI_SLAVE_FSM_INVALID_OPERATION,  HAL_SPI_SLAVE_FSM_INVALID_OPERATION,   HAL_SPI_SLAVE_FSM_INVALID_OPERATION,  HAL_SPI_SLAVE_FSM_INVALID_OPERATION},
    {HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION,     HAL_SPI_SLAVE_FSM_INVALID_OPERATION, HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION,  HAL_SPI_SLAVE_FSM_INVALID_OPERATION,   HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION,  HAL_SPI_SLAVE_FSM_INVALID_OPERATION},
    {HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CR, HAL_SPI_SLAVE_FSM_INVALID_OPERATION, HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CR, HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION,   HAL_SPI_SLAVE_FSM_ERROR_CW_AFTER_CR,  HAL_SPI_SLAVE_FSM_ERROR_WRITE_AFTER_CR},
    {HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CW, HAL_SPI_SLAVE_FSM_INVALID_OPERATION, HAL_SPI_SLAVE_FSM_ERROR_CR_AFTER_CW,  HAL_SPI_SLAVE_FSM_ERROR_READ_AFTER_CW, HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CW, HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION}
};
#ifdef HAL_SLEEP_MANAGER_ENABLED
static uint32_t g_spi_slave_ctrl_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_ie_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_tmout_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_cmd_def0_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_cmd_def1_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_cmd_def2_reg[HAL_SPI_SLAVE_MAX] = {0};
#endif

uint8_t g_last2now_status[2] = {PWROFF_STA, PWROFF_STA};

void spi_slave_lisr(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data)
{
    uint8_t last_fsm_status;
    uint32_t irq_status;
    hal_spi_slave_transaction_status_t status;
    hal_spi_slave_fsm_status_t fsm_state = HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION;

    irq_status = ((g_spi_slave_register[spi_port]->INT) & SPIS_INT_MASK);

    switch (irq_status) {
        case SPIS_INT_POWER_ON_MASK:
            status.interrupt_status = HAL_SPI_SLAVE_EVENT_POWER_ON;
#ifdef HAL_SLEEP_MANAGER_ENABLED
            /* after receive POWER-ON command, lock sleep */
            hal_sleep_manager_lock_sleep(SLEEP_LOCK_SPI_SLAVE);
#endif
            spi_slave_update_status(PWRON_STA);
            last_fsm_status = g_last2now_status[0];
            fsm_state = g_spi_slave_fsm[last_fsm_status][POWER_ON_CMD];
            /* set slv_on bit here */
            g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.SW_ON = SPIS_STA_SLV_ON_MASK;
            break;
        case SPIS_INT_POWER_OFF_MASK:
            status.interrupt_status = HAL_SPI_SLAVE_EVENT_POWER_OFF;
            spi_slave_update_status(PWROFF_STA);
            last_fsm_status = g_last2now_status[0];
            fsm_state = g_spi_slave_fsm[last_fsm_status][POWER_OFF_CMD];
            /* clear slv_on bit here */
            g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.SW_ON &= (~SPIS_STA_SLV_ON_MASK);
#ifdef HAL_SLEEP_MANAGER_ENABLED
            /* after spis de-init done, unlock sleep */
            hal_sleep_manager_unlock_sleep(SLEEP_LOCK_SPI_SLAVE);
#endif
            break;
        case SPIS_INT_RD_CFG_FINISH_MASK:
            status.interrupt_status = HAL_SPI_SLAVE_EVENT_CRD_FINISH;
            spi_slave_update_status(CR_STA);
            last_fsm_status = g_last2now_status[0];
            fsm_state = g_spi_slave_fsm[last_fsm_status][CONFIG_READ_CMD];
            break;
        case SPIS_INT_WR_CFG_FINISH_MASK:
            status.interrupt_status = HAL_SPI_SLAVE_EVENT_CWR_FINISH;
            spi_slave_update_status(CW_STA);
            last_fsm_status = g_last2now_status[0];
            fsm_state = g_spi_slave_fsm[last_fsm_status][CONFIG_WRITE_CMD];
            break;
        case SPIS_INT_RD_TRANS_FINISH_MASK:
            status.interrupt_status = HAL_SPI_SLAVE_EVENT_RD_FINISH;
            spi_slave_update_status(PWRON_STA);
            last_fsm_status = g_last2now_status[0];
            fsm_state = g_spi_slave_fsm[last_fsm_status][READ_CMD];
            /* clear TX_DMA_SW_READY bit here */
            g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.TXDMA_SW_RDY = 0;
            break;
        case SPIS_INT_WR_TRANS_FINISH_MASK:
            status.interrupt_status = HAL_SPI_SLAVE_EVENT_WR_FINISH;
            spi_slave_update_status(PWRON_STA);
            last_fsm_status = g_last2now_status[0];
            fsm_state = g_spi_slave_fsm[last_fsm_status][WRITE_CMD];
            /* clear RX_DMA_SW_READY bit here */
            g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.RXDMA_SW_RDY = 0;
            break;
        case SPIS_INT_RD_DATA_ERR_MASK:
        case SPIS_INT_WR_DATA_ERR_MASK:
        case SPIS_INT_TMOUT_ERR_MASK:
            if (irq_status == SPIS_INT_RD_DATA_ERR_MASK) {
                status.interrupt_status = HAL_SPI_SLAVE_EVENT_RD_ERR;
            } else if (irq_status == SPIS_INT_WR_DATA_ERR_MASK) {
                status.interrupt_status = HAL_SPI_SLAVE_EVENT_WR_ERR;
            } else {
                status.interrupt_status = HAL_SPI_SLAVE_EVENT_TIMEOUT_ERR;
            }
            spi_slave_update_status(PWRON_STA);
            /* clear TX/RX_DMA_SW_READY bit here */
            g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.TXDMA_SW_RDY = 0;
            g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.RXDMA_SW_RDY = 0;
            break;
        default:
            break;
    }
    status.fsm_status = fsm_state;
    user_callback(status, user_data);
}

void spi_slave_init(hal_spi_slave_port_t spi_port, const hal_spi_slave_config_t *spi_config)
{
    /* reset spi slave's status frist */
    g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.SW_RST = 1;
    g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.SW_RST = 0;

    /* user configure parameters */
    switch (spi_config->bit_order) {
        case HAL_SPI_SLAVE_LSB_FIRST:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL0 &= (~(SPIS_CTRL_TXMSBF_MASK | SPIS_CTRL_RXMSBF_MASK));
            break;
        case HAL_SPI_SLAVE_MSB_FIRST:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL0 |= (SPIS_CTRL_TXMSBF_MASK | SPIS_CTRL_RXMSBF_MASK);
            break;
    }

    switch (spi_config->phase) {
        case HAL_SPI_SLAVE_CLOCK_PHASE0:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL0 &= (~SPIS_CTRL_CPHA_MASK);
            break;
        case HAL_SPI_SLAVE_CLOCK_PHASE1:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL0 |= SPIS_CTRL_CPHA_MASK;
            break;
    }

    switch (spi_config->polarity) {
        case HAL_SPI_SLAVE_CLOCK_POLARITY0:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL0 &= (~SPIS_CTRL_CPOL_MASK);
            break;
        case HAL_SPI_SLAVE_CLOCK_POLARITY1:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL0 |= SPIS_CTRL_CPOL_MASK;
            break;
    }

    /* timeout threshold */
    g_spi_slave_register[spi_port]->TMOUT_THR = spi_config->timeout_threshold;

    /* enable all interrupt, set four-byte address and size, set sw decode bit */
    g_spi_slave_register[spi_port]->IE |= SPIS_IE_MASK;
    g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL0 |= SPIS_CTRL_SIZE_OF_ADDR_MASK;
    g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL1 = (SPIS_CTRL_DEC_ADDR_EN_MASK >> 8) | (SPIS_CTRL_SW_RDY_EN_MASK >> 8);
}

hal_spi_slave_status_t spi_slave_send(hal_spi_slave_port_t spi_port, const uint8_t *data, uint32_t size)
{
    uint32_t config_size = 0;

    /* return HAL_SPI_SLAVE_STATUS_ERROR if config_size isn't equal to size. */
    config_size = g_spi_slave_register[spi_port]->TRANS_LENGTH;
    if (config_size != size) {
        log_hal_error("[SPIS%d][send]:size error.\r\n", spi_port);
        return HAL_SPI_SLAVE_STATUS_ERROR;
    } else {
        /* set src_buffer_addr, buffer_size as size and tx_fifo_ready. */
        g_spi_slave_register[spi_port]->BUFFER_BASE_ADDR = (uint32_t)data;
        g_spi_slave_register[spi_port]->BUFFER_SIZE = size;
        g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.TXDMA_SW_RDY = 1;
    }

    return HAL_SPI_SLAVE_STATUS_OK;
}

hal_spi_slave_status_t spi_slave_query_config_info(hal_spi_slave_port_t spi_port, uint32_t *address, uint32_t *length)
{
    if ((g_spi_slave_register[spi_port]->STA_UNION.STA_CELLS.STA & 0xff) != (SPIS_STA_CFG_SUCCESS_MASK | SPIS_STA_SLV_ON_MASK)) {
        return HAL_SPI_SLAVE_STATUS_ERROR;
    }

    *address = g_spi_slave_register[spi_port]->TRANS_ADDR;
    *length = g_spi_slave_register[spi_port]->TRANS_LENGTH;

    return HAL_SPI_SLAVE_STATUS_OK;
}

hal_spi_slave_status_t spi_slave_receive(hal_spi_slave_port_t spi_port, uint8_t *buffer, uint32_t size)
{
    uint32_t config_size = 0;

    /* return HAL_SPI_SLAVE_STATUS_ERROR if config_size isn't equal to size */
    config_size = g_spi_slave_register[spi_port]->TRANS_LENGTH;
    if (config_size != size) {
        log_hal_error("[SPIS%d][receive]:size error.\r\n", spi_port);
        return HAL_SPI_SLAVE_STATUS_ERROR;
    } else {
        /* set src_buffer_addr, buffer_size as size and rx_fifo_ready. */
        g_spi_slave_register[spi_port]->BUFFER_BASE_ADDR = (uint32_t)buffer;
        g_spi_slave_register[spi_port]->BUFFER_SIZE = size;
        g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.RXDMA_SW_RDY = 1;
    }

    return HAL_SPI_SLAVE_STATUS_OK;
}

void spi_slave_set_early_miso(hal_spi_slave_port_t spi_port, hal_spi_slave_early_miso_t early_miso)
{
    switch (early_miso) {
        case HAL_SPI_SLAVE_EARLY_MISO_DISABLE:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.MISO_EARLY_TRANS = 0;
            break;
        case HAL_SPI_SLAVE_EARLY_MISO_ENABLE:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.MISO_EARLY_TRANS = 1;
            break;
    }
}

void spi_slave_set_command(hal_spi_slave_port_t spi_port, hal_spi_slave_command_type_t command, uint8_t value)
{
    switch (command) {
        case HAL_SPI_SLAVE_CMD_WS:
            g_spi_slave_register[spi_port]->CMD_DEF0_UNION.CMD_DEF0_CELLS.CMD_WS = value;
            break;
        case HAL_SPI_SLAVE_CMD_RS:
            g_spi_slave_register[spi_port]->CMD_DEF0_UNION.CMD_DEF0_CELLS.CMD_RS = value;
            break;
        case HAL_SPI_SLAVE_CMD_WR:
            g_spi_slave_register[spi_port]->CMD_DEF1_UNION.CMD_DEF1_CELLS.CMD_WR = value;
            break;
        case HAL_SPI_SLAVE_CMD_RD:
            g_spi_slave_register[spi_port]->CMD_DEF1_UNION.CMD_DEF1_CELLS.CMD_RD = value;
            break;
        case HAL_SPI_SLAVE_CMD_POWEROFF:
            g_spi_slave_register[spi_port]->CMD_DEF0_UNION.CMD_DEF0_CELLS.CMD_PWOFF = value;
            break;
        case HAL_SPI_SLAVE_CMD_POWERON:
            g_spi_slave_register[spi_port]->CMD_DEF0_UNION.CMD_DEF0_CELLS.CMD_PWON = value;
            break;
        case HAL_SPI_SLAVE_CMD_CW:
            g_spi_slave_register[spi_port]->CMD_DEF1_UNION.CMD_DEF1_CELLS.CMD_CW = value;
            break;
        case HAL_SPI_SLAVE_CMD_CR:
            g_spi_slave_register[spi_port]->CMD_DEF1_UNION.CMD_DEF1_CELLS.CMD_CR = value;
            break;
        case HAL_SPI_SLAVE_CMD_CT:
            g_spi_slave_register[spi_port]->CMD_DEF2 = value;
            break;
    }
}

void spi_slave_reset_default(hal_spi_slave_port_t spi_port)
{
    uint32_t int_status;

    g_spi_slave_register[spi_port]->CTRL_UNION.CTRL = 0x00000100;
    int_status = g_spi_slave_register[spi_port]->INT;
    int_status = int_status;
    g_spi_slave_register[spi_port]->IE = 0x00000000;
    g_spi_slave_register[spi_port]->CMD_DEF0_UNION.CMD_DEF0 = 0x08060402;
    g_spi_slave_register[spi_port]->CMD_DEF1_UNION.CMD_DEF1 = 0x0e810c0a;
    g_spi_slave_register[spi_port]->CMD_DEF2 = 0x00000010;

    g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.SW_ON = 0;
    g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.SW_RST = 1;
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
void spi_slave_backup_register_callback(void *data)
{
    hal_spi_slave_port_t slave_port;

    for (slave_port = HAL_SPI_SLAVE_0; slave_port < HAL_SPI_SLAVE_MAX; slave_port++) {
        /* backup related spi_slave register values */
        g_spi_slave_ctrl_reg[slave_port] = g_spi_slave_register[slave_port]->CTRL_UNION.CTRL;
        g_spi_slave_ie_reg[slave_port] = g_spi_slave_register[slave_port]->IE;
        g_spi_slave_tmout_reg[slave_port] = g_spi_slave_register[slave_port]->TMOUT_THR;
        g_spi_slave_cmd_def0_reg[slave_port] = g_spi_slave_register[slave_port]->CMD_DEF0_UNION.CMD_DEF0;
        g_spi_slave_cmd_def1_reg[slave_port] = g_spi_slave_register[slave_port]->CMD_DEF1_UNION.CMD_DEF1;
        g_spi_slave_cmd_def2_reg[slave_port] = g_spi_slave_register[slave_port]->CMD_DEF2;
    }
}

void spi_slave_restore_register_callback(void *data)
{
    hal_spi_slave_port_t slave_port;

    for (slave_port = HAL_SPI_SLAVE_0; slave_port < HAL_SPI_SLAVE_MAX; slave_port++) {
        /* restore related spi_slave register values */
        g_spi_slave_register[slave_port]->CTRL_UNION.CTRL = g_spi_slave_ctrl_reg[slave_port];
        g_spi_slave_register[slave_port]->IE = g_spi_slave_ie_reg[slave_port];
        g_spi_slave_register[slave_port]->TMOUT_THR = g_spi_slave_tmout_reg[slave_port];
        g_spi_slave_register[slave_port]->CMD_DEF0_UNION.CMD_DEF0 = g_spi_slave_cmd_def0_reg[slave_port];
        g_spi_slave_register[slave_port]->CMD_DEF1_UNION.CMD_DEF1 = g_spi_slave_cmd_def1_reg[slave_port];
        g_spi_slave_register[slave_port]->CMD_DEF2 = g_spi_slave_cmd_def2_reg[slave_port];
    }
}
#endif

#endif /*HAL_SPI_SLAVE_MODULE_ENABLED*/

