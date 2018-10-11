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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "task_def.h"
#include "bt_sink_srv_am_task.h"
#include "bt_sink_srv_ami.h"
#include "bt_sink_srv_media_mgr.h"

#include "bt_sink_srv_audio_tunning.h"
#ifdef __BT_AWS_SUPPORT__
#include "bt_aws.h"
#include "bt_codec.h"
#include "bt_sink_srv_audio_sync.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_aws.h"
#endif /* __BT_AWS_SUPPORT__ */

#ifdef __BT_SINK_AUDIO_TUNING__
#include "audio_dsp_fd216_db_to_gain_value_mapping_table.h"
#endif /* __BT_SINK_AUDIO_TUNING__ */
#if PRODUCT_VERSION == 2533
#include "external_dsp_application.h"
#endif

#ifdef __AM_DEBUG_INFO__
#include "bt_sink_srv_utils.h"
#endif

#include "bt_sink_srv_audio_setting.h"

#ifdef MTK_PROMPT_SOUND_ENABLE
#include "prompt_control.h"
#endif

const uint32_t g_volume_out_config[DEVICE_OUT_MAX][AUD_VOL_TYPE][AUD_VOL_OUT_MAX][2] = {  {{{0xFFFFFC18, 0x044C}, {0xFFFFFCE0, 0x044C}, {0xFFFFFDA8, 0x044C}, {0xFFFFFE70, 0x044C}, {0xFFFFFF38, 0x044C}, {0x00000000, 0x044C}, {0x00000000, 0x0384}},
        {{0xFFFFFC18, 0x044C}, {0xFFFFFCE0, 0x044C}, {0xFFFFFDA8, 0x044C}, {0xFFFFFE70, 0x044C}, {0xFFFFFF38, 0x044C}, {0x00000000, 0x044C}, {0x00000000, 0x0384}}
    },
    {   {{0xFFFFF768, 0x0000}, {0xFFFFF8F8, 0x0000}, {0xFFFFFA88, 0x0000}, {0xFFFFFC18, 0x0000}, {0xFFFFFDA8, 0x0000}, {0xFFFFFF38, 0x0000}, {0x00000000, 0x00C8}},
        {{0xFFFFF768, 0x0000}, {0xFFFFF8F8, 0x0000}, {0xFFFFFA88, 0x0000}, {0xFFFFFC18, 0x0000}, {0xFFFFFDA8, 0x0000}, {0xFFFFFF38, 0x0000}, {0x00000000, 0x00C8}}
    }
};
const uint32_t g_volume_in_config[DEVICE_IN_MAX][AUD_VOL_IN_MAX][2] = {{{0x00000000, 0x0258}}};
bt_sink_srv_am_amm_struct *ptr_callback_amm = NULL;
bt_sink_srv_am_aud_id_type_t g_rAm_aud_id[AM_REGISTER_ID_TOTAL];
static bt_sink_srv_am_background_t *g_prCurrent_resumer = NULL;
static uint8_t *g_bBT_Ringbuf = NULL;
#ifdef RTOS_TIMER
static TimerHandle_t g_xTimer_am;
static uint16_t g_lExpire_count = 0;
#endif
static bt_sink_srv_am_background_t g_rBackground_head = {0};
static bt_sink_srv_am_sink_state_t g_rSink_state = A2DP_SINK_CODEC_CLOSE;
static bt_sink_srv_am_media_handle_t g_prA2dp_sink_event_handle;
bt_media_handle_t *g_prHfp_media_handle = NULL;
bt_media_handle_t *g_prA2dp_sink_handle = NULL;
static uint16_t *aud_memory = NULL;
#ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
static uint8_t g_bt_sink_srv_am_ring_buffer[AM_RING_BUFFER_SIZE];
#endif

int32_t g_test_fix_warning = 0;

/* external dsp*/
//external_dsp_sending_path_register_value_t external_dsp_sending_path_register_value;

/* External DSP*/
//#if PRODUCT_VERSION == 2533
#if 0
external_dsp_sending_path_register_value_t external_dsp_sending_path_register_value;
#endif

/* files media handle */
static bt_sink_srv_am_files_media_handle_t g_am_files_media_handle;
static bt_sink_srv_am_file_state_t g_am_file_state = FILE_CODEC_CLOSE;

/* mp3 */
#ifdef __AUDIO_MP3_ENABLE__
static mp3_codec_media_handle_t *g_am_mp3_media_handle = NULL;
#endif

/* audio codec */
#ifdef __AUDIO_COMMON_CODEC_ENABLE__
static audio_codec_media_handle_t *g_am_audio_media_handle = NULL;
#endif

#ifdef __BT_AWS_SUPPORT__
static bt_aws_codec_type_t g_aws_codec_type = 0;
static uint16_t g_aws_sample_rate = 0;
static uint32_t g_aws_skew_loop_count = 0;
#endif /* __BT_AWS_SUPPORT__ */
static uint32_t g_a2dp_underflow_loop_count = 0;
#ifdef __BT_AWS_SUPPORT__
static uint32_t g_aws_underflow_loop_count = 0;
#endif /* __BT_AWS_SUPPORT__ */
#define BT_SINK_SRV_AM_MAX_UNDERFLOW_COUNT          (1)

static bool g_am_volume_enable = false;
static void am_receive_msg(bt_sink_srv_am_amm_struct *amm_ptr);
static void bt_codec_am_a2dp_sink_open(bt_sink_srv_am_a2dp_codec_t *a2dp_codec_t);
static bt_status_t bt_codec_am_a2dp_sink_play(bt_sink_srv_am_id_t aud_id);
static bt_status_t bt_codec_am_a2dp_sink_stop(bt_sink_srv_am_id_t aud_id);
static bt_status_t bt_codec_am_a2dp_sink_close(void);
static void bt_codec_am_hfp_open(bt_sink_srv_am_hfp_codec_t *hfp_codec_t);
static bt_status_t bt_codec_am_hfp_stop(void);

static void am_files_codec_open(bt_sink_srv_am_files_format_t *files_format);

static void am_files_codec_close(void);

#ifdef __BT_AWS_SUPPORT__
static int32_t bt_a2dp_aws_set_flag(bt_sink_srv_am_id_t aud_id, bool flag);
static int32_t bt_a2dp_aws_set_initial_sync(bt_sink_srv_am_id_t aud_id);
static void bt_a2dp_aws_plh_init(bt_sink_srv_am_id_t aud_id);
static void bt_a2dp_aws_plh_deinit(void);
static void aud_process_aws_a2dp_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
static int32_t bt_mp3_aws_set_flag(bt_sink_srv_am_id_t aud_id, bool flag);
static int32_t bt_mp3_aws_set_initial_sync(bt_sink_srv_am_id_t aud_id);
static int32_t bt_mp3_aws_init();
static int32_t bt_mp3_aws_deinit();
static int32_t bt_mp3_aws_set_clock_skew_compensation_value(int32_t sample_count);
static int32_t bt_mp3_aws_get_clock_skew_status(int32_t *status);
#endif /* __BT_AWS_SUPPORT__ */

#ifdef __AUDIO_MP3_ENABLE__
static void am_mp3_get_write_buffer(bt_sink_srv_am_id_t aud_id, uint8_t **buffer,
    uint32_t *length);

static void am_mp3_get_read_buffer(bt_sink_srv_am_id_t aud_id, uint8_t **buffer,
    uint32_t *length);

static int32_t am_mp3_get_data_count(bt_sink_srv_am_id_t aud_id);

static int32_t am_mp3_get_free_space(bt_sink_srv_am_id_t aud_id);

static void am_mp3_write_data_done(bt_sink_srv_am_id_t aud_id, uint32_t length);

static void am_mp3_finish_write_data(bt_sink_srv_am_id_t aud_id);

static int32_t am_mp3_play(bt_sink_srv_am_id_t aud_id);

static int32_t am_mp3_pause(bt_sink_srv_am_id_t aud_id);

static int32_t am_mp3_resume(bt_sink_srv_am_id_t aud_id);

static int32_t am_mp3_stop(bt_sink_srv_am_id_t aud_id);

static int32_t am_mp3_close_codec(bt_sink_srv_am_id_t aud_id);

static int32_t am_mp3_skip_id3v2_and_reach_next_frame(bt_sink_srv_am_id_t aud_id,
    uint32_t file_size);

static int32_t am_mp3_set_silence_frame_information(bt_sink_srv_am_id_t aud_id, silence_frame_information_t *frm_info);

static int32_t am_mp3_fill_silence_frame(bt_sink_srv_am_id_t aud_id, uint8_t *buffer);

static int32_t am_mp3_get_data_status(bt_sink_srv_am_id_t aud_id,
        mp3_codec_data_type_t type, int32_t *status);

static int32_t am_mp3_flush(bt_sink_srv_am_id_t aud_id, int32_t flush_data_flag);

static void am_files_mp3_callback(mp3_codec_media_handle_t *hdl, mp3_codec_event_t event);
#endif /* __AUDIO_MP3_ENABLE__ */

#ifdef __AUDIO_COMMON_CODEC_ENABLE__
static void am_audio_get_write_buffer(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length);
static void am_audio_write_data_done(bt_sink_srv_am_id_t aud_id, uint32_t length);
static void am_audio_finish_write_data(bt_sink_srv_am_id_t aud_id);
static int32_t am_audio_get_data_count(bt_sink_srv_am_id_t aud_id);
static int32_t am_audio_get_free_space(bt_sink_srv_am_id_t aud_id);
static int32_t am_audio_play(bt_sink_srv_am_id_t aud_id);
static int32_t am_audio_pause(bt_sink_srv_am_id_t aud_id);
static int32_t am_audio_resume(bt_sink_srv_am_id_t aud_id);
static int32_t am_audio_stop(bt_sink_srv_am_id_t aud_id);
static int32_t am_audio_close_codec(bt_sink_srv_am_id_t aud_id);
//static void aud_process_audio_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
static void am_files_audio_callback(audio_codec_media_handle_t *hdl, audio_codec_event_t event);
#endif

static void aud_process_mp3_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);

extern void BT_A2DP_CONVERT_SBC_CODEC(bt_codec_capability_t *dst_codec,
                               bt_a2dp_codec_capability_t *src_codec);

extern void BT_A2DP_CONVERT_AAC_CODEC(bt_codec_capability_t *dst_codec,
                               bt_a2dp_codec_capability_t *src_codec);

#ifndef MTK_DEBUG_LEVEL_INFO
extern int32_t g_test_fix_warning;
#endif


#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
static void bt_sink_srv_am_set_volume(bt_sink_srv_am_stream_type_t in_out, bt_sink_srv_audio_setting_vol_info_t *vol_info);
#endif

#ifdef __BT_AWS_SUPPORT__
extern void bt_sink_srv_fetch_bt_offset();
#endif

#ifndef WIN32_UT
/*****************************************************************************
 * FUNCTION
 *  am_task_create
 * DESCRIPTION
 *  This function is used to create a task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void am_task_create(void)
{
    xTaskCreate(am_task_main,
                AM_TASK_NAME,
                AM_TASK_STACKSIZE,
                NULL,
                AM_TASK_PRIO,
                NULL);
}
#endif

/*****************************************************************************
 * FUNCTION
 *  am_audio_set_play
 * DESCRIPTION
 *  This function is used to play the audio handler.
 * PARAMETERS
 *	background_ptr   [IN]
 * RETURNS
 *  void
 *****************************************************************************/
bt_sink_srv_am_hal_result_t aud_set_volume_level(bt_sink_srv_am_stream_type_t in_out,
        bt_sink_srv_am_volume_type_t vol_type,
        bt_sink_srv_am_device_set_t device,
        bt_sink_srv_am_volume_level_t level)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_hal_result_t eResult = HAL_AUDIO_STATUS_ERROR;
    bt_sink_srv_am_device_out_t dev_out;
    bt_sink_srv_am_device_in_t dev_in;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_sink_srv_report("[Sink][AM]in_out:%d, level:%d, device:0x%x", in_out, level, device);

    if (in_out == STREAM_OUT) {
        if (level < AUD_VOL_OUT_MAX) {
            if (device & DEVICE_LOUDSPEAKER) {
                //device = LOUDSPEAKER_STREAM_OUT;
                dev_out = LOUDSPEAKER_STREAM_OUT;
                bt_sink_srv_report("[Sink][AM]digital:0x%x, analog:0x%x", g_volume_out_config[dev_out][level][0], g_volume_out_config[dev_out][level][1]);
                if (g_am_volume_enable) {
                    ;
                } else {
                    eResult = hal_audio_set_stream_out_volume(g_volume_out_config[dev_out][vol_type][level][0], g_volume_out_config[dev_out][vol_type][level][1]);
                }
            } else if (device & DEVICE_EARPHONE) {
                //device = EARPHONE_STREAM_OUT;
                dev_out = EARPHONE_STREAM_OUT;
                bt_sink_srv_report("[Sink][AM]digital:0x%x, analog:0x%x", g_volume_out_config[dev_out][level][0], g_volume_out_config[dev_out][level][1]);
                if (g_am_volume_enable) {
                    ;
                } else {
                    eResult = hal_audio_set_stream_out_volume(g_volume_out_config[dev_out][vol_type][level][0], g_volume_out_config[dev_out][vol_type][level][1]);
                }
            }
        }
    } else if (in_out == STREAM_IN) {
        if (level < AUD_VOL_IN_MAX) {
            if (device & DEVICE_IN_LIST) {
                //device = MICPHONE_STREAM_IN;
                dev_in = MICPHONE_STREAM_IN;
                if (g_am_volume_enable) {
                    ;
                } else {
                    eResult = hal_audio_set_stream_in_volume(g_volume_in_config[dev_in][level][0], g_volume_in_config[dev_in][level][1]);
                }
            }
        }
    }
    if (eResult == HAL_AUDIO_STATUS_OK) {
        return HAL_AUDIO_STATUS_OK;
    } else {
#ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM][ERROR] Device and Stream type/Vol-level Not Matched");
#endif
        return HAL_AUDIO_STATUS_ERROR;
    }
}


void aud_stream_out_callback(bt_sink_srv_am_event_result_t event, void *user_data);

void aud_stream_in_callback(bt_sink_srv_am_event_result_t event, void *user_data);
/*****************************************************************************
 * FUNCTION
 *  am_audio_set_play
 * DESCRIPTION
 *  This function is used to play the audio handler.
 * PARAMETERS
 *	background_ptr   [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void am_audio_set_play(bt_sink_srv_am_background_t *background_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_hal_result_t        eResult = HAL_AUDIO_STATUS_ERROR;
    bt_sink_srv_am_id_t                bAud_id = background_ptr->aud_id;
    bt_sink_srv_am_audio_stream_out_t  *stream_out = &(background_ptr->audio_stream_out);
    bt_sink_srv_am_audio_stream_in_t   *stream_in = &(background_ptr->audio_stream_in);
    bt_sink_srv_am_stream_node_t       *pcm_stream_node = NULL;
    bt_sink_srv_am_stream_type_t       eIn_out;
#ifdef __BT_SINK_SRV_AUDIO_TUNING__
    bt_sink_srv_audio_tunning_context_t *aud_tunning_p = NULL;
#endif

#ifdef __BT_AWS_SUPPORT__
    bt_aws_role_t aws_role = BT_AWS_ROLE_NONE;
    bt_aws_codec_type_t aws_codec = 0;
    bt_sink_srv_am_a2dp_codec_t am_a2dp_codec = {{0}};
#endif /* __BT_AWS_SUPPORT__ */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (ptr_callback_amm == NULL) {
        ptr_callback_amm = (bt_sink_srv_am_amm_struct *)pvPortMalloc(sizeof(bt_sink_srv_am_amm_struct));
    }
    if (background_ptr->type == NONE) {
        g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;
        background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
    } else if (background_ptr->type == PCM) {
        pcm_stream_node = &(background_ptr->local_context.pcm_format.stream);
        eIn_out = background_ptr->local_context.pcm_format.in_out;
        if (eIn_out == STREAM_OUT) {
            bt_sink_srv_report("[Sink][AM]register_stream_out_callback: 0x%x\r\n", aud_stream_out_callback);
            hal_audio_register_stream_out_callback(aud_stream_out_callback, NULL);
            hal_audio_set_stream_out_sampling_rate(pcm_stream_node->stream_sample_rate);
            hal_audio_set_stream_out_channel_number(pcm_stream_node->stream_channel);
            hal_audio_set_stream_out_device(stream_out->audio_device);
            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                vol_info.type = VOL_PCM;
                vol_info.vol_info.pcm_vol_info.dev = stream_out->audio_device;
                vol_info.vol_info.pcm_vol_info.lev = stream_out->audio_volume;
                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
            }
            #else
            aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
            #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
            hal_audio_mute_stream_out(stream_out->audio_mute);
            eResult = hal_audio_write_stream_out(pcm_stream_node->buffer, pcm_stream_node->size);
        } else if (eIn_out == STREAM_IN) {
            bt_sink_srv_report("[Sink][AM]register_stream_in_callback: 0x%x\r\n", aud_stream_in_callback);
            hal_audio_register_stream_in_callback(aud_stream_in_callback, NULL);
            hal_audio_set_stream_in_sampling_rate(pcm_stream_node->stream_sample_rate);
            hal_audio_set_stream_in_channel_number(pcm_stream_node->stream_channel);
            hal_audio_set_stream_in_device(stream_in->audio_device);
            /* should add this case ?? */
            aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, stream_in->audio_device, (bt_sink_srv_am_volume_level_t)stream_in->audio_volume);
            hal_audio_mute_stream_in(stream_in->audio_mute);
            eResult = hal_audio_read_stream_in(pcm_stream_node->buffer, pcm_stream_node->size);
        }
        if (eResult == HAL_AUDIO_STATUS_ERROR) {
            background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
        } else {
            if (eIn_out == STREAM_OUT) {
                hal_audio_start_stream_out(background_ptr->audio_path_type);
            } else if (eIn_out == STREAM_IN) {
                hal_audio_start_stream_in(background_ptr->audio_path_type);
            }
            g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;
            background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
        }
    } else if (background_ptr->type == A2DP) {
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_A2DP;
            vol_info.vol_info.a2dp_vol_info.dev = stream_out->audio_device;
            vol_info.vol_info.a2dp_vol_info.lev = stream_out->audio_volume;
            bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
        }
        #else
        aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
        hal_audio_set_stream_in_device(stream_in->audio_device);
        bt_codec_am_a2dp_sink_open(&(background_ptr->local_context.a2dp_format.a2dp_codec));
    } else if (background_ptr->type == HFP) {
        if (background_ptr->local_context.hfp_format.hfp_codec.type == BT_HFP_CODEC_TYPE_CVSD) {
            hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
            hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
        } else if (background_ptr->local_context.hfp_format.hfp_codec.type == BT_HFP_CODEC_TYPE_MSBC) {
            hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);
            hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);
        }
        hal_audio_set_stream_out_channel_number(HAL_AUDIO_MONO);
        hal_audio_set_stream_in_channel_number(HAL_AUDIO_MONO);
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_HFP;
            vol_info.vol_info.hfp_vol_info.codec = background_ptr->local_context.hfp_format.hfp_codec.type;
            vol_info.vol_info.hfp_vol_info.dev_in = stream_in->audio_device;
            vol_info.vol_info.hfp_vol_info.dev_out = stream_out->audio_device;
            vol_info.vol_info.hfp_vol_info.lev_in = stream_in->audio_volume;
            vol_info.vol_info.hfp_vol_info.lev_out = stream_out->audio_volume;
            bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
        }
        #else
        aud_set_volume_level(STREAM_OUT, AUD_VOL_SPEECH, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_HFP;
            vol_info.vol_info.hfp_vol_info.codec = background_ptr->local_context.hfp_format.hfp_codec.type;
            vol_info.vol_info.hfp_vol_info.dev_in = stream_in->audio_device;
            vol_info.vol_info.hfp_vol_info.dev_out = stream_out->audio_device;
            vol_info.vol_info.hfp_vol_info.lev_in = stream_in->audio_volume;
            vol_info.vol_info.hfp_vol_info.lev_out = stream_out->audio_volume;
            bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
        }
        #else
        aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, stream_in->audio_device, (bt_sink_srv_am_volume_level_t)stream_in->audio_volume);
        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
        hal_audio_set_stream_out_device(stream_out->audio_device);
        hal_audio_set_stream_in_device(stream_in->audio_device);

