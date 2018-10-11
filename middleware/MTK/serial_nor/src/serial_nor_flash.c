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
#ifdef SERIAL_NOR_WITH_SPI_SUPPORT
#if ((PRODUCT_VERSION == 7687)||(PRODUCT_VERSION == 7697))

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* hal includes */
#include "hal.h"
#include "serial_nor_flash.h"
#include "hal_spi_master.h"


#define FLASH_READ_SR (0x5)
#define FLASH_READ_WEN (0x6)
#define FLASH_READ_CMD (0x3)
#define FLASH_WRITE_CMD (0x2)
#define FLASH_CHIP_ERASE (0x60)
#define FLASH_SECTOR_ERASE (0x20)
#define FLASH_BLOCK_32_ERASE (0x52) 
#define FLASH_BLOCK_64_ERASE (0xD8)


int32_t spi_flash_init = 0;
static hal_spi_master_port_t g_spim_port;

int32_t serial_nor_get_status(void)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t cmd[1];
    uint8_t buff[2] = {0, 0};
    hal_spi_master_port_t spi_master_port;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
    spi_master_port = HAL_SPI_MASTER_0;
    
    if (spi_flash_init != FLASH_INIT) {
        return SERIAL_NOR_STATUS_NOT_INIT;
    }

    cmd[0]= FLASH_READ_SR;
    spi_send_and_receive_config.receive_buffer = buff;    
    spi_send_and_receive_config.send_data = cmd;
    spi_send_and_receive_config.send_length = 0x1;
    //should be : command length 3 + send_length
    spi_send_and_receive_config.receive_length = 0x1 + spi_send_and_receive_config.send_length;
    
    status = hal_spi_master_send_and_receive_polling(spi_master_port, &spi_send_and_receive_config); 

    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_info("hal_spi_master_send_polling fail\n");
        return FLASH_BUSY;
    }

    //log_hal_info(" serial_nor_get_status: 0x%x \r\n", buff[1]);  

    if ((buff[1] & FLASH_BUSY) == 1) {
        return FLASH_STATUS_BUSY;
    } else {    
        return FLASH_STATUS_IDLE;
    }
}

/**
 * @brief     wait extenal flash status ready, flash should be in idle status before read, wirte or erase.
 * @param[in]  waiting time
 * @return
 * #FLASH_STATUS_BUSY or FLASH_STATUS_IDLE
 */
static flash_status_t serial_flash_wait_ready(uint32_t sleep_ms)
{
    uint32_t count = 0;
    
    if (spi_flash_init != FLASH_INIT) {
        return FLASH_STATUS_BUSY;
    }
    
    while(1) {   
        if ((serial_nor_get_status() & FLASH_STATUS_BUSY) == 1) {
            hal_gpt_delay_ms(1);
            count++;
            if (count > ((sleep_ms+1) * 1000)) { 
                log_hal_info("\r\n Flash is busy over 100s !!! \r\n");
                return FLASH_STATUS_BUSY;
            }
        } else {
            return FLASH_STATUS_IDLE;
        }
    }
}

serial_nor_status_t serial_nor_get_information(uint8_t *buff)
{
    hal_spi_master_status_t status;
    uint8_t cmd = 0x9F;   ////read flash RDID
    hal_spi_master_port_t spi_master_port;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
    spi_master_port = HAL_SPI_MASTER_0;
    
    if (spi_flash_init != FLASH_INIT) {
        return SERIAL_NOR_STATUS_NOT_INIT;
    }

    if (serial_flash_wait_ready(1)) {
        log_hal_info("\r\n flash is busy before read! \r\n");
        return SERIAL_NOR_STATUS_ERROR;
    }
    
    //log_hal_info("[Flash Status] read flash status start \r\n");
    spi_send_and_receive_config.receive_buffer = buff;    
    spi_send_and_receive_config.send_data = &cmd;
    spi_send_and_receive_config.send_length = 0x1;
    //should be : command length 3 + send_length
    spi_send_and_receive_config.receive_length = 0x3 + spi_send_and_receive_config.send_length;
    
    status = hal_spi_master_send_and_receive_polling(spi_master_port, &spi_send_and_receive_config); 

    printf("\r\n\r\n%02x - %02x - %02x - %02x\r\n\r\n", buff[0], buff[1], buff[2], buff[3]);

    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_info("hal_spi_master_send_polling fail\n");
        return SERIAL_NOR_STATUS_ERROR;
    }

    if (FLASH_STATUS_BUSY == serial_flash_wait_ready(1)) {
        log_hal_info("\r\n flash is busy before read! \r\n");
        return SERIAL_NOR_STATUS_ERROR;
    }
    return SERIAL_NOR_STATUS_OK;
}

