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
#include "bt_sink_srv.h"
#include "bt_sink_srv_ami.h"

#ifdef __BT_AWS_SUPPORT__
#include "bt_sink_srv_aws.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_action.h"
#include "bt_gap.h"
#include "bt_sink_srv_utils.h"
#include "hal_audio.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_codec.h"
#include "bt_sink_srv_media_mgr.h"
#include "bt_aws.h"
#include "bt_sink_srv_avrcp.h"
#include "hal_audio.h"
#include "bt_sink_srv_audio_sync.h"
#include "bt_sink_srv_mp3.h"
#include "bt_sink_srv_aws_mp3.h"
#include "bt_sink_srv_am_task.h"
#include "bt_sink_srv_mp3_state_machine.h"


/* static function declare */
static bt_status_t bt_sink_srv_aws_mp3_callback(bt_aws_command_result_t *cmd_result);
static bt_status_t bt_sink_srv_aws_mp3_req_callback(bt_sink_srv_aws_cmd_ind_t *cmd_ind);
static bt_status_t bt_sink_srv_aws_mp3_rsp_callback(bt_sink_srv_aws_cmd_rsp_t *cmd_rsp);

static int32_t bt_sink_srv_aws_mp3_handle_connect_cnf(bt_aws_connect_cnf_t *conn_cnf);
static int32_t bt_sink_srv_aws_mp3_handle_disconnect_cnf(bt_aws_disconnect_cnf_t *disconn_cnf);
static int32_t bt_sink_srv_aws_mp3_handle_disconnect_ind(bt_aws_disconnect_ind_t *disconn_ind);
static int32_t bt_sink_srv_aws_mp3_handle_play_ind(bt_aws_notify_play_ind_t *play_ind);
static int32_t bt_sink_srv_aws_mp3_handle_streaming_received_ind(bt_aws_streaming_received_ind_t *str_ind);
static int32_t bt_sink_srv_aws_mp3_handle_offset_update_ind(bt_aws_offset_update_ind_t *offset_ind);
static int32_t bt_sink_srv_aws_mp3_handle_start_cnf(bt_sink_srv_aws_mp3_packet *start_cnf);
static int32_t bt_sink_srv_aws_mp3_handle_start_ind(bt_sink_srv_aws_mp3_packet *start_ind);
static int32_t bt_sink_srv_aws_mp3_handle_stop_cnf(bt_sink_srv_aws_mp3_packet *suspend_cnf);
static int32_t bt_sink_srv_aws_mp3_handle_stop_ind(bt_sink_srv_aws_mp3_packet *suspend_ind);
static int32_t bt_sink_srv_aws_mp3_handle_change_track_ind(bt_sink_srv_aws_mp3_packet *ind);
static int32_t bt_sink_srv_aws_mp3_handle_change_track_cnf(bt_sink_srv_aws_mp3_packet *cnf);
static int32_t bt_sink_srv_aws_mp3_handle_set_role_cnf(bt_sink_srv_aws_mp3_packet *aws_role_set_ind);
static int32_t bt_sink_srv_aws_mp3_handle_set_role_ind(bt_sink_srv_aws_mp3_packet *aws_role_set_ind);

static inline bt_status_t bt_sink_srv_aws_mp3_send_request(uint32_t handle,
        bt_sink_srv_aws_cmd_t *req);
static inline bt_status_t bt_sink_srv_aws_mp3_send_response(uint32_t handle,
        bt_sink_srv_aws_cmd_t *resp);


bt_status_t bt_sink_srv_aws_mp3_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;

    if (status != BT_STATUS_SUCCESS) {
        bt_sink_srv_report("aws packet %d received failed", msg);
        return BT_STATUS_FAIL;
    }
    switch (msg) {
        case BT_AWS_CONNECT_CNF:
            bt_sink_srv_aws_mp3_handle_connect_cnf((bt_aws_connect_cnf_t *)buffer);
            break;

        case BT_AWS_DISCONNECT_CNF:
            bt_sink_srv_aws_mp3_handle_disconnect_cnf((bt_aws_disconnect_cnf_t *)buffer);
            break;

        case BT_AWS_DISCONNECT_IND:
            bt_sink_srv_aws_mp3_handle_disconnect_ind((bt_aws_disconnect_ind_t *)buffer);
            break;

        case BT_AWS_COMMAND_IND:
            bt_sink_srv_aws_mp3_callback((bt_aws_command_result_t *)buffer);
            break;

        case BT_AWS_NOTIFY_PLAY_IND:
            bt_sink_srv_aws_mp3_handle_play_ind((bt_aws_notify_play_ind_t *)buffer);
            break;

        case BT_AWS_STREAMING_PACKET_RECEIVED_IND:
            bt_sink_srv_aws_mp3_handle_streaming_received_ind((bt_aws_streaming_received_ind_t *)buffer);
            break;

        case BT_AWS_OFFSET_UPDATE_IND:
            bt_sink_srv_aws_mp3_handle_offset_update_ind((bt_aws_offset_update_ind_t *)buffer);
            break;

        default:
            break;
    }

    return ret;
}

static bt_status_t bt_sink_srv_aws_mp3_callback(bt_aws_command_result_t *cmd_result)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;

    switch (cmd_result->type) {
        case BT_SINK_SRV_AWS_COMMAND_TYPE_REQ:
            bt_sink_srv_aws_mp3_req_callback(&cmd_result->cmd_ind);
            break;
        case BT_SINK_SRV_AWS_COMMAND_TYPE_RSP:
            bt_sink_srv_aws_mp3_rsp_callback(&cmd_result->cmd_rsp);
            break;
        default:
            break;
    }

    return ret;
}

