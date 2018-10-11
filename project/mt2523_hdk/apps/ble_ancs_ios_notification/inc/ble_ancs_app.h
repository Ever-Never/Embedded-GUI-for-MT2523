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

#include "stdint.h"
#include "stdbool.h"
#include "ble_ancs.h"
#include "FreeRTOS.h"
#include "timers.h"

#define BLE_ANCS_APP_ID_LEN         50
#define BLE_ANCS_TITLE_LEN          40
#define BLE_ANCS_SUBTITLE_LEN       30
#define BLE_ANCS_MESSAGE_LEN        300
#define BLE_ANCS_MESSAGE_SIZE_LEN   4
#define BLE_ANCS_DATE_LEN           30
#define BLE_ANCS_APP_NAME_LEN       30

/*The max number of notificaiton can be queued, if more than 10, previous notifications will be droped*/
#define BLE_ANCS_MAX_NOTIF_NUM      10
#define BLE_ANCS_LAB_LEN            20
#define BLE_ANCS_TIMER_PERIOD       15000



/**
 *  @brief Define for ble ancs status type.
 */
#define ANCS_APP_STATUS_NONE                    0
#define ANCS_APP_STATUS_ENABLE_DATA_SOURCE      1
#define ANCS_APP_STATUS_ENABLE_NOTIF_SOURCE     2
#define ANCS_APP_STATUS_DISABLE_DATA_SOURCE     3
#define ANCS_APP_STATUS_DISABLE_NOTIF_SOURCE    4
#define ANCS_APP_STATUS_GET_NOTIF_ATTRIBUTE     5
#define ANCS_APP_STATUS_GET_APP_ATTRIBUTE       6
#define ANCS_APP_STATUS_PERFORM_ACTION          7
typedef uint8_t ble_ancs_app_status_t; 


typedef struct {
    uint8_t notif_uid[4];
    bt_handle_t connection_handle;
} ble_ancs_notification_info_t;

typedef struct _ble_ancs_notification_node_t {
    ble_ancs_notification_info_t notif;
    struct _ble_ancs_notification_node_t *next;
} ble_ancs_notification_node_t;

typedef struct {
    bt_handle_t connection_handle;
    TimerHandle_t ancs_timer;
    ble_ancs_app_status_t status;

    uint8_t appid[BLE_ANCS_APP_ID_LEN + 1];     /*String must be NULL_terminated, add 1 byte for '\0'*/
    uint8_t title[BLE_ANCS_TITLE_LEN + 1];
    uint8_t subtitle[BLE_ANCS_SUBTITLE_LEN + 1];
    uint8_t message[BLE_ANCS_MESSAGE_LEN + 1];
    uint8_t messagesize[BLE_ANCS_MESSAGE_SIZE_LEN + 1];
    uint8_t date[BLE_ANCS_DATE_LEN + 1];
    /*posi_lab and nega_lab only supported by IOS8.0 or later*/
    uint8_t posi_lab[BLE_ANCS_LAB_LEN + 1];     /*positive label*/
    uint8_t nega_lab[BLE_ANCS_LAB_LEN + 1];
    uint8_t app_name[BLE_ANCS_APP_NAME_LEN + 1];

    uint8_t notif_num;
    ble_ancs_notification_node_t notif_list;
    ble_ancs_notification_node_t *curr_notif;   /*the notification in getting attributes*/
    ble_ancs_notification_node_t *new_notif;    /*The latest received notification*/
} ble_ancs_app_cntx_t;


void ble_ancs_task_init(void);

void ble_ancs_perform_action(uint8_t *cmd);


