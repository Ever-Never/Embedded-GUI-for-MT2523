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

#ifndef __BT_NOTIFY_H__
#define __BT_NOTIFY_H__
#include "stdbool.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#ifdef __MTK_BLE_ONLY_ENABLE__
#include "bt_spp_dummy.h"
#else
#include "bt_spp.h"
#endif
#include <FreeRTOS.h>
#include <timers.h>

/**

 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothServiceBT_Notify BT_Notify 
 * @{
 * The Bluetooth notification service is a Bluetooth service that parses the information from a remote device. The information could be a common notification, message, missed call or a customized content.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b SPP                        | Serial Port Profile. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles#Serial_Port_Profile_.28SPP.29">Wikipedia</a>. |
 * |\b SDP                       | Service Discovery Protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Service_discovery_protocol_.28SDP.29">Wikipedia</a>. |
 * |\b DOGP                     | Data Over GATT Profile. It is a GATT based profile defined by MediaTek. It is used to transmit (send and receive) data through a GATT server.|
 * |\b EXCD                     | Extend Command. It is used to send and receive customized data.|
 *
 * @section bt_notify_srv_api_usage How to use this module
 * - An example code to parse a common notification:
 *  - Step 1. Mandatory, implement #bt_sdps_get_customized_record() to add SDP records for the SPP Server.
 *     For more information about the SDP record, please refer to \ref SPP. Then call #bt_notify_init() with the SDP ID to initialize the Bluetooth notification service.
 *   - Sample code:
 *     @code
 *       #define BT_SPP_UUID    0x00,0x00,0xFF,0x01,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFF
 *       static const uint8_t bt_spp_service_class_id_pattern[] =
 *       {
 *           BT_SPP_SDP_ATTRIBUTE_UUID_LENGTH,
 *           BT_SPP_SDP_ATTRIBUTE_UUID(BT_SPP_STANDARD_UUID)
 *       };
 *       static const uint8_t bt_spp_protocol_descriptor_list_pattern[] =
 *       {
 *           BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(BT_SPP_SERVER_ID_START)
 *       };
 *       static const uint8_t bt_spp_browse_group_pattern[] =
 *       {
 *           BT_SPP_SDP_ATTRIBUTE_PUBLIC_BROWSE_GROUP
 *       };
 *       static const uint8_t bt_spp_language_pattern[] =
 *       {
 *           BT_SPP_SDP_ATTRIBUTE_LANGUAGE
 *       };
 *       static const uint8_t bt_spp_service_name_pattern[] =
 *       {
 *           BT_SPP_SDP_ATTRIBUTE_SIZE_OF_SERVICE_NAME(11),
 *           'S', 'e', 'r', 'i', 'a', 'l', ' ', 'P', 'o', 'r', 't'
 *       };
 *       static const bt_sdps_attribute_t bt_spp_sdp_attributes_pattern[] =
 *       {
 *           BT_SPP_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(bt_spp_service_class_id_pattern),
 *           BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESC_LIST(bt_spp_protocol_descriptor_list_pattern),
 *           BT_SPP_SDP_ATTRIBUTE_BROWSE_GROUP_LIST(bt_spp_browse_group_pattern),
 *           BT_SPP_SDP_ATTRIBUTE_LANGUAGE_BASE_LIST(bt_spp_language_pattern),
 *           BT_SPP_SDP_ATTRIBUTE_SERVICE_NAME(bt_spp_service_name_pattern)
 *       };
 *       static const bt_sdps_record_t bt_spp_sdp_record_pattern = 
 *       {
 *           .attribute_list_length = sizeof(bt_spp_sdp_attributes_pattern),
 *           .attribute_list = bt_spp_sdp_attributes_pattern,
 *       };
 *       static const bt_sdps_record_t *sdps_spp_record[]= {
 *           &bt_spp_sdp_record_pattern,
 *       };
 *       uint8_t bt_sdps_get_customized_record(const bt_sdps_record_t *** record_list)
 *       {
 *           *record_list = sdps_spp_record;
 *          return sizeof(sdps_spp_record)/sizeof(bt_sdps_record_t*);
 *       }
 *       // Initialize the Bluetooth notification service. Call this function in application initialization or in system initialization.
 *       bt_notify_init(BT_SPP_STANDARD_UUID);
 *     @endcode
 *  - Step 2. Mandatory, register a callback function for the service.
 *   - Sample code:
 *     @code
 *       if (BT_NOTIFY_RESULT_REGISTER_OK !=  bt_notify_register_callback(NULL, "app_test", my_bt_notify_callback)) {
 *            // Failed to register.
 *            return;
 *       }
 *       
 *    @endcode
 *  - Step 3. Mandatory, implement the callback function as registered in step 2, to handle the connection information, notifications, write indications, etc.
 *   - Sample code:
 *     @code
 *      void my_bt_notify_callback(void *data)
 *      {   
 *          bt_notify_callback_data_t *p_cb_data = (bt_notify_callback_data_t *)data;
 *          switch (p_cb_data->evt_id) {
 *              case BT_NOTIFY_EVENT_CONNECTION: {
 *                  // Handle connection event.
 *                  break;
 *              }
 *              case BT_NOTIFY_EVENT_DISCONNECTION: {
 *                  // Handle disconnection event.
 *                  break;
 *              }
 *              case BT_NOTIFY_EVENT_SEND_IND: {
 *                  // An indication to notify the application to send data.
 *                  break;
 *              }
 *              case BT_NOTIFY_EVENT_DATA_RECEIVED: {
 *                  // Notify the application that the EXCD data has been received from a remote device.
 *                   break;
 *              }
 *              case BT_NOTIFY_EVENT_NOTIFICATION: {
 *                  // Notify the application that a notification has been received from a remote device.
 *                   break;
 *              }
 *              case BT_NOTIFY_EVENT_MISSED_CALL: {
 *                  // Notify the application that a missed call notification has been received from a remote device.
 *                   break;
 *              }
 *              case BT_NOTIFY_EVENT_SMS: {
 *                  // Notify the application that an SMS notification has been received from a remote device.
 *                 break;
 *              }
 *              default: {
 *                   break;
 *              }
 *          }         
 *      }
 *       
 *    @endcode
 */



