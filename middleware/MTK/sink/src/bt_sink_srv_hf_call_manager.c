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

#include <stdbool.h>
#include "bt_hfp.h"
#include "bt_sink_srv_hf.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_conmgr.h"
#include "bt_sink_srv_common.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_state_notify.h"

extern bt_sink_srv_hf_context_t g_sink_srv_hf_context[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
extern bt_sink_srv_hf_context_t *g_sink_srv_hf_missed_call_device_p;

static bool bt_sink_srv_hf_is_feature_support(bt_sink_srv_action_t action, bt_sink_srv_hf_context_t *device)
{
    bool result = false;
    bt_sink_srv_hf_link_context_t * link_cntx = &(device->link);
    switch (action) {
        case BT_SINK_SRV_ACTION_QUERY_CALL_LIST:
            if (link_cntx->ag_featues & (BT_HFP_AG_FEATURE_3_WAY | BT_HFP_AG_FEATURE_ENHANCED_CALL_STATUS)) {
                result = true;
            }
            break;

        case BT_SINK_SRV_ACTION_3WAY_RELEASE_ALL_HELD:
            if ((link_cntx->ag_featues & (BT_HFP_AG_FEATURE_3_WAY | BT_HFP_AG_FEATURE_ENHANCED_CALL_STATUS))
                    && (link_cntx->ag_chld_feature & BT_HFP_AG_HOLD_FEATURE_RELEASE_HELD_CALL)) {
                result = true;
            }
            break;

        case BT_SINK_SRV_ACTION_3WAY_RELEASE_ACTIVE_ACCEPT_OTHER:
            if ((link_cntx->ag_featues & (BT_HFP_AG_FEATURE_3_WAY | BT_HFP_AG_FEATURE_ENHANCED_CALL_STATUS))
                    && (link_cntx->ag_chld_feature & BT_HFP_AG_HOLD_FEATURE_RELEASE_ACTIVE_CALL)) {
                result = true;
            }
            break;

        case BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER:
            if ((link_cntx->ag_featues & (BT_HFP_AG_FEATURE_3_WAY | BT_HFP_AG_FEATURE_ENHANCED_CALL_STATUS))
                    && (link_cntx->ag_chld_feature & BT_HFP_AG_HOLD_FEATURE_HOLD_ACTIVE_CALL)) {
                result = true;
            }
            break;

        case BT_SINK_SRV_ACTION_3WAY_RELEASE_SPECIAL:
            if ((link_cntx->ag_featues & (BT_HFP_AG_FEATURE_3_WAY | BT_HFP_AG_FEATURE_ENHANCED_CALL_CTRL))
                    && (link_cntx->ag_chld_feature & BT_HFP_AG_HOLD_FEATURE_RELEASE_SPECIFIC_CALL)) {
                result = true;
            }
            break;

        case BT_SINK_SRV_ACTION_3WAY_HOLD_SPECIAL:
            if ((link_cntx->ag_featues & (BT_HFP_AG_FEATURE_3_WAY | BT_HFP_AG_FEATURE_ENHANCED_CALL_CTRL))
                    && (link_cntx->ag_chld_feature & BT_HFP_AG_HOLD_FEATURE_HOLD_SPECIFIC_CALL)) {
                result = true;
            }
            break;


        case BT_SINK_SRV_ACTION_3WAY_ADD_HELD_CALL_TO_CONVERSATION:
            if (link_cntx->ag_featues & BT_HFP_AG_FEATURE_3_WAY &&
                    link_cntx->ag_chld_feature & BT_HFP_AG_HOLD_FEATURE_ADD_HELD_CALL) {
                result = true;
            }
            break;

        case BT_SINK_SRV_ACTION_3WAY_EXPLICIT_CALL_TRANSFER:
            if ((link_cntx->ag_featues & BT_HFP_AG_FEATURE_3_WAY) &&
                    (link_cntx->ag_chld_feature & BT_HFP_AG_HOLD_FEATURE_CALL_TRANSFER)) {
                result = true;
            }
            break;

        case BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE:
            if (link_cntx->ag_featues & BT_HFP_AG_FEATURE_VOICE_RECOGNITION) {
                result = true;
            }
            break;

        default:
            break;
    }
    return result;
}

void bt_sink_srv_hf_call_set_state(bt_sink_srv_hf_context_t *link, bt_sink_srv_hf_call_state_t new_state)
{
    if (link) {
        bt_sink_srv_hf_link_context_t *cur_link = &link->link;
        bt_sink_srv_report("[Sink][HF] previous:0x%x, now:0x%x", cur_link->call_state, new_state);
        if (cur_link->call_state != new_state) {
            bt_sink_srv_hf_call_state_t previous = cur_link->call_state;
            cur_link->call_state = new_state;
            bt_sink_srv_hf_mp_state_change(link);
            bt_sink_srv_hf_call_state_change(link, previous, new_state);
        }
    }
}

void bt_sink_srv_hf_handle_setup_ind(bt_sink_srv_hf_context_t *link, bt_hfp_ciev_call_setup_state_t setup)
{
    if (link) {
        bt_sink_srv_hf_link_context_t *cur_link = &link->link;

        switch (setup) {
            case BT_HFP_CIEV_CALL_SETUP_STATE_NONE:
                switch (cur_link->call_state) {
                    case BT_SINK_SRV_HF_CALL_STATE_INCOMING:
                    case BT_SINK_SRV_HF_CALL_STATE_OUTGOING:
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_IDLE);
                        break;

                    case BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING:
                        BT_SINK_SRV_HF_QUERY_CALL_LIST(cur_link->handle);
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_ACTIVE);
                        break;

                    case BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING:
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE);
                        break;

                    default:
                        break;
                }
                break;

            case BT_HFP_CIEV_CALL_SETUP_STATE_INCOMING:
                switch (cur_link->call_state) {
                    case BT_SINK_SRV_HF_CALL_STATE_IDLE:
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_INCOMING);
                        break;

                    case BT_SINK_SRV_HF_CALL_STATE_ACTIVE:
                    case BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE:
                    case BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING:
                    case BT_SINK_SRV_HF_CALL_STATE_MULTIPARTY:
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING);
                        break;

                    default:
                        break;
                }
                break;

            case BT_HFP_CIEV_CALL_SETUP_STATE_OUTGOING:
            case BT_HFP_CIEV_CALL_SETUP_STATE_REMOTE_ALERT:
                switch (cur_link->call_state) {
                    case BT_SINK_SRV_HF_CALL_STATE_IDLE:
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_OUTGOING);
                        break;

                    case BT_SINK_SRV_HF_CALL_STATE_ACTIVE:
                    case BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE:
                    case BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING:
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING);
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}