/* AWS command + notification */
static bt_status_t bt_sink_srv_aws_mp3_req_callback(bt_sink_srv_aws_cmd_ind_t *cmd_ind)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_cmd_id_t id = cmd_ind->cmd_id;
    bt_sink_srv_aws_mp3_packet *cmd = (bt_sink_srv_aws_mp3_packet *)cmd_ind->data;
    bt_sink_srv_report("[sink][aws][mp3] handle req: %x", id);

    switch (id) {
        /* AWS command */
        case BT_SINK_SRV_AWS_COMMAND_ID_START:
            bt_sink_srv_aws_mp3_handle_start_ind(cmd);
            break;
        case BT_SINK_SRV_AWS_COMMAND_ID_STOP:
            bt_sink_srv_aws_mp3_handle_stop_ind(cmd);
            break;
        case BT_SINK_SRV_AWS_COMMAND_ID_NEXT:
            bt_sink_srv_aws_mp3_handle_change_track_ind(cmd);
            break;
        case BT_SINK_SRV_AWS_COMMAND_ID_PREV:
            bt_sink_srv_aws_mp3_handle_change_track_ind(cmd);
            break;
        case BT_SINK_SRV_AWS_COMMAND_ID_SET_VOLUME:
            /* nfa */
            break;
        case BT_SINK_SRV_AWS_COMMAND_ID_SET_ROLE:
            bt_sink_srv_aws_mp3_handle_set_role_ind(cmd);
            break;
        /* AWS notification */
        case BT_SINK_SRV_AWS_NOTIFY_START:
            bt_sink_srv_mp3_play();
            break;
        case BT_SINK_SRV_AWS_NOTIFY_STOP:
            bt_sink_srv_mp3_pause();
            break;
        case BT_SINK_SRV_AWS_NOTIFY_NEXT:
            bt_sink_srv_mp3_next_track();
            break;
        case BT_SINK_SRV_AWS_NOTIFY_PREV:
            bt_sink_srv_mp3_prev_track();
            break;
        case BT_SINK_SRV_AWS_NOTIFY_SET_VOLUME:
            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD,
                    (void *)(&(cmd->handle)));
            bt_sink_srv_report("[sink][aws][mp3] vol lev: %d", cmd->vol_lev);
            dev->mp3->vol_lev = cmd->vol_lev;
            bt_sink_srv_ami_audio_set_volume(dev->mp3->aud_id, cmd->vol_lev, STREAM_OUT);
            break;
        case BT_SINK_SRV_AWS_NOTIFY_SET_ROLE:
            /* nfa */
            break;

        default:
            break;
    }
    return ret;
}

/* AWS response */
static bt_status_t bt_sink_srv_aws_mp3_rsp_callback(bt_sink_srv_aws_cmd_rsp_t *cmd_rsp)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    bt_sink_srv_cmd_id_t id = cmd_rsp->cmd_id;
    bt_sink_srv_aws_mp3_packet *rsp = (bt_sink_srv_aws_mp3_packet *)cmd_rsp->data;
    bt_sink_srv_report("[sink][aws][mp3] handle rsp: %x", id);

    switch (id) {
        case BT_SINK_SRV_AWS_COMMAND_ID_START:
            bt_sink_srv_aws_mp3_handle_start_cnf(rsp);
            break;
        case BT_SINK_SRV_AWS_COMMAND_ID_STOP:
            bt_sink_srv_aws_mp3_handle_stop_cnf(rsp);
            break;
        case BT_SINK_SRV_AWS_COMMAND_ID_NEXT:
            bt_sink_srv_aws_mp3_handle_change_track_cnf(rsp);
            break;
        case BT_SINK_SRV_AWS_COMMAND_ID_PREV:
            bt_sink_srv_aws_mp3_handle_change_track_cnf(rsp);
            break;
        case BT_SINK_SRV_AWS_COMMAND_ID_SET_VOLUME:
            /* nfa */
            break;
        case BT_SINK_SRV_AWS_COMMAND_ID_SET_ROLE:
            bt_sink_srv_aws_mp3_handle_set_role_cnf(rsp);
            break;
        default:
            break;
    }
    return ret;
}

#if 0
static void bt_sink_srv_aws_mp3_init()
{
    uint32_t i;
    int8_t ori_aid = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);

    ori_aid = dev->mp3->aud_id;
    if (dev->mp3->aud_id != BT_SINK_SRV_INVALID_AID) {
        bt_sink_srv_ami_audio_close(dev->mp3->aud_id);
        dev->mp3->aud_id = BT_SINK_SRV_INVALID_AID;
    }
    dev->mp3->aud_id = bt_sink_srv_ami_audio_open(AUD_MIDDLE, bt_sink_srv_aws_mp3_ami_hdr);
    bt_sink_srv_report("[sink][aws]init-aws_aid: %d, ori: %d",
                       dev->mp3->aud_id, ori_aid);
}
#endif


static void bt_sink_srv_aws_mp3_reset_aws_device(bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_assert(dev);
    BT_SINK_SRV_REMOVE_FLAG(dev->conn_bit, BT_SINK_SRV_MUSIC_AWS_CONN_BIT);
    bt_sink_srv_memset(&(dev->dev_addr), 0, sizeof(bt_bd_addr_t));
    dev->aws_role = BT_AWS_ROLE_NONE;
    dev->gap_role = BT_SINK_SRV_GAP_ROLE_UNKNOWN;
    dev->aws_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
    // reset clock offset to prevent set wrong clock skew value
    dev->offset.offset = 0;
    dev->offset.offset_intra = 0;
}


