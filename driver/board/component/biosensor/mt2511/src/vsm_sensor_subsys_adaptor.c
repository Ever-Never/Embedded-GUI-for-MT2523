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
#include "task.h"
#include "vsm_driver.h"
#include "vsm_sensor_subsys_adaptor.h"
#include "sensor_alg_interface.h"
#include "sensor_manager.h" /* cmd_event_t */
#include "hal_eint.h"
#include "hal_pmu.h"

#define BIO_MAX_BUF_CNT 128
#define BIO_PPG_BUF_CNT 192
//#define ENABLE_DISABLE_TEST

log_create_module(mt2511_SM, PRINT_LEVEL_INFO);

#if 1
#define LOGE(fmt,arg...)   LOG_E(mt2511_SM, "[mt2511]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(mt2511_SM, "[mt2511]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(mt2511_SM, "[mt2511]"fmt,##arg)
#else
#define LOGE(fmt,arg...)   printf("[biogui]"fmt,##arg)
#define LOGW(fmt,arg...)   printf("[biogui]"fmt,##arg)
#define LOGI(fmt,arg...)   printf("[biogui]"fmt,##arg)
#endif

//#define MT2511_LOG_MSG
#ifdef MT2511_LOG_MSG
#define LOGD(fmt,arg...)   LOG_I(mt2511_SM, "[mt2511]"fmt,##arg)
#else
#define LOGD(fmt,arg...)
#endif

#define PPG_SAMPLE_PERIOD_MS    8
#define EKG_SAMPLE_PERIOD_MS    2
#define SENSOR_SPP_DATA_MAGIC       54321
#define SENSOR_SPP_DATA_RESERVED    12345
#define INTR_CTRL_ADDR      0x334C
#define INTR_STATUS_ADDR    0x3354
#define EKG_IRQ_STATUS_MASK    0xE
#define PPG1_IRQ_STATUS_MASK   0x70
#define PPG2_IRQ_STATUS_MASK   0x380
#define BISI_IRQ_STATUS_MASK   0x1C00

//#define DUMP_PPG_REG

uint32_t sram_data[VSM_SRAM_LEN];
uint8_t  sram_type = VSM_SRAM_PPG1;

uint32_t pre_operation_time_ppg1 = 0;
uint32_t pre_operation_time_ppg2 = 0;
uint32_t pre_operation_time_bisi = 0;
uint32_t pre_operation_time_ekg = 0;
uint32_t pre_operation_time_eeg = 0;
uint32_t pre_operation_time_emg = 0;
uint32_t pre_operation_time_gsr = 0;
static bool vsm_init = true;

static uint8_t vsm_eint_num;
cmd_event_t vsm_event;
#ifdef ENABLE_DISABLE_TEST
bool ppg_first_use = true;
#endif

#ifdef FUSION_BLOOD_PRESSURE_USE
extern int data_lost_callback(uint32_t sensor_type);
extern int32_t bp_data_full;
#endif

#ifdef DUMP_PPG_REG
uint16_t ppg_addr[] = {0x2308, 0x230C, 0x2324, 0x3344, 0x3348, 0x232C, 0x2330, 0x2334, 0x2338, 0x233c,
                       0x2340, 0x2344, 0x2348, 0x234C, 0x2350, 0x2354, 0x2358, 0x235C, 0x2360, 0x2364,
                       0x2368, 0x236C, 0x2370, 0x3368, 0x334C, 0x3300, 0x3318, 0x3320, 0x3324, 0x3328,
                       0x332C, 0x3334, 0x33DC, 0x33D0, 0x3360};


static void vsm_dump_ppg_register_setting(void)
{
    bus_data_t data;
    uint32_t read_data, i = 0;
    int len = sizeof(ppg_addr)/sizeof(ppg_addr[0]);
    data.data_buf = (uint8_t *)&read_data;
	data.length = sizeof(read_data);
    for (i = 0;i < len; i ++) {
        read_data = 0;
        data.reg  = (ppg_addr[i] & 0xFF);
        data.addr = (ppg_addr[i] & 0xFF00) >> 8;
        vsm_driver_read_register(&data);
        printf("0x%x,0x%x\r\n", ppg_addr[i], read_data);
    }
}

static void vsm_dump_all_register_setting(void)
{
    bus_data_t data;
    uint32_t read_data, i = 0;
    data.data_buf = (uint8_t *)&read_data;
	data.length = sizeof(read_data);
    //dump 0x23 domain
    while(data.reg <= 0xAC) {
        read_data = 0;
        data.addr = 0x23;
        vsm_driver_read_register(&data);
        printf("0x23 0x%x,0x%x\r\n", data.reg, read_data);
        data.reg  += 4;
    }
    data.reg = 0;
    //dump 0x33 domain
    while(data.reg < 0xFF) {
        read_data = 0;
        data.addr = 0x33;
        vsm_driver_read_register(&data);
        printf("0x33 0x%x,0x%x\r\n", data.reg, read_data);
        if (data.reg == 0xFC) {
            break;
        }
        data.reg  += 4;
    }
}
#endif

int32_t time_offset_compute(uint32_t current_tick, uint32_t pre_tick, int number_of_data)
{
    return (current_tick  - pre_tick)/(number_of_data + 1);
}

void vsm_data_drop(vsm_sram_type_t sram_type)
{
    int32_t output_len = 0, i = 5;

    vsm_driver_read_sram(sram_type, (uint32_t *)sram_data, &output_len);
    LOGD("%s():output_len %d\r\n", __func__, output_len);
    while(output_len !=0 && i != 0){
        output_len = 0;
        vsm_driver_read_sram(sram_type, (uint32_t *)sram_data, &output_len);
        i--;
        LOGD("%s():output_len %d\r\n", __func__, output_len);
    }
    LOGD("%s():--\r\n", __func__);
}

