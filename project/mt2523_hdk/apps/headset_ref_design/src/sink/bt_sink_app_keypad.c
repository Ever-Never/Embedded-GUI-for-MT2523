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
#include "bt_sink_app_keypad.h"
#include "bt_sink_app_main.h"
#ifdef MTK_AUDIO_MP3_ENABLED
#include "audio_player.h"
#endif


#if defined (MTK_CORD_FREE_HEADSET_ENABLE)
#define DEVICE_ONE_KEY DEVICE_KEY_POWER
#else /*MTK_CORD_FREE_HEADSET_ENABLE*/
#define DEVICE_ONE_KEY DEVICE_KEY_FUNCTION
#endif /*MTK_CORD_FREE_HEADSET_ENABLE*/

static const bt_sink_app_keypad_mapping_t g_bt_sink_app_key_mapping[] = {
    {DEVICE_ONE_KEY, BT_SINK_SRV_KEY_FUNC},
    {DEVICE_KEY_VOL_UP, BT_SINK_SRV_KEY_PREV},
    {DEVICE_KEY_VOL_DOWN, BT_SINK_SRV_KEY_NEXT}
};

// should map to sct_key_event_t
static const bt_sink_srv_key_action_t g_bt_sink_app_keydown_mapping[] = {
    BT_SINK_SRV_KEY_ACT_NONE,
    BT_SINK_SRV_KEY_ACT_NONE,    
    BT_SINK_SRV_KEY_ACT_LONG_PRESS_DOWN,
    BT_SINK_SRV_KEY_ACT_LONG_LONG_PRESS_DOWN,
    BT_SINK_SRV_KEY_ACT_VERY_LONG_PRESS_DOWN
};

static bt_sink_app_keypad_key_t g_bt_sink_app_key[] = {
    {DEVICE_ONE_KEY,  SCT_KEY_RELEASE},
    {DEVICE_KEY_VOL_UP, SCT_KEY_RELEASE},
    {DEVICE_KEY_VOL_DOWN, SCT_KEY_RELEASE}
};

void bt_sink_app_keypad_init(void)
{
    bool status;

    status = sct_key_event_init();
    if (status != true) {
        bt_sink_app_report("[Sink][Keypad] sct key init fail, status:%d", status);
    }

    status = sct_key_register_callback(bt_sink_app_keypad_callback, NULL);
    if (status != true) {
        bt_sink_app_report("[Sink][Keypad] register callback fail, status:%d", status);
    }
}

void bt_sink_app_keypad_post_callback(bt_sink_srv_event_t event_id, bt_sink_srv_status_t result, void *parameters)
{
    if (NULL != parameters) {
        vPortFree(parameters);
    }
}

void bt_sink_app_keypad_callback(sct_key_event_t event, uint8_t key_data, void *user_data)
{
        bt_sink_app_keypad_key_t *key = (bt_sink_app_keypad_key_t *)pvPortMalloc(sizeof(*key));
        key->event = event;
        key->value = key_data;
        bt_sink_app_keypad_post_event(BT_SINK_EVENT_APP_KEY_INPUT, (void*)key, bt_sink_app_keypad_post_callback);
}

void bt_sink_app_keypad_post_event(bt_sink_srv_event_t key_event, bt_sink_app_keypad_key_t *key, bt_sink_app_keypad_post_result_t callback)
{
    bt_sink_app_event_t event;

    bt_sink_app_report("[Sink] bt_sink_app_keypad_post_event, value:%d, event:%d", key->value, key->event);

    memset(&event, 0, sizeof(bt_sink_app_event_t));
    event.event_id = key_event;
    event.parameters = key;
    event.post_callback = callback;
    
    if (bt_sink_app_context.queue_handle == NULL) {
        bt_sink_app_report("[Sink] queue is not ready.");
        return;
    }

    if (key && key->event == SCT_KEY_REPEAT) {
        bt_sink_app_report("[Sink] sink app will not handle repeat event.");
        return;
    }
    
    if (pdPASS != xQueueSend(bt_sink_app_context.queue_handle, &event, 0)) {
        if (NULL != callback) {
            callback(key_event, BT_SINK_SRV_STATUS_FAIL, key);
        }
        bt_sink_app_report("[Sink][Fatal Error] key event lost");
    }
}

static bt_sink_app_keypad_key_t *bt_sink_app_keypad_find_key(uint8_t value)
{
    uint16_t i;
    for (i = 0; i < (sizeof(g_bt_sink_app_key) / sizeof(bt_sink_app_keypad_key_t)); i++) {
        if (value == g_bt_sink_app_key[i].value) {
            return &g_bt_sink_app_key[i];
        }
    }
    return NULL;
}

