/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
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

#ifndef __SENSOR_ALG_INTERFACE_H__
#define __SENSOR_ALG_INTERFACE_H__

/**
 * @addtogroup Sensor_Subsystem_Group Sensor Subsystem
 * @{
 * The sensor subsystem interface APIs could be used
 * to \ref sensor_driver_interface "port physical sensor drivers", to \ref sensor_fusion_algorithm_interface " port fusion algorithms", or  to \ref sensor_app_interface " subscribe to a specific sensor to acquire data",
 * as shown below.
 * @image html sensor_subsys_architecture.png
 * @section Sensor_Subsystem_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |<b>Sensor Subsystem</b>      |Sensor software stack, including physical sensor drivers, sensor manager, fusion algorithms and the interface APIs.|
 * |<b>Sensor Manager</b>    | Manages the buffer, the registration of physical sensor driver and fusion algorithm and the subscription from applications.|
 * |<b>Physical Sensor</b>     | Detects device-specific properties such as acceleration, rotation and environmental properties such as light, magnetic field and barometric pressure.|
 * |<b>Fusion Algorithm</b>   | Filters the output of physical sensors to provide abstract results, such as pedometer and activity recognition.|
 * |<b>PDR</b>  | Pedestrian dead reckoning, calculating one's current position by using a previously determined position or fix, and advancing that position based upon known or estimated speeds over elapsed time and course.|
 * |<b>EKG</b>   | Electrocardiography (ECG or EKG) is the process of recording the electrical activity of the heart over a period of time using electrodes placed on a patient's body.|
 * |<b>EEG</b>   | Electroencephalography (EEG) is an electrophysiological monitoring method to record electrical activity of the brain.|
 * |<b>EMG</b>   | Electromyography (EMG) is an electrodiagnostic medicine technique for evaluating and recording the electrical activity produced by skeletal muscles.|
 * |<b>GSR</b>   | Galvanic skin response (GSR) is an electrophysiological monitoring method to record conductance of the skin.|
 * |<b>PPG</b>   | Photoplethysmogram (PPG) is an optical volumetric measurement of an organ. By illuminating skin with LED, the PPG is measured from the light transmitted or reflected to photodiode.|
 * |<b>BISI</b>  | Beat interval (BI) and signal integrity (SI) detection. The BI is computed based on subtracting the peak position of the successive PPG beats. The SI is an indicator to identify the PPG signal integrity.|
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <math.h>

#if defined(__CC_ARM)
    #pragma anon_unions
#elif defined(__ICCARM__)
    #pragma language=extended
#elif defined(__GNUC__)
/* Anonymous unions are enabled by default. */
#endif

