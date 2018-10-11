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

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal.h"

#include "syslog.h"
#include "sys_init.h"

/* sensor subsys includes */
#include "mems_bus.h"
#include "sensor_manager.h" /* sensor manager task */
#include "sensor_alg_interface.h"

#ifdef MTK_SENSOR_ACCELEROMETER_USE_BMA255
#include "bma255_sensor_adaptor.h"
#endif

#ifdef MTK_SENSOR_ACCELEROMETER_GYROSCOPE_USE_BMI160
#include "bmi160.h"
#endif

#include "task_def.h"

static  int32_t sensor_send_digest_callback(sensor_data_t *const output);

sensor_subscriber_t acc_subscriber = {
    "ap0", 0, SENSOR_TYPE_ACCELEROMETER, 10, sensor_send_digest_callback
};

sensor_subscriber_t gyro_subscriber = {
    "ap4", 0, SENSOR_TYPE_GYROSCOPE, 10, sensor_send_digest_callback
};

/**
* @brief The callback function for accerometer data.
* @param[in] output, the accerometer data.
* @return 0, if the operation completed successfully.\n
*/
static int32_t sensor_send_digest_callback(sensor_data_t *const output)
{
    static uint32_t acc_log_downsample = 0;
    static uint32_t gyro_log_downsample = 0;

    switch (output->data[0].sensor_type) {

        case SENSOR_TYPE_ACCELEROMETER:
            if ((acc_log_downsample%200) == 0) {
                LOG_I(sensor, "acc  value = ( %ld , %ld , %ld ) (%ld) , timestamp = %lu \r\n",
                       output->data[0].accelerometer_t.x,
                       output->data[0].accelerometer_t.y,
                       output->data[0].accelerometer_t.z,
                       output->data[0].accelerometer_t.status,
                       output->data[0].time_stamp
                      );
            }
            acc_log_downsample++;
            break;

        case SENSOR_TYPE_GYROSCOPE:
            if ((gyro_log_downsample%200) == 0) {
                LOG_I(sensor, "gyro value = ( %ld , %ld , %ld ) (%ld) , timestamp = %lu \r\n",
                       output->data[0].gyroscope_t.x,
                       output->data[0].gyroscope_t.y,
                       output->data[0].gyroscope_t.z,
                       output->data[0].gyroscope_t.status,
                       output->data[0].time_stamp
                      );
            }
            gyro_log_downsample++;
            break;

        default:
            LOG_I(sensor, "type = %lu,  timestamp = %lu \r\n",
                   output->data[0].sensor_type,
                   output->data[0].time_stamp);
            break;
    }
    return 0;
}

/**
  * @brief  Main function
  * @param  None.
  * @retval None.
  */
int main(void)
{
    /* Do system initialization, eg: hardware, nvdm. */
    system_init();

    /* system log initialization.
     * This is the simplest way to initialize system log, that just inputs three NULLs
     * as input arguments. User can use advanced feature of system log along with NVDM.
     * For more details, please refer to the log dev guide under /doc folder or projects
     * under project/mtxxxx_hdk/apps/.
     */
    log_init(NULL, NULL, NULL);

    /* Here, the infrastructure is setup and user can initialize function hardware like:
     * GPS, audio, LCD etc.
     * User can call initialization APIs below here or in user task at runtime.
     * EX:  gnss_init();
     *      hal_audio_init();
     *      bsp_lcd_init();
     *      hal_g2d_init();
     */
    sensor_manager_init();

    /* As for generic HAL init APIs like: hal_uart_init(), hal_gpio_init() and hal_spi_master_init() etc,
     * user can call them when they need, which means user can call them here or in user task at runtime.
     */

    int32_t subscription;
    subscription = sensor_subscribe_sensor(&acc_subscriber);
    if (subscription < 0) {
        LOG_E(sensor, "acc subscription fail\r\b");
    }
    subscription = sensor_subscribe_sensor(&gyro_subscriber);
    if (subscription < 0) {
        LOG_E(sensor, "gyro subscription fail\r\b");
    }


    /* Call this function to indicate the system initialize done. */
    SysInitStatus_Set();

    /* Start the scheduler. */
    vTaskStartScheduler();
    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for( ;; );
}

