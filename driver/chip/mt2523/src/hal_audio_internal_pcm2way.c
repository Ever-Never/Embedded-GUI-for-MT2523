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

#include "hal_audio_internal_pcm2way.h"
#include "hal_audio_test.h"

#if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED)

#include "string.h"

audio_pcm2way_t audio_pcm2way;
pcm2way_internal_t voice_internal_handle;
extern hal_audio_stream_t audio_config;
uint16_t *voice_playback_buffer_pointer;
uint16_t *voice_record_buffer_pointer;
uint16_t pcm2way_nb_stream_in_buffer [NB_BUFFER_SIZE];
uint16_t pcm2way_wb_stream_in_buffer [WB_BUFFER_SIZE];

#ifdef MTK_EXTERNAL_DSP_ENABLE
static void audio_pcm2way_fix_afe_16k_band(void)
{
    *DSP_SPH_SFE_CTRL &= ~(1 << 7);     //UL SRC (upsample)
    *DSP_SPH_SFE_CTRL &= ~(1 << 8);     //DL SRC (downsample)
}
#endif

speech_path_type_t audio_pcm2way_get_path_type(void)
{
    return audio_pcm2way.path;
}

/*
DSP_SPH_SFE_CTRL
bit[1]:UL SRC (downsample)  main mic
bit[2]:DL SRC (upsample)
bit[3]:SideTone
bit[6]:UL2 SRC(downsample)  referece mic
bit[7]:UL SRC (upsample)
bit[8]:DL SRC (downsample)
*/
static void audio_pcm2way_set_band(bool is_widthband)
{
    *DSP_SPH_SCH_IMPROVE_CTRL |= 0x0030;     //set to fix WB ENH
    if (is_widthband) {
        *DSP_SPH_SFE_CTRL &= ~0x1C0;
        *DSP_SPH_8K_CTRL |= 0x8;
    } else {
        *DSP_SPH_SFE_CTRL |= 0x1C0;
        *DSP_SPH_8K_CTRL &= ~0x8;
    }
#if defined(__GNUC__)
#if defined(HAL_AUDIO_SLT_ENABLE)
    if (audio_is_slt_or_external_loopback_test()) {
        *DSP_SPH_SCH_IMPROVE_CTRL &= ~0x0030;
        *DSP_SPH_SFE_CTRL = 0;
        *DSP_SPH_8K_CTRL &= ~0x8;
    }
#endif /*defined(HAL_AUDIO_SLT_ENABLE)*/
#endif /*defined(__GNUC__)*/
}

static void audio_pcm2way_config_function(void)
{
    uint16_t ctrl_ul = *DSP_SPH_PNW_CTRL_UL;
    uint16_t ctrl_dl = *DSP_SPH_PNW_CTRL_DL;
    if (audio_pcm2way.audio_pcm2way_config.stream_in_d2m_on == true) {
        ctrl_ul |= (0x0001 << 10);
    } else {
        ctrl_ul &= ~(0x0001 << 10);
    }
    if (audio_pcm2way.audio_pcm2way_config.stream_out_m2d_on == true) {
        ctrl_dl |= (0x0001 << 11);
    } else {
        ctrl_dl &= ~(0x0001 << 11);
    }
    if (audio_pcm2way.audio_pcm2way_config.stream_in_is_after_enhancement == true) {
        ctrl_ul |= (0x0001 << 9);
    } else {
        ctrl_ul &= ~(0x0001 << 9);
    }
    if (audio_pcm2way.audio_pcm2way_config.stream_out_is_after_enhancement == true) {
        ctrl_dl |= (0x0001 << 9);
    } else {
        ctrl_dl &= ~(0x0001 << 9);
    }
    *DSP_SPH_PNW_CTRL_UL = ctrl_ul;
    *DSP_SPH_PNW_CTRL_DL = ctrl_dl;
    audio_dsp_speech_set_delay(2);
}

