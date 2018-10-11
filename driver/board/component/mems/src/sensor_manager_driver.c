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

#include "stdio.h"
#include "string.h" // memcpy
#include "FreeRTOS.h"
#include "task.h"
#include "hal_gpt.h"
#include "queue.h" /*QueueHandle_t*/
#include "sensor_alg_interface.h"

/* syslog.h included in FreeRTOSConfig.h in FreeRTOS.h*/
log_create_module(sensor, PRINT_LEVEL_INFO);

/* syslog */
#define LOGE(fmt,arg...)   LOG_E(sensor, "SMD: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(sensor ,"SMD: "fmt,##arg)
#define LOGD(fmt,arg...)

/*printf*/
//#define LOGE(fmt, args...)    printf("[SMD] ERR: "fmt, ##args)
//#define LOGI(fmt, args...)    printf("[SMD] INFO: "fmt, ##args)

QueueHandle_t sm_queue_handle;

struct sensor_driver_context { /*sensor context*/
    uint32_t enable;
    uint32_t delay;
    uint32_t timeout;
    sensor_driver_object_t obj;
};

static struct sensor_driver_context sdc[SENSOR_PHYSICAL_TYPE_ALL];
static uint32_t curr_tick;
static uint32_t curr_ms;
static uint32_t smd_tick_remainder;

int32_t sensor_driver_attach(uint32_t sensor_type, sensor_driver_object_t *obj)
{
    LOGI("sensor_driver_attach sensor_type= %ld \r\n", sensor_type);
    if (sensor_type >= SENSOR_PHYSICAL_TYPE_ALL) {
        return -1;
    }

    sdc[sensor_type].enable = 0;
    sdc[sensor_type].delay = 0x7FFFFFFF; //max delay
    sdc[sensor_type].timeout = 0;
    memcpy(&sdc[sensor_type].obj, obj, sizeof(sensor_driver_object_t));

    return 0;
}

int32_t sensor_driver_enable(uint32_t sensor_type, int32_t enable)
{
    int err = 0;

    if (sensor_type >= SENSOR_PHYSICAL_TYPE_ALL) {
        return -1;
    }
    if (NULL == sdc[sensor_type].obj.sensor_operate) {
        LOGD("sensor_driver_set_delay(%ld) null pointer\r\n", sensor_type);
        return -1;
    }

    // redundant check
    if ( sdc[sensor_type].enable == enable ) {
        return 0;
    }

    sdc[sensor_type].obj.sensor_operate(sdc[sensor_type].obj.self, \
                                        SENSOR_ENABLE, NULL, 0, NULL, &enable, sizeof(int32_t));

    if (err >= 0) {
        sdc[sensor_type].enable = enable;
    }

    return err;
}

/*------------get data--------------------------------------------------------*/
int32_t sensor_driver_get_data(uint32_t sensor_type, void *data_buf, int32_t size, int32_t *actual_out_size)
{
    int err = 0;

    if (sensor_type >= SENSOR_PHYSICAL_TYPE_ALL) {
        return -1;
    }
    if (NULL == sdc[sensor_type].obj.sensor_operate) {
        LOGD("sensor_driver_get_data(%ld) null pointer\r\n", sensor_type);
        return -1;
    }

    err = sdc[sensor_type].obj.sensor_operate(sdc[sensor_type].obj.self, SENSOR_GET_DATA,
            data_buf, size, actual_out_size, NULL, 0);

    if (err < 0) {
        LOGD("sensor_driver_get_data type(%ld) err(%d) \r\n", sensor_type, err);
    }

    return err;
}

/*------------set delay--------------------------------------------------------*/
// delay in ms unit
int32_t sensor_driver_set_delay(uint32_t sensor_type, int32_t delay)
{
    int err = 0;

    if (sensor_type >= SENSOR_PHYSICAL_TYPE_ALL) {
        return -1;
    }
    if (NULL == sdc[sensor_type].obj.sensor_operate) {
        LOGD("sensor_driver_set_delay(%ld) null pointer \r\n", sensor_type);
        return -1;
    }

    // redundant check
    if ( sdc[sensor_type].delay == delay ) {
        return 0;
    }

    err = sdc[sensor_type].obj.sensor_operate(sdc[sensor_type].obj.self, SENSOR_DELAY, \
            NULL, 0, NULL, &delay, sizeof(int32_t));

    if (err >= 0) {
        sdc[sensor_type].delay = delay;
    }

    return err;
}

