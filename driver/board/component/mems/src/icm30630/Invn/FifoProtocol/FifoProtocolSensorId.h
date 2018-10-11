/*
* ________________________________________________________________________________________________________
* Copyright (c) 2014-2015 InvenSense Inc. Portions Copyright (c) 2014-2015 Movea. All rights reserved.
*
* This software, related documentation and any modifications thereto (collectively "Software") is subject
* to InvenSense and its licensors' intellectual property rights under U.S. and international copyright and
* other intellectual property rights laws.
*
* InvenSense and its licensors retain all intellectual property and proprietary rights in and to the Software
* and any use, reproduction, disclosure or distribution of the Software without an express license
* agreement from InvenSense is strictly prohibited.
*
* EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE SOFTWARE IS
* PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
* EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
* INVENSENSE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, OR ANY
* DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
* NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
* OF THE SOFTWARE.
* ________________________________________________________________________________________________________
*/

/** @defgroup 	FifoProtocolSensorId	 FifoProtocolSensorId
 	@brief 		Sensor ID definition expected to be used with FIFO Protocol
    @ingroup 	FifoProtocolAdapter
    @{
*/

#ifndef _INV_FIFO_PROTOCOL_SENSOR_ID_H_
#define _INV_FIFO_PROTOCOL_SENSOR_ID_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Android sensor range
 *
 * These numbers should follow the Android specification for Sensor ID when possible
 */

#define FIFOPROTOCOL_SENSOR_ID_META_DATA                    0   /**< Meta-data */
#define FIFOPROTOCOL_SENSOR_ID_ACCELEROMETER                1   /**< Accelerometer */
#define FIFOPROTOCOL_SENSOR_ID_MAGNETOMETER                 2   /**< Magnetic field */
#define FIFOPROTOCOL_SENSOR_ID_ORIENTATION                  3   /**< Deprecated orientation */
#define FIFOPROTOCOL_SENSOR_ID_GYROSCOPE                    4   /**< Gyroscope */
#define FIFOPROTOCOL_SENSOR_ID_LIGHT                        5   /**< Ambient light sensor */
#define FIFOPROTOCOL_SENSOR_ID_PRESSURE                     6   /**< Barometer */
#define FIFOPROTOCOL_SENSOR_ID_TEMPERATURE                  7   /**< Temperature */
#define FIFOPROTOCOL_SENSOR_ID_PROXIMITY                    8   /**< Proximity */
#define FIFOPROTOCOL_SENSOR_ID_GRAVITY                      9   /**< Gravity */
#define FIFOPROTOCOL_SENSOR_ID_LINEAR_ACCELERATION          10  /**< Linear acceleration */
#define FIFOPROTOCOL_SENSOR_ID_ROTATION_VECTOR              11  /**< Rotation vector */
#define FIFOPROTOCOL_SENSOR_ID_HUMIDITY                     12  /**< Relative humidity */
#define FIFOPROTOCOL_SENSOR_ID_AMBIENT_TEMPERATURE          13  /**< Ambient temperature */
#define FIFOPROTOCOL_SENSOR_ID_UNCAL_MAGNETOMETER           14  /**< Uncalibrated magnetic field */
#define FIFOPROTOCOL_SENSOR_ID_GAME_ROTATION_VECTOR         15  /**< Game rotation vector */
#define FIFOPROTOCOL_SENSOR_ID_UNCAL_GYROSCOPE              16  /**< Uncalibrated gyroscope */
#define FIFOPROTOCOL_SENSOR_ID_SMD                          17  /**< Significant motion detection */
#define FIFOPROTOCOL_SENSOR_ID_STEP_DETECTOR                18  /**< Step detector */
#define FIFOPROTOCOL_SENSOR_ID_STEP_COUNTER                 19  /**< Step counter */
#define FIFOPROTOCOL_SENSOR_ID_GEOMAG_ROTATION_VECTOR       20  /**< Geomagnetic rotation vector */
#define FIFOPROTOCOL_SENSOR_ID_HEART_RATE                   21  /**< Heart rate */
#define FIFOPROTOCOL_SENSOR_ID_TILT_DETECTOR                22  /**< Tilt detector */
#define FIFOPROTOCOL_SENSOR_ID_WAKE_GESTURE                 23  /**< Wake-up gesture  */
#define FIFOPROTOCOL_SENSOR_ID_GLANCE_GESTURE               24  /**< Glance gesture  */
#define FIFOPROTOCOL_SENSOR_ID_PICK_UP_GESTURE              25  /**< Pick-up gesture */

