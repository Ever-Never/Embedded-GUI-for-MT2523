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

#include "yas533.h"
#include "yas_drv.h"
#include "yas_algo.h"
#include "stdio.h"
#include <string.h> // memset
#include "mems_bus.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sensor_alg_interface.h"
#include "sensor_manager.h" /* sensor_driver_get_ms_tick */
#include "memory_attribute.h"

#define VENDOR_ID        (0x0499)
#define DEVICE_ID        (0x0050)
//#define DEVICE_ID        (0x003d)
//#define DEVICE_ID        (0x0045)
#define SLAVE_ADDR        (0x2e)

/*syslog */
#define LOGI(fmt,arg...)   LOG_I(sensor, "YAS: "fmt,##arg)
#define LOGE(fmt,arg...)   LOG_E(sensor, "YAS: "fmt,##arg)
/*printf*/
//#define LOGI(fmt,arg...)   printf("YAS:INFO: "fmt,##arg)
//#define LOGE(fmt,arg...)   printf("YAS:ERROR: "fmt,##arg)

//#define YAS_LOG_MSG
#ifdef YAS_LOG_MSG
#define LOGD(fmt,arg...)   LOG_I(sensor, "YAS: "fmt,##arg)
#else
#define LOGD(fmt,arg...)
#endif

struct yas_mag_t {
    struct yas_algo_state state;
    int8_t hard_offset[3];
    int position;
    int delay;
};

static struct yas_mag_t t_yas_mag;

//#define MAG_PROFLING
#ifdef MAG_PROFLING
#include "utils.h"
static int dataindex = 0;
static uint64_t starttime = 0;
static uint64_t endtime = 0;
uint32_t t1 = 0;
uint32_t t2 = 0;
uint32_t t3 = 0;
uint32_t delaycount = 0;
#endif

static int device_open(int32_t type)
{
    /* user-written implementation-specific source code */

    return YAS_NO_ERROR;
}

static int device_close(int32_t type)
{
    /* user-written implementation-specific source code */

    return YAS_NO_ERROR;
}

uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) yas_txbuffer[2];

static int device_write(int32_t type, uint8_t addr, const uint8_t *buf, int len)
{
    /* user-written implementation-specific source code */

    // burst write is not supported for YAS533
    int32_t ret;

    yas_txbuffer[0] = addr;
    yas_txbuffer[1] = *buf;

    ret = mems_i2c_write(YAS533_I2C_ADDRESS, yas_txbuffer, (len * 2));

    if ( HAL_I2C_STATUS_OK != ret) {
        LOGE("I2C Trasmit error\n");
        return -1;
    }

    return YAS_NO_ERROR;
}

#define MAG_MAX_READ_LEN 16
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) yas_reg_addr;
uint8_t ATTR_ZIDATA_IN_NONCACHED_RAM __attribute__ ((aligned(4))) yas_rxbuffer[MAG_MAX_READ_LEN];

static int device_read(int32_t type, uint8_t addr, uint8_t *buf, int len)
{
    /* user-written implementation-specific source code */
    int32_t ret;

    if (len > MAG_MAX_READ_LEN) {
        assert(0);
    }

    yas_reg_addr  = addr;
    ret = mems_i2c_write(YAS533_I2C_ADDRESS, &yas_reg_addr, 1);
    if ( HAL_I2C_STATUS_OK != ret) {
        LOGE("I2C Trasmit error\n");
        return -1;
    }

    ret = mems_i2c_read(YAS533_I2C_ADDRESS, yas_rxbuffer, len);
    if ( HAL_I2C_STATUS_OK != ret) {
        LOGE("I2C Receive error\n");
        return -1;
    }
    memcpy(buf, yas_rxbuffer, len);

    return 0;
}

static uint32_t device_current_time(void)
{
    /* user-written implementation-specific source code */
    return sensor_driver_get_ms_tick();

}

static void device_usleep(int usec)
{
    /* user-written implementation-specific source code */
    vTaskDelay(1 + usec / 1000 / portTICK_RATE_MS); // sleep in 1 ms unit

#ifdef MAG_PROFLING
    delaycount++;
#endif
}

int yas533_sensor_init(void)
{
    struct yas_driver_callback cbk;
    int rt = 0;
#ifdef YAS_LOG_MSG
    int i = 0;
#endif

    cbk.device_open = device_open;
    cbk.device_close = device_close;
    cbk.device_write = device_write;
    cbk.device_read = device_read;
    cbk.usleep = device_usleep;
    cbk.current_time = device_current_time;

    rt += yas_driver_init(&cbk);
    LOGI("yas_driver_init[%d]\n", rt);

    rt += yas_driver_set_position(YAS_TYPE_MAG, 3);
    LOGI("yas_driver_set_position[%d]\n", rt);

    rt += yas_driver_set_delay(YAS_TYPE_MAG, 10 /* msec */);
    LOGI("yas_driver_set_delay[%d]\n", rt);

    return rt;
}

