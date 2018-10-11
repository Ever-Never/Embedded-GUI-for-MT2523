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


#ifndef __AAC_DECODER_API_INTERNAL_H__
#define __AAC_DECODER_API_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "syslog.h"
#include <string.h>
#include "memory_attribute.h"
#include "task_def.h"
#include "hal_platform.h"
#include "hal_gpt.h"
#ifndef HAL_AUDIO_MODULE_ENABLED
#error "please turn on audio feature option on hal_feature_config.h"
#endif
#include "hal_audio.h"
#include "hal_audio_internal_service.h"
#include "aac_decoder_api.h"
#ifdef MTK_AUDIO_AAC_DECODER_FEATURE_HEAAC
#include "heaac_decoder.h"
#else
#include "aac_decoder.h"
#endif

#define     SHARE_BUFFER_MINIMUM_SIZE     8192
#define     SHARE_BUFFER_CHECK_SIZE       1024
#define     MAX_ERROR_FRAME               10      /** The maximum number of continous error frame.*/
#define     MAX_AAC_DECODER_FUNCTION      3       /** The maximum number of function the queue can hold at any one time.*/
#define     AAC_DECODER_QUEUE_SIZE        10      /** The maximum number of items the queue can hold at any one time.*/
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
#define     PRE_BUFF_PCM_MULTIPLE         3*2     /** The mutliple number of pcm buffer.*/
#else
#define     PRE_BUFF_PCM_MULTIPLE         3       /** The mutliple number of pcm buffer.*/
#endif
#define     AAC_DECODER_API_LOG_ENABLE
#define     SHARE_BUFFER_SLEEP_TIME_IN_MS 20
#define     SHARE_BUFFER_SLEEP_LOOP_TIMES 10


log_create_module(AAC_DECODER_API, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(AAC_DECODER_API, "[AAC DEC]: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(AAC_DECODER_API, "[AAC DEC]: "fmt,##arg)
#if defined AAC_DECODER_API_LOG_ENABLE
#define LOGI(fmt,arg...)   LOG_I(AAC_DECODER_API, "[AAC DEC]: "fmt,##arg)
#else
#define LOGI(fmt,arg...)
#endif


typedef enum {
    AAC_DECODER_API_QUEUE_EVENT_NONE = 0,
    AAC_DECODER_API_QUEUE_EVENT_DECODE,
    AAC_DECODER_API_QUEUE_EVENT_TOTAL
} aac_decoder_api_queue_event_id_t;

/** @brief This structure defines the AAC decoder SBR operating option. */
typedef enum {
    AAC_DECODER_API_SBR_SIGNALING_DISABLED = 0,     /**< Only plain AAC is decoded and no upsampling is performed. */
    AAC_DECODER_API_SBR_SIGNALING_ENABLED  = 1,     /**< SBR decoding enabled, upsampling is always performed. */
    AAC_DECODER_API_SBR_SIGNALING_AUTO     = 2      /**< SBR decoding enabled, upsampling is only done when SBR is actually present (suggested). */
} aac_decoder_api_sbr_signaling_t;


/** @brief This structure defines the AAC decoder SBR mode. */
typedef enum {
    AAC_DECODER_API_SBR_MODE_LP   = 0,      /**< low power mode   : real value transforms (fast decoding). */
    AAC_DECODER_API_SBR_MODE_HQ   = 1,      /**< high quality mode: complex value transforms (good quality but slow decoding). */
    AAC_DECODER_API_SBR_MODE_AUTO = 2       /**< automatic : high quality for mono; otherwise, low power mode (suggested). */
} aac_decoder_api_sbr_mode_t;


/**
 *  @brief This structure defines the AAC decoder configuration.
 */
typedef struct {
    aac_decoder_api_sbr_signaling_t sbr_signaling;  /**< SBR signaling, only works for HE-AAC V1. #AAC_DECODER_SBR_SIGNALING_AUTO is the default setting. */
    aac_decoder_api_sbr_mode_t sbr_mode;            /**< SBR mode, only works for HE-AAC V1. #AAC_DECODER_SBR_MODE_AUTO is the default setting. */
} aac_decoder_api_initial_parameter_t;


typedef struct {
    aac_decoder_api_media_handle_t          handle;                 /** AAC deocder media handle. */
    aac_decoder_api_runtime_information_t   runtime_info;           /** defines the AAC decoder runtime information. */
    ring_buffer_information_t               pcm_out_buffer;         /** The pcm output buffer is for pcm playback. */
#ifdef MTK_AUDIO_AAC_DECODER_FEATURE_HEAAC
    heaac_decoder_initial_parameter_t       init_param;             /** AAC decoder IP used.*/
    heaac_decoder_runtime_information_t     info;                   /** AAC decoder IP used.*/
#else
    aac_decoder_runtime_information_t       info;                   /** AAC decoder IP used.*/
#endif
    void                                    *dec_handle;            /** AAC decoder IP used.*/
    uint8_t                                 *bitstream_buffer;      /** AAC decoder IP used.The pointer to bitstream buffer. */
    uint8_t                                 *internal_buffer;       /** AAC decoder IP used.The pointer to internal buffer. */
    uint8_t                                 *temp_buffer;           /** AAC decoder IP used. The pointer to temporary  buffer */
    int16_t                                 *pcm_buffer;            /** AAC decoder IP used. Store decoded pcm data.*/
    uint8_t                                 *audio_buffer;

    uint32_t                                internal_buffer_size;   /**< Internal buffer size of the decoder. (unit: Byte) */
    uint32_t                                temp_buffer_size;       /**< Temporary buffer size of the decoder. (unit: Byte)  */
    uint32_t                                pcm_buffer_size;        /**< PCM  buffer size of the decoder. (unit: Byte)  */
    uint32_t                                pcm_out_buffer_size;    /**< PCM output buffer size of the decoder. (unit: Byte)  */
    uint32_t                                bitstream_buffer_size;  /**< Minimum required size of bitstream buffer (unit: Byte)*/
    uint32_t                                read_buffer_size;       /**< Size of read buffer (unit: Byte)*/
    uint32_t                                remained_buf_size;      /**< Remained size of bitstream buffer (unit: Byte)*/
    uint32_t                                frame;                  /**< Counts of frame.*/
    uint32_t                                error_frame;            /**< Counts of error frame.*/
    bool                                    flush;                  /**< Counts of error.*/
    bool                                    first_play;
} aac_decoder_api_internal_handle_t;


typedef struct {
    aac_decoder_api_queue_event_id_t id;
    void *parameter;
} aac_decoder_api_queue_event_t;


typedef void (*aac_decoder_api_internal_callback_t)(void *parameter);


#ifdef __cplusplus
}
#endif

#endif  /*__AAC_DECODER_API_INTERNAL_H__*/