static int32_t bt_sink_srv_aws_mp3_handle_connect_cnf(bt_aws_connect_cnf_t *conn_cnf)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = 0, err = 0;
    bt_bd_addr_t *dev_addr = NULL;
    uint32_t max_packet_size = 0;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD, (void *)(&(conn_cnf->handle)));

    bt_sink_srv_report("[sink][aws]connect_cnf-aws_hd: 0x%x", conn_cnf->handle);

    if (!dev) {
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
        bt_sink_srv_report("[sink][aws]get local dev %x", dev);
    }

    if (conn_cnf->status == BT_STATUS_SUCCESS) {
        dev->aws_hd = conn_cnf->handle;
        dev_addr = bt_sink_srv_aws_get_address_by_handle(conn_cnf->handle);
        bt_sink_srv_memcpy(&(dev->dev_addr), dev_addr, sizeof(bt_bd_addr_t));

        BT_SINK_SRV_SET_FLAG(dev->conn_bit, BT_SINK_SRV_MUSIC_AWS_CONN_BIT);
        dev->gap_role = bt_sink_srv_cm_get_gap_role(&dev->dev_addr);

        // set mtu size
        max_packet_size = bt_aws_get_max_streaming_packet_size(conn_cnf->handle);
        bt_sink_srv_mp3_set_max_read_size(max_packet_size-sizeof(bt_sink_srv_mp3_media_header));

        if ((dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_PLAYING) &&
                (dev->aws_role != BT_AWS_ROLE_SOURCE)) {
            BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_AWS_STATE_SYNC);
            bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_PLAYING);
            bt_sink_srv_music_mp3_state_process();
        }
    } else {
        bt_sink_srv_report("[sink][aws]connect_cnf failed %x", conn_cnf->status);
        return ret;
    }

    if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_SOURCE_AVAILABLE)) {
        bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
        bt_sink_srv_music_mp3_state_process();
    }


    bt_sink_srv_report("[sink][aws]connect_cnf(e)-ret: %d, err: %d", ret, err);
    return ret;
}


static int32_t bt_sink_srv_aws_mp3_handle_disconnect_cnf(bt_aws_disconnect_cnf_t *disconn_cnf)
{
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *ctx = NULL;
    uint8_t source_flag = 0;
    int32_t ret = 0, err = 0;

    ctx = bt_sink_srv_music_get_context();

    bt_sink_srv_report("[sink][aws]disconnect_cnf[s]-state: %d, c_hd: 0x%x",
        ctx->state, disconn_cnf->handle);

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD, (void *)(&(disconn_cnf->handle)));

    if (dev) {
        if (BT_AWS_ROLE_SOURCE == dev->aws_role) {
            hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO);
            source_flag = 1;
        } else {
            if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_SOURCE_AVAILABLE) {
                bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
            } else {
                bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
                /* Currently no process can access none status, if need use
                 * none status, need add more flag */
                //bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_NONE);
            }
            bt_sink_srv_music_mp3_state_process();
        }
        bt_sink_srv_aws_mp3_reset_aws_device(dev);
        /* Maybe stuck on some state like sending commands after disconnected */
        if (source_flag == 1){
            bt_sink_srv_music_mp3_recovery_local_state(BT_SINK_SRV_MP3_RECOVERY_STATUS_DISCONNECT);
        }
    }

    ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;

    bt_sink_srv_report("[sink][aws]disconnect_cnf[e]-ret: %d, err: %d", ret, err);

    return ret;
}


static int32_t bt_sink_srv_aws_mp3_handle_disconnect_ind(bt_aws_disconnect_ind_t *disconn_ind)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = 0, err = 0;
    uint8_t source_flag = 0;
    bt_sink_srv_report("[sink][aws]disconnect_ind[s]-c_hd: 0x%x", disconn_ind->handle);
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD, (void *)(&(disconn_ind->handle)));
    if (dev) {
        if (BT_AWS_ROLE_SOURCE == dev->aws_role) {
            source_flag = 1;
            hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO);
        } else {
            if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_SOURCE_AVAILABLE) {
                bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
            } else {
                //bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_NONE);
                bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
            }
            bt_sink_srv_music_mp3_state_process();
        }
        bt_sink_srv_aws_mp3_reset_aws_device(dev);
        /* Maybe stuck on some state like sending commands after disconnected */
        if (source_flag == 1){
            bt_sink_srv_music_mp3_recovery_local_state(BT_SINK_SRV_MP3_RECOVERY_STATUS_DISCONNECT);
        }
    }
    bt_sink_srv_report("[sink][aws]disconnect_ind[e]-ret: %d, err: %d", ret, err);
    return ret;
}


static int32_t bt_sink_srv_aws_mp3_handle_play_ind(bt_aws_notify_play_ind_t *play_ind)
{
    hal_gpt_status_t gpt_ret = HAL_GPT_STATUS_OK;
    int32_t ret = 0;
    int32_t err = 0;
    uint32_t time_dur = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_aws_sync_info *sync_info = NULL;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD, (void *)(&(play_ind->handle)));
    sync_info = &(dev->local_info);
    if (BT_AWS_ROLE_SINK == dev->aws_role &&
            dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN) {
        bt_sink_srv_memcpy(&(dev->local_info), play_ind->play_notify->user_data,
                sizeof(bt_sink_srv_music_aws_sync_info));
        // set local play clock
        sync_info->clock.nclk = play_ind->play_notify->play_bt_clock.nclk;
        sync_info->clock.nclk_intra = play_ind->play_notify->play_bt_clock.nclk_intra;

        bt_sink_srv_music_mp3_state_process();
    }

    bt_sink_srv_report("[sink][aws]play_ind[e]-ret: %d, dur %d, err: %d, gpt_ret: %d",
        ret, time_dur, err, gpt_ret);

    return ret;
}


