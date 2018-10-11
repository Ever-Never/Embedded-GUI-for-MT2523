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

#ifndef __AT_COMMAND_AUDIO_TUNING_AUDIO_COEFFICIENT_H__
#define __AT_COMMAND_AUDIO_TUNING_AUDIO_COEFFICIENT_H__

#include "stdint.h"



#ifdef __cplusplus
extern "C" {
#endif



/*
 * @section Terms_Chapter Terms and acronyms
 *
 */

/*
 * @section Terms_Chapter Terms and acronyms
 *
 * |Terms               | Details                                                                                                         |
 * |--------------------|-----------------------------------------------------------------------------------------------------------------|
 * | \b NB              | Narrow band (8kHz sampling rate), which is commonly used to describe the speech signal sampling rate property.  |
 * | \b WB              | Wide band (16kHz sampling rate), which is commonly used to describe the speech signal sampling rate property.   |
 * | \b FIR             | Finite impulse response filter, which is one kind of the digital filter types for signal processing.            |
 * | \b IIR             | Infinite impulse response filter, which is one kind of the digital filter types for signal processing.          |
 */

/*
    Audio Modes:
    1.  Headphone
    2.  Loudspeaker (External PA)
    3.  External DAC (Master I2S)
    Voice Modes:
    1.  Headset (Headphone + Microphone)
    2.  Handset (Receiver + Microphone) / Handsfree (External PA + Microphone)
    3.  External DAC (Slave I2S / Master PCM)
    Record Modes:
    1.  Single Microphone from Handset (Record Enhancement @ Larkspur DSP)
    2.  Single Microphone from Headset (Record Enhancement @ Larkspur DSP)
    3.  Dual Microphone (Record Enhancement @ Smart Phone / Larkspur MCU)
 */
#define AMOUNT_OF_AUDIO_ACF_MODES       3
#define AMOUNT_OF_VOICE_MODES           3
#define AMOUNT_OF_RECORD_MODES          3
#define AMOUNT_OF_AUDIO_FILTERS         11

#define EAPS_SPEECH_COMMON_PARAMETER_SIZE 12
#define EAPS_AUDIO_IIR_FILTER_PARAMETER_SIZE 5
#define EAPS_VOICE_NB_MODE_PARAMETERS_PARAMETER_SIZE 48
#define EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE 45
#define EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE 45
#define EAPS_VOICE_WB_MODE_PARAMETERS_PARAMETER_SIZE 48
#define EAPS_VOICE_WB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE 90
#define EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE 90
#define EAPS_RECORD_NB_MODE_PARAMETERS_PARAMETER_SIZE 48
#define EAPS_RECORD_NB_CHANNEL_1_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE 90
#define EAPS_RECORD_NB_CHANNEL_2_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE 90
#define EAPS_RECORD_WB_MODE_PARAMETERS_PARAMETER_SIZE 48
#define EAPS_RECORD_WB_CHANNEL_1_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE 90
#define EAPS_RECORD_WB_CHANNEL_2_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE 90
#define EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE 16
#define EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE 1
#define EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE_TEMP_FOR_FUTURE_SIZE 7
#define EAPS_GAIN_PARAMETER_SIDETONE_GAIN_SIZE 1
#define EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE 16

#define EAPS_MAX_PARAMETER_SIZE 122


typedef struct {
    uint16_t stream_out_gain[EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE];
    uint16_t stream_in_gain[EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE_TEMP_FOR_FUTURE_SIZE];
    uint16_t sidetone_gain;
} gain_parameter_t;

typedef struct {
    gain_parameter_t handsfree_gain;
    gain_parameter_t headset_gain;
} handsfree_headset_gain_parameter_t;


typedef struct {
    uint16_t record_wb_mode_parameter[EAPS_RECORD_WB_MODE_PARAMETERS_PARAMETER_SIZE];
    uint16_t record_wb_channel_1_fir_coefficient[EAPS_RECORD_WB_CHANNEL_1_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE];
    uint16_t record_wb_channel_1_fir_frequency[EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE];
    uint16_t record_wb_channel_1_fir_response[EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE];
    uint16_t record_wb_channel_2_fir_coefficient[EAPS_RECORD_WB_CHANNEL_2_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE];
    uint16_t record_wb_channel_2_fir_frequency[EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE];
    uint16_t record_wb_channel_2_fir_response[EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE];
} record_wb_enhancement_parameter_t;

typedef struct {
    gain_parameter_t record_wb_gain;
    record_wb_enhancement_parameter_t record_wb_enhancement_parameter;
} record_wb_parameter_t;

typedef struct {
    record_wb_parameter_t record_wb_parameter[AMOUNT_OF_RECORD_MODES];
} record_parameter_t;


typedef struct {
    uint16_t wb_mode_parameter[EAPS_VOICE_WB_MODE_PARAMETERS_PARAMETER_SIZE];
    uint16_t wb_stream_in_fir_coefficient[EAPS_VOICE_WB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE];
    uint16_t wb_stream_in_fir_frequency[EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE];
    uint16_t wb_stream_in_fir_response[EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE];
    uint16_t wb_stream_out_fir_coefficient[EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE];
    uint16_t wb_stream_out_fir_frequency[EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE];
    uint16_t wb_stream_out_fir_response[EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE];
} voice_wb_enhancement_parameter_t;

typedef struct {
    gain_parameter_t voice_wb_gain;
    voice_wb_enhancement_parameter_t voice_wb_enhancement_parameter;
} voice_wb_parameter_t;

typedef struct {
    uint16_t nb_mode_parameter[EAPS_VOICE_NB_MODE_PARAMETERS_PARAMETER_SIZE];
    uint16_t nb_stream_in_fir_coefficient[EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE];
    uint16_t nb_stream_in_fir_frequency[EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE];
    uint16_t nb_stream_in_fir_response[EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE];
    uint16_t nb_stream_out_fir_coefficient[EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE];
    uint16_t nb_stream_out_fir_frequency[EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE];
    uint16_t nb_stream_out_fir_response[EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE];
} voice_nb_enhancement_parameter_t;

typedef struct {
    gain_parameter_t voice_nb_gain;
    voice_nb_enhancement_parameter_t voice_nb_enhancement_parameter;
} voice_nb_parameter_t;

typedef struct {
    voice_nb_parameter_t voice_nb_parameter[AMOUNT_OF_VOICE_MODES];
    voice_wb_parameter_t voice_wb_parameter[AMOUNT_OF_VOICE_MODES];
} voice_parameter_t;


typedef struct {
    uint32_t audio_iir_design_parameter[EAPS_AUDIO_IIR_FILTER_PARAMETER_SIZE];
} audio_post_processing_compensation_filter_t;

typedef struct {
    audio_post_processing_compensation_filter_t audio_post_processing_compensation_filter[AMOUNT_OF_AUDIO_FILTERS];
} audio_post_processing_parameter_t;

typedef struct {
    handsfree_headset_gain_parameter_t audio_handsfree_headset_gain_parameter;
    audio_post_processing_parameter_t audio_post_processing_parameter[AMOUNT_OF_AUDIO_ACF_MODES];
} audio_parameter_t;


typedef struct {
    uint16_t speech_common_parameter[EAPS_SPEECH_COMMON_PARAMETER_SIZE];
} speech_common_parameter_t;


typedef struct {
    speech_common_parameter_t speech_common_parameter;
    audio_parameter_t audio_parameter;
    voice_parameter_t voice_parameter;
    record_parameter_t record_parameter;
} audio_eaps_t;



#if PRODUCT_VERSION == 2533

#define EXTERNAL_DSP_REGISTER_SENDING_PATH_INDEX_AMOUNT 640

typedef struct {
uint16_t external_dsp_register_value[EXTERNAL_DSP_REGISTER_SENDING_PATH_INDEX_AMOUNT];
uint8_t need_to_download_index[EXTERNAL_DSP_REGISTER_SENDING_PATH_INDEX_AMOUNT];
} external_dsp_sending_path_register_value_t;

#endif



#ifdef __cplusplus
}
#endif

#endif  /* __AT_COMMAND_AUDIO_TUNING_AUDIO_COEFFICIENT_H__ */