#ifdef __BT_SINK_SRV_AUDIO_TUNING__
        aud_tunning_p = bt_sink_srv_audio_tunning_get_context();

        if ((aud_tunning_p->flag & TUNNING_FLAG_INIT) &&
                (aud_tunning_p->flag & TUNNING_FLAG_ON)) {
            bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_HF, TUNNING_TYPE_VOL);
            bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_HF, TUNNING_TYPE_DEV);
        }
#endif /* __BT_SINK_SRV_AUDIO_TUNING__ */

        bt_codec_am_hfp_open(&(background_ptr->local_context.hfp_format.hfp_codec));

#ifdef MTK_PROMPT_SOUND_ENABLE
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HFP_ON, NULL);
#endif
    } else if (background_ptr->type == AWS) {
        #ifdef __BT_AWS_SUPPORT__
        aws_role = background_ptr->local_context.aws_format.aws_codec.role;
        aws_codec = background_ptr->local_context.aws_format.aws_codec.codec_cap.type;
        /* AWS A2DP */
        if ((aws_role == BT_AWS_ROLE_SINK) &&
            ((aws_codec == BT_AWS_CODEC_TYPE_SBC) || (aws_codec == BT_AWS_CODEC_TYPE_AAC))) {
            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                vol_info.type = VOL_A2DP;
                vol_info.vol_info.a2dp_vol_info.dev = stream_out->audio_device;
                vol_info.vol_info.a2dp_vol_info.lev = stream_out->audio_volume;
                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
            }
            #else
            aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
            #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
            hal_audio_set_stream_in_device(stream_in->audio_device);
            am_a2dp_codec.role = BT_A2DP_SINK;
            if (aws_codec == BT_AWS_CODEC_TYPE_SBC) {
                am_a2dp_codec.codec_cap.type = BT_A2DP_CODEC_SBC;
            } else if (aws_codec == BT_AWS_CODEC_TYPE_AAC) {
                am_a2dp_codec.codec_cap.type = BT_A2DP_CODEC_AAC;
            } else {
            }
            am_a2dp_codec.codec_cap.sep_type = BT_A2DP_SINK;
            am_a2dp_codec.codec_cap.length = background_ptr->local_context.aws_format.aws_codec.codec_cap.length;
            memcpy(&(am_a2dp_codec.codec_cap.codec),
                    &(background_ptr->local_context.aws_format.aws_codec.codec_cap.codec),
                    background_ptr->local_context.aws_format.aws_codec.codec_cap.length);

            bt_codec_am_a2dp_sink_open(&am_a2dp_codec);
        } else if ((aws_role == BT_AWS_ROLE_SINK) &&
            (aws_codec == BT_AWS_CODEC_TYPE_MP3)) {
            bt_sink_srv_am_files_format_t file_fmt;
            file_fmt.file_type = FILE_MP3;
            am_files_codec_open(&file_fmt); // need to adjust yiquan
            aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
            hal_audio_set_stream_in_device(stream_in->audio_device);
            hal_audio_set_stream_out_device(stream_out->audio_device);
        }
        #endif
    } else if (background_ptr->type == FILES) {
        am_files_codec_open(&(background_ptr->local_context.files_format));
        hal_audio_set_stream_out_device(stream_out->audio_device);
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_MP3;
            vol_info.vol_info.mp3_vol_info.dev = stream_out->audio_device;
            vol_info.vol_info.mp3_vol_info.lev = stream_out->audio_volume;
            bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
        }
        #else
        aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
    } else {
        /* trace error */
    }
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_set_suspend
 * DESCRIPTION
 *  This function set to SUSPEND state when be interrupt by others.
 * PARAMETERS
 *	lead             [IN]
 *  background_ptr   [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void am_audio_set_suspend(bt_sink_srv_am_type_t lead, bt_sink_srv_am_background_t *background_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_cb_sub_msg_t   msg_id = AUD_SUSPEND_BY_NONE;
    bt_sink_srv_am_id_t             bAud_id = background_ptr->aud_id;
    bt_sink_srv_am_stream_type_t    eIn_out;
    void *parm = NULL;
    #ifdef __BT_AWS_SUPPORT__
    bt_aws_role_t aws_role = BT_AWS_ROLE_NONE;
    bt_aws_codec_type_t aws_codec = 0;
    #endif /* __BT_AWS_SUPPORT__ */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (lead) {
        case NONE:
            msg_id = AUD_SUSPEND_BY_NONE;
            break;
        case PCM:
            msg_id = AUD_SUSPEND_BY_PCM;
            break;
        case A2DP:
            msg_id = AUD_SUSPEND_BY_A2DP;
            break;
        case HFP:
            msg_id = AUD_SUSPEND_BY_HFP;
            break;
        case FILES:
            msg_id = AUD_SUSPEND_BY_FILES;
            break;
        case AWS:
            msg_id = AUD_SUSPEND_BY_AWS;
            break;
        default:
            break;
    }

    if (background_ptr->type == NONE) {

    } else if (background_ptr->type == PCM) {
        eIn_out = background_ptr->local_context.pcm_format.in_out;
        if (eIn_out == STREAM_OUT) {
            hal_audio_stop_stream_out();
        } else if (eIn_out == STREAM_IN) {
            hal_audio_stop_stream_in();
        }
    } else if (background_ptr->type == A2DP) {
        if (g_rSink_state == A2DP_SINK_CODEC_OPEN) {
            if (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR) {
                #ifdef __AM_DEBUG_INFO__
                bt_sink_srv_report("[Sink][AM][ERROR] Suspend A2DP");
                #endif
                return;
            }
        } else if (g_rSink_state == A2DP_SINK_CODEC_PLAY) {
            if ((bt_codec_am_a2dp_sink_stop(background_ptr->aud_id) == BT_CODEC_MEDIA_STATUS_ERROR) ||
                    (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR)) {
                #ifdef __AM_DEBUG_INFO__
                bt_sink_srv_report("[Sink][AM][ERROR] Suspend A2DP");
                #endif
                return;
            }
        }
    } else if (background_ptr->type == HFP) {
        if (bt_codec_am_hfp_stop() == BT_CODEC_MEDIA_STATUS_ERROR) {
#ifdef __AM_DEBUG_INFO__
            bt_sink_srv_report("[Sink][AM][ERROR] Suspend HFP");
#endif
            return;
        }
    } else if (background_ptr->type == AWS) {
        #ifdef __BT_AWS_SUPPORT__
        aws_role = background_ptr->local_context.aws_format.aws_codec.role;
        aws_codec = background_ptr->local_context.aws_format.aws_codec.codec_cap.type;
        /* AWS A2DP */
        if ((aws_role == BT_AWS_ROLE_SINK) &&
            ((aws_codec == BT_AWS_CODEC_TYPE_SBC) || (aws_codec == BT_AWS_CODEC_TYPE_AAC))) {
            if (g_rSink_state == A2DP_SINK_CODEC_OPEN) {
                if (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR) {
                    #ifdef __AM_DEBUG_INFO__
                    bt_sink_srv_report("[Sink][AM][ERROR][AWS]Suspend A2DP 1");
                    #endif
                    return;
                }
            } else if (g_rSink_state == A2DP_SINK_CODEC_PLAY) {
                if ((bt_codec_am_a2dp_sink_stop(background_ptr->aud_id) == BT_CODEC_MEDIA_STATUS_ERROR) ||
                        (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR)) {
                    #ifdef __AM_DEBUG_INFO__
                    bt_sink_srv_report("[Sink][AM][ERROR][AWS]Suspend A2DP 2");
                    #endif
                    return;
                }
            }
        } else if ((aws_role == BT_AWS_ROLE_SINK) && (aws_codec == BT_AWS_CODEC_TYPE_MP3)) {
            am_files_codec_close();
        }
        #endif /* __BT_AWS_SUPPORT__ */
    } else if (background_ptr->type == FILES) {
        am_files_codec_close();
    }

    g_rAm_aud_id[bAud_id].use = ID_SUSPEND_STATE;
    if (AUD_SUSPEND_BY_NONE == msg_id || AUD_SUSPEND_BY_HFP == msg_id) {
        parm = (void *) &g_int_dev_addr;
    }

    background_ptr->notify_cb(background_ptr->aud_id, AUD_SUSPEND_BY_IND, msg_id, parm);
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_search_suspended
 * DESCRIPTION
 *  This function is used to find the highest suspend event to resume it when no player.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void am_audio_search_suspended(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *recoder_current_ptr = NULL;
    bt_sink_srv_am_id_t bAud_id;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    recoder_current_ptr = g_rBackground_head.next;
    while (recoder_current_ptr != NULL) {
        bAud_id = recoder_current_ptr->aud_id;
        if (g_rAm_aud_id[bAud_id].use == ID_SUSPEND_STATE) {
            g_rAm_aud_id[bAud_id].use = ID_RESUME_STATE;
            g_prCurrent_resumer = recoder_current_ptr;
#ifdef RTOS_TIMER
            xTimerStart(g_xTimer_am, 0);
#endif
            recoder_current_ptr->notify_cb(recoder_current_ptr->aud_id, AUD_RESUME_IND, AUD_RESUME_IDLE_STATE, NULL);
            break;
        }
        recoder_current_ptr = recoder_current_ptr->next;
    }
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_set_stop
 * DESCRIPTION
 *  This function is used to stop audio handler.
 * PARAMETERS
 *  background_ptr   [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void am_audio_set_stop(bt_sink_srv_am_background_t *background_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_stream_type_t    eIn_out;
    #ifdef __BT_AWS_SUPPORT__
    bt_aws_role_t aws_role = BT_AWS_ROLE_NONE;
    bt_aws_codec_type_t aws_codec = 0;
    #endif /* __BT_AWS_SUPPORT__ */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (background_ptr->type == NONE) {
    } else if (background_ptr->type == PCM) {
        eIn_out = background_ptr->local_context.pcm_format.in_out;
        if (eIn_out == STREAM_OUT) {
            hal_audio_stop_stream_out();
        } else if (eIn_out == STREAM_IN) {
            hal_audio_stop_stream_in();
        }
    } else if (background_ptr->type == A2DP) {
        bt_sink_srv_report("[Sink][AM]A2DP sink state: %d", g_rSink_state);
        if (g_rSink_state == A2DP_SINK_CODEC_STOP) {
            if (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR) {
                #ifdef __AM_DEBUG_INFO__
                bt_sink_srv_report("[Sink][AM][ERROR] A2DP codec close fail");
                #endif
                background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
                return;
            }
        } else if (g_rSink_state == A2DP_SINK_CODEC_OPEN) {
            if (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR) {
                #ifdef __AM_DEBUG_INFO__
                bt_sink_srv_report("[Sink][AM][ERROR] A2DP codec close fail");
                #endif
                background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
                return;
            }
        } else if (g_rSink_state != A2DP_SINK_CODEC_CLOSE) {
            #ifdef __AM_DEBUG_INFO__
            bt_sink_srv_report("[Sink][AM][ERROR] A2DP codec NOT CLOSE");
            #endif
            background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
            return;
        }
    } else if (background_ptr->type == HFP) {
        if (bt_codec_am_hfp_stop() == BT_CODEC_MEDIA_STATUS_ERROR) {
#ifdef __AM_DEBUG_INFO__
            bt_sink_srv_report("[Sink][AM][ERROR] Close HFP");
#endif
            background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
            return;
        }
#ifdef MTK_PROMPT_SOUND_ENABLE
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HFP_OFF, NULL);
#endif
    } else if (background_ptr->type == AWS){
        #ifdef __BT_AWS_SUPPORT__
        aws_role = background_ptr->local_context.aws_format.aws_codec.role;
        aws_codec = background_ptr->local_context.aws_format.aws_codec.codec_cap.type;
        /* AWS A2DP */
        if ((aws_role == BT_AWS_ROLE_SINK) &&
            ((aws_codec == BT_AWS_CODEC_TYPE_SBC) || (aws_codec == BT_AWS_CODEC_TYPE_AAC))) {
            bt_sink_srv_report("[Sink][AM][AWS]A2DP sink state: %d", g_rSink_state);
            if (g_rSink_state == A2DP_SINK_CODEC_STOP) {
                if (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR) {
                    #ifdef __AM_DEBUG_INFO__
                    bt_sink_srv_report("[Sink][AM][ERROR][AWS]A2DP codec close fail");
                    #endif
                    background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
                    return;
                }
            } else if (g_rSink_state == A2DP_SINK_CODEC_OPEN) {
                if (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR) {
                    #ifdef __AM_DEBUG_INFO__
                    bt_sink_srv_report("[Sink][AM][ERROR][AWS]A2DP codec close fail");
                    #endif
                    background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
                    return;
                }
            } else if (g_rSink_state != A2DP_SINK_CODEC_CLOSE) {
                #ifdef __AM_DEBUG_INFO__
                bt_sink_srv_report("[Sink][AM][ERROR][AWS]A2DP codec NOT CLOSE");
                #endif
                background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
                return;
            }
        }
        #endif /* __BT_AWS_SUPPORT__ */
    } else if (background_ptr->type == FILES) {
        am_files_codec_close();
        background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_FILE_EVENT_DATA_END, NULL);
    }

    g_rAm_aud_id[background_ptr->aud_id].use = ID_IDLE_STATE;
    background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
}

/*****************************************************************************
 * FUNCTION
 *  aud_initial
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_hal_result_t
 *****************************************************************************/
static void aud_initial(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memset(&g_rAm_aud_id[0], 0, sizeof(bt_sink_srv_am_aud_id_type_t)*AM_REGISTER_ID_TOTAL);
    memset(&g_prA2dp_sink_event_handle, 0, sizeof(g_prA2dp_sink_event_handle));
}

