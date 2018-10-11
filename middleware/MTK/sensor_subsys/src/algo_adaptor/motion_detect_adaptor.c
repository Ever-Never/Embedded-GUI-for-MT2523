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

#include "sensor_alg_interface.h"
#include "stdio.h"
#include "algo_adaptor/algo_adaptor.h"
#include "FreeRTOS.h"

/* syslog */
//#define LOGI(fmt,arg...)   LOG_I(sensor, "[motion Detect]: "fmt,##arg)
//#define LOGE(fmt,arg...)   LOG_E(sensor, "[motion Detect]: "fmt,##arg)
/* printf*/
//#define LOGE(fmt, args...)    printf("[motion Detect] ERR: "fmt, ##args)
//#define LOGD(fmt, args...)    printf("[motion Detect] DBG: "fmt, ##args)

#define G_SENSOR_INPUT_ACCUMULATE 0
#define G_SENSOR_INPUT_POLLING_INTERVAL  200
#define G_SENSOR_INPUT_SAMPLE_INTERVAL   8

/*DEFINE DEPEND ON MODULE*/
#define G_SENSOR_MOTION_CHECK_THRESHOLD           160
#define G_SENSOR_MOTION_WAKEUP_CHECK_COUNT_MAX    2
#define G_SENSOR_SAMPLE_RATE                      1

#define ACC_ABS(x) ((x) > 0 ? (x) : -(x))

int g_sensor_motion_detection(int32_t acc_x,int32_t acc_y,int32_t acc_z);
int32_t g_sensor_initial_flag = 0;
int32_t g_sensor_motion_detect_x = 0; /**<Value of the X-axis.*/
int32_t g_sensor_motion_detect_y = 0; /**<Value of the Y-axis.*/
int32_t g_sensor_motion_detect_z = 0; /**<Value of the Z-axis.*/
int32_t g_sensor_motion_detect_count = 0;
int32_t g_sensor_motion_check_timestmap = 0;

typedef struct motion_detect_adaptor_t {
    uint32_t time_stamp;
    int state;
    int result;
} motion_detect_adaptor_t;

static sensor_input_list_t input_comp_acc;
static motion_detect_adaptor_t motion_detect_adaptor;
static resampling_t motion_detect_resampling = {0, 0, 0, 0, 0};

static int32_t get_motion_detect_result(sensor_data_t *const output)
{
    //calculate output;
    sensor_data_unit_t *motion_detect_data = output->data;
    motion_detect_data->sensor_type = SENSOR_TYPE_MOTION_DETECT;
    motion_detect_data->time_stamp = motion_detect_adaptor.time_stamp;
    motion_detect_data->value[0]= motion_detect_adaptor.result;
    return 1;
}

static int32_t motion_detect_process_data(const sensor_data_t *input_list, void *reserve)
{
    uint32_t input_time_stamp = input_list->data->time_stamp;
    uint32_t count = input_list->data_exist_count;
    sensor_data_unit_t *data_start = input_list->data;
    int32_t acc_X, acc_Y, acc_Z;

    if (data_start->sensor_type == SENSOR_TYPE_ACCELEROMETER ) {

        if (!motion_detect_resampling.init_flag) {
            motion_detect_resampling.last_time_stamp = input_time_stamp;
            motion_detect_resampling.init_flag = 1;
        }

        while (count != 0) {
            input_time_stamp = data_start->time_stamp;
            // resampling
            motion_detect_resampling.current_time_stamp = input_time_stamp;

            sensor_subsys_algorithm_resampling_type(&motion_detect_resampling);
            motion_detect_adaptor.time_stamp = motion_detect_resampling.last_time_stamp;
            acc_X = data_start->accelerometer_t.x;
            acc_Y = data_start->accelerometer_t.y;
            acc_Z = data_start->accelerometer_t.z;

            while (motion_detect_resampling.input_count > 0) {
                motion_detect_adaptor.time_stamp += motion_detect_resampling.input_sample_delay;
                motion_detect_adaptor.result = g_sensor_motion_detection(acc_X, acc_Y, acc_Z);
                motion_detect_resampling.input_count--;
                motion_detect_resampling.last_time_stamp = motion_detect_resampling.current_time_stamp;
            }
            data_start++;
            count--;
        }

        if (motion_detect_adaptor.result) {
            sensor_fusion_algorithm_notify(SENSOR_TYPE_MOTION_DETECT);
        }
    }
    return 1;
}

