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

#ifndef __AUDIO_MIXER_H__
#define __AUDIO_MIXER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_audio.h"

/**
 * @addtogroup Audio
 * @{
 * @addtogroup mixer
 * @{
 *
 * The audio mixer is used to mix main track (for example, A2DP playback or Audio Player) and side track (for example, prompt sound).
 *
 * @section audio_mixer_api_usage How to use this module
 *
 * - An example on how to use the audio mixer APIs. \n
 *  - 1.  Audio Player plays a mp3 file. In MP3 codec, use #audio_mixer_get_track_id to get track ID and \n
 *        use #audio_mixer_start_track to play the audio track.
 *  - 2.  Voice Prompt application open a prompt sound. \n
 *        Voice Prompt application use the member function "set_track_role" of nedia handle to indicate the side track. \n
 *        media_handle->set_track_role().
 *        Then Voice Prompt calls  #audio_mixer_get_track_id to get track ID and #audio_mixer_start_track to plays the prompt sound.
 *  - 3.  Audio mixer will mix the aound of Audio Player and Voice Prompt.
 *  - 4.  Voice Prompt application may adjust volume weighting by using #audio_mixer_set_volume.
 *  - 5.  When playback is paused or stoped, codec will call #audio_mixer_stop_track to stop the playback, and \n
 *        call #audio_mixer_free_track_id to free the track ID.
 *    - Sample code:
 *     @code
 *
 *
 *     @endcode
 */

/** @defgroup audio_mixer_enum Enum
  * @{
  */

/** @brief Audio Mixer Track Role. */
typedef enum {
    AUDIO_MIXER_TRACK_ROLE_MAIN   = 0,   /**< Main track. */
    AUDIO_MIXER_TRACK_ROLE_SIDE   = 1,   /**< Side track. */
    AUDIO_MIXER_TRACK_ROLE_UNKNOW        /**< Error. */
} audio_mixer_role_t;

/** @brief Audio Mixer Track ID. */
typedef enum {
    AUDIO_MIXER_TRACK_ID_ERROR    = -1   /**< An error occurred. */
} audio_mixer_track_id_t;

/** @brief Audio Mixer Status. */
typedef enum {
    AUDIO_MIXER_INVALID_PARAMETER = -2,  /**< A wrong parameter is given. */
    AUDIO_MIXER_STATUS_ERROR      = -1,  /**< An error occurred during the function call. */
    AUDIO_MIXER_STATUS_OK         = 0    /**< No error occurred during the function call. */
} audio_mixer_status_t;

/**
  * @}
  */


/**
 * @brief     This function gets the available track ID.
 * @param[in] None.
 * @return    Return AUDIO_MIXER_STATUS_OK if the operation is successful. Otherwise, error occurs.
 * @sa        #audio_mixer_init()
 */
audio_mixer_status_t audio_mixer_init(void);

/**
 * @brief     This function gets the available track ID.
 * @param[in] sampling_rate_enum is an enumeration to indicate the output sampling rate.
 * @param[in] channel_number_enum is an enumeration to indicate the channel number.
 * @param[in] callback is a functio for callback abd notify.
 * @param[in] user_data is a pointer for calback functio.
 * @param[in] role is track role: main or side track.
 * @return    Return the available track ID. If the return value is AUDIO_MIXER_TRACK_ID_ERROR, there is no available track ID.
 * @sa        #audio_mixer_get_track_id()
 */
audio_mixer_track_id_t audio_mixer_get_track_id(
    hal_audio_sampling_rate_t       sampling_rate_enum,
    hal_audio_channel_number_t      channel_number_enum,
    hal_audio_stream_out_callback_t callback,
    void                            *user_data,
    audio_mixer_role_t              role
);

/**
 * @brief     This function releases track ID.
 * @param[in] track_id is the track ID which we want to release.
 * @return    Return AUDIO_MIXER_STATUS_OK if the operation is successful. Otherwise, error occurs.
 * @sa        #audio_mixer_free_track_id()
 */
