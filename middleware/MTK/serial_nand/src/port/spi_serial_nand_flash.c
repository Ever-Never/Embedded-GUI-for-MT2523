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


#ifdef MTK_FATFS_ON_SERIAL_NAND
/* hal includes */
#include "memory_attribute.h"
#include "serial_nand_flash.h"
#include "hal_gpio.h"
#include "hal_gpt.h"
#include "hal.h"
#include "FreeRTOS.h"
#include "task.h"

/* Flash opcodes. */
#define SERIAL_NAND_CMD_WREN            0x06    /* Write enable */
#define SERIAL_NAND_CMD_WRDI            0x04    /* Write disable */
#define SERIAL_NAND_CMD_GETFEA          0x0F    /* Get feature */
#define SERIAL_NAND_CMD_SETFEA          0x1F    /* Set feature */
#define SERIAL_NAND_CMD_READ            0x13    /* read data to cache */
#define SERIAL_NAND_CMD_NORM_READ       0x03    /* Read data bytes (low frequency) */
#define SERIAL_NAND_CMD_FAST_READ       0x0B    /* Read data bytes (high frequency) */
#define SERIAL_NAND_CMD_PLOAD           0x02    /* Program load */
#define SERIAL_NAND_CMD_PLOAD_RANDOM    0x84    /* Program load Random Data */
#define SERIAL_NAND_CMD_PROG            0x10    /* Program execute */
#define SERIAL_NAND_CMD_ERASE           0xD8    /* Block erase */
#define SERIAL_NAND_CMD_RDID            0x9F    /* Read JEDEC ID */
#define SERIAL_NAND_CMD_RESET           0xFF    /* reset nand flash */

/* Flash Protection register */
#define SERIAL_NAND_PROTEC_REG          0xA0
#define SERIAL_NAND_PROTEC_BRWD         0x80
#define SERIAL_NAND_PROTEC_BP2          0x20
#define SERIAL_NAND_PROTEC_BP1          0x10
#define SERIAL_NAND_PROTEC_BP0          0x08
#define SERIAL_NAND_PROTEC_INV          0x04
#define SERIAL_NAND_PROTEC_CMP          0x02

/* Flash feature register */
#define SERIAL_NAND_FEATURE_REG         0xB0
#define SERIAL_NAND_FEATURE_OTPPRT      0x80
#define SERIAL_NAND_FEATURE_OPTEN       0x40
#define SERIAL_NAND_FEATURE_ECC_EN      0x10
#define SERIAL_NAND_FEATURE_QE          0x01

/* Flash status register. */
#define SERIAL_NAND_STATUS_REG          0xC0
#define SERIAL_NAND_STATUS_BUSY         0x1
#define SERIAL_NAND_STATUS_WREN         0x2
#define SERIAL_NAND_STATUS_EFAIL        0x4
#define SERIAL_NAND_STATUS_PFAIL        0x8
#define SERIAL_NAND_STATUS_ECCMASK      0x30
#define SERIAL_NAND_STATUS_ECC(x)       (x << 4)
#define SERIAL_NAND_STATUS_ECC0         0x0
#define SERIAL_NAND_STATUS_ECC1         0x1
#define SERIAL_NAND_STATUS_ECC2         0x2
#define SERIAL_NAND_STATUS_ECC3         0x3

#define SERIAL_NAND_VERC_STATUS_ECCMASK 0x70
#define SERIAL_NAND_VERC_STATUS_ERR     0x7

/* Define max times to check status register before we give up. */
#define MAX_CMD_SIZE                8
#define SERIAL_NAND_SUBFEATURE_LEN      1

#define FLASH_MAX_DELAY 0xffffffff

