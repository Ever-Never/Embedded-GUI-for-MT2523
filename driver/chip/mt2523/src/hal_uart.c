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

#include "hal_uart.h"

#ifdef HAL_UART_MODULE_ENABLED

#include "memory_attribute.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_gpt.h"
#include "hal_sleep_manager.h"
#include "core_cm4.h"
#endif
#include "hal_pdma_internal.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_clock.h"
#ifdef HAL_UART_FEATURE_SOFT_DTE_SUPPORT
#include "hal_gpio.h"
#include "hal_eint.h"
#endif
#include "hal_uart_internal.h"
#include "hal_uart_internal_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
/* Sleep handler's name with pattern "uartx"  */
#define UART_SLEEP_HANDLER_NAME_LENGTH 5
#endif

typedef enum {
    UART_HWSTATUS_UNINITIALIZED,
    UART_HWSTATUS_POLL_INITIALIZED,
    UART_HWSTATUS_DMA_INITIALIZED,
} uart_hwstatus_t;

typedef struct {
    hal_uart_callback_t func;
    void *arg;
} uart_callback_t;

#ifdef HAL_SLEEP_MANAGER_ENABLED
typedef enum {
    UART_FLOWCONTROL_NONE,
    UART_FLOWCONTROL_SOFTWARE,
    UART_FLOWCONTROL_HARDWARE,
} uart_flowcontrol_t;

typedef struct {
    uint8_t xon;
    uint8_t xoff;
    uint8_t escape_character;
} uart_sw_flowcontrol_config_t;
#endif

static bool g_uart_global_data_initialized = false;
#ifdef HAL_SLEEP_MANAGER_ENABLED
static bool g_uart_frist_send_complete_interrupt[HAL_UART_MAX];
static bool g_uart_restore_init;
static bool g_uart_send_lock_status[HAL_UART_MAX];
static uint8_t g_uart_sleep_handler[HAL_UART_MAX];
static uart_flowcontrol_t g_uart_flowcontrol_status[HAL_UART_MAX];
static uart_sw_flowcontrol_config_t g_uart_sw_flowcontrol_config[HAL_UART_MAX];
static hal_uart_config_t g_uart_config[HAL_UART_MAX];
static bool g_uart_irq_pending_status[HAL_UART_MAX];
#endif
static volatile uart_hwstatus_t g_uart_hwstatus[HAL_UART_MAX];
static uart_callback_t g_uart_callback[HAL_UART_MAX];
static uart_dma_callback_data_t g_uart_dma_callback_data[HAL_UART_MAX * 2];
static hal_uart_dma_config_t g_uart_dma_config[HAL_UART_MAX];
static const uint32_t g_uart_baudrate_map[] = {110, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600};

extern UART_REGISTER_T *const g_uart_regbase[];
extern hal_clock_cg_id g_uart_port_to_pdn[];
extern hal_nvic_irq_t g_uart_port_to_irq_num[];
#ifdef HAL_SLEEP_MANAGER_ENABLED
extern const char *const g_uart_sleep_handler_name[];
#endif

#ifdef HAL_UART_FEATURE_SOFT_DTE_SUPPORT
static volatile bool g_uart_dte_enable[HAL_UART_MAX] = {false};
static uart_dte_config_t g_uart_dte_config[HAL_UART_MAX];
static void uart_dte_reset(hal_uart_port_t uart_port);
#endif

static bool uart_port_is_valid(hal_uart_port_t uart_port)
{
    return (uart_port < HAL_UART_MAX);
}

static bool uart_baudrate_is_valid(hal_uart_baudrate_t baudrate)
{
    return (baudrate < HAL_UART_BAUDRATE_MAX);
}

static bool uart_config_is_valid(const hal_uart_config_t *config)
{
    return ((config->baudrate < HAL_UART_BAUDRATE_MAX) &&
            (config->word_length <= HAL_UART_WORD_LENGTH_8) &&
            (config->stop_bit <= HAL_UART_STOP_BIT_2) &&
            (config->parity <= HAL_UART_PARITY_EVEN));
}

/* triggered by vfifo dma rx thershold interrupt or UART receive timeout interrupt.
 * 1. When vfifo dma rx thershold interrupt happen,
 * this function is called with is_timeout=false.
 * then call suer's callback to notice that data can be fetched from receive buffer.
 * 2. When UART receive timeout interrupt happen,
 * this function is called with is_timeout=true.
 * then call suer's callback to notice that data can be fetched from receive buffer.
 */
void uart_receive_handler(hal_uart_port_t uart_port, bool is_timeout)
{
    vdma_channel_t channel;
    uint32_t avail_bytes;
    hal_uart_callback_t callback;
    void *arg;
    UART_REGISTER_T *uartx;
    vdma_status_t status;

    if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_DMA_INITIALIZED) {
        UART_ASSERT();
        return;
    }

    uartx = g_uart_regbase[uart_port];
    channel = uart_port_to_dma_channel(uart_port, 1);

    if (is_timeout == true) {
        uart_clear_timeout_interrupt(uartx);
    }

    status = vdma_get_available_receive_bytes(channel, &avail_bytes);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return;
    }
    if (avail_bytes == 0) {
        return;
    }

    callback = g_uart_callback[uart_port].func;
    arg = g_uart_callback[uart_port].arg;
    if (callback == NULL) {
        UART_ASSERT();
        return;
    }
    /* for callback re-entry issue */
    vdma_disable_interrupt(channel);
    callback(HAL_UART_EVENT_READY_TO_READ, arg);
    vdma_enable_interrupt(channel);

    status = vdma_get_available_receive_bytes(channel, &avail_bytes);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return;
    }
    if (avail_bytes >= g_uart_dma_config[uart_port].receive_vfifo_threshold_size) {
        status = vdma_disable_interrupt(channel);
        if (status != VDMA_OK) {
            UART_ASSERT();
            return;
        }
    }
}

/* triggered by vfifo dma tx thershold interrupt or uart transmit complete interrupt.
 * 1. When vfifo dma tx thershold interrupt happen,
 * this function is called with is_send_complete_trigger=false.
 * then call suer's callback to notice that data can be put in send buffer again.
 * 2. When UART transmit complete interrupt happen,
 * this function is called with is_send_complete_trigger=true.
 * Now all user data has been sent out, so we call hal_sleep_manager_unlock_sleep()
 * to unlock sleep.
 */
