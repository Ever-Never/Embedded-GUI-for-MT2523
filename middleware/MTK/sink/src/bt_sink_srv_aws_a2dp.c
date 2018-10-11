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

#include "bt_sink_srv_ami.h"

#ifdef __BT_AWS_A2DP_SUPPORT__
#include "bt_sink_srv_aws.h"
#include "bt_sink_srv_aws_a2dp.h"
#include "bt_sink_srv_music.h"
#include "bt_aws.h"
#include "audio_src_srv.h"
#include "bt_sink_srv_utils.h"
#include "sink_srv_util.h"
#include "bt_sink_srv_a2dp.h"
#include "bt_sink_srv_avrcp.h"
#include "bt_sink_srv_state_notify.h"
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
#include "bt_timer_external.h"
#endif

#include "hal_gpt.h"


extern uint32_t sink_loc_play_nclk;
extern uint32_t sink_loc_play_nclk_intra;

static audio_src_srv_handle_t *g_aws_a2dp_pse_hd = NULL;

#define BT_SINK_SRV_AWS_DATA_RECE_CNT          (10)


//static uint32_t bt_sink_srv_aws_data_rece_cnt = 0;

bt_aws_streaming_received_ind_t *bt_sink_srv_aws_rece_ind = NULL;

bt_hci_packet_t *bt_sink_srv_aws_pkt = NULL;

extern uint32_t g_a2dp_gpt_run_count_begin;
extern uint32_t g_a2dp_gpt_run_count_end;

extern uint32_t g_a2dp_srv_rece_pkt_byte;
extern uint32_t g_a2dp_decoder_read_pkt_byte;

//const static uint8_t BT_SINK_SRV_AWS_AAC_SYNC_HEAD[BT_SINK_SRV_AAC_ADTS_LENGTH] = {0xFF, 0xF9, 0x40, 0x20, 0x00, 0x1F, 0xFC};

extern void bt_sink_srv_music_aws_a2dp_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param);

/* Static function declare */
static void bt_sink_srv_aws_a2dp_gpt_cb(void *user_data);

static int32_t bt_sink_srv_aws_a2dp_start_response(uint32_t handle, bt_sink_srv_music_device_t *dev, uint8_t accept);

static int32_t bt_sink_srv_aws_a2dp_suspend_response(uint32_t handle, bt_sink_srv_music_device_t *dev, uint8_t accept);

static bt_status_t bt_sink_srv_aws_a2dp_req_handle(uint32_t handle, bt_sink_srv_aws_cmd_ind_t *cmd_ind);

static bt_status_t bt_sink_srv_aws_a2dp_rsp_handle(uint32_t handle, bt_sink_srv_aws_cmd_rsp_t *cmd_rsp);

static void bt_sink_srv_aws_a2dp_get_bt_clk_offset(uint32_t evt_id,void * param,void * user_data);

static void bt_sink_srv_aws_a2dp_vol_up(void);

static void bt_sink_srv_aws_a2dp_vol_down(void);

static void bt_sink_srv_aws_a2dp_play_music(void);

static void bt_sink_srv_aws_a2dp_pause_music(void);

static void bt_sink_srv_aws_a2dp_next_track(void);

static void bt_sink_srv_aws_a2dp_pre_track(void);

static void bt_sink_srv_aws_a2dp_handle_volume_change(uint8_t vol_lev);

extern void bt_sink_srv_a2dp_reset_last_pkt_info(sink_media_packet_info_t *last_pkt_info);

extern int32_t bt_sink_srv_aws_a2dp_suspend_request(uint32_t handle, bt_sink_srv_music_device_t *dev);


void bt_sink_srv_aws_a2dp_init()
{
    int8_t ori_aid = 0;
    bt_sink_srv_music_context_t *ctx = NULL;

    ctx = bt_sink_srv_music_get_context();
    ori_aid = ctx->aws_aid;
    if (ctx->aws_aid != BT_SINK_SRV_INVALID_AID) {
        bt_sink_srv_ami_audio_close(ctx->aws_aid);
        ctx->aws_aid = BT_SINK_SRV_INVALID_AID;
    }

    ctx->aws_aid = bt_sink_srv_ami_audio_open(AUD_MIDDLE, bt_sink_srv_music_aws_a2dp_ami_hdr);
    ctx->vol_lev = BT_SINK_SRV_AWS_A2DP_VOL_DEF_LEV;
    bt_sink_srv_report("[sink][aws_a2dp]init-aws_aid: %d, ori: %d",
                       ctx->aws_aid, ori_aid);
}


static void bt_sink_srv_aws_a2dp_fill_am_aud_param(bt_sink_srv_am_audio_capability_t  *aud_cap, 
                                bt_a2dp_codec_capability_t *a2dp_cap, bt_aws_role_t role)
{
    bt_aws_codec_capability_t aws_cap = {0};
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    if (a2dp_cap->type == BT_A2DP_CODEC_SBC) {
        aws_cap.type = BT_AWS_CODEC_TYPE_SBC;
    } else if (a2dp_cap->type == BT_A2DP_CODEC_AAC) {
        aws_cap.type = BT_AWS_CODEC_TYPE_AAC;
    } else {
        /* trace error */
    }
    aws_cap.length = a2dp_cap->length;
    memcpy(&(aws_cap.codec), &(a2dp_cap->codec), a2dp_cap->length);

    memset(aud_cap, 0x00, sizeof(bt_sink_srv_am_audio_capability_t));
    aud_cap->type = AWS;
    aud_cap->codec.aws_format.aws_codec.role = role;
    memcpy(&(aud_cap->codec.aws_format.aws_codec.codec_cap), &(aws_cap), sizeof(bt_aws_codec_capability_t));
    aud_cap->audio_stream_out.audio_device = BT_SINK_SRV_AWS_A2DP_OUTPUT_DEVICE;
    aud_cap->audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t)ctx->vol_lev;
    aud_cap->audio_stream_out.audio_mute = false;
}


static void bt_sink_srv_aws_a2dp_clear_codec(bt_sink_srv_music_device_t *aws_a2dp_dev)
{
    int32_t ret = 0;
    bt_sink_srv_music_context_t *ctx = NULL;

    ctx = bt_sink_srv_music_get_context();


    /* Clear GPT Timer */
    if (ctx->gpt_port != HAL_GPT_MAX_PORT) {
        hal_gpt_stop_timer(ctx->gpt_port);
        hal_gpt_deinit(ctx->gpt_port);
        ctx->gpt_port = HAL_GPT_MAX_PORT;
    }

    /* Clear recover flag */
    BT_SINK_SRV_REMOVE_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);

    /* 1. DRV stop */
    if (aws_a2dp_dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY) {
        BT_SINK_SRV_SET_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP);
        /* Sync DRV stop, or codec close must wait DRV stop done(cost time) */
        sink_srv_util_send_event(SINK_SRV_EVT_CODEC_STOP, aws_a2dp_dev);
    }
    /* 2. Codec close */
    if (aws_a2dp_dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN) {
        if (aws_a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER) {
            /* Skip codec close */
            ;
        } else {
            ret = bt_sink_srv_ami_audio_stop(ctx->aws_aid);
            if (ret != AUD_EXECUTION_SUCCESS) {
                /* Failed close codec */
                bt_sink_srv_report("[sink][aws_a2dp]clear_codec(error)--ret: %d", ret);
            } else {
                BT_SINK_SRV_REMOVE_FLAG(aws_a2dp_dev->op, BT_SINK_SRV_MUSIC_OP_CODEC_OPEN);
            }
        }
    }
    /* 3. Clear flag */
    /* Clear wait start gpt timer */
    BT_SINK_SRV_REMOVE_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
    /* Clear wait start gpt timeout */
    BT_SINK_SRV_REMOVE_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_GPT_TIMEOUT);
    /* Clear recover flag */
    //BT_SINK_SRV_REMOVE_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);
}


