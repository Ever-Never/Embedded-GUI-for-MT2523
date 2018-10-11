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

#include "FreeRTOS.h"
#include "task.h"
#include "bt_sink_srv.h"
#include "bt_sink_app_event.h"
#include "bt_sink_app_main.h"
#include "bt_sink_app_cmd.h"
//#include "bt_a2dp.h"
#include "bt_gap_le.h"
//#include "bt_gap.h"
#include "bt_init.h"
#include "bt_notify.h"
#ifdef MTK_PROMPT_SOUND_ENABLE
#include "voice_prompt_mix.h"
#endif

static void bt_sink_app_event_node_init(bt_sink_app_event_node_t *event_node)
{
    event_node->previous = event_node;
    event_node->next = event_node;
}

static void bt_sink_app_event_node_insert(bt_sink_app_event_node_t *head, bt_sink_app_event_node_t *node)
{
    node->next = head;
    node->previous = head->previous;
    head->previous->next = node;
    head->previous = node;
}

static void bt_sink_app_event_node_remove(bt_sink_app_event_node_t *node)
{
    node->previous->next = node->next;
    node->next->previous = node->previous;
}

static bt_sink_app_event_callback_node_t *bt_sink_app_event_node_find_callback(bt_sink_app_event_node_t *head,
        bt_sink_srv_event_t event_id,
        bt_sink_app_event_callback_t callback)
{
    bt_sink_app_event_node_t *current_node = head->next;
    bt_sink_app_event_callback_node_t *result = NULL;

    while (current_node != head) {
        if (((bt_sink_app_event_callback_node_t *)current_node)->event_id == event_id
                && ((bt_sink_app_event_callback_node_t *)current_node)->callback == callback) {
            result = (bt_sink_app_event_callback_node_t *)current_node;
            break;
        }
        current_node = current_node->next;
    }
    return result;
}

void bt_sink_app_event_init(void)
{
    bt_sink_app_context.invoking = BT_SINK_SRV_EVENT_ALL;
    bt_sink_app_event_node_init(&bt_sink_app_context.dynamic_callback_header);
}


void bt_sink_app_event_post(bt_sink_srv_event_t event_id, void *parameters, bt_sink_app_event_post_result_t callback)
{
    bt_sink_app_event_t event;

    bt_sink_app_report("[Sink] bt_sink_app_event_post, event:%x", event_id);

    memset(&event, 0, sizeof(bt_sink_app_event_t));
    event.event_id = event_id;
    event.parameters = parameters;
    event.post_callback = callback;
    
    if (bt_sink_app_context.queue_handle == NULL) {
        bt_sink_app_report("[Sink] queue is not ready.");
        return;
    }
    
    if (pdPASS != xQueueSend(bt_sink_app_context.queue_handle, &event, 0)) {
        if (NULL != callback) {
            callback(event_id, BT_SINK_SRV_STATUS_FAIL, parameters);
        }
        bt_sink_app_report("[Sink][Fatal Error] event lost:0x%x", event_id);
    }
}

void bt_sink_app_event_register_callback(bt_sink_srv_event_t event_id, bt_sink_app_event_callback_t callback)
{
    bt_sink_app_event_callback_node_t *callback_node =
        bt_sink_app_event_node_find_callback(&bt_sink_app_context.dynamic_callback_header, event_id, callback);

    if (NULL == callback_node) {
        callback_node = (bt_sink_app_event_callback_node_t *)pvPortMalloc(sizeof(*callback_node));

        if (NULL != callback_node) {
            memset(callback_node, 0, sizeof(bt_sink_app_event_callback_node_t));

            callback_node->event_id = event_id;
            callback_node->callback = callback;
            bt_sink_app_event_node_insert(&bt_sink_app_context.dynamic_callback_header, &callback_node->pointer);
        }
    } else {
        callback_node->dirty = false;
    }
}