void uart_send_handler(hal_uart_port_t uart_port, bool is_send_complete_trigger)
{
    vdma_channel_t channel;
    vdma_status_t status;
    uint32_t compare_space, avail_space;
    hal_uart_callback_t callback;
    void *arg;
#ifdef HAL_SLEEP_MANAGER_ENABLED
    uint32_t irq_status;
#endif

    if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_DMA_INITIALIZED) {
        UART_ASSERT();
        return;
    }

    channel = uart_port_to_dma_channel(uart_port, 0);

    if (is_send_complete_trigger == true) {
#ifdef HAL_SLEEP_MANAGER_ENABLED
        if (g_uart_frist_send_complete_interrupt[uart_port] == false) {
            irq_status = save_and_set_interrupt_mask();
            status = vdma_get_available_send_space(channel, &avail_space);
            if (status != VDMA_OK) {
                UART_ASSERT();
                return;
            }
            if (g_uart_dma_config[uart_port].send_vfifo_buffer_size - avail_space == 0) {
                if (g_uart_send_lock_status[uart_port] == true) {

                    if( hal_sleep_manager_is_sleep_handle_alive(g_uart_sleep_handler[uart_port]) == true) {
                        hal_sleep_manager_unlock_sleep(g_uart_sleep_handler[uart_port]);
                    }
                    g_uart_send_lock_status[uart_port] = false;
                }
            }
            restore_interrupt_mask(irq_status);
        } else {
            g_uart_frist_send_complete_interrupt[uart_port] = false;
        }
        return;
#endif
    } else {
        callback = g_uart_callback[uart_port].func;
        arg = g_uart_callback[uart_port].arg;
        if (callback == NULL) {
            UART_ASSERT();
            return;
        }
        /* for callback re-entry issue */
        vdma_disable_interrupt(channel);
        callback(HAL_UART_EVENT_READY_TO_WRITE, arg);
        vdma_enable_interrupt(channel);

        status = vdma_get_available_send_space(channel, &avail_space);
        if (status != VDMA_OK) {
            UART_ASSERT();
            return;
        }
        compare_space = g_uart_dma_config[uart_port].send_vfifo_buffer_size
                        - g_uart_dma_config[uart_port].send_vfifo_threshold_size;
        if (avail_space >= compare_space) {
            status = vdma_disable_interrupt(channel);
            if (status != VDMA_OK) {
                UART_ASSERT();
                return;
            }
        }
    }
}

/* Only triggered by UART error interrupt */
void uart_error_handler(hal_uart_port_t uart_port)
{
    UART_REGISTER_T *uartx;
    hal_uart_callback_t callback;
    void *arg;

    uartx = g_uart_regbase[uart_port];

    if (!uart_verify_error(uartx)) {
        uart_purge_fifo(uartx, 1);
        uart_purge_fifo(uartx, 0);
        callback = g_uart_callback[uart_port].func;
        arg = g_uart_callback[uart_port].arg;
        if (callback == NULL) {
            UART_ASSERT();
            return;
        }
        callback(HAL_UART_EVENT_TRANSACTION_ERROR, arg);
    }
}

static void uart_dma_callback_handler(vdma_event_t event, void  *user_data)
{
    uart_dma_callback_data_t *callback_data = (uart_dma_callback_data_t *)user_data;

    if (callback_data->is_rx == true) {
        uart_receive_handler(callback_data->uart_port, false);
    } else {
        uart_send_handler(callback_data->uart_port, false);
    }
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
void uart_backup_all_registers(void)
{
    hal_uart_port_t uart_port;
    uart_hwstatus_t uart_hwstatus;
    uart_flowcontrol_t uart_flowcontrol;
    hal_uart_callback_t uart_callback;
    void *uart_callback_arg;
#ifdef HAL_UART_FEATURE_SOFT_DTE_SUPPORT
    uart_dte_config_t *dte_config;
    hal_uart_dte_callback_t user_dte_callback;
    void *user_dte_data;
    bool uart_dte_enable;
#endif

    for (uart_port = HAL_UART_0; uart_port < HAL_UART_MAX; uart_port++) {
        if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_UNINITIALIZED) {
            uart_hwstatus = g_uart_hwstatus[uart_port];
            uart_flowcontrol = g_uart_flowcontrol_status[uart_port];
            uart_callback = g_uart_callback[uart_port].func;
            uart_callback_arg = g_uart_callback[uart_port].arg;
#ifdef HAL_UART_FEATURE_SOFT_DTE_SUPPORT
            if (g_uart_dte_enable[uart_port] == true) {
                uart_dte_enable = true;
                dte_config = &g_uart_dte_config[uart_port];
                user_dte_callback = dte_config->dte_config.user_callback;
                user_dte_data = dte_config->dte_config.user_data;
            } else {
                uart_dte_enable = false;
            }
#endif
            hal_uart_deinit(uart_port);
            g_uart_hwstatus[uart_port] = uart_hwstatus;
            g_uart_flowcontrol_status[uart_port] = uart_flowcontrol;
            g_uart_callback[uart_port].func = uart_callback;
            g_uart_callback[uart_port].arg = uart_callback_arg;
            g_uart_irq_pending_status[uart_port] = hal_nvic_get_pending_irq(g_uart_port_to_irq_num[uart_port]);
#ifdef HAL_UART_FEATURE_SOFT_DTE_SUPPORT
            if (uart_dte_enable == true) {
                dte_config->dte_config.user_callback = user_dte_callback;
                dte_config->dte_config.user_data = user_dte_data;
                g_uart_dte_enable[uart_port] = true;
            }
#endif
        }
    }
}

