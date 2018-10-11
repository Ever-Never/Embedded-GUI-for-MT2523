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
#include "system_mt7687.h"
#include "top.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
hal_gpt_port_t           gpt_port;
/* Private functions ---------------------------------------------------------*/

#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the HAL_UART_0 one at a time */
    hal_uart_put_char(HAL_UART_0, ch);
    return ch;
}

/**
*@brief Set pinmux to UART and initialize UART hardware initialization for logging.
*@param None.
*@return None.
*/
static void log_uart_init(void)
{
    hal_uart_config_t uart_config;
    /* Set Pinmux to UART */
    hal_pinmux_set_function(HAL_GPIO_0, HAL_GPIO_0_UART1_RTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_1, HAL_GPIO_1_UART1_CTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_2, HAL_GPIO_2_UART1_RX_CM4);
    hal_pinmux_set_function(HAL_GPIO_3, HAL_GPIO_3_UART1_TX_CM4);

    /* COM port settings */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.parity = HAL_UART_PARITY_NONE;
    hal_uart_init(HAL_UART_0, &uart_config);
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
*@brief  Initialize the periperal driver in this function. In this example, we initialize UART drivers.
*@param None.
*@return None.
*/
static void prvSetupHardware(void)
{
    /* Peripherals initialization */
    log_uart_init();
}

/**
*@brief  Get the current time with the unit of millisecond.
*@param None.
*@return In this function we return current time with the unit of millisecond.
*/
static uint32_t get_current_millisecond(void)
{
    hal_gpt_status_t ret;
    uint32_t count = 0;
    uint32_t time, time_s, time_ms;
    ret = hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count);

    if (ret != HAL_GPT_STATUS_OK) {
        printf("\r\n---gpt get free run count error, return = %d---\r\n", ret);
        return 0;
    }

    time_s = count / 32768;
    time_ms = ((count % 32768) * 1000 + 16384) / 32768;
    time = time_s * 1000 + time_ms;

    return time;
}

/**
*@brief  Callback of GPT.
*@param[in] parameter: pointer to the data that user will use in this callback.
*@return None.
*/
static void gpt_callback(void *parameter)
{
    static uint32_t current_time = 0;
    static uint32_t last_time = 0;
    static uint32_t timeout_times = 0;
    uint32_t duration_time = 0;
    hal_gpt_status_t         ret;

    timeout_times ++;
    if (timeout_times < 10) {
        current_time = get_current_millisecond();
        duration_time = current_time - last_time;
        last_time = current_time;
        if (duration_time > 1050 || duration_time < 950) {
            hal_gpt_stop_timer(gpt_port);
            hal_gpt_deinit(gpt_port);
            printf("\r\n---gpt %dth test fail, Expired at %d ms---\r\n", (int)timeout_times, (int)get_current_millisecond());
            printf("\r\n---gpt_example finished!!!---\r\n");
            /* This is for hal_examples auto-test */
            printf("\r\nexample project test fail.\r\n");
            return;
        }
    } else {
        ret = hal_gpt_stop_timer(gpt_port);
        if (ret != HAL_GPT_STATUS_OK) {
            printf("\r\n---gpt stop timer fail, return = %d---\r\n", ret);
            return;
        }

        ret = hal_gpt_deinit(gpt_port);
        if (ret != HAL_GPT_STATUS_OK) {
            printf("\r\n---hal_gpt_deinit fail, return = %d---\r\n", ret);
            return;
        }
        printf("\r\n---gpt_example finished!!!---\r\n");
        /* This is for hal_examples auto-test */
        printf("\r\nexample project test success.\r\n");
    }

}

/**
*@brief  Set GPT timer type to repeat mode and start GPT.
*@param None.
*@return None.
*/
static void hal_gpt_interrupt_repeat_mode_example(void)
{
    hal_gpt_status_t         ret;
    uint32_t                 time, count;

    printf("\r\n---gpt_example begin---\r\n");
    gpt_port = HAL_GPT_0;
    time     = 1000;

    /*Step1: Initilizes GPT channel */
    ret = hal_gpt_init(gpt_port);
    if (ret != HAL_GPT_STATUS_OK) {
        printf("\r\n---gpt init error, return = %d---\r\n", ret);
        return;
    }

    /*Step2: Register callback */
    ret = hal_gpt_register_callback(gpt_port, gpt_callback, NULL);
    if (ret != HAL_GPT_STATUS_OK) {
        printf("\r\n---gpt registercallback error, return = %d---\r\n", ret);
        hal_gpt_deinit(gpt_port);
        return;
    }

    /*Step3: Set GPT timer type and start GPT */
    ret = hal_gpt_start_timer_ms(gpt_port,  time, HAL_GPT_TIMER_TYPE_REPEAT);
    if (ret != HAL_GPT_STATUS_OK) {
        printf("\r\n---gpt start timer ms error, return = %d---\r\n", ret);
        return;
    }
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count);
}

int main(void)
{
    /* Configure system clock */
    SystemClock_Config();


    /* Configure the hardware */
    prvSetupHardware();

    /* enable NVIC interrupts and fault exception*/
    __enable_irq();
    __enable_fault_irq();

    /* Add your application code here */
    printf("\r\n\r\n");

    /* The output UART used by printf is set by plain_log_uart_init() */
    printf("welcome to main()\r\n");
    printf("\r\n\r\n");

    hal_gpt_interrupt_repeat_mode_example();

    while (1);
}