serial_nor_status_t serial_nor_init(hal_spi_master_port_t spim_port, uint32_t frequency)
{
    uint8_t reset_cmd[2] = {0x66, 0x99};  /* flash reset command */
    hal_spi_master_config_t spi_config;

/**********************************************************************************************
     Configure GPIO and Pinmux if EPT tool hasn't been used to configure the related pinmux. 
     The next is a example code: 
     Step1: Init GPIO
     hal_gpio_init(HAL_GPIO_7);
     hal_gpio_init(HAL_GPIO_24);
     hal_gpio_init(HAL_GPIO_25);
     hal_gpio_init(HAL_GPIO_26);
 
     Step2: Call hal_pinmux_set_function() to set GPIO pinmux
     hal_pinmux_set_function(HAL_GPIO_24, HAL_GPIO_24_SPI_MOSI_M_CM4);
     hal_pinmux_set_function(HAL_GPIO_25, HAL_GPIO_25_SPI_MISO_M_CM4);
     hal_pinmux_set_function(HAL_GPIO_26, HAL_GPIO_26_SPI_SCK_M_CM4);
     hal_pinmux_set_function(HAL_GPIO_7, HAL_GPIO_7_GPIO7);
***********************************************************************************************/

    g_spim_port = spim_port;
    /* Init SPI master */
    spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.clock_frequency = frequency;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE1;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY1;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_init(spim_port, &spi_config)) {
        log_hal_info("\r\n hal_spi_master_init fail\r\n");
        return SERIAL_NOR_STATUS_ERROR;
    }

    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(spim_port, reset_cmd, 2)) {
        log_hal_info("\r\n hal_spi_master_send_polling fail\r\n");
        return SERIAL_NOR_STATUS_ERROR;
    }

    spi_flash_init = FLASH_INIT;
    return SERIAL_NOR_STATUS_OK;
}

serial_nor_status_t serial_nor_deinit(void)
{
    if (spi_flash_init != FLASH_INIT) {
        return SERIAL_NOR_STATUS_NOT_INIT;
    }
   
    /* De-init spi master */
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_deinit(g_spim_port)) {
        log_hal_info("hal_spi_master_deinit fail\n");
    }
    
    spi_flash_init = FLASH_NOT_INIT;
    return SERIAL_NOR_STATUS_OK;    
}

static serial_nor_status_t nor_flash_read(uint32_t address, uint8_t *buffer, uint32_t length)
{
    uint8_t cmd[4];   
    uint32_t i;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
    hal_spi_master_port_t spi_master_port;
    spi_master_port = HAL_SPI_MASTER_0;
    
    if (spi_flash_init != FLASH_INIT) {
        return SERIAL_NOR_STATUS_NOT_INIT;
    }
    
    i = 0;
    cmd[i++] = FLASH_READ_CMD;
    cmd[i++] = address >> 16;  // 1: 3 byte address
    cmd[i++] = address >> 8;   // 2: 3 byte address
    cmd[i] = address;          // 3: 3 byte address

    spi_send_and_receive_config.receive_buffer = buffer;    
    spi_send_and_receive_config.send_data = cmd;
    spi_send_and_receive_config.send_length = 0x4;
     //should be : command length  + send_length
    spi_send_and_receive_config.receive_length = length + spi_send_and_receive_config.send_length;  

    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(spi_master_port, &spi_send_and_receive_config)) {
        log_hal_info("hal_spi_master_send_polling fail\r\n");
    }

    if (serial_flash_wait_ready(1)) {
        log_hal_info("\r\n flash is busy after read! \r\n");
        return SERIAL_NOR_STATUS_ERROR;
    }

    return SERIAL_NOR_STATUS_OK;
}

serial_nor_status_t serial_nor_read(uint32_t address, uint8_t *buffer, uint32_t length)
{
    serial_nor_status_t status; 
    uint8_t data[FLASH_DADA_MAX_LENGTH + 4];  //256 + 4
    uint32_t read_length = 0;
    uint32_t data_length = length;
        
    if (spi_flash_init != FLASH_INIT) {
        return SERIAL_NOR_STATUS_NOT_INIT;
    }

    if (serial_flash_wait_ready(1)) {
        log_hal_info("\r\n flash is busy before read! \r\n");
        return SERIAL_NOR_STATUS_ERROR;
    }

    while (data_length > FLASH_DADA_MAX_LENGTH) {
        read_length = FLASH_DADA_MAX_LENGTH;       
        nor_flash_read(address, data, read_length);        
        memcpy(buffer, &data[4], read_length);
        buffer = buffer + read_length;
        address = address + read_length;
        data_length = data_length - read_length;
    }

    read_length = data_length;
    status = nor_flash_read(address, data, data_length);

    memcpy(buffer, &data[4], data_length); 
    return status;
}


