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
#include "stdint.h"

#ifdef MT2523_WATCH_SENSOR_CALIBRATION
#include "sensor_transfer_info.h"
#endif

#if defined(FOTA_UBIN_DECOUPLING)
#include "blood_pressure_wrapper.h"
#include "ppg_control_wrapper.h"
#endif

#define BP_ENABLE 1
#define BP_GET_ECG_ERROR 1
#define BP_ECG_ERROR_INTERUPT 0
#define BP_WATCHBAND_TWICE 0
#define BP_ALG_CALIBRATION 1
#define BP_PWTT_VARIATION 1

//#define BP_MOTION_ENABLE
//#define BP_EXPERIMENT

#if BP_ENABLE
#include "bp_alg.h"
#include "nvdm.h"
#include <stdlib.h>
#endif

#if BP_GET_ECG_ERROR
static int32_t ecg_error_count;
static int32_t ecg_error_status;
#endif


// Revision History:
// v.1.0.0.1: 2016/03/16 init (Shu-Yu Hsu)
// v.1.0.0.2: 2016/05/20 add ppg_control (Shu-Yu Hsu)
// v.1.0.0.6: 2016/06/24 modify scenario for data collection experiments (Shu-Yu Hsu)
// v.1.0.0.7: 2016/07/04 modify scenario for data collection experiments (Shu-Yu Hsu)
// v.1.1.0.1: 2016/07/15 add realization of bp library (Chih-Ming Fu)
// v.1.1.0.2: 2016/07/27 modify pwtt output in bp library (Chih-Ming Fu)
// v.1.2.0.0: 2016/12/12 add calibration realization (Chih-Ming Fu)

// PPG control config
#if defined(MTK_SENSOR_BIO_USE_MT2511)
#define BP_PPG_CTRL_ON
#endif

#if defined(BP_PPG_CTRL_ON)
#include "vsm_driver.h"
#include "ppg_control.h"
#endif

#define CALIBRATION_MODE 1023
#define PERSONAL_MODE 127
#define GENERAL_MODE 0

#define NUM_CALIBRATION_DATA 18

#define EXPECT_ECG_LENGTH (512*1*25)
#define EXPECT_PPG_LENGTH (512*2*25)

#define BP_PPG_BUF_SIZE 10
#define BP_FS_PPG_HZ 512
#define BP_PPG_BITWIDTH 23

uint32_t bp_cnt_ecg;
uint32_t bp_cnt_ppg;
int32_t bp_switch;

int32_t process_data_first;

#if BP_ENABLE
extern void read_nvram_data(void);
extern void read_nvram_calibration_data(void);

#define MAX_NUM_PWTT_ONCE 10

#define BP_FEATURE_TYPE_CALIBRATION  3
#define BP_FEATURE_TYPE_NORMAL  0
int32_t FeatureType;

int32_t bp_data_loss_sensor_type;

bp_alg_out_t bp_output;
int32_t num_pwtt_out;
int32_t pwtt_out[MAX_NUM_PWTT_ONCE];
int32_t bp_height;
int32_t bp_weight;
int32_t bp_gender;
int32_t bp_age;
int32_t bp_handlen;
#endif

int32_t bp_output_calibration_data[7] = {1,2,3,4,5,6,7}; //20161125 init value for test use, actually calibration data is come from algo. //20170328 add mode

int32_t bp_input_calibration_data[NUM_CALIBRATION_DATA];

int32_t bp_mode;
int32_t bp_sbp1;
int32_t bp_dbp1;
int32_t bp_sbp2;
int32_t bp_dbp2;
int32_t bp_sbp3;
int32_t bp_dbp3;
int32_t bp_para1;
int32_t bp_para2;
int32_t bp_para3;
int32_t bp_para4;
int32_t bp_para5;
int32_t bp_para6;
int32_t bp_para7;
int32_t bp_para8;
int32_t bp_para9;
int32_t bp_para10;
int32_t bp_para11;
int32_t bp_para12;