/*****************************************************************************
 * define
 *****************************************************************************/
/**
 * @defgroup Bluetoothservice_bt_notify_define Define
 * @{
 * This section defines the macros for the Bluetooth notification service.
 */

/**
 *  @brief Define the macros for the maximum and minimum buffer sizes and lengths.
 */

#define BT_NOTIFY_MIN_RECEIVE_DATA_BUF_SIZE   (2*1024 + 1)  /**< The minimum buffer size used by EXCD to receive data. (2*1024+1) is the default and minimum buffer size. */
#define BT_NOTIFY_MIN_PARSER_BUF_SIZE         (3*1024)      /**< The minimum buffer size of the notification parser buffer.*/
#define BT_NOTIFY_MAX_CHANNEL                    1          /**< The maximum number of channels supported by a service. Currently, a service supports only one Bluetooth channel (link).*/
#define BT_NOTIFY_MAX_SENDER_LENGTH              50         /**< The maximum length of the sender field of the notification. */
#define BT_NOTIFY_MAX_PAGE_CONTENT_LENGTH        300        /**< The length of the page content.*/
#define BT_NOTIFY_MAX_PAGE_TITLE_LENGTH          30         /**< The maximum length of the page title.*/
#define BT_NOTIFY_MAX_PHONE_NUMBER_LENGTH              20         /**< The maximum length of the phone number.*/

/*actions related*/
#define BT_NOTIFY_MAX_ACTION_NUM                  5         /**< The maximum number of actions.*/
#define BT_NOTIFY_MAX_ACTION_NAME_LENGTH         20         /**< The maximum length of the action name.*/
#define BT_NOTIFY_MAX_ACTION_ID_LENGTH           20         /**< The maximum length of the action ID field.*/
#define BT_NOTIFY_MAX_GROUP_ID_LENGTH               30         /**< The maximum length of the group ID field of the notification.*/
#define BT_NOTIFY_MAX_SENDER_RECEIVER_ID_LENGTH     16         /**< The maximum length of the sender ID or the receiver ID.*/
#ifdef _ICON_SUPPORT_ /*Define _ICON_SUPPORT_ macro to enable the notification icon feature, i.e. a notification with icon data from a remote device. The application needs to decode the icon data itself.*/
#define BT_NOTIFY_MAX_ICON_SIZE                2560            /**< The maximum size of the notification icon.*/
#endif /*_ICON_SUPPORT_*/



