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
#include "bt_sink_srv_am_med_mgr.h"
#ifdef __BT_AWS_SUPPORT__
#include "bt_aws.h"
#include "bt_sink_srv_audio_sync.h"
#include "bt_sink_srv_aws.h"
#endif

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

bt_a2dp_codec_capability_t g_bt_sink_srv_a2dp_codec_list[BT_SINK_SRV_A2DP_CODEC_MAX_NUM];

SinkProfileA2dp sink_a2dp[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];

static uint32_t bt_sink_srv_a2dp_data_rece_cnt = 0;

bt_a2dp_streaming_received_ind_t *bt_sink_srv_a2dp_rece_ind = NULL;
bt_hci_packet_t *bt_sink_srv_a2dp_pkt = NULL;


const static uint8_t BT_SINK_SRV_A2DP_AAC_SYNC_HEAD[BT_SINK_SRV_AAC_ADTS_LENGTH] = {0xFF, 0xF9, 0x40, 0x20, 0x00, 0x1F, 0xFC};

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

#ifndef MTK_DEBUG_LEVEL_INFO
int32_t g_test_fix_warning = 0;
#endif
/* static function declare */
////////////////////////////////////////////////////////////////////////////
// Add timer for reject a2dp streaimg.
// flow: accept start ind --> send avrcp pause to A2DP, but not open codec -->accpet A2DP pause.
static TimerHandle_t bt_stop_music_tmr = NULL;
static int32_t bt_sink_srv_a2dp_pause_state = SINK_MUSIC_STATE_PAUSE_NONE;
void bt_sink_srv_set_music_pause_state(int32_t state)
{
    bt_sink_srv_report("[sink][a2dp]now_state:%d, prev_state:%d", state, bt_sink_srv_a2dp_pause_state);
    bt_sink_srv_a2dp_pause_state = state;
}

int32_t bt_sink_srv_get_music_pause_state(void)
{
    bt_sink_srv_report("[sink][a2dp]now_state:%d", bt_sink_srv_a2dp_pause_state);
    return bt_sink_srv_a2dp_pause_state;
}

static void bt_sink_srv_a2dp_stop_music_cb(TimerHandle_t time_hdl)
{
    uint32_t user_handle = ((uint32_t)time_hdl ^ (uint32_t)bt_stop_music_tmr);
    bt_status_t ret = bt_sink_srv_avrcp_stop_music(user_handle);
    bt_sink_srv_report("[sink][a2dp]stop music timeout, timer:0x%08X, usr_hdl:0x%08X, ret:0x%08X",
                                                                        time_hdl, user_handle, ret);
    xTimerStop(time_hdl, 0);
    xTimerDelete(time_hdl, 0);
    bt_stop_music_tmr = NULL;
}

uint32_t bt_sink_srv_a2dp_stop_music(uint32_t user_handle, uint32_t duration_ms)
{
    if (bt_stop_music_tmr) {
        bt_sink_srv_report("[sink][a2dp]stop timer exist, timer:0x%08X, avrcp_hd:0x%08X", bt_stop_music_tmr, user_handle);
        return BT_STATUS_FAIL;
    }
    bt_stop_music_tmr = xTimerCreate("bt_stop_music", /* Text name to facilitate debugging.  The kernel does not use this itself. */
            0xffff, /* The period for the timer. */
            pdFALSE, /* Don't auto-reload - hence a one shot timer. */
            NULL, /* The timer identifier.  In this case this is not used as the timer has its own callback. */
            bt_sink_srv_a2dp_stop_music_cb);        /* The callback to be called when the timer expires. */

    xTimerChangePeriod(bt_stop_music_tmr, duration_ms/portTICK_PERIOD_MS, 0xffff);
    xTimerReset(bt_stop_music_tmr, 0);
    bt_sink_srv_report("[sink][a2dp]stop music begin, timer:0x%08X, avrcp_hd:0x%08X", bt_stop_music_tmr, user_handle);
    bt_stop_music_tmr = (TimerHandle_t)((uint32_t)bt_stop_music_tmr ^ user_handle);
    return BT_STATUS_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
#ifdef __BT_AWS_SUPPORT__
static void bt_sink_srv_a2dp_gpt_cb(void *user_data);
static void bt_sink_srv_a2dp_start_aws_resync(void);
#endif

int32_t bt_sink_srv_a2dp_change_volume(uint8_t type);

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
        if (g_a2dp_codec_aac_support == true) {
            BT_A2DP_MAKE_AAC_CODEC(g_bt_sink_srv_a2dp_codec_list + num, BT_A2DP_SINK,
                                   aac_cap[0].vbr, aac_cap[0].object_type, aac_cap[0].channels,
                                   aac_cap[0].sample_rate, aac_cap[0].bit_rate);
            num++;
        }
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

static void bt_sink_srv_fill_audio_param(bt_sink_srv_am_audio_capability_t  *aud_cap, 
                                bt_a2dp_codec_capability_t *a2dp_cap, bt_a2dp_role_t role)
{
    bt_sink_srv_music_context_t *cntx = NULL;
    cntx = bt_sink_srv_music_get_context();
    memset(aud_cap, 0x00, sizeof(bt_sink_srv_am_audio_capability_t));
    aud_cap->type = A2DP;
    aud_cap->codec.a2dp_format.a2dp_codec.role = role;
    memcpy(&(aud_cap->codec.a2dp_format.a2dp_codec.codec_cap), a2dp_cap, sizeof(bt_a2dp_codec_capability_t));
    aud_cap->audio_stream_out.audio_device = HAL_AUDIO_DEVICE_HEADSET;
    aud_cap->audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t)cntx->vol_lev;
    aud_cap->audio_stream_out.audio_mute = false;
}
void bt_sink_srv_a2dp_set_dev_flag(bt_sink_srv_music_device_t *dev, uint32_t flag)
{
    dev->flag |= flag;
}

void bt_sink_srv_a2dp_clear_dev_flag(bt_sink_srv_music_device_t *dev, uint32_t flag)
{
    dev->flag &= (~flag);
}

bool bt_sink_srv_a2dp_check_dev_flag(bt_sink_srv_music_device_t *dev, uint32_t flag)
{
    return ((dev->flag & flag)!=0? true:false);
}

uint32_t bt_sink_srv_stop_close_codec(void)
{
    bt_sink_srv_music_device_t *a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
    bt_sink_srv_music_context_t *cntx = bt_sink_srv_music_get_context();
    SinkProfileA2dp *a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(a2dp_dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
    uint32_t ret = 0;
    bt_sink_srv_report("[sink][a2dp] stop_close codec, flag:0x%08X", a2dp_dev->flag);
    if (a2dp_dev && (a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC)) {
        /* update a2dp dev flag */
        uint32_t flag = BT_SINK_SRV_MUSIC_FLAG_AVRCP_PLAY | BT_SINK_SRV_MUSIC_FLAG_A2DP_STREAMING
                        | BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC;
        bt_sink_srv_a2dp_clear_dev_flag(a2dp_dev, flag);
        bt_sink_srv_a2dp_set_dev_flag(a2dp_dev, BT_SINK_SRV_MUSIC_FLAG_AVRCP_PAUSE);
        /* set streaming state */
        bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
        ret = a2dp->med_hd.stop(cntx->aud_id);
        bt_sink_srv_report("[sink][a2dp] stop_codec, ret: 0x%08X", ret);
        ret = bt_sink_srv_ami_audio_stop(cntx->aud_id);
        bt_sink_srv_report("[sink][a2dp] stop_codec, ret: 0x%08X", ret);
        cntx->state = BT_SINK_SRV_MUSIC_READY;
    }
    return ret;
}
uint32_t bt_sink_srv_open_a2dp_codec(void)
{
    bt_sink_srv_am_audio_capability_t  aud_cap;
    bt_sink_srv_music_context_t *cntx = NULL;
    SinkProfileA2dp *a2dp = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    uint32_t ret = 0;
    cntx = bt_sink_srv_music_get_context();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&cntx->a2dp_hd));
    a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
    bt_sink_srv_report("[sink][a2dp] open_a2dp_codec, cntx_state: 0x%08X, dev_flag:0x%08X", cntx->state, dev->flag);

    cntx->state = BT_SINK_SRV_MUSIC_AMI_PLAY;
    /* fill aud param */
    bt_sink_srv_fill_audio_param(&aud_cap, &(a2dp->codec), BT_A2DP_SINK);
    bt_sink_srv_report("[sink][a2dp]state change play--type: %d, role: %d, type2: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d, 8: %d, 9: %d\r\n",
                       aud_cap.type,
                       aud_cap.codec.a2dp_format.a2dp_codec.role,
                       aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.type,
                       aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.sep_type,
                       aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.length,
                       aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.object_type,
                       aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.freq_h,
                       aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.reserved,
                       aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.channels,
                       aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.freq_l,
                       aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.br_h,
                       aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.vbr,
                       aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.br_m,
                       aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.br_l);
    ret = bt_sink_srv_ami_audio_play(cntx->aud_id, &aud_cap);
    if (ret == 0) {
        bt_sink_srv_a2dp_set_dev_flag(dev, BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC);
    }
    return ret;
}
static void bt_sink_srv_a2dp_init()
{
    uint32_t i;
    int8_t ori_aid = 0;
    bt_sink_srv_music_context_t *cntx = NULL;

    for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
        sink_a2dp[i].profile.uuid = BT_SINK_SRV_UUID_A2DP;
        sink_a2dp[i].profile.state = BT_SINK_SRV_PROFILE_STATE_DISCONNECTED;
    }

    cntx = bt_sink_srv_music_get_context();
    ori_aid = cntx->aud_id;
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = ori_aid;
    #endif
    if (cntx->aud_id != BT_SINK_SRV_INVALID_AID) {
#ifdef __BT_SINK_SRV_AM_SUPPORT__
        bt_sink_srv_ami_audio_close(cntx->aud_id);
#endif /* __BT_SINK_SRV_AM_SUPPORT__ */
        cntx->aud_id = BT_SINK_SRV_INVALID_AID;
    }
#ifdef __BT_SINK_SRV_AM_SUPPORT__
    cntx->aud_id = bt_sink_srv_ami_audio_open(AUD_MIDDLE, bt_sink_srv_a2dp_ami_hdr);
#endif /* __BT_SINK_SRV_AM_SUPPORT__ */
    bt_sink_srv_report("[sink][a2dp]init-aud_id: %d, ori: %d\r\n",
                       cntx->aud_id, ori_aid);
#if defined(__GNUC__)
    bt_sink_srv_report("[sink][a2dp]--Version: %s\n", "__GNUC__");
#endif

#if defined(__ARMCC_VERSION)
    bt_sink_srv_report("[sink][a2dp]--Version: %s\n", "__ARMCC_VERSION");
#endif

#if defined(__ICCARM__)
    bt_sink_srv_report("[sink][a2dp]--Version: %s\n", "__ICCARM__");
#endif
}


static void bt_sink_srv_a2dp_fill_aac_header(uint8_t *data, uint32_t len, bt_a2dp_codec_capability_t *codec)
{
    uint8_t freqIndex = 0;
    uint8_t channelNum = 2; // Assume mono
    uint8_t temp = 0;

    temp = codec->codec.aac.freq_h << 4 | (codec->codec.aac.freq_l & 0x0F);

    while (temp ^ 1) {
        freqIndex++;
        temp >>= 1;
    }

    temp = codec->codec.aac.channels;
    if (temp == 1) {
        channelNum = 2;
    } else if (temp == 2) {
        channelNum = 1;
    }

    memcpy(data, BT_SINK_SRV_A2DP_AAC_SYNC_HEAD, BT_SINK_SRV_AAC_ADTS_LENGTH);

    data[2] |= (freqIndex << 2);
    data[3] |= (channelNum << 6) | (len >> 11);
    data[4] = (len >> 3) & 0xFF;
    data[5] |= (len & 0x07) << 5;
}


static void bt_sink_srv_a2dp_fetch_media_header(uint8_t *data, bt_sink_srv_a2dp_media_header_t *header)
{
    uint8_t temp = 0;


    temp = data[0];
    header->version = temp >> 6;
    header->padding = (temp & 0x20) >> 5;
    header->extension = (temp & 0x10) >> 4;
    header->csrc_count = temp & 0x0F;

    temp = data[1];
    header->marker = temp >> 7;
    header->payload_type = temp & 0x7F;
    header->sequence_number = (data[2] << 8) | data[3];
}

