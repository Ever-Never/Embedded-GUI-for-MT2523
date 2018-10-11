/* Copyright Statement:
 *
 * (C) 2005-2016 MediaTek Inc. All rights reserved.
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
#ifndef __BLE_ANCS_H__
#define __BLE_ANCS_H__

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothServicesBLE_ANCS BLE_ANCS
 * @{
 * Apple Notification Center Service (ANCS) is a convenient way to access different types of notifications generated on iOS devices. This section describes 
 * the ANCS APIs, enums and structures defined in the service, and steps on how to use this service to get the iOS notifications. Please refer
 * to the example project ble_ancs_ios_notifcation to get more detailed information. ANCS is supported only on MT25x3 platform. Please refer to the ANCS specification at 
 * https://developer.apple.com/library/content/documentation/CoreBluetooth/Reference/AppleNotificationCenterServiceSpecification/Introduction/Introduction.html
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b ANCS                      | Apple Notification Center Service. |
 * |\b NP                        | The publisher of the ANCS service is referred to as the Notification Provider (NP). |
 * |\b NC                        | Any client of the ANCS service is referred to as a Notification Consumer (NC).|
 *
 * @section ble_ancs_api_usage How to use this module
 *
 * ANCS should search for the services and characteristics in the callback function #ble_ancs_event_handler(). 
 * - Step 1: Call #bt_gattc_discovery_start() to start GATT service search. When #BT_GATTC_DISCOVERY_EVENT_COMPLETE is received, please initialize the ANCS by calling #ble_ancs_init().
 *
 *  - Sample code:
 *     @code
 *static void ble_ancs_gattc_callback(bt_gattc_discovery_event_t *event)
 *{
 *    
 *    switch (event->event_type) {
 *       case BT_GATTC_DISCOVERY_EVENT_COMPLETE:
 *           {
 *               uint8_t noti_source_uuid[16] = {0xbd, 0x1d, 0xa2, 0x99, 0xe6, 0x25, 0x58, 0x8c, 0xd9, 0x42, 0x01, 0x63, 0x0d, 0x12, 0xbf, 0x9f};
 *               uint8_t data_source_uuid[16] = {0xfb, 0x7b, 0x7c, 0xce, 0x6a, 0xb3, 0x44, 0xbe, 0xb5, 0x4b, 0xd6, 0x24, 0xe9, 0xc6, 0xea, 0x22};
 *               uint8_t i;
 *               ble_ancs_charc_t charc[BLE_ANCS_MAX_CHARC_NUMBER];
 *               
 *               LOG_I(ANCS, "DISCOVER_COMPLETE: start = %d, end = %d, char_num = %d, handle = %d, %d, %d\n", g_ancs_srv.service.start_handle, 
 *                   g_ancs_srv.service.end_handle, g_ancs_srv.service.char_count_found, g_ancs_srv.service.charateristics[0].handle, 
 *                   g_ancs_srv.service.charateristics[1].handle, g_ancs_srv.service.charateristics[2].handle);
 *
 *    
 *                for (i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) {
 *
 *                    if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, data_source_uuid, 16)) {
 *                       (charc + i)->uuid_type = BLE_ANCS_UUID_DATA_SOURCE;
 *
 *                   } else if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, noti_source_uuid, 16)) {
 *                       (charc + i)->uuid_type = BLE_ANCS_UUID_NOTIFICATION_SOURCE;
 *
 *                   } else {
 *                       (charc + i)->uuid_type = BLE_ANCS_UUID_CONTROL_POINT;
 *
 *                   }
 *                   (charc + i)->value_handle = g_ancs_srv.service.charateristics[i].value_handle;
 *                   (charc + i)->desc_handle = g_ancs_srv.service.charateristics[i].descriptor[0].handle;
 *               }
 *               
 *               ble_ancs_init(event->conn_handle, charc);
 *           }
 *           break;
 *           
 *       default:
 *           break;
 *  }
 *}
 *     @endcode
 * - Step 2: Enable the data source and notification source to enable ANCS notifications by calling #ble_ancs_enable_data_source() and #ble_ancs_enable_notification_source(). NC receives the notifications automatically.
 * - Step 3: Get Notification Attribute (optional). Call #ble_ancs_get_notification_attributes() to retrieve the attributes of a specific iOS notification. Afer the event #BT_GATTC_CHARC_VALUE_NOTIFICATION is received, call #ble_ancs_parse_notification() to parse the notification attributes. 
 * - Step 4: Get app attribute (optional). Call #ble_ancs_get_app_attributes() if the attributes of a specific app installed on the NP are needed. After the event #BT_GATTC_CHARC_VALUE_NOTIFICATION is received, call #ble_ancs_parse_attributes() to parse the app attributes.
 *  - Sample code:
 *     @code
 * static bt_status_t ble_ancs_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff)
 *{
 *    switch (msg)
 *   {    
 *       case BT_GATTC_CHARC_VALUE_NOTIFICATION:
 *           {
 *               bt_gatt_handle_value_notification_t *data = (bt_gatt_handle_value_notification_t *)buff;
 *               ble_ancs_event_t ancs_evt;
 *               uint8_t i, m;
 *               uint8_t noti_source_uuid[16] = {0xbd, 0x1d, 0xa2, 0x99, 0xe6, 0x25, 0x58, 0x8c, 0xd9, 0x42, 0x01, 0x63, 0x0d, 0x12, 0xbf, 0x9f};
 *               uint8_t data_source_uuid[16] = {0xfb, 0x7b, 0x7c, 0xce, 0x6a, 0xb3, 0x44, 0xbe, 0xb5, 0x4b, 0xd6, 0x24, 0xe9, 0xc6, 0xea, 0x22};
 *               
 *               for(i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) {
 *                  if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, noti_source_uuid, 16)) {
 *                      break;
 *                  }
 *               }
 *
 *                for (m = 0; m < BLE_ANCS_MAX_CHARC_NUMBER; m++) {
 *                   if (!memcmp(g_ancs_srv.service.charateristics[m].char_uuid.uuid.uuid, data_source_uuid, 16)) {
 *                       break;
 *                   }
 *               }
 *               if (data->att_rsp->handle == g_ancs_srv.service.charateristics[i].value_handle) {
 *
 *                   ancs_evt.result = ble_ancs_parse_notification(&ancs_evt.data.notification, data->length - 3, data->att_rsp->attribute_value);
 *                   ancs_evt.connection_handle = data->connection_handle;
 *                   ancs_evt.evt_type = BLE_ANCS_EVENT_IOS_NOTIFICATION;
 *                   g_ancs_srv.evt_handler(&ancs_evt);
 *                   
 *               } else if (data->att_rsp->handle == g_ancs_srv.service.charateristics[m].value_handle) {
 *                       bt_status_t result;
 *           
 *                       result = ble_ancs_parse_attributes(data->connection_handle, data->length - 3, data->att_rsp->attribute_value, &ancs_evt.data.attribute);
 *                   
 *                       if (BT_STATUS_SUCCESS == result || BT_STATUS_FAIL == result) {
 *                       //send event to app
 *                       ancs_evt.result = result;
 *                       ancs_evt.connection_handle = data->connection_handle;
 *                       if (ancs_evt.data.attribute.command_id == 0) {
 *                           ancs_evt.evt_type = BLE_ANCS_EVENT_NOTIFICATION_ATTRIBUTE;
 *                       } else if (ancs_evt.data.attribute.command_id == 1) {
 *                           ancs_evt.evt_type = BLE_ANCS_EVENT_APP_ATTRIBUTE;
 *                       }
 *                       
 *                       LOG_I(ANCS, "Parse attribute done, result = %d, event = %d\r\n", result, ancs_evt.evt_type);
 *                       g_ancs_srv.evt_handler(&ancs_evt);
 *                   }    
 *               }
 *               break;
 *           default:
 *               break;
 *      }
 *   }
 *     @endcode
 * - Step 5: Perform Notification Action (optional). Starting with iOS 8.0, the NC can perform a predetermined action on a specific iOS notification.
 * Call #ble_ancs_perform_notification_action() to implement this function.
 */

