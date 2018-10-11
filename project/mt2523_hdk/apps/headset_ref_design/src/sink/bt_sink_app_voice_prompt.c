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
 
#ifdef MTK_PROMPT_SOUND_ENABLE

#include "bt_sink_app_main.h"
#include "bt_sink_app_voice_prompt.h"
#include "prompt_control.h"

log_create_module(VOICE_PROMPT_APP, PRINT_LEVEL_INFO);
extern void pmu_enter_power_off(void);
void bt_sink_srv_voice_prompt_reset(void);

#include "bt_sink_srv_resource.h"
#ifdef  __BT_SINK_SRV_VOICE_PROMPT__
static void bt_sink_app_voice_prompt_stop(void);
#endif

void bt_sink_app_voice_prompt_play(uint8_t tone_idx);

#ifdef  __BT_SINK_SRV_VOICE_PROMPT__
static void bt_sink_app_voice_prompt_callback(prompt_control_event_t event_id);
#endif

bool g_bt_sink_app_voice_prompt_power_off = false;
bt_sink_app_voice_prompt_context_t bt_sink_app_voice_prompt_context;

/* WAV Tone */
static const uint8_t voice_prompt_tone_power_on[] = {
    #include "power_on_MADPCM.wav.txt"
};

static const uint8_t voice_prompt_tone_power_off[] = {
    #include "power_off_MADPCM.wav.txt"
};

static const uint8_t voice_prompt_tone_reset_pairing_list[] = {
    #include "clean_pairing_list_MADPCM.wav.txt"
};

static const uint8_t voice_prompt_tone_pairing[] = {
    #include "pairing_MADPCM.wav.txt"
};

static const uint8_t voice_prompt_tone_connected[] = {
    #include "connected_MADPCM.wav.txt"
};

static const uint8_t voice_prompt_tone_disconnected[] = {
    #include "disconnected_MADPCM.wav.txt"
};

static const uint8_t voice_prompt_tone_volume_change[] = {
    #include "DU1_MADPCM.wav.txt"
};

static const uint8_t voice_prompt_tone_call_audio_transfer[] = {
    #include "DU1_MADPCM.wav.txt"
};

static const uint8_t voice_prompt_tone_call_end[] = {
    #include "call ended_MADPCM.wav.txt"
};

static const uint8_t voice_prompt_tone_call_refused[] = {
    #include "call refused_MADPCM.wav.txt"
};

static const uint8_t voice_prompt_tone_redialing[] = {
    #include "redialing_MADPCM.wav.txt"
};

static const uint8_t voice_prompt_low_battery[] = {
    #include "battery low_MADPCM.wav.txt"
};

static const uint8_t voice_prompt_tone_MP3_music[] = {
    #include "MP3 music_MADPCM.wav.txt"
};

static const uint8_t voice_prompt_tone_BT_music[] = {
    #include "BT music_MADPCM.wav.txt"
};

static bt_sink_app_voice_prompt_buf_t bt_sink_app_voice_prompt_tone[] = {
    {
        .buf = voice_prompt_tone_power_on,
        .size = sizeof(voice_prompt_tone_power_on),
    },
    {
        .buf = voice_prompt_tone_power_off,
        .size = sizeof(voice_prompt_tone_power_off),
    },
    {
        .buf = voice_prompt_tone_reset_pairing_list,
        .size = sizeof(voice_prompt_tone_reset_pairing_list),
    },
    {
        .buf = voice_prompt_tone_pairing,
        .size = sizeof(voice_prompt_tone_pairing),
    },
    {
        .buf = voice_prompt_tone_connected,
        .size = sizeof(voice_prompt_tone_connected),
    },
    {
        .buf = voice_prompt_tone_disconnected,
        .size = sizeof(voice_prompt_tone_disconnected),
    },
    {
        .buf = voice_prompt_tone_volume_change,
        .size = sizeof(voice_prompt_tone_volume_change),
    },
    {
        .buf = voice_prompt_tone_call_audio_transfer,
        .size = sizeof(voice_prompt_tone_call_audio_transfer),
    },
    {
        .buf = voice_prompt_tone_call_end,
        .size = sizeof(voice_prompt_tone_call_end),
    },
    {
        .buf = voice_prompt_tone_call_refused,
        .size = sizeof(voice_prompt_tone_call_refused),
    },  
    {
        .buf = voice_prompt_tone_redialing,
        .size = sizeof(voice_prompt_tone_redialing),
    },
    {
        .buf = voice_prompt_low_battery,
        .size = sizeof(voice_prompt_low_battery),
    },
    {
        .buf = voice_prompt_tone_MP3_music,
        .size = sizeof(voice_prompt_tone_MP3_music),
    },
    {
        .buf = voice_prompt_tone_BT_music,
        .size = sizeof(voice_prompt_tone_BT_music),
    },
};