/**Accelerometer.*/
#define SENSOR_TYPE_ACCELEROMETER                   (0)
/**Magnetic field sensor.*/
#define SENSOR_TYPE_MAGNETIC_FIELD                  (1)
/**Pressure sensor (also known as barometer). */
#define SENSOR_TYPE_PRESSURE                        (2)
/**Gyroscope sensor.*/
#define SENSOR_TYPE_GYROSCOPE                       (3)
/**Proximity sensor.*/
#define SENSOR_TYPE_PROXIMITY                       (4)
/**Biosensor EKG data.*/
#define SENSOR_TYPE_BIOSENSOR_EKG                   (5)
/**Biosensor EEG data.*/
#define SENSOR_TYPE_BIOSENSOR_EEG                   (6)
/**Biosensor EMG data.*/
#define SENSOR_TYPE_BIOSENSOR_EMG                   (7)
/**Biosensor GSR data.*/
#define SENSOR_TYPE_BIOSENSOR_GSR                   (8)
/**Biosensor PPG channel 1 for optical heart rate monitoring.*/
#define SENSOR_TYPE_BIOSENSOR_PPG1                  (9)
/**Biosensor PPG channel 2. To measure the pulse oximetry, two independent channels of PPG with different LED wavelengths are required.*/
#define SENSOR_TYPE_BIOSENSOR_PPG2                  (10)
/**Biosensor BISI data.*/
#define SENSOR_TYPE_BIOSENSOR_BISI                  (11)
/**Biosensor PPG 512Hz data.*/
#define SENSOR_TYPE_BIOSENSOR_PPG1_512HZ            (12)
/**Total number of the listed physical sensors.*/
#define SENSOR_PHYSICAL_TYPE_ALL                    (13)
/**Starting number of the virtual sensor type for fusion algorithm.*/
#define SENSOR_VIRTUAL_TYPE_START                   (13)
/**Step counter sensor.*/
#define SENSOR_TYPE_PEDOMETER                       (SENSOR_VIRTUAL_TYPE_START)
/**Activity recognition sensor.*/
#define SENSOR_TYPE_ACTIVITY                        (SENSOR_VIRTUAL_TYPE_START+1)
/**A sensor to detect a gesture of checking the time or a glance at the screen.*/
#define SENSOR_TYPE_GESTURE_CHECK_TIME              (SENSOR_VIRTUAL_TYPE_START+2)
/**A sensor to detect a gesture of tapping the screen.*/
#define SENSOR_TYPE_GESTURE_TAP                     (SENSOR_VIRTUAL_TYPE_START+3)
/**A sensor to detect the gesture of shaking the device up and down or left and right.*/
#define SENSOR_TYPE_GESTURE_SHAKE                   (SENSOR_VIRTUAL_TYPE_START+4)
/**A sensor to detect the gesture of twisting the device attached to a wrist.*/
#define SENSOR_TYPE_GESTURE_TWIST                   (SENSOR_VIRTUAL_TYPE_START+5)
/**Fall detection sensor.*/
#define SENSOR_TYPE_FALL                            (SENSOR_VIRTUAL_TYPE_START+6)
/**Sleep tracker sensor. */
#define SENSOR_TYPE_SLEEP                           (SENSOR_VIRTUAL_TYPE_START+7)
/**PDR sensor. */
#define SENSOR_TYPE_PDR                             (SENSOR_VIRTUAL_TYPE_START+8)
/**A sensor to monitor heart rate with PPG. */
#define SENSOR_TYPE_HEART_RATE_MONITOR              (SENSOR_VIRTUAL_TYPE_START+9)
/**A sensor to monitor one's heart rate variability. */
#define SENSOR_TYPE_HEART_RATE_VARIABILITY_MONITOR  (SENSOR_VIRTUAL_TYPE_START+10)
/**A sensor to monitor one's blood pressure. */
#define SENSOR_TYPE_BLOOD_PRESSURE_MONITOR          (SENSOR_VIRTUAL_TYPE_START+11)
/**Sleep staging sensor.*/
#define SENSOR_TYPE_SLEEP_STAGING                   (SENSOR_VIRTUAL_TYPE_START+12)
/**Motion detector.*/
#define SENSOR_TYPE_MOTION_DETECT                   (SENSOR_VIRTUAL_TYPE_START+13)
/**A sensor to monitor heart rate with ECG. */
#define SENSOR_TYPE_ELECTRIC_HEART_RATE_MONITOR     (SENSOR_VIRTUAL_TYPE_START+14)
/**Total number of the listed physical sensors and fusion algorithms.*/
#define SENSOR_TYPE_ALL                             (SENSOR_VIRTUAL_TYPE_START+15)
/**
* @addtogroup sensor_fusion_algorithm_interface Sensor fusion algorithm interface
* @{
* The sensor fusion algorithm interface enables the plug-in sensors, such as pedometer and
* activity recognition and the application of the fusion algorithms. The fusion algorithm is available in a library format.
* The APIs are designed to feed the physical sensor data into fusion algorithms
* and obtain the results based on the various application needs.
* @section sensor_fusion_algorithm_interface_usage_chapter How to use sensor fusion algorithm interface
*
* Below is an example of the pedometer fusion algorithm (requires accelerometer of 50Hz sampling rate) registration.
* The steps are shown below:
*  - Step 1. Prepare the #sensor_descriptor_t for the pedometer.
*  - Step 2. Set the required input sensor.
*  - Step 3. Register the pedometer fusion algorithm using #sensor_fusion_algorithm_register_type().
*  - Step 4. Register the data buffer using #sensor_fusion_algorithm_register_data_buffer(). Please refer to sensor_manager_t in [sdk_root]/middleware/MTK/sensor_subsys/src/sensor_manager.c for data buffer allocation.
*  - Sample code:
*    @code
* static sensor_input_list_t input_comp_acc;
* static int32_t get_pedometer_result(sensor_data_t *const output)
* {
*     // Return the pedometer result.
*     return 1;
* }
*
* static int32_t pedometer_process_data(const sensor_data_t *input_list, void *reserved)
* {
*     int32_t count = input_list->data_exist_count;
*     sensor_data_unit_t *data_start = input_list->data;
*
*     while (count != 0) {
*         // Process the input data.
*         sensor_fusion_algorithm_notify(SENSOR_TYPE_PEDOMETER); // Call for data update notification.
*         data_start++;
*         count--;
*     }
*     return 1;
* }
*
* static int32_t pedometer_operate(int32_t command, void *buffer_out, int32_t size_out, \
*                                  void *buffer_in, int32_t size_in)
* {
*     // Process the customized parameter.
*     return 0;
* }
*
* const sensor_descriptor_t pedometer_desp = {
*     SENSOR_TYPE_PEDOMETER,
*     1, // version.
*     SENSOR_REPORT_MODE_ON_CHANGE, // sensor_report_mode_t.
*     &input_comp_acc, // Required input sensor list.
*     pedometer_operate,
*     get_pedometer_result,
*     pedometer_process_data,
*     0 // Not used
* };
*
* int pedometer_register()
* {
*     int ret;
*
*     input_comp_acc.input_type = SENSOR_TYPE_ACCELEROMETER;
*     // Set accelerometer sensor sampling at 20ms interval (50Hz) with a batch timeout of 200ms.
*     input_comp_acc.sampling_delay = 20; // 20ms interval, 50Hz sampling rate.
*     input_comp_acc.timeout = 200;
*     input_comp_acc.next_input = NULL;
*
*     ret = sensor_fusion_algorithm_register_type(&pedometer_desp);
*     if (ret < 0) {
*         // Registration type error handling.
*     }
*     ret = sensor_fusion_algorithm_register_data_buffer(SENSOR_TYPE_PEDOMETER, 1);
*     if (ret < 0) {
*         // Registration data buffer error handling.
*     }
*     return ret;
* }

*    @endcode

*/

