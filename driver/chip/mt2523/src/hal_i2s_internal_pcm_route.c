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

#include "hal_i2s_internal_pcm_route.h"

#ifdef HAL_I2S_MODULE_ENABLED

#include "hal_nvic_internal.h"
#include "hal_audio_internal_service.h"

pcm_route_t pcm_route;

void pcm_route_start()
{
    if (!pcm_route.isr_handler) {
        pcm_route.isr_handler = pcm_route_isr_handler;
        pcm_route.isr_enable = false;
        pcm_route.rx_buffer = &hal_i2s.rx_buffer;
        pcm_route.tx_buffer = &hal_i2s.tx_buffer;

        if (hal_i2s.i2s_config.i2s_in.channel_number == HAL_I2S_MONO) {
            pcm_route.stereo2mono = true;
            pcm_route.handle.read_data_from_dsp = pcm_route_get_data_from_dsp_dropR;
        } else {
            pcm_route.stereo2mono = false;
            pcm_route.handle.read_data_from_dsp = pcm_route_get_data_from_dsp;
        }

        if (hal_i2s.i2s_config.i2s_out.channel_number == HAL_I2S_MONO) {
            pcm_route.mono2stereo = true;
            pcm_route.handle.write_data_to_dsp = pcm_route_put_data_to_dsp_duplicate;
            pcm_route.handle.write_silence_to_dsp = pcm_route_put_silence_to_dsp_duplicate;
        } else {
            pcm_route.mono2stereo = false;
            pcm_route.handle.write_data_to_dsp = pcm_route_put_data_to_dsp;
            pcm_route.handle.write_silence_to_dsp = pcm_route_put_silence_to_dsp;
        }

        pcm_route.pcm_route_id = audio_pcm_route_register_isr(pcm_route_isr, NULL, AUDIO_PCM_ROUTE_ORDER_VOICE_I2S);
        pcm_route.isr_enable = true;
    }
}

void pcm_route_isr(void *data)
{
    uint16_t pcm_route_buffer_length = *DSP_PCM_ROUTE_DATA_LENGTH;
    /* define DSP sherif access address */
    pcm_route.buffer_address = (*DSP_PCM_ROUTE_DATA_ADDRESS) & 0x3FFF;
    pcm_route.channel_number = (pcm_route_buffer_length >> 15) & 0x0001;
    pcm_route.page_number = (*DSP_PCM_ROUTE_DATA_PAGENUM) & 0x000F;
    pcm_route.data_length_get = pcm_route_buffer_length & 0x3FFF;
    pcm_route.data_length_put = pcm_route.data_length_get;    //when both get/put are in stereo, they have the same data length from DSP
    /* if hisr callback function is registered, then executes */
    if (pcm_route.isr_handler) {
        pcm_route.isr_handler();
    }
    *DSP_TASK4_COSIM_HANDSHAKE = 0;

}

void pcm_route_stop(void)
{
    if (pcm_route.isr_enable) {
        audio_pcm_route_deregister_isr(pcm_route.pcm_route_id);
        pcm_route.isr_handler = NULL;
        pcm_route.isr_enable = false;
    }
}

