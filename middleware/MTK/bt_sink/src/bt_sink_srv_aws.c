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

#ifdef __BT_AWS_SUPPORT__
#include "bt_sink_srv_aws.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_action.h"
#include "bt_gap.h"
#include "bt_sink_srv_utils.h"
#include "hal_audio.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_codec.h"
#include "bt_sink_srv_am_med_mgr.h"
#include "bt_aws.h"
#include "bt_a2dp.h"
#include "bt_sink_srv_avrcp.h"
#include "hal_audio.h"
#include "bt_sink_srv_audio_sync.h"
#include "bt_sink_srv_a2dp.h"


#define BT_SINK_SRV_AWS_DATA_RECE_CNT          (10)


SinkProfileAws sink_aws[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];

static uint32_t bt_sink_srv_aws_data_rece_cnt = 0;

bt_aws_streaming_received_ind_t *bt_sink_srv_aws_rece_ind = NULL;

bt_hci_packet_t *bt_sink_srv_aws_pkt = NULL;

uint32_t sink_loc_play_nclk;
uint32_t sink_loc_play_nclk_intra;

const static uint8_t BT_SINK_SRV_AWS_AAC_SYNC_HEAD[BT_SINK_SRV_AAC_ADTS_LENGTH] = {0xFF, 0xF9, 0x40, 0x20, 0x00, 0x1F, 0xFC};

/* static function declare */
static void bt_sink_srv_aws_gpt_cb(void *user_data);

static void bt_sink_srv_aws_start_resync(void);

static void bt_sink_srv_aws_init()
{
    uint32_t i;
    int8_t ori_aid = 0;
    bt_sink_srv_music_context_t *cntx = NULL;

    for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
        sink_aws[i].profile.uuid = BT_SINK_SRV_UUID_AWS;
        sink_aws[i].profile.state = BT_SINK_SRV_PROFILE_STATE_DISCONNECTED;
    }

    cntx = bt_sink_srv_music_get_context();
    ori_aid = cntx->aws_aid;
    if (cntx->aws_aid != BT_SINK_SRV_INVALID_AID) {
#ifdef __BT_SINK_SRV_AM_SUPPORT__
        bt_sink_srv_ami_audio_close(cntx->aws_aid);
#endif /* __BT_SINK_SRV_AM_SUPPORT__ */
        cntx->aws_aid = BT_SINK_SRV_INVALID_AID;
    }
#ifdef __BT_SINK_SRV_AM_SUPPORT__
    cntx->aws_aid = bt_sink_srv_ami_audio_open(AUD_MIDDLE, bt_sink_srv_aws_ami_hdr);
#endif /* __BT_SINK_SRV_AM_SUPPORT__ */
    bt_sink_srv_report("[sink][aws]init-aws_aid: %d, ori: %d\r\n",
                       cntx->aws_aid, ori_aid);
}


static void bt_sink_srv_aws_fill_aac_header(uint8_t *data, uint32_t len, bt_aws_codec_capability_t *codec)
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

    memcpy(data, BT_SINK_SRV_AWS_AAC_SYNC_HEAD, BT_SINK_SRV_AAC_ADTS_LENGTH);

    data[2] |= (freqIndex << 2);
    data[3] |= (channelNum << 6) | (len >> 11);
    data[4] = (len >> 3) & 0xFF;
    data[5] |= (len & 0x07) << 5;
}


static void bt_sink_srv_aws_fetch_media_header(uint8_t *data, bt_sink_srv_aws_media_header_t *header)
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


static void *bt_sink_srv_aws_uppack_media_data(uint32_t handle, void *data, uint32_t data_len, uint32_t *payload_len)
{
    SinkProfileAws *aws = NULL;
    void *p_in = NULL;
    uint8_t *med = NULL;
    bt_sink_srv_music_device_t *dev = NULL;

    p_in = (uint8_t *)data + BT_SINK_SRV_MEDIA_PKT_HEADER_LEN;
    med = p_in;
    *payload_len = data_len - BT_SINK_SRV_MEDIA_PKT_HEADER_LEN;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&handle));
    aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
    // search the payload start pointer for AAC.
    if (aws->codec.type == BT_AWS_CODEC_TYPE_AAC) {
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
        bt_sink_srv_aws_fill_aac_header(med, *payload_len, &(aws->codec));
    } else if (aws->codec.type == BT_AWS_CODEC_TYPE_SBC) {
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


void bt_sink_srv_aws_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *parm)
{
    SinkProfileAws *aws = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    int32_t ret = 0;
    int32_t err_code = 0;
    bt_sink_srv_am_media_handle_t *media = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_device_t *aws_dev = NULL;

    bt_sink_srv_am_files_media_handle_t *f_media = NULL;
    uint8_t cmd_buf[16] = {0};

    cntx = bt_sink_srv_music_get_context();

    if ((cntx->aws_aid == aud_id && (msg_id == AUD_AWS_A2DP_PROC_IND)&&
        (sub_msg == AUD_STREAM_EVENT_DATA_REQ || AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW == sub_msg)) ||
        msg_id == AUD_FILE_PROC_IND) {
        // drop
        ;
    } else {
        bt_sink_srv_report("[sink][aws]ami_hdr[s]-aid: %d, aud_id: %d, msg_id: %d, sub_msg: %d, 2nd: 0x%x\n",
                           cntx->aws_aid, aud_id, msg_id, sub_msg, sub_msg);
    }

    if (cntx->aws_aid == aud_id) {
        switch (msg_id) {
            case AUD_FILE_OPEN_CODEC:
                if (cntx->state == BT_SINK_SRV_MUSIC_AMI_AWS_PLAY) {
                    f_media = (bt_sink_srv_am_files_media_handle_t *)parm;
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(cntx->aws_hd)));
                    aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
                    if (aws) {
                        memcpy(&(aws->f_med_hd), f_media, sizeof(bt_sink_srv_am_files_media_handle_t));
                    }

                    // notify audio manager to check threshold
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_RESUME_IND, NULL);
                }
                /* It's trick (no break) */
            case AUD_AWS_SINK_OPEN_CODEC: {
                if (cntx->state == BT_SINK_SRV_MUSIC_AMI_AWS_PLAY) {
                    media = (bt_sink_srv_am_media_handle_t *)parm;
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(cntx->aws_hd)));
                    aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
                    if (aws) {
                        if (f_media == NULL) {
                            memcpy(&(aws->med_hd), media, sizeof(bt_sink_srv_am_media_handle_t));
                        }
                        aws->profile.state = BT_SINK_SRV_PROFILE_STATE_AUDIO_ON;
                        cntx->state = BT_SINK_SRV_MUSIC_AWS_PLAY;
                        cntx->log_cnt = 0;
                        aws->header.sequence_number = 0;
                        /* always reset interrupt flag */
                        dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_INTERRUPT);

                        /* dev aws flag streaming set, play & pause flag reset */
                        dev->flag |= BT_SINK_SRV_MUSIC_FLAG_AWS_STREAMING;
                        dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AWS_PLAY);
                        dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AWS_PAUSE);

                        ret = bt_aws_start_streaming_response(cntx->aws_hd, true);
                        bt_sink_srv_report("[sink][aws]start streaming resp-ret: %d", ret);

                        /* update focus dev */
                        bt_sink_srv_music_set_focus_device(dev);


                        bt_sink_srv_state_set(BT_SINK_SRV_STATE_STREAMING);

                        cntx->flag &= ~(BT_SINK_SRV_MUSIC_FLAG_AWS_PLAYING);
                        cntx->write_len = 0;

                        cntx->pkt_cnt = 0;

                        if (BT_AWS_CODEC_TYPE_AAC == aws->codec.type) {
                            dev->flag |= BT_SINK_SRV_MUSIC_FLAG_AWS_AAC;
                        } else {
                            dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AWS_AAC);
                        }

                        /* clear med node */
                        #ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                        bt_sink_srv_am_med_mgr_clear_node();
                        #endif
                    } else {
                        // trace error
                        err_code = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
                    }
                }
                break;
            }

            /* interrupt */
            case AUD_SUSPEND_BY_IND: {
                aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
                aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(aws_dev->dev_addr), BT_SINK_SRV_UUID_AWS);
                if (aws_dev) {
                    if (BT_AWS_ROLE_SINK == aws_dev->aws_role) {
                        /* reset state */
                        cntx->state = BT_SINK_SRV_MUSIC_READY;
                        if (aws_dev) {
                            /* dev aws flag streaming, play, pause flag reset */
                            aws_dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AWS_STREAMING);
                            aws_dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AWS_PLAY);
                            aws_dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AWS_PAUSE);
                        }
                        bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
                        /* notfiy app interrupt */
                        if (aws->codec.type == BT_AWS_CODEC_TYPE_MP3) {
                            /* transfer interrupt */
                            cmd_buf[0] = BT_SINK_SRV_AWS_CMD_SNK_INTERRUPT;
                            cmd_buf[1] = (0 & 0x00ff);
                            cmd_buf[2] = ((0 & 0xff00) >> 8);
                            ret = bt_aws_send_command(aws_dev->aws_hd, BT_AWS_COMMAND_ID_CUSTOMIZE, cmd_buf, 3);

                            bt_sink_srv_report("[sink][aws]ami_hdr[SUSPEND]-ret: %d\n", ret);
                            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_SNK_INTERRUPT, NULL);
                        }
                    }
                } else {
                    err_code = BT_SINK_SRV_MUSIC_ERR_FAIL_6TH;
                }
                break;
            }

            case AUD_RESUME_IND: {
                break;
            }

            case AUD_A2DP_PROC_IND: {
                break;
            }

            case AUD_AWS_A2DP_PROC_IND: {
                if (sub_msg == AUD_A2DP_CODEC_RESTART) {
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(cntx->aws_hd)));
                    aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
                    media = &(aws->med_hd);
                    #ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                    bt_sink_srv_am_med_mgr_clear_node();
                    #else
                    media->reset_share_buffer(cntx->aws_aid);
                    #endif /* __BT_SINK_SRV_AM_MED_LIST_SUPPORT__ */
                    bt_sink_srv_aws_start_resync();
                } else if (sub_msg == AUD_A2DP_AWS_UNDERFLOW) {
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(cntx->aws_hd)));
                    aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
                    media = &(aws->med_hd);
                    media->stop(cntx->aws_aid);
                    #ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                    bt_sink_srv_am_med_mgr_clear_node();
                    #else
                    media->reset_share_buffer(cntx->aws_aid);
                    #endif 
                    bt_sink_srv_aws_start_resync();
                }
                break;
            }

            // notify audio player to write dsp buffer
            case AUD_FILE_PROC_IND: {
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_DATA_REQ, NULL);
                break;
            }

            default:
                break;
        }
    }

    if ((cntx->aws_aid == aud_id && msg_id == AUD_AWS_A2DP_PROC_IND &&
        (sub_msg == AUD_STREAM_EVENT_DATA_REQ || AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW == sub_msg)) ||
        msg_id == AUD_FILE_PROC_IND) {
        // drop
        ;
    } else {
        bt_sink_srv_report("[sink][aws]ami_hdr[e]-err_code: %d, ret: %d\r\n", err_code, ret);
    }
}