#if BP_PWTT_VARIATION
#define BP_PWTT_VARIATION_TOOMUCH 1
#define BP_CAL_PWTT_VARIATION_TOOMUCH 2
int32_t bp_pwtt_data;
int32_t pwtt_out_value;
int32_t pwtt_out_value_avg;
int32_t bp_variation_count;
#endif

#ifdef MT2523_WATCH_SENSOR_CALIBRATION
uint32_t nvram_num = sizeof(nvram_ef_bp_info_struct);
nvram_ef_bp_info_struct bp_info;
#endif

#if defined(BP_PPG_CTRL_ON)
int32_t bp_ppg_buf[BP_PPG_BUF_SIZE];
int32_t bp_ppg_buf_cnt;
uint32_t bp_init_agc_flag;
#endif

#ifdef MT2511_ENABLE_LEADOFF
bus_data_t ecg_reg_info;
#endif

#ifdef MT2511_ENABLE_LEADOFF
bus_data_t ecg_reg_info;
#endif

/* syslog */
#define LOGI(fmt,arg...)   LOG_I(sensor, "[BP]: "fmt,##arg)
#define LOGE(fmt,arg...)   LOG_E(sensor, "[BP]: "fmt,##arg)
/* printf*/
//#define LOGE(fmt, args...)    printf("[BP] ERR: "fmt, ##args)
//#define LOGD(fmt, args...)    printf("[BP] DBG: "fmt, ##args)

#define BLOOD_PRESSURE_MONITOR_PPG1_512HZ_POLLING_TIME 64
#define BLOOD_PRESSURE_MONITOR_ECG_POLLING_TIME 64

static sensor_input_list_t input_blood_pressure_monitor_comp_ppg1;
static sensor_input_list_t input_blood_pressure_monitor_comp_ecg;

#if defined(BP_MOTION_ENABLE)
    #define BLOOD_PRESSURE_MONITOR_ACC_DELAY 8
    #define BLOOD_PRESSURE_MONITOR_ACC_POLLING_TIME 200
    static sensor_input_list_t input_blood_pressure_monitor_comp_acc;
#endif //#if defined(BP_MOTION_ENABLE)

int32_t bp_data_full;

//static uint32_t fusion_bp_prev_notify_time;

#if BP_GET_ECG_ERROR
static int32_t get_ecg_error(int32_t data_curr)
{
    static int32_t data_pre;
#ifdef MT2511_ENABLE_LEADOFF
    if(((uint32_t)ecg_reg_info.data_buf & 0x10000) == 0 ) {
        if ( (data_curr == 3932160) || (data_curr-data_pre< 10 && data_curr-data_pre>-10)  || data_curr-data_pre> 25000 || data_pre - data_curr> 25000) {
            ecg_error_count ++;
        }
    } else {
        if ( (data_curr-data_pre< 10 && data_curr-data_pre>-10)  || data_curr-data_pre> 25000 || data_pre - data_curr> 25000) {
            ecg_error_count ++;
        }
    }
#else
    if ( (data_curr-data_pre< 10 && data_curr-data_pre>-10)  || data_curr-data_pre> 25000 || data_pre - data_curr> 25000) {
        ecg_error_count ++;
    }
#endif
    data_pre = data_curr;
    if(ecg_error_count>1536) {
#ifdef MT2511_ENABLE_LEADOFF
        ecg_error_status = 1;
        sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
#endif
        return 1;
    } else {
        return 0;
    }
}
#endif

