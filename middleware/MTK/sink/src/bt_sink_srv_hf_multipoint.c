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

#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_hf.h"
#include "bt_sink_srv_conmgr.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_sink_srv_resource.h"
#ifdef __BT_AWS_SUPPORT__
#include "bt_sink_srv_aws_system.h"
#endif

extern bt_sink_srv_hf_context_t g_sink_srv_hf_context[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
static bt_sink_srv_hf_context_t *g_sink_srv_hf_hightlight_p;

extern void bt_sink_srv_hf_dev_play_int(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p, bt_sink_srv_hf_audio_type_t audio_type);

static bt_status_t bt_sink_srv_hf_set_audio_status(uint32_t handle, bt_hfp_audio_status_t status);


static void bt_sink_srv_hf_state_set_and_sync(bt_sink_srv_state_t state)
{
    bt_sink_srv_state_set(state);
    #ifdef __BT_AWS_SUPPORT__
    bt_sink_srv_aws_system_send_sink_state(state);
    #endif
    return;
}

void bt_sink_srv_hf_set_highlight_device(bt_sink_srv_hf_context_t *device)
{
    bt_sink_srv_hf_context_t *hilight = g_sink_srv_hf_hightlight_p;

    bt_sink_srv_report("[Sink][HF][MP] set highlight, hilight_hd:0x%x, device_hd:0x%x",
                       (hilight == NULL ? 0 : hilight->link.handle),
                       (device == NULL ? 0 : device->link.handle));

    g_sink_srv_hf_hightlight_p = device;
    bt_sink_srv_hf_mp_set_sco(hilight, device);
    bt_sink_srv_report("[Sink][HF][MP] set done, hilight:0x%x",
                       (g_sink_srv_hf_hightlight_p == NULL ? NULL : g_sink_srv_hf_hightlight_p));}

bt_sink_srv_hf_context_t *bt_sink_srv_hf_get_highlight_device(void)
{
    return g_sink_srv_hf_hightlight_p;
}

void bt_sink_srv_hf_mp_state_change(bt_sink_srv_hf_context_t *device)
{
    if (device) {
        bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = NULL;

        if (NULL == bt_sink_srv_hf_get_highlight_device()) {
            bt_sink_srv_hf_set_highlight_device(device);
        }

        switch (device->link.call_state) {
            case BT_SINK_SRV_HF_CALL_STATE_IDLE: {
                bt_sink_srv_hf_context_p =
                    bt_sink_srv_hf_find_device_by_call_state(BT_SINK_SRV_HF_CALL_STATE_ALL);

                if (NULL != bt_sink_srv_hf_context_p) {
                    if (BT_SINK_SRV_HF_CALL_STATE_IDLE != bt_sink_srv_hf_context_p->link.call_state) {

                        bt_sink_srv_hf_set_highlight_device(bt_sink_srv_hf_context_p);

                        if (bt_sink_srv_hf_context_p->link.call_state & BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING) {
                            BT_SINK_SRV_HF_HOLD_CALL(bt_sink_srv_hf_context_p->link.handle, BT_SINK_SRV_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER);
                        }
                        bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)bt_sink_srv_hf_context_p->link.call_state);
                        break;
                    }
                } else {
                    bt_sink_srv_hf_set_highlight_device(NULL);
                    bt_sink_srv_hf_state_set_and_sync(BT_SINK_SRV_STATE_CONNECTED);
                }
                device->link.codec = BT_HFP_CODEC_TYPE_NONE;
            }
            break;

            case BT_SINK_SRV_HF_CALL_STATE_INCOMING: {
                bt_sink_srv_hf_context_p =
                    bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                            (BT_SINK_SRV_HF_CALL_STATE_OUTGOING | BT_SINK_SRV_HF_CALL_STATE_ACTIVE |
                             BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING | BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING |
                             BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE | BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING |
                             BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING));

                if (NULL != bt_sink_srv_hf_context_p) {
                    bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING);
                } else {
                    bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)device->link.call_state);
                }
            }
            break;

            case BT_SINK_SRV_HF_CALL_STATE_OUTGOING: {
                bt_sink_srv_hf_context_p =
                    bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                            (BT_SINK_SRV_HF_CALL_STATE_INCOMING | BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING));

                if (NULL != bt_sink_srv_hf_context_p) {
                    bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING);
                } else {
                    bt_sink_srv_hf_context_p =
                        bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                                (BT_SINK_SRV_HF_CALL_STATE_ACTIVE | BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING |
                                 BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE | BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING |
                                 BT_SINK_SRV_HF_CALL_STATE_MULTIPARTY));
                    if (NULL != bt_sink_srv_hf_context_p) {
                        bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING);
                    } else {
                        bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)device->link.call_state);
                    }
                }
            }
            break;

            case BT_SINK_SRV_HF_CALL_STATE_ACTIVE: {
                bt_sink_srv_hf_context_p =
                    bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                            (BT_SINK_SRV_HF_CALL_STATE_INCOMING | BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING));

                if (NULL != bt_sink_srv_hf_context_p) {
                    bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING);
                } else {
                    bt_sink_srv_hf_context_p =
                        bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                                (BT_SINK_SRV_HF_CALL_STATE_OUTGOING | BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING));
                    if (NULL != bt_sink_srv_hf_context_p) {
                        bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING);
                    } else {
                        bt_sink_srv_hf_context_p =
                            bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                                    (BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING));
                        if (NULL != bt_sink_srv_hf_context_p) {
                            bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE);
                        } else {              
                            // when call state change from hold to active, should active SCO
                            if (device == bt_sink_srv_hf_get_highlight_device()) {
                                if ((device->link.flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) &&
                                        (!(device->link.flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE))) {
                                    device->link.flag |= BT_SINK_SRV_HF_FLAG_SCO_ACTIVE;
                                    bt_sink_srv_hf_set_audio_status(device->link.handle, BT_HFP_AUDIO_STATUS_ACTIVE);
                                }
                            }                 
                            bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)device->link.call_state);
                        }
                    }
                }
            }
            break;

            case BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING: {
                bt_sink_srv_hf_context_p =
                    bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                            (BT_SINK_SRV_HF_CALL_STATE_INCOMING | BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING));

                if (NULL != bt_sink_srv_hf_context_p) {
                    bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING);
                } else {
                    bt_sink_srv_hf_context_p =
                        bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                                (BT_SINK_SRV_HF_CALL_STATE_OUTGOING | BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING));
                    if (NULL != bt_sink_srv_hf_context_p) {
                        bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING);
                    } else {
                        bt_sink_srv_hf_context_p =
                            bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                                    (BT_SINK_SRV_HF_CALL_STATE_ACTIVE));
                        if (NULL != bt_sink_srv_hf_context_p) {
                            bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE);
                        } else {
                            bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)device->link.call_state);
                        }
                    }
                }
            }
            break;

            default: {
                bt_sink_srv_hf_state_set_and_sync((bt_sink_srv_state_t)device->link.call_state);
            }
            break;
        }
    }
}