void bt_sink_srv_hf_handle_call_ind(bt_sink_srv_hf_context_t *link, bt_hfp_ciev_call_state_t call)
{
    if (link) {
        bt_sink_srv_hf_link_context_t *cur_link = &link->link;

        switch (call) {
            case BT_HFP_CIEV_CALL_STATE_NONE:
                switch (cur_link->call_state) {
                    case BT_SINK_SRV_HF_CALL_STATE_IDLE:
                        break;

                    case BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING:
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_INCOMING);
                        break;

                    case BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING:
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_OUTGOING);
                        break;

                    default:
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_IDLE);
                        break;
                }
                break;

            case BT_HFP_CIEV_CALL_STATE_CALL:
                switch (cur_link->call_state) {
                    case BT_SINK_SRV_HF_CALL_STATE_IDLE:
                    case BT_SINK_SRV_HF_CALL_STATE_INCOMING:
                    case BT_SINK_SRV_HF_CALL_STATE_OUTGOING:
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_ACTIVE);
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}


static void bt_sink_srv_hf_wait_call_ind_timeout_handler(void *params)
{
    bt_sink_srv_hf_context_t *hf_contex = (bt_sink_srv_hf_context_t *)params;
    bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_HF_WAIT_CALL_IND);
    bt_sink_srv_report("[Sink][HF] Call ind timeout, hf_contex:0x%x",hf_contex);

    if (NULL != hf_contex ) {
        bt_sink_srv_report("[Sink][HF] Call ind timeout, call state:0x%x",hf_contex->link.call_state );
        if (hf_contex->link.call_state == BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING) {
            bt_sink_srv_hf_call_set_state(hf_contex, BT_SINK_SRV_HF_CALL_STATE_ACTIVE);
        }
    }
    return;
}

