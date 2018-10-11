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
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "vsm_driver.h" 
#include "vsm_spi_operation.h"
#include "semphr.h"
#include "syslog.h"
#include "memory_attribute.h"

log_create_module(mt2511_spi_driver, PRINT_LEVEL_INFO);

//#define DEBUG_VSM_SPI
#ifdef DEBUG_VSM_SPI
#define LOGD(fmt,arg...)   LOG_I(mt2511_spi_driver, "[mt2511_spi]"fmt,##arg)
//#define LOGD(fmt,arg...)   printf("[mt2511_spi]"fmt"\r\n",##arg)
#else
#define LOGD(fmt,arg...)
#endif

#if 1
#define LOGE(fmt,arg...)   LOG_E(mt2511_spi_driver, "[mt2511_spi]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(mt2511_spi_driver, "[mt2511_spi]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(mt2511_spi_driver, "[mt2511_spi]"fmt,##arg)
#else
#define LOGE(fmt,arg...)   printf("[mt2511_spi]"fmt"\r\n",##arg)
#define LOGW(fmt,arg...)   printf("[mt2511_spi]"fmt"\r\n",##arg)
#define LOGI(fmt,arg...)   printf("[mt2511_spi]"fmt"\r\n",##arg)
#endif

#define	SPI_BUFFER_LEN 8
#define SPI_CHIP_SELECT_VSM 0
/*SPI protocol for mt2511*/
#define SPI_CR_CMD  0x02 //Config Read
#define SPI_CW_CMD  0x04 //Config Write
#define SPI_WD_CMD  0x06 //Write Data
#define SPI_RD_CMD  0x81 //Read Data
#define SPI_RS_CMD  0x0A //Read Status
#define SPI_WS_CMD  0x08 //Write Status
/*2523 spi power on/off cmd*/
#define	SPI_PWRO_CMD    0x0E // Power on
#define SPI_PWRF_CMD    0x0C // Power Off
#define SPI_ERR_MASK (SPISLV_STATUS_RD_ERR_MASK | SPISLV_STATUS_WR_ERR_MASK | SPISLV_STATUS_TIMOUT_ERR_MASK)

#define FAST_SPI

#define SENSOR_SPP_DATA_MAGIC       54321
#define SENSOR_SPP_DATA_RESERVED    12345
 
#ifndef __ICCARM__
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) vsm_array[SPI_BUFFER_LEN];
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) vsm_reg;
#else
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t  vsm_array[SPI_BUFFER_LEN];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t  vsm_reg;
#endif

static hal_spi_master_config_t g_vsm_spi_cfg;
static hal_spi_master_port_t g_vsm_spi_master_port;
static bool spi_init = false;

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
static xSemaphoreHandle g_vsm_spi_dma_semaphore = NULL;
#if 0
static TickType_t g_vsm_spi_timeout_tick = 0;
#endif

extern int send_sensor_data_via_btspp(int32_t magic, int32_t sensor_type, int32_t x, int32_t y, int32_t z, int32_t status, int32_t time_stamp);

static void vsm_spi_dma_complete(hal_spi_master_callback_event_t event, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = 0;

    if ((HAL_SPI_MASTER_EVENT_SEND_FINISHED == event) || (HAL_SPI_MASTER_EVENT_RECEIVE_FINISHED == event)){
        xSemaphoreGiveFromISR(g_vsm_spi_dma_semaphore, &xHigherPriorityTaskWoken);
    }
    if ( xHigherPriorityTaskWoken ) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
    }
}

#define SPI_DMA_NON_CACHEABLE_ACCESS
#ifdef SPI_DMA_NON_CACHEABLE_ACCESS
#ifndef __ICCARM__
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) vsm_spi_send_buffer[SPI_BUFFER_LEN];
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) vsm_spi_receive_buffer[SPI_BUFFER_LEN];
#else
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t vsm_spi_send_buffer[SPI_BUFFER_LEN];
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t vsm_spi_receive_buffer[SPI_BUFFER_LEN];
#endif
#endif /*SPI_DMA_NON_CACHEABLE_ACCESS*/
#endif /*HAL_SPI_MASTER_FEATURE_DMA_MODE*/