static void audio_pcm2way_ex_on(void)
{
    if (audio_pcm2way.audio_pcm2way_config.stream_in_d2m_on && audio_pcm2way.audio_pcm2way_config.stream_out_m2d_on) {
        audio_pcm2way.path = SPH_BOTH_PATH;
    } else if (audio_pcm2way.audio_pcm2way_config.stream_out_m2d_on) {
        audio_pcm2way.path = SPH_VOICE_PCM2WAY_DL;
    } else {
        audio_pcm2way.path = SPH_VOICE_PCM2WAY_UL;
    }
    audio_dsp_reload_coeficient();
    afe_voice_digital_on();
    audio_pcm2way_config_function();
    VBI_RESET();
    return;
}

static void audio_pcm2way_ex_off(void)
{
    VBI_END();
    afe_switch_mic_bias(false);
    afe_audio_analog_device_off();
    afe_voice_digital_off();
    audio_pcm2way.path = (speech_path_type_t)0;
    return;
}

static volatile uint16_t *audio_pcm2way_get_stream_out_buffer()
{
    volatile uint16_t *addr = 0;
    addr = DSP_SPH_DM_ADDR_PNW_DL_BUF;
    return DSP_DM_ADDR(5, *addr);
}

static volatile uint16_t *audio_pcm2way_get_stream_in_buffer()
{
    volatile uint16_t *addr = 0;
    addr = DSP_SPH_DM_ADDR_PNW_UL_BUF;
    return DSP_DM_ADDR(5, *addr);
}

static volatile uint16_t *audio_pcm2way_get_stream_in_dual_mic_buffer()
{
    return DSP_DM_ADDR(5, *DSP_DM_ADDR(5, DSP_SPH_SE2_PTR));
}

void audio_pcm2way_get_from_stream_in(uint16_t *stream_in_buffer)
{
    volatile uint16_t *ptr = audio_pcm2way_get_stream_in_buffer();
    uint32_t I = audio_pcm2way.dsp_pcm_ex_rx_len;
    while (I-- > 0) {
        *stream_in_buffer++ = *ptr++;
    }
}

void audio_pcm2way_get_from_stream_in_second_mic(uint16_t *stream_in_buffer)
{
    volatile uint16_t *ptr = audio_pcm2way_get_stream_in_dual_mic_buffer();
    uint32_t I = audio_pcm2way.dsp_pcm_ex_rx_len;
    while (I-- > 0) {
        *stream_in_buffer++ = *ptr++;
    }
}

void audio_pcm2way_put_to_stream_out(uint16_t *stream_out_buffer)
{
    volatile uint16_t *ptr = audio_pcm2way_get_stream_out_buffer();
    uint32_t I = audio_pcm2way.dsp_pcm_ex_tx_len;
    while (I-- > 0) {
        *ptr++ = *stream_out_buffer++;
    }
}

static void audio_pcm2way_set_state(uint8_t is_tx, pcm_ex_state_t state)
{
    volatile uint16_t *addr = is_tx ? DSP_SPH_PNW_CTRL_DL : DSP_SPH_PNW_CTRL_UL;
    if (state == PCMEX_ON) {
        *addr = (*addr & 0xfff0) | 0x0101;
    } else {
        *addr = (*addr & 0xfef0);
    }
}

static bool audio_pcm2way_check_state(uint8_t is_DL, pcm_ex_state_t state)
{
    bool ret = false;
    if (is_DL) {
        ret = ((*DSP_SPH_PNW_CTRL_DL & 0xf) == state);
    } else {
        ret = ((*DSP_SPH_PNW_CTRL_UL & 0xf) == state);
    }
    return ret;
}

static void audio_pcm2way_query_buf_size(audio_pcm2way_format_t format, uint32_t *rx_buf_size, uint32_t *tx_buf_size)
{
    switch (format) {
        case PCM2WAY_FORMAT_NORMAL:
            *rx_buf_size = NB_BUFFER_SIZE;
            *tx_buf_size = NB_BUFFER_SIZE;
            break;
        case PCM2WAY_FORMAT_WB_NORMAL:
            *rx_buf_size = WB_BUFFER_SIZE;
            *tx_buf_size = WB_BUFFER_SIZE;
            break;
        default:
            *rx_buf_size = 0;
            *tx_buf_size = 0;
            break;
    }
}

