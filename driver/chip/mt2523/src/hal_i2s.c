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

#include "hal_i2s.h"

#ifdef HAL_I2S_MODULE_ENABLED

#include "hal_i2s_internal.h"

hal_i2s_t      hal_i2s;

hal_i2s_status_t hal_i2s_init(hal_i2s_initial_type_t i2s_initial_type)
{
    if (hal_i2s.i2s_state != I2S_STATE_IDLE) {
        return HAL_I2S_STATUS_ERROR;
    }
    switch (i2s_initial_type) {
        case HAL_I2S_TYPE_EXTERNAL_MODE:
        case HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE:
            break;
        default:
            return HAL_I2S_STATUS_INVALID_PARAMETER;
    }
    i2s_set_initial_parameter(&hal_i2s);
    hal_i2s.i2s_initial_type = i2s_initial_type;

    return HAL_I2S_STATUS_OK;
}

hal_i2s_status_t hal_i2s_deinit(void)
{
    if (hal_i2s.i2s_state != I2S_STATE_IDLE) {
        return HAL_I2S_STATUS_ERROR;
    }
    hal_i2s.i2s_initial_type = HAL_I2S_TYPE_EXTERNAL_MODE;

    return HAL_I2S_STATUS_OK;
}

hal_i2s_status_t hal_i2s_set_config(const hal_i2s_config_t *config)
{
    if (!config) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }
    if (hal_i2s.i2s_state != I2S_STATE_IDLE) {
        return HAL_I2S_STATUS_ERROR;
    }

    //check clock mode
    if (i2s_set_clock_mode(config->clock_mode) != HAL_I2S_STATUS_OK) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }

    //check channel number & sample rate
    if (config->clock_mode == HAL_I2S_MASTER) {
        if ((config->i2s_in.sample_rate == config->i2s_out.sample_rate)
                && (i2s_is_master_sample_rate(config->i2s_in.sample_rate) == HAL_I2S_STATUS_OK)) {
            hal_i2s.i2s_config.i2s_in.sample_rate = config->i2s_in.sample_rate;
            hal_i2s.i2s_config.i2s_out.sample_rate = config->i2s_out.sample_rate;
        } else {
            return HAL_I2S_STATUS_INVALID_PARAMETER;
        }
        if ((i2s_is_master_channel_number(config->i2s_in.channel_number) == HAL_I2S_STATUS_OK)
                && (i2s_is_master_channel_number(config->i2s_out.channel_number) == HAL_I2S_STATUS_OK)) {
            hal_i2s.i2s_config.i2s_in.channel_number = config->i2s_in.channel_number;
            hal_i2s.i2s_config.i2s_out.channel_number = config->i2s_out.channel_number;
        } else {
            return HAL_I2S_STATUS_INVALID_PARAMETER;
        }

        if (hal_i2s.i2s_initial_type == HAL_I2S_TYPE_INTERNAL_LOOPBACK_MODE) {
            hal_i2s.internal_loopback = true;
        } else {
            hal_i2s.internal_loopback = false;
        }
    } else {    //HAL_I2S_SLAVE
        if ((config->i2s_in.sample_rate == config->i2s_out.sample_rate)
                & (i2s_is_slave_sample_rate(config->i2s_in.sample_rate) == HAL_I2S_STATUS_OK)) {
            hal_i2s.i2s_config.i2s_in.sample_rate = config->i2s_in.sample_rate;
            hal_i2s.i2s_config.i2s_out.sample_rate = config->i2s_out.sample_rate;
        } else {
            return HAL_I2S_STATUS_INVALID_PARAMETER;
        }

        if ((config->i2s_in.channel_number == config->i2s_out.channel_number)
                && (i2s_is_slave_channel_number(config->i2s_in.channel_number) == HAL_I2S_STATUS_OK)) {
            hal_i2s.i2s_config.i2s_in.channel_number = config->i2s_in.channel_number;
            hal_i2s.i2s_config.i2s_out.channel_number = config->i2s_out.channel_number;
        } else {
            return HAL_I2S_STATUS_INVALID_PARAMETER;
        }
    }
    return HAL_I2S_STATUS_OK;
}

hal_i2s_status_t hal_i2s_get_config(hal_i2s_config_t *config)
{
    if (!config) {
        return HAL_I2S_STATUS_INVALID_PARAMETER;
    }
    *config = hal_i2s.i2s_config;
    return HAL_I2S_STATUS_OK;
}

