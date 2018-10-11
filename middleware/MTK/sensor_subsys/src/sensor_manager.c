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

#include "string.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

#include "sensor_alg_interface.h"
#include "sensor_manager.h"
#include "task_def.h"
#include "mems_init.h" /* bsp_sensor_peripheral_init */

#if (defined(FUSION_PEDOMETER_USE) && (FUSION_PEDOMETER_USE==M_INHOUSE_PEDOMETER)) \
    || (defined(FUSION_PEDOMETER_USE) && (FUSION_PEDOMETER_USE==ICM_PEDOMETER)) \
    || (defined(FUSION_HEART_RATE_MONITOR_USE) && (FUSION_HEART_RATE_MONITOR_USE==M_INHOUSE_HEART_RATE_MONITOR)) \
    || (defined(FUSION_SLEEP_TRACKER_USE) && (FUSION_SLEEP_TRACKER_USE==M_INHOUSE_SLEEP_TRACKER)) \
    || (defined(FUSION_SLEEP_STAGING_USE) && (FUSION_SLEEP_STAGING_USE==M_INHOUSE_SLEEP_STAGING)) \
    || (defined(FUSION_HEART_RATE_VARIABILITY_USE) && (FUSION_HEART_RATE_VARIABILITY_USE==M_INHOUSE_HEART_RATE_VARIABILITY)) \
    || (defined(FUSION_BLOOD_PRESSURE_USE) && (FUSION_BLOOD_PRESSURE_USE==M_INHOUSE_BLOOD_PRESSURE))
/* fusion algorithm adaptor */
#include "algo_adaptor/algo_adaptor.h"
#endif

#if (defined(FUSION_HEART_RATE_MONITOR_USE) && (FUSION_HEART_RATE_MONITOR_USE==M_INHOUSE_HEART_RATE_MONITOR))
  #ifdef HEART_RATE_MONITOR_OUT_PATTERN
    #include "20151103_11-41-01-933_2523_out.h"
    int golden_idx = 0;
    int golden_total = 0;
    uint32_t hr_last_update_time;
  #endif
#endif

#if (defined(FUSION_HEART_RATE_VARIABILITY_USE) && (FUSION_HEART_RATE_VARIABILITY_USE==M_INHOUSE_HEART_RATE_VARIABILITY))
  #ifdef HEART_RATE_VARIABILITY_OUT_PATTERN
    #include "output_hrv.h"
    uint32_t hrv_start_time;
    uint32_t hrv_reported;
  #endif
#endif

#if (defined(FUSION_BLOOD_PRESSURE_USE) && (FUSION_BLOOD_PRESSURE_USE==M_INHOUSE_HEART_RATE_VARIABILITY))
  #ifdef BLOOD_PRESSURE_OUT_PATTERN
    uint32_t bp_start_time;
    uint32_t bp_reported;
  #endif
#endif

#if (defined(FUSION_SLEEP_TRACKER_USE) && (FUSION_SLEEP_TRACKER_USE==M_INHOUSE_SLEEP_TRACKER))
  #ifdef SLEEP_TRACKER_OUT_PATTERN
    uint32_t st_start_time;
    uint32_t st_reported;
  #endif
#endif


/* syslog */
#define SM_ERR(fmt,arg...)   LOG_E(sensor, "SensorManager: "fmt,##arg)
#define SM_WARN(fmt,arg...)   LOG_W(sensor, "SensorManager: "fmt,##arg)
#ifndef SENSOR_MANAGER_LOG_DISABLE
#define SM_INFO(fmt,arg...)   LOG_I(sensor, "SensorManager: "fmt,##arg)
#else
#define SM_INFO(fmt,arg...)
#endif

/*printf*/
//#define SM_ERR(fmt, args...)    printf("[SensorManager] ERR: "fmt, ##args)
//#define SM_INFO(fmt, args...)    printf("[SensorManager] INFO: "fmt, ##args)

//#define SM_DEBUG
#ifdef SM_DEBUG
#define SM_DBG(fmt, args...)    LOG_I(sensor, "SensorManager: "fmt, ##args)
#else
#define SM_DBG(fmt, args...)
#endif

#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
#endif /* __GNUC__ */

//#define SDK_PESUDO_PEDOMETER
#define ACC_MAX_BUF_CNT 32
#define GYRO_MAX_BUF_CNT 8
#define ACTIVITY_MAX_BUF_CNT 10
#define GESTURE_MAX_BUF_CNT 10
#define BIO_MAX_BUF_CNT 128
#define BIO_PPG_BUF_CNT 192

typedef struct {
    uint32_t start_time;
    sensor_data_unit_t mgr_acc_data[ACC_MAX_BUF_CNT];
    uint32_t mgr_acc_delay;
    sensor_data_unit_t mgr_gyro_data[GYRO_MAX_BUF_CNT];
    uint32_t mgr_gyro_delay;
    sensor_data_unit_t mgr_mag_data;
    sensor_data_unit_t mgr_baro_data;
    sensor_data_unit_t mgr_proximity_data;
#ifdef MTK_SENSOR_BIO_USE_MT2511
    sensor_data_unit_t ppg1_data[BIO_PPG_BUF_CNT];
    sensor_data_unit_t ppg2_data[BIO_MAX_BUF_CNT];
    sensor_data_unit_t ekg_data[BIO_MAX_BUF_CNT];
    sensor_data_unit_t bisi_data[BIO_MAX_BUF_CNT];
#endif
    uint32_t data_updated; // bitmap for which physical sensor updated

    sensor_descriptor_t pedometer_descriptor;
    sensor_descriptor_t sleep_descriptor;
    sensor_descriptor_t sleep_staging_descriptor;
    sensor_descriptor_t heart_rate_monitor_descriptor;
    sensor_descriptor_t heart_rate_variability_descriptor;
    sensor_descriptor_t blood_pressure_descriptor;
    sensor_descriptor_t motion_detect_descriptor;
    sensor_descriptor_t heart_rate_ecg_monitor_descriptor;

    sensor_data_unit_t pedometer_data;

    uint32_t sleep_data_buffer_number;
    sensor_data_unit_t *sleep_data;
    uint32_t sleep_staging_buffer_number;
    sensor_data_unit_t *sleep_staging_data;

    sensor_data_unit_t heart_rate_monitor_data;
    sensor_data_unit_t heart_rate_variability_data;
    sensor_data_unit_t blood_pressure_data;
    sensor_data_unit_t motion_detect_data;
    sensor_data_unit_t heart_rate_monitor_ecg_data;

    uint32_t polling_period;
} sensor_manager_t;

static sensor_manager_t smt;

//#define SENSOR_PROFILING
#ifdef SENSOR_PROFILING
#define PROFILE_PERIOD 60UL

#define SENSOR_SPP_DATA_MAGIC       54321
#define SENSOR_SPP_DATA_RESERVED    12345

typedef struct {
uint32_t timestampstart;
uint32_t timestampend;
uint32_t first_data_ready;
uint32_t first_timeout;
uint32_t acc_count;
uint32_t acc_total_time;
uint32_t acc_max_time;
uint32_t baro_count;
uint32_t baro_total_time;
uint32_t baro_max_time;
uint32_t pedometer_count;
uint32_t pedometer_total_time;
uint32_t pedometer_max_time;
uint32_t st_count;
uint32_t st_total_time;
uint32_t st_max_time;
uint32_t hr_count;
uint32_t hr_total_time;
uint32_t hr_max_time;
uint32_t hrv_count;
uint32_t hrv_total_time;
uint32_t hrv_max_time;
uint32_t report_count;
uint32_t report_total_time;
uint32_t report_max_time;
uint32_t temp1;
uint32_t temp2;
uint32_t logged;
} profile_timestamp;


static profile_timestamp sensor_subsys_pft;

//#include "sensor_bt_spp_server.h"

void profile_time_print(void)
{
    if (sensor_subsys_pft.logged != 0) {
        return;
    }

    sensor_data_unit_t sm_pf_out_unit;
    sensor_data_t sm_pf_out;

    sm_pf_out_unit.value[0] = SENSOR_TYPE_PEDOMETER;
    sm_pf_out_unit.value[1] = sensor_subsys_pft.pedometer_count;
    sm_pf_out_unit.value[2] = sensor_subsys_pft.pedometer_total_time;
    sm_pf_out_unit.value[3] = sensor_subsys_pft.pedometer_max_time;
    sm_pf_out.data = &sm_pf_out_unit;
    send_sensor_data_via_btspp(
                   SENSOR_SPP_DATA_MAGIC,
                   9999,
                   sm_pf_out_unit.value[0],
                   sm_pf_out_unit.value[1],
                   sm_pf_out_unit.value[2],
                   sm_pf_out_unit.value[3],
                   SENSOR_SPP_DATA_RESERVED
                   );

    sm_pf_out_unit.value[0] = SENSOR_TYPE_HEART_RATE_MONITOR;
    sm_pf_out_unit.value[1] = sensor_subsys_pft.hr_count;
    sm_pf_out_unit.value[2] = sensor_subsys_pft.hr_total_time;
    sm_pf_out_unit.value[3] = sensor_subsys_pft.hr_max_time;
    sm_pf_out.data = &sm_pf_out_unit;
    send_sensor_data_via_btspp(
                   SENSOR_SPP_DATA_MAGIC,
                   9999,
                   sm_pf_out_unit.value[0],
                   sm_pf_out_unit.value[1],
                   sm_pf_out_unit.value[2],
                   sm_pf_out_unit.value[3],
                   SENSOR_SPP_DATA_RESERVED
                   );

    sm_pf_out_unit.value[0] = SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR;
    sm_pf_out_unit.value[1] = sensor_subsys_pft.hrv_count;
    sm_pf_out_unit.value[2] = sensor_subsys_pft.hrv_total_time;
    sm_pf_out_unit.value[3] = sensor_subsys_pft.hrv_max_time;
    sm_pf_out.data = &sm_pf_out_unit;
    send_sensor_data_via_btspp(
                   SENSOR_SPP_DATA_MAGIC,
                   9999,
                   sm_pf_out_unit.value[0],
                   sm_pf_out_unit.value[1],
                   sm_pf_out_unit.value[2],
                   sm_pf_out_unit.value[3],
                   SENSOR_SPP_DATA_RESERVED
                   );

    sm_pf_out_unit.value[0] = SENSOR_TYPE_SLEEP;
    sm_pf_out_unit.value[1] = sensor_subsys_pft.st_count;
    sm_pf_out_unit.value[2] = sensor_subsys_pft.st_total_time;
    sm_pf_out_unit.value[3] = sensor_subsys_pft.st_max_time;
    sm_pf_out.data = &sm_pf_out_unit;
    send_sensor_data_via_btspp(
                   SENSOR_SPP_DATA_MAGIC,
                   9999,
                   sm_pf_out_unit.value[0],
                   sm_pf_out_unit.value[1],
                   sm_pf_out_unit.value[2],
                   sm_pf_out_unit.value[3],
                   SENSOR_SPP_DATA_RESERVED
                   );


    printf("Start at (%ld). Stop at (%ld).\r\n", sensor_subsys_pft.timestampstart, sensor_subsys_pft.timestampend);
    printf("Pedometer count (%ld)\r\n", sensor_subsys_pft.pedometer_count);
    printf("Pedometer total (%ld)\r\n", sensor_subsys_pft.pedometer_total_time);
    printf("Pedometer max (%ld)\r\n", sensor_subsys_pft.pedometer_max_time);
    printf("HR count (%ld)\r\n", sensor_subsys_pft.hr_count);
    printf("HR total (%ld)\r\n", sensor_subsys_pft.hr_total_time);
    printf("HR max (%ld)\r\n", sensor_subsys_pft.hr_max_time);
    printf("HRV count (%ld)\r\n", sensor_subsys_pft.hrv_count);
    printf("HRV total (%ld)\r\n", sensor_subsys_pft.hrv_total_time);
    printf("HRV max (%ld)\r\n", sensor_subsys_pft.hrv_max_time);
    printf("ST count (%ld)\r\n", sensor_subsys_pft.st_count);
    printf("ST total (%ld)\r\n", sensor_subsys_pft.st_total_time);
    printf("ST max (%ld)\r\n", sensor_subsys_pft.st_max_time);
    printf("report count (%ld)\r\n", sensor_subsys_pft.report_count);
    printf("report total (%ld)\r\n", sensor_subsys_pft.report_total_time);
    printf("report max (%ld)\r\n", sensor_subsys_pft.report_max_time);

    sensor_subsys_pft.logged = 1;
}

