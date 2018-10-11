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
#include "bt_sink_app_main.h"
#include "bt_sink_app_event.h"
#include "bt_sink_app_cmd.h"
#include "bt_sink_srv.h"
#include "task_def.h"
#include "syslog.h"

bt_sink_app_context_t bt_sink_app_context;

log_create_module(sink_app, PRINT_LEVEL_INFO);

void bt_sink_srv_event_callback(bt_sink_srv_event_t event_id, bt_sink_srv_event_param_t *param)
{
    void *event_params = NULL;

    if (NULL != param) {
        event_params = (bt_sink_srv_event_param_t *)pvPortMalloc(sizeof((*param)));
        memcpy(event_params, param, sizeof(*param));
    }
    bt_sink_app_event_post(event_id, event_params, bt_sink_app_event_post_callback);
}

void bt_sink_app_task_main(void *arg)
{
    bt_sink_app_event_t event;
    bt_sink_srv_features_config_t config;

    bt_sink_app_report("enter main");

    memset(&bt_sink_app_context, 0, sizeof(bt_sink_app_context_t));

    // initialize event
    bt_sink_app_event_init();

    // initialize queue
    bt_sink_app_context.queue_handle = xQueueCreate(BT_SINK_APP_QUEUE_SIZE, sizeof(bt_sink_app_event_t));

    bt_sink_app_event_register_callback(BT_SINK_SRV_EVENT_ALL, bt_sink_app_event_handler);

    bt_sink_app_atci_init();

    // initialize sink service
    config.features = BT_SINK_SRV_FEATURE_DISCOVERABLE_ONCE;
    bt_sink_srv_init(&config);

    while (1) {
        if (pdPASS == xQueueReceive(bt_sink_app_context.queue_handle, &event, portMAX_DELAY)) {
            bt_sink_app_event_process(&event);
        }
    }
}

void bt_sink_app_task_create(void)
{
    xTaskCreate(bt_sink_app_task_main,
                BT_SINK_APP_TASK_NAME,
                BT_SINK_APP_TASK_STACKSIZE / ((uint32_t)sizeof(StackType_t)),
                NULL,
                BT_SINK_APP_TASK_PRIO,
                NULL);
}

