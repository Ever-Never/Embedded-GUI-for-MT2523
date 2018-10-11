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

#include "audio_mixer.h"
#include "audio_mixer_internal.h"

// HAL audio
#include "hal_audio_internal_service.h"
#include "hal_audio_internal_pcm.h"
#include "hal_audio_fw_sherif.h"

#include "memory_attribute.h"
#include "hal_cm4_topsm.h"

// RTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "task_def.h"

#include "syslog.h"
#include "hal_log.h"

#ifdef HAL_AUDIO_LOW_POWER_ENABLED
#include "hal_audio_low_power.h"
#endif

#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs.h"
#endif

#include "bt_hfp_codec_internal.h"

// Cast to 4-byte align
#define FOUR_BYTE_ALIGNED_ADDR(addr) ( ( ((uint32_t) addr) + 3) & (~0x3) )

// ======== Definition ========
#define __AUDIO_MIXER_CONCURRENCY_CHECK__


// ======== Static Structure ========
audio_mixer_t Audio_Mixer;


// ======== Function Declaration ========
static int32_t audio_mixer_query_gain(track_info_t *p_track);


// ======== DVFS related part ========
#ifdef HAL_DVFS_MODULE_ENABLED
static bool mixer_dvfs_valid(uint32_t voltage, uint32_t frequency)
{
    if (frequency < AUDIO_MIXER_FREQ_L_BOUND) {
        return false;
    } else {
        return true;
    }
}

static dvfs_notification_t mixer_dvfs_desc = {
    .domain = "VCORE",
    .module = "CM_CK0",
    .addressee = "mixer_dvfs",
    .ops = {
        .valid = mixer_dvfs_valid,
    }
};

static void audio_mixer_register_dvfs(bool flag)
{
    if (flag) {
        dvfs_register_notification(&mixer_dvfs_desc);
        hal_dvfs_target_cpu_frequency(AUDIO_MIXER_FREQ_L_BOUND, HAL_DVFS_FREQ_RELATION_L);
    } else {
        dvfs_deregister_notification(&mixer_dvfs_desc);
    }
}
#endif /*HAL_DVFS_MODULE_ENABLED*/


// ======== Adaptation for BLI SRC ========
static bool audio_mixer_adapt_resample_open( audio_mixer_track_id_t track_id)
{
    bool is_need_bli_src = false;
    bool is_need_simple_convert = false;

    if (Audio_Mixer.hal_sampling_rate != Audio_Mixer.track[track_id].sampling_rate) {
        is_need_bli_src = true;
    } else if (Audio_Mixer.hal_channel_number != Audio_Mixer.track[track_id].channel_number) {
        is_need_simple_convert = true;
    }
    
    if (is_need_bli_src) {
        uint32_t in_sr = Audio_Mixer.track[track_id].sampling_rate;
        uint32_t in_ch = Audio_Mixer.track[track_id].channel_number;
        uint32_t ou_sr = Audio_Mixer.hal_sampling_rate;
        uint32_t ou_ch = Audio_Mixer.hal_channel_number;
        uint32_t buf_size = 0;
        
        // Prepare for BLI SRC 
        BLI_GetMemSize((unsigned int)in_sr, (unsigned int)in_ch, (unsigned int)ou_sr, (unsigned int)ou_ch, (unsigned int *)&buf_size);
        Audio_Mixer.track[track_id].bli_allocate_buffer_alloc = (void *)pvPortMalloc(buf_size + 4);
        if (Audio_Mixer.track[track_id].bli_allocate_buffer_alloc == NULL) {
            log_hal_error("[Mixer] alloc fail: bli_allocate_buffer %d \n", buf_size);
            return false;
        }
        Audio_Mixer.track[track_id].bli_allocate_buffer = (uint8_t *)FOUR_BYTE_ALIGNED_ADDR(Audio_Mixer.track[track_id].bli_allocate_buffer_alloc);
        Audio_Mixer.track[track_id].bli_handle = BLI_Open((unsigned int)in_sr, (unsigned int)in_ch, (unsigned int)ou_sr, (unsigned int)ou_ch, (char *)Audio_Mixer.track[track_id].bli_allocate_buffer, NULL);
    } else if (is_need_simple_convert) {
        if (Audio_Mixer.hal_channel_number == 1)
            Audio_Mixer.track[track_id].channel_convert = AUDIO_MIXER_CHANNEL_CONVERT_STEREO_TO_MONO;
        else
            Audio_Mixer.track[track_id].channel_convert = AUDIO_MIXER_CHANNEL_CONVERT_MONO_TO_STEREO;
    } else {
        Audio_Mixer.track[track_id].channel_convert = AUDIO_MIXER_CHANNEL_CONVERT_NONE;
    }
    
    return true;
}

static uint32_t audio_mixer_adapt_resample_covert(
    track_info_t *p_track,
    uint8_t  *in_buf,            /* Input, pointer to input buffer */
    uint32_t *in_length,         /* Input, length(byte) of input buffer */
                                 /* Output, length(byte) left in the input buffer after conversion */
    uint8_t  *out_buf,           /* Input, pointer to output buffer */
    uint32_t *out_length)        /* Input, length(byte) of output buffer */
                                 /* Output, output data length(byte) */
{
    uint32_t consumed_source_data_count = 0;
    uint32_t i, u_src_count, u_dst_count, u_sample_count;
    int16_t *p_src, *p_dst;

    if (p_track->bli_handle) {
        consumed_source_data_count = BLI_Convert(p_track->bli_handle, (short *)in_buf, (unsigned int *)in_length, (short *)out_buf, (unsigned int *)out_length);
    } else if (p_track->channel_convert == AUDIO_MIXER_CHANNEL_CONVERT_STEREO_TO_MONO) {
        u_src_count = (*in_length)/4;
        u_dst_count = (*out_length)/2;
        
        // Calculate valid length
        if (u_src_count >= u_dst_count) {
            u_sample_count = u_dst_count;
        } else {
            u_sample_count = u_src_count;
        }
        
        p_src = (int16_t *)in_buf;
        p_dst = (int16_t *)out_buf;
        
        // Stereo to mono
        for (i=0; i<u_sample_count; i++) {
            int32_t temp, temp1, temp2;
            
            temp1 = *p_src++;
            temp2 = *p_src++;
            temp = (temp1 + temp2)/2;
            *p_dst++ = (int16_t)temp;
        }
        *in_length = (*in_length) - (u_sample_count*4);
        *out_length = u_sample_count*2;
        consumed_source_data_count = u_sample_count*4;
    } else if (p_track->channel_convert == AUDIO_MIXER_CHANNEL_CONVERT_MONO_TO_STEREO) {
        u_src_count = (*in_length)/2;
        u_dst_count = (*out_length)/4;
        
        // Calculate valid length
        if (u_src_count >= u_dst_count) {
            u_sample_count = u_dst_count;
        } else {
            u_sample_count = u_src_count;
        }
        
        p_src = (int16_t *)in_buf;
        p_dst = (int16_t *)out_buf;
        
        // Mono to stereo
        for (i=0; i<u_sample_count; i++) {
            int16_t temp;
            
            temp = *p_src++;
            *p_dst++ = temp;
            *p_dst++ = temp;
        }
        *in_length = (*in_length) - (u_sample_count*2);
        *out_length = u_sample_count*4;
        consumed_source_data_count = u_sample_count*2;
    }

    return consumed_source_data_count;
}