static int32_t bt_sink_srv_aws_mp3_handle_streaming_received_ind(bt_aws_streaming_received_ind_t *str_ind)
{
    int32_t ret = 0;
    bt_aws_custom_packet_t *packet;
    bt_sink_srv_music_device_t *dev = NULL;
    packet = &str_ind->packet.custom_packet;
    bt_sink_srv_music_disable_log();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD, (void *)(&(str_ind->handle)));
    bt_sink_srv_music_enable_log();

    if (packet->data_length == 0 || !(dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN)) {
        bt_sink_srv_report("skip packet, length: %d codec:%x %x",
                packet->data_length, dev, dev->op);
        return ret;
    }

    bt_sink_srv_aws_mp3_handle_streaming_ind(packet);

    return ret;
}


static int32_t bt_sink_srv_aws_mp3_handle_offset_update_ind(bt_aws_offset_update_ind_t *offset_ind)
{
    int32_t ret = 0;
    bt_sink_srv_audio_sync_calc_t audio_calc = {0};
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_music_disable_log();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD, (void *)(&(offset_ind->handle)));
    bt_sink_srv_music_enable_log();
    ctx = bt_sink_srv_music_get_context();

    if (dev == NULL) {
        bt_sink_srv_report("[sink][aws]fail to find device");
        return ret;
    }

    dev->offset.offset = offset_ind->offset.offset;
    dev->offset.offset_intra = offset_ind->offset.offset_intra;
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET);
    bt_sink_srv_report("[sink][aws]got offset %x intra %x",
            offset_ind->offset.offset, offset_ind->offset.offset_intra);

    // set clock skew if device disconnected before
    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_AWS_STATE_SYNC) {
        bt_sink_srv_audio_clock_init(&offset_ind->offset);
    // clock skew check
    } else if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_PLAYING) {
        audio_calc.media_handle= NULL;
        audio_calc.sampling_rate = ctx->med_play_info.sample_rate;
        audio_calc.type = BT_AWS_CODEC_TYPE_MP3;
        bt_sink_srv_audio_clock_calibrate(&audio_calc);
    }

    return ret;
}


static int32_t bt_sink_srv_aws_mp3_handle_start_cnf(bt_sink_srv_aws_mp3_packet *start_cnf)
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *dev = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD, (void *)(&(start_cnf->handle)));
    if (bt_sink_srv_timer_is_exist(BT_SINK_SRV_TIMER_AWS_MP3_REQUEST_DELAY)) {
        bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_AWS_MP3_REQUEST_DELAY);
    }
    if (start_cnf->status != BT_STATUS_SUCCESS) {
        bt_sink_srv_report("[sink][aws]handle_start_cnf not accept by sink");
        bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK);
    }

    bt_sink_srv_report("[sink][aws]handle_start_cnf[s]");
    bt_sink_srv_music_mp3_state_process();
    return ret;
}


static int32_t bt_sink_srv_aws_mp3_handle_start_ind(bt_sink_srv_aws_mp3_packet *start_ind)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = 0;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD, (void *)(&(start_ind->handle)));
    if (dev) {
        if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT &&
                !(dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START)) {
            /* No UE can play mp3 when currenlty audio device is not MP3 */
            bt_sink_srv_report("[sink][aws_mp3]start fail for interrupt");
            bt_sink_srv_aws_mp3_start_response(start_ind->handle, BT_STATUS_FAIL);
            return ret;
        }

        BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
        if (bt_sink_srv_music_mp3_set_target_and_process(AUDIO_SRC_SRV_STATE_PLAYING) == false) {
            bt_sink_srv_aws_mp3_start_response(start_ind->handle, BT_STATUS_SUCCESS);
        }
        bt_sink_srv_report("[sink][aws_mp3]start_ind-hd: %x", start_ind->handle);
    } else {
        /* upexpected state */
        ret = BT_SINK_SRV_MUSIC_ERR_FAIL_7TH;
    }

    return ret;
}


static int32_t bt_sink_srv_aws_mp3_handle_stop_cnf(bt_sink_srv_aws_mp3_packet *suspend_cnf)
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *dev = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD, (void *)(&(suspend_cnf->handle)));
    if (bt_sink_srv_timer_is_exist(BT_SINK_SRV_TIMER_AWS_MP3_REQUEST_DELAY)) {
        bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_AWS_MP3_REQUEST_DELAY);
    }

    if (dev && dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT) {
        return ret;
    }

    // check it's timing issue device did not auto play music
    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK) {
        bt_sink_srv_music_mp3_state_process();
    } else {
        bt_sink_srv_state_reset(BT_SINK_SRV_STATE_LOCAL_PLAYING);
    }
    bt_sink_srv_report("[sink][aws]handle_stop_cnf--flag: 0x%x", dev->flag);


    return ret;
}


static int32_t bt_sink_srv_aws_mp3_handle_stop_ind(bt_sink_srv_aws_mp3_packet *suspend_ind)
{
    int32_t ret = 0, err = 0;
    bt_sink_srv_music_device_t *dev = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD, (void *)(&(suspend_ind->handle)));
    ret = bt_sink_srv_aws_mp3_stop_response(suspend_ind->handle, BT_STATUS_SUCCESS);

    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT) {
        return ret;
    }

    bt_sink_srv_memset(&(dev->local_info), 0, sizeof(bt_sink_srv_music_aws_sync_info));
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK);
    // Received stop ind which means no data will received later
    // Check the mp3 task is running by get data in shared buffer
    // 1. wait mp3 task running done
    if (bt_sink_srv_mp3_over_threshold()) {
        bt_sink_srv_music_mp3_set_target_and_process(AUDIO_SRC_SRV_STATE_READY);
    // 2. no mp3 task running just process state
    } else {
        bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
        bt_sink_srv_music_mp3_state_process();
    }

    bt_sink_srv_report("[sink][aws]suspend_streaming_ind[e]-ret: %d, err: %d", ret, err);
    return ret;
}


