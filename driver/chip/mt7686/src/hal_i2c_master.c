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

/* Includes ------------------------------------------------------------------*/
#include "hal_i2c_master.h"

#if defined(HAL_I2C_MASTER_MODULE_ENABLED)
#include "hal_i2c_master_internal.h"
#include "hal_pdma_internal.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_log.h"
#include "hal_clock.h"
#include <assert.h>
#include "string.h"
#include "memory_attribute.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#include "hal_gpt.h"

static void hal_i2c_master_isr(hal_nvic_irq_t irq_number);
void hal_i2c_isr_handler(hal_i2c_port_t i2c_port, hal_i2c_callback_t user_callback, void *user_data);
static void hal_i2c_master_dma_callback(pdma_event_t event, void  *user_data);
#ifdef I2C_ENABLE_POLLING_TIMEOUT
static void hal_i2c_master_wait_transaction_finish(hal_i2c_port_t i2c_port, i2c_transaction_type_t transaction_type);
static void hal_i2c_wait_dma_finish(hal_i2c_port_t i2c_port);
#endif
volatile static uint8_t s_i2c_master_status[HAL_I2C_MASTER_MAX] = {0};
static hal_i2c_callback_t s_i2c_callback[HAL_I2C_MASTER_MAX] = {NULL};
static void *s_i2c_callback_parameter[HAL_I2C_MASTER_MAX] = {NULL};

const static hal_nvic_irq_t s_i2c_irq_table[HAL_I2C_MASTER_MAX] = {HAL_I2C_IRQ_TABLE};
extern uint32_t g_i2c_source_clock_table[];

volatile static i2c_dma_callback_parameter_t s_i2c_dma_parameter[HAL_I2C_MASTER_MAX];
volatile static uint8_t i2c_dma_callback_count[HAL_I2C_MASTER_MAX] = {0, 0};


extern I2C_REGISTER_T *i2c_master_register[];
extern pdma_channel_t g_i2c_tx_dma_channel[];
extern pdma_channel_t g_i2c_rx_dma_channel[];
extern hal_clock_cg_id g_i2c_pdn_table[];

#ifdef HAL_SLEEP_MANAGER_ENABLED
ATTR_ZIDATA_IN_TCM static volatile i2c_register_backup_t i2c_register_backup[HAL_I2C_MASTER_MAX];
static sleep_management_lock_request_t i2c_lock_sleep_handle[HAL_I2C_MASTER_MAX] = {SLEEP_LOCK_I2C0, SLEEP_LOCK_I2C1};

#define I2C_LOCK_SLEEP(i2c_port) \
do{ \
    hal_sleep_manager_lock_sleep(i2c_lock_sleep_handle[i2c_port]); \
}while(0)

#define I2C_UNLOCK_SLEEP(i2c_port) \
do{ \
    hal_sleep_manager_unlock_sleep(i2c_lock_sleep_handle[i2c_port]); \
}while(0)

#else
#define I2C_LOCK_SLEEP(i2c_port)
#define I2C_UNLOCK_SLEEP(i2c_port)
#endif

#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_clock_internal.h"
static dvfs_lock_t i2c_dvfs_lock = {
    .domain = "VCORE",
    .addressee = "I2C",
};

#define I2C_DVFS_UPDATE_CLOCK()                                     \
do{                                                                 \
    uint32_t bus_clock = clock_get_freq_peri_bus();                 \
    if (62400000 == bus_clock) {                                    \
        g_i2c_source_clock_table[2] = I2C_SOURCE_CLOCK_RATE_62M;    \
        g_i2c_source_clock_table[3] = I2C_SOURCE_CLOCK_RATE_62M;    \
    } else if (13000000 == bus_clock) {                             \
        g_i2c_source_clock_table[2] = I2C_SOURCE_CLOCK_RATE_13M;    \
        g_i2c_source_clock_table[3] = I2C_SOURCE_CLOCK_RATE_13M;    \
    }                                                               \
}while(0)

#define I2C_LOCK_DVFS()                                             \
do{                                                                 \
    dvfs_lock(&i2c_dvfs_lock);                                      \
}while(0)
#define I2C_UNLOCK_DVFS()                                           \
do{                                                                 \
    dvfs_unlock(&i2c_dvfs_lock);                                    \
}while(0)