// ======== Private utility functions ========
static const uint32_t sampling_rate_table[10] = {
     8000, 11025, 12000,
    16000, 22050, 24000,
    32000, 44100, 48000,
    96000
};

static const uint8_t channel_number_table[4] = {
    1, 2, 2, 2
};

// To convert sampling rate index to sampling rate.
static uint32_t audio_mixer_convert_sampling_rate(hal_audio_sampling_rate_t sampling_rate_enum) 
{
    if ((sampling_rate_enum > 9) || (sampling_rate_enum < 0))
        return 0;
    
    return sampling_rate_table[sampling_rate_enum];
}

// To convert channel number index to channel number
static uint8_t audio_mixer_convert_channel_number(hal_audio_channel_number_t channel_number_enum)
{
    if ((channel_number_enum > 3) || (channel_number_enum < 0))
        return 0;

    return channel_number_table[channel_number_enum];
}

static bool audio_mixer_activate_track(
    audio_mixer_track_id_t          track_id)
{
    bool ret;
    
    ret = audio_mixer_adapt_resample_open(track_id);
    if (ret == false) {
        return false;
    }

    // Update active track count
    if (Audio_Mixer.track[track_id].role == AUDIO_MIXER_TRACK_ROLE_MAIN) {
        Audio_Mixer.active_main_track_count++;
    } else if (Audio_Mixer.track[track_id].role == AUDIO_MIXER_TRACK_ROLE_SIDE) {
        Audio_Mixer.active_side_track_count++;
    }

    // Fill the current gain
    Audio_Mixer.track[track_id].current_gain = audio_mixer_query_gain( &Audio_Mixer.track[track_id] );

    Audio_Mixer.track[track_id].waiting = false;
    Audio_Mixer.track[track_id].state = AUDIO_MIXER_STATE_MACHINE_RUNNING;

    return true;
}

static void audio_mixer_deactivate_track(
    audio_mixer_track_id_t          track_id)
{
    Audio_Mixer.track[track_id].state = AUDIO_MIXER_STATE_MACHINE_CONFIGURED;
    
    if (Audio_Mixer.track[track_id].bli_allocate_buffer_alloc) {
        vPortFree(Audio_Mixer.track[track_id].bli_allocate_buffer_alloc);
        Audio_Mixer.track[track_id].bli_allocate_buffer_alloc = NULL;
        Audio_Mixer.track[track_id].bli_handle = NULL;
    }
}

// == Buffer Operation ==
static uint32_t audio_mixer_bo_get_data_count(track_buffer_t *p_bs)
{
    uint32_t read, write, data_count;
    
    read = p_bs->read;
    write = p_bs->write;
    if (write >= read) {
        data_count = write - read;
    } else {
        data_count = p_bs->size - read + write;
    }
    
    return data_count;
}

static uint32_t audio_mixer_bo_get_free_count(track_buffer_t *p_bs)
{
    uint32_t data_count, free_count;
    
    data_count = audio_mixer_bo_get_data_count(p_bs);
    free_count = p_bs->size - 1 - data_count;

    return free_count & (~0x1);
//    return free_count;
}

static void audio_mixer_bo_get_free_buffer(
    track_buffer_t                  *p_bs,
    uint8_t                         **p_buf,
    uint32_t                        *buf_size)
{
    uint32_t read, write, segment;
    
    read = p_bs->read;
    write = p_bs->write;
    if (write >= read) {
        segment = p_bs->size - write;
        if (read == 0) {
            segment--;
        }
    } else {
        segment = read - write - 1;
    }
    
    // Force even
    segment = segment & (~0x1);
    
    *p_buf = p_bs->base_address + p_bs->write;
    *buf_size = segment;
}

static void audio_mixer_bo_write_data_done(
    track_buffer_t                  *p_bs,
    uint32_t                        amount)
{
    uint32_t write = p_bs->write;
    
    write += amount;
    if (write >= p_bs->size) {
        write -= p_bs->size;
    }
    p_bs->write = write;
}

static void audio_mixer_bo_get_data_buffer(
    track_buffer_t                  *p_bs,
    uint8_t                         **p_buf,
    uint32_t                        *buf_size)
{
    uint32_t read, write, segment;
    
    read = p_bs->read;
    write = p_bs->write;
    if (write >= read) {
        segment = write - read;
    } else {
        segment = p_bs->size - read;
    }
    
    *p_buf = p_bs->base_address + p_bs->read;
    *buf_size = segment;
}

static void audio_mixer_bo_read_data_done(
    track_buffer_t                  *p_bs,
    uint32_t                        amount)
{
    uint32_t read = p_bs->read;
    
    read += amount;
    if (read >= p_bs->size) {
        read -= p_bs->size;
    }
    p_bs->read = read;
}

// ======== Mixer Process ========
static int32_t audio_mixer_query_gain(track_info_t *p_track)
{
    int32_t ret_gain = AUDIO_MIXER_UNIT_GAIN;
    
    if ( Audio_Mixer.hal_is_pcm_route ||
         Audio_Mixer.hal_is_p2w ||
         (Audio_Mixer.active_main_track_count && Audio_Mixer.active_side_track_count) ) {
        // Overlap
        if (p_track->role == AUDIO_MIXER_TRACK_ROLE_MAIN) {
            ret_gain = Audio_Mixer.main_track_gain;
        } else if (p_track->role == AUDIO_MIXER_TRACK_ROLE_SIDE) {
            ret_gain = Audio_Mixer.side_track_gain;
        }
    }
    
    return ret_gain;
}

