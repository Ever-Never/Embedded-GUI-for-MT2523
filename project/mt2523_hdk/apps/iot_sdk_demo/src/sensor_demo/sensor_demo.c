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

#include "sensor_demo.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include <string.h>

#ifdef SENSOR_DEMO
/* syslog */
#define SENSOR_DEMO_LOGI(fmt,...) LOG_I(sensor,  (fmt), ##__VA_ARGS__)
/* printf*/
//#define SENSOR_DEMO_LOGI(fmt,arg...)   printf("SENSOR_DEMO:INFO: "fmt,##arg)

/* sensor subsys includes */
#include "sensor_alg_interface.h"

static uint32_t acc_count, gyro_count, mag_count, baro_count;
static int32_t all_sensor_send_digest_callback(sensor_data_t *const output);

#ifdef FUSION_HEART_RATE_MONITOR_USE
static sensor_subscriber_t heart_rate_monitor_subscriber = {
    "ap21", 0, SENSOR_TYPE_HEART_RATE_MONITOR, 0, all_sensor_send_digest_callback, 0
};
uint32_t hr_triggered;
#endif

#ifdef FUSION_HEART_RATE_VARIABILITY_USE
static sensor_subscriber_t heart_rate_variability_subscriber = {
    "ap22", 0, SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR, 0, all_sensor_send_digest_callback, 0
};
static uint32_t hrv_triggered;
#endif

#ifdef FUSION_BLOOD_PRESSURE_USE
static sensor_subscriber_t blood_pressure_subscriber = {
    "ap23", 0, SENSOR_TYPE_BLOOD_PRESSURE_MONITOR, 0, all_sensor_send_digest_callback, 0
};
static uint32_t bp_triggered;
#endif