#endif

uint32_t xSMLastExecutionTime;

#define SM_TIMER_ID0 0
static int32_t sm_timer_id;
static TimerHandle_t xTimerofMems; /* possibly more timer for different polling rate needed */

cmd_event_t sm_timer_event;

#ifdef SENSOR_FUSTION_TASK_ENABLED
#define SUPPORT_DUAL_TASK
#endif

#ifdef SUPPORT_DUAL_TASK
QueueHandle_t data_queue_handle;
#endif

#ifndef SUPPORT_DUAL_TASK
static sensor_data_t this_acc_data;
 #ifdef MTK_SENSOR_BIO_USE_MT2511
static sensor_data_t this_bio_ppg1_data;
static sensor_data_t this_bio_ecg_data;
static sensor_data_t this_bio_bisi_data;
 #endif
#endif

//+++ sensor throttling
#define SENSOR_DRIVER_MAX_DELAY 0x7FFFFFFF

struct sensor_control_data {
    uint32_t enable;
    uint32_t delay;
    uint32_t timeout;
};
static struct sensor_control_data all_sensor_ctrl_data[SENSOR_TYPE_ALL];
sensor_subscriber_t subscriber_list[SENSOR_TYPE_ALL];//TODO change to multi-app sub 1 sensor
uint32_t is_sensor_enabled(uint32_t sensor_type);

static int32_t sensor_throttling_control(uint32_t sensor_type, uint32_t enable, uint32_t delay, uint32_t timeout);
static void send_sensor_subscribe_event(uint32_t sensor_type, uint32_t enable, uint32_t delay, uint32_t timeout);
static void send_data_to_subscriber(sensor_data_t data, uint32_t sensor_type)
{
  #ifdef SENSOR_PROFILING
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp1));
  #endif

    int i;
    sensor_data_t *const output = &data;
    for (i = 0; i < SENSOR_TYPE_ALL; i++) {
#ifndef FUSION_REPORT_RAW_DATA_ENABLE
        if ( subscriber_list[i].send_digest != NULL && sensor_type == subscriber_list[i].type ) {
            SM_DBG("%ld %ld %ld %ld report output\r\n", output->data[0].value[0], output->data[0].value[1], output->data[0].value[2], output->data[0].value[3]);
            subscriber_list[subscriber_list[i].type].send_digest(output);
        }
#else
        //force HW sensor report data ( when hw sensor enable from fusion )
        if ( subscriber_list[i].send_digest != NULL ) {
            SM_DBG("%ld %ld %ld %ld report output\r\n", output->data[0].value[0], output->data[0].value[1], output->data[0].value[2], output->data[0].value[3]);
            subscriber_list[subscriber_list[i].type].send_digest(output);
            break;
        }
#endif
    }

  #ifdef SENSOR_PROFILING
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp2));
    sensor_subsys_pft.report_count++;
    uint32_t elapse = sensor_subsys_pft.temp2 - sensor_subsys_pft.temp1;
    if (elapse > sensor_subsys_pft.report_max_time) {
        sensor_subsys_pft.report_max_time = elapse;
    }
    sensor_subsys_pft.report_total_time += elapse;
  #endif
}
//---

#ifdef SDK_PESUDO_PEDOMETER
static sensor_input_list_t input_comp_acc;
static uint32_t pedometer_change;
static uint32_t user_step_length;

static int32_t get_pedometer_result(sensor_data_t *const output)
{
    //Return the pedometer result.
    sensor_data_unit_t *pedometer_data = output->data;
    pedometer_data->sensor_type = SENSOR_TYPE_PEDOMETER;
    pedometer_data->time_stamp = 0;
    pedometer_data->pedometer_t.accumulated_step_count = 1234;

    return 1;
}

static int32_t pedometer_process_data(const sensor_data_t *input_list, void *reserve)
{
    int32_t count = input_list->data_exist_count;
    sensor_data_unit_t *data_start = input_list->data;

    while (count != 0) {
        // Process the input data.
        if (pedometer_change == 0) {
            sensor_fusion_algorithm_notify(SENSOR_TYPE_PEDOMETER); // Call for data change notification.
            pedometer_change = 1;
        }
        data_start++;
        count--;
    }
    return 1;
}

static int32_t pedometer_operate(int32_t command, void *buffer_out, int32_t size_out, \
                                 void *buffer_in, int32_t size_in)
{
    // Process the customized parameter.
    user_step_length = *((uint32_t *)buffer_in);
    return 0;
}

const sensor_descriptor_t pedometer_desp = {
    SENSOR_TYPE_PEDOMETER,
    1, // version.
    SENSOR_REPORT_MODE_ON_CHANGE, // sensor_report_mode_t.
    &input_comp_acc, // Required input sensor list.
    pedometer_operate,
    get_pedometer_result,
    pedometer_process_data,
    0 // Not used
};

int pedometer_register()
{
    int ret;

    input_comp_acc.input_type = SENSOR_TYPE_ACCELEROMETER;
    input_comp_acc.sampling_delay = 20;
    input_comp_acc.timeout = 200;
    input_comp_acc.next_input = NULL;

    ret = sensor_fusion_algorithm_register_type(&pedometer_desp);
    if (ret < 0) {
        // Registration type error handling.
    }
    ret = sensor_fusion_algorithm_register_data_buffer(SENSOR_TYPE_PEDOMETER, 1);
    if (ret < 0) {
        // Registration data buffer error handling.
    }
    return ret;
}

int pedometer_init()
{
    return 0;
}
#endif


/* For platform dependent code. Should be overwritten in project main.c */
__weak void bsp_sensor_peripheral_init()
{
    return;
}

/* running at timer task*/
void vsensor_manager_timeout( TimerHandle_t pxTimer )
{
    sm_timer_event.event = SM_EVENT_TIMEOUT;
    sm_timer_event.period = smt.polling_period;

    // Post the event.
    if( xQueueSend( sm_queue_handle, ( void * ) &sm_timer_event, ( TickType_t ) 0 ) != pdPASS )
    {
    	// Failed to post the message.
    	SM_WARN("msg lost\r\n");
    }
}

//#define ARIMUTH_ESTIMATE
#ifdef ARIMUTH_ESTIMATE
/* Calculate azimuth of magnetic. Y axis is pointing forward.
  North is 0degree, Input X=0, Y=+Max.
  East is 90degree, Input X=-Max, Y=0.
  South is 180degree, Input X=0, Y=-Max.
  West is 270degree, Input X=+Max, Y=0.
*/
int32_t get_degree(int32_t X, int32_t Y)
{
    double dwDegree;
    double lx, ly;

    lx = (double)X;
    ly = (double)Y;

    dwDegree = 180 * atan(lx / ly) / 3.141592L;

    if (Y < 0) {
        dwDegree = 180 - dwDegree;
    } else {
        dwDegree = 360 - dwDegree;
    }

    if (dwDegree >= 360) {
        dwDegree -= 360;
    }

    return (int32_t)dwDegree;
}
#endif

uint32_t get_defined_sensor()
{
    uint32_t supported_algo = 0;

    /* physical sensor driver */
    if (sensor_driver_get_attached(SENSOR_TYPE_ACCELEROMETER) >= 0) {
        supported_algo |= (1U << SENSOR_TYPE_ACCELEROMETER);
    }
    if (sensor_driver_get_attached(SENSOR_TYPE_MAGNETIC_FIELD) >= 0) {
        supported_algo |= (1U << SENSOR_TYPE_MAGNETIC_FIELD);
    }
    if (sensor_driver_get_attached(SENSOR_TYPE_PRESSURE) >= 0) {
        supported_algo |= (1U << SENSOR_TYPE_PRESSURE);
    }
    if (sensor_driver_get_attached(SENSOR_TYPE_GYROSCOPE) >= 0) {
        supported_algo |= (1U << SENSOR_TYPE_GYROSCOPE);
    }
    if (sensor_driver_get_attached(SENSOR_TYPE_PROXIMITY) >= 0) {
        supported_algo |= (1U << SENSOR_TYPE_PROXIMITY);
    }

    if (sensor_driver_get_attached(SENSOR_TYPE_BIOSENSOR_PPG1) >= 0) {
        supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_PPG1);
    }

    if (sensor_driver_get_attached(SENSOR_TYPE_BIOSENSOR_PPG2) >= 0) {
        supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_PPG2);
    }

    if (sensor_driver_get_attached(SENSOR_TYPE_BIOSENSOR_BISI) >= 0) {
        supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_BISI);
    }

    if (sensor_driver_get_attached(SENSOR_TYPE_BIOSENSOR_EKG) >= 0) {
        supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_EKG);
    }

    if (sensor_driver_get_attached(SENSOR_TYPE_BIOSENSOR_EEG) >= 0) {
        supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_EEG);
    }

    if (sensor_driver_get_attached(SENSOR_TYPE_BIOSENSOR_EMG) >= 0) {
        supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_EMG);
    }

    if (sensor_driver_get_attached(SENSOR_TYPE_BIOSENSOR_GSR) >= 0) {
        supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_GSR);
    }

    if (sensor_driver_get_attached(SENSOR_TYPE_BIOSENSOR_PPG1_512HZ) >= 0) {
        supported_algo |= (1U << SENSOR_TYPE_BIOSENSOR_PPG1_512HZ);
    }

    /* sensor fusion */
#if defined(FUSION_PEDOMETER_USE)
    supported_algo |= (1U << SENSOR_TYPE_PEDOMETER);
#endif

#if defined(FUSION_SLEEP_TRACKER_USE)
    supported_algo |= (1U << SENSOR_TYPE_SLEEP);
#endif

#if defined(FUSION_SLEEP_STAGING_USE)
    supported_algo |= (1U << SENSOR_TYPE_SLEEP_STAGING);
#endif

#if defined(FUSION_HEART_RATE_MONITOR_USE)
    supported_algo |= (1U << SENSOR_TYPE_HEART_RATE_MONITOR);
#endif

#if defined(FUSION_HEART_RATE_VARIABILITY_USE)
    supported_algo |= (1U << SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR);
#endif

#if defined(FUSION_BLOOD_PRESSURE_USE)
    supported_algo |= (1U << SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
#endif

#if defined(AUTOMATIC_MOTION_DETECT)
    supported_algo |= (1U << SENSOR_TYPE_MOTION_DETECT);
#endif

#if defined(FUSION_HEART_RATE_BY_ECG_USE)
    supported_algo |= (1U << SENSOR_TYPE_ELECTRIC_HEART_RATE_MONITOR);
#endif

    return supported_algo;
}

#ifdef SUPPORT_DUAL_TASK

static uint32_t _sm_malloc_size;
static uint32_t _sm_free_count;