static void *bt_sink_srv_a2dp_uppack_media_data(uint32_t handle, void *data, uint32_t data_len, uint32_t *payload_len)
{
    SinkProfileA2dp *a2dp = NULL;
    void *p_in = NULL;
    uint8_t *med = NULL;
    bt_sink_srv_music_device_t *dev = NULL;

    p_in = (uint8_t *)data + BT_SINK_SRV_MEDIA_PKT_HEADER_LEN;
    med = p_in;
    *payload_len = data_len - BT_SINK_SRV_MEDIA_PKT_HEADER_LEN;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&handle));
    a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
    // search the payload start pointer for AAC.
    if (a2dp->codec.type == BT_A2DP_CODEC_AAC) {
        uint8_t payload_offset = 9;
        uint8_t tempByte = 0;
        do {
            tempByte = ((uint8_t *)data)[BT_SINK_SRV_MEDIA_PKT_HEADER_LEN + payload_offset];
            payload_offset++;
        } while (tempByte == 0xFF);

        *payload_len -= payload_offset;
        med = (uint8_t *)data + BT_SINK_SRV_MEDIA_PKT_HEADER_LEN + payload_offset;

        med -= 7;
        *payload_len += 7;
        bt_sink_srv_a2dp_fill_aac_header(med, *payload_len, &(a2dp->codec));
    } else if (a2dp->codec.type == BT_A2DP_CODEC_SBC) {
        //add sync word
        med = med - BT_SINK_SRV_MEDIA_SBC_SYNC_WORD_LEN;
        med[0] = (BT_SINK_SRV_MEDIA_SBC_SYNC_WORD & 0x00ff);
        med[1] = (BT_SINK_SRV_MEDIA_SBC_SYNC_WORD & 0xff00) >> 8;
        med[2] = (*payload_len & 0x00ff);
        med[3] = (*payload_len & 0xff00) >> 8;
        *payload_len = *payload_len + BT_SINK_SRV_MEDIA_SBC_SYNC_WORD_LEN;
    }

    return med;
}

void bt_sink_srv_a2dp_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *parm)
{
    SinkProfileA2dp *a2dp = NULL;
    SinkProfileAvrcp *avrcp = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    //bt_sink_srv_am_id_t ami_ret = 0;
    int32_t ret = 0;
    int32_t err_code = 0;
    bt_sink_srv_am_media_handle_t *media = NULL;
    //bt_sink_srv_a2dp_event_t a2dp_event;
    //bt_sink_srv_a2dp_play_t a2dp_play;
    bt_sink_srv_music_device_t *dev = NULL, *int_dev = NULL;
    bt_sink_srv_music_device_t *aws_dev = NULL;
    bt_bd_addr_t *dev_addr = NULL;

    cntx = bt_sink_srv_music_get_context();

    if (cntx->aud_id == aud_id && msg_id == AUD_A2DP_PROC_IND && 
        (sub_msg == AUD_STREAM_EVENT_DATA_REQ || AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW == sub_msg)) {
        // drop
        ;
    } else {
        bt_sink_srv_report("[sink][a2dp]ami_hdr[s]-aid: %d, aud_id: %d, msg_id: %d, sub_msg: %d, 2nd: 0x%x\n",
                           cntx->aud_id, aud_id, msg_id, sub_msg, sub_msg);
    }
    if (cntx->aud_id == aud_id) {
        switch (msg_id) {
            case AUD_SINK_OPEN_CODEC: {
                if (cntx->state == BT_SINK_SRV_MUSIC_AMI_PLAY) {
                    media = (bt_sink_srv_am_media_handle_t *)parm;
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                    avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
                    a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
                    if (avrcp && a2dp) {
                        memcpy(&(a2dp->med_hd), media, sizeof(bt_sink_srv_am_media_handle_t));
                        a2dp->profile.state = BT_SINK_SRV_PROFILE_STATE_AUDIO_ON;
                        avrcp->profile.state = BT_SINK_SRV_AVRCP_PLAY;
                        cntx->state = BT_SINK_SRV_MUSIC_PLAY;
                        cntx->log_cnt = 0;
                        a2dp->header.sequence_number = 0;
                        /* always reset interrupt flag */
                        dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_INTERRUPT);

                        /* dev a2dp flag streaming; avrcp play & pause flag reset */
                        dev->flag |= BT_SINK_SRV_MUSIC_FLAG_A2DP_STREAMING;
                        dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AVRCP_PLAY);
                        dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AVRCP_PAUSE);

                        //ret = bt_a2dp_start_streaming_response(cntx->a2dp_hd, true);
                        /* AWS connection */
                        aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
                        bt_sink_srv_report("[sink][a2dp]ami_hdr(aws)-aws_dev: 0x%x, conn_bit: 0x%x\n",
                                           aws_dev, aws_dev->conn_bit);
                        if ((aws_dev) && (aws_dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT)) {
                            if (cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_START_STREAMING_CNF) {
                                cntx->flag &= ~BT_SINK_SRV_MUSIC_FLAG_AWS_START_STREAMING_CNF;
                                cntx->flag &= ~BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC;
                                ret = bt_a2dp_start_streaming_response(cntx->a2dp_hd, true);
                            } else {
                                cntx->flag |= BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC;
                            }
                        } else {
                            // should discard the avdtp start response if currently resumed by local music but 
                            // interrupted by it previously in 12s.
                            if (false == bt_sink_srv_a2dp_check_dev_flag(dev, BT_SINK_SRV_MUSIC_FALG_A2DP_DISCARD_DATA)) {
                                ret = bt_a2dp_start_streaming_response(cntx->a2dp_hd, true);
                            }
                        }

                        /* update focus dev */
                        bt_sink_srv_music_set_focus_device(dev);

                        //a2dp_play.handle = cntx->a2dp_hd;
                        /* notify a2dp event */
                        //a2dp_event.event = BT_SINK_SRV_A2DP_EVENT_PLAY;
                        //a2dp_event.param = &a2dp_play;

                        bt_sink_srv_state_set(BT_SINK_SRV_STATE_STREAMING);

                        cntx->flag &= ~(BT_SINK_SRV_MUSIC_FLAG_PLAYING);
                        cntx->write_len = 0;
                        cntx->pkt_cnt = 0;

                        if (BT_A2DP_CODEC_AAC == a2dp->codec.type) {
                            dev->flag |= BT_SINK_SRV_MUSIC_FLAG_AAC;
                        } else {
                            dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AAC);
                        }
                        /* clear med node */
#ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                        bt_sink_srv_am_med_mgr_clear_node();
#endif
                    } else {
                        // trace error
                        err_code = -1;
                    }
                }
                break;
            }

            /* interrupt */
            case AUD_SUSPEND_BY_IND: {
                if (cntx->state == BT_SINK_SRV_MUSIC_PLAY) {
                    if (AUD_SUSPEND_BY_HFP == sub_msg) {
                        dev_addr = (bt_bd_addr_t *)parm;
                        /* whether the same device */
                        if (cntx->a2dp_hd == 12) {
                            /* ignore */
                        } else {
                            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                            avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
                            a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
                            if (avrcp && a2dp) {
                                a2dp->profile.state = BT_SINK_SRV_PROFILE_STATE_CONNECTED;
                                avrcp->profile.state = BT_SINK_SRV_AVRCP_STOP;
                                cntx->state = BT_SINK_SRV_MUSIC_READY;
                                int_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR, (void *)dev_addr);
                                if ((int_dev) && (int_dev->a2dp_hd == cntx->a2dp_hd)) {
                                    /* ignore interrupt */
                                    err_code = 500;
                                } else {
                                    dev->flag |= BT_SINK_SRV_MUSIC_FLAG_INTERRUPT;
                                    ret = bt_sink_srv_avrcp_stop_music(dev->avrcp_hd);
                                    err_code = 501;
                                }
                            } else {
                                // trace error
                                err_code = -2;
                            }
                        }
                    } else if (AUD_SUSPEND_BY_NONE == sub_msg) {
                        dev_addr = (bt_bd_addr_t *)parm;
                        /* whether the same device */
                        if (cntx->a2dp_hd == 12) {
                            /* ignore */
                        } else {
                            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                            avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
                            a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
                            if (avrcp && a2dp) {
                                a2dp->profile.state = BT_SINK_SRV_PROFILE_STATE_CONNECTED;
                                avrcp->profile.state = BT_SINK_SRV_AVRCP_STOP;
                                cntx->state = BT_SINK_SRV_MUSIC_READY;
                                int_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR, parm);
                                if ((int_dev) && (int_dev->a2dp_hd == cntx->a2dp_hd)) {
                                    /* ignore interrupt */
                                    err_code = 600;
                                } else {
                                    dev->flag |= BT_SINK_SRV_MUSIC_FLAG_INTERRUPT;
                                    ret = bt_sink_srv_avrcp_stop_music(dev->avrcp_hd);
                                    err_code = 601;
                                }
                            } else {
                                // trace error
                                err_code = -2;
                            }
                        }
                    } else {
                        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                        avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
                        a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
                        if (avrcp && a2dp) {
                            a2dp->profile.state = BT_SINK_SRV_PROFILE_STATE_CONNECTED;
                            avrcp->profile.state = BT_SINK_SRV_AVRCP_STOP;
                            cntx->state = BT_SINK_SRV_MUSIC_READY;
                            ret = bt_sink_srv_avrcp_stop_music(dev->avrcp_hd);
                        } else {
                            // trace error
                            err_code = -3;
                        }
                    }
                }
                break;
            }

            case AUD_RESUME_IND: {
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_INTERRUPT, NULL);
                if (dev) {
                    dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_INTERRUPT);
                    ret = bt_sink_srv_avrcp_play_music(dev->avrcp_hd);
                } else {
                    err_code = -101;
                }

                break;
            }

            case AUD_A2DP_PROC_IND: {
#ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                if (sub_msg == AUD_A2DP_CODEC_RESTART) {
                    bt_sink_srv_am_med_mgr_clear_node();
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                    a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
                    media = &(a2dp->med_hd);
                    #ifdef __BT_AWS_SUPPORT__
                    bt_sink_srv_a2dp_start_aws_resync();
                    #else
                    ret = media->play(cntx->aud_id);
                    bt_sink_srv_report("[sink][a2dp](list)ami_hdr-restart ret: %d, ret: %d\r\n", ret);
                    #endif
                } else if (sub_msg == AUD_A2DP_AWS_UNDERFLOW) {
                    bt_sink_srv_am_med_mgr_clear_node();
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                    a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
                    media = &(a2dp->med_hd);
                    media->stop(cntx->aud_id);
                    #ifdef __BT_AWS_SUPPORT__
                    bt_sink_srv_a2dp_start_aws_resync();
                    #else
                    ret = media->play(cntx->aud_id);
                    bt_sink_srv_report("[sink][a2dp](list-ud)ami_hdr-restart ret: %d, ret: %d\r\n", ret);
                    #endif
                }
#endif /* __BT_SINK_SRV_AM_MED_LIST_SUPPORT__ */
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
                a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
                if (avrcp && a2dp) {
                    media = &(a2dp->med_hd);
                    #ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                    if (sub_msg == AUD_A2DP_CODEC_RESTART) {
                        #ifdef __BT_AWS_SUPPORT__
                        bt_sink_srv_a2dp_start_aws_resync();
                        #else
                        ret = media->play(cntx->aud_id);
                        bt_sink_srv_report("[sink][a2dp]ami_hdr-restart ret: %d, ret: %d\r\n", ret);
                        #endif
                    }
                    #endif
                    //media->stop(cntx->aud_id);
                    //media->play(cntx->aud_id);
                    //media->process(cntx->aud_id, sub_msg);
                } else {
                    // trace error
                    err_code = -4;
                    #ifndef MTK_DEBUG_LEVEL_INFO
                    g_test_fix_warning = err_code;
                    g_test_fix_warning = ret;
                    #endif
                }
                break;
            }
            default:
                break;
        }
    }

    if (cntx->aud_id == aud_id && msg_id == AUD_A2DP_PROC_IND &&
        (sub_msg == AUD_STREAM_EVENT_DATA_REQ || AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW == sub_msg)) {
        // drop
        ;
    } else {
        bt_sink_srv_report("[sink][a2dp]ami_hdr[e]-err_code: %d, ret: %d\r\n", err_code, ret);
    }
}


