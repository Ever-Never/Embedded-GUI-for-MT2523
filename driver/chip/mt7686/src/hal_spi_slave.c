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
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_clock.h"
#include "hal_cache.h"
#include "hal_log.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs_internal.h"
#endif

static const IRQn_Type g_spi_slave_irq_code[HAL_SPI_SLAVE_MAX] = {HAL_SPI_SLAVE_IRQ_TABLE};
static const uint16_t g_spi_slave_pri_code[HAL_SPI_SLAVE_MAX] = {HAL_SPI_SLAVE_PRI_TABLE};
static hal_spi_slave_callback_t g_spi_slave_callback[HAL_SPI_SLAVE_MAX] = {NULL};
static void *g_spi_slave_user_data[HAL_SPI_SLAVE_MAX] = {NULL};
static volatile uint8_t g_spi_slave_status[HAL_SPI_SLAVE_MAX] = {0};
static const hal_clock_cg_id g_spi_slave_cg_code[HAL_SPI_SLAVE_MAX] = {HAL_CLOCK_CG_SPISLV};
#ifdef HAL_DVFS_MODULE_ENABLED
static dvfs_lock_t g_spi_slave_dvfs_lock = {
    .domain = "VCORE",
    .addressee = "SPIS_DVFS",
};
#endif

extern uint8_t g_last2now_status[2];

static bool is_slave_port(hal_spi_slave_port_t spi_port)
{
    return (spi_port < HAL_SPI_SLAVE_MAX);
}

static bool is_slave_config(const hal_spi_slave_config_t *spi_configure)
{
    bool ret = true;

    ret &= (((spi_configure->bit_order) == HAL_SPI_SLAVE_LSB_FIRST) ||
            ((spi_configure->bit_order) == HAL_SPI_SLAVE_MSB_FIRST));

    ret &= (((spi_configure->polarity) == HAL_SPI_SLAVE_CLOCK_POLARITY0) ||
            ((spi_configure->polarity) == HAL_SPI_SLAVE_CLOCK_POLARITY1));

    ret &= (((spi_configure->phase) == HAL_SPI_SLAVE_CLOCK_PHASE0) ||
            ((spi_configure->phase) == HAL_SPI_SLAVE_CLOCK_PHASE1));

    return ret;
}

static void spi_slave_isr(hal_nvic_irq_t irq_number)
{
    uint32_t i;
    hal_spi_slave_port_t spi_port = HAL_SPI_SLAVE_0;

    for (i = 0; i < HAL_SPI_SLAVE_MAX; i++) {
        if (g_spi_slave_irq_code[i] == irq_number) {
            spi_port = (hal_spi_slave_port_t)i;
            break;
        }
    }

    spi_slave_lisr(spi_port, g_spi_slave_callback[spi_port], g_spi_slave_user_data[spi_port]);
}

hal_spi_slave_status_t hal_spi_slave_init(hal_spi_slave_port_t spi_port, hal_spi_slave_config_t *spi_configure)
{
    hal_spi_slave_status_t busy_status;

    if (!is_slave_port(spi_port)) {
        return HAL_SPI_SLAVE_STATUS_ERROR_PORT;
    }
    if (!is_slave_config(spi_configure)) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }

    /* thread safe protect */
    SPI_SLAVE_CHECK_AND_SET_BUSY(spi_port, busy_status);
    if (HAL_SPI_SLAVE_STATUS_ERROR_BUSY == busy_status) {
        log_hal_error("[SPIS%d][init]:busy.\r\n", spi_port);
        return HAL_SPI_SLAVE_STATUS_ERROR_BUSY;
    }

    if (hal_clock_is_enabled(g_spi_slave_cg_code[spi_port]) != true) {
        if (HAL_CLOCK_STATUS_OK != hal_clock_enable(g_spi_slave_cg_code[spi_port])) {
            log_hal_error("[SPIS%d] Clock enable failed!\r\n", spi_port);
            return HAL_SPI_SLAVE_STATUS_ERROR;
        }
    }

    spi_slave_reset_default(spi_port);

    spi_slave_init(spi_port, spi_configure);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_SPI_SLAVE, (sleep_management_suspend_callback_t)spi_slave_backup_register_callback, NULL);
    sleep_management_register_resume_callback(SLEEP_BACKUP_RESTORE_SPI_SLAVE, (sleep_management_resume_callback_t)spi_slave_restore_register_callback, NULL);
#endif

#ifdef HAL_DVFS_MODULE_ENABLED
    dvfs_lock(&g_spi_slave_dvfs_lock);
#endif

    return HAL_SPI_SLAVE_STATUS_OK;
}

hal_spi_slave_status_t hal_spi_slave_deinit(hal_spi_slave_port_t spi_port)
{
    if (!is_slave_port(spi_port)) {
        return HAL_SPI_SLAVE_STATUS_ERROR_PORT;
    }

    spi_slave_reset_default(spi_port);

    hal_nvic_disable_irq(g_spi_slave_irq_code[spi_port]);

    /* reset state to idle */
    SPI_SLAVE_SET_IDLE(spi_port);

    /* reset slave fsm */
    g_last2now_status[0] = PWROFF_STA;
    g_last2now_status[1] = PWROFF_STA;

    /* turn off the clock gating */
    hal_clock_disable(g_spi_slave_cg_code[spi_port]);

#ifdef HAL_DVFS_MODULE_ENABLED
    dvfs_unlock(&g_spi_slave_dvfs_lock);
#endif

    return HAL_SPI_SLAVE_STATUS_OK;
}

