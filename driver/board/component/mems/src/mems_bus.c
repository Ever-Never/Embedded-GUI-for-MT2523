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

#include <string.h> /* memcpy */
#include "FreeRTOS.h"
#include "semphr.h"
#include "hal_i2c_master.h"
#include "hal_spi_master.h"
#include "memory_attribute.h"

/* syslog */
#define LOGI(fmt,arg...)   LOG_I(sensor, "MEMS_BUS: "fmt,##arg)
#define LOGE(fmt,arg...)   LOG_E(sensor, "MEMS_BUS: "fmt,##arg)
/*printf*/
//#define LOGI(fmt,arg...)   printf("MEMS_BUS:INFO: "fmt,##arg)
//#define LOGE(fmt,arg...)   printf("MEMS_BUS:ERROR: "fmt,##arg)

static TickType_t g_mems_i2c_timeout_tick = 0;
static xSemaphoreHandle g_mems_i2c_dma_semaphore = NULL;
static hal_i2c_port_t i2c_work_port;
static hal_i2c_frequency_t i2c_work_freq;
static int32_t _mems_i2c_dma_status = 0;

static void i2c_dma_complete(uint8_t slave_address, hal_i2c_callback_event_t event, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;;

    _mems_i2c_dma_status = event; /* i2c dma transmission status */
    hal_i2c_master_deinit(i2c_work_port);

    xSemaphoreGiveFromISR(g_mems_i2c_dma_semaphore, &xHigherPriorityTaskWoken);
    if ( xHigherPriorityTaskWoken ) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
    }
}
void mems_i2c_init(hal_i2c_port_t i2c_port, hal_i2c_frequency_t i2c_freq)
{
    i2c_work_port = i2c_port;
    i2c_work_freq = i2c_freq;
    /* timeout set to 5 seconds. The timeout value should be long enough for i2c transmission */
    g_mems_i2c_timeout_tick = (TickType_t)(5000 / portTICK_PERIOD_MS);
    g_mems_i2c_dma_semaphore = xSemaphoreCreateBinary();
}

int32_t mems_i2c_write(uint8_t slave_addr, uint8_t *data, uint16_t len)
{
    hal_i2c_config_t i2c_init_parameter;
    i2c_init_parameter.frequency = i2c_work_freq;
    int32_t ret = 0;

    ret = hal_i2c_master_init(i2c_work_port, &i2c_init_parameter);
    if (ret < 0) {
        LOGE("hal_i2c_master_init failed. (%ld)\r\n", ret);
        return ret;
    }

#if 0 /* DMA mode*/
    ret = hal_i2c_master_register_callback(i2c_work_port, i2c_dma_complete, NULL);
    if (ret < 0) {
        LOGE("register callback failed (%ld).\r\n", ret);
    } else {
        ret = hal_i2c_master_send_dma(i2c_work_port, slave_addr, data, len);
        if (ret < 0) {
            LOGE("send dma failed (%ld).\r\n", ret);
        } else {
            if (pdFALSE == xSemaphoreTake(g_mems_i2c_dma_semaphore, g_mems_i2c_timeout_tick)) {
                LOGE("xSemaphoreTake timeout.\r\n");
                ret = HAL_I2C_STATUS_ERROR;
            }
        }
    }

    if (ret < 0) {
        hal_i2c_master_deinit(i2c_work_port);
    } else {
        if (_mems_i2c_dma_status < 0) {
            LOGE("i2c dma error (%d) \r\n", _mems_i2c_dma_status);
            ret = HAL_I2C_STATUS_ERROR;
        }
    }
#else /* Polling */
    ret = hal_i2c_master_send_polling(i2c_work_port, slave_addr, data, len);
     if (ret < 0) {
        LOGE("send polling failed (%ld).\r\n", ret);
    }
     hal_i2c_master_deinit(i2c_work_port);
#endif

    return ret;
}

int32_t mems_i2c_read(uint8_t slave_addr, uint8_t *data, uint16_t len)
{
    hal_i2c_config_t i2c_init_parameter;
    i2c_init_parameter.frequency = i2c_work_freq;
    int32_t ret = 0;

    ret = hal_i2c_master_init(i2c_work_port, &i2c_init_parameter);
    if (ret < 0) {
        LOGE("hal_i2c_master_init failed. (%ld)\r\n", ret);
        return ret;
    }

#if 1 /* DMA mode*/
    ret = hal_i2c_master_register_callback(i2c_work_port, i2c_dma_complete, NULL);
     if (ret < 0) {
        LOGE("register callback failed (%ld).\r\n", ret);
    } else {
        ret = hal_i2c_master_receive_dma(i2c_work_port, slave_addr, data, len);
        if (ret < 0) {
            LOGE("receive dma failed (%ld).\r\n", ret);
        } else {
            if (pdFALSE == xSemaphoreTake(g_mems_i2c_dma_semaphore, g_mems_i2c_timeout_tick)) {
                LOGE("xSemaphoreTake timeout.\r\n");
                ret = HAL_I2C_STATUS_ERROR;
            }
        }
    }
    if (ret < 0) {
        hal_i2c_master_deinit(i2c_work_port);
    } else {
        if (_mems_i2c_dma_status < 0) {
            LOGE("i2c dma error (%d) \r\n", _mems_i2c_dma_status);
            ret = HAL_I2C_STATUS_ERROR;
        }
    }

#else /* Polling */
    ret = hal_i2c_master_receive_polling(i2c_work_port, slave_addr, data, len);
    if (ret < 0) {
        LOGE("receive polling failed (%ld).\r\n", ret);
    }
    hal_i2c_master_deinit(i2c_work_port);
#endif

    return ret;
}
#ifndef __ICCARM__
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) mems_i2c_send_reg;
#else
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t mems_i2c_send_reg;
#endif