/* Define parameters of NAND flash. */
#define SERIAL_NAND_SPARE_SIZE 0x00000100
#define SERIAL_NAND_SPARE_SIZE_MASK 0x000000ff
#define SERIAL_NAND_SPARE_SIZE_SHIFT 8
#define SERIAL_NAND_PAGE_SIZE 0x00001000
#define SERIAL_NAND_PAGE_SIZE_MASK 0x00000fff
#define SERIAL_NAND_PAGE_SIZE_SHIFT 12
#define SERIAL_NAND_BLOCK_SIZE 0x00040000
#define SERIAL_NAND_BLOCK_SIZE_MASK 0x0003ffff
#define SERIAL_NAND_BLOCK_SIZE_SHIFT 18
#define SERIAL_NAND_MAX_ADDRESS 0xffffffff
#define SERIAL_NAND_TOTAL_SIZE 0x100000000
#define SERIAL_NAND_MAX_FREQUENCY 13000000

#define SPI_TX_BUFFER (SERIAL_NAND_PAGE_SIZE + SERIAL_NAND_SPARE_SIZE + 32)
#define SPI_RX_BUFFER (SERIAL_NAND_PAGE_SIZE + SERIAL_NAND_SPARE_SIZE + 32)

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_spi_tx_buffer[SPI_TX_BUFFER];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t g_spi_rx_buffer[SPI_RX_BUFFER];

static bool g_flash_init_flag = false;
static volatile bool g_flash_dma_complete_flag = false;
static hal_spi_master_port_t g_spim_port;

static void wait_spi_dma_complete(void);

static bool flash_internal_dma_write(hal_spi_master_port_t master_port, uint8_t *data, uint32_t size)
{
    hal_spi_master_status_t ret;

    //if ((xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) || (HAL_NVIC_QUERY_EXCEPTION_NUMBER != 0)) {
    if ((SysInitStatus_Query() == 0) || (HAL_NVIC_QUERY_EXCEPTION_NUMBER != 0)) {
        ret = hal_spi_master_send_dma_blocking(master_port, data, size);
        if (ret != HAL_SPI_MASTER_STATUS_OK) {
            return false;
        }
    } else {
        ret = hal_spi_master_send_dma(master_port, data, size);
        if (ret != HAL_SPI_MASTER_STATUS_OK) {
            return false;
        }
        wait_spi_dma_complete();
    }

    return true;
}

static bool flash_internal_dma_read(hal_spi_master_port_t master_port,
                                    hal_spi_master_send_and_receive_config_t *spi_send_and_receive_config)
{
    hal_spi_master_status_t ret;

    //if ((xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) || (HAL_NVIC_QUERY_EXCEPTION_NUMBER != 0)) {
    if ((SysInitStatus_Query() == 0) || (HAL_NVIC_QUERY_EXCEPTION_NUMBER != 0)) {
        ret = hal_spi_master_send_and_receive_dma_blocking(master_port, spi_send_and_receive_config);
        if (ret != HAL_SPI_MASTER_STATUS_OK) {
            return false;
        }
    } else {
        ret = hal_spi_master_send_and_receive_dma(master_port, spi_send_and_receive_config);
        if (ret != HAL_SPI_MASTER_STATUS_OK) {
            return false;
        }
        wait_spi_dma_complete();
    }

    return true;
}

static void flash_dma_callback(hal_spi_master_callback_event_t event, void *user_data)
{
    g_flash_dma_complete_flag = true;
    //log_hal_info("flash_dma_callback\r\n");
}


static void serial_nand_set_feature(uint8_t reg, uint8_t value)
{
    hal_spi_master_status_t ret;

    /* 1FH (SET FEATURES command) */
    g_spi_tx_buffer[0] = SERIAL_NAND_CMD_SETFEA;
    g_spi_tx_buffer[1] = reg;
    g_spi_tx_buffer[2] = value;
    ret = hal_spi_master_send_polling(g_spim_port, g_spi_tx_buffer, 3);
    if (ret != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("SET FEATURE fail\r\n");
    }
}

