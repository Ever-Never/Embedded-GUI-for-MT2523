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

/* Includes ------------------------------------------------------------------*/
#ifdef MTK_PROMPT_SOUND_ENABLE

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "audio_mixer.h"
#include "audio_codec.h"
#include "bt_sink_srv.h"
#include "mp3_codec.h"
#include "hal_audio_internal_service.h"
#include "prompt_control.h"

log_create_module(VPC, PRINT_LEVEL_INFO);

/********************************************************
 * Macro & Define
 *
 ********************************************************/
#define PROMPT_CONTROL_MAIN_TRACK_GAIN    0x2000 /*weight 0.25*/
#define PROMPT_CONTROL_SIDE_TRACK_GAIN    0x8000 /*weight 1.0 */


/********************************************************
 * Global variable
 *
 ********************************************************/
static uint16_t mixer_main_track_gain = PROMPT_CONTROL_MAIN_TRACK_GAIN;
static uint16_t mixer_side_track_gain = PROMPT_CONTROL_SIDE_TRACK_GAIN;
static audio_codec_media_handle_t *g_codec_handle = NULL;
static mp3_codec_media_handle_t   *g_mp3_codec_handle = NULL;
static prompt_control_callback_t  g_app_callback = NULL;

/********************************************************
 * Function
 *
 ********************************************************/
static void prompt_control_set_mixer_volume()
{
    audio_mixer_status_t status;

    status = audio_mixer_set_volume(mixer_main_track_gain, mixer_side_track_gain);
    if (status != AUDIO_MIXER_STATUS_OK) {
        LOG_I(VPC, "[VPC]Set mixer volume failed : status=%d\r\n", (int32_t)status);
    }
}

static void prompt_control_wav_callback(audio_codec_media_handle_t *handle, audio_codec_event_t event)
{
    LOG_I(VPC, "[VPC]wav tone callback:%d\r\n", (uint32_t)event);
    switch (event) {
        case AUDIO_CODEC_MEDIA_OK:
            if (g_app_callback) {
                g_app_callback(PROMPT_CONTROL_MEDIA_END);
            }
            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_MIXER_TONE_STOP, NULL);
            break;
        default:
            break;
    }
}

static bool prompt_control_play_wav_tone(uint8_t *tone_buf, uint32_t tone_size)
{
    audio_codec_media_handle_t *handle;

    LOG_I(VPC, "[VPC] Play wav tone (0x%x, %d)", tone_buf, tone_size);

    if (tone_size <= 0) {
        LOG_E(VPC, "[VPC]Error found:tone size=%d", tone_size);
        return false;
    }

    handle = audio_codec_wav_codec_open(prompt_control_wav_callback, NULL);
    if (handle != NULL) {
        g_codec_handle = handle;

        handle->set_track_role(handle, AUDIO_MIXER_TRACK_ROLE_SIDE);

        handle->set_bitstream_buffer(handle, tone_buf, tone_size);

        if (handle->play(handle) != AUDIO_CODEC_RETURN_OK) {
            LOG_I(VPC, "[VPC]play WAV codec failed.\r\n");
            prompt_control_stop_tone();
            return false;
        }
    } else {
        LOG_I(VPC, "[VPC]open WAV codec error\r\n");
        return false;
    }

    return true;
}

static void prompt_control_mp3_callback(mp3_codec_media_handle_t *handle, mp3_codec_event_t event)
{
    LOG_I(VPC, "[VPC]mp3 tone callback:%d\r\n", event);
    switch (event) {
        case MP3_CODEC_MEDIA_BITSTREAM_END: {
            if (g_app_callback) {
                g_app_callback(PROMPT_CONTROL_MEDIA_END);
            }
            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_MIXER_TONE_STOP, NULL);
        }
        break;
        default:
            break;
    }
}

static bool prompt_control_play_mp3_tone(uint8_t *tone_buf, uint32_t tone_size)
{
    mp3_codec_media_handle_t *handle;

    LOG_I(VPC, "[VPC] Play mp3 tone (0x%x, %d)", tone_buf, tone_size);

    if (tone_size <= 0) {
        LOG_E(VPC, "[VPC]Error found:tone size=%d", tone_size);
        return false;
    }

    handle = mp3_codec_open(prompt_control_mp3_callback);
    if (handle != NULL) {
        g_mp3_codec_handle = handle;

        mp3_codec_set_memory2(handle);

        handle->set_track_role(handle, AUDIO_MIXER_TRACK_ROLE_SIDE);

        handle->set_bitstream_buffer(handle, tone_buf, tone_size);

        if (handle->play(handle) != MP3_CODEC_RETURN_OK) {
            LOG_I(VPC, "[VPC]play mp3 codec failed.\r\n");
            prompt_control_stop_tone();
            return false;
        }
    } else {
        LOG_I(VPC, "[VPC]open mp3 codec error\r\n");
        return false;
    }
    
    return true;
}

void prompt_control_set_mixer_gain(uint16_t main_gain, uint16_t side_gain)
{
    LOG_I(VPC, "[VPC]set main_gain=0x%x, side_gain=0x%x\n", main_gain, side_gain);
    mixer_main_track_gain = main_gain;
    mixer_side_track_gain = side_gain;
}

bool prompt_control_play_tone(prompt_control_tone_type_t tone_type, uint8_t *tone_buf, uint32_t tone_size, prompt_control_callback_t callback)
{
    bool ret = false;
    
    LOG_I(VPC, "[VPC]mix play internal type=%d, buf=%x, size=%d\n", tone_type, tone_buf, tone_size);

#if defined(__HAL_AUDIO_AWS_SUPPORT__)
    audio_service_aws_skip_clock_skew(1);
#endif  /* __HAL_AUDIO_AWS_SUPPORT__ */

    if (g_codec_handle || g_mp3_codec_handle) {
        return ret;
    }

    g_app_callback = callback;

    prompt_control_set_mixer_volume();

    if (tone_type == VPC_WAV) {
        ret = prompt_control_play_wav_tone(tone_buf, tone_size);
    } else if (tone_type == VPC_MP3) {
        ret = prompt_control_play_mp3_tone(tone_buf, tone_size);
    }
    
    return ret;
}

void prompt_control_stop_tone(void)
{
    LOG_I(VPC, "[VPC]stop codec internal done (0x%x,0x%x).\n", g_codec_handle, g_mp3_codec_handle);

#if defined(__HAL_AUDIO_AWS_SUPPORT__)
    audio_service_aws_skip_clock_skew(0);
#endif  /* __HAL_AUDIO_AWS_SUPPORT__ */

    if (g_codec_handle) {
        if (g_codec_handle->stop(g_codec_handle) != AUDIO_CODEC_RETURN_OK ) {
            LOG_I(VPC, "[VPC]stop WAV codec fail.\n");
        }
        if (audio_codec_wav_codec_close(g_codec_handle) != AUDIO_CODEC_RETURN_OK ) {
            LOG_I(VPC, "[VPC]close WAV codec fail.\n");
        }
        g_codec_handle = NULL;
    } else if (g_mp3_codec_handle) {
        if (g_mp3_codec_handle->stop(g_mp3_codec_handle) != MP3_CODEC_RETURN_OK ) {
            LOG_I(VPC, "[VPC]stop mp3 codec fail.\n");
        }
        if (mp3_codec_close(g_mp3_codec_handle) != MP3_CODEC_RETURN_OK ) {
            LOG_I(VPC, "[VPC]close mp3 codec fail.\n");
        }
        g_mp3_codec_handle = NULL;
    }
    g_app_callback = NULL;
}

#endif

