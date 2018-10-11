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


#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_music.h"
#include "audio_src_srv.h"
#include "bt_sink_srv_aws_mp3.h"
#include "bt_sink_srv_mp3_state_machine.h"
#include "bt_sink_srv_state_notify.h"
#include "nvdm.h"

/* Function declare */

static void bt_sink_srv_music_transfer_state(audio_src_srv_state_t state);

//static void bt_sink_srv_music_update_dev_state(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_state_t state);

static void bt_sink_srv_music_set_dev_transient_state(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_transient_state_t substate);

static void bt_sink_srv_music_mp3_state_none_handle(bt_sink_srv_music_state_t state);

static void bt_sink_srv_music_mp3_state_ready_handle(bt_sink_srv_music_state_t state);

static void bt_sink_srv_music_mp3_state_playing_handle(bt_sink_srv_music_state_t state);




static void bt_sink_srv_music_transfer_state(bt_sink_srv_music_state_t state)
{
    bt_sink_srv_music_device_t *dev;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);

    bt_sink_srv_report("[Music]transfer_state--ori: %d, cur: %d", dev->state, state);
    dev->state = state;
}


#if 0
static void bt_sink_srv_music_update_dev_state(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_state_t state)
{
    bt_sink_srv_report("[Music]update_dev_state-type: %d, ori: %d, cur: %d", dev->mp3->audio_src->type, dev->state, state);
    dev->state = state;
}
#endif


static void bt_sink_srv_music_set_dev_transient_state(bt_sink_srv_music_device_t *dev, bt_sink_srv_music_transient_state_t substate)
{
    bt_sink_srv_report("[Music]set_dev_transient_state--type: %d, ori: %d, cur: %d", dev->mp3->audio_src->type, dev->substate, substate);
    dev->substate = substate;
}

/* 00 means master, 01 means slave */
static uint8_t g_mp3_local_role;
void bt_sink_srv_mp3_get_local_device_role()
{

    uint8_t local_role = 0;
    uint32_t size = 1;
    nvdm_status_t nvdm_status = nvdm_read_data_item("BT_SINK", "role", (uint8_t *)&local_role, &size);
    bt_sink_srv_report("read local role result:%d, role %d", nvdm_status, local_role);
    g_mp3_local_role = local_role;
}

static void bt_sink_srv_music_state_ready_none(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_report("[mp3][state]%s flag %x", __func__, dev->flag);
#ifdef __BT_AWS_SUPPORT__
    // aws cases
    if (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) {
        // aws sink receive start ind
        if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START) {
            BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
            bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC);
            if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT) {
                BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT);
                dev->mp3->audio_src->play(dev->mp3->audio_src);
            } else {
                audio_src_srv_update_state(dev->mp3->audio_src, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
            }
        // not able to be aws src
        } else if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_SOURCE_AVAILABLE)
        /* local role is get from application */
          || g_mp3_local_role == 0) {
            BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
            bt_sink_srv_aws_mp3_notify_start(dev->aws_hd);
        // deside to send set role or start
        } else {
            if (dev->aws_role != BT_AWS_ROLE_SOURCE) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_ROLE_SET);
                // want to set local as source, so send to remote as sink
                dev->aws_role = BT_AWS_ROLE_SOURCE;
                bt_sink_srv_aws_mp3_set_role_request(dev->aws_hd, BT_AWS_ROLE_SINK);
            } else {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_ROLE_SET);
                bt_sink_srv_music_mp3_state_process();
            }
        }
    // local
    } else
#endif
    {
#ifdef __BT_AWS_SUPPORT__
        // send an un-specific command to wake up N9 bt clock
        bt_gap_read_inquiry_response_tx();
#endif

        bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC);
        if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT) {
            BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT);
            dev->mp3->audio_src->play(dev->mp3->audio_src);
        } else {
            audio_src_srv_update_state(dev->mp3->audio_src, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
        }
    }
}
#ifdef __BT_AWS_SUPPORT__
static void bt_sink_srv_music_state_ready_wait_aws_role_set(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_report("[mp3][state]%s", __func__);
    bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_START);
    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK) {
        bt_sink_srv_aws_mp3_change_track_request(dev->aws_hd);
    } else {
        bt_sink_srv_aws_mp3_start_request(dev->aws_hd);
    }
}

static void bt_sink_srv_music_state_ready_wait_aws_start(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_report("[mp3][state]%s", __func__);
    bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC);
    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT) {
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT);
        dev->mp3->audio_src->play(dev->mp3->audio_src);
    } else {
        audio_src_srv_update_state(dev->mp3->audio_src, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);
    }
}

