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
#include "spi_external_flash.h"
#include "memory_attribute.h"

#ifndef min
#define min(_a, _b)   (((_a)<(_b))?(_a):(_b))
#endif

#ifndef max
#define max(_a, _b)   (((_a)>(_b))?(_a):(_b))
#endif

hal_spi_master_port_t spi_master_port = HAL_SPI_MASTER_MAX;
volatile static uint32_t dsp_sfc_dma_flag;

/* for test */
uint32_t gpt_count_num = 0;
uint32_t gpt_cur_count = 0;
int32_t spi_flash_init = 0;

ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t cmd_non_cache[4];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static uint8_t non_buffer[FLASH_DADA_MAX_LENGTH + 4];

static uint32_t debug_info_enable = 0;


flash_status_t get_flash_status(void)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *cmd = cmd_non_cache;   //read flash status
    uint8_t buff[2] = {0, 0};

    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
    //spi_master_port = HAL_SPI_MASTER_0;
    
    if (spi_flash_init != FLASH_INIT) {
        return status;
    }

    cmd[0]= 0x5;    //read flash status command 0x5
    //log_hal_info("[Flash Status] read flash status start \r\n");
    spi_send_and_receive_config.receive_buffer = buff;
    spi_send_and_receive_config.send_data = cmd;
    spi_send_and_receive_config.send_length = 0x1;
    spi_send_and_receive_config.receive_length = 0x1 + spi_send_and_receive_config.send_length;
    
    status = hal_spi_master_send_and_receive_polling(spi_master_port, &spi_send_and_receive_config); 

    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_info("hal_spi_master_send_polling fail\n");
        return FLASH_BUSY;
    }

    //log_hal_info(" get_flash_status = 0x%x \r\n", buff[1]);  

    if ((buff[1] & FLASH_BUSY) == 1) {
        return FLASH_STATUS_BUSY;
    } else {    
        return FLASH_STATUS_IDLE;
    }
}


flash_status_t wait_flash_ready(uint32_t ms)
{
    uint32_t count = 0;
    int32_t status = FLASH_NOT_INIT;
    
    if (spi_flash_init != FLASH_INIT) {
        return status;
    }
    
    while(1) {    	   
        if ((get_flash_status() & FLASH_STATUS_BUSY) == 1) {
            hal_gpt_delay_ms(1);
            count++;
            if (count > ((ms+1) * 1000)) { 
                log_hal_info("\r\n Flash is busy over time!!! \r\n");
                return FLASH_STATUS_BUSY;
            }
        } else {
            return FLASH_STATUS_IDLE;
        }
    }
}

int32_t spi_external_flash_get_rdid(uint8_t *buffer)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t cmd = 0x9F;   ////read flash RDID
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    if (spi_flash_init != FLASH_INIT) {
        return status;
    }

    if (wait_flash_ready(1)) {
        log_hal_info("\r\n flash is busy! \r\n");
        return FLASH_STATUS_BUSY;
    }
    
    log_hal_info("[Flash Status] read flash status start \r\n");
    spi_send_and_receive_config.receive_buffer = buffer;    
    spi_send_and_receive_config.send_data = &cmd;
    spi_send_and_receive_config.send_length = 0x1;
    spi_send_and_receive_config.receive_length = 0x3 + spi_send_and_receive_config.send_length;
    
    status = hal_spi_master_send_and_receive_polling(spi_master_port, &spi_send_and_receive_config); 

    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_info("hal_spi_master_send_polling fail\n");
        return HAL_SPI_MASTER_STATUS_ERROR;
    }

    if (FLASH_STATUS_BUSY == wait_flash_ready(1)) {
        log_hal_info("\r\n flash is busy before read! \r\n");
        return SPI_FLASH_STATUS_ERROR;
    }
    return SPI_FLASH_STATUS_OK;
}

