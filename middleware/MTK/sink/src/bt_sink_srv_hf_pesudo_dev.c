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


#include "audio_src_srv.h"
#include "bt_sink_srv_ami.h"
#include "bt_sink_srv_hf.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_vp_pesudo_dev.h"

static void bt_sink_srv_hf_audio_am_callback(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *parm)
{
    //uint8_t current_id;
    bt_sink_srv_report("[Sink][HF][AM] cb, aud_id:%x, msg_id:%x, sub_msg:%x", aud_id, msg_id, sub_msg);
}

void bt_sink_srv_hf_dev_play(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_am_result_t result = AUD_EXECUTION_FAIL;
    bt_bd_addr_t *address_p = NULL;
#ifdef __BT_SINK_SRV_AM_SUPPORT__
    bt_sink_srv_am_audio_capability_t audio_capability;
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p;
    bt_sink_srv_hf_link_context_t *link_cntx = NULL;
    bt_sink_srv_hf_context_p = (bt_sink_srv_hf_context_t *)bt_sink_srv_hf_get_device(handle);

    link_cntx = &bt_sink_srv_hf_context_p->link;
    bt_sink_srv_assert(bt_sink_srv_hf_context_p);

    bt_sink_srv_report("[Sink][HF][Audio] bt_sink_srv_hf_dev_play 0x%x", link_cntx->audio_type);

    bt_sink_srv_memset(&audio_capability, 0, sizeof(bt_sink_srv_am_audio_capability_t));

    address_p = bt_hfp_get_bd_addr_by_handle(link_cntx->handle);
    bt_sink_srv_memcpy((void *)&audio_capability.dev_addr, (void *)address_p, sizeof(bt_bd_addr_t));
    
  
    //link_cntx->audio_id = bt_sink_srv_ami_audio_open(AUD_HIGH, bt_sink_srv_hf_audio_am_callback);

    if (BT_SINK_SRV_HF_AUDIO_SCO == link_cntx->audio_type) {
        bt_sink_srv_hf_audio_sco_parameter_init(&audio_capability, (bt_hfp_audio_codec_type_t)link_cntx->codec);
    } else if (BT_SINK_SRV_HF_AUDIO_RING == link_cntx->audio_type) {
        bt_sink_srv_hf_audio_pcm_parameter_init(&audio_capability);
    } else if (BT_SINK_SRV_HF_AUDIO_NULL == link_cntx->audio_type)  {
        audio_capability.type = NONE;
    }
    result = bt_sink_srv_ami_audio_play(link_cntx->audio_id, &audio_capability);  
#endif /* __BT_SINK_SRV_AM_SUPPORT__ */
    bt_sink_srv_report("[Sink][HF][Audio] play result:%d", result);

    if (result == AUD_EXECUTION_FAIL) {
        bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_STOP);
        link_cntx->audio_type = BT_SINK_SRV_HF_AUDIO_NONE;
    } else {
        bt_sink_srv_hf_dev_set_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_STATE_PLAYING);
        bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_NONE);
    }
}


void bt_sink_srv_hf_dev_reject(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p;
    
    bt_sink_srv_hf_context_p = (bt_sink_srv_hf_context_t *)bt_sink_srv_hf_get_device(handle);
    bt_sink_srv_assert(bt_sink_srv_hf_context_p);

    bt_sink_srv_report("[Sink][HF][Audio] bt_sink_srv_hf_dev_reject 0x%x", bt_sink_srv_hf_context_p->link.audio_type);
    bt_sink_srv_hf_context_p->link.audio_type = BT_SINK_SRV_HF_AUDIO_NONE;

    bt_sink_srv_hf_dev_set_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_STATE_READY);
    bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_NONE);
}

void bt_sink_srv_hf_dev_suspend(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd)
{
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p;
    
    bt_sink_srv_hf_context_p = (bt_sink_srv_hf_context_t *)bt_sink_srv_hf_get_device(handle);
    bt_sink_srv_assert(bt_sink_srv_hf_context_p);

    bt_sink_srv_report("[Sink][HF][Audio] bt_sink_srv_hf_dev_suspend 0x%x", bt_sink_srv_hf_context_p->link.audio_type);

    bt_sink_srv_hf_dev_stop_int(bt_sink_srv_hf_context_p, bt_sink_srv_hf_context_p->link.audio_type);

    bt_sink_srv_hf_dev_set_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_STATE_READY);
    bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_NONE);
}