static int32_t bt_sink_srv_a2dp_handle_connect_cnf(bt_a2dp_connect_cnf_t *conn_cnf)
{
    SinkProfileA2dp *a2dp = NULL;
    //SinkProfileAvrcp *avrcp = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    //bt_sink_srv_a2dp_event_t a2dp_event;
    bt_sink_srv_a2dp_connect_t a2dp_connect;
    int32_t ret = -110;
    bt_bd_addr_t dev_addr;
    bool connected = false;
    uint32_t hd = 0;

    cntx = bt_sink_srv_music_get_context();

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(conn_cnf->handle)));

    bt_sink_srv_memcpy(&dev_addr, &(dev->dev_addr), sizeof(bt_bd_addr_t));

    a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&dev->dev_addr, BT_SINK_SRV_UUID_A2DP);
    if (a2dp) {
        a2dp->profile.state = BT_SINK_SRV_PROFILE_STATE_CONNECTED;
        memcpy(&(a2dp->codec), (conn_cnf->codec_cap), sizeof(bt_a2dp_codec_capability_t));
    }

    bt_sink_srv_report("[sink][a2dp]connect_cnf-a2dp_hd: 0x%x\n", conn_cnf->handle);

    if (conn_cnf->status == BT_STATUS_SUCCESS) {
        /* a2dp connected */
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(conn_cnf->handle)));
        if (dev) {
            ;
        } else {
            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, NULL);
        }
        /* as slave */
        bt_sink_srv_cm_set_role(&(dev->dev_addr), BT_ROLE_SLAVE);
        dev->conn_bit |= BT_SINK_SRV_MUSIC_A2DP_CONN_BIT;

        /* 1st dev a2dp connect*/
        if (cntx->state == BT_SINK_SRV_MUSIC_IDLE) {
            cntx->state = BT_SINK_SRV_MUSIC_READY;
        }

        /* fill notify event struct */
        a2dp_connect.ret = 0;
        a2dp_connect.handle = dev->a2dp_hd;
        memcpy(&(a2dp_connect.dev_addr), &(dev->dev_addr), sizeof(bt_bd_addr_t));

        if ((dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_CON_IND)) {
            a2dp_connect.ind = true;
        } else {
            a2dp_connect.ind = false;
        }
#if 0
        if ((dev->flag & BT_SINK_SRV_FLAG_A2DP_CON_IND) && (!(dev->conn_bit & BT_SINK_SRV_AVRCP_CONN_BIT))) {
            /* connect avrcp */
            bt_avrcp_connect(&(dev->dev_addr), BT_AVRCP_ROLE_CT);
        }
        /* active connect avrcp */
        if ((!(dev->flag & BT_SINK_SRV_FLAG_A2DP_CON_IND)) && (!(dev->conn_bit & BT_SINK_SRV_AVRCP_CONN_BIT))) {
            /* connect avrcp */
            bt_avrcp_connect(&(dev->dev_addr), BT_AVRCP_ROLE_CT);
        }
#endif
        bt_sink_srv_report("[sink][a2dp]connect_cnf-con_bit: 0x%x\n", dev->conn_bit);
        if (!(dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
            ret = bt_avrcp_connect(&hd, (const bt_bd_addr_t *)(&(dev->dev_addr)));
        #ifdef MTK_BT_COMMAND_ENABLE
            if (ret == BT_STATUS_SUCCESS) {
                bt_sink_srv_cm_profile_state_change_notify(&(dev->dev_addr), 
                        BT_SINK_SRV_CM_PROFILE_AVRCP, BT_SINK_SRV_CM_STATE_CONNECTING);
            } 
        #endif 
            //if (BT_STATUS_SUCCESS == ret) {
            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR, &(dev->dev_addr));
            if (dev) {
                ;
            } else {
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, NULL);
            }
            dev->avrcp_hd = hd;
            //}
            bt_sink_srv_report("[sink][a2dp]connect_cnf-ret: %d, avrcp_hd: 0x%x\n", ret, hd);
        }
        connected = true;
    } else {
        /* fill notify event struct */
        a2dp_connect.ret = -1;
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(dev->a2dp_hd)));
        if (dev) {
            if ((dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_CON_IND)) {
                a2dp_connect.ind = true;
            } else {
                a2dp_connect.ind = false;
            }
            bt_sink_srv_music_reset_device(dev);
        }
    }

    /* notify a2dp event */
    //a2dp_event.event = BT_SINK_SRV_A2DP_EVENT_CONNECT;
    //a2dp_event.param = &a2dp_connect;

    bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_TYPE_A2DP, connected);
#ifdef MTK_BT_COMMAND_ENABLE
    if(connected) {
        bt_sink_srv_cm_profile_state_change_notify(&(dev->dev_addr), 
            BT_SINK_SRV_CM_PROFILE_A2DP, BT_SINK_SRV_CM_STATE_CONNECTED);
    } else {
        bt_sink_srv_cm_profile_state_change_notify(&(dev->dev_addr), 
            BT_SINK_SRV_CM_PROFILE_A2DP, BT_SINK_SRV_CM_STATE_DISCONNECTED);
    }
#endif

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_connect_ind(bt_a2dp_connect_ind_t *conn_ind)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = 0;
    //bt_sink_srv_music_context_t *cntx = NULL;

    //cntx = bt_sink_srv_music_get_context();

    bt_sink_srv_report("[sink][a2dp]connect_ind-a2dp_hd: 0x%x\n", conn_ind->handle);

    /* a2dp connected */
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(conn_ind->handle)));
    if (dev) {
        ;
    } else {
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, NULL);
    }
    if (dev) {
        dev->a2dp_hd = conn_ind->handle;
        dev->role = conn_ind->role;
        memcpy(&(dev->dev_addr), conn_ind->address, sizeof(bt_bd_addr_t));
        dev->flag |= BT_SINK_SRV_MUSIC_FLAG_A2DP_CON_IND;
    }
    ret = bt_a2dp_connect_response(conn_ind->handle, true);
#ifdef MTK_BT_COMMAND_ENABLE
    bt_sink_srv_cm_profile_state_change_notify(&(dev->dev_addr), 
        BT_SINK_SRV_CM_PROFILE_A2DP, BT_SINK_SRV_CM_STATE_CONNECTING);
#endif
    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_disconnect_cnf(bt_a2dp_disconnect_cnf_t *disconn_cnf)
{
    SinkProfileA2dp *a2dp = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    int32_t ret = 0, err = -200;
    //bt_sink_srv_a2dp_event_t a2dp_event;
    bt_sink_srv_a2dp_disconnect_t a2dp_disconnect;
    bt_bd_addr_t dev_addr;
    bool connected = false;

    cntx = bt_sink_srv_music_get_context();
    bt_sink_srv_set_music_pause_state(SINK_MUSIC_STATE_PAUSE_NONE);

    bt_sink_srv_report("[sink][a2dp]disconnect_cnf-state: %d, hd: 0x%x\n", cntx->state, disconn_cnf->handle);

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(disconn_cnf->handle)));
    bt_sink_srv_memcpy(&dev_addr, &(dev->dev_addr), sizeof(bt_bd_addr_t));

    if (disconn_cnf->status == BT_STATUS_SUCCESS) {
        a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
        if (a2dp) {
            a2dp->profile.state = BT_SINK_SRV_PROFILE_STATE_DISCONNECTED;
            memset(&(a2dp->header), 0x00, sizeof(bt_sink_srv_a2dp_media_header_t));
        }

        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(disconn_cnf->handle)));
        if (dev) {
            /* dev a2dp flag streaming reset; avrcp play & pause flag reset */
            uint32_t flag = BT_SINK_SRV_MUSIC_FLAG_A2DP_STREAMING | BT_SINK_SRV_MUSIC_FLAG_AVRCP_PLAY 
                            | BT_SINK_SRV_MUSIC_FLAG_AVRCP_PAUSE | BT_SINK_SRV_MUSIC_FALG_A2DP_DISCARD_DATA;
            bt_sink_srv_a2dp_clear_dev_flag(dev, flag);

            dev->conn_bit &= ~(BT_SINK_SRV_MUSIC_A2DP_CONN_BIT);
            a2dp_disconnect.handle = dev->a2dp_hd;

            memcpy(&(a2dp_disconnect.dev_addr), &(dev->dev_addr), sizeof(bt_bd_addr_t));
            if (!(dev->conn_bit)) {
                bt_sink_srv_music_reset_device(dev);
            }
        }

        if (cntx->a2dp_hd == disconn_cnf->handle) {
            if (cntx->state == BT_SINK_SRV_MUSIC_PLAY) {
        #ifdef __MUSIC_MODE_SWITCH_SUPPORT__
                uint8_t mode = (uint8_t)bt_sink_srv_ami_get_music_mode();
                if (mode == BT_SINK_SRV_MODE_BT_MUSIC)
        #endif
                {
                    err = a2dp->med_hd.stop(cntx->aud_id);
                    ret = bt_sink_srv_ami_audio_stop(cntx->aud_id);
                }
                bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
                bt_sink_srv_report("[sink][a2dp]disconnect_cnf-ret: %d, err: %d\n", ret, err);
            }
            cntx->a2dp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
            cntx->state = BT_SINK_SRV_MUSIC_IDLE;
        }
        a2dp_disconnect.ret = 0;
    } else {
        a2dp_disconnect.ret = -1;
    }

    a2dp_disconnect.ind = false;
    /* notify a2dp event */
    //a2dp_event.event = BT_SINK_SRV_A2DP_EVENT_DISCONNECT;
    //a2dp_event.param = &a2dp_disconnect;
    bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_TYPE_A2DP, connected);
#ifdef MTK_BT_COMMAND_ENABLE
    bt_sink_srv_cm_profile_state_change_notify(&(dev->dev_addr), 
            BT_SINK_SRV_CM_PROFILE_A2DP, BT_SINK_SRV_CM_STATE_DISCONNECTED);
#endif
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_disconnect_ind(bt_a2dp_disconnect_ind_t *disconn_ind)
{
    SinkProfileA2dp *a2dp = NULL;
    //SinkProfileAvrcp *avrcp = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    int32_t ret = -101, err = -102;
    //bt_sink_srv_a2dp_event_t a2dp_event;
    bt_sink_srv_a2dp_disconnect_t a2dp_disconnect;
    bt_bd_addr_t dev_addr;
    bool connected = false;

    cntx = bt_sink_srv_music_get_context();

    bt_sink_srv_report("[sink][a2dp]disconnect_ind-state: %d, hd: 0x%x\n", cntx->state, disconn_ind->handle);
    bt_sink_srv_set_music_pause_state(SINK_MUSIC_STATE_PAUSE_NONE);

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(disconn_ind->handle)));
    bt_sink_srv_memcpy(&dev_addr, &(dev->dev_addr), sizeof(bt_bd_addr_t));

    a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
    if (a2dp) {
        a2dp->profile.state = BT_SINK_SRV_PROFILE_STATE_DISCONNECTED;
        memset(&(a2dp->header), 0x00, sizeof(bt_sink_srv_a2dp_media_header_t));
    }

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(disconn_ind->handle)));
    if (dev) {
        /* dev a2dp flag streaming reset; avrcp play & pause flag reset */
        uint32_t flag = BT_SINK_SRV_MUSIC_FLAG_A2DP_STREAMING | BT_SINK_SRV_MUSIC_FLAG_AVRCP_PLAY 
                        | BT_SINK_SRV_MUSIC_FLAG_AVRCP_PAUSE | BT_SINK_SRV_MUSIC_FALG_A2DP_DISCARD_DATA;
        bt_sink_srv_a2dp_clear_dev_flag(dev, flag);
        dev->conn_bit &= ~(BT_SINK_SRV_MUSIC_A2DP_CONN_BIT);
        a2dp_disconnect.handle = dev->a2dp_hd;
        memcpy(&(a2dp_disconnect.dev_addr), &(dev->dev_addr), sizeof(bt_bd_addr_t));
        if (!(dev->conn_bit)) {
            bt_sink_srv_music_reset_device(dev);
        }
    }
#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
    bt_sink_srv_music_mode_t mode = bt_sink_srv_ami_get_music_mode();
    if (mode == BT_SINK_SRV_MODE_BT_MUSIC) {
#endif
        if (cntx->a2dp_hd == disconn_ind->handle) {
            if (cntx->flag | BT_SINK_SRV_MUSIC_FLAG_PLAYING) {
                cntx->flag &= ~(BT_SINK_SRV_MUSIC_FLAG_PLAYING);
            }
            if (cntx->state == BT_SINK_SRV_MUSIC_PLAY) {
                err = a2dp->med_hd.stop(cntx->aud_id);
                ret = bt_sink_srv_ami_audio_stop(cntx->aud_id);
                bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
                bt_sink_srv_report("[sink][a2dp]disconnect_ind-ret: %d, err: %d\n", ret, err);
            }
            cntx->a2dp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
            cntx->state = BT_SINK_SRV_MUSIC_IDLE;
        }
#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
    }
#endif

    a2dp_disconnect.ret = 0;
    a2dp_disconnect.ind = true;
    /* notify a2dp event */
    //a2dp_event.event = BT_SINK_SRV_A2DP_EVENT_DISCONNECT;
    //a2dp_event.param = &a2dp_disconnect;

    bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_TYPE_A2DP, connected);
#ifdef MTK_BT_COMMAND_ENABLE
    bt_sink_srv_cm_profile_state_change_notify(&(dev->dev_addr), 
        BT_SINK_SRV_CM_PROFILE_A2DP, BT_SINK_SRV_CM_STATE_DISCONNECTED);
#endif
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_start_streaming_cnf(bt_a2dp_start_streaming_cnf_t *start_cnf)
{
    int32_t ret = 0;

    return ret;
}