static bt_sink_srv_key_value_t bt_sink_app_keypad_find_sink_key(uint8_t value)
{
    uint16_t i;
    bt_sink_srv_key_value_t sink_key = BT_SINK_SRV_KEY_NONE;

    for (i = 0; i < (sizeof(g_bt_sink_app_key_mapping) / sizeof(bt_sink_app_keypad_mapping_t)); i++) {
        if (value == g_bt_sink_app_key_mapping[i].value) {
            sink_key = g_bt_sink_app_key_mapping[i].sink_key;
            break;
        }
    }
    return sink_key;
}

static void bt_sink_app_keypad_handler(bt_sink_app_keypad_key_t *next)
{
    bt_sink_app_keypad_key_t *current = bt_sink_app_keypad_find_key(next->value);

    bt_sink_app_report("[Sink][KEY] event:%d, value:%d", next->event, next->value);

    if (NULL != current && current->event != next->event) {
        bt_sink_srv_key_value_t sink_key_value = bt_sink_app_keypad_find_sink_key(next->value);
        bt_sink_srv_key_action_t sink_key_action = BT_SINK_SRV_KEY_ACT_NONE;

        if (SCT_KEY_SHORT_PRESS == next->event) {
            current->event = next->event;
            sink_key_action = BT_SINK_SRV_KEY_ACT_PRESS_UP;
        } else if (SCT_KEY_DOUBLE_PRESS == next->event) {
            current->event = next->event;
            sink_key_action = BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK;
        } else if (SCT_KEY_TRIPLE_PRESS == next->event) {
            current->event = next->event;
            sink_key_action = BT_SINK_SRV_KEY_ACT_TRIPLE_CLICK;
        } else if (SCT_KEY_RELEASE == next->event) {
            switch (current->event) {
                case SCT_KEY_LONG_PRESS_1:
                    sink_key_action = BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP;
                    break;

                case SCT_KEY_LONG_PRESS_2:
                    sink_key_action = BT_SINK_SRV_KEY_ACT_LONG_LONG_PRESS_UP;
                    break;

                case SCT_KEY_LONG_PRESS_3:
                    sink_key_action = BT_SINK_SRV_KEY_ACT_VERY_LONG_PRESS_UP;
                    break;

                default:
                    break;
            }
            current->event = next->event;
        } else if (next->event <= SCT_KEY_LONG_PRESS_3) {
            sink_key_action = g_bt_sink_app_keydown_mapping[next->event];
            current->event = next->event;
        } else {
            // HAL_KEYPAD_KEY_REPEAT
            // HAL_KEYPAD_KEY_PMU_LONG_PRESS
        }
        bt_sink_app_key_action_handler(sink_key_value, sink_key_action);
    }
}

bt_sink_srv_status_t bt_sink_app_keypad_event_handler(bt_sink_srv_event_t event_id, void *parameters)
{
    if (BT_SINK_EVENT_APP_KEY_INPUT == event_id) {
        bt_sink_app_keypad_handler((bt_sink_app_keypad_key_t *)parameters);
    }
    return BT_SINK_SRV_STATUS_EVENT_STOP;
}

#ifdef BT_SINK_APP_TEST_EXIT_MASS_STORAGE
#include "usb.h"
#ifdef MTK_AUDIO_MP3_ENABLED
#include "bt_sink_srv_mp3.h"
#include "audio_file_manager.h"
#endif
extern bool bt_hci_log_enabled(void);
extern void bt_demo_hci_log_switch(bool on_off);

static bool g_bt_sink_app_com_port_mode = false;
bool bt_sink_app_usb_is_com_port_mode(void)
{
    return g_bt_sink_app_com_port_mode;
}

static void bt_sink_app_key_exit_mass_storage(void)
{
    bool ret = false;
    if(g_bt_sink_app_com_port_mode) {
        bt_sink_app_report("[Sink][KEY][USB] alreay exit mass storage");
        return;
    }
    ret = ap_usb_deinit();
    bt_sink_app_report("[Sink][KEY][USB] deinit:%d", ret);
    if (ret) {
        ret =  ap_usb_init(USB_CDC_ACM);
        bt_sink_app_report("[Sink][KEY][USB]  init com port:%d", ret);
        if (ret) {
            g_bt_sink_app_com_port_mode = true;
            #ifdef MTK_AUDIO_MP3_ENABLED
            audio_file_manager_init();
            bt_sink_srv_mp3_init();
            #endif
            bt_hci_log_enabled();
            bt_demo_hci_log_switch(true);
        }
    }
}
#endif