void vsm_revert_ppg_data(uint32_t *data_buf, int32_t len)
{
    int32_t i = 0;
    uint32_t temp = 0;
    for (i = 0; i < len; i ++){
        if (i % 2 == 1) {
            temp = *(data_buf + i - 1);
            *(data_buf + i - 1) = *(data_buf + i);
            *(data_buf + i) = temp;
        }
    }

}

//sensor manager operation call
int32_t vsm_ppg1_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                    void *buff_in, int32_t size_in)
{
    int err = 0;
    int32_t value = 0;

    if (vsm_init == false) {
        if (actualout) {
            *actualout = 0;
        }
        err = -1;
        return err;
    }
    switch (command) {
        case SENSOR_DELAY:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("BIO PPG1 SENSOR_DELAY sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO PPG1 SENSOR_DELAY ms (%ld) \r\n", value);
            }
            break;

        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("BIO PPG1 Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO PPG1 SENSOR_ENABLE (%ld) \r\n", value);

                if (value) {
#ifndef ENABLE_DISABLE_TEST
                    vsm_driver_set_signal(VSM_SIGNAL_PPG1);
#else
                    if (ppg_first_use) {
                        ppg_first_use = false;
                        LOGI("Set signal \r\n");
                        vsm_driver_set_signal(VSM_SIGNAL_PPG1);
                    } else {
                        LOGI("Enable signal \r\n");
                        vsm_driver_enable_signal(VSM_SIGNAL_PPG1);
                    }
#endif
                } else {
                    // ToDo: disable sensor
                    //vsm_driver_disable_signal(VSM_SIGNAL_PPG1);
                    vsm_driver_clear_signal(VSM_SIGNAL_PPG1);
                }
            }
            break;

        case SENSOR_GET_DATA: {
            int32_t output_len, i = 0;
            uint32_t current_tick;
            int32_t time_offset;
            #ifdef PPG_DEBUG_RESET
            static int32_t ppg1_zero_cnt = 0;
            #endif
            #ifdef DUMP_PPG_REG
            static int32_t ppg1_print_cnt = 0;
            #endif

            memset(sram_data, 0, sizeof(sram_data));
            current_tick = sensor_driver_get_ms_tick();
            vsm_driver_read_sram(VSM_SRAM_PPG1, (uint32_t *)sram_data, &output_len);
            if (output_len > BIO_MAX_BUF_CNT || output_len > size_out ) {
                send_sensor_data_via_btspp(
                            SENSOR_SPP_DATA_MAGIC,
                            979,
                            output_len,
                            size_out,
                            0,
                            0,
                            SENSOR_SPP_DATA_RESERVED);
                LOGE("BIO PPG1 o (%d) s (%d)\r\n", output_len,size_out);
            }

            //LOGI("BIO PPG1 output (%d)\r\n", output_len);
            *actualout = (output_len > BIO_MAX_BUF_CNT) ? BIO_MAX_BUF_CNT: output_len;
#ifndef BIO_TUNNING_TOOL_USE
            //make amb/led order to led/amb
            vsm_revert_ppg_data(sram_data, *actualout);
#endif
            #ifdef DUMP_PPG_REG
            if (ppg1_print_cnt++ % 100 == 0) {
                vsm_dump_ppg_register_setting();
            }
            #endif
            //put sensor data to sensor manager
            if ( output_len > 0) {
                sensor_data_unit_t *start = (sensor_data_unit_t *)buff_out;
                time_offset = PPG_SAMPLE_PERIOD_MS;
                if(pre_operation_time_ppg1 != 0 &&
                    pre_operation_time_ppg1 >= (current_tick - ((*actualout/2 - 1) * time_offset))) {
                    time_offset = time_offset_compute(current_tick, pre_operation_time_ppg1, *actualout/2);
                }
                for (i = 0; i < *actualout; i ++) {
                    start->bio_data.data= sram_data[i];
                    start->sensor_type= SENSOR_TYPE_BIOSENSOR_PPG1;
                    start->time_stamp = current_tick - ((*actualout/2 - (i/2) - 1) * time_offset);
                    if (i == 0) {
                        if (pre_operation_time_ppg1 >= start->time_stamp) {
                            LOGE("%s():current_tick %lu,pre_operation_time_ppg1 %lu,time_offset %lu\r\n",
                                __func__, current_tick, pre_operation_time_ppg1, time_offset);
                        }
                    }
                    start++;
                }
                #ifdef PPG_DEBUG_RESET
                ppg1_zero_cnt = 0;
                #endif
            } else if ( output_len == 0) {
                #ifdef PPG_DEBUG_RESET
                ppg1_zero_cnt++;
                LOGI("ppg1_zero_cnt (%d)\r\n", ppg1_zero_cnt);
                #ifndef HR_PPG_64HZ
                if (ppg1_zero_cnt % 5 == 0) {
                #else
                if (ppg1_zero_cnt % 20 == 0) {
                #endif
                    /***reset mt2511***/
                    vsm_driver_clear_signal(VSM_SIGNAL_PPG1);
                    vTaskDelay(100);
                    vsm_driver_set_signal(VSM_SIGNAL_PPG1);
                }
                #else
                LOGI("PPG data output_len (%d)\r\n", output_len);
                #endif
            }
            pre_operation_time_ppg1 = current_tick;
        }

        break;

        case SENSOR_CUST:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("PPG1 SENSOR_CUST parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("PPG1 SENSOR_CUST (%ld) \r\n", value);
                //ToDo: Customization setting
            }
            break;

        case SENSOR_BATCH:
            break;

        default:
            LOGE("operate function no this parameter %ld! \r\n", command);
            err = -1;
            break;
    }

    return err;
}