static int32_t bt_sink_srv_aws_handle_connect_cnf(bt_aws_connect_cnf_t *conn_cnf)
{
    SinkProfileAws *aws = NULL;
    SinkProfileA2dp *a2dp = NULL;
    bt_sink_srv_music_device_t *dev = NULL, *sp_dev = NULL;
    int32_t ret = 0, err = 0;
    bt_bd_addr_t dev_addr;
    bool connected = false;
    bt_sink_srv_music_context_t *cntx = NULL;
    //bt_aws_codec_capability_t aws_cap;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    uint8_t mode = 0;


    cntx = bt_sink_srv_music_get_context();

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(conn_cnf->handle)));

    bt_sink_srv_memcpy(&dev_addr, &(dev->dev_addr), sizeof(bt_bd_addr_t));

    aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&dev->dev_addr, BT_SINK_SRV_UUID_AWS);
    if (aws) {
        aws->profile.state = BT_SINK_SRV_PROFILE_STATE_CONNECTED;
    }

    bt_sink_srv_report("[sink][aws]connect_cnf-aws_hd: 0x%x\n", conn_cnf->handle);

    if (conn_cnf->status == BT_STATUS_SUCCESS) {
        /* aws connected */
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(conn_cnf->handle)));
        if (dev) {
            ;
        } else {
            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, NULL);
        }

        dev->conn_bit |= BT_SINK_SRV_MUSIC_AWS_CONN_BIT;

        bt_sink_srv_report("[sink][aws]connect_cnf-con_bit: 0x%x\n", dev->conn_bit);
        connected = true;
    } else {
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(conn_cnf->handle)));
        if (dev) {
            /* check a2dp & avrcp connection staus */
            if ((dev->conn_bit | BT_SINK_SRV_MUSIC_A2DP_CONN_BIT) ||
                (dev->conn_bit | BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
                dev->aws_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
            } else {
                bt_sink_srv_music_reset_device(dev);
            }
        }
    }

    bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_TYPE_AWS, connected);

    sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
    a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(sp_dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
    if ((a2dp) &&
        (sp_dev) &&
        (sp_dev->conn_bit && BT_SINK_SRV_MUSIC_A2DP_CONN_BIT) &&
        (cntx->flag & BT_SINK_SRV_MUSIC_FLAG_PLAYING) &&
        (connected)) {
        /* resync case for connect */
        med_hd = &(a2dp->med_hd);
        med_hd->stop(cntx->aud_id);
        cntx->flag |= BT_SINK_SRV_MUSIC_FLAG_AWS_SRC_RESYNC;
    } else {
        err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

    /* sync AWS local mode */
#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
    mode = bt_sink_srv_ami_get_music_mode();
    if (mode == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
        bt_sink_srv_aws_notify_mode_change(conn_cnf->handle, BT_AWS_MODE_MP3);
    }
#endif

    bt_sink_srv_report("[sink][aws]connect_cnf(e)-ret: %d, err: %d\n", ret, err);
    return ret;
}


static int32_t bt_sink_srv_aws_handle_connect_ind(bt_aws_connect_ind_t *conn_ind)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = 0;


    bt_sink_srv_report("[sink][aws]connect_ind-aws_hd: 0x%x\n", conn_ind->handle);

    /* aws connected */
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR, (void *)(conn_ind->address));
    if (dev) {
        ;
    } else {
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, NULL);
    }

    if (dev) {
        dev->aws_hd = conn_ind->handle;
        memcpy(&(dev->dev_addr), conn_ind->address, sizeof(bt_bd_addr_t));
        dev->flag |= BT_SINK_SRV_MUSIC_FLAG_AWS_CON_IND;
    }

    ret = bt_aws_connect_response(conn_ind->handle, true);

    return ret;
}


static int32_t bt_sink_srv_aws_handle_disconnect_cnf(bt_aws_disconnect_cnf_t *disconn_cnf)
{
    SinkProfileAws *aws = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    int32_t ret = 0, err = 0;
    bt_bd_addr_t dev_addr;
    bool connected = false;
    bt_sink_srv_music_device_t *sp_dev = NULL;    
    SinkProfileA2dp *a2dp = NULL;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    bt_sink_srv_am_files_media_handle_t *f_med_hd = NULL;

    cntx = bt_sink_srv_music_get_context();

    bt_sink_srv_report("[sink][aws]disconnect_cnf[s]-state: %d, c_hd: 0x%x, aws_hd: 0x%x\n",
        cntx->state, cntx->aws_hd, disconn_cnf->handle);

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(disconn_cnf->handle)));
    bt_sink_srv_memcpy(&dev_addr, &(dev->dev_addr), sizeof(bt_bd_addr_t));

    if (dev) {
        if (BT_AWS_ROLE_SINK == dev->aws_role) {
            aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
            if ((aws) && (dev->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_STREAMING)) {
                med_hd = &(aws->med_hd);
                if (med_hd->aws_plh_deinit) {
                    med_hd->set_aws_flag(cntx->aws_aid, false);
                    med_hd->aws_plh_deinit();
                }
                f_med_hd = &(aws->f_med_hd);
                if (f_med_hd->media_handle.mp3.set_aws_flag) {
                    f_med_hd->media_handle.mp3.set_aws_flag(cntx->aws_aid, false);
                    f_med_hd->media_handle.mp3.aws_plh_deinit();
                }
            }
        } else if (BT_AWS_ROLE_SOURCE == dev->aws_role) {
            hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO);
            sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
            a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(sp_dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
            aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
            if ((a2dp) && (sp_dev) && (sp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_STREAMING)) {
                med_hd = &(a2dp->med_hd);
                med_hd->set_aws_flag(cntx->aud_id, false);
                med_hd->aws_plh_deinit();
            } else if ((aws) && (dev->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_STREAMING)) {
                f_med_hd = &(aws->f_med_hd);
                if (f_med_hd) {
                    f_med_hd->media_handle.mp3.set_aws_flag(cntx->aws_aid, false);
                    f_med_hd->media_handle.mp3.aws_plh_deinit();
                }
            }
        }
    }

    if (disconn_cnf->status == BT_STATUS_SUCCESS) {
        aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
        if (aws) {
            aws->profile.state = BT_SINK_SRV_PROFILE_STATE_DISCONNECTED;
            memset(&(aws->header), 0x00, sizeof(bt_sink_srv_aws_media_header_t));
        }

        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(disconn_cnf->handle)));
        if (dev) {
            dev->conn_bit &= ~(BT_SINK_SRV_MUSIC_AWS_CONN_BIT);

            if (!(dev->conn_bit)) {
                bt_sink_srv_music_reset_device(dev);
            } else {
                dev->aws_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
            }
        }

        if (cntx->aws_hd == disconn_cnf->handle) {
            if (cntx->state == BT_SINK_SRV_MUSIC_AWS_PLAY) {
                err = aws->med_hd.stop(cntx->aws_aid);
                ret = bt_sink_srv_ami_audio_stop(cntx->aws_aid);
                bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
            }
            cntx->aws_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
            cntx->state = BT_SINK_SRV_MUSIC_IDLE;
            err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_1ST;
        }
        ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    } else {
        ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

    bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_TYPE_AWS, connected);

    bt_sink_srv_report("[sink][aws]disconnect_cnf[e]-ret: %d, err: %d\n", ret, err);

    return ret;
}


static int32_t bt_sink_srv_aws_handle_disconnect_ind(bt_aws_disconnect_ind_t *disconn_ind)
{
    SinkProfileAws *aws = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    int32_t ret = 0, err = 0;
    bt_bd_addr_t dev_addr;
    bool connected = false;
    bt_sink_srv_music_device_t *sp_dev = NULL;    
    SinkProfileA2dp *a2dp = NULL;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    bt_sink_srv_am_files_media_handle_t *f_med_hd = NULL;

    cntx = bt_sink_srv_music_get_context();

    bt_sink_srv_report("[sink][aws]disconnect_ind[s]-state: %d, c_hd: 0x%x, aws_hd: 0x%x\n",
        cntx->state, cntx->aws_hd, disconn_ind->handle);

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(disconn_ind->handle)));

    bt_sink_srv_memcpy(&dev_addr, &(dev->dev_addr), sizeof(bt_bd_addr_t));

    if (dev) {
        aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
        bt_sink_srv_report("[sink][aws]aws %x type %x\n", aws, aws->codec.type);
        if (BT_AWS_ROLE_SINK == dev->aws_role) {
            if ((aws) && (dev->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_STREAMING)) {
                if (aws->codec.type == BT_AWS_CODEC_TYPE_MP3) {
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_STOP_IND, NULL);
                } else {
                    med_hd = &(aws->med_hd);
                    med_hd->set_aws_flag(cntx->aws_aid, false);
                    med_hd->aws_plh_deinit();
                }
            }
        } else if (BT_AWS_ROLE_SOURCE == dev->aws_role) {
            hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO);
            sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
            a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(sp_dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
            if ((a2dp) && (sp_dev) && (sp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_STREAMING)) {
                med_hd = &(a2dp->med_hd);
                med_hd->set_aws_flag(cntx->aud_id, false);
                med_hd->aws_plh_deinit();
            } else if (aws) {
                bt_sink_srv_report("[sink][aws]aws %x type %x\n", aws, aws->codec.type);
                if (aws->codec.type == BT_AWS_CODEC_TYPE_MP3) {
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_STOP_IND, NULL);
                }
            }
        }
    }

    aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
    if (aws) {
        aws->profile.state = BT_SINK_SRV_PROFILE_STATE_DISCONNECTED;
        memset(&(aws->header), 0x00, sizeof(bt_sink_srv_aws_media_header_t));
    }

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(disconn_ind->handle)));

    if (dev) {
        dev->conn_bit &= ~(BT_SINK_SRV_MUSIC_AWS_CONN_BIT);
    
        if (!(dev->conn_bit)) {
            bt_sink_srv_music_reset_device(dev);
        } else {
            dev->aws_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        }
    }

    if (cntx->aws_hd == disconn_ind->handle) {
        if (cntx->state == BT_SINK_SRV_MUSIC_AWS_PLAY) {
            if (aws->codec.type == BT_AWS_CODEC_TYPE_MP3) {
                f_med_hd = &(aws->f_med_hd);
                err = f_med_hd->media_handle.mp3.stop(cntx->aws_aid);
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_STOP_IND, NULL);
            } else {
                med_hd = &(aws->med_hd);
                err = med_hd->stop(cntx->aws_aid);
                ret = bt_sink_srv_ami_audio_stop(cntx->aws_aid);
                bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);
            }
        }
        cntx->aws_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        cntx->state = BT_SINK_SRV_MUSIC_IDLE;
    }

    bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_TYPE_AWS, connected);

    bt_sink_srv_report("[sink][aws]disconnect_ind[e]-ret: %d, err: %d\n", ret, err);

    return ret;
}


