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

#ifndef BT_SINK_APP_MAIN_H
#define BT_SINK_APP_MAIN_H

#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "bt_sink_srv.h"
#include "bt_sink_app_event.h"

#ifdef MTK_LED_ENABLE
#include "bt_sink_app_led.h"
#endif
#ifdef MTK_PROMPT_SOUND_ENABLE
#include "bt_sink_app_voice_prompt.h"
#endif /*MTK_PROMPT_SOUND_ENABLE*/

#define BT_SINK_APP_TASK_PRIORITY   (1)
#define BT_SINK_APP_TASK_STACK_SIZE (1024)
#define BT_SINK_APP_QUEUE_SIZE      (30)

#define  BT_SINK_APP_LOW_BATTERY_ALARM 5
#define  BT_SINK_APP_LOW_BATTERY_POWER_OFF 2

#define BT_SINK_APP_MAX_CONNECTED_DEVICE_NUM      2

/**
    *  @brief Define for the device role.
*/
#define BT_SINK_APP_DEVICE_MASTER  (0x00)
#define BT_SINK_APP_DEVICE_SLAVE       (0x01)
typedef uint8_t bt_sink_app_device_role_t;

typedef struct {
    bt_bd_addr_t addr;
} bt_sink_app_connected_device_t;

typedef struct {
    QueueHandle_t            queue_handle;
    bt_sink_srv_state_t      state;
    bt_sink_app_event_node_t dynamic_callback_header;
    bt_sink_srv_event_t      invoking;
    bt_sink_app_device_role_t device_role;
    uint8_t battery_level;
    bt_sink_srv_features_config_t feature_config;
    bt_sink_app_connected_device_t remote[BT_SINK_APP_MAX_CONNECTED_DEVICE_NUM];
    bool notify_connection_state;
    //bool connected;
} bt_sink_app_context_t;

extern bt_sink_app_context_t bt_sink_app_context;

#define bt_sink_app_report(_message,...) LOG_I(sink_app, (_message), ##__VA_ARGS__)
#define bt_sink_app_assert configASSERT

void bt_sink_app_task_create(void);
void bt_sink_app_task_main(void *arg);
bt_sink_app_device_role_t bt_sink_app_get_device_role(void);
void bt_sink_app_set_device_role(bt_sink_app_device_role_t role);
void bt_sink_app_key_action_handler(bt_sink_srv_key_value_t key_value, bt_sink_srv_key_action_t key_action);
void bt_sink_app_reset_hf_battery(void);
void bt_sink_app_battery_level_handler(int32_t charger_exist, uint8_t capacity);
#ifdef BT_SINK_APP_NO_ACTION_AUTO_POWER_OFF
void bt_sink_app_auto_power_off_by_state(bt_sink_srv_state_t current);
#endif
bool bt_sink_app_get_connected_device(bt_bd_addr_t *address) ;

#endif /* BT_SINK_APP_MAIN_H */

