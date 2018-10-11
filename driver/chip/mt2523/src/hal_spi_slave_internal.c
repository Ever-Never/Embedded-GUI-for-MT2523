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

static SPI_SLAVE_REGISTER_T *const spi_slave_register[HAL_SPI_SLAVE_MAX] = {SPI_SLAVE};
static const hal_clock_cg_id g_slvx_cg_code[HAL_SPI_SLAVE_MAX] = {HAL_SPI_SLAVE_CG_TABLE};
static hal_spi_slave_fsm_status_t g_spi_slave_fsm[MAX_STATUS][MAX_OPERATION_CMD] = {
    {HAL_SPI_SLAVE_FSM_INVALID_OPERATION,     HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION, HAL_SPI_SLAVE_FSM_INVALID_OPERATION,  HAL_SPI_SLAVE_FSM_INVALID_OPERATION,   HAL_SPI_SLAVE_FSM_INVALID_OPERATION,  HAL_SPI_SLAVE_FSM_INVALID_OPERATION},
    {HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION,     HAL_SPI_SLAVE_FSM_INVALID_OPERATION, HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION,  HAL_SPI_SLAVE_FSM_INVALID_OPERATION,   HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION,  HAL_SPI_SLAVE_FSM_INVALID_OPERATION},
    {HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CR, HAL_SPI_SLAVE_FSM_INVALID_OPERATION, HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CR, HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION,   HAL_SPI_SLAVE_FSM_ERROR_CW_AFTER_CR,  HAL_SPI_SLAVE_FSM_ERROR_WRITE_AFTER_CR},
    {HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CW, HAL_SPI_SLAVE_FSM_INVALID_OPERATION, HAL_SPI_SLAVE_FSM_ERROR_CR_AFTER_CW,  HAL_SPI_SLAVE_FSM_ERROR_READ_AFTER_CW, HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CW, HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION}
};
#ifdef HAL_SLEEP_MANAGER_ENABLED
static uint32_t g_spi_slave_ctrl_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_timout_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_early_half_sck_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_cmd_def0_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_cmd_def1_reg[HAL_SPI_SLAVE_MAX] = {0};
extern uint8_t spis_sleep_handler[HAL_SPI_SLAVE_MAX];
#endif

uint8_t g_last2now_status[2] = {PWROFF_STA, PWROFF_STA};