/*****************************************************************************
 * FUNCTION
 *  aud_set_open_stream_req_hdlr
 * DESCRIPTION
 *  Get register ID
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_open_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_srv_am_id_t bAud_id = am_background_temp->aud_id;
    bt_sink_srv_am_background_t *Background_ptr = NULL;
    bt_sink_srv_am_background_t *recoder_current_ptr = NULL, *recoder_previous_ptr = NULL;
    bt_sink_srv_am_hal_result_t eResult = HAL_AUDIO_STATUS_ERROR;
    uint32_t g_AudMem_size = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_aud_id_num == 1) {
        eResult = hal_audio_get_memory_size(&g_AudMem_size);
        if (eResult == HAL_AUDIO_STATUS_OK) {
            if (!aud_memory) {
#ifndef WIN32_UT
                aud_memory = (uint16_t *)pvPortMalloc(g_AudMem_size);
#else
                aud_memory = (uint16_t *)malloc(g_AudMem_size);
#endif
            }
            hal_audio_set_memory(aud_memory);
        } else {
#ifdef __AM_DEBUG_INFO__
            bt_sink_srv_report("[Sink][AM][ERROR] Get memoey size");
#endif
        }
    }

#ifndef WIN32_UT
    Background_ptr = (bt_sink_srv_am_background_t *)pvPortMalloc(sizeof(bt_sink_srv_am_background_t));
#else
    Background_ptr = (bt_sink_srv_am_background_t *)malloc(sizeof(bt_sink_srv_am_background_t));
#endif
    if (Background_ptr) {
    Background_ptr->type = NONE;
    Background_ptr->aud_id = bAud_id;
    Background_ptr->priority = am_background_temp->priority;
    Background_ptr->notify_cb = am_background_temp->notify_cb;

    recoder_previous_ptr = &g_rBackground_head;
    recoder_current_ptr = g_rBackground_head.next;
    while ((recoder_current_ptr != NULL) && (recoder_current_ptr->priority > Background_ptr->priority)) {
        recoder_previous_ptr = recoder_current_ptr;
        recoder_current_ptr = recoder_current_ptr->next;
    }
    Background_ptr->prior = recoder_previous_ptr;
    Background_ptr->next = recoder_current_ptr;
    recoder_previous_ptr->next = Background_ptr;
    if (recoder_current_ptr != NULL) {
        recoder_current_ptr->prior = Background_ptr;
    }
    g_rAm_aud_id[bAud_id].contain_ptr = Background_ptr;
    //Background_ptr->notify_cb(am_background_temp->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
    }

    //printf("[AudM]open_hdr-id: %d, num: %d, use: %d, b_ptr: 0x%x\n", bAud_id,
        //g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)Background_ptr);
    bt_sink_srv_report("[AudM]open_hdr-id: %d, num: %d, use: %d, b_ptr: 0x%x", bAud_id,
        g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)Background_ptr);
}

/*****************************************************************************
 * FUNCTION
 *  aud_set_play_stream_req_hdlr
 * DESCRIPTION
 *  Start to play the specified audio handler.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_play_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_srv_am_id_t         bAud_id = am_background_temp->aud_id;
    bt_sink_srv_am_background_t *recoder_current_ptr = g_rAm_aud_id[bAud_id].contain_ptr;
    bt_sink_srv_am_background_t *recoder_high_t = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prCurrent_resumer != NULL) {
#ifdef RTOS_TIMER
        xTimerStop(g_xTimer_am, 20);
        g_lExpire_count = 0;
#endif
        g_rAm_aud_id[g_prCurrent_resumer->aud_id].use = ID_IDLE_STATE;
        g_prCurrent_resumer = NULL;
    }

    //printf("[AudM]play_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x\n",
        //bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);
    bt_sink_srv_report("[AudM]play_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x",
        bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);

    if (recoder_current_ptr) {
    recoder_current_ptr->type = am_background_temp->type;
    recoder_current_ptr->audio_path_type = am_background_temp->audio_path_type;
    memcpy(&(recoder_current_ptr->local_context), &(am_background_temp->local_context), sizeof(bt_sink_srv_am_codec_t));
    memcpy(&(recoder_current_ptr->audio_stream_in), &(am_background_temp->audio_stream_in), sizeof(bt_sink_srv_am_audio_stream_in_t));
    memcpy(&(recoder_current_ptr->audio_stream_out), &(am_background_temp->audio_stream_out), sizeof(bt_sink_srv_am_audio_stream_out_t));
    /* Find which is high priority */
    recoder_high_t = g_rBackground_head.next;
    while ((recoder_high_t != NULL) && (recoder_high_t->priority > recoder_current_ptr->priority)) {
        recoder_high_t = recoder_high_t->next;
    }
    if (recoder_high_t->aud_id != bAud_id) {
        if (recoder_current_ptr->next != NULL) {
            recoder_current_ptr->next->prior = recoder_current_ptr->prior;
        }
        recoder_current_ptr->prior->next = recoder_current_ptr->next;
        recoder_high_t->prior->next = recoder_current_ptr;
        recoder_current_ptr->prior = recoder_high_t->prior;
        recoder_high_t->prior = recoder_current_ptr;
        recoder_current_ptr->next = recoder_high_t;
    }

#ifdef MTK_PROMPT_SOUND_ENABLE
    prompt_control_stop_tone();
    // bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_MIXER_TONE_STOP, NULL);
#endif
    if (g_prCurrent_player != NULL) {
        if (recoder_current_ptr->priority < g_prCurrent_player->priority) {
            g_rAm_aud_id[bAud_id].use = ID_SUSPEND_STATE;
            recoder_current_ptr->notify_cb(bAud_id, AUD_SUSPEND_IND, AUD_EMPTY, NULL);
        } else {
            am_audio_set_suspend(recoder_current_ptr->type, g_prCurrent_player);
            g_prCurrent_player = recoder_current_ptr;
            am_audio_set_play(g_prCurrent_player);
        }
    } else {
        g_prCurrent_player = recoder_current_ptr;
        am_audio_set_play(g_prCurrent_player);
    }
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_set_stop_stream_req_hdlr
 * DESCRIPTION
 *  Stop playing the specified audio handler.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_stop_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_srv_am_id_t bAud_id = am_background_temp->aud_id;
    bt_sink_srv_am_background_t *recoder_current_ptr = g_rAm_aud_id[bAud_id].contain_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //printf("[AudM]stop_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x\n",
        //bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);

    bt_sink_srv_report("[AudM]stop_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x",
        bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);

    g_prCurrent_player = NULL;
    am_audio_set_stop(recoder_current_ptr);
    am_audio_search_suspended();
}

/*****************************************************************************
 * FUNCTION
 *  aud_set_close_stream_req_hdlr
 * DESCRIPTION
 *  Close the opening audio handler.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_close_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_srv_am_id_t bAud_id = am_background_temp->aud_id;
    bt_sink_srv_am_background_t *recoder_current_ptr = g_rAm_aud_id[bAud_id].contain_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_rAm_aud_id[bAud_id].use == ID_RESUME_STATE) {
#ifdef RTOS_TIMER
        xTimerStop(g_xTimer_am, 20);
        g_lExpire_count = 0;
#endif
        g_prCurrent_resumer = NULL;
    }

    //printf("[AudM]close_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x\n",
        //bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);
    bt_sink_srv_report("[AudM]close_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x",
        bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);
    if (recoder_current_ptr) {
    g_rAm_aud_id[bAud_id].contain_ptr = NULL;
    //recoder_current_ptr->notify_cb(bAud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
    if (recoder_current_ptr->next != NULL) {
        recoder_current_ptr->next->prior = recoder_current_ptr->prior;
    }
    if (recoder_current_ptr->prior != NULL) {
        recoder_current_ptr->prior->next = recoder_current_ptr->next;
    }
#ifndef WIN32_UT
    vPortFree(recoder_current_ptr);
#else
    free(recoder_current_ptr);
#endif

    if (g_aud_id_num == 1) {
        if (aud_memory != NULL) {
#ifndef WIN32_UT
            vPortFree(aud_memory);
#else
            free(aud_memory);
#endif
            aud_memory = NULL;
        }
    }
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_set_volume_stream_out_req_hdlr
 * DESCRIPTION
 *  Set audio in/out volume.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_volume_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_srv_am_volume_level_t level;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (!g_prCurrent_player) {
        return ;
    }

    if (am_background_temp->in_out == STREAM_OUT) {
        level = (bt_sink_srv_am_volume_level_t)am_background_temp->audio_stream_out.audio_volume;
        g_prCurrent_player->audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t)level;
        if (g_prCurrent_player->type == HFP) {
            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
                vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
                vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
                vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
            }
            #else
            aud_set_volume_level(STREAM_OUT, AUD_VOL_SPEECH, g_prCurrent_player->audio_stream_out.audio_device, level);
            #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
        } else {
            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                if (g_prCurrent_player->type == PCM) {
                    vol_info.type = VOL_PCM;
                    vol_info.vol_info.pcm_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.pcm_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                } else if (g_prCurrent_player->type == A2DP) {
                    vol_info.type = VOL_A2DP;
                    vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                } else if (g_prCurrent_player->type == FILES) {
                    vol_info.type = VOL_MP3;
                    vol_info.vol_info.mp3_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.mp3_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                } else if (g_prCurrent_player->type == AWS) {
                    vol_info.type = VOL_A2DP;
                    vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                } else {
                    // Fix Coverity issue
                    vol_info.type = VOL_DEF;
                    vol_info.vol_info.def_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.def_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                    
                    bt_sink_srv_report("[AudM]volume. Unknown type: %d\n", g_prCurrent_player->type);
                }

                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
            }
            #else
            aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, g_prCurrent_player->audio_stream_out.audio_device, level);
            #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
        }
    } else if (am_background_temp->in_out == STREAM_IN) {
        if (g_prCurrent_player->type == HFP) {
        level = (bt_sink_srv_am_volume_level_t)am_background_temp->audio_stream_in.audio_volume;
        g_prCurrent_player->audio_stream_in.audio_volume = (bt_sink_srv_am_volume_level_in_t)level;
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_HFP;
            vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
            vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
            vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
            vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
            vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
            bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
        }
        #else
        aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, g_prCurrent_player->audio_stream_in.audio_device, level);
        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
        } else {
            level = (bt_sink_srv_am_volume_level_t)am_background_temp->audio_stream_in.audio_volume;
            g_prCurrent_player->audio_stream_in.audio_volume = (bt_sink_srv_am_volume_level_in_t)level;
            aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, g_prCurrent_player->audio_stream_in.audio_device, level);
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_mute_device_stream_req_hdlr
 * DESCRIPTION
 *	Mute audio input/output device.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_mute_device_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (am_background_temp->in_out == STREAM_OUT) {
        g_prCurrent_player->audio_stream_out.audio_mute = am_background_temp->audio_stream_out.audio_mute;
        hal_audio_mute_stream_out(g_prCurrent_player->audio_stream_out.audio_mute);
    } else if (am_background_temp->in_out == STREAM_IN) {
        g_prCurrent_player->audio_stream_in.audio_mute = am_background_temp->audio_stream_in.audio_mute;
        hal_audio_mute_stream_out(g_prCurrent_player->audio_stream_in.audio_mute);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_config_device_stream_req_hdlr
 * DESCRIPTION
 *	Set audio input/output device.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_config_device_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (am_background_temp->in_out == STREAM_OUT) {
        g_prCurrent_player->audio_stream_out.audio_device = am_background_temp->audio_stream_out.audio_device;
        hal_audio_mute_stream_out(TRUE);
        hal_audio_set_stream_out_device(g_prCurrent_player->audio_stream_out.audio_device);
        hal_audio_mute_stream_out(FALSE);
    } else if (am_background_temp->in_out == STREAM_IN) {
        g_prCurrent_player->audio_stream_in.audio_device = am_background_temp->audio_stream_in.audio_device;
        hal_audio_mute_stream_in(TRUE);
        hal_audio_set_stream_in_device(g_prCurrent_player->audio_stream_out.audio_device);
        hal_audio_mute_stream_in(FALSE);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_rw_stream_data_req_hdlr
 * DESCRIPTION
 *	Write data to audio output for palyback / Read data to audio input for record.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_rw_stream_data_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t     *am_background_temp = &(amm_ptr->background_info);
    uint8_t                     bAud_id = am_background_temp->aud_id;
    bt_sink_srv_am_hal_result_t     eResult = HAL_AUDIO_STATUS_ERROR;
    bt_sink_srv_am_stream_type_t    eIn_out;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_rAm_aud_id[bAud_id].use == ID_RESUME_STATE) {
#ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM] Resume to play");
#endif
#ifdef RTOS_TIMER
        xTimerStop(g_xTimer_am, 20);
        g_lExpire_count = 0;
#endif
        g_prCurrent_player = g_prCurrent_resumer;
        am_audio_set_play(g_prCurrent_player);
        g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;
        g_prCurrent_resumer = NULL;
    } else {
#ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM] Continue playing");
#endif
        eIn_out = g_prCurrent_player->local_context.pcm_format.in_out;
        if (eIn_out == STREAM_OUT) {
            eResult = hal_audio_write_stream_out(am_background_temp->local_context.pcm_format.stream.buffer,
                                                 am_background_temp->local_context.pcm_format.stream.size);
            if (eResult == HAL_AUDIO_STATUS_OK) {
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
                return;
            }
        } else if (eIn_out == STREAM_IN) {
            eResult = hal_audio_read_stream_in(am_background_temp->local_context.pcm_format.stream.buffer,
                                               am_background_temp->local_context.pcm_format.stream.size);
            if (eResult == HAL_AUDIO_STATUS_OK) {
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
                return;
            }
        }
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_query_stream_len_req_hdlr
 * DESCRIPTION
 *	Query available input/output data length.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_query_stream_len_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t     *am_background_temp = &(amm_ptr->background_info);
    bt_sink_srv_am_id_t             bAud_id = am_background_temp->aud_id;
    bt_sink_srv_am_background_t     *recoder_current_ptr = g_rAm_aud_id[bAud_id].contain_ptr;
    bt_sink_srv_am_hal_result_t     eResult = HAL_AUDIO_STATUS_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (am_background_temp->in_out == STREAM_OUT) {
        eResult = hal_audio_get_stream_out_sample_count(am_background_temp->data_length_ptr);
    } else if (am_background_temp->in_out == STREAM_IN) {
        eResult = hal_audio_get_stream_in_sample_count(am_background_temp->data_length_ptr);
    }
    /* CallBack to AP */
    if (eResult == HAL_AUDIO_STATUS_OK) {
        recoder_current_ptr->notify_cb(bAud_id, AUD_SELF_CMD_REQ,  AUD_CMD_COMPLETE, NULL);
    } else {
        recoder_current_ptr->notify_cb(bAud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_bt_codec_a2dp_callback
 * DESCRIPTION
 *  This function is used to send L1Audio events to A.M.
 * PARAMETERS
 *  handle           [IN]
 *  event_id         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_bt_codec_a2dp_callback(bt_media_handle_t *handle, bt_sink_srv_am_bt_event_t event_id)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prCurrent_player->type == A2DP) {
        #ifdef __BT_AWS_SUPPORT__
        if (event_id == BT_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW) {
            g_aws_skew_loop_count++;
            if (g_aws_skew_loop_count >= BT_SINK_SRV_AWS_SKEW_LOOP_COUNT) {
                g_aws_skew_loop_count = 0;
                g_prCurrent_player->local_context.a2dp_format.a2dp_event = event_id;
                bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_A2DP_PROC_IND,
                                         MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ,
                                         g_prCurrent_player,
                                         TRUE, ptr_callback_amm);
            }
        } else if (event_id == BT_CODEC_MEDIA_AWS_CHECK_UNDERFLOW) {
           if (g_aws_skew_loop_count == BT_SINK_SRV_AWS_SKEW_LOOP_1ST_COUNT) {
               g_prCurrent_player->local_context.a2dp_format.a2dp_event = event_id;
                bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_A2DP_PROC_IND,
                                         MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ,
                                         g_prCurrent_player,
                                         TRUE, ptr_callback_amm);
           }
        } else {
            g_prCurrent_player->local_context.a2dp_format.a2dp_event = event_id;
            bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_A2DP_PROC_IND,
                                     MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ,
                                     g_prCurrent_player,
                                     TRUE, ptr_callback_amm);
        }
        #else
        g_prCurrent_player->local_context.a2dp_format.a2dp_event = event_id;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_A2DP_PROC_IND,
                                 MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ,
                                 g_prCurrent_player,
                                 TRUE, ptr_callback_amm);
        #endif /* __BT_AWS_SUPPORT__ */
    } else if (g_prCurrent_player->type == AWS) {
        #ifdef __BT_AWS_SUPPORT__
        g_prCurrent_player->local_context.aws_format.aws_event = event_id;
        if (event_id == BT_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW) {
            g_aws_skew_loop_count++;
            if (g_aws_skew_loop_count >= BT_SINK_SRV_AWS_SKEW_LOOP_COUNT) {
                g_aws_skew_loop_count = 0;
                bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_AWS_A2DP_PROC_IND,
                                         MSG_ID_MEDIA_AWS_A2DP_PROC_CALL_EXT_REQ,
                                         g_prCurrent_player,
                                         TRUE, ptr_callback_amm);
            }
        } else if (event_id == BT_CODEC_MEDIA_AWS_CHECK_UNDERFLOW) {
            if (g_aws_skew_loop_count == BT_SINK_SRV_AWS_SKEW_LOOP_1ST_COUNT) {
                bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_AWS_A2DP_PROC_IND,
                                         MSG_ID_MEDIA_AWS_A2DP_PROC_CALL_EXT_REQ,
                                         g_prCurrent_player,
                                         TRUE, ptr_callback_amm);
           }
        } else {
            bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_AWS_A2DP_PROC_IND,
                                     MSG_ID_MEDIA_AWS_A2DP_PROC_CALL_EXT_REQ,
                                     g_prCurrent_player,
                                     TRUE, ptr_callback_amm);
        }
        #endif /* __BT_AWS_SUPPORT__ */
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_bt_codec_hfp_callback
 * DESCRIPTION
 *  This function is used to send L1Audio events to A.M.
 * PARAMETERS
 *  handle           [IN]
 *  event_id         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_bt_codec_hfp_callback(bt_media_handle_t *handle, bt_sink_srv_am_bt_event_t event_id)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prCurrent_player->type == HFP) {
        g_prCurrent_player->local_context.hfp_format.hfp_event = event_id;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_HFP_EVENT_IND,
                                 MSG_ID_MEDIA_HFP_EVENT_CALL_EXT_REQ,
                                 g_prCurrent_player,
                                 FALSE, ptr_callback_amm);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_stream_out_callback
 * DESCRIPTION
 *  This callback function is notified A.M. for stream-out by AUD HISR.
 * PARAMETERS
 *  event            [OUT]
 *  user_data        [OUT]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_stream_out_callback(bt_sink_srv_am_event_result_t event, void *user_data)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prCurrent_player == NULL) {
        return;
    }
    if (g_prCurrent_player->type == PCM) {
        g_prCurrent_player->local_context.pcm_format.event = event;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_STREAM_EVENT_IND,
                                 MSG_ID_MEDIA_EVENT_STREAM_OUT_CALL_EXT_REQ,
                                 g_prCurrent_player,
                                 TRUE, ptr_callback_amm);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_stream_in_callback
 * DESCRIPTION
 *  This callback function is notified A.M. for stream-in by AUD HISR.
 * PARAMETERS
 *  event            [OUT]
 *  user_data        [OUT]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_stream_in_callback(bt_sink_srv_am_event_result_t event, void *user_data)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prCurrent_player == NULL) {
        return;
    }
    if (g_prCurrent_player->type == PCM) {
        g_prCurrent_player->local_context.pcm_format.event = event;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_STREAM_EVENT_IND,
                                 MSG_ID_MEDIA_EVENT_STREAM_IN_CALL_EXT_REQ,
                                 g_prCurrent_player,
                                 TRUE, ptr_callback_amm);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_process_a2dp_callback_hdlr
 * DESCRIPTION
 *	This function is used to handle A2DP process callback.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_process_a2dp_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_bt_event_t event_id = amm_ptr->background_info.local_context.a2dp_format.a2dp_event;
    //bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
#ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;
#endif
    bt_sink_srv_am_device_set_t dev;
    bt_sink_srv_am_volume_level_out_t vol;
    uint32_t digital_vol = 0x7FFF, analog_vol = 0x0002;
#ifdef __BT_AWS_SUPPORT__
    uint32_t sampling_rate = 44100;
#endif

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prA2dp_sink_handle != NULL) {
        g_prA2dp_sink_handle->process(g_prA2dp_sink_handle, event_id);

        switch (event_id) {
            case BT_CODEC_MEDIA_ERROR: {
#ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                //g_prA2dp_sink_handle->stop(g_prA2dp_sink_handle);
                //g_rSink_state = A2DP_SINK_CODEC_STOP;
                dev = g_prCurrent_player->audio_stream_out.audio_device;
                vol = g_prCurrent_player->audio_stream_out.audio_volume;
                hal_audio_set_stream_out_device(dev);
                if (dev & DEVICE_LOUDSPEAKER) {
                    digital_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else if (dev & DEVICE_EARPHONE) {
                    digital_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else {
                    ;
                }
                if (g_am_volume_enable) {
                    bt_sink_srv_report("[sink][AM]-d_gain: 0x%x, a_gain: 0x%x", digital_vol, analog_vol);
                } else {
                    #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                    {
                        bt_sink_srv_audio_setting_vol_info_t vol_info;

                        vol_info.type = VOL_A2DP;
                        vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                        vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                        bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                    }
                    #else
                    hal_audio_set_stream_out_volume(digital_vol, analog_vol);
                    #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
                }
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_CODEC_RESTART, NULL);
#else
                g_prA2dp_sink_handle->stop(g_prA2dp_sink_handle);
                g_rSink_state = A2DP_SINK_CODEC_STOP;
                g_prA2dp_sink_handle->reset_share_buffer(g_prA2dp_sink_handle);
                dev = g_prCurrent_player->audio_stream_out.audio_device;
                vol = g_prCurrent_player->audio_stream_out.audio_volume;
                hal_audio_set_stream_out_device(dev);
                if (dev & DEVICE_LOUDSPEAKER) {
                    digital_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else if (dev & DEVICE_EARPHONE) {
                    digital_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else {
                    ;
                }
                if (g_am_volume_enable) {
                    ;
                } else {
                    #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                    {
                        bt_sink_srv_audio_setting_vol_info_t vol_info;

                        vol_info.type = VOL_A2DP;
                        vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                        vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                        bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                    }
                    #else
                    hal_audio_set_stream_out_volume(digital_vol, analog_vol);
                    #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
                }

                //eResult = g_prA2dp_sink_handle->play(g_prA2dp_sink_handle);
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_CODEC_RESTART, NULL);
                if (eResult == BT_CODEC_MEDIA_STATUS_ERROR) {
                    bt_sink_srv_report("[sink][AM] Error");
                }
                bt_sink_srv_report("[sink][AM]-restart play");
#endif /* __BT_SINK_SRV_AM_MED_LIST_SUPPORT__ */
                break;
            }

            #ifdef __BT_AWS_SUPPORT__
            case BT_CODEC_MEDIA_AWS_CHECK_UNDERFLOW: {
                bt_sink_srv_fetch_bt_offset();
                break;
            }

            case BT_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW: {
                if (BT_AWS_CODEC_TYPE_SBC == g_aws_codec_type) {
                    switch (g_aws_sample_rate) {
                        case 8:
                            sampling_rate = 16000;
                            break;
                        case 4:
                            sampling_rate = 32000;
                            break;
                        case 2:
                            sampling_rate = 44100;
                            break;
                        case 1:
                            sampling_rate = 48000;
                            break;

                        default:
                            sampling_rate = 44100;
                            break;
                    }
                } else if (BT_AWS_CODEC_TYPE_AAC == g_aws_codec_type){
                    switch (g_aws_sample_rate) {
                        case 0x800:
                            sampling_rate = 8000;
                            break;
                        case 0x400:
                            sampling_rate = 11025;
                            break;
                        case 0x200:
                            sampling_rate = 12000;
                            break;
                        case 0x100:
                            sampling_rate = 16000;
                            break;
                        case 0x80:
                            sampling_rate = 22050;
                            break;
                        case 0x40:
                            sampling_rate = 24000;
                            break;
                        case 0x20:
                            sampling_rate = 32000;
                            break;
                        case 0x10:
                            sampling_rate = 44100;
                            break;
                        case 0x8:
                            sampling_rate = 48000;
                            break;
                        default:
                            sampling_rate = 44100;
                            break;
                    }
                }
                bt_sink_srv_audio_sync_calc_t audio_calc;
                audio_calc.media_handle= g_prA2dp_sink_handle;
                audio_calc.sampling_rate = sampling_rate;
                audio_calc.type = g_aws_codec_type;
                bt_sink_srv_audio_clock_calibrate(&audio_calc);
                break;
            }
            #endif /* __BT_AWS_SUPPORT__ */

            case BT_CODEC_MEDIA_UNDERFLOW: {
                g_a2dp_underflow_loop_count++;
                if (g_a2dp_underflow_loop_count >= BT_SINK_SRV_AM_MAX_UNDERFLOW_COUNT) {
                    g_a2dp_underflow_loop_count = 0;
                    g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_AWS_UNDERFLOW, NULL);
                }
                break;
            }

            default:
                break;
        }
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, (bt_sink_srv_am_cb_sub_msg_t)event_id, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_process_hfp_callback_hdlr
 * DESCRIPTION
 *	This function is used to inform HF AP about hf error event.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_process_hfp_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_bt_event_t event_id = amm_ptr->background_info.local_context.hfp_format.hfp_event;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prA2dp_sink_handle != NULL) {
        if (event_id == BT_CODEC_MEDIA_ERROR) {
            bt_codec_am_hfp_stop();
        }
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_HFP_EVENT_IND, (bt_sink_srv_am_cb_sub_msg_t)event_id, NULL);

    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_event_stream_callback_hdlr
 * DESCRIPTION
 *	This function is used to handle stream event callback.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_event_stream_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_event_result_t eEvent = amm_ptr->background_info.local_context.pcm_format.event;
    bt_sink_srv_am_id_t bAud_id = amm_ptr->background_info.aud_id;
    bt_sink_srv_am_stream_type_t    eIn_out;
    uint32_t                    data_length = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((g_prCurrent_player == NULL) || (bAud_id != g_prCurrent_player->aud_id)) {
        return;
    }
    if (g_prCurrent_player->type == PCM) {
        eIn_out = g_prCurrent_player->local_context.pcm_format.in_out;
        if (eEvent == HAL_AUDIO_EVENT_UNDERFLOW) {
            if (eIn_out == STREAM_OUT) {
                hal_audio_stop_stream_out();
            } else if (eIn_out == STREAM_IN) {
                hal_audio_stop_stream_in();
            }
            g_rAm_aud_id[g_prCurrent_player->aud_id].use = ID_IDLE_STATE;
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_UNDERFLOW, NULL);
        } else if (eEvent == HAL_AUDIO_EVENT_DATA_REQUEST) {
            if (eIn_out == STREAM_OUT) {
                hal_audio_get_stream_out_sample_count(&data_length);
            } else if (eIn_out == STREAM_IN) {
                hal_audio_get_stream_in_sample_count(&data_length);
            }
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_DATA_REQ, &data_length);
        } else if (eEvent == HAL_AUDIO_EVENT_ERROR) {
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_ERROR, NULL);
        } else if (eEvent == HAL_AUDIO_EVENT_NONE) {
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_UNDERFLOW, NULL);
        } else if (eEvent == HAL_AUDIO_EVENT_DATA_NOTIFICATION) {
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_DATA_NOTIFICATION, NULL);
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_process_dsp_nvdm_setting
 * DESCRIPTION
 *  This function is used to read setting value from nvram and set to dsp
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
#if (PRODUCT_VERSION == 2533)
static void aud_process_dsp_nvdm_setting(void)
{
	int i;

	if (external_dsp_is_power_on()) {
        const audio_eaps_t *am_speech_eaps = audio_nvdm_get_global_eaps_address();
        uint32_t download_index = 0;
        uint16_t download_value = 0;

        //audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);

        bt_sink_srv_report("[Sink][AM][EXTDSP]--cur: 0x%08x, type: 0x%08x, c_type: %d",
            g_prCurrent_player, g_prCurrent_player->type, g_prCurrent_player->local_context.hfp_format.hfp_codec.type);

        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
            if (g_prCurrent_player &&
               (g_prCurrent_player->type == HFP) &&
               (BT_HFP_CODEC_TYPE_MSBC == g_prCurrent_player->local_context.hfp_format.hfp_codec.type)) {
                if(am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].need_to_download_flag) {
                    download_index = (uint32_t)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].index_value;
                    download_value = (uint16_t)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].register_value;
                    external_dsp_write_parameter(download_index, download_value);
                    bt_sink_srv_report("[Sink][AM][EXTDSP]WB nvdm setting:i=%d, download_index=%d, download_value=0x%x", i, download_index, download_value);
                    vTaskDelay(20);
                }
            } else if (g_prCurrent_player &&
                      (g_prCurrent_player->type == HFP) &&
                      (BT_HFP_CODEC_TYPE_CVSD == g_prCurrent_player->local_context.hfp_format.hfp_codec.type)) {
                if(am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info[i].need_to_download_flag) {
                    download_index = (uint32_t)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info[i].index_value;
                    download_value = (uint16_t)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info[i].register_value;
                    external_dsp_write_parameter(download_index, download_value);
                    bt_sink_srv_report("[Sink][AM][EXTDSP]NB nvdm setting:i=%d, download_index=%d, download_value=0x%x", i, download_index, download_value);
                    vTaskDelay(20);
                }
            } else {
                bt_sink_srv_report("[Sink][AM][EXTDSP] error");
            }
        }
        //vPortFree(am_speech_eaps);


/*
		// get audio data
		audio_nvdm_get_external_dsp_register_value_by_memcpy(DSP_REGISTER_PROFILE_BT, &external_dsp_sending_path_register_value);

		// write to dsp
		for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_INDEX_AMOUNT; i++) {
			if(external_dsp_sending_path_register_value.need_to_download_index[i]) {
				external_dsp_write_parameter(i, external_dsp_sending_path_register_value.external_dsp_register_value[i]);
				bt_sink_srv_report("[Sink][AM][EXTDSP]nvdm setting:%d=>0x%x", i, external_dsp_sending_path_register_value.external_dsp_register_value[i]);
				vTaskDelay(20);
			}
		}
*/
	} else {
		bt_sink_srv_report("[Sink][AM][EXTDSP][ERROR] DSP power off");
	}
}
#endif

/*****************************************************************************
 * FUNCTION
 *  bt_get_write_buffer_a2dp_sink_codec
 * DESCRIPTION
 *  This function is used to process "get_write_buffer" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 *  buffer           [OUT]
 *	length           [OUT]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
static bt_status_t bt_get_write_buffer_a2dp_sink_codec(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (aud_id == g_prCurrent_player->aud_id) {
        g_prA2dp_sink_handle->get_write_buffer(g_prA2dp_sink_handle, buffer, length);
        return BT_CODEC_MEDIA_STATUS_OK;
    }
    return BT_CODEC_MEDIA_STATUS_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_write_data_done_a2dp_sink_codec
 * DESCRIPTION
 *  This function is used to process "write data done" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 *  length           [IN]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
static bt_status_t bt_write_data_done_a2dp_sink_codec(bt_sink_srv_am_id_t aud_id, uint32_t length)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (aud_id == g_prCurrent_player->aud_id) {
        g_prA2dp_sink_handle->write_data_done(g_prA2dp_sink_handle, length);
        return BT_CODEC_MEDIA_STATUS_OK;
    }
    return BT_CODEC_MEDIA_STATUS_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_finish_write_data_a2dp_sink_codec
 * DESCRIPTION
 *  This function is used to process "finish_write_data" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 *  length           [IN]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
static bt_status_t bt_finish_write_data_a2dp_sink_codec(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (aud_id == g_prCurrent_player->aud_id) {
        g_prA2dp_sink_handle->finish_write_data(g_prA2dp_sink_handle);
        return BT_CODEC_MEDIA_STATUS_OK;
    }
    return BT_CODEC_MEDIA_STATUS_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_get_free_space_a2dp_sink_codec
 * DESCRIPTION
 *  This function is used to process "get_free_space" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 *  length           [IN]
 * RETURNS
 *  int32_t
 *****************************************************************************/
