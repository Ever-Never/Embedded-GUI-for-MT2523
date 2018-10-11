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

#ifndef __HAL_I2S_INTERNAL_H__
#define __HAL_I2S_INTERNAL_H__

#include "hal_i2s.h"

#if defined(HAL_I2S_MODULE_ENABLED)

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "hal_audio_internal_pcm2way.h"

#define HAL_I2S_DSP_DECODE_CONCURRENT_ENABLED

#define I2S_RING_BUFFER_SIZE    4096  /**< Tocheck the size of Tx/Rx buffer */
#define I2S_RX_AUDDEC_FRM_LEN   1020

#define I2S_STATE_IDLE          0
#define I2S_STATE_RX_RUNNING    1
#define I2S_STATE_TX_RUNNING    2

typedef enum {
    HAL_I2S_RX      = 0,
    HAL_I2S_TX      = 1
} hal_i2s_link_t;

typedef struct {
    int16_t          *base;
    uint16_t         write;
    uint16_t         read;
    uint16_t         size;
} ring_buffer_t;

typedef struct {
    /* structure for hal i2s config */
    hal_i2s_initial_type_t  i2s_initial_type;
    hal_i2s_config_t        i2s_config;

    /* state record */
    uint16_t                i2s_state;
    bool                    is_running;
    /* internal used memory */
    uint8_t                 *memory_pool;
    uint32_t                memory_offset;      /**< Maximum = MAX_INTERNAL_USE_SIZE */
    uint32_t                audio_id;
    /* ring buffer */
    ring_buffer_t           rx_buffer;            /**< Rx buffer- internal use */
    ring_buffer_t           tx_buffer;           /**< Tx buffer- internal use */
    /* user defined callback functions */
    hal_i2s_tx_callback_t   tx_callback_func;
    hal_i2s_rx_callback_t   rx_callback_func;
    void                    *tx_data;
    void                    *rx_data;
    bool                    internal_loopback;
#ifdef HAL_I2S_DSP_DECODE_CONCURRENT_ENABLED
    bool                    dsp_decode_concurrency;
    uint16_t                *rx_aud_dec_buffer;
#endif
} hal_i2s_t;

/* check function: prototype definition */
void i2s_set_initial_parameter(hal_i2s_t *hal_i2s);
hal_i2s_status_t i2s_set_clock_mode(hal_i2s_clock_mode_t clock_mode);
hal_i2s_status_t i2s_is_master_sample_rate(hal_i2s_sample_rate_t sample_rate);
hal_i2s_status_t i2s_is_slave_sample_rate(hal_i2s_sample_rate_t sample_rate);
hal_i2s_status_t i2s_is_master_channel_number(hal_i2s_channel_number_t channel_number);
hal_i2s_status_t i2s_is_slave_channel_number(hal_i2s_channel_number_t channel_number);

/* prototype definition */
uint32_t i2s_get_free_space(ring_buffer_t *ring_buffer);
uint32_t i2s_get_data_count(ring_buffer_t *ring_buffer);
uint8_t *i2s_buffer_get_read_pointer(ring_buffer_t *ring_buffer);
uint8_t *i2s_buffer_get_write_pointer(ring_buffer_t *ring_buffer);
void i2s_get_free_space_two_section(ring_buffer_t *ring_buffer, uint32_t *free_length1, uint32_t *free_length2);
void i2s_get_data_count_two_section(ring_buffer_t *ring_buffer, uint32_t *data_length1, uint32_t *data_length2);
hal_i2s_status_t i2s_read_from_buffer(ring_buffer_t *ring_buffer, uint32_t read_length, void *buffer);
hal_i2s_status_t i2s_write_to_buffer(ring_buffer_t *ring_buffer, uint32_t write_length, void *buffer);
uint8_t *i2s_buffer_update_write_index(ring_buffer_t *ring_buffer, uint32_t offset);
uint8_t *i2s_buffer_update_read_index(ring_buffer_t *ring_buffer, uint32_t offset);

/* for internal-use memory */
uint16_t *i2s_alloc_internal_memory(uint32_t memory_size);

/* DSP */
void i2s_master_dsp_turn_on(void);
void i2s_master_dsp_turn_off(void);
audio_pcm2way_config_t *i2s_pcm2way_set_config(bool tx_on, bool rx_on);

void i2s_path_turn_on(hal_i2s_clock_mode_t i2s_clock_mode, hal_i2s_link_t i2s_link);
void i2s_path_turn_off(hal_i2s_clock_mode_t i2s_clock_mode);

void i2s_set_data_interrupt_rate(void);
bool i2s_is_legecy_in_used(void);
extern void pcm_route_start(void);
extern void pcm_route_stop(void);

bool i2s_is_dsp_decode_concurrency(void);
void i2s_rx_dsp_decode(bool on);

extern hal_i2s_t      hal_i2s;

#ifdef __cplusplus
}
#endif

#endif  /* defined(HAL_I2S_MODULE_ENABLED)*/

#endif /* __HAL_I2S_INTERNAL_H__ */