static void audio_mixer_process_track(track_info_t *p_track)
{
    uint8_t *p_src_buf = Audio_Mixer.resample_temp_block;
    uint32_t i;
    uint32_t SRC_free_count = Audio_Mixer.mixer_block_size;
    
    if (p_track->bli_handle || p_track->channel_convert) {
        // Need SRC
        
        for (i=0; i<2; i++) {
            uint8_t *p_source_buf;
            uint32_t u_source_data_count;
            uint32_t consumed_source_data_count, produced_SRC_data_count;
            
            audio_mixer_bo_get_data_buffer( &p_track->source_buffer, &p_source_buf, &u_source_data_count);
            produced_SRC_data_count = SRC_free_count;
            
            if ( (u_source_data_count == 0) || (produced_SRC_data_count == 0) ) {
                break;
            }
            
            consumed_source_data_count = audio_mixer_adapt_resample_covert(p_track, p_source_buf, &u_source_data_count, p_src_buf, &produced_SRC_data_count);
            
            // Update information
            audio_mixer_bo_read_data_done( &p_track->source_buffer, consumed_source_data_count );
            p_src_buf += produced_SRC_data_count;
            SRC_free_count -= produced_SRC_data_count;
        }
        if (SRC_free_count) {
            memset(p_src_buf, 0, SRC_free_count);
            log_hal_info("[Mixer] UDF1 t %x, c %d\n", p_track, SRC_free_count);
        }
    } else {
        // Need not SRC
        for (i=0; i<2; i++) {
            uint8_t *p_source_buf;
            uint32_t u_source_data_count;
            uint32_t segment;
            
            audio_mixer_bo_get_data_buffer( &p_track->source_buffer, &p_source_buf, &u_source_data_count);
            if (u_source_data_count >= SRC_free_count) {
                segment = SRC_free_count;
            } else {
                segment = u_source_data_count;
            }
            
            if (segment == 0) {
                break;
            }
            
            memcpy(p_src_buf, p_source_buf, segment);
            
            // Update information
            audio_mixer_bo_read_data_done( &p_track->source_buffer, segment );
            p_src_buf += segment;
            SRC_free_count -= segment;
        }
        if (SRC_free_count) {
            memset(p_src_buf, 0, SRC_free_count);
            log_hal_info("[Mixer] UDF2 t %x, c %d\n", p_track, SRC_free_count);
        }
    }
    
    // Apply gain and accumulate into mixer_temp_block
    {
        int16_t *p_src_buf16 = (int16_t *)Audio_Mixer.resample_temp_block;
        int16_t *p_dst_buf16 = (int16_t *)Audio_Mixer.mixer_temp_block;
        int32_t target_gain = audio_mixer_query_gain( p_track );
        int32_t current_gain = p_track->current_gain;
        int32_t delta_gain = 0;

        if (target_gain != current_gain) {
            delta_gain = (target_gain - current_gain) / (int32_t)(Audio_Mixer.mixer_block_size/2);
        }
        if (delta_gain == 0) {
            current_gain = target_gain;
        }

        for (i=0; i < (Audio_Mixer.mixer_block_size/2); i++) {
            int32_t src_data, dst_data, temp;
            
            src_data = *p_src_buf16++;
            dst_data = *p_dst_buf16;
            
            temp = (src_data * current_gain) >> 15;
            temp += dst_data;
            if (temp > 32767) {
                temp = 32767;
            } else if (temp < -32768) {
                temp = -32768;
            }
            
            *p_dst_buf16++ = (int16_t) temp;
            current_gain += delta_gain;
        }
        p_track->current_gain = target_gain;
    }
    
    // Update EOF state
    if (p_track->eof_state == AUDIO_MIXER_EOF_STATE_FLUSH_TRACK) {
        int32_t temp_count = audio_mixer_bo_get_data_count( &p_track->source_buffer );
        
        log_hal_info("[Mixer] Track EOF. Remaining %d \n", temp_count);
        if (temp_count < 4) {
            p_track->eof_state = AUDIO_MIXER_EOF_STATE_FLUSH_MIXER;
            p_track->eof_remaining_size = audio_mixer_bo_get_data_count( &Audio_Mixer.mixer_buffer );
            log_hal_info("[Mixer] enter mixer EOF. Remaining %d \n", p_track->eof_remaining_size);
        }
        
    }
}

static bool audio_mixer_process_block(void)
{
    uint32_t i, free_count, data_count;
    uint8_t *p_src_buf;
    
    // Check the free buffer size of mixer buffer
    free_count = audio_mixer_bo_get_free_count( &Audio_Mixer.mixer_buffer );
    if (free_count < Audio_Mixer.mixer_block_size) {
        // Need not to process
        return false;
    }

    // To simplified buffer operation
    if (Audio_Mixer.f_direct_track_buffer) {
        return true;
    }

    // Clear mixer temp block
    memset(Audio_Mixer.mixer_temp_block, 0, Audio_Mixer.mixer_block_size);

    // Process each track
    for (i=0; i<AUDIO_MIXER_MAX_TRACK_NUMBER; i++) {
        track_info_t *p_track = &Audio_Mixer.track[i];
        
        if (p_track->state == AUDIO_MIXER_STATE_MACHINE_RUNNING) {
            audio_mixer_process_track(p_track);
        }
    }
    
    // Copy data from mixer_temp_block into mixer buffer
    data_count = Audio_Mixer.mixer_block_size;
    p_src_buf = Audio_Mixer.mixer_temp_block;
    for (i=0; (i<2) && data_count; i++) {
        uint8_t *p_dst_buf;
        uint32_t u_dst_buf_count, segment;
        
        audio_mixer_bo_get_free_buffer( &Audio_Mixer.mixer_buffer, &p_dst_buf, &u_dst_buf_count);
        if (data_count >= u_dst_buf_count) {
            segment = u_dst_buf_count;
        } else {
            segment = data_count;
        }
        memcpy(p_dst_buf, p_src_buf, segment);
        audio_mixer_bo_write_data_done( &Audio_Mixer.mixer_buffer, segment );
        
        // Update source information
        p_src_buf += segment;
        data_count -= segment;
    }
    
    return true;
}

static void audio_mixer_notify_track(void)
{
    uint32_t i;

    for (i=0; i<AUDIO_MIXER_MAX_TRACK_NUMBER; i++) {
        if (Audio_Mixer.track[i].state == AUDIO_MIXER_STATE_MACHINE_RUNNING) {
            uint32_t free_amount;
            
            if (Audio_Mixer.track[i].eof_state == AUDIO_MIXER_EOF_STATE_FLUSH_HAL) {
                if ( Audio_Mixer.hal_is_pcm_route ||
                     Audio_Mixer.hal_is_p2w ||
                     (Audio_Mixer.active_main_track_count + Audio_Mixer.active_side_track_count > 1) ) {
                    Audio_Mixer.track[i].callback( HAL_AUDIO_EVENT_END, Audio_Mixer.track[i].callback_user_data );
                } else {
                   log_hal_info("mixer set eof to HAL\r\n");
                   Audio_Mixer.hal_eof_waiting = true;
                   audio_pcm_set_eof();
                }
                break;
            }
            
            free_amount = audio_mixer_bo_get_free_count( &Audio_Mixer.track[i].source_buffer );
//            if ( (free_amount >= AUDIO_MIXER_NOTIFY_THRESHOLD) && !Audio_Mixer.track[i].waiting ) {
            if ( free_amount >= AUDIO_MIXER_NOTIFY_THRESHOLD ) {
                Audio_Mixer.track[i].waiting = true;
                Audio_Mixer.track[i].callback( HAL_AUDIO_EVENT_DATA_REQUEST, Audio_Mixer.track[i].callback_user_data );
            }
        }
    }
}

static void audio_mixer_process(audio_mixer_queue_event_id_t id, void *data)
{
    bool f_consume = true;
    uint32_t times = 0;
    uint32_t i;
    
    // HAL consumed all data, we should notify all tracks
    if( id == AUDIO_MIXER_QUEUE_EVENT_END ) {
        for (i=0; i<AUDIO_MIXER_MAX_TRACK_NUMBER; i++) {
            if (Audio_Mixer.track[i].state == AUDIO_MIXER_STATE_MACHINE_RUNNING) {
                    Audio_Mixer.track[i].waiting = true;
                    Audio_Mixer.track[i].callback( HAL_AUDIO_EVENT_END, Audio_Mixer.track[i].callback_user_data );
            }
        }
        Audio_Mixer.hal_callback_waiting = true;
        return;
    }

    do {
        // Execute a block size
        f_consume = audio_mixer_process_block();

        // Callback if the data isn't enough
        audio_mixer_notify_track();
            
        times++;
    } while (f_consume && (times<1));

    Audio_Mixer.hal_callback_waiting = false;
}

// ======== Task related part ========
static void audio_mixer_task_main(void *arg)
{
    log_hal_info("[Mixer] task_main create\n");

    audio_mixer_queue_event_t event;

    while (1) {
        if (xQueueReceive(Audio_Mixer.queue_handle, &event, portMAX_DELAY)) {
            audio_mixer_process(event.id, event.parameter);
        }
    }
}

