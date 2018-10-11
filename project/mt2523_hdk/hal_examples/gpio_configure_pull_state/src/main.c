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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the HAL_UART_2 one at a time */
    hal_uart_put_char(HAL_UART_2, ch);
    return ch;
}

/**
*@brief Configure and initialize UART hardware initialization for logging.
*@param None.
*@return None.
*/
static void log_uart_init(void)
{
    hal_uart_config_t uart_config;

    /* gpio config for uart2 */
    hal_gpio_init(HAL_GPIO_0);
    hal_gpio_init(HAL_GPIO_1);
    hal_pinmux_set_function(HAL_GPIO_0, HAL_GPIO_0_U2RXD);
    hal_pinmux_set_function(HAL_GPIO_1, HAL_GPIO_1_U2TXD);

    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.parity = HAL_UART_PARITY_NONE;

    hal_uart_init(HAL_UART_2, &uart_config);
}

/**
*@brief Configure and initialize the systerm clock. In this example, we invoke hal_clock_init to initialize clock driver and clock gates.
*@param None.
*@return None.
*/
static void SystemClock_Config(void)
{
    hal_clock_init();
}

/**
*@brief  Initialize the periperal driver in this function. In this example, we initialize UART, Flash, and NVIC drivers.
*@param None.
*@return None.
*/
static void prvSetupHardware(void)
{
    /* Peripherals initialization */
    log_uart_init();
    hal_flash_init();
    hal_nvic_init();

}

/**
*@brief  Configure the GPIO pull state to pull-up and to pull-down. Verify the pull states by inputs.
*@param None.
*@return None.
*/
static void gpio_configure_pull_state_example()
{

    hal_gpio_data_t data_pull_up;
    hal_gpio_data_t data_pull_down;
    hal_gpio_status_t ret;
    hal_pinmux_status_t ret_pinmux_status;

    printf("\r\n ---gpio_example begin---\r\n");

    ret = hal_gpio_init(HAL_GPIO_10);
    if (HAL_GPIO_STATUS_OK != ret) {
        printf("GPIO init failed\r\n");
        hal_gpio_deinit(HAL_GPIO_10);
        return;
    }

    /*Step1: Set pinmux as GPIO mode, configure GPIO 1 to input and pull up it. Afetr that we read the input data for further validation*/

    /* Set pin as GPIO mode.*/
    ret_pinmux_status = hal_pinmux_set_function(HAL_GPIO_10, HAL_GPIO_10_GPIO10);
    if (HAL_PINMUX_STATUS_OK != ret_pinmux_status) {
        printf("hal_pinmux_set_function failed\r\n");
        hal_gpio_deinit(HAL_GPIO_10);
        return;
    }

    /* Set GPIO as input.*/
    ret = hal_gpio_set_direction(HAL_GPIO_10, HAL_GPIO_DIRECTION_INPUT);
    if (HAL_GPIO_STATUS_OK != ret) {
        printf("hal_gpio_set_direction failed\r\n");
        hal_gpio_deinit(HAL_GPIO_10);
        return;
    }

    /* Configure the pull state to pull-up. */
    ret = hal_gpio_pull_up(HAL_GPIO_10);
    if (HAL_GPIO_STATUS_OK != ret) {
        printf("hal_gpio_pull_up failed\r\n");
        hal_gpio_deinit(HAL_GPIO_10);
        return;
    }

    /* Read the input data of the pin for further validation.*/
    ret = hal_gpio_get_input(HAL_GPIO_10, &data_pull_up);
    if (HAL_GPIO_STATUS_OK != ret) {
        printf("hal_gpio_get_input failed\r\n");
        hal_gpio_deinit(HAL_GPIO_10);
        return;
    }

    /*Step2: Change the pull state to pull-down and read the input data*/

    /* Configure the pull state to pull-down.*/
    ret = hal_gpio_pull_down(HAL_GPIO_10);
    if (HAL_GPIO_STATUS_OK != ret) {
        printf("hal_gpio_pull_down failed\r\n");
        hal_gpio_deinit(HAL_GPIO_10);
        return;
    }

    /* Read the input data of the pin for further validation.*/
    ret = hal_gpio_get_input(HAL_GPIO_10, &data_pull_down);
    if (HAL_GPIO_STATUS_OK != ret) {
        printf("hal_gpio_get_input failed\r\n");
        hal_gpio_deinit(HAL_GPIO_10);
        return;
    }

    /*Step3: Verify the success of pull state.*/

    if ((data_pull_down == HAL_GPIO_DATA_LOW) && (data_pull_up == HAL_GPIO_DATA_HIGH)) {
        printf("GPIO pull state configuration is successful\r\n");
    } else {
        printf("GPIO pull state configuration failed\r\n");
    }

    ret = hal_gpio_deinit(HAL_GPIO_10);;
    if (HAL_GPIO_STATUS_OK != ret) {
        printf("hal_gpio_deinit failed\r\n");
        return;
    }

    printf("\r\n ---gpio_example finished!!!---\r\n");

    /* This is for hal_examples auto-test */
    printf("example project test success.\n");
}

int main(void)
{
    /* Configure system clock */
    SystemClock_Config();

    SystemCoreClockUpdate();

    /* Configure the hardware */
    prvSetupHardware();

    /* Enable I,F bits */
    __enable_irq();
    __enable_fault_irq();

    /* Add your application code here */
    printf("\r\n\r\n");/* The output UART used by printf is set by log_uart_init() */
    printf("welcome to main()\r\n");
    printf("pls add your own code from here\r\n");
    printf("\r\n\r\n");

    gpio_configure_pull_state_example();

    while (1);
}