static void sensor_fusion_task( void *pvParameters )
{
    const TickType_t xTicksToWait = portMAX_DELAY;
    uint32_t data_updated = 0;
    sensor_data_t sensor_data;
    while (1) {
        if( xQueueReceive(data_queue_handle, &sensor_data, xTicksToWait)) {

            if(sensor_data.data->sensor_type == SENSOR_TYPE_ACCELEROMETER){
                data_updated |= (1U<<SENSOR_TYPE_ACCELEROMETER);
            }

            if(sensor_data.data->sensor_type == SENSOR_TYPE_BIOSENSOR_PPG1){
                data_updated |= (1U<<SENSOR_TYPE_BIOSENSOR_PPG1);
            }

            if(sensor_data.data->sensor_type == SENSOR_TYPE_BIOSENSOR_PPG1_512HZ){
                data_updated |= (1U<<SENSOR_TYPE_BIOSENSOR_PPG1);
            }

            if(sensor_data.data->sensor_type == SENSOR_TYPE_BIOSENSOR_EKG){
                data_updated |= (1U<<SENSOR_TYPE_BIOSENSOR_EKG);
            }

            if(sensor_data.data->sensor_type == SENSOR_TYPE_BIOSENSOR_BISI){
                data_updated |= (1U<<SENSOR_TYPE_BIOSENSOR_BISI);
            }

#if defined(FUSION_PEDOMETER_USE)
            if (is_sensor_enabled(SENSOR_TYPE_PEDOMETER)) {
                if (smt.pedometer_descriptor.input_list != NULL) {
                    if (data_updated & (1U << SENSOR_TYPE_ACCELEROMETER)) {
                       smt.pedometer_descriptor.process_data(&sensor_data, NULL);
                    }
                }
            }
#endif

#if defined(FUSION_HEART_RATE_MONITOR_USE)
            if (is_sensor_enabled(SENSOR_TYPE_HEART_RATE_MONITOR)) {
                if (data_updated & (1U << SENSOR_TYPE_ACCELEROMETER)) {
                    smt.heart_rate_monitor_descriptor.process_data(&sensor_data, NULL);
                }
                if (data_updated & (1U << SENSOR_TYPE_BIOSENSOR_PPG1)){
                    smt.heart_rate_monitor_descriptor.process_data(&sensor_data, NULL);
                }
            }
#endif

#if defined(FUSION_HEART_RATE_VARIABILITY_USE)
            if (is_sensor_enabled(SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR)) {
        #ifndef HEART_RATE_VARIABILITY_OUT_PATTERN
                if (data_updated & (1U << SENSOR_TYPE_BIOSENSOR_EKG)) {
                    smt.heart_rate_variability_descriptor.process_data(&sensor_data, NULL);
                }
        #else
                if (((xSMLastExecutionTime-hrv_start_time) > 10000UL) && (hrv_reported == 0)) {
                    /* send beat pos output pattern after subscription 10 seconds */
                    sensor_fusion_algorithm_notify(SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR);
                } else if (((xSMLastExecutionTime-hrv_start_time) > 20000UL) && (hrv_reported == 1)) {
                    /* send HRV output pattern after subscription 20 seconds */
                    sensor_fusion_algorithm_notify(SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR);
                }
        #endif
            }
#endif

#if defined(FUSION_BLOOD_PRESSURE_USE)
            if (is_sensor_enabled(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR)) {
        #ifndef BLOOD_PRESSURE_OUT_PATTERN
                /* not use 512HZ ppg1 type*/
                if (data_updated & (1U << SENSOR_TYPE_BIOSENSOR_PPG1)) {
                    smt.blood_pressure_descriptor.process_data(&sensor_data, NULL);
                }
                if (data_updated & (1U << SENSOR_TYPE_BIOSENSOR_EKG)) {
                    smt.blood_pressure_descriptor.process_data(&sensor_data, NULL);
                }
        #else
                if (((xSMLastExecutionTime-bp_start_time) > 10000UL) && (bp_reported == 0)) {
                    /* send pwtt output pattern after subscription 10 seconds */
                    sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
                } else if (((xSMLastExecutionTime-bp_start_time) > 20000UL) && (bp_reported == 1)) {
                    /* send BP output pattern after subscription 20 seconds */
                    sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
                }
        #endif
            }

#endif

#if defined(FUSION_SLEEP_TRACKER_USE)
            if (is_sensor_enabled(SENSOR_TYPE_SLEEP)) {
                if (data_updated & (1U << SENSOR_TYPE_ACCELEROMETER)) {
                   smt.sleep_descriptor.process_data(&sensor_data, NULL);
                }
            }
#endif
#if defined(FUSION_SLEEP_STAGING_USE)
            if (is_sensor_enabled(SENSOR_TYPE_SLEEP_STAGING)) {
                if (data_updated & (1U << SENSOR_TYPE_ACCELEROMETER)) {
                   smt.sleep_staging_descriptor.process_data(&sensor_data, NULL);
                }
                if (data_updated & (1U << SENSOR_TYPE_BIOSENSOR_PPG1)) {
                   smt.sleep_staging_descriptor.process_data(&sensor_data, NULL);
                }
                if (data_updated & (1U << SENSOR_TYPE_BIOSENSOR_BISI)) {
                    smt.sleep_staging_descriptor.process_data(&sensor_data, NULL);
                }
            }
#endif

#if defined(AUTOMATIC_MOTION_DETECT)
            if (is_sensor_enabled(SENSOR_TYPE_MOTION_DETECT)) {
                if (data_updated & (1U << SENSOR_TYPE_ACCELEROMETER)) {
                   smt.motion_detect_descriptor.process_data(&sensor_data, NULL);
                }
            }
#endif

#if defined(FUSION_HEART_RATE_BY_ECG_USE)
            if (is_sensor_enabled(SENSOR_TYPE_ELECTRIC_HEART_RATE_MONITOR)) {
                if (data_updated & (1U << SENSOR_TYPE_BIOSENSOR_EKG)) {
                   smt.heart_rate_ecg_monitor_descriptor.process_data(&sensor_data, NULL);
                }
            }
#endif
            vPortFree(sensor_data.data);
            _sm_malloc_size -= sizeof(sensor_data_unit_t)*sensor_data.data_exist_count;
            _sm_free_count++;

            data_updated = 0;
        }
    }
}

static void send_to_fusion_task(int32_t out_size, sensor_data_unit_t *const data_src, uint32_t data_time)
{
    sensor_data_t data_to_fusion;

    data_to_fusion.data_exist_count = out_size;
    data_to_fusion.data = (sensor_data_unit_t *)pvPortMalloc(sizeof(sensor_data_unit_t)*out_size);

    if (data_to_fusion.data != NULL) {
        _sm_malloc_size += sizeof(sensor_data_unit_t)*out_size;
        memcpy(data_to_fusion.data, data_src, (sizeof(sensor_data_unit_t)*out_size));
        if ( xQueueSendToBack( data_queue_handle, (void *) &data_to_fusion , (TickType_t) 0 ) != pdPASS ) {
            SM_WARN("send type(%d) to fusion task fail (time = %ld)\r\n", data_src->sensor_type, data_src->time_stamp);
            send_sensor_data_via_btspp(
               54321,
               9998,
               data_src->sensor_type,
               data_src->time_stamp,
               out_size,
               _sm_malloc_size,
               12345
               );
        }
    } else {
        SM_WARN("type(%d) malloc fail (time = %ld)\r\n", data_src->sensor_type, data_src->time_stamp);
        send_sensor_data_via_btspp(
           54321,
           9997,
           data_src->sensor_type,
           data_src->time_stamp,
           out_size,
           _sm_malloc_size,
           12345
           );
    }
}
#endif /* SUPPORT_DUAL_TASK */

