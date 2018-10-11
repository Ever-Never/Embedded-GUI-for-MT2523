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
#define SPIM_TEST_FREQUENCY     6000000
#define SPIM_TEST_PORT          HAL_SPI_MASTER_3
#define SPI_TEST_MASTER_SLAVE   HAL_SPI_MASTER_SLAVE_0
#define SPIM_PIN_NUMBER_CS      HAL_GPIO_25
#define SPIM_PIN_FUNC_CS        HAL_GPIO_25_MA_SPI3_A_CS
#define SPIM_PIN_NUMBER_CLK     HAL_GPIO_26
#define SPIM_PIN_FUNC_CLK       HAL_GPIO_26_MA_SPI3_A_SCK
#define SPIM_PIN_NUMBER_MOSI    HAL_GPIO_27
#define SPIM_PIN_FUNC_MOSI      HAL_GPIO_27_MA_SPI3_A_MOSI
#define SPIM_PIN_NUMBER_MISO    HAL_GPIO_28
#define SPIM_PIN_FUNC_MISO      HAL_GPIO_28_MA_SPI3_A_MISO
#define SPIS_CFG_RD_CMD         0x02
#define SPIS_RD_CMD             0x81
#define SPIS_CFG_WR_CMD         0x04
#define SPIS_WR_CMD             0x06
#define SPIS_RS_CMD             0x0a
#define SPIS_PWON_CMD           0x0e
#define SPIS_PWOFF_CMD          0x0c
#define SPIS_ADDRESS_ID         0x55aa0000
#define SPI_TEST_DATA_SIZE      (1024)
#define SPI_TEST_DATA_RANGE     115
#define SPIS_CFG_LENGTH         (SPI_TEST_DATA_SIZE - 1)
#define COMMAND_DELAY           0

/* Private variables ---------------------------------------------------------*/
static volatile bool g_spi_transaction_finish;
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_transfer_tx_big_buffer[SPI_TEST_DATA_SIZE + 1];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_transfer_rx_big_buffer[SPI_TEST_DATA_SIZE + 1];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_transfer_small_buffer[64];

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
*@brief Configure and initialize the systerm clock.
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
    plain_log_uart_init();
    hal_flash_init();
    hal_nvic_init();
}

/**
*@brief     In this function we notice spi driver owner that spi transfer has been completed.
*@param[in] event: event of spi driver.
*@param[in] user_data: pointer to the data that registered to spi driver.
*@return    None.
*/
static void spi_master_dma_callback(hal_spi_master_callback_event_t event, void *user_data)
{
    printf("  spi_master_dma_callback() enter, event = %d\r\n", event);
    g_spi_transaction_finish = true;
}


/**
*@brief     In this function we query status of spi slaver.
*@param[in] status: Pointer to the result of spi slaver's status.
*@return    None.
*/
static void spi_query_slave_status(uint8_t *status)
{
    uint8_t status_cmd = SPIS_RS_CMD;
    uint8_t status_receive[2];
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    /* Note:
     * The value of receive_length is the valid number of bytes received plus the number of bytes to send.
     * For example, here the valid number of bytes received is 1 byte,
     * and the number of bytes to send also is 1 byte, so the receive_length is 2.
     */
    status_receive[1] = 0;
    spi_send_and_receive_config.receive_length = 2;
    spi_send_and_receive_config.send_length = 1;
    spi_send_and_receive_config.send_data = &status_cmd;
    spi_send_and_receive_config.receive_buffer = status_receive;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(SPIM_TEST_PORT, &spi_send_and_receive_config)) {
        printf("SPI master query status of slaver failed\r\n");
        return;
    }

    printf("  Status receive: 0x%x\r\n", status_receive[1]);
    *status = status_receive[1];
}