int32_t vsm_ppg2_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                    void *buff_in, int32_t size_in)
{
    int err = 0;
    int32_t value = 0;
    //unsigned char receive[1] = {0};
    if (vsm_init == false) {
        if (actualout) {
            *actualout = 0;
        }
        err = -1;
        return err;
    }
    switch (command) {
        case SENSOR_DELAY:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("BIO PPG2 SENSOR_DELAY sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO PPG2 SENSOR_DELAY ms (%ld) \r\n", value);
            }
            break;

        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("BIO PPG2 Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO PPG2 SENSOR_ENABLE (%ld) \r\n", value);

                if (value) {
                    //vsm_enable_mode();
                    vsm_driver_set_signal(VSM_SIGNAL_PPG2);
                } else {
                    // ToDo: disable sensor
                    //vsm_driver_disable_signal(VSM_SIGNAL_PPG2);
                    vsm_driver_clear_signal(VSM_SIGNAL_PPG2);
                }
            }
            break;

        case SENSOR_GET_DATA: {
            int32_t output_len, i = 0;
            uint32_t current_tick;
            int32_t time_offset;

            memset(sram_data, 0, sizeof(sram_data));
            current_tick = sensor_driver_get_ms_tick();
            vsm_driver_read_sram(VSM_SRAM_PPG2, (uint32_t *)sram_data, &output_len);
            if (output_len > BIO_MAX_BUF_CNT || output_len > size_out) {
                send_sensor_data_via_btspp(
                            SENSOR_SPP_DATA_MAGIC,
                            980,
                            output_len,
                            size_out,
                            0,
                            0,
                            SENSOR_SPP_DATA_RESERVED);
                LOGE("BIO PPG2 o (%d) s (%d)\r\n", output_len,size_out);
            }
            *actualout = (output_len > BIO_MAX_BUF_CNT) ? BIO_MAX_BUF_CNT: output_len;
            //put sensor data to sensor manager
            if ( output_len > 0) {
                sensor_data_unit_t *start = (sensor_data_unit_t *)buff_out;
                time_offset = PPG_SAMPLE_PERIOD_MS;
                if(pre_operation_time_ppg2 != 0 &&
                    pre_operation_time_ppg2 >= (current_tick - ((*actualout/2 ) * time_offset))) {
                    time_offset = time_offset_compute(current_tick, pre_operation_time_ppg2, *actualout/2);
                }
                for (i = 0; i < *actualout; i ++) {
                    start->bio_data.data = sram_data[i];
                    start->sensor_type = SENSOR_TYPE_BIOSENSOR_PPG2;
                    start->time_stamp = current_tick - ((*actualout/2 - (i/2) - 1) * time_offset);
                    if (i == 0) {
                        if (pre_operation_time_ppg2 >= start->time_stamp) {
                            LOGE("%s():current_tick %lu,pre_operation_time_ppg2 %lu,time_offset %lu\r\n",
                                __func__, current_tick, pre_operation_time_ppg2, time_offset);
                        }
                    }
                    start++;
                }
            }
            pre_operation_time_ppg2 = current_tick;
        }

        break;

        case SENSOR_CUST:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("PPG2 SENSOR_CUST parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("PPG2 SENSOR_CUST (%ld) \r\n", value);
                //ToDo: Customization setting
            }
            break;

        case SENSOR_BATCH:
            break;

        default:
            LOGE("operate function no this parameter %ld! \r\n", command);
            err = -1;
            break;
    }

    return err;
}

int32_t vsm_bisi_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                    void *buff_in, int32_t size_in)
{
    int err = 0;
    int32_t value = 0;
    //unsigned char receive[1] = {0};
    if (vsm_init == false) {
        if (actualout) {
            *actualout = 0;
        }
        err = -1;
        return err;
    }
    switch (command) {
        case SENSOR_DELAY:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("BIO BISI SENSOR_DELAY sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO BISI SENSOR_DELAY ms (%ld) \r\n", value);
            }
            break;

        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("BIO BISI Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO BISI SENSOR_ENABLE (%ld) \r\n", value);

                if (value) {
                    //vsm_enable_mode();
                    vsm_driver_set_signal(VSM_SIGNAL_BISI);
                } else {
                    // ToDo: disable sensor
                    //vsm_driver_disable_signal(VSM_SIGNAL_BISI);
                    vsm_driver_clear_signal(VSM_SIGNAL_BISI);
                }
            }
            break;

        case SENSOR_GET_DATA: {
            int32_t output_len, i = 0;
            uint32_t current_tick;
            int32_t time_offset;

            memset(sram_data, 0, sizeof(sram_data));
            current_tick = sensor_driver_get_ms_tick();
            vsm_driver_read_sram(VSM_SRAM_BISI, (uint32_t *)sram_data, &output_len);
            if (output_len > BIO_MAX_BUF_CNT || output_len > size_out) {
                send_sensor_data_via_btspp(
                            SENSOR_SPP_DATA_MAGIC,
                            981,
                            output_len,
                            size_out,
                            0,
                            0,
                            SENSOR_SPP_DATA_RESERVED);
                LOGE("BIO BISI o (%d) s (%d)\r\n", output_len,size_out);
            }
            *actualout = (output_len > BIO_MAX_BUF_CNT) ? BIO_MAX_BUF_CNT: output_len;
            //put sensor data to sensor manager
            if ( output_len > 0) {
                sensor_data_unit_t *start = (sensor_data_unit_t *)buff_out;
                time_offset = PPG_SAMPLE_PERIOD_MS;
                if(pre_operation_time_bisi != 0 &&
                    pre_operation_time_bisi >= (current_tick - ((*actualout - 1) * time_offset))) {
                    time_offset = time_offset_compute(current_tick, pre_operation_time_bisi, *actualout);
                }
                for (i = 0; i < *actualout; i ++) {
                    start->bio_data.data= sram_data[i];
                    start->sensor_type = SENSOR_TYPE_BIOSENSOR_BISI;
                    start->time_stamp = current_tick - ((*actualout- 1) * time_offset);
                    if (i == 0) {
                        if (pre_operation_time_bisi >= start->time_stamp) {
                            LOGE("%s():current_tick %lu,pre_operation_time_bisi %lu,time_offset %lu\r\n",
                                __func__, current_tick, pre_operation_time_bisi, time_offset);
                        }
                    }
                    start++;
                }
            }
            pre_operation_time_bisi = current_tick;
        }

        break;

        case SENSOR_CUST:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("BISI SENSOR_CUST parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BISI SENSOR_CUST (%ld) \r\n", value);
                //ToDo: Customization setting
            }
            break;

        case SENSOR_BATCH:
            break;

        default:
            LOGE("operate function no this parameter %ld! \r\n", command);
            err = -1;
            break;
    }

    return err;
}