static int32_t bt_sink_srv_a2dp_handle_start_streaming_ind(bt_a2dp_start_streaming_ind_t *start_ind)
{
    SinkProfileA2dp *a2dp = NULL;
    SinkProfileAvrcp *avrcp = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    int32_t ret = -100;
    bt_sink_srv_am_id_t ami_ret = 0;
    bt_sink_srv_am_audio_capability_t  aud_cap;
    #ifdef __BT_AWS_SUPPORT__
    bt_aws_codec_capability_t aws_cap;
    bt_sink_srv_music_device_t *aws_dev = NULL;
    #endif

    cntx = bt_sink_srv_music_get_context();

    bt_sink_srv_report("[sink][a2dp]handle_strat_streaming_ind[s]-state: %d, pd: %d\r\n",
                       cntx->state, cntx->partymode);
#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
    // check whether it's in BT mode. If not, accept and suspend it later to avoid IOT issues
    if (bt_sink_srv_ami_get_music_mode() != BT_SINK_SRV_MODE_BT_MUSIC) {
        bt_sink_srv_report("[sink][a2dp] response in local music mode for start ind");
        bt_a2dp_start_streaming_response(start_ind->handle, true);
        bt_sink_srv_set_music_pause_state(SINK_MUSIC_STATE_PAUSE_NONE);

        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(start_ind->handle)));
        if (NULL != dev) {
            a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
            bt_sink_srv_a2dp_set_dev_flag(dev, BT_SINK_SRV_MUSIC_FALG_A2DP_DISCARD_DATA);
        }
        if (a2dp) {
            //remember the codec, such that the mode will be changed to A2DP before a2dp suspend ind.
            memcpy(&a2dp->codec, (start_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
        }
        if (NULL != dev) {
            bt_status_t ret = bt_sink_srv_a2dp_stop_music(dev->avrcp_hd, 100);
            bt_sink_srv_report("[sink][a2dp] pause 100ms later, avrcp_hd:0x%08X, ret:0x%08X", 
            dev->avrcp_hd, ret);
        }
        return BT_STATUS_SUCCESS;
    }
#endif

#if 0
    bt_a2dp_start_streaming_response(start_ind->handle, true);
#else
    /* always accept to avoid IOT issue */
    //bt_a2dp_start_streaming_response(start_ind->conn_id, true);
    if (cntx->state == BT_SINK_SRV_MUSIC_PLAY) {
        /* party mode */
        switch (cntx->partymode) {
            case BT_SINK_SRV_PARTYMODE_INTERRUPT: {
                /* change state and stop pre music */
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
                a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
                if (avrcp && a2dp) {
                    int32_t ret1 = -1, ret2 = -1, ret3 = -1;
                    a2dp->profile.state = BT_SINK_SRV_PROFILE_STATE_CONNECTED;
                    ret1 = bt_sink_srv_avrcp_stop_music(dev->avrcp_hd);
                    cntx->a2dp_hd = start_ind->handle;
                    ret2 = a2dp->med_hd.stop(cntx->aud_id);
                    cntx->state = BT_SINK_SRV_MUSIC_AMI_STOP;
                    ret3 = bt_sink_srv_ami_audio_stop(cntx->aud_id);
                    bt_sink_srv_report("[sink][a2dp]start_ind(int)-ret1: %d, ret2: %d, ret3: %d\n", ret1, ret2, ret3);
                    #ifndef MTK_DEBUG_LEVEL_INFO
                    g_test_fix_warning = ret1;
                    g_test_fix_warning = ret2;
                    g_test_fix_warning = ret3;
                    g_test_fix_warning = ami_ret;
                    #endif
                } else {
                    // trace error
                    ret = -(1000 + 0);
                }

                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(start_ind->handle)));
                avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
                a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
                if (avrcp && a2dp) {
                    cntx->a2dp_hd = start_ind->handle;
                    cntx->state = BT_SINK_SRV_MUSIC_AMI_PLAY;
                    /* fill aud param */
                    bt_sink_srv_fill_audio_param(&aud_cap, start_ind->codec_cap, BT_A2DP_SINK);
                    ami_ret = bt_sink_srv_ami_audio_play(cntx->aud_id, &aud_cap);
                    memcpy(&a2dp->codec, (start_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
                } else {
                    // trace error
                    ret = -(1000 + 1);
                }
                break;
            }

            case BT_SINK_SRV_PARTYMODE_DROP: {
                if (start_ind->handle != cntx->a2dp_hd) {
                    bt_a2dp_start_streaming_response(start_ind->handle, true);
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(start_ind->handle)));
                    avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
                    bt_sink_srv_avrcp_stop_music(dev->avrcp_hd);
                    if (avrcp) {
                        avrcp->profile.state = BT_SINK_SRV_AVRCP_STOP;
                    } else {
                        // trace error
                        ret = -(1000 + 2);
                    }
                }
                break;
            }

            default:
                break;
        }
    } else if (cntx->state == BT_SINK_SRV_MUSIC_READY) {
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(start_ind->handle)));
        avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
        a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
        if (avrcp && a2dp) {
            //a2dp->profile.state = BT_SINK_SRV_A2DP_STREAMING;
            //avrcp->profile.state = BT_SINK_SRV_AVRCP_PLAY;
            cntx->a2dp_hd = start_ind->handle;
            //cntx->state = BT_SINK_SRV_MUSIC_PLAY;
            cntx->state = BT_SINK_SRV_MUSIC_AMI_PLAY;
            /* fill aud param */
            bt_sink_srv_fill_audio_param(&aud_cap, start_ind->codec_cap, BT_A2DP_SINK);  
#if 1
            if (aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.type == BT_A2DP_CODEC_SBC) {
                bt_sink_srv_report("[sink][a2dp]start_ind(am)--type: %d, role: %d, type2: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d\r\n",
                                   aud_cap.type,
                                   aud_cap.codec.a2dp_format.a2dp_codec.role,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.type,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.sep_type,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.length,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.sbc.channel_mode,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.sbc.sample_freq,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.sbc.alloc_method,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.sbc.subbands,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.sbc.block_len,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.sbc.min_bitpool,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.sbc.max_bitpool);
            } else if (aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.type == BT_A2DP_CODEC_AAC) {
                bt_sink_srv_report("[sink][a2dp]start_ind(am)--type: %d, role: %d, type2: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d, 8: %d, 9: %d\r\n",
                                   aud_cap.type,
                                   aud_cap.codec.a2dp_format.a2dp_codec.role,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.type,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.sep_type,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.length,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.object_type,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.freq_h,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.reserved,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.channels,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.freq_l,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.br_h,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.vbr,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.br_m,
                                   aud_cap.codec.a2dp_format.a2dp_codec.codec_cap.codec.aac.br_l);
            }

            cntx->flag &= ~BT_SINK_SRV_MUSIC_FLAG_AWS_START_STREAMING_CNF;
            cntx->flag &= ~BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC;
            cntx->flag &= ~BT_SINK_SRV_MUSIC_FLAG_AWS_WAIT_ROLE_STREAMING;

            #ifdef __BT_AWS_SUPPORT__
            aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);

            if ((aws_dev) &&
                (aws_dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) &&
                (aws_dev->aws_role == BT_AWS_ROLE_SOURCE)) {
                /* AWS connection, translate start streaming ind */
                if (start_ind->codec_cap->type == BT_A2DP_CODEC_SBC) {
                    aws_cap.type = BT_AWS_CODEC_TYPE_SBC;
                } else if (start_ind->codec_cap->type == BT_A2DP_CODEC_AAC) {
                    aws_cap.type = BT_AWS_CODEC_TYPE_AAC;
                } else {
                    /* trace error */
                }

                aws_cap.length = start_ind->codec_cap->length;
                memcpy(&(aws_cap.codec), &(start_ind->codec_cap->codec), start_ind->codec_cap->length);
                ret = bt_aws_start_streaming_request(aws_dev->aws_hd, &aws_cap);
                // aws status wrong, suspend aws dev and try to play at aws suspend cnf
                if (ret == BT_STATUS_FAIL) {
                    cntx->flag |= BT_SINK_SRV_MUSIC_FLAG_AWS_SEND_START_STREAMING;
                    memcpy(&a2dp->codec, (start_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
                    ret = bt_aws_suspend_streaming_request(aws_dev->aws_hd);
                    bt_sink_srv_report("[sink][aws]suspend_ind(aws)--ret: %d type %x len %x a2dp %x\n", ret, a2dp->codec.type, a2dp->codec.length, &a2dp->codec);
                    return ret;
                }
                bt_sink_srv_report("[sink][a2dp]start_ind(aws)--ret: %d\n", ret);
            } else if ((aws_dev) &&
                       (aws_dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) &&
                       (aws_dev->aws_role != BT_AWS_ROLE_SOURCE)) {
                cntx->flag |= BT_SINK_SRV_MUSIC_FLAG_AWS_WAIT_ROLE_STREAMING;
                bt_aws_set_role_request(aws_dev->aws_hd, BT_AWS_ROLE_SOURCE);
                bt_sink_srv_report("[sink][a2dp]start_ind(aws)--wait aws role\n");
            } else {
                bt_sink_srv_report("[sink][a2dp]start_ind(noaws)--aws: 0x%x, bit: 0x%x, role: %d\n",
                    aws_dev, aws_dev->conn_bit, aws_dev->aws_role);
            }
            #endif /* __BT_AWS_SUPPORT__ */
            ami_ret = bt_sink_srv_ami_audio_play(cntx->aud_id, &aud_cap);
            if (ami_ret == 0) {
                bt_sink_srv_a2dp_set_dev_flag(dev, BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC);
            }
            bt_sink_srv_report("[sink][a2dp]start_ind(am)--aud_id:%d, ret: %d\r\n", cntx->aud_id, ami_ret);
#else
            bt_sink_srv_report("[sink][a2dp]common_hdr without\r\n");
            bt_a2dp_start_streaming_response(start_ind->conn_id, true);
            cntx->state = BT_SINK_SRV_MUSIC_PLAY;
#endif
            memcpy(&a2dp->codec, (start_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
        } else {
            // trace error
            ret = -(1000 + 3);
        }
    } else if ((cntx->state == BT_SINK_SRV_MUSIC_AMI_PLAY)) {
        /* party mode */
        switch (cntx->partymode) {
            case BT_SINK_SRV_PARTYMODE_INTERRUPT: {
                /* change state and stop pre music */
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
                a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
                if (avrcp && a2dp) {
                    a2dp->profile.state = BT_SINK_SRV_PROFILE_STATE_CONNECTED;
                    avrcp->profile.state = BT_SINK_SRV_AVRCP_STOP;
                    bt_sink_srv_avrcp_stop_music(dev->avrcp_hd);
                    bt_a2dp_start_streaming_response(start_ind->handle, true);
                } else {
                    // trace error
                    ret = -(1000 + 4);
                }
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(start_ind->handle)));
                avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
                a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
                if (avrcp && a2dp) {
                    cntx->a2dp_hd = start_ind->handle;
                    cntx->state = BT_SINK_SRV_MUSIC_AMI_PLAY;
                    /* fill aud param */
                    bt_sink_srv_fill_audio_param(&aud_cap, start_ind->codec_cap, BT_A2DP_SINK);
                    ami_ret = bt_sink_srv_ami_audio_play(cntx->aud_id, &aud_cap);
                    if (ami_ret == 0) {
                        bt_sink_srv_a2dp_set_dev_flag(dev, BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC);
                    }
                    bt_sink_srv_report("[sink][a2dp]start_ind(am)--aud_id:%d, ret: %d\r\n", cntx->aud_id, ami_ret);

                    memcpy(&a2dp->codec, (start_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
                } else {
                    // trace error
                    ret = -(1000 + 5);
                }
                break;
            }

            case BT_SINK_SRV_PARTYMODE_DROP: {
                if (start_ind->handle != cntx->a2dp_hd) {
                    bt_a2dp_start_streaming_response(start_ind->handle, true);
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(start_ind->handle)));
                    avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
                    bt_sink_srv_avrcp_stop_music(dev->avrcp_hd);
                    if (avrcp) {
                        avrcp->profile.state = BT_SINK_SRV_AVRCP_STOP;
                    } else {
                        // trace error
                        ret = -(1000 + 6);
                    }
                }
                break;
            }

            default:
                break;
        }
    } else {
        /* upexpected state */
        ret = -(1000 + 7);
    }