static void auddio_mixer_send_event(audio_mixer_queue_event_id_t id, void *parameter)
{
    audio_mixer_queue_event_t event;
    
    event.id        = id;
    event.parameter = parameter;

    if (Audio_Mixer.queue_handle != NULL) {
        if (xQueueSendFromISR(Audio_Mixer.queue_handle, &event, 0) != pdPASS) {
            log_hal_info("[Mixer] send event fail\n");
        }
    }

}

static void audio_mixer_apply_gain(uint8_t *p_src_buf, uint32_t size)
{
    int16_t *p_buf16 = (int16_t *)p_src_buf;
    int32_t i, temp;
    int32_t target_gain = Audio_Mixer.main_track_gain;
    int32_t current_gain = Audio_Mixer.pcm_route_current_gain;
    int32_t delta_gain = 0;
    
    if (target_gain != current_gain) {
        delta_gain = (target_gain - current_gain) / (int32_t)(size/2);
    }
    if (delta_gain == 0) {
        current_gain = target_gain;
    }
    
    for (i=0; i<(size/2); i++) {
        temp = *p_buf16;
        temp = (temp * current_gain)>>15;
        
        *p_buf16++ = (int16_t)temp;
        current_gain += delta_gain;
    }
    Audio_Mixer.pcm_route_current_gain = target_gain;
}

void audio_mixer_mix_two_buffer(uint8_t *p_acc_buf, uint8_t *p_ref_buf, uint32_t size)
{
    int16_t *p_acc_buf16 = (int16_t *)p_acc_buf;
    int16_t *p_ref_buf16 = (int16_t *)p_ref_buf;
    int32_t i, temp;

    for (i=0; i<(size/2); i++) {
        temp = *p_ref_buf16++;
        temp += *p_acc_buf16;
        if (temp > 32767) {
            temp = 32767;
        } else if (temp < -32768) {
            temp = -32768;
        }
        *p_acc_buf16++ = (int16_t)temp;
    }
}

static void audio_mixer_ISR_update_eof_count(int32_t consume_size)
{
    uint32_t i;

    for (i=0; i<AUDIO_MIXER_MAX_TRACK_NUMBER; i++) {
        if (Audio_Mixer.track[i].state == AUDIO_MIXER_STATE_MACHINE_RUNNING) {
            if (Audio_Mixer.track[i].eof_state == AUDIO_MIXER_EOF_STATE_FLUSH_MIXER) {
                Audio_Mixer.track[i].eof_remaining_size -= consume_size;
                if (Audio_Mixer.track[i].eof_remaining_size <= 0) {
                    Audio_Mixer.track[i].eof_state = AUDIO_MIXER_EOF_STATE_FLUSH_HAL;
                }
            }
        }
    }
}

static void audio_mixer_merge_buffer(uint8_t *p_acc_buf, uint32_t len)
{
    uint32_t loop_idx;
    
    // Mixer info
    uint8_t *p_mixer_buf;
    uint32_t mixer_data_count;
    
    uint8_t *p_buf_temp = p_acc_buf;
    uint32_t u_remaining_size = len;
    
    audio_mixer_apply_gain(p_acc_buf, len);

    for (loop_idx = 0; loop_idx < 2; loop_idx++) {
        uint32_t segment;
        
        // Get mixer data amount
        audio_mixer_bo_get_data_buffer( Audio_Mixer.p_buffer_to_hal, &p_mixer_buf, &mixer_data_count);

        // Compare data amount
        if (mixer_data_count >= u_remaining_size) {
            segment = u_remaining_size;
        } else {
            segment = mixer_data_count;
        }
        
        // Mix data into PCM route
        audio_mixer_mix_two_buffer(p_buf_temp, p_mixer_buf, segment);
        
        // Update read pointer
        audio_mixer_bo_read_data_done( Audio_Mixer.p_buffer_to_hal, segment);
        p_buf_temp += segment;
        u_remaining_size -= segment;
    }
}


static void audio_mixer_pcmroute_callback(void *data)
{
    // Do not modify
    uint8_t *p_route_buf;
    uint32_t u_route_size;

    // PCM Route information
    uint16_t word_cnt = *DSP_PCM_ROUTE_DATA_LENGTH & 0x7FFF;
    uint16_t dsp_addr = *DSP_PCM_ROUTE_DATA_ADDRESS;
    uint16_t page_num = *DSP_PCM_ROUTE_DATA_PAGENUM;
    volatile uint16_t *p_dsp_buf = DSP_DM_ADDR(page_num, dsp_addr);

//log_hal_info("[Mixer] PCMRoute ISR\n");

    p_route_buf = Audio_Mixer.pcm_route_buffer;
    u_route_size = word_cnt * 2;
    
    // Read from DSP
    audio_idma_read_from_dsp( (uint16_t *)p_route_buf, p_dsp_buf, word_cnt);    
    
    // Apply gain and merge mixed data into
    audio_mixer_merge_buffer(p_route_buf, u_route_size);
    
    // Write to DSP
    audio_idma_write_to_dsp(p_dsp_buf, (uint16_t *)p_route_buf, word_cnt);
    audio_mixer_ISR_update_eof_count(u_route_size);

    // If mixer data isn't enough, notify task to process
    {
        uint32_t mixer_free_space;
        
        mixer_free_space = audio_mixer_bo_get_free_count( Audio_Mixer.p_buffer_to_hal );
        if ( (mixer_free_space >= Audio_Mixer.mixer_block_size) && (!Audio_Mixer.hal_callback_waiting) ) {
            Audio_Mixer.hal_callback_waiting = true;
            auddio_mixer_send_event(AUDIO_MIXER_QUEUE_EVENT_PROCESS, NULL);
        }
    }
}

