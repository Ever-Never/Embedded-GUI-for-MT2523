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

#ifndef BT_SINK_APP_EVENT_H
#define BT_SINK_APP_EVENT_H

#include <stdbool.h>
#include <stdint.h>
#include "bt_sink_srv.h"
#include "bt_gap_le.h"
#include "bt_sink_srv_resource.h"

#if defined( __USB_MASS_STORAGE_ENABLE__) && defined(MTK_INEAR_NOODLES_HEADSET_ENABLE)
#define BT_SINK_APP_TEST_EXIT_MASS_STORAGE
#endif

#if defined(MTK_INEAR_NOODLES_HEADSET_ENABLE)
#define BT_SINK_APP_NO_ACTION_AUTO_POWER_OFF
#endif

#define  BT_SINK_EVENT_APP_BASE               (BT_SINK_SRV_EVENT_USER + 30 )
#define BT_SINK_EVENT_APP_EXT_COMMAND         (BT_SINK_EVENT_APP_BASE + 1)
#define BT_SINK_EVENT_APP_HCI_LOG_ON          (BT_SINK_EVENT_APP_BASE + 2)
#define BT_SINK_EVENT_APP_HCI_LOG_OFF         (BT_SINK_EVENT_APP_BASE + 3)
#define BT_SINK_EVENT_APP_SYS_LOG_ON          (BT_SINK_EVENT_APP_BASE + 4)
#define BT_SINK_EVENT_APP_SYS_LOG_OFF         (BT_SINK_EVENT_APP_BASE + 5)
#define BT_SINK_EVENT_APP_KEY_INPUT           (BT_SINK_EVENT_APP_BASE + 7)
#define BT_SINK_EVENT_APP_NOTIFICATION        (BT_SINK_EVENT_APP_BASE + 8)
#define BT_SINK_EVENT_APP_LED_NOTIFICATION    (BT_SINK_EVENT_APP_BASE + 9)
#define BT_SINK_EVENT_APP_BATTERY_NOTIFICATION    (BT_SINK_EVENT_APP_BASE + 10)

/*Local music*/
#define AUDIO_PLAYER_ACTION_PLAY            (BT_SINK_SRV_ACTION_USER_START)
#define AUDIO_PLAYER_ACTION_PAUSE           (BT_SINK_SRV_ACTION_USER_START + 1)
#define AUDIO_PLAYER_ACTION_NEXT_TRACK      (BT_SINK_SRV_ACTION_USER_START + 2)
#define AUDIO_PLAYER_ACTION_PRE_TRACK       (BT_SINK_SRV_ACTION_USER_START + 3)
#define AUDIO_PLAYER_ACTION_VOLUME_UP       (BT_SINK_SRV_ACTION_USER_START + 4)
#define AUDIO_PLAYER_ACTION_VOLUME_DOWN     (BT_SINK_SRV_ACTION_USER_START + 5)
#define AUDIO_PLAYER_ACTION_VOLUME_MAX      (BT_SINK_SRV_ACTION_USER_START + 6)
#define AUDIO_PLAYER_ACTION_VOLUME_MIN      (BT_SINK_SRV_ACTION_USER_START + 7)

#define AUDIO_PLAYER_ACTION_PAUSE_WAV       (BT_SINK_SRV_ACTION_USER_START + 8)
#define AUDIO_PLAYER_ACTION_RESUME_WAV      (BT_SINK_SRV_ACTION_USER_START + 9)

#ifdef BT_SINK_APP_TEST_EXIT_MASS_STORAGE
#define BT_SINK_APP_EXIT_MASS_STORGE_MODE      (BT_SINK_SRV_ACTION_USER_START + 20)
#endif

#define BT_SINK_APP_ACTION_PLAY_BY_MODE       (BT_SINK_SRV_ACTION_USER_START + 21)

typedef struct {
    bt_sink_srv_key_value_t key_value;
    bt_sink_srv_key_action_t key_action;
} bt_sink_app_ext_cmd_t;

typedef struct {
    int32_t charger_exist;
    uint8_t capacity;
} bt_sink_app_battery_info_t;

typedef bt_sink_srv_status_t (*bt_sink_app_event_callback_t)(bt_sink_srv_event_t event_id, void *parameters);
typedef void (*bt_sink_app_event_post_result_t)(bt_sink_srv_event_t event_id, bt_sink_srv_status_t result, void *parameters);

typedef struct sink_event_node_t {
    struct sink_event_node_t *previous;
    struct sink_event_node_t *next;
} bt_sink_app_event_node_t;

typedef struct {
    bt_sink_srv_event_t event_id;
    bt_sink_app_event_callback_t callback;
} bt_sink_app_event_callback_table_t;

typedef struct {
    bt_sink_srv_event_t event_id;
    void *parameters;
    bt_sink_app_event_post_result_t post_callback;
} bt_sink_app_event_t;

typedef struct {
    bt_sink_app_event_node_t pointer;
    bt_sink_srv_event_t event_id;
    bt_sink_app_event_callback_t callback;
    bool dirty;
} bt_sink_app_event_callback_node_t;

void bt_sink_app_event_init(void);
void bt_sink_app_event_post(bt_sink_srv_event_t event_id, void *parameters, bt_sink_app_event_post_result_t callback);
void bt_sink_app_event_register_callback(bt_sink_srv_event_t event_id, bt_sink_app_event_callback_t callback);
void bt_sink_app_event_deregister_callback(bt_sink_srv_event_t event_id, bt_sink_app_event_callback_t callback);
void bt_sink_app_event_process(bt_sink_app_event_t *event);
bt_sink_srv_status_t bt_sink_app_event_handler(bt_sink_srv_event_t event_id, void *parameters);
void bt_sink_app_event_post_callback(bt_sink_srv_event_t event_id, bt_sink_srv_status_t result, void *parameters);
bt_gap_le_local_config_req_ind_t *bt_sink_app_get_le_local_configuration(void);
bt_gap_le_bonding_info_t *bt_sink_app_gap_le_get_bonding_info(const bt_addr_t remote_addr);
bt_status_t bt_sink_app_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind);
#endif /* BT_SINK_APP_EVENT_H */