static int32_t bt_sink_srv_aws_mp3_handle_change_track_cnf(bt_sink_srv_aws_mp3_packet *cnf)
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *dev = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD, (void *)(&(cnf->handle)));
    if (bt_sink_srv_timer_is_exist(BT_SINK_SRV_TIMER_AWS_MP3_REQUEST_DELAY)) {
        bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_AWS_MP3_REQUEST_DELAY);
    }

    if (cnf->status != BT_STATUS_SUCCESS) {
        bt_sink_srv_report("[sink][aws]handle_change_track_cnf not accept by sink");
        bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK);
    }


    bt_sink_srv_report("[sink][aws]handle_change_track_cnf[s]");
    bt_sink_srv_music_mp3_state_process();
    return ret;
}


static int32_t bt_sink_srv_aws_mp3_handle_change_track_ind(bt_sink_srv_aws_mp3_packet *ind)
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *dev = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD, (void *)(&(ind->handle)));
    if (dev) {

        if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT) {
            /* No UE can play mp3 when currenlty audio device is not MP3 */
            bt_sink_srv_report("[sink][aws_mp3]change track fail for interrupt");
            bt_sink_srv_aws_mp3_change_track_response(ind->handle, BT_STATUS_FAIL);
            return ret;
        }

        BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
        BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK);
        // Received change track ind which means no data will received later
        // Check the mp3 task is running by get data in shared buffer
        // 1. wait mp3 task running done
        if (bt_sink_srv_mp3_over_threshold()) {
            bt_sink_srv_music_mp3_set_target_and_process(AUDIO_SRC_SRV_STATE_READY);
        // 2. no mp3 task running just process state
        } else {
            bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
            bt_sink_srv_music_mp3_state_process();
        }
        bt_sink_srv_report("[sink][aws_mp3]ind-hd: %d", ind->handle);
    } else {
        /* upexpected state */
        ret = BT_SINK_SRV_MUSIC_ERR_FAIL_7TH;
    }

    return ret;
}


static int32_t bt_sink_srv_aws_mp3_handle_set_role_cnf(bt_sink_srv_aws_mp3_packet *aws_role_set_ind)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t err = 0;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD, (void *)(&(aws_role_set_ind->handle)));
    if (bt_sink_srv_timer_is_exist(BT_SINK_SRV_TIMER_AWS_MP3_REQUEST_DELAY)) {
        bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_AWS_MP3_REQUEST_DELAY);
    }

    if (dev && aws_role_set_ind->status == BT_STATUS_SUCCESS) {
        /* AWS SNK role */
        if (BT_AWS_ROLE_SINK == dev->aws_role) {
            hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO_BOTH_R_CHANNEL);
        } else if (BT_AWS_ROLE_SOURCE == dev->aws_role) {
            hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO_BOTH_L_CHANNEL);

            bt_sink_srv_aws_mp3_notify_vol_level(dev->aws_hd, (bt_sink_srv_am_volume_level_out_t)dev->mp3->vol_lev);
            bt_sink_srv_music_mp3_state_process();
        }
    } else {
        dev->aws_role = BT_AWS_ROLE_NONE;
        ret = BT_SINK_SRV_MUSIC_ERR_FAIL_7TH;
    }

    bt_sink_srv_report("[sink][aws]role_set_ind-aws_hd: 0x%x, ret: %d, role: %d, err: %d",
        aws_role_set_ind->handle, ret, aws_role_set_ind->role, err);

    return ret;
}

static int32_t bt_sink_srv_aws_mp3_handle_set_role_ind(bt_sink_srv_aws_mp3_packet *aws_role_set_ind)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t err = 0;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD, (void *)(&(aws_role_set_ind->handle)));

    if (dev) {
        dev->aws_role = aws_role_set_ind->role;

        /* AWS SNK role */
        if (BT_AWS_ROLE_SINK == dev->aws_role) {
            hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO_BOTH_R_CHANNEL);
        } else if (BT_AWS_ROLE_SOURCE == dev->aws_role) {
            hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO_BOTH_L_CHANNEL);
        }
        bt_sink_srv_aws_mp3_set_role_response(dev->aws_hd, BT_STATUS_SUCCESS);
    } else {
        ret = BT_SINK_SRV_MUSIC_ERR_FAIL_7TH;
        bt_sink_srv_aws_mp3_set_role_response(dev->aws_hd, BT_STATUS_FAIL);
    }

    bt_sink_srv_report("[sink][aws]role_set_ind-aws_hd: 0x%x, ret: %d, role: %d, err: %d",
        aws_role_set_ind->handle, ret, aws_role_set_ind->role, err);

    return ret;
}


#if 0
// todo:
static void bt_sink_srv_aws_mp3_update_sync_time(bt_sink_srv_music_aws_sync_info *sync_info)
{
    bt_aws_clock_t cur_clock = {0};
    bt_aws_get_bt_local_time(&cur_clock);

}
#endif