static int32_t bt_get_free_space_a2dp_sink_codec(bt_sink_srv_am_id_t aud_id)
{
    if (aud_id == g_prCurrent_player->aud_id) {
        return g_prA2dp_sink_handle->get_free_space(g_prA2dp_sink_handle);
    }
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  bt_reset_share_buffer_a2dp_sink_codec
 * DESCRIPTION
 *  This function is used to process "reset_share_buffer" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_reset_share_buffer_a2dp_sink_codec(bt_sink_srv_am_id_t aud_id)
{
    if (aud_id == g_prCurrent_player->aud_id) {
        g_prA2dp_sink_handle->reset_share_buffer(g_prA2dp_sink_handle);
    }
}


/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_a2dp_sink_open
 * DESCRIPTION
 * 	This function is used to open codec for A2DP sink by BT APP
 * PARAMETERS
 *	a2dp_codec_t     [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_codec_am_a2dp_sink_open(bt_sink_srv_am_a2dp_codec_t *a2dp_codec_t)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_codec_a2dp_audio_t a2dp_codec;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (a2dp_codec_t->role) {
        case BT_A2DP_SINK:
            a2dp_codec.role = a2dp_codec_t->role;
            if (a2dp_codec_t->codec_cap.type == BT_A2DP_CODEC_SBC) {
                BT_A2DP_CONVERT_SBC_CODEC(&(a2dp_codec.codec_cap), &(a2dp_codec_t->codec_cap));
                #ifdef __BT_AWS_SUPPORT__
                g_aws_codec_type = BT_AWS_CODEC_TYPE_SBC;
                g_aws_sample_rate = a2dp_codec.codec_cap.codec.sbc.sample_rate;
                #endif /* __BT_AWS_SUPPORT__ */
            } else if (a2dp_codec_t->codec_cap.type == BT_A2DP_CODEC_AAC) {
                BT_A2DP_CONVERT_AAC_CODEC(&(a2dp_codec.codec_cap), &(a2dp_codec_t->codec_cap));
                #ifdef __BT_AWS_SUPPORT__
                g_aws_codec_type = BT_AWS_CODEC_TYPE_AAC;
                g_aws_sample_rate = a2dp_codec.codec_cap.codec.aac.sample_rate;
                #endif /* __BT_AWS_SUPPORT__ */
            }
            g_prA2dp_sink_handle = bt_codec_a2dp_open(aud_bt_codec_a2dp_callback, &a2dp_codec);
            bt_sink_srv_assert(g_prA2dp_sink_handle);
            break;
        default:
            break;
    }
    if (g_bBT_Ringbuf == NULL) {
#ifndef WIN32_UT
        //g_bBT_Ringbuf = (uint8_t *)pvPortMalloc(AM_RING_BUFFER_SIZE * sizeof(uint8_t));
        g_bBT_Ringbuf = NULL;
#ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
        g_bBT_Ringbuf = g_bt_sink_srv_am_ring_buffer;
#endif
#else
        g_bBT_Ringbuf = (uint8_t *)malloc(AM_RING_BUFFER_SIZE * sizeof(uint8_t));
#endif
    }
#ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
    g_prA2dp_sink_handle->set_get_data_function(g_prA2dp_sink_handle, bt_sink_srv_media_mgr_read_data);
    g_prA2dp_sink_handle->set_get_data_count_function(g_prA2dp_sink_handle, bt_sink_srv_media_mgr_get_data_count);
#else
    g_prA2dp_sink_handle->set_buffer(g_prA2dp_sink_handle, g_bBT_Ringbuf, AM_RING_BUFFER_SIZE);
#endif /* __BT_SINK_SRV_AM_MED_LIST_SUPPORT__ */
    g_prA2dp_sink_event_handle.get_write_buffer = bt_get_write_buffer_a2dp_sink_codec;
    g_prA2dp_sink_event_handle.write_data_done = bt_write_data_done_a2dp_sink_codec;
    g_prA2dp_sink_event_handle.finish_write_data = bt_finish_write_data_a2dp_sink_codec;
    g_prA2dp_sink_event_handle.get_free_space = bt_get_free_space_a2dp_sink_codec;
    g_prA2dp_sink_event_handle.reset_share_buffer = bt_reset_share_buffer_a2dp_sink_codec;
    g_prA2dp_sink_event_handle.play = bt_codec_am_a2dp_sink_play;
    g_prA2dp_sink_event_handle.stop = bt_codec_am_a2dp_sink_stop;
    g_prA2dp_sink_event_handle.med_hd = g_prA2dp_sink_handle;
    #ifdef __BT_AWS_SUPPORT__
    g_prA2dp_sink_event_handle.set_aws_flag = bt_a2dp_aws_set_flag;
    g_prA2dp_sink_event_handle.set_aws_initial_sync = bt_a2dp_aws_set_initial_sync;
    g_prA2dp_sink_event_handle.aws_plh_init = bt_a2dp_aws_plh_init;
    g_prA2dp_sink_event_handle.aws_plh_deinit = bt_a2dp_aws_plh_deinit;
    #endif /* __BT_AWS_SUPPORT__ */
    g_rSink_state = A2DP_SINK_CODEC_OPEN;
    g_rAm_aud_id[g_prCurrent_player->aud_id].use = ID_PLAY_STATE;
    if (g_prCurrent_player->type == A2DP) {
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SINK_OPEN_CODEC, AUD_SINK_PROC_PTR, &g_prA2dp_sink_event_handle);
    } else if (g_prCurrent_player->type == AWS) {
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_AWS_SINK_OPEN_CODEC, AUD_SINK_PROC_PTR, &g_prA2dp_sink_event_handle);
    } else {
        /* trace error */
    }

#ifdef __BT_SINK_SRV_ACF_MODE_SUPPORT__
    bt_sink_srv_set_acf_mode(0);
#endif
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_a2dp_sink_play
 * DESCRIPTION
 *  This function is used to process "play" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
bt_status_t bt_codec_am_a2dp_sink_play(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;
    bt_sink_srv_am_device_set_t dev;
    bt_sink_srv_am_volume_level_out_t vol;
    uint32_t digital_vol = 0x7FFF, analog_vol = 0x0002;
#ifdef __BT_SINK_SRV_AUDIO_TUNING__
    bt_sink_srv_audio_tunning_context_t *aud_tunning_p = NULL;
#endif

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    bt_sink_srv_report("[sink][AM]sink_play--aud_id: %d, aid: %d, g_prCurrent_player: 0x%x, g_prA2dp_sink_handle: 0x%x",
        aud_id, g_prCurrent_player->aud_id, g_prCurrent_player, g_prA2dp_sink_handle);

    if ((g_prCurrent_player != NULL) && (aud_id == g_prCurrent_player->aud_id)) {
        if (g_prA2dp_sink_handle != NULL) {
            dev = g_prCurrent_player->audio_stream_out.audio_device;
            vol = g_prCurrent_player->audio_stream_out.audio_volume;
            hal_audio_set_stream_out_device(dev);
            if (dev & DEVICE_LOUDSPEAKER) {
                digital_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                analog_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
            } else if (dev & DEVICE_EARPHONE) {
                digital_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                analog_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
            } else {
                ;
            }
            if (g_am_volume_enable) {
                bt_sink_srv_report("[sink][AM]play-d_gain: 0x%x, a_gain: 0x%x", digital_vol, analog_vol);
            } else {
                #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                {
                    bt_sink_srv_audio_setting_vol_info_t vol_info;

                    vol_info.type = VOL_A2DP;
                    vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                    bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                }
                #else
                hal_audio_set_stream_out_volume(digital_vol, analog_vol);
                #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
            }
#ifdef __BT_SINK_SRV_AUDIO_TUNING__
            aud_tunning_p = bt_sink_srv_audio_tunning_get_context();
            if ((aud_tunning_p->flag & TUNNING_FLAG_INIT) &&
                    (aud_tunning_p->flag & TUNNING_FLAG_ON)) {
                bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_A2DP, TUNNING_TYPE_VOL);
                bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_A2DP, TUNNING_TYPE_DEV);
            }
#endif /* __BT_SINK_SRV_AUDIO_TUNING__ */
            eResult = g_prA2dp_sink_handle->play(g_prA2dp_sink_handle);
            if (eResult == BT_CODEC_MEDIA_STATUS_OK) {
                g_rSink_state = A2DP_SINK_CODEC_PLAY;
                return BT_CODEC_MEDIA_STATUS_OK;
            }
        }
    }

    bt_sink_srv_report("[sink][AM]sink_play--aud_id: %d, aid: %d, g_prCurrent_player: 0x%x, g_prA2dp_sink_handle: 0x%x",
        aud_id, g_prCurrent_player->aud_id, g_prCurrent_player, g_prA2dp_sink_handle);

    return BT_CODEC_MEDIA_STATUS_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_a2dp_sink_stop
 * DESCRIPTION
 *  This function is used to process "play" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
bt_status_t bt_codec_am_a2dp_sink_stop(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (aud_id == g_prCurrent_player->aud_id) {
        if (g_prA2dp_sink_handle != NULL) {
            eResult = g_prA2dp_sink_handle->stop(g_prA2dp_sink_handle);
            if (eResult == BT_CODEC_MEDIA_STATUS_OK) {
#ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                g_prA2dp_sink_handle->reset_share_buffer(g_prA2dp_sink_handle);
#endif
                g_rSink_state = A2DP_SINK_CODEC_STOP;
                return BT_CODEC_MEDIA_STATUS_OK;
            }
        }
    }
    return BT_CODEC_MEDIA_STATUS_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_a2dp_sink_close
 * DESCRIPTION
 *  This function is used to process "close" for A2DP sink by BT APP.
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
bt_status_t bt_codec_am_a2dp_sink_close(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t	eResult = BT_CODEC_MEDIA_STATUS_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prA2dp_sink_handle != NULL) {
        eResult = bt_codec_a2dp_close(g_prA2dp_sink_handle);
        if (eResult == BT_CODEC_MEDIA_STATUS_OK) {
            g_prA2dp_sink_handle = NULL;
            g_rSink_state = A2DP_SINK_CODEC_CLOSE;
            if (g_bBT_Ringbuf != NULL) {
#ifndef WIN32_UT
                //vPortFree(g_bBT_Ringbuf);
#ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                bt_sink_srv_memset(g_bt_sink_srv_am_ring_buffer, 0, sizeof(g_bt_sink_srv_am_ring_buffer));
#endif
                g_bBT_Ringbuf = NULL;
#else
                free(g_bBT_Ringbuf);
                g_bBT_Ringbuf = NULL;
#endif
            }
            return BT_CODEC_MEDIA_STATUS_OK;
        }
    }
    return BT_CODEC_MEDIA_STATUS_ERROR;
}


#ifdef __BT_AWS_SUPPORT__
/**
 * @brief     This function sets the advanced wireless stereo flag.
 * @param[in] aud_id is audio ID.
 * @param[in] flag is used to determine to turn on or turn off AWS mechanism. 1: turn on AWS, 0: turn off AWS.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
static int32_t bt_a2dp_aws_set_flag(bt_sink_srv_am_id_t aud_id, bool flag)
{
    int32_t ret = 0;

    bt_sink_srv_report("[sink][AM]aws_set_flag--aud_id: %d, aid: %d, flag: %d",
        aud_id, g_prCurrent_player->aud_id, flag);

    if (aud_id == g_prCurrent_player->aud_id) {
        ret = bt_codec_a2dp_aws_set_flag(g_prA2dp_sink_handle, flag);
    }

    return ret;
}


/**
 * @brief     This function sets the advanced wireless stereo initial synchronization.
 * @param[in] aud_id is audio ID.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
static int32_t bt_a2dp_aws_set_initial_sync(bt_sink_srv_am_id_t aud_id)
{
    int32_t ret = AM_ERR_FAIL_1ST;

    if (aud_id == g_prCurrent_player->aud_id) {
        //hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_HIGH);
        ret = bt_codec_a2dp_aws_set_initial_sync(g_prA2dp_sink_handle);
        //hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_LOW);
    }

    return ret;
}


static void bt_a2dp_aws_plh_init(bt_sink_srv_am_id_t aud_id)
{
    if (aud_id == g_prCurrent_player->aud_id) {
        bt_a2dp_plh_init(g_prA2dp_sink_handle);
    }
}


static void bt_a2dp_aws_plh_deinit(void)
{
    bt_a2dp_plh_deinit();
}


// for AWS check Rance.
/**
 * @brief     This function sets the advanced wireless stereo flag.
 * @param[in] aud_id is audio ID.
 * @param[in] flag is used to determine to turn on or turn off AWS mechanism. 1: turn on AWS, 0: turn off AWS.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
static int32_t bt_mp3_aws_set_flag(bt_sink_srv_am_id_t aud_id, bool flag)
{
    int32_t ret = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        ret = mp3_codec_aws_set_flag(g_am_mp3_media_handle, flag);
    }

    return ret;
}


/**
 * @brief     This function sets the advanced wireless stereo initial synchronization.
 * @param[in] aud_id is audio ID.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
static int32_t bt_mp3_aws_set_initial_sync(bt_sink_srv_am_id_t aud_id)
{
    int32_t ret = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        ret = mp3_codec_aws_set_initial_sync(g_am_mp3_media_handle);
    }

    return ret;
}

static int32_t bt_mp3_aws_init()
{
    int32_t ret = 0;

    ret = g_am_mp3_media_handle->aws_init(g_am_mp3_media_handle);
    return ret;
}


static int32_t bt_mp3_aws_deinit()
{
    int32_t ret = 0;

    ret = g_am_mp3_media_handle->aws_deinit(g_am_mp3_media_handle);
    return ret;
}

static int32_t bt_mp3_aws_set_clock_skew_compensation_value(int32_t sample_count)
{
    int32_t ret = 0;

    ret =g_am_mp3_media_handle->aws_set_clock_skew_compensation_value(g_am_mp3_media_handle, sample_count);
    return ret;
}

static int32_t bt_mp3_aws_get_clock_skew_status(int32_t *status)
{
    int32_t ret = 0;

    ret = g_am_mp3_media_handle->aws_get_clock_skew_status(g_am_mp3_media_handle, status);
    return ret;
}

static int32_t bt_mp3_aws_set_clock_skew(bool flag)
{
    int32_t ret = 0;

    ret = g_am_mp3_media_handle->aws_set_clock_skew(g_am_mp3_media_handle, flag);
    return ret;
}


/*****************************************************************************
 * FUNCTION
 *  aud_process_aws_a2dp_callback_hdlr
 * DESCRIPTION
 *  This function is used to handle A2DP process callback.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_process_aws_a2dp_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_bt_event_t event_id = amm_ptr->background_info.local_context.aws_format.aws_event;
    #ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;
    #endif
    bt_sink_srv_am_device_set_t dev;
    bt_sink_srv_am_volume_level_out_t vol;
    uint32_t digital_vol = 0x7FFF, analog_vol = 0x0002;
    uint32_t sampling_rate = 44100;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prA2dp_sink_handle != NULL) {
        g_prA2dp_sink_handle->process(g_prA2dp_sink_handle, event_id);

        switch (event_id) {
            case BT_CODEC_MEDIA_ERROR: {
                #ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                //g_prA2dp_sink_handle->stop(g_prA2dp_sink_handle);
                //g_rSink_state = A2DP_SINK_CODEC_STOP;
                dev = g_prCurrent_player->audio_stream_out.audio_device;
                vol = g_prCurrent_player->audio_stream_out.audio_volume;
                hal_audio_set_stream_out_device(dev);
                if (dev & DEVICE_LOUDSPEAKER) {
                    digital_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else if (dev & DEVICE_EARPHONE) {
                    digital_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else {
                    bt_sink_srv_report("[sink][AM]aws-d_gain: 0x%x, a_gain: 0x%x", digital_vol, analog_vol);
                }
                #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                {
                    bt_sink_srv_audio_setting_vol_info_t vol_info;

                    vol_info.type = VOL_A2DP;
                    vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                    bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                }
                #else
                hal_audio_set_stream_out_volume(digital_vol, analog_vol);
                #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_AWS_A2DP_PROC_IND, AUD_A2DP_CODEC_RESTART, NULL);
                #else
                g_prA2dp_sink_handle->stop(g_prA2dp_sink_handle);
                g_rSink_state = A2DP_SINK_CODEC_STOP;
                g_prA2dp_sink_handle->reset_share_buffer(g_prA2dp_sink_handle);
                dev = g_prCurrent_player->audio_stream_out.audio_device;
                vol = g_prCurrent_player->audio_stream_out.audio_volume;
                hal_audio_set_stream_out_device(dev);
                if (dev & DEVICE_LOUDSPEAKER) {
                    digital_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else if (dev & DEVICE_EARPHONE) {
                    digital_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else {
                    ;
                }
                #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                {
                    bt_sink_srv_audio_setting_vol_info_t vol_info;

                    vol_info.type = VOL_A2DP;
                    vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                    bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                }
                #else
                hal_audio_set_stream_out_volume(digital_vol, analog_vol);
                #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */

                //eResult = g_prA2dp_sink_handle->play(g_prA2dp_sink_handle);
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_AWS_A2DP_PROC_IND, AUD_A2DP_CODEC_RESTART, NULL);
                if (eResult == BT_CODEC_MEDIA_STATUS_ERROR) {
                    bt_sink_srv_report("[sink][AM] Error");
                }
                bt_sink_srv_report("[sink][AM]-restart play");
                #endif /* __BT_SINK_SRV_AM_MED_LIST_SUPPORT__ */
                break;
            }

            case BT_CODEC_MEDIA_AWS_CHECK_UNDERFLOW: {
                bt_sink_srv_fetch_bt_offset();
                break;
            }

            case BT_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW: {
                if (BT_AWS_CODEC_TYPE_SBC == g_aws_codec_type) {
                    switch (g_aws_sample_rate) {
                        case 8:
                            sampling_rate = 16000;
                            break;
                        case 4:
                            sampling_rate = 32000;
                            break;
                        case 2:
                            sampling_rate = 44100;
                            break;
                        case 1:
                            sampling_rate = 48000;
                            break;

                        default:
                            sampling_rate = 44100;
                            break;
                    }
                } else if (BT_AWS_CODEC_TYPE_AAC == g_aws_codec_type){
                    switch (g_aws_sample_rate) {
                        case 0x800:
                            sampling_rate = 8000;
                            break;
                        case 0x400:
                            sampling_rate = 11025;
                            break;
                        case 0x200:
                            sampling_rate = 12000;
                            break;
                        case 0x100:
                            sampling_rate = 16000;
                            break;
                        case 0x80:
                            sampling_rate = 22050;
                            break;
                        case 0x40:
                            sampling_rate = 24000;
                            break;
                        case 0x20:
                            sampling_rate = 32000;
                            break;
                        case 0x10:
                            sampling_rate = 44100;
                            break;
                        case 0x8:
                            sampling_rate = 48000;
                            break;
                        default:
                            sampling_rate = 44100;
                            break;
                    }
                }
                bt_sink_srv_audio_sync_calc_t audio_calc;
                audio_calc.media_handle= g_prA2dp_sink_handle;
                audio_calc.sampling_rate = sampling_rate;
                audio_calc.type = g_aws_codec_type;
                bt_sink_srv_audio_clock_calibrate(&audio_calc);
                break;
            }

            case BT_CODEC_MEDIA_UNDERFLOW: {
                #ifdef __BT_AWS_SUPPORT__
                g_aws_underflow_loop_count++;
                if (g_aws_underflow_loop_count >= BT_SINK_SRV_AM_MAX_UNDERFLOW_COUNT) {
                    g_aws_underflow_loop_count = 0;
                    g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_AWS_A2DP_PROC_IND, AUD_A2DP_AWS_UNDERFLOW, NULL);
                }
                #endif
                break;
            }

            default:
                break;
        }
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_AWS_A2DP_PROC_IND, (bt_sink_srv_am_cb_sub_msg_t)event_id, NULL);
    }
}