#else /* HAL_DVFS_MODULE_ENABLED */
#define I2C_DVFS_UPDATE_CLOCK()
#define I2C_LOCK_DVFS()
#define I2C_UNLOCK_DVFS()
#endif
/*****************************************************************************
* Function
*****************************************************************************/
void hal_i2c_backup_all_register(void);
void hal_i2c_restore_all_register(void);


hal_i2c_status_t hal_i2c_master_init(hal_i2c_port_t i2c_port, hal_i2c_config_t *i2c_config)
{
    hal_i2c_status_t busy_status;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }

    if (HAL_I2C_FREQUENCY_MAX <= i2c_config->frequency) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 1);
    I2C_CHECK_AND_SET_BUSY(i2c_port, busy_status);
    if (HAL_I2C_STATUS_ERROR_BUSY == busy_status) {
        log_hal_error("[I2C%d][init]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

#ifdef HAL_SLEEP_MANAGER_ENABLED
    sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_I2C, (sleep_management_suspend_callback_t)hal_i2c_backup_all_register, NULL);
    sleep_management_register_resume_callback(SLEEP_BACKUP_RESTORE_I2C, (sleep_management_resume_callback_t)hal_i2c_restore_all_register, NULL);
#endif


    hal_clock_enable(g_i2c_pdn_table[i2c_port]);

    /* i2c lock dvfs */
    I2C_LOCK_DVFS();
    /*update source clock*/
    I2C_DVFS_UPDATE_CLOCK();

    /* calculate speed to register value */
    i2c_set_frequency(i2c_port, i2c_config->frequency);

    return HAL_I2C_STATUS_OK;
}


hal_i2c_status_t hal_i2c_master_deinit(hal_i2c_port_t i2c_port)
{
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 2);
    /* disable I2C */
    I2C_STOP(i2c_port);

    /* disable IRQ */
    hal_nvic_disable_irq(s_i2c_irq_table[i2c_port]);

    /*mask interrupt*/
    I2C_MASK_INTERRUPT(i2c_port);

    /*clear callback*/
    s_i2c_callback[i2c_port] = NULL;

    /* set dma false */
    s_i2c_dma_parameter[i2c_port].dma_finish = false;

    hal_clock_disable(g_i2c_pdn_table[i2c_port]);

    /*unlock dvfs*/
    I2C_UNLOCK_DVFS();
    /* set to idle */
    I2C_SET_IDLE(i2c_port);

    return HAL_I2C_STATUS_OK;
}


hal_i2c_status_t hal_i2c_master_set_frequency(hal_i2c_port_t i2c_port, hal_i2c_frequency_t frequency)
{
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (HAL_I2C_FREQUENCY_MAX <= frequency) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }
    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][set_freq]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 3);
    /* calculate speed to register value */
    i2c_set_frequency(i2c_port, frequency);

    return HAL_I2C_STATUS_OK;
}




hal_i2c_status_t hal_i2c_master_register_callback(hal_i2c_port_t i2c_port, hal_i2c_callback_t i2c_callback, void *user_data)
{
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_callback) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 4);
    s_i2c_callback[i2c_port] = i2c_callback;
    s_i2c_callback_parameter[i2c_port] = user_data;

    /* register NVIC */
    hal_nvic_register_isr_handler(s_i2c_irq_table[i2c_port], hal_i2c_master_isr);
    hal_nvic_enable_irq(s_i2c_irq_table[i2c_port]);

    return HAL_I2C_STATUS_OK;
}


hal_i2c_status_t hal_i2c_master_send_polling(hal_i2c_port_t i2c_port, uint8_t slave_address, const uint8_t *data, uint32_t size)
{
    i2c_transaction_status_t transaction_status;
    i2c_transaction_size_t config_size;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == data || size > I2C_FIFO_DEPTH) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][send_polling]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 5);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    /*mask interrupt*/
    I2C_MASK_INTERRUPT(i2c_port);

    /*config i2c hardware*/
    config_size.send_size = size;
    config_size.receive_size = 0;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_POLLING, I2C_GET_SEND_ADDRESS(slave_address), &config_size);

    /*set data to fifo*/
    i2c_write_fifo(i2c_port, data, size);

    /*start i2c*/
    I2C_START(i2c_port);
