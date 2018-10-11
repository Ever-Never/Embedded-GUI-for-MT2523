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


#ifndef __BT_GATTC_DISCOVERY_H__
#define __BT_GATTC_DISCOVERY_H__

#include "bt_uuid.h"
#include "bt_type.h"
#include "bt_system.h"
#include "bt_gattc.h"
#include "bt_gatt.h"
#include "bt_gap_le.h"
#include "bt_hci.h"
#include "bt_debug.h"

BT_EXTERN_C_BEGIN

/**
 * @defgroup bt_gattc_discovery_define Define
 * @{
 */

#define BT_GATTC_DISCOVERY_MAX_SRVS (10)  /**< Maximum number of services supported by this module. This also indicates the maximum number of users allowed to be registered to this module. */    

/**
 * @brief BLE UUID types.
 */
typedef uint8_t  ble_uuid_type_t;
#define BLE_UUID_TYPE_UNKNOWN       0x00 /**< Invalid UUID type. */
#define BLE_UUID_TYPE_16BIT         0x01 /**< Bluetooth SIG UUID (16-bit). */
#define BLE_UUID_TYPE_32BIT         0x02 /**< Bluetooth UUID (32-bit). */
#define BLE_UUID_TYPE_128BIT        0x03 /**< Vendor UUID types start at this index (128-bit). */

/**
 * @brief BLE Gattc Discovery status.
 */
typedef int32_t  bt_gattc_discovery_status_t;
#define BT_GATTC_DISCOVERY_STATUS_SUCCESS               0x00   /**< Operation success. */
#define BT_GATTC_DISCOVERY_STATUS_FAIL                 -0x01   /**< Operation fail. */
#define BT_GATTC_DISCOVERY_STATUS_INVALID_PARAMS       -0x02   /**< Input parameters error, like Null Pointer. */
#define BT_GATTC_DISCOVERY_STATUS_BUSY                 -0x03   /**< Busy. */
#define BT_GATTC_DISCOVERY_STATUS_INVALID_STATE        -0x04   /**< Invalid state, operation disallowed in this state. */
#define BT_GATTC_DISCOVERY_STATUS_OUT_OF_MEMORY        -0x05   /**< No Memory for operation. */
#define BT_GATTC_DISCOVERY_STATUS_INVALID_UUID         -0x06   /**< Invalid UUID. */

  
/**
 *  @brief Type of the GATTC discovery error code.
 */
 
typedef int32_t  bt_gattc_discovery_error_t;
#define BT_GATTC_DISCOVERY_ERROR_SERVICE_NOT_FOUND     0x01   /**< Error indicating that the service was not found at the peer.*/
#define BT_GATTC_DISCOVERY_ERROR_CHAR_FOUND_FAIL       0x02   /**< Error indicating that the characteristic was found fail.*/
#define BT_GATTC_DISCOVERY_ERROR_DESCRIPTOR_FOUND_FAIL 0x03   /**< Error indicating that the descriptor was found fail.*/
#define BT_GATTC_DISCOVERY_ERROR_UNEXPECT_DISCONNECT   0x04   /**< Error indicating that the service was not found fail because of disconnection come.*/

/**@brief Type of the GATTC Discovery event type.
 */
 
typedef int32_t  bt_gattc_discovery_result_t;
#define BT_GATTC_DISCOVERY_EVENT_COMPLETE              0x00   /**< Event indicating that the GATT Database discovery is complete. */
#define BT_GATTC_DISCOVERY_EVENT_FAIL                 -0x10   /**< Event indicating that an internal error has occurred in the BT GATTC Discovery module. This could typically be because of the SoftDevice API returning an error code during the DB discover.*/

/**
 * @}
 */

/**
 * @defgroup bt_gatt_discovery_struct Struct
 * @{
 */

/**
 *  @brief Bluetooth Low Energy UUID type, encapsulates both 16-bit and 128-bit UUIDs.
 */
