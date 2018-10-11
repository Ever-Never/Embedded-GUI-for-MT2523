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

#include "hal_audio.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

#include "hal_audio_internal_pcm.h"
#include "hal_audio_enhancement.h"
#include "hal_audio_internal_afe.h"
#include "hal_audio_fw_sherif.h"
#include "hal_audio_fw_interface.h"
#include "hal_audio_internal_pcm2way.h"
#include "hal_audio_lzma_decode_interface.h"

#ifdef HAL_AUDIO_LOW_POWER_ENABLED
#include "hal_audio_low_power.h"
#endif

pcm_playback_internal_t pcm_playback_internal_handle;
pcm_record_internal_t   pcm_record_internal_handle;
extern hal_audio_stream_t audio_config;
extern audio_common_t audio_common;
uint32_t *lzma_working_buffer;
uint16_t pcm_nb_stream_in_buffer [NB_BUFFER_SIZE];
uint16_t pcm_wb_stream_in_buffer [WB_BUFFER_SIZE];
uint16_t pcm_nb_stream_in_buffer_sec_mic [NB_BUFFER_SIZE];
uint16_t pcm_wb_stream_in_buffer_sec_mic [WB_BUFFER_SIZE];
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
static uint16_t last_sample = 0;
static uint16_t second_last_sample = 0;
#endif

hal_audio_status_t audio_pcm_set_memory(uint8_t *memory)
{
    if (memory == NULL) {
        log_hal_info("audio_pcm_set_memory: error: address should not be NULL\r\n");
        return HAL_AUDIO_STATUS_INVALID_PARAMETER;
    }
    /*address should be 4 bytes aligned*/
    if ((((uint32_t)memory) & 0x3) != 0) {
        log_hal_info("audio_pcm_set_memory: error: address should be 4 bytes aligned\r\n");
        return HAL_AUDIO_STATUS_INVALID_PARAMETER;
    }
    /*assign memory pool memory*/
    /*internal buffer*/
#ifdef __DSP_COMPRESS_7Z_
    {
        hal_audio_LZMA_decode_set_memory(memory);
        memory += LZMA_INTERNAL_MEMORY_SIZE;
    }
    {
        lzma_working_buffer = (uint32_t *)memory;
        memory += LZMA_BUFFER_MEMORY_SIZE;
    }
#endif
    {
        ring_buffer_information_t *p_mcu_info = &pcm_record_internal_handle.mcu_buffer_info;
        p_mcu_info->read_pointer        = 0;
        p_mcu_info->write_pointer       = 0;
        p_mcu_info->buffer_byte_count   = AUDIO_PCM_STREAM_IN_MEMORY_SIZE;
        p_mcu_info->buffer_base_pointer = memory;
        memory += AUDIO_PCM_STREAM_IN_MEMORY_SIZE;
    }
    {
        ring_buffer_information_t *p_mcu_info = &pcm_playback_internal_handle.mcu_buffer_info;
        p_mcu_info->read_pointer        = 0;
        p_mcu_info->write_pointer       = 0;
        p_mcu_info->buffer_byte_count   = AUDIO_PCM_STREAM_OUT_MEMORY_SIZE;
        p_mcu_info->buffer_base_pointer = memory;
        memory += AUDIO_PCM_STREAM_OUT_MEMORY_SIZE;
    }

    return HAL_AUDIO_STATUS_OK;
}

uint32_t audio_pcm_get_memory_size(void)
{
    return (AUDIO_PCM_MEMORY_SIZE);
}

void audio_pcm_register_stream_out_callback(hal_audio_stream_out_callback_t callback, void *user_data)
{
    pcm_playback_internal_handle.callback = callback;
    pcm_playback_internal_handle.user_data = user_data;
}

void audio_pcm_register_stream_in_callback(hal_audio_stream_in_callback_t callback, void *user_data)
{
    pcm_record_internal_handle.callback = callback;
    pcm_record_internal_handle.user_data = user_data;
}

/*MCU buffer operations*/
void audio_pcm_get_stream_out_free_space(uint32_t *sample_count) //[in bytes]
{
    *sample_count = ring_buffer_get_space_byte_count(&pcm_playback_internal_handle.mcu_buffer_info);
}

void audio_pcm_copy_stream_out_enable(hal_audio_stream_copy_callback_t callback, void *user_data)
{
    pcm_playback_internal_handle.copy_user_data = user_data;
    pcm_playback_internal_handle.copy_callback  = callback;
}