void bt_sink_srv_hf_handle_held_ind(bt_sink_srv_hf_context_t *link, bt_hfp_ciev_call_hold_state_t held)
{
    if (link) {
        bt_sink_srv_hf_link_context_t *cur_link = &link->link;

        switch (held) {
            case BT_HFP_CIEV_CALL_HOLD_STATE_NONE:
                switch (cur_link->call_state) {
                    case BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING:
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_OUTGOING);
                        break;

                    case BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING:
                        //bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_ACTIVE);
                        bt_sink_srv_timer_start(BT_SINK_SRV_TIMER_HF_WAIT_CALL_IND,
                                            50,
                                            bt_sink_srv_hf_wait_call_ind_timeout_handler,
                                            (void*)link);
                        break;
                    case BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE:
                        BT_SINK_SRV_HF_QUERY_CALL_LIST(cur_link->handle);
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_ACTIVE);
                        break;

                    default:
                        break;
                }
                break;

            case BT_HFP_CIEV_CALL_HOLD_STATE_SOME:
                switch (cur_link->call_state) {
                    case BT_SINK_SRV_HF_CALL_STATE_ACTIVE:
                    case BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING:
                    case BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING:
                    case BT_SINK_SRV_HF_CALL_STATE_MULTIPARTY:
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE);
                        break;

                    default:
                        break;
                }
                break;

            case BT_HFP_CIEV_CALL_HOLD_STATE_ALL:
                switch (cur_link->call_state) {
                    case BT_SINK_SRV_HF_CALL_STATE_ACTIVE:
                    case BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING:
                    case BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE:
                    case BT_SINK_SRV_HF_CALL_STATE_MULTIPARTY:
                        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING);
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}


void bt_sink_srv_hf_handle_call_info_ind(bt_sink_srv_hf_context_t *link, bt_hfp_call_list_ind_t *clcc)
{
    if (clcc->multiple_party) {
        bt_sink_srv_hf_call_set_state(link, BT_SINK_SRV_HF_CALL_STATE_MULTIPARTY);
    }
}

bt_sink_srv_hf_context_t *bt_sink_srv_hf_find_device_by_call_state(bt_sink_srv_hf_call_state_t call_state)
{
    uint32_t i;
    bt_sink_srv_hf_context_t *device = NULL, *highlight = NULL;

    highlight = bt_sink_srv_hf_get_highlight_device();

    if (NULL != highlight && highlight->link.call_state & call_state) {
        device = highlight;
    } else {
        for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
            if (highlight != &g_sink_srv_hf_context[i] &&
                    call_state & g_sink_srv_hf_context[i].link.call_state) {
                device = &g_sink_srv_hf_context[i];
                break;
            }
        }
    }
    return device;
}

void bt_sink_srv_hf_answer_call(bool accept)
{
    bt_sink_srv_hf_context_t *device;

    if (NULL != (device = bt_sink_srv_hf_find_device_by_call_state(BT_SINK_SRV_HF_CALL_STATE_INCOMING))) {
        if (device != bt_sink_srv_hf_get_highlight_device()) {
            bt_sink_srv_hf_mp_answer(device, accept);
        } else {
            if (accept) {
                BT_SINK_SRV_HF_ANSWER_CALL(device->link.handle);
            } else {
                device->link.flag |= BT_SINK_SRV_HF_FLAG_USER_REJECT;
                BT_SINK_SRV_HF_REJECT_CALL(device->link.handle);
            }
        }
    }
}