/** @defgroup sensor_fusion_algorithm_enum Enum
  * @{
  */

/** @brief The types of activity recognition*/
typedef enum {
    SENSOR_ACTIVITY_STILL, /**< Stationary.*/
    SENSOR_ACTIVITY_STANDING, /**< Standing.*/
    SENSOR_ACTIVITY_SITTING, /**< Sitting.*/
    SENSOR_ACTIVITY_LAYING, /**< Laying.*/
    SENSOR_ACTIVITY_ON_FOOT, /**< In motion.*/
    SENSOR_ACTIVITY_WALKING, /**< Walking.*/
    SENSOR_ACTIVITY_RUNNING, /**< Running.*/
    SENSOR_ACTIVITY_CLIMBING, /**< Climbing.*/
    SENSOR_ACTIVITY_ON_BICYCLE, /**< On bicycle.*/
    SENSOR_ACTIVITY_IN_VEHICLE, /**< In vehicle.*/
    SENSOR_ACTIVITY_TILTING, /**< Tilting, the vertical direction of the device changes by at least 35 degrees in the predefined period of, e.g., 2 seconds.*/
    SENSOR_ACTIVITY_UNKNOWN, /**< Unknown activity type.*/
    SENSOR_ACTIVITY_MAX /**< The maximum number of activity types.*/
} sensor_activity_type_t;

/** @brief The types of gesture detection defined in the fusion algorithm design.*/
typedef enum {
    SENSOR_GESTURE_NONE,/**< No gesture is detected.*/
    SENSOR_GESTURE_SHAKE, /**< Gesture of shaking the device up and down or left and right.*/
    SENSOR_GESTURE_TAP,/**< Gesture of tapping the screen.*/
    SENSOR_GESTURE_TWIST,/**< Gesture of twisting the device attached to the wrist.*/
    SENSOR_GESTURE_FLIP,/**< Gesture of flipping the device.*/
    SENSOR_GESTURE_SNAPSHOT,/**< Gesture of positioning the device to take a snapshot.*/
    SENSOR_GESTURE_PICKUP,/**< Gesture of picking up the device.*/
    SENSOR_GESTURE_CHECK_TIME /**< Gesture of checking the time or a glance at the screen.*/
} sensor_gesture_type_t;

/** @brief The types of fall detection.*/
typedef enum {
    SENSOR_FALL_NONE, /**< No fall is detected.*/
    SENSOR_FALL_FALL,/**< Falling.*/
    SENSOR_FALL_FLOP,/**< A fall type that can be customized for a special fusion algorithm. For example, it could detect if a basketball player flopped during a game.*/
    SENSOR_FALL_MAX/**< The maximum number of fall types.*/
} sensor_fall_type_t;

/** @brief Methods to generate events with sensors.*/
typedef enum {
    SENSOR_REPORT_MODE_ON_CHANGE = 1, /**< Sensor events are generated only if the measured values are changed.*/
    SENSOR_REPORT_MODE_ONE_SHOT = 2, /**< Sensor events are generated upon the detection.*/
    SENSOR_REPORT_MODE_CONTINUOUS = 4 /**< Sensor events are generated at a constant rate.*/
} sensor_report_mode_t;