static void serial_nand_get_feature(uint8_t reg, uint8_t *value)
{
    hal_spi_master_status_t ret;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    /* 0FH (GET FEATURES command to read the status) */
    g_spi_tx_buffer[0] = SERIAL_NAND_CMD_GETFEA;
    g_spi_tx_buffer[1] = reg;
    spi_send_and_receive_config.send_length = 2;
    spi_send_and_receive_config.send_data = g_spi_tx_buffer;
    spi_send_and_receive_config.receive_length = 1 + spi_send_and_receive_config.send_length;
    spi_send_and_receive_config.receive_buffer = g_spi_rx_buffer;
    ret = hal_spi_master_send_and_receive_polling(g_spim_port, &spi_send_and_receive_config);
    if (ret != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("GET FEATURE fail\r\n");
    }

    if (value != NULL) {
        *value = g_spi_rx_buffer[2];
    }
}

static bool flash_wait_busy(uint32_t timeout_ms, uint8_t *status)
{
    uint8_t tx_buffer[16], rx_buffer[16];
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
    uint32_t begin_time, curr_time;
    hal_spi_master_status_t ret;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &begin_time);
    do {
        tx_buffer[0] = SERIAL_NAND_CMD_GETFEA;
        tx_buffer[1] = SERIAL_NAND_STATUS_REG; /* Status Register */
        spi_send_and_receive_config.send_length = 2;
        spi_send_and_receive_config.send_data = tx_buffer;
        spi_send_and_receive_config.receive_length = 1 + spi_send_and_receive_config.send_length;
        spi_send_and_receive_config.receive_buffer = rx_buffer;
        ret = hal_spi_master_send_and_receive_polling(g_spim_port, &spi_send_and_receive_config);
        if (ret != HAL_SPI_MASTER_STATUS_OK) {
            log_hal_error("GET FEATURE fail\r\n");
        }

        if (!(rx_buffer[2] & SERIAL_NAND_STATUS_BUSY)) {
            if (status != NULL) {
                *status = rx_buffer[2];
            }
            return true;
        }

        if (timeout_ms != FLASH_MAX_DELAY) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_time);
            if (((curr_time >= begin_time) && ((curr_time - begin_time) > (timeout_ms * 1000))) ||
                    ((curr_time < begin_time) && (((0xffffffff - begin_time) + (curr_time + 1)) > (timeout_ms * 1000)))) {
                if (status != NULL) {
                    *status = rx_buffer[2];
                }
                return false;
            }
        }
    } while (1);
}


uint8_t serial_nand_get_status(void)
{

    return 1;
}

static void wait_spi_dma_complete(void)
{
    while (g_flash_dma_complete_flag == false) {
        vTaskDelay(5);
    }

    g_flash_dma_complete_flag = false;
}

/*
 * 1. 13H (PAGE READ to cache)
 * 2. 0FH (GET FEATURES command to read the status)
 * 3. 03H or 0BH (Read from cache)
 */