bool audio_pcm2way_record_is_record_silence(void)
{
    if (voice_internal_handle.presilence > PCM2WAY_PRESILENCE_NUM) {
        return false;
    }
    voice_internal_handle.presilence++;
    return true;
}

static void audio_pcm_stream_in_handle_hisr()
{
    if (!audio_pcm2way_record_is_record_silence()) {
        audio_pcm2way.stream_in_callback();
    }
}
static void audio_pcm2way_start_common(void (*pcmnway_dl_hdlr)(void), void (*pcmnway_ul_hdlr)(void), audio_pcm2way_config_t *config)
{
    audio_pcm2way.audio_id = audio_get_id();
    audio_service_setflag(audio_pcm2way.audio_id);
    audio_service_hook_isr(DSP_PCM_EX_DL, (isr)pcmnway_dl_hdlr, NULL);
    audio_service_hook_isr(DSP_PCM_EX_UL, (isr)audio_pcm_stream_in_handle_hisr, NULL);
    audio_pcm2way.stream_in_callback = pcmnway_ul_hdlr;
    audio_pcm2way.audio_pcm2way_config.stream_out_m2d_on = config->stream_out_m2d_on;
    audio_pcm2way.audio_pcm2way_config.stream_in_d2m_on = config->stream_in_d2m_on;
    audio_pcm2way.audio_pcm2way_config.stream_in_is_after_enhancement = config->stream_in_is_after_enhancement;
    audio_pcm2way.audio_pcm2way_config.stream_out_is_after_enhancement = config->stream_out_is_after_enhancement;
    audio_pcm2way.audio_pcm2way_config.band = config->band;
    audio_pcm2way.audio_pcm2way_config.format = config->format;
    audio_pcm2way_query_buf_size(audio_pcm2way.audio_pcm2way_config.format, &(audio_pcm2way.dsp_pcm_ex_rx_len), &(audio_pcm2way.dsp_pcm_ex_tx_len));
    return;
}

static void audio_pcm2way_config_and_check_start_state(void)
{
#if defined(HAL_I2S_MODULE_ENABLED)
    if (audio_pcm2way.pcm2way_uplink_fifo == I2S_FIFO){
        afe_set_and_swtich_master_i2s(true);
        *AFE_AMCU_CON1 |= (1 << 12);
    }
#endif
    audio_pcm2way_ex_on();
    audio_pcm2way_set_state(1, PCMEX_ON);/*DL*/
    audio_pcm2way_set_state(0, PCMEX_ON);/*UL*/
    while (1) {
        if ((*DSP_SPH_PNW_CTRL_DL & 0xf) == PCMEX_READY && (*DSP_SPH_PNW_CTRL_UL & 0xf) == PCMEX_READY) {
            break;
        }
    }
    return;
}

void audio_pcm2way_start(void (*pcm2way_dl_hdlr)(void), void (*pcm2way_ul_hdlr)(void), audio_pcm2way_config_t *config)
{
    voice_internal_handle.presilence = 0;
    audio_pcm2way_start_common(pcm2way_dl_hdlr, pcm2way_ul_hdlr, config);
    if (config -> band == PCM2WAY_NARROWBAND) {
        audio_pcm2way_set_band(false);
        afe_digital_set_wideband_voice(false);
    } else {
        audio_pcm2way_set_band(true);
        afe_digital_set_wideband_voice(true);
    }
#ifdef MTK_EXTERNAL_DSP_ENABLE
    if ((audio_config.stream_in.audio_device == HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC) || (audio_config.stream_in.audio_device == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC)) {
        //[always fix AFE WB input]
        audio_pcm2way_fix_afe_16k_band();
        afe_digital_set_wideband_voice(true);
    }
#endif
    afe_switch_mic_bias(true);
    afe_switch_rx_signal_path();
    if (audio_config.stream_in.audio_device != HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC && audio_config.stream_in.audio_device != HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) {
        afe_mic_power_setting();
    }
    audio_pcm2way_config_and_check_start_state();
    afe_switch_tx_signal_path(true);
    afe_audio_device_volume_update();
    return;
}