/** @brief Measurement status of the sensor.*/
typedef enum {
    SENSOR_STATUS_NO_CONTACT = -1, /**< This sensor cannot be trusted because the sensor had no contact with what it was measuring.*/
    SENSOR_STATUS_UNRELIABLE = 0, /**< The values returned by this sensor cannot be trusted.*/
    SENSOR_STATUS_ACCURACY_LOW = 1, /**< This sensor is reporting data with low accuracy.*/
    SENSOR_STATUS_ACCURACY_MEDIUM = 2, /**< This sensor is reporting data with an average level of accuracy.*/
    SENSOR_STATUS_ACCURACY_HIGH = 3, /**< This sensor is reporting data with maximum accuracy.*/
} sensor_status_t;

/**
  * @}
  */

/** @defgroup sensor_fusion_algorithm_struct Struct
  * @{
  */

/** @brief Sensor event data over 3-axis.*/
typedef struct {
    union {
        int32_t v[3]; /**<Values.*/
        struct {
            int32_t x; /**<Value of the X-axis.*/
            int32_t y; /**<Value of the Y-axis.*/
            int32_t z; /**<Value of the Z-axis.*/
        };
        struct {
            int32_t azimuth; /**<Rotation around the Z-axis.*/
            int32_t pitch; /**<Rotation around the X-axis.*/
            int32_t roll; /**<Rotation around the Y-axis.*/
        };
    };
    uint32_t status; /**<Quality of the measurement.*/
    int32_t reserved[3]; /**<Reserved.*/
} sensor_vec_t;

/** @brief Uncalibrated sensor event data over 3-axis.*/
typedef struct {
    union {
        int32_t uncali[3]; /**<Values.*/
        struct {
            int32_t x_uncali; /**<Value of the X-axis.*/
            int32_t y_uncali; /**<Value of the Y-axis.*/
            int32_t z_uncali; /**<Value of the Z-axis.*/
        };
    };
    union {
        int32_t bias[3]; /**<Biases.*/
        struct {
            int32_t x_bias; /**<Bias of the X-axis.*/
            int32_t y_bias; /**<Bias of the Y-axis.*/
            int32_t z_bias; /**<Bias of the Z-axis.*/
        };
    };
} sensor_uncalibrated_event_t;

/** @brief Heart rate sensor event data.*/
typedef struct {
    int32_t bpm; /**<Beats per minute.*/
    int32_t status; /**<Quality of the measurement, subject to fusion algorithm design.*/
} sensor_heart_rate_event_t;

/** @brief Heart rate variability sensor event data.*/
typedef struct {
  int32_t SDNN; /**<Standard deviation of ECG normal to normal beat interval. The output value needs to be divided by 1000*/
  int32_t LF; /**<ECG autonomic nervous system low frequency band. The output value needs to be divided by 1000.*/
  int32_t HF; /**<ECG autonomic nervous system high frequency band. The output value needs to be divided by 1000.*/
  int32_t LF_HF; /**<The balance of autonomic nervous system, represented by low frequency band to high frequency band ratio. The output value needs to be divided by 1000.*/
  int32_t bpm; /**<Heart rate measurement. Beats per minute.*/
  int32_t status; /**<Quality of the measurement, subject to fusion algorithm design.*/
  int32_t *beatPos; /**<The count of each heartbeat occurrence depending on the EKG sampling rate.*/
  int32_t numBeatPos;  /**<The total number of the counts.*/
  int32_t feature_type; /**<An indicator to heart rate variability monitor output parameters. If feature_type is 0, beatPos outputs the count of each heartbeat occurrence.*/
} sensor_heart_rate_variability_event_t;

/** @brief Blood pressure sensor event data.*/
typedef struct {
    int32_t sbp; /**<Systolic blood pressure in mmHg.*/
    int32_t dbp; /**<Diastolic blood pressure in mmHg.*/
    int32_t bpm; /**<Heart rate measurement. Beats per minute.*/
    int32_t status; /**<Quality of the measurement, subject to fusion algorithm design.*/
    int32_t *pwtt; /**<The sequence of each Pulse Wave Transit Time (PWTT) interval between ECG and PPG in sampling rate unit.*/
    int32_t numPwtt;  /**<The total number of PWTT contained in the sequence.*/
    int32_t feature_type; /**<An indicator to blood pressure monitor sensor output parameters. If feature_type is 0, the pwtt sequence outputs PWTT values. If the feature_type is 3, the pwtt sequence outputs data for blood pressure calibration.*/
} sensor_blood_pressure_event_t;

/** @brief Biosensor event data.*/
typedef struct {
    int32_t type; /**<Biosensor type.*/
    uint32_t data; /**<Biosensor data.*/
} sensor_bio_t;

