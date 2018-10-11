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

#include "bt_sink_srv_a2dp.h"
#include "bt_sink_srv_avrcp.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_action.h"
#include "bt_sink_srv_ami.h"
#include "bt_gap.h"
#include "hal_audio.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_sink_srv_utils.h"
#include "bt_codec.h"
#include "bt_sink_srv_media_mgr.h"
#include "sink_srv_util.h"
#include "bt_sink_srv_audio_sync.h"
#ifdef __BT_AWS_A2DP_SUPPORT__
#include "bt_aws.h"
#include "bt_sink_srv_aws.h"
#include "bt_sink_srv_aws_a2dp.h"
#endif

#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
#include "bt_timer_external.h"
#endif

#include "hal_gpt.h"

#ifdef __BT_SINK_SRV_VOICE_PROMPT__
#include "hal_nvic_internal.h"
#include "audio_src_srv_internal.h"
#endif
#include "bt_sink_srv_resource.h"


#define BT_SINK_SRV_A2DP_DATA_RECE_CNT          (10)

#ifdef MTK_BT_A2DP_AAC_ENABLE
#define BT_SINK_SRV_A2DP_AAC_SUPPORT
#endif


/* currently care sink role */
#ifdef BT_SINK_SRV_A2DP_AAC_SUPPORT
#define BT_SINK_SRV_A2DP_CODEC_MAX_NUM          (2)
#else
#define BT_SINK_SRV_A2DP_CODEC_MAX_NUM          (1)
#endif

#ifdef __BT_AWS_A2DP_SUPPORT__
#define BT_SINK_SRV_A2DP_BASE_UPDATE_TIME    (20 * 60 * 1000)
#define BT_SINK_SRV_A2DP_BASE_UPDATE_COUNT   (BT_SINK_SRV_A2DP_BASE_UPDATE_TIME/23)
#endif

bt_sink_srv_a2dp_pseudo_handle_t g_a2dp_pse_hd[BT_SINK_SRV_A2DP_PSEUDO_COUNT] = {{0}};

bt_a2dp_codec_capability_t g_bt_sink_srv_a2dp_codec_list[BT_SINK_SRV_A2DP_CODEC_MAX_NUM];

bt_a2dp_streaming_received_ind_t *bt_sink_srv_a2dp_rece_ind = NULL;
bt_hci_packet_t *bt_sink_srv_a2dp_pkt = NULL;


uint32_t g_a2dp_gpt_run_count_begin = 0;
uint32_t g_a2dp_gpt_run_count_end = 0;

uint32_t g_a2dp_srv_rece_pkt_byte = 0;
uint32_t g_a2dp_decoder_read_pkt_byte = 0;

//const static uint8_t BT_SINK_SRV_A2DP_AAC_SYNC_HEAD[BT_SINK_SRV_AAC_ADTS_LENGTH] = {0xFF, 0xF9, 0x40, 0x20, 0x00, 0x1F, 0xFC};

extern uint32_t sink_loc_play_nclk;
extern uint32_t sink_loc_play_nclk_intra;

static const bt_codec_sbc_t sbc_cap[1] = {
    {
        25,  // min_bit_pool
        75,  // max_bit_pool
        0xf, // block_len: all
        0xf, // subband_num: all
        0x3, // both snr/loudness
        0xf, // sample_rate: all
        0xf  // channel_mode: all
    }
};

#ifdef BT_SINK_SRV_A2DP_AAC_SUPPORT
static bt_codec_aac_t aac_cap[1] = {
    {
        true,    /*VBR         */
        0xc0,    /*Object type */
        0x03,    /*Channels    */
        0x0ff8,  /*Sample_rate */
        0x60000  /*bit_rate, 384 Kbps */
    }
};
#endif


int32_t bt_sink_srv_a2dp_change_volume(uint8_t type);


extern void bt_sink_srv_music_a2dp_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param);

static audio_src_srv_handle_t *bt_sink_srv_a2dp_alloc_pseudo_handle(void);

static void bt_sink_srv_a2dp_free_pseudo_handle(audio_src_srv_handle_t *hd);

void bt_sink_srv_a2dp_reset_last_pkt_info(sink_media_packet_info_t *last_pkt_info);

#ifdef __BT_AWS_A2DP_SUPPORT__
static void bt_sink_srv_a2dp_gpt_cb(void *user_data);

static int32_t bt_sink_srv_a2dp_suspend_request(uint32_t handle, bt_sink_srv_music_device_t *dev);

static void bt_sink_srv_a2dp_get_bt_clk_offset(uint32_t evt_id, void *param, void *user_data);

static void bt_sink_srv_a2dp_handle_trans_packet(bt_a2dp_streaming_received_ind_t *data_ind);
#endif

#ifdef __BT_SINK_SRV_VOICE_PROMPT__
static void bt_sink_srv_a2dp_wait_vpa_timeout(uint32_t timer_id, uint32_t data);
#endif

static bool g_a2dp_codec_aac_support = true;

void bt_sink_srv_switch_aac_codec(bool enable)
{
    bt_sink_srv_report("[sink][a2dp] aac switch, origin: %d, current: %d", 
                                          g_a2dp_codec_aac_support, enable);
    g_a2dp_codec_aac_support = enable;
}


bt_status_t bt_sink_srv_a2dp_get_init_params(bt_a2dp_init_params_t *param)
{
    int32_t ret = BT_STATUS_FAIL;
    uint32_t num = 0;

    if (param) {
        /* init sink sep */
        BT_A2DP_MAKE_SBC_CODEC(g_bt_sink_srv_a2dp_codec_list + num, BT_A2DP_SINK,
                               sbc_cap[0].min_bit_pool, sbc_cap[0].max_bit_pool,
                               sbc_cap[0].block_length, sbc_cap[0].subband_num,
                               sbc_cap[0].alloc_method, sbc_cap[0].sample_rate,
                               sbc_cap[0].channel_mode);
        num++;
#ifdef BT_SINK_SRV_A2DP_AAC_SUPPORT
        BT_A2DP_MAKE_AAC_CODEC(g_bt_sink_srv_a2dp_codec_list + num, BT_A2DP_SINK,
                               aac_cap[0].vbr, aac_cap[0].object_type, aac_cap[0].channels,
                               aac_cap[0].sample_rate, aac_cap[0].bit_rate);
        num++;
#endif

        /* init source sep */

        /* fill init params */
        param->codec_number = num;
        param->codec_list = g_bt_sink_srv_a2dp_codec_list;

        ret = BT_STATUS_SUCCESS;
    }

    bt_sink_srv_report("[sink][a2dp]init-ret: %d\n", ret);

    return ret;
}


static void bt_sink_srv_a2dp_fill_am_aud_param(bt_sink_srv_am_audio_capability_t  *aud_cap, 
                                bt_a2dp_codec_capability_t *a2dp_cap, bt_a2dp_role_t role)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    memset(aud_cap, 0x00, sizeof(bt_sink_srv_am_audio_capability_t));
    aud_cap->type = A2DP;
    aud_cap->codec.a2dp_format.a2dp_codec.role = role;
    memcpy(&(aud_cap->codec.a2dp_format.a2dp_codec.codec_cap), a2dp_cap, sizeof(bt_a2dp_codec_capability_t));
    aud_cap->audio_stream_out.audio_device = BT_SINK_SRV_A2DP_OUTPUT_DEVICE;
    aud_cap->audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t)(ctx->vol_lev);
    aud_cap->audio_stream_out.audio_mute = false;
}


static void bt_sink_srv_a2dp_init()
{
    int8_t ori_aid = 0;
    bt_sink_srv_music_context_t *ctx = NULL;

    ctx = bt_sink_srv_music_get_context();
    ori_aid = ctx->a2dp_aid;

    if (ctx->a2dp_aid != BT_SINK_SRV_INVALID_AID) {
        bt_sink_srv_ami_audio_close(ctx->a2dp_aid);
        ctx->a2dp_aid = BT_SINK_SRV_INVALID_AID;
    }

    ctx->a2dp_aid = bt_sink_srv_ami_audio_open(AUD_MIDDLE, bt_sink_srv_music_a2dp_ami_hdr);
    bt_sink_srv_report("[sink][a2dp]init-a2dp_aid: %d, ori: %d",
                       ctx->a2dp_aid, ori_aid);
    #if defined(__GNUC__)
    bt_sink_srv_report("[sink][a2dp]--Version: %s", "__GNUC__");
    #endif

    #if defined(__ARMCC_VERSION)
    bt_sink_srv_report("[sink][a2dp]--Version: %s", "__ARMCC_VERSION");
    #endif

    #if defined(__ICCARM__)
    bt_sink_srv_report("[sink][a2dp]--Version: %s", "__ICCARM__");
    #endif
}


void bt_sink_srv_a2dp_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param)
{
    bt_sink_srv_music_context_t *ctx = NULL;
    int32_t ret = 0;
    int32_t err_code = 0;
    bool accept_start = false;
    bt_sink_srv_music_device_t *run_dev = NULL, *aws_dev = NULL;
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_timer_ext_status_t timer_ret = BT_TIMER_EXT_STATUS_SUCCESS;
#endif
    ctx = bt_sink_srv_music_get_context();
    run_dev = ctx->run_dev;
    //bt_sink_srv_assert(run_dev);
    if ((ctx->a2dp_aid == aud_id) &&
        (msg_id == AUD_A2DP_PROC_IND) && 
        (sub_msg == AUD_STREAM_EVENT_DATA_REQ ||
         sub_msg == AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW ||
         sub_msg == AUD_CODEC_MEDIA_AWS_CHECK_UNDERFLOW)) {
        // drop
        ;
    } else {
        bt_sink_srv_report("[sink][a2dp]ami_hdr[s]-aid: %d, aud_id: %d, msg_id: %d, sub_msg: %d, 2nd: 0x%x",
                           ctx->a2dp_aid, aud_id, msg_id, sub_msg, sub_msg);
    }
    if (ctx->a2dp_aid == aud_id) {
        switch (msg_id) {
            case AUD_SELF_CMD_REQ: {
                if (AUD_CMD_FAILURE == sub_msg) {
                }
                break;
            }

            case AUD_SINK_OPEN_CODEC: {
                if ((run_dev) && (run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC)) {
                    /* Save codec handle */
                    bt_sink_srv_memcpy(&(run_dev->med_handle), param, sizeof(bt_sink_srv_am_media_handle_t));

                    BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC);

                    /* Set codec open flag */
                    BT_SINK_SRV_SET_FLAG(run_dev->op, BT_SINK_SRV_MUSIC_OP_CODEC_OPEN);

                    /* Clear media node */
                    bt_sink_srv_media_mgr_clear();

                    aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
                    #ifdef __BT_AWS_A2DP_SUPPORT__
                    if ((aws_dev) &&
                        (aws_dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT)) {
                        if (run_dev->op & BT_SINK_SRV_MUSIC_OP_AWS_START) {
                            //ctx->trans = true;
                            accept_start = true;
                        } else {
                            /* Wait AWS start streaming confirm */
                            BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
                            bt_sink_srv_aws_a2dp_start_request(aws_dev->aws_hd, run_dev);
                        #ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
                            timer_ret = bt_timer_ext_start(BT_SINK_SRV_A2DP_WAIT_AWS_CNF_TIMER_ID, (uint32_t)run_dev, BT_SINK_SRV_A2DP_WAIT_AWS_CNF_TIMER_DUR, bt_sink_srv_aws_a2dp_wait_start_cnf_timeout);
                            bt_sink_srv_assert(timer_ret == BT_TIMER_EXT_STATUS_SUCCESS && "start wait timer fail ");
                            bt_sink_srv_report("[sink][a2dp]ami_hdr--start wait timer");
                        #endif
                        }
                        /* Set SPK channel L */
                        hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO_BOTH_L_CHANNEL);
                    } else {
                        accept_start = true;
                        /* Set SPK channel stereo */
                        hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO);
                    }
                    #else
                        accept_start = true;
                        hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO);
                    #endif
                    if (accept_start) {
                    #ifdef __BT_SINK_SRV_AVRCP_STATUS_SUPPORT__
                        if (run_dev->op & BT_SINK_SRV_MUSIC_AVRCP_PLAY_TRIGGER) {
                            ret = BT_STATUS_SUCCESS;
                        } else {
                            ret = bt_a2dp_start_streaming_response(run_dev->a2dp_hd, true);
                        }
                    #else
                        ret = bt_a2dp_start_streaming_response(run_dev->a2dp_hd, true);
                    #endif
                        if ((BT_STATUS_SUCCESS == ret) && (ctx->run_dev)) {
                            /* Reset last packet information */
                            bt_sink_srv_a2dp_reset_last_pkt_info(&(ctx->med_pkt_info));
                            sink_srv_init_media_play_info(&(ctx->med_play_info), &(run_dev->codec), run_dev->med_handle.med_hd);
                            bt_sink_srv_media_mgr_set_codec_type(run_dev->codec.type, &(run_dev->codec));
                        #ifdef __BT_AWS_A2DP_SUPPORT__
                            ctx->simple_count = 0;
                            ctx->threshold_count = sinv_srv_util_us_2_sample_count(BT_SINK_SRV_MUSIC_AWS_PREPARE_BUF_THRESHOLD, ctx->med_play_info.sample_rate);
                        #else
                            ctx->simple_count = 0;
                            ctx->threshold_count = sinv_srv_util_us_2_sample_count(BT_SINK_SRV_MUSIC_PREPARE_BUF_THRESHOLD, ctx->med_play_info.sample_rate);
                        #endif
                            bt_sink_srv_report("[sink][a2dp]ami_hdr--hold: %d", ctx->threshold_count);
                            bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_CODEC_OPEN, NULL);
                        } else {
                            if (ctx->run_dev) {
                                /* Error handle */
                                bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL, NULL);
                            }
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

            case AUD_A2DP_PROC_IND: {
                if (run_dev) {
                    if (sub_msg == AUD_A2DP_CODEC_RESTART) {
                        bt_sink_srv_media_mgr_clear();
                        BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);
                        BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP);
                        sink_srv_util_send_event(SINK_SRV_EVT_CODEC_STOP, run_dev);
                        //bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_RECOVER, NULL);
                    } else if (sub_msg == AUD_A2DP_AWS_UNDERFLOW) {
                        bt_sink_srv_media_mgr_clear();
                        BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);
                        BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP);
                        sink_srv_util_send_event(SINK_SRV_EVT_CODEC_STOP, run_dev);
                        //bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_RECOVER, NULL);
                    }
                } else {
                    bt_sink_srv_report("[sink][a2dp]ami_hdr--empty run_dev, plase note!!!");
                }
                break;
            }
            default:
                break;
        }
    }

    if (ctx->a2dp_aid == aud_id && msg_id == AUD_A2DP_PROC_IND &&
        (sub_msg == AUD_STREAM_EVENT_DATA_REQ ||
         sub_msg == AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW ||
         sub_msg == AUD_CODEC_MEDIA_AWS_CHECK_UNDERFLOW)) {
        // drop
        ;
    } else {
        bt_sink_srv_report("[sink][a2dp]ami_hdr[e]-err_code: %d, ret: %d", err_code, ret);
    }
}