void bt_sink_srv_aws_a2dp_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param)
{
    bt_sink_srv_music_context_t *ctx = NULL;
    int32_t ret = 0;
    int32_t err_code = 0;
    bt_sink_srv_music_device_t *run_dev = NULL;

    ctx = bt_sink_srv_music_get_context();
    run_dev = ctx->run_dev;
    //bt_sink_srv_assert(run_dev);
    if ((ctx->aws_aid == aud_id) &&
        (msg_id == AUD_AWS_A2DP_PROC_IND) && 
        (sub_msg == AUD_STREAM_EVENT_DATA_REQ || AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW == sub_msg)) {
        // drop
        ;
    } else {
        bt_sink_srv_report("[sink][aws_a2dp]ami_hdr[s]-aid: %d, aud_id: %d, msg_id: %d, sub_msg: %d, 2nd: 0x%x",
                           ctx->aws_aid, aud_id, msg_id, sub_msg, sub_msg);
    }

    if (ctx->aws_aid == aud_id) {
        switch (msg_id) {
            case AUD_SELF_CMD_REQ: {
                if (AUD_CMD_FAILURE == sub_msg) {
                }
                break;
            }

            case AUD_AWS_SINK_OPEN_CODEC: {
                if ((run_dev) && (run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC)) {
                    /* Save codec handle */
                    bt_sink_srv_memcpy(&(run_dev->med_handle), param, sizeof(bt_sink_srv_am_media_handle_t));

                    BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC);

                    /* Set codec open flag */
                    BT_SINK_SRV_SET_FLAG(run_dev->op, BT_SINK_SRV_MUSIC_OP_CODEC_OPEN);

                    /* Clear media node */
                    bt_sink_srv_media_mgr_clear();

                    if (run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET) {
                        /* Later accept */
                        BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
                    } else {
                        /* Accept start streaming */
                        ret = bt_sink_srv_aws_a2dp_start_response(run_dev->aws_hd, run_dev, 1);
                        if (BT_STATUS_SUCCESS == ret) {
                            bt_sink_srv_a2dp_reset_last_pkt_info(&(ctx->med_pkt_info));
                            sink_srv_init_media_play_info(&(ctx->med_play_info), &(run_dev->codec), run_dev->med_handle.med_hd);
                            bt_sink_srv_media_mgr_set_codec_type(run_dev->codec.type, &(run_dev->codec));
                            //ctx->threshold_count = sinv_srv_util_us_2_sample_count(BT_SINK_SRV_MUSIC_AWS_PREPARE_BUF_THRESHOLD, ctx->med_play_info.sample_rate);
                            ctx->threshold_count = 0;
                            ctx->simple_count = 0;
                            bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_CODEC_OPEN, NULL);
                        } else {
                            /* Error handle */
                            bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL, NULL);
                        }
                    }
                }
                break;
            }

            /* interrupt */
            case AUD_SUSPEND_BY_IND: {
                break;
            }

            case AUD_RESUME_IND: {
            }
            break;

            case AUD_AWS_A2DP_PROC_IND: {
                if (sub_msg == AUD_A2DP_CODEC_RESTART) {
                    bt_sink_srv_media_mgr_clear();
                    BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);
                    BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP);
                    sink_srv_util_send_event(SINK_SRV_EVT_CODEC_STOP, run_dev);
                } else if (sub_msg == AUD_A2DP_AWS_UNDERFLOW) {
                    bt_sink_srv_media_mgr_clear();
                    BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);
                    BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP);
                    sink_srv_util_send_event(SINK_SRV_EVT_CODEC_STOP, run_dev);
                }
                break;
            }

            default:
                break;
        }
    }

    if (ctx->aws_aid == aud_id && msg_id == AUD_AWS_A2DP_PROC_IND &&
        (sub_msg == AUD_STREAM_EVENT_DATA_REQ || AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW == sub_msg)) {
        // drop
        ;
    } else {
        bt_sink_srv_report("[sink][aws_a2dp]ami_hdr[e]-err_code: %d, ret: %d", err_code, ret);
    }
}



static int32_t bt_sink_srv_aws_a2dp_handle_connect_cnf(bt_aws_connect_cnf_t *conn_cnf)
{
    bt_sink_srv_music_device_t *dev = NULL, *sp_dev = NULL;
    int32_t ret = 0, err = 0;
    bt_bd_addr_t *dev_addr = NULL;

    bt_sink_srv_report("[sink][aws_a2dp]connect_cnf(s)-aws_hd: 0x%x", conn_cnf->handle);


    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_A2DP_HD, (void *)(&(conn_cnf->handle)));

    if (!dev) {
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
    }

    bt_sink_srv_assert(dev);

    if (conn_cnf->status == BT_STATUS_SUCCESS) {
        dev->aws_hd = conn_cnf->handle;
        dev_addr = bt_sink_srv_aws_get_address_by_handle(conn_cnf->handle);
        bt_sink_srv_memcpy(&(dev->dev_addr), dev_addr, sizeof(bt_bd_addr_t));
        BT_SINK_SRV_SET_FLAG(dev->conn_bit, BT_SINK_SRV_MUSIC_AWS_CONN_BIT);
        /* Init pse handle */
        dev->handle = g_aws_a2dp_pse_hd;
        bt_sink_srv_music_fill_audio_src_callback(g_aws_a2dp_pse_hd);
        dev->handle->priority = AUDIO_SRC_SRV_PRIORITY_NORMAL;
        bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_READY, NULL);
        dev->gap_role = bt_sink_srv_cm_get_gap_role(&(dev->dev_addr));
        dev->handle->dev_id = bt_sink_srv_music_convert_btaddr_to_devid(&(dev->dev_addr));
        bt_sink_srv_report("[sink][aws_a2dp]connect_cnf(ok)-dev: 0x%x, hd: 0x%x, gap_role: %d", dev, dev->aws_hd, dev->gap_role);
    } else {
        bt_sink_srv_music_state_machine_handle(dev, BT_AWS_CONNECT_CNF, NULL);
        /* Check a2dp & avrcp connection staus */
        if ((dev->conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT) ||
            (dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
            dev->aws_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        } else {
            bt_sink_srv_music_reset_device(dev);
        }
    }

    sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_SP, NULL);
    /* SPK1 case */
    if ((sp_dev) &&
        (sp_dev->conn_bit && BT_SINK_SRV_MUSIC_A2DP_CONN_BIT) &&
        (sp_dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN)) {
        /* Case 4. AWS: SP2 dynamic plug in */
        BT_SINK_SRV_SET_FLAG(sp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
        bt_sink_srv_aws_a2dp_start_request(dev->aws_hd, sp_dev);
        /* Need auto sync */
        BT_SINK_SRV_SET_FLAG(sp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_AWS_AUTO_SYNC);
        /* Set master channel L */
        hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO_BOTH_L_CHANNEL);
    } else {
        err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

    bt_sink_srv_report("[sink][aws_a2dp]connect_cnf(e)-ret: %d, err: %d", ret, err);
    return ret;
}


static int32_t bt_sink_srv_aws_a2dp_handle_disconnect_cnf(bt_aws_disconnect_cnf_t *disconn_cnf)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = 0, err = 0;
    bt_sink_srv_music_device_t *sp_dev = NULL;    
    bool free_hd = false;


    bt_sink_srv_report("[sink][aws_a2dp]disconnect_cnf[s]--aws_hd: 0x%x", disconn_cnf->handle);

    bt_sink_srv_music_disable_transfer_packet();

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_A2DP_HD, (void *)(&(disconn_cnf->handle)));
    sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_SP, NULL);

    if (dev) {
        BT_SINK_SRV_REMOVE_FLAG(dev->conn_bit, BT_SINK_SRV_MUSIC_AWS_CONN_BIT);
        /* Master */
        if (sp_dev) {
            free_hd = true;
            bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_UNAVAILABLE, NULL);
            if (sp_dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN) {
                hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO);
            }
        } else {
        /*  Slave */
            /* Clear recover flag */
            BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);
            if (!(dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY)) {
                free_hd = true;
            }
            bt_sink_srv_music_state_machine_handle(dev, BT_AWS_DISCONNECT_CNF, NULL);
        }
    }

    if (free_hd) {
        /* Deinit pse handle */
        //bt_sink_srv_a2dp_free_pseudo_handle(dev->handle);
        dev->handle = NULL;
        if (!(dev->conn_bit)) {
            bt_sink_srv_music_reset_device(dev);
        }
    }

    bt_sink_srv_report("[sink][aws_a2dp]disconnect_cnf[e]-ret: %d, err: %d", ret, err);

    return ret;
}


static int32_t bt_sink_srv_aws_a2dp_handle_disconnect_ind(bt_aws_disconnect_ind_t *disconn_ind)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = 0, err = 0;
    bt_sink_srv_music_device_t *sp_dev = NULL;    
    bool free_hd = false;

    bt_sink_srv_report("[sink][aws_a2dp]disconnect_ind[s]-aws_hd: 0x%x", disconn_ind->handle);

    bt_sink_srv_music_disable_transfer_packet();

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_A2DP_HD, (void *)(&(disconn_ind->handle)));
    sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_SP, NULL);

    if (dev) {
        BT_SINK_SRV_REMOVE_FLAG(dev->conn_bit, BT_SINK_SRV_MUSIC_AWS_CONN_BIT);
        /* Master */
        if (sp_dev) {
            free_hd = true;
            bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_UNAVAILABLE, NULL);
            if (sp_dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN) {
                hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO);
            }
        } else {
        /* Slave */
            /* Clear recover flag */
            BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);
            if (!(dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY)) {
                free_hd = true;
            }
            bt_sink_srv_music_state_machine_handle(dev, BT_AWS_DISCONNECT_IND, NULL);
        }
    }

    if (free_hd) {
        /* Deinit pse handle */
        //bt_sink_srv_a2dp_free_pseudo_handle(dev->handle);
        dev->handle = NULL;
        if (!(dev->conn_bit)) {
            bt_sink_srv_music_reset_device(dev);
        }
    }

    bt_sink_srv_report("[sink][aws_a2dp]disconnect_ind[e]-ret: %d, err: %d", ret, err);

    return ret;
}


static int32_t bt_sink_srv_aws_a2dp_handle_start_streaming_cnf(bt_sink_srv_music_aws_start_streaming_cnf_t *start_cnf)
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *sp_dev = NULL;    
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;
    bt_sink_srv_music_context_t *ctx = NULL;

    ctx = bt_sink_srv_music_get_context();

    sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);

    bt_sink_srv_report("[sink][aws_a2dp]start_cnf(s)--hd: 0x%08x, flag: 0x%08x, op: 0x%08x\n", start_cnf->handle, sp_dev->flag, sp_dev->op);

#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    if (bt_timer_ext_find(BT_SINK_SRV_A2DP_WAIT_AWS_CNF_TIMER_ID)) {
        bt_timer_ext_stop(BT_SINK_SRV_A2DP_WAIT_AWS_CNF_TIMER_ID);
    }