/** @brief Activity recognition sensor event data.*/
typedef struct {
    int32_t probability[SENSOR_ACTIVITY_MAX]; /**<Probability of an individual activity in #sensor_activity_type_t, the range is from 0 to 100.*/
} sensor_activity_t;

/** @brief Gesture detection sensor event data.*/
typedef struct {
    int32_t probability; /**<Probability of gesture detection, the range is from 0 to 100.*/
} sensor_gesture_t;

/** @brief Pedometer sensor event data.*/
typedef struct {
    uint32_t accumulated_step_count; /**<Accumulated step count.*/
    uint32_t accumulated_step_length; /**<Accumulated step length.*/
    uint32_t step_frequency; /**<Step frequency.*/
    uint32_t step_length; /**<Step length.*/
    uint8_t step_type;/**<Step type: 1 for walk, 2 for run, and 3 for jogging.*/
} sensor_pedometer_event_t;

/** @brief Pressure sensor event data.*/
typedef struct {
    int32_t pressure; /**<Pressure measurement.*/
    int32_t temperature; /**<Temperature measurement.*/
    uint32_t status; /**<Status of the measurement.*/
} sensor_pressure_vec_t;

/** @brief Relative humidity sensor event data.*/
typedef struct {
    int32_t relative_humidity; /**<Relative humidity measurement.*/
    int32_t temperature; /**<Temperature measurement.*/
    uint32_t status; /**<Status of the measurement.*/
} sensor_relative_humidity_vec_t;

/** @brief Sleep monitor sensor event data.*/
typedef struct {
    int32_t state; /**<The detected state of a person's sleep, subject to fusion algorithm design.*/
    int32_t *feature; /**<Intermediate features for sleep monitor.*/
    int32_t feature_num;  /**<The number of intermediate features.*/
    int32_t feature_type; /**<The intermediate feature type.*/
} sensor_sleepmonitor_event_t;

/** @brief Fall detection sensor event data.*/
typedef struct {
    int32_t probability[SENSOR_FALL_MAX];  /**<The probability of fall detection, 0-100.*/
} sensor_fall_t;

/** @brief Timstamped sensor event data.*/
typedef struct {
    uint32_t sensor_type; /**<Sensor type.*/
    uint32_t time_stamp; /**<Timestamp.*/
    union {
        union {
            sensor_vec_t accelerometer_t; /**<Accelerometer sensor event data.*/
            sensor_vec_t gyroscope_t; /**<Gyroscope sensor event data.*/
            sensor_vec_t magnetic_t; /**<Magnetic field sensor event data.*/
            sensor_vec_t orientation_t; /**<Orientation sensor event data.*/
            sensor_vec_t pdr_event; /**<PDR sensor event data.*/

            int32_t light; /**<Light sensor event data.*/
            int32_t distance; /**<Proximity sensor event data.*/
            sensor_pressure_vec_t pressure_t; /**<Pressure sensor event data.*/
            sensor_relative_humidity_vec_t relative_humidity_t; /**<Relative humidity sensor event data.*/

            sensor_uncalibrated_event_t uncalibrated_acc_t; /**<Uncalibrated accelerometer sensor event data.*/
            sensor_uncalibrated_event_t uncalibrated_mag_t; /**<Uncalibrated magnetic field sensor event data.*/
            sensor_uncalibrated_event_t uncalibrated_gyro_t; /**<Uncalibrated gyroscope sensor event data.*/

            sensor_pedometer_event_t pedometer_t; /**<Pedometer sensor event data.*/
            sensor_heart_rate_event_t heart_rate_t; /**<Heart rate sensor event data.*/
            sensor_heart_rate_variability_event_t heart_rate_variability_t; /**<Heart rate sensor event data.*/
            sensor_blood_pressure_event_t blood_pressure_t; /**< Blood pressure sensor event data.*/

            sensor_sleepmonitor_event_t sleep_data_t; /**<Sleep monitor sensor event data.*/
            sensor_activity_t activity_data_t; /**<Activity recognition sensor event data.*/
            sensor_gesture_t gesture_data_t; /**<Gesture detection sensor event data.*/
            sensor_fall_t fall_data_t; /**<Fall detection sensor event data.*/
            sensor_bio_t bio_data; /**<Biosensor event data.*/
            int32_t value[16]; /**<The values of augmented sensor data.*/
        };
        union {
            uint64_t data[8]; /**<The values of augmented sensor data in 64-bit units.*/
            uint64_t step_counter; /**<The number of steps taken.*/
        };
    };
} sensor_data_unit_t;