hal_audio_status_t audio_pcm_stream_out_write(const void *buffer, uint32_t size) //[size: bytes]
{
    ring_buffer_information_t *p_mcu_info = &pcm_playback_internal_handle.mcu_buffer_info;
    hal_audio_status_t result;
    uint32_t space_byte_count = ring_buffer_get_space_byte_count(p_mcu_info);
    if (space_byte_count >= size) {
        uint8_t *p_dst = NULL;
        uint8_t *p_src = (uint8_t *)buffer;
        uint32_t data_byte_count = size;
        uint32_t write_byte_count = 0;
        uint32_t loop_idx;
        for (loop_idx = 0; loop_idx < 2; loop_idx++) {
            ring_buffer_get_write_information(p_mcu_info, &p_dst, &write_byte_count);
            write_byte_count = MINIMUM(write_byte_count, data_byte_count);
            memcpy(p_dst, p_src, write_byte_count);
            ring_buffer_write_done(p_mcu_info, write_byte_count);
            data_byte_count -= write_byte_count;
            p_src += write_byte_count;
        }
        pcm_playback_internal_handle.waiting = false;
        pcm_playback_internal_handle.underflow = false;
        result = HAL_AUDIO_STATUS_OK;
    } else {
        result = HAL_AUDIO_STATUS_ERROR;
    }
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    if (ring_buffer_get_data_byte_count(p_mcu_info) >= 4) {
        audio_get_last_two_write_sample(p_mcu_info, &last_sample, &second_last_sample);
    }
#endif

    // handle copy flow, temp
    if ( pcm_playback_internal_handle.copy_callback != NULL ) {
        hal_audio_copy_stream_t event;
        event.user_data   = pcm_playback_internal_handle.copy_user_data;
        event.buffer      = (uint8_t *)buffer;
        event.buffer_size = size;
        event.sample_rate = (uint32_t)pcm_playback_internal_handle.sample_rate;
        event.channel     = (uint32_t)pcm_playback_internal_handle.channel;
        pcm_playback_internal_handle.copy_callback(HAL_AUDIO_EVENT_DATA_NOTIFICATION, &event);
    }

    return result;
}

hal_audio_status_t audio_pcm_stream_in_read(const void *buffer, uint32_t size) //[size: bytes]
{
    hal_audio_status_t result;
    ring_buffer_information_t *p_mcu_info = &pcm_record_internal_handle.mcu_buffer_info;
    uint32_t read_byte_count = 0;
    uint8_t *p_src = NULL;
    uint8_t *p_dst = (uint8_t *)buffer;
    uint32_t data_byte_count = size;
    for (uint32_t loop_idx = 0; loop_idx < 2; loop_idx++) {
        ring_buffer_get_read_information(p_mcu_info, (uint8_t **)&p_src, &read_byte_count);
        read_byte_count = MINIMUM(read_byte_count, data_byte_count);
        memcpy(p_dst, p_src, read_byte_count);
        ring_buffer_read_done(p_mcu_info, read_byte_count);
        data_byte_count -= read_byte_count;
        p_dst += read_byte_count;
    }
    pcm_record_internal_handle.waiting = false;
    result = HAL_AUDIO_STATUS_OK;
    return result;
}

void audio_pcm_set_eof(void)
{
    pcm_playback_internal_handle.end_of_data = true;
}

static void audio_pcm_set_interrupt_sample(uint16_t sample)
{
    *DSP_AUDIO_DEL_CNTR_THRESHOLD = sample;
}

static void audio_pcm_playback_init_dsp(void)
{
    if (pcm_playback_internal_handle.state == PCM_STATE_INIT) {
        ring_buffer_information_t *p_dsp_info = &pcm_playback_internal_handle.dsp_buffer_info;
        p_dsp_info->read_pointer        = 0;
        p_dsp_info->write_pointer       = 0;
        p_dsp_info->buffer_byte_count   = ASP_FRAMELEN_PCM << 1;
        p_dsp_info->buffer_base_pointer = (uint8_t *)DSP_DM_ADDR(PCM_PLAYBACK_DSP_PAGE_NUM, ASP_DSP_PCM_BUFFER_BASE_PCM);
        *DSP_ASP_WAV_W_IO_ADDR = ASP_DSP_PCM_BUFFER_BASE_PCM;
        *DSP_ASP_WAV_R_IO_ADDR = ASP_DSP_PCM_BUFFER_BASE_PCM;
        pcm_playback_internal_handle.state = PCM_STATE_RUNNING;
    }
}

