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


#include <string.h>
#include "mp3_codec_internal.h"
#include "mp3_codec.h"

#include "ff.h"

#include "hal_audio_internal_service.h"

#include "memory_attribute.h"
#include "hal_cm4_topsm.h"
#include "task_def.h"
#include "audio_codec.h"

#if defined(MTK_AUDIO_MIXER_SUPPORT)
#include "prompt_control.h"
#endif

#include "hal_audio_internal_pcm.h"

#ifdef HAL_AUDIO_LOW_POWER_ENABLED
#include "hal_audio_low_power.h"
#endif
#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs.h"
#define MP3_CPU_FREQ_L_BOUND (104000)
#endif

#ifdef MTK_PROMPT_SOUND_ENABLE
#include "prompt_control.h"
#endif

#include "syslog.h"

log_create_module(MP3_CODEC, PRINT_LEVEL_INFO);
//#define DEBUG_USE_PRINTF
#ifdef DEBUG_USE_PRINTF
#define LOGI(fmt, arg...)          printf(fmt, ##arg);
#define LOGW(fmt, arg...)          printf(fmt, ##arg);
#define LOGE(fmt, arg...)          printf(fmt, ##arg);
#else
#define LOGI(fmt, arg...)          LOG_I(MP3_CODEC, fmt, ##arg);
#define LOGW(fmt, arg...)          LOG_W(MP3_CODEC, fmt, ##arg);
#define LOGE(fmt, arg...)          LOG_E(MP3_CODEC, fmt, ##arg);
#endif

#define MINIMUM_MP3_FRAME_SIZE (24) // in bytes
#define MAXIMUM_MP3_FRAME_SIZE (1441) // in bytes

#if defined(MTK_AUDIO_MIXER_SUPPORT)
#else
static mp3_codec_media_handle_t *global_mp3_handle = NULL; // backward compatibility for "mp3_codec_set_memory2(void)"
#endif

static volatile bool blIsRegister = false;
static volatile int32_t mp3_codec_open_counter = 0;

void mp3_codec_enter_suspend(void *data);
void mp3_codec_enter_resume(void *data);

//pcm playback use
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
#define AUDIO_INTERNAL_RING_BUFFER_SIZE (1024 * 12)
#else
#define AUDIO_INTERNAL_RING_BUFFER_SIZE (4096)
#endif
//mp3 decoder lib use

#ifdef HAL_AUDIO_LOW_POWER_ENABLED
#define MP3_DECODE_BUFFER_SIZE (41000 + 15000 + 4096)
#else
#define MP3_DECODE_BUFFER_SIZE (41000 + 1024) // since we add share_buffer_data_amount <= SHARE_BUFFER_TOO_LESS_AMOUNT feature so add 1024 to compensate it.
#endif

/* for calculate MCPS
volatile int *DWT_CONTROL = (int *)0xE0001000;
volatile int *DWT_CYCCNT = (int *)0xE0001004;
volatile int *DEMCR = (int *)0xE000EDFC;
volatile uint32_t count_test = 0;
volatile uint32_t offset = 0;
#define CPU_RESET_CYCLECOUNTER do { *DEMCR = *DEMCR | 0x01000000; \
*DWT_CYCCNT = 0; \
*DWT_CONTROL = *DWT_CONTROL | 1 ; } while(0)
*/

#define ID3V2_HEADER_LENGTH 10  // in bytes
#define MPEG_AUDIO_FRAME_HEADER_LENGTH 4 // in bytes
static int32_t MP3SilenceFrameSize = 0; // in bytes
static uint8_t MP3SilenceFrameHeader[MPEG_AUDIO_FRAME_HEADER_LENGTH];

static char mp3_silence_frame_pattern[] = "This is a silence frame."; // sizeof(mp3_silence_frame_pattern) = 25, sizeof(char) = 1
#define MP3_SILENCE_FRAME_PATTERN_LENGTH (25) // in bytes

/*
#define IS_MP3_HEAD(head) (!( ((head & 0xfff80000) != 0xfff80000) || \
  ( ((head>>17)&3)!= 1) || \
  ( ((head>>12)&0xf) == 0xf) || ( ((head>>12)&0xf) == 0x0) || \
( ((head>>10)&0x3) == 0x3 )))
*/

#define IS_MP3_HEAD(head) (!( (((head & 0xfff00000) != 0xfff00000) && ((head & 0xfff80000) != 0xffe00000) ) || \
  ( ((head>>17)&3)== 3) || (((head>>17)&3)== 0) || \
  ( ((head>>12)&0xf) == 0xf) || ( ((head>>12)&0xf) == 0x0) || \
( ((head>>10)&0x3) == 0x3 )))


#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_NA    0
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_32    1
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_40    2
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_48    3
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_56    4
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_64    5
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_80    6
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_96    7
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_112    8
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_128    9
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_160    0xa
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_192    0xb
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_224    0xc
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_256    0xd
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_320    0xe
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_NA2    0xf


#define SHARE_BUFFER_TOO_LESS_AMOUNT 1000        // maybe can be more precisely
#define SHARE_BUFFER_TOO_LESS_SLEEP_TIME_IN_MS 30
#define SHARE_BUFFER_TOO_LESS_SLEEP_LOOP_TIMES 10

//#define MP3_DEBUG_LOG
#if defined(MP3_DEBUG_LOG)
#define MP3_DEBUG_PRINTF(message...)   do { printf(message); } while (0)
#else
#define MP3_DEBUG_PRINTF(message...)
#endif

IPCOMMON_PLUS

static void mp3_codec_event_send_from_isr(mp3_codec_queue_event_id_t id, void *parameter);

#ifdef HAL_DVFS_MODULE_ENABLED
static bool mp3_dvfs_valid(uint32_t voltage, uint32_t frequency)
{
    if (frequency < MP3_CPU_FREQ_L_BOUND) {
        return false;
    } else {
        return true;
    }
}

static dvfs_notification_t mp3_dvfs_desc = {
    .domain = "VCORE",
    .module = "CM_CK0",
    .addressee = "mp3_dvfs",
    .ops = {
        .valid = mp3_dvfs_valid,
    }
};

static void mp3_codec_register_mp3_dvfs(bool flag)
{
    if (flag) {
        dvfs_register_notification(&mp3_dvfs_desc);
        hal_dvfs_target_cpu_frequency(MP3_CPU_FREQ_L_BOUND, HAL_DVFS_FREQ_RELATION_L);
    } else {
        dvfs_deregister_notification(&mp3_dvfs_desc);
    }

}
#endif /*HAL_DVFS_MODULE_ENABLED*/

#if defined(MTK_AUDIO_MIXER_SUPPORT)
void mp3_codec_set_track_role(mp3_codec_media_handle_t *handle, audio_mixer_role_t role)
{
        handle->mixer_track_role = role;
        return;
}
#endif

// ======== Private lock functions ========
static void mp3_codec_mutex_lock(mp3_codec_media_handle_t *handle)
{
    mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(handle);

    if (internal_handle->semaphore_handle != NULL) {
        LOGI("[MP3 Codec] mp3_codec_mutex_lock() +\r\n");
        xSemaphoreTake(internal_handle->semaphore_handle, portMAX_DELAY);
        LOGI("[MP3 Codec] mp3_codec_mutex_lock() -\r\n");
    }
}

static void mp3_codec_mutex_unlock(mp3_codec_media_handle_t *handle)
{
    mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(handle);

    if (internal_handle->semaphore_handle != NULL) {
        LOGI("[MP3 Codec] mp3_codec_mutex_unlock()\r\n");
        xSemaphoreGive(internal_handle->semaphore_handle);
    }
}

/*	share buffer operation function */
static mp3_codec_function_return_state_t mp3_codec_set_bitstream_buffer(mp3_codec_media_handle_t *handle, uint8_t *buffer, uint32_t length)
{
    length = (length + 3) & ~0x3;
    handle->share_buff.buffer_base = buffer;
    handle->share_buff.buffer_size = length;
    handle->share_buff.write = length;
    handle->share_buff.read = 0;
    handle->waiting = false;
    handle->underflow = false;
    handle->linear_buff = true;

    return MP3_CODEC_RETURN_OK;
}

static void mp3_codec_set_share_buffer(mp3_codec_media_handle_t *handle, uint8_t *buffer, uint32_t length)
{
    handle->share_buff.buffer_base = buffer;
    //length &= ~0x1; // make buffer size even
    handle->share_buff.buffer_size = length;
    handle->share_buff.write = 0;
    handle->share_buff.read = 0;
    handle->waiting = false;
    handle->underflow = false;
    handle->linear_buff = false;
}

static void mp3_codec_get_share_buffer_write_information(mp3_codec_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->share_buff.read > handle->share_buff.write) {
        count = handle->share_buff.read - handle->share_buff.write - 1;
    } else if (handle->share_buff.read == 0) {
        count = handle->share_buff.buffer_size - handle->share_buff.write - 1;
    } else {
        count = handle->share_buff.buffer_size - handle->share_buff.write;
    }
    *buffer = handle->share_buff.buffer_base + handle->share_buff.write;
    *length = count;
}


static void mp3_codec_get_share_buffer_read_information(mp3_codec_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->share_buff.write >= handle->share_buff.read) {
        count = handle->share_buff.write - handle->share_buff.read;
    } else {
        count = handle->share_buff.buffer_size - handle->share_buff.read;
    }
    *buffer = handle->share_buff.buffer_base + handle->share_buff.read;
    *length = count;
}


static void mp3_codec_share_buffer_write_data_done(mp3_codec_media_handle_t *handle, uint32_t length)
{
    handle->share_buff.write += length;
    if (handle->share_buff.write == handle->share_buff.buffer_size) {
        handle->share_buff.write = 0;
    }
}

static int32_t mp3_codec_get_share_buffer_data_count(mp3_codec_media_handle_t *);
static void mp3_codec_finish_write_data(mp3_codec_media_handle_t *handle)
{
    handle->waiting = false;
    handle->underflow = false;

    if (MP3_CODEC_STATE_PLAY == handle->state) {
        int32_t share_buffer_data_amount = mp3_codec_get_share_buffer_data_count(handle);
        if (mp3_handle_ptr_to_previous_mp3_frame_size(handle) > 0) {
            if (share_buffer_data_amount > mp3_handle_ptr_to_previous_mp3_frame_size(handle)) {
                mp3_codec_event_send_from_isr(MP3_CODEC_QUEUE_EVENT_DECODE, handle);
            }
        } else if (share_buffer_data_amount > SHARE_BUFFER_TOO_LESS_AMOUNT) {
            mp3_codec_event_send_from_isr(MP3_CODEC_QUEUE_EVENT_DECODE, handle);
        }
    }

    uint32_t current_cnt = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_cnt);
#if defined(MTK_AUDIO_MIXER_SUPPORT)
    LOGI("[MP3 Codec] finish_write_data role%d: %d ms\n", handle->mixer_track_role, (current_cnt - mp3_handle_ptr_to_share_buffer_waiting_cnt(handle)) / 1000);
#else
    LOGI("[MP3 Codec] finish_write_data: %d ms\n", (current_cnt - mp3_handle_ptr_to_share_buffer_waiting_cnt(handle)) / 1000);
#endif

}

static void mp3_codec_flush(mp3_codec_media_handle_t *handle, int32_t flush_data_flag)
{
    handle->flush_data_flag = flush_data_flag;
    LOGI("[MP3 Codec] flush_data_flag = %d\n", handle->flush_data_flag);
}

static void mp3_codec_get_data_status(mp3_codec_media_handle_t *handle, mp3_codec_data_type_t data_type, int32_t *data_status)
{
    *data_status = 0;

    switch (data_type) {
        case MP3_CODEC_DATA_TYPE_AUDIO:
            if (handle->state == MP3_CODEC_STATE_PLAY) {
                mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(handle);
                uint8_t *out_buf_ptr    = NULL;
                uint32_t stream_out_pcm_buffer_data_count = 0;

                ring_buffer_get_read_information(&internal_handle->stream_out_pcm_buff, &out_buf_ptr, &stream_out_pcm_buffer_data_count);
                if (stream_out_pcm_buffer_data_count == 0) {
                    uint32_t sample_count = 0;
#if defined(MTK_AUDIO_MIXER_SUPPORT)
                    audio_mixer_query_free_count(handle->mixer_track_id, &sample_count);
#else
                    hal_audio_get_stream_out_sample_count(&sample_count);
#endif

#if defined(MTK_AUDIO_MIXER_SUPPORT)
                    *data_status = 1;
#else
                    if (mp3_handle_ptr_to_total_sample_count(handle) <= sample_count) {
                        *data_status = 1;
                    }
#endif
                }
            }
            break;
        case MP3_CODEC_DATA_TYPE_SAMPLES_PER_CHANNEL:
            if (handle->state == MP3_CODEC_STATE_PLAY) {
                mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(handle);
                *data_status = internal_handle->mp3_handle->PCMSamplesPerCH;
            }
            break;
    }
}

static void mp3_codec_reset_share_buffer(mp3_codec_media_handle_t *handle)
{
    memset(handle->share_buff.buffer_base, 0, handle->share_buff.buffer_size);  // do this or it will have previous data. we met that when change to bad mp3 file, but at this time app still use write_data_done, it will have previous header

    handle->share_buff.write = 0;
    handle->share_buff.read = 0;
    handle->waiting = false;
    handle->underflow = false;
}


static void mp3_codec_share_buffer_read_data_done(mp3_codec_media_handle_t *handle, uint32_t length)
{
    handle->share_buff.read += length;
    if (handle->share_buff.read == handle->share_buff.buffer_size) {
        handle->share_buff.read = 0;
    }
}


static int32_t mp3_codec_get_share_buffer_free_space(mp3_codec_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->share_buff.read - handle->share_buff.write - 2;
    if (count < 0) {
        count += handle->share_buff.buffer_size;
    }
    return count;
}

static int32_t mp3_codec_get_share_buffer_data_count(mp3_codec_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->share_buff.write - handle->share_buff.read;
    if (count < 0) {
        count += handle->share_buff.buffer_size;
    }
    return count;
}

static void mp3_codec_reset_stream_out_pcm_buffer(mp3_codec_media_handle_t *handle)
{
    mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(handle);
    internal_handle->stream_out_pcm_buff.read_pointer = 0;
    internal_handle->stream_out_pcm_buff.write_pointer = 0;
}