void spi_slave_lisr(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data)
{
    uint8_t last_fsm_status;
    uint32_t irq_status;
    hal_spi_slave_transaction_status_t status;
    hal_spi_slave_fsm_status_t fsm_state = HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION;

    irq_status = ((spi_slave_register[spi_port]->SPISLV_IRQ) & SPISLV_IRQ_MASK);

    switch (irq_status) {
        case SPISLV_IRQ_POWERON_IRQ_MASK:
            status.interrupt_status = HAL_SPI_SLAVE_EVENT_POWER_ON;
#ifdef HAL_SLEEP_MANAGER_ENABLED
            hal_sleep_manager_lock_sleep(spis_sleep_handler[spi_port]);
#endif
            /* turn on the clock gating */
            hal_clock_enable(g_slvx_cg_code[spi_port]);
            spi_slave_update_status(PWRON_STA);
            last_fsm_status = g_last2now_status[0];
            fsm_state = g_spi_slave_fsm[last_fsm_status][POWER_ON_CMD];
            /* set slv_on bit here */
            spi_slave_register[spi_port]->SPISLV_STATUS |= SPISLV_STATUS_SLV_ON_MASK;
            break;
        case SPISLV_IRQ_POWEROFF_IRQ_MASK:
            status.interrupt_status = HAL_SPI_SLAVE_EVENT_POWER_OFF;
            spi_slave_update_status(PWROFF_STA);
            last_fsm_status = g_last2now_status[0];
            fsm_state = g_spi_slave_fsm[last_fsm_status][POWER_OFF_CMD];
            /* clear slv_on bit here */
            spi_slave_register[spi_port]->SPISLV_STATUS &= (~SPISLV_STATUS_SLV_ON_MASK);
            /* turn off the clock gating */
            hal_clock_disable(g_slvx_cg_code[spi_port]);
#ifdef HAL_SLEEP_MANAGER_ENABLED
            hal_sleep_manager_unlock_sleep(spis_sleep_handler[spi_port]);
#endif
            break;
        case SPISLV_IRQ_CRD_FINISH_IRQ_MASK:
            status.interrupt_status = HAL_SPI_SLAVE_EVENT_CRD_FINISH;
            spi_slave_update_status(CR_STA);
            last_fsm_status = g_last2now_status[0];
            fsm_state = g_spi_slave_fsm[last_fsm_status][CONFIG_READ_CMD];
            break;
        case SPISLV_IRQ_CWR_FINISH_IRQ_MASK:
            status.interrupt_status = HAL_SPI_SLAVE_EVENT_CWR_FINISH;
            spi_slave_update_status(CW_STA);
            last_fsm_status = g_last2now_status[0];
            fsm_state = g_spi_slave_fsm[last_fsm_status][CONFIG_WRITE_CMD];
            break;
        case SPISLV_IRQ_RD_FINISH_IRQ_MASK:
            status.interrupt_status = HAL_SPI_SLAVE_EVENT_RD_FINISH;
            spi_slave_update_status(PWRON_STA);
            last_fsm_status = g_last2now_status[0];
            fsm_state = g_spi_slave_fsm[last_fsm_status][READ_CMD];
            /* clear TX_DMA_SW_READY bit here */
            spi_slave_register[spi_port]->SPISLV_CTRL &= (~SPISLV_CTRL_TX_DMA_SW_READY_MASK);
            break;
        case SPISLV_IRQ_WR_FINISH_IRQ_MASK:
            status.interrupt_status = HAL_SPI_SLAVE_EVENT_WR_FINISH;
            spi_slave_update_status(PWRON_STA);
            last_fsm_status = g_last2now_status[0];
            fsm_state = g_spi_slave_fsm[last_fsm_status][WRITE_CMD];
            /* clear RX_DMA_SW_READY bit here */
            spi_slave_register[spi_port]->SPISLV_CTRL &= (~SPISLV_CTRL_RX_DMA_SW_READY_MASK);
            break;
        case SPISLV_IRQ_RD_ERR_IRQ_MASK:
        case SPISLV_IRQ_WR_ERR_IRQ_MASK:
        case SPISLV_IRQ_TIMEOUT_ERR_IRQ_MASK:
            if (irq_status == SPISLV_IRQ_RD_ERR_IRQ_MASK) {
                status.interrupt_status = HAL_SPI_SLAVE_EVENT_RD_ERR;
            } else if (irq_status == SPISLV_IRQ_WR_ERR_IRQ_MASK) {
                status.interrupt_status = HAL_SPI_SLAVE_EVENT_WR_ERR;
            } else {
                status.interrupt_status = HAL_SPI_SLAVE_EVENT_TIMEOUT_ERR;
            }
            spi_slave_update_status(PWRON_STA);
            /* clear TX/RX_DMA_SW_READY bit here */
            spi_slave_register[spi_port]->SPISLV_CTRL &= (~(SPISLV_CTRL_RX_DMA_SW_READY_MASK | SPISLV_CTRL_TX_DMA_SW_READY_MASK));
            break;
        default:
            break;
    }
    status.fsm_status = fsm_state;
    user_callback(status, user_data);
}