#include "stdint.h"
#include "bt_type.h"

BT_EXTERN_C_BEGIN

/**
<<<<<<< HEAD
 * @defgroup BluetoothBLE_ANCS_define Define
||||||| merged common ancestors
 * @defgroup Bluetoothhbif_gap_define Define
=======
 * @defgroup Bluetoothhbif_ble_ancs_define Define
>>>>>>> origin/geneva-mp-mp3
 * @{
 */

#define BLE_ANCS_MAX_EVENT_ID_NUMBER            (3)     /**< The number of event types, added, modified and removed.*/
#define BLE_ANCS_MAX_CATEGORY_ID_NUMBER         (12)    /**< The number of notification categories.*/
#define BLE_ANCS_MAX_NOTIFICATION_ATTR_NUMBER   (8)     /**< The number of notification attributes.*/
#define BLE_ANCS_MAX_CHARC_NUMBER               (3)     /**< The number of ANCS characteristics.*/
#define BLE_ANCS_NOTIFICATION_UID_LENGTH        (4)     /**< The length of ANCS notification UID.*/


#define BLE_ANCS_ACTION_ID_POSITIVE         0       /**< The positive action ID, used in Perform Notification Action.*/
#define BLE_ANCS_ACTION_ID_NEGATIVE         1       /**< The negative action ID, used in Perform Notification Action.*/
typedef uint8_t ble_ancs_action_id_t;             /**< The ANCS action type.*/


