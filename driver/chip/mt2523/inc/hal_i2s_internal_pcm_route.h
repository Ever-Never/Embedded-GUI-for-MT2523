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

#ifndef __HAL_I2S_INTERNAL_PCM_ROUTE_H__
#define __HAL_I2S_INTERNAL_PCM_ROUTE_H__

#include "hal_i2s.h"

#if defined(HAL_I2S_MODULE_ENABLED)

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_i2s_internal.h"
#include "hal_audio_fw_sherif.h"
#include "hal_audio_fw_interface.h"

#define PCM_ROUTE_MASK      0x80
#define PCM_ROUTE_ON        0x81

extern hal_i2s_t      hal_i2s;

typedef struct pcm_route_handle_s pcm_route_handle_t;
struct pcm_route_handle_s {
    void (*write_data_to_dsp)(uint16_t *buffer, uint32_t dsp_offset, uint32_t length);
    void (*write_silence_to_dsp)(uint32_t dsp_offset, uint32_t count);
    void (*read_data_from_dsp)(uint16_t *buffer, uint32_t dsp_offset, uint32_t length);
};

typedef struct {
    void (*isr_handler)(void);
    /* pcm route control flag */
    bool        isr_enable;
    int32_t     pcm_route_id;

    /* pcm route information from DSP */
    uint16_t    buffer_address;
    uint16_t    channel_number;
    uint16_t    page_number;
    uint16_t    data_length_get;
    uint16_t    data_length_put;
    bool        stereo2mono;
    bool        mono2stereo;

    /* user input buffer */
    ring_buffer_t    *rx_buffer;            /**< Rx buffer- internal use */
    ring_buffer_t    *tx_buffer;            /**< Tx buffer- internal use */
    pcm_route_handle_t  handle;
} pcm_route_t;

void pcm_route_get_length_info(uint32_t *sample_count_get, uint32_t *sample_count_put);
void pcm_route_get_data_from_dsp(uint16_t *buffer, uint32_t dsp_offset, uint32_t length);
void pcm_route_get_data_from_dsp_dropR(uint16_t *buffer, uint32_t dsp_offset, uint32_t length);
void pcm_route_put_data_to_dsp(uint16_t *buffer, uint32_t dsp_offset, uint32_t length);
void pcm_route_put_data_to_dsp_duplicate(uint16_t *buffer, uint32_t dsp_offset, uint32_t length);
void pcm_route_put_silence_to_dsp(uint32_t dsp_offset, uint32_t count);
void pcm_route_put_silence_to_dsp_duplicate(uint32_t dsp_offset, uint32_t count);

void pcm_route_start(void);
void pcm_route_stop(void);
void pcm_route_turn_on_interrupt(void);
void pcm_route_turn_off_interrupt(void);

void pcm_route_isr(void *data);
void pcm_route_callback(void);
void pcm_route_isr_handler(void);

void audio_manager_set_post_process_control(uint16_t mask, uint16_t control);
void audio_manager_clear_post_process_control(uint16_t mask);

#ifdef __cplusplus
}
#endif

#endif  /* defined(HAL_I2S_MODULE_ENABLED)*/

#endif /* __HAL_I2S_INTERNAL_PCM_ROUTE_H__ */