hal_i2s_status_t hal_i2s_get_memory_size(uint32_t *memory_size)
{
#ifdef HAL_I2S_DSP_DECODE_CONCURRENT_ENABLED
    if (hal_i2s.dsp_decode_concurrency) {
        *memory_size = I2S_RX_AUDDEC_FRM_LEN * 2;
    } else
#endif
        if (hal_i2s.i2s_config.clock_mode == HAL_I2S_MASTER) {
            *memory_size = I2S_RING_BUFFER_SIZE * 2;
        } else {    //HAL_I2S_SLAVE
            if (hal_i2s.i2s_config.i2s_in.sample_rate == HAL_I2S_SAMPLE_RATE_8K) {
                *memory_size = NB_BUFFER_SIZE * 2 * 2; //word to byte, both TX/RX
            } else {
                *memory_size = WB_BUFFER_SIZE * 2 * 2;
            }
        }
    return HAL_I2S_STATUS_OK;
}


hal_i2s_status_t hal_i2s_set_memory(uint8_t *memory)
{
    if (!memory) {
        return HAL_I2S_STATUS_ERROR;
    }
    hal_i2s.memory_pool = memory;
    hal_i2s.memory_offset = 0;

#ifdef HAL_I2S_DSP_DECODE_CONCURRENT_ENABLED
    if (hal_i2s.dsp_decode_concurrency) {
        hal_i2s.rx_aud_dec_buffer = (uint16_t *)hal_i2s.memory_pool;
    } else
#endif
        if (hal_i2s.i2s_config.clock_mode == HAL_I2S_MASTER) {
            hal_i2s.rx_buffer.write = 0;
            hal_i2s.rx_buffer.read = 0;
            hal_i2s.rx_buffer.size = I2S_RING_BUFFER_SIZE;
            hal_i2s.rx_buffer.base = (int16_t *)i2s_alloc_internal_memory(hal_i2s.rx_buffer.size);
            if (!hal_i2s.rx_buffer.base) {
                return HAL_I2S_STATUS_ERROR;
            }

            hal_i2s.tx_buffer.write = 0;
            hal_i2s.tx_buffer.read = 0;
            hal_i2s.tx_buffer.size = I2S_RING_BUFFER_SIZE;
            hal_i2s.tx_buffer.base = (int16_t *)i2s_alloc_internal_memory(hal_i2s.tx_buffer.size);
            if (!hal_i2s.tx_buffer.base) {
                return HAL_I2S_STATUS_ERROR;
            }
        } else {    //HAL_I2S_SLAVE
            uint32_t buffer_byte_size = 0;
            if (hal_i2s.i2s_config.i2s_in.sample_rate == HAL_I2S_SAMPLE_RATE_8K) {
                buffer_byte_size = NB_BUFFER_SIZE * 2;
            } else {
                buffer_byte_size = WB_BUFFER_SIZE * 2;
            }
            hal_i2s.rx_buffer.size = buffer_byte_size;
            hal_i2s.rx_buffer.base = (int16_t *)i2s_alloc_internal_memory(hal_i2s.rx_buffer.size);

            if (!hal_i2s.rx_buffer.base) {
                return HAL_I2S_STATUS_ERROR;
            }
            hal_i2s.tx_buffer.size = buffer_byte_size;
            hal_i2s.tx_buffer.base = (int16_t *)i2s_alloc_internal_memory(hal_i2s.tx_buffer.size);
            if (!hal_i2s.tx_buffer.base) {
                return HAL_I2S_STATUS_ERROR;
            }
        }

    return HAL_I2S_STATUS_OK;
}

hal_i2s_status_t hal_i2s_get_memory_pointer(uint8_t **memory)
{
    *memory = hal_i2s.memory_pool;
    return HAL_I2S_STATUS_OK;
}

hal_i2s_status_t hal_i2s_register_tx_callback(hal_i2s_tx_callback_t tx_callback, void *user_data)
{
    hal_i2s.tx_callback_func = tx_callback;
    hal_i2s.tx_data = user_data;
    return HAL_I2S_STATUS_OK;
}

hal_i2s_status_t hal_i2s_register_rx_callback(hal_i2s_rx_callback_t rx_callback, void *user_data)
{
    hal_i2s.rx_callback_func = rx_callback;
    hal_i2s.rx_data = user_data;
    return HAL_I2S_STATUS_OK;
}

hal_i2s_status_t hal_i2s_enable_tx(void)
{
    if (!(hal_i2s.i2s_state & I2S_STATE_TX_RUNNING)) {
        if (!hal_i2s.tx_callback_func) {
            return HAL_I2S_STATUS_ERROR;
        }
        if (hal_i2s.i2s_state == I2S_STATE_IDLE) {
            hal_i2s.is_running = true;
            i2s_path_turn_on(hal_i2s.i2s_config.clock_mode, HAL_I2S_TX);
        }
        hal_i2s.i2s_state |= I2S_STATE_TX_RUNNING;

        return HAL_I2S_STATUS_OK;
    }
    return HAL_I2S_STATUS_ERROR;
}