/*
    should do initiation before access external flash device 
*/
int32_t spi_external_flash_init(uint32_t frequency)
{
    //flash reset command
    uint8_t reset_cmd[2] = {0x66, 0x99};  
    //for 2523 max is 13Mhz
    uint32_t freq = frequency;
    hal_spi_master_config_t spi_config;
    hal_spi_master_status_t status;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    /* 1, configure the selected SPI port */
    /* 2, initiate GPIO */
    /* 3, configure pinmux */
    /* 4, set SPI master macro */
    
    //configure the right SPI
    spi_master_port = HAL_SPI_MASTER_0;

    hal_gpio_init(HAL_GPIO_4);
    hal_gpio_init(HAL_GPIO_5);
    hal_gpio_init(HAL_GPIO_6);
    hal_gpio_init(HAL_GPIO_7);

    /* Call hal_pinmux_set_function() to set GPIO pinmux*/
    hal_pinmux_set_function(HAL_GPIO_4, HAL_GPIO_4_MA_SPI0_B_CS);
    hal_pinmux_set_function(HAL_GPIO_5, HAL_GPIO_5_MA_SPI0_B_SCK);
    hal_pinmux_set_function(HAL_GPIO_6, HAL_GPIO_6_MA_SPI0_B_MOSI);
    hal_pinmux_set_function(HAL_GPIO_7, HAL_GPIO_7_MA_SPI0_B_MISO);


    spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.clock_frequency = freq;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_init(spi_master_port, &spi_config)) {
        log_hal_info("\r\n hal_spi_master_init fail\r\n");
        return SPI_FLASH_STATUS_ERROR;
    }
    hal_spi_master_set_macro_selection(spi_master_port,HAL_SPI_MASTER_MACRO_GROUP_B);

				hal_gpt_delay_ms(100);
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(spi_master_port, reset_cmd, 2)) {
        log_hal_info("\r\n hal_spi_master_send_polling fail\r\n");
        return SPI_FLASH_STATUS_ERROR;
    }
    
    hal_gpt_delay_ms(1);
    spi_flash_init = FLASH_INIT;
    return SPI_FLASH_STATUS_OK;
}

/*
    should do de-initiation when not use it 
*/
int32_t spi_external_flash_deinit()
{
    int32_t status = FLASH_NOT_INIT;
    if (spi_flash_init != FLASH_INIT) {
        return status;
    }
   
    /* De-init spi master */
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_deinit(spi_master_port)) {
        log_hal_info("hal_spi_master_deinit fail\n");
    }

    hal_gpio_deinit(HAL_GPIO_4);
    hal_gpio_deinit(HAL_GPIO_5);
    hal_gpio_deinit(HAL_GPIO_6);
    hal_gpio_deinit(HAL_GPIO_7);
   
    spi_master_port = HAL_SPI_MASTER_MAX;
    spi_flash_init = FLASH_NOT_INIT;
    return SPI_FLASH_STATUS_OK;
}

static void spi_sfc_dma_complete(void)
{
    dsp_sfc_dma_flag = 1;
}

static int32_t spi_flash_read_polling(uint32_t address, uint8_t *buffer, uint32_t length)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *cmd = cmd_non_cache;
    uint32_t i;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
    
    if (spi_flash_init != FLASH_INIT) {
        return status;
    }
    
    i = 0;
    cmd[i++] = 0x3;
    cmd[i++] = address >> 16;  // 1: 3 byte address
    cmd[i++] = address >> 8;   // 2: 3 byte address
    cmd[i] = address;          // 3: 3 byte address

    hal_gpt_get_free_run_count(0,&gpt_cur_count);
    gpt_count_num = gpt_cur_count;
    
    //log_hal_info("[Flash Read] read flash data start buffer = 0x%x, length = %d\r\n",buffer, length);
    spi_send_and_receive_config.receive_buffer = buffer;    
    spi_send_and_receive_config.send_data = cmd;
    spi_send_and_receive_config.send_length = 0x4;
    spi_send_and_receive_config.receive_length = length + spi_send_and_receive_config.send_length;

    status = hal_spi_master_send_and_receive_polling(spi_master_port, &spi_send_and_receive_config);
    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_info("hal_spi_master_send_polling fail 1234 status = 0x%x \r\n", status);
    }
    //wait flash ready
    if (wait_flash_ready(1)) {
        log_hal_info("\r\n flash is busy after read! \r\n");
        return SPI_FLASH_STATUS_ERROR;
    }

    hal_gpt_get_free_run_count(0,&gpt_cur_count); 
    gpt_count_num = gpt_cur_count - gpt_count_num;
    if (debug_info_enable) {
        log_hal_info("\r\n flash read len: %d ,  tick: %d ! \r\n", length, gpt_count_num);
    }
    gpt_count_num = 0;
    gpt_cur_count = 0;

    return HAL_SPI_MASTER_STATUS_OK;
}

