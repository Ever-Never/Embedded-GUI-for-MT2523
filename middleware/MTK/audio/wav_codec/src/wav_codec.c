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


#include "FreeRTOS.h"
#include "wav_codec_internal.h"
#include "hal_audio_internal_service.h"
#include "hal_audio_internal_pcm.h"

#if defined(MTK_AUDIO_MIXER_SUPPORT)
    #include "audio_mixer_internal.h"
    #include "prompt_control.h"
#else
    #define AUDIO_MIXER_TRACK_BUFFER_SIZE  (16*1024)
#endif

#ifdef HAL_AUDIO_LOW_POWER_ENABLED
#include "hal_audio_low_power.h"
#endif

static void wav_codec_deocde_event_handler(audio_codec_media_handle_t *handle, void *data);

/* share buffer operation function */
static void wav_codec_set_share_buffer(audio_codec_media_handle_t *handle, uint8_t *buffer, uint32_t length)
{
    wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;

    handle->buffer_info.buffer_base = buffer;
    length &= ~0x1; // make buffer size even
    handle->buffer_info.buffer_size = length;
    handle->buffer_info.write       = 0;
    handle->buffer_info.read        = 0;
    handle->waiting                 = false;
    handle->underflow               = false;
    internal_handle->linear_data    = false;
}

// set linear buffer
static audio_codec_status_t wav_codec_set_buffer(audio_codec_media_handle_t *handle, uint8_t *buffer, uint32_t length)
{
    wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;
    audio_codec_audio_buffer_t  *p_bs_ring       = &(handle->buffer_info);

    // allocated in open
    vPortFree(p_bs_ring->buffer_base);

    p_bs_ring->buffer_base = buffer;
    length &= ~0x1; // make buffer size even
    p_bs_ring->buffer_size = length;
    p_bs_ring->write       = length;
    p_bs_ring->read        = 0;
    handle->waiting        = false;
    handle->underflow      = false;
    internal_handle->linear_data = true;

    return AUDIO_CODEC_RETURN_OK;
}

static void wav_codec_get_share_buffer_write_information(audio_codec_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->buffer_info.read > handle->buffer_info.write) {
        count = handle->buffer_info.read - handle->buffer_info.write - 1;
    } else if (handle->buffer_info.read == 0) {
        count = handle->buffer_info.buffer_size - handle->buffer_info.write - 1;
    } else {
        count = handle->buffer_info.buffer_size - handle->buffer_info.write;
    }
    *buffer = handle->buffer_info.buffer_base + handle->buffer_info.write;
    *length = count;
}


static void wav_codec_get_share_buffer_read_information(audio_codec_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->buffer_info.write >= handle->buffer_info.read) {
        count = handle->buffer_info.write - handle->buffer_info.read;
    } else {
        count = handle->buffer_info.buffer_size - handle->buffer_info.read;
    }
    *buffer = handle->buffer_info.buffer_base + handle->buffer_info.read;
    *length = count;
}

// #if defined(MTK_AUDIO_MIXER_SUPPORT)
// #else
static void wav_codec_send_event_from_isr(audio_codec_media_handle_t *handle)
{
    wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;

    if ( !(internal_handle->status & WAV_CODEC_STATUS_IN_TASK_LIST) /* && handle->state == AUDIO_CODEC_STATE_PLAY */ ) {
        internal_handle->status |= WAV_CODEC_STATUS_IN_TASK_LIST;
        if ( audio_codec_event_send_from_isr(handle, NULL) == AUDIO_CODEC_RETURN_OK ) {
            TASK_LOG_D("AUDIO_CODEC_RETURN_OK\n");
        } else {
            internal_handle->status &= ~WAV_CODEC_STATUS_IN_TASK_LIST;
        }
    }
}
// #endif

static void wav_codec_share_buffer_write_data_done(audio_codec_media_handle_t *handle, uint32_t length)
{
    handle->buffer_info.write += length;
    if (handle->buffer_info.write == handle->buffer_info.buffer_size) {
        handle->buffer_info.write = 0;
    }
}

static void wav_codec_finish_write_data(audio_codec_media_handle_t *handle)
{
    handle->waiting   = false;
    handle->underflow = false;
}

static void wav_codec_reset_share_buffer(audio_codec_media_handle_t *handle)
{
    handle->buffer_info.write = 0;
    handle->buffer_info.read  = 0;
    handle->waiting           = false;
    handle->underflow         = false;
}


static void wav_codec_share_buffer_read_data_done(audio_codec_media_handle_t *handle, uint32_t length)
{
    wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;

    handle->buffer_info.read += length;
    if ( internal_handle->linear_data && handle->buffer_info.read == handle->buffer_info.write ) { /*empty*/
        /* do nothing */
    } else if (handle->buffer_info.read == handle->buffer_info.buffer_size) {
        handle->buffer_info.read = 0;
    }
}

static int32_t wav_codec_get_share_buffer_free_space(audio_codec_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->buffer_info.read - handle->buffer_info.write - 2;
    if (count < 0) {
        count += handle->buffer_info.buffer_size;
    }
    return count;
}

