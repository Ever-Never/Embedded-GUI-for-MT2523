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

#ifndef __BLE_GATT_CONNECT_H__
#define __BLE_GATT_CONNECT_H__

#include "bt_type.h"
#include "stdint.h"
#include "ble_gatt_common.h"


#define BLE_GATT_CONNECT_FLAG_INIT                 (0x0001)

/* device info flag */
#define BLE_GATT_CONNECT_INFO_FLAG_USED            (0x01)

#define BT_ADDR_TYPE_UNKNOW 0xFF

#define BLE_GATT_DEV_NAME                          "MediaTek_PXP"

typedef enum {
    BLE_GATT_CONNECT_INFO_HANDLE,   /* 0 */
    BLE_GATT_CONNECT_INFO_ADDR,   /* 1 */
    BLE_GATT_CONNECT_INFO_UNUSED, /* 2 */

    BLE_GATT_CONNECT_INFO_TOTAL
} ble_gatt_connect_info_type_t;

typedef struct {
    uint8_t flag;
    bt_handle_t       handle;
    bt_role_t         role;
    bt_addr_t         peer_addr;
    uint8_t           gatts_wait_att_rx_opcode; //use to wait handle value confirmation.
} ble_gatt_connect_info_t;

typedef struct {
    bt_addr_t bt_addr;
    bt_gap_le_bonding_info_t info;
} ble_gatt_bonded_info_t;


typedef struct {
    uint32_t flag;
    ble_gatt_connect_info_t *conn_info;
} ble_gatt_connect_context_t;


bt_gap_le_local_config_req_ind_t *ble_gatt_connect_gap_le_get_local_config(void);

bt_gap_le_bonding_info_t *ble_gatt_connect_gap_le_get_bonding_info(const bt_addr_t remote_addr);

bt_status_t bt_gatt_connect_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind);

bt_status_t ble_gap_event_handle(bt_msg_type_t msg, bt_status_t status,
    void *buff);

/* Extern API */
void ble_gatt_connect_init(void);

ble_gatt_connect_context_t *ble_gatt_connect_get_context(void);

#endif /* __BLE_GATT_CONNECT_H__ */

