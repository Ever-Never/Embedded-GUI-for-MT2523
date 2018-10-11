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
#include "bt_sink_app_keypad.h"
#include "bt_sink_srv.h"
#include "task_def.h"
#include "bt_init.h"
#include "syslog.h"
#include "bt_notify.h"
#ifdef MTK_AUDIO_MP3_ENABLED
#include "audio_player.h"
#endif
#include "nvdm.h"
#ifdef __CFW_CONFIG_MODE__
#include "cfw_define.h"
#endif
#include "fota_gprot.h"

bt_sink_app_context_t bt_sink_app_context;

log_create_module(sink_app, PRINT_LEVEL_INFO);
static void bt_sink_app_init_device_role(void);

#ifdef BT_SINK_APP_NO_ACTION_AUTO_POWER_OFF
/*Auto power off feature -- start */
#define POWER_OFF_AUTO_TIMEOUT         (10*60*1000)
static TimerHandle_t g_power_off_timer_id = NULL;

static void bt_sink_app_auto_power_off_timer_restart(void)
{
    bt_sink_app_report( "[Poweroff] re-start auto power off timer:0x%x", g_power_off_timer_id); 
    if (xTimerIsTimerActive(g_power_off_timer_id)) {
        bt_sink_app_report( "[Poweroff] timer has been started.");
        return;
    } 
    xTimerStart(g_power_off_timer_id, 0);
    return;
}

static void bt_sink_app_auto_power_off_timer_stop(void)
{
    if (xTimerIsTimerActive(g_power_off_timer_id)) {
        bt_sink_app_report( "[Poweroff] stop auto power off timer:0x%x", g_power_off_timer_id); 
        xTimerStop(g_power_off_timer_id, 0);
    }
    return;
}

static void bt_sink_app_auto_power_off_timeout_handler(TimerHandle_t xTimer)
{
    bt_sink_app_report("[Poweroff]auto power off.");    
    //pmu_enter_power_off();
    bt_sink_srv_action_send(BT_SINK_SRV_ACTION_POWER_OFF, NULL);
}

static void bt_sink_app_auto_power_off_init(void)
{
    g_power_off_timer_id  = xTimerCreate("POWER_OFF_AUTO_TIMER",     
                             (POWER_OFF_AUTO_TIMEOUT / portTICK_PERIOD_MS),  /* The timer period in ticks. */
                             pdFALSE,
                             (void *) NULL,
                             bt_sink_app_auto_power_off_timeout_handler);
    bt_sink_app_report( "[Poweroff] auto timer id:0x%x", g_power_off_timer_id); 
    xTimerStart(g_power_off_timer_id, 0);
    return;
}

void bt_sink_app_auto_power_off_by_state(bt_sink_srv_state_t current)
{
    bt_sink_app_report( "[Poweroff] current state:%d",current); 

    if (current == BT_SINK_SRV_STATE_POWER_ON) {
        bt_sink_app_auto_power_off_timer_restart();
    } else if(current == BT_SINK_SRV_STATE_NONE ) {
         bt_sink_app_report( "[Poweroff] error state"); 
    } else {
        bt_sink_app_auto_power_off_timer_stop();
    }
    return;
}
#endif /*#BT_SINK_APP_NO_ACTION_AUTO_POWER_OFF*/

/*Auto power off feature -- end*/

void bt_sink_srv_event_callback(bt_sink_srv_event_t event_id, bt_sink_srv_event_param_t *param)
{
    void *event_params = NULL;

    if (NULL != param) {
        event_params = (bt_sink_srv_event_param_t *)pvPortMalloc(sizeof((*param)));
        if (NULL != event_params) {
            memcpy(event_params, param, sizeof(*param));
        } else {
            bt_sink_app_report("malloc fail");
        }
    }
    bt_sink_app_event_post(event_id, event_params, bt_sink_app_event_post_callback);
}

void bt_notify_srv_event_callback(void *data)
{
    bt_notify_callback_data_t *notification = (bt_notify_callback_data_t *)data;

    if (NULL != notification) {
        uint32_t notification_id = (uint32_t)notification->evt_id;

        bt_sink_app_report("[Sink][NOTI] notification:%d", notification_id);
        if ((notification_id == BT_NOTIFY_EVENT_NOTIFICATION) && (notification->notification.action == BT_NOTIFY_ACTION_TYPE_NEW)) {
            bt_sink_app_event_post(BT_SINK_EVENT_APP_NOTIFICATION, (void *)notification_id, NULL);
        }
    }
}