#ifndef I2C_ENABLE_POLLING_TIMEOUT
    /* polling till transaction finish */
    while (0 == I2C_QUERY_TRANSACTION_FINISH_STATUS(i2c_port)) {
    }
#else
    hal_i2c_master_wait_transaction_finish(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_POLLING);
#endif

    /* read and status */
    transaction_status = i2c_get_transaction_status(i2c_port);

    /* unlock sleep */
    I2C_UNLOCK_SLEEP(i2c_port);


    if (I2C_TRANSACTION_SUCCESS != transaction_status) {
        log_hal_error("[I2C%d][send_polling]:status=%d\r\n", i2c_port, transaction_status);
        return HAL_I2C_STATUS_ERROR;
    } else {
        return HAL_I2C_STATUS_OK;
    }
}


hal_i2c_status_t hal_i2c_master_send_dma(hal_i2c_port_t i2c_port, uint8_t slave_address, const uint8_t *data, uint32_t size)
{
    i2c_transaction_size_t config_size;
    pdma_config_t  i2c_dma_config;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == data) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][send_dma]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 6);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);
    /* config dma hardware */
    pdma_init(g_i2c_tx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = size;
    i2c_dma_config.master_type = PDMA_TX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_tx_dma_channel[i2c_port], &i2c_dma_config);

    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_tx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);


    /*config i2c hardware*/
    config_size.send_size = size;
    config_size.receive_size = 0;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA, I2C_GET_SEND_ADDRESS(slave_address), &config_size);
    /* unmask i2c interrupt */
    I2C_UNMASK_INTERRUPT(i2c_port);

    /* start DMA */
    pdma_start_interrupt(g_i2c_tx_dma_channel[i2c_port], (uint32_t)data);
    /* start I2C */
    I2C_START(i2c_port);




    return HAL_I2C_STATUS_OK;
}


hal_i2c_status_t hal_i2c_master_receive_polling(hal_i2c_port_t i2c_port, uint8_t slave_address, uint8_t *buffer, uint32_t size)
{
    i2c_transaction_status_t transaction_status;
    i2c_transaction_size_t config_size;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == buffer || size > I2C_FIFO_DEPTH) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][receive_polling]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }
    I2C_DEBUG_RECORD_FLOW(i2c_port, 7);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);
    /*mask interrupt*/
    I2C_MASK_INTERRUPT(i2c_port);

    /*config i2c hardware*/
    config_size.send_size = 0;
    config_size.receive_size = size;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_POLLING, I2C_GET_RECEIVE_ADDRESS(slave_address), &config_size);

    /*start i2c*/
    I2C_START(i2c_port);

#ifndef I2C_ENABLE_POLLING_TIMEOUT
    /* polling till transaction finish */
    while (0 == I2C_QUERY_TRANSACTION_FINISH_STATUS(i2c_port)) {
    }
#else
    hal_i2c_master_wait_transaction_finish(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_POLLING);
#endif

    /* read and status */
    transaction_status = i2c_get_transaction_status(i2c_port);
    /* unlock sleep */
    I2C_UNLOCK_SLEEP(i2c_port);

    if (I2C_TRANSACTION_SUCCESS != transaction_status) {
        log_hal_error("[I2C%d][receive_polling]:status=%d\r\n", i2c_port, transaction_status);
        return HAL_I2C_STATUS_ERROR;
    } else {
        i2c_read_fifo(i2c_port, buffer, size);
        return HAL_I2C_STATUS_OK;
    }
}




hal_i2c_status_t hal_i2c_master_receive_dma(hal_i2c_port_t i2c_port, uint8_t slave_address, uint8_t  *buffer, uint32_t size)
{
    i2c_transaction_size_t config_size;
    pdma_config_t  i2c_dma_config;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == buffer) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][receive_dma]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 8);
    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    /* config dma hardware */
    pdma_init(g_i2c_rx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = size;
    i2c_dma_config.master_type = PDMA_RX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_rx_dma_channel[i2c_port], &i2c_dma_config);


    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_rx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);

    /*config i2c hardware*/
    config_size.send_size = 0;
    config_size.receive_size = size;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA, I2C_GET_RECEIVE_ADDRESS(slave_address), &config_size);
    /* unmask i2c interrupt */
    I2C_UNMASK_INTERRUPT(i2c_port);

    /* start DMA */
    pdma_start_interrupt(g_i2c_rx_dma_channel[i2c_port], (uint32_t)buffer);
    /* start I2C */
    I2C_START(i2c_port);

    return HAL_I2C_STATUS_OK;
}