void bt_sink_srv_hf_dev_stop(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = NULL;

    bt_sink_srv_hf_context_p = (bt_sink_srv_hf_context_t *)bt_sink_srv_hf_get_device(handle);

    if (NULL != bt_sink_srv_hf_context_p) {
        bt_sink_srv_am_result_t result = AUD_EXECUTION_FAIL;
        bt_sink_srv_hf_link_context_t *link_cntx = &bt_sink_srv_hf_context_p->link;

        bt_sink_srv_report("[Sink][HF][Audio] stop  type:0x%x", link_cntx->audio_type);

        link_cntx->audio_type = BT_SINK_SRV_HF_AUDIO_NONE;

        result = bt_sink_srv_ami_audio_stop(link_cntx->audio_id);
       // result = bt_sink_srv_ami_audio_close(link_cntx->audio_id);
       // link_cntx->audio_id = BT_SINK_SRV_HF_INVALID_AUDIO_ID;
         bt_sink_srv_report("[Sink][HF][Audio] stop result:%d", result);

        if (bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_DESTROY_CONTEXT) {
            bt_sink_srv_hf_dev_set_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_STATE_READY);
            /* The ready is to reply aud_src srv's stop status, none is to set the final status.
             * It better to move this code to disconnect ind because the stop is called after 
             * the prepare stop status set */
            bt_sink_srv_hf_dev_set_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_STATE_NONE);
            bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_NONE);
            bt_sink_srv_hf_reset_link_context(&(bt_sink_srv_hf_context_p->link));
        } else {
        #ifdef __BT_SINK_SRV_VOICE_PROMPT__
            audio_src_srv_handle_t * vp_handle = bt_sink_srv_vp_dev_get_audio_handle();
            audio_src_srv_add_waiting_list(vp_handle);
        #endif
            bt_sink_srv_hf_dev_set_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_STATE_READY);
            
            bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_NONE);
        }

    }
}

void bt_sink_srv_hf_dev_exception_handle(audio_src_srv_handle_t *handle, int32_t event, void *param)
{
}


void bt_sink_srv_hf_dev_stop_int(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p, bt_sink_srv_hf_audio_type_t audio_type)
{
    if (NULL != bt_sink_srv_hf_context_p) {
        bt_sink_srv_hf_link_context_t *link_cntx = &bt_sink_srv_hf_context_p->link;
        bt_sink_srv_report("[Sink][HF][Audio] stop audio type, origin:%d, new:%d", link_cntx->audio_type, audio_type);
        if ((audio_type == BT_SINK_SRV_HF_AUDIO_TOTAL) ||(link_cntx->audio_type == audio_type) ) {
            link_cntx->audio_type = BT_SINK_SRV_HF_AUDIO_NONE;
            if (link_cntx->audio_id !=  BT_SINK_SRV_HF_INVALID_AUDIO_ID) {
                bt_sink_srv_am_result_t result = bt_sink_srv_ami_audio_stop(link_cntx->audio_id);
                bt_sink_srv_report("[Sink][HF][Audio] stop result:%d", result);
            }
        }
    }
}

void bt_sink_srv_hf_dev_play_int(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p, bt_sink_srv_hf_audio_type_t audio_type)
{
    bt_sink_srv_am_result_t result = AUD_EXECUTION_FAIL;
    bt_bd_addr_t *address_p = NULL;
#ifdef __BT_SINK_SRV_AM_SUPPORT__
    bt_sink_srv_am_audio_capability_t audio_capability;
    bt_sink_srv_hf_link_context_t *link_cntx = NULL;
    bt_sink_srv_memset(&audio_capability, 0, sizeof(bt_sink_srv_am_audio_capability_t));

    bt_sink_srv_assert(bt_sink_srv_hf_context_p);
    link_cntx = &bt_sink_srv_hf_context_p->link;

    bt_sink_srv_report("[Sink][HF][Audio] audio type: origin: %d, new: %d", link_cntx->audio_type, audio_type);
    if (link_cntx->audio_id !=  BT_SINK_SRV_HF_INVALID_AUDIO_ID) {
        if (link_cntx->audio_type != BT_SINK_SRV_HF_AUDIO_NONE) {
             bt_sink_srv_report("[Sink][HF][Audio] audio id: %d", link_cntx->audio_id);
            result = bt_sink_srv_ami_audio_stop(link_cntx->audio_id);
            bt_sink_srv_report("[Sink][HF][Audio] stop result: %d", result);
            if (result == AUD_EXECUTION_SUCCESS) {
                link_cntx->audio_type = BT_SINK_SRV_HF_AUDIO_NONE;
            }
        }
        address_p = bt_hfp_get_bd_addr_by_handle(link_cntx->handle);
        bt_sink_srv_memcpy((void *)&audio_capability.dev_addr, (void *)address_p, sizeof(bt_bd_addr_t));
        if (BT_SINK_SRV_HF_AUDIO_SCO == audio_type) {
            bt_sink_srv_hf_audio_sco_parameter_init(&audio_capability, (bt_hfp_audio_codec_type_t)link_cntx->codec);
        } else if (BT_SINK_SRV_HF_AUDIO_RING == audio_type) {
            bt_sink_srv_hf_audio_pcm_parameter_init(&audio_capability);
        } else if (BT_SINK_SRV_HF_AUDIO_NULL == audio_type)  {
            audio_capability.type = NONE;
        }
        result = bt_sink_srv_ami_audio_play(link_cntx->audio_id, &audio_capability);
        if (result == AUD_EXECUTION_SUCCESS) {
            link_cntx->audio_type = audio_type;
        }
    }
#endif /* __BT_SINK_SRV_AM_SUPPORT__ */
    bt_sink_srv_report("[Sink][HF][Audio] play result:%d", result);
}