void vsm_spi_speed_modify(int32_t speed)
{
    g_vsm_spi_cfg.clock_frequency = speed;
}

void vsm_spi_init(hal_spi_master_port_t spi_port, uint32_t spi_macro)
{
    int32_t ret;

    g_vsm_spi_cfg.clock_frequency = 1875000;
    g_vsm_spi_cfg.slave_port = HAL_SPI_MASTER_SLAVE_0;
    g_vsm_spi_cfg.bit_order = HAL_SPI_MASTER_LSB_FIRST;
    g_vsm_spi_cfg.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    g_vsm_spi_cfg.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
    g_vsm_spi_master_port = spi_port;
#if 0
    g_vsm_spi_timeout_tick = (TickType_t)(1000 / portTICK_PERIOD_MS);
    if (!g_vsm_spi_dma_semaphore) {
        g_vsm_spi_dma_semaphore = xSemaphoreCreateBinary();
    }
#endif

#ifdef FAST_SPI
    if (!spi_init) {
        spi_init = true;
        ret = hal_spi_master_init(g_vsm_spi_master_port, &g_vsm_spi_cfg);
        if (HAL_SPI_MASTER_STATUS_OK != ret) {
            LOGE("hal_spi_master_init failed (%d).\r\n", ret);
            return;
        }
    }
#endif

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
    ret = hal_spi_master_register_callback(g_vsm_spi_master_port, vsm_spi_dma_complete, NULL);
    if (HAL_SPI_MASTER_STATUS_OK != ret) {
        LOGE("hal_spi_master_register_callback failed (%d).\r\n", ret);
        return;
    }
#endif


}