static void bt_sink_srv_music_aws_mp3_state_sync(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_report("[mp3][state]%s substate %d", __func__, dev->substate);

    switch (dev->substate) {
        case BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE:
            bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_ROLE_SET);
            dev->aws_role = BT_AWS_ROLE_SOURCE;
            bt_sink_srv_aws_mp3_set_role_request(dev->aws_hd, BT_AWS_ROLE_SINK);
            break;
        case BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_ROLE_SET:
            bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_START);
            BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET);
            bt_aws_fetch_bt_time_offset(dev->aws_hd);
            bt_sink_srv_aws_mp3_start_request(dev->aws_hd);
            break;
        case BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_START:
            bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
            BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_AWS_STATE_SYNC);
            bt_sink_srv_aws_mp3_send_sync_time();
            break;
        default:
            break;
    }
}
#endif
static void bt_sink_srv_music_state_ready_prepare_codec(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_report("[mp3][state]%s", __func__);
    if (!(dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN)) {
        bt_sink_srv_report("[mp3][state] codec not opened yet");
        return;
    }
    BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET);
#ifdef __BT_AWS_SUPPORT__
    int32_t ret = 0;
    if ( dev->mp3->supportAws ) {
        if (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) {
            ret = bt_aws_fetch_bt_time_offset(dev->aws_hd);
            bt_sink_srv_report("[mp3][state]try to get offset-ret: %d", ret);
        }
    }
#endif
    bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER);
    bt_sink_srv_mp3_local_give_semaphore();
}

static void bt_sink_srv_music_state_ready_prepare_buffer(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_report("[mp3][state]%s", __func__);
    if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_PLAY)) {
        bt_sink_srv_report("[mp3][state] buffer not ready yet");
        return;
    }
#ifdef __BT_AWS_SUPPORT__
    if ( dev->mp3->supportAws ) {
        if (!(dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) ||
              (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT &&
              (dev->aws_role != BT_AWS_ROLE_SINK))) {
            bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_INIT_SYNC);
            BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
            bt_sink_srv_aws_mp3_send_sync_time();
        // mp3 play must called before init sync.
        // check BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER to make sure mp3 play was called
        } else if ((dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) &&
                    (dev->aws_role == BT_AWS_ROLE_SINK) &&
                    (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER)) {
            if (bt_sink_srv_aws_mp3_wait_sync_time() == 0) {
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_INIT_SYNC);
                BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
            }
        }
    }
    else {
        bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_INIT_SYNC);
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);

        // for low power test
        // if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_LOW_POWER_TEST) {
            bt_sink_srv_music_mp3_state_process();
        // }
    }
#else
    bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_INIT_SYNC);
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
    bt_sink_srv_music_mp3_state_process();
#endif
}

static void bt_sink_srv_music_state_ready_wait_init_sync(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_report("[mp3][state]%s", __func__);
    BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_PLAYING);
    bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_AWS_STATE_SYNC);
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK);
    bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_PLAYING);
    /* Update seudo device state */
    audio_src_srv_update_state(dev->mp3->audio_src, AUDIO_SRC_SRV_EVT_PLAYING);
    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK) {
        /* It is possible change track command coming after buffer prepare successful */
        bt_sink_srv_music_mp3_state_process();
    }
}

static void bt_sink_srv_music_state_ready_clear_codec(bt_sink_srv_music_device_t *dev)
{
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP);
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_AWS_STATE_SYNC);
    audio_src_srv_update_state(dev->mp3->audio_src, AUDIO_SRC_SRV_EVT_READY);
    bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);

    // waiting aws stop, dont process to playing if changing track
    if (dev->substate == BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_STOP) {
        bt_sink_srv_report("[mp3] %s wait aws stop", __func__);
        bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
        return;
    } else {
        bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
    }

    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK) {
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_MP3_PLAY, NULL);
    } else {
        /* If change track process, no need let sink srv know currenlty not playing,
         * but this may influence if sink srv and audio srv intergrate state */
        if (bt_sink_srv_state_query() == BT_SINK_SRV_STATE_LOCAL_PLAYING) {
            bt_sink_srv_state_reset(BT_SINK_SRV_STATE_LOCAL_PLAYING);
        }
#ifdef MTK_INEAR_NOODLES_HEADSET_ENABLE
        PMIC_VR_CONTROL(PMIC_VMC    , 0);
#endif
    }
}