static int32_t get_blood_pressure_monitor_result(sensor_data_t *const output)
{
    if(output == NULL || output->data == NULL) {
        return -1;
    }

    sensor_data_unit_t *bp_data = output->data;


    bp_data->sensor_type = SENSOR_TYPE_BLOOD_PRESSURE_MONITOR;
    bp_data->blood_pressure_t.dbp = -3;
    bp_data->blood_pressure_t.sbp = -3;
    bp_data->blood_pressure_t.status = 3;
    LOG_I(sensor,"BPM = %d\n",bp_data->blood_pressure_t.bpm);
    LOG_I(sensor,"BP Data loss sensor type = %d\n", bp_data_loss_sensor_type);
    LOG_I(sensor,"BP PWTT Data = %d\n", bp_pwtt_data);
    LOG_I(sensor,"ECG ERROR status = %d\n", ecg_error_status);
    LOG_I(sensor,"Numbers of PWTT = %d\n", num_pwtt_out);
    LOG_I(sensor,"FeatureType = %d\n", FeatureType);
    //bp_data->blood_pressure_t.pwtt = NULL;
    //num_pwtt_out = 0;
    if(bp_data_loss_sensor_type  == SENSOR_TYPE_BIOSENSOR_PPG1_512HZ) {
        bp_data->blood_pressure_t.dbp = -6;
        bp_data->blood_pressure_t.sbp = -6;
        bp_data->blood_pressure_t.status = 6;
        bp_data->blood_pressure_t.pwtt = NULL;
        bp_data_loss_sensor_type  = 0;
        num_pwtt_out = 0;
    } else if (bp_data_loss_sensor_type  == SENSOR_TYPE_BIOSENSOR_EKG) {
        bp_data->blood_pressure_t.dbp = -7;
        bp_data->blood_pressure_t.sbp = -7;
        bp_data->blood_pressure_t.status = 7;
        bp_data->blood_pressure_t.pwtt = NULL;
        bp_data_loss_sensor_type  = 0;
        num_pwtt_out = 0;
    }
#if BP_PWTT_VARIATION
    else if (bp_pwtt_data == BP_PWTT_VARIATION_TOOMUCH) {
        bp_data->blood_pressure_t.dbp = -8;
        bp_data->blood_pressure_t.sbp = -8;
        bp_data->blood_pressure_t.status = 8;
        bp_data->blood_pressure_t.pwtt = NULL;
        bp_pwtt_data = 0;
        num_pwtt_out = 0;
    } else if (bp_pwtt_data == BP_CAL_PWTT_VARIATION_TOOMUCH) {
        bp_data->blood_pressure_t.dbp = -9;
        bp_data->blood_pressure_t.sbp = -9;
        bp_data->blood_pressure_t.status = 9;
        bp_data->blood_pressure_t.pwtt = NULL;
        bp_pwtt_data = 0;
        num_pwtt_out = 0;
    }
#endif
    else if (ecg_error_status)
#if BP_ENABLE
#if BP_GET_ECG_ERROR
    {
        bp_data->blood_pressure_t.dbp = -1;
        bp_data->blood_pressure_t.sbp = -1;
        bp_data->blood_pressure_t.bpm = -1;
        bp_data->blood_pressure_t.pwtt = NULL;
        bp_data->blood_pressure_t.numPwtt = 0;
        bp_data->blood_pressure_t.status = 1;
        bp_data->blood_pressure_t.feature_type = BP_FEATURE_TYPE_NORMAL;
        num_pwtt_out = 0;
    } else {
#endif
        if (num_pwtt_out!=0 ) {
            bp_data->blood_pressure_t.dbp = 0;
            bp_data->blood_pressure_t.sbp = 0;
            bp_data->blood_pressure_t.bpm = 0;
            bp_data->blood_pressure_t.pwtt = pwtt_out;
            bp_data->blood_pressure_t.numPwtt = num_pwtt_out;
            bp_data->blood_pressure_t.status = num_pwtt_out;
            bp_data->blood_pressure_t.feature_type = BP_FEATURE_TYPE_NORMAL;
            num_pwtt_out = 0;
        } else if (FeatureType == BP_FEATURE_TYPE_CALIBRATION) {
            bp_data->blood_pressure_t.dbp = 0;
            bp_data->blood_pressure_t.sbp = 0;
            bp_data->blood_pressure_t.bpm = 0;
            bp_data->blood_pressure_t.pwtt = bp_output_calibration_data;
            bp_data->blood_pressure_t.numPwtt = 7;
            bp_data->blood_pressure_t.status = 6;
            bp_data->blood_pressure_t.feature_type = BP_FEATURE_TYPE_CALIBRATION;
            num_pwtt_out = 0;
        } else if(num_pwtt_out==0) {
            bp_data->blood_pressure_t.dbp = bp_output.dbp;
            bp_data->blood_pressure_t.sbp = bp_output.sbp;
            bp_data->blood_pressure_t.bpm = bp_output.bpm;
            bp_data->blood_pressure_t.pwtt = NULL;
            bp_data->blood_pressure_t.numPwtt = 0;
            bp_data->blood_pressure_t.status = ecg_error_status;
            bp_data->blood_pressure_t.feature_type = BP_FEATURE_TYPE_NORMAL;
            num_pwtt_out = 0;
        }
    }
#endif
    return 1;
}

