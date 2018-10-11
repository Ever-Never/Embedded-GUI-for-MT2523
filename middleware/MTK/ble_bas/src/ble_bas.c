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

#include <stdint.h>
#include "ble_bas.h"
#include "syslog.h"

/* Create the log control block as user wishes. Here we use 'BLE_BAS' as module name.
 * User needs to define their own log control blocks as project needs.
 * Please refer to the log dev guide under /doc folder for more details.
 */
log_create_module(BLE_BAS, PRINT_LEVEL_INFO);

/************************************************
*   Macro and Structure
*************************************************/
#define BAS_BATTERY_LEVEL_VALUE_HANDLE   (0x0033)   /**< Attribute Vlaue Hanlde of Battery Level Characteristic. */

/************************************************
*   Global
*************************************************/ 
const bt_uuid_t BT_SIG_UUID_BATTERY_LEVEL = BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_BATTERY_LEVEL);

/************************************************
*   Utilities
*************************************************/
static uint32_t ble_bas_battery_level_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
static uint32_t ble_bas_client_config_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_ble_bas_write_callback =_default_ble_bas_write_callback")
#pragma comment(linker, "/alternatename:_ble_bas_read_callback =_default_ble_bas_read_callback")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION) || defined(__CC_ARM)
#pragma weak ble_bas_write_callback = default_ble_bas_write_callback
#pragma weak ble_bas_read_callback = default_ble_bas_read_callback
#else
#error "Unsupported Platform"
#endif

uint8_t default_ble_bas_read_callback(ble_bas_event_t event, bt_handle_t conn_handle)
{
    return 0;
}

void default_ble_bas_write_callback(ble_bas_event_t event, bt_handle_t conn_handle, void *data)
{
    return;
}

BT_GATTS_NEW_PRIMARY_SERVICE_16(ble_bas_primary_service, BT_GATT_UUID16_BATTERY_SERVICE);
BT_GATTS_NEW_CHARC_16(ble_bas_char4_battery_level,
    BT_GATT_CHARC_PROP_READ|BT_GATT_CHARC_PROP_NOTIFY, BAS_BATTERY_LEVEL_VALUE_HANDLE, BT_SIG_UUID16_BATTERY_LEVEL);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_bas_battery_level, BT_SIG_UUID_BATTERY_LEVEL,
                BT_GATTS_REC_PERM_READABLE,
                ble_bas_battery_level_callback);
BT_GATTS_NEW_CLIENT_CHARC_CONFIG(ble_bas_client_config,
        BT_GATTS_REC_PERM_READABLE|BT_GATTS_REC_PERM_WRITABLE,
        ble_bas_client_config_callback);

static const bt_gatts_service_rec_t *ble_bas_service_rec[] = {
    (const bt_gatts_service_rec_t*) &ble_bas_primary_service,
    (const bt_gatts_service_rec_t*) &ble_bas_char4_battery_level,
    (const bt_gatts_service_rec_t*) &ble_bas_battery_level,
    (const bt_gatts_service_rec_t*) &ble_bas_client_config,
    };

const bt_gatts_service_t ble_bas_service = {
    .starting_handle = 0x0031,
    .ending_handle = 0x0034,
    .required_encryption_key_size = 0,
    .records = ble_bas_service_rec
    };

/** Battery State Service 0x0021*/
/*need to do, how to get the current battery level*/
static uint32_t ble_bas_battery_level_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_I(BLE_BAS, "ble_bas_battery_level_callback, opcode:%d, size:%d \r\n", rw, size);

    if (handle > 0) {                    
        if (rw == BT_GATTS_CALLBACK_READ) {
            if (0 != size) {
                uint8_t *read_rsp_value = (uint8_t*)data;
                *read_rsp_value = ble_bas_read_callback(BLE_BAS_EVENT_BATTRY_LEVEL_READ, handle);
                LOG_I(BLE_BAS, "read battery value = %d\r\n", *read_rsp_value);
            }  
            return sizeof(uint8_t);
        }
    }
    return 0;
}

/** Client Characteristic Configuration Descriptor. */
static uint32_t ble_bas_client_config_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_I(BLE_BAS, "ble_bas_client_config_callback, opcode:%d, size:%d \r\n", rw, size);
    
    if (handle > 0) {
        /** record for each connection. */                
        if (rw == BT_GATTS_CALLBACK_WRITE){
            if (size != sizeof(uint16_t)){ //Size check
                return 0;
            }
            ble_bas_write_callback(BLE_BAS_EVENT_CCCD_WRITE, handle, data);
            LOG_I(BLE_BAS, "ble_bas_client_config_callback, data:%d \r\n", *(uint16_t *)data);
        } else if (rw == BT_GATTS_CALLBACK_READ) {
            if (size != 0) {
                uint16_t *buf = (uint16_t*) data;
                *buf = (uint16_t)ble_bas_read_callback(BLE_BAS_EVENT_CCCD_READ, handle);
                LOG_I(BLE_BAS, "read cccd value = %d\r\n", *buf);
            }
        }
        return sizeof(uint16_t);
    }
    return 0;
}

bt_status_t ble_bas_notify_battery_level(bt_handle_t conn_handle, uint8_t battery_level)
{
    uint8_t buf[32]={0};
    bt_gattc_charc_value_notification_indication_t *bas_noti_rsp;
    bas_noti_rsp = (bt_gattc_charc_value_notification_indication_t*) buf;

    LOG_I(BLE_BAS, "%s: new battery level is %d\r\n", __FUNCTION__, battery_level);  

    
    if (conn_handle > 0) {
        bas_noti_rsp->att_req.opcode = BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION;
        bas_noti_rsp->att_req.handle = BAS_BATTERY_LEVEL_VALUE_HANDLE;
        memcpy((void*)(bas_noti_rsp->att_req.attribute_value), (void *)(&battery_level), sizeof(uint8_t));

        bas_noti_rsp->attribute_value_length = 3 + sizeof(uint8_t);
        
        LOG_I(BLE_BAS, "ble_bas_notify_battery_level conn_handle is %x, battery_level is %d\r\n", conn_handle, battery_level);
        return bt_gatts_send_charc_value_notification_indication(conn_handle, bas_noti_rsp); 
    }
    return BT_STATUS_FAIL;
}


