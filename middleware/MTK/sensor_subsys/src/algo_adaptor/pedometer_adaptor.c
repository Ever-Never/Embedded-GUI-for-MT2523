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
#include "pedometer.h"
#include "algo_adaptor/algo_adaptor.h"
#include "FreeRTOS.h"

#if defined(FOTA_UBIN_DECOUPLING)
#include "pedometer_wrapper.h"
#endif

/* syslog */
#define LOGI(fmt,arg...)   LOG_I(sensor, "[Pedometer]: "fmt,##arg)
#define LOGE(fmt,arg...)   LOG_E(sensor, "[Pedometer]: "fmt,##arg)
/* printf*/
//#define LOGE(fmt, args...)    printf("[Pedometer] ERR: "fmt, ##args)
//#define LOGD(fmt, args...)    printf("[Pedometer] DBG: "fmt, ##args)


static sensor_input_list_t input_comp_acc;
typedef struct pedometer_adaptor_t {
    uint32_t time_stamp;
    uint32_t step_length;
    uint32_t prev_pedometer_result;
}pedometer_adaptor_t;

static pedometer_adaptor_t pedometer_adaptor = {0, 0, 0};
static resampling_t pedometer_resampling = {0, 0, 0, 0, 0};

static int32_t get_pedometer_result(sensor_data_t *const output)
{
    //calculate output;
    sensor_data_unit_t *pedometer_data = output->data;
    pedometer_data->sensor_type = SENSOR_TYPE_PEDOMETER;
    pedometer_data->time_stamp = pedometer_adaptor.time_stamp;
    pedometer_data->pedometer_t.accumulated_step_count = get_pedometer_final_step_count();
    pedometer_data->pedometer_t.step_type = get_pedomter_step_type();
    return 1;
}

static int32_t pedometer_process_data(const sensor_data_t *input_list, void *reserve)
{
    uint32_t input_time_stamp = input_list->data->time_stamp;
    uint32_t count = input_list->data_exist_count;
    uint32_t pedometer_step_count;
    sensor_data_unit_t *data_start = input_list->data;
    int32_t acc_X, acc_Y, acc_Z;
    if (data_start->sensor_type == SENSOR_TYPE_ACCELEROMETER) {

        if (!pedometer_resampling.init_flag) {
            pedometer_resampling.last_time_stamp = input_time_stamp;
            pedometer_resampling.init_flag = 1;
        }

        while (count != 0) {
            input_time_stamp = data_start->time_stamp;
            // resampling
            pedometer_resampling.current_time_stamp = input_time_stamp;

            sensor_subsys_algorithm_resampling_type(&pedometer_resampling);
            pedometer_adaptor.time_stamp = pedometer_resampling.last_time_stamp;
            acc_X = data_start->accelerometer_t.x;
            acc_Y = data_start->accelerometer_t.y;
            acc_Z = data_start->accelerometer_t.z;
        	#if SET_ADAPTOR_103
        		//Ideally we should use this one, since this is the entry point of from Driver->Algorithm
        		acc_X = ((acc_X*8445) >> 13);  // multiply by 1.031 to stick with the mistake of 2G saturate Driver magnitude
        		acc_Y = ((acc_Y*8445) >> 13);  //
        		acc_Z = ((acc_Z*8445) >> 13);  //
        	#endif
            while (pedometer_resampling.input_count > 0) {
                pedometer_adaptor.time_stamp += pedometer_resampling.input_sample_delay;
                set_pedometer_t_s(pedometer_adaptor.time_stamp);
                pedometer_detector(acc_X, acc_Y, acc_Z);
                pedometer_resampling.input_count--;
                pedometer_resampling.last_time_stamp = pedometer_resampling.current_time_stamp;
            }
            data_start++;
            count--;
        }
        // notify to get results if the detected step increases
        pedometer_step_count = get_pedometer_final_step_count();
        if (pedometer_step_count != pedometer_adaptor.prev_pedometer_result) {
            pedometer_adaptor.prev_pedometer_result = pedometer_step_count;
            sensor_fusion_algorithm_notify(SENSOR_TYPE_PEDOMETER);
        }
    }
    return 1;
}

static int32_t pedometer_operate(int32_t command, void *buffer_out, int32_t size_out, \
                                 void *buffer_in, int32_t size_in)
{
    int32_t err = 0;
    switch (command) {
        case PEDOMETER_STEP_LENGTH:
            if ((buffer_in == NULL) || size_in != sizeof(uint32_t)) {
                LOGE("PEDOMETER_STEP_LENGTH parameter error \r\n");
                err = -1;
            } else {
                pedometer_adaptor.step_length = *((uint32_t *)buffer_in);
            }
            break;
        case PEDOMETER_INFO_RESET:
            pedometer_algorithm_init();
            break;
        default:
            LOGE("operate function no this parameter %d! \r\n", command);
            err = -1;
            break;
    }
    return err;
}

const sensor_descriptor_t pedometer_desp = {
    SENSOR_TYPE_PEDOMETER, //output_type
    1, /* version */
    SENSOR_REPORT_MODE_ON_CHANGE, /* report_mode */
    &input_comp_acc, /* sensor_input_list_t */
    pedometer_operate,
    get_pedometer_result,
    pedometer_process_data,
    250 /* accumulate */
};

int pedometer_register(void)
{
    int ret; /*return: fail=-1, pass>=0, which means the count of current register algorithm */

    input_comp_acc.input_type = SENSOR_TYPE_ACCELEROMETER;
    input_comp_acc.sampling_delay = PEDOMETER_INPUT_SAMPLE_INTERVAL;
    input_comp_acc.timeout = PEDOMETER_INPUT_POLLING_INTERVAL;
    input_comp_acc.next_input = NULL;

    ret = sensor_fusion_algorithm_register_type(&pedometer_desp);
    if (ret < 0) {
        LOGE("fail to register Pedometer \r\n");
    }
    ret = sensor_fusion_algorithm_register_data_buffer(SENSOR_TYPE_PEDOMETER, 1);
    if (ret < 0) {
        LOGE("fail to register buffer \r\n");
    }
    return ret;
}

int pedometer_init(void)
{
    pedometer_algorithm_init();
    pedometer_adaptor.time_stamp = 0;
    pedometer_resampling.init_flag = 0;
    pedometer_resampling.input_sample_delay = PEDOMETER_INPUT_SAMPLE_INTERVAL;
    pedometer_adaptor.prev_pedometer_result = get_pedometer_final_step_count();
    return 1;
}

int pedometer_deinit(void)
{
    return 1;
}