int32_t mems_i2c_write_read(uint8_t slave_addr, uint8_t reg, uint8_t *data, uint16_t len)
{
    hal_i2c_config_t i2c_init_parameter;
    i2c_init_parameter.frequency = i2c_work_freq;
    int32_t ret = 0;

    ret = hal_i2c_master_init(i2c_work_port, &i2c_init_parameter);
    if (ret < 0) {
        LOGE("hal_i2c_master_init failed. (%ld)\r\n", ret);
        return ret;
    }

#ifdef HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE
    mems_i2c_send_reg = reg;
    hal_i2c_send_to_receive_config_t config;

    config.slave_address = slave_addr;
    config.send_data = &mems_i2c_send_reg;
    config.send_length = 1;
    config.receive_buffer = data;
    config.receive_length = len;

  #if 0 /* DMA mode*/
    ret = hal_i2c_master_register_callback(i2c_work_port, i2c_dma_complete, NULL);
    if (ret < 0) {
        LOGE("register callback failed. (%ld)\r\n", ret);
    } else {
        ret = hal_i2c_master_send_to_receive_dma(i2c_work_port, &config);

        if (ret < 0) {
            LOGE("write read dma failed (%ld).\r\n", ret);
        } else {
            if (pdFALSE == xSemaphoreTake(g_mems_i2c_dma_semaphore, g_mems_i2c_timeout_tick)) {
                LOGE("xSemaphoreTake timeout.\r\n");
                ret = HAL_I2C_STATUS_ERROR;
            }
        }
    }
    if (ret < 0) {
        hal_i2c_master_deinit(i2c_work_port);
    } else {
        if (_mems_i2c_dma_status < 0) {
            LOGE("i2c dma error (%d) \r\n", _mems_i2c_dma_status);
            ret = HAL_I2C_STATUS_ERROR;
        }
    }
  #else /* Polling */
    ret = hal_i2c_master_send_to_receive_polling(i2c_work_port, &config);
    if (ret < 0) {
        LOGE("write read polling failed (%ld).\r\n", ret);
    }
    hal_i2c_master_deinit(i2c_work_port);
  #endif
#endif

    return ret;
}

#ifdef HAL_I2C_MASTER_FEATURE_EXTENDED_DMA
int32_t mems_i2c_write_read_dma(uint8_t slave_addr, uint8_t reg, uint8_t *data, uint16_t len)
{
    hal_i2c_config_t i2c_init_parameter;
    i2c_init_parameter.frequency = i2c_work_freq;
    int32_t ret = 0;

    ret = hal_i2c_master_init(i2c_work_port, &i2c_init_parameter);
    if (ret < 0) {
        LOGE("hal_i2c_master_init failed. (%ld)\r\n", ret);
        return ret;
    }

    mems_i2c_send_reg = reg;
    hal_i2c_send_to_receive_config_ex_t config;

    config.slave_address = slave_addr;
    config.send_data = &mems_i2c_send_reg;
    config.receive_buffer = data;
    config.send_packet_length = 1;
    config.send_bytes_in_one_packet = 1;
    config.receive_packet_length = 1;
    config.receive_bytes_in_one_packet = len;

    ret = hal_i2c_master_register_callback(i2c_work_port, i2c_dma_complete, NULL);
    if (ret < 0) {
        LOGE("register callback failed. (%ld)\r\n", ret);
    } else {

        ret = hal_i2c_master_send_to_receive_dma_ex(i2c_work_port, &config);

        if (ret < 0) {
            LOGE("write read dma failed (%ld).\r\n", ret);
        } else {
            if (pdFALSE == xSemaphoreTake(g_mems_i2c_dma_semaphore, g_mems_i2c_timeout_tick)) {
                LOGE("xSemaphoreTake timeout.\r\n");
                ret = HAL_I2C_STATUS_ERROR;
            }
        }
    }

    if (ret < 0) {
        hal_i2c_master_deinit(i2c_work_port);
    } else {
        if (_mems_i2c_dma_status < 0) {
            LOGE("i2c dma error (%d) \r\n", _mems_i2c_dma_status);
            ret = HAL_I2C_STATUS_ERROR;
        }
    }
    return ret;
}
#endif