typedef struct
{
    ble_uuid_type_t     type;  /**< UUID type, see @ref ble_uuid_type_t. If type is @ref BLE_UUID_TYPE_UNKNOWN, the value of uuid is undefined. */
    union {
        uint16_t uuid16;       /**< 16-bit UUID. */
        uint32_t uuid32;       /**< 32-bit UUID. */
        uint8_t uuid[16];      /**< An array to store 128-bit UUID. */
    } uuid;
} ble_uuid_t;

/**
 *  @brief Structure for holding the descriptor during the discovery process.
 */
typedef struct
{
    uint16_t         handle;                /**< Descriptor Handle value for this characteristic. */
    ble_uuid_t       descriptor_uuid;       /**< UUID of the descriptor. */
} bt_gattc_discovery_descriptor_t;

/**
 *  @brief Structure for holding information about the characteristic and the descriptors found during
 *         the discovery process.
 */
typedef struct
{
    ble_uuid_t                        char_uuid;         /**< UUID of the characteristic. */ 
    uint16_t                          handle;            /**< Handle of the characteristic. */ 
    uint16_t                          value_handle;      /**< Handle of the characteristic value. */ 
    uint8_t                           property;          /**< Property of the characteristic. */
    uint8_t                           descr_count_found; /**< Real found Number of Descriptors of a Characteristic you want to store. */
    uint8_t                           descriptor_count;  /**< User-defined, The Max Number of Descriptors of a Characteristic you want to store. */
    bt_gattc_discovery_descriptor_t   *descriptor;     /**< User-defined global buffer, to save the list of 1 or more descriptor Information. */
} bt_gattc_discovery_characteristic_t;

/**
 *  @brief Structure for holding information about the service and the characteristics found during
 *         the discovery process.
 */
typedef struct
{
    ble_uuid_t                  service_uuid;                 /**< UUID of the service. */    
    uint16_t                    start_handle;                 /**< Service Handle Range start. */
    uint16_t                    end_handle;                   /**< Service Handle Range end. */
    uint8_t                     char_count_found;             /**< Real Found Number of characteristics of a service. */
    uint8_t                     characteristic_count;         /**< User-defined, The Max Number of characteristics of a service you want to store. */
    bt_gattc_discovery_characteristic_t   *charateristics;  /**< User-defined global buffer, to save the list of information related to the characteristics present in the service. */
} bt_gattc_discovery_service_t;


/**
 *  @brief Structure containing the event from the DB discovery module to the application.
 */
typedef struct
{
    uint16_t                         conn_handle;      /**< Handle of the connection for which this event has occurred. */
    bt_gattc_discovery_result_t      event_type;       /**< Type of event. */
    union
    {
        bt_gattc_discovery_service_t *discovered_db;   /**< Structure containing the information about the GATT Database at the server. This will be filled when the event type is @ref BT_GATTC_DISCOVERY_EVENT_COMPLETE.*/
        int32_t                      error_code;       /**< Error code indicating the type of error which occurred in the DB Discovery module. This will be filled when the event type is @ref BT_GATTC_DISCOVERY_EVENT_FAIL. */
    } params;
} bt_gattc_discovery_event_t;

/**
 *  @brief BT GATTc Discovery event handler type. 
 */
typedef void (* bt_gattc_discovery_event_handler_t)(bt_gattc_discovery_event_t *event);

/**
 *  @brief   Structure for holding the information related to the GATT database at the server. This is intended for internal use during service discovery.
 *  @details This module identifies a remote database. Use one instance of this structure per 
 *           connection.
 *  @warning  This structure must be zero-initialized.
 */
typedef struct
{
    uint16_t                 conn_handle;                         /**< Connection handle as provided by the SoftDevice. */
    uint8_t                  descr_count;                          /**< The real Number of descriptor being discovered of the current characteristic. This is intended for internal use during service discovery.*/
    uint8_t                  char_count;                           /**< The real Number of characteristic being discovered of the current service. This is intended for internal use during service discovery.*/
    uint8_t                  curr_char_ind;                       /**< Index of the current characteristic being discovered. This is intended for internal use during service discovery.*/
    uint8_t                  curr_srv_ind;                        /**< Index of the current service being discovered. This is intended for internal use during service discovery.*/
    bool                     discovery_in_progress;               /**< Variable to indicate if there is a service discovery in progress. */
} bt_gattc_discovery_t; 