static void audio_pcm_set_interrupt_time(uint32_t msec)
{
    uint32_t sample = (uint32_t)pcm_playback_internal_handle.sample_rate * pcm_playback_internal_handle.channel * msec / 1000;
    audio_pcm_set_interrupt_sample((uint16_t)sample);
}

static uint32_t audio_pcm_dsp_get_data_count(void)
{
    uint32_t current_read, current_write, data_count;
    current_read  = (uint32_t) * DSP_ASP_WAV_R_IO_ADDR;
    current_write = (uint32_t) * DSP_ASP_WAV_W_IO_ADDR;
    if (current_write >= current_read) {
        data_count = current_write - current_read;
    } else {
        data_count = ASP_FRAMELEN_PCM + current_write - current_read;
    }
    return data_count;
}

static void audio_pcm_dsp_get_write_buffer(uint16_t *write, uint32_t *length)
{
    uint32_t current_read, current_write, dsp_write_length;
    current_read  = (uint32_t) * DSP_ASP_WAV_R_IO_ADDR;
    current_write = (uint32_t) * DSP_ASP_WAV_W_IO_ADDR;
    if (current_read > current_write) {
        dsp_write_length = current_read - current_write - 1;
    } else {
        dsp_write_length = ASP_DSP_PCM_BUFFER_BASE_PCM + ASP_FRAMELEN_PCM - current_write;
        if (current_read == ASP_DSP_PCM_BUFFER_BASE_PCM) {
            dsp_write_length--;
        }
    }
    *length = dsp_write_length & ~0x1; /* For L / R pair */
    *write = (uint16_t)current_write;
}

static void audio_pcm_dsp_write_data_done(uint32_t length)
{
    uint16_t current_write = *DSP_ASP_WAV_W_IO_ADDR;
    current_write += length;
    if (current_write >= ASP_DSP_PCM_BUFFER_BASE_PCM + ASP_FRAMELEN_PCM) {
        current_write = ASP_DSP_PCM_BUFFER_BASE_PCM;
    }
    *DSP_ASP_WAV_W_IO_ADDR = current_write;
}

static uint32_t audio_pcm_write_data_to_dsp(uint16_t *source, uint32_t source_word, bool mono_to_stereo)
{
    volatile uint16_t *dsp_addptr;
    uint32_t dsp_word = 0, use_word = 0;
    uint16_t dsp_write_position = 0;
    if (source_word > 0) {
        audio_pcm_dsp_get_write_buffer(&dsp_write_position, &dsp_word);
        dsp_addptr = DSP_DM_ADDR(PCM_PLAYBACK_DSP_PAGE_NUM, dsp_write_position);
        if (mono_to_stereo) {
            dsp_word = MINIMUM(dsp_word, source_word << 1);
        } else {
            source_word &= ~0x1;
            dsp_word = MINIMUM(dsp_word, source_word);
        }
        if (dsp_word > 0) {
            if (mono_to_stereo) {
                audio_idma_write_to_dsp_duplicate(dsp_addptr, source, dsp_word);
                use_word = dsp_word >> 1;
            } else {
                audio_idma_write_to_dsp(dsp_addptr, source, dsp_word);
                use_word = dsp_word;
            }
            audio_pcm_dsp_write_data_done(dsp_word);
        }
    }
    return use_word;
}