static void flash_internal_read(uint32_t page, uint32_t column, uint8_t *buffer, uint32_t length,
                                uint32_t spare_offset, uint8_t *spare_data, uint32_t spare_length)
{
    uint32_t page_index;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
    hal_spi_master_status_t ret;
    uint8_t status;

    //log_hal_info("[Read]:page=0x%x,column=%d,buffer=0x%x,length=%d,spare_offset=0x%x,spare_data=%08x,spare_length=%d\r\n",
    //               page, column, (uint32_t)buffer, length, spare_offset, (uint32_t)spare_data, spare_length);

    /* 13H (PAGE READ to cache) */
    //log_hal_info("PAGE READ to cache\r\n");
    page_index = page >> SERIAL_NAND_PAGE_SIZE_SHIFT;
    g_spi_tx_buffer[0] = SERIAL_NAND_CMD_READ;
    g_spi_tx_buffer[1] = (page_index >> 16) & 0xff;
    g_spi_tx_buffer[2] = (page_index >> 8) & 0xff;
    g_spi_tx_buffer[3] = page_index & 0xff;
    ret = hal_spi_master_send_polling(g_spim_port, g_spi_tx_buffer, 4);
    if (ret != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("PAGE READ to cache\r\n");
    }

    /* 0FH (GET FEATURES command to read the status) */
    do {
        serial_nand_get_feature(SERIAL_NAND_STATUS_REG, &status);
        //log_hal_info("status reg: %02x\r\n", status);
    } while (status & SERIAL_NAND_STATUS_BUSY);

    /* 03H or 0BH (Read from cache) */
    if (buffer != NULL) {
        //log_hal_info("Read from cache 1\r\n");
        g_spi_tx_buffer[0] = SERIAL_NAND_CMD_NORM_READ;
        g_spi_tx_buffer[1] = (column >> 8) & 0xff;
        g_spi_tx_buffer[2] = column & 0xff;
        g_spi_tx_buffer[3] = 0x00;
        spi_send_and_receive_config.send_length = 4;
        spi_send_and_receive_config.send_data = g_spi_tx_buffer;
        spi_send_and_receive_config.receive_length = length + spi_send_and_receive_config.send_length;
        spi_send_and_receive_config.receive_buffer = g_spi_rx_buffer;
        if (flash_internal_dma_read(g_spim_port, &spi_send_and_receive_config) == false) {
            log_hal_error("Read from cache1\r\n");
        }
        memcpy(buffer, &g_spi_rx_buffer[4], length);
    }
    if (spare_data != NULL) {
        //log_hal_info("Read from cache 2\r\n");
        g_spi_tx_buffer[0] = SERIAL_NAND_CMD_NORM_READ;
        g_spi_tx_buffer[1] = ((SERIAL_NAND_PAGE_SIZE + spare_offset) >> 8) & 0xff;
        g_spi_tx_buffer[2] = (SERIAL_NAND_PAGE_SIZE + spare_offset) & 0xff;
        g_spi_tx_buffer[3] = 0x00;
        spi_send_and_receive_config.send_length = 4;
        spi_send_and_receive_config.send_data = g_spi_tx_buffer;
        spi_send_and_receive_config.receive_length = spare_length + spi_send_and_receive_config.send_length;
        spi_send_and_receive_config.receive_buffer = g_spi_rx_buffer;
        if (flash_internal_dma_read(g_spim_port, &spi_send_and_receive_config) == false) {
            log_hal_error("Read from cache1\r\n");
        }
        memcpy(spare_data, &g_spi_rx_buffer[4], spare_length);
    }
}