/********************************************************
 * Function
 *
 ********************************************************/
 #ifdef  __BT_SINK_SRV_VOICE_PROMPT__
static void bt_sink_app_voice_prompt_callback(prompt_control_event_t event_id)
{
    bt_sink_app_voice_prompt_report("callback event=%d\n", event_id);
    
    if (event_id == PROMPT_CONTROL_MEDIA_END) {
        bt_sink_app_voice_prompt_report("callback: playback complete");
        if (g_bt_sink_app_voice_prompt_power_off) {
            pmu_enter_power_off();
        }
    }
}
#endif

void bt_sink_app_voice_prompt_play(uint8_t tone_idx)
{
#ifdef  __BT_SINK_SRV_VOICE_PROMPT__
    uint8_t *tone_buf = NULL;
    uint32_t tone_size = 0;
    bool ret;
    
    bt_sink_app_voice_prompt_stop();

    bt_sink_app_voice_prompt_report("voice prompt: idx: %d", tone_idx);

    tone_buf = (uint8_t *)bt_sink_app_voice_prompt_tone[tone_idx].buf;
    tone_size = bt_sink_app_voice_prompt_tone[tone_idx].size;
    ret = prompt_control_play_tone(VPC_WAV, tone_buf, tone_size, bt_sink_app_voice_prompt_callback);
    
    bt_sink_app_voice_prompt_report("mix play result: %d", ret);
#endif    /*__BT_SINK_SRV_VOICE_PROMPT__*/
}

#ifdef  __BT_SINK_SRV_VOICE_PROMPT__
static void bt_sink_app_voice_prompt_stop(void)
{
    bt_sink_app_voice_prompt_report("voice prompt stop done.");
    prompt_control_stop_tone();
}
#endif

void bt_sink_app_voice_prompt_event_handle(bt_sink_srv_action_t action)
{

    bt_sink_app_voice_prompt_tone_t mix_tone = VOICE_PROMPT_TONE_NUM;

    switch (action) {
        case BT_SINK_SRV_ACTION_POWER_OFF: 
        {
            mix_tone = VOICE_PROMPT_TONE_POWER_OFF;
            g_bt_sink_app_voice_prompt_power_off = true;
        }
        break;

        default:
        break;
    }
    
    if (mix_tone != VOICE_PROMPT_TONE_NUM)
    bt_sink_app_voice_prompt_play(mix_tone);
    return;
}