int32_t bt_sink_srv_a2dp_handle_connect_cnf(bt_a2dp_connect_cnf_t *conn_cnf)
{
    bt_sink_srv_music_device_t *dev = NULL;
    //bt_sink_srv_music_context_t *ctx = NULL;
    int32_t ret = 0;
    bool connected = false;
    bt_bd_addr_t dev_addr;
    uint32_t hd = 0;

    //ctx = bt_sink_srv_music_get_context();

    bt_sink_srv_report("[sink][a2dp]connect_cnf-a2dp_hd: 0x%x", conn_cnf->handle);

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(conn_cnf->handle)));

    bt_sink_srv_assert(dev);
    bt_sink_srv_memcpy(&dev_addr, &(dev->dev_addr), sizeof(bt_bd_addr_t));
    if (conn_cnf->status == BT_STATUS_SUCCESS) {
        /* Save codec capability */
        memcpy(&(dev->codec), (conn_cnf->codec_cap), sizeof(bt_a2dp_codec_capability_t));

        bt_sink_srv_report("[sink][a2dp]connect_cnf, codec_type:%d, codec_type: %d", 
                            dev->codec.type , conn_cnf->codec_cap->type);

        /* As slave */
        bt_sink_srv_cm_set_role(&(dev->dev_addr), BT_ROLE_SLAVE);

        BT_SINK_SRV_SET_FLAG(dev->conn_bit, BT_SINK_SRV_MUSIC_A2DP_CONN_BIT);

        bt_sink_srv_report("[sink][a2dp]connect_cnf-con_bit: 0x%x", dev->conn_bit);
        if (!(dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
            ret = bt_avrcp_connect(&hd, (const bt_bd_addr_t *)(&(dev->dev_addr)));
            if (BT_STATUS_SUCCESS == ret) {
                dev->avrcp_hd = hd;
            }
            bt_sink_srv_report("[sink][a2dp]connect_cnf-ret: %d, avrcp_hd: 0x%x", ret, hd);
        }
        connected = true;
        /* Init pse handle */
        dev->handle = bt_sink_srv_a2dp_alloc_pseudo_handle();
        dev->handle->dev_id = bt_sink_srv_music_convert_btaddr_to_devid(&(dev->dev_addr));
        dev->handle->priority = AUDIO_SRC_SRV_PRIORITY_NORMAL;
        /* State machine handle */
        bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_READY, NULL);
    } else {
        /* State machine handle */
        bt_sink_srv_music_state_machine_handle(dev, BT_A2DP_CONNECT_CNF, conn_cnf);
        BT_SINK_SRV_REMOVE_FLAG(dev->conn_bit, BT_SINK_SRV_MUSIC_A2DP_CONN_BIT);
        if (!(dev->conn_bit)) {
            bt_sink_srv_music_reset_device(dev);
        } else {
            dev->a2dp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        }
    }
    if (connected) {
        bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_PROFILE_A2DP_SINK, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED);
    }

#ifdef __BT_SINK_SRV_MUSIC_MODE_SUPPORT__
    extern void bt_sink_srv_music_mode_timer_hdl(uint32_t timer_id, uint32_t data);
    if (connected) {
        bt_timer_ext_status_t timer_ret;
        if (bt_timer_ext_find(BT_SINK_SRV_MUSIC_MODE_TIMER_ID)) {
            bt_timer_ext_stop(BT_SINK_SRV_MUSIC_MODE_TIMER_ID);
        }
        bt_timer_ext_start(BT_SINK_SRV_MUSIC_MODE_TIMER_ID, (uint32_t)NULL, BT_SINK_SRV_MUSIC_MODE_TIMER_DUR, bt_sink_srv_music_mode_timer_hdl);
        bt_sink_srv_assert(timer_ret == BT_TIMER_EXT_STATUS_SUCCESS && "start check timer fail ");
        bt_sink_srv_report("[Sink][mode]a2dp_con_cnf--start check timer");
    }
#endif /* __BT_SINK_SRV_MUSIC_MODE_SUPPORT__ */
    return ret;
}


int32_t bt_sink_srv_a2dp_handle_connect_ind(bt_a2dp_connect_ind_t *conn_ind)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = 0;

    bt_sink_srv_report("[sink][a2dp]connect_ind-a2dp_hd: 0x%x", conn_ind->handle);

    /* A2dp connected */
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_AVRCP, (void *)(conn_ind->address));
    if (dev) {
        ;
    } else {
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP);
    }
    if (dev) {
        dev->a2dp_hd = conn_ind->handle;
        dev->role = conn_ind->role;
        memcpy(&(dev->dev_addr), conn_ind->address, sizeof(bt_bd_addr_t));
        BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_CON_IND);
    }
    ret = bt_a2dp_connect_response(conn_ind->handle, true);

    bt_sink_srv_music_state_machine_handle(dev, BT_A2DP_CONNECT_IND, conn_ind);

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_disconnect_cnf(bt_a2dp_disconnect_cnf_t *disconn_cnf)
{
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
    bt_bd_addr_t dev_addr = {0};
    int32_t ret = 0;
    bool free_hd = false;

    bt_sink_srv_report("[sink][a2dp]disconnect_cnf-hd: 0x%x", disconn_cnf->handle);

    a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(disconn_cnf->handle)));
    if (a2dp_dev) {
        bt_sink_srv_memcpy(&dev_addr, &(a2dp_dev->dev_addr), sizeof(bt_bd_addr_t));
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->conn_bit, BT_SINK_SRV_MUSIC_A2DP_CONN_BIT);
        /* Clear recover flag */
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);
        if (!(a2dp_dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY)) {
            free_hd = true;
        }
        /* Music state machine handle first */
        bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_A2DP_DISCONNECT_CNF, NULL);
    }

    if (free_hd) {
        /* Deinit pse handle */
        bt_sink_srv_a2dp_free_pseudo_handle(a2dp_dev->handle);
        a2dp_dev->handle = NULL;
        if (!(a2dp_dev->conn_bit)) {
            bt_sink_srv_music_reset_device(a2dp_dev);
        }
    }

    bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_PROFILE_A2DP_SINK, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED);

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_disconnect_ind(bt_a2dp_disconnect_ind_t *disconn_ind)
{
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
    bt_bd_addr_t dev_addr = {0};
    int32_t ret = 0;
    bool free_hd = false;

    bt_sink_srv_report("[sink][a2dp]disconnect_ind-hd: 0x%x", disconn_ind->handle);

    a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(disconn_ind->handle)));
    if (a2dp_dev) {
        bt_sink_srv_memcpy(&dev_addr, &(a2dp_dev->dev_addr), sizeof(bt_bd_addr_t));
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->conn_bit, BT_SINK_SRV_MUSIC_A2DP_CONN_BIT);
        /* Clear recover flag */
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);
        if (!(a2dp_dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY)) {
            free_hd = true;
        }
        /* Music state machine handle first */
        bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_A2DP_DISCONNECT_IND, NULL);
    }

    if (free_hd) {
        /* Deinit pse handle */
        bt_sink_srv_a2dp_free_pseudo_handle(a2dp_dev->handle);
        a2dp_dev->handle = NULL;
        if (!(a2dp_dev->conn_bit)) {
            bt_sink_srv_music_reset_device(a2dp_dev);
        }
    }

    bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_PROFILE_A2DP_SINK, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED);

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_start_streaming_cnf(bt_a2dp_start_streaming_cnf_t *start_cnf)
{
    int32_t ret = 0;

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_start_streaming_ind(bt_a2dp_start_streaming_ind_t *start_ind)
{
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_device_t *aws_dev = NULL;
    int32_t ret = 0;
#ifdef __BT_SINK_SRV_VOICE_PROMPT__
    audio_src_srv_context_t *audio_src_ctx = audio_src_srv_get_ctx();
    bt_timer_ext_status_t timer_ret = BT_TIMER_EXT_STATUS_SUCCESS;
    uint32_t int_mask = 0;
    bt_sink_srv_music_device_t *a2dp_int_dev = NULL;
    bt_timer_ext_t *timer = NULL;
#endif

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(start_ind->handle)));
    aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
    bt_sink_srv_assert(dev);
    //bt_sink_srv_memcpy(&(dev->codec), start_ind->codec_cap, sizeof(bt_a2dp_codec_capability_t));

#ifdef __BT_SINK_SRV_VOICE_PROMPT__
    bt_sink_srv_report("[sink][a2dp]start_ind(noodle)-running: 0x%08x, type: %d, flag: 0x%08x, op: 0x%08x, hd: 0x:%08x",
                       audio_src_ctx->running, audio_src_ctx->running->type, dev->flag, dev->op, start_ind->handle);
    if (dev->op & BT_SINK_SRV_MUSIC_WAIT_VAP_RESUME) {
        /* Ignore it */
        return ret;
    }