#define BLE_ANCS_CATEGORY_ID_OTHER                  0   /**< Unclassified notification category.*/
#define BLE_ANCS_CATEGORY_ID_INCOMING_CALL          1   /**< The incoming call notification category.*/
#define BLE_ANCS_CATEGORY_ID_MISSED_CALL            2   /**< The missed call notification category.*/
#define BLE_ANCS_CATEGORY_ID_VOICEMAIL              3   /**< The voicemail notification category.*/
#define BLE_ANCS_CATEGORY_ID_SOCIAL                 4   /**< The social notification category.*/
#define BLE_ANCS_CATEGORY_ID_SCHEDULE               5   /**< The schedule notification category.*/
#define BLE_ANCS_CATEGORY_ID_EMAIL                  6   /**< The email notification category.*/
#define BLE_ANCS_CATEGORY_ID_NEWS                   7   /**< The news notification category.*/
#define BLE_ANCS_CATEGORY_ID_HEALTH_AND_FITNESS     8   /**< The health and fitness notification category.*/
#define BLE_ANCS_CATEGORY_ID_BUSINESS_AND_FINANCE   9   /**< The business and finance notification category.*/
#define BLE_ANCS_CATEGORY_ID_LOCATION               10  /**< The location notification category.*/
#define BLE_ANCS_CATEGORY_ID_ENTERTAINMENT          11  /**< The entertainment notification category.*/
typedef uint8_t ble_ancs_category_id_t;               /**< The ANCS notification category type.*/


#define BLE_ANCS_EVENT_ID_NOTIFICATION_ADDED    0   /**< The notification is newly added, usually it means a notification is received.*/
#define BLE_ANCS_EVENT_ID_NOTIFICATION_MODIFIED 1   /**< The notification is modified, rarely to see this type of event.*/
#define BLE_ANCS_EVENT_ID_NOTIFICATION_REMOVED  2   /**< The notification is removed. Usually the user just browsed the notification and it will be removed.*/
typedef uint8_t ble_ancs_event_id_t;    /**< The ANCS notification event ID.*/


#define BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_APP_IDENTIFIER           0   /**< The attribute app identifier, usually it is the app name.*/
#define BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_TITLE                    1   /**< The attribute title, it is the title of the notification.*/
#define BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_SUBTITLE                 2   /**< The attribute subtitle.*/
#define BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_MESSAGE                  3   /**< The attribute message, which is the notification content.*/
#define BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_MESSAGE_SIZE             4   /**< The attribute message size in bytes.*/
#define BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_DATE                     5   /**< The attribute date, which is the notification timestamp.*/
#define BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_POSITIVE_ACTION_LABEL    6   /**< The attribute positive action label, a string describing the positive action.*/
#define BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_NEGATIVE_ACTION_LABEL    7   /**< The attribute negative action label, a string describing the negative action.*/
typedef uint8_t ble_ancs_notification_attribute_id_t;   /**< The ANCS notification attribute ID.*/


#define BLE_ANCS_UUID_PRIMARY_SERVICE       0   /**< The enum of the ANCS primary service.*/
#define BLE_ANCS_UUID_NOTIFICATION_SOURCE   1   /**< The enum of the ANCS characteristic notification source.*/
#define BLE_ANCS_UUID_CONTROL_POINT         2   /**< The enum of the ANCS characteristic control point.*/
#define BLE_ANCS_UUID_DATA_SOURCE           3   /**< The enum of the ANCS characteristic data source.*/
typedef uint8_t ble_ancs_uuid_t;   /**< The ANCS service and characteristic type.*/
/**
 * @}
 */