/* 
    buffer should be 4 bytes alignment and non_cacheable
*/
static int32_t spi_flash_read_dma(uint32_t address, uint8_t *buffer, uint32_t length)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *cmd = cmd_non_cache;
    uint32_t i;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    if (length == 0) {
        return status;
    }
    
    if (spi_flash_init != FLASH_INIT) {
        return status;
    }

    i = 0;
    cmd[i++] = 0x3;
    cmd[i++] = address >> 16;  // 1: 3 byte address
    cmd[i++] = address >> 8;   // 2: 3 byte address
    cmd[i] = address;          // 3: 3 byte address

    hal_gpt_get_free_run_count(0,&gpt_cur_count);
    gpt_count_num = gpt_cur_count;
    
    //log_hal_info("[Flash Read] read flash data start buffer = 0x%x, length = %d\r\n",buffer, length);
    spi_send_and_receive_config.receive_buffer = buffer;
    spi_send_and_receive_config.send_data = cmd;
    spi_send_and_receive_config.send_length = 0x4;
    spi_send_and_receive_config.receive_length = length + spi_send_and_receive_config.send_length;

    status = hal_spi_master_send_and_receive_dma(spi_master_port, &spi_send_and_receive_config);
    if (status != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("[dsp_spi_burst_write] fail reason: %d\r\n", status);
        return status;
    }
    
    while (dsp_sfc_dma_flag == 0);
    hal_gpt_delay_ms(5);
    dsp_sfc_dma_flag = 0;

    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_info("hal_spi_master_send_polling fail 1234 status = 0x%x \r\n", status);
    }

    hal_gpt_get_free_run_count(0,&gpt_cur_count); 
    gpt_count_num = gpt_cur_count - gpt_count_num;
     
    if (debug_info_enable) {
    	   log_hal_info("\r\n flash read len: %d ,  tick: %d ! \r\n", length, gpt_count_num);
    }
    
    gpt_count_num = 0;
    gpt_cur_count = 0;
   
    return HAL_SPI_MASTER_STATUS_OK;
}

/* 
    buffer should be 4 bytes alignment and non_cacheable
*/
int32_t spi_external_flash_read(uint32_t address, uint8_t *buffer, uint32_t length)
{
    int32_t status = FLASH_NOT_INIT; 
    //uint8_t data[FLASH_DADA_MAX_LENGTH + 4];  //256 + 4
    uint8_t *data = non_buffer;
    uint32_t read_length = 0;
    uint32_t i;
    uint32_t data_length = length;
        
    if (length == 0) {
        return status;
    }
    
    if (spi_flash_init != FLASH_INIT) {
        return status;
    }

    if (wait_flash_ready(1)) {
        log_hal_info("\r\n flash is busy before read! \r\n");
        return SPI_FLASH_STATUS_ERROR;
    }

    if ((data_length+4) < HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE) {  // should add 4 as the cmd & address
        status = spi_flash_read_polling(address, data, data_length);
        memcpy(buffer, &data[4], data_length);
        return status;
    }

    hal_spi_master_register_callback(spi_master_port, spi_sfc_dma_complete, NULL);

    while (data_length > FLASH_DADA_MAX_LENGTH) {
        read_length = FLASH_DADA_MAX_LENGTH;
        spi_flash_read_dma(address, &data, read_length);
        memcpy(buffer, &data[4], read_length);
        buffer = buffer + read_length;
        address = address + read_length;
        data_length = data_length - read_length;
    }

    //log_hal_info("\r\n read addr: 0x%x  len = %d \r\n", address, data_length); 
    read_length = data_length;
    status = spi_flash_read_dma(address, data, data_length);

    memcpy(buffer, &data[4], data_length); 
    return status;
}


