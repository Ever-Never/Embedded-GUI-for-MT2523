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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* hal includes */
#include "hal.h"
#include "memory_attribute.h"
#include "system_mt7687.h"
#include "top.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define VFIFO_SIZE (256)
#define SEND_THRESHOLD_SIZE (50)
#define RECEIVE_THRESHOLD_SIZE (150)
#define RECEIVE_ALERT_SIZE (30)
#define UART_PROMPT_INFO "\r\n\r\nPlease input data to this UART port and watch it's output:\r\n"
#define UART_PROMPT_INFO_SIZE sizeof(UART_PROMPT_INFO)

/* Private variables ---------------------------------------------------------*/

/* Buffer used by UART driver in dma mode must declare with ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN. */
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_uart_send_buffer[VFIFO_SIZE];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_uart_receive_buffer[VFIFO_SIZE];
static volatile bool g_uart_receive_event = false;

/* Private functions ---------------------------------------------------------*/

#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the HAL_UART_0 one at a time */
    return ch;
}

/**
*@brief Configure and initialize the systerm clock.
*@param None.
*@return None.
*/
static void SystemClock_Config(void)
{
    top_xtal_init();
}

/**
*@brief  Initialize the periperal driver in this function.
*@param None.
*@return None.
*/
static void prvSetupHardware(void)
{
    /* System HW initialization */

    /* Peripherals initialization */

    /* Board HW initialization */
}

/**
*@brief  User defined callback.
*@param[in] user_data: pointer to the data that user will use in this callback.
*@param[in] event: This enum shows UART event when an interrupt occurs.
*@return None.
*/
static void uart_read_from_input(hal_uart_callback_event_t event, void *user_data)
{
    if (event == HAL_UART_EVENT_READY_TO_READ) {
        g_uart_receive_event = true;
    }
}

/**
*@brief  Example of UART loopback data with the dma mode.
*@param None.
*@return None.
*/
static void uart_loopback_data_dma_example(void)
{
    hal_uart_config_t basic_config;
    hal_uart_dma_config_t dma_config;
    uint8_t buffer[VFIFO_SIZE];
    uint32_t length;

    /* Step1: Call hal_pinmux_set_function() to set GPIO pinmux, if EPT tool was not used to configure the related pinmux.*/
    hal_gpio_init(HAL_GPIO_2);
    hal_gpio_init(HAL_GPIO_3);
    hal_pinmux_set_function(HAL_GPIO_2, HAL_GPIO_2_UART1_RX_CM4);
    hal_pinmux_set_function(HAL_GPIO_3, HAL_GPIO_3_UART1_TX_CM4);

    /* Configure UART port with basic function */
    basic_config.baudrate = HAL_UART_BAUDRATE_115200;
    basic_config.parity = HAL_UART_PARITY_NONE;
    basic_config.stop_bit = HAL_UART_STOP_BIT_1;
    basic_config.word_length = HAL_UART_WORD_LENGTH_8;
    hal_uart_init(HAL_UART_0, &basic_config);

    /*Step2: Configure UART port to dma mode. */
    dma_config.receive_vfifo_alert_size = RECEIVE_ALERT_SIZE;
    dma_config.receive_vfifo_buffer = g_uart_receive_buffer;
    dma_config.receive_vfifo_buffer_size = VFIFO_SIZE;
    dma_config.receive_vfifo_threshold_size = RECEIVE_THRESHOLD_SIZE;
    dma_config.send_vfifo_buffer = g_uart_send_buffer;
    dma_config.send_vfifo_buffer_size = VFIFO_SIZE;
    dma_config.send_vfifo_threshold_size = SEND_THRESHOLD_SIZE;
    hal_uart_set_dma(HAL_UART_0, &dma_config);
    hal_uart_register_callback(HAL_UART_0, uart_read_from_input, NULL);

    /* Print the prompt content to the test port */
    hal_uart_send_dma(HAL_UART_0, (const uint8_t *)UART_PROMPT_INFO, UART_PROMPT_INFO_SIZE);

    /*Step3: Loop the data received from the UART input to its output */
    while (1) {
        if (g_uart_receive_event == true) {
            length = hal_uart_get_available_receive_bytes(HAL_UART_0);
            hal_uart_receive_dma(HAL_UART_0, buffer, length);
            hal_uart_send_dma(HAL_UART_0, buffer, length);
            g_uart_receive_event = false;
        }
    }
}

int main(void)
{
    /* Configure system clock */
    SystemClock_Config();

    /* Configure the hardware */
    prvSetupHardware();

    /* Enable I,F bits */
    __enable_irq();
    __enable_fault_irq();

    /* Enter uart example function */
    uart_loopback_data_dma_example();

    return 0;
}

