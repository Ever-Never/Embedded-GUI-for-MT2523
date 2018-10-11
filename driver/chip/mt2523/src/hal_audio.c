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

#include "hal_audio.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

#include "hal_audio_internal_pcm2way.h"
#include "hal_audio_internal_pcm.h"
#include "hal_audio_internal_service.h"
#include "hal_audio_internal_afe.h"
#include "hal_audio_enhancement.h"
#include "hal_audio_post_processing_internal.h"
#include "hal_audio_fw_interface.h"
#include "hal_dsp_topsm.h"
#include "hal_nvic.h"
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
#include "hal_audio_low_power.h"
#endif
#include <stdio.h>
#include <stdbool.h>

/**
  * @ Initialize audio HW setting and audio SW structure when system boot up
  * @ Retval: true if operation success, false if operation fail
  */

hal_audio_stream_t audio_config;
extern afe_t afe;
extern audio_common_t audio_common;
hal_audio_status_t hal_audio_init(void)
{
    if (audio_common.init) {
        return HAL_AUDIO_STATUS_OK;
    }
    hal_dsp_topsm_get_resource(RM_MODEM_DSP_3, true);
    dsp_audio_fw_reboot();
    hal_nvic_register_isr_handler(DSP22CPU_IRQn, (hal_nvic_isr_t)audio_service_dsp_to_mcu_interrupt);
    hal_nvic_enable_irq(DSP22CPU_IRQn);
    audio_common.running_state = false;
    afe_audio_initial();
    speech_init();
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
    audio_lowpower_init();
#endif
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    audio_post_processing_init();
#endif
#if defined(__HAL_AUDIO_AWS_SUPPORT__)
    audio_service_aws_set_flag(false);
    audio_service_aws_set_clock_skew(true);
#endif  /* __HAL_AUDIO_AWS_SUPPORT__ */
    audio_service_dsp_memory_dump_init();
    hal_dsp_topsm_get_resource(RM_MODEM_DSP_3, false);
    audio_common.init = true;
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_deinit(void)
{
    afe_audio_deinit();
    audio_common.init = false;
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_register_copied_stream_out_callback(hal_audio_stream_copy_callback_t callback, void *user_data)
{
    audio_pcm_copy_stream_out_enable(callback, user_data);
    return HAL_AUDIO_STATUS_OK;
}

/**
  * @ Power on analog and digital audio hardware for audio out
  */
hal_audio_status_t hal_audio_start_stream_out(hal_audio_active_type_t active_type)
{
    hal_audio_status_t result = HAL_AUDIO_STATUS_OK;
    audio_config.audio_path_type = active_type;
    if (active_type == HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE) { /* stream in sampling rate fixed to 16kHz due to hardware limitation */
        if (audio_config.stream_out.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_8KHZ ||
            audio_config.stream_out.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_11_025KHZ ||
            audio_config.stream_out.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_12KHZ) {    /* Apply downlink SRC */
            audio_config.stream_out.stream_sampling_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
        }
    }
    if (active_type == HAL_AUDIO_PLAYBACK_MUSIC || active_type == HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE) {
        afe_set_path_type(HAL_AUDIO_PLAYBACK_MUSIC);
        result = audio_pcm_play_open_device();
    } else if (active_type == HAL_AUDIO_PLAYBACK_VOICE || active_type == HAL_AUDIO_TWO_WAY_VOICE) {
        if (audio_config.stream_in.stream_sampling_rate != audio_config.stream_out.stream_sampling_rate && active_type == HAL_AUDIO_TWO_WAY_VOICE) {
            result = HAL_AUDIO_STATUS_INVALID_PARAMETER;
        } else {
            audio_pcm2way_play_voice();
        }
    }
    return result;
}

/**
  * @ Power off analog and digital audio hardware for audio out
  */
void hal_audio_stop_stream_out(void)
{
    if (audio_config.audio_path_type == HAL_AUDIO_PLAYBACK_MUSIC || audio_config.audio_path_type == HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE) {
        audio_pcm_play_close_device();
    } else if (audio_config.audio_path_type == HAL_AUDIO_PLAYBACK_VOICE || audio_config.audio_path_type == HAL_AUDIO_TWO_WAY_VOICE) {
        audio_pcm2way_stop_voice();
    }
    return;
}

/**
  * @ Updates the audio output frequency
  * @ sample_rate : audio frequency used to play the audio stream
  * @ This API should be called before hal_audio_start_stream_out() to adjust the audio frequency
  * @ Retval: true if operation success, false if sample rate is invalid
  */
hal_audio_status_t hal_audio_set_stream_out_sampling_rate(hal_audio_sampling_rate_t sampling_rate)
{
    switch (sampling_rate) {
        case HAL_AUDIO_SAMPLING_RATE_8KHZ:
        case HAL_AUDIO_SAMPLING_RATE_11_025KHZ:
        case HAL_AUDIO_SAMPLING_RATE_12KHZ:
        case HAL_AUDIO_SAMPLING_RATE_16KHZ:
        case HAL_AUDIO_SAMPLING_RATE_22_05KHZ:
        case HAL_AUDIO_SAMPLING_RATE_24KHZ:
        case HAL_AUDIO_SAMPLING_RATE_32KHZ:
        case HAL_AUDIO_SAMPLING_RATE_44_1KHZ:
        case HAL_AUDIO_SAMPLING_RATE_48KHZ:
            audio_config.stream_out.stream_sampling_rate = sampling_rate;
            return HAL_AUDIO_STATUS_OK;
        default:
            return HAL_AUDIO_STATUS_INVALID_PARAMETER;
    }
}

/**
  * @ Updates the audio output channel number
  * @ channel_number : audio channel mode to play the audio stream
  * @ This API should be called before hal_audio_start_stream_out() to adjust the output channel number
  * @ Retval: true if operation success, false if channel number is invalid
  */
hal_audio_status_t hal_audio_set_stream_out_channel_number(hal_audio_channel_number_t channel_number)
{
    hal_audio_status_t result = HAL_AUDIO_STATUS_OK;
    switch (channel_number) {
        case HAL_AUDIO_MONO:
            audio_config.stream_out.stream_channel = HAL_AUDIO_MONO;
            break;
        case HAL_AUDIO_STEREO:
        case HAL_AUDIO_STEREO_BOTH_L_CHANNEL:
        case HAL_AUDIO_STEREO_BOTH_R_CHANNEL:
        case HAL_AUDIO_STEREO_BOTH_L_R_SWAP:
            audio_config.stream_out.stream_channel = HAL_AUDIO_STEREO;
            break;
        default:
            result = HAL_AUDIO_STATUS_INVALID_PARAMETER;
            break;
    }
    audio_dsp_assign_audio_channel(channel_number);
    return result;
}

/**
  * @ Updates the audio output volume
  * @ digital_volume_index: digital gain index
  * @ analog_volume_index : analog gain index
  */
hal_audio_status_t hal_audio_set_stream_out_volume(uint32_t digital_volume_index, uint32_t analog_volume_index)
{
    log_hal_info("hal_audio_set_stream_out_volume: digital_volume_index=%x, analog_volume_index=%x,\r\n", digital_volume_index, analog_volume_index);

    audio_config.stream_out.digital_gain_index = digital_volume_index;
    audio_config.stream_out.analog_gain_index = analog_volume_index;
    afe_audio_set_output_volume(analog_volume_index, digital_volume_index);
    return HAL_AUDIO_STATUS_OK;
}

/**
  * @ Mute stream ouput path
  * @ mute: true-> set mute / false->set unmute
  */
void hal_audio_mute_stream_out(bool mute)
{
    afe_set_stream_out_mute(mute);
}

/**
  * @ Control the audio output device
  * @ device: output device
  */
hal_audio_status_t hal_audio_set_stream_out_device(hal_audio_device_t device)
{
    audio_config.stream_out.audio_device = device;
    afe_audio_set_output_device(device);
    if (afe_is_audio_enable()) {
        afe_switch_tx_signal_path(true);
    }
    return HAL_AUDIO_STATUS_OK;
}

/**
  * @ Starts playing audio stream from a data buffer for a determined size.
  * @ buffer: Pointer to the buffer
  * @ size : number of audio data [in bytes]
  * @ Retval true if operation success, false if fail
  */
hal_audio_status_t hal_audio_write_stream_out(const void *buffer, uint32_t size)
{
    hal_audio_status_t result = HAL_AUDIO_STATUS_OK;

    if(buffer == 0)
        return HAL_AUDIO_STATUS_ERROR;
    if (audio_config.audio_path_type == HAL_AUDIO_PLAYBACK_MUSIC || audio_config.audio_path_type == HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE) {
        result = audio_pcm_stream_out_write(buffer, size);
    } else {
        result = audio_pcm2way_stream_out_write(buffer, size);
    }
    return result;
}

hal_audio_status_t hal_audio_get_stream_out_sample_count(uint32_t *sample_count)
{
    if (audio_config.audio_path_type == HAL_AUDIO_PLAYBACK_MUSIC || audio_config.audio_path_type == HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE) {
        audio_pcm_get_stream_out_free_space(sample_count);
    } else if (audio_config.audio_path_type == HAL_AUDIO_PLAYBACK_VOICE || audio_config.audio_path_type == HAL_AUDIO_TWO_WAY_VOICE) {
        if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_8KHZ) {
            *sample_count = NB_BUFFER_SIZE * 2;
        } else {
            *sample_count = WB_BUFFER_SIZE * 2;
        }
    }
    return HAL_AUDIO_STATUS_OK;
}

/**
  * @ Power on analog and digital audio hardware for audio in
  */
hal_audio_status_t hal_audio_start_stream_in(hal_audio_active_type_t active_type)
{
    if (active_type == HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE) { /* stream in sampling rate fixed to 16kHz due to hardware limitation */
        if (audio_config.stream_in.stream_sampling_rate != HAL_AUDIO_SAMPLING_RATE_16KHZ) {
            log_hal_info("Invalid playback + record stream in sampling rate : %d\r\n", (int)audio_config.stream_in.stream_sampling_rate);
            return HAL_AUDIO_STATUS_ERROR;
        }
    }
    if (active_type == HAL_AUDIO_RECORD_VOICE || active_type == HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE) {
        afe_set_path_type(active_type);
        audio_config.audio_path_type = active_type;
        audio_pcm_record_handle(true);
        afe_switch_mic_bias(true);
        afe_switch_rx_signal_path();
        if (audio_config.stream_in.audio_device != HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC && audio_config.stream_in.audio_device != HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC) {
            afe_mic_power_setting();
        }
        afe_audio_device_volume_update();
    } else if (active_type == HAL_AUDIO_TWO_WAY_VOICE) {
        if (audio_config.stream_in.stream_sampling_rate != audio_config.stream_out.stream_sampling_rate && active_type == HAL_AUDIO_TWO_WAY_VOICE) {
            return HAL_AUDIO_STATUS_INVALID_PARAMETER;
        }
        audio_pcm2way_play_voice();
    }
    return HAL_AUDIO_STATUS_OK;

}

/**
  * @ Power off analog and digital audio hardware for audio in
  */
void hal_audio_stop_stream_in()
{
    if (audio_config.audio_path_type == HAL_AUDIO_RECORD_VOICE || audio_config.audio_path_type == HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE) {
        afe_switch_mic_bias(false);
        afe_mic_power_setting();
        audio_pcm_record_handle(false);
    } else if (audio_config.audio_path_type == HAL_AUDIO_TWO_WAY_VOICE) {
        audio_pcm2way_stop_voice();
    }
}

/**
  * @ Updates the audio input frequency
  * @ sample_rate : audio frequency used to record the audio stream
  * @ This API should be called before hal_audio_start_stream_in() to adjust the audio frequency
  * @ Retval: true if operation success, false if sample rate is invalid
  */
hal_audio_status_t hal_audio_set_stream_in_sampling_rate(hal_audio_sampling_rate_t sampling_rate)
{
    switch (sampling_rate) {
        case HAL_AUDIO_SAMPLING_RATE_8KHZ:
        case HAL_AUDIO_SAMPLING_RATE_16KHZ:
            audio_config.stream_in.stream_sampling_rate = sampling_rate;
            return HAL_AUDIO_STATUS_OK;
        default:
            return HAL_AUDIO_STATUS_INVALID_PARAMETER;
    }
}

/**
  * @ Updates the audio input channel number
  * @ channel_number : audio channel mode to record the audio stream
  * @ This API should be called before hal_audio_start_stream_in() to adjust the input channel number
  * @ Retval: true if operation success, false if channel number is invalid
  */
hal_audio_status_t hal_audio_set_stream_in_channel_number(hal_audio_channel_number_t channel_number)
{
    switch (channel_number) {
        case HAL_AUDIO_MONO:
        case HAL_AUDIO_STEREO:
            audio_config.stream_in.stream_channel = channel_number;
            return HAL_AUDIO_STATUS_OK;
        default:
            return HAL_AUDIO_STATUS_INVALID_PARAMETER;
    }
}

/**
  * @ Updates the audio input volume
  * @ digital_volume_index: digital gain index
  * @ analog_volume_index : analog gain index
  */
hal_audio_status_t hal_audio_set_stream_in_volume(uint32_t digital_volume_index, uint32_t analog_volume_index)
{
    log_hal_info("hal_audio_set_stream_in_volume: digital_volume_index=%x, analog_volume_index=%x,\r\n", digital_volume_index, analog_volume_index);

    afe_audio_set_input_volume(analog_volume_index, digital_volume_index);
    return HAL_AUDIO_STATUS_OK;
}

/**
  * @ Mute stream input path
  * @ mute: true-> set mute / false->set unmute
  */
void hal_audio_mute_stream_in(bool mute)
{
    afe_set_stream_in_mute(mute);
}

/**
  * @ Control the audio input device
  * @ device: input device
  */
hal_audio_status_t hal_audio_set_stream_in_device(hal_audio_device_t device)
{
    audio_config.stream_in.audio_device = device;
    afe_audio_set_input_device(device);
    if (afe_is_audio_enable()) {
        afe_switch_rx_signal_path();
        afe_mic_power_setting();
    }
    return HAL_AUDIO_STATUS_OK;
}

/**
  * @ Starts audio recording
  * @ pBuffer: buffer pointer for the recorded data storing
  * @ size : number of audio data
  * @ Retval true if operation success, false if fail
  */
hal_audio_status_t hal_audio_read_stream_in(void *pBuffer, uint32_t size)
{
    if (audio_config.audio_path_type == HAL_AUDIO_RECORD_VOICE || audio_config.audio_path_type == HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE) {
        audio_pcm_stream_in_read(pBuffer, size);
    } else {
        audio_pcm2way_stream_in_read(pBuffer, size);
    }
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_get_stream_in_sample_count(uint32_t *sample_count)
{
    if (audio_config.audio_path_type == HAL_AUDIO_RECORD_VOICE || audio_config.audio_path_type == HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE) {
        audio_pcm_get_stream_in_record_size(sample_count);
    } else if (audio_config.audio_path_type == HAL_AUDIO_TWO_WAY_VOICE) {
        if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_8KHZ) {
            *sample_count = NB_BUFFER_SIZE * 2;
        } else {
            *sample_count = WB_BUFFER_SIZE * 2;
        }
    }
    return HAL_AUDIO_STATUS_OK;
};

hal_audio_status_t hal_audio_get_memory_size(uint32_t *memory_size)
{
    *memory_size = audio_pcm_get_memory_size();
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_memory(void *memory)
{
    return audio_pcm_set_memory(memory);
}

hal_audio_status_t hal_audio_register_stream_out_callback(hal_audio_stream_out_callback_t callback, void *userdata)
{
    audio_pcm_register_stream_out_callback(callback, userdata);
    audio_pcm2way_register_stream_out_callback(callback, userdata);
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_register_stream_in_callback(hal_audio_stream_in_callback_t callback, void *user_data)
{
    audio_pcm_register_stream_in_callback(callback, user_data);
    audio_pcm2way_register_stream_in_callback(callback, user_data);
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_get_audio_clock(uint32_t *sample_count)
{
    uint32_t smpl_cnt = audio_dsp_get_audio_sample_counter();
    *sample_count = smpl_cnt;
    return HAL_AUDIO_STATUS_OK;
}

#endif /*HAL_AUDIO_MODULE_ENABLED*/
