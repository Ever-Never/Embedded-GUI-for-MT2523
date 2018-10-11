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


#ifndef __AUDIO_CODEC_H__
#define __AUDIO_CODEC_H__

/**
 * @addtogroup Audio Codec
 * @{
 * @addtogroup Audio Codec Common
 * @{
 * This section introduces the Audio codec APIs including terms and acronyms,
 * supported features, details on how to use the codec, function groups, enums, structures and functions.
 *
 * The Audio codec is used audio playback.
 *
 * @section audio_codec_api_usage How to use this module
 *
 * - An example on how to use the audio common codec APIs and a loudspeaker to play it out. \n
 *  - 1.  Call open API to open different codec and get common #audio_codec_media_handle_t. \n
 *        e.g. #audio_codec_wav_codec_open() to open WAV codec and get #audio_codec_media_handle_t.
 *  - 2.  handler->set_bitstream_buffer() to set bitstream buffer for vioce prompt.
 *  - 3.  Open audio file.
 *  - 4.  Use handler->get_write_buffer() to get the buffer address to write the audio data into.
 *  - 5.  Read the amount of audio data from the file and store in the buffer from the previous step.
 *  - 6.  Set output device and volume.
 *  - 7.  Use handler->play() to start playback.
 *  - 8.  Use handler->pause()/handler->resume() to pause/resume playing.
 *  - 9.  Use handler->stop() to stop playback.
 *  - 10. Use handler->close_codec() to close the codec.
 *    - Sample code:
 *     @code
 *
 *       static void test_sd_event_callback(audio_codec_media_handle_t *hdl, audio_codec_event_t event)
 *       {
 *            uint8_t *share_buf;
 *            uint32_t share_buf_len;
 *
 *            switch (event) {
 *                case AUDIO_CODEC_MEDIA_REQUEST:
 *                case AUDIO_CODEC_MEDIA_UNDERFLOW:
 *                    uint32_t loop_idx;
 *                    uint32_t loop_cnt = 2;
 *                    for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
 *                        hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
 *                        f_read(&fdst, share_buf, share_buf_len, &length_read);
 *                        hdl->write_data_done(hdl, share_buf_len);
 *                    }
 *
 *                    hdl->finish_write_data(hdl);
 *                    break;
 *            }
 *        }
 *
 *        void audio_codec_demo()
 *        {
 *             audio_codec_media_handle_t *hdl = NULL;
 *             hdl = audio_codec_wav_codec_open(test_sd_event_callback, NULL);
 *             if (hdl == NULL) {
 *                 log_hal_error("[Audio Codec ut] Failed to open the codec.");
 *                 return;
 *             }
 *
 *             if (open_file_from_sd(&fdst, (const TCHAR *)"SD:/temp/1.wav") < 0) {
 *                 log_hal_error("[Audio Codec ut] Failed to open SD:/temp/1.wav.");
 *             return;
 *             }
 *
 *             //prefill data to share buffer
 *             hdl->get_write_buffer(hdl, &share_buf, &share_buf_len);
 *             f_read(&fdst, share_buf, share_buf_len, &length_read);
 *
 *             hdl->write_data_done(hdl, share_buf_len);
 *             hdl->finish_write_data(hdl);
 *
 *
 *             hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HANDS_FREE_MONO);
 *             hal_audio_set_stream_out_volume(0x0, 0xFFFFFB50);
 *
 *             log_hal_info("[Audio Codec ut] play\n");
 *             hdl->play(hdl);
 *             vTaskDelay(20000 / portTICK_RATE_MS);
 *
 *             hdl->pause(hdl);
 *             vTaskDelay(5000 / portTICK_RATE_MS);
 *             hdl->resume(hdl);
 *
 *             hdl->stop(hdl);
 *             log_hal_info("[Audio Codec ut] close codec\n");
 *             hdl->close_codec(hdl);
 *
 *      @endcode
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#if defined(MTK_AUDIO_MIXER_SUPPORT)
    #include "audio_mixer.h"
    #include "audio_mixer_internal.h"
#else
    #define audio_mixer_role_t int
    #define audio_mixer_track_id_t int
#endif



#define AUDIO_CODEC_MAX_FUNCTIONS 3

/** @defgroup audio_codec_enum Enum
  * @{
  */