void bt_sink_app_event_deregister_callback(bt_sink_srv_event_t event_id, bt_sink_app_event_callback_t callback)
{
    bt_sink_app_event_callback_node_t *callback_node =
        bt_sink_app_event_node_find_callback(&bt_sink_app_context.dynamic_callback_header, event_id, callback);
    if (NULL != callback_node) {
        if ((event_id == BT_SINK_SRV_EVENT_ALL && BT_SINK_SRV_EVENT_ALL != bt_sink_app_context.invoking)
                || event_id == bt_sink_app_context.invoking) {
            callback_node->dirty = true;
        } else {
            bt_sink_app_event_node_remove(&callback_node->pointer);
            vPortFree((void *)callback_node);
        }
    }
}

static bt_sink_srv_status_t bt_sink_app_event_invoke(bt_sink_srv_event_t event, void *parameters)
{
    bt_sink_srv_status_t result = BT_SINK_SRV_STATUS_SUCCESS;
    bt_sink_app_event_node_t *dynamic_callback = &bt_sink_app_context.dynamic_callback_header;

    bt_sink_app_context.invoking = event;
    while ((dynamic_callback = dynamic_callback->next) != &bt_sink_app_context.dynamic_callback_header) {
        if (BT_SINK_SRV_EVENT_ALL == ((bt_sink_app_event_callback_node_t *)dynamic_callback)->event_id) {
            result = ((bt_sink_app_event_callback_node_t *)dynamic_callback)->callback(event, parameters);
            if (BT_SINK_SRV_STATUS_EVENT_STOP == result) {
                // TRACE
                break;
            }
        } else if (event == ((bt_sink_app_event_callback_node_t *)dynamic_callback)->event_id) {
            result = ((bt_sink_app_event_callback_node_t *)dynamic_callback)->callback(event, parameters);
            if (BT_SINK_SRV_STATUS_EVENT_STOP == result) {
                // TRACE
                break;
            }
        } else {
            // TRACE
        }
    }
    bt_sink_app_context.invoking = BT_SINK_SRV_EVENT_ALL;

    dynamic_callback = bt_sink_app_context.dynamic_callback_header.next;

    while (dynamic_callback != &bt_sink_app_context.dynamic_callback_header) {
        if (((bt_sink_app_event_callback_node_t *)dynamic_callback)->dirty) {
            bt_sink_app_event_node_t *dirty_node = dynamic_callback;

            dynamic_callback = dynamic_callback->next;
            bt_sink_app_event_node_remove(dirty_node);
            vPortFree((void *)dirty_node);
            continue;
        }
        dynamic_callback = dynamic_callback->next;
    }
    return result;
}

void bt_sink_app_event_process(bt_sink_app_event_t *event)
{
    bt_sink_srv_status_t result;

    if (NULL != event) {
        bt_sink_app_report("[Sink] bt_sink_app_event_process:0x%x" , event->event_id);

        result = bt_sink_app_event_invoke(event->event_id, event->parameters);

        if (event->post_callback) {
            event->post_callback(event->event_id, result, event->parameters);
        }
    }
}

void bt_sink_app_event_post_callback(bt_sink_srv_event_t event_id, bt_sink_srv_status_t result, void *parameters)
{
    bt_sink_app_report("[Sink] free event:0x%x params:0x%x", event_id, parameters);

    if (NULL != parameters) {
        vPortFree(parameters);
        parameters = NULL;
    }
}

static void bt_sink_app_notification_handler(void *parameters)
{
    uint32_t event = (uint32_t)parameters;

    switch (event) {
        case BT_NOTIFY_EVENT_NOTIFICATION:
            bt_sink_app_atci_indicate(BT_SINK_APP_INT_TYPE_NOTI_NEW, 0);
            break;

        case BT_NOTIFY_EVENT_MISSED_CALL:
            bt_sink_app_atci_indicate(BT_SINK_APP_IND_TYPE_NOTI_MISSED_CALL, 0);
            break;

        case BT_NOTIFY_EVENT_SMS:
            bt_sink_app_atci_indicate(BT_SINK_APP_INT_TYPE_NOTI_SMS, 0);
            break;

        default:
            break;
    }
}


