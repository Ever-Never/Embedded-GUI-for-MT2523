/* Copyright Statement:
 *
 * (C) 2005-2017  MediaTek Inc. All rights reserved.
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


#include "hal_audio.h"
#include "bt_sink_srv_audio_sync.h"
#include "bt_sink_srv_vp_pesudo_dev.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_resource.h"
#include "bt_sink_srv_utils.h"

#ifdef __BT_SINK_SRV_VOICE_PROMPT__

#include "prompt_control.h"

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_sink_srv_get_tone_parameter=_default_bt_sink_srv_get_tone_parameter")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_sink_srv_get_tone_parameter = default_bt_sink_srv_get_tone_parameter
#else
#error "Unsupported Platform"
#endif

bt_sink_srv_status_t default_bt_sink_srv_get_tone_parameter(bt_sink_srv_action_t action, bt_sink_srv_voice_prompt_tone_t *params)
{
    bt_sink_srv_report("[Sink][VP] not implement tone parameter.");
    return BT_SINK_SRV_STATUS_FAIL;
}

bt_sink_srv_vp_dev_context_t g_sink_srv_vp_dev_context;

void bt_sink_srv_voice_prompt_callback(prompt_control_event_t event_id)
{
    if (event_id == PROMPT_CONTROL_MEDIA_END) {
        bt_sink_srv_report("[Sink][VP] callback: playback complete");
        g_sink_srv_vp_dev_context.action = BT_SINK_SRV_ACTION_ALL;
        //audio_src_srv_update_state(g_sink_srv_vp_dev_context.audio_src, AUDIO_SRC_SRV_EVT_READY);
        audio_src_srv_update_state(g_sink_srv_vp_dev_context.audio_src, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
    }
    return;
}

void bt_sink_srv_vp_dev_set_action(bt_sink_srv_action_t action)
{
    bt_sink_srv_report("[Sink][VP] set action:0x%x",action);
    g_sink_srv_vp_dev_context.action = action;
}


void bt_sink_srv_vp_dev_play(audio_src_srv_handle_t *handle)
{
    //play voice tone;
    bt_sink_srv_report("[Sink][VP] dev play");
    bt_sink_srv_voice_prompt_tone_t params;
    if (BT_SINK_SRV_STATUS_SUCCESS == bt_sink_srv_get_tone_parameter(g_sink_srv_vp_dev_context.action, &params)) {
        #if 1
        prompt_control_stop_tone();
        if (prompt_control_play_tone((prompt_control_tone_type_t )params.tone_type, 
                                                         params.tone_buffer, 
                                                         params.tone_size, 
                                                         bt_sink_srv_voice_prompt_callback)) {
            audio_src_srv_update_state(g_sink_srv_vp_dev_context.audio_src, AUDIO_SRC_SRV_EVT_PLAYING);    
        } else {
            audio_src_srv_update_state(g_sink_srv_vp_dev_context.audio_src, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
        }
        #else
        audio_src_srv_update_state(g_sink_srv_vp_dev_context.audio_src, AUDIO_SRC_SRV_EVT_PLAYING);
        bt_sink_srv_vp_send_voice_prompt_event(g_sink_srv_vp_dev_context.action);
        #endif
    } else {
        audio_src_srv_update_state(g_sink_srv_vp_dev_context.audio_src, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
    }
    
    return;
}

void bt_sink_srv_vp_dev_reject(audio_src_srv_handle_t *handle)
{

}

void bt_sink_srv_vp_dev_suspend(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd)
{
    bt_sink_srv_report("[Sink][VP] dev suspend");
    prompt_control_stop_tone();
    audio_src_srv_update_state(g_sink_srv_vp_dev_context.audio_src, AUDIO_SRC_SRV_EVT_READY);
}

void bt_sink_srv_vp_dev_stop(audio_src_srv_handle_t *handle)
{
   bt_sink_srv_report("[Sink][VP] dev stop");
   audio_src_srv_update_state(g_sink_srv_vp_dev_context.audio_src, AUDIO_SRC_SRV_EVT_READY);
}

void bt_sink_srv_vp_dev_exception_handle(audio_src_srv_handle_t *handle, int32_t event, void *param)
{

}

void bt_sink_srv_vp_dev_init(void) 
{
    g_sink_srv_vp_dev_context.audio_src = audio_src_srv_construct_handle(AUDIO_SRC_SRV_PSEUDO_DEVICE_VP);
    g_sink_srv_vp_dev_context.audio_src->priority = AUDIO_SRC_SRV_PRIORITY_HIGH;
    g_sink_srv_vp_dev_context.audio_src->play = bt_sink_srv_vp_dev_play;
    g_sink_srv_vp_dev_context.audio_src->stop = bt_sink_srv_vp_dev_stop;
    g_sink_srv_vp_dev_context.audio_src->suspend = bt_sink_srv_vp_dev_suspend;
    g_sink_srv_vp_dev_context.audio_src->reject = bt_sink_srv_vp_dev_reject;
    g_sink_srv_vp_dev_context.audio_src->exception_handle = bt_sink_srv_vp_dev_exception_handle;
    
    audio_src_srv_update_state(g_sink_srv_vp_dev_context.audio_src, AUDIO_SRC_SRV_EVT_READY);
    return;
}

audio_src_srv_handle_t * bt_sink_srv_vp_dev_get_audio_handle(void)
 {
    return g_sink_srv_vp_dev_context.audio_src;
} 

void bt_sink_srv_vp_send_voice_prompt_event(bt_sink_srv_action_t action)
{
    bt_sink_srv_event_param_t event;
    event.voice_prompt.action = action;
    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_VOICE_PROMPT, &event);
    return;
}

void bt_sink_srv_voice_prompt_reset(void)
{
    audio_src_srv_update_state(g_sink_srv_vp_dev_context.audio_src, AUDIO_SRC_SRV_EVT_READY);
    return;
}

#endif /*__BT_SINK_SRV_VOICE_PROMPT__*/