#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    timer = bt_timer_ext_find(BT_SINK_SRV_A2DP_WAIT_VPA_TIMER_ID);
    if (timer) {
        bt_sink_srv_music_device_t *dev = (bt_sink_srv_music_device_t *)timer->data;
        bt_sink_srv_report("[sink][a2dp]start_ind(noodle--NOTE)--dev: 0x%08x, flag: 0x%08x, op: 0x%08x, hd: 0x%08x", dev, dev->flag, dev->op, dev->a2dp_hd);
    }
#endif

    /* disable int */
    int_mask = save_and_set_interrupt_mask();
    if (audio_src_ctx->running && audio_src_ctx->running->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_VP) {
        /* WSAP00041118 remove waiting list A2DP PSE handle */        
        a2dp_int_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_INTERRUPT, NULL);
        if (a2dp_int_dev) {            
            BT_SINK_SRV_REMOVE_FLAG(a2dp_int_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT);
            audio_src_srv_del_waiting_list(a2dp_int_dev->handle);
        }
        #ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
        timer_ret = bt_timer_ext_start(BT_SINK_SRV_A2DP_WAIT_VPA_TIMER_ID, (uint32_t)dev, BT_SINK_SRV_A2DP_WAIT_VPA_TIMER_DUR, bt_sink_srv_a2dp_wait_vpa_timeout);
        bt_sink_srv_assert(timer_ret == BT_TIMER_EXT_STATUS_SUCCESS && "start wait VPA timer fail ");
        bt_sink_srv_report("[sink][a2dp]start_ind(noodle)--start wait VPA timer");
        #endif
        audio_src_srv_add_waiting_list(dev->handle);
        BT_SINK_SRV_SET_FLAG(dev->op, BT_SINK_SRV_MUSIC_WAIT_VAP_RESUME);
        /* enable int */
        restore_interrupt_mask(int_mask);
        return ret;
    }
    /* enable int */
    restore_interrupt_mask(int_mask);
#endif

#ifdef __BT_SINK_SRV_AVRCP_STATUS_SUPPORT__
    if ((!aws_dev) && (dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN)) {
        ret = bt_a2dp_start_streaming_response(start_ind->handle, true);
        return ret;
    }
#endif /* __BT_SINK_SRV_AVRCP_STATUS_SUPPORT__ */

    bt_sink_srv_music_state_machine_handle(dev, BT_A2DP_START_STREAMING_IND, NULL);

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_suspend_streaming_cnf(bt_a2dp_suspend_streaming_cnf_t *suspend_cnf)
{
    int32_t ret = 0;

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_suspend_streaming_ind(bt_a2dp_suspend_streaming_ind_t *suspend_ind)
{
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
    bt_sink_srv_music_context_t *ctx = NULL;
    int32_t ret = 0;


    ctx = bt_sink_srv_music_get_context();

    a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(suspend_ind->handle)));

    if ((a2dp_dev) &&
        (ctx->run_dev) &&
        (a2dp_dev == ctx->run_dev) &&
        (a2dp_dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN)) {
        /* SP suspend */
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_PLAY);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_PLAYING);
        ret = bt_a2dp_suspend_streaming_response(suspend_ind->handle, true);
        bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_A2DP_SUSPEND_STREAMING_IND, NULL);
    } else {
        /* Sink pause music */
        ret = bt_a2dp_suspend_streaming_response(suspend_ind->handle, true);
    }

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_reconfigure_cnf(bt_a2dp_reconfigure_cnf_t *reconfigure_cnf)
{
    int32_t ret = 0;

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_reconfigure_ind(bt_a2dp_reconfigure_ind_t *reconfigure_ind)
{
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
    int32_t ret = 0;

    a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(reconfigure_ind->handle)));
    if (a2dp_dev) {
        memcpy(&(a2dp_dev->codec), reconfigure_ind->codec_cap, sizeof(bt_a2dp_codec_capability_t));
    }

    ret = bt_a2dp_reconfigure_response(reconfigure_ind->handle, true);

    return ret;
}
#ifdef __BT_AWS_A2DP_SUPPORT__

static int32_t bt_sink_srv_a2dp_sync_base_request(uint32_t handle, bt_sink_srv_music_aws_sync_info *sync_clock)
{
    int32_t ret = 0;
    uint8_t buf[256] = {0};
    bt_sink_srv_aws_cmd_t *req = NULL;

    req = (bt_sink_srv_aws_cmd_t *)buf;
    req->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_SYNC_BASE;
    req->type = BT_SINK_SRV_AWS_COMMAND_TYPE_REQ;
    req->accept = false;
    req->data_len = sizeof(bt_sink_srv_music_aws_sync_info);
    bt_sink_srv_memcpy(req->data, sync_clock, sizeof(bt_sink_srv_music_aws_sync_info));

    ret = bt_sink_srv_aws_send_cmd(handle, BT_SINK_SRV_AWS_MODULE_ID_A2DP, req);
    bt_sink_srv_report("[sink][a2dp]base req-ret: %d", ret);

    return ret;
}

static int32_t bt_sink_srv_a2dp_sync_base_clock(bt_a2dp_streaming_received_ind_t *data_ind)
{
    int32_t ret = 0;
    bt_sink_srv_music_pkt_info pkt_info;
    bt_hci_packet_t *trans_pkt = NULL;
    bt_sink_srv_music_context_t *ctx = NULL;
    sink_srv_clock_mapping_t base_clock = {{0}}; 
    sink_srv_clock_mapping_t target_clock = {{0}};
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;

    ctx = bt_sink_srv_music_get_context();
    trans_pkt = (bt_hci_packet_t *)(data_ind->data_node);
    pkt_info.sn = sink_srv_util_get_a2dp_sn(trans_pkt);
    pkt_info.ts = sink_srv_util_get_a2dp_time_stamp(trans_pkt);

    if (BT_SINK_SRV_A2DP_BASE_UPDATE_COUNT <= (pkt_info.sn - ctx->local_info.pkt_info.sn)) {
        base_clock.local_clk.nclk = ctx->local_info.clock.nclk;
        base_clock.local_clk.nclk_intra = ctx->local_info.clock.nclk_intra;
        base_clock.piconet_clk.nclk = ctx->piconet_info.clock.nclk;
        base_clock.piconet_clk.nclk_intra = ctx->piconet_info.clock.nclk_intra;
        base_clock.time_stamp = ctx->piconet_info.pkt_info.ts;
        base_clock.seq_idx = ctx->piconet_info.pkt_info.sn;

        target_clock.seq_idx = pkt_info.sn;
        target_clock.time_stamp = pkt_info.ts;

        sink_srv_calc_target_clock(&(ctx->med_play_info), &base_clock, &target_clock);

        /* Update base local clock */
        ctx->local_info.clock.nclk = target_clock.local_clk.nclk;
        ctx->local_info.clock.nclk_intra = target_clock.local_clk.nclk_intra;
        ctx->local_info.pkt_info.ts = target_clock.time_stamp;
        ctx->local_info.pkt_info.sn = target_clock.seq_idx;
        
        /* Update base bt clock */
        ctx->piconet_info.clock.nclk = target_clock.piconet_clk.nclk;
        ctx->piconet_info.clock.nclk_intra = target_clock.piconet_clk.nclk_intra;
        ctx->piconet_info.pkt_info.ts = target_clock.time_stamp;
        ctx->piconet_info.pkt_info.sn = target_clock.seq_idx;

        bt_sink_srv_report("[sink][a2dp]sync_base(s)--sn: 0x%x, ts: 0x%x, local_nclk: 0x%x, local_intra: 0x%x\r\n",
            target_clock.seq_idx, target_clock.time_stamp, target_clock.local_clk.nclk,
            target_clock.local_clk.nclk_intra);

        /** send sync base msg to AWS SPK2. */  
        aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
        if (aws_a2dp_dev) {
            bt_sink_srv_music_aws_sync_info sync_info = {{0}};

            sync_info.clock.nclk = target_clock.piconet_clk.nclk;
            sync_info.clock.nclk_intra = target_clock.piconet_clk.nclk_intra;
            sync_info.pkt_info.sn = target_clock.seq_idx;
            sync_info.pkt_info.ts = target_clock.time_stamp;
            sync_info.ts_diff_by_packet =  ctx->med_play_info.ts_diff_by_packet;
            
            bt_sink_srv_report("[sink][a2dp]sync_base(s)--sn: 0x%x, ts: 0x%x, bt_nclk: 0x%x, bt_intra: 0x%x, ts_diff: 0x%x\r\n",
                sync_info.pkt_info.sn, sync_info.pkt_info.ts, sync_info.clock.nclk,
                sync_info.clock.nclk_intra, sync_info.ts_diff_by_packet);   
            ret = bt_sink_srv_a2dp_sync_base_request(aws_a2dp_dev->aws_hd, &sync_info);
        }
    }

    //bt_sink_srv_report("[sink][a2dp]base sync-ret: %d", ret);
    return ret;
}
#endif

