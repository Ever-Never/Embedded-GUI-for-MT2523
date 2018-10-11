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

#include "hal_pdma_internal.h"
#include "hal_nvic.h"
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

UART_REGISTER_T *const g_uart_regbase[] = {UART0, UART1, UART2, UART3};
const hal_clock_cg_id g_uart_port_to_pdn[] = {HAL_CLOCK_CG_UART0, HAL_CLOCK_CG_UART1, HAL_CLOCK_CG_UART2, HAL_CLOCK_CG_UART3};
const hal_nvic_irq_t g_uart_port_to_irq_num[] = {UART0_IRQn, UART1_IRQn, UART2_IRQn, UART3_IRQn};
#ifdef HAL_SLEEP_MANAGER_ENABLED
const char *const g_uart_sleep_handler_name[] = {"uart0", "uart1", "uart2", "uart3"};
#endif

#ifdef HAL_UART_FEATURE_SOFT_DTE_SUPPORT
/* extern GPIOs and EINTs var declare */
extern const unsigned char HAL_UART0_DTE_RI_EINT;
extern const unsigned char HAL_UART0_DTE_DCD_EINT;
extern const unsigned char HAL_UART0_DTE_DSR_EINT;
extern const char HAL_UART0_DTE_DCD_PIN;
extern const char HAL_UART0_DTE_DSR_PIN;
extern const char HAL_UART0_DTE_DTR_PIN;
extern const char HAL_UART0_DTE_RI_PIN;

extern const unsigned char HAL_UART1_DTE_RI_EINT;
extern const unsigned char HAL_UART1_DTE_DCD_EINT;
extern const unsigned char HAL_UART1_DTE_DSR_EINT;
extern const char HAL_UART1_DTE_DCD_PIN;
extern const char HAL_UART1_DTE_DSR_PIN;
extern const char HAL_UART1_DTE_DTR_PIN;
extern const char HAL_UART1_DTE_RI_PIN;

extern const unsigned char HAL_UART2_DTE_RI_EINT;
extern const unsigned char HAL_UART2_DTE_DCD_EINT;
extern const unsigned char HAL_UART2_DTE_DSR_EINT;
extern const char HAL_UART2_DTE_DCD_PIN;
extern const char HAL_UART2_DTE_DSR_PIN;
extern const char HAL_UART2_DTE_DTR_PIN;
extern const char HAL_UART2_DTE_RI_PIN;

extern const unsigned char HAL_UART3_DTE_RI_EINT;
extern const unsigned char HAL_UART3_DTE_DCD_EINT;
extern const unsigned char HAL_UART3_DTE_DSR_EINT;
extern const char HAL_UART3_DTE_DCD_PIN;
extern const char HAL_UART3_DTE_DSR_PIN;
extern const char HAL_UART3_DTE_DTR_PIN;
extern const char HAL_UART3_DTE_RI_PIN;