static int32_t bt_sink_srv_aws_handle_start_streaming_cnf(bt_aws_start_streaming_cnf_t *start_cnf)
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    SinkProfileAws *aws = NULL;

    if (start_cnf->status != BT_STATUS_SUCCESS) {
        bt_sink_srv_report("[sink][aws]handle_start_streaming_cnf not accept by sink\r\n");
        return ret;
    }

    cntx = bt_sink_srv_music_get_context();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(start_cnf->handle)));
    aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);

    if (cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_SRC_RESYNC) {
        cntx->flag &= ~(BT_SINK_SRV_MUSIC_FLAG_AWS_SRC_RESYNC);
        bt_sink_srv_aws_start_resync();
    } else {
        if ((dev) && (cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC)) {
            cntx->flag &= ~BT_SINK_SRV_MUSIC_FLAG_AWS_START_STREAMING_CNF;
            cntx->flag &= ~BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC;
            ret = bt_a2dp_start_streaming_response(cntx->a2dp_hd, true);
        } else {
            cntx->flag |= BT_SINK_SRV_MUSIC_FLAG_AWS_START_STREAMING_CNF;
        }
    }

    // notify audio player
    bt_sink_srv_report("[sink][aws]handle_start_streaming_cnf[s]-codec type: %d\r\n",aws->codec.type);
    if (aws->codec.type == BT_AWS_CODEC_TYPE_MP3) {
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_PLAY_READY, NULL);
    }

    return ret;
}


static int32_t bt_sink_srv_aws_handle_start_streaming_ind(bt_aws_start_streaming_ind_t *start_ind)
{
    SinkProfileAws *aws = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    int32_t ret = 0;
    bt_sink_srv_am_id_t ami_ret = 0;
    bt_sink_srv_am_audio_capability_t  aud_cap;

    cntx = bt_sink_srv_music_get_context();

    bt_sink_srv_report("[sink][aws]handle_strat_streaming_ind[s]-state: %d, pd: %d\r\n",
                       cntx->state, cntx->partymode);

    if (cntx->state == BT_SINK_SRV_MUSIC_READY) {
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(start_ind->handle)));
        aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
        if (aws) {
            cntx->aws_hd = start_ind->handle;
            cntx->state = BT_SINK_SRV_MUSIC_AMI_AWS_PLAY;
            /* fill aud param */
            memset(&aud_cap, 0x00, sizeof(bt_sink_srv_am_audio_capability_t));
            aud_cap.type = AWS;
            aud_cap.codec.aws_format.aws_codec.role = BT_AWS_ROLE_SINK;
            memcpy(&(aud_cap.codec.aws_format.aws_codec.codec_cap), (start_ind->codec_cap), sizeof(bt_aws_codec_capability_t));

            aud_cap.audio_stream_out.audio_device = HAL_AUDIO_DEVICE_HEADSET;
            aud_cap.audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t)cntx->vol_lev;
            //cntx->vol_lev = AUD_VOL_OUT_LEVEL6;
            aud_cap.audio_stream_out.audio_mute = false;
            if (aud_cap.codec.aws_format.aws_codec.codec_cap.type == BT_AWS_CODEC_TYPE_SBC) {
                bt_sink_srv_report("[sink][aws]start_ind(am)--type: %d, role: %d, type2: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d\r\n",
                                   aud_cap.type,
                                   aud_cap.codec.aws_format.aws_codec.role,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.type,
                                   //aud_cap.codec.aws_format.aws_codec.codec_cap.sep_type,
                                   39,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.length,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.sbc.channel_mode,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.sbc.sample_freq,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.sbc.alloc_method,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.sbc.subbands,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.sbc.block_len,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.sbc.min_bitpool,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.sbc.max_bitpool);
            } else if (aud_cap.codec.aws_format.aws_codec.codec_cap.type == BT_AWS_CODEC_TYPE_AAC) {
                bt_sink_srv_report("[sink][aws]start_ind(am)--type: %d, role: %d, type2: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d, 8: %d, 9: %d\r\n",
                                   aud_cap.type,
                                   aud_cap.codec.aws_format.aws_codec.role,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.type,
                                   //aud_cap.codec.aws_format.aws_codec.codec_cap.sep_type,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.length,
                                   39,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.aac.object_type,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.aac.freq_h,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.aac.reserved,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.aac.channels,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.aac.freq_l,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.aac.br_h,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.aac.vbr,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.aac.br_m,
                                   aud_cap.codec.aws_format.aws_codec.codec_cap.codec.aac.br_l);
            } else if (aud_cap.codec.aws_format.aws_codec.codec_cap.type == BT_AWS_CODEC_TYPE_MP3) {
                if (aud_cap.codec.aws_format.aws_codec.codec_cap.codec.mp3.change_track) {
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_CHANGE_TRACK_IND, NULL);
                }
            }

            // if it's mp3, check sink state should not in call
            bt_sink_srv_state_t state = bt_sink_srv_state_query();
            if (state & BT_SINK_SRV_STATE_ALL_CALLS && aws->codec.type == BT_AWS_CODEC_TYPE_MP3) {
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_STOP_IND, NULL);
                cntx->state = BT_SINK_SRV_MUSIC_READY;
                ami_ret = bt_aws_start_streaming_response(cntx->aws_hd, false);
                bt_sink_srv_report("[sink][aws]start_ind(am)--reject\r\n");
            } else {
                ami_ret = bt_sink_srv_ami_audio_play(cntx->aws_aid, &aud_cap);
                bt_sink_srv_report("[sink][aws]start_ind(am)--aws_aid:%d, ret: %d vol: %d\r\n", cntx->aws_aid, ami_ret, cntx->vol_lev);
                memcpy(&aws->codec, (start_ind->codec_cap), sizeof(bt_aws_codec_capability_t));
            }
        } else {
            // trace error
            ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
        }
    } else {
        /* upexpected state */
        ret = BT_SINK_SRV_MUSIC_ERR_FAIL_7TH;
    }

    return ret;
}


static int32_t bt_sink_srv_aws_handle_suspend_streaming_cnf(bt_aws_suspend_streaming_cnf_t *suspend_cnf)
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    bt_sink_srv_music_device_t *sp_dev = NULL;
    SinkProfileAws *aws = NULL;
    SinkProfileA2dp *a2dp = NULL;
    bt_aws_codec_capability_t aws_cap;
    bt_sink_srv_am_audio_capability_t  aud_cap;

    cntx = bt_sink_srv_music_get_context();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(suspend_cnf->handle)));
    aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);

    bt_sink_srv_report("[sink][aws]handle_suspend_streaming_cnf--flag: 0x%x\r\n", cntx->flag);

    if ((dev)) {
        ;
    }

    cntx->flag &= ~(BT_SINK_SRV_MUSIC_FLAG_PLAYING);

    if (cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_SEND_SUSPEND) {
        sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
        if (sp_dev) {
            ret = bt_a2dp_suspend_streaming_response(sp_dev->a2dp_hd, true);
        }

        cntx->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AWS_SEND_SUSPEND);
    } 
    if (cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_SEND_START_STREAMING) {
        sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
        a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(sp_dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
        aws->codec.type = a2dp->codec.type;
        aws->codec.length = a2dp->codec.length;
        memcpy(&aws->codec.codec, &a2dp->codec.codec, aws->codec.length);
        //aws_cap.type = a2dp->codec.type;
        //aws_cap.length = a2dp->codec.length;
        //memcpy(&aws_cap.codec, &a2dp->codec.codec, aws_cap.length);
        ret = bt_aws_start_streaming_request(dev->aws_hd, &aws->codec);
        // set audio capability
        memset(&aud_cap, 0x00, sizeof(bt_sink_srv_am_audio_capability_t));
        aud_cap.type = A2DP;
        aud_cap.codec.a2dp_format.a2dp_codec.role = BT_A2DP_SINK;
        memcpy(&(aud_cap.codec.a2dp_format.a2dp_codec.codec_cap), &(a2dp->codec), sizeof(bt_a2dp_codec_capability_t));
        aud_cap.audio_stream_out.audio_device = HAL_AUDIO_DEVICE_HEADSET;
        aud_cap.audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t)cntx->vol_lev;
        aud_cap.audio_stream_out.audio_mute = false;
        ret = bt_sink_srv_ami_audio_play(cntx->aud_id, &aud_cap);
        bt_sink_srv_report("[aws]audio play ret = %d %x %x %x\r\n", ret, aws_cap.type, aws_cap.length, &a2dp->codec);
        cntx->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AWS_SEND_START_STREAMING);
    }
    if (cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_MODE_CHANGING) {
        uint8_t mode = (uint8_t)bt_sink_srv_ami_get_next_music_mode();
        bt_aws_mode_t aws_mode;
        if (mode == BT_SINK_SRV_MODE_BT_MUSIC)
        {
            aws_mode = BT_AWS_MODE_A2DP;
        }
        else
        {
            aws_mode = BT_AWS_MODE_MP3;
        }
        cntx->state = BT_SINK_SRV_MUSIC_READY;
        ret = bt_aws_set_mode_request(dev->aws_hd, aws_mode);
        bt_sink_srv_report("[aws]send mode change req ret = %d\r\n", ret);
    }
    //cntx->state = BT_SINK_SRV_MUSIC_READY;

    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_SUSPEND_CNF, NULL);

    return ret;
}


static int32_t bt_sink_srv_aws_handle_suspend_streaming_ind(bt_aws_suspend_streaming_ind_t *suspend_ind)
{
    SinkProfileAws *aws = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    int32_t ret = 0, err = 0;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;


    cntx = bt_sink_srv_music_get_context();

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(suspend_ind->handle)));
    aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
    ret = bt_aws_suspend_streaming_response(suspend_ind->handle, true);
    bt_sink_srv_report("[sink][aws]suspend_streaming_ind(s)-state: %d, h1: 0x%x h2 0x%x\n", cntx->state, cntx->aws_hd, suspend_ind->handle);
    if ((cntx->state == BT_SINK_SRV_MUSIC_AWS_PLAY) && (cntx->aws_hd == suspend_ind->handle)) {
        cntx->state = BT_SINK_SRV_MUSIC_READY;
        if (aws->codec.type == BT_AWS_CODEC_TYPE_MP3) {
            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_SUSPEND_IND, NULL);
        } else {
            med_hd = &(aws->med_hd);
            err = med_hd->stop(cntx->aws_aid);
            ret = bt_sink_srv_ami_audio_stop(cntx->aws_aid);
        }
        bt_sink_srv_report("[sink][aws]suspend_streaming_ind(am)-ret: %d, err: %d\n", ret, err);
    } else {
        err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

    aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
    if (aws) {
        aws->profile.state = BT_SINK_SRV_PROFILE_STATE_CONNECTED;
        memset(&(aws->header), 0x00, sizeof(bt_sink_srv_aws_media_header_t));
    } else {
        err = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
    }

    if (dev) {
        /* dev aws flag streaming, play, pause flag reset */
        dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AWS_STREAMING);
        dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AWS_PLAY);
        dev->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AWS_PAUSE);
    }

    bt_sink_srv_state_reset(BT_SINK_SRV_STATE_STREAMING);

    bt_sink_srv_report("[sink][aws]suspend_streaming_ind[e]-ret: %d, err: %d\n", ret, err);
    return ret;
}