static int32_t bt_sink_srv_a2dp_handle_data_received_ind(bt_a2dp_streaming_received_ind_t *data_ind)
{
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_music_device_t *run_dev = NULL;
    bt_sink_srv_media_mgr_context_t *med_ctx = NULL;
    int32_t ret = 0;
    static uint32_t s_a2dp_error_log_loop_cnt = BT_SINK_SRV_A2DP_DATA_ERROR_LOOP_COUNT;
    #ifdef __BT_AWS_A2DP_SUPPORT__
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;
    bt_sink_srv_music_device_t *find_dev = NULL;
    uint32_t time_dur = 0;
    #endif
    uint32_t chk_ret = 0;
    sink_media_packet_info_t *last_pkt_info;
    sink_media_packet_info_t pre_packet;
    bt_hci_packet_t *next_packet;
    void *fill_pkt = NULL;
    #ifdef __BT_AWS_A2DP_SUPPORT__
    bt_hci_packet_t *pkt = NULL;
    uint32_t duration = 0;
    uint8_t type = 0;
    #endif


    ctx = bt_sink_srv_music_get_context();
    med_ctx = bt_sink_srv_media_mgr_get_context();
    run_dev = ctx->run_dev;

#ifdef __BT_AWS_A2DP_SUPPORT__
    bt_sink_srv_music_disable_log();
    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
    bt_sink_srv_music_enable_log();
    bt_sink_srv_a2dp_handle_trans_packet(data_ind);
#endif

#ifdef __BT_AWS_A2DP_SUPPORT__
    bt_sink_srv_a2dp_sync_base_clock(data_ind);
#endif

    if ((run_dev) &&
        (run_dev->a2dp_hd == data_ind->handle)) {

        #ifdef __BT_AWS_A2DP_SUPPORT__
        if ((run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_PSEDEV_PLAYING)) {
            BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_PSEDEV_PLAYING);
            if ((run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_GPT_TIMEOUT)) {
                BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_PLAYING);
                BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_GPT_TIMEOUT);
            } else {
                BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_PLAYING);
                bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_PLAYING, NULL);
                bt_sink_srv_state_set(BT_SINK_SRV_STATE_STREAMING);
            }
        }
        #endif
        next_packet = (bt_hci_packet_t *)data_ind->data_node;
        last_pkt_info = &(ctx->med_pkt_info);
    #ifdef __BT_AWS_A2DP_SUPPORT__
        chk_ret = sink_srv_a2dp_check_media_continuous(run_dev->med_handle.med_hd, &(ctx->med_play_info), last_pkt_info, next_packet);
    #endif
        bt_sink_srv_memcpy(&pre_packet, last_pkt_info, sizeof(sink_media_packet_info_t));
        //chk_ret = SINK_SRV_PLH_TYPE_CONTINUOUS;
        /* Debug info */
        g_a2dp_srv_rece_pkt_byte += (next_packet->packet_length - next_packet->offset);

        /* Update last packet information */
        last_pkt_info->seq_num = sink_srv_util_get_a2dp_sn((bt_hci_packet_t *)(data_ind->data_node));
        last_pkt_info->time_stamp = sink_srv_util_get_a2dp_time_stamp((bt_hci_packet_t *)(data_ind->data_node));
        last_pkt_info->sample_counts = sink_srv_util_get_packet_sample_counts(run_dev->med_handle.med_hd, (bt_hci_packet_t *)(data_ind->data_node));

    #ifdef __BT_AWS_A2DP_SUPPORT__
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
                bt_sink_srv_report("[sink][a2dp]rece_ind--fill_pkt error");
            }
        } else if (SINK_SRV_PLH_TYPE_CONTINUOUS == chk_ret) {
            /* Normal */
            ;
        }
     #endif
        ret = bt_sink_srv_media_mgr_push_item((uint32_t)(data_ind->data_node));

        if (ret < 0) {
            if (s_a2dp_error_log_loop_cnt == BT_SINK_SRV_A2DP_DATA_ERROR_LOOP_COUNT) {
                bt_sink_srv_report("[sink][a2dp]rece_ind(error_push)--ret: %d, item: %d, size: %d, sn: 0x%x",
                    ret, med_ctx->item_count, med_ctx->size, sink_srv_util_get_a2dp_sn((bt_hci_packet_t *)(data_ind->data_node)));
                s_a2dp_error_log_loop_cnt = 0;
            }
            ++s_a2dp_error_log_loop_cnt;
            return ret;
        }

        /* Update last packet information */
        last_pkt_info->seq_num = sink_srv_util_get_a2dp_sn((bt_hci_packet_t *)(data_ind->data_node));
        last_pkt_info->time_stamp = sink_srv_util_get_a2dp_time_stamp((bt_hci_packet_t *)(data_ind->data_node));
        last_pkt_info->sample_counts = sink_srv_util_get_packet_sample_counts(run_dev->med_handle.med_hd, (bt_hci_packet_t *)(data_ind->data_node));

        if (run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER) {
            BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);
            BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_PLAY);
            BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_PLAY);
            /* Async DRV play */
            sink_srv_util_send_event(SINK_SRV_EVT_CODEC_PLAY, run_dev);

            BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
            BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_GPT_TIMEOUT);
        }

    #ifdef __BT_AWS_A2DP_SUPPORT__
        if ((run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER)) {
            if (aws_a2dp_dev) {
                find_dev = aws_a2dp_dev;
                type = 1;
            } else {
                find_dev = run_dev;
                type = 0;
            }
            pkt = (bt_hci_packet_t *)bt_sink_srv_music_find_suitable_packet(find_dev, &duration, type);
            if (pkt) {
                time_dur = duration;
                /* Update base bt clock */
                //bt_sink_srv_music_update_aws_sync_info(run_dev, pkt, 1);
                //ctx->piconet_info.clock.nclk_intra += time_dur;
                ctx->play_time.nclk = ctx->piconet_info.clock.nclk;
                ctx->play_time.nclk_intra = ctx->piconet_info.clock.nclk_intra + time_dur;
                ctx->gpt_port = (hal_gpt_port_t)bt_sink_srv_music_start_gpt_timer(bt_sink_srv_a2dp_gpt_cb, (void *)run_dev, time_dur);
                BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
            } else {
                /* Wait next packet incoming */
            }
        }
    #endif /* __BT_AWS_A2DP_SUPPORT__ */

        if (!(run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_PLAY)) {
            /* Calc simple count */
            if (ctx->simple_count >= ctx->threshold_count) {
                BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_PLAY);
                BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_PLAY);
                /* Async DRV play */
                sink_srv_util_send_event(SINK_SRV_EVT_CODEC_PLAY, run_dev);
                #ifdef __BT_AWS_A2DP_SUPPORT__
                //if (aws_a2dp_dev) {
                if ((aws_a2dp_dev) &&
                    !(run_dev->op & BT_SINK_SRV_MUSIC_OP_AUTO_SYNC)) {
                    /* Get aws bt clock offset */
                    BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET);
                    ret = bt_sink_srv_music_get_bt_clk_offset(aws_a2dp_dev->aws_hd, bt_sink_srv_a2dp_get_bt_clk_offset, run_dev);
                    if (ret != BT_STATUS_SUCCESS) {
                        BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET);
                        bt_sink_srv_report("[sink][a2dp]rece_ind(error_get_bt_clk_offset)--ret: %d", ret);
                    }
                } else {
                    /* Get bt local bt clock */
                    ret = bt_aws_get_bt_local_time(&(ctx->local_info.clock));
                    pkt = (bt_hci_packet_t *)bt_sink_srv_media_mgr_get_first_item();
                    ctx->local_info.pkt_info.sn = sink_srv_util_get_a2dp_sn(pkt);
                    ctx->local_info.pkt_info.ts = sink_srv_util_get_a2dp_time_stamp(pkt);
                    bt_sink_srv_report("[sink][a2dp]rece_ind(local_clk)--sn: 0x%x, ts: 0x%x, nclk: 0x%x, intra: 0x%x",
                        ctx->local_info.pkt_info.sn, ctx->local_info.pkt_info.ts, ctx->local_info.clock.nclk, ctx->local_info.clock.nclk_intra);
                    if (ret != BT_STATUS_SUCCESS) {
                        bt_sink_srv_report("[sink][a2dp]rece_ind(error_get_local_clk)--ret: %d", ret);
                    } else {
                        /* Start gpt timer */
                        time_dur = BT_SINK_SRV_A2DP_AWS_TIMER_DUR;
                        ctx->local_info.clock.nclk_intra += BT_SINK_SRV_A2DP_AWS_TIMER_DUR;
                        ctx->play_time.nclk = ctx->local_info.clock.nclk;
                        ctx->play_time.nclk_intra = ctx->local_info.clock.nclk_intra;
                        ctx->gpt_port = (hal_gpt_port_t)bt_sink_srv_music_start_gpt_timer(bt_sink_srv_a2dp_gpt_cb, (void *)run_dev, time_dur);
                    }

                    /* Continue auto sync flow */
                    if (run_dev->op & BT_SINK_SRV_MUSIC_OP_AUTO_SYNC) {
                        BT_SINK_SRV_REMOVE_FLAG(run_dev->op, BT_SINK_SRV_MUSIC_OP_AUTO_SYNC);
                        /* Update SPK1 bt clock */
                        bt_sink_srv_a2dp_update_base_bt_clock();
                    }
                }
                #endif /* __BT_AWS_A2DP_SUPPORT__ */
            } else {
                ctx->simple_count += sink_srv_util_get_packet_sample_counts(run_dev->med_handle.med_hd, (bt_hci_packet_t *)(data_ind->data_node));
            }
        }
    } else {
        /* Drop media data */
        bt_sink_srv_report("[sink][a2dp]rece_ind(error_drop)-run_dev: 0x%08x, state: %d, c_hd: 0x%08x, d_hd: 0x%08x",
            run_dev, run_dev->state, run_dev->a2dp_hd, data_ind->handle);
    }

    return ret;
}


static audio_src_srv_handle_t *bt_sink_srv_a2dp_alloc_pseudo_handle(void)
{
    int32_t i = 0;
    audio_src_srv_handle_t *hd = NULL;

    for (i = 0; i < BT_SINK_SRV_A2DP_PSEUDO_COUNT; ++i) {
        if (!(g_a2dp_pse_hd[i].flag & BT_SINK_SRV_A2DP_PSEUDO_FLAG_USEED)) {
            hd = g_a2dp_pse_hd[i].hd;
            BT_SINK_SRV_SET_FLAG(g_a2dp_pse_hd[i].flag, BT_SINK_SRV_A2DP_PSEUDO_FLAG_USEED);
            bt_sink_srv_music_fill_audio_src_callback(hd);
            break;
        }
    }

    bt_sink_srv_report("[sink][a2dp]alloc_pseudo_handle--hd: 0x%x", hd);

    bt_sink_srv_assert(hd);
    return hd;
}


static void bt_sink_srv_a2dp_free_pseudo_handle(audio_src_srv_handle_t *hd)
{
    int32_t i = 0;
    int32_t ret = 0;

    for (i = 0; i < BT_SINK_SRV_A2DP_PSEUDO_COUNT; ++i) {
        if ((hd) && 
            (g_a2dp_pse_hd[i].flag & BT_SINK_SRV_A2DP_PSEUDO_FLAG_USEED) &&
            (g_a2dp_pse_hd[i].hd == hd)) {
            BT_SINK_SRV_REMOVE_FLAG(g_a2dp_pse_hd[i].flag, BT_SINK_SRV_A2DP_PSEUDO_FLAG_USEED);
            ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_1ST;
            break;
        }
    }

    bt_sink_srv_report("[sink][a2dp]free_pseudo_handle--hd: 0x%x, ret: %d", hd, ret);
}

bool pass_through_on = false;

void bt_sink_srv_a2dp_reset_last_pkt_info(sink_media_packet_info_t *last_pkt_info)
{
    bt_sink_srv_assert(last_pkt_info);
    last_pkt_info->seq_num = 0xffff;
    last_pkt_info->time_stamp = 0xffffffff;
    last_pkt_info->sample_counts = 0xffffffff;
}

#ifdef MTK_AUDIO_PASS_THROUGH_ENABLED
extern void bt_codec_a2dp_pass_through_hook_callback (bool flag);
#endif