/** @brief Link-listed timestamped sensor event data.*/
typedef struct sensor_data {
    sensor_data_unit_t *data; /**<The link-listed timestamped sensor event data.*/
    uint32_t fifo_max_size; /**<Not used.*/
    uint32_t data_exist_count; /**<The count of #sensor_data_unit_t in data.*/
} sensor_data_t;

/** @brief The required input physical sensor list to register the fusion algorithms.*/
typedef struct sensor_input_list {
    uint32_t input_type; /**< Sensor type.*/
    uint32_t sampling_delay; /**< The sampling interval of the input data needs to be faster than this value, measured in milliseconds.*/
    struct sensor_input_list *next_input; /**< The next data in the linked list.*/
    uint32_t timeout; /**< Sets the batch timeout in ms.*/
} sensor_input_list_t;

/**
  * @}
  */

/** @defgroup sensor_fusion_algorithm_typedef Typedef
  * @{
  */

/** @brief Sets customized parameters in the sensor_descriptor structure.
 *   @param [in] command is the operation command.
 *   @param [out] buffer_out is a pointer to the output data.
 *   @param [in] size_out is the output buffer size.
 *   @param [in] buffer_in is a pointer to the input data.
 *   @param [in] size_in is the input buffer size.
 *   @return >=0, if the operation completed successfully.*/
typedef int32_t (*sensor_descriptor_operate_callback_t)(int32_t command, void *buffer_out, int32_t size_out, void *buffer_in, int32_t size_in);

/** @brief Gets the fusion algorithm results of the sensor event.
 *   @param [out] output is the result of sensor event.
 *   @return >=0, if the operation completed successfully.*/
typedef int32_t (*sensor_descriptor_get_result_callback_t)(sensor_data_t *const output);

/** @brief Feeds input sensor data and trigger the fusion algorithm to execute.
*   @param [in] input_list is the input of sensor event.
*   @param [in] reserved is not used.
*   @return >=0, if the operation completed successfully.*/
typedef int32_t (*sensor_descriptor_process_data_callback_t)(const sensor_data_t *input_list, void *reserved);

/**
  * @}
  */

/** @addtogroup sensor_fusion_algorithm_struct
  * @{
  */

/** @brief The description for a virtual sensor (fusion algorithm) registration.*/
typedef struct sensor_descriptor {
    uint32_t sensor_type; /**< Sensor type.*/
    int32_t version; /**< Fusion algorithm API version.*/
    int32_t report_mode; /**< Report mode. Please refer to #sensor_report_mode_t.*/
    sensor_input_list_t *input_list; /**<Required input sensor list. Please refer to #sensor_input_list_t.*/
    sensor_descriptor_operate_callback_t operate; /**<To set customized parameter to fusion algorithms, such as step length for pedometer.*/
    sensor_descriptor_get_result_callback_t get_result; /**<Call this function to get the fused results.*/
    sensor_descriptor_process_data_callback_t process_data; /**<Call this function if one of the input sensors of the fusion algorithm has updated data to be used for generating a fused result.*/
    int32_t accumulate; /**< Not used.*/
} sensor_descriptor_t;

/**
  * @}
  */

/** @brief Registers fusion algorithm of the specified sensor_descriptor.
*   @param [in] desp is the specified sensor_descriptor.
*   @return >=0, if the operation completed successfully.*/
int32_t sensor_fusion_algorithm_register_type(const sensor_descriptor_t *desp);

/** @brief Registers the required data buffer of fusion algorithm of the specified sensor type.
*   @param [in] sensor_type is the specified sensor type.
*   @param [in] exist_data_count is the required data buffer count.
*   @return >=0, if the operation completed successfully.*/
int32_t sensor_fusion_algorithm_register_data_buffer(uint32_t sensor_type, uint32_t exist_data_count);

/** @brief For fusion algorithm to notify sensor manager of sensor event.
*   @param [in] sensor_type is the specified sensor type.
*   @return >=0, if the operation completed successfully.*/
int32_t sensor_fusion_algorithm_notify(uint32_t sensor_type);

/**
* @}
*/

