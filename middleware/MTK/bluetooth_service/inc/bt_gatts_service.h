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


#ifndef __BT_GATTS_SERVICE_H__
#define __BT_GATTS_SERVICE_H__

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothServices_GATT GATTS Service
 * @{
 * This section provides APIs to set GAP service and characteristics information, such as device name, appearance, peripheral preferred connection parameters and central address resolution. 
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b GAP                        | Generic Access Profile. This profile defines the generic procedures related to discovery of Bluetooth enabled devices and link management aspects of connecting to the Bluetooth enabled devices. It also defines procedures related to the use of different security levels. |
 * |\b GATTS                      | GATT Server, is the device that accepts incoming commands and requests from the GATT client. |
 * |\b PPCP                       | Peripheral Preferred Connection Parameters, contains the preferred connection parameters of Peripheral.|
 *
 * @section gatts_service_api_usage How to use this module
 * - Step 1. Initialize the GATTS service.
 *  - Sample Code:
 *     @code
 *                bt_gatts_service_init();
 *     @endcode
 * - Step 2. Use GATTS Service APIs to set device name, appearance, peripheral preferred connection parameters and central address resolution characteristic of GAP service. 
 *  - Sample code:
 *     @code 
 *                uint8_t car_flag = 1;
 *                uint16_t appearance = 0x1234;
 *                char* local_device_name = "HB_DEVICE_LE";
 *                bt_gatts_le_gap_ppcp_t   gap_conn_param;
 *                gap_conn_param.min_conn_interval = 0x0010;
 *                gap_conn_param.max_conn_interval = 0x0020;
 *                gap_conn_param.slave_latency = 0x00;
 *                gap_conn_param.supervision_timeout = 0x0258;
 *                bt_gatts_service_set_gap_device_name((const uint8_t *)local_device_name, strlen(local_device_name));
 *                bt_gatts_service_set_gap_appearance(appearance);
 *                bt_gatts_service_set_gap_ppcp(&gap_conn_param);
 *                bt_gatts_service_set_gap_central_address_resolution(car_flag);
 *     @endcode
 */


#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "bt_type.h"

BT_EXTERN_C_BEGIN

/**
 * @defgroup GATTS_Service_struct Struct
 * @{
 * This section defines structures for the GATTS service.
 */

/**
 *  @brief The Peripheral Preferred Connection Parameters structure of the GAP service.
 */
typedef struct
{
  uint16_t min_conn_interval;         /**< Minimum Connection Interval in 1.25 ms units, value range: 0x0006 to 0x0C80.*/
  uint16_t max_conn_interval;         /**< Maximum Connection Interval in 1.25 ms units, value range: 0x0006 to 0x0C80.*/
  uint16_t slave_latency;             /**< Slave Latency in number of connection events, value range: 0x0000 to 0x03E8.*/
  uint16_t supervision_timeout;       /**< Connection Supervision Timeout in 10 ms units, value range: 0x000A to 0x0C80.*/
} bt_gatts_le_gap_ppcp_t;

/**
 * @}
 */
/* --------------Extern Service --------------------- */
extern const bt_gatts_service_t bt_if_gatt_service; /*< GATT Service. */
extern const bt_gatts_service_t bt_if_gap_service;  /*< GAP Service. */
 
/**
 * @brief   This function initializes the GATTS service. It is recommended to call this API once during the bootup.
 * @param[in] void.
 * @return    void.          
 */
void bt_gatts_service_init(void);

/**
 * @brief   This function sets Device Name Characteristic. It should be called after @ref bt_gatts_service_init().
 * @param[in] device_name         is a pointer to a UTF-8 encoded string.
 * @param[in] length              is the length of a UTF-8 string pointed to by @ref device_name in octets (shall be 0 to 248 octets).
 * @return                        BT_STATUS_SUCCESS, the device name is set successfully.
 *                                BT_STATUS_FAIL, the device name failed to set, a NULL pointer or invalid length is supplied.
 */
bt_status_t bt_gatts_service_set_gap_device_name(const uint8_t *device_name, uint16_t length);

/**
 * @brief   This function gets the current Device Name Characteristic value.
 * @param[in] void.
 * @return    device name         is a pointer to a UTF-8 encoded string.
 */
const uint8_t *bt_gatts_service_get_gap_device_name(void);

/**
 * @brief   This function sets Appearance Characteristic. It should be called after @ref bt_gatts_service_init().
 * @param[in] appearance          is the Appearance Characteristic value, it shall be 2 octets in length.
 * @return    void.
 */
void bt_gatts_service_set_gap_appearance(uint16_t appearance);

/**
 * @brief   This function sets Peripheral Preferred Connection Parameters Characteristic. It should be called after @bt_gatts_service_init().
 *          This function should be called by Peripheral role only to set the Peripheral Preferred Connection Parameters.
 * @param[in] ppcp_param          is a pointer to the @ref bt_gatts_le_gap_ppcp_t structure with the desired parameter.
 * @return                        BT_STATUS_SUCCESS, the PPCP value is set successfully.
 *                                BT_STATUS_FAIL, the PPCP value failed to set, an invalid parameter is supplied.
 */
bt_status_t bt_gatts_service_set_gap_ppcp(bt_gatts_le_gap_ppcp_t *ppcp_param);

/**
 * @brief   This function sets Central Address Resolution Characteristic. It should be called after @ref bt_gatts_service_init().
 *          This function should be called by Central role only to define whether the device supports privacy with the address resolution.
 * @param[in] flag                is a 1 octet value, 0 means the address resolution is not supported in this device, 1 means the address solution is supported in this device, the reserved value range is from 2 to 255.
 * @return                        BT_STATUS_SUCCESS, the resolution is set successfully.
 *                                BT_STATUS_FAIL, the resolution failed to set, an invalid parameter is supplied.
 */
bt_status_t bt_gatts_service_set_gap_central_address_resolution(uint8_t flag);


BT_EXTERN_C_END

/**
 * @}
 * @}
 */

#endif /*__BT_GATTS_SERVICE_H__*/