static int32_t motion_detect_operate(int32_t command, void *buffer_out, int32_t size_out, \
                                     void *buffer_in, int32_t size_in)
{
    return 0;
}

const sensor_descriptor_t motion_detect_desp = {
    SENSOR_TYPE_MOTION_DETECT , //output_type
    1, /* version */
    SENSOR_REPORT_MODE_ON_CHANGE, /* report_mode */
    &input_comp_acc, /* sensor_input_list_t */
    motion_detect_operate,
    get_motion_detect_result,
    motion_detect_process_data,
    G_SENSOR_INPUT_ACCUMULATE /* accumulate */
};

int motion_detect_register(void)
{
    int ret; /*return: fail=-1, pass>=0, which means the count of current register algorithm */

    input_comp_acc.input_type = SENSOR_TYPE_ACCELEROMETER;
    input_comp_acc.sampling_delay = G_SENSOR_INPUT_SAMPLE_INTERVAL;
    input_comp_acc.timeout = G_SENSOR_INPUT_POLLING_INTERVAL;
    input_comp_acc.next_input = NULL;

    ret = sensor_fusion_algorithm_register_type(&motion_detect_desp);
    if (ret < 0) {
        //     LOGE("fail to register g sensor \r\n");
    }
    ret = sensor_fusion_algorithm_register_data_buffer(SENSOR_TYPE_MOTION_DETECT , 1);
    if (ret < 0) {
        //     LOGE("fail to register buffer \r\n");
    }
    return ret;
}

int motion_detect_init(void)
{
    motion_detect_resampling.init_flag = 0;
    motion_detect_resampling.input_sample_delay = G_SENSOR_INPUT_SAMPLE_INTERVAL;
    motion_detect_adaptor.time_stamp = 0;
    motion_detect_adaptor.result = 0;
    return 1;
}

int g_sensor_motion_detection(int32_t acc_x,int32_t acc_y,int32_t acc_z)
{
    int enable_hr_measure_mode = 0;

    g_sensor_motion_check_timestmap++;
    if(g_sensor_motion_check_timestmap == G_SENSOR_SAMPLE_RATE) {
        g_sensor_motion_check_timestmap = 0;
        if(g_sensor_initial_flag == 1 ) {
            if((ACC_ABS(acc_x-g_sensor_motion_detect_x) >= G_SENSOR_MOTION_CHECK_THRESHOLD)
                    ||(ACC_ABS(acc_y-g_sensor_motion_detect_y) >= G_SENSOR_MOTION_CHECK_THRESHOLD)
                    ||(ACC_ABS(acc_z-g_sensor_motion_detect_z) >= G_SENSOR_MOTION_CHECK_THRESHOLD)) {
                g_sensor_motion_detect_count++;
            } else {
                g_sensor_motion_detect_count = 0;
            }

            g_sensor_motion_detect_x = acc_x;
            g_sensor_motion_detect_y = acc_y;
            g_sensor_motion_detect_z = acc_z;

            if(g_sensor_motion_detect_count > G_SENSOR_MOTION_WAKEUP_CHECK_COUNT_MAX) {
                g_sensor_motion_detect_count=0;
                g_sensor_motion_check_timestmap = 0;
                g_sensor_initial_flag = 0;
                enable_hr_measure_mode = 1;
            }
        }

        if(g_sensor_initial_flag == 0) { //get init g-sensor X.Y.Z value
            g_sensor_initial_flag = 1;
            g_sensor_motion_detect_x = acc_x;
            g_sensor_motion_detect_y = acc_y;
            g_sensor_motion_detect_z = acc_z;
            g_sensor_motion_detect_count = 0;
        }
    }
    return enable_hr_measure_mode;
}