static void audio_mixer_stream_out_callback(hal_audio_event_t event, void *data)
{

    uint32_t mixer_data_count;
    uint32_t stream_out_amount;
    uint8_t *p_src_buf;
    uint32_t loop_idx;
    uint32_t bloxk_size = Audio_Mixer.mixer_block_size >> 1;

    switch (event) {
        case HAL_AUDIO_EVENT_UNDERFLOW:
        case HAL_AUDIO_EVENT_DATA_REQUEST:
    
            // Special case for low power
            if ( (!Audio_Mixer.hal_is_aws) && (Audio_Mixer.hal_is_runnung) && (Audio_Mixer.f_direct_track_buffer) && (Audio_Mixer.active_main_track_count == 1) && (Audio_Mixer.active_side_track_count == 0) )
            {
                // Get mixer data amount
                audio_mixer_bo_get_data_buffer( Audio_Mixer.p_buffer_to_hal, &p_src_buf, &mixer_data_count);
                if ((mixer_data_count == 0) && (!Audio_Mixer.hal_callback_waiting)) {
                    Audio_Mixer.hal_callback_waiting = true;
                    auddio_mixer_send_event(AUDIO_MIXER_QUEUE_EVENT_PROCESS, NULL);
                    break;
                }
            }

            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                uint32_t segment;
                
                // Query HAL free space
                hal_audio_get_stream_out_sample_count(&stream_out_amount);
                
                // Get mixer data amount
                audio_mixer_bo_get_data_buffer( Audio_Mixer.p_buffer_to_hal, &p_src_buf, &mixer_data_count);

                // Compare data amount
                if (mixer_data_count >= stream_out_amount) {
                    segment = stream_out_amount;
                } else {
                    segment = mixer_data_count;
                }
                if (segment >= bloxk_size)
                    segment = bloxk_size;
                
                // EOF handling
                if (Audio_Mixer.hal_eof_waiting) {
                    if (Audio_Mixer.hal_eof_waiting_count < 20) {
                       Audio_Mixer.hal_eof_waiting_count++;
                    } else {
                       // Don't write data into DSP when EOF
                       segment = 0;
                    }
                }
                
                // Write to HAL
                if (segment || !loop_idx) {
                    hal_audio_write_stream_out(p_src_buf, segment);

                 //log_hal_info("[Mixer] HAL write:%d, free %d\r\n", segment, stream_out_amount);
                }
                
                // Update read pointer
                audio_mixer_bo_read_data_done( Audio_Mixer.p_buffer_to_hal, segment);
                audio_mixer_ISR_update_eof_count(segment);
                bloxk_size -= segment;
            }

            // If mixer data isn't enough, notify task to process
            {
                uint32_t mixer_free_space;
                
                mixer_free_space = audio_mixer_bo_get_free_count( Audio_Mixer.p_buffer_to_hal );
                if ( (mixer_free_space >= Audio_Mixer.mixer_block_size) && (!Audio_Mixer.hal_callback_waiting) ) {
                    Audio_Mixer.hal_callback_waiting = true;
                    auddio_mixer_send_event(AUDIO_MIXER_QUEUE_EVENT_PROCESS, NULL);
                }
            }

            break;
        case HAL_AUDIO_EVENT_END:
            auddio_mixer_send_event(AUDIO_MIXER_QUEUE_EVENT_END, NULL);
            break;
        case HAL_AUDIO_EVENT_ERROR:
        case HAL_AUDIO_EVENT_NONE:
        case HAL_AUDIO_EVENT_DATA_NOTIFICATION:
            log_hal_info("Event: not handle\r\n");
            break;
    }
}

// ======== HAL stream out related part ========
static bool audio_mixer_start_HAL(
    audio_mixer_track_id_t          track_id)
{
    bool ret;
    
    if (Audio_Mixer.hal_is_runnung) {
        log_hal_error("[Mixer] start re-entry\n");
        return true;
    }

    // Update HAL information
    Audio_Mixer.hal_eof_waiting = false;
    Audio_Mixer.hal_callback_waiting = false;
    Audio_Mixer.hal_eof_waiting_count = 0;
    Audio_Mixer.hal_is_pcm_route = audio_is_dsp_codec();
    Audio_Mixer.hal_is_p2w = bt_hfp_codec_query_is_running();
    Audio_Mixer.hal_is_aws = audio_service_aws_query_flag();
    
    if (Audio_Mixer.hal_is_p2w) {
        // Case: HFP / PCM 2 Way
        Audio_Mixer.hal_sampling_rate_enum = bt_hfp_codec_query_sampling_rate();
        Audio_Mixer.hal_channel_number_enum = bt_hfp_codec_query_channel_number();
        Audio_Mixer.hal_sampling_rate = audio_mixer_convert_sampling_rate( Audio_Mixer.hal_sampling_rate_enum );
        Audio_Mixer.hal_channel_number = audio_mixer_convert_channel_number( Audio_Mixer.hal_channel_number_enum );
        
        if (Audio_Mixer.hal_sampling_rate == 8000) {
            Audio_Mixer.mixer_block_size = 320*2;
        } else {
            Audio_Mixer.mixer_block_size = 640*2;
        }
    } else if (Audio_Mixer.hal_is_pcm_route) {
        // Case: PCM route
        // Apply PCM route info
        hal_audio_stream_t *pStreamInfo;
        
        audio_get_current_stream_config( &pStreamInfo );
        Audio_Mixer.hal_sampling_rate_enum = pStreamInfo->stream_out.stream_sampling_rate;
        Audio_Mixer.hal_channel_number_enum = pStreamInfo->stream_out.stream_channel;
        Audio_Mixer.hal_sampling_rate = audio_mixer_convert_sampling_rate( Audio_Mixer.hal_sampling_rate_enum );
        Audio_Mixer.hal_channel_number = audio_mixer_convert_channel_number( Audio_Mixer.hal_channel_number_enum );
        
        Audio_Mixer.mixer_block_size = AUDIO_MIXER_PCM_ROUTE_MAX_SIZE;
    } else {
        // Case: stream out
        // Apply the information of first stream
        uint32_t block_size;
        
        Audio_Mixer.hal_sampling_rate = Audio_Mixer.track[track_id].sampling_rate;
        Audio_Mixer.hal_channel_number = Audio_Mixer.track[track_id].channel_number;
        Audio_Mixer.hal_sampling_rate_enum = Audio_Mixer.track[track_id].sampling_rate_enum;
        Audio_Mixer.hal_channel_number_enum = Audio_Mixer.track[track_id].channel_number_enum;

        block_size = (Audio_Mixer.hal_sampling_rate * Audio_Mixer.hal_channel_number * 2 * AUDIO_MIXER_BLOCK_MS)/1000;
        Audio_Mixer.mixer_block_size = (block_size + 3) & (~0x3);
    }

    log_hal_info("[Mixer] Start HAL: DSP path %d, s %d, c %d, b %d\n", Audio_Mixer.hal_is_pcm_route, Audio_Mixer.hal_sampling_rate, Audio_Mixer.hal_channel_number, Audio_Mixer.mixer_block_size);

    // Activate track.
    ret = audio_mixer_activate_track(track_id);
    if (ret == false) {
       return false;
    }

    // Create task and queue
    Audio_Mixer.queue_handle = xQueueCreate(AUDIO_MIXER_QUEUE_LENGTH, sizeof(audio_mixer_queue_event_t));
    xTaskCreate(audio_mixer_task_main, AUDIO_MIXER_TASK_NAME, AUDIO_MIXER_TASK_STACKSIZE / sizeof(StackType_t), NULL, AUDIO_MIXER_TASK_PRIO, &Audio_Mixer.task_handle);

#ifdef HAL_DVFS_MODULE_ENABLED
    // DVFS for DSP
    audio_mixer_register_dvfs(true);
#endif

    // Allocate mixer buffer
    Audio_Mixer.mixer_buffer.base_address_alloc  = (uint8_t *)pvPortMalloc(AUDIO_MIXER_BUFFER_SIZE + 4);
    if (Audio_Mixer.mixer_buffer.base_address_alloc == NULL) {
        log_hal_error("[Mixer] alloc fail: mixer_buffer \n");
        return false;
    }
    Audio_Mixer.mixer_buffer.base_address  = (uint8_t *)FOUR_BYTE_ALIGNED_ADDR(Audio_Mixer.mixer_buffer.base_address_alloc);
    Audio_Mixer.mixer_buffer.size = AUDIO_MIXER_BUFFER_SIZE;
    Audio_Mixer.mixer_buffer.read = Audio_Mixer.mixer_buffer.write = 0;
    Audio_Mixer.mixer_temp_block_alloc = (uint8_t *)pvPortMalloc(AUDIO_MIXER_BLOCK_MAX_SIZE + 4);
    if (Audio_Mixer.mixer_temp_block_alloc == NULL) {
        log_hal_error("[Mixer] alloc fail: mixer_temp_block \n");
        return false;
    }
    Audio_Mixer.mixer_temp_block = (uint8_t *)FOUR_BYTE_ALIGNED_ADDR(Audio_Mixer.mixer_temp_block_alloc);

    Audio_Mixer.resample_temp_block_alloc = (uint8_t *)pvPortMalloc(AUDIO_MIXER_BLOCK_MAX_SIZE + 4);
    if (Audio_Mixer.resample_temp_block_alloc == NULL) {
        log_hal_error("[Mixer] alloc fail: resample_temp_block \n");
        return false;
    }
    Audio_Mixer.resample_temp_block = (uint8_t *)FOUR_BYTE_ALIGNED_ADDR(Audio_Mixer.resample_temp_block_alloc);

    // For simplified buffer
    Audio_Mixer.f_direct_track_buffer = false;
    Audio_Mixer.p_buffer_to_hal = &Audio_Mixer.mixer_buffer;

    if (Audio_Mixer.hal_is_p2w) {
        // PCM 2 Way
    } else if (Audio_Mixer.hal_is_pcm_route) {
        // Case: PCM route
        
        Audio_Mixer.pcm_route_buffer_alloc = (uint8_t *)pvPortMalloc(AUDIO_MIXER_PCM_ROUTE_MAX_SIZE + 4);
        if (Audio_Mixer.pcm_route_buffer_alloc == NULL) {
            log_hal_error("[Mixer] alloc fail: pcm_route_buffer \n");
            return false;
        }
        Audio_Mixer.pcm_route_buffer = (uint8_t *)FOUR_BYTE_ALIGNED_ADDR(Audio_Mixer.pcm_route_buffer_alloc);

        Audio_Mixer.pcm_route_current_gain = Audio_Mixer.main_track_gain;
        
        Audio_Mixer.hal_pcm_route_id = audio_pcm_route_register_isr(audio_mixer_pcmroute_callback, NULL, AUDIO_PCM_ROUTE_ORDER_VOICE_PROMPT);
        if (Audio_Mixer.hal_pcm_route_id < 0) {
            log_hal_error("[Mixer] register PCM route fail: %d\r\n", Audio_Mixer.hal_pcm_route_id);
        } else {
            audio_pcm_route_start();
        }
    } else {
        // Case: stream out
        
        uint32_t hal_memory_size;

        // Update simplified buffer
        Audio_Mixer.f_direct_track_buffer = true;
        Audio_Mixer.p_buffer_to_hal = &Audio_Mixer.track[track_id].source_buffer;

#ifdef HAL_AUDIO_LOW_POWER_ENABLED
        audio_lowpower_set_mode(true);
#endif
        
        hal_audio_set_stream_out_sampling_rate(Audio_Mixer.hal_sampling_rate_enum);
        hal_audio_set_stream_out_channel_number(Audio_Mixer.hal_channel_number_enum);

        hal_audio_register_stream_out_callback(audio_mixer_stream_out_callback, NULL);
        
        // Allocate memory for HAL
        hal_audio_get_memory_size(&hal_memory_size);
        Audio_Mixer.hal_memory_size = hal_memory_size;
        Audio_Mixer.hal_memory_alloc = (uint8_t *)pvPortMalloc(hal_memory_size + 4);
        if (Audio_Mixer.hal_memory_alloc == NULL) {
            log_hal_error("[Mixer] alloc fail: hal_memory %d\n",Audio_Mixer.hal_memory_size);
            return false;
        }
        Audio_Mixer.hal_memory = (void *)(uint8_t *)FOUR_BYTE_ALIGNED_ADDR(Audio_Mixer.hal_memory_alloc);
        hal_audio_set_memory(Audio_Mixer.hal_memory);

        //hal_audio_get_stream_out_sample_count(&total_sample_count);
        hal_audio_start_stream_out(HAL_AUDIO_PLAYBACK_MUSIC);
    }

    Audio_Mixer.hal_is_runnung = true;
    
    return true;
}