static hal_spi_master_config_t g_mems_spi_cfg;
static hal_spi_master_port_t g_mems_spi_master_port;
static hal_spi_master_macro_select_t g_mems_spi_master_macro;

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
static xSemaphoreHandle g_mems_spi_dma_semaphore = NULL;
static TickType_t g_mems_spi_timeout_tick = 0;

static void spi_dma_complete(hal_spi_master_callback_event_t event, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if ((HAL_SPI_MASTER_EVENT_SEND_FINISHED == event) || (HAL_SPI_MASTER_EVENT_RECEIVE_FINISHED == event)){
        xSemaphoreGiveFromISR(g_mems_spi_dma_semaphore, &xHigherPriorityTaskWoken);
    }
    if ( xHigherPriorityTaskWoken ) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
    }
}

#define SPI_DMA_NON_CACHEABLE_ACCESS
#ifdef SPI_DMA_NON_CACHEABLE_ACCESS
#ifndef __ICCARM__
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) mems_spi_send_buffer[2];
#else
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t mems_spi_send_buffer[2];
#endif
#endif /*SPI_DMA_NON_CACHEABLE_ACCESS*/
#endif /*HAL_SPI_MASTER_FEATURE_DMA_MODE*/



void mems_spi_init(uint32_t select, hal_spi_master_port_t spi_port, hal_spi_master_config_t *spi_cfg, hal_spi_master_macro_select_t spi_macro)
{
    int32_t ret;

    g_mems_spi_master_port = spi_port;
    g_mems_spi_master_macro = spi_macro;
    memcpy(&g_mems_spi_cfg, spi_cfg, sizeof(hal_spi_master_config_t));
#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
    g_mems_spi_timeout_tick = (TickType_t)(500 / portTICK_PERIOD_MS);
    g_mems_spi_dma_semaphore = xSemaphoreCreateBinary();
#endif

    ret = hal_spi_master_init(g_mems_spi_master_port, &g_mems_spi_cfg);
    if (HAL_SPI_MASTER_STATUS_OK != ret) {
        LOGE("hal_spi_master_init failed (%d).\r\n", ret);
        return;
    }

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
    ret = hal_spi_master_register_callback(g_mems_spi_master_port, spi_dma_complete, NULL);
    if (HAL_SPI_MASTER_STATUS_OK != ret) {
        LOGE("hal_spi_master_register_callback failed (%d).\r\n", ret);
        return;
    }
#endif

#ifdef HAL_SPI_MASTER_FEATURE_MACRO_CONFIG
    ret = hal_spi_master_set_macro_selection(g_mems_spi_master_port, g_mems_spi_master_macro);
    if (HAL_SPI_MASTER_STATUS_OK != ret) {
        LOGE("hal_spi_master_set_macro_selection failed (%d).\r\n", ret);
        return;
    }
#endif

}

int32_t mems_spi_write(uint32_t select, uint8_t *data, uint32_t size)
{
    int32_t ret;

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
    ret = hal_spi_master_send_dma(g_mems_spi_master_port, data, size);
    if (HAL_SPI_MASTER_STATUS_OK != ret) {
        LOGE("hal_spi_master_send_dma failed (%d).\r\n", ret);
        return ret;
    }

    if (pdFALSE == xSemaphoreTake(g_mems_spi_dma_semaphore, g_mems_spi_timeout_tick)) {
        LOGE("xSemaphoreTake timeout.\r\n");
        return HAL_SPI_MASTER_STATUS_ERROR;
    }
#else
    ret = hal_spi_master_send_polling(g_mems_spi_master_port, data, size);
    if (HAL_SPI_MASTER_STATUS_OK != ret) {
        LOGE("hal_spi_master_send_polling failed (%d).\r\n", ret);
        return ret;
    }
#endif

    return ret;
}

int32_t mems_spi_write_read(uint32_t select, uint8_t addr, uint8_t *data, uint32_t size)
{
    int32_t ret;

    hal_spi_master_send_and_receive_config_t cfg;
#ifdef SPI_DMA_NON_CACHEABLE_ACCESS
    mems_spi_send_buffer[0] = addr;
    cfg.send_data = mems_spi_send_buffer;
#else
    cfg.send_data = &addr;
#endif
    cfg.send_length = 1;
    cfg.receive_buffer = data;
    cfg.receive_length = size+1;

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
    ret = hal_spi_master_send_and_receive_dma(g_mems_spi_master_port, &cfg);
    if (HAL_SPI_MASTER_STATUS_OK != ret) {
        LOGE("hal_spi_master_send_and_receive_dma failed (%d).\r\n", ret);
    }

    if (pdFALSE == xSemaphoreTake(g_mems_spi_dma_semaphore, g_mems_spi_timeout_tick)) {
        LOGE("xSemaphoreTake timeout.\r\n");
        return HAL_SPI_MASTER_STATUS_ERROR;
    }
#else
    ret = hal_spi_master_send_and_receive_polling(g_mems_spi_master_port, &cfg);
    if (HAL_SPI_MASTER_STATUS_OK != ret) {
        LOGE("hal_spi_master_send_and_receive_polling failed (%d).\r\n", ret);
    }
#endif

    return ret;
}