static int32_t mp3_codec_set_silence_frame_information(mp3_codec_media_handle_t *handle, silence_frame_information_t *frm_info)
{
    if (frm_info->frame_size > 0) {
        MP3SilenceFrameSize = frm_info->frame_size;
        MP3SilenceFrameHeader[0] = frm_info->frame[0];
        MP3SilenceFrameHeader[1] = frm_info->frame[1];
        MP3SilenceFrameHeader[2] = frm_info->frame[2];
        MP3SilenceFrameHeader[3] = frm_info->frame[3];

        LOGI("MP3SilenceFrameSize = %d, MP3SilenceFrameHeader = 0x%X %X %X %X \n", MP3SilenceFrameSize, 
            MP3SilenceFrameHeader[0], MP3SilenceFrameHeader[1], MP3SilenceFrameHeader[2], MP3SilenceFrameHeader[3]);

        return 0;
    }

    return -1;
}

static int32_t mp3_codec_get_silence_frame_information(mp3_codec_media_handle_t *handle, int32_t *byte_count)
{
    if (MP3SilenceFrameSize > 0) {
        *byte_count = MP3SilenceFrameSize;
    }

    return 0;
}

static int32_t mp3_codec_fill_silence_frame(mp3_codec_media_handle_t *handle, uint8_t *buffer)
{
    if (MP3SilenceFrameSize > 0) {
        memset(buffer, 0, MP3SilenceFrameSize);
        buffer[0] = MP3SilenceFrameHeader[0];
        buffer[1] = MP3SilenceFrameHeader[1];
        buffer[2] = MP3SilenceFrameHeader[2];
        buffer[3] = MP3SilenceFrameHeader[3];
        strcpy((char *)(buffer + MPEG_AUDIO_FRAME_HEADER_LENGTH), mp3_silence_frame_pattern);
    }

    return 0;
}

#ifdef MTK_BT_AWS_ENABLE
void mp3_codec_aws_callback(aws_event_t event, void *user_data)
{
    mp3_codec_media_handle_t *handle = (mp3_codec_media_handle_t *)user_data;
    
    switch (event) {
        case CODEC_AWS_CHECK_CLOCK_SKEW:
            handle->handler(handle, MP3_CODEC_AWS_CHECK_CLOCK_SKEW);
            break;
        case CODEC_AWS_CHECK_UNDERFLOW:
            handle->handler(handle, MP3_CODEC_AWS_CHECK_UNDERFLOW);
            break;
        default:
            break;
    }
    return;
}
#endif

#ifdef MTK_BT_AWS_ENABLE
mp3_codec_function_return_state_t mp3_codec_aws_init(mp3_codec_media_handle_t *handle)
{
    int32_t aws_buf_size = 0;
    uint8_t *p_aws_buf;

    aws_buf_size = audio_service_aws_get_buffer_size(AWS_CODEC_TYPE_PCM_FORMAT);
    if (aws_buf_size > 0) {
        p_aws_buf = (uint8_t *)pvPortMalloc(aws_buf_size);
    } else {
        LOGE("mp3_codec_aws_init failed!!! audio_service_aws_get_buffer_size = %d < 0\n", aws_buf_size);
        return MP3_CODEC_RETURN_ERROR;
    }
    mp3_handle_ptr_to_p_aws_buf(handle) = p_aws_buf;
    LOGI("mp3_codec_aws_init: p_aws_buf = 0x%X, aws_buf_size = %d\n", p_aws_buf, aws_buf_size);
    if (p_aws_buf) {
        aws_buf_size = audio_service_aws_init(p_aws_buf, AWS_CODEC_TYPE_PCM_FORMAT, mp3_codec_aws_callback, (void *)handle);
        LOGI("audio_service_aws_init mp3_codec_aws_callback(0x%X)\n", mp3_codec_aws_callback);
        LOGI("audio_service_aws_init return %d\n", aws_buf_size);
    } else {
        return MP3_CODEC_RETURN_ERROR;
    }
    
    return MP3_CODEC_RETURN_OK;
}
#endif

#ifdef MTK_BT_AWS_ENABLE
mp3_codec_function_return_state_t mp3_codec_aws_deinit(mp3_codec_media_handle_t *handle)
{
    LOGI("mp3_codec_aws_deinit: Free p_aws_buf\r\n");
    vPortFree(mp3_handle_ptr_to_p_aws_buf(handle));
    mp3_handle_ptr_to_p_aws_buf(handle) = NULL;

    audio_service_aws_deinit();
    return MP3_CODEC_RETURN_OK;
}
#endif

#ifdef MTK_BT_AWS_ENABLE
mp3_codec_function_return_state_t mp3_codec_aws_set_clock_skew_compensation_value(mp3_codec_media_handle_t *handle, int32_t sample_count)
{
    audio_service_aws_set_clock_skew_compensation_value(sample_count);

    return MP3_CODEC_RETURN_OK;
}

mp3_codec_function_return_state_t mp3_codec_aws_get_clock_skew_status(mp3_codec_media_handle_t *handle, int32_t *aws_clock_skew_status)
{
    *aws_clock_skew_status = audio_service_aws_get_clock_skew_status();

    return MP3_CODEC_RETURN_OK;
}

mp3_codec_function_return_state_t mp3_codec_aws_set_clock_skew(mp3_codec_media_handle_t *handle, bool flag)
{
    if (flag) {
        audio_service_aws_set_clock_skew(true);
    } else {
        audio_service_aws_set_clock_skew(false);
    }
    return MP3_CODEC_RETURN_OK;
}

#endif

static void mp3_codec_buffer_function_init(mp3_codec_media_handle_t *handle)
{
    handle->set_share_buffer   = mp3_codec_set_share_buffer;
    handle->get_write_buffer   = mp3_codec_get_share_buffer_write_information;
    handle->get_read_buffer    = mp3_codec_get_share_buffer_read_information;
    handle->write_data_done    = mp3_codec_share_buffer_write_data_done;
    handle->finish_write_data  = mp3_codec_finish_write_data;
    handle->reset_share_buffer = mp3_codec_reset_share_buffer;
    handle->read_data_done     = mp3_codec_share_buffer_read_data_done;
    handle->get_free_space     = mp3_codec_get_share_buffer_free_space;
    handle->get_data_count     = mp3_codec_get_share_buffer_data_count;
    handle->flush              = mp3_codec_flush;
    handle->get_data_status    = mp3_codec_get_data_status;
	handle->set_bitstream_buffer = mp3_codec_set_bitstream_buffer;
    handle->set_silence_frame_information = mp3_codec_set_silence_frame_information;
    handle->get_silence_frame_information = mp3_codec_get_silence_frame_information;
    handle->fill_silence_frame = mp3_codec_fill_silence_frame;
#ifdef MTK_BT_AWS_ENABLE
    handle->aws_init            = mp3_codec_aws_init;
    handle->aws_deinit          = mp3_codec_aws_deinit;
    handle->aws_set_clock_skew_compensation_value = mp3_codec_aws_set_clock_skew_compensation_value;
    handle->aws_get_clock_skew_status = mp3_codec_aws_get_clock_skew_status;
    handle->aws_set_clock_skew = mp3_codec_aws_set_clock_skew;
#endif
#if defined(MTK_AUDIO_MIXER_SUPPORT)
    handle->set_track_role     = mp3_codec_set_track_role;
#endif
}


static uint32_t mp3_codec_get_bytes_from_share_buffer(mp3_codec_media_handle_t *handle, uint8_t *destination_buffer, uint32_t get_bytes_amount, bool want_move_read_ptr)
{
    uint8_t *share_buffer_read_address;
    uint32_t share_buffer_data_length;
    uint32_t share_buffer_read_index_original = 0;
    uint32_t bytes_amount_temp = get_bytes_amount;
    uint32_t got_bytes_amount = 0;  // real got bytes amount from share buffer

    share_buffer_read_index_original = handle->share_buff.read; // store original share_buffer read pointer


    uint16_t loop_idx = 0;
    for (loop_idx = 0; loop_idx < 2; loop_idx++) {
        mp3_codec_get_share_buffer_read_information(handle, &share_buffer_read_address, &share_buffer_data_length);
        if (share_buffer_data_length > 0) {
            uint32_t get_bytes_amount = MINIMUM(bytes_amount_temp, share_buffer_data_length);
            memcpy(destination_buffer, share_buffer_read_address, get_bytes_amount);
            bytes_amount_temp -= get_bytes_amount;
            destination_buffer += get_bytes_amount;
            mp3_codec_share_buffer_read_data_done(handle, get_bytes_amount);

            if (bytes_amount_temp == 0) {
                break;
            }
        } else {
            // share buffer empty
            break;
        }
    }


    got_bytes_amount = get_bytes_amount - bytes_amount_temp;  // real read amount

    if (got_bytes_amount != get_bytes_amount) {
        LOGI("[MP3 Codec]mp3_codec_get_bytes_from_share_buffer: got_bytes_amount(%ld) != get_bytes_amount(%ld)\n", got_bytes_amount, get_bytes_amount);
    }


    if (want_move_read_ptr == false) {
        handle->share_buff.read = share_buffer_read_index_original;
    }

    return got_bytes_amount;
}



static uint32_t mp3_codec_discard_bytes_of_share_buffer(mp3_codec_media_handle_t *handle, uint32_t discard_bytes_amount)
{
    uint8_t *share_buffer_read_address;
    uint32_t share_buffer_data_length;
    uint32_t bytes_amount_temp = discard_bytes_amount;
    uint32_t discarded_bytes_amount = 0;


    uint16_t loop_idx = 0;
    for (loop_idx = 0; loop_idx < 2; loop_idx++) {
        mp3_codec_get_share_buffer_read_information(handle, &share_buffer_read_address, &share_buffer_data_length);
        if (share_buffer_data_length > 0) {
            uint32_t get_bytes_amount = MINIMUM(bytes_amount_temp, share_buffer_data_length);
            bytes_amount_temp -= get_bytes_amount;
            mp3_codec_share_buffer_read_data_done(handle, get_bytes_amount);

            if (bytes_amount_temp == 0) {
                break;
            }
        } else {
            // share buffer empty
            break;
        }
    }

    discarded_bytes_amount = discard_bytes_amount - bytes_amount_temp;  // real read amount

    if (discarded_bytes_amount != discard_bytes_amount) {
        LOGI("[MP3 Codec]mp3_codec_discard_bytes_of_share_buffer : discarded_bytes_amount(%ld) != discard_bytes_amount(%ld)\n", discarded_bytes_amount, discard_bytes_amount);
    }


    return discarded_bytes_amount;
}

static mp3_codec_function_return_state_t mp3_codec_request_data_to_share_buffer(mp3_codec_media_handle_t *handle)
{
    // return MP3_CODEC_RETURN_OK:          request success
    // return  MP3_CODEC_RETURN_ERROR:    already request and waiting feed back

    mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(handle);

    if (handle->linear_buff == 1) {
        return MP3_CODEC_RETURN_OK; // Just return in Linear Buffer 
    }

    if (!handle->waiting) {
#if defined(MTK_AUDIO_MIXER_SUPPORT)
        LOGI("[MP3 Codec] role%d request data: share = %d, pcm = %d\r\n", handle->mixer_track_role, mp3_codec_get_share_buffer_data_count(handle), ring_buffer_get_space_byte_count(&internal_handle->stream_out_pcm_buff));
#else
        LOGI("[MP3 Codec] request data: share = %d, pcm = %d\r\n", mp3_codec_get_share_buffer_data_count(handle), ring_buffer_get_space_byte_count(&internal_handle->stream_out_pcm_buff));
#endif
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &(mp3_handle_ptr_to_share_buffer_waiting_cnt(handle)));
        handle->waiting = true;
        handle->handler(handle, MP3_CODEC_MEDIA_REQUEST);
        return MP3_CODEC_RETURN_OK;
    } else {
#if defined(MTK_AUDIO_MIXER_SUPPORT)
        LOGI("[MP3 Codec] role%d request data again: share = %d, pcm = %d\r\n", handle->mixer_track_role, mp3_codec_get_share_buffer_data_count(handle), ring_buffer_get_space_byte_count(&internal_handle->stream_out_pcm_buff));
#else
        LOGI("[MP3 Codec] request data again: share = %d, pcm = %d\r\n", mp3_codec_get_share_buffer_data_count(handle), ring_buffer_get_space_byte_count(&internal_handle->stream_out_pcm_buff));
#endif
        return MP3_CODEC_RETURN_ERROR;
    }
}

static uint32_t mp3_codec_combine_four_bytes_buffer_to_uint32_value(uint8_t *buffer)
{
    uint32_t uint32_value = 0;

    uint32_value = *buffer;
    uint32_value = uint32_value << 8 | *(buffer + 1);
    uint32_value = uint32_value << 8 | *(buffer + 2);
    uint32_value = uint32_value << 8 | *(buffer + 3);

    return uint32_value;
}