void uart_restore_all_registers(void)
{
    uint32_t delay;
    hal_uart_port_t uart_port;
    UART_REGISTER_T *uartx;
    uart_hwstatus_t uart_hwstatus;
    bool sleep_lock_status;
    g_uart_restore_init = true;

    for (uart_port = HAL_UART_0; uart_port < HAL_UART_MAX; uart_port++) {
        uart_hwstatus = g_uart_hwstatus[uart_port];
        if (uart_hwstatus != UART_HWSTATUS_UNINITIALIZED) {
            g_uart_hwstatus[uart_port] = UART_HWSTATUS_UNINITIALIZED;
            hal_uart_init(uart_port, &g_uart_config[uart_port]);
        }
        /* Because of hardware limitation, we have to send XON manually
         * when software flow control is turn on for that port.
         */
        if (g_uart_flowcontrol_status[uart_port] == UART_FLOWCONTROL_SOFTWARE) {
            uartx = g_uart_regbase[uart_port];
            uart_put_char_block(uartx, g_uart_sw_flowcontrol_config[uart_port].xon);
            /* must delay until xon character is sent out */
            delay = ((1000000 * 12) / g_uart_config[uart_port].baudrate) + 1;
            hal_gpt_delay_us(delay);
            hal_uart_set_software_flowcontrol(uart_port,
                                              g_uart_sw_flowcontrol_config[uart_port].xon,
                                              g_uart_sw_flowcontrol_config[uart_port].xoff,
                                              g_uart_sw_flowcontrol_config[uart_port].escape_character);
        } else if (g_uart_flowcontrol_status[uart_port] == UART_FLOWCONTROL_HARDWARE) {
            hal_uart_set_hardware_flowcontrol(uart_port);
        }

        if (uart_hwstatus == UART_HWSTATUS_DMA_INITIALIZED) {
            hal_uart_set_dma(uart_port, &g_uart_dma_config[uart_port]);
            hal_uart_register_callback(uart_port, g_uart_callback[uart_port].func, g_uart_callback[uart_port].arg);
        }
        /* Workaround for sleep flow debug requirement */
        sleep_lock_status = hal_sleep_manager_is_sleep_handle_alive(g_uart_sleep_handler[uart_port]);
        if ((sleep_lock_status == true) && (g_uart_irq_pending_status[uart_port] != 0)) {
            /* sleep is locked and uart irq pending is active, then unlock one time */
            hal_sleep_manager_unlock_sleep(g_uart_sleep_handler[uart_port]);
        }
#ifdef HAL_UART_FEATURE_SOFT_DTE_SUPPORT
        /* re-configure DTE relative setting. */
        if (g_uart_dte_enable[uart_port] == true) {
            g_uart_dte_enable[uart_port] = false;
            hal_uart_set_dte_config(uart_port, &(g_uart_dte_config[uart_port].dte_config));
        }
#endif
    }
    g_uart_restore_init = false;
}
#endif

hal_uart_status_t hal_uart_set_baudrate(hal_uart_port_t uart_port, hal_uart_baudrate_t baudrate)
{
    uint32_t actual_baudrate, irq_status;
    UART_REGISTER_T *uartx;

    if ((!uart_port_is_valid(uart_port)) ||
        (!uart_baudrate_is_valid(baudrate))) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    if (g_uart_hwstatus[uart_port] == UART_HWSTATUS_UNINITIALIZED) {
        return HAL_UART_STATUS_ERROR_UNINITIALIZED;
    }

    irq_status = save_and_set_interrupt_mask();
#ifdef HAL_SLEEP_MANAGER_ENABLED
    g_uart_config[uart_port].baudrate = baudrate;
#endif
    restore_interrupt_mask(irq_status);

    uartx = g_uart_regbase[uart_port];
    actual_baudrate = g_uart_baudrate_map[baudrate];

    uart_set_baudrate(uartx, actual_baudrate);

    return HAL_UART_STATUS_OK;
}

hal_uart_status_t hal_uart_set_format(hal_uart_port_t uart_port,
                                      const hal_uart_config_t *config)
{
    uint32_t irq_status;
    UART_REGISTER_T *uartx;

    if ((!uart_port_is_valid(uart_port)) ||
        (!uart_config_is_valid(config))) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    if (g_uart_hwstatus[uart_port] == UART_HWSTATUS_UNINITIALIZED) {
        return HAL_UART_STATUS_ERROR_UNINITIALIZED;
    }

    irq_status = save_and_set_interrupt_mask();
#ifdef HAL_SLEEP_MANAGER_ENABLED
    g_uart_config[uart_port].baudrate = config->baudrate;
    g_uart_config[uart_port].word_length = config->word_length;
    g_uart_config[uart_port].stop_bit = config->stop_bit;
    g_uart_config[uart_port].parity = config->parity;
#endif
    restore_interrupt_mask(irq_status);

    uartx = g_uart_regbase[uart_port];

    hal_uart_set_baudrate(uart_port, config->baudrate);
    uart_set_format(uartx, config->word_length, config->stop_bit, config->parity);

    return HAL_UART_STATUS_OK;
}

hal_uart_status_t hal_uart_init(hal_uart_port_t uart_port, hal_uart_config_t *uart_config)
{
    UART_REGISTER_T *uartx;
    uint32_t i, actual_baudrate, irq_status;
    hal_clock_status_t status;

    if ((!uart_port_is_valid(uart_port)) ||
        (!uart_config_is_valid(uart_config))) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    irq_status = save_and_set_interrupt_mask();
    if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_UNINITIALIZED) {
        restore_interrupt_mask(irq_status);
        return HAL_UART_STATUS_ERROR_BUSY;
    }

#ifdef HAL_SLEEP_MANAGER_ENABLED
    /* Because hal_uart_init() may be called in two place,
       * We only get new handler when it's called not in wakeup flow.
       */
    if (g_uart_restore_init == false) {
        g_uart_sleep_handler[uart_port] = hal_sleep_manager_set_sleep_handle((char *)g_uart_sleep_handler_name[uart_port]);
    }
#endif

    if (g_uart_global_data_initialized == false) {
        for (i = 0; i < HAL_UART_MAX; i++) {
            g_uart_hwstatus[i] = UART_HWSTATUS_UNINITIALIZED;
#ifdef HAL_SLEEP_MANAGER_ENABLED
            g_uart_flowcontrol_status[i] = UART_FLOWCONTROL_NONE;
            g_uart_frist_send_complete_interrupt[i] = false;
            g_uart_send_lock_status[i] = false;
#endif
            g_uart_callback[i].arg = NULL;
            g_uart_callback[i].func = NULL;
        }
        g_uart_global_data_initialized = true;
    }