void bt_sink_srv_skew_test(const char *value)
{
    int32_t sample_count = 0;

    sample_count = atoi(value);

    bt_sink_srv_report("[Sink][AM]skew_test--value: %s, count: %d", value, sample_count);

    if (g_prA2dp_sink_handle) {
        bt_codec_a2dp_aws_set_clock_skew_compensation_value(g_prA2dp_sink_handle, sample_count);
    }
}


#endif /* __BT_AWS_SUPPORT__ */


#ifdef __BT_SINK_AUDIO_TUNING__
#define BT_SINK_AM_SIDE_TONE_DB_MAX 31
void bt_codec_am_speech_nvdm_changed_callback(void *data)
{
    audio_eaps_t *am_speech_eaps;

#if PRODUCT_VERSION == 2533
	aud_process_dsp_nvdm_setting();

    am_speech_eaps = (audio_eaps_t *)pvPortMalloc(sizeof(audio_eaps_t));

    bt_sink_srv_report("[Sink][AM] nvdm changed, g_prCurrent_player:0x%x", g_prCurrent_player);

    if (am_speech_eaps != NULL) {
        audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);

        #ifndef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__

        speech_update_common(am_speech_eaps->speech_common_parameter.speech_common_parameter);
        speech_update_nb_fir(NULL,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient);
        speech_update_nb_param(am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_mode_parameter);
        speech_update_wb_fir(NULL,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient);
        speech_update_wb_param(am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_mode_parameter);
        #endif
        if (NULL != g_prCurrent_player && HFP == g_prCurrent_player->type) {
            int32_t stream_out_db = 0, stream_in_db = 0;
            int32_t analog_gain_in_db = 0, digital_gain_in_db = 0;
            int16_t temp_int16 = 0;
            uint32_t side_tone_db = 0;
            bool result = false;
            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
                vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
                vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
                vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                speech_set_enhancement(false);
                bt_sink_srv_audio_setting_update_voice_fillter_setting(&vol_info, am_speech_eaps);
                speech_set_enhancement(true);
            }
            #endif

            bt_sink_srv_report("[Sink][AM] type:0x%x, out:%d, in:%d",
                               g_prCurrent_player->local_context.hfp_format.hfp_codec.type,
                               g_prCurrent_player->audio_stream_out.audio_volume,
                               g_prCurrent_player->audio_stream_in.audio_volume);

            if (BT_HFP_CODEC_TYPE_CVSD == g_prCurrent_player->local_context.hfp_format.hfp_codec.type) {
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_gain.stream_out_gain[g_prCurrent_player->audio_stream_out.audio_volume];
                stream_out_db = (int32_t)temp_int16;
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_gain.stream_in_gain[g_prCurrent_player->audio_stream_in.audio_volume];
                stream_in_db = (int32_t)temp_int16;
                side_tone_db = (uint32_t)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_gain.sidetone_gain;
                result = true;
            } else if (BT_HFP_CODEC_TYPE_MSBC == g_prCurrent_player->local_context.hfp_format.hfp_codec.type) {
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_gain.stream_out_gain[g_prCurrent_player->audio_stream_out.audio_volume];
                stream_out_db = (int32_t)temp_int16;
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_gain.stream_in_gain[g_prCurrent_player->audio_stream_in.audio_volume];
                stream_in_db = (int32_t)temp_int16;
                side_tone_db = (uint32_t)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_gain.sidetone_gain;
                result = true;
            }

            bt_sink_srv_report("[Sink][AM] out_db:%d, in_db:%d, side_tone:%d", stream_out_db, stream_in_db, side_tone_db);

            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;
            
                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
                vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
                vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
                vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                bt_sink_srv_music_report("[Sink][AM]2533 update gain");
                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
            }
            #endif

            if (result) {
                audio_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(stream_out_db, &analog_gain_in_db, &digital_gain_in_db);
                digital_gain_in_db *= 100;
                analog_gain_in_db *= 100;
                bt_sink_srv_report("[Sink][AM] stream out, digital:0x%x, analog:0x%x", digital_gain_in_db, analog_gain_in_db);
#ifndef __BT_SINK_SRV_AUDIO_TUNING__
                hal_audio_set_stream_out_volume((uint32_t)digital_gain_in_db, (uint32_t)analog_gain_in_db);
#endif
                voice_uplink_dmic_db_transfer_to_analog_gain_and_digital_gain(stream_in_db, &analog_gain_in_db, &digital_gain_in_db);
                digital_gain_in_db *= 100;
                analog_gain_in_db *= 100;
                bt_sink_srv_report("[Sink][AM] stream in, digital:0x%x, analog:0x%x", digital_gain_in_db, analog_gain_in_db);
#ifndef __BT_SINK_SRV_AUDIO_TUNING__
                hal_audio_set_stream_in_volume((uint32_t)digital_gain_in_db, (uint32_t)analog_gain_in_db);
#endif

                if (BT_SINK_AM_SIDE_TONE_DB_MAX >= side_tone_db) {
                    bt_sink_srv_report("[Sink][AM] side_tone_db:0x%x", side_tone_db);
                    speech_set_sidetone_volume(side_tone_db);
                }
            }
        }

        vPortFree(am_speech_eaps);
    }

#else

    am_speech_eaps = (audio_eaps_t *)pvPortMalloc(sizeof(audio_eaps_t));

    bt_sink_srv_report("[Sink][AM] nvdm changed, g_prCurrent_player:0x%x", g_prCurrent_player);

    if (am_speech_eaps != NULL) {
        audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);

        #ifndef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        speech_set_enhancement(false);
        speech_update_common(am_speech_eaps->speech_common_parameter.speech_common_parameter);
        speech_update_nb_fir((int16_t *)am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient);
        speech_update_nb_param(am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_mode_parameter);
        speech_update_wb_fir((int16_t *)am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient);
        speech_update_wb_param(am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_mode_parameter);
        speech_set_enhancement(true);
        #endif

        if (NULL != g_prCurrent_player && HFP == g_prCurrent_player->type) {
            int32_t stream_out_db = 0, stream_in_db = 0;
            int32_t analog_gain_in_db = 0, digital_gain_in_db = 0;
            int16_t temp_int16 = 0;
            uint32_t side_tone_db = 0;
            bool result = false;

            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
                vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
                vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
                vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                speech_set_enhancement(false);
                bt_sink_srv_audio_setting_update_voice_fillter_setting(&vol_info, am_speech_eaps);
                speech_set_enhancement(true);
            }
            #endif

            bt_sink_srv_report("[Sink][AM] type:0x%x, out:%d, in:%d",
                               g_prCurrent_player->local_context.hfp_format.hfp_codec.type,
                               g_prCurrent_player->audio_stream_out.audio_volume,
                               g_prCurrent_player->audio_stream_in.audio_volume);

            if (BT_HFP_CODEC_TYPE_CVSD == g_prCurrent_player->local_context.hfp_format.hfp_codec.type) {
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_gain.stream_out_gain[g_prCurrent_player->audio_stream_out.audio_volume];
                stream_out_db = (int32_t)temp_int16;
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_gain.stream_in_gain[g_prCurrent_player->audio_stream_in.audio_volume];
                stream_in_db = (int32_t)temp_int16;
                side_tone_db = (uint32_t)am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_gain.sidetone_gain;
                result = true;
            } else if (BT_HFP_CODEC_TYPE_MSBC == g_prCurrent_player->local_context.hfp_format.hfp_codec.type) {
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_gain.stream_out_gain[g_prCurrent_player->audio_stream_out.audio_volume];
                stream_out_db = (int32_t)temp_int16;
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_gain.stream_in_gain[g_prCurrent_player->audio_stream_in.audio_volume];
                stream_in_db = (int32_t)temp_int16;
                side_tone_db = (uint32_t)am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_gain.sidetone_gain;
                result = true;
            }

            bt_sink_srv_report("[Sink][AM] out_db:%d, in_db:%d, side_tone:%d", stream_out_db, stream_in_db, side_tone_db);

            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;
            
                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
                vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
                vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
                vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                bt_sink_srv_music_report("[Sink][AM]2523 update gain");
                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
            }
            #endif

            if (result) {
                audio_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(stream_out_db, &analog_gain_in_db, &digital_gain_in_db);
                digital_gain_in_db *= 100;
                analog_gain_in_db *= 100;
                bt_sink_srv_report("[Sink][AM] stream out, digital:0x%x, analog:0x%x", digital_gain_in_db, analog_gain_in_db);
#ifndef __BT_SINK_SRV_AUDIO_TUNING__
                hal_audio_set_stream_out_volume((uint32_t)digital_gain_in_db, (uint32_t)analog_gain_in_db);
#endif
                voice_uplink_amp_db_transfer_to_analog_gain_and_digital_gain(stream_in_db, &analog_gain_in_db, &digital_gain_in_db);
                digital_gain_in_db *= 100;
                analog_gain_in_db *= 100;
                bt_sink_srv_report("[Sink][AM] stream in, digital:0x%x, analog:0x%x", digital_gain_in_db, analog_gain_in_db);
#ifndef __BT_SINK_SRV_AUDIO_TUNING__
                hal_audio_set_stream_in_volume((uint32_t)digital_gain_in_db, (uint32_t)analog_gain_in_db);
#endif

                if (BT_SINK_AM_SIDE_TONE_DB_MAX >= side_tone_db) {
                    bt_sink_srv_report("[Sink][AM] side_tone_db:0x%x", side_tone_db);
                    speech_set_sidetone_volume(side_tone_db);
                }
            }
        }

        vPortFree(am_speech_eaps);
    }
#endif
}
#endif /* __BT_SINK_AUDIO_TUNING__ */


/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_hfp_open
 * DESCRIPTION
 *  This function is used to open HFP codec.
 * PARAMETERS
 *  hfp_codec        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_codec_am_hfp_open(bt_sink_srv_am_hfp_codec_t *hfp_codec_t)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;
    const audio_eaps_t *am_speech_eaps = audio_nvdm_get_global_eaps_address();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    g_prHfp_media_handle = bt_codec_hfp_open(aud_bt_codec_hfp_callback, hfp_codec_t);

#if PRODUCT_VERSION != 2533
    //am_speech_eaps = (audio_eaps_t *)pvPortMalloc(sizeof(audio_eaps_t));
    if (am_speech_eaps != NULL) {
        //audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);

        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_HFP;
            vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
            vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
            vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
            vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
            vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
            bt_sink_srv_audio_setting_update_voice_fillter_setting(&vol_info, am_speech_eaps);
        }
        #else
        speech_update_common(am_speech_eaps->speech_common_parameter.speech_common_parameter);
        speech_update_nb_fir((int16_t *)am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient);
        speech_update_nb_param(am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_mode_parameter);
        speech_update_wb_fir((int16_t *)am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient);
        speech_update_wb_param(am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_mode_parameter);
        #endif
        //vPortFree(am_speech_eaps);
    }

#else

	//am_speech_eaps = (audio_eaps_t *)pvPortMalloc(sizeof(audio_eaps_t));
	if (am_speech_eaps != NULL) {
		//audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_HFP;
            vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
            vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
            vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
            vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
            vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
            bt_sink_srv_audio_setting_update_voice_fillter_setting(&vol_info, am_speech_eaps);
        }
        #else
		speech_update_common(am_speech_eaps->speech_common_parameter.speech_common_parameter);
		speech_update_nb_fir(NULL,
							 (int16_t *)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient);
		speech_update_nb_param(am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_mode_parameter);
		speech_update_wb_fir(NULL,
							 (int16_t *)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient);
		speech_update_wb_param(am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_mode_parameter);
		#endif
		//vPortFree(am_speech_eaps);
	}

#endif

#ifdef __BT_SINK_AUDIO_TUNING__
    audio_nvdm_register_eaps_is_changed_callback((audio_nvdm_callback_id *)g_prHfp_media_handle, bt_codec_am_speech_nvdm_changed_callback, NULL);
    bt_sink_srv_music_report("[sink][AM]eaps_reg_hf--cid: 0x%08x", g_prHfp_media_handle);
#endif /* __BT_SINK_AUDIO_TUNING__ */

    eResult = g_prHfp_media_handle->play(g_prHfp_media_handle);
    if (eResult == BT_CODEC_MEDIA_STATUS_OK) {
        g_rAm_aud_id[g_prCurrent_player->aud_id].use = ID_PLAY_STATE;
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
    } else {
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
    }


#if PRODUCT_VERSION == 2533
    aud_process_dsp_nvdm_setting(); // must put after play
#endif
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_hfp_stop
 * DESCRIPTION
 *	This function is used to stop HFP codec.
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
bt_status_t bt_codec_am_hfp_stop(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __BT_SINK_AUDIO_TUNING__
    audio_nvdm_unregister_eaps_is_changed_callback((audio_nvdm_callback_id *)g_prHfp_media_handle);
#endif /* __BT_SINK_AUDIO_TUNING__ */
    eResult = g_prHfp_media_handle->stop(g_prHfp_media_handle);
    if (eResult == BT_CODEC_MEDIA_STATUS_OK) {
        if (bt_codec_hfp_close(g_prHfp_media_handle) == BT_CODEC_MEDIA_STATUS_OK) {
            return BT_CODEC_MEDIA_STATUS_OK;
        }
    }
    return BT_CODEC_MEDIA_STATUS_ERROR;
}

#ifdef RTOS_TIMER
/*****************************************************************************
 * FUNCTION
 *  aud_timer_callback
 * DESCRIPTION
 *  This callback function is used to notify A.M. when the timer expires on timer service task.
 * PARAMETERS
 *  pxTimer          [OUT]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_timer_callback(TimerHandle_t pxTimer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t lTimer_ID;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __AM_DEBUG_INFO__
    //bt_sink_srv_report("[Sink][AM] Timer tick");
#endif
    configASSERT(pxTimer);
    lTimer_ID = (uint32_t)pvTimerGetTimerID(pxTimer);
    if (lTimer_ID == AM_TIMER_ID) {
        g_lExpire_count++;
        if (g_lExpire_count == AM_EXPIRE_TIMER_MAX) {
            xTimerStop(pxTimer, 0);
            g_lExpire_count = 0;
            bt_sink_srv_ami_send_amm(MOD_AM, MOD_TMR, AUD_TIMER_IND,
                                     MSG_ID_TIMER_OUT_CALL_EXT_REQ,
                                     g_prCurrent_resumer,
                                     FALSE, NULL);
        }
    }
}
#endif

/*****************************************************************************
 * FUNCTION
 *  aud_timer_out_callback_hdlr
 * DESCRIPTION
 *	This function is used to handle timer callback on A.M. task.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_timer_out_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    g_rAm_aud_id[g_prCurrent_resumer->aud_id].use = ID_IDLE_STATE;
    g_prCurrent_resumer->notify_cb(g_prCurrent_resumer->aud_id, AUD_RESUME_IND, AUD_RESUME_IDLE_STATE, NULL);
    g_prCurrent_resumer = NULL;
    am_audio_search_suspended();
}

/*****************************************************************************
 * FUNCTION
 *  am_task_main
 * DESCRIPTION
 *  This function is a main message handler on A.M. task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
#ifdef WIN32_UT
void am_task_main(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t bAud_timer_id = AM_TIMER_ID;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    hal_audio_register_stream_out_callback(aud_stream_out_callback, NULL);
    hal_audio_register_stream_in_callback(aud_stream_in_callback, NULL);
    aud_initial();
    while (1) {
        am_receive_msg(g_prAmm_current);
        free(g_prAmm_current);
        if (g_prCurrent_player != NULL) {
            printf("\tHave player: ID=%d\n", g_prCurrent_player->aud_id);
        } else if (g_prCurrent_resumer != NULL) {
            printf("\tHave resumer: ID=%d\n", g_prCurrent_resumer->aud_id);
        }
        break;
    }
}
#else
QueueHandle_t g_xQueue_am;
void am_task_main(void *arg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_amm_struct *amm_temp_t = NULL;
    uint32_t bAud_timer_id = AM_TIMER_ID;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __AM_DEBUG_INFO__
    bt_sink_srv_report("[Sink][AM] Start AM_Task");
#endif
    g_xQueue_am = xQueueCreate(5, sizeof(bt_sink_srv_am_amm_struct *));

    /* init med mgr */
    bt_sink_srv_media_mgr_init();

    if (g_xQueue_am != 0) {
        g_xSemaphore_ami = xSemaphoreCreateMutex();
        g_xTimer_am = xTimerCreate("Timer",
                                   (AM_TIMER_PERIOD / portTICK_PERIOD_MS),
                                   pdTRUE,
                                   (void *)bAud_timer_id,
                                   aud_timer_callback);
        hal_audio_register_stream_out_callback(aud_stream_out_callback, NULL);
        hal_audio_register_stream_in_callback(aud_stream_in_callback, NULL);
        aud_initial();
        while (1) {
            if (xQueueReceive(g_xQueue_am, &amm_temp_t, portMAX_DELAY)) {
                am_receive_msg((bt_sink_srv_am_amm_struct *)amm_temp_t);
                if (amm_temp_t != ptr_callback_amm) {
                    vPortFree(amm_temp_t);
                }
            }
        }
    }
#ifdef __AM_DEBUG_INFO__
    bt_sink_srv_report("[Sink][AM][ERROR] Start AM_Task Error");