/** @brief This enumeration defines the audio codec events. */
typedef enum {
    AUDIO_CODEC_MEDIA_OK     = 0,     /**< The codec operation was successful. */
    AUDIO_CODEC_MEDIA_REQUEST,        /**< The audio codec requested a bitstream. */
    AUDIO_CODEC_MEDIA_UNDERFLOW,      /**< The bitstream has an underflow. */
    AUDIO_CODEC_MEDIA_EVENT_END,      /**< The bitstream had been totally streamed out. */
} audio_codec_event_t;

/** @brief audio codec state. */
typedef enum {
    AUDIO_CODEC_STATE_ERROR  = -1,    /**< An error occurred. */
    AUDIO_CODEC_STATE_IDLE,           /**< The audio codec is inactive. */
    AUDIO_CODEC_STATE_READY,          /**< The audio codec is ready to play the media. */
    AUDIO_CODEC_STATE_PLAY,           /**< The audio codec is in playing state. */
    AUDIO_CODEC_STATE_PAUSE,          /**< The audio codec is in pause state. */
    AUDIO_CODEC_STATE_STOP,           /**< The audio codec has stopped. */
} audio_codec_state_t;

typedef enum {
    AUDIO_CODEC_RETURN_ERROR = -1,    /**< The audio codec reported an error. */
    AUDIO_CODEC_RETURN_OK    = 0,     /**< The audio codec works normally. */
} audio_codec_status_t;


/**
  * @}
  */

/** @defgroup audio_codec_struct Struct
  * @{
  */

/** @brief Shared buffer structure. */
typedef struct {
    uint8_t         *buffer_base;          /**< Pointer to the ring buffer. */
    uint32_t         buffer_size;          /**< Size of the ring buffer. */
    uint32_t         write;                /**< Index of the ring buffer to write the data. */
    uint32_t         read;                 /**< Index of the ring buffer to read the data. */
    void            *param;                /**< Extended parameter or information. */
} audio_codec_audio_buffer_t;


/** @brief audio codec handle structure. */
typedef struct audio_media_handle_s {
    audio_codec_state_t          state;          /**< The audio codec state. */
    audio_codec_audio_buffer_t   buffer_info;    /**< The shared buffer information for the audio codec. */
    bool                         underflow;      /**< The audio codec data underflow occurred. */
    bool                         waiting;        /**< The audio codec is waiting to fill data. */
    uint16_t                     audio_id;       /**< The audio ID of the audio codec. */

    void (*handler)(struct audio_media_handle_s *handle, audio_codec_event_t event_id);                     /**< The audio codec handler. */
    void (*get_write_buffer)(struct audio_media_handle_s *handle, uint8_t **buffer, uint32_t *length);      /**< Get the available length to write into a shared buffer and a pointer to the shared buffer. */
    void (*write_data_done)(struct audio_media_handle_s *handle, uint32_t length);                          /**< Update the write pointer to the shared buffer. */
    void (*finish_write_data)(struct audio_media_handle_s *handle);                                         /**< Indicate last data transfer. */
    int32_t (*get_free_space)(struct audio_media_handle_s *handle);                                         /**< Get the free space length available in the shared buffer. */
    int32_t (*get_data_count)(struct audio_media_handle_s *handle);                                         /**< Get available data amount of the shared buffer. */

    /* for voice prompt. */
    // temp mark for build pass.
    // audio_codec_status_t (*set_track_role)(struct audio_media_handle_s *handle, audio_mixer_role_t track ); /**< set role for this decoder. */
    audio_codec_status_t (*set_bitstream_buffer)(struct audio_media_handle_s *handle, uint8_t *buffer, uint32_t length);  /**< set voice buffer. */

    /* for audio playback & voice prompt. */
    audio_codec_status_t (*play)(struct audio_media_handle_s *handle);                                      /**< The audio codec play function. */
    audio_codec_status_t (*pause)(struct audio_media_handle_s *handle);                                     /**< The audio codec pause function. */
    audio_codec_status_t (*resume)(struct audio_media_handle_s *handle);                                    /**< The audio codec resume function. */
    audio_codec_status_t (*stop)(struct audio_media_handle_s *handle);                                      /**< The audio codec stop function. */

    audio_mixer_track_id_t      mixer_track_id;
    audio_mixer_role_t          mixer_track_role;
    void (*set_track_role)(struct audio_media_handle_s *handle, audio_mixer_role_t role);

} audio_codec_media_handle_t;