static int32_t blood_pressure_monitor_process_data(const sensor_data_t *input_list, void *reserve)
{
    int32_t ret = 1;
    int32_t count = input_list->data_exist_count;
    sensor_data_unit_t *data_start = input_list->data;
    uint32_t input_sensor_type = input_list->data->sensor_type;
//    uint32_t input_time_stamp = input_list->data->time_stamp;
    int32_t i;

#if defined(BP_PPG_CTRL_ON)
    ppg_control_t ppg1_ctrl_input;
    int32_t ppg_control_output[2];
#endif //#if defined(BP_PPG_CTRL_ON)

    // stop BP application when data is already 45-sec
    if (bp_cnt_ecg >= EXPECT_ECG_LENGTH && bp_cnt_ppg >= EXPECT_PPG_LENGTH) {
        /* notify to sensor manager collecting full data*/
        bp_data_full = 1;

        bp_switch=0;

        FeatureType = BP_FEATURE_TYPE_NORMAL;

        if (process_data_first) {
            process_data_first = 0;
            read_nvram_data();
            read_nvram_calibration_data(); //20161125 read init nvram data
            bp_output_calibration_data[6] = bp_mode;
            bp_alg_set_user_info(bp_age, bp_gender, bp_height, bp_weight, bp_handlen);
            if(bp_mode == PERSONAL_MODE) {
                bp_alg_set_calibration_data(bp_input_calibration_data, 18);
            }
        }
#if BP_ENABLE
        bp_alg_get_bp(&bp_output);
        bp_alg_get_calibration_data(bp_output_calibration_data,6);

        for(i=0; i<bp_output.num_pwtt; i++) {
            pwtt_out[num_pwtt_out] = bp_output.pwtt[i];
            num_pwtt_out++;
            if ( (num_pwtt_out==MAX_NUM_PWTT_ONCE-1) || (i==(bp_output.num_pwtt-1)) ) {
                sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
            }
        }
#endif

#if BP_PWTT_VARIATION
        if(bp_output.num_pwtt < 12) {
            if (bp_mode == CALIBRATION_MODE) {
                bp_pwtt_data = BP_CAL_PWTT_VARIATION_TOOMUCH;
            } else {
                bp_pwtt_data = BP_PWTT_VARIATION_TOOMUCH;
            }
            sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
        }
        for(i=0; i<bp_output.num_pwtt; i++) {
            pwtt_out_value += bp_output.pwtt[i];
        }
        pwtt_out_value_avg = pwtt_out_value / bp_output.num_pwtt;
        if(bp_mode == CALIBRATION_MODE) {
            for(i=0; i<bp_output.num_pwtt; i++) {
                if( bp_output.pwtt[i] < pwtt_out_value_avg * 0.8 ||  bp_output.pwtt[i] > pwtt_out_value_avg * 1.2) {
                    bp_variation_count++;
                }
                if (bp_variation_count > bp_output.num_pwtt * 0.1) {
                    bp_pwtt_data = BP_CAL_PWTT_VARIATION_TOOMUCH;
                    sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
                }
            }
        } else {
            for(i=0; i<bp_output.num_pwtt; i++) {
                if( bp_output.pwtt[i] < pwtt_out_value_avg * 0.7 ||  bp_output.pwtt[i] > pwtt_out_value_avg * 1.3) {
                    bp_variation_count++;
                }
                if (bp_variation_count > bp_output.num_pwtt * 0.3) {
                    bp_pwtt_data = BP_PWTT_VARIATION_TOOMUCH;
                    sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
                }
            }
        }
        pwtt_out_value = 0;
        pwtt_out_value_avg = 0;
        bp_variation_count = 0;
#endif

        FeatureType = BP_FEATURE_TYPE_CALIBRATION;
        sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);

        FeatureType = BP_FEATURE_TYPE_NORMAL;
        sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);

        /* prevent other signal not sending out*/
        bp_data_full = 0;
    }

    if(input_sensor_type == SENSOR_TYPE_BIOSENSOR_PPG1_512HZ) {

#if defined(BP_PPG_CTRL_ON)
        if(bp_cnt_ppg >= 10240) {
            bp_init_agc_flag = 1;
        }
#endif
        while (count != 0) {
#if BP_ENABLE
            if (bp_cnt_ppg%2==0) {
                bp_alg_process_data( data_start->bio_data.data, 1);
            }
            bp_cnt_ppg++;
#endif
#if defined(BP_PPG_CTRL_ON)
            if(bp_init_agc_flag == 0) {
                bp_ppg_buf[bp_ppg_buf_cnt] = data_start->bio_data.data;
                bp_ppg_buf_cnt++;

                if(bp_ppg_buf_cnt >= BP_PPG_BUF_SIZE) {
                    ppg1_ctrl_input.input = bp_ppg_buf;
                    ppg1_ctrl_input.input_fs = BP_FS_PPG_HZ;
                    ppg1_ctrl_input.input_length = bp_ppg_buf_cnt;
                    ppg1_ctrl_input.input_bit_width = BP_PPG_BITWIDTH;
                    ppg1_ctrl_input.input_config = 1;
                    ppg1_ctrl_input.input_source = 1;
                    ppg_control_process(&ppg1_ctrl_input, PPG_CONTROL_MODE_DUAL1, ppg_control_output);
                    bp_ppg_buf_cnt -= BP_PPG_BUF_SIZE;
                }
            }
#endif //#if defined(BP_PPG_CTRL_ON)

            data_start++;
            count--;

        }//while count

    } else if(input_sensor_type == SENSOR_TYPE_BIOSENSOR_EKG) {
        bp_cnt_ecg+=count;
        while (count != 0) {
#if BP_ENABLE
#if BP_GET_ECG_ERROR
            ecg_error_status = get_ecg_error(data_start->bio_data.data);
#if BP_ECG_ERROR_INTERUPT
            if (ecg_error_status==1) {
                sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
            }
#endif
#endif
            bp_alg_process_data( data_start->bio_data.data, 0);
#endif
            data_start++;
            count--;
        }
    }

    return ret;
}