static void bt_sink_srv_music_mp3_state_none_handle(bt_sink_srv_music_state_t state)
{
    bt_sink_srv_music_device_t *dev;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);

    if (state == AUDIO_SRC_SRV_STATE_NONE) {
        return;
    } else if (state == AUDIO_SRC_SRV_STATE_READY) {
        if ((dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_SOURCE_AVAILABLE)
    #ifdef __BT_AWS_SUPPORT__
        || dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT
    #endif
            ) {
            bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
            bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
            /* Update seudo device state */
            audio_src_srv_update_state(dev->mp3->audio_src, AUDIO_SRC_SRV_EVT_READY);
        }
    } else if ((state == AUDIO_SRC_SRV_STATE_PLAYING) && (dev->mp3->file_total > 0)) {
        bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_READY);
        bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
        audio_src_srv_update_state(dev->mp3->audio_src, AUDIO_SRC_SRV_EVT_READY);
        bt_sink_srv_music_mp3_state_process();
    }

}

static void bt_sink_srv_music_mp3_state_ready_handle(bt_sink_srv_music_state_t state)
{
    bt_sink_srv_music_device_t *dev;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    if (state == AUDIO_SRC_SRV_STATE_NONE) {
        bt_sink_srv_music_transfer_state(AUDIO_SRC_SRV_STATE_NONE);
        bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
        /* Update seudo device state */
        audio_src_srv_update_state(dev->mp3->audio_src, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
        return;
    } else if (state == AUDIO_SRC_SRV_STATE_READY) {
        // interrupt suspend comes
        if ((dev->substate >= BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC) &&
         (dev->substate <= BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_INIT_SYNC)) {
            // check this flag when timer timeout
            BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_PLAY);

            BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP);
            bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
#ifdef __BT_AWS_SUPPORT__
            // send stop
            if (dev->aws_role == BT_AWS_ROLE_SOURCE &&
                    dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT &&
                    dev->mp3->supportAws) {
                bt_sink_srv_aws_mp3_stop_request(dev->aws_hd);
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_STOP);
            }
#endif

            // local stop
            if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT) {
                dev->mp3->audio_src->stop(dev->mp3->audio_src);
            } else {
                audio_src_srv_update_state(dev->mp3->audio_src, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
        // common case
        } else {
            bt_sink_srv_music_state_ready_clear_codec(dev);
            return;
        }
    } else {
        switch (dev->substate) {
            case BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE:
                bt_sink_srv_music_state_ready_none(dev);
                break;
            case BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN:
                //bt_sink_srv_music_state_ready_wait_conn(dev);
                break;
        #ifdef __BT_AWS_SUPPORT__
            case BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_ROLE_SET:
                bt_sink_srv_music_state_ready_wait_aws_role_set(dev);
                break;
            case BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_START:
                bt_sink_srv_music_state_ready_wait_aws_start(dev);
                break;
            case BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_STOP:
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                bt_sink_srv_music_state_ready_clear_codec(dev);
                break;
        #endif
            case BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC:
                bt_sink_srv_music_state_ready_prepare_codec(dev);
                break;
            case BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER:
                bt_sink_srv_music_state_ready_prepare_buffer(dev);
                break;
            case BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_INIT_SYNC:
                bt_sink_srv_music_state_ready_wait_init_sync(dev);
                break;
            case BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC:
                bt_sink_srv_music_state_ready_clear_codec(dev);
                break;

            default:
                bt_sink_srv_report("[Music]sub state %d", dev->substate);
                break;
        }
    }
}


static void bt_sink_srv_music_mp3_state_playing_handle(bt_sink_srv_music_state_t state)
{
    bt_sink_srv_music_device_t *dev;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    bt_sink_srv_report("%s state %x flag %x %d", __func__, state, dev->flag,
            (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_STOPPING)));

    // changing track
    if (state == AUDIO_SRC_SRV_STATE_PLAYING &&
            dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK &&
            !(dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_STOPPING)) {
        // before stop
        if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP)) {
            // check this flag when timer timeout
            BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_PLAY);

            BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP);
            bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
            audio_src_srv_update_state(dev->mp3->audio_src, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
        // after stop
        } else {
            bt_sink_srv_music_state_ready_clear_codec(dev);
        }
