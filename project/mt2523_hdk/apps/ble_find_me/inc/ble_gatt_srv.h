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

#ifndef __BLE_GATT_SRV_H__
#define __BLE_GATT_SRV_H__

#include "stdint.h"
#include "bt_gap_le.h"

typedef int32_t ble_gatt_srv_status_t;

/* BLE APP event base */
#define BLE_GATT_SRV_BASE                           (0x00)

#define BLE_GATT_SRV_IAS_BASE                       (0x1000)
#define BLE_GATT_SRV_LLS_BASE                       (0x1100)


typedef enum {
    /* GAP event */
    BLE_GATT_SRV_LE_CONNECT = BLE_GATT_SRV_BASE,
    BLE_GATT_SRV_LE_DISCONNECT,
    /* IAS event */
    BLE_GATT_SRV_IAS_START = BLE_GATT_SRV_IAS_BASE,
    BLE_GATT_SRV_IAS_AL_READ,
    BLE_GATT_SRV_IAS_AL_WRITE,
    /* LLS event */
    BLE_GATT_SRV_LLS_START = BLE_GATT_SRV_LLS_BASE,
    BLE_GATT_SRV_LLS_AL_READ,
    BLE_GATT_SRV_LLS_AL_WRITE,

    BLE_GATT_SRV_END
} ble_gatt_srv_event_t;


#define BLE_GATT_SRV_STATUS_OK                      (0)
#define BLE_GATT_SRV_STATUS_FAIL                    (-1)
#define BLE_GATT_SRV_STATUS_REGISTERED              (-2)
#define BLE_GATT_SRV_STATUS_EXHAUST                 (-3)
#define BLE_GATT_SRV_STATUS_PARAM                   (-4)


typedef ble_gatt_srv_status_t (*ble_gatt_srv_callback_t)(ble_gatt_srv_event_t event, void *para, void *user_data);

typedef struct {
    bt_handle_t handle;
} ble_gatt_srv_connect_t;

typedef struct {
    bt_handle_t handle;
} ble_gatt_srv_disconnect_t;

typedef struct {
    bt_handle_t handle;
    uint8_t al;
} ble_gatt_char_alter_level_t;

ble_gatt_srv_status_t ble_gatt_srv_init(void);

ble_gatt_srv_status_t ble_gatt_srv_register(ble_gatt_srv_callback_t cb_func,
    void *user_data);

ble_gatt_srv_status_t ble_gatt_srv_deregister(ble_gatt_srv_callback_t cb_func,
    void *user_data);

#endif /* __BLE_GATT_SRV_H__ */