/* ... empty slots for new Android sensors... */

#define FIFOPROTOCOL_SENSOR_ID_ACTIVITY_CLASSIFIER          31  /**< Android activity classifier */

#define FIFOPROTOCOL_SENSOR_ID_MAX                          32  /**< Max sensor id for standard sensor */

/*
 * Aliases for backward compatibility
 */
#define FIFOPROTOCOL_SENSOR_ID_GYROMETER                    FIFOPROTOCOL_SENSOR_ID_GYROSCOPE
#define FIFOPROTOCOL_SENSOR_ID_UNCAL_GYROMETER              FIFOPROTOCOL_SENSOR_ID_UNCAL_GYROSCOPE

/*
 * Extended sensor range
 *
 * Reserved sensor id for InvenSense extension (ie: OEMs Private Sensors)
 */

#define FIFOPROTOCOL_SENSOR_ID_EXT_BASE                     FIFOPROTOCOL_SENSOR_ID_MAX

#define FIFOPROTOCOL_SENSOR_ID_EXT_0                        (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+0)
#define FIFOPROTOCOL_SENSOR_ID_EXT_1                        (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+1)
#define FIFOPROTOCOL_SENSOR_ID_EXT_2                        (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+2)
#define FIFOPROTOCOL_SENSOR_ID_EXT_3                        (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+3)
#define FIFOPROTOCOL_SENSOR_ID_EXT_4                        (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+4)
#define FIFOPROTOCOL_SENSOR_ID_EXT_5                        (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+5)
#define FIFOPROTOCOL_SENSOR_ID_EXT_6                        (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+6)
#define FIFOPROTOCOL_SENSOR_ID_EXT_7                        (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+7)

#define FIFOPROTOCOL_SENSOR_ID_EXT_8                        (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+8)
#define FIFOPROTOCOL_SENSOR_ID_EXT_9                        (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+9)
#define FIFOPROTOCOL_SENSOR_ID_EXT_10                       (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+10)
#define FIFOPROTOCOL_SENSOR_ID_EXT_11                       (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+11)
#define FIFOPROTOCOL_SENSOR_ID_EXT_12                       (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+12)
#define FIFOPROTOCOL_SENSOR_ID_EXT_13                       (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+13)
#define FIFOPROTOCOL_SENSOR_ID_EXT_14                       (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+14)
#define FIFOPROTOCOL_SENSOR_ID_EXT_15                       (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+15)

#define FIFOPROTOCOL_SENSOR_ID_EXT_16                       (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+16)
#define FIFOPROTOCOL_SENSOR_ID_EXT_17                       (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+17)
#define FIFOPROTOCOL_SENSOR_ID_EXT_18                       (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+18)
#define FIFOPROTOCOL_SENSOR_ID_EXT_19                       (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+19)
#define FIFOPROTOCOL_SENSOR_ID_EXT_20                       (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+20)
#define FIFOPROTOCOL_SENSOR_ID_EXT_21                       (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+21)
#define FIFOPROTOCOL_SENSOR_ID_EXT_22                       (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+22)
#define FIFOPROTOCOL_SENSOR_ID_EXT_23                       (FIFOPROTOCOL_SENSOR_ID_EXT_BASE+23)

#define FIFOPROTOCOL_SENSOR_ID_EXT_MAX                      (FIFOPROTOCOL_SENSOR_ID_EXT_23+1)
#define FIFOPROTOCOL_SENSOR_ID_EXT_CNT                      (FIFOPROTOCOL_SENSOR_ID_EXT_MAX - FIFOPROTOCOL_SENSOR_ID_EXT_BASE)

/*
 * Custom sensor range
 */

#define FIFOPROTOCOL_SENSOR_ID_CUST_BASE                    FIFOPROTOCOL_SENSOR_ID_EXT_MAX