#ifdef __BT_AWS_SUPPORT__
    // remote state is not playing, need to negotiate
    } else if (state == AUDIO_SRC_SRV_STATE_PLAYING &&
            dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_AWS_STATE_SYNC
            && dev->mp3->supportAws) {
        bt_sink_srv_music_aws_mp3_state_sync(dev);
#endif
    // stop
    } else if (state != AUDIO_SRC_SRV_STATE_PLAYING ||
            ((state == AUDIO_SRC_SRV_STATE_PLAYING) &&
             (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK))) {
        // before ami stop
        if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP)){
            // check this flag when timer timeout
            BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_PLAY);

            BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP);
            bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC);
            if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT) {
                dev->mp3->audio_src->stop(dev->mp3->audio_src);
            } else {
                audio_src_srv_update_state(dev->mp3->audio_src, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            }
        // after ami stop
        } else {
            #ifdef __BT_AWS_SUPPORT__
            if (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT &&
                    dev->aws_role != BT_AWS_ROLE_SINK &&
                    dev->mp3->supportAws) {
                bt_sink_srv_aws_mp3_stop_request(dev->aws_hd);
                bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_STOP);
            }
            #endif
            bt_sink_srv_music_state_ready_clear_codec(dev);
        }
    }
}


void bt_sink_srv_music_mp3_recovery_local_state(bt_sink_srv_mp3_recovery_status_t status)
{
    bt_sink_srv_music_device_t *dev;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    bt_sink_srv_report("[Music]recovery local status %d, state:%d, target_state:%d,substate %d"
               , status,dev->state, dev->target_state, dev->substate);
    if (dev->state != AUDIO_SRC_SRV_STATE_PLAYING) {
        bt_sink_srv_state_reset(BT_SINK_SRV_STATE_LOCAL_PLAYING);
    }

    if (dev->substate  == BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE) {
        /* No change need to do */
        return;
    }

    if (status == BT_SINK_SRV_MP3_RECOVERY_STATUS_DISCONNECT) {
        if (dev->target_state == AUDIO_SRC_SRV_STATE_PLAYING &&
            dev->state == AUDIO_SRC_SRV_STATE_READY) {
            switch(dev->substate) {
                case BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_ROLE_SET: {
                    bt_sink_srv_music_set_dev_transient_state(dev, BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE);
                    bt_sink_srv_music_mp3_state_process();
                    break;
                }
#ifdef __BT_AWS_SUPPORT__
                case BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_START: {
                    bt_sink_srv_music_state_ready_wait_aws_start(dev);
                    break;
                }
#endif
                default:
                    break;
            }
        }
    }

}
// for event triggered state process, check if:
// 1. state is changing, dont run the state machine once more
// 2. stable state, start to run state machine
bool bt_sink_srv_music_mp3_set_target_and_process(bt_sink_srv_music_state_t state)
{
    bt_sink_srv_music_device_t *dev;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);

    bt_sink_srv_music_mp3_set_target_state(state);

    if (dev->substate  == BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE) {
        bt_sink_srv_music_mp3_state_process();
    }
#ifdef __BT_AWS_SUPPORT__
    else if ((dev->substate == BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_INIT_SYNC) ||
            (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER)) {
        // should be stop or change track, all need to stop first
        bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
        bt_sink_srv_music_mp3_state_process();
    } else {
        BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_AWS_STATE_SYNC);
        bt_sink_srv_report("[Music]wait substate %d stable", dev->substate);
        return false;
    }
#else
    else {
        bt_sink_srv_report("[Music]wait substate %d stable", dev->substate);
        return false;
    }
#endif
    return true;
}

void bt_sink_srv_music_mp3_set_target_state(bt_sink_srv_music_state_t state)
{
    bt_sink_srv_music_device_t *dev;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);

    dev->target_state = state;
}

void bt_sink_srv_music_mp3_state_process()
{
    bt_sink_srv_music_device_t *dev;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);

    bt_sink_srv_report("[Music]state_machine_handle--state: %d, target state %d", dev->state, dev->target_state);

    switch (dev->state) {
        case AUDIO_SRC_SRV_STATE_NONE: {
            bt_sink_srv_music_mp3_state_none_handle(dev->target_state);
            break;
        }

        case AUDIO_SRC_SRV_STATE_READY: {
            bt_sink_srv_music_mp3_state_ready_handle(dev->target_state);
            break;
        }

        case AUDIO_SRC_SRV_STATE_PLAYING:
        {
            bt_sink_srv_music_mp3_state_playing_handle(dev->target_state);
            break;
        }

        default:
            break;
    }
}