static void flash_internal_write(uint32_t page, uint32_t column, uint8_t *buffer, uint32_t length,
                                 uint32_t spare_offset, uint8_t *spare_data, uint32_t spare_length)
{
    uint32_t page_index;
    hal_spi_master_status_t ret;
    uint8_t status;

    //log_hal_info("[Write]: page=0x%x,column=0x%x,buffer=%08x,length=%d,spare_offset=%d,spare_data=%08x,spare_length=%d\r\n",
    //                page, column, (uint32_t)buffer, length, spare_offset, (uint32_t)spare_data, spare_length);

    /* 13H (page read to cache) */
    //log_hal_info("PAGE READ to cache\r\n");
    page_index = page >> SERIAL_NAND_PAGE_SIZE_SHIFT;
    g_spi_tx_buffer[0] = SERIAL_NAND_CMD_READ;
    g_spi_tx_buffer[1] = (page_index >> 16) & 0xff;
    g_spi_tx_buffer[2] = (page_index >> 8) & 0xff;
    g_spi_tx_buffer[3] = page_index & 0xff;
    ret = hal_spi_master_send_polling(g_spim_port, g_spi_tx_buffer, 4);
    if (ret != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("PAGE READ to cache\r\n");
    }

    /* 84H (Program load random data) */
    if (buffer != NULL) {
        //log_hal_info("Program load random data for normal data\r\n");
        //memset(g_spi_tx_buffer, 0xFF, sizeof(g_spi_tx_buffer));
        g_spi_tx_buffer[0] = SERIAL_NAND_CMD_PLOAD_RANDOM;
        g_spi_tx_buffer[1] = (column >> 8) & 0xff;
        g_spi_tx_buffer[2] = column & 0xff;
        memcpy(&g_spi_tx_buffer[3], buffer, length);
        if (flash_internal_dma_write(g_spim_port, g_spi_tx_buffer, 3 + length) == false) {
            log_hal_error("Program load fail\r\n");
        }
        //log_hal_info("Program load ok\r\n");
    }
    if (spare_data != NULL) {
        //log_hal_info("Program load random data for spare data\r\n");
        //memset(g_spi_tx_buffer, 0xFF, sizeof(g_spi_tx_buffer));
        g_spi_tx_buffer[0] = SERIAL_NAND_CMD_PLOAD_RANDOM;
        g_spi_tx_buffer[1] = ((spare_offset + SERIAL_NAND_PAGE_SIZE) >> 8) & 0xff;
        g_spi_tx_buffer[2] = (spare_offset + SERIAL_NAND_PAGE_SIZE) & 0xff;
        memcpy(&g_spi_tx_buffer[3], spare_data, spare_length);
        if (flash_internal_dma_write(g_spim_port, g_spi_tx_buffer, 3 + spare_length) == false) {
            log_hal_error("Program load fail\r\n");
        }
        //log_hal_info("Program load ok\r\n");
    }

    /* 06H (WRITE ENABLE) */
    //log_hal_info("WRITE ENABLE\r\n");
    g_spi_tx_buffer[0] = SERIAL_NAND_CMD_WREN;
    ret = hal_spi_master_send_polling(g_spim_port, g_spi_tx_buffer, 1);
    if (ret != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("WRITE ENABLE fail\r\n");
    }

    /* 10H (PROGRAM EXECUTE) */
    //log_hal_info("PROGRAM EXECUTE\r\n");
    page_index = page >> SERIAL_NAND_PAGE_SIZE_SHIFT;
    g_spi_tx_buffer[0] = SERIAL_NAND_CMD_PROG;
    g_spi_tx_buffer[1] = (page_index >> 16) & 0xff; /* 24-bit row address */
    g_spi_tx_buffer[2] = (page_index >> 8) & 0xff; /* 24-bit row address */
    g_spi_tx_buffer[3] = page_index & 0xff; /* 24-bit row address */
    ret = hal_spi_master_send_polling(g_spim_port, g_spi_tx_buffer, 4);
    if (ret != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("PROGRAM EXECUTE fail 1\r\n");
    }

    /* 0FH (GET FEATURE command to read the status) */
    do {
        serial_nand_get_feature(SERIAL_NAND_STATUS_REG, &status);
        //log_hal_info("status reg: %02x\r\n", status);
        if (status & SERIAL_NAND_STATUS_PFAIL) {
            log_hal_error("PROGRAM EXECUTE fail: %02x, page: %d \r\n", status, page);
            return;
        }
    } while (status & SERIAL_NAND_STATUS_BUSY);

    /* 06H (WRITE DISABLE) */
    //log_hal_info("WRITE DISABLE\r\n");
    g_spi_tx_buffer[0] = SERIAL_NAND_CMD_WRDI;
    ret = hal_spi_master_send_polling(g_spim_port, g_spi_tx_buffer, 1);
    if (ret != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("WRITE DISABLE fail\r\n");
    }
}