#ifdef HAL_SLEEP_MANAGER_ENABLED
    g_uart_config[uart_port].baudrate = uart_config->baudrate;
    g_uart_config[uart_port].word_length = uart_config->word_length;
    g_uart_config[uart_port].stop_bit = uart_config->stop_bit;
    g_uart_config[uart_port].parity = uart_config->parity;
#endif
    g_uart_hwstatus[uart_port] = UART_HWSTATUS_POLL_INITIALIZED;
    restore_interrupt_mask(irq_status);

    status = hal_clock_enable(g_uart_port_to_pdn[uart_port]);
    if (status != HAL_CLOCK_STATUS_OK) {
        UART_ASSERT();
        return HAL_UART_STATUS_ERROR;
    }

    uartx = g_uart_regbase[uart_port];

    uart_reset_default_value(uartx);
    actual_baudrate = g_uart_baudrate_map[uart_config->baudrate];
    uart_set_baudrate(uartx, actual_baudrate);
    uart_set_format(uartx, uart_config->word_length, uart_config->stop_bit, uart_config->parity);
    uart_set_fifo(uartx);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    uart_set_sleep_mode(uartx);
#endif

    return HAL_UART_STATUS_OK;
}

hal_uart_status_t hal_uart_deinit(hal_uart_port_t uart_port)
{
    UART_REGISTER_T *uartx;
    vdma_channel_t tx_dma_channel, rx_dma_channel;
    uint32_t irq_status;
    vdma_status_t status;
    hal_clock_status_t clock_status;

    if (!uart_port_is_valid(uart_port)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    irq_status = save_and_set_interrupt_mask();
    if (g_uart_hwstatus[uart_port] == UART_HWSTATUS_UNINITIALIZED) {
        restore_interrupt_mask(irq_status);
        return HAL_UART_STATUS_ERROR_UNINITIALIZED;
    }

    uartx = g_uart_regbase[uart_port];

    /* wait all left data sent out before deinit. */
    uart_query_empty(uartx);

    /* unregister vdma module */
    if (g_uart_hwstatus[uart_port] == UART_HWSTATUS_DMA_INITIALIZED) {
        tx_dma_channel = uart_port_to_dma_channel(uart_port, 0);
        rx_dma_channel = uart_port_to_dma_channel(uart_port, 1);

        status = vdma_disable_interrupt(tx_dma_channel);
        if (status != VDMA_OK) {
            restore_interrupt_mask(irq_status);
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
        status = vdma_disable_interrupt(rx_dma_channel);
        if (status != VDMA_OK) {
            restore_interrupt_mask(irq_status);
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
        status = vdma_stop(tx_dma_channel);
        if (status != VDMA_OK) {
            restore_interrupt_mask(irq_status);
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
        status = vdma_stop(rx_dma_channel);
        if (status != VDMA_OK) {
            restore_interrupt_mask(irq_status);
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
        status = vdma_deinit(tx_dma_channel);
        if (status != VDMA_OK) {
            restore_interrupt_mask(irq_status);
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
        status = vdma_deinit(rx_dma_channel);
        if (status != VDMA_OK) {
            restore_interrupt_mask(irq_status);
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
        hal_nvic_disable_irq(g_uart_port_to_irq_num[uart_port]);
    }
    uart_reset_default_value(uartx);
    clock_status = hal_clock_disable(g_uart_port_to_pdn[uart_port]);
    if (clock_status != HAL_CLOCK_STATUS_OK) {
        restore_interrupt_mask(irq_status);
        UART_ASSERT();
        return HAL_UART_STATUS_ERROR;
    }

    g_uart_callback[uart_port].func = NULL;
    g_uart_callback[uart_port].arg = NULL;

#ifdef HAL_SLEEP_MANAGER_ENABLED
    g_uart_frist_send_complete_interrupt[uart_port] = false;
    g_uart_flowcontrol_status[uart_port] = UART_FLOWCONTROL_NONE;
//    if (g_uart_restore_init == false) {
//        hal_sleep_manager_release_sleep_handle(g_uart_sleep_handler[uart_port]);
//    }
#endif

#ifdef HAL_UART_FEATURE_SOFT_DTE_SUPPORT
    if (g_uart_dte_enable[uart_port] == true) {
        uart_dte_reset(uart_port);
    }
#endif

    g_uart_hwstatus[uart_port] = UART_HWSTATUS_UNINITIALIZED;

    restore_interrupt_mask(irq_status);

    return HAL_UART_STATUS_OK;
}

void hal_uart_put_char(hal_uart_port_t uart_port, char byte)
{
    UART_REGISTER_T *uartx;

    if (!uart_port_is_valid(uart_port)) {
        return;
    }

    if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_POLL_INITIALIZED) {
        return;
    }

    uartx = g_uart_regbase[uart_port];
    uart_put_char_block(uartx, byte);
}

uint32_t hal_uart_send_polling(hal_uart_port_t uart_port, const uint8_t *data, uint32_t size)
{
    uint32_t i = 0;

    if ((!uart_port_is_valid(uart_port)) || (data == NULL)) {
        return 0;
    }

    if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_POLL_INITIALIZED) {
        return 0;
    }

    for (i = 0; i < size; i++) {
        hal_uart_put_char(uart_port, *data);
        data++;
    }

    return size;
}

uint32_t hal_uart_send_dma(hal_uart_port_t uart_port, const uint8_t *data, uint32_t size)
{
    vdma_channel_t channel;
    uint32_t i, real_count, avail_space;
    vdma_status_t status;
#ifdef HAL_SLEEP_MANAGER_ENABLED
    uint32_t irq_status;
#endif

    if ((!uart_port_is_valid(uart_port)) || (data == NULL) || (size == 0)) {
        return 0;
    }

    if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_DMA_INITIALIZED) {
        return 0;
    }

#ifdef HAL_SLEEP_MANAGER_ENABLED
    irq_status = save_and_set_interrupt_mask();
    if (g_uart_send_lock_status[uart_port] == false) {
        hal_sleep_manager_lock_sleep(g_uart_sleep_handler[uart_port]);
        g_uart_send_lock_status[uart_port] = true;
    }
    restore_interrupt_mask(irq_status);
#endif

    channel = uart_port_to_dma_channel(uart_port, 0);

    status = vdma_get_available_send_space(channel, &avail_space);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return 0;
    }

    if (avail_space >= size) {
        real_count = size;
    } else {
        real_count = avail_space;
    }
    for (i = 0; i < real_count; i++) {
        status = vdma_push_data(channel, data[i]);
        if (status != VDMA_OK) {
            UART_ASSERT();
            return 0;
        }
    }

    /* If avail space is not enough, turn on TX IRQ
       * so that UART driver can notice user when user's data has been sent out.
       */
    if (real_count == avail_space) {
        status = vdma_enable_interrupt(channel);
        if (status != VDMA_OK) {
            UART_ASSERT();
            return 0;
        }
    }

    return real_count;
}

char hal_uart_get_char(hal_uart_port_t uart_port)
{
    char data;
    UART_REGISTER_T *uartx;

    if (!uart_port_is_valid(uart_port)) {
        return 0;
    }

    if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_POLL_INITIALIZED) {
        return 0;
    }

    uartx = g_uart_regbase[uart_port];

    data = uart_get_char_block(uartx);

    return data;
}

uint32_t hal_uart_get_char_unblocking(hal_uart_port_t uart_port)
{
    uint32_t data;
    UART_REGISTER_T *uartx;

    if (!uart_port_is_valid(uart_port)) {
        return 0;
    }

    if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_POLL_INITIALIZED) {
        return 0;
    }

    uartx = g_uart_regbase[uart_port];

    data = uart_get_char_unblocking(uartx);

    return data;
}

uint32_t hal_uart_receive_polling(hal_uart_port_t uart_port, uint8_t *buffer, uint32_t size)
{
    uint32_t i;
    uint8_t *pbuf = buffer;

    if ((!uart_port_is_valid(uart_port)) ||
        (buffer == NULL)) {
        return 0;
    }

    if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_POLL_INITIALIZED) {
        return 0;
    }

    for (i = 0; i < size; i++) {
        pbuf[i] = hal_uart_get_char(uart_port);
    }

    return size;
}