static int32_t wav_codec_get_share_buffer_data_count(audio_codec_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->buffer_info.write - handle->buffer_info.read;
    if (count < 0) {
        count += handle->buffer_info.buffer_size;
    }
    return count;
}

// ======== Private lock functions ========
static void wav_codec_mutex_lock(audio_codec_media_handle_t *handle)
{
    wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;

    if (internal_handle->semaphore_handle != NULL) {
        xSemaphoreTake(internal_handle->semaphore_handle, portMAX_DELAY);
    }
}
void wav_codec_mutex_unlock(audio_codec_media_handle_t *handle)
{
    wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;

    if (internal_handle->semaphore_handle != NULL) {
        xSemaphoreGive(internal_handle->semaphore_handle);
    }
}

static audio_codec_status_t wav_codec_fill_decoder_bs(audio_codec_media_handle_t *handle, unsigned int *bs_buf_size, unsigned int remain_data)
{
    wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;

    /* share ring buffer -> decoder_input_buf */
    uint32_t data_cnt    = wav_codec_get_share_buffer_data_count(handle);

    TASK_LOG_D("remain_data %d, data_cnt %d, internal_handle->decoder_input_buf_offset %d\n", remain_data, data_cnt, (internal_handle->decoder_input_buf_size - internal_handle->decoder_input_buf_offset));

    data_cnt = MINIMUM(remain_data, data_cnt);
    data_cnt = MINIMUM(data_cnt, (internal_handle->decoder_input_buf_size - internal_handle->decoder_input_buf_offset));

    if (data_cnt != 0) {
        uint8_t  *p_dst      = (internal_handle->decoder_input_buf + internal_handle->decoder_input_buf_offset);
        int32_t  loop_idx    = 0;
        uint32_t dst_cnt     = data_cnt;
        for (loop_idx = 0; loop_idx < 2; loop_idx++) {
            uint8_t *p_src    = NULL;
            uint32_t src_cnt  = 0;
            uint32_t read_cnt = 0;
            wav_codec_get_share_buffer_read_information(handle, &p_src, &src_cnt);
            read_cnt = MINIMUM(dst_cnt, src_cnt);
            memcpy(p_dst, p_src, read_cnt);
            p_dst += read_cnt;
            dst_cnt -= read_cnt;
            wav_codec_share_buffer_read_data_done(handle, read_cnt);
        }
        *bs_buf_size += ( internal_handle->decoder_input_buf_offset + data_cnt - dst_cnt );
    } else {
        TASK_LOG_D("share ring buffer underflow or no remain bs, data byte %d remain_data %d \n", data_cnt, remain_data);
        return AUDIO_CODEC_RETURN_ERROR;
    }

    return AUDIO_CODEC_RETURN_OK;
}

static audio_codec_status_t wav_codec_fill_pcm_data(audio_codec_media_handle_t *handle)
{
    FUNCTION_IN;
    wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;
    ring_buffer_information_t *p_ring = &(internal_handle->decoded_pcm_ring); // source

#ifdef WAV_CODEC_UT_FLOW
    {   /* ut flow. fill data to hal audio pcm ring buffer . */
        uint8_t *p_src    = NULL;
        int32_t  loop_idx = 0;
        uint32_t dst_cnt  = 0;
        uint32_t data_cnt = 0;
        uint32_t src_cnt  = 0;
        uint32_t read_cnt = 0;

        if ( internal_handle->linear_data == 1 ) {
#if defined(MTK_AUDIO_MIXER_SUPPORT)
            audio_mixer_query_free_count(handle->mixer_track_id, &dst_cnt);
#endif
            while ( dst_cnt > 0 ) {
                data_cnt = ring_buffer_get_data_byte_count(p_ring); // source
                if ( dst_cnt > 0 && data_cnt > 0 ) {
                    /* still have free space & source ring buffer have data. */
                    for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                        ring_buffer_get_read_information(p_ring, &p_src, &src_cnt);
                        read_cnt = MINIMUM(dst_cnt, src_cnt);
                        TASK_LOG_D("src_cnt = %d, dst_cnt = %d, read_cnt = %d\n", src_cnt, dst_cnt, read_cnt);
#if defined(MTK_AUDIO_MIXER_SUPPORT)
                        audio_mixer_write_data(handle->mixer_track_id, p_src, read_cnt);
#endif
                        dst_cnt -= read_cnt;
                        ring_buffer_read_done(p_ring, read_cnt);
                    }
                } else {
                    if ( internal_handle->remain_bs == 0 && data_cnt == 0 ) {
                        internal_handle->decode_end_flag = true;
                        return AUDIO_CODEC_RETURN_OK;
                    }
                    return AUDIO_CODEC_RETURN_ERROR;
                }

                if ( dst_cnt > 0 && internal_handle->remain_bs != 0 ) {
                    wav_codec_deocde_event_handler(handle, NULL);
                }
            }
        }
        else {
#if defined(MTK_AUDIO_MIXER_SUPPORT)
            audio_mixer_query_free_count(handle->mixer_track_id, &dst_cnt);// free space
#else
            hal_audio_get_stream_out_sample_count(&dst_cnt);// free space
#endif
            data_cnt = ring_buffer_get_data_byte_count(p_ring); // source
            TASK_LOG_D("dst_cnt = %d, data_cnt = %d\n", dst_cnt, data_cnt);

            if ( dst_cnt > 0 && data_cnt > 0 ) {
                /* still have free space & source ring buffer have data. */
                for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                    ring_buffer_get_read_information(p_ring, &p_src, &src_cnt);
                    read_cnt = MINIMUM(dst_cnt, src_cnt);
                    TASK_LOG_D("src_cnt = %d, dst_cnt = %d, read_cnt = %d\n", src_cnt, dst_cnt, read_cnt);
#if defined(MTK_AUDIO_MIXER_SUPPORT)
                    audio_mixer_write_data(handle->mixer_track_id, p_src, read_cnt);
#else
                    hal_audio_write_stream_out(p_src, read_cnt);
#endif
                    dst_cnt -= read_cnt;
                    ring_buffer_read_done(p_ring, read_cnt);
                }
            }
            else {
                if ( internal_handle->remain_bs == 0 && data_cnt == 0 ) {
                    internal_handle->decode_end_flag = true;
                    return AUDIO_CODEC_RETURN_OK;
                }
                return AUDIO_CODEC_RETURN_ERROR;
            }
        }
    }