/**
*@brief  Example of spi send datas. In this function, SPI master send various commonds to slave.
*@param  None.
*@return None.
*/
static void spi_master_transfer_data_two_boards_example(void)
{
    uint32_t i = 0;
    uint8_t status_receive;
    uint8_t poweron_cmd;
    uint8_t poweroff_cmd;
    uint8_t cfg_wr_cmd[9];
    uint8_t cfg_rd_cmd[9];
    hal_spi_master_config_t spi_config;
    hal_spi_master_advanced_config_t advanced_config;
    hal_spi_master_send_and_receive_config_t send_and_receive_config;

    printf("---spi_master_transfer_data_two_boards_example begins---\r\n\r\n");

    printf("Example will begin to run after 10 secound....\r\n\r\n");
    hal_gpt_delay_ms(10000);

    /* Step1: GPIO configuration for SPI master */
    printf("SPI Master GPIOs config\r\n");
    hal_gpio_init(SPIM_PIN_NUMBER_CS);
    hal_gpio_init(SPIM_PIN_NUMBER_CLK);
    hal_gpio_init(SPIM_PIN_NUMBER_MOSI);
    hal_gpio_init(SPIM_PIN_NUMBER_MISO);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_CS, SPIM_PIN_FUNC_CS);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_CLK, SPIM_PIN_FUNC_CLK);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_MOSI, SPIM_PIN_FUNC_MOSI);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_MISO, SPIM_PIN_FUNC_MISO);

    /* Step2: Initializes SPI master with normal mode */
    printf("SPI Master port init\r\n");
    spi_config.bit_order = HAL_SPI_MASTER_LSB_FIRST;
    spi_config.slave_port = SPI_TEST_MASTER_SLAVE;
    spi_config.clock_frequency = SPIM_TEST_FREQUENCY;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_init(SPIM_TEST_PORT, &spi_config)) {
        printf("SPI master init failed\r\n");
        return;
    }
    advanced_config.byte_order = HAL_SPI_MASTER_LITTLE_ENDIAN;
    advanced_config.chip_polarity = HAL_SPI_MASTER_CHIP_SELECT_LOW;
    advanced_config.get_tick = HAL_SPI_MASTER_GET_TICK_DELAY1;
    advanced_config.sample_select = HAL_SPI_MASTER_SAMPLE_POSITIVE;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_set_advanced_config(SPIM_TEST_PORT, &advanced_config)) {
        printf("SPI master set advanced config failed\r\n");
        return;
    }
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_register_callback(SPIM_TEST_PORT, spi_master_dma_callback, NULL)) {
        printf("SPI master register callback failed\r\n");
        return;
    }


    /* Step3: SPI Master starts to send power_on command to SPI slave */
    printf("SPI Master starts to send PWON command to SPI slave\r\n");
    poweron_cmd = SPIS_PWON_CMD;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(SPIM_TEST_PORT, &poweron_cmd, 1)) {
        printf("  SPI master sends PWON command fail\r\n");
        return;
    }
    do {
        spi_query_slave_status(&status_receive);
    } while ((status_receive & (uint8_t)SPISLV_STATUS_SLV_ON_MASK) != (uint8_t)SPISLV_STATUS_SLV_ON_MASK);

    /* Step4: SPI Master starts to send config write command to SPI slave */
    printf("SPI Master starts to send CW command to SPI slave\r\n");
    cfg_wr_cmd[0] = SPIS_CFG_WR_CMD;
    cfg_wr_cmd[1] = SPIS_ADDRESS_ID & 0xff;
    cfg_wr_cmd[2] = (SPIS_ADDRESS_ID >> 8) & 0xff;
    cfg_wr_cmd[3] = (SPIS_ADDRESS_ID >> 16) & 0xff;
    cfg_wr_cmd[4] = (SPIS_ADDRESS_ID >> 24) & 0xff;
    cfg_wr_cmd[5] = SPIS_CFG_LENGTH & 0xff;
    cfg_wr_cmd[6] = (SPIS_CFG_LENGTH >> 8) & 0xff;
    cfg_wr_cmd[7] = (SPIS_CFG_LENGTH >> 16) & 0xff;
    cfg_wr_cmd[8] = (SPIS_CFG_LENGTH >> 24) & 0xff;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(SPIM_TEST_PORT, cfg_wr_cmd, 9)) {
        printf("  SPI master send CW command failed\r\n");
        return;
    }
    do {
        spi_query_slave_status(&status_receive);
    } while ((status_receive & (uint8_t)SPISLV_STATUS_TXRX_FIFO_RDY_MASK) != (uint8_t)SPISLV_STATUS_TXRX_FIFO_RDY_MASK);

    /* Step5: Config SPI master to use single mode for write command */
    printf("SPI Master starts to send WR command to SPI slave\r\n");
    g_spi_transaction_finish = false;
    g_transfer_tx_big_buffer[0] = SPIS_WR_CMD;
    for (i = 1; i < (SPI_TEST_DATA_SIZE + 1); i++) {
        g_transfer_tx_big_buffer[i] = (i % SPI_TEST_DATA_RANGE);
    }
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_dma(SPIM_TEST_PORT, g_transfer_tx_big_buffer, (SPI_TEST_DATA_SIZE + 1))) {
        printf("  SPI master send WR command failed\r\n");
    }
    while (g_spi_transaction_finish == false);
    do {
        spi_query_slave_status(&status_receive);
    } while ((status_receive & (uint8_t)SPISLV_STATUS_RDWR_FINISH_MASK) != (uint8_t)SPISLV_STATUS_RDWR_FINISH_MASK);

    /* Step6: reinit buffer to zero */
    printf("Re-initialize the transfer buffer to zero for later receive\r\n");
    for (i = 0; i < (SPI_TEST_DATA_SIZE + 1); i++) {
        g_transfer_rx_big_buffer[i] = 0x00;
    }

    /* delay some time to wait SPI slave to finish the check of data */
    hal_gpt_delay_ms(500);

    /* Step7: SPI Master starts to send cfg_rd command to SPI slave */
    printf("SPI master starts to send CR command to SPI slave\r\n");
    cfg_rd_cmd[0] = SPIS_CFG_RD_CMD;
    cfg_rd_cmd[1] = SPIS_ADDRESS_ID & 0xff;
    cfg_rd_cmd[2] = (SPIS_ADDRESS_ID >> 8) & 0xff;
    cfg_rd_cmd[3] = (SPIS_ADDRESS_ID >> 16) & 0xff;
    cfg_rd_cmd[4] = (SPIS_ADDRESS_ID >> 24) & 0xff;
    cfg_rd_cmd[5] = SPIS_CFG_LENGTH & 0xff;
    cfg_rd_cmd[6] = (SPIS_CFG_LENGTH >> 8) & 0xff;
    cfg_rd_cmd[7] = (SPIS_CFG_LENGTH >> 16) & 0xff;
    cfg_rd_cmd[8] = (SPIS_CFG_LENGTH >> 24) & 0xff;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(SPIM_TEST_PORT, cfg_rd_cmd, 9)) {
        printf("  SPI master send CR command failed\r\n");
        return;
    }
    do {
        spi_query_slave_status(&status_receive);
    } while ((status_receive & (uint8_t)SPISLV_STATUS_TXRX_FIFO_RDY_MASK) != (uint8_t)SPISLV_STATUS_TXRX_FIFO_RDY_MASK);

    hal_gpt_delay_ms(COMMAND_DELAY);

    /* Step8: Config SPI master to use single mode for rd command */
    printf("SPI master starts to send RD command to SPI slave\r\n");
    g_spi_transaction_finish = false;
    g_transfer_small_buffer[0] = SPIS_RD_CMD;
    send_and_receive_config.send_data = g_transfer_small_buffer;
    send_and_receive_config.send_length = 1;
    send_and_receive_config.receive_buffer = g_transfer_rx_big_buffer;
    send_and_receive_config.receive_length = SPI_TEST_DATA_SIZE + send_and_receive_config.send_length;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_dma(SPIM_TEST_PORT, &send_and_receive_config)) {
        printf("  SPI master send RD command failed\r\n");
    }
    while (g_spi_transaction_finish == false);
    printf("SPI master starts to send RS command to SPI slave\r\n");
    do {
        spi_query_slave_status(&status_receive);
    } while ((status_receive & (uint8_t)SPISLV_STATUS_RDWR_FINISH_MASK) != (uint8_t)SPISLV_STATUS_RDWR_FINISH_MASK);

    /* Step9: SPI Master check whether the data received match the data sent before. */
    printf("Start to check data integrity\r\n");
    for (i = 1; i < (SPI_TEST_DATA_SIZE + 1); i++) {
        if (g_transfer_rx_big_buffer[i] != (i % SPI_TEST_DATA_RANGE)) {
            printf("  Data check fail\r\n");
            return;
        }
    }
    printf("  Data check pass\r\n");

    hal_gpt_delay_ms(COMMAND_DELAY);

    /* Step10: SPI Master starts to send PWOFF command to SPI slave */
    printf("SPI master starts to send PWOFF command to SPI slave\r\n");
    poweroff_cmd = SPIS_PWOFF_CMD;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(SPIM_TEST_PORT, &poweroff_cmd, 1)) {
        printf("  SPI master send PWOFF command failed\r\n");
        return;
    }
    do {
        spi_query_slave_status(&status_receive);
    } while ((status_receive & (uint8_t)SPISLV_STATUS_SLV_ON_MASK) == (uint8_t)SPISLV_STATUS_SLV_ON_MASK);

    /* Step11: De-initialize SPI master and the GPIO. */
    printf("SPI master deinit\r\n\r\n");
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_deinit(SPIM_TEST_PORT)) {
        printf("  SPI master deinit failed\r\n");
        return;
    }
    hal_gpio_deinit(SPIM_PIN_NUMBER_CS);
    hal_gpio_deinit(SPIM_PIN_NUMBER_CLK);
    hal_gpio_deinit(SPIM_PIN_NUMBER_MOSI);
    hal_gpio_deinit(SPIM_PIN_NUMBER_MISO);

    printf("---spi_master_transfer_data_two_boards_example ends---\r\n");
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
    printf("\r\n\r\n");
    /* The output UART used by printf is set by plain_log_uart_init() */
    printf("welcome to main()\r\n");
    printf("\r\n\r\n");

    /* Add your application code here. */
    spi_master_transfer_data_two_boards_example();

    while (1);
}

