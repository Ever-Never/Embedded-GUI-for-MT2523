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
#include "ble_ias.h"
#include "syslog.h"

/* Create the log control block as user wishes. Here we use 'BT_IAS' as module name.
 * User needs to define their own log control blocks as project needs.
 * Please refer to the log dev guide under /doc folder for more details.
 */
log_create_module(BLE_IAS, PRINT_LEVEL_INFO);

/************************************************
*   Global
*************************************************/ 

#define BLE_IAS_UUID16                       (0x1802)      /**< 16-bit UUID of Immediate Alert Service. */
#define BLE_IAS_ALERT_LEVEL_UUID16           (0x2A06)      /**< 16-bit UUID of Alert Level Characteristic. */
#define BLE_IAS_ALERT_LEVEL_VALUE_HANDLE     (0x0042)      /**< Attribute Vlaue Hanlde of Alert Level Characteristic. */

const bt_uuid_t BT_SIG_UUID_ALRT_LEVEL = BT_UUID_INIT_WITH_UUID16(BLE_IAS_ALERT_LEVEL_UUID16);

/************************************************
*   Utilities
*************************************************/
static uint32_t ble_ias_alert_level_write_callback_internal(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);


#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_ble_ias_alert_level_write_callback =_default_ble_ias_alert_level_write_callback")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION) || defined(__CC_ARM)
#pragma weak ble_ias_alert_level_write_callback = default_ble_ias_alert_level_write_callback
#else
#error "Unsupported Platform"
#endif

void default_ble_ias_alert_level_write_callback(uint16_t conn_handle, uint8_t alert_level)
{
    return;
}

static uint32_t ble_ias_alert_level_write_callback_internal(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
   LOG_I(BLE_IAS, "ble_ias_alert_level_write_callback_internal, opcode:%d, size:%d \r\n", rw, size);

    if (handle > 0) {                    
        if (BT_GATTS_CALLBACK_WRITE == rw) {
            if (0 != size) {
                uint8_t alert_level = 0;
                alert_level = *(uint8_t *)data;
                ble_ias_alert_level_write_callback(handle, alert_level);
                LOG_I(BLE_IAS, "Write conn_handle 0x%04x \r\n", handle);
            }  
            return sizeof(uint8_t);
        }
    }
    return 0;
}

BT_GATTS_NEW_PRIMARY_SERVICE_16(ble_ias_primary_service, BLE_IAS_UUID16);
BT_GATTS_NEW_CHARC_16(ble_ias_char4_alert_level,
    BT_GATT_CHARC_PROP_WRITE_WITHOUT_RSP, BLE_IAS_ALERT_LEVEL_VALUE_HANDLE, BLE_IAS_ALERT_LEVEL_UUID16);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_ias_alert_level, BT_SIG_UUID_ALRT_LEVEL,
                BT_GATTS_REC_PERM_WRITABLE,
                ble_ias_alert_level_write_callback_internal);

static const bt_gatts_service_rec_t *ble_ias_service_rec[] = {
    (const bt_gatts_service_rec_t*) &ble_ias_primary_service,
    (const bt_gatts_service_rec_t*) &ble_ias_char4_alert_level,
    (const bt_gatts_service_rec_t*) &ble_ias_alert_level,
    };

const bt_gatts_service_t ble_ias_service = {
    .starting_handle = 0x0040,
    .ending_handle = 0x0042,
    .required_encryption_key_size = 0,
    .records = ble_ias_service_rec
    };