void spi_slave_init(hal_spi_slave_port_t spi_port, const hal_spi_slave_config_t *spi_config)
{
    /* reset internal state and register value */
    spi_slave_register[spi_port]->SPISLV_SW_RST = SLV_DEFAULT_RST;
    spi_slave_register[spi_port]->SPISLV_SW_RST = SLV_DEFAULT_ZERO;
    spi_slave_register[spi_port]->SPISLV_CTRL = SLV_DEFAULT_CTRL;
    spi_slave_register[spi_port]->SPISLV_MISO_EARLY_HALF_SCK = SLV_DEFAULT_ZERO;
    spi_slave_register[spi_port]->SPISLV_CMD_DEFINE0 = SLV_CMD_DEFINE0;
    spi_slave_register[spi_port]->SPISLV_CMD_DEFINE1 = SLV_CMD_DEFINE1;
    /* user configure parameters */
    switch (spi_config->bit_order) {
        case HAL_SPI_SLAVE_LSB_FIRST:
            spi_slave_register[spi_port]->SPISLV_CTRL &= (~(SPISLV_CTRL_RXMSBF_MASK | SPISLV_CTRL_TXMSBF_MASK));
            break;
        case HAL_SPI_SLAVE_MSB_FIRST:
            spi_slave_register[spi_port]->SPISLV_CTRL |= (SPISLV_CTRL_RXMSBF_MASK | SPISLV_CTRL_TXMSBF_MASK);
            break;
    }

    switch (spi_config->phase) {
        case HAL_SPI_SLAVE_CLOCK_PHASE0:
            spi_slave_register[spi_port]->SPISLV_CTRL &= (~SPISLV_CTRL_CPHA_MASK);
            break;
        case HAL_SPI_SLAVE_CLOCK_PHASE1:
            spi_slave_register[spi_port]->SPISLV_CTRL |= SPISLV_CTRL_CPHA_MASK;
            break;
    }

    switch (spi_config->polarity) {
        case HAL_SPI_SLAVE_CLOCK_POLARITY0:
            spi_slave_register[spi_port]->SPISLV_CTRL &= (~SPISLV_CTRL_CPOL_MASK);
            break;
        case HAL_SPI_SLAVE_CLOCK_POLARITY1:
            spi_slave_register[spi_port]->SPISLV_CTRL |= SPISLV_CTRL_CPOL_MASK;
            break;
    }
    /* timeout threshold */
    spi_slave_register[spi_port]->SPISLV_TIMOUT_THR = spi_config->timeout_threshold;

    /* enable all interrupt
       set four-byte address and size
       set sw decode bit
    */
    spi_slave_register[spi_port]->SPISLV_CTRL |= SPISLV_CTRL_INTERRUPT_ALL_MASK;
    spi_slave_register[spi_port]->SPISLV_CTRL |= SPISLV_CTRL_SIZE_MASK;
    spi_slave_register[spi_port]->SPISLV_CTRL |= SPISLV_CTRL_SW_DECODE_MASK;

}

hal_spi_slave_status_t spi_slave_send(hal_spi_slave_port_t spi_port, const uint8_t *data, uint32_t size)
{
    uint32_t config_size = 0;

    /* return HAL_SPI_SLAVE_STATUS_ERROR if config_size isn't equal to size */
    config_size = spi_slave_register[spi_port]->SPISLV_TRANS_LENGTH;
    if (config_size != size) {
        log_hal_error("[SPIS%d][send]:size error.\r\n", spi_port);
        return HAL_SPI_SLAVE_STATUS_ERROR;
    } else {
        /* set src_buffer_addr as data;
           set buffer_size as size;
           set tx_fifo_ready;
         */
        spi_slave_register[spi_port]->SPISLV_BUFFER_BASE_ADDR = (uint32_t)data;
        spi_slave_register[spi_port]->SPISLV_BUFFER_SIZE = size;
        spi_slave_register[spi_port]->SPISLV_CTRL |= SPISLV_CTRL_TX_DMA_SW_READY_MASK;
    }
    return HAL_SPI_SLAVE_STATUS_OK;
}

hal_spi_slave_status_t spi_slave_query_config_info(hal_spi_slave_port_t spi_port, uint32_t *address, uint32_t *length)
{
    if ((spi_slave_register[spi_port]->SPISLV_STATUS & 0xff) != (SPISLV_STATUS_CFG_SUCCESS_MASK | SPISLV_STATUS_SLV_ON_MASK)) {
        return HAL_SPI_SLAVE_STATUS_ERROR;
    }

    *address = spi_slave_register[spi_port]->SPISLV_TRANS_ADDR;
    *length = spi_slave_register[spi_port]->SPISLV_TRANS_LENGTH;

    return HAL_SPI_SLAVE_STATUS_OK;
}

hal_spi_slave_status_t spi_slave_receive(hal_spi_slave_port_t spi_port, uint8_t *buffer, uint32_t size)
{
    uint32_t config_size = 0;

    /* return HAL_SPI_SLAVE_STATUS_ERROR if config_size isn't equal to size */
    config_size = spi_slave_register[spi_port]->SPISLV_TRANS_LENGTH;
    if (config_size != size) {
        log_hal_error("[SPIS%d][receive]:size error.\r\n", spi_port);
        return HAL_SPI_SLAVE_STATUS_ERROR;
    } else {
        /* set src_buffer_addr as buffer;
           set buffer_size as size;
           set rx_fifo_ready;
         */
        spi_slave_register[spi_port]->SPISLV_BUFFER_BASE_ADDR = (uint32_t)buffer;
        spi_slave_register[spi_port]->SPISLV_BUFFER_SIZE = size;
        spi_slave_register[spi_port]->SPISLV_CTRL |= SPISLV_CTRL_RX_DMA_SW_READY_MASK;
    }
    return HAL_SPI_SLAVE_STATUS_OK;
}