#endif

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_suspend_streaming_cnf(bt_a2dp_suspend_streaming_cnf_t *suspend_cnf)
{
    bt_sink_srv_music_context_t *cntx = NULL;
    int32_t ret = 0;

    cntx = bt_sink_srv_music_get_context();
#ifdef WIN32
    if (suspend_cnf->status == BT_STATUS_A2DP_OK) {
        bt_sink_srv_report("remote device agree with supending streaming");
        WaveLib_Pause(hd_wave, true);
    } else {
        bt_sink_srv_report("remote device disagree with suspending streaming");
    }
#endif
    cntx->state = BT_SINK_SRV_MUSIC_READY;

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_suspend_streaming_ind(bt_a2dp_suspend_streaming_ind_t *suspend_ind)
{
    SinkProfileA2dp *a2dp = NULL;
    SinkProfileAvrcp *avrcp = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    int32_t ret = 0, err = 0;
    //bt_sink_srv_a2dp_event_t a2dp_event;
    //bt_sink_srv_a2dp_suspend_t a2dp_suspend;
    #ifdef __BT_AWS_SUPPORT__
    bt_sink_srv_music_device_t *aws_dev = NULL;
    #endif

    cntx = bt_sink_srv_music_get_context();

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(suspend_ind->handle)));
    bt_sink_srv_a2dp_clear_dev_flag(dev, BT_SINK_SRV_MUSIC_FALG_A2DP_DISCARD_DATA);

#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
        // check whether it's in BT mode. If not, accept and suspend it later to avoid IOT issues
        bt_sink_srv_set_music_pause_state(SINK_MUSIC_STATE_PAUSE_A2DP);
        if (bt_sink_srv_ami_get_music_mode() != BT_SINK_SRV_MODE_BT_MUSIC) {
            bt_sink_srv_report("[sink][a2dp] response in local music mode for suspend ind");
            bt_a2dp_suspend_streaming_response(suspend_ind->handle, true);
            return BT_STATUS_SUCCESS;
        }
#endif


    a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
    #ifdef __BT_AWS_SUPPORT__
    aws_dev  = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
    if (aws_dev && (dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_STREAMING)) {
        cntx->flag |= BT_SINK_SRV_MUSIC_FLAG_AWS_SEND_SUSPEND;
        ret = bt_aws_suspend_streaming_request(aws_dev->aws_hd);
        bt_sink_srv_report("[sink][a2dp]suspend_streaming_ind(aws)-ret:0x%8X, cntx_flag:0x%08X", ret, cntx->flag);
        if (ret != BT_STATUS_SUCCESS) {
            bt_a2dp_suspend_streaming_response(suspend_ind->handle, true);
        }
    }
    #endif /* __BT_AWS_SUPPORT__ */
    #ifdef __BT_AWS_SUPPORT__
    if (aws_dev && (dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_STREAMING)) {
        /* wait aws suspend cnf */
    } else {
        ret = bt_a2dp_suspend_streaming_response(suspend_ind->handle, true);
    }
    #else
    ret = bt_a2dp_suspend_streaming_response(suspend_ind->handle, true);
    #endif /* __BT_AWS_SUPPORT__ */
    bt_sink_srv_am_med_mgr_clear_node();
    if ((cntx->state == BT_SINK_SRV_MUSIC_PLAY) && (cntx->a2dp_hd == suspend_ind->handle)) {
        cntx->state = BT_SINK_SRV_MUSIC_READY;
        if (cntx->flag | BT_SINK_SRV_MUSIC_FLAG_PLAYING) {
            cntx->flag &= ~(BT_SINK_SRV_MUSIC_FLAG_PLAYING);
        }
        err = a2dp->med_hd.stop(cntx->aud_id);
        ret = bt_sink_srv_ami_audio_stop(cntx->aud_id);
        bt_sink_srv_report("[sink][a2dp]suspend_streaming_ind(am)-ret: %d, err: %d\n", ret, err);
    } else {
        ;
    }
    a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
    avrcp = (SinkProfileAvrcp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AVRCP);
    if (avrcp && a2dp) {
        a2dp->profile.state = BT_SINK_SRV_PROFILE_STATE_CONNECTED;
        memset(&(a2dp->header), 0x00, sizeof(bt_sink_srv_a2dp_media_header_t));
        avrcp->profile.state = BT_SINK_SRV_AVRCP_STOP;
    } else {
        // trace error
    }

    if (dev) {
        /* dev a2dp flag streaming reset; avrcp play & pause flag reset */
        uint32_t a2dp_flag = (BT_SINK_SRV_MUSIC_FLAG_A2DP_STREAMING | BT_SINK_SRV_MUSIC_FLAG_AVRCP_PLAY
                            | BT_SINK_SRV_MUSIC_FLAG_AVRCP_PAUSE | BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC);
        
        bt_sink_srv_a2dp_clear_dev_flag(dev, a2dp_flag);
    }

    //a2dp_suspend.handle = suspend_ind->handle;
    /* notify a2dp event */
    //a2dp_event.event = BT_SINK_SRV_A2DP_EVENT_SUSPEND;
    //a2dp_event.param = &a2dp_suspend;

    bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_reconfigure_cnf(bt_a2dp_reconfigure_cnf_t *reconfigure_cnf)
{
    int32_t ret = 0;

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_reconfigure_ind(bt_a2dp_reconfigure_ind_t *reconfigure_ind)
{
    SinkProfileA2dp *a2dp = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = 0;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(reconfigure_ind->handle)));
    a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
    if (a2dp) {
        memcpy(&a2dp->codec, &(reconfigure_ind->codec_cap), sizeof(bt_a2dp_codec_capability_t));
    }
    ret = bt_a2dp_reconfigure_response(reconfigure_ind->handle, true);

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_data_received_ind(bt_a2dp_streaming_received_ind_t *data_ind)
{
    SinkProfileA2dp *a2dp = NULL;
    //SinkProfileAvrcp *avrcp = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    int32_t ret = 0;
    //uint32_t len = 0;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
#ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
    uint32_t free_len = 0, cur_len = 0, write_len = 0;
    uint8_t *ring_buf = NULL;
#endif
    uint8_t dev_db = 0;
    //bt_sink_srv_am_id_t ami_ret = 0;
    //bt_sink_srv_am_volume_index_t am_volume;
    uint8_t *med_data = NULL;
    uint32_t payload_size = 0;
    //bt_sink_srv_a2dp_event_t a2dp_event;
    //bt_sink_srv_a2dp_suspend_t a2dp_streaming;
    //static int32_t data_cnt = 0;
    uint8_t *hci_buff = NULL;
    bt_hci_packet_t *pkt = NULL;
    #ifdef __BT_AWS_SUPPORT__
    uint32_t time_dur = 0;
    hal_gpt_status_t gpt_ret = HAL_GPT_STATUS_OK;
    bt_sink_srv_music_device_t *aws_dev = NULL;
    int32_t err = 0;
    bt_aws_notify_t aws_notify = {0};
    #endif /* __BT_AWS_SUPPORT__ */

    cntx = bt_sink_srv_music_get_context();

    hci_buff = (uint8_t *)(data_ind->data_node);
    pkt = (bt_hci_packet_t *)(data_ind->data_node);

#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
    // check whether it's in BT mode. If not, suspend it.
            // check whether it's in BT mode. If not, accept and suspend it later to avoid IOT issues
    {
        bt_sink_srv_music_mode_t cur_mode = bt_sink_srv_ami_get_music_mode();
        bool codec_opened_flag;

        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(data_ind->handle)));
        codec_opened_flag= bt_sink_srv_a2dp_check_dev_flag(dev, BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC);
        if (cntx->a2dp_hd == BT_SINK_SRV_MUSIC_INVALID_HD ||
                cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_MODE_CHANGING) {
            bt_sink_srv_report("[sink][a2dp] data recv, drop it, a2dp_hd:0x%08X, pkt_num:%d", 
            cntx->a2dp_hd, bt_sink_srv_a2dp_data_rece_cnt);
            return BT_STATUS_SUCCESS;
        }
        if (cur_mode == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
            bt_sink_srv_report("[sink][a2dp] data in, drop it, pkt_num:%d",bt_sink_srv_a2dp_data_rece_cnt);
            if (true == codec_opened_flag) {
                #ifdef __BT_AWS_SUPPORT__
                aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
                if (aws_dev) {
                    ret = bt_aws_suspend_streaming_request(aws_dev->aws_hd);
                }
                bt_sink_srv_report("[sink][a2dp] send aws suspend--ret:0x%x", ret);
                #endif
                // need to send pause via, close codec, set discard to true. <bt-->mp3 just>
                ret = bt_sink_srv_avrcp_stop_music(dev->avrcp_hd);
                // close codec.
                ret = bt_sink_srv_stop_close_codec();
                bt_sink_srv_report("[sink][a2dp] data recv, in local mode, stop close codec--ret:0x%08X", ret);
                // set discard to true
                bt_sink_srv_a2dp_set_dev_flag(dev, BT_SINK_SRV_MUSIC_FALG_A2DP_DISCARD_DATA);
                bt_sink_srv_a2dp_clear_dev_flag(dev, BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC);
            }
            return BT_STATUS_SUCCESS;
        } else if (cur_mode == BT_SINK_SRV_MODE_BT_MUSIC) {
            if (false == codec_opened_flag) {
                // need to send open codec, set open codec to true. <mp3-->bt just>
                bt_sink_srv_report("[sink][a2dp] data recv, in bt mode, open codec, a2dp_hd:0x%08X, pkt_num:%d", 
                cntx->a2dp_hd, bt_sink_srv_a2dp_data_rece_cnt);

                ret = bt_sink_srv_open_a2dp_codec();
                bt_sink_srv_report("[sink][a2dp] data recv, in bt mode, open codec--ret:0x%08X", ret);
                bt_sink_srv_a2dp_set_dev_flag(dev, BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC);
            }
            if (false == codec_opened_flag) {
                bt_sink_srv_a2dp_clear_dev_flag(dev, BT_SINK_SRV_MUSIC_FALG_A2DP_DISCARD_DATA);
            }
        }
    }
#endif

    if ((cntx->state == BT_SINK_SRV_MUSIC_PLAY) &&
            (cntx->a2dp_hd == data_ind->handle)) {
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(data_ind->handle)));
        a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
        med_hd = &(a2dp->med_hd);

        if (a2dp->header.sequence_number == 0) {
            bt_sink_srv_a2dp_fetch_media_header(hci_buff + data_ind->media_offset,
                                                &(a2dp->header));
        }

        med_data = bt_sink_srv_a2dp_uppack_media_data(data_ind->handle,
                   hci_buff + data_ind->media_offset,
                   data_ind->total_length - data_ind->media_offset,
                   &payload_size);

        if (a2dp) {
            ;
        }

#ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
        /* queue list handle */
        pkt->offset = pkt->packet_length - payload_size;
        #ifdef __BT_AWS_SUPPORT__
        pkt->pad = ((pkt->pad & 0xffff0000) | (data_ind->media_offset));
        #endif
        ret = bt_sink_srv_am_med_mgr_push_data((void *)pkt);

        if (ret < 0) {
            bt_sink_srv_report("[sink][a2dp]handle_data(mgr)(error)-ret: %d, 0x%x\n", ret, med_data);
            #ifndef MTK_DEBUG_LEVEL_INFO
            g_test_fix_warning = (int32_t) med_data;
            #endif
        } else {
            cntx->write_len += payload_size;
            cntx->pkt_cnt += 1;
            #ifdef __BT_AWS_SUPPORT__
            aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
            if ((aws_dev) && (aws_dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT)) {
            #else
            if (0) {
            #endif
                #ifdef __BT_AWS_SUPPORT__
                if (!(cntx->flag & BT_SINK_SRV_MUSIC_FLAG_PLAYING)) {
                    if (cntx->write_len > BT_SINK_SRV_A2DP_AWS_DATA_THRESHHOLD) {
                        cntx->flag |= (BT_SINK_SRV_MUSIC_FLAG_PLAYING);
                        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                        if (bt_sink_srv_cm_get_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db)) {
                            if ((dev_db & 0xf0) == BT_SINK_SRV_A2DP_MAGIC_CODE) {
                                /* use storge volume value */
                                cntx->vol_lev = (dev_db & 0x0f);
                                bt_sink_srv_aws_notify_vol_level(aws_dev->aws_hd, cntx->vol_lev);
                            } else {
                                /* use dedefault volume value and update it */
                                cntx->vol_lev = AUD_VOL_OUT_LEVEL4;
                                dev_db = (BT_SINK_SRV_A2DP_MAGIC_CODE | cntx->vol_lev);
                                bt_sink_srv_cm_set_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db);
                            }
                        }
                        med_hd->set_aws_flag(cntx->aud_id, true);
                        med_hd->aws_plh_init(cntx->aud_id);
                        bt_sink_srv_ami_audio_set_volume(cntx->aud_id, cntx->vol_lev, STREAM_OUT);
                        ret = med_hd->play(cntx->aud_id);
                        time_dur = BT_SINK_SRV_A2DP_AWS_TIMER_DUR;
                        if (cntx->gpt_port == HAL_GPT_MAX_PORT) {
                            if (hal_gpt_init(HAL_GPT_1) == HAL_GPT_STATUS_OK) {
                                cntx->gpt_port = HAL_GPT_1;
                                hal_gpt_register_callback(cntx->gpt_port,
                                    bt_sink_srv_a2dp_gpt_cb, (void *)dev);
                                bt_aws_get_bt_local_time(&sink_loc_play_nclk, &sink_loc_play_nclk_intra);
                                sink_loc_play_nclk_intra += time_dur;
                                gpt_ret = hal_gpt_start_timer_us(cntx->gpt_port, time_dur-2000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
                                aws_notify.op_id = BT_AWS_NOTIFY_ID_READY_TO_PLAY;
                                aws_notify.play_time.play_time = time_dur;
                                aws_notify.play_time.loc_play_nclk = sink_loc_play_nclk;
                                aws_notify.play_time.loc_play_nclk_intra = sink_loc_play_nclk_intra;
                                aws_notify.play_time.bt_local_role = aws_dev->gap_role;
                                err = bt_aws_notify(aws_dev->aws_hd, &aws_notify);
                            } else if (hal_gpt_init(HAL_GPT_2) == HAL_GPT_STATUS_OK) {
                                cntx->gpt_port = HAL_GPT_2;
                                hal_gpt_register_callback(cntx->gpt_port,
                                    bt_sink_srv_a2dp_gpt_cb, (void *)dev);
                                bt_aws_get_bt_local_time(&sink_loc_play_nclk, &sink_loc_play_nclk_intra);
                                    sink_loc_play_nclk_intra += time_dur;
                                gpt_ret = hal_gpt_start_timer_us(cntx->gpt_port, time_dur-2000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
                                aws_notify.op_id = BT_AWS_NOTIFY_ID_READY_TO_PLAY;
                                aws_notify.play_time.play_time = time_dur;
                                aws_notify.play_time.loc_play_nclk = sink_loc_play_nclk;
                                aws_notify.play_time.loc_play_nclk_intra = sink_loc_play_nclk_intra;
                                aws_notify.play_time.bt_local_role = aws_dev->gap_role;
                                err = bt_aws_notify(aws_dev->aws_hd, &aws_notify);
                            } else {
                                err = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
                            }
                        } else {
                            err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
                        }
                        bt_sink_srv_report("[sink][a2dp](mgr)handle_data-(aws)--gpt_ret: %d, gpt_port: %d, err: %d, ret: %d\n", gpt_ret, cntx->gpt_port, err, ret);
                    } else if (cntx->pkt_cnt > BT_SINK_SRV_A2DP_AWS_PKT_THRESHHOLD) {
                        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                        if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_AAC) {
                            cntx->flag |= (BT_SINK_SRV_MUSIC_FLAG_PLAYING);
                            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                            if (bt_sink_srv_cm_get_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db)) {
                                if ((dev_db & 0xf0) == BT_SINK_SRV_A2DP_MAGIC_CODE) {
                                    /* use storge volume value */
                                    cntx->vol_lev = (dev_db & 0x0f);
                                    bt_sink_srv_aws_notify_vol_level(aws_dev->aws_hd, cntx->vol_lev);
                                } else {
                                    /* use dedefault volume value and update it */
                                    cntx->vol_lev = AUD_VOL_OUT_LEVEL4;
                                    dev_db = (BT_SINK_SRV_A2DP_MAGIC_CODE | cntx->vol_lev);
                                    bt_sink_srv_cm_set_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db);
                                }
                            }
                            med_hd->set_aws_flag(cntx->aud_id, true);
                            med_hd->aws_plh_init(cntx->aud_id);
                            bt_sink_srv_ami_audio_set_volume(cntx->aud_id, cntx->vol_lev, STREAM_OUT);
                            ret = med_hd->play(cntx->aud_id);
                            time_dur = BT_SINK_SRV_A2DP_AWS_TIMER_DUR;
                            if (cntx->gpt_port == HAL_GPT_MAX_PORT) {
                                if (hal_gpt_init(HAL_GPT_1) == HAL_GPT_STATUS_OK) {
                                    cntx->gpt_port = HAL_GPT_1;
                                    hal_gpt_register_callback(cntx->gpt_port,
                                        bt_sink_srv_a2dp_gpt_cb, (void *)dev);
                                    bt_aws_get_bt_local_time(&sink_loc_play_nclk, &sink_loc_play_nclk_intra);
                                    sink_loc_play_nclk_intra += time_dur;
                                    gpt_ret = hal_gpt_start_timer_us(cntx->gpt_port, time_dur-2000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
                                    aws_notify.op_id = BT_AWS_NOTIFY_ID_READY_TO_PLAY;
                                    aws_notify.play_time.play_time = time_dur;
                                    aws_notify.play_time.loc_play_nclk = sink_loc_play_nclk;
                                    aws_notify.play_time.loc_play_nclk_intra = sink_loc_play_nclk_intra;
                                    aws_notify.play_time.bt_local_role = aws_dev->gap_role;
                                    err = bt_aws_notify(aws_dev->aws_hd, &aws_notify);
                                } else if (hal_gpt_init(HAL_GPT_2) == HAL_GPT_STATUS_OK) {
                                    cntx->gpt_port = HAL_GPT_2;
                                    hal_gpt_register_callback(cntx->gpt_port,
                                        bt_sink_srv_a2dp_gpt_cb, (void *)dev);
                                    bt_aws_get_bt_local_time(&sink_loc_play_nclk, &sink_loc_play_nclk_intra);

                                    sink_loc_play_nclk_intra += time_dur;
                                    gpt_ret = hal_gpt_start_timer_us(cntx->gpt_port, time_dur-2000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
                                    aws_notify.op_id = BT_AWS_NOTIFY_ID_READY_TO_PLAY;
                                    aws_notify.play_time.play_time = time_dur;
                                    aws_notify.play_time.loc_play_nclk = sink_loc_play_nclk;
                                    aws_notify.play_time.loc_play_nclk_intra = sink_loc_play_nclk_intra;
                                    aws_notify.play_time.bt_local_role = aws_dev->gap_role;
                                    err = bt_aws_notify(aws_dev->aws_hd, &aws_notify);
                                } else {
                                    err = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
                                }
                            } else {
                                err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
                            }
                            bt_sink_srv_report("[sink][a2dp](mgr)handle_data-(aws aac)--gpt_ret: %d, gpt_port: %d, err: %d, ret: %d\n", gpt_ret, cntx->gpt_port, err, ret);
                        }
                    }
                }
                #endif /* __BT_AWS_SUPPORT__ */
            #ifdef __BT_AWS_SUPPORT__
            }
            #else
            }
            #endif
             else {
                if (!(cntx->flag & BT_SINK_SRV_MUSIC_FLAG_PLAYING)) {
                    if (cntx->write_len > BT_SINK_SRV_A2DP_DATA_THRESHHOLD) {
                        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                        if (bt_sink_srv_cm_get_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db)) {
                            if ((dev_db & 0xf0) == BT_SINK_SRV_A2DP_MAGIC_CODE) {
                                /* use storge volume value */
                                cntx->vol_lev = (dev_db & 0x0f);
                            } else {
                                /* use dedefault volume value and update it */
                                cntx->vol_lev = AUD_VOL_OUT_LEVEL4;
                                dev_db = (BT_SINK_SRV_A2DP_MAGIC_CODE | cntx->vol_lev);
                                bt_sink_srv_cm_set_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db);
                            }
                        }
                        bt_sink_srv_ami_audio_set_volume(cntx->aud_id, cntx->vol_lev, STREAM_OUT);
                        ret = med_hd->play(cntx->aud_id);
                        bt_sink_srv_report("[sink][a2dp](mgr)handle_data-start play--ret: 0x%x\n", ret);
                        if (ret == BT_CODEC_MEDIA_STATUS_OK) {
                            cntx->flag |= (BT_SINK_SRV_MUSIC_FLAG_PLAYING);
                        }

                    } else if (cntx->pkt_cnt > BT_SINK_SRV_A2DP_PKT_THRESHHOLD) {
                        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                        if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_AAC) {
                            cntx->flag |= (BT_SINK_SRV_MUSIC_FLAG_PLAYING);
                            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                            if (bt_sink_srv_cm_get_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db)) {
                                if ((dev_db & 0xf0) == BT_SINK_SRV_A2DP_MAGIC_CODE) {
                                    /* use storge volume value */
                                    cntx->vol_lev = (dev_db & 0x0f);
                                } else {
                                    /* use dedefault volume value and update it */
                                    cntx->vol_lev = AUD_VOL_OUT_LEVEL4;
                                    dev_db = (BT_SINK_SRV_A2DP_MAGIC_CODE | cntx->vol_lev);
                                    bt_sink_srv_cm_set_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db);
                                }
                            }
                            bt_sink_srv_ami_audio_set_volume(cntx->aud_id, cntx->vol_lev, STREAM_OUT);
                            ret = med_hd->play(cntx->aud_id);
                            bt_sink_srv_report("[sink][a2dp](mgr)handle_data-AAC start play--ret: 0x%x, cnt: %d, len: %d\n", ret, cntx->pkt_cnt, cntx->write_len);
                        }
                    }
                }
            }
        }
