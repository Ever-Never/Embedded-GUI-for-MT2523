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

#include <string.h>
#include "dma_sw.h"
#include "mt7687.h"
#include "system_mt7687.h"
#include "core_cm4.h"
#include "uart.h"
#include "top.h"
#include "hal_log.h"
#include "hal_uart.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_nvic.h"
#endif

#define UART_PORT_MAPPING(ext)          \
    (                                   \
        (                               \
            (ext == HAL_UART_0) ?       \
                UART_PORT0              \
            :                           \
                (ext == HAL_UART_1) ?   \
                    UART_PORT1          \
                :                       \
                    UART_MAX_PORT       \
        )                               \
    )


typedef struct {
    hal_uart_callback_t callback;
    void *user_data;
} hal_uart_callback_config_t;

typedef struct {
    bool basic_config_present;
    hal_uart_config_t basic_config;
    bool callback_present;
    hal_uart_callback_config_t callback_config;
    bool dma_config_present;
    hal_uart_dma_config_t dma_config;
} uart_context_t;

static uart_context_t g_uart_context_table[HAL_UART_MAX];

#ifdef HAL_SLEEP_MANAGER_ENABLED
static bool g_uart_frist_send_complete_interrupt[HAL_UART_MAX] = {false};
static bool g_uart_send_lock_status[HAL_UART_MAX] = {false};
static uint8_t g_uart_sleep_handler[HAL_UART_MAX];
extern const char *const g_uart_sleep_handler_name[];
extern hal_nvic_irq_t g_uart_port_to_irq_num[];
#endif

static bool hal_uart_is_port_valid(hal_uart_port_t uart_port)
{
    return (uart_port < HAL_UART_MAX);
}


static bool hal_uart_is_config_valid(hal_uart_port_t uart_port, const hal_uart_config_t *uart_config)
{
    if (!hal_uart_is_port_valid(uart_port)) {
        return false;
    }
    if (NULL == uart_config) {
        return false;
    }
    if ((uart_config->baudrate > HAL_UART_BAUDRATE_921600) ||
            (uart_config->parity > HAL_UART_PARITY_EVEN) ||
            (uart_config->stop_bit > HAL_UART_STOP_BIT_2) ||
            (uart_config->word_length > HAL_UART_WORD_LENGTH_8)) {
        return false;
    }

    return true;
}


static bool hal_uart_is_dma_config_valid(const hal_uart_dma_config_t *dma_config)
{
    if (NULL == dma_config) {
        return false;
    }
    if (NULL == dma_config->receive_vfifo_buffer) {
        return false;
    }
    if (dma_config->receive_vfifo_buffer_size < dma_config->receive_vfifo_threshold_size) {
        return false;
    }
    if (dma_config->receive_vfifo_buffer_size < dma_config->receive_vfifo_alert_size) {
        return false;
    }
    if (NULL == dma_config->send_vfifo_buffer) {
        return false;
    }
    if (dma_config->send_vfifo_buffer_size < dma_config->send_vfifo_threshold_size) {
        return false;
    }
    if (dma_config->receive_vfifo_buffer_size < dma_config->receive_vfifo_alert_size) {
        return false;
    }
    return true;
}


static UART_PORT hal_uart_translate_port(hal_uart_port_t uart_port)
{
    return (HAL_UART_0 == uart_port) ? UART_PORT0 : UART_PORT1;
}


static uint32_t hal_uart_translate_baudrate(hal_uart_baudrate_t bandrate)
{
    uint32_t baudrate_tbl[] = {
        110, 300, 1200, 2400, 4800, 9600, 19200,
        38400, 57600, 115200, 230400, 460800, 921600
    };
    return baudrate_tbl[bandrate];
}


static uint16_t hal_uart_translate_word_length(hal_uart_word_length_t word_length)
{
    uint16_t databit_tbl[] = {
        UART_WLS_5, UART_WLS_6, UART_WLS_7, UART_WLS_8
    };
    return databit_tbl[word_length];
}