int32_t vsm_spi_write(uint8_t addr, uint8_t reg, uint8_t *data, uint32_t size)
{
    int32_t ret = -1, i = 0;
    int32_t write_success = 0;
    uint8_t status_receive[2] = {0};
    uint8_t poweron_cmd = 0x0e;
    uint8_t read_status_cmd = SPI_RS_CMD;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
    hal_spi_master_chip_select_timing_t chip_select_timing;
    #ifndef FAST_SPI
        ret = hal_spi_master_init(g_vsm_spi_master_port, &g_vsm_spi_cfg);
        if (HAL_SPI_MASTER_STATUS_OK != ret) {
            LOGE("hal_spi_master_init failed (%d).\r\n", ret);
            return ret;
        }
    #endif
    chip_select_timing.chip_select_hold_count = 15;
    chip_select_timing.chip_select_idle_count = 15;
    chip_select_timing.chip_select_setup_count = 15;
    hal_spi_master_set_chip_select_timing(g_vsm_spi_master_port, chip_select_timing);

    /*step 1. Power on 2523 spi*/
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, &poweron_cmd, 1)) {
        LOGE("[spi write]SPI master send polling failed \r\n");
    } else {
        LOGD("[spi write]SPI master is sending data \r\n");
        /*step 2. Config Write*/
        vsm_spi_send_buffer[0] = SPI_CW_CMD;
        vsm_spi_send_buffer[1] = reg;
        vsm_spi_send_buffer[2] = addr;
        vsm_spi_send_buffer[3] = (size & 0xFF) - 1;
        vsm_spi_send_buffer[4] = (size & 0xFF00)>>8;        
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, vsm_spi_send_buffer, 5)) {
            LOGE("[spi write]SPI master send_and_receive polling failed \r\n");
        } else {
            LOGD("[spi write]SPI master CW ok, addr 0x%x, reg 0x%x \r\n",addr, reg);
        }

        /*Step 3. Read Status*/
        status_receive[1] = 0;
        spi_send_and_receive_config.receive_length = 2;
        spi_send_and_receive_config.send_length = 1;
        spi_send_and_receive_config.send_data = &read_status_cmd;
        spi_send_and_receive_config.receive_buffer = status_receive;
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config)) {
            LOGE("SPI master send_and_receive polling failed \r\n");
        } else {
            LOGD("status after CW, status_receive[0] 0x%x\r\n", status_receive[0]);
            if (status_receive[0] == (SPISLV_STATUS_CFG_SUCCESS_MASK)) {
                /*step 4. Start Data transmission*/
                vsm_array[0] = SPI_WD_CMD;
                for (i = 0; i < size; i ++) {
                    vsm_array[1+i] = data[i];
                }
                ret = hal_spi_master_send_polling(g_vsm_spi_master_port, vsm_array, size + 1);
                if (HAL_SPI_MASTER_STATUS_OK != ret) {
                    LOGE("[spi read]hal_spi_master_send_and_receive_polling failed (%d).\r\n", ret);
                } else {
                    /*Step 5. Check Status*/
                    status_receive[1] = 0;
                    spi_send_and_receive_config.receive_length = 2;
                    spi_send_and_receive_config.send_length = 1;
                    spi_send_and_receive_config.send_data = &read_status_cmd;
                    spi_send_and_receive_config.receive_buffer = status_receive;
                    
                    for (i = 0; i < 100; i ++) {
                        ret = hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config);
                        if (ret == HAL_SPI_MASTER_STATUS_OK && status_receive[0] == 0x22) {
                            break;
                        } else {
                            LOGD("SPI master send_and_receive polling failed at 0x%lx%lx, status 0x%lx\r\n", 
                                addr, reg, status_receive[0]);
                            ret = hal_spi_master_send_polling(g_vsm_spi_master_port, vsm_array, size + 1);
                        }
                    }

                    if (HAL_SPI_MASTER_STATUS_OK != ret) {
                        LOGE("SPI master send_and_receive polling failed \r\n");
                    } else {                        
                        LOGD("after WD, status_receive[0] 0x%x at addr 0x%lx%lx\r\n", status_receive[0], addr,reg);
                        if ((status_receive[0] & SPI_ERR_MASK) || status_receive[0] != 0x22) {
                            send_sensor_data_via_btspp(
                                SENSOR_SPP_DATA_MAGIC,
                                990,
                                status_receive[0],
                                0,
                                0,
                                0,
                                SENSOR_SPP_DATA_RESERVED);
                            /*Write Status to clear*/
                            vsm_spi_send_buffer[0] = SPI_WS_CMD;
                            vsm_spi_send_buffer[1] = (status_receive[0] & SPISLV_STATUS_CMD_ERR_MASK) | (status_receive[0] & SPISLV_STATUS_TIMOUT_ERR_MASK) |
                                        (status_receive[0] & SPISLV_STATUS_WR_ERR_MASK) | (status_receive[0] & SPISLV_STATUS_RD_ERR_MASK);
                            if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, vsm_spi_send_buffer, 2)) {
                                LOGE("[spi write]SPI master send_and_receive polling failed \r\n");
                            } else {
                                LOGD("[spi write]SPI master WS after WD ok\r\n");
                            }
                        } else {
                            write_success = 1;
                        }
                    }
                }
            } else {
                send_sensor_data_via_btspp(
                        SENSOR_SPP_DATA_MAGIC,
                        990,
                        status_receive[0],
                        0,
                        0,
                        0,
                        SENSOR_SPP_DATA_RESERVED);
                LOGE("vsm_spi_write status err 0x%x\r\n", status_receive[0]);
                /*Write Status to clear*/
                vsm_spi_send_buffer[0] = SPI_WS_CMD;
                vsm_spi_send_buffer[1] = (status_receive[0] & SPISLV_STATUS_CMD_ERR_MASK) | (status_receive[0] & SPISLV_STATUS_TIMOUT_ERR_MASK) |
                                        (status_receive[0] & SPISLV_STATUS_WR_ERR_MASK) | (status_receive[0] & SPISLV_STATUS_RD_ERR_MASK);
                if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, vsm_spi_send_buffer, 2)) {
                    LOGE("[spi write]SPI master send_and_receive polling failed \r\n");
                } else {
                    LOGD("[spi write]SPI master WS ok\r\n");
                    status_receive[1] = 0;
                    spi_send_and_receive_config.receive_length = 2;
                    spi_send_and_receive_config.send_length = 1;
                    spi_send_and_receive_config.send_data = &read_status_cmd;
                    spi_send_and_receive_config.receive_buffer = status_receive;
                    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config)) {
                        LOGD("SPI master send_and_receive polling failed \r\n");
                    } else {
                        LOGD("after write status status_receive[0] 0x%x, status_receive[1] 0x%x\r\n", status_receive[0], status_receive[1]);
                        LOGD("(status_receive[0] & SPI_ERR_MASK) 0x%x\r\n", (status_receive[0] & SPI_ERR_MASK));
                    }
                }
            }
        }
    }
    #ifndef FAST_SPI
    hal_spi_master_deinit(g_vsm_spi_master_port);
    #endif

    if (write_success) {
        ret = VSM_STATUS_OK;
    } else {
        ret = VSM_STATUS_ERROR;
    }

    return ret;
}

