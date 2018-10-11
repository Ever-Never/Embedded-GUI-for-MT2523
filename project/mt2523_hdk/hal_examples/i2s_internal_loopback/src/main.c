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
#include "hal_dsp_topsm.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define I2S_TEST_RX_DUMP_SIZE        256     /* in word */
#define I2S_TEST_TX_BUFFER_LENGTH    1024    /* in byte */
#define I2S_TEST_RX_BUFFER_LENGTH    1024    /* in byte */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static uint8_t     int_buffer[8192];               /* For driver internal-use, dynamic allocation is recommended. */
static uint8_t     user_tx_buffer[I2S_TEST_TX_BUFFER_LENGTH];
static uint8_t     user_rx_buffer[I2S_TEST_RX_BUFFER_LENGTH];
static uint16_t    dump_buffer[I2S_TEST_RX_DUMP_SIZE];             /* Buffer to record data from RX link */
static uint16_t    dump_sample_count = 0, tone_buffer_index = 0;
static uint32_t    rx_count = 0;                                   /* The count of RX interrupt which is used to record data. */
static bool        isr_tx_error = false, isr_rx_error = false;     /* The count of errors occurred in user callback functions. */
static volatile hal_i2s_event_t tx_event = HAL_I2S_EVENT_NONE;
static volatile hal_i2s_event_t rx_event = HAL_I2S_EVENT_NONE;

static const uint16_t i2s_tone2k_32ksr[32] = {                    /* 2k sine wave under 32kHz sample rate */
    0xFFF4, 0x30F0, 0x5A79, 0x763C, 0x7FFF, 0x7646, 0x5A8B, 0x3107,
    0x000C, 0xCF10, 0xA587, 0x89C4, 0x8001, 0x89BB, 0xA576, 0xCEF9,
    0xFFF3, 0x30F0, 0x5A79, 0x763C, 0x7FFF, 0x7645, 0x5A8A, 0x3106,
    0x000C, 0xCF10, 0xA587, 0x89C4, 0x8001, 0x89BB, 0xA576, 0xCEF9
};
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
static void prvSetupHardware( void )
{
    /* System HW initialization */

    /* Peripherals initialization */
    plain_log_uart_init();
    hal_flash_init();
    hal_nvic_init();

    /* Board HW initialization */
}
/**
*@brief  In this function, we fill 2K tone audio samples into the buffer with the unit of byte.
*@param[in] size: indicats the size of audio samples.
*@param[in] buffer: pointer to the buffer that will be used in this function.
*@return None.
*/
static void i2s_test_fill_tx_buffer(uint8_t *buffer, uint32_t size)
{
    uint32_t i;
    uint16_t *buffer_word = (uint16_t *)buffer;
    for (i = 0 ; i < (size >> 1) ; i++) {
        *buffer_word++ = i2s_tone2k_32ksr[tone_buffer_index];
        tone_buffer_index++;
        if (tone_buffer_index == 32) {
            tone_buffer_index = 0;
        }
    }
}
/**
*@brief  Log out the received audio data.
*@param None.
*@return None.
*/
static void i2s_test_dump_buffer(void)
{
    uint32_t i;
    printf("[I2S]Dump received buffer via internal loopback...\r\n");
    for (i = 0; i < I2S_TEST_RX_DUMP_SIZE; i++) {
        printf("0x%04x\t", dump_buffer[i]);
        if ((i + 1) % 8 == 0) {
            printf("\r\n");
        }
    }
}
/**
*@brief  Callback function of i2s_tx. This function is registered to handle data transfer in I2S module.
*@param[in] user_data: pointer to the data that user will use in this callback.
*@param[in] event: indicates i2s event such as underflow or request user to fill data.
*@return None.
*/
static void user_i2s_tx_callback(hal_i2s_event_t event, void *user_data)
{
    hal_i2s_status_t tx_status = HAL_I2S_STATUS_OK;
    uint32_t sample_count = 0;

    tx_event = event;
    if ((tx_event == HAL_I2S_EVENT_DATA_REQUEST) || (tx_event == HAL_I2S_EVENT_UNDERFLOW)) {
        tx_event = HAL_I2S_EVENT_NONE;
        hal_i2s_get_tx_sample_count(&sample_count);

        if (sample_count > 0) {
            if (sample_count >= I2S_TEST_TX_BUFFER_LENGTH) {
                sample_count = I2S_TEST_TX_BUFFER_LENGTH;
            }
            i2s_test_fill_tx_buffer(user_tx_buffer, sample_count);
            tx_status = hal_i2s_tx_write(user_tx_buffer, sample_count);
            if (tx_status != HAL_I2S_STATUS_OK) {
                isr_tx_error = true;
            }
        }
    }
}
/**
*@brief  Callback function of i2s_rx. This function is registered to handle data transfer in I2S module.
*@param[in] user_data: pointer to the data that user will use in this callback.
*@param[in] event: indicates i2s event such as underflow or notify user the RX data is ready.
*@return None.
*/
static void user_i2s_rx_callback(hal_i2s_event_t event, void *user_data)
{
    hal_i2s_status_t rx_status = HAL_I2S_STATUS_OK;
    uint32_t sample_count = 0;
    uint32_t i;

    rx_event = event;
    if ((rx_event == HAL_I2S_EVENT_DATA_NOTIFICATION) || (rx_event == HAL_I2S_EVENT_OVERFLOW)) {
        rx_event = HAL_I2S_EVENT_NONE;
        hal_i2s_get_rx_sample_count(&sample_count);
        if (sample_count > 0) {
            if (sample_count >= I2S_TEST_RX_BUFFER_LENGTH) {
                sample_count = I2S_TEST_RX_BUFFER_LENGTH;
            }
            rx_status = hal_i2s_rx_read(user_rx_buffer, sample_count);
            if (rx_status != HAL_I2S_STATUS_OK) {
                isr_rx_error = true;
            }
        }
        /* Record samples after several times of transmission to ensure getting non-zero(valid) data. */
        if ((rx_count >= 10) && (dump_sample_count < I2S_TEST_RX_DUMP_SIZE)) {
            for (i = 0; i < sample_count; i += 2) {
                dump_buffer[dump_sample_count++] = (user_rx_buffer[i + 1] << 8) + user_rx_buffer[i];
                if (dump_sample_count == I2S_TEST_RX_DUMP_SIZE) {
                    break;
                }
            }
        }
        rx_count ++;
    }
}