static int32_t blood_pressure_monitor_operate(int32_t command, void *buffer_out, int32_t size_out, \
        void *buffer_in, int32_t size_in)
{
    /* To handle user input of age, sex, height, weight information. */
    return 0;
}

const sensor_descriptor_t blood_pressure_monitor_desp = {
    SENSOR_TYPE_BLOOD_PRESSURE_MONITOR, //output_type
    1, /* version */
    SENSOR_REPORT_MODE_ON_CHANGE, /* report_mode */
    &input_blood_pressure_monitor_comp_ppg1, /* sensor_input_list_t */
    blood_pressure_monitor_operate,
    get_blood_pressure_monitor_result,
    blood_pressure_monitor_process_data,
    0 /* accumulate */
};

int data_lost_callback(uint32_t sensor_type)
{
    bp_data_loss_sensor_type  = sensor_type;
    return 1;
}

int blood_pressure_monitor_register(void)
{
    int ret; /*return: fail=-1, pass>=0, which means the count of current register algorithm */

#if defined(BP_MOTION_ENABLE)
    input_blood_pressure_monitor_comp_acc.input_type = SENSOR_TYPE_ACCELEROMETER;
    input_blood_pressure_monitor_comp_acc.sampling_delay = BLOOD_PRESSURE_MONITOR_ACC_DELAY; // ms
    input_blood_pressure_monitor_comp_acc.timeout = BLOOD_PRESSURE_MONITOR_ACC_POLLING_TIME; // ms
#endif //#if defined(BP_MOTION_ENABLE)

    input_blood_pressure_monitor_comp_ppg1.input_type = SENSOR_TYPE_BIOSENSOR_PPG1_512HZ;
    input_blood_pressure_monitor_comp_ppg1.sampling_delay = (1000/512); // ms
    input_blood_pressure_monitor_comp_ppg1.timeout= BLOOD_PRESSURE_MONITOR_PPG1_512HZ_POLLING_TIME; // ms

    input_blood_pressure_monitor_comp_ecg.input_type = SENSOR_TYPE_BIOSENSOR_EKG;
    input_blood_pressure_monitor_comp_ecg.sampling_delay = (1000/512); // ms
    input_blood_pressure_monitor_comp_ecg.timeout = BLOOD_PRESSURE_MONITOR_ECG_POLLING_TIME; // ms

#if defined(BP_MOTION_ENABLE)
    input_blood_pressure_monitor_comp_ppg1.next_input = &input_blood_pressure_monitor_comp_acc; // build as a signal linked list
    input_blood_pressure_monitor_comp_acc.next_input = &input_blood_pressure_monitor_comp_ecg;
#else
    input_blood_pressure_monitor_comp_ppg1.next_input = &input_blood_pressure_monitor_comp_ecg; // build as a signal linked list
#endif
    input_blood_pressure_monitor_comp_ecg.next_input = NULL;

    ret = sensor_fusion_algorithm_register_type(&blood_pressure_monitor_desp);
    if (ret < 0) {
        LOGE("fail to register blood pressure monitor \r\n");
    }
    ret = sensor_fusion_algorithm_register_data_buffer(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR, 1);
    if (ret < 0) {
        LOGE("fail to register buffer \r\n");
    }
    return ret;
}