/**
* @addtogroup sensor_driver_interface Sensor driver interface
* @{
* The sensor driver interface facilitates the physical sensor driver porting.
* Developers can easily enable/operate different physical sensors, such as accelerometer, gyroscope, and more
* using the interface APIs.
* An example to port the accelerometer driver is described below.
*  - Step 1. Configure GPIO pins for sensor hardware connection. For more information, please refer to [sdk_root]/project/mt2523_sdk/apps/sensor_subsys_accelerometer/inc/ept_gpio_drv.h and bsp_sensor_peripheral_init
* in [sdk_root]/driver/board/mt25x3_hdk/mems_init/mems_init.c.
*  - Step 2. Prepare the #sensor_driver_object_t for this sensor driver.
*  - Step 3. Call #sensor_driver_attach() to register this sensor driver in the initialization function (bma255_sensor_subsys_init(), please refer to [sdk_root]/driver/board/component/mems/src/bma255_sensor_adaptor.c).
*  - Step 4. Call the initialization function (bma255_sensor_subsys_init()) in bsp_sensor_peripheral_init().
*  - Sample code:
*    @code
* int32_t bma_acc_operate(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actual_out,
*                     void *buff_in, int32_t size_in)
* {
*     int err = 0;
*     int32_t value = 0;
*
*     switch (command) {
*         case SENSOR_DELAY:
*            // Set the delay.
*             break;
*
*         case SENSOR_ENABLE:
*             // Enable/Disable this physical sensor.
*             break;
*
*         case SENSOR_GET_DATA:
*             // Feed the sensor data.
*             break;
*
*         case SENSOR_CUST:
*             // Process the customization settings.
*            break;
*         default:
*             break;
*     }
*     return err;
* }
*
* BMA2x2_RETURN_FUNCTION_TYPE bma255_sensor_subsys_init()
* {
*     sensor_driver_object_t obj_acc;
*     BMA2x2_RETURN_FUNCTION_TYPE err = ERROR;
*
*
*     err = bma255_init();
*     obj_acc.self = (void *)&obj_acc;
*     obj_acc.polling = 0; // Interrupt driven
*
*     obj_acc.sensor_operate = bma_acc_operate;
*     sensor_driver_attach(SENSOR_TYPE_ACCELEROMETER, &obj_acc);
*
*
*     return err;
* }
*
* void bsp_sensor_peripheral_init()
* {
* //...
* // Accelerometer driver initialization
*     bma255_sensor_subsys_init();
* //...
* }
*    @endcode
*/

/**Default delay between two consecutive sensor events in ms.*/
#define SENSOR_DEFAULT_DELAY    (200)

/** Command of setting a delay to a physical sensor driver. */
#define SENSOR_DELAY	    0x01
/** Command of enabling/disabling a physical sensor driver. */
#define	SENSOR_ENABLE	    0x02
/** Command of getting data from a physical sensor driver. */
#define	SENSOR_GET_DATA	    0x04
/** Command of setting the customization to a physical sensor driver. */
#define SENSOR_CUST     0x08
/** Command of setting the batch timeout to a physical sensor driver. */
#define SENSOR_BATCH     0x10

/** @defgroup sensor_driver_struct Struct
  * @{
  */

/**
    * @}
*/

/** @defgroup sensor_driver_typedef Typedef
  * @{
  */

/** @brief Operates on the physical sensor.
 *   @param [in] self is the handle of the physical sensor driver.
 *   @param [in] command is the operation command.
 *   @param [out] buffer_out is a pointer to the output data.
 *   @param [in] size_out is the size of the buffer_out.
 *   @param [out] actual_out is the actual size of the output.
 *   @param [in] buffer_in is a pointer to the input data.
 *   @param [in] size_in is the size of the buffer_in.
 *   @return >=0, if the operation completed successfully.*/
typedef int32_t (*sensor_driver_object_operate_callback_t)(void *self, uint32_t command, void *buff_out, int32_t size_out, int32_t *actual_out,
        void *buff_in, int32_t size_in);
/**
* @}
*/

/** @addtogroup sensor_driver_struct
  * @{
  */

/** @brief The sensor driver object of a physical sensor.*/
typedef struct sensor_driver_object {
    void *self; /**< Handle.*/
    int32_t polling; /**< Data acquisition via polling mode or interrupt driven.*/
    sensor_driver_object_operate_callback_t sensor_operate; /**< Operates on this physical sensor.*/
} sensor_driver_object_t;

/**
* @}
*/

/** @brief Registers the physical sensor driver.
*   @param [in] sensor_type is the specified sensor type.
*   @param [in] obj is of type sensor_driver_object_t for the registration.
*   @return >=0, if the operation completed successfully.*/
int32_t sensor_driver_attach(uint32_t sensor_type, sensor_driver_object_t *obj);

/**
* @}
*/