bool bt_sink_app_get_connected_device(bt_bd_addr_t *address) 
 {
   bool ret = false;
   static const bt_bd_addr_t empty_address = {0};

    bt_sink_app_report("[Sink][CM] address:0x%x",address);
   if ((address == NULL) || (memcmp(address, empty_address, sizeof(*address)) == 0)) {
        return ret;
   }

    bt_sink_app_report("[Sink][CM] address:%02x:%02x:%02x:%02x:%02x:%02x",
                           (*address)[5], (*address)[4], (*address)[3],(*address)[2], (*address)[1], (*address)[0]);
    
    for (uint8_t i = 0; i < BT_SINK_APP_MAX_CONNECTED_DEVICE_NUM; ++i) {
        if(0 == (memcmp((void *)&bt_sink_app_context.remote[i].addr, (void *)address, sizeof(*address)))) {
            bt_sink_app_report("[Sink][CM] index:0x%d",i);
            ret = true;
            break;
      }
    }
    bt_sink_app_report("[Sink] [CM]get conn device:%d", ret);

    return ret;
}

static void bt_sink_app_set_connected_device(bt_bd_addr_t *address) 
{
    static const bt_bd_addr_t empty_address = {0};
    bt_sink_app_report("[Sink][CM] address:0x%x",address);
    if ((address == NULL) || (memcmp((void *)address, (void *)empty_address, sizeof(*address)) == 0)) {
        return ;
    }

    for (uint8_t i = 0; i < BT_SINK_APP_MAX_CONNECTED_DEVICE_NUM; ++i) {
        if (memcmp((void *)&(bt_sink_app_context.remote[i].addr), (void *)empty_address, sizeof(*address)) == 0) {
            memcpy((void *)&(bt_sink_app_context.remote[i].addr),  (void *)address, sizeof(*address));
            bt_sink_app_report("[Sink][CM] index:0x%d",i);
            break;
        }
    }

    return;
}

static void bt_sink_app_reset_connected_device(bt_bd_addr_t *address) 
{
    static const bt_bd_addr_t empty_address = {0};
     bt_sink_app_report("[Sink][CM] address:0x%x",address);
    if ((address == NULL) || (memcmp((void *)address, (void *)empty_address, sizeof(*address)) == 0)) {
        return ;
    }
    bt_sink_app_report("[Sink][CM] address:%02x:%02x:%02x:%02x:%02x:%02x",
                           (*address)[5],  (*address)[4],  (*address)[3], (*address)[2],  (*address)[1],  (*address)[0]);

     for (uint8_t i = 0; i < BT_SINK_APP_MAX_CONNECTED_DEVICE_NUM; ++i) {
            if (memcmp((void *)&(bt_sink_app_context.remote[i].addr), (void *)address, sizeof(*address)) == 0) {
                memcpy((void *)&(bt_sink_app_context.remote[i].addr),  (void *)empty_address, sizeof(*address));
                bt_sink_app_report("[Sink][CM] index:0x%d",i);
                break;
            }
        }

    return;
}