bt_sink_srv_status_t bt_sink_srv_a2dp_action_handler(bt_sink_srv_action_t action, void *param)
{
    bt_status_t ret = 0;
    bt_bd_addr_t *dev_addr = NULL;
    uint32_t hd = 0;
    int32_t err = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    bt_sink_srv_profile_connection_action_t *conn_info = NULL;
#ifdef __BT_AWS_A2DP_SUPPORT__
    bt_sink_srv_music_device_t *aws_dev = NULL, *sp_dev = NULL;
#endif

    bt_sink_srv_report("[sink][a2dp]process_a2dp_action[s]-action: 0x%x, base: 0x%x\n", action, BT_SINK_SRV_ACTION_A2DP_START);
    cntx = bt_sink_srv_music_get_context();

    switch (action) {
        case BT_SINK_SRV_ACTION_PROFILE_INIT: {
            bt_sink_srv_a2dp_init();
            #ifdef __BT_AWS_A2DP_SUPPORT__
            bt_sink_srv_aws_a2dp_init();
            #endif
            break;
        }

        case BT_SINK_SRV_ACTION_PROFILE_DEINIT: {
            if (cntx->a2dp_aid != BT_SINK_SRV_INVALID_AID) {
                bt_sink_srv_ami_audio_close(cntx->a2dp_aid);
                cntx->a2dp_aid = BT_SINK_SRV_INVALID_AID;
            }
            #ifdef __BT_AWS_A2DP_SUPPORT__
            if (cntx->aws_aid != BT_SINK_SRV_INVALID_AID) {
                bt_sink_srv_ami_audio_close(cntx->aws_aid);
                cntx->aws_aid = BT_SINK_SRV_INVALID_AID;
            }
            #endif
            break;
        }

        case BT_SINK_SRV_ACTION_PROFILE_CONNECT: {
            conn_info = (bt_sink_srv_profile_connection_action_t *)param;
            dev_addr = &(conn_info->address);
            if (conn_info->profile_connection_mask & BT_SINK_SRV_PROFILE_A2DP_SINK) {
                ret = bt_a2dp_connect(&hd, (const bt_bd_addr_t *)dev_addr, BT_A2DP_SINK);
                if (BT_STATUS_SUCCESS == ret) {
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, (void *)dev_addr);
                    if (dev) {
                        dev->a2dp_hd = hd;
                        dev->role = BT_A2DP_SINK;
                        /* unexpected, need handle */
                        err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
                    } else {
                        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, NULL);
                        if (dev) {
                            dev->a2dp_hd = hd;
                            dev->role = BT_A2DP_SINK;
                            bt_sink_srv_memcpy(&(dev->dev_addr), dev_addr, sizeof(bt_bd_addr_t));
                            /* normal case */
                            err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_1ST;
                        } else {
                            /* unexpected, no unused device, please check log */
                            err = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
                        }
                    }
                }
            }
            break;
        }

        case BT_SINK_SRV_ACTION_PROFILE_DISCONNECT: {
            conn_info = (bt_sink_srv_profile_connection_action_t *)param;
            dev_addr = &(conn_info->address);
            if (conn_info->profile_connection_mask & BT_SINK_SRV_PROFILE_A2DP_SINK) {
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, (void *)dev_addr);
                if ((dev) && (dev->conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT)) {
                    ret = bt_a2dp_disconnect(dev->a2dp_hd);
                } else {
                    err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
                }
            }
            break;
        }

        case BT_SINK_SRV_ACTION_VOLUME_UP: {
            #ifdef __BT_AWS_A2DP_SUPPORT__
            aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
            sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_SP, NULL);
            if (!aws_dev) {
                /* No aws case */
                bt_sink_srv_a2dp_change_volume(VOLUME_UP);
            } else if (aws_dev && sp_dev) {
                /* SPK1 case */
                bt_sink_srv_a2dp_change_volume(VOLUME_UP);
            } else {
                /* SPK2 case */
                bt_sink_srv_report("[sink][a2dp]process_a2dp_action-SPK2 VUP");
            }
            #else
            bt_sink_srv_a2dp_change_volume(VOLUME_UP);
            #endif /* __BT_AWS_A2DP_SUPPORT__ */
            break;
        }

        case BT_SINK_SRV_ACTION_VOLUME_DOWN: {
            #ifdef __BT_AWS_A2DP_SUPPORT__
            aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
            sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_SP, NULL);
            if (!aws_dev) {
                /* No aws case */
                bt_sink_srv_a2dp_change_volume(VOLUME_DOWN);
            } else if (aws_dev && sp_dev) {
                /* SPK1 case */
                bt_sink_srv_a2dp_change_volume(VOLUME_DOWN);
            } else {
                /* SPK2 case */
                bt_sink_srv_report("[sink][a2dp]process_a2dp_action-SPK2 VDN");
            }
            #else
            bt_sink_srv_a2dp_change_volume(VOLUME_DOWN);
            #endif /* __BT_AWS_A2DP_SUPPORT__ */
            break;
        }
    #ifdef MTK_AUDIO_PASS_THROUGH_ENABLED
        #include "hal_audio_internal_service.h"
        case BT_SINK_SRV_ACTION_SWITCH_PASS_THROUGH_MODE: {
            //bt_sink_srv_report("[sink][a2dp] pass_through_on: %d", pass_through_on);
            if (!pass_through_on) {
                bt_codec_a2dp_pass_through_hook_callback(true);
                audio_service_pass_through_set_flag(true);
                pass_through_on = true;
                bt_sink_srv_report("[sink][a2dp] pass through ON.");
            } else {
                audio_service_pass_through_set_flag(false);
                pass_through_on = false;
                bt_sink_srv_report("[sink][a2dp] pass through OFF.");
            }    
        }
        break;
    #endif /*MTK_AUDIO_PASS_THROUGH_ENABLED*/

    #ifdef __BT_SINK_SRV_MUSIC_MODE_SUPPORT__
        case BT_SINK_SRV_ACTION_SWITCH_MODE: {
            bt_sink_srv_music_mode_t cur_mode, new_mode;

            cur_mode = bt_sink_srv_get_music_mode();
            if (cur_mode == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
                new_mode = BT_SINK_SRV_MODE_BT_MUSIC;
            } else if (cur_mode == BT_SINK_SRV_MODE_BT_MUSIC) {
                new_mode = BT_SINK_SRV_MODE_LOCAL_MUSIC;
            } else {
                new_mode = BT_SINK_SRV_MODE_LOCAL_MUSIC;
            }

            bt_sink_srv_set_music_mode(new_mode);
            break;
        }
    #endif /* __BT_SINK_SRV_MUSIC_MODE_SUPPORT__ */

        default:
            break;
    }

    bt_sink_srv_report("[sink][a2dp]process_a2dp_action[e]-ret: %d, err: %d", ret, err);

    return BT_SINK_SRV_STATUS_SUCCESS;
}


static void bt_sink_srv_a2dp_clear_codec(bt_sink_srv_music_device_t *a2dp_dev)
{
    int32_t ret = 0;
    bt_sink_srv_music_context_t *ctx = NULL;
    #ifdef __BT_AWS_A2DP_SUPPORT__
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;
    #endif

    ctx = bt_sink_srv_music_get_context();

    /* Clear GPT Timer */
    if (ctx->gpt_port != HAL_GPT_MAX_PORT) {
        hal_gpt_stop_timer(ctx->gpt_port);
        hal_gpt_deinit(ctx->gpt_port);
        ctx->gpt_port = HAL_GPT_MAX_PORT;
    }

    /* Clear recover flag */
    BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);

    /* 1. DRV stop */
    if (a2dp_dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY) {
        BT_SINK_SRV_SET_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP);
        /* Sync DRV stop, or codec close must wait DRV stop done(cost time) */
        sink_srv_util_send_event(SINK_SRV_EVT_CODEC_STOP, a2dp_dev);
    }

    /* 2. Codec close */
    if (a2dp_dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN) {
        if (a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER) {
            /* Skip codec close */
            ;
        } else {
            ret = bt_sink_srv_ami_audio_stop(ctx->a2dp_aid);
            if (ret != AUD_EXECUTION_SUCCESS) {
                /* Failed close codec */
                bt_sink_srv_report("[sink][a2dp]clear_codec(error)--ret: %d", ret);
            } else {
                BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_OP_CODEC_OPEN);
            }
        }
    }

    /* 3. Transfer suspend SPK2 */
    #ifdef __BT_AWS_A2DP_SUPPORT__
    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
    if (a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER) {
        ;
    } else {
        if ((aws_a2dp_dev) &&
            (a2dp_dev->op & BT_SINK_SRV_MUSIC_OP_AWS_START)) {
            BT_SINK_SRV_SET_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_SUSPEND);
            /* Disable transfer pkt */
            bt_sink_srv_music_disable_transfer_packet();
            ret = bt_sink_srv_a2dp_suspend_request(aws_a2dp_dev->aws_hd, a2dp_dev);
            if (ret != BT_STATUS_SUCCESS) {
                bt_sink_srv_assert(0 && "[sink][a2dp]clear_codec--trans suspend fail");
            }
            BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_OP_AWS_START);
        }
    }
    #endif
    /* 4. Clear flag */
    /* Clear wait start gpt timer */
    BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
    /* Clear wait start gpt timeout */
    BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_GPT_TIMEOUT);
    /* Clear recover flag */
    //BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER);
}


bt_status_t bt_sink_srv_a2dp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    int32_t ret = 0;
    int32_t gpt_ret = 0;
    uint32_t time_dur = 0;
    static uint32_t s_a2dp_data_log_loop_cnt = BT_SINK_SRV_A2DP_DATA_RCE_LOOP_COUNT;

    if (((BT_SINK_SRV_A2DP_DATA_RCE_LOOP_COUNT == s_a2dp_data_log_loop_cnt) && (msg == BT_A2DP_STREAMING_RECEIVED_IND)) 
        || (msg != BT_A2DP_STREAMING_RECEIVED_IND && msg != BT_MEMORY_FREE_GARBAGE_IND)) {
        bt_sink_srv_report("[sink][a2dp]common_hdr[s]-msg: 0x%x, status: %d", msg, status);
        if (BT_SINK_SRV_A2DP_DATA_RCE_LOOP_COUNT == s_a2dp_data_log_loop_cnt) {
            s_a2dp_data_log_loop_cnt = 0;
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
        case BT_A2DP_CONNECT_CNF: {
            bt_a2dp_connect_cnf_t *conn_cnf = (bt_a2dp_connect_cnf_t *)buffer;
            ret = bt_sink_srv_a2dp_handle_connect_cnf(conn_cnf);
            break;
        }

        case BT_A2DP_CONNECT_IND: {
            bt_a2dp_connect_ind_t *conn_ind = (bt_a2dp_connect_ind_t *)buffer;
            ret = bt_sink_srv_a2dp_handle_connect_ind(conn_ind);
            break;
        }

        case BT_A2DP_DISCONNECT_CNF: {
            bt_a2dp_disconnect_cnf_t *disconn_cnf = (bt_a2dp_disconnect_cnf_t *)buffer;
            ret = bt_sink_srv_a2dp_handle_disconnect_cnf(disconn_cnf);
            break;
        }

        case BT_A2DP_DISCONNECT_IND: {
            bt_a2dp_disconnect_ind_t *disconn_ind = (bt_a2dp_disconnect_ind_t *)buffer;
            ret = bt_sink_srv_a2dp_handle_disconnect_ind(disconn_ind);
            break;
        }

        case BT_A2DP_START_STREAMING_CNF: {
            bt_a2dp_start_streaming_cnf_t *start_cnf = (bt_a2dp_start_streaming_cnf_t *)buffer;
            ret = bt_sink_srv_a2dp_handle_start_streaming_cnf(start_cnf);
            break;
        }

        case BT_A2DP_START_STREAMING_IND: {
            bt_a2dp_start_streaming_ind_t *start_ind = (bt_a2dp_start_streaming_ind_t *)buffer;
            bt_sink_srv_report("[sink][a2dp]start_ind--buff: 0x%x, cap: 0x%x", buffer, start_ind->codec_cap);
            //bt_a2dp_clear_gpio_12();

            if (start_ind->codec_cap->type == BT_A2DP_CODEC_SBC) {
                bt_sink_srv_report("[sink][a2dp]start_ind(sbc)--hd: 0x%x, type: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d",
                                   start_ind->handle,
                                   start_ind->codec_cap->type,
                                   start_ind->codec_cap->sep_type,
                                   start_ind->codec_cap->length,
                                   start_ind->codec_cap->codec.sbc.channel_mode,
                                   start_ind->codec_cap->codec.sbc.sample_freq,
                                   start_ind->codec_cap->codec.sbc.alloc_method,
                                   start_ind->codec_cap->codec.sbc.subbands,
                                   start_ind->codec_cap->codec.sbc.block_len,
                                   start_ind->codec_cap->codec.sbc.min_bitpool,
                                   start_ind->codec_cap->codec.sbc.max_bitpool);
            } else if (start_ind->codec_cap->type == BT_A2DP_CODEC_AAC) {
                bt_sink_srv_report("[sink][a2dp]start_ind(aac)--hd: 0x%x, type: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d, 8: %d, 9: %d",
                                   start_ind->handle,
                                   start_ind->codec_cap->type,
                                   start_ind->codec_cap->sep_type,
                                   start_ind->codec_cap->length,
                                   start_ind->codec_cap->codec.aac.object_type,
                                   start_ind->codec_cap->codec.aac.freq_h,
                                   start_ind->codec_cap->codec.aac.reserved,
                                   start_ind->codec_cap->codec.aac.channels,
                                   start_ind->codec_cap->codec.aac.freq_l,
                                   start_ind->codec_cap->codec.aac.br_h,
                                   start_ind->codec_cap->codec.aac.vbr,
                                   start_ind->codec_cap->codec.aac.br_m,
                                   start_ind->codec_cap->codec.aac.br_l);
            }

            ret = bt_sink_srv_a2dp_handle_start_streaming_ind(start_ind);
            break;
        }

        case BT_A2DP_SUSPEND_STREAMING_CNF: {
            bt_a2dp_suspend_streaming_cnf_t *suspend_cnf = (bt_a2dp_suspend_streaming_cnf_t *)buffer;

            ret = bt_sink_srv_a2dp_handle_suspend_streaming_cnf(suspend_cnf);
            break;
        }

        case BT_A2DP_SUSPEND_STREAMING_IND: {
            bt_a2dp_suspend_streaming_ind_t *suspend_ind = (bt_a2dp_suspend_streaming_ind_t *)buffer;

            ret = bt_sink_srv_a2dp_handle_suspend_streaming_ind(suspend_ind);
            break;
        }

        case BT_A2DP_RECONFIGURE_CNF: {
            bt_a2dp_reconfigure_cnf_t *reconfigure_cnf = (bt_a2dp_reconfigure_cnf_t *)buffer;

            ret = bt_sink_srv_a2dp_handle_reconfigure_cnf(reconfigure_cnf);
            break;
        }

        case BT_A2DP_RECONFIGURE_IND: {
            bt_a2dp_reconfigure_ind_t *reconfigure_ind = (bt_a2dp_reconfigure_ind_t *)buffer;

            ret = bt_sink_srv_a2dp_handle_reconfigure_ind(reconfigure_ind);
            break;
        }

        case BT_A2DP_STREAMING_RECEIVED_IND: {
            bt_hci_packet_t *pkt = NULL;
            bt_a2dp_streaming_received_ind_t *data_ind = (bt_a2dp_streaming_received_ind_t *)buffer;
            ++s_a2dp_data_log_loop_cnt;

            bt_sink_srv_a2dp_rece_ind = (bt_a2dp_streaming_received_ind_t *)buffer;
            bt_sink_srv_a2dp_pkt = (bt_hci_packet_t *)(data_ind->data_node);

            pkt = (bt_hci_packet_t *)(data_ind->data_node);
            if (pkt->ref_count != 1) {
                /* Drop this packet */
                bt_sink_srv_report("[sink][a2dp]rece_ind--pkt: 0x%x, ref: %d, len: %d, offset: %d, size: %d, data_ind: 0x%x",
                                   pkt, pkt->ref_count, pkt->packet_length,
                                   pkt->offset, sizeof(bt_hci_packet_t), data_ind);
                bt_sink_srv_assert(0);
                break;
            }
            bt_sink_srv_media_mgr_garbage_free();
            ret = bt_sink_srv_a2dp_handle_data_received_ind(data_ind);
            break;
        }

        case BT_MEMORY_FREE_GARBAGE_IND: {
            bt_sink_srv_media_mgr_garbage_free();
            break;
        }

        default:
            break;
    }

    if (msg != BT_A2DP_STREAMING_RECEIVED_IND && msg != BT_MEMORY_FREE_GARBAGE_IND) {
        bt_sink_srv_report("[sink][a2dp]common_hdr[e]-ret: %d", ret);
    }

    return ret;
}