int32_t vsm_ekg_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                    void *buff_in, int32_t size_in)
{
    int err = 0;
    int32_t value = 0;
    //unsigned char receive[1] = {0};
    if (vsm_init == false) {
        if (actualout) {
            *actualout = 0;
        }
        err = -1;
        return err;
    }
    switch (command) {
        case SENSOR_DELAY:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("BIO EKG SENSOR_DELAY sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO EKG SENSOR_DELAY ms (%ld) \r\n", value);
            }
            break;

        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("BIO EKG Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO EKG SENSOR_ENABLE (%ld) \r\n", value);

                if (value) {
                    //vsm_enable_mode();
                    vsm_driver_set_signal(VSM_SIGNAL_EKG);
                } else {
                    // ToDo: disable sensor
                    //vsm_driver_disable_signal(VSM_SIGNAL_EKG);
                    vsm_driver_clear_signal(VSM_SIGNAL_EKG);
                }
            }
            break;

        case SENSOR_GET_DATA: {
            int32_t output_len, i = 0;
            uint32_t current_tick;
            int32_t time_offset;

            #ifdef FUSION_BLOOD_PRESSURE_USE
            if (bp_data_full) {
                *actualout = 0;
                break;
            }
            #endif

            memset(sram_data, 0, sizeof(sram_data));
            current_tick = sensor_driver_get_ms_tick();
            vsm_driver_read_sram(VSM_SRAM_EKG, (uint32_t *)sram_data, &output_len);
            if (output_len > BIO_MAX_BUF_CNT || output_len > size_out) {
                #ifdef FUSION_BLOOD_PRESSURE_USE
                data_lost_callback(SENSOR_TYPE_BIOSENSOR_EKG);
                #endif
                send_sensor_data_via_btspp(
                            SENSOR_SPP_DATA_MAGIC,
                            978,
                            output_len,
                            size_out,
                            0,
                            0,
                            SENSOR_SPP_DATA_RESERVED);
                LOGE(" EKG o (%d) s (%d)\r\n", output_len,size_out);
            }
            //printf(" EKG SENSOR_GET_DATA output_len (%d) \r\n", output_len);
            *actualout = (output_len > BIO_MAX_BUF_CNT) ? BIO_MAX_BUF_CNT: output_len;
            //put sensor data to sensor manager
            if ( output_len > 0) {
                sensor_data_unit_t *start = (sensor_data_unit_t *)buff_out;
                time_offset = EKG_SAMPLE_PERIOD_MS;
                if(pre_operation_time_ekg != 0 &&
                    pre_operation_time_ekg >= (current_tick - ((*actualout - 1) * time_offset))) {
                    time_offset = time_offset_compute(current_tick, pre_operation_time_ekg, *actualout);
                }
                for (i = 0; i < *actualout; i ++) {
                    start->bio_data.data= sram_data[i];
                    start->sensor_type = SENSOR_TYPE_BIOSENSOR_EKG;
                    start->time_stamp = current_tick - ((*actualout- 1 - i) * time_offset);
                    if (i == 0) {
                        if (pre_operation_time_ekg >= start->time_stamp) {
                            LOGE("%s():current_tick %lu,pre_operation_time_ekg %lu,time_offset %lu\r\n",
                                __func__, current_tick, pre_operation_time_ekg, time_offset);
                        }
                    }
                    start++;
                }
            }
            pre_operation_time_ekg = current_tick;
        }

        break;

        case SENSOR_CUST:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("EKG SENSOR_CUST parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("EKG SENSOR_CUST (%ld) \r\n", value);
                //ToDo: Customization setting
            }
            break;

        case SENSOR_BATCH:
            break;

        default:
            LOGE("operate function no this parameter %ld! \r\n", command);
            err = -1;
            break;
    }

    return err;
}