#endif /*WAV_CODEC_UT_FLOW*/

    FUNCTION_OUT;
    return AUDIO_CODEC_RETURN_OK;
}

static void wav_codec_deocde_event_handler(audio_codec_media_handle_t *handle, void *data)
{
    FUNCTION_IN;

    wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;

    audio_codec_status_t status = AUDIO_CODEC_RETURN_OK;
    WAV_DEC_HDL  *wav_handle    = internal_handle->wav_handle;
    uint32_t spce_cnt           = 0;
    unsigned int consumedBytes  = 0;
    unsigned int pcm_out_size   = 0;
    unsigned int bs_buf_size    = 0;
    uint8_t      *pcm_buffer    = internal_handle->decoder_output_buf;
    uint8_t      *bs_buffer     = internal_handle->decoder_input_buf;
    ring_buffer_information_t *p_ring = &(internal_handle->decoded_pcm_ring); // dst

    /* share ring buffer -> bs buffer */
    if ( status == AUDIO_CODEC_RETURN_OK && internal_handle->decoder_output_buf_offset == 0 && internal_handle->remain_bs ) {
        status = wav_codec_fill_decoder_bs(handle, &bs_buf_size, internal_handle->remain_bs);
        if ( status == AUDIO_CODEC_RETURN_OK ) {
            internal_handle->remain_bs -= (bs_buf_size - internal_handle->decoder_input_buf_offset);
        }
    }
    else {
        status = AUDIO_CODEC_RETURN_ERROR;
    }

    if ( status == AUDIO_CODEC_RETURN_OK && internal_handle->decoder_output_buf_offset == 0 ) {
        consumedBytes = WAV_Decode( /*in*/wav_handle, /*in*/bs_buffer, /*in*/bs_buf_size,
                                    /*out*/pcm_buffer, /*out*/&pcm_out_size);
        internal_handle->decoder_input_buf_offset = (bs_buf_size - consumedBytes);
        memmove(bs_buffer, \
                (bs_buffer + consumedBytes), \
                internal_handle->decoder_input_buf_offset);
        pcm_out_size <<= 1;
        internal_handle->decode_size = pcm_out_size;
        TASK_LOG_D("WAV_Decode : in %d out %d consumedBytes %d offset %d pcm_out_size %d \n", bs_buf_size, internal_handle->decode_size, consumedBytes, internal_handle->decoder_input_buf_offset, pcm_out_size);
    } else {
        TASK_LOG_I("bs not enough.\n");
        status = AUDIO_CODEC_RETURN_ERROR;
    }

    /* check if pcm ring overflow. */
    spce_cnt = ring_buffer_get_space_byte_count(p_ring);
    if ( spce_cnt > 0 && (internal_handle->decoder_output_buf_offset != internal_handle->decode_size) ) {
        /* put decoded pcm buffer to decoded pcm ring buffer. */
        int32_t loop_idx;
        uint8_t *p_src   = pcm_buffer + internal_handle->decoder_output_buf_offset;
        uint32_t src_cnt = internal_handle->decode_size - internal_handle->decoder_output_buf_offset;

        for (loop_idx = 0; loop_idx < 2; loop_idx++) {
            uint8_t *p_dst   = NULL;
            uint32_t dst_cnt = 0;
            uint32_t write_cnt;
            ring_buffer_get_write_information(p_ring, &p_dst, &dst_cnt);
            write_cnt = MINIMUM(dst_cnt, src_cnt);
            TASK_LOG_D("write_cnt %d dst_cnt %d dec_src_cnt %d\n", write_cnt, dst_cnt, src_cnt);
            memcpy(p_dst, p_src, write_cnt);
            p_src   += write_cnt;
            src_cnt -= write_cnt;
            internal_handle->decoder_output_buf_offset += write_cnt;
            ring_buffer_write_done(p_ring, write_cnt);
        }
    }
    else {
        TASK_LOG_I("pcm ring overflow.\n");
    }

    if ( internal_handle->decoder_output_buf_offset == internal_handle->decode_size ) {
        if( internal_handle->remain_bs != 0 ) {
            internal_handle->decoder_output_buf_offset = 0;
        }
    }

    TASK_LOG_D("internal_handle->status %d WAV_CODEC_STATUS_DSP_UNDERFLOW %d .\n",
               internal_handle->status, WAV_CODEC_STATUS_DSP_UNDERFLOW);

    if ( internal_handle->remain_bs && (internal_handle->status & WAV_CODEC_STATUS_DSP_UNDERFLOW) ) {
        internal_handle->status &= ~WAV_CODEC_STATUS_DSP_UNDERFLOW;
        TASK_LOG_I("dsp underflow case .\n");
        if ( wav_codec_fill_pcm_data(handle) != AUDIO_CODEC_RETURN_OK ) {
            internal_handle->status |= WAV_CODEC_STATUS_DSP_UNDERFLOW;
        }
    }

    internal_handle->status &= ~WAV_CODEC_STATUS_IN_TASK_LIST;

    FUNCTION_OUT;
}

