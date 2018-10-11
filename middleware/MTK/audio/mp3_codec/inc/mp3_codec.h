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


#ifndef __MP3_CODEC_H__
#define __MP3_CODEC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#if defined(MTK_AUDIO_MIXER_SUPPORT)
    #include "audio_mixer.h"
    #include "audio_mixer_internal.h"
#endif

/**
 * @addtogroup Audio
 * @{
 * @addtogroup mp3
 * @{
 *
 * The MP3 codec is used for MP3 playback.
 *
 * @section mp3_codec_api_usage How to use this module
 *
 * - An example on how to use the MP3 codec APIs and a loudspeaker to play it out. \n
 *  - 1.  Call #mp3_codec_open() to open MP3 codec and get MP3 codec handler.
 *  - 2.  Open MP3 file.
 *  - 3.  Use handler->get_write_buffer() to get the amount of MP3 format data to store and \n
 *        get the buffer address to write the MP3 data into.
 *  - 4.  Read the amount of MP3 data from the MP3 file and store in the buffer from the previous step.
 *  - 5.  Skip the ID3v2 header of the MP3 file.
 *  - 6.  Set output device and volume.
 *  - 7.  Use handler->play() to play MP3 file.
 *  - 8.  Use handler->pause()/handler->resume() to pause/resume playing.
 *  - 9.  Use handler->stop() to stop playing MP3 file.
 *  - 10. Use handler->close_codec() to close the MP3 codec.
 *    - Sample code:
 *     @code
 *
 *       static void test_sd_event_callback(mp3_codec_media_handle_t *hdl, mp3_codec_event_t event)
 *       {
 *            uint8_t *share_buf;
 *            uint32_t share_buf_len;
 *
 *            switch (event) {
 *                case MP3_CODEC_MEDIA_JUMP_FILE_TO:
 *                    res = f_lseek(&fdst, (DWORD)hdl->jump_file_to_specified_position);*
 *                    break;
 *                case MP3_CODEC_MEDIA_REQUEST:
 *                case MP3_CODEC_MEDIA_UNDERFLOW:
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
 *        void mp3_codec_demo()
 *        {
 *             mp3_codec_media_handle_t *hdl = NULL;
 *             hdl = mp3_codec_open(test_sd_event_callback);
 *             if (hdl == NULL) {
 *                 log_hal_error("[MP3 Codec ut] Failed to open the codec.");
 *                 return;
 *             }
 *
 *             if (open_file_from_sd(&fdst, (const TCHAR *)"SD:/temp/1.mp3") < 0) {
 *                 log_hal_error("[MP3 Codec ut] Failed to open SD:/temp/1.mp3.");
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
 *             hdl->skip_id3v2_and_reach_next_frame(hdl, (uint32_t)f_size(&fdst));
 *
 *             hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HANDS_FREE_MONO);
 *             hal_audio_set_stream_out_volume(0x0, 0xFFFFFB50);
 *
 *             log_hal_info("[MP3 Codec ut] play\n");
 *             hdl->play(hdl);
 *             vTaskDelay(20000 / portTICK_RATE_MS);
 *
 *             hdl->pause(hdl);
 *             vTaskDelay(5000 / portTICK_RATE_MS);
 *             hdl->resume(hdl);
 *
 *             hdl->stop(hdl);
 *             log_hal_info("[MP3 Codec ut] close mp3 codec\n");
 *             hdl->close_codec(hdl);
 *
 *      @endcode
 */



/** @defgroup mp3_codec_enum Enum
  * @{
  */

/** @brief This enumeration defines the MP3 codec events. */
typedef enum {
    MP3_CODEC_MEDIA_OK = 0,       /**< The codec operation was successful.   */
    MP3_CODEC_MEDIA_REQUEST,      /**< The MP3 codec requested a bitstream. */
    MP3_CODEC_MEDIA_UNDERFLOW,    /**< The bitstream has an underflow. */
    MP3_CODEC_MEDIA_JUMP_FILE_TO, /**< The MP3 codec requested to jump to specific position on the file. The position is jump_file_to_specified_position which is a member of #mp3_media_handle_s. */
    MP3_CODEC_AWS_CHECK_CLOCK_SKEW,
    MP3_CODEC_AWS_CHECK_UNDERFLOW,
	MP3_CODEC_MEDIA_BITSTREAM_END,/**< Tha MP3 codec play constant array end. */
}
mp3_codec_event_t;