void bt_sink_srv_hf_dev_codec_open(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p)
{
    bt_sink_srv_hf_context_p->link.audio_id =  bt_sink_srv_ami_audio_open(AUD_HIGH, bt_sink_srv_hf_audio_am_callback);
    bt_sink_srv_report("[Sink][HF] [Audio] Open audio id :0x%x", bt_sink_srv_hf_context_p->link.audio_id);
    bt_sink_srv_assert(bt_sink_srv_hf_context_p->link.audio_id != BT_SINK_SRV_HF_INVALID_AUDIO_ID);
    return;
}

void bt_sink_srv_hf_dev_codec_close(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p)
{
    bt_sink_srv_am_result_t result = bt_sink_srv_ami_audio_close(bt_sink_srv_hf_context_p->link.audio_id);
    bt_sink_srv_report("[Sink][HF] [Audio] Close result :0x%x", result);
    bt_sink_srv_hf_context_p->link.audio_id = BT_SINK_SRV_HF_INVALID_AUDIO_ID;
}

bool bt_sink_srv_hf_dev_check_sub_state_valid(
    bt_sink_srv_hf_dev_state state, 
    bt_sink_srv_hf_dev_sub_state sub_state)
{
    bool valid = true;
    switch (state) {
        case BT_SINK_SRV_HF_DEV_STATE_NONE: {
            if (sub_state != BT_SINK_SRV_HF_DEV_SUB_STATE_CONNECTING &&
                sub_state != BT_SINK_SRV_HF_DEV_SUB_STATE_NONE) {
                valid = false;
            }
            break;
        }
        case BT_SINK_SRV_HF_DEV_STATE_READY: {
            if (sub_state != BT_SINK_SRV_HF_DEV_SUB_STATE_DISCONNECTING &&
                sub_state != BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_PLAY &&
                sub_state != BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_STOP &&
                sub_state != BT_SINK_SRV_HF_DEV_SUB_STATE_NONE) {
                valid = false;
            }
            break;
        }
        case BT_SINK_SRV_HF_DEV_STATE_PLAYING: {
            if (sub_state != BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_STOP &&
                sub_state != BT_SINK_SRV_HF_DEV_SUB_STATE_NONE && 
                sub_state != BT_SINK_SRV_HF_DEV_SUB_STATE_DISCONNECTING) {
                valid = false;
            }
            break;
        }
        default : {
            valid = false;
            break;
        }
    }

    return valid;
}

void bt_sink_srv_hf_dev_set_state(bt_sink_srv_hf_context_t *hf_dev, bt_sink_srv_hf_dev_state state)
{
    audio_src_srv_handle_t *audio_handle = hf_dev->audio_src;
    audio_src_srv_event_t audio_src_evt = AUDIO_SRC_SRV_EVT_UNAVAILABLE;

    bt_sink_srv_report("[sink][hf]dev_set_state,ori:%d, curr:%d", hf_dev->dev_state, state);

    hf_dev->dev_state = state;

    switch (state) {
        case BT_SINK_SRV_HF_DEV_STATE_READY: {
            audio_src_evt = AUDIO_SRC_SRV_EVT_READY;
            break;
        }
        
        case BT_SINK_SRV_HF_DEV_STATE_PLAYING: {
            audio_src_evt = AUDIO_SRC_SRV_EVT_PLAYING;
            break;
        }
        
        case BT_SINK_SRV_HF_DEV_STATE_NONE: {
            bt_sink_srv_hf_dev_codec_close(hf_dev);
            audio_src_evt = AUDIO_SRC_SRV_EVT_UNAVAILABLE;
            break;
        }
        
        default: {
            audio_src_evt = AUDIO_SRC_SRV_EVT_UNAVAILABLE;
            break;
        }
    }
    audio_src_srv_update_state(audio_handle, audio_src_evt);
}


void bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_t *hf_dev, bt_sink_srv_hf_dev_sub_state sub_state)
{
    audio_src_srv_handle_t *audio_handle = hf_dev->audio_src;
    bool state_valid = false;

    bt_sink_srv_report("[sink][hf]dev_set_sub_state,ori:%d, curr:%d", hf_dev->dev_sub_state, sub_state);

    if (hf_dev->dev_sub_state == sub_state) {
        return;
    }

    state_valid = bt_sink_srv_hf_dev_check_sub_state_valid(hf_dev->dev_state, sub_state);
    if (!state_valid) {
        bt_sink_srv_report("[sink][hf]Error sub state,dev_state:%d, sub_state:%d", hf_dev->dev_state, sub_state);
        return;
    }
    //bt_sink_srv_assert(state_valid);
    
    hf_dev->dev_sub_state = sub_state;

    switch (hf_dev->dev_sub_state) {
        case BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_PLAY: {
            audio_src_srv_update_state(audio_handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
            break;
        }
        case BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_STOP: {
            audio_src_srv_update_state(audio_handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            break;
        }
    }

}




