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
#define GPC_INPUT_PIN                   HAL_GPIO_33,HAL_GPIO_33_PULSE_CNT
#define GPIO_PULSE_PIN                  HAL_GPIO_6

/* Private variables ---------------------------------------------------------*/

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
*@brief  Initialize the peripheral driver in this function. In this example, we initialize UART drivers.
*@param None.
*@return None.
*/
static void prvSetupHardware(void)
{
    /* Peripherals initialization */
    log_uart_init();
}

/**
*@brief  Initialize the hardware of GPC by setting pinmux.
*@param None.
*@return None.
*/
static void gpc_test_hardware_init(hal_gpc_edge_t edge)
{
    /* Set Pinmux to gpio mode to output pulse */
    hal_gpio_init(GPIO_PULSE_PIN);

    hal_pinmux_set_function(GPIO_PULSE_PIN, HAL_GPIO_6_GPIO6);

    hal_gpio_set_direction(GPIO_PULSE_PIN, HAL_GPIO_DIRECTION_OUTPUT);
    if (HAL_GPC_EDGE_RAISING == edge) {
        hal_gpio_set_output(GPIO_PULSE_PIN, HAL_GPIO_DATA_LOW);
    } else {
        hal_gpio_set_output(GPIO_PULSE_PIN, HAL_GPIO_DATA_HIGH);
    }

    /* Set Pinmux to pcnt mode */
    hal_pinmux_set_function(GPC_INPUT_PIN);
}


/**
*@brief  Generate the pulse by setting GPIO output.
*@param[in] count: this parameter determines the number of generated pulse.
*@return None.
*/
static void generate_pulse(uint32_t count)
{
    for (uint32_t index = 0; index < count; index++) {
        hal_gpio_set_output(GPIO_PULSE_PIN, HAL_GPIO_DATA_HIGH);
        hal_gpt_delay_ms(10);
        hal_gpio_set_output(GPIO_PULSE_PIN, HAL_GPIO_DATA_LOW);
        hal_gpt_delay_ms(10);
    }
}

/**
*@brief  Example of gpc. In this function we generate the pulse for GPC to capture.
*@param None.
*@return None.
*/
static void hal_gpc_example(void)
{
    uint32_t count_value;
    hal_gpc_status_t            ret;
    hal_gpc_config_t            gpc_config;

    printf("\r\n ---gpc_example begin---\r\n");

    gpc_config.edge = HAL_GPC_EDGE_RAISING;
    gpc_test_hardware_init(gpc_config.edge);


    /* Initialize the GPC */
    ret = hal_gpc_init(HAL_GPC_0, &gpc_config);
    if (ret != HAL_GPC_STATUS_OK) {
        printf("\r\n---gpc initialization failed---\r\n");
        return;
    }

    /* Enable GPC module */
    hal_gpc_enable(HAL_GPC_0);

    for (uint32_t index = 0; index < 100; index++) {

        /*Generate pulses for GPC to capture*/
        generate_pulse(index + 1);

        /*Get pulses count*/
        hal_gpc_get_and_clear_count(HAL_GPC_0, &count_value);
        if(index + 1 != count_value){
            printf("\r\n--- gpc %dth test fail,cout=%d---\r\n",(int)index, (int)count_value);
            hal_gpc_deinit(HAL_GPC_0);
            printf("\r\n ---gpc_example finished!!!---\r\n");

            /* This is for hal_examples auto-test */
            printf("\r\nexample project test fail.\r\n");
            return;
        }
    }
    /* Deinitialize the GPC */
    hal_gpc_deinit(HAL_GPC_0);

    printf("\r\n ---gpc_example finished!!!---\r\n");

    /* This is for hal_examples auto-test */
    printf("\r\nexample project test success.\r\n");

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

    /* Add your application code here */
    printf("\r\n\r\n");

    /* The output UART used by printf is set by log_uart_init() */
    printf("welcome to main()\r\n");
    printf("\r\n\r\n");

    hal_gpc_example();
    for (;;);
}