void bt_sink_app_task_main(void *arg)
{
    bt_sink_app_event_t event;
    //bt_sink_srv_features_config_t config;

    bt_sink_app_report("enter main");

    memset(&bt_sink_app_context, 0, sizeof(bt_sink_app_context_t));

    // initialize event
    bt_sink_app_event_init();

#ifdef MTK_LED_ENABLE
        bt_sink_app_led_init();
#endif

#ifdef BT_SINK_APP_NO_ACTION_AUTO_POWER_OFF
    bt_sink_app_auto_power_off_init();
#endif

    // initialize queue
    bt_sink_app_context.queue_handle = xQueueCreate(BT_SINK_APP_QUEUE_SIZE, sizeof(bt_sink_app_event_t));

    bt_sink_app_event_register_callback(BT_SINK_EVENT_APP_KEY_INPUT, bt_sink_app_keypad_event_handler);
    bt_sink_app_event_register_callback(BT_SINK_SRV_EVENT_ALL, bt_sink_app_event_handler);
    // Receive notification
    bt_notify_register_callback(NULL, "bt_sink_app", bt_notify_srv_event_callback);

    bt_sink_app_atci_init();
    bt_sink_app_keypad_init();
    // init sink app role
    bt_sink_app_init_device_role();

    // initialize sink service
#if defined(MTK_INEAR_NOODLES_HEADSET_ENABLE)
    bt_sink_app_context.feature_config.features = BT_SINK_SRV_FEATURE_POWER_ON_RECONNECT | BT_SINK_SRV_FEATURE_VISIBILITY 
        | BT_SINK_SRV_FEATURE_IOS_HFP_CUSTOM_AT_COMMAND | BT_SINK_SRV_FEATURE_RECONNECT_ON_LINK_LOSS;
#else
    if(BT_SINK_APP_DEVICE_MASTER == bt_sink_app_get_device_role()) {
        bt_sink_app_context.feature_config.features = BT_SINK_SRV_FEATURE_POWER_ON_RECONNECT | BT_SINK_SRV_FEATURE_VISIBILITY 
            |BT_SINK_SRV_FEATURE_RECONNECT_ON_LINK_LOSS |BT_SINK_SRV_FEATURE_AUTO_CONNECT_PBAPC
            |BT_SINK_SRV_FEATURE_SYNC_POWER_OFF | BT_SINK_SRV_FEATURE_LOCK_AWS_DEVICE;
    } else if (BT_SINK_APP_DEVICE_SLAVE == bt_sink_app_get_device_role()) {
        bt_sink_app_context.feature_config.features = BT_SINK_SRV_FEATURE_POWER_ON_RECONNECT |BT_SINK_SRV_FEATURE_RECONNECT_ON_LINK_LOSS |
            BT_SINK_SRV_FEATURE_SYNC_POWER_OFF | BT_SINK_SRV_FEATURE_INQUIRY_AND_CONNECT |BT_SINK_SRV_FEATURE_LOCK_AWS_DEVICE;
    }
#endif

    if (fota_is_power_on_after_update()) {
        bt_sink_app_context.feature_config.features &= ~BT_SINK_SRV_FEATURE_POWER_ON_RECONNECT;
    }
        
    bt_sink_srv_init(&(bt_sink_app_context.feature_config));

    bt_demo_power_on();

#ifdef MTK_LED_ENABLE
    bt_sink_app_led_light(BT_SINK_APP_LED_STATE_POWER_ON);
#endif

#ifdef MTK_PROMPT_SOUND_ENABLE
        bt_sink_app_voice_prompt_play(VOICE_PROMPT_TONE_POWER_ON);
#endif /*MTK_PROMPT_SOUND_ENABLE*/

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

static void bt_sink_app_init_device_role(void)
{
    bt_sink_app_device_role_t role = BT_SINK_APP_DEVICE_MASTER;
    uint32_t size = 1;

#ifdef __CFW_CONFIG_MODE__
    role = (bt_sink_app_device_role_t)(CFW_CFG_ITEM_VALUE(bt_device_role));
#else
    nvdm_status_t nvdm_status = nvdm_read_data_item("BT_SINK", "role", (uint8_t *)&role, &size);
    bt_sink_app_report("read role result:%d", nvdm_status);
#endif
    bt_sink_app_context.device_role  = role;
    bt_sink_app_report("init role:%d", bt_sink_app_context.device_role);
    return;
}

bt_sink_app_device_role_t bt_sink_app_get_device_role(void)
{
    //bt_sink_app_report("[Sink][APP] get device role:%d", bt_sink_app_context.device_role);   
    return bt_sink_app_context.device_role;
}

void bt_sink_app_set_device_role(bt_sink_app_device_role_t role)
{
    bt_sink_app_report("[Sink][APP] set device role:%d", role);   
    #ifndef __CFW_CONFIG_MODE__
    nvdm_status_t result =
        nvdm_write_data_item("BT_SINK",
                             "role",
                             NVDM_DATA_ITEM_TYPE_RAW_DATA,
                             (const uint8_t *)&role,
                             1);
    bt_sink_app_context.device_role = role;
    bt_sink_app_report("write role result:%d", result);
    #endif
    return;
}

void bt_sink_app_battery_report_post_callback(bt_sink_srv_event_t event_id, bt_sink_srv_status_t result, void *parameters)
{
    if (NULL != parameters) {
        vPortFree(parameters);
    }
}

void bt_sink_app_battery_report_handler(int32_t charger_exist, uint8_t capacity)
{
    if (bt_sink_app_context.queue_handle != NULL) {
        bt_sink_app_battery_info_t *battery_info_p = (bt_sink_app_battery_info_t *)pvPortMalloc(sizeof(*battery_info_p));
        battery_info_p->charger_exist = charger_exist;
        battery_info_p->capacity = capacity;
        bt_sink_app_event_post((bt_sink_srv_event_t)BT_SINK_EVENT_APP_BATTERY_NOTIFICATION,
                                       (void *)battery_info_p,
                                       bt_sink_app_battery_report_post_callback);
    }
    return;
}

void bt_sink_app_battery_level_handler(int32_t charger_exist, uint8_t capacity)
{  
    bt_sink_srv_ios_hf_features_t hf_feature = bt_sink_srv_get_ios_hf_features_params();

   /*Update battery level to remote ios device */
    if ((bt_sink_app_context.feature_config.features & BT_SINK_SRV_FEATURE_IOS_HFP_CUSTOM_AT_COMMAND) && 
          (hf_feature & BT_SINK_SRV_IOS_HF_BATTERY_REPORT)) {
        uint8_t new_level = 0;
        if (capacity == 0) {
            new_level = 0;
        } else if ((capacity > 0) && (capacity < 100)) {
            new_level = capacity / 10; //(0~9)
        } else if (capacity == 100) {
            new_level = 9;
        }
        bt_sink_app_report("[Sink][HF][battery]capacity:%d, old: %d, new:%d",capacity, bt_sink_app_context.battery_level, new_level);
        if (new_level != bt_sink_app_context.battery_level) {
            if(BT_SINK_SRV_STATUS_SUCCESS == bt_sink_srv_action_send(BT_SINK_SRV_ACTION_HF_REPORT_BATTERY, (void*)&new_level)) {
                bt_sink_app_context.battery_level = new_level;
            }
        }  
    }

    /*Update led state */
    #ifdef MTK_LED_ENABLE
        bt_sink_app_update_led_by_battery(charger_exist, capacity);
    #endif /*MTK_LED_ENABLE*/
    
    #ifdef MTK_PROMPT_SOUND_ENABLE
        bt_sink_app_voice_prompt_by_battery(charger_exist, capacity);
    #endif /*MTK_PROMPT_SOUND_ENABLE*/

    /*If low battery state, auto power off. */
    if ((charger_exist == 0) && (capacity <= BT_SINK_APP_LOW_BATTERY_POWER_OFF)) {
            bt_sink_app_report("[Sink][HF][battery] Low battery power off:%d",capacity);
        #ifdef MTK_LED_ENABLE
            bt_sink_app_led_light(BT_SINK_APP_LED_STATE_POWER_OFF);
        #endif /*MTK_LED_ENABLE*/
            bt_sink_srv_action_send(BT_SINK_SRV_ACTION_POWER_OFF, NULL);
    } 
    return;
}

void bt_sink_app_reset_hf_battery(void)
{
    bt_sink_app_context.battery_level = 0;
}

