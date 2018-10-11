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

#include "FreeRTOS.h"
#include "vsm_driver.h" 
#include "vsm_i2c_operation.h"
#include "semphr.h"
#include "syslog.h"
#include "mems_bus.h"
#include "memory_attribute.h"


log_create_module(mt2511_i2c_driver, PRINT_LEVEL_INFO);

#define LOGE(fmt,arg...)   LOG_E(mt2511_i2c_driver, "[mt2511]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(mt2511_i2c_driver, "[mt2511]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(mt2511_i2c_driver, "[mt2511]"fmt,##arg)

#define	I2C_BUFFER_LEN 8
 
 //i2c parameter
static hal_i2c_port_t vsm_i2c_work_port;
static hal_i2c_frequency_t vsm_i2c_work_freq;

#ifdef MT2511_BUS_USE_DMA
static TickType_t g_vsm_i2c_timeout_tick = 0;
static xSemaphoreHandle g_vsm_i2c_dma_semaphore = NULL;
#ifndef __ICCARM__
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) vsm_array[I2C_BUFFER_LEN];
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) vsm_reg;
#else
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t  vsm_array[I2C_BUFFER_LEN];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t  vsm_reg;
#endif


void i2c_dma_complete(uint8_t slave_address, hal_i2c_callback_event_t event, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = 0;
    
    if (HAL_I2C_EVENT_SUCCESS == event) {
        hal_i2c_master_deinit(vsm_i2c_work_port);
        xSemaphoreGiveFromISR(g_vsm_i2c_dma_semaphore, &xHigherPriorityTaskWoken);
    }
    if ( xHigherPriorityTaskWoken ) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
    }

}
#endif

void i2c_init(hal_i2c_port_t i2c_port, hal_i2c_frequency_t i2c_freq)
{
	//ToDo:enable I2C function pin? or enable it in peripheral function
	
	vsm_i2c_work_port = i2c_port;
    vsm_i2c_work_freq = i2c_freq;
#ifdef MT2511_BUS_USE_DMA
    g_vsm_i2c_timeout_tick = (TickType_t)(500 / portTICK_PERIOD_MS);
    g_vsm_i2c_dma_semaphore = xSemaphoreCreateBinary();
#endif
}

int32_t vsm_i2c_write(uint8_t slave_addr, uint8_t *data, uint16_t len)
{
    hal_i2c_config_t i2c_init_parameter;
    i2c_init_parameter.frequency = vsm_i2c_work_freq;
    hal_i2c_status_t status;
    

    status = hal_i2c_master_init(vsm_i2c_work_port, &i2c_init_parameter);

    if (status < 0) {
        LOGE("status = %d\r\n", status);
        return status;
    }
    
    #ifdef MT2511_BUS_USE_DMA /*DMA mode*/
    int i = 0;
    if (HAL_I2C_STATUS_OK != hal_i2c_master_register_callback(vsm_i2c_work_port, i2c_dma_complete, NULL)) {
        LOGE("register callback failed.\r\n");
        return HAL_I2C_STATUS_ERROR;
    }

    for (i = 0; i < len; i++) {
        vsm_array[i] = data[i];
    }
    status = hal_i2c_master_send_dma(vsm_i2c_work_port, slave_addr, vsm_array, len);
    if (HAL_I2C_STATUS_OK != status) {
        return VSM_STATUS_ERROR;
    }

    if (pdFALSE == xSemaphoreTake(g_vsm_i2c_dma_semaphore, g_vsm_i2c_timeout_tick)) {
        LOGE("xSemaphoreTake timeout.\r\n");
        hal_i2c_master_deinit(vsm_i2c_work_port);
        return VSM_STATUS_ERROR;
    }
    #else
    status = hal_i2c_master_send_polling(vsm_i2c_work_port, slave_addr, data, len); //send data
    hal_i2c_master_deinit(vsm_i2c_work_port);
    #endif    

    if(status != HAL_I2C_STATUS_OK) {
        LOGE("status = %d, port %d, freq %d\r\n", status, vsm_i2c_work_port, vsm_i2c_work_freq);
        return VSM_STATUS_ERROR;
    } else {
        return VSM_STATUS_OK;
    }
    
}

vsm_status_t vsm_i2c_write_read(uint8_t slave_addr, uint8_t reg, uint8_t *data, uint16_t len)
{
    hal_i2c_config_t i2c_init_parameter;
    i2c_init_parameter.frequency = vsm_i2c_work_freq;
    hal_i2c_status_t ret = HAL_I2C_STATUS_OK;

    ret = hal_i2c_master_init(vsm_i2c_work_port, &i2c_init_parameter);
    if (ret < 0) {
        LOGE("status = %d\r\n", ret);
        return VSM_STATUS_ERROR;
    }
#ifdef HAL_I2C_MASTER_FEATURE_EXTENDED_DMA

    #ifndef MT2511_BUS_USE_DMA /* Polling mode */
    hal_i2c_send_to_receive_config_t config;
    config.slave_address = slave_addr;
    config.send_length = 1;
    config.receive_length = len; 

    config.send_data = &reg;   
    config.receive_buffer = data;
    ret = hal_i2c_master_send_to_receive_polling(vsm_i2c_work_port, &config);
    hal_i2c_master_deinit(vsm_i2c_work_port);
    #else /* DMA mode */
    int i = 0;
    vsm_reg = reg;
    hal_i2c_send_to_receive_config_ex_t config;
    config.slave_address = slave_addr;
    config.send_data = &vsm_reg;
    config.receive_buffer = vsm_array;
    config.send_packet_length = 1;
    config.send_bytes_in_one_packet = 1;
    config.receive_packet_length = 1;
    config.receive_bytes_in_one_packet = len;

    if (HAL_I2C_STATUS_OK != hal_i2c_master_register_callback(vsm_i2c_work_port, i2c_dma_complete, NULL)) {
        LOGE("register callback failed.\r\n");
        return HAL_I2C_STATUS_ERROR;
    }    
    ret = hal_i2c_master_send_to_receive_dma_ex(vsm_i2c_work_port,&config);
    if (ret < 0) {
        LOGE("write read dma failed (%ld).\r\n", ret);
    } else {
        if (pdFALSE == xSemaphoreTake(g_vsm_i2c_dma_semaphore, g_vsm_i2c_timeout_tick)) {
            LOGE("xSemaphoreTake timeout.\r\n");
            ret = HAL_I2C_STATUS_ERROR;
            hal_i2c_master_deinit(vsm_i2c_work_port);
        } else {
            for (i = 0; i < len; i++) {
                data[i] = vsm_array[i];
            }
        }
    }   
    #endif
#endif    
    if (ret == HAL_I2C_STATUS_OK) {
        return VSM_STATUS_OK;
    } else {
        return VSM_STATUS_ERROR;
    }
}