void audio_pcm2way_stop(void)
{
    if (audio_pcm2way_check_state(0, PCMEX_READY)) {
        audio_pcm2way_set_state(0, PCMEX_OFF);
    }
    if (audio_pcm2way_check_state(1, PCMEX_READY)) {
        audio_pcm2way_set_state(1, PCMEX_OFF);
    }
    audio_pcm2way_ex_off();
    afe_switch_tx_signal_path(false);

#if defined(HAL_I2S_MODULE_ENABLED)
    if (audio_pcm2way.pcm2way_uplink_fifo == I2S_FIFO){
       afe_set_and_swtich_master_i2s(false);
       *AFE_AMCU_CON1 &= ~(1 << 12);
    }
#endif

    audio_service_unhook_isr(DSP_PCM_EX_DL);
    audio_service_unhook_isr(DSP_PCM_EX_UL);
    audio_service_clearflag(audio_pcm2way.audio_id);
    audio_free_id(audio_pcm2way.audio_id);
    return;
}

/*PCM interface related*/
void audio_pcm2way_set_dsp_uplink_fifo(audio_pcm2way_afe_uplink_fifo uplink_fifo)
{
#if defined(HAL_I2S_MODULE_ENABLED)
    if (uplink_fifo == I2S_FIFO) {
        printf("audio_pcm2way_set_dsp_uplink_fifo: SET I2S_FIFO\r\n");
        audio_pcm2way.pcm2way_uplink_fifo = I2S_FIFO;
        *DSP_SPH_8K_CTRL |= (1 << 5); // UL_I2S_FIFO Switch speech DSP UL data is from AFE UL FIFO or AFE I2S FIFO 0: UL FIFO 1: I2S FIFO
    } else
#endif
    {
        // default is from ul fifo
        audio_pcm2way.pcm2way_uplink_fifo = UL_FIFO;
        *DSP_SPH_8K_CTRL &= ~(1 << 5);
    }
}

void audio_pcm2way_turn_on_pcm_interface(void)
{
    *DSP_SPH_BT_MODE  = 0x0000; // Linear mode
    *DSP_SPH_BT_CTRL  = 0x0100; // MSB first, 0dB UL gain
    *DSP_SPH_DL_VOL   = 0x1000;
    *DSP_SPH_8K_CTRL  = (*DSP_SPH_8K_CTRL & 0x00FF) | 0x2000;
}

void audio_pcm2way_register_stream_out_callback(hal_audio_stream_out_callback_t callback, void *user_data)
{
    voice_internal_handle.out_callback = callback;
    voice_internal_handle.out_user_data = user_data;
}

void audio_pcm2way_register_stream_in_callback(hal_audio_stream_in_callback_t callback, void *user_data)
{
    voice_internal_handle.in_callback = callback;
    voice_internal_handle.in_user_data = user_data;
}

hal_audio_status_t audio_pcm2way_stream_in_read(const void *buffer, uint32_t size) //[size: bytes]
{
    hal_audio_status_t result;
    uint32_t read_byte_count = 0;
    uint16_t *p_dst = (uint16_t *)buffer;
    if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_8KHZ) {
        read_byte_count = NB_BUFFER_SIZE;
        memcpy(p_dst, pcm2way_nb_stream_in_buffer, read_byte_count * 2);
    } else {
        read_byte_count = WB_BUFFER_SIZE;
        memcpy(p_dst, pcm2way_wb_stream_in_buffer, read_byte_count * 2);
    }
    voice_internal_handle.in_waiting = false;
    result = HAL_AUDIO_STATUS_OK;
    return result;

}

hal_audio_status_t audio_pcm2way_stream_out_write(const void *buffer, uint32_t size) //[size: bytes]
{
    voice_playback_buffer_pointer = (uint16_t *)buffer;
    voice_internal_handle.out_waiting = false;
    voice_internal_handle.underflow = false;
    return HAL_AUDIO_STATUS_OK;
}