void bt_sink_srv_a2dp_drv_play(void *param)
{
    bt_sink_srv_music_device_t *a2dp_dev = (bt_sink_srv_music_device_t *)param;
    #ifdef __BT_AWS_A2DP_SUPPORT__
    bt_sink_srv_music_device_t *aws_dev = NULL;
    #endif
    bt_sink_srv_music_context_t *ctx = NULL;
    uint8_t dev_db = 0;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    int32_t ret = 0;
    uint32_t gpt_run_count_begin = 0;
    uint32_t gpt_run_count_end = 0;
    int32_t gpt_ret = 0;
    uint32_t cost_dur = 0;

    ctx = bt_sink_srv_music_get_context();

    if (ctx->run_dev == a2dp_dev) {
        /* Volume setting */
        if (bt_sink_srv_cm_get_profile_data(&(a2dp_dev->dev_addr), BT_SINK_SRV_PROFILE_A2DP_SINK, &dev_db)) {
            if ((dev_db & 0xf0) == BT_SINK_SRV_A2DP_MAGIC_CODE) {
                /* use storge volume value */
                ctx->vol_lev = (dev_db & 0x0f);
            } else {
                /* use dedefault volume value and update it */
                ctx->vol_lev = BT_SINK_SRV_A2DP_VOL_DEF_LEV;
                dev_db = (BT_SINK_SRV_A2DP_MAGIC_CODE | ctx->vol_lev);
                bt_sink_srv_cm_set_profile_data(&(a2dp_dev->dev_addr), BT_SINK_SRV_PROFILE_A2DP_SINK, &dev_db);
            }
        }
        bt_sink_srv_ami_audio_set_volume(ctx->a2dp_aid, ctx->vol_lev, STREAM_OUT);

        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_PLAY);
        med_hd = &(a2dp_dev->med_handle);
    #ifdef __BT_AWS_A2DP_SUPPORT__
        aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
        med_hd->set_aws_flag(ctx->a2dp_aid, true);
        med_hd->aws_plh_init(ctx->a2dp_aid);
        /* Sync AWS SRC volume setting */
        if (aws_dev) {
            bt_sink_srv_aws_a2dp_sync_vol_level(aws_dev->aws_hd, ctx->vol_lev);
        }

        if (aws_dev && aws_dev->gap_role == BT_ROLE_MASTER) {
            bt_codec_a2dp_aws_set_clock_skew(false);
        } else if (aws_dev && aws_dev->gap_role == BT_ROLE_SLAVE) {
            bt_codec_a2dp_aws_set_clock_skew(true);
        } else {
            bt_codec_a2dp_aws_set_clock_skew(false);
        }

        gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_run_count_begin);
        ret = med_hd->play(ctx->a2dp_aid);
        gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_run_count_end);
        cost_dur = (gpt_run_count_end - gpt_run_count_begin) * 1000 / 32768;
        bt_sink_srv_report("[sink][a2dp]drv_play(aws)--ret: %d, cost: %d, gpt_ret: %d", ret, cost_dur, gpt_ret);
        if (BT_CODEC_MEDIA_STATUS_OK == ret) {
            BT_SINK_SRV_SET_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_OP_DRV_PLAY);
        } else {
            /* Error handle */
        }
    #else
        gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_run_count_begin);
        ret = med_hd->play(ctx->a2dp_aid);
        gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &gpt_run_count_end);
        cost_dur = (gpt_run_count_end - gpt_run_count_begin) * 1000 / 32768;
        bt_sink_srv_report("[sink][a2dp]drv_play--ret: %d, cost: %d, gpt_ret: %d", ret, cost_dur, gpt_ret);
        if (BT_CODEC_MEDIA_STATUS_OK == ret) {
            BT_SINK_SRV_SET_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_OP_DRV_PLAY);
            BT_SINK_SRV_SET_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_PLAYING);
            bt_sink_srv_state_set(BT_SINK_SRV_STATE_STREAMING);
            bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_PLAYING, NULL);
        } else {
            /* Error handle */
            bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL, NULL);
        }
    #endif /* __BT_AWS_A2DP_SUPPORT__ */
    }
}


void bt_sink_srv_a2dp_drv_stop(void *param)
{
    bt_sink_srv_music_device_t *a2dp_dev = (bt_sink_srv_music_device_t *)param;
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    int32_t ret = 0;

    ctx = bt_sink_srv_music_get_context();

    if (ctx->run_dev == a2dp_dev) {
        med_hd = &(a2dp_dev->med_handle);
        #ifdef __BT_AWS_A2DP_SUPPORT__
        med_hd->set_aws_flag(ctx->a2dp_aid, false);
        med_hd->aws_plh_init(ctx->a2dp_aid);
        #endif
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP);
        ret = med_hd->stop(ctx->a2dp_aid);
        bt_sink_srv_report("[sink][a2dp]drv_stop-ret: 0x%x", ret);
        if (BT_CODEC_MEDIA_STATUS_OK == ret) {
            /* Remove DRV play flag */
            BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_OP_DRV_PLAY);
            /* Remove play flag */
            BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_PLAY);
            BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_PLAYING);
            //bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
        } else {
            /* Error handle */
            bt_sink_srv_report("[sink][a2dp]drv_stop(error)--ret: %d", ret);
        }
    }
}


int32_t bt_sink_srv_a2dp_change_volume(uint8_t type)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    bt_sink_srv_am_id_t ami_ret = 0;
    uint8_t vol = AUD_VOL_OUT_LEVEL0;
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
    uint8_t dev_db = 0;
#ifdef __BT_AWS_A2DP_SUPPORT__
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;
#endif


    ctx = bt_sink_srv_music_get_context();

    vol = ctx->vol_lev;
    /* volume up */
    if (VOLUME_UP == type) {
       if (vol < BT_SINK_SRV_A2DP_MAX_VOL_LEV) {
           vol = vol + 1;
       } else {
           ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
       }
    } else if (VOLUME_DOWN == type) {
        if (vol > BT_SINK_SRV_A2DP_MIN_VOL_LEV) {
            vol = vol - 1;
        } else {
            ret = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
        }
    }

    if (vol != ctx->vol_lev) {
        ami_ret = bt_sink_srv_ami_audio_set_volume(ctx->a2dp_aid, vol, STREAM_OUT);
        ctx->vol_lev = vol;
        dev_db = (BT_SINK_SRV_A2DP_MAGIC_CODE | ctx->vol_lev);
        a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
        bt_sink_srv_cm_set_profile_data(&(a2dp_dev->dev_addr), BT_SINK_SRV_PROFILE_A2DP_SINK, &dev_db);

        bt_sink_srv_avrcp_volume_notification(a2dp_dev->avrcp_hd, ctx->vol_lev);
    #ifdef __BT_AWS_A2DP_SUPPORT__
        aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
        if (aws_a2dp_dev) {
            bt_sink_srv_aws_a2dp_sync_vol_level(aws_a2dp_dev->aws_hd, ctx->vol_lev);
        }
    #endif
    }

    bt_sink_srv_report("[sink][a2dp]change_volume-ami_ret: %d, ret: %d, vol: %d",
        ami_ret, ret, ctx->vol_lev);

    return ret;
}


void bt_sink_srv_a2dp_play(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
    bt_sink_srv_am_audio_capability_t aud_cap;
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_am_result_t am_ret;
    int32_t ret = 0;
    #ifdef __BT_AWS_A2DP_SUPPORT__
    bt_sink_srv_music_device_t *aws_dev = NULL;
    #endif

    a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_PSE_HD, (void *)handle);
    bt_sink_srv_assert(a2dp_dev);

    bt_sink_srv_report("[sink][a2dp]play(s)--hd: 0x%08x, type: %d, flag: 0x%08x, op: 0x%08x",
        handle, handle->type, a2dp_dev->flag, a2dp_dev->op);

    #ifdef __BT_SINK_SRV_VOICE_PROMPT__
    #ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    if (bt_timer_ext_find(BT_SINK_SRV_A2DP_WAIT_VPA_TIMER_ID)) {
        bt_timer_ext_stop(BT_SINK_SRV_A2DP_WAIT_VPA_TIMER_ID);
    }
    #endif
    if (a2dp_dev->op & BT_SINK_SRV_MUSIC_WAIT_VAP_RESUME) {
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_WAIT_VAP_RESUME);
        a2dp_dev->substate = BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC;
        bt_sink_srv_report("[sink][a2dp]play(s)--VPA resume");
    }
    #endif

    if (a2dp_dev->op & BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT) {
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT);
        BT_SINK_SRV_SET_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_AVRCP_PLAY_TRIGGER);
        a2dp_dev->substate = BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC;
        bt_sink_srv_report("[sink][a2dp]play(s)--HF int resume");
    }

    if (a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT) {
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT);
        bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_RESUME, NULL);
        ret = bt_sink_srv_avrcp_play_music(a2dp_dev->avrcp_hd);
        if (ret != BT_STATUS_SUCCESS) {
            bt_sink_srv_report("[sink][a2dp]play(error)--ret: %d", ret);
        }
        bt_sink_srv_report("[sink][a2dp]play(int)--ret: %d", ret);
        return ;
    }

    if (a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER) {
        /* Update run device */
        bt_sink_srv_music_update_run_device(a2dp_dev);
        bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_CODEC_OPEN, NULL);
        bt_sink_srv_report("[sink][a2dp]play(recover)--flag: 0x%x", a2dp_dev->flag);
        return ;
    }

    /* Audio source accept play request */
    BT_SINK_SRV_SET_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC);
    /* Update run device */
    bt_sink_srv_music_update_run_device(a2dp_dev);
    /* 1. Open A2DP codec */
    bt_sink_srv_a2dp_fill_am_aud_param(&aud_cap, &a2dp_dev->codec, BT_A2DP_SINK);

    am_ret = bt_sink_srv_ami_audio_play(ctx->a2dp_aid, &aud_cap);
    if (AUD_EXECUTION_SUCCESS == am_ret) {
        #ifdef __BT_AWS_A2DP_SUPPORT__
        /* Check AWS connect */
        aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
        if ((aws_dev) &&
            (aws_dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT)) {
            /* Transfer audio cap */
            //BT_SINK_SRV_SET_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
        }
        #endif
    } else {
        /* Exception: play fail */
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC);
        bt_sink_srv_music_update_run_device(NULL);
        bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL, NULL);
    }

    BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_AVRCP_PLAY_TRIGGER);
}