static void audio_mixer_stop_HAL(void)
{
    log_hal_info("[Mixer] Stop HAL: DSP path %d\n", Audio_Mixer.hal_is_pcm_route);

    if ( !Audio_Mixer.hal_is_runnung ) {
        log_hal_error("[Mixer] stop re-entry\n");
        return;
    }
    
    if (Audio_Mixer.hal_is_p2w) {
        // Case: PCM 2 Way
    } else if (Audio_Mixer.hal_is_pcm_route) {
        // Case: PCM route
        audio_pcm_route_stop();
        audio_pcm_route_deregister_isr(Audio_Mixer.hal_pcm_route_id);
        
        vPortFree(Audio_Mixer.pcm_route_buffer_alloc);
        Audio_Mixer.pcm_route_buffer_alloc = NULL;
    } else {
        // Case: stream out
        
        hal_audio_stop_stream_out();
        vPortFree(Audio_Mixer.hal_memory_alloc);
        Audio_Mixer.hal_memory_alloc = NULL;

#ifdef HAL_AUDIO_LOW_POWER_ENABLED
        audio_lowpower_set_mode(false);
#endif
    }

#ifdef HAL_DVFS_MODULE_ENABLED
    // DVFS for DSP
    audio_mixer_register_dvfs(false);
#endif

    // Free Mixer Buffer
    vPortFree(Audio_Mixer.mixer_buffer.base_address_alloc);
    Audio_Mixer.mixer_buffer.base_address_alloc = NULL;
    vPortFree(Audio_Mixer.mixer_temp_block_alloc);
    Audio_Mixer.mixer_temp_block_alloc = NULL;
    vPortFree(Audio_Mixer.resample_temp_block_alloc);
    Audio_Mixer.resample_temp_block_alloc = NULL;

    // Destory task and queue
    vTaskDelete(Audio_Mixer.task_handle);
    Audio_Mixer.task_handle = NULL;
    vQueueDelete(Audio_Mixer.queue_handle);
    Audio_Mixer.queue_handle = NULL;

    Audio_Mixer.hal_is_runnung = false;
}

// ======== Track related part ========



// ======== Public API related part ========
audio_mixer_status_t audio_mixer_init(void)
{
    memset(&Audio_Mixer, 0, sizeof(Audio_Mixer));
    
    // Set mix mode as mean
    audio_mixer_set_volume(0x4000, 0x4000);
    
    return AUDIO_MIXER_STATUS_OK;
}

