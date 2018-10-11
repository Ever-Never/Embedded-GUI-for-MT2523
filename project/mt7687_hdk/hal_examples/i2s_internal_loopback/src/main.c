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
#define I2S_TX_BUFFER_LENGTH   32
#define I2S_RX_BUFFER_LENGTH   2048
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static uint32_t I2S_TxBuf[I2S_TX_BUFFER_LENGTH];
static uint32_t I2S_RxBuf[I2S_RX_BUFFER_LENGTH];
static uint32_t USER_Rx_Buf[I2S_TX_BUFFER_LENGTH];

static uint16_t audio_Tone2k_16kSR[32] = {
    0xffff, 0xcd1a, 0xb805, 0xcd1b, 0x0000, 0x32e6, 0x47fb, 0x32e5,
    0x0000, 0xcd1a, 0xb805, 0xcd1b, 0x0000, 0x32e5, 0x47fb, 0x32e5,
    0x0000, 0xcd1b, 0xb805, 0xcd1b, 0x0000, 0x32e5, 0x47fb, 0x32e5,
    0x0000, 0xcd1b, 0xb806, 0xcd1a, 0xffff, 0x32e5, 0x47f9, 0x32e6
};

/* Private functions ---------------------------------------------------------*/
static void SystemClock_Config(void);
static void prvSetupHardware(void);
extern void top_xtal_init(void);

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

/* UART hardware init for log output */
void log_uart_init(void)
{
    hal_uart_config_t uart_config;
    /* Set Pinmux to UART */
    hal_pinmux_set_function(HAL_GPIO_0, HAL_GPIO_0_UART1_RTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_1, HAL_GPIO_1_UART1_CTS_CM4);
    hal_pinmux_set_function(HAL_GPIO_2, HAL_GPIO_2_UART1_RX_CM4);
    hal_pinmux_set_function(HAL_GPIO_3, HAL_GPIO_3_UART1_TX_CM4);

    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.parity = HAL_UART_PARITY_NONE;
    hal_uart_init(HAL_UART_0, &uart_config);
}

static void SystemClock_Config(void)
{
    top_xtal_init();
}

static void prvSetupHardware(void)
{
    /* System HW initialization */

    /* Peripherals initialization */
    log_uart_init();

    /* Board HW initialization */
}


/* Example of i2s_configure */
static int8_t i2s_configure(void)
{
    hal_i2s_config_t i2s_config;
    hal_i2s_status_t result = HAL_I2S_STATUS_OK;

    /* Set I2S as internal loopback mode */
    result = hal_i2s_init(HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE);
    if (HAL_I2S_STATUS_OK != result) {
        printf("\r\n ---i2s:    hal_i2s_init failed---\r\n");
        return -1;
    }

    /* Configure I2S  */
    i2s_config.clock_mode = HAL_I2S_MASTER;
    i2s_config.rx_down_rate = HAL_I2S_RX_DOWN_RATE_DISABLE;
    i2s_config.tx_mode = HAL_I2S_TX_MONO_DUPLICATE_DISABLE;
    i2s_config.i2s_out.channel_number = HAL_I2S_MONO;

    i2s_config.i2s_out.sample_rate = HAL_I2S_SAMPLE_RATE_16K;
    i2s_config.i2s_in.sample_rate = HAL_I2S_SAMPLE_RATE_16K;
    i2s_config.i2s_in.msb_offset = 0;
    i2s_config.i2s_out.msb_offset = 0;
    i2s_config.i2s_in.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_DISABLE;
    i2s_config.i2s_out.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_DISABLE;
    i2s_config.i2s_in.lr_swap = HAL_I2S_LR_SWAP_DISABLE;
    i2s_config.i2s_out.lr_swap = HAL_I2S_LR_SWAP_DISABLE;

    result = hal_i2s_set_config(&i2s_config);
    if (HAL_I2S_STATUS_OK != result) {
        printf("\r\n ---i2s:    hal_i2s_set_config failed---\r\n");
        return -1;
    }

    result = hal_i2s_setup_tx_vfifo(I2S_TxBuf, I2S_TX_BUFFER_LENGTH / 2, I2S_TX_BUFFER_LENGTH);
    if (HAL_I2S_STATUS_OK != result) {
        printf("\r\n ---i2s:    hal_i2s_setup_tx_vfifo failed---\r\n");
        return -1;
    }

    result = hal_i2s_setup_rx_vfifo(I2S_RxBuf, I2S_RX_BUFFER_LENGTH / 2, I2S_RX_BUFFER_LENGTH);
    if (HAL_I2S_STATUS_OK != result) {
        printf("\r\n ---i2s:    hal_i2s_setup_rx_vfifo failed---\r\n");
        return -1;
    }

    return 1;
}