/* D2M */
static void wav_codec_pcm_isr_handler(hal_audio_event_t event, void *data)
{
    FUNCTION_IN;

    uint32_t dst_cnt  = 0;

    audio_codec_media_handle_t  *handle          = (audio_codec_media_handle_t *) data;
    wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;

    TASK_LOG_D("internal_handle->status = %d\n", internal_handle->status);

    switch (event) {
        case HAL_AUDIO_EVENT_UNDERFLOW:
        case HAL_AUDIO_EVENT_DATA_REQUEST: {
            if( internal_handle->linear_data == 1 ) {
                if ( handle->state == AUDIO_CODEC_STATE_PLAY && !(internal_handle->status & WAV_CODEC_STATUS_IN_TASK_LIST) ) {
                    if ( wav_codec_fill_pcm_data(handle) != AUDIO_CODEC_RETURN_OK) {
                        internal_handle->status |= WAV_CODEC_STATUS_DSP_UNDERFLOW;
                        TASK_LOG_D("dsp underflow .\n");
                    }
                }
#if defined(MTK_AUDIO_MIXER_SUPPORT)
                audio_mixer_query_free_count(handle->mixer_track_id, &dst_cnt);
#endif
                if ( internal_handle->decode_end_flag == true && internal_handle->eof_flag == false ) {
                    internal_handle->eof_flag = true;
#if defined(MTK_AUDIO_MIXER_SUPPORT)
                    audio_mixer_set_eof(handle->mixer_track_id);
#else
                    audio_pcm_set_eof();
#endif
                }
            }
            else {
                handle->handler(handle, AUDIO_CODEC_MEDIA_REQUEST);
                if ( handle->state == AUDIO_CODEC_STATE_PLAY ) {
                    if ( wav_codec_fill_pcm_data(handle) != AUDIO_CODEC_RETURN_OK) {
                        internal_handle->status |= WAV_CODEC_STATUS_DSP_UNDERFLOW;
                        TASK_LOG_D("dsp underflow, remain_bs %d\n", internal_handle->remain_bs);
                    }
                }
                if ( internal_handle->decode_end_flag == true ) {
                    if ( internal_handle->eof_flag == false ) {
                        TASK_LOG_D("audio_pcm_set_eof\r\n");
                        internal_handle->eof_flag = true;
#if defined(MTK_AUDIO_MIXER_SUPPORT)
                        audio_mixer_set_eof(handle->mixer_track_id);
#else
                        audio_pcm_set_eof();
#endif
                    }
                }
                else if ( internal_handle->eof_flag == false ) {
                    if ( handle->state == AUDIO_CODEC_STATE_PLAY ) {
                        wav_codec_send_event_from_isr(handle);
                        handle->handler(handle, AUDIO_CODEC_MEDIA_REQUEST);
                    }
                }
            }
        }
        break;
        case HAL_AUDIO_EVENT_END:
            if ( internal_handle->linear_data == 1 ) {
                TASK_LOG_I("linear_data == 1, get event end\r\n");
                internal_handle->decode_end_flag = false;
                handle->handler(handle, AUDIO_CODEC_MEDIA_OK);
            }
            else {
                TASK_LOG_I("linear_data == 0, get event end\r\n");
                internal_handle->decode_end_flag = false;
                handle->handler(handle, AUDIO_CODEC_MEDIA_EVENT_END);
            }
            break;
        case HAL_AUDIO_EVENT_ERROR:
        case HAL_AUDIO_EVENT_NONE:
        case HAL_AUDIO_EVENT_DATA_NOTIFICATION:
            TASK_LOG_W("error/none/data notify.\n");
            break;
        default:
            TASK_LOG_W("un-support event.\n");
            break;
    }

    FUNCTION_OUT;
}


