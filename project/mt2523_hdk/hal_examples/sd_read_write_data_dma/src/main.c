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

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define BLOCK_NUMBER  (4)

/* Private variables ---------------------------------------------------------*/

const unsigned char HAL_MSDC_EINT = 19; /*this variable is defined in ept_eint_var.c*/
ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint32_t  sd_read_mutil_buffer[BLOCK_NUMBER * 512 / 4];
ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint32_t  sd_write_mutil_buffer[BLOCK_NUMBER * 512 / 4];


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
static void plain_log_uart_init(void)
{
    hal_uart_config_t uart_config;

    /* gpio config for uart2 */
    hal_gpio_init(HAL_GPIO_0);
    hal_gpio_init(HAL_GPIO_1);
    hal_pinmux_set_function(HAL_GPIO_0, HAL_GPIO_0_U2RXD);
    hal_pinmux_set_function(HAL_GPIO_1, HAL_GPIO_1_U2TXD);
    /* COM port settings */
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
*@brief  Initialize the peripheral driver in this function. In this example, we initialize UART, Flash, and NVIC drivers.
*@param None.
*@return None.
*/
static void prvSetupHardware(void)
{
    /* Peripherals initialization */
    plain_log_uart_init();
    hal_flash_init();
    hal_nvic_init();

}

/**
*@brief  In this example we verify the success of sd communication by comparing the write and read back data.
*@param None.
*@return None.
*/
static void sd_read_write_data_dma_example(void)
{
    hal_sd_config_t sd_cfg = {HAL_SD_BUS_WIDTH_4, 45000};
    uint32_t read_block_number = 1;
    uint32_t write_block_number = 1;
    uint32_t i;
    uint32_t retry = 0;
    printf("SD read write data dma example test begin!\r\n");

    /* Step1: Call hal_pinmux_set_function() to set GPIO pinmux, if EPT tool was not used to configure the related pinmux.*/
    hal_pinmux_set_function(HAL_GPIO_30, HAL_GPIO_30_MC0_CK);// MC0_CK
    hal_pinmux_set_function(HAL_GPIO_31, HAL_GPIO_31_MC0_CM0);// MC0_CM0
    hal_pinmux_set_function(HAL_GPIO_32, HAL_GPIO_32_MC0_DA0);// MC0_DA0
    hal_pinmux_set_function(HAL_GPIO_33, HAL_GPIO_33_MC0_DA1);// MC0_DA1
    hal_pinmux_set_function(HAL_GPIO_34, HAL_GPIO_34_MC0_DA2);// MC0_DA2
    hal_pinmux_set_function(HAL_GPIO_35, HAL_GPIO_35_MC0_DA3);// MC0_DA3

    /*Step2: Initialize SD and write buffer.*/
    /*init sd*/
    while (retry < 3) {
        if (HAL_SD_STATUS_OK != hal_sd_init(HAL_SD_PORT_0, &sd_cfg)) {
            retry++;
        } else {
            break;
        }
    }

    if (3 <= retry) {
        printf("SD init fail!\r\n");
        return;
    }

    /*init write buffer*/
    for (i = 0; i < 128; i++) {
        sd_write_mutil_buffer[i] = 0xA5A5A5A5;
    }

    /*Step3: Write and read with dma mode and compare the data to verify the success of communication.*/
    /*write  with dma mode*/
    retry = 0;
    while (retry < 3) {

        if (HAL_SD_STATUS_OK != hal_sd_write_blocks_dma_blocking(HAL_SD_PORT_0, sd_write_mutil_buffer, 0x50, write_block_number)) {
            if (HAL_SD_STATUS_OK == hal_sd_init(HAL_SD_PORT_0, &sd_cfg)) {
                retry++;
            } else {
                printf("SD write (dma mode) failed!\r\n");
                break;
            }
        } else {
            break;
        }
    }

    if (3 <= retry) {
        printf("SD write (dma mode) failed!\r\n");
    }

    /*read with dma mode*/
    retry = 0;
    while (retry < 3) {
        if (HAL_SD_STATUS_OK != hal_sd_read_blocks_dma_blocking(HAL_SD_PORT_0, sd_read_mutil_buffer, 0x50, read_block_number)) {
            if (HAL_SD_STATUS_OK == hal_sd_init(HAL_SD_PORT_0, &sd_cfg)) {
                retry++;
            } else {
                printf("SD read (dma mode) failed!\r\n");
                break;
            }
        } else {
            break;
        }
    }

    if (3 <= retry) {
        printf("SD read (dma mode) failed!\r\n");
    }

    /*compare data*/
    for (i = 0; i < 128; i++) {
        if (sd_read_mutil_buffer[i] != 0xA5A5A5A5) {
            printf("SD data comparison failed!\r\n");
            printf("\r\n");
            return;
        }
    }

    printf("SD read write data dma example test ok!");
    printf("\r\n");

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
    printf("\r\n\r\n");/* The output UART used by printf is set by plain_log_uart_init() */
    printf("welcome to main()\r\n");
    printf("\r\n\r\n");

    sd_read_write_data_dma_example();

    while (1);
}

