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

#ifndef __BATTERY_MANAGEMENT_H__
#define __BATTERY_MANAGEMENT_H__

/**
 * @addtogroup Battery_management_Group Battery Management
 * @{
 * This section introduces the battery management interface APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this module, battery management interface function groups, enums, structures and functions.
 * The battery management interface APIs can be used to provide battery information such as voltage, temperature, charger type and more.
 * @section BMT_Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |<b>BMT</b>      |Battery Management Task.|
 * |<b>GM</b>      |Gauge Master, software fuel gauge algorithms.|
 *
 * @section BMT_Architechture_Chapter Software Architecture of the battery management
 * Battery management creates a BMT task and provides software fuel gauge through the GM library.
 * @image html battery_management_architecture.png
 * @section Battery_management_Usage_Chapter How to use the battery management interface
 *
 * - Step 1: Call #battery_management_init() to initialize the battery management.
 *           Initialize the battery management after the hardware initialization, such as HAL driver and platform initialization, before creating any tasks.
 * - Step 2: Call #battery_management_register_callback() to register a callback function to receive the battery management event.
 * - Step 3: Call #battery_management_get_battery_property() to get the battery information.
 * - Step 4: Call #battery_management_deinit() to deinitialize the battery management before the system powers off.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif
/** @defgroup battery_management_define Define
  * @{
  */
/** @brief  This macro represents an invalid return value used in #battery_management_get_battery_property() function.
  *
  */
#define BATTERY_INVALID_VALUE     (-9999)
/**
  * @}
  */

/** @defgroup BATTERY_PROPERTY_ENUM Enum
  * @{
  */

/** @brief
 * This enum defines the return status of the battery management functions.
 */
typedef enum {
    BATTERY_MANAGEMENT_STATUS_INVALID_PARAMETER = -2,   /**< An invalid parameter. */
    BATTERY_MANAGEMENT_STATUS_ERROR = -1,               /**< An error occurred during the function call. */
    BATTERY_MANAGEMENT_STATUS_OK = 0                    /**< The function call was successful. */
} battery_management_status_t;

/** @brief Battery properties */
typedef enum {
    BATTERY_PROPERTY_CAPACITY = 0,          /**< Remaining battery capacity in an integer percentage form. */
    BATTERY_PROPERTY_CHARGING_CURRENT,      /**< Average charging current. */
    BATTERY_PROPERTY_CHARGER_EXIST,         /**< Charger existence status. */
    BATTERY_PROPERTY_CHARGER_TYPE,          /**< Charger type. */
    BATTERY_PROPERTY_TEMPERATURE,           /**< Battery temperature. */
    BATTERY_PROPERTY_VOLTAGE,               /**< Battery's average voltage. */
    BATTERY_PROPERTY_CAPACITY_LEVEL         /**< Remaining battery capacity from 0 to 5 levels. */
} battery_property_t;

/** @brief Battery management events */
typedef enum {
    BATTERY_MANAGEMENT_EVENT_BATTERY_UPDATE = 0x00000001,  /**< Battery update event. */
} battery_management_event_t;

/**
  * @}
  */

/** @defgroup battery_management_typedef Typedef
  * @{
  */
/** @brief The prototype of the battery management callback function, which needs to be registered to receive the battery management event.
  * @param[in] event is a notification sent by the battery management.
  * @param[in] data is a pointer to the data to be used by this callback function.
  */
typedef void(* battery_management_callback_t)(battery_management_event_t event, const void *data);

/**
  * @}
  */

/** @brief This function initializes the battery management. The battery management should be initialized after the hardware initialization and before creating any tasks.
*   @return  #BATTERY_MANAGEMENT_STATUS_OK, if the battery management initialization was successful. \n
*            #BATTERY_MANAGEMENT_STATUS_ERROR, if the battery management initialization failed.
*   */
battery_management_status_t battery_management_init(void);

/** @brief This function gets the current information about the battery.
*   @param [in] property is one of the battery properties defined in #battery_property_t.
*   @return Returns the value of the battery property.
*           #BATTERY_INVALID_VALUE, if the value is invalid.
* |Enum                   |Return description                                                      |
* |------------------------------|------------------------------------------------------------------------|
* |<b>BATTERY_PROPERTY_CAPACITY</b>             |Remaining battery capacity. The unit is percentage.|
* |<b>BATTERY_PROPERTY_CHARGING_CURRENT</b>     |Average charging current. The unit is mA.|
* |<b>BATTERY_PROPERTY_CHARGER_EXIST</b>        |Charger existence status 0:charger is not connected; 1:charger is connected.|
* |<b>BATTERY_PROPERTY_CHARGER_TYPE</b>         |Charger type 0: unknown; 1: standard host; 2: non-standard AC charger; 3: standard AC charger; 4: charging host.|
* |<b>BATTERY_PROPERTY_TEMPERATURE</b>          |Battery temperature. The unit is Celsius degrees.|
* |<b>BATTERY_PROPERTY_VOLTAGE</b>              |Battery's average voltage. The unit is mV.|
* |<b>BATTERY_PROPERTY_CAPACITY_LEVEL</b>       |Remaining battery capacity in level format. Level range is from 0 to 5,-1 means uninitialized.|
* */
int32_t battery_management_get_battery_property(battery_property_t property);

/** @brief This function registers a callback function to receive the battery management event.
*   @param[in] callback is the callback to be registered.
*   @return  #BATTERY_MANAGEMENT_STATUS_OK, if the callback function registration was successful. \n
*            #BATTERY_MANAGEMENT_STATUS_ERROR, if the callback function registration failed. \n
*            #BATTERY_MANAGEMENT_STATUS_INVALID_PARAMETER, if a given parameter was invalid.
*   */
battery_management_status_t  battery_management_register_callback(battery_management_callback_t callback);

/** @brief This function deinitializes the battery management.
*   @return  #BATTERY_MANAGEMENT_STATUS_OK, if the battery management deinitialization was successful. \n
*            #BATTERY_MANAGEMENT_STATUS_ERROR, if the battery management deinitialization failed.
*   */
battery_management_status_t  battery_management_deinit(void);


#ifdef __cplusplus
}
#endif

/**
* @}
*/

#endif /*__BATTERY_MANAGEMENT_H__*/