/**
  * @}
  */

/** @defgroup audio_codec_typedef Typedef
  * @{
  */

/** @brief  This defines the audio codec callback function prototype. The user should register a callback function while opening the audio codec.
 *  @param[in] handle is the media handle of the audio codec.
 *  @param[in] event_id is the value defined in #audio_codec_event_t. This parameter is given by the driver to notify the user about the data flow behavior.
 */
typedef void (*audio_codec_callback_t)(audio_codec_media_handle_t *handle, audio_codec_event_t event_id);


/**
  * @}
  */

/**
 * @brief     This function opens the WAV codec.
 * @param[in] callback is a pointer to the callback function for the WAV data control.
 * @param[in] param is a pointer for extended parameter or information.
 * @return    Media handle of the WAV codec. Please refer to #audio_codec_media_handle_t.
 * @sa        #audio_codec_wav_codec_close()
 */
audio_codec_media_handle_t *audio_codec_wav_codec_open(audio_codec_callback_t callback, void *param);

/**
 * @brief     This function closes the WAV codec.
 * @param[in] handle is the WAV codec handle.
 * @return    Return state from WAV codec. Please refer to #audio_codec_status_t.
 * @sa        #audio_codec_wav_codec_open()
 */
audio_codec_status_t audio_codec_wav_codec_close(audio_codec_media_handle_t *handle);



/* internal usage. for codec drivers. */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief  callback function prototype for audio task queue.
 * @param[in] handle is the media handle of the audio codec.
 * @param[in] parameter is the extended parameter or information.
 */
typedef void (*audio_codec_internal_callback_t)(audio_codec_media_handle_t *handle, void *parameter);

/** @brief structure for queue event. */
typedef struct {
    audio_codec_media_handle_t *handle;
    void *parameter;
} audio_codec_queue_event_t;

/**
 * @brief  register callback for audio task queue.
 * @param[in] handle is the media handle of the audio codec. Please refer to #audio_codec_media_handle_t
 * @param[in] callback please refer to #audio_codec_internal_callback_t.
 * @return    AUDIO_CODEC_RETURN_OK if register sucess. Otherwise AUDIO_CODEC_RETURN_ERROR.
 */
audio_codec_status_t audio_codec_event_register_callback(audio_codec_media_handle_t *handle, audio_codec_internal_callback_t callback);

/**
 * @brief  deregister callback for audio task queue.
 * @param[in] handle is the media handle of the audio codec. Please refer to #audio_codec_media_handle_t
 * @return    AUDIO_CODEC_RETURN_OK if register sucess. Otherwise AUDIO_CODEC_RETURN_ERROR.
 */
audio_codec_status_t audio_codec_event_deregister_callback(audio_codec_media_handle_t *handle);

/**
 * @brief  send event to audio task queue.
 * @param[in] handle is the media handle of the audio codec. Please refer to #audio_codec_media_handle_t
 * @param[in] parameter is the extended parameter or information.
 * @return    AUDIO_CODEC_RETURN_OK if register sucess. Otherwise AUDIO_CODEC_RETURN_ERROR.
 */
audio_codec_status_t audio_codec_event_send_from_isr (audio_codec_media_handle_t *handle, void *parameter);

/**
 * @brief   create mutex.
 * @return  AUDIO_CODEC_RETURN_OK if create sucess. Otherwise AUDIO_CODEC_RETURN_ERROR.
 */
audio_codec_status_t audio_codec_mutex_create(void);

/**
 * @brief   delete mutex.
 * @return  AUDIO_CODEC_RETURN_OK if delete sucess. Otherwise AUDIO_CODEC_RETURN_ERROR.
 */
audio_codec_status_t audio_codec_mutex_delete(void);

/**
 * @brief   take semaphore to lock.
 * @return  AUDIO_CODEC_RETURN_OK if lock sucess. Otherwise AUDIO_CODEC_RETURN_ERROR.
 */
audio_codec_status_t audio_codec_mutex_lock(void);

/**
 * @brief   give semaphore to unlock.
 * @return  AUDIO_CODEC_RETURN_OK if unlock sucess. Otherwise AUDIO_CODEC_RETURN_ERROR.
 */
audio_codec_status_t audio_codec_mutex_unlock(void);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/


#endif /* __AUDIO_CODEC_H__ */


