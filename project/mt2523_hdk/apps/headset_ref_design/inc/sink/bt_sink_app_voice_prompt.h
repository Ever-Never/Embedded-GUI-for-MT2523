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

#ifndef BT_SINK_APP_VOICE_PROMPT_H
#define BT_SINK_APP_VOICE_PROMPT_H

#include "bt_sink_srv.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************
 * Macro & Define
 *
 ********************************************************/

#define bt_sink_app_voice_prompt_report(_message,...) LOG_I(VOICE_PROMPT_APP, (_message), ##__VA_ARGS__)

/********************************************************
 * Enum & Structures
 *
 ********************************************************/
/** @brief This enum define tone of voice prompt */
typedef enum {
    VOICE_PROMPT_TONE_POWER_ON,                   /**<  0  */ 
    VOICE_PROMPT_TONE_POWER_OFF,                 /**<  1  */
    VOICE_PROMPT_TONE_RESET_PAIRED_LIST,  /**< 2 */
    VOICE_PROMPT_TONE_PAIRING,                            /**<  3  */
    VOICE_PROMPT_TONE_CONNECTED,                  /**<  4  */
    VOICE_PROMPT_TONE_DISCONNECTED,           /**< 5 */
    VOICE_PROMPT_TONE_VOLUME_CHANGE,     /**<  6 */
    VOICE_PROMPT_TONE_CALL_AUDIO_TRANSFER,     /**<  7 */
    VOICE_PROMPT_TONE_CALL_END,                       /**<  8*/
    VOICE_PROMPT_TONE_CALL_REFUSED,             /**<  9 */
    VOICE_PROMPT_TONE_CALL_REDIALING,           /**<  10 */
    VOICE_PROMPT_TONE_LOW_BATTERY,              /**<   11 */
    VOICE_PROMPT_TONE_MP3_MUSIC,                    /**< 12 */
    VOICE_PROMPT_TONE_BT_MUSIC,                      /**< 13 */
    VOICE_PROMPT_TONE_NUM                   /**< The total number of voice prompt tone. */
} bt_sink_app_voice_prompt_tone_t;

typedef struct {
    const uint8_t *buf;
    uint32_t size;
} bt_sink_app_voice_prompt_buf_t;


typedef struct {
    bt_sink_srv_action_t curr_action;
} bt_sink_app_voice_prompt_context_t;

/********************************************************
 * External Function
 *
 ********************************************************/
void bt_sink_app_voice_prompt_by_sink_action(bt_sink_srv_action_t action);
void bt_sink_app_voice_prompt_by_battery(int32_t charger_exist, int32_t capacity);
bt_sink_srv_status_t bt_sink_app_voice_prompt_by_sink_event(bt_sink_srv_event_t event_id, void *parameters);
void bt_sink_app_voice_prompt_play(uint8_t tone_idx);

#ifdef __cplusplus
    }
#endif

#endif /* BT_SINK_APP_VOICE_PROMPT_H */