#if BP_ENABLE
void read_nvram_data()
{
#ifdef MT2523_WATCH_SENSOR_CALIBRATION
    nvdm_read_data_item("2511","bpInfo",(uint8_t *)&bp_info, &nvram_num);
    bp_height = bp_info.height;
    bp_weight = bp_info.weight;
    bp_gender = bp_info.gender;
    bp_age = bp_info.age;
#else
    uint8_t buf[11];
    uint32_t buf_size;

    nvdm_status_t nvdm_status;

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511","userid",(uint8_t*)buf,&buf_size);
    if(nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmuseridfail\n");
    } else {
        LOG_I(hal,"userid=%s\n",buf);
    }

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511","height",(uint8_t*)buf,&buf_size);
    if(nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmheightfail\n");
    } else {
        LOG_I(hal,"height=%s\n",buf);
    }
    bp_height = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511","weight",(uint8_t*)buf,&buf_size);
    if(nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmweightfail\n");
    } else {
        LOG_I(hal,"weight=%s\n",buf);
    }
    bp_weight = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511","gender",(uint8_t*)buf,&buf_size);
    if(nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmgenderfail\n");
    } else {
        LOG_I(hal,"gender=%s\n",buf);
    }
    bp_gender = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511","age",(uint8_t*)buf,&buf_size);
    if(nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmagefail\n");
    } else {
        LOG_I(hal,"age=%s\n",buf);
    }
    bp_age = atoi((char const *)buf);
#endif
}
#endif