static int32_t bt_sink_srv_aws_handle_data_received_ind(bt_aws_streaming_received_ind_t *data_ind)
{
    SinkProfileAws *aws = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    int32_t ret = 0;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
#ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
    uint32_t free_len = 0, cur_len = 0, write_len = 0;
    uint8_t *ring_buf = NULL;
#endif
    uint8_t *med_data = NULL;
    uint32_t payload_size = 0;
    uint8_t *hci_buff = NULL;
    bt_hci_packet_t *pkt = NULL;

    cntx = bt_sink_srv_music_get_context();

    hci_buff = (uint8_t *)(data_ind->data_node);
    pkt = (bt_hci_packet_t *)(data_ind->data_node);

    if ((cntx->state == BT_SINK_SRV_MUSIC_AWS_PLAY) &&
            (cntx->aws_hd == data_ind->handle)) {
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(data_ind->handle)));
        aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
        med_hd = &(aws->med_hd);

        if (aws->header.sequence_number == 0) {
            bt_sink_srv_aws_fetch_media_header(hci_buff + data_ind->media_offset,
                                                &(aws->header));
        }

        med_data = bt_sink_srv_aws_uppack_media_data(data_ind->handle,
                   hci_buff + data_ind->media_offset,
                   data_ind->total_length - data_ind->media_offset,
                   &payload_size);

        if (aws) {
            ;
        }

        #ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
        /* queue list handle */
        pkt->offset = pkt->packet_length - payload_size;
        pkt->pad = ((pkt->pad & 0xffff0000) | (data_ind->media_offset));
        ret = bt_sink_srv_am_med_mgr_push_data((void *)pkt);

        if (ret < 0) {
            bt_sink_srv_report("[sink][aws]handle_data(mgr)(error)-ret: %d, 0x%x, 0x%x\n", ret, med_data, med_hd);
        } else {
            cntx->write_len += payload_size;
            cntx->pkt_cnt += 1;

            if (!(cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_PLAYING)) {
                if (cntx->pkt_cnt > BT_SINK_SRV_AWS_PKT_THRESHHOLD) {
                    cntx->flag |= (BT_SINK_SRV_MUSIC_FLAG_AWS_PLAYING);
                    med_hd->set_aws_flag(cntx->aws_aid, true);
                    med_hd->aws_plh_init(cntx->aws_aid);
                    ret = med_hd->play(cntx->aws_aid);
                    bt_sink_srv_report("[sink][aws](list)start play-ret: %d\n", ret);
                }
            }
        }
        #else
        free_len = med_hd->get_free_space(cntx->aws_aid);
        if (free_len <= payload_size) {
            /* error */
            bt_sink_srv_report("[sink][aws]handle_data(error)-free_len: %d, pay_size: %d\n", free_len, payload_size);
        } else {
            med_hd->get_write_buffer(cntx->aws_aid, &ring_buf, &write_len);
            if (write_len < payload_size) {
                /* first part */
                memcpy(ring_buf, med_data, write_len);
                med_hd->write_data_done(cntx->aws_aid, write_len);
                cur_len = write_len;

                /* second part */
                med_hd->get_write_buffer(cntx->aws_aid, &ring_buf, &write_len);
                memcpy(ring_buf, med_data + cur_len, payload_size - cur_len);
                //med_hd->write_data_done(cntx->aws_aid, write_len);
                med_hd->write_data_done(cntx->aws_aid, payload_size - cur_len);
            } else {
                memcpy(ring_buf, med_data, payload_size);
                med_hd->write_data_done(cntx->aws_aid, payload_size);
            }

            med_hd->finish_write_data(cntx->aws_aid);
            cntx->write_len += payload_size;
            cntx->pkt_cnt += 1;

            if (!(cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_PLAYING)) {
                if (cntx->pkt_cnt > BT_SINK_SRV_AWS_PKT_THRESHHOLD) {
                    cntx->flag |= (BT_SINK_SRV_MUSIC_FLAG_AWS_PLAYING);
                    med_hd->set_aws_flag(cntx->aws_aid, true);
                    med_hd->aws_plh_init(cntx->aws_aid);
                    ret = med_hd->play(cntx->aws_aid);
                    bt_sink_srv_report("[sink][aws]start play-ret: %d\n", ret);
                }
            }
        }
        #endif /* __BT_SINK_SRV_AM_MED_LIST_SUPPORT__ */
    } else {
        /* drop media data */
        bt_sink_srv_report("[sink][aws]drop_data-state: %d, c_hd: %d, d_hd: %d\n", cntx->state, cntx->aws_hd, data_ind->handle);
    }

    if ((bt_sink_srv_aws_data_rece_cnt < BT_SINK_SRV_AWS_DATA_RECE_CNT)) {
        bt_sink_srv_state_set(BT_SINK_SRV_STATE_STREAMING);
    }

    return ret;
}

static int32_t bt_sink_srv_aws_handle_custom_streaming_ind(bt_aws_custom_streaming_received_ind_t *command_ind)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;

    //uint8_t *x = (uint8_t *)command_ind->data;
    if (command_ind->data_length == 0) {
        return ret;
    }
    bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));
    event->aws_data_info.buf = command_ind->data;
    event->aws_data_info.len = command_ind->data_length;
    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_DATA_IND, event);
    bt_sink_srv_memory_free(event);


    return ret;
}

static int32_t bt_sink_srv_aws_handle_role_set_ind(bt_aws_role_set_ind_t *aws_role_set_ind)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    bt_sink_srv_music_context_t *cntx = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    SinkProfileAws *aws = NULL;
    SinkProfileA2dp *a2dp = NULL;
    bt_sink_srv_music_device_t *sp_dev = NULL, *aws_dev = NULL;
    int32_t err = 0;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    bt_sink_srv_am_files_media_handle_t *f_med_hd = NULL;
    bt_aws_role_t ori_role = BT_AWS_ROLE_NONE;

    cntx = bt_sink_srv_music_get_context();

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(aws_role_set_ind->handle)));

    if (dev) {
        ori_role = dev->aws_role;
        dev->aws_role = aws_role_set_ind->role;
        dev->mp3_aws_attempt = 0;

        /* AWS SNK role */
        if (BT_AWS_ROLE_SINK == dev->aws_role) {
            hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO_BOTH_R_CHANNEL);
            if ((cntx->state == BT_SINK_SRV_MUSIC_IDLE) && (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT)) {
                cntx->state = BT_SINK_SRV_MUSIC_READY;
                //bt_sink_srv_cm_set_role(&(dev->dev_addr), BT_ROLE_SLAVE);
            }
        } else if (BT_AWS_ROLE_SOURCE == dev->aws_role) {
            hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO_BOTH_L_CHANNEL);
           // bt_sink_srv_cm_set_role(&(dev->dev_addr), BT_ROLE_SLAVE);

            bt_sink_srv_aws_notify_vol_level(dev->aws_hd, cntx->vol_lev);

           /* sync AWS SRC mode */
           #ifdef __MUSIC_MODE_SWITCH_SUPPORT__
           //bt_sink_srv_aws_notify_mode_change(dev->aws_hd);
           #endif
           if (cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_SRC_RESYNC) {
               sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
               a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(sp_dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
               aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
               aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
               if ((sp_dev) && (a2dp) && (aws_dev)) {
                   memset(&aws->codec, 0x00, sizeof(bt_aws_codec_capability_t));
                   if (a2dp->codec.type == BT_A2DP_CODEC_SBC) {
                       aws->codec.type = BT_AWS_CODEC_TYPE_SBC;
                   } else if (a2dp->codec.type == BT_A2DP_CODEC_AAC) {
                       aws->codec.type = BT_AWS_CODEC_TYPE_AAC;
                   } else {
                   }
                   aws->codec.length = a2dp->codec.length;
                   memcpy(&(aws->codec.codec), &(a2dp->codec.codec), a2dp->codec.length);
                   ret = bt_aws_start_streaming_request(aws_dev->aws_hd, &aws->codec);
                   bt_sink_srv_report("[sink][aws]role_set_ind(start)--ret: %d\n", ret);
                   err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_1ST;
               } else {
                   err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
               }
           } else if (cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_WAIT_ROLE_STREAMING) {
               sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
               a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(sp_dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
               aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
               aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
               if ((sp_dev) && (a2dp) && (aws_dev)) {
                   memset(&aws->codec, 0x00, sizeof(bt_aws_codec_capability_t));
                   if (a2dp->codec.type == BT_A2DP_CODEC_SBC) {
                       aws->codec.type = BT_AWS_CODEC_TYPE_SBC;
                   } else if (a2dp->codec.type == BT_A2DP_CODEC_AAC) {
                       aws->codec.type = BT_AWS_CODEC_TYPE_AAC;
                   } else {
                   }
                   aws->codec.length = a2dp->codec.length;
                   memcpy(&(aws->codec.codec), &(a2dp->codec.codec), a2dp->codec.length);
                   ret = bt_aws_start_streaming_request(aws_dev->aws_hd, &aws->codec);
                   bt_sink_srv_report("[sink][aws]role_set_ind(WAIT start)--ret: %d\n", ret);
                   err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_2ND;
               }
               cntx->flag &= ~BT_SINK_SRV_MUSIC_FLAG_AWS_WAIT_ROLE_STREAMING;
           } else if (cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_WAIT_ROLE_STREAMING_MP3) {
               aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
               aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
               // mp3 + aws
               if (aws_dev) {
                    memset(&aws->codec, 0x00, sizeof(bt_aws_codec_capability_t));
                    aws->codec.type = BT_AWS_CODEC_TYPE_MP3;
                    aws->codec.length = sizeof(bt_aws_codec_capability_t);

                    cntx->flag &= ~BT_SINK_SRV_MUSIC_FLAG_AWS_WAIT_ROLE_STREAMING;
                    ret = bt_aws_start_streaming_request(aws_dev->aws_hd, &aws->codec);
                    bt_sink_srv_report("[sink][aws]role_set_ind(WAIT start)--ret: %d\n", ret);
                    err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_2ND;

               } else {
                   err = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
               }
               cntx->flag &= ~BT_SINK_SRV_MUSIC_FLAG_AWS_WAIT_ROLE_STREAMING_MP3;
           }
        } else if (BT_AWS_ROLE_NONE == dev->aws_role) {
            if (BT_AWS_ROLE_SINK == ori_role) {
                aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
                if ((cntx->state == BT_SINK_SRV_MUSIC_AWS_PLAY) && (cntx->aws_hd == aws_role_set_ind->handle)) {
                    cntx->state = BT_SINK_SRV_MUSIC_READY;
                    if (aws) {
                        med_hd = &(aws->med_hd);
                        if (med_hd->set_aws_flag) {
                            med_hd->set_aws_flag(cntx->aws_aid, false);
                            med_hd->aws_plh_deinit();
                            err = med_hd->stop(cntx->aws_aid);
                        }
                        f_med_hd = &(aws->f_med_hd);
                        if (f_med_hd->media_handle.mp3.set_aws_flag) {
                            f_med_hd->media_handle.mp3.set_aws_flag(cntx->aws_aid, false);
                            f_med_hd->media_handle.mp3.aws_plh_deinit();
                            err = f_med_hd->media_handle.mp3.stop(cntx->aws_aid);
                        }
                        
                        ret = bt_sink_srv_ami_audio_stop(cntx->aws_aid);
                    }
                    bt_sink_srv_report("[sink][aws]role_set_ind(none)-ret: %d, err: %d\n", ret, err);
                }
            }
        }
    } else {
        ret = BT_SINK_SRV_MUSIC_ERR_FAIL_7TH;
    }

    bt_sink_srv_report("[sink][aws]role_set_ind-aws_hd: 0x%x, ret: %d, role: %d, err: %d\n",
        aws_role_set_ind->handle, ret, aws_role_set_ind->role, err);

    return ret;
}