int32_t vsm_eeg_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                    void *buff_in, int32_t size_in)
{
    int err = 0;
    int32_t value = 0;
    //unsigned char receive[1] = {0};
    if (vsm_init == false) {
        if (actualout) {
            *actualout = 0;
        }
        err = -1;
        return err;
    }
    switch (command) {
        case SENSOR_DELAY:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("BIO EEG SENSOR_DELAY sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO EEG SENSOR_DELAY ms (%ld) \r\n", value);
            }
            break;

        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("BIO EEG Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO EEG SENSOR_ENABLE (%ld) \r\n", value);

                if (value) {
                    //vsm_enable_mode();
                    vsm_driver_set_signal(VSM_SIGNAL_EEG);
                } else {
                    // ToDo: disable sensor
                    //vsm_driver_disable_signal(VSM_SIGNAL_EEG);
                    vsm_driver_clear_signal(VSM_SIGNAL_EEG);
                }
            }
            break;

        case SENSOR_GET_DATA: {
            int32_t output_len, i = 0;
            uint32_t current_tick;
            int32_t time_offset;

            memset(sram_data, 0, sizeof(sram_data));
            current_tick = sensor_driver_get_ms_tick();
            vsm_driver_read_sram(VSM_SRAM_EKG, (uint32_t *)sram_data, &output_len);
            if (output_len > BIO_MAX_BUF_CNT || output_len > size_out) {
                send_sensor_data_via_btspp(
                            SENSOR_SPP_DATA_MAGIC,
                            978,
                            output_len,
                            size_out,
                            0,
                            0,
                            SENSOR_SPP_DATA_RESERVED);
                LOGE(" EEG o (%d) s (%d)\r\n", output_len,size_out);
            }
            *actualout = (output_len > BIO_MAX_BUF_CNT) ? BIO_MAX_BUF_CNT: output_len;

            //put sensor data to sensor manager
            if ( output_len > 0) {
                sensor_data_unit_t *start = (sensor_data_unit_t *)buff_out;
                time_offset = EKG_SAMPLE_PERIOD_MS;
                if(pre_operation_time_eeg != 0 &&
                    pre_operation_time_eeg >= (current_tick - ((*actualout - 1) * time_offset))) {
                    time_offset = time_offset_compute(current_tick, pre_operation_time_eeg, *actualout);
                }
                for (i = 0; i < *actualout; i ++) {
                    start->bio_data.data= sram_data[i];
                    start->sensor_type = SENSOR_TYPE_BIOSENSOR_EEG;
                    start->time_stamp = current_tick - ((*actualout- 1 - i) * time_offset);
                    if (i == 0) {
                        if (pre_operation_time_eeg >= start->time_stamp) {
                            LOGE("%s():current_tick %lu,pre_operation_time_eeg %lu,time_offset %lu\r\n",
                                __func__, current_tick, pre_operation_time_eeg, time_offset);
                        }
                    }
                    start++;
                }
            }
            pre_operation_time_eeg = current_tick;
        }

        break;

        case SENSOR_CUST:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("EEG SENSOR_CUST parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("EEG SENSOR_CUST (%ld) \r\n", value);
                //ToDo: Customization setting
            }
            break;

        case SENSOR_BATCH:
            break;

        default:
            LOGE("operate function no this parameter %ld! \r\n", command);
            err = -1;
            break;
    }

    return err;
}

int32_t vsm_emg_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                    void *buff_in, int32_t size_in)
{
    int err = 0;
    int32_t value = 0;
    //unsigned char receive[1] = {0};
    if (vsm_init == false) {
        if (actualout) {
            *actualout = 0;
        }
        err = -1;
        return err;
    }
    switch (command) {
        case SENSOR_DELAY:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("BIO EMG SENSOR_DELAY sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO EMG SENSOR_DELAY ms (%ld) \r\n", value);
            }
            break;

        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("BIO EMG Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO EMG SENSOR_ENABLE (%ld) \r\n", value);

                if (value) {
                    //vsm_enable_mode();
                    vsm_driver_set_signal(VSM_SIGNAL_EMG);
                } else {
                    // ToDo: disable sensor
                    //vsm_driver_disable_signal(VSM_SIGNAL_EMG);
                    vsm_driver_clear_signal(VSM_SIGNAL_EMG);
                }
            }
            break;

        case SENSOR_GET_DATA: {
            int32_t output_len, i = 0;
            uint32_t current_tick;
            int32_t time_offset;

            memset(sram_data, 0, sizeof(sram_data));
            current_tick = sensor_driver_get_ms_tick();
            vsm_driver_read_sram(VSM_SRAM_EKG, (uint32_t *)sram_data, &output_len);
            if (output_len > BIO_MAX_BUF_CNT || output_len > size_out) {
                send_sensor_data_via_btspp(
                            SENSOR_SPP_DATA_MAGIC,
                            978,
                            output_len,
                            size_out,
                            0,
                            0,
                            SENSOR_SPP_DATA_RESERVED);
                LOGE(" EMG o (%d) s (%d)\r\n", output_len,size_out);
            }
            *actualout = (output_len > BIO_MAX_BUF_CNT) ? BIO_MAX_BUF_CNT: output_len;

            //put sensor data to sensor manager
            if ( output_len > 0) {
                sensor_data_unit_t *start = (sensor_data_unit_t *)buff_out;
                time_offset = EKG_SAMPLE_PERIOD_MS;
                if(pre_operation_time_emg != 0 &&
                    pre_operation_time_emg >= (current_tick - ((*actualout - 1 - i) * time_offset))) {
                    time_offset = time_offset_compute(current_tick, pre_operation_time_emg, *actualout);
                }
                for (i = 0; i < *actualout; i ++) {
                    start->bio_data.data= sram_data[i];
                    start->sensor_type = SENSOR_TYPE_BIOSENSOR_EMG;
                    start->time_stamp = current_tick - ((*actualout- 1) * time_offset);
                    if (i == 0) {
                        if (pre_operation_time_emg >= start->time_stamp) {
                            LOGE("%s():current_tick %lu,pre_operation_time_emg %lu,time_offset %lu\r\n",
                                __func__, current_tick, pre_operation_time_emg, time_offset);
                        }
                    }
                    start++;
                }
            }
            pre_operation_time_emg = current_tick;
        }

        break;

        case SENSOR_CUST:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("EMG SENSOR_CUST parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("EMG SENSOR_CUST (%ld) \r\n", value);
                //ToDo: Customization setting
            }
            break;

        case SENSOR_BATCH:
            break;

        default:
            LOGE("operate function no this parameter %ld! \r\n", command);
            err = -1;
            break;
    }

    return err;
}