void spi_slave_set_early_miso(hal_spi_slave_port_t spi_port, hal_spi_slave_early_miso_t early_miso)
{
    switch (early_miso) {
        case HAL_SPI_SLAVE_EARLY_MISO_DISABLE:
            spi_slave_register[spi_port]->SPISLV_MISO_EARLY_HALF_SCK &= (~SPISLV_MISO_EARLY_HALF_SCK_MASK);
            break;
        case HAL_SPI_SLAVE_EARLY_MISO_ENABLE:
            spi_slave_register[spi_port]->SPISLV_MISO_EARLY_HALF_SCK |= SPISLV_MISO_EARLY_HALF_SCK_MASK;
            break;
    }
}

void spi_slave_set_command(hal_spi_slave_port_t spi_port, hal_spi_slave_command_type_t command, uint8_t value)
{
    switch (command) {
        case HAL_SPI_SLAVE_CMD_WS:
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE0 &= (~SPISLV_CMD_WS_MASK);
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE0 |= (value << SPISLV_CMD_WS_OFFSET);
            break;
        case HAL_SPI_SLAVE_CMD_RS:
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE0 &= (~SPISLV_CMD_RS_MASK);
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE0 |= (value << SPISLV_CMD_RS_OFFSET);
            break;
        case HAL_SPI_SLAVE_CMD_WR:
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE0 &= (~SPISLV_CMD_WR_MASK);
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE0 |= (value << SPISLV_CMD_WR_OFFSET);
            break;
        case HAL_SPI_SLAVE_CMD_RD:
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE0 &= (~SPISLV_CMD_RD_MASK);
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE0 |= (value << SPISLV_CMD_RD_OFFSET);
            break;
        case HAL_SPI_SLAVE_CMD_POWEROFF:
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE1 &= (~SPISLV_CMD_POWEROFF_MASK);
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE1 |= (value << SPISLV_CMD_POWEROFF_OFFSET);
            break;
        case HAL_SPI_SLAVE_CMD_POWERON:
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE1 &= (~SPISLV_CMD_POWERON_MASK);
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE1 |= (value << SPISLV_CMD_POWERON_OFFSET);
            break;
        case HAL_SPI_SLAVE_CMD_CW:
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE1 &= (~SPISLV_CMD_CW_MASK);
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE1 |= (value << SPISLV_CMD_CW_OFFSET);
            break;
        case HAL_SPI_SLAVE_CMD_CR:
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE1 &= (~SPISLV_CMD_CR_MASK);
            spi_slave_register[spi_port]->SPISLV_CMD_DEFINE1 |= (value << SPISLV_CMD_CR_OFFSET);
            break;
    }
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
void spi_slave_backup_all_register(void)
{
    uint32_t i = 0;

    for (i = 0; i < HAL_SPI_SLAVE_MAX; i++) {
        g_spi_slave_ctrl_reg[i] = spi_slave_register[i]->SPISLV_CTRL;
        g_spi_slave_timout_reg[i] = spi_slave_register[i]->SPISLV_TIMOUT_THR;
        g_spi_slave_early_half_sck_reg[i] = spi_slave_register[i]->SPISLV_MISO_EARLY_HALF_SCK;
        g_spi_slave_cmd_def0_reg[i] = spi_slave_register[i]->SPISLV_CMD_DEFINE0;
        g_spi_slave_cmd_def1_reg[i] = spi_slave_register[i]->SPISLV_CMD_DEFINE1;
    }
}


void spi_slave_restore_all_register(void)
{
    uint32_t i = 0;

    for (i = 0; i < HAL_SPI_SLAVE_MAX; i++) {
        spi_slave_register[i]->SPISLV_CTRL = g_spi_slave_ctrl_reg[i];
        spi_slave_register[i]->SPISLV_TIMOUT_THR = g_spi_slave_timout_reg[i];
        spi_slave_register[i]->SPISLV_MISO_EARLY_HALF_SCK = g_spi_slave_early_half_sck_reg[i];
        spi_slave_register[i]->SPISLV_CMD_DEFINE0 = g_spi_slave_cmd_def0_reg[i];
        spi_slave_register[i]->SPISLV_CMD_DEFINE1 = g_spi_slave_cmd_def1_reg[i];
    }
}
#endif

#endif /*HAL_SPI_SLAVE_MODULE_ENABLED*/