int32_t bt_sink_srv_aws_mp3_set_role_request(uint32_t handle, bt_aws_role_t role)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    uint8_t buf[sizeof(bt_sink_srv_aws_cmd_t) + sizeof(bt_sink_srv_aws_mp3_packet)] = {0};
    bt_sink_srv_aws_cmd_t *req = (bt_sink_srv_aws_cmd_t *)buf;
    bt_sink_srv_aws_mp3_packet *data = (bt_sink_srv_aws_mp3_packet *)req->data;
    req->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_SET_ROLE;
    req->data_len = sizeof(bt_sink_srv_aws_mp3_packet);
    data->handle = handle;
    data->role = role;

    ret = bt_sink_srv_aws_mp3_send_request(handle, req);
    bt_sink_srv_report("[sink][aws]set role req-ret: %d", ret);

    return ret;
}

int32_t bt_sink_srv_aws_mp3_set_role_response(uint32_t handle, bt_status_t status)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    uint8_t buf[sizeof(bt_sink_srv_aws_cmd_t) + sizeof(bt_sink_srv_aws_mp3_packet)] = {0};
    bt_sink_srv_aws_cmd_t *resp = (bt_sink_srv_aws_cmd_t *)buf;
    bt_sink_srv_aws_mp3_packet *data = (bt_sink_srv_aws_mp3_packet *)resp->data;
    resp->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_SET_ROLE;
    resp->data_len = sizeof(bt_sink_srv_aws_mp3_packet);
    data->handle = handle;
    data->status = status;

    ret = bt_sink_srv_aws_mp3_send_response(handle, resp);
    bt_sink_srv_report("[sink][aws]set role resp-ret: %d", ret);

    return ret;
}

int32_t bt_sink_srv_aws_mp3_start_request(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    uint8_t buf[sizeof(bt_sink_srv_aws_cmd_t) + sizeof(bt_sink_srv_aws_mp3_packet)] = {0};
    bt_sink_srv_aws_cmd_t *req = (bt_sink_srv_aws_cmd_t *)buf;
    bt_sink_srv_aws_mp3_packet *data = (bt_sink_srv_aws_mp3_packet *)req->data;
    req->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_START;
    req->data_len = sizeof(bt_sink_srv_aws_mp3_packet);
    data->handle = handle;

    ret = bt_sink_srv_aws_mp3_send_request(handle, req);
    bt_sink_srv_report("%02x %02x %02x %02x %02x %02x %02x %02x", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
    bt_sink_srv_report("[sink][aws]start req-ret: %d", ret);

    return ret;
}

int32_t bt_sink_srv_aws_mp3_start_response(uint32_t handle, bt_status_t status)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    uint8_t buf[sizeof(bt_sink_srv_aws_cmd_t) + sizeof(bt_sink_srv_aws_mp3_packet)] = {0};
    bt_sink_srv_aws_cmd_t *resp = (bt_sink_srv_aws_cmd_t *)buf;
    bt_sink_srv_aws_mp3_packet *data = (bt_sink_srv_aws_mp3_packet *)resp->data;
    resp->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_START;
    resp->data_len = sizeof(bt_sink_srv_aws_mp3_packet);
    data->handle = handle;
    data->status = status;
    ret = bt_sink_srv_aws_mp3_send_response(handle, resp);
    bt_sink_srv_report("[sink][aws]start resp-ret: %d", ret);

    return ret;
}

int32_t bt_sink_srv_aws_mp3_stop_request(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    uint8_t buf[sizeof(bt_sink_srv_aws_cmd_t) + sizeof(bt_sink_srv_aws_mp3_packet)] = {0};
    bt_sink_srv_aws_cmd_t *req = (bt_sink_srv_aws_cmd_t *)buf;
    bt_sink_srv_aws_mp3_packet *data = (bt_sink_srv_aws_mp3_packet *)req->data;
    req->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_STOP;
    req->data_len = sizeof(bt_sink_srv_aws_mp3_packet);
    data->handle = handle;

    ret = bt_sink_srv_aws_mp3_send_request(handle, req);
    bt_sink_srv_report("[sink][aws]stop req - ret: %d", ret);

    return ret;
}

int32_t bt_sink_srv_aws_mp3_stop_response(uint32_t handle, bt_status_t status)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    uint8_t buf[sizeof(bt_sink_srv_aws_cmd_t) + sizeof(bt_sink_srv_aws_mp3_packet)] = {0};
    bt_sink_srv_aws_cmd_t *resp = (bt_sink_srv_aws_cmd_t *)buf;
    bt_sink_srv_aws_mp3_packet *data = (bt_sink_srv_aws_mp3_packet *)resp->data;
    resp->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_STOP;
    resp->data_len = sizeof(bt_sink_srv_aws_mp3_packet);
    data->handle = handle;
    data->status = status;

    ret = bt_sink_srv_aws_mp3_send_response(handle, resp);
    bt_sink_srv_report("[sink][aws]stop resp-ret: %d", ret);

    return ret;
}

int32_t bt_sink_srv_aws_mp3_change_track_request(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    uint8_t buf[sizeof(bt_sink_srv_aws_cmd_t) + sizeof(bt_sink_srv_aws_mp3_packet)] = {0};
    bt_sink_srv_aws_cmd_t *req = (bt_sink_srv_aws_cmd_t *)buf;
    bt_sink_srv_aws_mp3_packet *data = (bt_sink_srv_aws_mp3_packet *)req->data;
    req->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_NEXT;
    req->data_len = sizeof(bt_sink_srv_aws_mp3_packet);
    data->handle = handle;

    ret = bt_sink_srv_aws_mp3_send_request(handle, req);
    bt_sink_srv_report("[sink][aws]change track req - ret: %d", ret);

    return ret;
}