#endif
}
#endif /* WIN32_UT */

/*****************************************************************************
 * FUNCTION
 *  am_receive_msg
 * DESCRIPTION
 *  This function is a main message dispatching function of A.M.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void am_receive_msg(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /*
    #ifdef __AM_DEBUG_INFO__
     bt_sink_srv_report("[Sink][AM] Received Message");
     bt_sink_srv_report("[Sink][AM] Aud ID: %d, Type: %d, Priority: %d, Vol_D: %d, Vol_A: %d, SR: %d",
                     amm_ptr->background_info.aud_id,
                     amm_ptr->background_info.type,
                     amm_ptr->background_info.priority,
                     amm_ptr->background_info.audio_stream_out.audio_volume.digital_gain_index,
                     amm_ptr->background_info.audio_stream_out.audio_volume.analog_gain_index,
                     amm_ptr->background_info.local_context.pcm_format.stream.stream_sample_rate
                   );
    #endif
    */
    switch (amm_ptr->msg_id) {
        case MSG_ID_STREAM_OPEN_REQ:
            aud_set_open_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_PLAY_REQ:
            aud_set_play_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_STOP_REQ:
            aud_set_stop_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_CLOSE_REQ:
            aud_set_close_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_SET_VOLUME_REQ:
            aud_set_volume_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_MUTE_DEVICE_REQ:
            aud_mute_device_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_CONFIG_DEVICE_REQ:
            aud_config_device_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_READ_WRITE_DATA_REQ:
            aud_rw_stream_data_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_GET_LENGTH_REQ:
            aud_query_stream_len_req_hdlr(amm_ptr);
            break;

        case MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ:
            aud_process_a2dp_callback_hdlr(amm_ptr);
            break;
        case MSG_ID_MEDIA_HFP_EVENT_CALL_EXT_REQ:
            aud_process_hfp_callback_hdlr(amm_ptr);
            break;
        case MSG_ID_MEDIA_EVENT_STREAM_OUT_CALL_EXT_REQ:
            aud_event_stream_callback_hdlr(amm_ptr);
            break;
        case MSG_ID_MEDIA_EVENT_STREAM_IN_CALL_EXT_REQ:
            aud_event_stream_callback_hdlr(amm_ptr);
            break;
        case MSG_ID_TIMER_OUT_CALL_EXT_REQ:
            aud_timer_out_callback_hdlr(amm_ptr);
            break;
        case MSG_ID_MEDIA_AWS_A2DP_PROC_CALL_EXT_REQ:
            #ifdef __BT_AWS_SUPPORT__
            aud_process_aws_a2dp_callback_hdlr(amm_ptr);
            #endif
            break;
        case MSG_ID_MEDIA_FILE_PROCE_CALL_EXT_REQ:
            aud_process_mp3_callback_hdlr(amm_ptr);
            break;
        default:
            break;
    }

#if 0
    bt_sink_srv_report("[Sink][AM] Background Message");
    if (g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr != NULL) {
        bt_sink_srv_report("[Sink][AM] Aud ID: %d, State: %d, Type: %d, Priority: %d, Vol: %d, pcm_SR: %d, a2dp_SR: %d",
                           amm_ptr->background_info.aud_id,
                           g_rAm_aud_id[amm_ptr->background_info.aud_id].use,
                           g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->type,
                           g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->priority,
                           g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->audio_stream_out.audio_volume,
                           g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->local_context.pcm_format.stream.stream_sample_rate,
                           g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->local_context.a2dp_format.a2dp_codec.codec_cap.codec.aac.sample_rate
                          );
    }
#endif

}

#if STANDALONE_TEST
/* Hal API func */
#ifndef WIN32_UT
hal_audio_status_t hal_audio_init(void)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_deinit(void)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_stream_out_sampling_rate(hal_audio_sampling_rate_t sample_rate)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_stream_out_channel_number(hal_audio_channel_number_t channel_number)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_start_stream_out(hal_audio_active_type_t active_type)
{
    return HAL_AUDIO_STATUS_OK;
}

void hal_audio_stop_stream_out(void)
{}

hal_audio_status_t hal_audio_set_stream_out_volume(uint32_t digital_volume_index, uint32_t analog_volume_index)
{
#ifdef __AM_DEBUG_INFO__
    bt_sink_srv_report("[Sink][AM] Digital: %04x, Analog: %04x", digital_volume_index, analog_volume_index);
#endif
    return HAL_AUDIO_STATUS_OK;
}

void hal_audio_mute_stream_out(bool mute)
{}

hal_audio_status_t hal_audio_set_stream_out_device(hal_audio_device_t device)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_write_stream_out(const void *buffer, uint32_t size)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_get_stream_out_sample_count(uint32_t *sample_count)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_register_stream_out_callback(hal_audio_stream_out_callback_t callback, void *user_data)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_stream_in_sampling_rate(hal_audio_sampling_rate_t sample_rate)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_stream_in_channel_number(hal_audio_channel_number_t channel_number)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_start_stream_in(hal_audio_active_type_t active_type)
{
    return HAL_AUDIO_STATUS_OK;
}

void hal_audio_stop_stream_in(void)
{}

hal_audio_status_t hal_audio_set_stream_in_volume(uint32_t digital_volume_index, uint32_t analog_volume_index)
{
#ifdef __AM_DEBUG_INFO__
    bt_sink_srv_report("[Sink][AM] Digital: %04x, Analog: %04x", digital_volume_index, analog_volume_index);
#endif
    return HAL_AUDIO_STATUS_OK;
}

void hal_audio_mute_stream_in(bool mute)
{}

hal_audio_status_t hal_audio_set_stream_in_device(hal_audio_device_t device)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_read_stream_in(void *buffer, uint32_t sample_count)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_get_stream_in_sample_count(uint32_t *sample_count)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_register_stream_in_callback(hal_audio_stream_in_callback_t callback, void *user_data)
{
    return HAL_AUDIO_STATUS_OK;
}


/* BT API func */
bt_media_handle_t *temp_t = NULL;
bt_media_handle_t *hf_temp_t = NULL;

void bt_codec_a2dp_set_buffer(bt_media_handle_t *handle, uint8_t  *buffer, uint32_t  length)
{

}

void bt_codec_a2dp_get_write_buffer(bt_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    *length = 500;
}

void bt_codec_a2dp_reset_buffer(bt_media_handle_t *handle)
{

}

bt_status_t bt_codec_a2dp_play(bt_media_handle_t *handle)
{
    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_status_t bt_codec_a2dp_stop(bt_media_handle_t *handle)
{
    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_media_handle_t *bt_codec_a2dp_open(bt_codec_a2dp_callback_t callback, const bt_sink_srv_am_a2dp_codec_t *param)
{
    temp_t = (bt_media_handle_t *)pvPortMalloc(sizeof(bt_media_handle_t));
    temp_t->set_buffer         = bt_codec_a2dp_set_buffer;
    temp_t->get_write_buffer   = bt_codec_a2dp_get_write_buffer;
    temp_t->reset_share_buffer = bt_codec_a2dp_reset_buffer;
    temp_t->play               = bt_codec_a2dp_play;
    temp_t->stop               = bt_codec_a2dp_stop;
    return (bt_media_handle_t *)temp_t;
}

bt_status_t bt_codec_a2dp_close(bt_media_handle_t *handle)
{
    vPortFree(temp_t);
    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_status_t bt_codec_hfp_play(bt_media_handle_t *handle)
{
    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_status_t bt_codec_hfp_stop(bt_media_handle_t *handle)
{
    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_media_handle_t *bt_codec_hfp_open(bt_codec_hfp_callback_t callback, const bt_sink_srv_am_hfp_codec_t *param)
{
    hf_temp_t = (bt_media_handle_t *)pvPortMalloc(sizeof(bt_media_handle_t));
    hf_temp_t->play = bt_codec_hfp_play;
    hf_temp_t->stop = bt_codec_hfp_stop;
    return (bt_media_handle_t *)hf_temp_t;
}

bt_status_t bt_codec_hfp_close(bt_media_handle_t *handle)
{
    vPortFree(hf_temp_t);
    return BT_STATUS_SUCCESS;
}
#endif /*#ifndef WIN32_UT*/
#endif


static void am_files_codec_open(bt_sink_srv_am_files_format_t *files_format)
{
    am_file_type_t type = FILE_NONE;
    //int32_t len = 0;
    //bt_sink_srv_file_path_t *path = NULL;
    int32_t ret = 0, err = 0;

    #if 0
    #if FILE_PATH_TYPE_LEN == 1
    path = files_format->path;
    len = strlen(path);
    if (strncmp(&path[len - 4], ".mp3", 4) == 0) {
        type = FILE_MP3;
    } else if (strncmp(&path[len - 4], ".wav", 4) == 0) {
        type = FILE_WAV;
    } else {
        type = FILE_NONE;
    }
    #else
    path = (wchar_t *)files_format->path;
    len = wcslen(path);
    if (wcsncmp(&path[len - 4], L".mp3", 4) == 0) {
        type = FILE_MP3;
    } else if (wcsncmp(&path[len - 4], L".wav", 4) == 0) {
        type = FILE_WAV;
    } else {
        type = FILE_NONE;
    }
    #endif
    #endif
    type = files_format->file_type;
    switch (type) {
        case FILE_MP3: {
        #ifdef __AUDIO_MP3_ENABLE__
            #ifdef __BT_AWS_SUPPORT__
            g_aws_codec_type = BT_AWS_CODEC_TYPE_MP3;
            #endif
            // codec already opened
            if (g_am_mp3_media_handle) {
                g_prCurrent_player->local_context.files_format.file_event.type = FILE_MP3;
                ret = AM_ERR_SUCCESS_1ST;
                break;
            }
            g_am_mp3_media_handle = mp3_codec_open(am_files_mp3_callback);
            if (g_am_mp3_media_handle) {
                /* fill mp3 media handle */
                memset(&g_am_files_media_handle, 0x00, sizeof(bt_sink_srv_am_files_media_handle_t));
                g_am_files_media_handle.type = FILE_MP3;
                g_am_files_media_handle.media_handle.mp3.get_write_buffer = am_mp3_get_write_buffer;
                g_am_files_media_handle.media_handle.mp3.get_read_buffer = am_mp3_get_read_buffer;
                g_am_files_media_handle.media_handle.mp3.get_data_count = am_mp3_get_data_count;
                g_am_files_media_handle.media_handle.mp3.get_free_space = am_mp3_get_free_space;
                g_am_files_media_handle.media_handle.mp3.write_data_done = am_mp3_write_data_done;
                g_am_files_media_handle.media_handle.mp3.finish_write_data = am_mp3_finish_write_data;
                g_am_files_media_handle.media_handle.mp3.play = am_mp3_play;
                g_am_files_media_handle.media_handle.mp3.pause = am_mp3_pause;
                g_am_files_media_handle.media_handle.mp3.resume = am_mp3_resume;
                g_am_files_media_handle.media_handle.mp3.stop = am_mp3_stop;
                g_am_files_media_handle.media_handle.mp3.close_codec = am_mp3_close_codec;
                g_am_files_media_handle.media_handle.mp3.skip_id3v2_and_reach_next_frame = am_mp3_skip_id3v2_and_reach_next_frame;
                g_am_files_media_handle.media_handle.mp3.set_silence_frame_information = am_mp3_set_silence_frame_information;
                g_am_files_media_handle.media_handle.mp3.fill_silence_frame = am_mp3_fill_silence_frame;
                g_am_files_media_handle.media_handle.mp3.get_data_status = am_mp3_get_data_status;
                g_am_files_media_handle.media_handle.mp3.flush = am_mp3_flush;

                // aws + mp3
        #ifdef __BT_AWS_SUPPORT__
                g_am_files_media_handle.media_handle.mp3.set_aws_flag = bt_mp3_aws_set_flag;
                g_am_files_media_handle.media_handle.mp3.set_aws_initial_sync =
                    bt_mp3_aws_set_initial_sync;
                g_am_files_media_handle.media_handle.mp3.aws_init = bt_mp3_aws_init;
                g_am_files_media_handle.media_handle.mp3.aws_deinit = bt_mp3_aws_deinit;
                g_am_files_media_handle.media_handle.mp3.aws_set_clock_skew_compensation_value =
                    bt_mp3_aws_set_clock_skew_compensation_value;
                g_am_files_media_handle.media_handle.mp3.aws_get_clock_skew_status =
                    bt_mp3_aws_get_clock_skew_status;
                g_am_files_media_handle.media_handle.mp3.aws_set_clock_skew =
                    bt_mp3_aws_set_clock_skew;
        #endif
                g_prCurrent_player->local_context.files_format.file_event.type = FILE_MP3;
#if defined(MTK_AUDIO_MIXER_SUPPORT)
                mp3_codec_set_memory2(g_am_mp3_media_handle);
#else
                mp3_codec_set_memory2();
#endif

                ret = AM_ERR_SUCCESS_1ST;
            } else {
                ret = AM_ERR_FAIL_1ST;
            }
        #endif /* __AUDIO_MP3_ENABLE__ */
            break;
        }

        case FILE_WAV: {
        #ifdef __AUDIO_WAV_ENABLE__
            // codec already opened
            if (g_am_audio_media_handle) {
                g_prCurrent_player->local_context.files_format.file_event.type = FILE_WAV;
                ret = AM_ERR_SUCCESS_1ST;
                break;
            }
            g_am_audio_media_handle = audio_codec_wav_codec_open(am_files_audio_callback, NULL);
            if (g_am_audio_media_handle) {
                memset(&g_am_files_media_handle, 0x00, sizeof(bt_sink_srv_am_files_media_handle_t));
                g_am_files_media_handle.type = FILE_WAV;
                g_am_files_media_handle.media_handle.mp3.get_write_buffer                = am_audio_get_write_buffer;
                g_am_files_media_handle.media_handle.mp3.write_data_done                 = am_audio_write_data_done;
                g_am_files_media_handle.media_handle.mp3.finish_write_data               = am_audio_finish_write_data;
                g_am_files_media_handle.media_handle.mp3.get_data_count                  = am_audio_get_data_count;
                g_am_files_media_handle.media_handle.mp3.get_free_space                  = am_audio_get_free_space;
                g_am_files_media_handle.media_handle.mp3.play                            = am_audio_play;
                g_am_files_media_handle.media_handle.mp3.pause                           = am_audio_pause;
                g_am_files_media_handle.media_handle.mp3.resume                          = am_audio_resume;
                g_am_files_media_handle.media_handle.mp3.stop                            = am_audio_stop;
                g_am_files_media_handle.media_handle.mp3.close_codec                     = am_audio_close_codec;

                ret = AM_ERR_SUCCESS_1ST;
            } else {
                ret = AM_ERR_FAIL_1ST;
            }
        #endif /* __AUDIO_WAV_ENABLE__ */
            break;
        }

        default:
            break;
    }

    if (ret > AM_ERR_SUCCESS_OK) {
        g_am_file_state = FILE_CODEC_OPEN;
        g_rAm_aud_id[g_prCurrent_player->aud_id].use = ID_PLAY_STATE;
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id,
            AUD_FILE_OPEN_CODEC, AUD_FILE_PROC_PTR, &g_am_files_media_handle);

        #ifdef __BT_SINK_SRV_ACF_MODE_SUPPORT__
        bt_sink_srv_set_acf_mode(1);
        #endif
    }

    #ifdef __AM_DEBUG_INFO__
    bt_sink_srv_report("[Sink][AM]files_codec_open--type: %d, ret: %d, err: %d", type, ret, err);
    #endif
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}


static void am_files_codec_close(void)
{
    int32_t ret = AM_ERR_FAIL_7TH, err = AM_ERR_FAIL_7TH;
    am_file_type_t type = FILE_NONE;

    type = g_am_files_media_handle.type;

    switch (type) {
        case FILE_MP3: {
            #ifdef __AUDIO_MP3_ENABLE__
            if (g_am_mp3_media_handle) {
                if ((g_am_file_state == FILE_CODEC_PLAY) || (g_am_file_state == FILE_CODEC_PAUSE)) {
                    err = g_am_mp3_media_handle->stop(g_am_mp3_media_handle);
                }
                ret = g_am_mp3_media_handle->close_codec(g_am_mp3_media_handle);
                g_am_mp3_media_handle = NULL;
                g_am_file_state = FILE_CODEC_CLOSE;
            } else {
                bt_sink_srv_report("[Sink][AM]files_codec_close--empty hd");
            }
            #endif
            break;
        }

        case FILE_WAV: {
            #ifdef __AUDIO_WAV_ENABLE__
            if (g_am_audio_media_handle) {
                if (g_am_file_state == FILE_CODEC_PLAY) {
                    err = g_am_audio_media_handle->stop(g_am_audio_media_handle);
                }
                ret = audio_codec_wav_codec_close(g_am_audio_media_handle);
                g_am_audio_media_handle = NULL;
                g_am_file_state = FILE_CODEC_CLOSE;
            } else {
                bt_sink_srv_report("[Sink][AM]files_codec_close--empty hd");
            }
            #endif /*__AUDIO_WAV_ENABLE__*/
            break;
        }

        default:
            break;
    }

    bt_sink_srv_report("[Sink][AM]files_codec_close--type: %d, ret: %d, err: %d, f_state: %d", type, ret, err, g_am_file_state);
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = ret;
    g_test_fix_warning = err;
    #endif
}


#ifdef __AUDIO_MP3_ENABLE__
static void am_files_mp3_callback(mp3_codec_media_handle_t *hdl, mp3_codec_event_t event)
{
    if (g_prCurrent_player->type == FILES || g_prCurrent_player->type == AWS) {
        if (FILE_MP3 == g_prCurrent_player->local_context.files_format.file_event.type) {
            bt_sink_srv_am_background_t background_info;
            memcpy(&background_info, g_prCurrent_player, sizeof(bt_sink_srv_am_background_t));
            background_info.local_context.files_format.file_event.event.mp3.event = (am_mp3_event_type_t)event;
            //g_prCurrent_player->local_context.files_format.file_event.event.mp3.event = (am_mp3_event_type_t)event;
#ifdef __BT_AWS_SUPPORT__
            if (event == MP3_CODEC_AWS_CHECK_CLOCK_SKEW) {
                g_aws_skew_loop_count++;
                if (g_aws_skew_loop_count >= BT_SINK_SRV_AWS_SKEW_LOOP_COUNT) {
                    // send_amm used the same pointer to store event, take care dont  overlap it
                    bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_FILE_PROC_IND,
                                         MSG_ID_MEDIA_FILE_PROCE_CALL_EXT_REQ,
                                         &background_info, //g_prCurrent_player,
                                         TRUE, ptr_callback_amm);
                }
            } else if (event == MP3_CODEC_AWS_CHECK_UNDERFLOW) {
            } else
#endif
            {
                bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_FILE_PROC_IND,
                                 MSG_ID_MEDIA_FILE_PROCE_CALL_EXT_REQ, &background_info, // g_prCurrent_player,
                                 FALSE, NULL);
            }
        }
    } else {
        bt_sink_srv_report("[Sink[AM]]am_files_mp3_callback type %d event %d", g_prCurrent_player->type, g_prCurrent_player->local_context.files_format.file_event.type);
    }
}


static void am_mp3_get_write_buffer(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->get_write_buffer(g_am_mp3_media_handle, buffer, length);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]get_write_buffer--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}

static void am_mp3_get_read_buffer(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->get_read_buffer(g_am_mp3_media_handle, buffer, length);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]get_write_buffer--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}