static audio_codec_status_t wav_codec_resume(audio_codec_media_handle_t *handle)
{
    // wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;

    if (handle->state != AUDIO_CODEC_STATE_PAUSE) {
        TASK_LOG_W("unexpected flow. calling codec_resume before codec_pause.\n");
        return AUDIO_CODEC_RETURN_ERROR;
    }

    /* temp sol. to protect play/resume flow  */
    audio_codec_mutex_lock();
    wav_codec_mutex_lock(handle);

#if defined(MTK_AUDIO_MIXER_SUPPORT)
    if( handle->mixer_track_role == AUDIO_MIXER_TRACK_ROLE_MAIN ) {
        prompt_control_stop_tone();
    }
#endif

#if defined(MTK_AUDIO_MIXER_SUPPORT)
    {
        audio_mixer_status_t audio_mixer_status;
        
        audio_mixer_status = audio_mixer_start_track(handle->mixer_track_id);
        if (AUDIO_MIXER_STATUS_OK != audio_mixer_status) {
            wav_codec_mutex_unlock(handle);
            audio_codec_mutex_unlock();
            TASK_LOG_E("start track fail\n");
            
            return AUDIO_CODEC_RETURN_ERROR;
        }
    }
#else
    {
        hal_audio_start_stream_out(HAL_AUDIO_PLAYBACK_MUSIC);
    }
#endif

    handle->state = AUDIO_CODEC_STATE_PLAY;

    wav_codec_mutex_unlock(handle);
    /* temp sol. to protect play/resume flow  */
    audio_codec_mutex_unlock();

    return AUDIO_CODEC_RETURN_OK;
}

static audio_codec_status_t wav_codec_pause(audio_codec_media_handle_t *handle)
{
    // wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;

    if (handle->state != AUDIO_CODEC_STATE_PLAY) {
        TASK_LOG_W("unexpected flow. calling codec_pause before codec_play.\n");
        return AUDIO_CODEC_RETURN_ERROR;
    }

    wav_codec_mutex_lock(handle);

#if defined(MTK_AUDIO_MIXER_SUPPORT)
    // if ( internal_handle->linear_data == 1 ) {
        audio_mixer_stop_track(handle->mixer_track_id);
    // }
    // else
#else
    {
        hal_audio_stop_stream_out();
    }
#endif
    handle->state = AUDIO_CODEC_STATE_PAUSE;

    wav_codec_mutex_unlock(handle);

    return AUDIO_CODEC_RETURN_OK;
}