hal_spi_slave_status_t hal_spi_slave_register_callback(hal_spi_slave_port_t spi_port,
        hal_spi_slave_callback_t callback,
        void *user_data)
{
    if (!is_slave_port(spi_port)) {
        return HAL_SPI_SLAVE_STATUS_ERROR_PORT;
    }
    if (NULL == callback) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }

    /* register lisr to nvic */
    g_spi_slave_callback[spi_port] = callback;
    g_spi_slave_user_data[spi_port] = user_data;
    hal_nvic_register_isr_handler(g_spi_slave_irq_code[spi_port], spi_slave_isr);
    hal_nvic_set_priority(g_spi_slave_irq_code[spi_port], g_spi_slave_pri_code[spi_port]);
    hal_nvic_enable_irq(g_spi_slave_irq_code[spi_port]);

    return HAL_SPI_SLAVE_STATUS_OK;
}

/* this function will be called in SLV_CRD_FINISH_INT handler */
hal_spi_slave_status_t hal_spi_slave_send(hal_spi_slave_port_t spi_port, const uint8_t *data, uint32_t size)
{
    hal_spi_slave_status_t status = HAL_SPI_SLAVE_STATUS_OK;

    if (!is_slave_port(spi_port)) {
        return HAL_SPI_SLAVE_STATUS_ERROR_PORT;
    }
    if (NULL == data) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }
    if (size <= 0) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }
#ifdef HAL_CACHE_MODULE_ENABLED
    if (true == hal_cache_is_cacheable((uint32_t)data)) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }
#endif

    status = spi_slave_send(spi_port, data, size);

    return status;
}

hal_spi_slave_status_t hal_spi_slave_get_master_read_config(hal_spi_slave_port_t spi_port, uint32_t *address, uint32_t *length)
{
    if (!is_slave_port(spi_port)) {
        return HAL_SPI_SLAVE_STATUS_ERROR_PORT;
    }
    if ((address == NULL) || (length == NULL)) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }

    if (spi_slave_query_config_info(spi_port, address, length) != HAL_SPI_SLAVE_STATUS_OK) {
        return HAL_SPI_SLAVE_STATUS_ERROR;
    }

    return HAL_SPI_SLAVE_STATUS_OK;
}

/* this function will be called in SLV_CWR_FINISH_INT handler */
hal_spi_slave_status_t hal_spi_slave_receive(hal_spi_slave_port_t spi_port, uint8_t *buffer, uint32_t size)
{
    hal_spi_slave_status_t status = HAL_SPI_SLAVE_STATUS_OK;

    if (!is_slave_port(spi_port)) {
        return HAL_SPI_SLAVE_STATUS_ERROR_PORT;
    }
    if (NULL == buffer) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }
    if (size <= 0) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }
#ifdef HAL_CACHE_MODULE_ENABLED
    if (true == hal_cache_is_cacheable((uint32_t)buffer)) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }
#endif

    status = spi_slave_receive(spi_port, buffer, size);

    return status;
}

hal_spi_slave_status_t hal_spi_slave_get_master_write_config(hal_spi_slave_port_t spi_port, uint32_t *address, uint32_t *length)
{
    if (!is_slave_port(spi_port)) {
        return HAL_SPI_SLAVE_STATUS_ERROR_PORT;
    }
    if ((address == NULL) || (length == NULL)) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }

    if (spi_slave_query_config_info(spi_port, address, length) != HAL_SPI_SLAVE_STATUS_OK) {
        return HAL_SPI_SLAVE_STATUS_ERROR;
    }

    return HAL_SPI_SLAVE_STATUS_OK;
}

hal_spi_slave_status_t hal_spi_slave_set_early_miso(hal_spi_slave_port_t spi_port, hal_spi_slave_early_miso_t early_miso)
{
    if (!is_slave_port(spi_port)) {
        return HAL_SPI_SLAVE_STATUS_ERROR_PORT;
    }
    if (!(((early_miso) == HAL_SPI_SLAVE_EARLY_MISO_DISABLE) || \
            ((early_miso) == HAL_SPI_SLAVE_EARLY_MISO_ENABLE))) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }

    spi_slave_set_early_miso(spi_port, early_miso);

    return HAL_SPI_SLAVE_STATUS_OK;
}

hal_spi_slave_status_t hal_spi_slave_set_command(hal_spi_slave_port_t spi_port, hal_spi_slave_command_type_t command, uint8_t value)
{
    if (!is_slave_port(spi_port)) {
        return HAL_SPI_SLAVE_STATUS_ERROR_PORT;
    }
    if (!((command) <= HAL_SPI_SLAVE_CMD_CT)) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }
    /* command value should not be 0 or negative */
    if (value <= 0) {
        return HAL_SPI_SLAVE_STATUS_INVALID_PARAMETER;
    }

    spi_slave_set_command(spi_port, command, value);

    return HAL_SPI_SLAVE_STATUS_OK;
}

#endif /*HAL_SPI_SLAVE_MODULE_ENABLED*/