void pcm_route_isr_handler(void)
{
    uint32_t         number_sample_get;
    uint32_t         number_sample_put;
    hal_i2s_event_t get_event = HAL_I2S_EVENT_NONE, put_event = HAL_I2S_EVENT_NONE;
    uint32_t        free_length1, free_length2, data_length1, data_length2, remaining_put_space;
    uint16_t        *read_pointer, *write_pointer;

    pcm_route_get_length_info(&number_sample_get, &number_sample_put);
    if (pcm_route.stereo2mono) {
        number_sample_get >>= 1;
    }
    if (pcm_route.mono2stereo) {
        number_sample_put >>= 1;
    }

    /* handle get data */
    if (hal_i2s.i2s_state & I2S_STATE_RX_RUNNING) {
        if (pcm_route.rx_buffer->base) {
            i2s_get_free_space_two_section(pcm_route.rx_buffer, &free_length1, &free_length2);
            if (number_sample_get <= ((free_length1 + free_length2) >> 1)) { //in word
                // free buffer is enough
                get_event = HAL_I2S_EVENT_DATA_NOTIFICATION;
            } else {
                // free buffer is not enough: rx overflow -get less samples than the size actually available in DSP
                get_event = HAL_I2S_EVENT_OVERFLOW;
                number_sample_get = (free_length1 + free_length2) >> 1;
            }
            if (number_sample_get <= (free_length1 >> 1)) {
                // only one part of free buffer is needed
                write_pointer = (uint16_t *)i2s_buffer_get_write_pointer(pcm_route.rx_buffer);
                pcm_route.handle.read_data_from_dsp(write_pointer, 0, number_sample_get);
                i2s_buffer_update_write_index(pcm_route.rx_buffer, number_sample_get << 1);
            } else {
                write_pointer = (uint16_t *)i2s_buffer_get_write_pointer(pcm_route.rx_buffer);
                pcm_route.handle.read_data_from_dsp(write_pointer, 0, free_length1 >> 1);
                write_pointer = (uint16_t *)i2s_buffer_update_write_index(pcm_route.rx_buffer, free_length1);
                pcm_route.handle.read_data_from_dsp(write_pointer, free_length1 >> 1, number_sample_get - (free_length1 >> 1));
                i2s_buffer_update_write_index(pcm_route.rx_buffer, (number_sample_get - (free_length1 >> 1)) << 1);
            }
        }
    }
    /* handle put data */  /*currently, prebuffering is not implemented */
    if (hal_i2s.i2s_state & I2S_STATE_TX_RUNNING) {
        if (pcm_route.tx_buffer->base) {
            i2s_get_data_count_two_section(pcm_route.tx_buffer, &data_length1, &data_length2);
            if (data_length1 == 0) {
                // no data: tx underflow
                put_event = HAL_I2S_EVENT_UNDERFLOW;
                pcm_route.handle.write_silence_to_dsp(0, number_sample_put);
            } else if (((data_length1 + data_length2) >> 1) > number_sample_put) {
                // data buffer is enough
                put_event = HAL_I2S_EVENT_DATA_REQUEST;
                if ((data_length1 >> 1) >= number_sample_put) {
                    read_pointer = (uint16_t *)i2s_buffer_get_read_pointer(pcm_route.tx_buffer);
                    pcm_route.handle.write_data_to_dsp(read_pointer, 0, number_sample_put);
                    i2s_buffer_update_read_index(pcm_route.tx_buffer, number_sample_put << 1);
                } else {
                    remaining_put_space = number_sample_put - (data_length1 >> 1);
                    read_pointer = (uint16_t *)i2s_buffer_get_read_pointer(pcm_route.tx_buffer);
                    pcm_route.handle.write_data_to_dsp(read_pointer, 0, data_length1 >> 1);
                    read_pointer = (uint16_t *)i2s_buffer_update_read_index(pcm_route.tx_buffer, data_length1);
                    pcm_route.handle.write_data_to_dsp(read_pointer, data_length1 >> 1, remaining_put_space);
                    read_pointer = (uint16_t *)i2s_buffer_update_read_index(pcm_route.tx_buffer, remaining_put_space << 1);
                }
            } else { // 0 < data_length < number_sample_put
                // data buffer is not enough: tx underflow. fill data = (data_length of data) + (remain size of silence)
                put_event = HAL_I2S_EVENT_UNDERFLOW;
                remaining_put_space = number_sample_put - ((data_length1 + data_length2) >> 1);
                if (data_length2 == 0) {
                    read_pointer = (uint16_t *)i2s_buffer_get_read_pointer(pcm_route.tx_buffer);
                    pcm_route.handle.write_data_to_dsp(read_pointer, 0, data_length1 >> 1);
                    i2s_buffer_update_read_index(pcm_route.tx_buffer, data_length1);
                    pcm_route.handle.write_silence_to_dsp(data_length1 >> 1, remaining_put_space);
                } else {
                    read_pointer = (uint16_t *)i2s_buffer_get_read_pointer(pcm_route.tx_buffer);
                    pcm_route.handle.write_data_to_dsp(read_pointer, 0, data_length1 >> 1);
                    read_pointer = (uint16_t *)i2s_buffer_update_read_index(pcm_route.tx_buffer, data_length1);
                    pcm_route.handle.write_data_to_dsp(read_pointer, data_length1 >> 1, data_length2 >> 1);
                    i2s_buffer_update_read_index(pcm_route.tx_buffer, data_length2);
                    pcm_route.handle.write_silence_to_dsp((data_length1 + data_length2) >> 1, remaining_put_space);
                }
            }
        }
    }
    if (hal_i2s.i2s_state & I2S_STATE_RX_RUNNING) {
        hal_i2s.rx_callback_func(get_event, hal_i2s.rx_data);
    }

    if (hal_i2s.i2s_state & I2S_STATE_TX_RUNNING) {
        hal_i2s.tx_callback_func(put_event, hal_i2s.tx_data);
    }
}