uint32_t hal_uart_receive_dma(hal_uart_port_t uart_port, uint8_t *buffer, uint32_t size)
{
    vdma_channel_t channel;
    uint32_t receive_count, avail_count;
    uint32_t index;
    vdma_status_t status;

    if ((!uart_port_is_valid(uart_port)) ||
        (buffer == NULL) ||
        (size == 0)) {
        return 0;
    }

    if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_DMA_INITIALIZED) {
        return 0;
    }

    channel = uart_port_to_dma_channel(uart_port, 1);

    status = vdma_get_available_receive_bytes(channel, &avail_count);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return 0;
    }

    if (avail_count < size) {
        receive_count = avail_count;
    } else {
        receive_count = size;
    }
    for (index = 0; index < receive_count; index++) {
        status = vdma_pop_data(channel, &buffer[index]);
        if (status != VDMA_OK) {
            UART_ASSERT();
            return 0;
        }
    }

    /* If avail bytes is not enough, turn on RX IRQ
       * so that UART driver can notice user when new user's data has been received.
       */
    if (receive_count == avail_count) {
        status = vdma_enable_interrupt(channel);
        if (status != VDMA_OK) {
            UART_ASSERT();
            return 0;
        }
    }

    return receive_count;
}

static void uart_start_dma_transmission(hal_uart_port_t uart_port)
{
    UART_REGISTER_T *uartx;
    vdma_channel_t tx_dma_channel, rx_dma_channel;
    vdma_status_t status;

    uartx = g_uart_regbase[uart_port];
    tx_dma_channel = uart_port_to_dma_channel(uart_port, 0);
    rx_dma_channel = uart_port_to_dma_channel(uart_port, 1);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    uart_unmask_send_interrupt(uartx);
    g_uart_frist_send_complete_interrupt[uart_port] = true;
#endif
    uart_unmask_receive_interrupt(uartx);
    hal_nvic_enable_irq(g_uart_port_to_irq_num[uart_port]);
    status = vdma_enable_interrupt(tx_dma_channel);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return;
    }
    status = vdma_enable_interrupt(rx_dma_channel);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return;
    }
    status = vdma_start(tx_dma_channel);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return;
    }
    status = vdma_start(rx_dma_channel);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return;
    }
    uart_enable_dma(uartx);
}

hal_uart_status_t hal_uart_register_callback(hal_uart_port_t uart_port,
        hal_uart_callback_t user_callback,
        void *user_data)
{
    vdma_channel_t tx_dma_channel, rx_dma_channel;
    uint32_t irq_status;
    vdma_status_t status;
    hal_nvic_status_t nvic_status;

    if ((!uart_port_is_valid(uart_port)) ||
        (user_callback == NULL)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    irq_status = save_and_set_interrupt_mask();
    if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_POLL_INITIALIZED) {
        restore_interrupt_mask(irq_status);
        return HAL_UART_STATUS_ERROR_UNINITIALIZED;
    }

    tx_dma_channel = uart_port_to_dma_channel(uart_port, 0);
    rx_dma_channel = uart_port_to_dma_channel(uart_port, 1);

    g_uart_callback[uart_port].func = user_callback;
    g_uart_callback[uart_port].arg = user_data;

    uart_dma_channel_to_callback_data(tx_dma_channel, &g_uart_dma_callback_data[uart_port * 2]);
    status = vdma_register_callback(tx_dma_channel, uart_dma_callback_handler, &g_uart_dma_callback_data[uart_port * 2]);
    if (status != VDMA_OK) {
        restore_interrupt_mask(irq_status);
        UART_ASSERT();
        return HAL_UART_STATUS_ERROR;
    }
    uart_dma_channel_to_callback_data(rx_dma_channel, &g_uart_dma_callback_data[(uart_port * 2) + 1]);
    status = vdma_register_callback(rx_dma_channel, uart_dma_callback_handler, &g_uart_dma_callback_data[(uart_port * 2) + 1]);
    if (status != VDMA_OK) {
        restore_interrupt_mask(irq_status);
        UART_ASSERT();
        return HAL_UART_STATUS_ERROR;
    }
    nvic_status = hal_nvic_register_isr_handler(g_uart_port_to_irq_num[uart_port], uart_interrupt_handler);
    if (nvic_status != HAL_NVIC_STATUS_OK) {
        restore_interrupt_mask(irq_status);
        UART_ASSERT();
        return HAL_UART_STATUS_ERROR;
    }

    uart_start_dma_transmission(uart_port);

    g_uart_hwstatus[uart_port] = UART_HWSTATUS_DMA_INITIALIZED;

    restore_interrupt_mask(irq_status);

    return HAL_UART_STATUS_OK;
}