#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
static uint32_t audio_pcm_sd_write_data_to_dsp(uint16_t *source, uint32_t source_word, bool mono_to_stereo)
{
    volatile uint16_t *p_dsp_buf = NULL;
    ring_buffer_information_t *p_dsp_info = &pcm_playback_internal_handle.dsp_buffer_info;
    uint32_t read_byte_cnt  = source_word << 1;
    uint32_t write_byte_cnt = 0;
    uint32_t used_word_cnt = 0;
    uint32_t move_byte_cnt = 0;
    uint32_t move_word_cnt = 0;
    p_dsp_info->read_pointer = *DSP_PCM_SD_PLAYBACK_DM_DSP_R_PTR;
    ring_buffer_get_write_information(p_dsp_info, (uint8_t **)&p_dsp_buf, &write_byte_cnt);
    write_byte_cnt &= ~0x1;     /* Make it even */
    {
        if (mono_to_stereo) {
            read_byte_cnt <<= 1;
        }
        move_byte_cnt = MINIMUM(write_byte_cnt, read_byte_cnt);
        move_word_cnt = move_byte_cnt >> 1;
        used_word_cnt = mono_to_stereo ? move_word_cnt >> 1 : move_word_cnt;
    }
    if (move_word_cnt > 0) {    /* Move data */
        if (mono_to_stereo) {
            audio_idma_write_to_dsp_duplicate(p_dsp_buf, source, move_word_cnt);
            move_word_cnt >>= 1;
        } else {
            audio_idma_write_to_dsp(p_dsp_buf, source, move_word_cnt);
        }
    }
    ring_buffer_write_done(p_dsp_info, move_byte_cnt);
    *DSP_PCM_SD_PLAYBACK_DM_MCU_W_PTR = p_dsp_info->write_pointer;
    return used_word_cnt;
}
#endif

void audio_pcm_put_audio_data(bool mono_to_stereo)
{
    ring_buffer_information_t *p_mcu_info = &pcm_playback_internal_handle.mcu_buffer_info;
    uint16_t *p_src;
    uint32_t move_word_cnt = 0, read_byte_count = 0, dsp_use_word = 0;
    uint32_t consumed_byte_count;
    uint32_t total_consumed_byte_count = 0;
    uint32_t loop_idx;
    uint32_t loop_cnt = 4;
    for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
        ring_buffer_get_read_information(p_mcu_info, (uint8_t **)&p_src, &read_byte_count);
        read_byte_count &= ~0x1;     //make it even
        move_word_cnt = read_byte_count >> 1;
        if (move_word_cnt > 0) {
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
            if (pcm_playback_internal_handle.is_sd_task) {
                dsp_use_word = audio_pcm_sd_write_data_to_dsp(p_src, move_word_cnt, mono_to_stereo);
            } else {
                dsp_use_word = audio_pcm_write_data_to_dsp(p_src, move_word_cnt, mono_to_stereo);
            }
#else
            dsp_use_word = audio_pcm_write_data_to_dsp(p_src, move_word_cnt, mono_to_stereo);
#endif
            consumed_byte_count = dsp_use_word << 1;
            total_consumed_byte_count += consumed_byte_count;
            ring_buffer_read_done(p_mcu_info, consumed_byte_count);
        } else {    // Read buffer empty
            break;
        }
    }


#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    uint32_t sd_frame_len_byte = audio_common.sd_frame_len << 1;
    ring_buffer_information_t *p_dsp_info = &pcm_playback_internal_handle.dsp_buffer_info;
    if (pcm_playback_internal_handle.is_sd_task) {
        if (total_consumed_byte_count < sd_frame_len_byte) {
            uint32_t remain_byte_cnt = sd_frame_len_byte - total_consumed_byte_count;
            audio_sd_write_sample_to_dsp(p_dsp_info, remain_byte_cnt, last_sample, second_last_sample, mono_to_stereo);
        }
    }
#endif

}

#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
void audio_pcm_sd_playback_hisr(void)
{
    ring_buffer_information_t *p_mcu_info = &pcm_playback_internal_handle.mcu_buffer_info;
    uint8_t *p_src = NULL;
    uint32_t dsp_word;
    
    if (pcm_playback_internal_handle.state == PCM_STATE_RUNNING) {
        audio_pcm_put_audio_data(pcm_playback_internal_handle.channel == 1);
        if (!pcm_playback_internal_handle.waiting) {
            pcm_playback_internal_handle.waiting = true;
            pcm_playback_internal_handle.callback(HAL_AUDIO_EVENT_DATA_REQUEST, pcm_playback_internal_handle.user_data);
        }
        if( pcm_playback_internal_handle.end_of_data == true ) {
            ring_buffer_get_read_information(p_mcu_info, (uint8_t **)&p_src, &dsp_word);
            log_hal_info("HAL_AUDIO_EVENT_END: dsp_word=%d\r\n",dsp_word);
            if (dsp_word < 20) { //DSP will not play if the data is less than 12 sample
                pcm_playback_internal_handle.callback(HAL_AUDIO_EVENT_END, pcm_playback_internal_handle.user_data);
            }
        }
    }
    *DSP_TASK4_COSIM_HANDSHAKE = 0;
    return;
}
#endif