static int32_t spi_flash_write_polling(uint32_t address, uint8_t *data, int32_t length)
{
    int32_t status = FLASH_NOT_INIT;   
    uint8_t *buffer = non_buffer;
    uint32_t i;
    int32_t len = length;
    
    if (spi_flash_init != FLASH_INIT) {
        return status;
    }
    
   	hal_gpt_get_free_run_count(0,&gpt_cur_count);
    gpt_count_num = gpt_cur_count;
    
    buffer[0] = 0x6;   //OPCODE_WREN
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(spi_master_port, buffer, 1)) {
        log_hal_info("\r\n hal_spi_master_send_polling fail\r\n");
    }
    wait_flash_ready(1);
    
    i = 0;
    buffer[i++] = 0x2;
    buffer[i++] = address >> 16;  // 1: 3 byte address
    buffer[i++] = address >> 8;   //2: 3 byte address
    buffer[i++] = address;        //3: 3 byte address

    memcpy(&buffer[i], data, length);

    //if szie is less than 32bytes, it's not neccessory to disable interrupt
    status = hal_spi_master_send_polling(spi_master_port, buffer, (length+4)); 
    
    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_info("\r\n hal_spi_master_send_polling fail\r\n");
    }
    
    if (wait_flash_ready(10)) {
        log_hal_info("\r\n flash is busy before write! \r\n");
        return SPI_FLASH_STATUS_ERROR;
    }
    
    hal_gpt_get_free_run_count(0,&gpt_cur_count); 
    gpt_count_num = gpt_cur_count - gpt_count_num;
    if (debug_info_enable) {
        log_hal_info("\r\n flash write len: %d ,  tick: %d ! \r\n", length, gpt_count_num);
    }
    gpt_count_num = 0;
    gpt_cur_count = 0;
    
    return SPI_FLASH_STATUS_OK;
}

/* 
    buffer should be 4 bytes alignment and non_cacheable
*/
static int32_t spi_flash_write_dma(uint32_t address, uint8_t *data, int32_t length)
{
    int32_t status = FLASH_NOT_INIT;   
    //uint8_t buffer[FLASH_DADA_MAX_LENGTH + 4];
    uint8_t *buffer = non_buffer;
    uint32_t i;

    if (spi_flash_init != FLASH_INIT) {
        return status;
    }
    
   	hal_gpt_get_free_run_count(0,&gpt_cur_count);
    gpt_count_num = gpt_cur_count;
    
    buffer[0] = 0x6;   //OPCODE_WREN
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(spi_master_port, buffer, 1)) {
        log_hal_info("\r\n hal_spi_master_send_polling fail\r\n");
    }
    wait_flash_ready(1);
    
    i = 0;
    buffer[i++] = 0x2;
    buffer[i++] = address >> 16;  // 1: 3 byte address
    buffer[i++] = address >> 8;   //2: 3 byte address
    buffer[i++] = address;        //3: 3 byte address

    memcpy(&buffer[i], data, length);

    status =hal_spi_master_send_dma(spi_master_port, buffer, (length+4));
    if (status != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_error("[dsp_spi_burst_write] fail reason: %d\r\n", status);
        return status;
    }

    while (dsp_sfc_dma_flag == 0);
    hal_gpt_delay_ms(5);
    dsp_sfc_dma_flag = 0;
    
    hal_gpt_get_free_run_count(0,&gpt_cur_count); 
    gpt_count_num = gpt_cur_count - gpt_count_num;
    if (debug_info_enable) {
        log_hal_info("\r\n flash write len: %d ,  tick: %d ! \r\n", length, gpt_count_num);
    }
    gpt_count_num = 0;
    gpt_cur_count = 0;
    
    return SPI_FLASH_STATUS_OK;
}


