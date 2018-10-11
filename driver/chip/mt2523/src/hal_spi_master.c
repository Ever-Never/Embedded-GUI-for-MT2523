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
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_clock.h"
#include "hal_cache.h"
#include "hal_cache_internal.h"
#include "hal_log.h"
//#include <assert.h>
#include "hal_sleep_manager.h"
//#include "hal_dvfs_internal.h"

static const IRQn_Type g_spix_irq_code[HAL_SPI_MASTER_MAX] = {HAL_SPI_IRQ_TABLE};
static const hal_clock_cg_id g_spix_cg_code[HAL_SPI_MASTER_MAX] = {HAL_SPI_CG_TABLE};
static const uint16_t g_spix_pri_code[HAL_SPI_MASTER_MAX] = {HAL_SPI_PRI_TABLE};
static hal_spi_master_callback_t g_spix_callback[HAL_SPI_MASTER_MAX] = {NULL};
static void *g_spix_user_data[HAL_SPI_MASTER_MAX] = {NULL};
volatile static uint8_t g_spi_master_status[HAL_SPI_MASTER_MAX] = {0};

#ifdef HAL_SLEEP_MANAGER_ENABLED
uint8_t spim_sleep_handler[HAL_SPI_MASTER_MAX] = {0};
static uint32_t spim_cfg0_reg[HAL_SPI_MASTER_MAX] = {0};
static uint32_t spim_cfg1_reg[HAL_SPI_MASTER_MAX] = {0};
static uint32_t spim_cmd_reg[HAL_SPI_MASTER_MAX] = {0};
static uint32_t spim_pad_macro_sel_reg[HAL_SPI_MASTER_MAX] = {0};
static uint32_t spim_cfg2_reg[HAL_SPI_MASTER_MAX] = {0};
static char *spim_lock_sleep_name[HAL_SPI_MASTER_MAX] = {"SPIM0", "SPIM1", "SPIM2", "SPIM3"};
#endif
/*
#ifdef HAL_DVFS_MODULE_ENABLED
dvfs_lock_t spim_dvfs_lock = {
    .domain = "VCORE",
    .addressee = "SPIM_DVFS",
};
#endif
*/
extern SPI_MASTER_REGISTER_T *const spi_master_register[HAL_SPI_MASTER_MAX];
/* internal parameter check functions */
static bool is_master_port(hal_spi_master_port_t master_port)
{
    return (master_port < HAL_SPI_MASTER_MAX);
}

static bool is_master_config(const hal_spi_master_config_t *spi_config)
{
    bool ret = true;

    ret &= (((spi_config->clock_frequency) >= HAL_SPI_MASTER_CLOCK_MIN_FREQUENCY) && \
            ((spi_config->clock_frequency) <= HAL_SPI_MASTER_CLOCK_MAX_FREQUENCY));

    ret &= (((spi_config->bit_order) == HAL_SPI_MASTER_LSB_FIRST) || \
            ((spi_config->bit_order) == HAL_SPI_MASTER_MSB_FIRST));

    ret &= (((spi_config->polarity) == HAL_SPI_MASTER_CLOCK_POLARITY0) || \
            ((spi_config->polarity) == HAL_SPI_MASTER_CLOCK_POLARITY1));

    ret &= (((spi_config->phase) == HAL_SPI_MASTER_CLOCK_PHASE0) || \
            ((spi_config->phase) == HAL_SPI_MASTER_CLOCK_PHASE1));

    ret &= (((spi_config->phase) == HAL_SPI_MASTER_CLOCK_PHASE0) || \
            ((spi_config->phase) == HAL_SPI_MASTER_CLOCK_PHASE1));

    ret &= ((spi_config->slave_port) < HAL_SPI_MASTER_SLAVE_MAX);

    return ret;
}

#ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG
static bool is_master_advanced_config(const hal_spi_master_advanced_config_t *advanced_config)
{
    bool ret = true;

    ret &= (((advanced_config->byte_order) == HAL_SPI_MASTER_LITTLE_ENDIAN) || \
            ((advanced_config->byte_order) == HAL_SPI_MASTER_BIG_ENDIAN));

    ret &= (((advanced_config->chip_polarity) == HAL_SPI_MASTER_CHIP_SELECT_LOW) || \
            ((advanced_config->chip_polarity) == HAL_SPI_MASTER_CHIP_SELECT_HIGH));

    ret &= ((advanced_config->get_tick) <= HAL_SPI_MASTER_GET_TICK_DELAY7);

    ret &= (((advanced_config->sample_select) == HAL_SPI_MASTER_SAMPLE_POSITIVE) || \
            ((advanced_config->sample_select) == HAL_SPI_MASTER_SAMPLE_NEGATIVE));

    return ret;
}
#endif