void bt_sink_srv_hf_terminate_call(void)
{
    bt_sink_srv_hf_context_t *device;

    if (NULL != (device = bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                          (BT_SINK_SRV_HF_CALL_STATE_OUTGOING | BT_SINK_SRV_HF_CALL_STATE_ACTIVE)))) {
        BT_SINK_SRV_HF_TERMINATE_CALL(device->link.handle);
    } else if (NULL != (device = bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                                 (BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING | BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING |
                                  BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE | BT_SINK_SRV_HF_CALL_STATE_MULTIPARTY)))) {
        BT_SINK_SRV_HF_HOLD_CALL(device->link.handle, BT_SINK_SRV_HF_CHLD_RELEASE_ACTIVE_ACCEPT_OTHER);
    } else if (NULL != (device = bt_sink_srv_hf_find_device_by_call_state(BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING))) {
        BT_SINK_SRV_HF_HOLD_CALL(device->link.handle, BT_SINK_SRV_HF_CHLD_RELEASE_HELD_REJECT_WAITING);
    }
}

void bt_sink_srv_hf_release_all_held_call(void)
{
    bt_sink_srv_hf_context_t *device;

    if (NULL != (device = bt_sink_srv_hf_find_device_by_call_state(BT_SINK_SRV_HF_CALL_STATE_INCOMING))) {
        bt_sink_srv_hf_answer_call(false);
    } else if (NULL != (device = bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                                 (BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING | BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE |
                                  BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING)))) {
        if (bt_sink_srv_hf_is_feature_support(BT_SINK_SRV_ACTION_3WAY_RELEASE_ALL_HELD, device)) {
            BT_SINK_SRV_HF_HOLD_CALL(device->link.handle, BT_SINK_SRV_HF_CHLD_RELEASE_HELD_REJECT_WAITING);
        }
    }
}

void bt_sink_srv_hf_release_all_active_accept_others(void)
{
    bt_sink_srv_hf_context_t *device;

    device = bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
             (BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING | BT_SINK_SRV_HF_CALL_STATE_ACTIVE |
              BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE | BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING));

    if (NULL != device && bt_sink_srv_hf_is_feature_support(BT_SINK_SRV_ACTION_3WAY_RELEASE_ACTIVE_ACCEPT_OTHER, device)) {
        BT_SINK_SRV_HF_HOLD_CALL(device->link.handle, BT_SINK_SRV_HF_CHLD_RELEASE_ACTIVE_ACCEPT_OTHER);
    }
}

void bt_sink_srv_hf_hold_all_active_accept_others(void)
{
    bt_sink_srv_hf_context_t *device;
    bt_sink_srv_hf_context_t *held;

    if (NULL != (device = bt_sink_srv_hf_find_device_by_call_state(BT_SINK_SRV_HF_CALL_STATE_INCOMING))) {
        bt_sink_srv_hf_answer_call(true);
    } else if ((bt_sink_srv_hf_find_device_by_call_state(BT_SINK_SRV_HF_CALL_STATE_ALL) ==
                (device = bt_sink_srv_hf_find_device_by_call_state(BT_SINK_SRV_HF_CALL_STATE_ACTIVE))) &&
               (NULL != (held = bt_sink_srv_hf_find_device_by_call_state(BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING)))) {
        // Swap on different smart phone, high light is active only, the held in the another
        if (device) {
            bt_sink_srv_hf_mp_swap(device, held);
        }
    } else if (NULL != (device = bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
                                 (BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING | BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE |
                                  BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING | BT_SINK_SRV_HF_CALL_STATE_ACTIVE)))) {
        if (bt_sink_srv_hf_is_feature_support(BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER, device)) {
            BT_SINK_SRV_HF_HOLD_CALL(device->link.handle, BT_SINK_SRV_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER);
        }
    }
}


