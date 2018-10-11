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

#include "hal_i2s_internal.h"

#ifdef HAL_I2S_MODULE_ENABLED

#include "hal_audio_fw_sherif.h"
#include "hal_audio_fw_interface.h"     /* for dsp_dynamic_download(DDID_I2S) */
#include "hal_audio_internal_afe.h"
#include "hal_audio_internal_service.h"

static void i2s_slave_isr_tx_handler(void)
{
    if (hal_i2s.i2s_state & I2S_STATE_TX_RUNNING) {
        audio_pcm2way_put_to_stream_out((uint16_t *)hal_i2s.tx_buffer.base);
        hal_i2s.tx_callback_func(HAL_I2S_EVENT_DATA_REQUEST, hal_i2s.tx_data);
    }
}

static void i2s_slave_isr_rx_handler(void)
{
    if (hal_i2s.i2s_state & I2S_STATE_RX_RUNNING) {
        audio_pcm2way_get_from_stream_in((uint16_t *)hal_i2s.rx_buffer.base);
        hal_i2s.rx_callback_func(HAL_I2S_EVENT_DATA_NOTIFICATION, hal_i2s.rx_data);
    }
}

void i2s_path_turn_on(hal_i2s_clock_mode_t i2s_clock_mode, hal_i2s_link_t i2s_link)
{
    hal_i2s_clock_mode_t        clock_mode = i2s_clock_mode;
    hal_i2s_sample_rate_t       sample_rate;
    audio_pcm2way_config_t      config;

    if (i2s_link == HAL_I2S_RX) {
        sample_rate = hal_i2s.i2s_config.i2s_in.sample_rate;
    } else { //HAL_I2S_TX
        sample_rate = hal_i2s.i2s_config.i2s_out.sample_rate;
    }
    if (clock_mode == HAL_I2S_MASTER) {
        hal_i2s.audio_id = audio_get_id();
        audio_service_setflag(hal_i2s.audio_id);

        dsp_audio_fw_dynamic_download(DDID_I2S);
        i2s_set_data_interrupt_rate();
        afe_set_and_swtich_master_i2s(true);
        i2s_master_dsp_turn_on();

        *DSP_AUDIO_CUR_GAIN = 0x7FFF;
        afe_audio_set_digital_gain();
        afe_master_i2s_internal_loopback(hal_i2s.internal_loopback);

        pcm_route_start();
        audio_playback_on(ASP_TYPE_I2SBypass, sample_rate);
    } else { //HAL_I2S_SLAVE
        config.stream_in_d2m_on = true;
        config.stream_out_m2d_on = true;
        config.stream_in_is_after_enhancement = true;
        config.stream_out_is_after_enhancement = false;

        if (sample_rate == HAL_I2S_SAMPLE_RATE_8K) {
            config.band = PCM2WAY_NARROWBAND;
            config.format = PCM2WAY_FORMAT_NORMAL;
            audio_turn_on_pcm_interface(false);
        } else {
            config.band = PCM2WAY_WIDEBAND;
            config.format = PCM2WAY_FORMAT_WB_NORMAL;
            audio_turn_on_pcm_interface(true);
        }
        afe_set_slave_i2s_config(sample_rate);
        audio_pcm2way_start(i2s_slave_isr_tx_handler, i2s_slave_isr_rx_handler, &config);

        afe_slave_i2s_enable();    //src should always on. otherwise, no data       //should set after AFE_MCU_CON0=1

    }
}

void i2s_path_turn_off(hal_i2s_clock_mode_t i2s_clock_mode)
{
    if (i2s_clock_mode == HAL_I2S_MASTER) {
        audio_playback_off();
        pcm_route_stop();
        i2s_master_dsp_turn_off();
        afe_set_and_swtich_master_i2s(false);

        audio_service_clearflag(hal_i2s.audio_id);
        audio_free_id(hal_i2s.audio_id);
    } else { //HAL_I2S_SLAVE
        audio_turn_off_pcm_interface();
        afe_slave_i2s_disable();
        audio_pcm2way_stop();
        afe_reset_slave_i2s_config();
    }
}