static int32_t am_mp3_get_data_count(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;
    int32_t ret = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        ret = g_am_mp3_media_handle->get_data_count(g_am_mp3_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]get_data_count--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    return ret;
}

static int32_t am_mp3_get_free_space(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;
    int32_t ret = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        ret = g_am_mp3_media_handle->get_free_space(g_am_mp3_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]get_free_space--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    return ret;
}


static void am_mp3_write_data_done(bt_sink_srv_am_id_t aud_id, uint32_t length)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->write_data_done(g_am_mp3_media_handle, length);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]write_data_done--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}


static void am_mp3_finish_write_data(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->finish_write_data(g_am_mp3_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]finish_write_data--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}


static int32_t am_mp3_play(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->play(g_am_mp3_media_handle);
        g_am_file_state = FILE_CODEC_PLAY;
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]mp3_play--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    return err;
}


static int32_t am_mp3_pause(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->pause(g_am_mp3_media_handle);
        g_am_file_state = FILE_CODEC_PAUSE;
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]mp3_pause--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    return err;
}


static int32_t am_mp3_resume(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->resume(g_am_mp3_media_handle);
        g_am_file_state = FILE_CODEC_PLAY;
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]mp3_resume--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    return err;
}


static int32_t am_mp3_stop(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->stop(g_am_mp3_media_handle);
        g_am_file_state = FILE_CODEC_STOP;
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]mp3_stop--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    return err;
}


static int32_t am_mp3_close_codec(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->close_codec(g_am_mp3_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]mp3_close_codec--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    return err;
}


static int32_t am_mp3_skip_id3v2_and_reach_next_frame(bt_sink_srv_am_id_t aud_id,
    uint32_t file_size)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        err = g_am_mp3_media_handle->skip_id3v2_and_reach_next_frame(g_am_mp3_media_handle, file_size);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]skip_id3v2--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    #ifdef __AM_DEBUG_INFO__
    bt_sink_srv_report("[Sink][AM]skip_id3v2(e)--err: %d, size: %d", err, file_size);
    #endif

    return err;
}


static int32_t am_mp3_set_silence_frame_information(bt_sink_srv_am_id_t aud_id, silence_frame_information_t *frm_info)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        bt_sink_srv_report("[Sink][AM]set_silence_frame_information--frame %x size %d", frm_info->frame, frm_info->frame_size);
        err = g_am_mp3_media_handle->set_silence_frame_information(g_am_mp3_media_handle, frm_info);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]set_silence_frame_information--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifdef __AM_DEBUG_INFO__
    bt_sink_srv_report("[Sink][AM]set_silence_frame_information(e)--err: %d", err);
    #endif

    return err;
}


static int32_t am_mp3_fill_silence_frame(bt_sink_srv_am_id_t aud_id, uint8_t *buffer)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->fill_silence_frame(g_am_mp3_media_handle, buffer);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]fill_silence_frame--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    #ifdef __AM_DEBUG_INFO__
    bt_sink_srv_report("[Sink][AM]fill_silence_frame--err(e)--err: %d", err);
    #endif
    return err;
}


static int32_t am_mp3_get_data_status(bt_sink_srv_am_id_t aud_id,
        mp3_codec_data_type_t type, int32_t *status)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->get_data_status(g_am_mp3_media_handle, type, status);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]fill_silence_frame--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    #ifdef __AM_DEBUG_INFO__
    bt_sink_srv_report("[Sink][AM]fill_silence_frame--err(e)--err: %d", err);
    #endif
    return err;
}


static int32_t am_mp3_flush(bt_sink_srv_am_id_t aud_id, int32_t flush_data_flag)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->flush(g_am_mp3_media_handle, flush_data_flag);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]am_mp3_flush--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    #ifdef __AM_DEBUG_INFO__
    bt_sink_srv_report("[Sink][AM]am_mp3_flush--err(e)--err: %d", err);
    #endif
    return err;
}

#endif /* __AUDIO_MP3_ENABLE__*/


static void aud_process_mp3_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    am_file_event_t file_event;
    uint32_t offset = 0;
    bt_sink_srv_am_cb_sub_msg_t cb_sub_msg = AUD_FILE_EVENT_BASE;
    am_mp3_event_type_t event_id = amm_ptr->background_info.local_context.files_format.file_event.event.mp3.event;

    file_event.type = amm_ptr->background_info.local_context.files_format.file_type;
    file_event.event.mp3.event = event_id;

    if ( FILE_MP3 == file_event.type ) {
        switch (event_id) {
            case AM_MP3_CODEC_MEDIA_REQUEST: {
                cb_sub_msg = AUD_FILE_EVENT_DATA_REQ;
                break;
            }
            case AM_MP3_CODEC_MEDIA_UNDERFLOW: {
                cb_sub_msg = AUD_FILE_EVENT_UNDERFLOW;
                break;
            }

            case AM_MP3_CODEC_MEDIA_JUMP_FILE_TO: {
                offset = g_am_mp3_media_handle->jump_file_to_specified_position;
                file_event.event.mp3.param = (void *)offset;
                cb_sub_msg = AUD_FILE_EVENT_JUMP_INFO;
                break;
            }

            case AM_MP3_CODEC_AWS_CHECK_UNDERFLOW: {
                break;
            }

            case AM_MP3_CODEC_MEDIA_BITSTREAM_END: {
                cb_sub_msg = AUD_FILE_MP3_BITSTREAM_END;
                break;
            }
    #ifdef __BT_AWS_SUPPORT__
            case AM_MP3_CODEC_AWS_CHECK_CLOCK_SKEW: {
                //bt_sink_srv_audio_clock_calibrate(&audio_calc);
                cb_sub_msg = AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW;
                g_aws_skew_loop_count = 0;
                break;
            }
    #endif

            default:
                break;
        }
    } else {
        switch (event_id) {
            case AM_AUDIO_CODEC_MEDIA_REQUEST: {
                cb_sub_msg = AUD_FILE_EVENT_DATA_REQ;
            } break;
            case AM_AUDIO_CODEC_MEDIA_UNDERFLOW: {
                cb_sub_msg = AUD_FILE_EVENT_UNDERFLOW;
            } break;
            case AM_AUDIO_CODEC_MEDIA_EVENT_END: {
                cb_sub_msg = AUD_FILE_EVENT_DATA_END;
            } break;
            default:
                break;
        }
    }

    g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_FILE_PROC_IND, cb_sub_msg, &file_event);
}

#ifdef __AUDIO_COMMON_CODEC_ENABLE__

static void am_files_audio_callback(audio_codec_media_handle_t *hdl, audio_codec_event_t event)
{
    if (g_prCurrent_player->type == FILES) {
        g_prCurrent_player->local_context.files_format.file_event.event.audio.event = event;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_FILE_PROC_IND,
                             MSG_ID_MEDIA_FILE_PROCE_CALL_EXT_REQ, g_prCurrent_player,
                             TRUE, ptr_callback_amm);
    }
}

static void am_audio_get_write_buffer(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length)
{
    int32_t err = 0;
    //am_file_type_t type = g_prCurrent_player->local_context.files_format.file_event.type;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_audio_media_handle->get_write_buffer(g_am_audio_media_handle, buffer, length);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]get_write_buffer--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}


static void am_audio_write_data_done(bt_sink_srv_am_id_t aud_id, uint32_t length)
{
    int32_t err = 0;
    //am_file_type_t type = g_prCurrent_player->local_context.files_format.file_event.type;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_audio_media_handle->write_data_done(g_am_audio_media_handle, length);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]write_data_done--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}


static void am_audio_finish_write_data(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;
    //am_file_type_t type = g_prCurrent_player->local_context.files_format.file_event.type;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_audio_media_handle->finish_write_data(g_am_audio_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]finish_write_data--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}

static int32_t am_audio_get_data_count(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;
    //am_file_type_t type = g_prCurrent_player->local_context.files_format.file_event.type;

    if (aud_id == g_prCurrent_player->aud_id) {
        return g_am_audio_media_handle->get_data_count(g_am_audio_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]get_data_count--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif

    return 0;
}

static int32_t am_audio_get_free_space(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;
    //am_file_type_t type = g_prCurrent_player->local_context.files_format.file_event.type;

    if (aud_id == g_prCurrent_player->aud_id) {
        return g_am_audio_media_handle->get_free_space(g_am_audio_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]get_data_count--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif

    return 0;
}

#define _AM_AUDIO_PROCESS_(_action_,aud_id,_file_state_) { \
    int32_t err = 0; \
    if (aud_id == g_prCurrent_player->aud_id) {\
        g_am_audio_media_handle->_action_(g_am_audio_media_handle);\
        g_am_file_state = FILE_CODEC_##_file_state_; \
    } else { \
        err = AM_ERR_FAIL_1ST; \
    } \
    return err; \
}

static int32_t am_audio_play(bt_sink_srv_am_id_t aud_id) {
    // _AM_AUDIO_PROCESS_(play,aud_id,PLAY);
    int32_t err = 0;
    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_audio_media_handle->play(g_am_audio_media_handle);
        g_am_file_state = FILE_CODEC_PLAY;
    } else {
        err = AM_ERR_FAIL_1ST;
    }
    return err;
}

static int32_t am_audio_pause(bt_sink_srv_am_id_t aud_id) {
    _AM_AUDIO_PROCESS_(pause,aud_id,PAUSE);
}

static int32_t am_audio_resume(bt_sink_srv_am_id_t aud_id) {
    _AM_AUDIO_PROCESS_(resume,aud_id,PLAY);
}

static int32_t am_audio_stop(bt_sink_srv_am_id_t aud_id) {
    _AM_AUDIO_PROCESS_(stop,aud_id,STOP);
}

#undef _AM_AUDIO_PROCESS_

static int32_t am_audio_close_codec(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        audio_codec_wav_codec_close(g_am_audio_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        bt_sink_srv_report("[Sink][AM]audio_close_codec--err: %d, c_aid: %d, aid: %d",
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    return err;
}

#endif /*__AUDIO_COMMON_CODEC_ENABLE__*/

void bt_sink_srv_am_set_volume_change(bool enable)
{
    g_am_volume_enable = enable;
    printf("[AudM]set_volume_change-enalbe: %d\n", enable);
}


#ifdef __BT_SINK_SRV_ACF_MODE_SUPPORT__

static uint32_t g_sink_srv_audio_iir_filter_array[AMOUNT_OF_AUDIO_FILTERS * EAPS_AUDIO_IIR_FILTER_PARAMETER_SIZE];

extern int32_t audio_update_iir_design(const uint32_t *parameter);

/*
    A2DP ACF_MODE1 index 0
    LOCAL PLAYER(MP3) ACF_MODE2 index 1
*/

void bt_sink_srv_set_acf_mode(uint8_t mode)
{
    int32_t i = 0, j = 0, m = 0, n = 0, k = 0;
    const audio_eaps_t *am_speech_eaps = audio_nvdm_get_global_eaps_address();;
    int32_t ret = 0;

    //am_speech_eaps = (audio_eaps_t *)pvPortMalloc(sizeof(audio_eaps_t));

    if (am_speech_eaps) {
        //audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);

        m = AMOUNT_OF_AUDIO_FILTERS;
        n = EAPS_AUDIO_IIR_FILTER_PARAMETER_SIZE;
        k = 0;
        for (i = 0; i < m; ++i) {
            for (j = 0; j < n; ++j) {
                g_sink_srv_audio_iir_filter_array[k++] = am_speech_eaps->audio_parameter.audio_post_processing_parameter[mode].audio_post_processing_compensation_filter[i].audio_iir_design_parameter[j];
            }
            bt_sink_srv_report("[Sink][AM]acf--i: %d--0x%x, 0x%x, 0x%x, 0x%x, 0x%x", i,
                am_speech_eaps->audio_parameter.audio_post_processing_parameter[mode].audio_post_processing_compensation_filter[i].audio_iir_design_parameter[0],
                am_speech_eaps->audio_parameter.audio_post_processing_parameter[mode].audio_post_processing_compensation_filter[i].audio_iir_design_parameter[1],
                am_speech_eaps->audio_parameter.audio_post_processing_parameter[mode].audio_post_processing_compensation_filter[i].audio_iir_design_parameter[2],
                am_speech_eaps->audio_parameter.audio_post_processing_parameter[mode].audio_post_processing_compensation_filter[i].audio_iir_design_parameter[3],
                am_speech_eaps->audio_parameter.audio_post_processing_parameter[mode].audio_post_processing_compensation_filter[i].audio_iir_design_parameter[4]);
        }

        audio_update_iir_design((const uint32_t *)g_sink_srv_audio_iir_filter_array);
    } else {
        ret = AM_ERR_FAIL_1ST;
    }

    //if (am_speech_eaps != NULL) {
    //    vPortFree(am_speech_eaps);
    //}

    bt_sink_srv_report("[Sink][AM]acf--ret: %d, mode: %d, k: %d", ret, mode, k);
}
#endif


#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
static void bt_sink_srv_am_set_volume(bt_sink_srv_am_stream_type_t in_out, bt_sink_srv_audio_setting_vol_info_t *vol_info)
{
    uint32_t digital = 0;
    uint32_t analog = 0;
    bt_sink_srv_audio_setting_vol_t vol;
    vol_type_t type;


    memset(&vol, 0, sizeof(bt_sink_srv_audio_setting_vol_t));
    bt_sink_srv_audio_setting_get_vol(vol_info, &vol);
    type = vol.type;
    switch (type) {
        case VOL_A2DP: {
            digital = vol.vol.a2dp_vol.vol.digital;
            analog = vol.vol.a2dp_vol.vol.analog;
            break;
        }

        case VOL_HFP: {
            if (STREAM_IN == in_out) {
                digital = vol.vol.hfp_vol.vol_in.digital;
                analog = vol.vol.hfp_vol.vol_in.analog;
            } else if (STREAM_OUT == in_out) {
                digital = vol.vol.hfp_vol.vol_out.digital;
                analog = vol.vol.hfp_vol.vol_out.analog;
            }
            break;
        }

        case VOL_PCM: {
            digital = vol.vol.pcm_vol.vol.digital;
            analog = vol.vol.pcm_vol.vol.analog;
            break;
        }

        case VOL_MP3: {
            digital = vol.vol.mp3_vol.vol.digital;
            analog = vol.vol.mp3_vol.vol.analog;
            break;
        }

        case VOL_DEF: {
            digital = vol.vol.def_vol.vol.digital;
            analog = vol.vol.def_vol.vol.analog;
            break;
        }

        default:
            break;
    }

    if (g_am_volume_enable) {
        ;
    } else {
        if (STREAM_IN == in_out) {
            hal_audio_set_stream_in_volume(digital, analog);
        } else if (STREAM_OUT == in_out) {
            hal_audio_set_stream_out_volume(digital, analog);
        }
    }
    bt_sink_srv_report("[Sink][Setting]AM set vol--in_out: %d, d: 0x%x, a: 0x%x, type1: %d, type2: %d",
            in_out, digital, analog, vol_info->type, type);
    bt_sink_srv_music_report("[Sink][Setting]AM set vol--in_out: %d, d: 0x%x, a: 0x%x, type1: %d, type2: %d",
            in_out, digital, analog, vol_info->type, type);
}

#endif