#ifdef __BT_SINK_SRV_MUSIC_MODE_SUPPORT__ 
extern bt_sink_srv_music_mode_t bt_sink_srv_get_music_mode(void);
void bt_sink_app_send_play_action_by_mode(void) 
{
    bt_sink_srv_music_mode_t mode =  bt_sink_srv_get_music_mode();
    if (mode == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
        #ifdef MTK_AUDIO_MP3_ENABLED
        audio_player_sink_key_action_handler(AUDIO_PLAYER_ACTION_PLAY);
        #endif
    } else if (mode == BT_SINK_SRV_MODE_BT_MUSIC) {
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_PLAY, NULL);
    }
    return;
}
#endif /*__BT_SINK_SRV_MUSIC_MODE_SUPPORT__*/

void bt_sink_app_key_action_handler(bt_sink_srv_key_value_t key_value, bt_sink_srv_key_action_t key_action)
{
    bt_sink_app_report("[Sink][KEY] key value:0x%x, key action:0x%x", key_value, key_action);
    const bt_sink_srv_table_t *mapping_table = bt_sink_srv_get_mapping_table();
    if (BT_SINK_SRV_KEY_NONE == key_value ||BT_SINK_SRV_KEY_ACT_NONE == key_action || mapping_table == NULL) {
        bt_sink_app_report("[Sink][KEY] Invalid key param, vaule:0x%x, action:0x%x, table:0x%x", key_value, mapping_table);
        return;
    }
    
    bt_sink_srv_state_t sink_state = bt_sink_srv_state_query();
    bt_sink_app_report("[Sink][KEY] Current state:0x%x", sink_state);
    uint32_t index = 0;
    while (BT_SINK_SRV_KEY_NONE != mapping_table[index].key_value) {
        if (sink_state & mapping_table[index].sink_state && key_value == mapping_table[index].key_value
            && key_action == mapping_table[index].key_action) {
        #ifdef MTK_PROMPT_SOUND_ENABLE
            bt_sink_app_voice_prompt_by_sink_action(mapping_table[index].sink_action);
        #endif /*MTK_PROMPT_SOUND_ENABLE*/
            if (mapping_table[index].sink_action >= BT_SINK_SRV_ACTION_USER_START && mapping_table[index].sink_action <= BT_SINK_SRV_ACTION_USER_END) {
        #ifdef BT_SINK_APP_TEST_EXIT_MASS_STORAGE
                if (mapping_table[index].sink_action == BT_SINK_APP_EXIT_MASS_STORGE_MODE) {
                    bt_sink_app_key_exit_mass_storage();
                    break;
                }
        #endif
        #ifdef __BT_SINK_SRV_MUSIC_MODE_SUPPORT__ 
                if (mapping_table[index].sink_action == BT_SINK_APP_ACTION_PLAY_BY_MODE) {
                    bt_sink_app_send_play_action_by_mode();
                    break;
                }
        #endif
        #ifdef MTK_AUDIO_MP3_ENABLED
                audio_player_sink_key_action_handler(mapping_table[index].sink_action);
        #endif
            } else if (mapping_table[index].sink_action>= BT_SINK_SRV_ACTION_HF_START && mapping_table[index].sink_action <= BT_SINk_SRV_ACTION_HF_END) {
                if (BT_SINK_APP_DEVICE_MASTER == bt_sink_app_get_device_role()) {
                    bt_sink_srv_action_send(mapping_table[index].sink_action, NULL);
                } else {
                    bt_sink_srv_action_send(BT_SINK_SRV_ACTION_AWS_TRANSFER_SINK_ACTION, (void*)&(mapping_table[index].sink_action));
                }
            } else {
                #ifdef MTK_LED_ENABLE 
                if (mapping_table[index].sink_action == BT_SINK_SRV_ACTION_RESET_TRUSTED_LIST) {
                    bt_sink_app_led_light(BT_SINK_APP_LED_STATE_RESET_PAIRED_LIST);
                } else if (mapping_table[index].sink_action == BT_SINK_SRV_ACTION_POWER_OFF) {
                    bt_sink_app_led_light(BT_SINK_APP_LED_STATE_POWER_OFF);
                }
                #endif /*MTK_LED_ENABLE*/
                bt_sink_srv_action_send(mapping_table[index].sink_action, NULL);
            }
            bt_sink_app_report("[Sink][KEY] sink state:0x%x, sink action:0x%x", mapping_table[index].sink_state, mapping_table[index].sink_action);
            break;
        }
        index++;
    }
}