audio_mixer_track_id_t audio_mixer_get_track_id(
    hal_audio_sampling_rate_t       sampling_rate_enum,
    hal_audio_channel_number_t      channel_number_enum,
    hal_audio_stream_out_callback_t callback,
    void                            *user_data,
    audio_mixer_role_t              role)
{
    uint32_t sampling_rate, channel_number;
    track_info_t *p_track;
    
    log_hal_info("[Mixer] Get_id: s %d, c %d, r %d \n", sampling_rate_enum, channel_number_enum, role);
    
    // Check role
    if (role >= AUDIO_MIXER_TRACK_ROLE_UNKNOW) {
        log_hal_error("[Mixer] Get_id: Unknown track role %d\n", role);
        return AUDIO_MIXER_TRACK_ID_ERROR;
    }
    
    // Check whether track is available or not
    if ( Audio_Mixer.track_flag & (1<<role) ) {
        log_hal_error("[Mixer] Get_id: track %d is occupied\n", role);
        return AUDIO_MIXER_TRACK_ID_ERROR;
    }

    // Check sampling rate and channel number
    sampling_rate = audio_mixer_convert_sampling_rate(sampling_rate_enum);
    channel_number = audio_mixer_convert_channel_number(channel_number_enum);
    if ((sampling_rate == 0) || (channel_number == 0)) {
        log_hal_error("[Mixer] Get_id: Unknown format s=%d, c=%d\n", sampling_rate_enum, channel_number_enum);
        return AUDIO_MIXER_TRACK_ID_ERROR;
    }

    // Fill information into track
    Audio_Mixer.track_flag |= (1<<role);
    p_track = &Audio_Mixer.track[role];
    
    p_track->sampling_rate_enum = sampling_rate_enum;
    p_track->channel_number_enum = channel_number_enum;
    p_track->role = role;
    p_track->sampling_rate = sampling_rate;
    p_track->channel_number = channel_number;
    p_track->eof_state = AUDIO_MIXER_EOF_STATE_NONE;
    p_track->callback = callback;
    p_track->callback_user_data = user_data;
    
    // Prepare source buffer
    p_track->source_buffer.base_address_alloc = (uint8_t *)pvPortMalloc(AUDIO_MIXER_TRACK_BUFFER_SIZE + 4);
    if (p_track->source_buffer.base_address_alloc == NULL) {
        log_hal_error("[Mixer] alloc fail: source_buffer\n");
        Audio_Mixer.track_flag &= ~(1<<role);
        return AUDIO_MIXER_TRACK_ID_ERROR;
    }
    p_track->source_buffer.base_address = (uint8_t *)FOUR_BYTE_ALIGNED_ADDR(p_track->source_buffer.base_address_alloc);
    p_track->source_buffer.size = AUDIO_MIXER_TRACK_BUFFER_SIZE;
    p_track->source_buffer.read = 0;
    p_track->source_buffer.write = 0;
    
    p_track->state = AUDIO_MIXER_STATE_MACHINE_CONFIGURED;
    
    log_hal_info("[Mixer] Get_id success %d \n", role);
    
    return role;
}

audio_mixer_status_t audio_mixer_free_track_id(
    audio_mixer_track_id_t          track_id)
{
    log_hal_info("[Mixer] Free_id: %d \n", track_id);
    
    if (!(Audio_Mixer.track_flag & (1<<track_id) )) {
        log_hal_error("[Mixer] Free_id: track %d isn't valid\n", track_id);
        return AUDIO_MIXER_STATUS_ERROR;
    }

    if (Audio_Mixer.track[track_id].state != AUDIO_MIXER_STATE_MACHINE_CONFIGURED) {
        log_hal_error("[Mixer] Free_id: track %d wrong state\n", track_id);
        return AUDIO_MIXER_STATUS_ERROR;
    }

    // Update information
    Audio_Mixer.track[track_id].state = AUDIO_MIXER_STATE_MACHINE_IDLE;
    
    // Release resource
    vPortFree(Audio_Mixer.track[track_id].source_buffer.base_address_alloc);
    Audio_Mixer.track[track_id].source_buffer.base_address_alloc = NULL;
    
    // Clear flag
    Audio_Mixer.track_flag &= ~(1<<track_id);

    return AUDIO_MIXER_STATUS_OK;
}

static void audio_mixer_release_HAL_memory(
    audio_mixer_track_id_t          track_id)
{
    if (Audio_Mixer.hal_is_p2w) {
        // Case: PCM 2 Way
    } else if (Audio_Mixer.hal_is_pcm_route) {
        // Case: PCM route
        if (Audio_Mixer.pcm_route_buffer_alloc) {
            vPortFree(Audio_Mixer.pcm_route_buffer_alloc);
            Audio_Mixer.pcm_route_buffer_alloc = NULL;
        }
    } else {
        // Case: stream out
        if (Audio_Mixer.hal_memory_alloc) {
            vPortFree(Audio_Mixer.hal_memory_alloc);
            Audio_Mixer.hal_memory_alloc = NULL;
        }
    }

    // Free Mixer Buffer
    if (Audio_Mixer.mixer_buffer.base_address_alloc) {
        vPortFree(Audio_Mixer.mixer_buffer.base_address_alloc);
        Audio_Mixer.mixer_buffer.base_address_alloc = NULL;
    }
    if (Audio_Mixer.mixer_temp_block_alloc) {
        vPortFree(Audio_Mixer.mixer_temp_block_alloc);
        Audio_Mixer.mixer_temp_block_alloc = NULL;
    }
    if (Audio_Mixer.resample_temp_block_alloc) {
        vPortFree(Audio_Mixer.resample_temp_block_alloc);
        Audio_Mixer.resample_temp_block_alloc = NULL;
    }

    // Release resource
    if (Audio_Mixer.track[track_id].source_buffer.base_address_alloc) {
        vPortFree(Audio_Mixer.track[track_id].source_buffer.base_address_alloc);
        Audio_Mixer.track[track_id].source_buffer.base_address_alloc = NULL;
    }
}

audio_mixer_status_t audio_mixer_start_track(
    audio_mixer_track_id_t          track_id)
{
    uint32_t total_active_track;
    bool ret;
    
    log_hal_info("[Mixer] Start_track: %d \n", track_id);
    
    // Check ID
    if ( (track_id < 0) || (!(Audio_Mixer.track_flag & (1<<track_id) )) ) {
        log_hal_error("[Mixer] start: track %d isn't valid\n", track_id);
        return AUDIO_MIXER_STATUS_ERROR;
    }
    if (Audio_Mixer.track[track_id].state != AUDIO_MIXER_STATE_MACHINE_CONFIGURED) {
        log_hal_error("[Mixer] start: track %d wrong state\n", track_id);
        return AUDIO_MIXER_STATUS_ERROR;
    }

    // Check whether it is the first valid track
    total_active_track = Audio_Mixer.active_main_track_count + Audio_Mixer.active_side_track_count;
    if (total_active_track == 0) {
        ret = audio_mixer_start_HAL(track_id);
        if (ret == false) {
            audio_mixer_release_HAL_memory(track_id);
        }
    } else {
        // The incoming tack isn't the first track

#if defined(__AUDIO_MIXER_CONCURRENCY_CHECK__)
        // When main track is incoming, application should stop the other track.
        if (Audio_Mixer.track[track_id].role == AUDIO_MIXER_TRACK_ROLE_MAIN) {
           log_hal_error("[Mixer] start: concurrency forbidden\n");
           return AUDIO_MIXER_STATUS_ERROR;
        }
#endif 

        // Update simplified buffer
        Audio_Mixer.f_direct_track_buffer = false;
        Audio_Mixer.p_buffer_to_hal = &Audio_Mixer.mixer_buffer;

        ret = audio_mixer_activate_track(track_id);
        if (ret == false) {
           return AUDIO_MIXER_STATUS_ERROR;
        }
    }

    return AUDIO_MIXER_STATUS_OK;
}