hal_i2s_status_t hal_i2s_disable_tx(void)
{
    hal_i2s.i2s_state &= ~I2S_STATE_TX_RUNNING;

    if (hal_i2s.i2s_state == I2S_STATE_IDLE) {
        i2s_path_turn_off(hal_i2s.i2s_config.clock_mode);
        hal_i2s.is_running = false;
        hal_i2s.tx_buffer.base = NULL;
        hal_i2s.rx_buffer.base = NULL;
    }

    return HAL_I2S_STATUS_OK;
}

hal_i2s_status_t hal_i2s_tx_write(const void *buffer, uint32_t sample_count)
{
    if ((hal_i2s.i2s_state & I2S_STATE_TX_RUNNING) != I2S_STATE_TX_RUNNING) {
        return HAL_I2S_STATUS_ERROR;
    }

    if (hal_i2s.i2s_config.clock_mode == HAL_I2S_MASTER) {
        uint32_t free_space = i2s_get_free_space(&hal_i2s.tx_buffer);

        if ((sample_count == 0) || (sample_count > free_space)) {
            return HAL_I2S_STATUS_INVALID_PARAMETER;
        }
        return i2s_write_to_buffer(&hal_i2s.tx_buffer, sample_count, (void *)buffer);
    } else {    //HAL_I2S_SLAVE
        uint16_t *p_dst = (uint16_t *)hal_i2s.tx_buffer.base;
        uint32_t write_byte_count = hal_i2s.rx_buffer.size;

        memcpy(p_dst, buffer, write_byte_count);
        return HAL_I2S_STATUS_OK;
    }
}

hal_i2s_status_t hal_i2s_get_tx_sample_count(uint32_t *sample_count)
{
    if (hal_i2s.i2s_config.clock_mode == HAL_I2S_MASTER) {
        *sample_count = i2s_get_free_space(&hal_i2s.tx_buffer);
    } else {    //HAL_I2S_SLAVE
        *sample_count = hal_i2s.tx_buffer.size;
    }
    return HAL_I2S_STATUS_OK;
}

hal_i2s_status_t  hal_i2s_enable_rx(void)
{
    if (!(hal_i2s.i2s_state & I2S_STATE_RX_RUNNING)) {
        if (!hal_i2s.rx_callback_func) {
            return HAL_I2S_STATUS_ERROR;
        }
        if (hal_i2s.i2s_state == I2S_STATE_IDLE) {
            hal_i2s.is_running = true;
            i2s_path_turn_on(hal_i2s.i2s_config.clock_mode, HAL_I2S_RX);
        }
        hal_i2s.i2s_state |= I2S_STATE_RX_RUNNING;

        return HAL_I2S_STATUS_OK;
    }
    return HAL_I2S_STATUS_ERROR;
}

hal_i2s_status_t hal_i2s_disable_rx(void)
{
    hal_i2s.i2s_state &= ~I2S_STATE_RX_RUNNING;

    if (hal_i2s.i2s_state == I2S_STATE_IDLE) {
        i2s_path_turn_off(hal_i2s.i2s_config.clock_mode);
        hal_i2s.is_running = false;
        hal_i2s.tx_buffer.base = NULL;
        hal_i2s.rx_buffer.base = NULL;
    }
    return HAL_I2S_STATUS_OK;
}

hal_i2s_status_t hal_i2s_rx_read(void *buffer, uint32_t sample_count)
{
    if ((hal_i2s.i2s_state & I2S_STATE_RX_RUNNING) != I2S_STATE_RX_RUNNING) {
        return HAL_I2S_STATUS_ERROR;
    }

    if (hal_i2s.i2s_config.clock_mode == HAL_I2S_MASTER) {
        uint32_t data_count = i2s_get_data_count(&hal_i2s.rx_buffer);

        if ((sample_count == 0) || (sample_count > data_count)) {
            return HAL_I2S_STATUS_INVALID_PARAMETER;
        }
        return i2s_read_from_buffer(&hal_i2s.rx_buffer, sample_count, (void *)buffer);
    } else {    //HAL_I2S_SLAVE
        uint32_t read_byte_count = hal_i2s.rx_buffer.size;
        uint16_t *p_dst = (uint16_t *)buffer;

        memcpy((uint8_t *)p_dst, (uint8_t *)hal_i2s.rx_buffer.base, read_byte_count);
        return HAL_I2S_STATUS_OK;
    }
}

hal_i2s_status_t hal_i2s_get_rx_sample_count(uint32_t *sample_count)
{
    if (hal_i2s.i2s_config.clock_mode == HAL_I2S_MASTER) {
        *sample_count = i2s_get_data_count(&hal_i2s.rx_buffer);
    } else {    //HAL_I2S_SLAVE
        *sample_count = hal_i2s.rx_buffer.size;
    }
    return HAL_I2S_STATUS_OK;
}

#endif