static mp3_codec_function_return_state_t mp3_codec_reach_next_frame_and_get_audio_frame_header(mp3_codec_media_handle_t *handle, uint32_t *audio_frame_header, uint32_t maximum_check_bytes, uint32_t want_skip_frame_after_got_header)
{
    uint8_t check_mpeg_header_buffer[MPEG_AUDIO_FRAME_HEADER_LENGTH] = {0};
    uint32_t temp_mpeg_header = 0;
    uint32_t discard_bytes_amount = 0;
    uint32_t temp_uint32_t = 0;
    uint32_t temp_maximum_check_bytes = maximum_check_bytes;
    uint32_t maximum_request_data_time = maximum_check_bytes / handle->share_buff.buffer_size + 2;  // 2: arbitrarily selected


    do {
        if (mp3_codec_get_share_buffer_data_count(handle) < MPEG_AUDIO_FRAME_HEADER_LENGTH) {

            if (mp3_codec_request_data_to_share_buffer(handle) == MP3_CODEC_RETURN_OK) {
                maximum_request_data_time--;
            }

            if (mp3_codec_get_share_buffer_data_count(handle) < MPEG_AUDIO_FRAME_HEADER_LENGTH) {
                return MP3_CODEC_RETURN_ERROR;
            }
        }

        mp3_codec_get_bytes_from_share_buffer(handle, check_mpeg_header_buffer, MPEG_AUDIO_FRAME_HEADER_LENGTH, 0);
        temp_mpeg_header = mp3_codec_combine_four_bytes_buffer_to_uint32_value(check_mpeg_header_buffer);

        if (IS_MP3_HEAD(temp_mpeg_header)) {
            // find MP3 HEAD
            *audio_frame_header = temp_mpeg_header;

            if (want_skip_frame_after_got_header) {
                discard_bytes_amount = 4;
                temp_uint32_t = mp3_codec_discard_bytes_of_share_buffer(handle, discard_bytes_amount);
                if (temp_uint32_t < discard_bytes_amount) {  // share buffer didn't have enoungh data to discared
                    return MP3_CODEC_RETURN_ERROR;
                }
            }
            LOGI("[MP3 Codec]mp3_codec_reach_next_frame: find mp3 header=%x\n", *audio_frame_header);
            return MP3_CODEC_RETURN_OK;
        }

        discard_bytes_amount = 1;
        temp_uint32_t = MINIMUM(discard_bytes_amount, temp_maximum_check_bytes);
        temp_uint32_t = mp3_codec_discard_bytes_of_share_buffer(handle, temp_uint32_t);
        if (temp_uint32_t < discard_bytes_amount) {  // share buffer didn't have enoungh data to discared
            return MP3_CODEC_RETURN_ERROR;
        }

        temp_maximum_check_bytes -= temp_uint32_t;

    } while (temp_maximum_check_bytes != 0 && maximum_request_data_time != 0);


    LOGI("[MP3 Codec]mp3_codec_reach_next_frame: not find mp3 header\n");
    *audio_frame_header = 0;

    return MP3_CODEC_RETURN_ERROR;
}


static void mp3_codec_get_id3v2_info_and_skip(mp3_codec_media_handle_t *handle, uint32_t file_size)
{
    uint32_t want_get_bytes = 0;
    uint8_t temp_buffer[10] = {0};
    uint32_t id3v2_remain_tagesize = 0; // not include ID3v2 header size, refert to ID3v2 spec
    uint32_t id3v2_tage_size = 0;   // total ID3v2 tage size which include ID3v2 header
    uint32_t remain_file_data_size = file_size; // in bytes

    handle->jump_file_to_specified_position = 0;    // asume from file begin
    handle->id3v2_information.has_id3v2 = false;
    handle->id3v2_information.id3v2_tage_length = 0;

    while (1) {
        want_get_bytes = ID3V2_HEADER_LENGTH;
        if (mp3_codec_get_bytes_from_share_buffer(handle, temp_buffer, want_get_bytes, 0) != want_get_bytes) {
            LOGI("[MP3 Codec]mp3_codec_get_id3v2_info_and_skip: share buffer data amount less than ID3v2 header length\n");
            return;    // just return
        }

        if (strncmp((const char *)temp_buffer, "ID3", 3) == 0) {
            id3v2_remain_tagesize = ((temp_buffer[6] & 0x7f) << 21) | ((temp_buffer[7] & 0x7f) << 14) | ((temp_buffer[8] & 0x7f) <<  7) | ((temp_buffer[9] & 0x7f) <<  0);
            id3v2_tage_size = id3v2_remain_tagesize + ID3V2_HEADER_LENGTH;
            LOGI("[MP3 Codec]find id3v2: id3v2_tagesize=%ld, id3v2_remain_tagesize =%ld\n", id3v2_tage_size, id3v2_remain_tagesize);


            if (remain_file_data_size < id3v2_tage_size) {
                // the tag size calculate form ID3v2 may wrong
                return;
            }


            handle->id3v2_information.has_id3v2 = true;
            handle->id3v2_information.id3v2_tage_length += id3v2_tage_size;


            // Although the remaing data in share buffer can be used,
            // but the fast and clear way to skip ID3v2 is just ask user to jump file to specific position and refill the share buffer
            mp3_codec_reset_share_buffer(handle);   // since we want to ask user to jump file to specific position and get data, thus remaining data is no use.
            handle->jump_file_to_specified_position += id3v2_tage_size;
            handle->handler(handle, MP3_CODEC_MEDIA_JUMP_FILE_TO);

            mp3_codec_request_data_to_share_buffer(handle);


            remain_file_data_size -= id3v2_tage_size;


        } else {
            LOGI("[MP3 Codec]done skip ID3v2, has_id3v2=%d, id3v2_tage_length=%d\n", (uint32_t)handle->id3v2_information.has_id3v2, handle->id3v2_information.id3v2_tage_length);
            return;
        }

    }
}


#if 0

mp3_codec_function_return_state_t  mp3_codec_set_memory(uint8_t *memory)
{

    if (mp3_codec_internal_handle == NULL) {
        LOGE("[MP3 Codec]mp3_codec_internal_handle = NULL\n");
        return MP3_CODEC_RETURN_ERROR;
    }
    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;
    mp3_codec_media_handle_t *handle = &internal_handle->handle;

    if (handle->state != MP3_CODEC_STATE_READY) {
        return MP3_CODEC_RETURN_ERROR;
    }
    uint8_t *memory_base = NULL;
    uint32_t totl_memory_size = 0;
    internal_handle->memory_pool = memory_base = memory;

    totl_memory_size = internal_handle->share_buff_size
                       + internal_handle->decode_pcm_buffer_size
                       + internal_handle->working_buff1_size
                       + internal_handle->working_buff2_size
                       + internal_handle->stream_out_pcm_buff_size;

    //set share buffer
    mp3_codec_set_share_buffer(handle, memory_base, internal_handle->share_buff_size);
    memory_base += internal_handle->share_buff_size;

    // set PCM buffer
    internal_handle->decode_pcm_buff.buffer_base_pointer = memory_base;
    internal_handle->decode_pcm_buff.buffer_byte_count = internal_handle->decode_pcm_buffer_size;
    internal_handle->decode_pcm_buff.read_pointer = 0;
    internal_handle->decode_pcm_buff.write_pointer = 0;
    memory_base += internal_handle->decode_pcm_buffer_size;

    //set working buffer
    internal_handle->working_buff1 = memory_base;
    memory_base += internal_handle->working_buff1_size;
    internal_handle->working_buff2 = memory_base;
    memory_base += internal_handle->working_buff2_size;


    // set PCM buffer   // piter add
    internal_handle->stream_out_pcm_buff.buffer_base_pointer = memory_base;
    internal_handle->stream_out_pcm_buff.buffer_byte_count = internal_handle->stream_out_pcm_buff_size;
    internal_handle->stream_out_pcm_buff.read_pointer = 0;
    internal_handle->stream_out_pcm_buff.write_pointer = 0;


    // STEP 2 : Get MP3 Handler
    internal_handle->mp3_handle = MP3Dec_Init(internal_handle->working_buff1, internal_handle->working_buff2);
    if (internal_handle->mp3_handle == NULL) {
        LOGE("[MP3 Codec]MP3Dec_Init fail");
    }

#if 1
    LOGI("[MP3 Codec]set_memory memory range : start=%08x end=%08x",
                 memory, (memory + totl_memory_size));
    LOGI("[MP3 Codec]set_memory share buffer : base=%08x size=%u",
                 handle->share_buff.buffer_base, (unsigned int)handle->share_buff.buffer_size);
    LOGI("[MP3 Codec]set_memory decode_pcm_buff.buffer : base=%08x size=%u",
                 internal_handle->decode_pcm_buff.buffer_base_pointer, (unsigned int)internal_handle->decode_pcm_buff.buffer_byte_count);
    LOGI("[MP3 Codec]set_memory work1 buffer : base=%08x size=%u",
                 internal_handle->working_buff1, (unsigned int)internal_handle->working_buff1_size);
    LOGI("[MP3 Codec]set_memory work2 buffer : base=%08x size=%u",
                 internal_handle->working_buff2, (unsigned int)internal_handle->working_buff2_size);
    LOGI("[MP3 Codec]set_memory stream_out_pcm_buff : base=%08x size=%u",
                 internal_handle->stream_out_pcm_buff.buffer_base_pointer, (unsigned int)internal_handle->stream_out_pcm_buff.buffer_byte_count);
#endif



    return MP3_CODEC_RETURN_OK;

}

mp3_codec_function_return_state_t mp3_codec_get_memory_size(uint32_t *memory_size)
{

    if (mp3_codec_internal_handle == NULL) {
        LOGE("[MP3 Codec]mp3_codec_internal_handle = NULL\n");
        return MP3_CODEC_RETURN_ERROR;
    }
    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;
    mp3_codec_media_handle_t *handle = &internal_handle->handle;

    if (handle->state != MP3_CODEC_STATE_READY) {
        return MP3_CODEC_RETURN_ERROR;
    }

    int working_buff1_size,         /* the required Working buffer1 size in byte    */
        working_buff2_size,         /* the required Working buffer2 size in byte    */
        decode_pcm_buffer_size,              /* the required pcm buffer size in byte          */
        share_buff_size;            /* the share buffer size      */

    *memory_size = 0;

    /*STEP 1 : Allocate data memory for MP3 decoder*/
    MP3Dec_GetMemSize(&share_buff_size, &decode_pcm_buffer_size, &working_buff1_size, &working_buff2_size);

    //share_buff_size += 1024;  // piter delete

    //4bytes aligned
    share_buff_size = (share_buff_size + 3) & ~0x3;
    decode_pcm_buffer_size = (decode_pcm_buffer_size + 3) & ~0x3;
    working_buff1_size = (working_buff1_size + 3) & ~0x3;
    working_buff2_size = (working_buff2_size + 3) & ~0x3;


    internal_handle->share_buff_size = share_buff_size;
    internal_handle->decode_pcm_buffer_size = decode_pcm_buffer_size;
    internal_handle->working_buff1_size = working_buff1_size;
    internal_handle->working_buff2_size = working_buff2_size;
    internal_handle->stream_out_pcm_buff_size = decode_pcm_buffer_size * 3;

    //get total memory size for mp3
    *memory_size = internal_handle->share_buff_size + internal_handle->decode_pcm_buffer_size + internal_handle->working_buff1_size + internal_handle->working_buff2_size + internal_handle->stream_out_pcm_buff_size;

#if 0
    LOGI("[MP3 Codec]internal_handle->share_buff_size=%u\n", internal_handle->share_buff_size);
    LOGI("[MP3 Codec]internal_handle->pcm_buff_size=%u\n", internal_handle->pcm_buff_size);
    LOGI("[MP3 Codec]internal_handle->working_buff1_size=%u\n", internal_handle->working_buff1_size);
    LOGI("[MP3 Codec]internal_handle->working_buff2_size=%u\n", internal_handle->working_buff2_size);
#endif

    return MP3_CODEC_RETURN_OK;

}
#endif


#if 1
#if defined(MTK_AUDIO_MIXER_SUPPORT)
mp3_codec_function_return_state_t mp3_codec_set_memory2(mp3_codec_media_handle_t *handle)
#else
mp3_codec_function_return_state_t mp3_codec_set_memory2(void)
#endif
{
#if defined(MTK_AUDIO_MIXER_SUPPORT)
    mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(handle);
#else
    mp3_codec_media_handle_t *handle = global_mp3_handle; // backward compatibility for "mp3_codec_set_memory2()"
    mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(global_mp3_handle);
#endif
    if (internal_handle == NULL) {
        LOGE("[MP3 Codec]mp3_codec_internal_handle = NULL\n");
        return MP3_CODEC_RETURN_ERROR;
    }

    if (handle->state != MP3_CODEC_STATE_READY) {
        return MP3_CODEC_RETURN_ERROR;
    }

    uint8_t *memory_base = NULL;
    int working_buff1_size,         /* the required Working buffer1 size in byte    */
        working_buff2_size,         /* the required Working buffer2 size in byte    */
        decode_pcm_buffer_size,              /* the required pcm buffer size in byte          */
        share_buff_size;            /* the share buffer size      */

    /*STEP 1 : Allocate data memory for MP3 decoder*/
    MP3Dec_GetMemSize(&share_buff_size, &decode_pcm_buffer_size, &working_buff1_size, &working_buff2_size);

#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    share_buff_size = share_buff_size + 4096; // = 8 KB = 8192 = 4096 + 4096
#else
    share_buff_size = share_buff_size + 1024;   // since we add share_buffer_data_amount <= SHARE_BUFFER_TOO_LESS_AMOUNT feature so add 1024 to compensate it.
#endif

    //4bytes aligned
    share_buff_size = (share_buff_size + 3) & ~0x3;
    decode_pcm_buffer_size = (decode_pcm_buffer_size + 3) & ~0x3;
    working_buff1_size = (working_buff1_size + 3) & ~0x3;
    working_buff2_size = (working_buff2_size + 3) & ~0x3;

    internal_handle->share_buff_size = share_buff_size;
    internal_handle->decode_pcm_buffer_size = decode_pcm_buffer_size;
    internal_handle->working_buff1_size = working_buff1_size;
    internal_handle->working_buff2_size = working_buff2_size;
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    internal_handle->stream_out_pcm_buff_size = decode_pcm_buffer_size * 3 * 2;
#else
    internal_handle->stream_out_pcm_buff_size = decode_pcm_buffer_size * 3;
#endif
    //specify memory pool
    mp3_handle_ptr_to_mp3_decode_buffer(handle) = (uint8_t *)pvPortMalloc(MP3_DECODE_BUFFER_SIZE * sizeof(uint8_t));
    if (NULL == mp3_handle_ptr_to_mp3_decode_buffer(handle)) {
        LOGE("[MP3 Codec] mp3_codec_set_memory2 fail: cannot allocate mp3 decode buffer\n");
        return MP3_CODEC_RETURN_ERROR;
    }
    internal_handle->memory_pool = memory_base = mp3_handle_ptr_to_mp3_decode_buffer(handle);

    //set share buffer
    mp3_codec_set_share_buffer(handle, memory_base, share_buff_size);
    memory_base += share_buff_size;

    // set decode_pcm_buffer
    internal_handle->decode_pcm_buff.buffer_base_pointer = memory_base;
    internal_handle->decode_pcm_buff.buffer_byte_count = internal_handle->decode_pcm_buffer_size;
    internal_handle->decode_pcm_buff.read_pointer = 0;
    internal_handle->decode_pcm_buff.write_pointer = 0;
    memory_base += internal_handle->decode_pcm_buffer_size;


    //set working buffer
    internal_handle->working_buff1 = memory_base;
    memory_base += internal_handle->working_buff1_size;
    internal_handle->working_buff2 = memory_base;
    memory_base += internal_handle->working_buff2_size;

    // set PCM buffer
    internal_handle->stream_out_pcm_buff.buffer_base_pointer = memory_base;
    internal_handle->stream_out_pcm_buff.buffer_byte_count = internal_handle->stream_out_pcm_buff_size;
    internal_handle->stream_out_pcm_buff.read_pointer = 0;
    internal_handle->stream_out_pcm_buff.write_pointer = 0;




    /*STEP 2 : Get MP3 Handler */
    internal_handle->mp3_handle = MP3Dec_Init(internal_handle->working_buff1, internal_handle->working_buff2);
    if (internal_handle->mp3_handle == NULL) {
        LOGE("[MP3 Codec]MP3Dec_Init fail");
        return MP3_CODEC_RETURN_ERROR;
    }

#if 0

    printf("share_buff_size = %d\r\n", share_buff_size);
    printf("decode_pcm_buffer_size = %d\r\n", decode_pcm_buffer_size);
    printf("working_buff1_size = %d\r\n", working_buff1_size);
    printf("working_buff2_size = %d\r\n", working_buff2_size);

    printf("internal_handle->share_buff_size = %d\r\n", internal_handle->share_buff_size);
    printf("internal_handle->decode_pcm_buffer_size = %d\r\n", internal_handle->decode_pcm_buffer_size);
    printf("internal_handle->working_buff1_size = %d\r\n", internal_handle->working_buff1_size);
    printf("internal_handle->working_buff2_size = %d\r\n", internal_handle->working_buff2_size);

    printf("internal_handle->stream_out_pcm_buff_size = %d\r\n", internal_handle->stream_out_pcm_buff_size);

#endif

#if 0
    LOGI("[MP3 Codec]set_memory memory range : start=%08x end=%08x",
                 &mp3_decode_buffer[0], &mp3_decode_buffer[40999]);
    LOGI("[MP3 Codec]set_memory share buffer : base=%08x size=%u",
                 handle->share_buff.buffer_base, (unsigned int)handle->share_buff.buffer_size);
    LOGI("[MP3 Codec]set_memory decode_pcm_buff.buffer : base=%08x size=%u",
                 internal_handle->decode_pcm_buff.buffer_base_pointer, (unsigned int)internal_handle->decode_pcm_buff.buffer_byte_count);
    LOGI("[MP3 Codec]set_memory work1 buffer : base=%08x size=%u",
                 internal_handle->working_buff1, (unsigned int)internal_handle->working_buff1_size);
    LOGI("[MP3 Codec]set_memory work2 buffer : base=%08x size=%u",
                 internal_handle->working_buff2, (unsigned int)internal_handle->working_buff2_size);
    LOGI("[MP3 Codec]set_memory stream_out_pcm_buff : base=%08x size=%u",
                 internal_handle->stream_out_pcm_buff.buffer_base_pointer, (unsigned int)internal_handle->stream_out_pcm_buff.buffer_byte_count);
#endif

    return MP3_CODEC_RETURN_OK;

}
#endif