void bt_sink_srv_hf_mp_answer(bt_sink_srv_hf_context_t *current_device, bool accept)
{
    bt_sink_srv_hf_context_t *highlight = bt_sink_srv_hf_get_highlight_device();

    bt_sink_srv_report("[Sink][HF][MP] hiligh_hd:0x%x, curr_hd:0x%x", highlight->link.handle, current_device->link.handle);

    if (current_device != highlight) {
        if (accept && (highlight->link.call_state & BT_SINK_SRV_HF_CALL_STATE_ACTIVE)) {
            BT_SINK_SRV_HF_HOLD_CALL(highlight->link.handle, BT_SINK_SRV_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER);
        }

        if (accept) {
            bt_sink_srv_hf_set_highlight_device(current_device);
            BT_SINK_SRV_HF_ANSWER_CALL(current_device->link.handle);
        } else {
            current_device->link.flag |= BT_SINK_SRV_HF_FLAG_USER_REJECT;
            BT_SINK_SRV_HF_REJECT_CALL(current_device->link.handle);
        }
    }
}

void bt_sink_srv_hf_mp_swap(bt_sink_srv_hf_context_t *active_device, bt_sink_srv_hf_context_t *held_device)
{
    bt_sink_srv_report("[Sink][HF][MP] active:0x%x, held:0x%x", active_device->link.handle, held_device->link.handle);

    if (active_device->link.call_state & BT_SINK_SRV_HF_CALL_STATE_ACTIVE) {
        BT_SINK_SRV_HF_HOLD_CALL(active_device->link.handle, BT_SINK_SRV_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER);
    }

    if (held_device->link.call_state & BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING) {
        BT_SINK_SRV_HF_HOLD_CALL(held_device->link.handle, BT_SINK_SRV_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER);
    }
    bt_sink_srv_hf_set_highlight_device(held_device);
}