#endif

    if ((sp_dev) &&
        (sp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START)) {
        if (start_cnf->accept == 1) {
            BT_SINK_SRV_SET_FLAG(sp_dev->op, BT_SINK_SRV_MUSIC_OP_AWS_START);
            BT_SINK_SRV_REMOVE_FLAG(sp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
            if (sp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_AUTO_SYNC) {
                /* WSAP00036070 */
                if (sp_dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY) {
                    /* Update SPK1 bt clock */
                    bt_sink_srv_a2dp_update_base_bt_clock();
                } else {
                    /* Pending auto sync operation (update base bt clock), after SPK1 drv play done, continue auto sync flow */
                    BT_SINK_SRV_SET_FLAG(sp_dev->op, BT_SINK_SRV_MUSIC_OP_AUTO_SYNC);
                }
            } else if (sp_dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN) {
                ret = bt_a2dp_start_streaming_response(sp_dev->a2dp_hd, true);
                if (BT_STATUS_SUCCESS == ret) {
                    bt_sink_srv_music_enable_transfer_packet();
                    /* Reset last packet information */
                    bt_sink_srv_a2dp_reset_last_pkt_info(&(ctx->med_pkt_info));
                    sink_srv_init_media_play_info(&(ctx->med_play_info), &(sp_dev->codec), sp_dev->med_handle.med_hd);
                    bt_sink_srv_media_mgr_set_codec_type(sp_dev->codec.type, &(sp_dev->codec));
                    ctx->simple_count = 0;
                    ctx->threshold_count = sinv_srv_util_us_2_sample_count(BT_SINK_SRV_MUSIC_AWS_PREPARE_BUF_THRESHOLD, ctx->med_play_info.sample_rate);
                    bt_sink_srv_report("[sink][aws_a2dp]start_cnf--hold: %d", ctx->threshold_count);
                    bt_sink_srv_music_state_machine_handle(sp_dev, BT_SINK_SRV_MUSIC_EVT_CODEC_OPEN, NULL);
                } else {
                    /* Error handle */
                    bt_sink_srv_music_state_machine_handle(sp_dev, BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL, NULL);
                }
            }
        } else {
            bt_sink_srv_report("[sink][aws_a2dp]start_cnf(error_fail)--status: 0x%x, hd: 0x%x", start_cnf->accept, start_cnf->handle);
        }
    } else {
        bt_sink_srv_report("[sink][aws_a2dp]start_cnf(error_flag)--flag: 0x%x, hd: 0x%x", sp_dev->flag, start_cnf->handle);
        aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_A2DP_HD, (void *)(&(start_cnf->handle)));
        bt_sink_srv_aws_a2dp_suspend_request(start_cnf->handle, aws_a2dp_dev);
    }

    return ret;
}


static int32_t bt_sink_srv_aws_a2dp_handle_start_streaming_ind(bt_sink_srv_music_aws_start_streaming_ind_t *start_ind)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = 0;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_A2DP_HD, (void *)(&(start_ind->handle)));

    bt_sink_srv_report("[sink][aws_a2dp]strat_ind-hd: 0x%x, flag: 0x%x, op: 0x%x", start_ind->handle, dev->flag, dev->op);
    //bt_a2dp_clear_gpio_12();

    if (dev) {
        hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO_BOTH_R_CHANNEL);
        bt_sink_srv_memcpy(&(dev->codec), start_ind->codec_cap, sizeof(bt_a2dp_codec_capability_t));
        bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_START_IND, NULL);
    } else {
        bt_sink_srv_report("[sink][aws_a2dp]strat_ind(error)-hd: 0x%x", start_ind->handle);
    }

    return ret;
}


static int32_t bt_sink_srv_aws_a2dp_handle_suspend_streaming_cnf(bt_sink_srv_music_aws_suspend_streaming_cnf_t *suspend_cnf)
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *sp_dev = NULL;
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;

    bt_sink_srv_report("[sink][aws_a2dp]suspend_cnf--hd: 0x%x, accept: :%d", suspend_cnf->handle, suspend_cnf->accept);

    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_A2DP_HD, (void *)(&(suspend_cnf->handle)));
    sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);

    if ((aws_a2dp_dev) &&
        (sp_dev) &&
        (sp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_SUSPEND)) {
        BT_SINK_SRV_REMOVE_FLAG(sp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_SUSPEND);
        BT_SINK_SRV_REMOVE_FLAG(sp_dev->op, BT_SINK_SRV_MUSIC_OP_AWS_START);
    } else {
        bt_sink_srv_report("[sink][aws_a2dp]suspend_cnf(error)--sp_dev: 0x%x, flag: 0x:%x, op: 0x%x", sp_dev, sp_dev->flag, sp_dev->op);
    }

    return ret;
}


static int32_t bt_sink_srv_aws_a2dp_handle_suspend_streaming_ind(bt_sink_srv_music_aws_suspend_streaming_ind_t *suspend_ind)
{
    bt_sink_srv_music_device_t *a2dp_aws_dev = NULL;
    bt_sink_srv_music_context_t *ctx = NULL;
    int32_t ret = 0;

    ctx = bt_sink_srv_music_get_context();

    a2dp_aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_A2DP_HD, (void *)(&(suspend_ind->handle)));

    bt_sink_srv_report("[sink][aws_a2dp]suspend_ind-hd: 0x%x, flag: 0x%x, op: 0x%x", suspend_ind->handle, a2dp_aws_dev->flag, a2dp_aws_dev->op);

    if ((a2dp_aws_dev) &&
        (ctx->run_dev) &&
        (a2dp_aws_dev == ctx->run_dev)) {
        /* Accept suspend */
        bt_sink_srv_aws_a2dp_suspend_response(a2dp_aws_dev->aws_hd, a2dp_aws_dev, 1);
        /* Clear recover flag */
        BT_SINK_SRV_REMOVE_FLAG(a2dp_aws_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);
        /* SP suspend */
        bt_sink_srv_music_state_machine_handle(a2dp_aws_dev, BT_SINK_SRV_MUSIC_EVT_SUSPEND_IND, NULL);
    } else {
        bt_sink_srv_report("[sink][aws_a2dp]suspend_ind(error)--");
    }

    return ret;
}


static int32_t bt_sink_srv_aws_a2dp_handle_data_received_ind(bt_aws_streaming_received_ind_t *rece_ind)
{
    bt_a2dp_streaming_received_ind_t *data_ind = NULL, aws_data = {0};
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_music_device_t *run_dev = NULL;
    bt_sink_srv_media_mgr_context_t *med_ctx = NULL;
    int32_t ret = 0;
    static uint32_t s_aws_a2dp_error_log_loop_cnt = BT_SINK_SRV_A2DP_DATA_ERROR_LOOP_COUNT;
    bt_hci_packet_t *pkt = NULL;
    uint32_t time_dur = 0, duration = 0;
    uint32_t chk_ret = 0;
    sink_media_packet_info_t *last_pkt_info;
    sink_media_packet_info_t pre_packet;
    bt_hci_packet_t *next_packet;
    void *fill_pkt = NULL;


    /* Construct a2dp data ind */
    data_ind = &aws_data;
    data_ind->handle = rece_ind->handle;
    data_ind->data_node = rece_ind->packet.a2dp_packet.data_node;
    data_ind->total_length = rece_ind->packet.a2dp_packet.total_length;
    data_ind->media_offset = rece_ind->packet.a2dp_packet.media_offset;
    ctx = bt_sink_srv_music_get_context();
    med_ctx = bt_sink_srv_media_mgr_get_context();
    run_dev = ctx->run_dev;

    if ((run_dev) &&
        (run_dev->aws_hd == data_ind->handle)) {

        next_packet = (bt_hci_packet_t *)data_ind->data_node;
        /* Debug info */
        g_a2dp_srv_rece_pkt_byte += (next_packet->packet_length - next_packet->offset);

        last_pkt_info = &(ctx->med_pkt_info);
        chk_ret = sink_srv_a2dp_check_media_continuous(run_dev->med_handle.med_hd, &(ctx->med_play_info), last_pkt_info, next_packet);
        bt_sink_srv_memcpy(&pre_packet, last_pkt_info, sizeof(sink_media_packet_info_t));
        //chk_ret = SINK_SRV_PLH_TYPE_CONTINUOUS;

        /* Update last packet information */
        last_pkt_info->seq_num = sink_srv_util_get_a2dp_sn((bt_hci_packet_t *)(data_ind->data_node));
        last_pkt_info->time_stamp = sink_srv_util_get_a2dp_time_stamp((bt_hci_packet_t *)(data_ind->data_node));
        last_pkt_info->sample_counts = sink_srv_util_get_packet_sample_counts(run_dev->med_handle.med_hd, (bt_hci_packet_t *)(data_ind->data_node));

        if (SINK_SRV_PLH_TYPE_DROP == chk_ret) {
            //bt_sink_srv_assert(0 && "media_packet drop");
            return ret;
        } else if (SINK_SRV_PLH_TYPE_LACK == chk_ret) {
            /* Fill silence data */
            //bt_sink_srv_assert(0 && "media_packet lack");
            bt_a2dp_codec_type_t codec_type = run_dev->codec.type;
            fill_pkt = sink_srv_a2dp_fill_silence_frame(run_dev->med_handle.med_hd, &(ctx->med_play_info), &pre_packet, next_packet, codec_type);
            if (fill_pkt) {
                ret = bt_sink_srv_media_mgr_push_item((uint32_t)fill_pkt);
                bt_a2dp_release_media_data_node(fill_pkt);
            } else {
                bt_sink_srv_report("[sink][aws_a2dp]rece_ind--fill_pkt error");
            }
        } else if (SINK_SRV_PLH_TYPE_CONTINUOUS == chk_ret) {
            /* Normal */
            ;
        }

        ret = bt_sink_srv_media_mgr_push_item((uint32_t)(data_ind->data_node));

        if (ret < 0) {
            if (s_aws_a2dp_error_log_loop_cnt == BT_SINK_SRV_A2DP_DATA_ERROR_LOOP_COUNT) {
                bt_sink_srv_report("[sink][aws_a2dp]rece_ind(error_push)--ret: %d, item: %d, size: %d", ret, med_ctx->item_count, med_ctx->size);
                s_aws_a2dp_error_log_loop_cnt = 0;
            }
            ++s_aws_a2dp_error_log_loop_cnt;
            return ret;
        }

        /* Update last packet information */
        last_pkt_info->seq_num = sink_srv_util_get_a2dp_sn((bt_hci_packet_t *)(data_ind->data_node));
        last_pkt_info->time_stamp = sink_srv_util_get_a2dp_time_stamp((bt_hci_packet_t *)(data_ind->data_node));
        last_pkt_info->sample_counts = sink_srv_util_get_packet_sample_counts(run_dev->med_handle.med_hd, (bt_hci_packet_t *)(data_ind->data_node));

        if ((run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_PSEDEV_PLAYING)) {
            BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_PSEDEV_PLAYING);
            if ((run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_GPT_TIMEOUT)) {
                BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_AWS_A2DP_PLAYING);
                BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_GPT_TIMEOUT);
            } else {
                BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_AWS_A2DP_PLAYING);
                bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_PLAYING, NULL);
                bt_sink_srv_state_set(BT_SINK_SRV_STATE_STREAMING);
            }
        }

        if (ret < 0) {
            if (s_aws_a2dp_error_log_loop_cnt == BT_SINK_SRV_A2DP_DATA_ERROR_LOOP_COUNT) {
                bt_sink_srv_report("[sink][aws_a2dp]rece_ind(error_push)-ret: %d, item: %d, size: %d", ret, med_ctx->item_count, med_ctx->size);
                s_aws_a2dp_error_log_loop_cnt = 0;
            }
            ++s_aws_a2dp_error_log_loop_cnt;
            return ret;
        }

        if (run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER) {
            BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);
            BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_AWS_A2DP_PLAY);
            BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_PLAY);
            /* Async DRV play */
            sink_srv_util_send_event(SINK_SRV_EVT_CODEC_PLAY, run_dev);

            BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
            BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_GPT_TIMEOUT);
        }

        if ((run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER)) {
            pkt = (bt_hci_packet_t *)bt_sink_srv_music_find_suitable_packet(run_dev, &duration, 1);
            if (pkt) {
                time_dur = duration;
                /* Update base bt clock */
                //bt_sink_srv_music_update_aws_sync_info(run_dev, pkt, 1);
                //ctx->piconet_info.clock.nclk_intra += time_dur;
                ctx->play_time.nclk = ctx->piconet_info.clock.nclk;
                ctx->play_time.nclk_intra = ctx->piconet_info.clock.nclk_intra + time_dur;
                ctx->gpt_port = (hal_gpt_port_t)bt_sink_srv_music_start_gpt_timer(bt_sink_srv_aws_a2dp_gpt_cb, (void *)run_dev, time_dur);
                BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
            } else {
                /* Wait next packet incoming */
            }
        }

        if (!(run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_A2DP_PLAY)) {
            /* Calc simple count */
            if (ctx->simple_count >= ctx->threshold_count) {
                BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_AWS_A2DP_PLAY);
                BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_PLAY);
                /* Async DRV play */
                sink_srv_util_send_event(SINK_SRV_EVT_CODEC_PLAY, run_dev);
            } else {
                ctx->simple_count += sink_srv_util_get_packet_sample_counts(run_dev->med_handle.med_hd, (bt_hci_packet_t *)(data_ind->data_node));
            }
        }
    } else {
        /* Drop media data */
        bt_sink_srv_report("[sink][aws_a2dp]rece_ind(error_drop)-run_dev: 0x%08x, state: %d, c_hd: 0x%08x, d_hd: 0x%08x",
            run_dev, run_dev->state, run_dev->aws_hd, data_ind->handle);
    }

    return ret;
}