serial_nand_status_t serial_nand_init(hal_spi_master_port_t spim_port, uint32_t frequency)
{
    uint8_t status;
    hal_spi_master_config_t spi_config;

    if (frequency > 13000000) {
        return SERIAL_NAND_STATUS_INVALID_PARAMETER;
    }

    if (g_flash_init_flag == true) {
        return SERIAL_NAND_STATUS_DEVBUSY;
    }

    g_spim_port = spim_port;

    spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.clock_frequency = frequency;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    hal_spi_master_init(g_spim_port, &spi_config);
    hal_spi_master_set_macro_selection(g_spim_port, HAL_SPI_MASTER_MACRO_GROUP_C);
    hal_spi_master_register_callback(g_spim_port, flash_dma_callback, NULL);

    g_spi_tx_buffer[0] = SERIAL_NAND_CMD_RESET;
    hal_spi_master_send_polling(g_spim_port, g_spi_tx_buffer, 1);

    serial_nand_get_feature(SERIAL_NAND_STATUS_REG, &status);
    log_hal_info("Flash status register: %02x\r\n", status);
    serial_nand_get_feature(SERIAL_NAND_FEATURE_REG, &status);
    log_hal_info("Flash feature register: %02x\r\n", status);

    /* unlock the flash block */
    serial_nand_get_feature(SERIAL_NAND_PROTEC_REG, &status);
    log_hal_info("Flash Protection register: %02x\r\n", status);
    status &= ~(SERIAL_NAND_PROTEC_BP2 | SERIAL_NAND_PROTEC_BP1 | SERIAL_NAND_PROTEC_BP0);
    serial_nand_set_feature(SERIAL_NAND_PROTEC_REG, status);
    log_hal_info("Flash Protection register: %02x\r\n", status);

    g_flash_init_flag = true;

    return SERIAL_NAND_STATUS_OK;
}

serial_nand_status_t serial_nand_reset_device(void)
{
    if (g_flash_init_flag == true) {
        return SERIAL_NAND_STATUS_DEVBUSY;
    }

    g_spi_tx_buffer[0] = SERIAL_NAND_CMD_RESET;
    hal_spi_master_send_polling(g_spim_port, g_spi_tx_buffer, 1);

    return SERIAL_NAND_STATUS_OK;
}

serial_nand_status_t serial_nand_deinit(void)
{
    if (g_flash_init_flag == false) {
        return SERIAL_NAND_STATUS_ERROR;
    }

    hal_spi_master_deinit(g_spim_port);

    g_flash_init_flag = false;

    return SERIAL_NAND_STATUS_OK;
}

serial_nand_status_t serial_nand_get_information(serial_nand_information_t *information)
{
    uint8_t status, tx_buffer[16], rx_buffer[16];
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    if (information == NULL) {
        return SERIAL_NAND_STATUS_INVALID_PARAMETER;
    }

    if (g_flash_init_flag == false) {
        return SERIAL_NAND_STATUS_UNINITIALIZED;
    }

    serial_nand_get_feature(SERIAL_NAND_STATUS_REG, &status);
    if (status & SERIAL_NAND_STATUS_BUSY) {
        return SERIAL_NAND_STATUS_DEVBUSY;
    }

    tx_buffer[0] = SERIAL_NAND_CMD_RDID;
    tx_buffer[1] = 0x00;
    spi_send_and_receive_config.send_length = 2;
    spi_send_and_receive_config.send_data = tx_buffer;
    spi_send_and_receive_config.receive_length = 2 + spi_send_and_receive_config.send_length;
    spi_send_and_receive_config.receive_buffer = rx_buffer;
    hal_spi_master_send_and_receive_polling(g_spim_port, &spi_send_and_receive_config);
    information->maf_id = rx_buffer[2];
    information->dev_id = rx_buffer[3];
    if ((information->maf_id != 0xC8) || (information->dev_id != 0xC4)) {
        return SERIAL_NAND_STATUS_ERROR;
    }

    information->page_size = SERIAL_NAND_PAGE_SIZE;
    information->spare_size = SERIAL_NAND_SPARE_SIZE;
    information->block_size = SERIAL_NAND_BLOCK_SIZE;
    information->chip_size = SERIAL_NAND_TOTAL_SIZE;

    return SERIAL_NAND_STATUS_OK;
}

static bool parameter_check(uint32_t address, uint8_t *data, uint32_t length,
                            uint32_t spare_offset, uint8_t *spare_data, uint32_t spare_length)
{
    uint64_t temp_address = address;

    /* check whether crossover a page's normal data. */
    if ((temp_address + length) > ((temp_address + SERIAL_NAND_PAGE_SIZE) & (~SERIAL_NAND_PAGE_SIZE_MASK))) {
        return false;
    }

    /* check whether crossover a page's spare area. */
    if ((spare_offset + spare_length) > SERIAL_NAND_SPARE_SIZE) {
        return false;
    }

    /* check whether write nothing. */
    if (((spare_data == NULL) && (data == NULL)) ||
            ((spare_length == 0) && (length == 0)) ||
            ((spare_data == NULL) && (length == 0)) ||
            ((data == NULL) && (spare_length == 0))) {
        return false;
    }

    return true;
}