void bt_sink_srv_a2dp_stop(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
    bt_sink_srv_music_device_t *ori_dev = NULL, *cur_dev = NULL;
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();


    a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_PSE_HD, (void *)handle);

    bt_sink_srv_assert(a2dp_dev);

    ori_dev = ctx->run_dev;

    bt_sink_srv_report("[sink][a2dp]stop(s)--hd: 0x%x, type: %d, flag: 0x%x, op: 0x%x",
        handle, handle->type, a2dp_dev->flag, a2dp_dev->op);

    /* Clear codec */
    bt_sink_srv_a2dp_clear_codec(a2dp_dev);

    /* Clear done */
    bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_CODEC_CLEAR, NULL);

    if (!(a2dp_dev->conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT)) {
        bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
        bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_UNAVAILABLE, NULL);
        /* Deinit pse handle */
        bt_sink_srv_a2dp_free_pseudo_handle(a2dp_dev->handle);
        a2dp_dev->handle = NULL;
        if (!(a2dp_dev->conn_bit)) {
            bt_sink_srv_music_reset_device(a2dp_dev);
        }
    } else {
        bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
        bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_READY, NULL);
    }

    cur_dev = ctx->run_dev;

    if (ori_dev == cur_dev) {
        bt_sink_srv_music_update_run_device(NULL);
    }
}


void bt_sink_srv_a2dp_suspend(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd)
{
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
    bt_sink_srv_music_device_t *ori_dev = NULL, *cur_dev = NULL;
    int32_t ret = 0, err = 0;
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();


    a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_PSE_HD, (void *)handle);

    bt_sink_srv_assert(a2dp_dev);

    ori_dev = ctx->run_dev;

    bt_sink_srv_report("[sink][a2dp]suspend(s)--hd: 0x%x, type: %d, int: 0x%x, type: %d, flag: 0x%x, op: 0x%x",
        handle, handle->type, int_hd, int_hd->type, a2dp_dev->flag, a2dp_dev->op);

    /* Clear codec */
    bt_sink_srv_a2dp_clear_codec(a2dp_dev);

    /* Clear done */
    bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_CODEC_CLEAR, NULL);

    if ((handle->dev_id == int_hd->dev_id) &&
        (int_hd->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_HFP)) {
        if (a2dp_dev->avrcp_status == 0x01) {
            /* WSAP00041710 Nexus 5, HF don't interrupt A2DP */
            /* Add self in waiting list */
            audio_src_srv_add_waiting_list(handle);
            /* Set interrupt flag */
            BT_SINK_SRV_SET_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT);
            err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
        } else {
            /* The same device & HF interrupt */
            err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_1ST;
        }
    } else if ((int_hd->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP)){
        /* PartyMode interrupt */
        ret = bt_sink_srv_avrcp_stop_music(a2dp_dev->avrcp_hd);
        if (ret != BT_STATUS_SUCCESS) {
            /* Pause SP failed */
            bt_sink_srv_report("[sink][a2dp]suspend(err)--ret: %d", ret);
        }
        err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_2ND;
    } else {
        /* Add self in waiting list */
        audio_src_srv_add_waiting_list(handle);
        /* Set interrupt flag */
        BT_SINK_SRV_SET_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT);
        /* Send pause cmd */
        ret = bt_sink_srv_avrcp_stop_music(a2dp_dev->avrcp_hd);
        if (ret != BT_STATUS_SUCCESS) {
            /* Pause SP failed */
            bt_sink_srv_report("[sink][a2dp]suspend(error)--ret: %d", ret);
        }
        err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_3RD;
    }

    cur_dev = ctx->run_dev;

    if (ori_dev == cur_dev) {
        bt_sink_srv_music_update_run_device(NULL);
    }

    bt_sink_srv_report("[sink][a2dp]suspend(e)--ret: %d, err: %d", ret, err);
}


void bt_sink_srv_a2dp_reject(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_music_device_t *dev = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_PSE_HD, (void *)handle);

    bt_sink_srv_assert(dev);

    bt_sink_srv_report("[sink][a2dp]reject(s)--hd: 0x%x, type: %d, flag: 0x%x, op: 0x%x",
        handle, handle->type, dev->flag, dev->op);
#ifdef __BT_SINK_SRV_AVRCP_STATUS_SUPPORT__
    if (dev->op & BT_SINK_SRV_MUSIC_AVRCP_PLAY_TRIGGER) {
        /* Ignore */
    } else {
        /* Reject handle case 1. accept SP start streaming; 2. pause SP music */
        bt_a2dp_start_streaming_response(dev->a2dp_hd, true);
    }
    bt_sink_srv_avrcp_stop_music(dev->avrcp_hd);
#else
    /* Reject handle case 1. accept SP start streaming; 2. pause SP music */
    bt_a2dp_start_streaming_response(dev->a2dp_hd, true);

    bt_sink_srv_avrcp_stop_music(dev->avrcp_hd);
#endif
    /* Notify state machine reject reason */
    bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_REJECT, NULL);
}


void bt_sink_srv_a2dp_exception(audio_src_srv_handle_t *handle, int32_t event, void *param)
{
}


void bt_sink_srv_a2dp_create_pse_handle(void)
{
    int32_t i = 0;

    for (i = 0; i < BT_SINK_SRV_A2DP_PSEUDO_COUNT; ++i) {
        g_a2dp_pse_hd[i].hd = audio_src_srv_construct_handle(AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP);
    }
}


void bt_sink_srv_a2dp_destroy_pse_handle(void)
{
    int32_t i = 0;

    for (i = 0; i < BT_SINK_SRV_A2DP_PSEUDO_COUNT; ++i) {
        audio_src_srv_destruct_handle(g_a2dp_pse_hd[i].hd);
    }
}


int32_t bt_sink_srv_a2dp_set_volume(uint8_t volume)
{
    uint8_t dev_db = 0;    
    uint8_t new_vol = AUD_VOL_OUT_LEVEL0; 
    uint8_t old_vol = AUD_VOL_OUT_LEVEL0;
    bt_sink_srv_am_id_t ami_ret = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    bt_sink_srv_music_context_t *cntx = NULL;
#ifdef __BT_AWS_A2DP_SUPPORT__
    bt_sink_srv_music_device_t *aws_dev = NULL;
#endif

    new_vol = bt_sink_srv_avrcp_get_volume_level(volume);
    cntx = bt_sink_srv_music_get_context();
    old_vol = cntx->vol_lev;
    bt_sink_srv_report("[sink][a2dp]set_volume[s]-new_vol: %d, old_vol: %d", new_vol, old_vol);
    
    /* volume change */
    if (old_vol != new_vol) {
        ami_ret = bt_sink_srv_ami_audio_set_volume(cntx->a2dp_aid, new_vol, STREAM_OUT);
        cntx->vol_lev = new_vol;
        dev_db = (BT_SINK_SRV_A2DP_MAGIC_CODE | cntx->vol_lev);
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
        //dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
        ret = bt_sink_srv_cm_set_profile_data(&(dev->dev_addr), BT_SINK_SRV_PROFILE_A2DP_SINK, &dev_db);
#ifdef __BT_AWS_A2DP_SUPPORT__
        aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
        if (aws_dev) {
            bt_sink_srv_aws_a2dp_sync_vol_level(aws_dev->aws_hd, cntx->vol_lev);
        }
#endif
    }

    bt_sink_srv_report("[sink][a2dp]set_volume-ami_ret: %d, ret: %d, vol: %d",
        ami_ret, ret, cntx->vol_lev);
    return ret;
}


#ifdef __BT_AWS_A2DP_SUPPORT__
static void bt_sink_srv_a2dp_gpt_cb(void *user_data)
{
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_music_device_t *a2dp_dev = (bt_sink_srv_music_device_t *)user_data;
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;
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

    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);

    play_clock = ctx->play_time.nclk * 312 + (ctx->play_time.nclk >> 1) + ctx->play_time.nclk_intra;

    if (ctx->run_dev == a2dp_dev) {
        med_hd = &(a2dp_dev->med_handle);
        if (!(a2dp_dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY)) {
            /* GPT interrupt (re-entry) error handle */
            bt_sink_srv_report("[sink][a2dp]gpt_cb(error)--flag: 0x%0x, op: 0x%08x, size: %d, count: %d, bt_ac: %d, garbage_ac: %d, decoder_ac: %d, skip: %d, odd_byte: %d",
                a2dp_dev->flag, a2dp_dev->op,
                med_ctx->size, med_ctx->item_count, med_ctx->bt_ac_index, med_ctx->garbage_ac_index, med_ctx->decoder_ac_index, med_ctx->skip, med_ctx->odd_byte);
            BT_SINK_SRV_SET_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
            return;
        }

        if (!med_hd->set_aws_initial_sync) {
            bt_sink_srv_report("[sink][a2dp]gpt_cb(error)--empty func");
            return;
        }
        BT_SINK_SRV_SET_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_PSEDEV_PLAYING);
        __disable_irq();
        while(1)
        {
            if (aws_a2dp_dev) {
                bt_aws_get_curr_bt_time(aws_a2dp_dev->gap_role, ctx->clk_offset, &cur_bt_clk);
            } else {
                bt_aws_get_bt_local_time(&cur_bt_clk);
            }
            curr_clock = cur_bt_clk.nclk * 312 + (cur_bt_clk.nclk >> 1) + cur_bt_clk.nclk_intra;
            if (curr_clock >= play_clock)
            {
                break;
            }
        }
        ret = med_hd->set_aws_initial_sync(ctx->a2dp_aid);
        __enable_irq();
        bt_sink_srv_audio_clock_init(NULL);
        bt_sink_srv_report("[sink][a2dp]gpt_cb-start play--ret: %d", ret);
    } else {
        err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

    bt_sink_srv_report("[sink][a2dp]gpt_cb[e]-ret: %d, err: %d", ret, err);
}


static int32_t bt_sink_srv_a2dp_suspend_request(uint32_t handle, bt_sink_srv_music_device_t *dev)
{
    int32_t ret = 0;
    uint8_t buf[256] = {0};
    bt_sink_srv_aws_cmd_t *req = NULL;

    req = (bt_sink_srv_aws_cmd_t *)buf;
    req->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_STOP;
    req->type = BT_SINK_SRV_AWS_COMMAND_TYPE_REQ;
    req->accept = false;
    req->data_len = 1;
    req->data[0] = 0xfc;
    ret = bt_sink_srv_aws_send_cmd(handle, BT_SINK_SRV_AWS_MODULE_ID_A2DP, req);
    bt_sink_srv_report("[sink][a2dp]suspend req-ret: %d", ret);

    return ret;
}