void audio_pcm_playback_hisr(void)
{
    uint32_t buffer_time;
    bool is_dsp_underflow = false;
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    audio_lowpower_set_26mclock(false);
#endif
    *DSP_TASK4_COSIM_HANDSHAKE = 0;
    if (pcm_playback_internal_handle.state != PCM_STATE_RUNNING && pcm_playback_internal_handle.state != PCM_STATE_FLUSHED) {
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
        audio_lowpower_set_26mclock(true);
#endif
        return;
    }
    ring_buffer_information_t *p_mcu_info = &pcm_playback_internal_handle.mcu_buffer_info;
    uint16_t *p_src;
    uint32_t read_byte_count = 0;
    ring_buffer_get_read_information(p_mcu_info, (uint8_t **)&p_src, &read_byte_count);
    if (pcm_playback_internal_handle.state == PCM_STATE_RUNNING) {
        audio_pcm_put_audio_data(pcm_playback_internal_handle.channel == 1);
        if (!pcm_playback_internal_handle.waiting) {
            pcm_playback_internal_handle.waiting = true;
            if (is_dsp_underflow) {
                pcm_playback_internal_handle.underflow = true;
                pcm_playback_internal_handle.callback(HAL_AUDIO_EVENT_UNDERFLOW, pcm_playback_internal_handle.user_data);
            } else {
                pcm_playback_internal_handle.callback(HAL_AUDIO_EVENT_DATA_REQUEST, pcm_playback_internal_handle.user_data);
            }
        }
        if(read_byte_count<10 && pcm_playback_internal_handle.end_of_data ) {
            uint32_t dsp_word = audio_pcm_dsp_get_data_count();
            log_hal_info("HAL_AUDIO_EVENT_END: dsp_word=%d",dsp_word);
            if (dsp_word < 16) { //DSP will not play if the data is less than 12 sample
                pcm_playback_internal_handle.callback(HAL_AUDIO_EVENT_END, pcm_playback_internal_handle.user_data);
            }
        }
    }
    buffer_time = audio_pcm_dsp_get_data_count() * 1000 / (pcm_playback_internal_handle.sample_rate << 1);
    if (buffer_time < (PCM_SOUND_INTERRUPT_PERIOD + 20)) {
        audio_pcm_set_interrupt_time(PCM_SOUND_INTERRUPT_PERIOD);
    } else {
        audio_pcm_set_interrupt_time(buffer_time - PCM_SOUND_INTERRUPT_PERIOD);
    }
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    audio_lowpower_set_26mclock(true);
#endif
}

void audio_pcm_set_dsp_sd_task_playback(uint16_t frame_length)
{
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    ring_buffer_information_t *p_dsp_info = &pcm_playback_internal_handle.dsp_buffer_info;
    p_dsp_info->read_pointer        = 0;
    p_dsp_info->write_pointer       = 0;
    p_dsp_info->buffer_byte_count   = frame_length << 2;
    p_dsp_info->buffer_base_pointer = (uint8_t *)DSP_DM_ADDR(PCM_SD_PLAYBACK_DSP_PAGE, PCM_SD_PLAYBACK_DSP_ADDR);
    {   /* Clear buffer */
        volatile uint16_t *p_buf = (volatile uint16_t *)p_dsp_info->buffer_base_pointer;
        int32_t frm_len;
        for (frm_len = 0; frm_len < (frame_length << 1); frm_len++) {
            *p_buf++ = 0;
        }
    }
    ring_buffer_write_done(p_dsp_info, frame_length << 2);
    *DSP_PCM_SD_PLAYBACK_DM_MCU_W_PTR = p_dsp_info->write_pointer;
    *DSP_PCM_SD_PLAYBACK_CTRL = PCM_SD_PLAYBACK_STATE_START;
    *DSP_AUDIO_FLEXI_CTRL |= (FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);
    pcm_playback_internal_handle.is_sd_task = 1;
#endif
    return;
}