static int32_t bt_sink_srv_aws_a2dp_handle_play_ind(bt_aws_notify_play_ind_t *play_ind)
{
    int32_t ret = 0;
    uint32_t time_dur = 0, duration = 0;
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_hci_packet_t *pkt = NULL;
    bt_sink_srv_music_aws_sync_info *sync_info = NULL;

    ctx = bt_sink_srv_music_get_context();
    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_A2DP_HD, (void *)(&(play_ind->handle)));
    bt_sink_srv_assert(aws_a2dp_dev);
    time_dur = play_ind->play_notify->play_time;
    sync_info = (bt_sink_srv_music_aws_sync_info *)(play_ind->play_notify->user_data);
    bt_sink_srv_memcpy(&(ctx->piconet_info), sync_info, sizeof(bt_sink_srv_music_aws_sync_info));

    /* Add ts diff */
    ctx->med_play_info.ts_diff_by_packet = sync_info->ts_diff_by_packet;

    bt_sink_srv_report("[sink][aws_a2dp]play_ind(s)--sn: 0x%x, ts: 0x%x, nclk: 0x%x, intra: 0x%x, role: %d, dur: 0x%x, ts_diff: 0x%x",
            sync_info->pkt_info.sn, sync_info->pkt_info.ts, sync_info->clock.nclk,
            sync_info->clock.nclk_intra, play_ind->play_notify->bt_local_role, time_dur, ctx->med_play_info.ts_diff_by_packet);

    if ((time_dur == 0xffffffff) || !bt_sink_srv_music_duration_valid(BT_SINK_SRV_AWS_MODULE_ID_A2DP, (time_dur + BT_SINK_SRV_MUSIC_DUR_MIN))) {
        pkt = (bt_hci_packet_t *)bt_sink_srv_music_find_suitable_packet(aws_a2dp_dev, &duration, 1);
        if (pkt) {
            time_dur = duration;
            /* Update base bt clock */
            //bt_sink_srv_music_update_aws_sync_info(aws_a2dp_dev, pkt, 1);
            //ctx->piconet_info.clock.nclk_intra += time_dur;
            ctx->play_time.nclk = ctx->piconet_info.clock.nclk;
            ctx->play_time.nclk_intra = ctx->piconet_info.clock.nclk_intra + time_dur;
            ctx->gpt_port = (hal_gpt_port_t)bt_sink_srv_music_start_gpt_timer(bt_sink_srv_aws_a2dp_gpt_cb, (void *)aws_a2dp_dev, time_dur);
        } else {
            /* Wait new packe incoming & find suitable packet */
            BT_SINK_SRV_SET_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
        }
    } else {
        ctx->play_time.nclk = ctx->piconet_info.clock.nclk;
        ctx->play_time.nclk_intra = ctx->piconet_info.clock.nclk_intra;
        ctx->gpt_port = (hal_gpt_port_t)bt_sink_srv_music_start_gpt_timer(bt_sink_srv_aws_a2dp_gpt_cb, (void *)aws_a2dp_dev, time_dur);
    }

    bt_sink_srv_report("[sink][aws_a2dp]play_ind[e]-gpt_port: %d", ctx->gpt_port);

    return ret;
}


static bt_status_t bt_sink_srv_aws_a2dp_handle_command_ind(bt_aws_command_result_t *aws_cmd)
{
    int32_t ret = BT_STATUS_SUCCESS;

    bt_sink_srv_report("[sink][aws_a2dp]command_ind[s]-type: %d", aws_cmd->type);

    switch (aws_cmd->type) {
        case BT_SINK_SRV_AWS_COMMAND_TYPE_REQ: {
            bt_sink_srv_aws_a2dp_req_handle(aws_cmd->handle, &aws_cmd->cmd_ind);
            break;
        }

        case BT_SINK_SRV_AWS_COMMAND_TYPE_RSP: {
            bt_sink_srv_aws_a2dp_rsp_handle(aws_cmd->handle, &aws_cmd->cmd_rsp);
            break;
        }
        default:
            break;
    }

    return ret;
}


static bt_status_t bt_sink_srv_aws_a2dp_handle_offset_ind(bt_aws_offset_update_ind_t *offset_ind)
{
    int32_t ret = BT_STATUS_SUCCESS;
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_music_callback_t callback = NULL;

    ctx = bt_sink_srv_music_get_context();

    bt_sink_srv_report("[sink][aws_a2dp]offset_ind-offset: 0x%x, intra: 0x%x",
        offset_ind->offset.offset, offset_ind->offset.offset_intra);

    /* Update BT clock offset */
    ctx->clk_offset.offset = offset_ind->offset.offset;
    ctx->clk_offset.offset_intra = offset_ind->offset.offset_intra;

    if (ctx->callback) {
        callback = ctx->callback;
        ctx->callback = NULL;
        callback(0, (void *)offset_ind, ctx->user_data);
    }

    return ret;
}

static int32_t bt_sink_srv_aws_a2dp_handle_sync_base_ind(uint32_t handle, bt_sink_srv_music_aws_sync_info *sync_ind)
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *dev = NULL;  
    bt_sink_srv_music_context_t *ctx = NULL;
    
    ctx = bt_sink_srv_music_get_context();

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_A2DP_HD, (void *)(&handle));

    bt_sink_srv_report("[sink][aws_a2dp]base_sync_ind-hd: 0x%x, flag: 0x%x, op: 0x%x", handle, dev->flag, dev->op);

    if (dev) {
        bt_sink_srv_memcpy(&(ctx->piconet_info), sync_ind, sizeof(bt_sink_srv_music_aws_sync_info));
        /* Add ts diff */
        ctx->med_play_info.ts_diff_by_packet = sync_ind->ts_diff_by_packet;
    } else {
        bt_sink_srv_report("[sink][aws_a2dp]base_sync_ind(error)-hd: 0x%x\r\n", handle);
    }

    bt_sink_srv_report("[sink][aws_a2dp]base_sync_ind(s)--sn: 0x%x, ts: 0x%x, nclk: 0x%x, intra: 0x%x, ts_diff: 0x%x\r\n",
            sync_ind->pkt_info.sn, sync_ind->pkt_info.ts, sync_ind->clock.nclk,
            sync_ind->clock.nclk_intra, ctx->med_play_info.ts_diff_by_packet);

    return ret;
}