#ifndef FAST_SPI
int32_t vsm_spi_write_read(uint8_t addr, uint8_t reg, uint8_t *data, uint32_t size)
{
    int32_t ret, i = 0;
    int32_t read_success = 0;
    uint8_t read_status_cmd = SPI_RS_CMD;
    uint8_t read_data_cmd = SPI_RD_CMD;
    //uint8_t write_status_cmd = SPI_WS_CMD;
    uint8_t poweron_cmd = 0x0e;
    uint8_t status_receive[2] = {0};
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
    hal_spi_master_chip_select_timing_t chip_select_timing;

    ret = hal_spi_master_init(g_vsm_spi_master_port, &g_vsm_spi_cfg);
    if (HAL_SPI_MASTER_STATUS_OK != ret) {
        LOGE("hal_spi_master_init failed (%d).\r\n", ret);
        return -1;
    }
    
    chip_select_timing.chip_select_hold_count = 25;
    chip_select_timing.chip_select_idle_count = 25;
    chip_select_timing.chip_select_setup_count = 25;
    hal_spi_master_set_chip_select_timing(g_vsm_spi_master_port, chip_select_timing);

    /*step 1. Power on 2523 spi*/
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, &poweron_cmd, 1)) {
        LOGE("[spi read]SPI master send polling failed \r\n");
    } else {
        LOGD("[spi read]SPI master is sending data \r\n");
        /*step 2. Config Read*/
        vsm_spi_send_buffer[0] = SPI_CR_CMD;
        vsm_spi_send_buffer[1] = reg;
        vsm_spi_send_buffer[2] = addr;
        vsm_spi_send_buffer[3] = (size & 0xFF) - 1;
        vsm_spi_send_buffer[4] = (size & 0xFF00)>>8;        
        //if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config)) {
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, vsm_spi_send_buffer, 5)) {
            LOGE("[spi read]SPI master send_and_receive polling failed \r\n");
        }

        /*Step 3. Read Status*/
        status_receive[1] = 0;
        spi_send_and_receive_config.receive_length = 2;
        spi_send_and_receive_config.send_length = 1;
        spi_send_and_receive_config.send_data = &read_status_cmd;
        spi_send_and_receive_config.receive_buffer = status_receive;
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config)) {
            LOGE("[spi read]SPI master send_and_receive polling failed \r\n");
        } else {
            LOGD("status after config read,status_receive[1] 0x%x\r\n", status_receive[1]);
            //if (status_receive[0] == ( SPISLV_STATUS_CFG_SUCCESS_MASK | SPISLV_STATUS_TXRX_FIFO_RDY_MASK)) {
            if (status_receive[0] & SPISLV_STATUS_CFG_SUCCESS_MASK) {
                /*step 4. Start Data transmission*/
                vsm_spi_send_buffer[0] = read_data_cmd;
                spi_send_and_receive_config.send_data = vsm_spi_send_buffer;
                //spi_send_and_receive_config.send_data = &read_data_cmd;
                spi_send_and_receive_config.send_length = 1;
                //spi_send_and_receive_config.receive_buffer = data;
                spi_send_and_receive_config.receive_buffer = vsm_spi_send_buffer;
                spi_send_and_receive_config.receive_length = size+1;    
#if 0
        //#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
                ret = hal_spi_master_send_and_receive_dma(g_vsm_spi_master_port, &spi_send_and_receive_config);
                if (HAL_SPI_MASTER_STATUS_OK != ret) {
                    LOGE("hal_spi_master_send_and_receive_dma failed (%d).\r\n", ret);
                }

                if (pdFALSE == xSemaphoreTake(g_vsm_spi_dma_semaphore, g_vsm_spi_timeout_tick)) {
                    LOGE("xSemaphoreTake timeout.\r\n");
                    return HAL_SPI_MASTER_STATUS_ERROR;
                }
#else
                ret = hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config);
#endif
                if (HAL_SPI_MASTER_STATUS_OK != ret) {
                    LOGE("[spi read]hal_spi_master_send_and_receive_polling failed (%d).\r\n", ret);
                } else {
                    for (i = 0; i < size; i ++) {
                        if (i < 4) {
                            data[i] = vsm_spi_send_buffer[1+i];
                            LOGD("data[%d]:0x%x\r\n", i, data[i]);
                        }
                    }
                    /*Step 5. Check Status*/
                    status_receive[1] = 0;
                    spi_send_and_receive_config.receive_length = 2;
                    spi_send_and_receive_config.send_length = 1;
                    spi_send_and_receive_config.send_data = &read_status_cmd;
                    spi_send_and_receive_config.receive_buffer = status_receive;
                    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config)) {
                        LOGE("SPI master send_and_receive polling failed \r\n");
                    } else {
                        LOGD("after read data status_receive[0] 0x%x status_receive[1] 0x%x\r\n", status_receive[0], status_receive[1]);
                        if (status_receive[0] & SPI_ERR_MASK) {
                            /*Write Status to clear*/
                            vsm_spi_send_buffer[0] = SPI_WS_CMD;
                            vsm_spi_send_buffer[1] = 0;
                            if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, vsm_spi_send_buffer, 2)) {
                                LOGE("[spi write]SPI master send_and_receive polling failed \r\n");
                            } else {
                                LOGD("[spi write]SPI master WS ok\r\n");
                                status_receive[1] = 0;
                                spi_send_and_receive_config.receive_length = 2;
                                spi_send_and_receive_config.send_length = 1;
                                spi_send_and_receive_config.send_data = &read_status_cmd;
                                spi_send_and_receive_config.receive_buffer = status_receive;
                                if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config)) {
                                    LOGD("SPI master send_and_receive polling failed \r\n");
                                } else {
                                    LOGD("after write status status_receive[0] 0x%x, status_receive[1] 0x%x\r\n", status_receive[0], status_receive[1]);
                                    LOGD("(status_receive[0] & SPI_ERR_MASK) 0x%x\r\n", (status_receive[0] & SPI_ERR_MASK));
                                }
                            }
                        } else {
                            read_success = 1;
                        }
                    }
                }
            } else {
                /*Write Status to clear*/
                vsm_spi_send_buffer[0] = SPI_WS_CMD;
                vsm_spi_send_buffer[1] = 0;
                if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, vsm_spi_send_buffer, 2)) {
                    LOGE("[spi write]SPI master send_and_receive polling failed \r\n");
                } else {
                    LOGD("[spi write]SPI master WS ok\r\n");
                    status_receive[1] = 0;
                    spi_send_and_receive_config.receive_length = 2;
                    spi_send_and_receive_config.send_length = 1;
                    spi_send_and_receive_config.send_data = &read_status_cmd;
                    spi_send_and_receive_config.receive_buffer = status_receive;
                    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config)) {
                        LOGD("SPI master send_and_receive polling failed \r\n");
                    } else {
                        LOGD("after write status status_receive[0] 0x%x, status_receive[1] 0x%x\r\n", status_receive[0], status_receive[1]);
                        LOGD("(status_receive[0] & SPI_ERR_MASK) 0x%x\r\n", (status_receive[0] & SPI_ERR_MASK));
                    }
                }
            }
        }
    }
    
    hal_spi_master_deinit(g_vsm_spi_master_port);

    if (!read_success) {
        ret = VSM_STATUS_OK;
    }
    return ret;
}
#else