bt_sink_srv_status_t bt_sink_app_event_handler(bt_sink_srv_event_t event_id, void *parameters)
{
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)parameters;

    bt_sink_app_report("[Sink] event:0x%x", event_id);

    switch (event_id) {
        case BT_SINK_SRV_EVENT_STATE_CHANGE:
            bt_sink_app_report("[Sink] state change, previous:0x%x, now:0x%x", event->state_change.previous, event->state_change.now);
            bt_sink_app_context.state = event->state_change.now;
        #ifdef BT_SINK_APP_NO_ACTION_AUTO_POWER_OFF
            bt_sink_app_auto_power_off_by_state(event->state_change.now);
        #endif
            bt_sink_app_atci_indicate(BT_SINK_APP_IND_TYPE_STATE, (uint32_t)event->state_change.now);
            break;

        case BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE:
            bt_sink_app_report("[Sink] connection information, profile:0x%x", event->connection_info.profile_type);
            if ( BT_SINK_SRV_PROFILE_NONE == event->connection_info.profile_type) {
                //bt_sink_app_atci_indicate(BT_SINK_APP_IND_TYPE_LINK_LOST, 0);
                bt_sink_app_reset_connected_device(&(event->connection_info.bt_addr));
                //bt_sink_app_context.connected = false;
                bt_sink_app_context.notify_connection_state = true;
            } else if (!bt_sink_app_get_connected_device(&(event->connection_info.bt_addr))) {
                //bt_sink_app_context.connected = true;
                bt_sink_app_set_connected_device(&(event->connection_info.bt_addr)) ;
                bt_sink_app_context.notify_connection_state = true;
            } else {
                bt_sink_app_context.notify_connection_state = false;
            }
            break;
            
         case BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE:
            {
                bt_sink_app_report("[Sink] hf sco information,state:0x%x, codec:0x%x", event->hf_sco_state.state, event->hf_sco_state.codec_type);
                bt_sink_app_atci_indicate(BT_SINK_APP_IND_TYPE_HF_SCO_STATE, (uint32_t)event->hf_sco_state.state);
            }
            break;
          
        case BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION:
            bt_sink_app_report("[Sink] caller information, number:%s, name:%s",
                               event->caller_info.number,
                               event->caller_info.name);
            if (strlen((char *)event->caller_info.name) > 0) {
                bt_sink_app_atci_indicate(BT_SINK_APP_IND_TYPE_CALLER, (uint32_t)event->caller_info.name);
            } else if (strlen((char *)event->caller_info.number) > 0) {
                bt_sink_app_atci_indicate(BT_SINK_APP_IND_TYPE_CALLER, (uint32_t)event->caller_info.number);
            }
            break;

        case BT_SINK_SRV_EVENT_HF_MISSED_CALL:
            bt_sink_app_report("[Sink] Missed call, number:%s, name:%s",
                               event->caller_info.number,
                               event->caller_info.name);
            if (strlen((char *)event->caller_info.name) > 0) {
                bt_sink_app_atci_indicate(BT_SINK_APP_IND_TYPE_MISSED_CALL, (uint32_t)event->caller_info.name);
            } else if (strlen((char *)event->caller_info.number) > 0) {
                bt_sink_app_atci_indicate(BT_SINK_APP_IND_TYPE_MISSED_CALL, (uint32_t)event->caller_info.number);
            }
            break;

        case BT_SINK_SRV_EVENT_HF_SPEAKER_VOLUME_CHANGE:
            bt_sink_app_report("[Sink] speaker volume, %d", event->volume_change.current_volume);
            break;

        case BT_SINK_EVENT_APP_EXT_COMMAND: {
            bt_sink_app_ext_cmd_t *ext_cmd_p = (bt_sink_app_ext_cmd_t *)parameters;
            bt_sink_app_key_action_handler(ext_cmd_p->key_value, ext_cmd_p->key_action);
        }
        break;
        
        case BT_SINK_EVENT_APP_BATTERY_NOTIFICATION: {
            bt_sink_app_battery_info_t *battery_info_p = (bt_sink_app_battery_info_t *)parameters;
            bt_sink_app_report("[Sink] battery level, charger_exist:%d, capacity:%d", battery_info_p->charger_exist, battery_info_p->capacity);
            bt_sink_app_battery_level_handler(battery_info_p->charger_exist, battery_info_p->capacity);
            
            extern void ble_bas_app_update_battery_capacity(int32_t capacity);
            ble_bas_app_update_battery_capacity((int32_t)(battery_info_p->capacity));
        }
        break;

        case BT_SINK_SRV_EVENT_HF_CALL_LIST_INFORMATION:
            bt_sink_app_report("[Sink] call list information, address:0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x, final:%d",
                event->call_list.address[5],
                event->call_list.address[4],
                event->call_list.address[3],
                event->call_list.address[2],
                event->call_list.address[1],
                event->call_list.address[0],
                event->call_list.final);
            bt_sink_app_report("[Sink] call list information, idx:%d, dir:%d, state:%d, mode:%d, mul:%d, size:%d, type:%d",
                event->call_list.index,
                event->call_list.director,
                event->call_list.state,
                event->call_list.mode,
                event->call_list.multiple_party,
                event->call_list.num_size,
                event->call_list.type);
            break;

        case BT_SINK_SRV_EVENT_HF_VOICE_RECOGNITION_CHANGED:
            bt_sink_app_report("[Sink] Voice recognition address:0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x",
                event->voice_recognition.address[5],
                event->voice_recognition.address[4],
                event->voice_recognition.address[3],
                event->voice_recognition.address[2],
                event->voice_recognition.address[1],
                event->voice_recognition.address[0]);
            bt_sink_app_report("[Sink] Voice recognition enable:%d", event->voice_recognition.enable);
            break;

        case BT_SINK_SRV_EVENT_VISIBILITY_STATE_UPDATE:
            bt_sink_app_report("[Sink] Visibility state, %d", event->visibility_state.visibility_on);
            bt_sink_app_atci_indicate(BT_SINK_APP_IND_TYPE_VISIBILITY, (uint32_t)event->visibility_state.visibility_on);
            break;
            
         case BT_SINK_SRV_EVENT_PROFILE_CONNECTION_UPDATE:
            bt_sink_app_report("[Sink] profile:0x%x, state: 0x%x", event->profile_state.profile, event->profile_state.state);
            //bt_sink_app_atci_indicate(BT_SINK_APP_IND_TYPE_PROFILE_CONN_STATE, (uint32_t)&(event->profile_state));
            if (BT_SINK_SRV_PROFILE_HFP == event->profile_state.profile && 
                    BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED == event->profile_state.state) {
                bt_sink_app_reset_hf_battery();
            }
           break;

        case BT_SINK_SRV_EVENT_REMOTE_SINK_ACTION_RECEIVED:
            bt_sink_app_report("[Sink] remote sink action: 0x%x", event->remote_sink_action);
            bt_sink_srv_action_send(event->remote_sink_action, NULL);
           break;
           
        case BT_SINK_EVENT_APP_HCI_LOG_ON:
            bt_demo_hci_log_switch(true);
            break;

        case BT_SINK_EVENT_APP_HCI_LOG_OFF:
            bt_demo_hci_log_switch(false);
            break;

        case BT_SINK_EVENT_APP_SYS_LOG_ON:
            bt_demo_syslog_switch(true);
            break;

        case BT_SINK_EVENT_APP_SYS_LOG_OFF:
            bt_demo_syslog_switch(false);
            break;

        case BT_SINK_EVENT_APP_NOTIFICATION:
            bt_sink_app_notification_handler(parameters);
            break;

#ifdef MTK_PROMPT_SOUND_ENABLE
        case BT_SINK_SRV_EVENT_MIXER_TONE_PLAY: {
                voice_prompt_mix_tone_info_t *tone_info = (voice_prompt_mix_tone_info_t *)parameters;
                voice_prompt_mix_play_tone(tone_info->tone_type, tone_info->tone_idx);
            }
            break;
        case BT_SINK_SRV_EVENT_MIXER_TONE_STOP:
            voice_prompt_mix_stop_tone();
            break;
#endif
#ifdef MTK_AUDIO_MP3_ENABLED
        case BT_SINK_SRV_EVENT_MP3_PLAY:
            bt_sink_srv_mp3_play_process();
            break;
#endif

        default:
            break;
    }
    