int32_t bt_sink_srv_aws_mp3_change_track_response(uint32_t handle, bt_status_t status)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    uint8_t buf[sizeof(bt_sink_srv_aws_cmd_t) + sizeof(bt_sink_srv_aws_mp3_packet)] = {0};
    bt_sink_srv_aws_cmd_t *resp = (bt_sink_srv_aws_cmd_t *)buf;
    bt_sink_srv_aws_mp3_packet *data = (bt_sink_srv_aws_mp3_packet *)resp->data;
    resp->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_NEXT;
    resp->data_len = sizeof(bt_sink_srv_aws_mp3_packet);
    data->handle = handle;
    data->status = status;

    ret = bt_sink_srv_aws_mp3_send_response(handle, resp);
    bt_sink_srv_report("[sink][aws]change track resp-ret: %d", ret);

    return ret;
}

/* transfer SRC volume setting */
int32_t bt_sink_srv_aws_mp3_notify_vol_level(uint32_t handle, bt_sink_srv_am_volume_level_out_t level)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    uint8_t buf[sizeof(bt_sink_srv_aws_cmd_t) + sizeof(bt_sink_srv_aws_mp3_packet)] = {0};
    bt_sink_srv_aws_cmd_t *notify = (bt_sink_srv_aws_cmd_t *)buf;
    bt_sink_srv_aws_mp3_packet *data = (bt_sink_srv_aws_mp3_packet *)notify->data;
    notify->cmd_id = BT_SINK_SRV_AWS_NOTIFY_SET_VOLUME;
    notify->data_len = sizeof(bt_sink_srv_aws_mp3_packet);
    data->handle = handle;
    data->vol_lev = level;

    ret = bt_sink_srv_aws_mp3_send_request(handle, notify);
    bt_sink_srv_report("[sink][aws]set volume %d notification-ret: %d", level, ret);

    return ret;
}


int32_t bt_sink_srv_aws_mp3_notify_start(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    uint8_t buf[sizeof(bt_sink_srv_aws_cmd_t) + sizeof(bt_sink_srv_aws_mp3_packet)] = {0};
    bt_sink_srv_aws_cmd_t *notify = (bt_sink_srv_aws_cmd_t *)buf;
    notify->cmd_id = BT_SINK_SRV_AWS_NOTIFY_START;

    ret = bt_sink_srv_aws_mp3_send_request(handle, notify);
    bt_sink_srv_report("[sink][aws]start notification-ret: %d", ret);

    return ret;
}


int32_t bt_sink_srv_aws_mp3_notify_stop(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    uint8_t buf[sizeof(bt_sink_srv_aws_cmd_t) + sizeof(bt_sink_srv_aws_mp3_packet)] = {0};
    bt_sink_srv_aws_cmd_t *notify = (bt_sink_srv_aws_cmd_t *)buf;
    notify->cmd_id = BT_SINK_SRV_AWS_NOTIFY_STOP;

    ret = bt_sink_srv_aws_mp3_send_request(handle, notify);
    bt_sink_srv_report("[sink][aws]stop notification-ret: %d", ret);
    return ret;
}


int32_t bt_sink_srv_aws_mp3_notify_change_ntrack(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    uint8_t buf[sizeof(bt_sink_srv_aws_cmd_t) + sizeof(bt_sink_srv_aws_mp3_packet)] = {0};
    bt_sink_srv_aws_cmd_t *notify = (bt_sink_srv_aws_cmd_t *)buf;
    notify->cmd_id = BT_SINK_SRV_AWS_NOTIFY_NEXT;

    ret = bt_sink_srv_aws_mp3_send_request(handle, notify);
    bt_sink_srv_report("[sink][aws]next notification-ret: %d", ret);
    return ret;
}


int32_t bt_sink_srv_aws_mp3_notify_change_ptrack(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    uint8_t buf[sizeof(bt_sink_srv_aws_cmd_t) + sizeof(bt_sink_srv_aws_mp3_packet)] = {0};
    bt_sink_srv_aws_cmd_t *notify = (bt_sink_srv_aws_cmd_t *)buf;
    notify->cmd_id = BT_SINK_SRV_AWS_NOTIFY_PREV;

    ret = bt_sink_srv_aws_mp3_send_request(handle, notify);
    bt_sink_srv_report("[sink][aws]prev notification-ret: %d", ret);
    return ret;
}


static void bt_sink_srv_aws_mp3_cmd_timeout_handler(void *params)
{
    bt_sink_srv_report("[sink][aws]command timeout, start to disconnect link");
    bt_sink_srv_cm_disconnect_by_type(BT_SINK_SRV_PROFILE_AWS);
}


static inline bt_status_t bt_sink_srv_aws_mp3_send_request(uint32_t handle,
        bt_sink_srv_aws_cmd_t *req)

{
    req->type = BT_SINK_SRV_AWS_COMMAND_TYPE_REQ;

    // set timer for command response timeout
    if (req->cmd_id < BT_SINK_SRV_AWS_COMMAND_END) {
        bt_sink_srv_timer_start(BT_SINK_SRV_TIMER_AWS_MP3_REQUEST_DELAY,
                                BT_SINK_SRV_MP3_AWS_COMMAND_WAIT_DUR,
                                bt_sink_srv_aws_mp3_cmd_timeout_handler,
                                NULL);
    }

    return bt_sink_srv_aws_send_cmd(handle, BT_SINK_SRV_AWS_MODULE_ID_MP3, req);
}

static inline bt_status_t bt_sink_srv_aws_mp3_send_response(uint32_t handle,
        bt_sink_srv_aws_cmd_t *resp)
{
    resp->type = BT_SINK_SRV_AWS_COMMAND_TYPE_RSP;
    return bt_sink_srv_aws_send_cmd(handle, BT_SINK_SRV_AWS_MODULE_ID_MP3, resp);
}