/** user config setting & checking **/
void i2s_set_initial_parameter(hal_i2s_t *hal_i2s)
{
    hal_i2s_t hal_i2s_init_table;
    memset(&hal_i2s_init_table, 0 , sizeof(hal_i2s_t));
    hal_i2s_init_table.i2s_initial_type = HAL_I2S_TYPE_EXTERNAL_MODE;
    hal_i2s_init_table.i2s_config.i2s_in.sample_rate =     HAL_I2S_SAMPLE_RATE_8K;
    hal_i2s_init_table.i2s_config.i2s_in.channel_number =  HAL_I2S_STEREO;
    hal_i2s_init_table.i2s_config.i2s_out.sample_rate =    HAL_I2S_SAMPLE_RATE_8K;
    hal_i2s_init_table.i2s_config.i2s_out.channel_number = HAL_I2S_STEREO;
    hal_i2s_init_table.i2s_config.clock_mode =             HAL_I2S_MASTER;
    hal_i2s_init_table.i2s_state =                         I2S_STATE_IDLE;
    hal_i2s_init_table.is_running =                        false;

    *hal_i2s = hal_i2s_init_table;
}

hal_i2s_status_t i2s_set_clock_mode(hal_i2s_clock_mode_t clock_mode)
{
    switch (clock_mode) {
        case HAL_I2S_MASTER:
        case HAL_I2S_SLAVE:
            hal_i2s.i2s_config.clock_mode = clock_mode;
            return HAL_I2S_STATUS_OK;
        default:
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }
}

hal_i2s_status_t i2s_is_master_sample_rate(hal_i2s_sample_rate_t sample_rate)
{
    switch (sample_rate) {
        case HAL_I2S_SAMPLE_RATE_8K:
        case HAL_I2S_SAMPLE_RATE_11_025K:
        case HAL_I2S_SAMPLE_RATE_12K:
        case HAL_I2S_SAMPLE_RATE_16K:
        case HAL_I2S_SAMPLE_RATE_22_05K:
        case HAL_I2S_SAMPLE_RATE_24K:
        case HAL_I2S_SAMPLE_RATE_32K:
        case HAL_I2S_SAMPLE_RATE_44_1K:
        case HAL_I2S_SAMPLE_RATE_48K:
            return HAL_I2S_STATUS_OK;
        default:
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }
}

hal_i2s_status_t i2s_is_slave_sample_rate(hal_i2s_sample_rate_t sample_rate)
{
    switch (sample_rate) {
        case HAL_I2S_SAMPLE_RATE_8K:
        case HAL_I2S_SAMPLE_RATE_16K:
            return HAL_I2S_STATUS_OK;
        default:
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }
}

hal_i2s_status_t i2s_is_master_channel_number(hal_i2s_channel_number_t channel_number)
{
    switch (channel_number) {
        case HAL_I2S_STEREO:
        case HAL_I2S_MONO:
            return HAL_I2S_STATUS_OK;
        default:
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }
}

hal_i2s_status_t i2s_is_slave_channel_number(hal_i2s_channel_number_t channel_number)
{
    switch (channel_number) {
        case HAL_I2S_MONO:
            return HAL_I2S_STATUS_OK;
        case HAL_I2S_STEREO:
        default:
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }
}

/** ring buffer handle **/
//space one word (two-byte less than read index is always empty) to maintain ring buffer structure - return in byte
uint32_t i2s_get_free_space(ring_buffer_t *ring_buffer)
{
    int32_t count;
    count = (int32_t)ring_buffer->read - (int32_t)ring_buffer->write - 2;
    if (count < 0) {
        count += (int32_t)ring_buffer->size;
    }
    return (uint32_t)count;
}

/* return in byte */
uint32_t i2s_get_data_count(ring_buffer_t *ring_buffer)
{
    int32_t count;
    count = (int32_t)ring_buffer->write - (int32_t)ring_buffer->read;
    if (count < 0) {
        count += (int32_t)ring_buffer->size;
    }
    return (uint32_t)count;
}

uint8_t *i2s_buffer_get_read_pointer(ring_buffer_t *ring_buffer)
{
    return (uint8_t *)ring_buffer->base + ring_buffer->read;
}