int32_t sensor_driver_set_cust(uint32_t sensor_type, void *out_data_buf, int32_t out_size, int32_t *actual_out_size, \
                               void *in_data_buf, int32_t in_size)
{
    int err = 0;

    if (sensor_type >= SENSOR_PHYSICAL_TYPE_ALL) {
        return -1;
    }
    if (NULL == sdc[sensor_type].obj.sensor_operate) {
        LOGD("sensor_driver_set_cust(%ld) null pointer \r\n", sensor_type);
        return -1;
    }
    err = sdc[sensor_type].obj.sensor_operate(sdc[sensor_type].obj.self, SENSOR_CUST, \
            out_data_buf, out_size, actual_out_size, in_data_buf, in_size);
    return err;
}

int32_t sensor_driver_set_batch_timeout(uint32_t sensor_type, uint32_t timeout)
{
    int err = 0;

    if (sensor_type >= SENSOR_PHYSICAL_TYPE_ALL) {
        return -1;
    }
    if (NULL == sdc[sensor_type].obj.sensor_operate) {
        LOGD("sensor_driver_set_batch_timeout(%ld) null pointer \r\n", sensor_type);
        return -1;
    }

    // redundant check
    if ( sdc[sensor_type].timeout== timeout ) {
        return 0;
    }

    err = sdc[sensor_type].obj.sensor_operate(sdc[sensor_type].obj.self, SENSOR_BATCH, \
            NULL, 0, NULL, &timeout, sizeof(int32_t));

    if (err >= 0) {
        sdc[sensor_type].timeout= timeout;
    } else {
        LOGE("sensor type (%ld) not support batch \r\n", sensor_type);
    }

    return err;
}

uint32_t sensor_driver_get_enable(int sensor_type)
{
    return sdc[sensor_type].enable;
}

uint32_t sensor_driver_get_delay(int sensor_type)
{
    return sdc[sensor_type].delay;
}

uint32_t sensor_driver_get_batch_timeout(int sensor_type)
{
    return sdc[sensor_type].timeout;
}

int32_t sensor_driver_get_event_type(int sensor_type)
{
    return sdc[sensor_type].obj.polling;
}

int32_t sensor_driver_get_attached(int sensor_type)
{
    int32_t ret = -1;

    if (sdc[sensor_type].obj.self != NULL) {
        /* driver of ths sensor type attached */
        ret = 0;
    }
    return ret;
}

/* No thread safe. Use in task context. */
#include "hal_nvic_internal.h"
uint32_t sensor_driver_get_ms_tick(void)
{
    uint32_t new_curr_tick;
    hal_gpt_status_t ret;
    uint32_t mask = 0;
    uint32_t thisms;

    mask = save_and_set_interrupt_mask();

    ret = hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &new_curr_tick);
    if (ret != HAL_GPT_STATUS_OK) {
        LOGE("hal_gpt_get_free_run_count err (%d) \r\n", ret);
    }
    #if 0
    if (new_curr_tick - curr_tick > 3276800) {
        LOGI("new_curr_tick(%u), curr_tick(%u), diff(%d), curr_ms (%d), remainder(%d)\r\n", new_curr_tick, curr_tick, (new_curr_tick - curr_tick), curr_ms, smd_tick_remainder);
    }
    #endif
    curr_ms += (uint32_t)((((uint64_t)(new_curr_tick - curr_tick))*1000 + smd_tick_remainder)/32768);
    smd_tick_remainder = (uint32_t)((((uint64_t)(new_curr_tick - curr_tick))*1000 + smd_tick_remainder)%32768);

    //LOGI("new_curr_tick(%u), curr_tick(%u), diff(%d), curr_ms (%d), remainder(%d)\n", new_curr_tick, curr_tick, (new_curr_tick - curr_tick), curr_ms, smd_tick_remainder);
    curr_tick = new_curr_tick;
    thisms = curr_ms;
    restore_interrupt_mask(mask);

    return thisms;
}

#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
#endif /* __GNUC__ */

/* For platform dependent code. Should be overwritten in project main.c */
__weak void send_sensor_data_via_btspp(int32_t magic, int32_t sensor_type, int32_t x, int32_t y, int32_t z, int32_t status, int32_t time_stamp)
{
    return;
}