/* Example of i2s_open */
static void i2s_open(void)
{
    hal_i2s_enable_tx();
    hal_i2s_enable_rx();
    hal_i2s_enable_audio_top();
}


/* Example of i2s_write
*  hal_i2s_status_t hal_i2s_tx_write(uint32_t data);
*  The data format for I2S TX is as shown below:
*	          Byte 3      Byte 2    Byte 1    Byte 0
*  Stereo    R[15:8]    R[7:0]    L[15:8]    L[7:0]
*  Mono      8'b0         8'b0       L[15:8]    L[7:0]
*/
static void i2s_write(void)
{
    uint32_t write_sample_count = 0;
    uint32_t i = 0;
    hal_i2s_get_tx_sample_count(&write_sample_count);
    if (write_sample_count >= I2S_TX_BUFFER_LENGTH) {
        for (i = 0; i < I2S_TX_BUFFER_LENGTH; i++) {
            hal_i2s_tx_write(audio_Tone2k_16kSR[i]);
        }
    }

}


/* Example of i2s_read */
static void i2s_read(void)
{
    uint32_t read_sample_count = 0;
    uint32_t rx_data_temp = 0;
    uint32_t i = 0;

    while (1) {
        hal_i2s_get_rx_sample_count(&read_sample_count);
        if (read_sample_count > 1024) {
            break;
        }
    }

    hal_i2s_get_rx_sample_count(&read_sample_count);
    while (read_sample_count != 0) {
        hal_i2s_rx_read(&rx_data_temp);
        if (rx_data_temp != 0) {
            USER_Rx_Buf[i] = rx_data_temp;
            i++;
            break;
        }
        hal_i2s_get_rx_sample_count(&read_sample_count);
    }

    hal_i2s_get_rx_sample_count(&read_sample_count);
    while (read_sample_count != 0) {
        hal_i2s_rx_read(&rx_data_temp);
        if (i < (I2S_TX_BUFFER_LENGTH)) {
            USER_Rx_Buf[i] = rx_data_temp;
            i++;
        } else {
            break;
        }
        hal_i2s_get_rx_sample_count(&read_sample_count);
    }

}


/* Example of i2s_close */
static void i2s_close(void)
{

    hal_i2s_disable_tx();
    hal_i2s_disable_rx();
    hal_i2s_disable_audio_top();
    hal_i2s_deinit();
    hal_i2s_stop_tx_vfifo();
    hal_i2s_stop_rx_vfifo();
}


static void i2s_dump(void)
{
    uint32_t i = 0;
    for (i = 0; i < I2S_TX_BUFFER_LENGTH; i++) {
        printf("I2S_TxBuf[%u]=0x%08x\r\n", (int)i, (unsigned int)I2S_TxBuf[i]);
    }

    //for(i=0;i<I2S_RX_BUFFER_LENGTH;i++)
    //    printf("I2S_RxBuf[%d]=%08x\r\n", i, I2S_RxBuf[i]);

    for (i = 0; i < I2S_TX_BUFFER_LENGTH; i++) {
        printf("USER_Rx_Buf[%u]=0x%08x\r\n", (int)i, (unsigned int)USER_Rx_Buf[i]);
    }

}


void i2s_write_data_example(void)
{
    int8_t result = 0;

    printf("\r\n ---i2s_example begin---\r\n");

    result = i2s_configure();
    if (result == -1) {
        printf("\r\n ---i2s:    i2s_configure failed---\r\n");
    }

    i2s_write();
    i2s_open();
    i2s_read();
    //hal_gpt_delay_ms(100);
    i2s_close();
    i2s_dump();


    printf("\r\n ---i2s_example finished!!!---\r\n");
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
    printf("\r\n\r\n");/* The output UART used by printf is set by log_uart_init() */
    printf("welcome to main()\r\n");
    printf("\r\n\r\n");

    i2s_write_data_example();

    for (;;);
}


/*-----------------------------------------------------------*/


