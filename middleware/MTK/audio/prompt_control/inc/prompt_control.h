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

#ifndef __PROMPT_CONTROL_H__
#define __PROMPT_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @addtogroup Audio
 * @{
 * @addtogroup prompt_control
 * @{
 *
 * The prompt_control is used to provide interface for prompt sound control (for example, wav and MP3 prompt sound).
 *
 * @section prompt_control_api_usage How to use this module
 *
 * - An example on how to use the prompt_control APIs.
 *  - 1.  Voice Prompt application select a prompt sound.
 *        Use #prompt_control_play_tone() to play the prompt sound.
 *  - 2.  Voice Prompt application may halt the playback by using #prompt_control_stop_tone().
 *  - 3.  Voice Prompt application may adjust volume weighting by using #voice_prompt_mix_set_mixer_gain.
 *    - Sample code:
 *     @code
 *
 *
 *     @endcode
 */

/** @defgroup prompt_control_enum Enum
  * @{
  */

/** @brief Prompt Control Tone Type. */
typedef enum {
    VPC_WAV,
    VPC_MP3,
} prompt_control_tone_type_t;

/** @brief Prompt Control Events. */
typedef enum {
    PROMPT_CONTROL_MEDIA_END    /**< end of playback */
} prompt_control_event_t;

/** @brief Define prompt_control callback function prototype. The user should register a callback function when user play prompt sound.
 *  @param[in] event_id is the value defined in #mp3_codec_event_t. This parameter is given by the driver to notify the user about the data flow behavior.
 */
typedef void (*prompt_control_callback_t)(prompt_control_event_t event_id);


/**
  * @}
  */


/**
 * @brief     This function is used to play prompt sound.
 * @param[in] tone_type is an enumeration to indicate the audio format.
 * @param[in] tone_buf is the bitstream buffer address of the prompt sound.
 * @param[in] tone_size is the size of bitstream buffer.
 * @param[in] callback is the callback to notify application that the playback of prompt sound is complete.
 * @return    Return true if the operation is successful. Otherwise, error occurs.
 * @sa        #prompt_control_play_tone()
 */
bool prompt_control_play_tone(prompt_control_tone_type_t tone_type, uint8_t *tone_buf, uint32_t tone_size, prompt_control_callback_t callback);

/**
 * @brief     This function is to stop the prompt sound.
 * @param[in] None.
 * @return    None.
 * @sa        #prompt_control_stop_tone()
 */
void prompt_control_stop_tone(void);

/**
 * @brief     This function is used to adjust mixer gain.
 * @param[in] main_gain is the digital gain of main track. Format is Q.15, which means 0x8000 is unit gain = 1.
 * @param[in] side_gain is the digital gain of side track. Format is Q.15, which means 0x8000 is unit gain = 1.
 * @return    None.
 * @sa        #prompt_control_set_mixer_gain()
 */
void prompt_control_set_mixer_gain(uint16_t main_gain, uint16_t side_gain);

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/

#endif  /*__PROMPT_CONTROL_H__*/