/**
 * @}
 */


/**
 * @defgroup Bluetoothservice_bt_notify_struct Struct
 * @{
 * This section defines the structures for the Bluetooth notification service.
 */

/**
 *  @brief Defines the callback function type.
 */

typedef void (*bt_notify_callback_t)(void *data); /**< The callback function type.*/
/**
 *  @brief The structure defines the Bluetooth notification service indication event.
 */

typedef enum {
    BT_NOTIFY_EVENT_NONE = 1,       /**< Invalid event.*/
    BT_NOTIFY_EVENT_CONNECTION,     /**< Connection indication event.*/
    BT_NOTIFY_EVENT_DISCONNECTION,  /**< Disconnection indication  event.*/
    BT_NOTIFY_EVENT_SEND_IND,       /**< The event indicating that the data could be sent.*/
    BT_NOTIFY_EVENT_DATA_RECEIVED,  /**< Data received event.*/
    BT_NOTIFY_EVENT_NOTIFICATION,   /**< Generic notification.*/
    BT_NOTIFY_EVENT_MISSED_CALL,    /**< Missed call notification event.*/
    BT_NOTIFY_EVENT_SMS,            /**< SMS notification event.*/
    BT_NOTIFY_EVENT_END             /**< The end of event definition.*/

} bt_notify_event_t;

/**
 *  @brief Define the  structure of the event #BT_NOTIFY_EVENT_DATA_RECEIVED.
 */

typedef struct {
    char sender_id[BT_NOTIFY_MAX_SENDER_RECEIVER_ID_LENGTH];               /**< The sender's ID of this device.*/
    char receiver_id[BT_NOTIFY_MAX_SENDER_RECEIVER_ID_LENGTH];             /**< The receiver's ID of the remote device.*/
    int32_t error_code;                                                    /**< The error code from the remote device.*/
    uint8_t  data[BT_NOTIFY_MIN_RECEIVE_DATA_BUF_SIZE];                    /**< The data buffer to hold the data from the remote device.*/
    uint16_t length;                                                       /**< The length of the data received from the remote device.*/
} bt_notify_event_data_t;


/**
 *  @brief Define the remote device system type.
 */

typedef enum {
    BT_NOTIFY_REMOTE_SYSTEM_NONE,       /**< Invalid value.*/
    BT_NOTIFY_REMOTE_SYSTEM_ANDROID,    /**< The remote device is an Android device.*/
    BT_NOTIFY_REMOTE_SYSTEM_IOS         /**< The remote device is an iOS device.*/
} bt_notify_remote_system_type_t;

/**
 *  @brief Define the action type of the notification received from a remote device.
 */

typedef enum {
    BT_NOTIFY_ACTION_TYPE_NEW,        /**< The action type to add a new notification.*/
    BT_NOTIFY_ACTION_TYPE_DELETE      /**< The action type to delete a notification.*/
} bt_notify_action_type_t;


#ifdef _ICON_SUPPORT_ /*Define _ICON_SUPPORT_ macro to enable the notification icon feature, i.e. a notification with icon data from a remote device. The application needs to decode the icon data itself.*/
typedef struct {
    uint8_t icon_buffer[BT_NOTIFY_MAX_ICON_SIZE]; /*Requires a Base64 decoder.*/
    uint32_t icon_size;
} bt_notify_data_icon_t;
#endif /*_ICON_SUPPORT_*/

/**
 *  @brief Define the page structure of the notification received from a remote device.
 */

typedef struct page_struct {
    uint16_t content[BT_NOTIFY_MAX_PAGE_CONTENT_LENGTH];   /**< The page content.*/ 
    uint16_t title[BT_NOTIFY_MAX_PAGE_TITLE_LENGTH];       /**< The page title.*/ 
    struct page_struct *next;                              /**< The next page.*/
} bt_notify_page_content_list_t;