int yas533_sensor_deinit(void)
{
    int rt;

    static int8_t hard_offset[3];
    struct yas_algo_state state;

    rt = yas_driver_set_enable(YAS_TYPE_MAG, 0);
    LOGD("yas_driver_set_enable[%d]\n", rt);

    /* Save the current hardware offset to storage (YAS532/YAS533) */
#if YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS532 \
    || YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS533
    rt = yas_driver_ext(YAS_TYPE_MAG, YAS532_GET_HW_OFFSET, hard_offset);
    LOGD("yas_driver_ext[%d]"
         "\thard_offset[%d %d %d]\n", rt,
         hard_offset[0], hard_offset[1], hard_offset[2]);
#endif

    /* Save the current state to storage */
    rt = yas_algo_get_state(&state);
    if (rt < 0) {
        LOGE("yas_algo_get_state error(%d)\n", rt);
    }
    LOGD("yas_algo_get_state[%d]"
         "\tcalibration offset[%ld %ld %ld] accuracy[%d]\n", rt,
         state.offset[yas_magnetic].v[0],
         state.offset[yas_magnetic].v[1],
         state.offset[yas_magnetic].v[2],
         state.accuracy[yas_magnetic]);

    rt = yas_driver_term();
    if (rt < 0) {
        LOGE("yas_algo_get_state error(%d)\n", rt);
    }
    LOGD("yas_driver_term[%d]\n", rt);

    rt = yas_algo_term();
    if (rt < 0) {
        LOGE("yas_algo_get_state error(%d)\n", rt);
    }
    LOGD("yas_algo_term[%d]\n", rt);

    return 1;
}

/*--------------------------------------------------------------------------------*/
int32_t mag_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actualout,
                    void *buff_in, int32_t size_in)
{
    int err = 0;
    int value = 0;
    /* The static ellipsoidal calibration matrix.  Fill this with
     * Yamaha-provided values. */
    struct yas_matrix static_matrix = { {10000, 0, 0, 0, 10000, 0, 0, 0, 10000} };
    struct yas_data uncal, cal_and_filtered;

    switch (command) {
        case SENSOR_DELAY:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("Mag SENSOR_DELAY parameter error! \r\n");
                err = -1;
            } else {
                value = *(int *)buff_in;
                LOGI("Mag SENSOR_DELAY ms (%d) \r\n", value);
                err = yas_driver_set_delay(YAS_TYPE_MAG, value /* msec */);

                if (err < 0) {
                    LOGE("Mag SENSOR_DELAY error(%d) \r\n", err);
                } else {
                    t_yas_mag.delay = value;
                }
            }
            break;

        case SENSOR_ENABLE:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("Mag Enable sensor parameter error! \r\n");
                err = -1;
            } else {
                value = *(int *)buff_in;
                if (value) {
                    yas533_sensor_init();
                    err = yas_driver_set_enable(YAS_TYPE_MAG, 1);
                    LOGI("yas_driver_set_enable[%d] \r\n", err);

                    err = yas_algo_init();
                    LOGI("yas_algo_init[%d]\n", err);

                    err = yas_algo_set_calib_enable(YAS_TYPE_MAG, 1);
                    LOGI("yas_algo_set_calib_enable[%d] \r\n", err);

                    err = yas_algo_set_filter_enable(YAS_TYPE_MAG, 1);
                    LOGI("yas_algo_set_filter_enable[%d] \r\n", err);

                    /* Set the static matrix if needed */
#if YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS532 \
                    || YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS533
                    err = yas_driver_ext(YAS_TYPE_MAG, YAS532_SET_STATIC_MATRIX, &static_matrix);
#else
                    err = yas_driver_ext(YAS_TYPE_MAG, YAS537_SET_STATIC_MATRIX, &static_matrix);
                    err = yas_driver_ext(YAS_TYPE_MAG, YAS537_GET_STATIC_MATRIX, &static_matrix);
#endif
                    /* Restore the previous hardware offset from storage (YAS532/YAS533) */
#if YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS532 \
                    || YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS533
                    err = yas_driver_ext(YAS_TYPE_MAG, YAS532_SET_HW_OFFSET, t_yas_mag.hard_offset);
                    LOGI("yas_driver_ext[%d]\n", err);
#endif
                    err = yas_algo_set_state(&(t_yas_mag.state));
                    LOGI("yas_algo_set_state[%d]\n", err);
                } else {
                    err = yas533_sensor_deinit();
                    if (err < 0) {
                        LOGI("yas_algo_init[%d] \r\n", err);
                    }
                }
            }
            break;

        case SENSOR_GET_DATA: // // output unit is mili-degree/s
#ifdef MAG_PROFLING
            starttime = timestamp_get_ns();