#ifdef HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE
hal_i2c_status_t hal_i2c_master_send_to_receive_polling(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_t *i2c_send_to_receive_config)
{
    i2c_transaction_status_t transaction_status;
    i2c_transaction_size_t config_size;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_send_to_receive_config->send_data || NULL == i2c_send_to_receive_config->receive_buffer) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }
    if (i2c_send_to_receive_config->send_length > I2C_FIFO_DEPTH || i2c_send_to_receive_config->receive_length > I2C_FIFO_DEPTH) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }


    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][send2receive_polling]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 9);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);
    /*mask interrupt*/
    I2C_MASK_INTERRUPT(i2c_port);

    /*config i2c hardware*/
    config_size.send_size = i2c_send_to_receive_config->send_length;
    config_size.receive_size = i2c_send_to_receive_config->receive_length;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_POLLING, I2C_GET_SEND_ADDRESS(i2c_send_to_receive_config->slave_address), &config_size);

    /*set data to fifo*/
    i2c_write_fifo(i2c_port, i2c_send_to_receive_config->send_data, i2c_send_to_receive_config->send_length);

    /*start i2c*/
    I2C_START(i2c_port);
#ifndef I2C_ENABLE_POLLING_TIMEOUT
    /* polling till transaction finish */
    while (0 == I2C_QUERY_TRANSACTION_FINISH_STATUS(i2c_port)) {
    }
#else
    hal_i2c_master_wait_transaction_finish(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_POLLING);
#endif
    /* read and status */
    transaction_status = i2c_get_transaction_status(i2c_port);
    /* unlock sleep */
    I2C_UNLOCK_SLEEP(i2c_port);

    if (I2C_TRANSACTION_SUCCESS != transaction_status) {
        log_hal_error("[I2C%d][send2receive_polling]:status=%d\r\n", i2c_port, transaction_status);
        return HAL_I2C_STATUS_ERROR;
    } else {
        i2c_read_fifo(i2c_port, i2c_send_to_receive_config->receive_buffer, i2c_send_to_receive_config->receive_length);
        return HAL_I2C_STATUS_OK;
    }
}




hal_i2c_status_t hal_i2c_master_send_to_receive_dma(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_t *i2c_send_to_receive_config)
{
    i2c_transaction_size_t config_size;
    pdma_config_t  i2c_dma_config;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_send_to_receive_config->send_data || NULL == i2c_send_to_receive_config->receive_buffer) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][send2receive_dma]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 10);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    /* config dma tx hardware */
    pdma_init(g_i2c_tx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = i2c_send_to_receive_config->send_length;
    i2c_dma_config.master_type = PDMA_TX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_tx_dma_channel[i2c_port], &i2c_dma_config);

    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_tx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);

    /* config dma rx hardware */
    pdma_init(g_i2c_rx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = i2c_send_to_receive_config->receive_length;
    i2c_dma_config.master_type = PDMA_RX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_rx_dma_channel[i2c_port], &i2c_dma_config);

    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    i2c_dma_callback_count[i2c_port] = 0;
    pdma_register_callback(g_i2c_rx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);

    /*config i2c hardware*/
    config_size.send_size = i2c_send_to_receive_config->send_length;
    config_size.receive_size = i2c_send_to_receive_config->receive_length;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA, I2C_GET_SEND_ADDRESS(i2c_send_to_receive_config->slave_address), &config_size);
    /* unmask i2c interrupt */
    I2C_UNMASK_INTERRUPT(i2c_port);

    /* start dma tx */
    pdma_start_interrupt(g_i2c_tx_dma_channel[i2c_port], (uint32_t)i2c_send_to_receive_config->send_data);
    /*start dma rx */
    pdma_start_interrupt(g_i2c_rx_dma_channel[i2c_port], (uint32_t)i2c_send_to_receive_config->receive_buffer);
    /* start I2C */
    I2C_START(i2c_port);


    return HAL_I2C_STATUS_OK;
}



#endif /* #ifdef HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE */

#ifdef HAL_I2C_MASTER_FEATURE_EXTENDED_DMA
hal_i2c_status_t hal_i2c_master_send_dma_ex(hal_i2c_port_t i2c_port, hal_i2c_send_config_t *i2c_send_config)
{
    i2c_transaction_size_t config_size;
    pdma_config_t  i2c_dma_config;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_send_config->send_data) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][send_dma_enhanced]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 14);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);
    /* config dma hardware */
    pdma_init(g_i2c_tx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = (i2c_send_config->send_packet_length) * (i2c_send_config->send_bytes_in_one_packet);
    i2c_dma_config.master_type = PDMA_TX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_tx_dma_channel[i2c_port], &i2c_dma_config);

    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA_EXTEND;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_tx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);


    /*config i2c hardware*/
    config_size.transfer_length = i2c_send_config->send_bytes_in_one_packet;
    config_size.transaction_length = i2c_send_config->send_packet_length;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA_EXTEND, I2C_GET_SEND_ADDRESS(i2c_send_config->slave_address), &config_size);
    /* unmask i2c interrupt */
    I2C_UNMASK_INTERRUPT(i2c_port);

    /* start DMA */
    pdma_start_interrupt(g_i2c_tx_dma_channel[i2c_port], (uint32_t)i2c_send_config->send_data);
    /* start I2C */
    I2C_START(i2c_port);




    return HAL_I2C_STATUS_OK;
}
hal_i2c_status_t hal_i2c_master_receive_dma_ex(hal_i2c_port_t i2c_port, hal_i2c_receive_config_t *i2c_receive_config)
{
    i2c_transaction_size_t config_size;
    pdma_config_t  i2c_dma_config;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_receive_config->receive_buffer) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][receive_dma_enhanced]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 15);
    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    /* config dma hardware */
    pdma_init(g_i2c_rx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = (i2c_receive_config->receive_packet_length) * (i2c_receive_config->receive_bytes_in_one_packet);
    i2c_dma_config.master_type = PDMA_RX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_rx_dma_channel[i2c_port], &i2c_dma_config);


    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA_EXTEND;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_rx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);

    /*config i2c hardware*/
    config_size.transfer_length = i2c_receive_config->receive_bytes_in_one_packet;
    config_size.transaction_length = i2c_receive_config->receive_packet_length;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA_EXTEND, I2C_GET_RECEIVE_ADDRESS(i2c_receive_config->slave_address), &config_size);
    /* unmask i2c interrupt */
    I2C_UNMASK_INTERRUPT(i2c_port);

    /* start DMA */
    pdma_start_interrupt(g_i2c_rx_dma_channel[i2c_port], (uint32_t)i2c_receive_config->receive_buffer);
    /* start I2C */
    I2C_START(i2c_port);

    return HAL_I2C_STATUS_OK;
}

hal_i2c_status_t hal_i2c_master_send_to_receive_dma_ex(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_ex_t *i2c_send_to_receive_config_ex)
{
    i2c_transaction_size_t config_size;
    pdma_config_t  i2c_dma_config;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_send_to_receive_config_ex->send_data || NULL == i2c_send_to_receive_config_ex->receive_buffer) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][send2receive_dma_enhanced]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 16);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    /* config dma tx hardware */
    pdma_init(g_i2c_tx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = i2c_send_to_receive_config_ex->send_bytes_in_one_packet;
    i2c_dma_config.master_type = PDMA_TX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_tx_dma_channel[i2c_port], &i2c_dma_config);

    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    i2c_dma_callback_count[i2c_port] = 0;
    pdma_register_callback(g_i2c_tx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);

    /* config dma rx hardware */
    pdma_init(g_i2c_rx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = (i2c_send_to_receive_config_ex->receive_packet_length) * (i2c_send_to_receive_config_ex->receive_bytes_in_one_packet);
    i2c_dma_config.master_type = PDMA_RX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_rx_dma_channel[i2c_port], &i2c_dma_config);

    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_rx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);

    /*config i2c hardware*/
    config_size.transfer_length = i2c_send_to_receive_config_ex->send_bytes_in_one_packet;
    config_size.transfer_length_aux = i2c_send_to_receive_config_ex->receive_bytes_in_one_packet;
    config_size.transaction_length = i2c_send_to_receive_config_ex->receive_packet_length + 1;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND, I2C_GET_SEND_ADDRESS(i2c_send_to_receive_config_ex->slave_address), &config_size);
    /* unmask i2c interrupt */
    I2C_UNMASK_INTERRUPT(i2c_port);

    /* start dma tx */
    pdma_start_interrupt(g_i2c_tx_dma_channel[i2c_port], (uint32_t)i2c_send_to_receive_config_ex->send_data);
    /*start dma rx */
    pdma_start_interrupt(g_i2c_rx_dma_channel[i2c_port], (uint32_t)i2c_send_to_receive_config_ex->receive_buffer);
    /* start I2C */
    I2C_START(i2c_port);


    return HAL_I2C_STATUS_OK;
}