int32_t vsm_gsr_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                    void *buff_in, int32_t size_in)
{
    int err = 0;
    int32_t value = 0;
    //unsigned char receive[1] = {0};
    if (vsm_init == false) {
        if (actualout) {
            *actualout = 0;
        }
        err = -1;
        return err;
    }
    switch (command) {
        case SENSOR_DELAY:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("BIO GSR SENSOR_DELAY sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO GSR SENSOR_DELAY ms (%ld) \r\n", value);
            }
            break;

        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("BIO GSR Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO GSR SENSOR_ENABLE (%ld) \r\n", value);

                if (value) {
                    //vsm_enable_mode();
                    vsm_driver_set_signal(VSM_SIGNAL_GSR);
                } else {
                    // ToDo: disable sensor
                    //vsm_driver_disable_signal(VSM_SIGNAL_GSR);
                    vsm_driver_clear_signal(VSM_SIGNAL_GSR);
                }
            }
            break;

        case SENSOR_GET_DATA: {
            int32_t output_len, i = 0;
            uint32_t current_tick;
            int32_t time_offset;

            memset(sram_data, 0, sizeof(sram_data));
            current_tick = sensor_driver_get_ms_tick();
            vsm_driver_read_sram(VSM_SRAM_EKG, (uint32_t *)sram_data, &output_len);
            if (output_len > BIO_MAX_BUF_CNT || output_len > size_out) {
                send_sensor_data_via_btspp(
                            SENSOR_SPP_DATA_MAGIC,
                            978,
                            output_len,
                            size_out,
                            0,
                            0,
                            SENSOR_SPP_DATA_RESERVED);
                LOGE(" GSR o (%d) s (%d)\r\n", output_len,size_out);
            }
            *actualout = (output_len > BIO_MAX_BUF_CNT) ? BIO_MAX_BUF_CNT: output_len;

            //put sensor data to sensor manager
            if ( output_len > 0) {
                sensor_data_unit_t *start = (sensor_data_unit_t *)buff_out;
                time_offset = EKG_SAMPLE_PERIOD_MS;
                if(pre_operation_time_gsr != 0 &&
                    pre_operation_time_gsr >= (current_tick - ((*actualout - 1 - i) * time_offset))) {
                    time_offset = time_offset_compute(current_tick, pre_operation_time_gsr, *actualout);
                }
                for (i = 0; i < *actualout; i ++) {
                    start->bio_data.data= sram_data[i];
                    start->sensor_type = SENSOR_TYPE_BIOSENSOR_GSR;
                    start->time_stamp = current_tick - ((*actualout- 1) * time_offset);
                    if (i == 0) {
                        if (pre_operation_time_gsr >= start->time_stamp) {
                            LOGE("%s():current_tick %lu,pre_operation_time_gsr %lu,time_offset %lu\r\n",
                                __func__, current_tick, pre_operation_time_gsr, time_offset);
                        }
                    }
                    start++;
                }
            }
            pre_operation_time_gsr = current_tick;

        }

        break;

        case SENSOR_CUST:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("GSR SENSOR_CUST parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("GSR SENSOR_CUST (%ld) \r\n", value);
                //ToDo: Customization setting
            }
            break;

        case SENSOR_BATCH:
            break;

        default:
            LOGE("operate function no this parameter %ld! \r\n", command);
            err = -1;
            break;
    }

    return err;
}

int32_t vsm_ppg1_512hz_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                    void *buff_in, int32_t size_in)
{
    int err = 0;
    int32_t value = 0;
    //unsigned char receive[1] = {0};
    if (vsm_init == false) {
        if (actualout) {
            *actualout = 0;
        }
        err = -1;
        return err;
    }
    switch (command) {
        case SENSOR_DELAY:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("BIO PPG1_512HZ SENSOR_DELAY sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO PPG1_512HZ SENSOR_DELAY ms (%ld) \r\n", value);
            }
            break;

        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("BIO PPG1_512HZ Enable sensor parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("BIO PPG1_512HZ SENSOR_ENABLE (%ld) \r\n", value);

                if (value) {
                    //vsm_enable_mode();
                    vsm_driver_set_signal(VSM_SIGNAL_PPG1_512HZ);
                } else {
                    // ToDo: disable sensor
                    //vsm_driver_disable_signal(VSM_SIGNAL_PPG1_512HZ);
                    vsm_driver_clear_signal(VSM_SIGNAL_PPG1_512HZ);
                }
            }
            break;

        case SENSOR_GET_DATA: {
            int32_t output_len, i = 0;
            uint32_t current_tick;
            int32_t time_offset;
            #ifdef DUMP_PPG_REG
            static int32_t ppg512_print_cnt = 0;
            #endif

            #ifdef FUSION_BLOOD_PRESSURE_USE
            if (bp_data_full) {
                *actualout = 0;
                break;
            }
            #endif

            memset(sram_data, 0, sizeof(sram_data));
            current_tick = sensor_driver_get_ms_tick();
            vsm_driver_read_sram(VSM_SRAM_PPG1, (uint32_t *)sram_data, &output_len);
            if (output_len > BIO_PPG_BUF_CNT || output_len > size_out) {
                #ifdef FUSION_BLOOD_PRESSURE_USE
                data_lost_callback(SENSOR_TYPE_BIOSENSOR_PPG1_512HZ);
                #endif
                send_sensor_data_via_btspp(
                            SENSOR_SPP_DATA_MAGIC,
                            979,
                            output_len,
                            size_out,
                            0,
                            0,
                            SENSOR_SPP_DATA_RESERVED);
                LOGE("BIO PPG1 o (%d) s (%d)\r\n", output_len,size_out);
            }
            //printf("BIO PPG1_512HZ SENSOR_GET_DATA output_len (%d) \r\n", output_len);
            *actualout = (output_len > BIO_PPG_BUF_CNT) ? BIO_PPG_BUF_CNT: output_len;
#ifndef BIO_TUNNING_TOOL_USE
            //make amb/led order to led/amb
            vsm_revert_ppg_data(sram_data, *actualout);
#endif
            #ifdef DUMP_PPG_REG
            if (ppg512_print_cnt++ % 100 == 0) {
                vsm_dump_ppg_register_setting();
            }
            #endif
            //put sensor data to sensor manager
            if ( output_len > 0) {
                sensor_data_unit_t *start = (sensor_data_unit_t *)buff_out;
                time_offset = PPG_SAMPLE_PERIOD_MS;
                if(pre_operation_time_ppg1 != 0 &&
                    pre_operation_time_ppg1 >= (current_tick - ((*actualout/2 - 1) * time_offset))) {
                    time_offset = time_offset_compute(current_tick, pre_operation_time_ppg1, *actualout/2);
                }
                for (i = 0; i < *actualout; i ++) {
                    start->bio_data.data= sram_data[i];
                    start->sensor_type= SENSOR_TYPE_BIOSENSOR_PPG1_512HZ;
                    start->time_stamp = current_tick - ((*actualout/2 - (i/2) - 1) * time_offset);
                    if (i == 0) {
                        if (pre_operation_time_ppg1 >= start->time_stamp) {
                            LOGE("%s():current_tick %lu,pre_operation_time_ppg1 %lu,time_offset %lu\r\n",
                                __func__, current_tick, pre_operation_time_ppg1, time_offset);
                        }
                    }
                    start++;
                }
            }
            pre_operation_time_ppg1 = current_tick;
        }

        break;

        case SENSOR_CUST:
            if ((buff_in == NULL) || (size_in < sizeof(int32_t))) {
                LOGE("PPG1_512HZ SENSOR_CUST parameter error!\n");
                err = -1;
            } else {
                value = *(int32_t *)buff_in;
                LOGI("PPG1_512HZ SENSOR_CUST (%ld) \r\n", value);
                //ToDo: Customization setting
            }
            break;

        case SENSOR_BATCH:
            break;

        default:
            LOGE("operate function no this parameter %ld! \r\n", command);
            err = -1;
            break;
    }

    return err;
}