void pcm_route_get_length_info(uint32_t *sample_count_get, uint32_t *sample_count_put)
{
    *sample_count_get = pcm_route.data_length_get;
    *sample_count_put = pcm_route.data_length_put;
}

void pcm_route_get_data_from_dsp(uint16_t *buffer, uint32_t dsp_offset, uint32_t length)
{
    volatile uint16_t *idma_pointer;
    idma_pointer = DSP_DM_ADDR(pcm_route.page_number, pcm_route.buffer_address);
    idma_pointer += dsp_offset;

    audio_idma_read_from_dsp(buffer, idma_pointer, length);
}

void pcm_route_get_data_from_dsp_dropR(uint16_t *buffer, uint32_t dsp_offset, uint32_t length)
{
    volatile uint16_t *idma_pointer;
    idma_pointer = DSP_DM_ADDR(pcm_route.page_number, pcm_route.buffer_address);
    idma_pointer += dsp_offset;

    audio_idma_read_from_dsp_dropR(buffer, idma_pointer, length);
}

void pcm_route_put_data_to_dsp(uint16_t *buffer, uint32_t dsp_offset, uint32_t length)
{
    volatile uint16_t *idma_pointer;
    idma_pointer = DSP_DM_ADDR(pcm_route.page_number, pcm_route.buffer_address);
    idma_pointer += dsp_offset;

    audio_idma_write_to_dsp(idma_pointer, buffer, length);
}

void pcm_route_put_data_to_dsp_duplicate(uint16_t *buffer, uint32_t dsp_offset, uint32_t length)
{
    volatile uint16_t *idma_pointer;
    idma_pointer = DSP_DM_ADDR(pcm_route.page_number, pcm_route.buffer_address);
    idma_pointer += dsp_offset;
    audio_idma_write_to_dsp_duplicate(idma_pointer, buffer, length);
}

void pcm_route_put_silence_to_dsp(uint32_t dsp_offset, uint32_t count)
{
    volatile uint16_t *idma_pointer;
    idma_pointer = DSP_DM_ADDR(pcm_route.page_number, pcm_route.buffer_address);
    idma_pointer += dsp_offset;

    audio_idma_fill_to_dsp(idma_pointer, 0, count);
}

void pcm_route_put_silence_to_dsp_duplicate(uint32_t dsp_offset, uint32_t count)
{
    volatile uint16_t *idma_pointer;
    idma_pointer = DSP_DM_ADDR(pcm_route.page_number, pcm_route.buffer_address);
    idma_pointer += dsp_offset;

    audio_idma_fill_to_dsp(idma_pointer, 0, count << 1);
}

void pcm_route_fill_constant_to_dsp(uint16_t const_data, uint32_t dsp_offset, uint32_t count)
{
    volatile uint16_t *idma_pointer;
    idma_pointer = DSP_DM_ADDR(pcm_route.page_number, pcm_route.buffer_address);
    idma_pointer += dsp_offset;

    audio_idma_fill_to_dsp(idma_pointer, const_data, count);

}

void audio_manager_set_post_process_control(uint16_t mask, uint16_t control)
{
    uint32_t savedMask;
    uint16_t control_cache;

    savedMask = save_and_set_interrupt_mask();
    control_cache = *DSP_AUDIO_PP_CTRL & ~mask;
    control_cache |= ((control & mask) | 0x0001);
    *DSP_AUDIO_PP_CTRL |= control_cache;
    restore_interrupt_mask(savedMask);
}

void audio_manager_clear_post_process_control(uint16_t mask)
{
    uint32_t savedMask;

    savedMask = save_and_set_interrupt_mask();
    *DSP_AUDIO_PP_CTRL &= ~mask;
    restore_interrupt_mask(savedMask);
}

#endif /* HAL_I2S_MODULE_ENABLED */