#ifdef MTK_LED_ENABLE    
    bt_sink_app_update_led_by_sink_event(event_id, parameters);
#endif /*MTK_LED_ENABLE*/

#ifdef MTK_PROMPT_SOUND_ENABLE
    bt_sink_app_voice_prompt_by_sink_event(event_id, parameters);
#endif /*MTK_PROMPT_SOUND_ENABLE*/

    return BT_SINK_SRV_STATUS_SUCCESS;
}

bt_gap_le_local_key_t bt_sink_local_key_req = {
    .encryption_info = {{0}},
    .master_id = {0},
    .identity_info = {{0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x19, 0x28, 0x55, 0x33, 0x68, 0x33, 0x56, 0xde}},
    .signing_info = {{0}}
};

static bt_gap_le_local_config_req_ind_t bt_sink_le_local_config;

bt_gap_le_local_config_req_ind_t *bt_sink_app_get_le_local_configuration(void)
{
    bt_sink_le_local_config.local_key_req = &bt_sink_local_key_req;
    bt_sink_le_local_config.sc_only_mode_req = false;
    return &bt_sink_le_local_config;
}

static bt_gap_le_bonding_info_t bt_sink_le_bonding_info;

bt_gap_le_bonding_info_t *bt_sink_app_gap_le_get_bonding_info(const bt_addr_t remote_addr)
{
    return &bt_sink_le_bonding_info;
}

bt_status_t bt_sink_app_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind)
{
    return BT_STATUS_SUCCESS;
}