/**
*@brief  Open flow of I2S. In this function, we set I2S into an internal loopback mode, and enable both the I2S TX and RX links to transmit/receive data.
*@param None.
*@return In this example we return the i2s open status OK or not.
*/
static int32_t i2s_open(void)
{
    hal_i2s_config_t i2s_config;
    hal_i2s_status_t result = HAL_I2S_STATUS_OK;
    uint8_t  *memory;

    result = hal_i2s_init(HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE);
    if (result != HAL_I2S_STATUS_OK) {
        return -1;
    }

    /*I2S configuration: Master mode, channel(in/out)=(Stereo/Stereo), sample rate(in/out)=(32KHz/32kHz). */
    i2s_config.clock_mode = HAL_I2S_MASTER;
    i2s_config.i2s_in.channel_number = HAL_I2S_STEREO;
    i2s_config.i2s_out.channel_number = HAL_I2S_STEREO;
    i2s_config.i2s_in.sample_rate = HAL_I2S_SAMPLE_RATE_32K;
    i2s_config.i2s_out.sample_rate = HAL_I2S_SAMPLE_RATE_32K;

    result = hal_i2s_set_config(&i2s_config);
    if (result != HAL_I2S_STATUS_OK) {
        return -2;
    }

    /*
    A global array of the size of 8192 bytes is declared as a memory pool for the I2S driver.
    It is recommended to allocate memory dynamically if there is OS support.
    */
    memory = &int_buffer[0];
    result = hal_i2s_set_memory(memory);
    if (result != HAL_I2S_STATUS_OK) {
        return -3;
    }

    i2s_test_fill_tx_buffer(user_tx_buffer, I2S_TEST_TX_BUFFER_LENGTH);
    hal_i2s_register_tx_callback(user_i2s_tx_callback, NULL);
    hal_i2s_register_rx_callback(user_i2s_rx_callback, NULL);

    hal_i2s_enable_tx();
    hal_i2s_enable_rx();
    return 1;
}

/**
*@brief  Close flow of I2S. The function will disable both links of I2S module.
*@param None.
*@return In this example we return the status of i2s close succeed or not.
*/
static int32_t i2s_close(void)
{
    hal_i2s_disable_rx();
    hal_i2s_disable_tx();

    hal_i2s_deinit();
    if (isr_tx_error || isr_rx_error) {
        return -4;
    }
    return 1;

}
/**
*@brief  Example of i2s_internal_loopback. In this function we open and close i2s respectively, then we test the dump buffer of i2s.
*@param None.
*@return None.
*/
static void i2s_internal_loopback_example(void)
{
    int example_result;

    printf("\r\n[I2S] Hal example project: Start!\r\n");

    example_result = i2s_open();
    if (example_result < 0) {
        printf("[I2S] Error(%d): I2S open failed!\r\n", example_result);
        return;
    }
    hal_gpt_delay_ms(3000);

    example_result = i2s_close();
    if (example_result < 0) {
        printf("[I2S] Error(%d): I2S close failed!\r\n", example_result);
        return;
    }
    i2s_test_dump_buffer();
    printf("\r\n[I2S] Hal example project: Done!\r\n");
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
    printf("\r\n\r\n");

    /* Initialize audio hardware */
    dsp_topsm_init();
    hal_audio_init();

    i2s_internal_loopback_example();

    while (1);
}

