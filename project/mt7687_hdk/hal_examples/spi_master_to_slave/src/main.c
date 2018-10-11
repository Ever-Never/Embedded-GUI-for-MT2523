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
#define SPI_TEST_FREQUENCY             1000000
#define SPI_TEST_MASTER                HAL_SPI_MASTER_0
#define SPI_TEST_MASTER_SLAVE          HAL_SPI_MASTER_SLAVE_0
#define SPIM_PIN_NUMBER_CS             HAL_GPIO_32
#define SPIM_PIN_FUNC_CS               HAL_GPIO_32_SPI_CS_0_M_CM4
#define SPIM_PIN_NUMBER_CLK            HAL_GPIO_31
#define SPIM_PIN_FUNC_CLK              HAL_GPIO_31_SPI_SCK_M_CM4
#define SPIM_PIN_NUMBER_MOSI           HAL_GPIO_30
#define SPIM_PIN_FUNC_MOSI             HAL_GPIO_30_SPI_MISO_M_CM4
#define SPIM_PIN_NUMBER_MISO           HAL_GPIO_29
#define SPIM_PIN_FUNC_MISO             HAL_GPIO_29_SPI_MOSI_M_CM4

#define SPI_TEST_DATA_SIZE             (20 * 1024)
#define SPI_TEST_TX_DATA_PATTERN       0x01234567
#define SPI_TEST_RX_DATA_PATTERN       0x89abcdef
#define SPI_TEST_SLV_BUFFER_ADDRESS    0x20000200  /* SPI slave fix this buffer address according to linker script. */
#define SPI_TEST_CONTROL_SIZE          4
#define SPI_TEST_STATUS_SIZE           4
#define SPI_TEST_READ_DATA_BEGIN       0x0A
#define SPI_TEST_READ_DATA_END         0xA0
#define SPI_TEST_WRITE_DATA_BEGIN      0x55
#define SPI_TEST_WRITE_DATA_END        0xAA
#define SPI_TEST_ACK_FLAG              0x5A
#define SPI_SLAVER_REG_READ_LOW_DATA   0x00
#define SPI_SLAVER_REG_READ_HIGH_DATA  0x02
#define SPI_SLAVER_REG_WRITE_LOW_DATA  0x04
#define SPI_SLAVER_REG_WRITE_HIGH_DATA 0x06
#define SPI_SLAVER_REG_LOW_ADDRESS     0x08
#define SPI_SLAVER_REG_HIGH_ADDRESS    0x0A
#define SPI_SLAVER_REG_CONFIG          0x0C
#define SPI_SLAVER_REG_STATUS          0x10
#define SPI_SLAVER_REG_SLV_IRQ         0x14
#define SPI_SLAVER_REG_RCV_MAILBOX     0x18
#define SPI_SLAVER_CONFIG_BUS_SIZE     (0x02 << 1)  /* 4 word */
#define SPI_SLAVER_CONFIG_BUS_W        0x01         /* write */
#define SPI_SLAVER_CONFIG_BUS_R        0x00         /* read */
#define SPI_SLAVER_CONFIG_READ         0x00
#define SPI_SLAVER_CONFIG_WRITE        0x80

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
*@brief Configure and initialize UART hardware initialization for logging.
*@param None.
*@return None.
*/
static void plain_log_uart_init(void)
{
    hal_uart_config_t uart_config;

    /* gpio config for uart0 */
    hal_gpio_init(HAL_GPIO_2);
    hal_gpio_init(HAL_GPIO_3);
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
*@brief  Initialize the periperal driver in this function. In this example, we initialize UART, Flash, and NVIC drivers.
*@param None.
*@return None.
*/
static void prvSetupHardware(void)
{
    /* Peripherals initialization */
    plain_log_uart_init();
}

/**
*@brief SPI master query SPI slave's idle status until it's idle.
*@param None.
*@return None.
*/
static void spim_wait_slave_idle(void)
{
    uint8_t command_buffer[4];
    uint32_t temp_buffer;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    do {
        command_buffer[0] = SPI_SLAVER_CONFIG_READ;
        command_buffer[1] = SPI_SLAVER_REG_STATUS;
        spi_send_and_receive_config.send_data = command_buffer;
        spi_send_and_receive_config.send_length = 2;
        spi_send_and_receive_config.receive_buffer = (uint8_t *)&temp_buffer;
        spi_send_and_receive_config.receive_length = 4;
        hal_spi_master_send_and_receive_polling(SPI_TEST_MASTER, &spi_send_and_receive_config);
    } while (temp_buffer & 0x01000000);
}

/**
*@brief SPI master issue address to SPI slave.
*@param[in] address: address ready to write to SPI slave.
*@return None.
*/
static void spim_write_slave_address(uint32_t address)
{
    uint8_t write_low_addr[4];
    uint8_t write_high_addr[4];

    write_low_addr[0] = SPI_SLAVER_CONFIG_WRITE;
    write_low_addr[1] = SPI_SLAVER_REG_LOW_ADDRESS;
    write_low_addr[2] = (address >> 8) & 0xff;
    write_low_addr[3] = address & 0xff;
    write_high_addr[0] = SPI_SLAVER_CONFIG_WRITE;
    write_high_addr[1] = SPI_SLAVER_REG_HIGH_ADDRESS;
    write_high_addr[2] = (address >> 24) & 0xff;
    write_high_addr[3] = (address >> 16) & 0xff;
    hal_spi_master_send_polling(SPI_TEST_MASTER, write_low_addr, 4);
    hal_spi_master_send_polling(SPI_TEST_MASTER, write_high_addr, 4);
}

/**
*@brief SPI master issue read/write command to SPI slave.
*@param[in] is_read: identify read or write command.
*@return None.
*/
static void spim_write_slave_command(bool is_read)
{
    uint8_t command_buffer[4];

    command_buffer[0] = SPI_SLAVER_CONFIG_WRITE;
    command_buffer[1] = SPI_SLAVER_REG_CONFIG;
    command_buffer[2] = 0x00;
    if (is_read == true) {
        command_buffer[3] = SPI_SLAVER_CONFIG_BUS_SIZE | SPI_SLAVER_CONFIG_BUS_R;
    } else {
        command_buffer[3] = SPI_SLAVER_CONFIG_BUS_SIZE | SPI_SLAVER_CONFIG_BUS_W;
    }
    hal_spi_master_send_polling(SPI_TEST_MASTER, command_buffer, 4);
}

/**
*@brief SPI master write data to SPI slave.
*@param[in] data: the data ready to sent to SPI slave.
*@return None.
*/
static void spim_write_slave_data(uint32_t data)
{
    uint8_t write_low_data[4];
    uint8_t write_high_data[4];

    write_low_data[0] = SPI_SLAVER_CONFIG_WRITE;
    write_low_data[1] = SPI_SLAVER_REG_WRITE_LOW_DATA;
    write_low_data[2] = (data >> 8) & 0xff;
    write_low_data[3] = data & 0xff;
    write_high_data[0] = SPI_SLAVER_CONFIG_WRITE;
    write_high_data[1] = SPI_SLAVER_REG_WRITE_HIGH_DATA;
    write_high_data[2] = (data >> 24) & 0xff;
    write_high_data[3] = (data >> 16) & 0xff;

    hal_spi_master_send_polling(SPI_TEST_MASTER, write_low_data, 4);
    hal_spi_master_send_polling(SPI_TEST_MASTER, write_high_data, 4);
}

/**
*@brief SPI master read data from SPI slave.
*@param None.
*@return Return the data read from SPI slave.
*/
static uint32_t spim_read_slave_data(void)
{
    uint8_t command_buffer[4];
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
    uint32_t data, temp_buffer;

    command_buffer[0] = SPI_SLAVER_CONFIG_READ;
    command_buffer[1] = SPI_SLAVER_REG_READ_LOW_DATA;
    spi_send_and_receive_config.send_data = command_buffer;
    spi_send_and_receive_config.send_length = 2;
    spi_send_and_receive_config.receive_buffer = (uint8_t *)&temp_buffer;
    spi_send_and_receive_config.receive_length = 4;
    hal_spi_master_send_and_receive_polling(SPI_TEST_MASTER, &spi_send_and_receive_config);

    data = (temp_buffer >> 24) & 0x000000ff;
    data |= (temp_buffer >> 8) & 0x0000ff00;
    command_buffer[0] = SPI_SLAVER_CONFIG_READ;
    command_buffer[1] = SPI_SLAVER_REG_READ_HIGH_DATA;
    spi_send_and_receive_config.send_data = command_buffer;
    spi_send_and_receive_config.send_length = 2;
    spi_send_and_receive_config.receive_buffer = (uint8_t *)&temp_buffer;
    spi_send_and_receive_config.receive_length = 4;
    hal_spi_master_send_and_receive_polling(SPI_TEST_MASTER, &spi_send_and_receive_config);

    data |= (temp_buffer >> 8) & 0x00ff0000;
    data |= (temp_buffer << 8) & 0xff000000;

    return data;
}

/**
*@brief SPI master query status of SPI slave's ACK flag until it's set.
*@param None.
*@return None.
*/
static void spim_wait_slave_ack(void)
{
    uint32_t ack_flag;

    do {
        spim_write_slave_address(SPI_TEST_SLV_BUFFER_ADDRESS + SPI_TEST_CONTROL_SIZE);
        spim_write_slave_command(true);
        spim_wait_slave_idle();
        ack_flag = spim_read_slave_data();
    } while (ack_flag != SPI_TEST_ACK_FLAG);

    spim_write_slave_address(SPI_TEST_SLV_BUFFER_ADDRESS + SPI_TEST_CONTROL_SIZE);
    spim_write_slave_data(0);
    spim_write_slave_command(false);
    spim_wait_slave_idle();
}

/**
*@brief SPI master issue IRQ command to SPI slave.
*@param None.
*@return None.
*/
static void spim_active_slave_irq(void)
{
    uint8_t command_buffer[4];

    command_buffer[0] = SPI_SLAVER_CONFIG_WRITE;
    command_buffer[1] = SPI_SLAVER_REG_SLV_IRQ;
    command_buffer[2] = 0x00;
    command_buffer[3] = 0x01;
    hal_spi_master_send_polling(SPI_TEST_MASTER, command_buffer, 4);
}


/**
*@brief  This function is used to send data from SPI master to SPI slave.
*@param None.
*@return None.
*/
static void spim_send_data_to_spis(void)
{
    uint32_t i;

    printf("spim_send_data_to_spis() begin\r\n");

    /* Step1: wait SPI slave to be idle status. */
    spim_wait_slave_idle();

    /* Step2: tell slaver that master want to write date to slave. */
    spim_write_slave_address(SPI_TEST_SLV_BUFFER_ADDRESS);
    spim_write_slave_data(SPI_TEST_WRITE_DATA_BEGIN);
    spim_write_slave_command(false);
    spim_wait_slave_idle();
    spim_active_slave_irq();
    spim_wait_slave_ack();

    /* Step3: SPI master send data to SPI slaver. */
    spim_write_slave_address(SPI_TEST_SLV_BUFFER_ADDRESS + SPI_TEST_CONTROL_SIZE + SPI_TEST_STATUS_SIZE);
    for (i = 0; i < SPI_TEST_DATA_SIZE / 4; i++) {
        spim_write_slave_data(SPI_TEST_TX_DATA_PATTERN);
        spim_write_slave_command(false);
        spim_wait_slave_idle();
    }

    /* Step4: notice slave send completely and wait slave to ack. */
    spim_write_slave_address(SPI_TEST_SLV_BUFFER_ADDRESS);
    spim_write_slave_data(SPI_TEST_WRITE_DATA_END);
    spim_write_slave_command(false);
    spim_active_slave_irq();
    spim_wait_slave_ack();

    printf("spim_send_data_to_spis() pass\r\n\r\n");
}


/**
*@brief  This function is used to receive data from spi slave to spi master.
*@param None.
*@return None.
*/
static void spim_receive_data_from_spis(void)
{
    uint32_t i, data;

    printf("spim_receive_data_from_spis() begin\r\n");

    /* Step1: wait SPI slave to be idle status. */
    spim_wait_slave_idle();

    /* Step2: tell slaver that master want to read date from slave. */
    spim_write_slave_address(SPI_TEST_SLV_BUFFER_ADDRESS);
    spim_write_slave_data(SPI_TEST_READ_DATA_BEGIN);
    spim_write_slave_command(false);
    spim_wait_slave_idle();
    spim_active_slave_irq();
    spim_wait_slave_ack();

    /* Step3: SPI master read data from SPI slaver and verify it. */
    spim_write_slave_address(SPI_TEST_SLV_BUFFER_ADDRESS + SPI_TEST_CONTROL_SIZE + SPI_TEST_STATUS_SIZE);
    for (i = 0; i < SPI_TEST_DATA_SIZE / 4; i++) {
        spim_write_slave_command(true);
        spim_wait_slave_idle();
        data = spim_read_slave_data();
        if (data != SPI_TEST_RX_DATA_PATTERN) {
            printf("spim_read_slave_data fail: i=%d, data=%x\r\n", (int)i, (unsigned int)data);
            return;
        }
    }

    /* Step4: notice SPI slave master has read completely. */
    spim_write_slave_address(SPI_TEST_SLV_BUFFER_ADDRESS);
    spim_write_slave_data(SPI_TEST_READ_DATA_END);
    spim_write_slave_command(false);
    spim_wait_slave_idle();
    spim_active_slave_irq();

    printf("spim_receive_data_from_spis() pass\r\n\r\n");
}

/**
*@brief  Example of spi send datas. In this function, SPI master send various commonds to slave.
*@param  None.
*@return None.
*/
static void spi_master_transfer_data_two_boards_example(void)
{
    hal_spi_master_config_t spi_config;

    printf("---spi_master_transfer_data_two_boards_example begins---\r\n\r\n");

    /*Step1: init GPIO, set SPIM pinmux(if EPT tool hasn't been used to configure the related pinmux).*/
    hal_gpio_init(SPIM_PIN_NUMBER_CS);
    hal_gpio_init(SPIM_PIN_NUMBER_CLK);
    hal_gpio_init(SPIM_PIN_NUMBER_MOSI);
    hal_gpio_init(SPIM_PIN_NUMBER_MISO);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_CS, SPIM_PIN_FUNC_CS);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_CLK, SPIM_PIN_FUNC_CLK);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_MOSI, SPIM_PIN_FUNC_MOSI);
    hal_pinmux_set_function(SPIM_PIN_NUMBER_MISO, SPIM_PIN_FUNC_MISO);

    /*Step2: initialize SPI master. */
    spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.clock_frequency = SPI_TEST_FREQUENCY;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_init(SPI_TEST_MASTER, &spi_config)) {
        printf("hal_spi_master_init fail\r\n");
        return;
    } else {
        printf("hal_spi_master_init pass\r\n");
    }

    /* Step3: SPI master send data to SPI slaver and verify whether received data is correct. */
    spim_send_data_to_spis();

    /* Step4: SPI master receive data from SPI slaver and verify whether received data is correct. */
    spim_receive_data_from_spis();

    /* Step5: deinit spi master and related GPIO. */
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_deinit(SPI_TEST_MASTER)) {
        printf("hal_spi_master_deinit fail\r\n");
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