/** @brief MP3 codec state. */
typedef enum {
    MP3_CODEC_STATE_ERROR = -1,     /**< An error occurred. */
    MP3_CODEC_STATE_IDLE,           /**< The MP3 codec is inactive. */
    MP3_CODEC_STATE_READY,          /**< The MP3 codec is ready to play the media. */
    MP3_CODEC_STATE_PLAY,           /**< The MP3 codec is in playing state. */
    MP3_CODEC_STATE_PAUSE,          /**< The MP3 codec is in pause state. */
    MP3_CODEC_STATE_STOP,           /**< The MP3 codec has stopped. */
} mp3_codec_state_t;


typedef enum {
    MP3_CODEC_RETURN_ERROR = -1,            /**< The MP3 codec reported an error. */
    MP3_CODEC_RETURN_OK = 0,                /**< The MP3 codec works normally. */
} mp3_codec_function_return_state_t;

typedef enum {
    MP3_CODEC_DATA_TYPE_AUDIO = 1,                  /**< Data type for audio. */
    MP3_CODEC_DATA_TYPE_SAMPLES_PER_CHANNEL = 2,    /**< Data type for samples per channel. */
} mp3_codec_data_type_t;

/**
  * @}
  */

/** @defgroup mp3_codec_struct Struct
  * @{
  */

/** @brief Shared buffer structure. */
typedef struct {
    uint8_t         *buffer_base;          /**< Pointer to the ring buffer. */
    uint32_t         buffer_size;          /**< Size of the ring buffer. */
    uint32_t         write;                /**< Index of the ring buffer to write the data. */
    uint32_t         read;                 /**< Index of the ring buffer to read the data. */
    void            *param;                /**< Extended parameter or information. */
} mp3_codec_audio_buffer_t;


/** @brief MP3 id3v2 structure. */
typedef struct {
    bool        has_id3v2;                  /**< The MP3 file has ID3v2 header. */
    uint32_t    id3v2_tage_length;          /**< The ID3v2 header length. */
} mp3_codec_mp3_id3v2_information_t;

/** @brief Silence Frame Information structure. */
typedef struct {
    int32_t         frame_size;            /**< Size of one mp3 frame. */
    uint8_t         *frame;                /**< One mp3 frame.         */
    int32_t         sampling_rate;         /**< Sampling Rate.         */
    void            *param;                /**< Extended parameter or information. */
} silence_frame_information_t;

/** @brief MP3 codec handle structure type. */
typedef struct mp3_media_handle_s mp3_codec_media_handle_t; /**< The MP3 codec handler. */