void bt_sink_srv_hf_release_special(uint8_t index)
{
    bt_sink_srv_hf_context_t *device;

    device = bt_sink_srv_hf_find_device_by_call_state(BT_SINK_SRV_HF_CALL_STATE_ACTIVE);

    if (NULL != device && bt_sink_srv_hf_is_feature_support(BT_SINK_SRV_ACTION_3WAY_RELEASE_SPECIAL, device)) {
        char buffer[BT_SINK_SRV_HF_CMD_LENGTH];

        snprintf((char *)buffer,
                 BT_SINK_SRV_HF_CMD_LENGTH,
                 "AT+CHLD=1%d",
                 index);

        BT_SINK_SRV_HF_HOLD_CALL(device->link.handle, buffer);
    }
}

void bt_sink_srv_hf_hold_special(uint8_t index)
{
    bt_sink_srv_hf_context_t *device;

    device = bt_sink_srv_hf_find_device_by_call_state(BT_SINK_SRV_HF_CALL_STATE_ACTIVE);

    if (NULL != device && bt_sink_srv_hf_is_feature_support(BT_SINK_SRV_ACTION_3WAY_HOLD_SPECIAL, device)) {
        char buffer[BT_SINK_SRV_HF_CMD_LENGTH];

        snprintf((char *)buffer,
                 BT_SINK_SRV_HF_CMD_LENGTH,
                 "AT+CHLD=2%d",
                 index);

        BT_SINK_SRV_HF_HOLD_CALL(device->link.handle, buffer);
    }
}

void bt_sink_srv_hf_add_held_to_conversation(void)
{
    bt_sink_srv_hf_context_t *device;

    device = bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
             (BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE |
              BT_SINK_SRV_HF_CALL_STATE_MULTIPARTY));

    if (NULL != device && bt_sink_srv_hf_is_feature_support(BT_SINK_SRV_ACTION_3WAY_ADD_HELD_CALL_TO_CONVERSATION, device)) {
        BT_SINK_SRV_HF_HOLD_CALL(device->link.handle, BT_SINK_SRV_HF_CHLD_ADD_HELD_TO_CONVERSATION);
    }
}

void bt_sink_srv_hf_explicit_call_transfer(void)
{
    bt_sink_srv_hf_context_t *device;

    device = bt_sink_srv_hf_find_device_by_call_state((bt_sink_srv_hf_call_state_t)
             (BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE |
              BT_SINK_SRV_HF_CALL_STATE_MULTIPARTY));

    if (NULL != device && bt_sink_srv_hf_is_feature_support(BT_SINK_SRV_ACTION_3WAY_EXPLICIT_CALL_TRANSFER, device)) {
        BT_SINK_SRV_HF_HOLD_CALL(device->link.handle, BT_SINK_SRV_HF_CHLD_EXPLICIT_CALL_TRANSFER);
    }
}

void bt_sink_srv_hf_voice_recognition_activate(bool active)
{
    bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];

    if (bt_sink_srv_cm_get_connected_device(BT_SINK_SRV_PROFILE_HFP, addr_list) > 0) {
       bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = (bt_sink_srv_hf_context_t *)bt_sink_srv_cm_get_profile_info(&addr_list[0], BT_SINK_SRV_PROFILE_HFP);

        if (NULL != bt_sink_srv_hf_context_p) {
            if (NULL == bt_sink_srv_hf_find_device_by_call_state(BT_SINK_SRV_HF_CALL_STATE_ALL) &&
                bt_sink_srv_hf_is_feature_support(BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE, bt_sink_srv_hf_context_p)) {
                char buffer[BT_SINK_SRV_HF_CMD_LENGTH];

                snprintf((char *)buffer,
                         BT_SINK_SRV_HF_CMD_LENGTH,
                         "AT+BVRA=%d",
                         active);
                BT_SINK_SRV_HF_VOICE_RECOGNITION(bt_sink_srv_hf_context_p->link.handle, buffer);
            }
        }
    }

}