// data should locates on non-cacheable memory
int32_t spi_external_flash_write(uint32_t address, uint8_t *data, int32_t length)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *buffer = data;
    uint32_t write_length;
    uint32_t data_length = length;
    uint32_t page_offset;
    int32_t len = length;
    uint32_t addr = address;
    
    if (spi_flash_init != FLASH_INIT) {
        return status;
    }

    if (wait_flash_ready(2)) {
        log_hal_info("\r\n flash is busy before write! \r\n");
        return SPI_FLASH_STATUS_ERROR;
    }

    hal_spi_master_register_callback(spi_master_port, spi_sfc_dma_complete, NULL);

    page_offset = address % FLASH_DADA_MAX_LENGTH;

    while (len > 0) {
        write_length = min(len, (FLASH_DADA_MAX_LENGTH - page_offset));
        page_offset = 0;
        
        if ((write_length + 4) < HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE) {
            status = spi_flash_write_polling(addr, buffer, write_length);
            if (status != SPI_FLASH_STATUS_OK) {
                return status;
            }
        } else {    
            status = spi_flash_write_dma(addr, buffer, write_length);
            if (status != SPI_FLASH_STATUS_OK) {
                return status;
            } 
        }
        buffer += write_length;
        addr += write_length;
        len -= write_length; 
    	}
            
    return status;
}

int32_t spi_external_flash_erase(uint32_t address, block_size_type_t block_size)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *cmd = cmd_non_cache;
    uint32_t i;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
        
    if (spi_flash_init != FLASH_INIT) {
        return status;
    }

    if (wait_flash_ready(2)) {
        log_hal_info("\r\n flash is busy before erase! \r\n");
        return SPI_FLASH_STATUS_ERROR;
    }

    hal_gpt_get_free_run_count(0,&gpt_cur_count);
    gpt_count_num = gpt_cur_count;
    
    cmd[0] = 0x6;
    log_hal_info("[Flash Erase] Erase flash device start \r\n");
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(spi_master_port, cmd, 1)) {
        log_hal_info("\r\n hal_spi_master_send_polling fail\r\n");
    }

    wait_flash_ready(1); 
    
    if (block_size == FLASH_SECTOR_ALL) {
        cmd[0] = 0x60;   // CHIP ERASE  0X60 OR 0XC7  
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(spi_master_port, cmd, 1)) {
            log_hal_info(" \r\n hal_spi_master_send_polling fail\r\n ");
            return SPI_FLASH_STATUS_ERROR;
        }
    } else {
        i = 0;
        if (block_size == FLASH_SECTOR_4K) {
            cmd[i++] = 0x20;   // SECTOR 4K ERASE
        } else if (block_size == FLASH_SECTOR_32K) {
            cmd[i++] = 0x52;   // BLOCK 32K ERASE
        } else if (block_size == FLASH_SECTOR_64K) {
            cmd[i++] = 0xD8;   // BLOCK 64K ERASE 
        }
        
        cmd[i++] = address>>16;  // 1: 3 byte address
        cmd[i++] = address>>8;   // 2: 3 byte address
        cmd[i] = address;        // 3: 3 byte address
        //log_hal_info("\r\n flash erase address is : 0x%x! \r\n", address);
        status = hal_spi_master_send_polling(spi_master_port, cmd, 4);
        if (HAL_SPI_MASTER_STATUS_OK != status) {
            log_hal_info(" \r\n[Flash Erase] hal_spi_master_send_polling fail\r\n ");
            return SPI_FLASH_STATUS_ERROR;
        }
    }

    hal_gpt_get_free_run_count(0,&gpt_cur_count); 
    gpt_count_num = gpt_cur_count - gpt_count_num;
    if (debug_info_enable) {
        log_hal_info("\r\n flash erase tick: %d ! \r\n", gpt_count_num);
    }
    gpt_count_num = 0;
    gpt_cur_count = 0;
    
    //Wait flash erase finished
    if (wait_flash_ready(1000) == FLASH_STATUS_IDLE) {
        log_hal_info("\r\n[Flash Erase] flash erase is finished \r\n");
        return SPI_FLASH_STATUS_OK;
    }

    return SPI_FLASH_STATUS_ERROR;    
}
