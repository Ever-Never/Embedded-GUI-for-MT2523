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

#include "bt_type.h"
#include "bt_gattc_discovery.h"


/**
 *  @brief Define ancs event type.
 */
#define BLE_ANCS_EVENT_CONNECTED                0
#define BLE_ANCS_EVENT_IOS_NOTIFICATION         1
#define BLE_ANCS_EVENT_NOTIFICATION_ATTRIBUTE   2
#define BLE_ANCS_EVENT_APP_ATTRIBUTE            3
#define BLE_ANCS_EVENT_REQUEST_COMPLETED        4
#define BLE_ANCS_EVENT_DISCONNECTED             5
typedef uint8_t ble_ancs_event_type_t;


/**
 *  @brief Define bt_ancs_common status.
 */
#define ANCS_COMM_STATUS_READY              0
#define ANCS_COMM_STATUS_READ_NOTIF_SOURCE  1
#define ANCS_COMM_STATUS_READ_DATA_SOURCE   2
typedef uint8_t ble_ancs_comm_status_t;

typedef struct {
    ble_ancs_event_type_t evt_type;
    bt_handle_t connection_handle;
    bt_status_t result;
    union {
        ble_ancs_event_notification_t notification;
        ble_ancs_event_attribute_t attribute;
    } data;
} ble_ancs_event_t;
 
typedef void(* ble_ancs_event_handler_t)(ble_ancs_event_t *p_evt);

typedef struct {
    ble_ancs_comm_status_t status;
    ble_ancs_event_handler_t evt_handler;
    bt_gattc_discovery_service_t service;
    bt_gattc_discovery_characteristic_t charc[BLE_ANCS_MAX_CHARC_NUMBER];
    bt_gattc_discovery_descriptor_t descrp[BLE_ANCS_MAX_CHARC_NUMBER];
} ble_ancs_srv_content_t;


void ble_ancs_start(ble_ancs_event_handler_t evt_handler);