void bt_sink_srv_hf_dial_last(void)
{
    bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];

    if (bt_sink_srv_cm_get_connected_device(BT_SINK_SRV_PROFILE_HFP, addr_list) > 0) {
        bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = (bt_sink_srv_hf_context_t *)bt_sink_srv_cm_get_profile_info(&addr_list[0], BT_SINK_SRV_PROFILE_HFP);

        if (NULL != bt_sink_srv_hf_context_p) {
            BT_SINK_SRV_HF_DIAL_LAST_NUMBER(bt_sink_srv_hf_context_p->link.handle);
        }
    }
}

void bt_sink_srv_hf_dial_number(char *number)
{
    #ifdef MTK_BT_COMMAND_ENABLE
    bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];

    if (bt_sink_srv_cm_get_connected_device(BT_SINK_SRV_PROFILE_HFP, addr_list) > 0) {
        bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = (bt_sink_srv_hf_context_t *)bt_sink_srv_cm_get_profile_info(&addr_list[0], BT_SINK_SRV_PROFILE_HFP);

        char buffer[BT_SINK_SRV_HF_DIAL_CMD_LENGTH];
        snprintf((char *)buffer,
                 BT_SINK_SRV_HF_DIAL_CMD_LENGTH,
                 "ATD%s;",
                 number);

        if (NULL != bt_sink_srv_hf_context_p) {
            BT_SINK_SRV_HF_DIAL_NUMBER(bt_sink_srv_hf_context_p->link.handle, buffer);
        }
    }
    #endif /*MTK_BT_COMMAND_ENABLE*/
}

void bt_sink_srv_hf_dial_missed(void)
{
    if (NULL != g_sink_srv_hf_missed_call_device_p
            && g_sink_srv_hf_missed_call_device_p->link.handle != 0
            && bt_sink_srv_strlen((char *)g_sink_srv_hf_missed_call_device_p->link.caller.number) > 0) {
        char buffer[BT_SINK_SRV_HF_DIAL_CMD_LENGTH];

        snprintf((char *)buffer,
                 BT_SINK_SRV_HF_DIAL_CMD_LENGTH,
                 "ATD%s;",
                 g_sink_srv_hf_missed_call_device_p->link.caller.number);

        BT_SINK_SRV_HF_DIAL_NUMBER(g_sink_srv_hf_missed_call_device_p->link.handle, buffer);
    }
}

void bt_sink_srv_hf_switch_audio_path(void)
{
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_highlight_device();
    bt_sink_srv_report("[Sink][HF] bt_sink_srv_hf_context_p:0x%x", bt_sink_srv_hf_context_p);
    if (NULL != bt_sink_srv_hf_context_p && BT_SINK_SRV_HF_CALL_STATE_IDLE != bt_sink_srv_hf_context_p->link.call_state) {
        bt_sink_srv_report("[Sink][HF] call_state:0x%x, 0x%x", bt_sink_srv_hf_context_p->link.call_state, bt_sink_srv_hf_context_p->link.audio_type);
        if (bt_sink_srv_hf_context_p->link.audio_type == BT_SINK_SRV_HF_AUDIO_SCO) {
            bt_hfp_audio_transfer(bt_sink_srv_hf_context_p->link.handle, BT_HFP_AUDIO_TO_AG);
        } else {
            bt_hfp_audio_transfer(bt_sink_srv_hf_context_p->link.handle, BT_HFP_AUDIO_TO_HF);
        }
    }
}

void bt_sink_srv_hf_switch_audio_device(void)
{
    bt_sink_srv_hf_mp_switch_audio();
}