#else
        free_len = med_hd->get_free_space(cntx->aud_id);
        if (free_len <= payload_size) {
            /* error */
            bt_sink_srv_report("[sink][a2dp]handle_data(error)-free_len: %d, pay_size: %d\n", free_len, payload_size);
        } else {
            med_hd->get_write_buffer(cntx->aud_id, &ring_buf, &write_len);
            if (write_len < payload_size) {
                /* first part */
                memcpy(ring_buf, med_data, write_len);
                med_hd->write_data_done(cntx->aud_id, write_len);
                cur_len = write_len;

                /* second part */
                med_hd->get_write_buffer(cntx->aud_id, &ring_buf, &write_len);
                memcpy(ring_buf, med_data + cur_len, payload_size - cur_len);
                //med_hd->write_data_done(cntx->aud_id, write_len);
                med_hd->write_data_done(cntx->aud_id, payload_size - cur_len);
            } else {
                memcpy(ring_buf, med_data, payload_size);
                med_hd->write_data_done(cntx->aud_id, payload_size);
            }

            med_hd->finish_write_data(cntx->aud_id);
            cntx->write_len += payload_size;
            cntx->pkt_cnt += 1;

            aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
            if ((aws_dev) && (aws_dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT)) {
                #ifdef __BT_AWS_SUPPORT__
                if (!(cntx->flag & BT_SINK_SRV_MUSIC_FLAG_PLAYING)) {
                    if (cntx->write_len > BT_SINK_SRV_A2DP_AWS_DATA_THRESHHOLD) {
                        cntx->flag |= (BT_SINK_SRV_MUSIC_FLAG_PLAYING);
                        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                        if (bt_sink_srv_cm_get_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db)) {
                            if ((dev_db & 0xf0) == BT_SINK_SRV_A2DP_MAGIC_CODE) {
                                /* use storge volume value */
                                cntx->vol_lev = (dev_db & 0x0f);
                                bt_sink_srv_aws_notify_vol_level(aws_dev->aws_hd, cntx->vol_lev);
                            } else {
                                /* use dedefault volume value and update it */
                                cntx->vol_lev = AUD_VOL_OUT_LEVEL4;
                                dev_db = (BT_SINK_SRV_A2DP_MAGIC_CODE | cntx->vol_lev);
                                bt_sink_srv_cm_set_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db);
                            }
                        }
                        med_hd->set_aws_flag(cntx->aud_id, true);
                        med_hd->aws_plh_init(cntx->aud_id);
                        bt_sink_srv_ami_audio_set_volume(cntx->aud_id, cntx->vol_lev, STREAM_OUT);
                        ret = med_hd->play(cntx->aud_id);
                        time_dur = BT_SINK_SRV_A2DP_AWS_TIMER_DUR;
                        if (cntx->gpt_port == HAL_GPT_MAX_PORT) {
                            if (hal_gpt_init(HAL_GPT_1) == HAL_GPT_STATUS_OK) {
                                cntx->gpt_port = HAL_GPT_1;
                                hal_gpt_register_callback(cntx->gpt_port,
                                    bt_sink_srv_a2dp_gpt_cb, (void *)dev);
                                bt_aws_get_bt_local_time(&sink_loc_play_nclk, &sink_loc_play_nclk_intra);
                                    sink_loc_play_nclk_intra += time_dur;
                                gpt_ret = hal_gpt_start_timer_us(cntx->gpt_port, time_dur-2000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
                                aws_notify.op_id = BT_AWS_NOTIFY_ID_READY_TO_PLAY;
                                aws_notify.play_time.play_time = time_dur;
                                aws_notify.play_time.loc_play_nclk = sink_loc_play_nclk;
                                aws_notify.play_time.loc_play_nclk_intra = sink_loc_play_nclk_intra;
                                aws_notify.play_time.bt_local_role = aws_dev->gap_role;
                                err = bt_aws_notify(aws_dev->aws_hd, &aws_notify);
                            } else if (hal_gpt_init(HAL_GPT_2) == HAL_GPT_STATUS_OK) {
                                cntx->gpt_port = HAL_GPT_2;
                                hal_gpt_register_callback(cntx->gpt_port,
                                    bt_sink_srv_a2dp_gpt_cb, (void *)dev);
                                bt_aws_get_bt_local_time(&sink_loc_play_nclk, &sink_loc_play_nclk_intra);
                                    sink_loc_play_nclk_intra += time_dur;
                                gpt_ret = hal_gpt_start_timer_us(cntx->gpt_port, time_dur-2000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
                                aws_notify.op_id = BT_AWS_NOTIFY_ID_READY_TO_PLAY;
                                aws_notify.play_time.play_time = time_dur;
                                aws_notify.play_time.loc_play_nclk = sink_loc_play_nclk;
                                aws_notify.play_time.loc_play_nclk_intra = sink_loc_play_nclk_intra;
                                aws_notify.play_time.bt_local_role = aws_dev->gap_role;
                                err = bt_aws_notify(aws_dev->aws_hd, &aws_notify);
                            } else {
                                err = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
                            }
                        } else {
                            err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
                        }
                        bt_sink_srv_report("[sink][a2dp]handle_data-(aws)--gpt_ret: %d, gpt_port: %d, err: %d, ret: %d\n", gpt_ret, cntx->gpt_port, err, ret);
                    }  else if (cntx->pkt_cnt > BT_SINK_SRV_A2DP_AWS_PKT_THRESHHOLD) {
                        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                        if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_AAC) {
                            cntx->flag |= (BT_SINK_SRV_MUSIC_FLAG_PLAYING);
                            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                            if (bt_sink_srv_cm_get_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db)) {
                                if ((dev_db & 0xf0) == BT_SINK_SRV_A2DP_MAGIC_CODE) {
                                    /* use storge volume value */
                                    cntx->vol_lev = (dev_db & 0x0f);
                                    bt_sink_srv_aws_notify_vol_level(aws_dev->aws_hd, cntx->vol_lev);
                                } else {
                                    /* use dedefault volume value and update it */
                                    cntx->vol_lev = AUD_VOL_OUT_LEVEL4;
                                    dev_db = (BT_SINK_SRV_A2DP_MAGIC_CODE | cntx->vol_lev);
                                    bt_sink_srv_cm_set_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db);
                                }
                            }
                            med_hd->set_aws_flag(cntx->aud_id, true);
                            med_hd->aws_plh_init(cntx->aud_id);
                            bt_sink_srv_ami_audio_set_volume(cntx->aud_id, cntx->vol_lev, STREAM_OUT);
                            ret = med_hd->play(cntx->aud_id);
                            time_dur = BT_SINK_SRV_A2DP_AWS_TIMER_DUR;
                            if (cntx->gpt_port == HAL_GPT_MAX_PORT) {
                                if (hal_gpt_init(HAL_GPT_1) == HAL_GPT_STATUS_OK) {
                                    cntx->gpt_port = HAL_GPT_1;
                                    hal_gpt_register_callback(cntx->gpt_port,
                                        bt_sink_srv_a2dp_gpt_cb, (void *)dev);
                                    bt_aws_get_bt_local_time(&sink_loc_play_nclk, &sink_loc_play_nclk_intra);
                                    sink_loc_play_nclk_intra += time_dur;
                                    gpt_ret = hal_gpt_start_timer_us(cntx->gpt_port, time_dur-2000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
                                    aws_notify.op_id = BT_AWS_NOTIFY_ID_READY_TO_PLAY;
                                    aws_notify.play_time.play_time = time_dur;
                                    aws_notify.play_time.loc_play_nclk = sink_loc_play_nclk;
                                    aws_notify.play_time.loc_play_nclk_intra = sink_loc_play_nclk_intra;
                                    aws_notify.play_time.bt_local_role = aws_dev->gap_role;
                                    err = bt_aws_notify(aws_dev->aws_hd, &aws_notify);
                                } else if (hal_gpt_init(HAL_GPT_2) == HAL_GPT_STATUS_OK) {
                                    cntx->gpt_port = HAL_GPT_2;
                                    hal_gpt_register_callback(cntx->gpt_port,
                                        bt_sink_srv_a2dp_gpt_cb, (void *)dev);
                                    bt_aws_get_bt_local_time(&sink_loc_play_nclk, &sink_loc_play_nclk_intra);
                                    sink_loc_play_nclk_intra += time_dur;                                    
                                    gpt_ret = hal_gpt_start_timer_us(cntx->gpt_port, time_dur-2000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
                                    aws_notify.op_id = BT_AWS_NOTIFY_ID_READY_TO_PLAY;
                                    aws_notify.play_time.play_time = time_dur;
                                    aws_notify.play_time.loc_play_nclk = sink_loc_play_nclk;
                                    aws_notify.play_time.loc_play_nclk_intra = sink_loc_play_nclk_intra;
                                    aws_notify.play_time.bt_local_role = aws_dev->gap_role;
                                    err = bt_aws_notify(aws_dev->aws_hd, &aws_notify);
                                } else {
                                    err = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
                                }
                            } else {
                                err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
                            }
                            bt_sink_srv_report("[sink][a2dp]handle_data-(aws aac)--gpt_ret: %d, gpt_port: %d, err: %d, ret: %d\n", gpt_ret, cntx->gpt_port, err, ret);
                        }
                    }
                }
                #endif /* __BT_AWS_SUPPORT__ */
            } else {
                if (!(cntx->flag & BT_SINK_SRV_MUSIC_FLAG_PLAYING)) {
                    if (cntx->write_len > BT_SINK_SRV_A2DP_DATA_THRESHHOLD) {
                        cntx->flag |= (BT_SINK_SRV_MUSIC_FLAG_PLAYING);
                        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                        if (bt_sink_srv_cm_get_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db)) {
                            if ((dev_db & 0xf0) == BT_SINK_SRV_A2DP_MAGIC_CODE) {
                                /* use storge volume value */
                                cntx->vol_lev = (dev_db & 0x0f);
                            } else {
                                /* use dedefault volume value and update it */
                                cntx->vol_lev = AUD_VOL_OUT_LEVEL4;
                                dev_db = (BT_SINK_SRV_A2DP_MAGIC_CODE | cntx->vol_lev);
                                bt_sink_srv_cm_set_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db);
                            }
                        }
                        //ami_ret = bt_sink_srv_ami_audio_set_volume(cntx->aud_id, am_volume);
                        bt_sink_srv_ami_audio_set_volume(cntx->aud_id, cntx->vol_lev, STREAM_OUT);
                        ret = med_hd->play(cntx->aud_id);
                        //hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HEADSET);
                        //*(volatile uint16_t *)(0xA21C00B4) = 0x7E31;
                        bt_sink_srv_report("[sink][a2dp]handle_data-start play--ret: 0x%x\n", ret);
                    }  else if (cntx->pkt_cnt > BT_SINK_SRV_A2DP_PKT_THRESHHOLD) {
                        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                        if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_AAC) {
                            cntx->flag |= (BT_SINK_SRV_MUSIC_FLAG_PLAYING);
                            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
                            if (bt_sink_srv_cm_get_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db)) {
                                if ((dev_db & 0xf0) == BT_SINK_SRV_A2DP_MAGIC_CODE) {
                                    /* use storge volume value */
                                    cntx->vol_lev = (dev_db & 0x0f);
                                } else {
                                    /* use dedefault volume value and update it */
                                    cntx->vol_lev = AUD_VOL_OUT_LEVEL4;
                                    dev_db = (BT_SINK_SRV_A2DP_MAGIC_CODE | cntx->vol_lev);
                                    bt_sink_srv_cm_set_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db);
                                }
                            }
                            bt_sink_srv_ami_audio_set_volume(cntx->aud_id, cntx->vol_lev, STREAM_OUT);
                            ret = med_hd->play(cntx->aud_id);
                            bt_sink_srv_report("[sink][a2dp]handle_data-AAC start play--ret: 0x%x\n", ret);
                        }
                    }
                }
            }
        }
#endif /* __BT_SINK_SRV_AM_MED_LIST_SUPPORT__ */
    } else {
        /* drop media data */
        bt_sink_srv_report("[sink][a2dp]drop_data-state: %d, c_hd: %d, d_hd: %d, flag: 0x%x\n",
            cntx->state, cntx->a2dp_hd, data_ind->handle, cntx->flag);
    }

    if ((bt_sink_srv_a2dp_data_rece_cnt < BT_SINK_SRV_A2DP_DATA_RECE_CNT)) {
        //a2dp_streaming.handle = data_ind->handle;
        /* notify a2dp event */
        //a2dp_event.event = BT_SINK_SRV_A2DP_EVENT_STREAMING;
        //a2dp_event.param = &a2dp_streaming;

        bt_sink_srv_state_set(BT_SINK_SRV_STATE_STREAMING);
    }

    return ret;
}


bt_sink_srv_status_t bt_sink_srv_a2dp_action_handler(bt_sink_srv_action_t action, void *param)
{
    bt_status_t ret = 0;
    bt_bd_addr_t *dev_addr = NULL;
    uint32_t hd = 0;
    int32_t err = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    bt_sink_srv_profile_connection_action_t *conn_info = NULL;
#ifdef __BT_AWS_SUPPORT__
    bt_sink_srv_music_device_t *aws_dev = NULL;
#endif

    bt_sink_srv_report("[sink][a2dp]process_a2dp_action[s]-action: 0x%x, base: 0x%x\n", action, BT_SINK_SRV_ACTION_A2DP_START);
    cntx = bt_sink_srv_music_get_context();

    switch (action) {
        case BT_SINK_SRV_ACTION_PROFILE_INIT: {
            bt_sink_srv_a2dp_init();
            break;
        }

        case BT_SINK_SRV_ACTION_PROFILE_DEINIT: {
            if (cntx->aud_id != BT_SINK_SRV_INVALID_AID) {
#ifdef __BT_SINK_SRV_AM_SUPPORT__
                bt_sink_srv_ami_audio_close(cntx->aud_id);
#endif /* __BT_SINK_SRV_AM_SUPPORT__ */
                cntx->aud_id = BT_SINK_SRV_INVALID_AID;
            }
            break;
        }

        case BT_SINK_SRV_ACTION_PROFILE_CONNECT: {
            conn_info = (bt_sink_srv_profile_connection_action_t *)param;
            dev_addr = &(conn_info->bt_addr);
            if (conn_info->profile_connection_mask & BT_SINK_SRV_PROFILE_CONNECTION_TYPE_A2DP_SINK) {
                ret = bt_a2dp_connect(&hd, (const bt_bd_addr_t *)dev_addr, BT_A2DP_SINK);
                if (BT_STATUS_SUCCESS == ret) {
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR, dev_addr);
                    if (dev) {
                        dev->a2dp_hd = hd;
                        dev->role = BT_A2DP_SINK;
                        /* unexpected, need handle */
                        err = -100;
                    } else {
                        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, NULL);
                        if (dev) {
                            dev->a2dp_hd = hd;
                            dev->role = BT_A2DP_SINK;
                            bt_sink_srv_memcpy(&(dev->dev_addr), dev_addr, sizeof(bt_bd_addr_t));
                            /* normal case */
                            err = 1;
                        } else {
                            /* unexpected, no unused device, please check log */
                            err = -101;
                        }
                    }
                }
        #ifdef MTK_BT_COMMAND_ENABLE
            bt_sink_srv_cm_profile_state_change_notify(&(dev->dev_addr), 
                BT_SINK_SRV_CM_PROFILE_A2DP, BT_SINK_SRV_CM_STATE_CONNECTING);
        #endif
            }
            break;
        }

        case BT_SINK_SRV_ACTION_PROFILE_DISCONNECT: {
            conn_info = (bt_sink_srv_profile_connection_action_t *)param;
            dev_addr = &(conn_info->bt_addr);
            if (conn_info->profile_connection_mask & BT_SINK_SRV_PROFILE_CONNECTION_TYPE_A2DP_SINK) {
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR, (void *)dev_addr);
                if ((dev) && (dev->conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT)) {
                    ret = bt_a2dp_disconnect(dev->a2dp_hd);
                } else {
                    err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
                }
            }
            break;
        }

        case BT_SINK_SRV_ACTION_VOLUME_UP: {
            bt_sink_srv_a2dp_change_volume(VOLUME_UP);
            #ifdef __BT_AWS_SUPPORT__
            aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
            if (aws_dev) {
                bt_sink_srv_aws_notify_vol_up(aws_dev->aws_hd);
            }
            #endif
            break;
        }

        case BT_SINK_SRV_ACTION_VOLUME_DOWN: {
            bt_sink_srv_a2dp_change_volume(VOLUME_DOWN);
            #ifdef __BT_AWS_SUPPORT__
            aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
            if (aws_dev) {
                bt_sink_srv_aws_notify_vol_down(aws_dev->aws_hd);
            }
            #endif
            break;
        }

        default:
            break;
    }

    bt_sink_srv_report("[sink][a2dp]process_a2dp_action[e]-ret: %d, err: %d\n", ret, err);
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
    return BT_SINK_SRV_STATUS_SUCCESS;
}