void bt_sink_srv_hf_mp_switch_audio(void)
{
    bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
    uint32_t count = bt_sink_srv_cm_get_connected_device(BT_SINK_SRV_PROFILE_HFP, addr_list);
    uint16_t i;

    for (i = 0; i < count; i++) {
        bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = (bt_sink_srv_hf_context_t *)bt_sink_srv_cm_get_profile_info(&addr_list[i], BT_SINK_SRV_PROFILE_HFP);

        if (NULL != bt_sink_srv_hf_context_p) {
            if (bt_sink_srv_hf_context_p != bt_sink_srv_hf_get_highlight_device()) {
                bt_sink_srv_hf_set_highlight_device(bt_sink_srv_hf_context_p);
                break;
            }
        }
    }
}

static bt_status_t bt_sink_srv_hf_set_audio_status(uint32_t handle, bt_hfp_audio_status_t status)
{
    bt_sink_srv_report("[Sink][HF][MP] set audio, handle:0x%x, status:0x%x", handle, status);
    return bt_hfp_set_audio_status(handle, status);
}

void bt_sink_srv_hf_mp_set_sco(bt_sink_srv_hf_context_t *highlight, bt_sink_srv_hf_context_t *device)
{


    if (NULL != device) {
        bt_sink_srv_hf_link_context_t *link_cntx = &device->link;
        bt_sink_srv_hf_link_context_t *hilit_cntx = &highlight->link;
        if ( NULL != highlight) {
            bt_sink_srv_report("[Sink][HF][MP] hilight_hd:0x%x, device_hd:0x%x, hilight_flag:0x%x, device_flag:0x%x",
                               hilit_cntx->handle, link_cntx->handle, hilit_cntx->flag, link_cntx->flag);

            if (device == highlight) {
                if ((link_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) &&
                        (!(link_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE))) {
                    link_cntx->flag |= BT_SINK_SRV_HF_FLAG_SCO_ACTIVE;
                    bt_sink_srv_hf_set_audio_status(link_cntx->handle, BT_HFP_AUDIO_STATUS_ACTIVE);
                }
            } else {
                if ((hilit_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) &&
                        (hilit_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE)) {
                    hilit_cntx->flag &= (~(BT_SINK_SRV_HF_FLAG_SCO_ACTIVE));
                    if (highlight->dev_state == BT_SINK_SRV_HF_DEV_STATE_PLAYING ||
                        highlight->dev_sub_state == BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_PLAY) {
                        bt_sink_srv_hf_dev_play_int(highlight, BT_SINK_SRV_HF_AUDIO_NULL);
                    }
                    bt_sink_srv_hf_set_audio_status(hilit_cntx->handle, BT_HFP_AUDIO_STATUS_INACTIVE);
                } else if ((link_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) &&
                           (!(link_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE))) {
                    link_cntx->flag |= BT_SINK_SRV_HF_FLAG_SCO_ACTIVE;
                    bt_sink_srv_hf_set_audio_status(link_cntx->handle, BT_HFP_AUDIO_STATUS_ACTIVE);
                }
            }
        }else {
            if ((link_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) &&
                        (!(link_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE))) {
                    link_cntx->flag |= BT_SINK_SRV_HF_FLAG_SCO_ACTIVE;
                    bt_sink_srv_hf_set_audio_status(link_cntx->handle, BT_HFP_AUDIO_STATUS_ACTIVE);
            }
        }
    }
}