static void mp3_codec_event_send_from_isr(mp3_codec_queue_event_id_t id, void *parameter)
{
    mp3_codec_queue_event_t event;
    event.id        = id;
    event.parameter = parameter;

    if (mp3_handle_ptr_to_queue_handle(parameter) != NULL) {
        //printf("queue stuck amount=%d\r\n", uxQueueMessagesWaitingFromISR(mp3_codec_queue_handle));
        if (xQueueSendFromISR(mp3_handle_ptr_to_queue_handle(parameter), &event, 0) != pdPASS) {
            LOGI("[MP3 Codec]queue not pass %d\r\n", id);
            return;
        }
    } else {
        MP3_DEBUG_PRINTF("mp3_codec_queue_handle NULL\r\n");
    }

    return;
}

static void mp3_codec_event_register_callback(mp3_codec_media_handle_t *handle, mp3_codec_queue_event_id_t reg_id, mp3_codec_internal_callback_t callback)
{
    uint32_t id_idx;
    for (id_idx = 0; id_idx < MAX_MP3_CODEC_FUNCTIONS; id_idx++) {
        if ((mp3_handle_ptr_to_queue_id_array(handle))[id_idx] == MP3_CODEC_QUEUE_EVENT_NONE) {
            (mp3_handle_ptr_to_queue_id_array(handle))[id_idx] = reg_id;
            (mp3_handle_ptr_to_queue_handler(handle))[id_idx] = callback;
            break;
        }
    }
    return;
}

static void mp3_codec_event_deregister_callback(mp3_codec_media_handle_t *handle, mp3_codec_queue_event_id_t dereg_id)
{
    uint32_t id_idx;
    for (id_idx = 0; id_idx < MAX_MP3_CODEC_FUNCTIONS; id_idx++) {
        if ((mp3_handle_ptr_to_queue_id_array(handle))[id_idx] == dereg_id) {
            (mp3_handle_ptr_to_queue_id_array(handle))[id_idx] = MP3_CODEC_QUEUE_EVENT_NONE;
            break;
        }
    }
    return;
}


static void mp3_codec_task_main(void *arg)
{
    LOGI("[MP3 Codec]mp3_codec_task_main create\n");

    mp3_codec_queue_event_t event;

    while (1) {
        if (xQueueReceive(mp3_handle_ptr_to_queue_handle(arg), &event, portMAX_DELAY)) {
            //LOGI("[MP3 Codec]xQueueReceive event\n");
            mp3_codec_queue_event_id_t rece_id = event.id;
            uint32_t id_idx;
            for (id_idx = 0; id_idx < MAX_MP3_CODEC_FUNCTIONS; id_idx++) {
                if ((mp3_handle_ptr_to_queue_id_array(arg))[id_idx] == rece_id) {
                    (mp3_handle_ptr_to_queue_handler(arg))[id_idx](event.parameter);
                    break;
                }
            }
        }
    }
}



static void mp3_codec_task_create(mp3_codec_media_handle_t *handle)
{
    if (mp3_handle_ptr_to_task_handle(handle) ==  NULL) {
        LOGI("[MP3 Codec] create mp3 task\r\n");
        xTaskCreate(mp3_codec_task_main, MP3_CODEC_TASK_NAME, MP3_CODEC_TASK_STACKSIZE / sizeof(StackType_t), handle, MP3_CODEC_TASK_PRIO, &(mp3_handle_ptr_to_task_handle(handle)));
    }
}


static hal_audio_channel_number_t mp3_codec_translate_decoder_ip_channel_number(uint16_t channel_number)
{
    hal_audio_channel_number_t hal_audio_channel_number = HAL_AUDIO_STEREO;

    hal_audio_channel_number = (channel_number == 1) ? HAL_AUDIO_MONO : HAL_AUDIO_STEREO;

    return hal_audio_channel_number;
}

static hal_audio_sampling_rate_t mp3_codec_translate_decoder_ip_sample_rate_index(uint16_t sample_rate_index)
{

    hal_audio_sampling_rate_t hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;

    switch (sample_rate_index) {
        case 0:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
            break;
        case 1:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
            break;
        case 2:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_32KHZ;
            break;
        case 3:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_22_05KHZ;
            break;
        case 4:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_24KHZ;
            break;
        case 5:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
            break;
        case 6:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_11_025KHZ;
            break;
        case 7:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_12KHZ;
            break;
        case 8:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_8KHZ;
            break;
    }
    return hal_audio_sampling_rate;
}

#if 0
static uint16_t mp3_codec_get_stream_out_slience_tone(void)
{
    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;
    uint16_t silence_tone = 0;

    uint8_t *out_buf_ptr    = NULL;
    uint32_t stream_out_pcm_buffer_data_count = 0;
    ring_buffer_get_read_information(&internal_handle->stream_out_pcm_buff, &out_buf_ptr, &stream_out_pcm_buffer_data_count);

    if ((out_buf_ptr - internal_handle->stream_out_pcm_buff.buffer_base_pointer) >= 2) {
        silence_tone = *((uint16_t *)(out_buf_ptr - 2));
    } else if (out_buf_ptr == internal_handle->stream_out_pcm_buff.buffer_base_pointer) {
        silence_tone = *((uint16_t *)(internal_handle->stream_out_pcm_buff.buffer_base_pointer + internal_handle->stream_out_pcm_buff.buffer_byte_count - 2));
    } else {
        silence_tone = (uint16_t)(*(internal_handle->stream_out_pcm_buff.buffer_base_pointer + internal_handle->stream_out_pcm_buff.buffer_byte_count - 1)) << 0;
        silence_tone = (uint16_t)(*(internal_handle->stream_out_pcm_buff.buffer_base_pointer)) << 8;
    }

    return silence_tone;
}

static void mp3_codec_put_silence_samples_to_stream_out_pcm_buffer(uint32_t sliences_sample_amount)
{
    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;

    uint16_t slience_tone = mp3_codec_get_stream_out_slience_tone();

    // TODO: we may get stereo silence, not only mono

    uint32_t loop_idx = 0;
    uint32_t decoded_data_count = sliences_sample_amount * 2 * internal_handle->mp3_handle->CHNumber; // word -> bytes

    decoded_data_count = (decoded_data_count > 0) ? decoded_data_count : (1152 * 2 * 2);    // 1152: mp3 one frame max samples; 2(stereo) * 2 (two bytes)

    for (loop_idx = 0; loop_idx < 2; loop_idx++) {
        uint8_t *out_buf_ptr    = NULL;
        uint32_t out_byte_count = 0;
        ring_buffer_get_write_information(&internal_handle->stream_out_pcm_buff, &out_buf_ptr, &out_byte_count);
        if (out_byte_count > 0) {
            uint32_t consumed_byte_count = MINIMUM(decoded_data_count, out_byte_count);
            uint32_t i = 0;
            for (i = 0; i < consumed_byte_count / 2; i += 2) {
                *((uint16_t *)(out_buf_ptr + i)) = slience_tone;
            }

            decoded_data_count  -= consumed_byte_count;
            ring_buffer_write_done(&internal_handle->stream_out_pcm_buff, consumed_byte_count);
        } else {
            // stream_out_pcm_buffer full, do nothing.
            MP3_DEBUG_PRINTF("mp3_codec_put_one_frame_silence_samples_to_stream_out_pcm_buffer: stream_out_pcm_buffer full\r\n");
            break;
        }
    }
}
#endif

static void mp3_codec_pcm_out_isr_callback(hal_audio_event_t event, void *data)
{
#if defined(MTK_AUDIO_MIXER_SUPPORT)
    mp3_codec_media_handle_t *handle = (mp3_codec_media_handle_t *)data;
    uint32_t total_stream_out_amount = 0;
    uint32_t current_cnt = 0;
#endif
    mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(data);
    uint32_t sample_count = 0;
    uint32_t stream_out_amount = 0;
    uint32_t loop_idx = 0;

    if (1 == internal_handle->media_bitstream_end) {
        LOGI("[MP3 Codec] isr just return becase media_bitstream_end == 1\r\n");
        mp3_codec_event_send_from_isr(MP3_CODEC_QUEUE_EVENT_DECODE, data); // "handle->handler(handle, MP3_CODEC_MEDIA_BITSTREAM_END)" is called in "mp3_codec_deocde_hisr_handler"
        return;
    }

    uint32_t one_frame_samples = internal_handle->mp3_handle->PCMSamplesPerCH * internal_handle->mp3_handle->CHNumber;
    one_frame_samples = (one_frame_samples > 0) ? one_frame_samples : (1152 * 2);    // 1152( mp3 one frame max samples) * 2(stereo)

    uint32_t stream_out_data_amount_bytes = ring_buffer_get_data_byte_count(&internal_handle->stream_out_pcm_buff);
    //printf("1curr_cnt=%d, stream_out_data_amount_bytes=%d, one_frame_bytes=%d\r\n", (curr_cnt/1000), stream_out_data_amount_bytes, one_frame_samples*2);

    if ((stream_out_data_amount_bytes / 2) < (one_frame_samples / 4)) {
        uint32_t curr_cnt = 0;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
        LOGI("[MP3 Codec] isr(%d), pcm data(%d) less 1/4 frame\r\n", (curr_cnt / 1000), stream_out_data_amount_bytes, (one_frame_samples * 2));
        //mp3_codec_put_silence_samples_to_stream_out_pcm_buffer(2 * one_frame_samples - (stream_out_data_amount_bytes / 2)); // 2(gave one more frame for buffer to wait decode) *
        //printf("curr_cnt=%d, before=%d, after=%d, perCH=%d, CHN=%d\r\n", (curr_cnt/1000), stream_out_data_amount_bytes, ring_buffer_get_data_byte_count(&internal_handle->stream_out_pcm_buff), internal_handle->mp3_handle->PCMSamplesPerCH, internal_handle->mp3_handle->CHNumber);
    }

    switch (event) {
        case HAL_AUDIO_EVENT_UNDERFLOW:
        case HAL_AUDIO_EVENT_DATA_REQUEST:
#if defined(MTK_AUDIO_MIXER_SUPPORT)
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_cnt);
#endif
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                uint8_t *out_buf_ptr    = NULL;
                uint32_t stream_out_pcm_buffer_data_count = 0;

#if defined(MTK_AUDIO_MIXER_SUPPORT)
                audio_mixer_query_free_count(handle->mixer_track_id, &sample_count);
#else
                hal_audio_get_stream_out_sample_count(&sample_count);
#endif
                ring_buffer_get_read_information(&internal_handle->stream_out_pcm_buff, &out_buf_ptr, &stream_out_pcm_buffer_data_count);

                if (stream_out_pcm_buffer_data_count > 0) {
                    stream_out_amount = MINIMUM(sample_count, stream_out_pcm_buffer_data_count);
#if defined(MTK_AUDIO_MIXER_SUPPORT)
                    audio_mixer_write_data(handle->mixer_track_id, out_buf_ptr, stream_out_amount);
                    total_stream_out_amount += stream_out_amount;
#else
                    hal_audio_write_stream_out(out_buf_ptr, stream_out_amount);
#endif

                    ring_buffer_read_done(&internal_handle->stream_out_pcm_buff, stream_out_amount);

                    if ((sample_count - stream_out_amount) == 0) { // if first time sample_count < stream_out_pcm_buffer_data_count, then this will avoid loop again
                        break;
                    }
                } else {    /* 16kHz speech Tx buffer is empty */
                    // stream_out_pcm_buff no data
                    uint32_t curr_cnt = 0;
                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
                    LOGI("[MP3 Codec] curr_cnt=%d, stream_out_pcm_buff no data. loop_idx=%d\r\n", (curr_cnt / 1000), loop_idx);
#if defined(MTK_AUDIO_MIXER_SUPPORT)
                    audio_mixer_write_data(handle->mixer_track_id, out_buf_ptr, 0);
#else
                    hal_audio_write_stream_out(out_buf_ptr, 0);     // must have this or the isr will not to request again
#endif
                    break;
                }

            }