int32_t vsm_spi_write_read(uint8_t addr, uint8_t reg, uint8_t *data, uint32_t size)
{
    int32_t ret = 0, i = 0;
    uint8_t read_data_cmd = SPI_RD_CMD;
    uint8_t status_receive[2] = {0};
    uint8_t read_status_cmd = SPI_RS_CMD;
    
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    /*step 1. Config Read*/
    vsm_spi_send_buffer[0] = SPI_CR_CMD;
    vsm_spi_send_buffer[1] = reg;
    vsm_spi_send_buffer[2] = addr;
    vsm_spi_send_buffer[3] = (size & 0xFF) - 1;
    vsm_spi_send_buffer[4] = (size & 0xFF00)>>8;    

    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, vsm_spi_send_buffer, 5)) {
        LOGE("[spi read]SPI master send_and_receive polling failed,port %d \r\n", g_vsm_spi_master_port);
    }

    
    
    /*step 2. Start Data transmission*/
    vsm_spi_send_buffer[0] = read_data_cmd;
    spi_send_and_receive_config.send_data = vsm_spi_send_buffer;
    spi_send_and_receive_config.send_length = 1;
    spi_send_and_receive_config.receive_buffer = vsm_spi_receive_buffer;
    spi_send_and_receive_config.receive_length = size+1;
    //#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