static audio_codec_status_t wav_codec_play_internal(audio_codec_media_handle_t *handle)
{
    wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;

    WAV_STATUS   decode_status  = WAV_SUCCESS;
    WAV_DEC_HDL  *wav_handle    = NULL;
    audio_codec_status_t status = AUDIO_CODEC_RETURN_ERROR;
    unsigned int data_cnt       = WAV_CODEC_WAV_BITSTREAM_SIZE;
    unsigned int consumedBytes  = 0;
    unsigned int bs_buf_size    = 0;
    uint8_t      *bs_buffer     = internal_handle->decoder_input_buf;

#if defined(MTK_AUDIO_MIXER_SUPPORT)
    if( handle->mixer_track_role == AUDIO_MIXER_TRACK_ROLE_MAIN ) {
        prompt_control_stop_tone();
    }
#endif

    /* init eof flag */
    internal_handle->eof_flag = false;
    internal_handle->decode_end_flag = false;

    /* register callback for decode task */
    audio_codec_event_register_callback(handle, wav_codec_deocde_event_handler);

    /* init codec. */
    memset(internal_handle->decoder_working_buf, 0, internal_handle->decoder_working_buf_size * sizeof(uint8_t));
    internal_handle->wav_handle = WAV_InitDecoder(internal_handle->decoder_working_buf);
    wav_handle = internal_handle->wav_handle;

    if (internal_handle->wav_handle == NULL) {
        return AUDIO_CODEC_RETURN_ERROR;
    }

    {   /* parse header, reach data chuck */
        data_cnt = wav_codec_get_share_buffer_data_count(handle);
        data_cnt = ( data_cnt > WAV_CODEC_WAV_BITSTREAM_SIZE ) ? WAV_CODEC_WAV_BITSTREAM_SIZE : data_cnt;
        do { /* parse header */
            status = wav_codec_fill_decoder_bs(handle, &bs_buf_size, data_cnt); /* share ring buffer -> bs_buffer */
            if ( status != AUDIO_CODEC_RETURN_OK ) {
                TASK_LOG_E("bs not enough.\n");
                return AUDIO_CODEC_RETURN_ERROR;
            }
            TASK_LOG_D("bs_buf_size %d.\n", bs_buf_size);
            decode_status = WAV_ParseHeader(wav_handle, bs_buffer, bs_buf_size, &consumedBytes);
            if (decode_status == WAV_SUCCESS) {
                break;
            } else if (decode_status == WAV_REFILL_INBUF) {
                /* do nothing */
            } else {
                TASK_LOG_E("Unsupported File or Bad File.\n");
                return AUDIO_CODEC_RETURN_ERROR;
            }
        } while (1);

        { /* adjust share ring buffer read pointer */
            handle->buffer_info.read     = wav_handle->data_offset;
        }
    }

    // dump audio info
    TASK_LOG_I("fomat %d data_offset %d data_chunk_length %d bit_rate %d dutation %d sample_rate %d channel_num %d \n",
               wav_handle->format, wav_handle->data_offset, wav_handle->data_chunk_length, wav_handle->bit_rate,
               wav_handle->total_duration, wav_handle->sample_rate, wav_handle->channel_num);

    /* initialize */
    internal_handle->remain_bs   = wav_handle->data_chunk_length;
    internal_handle->decode_size = 0;
    internal_handle->status      = WAV_CODEC_STATUS_NONE;

    hal_audio_sampling_rate_t sample_rate;
    switch (wav_handle->sample_rate) {
        case 8000:
            sample_rate = HAL_AUDIO_SAMPLING_RATE_8KHZ;
            break;
        case 11025:
            sample_rate = HAL_AUDIO_SAMPLING_RATE_11_025KHZ;
            break;
        case 12000:
            sample_rate = HAL_AUDIO_SAMPLING_RATE_12KHZ;
            break;
        case 16000:
            sample_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
            break;
        case 22050:
            sample_rate = HAL_AUDIO_SAMPLING_RATE_22_05KHZ;
            break;
        case 24000:
            sample_rate = HAL_AUDIO_SAMPLING_RATE_24KHZ;
            break;
        case 32000:
            sample_rate = HAL_AUDIO_SAMPLING_RATE_32KHZ;
            break;
        case 44100:
            sample_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
            break;
        case 48000:
            sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
            break;
        default:
            sample_rate = HAL_AUDIO_SAMPLING_RATE_8KHZ;
    }
    hal_audio_channel_number_t channel_num = (wav_handle->channel_num == 1) ? HAL_AUDIO_MONO : HAL_AUDIO_STEREO;

#if defined(MTK_AUDIO_MIXER_SUPPORT)
    // if ( internal_handle->linear_data == 1 ) {
        handle->mixer_track_id = audio_mixer_get_track_id(sample_rate,
                                 channel_num,
                                 wav_codec_pcm_isr_handler,
                                 handle,
                                 handle->mixer_track_role);
        if ( handle->mixer_track_id < 0 ) {
            TASK_LOG_E("get track id fail\n");
            return AUDIO_CODEC_RETURN_ERROR;
        }

        wav_codec_deocde_event_handler(handle, NULL);
        handle->handler(handle, AUDIO_CODEC_MEDIA_REQUEST);

        TASK_LOG_I("start mixer\n");
        handle->state = AUDIO_CODEC_STATE_PLAY;

        {
            audio_mixer_status_t audio_mixer_status;
            
            audio_mixer_status = audio_mixer_start_track(handle->mixer_track_id);
            if (AUDIO_MIXER_STATUS_OK != audio_mixer_status) {
                TASK_LOG_E("start track fail\n");
                return AUDIO_CODEC_RETURN_ERROR;
            }
        }

#else
    {
        wav_codec_deocde_event_handler(handle, NULL);
        handle->handler(handle, AUDIO_CODEC_MEDIA_REQUEST);

        hal_audio_set_stream_out_sampling_rate(sample_rate);
        hal_audio_set_stream_out_channel_number(channel_num);
//        hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HEADSET);
        hal_audio_register_stream_out_callback(wav_codec_pcm_isr_handler, handle);
        hal_audio_set_memory(internal_handle->pcm_ring);

#ifdef HAL_AUDIO_LOW_POWER_ENABLED
        audio_lowpower_set_mode(true);
#endif
        TASK_LOG_I("start stream out\n");
        handle->state = AUDIO_CODEC_STATE_PLAY;
        hal_audio_start_stream_out(HAL_AUDIO_PLAYBACK_MUSIC);
    }
#endif
    // handle->state = AUDIO_CODEC_STATE_PLAY;

    // TASK_LOG_I("wav_codec_play --\n");
    return AUDIO_CODEC_RETURN_OK;
}


static audio_codec_status_t wav_codec_play(audio_codec_media_handle_t *handle)
{
    audio_codec_status_t ret = AUDIO_CODEC_RETURN_ERROR;

    if (handle->state != AUDIO_CODEC_STATE_READY && handle->state != AUDIO_CODEC_STATE_STOP) {
        TASK_LOG_W("unexpected flow. calling codec_play before codec_open or codec_stop.\n");
        return AUDIO_CODEC_RETURN_ERROR;
    }

    /* temp sol. to protect play/resume flow  */
    audio_codec_mutex_lock();
    wav_codec_mutex_lock(handle);

    TASK_LOG_I("wav_codec_play ++\n");
    ret = wav_codec_play_internal(handle);
    TASK_LOG_I("wav_codec_play --\n");

    wav_codec_mutex_unlock(handle);
    /* temp sol. to protect play/resume flow  */
    audio_codec_mutex_unlock();

    return ret;
}


static audio_codec_status_t wav_codec_stop(audio_codec_media_handle_t *handle)
{
    FUNCTION_IN;

    if (handle->state != AUDIO_CODEC_STATE_PLAY && handle->state != AUDIO_CODEC_STATE_PAUSE) {
        TASK_LOG_W("unexpected flow. stop before play.\n");
        return AUDIO_CODEC_RETURN_ERROR;
    }

    wav_codec_mutex_lock(handle);

    wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;
    ring_buffer_information_t *p_ring = &(internal_handle->decoded_pcm_ring);

#if defined(MTK_AUDIO_MIXER_SUPPORT)
    // if ( internal_handle->linear_data == 1 ) {
        audio_mixer_stop_track(handle->mixer_track_id);
        audio_mixer_free_track_id(handle->mixer_track_id);
    // }
    // else
#else
    {
        /* stop hal audio. */
        hal_audio_stop_stream_out();
    }
#endif

    audio_codec_event_deregister_callback(handle);

    /* reset */
    wav_codec_reset_share_buffer(handle);
    p_ring->read_pointer         = 0;
    p_ring->write_pointer        = 0;

    internal_handle->decode_size = 0;
    internal_handle->status      = WAV_CODEC_STATUS_NONE;

    handle->state = AUDIO_CODEC_STATE_STOP;

    wav_codec_mutex_unlock(handle);

    FUNCTION_OUT;

    return AUDIO_CODEC_RETURN_OK;
}