static int32_t bt_sink_srv_aws_handle_command_ind(bt_aws_command_ind_t *command_ind)
{
    int32_t ret = 0, err = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_device_t *sp_dev = NULL;
    SinkProfileAws *aws = NULL;
    uint8_t cmd_type = 0;
    uint8_t vol_lev = 0;
#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
    bt_sink_srv_music_mode_t mode;
    bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));
    mode = bt_sink_srv_ami_get_music_mode();
#endif
    bt_sink_srv_am_id_t am_id;
    bt_sink_srv_music_context_t *cntx = NULL;


    cntx = bt_sink_srv_music_get_context();

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(command_ind->handle)));

    bt_sink_srv_report("[sink][aws]command_ind[s]-aws_hd: 0x%x, cid: 0x%x, con_bit: 0x%x, aws_role: %d\n",
        command_ind->handle, command_ind->command_id, dev->conn_bit, dev->aws_role);
    bt_sink_srv_report("[sink][aws]command_data:%s, command_length:%d", command_ind->data, command_ind->data_length);

    if (dev) {
            sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
            aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);

            bt_sink_srv_report("[sink][aws]command_ind-dev: 0x%x, con_bit: 0x%x, type: %d\n",
                sp_dev, sp_dev->conn_bit, aws->codec.type);
                switch (command_ind->command_id) {
                    case BT_AWS_COMMAND_ID_PLAY: {
                        if (sp_dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT &&
                                mode == BT_SINK_SRV_MODE_BT_MUSIC) {
                            ret = bt_sink_srv_avrcp_play_music(sp_dev->avrcp_hd);
#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
                        } else if (mode == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
                            /* local play */
                            event->key_info.value = BT_SINK_SRV_KEY_FUNC;
                            event->key_info.action = BT_SINK_SRV_KEY_ACT_PRESS_UP;
                            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_KEY_PRESS, event);
#endif
                        }
                        break;
                    }
                    case BT_AWS_COMMAND_ID_PAUSE: {
                        if (sp_dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT &&
                                mode == BT_SINK_SRV_MODE_BT_MUSIC) {
                            ret = bt_sink_srv_avrcp_stop_music(sp_dev->avrcp_hd);
#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
                        } else if (mode == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
                            /* local play */
                            event->key_info.value = BT_SINK_SRV_KEY_FUNC;
                            event->key_info.action = BT_SINK_SRV_KEY_ACT_PRESS_UP;
                            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_KEY_PRESS, event);
#endif
                        }
                        break;
                    }
                    case BT_AWS_COMMAND_ID_FORWARD: {
                        if (sp_dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT &&
                                mode == BT_SINK_SRV_MODE_BT_MUSIC) {
                            ret = bt_sink_srv_avrcp_change_ntrack(sp_dev->avrcp_hd);
#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
                        } else if (mode == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
                            /* local play */
                            event->key_info.value = BT_SINK_SRV_KEY_NEXT;
                            event->key_info.action = BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP;
                            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_KEY_PRESS, event);
#endif
                        }
                        break;
                    }
                    case BT_AWS_COMMAND_ID_BACKWARD: {
                        if (sp_dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT &&
                                mode == BT_SINK_SRV_MODE_BT_MUSIC) {
                            ret = bt_sink_srv_avrcp_change_ptrack(sp_dev->avrcp_hd);
#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
                        } else if (mode == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
                            /* local play */
                            event->key_info.value = BT_SINK_SRV_KEY_PREV;
                            event->key_info.action = BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP;
                            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_KEY_PRESS, event);
#endif
                        }
                        break;
                    }
                    case BT_AWS_COMMAND_ID_RESYNC: {
                        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_RESYNC, NULL);
                        break;
                    }
                    case BT_AWS_COMMAND_ID_MODE_CHANGE: {
                        uint8_t new_mode = command_ind->data[0];
                        uint8_t cur_mode = bt_sink_srv_ami_get_music_mode();
                        if (cur_mode == BT_SINK_SRV_MODE_BT_MUSIC) {
                            cur_mode = BT_AWS_MODE_A2DP;
                        } else {
                            cur_mode = BT_AWS_MODE_MP3;
                        }
                        // do mode change if receive 1. no specific mode 2. mode is different
                        if (new_mode == BT_AWS_MODE_NONE ||
                                new_mode != cur_mode) {
                            bt_sink_srv_ami_check_music_mode(bt_sink_srv_ami_get_next_music_mode());
                            bt_sink_srv_aws_set_mode_change(dev->aws_hd);
                        // send set mode request to sync lower layer mode
                        } else {
                            ret = bt_aws_set_mode_request(dev->aws_hd, new_mode);
                            bt_sink_srv_report("[sink][aws] set %d mode req hd: 0x%x, ret: %d\n",
                                    new_mode, dev->aws_hd, ret);
                        }

                        break;
                    }
                    case BT_AWS_COMMAND_ID_CUSTOMIZE: {
#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
                        cmd_type = command_ind->data[0];
#endif
                        if (cmd_type == BT_SINK_SRV_AWS_CMD_VOLUME_SET) {
                            vol_lev = command_ind->data[3];
                            am_id = -1;
                            if (BT_AWS_ROLE_SINK == dev->aws_role) {
                                if (BT_AWS_CODEC_TYPE_MP3 == aws->codec.type) {
                                    am_id = cntx->aws_aid;
                                } else if (BT_AWS_CODEC_TYPE_SBC == aws->codec.type || BT_AWS_CODEC_TYPE_AAC == aws->codec.type) {
                                    am_id = cntx->aws_aid;
                                }
                            }
                            bt_sink_srv_ami_audio_set_volume(am_id, vol_lev, STREAM_OUT);
                            cntx->vol_lev = vol_lev;
                        }

                        if ((BT_AWS_ROLE_SOURCE == dev->aws_role) && BT_AWS_CODEC_TYPE_MP3 == aws->codec.type ) {
                                cmd_type = command_ind->data[0];
                                switch (cmd_type) {
                                    case BT_SINK_SRV_AWS_CMD_SNK_INTERRUPT: {
                                        ret = bt_aws_suspend_streaming_request(command_ind->handle);
                                        bt_sink_srv_report("[sink][aws]command_ind(SNK_INT)-ret: %d\n", ret);
                                        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_SNK_INTERRUPT_IND, NULL);
                                        break;
                                    }

                                    case BT_SINK_SRV_AWS_CMD_SNK_RESUME: {
                                        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_SNK_RESUME_IND, NULL);
                                        break;
                                    }
                                    default:
                                        break;
                                }
                        }
                        break;
                    }

                    default:
                        break;
            }
    } else {
        ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
    bt_sink_srv_memory_free(event);
#endif
    bt_sink_srv_report("[sink][aws]command_ind[e]-ret: %d, err: %d\n", ret, err);

    return ret;
}


static int32_t bt_sink_srv_aws_handle_notify_ind(bt_aws_notification_ind_t *notify_ind)
{
    int32_t ret = 0, err = 0;
    bt_sink_srv_music_context_t *cntx = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    uint32_t time_dur = 0;
    hal_gpt_status_t gpt_ret = HAL_GPT_STATUS_OK;
    SinkProfileAws *aws = NULL;

    cntx = bt_sink_srv_music_get_context();

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(notify_ind->handle)));

    bt_sink_srv_report("[sink][aws]notify_ind[s]-aws_hd: 0x%x, op_id: 0x%x, aws_role: %d port %d dev %x\n",
        notify_ind->handle, notify_ind->param->op_id, dev->aws_role, cntx->gpt_port, dev);

    if (dev) {
        if ((BT_AWS_ROLE_SINK == dev->aws_role) &&
            (notify_ind->param->op_id == BT_AWS_NOTIFY_ID_READY_TO_PLAY)) {
            time_dur = notify_ind->param->play_time.play_time;

            // stack returns invalid time if start time passed
            if (time_dur == BT_SINK_SRV_AWS_INVALID_TIME) {
                bt_sink_srv_am_media_handle_t *media = NULL;
                bt_sink_srv_music_device_t *dev = NULL;
                SinkProfileAws *aws = NULL;

                bt_sink_srv_report("[sink][aws]time_dur:0x%x invalid, need resync", time_dur);
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(cntx->aws_hd)));
                aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
                if (aws->codec.type == BT_AWS_CODEC_TYPE_MP3) {
                    bt_aws_send_command(dev->aws_hd, BT_AWS_COMMAND_ID_RESYNC, NULL, 0);
                } else {
                    media = &(aws->med_hd);
                    media->stop(cntx->aws_aid);
                #ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                    bt_sink_srv_am_med_mgr_clear_node();
                #else
                    media->reset_share_buffer(cntx->aws_aid);
                #endif /* __BT_SINK_SRV_AM_MED_LIST_SUPPORT__ */
                    bt_sink_srv_aws_start_resync();
                }
            } else {
                sink_loc_play_nclk = notify_ind->param->play_time.loc_play_nclk;
                sink_loc_play_nclk_intra = notify_ind->param->play_time.loc_play_nclk_intra;
                if (cntx->gpt_port == HAL_GPT_MAX_PORT) {
                    if (hal_gpt_init(HAL_GPT_1) == HAL_GPT_STATUS_OK) {
                        cntx->gpt_port = HAL_GPT_1;
                        hal_gpt_register_callback(cntx->gpt_port,
                            bt_sink_srv_aws_gpt_cb, (void *)dev);
                        gpt_ret = hal_gpt_start_timer_us(cntx->gpt_port, time_dur - 2000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
                    } else if (hal_gpt_init(HAL_GPT_2) == HAL_GPT_STATUS_OK) {
                        cntx->gpt_port = HAL_GPT_2;
                        hal_gpt_register_callback(cntx->gpt_port,
                            bt_sink_srv_aws_gpt_cb, (void *)dev);
                        gpt_ret = hal_gpt_start_timer_us(cntx->gpt_port, time_dur - 2000, HAL_GPT_TIMER_TYPE_ONE_SHOT);
                    } else {
                        err = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
                    }
                } else {
                    err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
                }
            }
        }

        // adjust volume level
        aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
        switch (notify_ind->param->op_id) {
            case BT_AWS_NOTIFY_ID_VOLUME_UP: {
                if ((BT_AWS_CODEC_TYPE_SBC == aws->codec.type) ||
                    (BT_AWS_CODEC_TYPE_AAC == aws->codec.type)) {
                    bt_sink_srv_a2dp_change_volume(VOLUME_UP);
                } else if (BT_AWS_CODEC_TYPE_MP3 == aws->codec.type) {
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_VOLUME_UP, NULL);
                }
                break;
            }
            case BT_AWS_NOTIFY_ID_VOLUME_DOWN: {
                if ((BT_AWS_CODEC_TYPE_SBC == aws->codec.type) ||
                    (BT_AWS_CODEC_TYPE_AAC == aws->codec.type)) {
                    bt_sink_srv_a2dp_change_volume(VOLUME_DOWN);
                } else if (BT_AWS_CODEC_TYPE_MP3 == aws->codec.type) {
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_VOLUME_DOWN, NULL);
                }
                break;
            }

            default:
                break;
        }
    } else {
        ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

    bt_sink_srv_report("[sink][aws]notify_ind[e]-ret: %d, err: %d, gpt_ret: %d\n",
        ret, err, gpt_ret);

    return ret;
}