/**
 *  @brief Define the action content of the notification received from a remote device.
 */

typedef struct {
    uint16_t action_id[BT_NOTIFY_MAX_ACTION_ID_LENGTH];         /**< The action ID.*/ 
    uint16_t action_name[BT_NOTIFY_MAX_ACTION_NAME_LENGTH];     /**< The action content.*/
} bt_notify_action_content_t;

/**
 *  @brief Define the notification content received from a remote device.
 */

typedef struct {
    uint16_t sender[BT_NOTIFY_MAX_SENDER_LENGTH];              /**< The notification's sender.*/
    uint32_t page_num;                                         /**< The notification's page number.*/
    bt_notify_page_content_list_t *page_content;                    /**< The notification's page content.*/
    uint32_t action_number;                                    /**< The action number.*/
    bt_notify_action_content_t action_content[BT_NOTIFY_MAX_ACTION_NUM];  /**< The action content.*/
    uint8_t group_id[BT_NOTIFY_MAX_GROUP_ID_LENGTH];              /**< The group ID.*/
    uint32_t timestamp;                                        /**< The timestamp.*/
    uint32_t app_id;                                           /**< The application ID of a remote device*/
    bt_notify_action_type_t action;                    /**< The action type.*/
    uint32_t msg_id;                                           /**< The notification ID.*/
#ifdef _ICON_SUPPORT_ /*Define _ICON_SUPPORT_ macro to enable the notification icon feature, i.e. a notification with icon data from a remote device. The application needs to decode the icon data itself.*/
    bt_notify_data_icon_t *icon;                               /**< The icon.*/
#endif /*_ICON_SUPPORT_*/

} bt_notify_notification_t;

/**
 *  @brief Define the SMS content received from a remote device.
 */

typedef struct {
    uint16_t sender[BT_NOTIFY_MAX_SENDER_LENGTH];            /**< The sender.*/
    uint16_t sender_number[BT_NOTIFY_MAX_PHONE_NUMBER_LENGTH]; /**< The phone number.*/
    bt_notify_page_content_list_t *page_content;             /**< The message content.*/
    uint32_t msg_id;                                         /**< The notification ID.*/
    uint32_t timestamp;                                      /**< the timestamp.*/
} bt_notify_sms_t;

/**
 *  @brief Define the missed call content received from the remote device.
 */

typedef struct {
    uint16_t sender[BT_NOTIFY_MAX_SENDER_LENGTH];      /**< The caller.*/
    uint16_t sender_number[BT_NOTIFY_MAX_PHONE_NUMBER_LENGTH]; /**< The call number.*/
    uint32_t msg_id;                                   /**< The notification ID.*/
    uint32_t missed_call_count;                        /**< The number of missed calls.*/
    uint32_t timestamp;                                /**< The timestamp.*/
} bt_notify_missed_call_t;

/**
 *  @brief Define the structure for the callback content.
 */


typedef struct {
    bt_notify_event_t evt_id;                /**< If the event ID is #BT_NOTIFY_EVENT_DATA_RECEIVED, read the corresponding event_data field from this structure. If the event ID is #BT_NOTIFY_EVENT_NOTIFICATION, read the corresponding notification field from this structure. If the event ID is #BT_NOTIFY_EVENT_MISSED_CALL, read the corresponding sms field from this structure. If the event ID is #BT_NOTIFY_EVENT_SMS, read the corresponding missed_call field from this structure. */
    bt_notify_remote_system_type_t remote_system; /**< The remote device's OS.*/
    bt_bd_addr_t bt_addr;                    /**< The remote device's Bluetooth address.*/
    bt_notify_event_data_t event_data;              /**< Data from remote device.*/
    bt_notify_notification_t notification;           /**< Generic notification.*/
    bt_notify_sms_t sms;                     /**< SMS notification.*/
    bt_notify_missed_call_t missed_call;            /**< Missed call notification.*/
} bt_notify_callback_data_t;

/**
 *  @brief This structure defines the Bluetooth notification service results.
 */