void bt_sink_srv_hf_query_call_list(bt_bd_addr_t *address)
{
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = NULL;
    bt_bd_addr_t *address_p = address;

    if (NULL == address_p) {
        address_p = bt_sink_srv_cm_last_connected_device();
    }

    bt_sink_srv_hf_context_p = bt_sink_srv_cm_get_profile_info(address, BT_SINK_SRV_PROFILE_HFP);

    if (NULL != bt_sink_srv_hf_context_p) {
        if (!(bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_QUERY_LIST)) {
            bt_sink_srv_hf_context_p->link.flag |= BT_SINK_SRV_HF_FLAG_QUERY_LIST;
            BT_SINK_SRV_HF_QUERY_CALL_LIST(bt_sink_srv_hf_context_p->link.handle);
        }
    }
}

void bt_sink_srv_hf_send_dtmf(bt_sink_srv_send_dtmf_t *request)
{
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = NULL;

    if (NULL != request) {
        bt_sink_srv_hf_context_p = bt_sink_srv_cm_get_profile_info(&request->address, BT_SINK_SRV_PROFILE_HFP);

        if ((request->code == '*' || request->code == '#' || (request->code <= '9' && request->code >= '0' ))) {

            if (NULL != bt_sink_srv_hf_context_p) {
                char buffer[BT_SINK_SRV_HF_CMD_LENGTH];

                snprintf((char *)buffer,
                         BT_SINK_SRV_HF_CMD_LENGTH,
                         "AT+VTS=%c",
                         request->code);
                BT_SINK_SRV_HF_SEND_DTMF(bt_sink_srv_hf_context_p->link.handle, buffer);
            }
        }
    }
}

void bt_sink_srv_hf_enable_apl_custom_commands(uint32_t handle, bt_sink_srv_ios_hf_features_t features)
{
    char buffer[BT_SINK_SRV_HF_LONG_CMD_LENGTH];

    /*  Enable custom AT commands */
    /* Format: AT+XAPL=vendorID-productID-version,features */
    snprintf((char *)buffer,
             BT_SINK_SRV_HF_LONG_CMD_LENGTH,
             "AT+XAPL=MTK-HB-0400,%d",
             features);

    BT_SINK_SRV_HF_ENABLE_CUSTOM_CMDS(handle, buffer);
}

bt_sink_srv_status_t bt_sink_srv_hf_apl_report_battery(uint8_t battery_level)
{
    const bt_sink_srv_features_config_t *config_p = bt_sink_srv_get_features_config();
    if (BT_SINK_SRV_FEATURE_IOS_HFP_CUSTOM_AT_COMMAND & config_p->features) {
        char buffer[BT_SINK_SRV_HF_LONG_CMD_LENGTH];
        bt_sink_srv_report("[Sink][HF] battery: %d", battery_level);

        /* Check connected hfp device or battery level 0~9*/
        if (battery_level > 9 ) {
            return BT_SINK_SRV_STATUS_INVALID_PARAM;
        }
            /* Send custom AT commands */
            /* Format: AT+IPHONEACCEV = Number of key/value pairs, key1,val1, key2, val2...*/
        snprintf((char *)buffer,
             BT_SINK_SRV_HF_LONG_CMD_LENGTH,
             "AT+IPHONEACCEV=1,1,%d",
             battery_level);
  
        for (uint32_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
            if (g_sink_srv_hf_context[i].link.handle) {
                bt_sink_srv_report("[Sink][HF] hf handle: 0x%x", g_sink_srv_hf_context[i].link.handle);
                BT_SINK_SRV_HF_ENABLE_CUSTOM_CMDS(g_sink_srv_hf_context[i].link.handle, buffer);
            }
        }
        return BT_SINK_SRV_STATUS_SUCCESS;
    }
    return BT_SINK_SRV_STATUS_INVALID_STATUS;
}