void read_nvram_calibration_data()
{
#ifdef MT2523_WATCH_SENSOR_CALIBRATION
    nvdm_read_data_item("2511","bpInfo",(uint8_t *)&bp_info, &nvram_num);
    bp_mode = bp_info.bp_mode;
    bp_sbp1 = bp_info.calibrate_para[0];
    bp_dbp1 = bp_info.calibrate_para[1];
    bp_sbp2 = bp_info.calibrate_para[2];
    bp_dbp2 = bp_info.calibrate_para[3];
    bp_para1 = bp_info.calibrate_para[6];
    bp_para2 = bp_info.calibrate_para[7];
    bp_para3 = bp_info.calibrate_para[8];
    bp_para4 = bp_info.calibrate_para[9];
    bp_para5 = bp_info.calibrate_para[10];
    bp_para6 = bp_info.calibrate_para[11];
#if BP_WATCHBAND_TWICE
    bp_sbp3 = bp_info.calibrate_para[4];
    bp_dbp3 = bp_info.calibrate_para[5];
    bp_para7= bp_info.calibrate_para[12];
    bp_para8 = bp_info.calibrate_para[13];
    bp_para9 = bp_info.calibrate_para[14];
    bp_para10 = bp_info.calibrate_para[15];
    bp_para11 = bp_info.calibrate_para[16];
    bp_para12 = bp_info.calibrate_para[17];
#endif
#else
    uint8_t buf[11];
    uint32_t buf_size;
    nvdm_status_t nvdm_status;

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "mode", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmmodefail\n");
    } else {
        LOG_I(hal, "mode %s\n", buf);
    }
    bp_mode = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "sbp1", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmsbp1fail\n");
    } else {
        LOG_I(hal, "sbp1 %s\n", buf);
    }
    bp_sbp1 = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "dbp1", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmdbp1fail\n");
    } else {
        LOG_I(hal, "dbp1 %s\n", buf);
    }
    bp_dbp1 = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "sbp2", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmsbp2fail\n");
    } else {
        LOG_I(hal, "sbp2 %s\n", buf);
    }
    bp_sbp2 = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "dbp2", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmdbp2fail\n");
    } else {
        LOG_I(hal, "dbp2 %s\n", buf);
    }
    bp_dbp2 = atoi((char const *)buf);

#if BP_WATCHBAND_TWICE
    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "sbp3", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmsbp3fail\n");
    } else {
        LOG_I(hal, "sbp3 %s\n", buf);
    }
    bp_sbp3 = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "dbp3", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmdbp3fail\n");
    } else {
        LOG_I(hal, "dbp3 %s\n", buf);
    }
    bp_dbp3 = atoi((char const *)buf);
#endif

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para1", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmpara1fail\n");
    } else {
        LOG_I(hal, "para1 %s\n", buf);
    }
    bp_para1 = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para2", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmpara2fail\n");
    } else {
        LOG_I(hal, "para2 %s\n", buf);
    }
    bp_para2 = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para3", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmpara3fail\n");
    } else {
        LOG_I(hal, "para3 %s\n", buf);
    }
    bp_para3 = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para4", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmpara4fail\n");
    } else {
        LOG_I(hal, "para4 %s\n", buf);
    }
    bp_para4 = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para5", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmpara5fail\n");
    } else {
        LOG_I(hal, "para5 %s\n", buf);
    }
    bp_para5 = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para6", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmpara6fail\n");
    } else {
        LOG_I(hal, "para6 %s\n", buf);
    }
    bp_para6 = atoi((char const *)buf);