/**
* @brief The callback function for sensor data.
* @param[in] output, the sensor data.
* @return 0, if the operation completed successfully.\n
*/
static int32_t all_sensor_send_digest_callback(sensor_data_t *const output)
{
    switch (output->data[0].sensor_type) {
        case SENSOR_TYPE_ACCELEROMETER:
        if ((acc_count++) % 400 == 0) {
            SENSOR_DEMO_LOGI("type = %lu , value = ( %ld , %ld , %ld )(mm/s^2) , timestampe = %lu \r\n",
                   output->data[0].sensor_type,
                   output->data[0].accelerometer_t.x,
                   output->data[0].accelerometer_t.y,
                   output->data[0].accelerometer_t.z,
                   output->data[0].time_stamp);
        }
            break;

        case SENSOR_TYPE_MAGNETIC_FIELD:
            if ((mag_count++) % 200 == 0) {
                SENSOR_DEMO_LOGI("type = %lu , value = (%ld.%03ld, %ld.%03ld, %ld.%03ld) uT Status(%ld), timestampe = %lu \r\n",
                       output->data[0].sensor_type,
                       output->data[0].magnetic_t.x/1000, output->data[0].magnetic_t.x%1000,
                       output->data[0].magnetic_t.y/1000, output->data[0].magnetic_t.y%1000,
                       output->data[0].magnetic_t.y/1000, output->data[0].magnetic_t.y%1000,
                       output->data[0].magnetic_t.status,
                       output->data[0].time_stamp
                       );
            }
            break;

        case SENSOR_TYPE_PRESSURE:
            if ((baro_count++) % 200 == 0) {
                SENSOR_DEMO_LOGI("type = %lu , value = (%ld.%02ld) hPa , timestampe = %lu \r\n",
                       output->data[0].sensor_type,
                       output->data[0].pressure_t.pressure / 100,
                       output->data[0].pressure_t.pressure % 100,
                       output->data[0].time_stamp);
            }
            break;
        case SENSOR_TYPE_GYROSCOPE:
        if ((gyro_count++) % 400 == 0) {
            SENSOR_DEMO_LOGI("type = %lu , value = ( %ld , %ld , %ld )(degree/s) , timestampe = %lu \r\n",
                   output->data[0].sensor_type,
                   output->data[0].accelerometer_t.x,
                   output->data[0].accelerometer_t.y,
                   output->data[0].accelerometer_t.z,
                   output->data[0].time_stamp);
        }
            break;

        case SENSOR_TYPE_PROXIMITY:
            SENSOR_DEMO_LOGI("type = %lu , value = (%ld) , timestampe = %lu \r\n",
               output->data[0].sensor_type,
               output->data[0].distance,
               output->data[0].time_stamp);
            break;

        case SENSOR_TYPE_HEART_RATE_MONITOR:
            SENSOR_DEMO_LOGI("Heart rate , bpm = %ld , timestampe = %lu \r\n",
               output->data[0].heart_rate_t.bpm,
               output->data[0].time_stamp);

            update_hr_data(output->data[0].heart_rate_t.bpm);

            break;

        case SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR:
        {
            SENSOR_DEMO_LOGI("Heart rate variability: T(%ld), SDNN(%ld), LF(%ld), HF(%ld), LF_HF(%ld)\r\n", output->data[0].time_stamp, output->data[0].heart_rate_variability_t.SDNN,
                output->data[0].heart_rate_variability_t.LF, output->data[0].heart_rate_variability_t.HF, output->data[0].heart_rate_variability_t.LF_HF);

            update_hrv_data(
                    output->data[0].heart_rate_variability_t.SDNN,
                    output->data[0].heart_rate_variability_t.LF,
                    output->data[0].heart_rate_variability_t.HF,
                    output->data[0].heart_rate_variability_t.LF_HF);

            /* To unsubscribe here? */
#ifdef FUSION_HEART_RATE_VARIABILITY_USE
            int32_t unsubscription;

            unsubscription = sensor_unsubscribe_sensor(heart_rate_variability_subscriber.handle);
            if (unsubscription < 0) {
                SENSOR_DEMO_LOGI("HRV unsubscription fail\r\n");
            } else {
                hrv_triggered = 0;
                SENSOR_DEMO_LOGI("HRV unsubscribed\r\n");
            }
#endif
            break;
        }

        case SENSOR_TYPE_BLOOD_PRESSURE_MONITOR:
        {
            if (output->data[0].blood_pressure_t.pwtt == NULL) {
                SENSOR_DEMO_LOGI("B.P: T(%ld), sdp(%ld), dbp(%ld), status(%ld) \r\n", output->data[0].time_stamp, output->data[0].blood_pressure_t.sbp,
                    output->data[0].blood_pressure_t.dbp, output->data[0].blood_pressure_t.status);

                update_bp_data(
                        output->data[0].blood_pressure_t.sbp,
                        output->data[0].blood_pressure_t.dbp);

                /* To unsubscribe here? */
#ifdef FUSION_BLOOD_PRESSURE_USE
                int32_t unsubscription;

                unsubscription = sensor_unsubscribe_sensor(blood_pressure_subscriber.handle);
                if (unsubscription < 0) {
                    SENSOR_DEMO_LOGI("B.P. unsubscription fail\r\n");
                } else {
                    bp_triggered = 0;
                    SENSOR_DEMO_LOGI("B.P. unsubscribed\r\n");
                }
#endif

#ifdef FUSION_HEART_RATE_MONITOR_USE
                int32_t subscription;

                if (hr_triggered == 2) {
                    subscription = sensor_subscribe_sensor(&heart_rate_monitor_subscriber);
                    if (subscription < 0) {
                        SENSOR_DEMO_LOGI("HR subscription fail\r\n");
                    } else {
                        hr_triggered = 1;
                        SENSOR_DEMO_LOGI("HR subscribed\r\n");
                    }
                }
#endif
            }

            break;
        }

        default:
            SENSOR_DEMO_LOGI("type = %lu , value = ( %ld %ld %ld ) , timestamp = %lu \r\n",
                   output->data[0].sensor_type,
                   output->data[0].value[0],
                   output->data[0].value[1],
                   output->data[0].value[2],
                   output->data[0].time_stamp);
            break;
    }

    return 0;
}

/**
  * @brief  Enable the on-change sensors according the preprocessor options.
  * @param  None.
  * @retval None.
  */
