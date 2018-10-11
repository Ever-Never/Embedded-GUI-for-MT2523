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

#include <string.h>
#include "bt_gattc.h"
#include "bt_gatt.h"
#include "bt_gatts.h"
#include "ble_ias.h"
#include "ble_bas.h"
#include "bt_gatts_service.h"

//Declare every record here
//service collects all bt_gatts_service_rec_t
//IMPORTAMT: handle:0x0000 is reserved, please start your handle from 0x0001
//GAP 0x0001

#ifdef MTK_BLE_SMTCN_ENABLE
extern const bt_gatts_service_t bt_if_dtp_service;
#endif
//extern const bt_gatts_service_t bt_if_gatt_service;
//extern const bt_gatts_service_t bt_if_gap_service;
extern const bt_gatts_service_t bt_if_dogp_service;
//extern const bt_gatts_service_t ble_bas_service;
//extern const bt_gatts_service_t ble_ias_service;

/**< gatt server collects all service. */
const bt_gatts_service_t * bt_if_clm_gatt_server[] = {
    &bt_if_gap_service, /**< handle range: 0x0001 to 0x0009. */
    &bt_if_gatt_service,/**< handle range: 0x0011 to 0x0015. */
#ifdef MTK_BLE_SMTCN_ENABLE
    &bt_if_dtp_service, //0x0014-0x0017
#endif
    &bt_if_dogp_service,/**< handle range: 0x0020 to 0x0025. */
    &ble_bas_service,/**< handle range: 0x0031 to 0x0034. */
    &ble_ias_service,/**< handle range: 0x0040 to 0x0042. */
    NULL
    };


/**< When GATTS get req from remote client, GATTS will call bt_get_gatt_server() to get application's gatt service DB. */
/**< You have to return the DB(bt_gatts_service_t pointer) to gatts stack. */
const bt_gatts_service_t** bt_get_gatt_server()
{ 
    return bt_if_clm_gatt_server;
}