bt_sink_srv_status_t bt_sink_app_voice_prompt_by_sink_event(bt_sink_srv_event_t event_id, void *parameters)
{
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)parameters;
    switch (event_id) {
        case BT_SINK_SRV_EVENT_STATE_CHANGE: 
        {
            bt_sink_srv_state_t curr_state = event->state_change.now;
            bt_sink_app_voice_prompt_report("state change, previous:0x%x, now:0x%x", event->state_change.previous, curr_state);
            if (curr_state == BT_SINK_SRV_STATE_STREAMING) {
            } else if (curr_state == BT_SINK_SRV_STATE_LOCAL_PLAYING) {
            } else if (curr_state == BT_SINK_SRV_STATE_MULTIPARTY) {
            } else if (curr_state == BT_SINK_SRV_STATE_HELD_REMAINING) {
            } else if (curr_state ==BT_SINK_SRV_STATE_HELD_ACTIVE) {
            }else  if (curr_state == BT_SINK_SRV_STATE_TWC_OUTGOING) {
            } else if (curr_state ==BT_SINK_SRV_STATE_TWC_INCOMING) {
            } else if (curr_state == BT_SINK_SRV_STATE_ACTIVE) {
            } else if (curr_state == BT_SINK_SRV_STATE_OUTGOING) {
            } else if (curr_state == BT_SINK_SRV_STATE_INCOMING) {
            }else  if (curr_state == BT_SINK_SRV_STATE_CONNECTED) {
                if(event->state_change.previous == BT_SINK_SRV_STATE_POWER_ON) {
                    //bt_sink_app_voice_prompt_play(VOICE_PROMPT_TONE_CONNECTED);
                } else {
                }
            } else if (curr_state == BT_SINK_SRV_STATE_POWER_ON) {
                if (event->state_change.previous != BT_SINK_SRV_STATE_NONE) {
                    //bt_sink_app_voice_prompt_play(VOICE_PROMPT_TONE_DISCONNECTED);
                } else {
                }
            } else if (curr_state == BT_SINK_SRV_STATE_LOCAL_PLAYING) {
                //bt_sink_app_voice_prompt_play(VOICE_PROMPT_TONE_MP3_MUSIC);
            } else if (curr_state == BT_SINK_SRV_STATE_NONE) {
            
            } else {

            }
        }
        break;
        
        case BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE:
        {
            bt_sink_app_report("[Sink] connection information, profile:0x%x", event->connection_info.profile_type);
            if (bt_sink_app_context.notify_connection_state) {
                if (bt_sink_app_get_connected_device(&(event->connection_info.bt_addr))) {
                    bt_sink_app_voice_prompt_play(VOICE_PROMPT_TONE_CONNECTED);
                } else {
                    bt_sink_app_voice_prompt_play(VOICE_PROMPT_TONE_DISCONNECTED);
                }
            }
        }
        break;
        
        case BT_SINK_SRV_EVENT_VISIBILITY_STATE_UPDATE:
        {
            bt_sink_app_voice_prompt_report("Visibility state, %d", event->visibility_state.visibility_on);
            if (event->visibility_state.visibility_on) {
                bt_sink_app_voice_prompt_play(VOICE_PROMPT_TONE_PAIRING);
            }
        }
        break;

        case BT_SINK_SRV_EVENT_VOICE_PROMPT:
        {
            bt_sink_app_voice_prompt_event_handle(event->voice_prompt.action);
        }
        break;

        case BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE:
        {
            bt_sink_app_voice_prompt_report("sco state, %d", event->hf_sco_state.state);
            if (bt_sink_app_voice_prompt_context.curr_action == BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH) {
                bt_sink_app_voice_prompt_play(VOICE_PROMPT_TONE_CALL_AUDIO_TRANSFER);
                bt_sink_app_voice_prompt_context.curr_action = BT_SINK_SRV_ACTION_ALL;
            }
        }
        break;

        
        default:
            break;
    }
    return BT_SINK_SRV_STATUS_SUCCESS;
}

void bt_sink_app_voice_prompt_by_battery(int32_t charger_exist, int32_t capacity)
{
    if (charger_exist) { //in charging
        if (capacity == 100) { //battery full
        } else {
        }  
    } else { //out charing
        if (capacity <= BT_SINK_APP_LOW_BATTERY_ALARM) {
            bt_sink_app_voice_prompt_play(VOICE_PROMPT_TONE_LOW_BATTERY);
        } else  {

        }
    }
    return;
}