void audio_pcm_stop_dsp_sd_task_playback(void)
{
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    {   /* Start --> Running */
        int32_t loop_cnt = 0;
        uint16_t ctrl_reg;
        do {
            ctrl_reg = *DSP_PCM_SD_PLAYBACK_CTRL;
            if ((ctrl_reg & PCM_SD_PLAYBACK_STATE_MASK) == PCM_SD_PLAYBACK_STATE_RUNNING) {
                break;
            }
            loop_cnt++;
            if (loop_cnt >= 100) {
                log_hal_info("pcm_sd start error\r\n");
                break;
            }
            hal_gpt_delay_ms(3);
        } while ((ctrl_reg & PCM_SD_PLAYBACK_STATE_MASK) == PCM_SD_PLAYBACK_STATE_START);
    }
    {   /* Running --> Flush */
        uint16_t ctrl_reg = *DSP_PCM_SD_PLAYBACK_CTRL;
        if ((ctrl_reg & PCM_SD_PLAYBACK_STATE_MASK) == PCM_SD_PLAYBACK_STATE_RUNNING) {
            ctrl_reg &= PCM_SD_PLAYBACK_STATE_CLR;
            ctrl_reg |= PCM_SD_PLAYBACK_STATE_FLUSH;
            *DSP_PCM_SD_PLAYBACK_CTRL = ctrl_reg;
        }
    }
    {   /* Flush --> Idle */
        int32_t loop_cnt = 0;
        uint16_t ctrl_reg;
        do {
            ctrl_reg = *DSP_PCM_SD_PLAYBACK_CTRL;
            if ((ctrl_reg & PCM_SD_PLAYBACK_STATE_MASK) == PCM_SD_PLAYBACK_STATE_IDLE) {
                break;
            }
            loop_cnt++;
            if (loop_cnt >= 100) {
                log_hal_info("pcm_sd flush error\r\n");
                break;
            }
            hal_gpt_delay_ms(3);
        } while ((ctrl_reg & PCM_SD_PLAYBACK_STATE_MASK) == PCM_SD_PLAYBACK_STATE_FLUSH);
    }
#endif
    return;
}

hal_audio_status_t audio_pcm_play_open_device(void)
{
    if (pcm_playback_internal_handle.state != PCM_STATE_RUNNING) {
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
        pcm_playback_internal_handle.is_sd_task = 0;
#endif
        pcm_playback_internal_handle.state = PCM_STATE_INIT;
        pcm_playback_internal_handle.audio_id = audio_get_id();
        pcm_playback_internal_handle.channel = audio_transfer_channel_number(audio_config.stream_out.stream_channel);
        pcm_playback_internal_handle.sample_rate = audio_transfer_sample_rate(audio_config.stream_out.stream_sampling_rate);
        pcm_playback_internal_handle.end_of_data = false;
        if (pcm_playback_internal_handle.audio_id > MAX_AUDIO_FUNCTIONS) {
            return HAL_AUDIO_STATUS_ERROR;
        }
        audio_service_hook_isr(DSP_D2M_PCM_INT, (isr)audio_pcm_playback_hisr, NULL);
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
        audio_service_hook_isr(DSP_D2C_PCM_SD_PLAYBACK_INT, (isr)audio_pcm_sd_playback_hisr, NULL);
#endif
        audio_service_setflag(pcm_playback_internal_handle.audio_id);
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
        if (audio_lowpower_check_status()) {
            audio_lowpower_set_clock_mode(true);
            audio_lowpower_sw_trigger_event_set(true);
            audio_lowpower_set_26mclock(true);
        }
#endif
        audio_clear_dsp_common_flag();
        dsp_audio_fw_dynamic_download(DDID_AUDIO_HEADER);
        audio_pcm_playback_init_dsp();
        audio_pcm_set_interrupt_sample(0);
        audio_playback_on(ASP_TYPE_PCM_HI, audio_config.stream_out.stream_sampling_rate);
    } else {
        return HAL_AUDIO_STATUS_ERROR;
    }
    return HAL_AUDIO_STATUS_OK;
}

void audio_pcm_play_close_device(void)
{
    pcm_playback_internal_handle.state = PCM_STATE_STOP;
    audio_playback_off();
    audio_service_unhook_isr(DSP_D2M_PCM_INT);
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    audio_service_unhook_isr(DSP_D2C_PCM_SD_PLAYBACK_INT);
#endif
    audio_service_clearflag(pcm_playback_internal_handle.audio_id);
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    if (audio_lowpower_check_status()) {
        audio_lowpower_set_26mclock(false);
        audio_lowpower_sw_trigger_event_set(false);
        audio_lowpower_set_clock_mode(false);
    }
#endif
    pcm_playback_internal_handle.state = PCM_STATE_IDLE;

    pcm_playback_internal_handle.mcu_buffer_info.read_pointer = 0;
    pcm_playback_internal_handle.mcu_buffer_info.write_pointer = 0;
    pcm_playback_internal_handle.dsp_buffer_info.read_pointer = 0;
    pcm_playback_internal_handle.dsp_buffer_info.write_pointer = 0;

    pcm_playback_internal_handle.waiting = false;
    pcm_playback_internal_handle.end_of_data = false;
    audio_free_id(pcm_playback_internal_handle.audio_id);
}