int32_t vsm_reenable_irq(uint32_t intr_ctrl)
{
    bus_data_t data;
    int32_t ret = 0;
    uint32_t read_data;

    data.addr = (INTR_CTRL_ADDR & 0xFF00)>>8;
    data.reg = (INTR_CTRL_ADDR & 0xFF);
    data.data_buf = (uint8_t *)&read_data;
    data.length = sizeof(read_data);
    ret = vsm_driver_write_register(&data);
    vsm_driver_update_register();
    return ret;
}

int32_t vsm_check_trigger_signal(uint32_t *signal, uint32_t *intr_ctrl)
{
    bus_data_t data;
    uint32_t status_data, ctrl_data;
    uint32_t bak_ctrl_data, bak_status_data;
    int32_t ret = 0;

    if (!signal) {
        return -1;
    }
    //1.check which source trigger interrupt
    data.addr = (INTR_STATUS_ADDR & 0xFF00)>>8;
    data.reg = (INTR_STATUS_ADDR & 0xFF);
    data.data_buf = (uint8_t *)&status_data;
    data.length = sizeof(status_data);
    vsm_driver_read_register(&data);
    bak_status_data = status_data;

    //2.backup ctrl register data
    data.addr = (INTR_CTRL_ADDR & 0xFF00)>>8;
    data.reg = (INTR_CTRL_ADDR & 0xFF);
    data.data_buf = (uint8_t *)&ctrl_data;
    data.length = sizeof(ctrl_data);
    vsm_driver_read_register(&data);
    bak_ctrl_data = ctrl_data;

    //3.clear ctrl rgister
    ctrl_data = 0;
    vsm_driver_write_register(&data);
    vsm_driver_read_register(&data);

    //4.write irq in status rgister
    data.addr = (INTR_STATUS_ADDR & 0xFF00)>>8;
    data.reg = (INTR_STATUS_ADDR & 0xFF);
    data.data_buf = (uint8_t *)&status_data;
    data.length = sizeof(status_data);
    status_data = (status_data & 0xFFF) << 16;

    vsm_driver_write_register(&data);
    status_data = 0;
    vsm_driver_read_register(&data);

    //5.clear irq in status rgister
    status_data = 0;
    vsm_driver_write_register(&data);

    vsm_driver_read_register(&data);

    vsm_driver_update_register();
    //LOGD("%s():bak_ctrl_data 0x%x, bak_status_data 0x%x\r\n", __func__, bak_ctrl_data, bak_status_data);
    //backup control regiser
    *intr_ctrl = bak_ctrl_data;
    /*By control register or by status register*/
    *signal = 0;
    if (bak_status_data & EKG_IRQ_STATUS_MASK) {
        *signal |= (1U << SENSOR_TYPE_BIOSENSOR_EKG);
    }

    if (bak_status_data & PPG1_IRQ_STATUS_MASK) {
        *signal |= (1U << SENSOR_TYPE_BIOSENSOR_PPG1);
    }

    if (bak_status_data & PPG2_IRQ_STATUS_MASK) {
        *signal |= (1U << SENSOR_TYPE_BIOSENSOR_PPG2);
    }

    if (bak_status_data & BISI_IRQ_STATUS_MASK) {
        *signal |= (1U << SENSOR_TYPE_BIOSENSOR_BISI);
    }
    //LOGD("%s():signal 0x%x\r\n", __func__, *signal);
    return ret;
}

void vsm_eint_set(uint32_t eint_num)
{
    vsm_eint_num = (uint8_t)eint_num;
}