#if BP_WATCHBAND_TWICE
    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para7", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmpara7fail\n");
    } else {
        LOG_I(hal, "para7 %s\n", buf);
    }
    bp_para7 = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para8", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmpara8fail\n");
    } else {
        LOG_I(hal, "para8 %s\n", buf);
    }
    bp_para8 = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para9", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmpara9fail\n");
    } else {
        LOG_I(hal, "para9 %s\n", buf);
    }
    bp_para9 = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para10", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmpara10fail\n");
    } else {
        LOG_I(hal, "para10 %s\n", buf);
    }
    bp_para10 = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para11", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmpara11fail\n");
    } else {
        LOG_I(hal, "para11 %s\n", buf);
    }
    bp_para11 = atoi((char const *)buf);

    buf_size = sizeof(buf);
    nvdm_status = nvdm_read_data_item("2511", "para12", (uint8_t *)buf, &buf_size);
    if (nvdm_status != NVDM_STATUS_OK) {
        LOG_I(hal,"rednvdmpara12fail\n");
    } else {
        LOG_I(hal, "para12 %s\n", buf);
    }
    bp_para12 = atoi((char const *)buf);
#endif
#endif

#if BP_WATCHBAND_TWICE
    bp_input_calibration_data[0] = bp_sbp1;
    bp_input_calibration_data[1] = bp_dbp1;
    bp_input_calibration_data[2] = bp_sbp2;
    bp_input_calibration_data[3] = bp_dbp2;
    bp_input_calibration_data[4] = bp_sbp3;
    bp_input_calibration_data[5] = bp_dbp3;
    bp_input_calibration_data[6] = bp_para1;
    bp_input_calibration_data[7] = bp_para2;
    bp_input_calibration_data[8] = bp_para3;
    bp_input_calibration_data[9] = bp_para4;
    bp_input_calibration_data[10] = bp_para5;
    bp_input_calibration_data[11] = bp_para6;
    bp_input_calibration_data[12] = bp_para7;
    bp_input_calibration_data[13] = bp_para8;
    bp_input_calibration_data[14] = bp_para9;
    bp_input_calibration_data[15] = bp_para10;
    bp_input_calibration_data[16] = bp_para11;
    bp_input_calibration_data[17] = bp_para12;
#else  //20161205 set sbp3.dbp3.para7-12 to 0
    bp_input_calibration_data[0] = bp_sbp1;
    bp_input_calibration_data[1] = bp_dbp1;
    bp_input_calibration_data[2] = bp_sbp2;
    bp_input_calibration_data[3] = bp_dbp2;
    bp_input_calibration_data[4] = 0;
    bp_input_calibration_data[5] = 0;
    bp_input_calibration_data[6] = bp_para1;
    bp_input_calibration_data[7] = bp_para2;
    bp_input_calibration_data[8] = bp_para3;
    bp_input_calibration_data[9] = bp_para4;
    bp_input_calibration_data[10] = bp_para5;
    bp_input_calibration_data[11] = bp_para6;
    bp_input_calibration_data[12] = 0;
    bp_input_calibration_data[13] = 0;
    bp_input_calibration_data[14] = 0;
    bp_input_calibration_data[15] = 0;
    bp_input_calibration_data[16] = 0;
    bp_input_calibration_data[17] = 0;
#endif
}


int blood_pressure_monitor_init(void)
{
    bp_data_full = 0;
    bp_cnt_ecg = 0;
    bp_cnt_ppg = 0;
#if defined(BP_PPG_CTRL_ON)
    bp_init_agc_flag = 0;
    bp_ppg_buf_cnt = 0;
    // Call PPG control lib init
#ifdef MT2511_ENABLE_LEADOFF
    uint32_t data_buf;
    ecg_reg_info.reg  = 0x08;
    ecg_reg_info.addr = 0x33;
    ecg_reg_info.length = 4;
    ecg_reg_info.data_buf = &data_buf;
    vsm_driver_read_register(&ecg_reg_info);
#endif
    ppg_control_init();
    ppg_control_set_app(PPG_CONTROL_APP_BP);
#endif //#if defined(BP_PPG_CTRL_ON)
#if BP_ENABLE
    bp_output.sbp = 0;
    bp_output.dbp = 0;
    bp_output.bpm = 0;
    num_pwtt_out = 0;
#if BP_GET_ECG_ERROR
    ecg_error_count = 0;
#endif
    bp_alg_init();
    process_data_first = 1; //to fix bp data lost
#endif
    return 1;
}