uint8_t *i2s_buffer_get_write_pointer(ring_buffer_t *ring_buffer)
{
    return (uint8_t *)ring_buffer->base + ring_buffer->write;
}

void i2s_get_free_space_two_section(ring_buffer_t *ring_buffer, uint32_t *free_length1, uint32_t *free_length2)
{
    int32_t count;
    count = (int32_t)ring_buffer->read - (int32_t)ring_buffer->write - 2;
    if (count < 0) {
        count += (int32_t)ring_buffer->size;
        if (ring_buffer->read == 0) {
            *free_length1 = (uint32_t)(ring_buffer->size - ring_buffer->write - 2);
            *free_length2 = 0;
        } else {
            *free_length1 = (uint32_t)(ring_buffer->size - ring_buffer->write);
            *free_length2 = ring_buffer->read - 2;// *free_length2 = (uint32_t)count - *free_length1;
        }
    } else {
        *free_length1 = (uint32_t)count;
        *free_length2 = 0;
    }

}

void i2s_get_data_count_two_section(ring_buffer_t *ring_buffer, uint32_t *data_length1, uint32_t *data_length2)
{
    int32_t count;
    count = (int32_t)ring_buffer->write - (int32_t)ring_buffer->read;
    if (count < 0) {
        count += (int32_t)ring_buffer->size;
        *data_length1 = (uint32_t)(ring_buffer->size - ring_buffer->read);
        *data_length2 = ring_buffer->write;
    } else {
        *data_length1 = (uint32_t)count;
        *data_length2 = 0;

    }
}

hal_i2s_status_t i2s_read_from_buffer(ring_buffer_t *ring_buffer, uint32_t read_length, void *buffer)
{
    uint32_t   buffer_space = ring_buffer->size - ring_buffer->read;
    uint32_t   data_count = i2s_get_data_count(ring_buffer);

    if (0 == read_length) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    } else if (data_count < (uint32_t)read_length) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    /* check if it uses incontinuous ring buffer */
    if (read_length > buffer_space) { /* seperate to two parts */
        memcpy((uint8_t *)buffer, (uint8_t *)ring_buffer->base + ring_buffer->read, buffer_space);
        memcpy((uint8_t *)buffer + buffer_space, (uint8_t *)ring_buffer->base, (read_length - buffer_space));
        ring_buffer->read = (read_length - buffer_space);
    } else {/* continuely read */
        memcpy((uint8_t *)buffer, (uint8_t *)ring_buffer->base + ring_buffer->read, read_length);
        ring_buffer->read += read_length;
        if (ring_buffer->size == ring_buffer->read) {
            ring_buffer->read = 0;
        }
    }
    return HAL_I2S_STATUS_OK;
}

/** @brief
 *  @param[in] buffer is a register in DSP sherif
 */
hal_i2s_status_t i2s_write_to_buffer(ring_buffer_t *ring_buffer, uint32_t write_length, void *buffer)
{
    uint32_t   buffer_space = ring_buffer->size - ring_buffer->write;
    uint32_t   free_space = i2s_get_free_space(ring_buffer);

    if (0 == write_length) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    } else if (free_space < (uint32_t)write_length) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    /* check if it uses incontinuous ring buffer */
    if (write_length > buffer_space) {
        memcpy((uint8_t *)ring_buffer->base + ring_buffer->write, (uint8_t *)buffer, buffer_space);
        memcpy((uint8_t *)ring_buffer->base, ((uint8_t *)buffer + buffer_space), (write_length - buffer_space));
        ring_buffer->write = write_length - buffer_space;
    } else {
        memcpy((uint8_t *)ring_buffer->base + ring_buffer->write, (uint8_t *)buffer, write_length);
        ring_buffer->write += write_length;
        if (ring_buffer->size == ring_buffer->write) {
            ring_buffer->write = 0;
        }
    }
    return HAL_I2S_STATUS_OK;
}

uint8_t *i2s_buffer_update_write_index(ring_buffer_t *ring_buffer, uint32_t offset)
{
    if ((ring_buffer->write + offset) > ring_buffer->size) {
        ring_buffer->write += (offset - ring_buffer->size);
    } else {
        ring_buffer->write += offset;
        if (ring_buffer->size == ring_buffer->write) {
            ring_buffer->write = 0;
        }
    }
    return (uint8_t *)ring_buffer->base + ring_buffer->write;
}

