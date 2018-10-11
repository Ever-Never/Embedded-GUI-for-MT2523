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
#ifndef __BLE_DOGP_COMMON_H__
#define __BLE_DOGP_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "timers.h"
#include "bt_type.h"

typedef enum {
    BLE_GAP_ADV_INTERVAL_DEFAULT,
    BLE_GAP_ADV_INTERVAL_FAST_CONNECT,
    BLE_GAP_ADV_INTERVAL_LOW_POWER
} ble_gap_adv_interval_enum_t;
 
typedef enum {
    BLE_GAP_CONN_PARAM_PRIORITY_DEFAULT,
    BLE_GAP_CONN_PARAM_HIGH_SPEED_ANDROID, /**< please use this High speed enum, when remote device is android. */
    BLE_GAP_CONN_PARAM_HIGH_SPEED_IOS,     /**< please use this High speed enum, when remote device is IOS. */
    BLE_GAP_CONN_PARAM_LOW_POWER
} ble_gap_conn_param_priority_t;

typedef struct {
    uint16_t                        conn_handle;
    uint16_t                        conn_interval;
    ble_gap_conn_param_priority_t   conn_priority;
    ble_gap_adv_interval_enum_t     adv_interval;
    uint8_t                         remote_device_type;
} ble_dogp_adp_low_powe_cntx_t;

#if 0
void ble_dogp_adp_advertise_data_init(void);
#endif
bt_status_t bt_dogp_app_event_callback(bt_msg_type_t msg, void *buff);
bt_status_t ble_dogp_update_connection_interval(uint16_t conn_handle, ble_gap_conn_param_priority_t conn_priority);
bt_status_t bt_dogp_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff);
bt_status_t ble_dogp_event_callback_init(void);






#ifdef __cplusplus
}
#endif

#endif /*__BLE_DOGP_COMMON_H__*/