uint32_t hal_uart_get_available_send_space(hal_uart_port_t uart_port)
{
    vdma_channel_t channel;
    uint32_t roomleft;
    vdma_status_t status;

    if (!uart_port_is_valid(uart_port)) {
        return 0;
    }

    if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_DMA_INITIALIZED) {
        return 0;
    }

    channel = uart_port_to_dma_channel(uart_port, 0);
    status = vdma_get_available_send_space(channel, &roomleft);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return 0;
    }

    return roomleft;
}

uint32_t hal_uart_get_available_receive_bytes(hal_uart_port_t uart_port)
{
    vdma_channel_t channel;
    uint32_t avail;
    vdma_status_t status;

    if (!uart_port_is_valid(uart_port)) {
        return 0;
    }

    if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_DMA_INITIALIZED) {
        return 0;
    }

    channel = uart_port_to_dma_channel(uart_port, 1);
    status = vdma_get_available_receive_bytes(channel, &avail);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return 0;
    }

    return avail;
}

hal_uart_status_t hal_uart_set_hardware_flowcontrol(hal_uart_port_t uart_port)
{
    UART_REGISTER_T *uartx;

    if (!uart_port_is_valid(uart_port)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    if (g_uart_hwstatus[uart_port] == UART_HWSTATUS_UNINITIALIZED) {
        return HAL_UART_STATUS_ERROR_UNINITIALIZED;
    }

    uartx = g_uart_regbase[uart_port];

    uart_set_hardware_flowcontrol(uartx);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    g_uart_flowcontrol_status[uart_port] = UART_FLOWCONTROL_HARDWARE;
#endif

    return HAL_UART_STATUS_OK;
}

hal_uart_status_t hal_uart_set_software_flowcontrol(hal_uart_port_t uart_port,
        uint8_t xon,
        uint8_t xoff,
        uint8_t escape_character)
{
    UART_REGISTER_T *uartx;

    if (!uart_port_is_valid(uart_port)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    if (g_uart_hwstatus[uart_port] == UART_HWSTATUS_UNINITIALIZED) {
        return HAL_UART_STATUS_ERROR_UNINITIALIZED;
    }

    uartx = g_uart_regbase[uart_port];

    uart_set_software_flowcontrol(uartx, xon, xoff, escape_character);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    g_uart_flowcontrol_status[uart_port] = UART_FLOWCONTROL_SOFTWARE;
    g_uart_sw_flowcontrol_config[uart_port].xon = xon;
    g_uart_sw_flowcontrol_config[uart_port].xoff = xoff;
    g_uart_sw_flowcontrol_config[uart_port].escape_character = escape_character;
#endif

    return HAL_UART_STATUS_OK;
}

hal_uart_status_t hal_uart_disable_flowcontrol(hal_uart_port_t uart_port)
{
    UART_REGISTER_T *uartx;

    if (!uart_port_is_valid(uart_port)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    if (g_uart_hwstatus[uart_port] == UART_HWSTATUS_UNINITIALIZED) {
        return HAL_UART_STATUS_ERROR_UNINITIALIZED;
    }

    uartx = g_uart_regbase[uart_port];

    uart_disable_flowcontrol(uartx);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    g_uart_flowcontrol_status[uart_port] = UART_FLOWCONTROL_NONE;
#endif

    return HAL_UART_STATUS_OK;
}


hal_uart_status_t hal_uart_set_dma(hal_uart_port_t uart_port, const hal_uart_dma_config_t *dma_config)
{
    uint32_t irq_status;
    vdma_config_t internal_dma_config;
    vdma_channel_t tx_dma_channel, rx_dma_channel;
    vdma_status_t status;

    if (!uart_port_is_valid(uart_port)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }
    if ((dma_config->send_vfifo_buffer == NULL) ||
        (dma_config->receive_vfifo_buffer == NULL)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }
    if ((dma_config->send_vfifo_buffer_size >= UART_DMA_MAX_SETTING_VALUE) ||
        (dma_config->send_vfifo_threshold_size >= UART_DMA_MAX_SETTING_VALUE) ||
        (dma_config->send_vfifo_threshold_size > dma_config->send_vfifo_buffer_size)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }
    if ((dma_config->receive_vfifo_buffer_size >= UART_DMA_MAX_SETTING_VALUE) ||
        (dma_config->receive_vfifo_threshold_size >= UART_DMA_MAX_SETTING_VALUE) ||
        (dma_config->receive_vfifo_alert_size >= UART_DMA_MAX_SETTING_VALUE) ||
        (dma_config->receive_vfifo_threshold_size > dma_config->receive_vfifo_buffer_size) ||
        (dma_config->receive_vfifo_alert_size > dma_config->receive_vfifo_buffer_size)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    if (g_uart_hwstatus[uart_port] != UART_HWSTATUS_POLL_INITIALIZED) {
        return HAL_UART_STATUS_ERROR_UNINITIALIZED;
    }

    tx_dma_channel = uart_port_to_dma_channel(uart_port, 0);
    rx_dma_channel = uart_port_to_dma_channel(uart_port, 1);

    status = vdma_init(tx_dma_channel);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return HAL_UART_STATUS_ERROR;
    }
    internal_dma_config.base_address = (uint32_t)dma_config->send_vfifo_buffer;
    internal_dma_config.size = dma_config->send_vfifo_buffer_size;
    status = vdma_configure(tx_dma_channel, &internal_dma_config);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return HAL_UART_STATUS_ERROR;
    }
    status = vdma_set_threshold(tx_dma_channel, dma_config->send_vfifo_threshold_size);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return HAL_UART_STATUS_ERROR;
    }

    status = vdma_init(rx_dma_channel);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return HAL_UART_STATUS_ERROR;
    }
    internal_dma_config.base_address = (uint32_t)dma_config->receive_vfifo_buffer;
    internal_dma_config.size = dma_config->receive_vfifo_buffer_size;
    status = vdma_configure(rx_dma_channel, &internal_dma_config);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return HAL_UART_STATUS_ERROR;
    }
    status = vdma_set_threshold(rx_dma_channel, dma_config->receive_vfifo_threshold_size);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return HAL_UART_STATUS_ERROR;
    }
    status = vdma_set_alert_length(rx_dma_channel, dma_config->receive_vfifo_alert_size);
    if (status != VDMA_OK) {
        UART_ASSERT();
        return HAL_UART_STATUS_ERROR;
    }

    irq_status = save_and_set_interrupt_mask();
    g_uart_dma_config[uart_port].send_vfifo_buffer = dma_config->send_vfifo_buffer;
    g_uart_dma_config[uart_port].send_vfifo_buffer_size = dma_config->send_vfifo_buffer_size;
    g_uart_dma_config[uart_port].send_vfifo_threshold_size = dma_config->send_vfifo_threshold_size;
    g_uart_dma_config[uart_port].receive_vfifo_alert_size = dma_config->receive_vfifo_alert_size;
    g_uart_dma_config[uart_port].receive_vfifo_buffer = dma_config->receive_vfifo_buffer;
    g_uart_dma_config[uart_port].receive_vfifo_buffer_size = dma_config->receive_vfifo_buffer_size;
    g_uart_dma_config[uart_port].receive_vfifo_threshold_size = dma_config->receive_vfifo_threshold_size;
    restore_interrupt_mask(irq_status);

    return HAL_UART_STATUS_OK;
}