uint8_t *i2s_buffer_update_read_index(ring_buffer_t *ring_buffer, uint32_t offset)
{
    if ((ring_buffer->read + offset) > ring_buffer->size) {
        ring_buffer->read += (offset - ring_buffer->size);
    } else {
        ring_buffer->read += offset;
        if (ring_buffer->size == ring_buffer->read) {
            ring_buffer->read = 0;
        }
    }
    return (uint8_t *)ring_buffer->base + ring_buffer->read;
}

// allocate from memory pool (in byte)
uint16_t *i2s_alloc_internal_memory(uint32_t memory_size)
{
    uint8_t *return_address;
    if ((hal_i2s.memory_offset + memory_size) > 2 * memory_size) {
        return NULL;
    }
    return_address = (uint8_t *)hal_i2s.memory_pool + hal_i2s.memory_offset;
    hal_i2s.memory_offset += memory_size;
    return (uint16_t *)return_address;
}

/** DSP **/
void i2s_master_dsp_turn_on(void)
{
    *DSP_AUDIO_CTRL2 |= DSP_PCM_R_DIS;
    *DSP_AUDIO_FLEXI_CTRL |= (FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);
    *DSP_I2S_PLAYERROR_INFO = 0;
    *DSP_I2S_PLAYERROR_CONTROL = 0;
}

void i2s_master_dsp_turn_off(void)
{
    *DSP_AUDIO_FLEXI_CTRL &= ~(FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);
    *DSP_AUDIO_CTRL2 &= ~DSP_PCM_R_DIS;
}

void i2s_set_data_interrupt_rate(void)
{
    *AFE_VAM_SET &= ~0x07;
    *AFE_VAM_SET |= 0x02;
}

bool i2s_is_legecy_in_used(void)
{
    return hal_i2s.is_running;
}

/*Special case to enable i2s UL when dsp decode*/
#ifdef HAL_I2S_DSP_DECODE_CONCURRENT_ENABLED

static void i2s_rx_audio_decode_isr_handler(void *data)
{
    uint16_t page = *DSP_I2S_RX_AUD_DEC_PAGE_NUMBER;
    uint16_t addr = *DSP_I2S_RX_AUD_DEC_ADDRESS;
    volatile uint16_t *data_ptr = DSP_DM_ADDR(page, addr);
    uint16_t *buffer_addr = hal_i2s.rx_aud_dec_buffer;
    int32_t loop_idx;
    for (loop_idx = 0; loop_idx < I2S_RX_AUDDEC_FRM_LEN; loop_idx++) {
        *buffer_addr++ = *data_ptr++;
    }
    *DSP_TASK6_HANDSHAKE = 0;
    hal_i2s.rx_callback_func(HAL_I2S_EVENT_DATA_NOTIFICATION, hal_i2s.rx_data);
    return;
}

bool i2s_is_dsp_decode_concurrency(void)
{
    return hal_i2s.dsp_decode_concurrency;
}


void i2s_rx_dsp_decode(bool on)
{
    if (on) {
        audio_service_hook_isr(DSP_D2C_I2S_RX_AUD_DEC_INT, i2s_rx_audio_decode_isr_handler, NULL);
        *DSP_I2S_RX_AUD_DEC_FRAME_LENGTH = I2S_RX_AUDDEC_FRM_LEN;
        *DSP_I2S_RX_AUD_DEC_CTRL = 0x0001;
        i2s_set_data_interrupt_rate();
        afe_set_and_swtich_master_i2s(true);
    } else {
        audio_service_unhook_isr(DSP_D2C_I2S_RX_AUD_DEC_INT);
        *DSP_I2S_RX_AUD_DEC_CTRL = 0x0000;
        afe_set_and_swtich_master_i2s(false);
    }
}

#endif /* HAL_I2S_DSP_DECODE_CONCURRENT_ENABLED */

#endif /* HAL_I2S_MODULE_ENABLED */