/** @brief MP3 codec handle structure. */
struct mp3_media_handle_s {
    mp3_codec_state_t            state;          /**< The MP3 codec state. */
    mp3_codec_audio_buffer_t     share_buff;     /**< The shared buffer information for the MP3 codec. */
    bool                         underflow;      /**< The MP3 codec data underflow occurred. */
    bool                         waiting;        /**< The MP3 codec is waiting to fill data. */
	bool                         linear_buff;    /**< The MP3 codec use linear buffer or not. */
    uint16_t                     audio_id;       /**< The audio ID of the MP3 codec. */
    uint32_t                     jump_file_to_specified_position;    /** < MP3 codec uses the event MP3_CODEC_MEDIA_JUMP_FILE_TO to jump to a specified location (in bytes) in the file. */
    mp3_codec_mp3_id3v2_information_t  id3v2_information;
    int32_t                     flush_data_flag; /**< The flag to flush data. */   
    void                         *private_data;       /**< Internal use only */
    void (*handler)(mp3_codec_media_handle_t *handle, mp3_codec_event_t event_id);                                  /**< The MP3 codec handler. */
    void (*set_share_buffer)(mp3_codec_media_handle_t *handle, uint8_t  *buffer, uint32_t  length);                 /**< Set the shared buffer for bitstream. */
    void (*get_write_buffer)(mp3_codec_media_handle_t *handle, uint8_t **buffer, uint32_t *length);                 /**< Get the available length to write into a shared buffer and a pointer to the shared buffer. */
    void (*get_read_buffer)(mp3_codec_media_handle_t *handle, uint8_t **buffer, uint32_t *length);                  /**< Get the available length to read from a shared buffer and a pointer to the shared buffer. */
    void (*write_data_done)(mp3_codec_media_handle_t *handle, uint32_t length);                                     /**< Update the write pointer to the shared buffer. */
    void (*finish_write_data)(mp3_codec_media_handle_t *handle);                                                    /**< Indicate last data transfer. */
    void (*flush)(mp3_codec_media_handle_t *handle, int32_t flush_data_flag);                                       /**< Flush the shared buffer to decode. */
    void (*reset_share_buffer)(mp3_codec_media_handle_t *handle);                                                   /**< Reset the shared buffer's information. */
    void (*read_data_done)(mp3_codec_media_handle_t *handle, uint32_t length);                                      /**< Update the read pointer to the shared buffer. */
    int32_t (*get_free_space)(mp3_codec_media_handle_t *handle);                                                    /**< Get the free space length available in the shared buffer. */
    int32_t (*get_data_count)(mp3_codec_media_handle_t *handle);                                                    /**< Get available data amount of the shared buffer. */
    void (*get_data_status)(mp3_codec_media_handle_t *handle, mp3_codec_data_type_t data_type, int32_t *data_status);          /**< Get data status. */
    int32_t (*get_silence_frame_information)(mp3_codec_media_handle_t *handle, int32_t *byte_count);                /**< Get silence frame information. */
    int32_t (*set_silence_frame_information)(mp3_codec_media_handle_t *handle, silence_frame_information_t *frm_info);              /**< Get silence frame information. */
    int32_t (*fill_silence_frame)(mp3_codec_media_handle_t *handle, uint8_t *buffer);                                               /**< Fill silence frame. */
    mp3_codec_function_return_state_t (*play)(mp3_codec_media_handle_t *handle);                                                    /**< The MP3 codec play function. */
    mp3_codec_function_return_state_t (*pause)(mp3_codec_media_handle_t *handle);                                                   /**< The MP3 codec pause function. */
    mp3_codec_function_return_state_t (*resume)(mp3_codec_media_handle_t *handle);                                                  /**< The MP3 codec resume function. */
    mp3_codec_function_return_state_t (*process)(mp3_codec_media_handle_t *handle, mp3_codec_event_t event_id);                     /**< The MP3 codec process function. */
    mp3_codec_function_return_state_t (*stop)(mp3_codec_media_handle_t *handle);                                                    /**< The MP3 codec stop function. */
    mp3_codec_function_return_state_t (*close_codec)(mp3_codec_media_handle_t *handle);                                             /**< The MP3 codec close_codec function. */
    mp3_codec_function_return_state_t (*skip_id3v2_and_reach_next_frame)(mp3_codec_media_handle_t *handle, uint32_t file_size);     /**< Skip id3v2 header and reach next frame. */
	mp3_codec_function_return_state_t (*set_bitstream_buffer)(mp3_codec_media_handle_t *handle, uint8_t *bitstrean_buffer, uint32_t length);/**< Set the bitstream buffer(constant array). */
#ifdef MTK_BT_AWS_ENABLE
    mp3_codec_function_return_state_t (*aws_init)(mp3_codec_media_handle_t *handle);                                                    /**< Initialize AWS. */
    mp3_codec_function_return_state_t (*aws_deinit)(mp3_codec_media_handle_t *handle);                                                  /**< Deinitialize AWS. */
    mp3_codec_function_return_state_t (*aws_set_clock_skew_compensation_value)(mp3_codec_media_handle_t *handle, int32_t sample_count); /**< Set clock skew compensation value. */
    mp3_codec_function_return_state_t (*aws_get_clock_skew_status)(mp3_codec_media_handle_t *handle, int32_t *aws_clock_skew_status);   /**< Get clock skew status. */
    mp3_codec_function_return_state_t (*aws_set_clock_skew)(mp3_codec_media_handle_t *handle, bool flag);                               /**< Sets the clock skew flag. flag is used to turn on or turn off clock skew. 1: turn on clock skew, 0: turn off clock skew. */
#endif
#if defined(MTK_AUDIO_MIXER_SUPPORT)
    audio_mixer_track_id_t      mixer_track_id;
    audio_mixer_role_t          mixer_track_role;
    void (*set_track_role)(mp3_codec_media_handle_t *handle, audio_mixer_role_t role);
#endif
};

