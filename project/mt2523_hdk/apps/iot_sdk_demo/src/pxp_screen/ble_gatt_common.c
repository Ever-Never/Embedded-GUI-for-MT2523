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

#include "bt_gatts.h"
#include "ble_gatt_common.h"
#include "ble_gatt_utils.h"
#include "ble_gatt_connect.h"
#include "ble_hogp.h"

//server collects all service
const bt_gatts_service_t *bt_if_gatt_server[] = {
    &bt_if_gap_service,
    &bt_if_gatt_service,
    &bt_if_hogp_device_info_service,
    &bt_if_hogp_battery_service,
    &bt_if_hogp_hid_service,
    &bt_if_lls_service,
    NULL
};


const bt_gatts_service_t **bt_get_gatt_server()
{
    return bt_if_gatt_server;
}


/**************
bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status,
    void *buff)
{
    return ble_app_event_handle(msg, status, buff);
}
**********/


bt_status_t ble_app_event_handle(bt_msg_type_t msg, bt_status_t status,
    void *buff)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    int32_t err_1st = BLE_GATT_ERR_FAIL_1ST;
    uint32_t module = msg & 0xFF000000;

    if ((BT_MODULE_GAP == module) ||
        (BT_MODULE_GATT == module) ||
        (BT_MODULE_SYSTEM == module)) {
    ble_gatt_report("[gatt][common]ent_hdr(s)--msg: 0x%x, status: %d, buff: 0x%x",
        msg, status, buff);
    }

    if (BT_MODULE_GAP == module) {
        ret = ble_gap_event_handle(msg, status, buff);
    } else if (BT_MODULE_GATT == module) {
        ret = ble_gatt_event_handle(msg, status, buff);
    } else if (BT_MODULE_SYSTEM == module) {
        ret = ble_gap_event_handle(msg, status, buff);
    }

    if ((BT_MODULE_GAP == module) ||
        (BT_MODULE_GATT == module) ||
        (BT_MODULE_SYSTEM == module)) {
    ble_gatt_report("[gatt][common]ent_hdr(e)--ret: %d, err_1st: %d\n", ret,
        err_1st);
    }

    return ret;
}


/**************
bt_gap_le_local_config_req_ind_t *bt_gap_le_get_local_config(void)
{
    return ble_gatt_connect_gap_le_get_local_config();
}


bt_gap_le_bonding_info_t *bt_gap_le_get_bonding_info(const bt_addr_t remote_addr)
{
    return ble_gatt_connect_gap_le_get_bonding_info(remote_addr);
}


bt_status_t bt_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind)
{
    return bt_gatt_connect_gap_le_get_pairing_config(ind);
}
**************/


bt_status_t ble_gatt_event_handle(bt_msg_type_t msg, bt_status_t status,
    void *buff)
{
    bt_status_t ret = BT_STATUS_SUCCESS;

    switch (msg) {
        case BT_GATTC_DISCOVER_PRIMARY_SERVICE_BY_UUID: {
            break;
        }
        case BT_GATTC_FIND_INCLUDED_SERVICES: {
            break;
        }
        case BT_GATTC_DISCOVER_CHARC: {
            break;
        }
        case BT_GATTC_DISCOVER_CHARC_DESCRIPTOR: {
            break;
        }
        case BT_GATTC_WRITE_CHARC: {
            break;
        }
        case BT_GATTC_CHARC_VALUE_NOTIFICATION: {
            break;
        }

        default:
            break;
    }

    ble_gatt_report("[gatt][common]ent_hdr(e)--ret: %d\n", ret);
    return ret;
}