void vsm_eint_handler(void *parameter)
{
    BaseType_t xHigherPriorityTaskWoken;

    hal_eint_mask((hal_eint_number_t)vsm_eint_num);
    hal_eint_unmask((hal_eint_number_t)vsm_eint_num);

    vsm_event.event = SM_EVENT_DATA_READY;

    vsm_event.data_ready = (1U << SENSOR_TYPE_BIOSENSOR_PPG1);

    //bma255_event.timestamp = sensor_driver_get_ms_tick();
    /*ToDo: assign correct sample rate */
    vsm_event.delay = 8; /* PPG delay */

    // Post the event.
    xQueueSendFromISR(sm_queue_handle, &vsm_event, &xHigherPriorityTaskWoken);

    // Now the buffer is empty we can switch context if necessary.
    if ( xHigherPriorityTaskWoken ) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR (xHigherPriorityTaskWoken);
    }

}

vsm_status_t vsm_driver_sensor_subsys_init(void)
{
    sensor_driver_object_t obj_vsm_ppg1;
    sensor_driver_object_t obj_vsm_ppg2;
    sensor_driver_object_t obj_vsm_ekg;
    sensor_driver_object_t obj_vsm_eeg;
    sensor_driver_object_t obj_vsm_emg;
    sensor_driver_object_t obj_vsm_gsr;
    sensor_driver_object_t obj_vsm_bisi;
    sensor_driver_object_t obj_vsm_ppg1_512hz;

    vsm_status_t err;

    //driver initialization
    err = vsm_driver_init();
    if (err != VSM_STATUS_OK) {
        vsm_init = false;
    }

    /* register ppg1 to sensor manager */
    obj_vsm_ppg1.self = (void *)&obj_vsm_ppg1;
    #ifdef MT2511_USE_IRQ
    obj_vsm_ppg1.polling = 0; // interrupt mode
    #else
    obj_vsm_ppg1.polling = 1; // polling mode
    #endif
    obj_vsm_ppg1.sensor_operate = vsm_ppg1_operate;
    sensor_driver_attach(SENSOR_TYPE_BIOSENSOR_PPG1, &obj_vsm_ppg1);

    /* register ekg to sensor manager */
    obj_vsm_ekg.self = (void *)&obj_vsm_ekg;
    #ifdef MT2511_USE_IRQ
    obj_vsm_ekg.polling = 0; // interrupt mode
    #else
    obj_vsm_ekg.polling = 1; // polling mode
    #endif
    obj_vsm_ekg.sensor_operate = vsm_ekg_operate;
    sensor_driver_attach(SENSOR_TYPE_BIOSENSOR_EKG, &obj_vsm_ekg);

    /* register eeg to sensor manager */
    obj_vsm_eeg.self = (void *)&obj_vsm_eeg;
    #ifdef MT2511_USE_IRQ
    obj_vsm_eeg.polling = 0; // interrupt mode
    #else
    obj_vsm_eeg.polling = 1; // polling mode
    #endif
    obj_vsm_eeg.sensor_operate = vsm_eeg_operate;
    sensor_driver_attach(SENSOR_TYPE_BIOSENSOR_EEG, &obj_vsm_eeg);

    /* register emg to sensor manager */
    obj_vsm_emg.self = (void *)&obj_vsm_emg;
    #ifdef MT2511_USE_IRQ
    obj_vsm_emg.polling = 0; // interrupt mode
    #else
    obj_vsm_emg.polling = 1; // polling mode
    #endif
    obj_vsm_emg.sensor_operate = vsm_emg_operate;
    sensor_driver_attach(SENSOR_TYPE_BIOSENSOR_EMG, &obj_vsm_emg);

    /* register gsr to sensor manager */
    obj_vsm_gsr.self = (void *)&obj_vsm_gsr;
    #ifdef MT2511_USE_IRQ
    obj_vsm_gsr.polling = 0; // interrupt mode
    #else
    obj_vsm_gsr.polling = 1; // polling mode
    #endif
    obj_vsm_gsr.sensor_operate = vsm_gsr_operate;
    sensor_driver_attach(SENSOR_TYPE_BIOSENSOR_GSR, &obj_vsm_gsr);

    /* register ppg2 to sensor manager */
    obj_vsm_ppg2.self = (void *)&obj_vsm_ppg2;
    #ifdef MT2511_USE_IRQ
    obj_vsm_ppg2.polling = 0; // interrupt mode
    #else
    obj_vsm_ppg2.polling = 1; // polling mode
    #endif
    obj_vsm_ppg2.sensor_operate = vsm_ppg2_operate;
    sensor_driver_attach(SENSOR_TYPE_BIOSENSOR_PPG2, &obj_vsm_ppg2);

    /* register bisi to sensor manager */
    obj_vsm_bisi.self = (void *)&obj_vsm_bisi;
    #ifdef MT2511_USE_IRQ
    obj_vsm_bisi.polling = 0; // interrupt mode
    #else
    obj_vsm_bisi.polling = 1; // polling mode
    #endif
    obj_vsm_bisi.sensor_operate = vsm_bisi_operate;
    sensor_driver_attach(SENSOR_TYPE_BIOSENSOR_BISI, &obj_vsm_bisi);


    /* register blood pressure to sensor manager */
    obj_vsm_ppg1_512hz.self = (void *)&obj_vsm_ppg1_512hz;
    #ifdef MT2511_USE_IRQ
    obj_vsm_ppg1_512hz.polling = 0; // interrupt mode
    #else
    obj_vsm_ppg1_512hz.polling = 1; // polling mode
    #endif
    obj_vsm_ppg1_512hz.sensor_operate = vsm_ppg1_512hz_operate;
    sensor_driver_attach(SENSOR_TYPE_BIOSENSOR_PPG1_512HZ, &obj_vsm_ppg1_512hz);

    return err;
}
