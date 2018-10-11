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

#ifndef __HEAAC_DECODER_H__
#define __HEAAC_DECODER_H__

#include <stdint.h>

typedef enum {
    HEAAC_DECODER_SBR_SIGNALING_DISABLED = 0, /* Only plain AAC is decoded and no up-sampling is performed.                               */
    HEAAC_DECODER_SBR_SIGNALING_ENABLED  = 1, /* SBR decoding enabled, up-sampling is always performed.                                   */
    HEAAC_DECODER_SBR_SIGNALING_AUTO     = 2  /* SBR decoding enabled, up-sampling is only done when SBR is actually present (suggested). */
} heaac_decoder_sbr_signaling_t;

typedef enum {
    HEAAC_DECODER_SBR_MODE_LP   = 0,  /* low power    : real value transforms (fast)                                 */
    HEAAC_DECODER_SBR_MODE_HQ   = 1,  /* high quality : complex value transforms (good quality but slow)             */
    HEAAC_DECODER_SBR_MODE_AUTO = 2   /* automatic    : high quality for mono; otherwise, low power mode (suggested) */
} heaac_decoder_sbr_mode_t;

typedef struct {
    uint32_t internal_buffer_size;  /* Required size of internal buffer (unit: Byte)          */
    uint32_t temporary_buffer_size; /* Required size of temporary buffer (unit: Byte)         */
    uint32_t pcm_buffer_size;       /* Required size of PCM buffer (unit: Byte)               */
    uint32_t bitstream_buffer_size; /* Minimum required size of bitstream buffer (unit: Byte) */
} heaac_decoder_buffer_size_t;

typedef struct {
    heaac_decoder_sbr_signaling_t sbr_signaling;        /* SBR operating options                */
    heaac_decoder_sbr_mode_t      sbr_mode;             /* SBR modes                            */
    int32_t                       force_stereo_flag;    /* Force output PCM data to be stereo
                                                           0 --> depends on bitstream
                                                           1 --> force stereo                   */
} heaac_decoder_initial_parameter_t;

typedef struct {
    uint32_t sampling_rate;     /* Sampling rate of the output PCM data (unit: Hz) */
    uint32_t channel_number;    /* Channel number of the output PCM data           */
    uint32_t frame_size;        /* PCM frame size per channel (unit: sample)       */
    uint32_t sbr_flag;          /* SBR bitstream detected or not
                                   0 --> no SBR
                                   1 --> with SBR                                  */
} heaac_decoder_runtime_information_t;

/*
    Description
        This function is used to get current version of the library.
    
    Return Value
        Version number
*/
uint32_t heaac_decoder_get_version (void);

/*
    Description
        This function get the buffer sizes for HEAAC decoder.
    
    Output Parameters
        buffer_size : Pointer to the buffer size information structure
    
    Return Value
        >= 0 : Normal
        <  0 : Error
*/
int32_t heaac_decoder_get_buffer_size (heaac_decoder_buffer_size_t *buffer_size);

/*
    Description
        This function initializes the internal buffer for HEAAC decoder.
    
    Input Parameters
        internal_buffer   : HEAAC decoder internal buffer
        initial_parameter : HEAAC decoder initial parameters
    
    Output Parameters
        handle_pointer    : pointer to the HEAAC decoder handle
    
    Return Value
        >= 0 : Normal
        <  0 : Error
*/
int32_t heaac_decoder_init (void **handle_pointer, uint8_t *internal_buffer, heaac_decoder_initial_parameter_t *initial_parameter);

/*
    Description
        This function decodes HEAAC bitstreams and output PCM data as the following:
        1.  1024 PCM samples per channel for pure AAC
        2.  2048 PCM samples per channel for HEAAC
    
    Input Parameters
        handle              : HEAAC decoder handle
        temp_buffer         : pointer to the temporary buffer
        input_buffer        : pointer to the input bitstream buffer
        input_byte_count    : available input bitstream size (unit: Byte)
        output_buffer       : pointer to the output PCM buffer
        output_byte_count   : available output PCM size (unit: Byte)

    Output Parameters
        input_byte_count    : consumed input bitstream size (unit: Byte)
        output_byte_count   : produced output PCM size (unit: Byte)
        runtime_information : HEAAC decoder runtime information

    Return Value
        >= 0 : Normal
        <  0 : Error
*/
int32_t heaac_decoder_process (void *handle, uint8_t *temp_buffer, uint8_t *input_buffer, uint32_t *input_byte_count, int16_t *output_buffer, uint32_t *output_byte_count, heaac_decoder_runtime_information_t *runtime_information);

/*
    Description
        This function is used to set optional HEAAC decoder parameter "dual_sce_config".
    
    Input Parameters
        handle          : HEAAC decoder handle
        dual_sce_config : 0      --> Output main channel, mono (default)
                          1      --> Output main channel, mono
                          2      --> Output sub-channel, mono
                          3      --> Output both channels, stereo
                          Others --> Undefined

    Return Value
        >= 0 : Normal
        <  0 : Error
*/
int32_t heaac_decoder_set_parameter_dual_sce_config (void *handle, int32_t dual_sce_config);

/*
    Description
        This function is used to set optional HEAAC decoder parameter "bypass_decode".
    
    Input Parameters
        handle        : HEAAC decoder handle
        bypass_decode : 0      --> Normal decode
                        1      --> Bypass decode
                        Others --> Undefined

    Return Value
        >= 0 : Normal
        <  0 : Error
*/
int32_t heaac_decoder_set_parameter_bypass_decode (void *handle, int32_t bypass_decode);

/*
    Description
        This function is used to set optional HEAAC decoder parameter "bypass_crc_check".
    
    Input Parameters
        handle           : HEAAC decoder handle
        bypass_crc_check : 0      --> Normal CRC check
                           1      --> Bypass CRC check
                           Others --> Undefined

    Return Value
        >= 0 : Normal
        <  0 : Error
*/
int32_t heaac_decoder_set_parameter_bypass_crc_check (void *handle, int32_t bypass_crc_check);

#endif  /* __HEAAC_DECODER_H__ */