bt_status_t bt_sink_srv_a2dp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    int32_t ret = 0;

    if ((bt_sink_srv_a2dp_data_rece_cnt < BT_SINK_SRV_A2DP_DATA_RECE_CNT) || (msg != BT_A2DP_STREAMING_RECEIVED_IND)) {
        bt_sink_srv_report("[sink][a2dp]common_hdr[s]-msg: 0x%x, status: %d\n", msg, status);
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
            bt_sink_srv_a2dp_data_rece_cnt = 0;

            ret = bt_sink_srv_a2dp_handle_disconnect_cnf(disconn_cnf);
            break;
        }

        case BT_A2DP_DISCONNECT_IND: {
            bt_a2dp_disconnect_ind_t *disconn_ind = (bt_a2dp_disconnect_ind_t *)buffer;
            bt_sink_srv_a2dp_data_rece_cnt = 0;

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
            bt_sink_srv_a2dp_data_rece_cnt = 0;

            bt_sink_srv_report("[sink][a2dp]start_ind(comm_bk)--buff: 0x%x, cap: 0x%x\n", buffer, start_ind->codec_cap);

            if (start_ind->codec_cap->type == BT_A2DP_CODEC_SBC) {
                bt_sink_srv_report("[sink][a2dp]start_ind(comm_bk_sbc)--hd: 0x%x, type: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d\r\n",
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
                bt_sink_srv_report("[sink][a2dp]start_ind(comm_bk_aac)--hd: 0x%x, type: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d, 8: %d, 9: %d\r\n",
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

            bt_sink_srv_a2dp_data_rece_cnt = 0;

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
            ++bt_sink_srv_a2dp_data_rece_cnt;

            bt_sink_srv_a2dp_rece_ind = (bt_a2dp_streaming_received_ind_t *)buffer;
            bt_sink_srv_a2dp_pkt = (bt_hci_packet_t *)(data_ind->data_node);

            pkt = (bt_hci_packet_t *)(data_ind->data_node);
            if (pkt->ref_count != 1) {
            //if (pkt->ref_count != 1 || pkt->indicator != 2) {
                /* drop this packet */
                bt_sink_srv_report("[sink][a2dp]comm_bk(rece_ind)--pkt: 0x%x, ref: %d, len: %d, offset: %d, size: %d, data_ind: 0x%x\n",
                                   pkt, pkt->ref_count, pkt->packet_length,
                                   pkt->offset, sizeof(bt_hci_packet_t), data_ind);
#ifdef __BT_SINK_SRV_A2DP_LIST_DEBUG__
                bt_sink_srv_assert(0);
#endif
                break;
            }

            if (bt_sink_srv_am_med_mgr_node_is_exist((void *)(data_ind->data_node))) {
                /* drop this packet */
                bt_sink_srv_report("[sink][a2dp]comm_bk(rece_ind exist)--pkt: 0x%x, ref: %d, len: %d, offset: %d, size: %d, data_ind: 0x%x\n",
                                   pkt, pkt->ref_count, pkt->packet_length,
                                   pkt->offset, sizeof(bt_hci_packet_t), data_ind);
#ifdef __BT_SINK_SRV_A2DP_LIST_DEBUG__
                bt_sink_srv_assert(0);
#endif
            }
#if 1
            ret = bt_sink_srv_a2dp_handle_data_received_ind(data_ind);
#else
            if ((bt_sink_srv_a2dp_data_rece_cnt < BT_SINK_SRV_A2DP_DATA_RECE_CNT)) {
                a2dp_streaming.conn_id = data_ind->conn_id;
                /* notify a2dp event */
                a2dp_event.event = BT_SINK_SRV_A2DP_EVENT_STREAMING;
                a2dp_event.param = &a2dp_streaming;
                bufferbuffer
                bt_sink_srv_state_set(BT_SINK_SRV_STATE_STREAMING);
            }
#endif
            break;
        }

        default:
            break;
    }

    if ((bt_sink_srv_a2dp_data_rece_cnt < BT_SINK_SRV_A2DP_DATA_RECE_CNT) || (msg != BT_A2DP_STREAMING_RECEIVED_IND)) {
        bt_sink_srv_report("[sink][a2dp]common_hdr[e]-ret: %d\n", ret);
    }

    return ret;
}


#ifdef __BT_AWS_SUPPORT__
static void bt_sink_srv_a2dp_gpt_cb(void *user_data)
{
    bt_sink_srv_music_context_t *cntx = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    SinkProfileA2dp *a2dp = NULL;
    int32_t ret = 0, err = 0;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;

    cntx = bt_sink_srv_music_get_context();

    dev = (bt_sink_srv_music_device_t *)user_data;
    /* reset gpt */
    hal_gpt_stop_timer(cntx->gpt_port);
    hal_gpt_deinit(cntx->gpt_port);
    cntx->gpt_port = HAL_GPT_MAX_PORT;
    uint32_t nclk;
    uint32_t nclk_intra;
    int64_t curr_clock;
    int64_t play_clock;
    play_clock = sink_loc_play_nclk * 312.5 + sink_loc_play_nclk_intra;

    a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);

    if (a2dp) {
        med_hd = &(a2dp->med_hd);
        //ret = med_hd->play(cntx->aud_id);
        __disable_irq();
        //hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_HIGH);
        while(1)
        {                    
            bt_aws_get_bt_local_time(&nclk,&nclk_intra);

            curr_clock = nclk * 312 + (nclk >> 1) + nclk_intra;
            if (curr_clock >= play_clock)
            {
                /*uint8_t log_buff[512];              
                sprintf((char *)log_buff, "curr native clock, nclk:%x, nclk_intra:%x, play_clock:%x\n", nclk, nclk_intra, play_clock);
                atci_send_data(log_buff, strlen(log_buff));*/
                break;
            }
        }        
        
        ret = med_hd->set_aws_initial_sync(cntx->aud_id);
        __enable_irq();
        bt_sink_srv_audio_clock_init();
        bt_sink_srv_report("[sink][a2dp]gpt_cb-start play--ret: %d\n", ret);
    } else {
        err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

    bt_sink_srv_report("[sink][a2dp]gpt_cb[e]-ret: %d, err: %d\n", ret, err);
}


static void bt_sink_srv_a2dp_start_aws_resync(void)
{
    int32_t ret = 0, err = 0;
    bt_sink_srv_music_device_t *aws_dev = NULL, *dev = NULL;
    SinkProfileA2dp *a2dp = NULL;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;


    cntx = bt_sink_srv_music_get_context();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
    a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
    med_hd = &(a2dp->med_hd);
    aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);

    if ((aws_dev) && (aws_dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT)) {
        /* aws connection, need resync */
        med_hd->set_aws_flag(cntx->aud_id, true);
        med_hd->aws_plh_init(cntx->aud_id);
        cntx->flag &= ~(BT_SINK_SRV_MUSIC_FLAG_PLAYING);
        cntx->write_len = 0;
        cntx->pkt_cnt = 0;
        ret = bt_aws_resync_request(aws_dev->aws_hd);
        err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_2ND;
    } else {
        ret = med_hd->play(cntx->aud_id);
        err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_1ST;
    }

    bt_sink_srv_report("[sink][a2dp]aws_resync-ret: %d, err: %d\n", ret, err);
}
#endif


int32_t bt_sink_srv_a2dp_change_volume(uint8_t type)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    bt_sink_srv_am_id_t ami_ret = 0;
    uint8_t vol = AUD_VOL_OUT_LEVEL0;
    bt_sink_srv_music_context_t *cntx = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    uint8_t dev_db = 0;

    cntx = bt_sink_srv_music_get_context();

    vol = cntx->vol_lev;
    /* volume up */
    if (VOLUME_UP == type) {
       if (vol < AUD_VOL_OUT_LEVEL15) {
           vol = vol + 1;
       } else {
           ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
       }
    } else if (VOLUME_DOWN == type) {
        if (vol > AUD_VOL_OUT_LEVEL0) {
            vol = vol - 1;
        } else {
            ret = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
        }
    }

    if (vol != cntx->vol_lev) {
        ami_ret = bt_sink_srv_ami_audio_set_volume(cntx->aud_id, vol, STREAM_OUT);
        cntx->vol_lev = vol;
        dev_db = (BT_SINK_SRV_A2DP_MAGIC_CODE | cntx->vol_lev);
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
        bt_sink_srv_cm_set_profile_data(&(dev->dev_addr), BT_SINK_SRV_UUID_A2DP, &dev_db);
    }

    bt_sink_srv_report("[sink][a2dp]change_volume-ami_ret: %d, ret: %d, vol: %d\n",
        ami_ret, ret, cntx->vol_lev);

    return ret;
}

#ifdef __MUSIC_MODE_SWITCH_SUPPORT__

int32_t bt_sink_srv_a2dp_mode_check(bt_sink_srv_music_mode_t mode)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    bool codec_opened_flag;
    bt_sink_srv_music_context_t *cntx = NULL;
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
#ifdef __BT_AWS_SUPPORT__
    bt_sink_srv_music_device_t *aws_dev = NULL;
#endif

    cntx = bt_sink_srv_music_get_context();
    a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(cntx->a2dp_hd)));
    codec_opened_flag = bt_sink_srv_a2dp_check_dev_flag(a2dp_dev, BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC);
 
    bt_sink_srv_report("[sink][a2dp] mode check %d", mode);
#if 0
    if (cntx->a2dp_hd == BT_SINK_SRV_MUSIC_INVALID_HD) {
        bt_sink_srv_report("[sink][a2dp] a2dp mode check, no a2dp");
            return BT_STATUS_SUCCESS;
    }
#endif
    if (mode == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
        bt_sink_srv_report("[sink][a2dp] codec open flag:%x", codec_opened_flag);
        if (true == codec_opened_flag) {
#ifdef __BT_AWS_SUPPORT__
            aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
            if (aws_dev) {
                if (bt_aws_suspend_streaming_request(aws_dev->aws_hd) == BT_STATUS_SUCCESS) {
                    cntx->flag |= BT_SINK_SRV_MUSIC_FLAG_AWS_MODE_CHANGING;
                    bt_sink_srv_report("[sink][a2dp] send aws suspend--ret:0x%x", ret);
                }
            }
#endif
            if (a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_STREAMING) {
                // need to send pause via, close codec, set discard to true. <bt-->mp3 just>
                ret = bt_sink_srv_avrcp_stop_music(a2dp_dev->avrcp_hd);
            }
            // close codec.
            ret = bt_sink_srv_stop_close_codec();
            bt_sink_srv_report("[sink][a2dp] cur in bt mode, stop close codec--ret:0x%08X", ret);
            // set discard to true
            bt_sink_srv_a2dp_set_dev_flag(a2dp_dev, BT_SINK_SRV_MUSIC_FALG_A2DP_DISCARD_DATA);
            bt_sink_srv_a2dp_clear_dev_flag(a2dp_dev, BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC);
        } else {
#ifdef __BT_AWS_SUPPORT__
            aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
            if (aws_dev && aws_dev->aws_role != BT_AWS_ROLE_SINK){
                ret = bt_sink_srv_aws_set_mode_change(aws_dev->aws_hd);
                bt_sink_srv_report("[Sink][A2DP] send mode switch--new: %d ret: %d\n", mode, ret);
            }
#endif
        }
        return BT_STATUS_SUCCESS;
    } else if (mode == BT_SINK_SRV_MODE_BT_MUSIC) {
        if (false == codec_opened_flag) {
            // need to send open codec, set open codec to true. <mp3-->bt just>
            bt_sink_srv_report("[sink][a2dp] in bt mode, open codec, a2dp_hd:0x%08X, pkt_num:%d", cntx->a2dp_hd);

            ret = bt_sink_srv_open_a2dp_codec();
            bt_sink_srv_report("[sink][a2dp] in bt mode, open codec--ret:0x%08X", ret);
            bt_sink_srv_a2dp_set_dev_flag(a2dp_dev, BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC);
        }
        if (false == codec_opened_flag) {
            bt_sink_srv_a2dp_clear_dev_flag(a2dp_dev, BT_SINK_SRV_MUSIC_FALG_A2DP_DISCARD_DATA);
        }
    }
    return ret;
}
#endif