static void sensor_mgr_task( void *pvParameters )
{
    const TickType_t xTicksToWait = portMAX_DELAY;
    uint32_t sensor_type = 0;
    int32_t acc_actual_out_size = 0;
    int32_t gyro_actual_out_size = 0;
    int32_t mag_actual_out_size = 0;
    int32_t baro_actual_out_size = 0;
    int32_t proximity_out_size = 0;
#ifdef MTK_SENSOR_BIO_USE_MT2511
    int32_t ppg1_actual_out_size = 0;
    int32_t ppg2_actual_out_size = 0;
    int32_t bisi_actual_out_size = 0;
    int32_t ekg_actual_out_size = 0;
    int32_t eeg_actual_out_size = 0;
    int32_t emg_actual_out_size = 0;
    int32_t gsr_actual_out_size = 0;
#endif
    cmd_event_t event_item;
    uint32_t i = 0;
    int input;

    bsp_sensor_peripheral_init();

    SM_INFO("defined sensors: %X\r\n", get_defined_sensor());

    smt.start_time = sensor_driver_get_ms_tick();
#ifdef ENABLE_SENSOR_POLLING_EMULATE
    /* Reset the time out period according to polling mode sensors */
    xTimerChangePeriod( xTimerofMems, smt.polling_period / portTICK_PERIOD_MS, xTicksToWait);
    xTimerReset(xTimerofMems, 1000 / portTICK_PERIOD_MS);
#endif

  #ifdef SENSOR_PROFILING
    uint32_t elapse;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.timestampstart));
  #endif

    while (1) {
        if ( xQueueReceive(sm_queue_handle, &event_item, xTicksToWait)) {

            xSMLastExecutionTime = sensor_driver_get_ms_tick();

            switch (event_item.event) {
                case SM_EVENT_TIMEOUT:

                    /* magnetic*/
                    if ( sensor_driver_get_enable(SENSOR_TYPE_MAGNETIC_FIELD) == 1 ) {
                        sensor_type = SENSOR_TYPE_MAGNETIC_FIELD;
                        sensor_driver_get_data(sensor_type, &(smt.mgr_mag_data), \
                                               sizeof(sensor_data_unit_t), &mag_actual_out_size);
                        if (mag_actual_out_size > 0) {
                            smt.mgr_mag_data.time_stamp = xSMLastExecutionTime;
                            smt.data_updated |= (1U << SENSOR_TYPE_MAGNETIC_FIELD);
#ifdef SUPPORT_DUAL_TASK
                            send_to_fusion_task(mag_actual_out_size, &(smt.mgr_mag_data), smt.mgr_mag_data.time_stamp);
#endif
                        }
                    }
                    /* barometer */
                    if ( sensor_driver_get_enable(SENSOR_TYPE_PRESSURE) == 1 ) {
                        sensor_type = SENSOR_TYPE_PRESSURE;
                        sensor_driver_get_data(sensor_type, &(smt.mgr_baro_data), \
                                               sizeof(sensor_data_unit_t), &baro_actual_out_size);
                        if (baro_actual_out_size > 0) {
                            smt.mgr_baro_data.time_stamp = xSMLastExecutionTime;
                            smt.data_updated |= (1U << SENSOR_TYPE_PRESSURE);
#ifdef SUPPORT_DUAL_TASK
                            send_to_fusion_task(baro_actual_out_size, &(smt.mgr_baro_data), smt.mgr_baro_data.time_stamp);
#endif
                        }
                    }
    #ifdef BMA255_USE_POLLING
                    if ( sensor_driver_get_enable(SENSOR_TYPE_ACCELEROMETER) == 1) {
                        /*Accelerometer*/
                        sensor_type = SENSOR_TYPE_ACCELEROMETER;
                        sensor_driver_get_data(sensor_type, smt.mgr_acc_data, \
                                               ACC_MAX_BUF_CNT * sizeof(sensor_data_unit_t), &acc_actual_out_size);

                        if (acc_actual_out_size > 0) {
                            smt.data_updated |= (1U << SENSOR_TYPE_ACCELEROMETER);
#ifdef SUPPORT_DUAL_TASK
                            send_to_fusion_task(acc_actual_out_size, smt.mgr_acc_data,  smt.mgr_acc_data[0].time_stamp);
#endif
                        }

                    }
    #endif
                    /* bio sensor */
                    #ifdef MTK_SENSOR_BIO_USE_MT2511
            #ifndef MT2511_USE_IRQ
                    if ( sensor_driver_get_enable(SENSOR_TYPE_BIOSENSOR_PPG1) == 1 ) {
                        sensor_type = SENSOR_TYPE_BIOSENSOR_PPG1;
                        sensor_driver_get_data(sensor_type, &(smt.ppg1_data), \
                                               BIO_MAX_BUF_CNT, &ppg1_actual_out_size);
                        if (ppg1_actual_out_size > 0) {
                            //smt.ppg1_data[ppg1_actual_out_size - 1].time_stamp = xSMLastExecutionTime;
                            smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_PPG1);
#ifdef SUPPORT_DUAL_TASK
                            send_to_fusion_task(ppg1_actual_out_size, smt.ppg1_data,  smt.ppg1_data[0].time_stamp);
#endif
                        }
                    }

                    if ( sensor_driver_get_enable(SENSOR_TYPE_BIOSENSOR_PPG2) == 1 ) {
                        sensor_type = SENSOR_TYPE_BIOSENSOR_PPG2;
                        sensor_driver_get_data(sensor_type, &(smt.ppg2_data), \
                                               BIO_MAX_BUF_CNT, &ppg2_actual_out_size);
                        if (ppg2_actual_out_size > 0) {
                            //smt.ppg2_data[ppg2_actual_out_size - 1].time_stamp = xSMLastExecutionTime;
                            smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_PPG2);
#ifdef SUPPORT_DUAL_TASK
                            send_to_fusion_task(ppg2_actual_out_size, smt.ppg2_data,  smt.ppg2_data[0].time_stamp);
#endif
                        }
                    }

                    if ( sensor_driver_get_enable(SENSOR_TYPE_BIOSENSOR_BISI) == 1 ) {
                        sensor_type = SENSOR_TYPE_BIOSENSOR_BISI;
                        sensor_driver_get_data(sensor_type, &(smt.bisi_data), \
                                               BIO_MAX_BUF_CNT, &bisi_actual_out_size);
                        if (bisi_actual_out_size > 0) {
                            smt.bisi_data[bisi_actual_out_size - 1].time_stamp = xSMLastExecutionTime;
                            smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_BISI);
#ifdef SUPPORT_DUAL_TASK
                            send_to_fusion_task(bisi_actual_out_size, smt.bisi_data,  smt.bisi_data[0].time_stamp);
#endif
                        }
                    }

                    if ( sensor_driver_get_enable(SENSOR_TYPE_BIOSENSOR_EKG) == 1 ) {
                        sensor_type = SENSOR_TYPE_BIOSENSOR_EKG;
                        sensor_driver_get_data(sensor_type, &(smt.ekg_data), \
                                               BIO_MAX_BUF_CNT, &ekg_actual_out_size);
                        if (ekg_actual_out_size > 0) {
                            //smt.ekg_data[ekg_actual_out_size - 1].time_stamp = xSMLastExecutionTime;
                            smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_EKG);
#ifdef SUPPORT_DUAL_TASK
                            send_to_fusion_task(ekg_actual_out_size, smt.ekg_data,  smt.ekg_data[0].time_stamp);
#endif
                        }
                    }

                    if ( sensor_driver_get_enable(SENSOR_TYPE_BIOSENSOR_EEG) == 1 ) {
                        sensor_type = SENSOR_TYPE_BIOSENSOR_EEG;
                        sensor_driver_get_data(sensor_type, &(smt.ekg_data), \
                                               BIO_MAX_BUF_CNT, &eeg_actual_out_size);
                        if (eeg_actual_out_size > 0) {
                            //smt.ekg_data[eeg_actual_out_size - 1].time_stamp = xSMLastExecutionTime;
                            smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_EEG);
                        }
                    }

                    if ( sensor_driver_get_enable(SENSOR_TYPE_BIOSENSOR_EMG) == 1 ) {
                        sensor_type = SENSOR_TYPE_BIOSENSOR_EMG;
                        sensor_driver_get_data(sensor_type, &(smt.ekg_data), \
                                               BIO_MAX_BUF_CNT, &emg_actual_out_size);
                        if (emg_actual_out_size > 0) {
                            //smt.ekg_data[emg_actual_out_size - 1].time_stamp = xSMLastExecutionTime;
                            smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_EMG);
                        }
                    }

                    if ( sensor_driver_get_enable(SENSOR_TYPE_BIOSENSOR_GSR) == 1 ) {
                        sensor_type = SENSOR_TYPE_BIOSENSOR_GSR;
                        sensor_driver_get_data(sensor_type, &(smt.ekg_data), \
                                               BIO_MAX_BUF_CNT, &gsr_actual_out_size);
                        if (gsr_actual_out_size > 0) {
                            //smt.ekg_data[gsr_actual_out_size - 1].time_stamp = xSMLastExecutionTime;
                            smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_GSR);
                        }
                    }
                    if ( sensor_driver_get_enable(SENSOR_TYPE_BIOSENSOR_PPG1_512HZ) == 1 ) {
                        sensor_type = SENSOR_TYPE_BIOSENSOR_PPG1_512HZ;
                        sensor_driver_get_data(sensor_type, &(smt.ppg1_data), \
                                               BIO_PPG_BUF_CNT, &ppg1_actual_out_size);
                        if (ppg1_actual_out_size > 0) {
                            //smt.ppg1_data[ppg1_actual_out_size - 1].time_stamp = xSMLastExecutionTime;
                            smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_PPG1_512HZ);
#ifdef SUPPORT_DUAL_TASK
                            send_to_fusion_task(ppg1_actual_out_size, smt.ppg1_data,  smt.ppg1_data[0].time_stamp);
#endif
                        }
                    }
                    #endif
            #endif
                    break;

                case SM_EVENT_DATA_READY:
                    {
                        uint32_t current_time_ms = sensor_driver_get_ms_tick();

                        if (event_item.data_ready & (1U << SENSOR_TYPE_ACCELEROMETER)) {

                            /*Accelerometer*/
                            sensor_type = SENSOR_TYPE_ACCELEROMETER;
                            sensor_driver_get_data(sensor_type, smt.mgr_acc_data, \
                                                   ACC_MAX_BUF_CNT * sizeof(sensor_data_unit_t), &acc_actual_out_size);

                            if (acc_actual_out_size > 0) {
                                smt.data_updated |= (1U << SENSOR_TYPE_ACCELEROMETER);
#ifdef SUPPORT_DUAL_TASK
                                send_to_fusion_task(acc_actual_out_size, smt.mgr_acc_data,  smt.mgr_acc_data[0].time_stamp);
#endif
                            }

                        }
                        if (event_item.data_ready & (1U << SENSOR_TYPE_GYROSCOPE)) {
                            /*Gyro*/
                            sensor_type = SENSOR_TYPE_GYROSCOPE;
                            sensor_driver_get_data(sensor_type, smt.mgr_gyro_data, \
                                                   GYRO_MAX_BUF_CNT * sizeof(sensor_data_unit_t), &gyro_actual_out_size);

                            if (gyro_actual_out_size > 0) {
                                smt.data_updated |= (1U << SENSOR_TYPE_GYROSCOPE);
#ifdef SUPPORT_DUAL_TASK
                                send_to_fusion_task(gyro_actual_out_size, smt.mgr_gyro_data,  smt.mgr_gyro_data[0].time_stamp);
#endif
                            }
                        }
                        if (event_item.data_ready & (1U << SENSOR_TYPE_PROXIMITY)) {
                            /*Proximity*/
                            sensor_type = SENSOR_TYPE_PROXIMITY;
                            sensor_driver_get_data(sensor_type, &(smt.mgr_proximity_data), \
                                                   sizeof(sensor_data_unit_t), &proximity_out_size);
                            smt.mgr_proximity_data.time_stamp = current_time_ms;
                            smt.data_updated |= (1U << SENSOR_TYPE_PROXIMITY);
                            SM_INFO("Distance(%ld), time(%ld) \r\n", smt.mgr_proximity_data.distance, current_time_ms);
                        }
#ifdef MTK_SENSOR_BIO_USE_MT2511
                        #ifdef MT2511_USE_IRQ
                        if (event_item.data_ready & (1U << SENSOR_TYPE_BIOSENSOR_PPG1)) {
                            uint32_t signal = 0, intr_ctrl;
                            /*ppg1*/
                            SM_INFO("PPG1 IRQ!!\r\n");

                            vsm_check_trigger_signal(&signal, &intr_ctrl);
                            if (signal & (1U << SENSOR_TYPE_BIOSENSOR_PPG1)) {
                                sensor_type = SENSOR_TYPE_BIOSENSOR_PPG1;
                                sensor_driver_get_data(sensor_type, smt.ppg1_data, \
                                                       BIO_MAX_BUF_CNT * sizeof(sensor_data_unit_t), &ppg1_actual_out_size);

                                if (ppg1_actual_out_size > 0) {
                                    smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_PPG1);
#ifdef SUPPORT_DUAL_TASK
                                    send_to_fusion_task(ppg1_actual_out_size, smt.ppg1_data,  smt.ppg1_data[0].time_stamp);
#endif
                                }
                            }
                            if (signal & (1U << SENSOR_TYPE_BIOSENSOR_EKG)) {
                                sensor_type = SENSOR_TYPE_BIOSENSOR_EKG;
                                sensor_driver_get_data(sensor_type, smt.ekg_data, \
                                                       BIO_MAX_BUF_CNT * sizeof(sensor_data_unit_t), &ekg_actual_out_size);

                                if (ekg_actual_out_size > 0) {
                                    smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_EKG);
#ifdef SUPPORT_DUAL_TASK
                                    send_to_fusion_task(ekg_actual_out_size, smt.ekg_data,  smt.ekg_data[0].time_stamp);
#endif
                                }
                            }
                            if (signal & (1U << SENSOR_TYPE_BIOSENSOR_PPG2)) {
                                sensor_type = SENSOR_TYPE_BIOSENSOR_PPG2;
                                sensor_driver_get_data(sensor_type, smt.ppg2_data, \
                                                       BIO_MAX_BUF_CNT * sizeof(sensor_data_unit_t), &ppg2_actual_out_size);

                                if (ppg2_actual_out_size > 0) {
                                    smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_PPG2);
#ifdef SUPPORT_DUAL_TASK
                                    send_to_fusion_task(ppg2_actual_out_size, smt.ppg2_data,  smt.ppg2_data[0].time_stamp);
#endif
                                }
                            }
                            if (signal & (1U << SENSOR_TYPE_BIOSENSOR_BISI)) {
                                sensor_type = SENSOR_TYPE_BIOSENSOR_BISI;
                                sensor_driver_get_data(sensor_type, smt.bisi_data, \
                                                       BIO_MAX_BUF_CNT * sizeof(sensor_data_unit_t), &bisi_actual_out_size);

                                if (bisi_actual_out_size > 0) {
                                    smt.data_updated |= (1U << SENSOR_TYPE_BIOSENSOR_BISI);
#ifdef SUPPORT_DUAL_TASK
                                    send_to_fusion_task(bisi_actual_out_size, smt.bisi_data,  smt.bisi_data[0].time_stamp);
#endif
                                }
                            }
                            vsm_reenable_irq(intr_ctrl);
                        }
#endif
                        #endif

                        if (event_item.data_ready & (1U << SENSOR_TYPE_PEDOMETER)) {
                            /* smart sensor, driver notifies the step count */
                        #if defined(FUSION_PEDOMETER_USE)
                            if (is_sensor_enabled(SENSOR_TYPE_PEDOMETER)) {
                                if (smt.pedometer_descriptor.input_list == NULL) {
                                    smt.pedometer_descriptor.process_data(NULL, NULL);
                                }
                            }
                        #endif
                        }
                    }
                    break;

                case SM_EVENT_SUBSCRIBE:
                    sensor_throttling_control(event_item.sensor_type, event_item.enable, event_item.delay, event_item.timeout);
                    continue;

                case SM_EVENT_CALIBRATION:
                    input = 0;
                    sensor_driver_set_cust(SENSOR_TYPE_GYROSCOPE, NULL, 0, NULL, (void *)(&input), sizeof(int));
                    continue;

            }

#ifndef SUPPORT_DUAL_TASK
            /* Fusion algorithm */
            if (smt.data_updated & (1U << SENSOR_TYPE_ACCELEROMETER)) {
                this_acc_data.data_exist_count = acc_actual_out_size;
                this_acc_data.data = smt.mgr_acc_data;
                (void)this_acc_data;
            }
#ifdef MTK_SENSOR_BIO_USE_MT2511
            if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_PPG1)) {
                this_bio_ppg1_data.data_exist_count = ppg1_actual_out_size;
                this_bio_ppg1_data.data = smt.ppg1_data;
                (void)this_bio_ppg1_data;
            }

            if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_PPG1_512HZ)) {
                this_bio_ppg1_data.data_exist_count = ppg1_actual_out_size;
                this_bio_ppg1_data.data = smt.ppg1_data;
                (void)this_bio_ppg1_data;
            }

            if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_EKG)) {
                this_bio_ecg_data.data_exist_count = ekg_actual_out_size;
                this_bio_ecg_data.data = smt.ekg_data;
                (void)this_bio_ecg_data;
            }

            if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_BISI)) {
                this_bio_bisi_data.data_exist_count = bisi_actual_out_size;
                this_bio_bisi_data.data = smt.bisi_data;
                (void)this_bio_bisi_data;
            }
#endif
            /* Pedometer*/
#if defined(FUSION_PEDOMETER_USE)
            if (is_sensor_enabled(SENSOR_TYPE_PEDOMETER)) {
                if (smt.pedometer_descriptor.input_list != NULL) {
                    if (smt.data_updated & (1U << SENSOR_TYPE_ACCELEROMETER)) {

                    #ifdef SENSOR_PROFILING
                        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp1));
                    #endif

                      smt.pedometer_descriptor.process_data(&this_acc_data, NULL);

                  #ifdef SENSOR_PROFILING
                      sensor_subsys_pft.pedometer_count++;
                      hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp2));
                      elapse = sensor_subsys_pft.temp2 - sensor_subsys_pft.temp1;
                      if (elapse > sensor_subsys_pft.pedometer_max_time) {
                          sensor_subsys_pft.pedometer_max_time = elapse;
                      }
                      sensor_subsys_pft.pedometer_total_time += elapse;
                  #endif
                    }
                }
            }
#endif

            /* Sleep tracker */