static serial_nor_status_t nor_flash_write(uint32_t address, uint8_t *data, uint32_t length)
{
    int32_t status = FLASH_NOT_INIT;   
    uint8_t buffer[FLASH_DADA_MAX_LENGTH + 4];
    uint32_t i;
    hal_spi_master_port_t spi_master_port;
    
    spi_master_port = HAL_SPI_MASTER_0;

    if (spi_flash_init != FLASH_INIT) {
        return SERIAL_NOR_STATUS_NOT_INIT;
    }
    
    buffer[0] = FLASH_READ_WEN;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(spi_master_port, buffer, 1)) {
        log_hal_info("\r\n hal_spi_master_send_polling fail\r\n");
    }
    serial_flash_wait_ready(1);
    
    i = 0;
    buffer[i++] = FLASH_WRITE_CMD;
    buffer[i++] = address >> 16;  // 1: 3 byte address
    buffer[i++] = address >> 8;   //2: 3 byte address
    buffer[i++] = address;        //3: 3 byte address

    memcpy(&buffer[i], data, length);
    
    status = hal_spi_master_send_polling(g_spim_port, buffer, (length+4));  // data length + 4 bytes command

    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_info("\r\n hal_spi_master_send_polling fail\r\n");
    }
    
    if (serial_flash_wait_ready(10)) {
        log_hal_info("\r\n flash is busy before write! \r\n");
        return SERIAL_NOR_STATUS_ERROR;
    }
    
    return SERIAL_NOR_STATUS_OK;
}

serial_nor_status_t serial_nor_write(uint32_t address, uint8_t *data, uint32_t length)
{
    serial_nor_status_t status;
    uint8_t *buffer = data;
    uint32_t write_length;
    uint32_t data_length = length;
    
    if (spi_flash_init != FLASH_INIT) {
        return SERIAL_NOR_STATUS_NOT_INIT;
    }

    if (serial_flash_wait_ready(2)) {
        log_hal_info("\r\n flash is busy before write! \r\n");
        return SERIAL_NOR_STATUS_ERROR;
    }

    while (data_length > FLASH_DADA_MAX_LENGTH) {
        write_length = FLASH_DADA_MAX_LENGTH;
        log_hal_info("\r\n write address: 0x%x , len: %d \r\n", address, write_length);      
        nor_flash_write(address, buffer, write_length);
        buffer = buffer + write_length;
        address = address + write_length;
        data_length = data_length - write_length;
    }
    write_length = data_length;
    status = nor_flash_write(address, buffer, data_length);
    return status;
}

serial_nor_status_t serial_nor_erase(uint32_t address, block_size_type_t block_size)
{
    uint8_t cmd[4];
    uint32_t i;
    
    if (spi_flash_init != FLASH_INIT) {
        return SERIAL_NOR_STATUS_NOT_INIT;
    }

    if (serial_flash_wait_ready(2)) {
        log_hal_info("\r\n flash is busy before erase! \r\n");
        return SERIAL_NOR_STATUS_ERROR;
    }
    
    cmd[0] = FLASH_READ_WEN;

    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_spim_port, cmd, 1)) {
        log_hal_info("\r\n hal_spi_master_send_polling fail\r\n");
    }

    serial_flash_wait_ready(1); 
    
    if (block_size == FLASH_SECTOR_ALL) {
        cmd[0] = FLASH_CHIP_ERASE;   // CHIP ERASE  0X60 OR 0XC7  
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_spim_port, cmd, 1)) {
            log_hal_info(" \r\n hal_spi_master_send_polling fail\r\n ");
            return SERIAL_NOR_STATUS_ERROR;
        }
    } else {
        i = 0;
        if (block_size == FLASH_SECTOR_4K) {
            cmd[i++] = FLASH_SECTOR_ERASE;
        } else if (block_size == FLASH_SECTOR_32K) {
            cmd[i++] = FLASH_BLOCK_32_ERASE;
        } else if (block_size == FLASH_SECTOR_64K) {
            cmd[i++] = FLASH_BLOCK_64_ERASE;
        }
        
        cmd[i++] = address>>16;  // 1: 3 byte address
        cmd[i++] = address>>8;   // 2: 3 byte address
        cmd[i] = address;        // 3: 3 byte address

        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_spim_port, cmd, 4)) {
            log_hal_info(" \r\n[Flash Erase] hal_spi_master_send_polling fail\r\n ");
            return SERIAL_NOR_STATUS_ERROR;
        }
    }
    
    //Wait flash erase finished 
    if (serial_flash_wait_ready(5000) == FLASH_STATUS_IDLE) {
        log_hal_info("\r\n[Flash Erase] flash erase is finished \r\n");
        return SERIAL_NOR_STATUS_OK;
    }
    return SERIAL_NOR_STATUS_ERROR;
}
#endif
#endif