static bt_status_t bt_sink_srv_aws_a2dp_req_handle(uint32_t handle, bt_sink_srv_aws_cmd_ind_t *cmd_ind)
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
    bt_sink_srv_cmd_id_t cmd_id = cmd_ind->cmd_id;
    bt_sink_srv_music_aws_start_streaming_ind_t start_ind = {0};
    bt_sink_srv_music_aws_suspend_streaming_ind_t suspend_ind = {0};
    uint8_t vol_lev = 0;

    bt_sink_srv_report("[sink][aws_a2dp]req_handle[s]-cmd_id: %d", cmd_id);

    switch (cmd_id) {
        case BT_SINK_SRV_AWS_COMMAND_ID_START: {
            start_ind.handle = handle;
            start_ind.codec_cap = (void *)cmd_ind->data;
            bt_sink_srv_aws_a2dp_handle_start_streaming_ind(&start_ind);
            break;
        }

        case BT_SINK_SRV_AWS_COMMAND_ID_STOP: {
            suspend_ind.handle = handle;
            bt_sink_srv_aws_a2dp_handle_suspend_streaming_ind(&suspend_ind);
            break;
        }

        case BT_SINK_SRV_AWS_COMMAND_ID_SET_VOLUME: {
            vol_lev = cmd_ind->data[0];
            bt_sink_srv_aws_a2dp_handle_volume_change(vol_lev);
            break;
        }

        case BT_SINK_SRV_AWS_COMMAND_ID_NEXT: {
            a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
            if (a2dp_dev) {
                bt_sink_srv_avrcp_change_ntrack(a2dp_dev->avrcp_hd);
            }
            break;
        }

        case BT_SINK_SRV_AWS_COMMAND_ID_PREV: {
            a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
            if (a2dp_dev) {
                bt_sink_srv_avrcp_change_ptrack(a2dp_dev->avrcp_hd);
            }
            break;
        }

        case BT_SINK_SRV_AWS_COMMAND_ID_SINK_PLAY: {
            a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
            if (a2dp_dev) {
                bt_sink_srv_avrcp_play_music(a2dp_dev->avrcp_hd);
            }
            break;
        }

        case BT_SINK_SRV_AWS_COMMAND_ID_SINK_PAUSE: {
            a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
            if (a2dp_dev) {
                bt_sink_srv_avrcp_stop_music(a2dp_dev->avrcp_hd);
            }
            break;
        }
        case BT_SINK_SRV_AWS_COMMAND_ID_SYNC_BASE: {
            bt_sink_srv_music_aws_sync_info *base_sync_ind = NULL;
            base_sync_ind = (bt_sink_srv_music_aws_sync_info *)cmd_ind->data;
            bt_sink_srv_aws_a2dp_handle_sync_base_ind(handle, base_sync_ind);
            break;
        }
        default:
            break;
    }

    return ret;
}

static bt_status_t bt_sink_srv_aws_a2dp_rsp_handle(uint32_t handle, bt_sink_srv_aws_cmd_rsp_t *cmd_rsp)
{
    int32_t ret = 0;
    bt_sink_srv_cmd_id_t id = cmd_rsp->cmd_id;
    bt_sink_srv_music_aws_start_streaming_cnf_t start_cnf = {0};
    bt_sink_srv_music_aws_suspend_streaming_cnf_t suspend_cnf = {0};

    bt_sink_srv_report("[sink][aws_a2dp]rsp_handle[s]-id: %d", id);

    switch (id) {
        case BT_SINK_SRV_AWS_COMMAND_ID_START: {
            start_cnf.handle = handle;
            start_cnf.accept = cmd_rsp->accept;
            bt_sink_srv_aws_a2dp_handle_start_streaming_cnf(&start_cnf);
            break;
        }

        case BT_SINK_SRV_AWS_COMMAND_ID_STOP: {
            suspend_cnf.handle = handle;
            suspend_cnf.accept = cmd_rsp->accept;
            bt_sink_srv_aws_a2dp_handle_suspend_streaming_cnf(&suspend_cnf);
            break;
        }

        default:
            break;
    }
    return ret;
}


static int32_t bt_sink_srv_aws_a2dp_start_response(uint32_t handle, bt_sink_srv_music_device_t *dev, uint8_t accept)
{
    int32_t ret = BT_STATUS_SUCCESS;
    bt_sink_srv_aws_cmd_t rsp = {0};

    rsp.cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_START;
    rsp.type = BT_SINK_SRV_AWS_COMMAND_TYPE_RSP;
    rsp.accept = accept;
    rsp.data_len = 1;
    ret = bt_sink_srv_aws_send_cmd(handle, BT_SINK_SRV_AWS_MODULE_ID_A2DP, &rsp);
    bt_sink_srv_report("[sink][aws_a2dp]start rsp-ret: %d", ret);

    return ret;
}


static int32_t bt_sink_srv_aws_a2dp_suspend_response(uint32_t handle, bt_sink_srv_music_device_t *dev, uint8_t accept)
{
    int32_t ret = BT_STATUS_SUCCESS;
    bt_sink_srv_aws_cmd_t rsp = {0};

    rsp.cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_STOP;
    rsp.type = BT_SINK_SRV_AWS_COMMAND_TYPE_RSP;
    rsp.accept = accept;
    rsp.data_len = 1;
    rsp.data[0] = 0xfb;
    ret = bt_sink_srv_aws_send_cmd(handle, BT_SINK_SRV_AWS_MODULE_ID_A2DP, &rsp);
    bt_sink_srv_report("[sink][aws_a2dp]suspend rsp-ret: %d", ret);

    return ret;
}



//static int32_t bt_sink_srv_aws_a2dp_set_volume(uint8_t vol_lev)
//{
//    bt_sink_srv_music_context_t *ctx = NULL;
//    int32_t ret = 0;
//
//    ctx = bt_sink_srv_music_get_context();
//    ctx->vol_lev = vol_lev;
//    ret = bt_sink_srv_ami_audio_set_volume(ctx->aws_aid, vol_lev, STREAM_OUT);
//    bt_sink_srv_report("[sink][aws_a2dp]set_volume-ret: %d, vol: %d", ret, vol_lev);
//
//   return ret;
//}
//


static void bt_sink_srv_aws_a2dp_handle_volume_change(uint8_t vol_lev)
{
    bt_sink_srv_music_device_t *sp_dev = NULL;
    bt_sink_srv_music_context_t *ctx = NULL;
    int32_t ret = 0;
    uint8_t dev_db = 0;
    bt_sink_srv_am_id_t ami_ret = 0;

    ctx = bt_sink_srv_music_get_context();
    sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
    if (sp_dev) {
        /* SPK1 volume setting */
        ami_ret = bt_sink_srv_ami_audio_set_volume(ctx->a2dp_aid, vol_lev, STREAM_OUT);
        dev_db = (BT_SINK_SRV_A2DP_MAGIC_CODE | ctx->vol_lev);
        bt_sink_srv_cm_set_profile_data(&(sp_dev->dev_addr), BT_SINK_SRV_PROFILE_A2DP_SINK, &dev_db);

        bt_sink_srv_avrcp_volume_notification(sp_dev->avrcp_hd, vol_lev);
    } else {
        /* SP2 volume setting */
        ami_ret = bt_sink_srv_ami_audio_set_volume(ctx->aws_aid, vol_lev, STREAM_OUT);
    }
    ctx->vol_lev = vol_lev;

    bt_sink_srv_report("[sink][aws_a2dp]volume_change-ami_ret: %d, ret: %d, vol: %d",
        ami_ret, ret, ctx->vol_lev);
}


int32_t bt_sink_srv_aws_a2dp_change_volume(uint8_t type)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_FAIL_7TH;
    bt_sink_srv_am_id_t ami_ret = 0;
    uint8_t vol_lev = AUD_VOL_OUT_LEVEL0;
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;

    ctx = bt_sink_srv_music_get_context();
    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);

    if (aws_a2dp_dev) {
        vol_lev = ctx->vol_lev;
        /* Volume up */
        if (VOLUME_UP == type) {
           if (vol_lev < BT_SINK_SRV_A2DP_MAX_VOL_LEV) {
               vol_lev = vol_lev + 1;
           } else {
               ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
           }
        } else if (VOLUME_DOWN == type) {
            if (vol_lev > BT_SINK_SRV_A2DP_MIN_VOL_LEV) {
                vol_lev = vol_lev - 1;
            } else {
                ret = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
            }
        }

        if (vol_lev != ctx->vol_lev) {
            ami_ret = bt_sink_srv_ami_audio_set_volume(ctx->aws_aid, vol_lev, STREAM_OUT);
            ctx->vol_lev = vol_lev;
            ret = bt_sink_srv_aws_a2dp_sync_vol_level(aws_a2dp_dev->aws_hd, vol_lev);
        }
    }
    bt_sink_srv_report("[sink][aws_a2dp]change_volume-ami_ret: %d, ret: %d, vol: %d",
        ami_ret, ret, ctx->vol_lev);

    return ret;
}


static void bt_sink_srv_aws_a2dp_vol_up(void)
{
    bt_sink_srv_aws_a2dp_change_volume(VOLUME_UP);
}


static void bt_sink_srv_aws_a2dp_vol_down(void)
{
    bt_sink_srv_aws_a2dp_change_volume(VOLUME_DOWN);
}