#if defined(FUSION_SLEEP_TRACKER_USE)
            if (is_sensor_enabled(SENSOR_TYPE_SLEEP)) {
                if (smt.data_updated & (1U << SENSOR_TYPE_ACCELEROMETER)) {
                #ifdef SENSOR_PROFILING
                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp1));
                #endif

                  smt.sleep_descriptor.process_data(&this_acc_data, NULL);

              #ifdef SENSOR_PROFILING
                  sensor_subsys_pft.st_count++;
                  hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp2));
                  elapse = sensor_subsys_pft.temp2 - sensor_subsys_pft.temp1;
                  if (elapse > sensor_subsys_pft.st_max_time) {
                      sensor_subsys_pft.st_max_time = elapse;
                  }
                  sensor_subsys_pft.st_total_time += elapse;
              #endif
                }
            }
#endif

#if defined(FUSION_SLEEP_STAGING_USE)
            if (is_sensor_enabled(SENSOR_TYPE_SLEEP_STAGING)) {
                if (smt.data_updated & (1U << SENSOR_TYPE_ACCELEROMETER)) {
                #ifdef SENSOR_PROFILING
                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp1));
                #endif

                  smt.sleep_staging_descriptor.process_data(&this_acc_data, NULL);

              #ifdef SENSOR_PROFILING
                  sensor_subsys_pft.st_count++;
                  hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp2));
                  elapse = sensor_subsys_pft.temp2 - sensor_subsys_pft.temp1;
                  if (elapse > sensor_subsys_pft.st_max_time) {
                      sensor_subsys_pft.st_max_time = elapse;
                  }
                  sensor_subsys_pft.st_total_time += elapse;
              #endif
                }
                if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_PPG1)) {
                #ifdef SENSOR_PROFILING
                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp1));
                #endif

                  smt.sleep_staging_descriptor.process_data(&this_bio_ppg1_data, NULL);

              #ifdef SENSOR_PROFILING
                  sensor_subsys_pft.st_count++;
                  hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp2));
                  elapse = sensor_subsys_pft.temp2 - sensor_subsys_pft.temp1;
                  if (elapse > sensor_subsys_pft.st_max_time) {
                      sensor_subsys_pft.st_max_time = elapse;
                  }
                  sensor_subsys_pft.st_total_time += elapse;
              #endif
                }
                if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_BISI)) {
                #ifdef SENSOR_PROFILING
                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp1));
                #endif

                  smt.sleep_staging_descriptor.process_data(&this_bio_bisi_data, NULL);

              #ifdef SENSOR_PROFILING
                  sensor_subsys_pft.st_count++;
                  hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp2));
                  elapse = sensor_subsys_pft.temp2 - sensor_subsys_pft.temp1;
                  if (elapse > sensor_subsys_pft.st_max_time) {
                      sensor_subsys_pft.st_max_time = elapse;
                  }
                  sensor_subsys_pft.st_total_time += elapse;
              #endif
                }
            }
#endif

#if defined(FUSION_HEART_RATE_MONITOR_USE)
            if (is_sensor_enabled(SENSOR_TYPE_HEART_RATE_MONITOR)) {

              #ifndef HEART_RATE_MONITOR_OUT_PATTERN
                if (smt.data_updated & (1U << SENSOR_TYPE_ACCELEROMETER)) {
                      #ifdef SENSOR_PROFILING
                        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp1));
                      #endif

                        smt.heart_rate_monitor_descriptor.process_data(&this_acc_data, NULL);

                    #ifdef SENSOR_PROFILING
                          hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp2));
                          elapse = sensor_subsys_pft.temp2 - sensor_subsys_pft.temp1;
                          if (elapse > sensor_subsys_pft.hr_max_time) {
                              sensor_subsys_pft.hr_max_time = elapse;
                          }
                          sensor_subsys_pft.hr_total_time += elapse;
                          sensor_subsys_pft.hr_count++;
                    #endif
                }
                if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_PPG1)){
              /* process biosensor data */
                #ifdef SENSOR_PROFILING
                  hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp1));
                #endif

                  smt.heart_rate_monitor_descriptor.process_data(&this_bio_ppg1_data, NULL);

                #ifdef SENSOR_PROFILING
                  hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp2));
                  elapse = sensor_subsys_pft.temp2 - sensor_subsys_pft.temp1;
                  if (elapse > sensor_subsys_pft.hr_max_time) {
                      sensor_subsys_pft.hr_max_time = elapse;
                  }
                  sensor_subsys_pft.hr_total_time += elapse;
                  sensor_subsys_pft.hr_count++;
                #endif
                }
              #else
                if ((xSMLastExecutionTime - hr_last_update_time) > 1000UL) {
                    /* update every second*/
                    sensor_fusion_algorithm_notify(SENSOR_TYPE_HEART_RATE_MONITOR);
                    hr_last_update_time = xSMLastExecutionTime;
                }
              #endif
            }
#endif

#if defined(FUSION_HEART_RATE_VARIABILITY_USE)
        if (is_sensor_enabled(SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR)) {
            #ifndef HEART_RATE_VARIABILITY_OUT_PATTERN
            if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_EKG)) {
            #ifdef SENSOR_PROFILING
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp1));
            #endif

                smt.heart_rate_variability_descriptor.process_data(&this_bio_ecg_data, NULL);

          #ifdef SENSOR_PROFILING
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.temp2));
                elapse = sensor_subsys_pft.temp2 - sensor_subsys_pft.temp1;
                if (elapse > sensor_subsys_pft.hrv_max_time) {
                    sensor_subsys_pft.hrv_max_time = elapse;
                }
                sensor_subsys_pft.hrv_total_time += elapse;
                sensor_subsys_pft.hrv_count++;
          #endif
            }
            #else
            if (((xSMLastExecutionTime-hrv_start_time) > 10000UL) && (hrv_reported == 0)) {
                /* send HRV output pattern after subscription 10 seconds */
                sensor_fusion_algorithm_notify(SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR);
                hrv_reported = 1; /* one-shot notification */
            }
            #endif
        }
#endif

#if defined(FUSION_BLOOD_PRESSURE_USE)
            if (is_sensor_enabled(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR)) {
            #ifndef BLOOD_PRESSURE_OUT_PATTERN
                if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_PPG1_512HZ)) {
                    smt.blood_pressure_descriptor.process_data(&this_bio_ppg1_data, NULL);
                }

                if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_EKG)) {
                    smt.blood_pressure_descriptor.process_data(&this_bio_ecg_data, NULL);
                }
            #else
                if (((xSMLastExecutionTime-bp_start_time) > 10000UL) && (bp_reported == 0)) {
                    /* send BP output pattern after subscription 10 seconds */
                    sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
                    bp_reported = 1; /* one-shot notification */
                } else if (((xSMLastExecutionTime-bp_start_time) > 20000UL) && (bp_reported == 1)) {
                    /* send pwtt output pattern after subscription 20 seconds */
                    sensor_fusion_algorithm_notify(SENSOR_TYPE_BLOOD_PRESSURE_MONITOR);
                    bp_reported = 2; /* one-shot notification */
                }
            #endif
            }
#endif
#endif /* SUPPORT_DUAL_TASK */

            //+++ report hw sensor data
            sensor_data_t physical_sensor_data;
            if (smt.data_updated & (1U << SENSOR_TYPE_ACCELEROMETER)) {
                for (i = 0; i < acc_actual_out_size; i++) {
                    physical_sensor_data.data = &smt.mgr_acc_data[i];
                    physical_sensor_data.data_exist_count = 1;
                    send_data_to_subscriber(physical_sensor_data, SENSOR_TYPE_ACCELEROMETER);
                }
            }
            if (smt.data_updated & (1U << SENSOR_TYPE_GYROSCOPE)) {
                for (i = 0; i < gyro_actual_out_size; i++) {
                    physical_sensor_data.data = &smt.mgr_gyro_data[i];
                    physical_sensor_data.data_exist_count = 1;
                    send_data_to_subscriber(physical_sensor_data, SENSOR_TYPE_GYROSCOPE);
                }
            }
            if (smt.data_updated & (1U << SENSOR_TYPE_MAGNETIC_FIELD)) {
                physical_sensor_data.data = &smt.mgr_mag_data;
                physical_sensor_data.data_exist_count = mag_actual_out_size;
                send_data_to_subscriber(physical_sensor_data, SENSOR_TYPE_MAGNETIC_FIELD);
            }
            if (smt.data_updated & (1U << SENSOR_TYPE_PRESSURE)) {
                physical_sensor_data.data = &smt.mgr_baro_data;
                physical_sensor_data.data_exist_count = baro_actual_out_size;
                send_data_to_subscriber(physical_sensor_data, SENSOR_TYPE_PRESSURE);
            }
            if (smt.data_updated & (1U << SENSOR_TYPE_PROXIMITY)) {
                physical_sensor_data.data = &smt.mgr_proximity_data;
                physical_sensor_data.data_exist_count = proximity_out_size;
                send_data_to_subscriber(physical_sensor_data, SENSOR_TYPE_PROXIMITY);
            }
#ifdef MTK_SENSOR_BIO_USE_MT2511
            if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_PPG1)) {
                for (i = 0; i < ppg1_actual_out_size; i++) {
                    physical_sensor_data.data = &smt.ppg1_data[i];
                    physical_sensor_data.data_exist_count = 1;
                    send_data_to_subscriber(physical_sensor_data, SENSOR_TYPE_BIOSENSOR_PPG1);
                }
            }
            if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_PPG1_512HZ)) {
                for (i = 0; i < ppg1_actual_out_size; i++) {
                    physical_sensor_data.data = &smt.ppg1_data[i];
                    physical_sensor_data.data_exist_count = 1;
                    send_data_to_subscriber(physical_sensor_data, SENSOR_TYPE_BIOSENSOR_PPG1);
                }
            }
            if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_PPG2)) {
                for (i = 0; i < ppg2_actual_out_size; i++) {
                    physical_sensor_data.data = &smt.ppg2_data[i];
                    physical_sensor_data.data_exist_count = 1;
                    send_data_to_subscriber(physical_sensor_data, SENSOR_TYPE_BIOSENSOR_PPG2);
                }
            }
            if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_EKG)) {
                for (i = 0; i < ekg_actual_out_size; i++) {
                    physical_sensor_data.data = &smt.ekg_data[i];
                    physical_sensor_data.data_exist_count = 1;
                    send_data_to_subscriber(physical_sensor_data, SENSOR_TYPE_BIOSENSOR_EKG);
                }
            }
            if (smt.data_updated & (1U << SENSOR_TYPE_BIOSENSOR_BISI)) {
                for (i = 0; i < bisi_actual_out_size; i++) {
                    physical_sensor_data.data = &smt.bisi_data[i];
                    physical_sensor_data.data_exist_count = 1;
                    send_data_to_subscriber(physical_sensor_data, SENSOR_TYPE_BIOSENSOR_BISI);
                }
            }
            (void)eeg_actual_out_size;
            (void)emg_actual_out_size;
            (void)gsr_actual_out_size;
#endif
            //---
            smt.data_updated = 0;

    #ifdef SENSOR_PROFILING
          hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sensor_subsys_pft.timestampend));
          if (sensor_subsys_pft.timestampend - sensor_subsys_pft.timestampstart > 32768UL*PROFILE_PERIOD) {
              profile_time_print();
          }
    #endif

        }

    }
}