static int32_t bt_sink_srv_aws_handle_resync_ind(bt_aws_resync_ind_t *sync_ind)
{
    int32_t ret = 0, err = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_device_t *sp_dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    SinkProfileAws *aws = NULL;
    SinkProfileA2dp *a2dp = NULL;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;

    cntx = bt_sink_srv_music_get_context();

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(sync_ind->handle)));
    if (dev) {
        if (BT_AWS_ROLE_SINK == dev->aws_role) {
            aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
            if (aws) {
                cntx->flag &= ~(BT_SINK_SRV_MUSIC_FLAG_AWS_PLAYING);
                cntx->write_len = 0;
                cntx->pkt_cnt = 0;
                if (aws->codec.type == BT_AWS_CODEC_TYPE_MP3) {
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_RESYNC_IND, NULL);
                } else {
                    med_hd = &(aws->med_hd);
                    med_hd->stop(cntx->aws_aid);
                    #ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                    bt_sink_srv_am_med_mgr_clear_node();
                    #else
                    med_hd->reset_share_buffer(cntx->aws_aid);
                    #endif
                }
            } else {
                err = BT_SINK_SRV_MUSIC_ERR_FAIL_4TH;
            }
        } else if (BT_AWS_ROLE_SOURCE == dev->aws_role) {
            sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
            a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(sp_dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
            if ((sp_dev) && (a2dp)) {
                cntx->flag &= ~(BT_SINK_SRV_MUSIC_FLAG_PLAYING);
                cntx->write_len = 0;
                cntx->pkt_cnt = 0;
                med_hd = &(a2dp->med_hd);
                med_hd->stop(cntx->aud_id);
                #ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                bt_sink_srv_am_med_mgr_clear_node();
                #else
                med_hd->reset_share_buffer(cntx->aws_aid);
                #endif
            } else {
                err = BT_SINK_SRV_MUSIC_ERR_FAIL_3RD;
            }
        } else {
            err = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
        }
    } else {
        err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

    ret = bt_aws_resync_response(sync_ind->handle);

    bt_sink_srv_report("[sink][aws]resync_ind-hd: 0x%x, ret: %d, err: %d, aws_role: %d\n", sync_ind->handle, ret, err, dev->aws_role);

    return ret;
}


static int32_t bt_sink_srv_aws_handle_resync_cnf(bt_aws_resync_cnf_t *sync_cnf)
{
    bt_sink_srv_music_device_t *dev = NULL;
    SinkProfileAws *aws = NULL;
    int32_t ret = 0, err = 0;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&(sync_cnf->handle)));
    aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
    bt_sink_srv_report("[sink][aws]resync_cnf-hd: 0x%x, status: %d, ret: %d, err: %d\n", sync_cnf->handle, sync_cnf->status, ret, err);

    // notify audio player
    if (aws->codec.type == BT_AWS_CODEC_TYPE_MP3) {
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_PLAY_READY, NULL);
    }
    return ret;
}


bt_sink_srv_status_t bt_sink_srv_aws_action_handler(bt_sink_srv_action_t action, void *param)
{
    bt_status_t ret = 0;
    bt_bd_addr_t *dev_addr = NULL;
    uint32_t hd = 0;
    int32_t err = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;
    bt_sink_srv_profile_connection_action_t *conn_info = NULL;
#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
    bt_sink_srv_music_mode_t mode = bt_sink_srv_ami_get_music_mode();
    bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));
#endif


    bt_sink_srv_report("[sink][aws]process_aws_action[s]-action: 0x%x, base: 0x%x\n", action, BT_SINK_SRV_ACTION_AVRCP_START);
    cntx = bt_sink_srv_music_get_context();

    switch (action) {
        case BT_SINK_SRV_ACTION_PROFILE_INIT: {
            bt_sink_srv_aws_init();
            break;
        }

        case BT_SINK_SRV_ACTION_PROFILE_DEINIT: {
            if (cntx->aws_aid != BT_SINK_SRV_INVALID_AID) {
                #ifdef __BT_SINK_SRV_AM_SUPPORT__
                bt_sink_srv_ami_audio_close(cntx->aws_aid);
                #endif /* __BT_SINK_SRV_AM_SUPPORT__ */
                cntx->aws_aid = BT_SINK_SRV_INVALID_AID;
            }
            break;
        }

        case BT_SINK_SRV_ACTION_PROFILE_CONNECT: {
            conn_info = (bt_sink_srv_profile_connection_action_t *)param;
            dev_addr = &(conn_info->bt_addr);
            if (conn_info->profile_connection_mask & BT_SINK_SRV_PROFILE_CONNECTION_TYPE_AWS) {
                ret = bt_aws_connect(&hd, dev_addr);
                if (BT_STATUS_SUCCESS == ret) {
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR, dev_addr);
                    if (dev) {
                        dev->aws_hd = hd;
                        /* unexpected, need handle */
                        err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
                    } else {
                        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, NULL);
                        if (dev) {
                            dev->aws_hd = hd;
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
            dev_addr = &(conn_info->bt_addr);
            if (conn_info->profile_connection_mask & BT_SINK_SRV_PROFILE_CONNECTION_TYPE_AWS) {
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR, (void *)dev_addr);
                if ((dev) && (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT)) {
                    ret = bt_aws_disconnect(dev->aws_hd);
                } else {
                    err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
                }
            }
            break;
        }
#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
        case BT_SINK_SRV_ACTION_PLAY: {
            if (mode == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
                event->key_info.value = BT_SINK_SRV_KEY_FUNC;
                event->key_info.action = BT_SINK_SRV_KEY_ACT_PRESS_UP;
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_KEY_PRESS, event);
            }
            break;
        }

        case BT_SINK_SRV_ACTION_PAUSE: {
            if (mode == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
                event->key_info.value = BT_SINK_SRV_KEY_FUNC;
                event->key_info.action = BT_SINK_SRV_KEY_ACT_PRESS_UP;
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_KEY_PRESS, event);
            }
            break;
        }

        case BT_SINK_SRV_ACTION_NEXT_TRACK: {
            if (mode == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
                event->key_info.value = BT_SINK_SRV_KEY_NEXT;
                event->key_info.action = BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP;
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_KEY_PRESS, event);
            }
            break;
        }

        case BT_SINK_SRV_ACTION_PRE_TRACK: {
            if (mode == BT_SINK_SRV_MODE_LOCAL_MUSIC) {
                event->key_info.value = BT_SINK_SRV_KEY_FUNC;
                event->key_info.action = BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP;
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_KEY_PRESS, event);
            }
            break;
        }
#endif

        default:
            break;
    }

#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
    bt_sink_srv_memory_free(event);
#endif
    bt_sink_srv_report("[sink][aws]process_aws_action[e]-ret: %d, err: %d\n", ret, err);
    return BT_SINK_SRV_STATUS_SUCCESS;
}


