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

#include "bt_a2dp.h"
#include "bt_sink_srv_music.h"
#include "audio_src_srv.h"
#include "bt_sink_srv_utils.h"

/* Function declare */

static void bt_sink_srv_music_transfer_state(audio_src_srv_state_t state);

static void bt_sink_srv_music_update_dev_state(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_state_t state);

static void bt_sink_srv_music_set_dev_transient_state(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_transient_state_t substate);

static void bt_sink_srv_music_state_none_handle(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_event_t evt_id, void *param);

static void bt_sink_srv_music_state_ready_handle(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_event_t evt_id, void *param);

static void bt_sink_srv_music_state_playing_handle(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_event_t evt_id, void *param);




static void bt_sink_srv_music_transfer_state(bt_sink_srv_music_state_t state)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    bt_sink_srv_report("[Music]transfer_state--ori: %d, cur: %d", ctx->state, state);
    ctx->state = state;
}


static void bt_sink_srv_music_update_dev_state(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_state_t state)
{
    bt_sink_srv_report("[Music]update_dev_state-type: %d, ori: %d, cur: %d", dev->handle->type, dev->state, state);
    dev->state = state;
}


static void bt_sink_srv_music_set_dev_transient_state(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_transient_state_t substate)
{
    bt_sink_srv_report("[Music]set_dev_transient_state--type: %d, ori: %d, cur: %d", dev->handle->type, dev->substate, substate);
    dev->substate = substate;
}


static void bt_sink_srv_music_state_none_handle(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_event_t evt_id, void *param)
{
    switch (evt_id) {
        /* SDK event */
        case BT_A2DP_CONNECT_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN);
            } else {
                /* Error */
            }
            break;
        }

        case BT_A2DP_CONNECT_CNF: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
            } else {
                /* Exception */
            }
            break;
        }

#ifdef __BT_AWS_SUPPORT__
        case BT_AWS_CONNECT_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN);
            } else {
                /* Error */
            }
            break;
        }

        case BT_AWS_CONNECT_CNF: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
            } else {
                /* Exception */
            }
            break;
        }
#endif
        /* Sink music event */
        case BT_SINK_SRV_MUSIC_EVT_READY: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN == dev->substate) {
                /* Reset transient state */
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                /* Update seudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update music state */
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                /* AWS connection */
                /* Update seudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update music state */
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else {
                /* Exception */
            }
            break;
        }

        default:
            break;
    }
}


static void bt_sink_srv_music_state_ready_handle(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_event_t evt_id, void *param)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    switch (evt_id) {
        /* SDK event */
        case BT_A2DP_START_STREAMING_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC);
                /* Query audio source play */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
            }
            break;
        }

        case BT_A2DP_DISCONNECT_IND : {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            } else if ((BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->substate) &&
                       (dev == ctx->run_dev)) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            } else if ((BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC == dev->substate) &&
                       (dev == ctx->run_dev)) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_A2DP_DISCONNECT_CNF: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            } else if ((BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->substate) &&
                       (dev == ctx->run_dev)) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            } else if ((BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC == dev->substate) &&
                       (dev == ctx->run_dev)) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_A2DP_SUSPEND_STREAMING_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

    #ifdef __BT_AWS_SUPPORT__
        case BT_AWS_DISCONNECT_IND : {
            audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            break;
        }
    #endif

        /* Sink music event */
        case BT_SINK_SRV_MUSIC_EVT_REJECT: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_CODEC_OPEN: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_RESUME: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_START_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC);
                /* Query audio source play */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_PLAYING: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);                
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_PLAYING);
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_PLAYING);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PLAYING);
            }

            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_READY: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN == dev->substate ) {
                /* Reset transient state */
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                /* Update seudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                /* AWS connection */
                /* Update seudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else {
                /* Exception */
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_UNAVAILABLE: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate ) {
                /* Update pseudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_NONE);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            } else {
                /* Exception */
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_SUSPEND_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_CODEC_CLEAR: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC == dev->substate ||
                       BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_AVRCP_PLAYING: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC);
                /* Query audio source play */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_AVRCP_PAUSED: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        default:
            break;
    }
}


static void bt_sink_srv_music_state_playing_handle(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_event_t evt_id, void *param)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    switch (evt_id) {
        /* SDK event */
        case BT_A2DP_SUSPEND_STREAMING_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_A2DP_DISCONNECT_IND : {
            if ((BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) &&
                (dev == ctx->run_dev)) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            } else {
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            }
            break;
        }

        case BT_A2DP_DISCONNECT_CNF : {
            if ((BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) &&
                (dev == ctx->run_dev)) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            } else {
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            }
            break;
        }

        case BT_A2DP_START_STREAMING_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC);
                /* Query audio source play */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
            }
            break;
        }
#ifdef __BT_AWS_SUPPORT__
        case BT_AWS_DISCONNECT_IND : {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_AWS_DISCONNECT_CNF: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }
#endif
        /* Sink music event */
        case BT_SINK_SRV_MUSIC_EVT_CODEC_CLEAR: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                /* Interrupt case */
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);                
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Nature */
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC == dev->substate ||
                       BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);                
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Nature */
                bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_RECOVER: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                /* Recover flow equal waiting list + stop */
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                /* Priority +1 */
                ++dev->handle->priority;
                /* Push waiting list */
                audio_src_srv_add_waiting_list(dev->handle);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_SUSPEND_IND: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_READY: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN == dev->substate ) {
                /* Reset transient state */
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                /* Update seudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate){
                /* AWS connection */
                /* Update seudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_READY);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_READY);
            } else {
                /* Exception */
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_UNAVAILABLE: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate ) {
                /* Update pseudo device state */
                bt_sink_srv_music_update_dev_state(dev, AUDIO_SRC_SRV_STATE_NONE);
                /* Update audio source state */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            } else {
                /* Exception */
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_AVRCP_PLAYING: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC);
                /* Query audio source play */
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
            }
            break;
        }

        case BT_SINK_SRV_MUSIC_EVT_AVRCP_PAUSED: {
            if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE == dev->substate) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
                audio_src_srv_update_state(dev->handle, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
            break;
        }

        default:
            break;
    }
}


void bt_sink_srv_music_state_machine_handle(bt_sink_srv_music_device_t *dev, uint32_t evt_id, void *param)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    bt_sink_srv_report("[Music]state_machine_handle--state: %d, type: %d, evt: 0x%x, dev--state: %d, substate: %d",
        ctx->state, dev->handle->type, evt_id, dev->state, dev->substate);

    switch (ctx->state) {
        case AUDIO_SRC_SRV_STATE_NONE: {
            bt_sink_srv_music_state_none_handle(dev, evt_id, param);
            break;
        }

        case AUDIO_SRC_SRV_STATE_READY: {
            bt_sink_srv_music_state_ready_handle(dev, evt_id, param);
            break;
        }

        case AUDIO_SRC_SRV_STATE_PLAYING:
        {
            bt_sink_srv_music_state_playing_handle(dev, evt_id, param);
            break;
        }
    }
}