static void bt_sink_srv_aws_a2dp_play_music(void)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_FAIL_7TH;
    uint8_t buf[32] = {0};
    bt_sink_srv_aws_cmd_t *req = NULL;
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;

    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);

    if (aws_a2dp_dev) {
        req = (bt_sink_srv_aws_cmd_t *)buf;
        req->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_SINK_PLAY;
        req->type = BT_SINK_SRV_AWS_COMMAND_TYPE_REQ;
        req->data_len = 1;
        req->data[0] = 0xca;
        ret = bt_sink_srv_aws_send_cmd(aws_a2dp_dev->aws_hd, BT_SINK_SRV_AWS_MODULE_ID_A2DP, req);
    }
    bt_sink_srv_report("[sink][aws_a2dp]play music--hd: 0x%x, ret: %d\n", aws_a2dp_dev->aws_hd, ret);

}


static void bt_sink_srv_aws_a2dp_pause_music(void)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_FAIL_7TH;
    uint8_t buf[32] = {0};
    bt_sink_srv_aws_cmd_t *req = NULL;
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;

    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);

    if (aws_a2dp_dev) {
        req = (bt_sink_srv_aws_cmd_t *)buf;
        req->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_SINK_PAUSE;
        req->type = BT_SINK_SRV_AWS_COMMAND_TYPE_REQ;
        req->data_len = 1;
        req->data[0] = 0xca;
        ret = bt_sink_srv_aws_send_cmd(aws_a2dp_dev->aws_hd, BT_SINK_SRV_AWS_MODULE_ID_A2DP, req);
    }
    bt_sink_srv_report("[sink][aws_a2dp]pause music--hd: 0x%x, ret: %d", aws_a2dp_dev->aws_hd, ret);

}


static void bt_sink_srv_aws_a2dp_next_track(void)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_FAIL_7TH;
    uint8_t buf[32] = {0};
    bt_sink_srv_aws_cmd_t *req = NULL;
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;

    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);

    if (aws_a2dp_dev) {
        req = (bt_sink_srv_aws_cmd_t *)buf;
        req->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_NEXT;
        req->type = BT_SINK_SRV_AWS_COMMAND_TYPE_REQ;
        req->data_len = 1;
        req->data[0] = 0xfa;
        ret = bt_sink_srv_aws_send_cmd(aws_a2dp_dev->aws_hd, BT_SINK_SRV_AWS_MODULE_ID_A2DP, req);
    }
    bt_sink_srv_report("[sink][aws_a2dp]next track--hd: 0x%x, ret: %d", aws_a2dp_dev->aws_hd, ret);
}


static void bt_sink_srv_aws_a2dp_pre_track(void)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_FAIL_7TH;
    uint8_t buf[32] = {0};
    bt_sink_srv_aws_cmd_t *req = NULL;
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;

    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);

    if (aws_a2dp_dev) {
        req = (bt_sink_srv_aws_cmd_t *)buf;
        req->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_PREV;
        req->type = BT_SINK_SRV_AWS_COMMAND_TYPE_REQ;
        req->data_len = 1;
        req->data[0] = 0xfc;
        ret = bt_sink_srv_aws_send_cmd(aws_a2dp_dev->aws_hd, BT_SINK_SRV_AWS_MODULE_ID_A2DP, req);
    }
    bt_sink_srv_report("[sink][aws_a2dp]pre track--hd: 0x%x, ret: %d", aws_a2dp_dev->aws_hd, ret);
}


bt_sink_srv_status_t bt_sink_srv_aws_a2dp_action_handler(bt_sink_srv_action_t action, void *param)
{
    bt_status_t ret = 0;
    int32_t err = 0;
    bt_sink_srv_music_device_t *a2dp_dev = NULL;


    bt_sink_srv_report("[sink][aws_a2dp]action[s]-action: 0x%x, play: 0x%x", action, BT_SINK_SRV_ACTION_PLAY);

    a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_SP, NULL);

    if (a2dp_dev) {
        /* SP connection, ingore it */
        return BT_SINK_SRV_STATUS_SUCCESS;
    }

    switch (action) {
        case BT_SINK_SRV_ACTION_PLAY: {
            bt_sink_srv_aws_a2dp_play_music();
            break;
        }

        case BT_SINK_SRV_ACTION_PAUSE: {
            bt_sink_srv_aws_a2dp_pause_music();
            break;
        }

        case BT_SINK_SRV_ACTION_NEXT_TRACK: {
            bt_sink_srv_aws_a2dp_next_track();
            break;
        }

        case BT_SINK_SRV_ACTION_PRE_TRACK: {
            bt_sink_srv_aws_a2dp_pre_track();
            break;
        }

        case BT_SINK_SRV_ACTION_VOLUME_UP: {
            bt_sink_srv_aws_a2dp_vol_up();
            break;
        }

        case BT_SINK_SRV_ACTION_VOLUME_DOWN: {
            bt_sink_srv_aws_a2dp_vol_down();
            break;
        }

        default:
            break;
    }

    bt_sink_srv_report("[sink][aws_a2dp]action[e]-ret: %d, err: %d", ret, err);

    return BT_SINK_SRV_STATUS_SUCCESS;
}


bt_status_t bt_sink_srv_aws_a2dp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    int32_t ret = 0;
    int32_t gpt_ret = 0;
    uint32_t time_dur = 0;
    static uint32_t s_aws_a2dp_data_log_loop_cnt = BT_SINK_SRV_A2DP_DATA_RCE_LOOP_COUNT;

    if ((BT_SINK_SRV_A2DP_DATA_RCE_LOOP_COUNT == s_aws_a2dp_data_log_loop_cnt) || 
        (msg != BT_AWS_STREAMING_PACKET_RECEIVED_IND)) {
        bt_sink_srv_report("[sink][aws_a2dp]common_hdr[s]-msg: 0x%x, status: %d", msg, status);
        if (BT_SINK_SRV_A2DP_DATA_RCE_LOOP_COUNT == s_aws_a2dp_data_log_loop_cnt) {
            s_aws_a2dp_data_log_loop_cnt = 0;
            gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &g_a2dp_gpt_run_count_end);
            time_dur = (g_a2dp_gpt_run_count_end - g_a2dp_gpt_run_count_begin) * 1000 / 32768;
            bt_sink_srv_report("[sink][a2dp]common_hdr(AWS info)--time_dur: %d, rece: %d, read: %d, gpt_ret: %d",
                time_dur, g_a2dp_srv_rece_pkt_byte, g_a2dp_decoder_read_pkt_byte, gpt_ret);
            /* Update info */
            g_a2dp_gpt_run_count_begin = g_a2dp_gpt_run_count_end;
            g_a2dp_srv_rece_pkt_byte = 0;
            g_a2dp_decoder_read_pkt_byte = 0;
        }
    }

    switch (msg) {
        case BT_AWS_CONNECT_CNF: {
            bt_aws_connect_cnf_t *conn_cnf = (bt_aws_connect_cnf_t *)buffer;

            ret = bt_sink_srv_aws_a2dp_handle_connect_cnf(conn_cnf);
            break;
        }

        case BT_AWS_DISCONNECT_CNF: {
            bt_aws_disconnect_cnf_t *disconn_cnf = (bt_aws_disconnect_cnf_t *)buffer;

            ret = bt_sink_srv_aws_a2dp_handle_disconnect_cnf(disconn_cnf);
            break;
        }

        case BT_AWS_DISCONNECT_IND: {
            bt_aws_disconnect_ind_t *disconn_ind = (bt_aws_disconnect_ind_t *)buffer;

            ret = bt_sink_srv_aws_a2dp_handle_disconnect_ind(disconn_ind);
            break;
        }

        case BT_AWS_NOTIFY_PLAY_IND: {
            bt_aws_notify_play_ind_t *play_ind = (bt_aws_notify_play_ind_t *)buffer;
            ret = bt_sink_srv_aws_a2dp_handle_play_ind(play_ind);
            break;
        }

        case BT_AWS_STREAMING_PACKET_RECEIVED_IND: {
            bt_aws_streaming_received_ind_t *rece_ind = (bt_aws_streaming_received_ind_t *)buffer;
            ++s_aws_a2dp_data_log_loop_cnt;
            bt_sink_srv_media_mgr_garbage_free();
            ret = bt_sink_srv_aws_a2dp_handle_data_received_ind(rece_ind);

            break;
        }

        case BT_AWS_COMMAND_IND: {
            bt_aws_command_result_t *aws_cmd = (bt_aws_command_result_t *)buffer;
            ret = bt_sink_srv_aws_a2dp_handle_command_ind(aws_cmd);
            break;
        }

        case BT_AWS_OFFSET_UPDATE_IND: {
            bt_aws_offset_update_ind_t *offset_ind = (bt_aws_offset_update_ind_t *)buffer;
            ret = bt_sink_srv_aws_a2dp_handle_offset_ind(offset_ind);
            break;
        }

        case BT_MEMORY_FREE_GARBAGE_IND: {
            bt_sink_srv_media_mgr_garbage_free();
            break;
        }

        default:
            break;
    }

    if (msg != BT_AWS_STREAMING_PACKET_RECEIVED_IND) {
        bt_sink_srv_report("[sink][aws_a2dp]common_hdr[e]-ret: %d", ret);
        if (BT_SINK_SRV_A2DP_DATA_RCE_LOOP_COUNT == s_aws_a2dp_data_log_loop_cnt) {
            s_aws_a2dp_data_log_loop_cnt = 0;
        }
    }

    return ret;
}