audio_mixer_status_t audio_mixer_free_track_id(
    audio_mixer_track_id_t          track_id
);

/**
 * @brief     This function starts to play the track.
 * @param[in] track_id is an enumeration to indicate track ID obtained from audio_mixer_get_track_id.
 * @return    Return status of this API call. Please refer to #audio_mixer_status_t.
 * @sa        #audio_mixer_start_track()
 */
audio_mixer_status_t audio_mixer_start_track(
    audio_mixer_track_id_t          track_id
);

/**
 * @brief     This function stops the track.
 * @param[in] track_id is an enumeration to indicate track ID obtained from audio_mixer_get_track_id.
 * @return    Return status of this API call. Please refer to #audio_mixer_status_t.
 * @sa        #audio_mixer_stop_track()
 */
audio_mixer_status_t audio_mixer_stop_track(
    audio_mixer_track_id_t          track_id
);

/**
 * @brief     This function is used to query the free space of track.
 * @param[in] track_id is an enumeration to indicate track ID obtained from audio_mixer_get_track_id.
 * @param[out] free_space_count is the data count (in bytes) which host can fill in.
 * @return    Return status of this API call. Please refer to #audio_mixer_status_t.
 * @sa        #audio_mixer_query_free_sample_count()
 */
audio_mixer_status_t audio_mixer_query_free_count(
    audio_mixer_track_id_t          track_id,
    uint32_t                        *free_space_count
);

/**
 * @brief     This function is used to query the unprocessed data count of track.
 * @param[in] track_id is an enumeration to indicate track ID obtained from audio_mixer_get_track_id.
 * @param[out] data_count is the unprocessed data count (in bytes).
 * @return    Return status of this API call. Please refer to #audio_mixer_status_t.
 * @sa        #audio_mixer_query_data_sample_count()
 */
audio_mixer_status_t audio_mixer_query_data_count(
    audio_mixer_track_id_t          track_id,
    uint32_t                        *data_count
);

/**
 * @brief     This function is used to fill data into track.
 * @param[in] track_id is an enumeration to indicate track ID obtained from audio_mixer_get_track_id.
 * @param[in] buffer is the pointer of PCM data.
 * @param[in] size is the data count (in bytes) of PCM data.
 * @return    Return status of this API call. Please refer to #audio_mixer_status_t.
 * @sa        #audio_mixer_query_data_sample_count()
 */
audio_mixer_status_t audio_mixer_write_data(
    audio_mixer_track_id_t          track_id,
    const void                      *buffer,
    uint32_t                        size
);

/**
 * @brief     This function is used to set the weighting of overlap volume.
 * @param[in] main_track_gain is the digital volume of main track. Format is Q.15, which means 0x8000 is unit gain = 1.
 * @param[in] side_track_gain is the digital volume of side track. Format is Q.15, which means 0x8000 is unit gain = 1.
 * @return    Return status of this API call. Please refer to #audio_mixer_status_t.
 * @sa        #audio_mixer_query_data_sample_count()
 */
audio_mixer_status_t audio_mixer_set_volume(
    uint16_t                        main_track_gain,
    uint16_t                        side_track_gain
);

/**
 * @brief     This function set the end of data flag.
 * @param[in] track_id is an enumeration to indicate track ID obtained from audio_mixer_get_track_id.
 * @return    None.
 * @sa        #audio_mixer_set_eof()
 */
void audio_mixer_set_eof(
    audio_mixer_track_id_t          track_id
);

/**
 * @brief     This function is used to merge data into p_acc_buf.
 * @param[in] p_acc_buf is an accumulated buffer.
 * @param[in] len is the length of p_acc_buf in byte.
 * @return    None.
 * @sa        #audio_mixer_merge_buffer()
 */
void audio_mixer_p2w_isr(
    uint8_t *p_acc_buf,
    uint32_t len
);

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/

#endif  /*__AUDIO_MIXER_H__*/