void uart_dte_init_config(uart_dte_config_t *uart_dte_config)
{
    uart_dte_config[HAL_UART_0].dsr_pin = (hal_gpio_pin_t)HAL_UART0_DTE_DSR_PIN;
    uart_dte_config[HAL_UART_0].dcd_pin = (hal_gpio_pin_t)HAL_UART0_DTE_DCD_PIN;
    uart_dte_config[HAL_UART_0].dtr_pin = (hal_gpio_pin_t)HAL_UART0_DTE_DTR_PIN;
    uart_dte_config[HAL_UART_0].ri_pin = (hal_gpio_pin_t)HAL_UART0_DTE_RI_PIN;
    uart_dte_config[HAL_UART_0].dsr_eint = (hal_eint_number_t)HAL_UART0_DTE_DSR_EINT;
    uart_dte_config[HAL_UART_0].ri_eint = (hal_eint_number_t)HAL_UART0_DTE_RI_EINT;
    uart_dte_config[HAL_UART_0].dcd_eint = (hal_eint_number_t)HAL_UART0_DTE_DCD_EINT;
    uart_dte_config[HAL_UART_0].uart_port = HAL_UART_0;

    uart_dte_config[HAL_UART_1].dsr_pin = (hal_gpio_pin_t)HAL_UART1_DTE_DSR_PIN;
    uart_dte_config[HAL_UART_1].dcd_pin = (hal_gpio_pin_t)HAL_UART1_DTE_DCD_PIN;
    uart_dte_config[HAL_UART_1].dtr_pin = (hal_gpio_pin_t)HAL_UART1_DTE_DTR_PIN;
    uart_dte_config[HAL_UART_1].ri_pin = (hal_gpio_pin_t)HAL_UART1_DTE_RI_PIN;
    uart_dte_config[HAL_UART_1].dsr_eint = (hal_eint_number_t)HAL_UART1_DTE_DSR_EINT;
    uart_dte_config[HAL_UART_1].ri_eint = (hal_eint_number_t)HAL_UART1_DTE_RI_EINT;
    uart_dte_config[HAL_UART_1].dcd_eint = (hal_eint_number_t)HAL_UART1_DTE_DCD_EINT;
    uart_dte_config[HAL_UART_1].uart_port = HAL_UART_1;

    uart_dte_config[HAL_UART_2].dsr_pin = (hal_gpio_pin_t)HAL_UART2_DTE_DSR_PIN;
    uart_dte_config[HAL_UART_2].dcd_pin = (hal_gpio_pin_t)HAL_UART2_DTE_DCD_PIN;
    uart_dte_config[HAL_UART_2].dtr_pin = (hal_gpio_pin_t)HAL_UART2_DTE_DTR_PIN;
    uart_dte_config[HAL_UART_2].ri_pin = (hal_gpio_pin_t)HAL_UART2_DTE_RI_PIN;
    uart_dte_config[HAL_UART_2].dsr_eint = (hal_eint_number_t)HAL_UART2_DTE_DSR_EINT;
    uart_dte_config[HAL_UART_2].ri_eint = (hal_eint_number_t)HAL_UART2_DTE_RI_EINT;
    uart_dte_config[HAL_UART_2].dcd_eint = (hal_eint_number_t)HAL_UART2_DTE_DCD_EINT;
    uart_dte_config[HAL_UART_2].uart_port = HAL_UART_2;

    uart_dte_config[HAL_UART_3].dsr_pin = (hal_gpio_pin_t)HAL_UART3_DTE_DSR_PIN;
    uart_dte_config[HAL_UART_3].dcd_pin = (hal_gpio_pin_t)HAL_UART3_DTE_DCD_PIN;
    uart_dte_config[HAL_UART_3].dtr_pin = (hal_gpio_pin_t)HAL_UART3_DTE_DTR_PIN;
    uart_dte_config[HAL_UART_3].ri_pin = (hal_gpio_pin_t)HAL_UART3_DTE_RI_PIN;
    uart_dte_config[HAL_UART_3].dsr_eint = (hal_eint_number_t)HAL_UART3_DTE_DSR_EINT;
    uart_dte_config[HAL_UART_3].ri_eint = (hal_eint_number_t)HAL_UART3_DTE_RI_EINT;
    uart_dte_config[HAL_UART_3].dcd_eint = (hal_eint_number_t)HAL_UART3_DTE_DCD_EINT;
    uart_dte_config[HAL_UART_3].uart_port = HAL_UART_3;
}
#endif

vdma_channel_t uart_port_to_dma_channel(hal_uart_port_t uart_port, int32_t is_rx)
{
    vdma_channel_t dma_channel = VDMA_UART0RX;

    switch (uart_port) {
        case HAL_UART_0:
            if (is_rx) {
                dma_channel = VDMA_UART0RX;
            } else {
                dma_channel = VDMA_UART0TX;
            }
            break;
        case HAL_UART_1:
            if (is_rx) {
                dma_channel = VDMA_UART1RX;
            } else {
                dma_channel = VDMA_UART1TX;
            }
            break;
        case HAL_UART_2:
            if (is_rx) {
                dma_channel = VDMA_UART2RX;
            } else {
                dma_channel = VDMA_UART2TX;
            }
            break;
        case HAL_UART_3:
            if (is_rx) {
                dma_channel = VDMA_UART3RX;
            } else {
                dma_channel = VDMA_UART3TX;
            }
            break;
        default:
            break;
    }

    return dma_channel;
}