#ifdef HAL_UART_FEATURE_SOFT_DTE_SUPPORT

static volatile bool g_uart_dte_config_init_flag = false;

static void uart_ri_eint_handler(void *user_data)
{
    hal_uart_dte_callback_t callback;
    void *arg;
    hal_gpio_data_t gpio_data;
    hal_uart_dte_config_t *user_config;
    uart_dte_config_t *dte_config;
    hal_eint_status_t eint_status;

    dte_config = (uart_dte_config_t *)user_data;
    user_config = &(dte_config->dte_config);

    callback = user_config->user_callback;
    arg = user_config->user_data;
    if (callback == NULL) {
        UART_ASSERT();
        return;
    }

    hal_gpio_get_input(dte_config->ri_pin, &gpio_data);
    if (gpio_data == HAL_GPIO_DATA_LOW) {
        callback(HAL_UART_DTE_EVENT_RI_LOW, arg);
    } else {
        callback(HAL_UART_DTE_EVENT_RI_HIGH, arg);
    }

    eint_status = hal_eint_unmask(dte_config->ri_eint);
    if (eint_status != HAL_EINT_STATUS_OK) {
        UART_ASSERT();
        return;
    }
}

static void uart_dsr_eint_handler(void *user_data)
{
    hal_uart_dte_callback_t callback;
    void *arg;
    hal_gpio_data_t gpio_data;
    hal_uart_dte_config_t *user_config;
    uart_dte_config_t *dte_config;
    hal_eint_status_t eint_status;

    dte_config = (uart_dte_config_t *)user_data;
    user_config = &(dte_config->dte_config);

    callback = user_config->user_callback;
    arg = user_config->user_data;
    if (callback == NULL) {
        UART_ASSERT();
        return;
    }

    hal_gpio_get_input(dte_config->dsr_pin, &gpio_data);
    if (gpio_data == HAL_GPIO_DATA_LOW) {
        if (user_config->dsr_is_active_high == true) {
            callback(HAL_UART_DTE_EVENT_DSR_DEACTIVE, arg);
        } else {
            callback(HAL_UART_DTE_EVENT_DSR_ACTIVE, arg);
        }
    } else {
        if (user_config->dsr_is_active_high == true) {
            callback(HAL_UART_DTE_EVENT_DSR_ACTIVE, arg);
        } else {
            callback(HAL_UART_DTE_EVENT_DSR_DEACTIVE, arg);
        }
    }

    eint_status = hal_eint_unmask(dte_config->dsr_eint);
    if (eint_status != HAL_EINT_STATUS_OK) {
        UART_ASSERT();
        return;
    }
}

static void uart_dcd_eint_handler(void *user_data)
{
    hal_uart_dte_callback_t callback;
    void *arg;
    hal_gpio_data_t gpio_data;
    hal_uart_dte_config_t *user_config;
    uart_dte_config_t *dte_config;
    hal_eint_status_t eint_status;

    dte_config = (uart_dte_config_t *)user_data;
    user_config = &(dte_config->dte_config);

    callback = user_config->user_callback;
    arg = user_config->user_data;
    if (callback == NULL) {
        UART_ASSERT();
        return;
    }

    hal_gpio_get_input(dte_config->dcd_pin, &gpio_data);
    if (gpio_data == HAL_GPIO_DATA_LOW) {
        if (user_config->dcd_is_active_high == true) {
            callback(HAL_UART_DTE_EVENT_DCD_DEACTIVE, arg);
        } else {
            callback(HAL_UART_DTE_EVENT_DCD_ACTIVE, arg);
        }
    } else {
        if (user_config->dcd_is_active_high == true) {
            callback(HAL_UART_DTE_EVENT_DCD_ACTIVE, arg);
        } else {
            callback(HAL_UART_DTE_EVENT_DCD_DEACTIVE, arg);
        }
    }

    eint_status = hal_eint_unmask(dte_config->dcd_eint);
    if (eint_status != HAL_EINT_STATUS_OK) {
        UART_ASSERT();
        return;
    }
}