#if 0
    ret = hal_spi_master_send_and_receive_dma(g_vsm_spi_master_port, &spi_send_and_receive_config);
    if (HAL_SPI_MASTER_STATUS_OK != ret) {
        LOGE("hal_spi_master_send_and_receive_dma failed (%d).\r\n", ret);
    }
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &t3);
    if (pdFALSE == xSemaphoreTake(g_vsm_spi_dma_semaphore, g_vsm_spi_timeout_tick)) {
        LOGE("xSemaphoreTake timeout.\r\n");
        return HAL_SPI_MASTER_STATUS_ERROR;
    }
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &t4);
#else
    ret = hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config);
#endif

    for (i = 0; i < size; i ++) {
        if (i < 4) {
            data[i] = vsm_spi_receive_buffer[1+i];
        }
    }
    //Step 3. Read Status,if data == 0, make sure it's right status
    if (*((uint32_t *)(data)) == 0) {
        
        status_receive[1] = 0;
        spi_send_and_receive_config.receive_length = 2;
        spi_send_and_receive_config.send_length = 1;
        spi_send_and_receive_config.send_data = &read_status_cmd;
        spi_send_and_receive_config.receive_buffer = status_receive;
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config)) {
            LOGE("[spi read]SPI master send_and_receive polling failed \r\n");
            ret = -1;
        } else {
                LOGD("[spi read]err satus 0x%x in addr 0x%x, reg 0x%x\r\n",status_receive[0], addr, reg);
                if (status_receive[0] & SPISLV_STATUS_CMD_ERR_MASK || status_receive[0] & SPISLV_STATUS_TIMOUT_ERR_MASK ||
                    status_receive[0] & SPISLV_STATUS_WR_ERR_MASK || status_receive[0] & SPISLV_STATUS_RD_ERR_MASK) {
                    send_sensor_data_via_btspp(
                        SENSOR_SPP_DATA_MAGIC,
                        990,
                        status_receive[0],
                        0,
                        0,
                        0,
                        SENSOR_SPP_DATA_RESERVED);
                    LOGE("vsm_spi_write_read status err 0x%x\r\n", status_receive[0]);
                    ret = -1;
                } else {
                    ret = 0;
                }
        }
    }

    if (!ret) {
        ret = VSM_STATUS_OK;
    } else {
        ret = VSM_STATUS_ERROR;
    }
    return ret;
}

