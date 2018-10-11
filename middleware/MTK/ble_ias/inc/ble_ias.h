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
#ifndef __BLE_IAS_H__
#define __BLE_IAS_H__

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothServices_IAS BLE IAS
 * @{
 * This section defines the database of Immediate Alert Service and manages the write event of Alert Level Characteristic from a peer device.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b IAS                        | Immediate Alert Service. This service exposes a control point to allow a peer device to cause the device to immediately alert. |
 *
 * @section ble_ias_api_usage How to use this module
 * - Step 1. Add IAS service to the application's GATT service database. And then return the bt_gatts_service_t pointer to GATTS stack by implementing the user-defined callback @ref bt_get_gatt_server(). 
 *  - Sample Code:
 *     @code
 *               // The GATTS collects all services.
 *               const bt_gatts_service_t * ble_gatt_server[] = {
 *                   &ble_gap_service,  // handle range: 0x0001 to 0x0009.
 *                   &ble_gatt_service, // handle range: 0x0011 to 0x0015.
 *                   &ble_ias_service,  // handle range: 0x0040 to 0x0042.
 *                   NULL
 *                };
 *                const bt_gatts_service_t** bt_get_gatt_server()
 *                { 
 *                    return ble_gatt_server;
 *                }

 *     @endcode
 * - Step 2. Implement the user-defined callback @ref ble_ias_alert_level_write_callback() to listen to the write event of the Alert Level Characteristic.
 *  - Sample Code:
 *     @code
 *                void ble_ias_alert_level_write_callback(uint16_t conn_handle, uint8_t alert_level)
 *                {
 *                    if (conn_handle) {
 *                        // Save the alert level.
 *                    }
 *                }
 *     @endcode
 */

#include "bt_type.h"
#include "bt_system.h"
#include "bt_gatt.h"
#include "bt_gatts.h"
#include "bt_gap_le.h"
#include "bt_platform.h"


BT_EXTERN_C_BEGIN
/* --------------Extern Service --------------------- */
extern const bt_gatts_service_t ble_ias_service; /*< Immediate Alert Service. */

/**
 * @brief   This function is a user-defined callback for the application to listen to the write event of Alert Level Characteristic.
 * @param[in] conn_handle     is the connection handle.
 * @param[in] alert_level     is the alert level value written by the remote device.
 * @return    void.
 */
void ble_ias_alert_level_write_callback(uint16_t conn_handle, uint8_t alert_level);

BT_EXTERN_C_END

/**
 * @}
 * @}
 */

#endif /*__BT_BATTERY_SERVICE_H__*/