/**
  * @}
  */

/** @defgroup mp3_codec_typedef Typedef
  * @{
  */

/** @brief  This defines the MP3 codec callback function prototype. The user should register a callback function while opening the MP3 codec. Please refer to #mp3_codec_open().
 *  @param[in] handle is the media handle of the MP3 codec.
 *  @param[in] event_id is the value defined in #mp3_codec_event_t. This parameter is given by the driver to notify the user about the data flow behavior.
 */
typedef void (*mp3_codec_callback_t)(mp3_codec_media_handle_t *handle, mp3_codec_event_t event_id);


/**
  * @}
  */

/**
 * @brief     This function gets the memory requirement of MP3 decoder.
 * @param[in] memory_size is a pointer to get the memory requirment of MP3 decoder.
 * @return    Return status of this API call. Please refer to #mp3_codec_function_return_state_t.
 * @sa        #mp3_codec_set_memory()
 */
mp3_codec_function_return_state_t mp3_codec_get_memory_size(uint32_t *memory_size);

/**
 * @brief     This function sets the memory for MP3 decoder.
 * @param[in] memory is a pointer to set the memory for MP3 decoder. The memory size should be the same size as #mp3_codec_get_memory_size().
 * @return    Return status of this API call. Please refer to #mp3_codec_function_return_state_t.
 * @sa        #mp3_codec_get_memory_size()
 */
mp3_codec_function_return_state_t  mp3_codec_set_memory(uint8_t *memory);


/**
 * @brief     This function sets the memory for MP3 decoder.
 * @return    Return status of this API call. Please refer to #mp3_codec_function_return_state_t.
 */
#if defined(MTK_AUDIO_MIXER_SUPPORT)
mp3_codec_function_return_state_t mp3_codec_set_memory2(mp3_codec_media_handle_t *internal_handle);
#else
mp3_codec_function_return_state_t mp3_codec_set_memory2(void);
#endif

#if 0
/**
 * @brief     This function will lock DVFS.
 * @return    Return status of this API call. Please refer to #mp3_codec_function_return_state_t.
 */
mp3_codec_function_return_state_t mp3_codec_enter_resume(void);

/**
 * @brief     This function will unlock DVFS.
 * @return    Return status of this API call. Please refer to #mp3_codec_function_return_state_t.
 */
mp3_codec_function_return_state_t mp3_codec_enter_suspend(void);
#endif

/**
 * @brief     This function opens the MP3 codec.
 * @param[in] callback is a pointer to the callback function for the MP3 data control.
 * @return    Media handle of the MP3 codec. Please refer to #mp3_media_handle_t.
 * @sa        #mp3_codec_close()
 */
mp3_codec_media_handle_t *mp3_codec_open(mp3_codec_callback_t callback);

/**
 * @brief     This function closes the MP3 codec.
 * @param[in] handle is the MP3 codec handle.
 * @return    Return state from MP3 codec. Please refer to #mp3_codec_function_return_state_t.
 * @sa        #mp3_codec_open()
 */
mp3_codec_function_return_state_t mp3_codec_close(mp3_codec_media_handle_t *handle);

/**
 * @brief     This function sets the advanced wireless stereo flag.
 * @param[in] handle is the mp3 codec handle.
 * @param[in] flag is used to determine to turn on or turn off AWS mechanism. 1: turn on AWS, 0: turn off AWS.
 * @return    If the operation completed successfully, the return value is #MP3_CODEC_RETURN_OK, otherwise the return value is #MP3_CODEC_RETURN_ERROR.
 */
mp3_codec_function_return_state_t mp3_codec_aws_set_flag(mp3_codec_media_handle_t *handle, bool flag);

/**
 * @brief     This function sets the advanced wireless stereo initial synchronization.
 * @param[in] handle is the mp3 codec handle.
 * @return    If the operation completed successfully, the return value is #MP3_CODEC_RETURN_OK, otherwise the return value is #MP3_CODEC_RETURN_ERROR.
 */
mp3_codec_function_return_state_t mp3_codec_aws_set_initial_sync(mp3_codec_media_handle_t *handle);

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/


#endif /* __MP3_CODEC_H__ */


