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
#define SPIS_TEST_PORT         HAL_SPI_SLAVE_0
#define SPIS_PIN_NUMBER_CS     HAL_GPIO_25
#define SPIS_PIN_FUNC_CS       HAL_GPIO_25_SLV_SPI0_CS
#define SPIS_PIN_NUMBER_SLK    HAL_GPIO_26
#define SPIS_PIN_FUNC_SLK      HAL_GPIO_26_SLV_SPI0_SCK
#define SPIS_PIN_NUMBER_MOSI   HAL_GPIO_27
#define SPIS_PIN_FUNC_MOSI     HAL_GPIO_27_SLV_SPI0_MOSI
#define SPIS_PIN_NUMBER_MISO   HAL_GPIO_28
#define SPIS_PIN_FUNC_MISO     HAL_GPIO_28_SLV_SPI0_MISO
#define SPI_TEST_DATA_SIZE     (1024)
#define SPI_TEST_DATA_RANGE    115

/* Private variables ---------------------------------------------------------*/
static volatile bool spi_slave_poweron_flag;
static volatile bool spi_slave_cfg_write_flag;
static volatile bool spi_slave_write_data_flag;
static volatile bool spi_slave_cfg_read_flag;
static volatile bool spi_slave_read_data_flag;
static volatile bool spi_slave_timeout_flag;
static volatile bool spi_slave_read_error_flag;
static volatile bool spi_slave_write_error_flag;
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_tansfer_buffer[SPI_TEST_DATA_SIZE] = {0};

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
*@brief     In this function user define it's callback.
*@param[in] status:    offers the SPI slave FSM status and interupt status of SPI slave controller.
*@param[in] user_data: pointer to user's data that user will use in this callback.
*@return    None.
*/
static void spi_slave_callback(hal_spi_slave_transaction_status_t status, void *user_data)
{
    hal_spi_slave_callback_event_t slave_status;
    uint32_t request_address, request_length;

    if (HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION == (status.fsm_status)) {
        /* Normal fsm behavior */
        slave_status = status.interrupt_status;
        switch (slave_status) {
            case HAL_SPI_SLAVE_EVENT_POWER_ON:
                /* PDN is turned on, initializes spi slave controller here */
                spi_slave_poweron_flag = true;
                printf("  ---Receive POWERON command----\r\n");
                break;
            case HAL_SPI_SLAVE_EVENT_POWER_OFF:
                hal_spi_slave_deinit(SPIS_TEST_PORT);
                spi_slave_poweron_flag = false;
                printf("  ---Receive POWEROFF command----\r\n");
                break;
            case HAL_SPI_SLAVE_EVENT_CRD_FINISH:
                /* Call SPI slave send function here to set data address and size*/
                hal_spi_slave_get_master_read_config(SPIS_TEST_PORT, &request_address, &request_length);
                hal_spi_slave_send(SPIS_TEST_PORT, g_tansfer_buffer, request_length);
                spi_slave_cfg_read_flag = true;
                printf("  ---Receive CRD_FINISH command----\r\n");
                break;
            case HAL_SPI_SLAVE_EVENT_CWR_FINISH:
                /* call spi slave read function here to set data address and size*/
                hal_spi_slave_get_master_write_config(SPIS_TEST_PORT, &request_address, &request_length);
                hal_spi_slave_receive(SPIS_TEST_PORT, g_tansfer_buffer, request_length);
                spi_slave_cfg_write_flag = true;
                printf("  ---Receive CWR_FINISH command----\r\n");
                break;
            case HAL_SPI_SLAVE_EVENT_RD_FINISH:
                spi_slave_read_data_flag = true;
                printf("  ---Receive RD_FINISH command----\r\n");
                break;
            case HAL_SPI_SLAVE_EVENT_WR_FINISH:
                /* User can now get the data from the address set before */
                spi_slave_write_data_flag = true;
                printf("  ---Receive WR_FINISH command----\r\n");
                break;
            case HAL_SPI_SLAVE_EVENT_RD_ERR:
                /* Data buffer may be reserved for retransmit depending on user's usage */
                spi_slave_read_error_flag = true;
                printf("  ---detect RD_ERR----\r\n");
                break;
            case HAL_SPI_SLAVE_EVENT_WR_ERR:
                /* Data in the address set before isn't correct, user may abandon them */
                spi_slave_write_error_flag = true;
                printf("  ---detect WR_ERR----\r\n");
                break;
            case HAL_SPI_SLAVE_EVENT_TIMEOUT_ERR:
                /* timeout happen */
                spi_slave_timeout_flag = true;
                printf("  ---detect TMOUT_ERR----\r\n");
                break;
            default:
                break;
        }
    } else if (HAL_SPI_SLAVE_FSM_INVALID_OPERATION != (status.fsm_status)) {
        switch (status.fsm_status) {
            case HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CR:
                printf("  HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CR, fsm is poweroff\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CW:
                printf("  HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CW, fsm is poweroff\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CR:
                printf("  HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CR, fsm is CR\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CR_AFTER_CW:
                printf("  HAL_SPI_SLAVE_FSM_ERROR_CR_AFTER_CW, fsm is CR\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CW:
                printf("  HAL_SPI_SLAVE_FSM_ERROR_CONTINOUS_CW, fsm is CW\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_CW_AFTER_CR:
                printf("  HAL_SPI_SLAVE_FSM_ERROR_CW_AFTER_CR, fsm is CW\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_WRITE_AFTER_CR:
                printf("  HAL_SPI_SLAVE_FSM_ERROR_WRITE_AFTER_CR, fsm is poweron\r\n");
                break;
            case HAL_SPI_SLAVE_FSM_ERROR_READ_AFTER_CW:
                printf("  HAL_SPI_SLAVE_FSM_ERROR_READ_AFTER_CW, fsm is poweron\r\n");
                break;
            default:
                break;
        }
    } else {
        printf("  HAL_SPI_SLAVE_FSM_INVALID_OPERATION, fsm is poweron\r\n");
    }
}

/**
*@brief  Example of SPI slave receive data. In this function, SPI slave receive various commond from SPI master.
*@param  None.
*@return None.
*/
static void spi_slave_transfer_data_two_boards_example(void)
{
    uint32_t i = 0;
    hal_spi_slave_config_t spi_configure;

    printf("---spi_slave_transfer_data_two_boards_example begins---\r\n\r\n");

    /* Step1: Reset SPI slaver status and transfer buffer. */
    spi_slave_poweron_flag = false;
    spi_slave_cfg_write_flag = false;
    spi_slave_write_data_flag = false;
    spi_slave_write_error_flag = false;
    spi_slave_cfg_read_flag = false;
    spi_slave_read_data_flag = false;
    spi_slave_read_error_flag = false;
    spi_slave_timeout_flag = false;
    memset(g_tansfer_buffer, 0x00, sizeof(g_tansfer_buffer));

    /* Step2: GPIO configuration for SPI master */
    hal_gpio_init(SPIS_PIN_NUMBER_CS);
    hal_gpio_init(SPIS_PIN_NUMBER_SLK);
    hal_gpio_init(SPIS_PIN_NUMBER_MOSI);
    hal_gpio_init(SPIS_PIN_NUMBER_MISO);
    hal_pinmux_set_function(SPIS_PIN_NUMBER_CS, SPIS_PIN_FUNC_CS);
    hal_pinmux_set_function(SPIS_PIN_NUMBER_SLK, SPIS_PIN_FUNC_SLK);
    hal_pinmux_set_function(SPIS_PIN_NUMBER_MOSI, SPIS_PIN_FUNC_MOSI);
    hal_pinmux_set_function(SPIS_PIN_NUMBER_MISO, SPIS_PIN_FUNC_MISO);
    spi_configure.bit_order = HAL_SPI_SLAVE_LSB_FIRST;
    spi_configure.phase = HAL_SPI_SLAVE_CLOCK_PHASE0;
    spi_configure.polarity = HAL_SPI_SLAVE_CLOCK_POLARITY0;
    spi_configure.timeout_threshold = 0xFFFFFFFF;
    if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_init(SPIS_TEST_PORT, &spi_configure)) {
        printf("SPI slave init fail\r\n");
    }

    /* Step3: Register callback function to SPI slaver driver. */
    if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_register_callback(SPIS_TEST_PORT, spi_slave_callback, NULL)) {
        printf("SPI slave register callback fail\r\n");
    }

    /* Step4: wait SPI master to send power on command. */
    printf("SPI Slave is waiting for PWON command\r\n");
    while (spi_slave_poweron_flag == false) {
        if (spi_slave_timeout_flag == true) {
            printf("  Slave detect PWON command error\r\n");
            return;
        }
    }

    /* Step5: wait SPI master to send config write command. */
    printf("SPI Slave is waiting for CW command\r\n");
    while (spi_slave_cfg_write_flag == false);

    /* Step6: wait SPI master to send write data command. */
    printf("SPI Slave is waiting for WR command\r\n");
    while (spi_slave_write_data_flag == false) {
        if (spi_slave_write_error_flag == true) {
            printf("  Slave detect WR command error\r\n");
            return;
        }
    }

    /* Step7: check whether the data received is correct. */
    printf("SPI slave Start to check data integrity\r\n");
    for (i = 0; i < SPI_TEST_DATA_SIZE; i++) {
        if (g_tansfer_buffer[i] != ((i + 1) % SPI_TEST_DATA_RANGE)) {
            printf("  Data check fail\r\n");
            return;
        }
    }
    printf("  Data check pass\r\n");

    /* Step8: wait SPI master to send config read command. */
    printf("SPI Slave is waiting for CR command\r\n");
    while (spi_slave_cfg_read_flag == false);

    /* Step9: wait SPI master to send read data command. */
    printf("SPI Slave is waiting for RD command\r\n");
    while (spi_slave_read_data_flag == false) {
        if (spi_slave_read_error_flag == true) {
            printf("  SPI Slave detect RD error\r\n");
            return;
        }
    }

    /* Step10: wait SPI master to send power off command. */
    printf("SPI Slave is waiting for PWOFF command\r\n");
    while (spi_slave_poweron_flag == true);

    /* Step11: De-initialize SPI master and the GPIO. */
    printf("SPI slave deinit\r\n\r\n");
    if (HAL_SPI_SLAVE_STATUS_OK != hal_spi_slave_deinit(SPIS_TEST_PORT)) {
        printf("  SPI slave deinit failed\r\n");
        return;
    }
    hal_gpio_deinit(SPIS_PIN_NUMBER_CS);
    hal_gpio_deinit(SPIS_PIN_NUMBER_SLK);
    hal_gpio_deinit(SPIS_PIN_NUMBER_MOSI);
    hal_gpio_deinit(SPIS_PIN_NUMBER_MISO);

    printf("---spi_slave_transfer_data_two_boards_example ends---\r\n");
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
    spi_slave_transfer_data_two_boards_example();

    while (1);
}