int32_t vsm_spi_write_read_retry(uint8_t addr, uint8_t reg, uint8_t *data, uint32_t size)
{
    int32_t ret = -1, i = 0;
    uint8_t read_data_cmd = SPI_RD_CMD;
    uint8_t status_receive[2] = {0};
    uint8_t read_status_cmd = SPI_RS_CMD;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    /*Step 1. Read Status*/
    status_receive[1] = 0;
    spi_send_and_receive_config.receive_length = 2;
    spi_send_and_receive_config.send_length = 1;
    spi_send_and_receive_config.send_data = &read_status_cmd;
    spi_send_and_receive_config.receive_buffer = status_receive;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config)) {
        LOGE("[spi read]SPI master send_and_receive polling failed \r\n");
    } else {
        //LOGE("[spi read]status_receive[0] 0x%x\r\n",status_receive[0]);
        /*Step 2. Clear Status*/
        if (status_receive[0] & SPISLV_STATUS_CMD_ERR_MASK || status_receive[0] & SPISLV_STATUS_TIMOUT_ERR_MASK ||
            status_receive[0] & SPISLV_STATUS_WR_ERR_MASK || status_receive[0] & SPISLV_STATUS_RD_ERR_MASK) {
            LOGE("[spi write]status_receive[0] 0x%x,addr 0x%x, reg 0x%x\r\n",status_receive[0],addr,reg);
            /*Write Status to clear*/
            vsm_spi_send_buffer[0] = SPI_WS_CMD;
            vsm_spi_send_buffer[1] = (status_receive[0] & SPISLV_STATUS_CMD_ERR_MASK) | (status_receive[0] & SPISLV_STATUS_TIMOUT_ERR_MASK) |
                                     (status_receive[0] & SPISLV_STATUS_WR_ERR_MASK) | (status_receive[0] & SPISLV_STATUS_RD_ERR_MASK);
            LOGE("[spi write]WS data 0x%x \r\n",vsm_spi_send_buffer[1]);
            if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, vsm_spi_send_buffer, 2)) {
                LOGE("[spi write]SPI master send_and_receive polling failed \r\n");
            }
        }

        /*step 3. Config Read*/
        vsm_spi_send_buffer[0] = SPI_CR_CMD;
        vsm_spi_send_buffer[1] = reg;
        vsm_spi_send_buffer[2] = addr;
        vsm_spi_send_buffer[3] = (size & 0xFF) - 1;
        vsm_spi_send_buffer[4] = (size & 0xFF00)>>8;    

        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(g_vsm_spi_master_port, vsm_spi_send_buffer, 5)) {
            LOGE("[spi read]SPI master send_and_receive polling failed,port %d \r\n", g_vsm_spi_master_port);
        }
        /*step 4. Start Data transmission*/
        vsm_spi_send_buffer[0] = read_data_cmd;
        spi_send_and_receive_config.send_data = vsm_spi_send_buffer;
        spi_send_and_receive_config.send_length = 1;
        spi_send_and_receive_config.receive_buffer = vsm_spi_receive_buffer;
        spi_send_and_receive_config.receive_length = size+1;
        //#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
        ret = hal_spi_master_send_and_receive_polling(g_vsm_spi_master_port, &spi_send_and_receive_config);

        for (i = 0; i < size; i ++) {
            if (i < 4) {
                data[i] = vsm_spi_receive_buffer[1+i];
            }
        }
    }
    

    if (!ret) {
        ret = VSM_STATUS_OK;
    } else {
        ret = VSM_STATUS_ERROR;
    }
    return ret;
}
#endif
