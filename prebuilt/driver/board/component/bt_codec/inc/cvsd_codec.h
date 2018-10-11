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

#ifndef __CVSD_CODEC_H__
#define __CVSD_CODEC_H__

#include <stdint.h>

/*
    Output Parameter
        internal_buffer_size    CVSD decoder internal buffer size, unit: byte
*/

uint32_t cvsd_decoder_get_buffer_size (void);

/*
    Output Parameter
        internal_buffer_size    CVSD encoder internal buffer size, unit: byte
*/

uint32_t cvsd_encoder_get_buffer_size (void);

/*
    Input Parameters
        internal_buffer     CVSD decoder internal buffer
    Output Parameters
        handle_pointer      pointer to the CVSD decoder handle
    Return Value
        >= 0    Normal
        < 0     Error
*/

int32_t cvsd_decode_init (void **handle_pointer, uint8_t *internal_buffer);

/*
    Input Parameters
        internal_buffer     CVSD encoder internal buffer
    Output Parameters
        handle_pointer      pointer to the CVSD encoder handle
    Return Value
        >= 0    Normal
        < 0     Error
*/

int32_t cvsd_encode_init (void **handle_pointer, uint8_t *internal_buffer);

/*
    Input Parameters
        handle              CVSD decoder handle
        input_bitstream     pointer to the input bitstream buffer
        input_byte_count    available input bitstream buffer size, unit: byte
        output_pcm          pointer to the output PCM buffer
        output_byte_count   available output PCM buffer size, unit: byte
    Output Parameters
        input_byte_count    consumed input bitstream buffer size, unit: byte
        output_byte_count   produced output PCM buffer size, unit: byte
    Return Value
        >= 0    Normal
        < 0     Error
*/

int32_t cvsd_decode_process (
    void *handle,
    uint8_t *input_bitstream,
    uint32_t *input_byte_count,
    int16_t *output_pcm,
    uint32_t *output_byte_count
);

/*
    Input Parameters
        handle              CVSD encoder handle
        input_pcm           pointer to the input PCM buffer
        input_byte_count    available input PCM buffer size, unit: byte
        output_bitstream    pointer to the output bitstream buffer
        output_byte_count   available output bitstream buffer size, unit: byte
    Output Parameters
        input_byte_count    consumed input PCM buffer size, unit: byte
        output_byte_count   produced output bitstream buffer size, unit: byte
    Return Value
        >= 0    Normal
        < 0     Error
*/

int32_t cvsd_encode_process (
    void *handle,
    int16_t *input_pcm,
    uint32_t *input_byte_count,
    uint8_t *output_bitstream,
    uint32_t *output_byte_count
);

uint32_t cvsd_src_down_sampling_get_buffer_size (void);
uint32_t cvsd_src_up_sampling_get_buffer_size   (void);
int32_t cvsd_src_down_sampling_init (void **handle_pointer, uint8_t *internal_buffer);
int32_t cvsd_src_up_sampling_init   (void **handle_pointer, uint8_t *internal_buffer);
int32_t cvsd_src_down_sampling_process (void *hdl, int16_t *p_tmp_buf, int16_t *p_in_buf, int16_t *p_ou_buf, uint32_t in_byte_cnt);
int32_t cvsd_src_up_sampling_process   (void *hdl, int16_t *p_tmp_buf, int16_t *p_in_buf, int16_t *p_ou_buf, uint32_t in_byte_cnt);

#endif  /* __CVSD_CODEC_H__ */