audio_codec_status_t audio_codec_wav_codec_close(audio_codec_media_handle_t *handle)
{
    FUNCTION_IN;
    TASK_LOG_I("Close codec\n");

    wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;
    if (handle->state != AUDIO_CODEC_STATE_STOP && handle->state != AUDIO_CODEC_STATE_READY) {
        TASK_LOG_W("unexpected flow. calling codec_close before codec_stop or codec_open.\n");
        return AUDIO_CODEC_RETURN_ERROR;
    }

    wav_codec_mutex_lock(handle);

    handle->state = AUDIO_CODEC_STATE_IDLE;

    ring_buffer_information_t  *p_pcm_ring = &(internal_handle->decoded_pcm_ring);
    audio_codec_audio_buffer_t *p_bs_ring  = &(handle->buffer_info);

    audio_free_id(handle->audio_id);
    vPortFree(internal_handle->decoder_working_buf);
    vPortFree(internal_handle->decoder_input_buf);
    vPortFree(internal_handle->decoder_output_buf);


    vPortFree(p_pcm_ring->buffer_base_pointer);

    // TBD. condition could be dangerous.
    if ( !internal_handle->linear_data ) {
        vPortFree(p_bs_ring->buffer_base);
    }

#if defined(MTK_AUDIO_MIXER_SUPPORT)
#else

#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    audio_lowpower_set_mode(false);
#endif

    vPortFree(internal_handle->pcm_ring);
#endif

    wav_codec_mutex_unlock(handle);
    vSemaphoreDelete(internal_handle->semaphore_handle);

    vPortFree(internal_handle);
    internal_handle = NULL;

// #ifdef HAL_AUDIO_LOW_POWER_ENABLED
    // audio_lowpower_set_mode(false);
// #endif

    FUNCTION_OUT;

    return AUDIO_CODEC_RETURN_OK;
}

static void wav_codec_set_track_role(audio_codec_media_handle_t *handle, audio_mixer_role_t role)
{
    handle->mixer_track_role = role;
}

void wav_codec_buffer_function_init(audio_codec_media_handle_t *handle)
{
    FUNCTION_IN;
    handle->get_write_buffer   = wav_codec_get_share_buffer_write_information;
    handle->write_data_done    = wav_codec_share_buffer_write_data_done;
    handle->finish_write_data  = wav_codec_finish_write_data;
    handle->get_free_space     = wav_codec_get_share_buffer_free_space;
    handle->get_data_count     = wav_codec_get_share_buffer_data_count;
    FUNCTION_OUT;
}