/**
* @addtogroup sensor_app_interface Sensor application subscription interface
* @{
* The sensor application subscription interface targets
* the events of a specific sensor, such as pedometer, to provide rich information on user's daily activities.
* An example implementation of the subscription to accelerometer data can be found in [sdk_root]/project/mt2523_sdk/apps/sensor_subsys_accelerometer.
* The steps are described below.
*  - 1. Call #sensor_manager_init() to initialize the sensor manager.
*  - 2. Prepare the sensor data subscription structure. Refer to #sensor_subscriber_t.
*  - 3. Call #sensor_subscribe_sensor() to subscribe the specified sensor in application.
*  - Sample code:
*    @code
* int32_t acc_sensor_send_digest_callback(sensor_data_t *const output);
*
* sensor_subscriber_t acc_subscriber = {
*     "ap0", 0, SENSOR_TYPE_ACCELEROMETER, 10, 100, acc_sensor_send_digest_callback
* };
* int32_t acc_sensor_send_digest_callback(sensor_data_t *const output)
* {
*     switch (output->data[0].sensor_type) {
*
*         case SENSOR_TYPE_ACCELEROMETER:
*             printf("acc sensor type = %lu , value = ( %ld , %ld , %ld ) (%ld) , timestamp = %lu \r\n",
*                    output->data[0].sensor_type,
*                    output->data[0].accelerometer_t.x,
*                    output->data[0].accelerometer_t.y,
*                    output->data[0].accelerometer_t.z,
*                    output->data[0].accelerometer_t.status,
*                    output->data[0].time_stamp
*                   );
*             break;
*
*         default:
*             printf("type = %lu,  timestamp = %lu \r\n",
*                    output->data[0].sensor_type,
*                    output->data[0].time_stamp);
*             break;
*     }
*     return 0;
* }
*
* int main(void)
* {
*     sensor_manager_init();
*     int32_t subscription;
*     subscription = sensor_subscribe_sensor(&acc_subscriber);
*     if (subscription < 0) {
*         printf("acc subscription fail\r\b");
*     }
*    @endcode
*/

/** @defgroup sensor_app_struct Struct
  * @{
  */

/**
* @}
*/

/** @defgroup sensor_app_typedef Typedef
  * @{
  */

/** @brief Sends sensor data to the subscriber.
*   @param [in] output is the sensor event data to callback.
*   @return >=0, if the operation completed successfully.*/
typedef int32_t (*sensor_subscriber_send_digest_callback_t)(sensor_data_t *const output);

/**
    * @}
*/

/**The maximum sensor subscriber name length.*/
#define SENSOR_MAX_SUBSCRIBER_NAME_LEN			( 8 )

/** @addtogroup sensor_app_struct
  * @{
  */

/** @brief The subscriber information for the sensor event of the specified sensor.*/
typedef struct sensor_subscriber {
    char name[SENSOR_MAX_SUBSCRIBER_NAME_LEN]; /**< Subscriber's name.*/
    int32_t handle; /**< Handle of this subscription, filled in #sensor_subscribe_sensor.*/
    uint32_t type; /**< The sensor type for the subscription.*/
    uint32_t delay; /**< The sampling interval for the sensor event.*/
    sensor_subscriber_send_digest_callback_t send_digest; /**< To notify a sensor event to this subscriber.*/
    uint32_t timeout; /**< The batch timeout in ms.*/
} sensor_subscriber_t;

/**
* @}
*/

/** @brief This function initializes the data structures in sensor manager.
*   @return >=0, if the operation completed successfully.*/
int32_t sensor_manager_init(void);

/** @brief This function gets the latest data of the specified sensor type.
*   @param [in] type is the sensor type for the sensor event data acquisition.
*   @param [out] output is the sensor event data.
*   @return >=0, if the operation completed successfully.*/
int32_t sensor_get_latest_sensor_data(uint32_t type, sensor_data_t *const output);

/** @brief This function subscribes the sensor event for a specified sensor type.
*   @param [in, out] subscriber is the data structure for the subscription.
*   @return >=0, if the operation completed successfully.*/
int32_t sensor_subscribe_sensor(sensor_subscriber_t *subscriber);

/** @brief This function unsubscribes the sensor event for a specified sensor type.
*   @param [in] handle is the subscriber's handle to be unsubscribed.
*   @return >=0, if the operation completed successfully.*/
int32_t sensor_unsubscribe_sensor(int32_t handle);

/** @brief This function sends the sensor event for a specified sensor type from application layer.
*   @param [in] sensor_type is the sensor type for the sensor event.
*   @param [in] in_data_buf is a pointer to the input data.
*   @param [in] in_size is the input buffer size.
*   @return >=0, if the operation completed successfully.*/
int sensor_manager_set_cust_from_app(uint32_t sensor_type, void *in_data_buf, int32_t in_size);

/**
* @}
*/

#ifdef __cplusplus
}
#endif

/**
* @}
*/

#endif /*__SENSOR_ALG_INTERFACE_H__*/