static TaskHandle_t _sensor_manager_task_handle;
#ifdef SENSOR_FUSTION_TASK_ENABLED
static TaskHandle_t _sensor_fusion_task_handle;
#endif
int32_t sensor_manager_init(void)
{
    uint32_t i = 0;

    memset(&smt, 0, sizeof(sensor_manager_t));
    for (i = 0; i < ACC_MAX_BUF_CNT; i++) {
        smt.mgr_acc_data[i].sensor_type = SENSOR_TYPE_ACCELEROMETER;
    }
    for (i = 0; i < GYRO_MAX_BUF_CNT; i++) {
        smt.mgr_gyro_data[i].sensor_type = SENSOR_TYPE_GYROSCOPE;
    }
    smt.mgr_mag_data.sensor_type = SENSOR_TYPE_MAGNETIC_FIELD;
    smt.mgr_baro_data.sensor_type = SENSOR_TYPE_PRESSURE;
    smt.mgr_proximity_data.sensor_type = SENSOR_TYPE_PROXIMITY;
#ifdef MTK_SENSOR_BIO_USE_MT2511
    for (i = 0; i < BIO_MAX_BUF_CNT; i++) {
        smt.ppg1_data[i].sensor_type = SENSOR_TYPE_BIOSENSOR_PPG1;
        smt.ppg2_data[i].sensor_type = SENSOR_TYPE_BIOSENSOR_PPG2;
        smt.ekg_data[i].sensor_type = SENSOR_TYPE_BIOSENSOR_EKG;
        smt.bisi_data[i].sensor_type = SENSOR_TYPE_BIOSENSOR_BISI;
    }
#endif
    smt.polling_period = 20; //ms

#if defined(FUSION_PEDOMETER_USE)
    pedometer_register();
#endif

#if defined(FUSION_SLEEP_TRACKER_USE)
    sleep_register();
#endif

#if defined(FUSION_SLEEP_STAGING_USE)
    sleep_staging_register();
#endif

#if defined(FUSION_HEART_RATE_MONITOR_USE)
    heart_rate_monitor_register();
  #ifdef HEART_RATE_MONITOR_OUT_PATTERN
    golden_total = sizeof(hr_output) / sizeof(hr_output[0]);
  #endif
#endif

#if defined(FUSION_HEART_RATE_VARIABILITY_USE)
    heart_rate_variability_monitor_register();
#endif

#if defined(FUSION_BLOOD_PRESSURE_USE)
    blood_pressure_monitor_register();
#endif

#ifdef AUTOMATIC_MOTION_DETECT
    motion_detect_register();
#endif

#ifdef FUSION_HEART_RATE_BY_ECG_USE
    heart_rate_ecg_register();
#endif

    /* Timer create for polling mode sensor */
    sm_timer_id = SM_TIMER_ID0;
    xTimerofMems = xTimerCreate("TimerofMems",       /* Just a text name, not used by the kernel. */
                                ( smt.polling_period / portTICK_PERIOD_MS ),  /* The timer period in ticks. */
                                pdTRUE,        /* The timers will auto-reload themselves when they expire. */
                                ( void *) sm_timer_id,   /* Assign each timer a unique id equal to its array index. */
                                vsensor_manager_timeout /* Each timer calls the same callback when it expires. */
                               );

    if ( xTimerofMems == NULL ) {
        SM_ERR("xTimerofMems create fail \r\n");
    }

    /* Queue creation */
    sm_queue_handle = xQueueCreate(32, sizeof(cmd_event_t));
#ifdef SUPPORT_DUAL_TASK
    data_queue_handle = xQueueCreate(2048, sizeof(sensor_data_t));
#endif

    xTaskCreate(sensor_mgr_task, SENSOR_MGR_TASK_NAME, SENSOR_MGR_STACKSIZE / sizeof(StackType_t), NULL, TASK_PRIORITY_SOFT_REALTIME, &_sensor_manager_task_handle);
#ifdef SENSOR_FUSTION_TASK_ENABLED
    xTaskCreate(sensor_fusion_task, SENSOR_FUSION_TASK_NAME, SENSOR_FUSION_STACKSIZE / sizeof(StackType_t), NULL, TASK_PRIORITY_NORMAL, &_sensor_fusion_task_handle);
#endif

    return 1;
}

int32_t sensor_fusion_algorithm_register_type(const sensor_descriptor_t *desp)
{
    SM_INFO("sensor_fusion_algorithm_register_type(%ld) \r\n", desp->sensor_type);
    if (SENSOR_TYPE_PEDOMETER == desp->sensor_type) {
        memcpy(&(smt.pedometer_descriptor), desp, sizeof(sensor_descriptor_t));
    }  else if (SENSOR_TYPE_SLEEP == desp->sensor_type) {
        memcpy(&(smt.sleep_descriptor), desp, sizeof(sensor_descriptor_t));
    } else if (SENSOR_TYPE_SLEEP_STAGING == desp->sensor_type) {
        memcpy(&(smt.sleep_staging_descriptor), desp, sizeof(sensor_descriptor_t));
    } else if (SENSOR_TYPE_HEART_RATE_MONITOR == desp->sensor_type) {
        memcpy(&(smt.heart_rate_monitor_descriptor), desp, sizeof(sensor_descriptor_t));
    } else if (SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR == desp->sensor_type) {
        memcpy(&(smt.heart_rate_variability_descriptor), desp, sizeof(sensor_descriptor_t));
    } else if (SENSOR_TYPE_BLOOD_PRESSURE_MONITOR == desp->sensor_type) {
        memcpy(&(smt.blood_pressure_descriptor), desp, sizeof(sensor_descriptor_t));
    } else if (SENSOR_TYPE_MOTION_DETECT == desp->sensor_type) {
        memcpy(&(smt.motion_detect_descriptor), desp, sizeof(sensor_descriptor_t));
    } else if (SENSOR_TYPE_ELECTRIC_HEART_RATE_MONITOR == desp->sensor_type) {
        memcpy(&(smt.heart_rate_ecg_monitor_descriptor), desp, sizeof(sensor_descriptor_t));
    } else {
        return -1;
    }
    return 1;
}

int32_t sensor_fusion_algorithm_register_data_buffer(uint32_t sensor_type, uint32_t exist_data_count)
{
    SM_INFO("sensor_fusion_algorithm_register_data_buffer(%ld), Count(%ld) \r\n", sensor_type, exist_data_count);

    switch (sensor_type) {
        case SENSOR_TYPE_SLEEP:
            smt.sleep_data_buffer_number = exist_data_count;
            break;
        case SENSOR_TYPE_SLEEP_STAGING:
            smt.sleep_staging_buffer_number = exist_data_count;
            break;
        default:
            if (exist_data_count > 1) {
                SM_ERR("sensor_fusion_algorithm_register_data_buffer for more than 1 NOT support... \r\n");
                configASSERT(0);
                return -1;
            }
            break;
    }

    return 1;
}

#ifdef M_INHOUSE_ACTIVITY
const static activity_mapping_t act_map = {
    .class_name = {"still", "stand", "sit", "lie", "onfoot", \
        "walk", "run", "stairs", "cycle", "vehicle", "tilt", "unknown"
    },
};
#endif

int32_t sensor_fusion_algorithm_notify(uint32_t sensor_type)
{
    sensor_data_t algorithm_result;
    int32_t ret = 1;
  #ifdef BLOOD_PRESSURE_OUT_PATTERN
    int32_t pwtt_out_pattern[9] = {11, 22, 33, 44, 55, 66, 77, 88, 99};
  #endif
  #ifdef HEART_RATE_VARIABILITY_OUT_PATTERN
    int32_t beatpos_out_pattern[8] = {7, 14, 21, 28, 35, 42, 49, 56};
  #endif

    /* Not consider one result containing multiple data */
    algorithm_result.data_exist_count = 1;
    algorithm_result.fifo_max_size = 0;

    SM_DBG("sensor_fusion_algorithm_notify(%ld)\r\n", sensor_type);
    if (SENSOR_TYPE_PEDOMETER == sensor_type) {
        if (smt.pedometer_descriptor.get_result != NULL) {
            algorithm_result.data = &(smt.pedometer_data);
            smt.pedometer_descriptor.get_result(&algorithm_result);
            SM_INFO("Pedometer on-change (%ld) \r\n", smt.pedometer_data.pedometer_t.accumulated_step_count);
        }
    } else if (SENSOR_TYPE_SLEEP == sensor_type) {
        if (smt.sleep_descriptor.get_result != NULL) {
            algorithm_result.data = smt.sleep_data;
        #ifndef SLEEP_TRACKER_OUT_PATTERN
            smt.sleep_descriptor.get_result(&algorithm_result);
            SM_INFO("Sleep tracker data_exist_count (%ld) (%ld) \r\n", smt.sleep_data->time_stamp, algorithm_result.data_exist_count);
            int i = 0;
            while (i < algorithm_result.data_exist_count) {
                SM_INFO("Sleep tracker on-change: %ld, %ld\r\n", (algorithm_result.data+i)->time_stamp, (algorithm_result.data+i)->sleep_data_t.state);
                i++;
            }
        #else
            uint32_t i = 0;
            algorithm_result.data_exist_count = 20;
            for (i=0; i<20; i++) {
                algorithm_result.data[i].sensor_type = SENSOR_TYPE_SLEEP;
                algorithm_result.data[i].time_stamp = i*100;
                algorithm_result.data[i].sleep_data_t.state = (i*100)+1;
            }
            st_reported++;
        #endif
        }
    } else if (SENSOR_TYPE_SLEEP_STAGING == sensor_type) {
        if (smt.sleep_staging_descriptor.get_result != NULL) {
            algorithm_result.data = smt.sleep_staging_data;
            smt.sleep_descriptor.get_result(&algorithm_result);
            SM_INFO("Sleep staging data_exist_count (%ld) (%ld) \r\n", smt.sleep_data->time_stamp, algorithm_result.data_exist_count);
            int i = 0;
            while (i < algorithm_result.data_exist_count) {
                SM_INFO("Sleep tracker on-change: %ld, %ld\r\n", (algorithm_result.data+i)->time_stamp, (algorithm_result.data+i)->sleep_data_t.state);
                i++;
            }
        }
    } else if (SENSOR_TYPE_HEART_RATE_MONITOR== sensor_type) {
        if (smt.heart_rate_monitor_descriptor.get_result != NULL) {
            algorithm_result.data = &(smt.heart_rate_monitor_data);

        #ifndef HEART_RATE_MONITOR_OUT_PATTERN
            smt.heart_rate_monitor_descriptor.get_result(&algorithm_result);
        #else
            smt.heart_rate_monitor_data.sensor_type = SENSOR_TYPE_HEART_RATE_MONITOR;
            smt.heart_rate_monitor_data.time_stamp = xSMLastExecutionTime;
            smt.heart_rate_monitor_data.heart_rate_t.bpm = hr_output[golden_idx];
            golden_idx = (golden_idx+1)%golden_total;
        #endif
            SM_INFO("Heart rate monitor on-change: %ld, %ld\r\n", smt.heart_rate_monitor_data.time_stamp, smt.heart_rate_monitor_data.heart_rate_t.bpm);
        }
    }  else if (SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR == sensor_type) {
        if (smt.heart_rate_variability_descriptor.get_result != NULL) {
            algorithm_result.data = &(smt.heart_rate_variability_data);
            #ifndef HEART_RATE_VARIABILITY_OUT_PATTERN
            smt.heart_rate_variability_descriptor.get_result(&algorithm_result);
            SM_INFO("Heart rate variability on-change: %ld, %ld, %ld, %ld, %ld\r\n", smt.heart_rate_variability_data.time_stamp, smt.heart_rate_variability_data.heart_rate_variability_t.SDNN,
                smt.heart_rate_variability_data.heart_rate_variability_t.LF, smt.heart_rate_variability_data.heart_rate_variability_t.HF, smt.heart_rate_variability_data.heart_rate_variability_t.LF_HF);
            #else
              if (hrv_reported == 0) {
                  smt.heart_rate_variability_data.sensor_type = SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR;
                  smt.heart_rate_variability_data.time_stamp = xSMLastExecutionTime;
                  smt.heart_rate_variability_data.heart_rate_variability_t.SDNN = 0;
                  smt.heart_rate_variability_data.heart_rate_variability_t.LF = 0;
                  smt.heart_rate_variability_data.heart_rate_variability_t.HF = 0;
                  smt.heart_rate_variability_data.heart_rate_variability_t.LF_HF= 0;
                  smt.heart_rate_variability_data.heart_rate_variability_t.beatPos = beatpos_out_pattern;
                  smt.heart_rate_variability_data.heart_rate_variability_t.numBeatPos = 8;
                  hrv_reported = 1;
              } else if (hrv_reported == 1) {
                  smt.heart_rate_variability_data.sensor_type = SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR;
                  smt.heart_rate_variability_data.time_stamp = xSMLastExecutionTime;
                  smt.heart_rate_variability_data.heart_rate_variability_t.SDNN = hrv_output[0];
                  smt.heart_rate_variability_data.heart_rate_variability_t.LF = hrv_output[1];
                  smt.heart_rate_variability_data.heart_rate_variability_t.HF = hrv_output[2];
                  smt.heart_rate_variability_data.heart_rate_variability_t.LF_HF= hrv_output[3];
                  smt.heart_rate_variability_data.heart_rate_variability_t.beatPos = NULL;
                  smt.heart_rate_variability_data.heart_rate_variability_t.numBeatPos = 0;
                  hrv_reported = 2;
            }
            #endif
        }
    } else if (SENSOR_TYPE_BLOOD_PRESSURE_MONITOR == sensor_type) {
        if (smt.blood_pressure_descriptor.get_result != NULL) {
            algorithm_result.data = &(smt.blood_pressure_data);
            #ifndef BLOOD_PRESSURE_OUT_PATTERN
            smt.blood_pressure_descriptor.get_result(&algorithm_result);
            SM_INFO("Blood pressure sbp(%ld), dbp(%ld), status(%ld) \r\n", smt.blood_pressure_data.blood_pressure_t.sbp,
                smt.blood_pressure_data.blood_pressure_t.dbp, smt.blood_pressure_data.blood_pressure_t.status);
            #else
              if (bp_reported == 0) {
                  smt.blood_pressure_data.sensor_type = SENSOR_TYPE_BLOOD_PRESSURE_MONITOR;
                  smt.blood_pressure_data.time_stamp = xSMLastExecutionTime;
                  smt.blood_pressure_data.blood_pressure_t.sbp = 0;
                  smt.blood_pressure_data.blood_pressure_t.dbp = 0;
                  smt.blood_pressure_data.blood_pressure_t.status = 3;
                  smt.blood_pressure_data.blood_pressure_t.pwtt = pwtt_out_pattern;
                  smt.blood_pressure_data.blood_pressure_t.numPwtt = 9;
                  bp_reported = 1;
              } else if (bp_reported == 1) {
                  smt.blood_pressure_data.sensor_type = SENSOR_TYPE_BLOOD_PRESSURE_MONITOR;
                  smt.blood_pressure_data.time_stamp = xSMLastExecutionTime;
                  smt.blood_pressure_data.blood_pressure_t.sbp = 130;
                  smt.blood_pressure_data.blood_pressure_t.dbp = 80;
                  smt.blood_pressure_data.blood_pressure_t.status = -1;
                  smt.blood_pressure_data.blood_pressure_t.pwtt = NULL;
                  smt.blood_pressure_data.blood_pressure_t.numPwtt = 0;
                  bp_reported = 2;
              }
            #endif
        }
    } else if (SENSOR_TYPE_MOTION_DETECT == sensor_type) {
        if (smt.motion_detect_descriptor.get_result != NULL) {
            algorithm_result.data = &(smt.motion_detect_data);
            smt.motion_detect_descriptor.get_result(&algorithm_result);
        }
    } else if (SENSOR_TYPE_ELECTRIC_HEART_RATE_MONITOR == sensor_type) {
        if (smt.heart_rate_ecg_monitor_descriptor.get_result != NULL) {
            algorithm_result.data = &(smt.heart_rate_monitor_ecg_data);
            smt.heart_rate_ecg_monitor_descriptor.get_result(&algorithm_result);
        }
    } else {
        ret = -1;
        return ret;
    }

    int i;
    sensor_data_t *const output = &algorithm_result;
    for (i = 0; i < SENSOR_TYPE_ALL; i++) {
        if ( subscriber_list[i].send_digest != NULL && sensor_type == subscriber_list[i].type ) {
            SM_DBG("%ld %ld %ld %ld report output\r\n", output->data[0].value[0], output->data[0].value[1], output->data[0].value[2], output->data[0].value[3]);
            subscriber_list[subscriber_list[i].type].send_digest(output);
        }
    }

    return ret;
}

