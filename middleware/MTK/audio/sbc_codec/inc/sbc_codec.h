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

#ifndef __SBC_CODEC_H__
#define __SBC_CODEC_H__

#include "bt_codec.h"

#define LOUDNESS    0x00
#define SNR         0x01

#define PREFFERED_ALLOCATION_MODE       LOUDNESS

/* SBC internal functions */
#if (PREFFERED_ALLOCATION_MODE==LOUDNESS)
#define SBC_ALLOCATION_SUPPORT      0x1 // alloc_method: loudness only
#elif (PREFFERED_ALLOCATION_MODE==SNR)
#define SBC_ALLOCATION_SUPPORT      0x2 // alloc_method: snr only
#else
#error wrong definition in sbc.h
#endif

#define SBC_FRAME_RING_SIZE         (10*1024)
#define PCM_DATA_RING_SIZE          (24*1024)
#define SBC_ENCODE_BUFFER_SIZE      (512) // max 8*16*2*2

/** @brief This enumeration defines the audio codec events. */
typedef enum {
    SBC_CODEC_MEDIA_OK     = 0,      /**< The codec operation was successful. */
    SBC_CODEC_MEDIA_GET_PAYLOAD,     /**< The sbc codec notify to get payload. */
} sbc_codec_event_t;

typedef enum {
    SBC_CODEC_MEDIA_ENCODER     = 0, /**< The codec operation was successful. */
    SBC_CODEC_MEDIA_DECODER,         /**< The sbc codec notify to get payload. */
} sbc_codec_role_t;

/** @brief audio codec state. */
typedef enum {
    SBC_CODEC_STATE_ERROR  = -1,    /**< An error occurred. */
    SBC_CODEC_STATE_IDLE,           /**< The audio codec is inactive. */
    SBC_CODEC_STATE_READY,          /**< The audio codec is ready to play the media. */
    SBC_CODEC_STATE_PLAY,           /**< The audio codec is in playing state. */
    SBC_CODEC_STATE_PAUSE,          /**< The audio codec is in pause state. */
    SBC_CODEC_STATE_WAIT_DRAIN,     /**< The audio codec is waiting data drain. */
    SBC_CODEC_STATE_STOP,           /**< The audio codec has stopped. */
} sbc_codec_state_t;

typedef struct {
    uint8_t min_bit_pool;       /**< The minimum bit pool. */
    uint8_t max_bit_pool;       /**< The maximum bit pool. */
    uint8_t block_length;       /**< b0: 16, b1: 12, b2: 8, b3: 4. */
    uint8_t subband_num;        /**< b0: 8, b1: 4. */
    uint8_t alloc_method;       /**< b0: loudness, b1: SNR. */
    uint8_t sample_rate;        /**< b0: 48000, b1: 44100, b2: 32000, b3: 16000. */
    uint8_t channel_mode;       /**< b0: joint stereo, b1: stereo, b2: dual channel, b3: mono. */
} sbc_codec_sbc_t;

/** @brief This structure defines the sbc codec. */
typedef struct {
    sbc_codec_role_t role;
    sbc_codec_sbc_t   cap;
} sbc_codec_initial_parameter_t;


/** @brief audio codec handle structure. */
typedef struct sbc_codec_media_handle_s {
    sbc_codec_role_t             role;
    sbc_codec_state_t            state;
    bool                         waiting;

    void     (*handler)(/*handle*/void *handle, sbc_codec_event_t event_id);                                                 /**< The audio codec handler. */
    void     (*set_buffer)(struct sbc_codec_media_handle_s *handle, uint8_t  *buffer, uint32_t  length);                     /**< Set the buffer for sbc codec.*/

    /* for encoder. */
    uint32_t (*get_payload)(struct sbc_codec_media_handle_s *handle, uint8_t* buffer, uint32_t size, uint32_t *sample_cnt);  /**< Get payload. */
    void     (*get_payload_done)(struct sbc_codec_media_handle_s *handle);                                                   /**< Get payload done. */
    void     (*query_payload_size)(struct sbc_codec_media_handle_s *handle, uint32_t *minimum, uint32_t *total);             /**< Query minimum and total sbc frame size. */
    uint32_t (*set_bit_rate)(struct sbc_codec_media_handle_s *handle, uint32_t bit_rate);                                    /**< Set bit rate. */

    /* Operation. */
    /**
     * Return Value
     *     >= 0    Normal
     *     < 0     Error
     */
    int32_t (*play)(struct sbc_codec_media_handle_s *handle);                                      /**< The audio codec play function. */
    int32_t (*pause)(struct sbc_codec_media_handle_s *handle);                                     /**< The audio codec pause function. */
    int32_t (*resume)(struct sbc_codec_media_handle_s *handle);                                    /**< The audio codec resume function. */
    int32_t (*stop)(struct sbc_codec_media_handle_s *handle);                                      /**< The audio codec stop function. */
} sbc_codec_media_handle_t;

/** @brief structure for queue event. */
typedef struct {
    sbc_codec_media_handle_t *handle;
    void *parameter;
} sbc_codec_queue_event_t;

typedef void (*sbc_codec_callback_t)(/*bt_media_handle_t*/void *handle, sbc_codec_event_t event_id);

/*
    Input Parameters
        callback            Notification callback
        param               SBC encoder initial parameters
        user_data           User data
    Return Value
        SBC codec handle
*/
sbc_codec_media_handle_t *sbc_codec_open (sbc_codec_callback_t callback, sbc_codec_initial_parameter_t *param, void *user_data);

/*
    Input Parameters
        handle              SBC encoder handle
    Return Value
        >= 0    Normal
        < 0     Error
*/
int32_t  sbc_codec_close(sbc_codec_media_handle_t *handle);

/*
    Return Value
        Total buffer size needed.
*/
uint32_t sbc_codec_get_buffer_size(void);

#endif  /* __SBC_CODEC_H__ */