#endif
hal_i2c_status_t hal_i2c_master_get_running_status(hal_i2c_port_t i2c_port, hal_i2c_running_status_t *running_status)
{
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    running_status->running_status = (hal_i2c_running_type_t)I2C_QUERY_RUNNING_STATUS(i2c_port);
    return HAL_I2C_STATUS_OK;
}

static void hal_i2c_master_isr(hal_nvic_irq_t irq_number)
{
    hal_i2c_port_t i2c_port = HAL_I2C_MASTER_MAX;
    uint32_t i = 0;

    for (i = 0; i < HAL_I2C_MASTER_MAX; i++) {
        if (s_i2c_irq_table[i] == irq_number) {
            i2c_port = (hal_i2c_port_t)i;
            break;
        }
    }

    if (HAL_I2C_MASTER_MAX == i2c_port) {
        log_hal_error("[I2C%d][isr]:port=%d\r\n", i2c_port, i2c_port);
        return;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 11);
    hal_i2c_isr_handler(i2c_port, s_i2c_callback[i2c_port], s_i2c_callback_parameter[i2c_port]);

}

void hal_i2c_isr_handler(hal_i2c_port_t i2c_port, hal_i2c_callback_t user_callback, void *user_data)
{
    i2c_transaction_status_t transaction_status;

    /* unlock sleep */
    I2C_UNLOCK_SLEEP(i2c_port);

    /* read i2c status */
    transaction_status = i2c_get_transaction_status(i2c_port);

    if (I2C_TRANSACTION_SUCCESS != transaction_status) {
        log_hal_error("[I2C%d][isr]:status=%d\r\n", i2c_port, transaction_status);
        /* In case of dummy interrupt */
        if (transaction_status == I2C_TRANSACTION_UNKNOWN_ERROR) {
            return;
        }
    }

#ifndef I2C_ENABLE_POLLING_TIMEOUT
    /* wait until dma finish  */
    while (1) {
        if (I2C_IF_DMA_FINISH(i2c_port)) {
            break;
        }
    }
#else
    hal_i2c_wait_dma_finish(i2c_port);
#endif

    /* call user callback */
    if (NULL != user_callback) {
        I2C_DEBUG_RECORD_FLOW(i2c_port, 12);
        user_callback(I2C_GET_SLAVE_ADDRESS(i2c_port), (hal_i2c_callback_event_t)transaction_status, user_data);
    } else {
        log_hal_error("[I2C%d][isr]:NO isr callback\r\n", i2c_port);
    }

}

static void hal_i2c_master_dma_callback(pdma_event_t event, void  *user_data)
{
    i2c_dma_callback_parameter_t *i2c_dma_parameter;

    i2c_dma_parameter = (i2c_dma_callback_parameter_t *)user_data;
    if (I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA == i2c_dma_parameter->transaction_type ||
            I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA_EXTEND == i2c_dma_parameter->transaction_type) {
        pdma_stop(g_i2c_tx_dma_channel[i2c_dma_parameter->i2c_port]);
        pdma_deinit(g_i2c_tx_dma_channel[i2c_dma_parameter->i2c_port]);
        i2c_dma_parameter->dma_finish = true;
    } else if (I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA == i2c_dma_parameter->transaction_type ||
               I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA_EXTEND == i2c_dma_parameter->transaction_type) {
        pdma_stop(g_i2c_rx_dma_channel[i2c_dma_parameter->i2c_port]);
        pdma_deinit(g_i2c_rx_dma_channel[i2c_dma_parameter->i2c_port]);
        i2c_dma_parameter->dma_finish = true;
    } else if (I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA == i2c_dma_parameter->transaction_type ||
               I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND == i2c_dma_parameter->transaction_type) {
        i2c_dma_callback_count[i2c_dma_parameter->i2c_port]++;
        /* i2c_dma_callback_count is only for hal_i2c_master_send_to_receive_dma
           when use this function, two DMA interrupt will come:Tx and Rx. Only the second
           DMA interrupt(Rx) means the transaction complete,so we don't change dma_finish to true
           until the sencond interrupt comes.
        */
        if (I2C_DMA_FINISH_CALLBACK_COUNT == i2c_dma_callback_count[i2c_dma_parameter->i2c_port]) {
            pdma_stop(g_i2c_tx_dma_channel[i2c_dma_parameter->i2c_port]);
            pdma_stop(g_i2c_rx_dma_channel[i2c_dma_parameter->i2c_port]);
            pdma_deinit(g_i2c_tx_dma_channel[i2c_dma_parameter->i2c_port]);
            pdma_deinit(g_i2c_rx_dma_channel[i2c_dma_parameter->i2c_port]);
            i2c_dma_parameter->dma_finish = true;
            i2c_dma_callback_count[i2c_dma_parameter->i2c_port] = 0;
        }
    }
}