#if defined(MTK_AUDIO_MIXER_SUPPORT)
/*            if (mp3_handle_ptr_to_pcm_isr_cnt(handle) > 0) {
                log_hal_info("[MP3 Codec] role%d, isr(%d), wdc(%d)\n", 
                    handle->mixer_track_role,
                    (current_cnt - mp3_handle_ptr_to_pcm_isr_cnt(handle)) / 1000, 
                    total_stream_out_amount);
            }*/
            mp3_handle_ptr_to_pcm_isr_cnt(handle) = current_cnt;
#endif

            mp3_codec_event_send_from_isr(MP3_CODEC_QUEUE_EVENT_DECODE, data);

            break;
        case HAL_AUDIO_EVENT_END:
            {
                uint32_t curr_cnt = 0;
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
                LOGI("[MP3 Codec] isr(%d), MP3_CODEC_MEDIA_BITSTREAM_END\r\n", (curr_cnt / 1000));
#if defined(MTK_AUDIO_MIXER_SUPPORT)
                LOGI("[MP3 Codec] isr(%d): handle->handler(handle, MP3_CODEC_MEDIA_BITSTREAM_END);\r\n", (curr_cnt / 1000));
#endif
            }
            internal_handle->media_bitstream_end = 1;

#if defined(MTK_AUDIO_MIXER_SUPPORT)
            handle->handler(handle, MP3_CODEC_MEDIA_BITSTREAM_END);
#endif
            break;

        case HAL_AUDIO_EVENT_ERROR:
        case HAL_AUDIO_EVENT_NONE:
        case HAL_AUDIO_EVENT_DATA_NOTIFICATION:
            LOGI("not here\r\n");
            break;
    }
}


#if 0
static void mp3_codec_fill_data_hisr_handler(void *data)
{
    LOGI("[MP3 Codec]mp3_codec_fill_data_hisr_handler\n");

    uint32_t sample_count = 0;
    uint32_t PCM_length = 0;
    uint32_t segment = 0;
    uint8_t *PCM_Src = NULL;
    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;
    PCM_Src = internal_handle->PCM_ring.buffer_base_pointer;
    PCMSmples = internal_handle->decHandle->PCMSamplesPerCH * internal_handle->decHandle->CHNumber * 2;
    LOGI("[MP3 Codec]PCMSmples=%u\n", PCMSmples);
    do {
        hal_audio_get_stream_out_sample_count(&sample_count);
        LOGI("[MP3 Codec]sample_count=%u\n", sample_count);
        segment = MINIMUM(PCM_length, sample_count);
        hal_audio_write_stream_out(PCM_Src, segment);
        PCM_length -= segment;
        PCM_Src += segment;
        //LOGI("[MP3 Codec]PCMSmples=%u\n",PCMSmples);
    } while (PCM_length != 0);

    //mp3_codec_event_send_from_isr(MP3_CODEC_QUEUE_EVENT_DECODE, NULL);
}
#endif

static int32_t MP3_MPEG1_AUDIO_FRAME_BIT_RATE[] = { // MPEG 1 Layer 3
    0,
    32000,
    40000,
    48000,
    56000,
    64000,
    80000,
    96000,
    112000,
    128000,
    160000,
    192000,
    224000,
    256000,
    320000,
    0,
};

static int32_t MP3_MPEG2_AUDIO_FRAME_BIT_RATE[] = { // MPEG 2 or MPEG 2.5 Layer 3
    0,
    8000,
    16000,
    24000,
    32000,
    40000,
    48000,
    56000,
    64000,
    80000,
    96000,
    112000,
    128000,
    144000,
    160000,
    0,
};

static int32_t MP3_MPEG1_AUDIO_SAMPLING_RATE[] = {44100, 48000, 32000, 0};
static int32_t MP3_MPEG2_AUDIO_SAMPLING_RATE[] = {22050, 24000, 16000, 0};
static int32_t MP3_MPEG25_AUDIO_SAMPLING_RATE[] = {11025, 12000, 8000, 0};

typedef enum {
    MP3_CODEC_RETURN_NEXT_FRAME_SIZE_INVALID_FRAME_SIZE = -2,
    MP3_CODEC_RETURN_NEXT_FRAME_SIZE_INVALID_SECOND_FRAME_HEDAER = -1,
    MP3_CODEC_RETURN_NEXT_FRAME_SIZE_OK = 0,
} mp3_codec_function_return_next_frame_size_t;

#define MPEG1 (3)
#define MPEG2 (2)
#define MPEG25 (0)
#define LAYER3 (1)

static int32_t mp3_codec_get_next_mp3_frame_size(uint8_t *mp3_header, uint8_t *buffer_base, int32_t buffer_size, int32_t *mp3_frame_size)
{
    uint32_t temp_mp3_header = 0;
    int32_t bitrate = 0;
    int32_t samplingRate = 0;
    int32_t paddingBit = 0;
    int32_t frameSize = 0;
    int32_t ret = MP3_CODEC_RETURN_NEXT_FRAME_SIZE_INVALID_FRAME_SIZE; // mp3_header does NOT match MP3 Header
    uint8_t *buffer_end = buffer_base + buffer_size - 1;
    int8_t mpegVer, layerVer;
    int32_t mpegFactor;

    *mp3_frame_size = 0;
    temp_mp3_header = *mp3_header;
    temp_mp3_header = (temp_mp3_header << 8) | (((mp3_header + 1) > (buffer_end))?*(buffer_base + (mp3_header + 1 - buffer_end - 1)):*(mp3_header + 1)); // *(mp3_header + 1);
    temp_mp3_header = (temp_mp3_header << 8) | (((mp3_header + 2) > (buffer_end))?*(buffer_base + (mp3_header + 2 - buffer_end - 1)):*(mp3_header + 2)); // *(mp3_header + 2);
    temp_mp3_header = (temp_mp3_header << 8) | (((mp3_header + 3) > (buffer_end))?*(buffer_base + (mp3_header + 3 - buffer_end - 1)):*(mp3_header + 3)); // *(mp3_header + 3);

    if (IS_MP3_HEAD(temp_mp3_header)) {
        layerVer = (temp_mp3_header >> 17) & 0x3;
        if (layerVer != LAYER3) {
            LOGI("[MP3 Codec] mp3_codec_get_next_mp3_frame_size: Cannot parse non-Layer 3!!!\n");
            return MP3_CODEC_RETURN_NEXT_FRAME_SIZE_INVALID_FRAME_SIZE;
        }
        mpegVer = (temp_mp3_header >> 19) & 0x3; 
        switch (mpegVer) {
            case MPEG1: // MPEG Version 1
                // LOGI("[MP3 Codec] MPEG1\n");
                bitrate = MP3_MPEG1_AUDIO_FRAME_BIT_RATE[((temp_mp3_header >> 12) & 0xF)];
                samplingRate = MP3_MPEG1_AUDIO_SAMPLING_RATE[(temp_mp3_header >> 10) & 0x3];
                mpegFactor = 144;
                break;
            case MPEG2: // MPEG Version 2
                // LOGI("[MP3 Codec] MPEG2\n");
                bitrate = MP3_MPEG2_AUDIO_FRAME_BIT_RATE[((temp_mp3_header >> 12) & 0xF)];
                samplingRate = MP3_MPEG2_AUDIO_SAMPLING_RATE[(temp_mp3_header >> 10) & 0x3];
                mpegFactor = 72;
                break;
            case MPEG25: // MPEG Version 2.5
                // LOGI("[MP3 Codec] MPEG25\n");
                bitrate = MP3_MPEG2_AUDIO_FRAME_BIT_RATE[((temp_mp3_header >> 12) & 0xF)];
                samplingRate = MP3_MPEG25_AUDIO_SAMPLING_RATE[(temp_mp3_header >> 10) & 0x3];
                mpegFactor = 72;
                break;
            default:
                return MP3_CODEC_RETURN_NEXT_FRAME_SIZE_INVALID_FRAME_SIZE;
        }

        paddingBit = (temp_mp3_header >> 9) & 0x1;
        frameSize = ((mpegFactor * bitrate) / samplingRate) + paddingBit;
        // LOGI("[MP3 Codec] frameSize = %d, bitrate = %d, samplingRate = %d, paddingBit = %d\n", frameSize, bitrate, samplingRate, paddingBit);
        if (frameSize > buffer_size) {
            *mp3_frame_size = 0;
            LOGI("[MP3 Codec] Set frameSize to 0 because frameSize(%d) > buffer_size(%d)\n", frameSize, buffer_size);
        }
        // Check the next frame
        mp3_header += frameSize;
        temp_mp3_header = (mp3_header > buffer_end)?*(buffer_base + (mp3_header - buffer_end - 1)):*mp3_header; // *mp3_header;
        temp_mp3_header = (temp_mp3_header << 8) | (((mp3_header + 1) > buffer_end)?*(buffer_base + (mp3_header + 1 - buffer_end - 1)):*(mp3_header + 1)); // *(mp3_header + 1);
        temp_mp3_header = (temp_mp3_header << 8) | (((mp3_header + 2) > buffer_end)?*(buffer_base + (mp3_header + 2 - buffer_end - 1)):*(mp3_header + 2)); // *(mp3_header + 2);
        temp_mp3_header = (temp_mp3_header << 8) | (((mp3_header + 3) > buffer_end)?*(buffer_base + (mp3_header + 3 - buffer_end - 1)):*(mp3_header + 3)); // *(mp3_header + 3);

        // LOGI("[MP3 Codec] The next frame header is 0x%04X\n", temp_mp3_header);
        *mp3_frame_size = frameSize;
        if (IS_MP3_HEAD(temp_mp3_header)) {
            // LOGI("[MP3 Codec] bitrate = %d, samplingRate = %d, paddingBit = %d, frameSize = %d\n", bitrate, samplingRate, paddingBit, frameSize);
            ret = MP3_CODEC_RETURN_NEXT_FRAME_SIZE_OK; // the next two frame is available
        } else {
            ret = MP3_CODEC_RETURN_NEXT_FRAME_SIZE_INVALID_SECOND_FRAME_HEDAER; // the next two frame is NOT available
        }
    }

    return ret;
}