/**
 * @defgroup BluetoothBLE_ANCS_struct Struct
 * @{
 */

/**
 *  @brief This structure defines the notification attributes or app attributes.
 */
typedef struct {
    ble_ancs_notification_attribute_id_t attribute_id;  /**< Attribute ID.*/
    uint16_t atrribute_len;                             /**< The length of the attribute data in bytes.*/
    uint8_t *data;                                      /**< Attribute data pointer.*/
} ble_ancs_attribute_list_t;


/**
 *  @brief This structure defines the notification details.
 */
typedef struct {
    uint8_t command_id;                                                         /**< Defined by the ANCS specification. 0, Get Notification Attribute, and 1, Get App attribute.*/
    uint8_t notification_uid[BLE_ANCS_NOTIFICATION_UID_LENGTH];                 /**< The UID of the iOS notification.*/
    uint8_t attr_num;                                                           /**< The number of attributes.*/
    ble_ancs_attribute_list_t attr_list[BLE_ANCS_MAX_NOTIFICATION_ATTR_NUMBER]; /**< The attribute list.*/
} ble_ancs_event_attribute_t;


/**
 *  @brief This structure defines the general information of a notification.
 */
typedef struct {
    ble_ancs_event_id_t event_id;       /**< The event ID of the notification (Added, Removed or Modified), as defined in the ANCS specification.*/
    uint8_t event_flags;                /**< The bitmask, as defined in the ANCS specification.*/
    ble_ancs_category_id_t category_id; /**< The category ID of the notification, as defined in the ANCS specification.*/
    uint8_t category_count;             /**< The current number of active iOS notifications in the given category, as defined in the ANCS specification.*/
    uint8_t notification_uid[BLE_ANCS_NOTIFICATION_UID_LENGTH];        /**< The UID of the iOS notification, as defined in the ANCS specification.*/
} ble_ancs_event_notification_t;


/**
 *  @brief This structure defines the initial conditions for the ANCS.
 */
typedef struct {
    ble_ancs_uuid_t uuid_type;      /**< UUID type.*/
    uint16_t value_handle;          /**< The handle of characteristic value.*/
    uint16_t desc_handle;           /**< The handle of descriptor.*/
} ble_ancs_charc_t;
/**
 * @}
 */



/**
 * @brief               This function initializes the ANCS.
 * @param[in] connection_handle   is the handle of the current connection.
 * @param[in] charc      is the characteristic information.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the ANCS connection is full or ANCS does not support this chip.
 */
bt_status_t ble_ancs_init(bt_handle_t connection_handle, ble_ancs_charc_t *charc);

/**
 * @brief               This function deinitializes ANCS. It can be called at Bluetooth LE disconnection.
 * @param[in] connection_handle   is the handle of the current connection.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the ANCS does not support this chip.
 */
bt_status_t ble_ancs_deinit(bt_handle_t connection_handle);

/**
 * @brief               This function enables the notification source to send notifications with an iOS device.
 * @param[in] connection_handle   is the handle of the current connection.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the ANCS does not support this chip or connection handle is wrong or ANCS is not initialized or fail to write the characteristic.
 *                      #BT_STATUS_BUSY, the service is busy.
 */
bt_status_t ble_ancs_enable_notification_source(bt_handle_t connection_handle);

/**
 * @brief               This function disables the notification source and stops the iOS device to send notifications.
 * @param[in] connection_handle   is the handle of the current connection.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the ANCS does not support this chip or connection handle is wrong or ANCS is not initialized or fail to write the characteristic.
 *                      #BT_STATUS_BUSY, the service is busy.
 */
bt_status_t ble_ancs_disable_notification_source(bt_handle_t connection_handle);

/**
 * @brief               This function enables the NC to retrieve more information about the iOS notification.
 * @param[in] connection_handle   is the handle of the current connection.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the ANCS does not support this chip or connection handle is wrong or ANCS is not initialized or fail to write the characteristic.
 *                      #BT_STATUS_BUSY, the service is busy.
 */
bt_status_t ble_ancs_enable_data_source(bt_handle_t connection_handle);