static void audio_pcm_voice_isr_stream_out_handler(void)
{
    bool is_dsp_underflow = false;
    if (audio_config.audio_path_type == HAL_AUDIO_PLAYBACK_VOICE || audio_config.audio_path_type == HAL_AUDIO_TWO_WAY_VOICE) {
        if (voice_internal_handle.state != PCM_STATE_RUNNING) {
            return;
        }
        if (voice_internal_handle.state == PCM_STATE_RUNNING) {
            audio_pcm2way_put_to_stream_out(voice_playback_buffer_pointer);
            if (!voice_internal_handle.out_waiting) {
                voice_internal_handle.out_waiting = true;
                if (is_dsp_underflow) {
                    voice_internal_handle.underflow = true;
                    voice_internal_handle.out_callback(HAL_AUDIO_EVENT_UNDERFLOW, voice_internal_handle.out_user_data);
                } else {
                    voice_internal_handle.out_callback(HAL_AUDIO_EVENT_DATA_REQUEST, voice_internal_handle.out_user_data);
                }
            }
        }
    }

}

static void audio_pcm_voice_isr_stream_in_handler(void)
{
    if (audio_config.audio_path_type == HAL_AUDIO_TWO_WAY_VOICE) {
        if (audio_config.stream_in.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
            audio_pcm2way_get_from_stream_in(pcm2way_wb_stream_in_buffer);
        } else {
            audio_pcm2way_get_from_stream_in(pcm2way_nb_stream_in_buffer);
        }
        if (!voice_internal_handle.in_waiting) {
            voice_internal_handle.in_waiting = true;
            voice_internal_handle.in_callback(HAL_AUDIO_EVENT_DATA_NOTIFICATION, voice_internal_handle.in_user_data);
        }
    }
}

void audio_pcm2way_play_voice(void)
{
    audio_pcm2way_config_t config;
    if (voice_internal_handle.state != PCM_STATE_RUNNING) {
        voice_internal_handle.state = PCM_STATE_INIT;
        voice_internal_handle.audio_id = audio_get_id();
        voice_internal_handle.channel = audio_transfer_channel_number(audio_config.stream_out.stream_channel);
        voice_internal_handle.sample_rate = audio_transfer_sample_rate(audio_config.stream_out.stream_sampling_rate);
        if (voice_internal_handle.audio_id > MAX_AUDIO_FUNCTIONS) {
            return;
        }
        if (voice_internal_handle.state == PCM_STATE_INIT) {
            ring_buffer_information_t *p_dsp_info = &voice_internal_handle.dsp_buffer_info;
            p_dsp_info->read_pointer        = 0;
            p_dsp_info->write_pointer       = 0;
            voice_internal_handle.state = PCM_STATE_RUNNING;
        }
        if (audio_config.audio_path_type == HAL_AUDIO_PLAYBACK_VOICE) {
            config.stream_out_m2d_on = true;
            config.stream_out_is_after_enhancement = false;
            if (audio_config.stream_out.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_8KHZ) {
                config.band   = PCM2WAY_NARROWBAND;
                config.format = PCM2WAY_FORMAT_NORMAL;
            } else {
                config.band   = PCM2WAY_WIDEBAND;
                config.format = PCM2WAY_FORMAT_WB_NORMAL;
            }
        } else {
            config.stream_in_d2m_on = true;
            config.stream_out_m2d_on = true;
            config.stream_in_is_after_enhancement = true;
            config.stream_out_is_after_enhancement = false;
            if (audio_config.stream_out.stream_sampling_rate == HAL_AUDIO_SAMPLING_RATE_8KHZ) {
                config.band   = PCM2WAY_NARROWBAND;
                config.format = PCM2WAY_FORMAT_NORMAL;
            } else {
                config.band   = PCM2WAY_WIDEBAND;
                config.format = PCM2WAY_FORMAT_WB_NORMAL;
            }
        }
        audio_pcm2way_start(audio_pcm_voice_isr_stream_out_handler, audio_pcm_voice_isr_stream_in_handler, &config);
    }
}

void audio_pcm2way_stop_voice(void)
{
    if (voice_internal_handle.state != PCM_STATE_IDLE) {
        audio_pcm2way_stop();
        audio_free_id(voice_internal_handle.audio_id);
        voice_internal_handle.state = PCM_STATE_IDLE;
    }
}

#endif  /* defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED) */