#define FIFOPROTOCOL_SENSOR_ID_CUST_0                       (FIFOPROTOCOL_SENSOR_ID_CUST_BASE+0)
#define FIFOPROTOCOL_SENSOR_ID_CUST_1                       (FIFOPROTOCOL_SENSOR_ID_CUST_BASE+1)
#define FIFOPROTOCOL_SENSOR_ID_CUST_2                       (FIFOPROTOCOL_SENSOR_ID_CUST_BASE+2)
#define FIFOPROTOCOL_SENSOR_ID_CUST_3                       (FIFOPROTOCOL_SENSOR_ID_CUST_BASE+3)
#define FIFOPROTOCOL_SENSOR_ID_CUST_4                       (FIFOPROTOCOL_SENSOR_ID_CUST_BASE+4)
#define FIFOPROTOCOL_SENSOR_ID_CUST_5                       (FIFOPROTOCOL_SENSOR_ID_CUST_BASE+5)
#define FIFOPROTOCOL_SENSOR_ID_CUST_6                       (FIFOPROTOCOL_SENSOR_ID_CUST_BASE+6)
#define FIFOPROTOCOL_SENSOR_ID_CUST_7                       (FIFOPROTOCOL_SENSOR_ID_CUST_BASE+7)

#define FIFOPROTOCOL_SENSOR_ID_CUST_MAX                     (FIFOPROTOCOL_SENSOR_ID_CUST_7+1)
#define FIFOPROTOCOL_SENSOR_ID_CUST_CNT                     (FIFOPROTOCOL_SENSOR_ID_CUST_MAX - FIFOPROTOCOL_SENSOR_ID_CUST_BASE)

/*
 * Audio sensor range
 *
 * Although we only have one audio sensor now, we may have more in the future
 */
// Use legacy define for Audio Base
#define FIFOPROTOCOL_SENSOR_ID_AUDIO_BASE                    48

#define FIFOPROTOCOL_SENSOR_ID_AUDIO_0                      (FIFOPROTOCOL_SENSOR_ID_AUDIO_BASE+0)
#define FIFOPROTOCOL_SENSOR_ID_AUDIO_1                      (FIFOPROTOCOL_SENSOR_ID_AUDIO_BASE+1)
#define FIFOPROTOCOL_SENSOR_ID_AUDIO_2                      (FIFOPROTOCOL_SENSOR_ID_AUDIO_BASE+2)
#define FIFOPROTOCOL_SENSOR_ID_AUDIO_3                      (FIFOPROTOCOL_SENSOR_ID_AUDIO_BASE+3)
#define FIFOPROTOCOL_SENSOR_ID_AUDIO_4                      (FIFOPROTOCOL_SENSOR_ID_AUDIO_BASE+4)
#define FIFOPROTOCOL_SENSOR_ID_AUDIO_5                      (FIFOPROTOCOL_SENSOR_ID_AUDIO_BASE+5)
#define FIFOPROTOCOL_SENSOR_ID_AUDIO_6                      (FIFOPROTOCOL_SENSOR_ID_AUDIO_BASE+6)
#define FIFOPROTOCOL_SENSOR_ID_AUDIO_7                      (FIFOPROTOCOL_SENSOR_ID_AUDIO_BASE+7)
#define FIFOPROTOCOL_SENSOR_ID_AUDIO_MAX                    (FIFOPROTOCOL_SENSOR_ID_AUDIO_BASE+8)

#define FIFOPROTOCOL_SENSOR_ID_AUDIO_CNT                    (FIFOPROTOCOL_SENSOR_ID_AUDIO_MAX - FIFOPROTOCOL_SENSOR_ID_AUDIO_BASE)

#define FIFOPROTOCOL_START_AUDIO_SENSOR FIFOPROTOCOL_SENSOR_ID_AUDIO_BASE
#define FIFOPROTOCOL_ZSP_AUDIO_SENSOR   FIFOPROTOCOL_SENSOR_ID_AUDIO_0
#define FIFOPROTOCOL_END_AUDIO_SENSOR   FIFOPROTOCOL_SENSOR_ID_AUDIO_MAX

#define FIFOPROTOCOL_NUM_AUDIO_SENSORS  (FIFOPROTOCOL_SENSOR_ID_AUDIO_CNT)

/*
 * Wake-up bit to specify if the commanded sensor is the wake-up version
 */
#define FIFOPROTOCOL_SENSOR_ID_WAKEUP_FLAG                  0x80


#ifdef __cplusplus
}
#endif

#endif /* _INV_FIFO_PROTOCOL_SENSOR_ID_H_ */

/** @} */