int32_t sensor_subscribe_sensor(sensor_subscriber_t *subscriber)
{
    //TODO change to multi-app subscribe

    //TODO get unique subscriber_list index
    int index = subscriber->type;

    //store app subscriber in sensor manager

    /* Store the suscriber name. */
    uint32_t i = 0;
    for( i = 0; i < SENSOR_MAX_SUBSCRIBER_NAME_LEN; i++ )
    {
        subscriber_list[index].name[i] = subscriber->name[i];
        if( subscriber->name[i] == 0x00 ) {
            break;
        }
    }
    subscriber_list->name[ SENSOR_MAX_SUBSCRIBER_NAME_LEN - 1 ] = '\0';
    //strcpy(subscriber_list[index].name , subscriber->name);//TODO subscribe index generate

    subscriber->handle =  subscriber->type;//TODO assign by sensor manager when multi-app subscribe
    subscriber_list[index].handle = subscriber->type;//TODO assign by sensor manager when multi-app subscribe
    subscriber_list[index].type = subscriber->type;
    subscriber_list[index].delay = subscriber->delay;
    subscriber_list[index].timeout = subscriber->timeout;
    subscriber_list[index].send_digest = subscriber->send_digest;

    //TODO if multi-app subscribe , need check duplicate subscribe

    send_sensor_subscribe_event( subscriber->type , 1, subscriber->delay, subscriber->timeout);
    return 1;
}

int32_t sensor_unsubscribe_sensor(int32_t handle)
{
    //TODO when change multi-app subscribe , need modification

    sensor_subscriber_t subscriber = subscriber_list[handle];//TODO handle to subscriber_t will different in multi-app subscribe

    //TODO check handle to subscribe_t exist
    send_sensor_subscribe_event(subscriber.type , 0, subscriber.delay, subscriber.timeout);

    memset(&subscriber_list[handle], 0, sizeof(sensor_subscriber_t));
    return 1;
}

//+++ sensor throttling start
uint32_t is_sensor_enabled(uint32_t sensor_type)
{
    return all_sensor_ctrl_data[sensor_type].enable;
}

sensor_input_list_t *get_algorithm_input_list(uint32_t sensor_type)
{
    sensor_input_list_t *tmp_input_list = NULL;
    switch (sensor_type) {
        case SENSOR_TYPE_PEDOMETER:
            tmp_input_list = smt.pedometer_descriptor.input_list;
            break;
        case SENSOR_TYPE_SLEEP:
            tmp_input_list = smt.sleep_descriptor.input_list;
            break;
        case SENSOR_TYPE_SLEEP_STAGING:
            tmp_input_list = smt.sleep_staging_descriptor.input_list;
            break;
        case SENSOR_TYPE_HEART_RATE_MONITOR:
            tmp_input_list = smt.heart_rate_monitor_descriptor.input_list;
            break;
        case SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR:
            tmp_input_list = smt.heart_rate_variability_descriptor.input_list;
            break;
        case SENSOR_TYPE_BLOOD_PRESSURE_MONITOR:
            tmp_input_list = smt.blood_pressure_descriptor.input_list;
            break;
        case SENSOR_TYPE_MOTION_DETECT:
            tmp_input_list = smt.motion_detect_descriptor.input_list;
            break;
        case SENSOR_TYPE_ELECTRIC_HEART_RATE_MONITOR:
            tmp_input_list = smt.heart_rate_ecg_monitor_descriptor.input_list;
            break;
        default:
            tmp_input_list = NULL;
            SM_ERR("get_algorithm_input_list: illegal sensor type %ld \r\n", sensor_type);
            break;
    }
    return tmp_input_list;
}

uint32_t get_algorithm_used_hw_sensor_delay(int algorithm_type, int hw_sensor_type)
{
    uint32_t hw_sensor_delay = SENSOR_DRIVER_MAX_DELAY;
    sensor_input_list_t *tmp_input_list;
    tmp_input_list = get_algorithm_input_list(algorithm_type);

    while (tmp_input_list != NULL) {
        if (hw_sensor_type == tmp_input_list->input_type) {
            hw_sensor_delay = tmp_input_list->sampling_delay;
            break;
        } else {
            tmp_input_list = tmp_input_list->next_input;
        }
    }

    return hw_sensor_delay;
}

uint32_t get_algorithm_used_hw_sensor_timeout(int algorithm_type, int hw_sensor_type)
{
    uint32_t hw_sensor_timeout = SENSOR_DRIVER_MAX_DELAY;
    sensor_input_list_t *tmp_input_list;
    tmp_input_list = get_algorithm_input_list(algorithm_type);

    while (tmp_input_list != NULL) {
        if (hw_sensor_type == tmp_input_list->input_type) {
            hw_sensor_timeout = tmp_input_list->timeout;
            break;
        } else {
            tmp_input_list = tmp_input_list->next_input;
        }
    }

    return hw_sensor_timeout;
}

uint32_t recalcuate_new_hw_sensor_delay(uint32_t hw_sensor_type)
{
    int i;
    uint32_t optimized_hw_sensor_delay = SENSOR_DRIVER_MAX_DELAY;
    uint32_t tmp_hw_sensor_delay = 0;
    // check hw sensor
    if ( all_sensor_ctrl_data[hw_sensor_type].enable == 1) {
        optimized_hw_sensor_delay = all_sensor_ctrl_data[hw_sensor_type].delay;
    }
    // check algorithm
    for (i = SENSOR_VIRTUAL_TYPE_START; i < SENSOR_TYPE_ALL; i++) {
        if ( all_sensor_ctrl_data[i].enable == 1 ) {
            tmp_hw_sensor_delay = get_algorithm_used_hw_sensor_delay(i, hw_sensor_type);
            if ( tmp_hw_sensor_delay > 0 && tmp_hw_sensor_delay < optimized_hw_sensor_delay ) {
                optimized_hw_sensor_delay = tmp_hw_sensor_delay;
            }
        }
    }
    return optimized_hw_sensor_delay;
}

uint32_t recalcuate_new_hw_sensor_timeout(uint32_t hw_sensor_type)
{
    int i;
    uint32_t optimized_hw_sensor_timeout = SENSOR_DRIVER_MAX_DELAY;
    uint32_t tmp_hw_sensor_timeout = 0;
    // check hw sensor
    if ( all_sensor_ctrl_data[hw_sensor_type].enable == 1) {
        optimized_hw_sensor_timeout = all_sensor_ctrl_data[hw_sensor_type].timeout;
    }
    // check algorithm
    for (i = SENSOR_VIRTUAL_TYPE_START; i < SENSOR_TYPE_ALL; i++) {
        if ( all_sensor_ctrl_data[i].enable == 1 ) {
            tmp_hw_sensor_timeout = get_algorithm_used_hw_sensor_timeout(i, hw_sensor_type);
            if ( tmp_hw_sensor_timeout > 0 && tmp_hw_sensor_timeout < optimized_hw_sensor_timeout ) {
                optimized_hw_sensor_timeout = tmp_hw_sensor_timeout;
            }
        }
    }
    return optimized_hw_sensor_timeout;
}

uint32_t is_algorithm_with_hw_sensor(int algorithm_type, int hw_sensor_type)
{
    uint32_t hw_sensor_enable = 0;
    sensor_input_list_t *tmp_input_list;
    tmp_input_list = get_algorithm_input_list(algorithm_type);

    while (tmp_input_list != NULL) {
        if (hw_sensor_type == tmp_input_list->input_type) {
            hw_sensor_enable = 1;
            break;
        } else {
            tmp_input_list = tmp_input_list->next_input;
        }
    }

    return hw_sensor_enable;
}

uint32_t recalcuate_new_hw_sensor_enable(uint32_t hw_sensor_type)
{
    int i;
    uint32_t optimized_hw_sensor_enable = 0;
    // check hw sensor
    if ( all_sensor_ctrl_data[hw_sensor_type].enable == 1) {
        optimized_hw_sensor_enable = 1;
    }
    // check algorithm
    for (i = SENSOR_VIRTUAL_TYPE_START; i < SENSOR_TYPE_ALL; i++) {
        if ( all_sensor_ctrl_data[i].enable == 1 ) {
            if ( is_algorithm_with_hw_sensor(i, hw_sensor_type) == 1 ) {
                optimized_hw_sensor_enable = 1;
            }
        }
    }
    return optimized_hw_sensor_enable;
}

void dump_all_sensor_ctrl_data()
{
    int i = 0;
    for (i = 0; i < SENSOR_TYPE_ALL; i++) {
        SM_DBG("type=%d enable=%ld delay=%ld timeout=%ld\r\n", i, all_sensor_ctrl_data[i].enable, all_sensor_ctrl_data[i].delay, all_sensor_ctrl_data[i].timeout);
    }
}