static void mp3_codec_deocde_hisr_handler(void *data)
{
    int32_t consumeBS = -1;
    uint8_t *share_buff_read_ptr = NULL;

    mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(data);
    mp3_codec_media_handle_t *handle = &internal_handle->handle;


    uint32_t curr_cnt = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);

    if (1 == internal_handle->media_bitstream_end) {
        LOGI("[MP3 Codec] hisr(%d): handle->handler(handle, MP3_CODEC_MEDIA_BITSTREAM_END);\r\n", (curr_cnt / 1000));
        handle->handler(handle, MP3_CODEC_MEDIA_BITSTREAM_END);
        return;
    }

    uint32_t one_frame_bytes = internal_handle->mp3_handle->PCMSamplesPerCH * internal_handle->mp3_handle->CHNumber * 2;
    one_frame_bytes = (one_frame_bytes > 0) ? one_frame_bytes : (1152 * 2 * 2);    // 1152( mp3 one frame max samples) * 2(stereo) * (word -> bytes)

    if (ring_buffer_get_space_byte_count(&internal_handle->stream_out_pcm_buff) < one_frame_bytes) {
        // if streamout pcm data > decode_pcm_buffer_size just return , or it will throw some data
        LOGI("[MP3 Codec] hisr(%d): pcm space(%d) < one_frame_bytes(%d), just return\r\n", (curr_cnt / 1000), ring_buffer_get_space_byte_count(&internal_handle->stream_out_pcm_buff), one_frame_bytes);
        return;
    }


    int32_t share_buffer_data_amount = mp3_codec_get_share_buffer_data_count(handle);
    
    if ((1 == handle->flush_data_flag) && (share_buffer_data_amount <= MPEG_AUDIO_FRAME_HEADER_LENGTH)) {
        uint8_t *out_buf_ptr    = NULL;
        uint32_t stream_out_pcm_buffer_data_count = 0;
        ring_buffer_get_read_information(&internal_handle->stream_out_pcm_buff, &out_buf_ptr, &stream_out_pcm_buffer_data_count);

        if (0 == stream_out_pcm_buffer_data_count) {
#if defined(MTK_AUDIO_MIXER_SUPPORT)
            LOGI("[MP3 Codec] hisr(%d): role%d: audio_mixer_set_eof()\r\n", (curr_cnt / 1000), handle->mixer_track_role);
            audio_mixer_set_eof(handle->mixer_track_id);
#else
            LOGI("[MP3 Codec] hisr(%d): audio_pcm_set_eof()\r\n", (curr_cnt / 1000));
            audio_pcm_set_eof();
#endif
        }
    }

    if ((1 == handle->waiting) // wait data from caller
        && (ring_buffer_get_data_byte_count(&internal_handle->stream_out_pcm_buff) > (10240)) // PCM data is enough (> 10KB)
        && (mp3_codec_get_share_buffer_data_count(handle) < (handle->share_buff.buffer_size / 2))) { // Share buffer is not enough (< 1/2 size)

        LOGI("[MP3 Codec] hisr(%d): wait data just return\r\n", (curr_cnt / 1000));
        return;
    }

    //update read ptr to share buffer
    share_buff_read_ptr = handle->share_buff.buffer_base + handle->share_buff.read;

    int32_t mp3_frame_size = 0;
    int32_t check_min_share_size = 0;
    if ((handle->flush_data_flag == 1) || (handle->linear_buff == 1)) {
        check_min_share_size = MPEG_AUDIO_FRAME_HEADER_LENGTH;
    } else {
        if (share_buffer_data_amount >= MPEG_AUDIO_FRAME_HEADER_LENGTH) {
            mp3_codec_get_next_mp3_frame_size(share_buff_read_ptr, 
                                                handle->share_buff.buffer_base,
                                                handle->share_buff.buffer_size, 
                                                &mp3_frame_size);
        }
        mp3_frame_size = (mp3_frame_size > 0)?mp3_frame_size:mp3_handle_ptr_to_previous_mp3_frame_size(data);
        mp3_handle_ptr_to_previous_mp3_frame_size(data) = mp3_frame_size;
        check_min_share_size = mp3_frame_size;
    }

    if (share_buffer_data_amount < (check_min_share_size + 1)) {
        mp3_codec_request_data_to_share_buffer(handle);
        return;
    }

    //taskENTER_CRITICAL();
    // calculate mcps
    //CPU_RESET_CYCLECOUNTER;
    //__asm volatile("nop");
    //count_test = *DWT_CYCCNT;
    //offset = count_test - 1;
    //CPU_RESET_CYCLECOUNTER;
    uint8_t one_mp3_silence_frame[MPEG_AUDIO_FRAME_HEADER_LENGTH + MP3_SILENCE_FRAME_PATTERN_LENGTH] = {0};
    uint32_t i = 0;
    for (i = 0; i < MPEG_AUDIO_FRAME_HEADER_LENGTH + MP3_SILENCE_FRAME_PATTERN_LENGTH; i++) {
        if ((handle->share_buff.read + i) >= handle->share_buff.buffer_size) {
            one_mp3_silence_frame[i] = *(share_buff_read_ptr + i - handle->share_buff.buffer_size);
        } else {
            one_mp3_silence_frame[i] = *(share_buff_read_ptr + i);
        }
    }
    one_mp3_silence_frame[MPEG_AUDIO_FRAME_HEADER_LENGTH + MP3_SILENCE_FRAME_PATTERN_LENGTH - 1] = 0;
    if (0 == strncmp((const char *)(one_mp3_silence_frame + MPEG_AUDIO_FRAME_HEADER_LENGTH), mp3_silence_frame_pattern, sizeof(mp3_silence_frame_pattern) / sizeof(char))) {
        uint32_t decoded_data_count = internal_handle->mp3_handle->PCMSamplesPerCH * internal_handle->mp3_handle->CHNumber * 2;
        memset(internal_handle->decode_pcm_buff.buffer_base_pointer, 0, decoded_data_count);
        mp3_codec_get_silence_frame_information(handle, &consumeBS);
    } else {
    /*   Deocde mp3 frame
        *   return: The consumed data size of Bitsream buffer for this  frame
      */
    consumeBS = MP3Dec_Decode(internal_handle->mp3_handle,
                              internal_handle->decode_pcm_buff.buffer_base_pointer,
                              handle->share_buff.buffer_base,
                              handle->share_buff.buffer_size,
                              share_buff_read_ptr);
    }

    //count_test = *DWT_CYCCNT - offset;
    //LOGI("%d\n", count_test);
    //taskEXIT_CRITICAL();

    int32_t share_data_amount = mp3_codec_get_share_buffer_data_count(handle);
    //printf("hisr:consumeBS=%d, buffer_size=%d, buff.read=%d, buff.write=%d, share_data_amount=%d, pcm_data_amount=%d, one_frame_bytes=%d\r\n", consumeBS, handle->share_buff.buffer_size, handle->share_buff.read, handle->share_buff.write, share_data_amount, ring_buffer_get_data_byte_count(&internal_handle->stream_out_pcm_buff), one_frame_bytes);


    bool decode_abnormal = false;
    
    if ((consumeBS != mp3_frame_size) || (consumeBS < MINIMUM_MP3_FRAME_SIZE) || (consumeBS > MAXIMUM_MP3_FRAME_SIZE)) {
#if defined(MTK_AUDIO_MIXER_SUPPORT)
        LOGI("hisr(%d): role%d: mp3 frame[ 0] = 0x%02X %02X %02X %02X\r\n",
            (curr_cnt / 1000), handle->mixer_track_role,
            one_mp3_silence_frame[0], one_mp3_silence_frame[1], one_mp3_silence_frame[2], one_mp3_silence_frame[3]);
        LOGI("hisr(%d): role%d: mp3 frame[ 5] = 0x%02X %02X %02X %02X\r\n",
            (curr_cnt / 1000), handle->mixer_track_role,
            one_mp3_silence_frame[4], one_mp3_silence_frame[5], one_mp3_silence_frame[6], one_mp3_silence_frame[7]);
        LOGI("hisr(%d): role%d: mp3 frame[ 8] = 0x%02X %02X %02X %02X\r\n",
            (curr_cnt / 1000), handle->mixer_track_role,
            one_mp3_silence_frame[8], one_mp3_silence_frame[9], one_mp3_silence_frame[10], one_mp3_silence_frame[11]);
        LOGI("hisr(%d): role%d: mp3 frame[12] = 0x%02X %02X %02X %02X\r\n",
            (curr_cnt / 1000), handle->mixer_track_role,
            one_mp3_silence_frame[12], one_mp3_silence_frame[13], one_mp3_silence_frame[14], one_mp3_silence_frame[15]);
        LOGI("[MP3 Codec] hisr(%d): role%d: consumeBS = %d, mp3_frame_size = %d, share_buff.read = %d, share_buff.write = %d, share_buff.buffer_size = %d\r\n", (curr_cnt / 1000), handle->mixer_track_role, (int)consumeBS, mp3_frame_size, handle->share_buff.read, handle->share_buff.write, handle->share_buff.buffer_size);
        LOGI("[MP3 Codec] hisr(%d): role%d: flush_data_flag = %d, linear_buff = %d\r\n", (curr_cnt / 1000), handle->mixer_track_role, handle->flush_data_flag, handle->linear_buff);
#else
        LOGI("hisr(%d): mp3 frame[ 0] = 0x%02X %02X %02X %02X\r\n",
            (curr_cnt / 1000),
            one_mp3_silence_frame[0], one_mp3_silence_frame[1], one_mp3_silence_frame[2], one_mp3_silence_frame[3]);
        LOGI("hisr(%d): mp3 frame[ 5] = 0x%02X %02X %02X %02X\r\n",
            (curr_cnt / 1000),
            one_mp3_silence_frame[4], one_mp3_silence_frame[5], one_mp3_silence_frame[6], one_mp3_silence_frame[7]);
        LOGI("hisr(%d): mp3 frame[ 8] = 0x%02X %02X %02X %02X\r\n",
            (curr_cnt / 1000),
            one_mp3_silence_frame[8], one_mp3_silence_frame[9], one_mp3_silence_frame[10], one_mp3_silence_frame[11]);
        LOGI("hisr(%d): mp3 frame[12] = 0x%02X %02X %02X %02X\r\n",
            (curr_cnt / 1000),
            one_mp3_silence_frame[12], one_mp3_silence_frame[13], one_mp3_silence_frame[14], one_mp3_silence_frame[15]);
        LOGI("[MP3 Codec] hisr(%d): consumeBS = %d, mp3_frame_size = %d, share_buff.read = %d, share_buff.write = %d, share_buff.buffer_size = %d\r\n", (curr_cnt / 1000), (int)consumeBS, mp3_frame_size, handle->share_buff.read, handle->share_buff.write, handle->share_buff.buffer_size);
        LOGI("[MP3 Codec] hisr(%d): flush_data_flag = %d, linear_buff = %d\r\n", (curr_cnt / 1000), handle->flush_data_flag, handle->linear_buff);
#endif // #if defined(MTK_AUDIO_MIXER_SUPPORT)
    }

    if ((consumeBS != mp3_frame_size) && (handle->flush_data_flag != 1) && (handle->linear_buff == 0)) {
        LOGI("[MP3 Codec] consumeBS != mp3_frame_size: consumeBS = %d, mp3_frame_size = %d\n", (int)consumeBS, mp3_frame_size);
    }
    
    if (consumeBS < 0) {
        LOGI("[MP3 Codec] hisr(%d): Invalid return , consumeBS= %d\n", (curr_cnt / 1000), (int)consumeBS);
        //todo, error frame handle
    } else if (consumeBS >= 0 && consumeBS <= share_data_amount) {
        //update read index to share buffer
        handle->share_buff.read += consumeBS;
        if (handle->share_buff.read >= handle->share_buff.buffer_size) {    // although share buffer is a ring buffer, the mp3 decoder ip only code to the end of buffer at most.
            if (handle->linear_buff == 1) {
                handle->share_buff.read = handle->share_buff.buffer_size;
            } else {
                handle->share_buff.read -= handle->share_buff.buffer_size;
            }
        }

        if (consumeBS == 1) {
#if defined(MTK_AUDIO_MIXER_SUPPORT)
            LOGI("[MP3 Codec] hisr(%d): role%d: decode abnormal. consumeBS =1, handle->share_buff.read = %d, handle->share_buff.write = %d, handle->share_buff.buffer_size = %d\r\n", (curr_cnt / 1000), handle->mixer_track_role, handle->share_buff.read, handle->share_buff.write, handle->share_buff.buffer_size);
#else
            LOGI("[MP3 Codec] hisr(%d): decode abnormal. consumeBS =1, handle->share_buff.read = %d, handle->share_buff.write = %d, handle->share_buff.buffer_size = %d\r\n", (curr_cnt / 1000), handle->share_buff.read, handle->share_buff.write, handle->share_buff.buffer_size);
#endif
            decode_abnormal = true;
        }

    } else if (consumeBS > share_data_amount) {
        consumeBS = share_data_amount;  // It strange here, in fact, mp3 SWIP consumBS must not > share_data_amount. we met consumBS > share_data_amount in playing combine two songs in one mp3 file, so just workaround here.
        handle->share_buff.read += consumeBS;
        if (handle->share_buff.read >= handle->share_buff.buffer_size) {    // although share buffer is a ring buffer, the mp3 decoder ip only code to the end of buffer at most.
            if (handle->linear_buff == 1) {
                handle->share_buff.read = handle->share_buff.buffer_size;
            } else {
                handle->share_buff.read -= handle->share_buff.buffer_size;
            }
        }

#if defined(MTK_AUDIO_MIXER_SUPPORT)
        LOGI("[MP3 Codec] hisr(%d): role%d, decode abnormal. consumeBS > share_data_amount\r\n", (curr_cnt / 1000), handle->mixer_track_role);
#else
        LOGI("[MP3 Codec] hisr(%d): decode abnormal. consumeBS > share_data_amount\r\n", (curr_cnt / 1000));
#endif
        decode_abnormal = true;
    } else {
        LOGI("[MP3 Codec] hisr(%d): never hear\n", (curr_cnt / 1000));
    }

    if (decode_abnormal == false) {
        if ((internal_handle->mp3_handle->CHNumber != -1) && (internal_handle->mp3_handle->sampleRateIndex != -1)) {
            uint32_t loop_idx = 0;
            uint32_t decoded_data_count = internal_handle->mp3_handle->PCMSamplesPerCH * internal_handle->mp3_handle->CHNumber * 2;
            uint32_t decoded_data_index = 0;
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                uint8_t *out_buf_ptr    = NULL;
                uint32_t out_byte_count = 0;
                ring_buffer_get_write_information(&internal_handle->stream_out_pcm_buff, &out_buf_ptr, &out_byte_count);
                if (out_byte_count > 0) {
                    uint32_t consumed_byte_count = MINIMUM(decoded_data_count, out_byte_count);
                    uint8_t *p_in_base         = internal_handle->decode_pcm_buff.buffer_base_pointer;
                    uint8_t *p_in_buf          = p_in_base + decoded_data_index;
                    memcpy(out_buf_ptr, p_in_buf, consumed_byte_count);
                    decoded_data_index += consumed_byte_count;
                    decoded_data_count  -= consumed_byte_count;
                    ring_buffer_write_done(&internal_handle->stream_out_pcm_buff, consumed_byte_count);
                } else {
                    // stream_out_pcm_buffer full, do nothing.
                    MP3_DEBUG_PRINTF("[MP3 Codec] hisr(%d): stream_out_pcm_buffer full remain decoded_data_count=%d, stream out pcm data=%d, share buffer data=%d\r\n", (curr_cnt / 1000), decoded_data_count, ring_buffer_get_data_byte_count(&internal_handle->stream_out_pcm_buff), mp3_codec_get_share_buffer_data_count(handle));
                    break;
                }

            }
        }
    }

    if (mp3_codec_get_share_buffer_data_count(handle) <= (handle->share_buff.buffer_size / 2)) {    // share buufer less than decode bs * 3, we request user to fill data
        mp3_codec_request_data_to_share_buffer(handle);
    }

    if (ring_buffer_get_space_byte_count(&internal_handle->stream_out_pcm_buff) >= one_frame_bytes) {
        mp3_codec_event_send_from_isr(MP3_CODEC_QUEUE_EVENT_DECODE, data);
    }

}




static mp3_codec_function_return_state_t mp3_codec_skip_id3v2_and_reach_next_frame(mp3_codec_media_handle_t *handle, uint32_t file_size)
{
    uint32_t auido_frame_header = 0;
    if (mp3_codec_get_share_buffer_data_count(handle) < ID3V2_HEADER_LENGTH) {
        return MP3_CODEC_RETURN_ERROR;
    }

    mp3_codec_get_id3v2_info_and_skip(handle, file_size);

    if (mp3_codec_reach_next_frame_and_get_audio_frame_header(handle, &auido_frame_header, 2048, 0) != MP3_CODEC_RETURN_OK) {
        return MP3_CODEC_RETURN_ERROR;
    }

    return MP3_CODEC_RETURN_OK;
}