/**
 *  @brief   Array of structures containing pending events to be sent to the application modules.
 *  @details Whenever a discovery related event is to be raised to a user module, it will be stored
 *           in this array first. When all services needed to be discovered have been
 *           discovered, all pending events will be sent to the corresponding user modules.
 **/
typedef struct
{
    bt_gattc_discovery_event_t         event;          /**< The pending event. */
    bt_gattc_discovery_event_handler_t event_handler;  /**< The event handler which should be called to raise this event. */
} bt_gattc_pending_user_events;


/**
*  @brief Array of structures containing information about the registered application modules. 
*/
typedef struct
{
    ble_uuid_t                         service_uuid;     /**< The UUID of the service for which the application module had registered itself.*/
    bt_gattc_discovery_event_handler_t event_handler;      /**< The event handler of the application module to be called in case there are any events.*/
    bt_gattc_discovery_service_t       *service_info;
} bt_gattc_registered_handlers;

/**
 * @}
 */
 

/**
 * @brief   Function for initializing the GATTC Discovery module. It is better to invoke this function when bootup and it shuold be called one time in a project.
 * @param[in] void.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully.
 *            #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_gattc_discovery_init(void);

/**
 * @brief   Function for registering services with the GATTC Discovery module.
 * @details   The application can use this function to inform which service it is interested in 
 *            discovering at the server.
 * @param[in] uuid               Pointer to the UUID of the service to be discovered at the server.
 * @param[in] srv_info           Pointer to a user-defined structure of the service to store the discovered service information.
 *                               The pointer should not be NULL and should point to a global variable.
 * @param[in] handler            Event handler to be called by the GATTC discovery module when any event 
 *                               related to discovery failure of the registered services occurs or all registered services have been discovered.
 *
 * @note      The total number of services that can be discovered by this module is @ref 
 *            BT_GATTC_DISCOVERY_MAX_SRVS. This effectively means that the maximum number of 
 *            registrations possible is equal to the @ref BT_GATTC_DISCOVERY_MAX_SRVS. 
 *
 * @return    #BT_GATTC_DISCOVERY_STATUS_SUCCESS             Operation success.
 *            #BT_GATTC_DISCOVERY_STATUS_INVALID_PARAMS      When a NULL pointer is passed as input.
 *            #BT_GATTC_DISCOVERY_STATUS_INVALID_STATE       If this function is called without calling the @ref bt_gattc_discovery_init.
 *            #BT_GATTC_DISCOVERY_STATUS_OUT_OF_MEMORY       The maximum number of registrations allowed by this module has been reached.
 *            #BT_GATTC_DISCOVERY_STATUS_BUSY                If a discovery is already in progress. 
 */
bt_gattc_discovery_status_t bt_gattc_discovery_service_register(ble_uuid_t *uuid, bt_gattc_discovery_service_t *srv_info,
                                         const bt_gattc_discovery_event_handler_t handler);
                                     
/**
 * @brief   Function for starting the discovery of the GATT database at the server, it is better to invoke after bonding complete.
 * @param[in]  conn_handle The handle of the connection for which the discovery should be started.
 * @return     #BT_GATTC_DISCOVERY_STATUS_SUCCESS         Operation success.
 *             #BT_GATTC_DISCOVERY_STATUS_FAIL            Operation fail.
 *             #BT_GATTC_DISCOVERY_STATUS_INVALID_STATE   If this function is called without calling the @ref bt_gattc_discovery_init, or without calling @ref bt_gattc_discovery_evt_register.
 *             #BT_GATTC_DISCOVERY_STATUS_BUSY      If a discovery is already in progress for the current connection.
 */
bt_gattc_discovery_status_t bt_gattc_discovery_start(uint16_t conn_handle);


BT_EXTERN_C_END

#endif /*__BT_GATTC_DISCOVERY_H__*/