#ifdef I2C_ENABLE_POLLING_TIMEOUT
static void hal_i2c_master_wait_transaction_finish(hal_i2c_port_t i2c_port, i2c_transaction_type_t transaction_type)
{
    uint32_t gpt_start_count, gpt_current_count, gpt_duration_count;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
    /* polling till transaction finish */
    while (1) {
        uint32_t saved_mask;
        saved_mask = save_and_set_interrupt_mask();
        if (0 != I2C_QUERY_TRANSACTION_FINISH_STATUS(i2c_port)) {
            restore_interrupt_mask(saved_mask);
            break;
        }
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
        hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
        if (I2C_POLLING_TIMEOUT_VALUE < gpt_duration_count) {
            log_hal_error("[I2C%d][type=%d]:timeout!\r\n", i2c_port, transaction_type);
            //assert(0);
        }
        restore_interrupt_mask(saved_mask);
    }
}

static void hal_i2c_wait_dma_finish(hal_i2c_port_t i2c_port)
{
    uint32_t gpt_start_count, gpt_current_count, gpt_duration_count;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
    /* polling till dma transaction finish */
    while (1) {
        uint32_t saved_mask;
        saved_mask = save_and_set_interrupt_mask();
        if (I2C_IF_DMA_FINISH(i2c_port)) {
            restore_interrupt_mask(saved_mask);
            break;
        }
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
        hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);

        if (I2C_POLLING_TIMEOUT_VALUE < gpt_duration_count) {
            log_hal_error("[I2C%d][isr]:dma timeout!\r\n", i2c_port);
            I2C_DEBUG_RECORD_FLOW(i2c_port, 13);
            restore_interrupt_mask(saved_mask);
            //assert(0);
            hal_i2c_master_dma_callback(PDMA_EVENT_TRANSACTION_SUCCESS, (void *)&s_i2c_dma_parameter[i2c_port]);
            if ((I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA == s_i2c_dma_parameter[i2c_port].transaction_type ||
                    I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND == s_i2c_dma_parameter[i2c_port].transaction_type) && (i2c_dma_callback_count[i2c_port] == 1)) {
                hal_i2c_master_dma_callback(PDMA_EVENT_TRANSACTION_SUCCESS, (void *)&s_i2c_dma_parameter[i2c_port]);
            }
            break;
        }
        restore_interrupt_mask(saved_mask);
    }
}

#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
void hal_i2c_backup_all_register(void)
{
    uint32_t i = 0;

    for (i = 0; i < HAL_I2C_MASTER_MAX; i++) {
        if (s_i2c_master_status[i] == I2C_BUSY) {
            i2c_register_backup[i].clock_div = i2c_master_register[i]->CLOCK_DIV;
            i2c_register_backup[i].timing = i2c_master_register[i]->TIMING_UNION.TIMING;
        }
    }
}


void hal_i2c_restore_all_register(void)
{
    uint32_t i = 0;

    for (i = 0; i < HAL_I2C_MASTER_MAX; i++) {
        if (s_i2c_master_status[i] == I2C_BUSY) {
            i2c_master_register[i]->CLOCK_DIV = i2c_register_backup[i].clock_div;
            i2c_master_register[i]->TIMING_UNION.TIMING = i2c_register_backup[i].timing;
        }
    }
}

#endif


#endif