typedef enum {
    BT_NOTIFY_RESULT_NOT_SUPPORTED = -9,                        /**< Not supported.*/
    BT_NOTIFY_RESULT_INVALID_PARAMETER,                         /**< Invalid parameter.*/
    BT_NOTIFY_RESULT_PARSING_ERROR,                             /**< Notification parsing error.*/
    BT_NOTIFY_RESULT_CHANNAL_UNAVAILABLE,                       /**< No available channel.*/
    BT_NOTIFY_RESULT_FAILED,                                    /**< Failed.*/
    BT_NOTIFY_RESULT_REGISTER_INVALID_PARAMETER,                /**< Invalid parameter is given during the registration.*/
    BT_NOTIFY_RESULT_REGISTER_SYSTEM_ALREADY_REGISTERED,        /**< The command type (DATA, SYNC, ACKY, ACKN, VERN) have already registered in the service,*/
    BT_NOTIFY_RESULT_REGISTER_REPEATED_REGISTRATION,            /**< Repeated registration from the same sender with the same customized command.*/
    BT_NOTIFY_RESULT_REGISTER_NOT_IMPLEMENTED,                  /**< Not implemented yet.*/
    BT_NOTIFY_RESULT_REGISTER_OK,                               /**< Registration is successful.*/
    BT_NOTIFY_RESULT_NO_ERROR = BT_NOTIFY_RESULT_REGISTER_OK,   /**< Success.*/
} bt_notify_result_t;

/**
 *  @brief This structure defines the data transfer mode, SPP or DOGP mode.
 */

typedef enum {
    BT_NOTIFY_DATA_SOURCE_INVALID = 0x00,   /**< Invalid value.*/
    BT_NOTIFY_DATA_SOURCE_SPP = 0x01,       /**< Data transfer in the SPP mode.*/
    BT_NOTIFY_DATA_SOURCE_DOGP = 0x02,      /**< Data transfer in the DOGP mode.*/
} bt_notify_data_source_t;

/**
 * @}
 */

/**
 * @brief        This function registers a callback function to the Bluetooth notification service.
 * @param[in] bt_addr       is the Bluetooth address of a remote device. It can be NULL.
 * @param[in] sender         is a character string to identify the entity the callback is registered for.
 * @param[in] cb_ptr         is the callback function.
 * @return      The result of the registration, as defined in #bt_notify_result_t.
 */

bt_notify_result_t bt_notify_register_callback(const bt_bd_addr_t *bt_addr,  char *sender, bt_notify_callback_t cb_ptr);
/**
 * @brief        This function unregisters the callback function that has already been registered.
 * @param[in] bt_addr       is the Bluetooth address of a remote device. It cannot be NULL.
 * @param[in] cb_ptr         is the callback function to unregister.
 * @return      The result of the registration, as defined in #bt_notify_result_t.
 */

bt_notify_result_t bt_notify_deregister_callback(const bt_bd_addr_t *bt_addr, bt_notify_callback_t cb_ptr);
/**
 * @brief         Call this function to send data. Wait for the #BT_NOTIFY_EVENT_SEND_IND event to send more data.
 * @param[in] bt_addr       is the Bluetooth address of the remote device to send data to. It cannot be NULL.
 * @param[in] data            is the data to be sent.
 * @param[in] data_len      is the data length.
 * @param[in] is_new        is the flag indicating if the data is new.
 * @return      The length of the send data.  
 */

int32_t bt_notify_send_data(bt_bd_addr_t *bt_addr,  const char *data, uint16_t data_len, bool is_new);

/**
 * @brief        This function initializes the Bluetooth notification service. It should be called either in application initialization or in system initialization.
 * @param[in] sdp_id     is the record ID of the SDP.        
 */

void bt_notify_init(uint8_t sdp_id);

/**
 * @brief        Call this function to get the data transfer mode.
 * @param[in] bt_addr       is the Bluetooth address of a remote device. It cannot be NULL.   
 * @return      The data transfer mode, SPP or DOGP mode.
 */

bt_notify_data_source_t bt_notify_get_data_source(bt_bd_addr_t *bt_addr);


/**
 * @}
 * @}
 *
*/

#endif/*__BT_NOTIFY_H__*/
