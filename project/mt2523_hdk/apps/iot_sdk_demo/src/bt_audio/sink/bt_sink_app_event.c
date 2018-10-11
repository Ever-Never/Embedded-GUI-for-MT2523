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
#include "bt_a2dp.h"
#include "bt_gap_le.h"
#include "bt_gap.h"
#include "bt_init.h"

static bt_gap_le_local_config_req_ind_t bt_sink_le_local_config;

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

bt_sink_srv_status_t bt_sink_app_event_handler(bt_sink_srv_event_t event_id, void *parameters)
{
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)parameters;

    bt_sink_app_report("[Sink] event:0x%x", event_id);

    switch (event_id) {
        case BT_SINK_SRV_EVENT_STATE_CHANGE:
            bt_sink_app_report("[Sink] state change, previous:0x%x, now:0x%x", event->state_change.previous, event->state_change.now);
            bt_sink_app_context.state = event->state_change.now;
            bt_sink_app_atci_indicate(event->state_change.now);
            break;


        case BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE:
            bt_sink_app_report("[Sink] connection information, profile:0x%x", event->connection_info.profile_type);
            break;

        case BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION:
            bt_sink_app_report("[Sink] caller information, number:%s, name:%s",
                               event->caller_info.number,
                               event->caller_info.name);
            break;

        case BT_SINK_SRV_EVENT_HF_MISSED_CALL:
            bt_sink_app_report("[Sink] Missed call, number:%s, name:%s",
                               event->caller_info.number,
                               event->caller_info.name);
            break;

        case BT_SINK_SRV_EVENT_HF_SPEAKER_VOLUME_CHANGE:
            bt_sink_app_report("[Sink] speaker volume, %d", event->volume_change.current_volume);
            break;

        case BT_SINK_EVENT_APP_EXT_COMMAND: {
            bt_sink_app_ext_cmd_t *ext_cmd_p = (bt_sink_app_ext_cmd_t *)parameters;
            bt_sink_srv_key_action(ext_cmd_p->key_value, ext_cmd_p->key_action);
            break;
        }

        case BT_SINK_EVENT_APP_HCI_LOG_ON:
            bt_demo_hci_log_switch(true);
            break;

        case BT_SINK_EVENT_APP_HCI_LOG_OFF:
            bt_demo_hci_log_switch(false);
            break;

        default:
            break;
    }
    return BT_SINK_SRV_STATUS_SUCCESS;
}

bt_gap_le_local_key_t bt_sink_local_key_req = {
    .encryption_info = {{0}},
    .master_id = {0},
    .identity_info = {{0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x19, 0x28, 0x55, 0x33, 0x68, 0x33, 0x56, 0xde}},
    .signing_info = {{0}}
};

bt_gap_le_local_config_req_ind_t *bt_sink_app_get_le_local_configuration(void)
{
    bt_sink_le_local_config.local_key_req = &bt_sink_local_key_req;
    bt_sink_le_local_config.sc_only_mode_req = false;
    return &bt_sink_le_local_config;
}

const bt_gap_config_t *bt_sink_app_get_local_configuration(void)
{
    return bt_sink_srv_gap_config();
}