static int is_timer_stop;
static int32_t sensor_throttling_control(uint32_t sensor_type, uint32_t enable, uint32_t delay, uint32_t timeout)
{
    int32_t ret = 0;
    uint32_t optimized_hw_sensor_enable[SENSOR_PHYSICAL_TYPE_ALL];
    uint32_t optimized_hw_sensor_delay[SENSOR_PHYSICAL_TYPE_ALL];
    uint32_t optimized_hw_sensor_timeout[SENSOR_PHYSICAL_TYPE_ALL];
    int i;
    sensor_input_list_t *tmp_input_list = NULL;
    uint32_t finetuned_polling_sensors_delay = 0;
    uint32_t tmp_timeout = 0;

    uint32_t supported_algo = get_defined_sensor();
    if ( (supported_algo & (1U << sensor_type) ) == 0 ) {
        SM_INFO("sensor type %ld undefined \r\n", sensor_type);
        return -1;
    }

    SM_INFO("type(%ld), enable(%ld), delay(%ld), timeout(%ld)\r\n", sensor_type, enable, delay, timeout);

    // get current hw sensor enable,delay configuration
    for (i = 0; i < SENSOR_PHYSICAL_TYPE_ALL; i++) {
        optimized_hw_sensor_enable[i] = sensor_driver_get_enable(i);
        optimized_hw_sensor_delay[i] = sensor_driver_get_delay(i);
        optimized_hw_sensor_timeout[i] = sensor_driver_get_batch_timeout(i);
    }
    //log
    //dump_all_sensor_ctrl_data();

    // check cmd with new sensor config
    if ( (enable == all_sensor_ctrl_data[sensor_type].enable) && (delay == all_sensor_ctrl_data[sensor_type].delay) && (timeout== all_sensor_ctrl_data[sensor_type].timeout)) {
        SM_DBG("recv (enable,delay, timeout) is the same with current sensor ctrl data \r\n");
        return 0;
    }

    SM_DBG("original_sensor_ctrl enable=%ld delay=%ld\r\n", all_sensor_ctrl_data[sensor_type].enable, all_sensor_ctrl_data[sensor_type].delay, all_sensor_ctrl_data[sensor_type].timeout);

    all_sensor_ctrl_data[sensor_type].enable = enable;
    all_sensor_ctrl_data[sensor_type].delay = delay;
    all_sensor_ctrl_data[sensor_type].timeout = timeout;

    //log
    dump_all_sensor_ctrl_data();

    // optimize hw sensor delay & enable
    if (sensor_type < SENSOR_PHYSICAL_TYPE_ALL) {
        //hw sensor
        SM_DBG("hw sensor cmd\r\n");
        optimized_hw_sensor_delay[sensor_type] = recalcuate_new_hw_sensor_delay(sensor_type);
        optimized_hw_sensor_timeout[sensor_type] = recalcuate_new_hw_sensor_timeout(sensor_type);
        optimized_hw_sensor_enable[sensor_type] = recalcuate_new_hw_sensor_enable(sensor_type);
    } else {

        tmp_input_list =  get_algorithm_input_list(sensor_type);
        while (tmp_input_list != NULL) {
            SM_DBG("tmp_input_list type=%ld,delay=%ld\r\n", (uint32_t)tmp_input_list->input_type, tmp_input_list->sampling_delay);
            optimized_hw_sensor_delay[tmp_input_list->input_type] = recalcuate_new_hw_sensor_delay(tmp_input_list->input_type);
            optimized_hw_sensor_timeout[tmp_input_list->input_type] = recalcuate_new_hw_sensor_timeout(tmp_input_list->input_type);
            optimized_hw_sensor_enable[tmp_input_list->input_type] = recalcuate_new_hw_sensor_enable(tmp_input_list->input_type);
            tmp_input_list = tmp_input_list->next_input;
        }
    }

    /* enable hw sensor */
    char sensors_dump[160] = {0};
    uint32_t log_len = 0;

    for (i = 0; i < SENSOR_PHYSICAL_TYPE_ALL; i++) {
        snprintf(sensors_dump+log_len, 160-log_len, "%ld,%ld,%ld|", optimized_hw_sensor_enable[i], optimized_hw_sensor_delay[i], optimized_hw_sensor_timeout[i]);
        log_len = strlen(sensors_dump);

        if (optimized_hw_sensor_enable[i] != 0) {
            sensor_driver_set_delay(i, optimized_hw_sensor_delay[i]);
            sensor_driver_set_batch_timeout(i, optimized_hw_sensor_timeout[i]);
        }

        //TODO Need to turn on ACC, then GYRO
        sensor_driver_enable(i, optimized_hw_sensor_enable[i]);

    }
    SM_INFO("%s\n", sensors_dump);

    /* enable algorithm*/
    //algorithm
    SM_DBG("algorithm cmd sensor_type = %lu \r\n", sensor_type);
    switch (sensor_type) {
#if defined(FUSION_PEDOMETER_USE)
        case SENSOR_TYPE_PEDOMETER:
            if (enable != 0) {
                pedometer_init();
            } else {
                pedometer_deinit();
            }
            break;
#endif

#if defined(FUSION_SLEEP_TRACKER_USE)
        case SENSOR_TYPE_SLEEP:
            if (enable != 0) {
            #ifdef SLEEP_TRACKER_OUT_PATTERN
                st_start_time = xSMLastExecutionTime;
            #endif
                smt.sleep_data = (sensor_data_unit_t *) pvPortMalloc(sizeof(sensor_data_unit_t) * smt.sleep_data_buffer_number);
                sleep_init();
            } else {
            #ifdef SLEEP_TRACKER_OUT_PATTERN
                st_reported = 0;
            #endif
                vPortFree(smt.sleep_data);
                smt.sleep_data = NULL;
            }
            break;
#endif

#if defined(FUSION_SLEEP_STAGING_USE)
        case SENSOR_TYPE_SLEEP_STAGING:
            if (enable != 0) {
                smt.sleep_staging_data = (sensor_data_unit_t *) pvPortMalloc(sizeof(sensor_data_unit_t) * smt.sleep_staging_buffer_number);
                sleep_staging_init();
            } else {
                vPortFree(smt.sleep_staging_data);
                smt.sleep_staging_data = NULL;
            }
            break;
#endif

#if defined(FUSION_HEART_RATE_MONITOR_USE)
        case SENSOR_TYPE_HEART_RATE_MONITOR:
            if (enable != 0) {
                heart_rate_monitor_init();
            }
            break;
#endif

#if defined(FUSION_HEART_RATE_VARIABILITY_USE)
        case SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR:
            if (enable != 0) {
            #ifdef HEART_RATE_VARIABILITY_OUT_PATTERN
                hrv_start_time = xSMLastExecutionTime;
            #endif
                heart_rate_variability_monitor_init();
            } else {
            #ifdef HEART_RATE_VARIABILITY_OUT_PATTERN
                hrv_reported = 0;
            #endif
            }
            break;
#endif

#if defined(FUSION_BLOOD_PRESSURE_USE)
        case SENSOR_TYPE_BLOOD_PRESSURE_MONITOR:
            if (enable != 0) {
            #ifdef BLOOD_PRESSURE_OUT_PATTERN
                bp_start_time = xSMLastExecutionTime;
            #endif
                blood_pressure_monitor_init();
            } else {
            #ifdef BLOOD_PRESSURE_OUT_PATTERN
                bp_reported = 0;
            #endif
            }
            break;
#endif

#ifdef AUTOMATIC_MOTION_DETECT
case SENSOR_TYPE_MOTION_DETECT:
            if (enable != 0) {
                motion_detect_init();
            }
            break;
#endif

#ifdef FUSION_HEART_RATE_BY_ECG_USE
case SENSOR_TYPE_ELECTRIC_HEART_RATE_MONITOR:
            if (enable != 0) {
                heart_rate_monitor_ecg_init();
            }
            break;
#endif
        default:
            break;
    }
    // handle polling or batch mode timeout sensor
    finetuned_polling_sensors_delay = 0;
    for( i=0; i < SENSOR_PHYSICAL_TYPE_ALL ;i++) {
        if( sensor_driver_get_event_type(i) == 1){
            //polling sensor
            if( sensor_driver_get_enable(i) > 0){
                if( finetuned_polling_sensors_delay == 0 ) {
                    finetuned_polling_sensors_delay = sensor_driver_get_batch_timeout(i);
                    if (finetuned_polling_sensors_delay == 0) {
                        /* This sensor does not support batch */
                    finetuned_polling_sensors_delay = sensor_driver_get_delay(i);
                    }
                }else {
                    tmp_timeout = sensor_driver_get_batch_timeout(i);
                    if (tmp_timeout == 0) {
                        /* This sensor does not support batch */
                        tmp_timeout = sensor_driver_get_delay(i);
                    }
                    if( finetuned_polling_sensors_delay >  tmp_timeout ) {
                        finetuned_polling_sensors_delay = tmp_timeout;
                    }
                }
            }
        }
    }

    SM_INFO("finetuned_polling_sensors_delay = %ld\r\n", finetuned_polling_sensors_delay);
    if( finetuned_polling_sensors_delay == 0 ){
        //stop timeout timer
        SM_INFO("stop timer\r\n");
        xTimerStop( xTimerofMems, 1000 / portTICK_PERIOD_MS);
        is_timer_stop = 1;
    } else {

        smt.polling_period = finetuned_polling_sensors_delay;
        xTimerChangePeriod( xTimerofMems, smt.polling_period / portTICK_PERIOD_MS, 10000 / portTICK_PERIOD_MS);
        if (is_timer_stop == 1) {
            SM_INFO("reset timer\r\n");
            xTimerReset(xTimerofMems, 1000 / portTICK_PERIOD_MS);
            is_timer_stop = 0;
        }
    }

    return ret;
}

static void send_sensor_subscribe_event(uint32_t sensor_type, uint32_t enable, uint32_t delay, uint32_t timeout)
{
    // BaseType_t xHigherPriorityTaskWoken;
    cmd_event_t send_cmd;
    send_cmd.event = SM_EVENT_SUBSCRIBE;
    send_cmd.sensor_type = sensor_type;
    send_cmd.enable = enable;
    send_cmd.delay = delay;
    send_cmd.timeout = timeout;

    SM_INFO("send cmd type = %ld | enable = %ld | delay = %ld | timeout = %ld \r\n", send_cmd.sensor_type, send_cmd.enable, send_cmd.delay, send_cmd.timeout);

    if ( xQueueSendToBack( sm_queue_handle, (void *) &send_cmd , (TickType_t) 0 ) != pdPASS ) {
        //TODO send msg fail , how to re-send again
        SM_ERR("send subscribe event fail \r\n");
    }
}


//--- sensor throttling end

int32_t sensor_get_latest_sensor_data(uint32_t type, sensor_data_t *const output)
{
    int ret = 0;
    output->data->sensor_type = type;
    output->data_exist_count = 1;
    output->fifo_max_size = 0;

    switch (type) {
        case SENSOR_TYPE_ACCELEROMETER:
            output->data = &(smt.mgr_acc_data[0]);
            break;
        case SENSOR_TYPE_GYROSCOPE:
            output->data = &(smt.mgr_gyro_data[0]);
            break;
        case SENSOR_TYPE_MAGNETIC_FIELD:
            output->data = &(smt.mgr_mag_data);
            break;
        case SENSOR_TYPE_PRESSURE:
            output->data = &(smt.mgr_baro_data);
            break;
        case SENSOR_TYPE_PEDOMETER:
            output->data = &(smt.pedometer_data);
            break;
        case SENSOR_TYPE_SLEEP:
            output->data = smt.sleep_data;
            break;
        case SENSOR_TYPE_SLEEP_STAGING:
            output->data = smt.sleep_staging_data;
            break;
        case SENSOR_TYPE_HEART_RATE_MONITOR:
            output->data = &(smt.heart_rate_monitor_data);
            break;
        default:
            output->data = NULL;
            SM_ERR("get_latest_sensor_data wrong  type %ld \r\n", (int32_t)type);
            //return 0;
            break;
    }
    return ret;
}

int sensor_manager_set_cust_from_app(uint32_t sensor_type, void *in_data_buf, int32_t in_size)
{
    int err = 0;
    int value = 0;
    cmd_event_t send_cmd;

    value = *(int *)in_data_buf;

    switch (sensor_type) {
        case SENSOR_TYPE_GYROSCOPE:
            if ( value == 0 ) {
                SM_INFO("send calibration event\r\n");
                send_cmd.event = SM_EVENT_CALIBRATION;
                send_cmd.sensor_type = sensor_type;
                if ( xQueueSendToBack( sm_queue_handle, (void *) &send_cmd , (TickType_t) 0 ) != pdPASS ) {
                    SM_ERR("send calibration event fail \r\n");
                }
            }
            break;
        default:
            SM_ERR("sensor_manager_set_cust_from_app wrong  type %ld \r\n", (int32_t)sensor_type);
            break;
    }
    return err;
}