bt_status_t bt_sink_srv_aws_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    int32_t ret = 0;

    if ((bt_sink_srv_aws_data_rece_cnt < BT_SINK_SRV_AWS_DATA_RECE_CNT) || 
            ((msg != BT_AWS_STREAMING_RECEIVED_IND) &&
            (msg != BT_AWS_CUSTOM_STREAMING_RECEIVED_IND))) {
        bt_sink_srv_report("[sink][aws]common_hdr[s]-msg: 0x%x, status: %d\n", msg, status);
    }

    switch (msg) {
        case BT_AWS_CONNECT_CNF: {
            bt_aws_connect_cnf_t *conn_cnf = (bt_aws_connect_cnf_t *)buffer;

            ret = bt_sink_srv_aws_handle_connect_cnf(conn_cnf);
            break;
        }

        case BT_AWS_CONNECT_IND: {
            bt_aws_connect_ind_t *conn_ind = (bt_aws_connect_ind_t *)buffer;

            ret = bt_sink_srv_aws_handle_connect_ind(conn_ind);
            break;
        }

        case BT_AWS_DISCONNECT_CNF: {
            bt_aws_disconnect_cnf_t *disconn_cnf = (bt_aws_disconnect_cnf_t *)buffer;
            bt_sink_srv_aws_data_rece_cnt = 0;

            ret = bt_sink_srv_aws_handle_disconnect_cnf(disconn_cnf);
            break;
        }

        case BT_AWS_DISCONNECT_IND: {
            bt_aws_disconnect_ind_t *disconn_ind = (bt_aws_disconnect_ind_t *)buffer;
            bt_sink_srv_aws_data_rece_cnt = 0;

            ret = bt_sink_srv_aws_handle_disconnect_ind(disconn_ind);
            break;
        }

        case BT_AWS_START_STREAMING_CNF: {
            bt_aws_start_streaming_cnf_t *start_cnf = (bt_aws_start_streaming_cnf_t *)buffer;

            ret = bt_sink_srv_aws_handle_start_streaming_cnf(start_cnf);
            break;
        }

        case BT_AWS_START_STREAMING_IND: {
            bt_aws_start_streaming_ind_t *start_ind = (bt_aws_start_streaming_ind_t *)buffer;
            bt_sink_srv_aws_data_rece_cnt = 0;

            bt_sink_srv_report("[sink][aws]start_ind(comm_bk)--buff: 0x%x, cap: 0x%x\n", buffer, start_ind->codec_cap);

            if (start_ind->codec_cap->type == BT_AWS_CODEC_TYPE_SBC) {
                bt_sink_srv_report("[sink][aws]start_ind(comm_bk_sbc)--hd: 0x%x, type: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d\r\n",
                                   start_ind->handle,
                                   start_ind->codec_cap->type,
                                   //start_ind->codec_cap->sep_type,
                                   38,
                                   start_ind->codec_cap->length,
                                   start_ind->codec_cap->codec.sbc.channel_mode,
                                   start_ind->codec_cap->codec.sbc.sample_freq,
                                   start_ind->codec_cap->codec.sbc.alloc_method,
                                   start_ind->codec_cap->codec.sbc.subbands,
                                   start_ind->codec_cap->codec.sbc.block_len,
                                   start_ind->codec_cap->codec.sbc.min_bitpool,
                                   start_ind->codec_cap->codec.sbc.max_bitpool);
            } else if (start_ind->codec_cap->type == BT_AWS_CODEC_TYPE_AAC) {
                bt_sink_srv_report("[sink][aws]start_ind(comm_bk_aac)--hd: 0x%x, type: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d, 8: %d, 9: %d\r\n",
                                   start_ind->handle,
                                   start_ind->codec_cap->type,
                                   //start_ind->codec_cap->sep_type,
                                   38,
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
            } else if (start_ind->codec_cap->type == BT_AWS_CODEC_TYPE_MP3) {
                bt_sink_srv_report("[sink][aws]start_ind(comm_bk_mp3)--hd: 0x%x, type: %d, len: %d\r\n",
                                   start_ind->handle,
                                   start_ind->codec_cap->type,
                                   start_ind->codec_cap->length);
            }

            ret = bt_sink_srv_aws_handle_start_streaming_ind(start_ind);
            break;
        }

        case BT_AWS_SUSPEND_STREAMING_CNF: {
            bt_aws_suspend_streaming_cnf_t *suspend_cnf = (bt_aws_suspend_streaming_cnf_t *)buffer;

            ret = bt_sink_srv_aws_handle_suspend_streaming_cnf(suspend_cnf);
            break;
        }

        case BT_AWS_SUSPEND_STREAMING_IND: {
            bt_aws_suspend_streaming_ind_t *suspend_ind = (bt_aws_suspend_streaming_ind_t *)buffer;

            bt_sink_srv_aws_data_rece_cnt = 0;

            ret = bt_sink_srv_aws_handle_suspend_streaming_ind(suspend_ind);
            break;
        }

        case BT_AWS_STREAMING_RECEIVED_IND: {
            bt_hci_packet_t *pkt = NULL;
            bt_aws_streaming_received_ind_t *data_ind = (bt_aws_streaming_received_ind_t *)buffer;
            ++bt_sink_srv_aws_data_rece_cnt;

            bt_sink_srv_aws_rece_ind = (bt_aws_streaming_received_ind_t *)buffer;
            bt_sink_srv_aws_pkt = (bt_hci_packet_t *)(data_ind->data_node);

            pkt = (bt_hci_packet_t *)(data_ind->data_node);
            if (pkt->ref_count != 1) {
            //if (pkt->ref_count != 1 || pkt->indicator != 2) {
                /* drop this packet */
                bt_sink_srv_report("[sink][aws]comm_bk(rece_ind)--pkt: 0x%x, ref: %d, len: %d, offset: %d, size: %d, data_ind: 0x%x\n",
                                   pkt, pkt->ref_count, pkt->packet_length,
                                   pkt->offset, sizeof(bt_hci_packet_t), data_ind);
                #ifdef __BT_SINK_SRV_AWS_LIST_DEBUG__
                bt_sink_srv_assert(0);
                #endif
                break;
            }

            if (bt_sink_srv_am_med_mgr_node_is_exist((void *)(data_ind->data_node))) {
                /* drop this packet */
                bt_sink_srv_report("[sink][aws]comm_bk(rece_ind exist)--pkt: 0x%x, ref: %d, len: %d, offset: %d, size: %d, data_ind: 0x%x\n",
                                   pkt, pkt->ref_count, pkt->packet_length,
                                   pkt->offset, sizeof(bt_hci_packet_t), data_ind);
                #ifdef __BT_SINK_SRV_AWS_LIST_DEBUG__
                bt_sink_srv_assert(0);
                #endif
            }

            ret = bt_sink_srv_aws_handle_data_received_ind(data_ind);

            break;
        }

        case BT_AWS_CUSTOM_STREAMING_RECEIVED_IND: {
            ++bt_sink_srv_aws_data_rece_cnt;
            bt_aws_custom_streaming_received_ind_t *command_ind = (bt_aws_custom_streaming_received_ind_t *)buffer;
            ret = bt_sink_srv_aws_handle_custom_streaming_ind(command_ind);
            break;
        }

        case BT_AWS_ROLE_SET_IND: {
            bt_aws_role_set_ind_t *aws_role_set_ind = (bt_aws_role_set_ind_t *)buffer;

            ret = bt_sink_srv_aws_handle_role_set_ind(aws_role_set_ind);
            break;
        }

        case BT_AWS_MODE_SET_IND: {
            bt_aws_mode_set_ind_t *aws_mode_set_ind = (bt_aws_mode_set_ind_t *)buffer;
            bt_sink_srv_music_mode_t cmd_mode;
            bt_sink_srv_music_device_t *dev = NULL;
            bt_sink_srv_music_context_t *cntx = NULL;
            cntx = bt_sink_srv_music_get_context();
            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);

            if (aws_mode_set_ind->aws_mode == BT_AWS_MODE_A2DP)
            {
                cmd_mode = BT_SINK_SRV_MODE_BT_MUSIC;
            }
            else 
            {
                cmd_mode = BT_SINK_SRV_MODE_LOCAL_MUSIC;
            }
            if (BT_AWS_ROLE_SINK == dev->aws_role) {
                bt_sink_srv_ami_check_music_mode(cmd_mode);
                cntx->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AWS_MODE_CHANGING);
            }
            bt_sink_srv_ami_set_music_mode(cmd_mode);

            bt_aws_set_mode_response(aws_mode_set_ind->handle,true);
            break;
        }

        case BT_AWS_MODE_SET_CNF: {
            bt_sink_srv_music_context_t *cntx = NULL;
            cntx = bt_sink_srv_music_get_context();
            cntx->flag &= (~BT_SINK_SRV_MUSIC_FLAG_AWS_MODE_CHANGING);

            bt_sink_srv_ami_set_music_mode(bt_sink_srv_ami_get_next_music_mode());
            break;
        }

        case BT_AWS_COMMAND_IND: {
            bt_aws_command_ind_t *command_ind = (bt_aws_command_ind_t *)buffer;

            ret = bt_sink_srv_aws_handle_command_ind(command_ind);
            break;
        }

        case BT_AWS_NOTIFY_IND: {
            bt_aws_notification_ind_t *notify_ind = (bt_aws_notification_ind_t *)buffer;

            ret = bt_sink_srv_aws_handle_notify_ind(notify_ind);
            break;
        }

        case BT_AWS_RESYNC_IND: {
            bt_aws_resync_ind_t *sync_ind = (bt_aws_resync_ind_t *)buffer;

            ret = bt_sink_srv_aws_handle_resync_ind(sync_ind);
            break;
        }

        case BT_AWS_RESYNC_CNF: {
            bt_aws_resync_cnf_t *sync_cnf = (bt_aws_resync_cnf_t *)buffer;

            ret = bt_sink_srv_aws_handle_resync_cnf(sync_cnf);
            break;
        }

        default:
            break;
    }

    if ((bt_sink_srv_aws_data_rece_cnt < BT_SINK_SRV_AWS_DATA_RECE_CNT) || 
            ((msg != BT_AWS_STREAMING_RECEIVED_IND) &&
            (msg != BT_AWS_CUSTOM_STREAMING_RECEIVED_IND))) {
        bt_sink_srv_report("[sink][aws]common_hdr[e]-ret: %d\n", ret);
    }

    return ret;
}


static void bt_sink_srv_aws_gpt_cb(void *user_data)
{
    bt_sink_srv_music_context_t *cntx = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    SinkProfileAws *aws = NULL;
    int32_t ret = 0, err = 0;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    bt_sink_srv_am_files_media_handle_t *f_med_hd = NULL;

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

    aws = (SinkProfileAws *)bt_sink_srv_cm_find_profile_by_address(&(dev->dev_addr), BT_SINK_SRV_UUID_AWS);
    play_clock = sink_loc_play_nclk * 312 + (sink_loc_play_nclk >> 1)  + sink_loc_play_nclk_intra;
    if (BT_AWS_ROLE_SINK == dev->aws_role) {
        if (aws) {
            if ((BT_AWS_CODEC_TYPE_SBC == aws->codec.type) ||
                (BT_AWS_CODEC_TYPE_AAC == aws->codec.type)) {
                med_hd = &(aws->med_hd);
                cntx->flag |= (BT_SINK_SRV_MUSIC_FLAG_AWS_PLAYING);
                //ret = med_hd->play(cntx->aws_aid);
                __disable_irq();
                //hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_HIGH);
                while(1)
                {                    
                    bt_aws_get_bt_local_time(&nclk,&nclk_intra);

                    curr_clock = nclk * 312.5 + nclk_intra;
                    if (curr_clock >= play_clock)
                    {
                        hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_LOW);
                        /*uint8_t log_buff[512];
                        sprintf((char *)log_buff, "curr native clock, nclk:%x, nclk_intra:%x\n", nclk, nclk_intra);
                        atci_send_data(log_buff, strlen(log_buff));*/
                        break;
                    }
                }                
                ret = med_hd->set_aws_initial_sync(cntx->aws_aid);
                __enable_irq();
                bt_sink_srv_audio_clock_init();
                bt_sink_srv_report("[sink][aws]gpt_cb-start play--ret: %d\n", ret);
            } else if (BT_AWS_CODEC_TYPE_MP3 == aws->codec.type) {
                f_med_hd = &(aws->f_med_hd);
                cntx->flag |= (BT_SINK_SRV_MUSIC_FLAG_AWS_PLAYING);
                //ret = f_med_hd->media_handle.mp3.play(cntx->aws_aid);
                __disable_irq();
                //hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_HIGH);
                while(1)
                {
                    bt_aws_get_bt_local_time(&nclk,&nclk_intra);

                    curr_clock = nclk * 312 + (nclk >> 1)  + nclk_intra;
                    if (curr_clock >= play_clock)
                    {
                        hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_LOW);
                        hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_HIGH);
                        hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_LOW);
                        /*uint8_t log_buff[512];
                        sprintf((char *)log_buff, "curr native clock, nclk:%x, nclk_intra:%x\n", nclk, nclk_intra);
                        atci_send_data(log_buff, strlen(log_buff));*/
                        break;
                    }
                }
                ret = f_med_hd->media_handle.mp3.set_aws_initial_sync(cntx->aws_aid);
                __enable_irq();
                bt_sink_srv_audio_clock_init();
                bt_sink_srv_report("[sink][aws]gpt_cb-mp3 start play--ret: %d\n", ret);
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_INIT_SYNC, NULL);
            } else {
                err = BT_SINK_SRV_MUSIC_ERR_FAIL_3RD;
            }
        } else {
            err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
        }
    } else {
        err = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
    }

    bt_sink_srv_report("[sink][aws]gpt_cb[e]-ret: %d, err: %d\n", ret, err);
}