static bool uart_dte_config_is_valid(hal_uart_dte_config_t *uart_dte_config)
{
    return (uart_dte_config->user_callback != NULL);
}

/* called in hal_uart_deinit() */
static void uart_dte_reset(hal_uart_port_t uart_port)
{
    uart_dte_config_t *dte_config;

    dte_config = &g_uart_dte_config[uart_port];

    hal_eint_deinit(dte_config->dcd_eint);
    hal_eint_deinit(dte_config->dsr_eint);
    hal_eint_deinit(dte_config->ri_eint);

    dte_config->dte_config.user_callback = NULL;
    dte_config->dte_config.user_data = NULL;

    g_uart_dte_enable[uart_port] = false;
}

/* Call this function to config DTE feature for the UART port */
hal_uart_status_t hal_uart_set_dte_config(hal_uart_port_t uart_port,
        hal_uart_dte_config_t *uart_dte_config)
{
    hal_eint_config_t eint_config;
    hal_eint_status_t eint_status;
    uart_dte_config_t *dte_config;
    uint32_t irq_status;

    if ((!uart_port_is_valid(uart_port)) ||
        (!uart_dte_config_is_valid(uart_dte_config))) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    if (g_uart_hwstatus[uart_port] == UART_HWSTATUS_UNINITIALIZED) {
        return HAL_UART_STATUS_ERROR_UNINITIALIZED;
    }

    irq_status = save_and_set_interrupt_mask();
    if (g_uart_dte_enable[uart_port] == true) {
        restore_interrupt_mask(irq_status);
        return HAL_UART_STATUS_ERROR_BUSY;
    }
    g_uart_dte_enable[uart_port] = true;
    restore_interrupt_mask(irq_status);

    if (g_uart_dte_config_init_flag == false) {
        uart_dte_init_config(g_uart_dte_config);
        g_uart_dte_config_init_flag = true;
    }

    dte_config = &g_uart_dte_config[uart_port];

    dte_config->dte_config = *uart_dte_config;

    /* config EINT for RI pin */
    if (dte_config->ri_eint != 0xff) {
        hal_eint_mask(dte_config->ri_eint);
        eint_config.trigger_mode = HAL_EINT_EDGE_FALLING_AND_RISING;
        eint_config.debounce_time = uart_dte_config->ri_debounce_time;
        eint_status = hal_eint_init(dte_config->ri_eint, &eint_config);
        if (eint_status != HAL_EINT_STATUS_OK) {
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
        eint_status = hal_eint_register_callback(dte_config->ri_eint, uart_ri_eint_handler, dte_config);
        if (eint_status != HAL_EINT_STATUS_OK) {
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
        eint_status = hal_eint_unmask(dte_config->ri_eint);
        if (eint_status != HAL_EINT_STATUS_OK) {
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
    }

    /* config EINT for DCD pin */
    if (dte_config->dcd_eint != 0xff) {
        hal_eint_mask(dte_config->dcd_eint);
        eint_config.trigger_mode = HAL_EINT_EDGE_FALLING_AND_RISING;
        eint_config.debounce_time = uart_dte_config->dcd_debounce_time;
        eint_status = hal_eint_init(dte_config->dcd_eint, &eint_config);
        if (eint_status != HAL_EINT_STATUS_OK) {
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
        eint_status = hal_eint_register_callback(dte_config->dcd_eint, uart_dcd_eint_handler, dte_config);
        if (eint_status != HAL_EINT_STATUS_OK) {
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
        eint_status = hal_eint_unmask(dte_config->dcd_eint);
        if (eint_status != HAL_EINT_STATUS_OK) {
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
    }

    /* config EINT for DSR pin */
    if (dte_config->dsr_eint != 0xff) {
        hal_eint_mask(dte_config->dsr_eint);
        eint_config.trigger_mode = HAL_EINT_EDGE_FALLING_AND_RISING;
        eint_config.debounce_time = uart_dte_config->dsr_debounce_time;
        eint_status = hal_eint_init(dte_config->dsr_eint, &eint_config);
        if (eint_status != HAL_EINT_STATUS_OK) {
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
        eint_status = hal_eint_register_callback(dte_config->dsr_eint, uart_dsr_eint_handler, dte_config);
        if (eint_status != HAL_EINT_STATUS_OK) {
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
        eint_status = hal_eint_unmask(dte_config->dsr_eint);
        if (eint_status != HAL_EINT_STATUS_OK) {
            UART_ASSERT();
            return HAL_UART_STATUS_ERROR;
        }
    }

    return HAL_UART_STATUS_OK;
}

/* Call this function to change DTR pin to active status */
hal_uart_status_t hal_uart_set_dte_dtr_active(hal_uart_port_t uart_port)
{
    uart_dte_config_t *dte_config;
    hal_uart_dte_config_t *user_config;

    if (!uart_port_is_valid(uart_port)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    dte_config = &g_uart_dte_config[uart_port];
    user_config = &(dte_config->dte_config);
    if (user_config->dtr_is_active_high == true) {
        hal_gpio_set_output(dte_config->dtr_pin, HAL_GPIO_DATA_HIGH);
    } else {
        hal_gpio_set_output(dte_config->dtr_pin, HAL_GPIO_DATA_LOW);
    }

    return HAL_UART_STATUS_OK;
}

/* Call this function to change DTR pin to de-active status */
hal_uart_status_t hal_uart_set_dte_dtr_deactive(hal_uart_port_t uart_port)
{
    uart_dte_config_t *dte_config;
    hal_uart_dte_config_t *user_config;

    if (!uart_port_is_valid(uart_port)) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }

    dte_config = &g_uart_dte_config[uart_port];
    user_config = &(dte_config->dte_config);
    if (user_config->dtr_is_active_high == true) {
        hal_gpio_set_output(dte_config->dtr_pin, HAL_GPIO_DATA_LOW);
    } else {
        hal_gpio_set_output(dte_config->dtr_pin, HAL_GPIO_DATA_HIGH);
    }

    return HAL_UART_STATUS_OK;
}

#endif
#ifdef __cplusplus
}
#endif

#endif