serial_nand_status_t serial_nand_read_page(uint32_t address, uint8_t *data, uint32_t length,
        uint32_t spare_offset, uint8_t *spare_data, uint32_t spare_length)
{
    uint8_t status;
    uint32_t page_address, page_offset;
    bool read_twice;

    if (parameter_check(address, data, length, spare_offset, spare_data, spare_length) == false) {
        return SERIAL_NAND_STATUS_INVALID_PARAMETER;
    }

    if (g_flash_init_flag == false) {
        return SERIAL_NAND_STATUS_UNINITIALIZED;
    }

    serial_nand_get_feature(SERIAL_NAND_STATUS_REG, &status);
    if (status & SERIAL_NAND_STATUS_BUSY) {
        return SERIAL_NAND_STATUS_DEVBUSY;
    }

    read_twice = false;
    page_address = address & (~SERIAL_NAND_PAGE_SIZE_MASK);
    page_offset = address & SERIAL_NAND_PAGE_SIZE_MASK;

    if (((data != NULL) && (spare_data != NULL)) &&
            ((spare_offset != 0) || ((page_offset + length) < SERIAL_NAND_PAGE_SIZE))) {
        read_twice = true;
    }

    if (read_twice == true) {
        /* read discrete normal data and spare data. */
        //log_hal_info("read discrete normal data and spare data\r\n");
        flash_internal_read(page_address, page_offset, data, length, spare_offset, spare_data, spare_length);
    } else if (data == NULL) {
        /* read only spare data. */
        //log_hal_info("read only spare data\r\n");
        flash_internal_read(page_address, 0, NULL, 0, spare_offset, spare_data, spare_length);
    } else if (spare_data == NULL) {
        /* read only normal data. */
        //log_hal_info("read only normal data\r\n");
        flash_internal_read(page_address, page_offset, data, length, 0, NULL, 0);
    } else {
        /* read continuous normal data and spare data. */
        //log_hal_info("read continuous normal data and spare data\r\n");
        //flash_internal_read(page_address, page_offset, data, length + spare_length, 0, NULL, 0);
        flash_internal_read(page_address, page_offset, data, length, spare_offset, spare_data, spare_length);
    }

    return SERIAL_NAND_STATUS_OK;
}

/* buffers needed to be non-cacheable and 4byte align. */
serial_nand_status_t serial_nand_write_page(uint32_t address, uint8_t *data, uint32_t length,
        uint32_t spare_offset, uint8_t *spare_data, uint32_t spare_length)
{
    uint8_t status;
    uint32_t page_address, page_offset;
    bool write_twice;

    if (parameter_check(address, data, length, spare_offset, spare_data, spare_length) == false) {
        return SERIAL_NAND_STATUS_INVALID_PARAMETER;
    }

    if (g_flash_init_flag == false) {
        return SERIAL_NAND_STATUS_UNINITIALIZED;
    }

    serial_nand_get_feature(SERIAL_NAND_STATUS_REG, &status);
    if (status & SERIAL_NAND_STATUS_BUSY) {
        return SERIAL_NAND_STATUS_DEVBUSY;
    }

    write_twice = false;
    page_address = address & (~SERIAL_NAND_PAGE_SIZE_MASK);
    page_offset = address & SERIAL_NAND_PAGE_SIZE_MASK;

    if (((data != NULL) && (spare_data != NULL)) &&
            ((spare_offset != 0) || ((page_offset + length) < SERIAL_NAND_PAGE_SIZE))) {
        write_twice = true;
    }

    if (write_twice == true) {
        /* write discrete normal data and spare data. */
        //log_hal_info("write discrete normal data and spare data\r\n");
        flash_internal_write(page_address, page_offset, data, length, spare_offset, spare_data, spare_length);
    } else if (data == NULL) {
        /* write only spare data. */
        //log_hal_info("write only spare data\r\n");
        flash_internal_write(page_address, 0, NULL, 0, spare_offset, spare_data, spare_length);
    } else if (spare_data == NULL) {
        /* write only normal data. */
        //log_hal_info("write only normal data\r\n");
        flash_internal_write(page_address, page_offset, data, length, 0, NULL, 0);
    } else {
        /* write continuous normal data and spare data. */
        //log_hal_info("write continuous normal data and spare data\r\n");
        //flash_internal_write(page_address, page_offset, data, length + spare_length, 0, NULL, 0);
        flash_internal_write(page_address, page_offset, data, length, spare_offset, spare_data, spare_length);
    }

    return SERIAL_NAND_STATUS_OK;
}