audio_mixer_status_t audio_mixer_stop_track(
    audio_mixer_track_id_t          track_id)
{
    uint32_t total_active_track;
    
    log_hal_info("[Mixer] Stop_track: %d \n", track_id);
    
    // Check ID
    if ( (track_id < 0) || (!(Audio_Mixer.track_flag & (1<<track_id) )) ) {
        log_hal_error("[Mixer] stop: track %d isn't valid\n", track_id);
        return AUDIO_MIXER_STATUS_ERROR;
    }
    if (Audio_Mixer.track[track_id].state != AUDIO_MIXER_STATE_MACHINE_RUNNING) {
        log_hal_error("[Mixer] stop: track %d wrong state\n", track_id);
        return AUDIO_MIXER_STATUS_ERROR;
    }

    // Update active track count
    if (Audio_Mixer.track[track_id].role == AUDIO_MIXER_TRACK_ROLE_MAIN) {
        Audio_Mixer.active_main_track_count--;
    } else if (Audio_Mixer.track[track_id].role == AUDIO_MIXER_TRACK_ROLE_SIDE) {
        Audio_Mixer.active_side_track_count--;
    }

    // Check whether all tracks are inactive
    total_active_track = Audio_Mixer.active_main_track_count + Audio_Mixer.active_side_track_count;
    if (total_active_track == 0) {
        audio_mixer_stop_HAL();
    }

    audio_mixer_deactivate_track(track_id);

    return AUDIO_MIXER_STATUS_OK;
}

audio_mixer_status_t audio_mixer_query_free_count(
    audio_mixer_track_id_t          track_id,
    uint32_t                        *free_space_count)
{ 
    *free_space_count = 0;
    
    // Check ID
    if ( (track_id < 0) || (!(Audio_Mixer.track_flag & (1<<track_id) )) ) {
        log_hal_error("[Mixer] query: track %d isn't valid\n", track_id);
        return AUDIO_MIXER_STATUS_ERROR;
    }

    if (  (!Audio_Mixer.hal_is_pcm_route) && (!Audio_Mixer.hal_is_p2w) && (!Audio_Mixer.hal_is_aws) && (Audio_Mixer.hal_is_runnung) &&
    	    (Audio_Mixer.f_direct_track_buffer) && (Audio_Mixer.track[track_id].role == AUDIO_MIXER_TRACK_ROLE_MAIN) ) {
        // Special case for low power
        hal_audio_get_stream_out_sample_count(free_space_count);
    } else {
        *free_space_count = audio_mixer_bo_get_free_count( &Audio_Mixer.track[track_id].source_buffer );
    }

    return AUDIO_MIXER_STATUS_OK;
}

audio_mixer_status_t audio_mixer_query_data_count(
    audio_mixer_track_id_t          track_id,
    uint32_t                        *data_count)
{
    *data_count = 0;
    
    // Check ID
    if ( (track_id < 0) || (!(Audio_Mixer.track_flag & (1<<track_id) )) ) {
        log_hal_error("[Mixer] query: track %d isn't valid\n", track_id);
        return AUDIO_MIXER_STATUS_ERROR;
    }

    *data_count = audio_mixer_bo_get_data_count( &Audio_Mixer.track[track_id].source_buffer );

    return AUDIO_MIXER_STATUS_OK; 
}

audio_mixer_status_t audio_mixer_write_data(
    audio_mixer_track_id_t          track_id,
    const void                      *buffer,
    uint32_t                        size)
{
    uint32_t free_count = 0;
    uint32_t i;
    track_buffer_t *p_bs;
    uint8_t *p_source_buf = (uint8_t *)buffer;
    
//    log_hal_info("[Mixer] track %d, size %d\n", track_id, size);
    
    Audio_Mixer.track[track_id].waiting = false;
    
    // Special case for low power
    if (  (!Audio_Mixer.hal_is_pcm_route) && (!Audio_Mixer.hal_is_p2w) && (!Audio_Mixer.hal_is_aws) && (Audio_Mixer.hal_is_runnung) &&
    	    (Audio_Mixer.f_direct_track_buffer) && (Audio_Mixer.track[track_id].role == AUDIO_MIXER_TRACK_ROLE_MAIN) ) {
        hal_audio_write_stream_out(buffer, size);
        
        return AUDIO_MIXER_STATUS_OK;
    }
    
    free_count = audio_mixer_bo_get_free_count( &Audio_Mixer.track[track_id].source_buffer );
    
    if (size > free_count) {
        log_hal_error("[Mixer] write error. track %d, free %d, write %d\n", track_id, free_count, size);
        return AUDIO_MIXER_STATUS_ERROR;
    }
    
    p_bs = &Audio_Mixer.track[track_id].source_buffer;
    for (i=0; (i<2) && size; i++) {
        uint8_t *pDesBuf;
        uint32_t buf_size, segment;
        
        audio_mixer_bo_get_free_buffer(p_bs, &pDesBuf, &buf_size);
        if (size >= buf_size) {
            segment = buf_size;
        } else {
            segment = size;
        }
        memcpy(pDesBuf, p_source_buf, segment);
        audio_mixer_bo_write_data_done(p_bs, segment);
        p_source_buf += segment;
        size -= segment;
    }
    
    return AUDIO_MIXER_STATUS_OK;
}

audio_mixer_status_t audio_mixer_set_volume(
    uint16_t                        main_track_gain,
    uint16_t                        side_track_gain)
{
    Audio_Mixer.main_track_gain = (int32_t)main_track_gain;
    Audio_Mixer.side_track_gain = (int32_t)side_track_gain;
    
    return AUDIO_MIXER_STATUS_OK; 
}

void audio_mixer_set_eof(
    audio_mixer_track_id_t          track_id
)
{
    if (Audio_Mixer.track[track_id].eof_state == AUDIO_MIXER_EOF_STATE_NONE) {
        if (Audio_Mixer.f_direct_track_buffer) {
            if (Audio_Mixer.track[track_id].role == AUDIO_MIXER_TRACK_ROLE_MAIN) {
                Audio_Mixer.track[track_id].eof_state = AUDIO_MIXER_EOF_STATE_FLUSH_HAL;
                Audio_Mixer.hal_eof_waiting = true;
                audio_pcm_set_eof();
            } else {
                Audio_Mixer.track[track_id].eof_state = AUDIO_MIXER_EOF_STATE_FLUSH_MIXER;
                Audio_Mixer.track[track_id].eof_remaining_size = audio_mixer_bo_get_data_count( &Audio_Mixer.mixer_buffer );
            }
        } else {
            Audio_Mixer.track[track_id].eof_state = AUDIO_MIXER_EOF_STATE_FLUSH_TRACK;
            Audio_Mixer.track[track_id].eof_remaining_size = audio_mixer_bo_get_data_count( &Audio_Mixer.track[track_id].source_buffer );
        }
        
        log_hal_info("[Mixer] set EOF. Remaining %d \n", Audio_Mixer.track[track_id].eof_remaining_size);
    }
}

void audio_mixer_p2w_isr(uint8_t *p_acc_buf, uint32_t len)
{
    if (Audio_Mixer.mixer_buffer.base_address_alloc == NULL)
    {
        // Need not to mix
        return;
    }

//    log_hal_info("[Mixer] P2W ISR\n");

    audio_mixer_merge_buffer(p_acc_buf, len);

    audio_mixer_ISR_update_eof_count(len);

    // If mixer data isn't enough, notify task to process
    {
        uint32_t mixer_free_space;
        
        mixer_free_space = audio_mixer_bo_get_free_count( &Audio_Mixer.mixer_buffer );
        if ( (mixer_free_space >= Audio_Mixer.mixer_block_size) && (!Audio_Mixer.hal_callback_waiting) ) {
            Audio_Mixer.hal_callback_waiting = true;
            auddio_mixer_send_event(AUDIO_MIXER_QUEUE_EVENT_PROCESS, NULL);
        }
    }
}