static mp3_codec_function_return_state_t mp3_codec_play_internal(mp3_codec_media_handle_t *handle)
{
    int32_t consumeBS = -1;
    uint8_t *share_buff_read_ptr = NULL;
    int16_t max_loop_times = 20;    // arbitrarily selected
#ifndef MTK_AUDIO_MIXER_SUPPORT
    uint32_t memory_size = 0;
#endif

    LOGI("[MP3 Codec] mp3_codec_play_internal ++\n");

/* temp sol. to protect play/resume flow  */
#if defined(MTK_AUDIO_MIXER_SUPPORT)
    if( handle->mixer_track_role == AUDIO_MIXER_TRACK_ROLE_MAIN ) {
        prompt_control_stop_tone();
    }
#endif

    mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(handle);


    mp3_codec_event_register_callback(handle, MP3_CODEC_QUEUE_EVENT_DECODE, mp3_codec_deocde_hisr_handler);

    //lock sleep
    //audio_service_setflag(handle->audio_id);


    do {
        //update read ptr to share buffer
        share_buff_read_ptr = handle->share_buff.buffer_base + handle->share_buff.read;


        consumeBS = MP3Dec_Decode(internal_handle->mp3_handle,
                                  internal_handle->decode_pcm_buff.buffer_base_pointer,
                                  handle->share_buff.buffer_base,
                                  handle->share_buff.buffer_size,
                                  share_buff_read_ptr);

        int32_t share_data_amount = mp3_codec_get_share_buffer_data_count(handle);
        if (consumeBS < 0) {
            LOGI("[MP3 Codec] Invalid return , consumeBS= %d\n", (int)consumeBS);
        } else if ((consumeBS > 1) && (consumeBS < share_data_amount) && (MP3SilenceFrameSize <= 0)) {        
            MP3SilenceFrameSize = consumeBS;
            MP3SilenceFrameHeader[0] = share_buff_read_ptr[0];
            MP3SilenceFrameHeader[1] = share_buff_read_ptr[1];
            MP3SilenceFrameHeader[2] = share_buff_read_ptr[2];
            MP3SilenceFrameHeader[3] = share_buff_read_ptr[3];

            LOGI("[MP3 Codec] MP3SilenceFrameSize = %d, MP3SilenceFrameHeader = 0x%02X %02X %02X %02X \n", MP3SilenceFrameSize,
                MP3SilenceFrameHeader[0], MP3SilenceFrameHeader[1], MP3SilenceFrameHeader[2], MP3SilenceFrameHeader[3]);
        }

        // TODO: maybe we can break do while when meet consumeBS=1 or consumeBS > share buffer data

        if ((internal_handle->mp3_handle->CHNumber != -1) && (internal_handle->mp3_handle->sampleRateIndex != -1)) {
            {
                //update read index to share buffer
                handle->share_buff.read += consumeBS;
                if (handle->share_buff.read >= handle->share_buff.buffer_size) {
                    handle->share_buff.read -= handle->share_buff.buffer_size;
                }
            }


            uint32_t loop_idx = 0;
            uint32_t decoded_data_count = internal_handle->mp3_handle->PCMSamplesPerCH * internal_handle->mp3_handle->CHNumber * 2;
            uint32_t decoded_data_index = 0;

            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                uint8_t *out_buf_ptr    = NULL;
                uint32_t out_byte_count = 0;
                ring_buffer_get_write_information(&internal_handle->stream_out_pcm_buff, &out_buf_ptr, &out_byte_count);
                if (out_byte_count > 0) {
                    uint32_t consumed_byte_count = MINIMUM(decoded_data_count, out_byte_count);
                    uint8_t *p_in_base         = internal_handle->decode_pcm_buff.buffer_base_pointer;
                    uint8_t *p_in_buf          = p_in_base + decoded_data_index;
                    memcpy(out_buf_ptr, p_in_buf, consumed_byte_count);
                    decoded_data_index += consumed_byte_count;
                    decoded_data_count  -= consumed_byte_count;
                    ring_buffer_write_done(&internal_handle->stream_out_pcm_buff, consumed_byte_count);
                } else {
                    // stream_out_pcm_buffer full, do nothing.
                    break;
                }

            }
        }

        max_loop_times--;

        if (max_loop_times < 0) {
            LOGI("[MP3 Codec] mp3_codec_play_internal: decode mp3 data, but fail, please check the file\n");
            return MP3_CODEC_RETURN_ERROR;
        }
    } while (ring_buffer_get_data_byte_count(&internal_handle->stream_out_pcm_buff) <= 1152 * 2 * 3);   // one frame max samples * stereo * 3(for buffer three frames)

    LOGI("[MP3 Codec] play internal, share buffer data=%d, stream out pcm data=%d\r\n", mp3_codec_get_share_buffer_data_count(handle), ring_buffer_get_data_byte_count(&internal_handle->stream_out_pcm_buff));
    mp3_codec_request_data_to_share_buffer(handle); // since we decoded some data, thus we request again to fill share buffer.
#if defined(MTK_AUDIO_MIXER_SUPPORT)
    handle->mixer_track_id = audio_mixer_get_track_id(mp3_codec_translate_decoder_ip_sample_rate_index((uint16_t)internal_handle->mp3_handle->sampleRateIndex),
                                                      mp3_codec_translate_decoder_ip_channel_number(internal_handle->mp3_handle->CHNumber),
                                                      mp3_codec_pcm_out_isr_callback,
                                                      handle,
                                                      handle->mixer_track_role);

    // Check ID
    if (handle->mixer_track_id < 0) {
        log_hal_info("[MP3 Codec] mp3_codec_play_internal: invalid track ID %d !!! \n", handle->mixer_track_id);
        return MP3_CODEC_RETURN_ERROR;
    }

    audio_mixer_query_free_count(handle->mixer_track_id, &mp3_handle_ptr_to_total_sample_count(handle));
    audio_mixer_status_t audio_mixer_status;
    mp3_handle_ptr_to_pcm_isr_cnt(handle) = 0;
    mp3_handle_ptr_to_is_bitstream_end_called(handle) = 0;
    audio_mixer_status = audio_mixer_start_track(handle->mixer_track_id);
    LOGI("[MP3 Codec] start mixer: %d, handle(%X) role%d\n", audio_mixer_status, handle, handle->mixer_track_role);
    if (AUDIO_MIXER_STATUS_OK != audio_mixer_status) {
        return MP3_CODEC_RETURN_ERROR;
    }
#else
    mp3_codec_event_send_from_isr(MP3_CODEC_QUEUE_EVENT_DECODE, handle);  // triger here, or if pcm buffer is too large, it will put all too dsp at first dsp interrupt, than it will has no pcm data at second dsp interrupt.
    hal_audio_set_stream_out_sampling_rate(mp3_codec_translate_decoder_ip_sample_rate_index((uint16_t)internal_handle->mp3_handle->sampleRateIndex));
    hal_audio_set_stream_out_channel_number(mp3_codec_translate_decoder_ip_channel_number(internal_handle->mp3_handle->CHNumber));

    hal_audio_register_stream_out_callback(mp3_codec_pcm_out_isr_callback, handle);
    hal_audio_get_memory_size(&memory_size);
    LOGI("[MP3 Codec] pvPortMalloc(%d) +\n", memory_size * sizeof(uint8_t));
    mp3_handle_ptr_to_audio_internal_ring_buffer(handle) = (uint8_t *)pvPortMalloc(memory_size * sizeof(uint8_t)); // 8 byte alignment
    LOGI("[MP3 Codec] pvPortMalloc(%d) -, audio_internal_ring_buffer = 0x%X\n", memory_size * sizeof(uint8_t), mp3_handle_ptr_to_audio_internal_ring_buffer(handle));
    if(NULL == mp3_handle_ptr_to_audio_internal_ring_buffer(handle)) {
        LOGI("[MP3 Codec] mp3_codec_play_internal: pvPortMalloc(%d) failed!!! \n", memory_size * sizeof(uint8_t));
        return MP3_CODEC_RETURN_ERROR;
    }
    hal_audio_set_memory(mp3_handle_ptr_to_audio_internal_ring_buffer(handle));
    hal_audio_get_stream_out_sample_count(&mp3_handle_ptr_to_total_sample_count(handle));
    // We don't put data (using hal_audio_write_stream_out) first, since we want  stream_out_pcm_buff have maximum amount of data
    hal_audio_start_stream_out(HAL_AUDIO_PLAYBACK_MUSIC);
#endif
    handle->flush_data_flag = 0;
    handle->state = MP3_CODEC_STATE_PLAY;


#ifdef MTK_BT_AWS_ENABLE
    if (internal_handle->aws_flag) {
        LOGI("[MP3 Codec] AWS is setted, hold playing until mp3_codec_aws_set_initial_sync\n");
    }
#endif


    LOGI("[MP3 Codec] mp3_codec_play_internal --\n");
    return MP3_CODEC_RETURN_OK;
}


#ifdef MTK_BT_AWS_ENABLE

static void mp3_codec_aws_play_setting(mp3_codec_internal_handle_t *p_info)
{
    //p_info->accumulated_sample_count = 0;
    //p_info->aws_internal_flag = true;
    audio_service_aws_set_flag(true);
    //{   /* PCM route setting */
    //    uint16_t mask    = AUDIO_DSP_POST_PROCESSING_ENABLE_MASK | AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_MASK;
    //    uint16_t control = AUDIO_DSP_POST_PROCESSING_ENABLE_ON   | AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_ON;
    //    audio_service_set_post_process_control(mask, control);
    //    audio_service_hook_isr(DSP_D2M_PCM_ROUTE_INT, bt_codec_a2dp_aws_pcm_route_isr, (void *)p_info);
    //}
    return;
}

static void mp3_codec_aws_stop_setting(mp3_codec_internal_handle_t *p_info)
{
    //{   /* PCM route setting */
    //    uint16_t mask    = AUDIO_DSP_POST_PROCESSING_ENABLE_MASK | AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_MASK;
    //    uint16_t control = AUDIO_DSP_POST_PROCESSING_ENABLE_OFF  | AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_OFF;
    //    audio_service_unhook_isr(DSP_D2M_PCM_ROUTE_INT);
    //    audio_service_set_post_process_control(mask, control);
    //}
    audio_service_aws_set_flag(false);
    //p_info->aws_internal_flag = false;
    return;
}
#endif  /* defined(MTK_AUDIO_MP3_CODEC_AWS_SUPPORT) */




mp3_codec_function_return_state_t mp3_codec_aws_set_flag(mp3_codec_media_handle_t *handle, bool flag)
{
#ifdef MTK_BT_AWS_ENABLE
    mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(handle);
    internal_handle->aws_flag = flag;

    if (internal_handle->aws_flag) {
        LOGI("[MP3 Codec] enable mp3 aws feature\n");
        mp3_codec_aws_play_setting(internal_handle);
    } else {
        mp3_codec_aws_stop_setting(internal_handle);
        LOGI("[MP3 Codec] stop mp3 aws feature\n");
    }

    return MP3_CODEC_RETURN_OK;
#else
    LOGI("[MP3 Codec]Not support mp3 codec aws\n");
    return MP3_CODEC_RETURN_ERROR;
#endif
}

mp3_codec_function_return_state_t mp3_codec_aws_set_initial_sync(mp3_codec_media_handle_t *handle)

{
#ifdef MTK_BT_AWS_ENABLE
    mp3_codec_function_return_state_t result = MP3_CODEC_RETURN_ERROR;
    
    if (mp3_handle_ptr_to_internal_ptr(handle) != NULL) {
        mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(handle);

        if ((handle != NULL) && (internal_handle != NULL)) {
            if (internal_handle->aws_flag) {
                audio_service_aws_set_initial_sync();
                result = MP3_CODEC_RETURN_OK;
            }
        }
    }
    return result;
#else
    LOGI("[MP3 Codec]Not support mp3 codec aws\n");
    return MP3_CODEC_RETURN_ERROR;
#endif
}


static mp3_codec_function_return_state_t mp3_codec_resume(mp3_codec_media_handle_t *handle)
{    
    LOGI("[MP3 Codec] resume ++\r\n");
    audio_codec_mutex_lock();
    mp3_codec_mutex_lock(handle);
    if (handle->state != MP3_CODEC_STATE_PAUSE) {
#if defined(MTK_AUDIO_MIXER_SUPPORT)
        LOGI("[MP3 Codec] resume --: handle(%X) role%d, cannot resume because state(%d)\r\n", handle, handle->mixer_track_role, handle->state);
#else
        LOGI("[MP3 Codec] resume --: cannot resume because state(%d)\r\n", handle->state);
#endif
        mp3_codec_mutex_unlock(handle);        
        audio_codec_mutex_unlock();
        return MP3_CODEC_RETURN_ERROR;
    }

    LOGI("[MP3 Codec] resume: stream out pcm data=%d, share buffer data=%d\r\n", ring_buffer_get_data_byte_count(&(mp3_handle_ptr_to_internal_ptr(handle))->stream_out_pcm_buff), mp3_codec_get_share_buffer_data_count(handle));

#if defined(MTK_AUDIO_MIXER_SUPPORT)
    if( handle->mixer_track_role == AUDIO_MIXER_TRACK_ROLE_MAIN ) { /* temp sol. to protect play/resume flow  */
        prompt_control_stop_tone();
    }

    mp3_handle_ptr_to_pcm_isr_cnt(handle) = 0;
    
    audio_mixer_status_t audio_mixer_status;
    audio_mixer_status = audio_mixer_start_track(handle->mixer_track_id);
    if (AUDIO_MIXER_STATUS_OK != audio_mixer_status) {
        mp3_codec_mutex_unlock(handle);    
        audio_codec_mutex_unlock();
        return MP3_CODEC_RETURN_ERROR;
    }
#else
    hal_audio_start_stream_out(HAL_AUDIO_PLAYBACK_MUSIC);
#endif
    LOGI("[MP3 Codec] resume: stream out pcm data=%d, share buffer data=%d\r\n", ring_buffer_get_data_byte_count(&(mp3_handle_ptr_to_internal_ptr(handle))->stream_out_pcm_buff), mp3_codec_get_share_buffer_data_count(handle));

    handle->flush_data_flag = 0;
    handle->state = MP3_CODEC_STATE_PLAY;
    mp3_codec_mutex_unlock(handle);    
    audio_codec_mutex_unlock();
    LOGI("[MP3 Codec] resume -- \r\n");
    return MP3_CODEC_RETURN_OK;
}

static mp3_codec_function_return_state_t mp3_codec_pause(mp3_codec_media_handle_t *handle)
{
    LOGI("[MP3 Codec] pause ++\r\n");
    mp3_codec_mutex_lock(handle);
    if (handle->state != MP3_CODEC_STATE_PLAY) {
#if defined(MTK_AUDIO_MIXER_SUPPORT)
        LOGI("[MP3 Codec] pause --: handle(%X) role%d, cannot pause because state(%d)\r\n", handle, handle->mixer_track_role, handle->state);
#else
        LOGI("[MP3 Codec] pause --: cannot pause because state(%d)\r\n", handle->state);
#endif
        mp3_codec_mutex_unlock(handle);
        return MP3_CODEC_RETURN_ERROR;
    }

#if defined(MTK_AUDIO_MIXER_SUPPORT)
    audio_mixer_stop_track(handle->mixer_track_id);
#else
    hal_audio_stop_stream_out();
#endif

    handle->state = MP3_CODEC_STATE_PAUSE;
    mp3_codec_mutex_unlock(handle);
    LOGI("[MP3 Codec] pause --\r\n", ring_buffer_get_data_byte_count(&(mp3_handle_ptr_to_internal_ptr(handle))->stream_out_pcm_buff), mp3_codec_get_share_buffer_data_count(handle));
    return MP3_CODEC_RETURN_OK;
}