serial_nand_status_t serial_nand_erase(uint32_t address, uint32_t length)
{
    uint32_t block_address, block_count;
    uint8_t status;
    hal_spi_master_status_t ret;
    //log_hal_info("[ERASE]:address=%x,length=0x%x \r\n", address, length);

    if (((address & SERIAL_NAND_BLOCK_SIZE_MASK) != 0) ||
            ((length & SERIAL_NAND_BLOCK_SIZE_MASK) != 0) ||
            (length == 0)) {
        return SERIAL_NAND_STATUS_INVALID_PARAMETER;
    }

    if (g_flash_init_flag == false) {
        return SERIAL_NAND_STATUS_UNINITIALIZED;
    }

    if (flash_wait_busy(0, NULL) == false) {
        return SERIAL_NAND_STATUS_DEVBUSY;
    }

    /* 06H (WRITE ENBALE command) */
    g_spi_tx_buffer[0] = SERIAL_NAND_CMD_WREN;
    ret = hal_spi_master_send_polling(g_spim_port, g_spi_tx_buffer, 1);
    if (ret != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("WRITE ENBALE command fail\r\n");
        return SERIAL_NAND_STATUS_ERROR;
    }

    block_address = address >> SERIAL_NAND_PAGE_SIZE_SHIFT;
    block_count = length >> SERIAL_NAND_BLOCK_SIZE_SHIFT;
    while (block_count) {
        /* Block Erase (D8H) */
        g_spi_tx_buffer[0] = SERIAL_NAND_CMD_ERASE;
        g_spi_tx_buffer[1] = (block_address >> 16) & 0xff;
        g_spi_tx_buffer[2] = (block_address >> 8) & 0xff;
        g_spi_tx_buffer[3] = block_address & 0xff;
        ret = hal_spi_master_send_polling(g_spim_port, g_spi_tx_buffer, 4);
        if (ret != HAL_SPI_MASTER_STATUS_OK) {
            log_hal_error("Block Erase fail 1\r\n");
            return SERIAL_NAND_STATUS_ERROR;
        }

        /* 0FH (GET FEATURES command to read the status) */
        do {
            serial_nand_get_feature(SERIAL_NAND_STATUS_REG, &status);
            //log_hal_info("status reg: %02x\r\n", status);
            if (status & SERIAL_NAND_STATUS_EFAIL) {
                log_hal_error("Block Erase status fail: %02x  addr: 0x%x \r\n", status, address);
                return SERIAL_NAND_STATUS_ERROR;
            }
        } while (status & SERIAL_NAND_STATUS_BUSY);

        block_address++;
        block_count--;
    }

    /* 04H (Write disable command) */
    g_spi_tx_buffer[0] = SERIAL_NAND_CMD_WRDI;
    ret = hal_spi_master_send_polling(g_spim_port, g_spi_tx_buffer, 1);
    if (ret != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("Write disable command fail\r\n");
        return SERIAL_NAND_STATUS_ERROR;
    }

    return SERIAL_NAND_STATUS_OK;
}

#endif