int32_t bt_sink_srv_aws_mp3_build_streaming_packet(uint8_t *mp3_hdr,
          uint8_t *data, int32_t data_len, bt_aws_streaming_packet_t *packet)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    packet->type = BT_AWS_STREAMING_TYPE_CUSTOM;
    packet->custom_packet.data = data;
    packet->custom_packet.data_length = data_len;
    packet->custom_packet.media_header = mp3_hdr;
    packet->custom_packet.media_header_len = sizeof(bt_sink_srv_mp3_media_header);

    return ret;
}

void bt_sink_srv_aws_mp3_send_sync_time(void)
{
    uint32_t time_dur = 0;
    hal_gpt_status_t gpt_ret = HAL_GPT_STATUS_OK;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_music_aws_sync_info *sync_info = NULL;
    bt_aws_play_notify_t notify = {0};
    bt_aws_clock_t bt_clock;
    int32_t err = 0;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    ctx = bt_sink_srv_music_get_context();
    sync_info = &(dev->local_info);

    bt_sink_srv_report("[sink][aws](mgr)handle_data-(aws)--con_bit: %d", dev->conn_bit);

    time_dur = BT_SINK_SRV_MP3_AWS_TIMER_DUR;
    if (ctx->gpt_port == HAL_GPT_MAX_PORT) {
        if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_PLAYING)) {
            if (hal_gpt_init(HAL_GPT_1) == HAL_GPT_STATUS_OK) {
                ctx->gpt_port = HAL_GPT_1;
            } else if (hal_gpt_init(HAL_GPT_2) == HAL_GPT_STATUS_OK) {
                ctx->gpt_port = HAL_GPT_2;
            } else {
                err = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
                bt_sink_srv_report("[sink][aws](mgr) init gpt failed");
                return;
            }
            hal_gpt_register_callback(ctx->gpt_port,
                bt_sink_srv_aws_mp3_gpt_cb, NULL);
            // disable irq to prevent been interrupt between get bt clk and start timer
            __disable_irq();
            bt_aws_get_bt_local_time(&sync_info->clock);
            bt_aws_get_bt_time_by_local_time(dev->gap_role, dev->offset, sync_info->clock, &bt_clock);
            sync_info->pkt_info.sn = 0;
            sync_info->pkt_info.ts = 0;

            gpt_ret = hal_gpt_start_timer_us(ctx->gpt_port, time_dur-2000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
            dev->play_time.nclk = (bt_clock.nclk + (time_dur * 4 / 1250)) & 0x0FFFFFFF;
            dev->play_time.nclk_intra = bt_clock.nclk_intra + (time_dur % 1250);
            __enable_irq();
            bt_sink_srv_report("[sink][aws]local clk %x %x",
                    sync_info->clock.nclk,
                    sync_info->clock.nclk_intra);
        } else {
            bt_aws_get_bt_time_by_local_time(dev->gap_role, dev->offset, sync_info->clock, &bt_clock);
            bt_sink_srv_report("[sink][aws]get play bt clk %x %x local clk %x %x",
                    bt_clock.nclk,
                    bt_clock.nclk_intra,
                    sync_info->clock.nclk,
                    sync_info->clock.nclk_intra);
        }
        if (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) {
            bt_sink_srv_report("[sink][aws]send sync time dur %x ts %x clk %x intra %x role %d",
                time_dur,
                sync_info->pkt_info.ts,
                bt_clock.nclk,
                bt_clock.nclk_intra,
                dev->gap_role);

            notify.play_time = sync_info->pkt_info.ts;
            notify.sequence_num = sync_info->pkt_info.sn;
            notify.play_bt_clock.nclk = bt_clock.nclk;
            notify.play_bt_clock.nclk_intra = bt_clock.nclk_intra;
            notify.data_len = sizeof(bt_sink_srv_music_aws_sync_info);
            notify.user_data = (void *)(sync_info);
            notify.bt_local_role = dev->gap_role;

            err = bt_aws_notify_play(dev->aws_hd,
                BT_SINK_SRV_AWS_MODULE_ID_MP3, &notify);
        }
    } else {
        err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

    bt_sink_srv_report("[sink][aws](mgr)handle_data-(aws)--gpt ret: %d, err: %d", gpt_ret, err);
}

int32_t bt_sink_srv_aws_mp3_wait_sync_time(void)
{
    hal_gpt_status_t gpt_ret = HAL_GPT_STATUS_OK;
    int32_t err = 0;
    uint32_t time_dur = 0;
    bt_sink_srv_music_context_t *ctx = NULL;

    ctx = bt_sink_srv_music_get_context();

    time_dur = bt_sink_srv_mp3_find_suitable_frame();
    if ((ctx->gpt_port == HAL_GPT_MAX_PORT) &&
            (time_dur != BT_SINK_SRV_MP3_AWS_INVALID_DUR)) {
        if (hal_gpt_init(HAL_GPT_1) == HAL_GPT_STATUS_OK) {
            ctx->gpt_port = HAL_GPT_1;
        } else if (hal_gpt_init(HAL_GPT_2) == HAL_GPT_STATUS_OK) {
            ctx->gpt_port = HAL_GPT_2;
        } else {
            err = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
            bt_sink_srv_report("[sink][aws](mgr) init gpt failed");
            return err;
        }
        hal_gpt_register_callback(ctx->gpt_port,
            bt_sink_srv_aws_mp3_gpt_cb, NULL);
        gpt_ret = hal_gpt_start_timer_us(ctx->gpt_port, time_dur - 2000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
    } else {
        err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }
    bt_sink_srv_report("[sink][aws]wait sync time %x err %d ret %d", time_dur, err, gpt_ret);

    return err;
}
#endif /* __BT_AWS_SUPPORT__ */