bool pcm_record_is_record_silence(void)
{
    if (pcm_record_internal_handle.presilence > RECORD_PRESILENCE_NUM) {
        return false;
    }
    pcm_record_internal_handle.presilence++;
    return true;
}

void audio_pcm_get_stream_in_record_size(uint32_t *sample_count) //[in bytes]
{
    *sample_count = ring_buffer_get_data_byte_count(&pcm_record_internal_handle.mcu_buffer_info);
}

void audio_pcm_record_mediadata_notification()
{
    if (!pcm_record_internal_handle.waiting) {
        pcm_record_internal_handle.waiting = true;
        pcm_record_internal_handle.callback(HAL_AUDIO_EVENT_DATA_NOTIFICATION, pcm_record_internal_handle.user_data);
    }
}

static bool audio_pcm_is_single_mic(void)
{
    bool result;
    hal_audio_device_t in_device = audio_config.stream_in.audio_device;
    if (in_device == HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC || in_device == HAL_AUDIO_DEVICE_MAIN_MIC || in_device == HAL_AUDIO_DEVICE_HEADSET_MIC) {
        result = true;
    } else {
        result = false;
    }
    return result;
}

void audio_pcm_record_hisr()
{
    uint32_t sample_count;
    hal_audio_sampling_rate_t sampling_rate = audio_config.stream_in.stream_sampling_rate;
    bool is_single_mic = audio_pcm_is_single_mic();
    if (sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
        sample_count = WB_BUFFER_SIZE;
    } else {
        sample_count = NB_BUFFER_SIZE;
    }
    if (!pcm_record_is_record_silence()) {
        {   /* DSP Buffer --> Frame Buffer */
            {   /* Mic 1 */
                uint16_t *ptr_in_buffer_ch1 = sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ ? pcm_wb_stream_in_buffer : pcm_nb_stream_in_buffer;
                volatile uint16_t *dspAddr_ch1 = DSP_DM_ADDR(5, *(DSP_DM_ADDR(5, *DSP_SPH_DM_ADDR_SE_PTR)));
                uint32_t I = sample_count;
                while (I-- > 0) {
                    *ptr_in_buffer_ch1++ = *dspAddr_ch1++;
                }
            }
            if (!is_single_mic) {  /* Mic 2 */
                uint16_t *ptr_in_buffer_ch2 = sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ ? pcm_wb_stream_in_buffer_sec_mic : pcm_nb_stream_in_buffer_sec_mic;
                volatile uint16_t *dspAddr_ch2 = DSP_DM_ADDR(5, *DSP_DM_ADDR(5, DSP_SPH_SE2_PTR));
                uint32_t I = sample_count;
                while (I-- > 0) {
                    *ptr_in_buffer_ch2++ = *dspAddr_ch2++;
                }
            }
        }
        {   /* Frame Buffer --> Ring Buffer */
            ring_buffer_information_t *p_mcu_info = &pcm_record_internal_handle.mcu_buffer_info;
            uint16_t *ptr_in_buffer_ch1;
            uint16_t *ptr_in_buffer_ch2;
            uint32_t loop_idx;
            uint32_t channel_number = is_single_mic ? 1 : 2;
            uint32_t data_byte_count = sample_count * channel_number * sizeof(uint16_t);
            if (sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
                ptr_in_buffer_ch1 = pcm_wb_stream_in_buffer;
                ptr_in_buffer_ch2 = pcm_wb_stream_in_buffer_sec_mic;
            } else {
                ptr_in_buffer_ch1 = pcm_nb_stream_in_buffer;
                ptr_in_buffer_ch2 = pcm_nb_stream_in_buffer_sec_mic;
            }
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                uint32_t write_byte_count = 0;
                uint16_t *p_dst = NULL;
                ring_buffer_get_write_information(p_mcu_info, (uint8_t **)&p_dst, &write_byte_count);
                write_byte_count = MINIMUM(write_byte_count, data_byte_count);
                if (is_single_mic) {    /* Single Mic */
                    memcpy(p_dst, ptr_in_buffer_ch1, write_byte_count);
                    ptr_in_buffer_ch1 += write_byte_count >> 1;
                } else {    /* Dual Mic */
                    uint32_t smpl_idx;
                    for (smpl_idx = 0; smpl_idx < write_byte_count >> 2; smpl_idx++) {
                        *p_dst++ = *ptr_in_buffer_ch1++;
                        *p_dst++ = *ptr_in_buffer_ch2++;
                    }
                }
                ring_buffer_write_done(p_mcu_info, write_byte_count);
                data_byte_count -= write_byte_count;
            }
            {   /* Check threshold for notification */
                uint32_t byte_count = ring_buffer_get_data_byte_count(p_mcu_info);
                uint32_t threshold = sample_count * channel_number * sizeof(uint16_t);
                if (byte_count >= threshold) {
                    audio_pcm_record_mediadata_notification();
                }
            }
        }
    }
    return;
}