static void bt_sink_srv_aws_start_resync(void)
{
    int32_t ret = 0, err = 0;
    bt_sink_srv_music_device_t *aws_dev = NULL, *sp_dev = NULL;
    SinkProfileA2dp *a2dp = NULL;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    bt_sink_srv_music_context_t *cntx = NULL;


    cntx = bt_sink_srv_music_get_context();

    aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);

    if (aws_dev) {
        if (BT_AWS_ROLE_SINK == aws_dev->aws_role) {
            if ((aws_dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT)) {
                /* aws connection, need resync */
                cntx->flag &= ~(BT_SINK_SRV_MUSIC_FLAG_AWS_PLAYING);
                cntx->write_len = 0;
                cntx->pkt_cnt = 0;
                ret = bt_aws_resync_request(aws_dev->aws_hd);
                err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_2ND;
            } else {
                err = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
            }
        } else if (BT_AWS_ROLE_SOURCE == aws_dev->aws_role) {
            sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
            a2dp = (SinkProfileA2dp *)bt_sink_srv_cm_find_profile_by_address(&(sp_dev->dev_addr), BT_SINK_SRV_UUID_A2DP);
            if ((sp_dev) && (a2dp)) {
                cntx->flag &= ~(BT_SINK_SRV_MUSIC_FLAG_PLAYING);
                cntx->write_len = 0;
                cntx->pkt_cnt = 0;
                med_hd = &(a2dp->med_hd);
                //med_hd->stop(cntx->aud_id);
                #ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                bt_sink_srv_am_med_mgr_clear_node();
                #else
                med_hd->reset_share_buffer(cntx->aud_id);
                #endif
                ret = bt_aws_resync_request(aws_dev->aws_hd);
                err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_3RD;
            } else {
                err = BT_SINK_SRV_MUSIC_ERR_FAIL_3RD;
            }
        }
    } else {
        err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

    bt_sink_srv_report("[sink][aws]start_resync-ret: %d, err: %d, med_hd: 0x%x\n", ret, err, med_hd);
}

int32_t bt_sink_srv_aws_set_play(uint32_t handle, bt_aws_role_t role, uint8_t change_track)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    bt_sink_srv_music_context_t *cntx = NULL;
    cntx = bt_sink_srv_music_get_context();
    bt_aws_codec_capability_t aws_cap;

    if (role != BT_AWS_ROLE_NONE) {
        cntx->flag |= BT_SINK_SRV_MUSIC_FLAG_AWS_WAIT_ROLE_STREAMING_MP3;
        ret = bt_aws_set_role_request(handle, role);
        bt_sink_srv_report("[sink][aws]set role-ret: %x\n", ret);
    } else {
        memset(&aws_cap, 0x00, sizeof(bt_aws_codec_capability_t));
        aws_cap.type = BT_AWS_CODEC_TYPE_MP3;
        aws_cap.length = sizeof(bt_aws_codec_capability_t);
        aws_cap.codec.mp3.change_track = change_track;
        ret = bt_aws_start_streaming_request(handle, &aws_cap);
        bt_sink_srv_report("[sink][aws]start streaming-ret: %d change track %d\n", ret, change_track);
    }

    return ret;
}

int32_t bt_sink_srv_aws_set_pause(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;

    ret = bt_aws_suspend_streaming_request(handle);
    bt_sink_srv_report("[sink][aws]suspend streaming req-ret: %d\n", ret);
    return ret;
}

int32_t bt_sink_srv_aws_set_resync(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;

    ret = bt_aws_resync_request(handle);
    bt_sink_srv_report("[sink][aws]resync req-ret: %d\n", ret);
    return ret;
}

/* transfer SRC volume setting */
int32_t bt_sink_srv_aws_notify_vol_level(uint32_t handle, uint8_t level)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    uint8_t cmd_buf[16] = {0};
    uint16_t cmd_len = 0;
    cmd_buf[0] = BT_SINK_SRV_AWS_CMD_VOLUME_SET;
    cmd_len = 4;
    cmd_buf[1] = (1 & 0x00ff);
    cmd_buf[2] = ((1 & 0xff00) >> 8);
    cmd_buf[3] = level;
    ret = bt_aws_send_command(handle, BT_AWS_COMMAND_ID_CUSTOMIZE, cmd_buf, cmd_len);
    bt_sink_srv_report("[sink][aws]send volume: %d ret: %d\n", level, ret);

    return ret;
}

int32_t bt_sink_srv_aws_notify_vol_up(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    bt_aws_notify_t aws_notify = {0};
    aws_notify.op_id = BT_AWS_NOTIFY_ID_VOLUME_UP;

    ret = bt_aws_notify(handle, &aws_notify);
    return ret;
}

int32_t bt_sink_srv_aws_notify_vol_down(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    bt_aws_notify_t aws_notify = {0};
    aws_notify.op_id = BT_AWS_NOTIFY_ID_VOLUME_DOWN;

    ret = bt_aws_notify(handle, &aws_notify);
    return ret;
}

int32_t bt_sink_srv_aws_notify_play(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    ret = bt_aws_send_command(handle, BT_AWS_COMMAND_ID_PLAY, NULL, 0);
    bt_sink_srv_report("[sink][aws]play_music-hd: 0x%x, ret: %d\n", handle, ret);
    return ret;
}


int32_t bt_sink_srv_aws_notify_stop(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    ret = bt_aws_send_command(handle, BT_AWS_COMMAND_ID_PAUSE, NULL, 0);
    bt_sink_srv_report("[sink][aws]stop_music-hd: 0x%x, ret: %d\n", handle, ret);
    return ret;
}


int32_t bt_sink_srv_aws_notify_change_ntrack(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    ret = bt_aws_send_command(handle, BT_AWS_COMMAND_ID_FORWARD, NULL, 0);
    bt_sink_srv_report("[sink][aws]change_ntrack-hd: 0x%x, ret: %d\n", handle, ret);
    return ret;
}


int32_t bt_sink_srv_aws_notify_change_ptrack(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    ret = bt_aws_send_command(handle, BT_AWS_COMMAND_ID_BACKWARD, NULL, 0);
    bt_sink_srv_report("[sink][aws]change_ptrack-hd: 0x%x, ret: %d\n", handle, ret);
    return ret;
}

int32_t bt_sink_srv_aws_set_mode_change(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    bt_sink_srv_music_context_t *cntx = NULL;
    uint8_t mode = (uint8_t)bt_sink_srv_ami_get_next_music_mode();
    bt_aws_mode_t aws_mode;

    cntx = bt_sink_srv_music_get_context();

    if (mode == BT_SINK_SRV_MODE_BT_MUSIC)
    {
        aws_mode = BT_AWS_MODE_A2DP;
    }
    else
    {
        aws_mode = BT_AWS_MODE_MP3;
    }

    if (cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_MODE_CHANGING) {
        bt_sink_srv_report("[sink][aws]mode_changing hd: 0x%x\n", handle);
    } else {
        ret = bt_aws_set_mode_request(handle, aws_mode);
        bt_sink_srv_report("[sink][aws]mode_change req hd: 0x%x, ret: %d\n", handle, ret);
    }
    return ret;
}

int32_t bt_sink_srv_aws_notify_mode_change(uint32_t handle, uint8_t mode)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    ret = bt_aws_send_command(handle, BT_AWS_COMMAND_ID_MODE_CHANGE, &mode, 1);
    bt_sink_srv_report("[sink][aws]notify mode_change_hd: 0x%x, ret: %d\n", handle, ret);
    return ret;
}

int32_t bt_sink_srv_aws_notify_resync(uint32_t handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
    ret = bt_aws_send_command(handle, BT_AWS_COMMAND_ID_RESYNC, NULL, 0);
    bt_sink_srv_report("[sink][aws]notify resync: 0x%x, ret: %d\n", handle, ret);
    return ret;
}

void bt_sink_srv_local_mode_check(bt_sink_srv_music_mode_t mode)
{
    bt_sink_srv_music_context_t *cntx = NULL;
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    cntx = bt_sink_srv_music_get_context();
    // set mode changing to indicate stopping current streaming
    bt_sink_srv_report("[sink][aws]mp3 mode check flag %x\n", cntx->flag);
    if (cntx->flag & BT_SINK_SRV_MUSIC_FLAG_PLAYING ||
            cntx->flag & BT_SINK_SRV_MUSIC_FLAG_AWS_PLAYING) {
        bt_sink_srv_music_device_t *aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
        if (aws_dev) {
            cntx->flag |= BT_SINK_SRV_MUSIC_FLAG_AWS_MODE_CHANGING;
        }
    // send set mode command if it's source
    } else {
        bt_sink_srv_music_device_t *dev = NULL;
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, NULL);
        if (dev && BT_AWS_ROLE_SINK != dev->aws_role) {
            ret = bt_sink_srv_aws_set_mode_change(dev->aws_hd);
            bt_sink_srv_report("[Sink][AWS] send mode switch--new: %d ret %d\n", mode, ret);
        }
    }

    if (mode == BT_SINK_SRV_MODE_BT_MUSIC) {
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MP3_STOP_IND, NULL);
    }
}


bt_status_t bt_aws_get_a2dp_sink_status(uint32_t handle, bt_aws_a2dp_sink_status_t *a2dp_status)
{
    int32_t ret = 0, err = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_device_t *sp_dev = NULL;


    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, (void *)(&handle));

    sp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_FOCUS, NULL);
    if ((dev)) {
        if (sp_dev) {
            a2dp_status->status = true;
            a2dp_status->a2dp_handle = sp_dev->a2dp_hd;
            err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_1ST;
        } else {
            a2dp_status->status = false;
            err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_2ND;
        }
    } else {
        ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

    bt_sink_srv_report("[sink][aws]bt_aws_get_a2dp_sink_status[e]-ret: %d, err: %d\n",
        ret, err);

    return ret;
}
#endif /* __BT_AWS_SUPPORT__ */