#endif
            err = yas_driver_measure(YAS_TYPE_MAG, &uncal, 1);
            if (err < 0) {
                LOGI("yas_driver_measure error[%d] \r\n", err);
                *actualout = 0;
                break;
            }
#ifdef MAG_PROFLING
            endtime = timestamp_get_ns();
            t1 = (endtime - starttime) / 1000;
            starttime = endtime;
#endif
            err = yas_algo_update(&uncal, &cal_and_filtered, 1);
            if (err < 0) {
                LOGI("yas_algo_update[%d]\n", err);
                *actualout = 0;
                break;
            }
#ifdef MAG_PROFLING
            endtime = timestamp_get_ns();
            t2 = (endtime - starttime) / 1000;
            starttime = endtime;
#endif
            *actualout = sizeof(sensor_data_unit_t);
            sensor_data_unit_t *start = (sensor_data_unit_t *)buff_out;
            start->magnetic_t.x = cal_and_filtered.xyz.v[0];
            start->magnetic_t.y = cal_and_filtered.xyz.v[1];
            start->magnetic_t.z = cal_and_filtered.xyz.v[2];
            start->magnetic_t.status = cal_and_filtered.accuracy;

            // handle saving accuracy >= 2 case!
            if (cal_and_filtered.accuracy == 3) {
                /* Save the current hardware offset to storage (YAS532/YAS533) */
#if YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS532 \
                || YAS_MAG_DRIVER == YAS_MAG_DRIVER_YAS533
                err = yas_driver_ext(YAS_TYPE_MAG, YAS532_GET_HW_OFFSET, t_yas_mag.hard_offset);
#endif

                /* Save the current state to storage */
                err = yas_algo_get_state(&(t_yas_mag.state));
                if (err < 0) {
                    LOGE("yas_algo_get_state error(%d)\n", err);
                }
            }
#ifdef MAG_PROFLING
            endtime = timestamp_get_ns();
            t3 = (endtime - starttime) / 1000;
            if (delaycount > 1) {
                LOGI("delaycount(%ld), measure(%ld), algo_update(%ld), ext(%ld) \r\n", delaycount, t1, t2, t3);
            }
            delaycount = 0;
            dataindex++;
#endif

            break;

        case SENSOR_CUST:
            if ((buff_in == NULL) || (size_in < sizeof(int))) {
                LOGE("Mag SENSOR_CUST parameter error! \r\n");
                err = -1;
            } else {
                LOGI("hard_offset[%d %d %d]\n",
                     t_yas_mag.hard_offset[0], t_yas_mag.hard_offset[1], t_yas_mag.hard_offset[2]);
                LOGI("calibration offset[%ld %ld %ld] accuracy[%d]\n",
                     t_yas_mag.state.offset[yas_magnetic].v[0],
                     t_yas_mag.state.offset[yas_magnetic].v[1],
                     t_yas_mag.state.offset[yas_magnetic].v[2],
                     t_yas_mag.state.accuracy[yas_magnetic]);
            }
            break;

        default:
            LOGE("operate function no this parameter %ld! \r\n", command);
            err = -1;
            break;
    }

    return err;
}

const uint8_t offset_save[19] = {0xFD, 0xF9, 0x09, 0x10, 0x40, 0x00, 0x00, 0x10, \
                                 0x72, 0x00, 0x00, 0xB0, 0x68, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00
                                };

void mag_init(void)
{
    sensor_driver_object_t obj_mag;
    int ret;

    t_yas_mag.delay = 20;
    t_yas_mag.position = 3;
    memset(&(t_yas_mag.state), 0, sizeof(struct yas_algo_state));
    t_yas_mag.hard_offset[0] = 127;
    t_yas_mag.hard_offset[1] = 127;
    t_yas_mag.hard_offset[2] = 127;

    //  get yas53x offset from file
    memcpy(t_yas_mag.hard_offset, offset_save, sizeof(t_yas_mag.hard_offset));
    memcpy(&(t_yas_mag.state), offset_save + sizeof(t_yas_mag.hard_offset), \
           sizeof(t_yas_mag.state));

    LOGI("Hard offset: %d, %d, %d, State: %ld, %ld, %ld, %d \r\n", t_yas_mag.hard_offset[0], \
         t_yas_mag.hard_offset[1], t_yas_mag.hard_offset[2], (t_yas_mag.state.offset[0].v[0]), \
         (t_yas_mag.state.offset[0].v[1]), (t_yas_mag.state.offset[0].v[2]), t_yas_mag.state.accuracy[0]);

    obj_mag.self = (void *)&obj_mag;
    obj_mag.polling = 1; // polling mode
    obj_mag.sensor_operate = mag_operate;
    sensor_driver_attach(SENSOR_TYPE_MAGNETIC_FIELD, &obj_mag);
}