/* Transfer volume setting */
int32_t bt_sink_srv_aws_a2dp_sync_vol_level(uint32_t handle, uint8_t vol_lev)
{
    int32_t ret = 0;
    uint8_t buf[32] = {0};
    bt_sink_srv_aws_cmd_t *req = NULL;

    req = (bt_sink_srv_aws_cmd_t *)buf;
    req->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_SET_VOLUME;
    req->type = BT_SINK_SRV_AWS_COMMAND_TYPE_REQ;
    req->data_len = 1;
    bt_sink_srv_memcpy(req->data, &vol_lev, req->data_len);
    ret = bt_sink_srv_aws_send_cmd(handle, BT_SINK_SRV_AWS_MODULE_ID_A2DP, req);

    bt_sink_srv_report("[sink][aws_a2dp]send volume--hd: 0x%08x, vol: %d, ret: %d", handle, vol_lev, ret);

    return ret;
}


void bt_sink_srv_aws_a2dp_drv_play(void *param)
{
    bt_sink_srv_music_device_t *aws_a2dp_dev = (bt_sink_srv_music_device_t *)param;
    bt_sink_srv_music_context_t *ctx = NULL;
    //uint8_t dev_db = 0;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    int32_t ret = 0;
    uint32_t gpt_run_count_begin = 0;
    uint32_t gpt_run_count_end = 0;
    int32_t gpt_ret = 0;
    uint32_t cost_dur = 0;

    ctx = bt_sink_srv_music_get_context();

    if (ctx->run_dev == aws_a2dp_dev) {
        #if 0
        /* Volume setting */
        if (bt_sink_srv_cm_get_profile_data(&(aws_a2dp_dev->dev_addr), BT_SINK_SRV_PROFILE_A2DP_SINK, &dev_db)) {
            if ((dev_db & 0xf0) == BT_SINK_SRV_AWS_A2DP_MAGIC_CODE) {
                /* use storge volume value */
                ctx->vol_lev = (dev_db & 0x0f);
            } else {
                /* use dedefault volume value and update it */
                ctx->vol_lev = BT_SINK_SRV_AWS_A2DP_VOL_DEF_LEV;
                dev_db = (BT_SINK_SRV_AWS_A2DP_MAGIC_CODE | ctx->vol_lev);
                //bt_sink_srv_cm_set_profile_data(&(aws_a2dp_dev->dev_addr), BT_SINK_SRV_PROFILE_AWS, &dev_db);
            }
        }
        #endif

        bt_sink_srv_ami_audio_set_volume(ctx->aws_aid, ctx->vol_lev, STREAM_OUT);

        BT_SINK_SRV_REMOVE_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_PLAY);

        if (aws_a2dp_dev && aws_a2dp_dev->gap_role == BT_ROLE_MASTER) {
            bt_codec_a2dp_aws_set_clock_skew(false);
        } else if (aws_a2dp_dev && aws_a2dp_dev->gap_role == BT_ROLE_SLAVE) {
            bt_codec_a2dp_aws_set_clock_skew(true);
        } else {
            bt_codec_a2dp_aws_set_clock_skew(false);
        }

        med_hd = &(aws_a2dp_dev->med_handle);
        med_hd->set_aws_flag(ctx->aws_aid, true);
        med_hd->aws_plh_init(ctx->aws_aid);
        gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_run_count_begin);
        ret = med_hd->play(ctx->aws_aid);
        gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_run_count_end);
        cost_dur = (gpt_run_count_end - gpt_run_count_begin) * 1000 / 32768;
        bt_sink_srv_report("[sink][aws_a2dp]drv_play--ret: %d, cost: %d, gpt_ret: %d", ret, cost_dur, gpt_ret);
        if (BT_CODEC_MEDIA_STATUS_OK == ret) {
            //BT_SINK_SRV_SET_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_AWS_A2DP_PLAY);
            BT_SINK_SRV_SET_FLAG(aws_a2dp_dev->op, BT_SINK_SRV_MUSIC_OP_DRV_PLAY);
        } else {
            /* Error handle */
            bt_sink_srv_report("[sink][aws_a2dp]drv_play(error)-ret: 0x%x", ret);
        }
    }
}


void bt_sink_srv_aws_a2dp_drv_stop(void *param)
{
    bt_sink_srv_music_device_t *aws_a2dp_dev = (bt_sink_srv_music_device_t *)param;
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    int32_t ret = 0;

    ctx = bt_sink_srv_music_get_context();

    if (ctx->run_dev == aws_a2dp_dev) {
        med_hd = &(aws_a2dp_dev->med_handle);
        med_hd->set_aws_flag(ctx->aws_aid, false);
        med_hd->aws_plh_init(ctx->aws_aid);
        BT_SINK_SRV_REMOVE_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP);
        ret = med_hd->stop(ctx->aws_aid);
        bt_sink_srv_report("[sink][aws_a2dp]drv_stop-ret: 0x%x", ret);
        if (BT_CODEC_MEDIA_STATUS_OK == ret) {
            /* Remove flag */
            BT_SINK_SRV_REMOVE_FLAG(aws_a2dp_dev->op, BT_SINK_SRV_MUSIC_OP_DRV_PLAY);
        } else {
            /* Error handle */
            bt_sink_srv_report("[sink][aws_a2dp]drv_stop(error)-ret: 0x%x", ret);
        }
        BT_SINK_SRV_REMOVE_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_AWS_A2DP_PLAY);
        BT_SINK_SRV_REMOVE_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_AWS_A2DP_PLAYING);
    }
}


void bt_sink_srv_aws_a2dp_play(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;
    bt_sink_srv_am_audio_capability_t aud_cap;
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_am_result_t am_ret;
    int32_t ret = 0;

    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_PSE_HD, (void *)handle);
    bt_sink_srv_assert(aws_a2dp_dev);

    bt_sink_srv_report("[sink][aws_a2dp]play(s)--hd: 0x%x, type: %d, flag: 0x%x, op: 0x%x",
        handle, handle->type, aws_a2dp_dev->flag, aws_a2dp_dev->op);

    /* Audio source accept play request */
    BT_SINK_SRV_SET_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC);
    /* Update run device */
    bt_sink_srv_music_update_run_device(aws_a2dp_dev);
    /* 1. Open A2DP codec */
    bt_sink_srv_aws_a2dp_fill_am_aud_param(&aud_cap, &aws_a2dp_dev->codec, BT_AWS_ROLE_SINK);

    BT_SINK_SRV_SET_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET);
    ret = bt_sink_srv_music_get_bt_clk_offset(aws_a2dp_dev->aws_hd, bt_sink_srv_aws_a2dp_get_bt_clk_offset, aws_a2dp_dev);
    if (ret != BT_STATUS_SUCCESS) {
        BT_SINK_SRV_REMOVE_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET);
        bt_sink_srv_report("[sink][aws_a2dp]play(error_get_bt_time)");
    }

    am_ret = bt_sink_srv_ami_audio_play(ctx->aws_aid, &aud_cap);
    if (am_ret != AUD_EXECUTION_SUCCESS) {
        /* Exception: play fail */
        BT_SINK_SRV_REMOVE_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC);
        bt_sink_srv_music_update_run_device(NULL);
        bt_sink_srv_music_state_machine_handle(aws_a2dp_dev, BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL, NULL);
        bt_sink_srv_report("[sink][aws_a2dp]aws_play(error)--am_ret: %d", am_ret);
    }
}


void bt_sink_srv_aws_a2dp_stop(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;
    bt_sink_srv_music_device_t *ori_dev = NULL;
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_PSE_HD, (void *)handle);

    bt_sink_srv_assert(aws_a2dp_dev);

    ori_dev = ctx->run_dev;

    bt_sink_srv_report("[sink][aws_a2dp]stop(s)--hd: 0x%x, type: %d, flag: 0x%x, op: 0x%x",
        handle, handle->type, aws_a2dp_dev->flag, aws_a2dp_dev->op);

    /* Clear codec */
    bt_sink_srv_aws_a2dp_clear_codec(aws_a2dp_dev);

    /* Clear done */
    bt_sink_srv_music_state_machine_handle(aws_a2dp_dev, BT_SINK_SRV_MUSIC_EVT_CODEC_CLEAR, NULL);

    if (!(aws_a2dp_dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT)) {        
        bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
        bt_sink_srv_music_state_machine_handle(aws_a2dp_dev, BT_SINK_SRV_MUSIC_EVT_UNAVAILABLE, NULL);
        aws_a2dp_dev->handle = NULL;
        if (!(aws_a2dp_dev->conn_bit)) {
            bt_sink_srv_music_reset_device(aws_a2dp_dev);
        }
    } else {
        bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
        bt_sink_srv_music_state_machine_handle(aws_a2dp_dev, BT_SINK_SRV_MUSIC_EVT_READY, NULL);
    }

    if (ori_dev == ctx->run_dev) {
        bt_sink_srv_music_update_run_device(NULL);
    }
}


void bt_sink_srv_aws_a2dp_suspend(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd)
{
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;
    int32_t ret = 0;

    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_PSE_HD, (void *)handle);

    bt_sink_srv_report("[sink][aws_a2dp]suspend(s)--hd: 0x%x, type: %d, int: 0x%x, type: %d, flag: 0x%x, op: 0x%x",
        handle, handle->type, int_hd, int_hd->type, aws_a2dp_dev->flag, aws_a2dp_dev->op);

    /* Clear codec */
    bt_sink_srv_aws_a2dp_clear_codec(aws_a2dp_dev);

    /* Clear done */
    bt_sink_srv_music_state_machine_handle(aws_a2dp_dev, BT_SINK_SRV_MUSIC_EVT_CODEC_CLEAR, NULL);

    if ((handle->dev_id == int_hd->dev_id) &&
        (int_hd->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_HFP)) {
        /* The same device & HF interrupt */
        ;
    } else if (int_hd->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_MP3) {
        /* Ignore, master handle it */
        ;
    } else {
        /* Add self in waiting list */
        audio_src_srv_add_waiting_list(handle);
        /* Set interrupt flag */
        BT_SINK_SRV_SET_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT);
        /* Send pause cmd */
        //ret = bt_sink_srv_aws_a2dp_stop_music(aws_a2dp_dev->aws_hd);
        if (ret != BT_STATUS_SUCCESS) {
            /* Pause SP failed */
            bt_sink_srv_report("[sink][aws_a2dp]suspend(error)--ret: %d", ret);
        }
    }
}


