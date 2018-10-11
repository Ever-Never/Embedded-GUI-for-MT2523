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


#ifndef __MP3_CODEC_INTERNAL_H__
#define __MP3_CODEC_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "syslog.h"
#include <string.h>
#include "mp3_codec.h"
#include "mp3dec_exp.h"
#include "audip_exp.h"
#include "semphr.h"

#ifndef HAL_AUDIO_MODULE_ENABLED
#error "please turn on audio feature option on hal_feature_config.h"
#endif
#include "hal_audio_internal_service.h"
#include "hal_audio.h"
#include "hal_gpt.h"
#include "hal_log.h"

#define MAX_MP3_CODEC_FUNCTIONS      3

#define mp3_handle_ptr_to_internal_ptr(handle) (((mp3_codec_private_t *)(((mp3_codec_media_handle_t *)handle)->private_data))->mp3_codec_internal_hdl)
#define mp3_handle_ptr_to_queue_handle(handle) (((mp3_codec_private_t *)(((mp3_codec_media_handle_t *)handle)->private_data))->mp3_codec_queue_hdl)
#define mp3_handle_ptr_to_task_handle(handle) (((mp3_codec_private_t *)(((mp3_codec_media_handle_t *)handle)->private_data))->mp3_task_handle)
#define mp3_handle_ptr_to_queue_id_array(handle) (((mp3_codec_private_t *)(((mp3_codec_media_handle_t *)handle)->private_data))->mp3_codec_queue_id_array)
#define mp3_handle_ptr_to_queue_handler(handle) (((mp3_codec_private_t *)(((mp3_codec_media_handle_t *)handle)->private_data))->mp3_codec_queue_handler)
#define mp3_handle_ptr_to_audio_internal_ring_buffer(handle) (((mp3_codec_private_t *)(((mp3_codec_media_handle_t *)handle)->private_data))->audio_internal_ring_buffer)
#define mp3_handle_ptr_to_mp3_decode_buffer(handle) (((mp3_codec_private_t *)(((mp3_codec_media_handle_t *)handle)->private_data))->mp3_decode_buffer)
#define mp3_handle_ptr_to_previous_mp3_frame_size(handle) (((mp3_codec_private_t *)(((mp3_codec_media_handle_t *)handle)->private_data))->previous_mp3_frame_size)
#define mp3_handle_ptr_to_total_sample_count(handle) (((mp3_codec_private_t *)(((mp3_codec_media_handle_t *)handle)->private_data))->total_sample_count)
#define mp3_handle_ptr_to_share_buffer_waiting_cnt(handle) (((mp3_codec_private_t *)(((mp3_codec_media_handle_t *)handle)->private_data))->share_buffer_waiting_cnt)
#if defined(MTK_AUDIO_MIXER_SUPPORT)
#define mp3_handle_ptr_to_pcm_isr_cnt(handle) (((mp3_codec_private_t *)(((mp3_codec_media_handle_t *)handle)->private_data))->pcm_isr_cnt)
#define mp3_handle_ptr_to_is_bitstream_end_called(handle) (((mp3_codec_private_t *)(((mp3_codec_media_handle_t *)handle)->private_data))->is_bitstream_end_called)
#endif
#ifdef MTK_BT_AWS_ENABLE
#define mp3_handle_ptr_to_p_aws_buf(handle) (((mp3_codec_private_t *)(((mp3_codec_media_handle_t *)handle)->private_data))->p_aws_buf)
#endif

typedef enum {
    MP3_CODEC_QUEUE_EVENT_NONE = 0,
    MP3_CODEC_QUEUE_EVENT_DECODE,
    MP3_CODEC_QUEUE_EVENT_FILL_DATA,
    MP3_CODEC_QUEUE_EVENT_TOTAL
} mp3_codec_queue_event_id_t;


typedef struct {
    mp3_codec_media_handle_t    handle;                         /** MP3 codec handle */
    ring_buffer_information_t   decode_pcm_buff;                /** MP3 codec IP used. the mp3 codec IP will put decoded data in this buffer */
    ring_buffer_information_t   stream_out_pcm_buff;            /** This buffer contains the pcm data that are ready to play */
    Mp3dec_handle               *mp3_handle;                    /** MP3 codec IP used.  */

    void                        *working_buff1;                 /** MP3 codec IP used. The pointer to Working buffer1  */
    void                        *working_buff2;                 /** MP3 codec IP used.The pointer to Working buffer2  */
    uint8_t                     *memory_pool;
    bool                        IsMP3Exit;                      /** Let user know the MP3 codec is available or not*/
    uint32_t                    share_buff_size;                /* The share buffer size */
    uint32_t                    working_buff1_size;             /* The required Working buffer1 size in byte */
    uint32_t                    working_buff2_size;             /* The required Working buffer2 size in byte */
    uint32_t                    decode_pcm_buffer_size;         /* The required PCM buffer size for codec used. in byte */
    uint32_t                    stream_out_pcm_buff_size;       /* Stream out pcm buffer size */
    bool                        aws_flag;                       /* Advanced wireless Stereo */
    SemaphoreHandle_t           semaphore_handle;
    int32_t                     media_bitstream_end;
} mp3_codec_internal_handle_t;


typedef struct {
    mp3_codec_queue_event_id_t id;
    void *parameter;
} mp3_codec_queue_event_t;


typedef void (*mp3_codec_internal_callback_t)(void *parameter);

typedef struct {
    mp3_codec_internal_handle_t    *mp3_codec_internal_hdl;                         /** MP3 codec internal handle */
    PRIVILEGED_DATA QueueHandle_t  mp3_codec_queue_hdl;
    TaskHandle_t mp3_task_handle;
    mp3_codec_queue_event_id_t mp3_codec_queue_id_array[MAX_MP3_CODEC_FUNCTIONS];
    mp3_codec_internal_callback_t mp3_codec_queue_handler[MAX_MP3_CODEC_FUNCTIONS];
    uint8_t *audio_internal_ring_buffer;
    uint8_t *mp3_decode_buffer;
    int32_t previous_mp3_frame_size;
    uint32_t total_sample_count;
    uint32_t share_buffer_waiting_cnt;
#if defined(MTK_AUDIO_MIXER_SUPPORT)
    uint32_t pcm_isr_cnt;
    int32_t is_bitstream_end_called;
#endif
#ifdef MTK_BT_AWS_ENABLE
    uint8_t *p_aws_buf;
#endif
} mp3_codec_private_t;


#ifdef __cplusplus
}
#endif

#endif  /*__MP3_CODEC_INTERNAL_H__*/