#ifdef HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING
static bool is_master_chip_select_timing(hal_spi_master_chip_select_timing_t chip_select_timing)
{
    bool ret = true;

    ret &= ((chip_select_timing.chip_select_hold_count) <= SPI_CFG0_CS_HOLD_COUNT_MASK);

    ret &= ((chip_select_timing.chip_select_setup_count) <= SPI_CFG0_CS_HOLD_COUNT_MASK);

    ret &= ((chip_select_timing.chip_select_idle_count) <= SPI_CFG1_CS_IDLE_COUNT_MASK);

    return ret;
}
#endif

static void hal_spi_master_isr(hal_nvic_irq_t irq_number)
{
    hal_spi_master_port_t spi_port = HAL_SPI_MASTER_0;
    uint32_t i;
    for (i = 0; i < HAL_SPI_MASTER_MAX; i++) {
        if (g_spix_irq_code[i] == irq_number) {
            spi_port = (hal_spi_master_port_t)i;
            break;
        }
    }

    spi_isr_handler(spi_port, g_spix_callback[spi_port], g_spix_user_data[spi_port]);
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
void spim_backup_register_callback(void)
{
    uint32_t spi_port = 0;

    for (spi_port = HAL_SPI_MASTER_0; spi_port < HAL_SPI_MASTER_MAX; spi_port++) {
        /* backup related spim register values */
        spim_cfg0_reg[spi_port] = spi_master_register[spi_port]->SPI_CFG0;
        spim_cfg1_reg[spi_port] = spi_master_register[spi_port]->SPI_CFG1;
        spim_cmd_reg[spi_port] = spi_master_register[spi_port]->SPI_CMD;
        spim_pad_macro_sel_reg[spi_port] = spi_master_register[spi_port]->SPI_PAD_MACRO_SEL;
        spim_cfg2_reg[spi_port] = spi_master_register[spi_port]->SPI_CFG2;
    }
}

void spim_restore_register_callback(void)
{
    uint32_t spi_port = 0;

    for (spi_port = HAL_SPI_MASTER_0; spi_port < HAL_SPI_MASTER_MAX; spi_port++) {
        /* restore related spim register values */
        spi_master_register[spi_port]->SPI_CFG0 = spim_cfg0_reg[spi_port];
        spi_master_register[spi_port]->SPI_CFG1 = spim_cfg1_reg[spi_port];
        spi_master_register[spi_port]->SPI_CMD = spim_cmd_reg[spi_port];
        spi_master_register[spi_port]->SPI_PAD_MACRO_SEL = spim_pad_macro_sel_reg[spi_port];
        spi_master_register[spi_port]->SPI_CFG2 = spim_cfg2_reg[spi_port];
    }
}
#endif

hal_spi_master_status_t hal_spi_master_init(hal_spi_master_port_t master_port,
        hal_spi_master_config_t *spi_config)
{
    hal_spi_master_status_t busy_status;

    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (!is_master_config(spi_config)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    /* thread safe protect */
    SPI_CHECK_AND_SET_BUSY(master_port, busy_status);
    if (HAL_SPI_MASTER_STATUS_ERROR_BUSY == busy_status) {
        log_hal_error("[SPIM%d][init]:busy.\r\n", master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    if (hal_clock_is_enabled(g_spix_cg_code[master_port]) != true) {
        if (HAL_CLOCK_STATUS_OK != hal_clock_enable(g_spix_cg_code[master_port])) {
            log_hal_error("[SPIM%d] Clock enable failed!\r\n", master_port);
            return HAL_SPI_MASTER_STATUS_ERROR;
        }
    }

    /* set parameter to registers */
    spi_master_init(master_port, spi_config);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    spim_sleep_handler[master_port] = hal_sleep_manager_set_sleep_handle(spim_lock_sleep_name[master_port]);
#endif
    return HAL_SPI_MASTER_STATUS_OK;
}

#ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG
hal_spi_master_status_t hal_spi_master_set_advanced_config(hal_spi_master_port_t master_port,
        hal_spi_master_advanced_config_t *advanced_config)
{
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (!is_master_advanced_config(advanced_config)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_get_status(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_master_set_advanced_config(master_port, advanced_config);

    return HAL_SPI_MASTER_STATUS_OK;
}
#endif

hal_spi_master_status_t hal_spi_master_deinit(hal_spi_master_port_t master_port)
{
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }

    /* disable interrupt */
    hal_nvic_disable_irq(g_spix_irq_code[master_port]);

    /* reset state to idle */
    SPI_SET_IDLE(master_port);

    /* clear callback */
    g_spix_callback[master_port] = NULL;

    hal_clock_disable(g_spix_cg_code[master_port]);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_release_sleep_handle(spim_sleep_handler[master_port]);
#endif
    return HAL_SPI_MASTER_STATUS_OK;
}

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
hal_spi_master_status_t hal_spi_master_register_callback(hal_spi_master_port_t master_port,
        hal_spi_master_callback_t callback,
        void *user_data)
{
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == callback) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    g_spix_callback[master_port] = callback;
    /* user_data can be as NULL */
    g_spix_user_data[master_port] = user_data;

    /* register NVIC */
    hal_nvic_register_isr_handler(g_spix_irq_code[master_port], hal_spi_master_isr);
    hal_nvic_set_priority(g_spix_irq_code[master_port], g_spix_pri_code[master_port]);
    hal_nvic_enable_irq(g_spix_irq_code[master_port]);

    return HAL_SPI_MASTER_STATUS_OK;
}
#endif

hal_spi_master_status_t hal_spi_master_send_polling(hal_spi_master_port_t master_port,
        uint8_t *data,
        uint32_t size)
{
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;

    if (!is_master_port(master_port)) {
        status = HAL_SPI_MASTER_STATUS_ERROR_PORT;
        return status;
    }
    if (NULL == data) {
        status = HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
        return status;
    }
    if ((size <= 0) || (size > HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE)) {
        status = HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
        return status;
    }

    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_get_status(master_port)) {
        log_hal_error("[SPIM%d][send_polling]:busy.\r\n", master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_set_mode(master_port, SPI_TX, SPI_MODE_FIFO);
    spi_set_mode(master_port, SPI_RX, SPI_MODE_FIFO);
    spi_set_interrupt(master_port, false);
    spi_clear_fifo(master_port);
    /* HW require configure transfer length first */
    status = spi_analyse_transfer_size(master_port, size);
    if (HAL_SPI_MASTER_STATUS_OK != status) {
        return status;
    }
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_lock_sleep(spim_sleep_handler[master_port]);
#endif
    status = spi_push_data(master_port, data, size);
    if (HAL_SPI_MASTER_STATUS_OK != status) {
        return status;
    }
    /*
    #ifdef HAL_DVFS_MODULE_ENABLED
        dvfs_lock(&spim_dvfs_lock);
    #endif
    */
    spi_start_transfer(master_port, SPI_MODE_FIFO);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_unlock_sleep(spim_sleep_handler[master_port]);
#endif
    /*
    #ifdef HAL_DVFS_MODULE_ENABLED
        dvfs_unlock(&spim_dvfs_lock);
    #endif
    */
    return status;
}

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
hal_spi_master_status_t hal_spi_master_send_dma(hal_spi_master_port_t master_port,
        uint8_t *data,
        uint32_t size)
{
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;

    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == data) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    /* the address for DMA buffer must be 4 bytes aligned */
    if (((uint32_t)data % 4) > 0) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (size <= 0) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#ifdef HAL_CACHE_MODULE_ENABLED
    if (true == hal_cache_is_cacheable((uint32_t)data)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#endif
    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_get_status(master_port)) {
        log_hal_error("[SPIM%d][send_dma]:busy.\r\n", master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_set_rwaddr(master_port, SPI_TX, data);
    spi_set_mode(master_port, SPI_TX, SPI_MODE_DMA);
    spi_set_mode(master_port, SPI_RX, SPI_MODE_FIFO);
    spi_set_interrupt(master_port, true);
    status = spi_analyse_transfer_size(master_port, size);
    if (status != HAL_SPI_MASTER_STATUS_OK) {
        return status;
    } else {
#ifdef HAL_SLEEP_MANAGER_ENABLED
        hal_sleep_manager_lock_sleep(spim_sleep_handler[master_port]);
#endif
        /*
        #ifdef HAL_DVFS_MODULE_ENABLED
                dvfs_lock(&spim_dvfs_lock);
        #endif
        */
        spi_start_transfer(master_port, SPI_MODE_DMA);
        g_spi_direction[master_port] = SPI_TX;
    }

    return status;
}

hal_spi_master_status_t hal_spi_master_send_dma_blocking(hal_spi_master_port_t master_port,
        uint8_t *data,
        uint32_t size)
{
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;

    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == data) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    /* the address for DMA buffer must be 4 bytes aligned */
    if (((uint32_t)data % 4) > 0) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (size <= 0) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#ifdef HAL_CACHE_MODULE_ENABLED
    if (true == hal_cache_is_cacheable((uint32_t)data)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#endif
    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_get_status(master_port)) {
        log_hal_error("[SPIM%d][send_dma]:busy.\r\n", master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_set_rwaddr(master_port, SPI_TX, data);
    spi_set_mode(master_port, SPI_TX, SPI_MODE_DMA);
    spi_set_mode(master_port, SPI_RX, SPI_MODE_FIFO);
    spi_set_interrupt(master_port, false);
    status = spi_analyse_transfer_size(master_port, size);
    if (status != HAL_SPI_MASTER_STATUS_OK) {
        return status;
    } else {
        spi_start_transfer_dma_blocking(master_port);
    }

    return status;
}
#endif

hal_spi_master_status_t hal_spi_master_send_and_receive_polling(hal_spi_master_port_t master_port,
        hal_spi_master_send_and_receive_config_t *spi_send_and_receive_config)
{
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;

    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == (spi_send_and_receive_config->receive_buffer)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (NULL == (spi_send_and_receive_config->send_data)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (((spi_send_and_receive_config->receive_length) <= 0) ||
            ((spi_send_and_receive_config->receive_length) > HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (((spi_send_and_receive_config->send_length) <= 0) ||
            ((spi_send_and_receive_config->send_length) > HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_get_status(master_port)) {
        log_hal_error("[SPIM%d][send_and_receive_polling]:busy.\r\n", master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_set_mode(master_port, SPI_TX, SPI_MODE_FIFO);
    spi_set_mode(master_port, SPI_RX, SPI_MODE_FIFO);
    spi_set_interrupt(master_port, false);
    spi_clear_fifo(master_port);
    /* receive_length is the number of bytes received after sending data
       finished plus the number of bytes to send */
    status = spi_analyse_transfer_size(master_port, spi_send_and_receive_config->receive_length);
    if (HAL_SPI_MASTER_STATUS_OK != status) {
        return status;
    }
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_lock_sleep(spim_sleep_handler[master_port]);
#endif
    status = spi_push_data(master_port, spi_send_and_receive_config->send_data, spi_send_and_receive_config->send_length);
    if (HAL_SPI_MASTER_STATUS_OK != status) {
        return status;
    }
    /*
    #ifdef HAL_DVFS_MODULE_ENABLED
        dvfs_lock(&spim_dvfs_lock);
    #endif
    */
    spi_start_transfer(master_port, SPI_MODE_FIFO);

    status = spi_pop_data(master_port, spi_send_and_receive_config->receive_buffer, spi_send_and_receive_config->receive_length);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_unlock_sleep(spim_sleep_handler[master_port]);
#endif
    /*
    #ifdef HAL_DVFS_MODULE_ENABLED
        dvfs_unlock(&spim_dvfs_lock);
    #endif
    */
    return status;
}

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
hal_spi_master_status_t hal_spi_master_send_and_receive_dma(hal_spi_master_port_t master_port,
        hal_spi_master_send_and_receive_config_t *spi_send_and_receive_config)
{
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;

    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == (spi_send_and_receive_config->receive_buffer)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (NULL == (spi_send_and_receive_config->send_data)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    /* the address for DMA buffer must be 4 bytes aligned */
    if ((((uint32_t)(spi_send_and_receive_config->receive_buffer) % 4) > 0) ||
            (((uint32_t)(spi_send_and_receive_config->send_data) % 4) > 0)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->receive_length) <= 0) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->send_length) <= 0) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#ifdef HAL_CACHE_MODULE_ENABLED
    if (true == hal_cache_is_cacheable((uint32_t)(spi_send_and_receive_config->receive_buffer)) ||
            true == hal_cache_is_cacheable((uint32_t)(spi_send_and_receive_config->send_data))
       ) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#endif
    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_get_status(master_port)) {
        log_hal_error("[SPIM%d][send_and_receive_dma]:busy.\r\n", master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_set_rwaddr(master_port, SPI_TX, spi_send_and_receive_config->send_data);
    spi_set_rwaddr(master_port, SPI_RX, spi_send_and_receive_config->receive_buffer);
    spi_set_mode(master_port, SPI_RX, SPI_MODE_DMA);
    spi_set_mode(master_port, SPI_TX, SPI_MODE_DMA);
    spi_set_interrupt(master_port, true);
    status = spi_analyse_transfer_size(master_port, spi_send_and_receive_config->receive_length);
    if (status != HAL_SPI_MASTER_STATUS_OK) {
        return status;
    } else {
#ifdef HAL_SLEEP_MANAGER_ENABLED
        hal_sleep_manager_lock_sleep(spim_sleep_handler[master_port]);
#endif
        /*
        #ifdef HAL_DVFS_MODULE_ENABLED
                dvfs_lock(&spim_dvfs_lock);
        #endif
        */
        spi_start_transfer(master_port, SPI_MODE_DMA);
        g_spi_direction[master_port] = SPI_RX;
    }

    return status;
}

hal_spi_master_status_t hal_spi_master_send_and_receive_dma_blocking(hal_spi_master_port_t master_port,
        hal_spi_master_send_and_receive_config_t *spi_send_and_receive_config)
{
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;

    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == (spi_send_and_receive_config->receive_buffer)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (NULL == (spi_send_and_receive_config->send_data)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    /* the address for DMA buffer must be 4 bytes aligned */
    if ((((uint32_t)(spi_send_and_receive_config->receive_buffer) % 4) > 0) ||
            (((uint32_t)(spi_send_and_receive_config->send_data) % 4) > 0)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->receive_length) <= 0) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->send_length) <= 0) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#ifdef HAL_CACHE_MODULE_ENABLED
    if (true == hal_cache_is_cacheable((uint32_t)(spi_send_and_receive_config->receive_buffer)) ||
            true == hal_cache_is_cacheable((uint32_t)(spi_send_and_receive_config->send_data))
       ) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#endif
    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_get_status(master_port)) {
        log_hal_error("[SPIM%d][send_and_receive_dma]:busy.\r\n", master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_set_rwaddr(master_port, SPI_TX, spi_send_and_receive_config->send_data);
    spi_set_rwaddr(master_port, SPI_RX, spi_send_and_receive_config->receive_buffer);
    spi_set_mode(master_port, SPI_RX, SPI_MODE_DMA);
    spi_set_mode(master_port, SPI_TX, SPI_MODE_DMA);
    spi_set_interrupt(master_port, false);
    status = spi_analyse_transfer_size(master_port, spi_send_and_receive_config->receive_length);
    if (status != HAL_SPI_MASTER_STATUS_OK) {
        return status;
    } else {
        spi_start_transfer_dma_blocking(master_port);
    }

    return status;
}
#endif

hal_spi_master_status_t hal_spi_master_get_running_status(hal_spi_master_port_t master_port,
        hal_spi_master_running_status_t *running_status)
{
    uint32_t status = 0;
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == running_status) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (hal_clock_is_enabled(g_spix_cg_code[master_port]) != true) {
        return HAL_SPI_MASTER_STATUS_ERROR;
    }

    /* get hardware status */
    status = spi_get_status(master_port);
    if (status != 0) {
        *running_status = HAL_SPI_MASTER_IDLE;
    } else {
        *running_status = HAL_SPI_MASTER_BUSY;
    }

    return HAL_SPI_MASTER_STATUS_OK;
}

#ifdef HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING
hal_spi_master_status_t hal_spi_master_set_chip_select_timing(hal_spi_master_port_t master_port,
        hal_spi_master_chip_select_timing_t chip_select_timing)
{
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (!is_master_chip_select_timing(chip_select_timing)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_get_status(master_port)) {
        log_hal_error("[SPIM%d][cs_timing]:busy.\r\n", master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_master_set_chip_select_timing(master_port, chip_select_timing);

    return HAL_SPI_MASTER_STATUS_OK;
}
#endif

#ifdef HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG
hal_spi_master_status_t hal_spi_master_set_deassert(hal_spi_master_port_t master_port,
        hal_spi_master_deassert_t deassert)
{
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (!(((deassert) == HAL_SPI_MASTER_DEASSERT_DISABLE) || \
            ((deassert) == HAL_SPI_MASTER_DEASSERT_ENABLE))) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_get_status(master_port)) {
        log_hal_error("[SPIM%d][deassert]:busy.\r\n", master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_master_set_deassert(master_port, deassert);

    return HAL_SPI_MASTER_STATUS_OK;
}
#endif

#ifdef HAL_SPI_MASTER_FEATURE_MACRO_CONFIG
hal_spi_master_status_t hal_spi_master_set_macro_selection(hal_spi_master_port_t master_port,
        hal_spi_master_macro_select_t macro_select)
{
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (!(((macro_select) == HAL_SPI_MASTER_MACRO_GROUP_A) || \
            ((macro_select) == HAL_SPI_MASTER_MACRO_GROUP_B) || \
            ((macro_select) == HAL_SPI_MASTER_MACRO_GROUP_C))) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    return HAL_SPI_MASTER_STATUS_OK;
}
#endif


#endif /* HAL_SPI_MASTER_MODULE_ENABLED */