static void bt_sink_srv_a2dp_get_bt_clk_offset(uint32_t evt_id, void *param, void *user_data)
{
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_aws_offset_update_ind_t *offset_ind = (bt_aws_offset_update_ind_t *)param;
    bt_aws_clock_t bt_clock = {0};
    bt_sink_srv_music_device_t *a2dp_dev = (bt_sink_srv_music_device_t *)user_data;
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;
    int32_t ret = 0;
    bt_hci_packet_t *pkt = NULL;
    bt_aws_play_notify_t play_noti = {0};
    uint32_t time_dur = 0;


    bt_sink_srv_report("[sink][a2dp]clk_offset(s)-offset: 0x%x, intra: 0x%x",
        offset_ind->offset.offset, offset_ind->offset.offset_intra);

    if ((a2dp_dev) && (a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET)) {
        aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);

        ctx = bt_sink_srv_music_get_context();
        ctx->clk_offset.offset = offset_ind->offset.offset;
        ctx->clk_offset.offset_intra = offset_ind->offset.offset_intra;


        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET);

        if (a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_AUTO_SYNC) {
            /* Case 4. Dynamic plug in */
            bt_sink_srv_music_enable_transfer_packet();
            ctx->first_trans = true;
        } else {
            ret = bt_aws_get_curr_bt_time(aws_a2dp_dev->gap_role, ctx->clk_offset, &bt_clock);
            /* Save current BT clock */
            ctx->piconet_info.clock.nclk = bt_clock.nclk;
            ctx->piconet_info.clock.nclk_intra = bt_clock.nclk_intra;

            time_dur = BT_SINK_SRV_A2DP_AWS_TIMER_DUR;
            ctx->piconet_info.clock.nclk_intra = bt_clock.nclk_intra + time_dur;
            /* Case 3. Connection first, then play */
            pkt = (bt_hci_packet_t *)bt_sink_srv_media_mgr_get_first_item();
            ctx->piconet_info.pkt_info.sn = sink_srv_util_get_a2dp_sn(pkt);
            ctx->piconet_info.pkt_info.ts = sink_srv_util_get_a2dp_time_stamp(pkt);


            /* Save local clock */
            ret = bt_aws_get_bt_local_time(&(ctx->local_info.clock));
            ctx->local_info.clock.nclk_intra += time_dur;
            pkt = (bt_hci_packet_t *)bt_sink_srv_media_mgr_get_first_item();
            ctx->local_info.pkt_info.sn = sink_srv_util_get_a2dp_sn(pkt);
            ctx->local_info.pkt_info.ts = sink_srv_util_get_a2dp_time_stamp(pkt);
            ctx->local_info.ts_diff_by_packet = ctx->med_play_info.ts_diff_by_packet;
            bt_sink_srv_report("[sink][a2dp]clk_offset(local)-nclk: 0x%08x, intra: 0x%08x, sn: 0x%x, ts: 0x%x, ret: %d",
                ctx->local_info.clock.nclk, ctx->local_info.clock.nclk_intra,
                ctx->local_info.pkt_info.sn, ctx->local_info.pkt_info.ts, ret);


            play_noti.play_time = time_dur;
            play_noti.sequence_num = time_dur;
            play_noti.play_bt_clock.nclk = ctx->piconet_info.clock.nclk;
            play_noti.play_bt_clock.nclk_intra = ctx->piconet_info.clock.nclk_intra;
            play_noti.data_len = sizeof(bt_sink_srv_music_aws_sync_info);
            /* Add ts diff */
            ctx->piconet_info.ts_diff_by_packet = ctx->med_play_info.ts_diff_by_packet;
            play_noti.user_data = (void *)(&(ctx->piconet_info));
            play_noti.bt_local_role = aws_a2dp_dev->gap_role;
            bt_sink_srv_report("[sink][a2dp]clk_offset-nclk: 0x%x, intra: 0x%x, sn: 0x%x, ts: 0x%x, role: %d",
                play_noti.play_bt_clock.nclk, play_noti.play_bt_clock.nclk_intra,
                ctx->piconet_info.pkt_info.sn, ctx->piconet_info.pkt_info.ts, aws_a2dp_dev->gap_role);
            ret = bt_aws_notify_play(aws_a2dp_dev->aws_hd, BT_SINK_SRV_AWS_MODULE_ID_A2DP, &play_noti);
            /* Start gpt timer */
            ctx->play_time.nclk = ctx->piconet_info.clock.nclk;
            ctx->play_time.nclk_intra = ctx->piconet_info.clock.nclk_intra;
            ctx->gpt_port = (hal_gpt_port_t)bt_sink_srv_music_start_gpt_timer(bt_sink_srv_a2dp_gpt_cb, (void *)a2dp_dev, time_dur);
        }
    }
}


/* Case 4. Update base bt clock */
void bt_sink_srv_a2dp_update_base_bt_clock(void)
{
    bt_sink_srv_music_device_t *sp_dev = NULL, *aws_a2dp_dev = NULL;
    int32_t ret = 0;

    sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);

    /* Get aws bt clock offset */
    BT_SINK_SRV_SET_FLAG(sp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET);
    ret = bt_sink_srv_music_get_bt_clk_offset(aws_a2dp_dev->aws_hd, bt_sink_srv_a2dp_get_bt_clk_offset, sp_dev);
    if (ret != BT_STATUS_SUCCESS) {
        BT_SINK_SRV_REMOVE_FLAG(sp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET);
        bt_sink_srv_report("[sink][a2dp]update_base_bt_clock(error)");
    }

    bt_sink_srv_report("[sink][a2dp]update_base_bt_clock--sp_flag: 0x%08x, sp_op: 0x%08x, aws_flag: 0x%08, aws_op: 0x%08x\n",
        sp_dev->flag, sp_dev->op, aws_a2dp_dev->flag, aws_a2dp_dev->op);
}


static void bt_sink_srv_a2dp_handle_trans_packet(bt_a2dp_streaming_received_ind_t *data_ind)
{
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_music_device_t *run_dev = NULL;
    int32_t ret = 0;
    bt_aws_streaming_packet_t aws_pkt = {0};
    bt_sink_srv_music_device_t *aws_a2dp_dev = NULL;
    bt_aws_play_notify_t play_noti = {0};
    uint32_t time_dur = 0;
    bt_hci_packet_t *trans_pkt = NULL;
    bt_aws_clock_t cur_local_clock = {0}, cur_bt_clk = {0};
    sink_srv_clock_mapping_t base_clk = {{0}};
    uint32_t time_stamp = 0;

    ctx = bt_sink_srv_music_get_context();
    run_dev = ctx->run_dev;

    bt_sink_srv_music_disable_log();
    aws_a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
    bt_sink_srv_music_enable_log();

    if ((ctx->trans) &&
        (aws_a2dp_dev) &&
        (run_dev)) {

        if (ctx->first_trans) {
            ctx->first_trans = false;
            bt_sink_srv_report("[sink][a2dp]trans_packet(start_trans)--ret: %d", ret);
        }

        if ((run_dev) &&
            (run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_AUTO_SYNC)) {
            /* Calc lcoal bt clock */
            trans_pkt = (bt_hci_packet_t *)(data_ind->data_node);
            ret = bt_aws_get_bt_local_time(&cur_local_clock);
            bt_sink_srv_report("[sink][a2dp]trans_packet(auto_sync)cur_local_clk--nclk: 0x%08x, intra: 0x%08x, ret: 0x%x\n",
                cur_local_clock.nclk, cur_local_clock.nclk_intra, ret);
            /* Base base local bt clock */
            base_clk.piconet_clk.nclk = ctx->local_info.clock.nclk;
            base_clk.piconet_clk.nclk_intra = ctx->local_info.clock.nclk_intra;
            base_clk.time_stamp = ctx->local_info.pkt_info.ts;
            base_clk.seq_idx = ctx->local_info.pkt_info.sn;

            /* Get next packet time dur */
            time_stamp = sink_srv_util_get_a2dp_time_stamp(trans_pkt);            
            ctx->med_play_info.sequence_idx = sink_srv_util_get_a2dp_sn(trans_pkt);
            time_dur = sink_srv_get_play_duration(&(ctx->med_play_info),
                    &base_clk, &cur_local_clock, time_stamp);

            bt_sink_srv_report("[sink][a2dp]trans_packet(auto_sync)--time_dur: 0x%08x\n", time_dur);
            if (time_dur >= BT_SINK_SRV_MUSIC_NEXT_DUR_MIN && time_dur <= BT_SINK_SRV_MUSIC_NEXT_DUR_MAX) {
                ;
            } else {
                //bt_sink_srv_assert(0 && "[sink][a2dp]trans_packet(invalid dur)");
                /* WSAP00038053 */
                bt_sink_srv_report("[sink][a2dp]trans_packet(invalid dur ASSERT)--time_dur: 0x%08x\n", time_dur);
                time_dur = BT_SINK_SRV_A2DP_AWS_TIMER_DUR;
            }
            /* Update base local bt clock */
            ctx->local_info.clock.nclk = cur_local_clock.nclk;
            ctx->local_info.clock.nclk_intra = cur_local_clock.nclk_intra + time_dur;
            ctx->local_info.pkt_info.ts = sink_srv_util_get_a2dp_time_stamp(trans_pkt);
            ctx->local_info.pkt_info.sn = sink_srv_util_get_a2dp_sn(trans_pkt);

            /* Calc bt clock */
            ret = bt_aws_get_bt_time_by_local_time(aws_a2dp_dev->gap_role, ctx->clk_offset, cur_local_clock,  &cur_bt_clk);
            bt_sink_srv_report("[sink][a2dp]trans_packet(auto_sync)trans_bt_clk--nclk: 0x%08x, intra: 0x%08x, ret: 0x%x\n",
                cur_bt_clk.nclk, cur_bt_clk.nclk_intra, ret);

            /* Update base bt clock */
            ctx->piconet_info.clock.nclk = cur_bt_clk.nclk;
            ctx->piconet_info.clock.nclk_intra = cur_bt_clk.nclk_intra + time_dur;
            ctx->piconet_info.pkt_info.ts = sink_srv_util_get_a2dp_time_stamp(trans_pkt);
            ctx->piconet_info.pkt_info.sn = sink_srv_util_get_a2dp_sn(trans_pkt);

            /* Notify play ind */
            play_noti.play_time = time_dur;
            play_noti.sequence_num = time_dur;
            play_noti.play_bt_clock.nclk = ctx->piconet_info.clock.nclk;
            play_noti.play_bt_clock.nclk_intra = ctx->piconet_info.clock.nclk_intra;
            play_noti.data_len = sizeof(bt_sink_srv_music_aws_sync_info);
            /* Add ts diff */
            ctx->piconet_info.ts_diff_by_packet = ctx->med_play_info.ts_diff_by_packet;
            play_noti.user_data = (void *)(&(ctx->piconet_info));
            play_noti.bt_local_role = aws_a2dp_dev->gap_role;
            ret = bt_aws_notify_play(aws_a2dp_dev->aws_hd, BT_SINK_SRV_AWS_MODULE_ID_A2DP, &play_noti);
            if (ret != BT_STATUS_SUCCESS) {
                bt_sink_srv_report("[sink][a2dp]trans_packet(error_noti_play)--ret: %d", ret);
            }

            bt_sink_srv_report("[sink][a2dp]trans_packet(noti_play)--sn: 0x%x, ts: 0x%x, nclk: 0x%x, intra: 0x%x, role: %d, dur: 0x%x",
                ctx->piconet_info.pkt_info.sn, ctx->piconet_info.pkt_info.ts, ctx->piconet_info.clock.nclk, ctx->piconet_info.clock.nclk_intra,
                play_noti.bt_local_role, time_dur);
            /* Sync SPK1 volume level */
            bt_sink_srv_aws_a2dp_sync_vol_level(aws_a2dp_dev->aws_hd, ctx->vol_lev);
            BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_AWS_AUTO_SYNC);
        }
        aws_pkt.type = BT_AWS_STREAMING_TYPE_A2DP;
        aws_pkt.a2dp_packet.data_node = data_ind->data_node;
        aws_pkt.a2dp_packet.total_length = data_ind->total_length;
        aws_pkt.a2dp_packet.media_offset = data_ind->media_offset;
        ret = bt_aws_send_streaming_packet(aws_a2dp_dev->aws_hd, BT_SINK_SRV_AWS_MODULE_ID_A2DP, &aws_pkt);

        if (ret) {
            bt_sink_srv_report("[sink][a2dp]trans_packet(error_trans)--ret: %d", ret);
        }
    }
}


int32_t bt_sink_srv_aws_a2dp_suspend_request(uint32_t handle, bt_sink_srv_music_device_t *dev)
{
    return bt_sink_srv_a2dp_suspend_request(handle, dev);
}

#endif


#ifdef __BT_SINK_SRV_VOICE_PROMPT__
static void bt_sink_srv_a2dp_wait_vpa_timeout(uint32_t timer_id, uint32_t data)
{
    bt_sink_srv_music_device_t *dev = (bt_sink_srv_music_device_t *)data;

    bt_sink_srv_report("[sink][a2dp]wait_vpa_timeout--dev: 0x%08x, flag: 0x%08x, op: 0x%08x", dev, dev->flag, dev->op);

    BT_SINK_SRV_REMOVE_FLAG(dev->op, BT_SINK_SRV_MUSIC_WAIT_VAP_RESUME);

    bt_a2dp_start_streaming_response(dev->a2dp_hd, true);
    bt_sink_srv_avrcp_stop_music(dev->avrcp_hd);
}
#endif /* __BT_SINK_SRV_VOICE_PROMPT__ */