audio_codec_status_t wav_codec_buffer_and_codec_init(audio_codec_media_handle_t *handle)
{
    FUNCTION_IN;
    // allocate
    //     - bs ring buffer for data exchange btw user & driver
    //     - IP working buffer
    //     - IP input / output temp buffer

    uint8_t *int_buffer, *pcm_buffer, *bs_buffer, *bs_ring, *decode_pcm_ring;
    unsigned int int_buf_size         = 0;
    unsigned int pcm_buf_size         = 0;
    unsigned int bs_buf_size          = WAV_CODEC_WAV_BITSTREAM_SIZE;
    uint32_t     bs_ring_size         = WAV_CODEC_BS_RING_BUFFER_SIZE;
    uint32_t     decode_pcm_ring_size = WAV_CODEC_DECODE_PCM_RING_BUFFER_SIZE;

    wav_codec_internal_handle_t *internal_handle = (wav_codec_internal_handle_t *) handle;

    WAV_GetDecBufferSize(bs_buf_size , &pcm_buf_size, &int_buf_size);
    {   /* Force buffer size to be 4-byte-aligned */
        int_buf_size = FOUR_BYTE_ALIGNED(int_buf_size);
        pcm_buf_size = FOUR_BYTE_ALIGNED(pcm_buf_size);
    }

    TASK_LOG_I("memory usage : bs_ring_size %d int_buf_size %d bs_buf_size %d pcm_buf_size %d decode_pcm_ring %d\n",
               bs_ring_size, int_buf_size, bs_buf_size, pcm_buf_size, decode_pcm_ring_size);

    {
        /* bs ring buffer for data exchange btw user & driver. */
        bs_ring = (uint8_t *)pvPortMalloc(bs_ring_size);

        if ( bs_ring == NULL) {
            TASK_LOG_E("PortMalloc fail, bs_ring\n");
            return AUDIO_CODEC_RETURN_ERROR;
        }

        memset(bs_ring , 0, bs_ring_size * sizeof(uint8_t));
        wav_codec_set_share_buffer(handle, bs_ring, bs_ring_size);
        if ( internal_handle->linear_data == 0 ) {
#if defined(MTK_AUDIO_MIXER_SUPPORT)
            wav_codec_set_track_role(handle, AUDIO_MIXER_TRACK_ROLE_MAIN);
#endif
        }
    }

    /* allocate buffers for decoded pcm data */
    decode_pcm_ring = (uint8_t *)pvPortMalloc(decode_pcm_ring_size);
    internal_handle->decoded_pcm_ring.buffer_base_pointer = decode_pcm_ring;
    internal_handle->decoded_pcm_ring.buffer_byte_count   = decode_pcm_ring_size;
    internal_handle->decoded_pcm_ring.read_pointer        = 0;
    internal_handle->decoded_pcm_ring.write_pointer       = 0;

    /* allocate buffers for wav decoder */
    int_buffer = (uint8_t *)pvPortMalloc(int_buf_size);
    internal_handle->decoder_working_buf_size = int_buf_size;
    internal_handle->decoder_working_buf      = int_buffer;

    bs_buffer = (uint8_t *)pvPortMalloc(bs_buf_size);
    internal_handle->decoder_input_buf_size   = bs_buf_size;
    internal_handle->decoder_input_buf        = bs_buffer;
    internal_handle->decoder_input_buf_offset = 0;

    pcm_buffer = (uint8_t *)pvPortMalloc(pcm_buf_size);
    internal_handle->decoder_output_buf_size   = pcm_buf_size;
    internal_handle->decoder_output_buf        = pcm_buffer;
    internal_handle->decoder_output_buf_offset = 0;

    if ( int_buffer == NULL || bs_buffer == NULL || pcm_buffer == NULL) {
        TASK_LOG_E("PortMalloc fail, int_buffer %d, bs_buffer %d, pcm_buffer %d\n", int_buffer, bs_buffer, pcm_buffer);
        return AUDIO_CODEC_RETURN_ERROR;
    }


    memset(int_buffer, 0, int_buf_size * sizeof(uint8_t));
    memset(pcm_buffer, 0, pcm_buf_size * sizeof(uint8_t));
    memset(bs_buffer , 0, bs_buf_size * sizeof(uint8_t));

#if defined(MTK_AUDIO_MIXER_SUPPORT)
#else
    {
        uint8_t *pcm_ring = NULL;
        uint32_t pcm_ring_size = 0;
        hal_audio_get_memory_size(&pcm_ring_size);
        pcm_ring = (uint8_t *)pvPortMalloc(pcm_ring_size);
        memset(pcm_ring , 0, pcm_ring_size * sizeof(uint8_t));

        internal_handle->pcm_ring      = pcm_ring;
        internal_handle->pcm_ring_size = pcm_ring_size;

        TASK_LOG_I("UT memory usage : pcm_ring_size %d \n", pcm_ring_size);
    }
#endif /*WAV_CODEC_UT_FLOW*/

    FUNCTION_OUT;

    return AUDIO_CODEC_RETURN_OK;
}

audio_codec_media_handle_t *audio_codec_wav_codec_open(audio_codec_callback_t wav_codec_callback, void *param)
{
    FUNCTION_IN;

    TASK_LOG_I("Open codec\n");

    audio_codec_media_handle_t *handle;
    wav_codec_internal_handle_t *internal_handle; /*internal handler*/
    audio_codec_status_t ret = AUDIO_CODEC_RETURN_OK;

    // get audio id
    uint16_t audio_id = audio_get_id();
    if (audio_id > MAX_AUDIO_FUNCTIONS) {
        TASK_LOG_I("Audio ID > MAX AUDIO FUNCTIONS\n");
        return 0;
    }

    /* temp sol. to protect play/resume flow  */
    audio_codec_mutex_create();

    /* alloc internal handle space */
    internal_handle  = (wav_codec_internal_handle_t *)pvPortMalloc(sizeof(wav_codec_internal_handle_t));
    memset(internal_handle, 0, sizeof(wav_codec_internal_handle_t));
    handle           = &internal_handle->handle;
    handle->audio_id = audio_id;
    handle->handler  = wav_codec_callback;
    handle->play     = wav_codec_play;
    handle->pause    = wav_codec_pause;
    handle->resume   = wav_codec_resume;
    handle->stop     = wav_codec_stop;
    handle->set_track_role = wav_codec_set_track_role;

    // for voice prompt
    handle->set_bitstream_buffer = wav_codec_set_buffer;
    
    // Create mutex
    internal_handle->semaphore_handle = xSemaphoreCreateMutex();

    wav_codec_mutex_lock(handle);

    wav_codec_buffer_function_init(handle);
    if ( wav_codec_buffer_and_codec_init(handle) != AUDIO_CODEC_RETURN_OK ) {
        // return NULL;
        ret = AUDIO_CODEC_RETURN_ERROR;
    }

    handle->state = AUDIO_CODEC_STATE_READY;

    wav_codec_mutex_unlock(handle);

    FUNCTION_OUT;

    if ( ret == AUDIO_CODEC_RETURN_OK )
        return handle;
    else
        return NULL;
}