/**
 * @brief               This function disables the NC to retrieve more information about the iOS notification.
 * @param[in] connection_handle   is the handle of the current connection.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the ANCS does not support this chip or connection handle is wrong or ANCS is not initialized or fail to write the characteristic.
 *                      #BT_STATUS_BUSY, the service is busy.
 */
bt_status_t ble_ancs_disable_data_source(bt_handle_t connection_handle);

/**
 * @brief               This function retrieves the attributes of a specific iOS notification.
 * @param[in] connection_handle   is the handle of the current connection.
 * @param[in] uid   is the uuid of the notification.
 * @param[in] attr_list   is the attribute list requested.
 * @param[in] attr_num   is the number of attribute list requested.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the ANCS does not support this chip or connection handle is wrong or ANCS is not initialized or fail to write the characteristic or the input parameters are wrong.
 *                      #BT_STATUS_BUSY, the service is busy.
 */
bt_status_t ble_ancs_get_notification_attributes(bt_handle_t connection_handle,
        uint8_t uid[BLE_ANCS_NOTIFICATION_UID_LENGTH], ble_ancs_attribute_list_t *attr_list, uint8_t attr_num);

/**
 * @brief               This function retrieves the attributes of a specific app installed on the Notification Provider.
 * @param[in] connection_handle   is the handle of the current connection.
 * @param[in] appid   is the pointer of the app ID string.
 * @param[in] appid_len   is the app ID length.
 * @param[in] attr_list   is the attribute list requested.
 * @param[in] attr_num   is the number of attribute list requested.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the ANCS does not support this chip or connection handle is wrong or ANCS is not initialized or fail to write the characteristic or the input parameters are wrong.
 *                      #BT_STATUS_BUSY, the service is busy.
 */
bt_status_t ble_ancs_get_app_attributes(bt_handle_t connection_handle,
        uint8_t *appid,
        uint16_t appid_len,
        ble_ancs_attribute_list_t *attr_list,
        uint8_t attr_num);

/**
 * @brief               This function performs a predetermined action on a specific iOS notification.
 * @param[in] connection_handle   is the handle of the current connection.
 * @param[in] uid   is the UID of the iOS notification on which the client wants to perform an action.
 * @param[in] action_id   is the desired action on the Notification Consumer wants to be performed on the iOS notification.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the ANCS does not support this chip or connection handle is wrong or ANCS is not initialized or fail to write the characteristic or the input parameters are wrong.
 *                      #BT_STATUS_BUSY, the service is busy.
 */
bt_status_t ble_ancs_perform_notification_action(bt_handle_t connection_handle,
        uint8_t uid[BLE_ANCS_NOTIFICATION_UID_LENGTH], ble_ancs_action_id_t action_id);

/**
 * @brief               This function is a static callback for the ANCS service to listen to the bt stack events.
 * @param[in] msg     is the callback message type.
 * @param[in] status  is the status of the callback message.
 * @param[in] buffer   is the payload of the callback message.
 * @return            #BT_STATUS_SUCCESS, the operation completed successfully.
 */
bt_status_t ble_ancs_event_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);

/**
 * @brief               This function parses the Notification Source which informs the client the arrival of a new iOS notification.
 * @param[out] notification   is the structure to save the parsed information.
 * @param[in] length   is the number of bytes of the data to parse.
 * @param[in] data   is the received Notification Source.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the length is not 8.
 */
bt_status_t ble_ancs_parse_notification(ble_ancs_event_notification_t *notification, uint16_t length, uint8_t *data);

/**
 * @brief               This function parses the Notification Attributes and App Attributes.
 * @param[in] connection_handle   is the handle of the current connection.
 * @param[in] length   is the length of the data to parse.
 * @param[in] data   is the received Notification Attributes or App Attributes.
 * @param[out] attribute   is the buffer of pointer to save the parsed attributes.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the data is wrong making the parse fail.
 *                      #BT_STATUS_PENDING, the Notification Attributes are not fully received, wait for the next part of the  Notification Attributes to parse.
 */
bt_status_t ble_ancs_parse_attributes(bt_handle_t connection_handle, uint16_t length, uint8_t *data, ble_ancs_event_attribute_t *attribute);

BT_EXTERN_C_END

/**
 * @}
 * @}
 */

#endif /*__BLE_ANCS_H__*/