void bt_sink_app_voice_prompt_by_sink_action(bt_sink_srv_action_t action)
{
    bt_sink_app_voice_prompt_tone_t mix_tone = VOICE_PROMPT_TONE_NUM;

    switch (action) {
        case BT_SINK_SRV_ACTION_INQUIRY: 
        {
            mix_tone = VOICE_PROMPT_TONE_PAIRING;
        }
        break;

        case BT_SINK_SRV_ACTION_RESET_TRUSTED_LIST: 
        {
            mix_tone = VOICE_PROMPT_TONE_RESET_PAIRED_LIST;
        }
        break;
        
        case BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH: 
        {
            //mix_tone = VOICE_PROMPT_TONE_CALL_AUDIO_TRANSFER;
            bt_sink_app_voice_prompt_context.curr_action = BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH;
        }
        break;

        case BT_SINK_SRV_ACTION_DIAL_LAST: 
        {
            mix_tone = VOICE_PROMPT_TONE_CALL_REDIALING;
        }
        break;
        
        case BT_SINK_SRV_ACTION_CALL_VOLUME_UP: 
        case BT_SINK_SRV_ACTION_CALL_VOLUME_DOWN :
        case BT_SINK_SRV_ACTION_VOLUME_UP:
        case BT_SINK_SRV_ACTION_VOLUME_DOWN:
        case AUDIO_PLAYER_ACTION_VOLUME_UP:
        case AUDIO_PLAYER_ACTION_VOLUME_DOWN:
        {
            mix_tone = VOICE_PROMPT_TONE_VOLUME_CHANGE;
        }
       
        default:
        break;
    }
   // LOG_E(VPM, "[VPM]mix_tone:0x%x", mix_tone);

    if (mix_tone != VOICE_PROMPT_TONE_NUM)
    bt_sink_app_voice_prompt_play(mix_tone);
    vTaskDelay(300);
    return;
}

bt_sink_srv_status_t bt_sink_srv_get_tone_parameter(bt_sink_srv_action_t action, bt_sink_srv_voice_prompt_tone_t *params)
{
    bt_sink_srv_status_t result = BT_SINK_SRV_STATUS_SUCCESS;
    
    switch (action) {
        case BT_SINK_SRV_ACTION_REJECT:
        {
            params->tone_type = (uint8_t)VPC_WAV;
            params->tone_buffer = (uint8_t *)bt_sink_app_voice_prompt_tone[VOICE_PROMPT_TONE_CALL_REFUSED].buf;
            params->tone_size = bt_sink_app_voice_prompt_tone[VOICE_PROMPT_TONE_CALL_REFUSED].size;
        }
        break;
            
        case BT_SINK_SRV_ACTION_HANG_UP:
        {
            params->tone_type = (uint8_t)VPC_WAV;
            params->tone_buffer = (uint8_t *)bt_sink_app_voice_prompt_tone[VOICE_PROMPT_TONE_CALL_END].buf;
            params->tone_size = bt_sink_app_voice_prompt_tone[VOICE_PROMPT_TONE_CALL_END].size;
        }
        break;

        case BT_SINK_SRV_ACTION_DIAL_LAST:
        {
            params->tone_type = (uint8_t)VPC_WAV;
            params->tone_buffer = (uint8_t *)bt_sink_app_voice_prompt_tone[VOICE_PROMPT_TONE_CALL_REDIALING].buf;
            params->tone_size = bt_sink_app_voice_prompt_tone[VOICE_PROMPT_TONE_CALL_REDIALING].size;
        }
        break;

        case BT_SINK_SRV_ACTION_POWER_OFF:
        {
            params->tone_type = (uint8_t)VPC_WAV;
            params->tone_buffer = (uint8_t *)bt_sink_app_voice_prompt_tone[VOICE_PROMPT_TONE_POWER_OFF].buf;
            params->tone_size = bt_sink_app_voice_prompt_tone[VOICE_PROMPT_TONE_POWER_OFF].size;
        }
        break;

        default:
        {
             result = BT_SINK_SRV_STATUS_FAIL;
        }
        break;
    }
    bt_sink_app_voice_prompt_report("action:0x%x, result, %d", action, result);
    return result;
}

#endif /*MTK_PROMPT_SOUND_ENABLE*/