void uart_dma_channel_to_callback_data(vdma_channel_t dma_channel, uart_dma_callback_data_t *user_data)
{
    switch (dma_channel) {
        case VDMA_UART0RX:
            user_data->is_rx = true;
            user_data->uart_port = HAL_UART_0;
            break;
        case VDMA_UART1RX:
            user_data->is_rx = true;
            user_data->uart_port = HAL_UART_1;
            break;
        case VDMA_UART2RX:
            user_data->is_rx = true;
            user_data->uart_port = HAL_UART_2;
            break;
        case VDMA_UART3RX:
            user_data->is_rx = true;
            user_data->uart_port = HAL_UART_3;
            break;
        case VDMA_UART0TX:
            user_data->is_rx = false;
            user_data->uart_port = HAL_UART_0;
            break;
        case VDMA_UART1TX:
            user_data->is_rx = false;
            user_data->uart_port = HAL_UART_1;
            break;
        case VDMA_UART2TX:
            user_data->is_rx = false;
            user_data->uart_port = HAL_UART_2;
            break;
        case VDMA_UART3TX:
            user_data->is_rx = false;
            user_data->uart_port = HAL_UART_3;
            break;
        default:
            break;
    }
}

void uart_enable_dma(UART_REGISTER_T *uartx)
{
    uartx->DMA_EN = UART_DMA_EN_RX_DMA_EN_MASK | UART_DMA_EN_TX_DMA_EN_MASK;
}

void uart_interrupt_handler(hal_nvic_irq_t irq_number)
{
    hal_uart_port_t uart_port;
    uart_interrupt_type_t type;
    UART_REGISTER_T *uartx = UART0;

    switch (irq_number) {
        case UART0_IRQn:
            uart_port = HAL_UART_0;
            break;
        case UART1_IRQn:
            uart_port = HAL_UART_1;
            break;
        case UART2_IRQn:
            uart_port = HAL_UART_2;
            break;
        case UART3_IRQn:
            uart_port = HAL_UART_3;
            break;
        default:
            uart_port = HAL_UART_0;
    }

    uartx = g_uart_regbase[uart_port];
    type = uart_query_interrupt_type(uartx);

    switch (type) {
        /* received data and timeout happen */
        case UART_INTERRUPT_RECEIVE_TIMEOUT:
            uart_receive_handler(uart_port, true);
            break;
        /* receive line status changed Any of BI/FE/PE/OE becomes set */
        case UART_INTERRUPT_RECEIVE_ERROR:
            uart_error_handler(uart_port);
            break;
        /* received data or received Trigger level reached */
        case UART_INTERRUPT_RECEIVE_AVAILABLE:
            //uart_receive_handler(uart_port, false);
            break;
        /* false interrupt detect */
        case UART_INTERRUPT_NONE:
            break;
        /* received break signal */
        case UART_INTERRUPT_RECEIVE_BREAK:
            uart_purge_fifo(uartx, 1);
            break;
        /* TX holding register is empty or the TX FIFO reduce to it's trigger level */
        case UART_INTERRUPT_SEND_AVAILABLE:
#ifdef HAL_SLEEP_MANAGER_ENABLED
            uart_send_handler(uart_port, true);
            break;
#endif
        /* detect hardware flow control request (CTS is high) */
        case UART_INTERRUPT_HARDWARE_FLOWCONTROL:
        /* an XOFF character has been received */
        case UART_INTERRUPT_SOFTWARE_FLOWCONTROL:
        default:
            UART_ASSERT();
    }
}

#ifdef __cplusplus
}
#endif

#endif