void bt_sink_srv_aws_a2dp_reject(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;

    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_PSE_HD, (void *)handle);

    bt_sink_srv_assert(aws_a2dp_dev);

    bt_sink_srv_report("[sink][aws_a2dp]reject(s)--hd: 0x%x, type: %d, flag: 0x%x, op: 0x%x",
        handle, handle->type, aws_a2dp_dev->flag, aws_a2dp_dev->op);

    /* Reject handle case 1. reject AWS SRC start streaming */
    //bt_a2dp_start_streaming_response(aws_a2dp_dev->aws_hd, true);

    /* Notify state machine reject reason */
    bt_sink_srv_music_state_machine_handle(aws_a2dp_dev, BT_SINK_SRV_MUSIC_EVT_REJECT, NULL);
}


void bt_sink_srv_aws_a2dp_exception(audio_src_srv_handle_t *handle, int32_t event, void *param)
{
}


void bt_sink_srv_aws_a2dp_create_pse_handle(void)
{
    g_aws_a2dp_pse_hd = audio_src_srv_construct_handle(AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
}


void bt_sink_srv_aws_a2dp_destroy_pse_handle(void)
{
    audio_src_srv_destruct_handle(g_aws_a2dp_pse_hd);
}


int32_t bt_sink_srv_aws_a2dp_start_request(uint32_t handle, bt_sink_srv_music_device_t *dev)
{
    int32_t ret = 0;
    uint8_t buf[256] = {0};
    bt_sink_srv_aws_cmd_t *req = NULL;

    req = (bt_sink_srv_aws_cmd_t *)buf;
    req->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_START;
    req->type = BT_SINK_SRV_AWS_COMMAND_TYPE_REQ;
    req->accept = false;
    req->data_len = sizeof(bt_a2dp_codec_capability_t);
    bt_sink_srv_memcpy(req->data, &(dev->codec), sizeof(bt_a2dp_codec_capability_t));
    ret = bt_sink_srv_aws_send_cmd(handle, BT_SINK_SRV_AWS_MODULE_ID_A2DP, req);
    bt_sink_srv_report("[sink][aws_a2dp]start req-ret: %d", ret);

    return ret;
}


static void bt_sink_srv_aws_a2dp_gpt_cb(void *user_data)
{
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_music_device_t *aws_a2dp_dev = (bt_sink_srv_music_device_t *)user_data;
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_FAIL_7TH, err = BT_SINK_SRV_MUSIC_ERR_FAIL_7TH;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    int64_t curr_clock;
    int64_t play_clock;
    bt_aws_clock_t cur_bt_clk = {0};
    bt_sink_srv_media_mgr_context_t *med_ctx = NULL;

    ctx = bt_sink_srv_music_get_context();
    med_ctx = bt_sink_srv_media_mgr_get_context();

    /* Reset gpt */
    hal_gpt_stop_timer(ctx->gpt_port);
    hal_gpt_deinit(ctx->gpt_port);
    ctx->gpt_port = HAL_GPT_MAX_PORT;

    play_clock = ctx->play_time.nclk * 312 + (ctx->play_time.nclk >> 1) + ctx->play_time.nclk_intra;

    if (ctx->run_dev == aws_a2dp_dev) {
        med_hd = &(aws_a2dp_dev->med_handle);
        if (!(aws_a2dp_dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY)) {
            /* GPT interrupt (re-entry) error handle */
            bt_sink_srv_report("[sink][aws_a2dp]gpt_cb(error)--flag: 0x%0x, op: 0x%08x, size: %d, count: %d, bt_ac: %d, garbage_ac: %d, decoder_ac: %d, skip: %d, odd_byte: %d",
                aws_a2dp_dev->flag, aws_a2dp_dev->op,
                med_ctx->size, med_ctx->item_count, med_ctx->bt_ac_index, med_ctx->garbage_ac_index, med_ctx->decoder_ac_index, med_ctx->skip, med_ctx->odd_byte);
            BT_SINK_SRV_SET_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
            return;
        }

        if (!med_hd->set_aws_initial_sync) {
            bt_sink_srv_report("[sink][aws_a2dp]gpt_cb(error)--empty func");
            return;
        }
        BT_SINK_SRV_SET_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_PSEDEV_PLAYING);
        __disable_irq();
        while(1)
        {
            bt_aws_get_curr_bt_time(aws_a2dp_dev->gap_role, ctx->clk_offset, &cur_bt_clk);

            curr_clock = cur_bt_clk.nclk * 312 + (cur_bt_clk.nclk >> 1) + cur_bt_clk.nclk_intra;
            if (curr_clock >= play_clock)
            {
                break;
            }
        }
        ret = med_hd->set_aws_initial_sync(ctx->aws_aid);
        __enable_irq();
        bt_sink_srv_audio_clock_init(&ctx->clk_offset);
        bt_sink_srv_report("[sink][aws_a2dp]gpt_cb-start play--ret: %d", ret);
    } else {
        err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

    bt_sink_srv_report("[sink][aws_a2dp]gpt_cb-ret: %d, err: %d", ret, err);
}


static void bt_sink_srv_aws_a2dp_get_bt_clk_offset(uint32_t evt_id,void * param,void * user_data)
{
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_aws_offset_update_ind_t *offset_ind = (bt_aws_offset_update_ind_t *)param;
    bt_sink_srv_music_device_t *aws_a2dp_dev = (bt_sink_srv_music_device_t *)user_data;
    int32_t ret = 0;
    bt_sink_srv_music_device_t *run_dev = NULL;

    ctx = bt_sink_srv_music_get_context();
    run_dev = ctx->run_dev;

    bt_sink_srv_report("[sink][aws_a2dp]clk_offset(s)-offset: 0x%x, intra: 0x%x",
        offset_ind->offset.offset, offset_ind->offset.offset_intra);

    if ((aws_a2dp_dev) && (aws_a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET)) {
        BT_SINK_SRV_REMOVE_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET);

        ctx->clk_offset.offset = offset_ind->offset.offset;
        ctx->clk_offset.offset_intra = offset_ind->offset.offset_intra;

        if (aws_a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START) {
            /* Accept start streaming */
            ret = bt_sink_srv_aws_a2dp_start_response(run_dev->aws_hd, run_dev, 1);
            if (BT_STATUS_SUCCESS == ret) {
                bt_sink_srv_a2dp_reset_last_pkt_info(&(ctx->med_pkt_info));
                sink_srv_init_media_play_info(&(ctx->med_play_info), &(run_dev->codec), run_dev->med_handle.med_hd);
                bt_sink_srv_media_mgr_set_codec_type(run_dev->codec.type, &(run_dev->codec));
                //ctx->threshold_count = sinv_srv_util_us_2_sample_count(BT_SINK_SRV_MUSIC_AWS_PREPARE_BUF_THRESHOLD, ctx->med_play_info.sample_rate);
                ctx->simple_count = 0;
                ctx->threshold_count = 0;
                bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_CODEC_OPEN, NULL);
            } else {
                /* Error handle */
                bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL, NULL);
            }
            BT_SINK_SRV_REMOVE_FLAG(aws_a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
        }
    }
}


void bt_sink_srv_aws_a2dp_wait_start_cnf_timeout(uint32_t timer_id, uint32_t data)
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *sp_dev = NULL;
    bt_sink_srv_music_context_t *ctx = NULL;

    ctx = bt_sink_srv_music_get_context();

    sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);

    bt_sink_srv_report("[sink][aws_a2dp]start_cnf_timeout(s)--flag: 0x%08x, op: 0x%08x\n", sp_dev->flag, sp_dev->op);

    if ((sp_dev) &&
        (sp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START)) {
            BT_SINK_SRV_REMOVE_FLAG(sp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
            ret = bt_a2dp_start_streaming_response(sp_dev->a2dp_hd, true);
            /* Set SPK channel stereo */
            hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO);
            /* Disable transfer pkt */
            bt_sink_srv_music_disable_transfer_packet();
            if (BT_STATUS_SUCCESS == ret) {
                /* Reset last packet information */
                bt_sink_srv_a2dp_reset_last_pkt_info(&(ctx->med_pkt_info));
                sink_srv_init_media_play_info(&(ctx->med_play_info), &(sp_dev->codec), sp_dev->med_handle.med_hd);
                bt_sink_srv_media_mgr_set_codec_type(sp_dev->codec.type, &(sp_dev->codec));
                ctx->simple_count = 0;
                ctx->threshold_count = sinv_srv_util_us_2_sample_count(BT_SINK_SRV_MUSIC_AWS_PREPARE_BUF_THRESHOLD, ctx->med_play_info.sample_rate);
                bt_sink_srv_report("[sink][aws_a2dp]start_cnf_timeout--hold: %d", ctx->threshold_count);
                bt_sink_srv_music_state_machine_handle(sp_dev, BT_SINK_SRV_MUSIC_EVT_CODEC_OPEN, NULL);
            } else {
                /* Error handle */
                bt_sink_srv_music_state_machine_handle(sp_dev, BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL, NULL);
            }
    } else {
        bt_sink_srv_report("[sink][aws_a2dp]start_cnf_timeout(error_flag)--flag: 0x%x", sp_dev->flag);
    }
}

#endif /* __BT_AWS_A2DP_SUPPORT__ */