void enable_all_sensors(void)
{
    int32_t subscription = 0;

    (void)subscription;

#ifdef FUSION_PEDOMETER_USE
    subscription = sensor_subscribe_sensor(&pedometer_subscriber);
    if (subscription < 0) {
        SENSOR_DEMO_LOGI("Pedometer subscription fail\r\b");
    }
#endif

#ifdef FUSION_SLEEP_TRACKER_USE
    subscription = sensor_subscribe_sensor(&sleep_tracker_subscriber);
    if (subscription < 0) {
        SENSOR_DEMO_LOGI("Sleep tracker subscription fail\r\b");
    }
#endif

#ifdef FUSION_HEART_RATE_MONITOR_USE
    if (hr_triggered == 0) {
        subscription = sensor_subscribe_sensor(&heart_rate_monitor_subscriber);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("HR subscription fail\r\n");
        }
        else {
            SENSOR_DEMO_LOGI("HR triggered\r\n");
            hr_triggered = 1;
        }
    }
#endif
}

/**
  * @brief  Disable the on-change sensors according the preprocessor options.
  * @param  None.
  * @retval None.
  */
void disable_all_sensors(void)
{
    int32_t unsubscription = 0;

    (void)unsubscription;

#ifdef FUSION_HEART_RATE_MONITOR_USE
    if (hr_triggered == 1) {
        unsubscription = sensor_unsubscribe_sensor(heart_rate_monitor_subscriber.handle);
        if (unsubscription < 0) {
            SENSOR_DEMO_LOGI("HR unsubscription fail\r\b");
        } else {
            hr_triggered = 0;
            SENSOR_DEMO_LOGI("HR unsubscribed\r\n");
        }
    }
#endif

#ifdef FUSION_HEART_RATE_VARIABILITY_USE
    if (hrv_triggered == 1) {
        unsubscription = sensor_unsubscribe_sensor(heart_rate_variability_subscriber.handle);
        if (unsubscription < 0) {
            SENSOR_DEMO_LOGI("HRV unsubscription fail\r\n");
        } else {
            hrv_triggered = 0;
            SENSOR_DEMO_LOGI("HRV unsubscribed\r\n");
        }
    }
#endif

#ifdef FUSION_BLOOD_PRESSURE_USE
    if (bp_triggered == 1) {
        unsubscription = sensor_unsubscribe_sensor(blood_pressure_subscriber.handle);
        if (unsubscription < 0) {
            SENSOR_DEMO_LOGI("B.P. unsubscription fail\r\n");
        } else {
            bp_triggered = 0;
            SENSOR_DEMO_LOGI("B.P. unsubscribed\r\n");
        }
    }
#endif
}

#ifdef FUSION_HEART_RATE_VARIABILITY_USE
/**
  * @brief  Enable the heart rate variability sensor (of one-shot type).
  * @param  None.
  * @retval None.
  */
void enable_hrv(void)
{
    int32_t subscription = 0;

    (void)subscription;

    if (hrv_triggered == 0) {
        subscription = sensor_subscribe_sensor(&heart_rate_variability_subscriber);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("Heart rate variability subscription fail\r\n");
        }
        else {
            SENSOR_DEMO_LOGI("Heart rate variability triggered\r\n");
            hrv_triggered = 1;
        }
    }
}
#endif

#ifdef FUSION_BLOOD_PRESSURE_USE
/**
  * @brief  Enable the blood pressure sensor (of one-shot type).
  * @param  None.
  * @retval None.
  */
void enable_bp(void)
{
    int32_t subscription = 0;

    (void)subscription;

    /* B.P. usage is exclusive with Heart rate*/

#ifdef FUSION_HEART_RATE_MONITOR_USE
    if (hr_triggered == 1) {
        subscription = sensor_unsubscribe_sensor(heart_rate_monitor_subscriber.handle);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("HR unsubscription fail\r\b");
        } else {
            hr_triggered = 2;
            SENSOR_DEMO_LOGI("HR unsubscribed\r\n");
        }
    }
#endif

    if (bp_triggered == 0) {
        subscription = sensor_subscribe_sensor(&blood_pressure_subscriber);
        if (subscription < 0) {
            SENSOR_DEMO_LOGI("B.P. subscription fail\r\n");
        }
        else {
            SENSOR_DEMO_LOGI("B.P. triggered\r\n");
            bp_triggered = 1;
        }
    }
}
#endif

#else
void enable_all_sensors(void)
{

}
void disable_all_sensors(void)
{

}
#endif /*SENSOR_DEMO*/