static uint16_t hal_uart_translate_parity(hal_uart_parity_t parity)
{
    uint16_t parity_tbl[] = {
        UART_NONE_PARITY, UART_ODD_PARITY, UART_EVEN_PARITY, HAL_UART_PARITY_NONE, HAL_UART_PARITY_NONE
    };
    return parity_tbl[parity];
}


static uint16_t hal_uart_translate_stopbit(hal_uart_stop_bit_t stopbit)
{
    uint16_t stopbit_tbl[] = {
        UART_1_STOP, UART_2_STOP, UART_1_5_STOP
    };
    return stopbit_tbl[stopbit];
}


static uint32_t hal_uart_translate_timeout(uint32_t timeout_us)
{
    uint32_t ticks_per_us;

    ticks_per_us = top_mcu_freq_get() / 1000000;

    return ticks_per_us * timeout_us;
}

static DMA_VFIFO_CHANNEL uart_channel_to_dma_channel(UART_PORT port, bool is_rx)
{
    DMA_VFIFO_CHANNEL ch;

    if (port == UART_PORT0) {
        if (is_rx) {
            ch = VDMA_UART1RX_CH;
        } else {
            ch = VDMA_UART1TX_CH;
        }
    } else {
        if (is_rx) {
            ch = VDMA_UART2RX_CH;
        } else {
            ch = VDMA_UART2TX_CH;
        }
    }

    return ch;
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
void uart_send_handler(UART_PORT u_port)
{
    uint32_t length;
    uint32_t irq_status;

    if (g_uart_frist_send_complete_interrupt[u_port] == false) {
        hal_nvic_save_and_set_interrupt_mask(&irq_status);
        halUART_Get_VFIFO_Length(u_port, 0, &length);
        if (g_uart_context_table[u_port].dma_config.send_vfifo_buffer_size - length == 0) {
            if (g_uart_send_lock_status[u_port] == true) {
                hal_sleep_manager_unlock_sleep(g_uart_sleep_handler[u_port]);
                g_uart_send_lock_status[u_port] = false;
            }
        }
        hal_nvic_restore_interrupt_mask(irq_status);
    } else {
        g_uart_frist_send_complete_interrupt[u_port] = false;
    }
}
#endif

hal_uart_status_t hal_uart_init(hal_uart_port_t uart_port, hal_uart_config_t *uart_config)
{
    if (!hal_uart_is_config_valid(uart_port, uart_config)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }
    if (g_uart_context_table[uart_port].basic_config_present == true) {
        return HAL_UART_STATUS_ERROR_BUSY;
    }
    memset(&g_uart_context_table[uart_port], 0, sizeof(uart_context_t));
    memcpy(&g_uart_context_table[uart_port].basic_config,
           uart_config, sizeof(hal_uart_config_t));
    g_uart_context_table[uart_port].basic_config_present = true;
    uart_reset_default_value(hal_uart_translate_port(uart_port));
    halUART_HWInit(hal_uart_translate_port(uart_port));

    if (HAL_UART_STATUS_OK != hal_uart_set_format(uart_port, uart_config)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

#ifdef HAL_SLEEP_MANAGER_ENABLED
    uart_set_sleep_mode(hal_uart_translate_port(uart_port));
    g_uart_frist_send_complete_interrupt[uart_port] = false;
    g_uart_send_lock_status[uart_port] = false;
    g_uart_sleep_handler[uart_port] = hal_sleep_manager_set_sleep_handle((char *)g_uart_sleep_handler_name[uart_port]);
#endif

    return HAL_UART_STATUS_OK;
}


hal_uart_status_t hal_uart_deinit(hal_uart_port_t uart_port)
{
    if (!hal_uart_is_port_valid(uart_port)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }
    if (g_uart_context_table[uart_port].basic_config_present == false) {
        return HAL_UART_STATUS_ERROR_UNINITIALIZED;
    }
    memset(&g_uart_context_table[uart_port], 0, sizeof(uart_context_t));

    /* wait all left data sent out before deinit. */
    uart_query_empty(hal_uart_translate_port(uart_port));

#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_nvic_disable_irq(g_uart_port_to_irq_num[uart_port]);
    g_uart_frist_send_complete_interrupt[uart_port] = false;
    g_uart_send_lock_status[uart_port] = false;
    uart_mask_send_interrupt(hal_uart_translate_port(uart_port));
    hal_sleep_manager_release_sleep_handle(g_uart_sleep_handler[uart_port]);
#endif
    uart_reset_default_value(hal_uart_translate_port(uart_port));

    return HAL_UART_STATUS_OK;
}


void hal_uart_put_char(hal_uart_port_t uart_port, char byte)
{
    UART_PORT   int_no = UART_PORT_MAPPING(uart_port);

    if (int_no != UART_MAX_PORT) {
        uart_output_char(int_no, (unsigned int)byte);
    } else {
        //log_hal_error("uart port invalid: %d", uart_port);
    }
}


uint32_t hal_uart_send_polling(hal_uart_port_t uart_port, const uint8_t *data, uint32_t size)
{
    UART_PORT   int_no = UART_PORT_MAPPING(uart_port);
    uint32_t    index;

    if (data == NULL) {
        return 0;
    }

    if (int_no < UART_MAX_PORT) {
        for (index = 0; index < size; index++) {
            uart_output_char(int_no, (unsigned char)data[index]);
        }
        return size;
    }
    return 0;
}

uint32_t hal_uart_send_dma(hal_uart_port_t uart_port, const uint8_t *data, uint32_t size)
{
    uint32_t actual_space, send_size;
    DMA_VFIFO_CHANNEL ch;
#ifdef HAL_SLEEP_MANAGER_ENABLED
    uint32_t irq_status;
#endif

    if (!hal_uart_is_port_valid(uart_port)) {
        //log_hal_error("Wrong uart port: %d", uart_port);
        return 0;
    }
    if ((data == NULL) || (size == 0)) {
        return 0;
    }
    if (!g_uart_context_table[uart_port].dma_config_present) {
        //log_hal_error("DMA not actived.");
        return 0;
    }

#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_nvic_save_and_set_interrupt_mask(&irq_status);
    if (g_uart_send_lock_status[uart_port] == false) {
        hal_sleep_manager_lock_sleep(g_uart_sleep_handler[uart_port]);
        g_uart_send_lock_status[uart_port] = true;
    }
    hal_nvic_restore_interrupt_mask(irq_status);
#endif

    halUART_Get_VFIFO_Length(hal_uart_translate_port(uart_port), 0, &actual_space);
    if (actual_space >= size) {
        send_size = size;
    } else {
        send_size = actual_space;
    }
    halUART_Write_VFIFO_Data(hal_uart_translate_port(uart_port), data, send_size);

    if (send_size == actual_space) {
        ch = uart_channel_to_dma_channel(hal_uart_translate_port(uart_port), 0);
        DMA_Vfifo_enable_interrupt(ch);
    }
    return send_size;
}


char hal_uart_get_char(hal_uart_port_t uart_port)
{
    UART_PORT   int_no = UART_PORT_MAPPING(uart_port);

    if (int_no < UART_MAX_PORT) {
        return (char)uart_input_char(int_no);
    } else {
        //log_hal_error("uart port invalid: %d", uart_port);
        return 0;
    }
}

uint32_t hal_uart_get_char_unblocking(hal_uart_port_t uart_port)
{
    UART_PORT   int_no = UART_PORT_MAPPING(uart_port);

    if (int_no < UART_MAX_PORT) {
        return uart_input_char_unblocking(int_no);
    } else {
        //log_hal_error("uart port invalid: %d", uart_port);
        return 0;
    }
}


uint32_t hal_uart_receive_polling(hal_uart_port_t uart_port, uint8_t *buffer, uint32_t size)
{
    UART_PORT   int_no = UART_PORT_MAPPING(uart_port);
    uint32_t    index;

    if (buffer == NULL) {
        return 0;
    }

    if (int_no < UART_MAX_PORT) {
        for (index = 0; index < size; index++) {
            buffer[index] = (uint8_t)uart_input_char(int_no);
        }
        return size;
    }
    return 0;
}

uint32_t hal_uart_receive_dma(hal_uart_port_t uart_port, uint8_t *buffer, uint32_t size)
{
    uint32_t actual_size, receive_size;
    DMA_VFIFO_CHANNEL ch;

    if (!hal_uart_is_port_valid(uart_port)) {
        //log_hal_error("Wrong uart port: %d", uart_port);
        return 0;
    }
    if ((buffer == NULL) || (size == 0)) {
        return 0;
    }

    halUART_Get_VFIFO_Length(hal_uart_translate_port(uart_port), 1, &actual_size);
    if (actual_size < size) {
        receive_size = actual_size;
    } else {
        receive_size = size;
    }
    halUART_Read_VFIFO_Data(hal_uart_translate_port(uart_port), buffer, receive_size);

    if (receive_size == actual_size) {
        ch = uart_channel_to_dma_channel(hal_uart_translate_port(uart_port), 1);
        DMA_Vfifo_enable_interrupt(ch);
    }

    return receive_size;
}


uint32_t hal_uart_get_available_send_space(hal_uart_port_t uart_port)
{
    uint32_t length = 0;

    if (!hal_uart_is_port_valid(uart_port)) {
        return 0;
    }

    halUART_Get_VFIFO_Length(hal_uart_translate_port(uart_port), 0, &length);
    return length;
}


uint32_t hal_uart_get_available_receive_bytes(hal_uart_port_t uart_port)
{
    uint32_t length = 0;

    if (!hal_uart_is_port_valid(uart_port)) {
        //log_hal_error("Wrong uart port: %d", uart_port);
        return 0;
    }
    halUART_Get_VFIFO_Length(hal_uart_translate_port(uart_port), 1, &length);

    return length;
}

static void hal_uart_check_irq_disable(UART_PORT uart_port, bool is_rx)
{
    uint32_t avail_size, avail_space;
    hal_uart_dma_config_t *dma_config;
    DMA_VFIFO_CHANNEL ch;

    dma_config = &g_uart_context_table[uart_port].dma_config;
    if (is_rx) {
        halUART_Get_VFIFO_Length(uart_port, 1, &avail_size);
        if (avail_size >= dma_config->receive_vfifo_threshold_size) {
            ch = uart_channel_to_dma_channel(uart_port, 1);
            DMA_Vfifo_disable_interrupt(ch);
        }
    } else {
        halUART_Get_VFIFO_Length(uart_port, 0, &avail_space);
        if (avail_space >= dma_config->send_vfifo_buffer_size - dma_config->send_vfifo_threshold_size) {
            ch = uart_channel_to_dma_channel(uart_port, 0);
            DMA_Vfifo_disable_interrupt(ch);
        }
    }
}

static void hal_uart_receive_callback0(void)
{
    if ((g_uart_context_table[HAL_UART_0].callback_present) &&
            (hal_uart_get_available_receive_bytes(HAL_UART_0) != 0)) {
        hal_uart_callback_config_t *callback_context = &g_uart_context_table[HAL_UART_0].callback_config;
        if (NULL != callback_context->callback) {
            callback_context->callback(HAL_UART_EVENT_READY_TO_READ, callback_context->user_data);
        }
    }
    hal_uart_check_irq_disable(hal_uart_translate_port(HAL_UART_0), 1);
}


static void hal_uart_receive_callback1(void)
{
    if ((g_uart_context_table[HAL_UART_1].callback_present) &&
            (hal_uart_get_available_receive_bytes(HAL_UART_1) != 0)) {
        hal_uart_callback_config_t *callback_context = &g_uart_context_table[HAL_UART_1].callback_config;
        if (NULL != callback_context->callback) {
            callback_context->callback(HAL_UART_EVENT_READY_TO_READ, callback_context->user_data);
        }
    }
    hal_uart_check_irq_disable(hal_uart_translate_port(HAL_UART_1), 1);
}

static void hal_uart_send_callback0(void)
{
    if (g_uart_context_table[HAL_UART_0].callback_present) {
        hal_uart_callback_config_t *callback_context = &g_uart_context_table[HAL_UART_0].callback_config;
        if (NULL != callback_context->callback) {
            callback_context->callback(HAL_UART_EVENT_READY_TO_WRITE, callback_context->user_data);
        }
    }
    hal_uart_check_irq_disable(hal_uart_translate_port(HAL_UART_0), 0);
}


static void hal_uart_send_callback1(void)
{
    if (g_uart_context_table[HAL_UART_1].callback_present) {
        hal_uart_callback_config_t *callback_context = &g_uart_context_table[HAL_UART_1].callback_config;
        if (NULL != callback_context->callback) {
            callback_context->callback(HAL_UART_EVENT_READY_TO_WRITE, callback_context->user_data);
        }
    }
    hal_uart_check_irq_disable(hal_uart_translate_port(HAL_UART_1), 0);
}

static inline VOID_FUNC hal_uart_map_receive_callback(hal_uart_port_t uart_port)
{
    static const VOID_FUNC func_tbl[] = {
        hal_uart_receive_callback0, hal_uart_receive_callback1
    };
    return func_tbl[uart_port];
}


hal_uart_status_t hal_uart_register_callback(hal_uart_port_t uart_port,
        hal_uart_callback_t user_callback,
        void *user_data)
{
    DMA_VFIFO_CHANNEL ch;

    if (!hal_uart_is_port_valid(uart_port)) {
        //log_hal_error("Wrong uart port: %d", uart_port);
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }
    if (user_callback == NULL) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }
    g_uart_context_table[uart_port].callback_config.callback = user_callback;
    g_uart_context_table[uart_port].callback_config.user_data = user_data;
    g_uart_context_table[uart_port].callback_present = true;

    halUART_VDMA_Rx_Register_Callback(hal_uart_translate_port(uart_port),
                                      hal_uart_map_receive_callback(uart_port));
    halUART_VDMA_Rx_Register_TO_Callback(hal_uart_translate_port(uart_port),
                                         hal_uart_map_receive_callback(uart_port));
    if (hal_uart_translate_port(uart_port) == UART_PORT0) {
        DMA_Vfifo_Register_Callback(VDMA_UART1TX_CH, hal_uart_send_callback0);
    } else {
        DMA_Vfifo_Register_Callback(VDMA_UART2TX_CH, hal_uart_send_callback1);
    }
    ch = uart_channel_to_dma_channel(hal_uart_translate_port(uart_port), 0);
    DMA_Vfifo_enable_interrupt(ch);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    g_uart_frist_send_complete_interrupt[uart_port] = true;
    hal_nvic_register_isr_handler(g_uart_port_to_irq_num[uart_port], uart_interrupt_handler);
    uart_unmask_send_interrupt(hal_uart_translate_port(uart_port));
    hal_nvic_enable_irq(g_uart_port_to_irq_num[uart_port]);
#endif

    return HAL_UART_STATUS_OK;
}


hal_uart_status_t hal_uart_set_baudrate(hal_uart_port_t uart_port, hal_uart_baudrate_t baudrate)
{
    if ((!hal_uart_is_port_valid(uart_port)) ||
            (baudrate > HAL_UART_BAUDRATE_921600)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }
    g_uart_context_table[uart_port].basic_config.baudrate = baudrate;
    return hal_uart_set_format(uart_port, &g_uart_context_table[uart_port].basic_config);
}


hal_uart_status_t hal_uart_set_format(hal_uart_port_t uart_port, const hal_uart_config_t *config)
{
    if (!hal_uart_is_config_valid(uart_port, config)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }
    memcpy(&g_uart_context_table[uart_port].basic_config, config, sizeof(hal_uart_config_t));
    halUART_SetFormat(hal_uart_translate_port(uart_port),
                      hal_uart_translate_baudrate(config->baudrate),
                      hal_uart_translate_word_length(config->word_length),
                      hal_uart_translate_parity(config->parity),
                      hal_uart_translate_stopbit(config->stop_bit));
    return HAL_UART_STATUS_OK;
}


hal_uart_status_t hal_uart_set_dma(hal_uart_port_t uart_port, const hal_uart_dma_config_t *uart_dma_config)
{
    if (!hal_uart_is_port_valid(uart_port)) {
        //log_hal_error("Invalid port!");
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }
    if (!hal_uart_is_dma_config_valid(uart_dma_config)) {
        //log_hal_error("Invalid dma config!");
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }
    g_uart_context_table[uart_port].dma_config_present = true;
    memcpy(&g_uart_context_table[uart_port].dma_config,
           uart_dma_config, sizeof(hal_uart_dma_config_t));
    DMA_Init();
    DMA_Vfifo_init();
    halUART_Rx_VFIFO_Initialize(hal_uart_translate_port(uart_port),
                                uart_dma_config->receive_vfifo_buffer,
                                uart_dma_config->receive_vfifo_buffer_size,
                                uart_dma_config->receive_vfifo_alert_size,
                                uart_dma_config->receive_vfifo_threshold_size,
                                hal_uart_translate_timeout(5000)); /* 5ms */
    halUART_Tx_VFIFO_Initialize(hal_uart_translate_port(uart_port),
                                uart_dma_config->send_vfifo_buffer,
                                uart_dma_config->send_vfifo_buffer_size,
                                uart_dma_config->send_vfifo_threshold_size);

    return HAL_UART_STATUS_OK;
}

hal_uart_status_t hal_uart_set_hardware_flowcontrol(hal_uart_port_t uart_port)
{
    UART_PORT int_no;

    if (!hal_uart_is_port_valid(uart_port)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    int_no = UART_PORT_MAPPING(uart_port);
    uart_set_hardware_flowcontrol(int_no);

    return HAL_UART_STATUS_OK;
}

hal_uart_status_t hal_uart_set_software_flowcontrol(hal_uart_port_t uart_port,
        uint8_t xon,
        uint8_t xoff,
        uint8_t escape_character)
{
    UART_PORT int_no;

    if (!hal_uart_is_port_valid(uart_port)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    int_no = UART_PORT_MAPPING(uart_port);
    uart_set_software_flowcontrol(int_no, xon, xoff, escape_character);

    return HAL_UART_STATUS_OK;
}

hal_uart_status_t hal_uart_disable_flowcontrol(hal_uart_port_t uart_port)
{
    UART_PORT int_no;

    if (!hal_uart_is_port_valid(uart_port)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    int_no = UART_PORT_MAPPING(uart_port);
    uart_disable_flowcontrol(int_no);

    return HAL_UART_STATUS_OK;
}

#ifdef HAL_UART_FEATURE_VFIFO_DMA_TIMEOUT
hal_uart_status_t hal_uart_set_dma_timeout(hal_uart_port_t uart_port, uint32_t timeout)
{
    if (!hal_uart_is_port_valid(uart_port)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    if (uart_port == HAL_UART_0) {
        DMA_Vfifo_Set_timeout(VDMA_UART1RX_CH, hal_uart_translate_timeout(timeout));
    } else {
        DMA_Vfifo_Set_timeout(VDMA_UART2RX_CH, hal_uart_translate_timeout(timeout));
    }

    return HAL_UART_STATUS_OK;
}
#endif