void audio_pcm_record_on()
{
    if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
        *DSP_SPH_PCM_REC_CTRL |= 0x0001;
        *DSP_SPH_SCH_IMPROVE_CTRL |= 0x0030; //fix width band SCH-BAND
        *DSP_SPH_SFE_CTRL = 0x0;
        *DSP_SPH_8K_CTRL = 0x8008; //COD_BAND
        *AFE_VMCU_CON1 &= ~0x0200;
        audio_dsp_speech_set_delay(1);
    } else if (audio_pcm_is_single_mic()) {
        *DSP_SPH_PCM_REC_CTRL |= 0x0001;
        *DSP_SPH_SCH_IMPROVE_CTRL |= 0x0030; //fix width band SCH-BAND
        *DSP_SPH_SFE_CTRL = 0x0;
        *DSP_SPH_8K_CTRL |= 0x8000; //COD_BAND
        *AFE_VMCU_CON1 &= ~0x0200;
        *DSP_SPH_MUTE_CTRL |= 0x0002;
        *DSP_SC_MODE =  0x0000;
        *DSP_SPH_COD_CTRL |= 0x0022;
        audio_dsp_speech_set_delay(0);
    } else {
        *DSP_SPH_PCM_REC_CTRL |= 0x0001;
        *DSP_SPH_SCH_IMPROVE_CTRL &= ~0x0030;
        *DSP_SPH_SFE_CTRL = 0;
        *DSP_SPH_8K_CTRL = 0x8000; //COD_BAND
        *AFE_VMCU_CON1 |= 0x0200;
        *DSP_SPH_MUTE_CTRL |= 0x0002;
        *DSP_SC_MODE =  0x0000;
        *DSP_SPH_COD_CTRL |= 0x0022;
        audio_dsp_speech_set_delay(0);
    }
    afe_voice_digital_on();
    VBI_RESET();
}

void audio_pcm_record_off()
{
    if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
        *DSP_SPH_MUTE_CTRL &= ~0x0002;
        *DSP_SPH_COD_CTRL = 0x0000;
    } else {
        *DSP_SPH_PCM_REC_CTRL &= ~0x0001;
        *DSP_SPH_COD_CTRL &= ~0x0022;
        *DSP_SPH_MUTE_CTRL &= ~0x0002;
        *DSP_SPH_FLT_COEF_ADDR_SRC = 0;
    }
    VBI_END();
    afe_audio_analog_device_off();
    afe_voice_digital_off();
}

void audio_pcm_record_handle(bool enable)
{
    if (enable) {
        pcm_record_internal_handle.audio_id = audio_get_id();
        audio_service_setflag(pcm_record_internal_handle.audio_id);
        pcm_record_internal_handle.presilence = 0;
        audio_service_hook_isr(DSP_PCM_REC_INT, (isr)audio_pcm_record_hisr, NULL);
        audio_pcm_record_on();
        if (audio_pcm_is_single_mic()) {
            speech_set_mode(SPH_MODE_RECORD);
            speech_set_enhancement(true);
        }
    } else {
        if (audio_pcm_is_single_mic()) {
            speech_set_enhancement(false);
        }
        audio_pcm_record_off();
        audio_service_unhook_isr(DSP_PCM_REC_INT);
        audio_service_clearflag(pcm_record_internal_handle.audio_id);
        audio_free_id(pcm_record_internal_handle.audio_id);
    }
}

#endif /*HAL_AUDIO_MODULE_ENABLED*/