static mp3_codec_function_return_state_t mp3_codec_play(mp3_codec_media_handle_t *handle)
{
    LOGI("[MP3 Codec] mp3_codec_play ++\r\n");
    audio_codec_mutex_lock(); /* temp sol. to protect play/resume flow  */
    mp3_codec_mutex_lock(handle);
    if (handle->state != MP3_CODEC_STATE_READY && handle->state != MP3_CODEC_STATE_STOP) {
        mp3_codec_mutex_unlock(handle);
        audio_codec_mutex_unlock();
        return MP3_CODEC_RETURN_ERROR;
    }

    mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(handle);

    internal_handle->mp3_handle = MP3Dec_Init(internal_handle->working_buff1, internal_handle->working_buff2);  // must do this, or SW mp3 IP will work wrong
    if (internal_handle->mp3_handle == NULL) {
        mp3_codec_mutex_unlock(handle);
        audio_codec_mutex_unlock(); /* temp sol. to protect play/resume flow  */
        return MP3_CODEC_RETURN_ERROR;
    }

/*
#ifdef MTK_BT_AWS_ENABLE
        if (internal_handle->aws_flag) {
            mp3_codec_aws_play_setting(internal_handle);
        }
#endif
*/
    mp3_handle_ptr_to_internal_ptr(handle)->media_bitstream_end = 0;
    mp3_codec_function_return_state_t ret = mp3_codec_play_internal(handle);

    mp3_codec_mutex_unlock(handle);
    audio_codec_mutex_unlock(); /* temp sol. to protect play/resume flow  */
    LOGI("[MP3 Codec] mp3_codec_play --\r\n");
    return ret;
}


static mp3_codec_function_return_state_t mp3_codec_stop(mp3_codec_media_handle_t *handle)
{
#if defined(MTK_AUDIO_MIXER_SUPPORT)
    LOGI("[MP3 Codec] stop ++: handle(%X) role%d\r\n", handle, handle->mixer_track_role);
#else
    LOGI("[MP3 Codec] stop ++\r\n");
#endif
    mp3_codec_mutex_lock(handle);
    if (handle->state != MP3_CODEC_STATE_PLAY && handle->state != MP3_CODEC_STATE_PAUSE) {
#if defined(MTK_AUDIO_MIXER_SUPPORT)
        LOGI("[MP3 Codec] stop --: handle(%X) role%d, cannot stop because state(%d)\r\n", handle, handle->mixer_track_role, handle->state);
#else
        LOGI("[MP3 Codec] stop --: cannot stop because state(%d)\r\n", handle->state);
#endif
        mp3_codec_mutex_unlock(handle);
        return MP3_CODEC_RETURN_ERROR;
    }
#if defined(MTK_AUDIO_MIXER_SUPPORT)
    audio_mixer_stop_track(handle->mixer_track_id);
    audio_mixer_free_track_id(handle->mixer_track_id);
#else
    hal_audio_stop_stream_out();
#endif
    mp3_handle_ptr_to_internal_ptr(handle)->media_bitstream_end = 0;
    mp3_codec_event_deregister_callback(handle, MP3_CODEC_QUEUE_EVENT_DECODE);

    //audio_service_clearflag(handle->audio_id);

    mp3_codec_reset_share_buffer(handle);
    mp3_codec_reset_stream_out_pcm_buffer(handle);    // if don't do this it will have residue data, and it will be played if you play again
#if defined(MTK_AUDIO_MIXER_SUPPORT)
#else
    vPortFree(mp3_handle_ptr_to_audio_internal_ring_buffer(handle));
    mp3_handle_ptr_to_audio_internal_ring_buffer(handle) = NULL;
#endif

    handle->state = MP3_CODEC_STATE_STOP;

    mp3_codec_mutex_unlock(handle);

#if defined(MTK_AUDIO_MIXER_SUPPORT)
    LOGI("[MP3 Codec] stop --: handle(%X) role%d\r\n", handle, handle->mixer_track_role);
#else
    LOGI("[MP3 Codec] stop --\r\n");
#endif
    return MP3_CODEC_RETURN_OK;
}


mp3_codec_function_return_state_t mp3_codec_close(mp3_codec_media_handle_t *handle)
{
#if defined(MTK_AUDIO_MIXER_SUPPORT)
    LOGI("[MP3 Codec] close++ handle(%X), role%d\n", handle, handle->mixer_track_role);
#else
    LOGI("[MP3 Codec] close++\n");
#endif
    mp3_codec_mutex_lock(handle);
    mp3_codec_internal_handle_t *internal_handle = mp3_handle_ptr_to_internal_ptr(handle);
#if defined(MTK_AUDIO_MP3_CODEC_AWS_SUPPORT)
        if (internal_handle->aws_flag) {
            mp3_codec_aws_stop_setting(internal_handle);    // to avoid app forget to stop aws
        }
#endif

    if (handle->state != MP3_CODEC_STATE_STOP && handle->state != MP3_CODEC_STATE_READY) {
        mp3_codec_mutex_unlock(handle);
#if defined(MTK_AUDIO_MIXER_SUPPORT)
        LOGI("[MP3 Codec] close--: handle(%X) role%d, cannot close because state(%d)\r\n", handle, handle->mixer_track_role, handle->state);
#else
        LOGI("[MP3 Codec] close--: cannot close because state(%d)\r\n", handle->state);
#endif
        return MP3_CODEC_RETURN_ERROR;
    }
    handle->state = MP3_CODEC_STATE_IDLE;

    if (mp3_handle_ptr_to_task_handle(handle) != NULL) {
        LOGI("[MP3 Codec] delete mp3 task\n");
        vTaskDelete(mp3_handle_ptr_to_task_handle(handle));
        mp3_handle_ptr_to_task_handle(handle) = NULL;
    }

    if (mp3_handle_ptr_to_queue_handle(handle) != NULL) {
        LOGI("[MP3 Codec] delete mp3 queue\n");
        vQueueDelete(mp3_handle_ptr_to_queue_handle(handle));
        mp3_handle_ptr_to_queue_handle(handle) = NULL;
    }


    audio_free_id(handle->audio_id);
    //vPortFree(internal_handle->memory_pool);  // since now we are using static memory, so can't free, but even using dynamic, i think free function also act at app site
    vPortFree(mp3_handle_ptr_to_mp3_decode_buffer(handle));
    mp3_handle_ptr_to_mp3_decode_buffer(handle) = NULL;

    internal_handle->memory_pool = NULL;
    internal_handle->IsMP3Exit = false;
    mp3_codec_mutex_unlock(handle);
    vSemaphoreDelete(internal_handle->semaphore_handle);
    vPortFree(internal_handle);
    internal_handle = NULL;

    mp3_handle_ptr_to_internal_ptr(handle) = NULL;

#if defined(MTK_AUDIO_MIXER_SUPPORT)
// if mixer enable, audio_lowpower_set_mode is control by mixer driver
#else
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    audio_lowpower_set_mode(false);
#endif
#endif

    vPortFree(handle->private_data);
    handle->private_data = NULL;

    
    if (mp3_codec_open_counter < 1) {
        mp3_codec_open_counter = 0;
    } else {
        mp3_codec_open_counter--;
    }

#if defined(MTK_AUDIO_MIXER_SUPPORT)
        LOGI("[MP3 Codec] close-- handle(%X), role%d\n", handle, handle->mixer_track_role);
#else
        LOGI("[MP3 Codec] close--\n");
#endif    

    return MP3_CODEC_RETURN_OK;
}

mp3_codec_media_handle_t *mp3_codec_open(mp3_codec_callback_t mp3_codec_callback)
{
    LOGI("[MP3 Codec]Open codec\n");

    mp3_codec_media_handle_t *handle;
    mp3_codec_internal_handle_t *internal_handle; /*internal handler*/

    //get audio id
    uint16_t audio_id = audio_get_id();

    if (audio_id > MAX_AUDIO_FUNCTIONS) {
        LOGI("[MP3 Codec]Audio ID > MAX AUDIO FUNCTIONS\n");
        return 0;
    }
    audio_codec_mutex_create(); /* temp sol. to protect play/resume flow  */    

#if defined(MTK_AUDIO_MIXER_SUPPORT)
// if mixer enable, audio_lowpower_set_mode is control by mixer driver
#else
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    audio_lowpower_set_mode(true);
#endif
#endif

    mp3_codec_private_t *private_data;
    private_data = (mp3_codec_private_t *)pvPortMalloc(sizeof(mp3_codec_private_t));
    if (NULL == private_data) {        
        LOGE("[MP3 Codec] mp3_codec_open failed: cannot allocate private_data\n");
        return NULL;
    }
    memset(private_data, 0, sizeof(mp3_codec_private_t));
    private_data->previous_mp3_frame_size = SHARE_BUFFER_TOO_LESS_AMOUNT;

    /* alloc internal handler space */
    internal_handle = (mp3_codec_internal_handle_t *)pvPortMalloc(sizeof(mp3_codec_internal_handle_t));
    if (NULL == internal_handle) {
        LOGE("[MP3 Codec] mp3_codec_open failed: cannot allocate internal_handle\n");
        return NULL;
    }
    memset(internal_handle, 0, sizeof(mp3_codec_internal_handle_t));
    LOGI("mp3_codec_open: internal_handle = 0x%X", internal_handle);
    
    /* assign internal handler to be global and static handler*/
    private_data->mp3_codec_internal_hdl = internal_handle;

    /* initialize internal handle*/
    internal_handle->share_buff_size = 0;
    internal_handle->decode_pcm_buffer_size = 0;
    internal_handle->stream_out_pcm_buff_size = 0;
    internal_handle->working_buff1_size = 0;
    internal_handle->working_buff2_size = 0;
    internal_handle->IsMP3Exit = true;
    internal_handle->semaphore_handle = xSemaphoreCreateMutex();
    internal_handle->media_bitstream_end = 0;
    handle = &internal_handle->handle;
    handle->audio_id = audio_id;
    handle->handler  = mp3_codec_callback;
    handle->play     = mp3_codec_play;
    handle->pause    = mp3_codec_pause;
    handle->resume   = mp3_codec_resume;
    handle->stop     = mp3_codec_stop;
    handle->close_codec = mp3_codec_close;
    handle->skip_id3v2_and_reach_next_frame = mp3_codec_skip_id3v2_and_reach_next_frame;
    handle->state    = MP3_CODEC_STATE_READY;
    handle->flush_data_flag = 0;
    handle->private_data = private_data;
    mp3_codec_buffer_function_init(handle);

#if defined(MTK_AUDIO_MIXER_SUPPORT)
    handle->mixer_track_role = AUDIO_MIXER_TRACK_ROLE_MAIN;
#endif

    if (private_data->mp3_codec_queue_hdl == NULL) {
        private_data->mp3_codec_queue_hdl = xQueueCreate(MP3_CODEC_QUEUE_LENGTH, sizeof(mp3_codec_queue_event_t));
        LOGI("[MP3 Codec] create mp3 queue (0x%X)\r\n", private_data->mp3_codec_queue_hdl);
    }

    {   /* Initialize queue registration */
        uint32_t id_idx;
        for (id_idx = 0; id_idx < MAX_MP3_CODEC_FUNCTIONS; id_idx++) {
            (mp3_handle_ptr_to_queue_id_array(handle))[id_idx] = MP3_CODEC_QUEUE_EVENT_NONE;
        }
    }

    //create decode task
    mp3_codec_task_create(handle);


    if (mp3_codec_open_counter < 0) {
        mp3_codec_open_counter = 1;
    } else {
        mp3_codec_open_counter++;
    }
    if (blIsRegister == false) {
        blIsRegister = true;
        hal_cm4_topsm_register_resume_cb((cm4_topsm_cb)mp3_codec_enter_resume, NULL);
        hal_cm4_topsm_register_suspend_cb((cm4_topsm_cb)mp3_codec_enter_suspend, NULL);
    }

#if defined(MTK_AUDIO_MIXER_SUPPORT)
#else
    global_mp3_handle = handle;
#endif
    return handle;
}


void mp3_codec_enter_resume(void *data)
{
    //LOGI("[MP3 Codec] mp3_codec_enter_resume: mp3_codec_open_counter = %d\n", mp3_codec_open_counter);
    if (mp3_codec_open_counter < 1) {
        return ;
    }

#ifdef HAL_DVFS_MODULE_ENABLED
    mp3_codec_register_mp3_dvfs(true);
#endif

}


void mp3_codec_enter_suspend(void *data)
{
    //LOGI("[MP3 Codec] mp3_codec_enter_suspend: mp3_codec_open_counter = %d\n", mp3_codec_open_counter);
    if (mp3_codec_open_counter < 1) {
        return ;
    }

#ifdef HAL_DVFS_MODULE_ENABLED
    mp3_codec_register_mp3_dvfs(false);
#endif
}

#if 0
mp3_codec_function_return_state_t mp3_codec_enter_resume(void)
{
    //printf("mp3_codec_enter_resume IsMP3Exit=%d\n",mp3_codec_internal_handle->IsMP3Exit);
    if (mp3_codec_internal_handle->IsMP3Exit == false) {
        return MP3_CODEC_RETURN_ERROR;
    }

#ifdef HAL_DVFS_MODULE_ENABLED
    mp3_codec_register_dsp_dvfs(true);
#endif
    return MP3_CODEC_RETURN_OK;
}


mp3_codec_function_return_state_t mp3_codec_enter_suspend(void)
{
    //printf("mp3_codec_enter_suspend IsMP3Exit=%d\n",mp3_codec_internal_handle->IsMP3Exit);
    if (mp3_codec_internal_handle->IsMP3Exit == false) {
        return MP3_CODEC_RETURN_ERROR;
    }

#ifdef HAL_DVFS_MODULE_ENABLED
    //mp3_codec_register_dsp_dvfs(false);
#endif
    return MP3_CODEC_RETURN_OK;

}
#endif

