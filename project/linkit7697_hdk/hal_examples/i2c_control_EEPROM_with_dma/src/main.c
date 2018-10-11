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
#include "memory_attribute.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define I2C_EEPROM_SLAVE_ADDRESS (0xA0>>1)

/* Private variables ---------------------------------------------------------*/
volatile uint32_t test_length = 5; /* one single write should be a maximum of 16 bytes */
volatile uint32_t i2c_finish_flag = 0;
uint8_t i2c_send_data[32] = {0x00, 0x00, 0xFF, 0xAA, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
                             0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F
                            };
uint8_t i2c_receive_data[32] = {0};




/* Private functions ---------------------------------------------------------*/

#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    /* Place your implementation of fputc here */
    /* E.g. write a character to the HAL_UART_0 one at a time */
    hal_uart_put_char(HAL_UART_0, ch);
    return ch;
}

/**
*@brief Set pinmux to UART and initialize UART hardware initialization for logging.
*@param None.
*@return None.
*/
static void plain_log_uart_init(void)
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
    plain_log_uart_init();
}

/**
*@brief User defined callback. This function will be called after I2C send or receive data each time..
*@param None.
*@return None.
*/
static void i2c_callback(uint8_t slave_address, hal_i2c_callback_event_t event, void *user_data)
{
    printf("\r\nI2C callback function!\r\n");
    i2c_finish_flag = 1;
}

/**
*@brief  In this function, we send datum to EEPROM and read them back to verify the success of i2c communication with EEPROM.
*@param None.
*@return None.
*/
static void i2c_control_EEPROM_with_dma_example()
{

    hal_i2c_config_t i2c_init;
    uint32_t i;
    hal_i2c_frequency_t input_frequency = HAL_I2C_FREQUENCY_400K;
    hal_i2c_port_t i2c_port = HAL_I2C_MASTER_0;
    uint32_t test_fail = 0;

    printf("\r\nI2C test case begin\r\n");

    /*----------------------------------------------------------------------*/
    /*Step1: Init GPIO and set GPIO pinmux(if EPT tool hasn't been used to configure the related pinmux).*/
    hal_gpio_init(HAL_GPIO_27);
    hal_gpio_init(HAL_GPIO_28);
    /* Call hal_pinmux_set_function() to set GPIO pinmux */
    hal_pinmux_set_function(HAL_GPIO_27, HAL_GPIO_27_I2C1_CLK);
    hal_pinmux_set_function(HAL_GPIO_28, HAL_GPIO_28_I2C1_DATA);

    /*Step2: Write data to i2c slave.*/
    test_length = 5;
    i2c_init.frequency = input_frequency;
    if (HAL_I2C_STATUS_OK != hal_i2c_master_init(i2c_port, &i2c_init)) {
        printf("I2C initialize1 failed\r\n");
        return;
    }
    /* Register callback function */
    if (HAL_I2C_STATUS_OK != hal_i2c_master_register_callback(i2c_port, i2c_callback, NULL)) {
        printf("I2C register1 callback failed\r\n");
        return;
    }
    if (HAL_I2C_STATUS_OK != hal_i2c_master_send_dma(i2c_port, I2C_EEPROM_SLAVE_ADDRESS, i2c_send_data, test_length+1)) {
        printf("I2C send1 failed\r\n");
        return;
    }

    /* Deinitialize I2C */
    while (0 == i2c_finish_flag);
    i2c_finish_flag = 0;
    if (HAL_I2C_STATUS_OK != hal_i2c_master_deinit(i2c_port)) {
        printf("I2C deinitialize1 failed\r\n");
        return;
    }
    /*Wait some time till the data is stable*/
    hal_gpt_delay_ms(200);

    /*Step3: Write 1 byte data to slave to indicate the address user want to read.*/
    i2c_init.frequency = input_frequency;

    if (HAL_I2C_STATUS_OK != hal_i2c_master_init(i2c_port, &i2c_init)) {
        printf("I2C initialize2 failed\r\n");
        return;
    }
    /* Register callback function */
    if (HAL_I2C_STATUS_OK != hal_i2c_master_register_callback(i2c_port, i2c_callback, NULL)) {
        printf("I2C register2 callback failed\r\n");
        return;
    }
    /* Write 1 byte to slave to indicate the address you want to read */
    if (HAL_I2C_STATUS_OK != hal_i2c_master_send_dma(i2c_port, I2C_EEPROM_SLAVE_ADDRESS, i2c_send_data, 1)) {
        printf("I2C send2 failed\r\n");
        return;
    }
    /* Deinitialize I2C */
    while (0 == i2c_finish_flag);
    i2c_finish_flag = 0;
    if (HAL_I2C_STATUS_OK != hal_i2c_master_deinit(i2c_port)) {
        printf("I2C deinitialize2 failed\r\n");
        return;
    }

    /*Wait some time till the data is stable*/
    hal_gpt_delay_ms(200);
    /*----------------------------------------------------------------------*/

    /*Step4: Read data back from i2c slave.*/
    i2c_init.frequency = input_frequency;
    if (HAL_I2C_STATUS_OK != hal_i2c_master_init(i2c_port, &i2c_init)) {
        printf("I2C initialize3 failed\r\n");
        return;
    }
    /* Register callback function */
    if (HAL_I2C_STATUS_OK != hal_i2c_master_register_callback(i2c_port, i2c_callback, NULL)) {
        printf("I2C register3 callback failed\r\n");
        return;
    }
    /* Read data back to check if data transfer is successful. */

    if (HAL_I2C_STATUS_OK != hal_i2c_master_receive_dma(i2c_port, I2C_EEPROM_SLAVE_ADDRESS, i2c_receive_data, test_length)) {
        printf("I2C receive3 failed\r\n");
        return;
    }

    while (0 == i2c_finish_flag);
    i2c_finish_flag = 0;
    if (HAL_I2C_STATUS_OK != hal_i2c_master_deinit(i2c_port)) {
        printf("I2C deinitialize3 failed\r\n");
        return;
    }
    /*Step5: Verify the success of data transfer by comparing the write data and the data read back.*/
    /*----------------------------------------------------------------------*/
    printf("begin compare...\r\n");

    for (i = 0; i < (test_length); i++) {
        if (i2c_send_data[i + 1] != i2c_receive_data[i]) {
            test_fail = 1;
            printf("I2C test failed\r\n");
            break;
        }
    }
    if (0 == test_fail) {
        printf("I2C test is successful\r\n");
    }

    hal_gpio_deinit(HAL_GPIO_27);
    hal_gpio_deinit(HAL_GPIO_28);

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
    /* The output UART used by printf is set by plain_log_uart_init() */
    printf("welcome to main()\r\n");
    printf("pls add your own code from here\r\n");
    printf("\r\n\r\n");

    i2c_control_EEPROM_with_dma_example();

    while (1);
}

