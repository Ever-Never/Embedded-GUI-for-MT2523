/* Copyright Statement:
 *
 * (C) 2005-2017  MediaTek Inc. All rights reserved.
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


#ifndef __WAV_CODEC_INTERNAL_H__
#define __WAV_CODEC_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "syslog.h"
#include <string.h>
#include "semphr.h"
#include "audio_codec.h"
#include "wav_codec_exp.h"

#ifndef HAL_AUDIO_MODULE_ENABLED
#error "please turn on audio feature option on hal_feature_config.h"
#endif
#include "hal_audio_internal_service.h"
#include "hal_audio.h"
#include "hal_log.h"

#define TASK_ERROR_LOGGING
#define TASK_WARNING_LOGGING
#define TASK_PROCESS_LOGGING
// #define TASK_DEBUG_LOGGING
// log_create_module(WAV_CODEC, PRINT_LEVEL_INFO);

#if defined(TASK_ERROR_LOGGING)
#define TASK_LOG_E(fmt,arg...)      log_hal_info("[WAV_CODEC][Error] " fmt,##arg)
#else
#define TASK_LOG_E(fmt,arg...)
#endif

#if defined(TASK_WARNING_LOGGING)
#define TASK_LOG_W(fmt,arg...)      log_hal_info("[WAV_CODEC][Warning] " fmt,##arg)
#else
#define TASK_LOG_W(fmt,arg...)
#endif

#if defined(TASK_PROCESS_LOGGING)
#define TASK_LOG_I(fmt,arg...)      log_hal_info("[WAV_CODEC][Info] " fmt,##arg)
#else
#define TASK_LOG_I(fmt,arg...)
#endif

#if defined(TASK_DEBUG_LOGGING)
#define TASK_LOG_D(fmt,arg...)      log_hal_info("[WAV_CODEC][Debug] " fmt,##arg)
#else
#define TASK_LOG_D(fmt,arg...)
#endif

#if 0
#define FUNCTION_IN  TASK_LOG_D("+\n")
#define FUNCTION_OUT TASK_LOG_D("-\n")
#else
#define FUNCTION_IN
#define FUNCTION_OUT
#endif

#define WAV_CODEC_UT_FLOW

#if 0
#define WAV_CODEC_DECODE_PCM_OUTPUT_SIZE      (2048) // IP output
#define WAV_CODEC_DECODE_PCM_RING_BUFFER_SIZE (WAV_CODEC_DECODE_PCM_OUTPUT_SIZE*2) // buffering
#define WAV_CODEC_WAV_BITSTREAM_SIZE          (WAV_CODEC_DECODE_PCM_OUTPUT_SIZE*2) /*It should be a multiple number of 4. minimum size is 256*/ // IP input
#define WAV_CODEC_BS_RING_BUFFER_SIZE         (WAV_CODEC_WAV_BITSTREAM_SIZE)
#endif /* HAL_AUDIO_LOW_POWER_ENABLED */

#define WAV_CODEC_WAV_BITSTREAM_SIZE (16 * 1024) /*It should be a multiple number of 4. minimum size is 256*/ // IP input
#define WAV_CODEC_DECODE_PCM_OUTPUT_SIZE (WAV_CODEC_WAV_BITSTREAM_SIZE*4) // IP output
#define WAV_CODEC_DECODE_PCM_RING_BUFFER_SIZE (WAV_CODEC_WAV_BITSTREAM_SIZE * 4) // buffering
#define WAV_CODEC_BS_RING_BUFFER_SIZE (WAV_CODEC_WAV_BITSTREAM_SIZE * 2) // user fill bs data to dirver

/** @brief audio codec state. */
typedef enum {
    WAV_CODEC_STATUS_NONE          = 0,
    WAV_CODEC_STATUS_ERROR         = 1 << 0,
    WAV_CODEC_STATUS_DSP_UNDERFLOW = 1 << 1,
    WAV_CODEC_STATUS_IN_TASK_LIST  = 1 << 2,
} wav_codec_state_t;

typedef struct {
    audio_codec_media_handle_t  handle;                     /** Wav codec handle. */
    WAV_DEC_HDL                 *wav_handle;                /** Wav codec IP handle. */

    wav_codec_state_t           status;
    ring_buffer_information_t   decoded_pcm_ring;
    bool                        linear_data;
    bool                        eof_flag;
    bool                        decode_end_flag;
    uint8_t                     *decoder_working_buf;       /** Wav codec IP working buffer. */
    uint8_t                     *decoder_input_buf;         /** bitstream buffer*/
    uint8_t                     *decoder_output_buf;        /** pcm buffer*/
    unsigned int                decoder_input_buf_offset;   /** record consumed data(decoder use) offset*/
    unsigned int                decoder_output_buf_offset;  /** record consumed data(fill to decodec_pcm_ring) offset*/
    unsigned int                decoder_working_buf_size;   /** Wav codec IP working buffer size. */
    unsigned int                decoder_input_buf_size;     /** bitstream buffer size*/
    unsigned int                decoder_output_buf_size;    /** pcm buffer size*/
    unsigned int                decode_size;
    unsigned int                remain_bs;                  /* the total audio data number (bytes) in the file */
    SemaphoreHandle_t           semaphore_handle;
#ifdef WAV_CODEC_UT_FLOW
    uint8_t                     *pcm_ring;                  /** pcm ring buffer. */
    unsigned int                pcm_ring_size;              /** pcm ring buffer size. */
#endif
} wav_codec_internal_handle_t;


#ifdef __cplusplus
}
#endif

#endif  /*__WAV_CODEC_INTERNAL_H__*/

