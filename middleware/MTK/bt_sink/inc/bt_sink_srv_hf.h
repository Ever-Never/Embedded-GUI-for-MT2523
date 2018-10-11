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

#ifndef __BT_SINK_SRV_HFP_H__
#define __BT_SINK_SRV_HFP_H__

#include <stdint.h>
#include "bt_hfp.h"
#include "bt_type.h"
#include "bt_sink_srv_resource.h"
#include "bt_sink_srv_action.h"
#include "bt_sink_srv_conmgr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_SINK_SRV_HF_AUDIO_DISC_DURATION  20
#define BT_SINK_SRV_HF_AUDIO_CONN_DURATION  3000
#ifdef __BT_SINK_SRV_AM_SUPPORT__
#define BT_SINK_SRV_HF_MAX_VOLUME  (AUD_VOL_OUT_MAX -1)
#define BT_SINK_SRV_HF_MIN_VOLUME  AUD_VOL_OUT_LEVEL0
#define BT_SINK_SRV_HF_DEFAULT_VOLUME AUD_VOL_OUT_LEVEL10
#else
#define BT_SINK_SRV_HF_MAX_VOLUME  6
#define BT_SINK_SRV_HF_MIN_VOLUME  0
#define BT_SINK_SRV_HF_DEFAULT_VOLUME 4
#endif
#define BT_SINK_SRV_HF_INVALID_AUDIO_ID  0xFF
#define BT_SINK_SRV_HF_CMD_LENGTH 12
#define BT_SINK_SRV_HF_LONG_CMD_LENGTH 48
#define BT_SINK_SRV_HF_DIAL_CMD_LENGTH 48

typedef enum {
    BT_SINK_SRV_HF_CALL_STATE_IDLE              = 0,
    BT_SINK_SRV_HF_CALL_STATE_INCOMING          = BT_SINK_SRV_STATE_INCOMING,
    BT_SINK_SRV_HF_CALL_STATE_OUTGOING          = BT_SINK_SRV_STATE_OUTGOING,
    BT_SINK_SRV_HF_CALL_STATE_ACTIVE            = BT_SINK_SRV_STATE_ACTIVE,
    BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING      = BT_SINK_SRV_STATE_TWC_INCOMING,
    BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING      = BT_SINK_SRV_STATE_TWC_OUTGOING,
    BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE       = BT_SINK_SRV_STATE_HELD_ACTIVE,
    BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING    = BT_SINK_SRV_STATE_HELD_REMAINING,
    BT_SINK_SRV_HF_CALL_STATE_MULTIPARTY        = BT_SINK_SRV_STATE_MULTIPARTY
} bt_sink_srv_hf_call_state_t;

#define BT_SINK_SRV_HF_CALL_STATE_ALL (bt_sink_srv_hf_call_state_t) \
                                    (BT_SINK_SRV_HF_CALL_STATE_INCOMING| \
                                    BT_SINK_SRV_HF_CALL_STATE_OUTGOING| \
                                    BT_SINK_SRV_HF_CALL_STATE_ACTIVE| \
                                    BT_SINK_SRV_HF_CALL_STATE_TWC_INCOMING| \
                                    BT_SINK_SRV_HF_CALL_STATE_TWC_OUTGOING| \
                                    BT_SINK_SRV_HF_CALL_STATE_HELD_ACTIVE| \
                                    BT_SINK_SRV_HF_CALL_STATE_HELD_REMAINING| \
                                    BT_SINK_SRV_HF_CALL_STATE_MULTIPARTY)

//typedef enum {
#define BT_SINK_SRV_HF_CHLD_RELEASE_HELD_REJECT_WAITING  "AT+CHLD=0"
#define BT_SINK_SRV_HF_CHLD_RELEASE_ACTIVE_ACCEPT_OTHER  "AT+CHLD=1"
#define BT_SINK_SRV_HF_CHLD_HOLD_ACTIVE_ACCEPT_OTHER     "AT+CHLD=2"
#define BT_SINK_SRV_HF_CHLD_ADD_HELD_TO_CONVERSATION     "AT+CHLD=3"
#define BT_SINK_SRV_HF_CHLD_EXPLICIT_CALL_TRANSFER       "AT+CHLD=4"
//} bt_sink_srv_hf_hold_action_t;

typedef enum {
    BT_SINK_SRV_HF_VOL_ACT_UP,
    BT_SINK_SRV_HF_VOL_ACT_DOWN
} bt_sink_srv_hf_volume_act_t;

typedef enum {
    BT_SINK_SRV_HF_AUDIO_CALL  = 0,
    BT_SINK_SRV_HF_AUDIO_SCO,
    BT_SINK_SRV_HF_AUDIO_RING,
    BT_SINK_SRV_HF_AUDIO_TOTAL
} bt_sink_srv_hf_audio_type_t;

typedef enum {
    BT_SINK_SRV_HF_FLAG_NONE        = 0x0000,
    BT_SINK_SRV_HF_FLAG_SCO_CREATED = 0x0001,
    BT_SINK_SRV_HF_FLAG_SCO_ACTIVE  = 0x0002,
    BT_SINK_SRV_HF_FLAG_QUERY_NAME  = 0x0004,
    BT_SINK_SRV_HF_FLAG_USER_REJECT = 0x0008,
    BT_SINK_SRV_HF_FLAG_QUERY_LIST  = 0x0010
} bt_sink_srv_hf_flag_t;

typedef enum {
    BT_SINK_SRV_HF_APL_NONE        = 0x0001, /* reserved */
    BT_SINK_SRV_HF_APL_BAT_REPORT  = 0x0002, /* battery reporting */
    BT_SINK_SRV_HF_APL_DOCKED      = 0x0004, /* is docked or powered */
    BT_SINK_SRV_HF_APL_SIRI_REPORT = 0x0008, /* Siri status reporting */
    BT_SINK_SRV_HF_APL_NR_REPORT   = 0x0010  /* noise reduction(NR) status reporting */
} bt_sink_srv_hf_apl_features_t;

typedef struct {
    uint32_t handle;
    uint16_t audio_type;
    uint8_t audio_id[BT_SINK_SRV_HF_AUDIO_TOTAL];
    bt_hfp_ag_feature_t ag_featues;
    bt_hfp_ag_hold_feature_t ag_chld_feature;
    bt_sink_srv_hf_call_state_t call_state;
    bt_hfp_audio_codec_type_t codec;
    bt_sink_srv_hf_flag_t flag;
    bt_sink_srv_caller_information_t caller;
} bt_sink_srv_hf_context_t;

typedef struct {
    bt_hfp_audio_codec_type_t codec;
    bool connected;
} bt_sink_srv_hf_sco_state_change_t;

typedef struct {
    bt_bd_addr_t          bt_addr;
    uint8_t               volume;
} bt_sink_srv_speaker_volume_change_t;

#ifdef MTK_BT_COMMAND_ENABLE
void bt_sink_srv_hf_sco_state_change_notify(bt_hfp_audio_codec_type_t codec_type, bool is_connected);
#endif

bt_status_t bt_sink_srv_hf_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);
bt_sink_srv_status_t bt_sink_srv_hf_action_handler(bt_sink_srv_action_t action, void *parameters);
void bt_sink_srv_hf_handle_setup_ind(bt_sink_srv_hf_context_t *link, bt_hfp_ciev_call_setup_state_t setup);
void bt_sink_srv_hf_handle_call_ind(bt_sink_srv_hf_context_t *link, bt_hfp_ciev_call_state_t call);
void bt_sink_srv_hf_handle_held_ind(bt_sink_srv_hf_context_t *link, bt_hfp_ciev_call_hold_state_t held);
void bt_sink_srv_hf_handle_call_info_ind(bt_sink_srv_hf_context_t *link, bt_hfp_call_list_ind_t *clcc);
void bt_sink_srv_hf_answer_call(bool accept);
void bt_sink_srv_hf_terminate_call(void);
void bt_sink_srv_hf_dial_number(char *number);
void bt_sink_srv_hf_dial_last(void);
void bt_sink_srv_hf_dial_missed(void);
void bt_sink_srv_hf_switch_audio_path(void);
void bt_sink_srv_hf_release_all_held_call(void);
void bt_sink_srv_hf_release_all_active_accept_others(void);
void bt_sink_srv_hf_hold_all_active_accept_others(void);
void bt_sink_srv_hf_release_special(uint8_t index);
void bt_sink_srv_hf_hold_special(uint8_t index);
void bt_sink_srv_hf_add_held_to_conversation(void);
void bt_sink_srv_hf_explicit_call_transfer(void);
void bt_sink_srv_hf_voice_recognition_activate(bool active);
void bt_sink_srv_hf_switch_audio_device(void);
void bt_sink_srv_hf_query_call_list(bt_bd_addr_t *address);
void bt_sink_srv_hf_send_dtmf(bt_sink_srv_send_dtmf_t *request);
void bt_sink_srv_hf_enable_apl_custom_commands(uint32_t handle, bt_sink_srv_hf_apl_features_t features);
bt_sink_srv_hf_context_t *bt_sink_srv_hf_get_highlight_device(void);
bt_sink_srv_hf_context_t *bt_sink_srv_hf_find_device_by_call_state(bt_sink_srv_hf_call_state_t call_state);
bool bt_sink_srv_hf_audio_connect(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p, bt_sink_srv_hf_audio_type_t audio_type);
void bt_sink_srv_hf_audio_disconnect(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p, bt_sink_srv_hf_audio_type_t audio_type);
void bt_sink_srv_hf_call_state_change(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p,
                                      bt_sink_srv_hf_call_state_t previous_state,
                                      bt_sink_srv_hf_call_state_t new_state);
void bt_sink_srv_hf_mp_answer(bt_sink_srv_hf_context_t *current_device, bool accept);
void bt_sink_srv_hf_mp_swap(bt_sink_srv_hf_context_t *active_device, bt_sink_srv_hf_context_t *held_device);
void bt_sink_srv_hf_mp_state_change(bt_sink_srv_hf_context_t *device);
void bt_sink_srv_hf_mp_set_sco(bt_sink_srv_hf_context_t *highlight, bt_sink_srv_hf_context_t *device);
void bt_sink_srv_hf_mp_switch_audio(void);

#define BT_SINK_SRV_HF_QUERY_CALL_LIST(handle) bt_hfp_send_command(handle, (uint8_t *)"AT+CLCC", (uint16_t)bt_sink_srv_strlen("AT+CLCC"))
#define BT_SINK_SRV_HF_DIAL_LAST_NUMBER(handle) bt_hfp_send_command(handle, (uint8_t *)"AT+BLDN", (uint16_t)bt_sink_srv_strlen("AT+BLDN"))
#define BT_SINK_SRV_HF_ANSWER_CALL(handle) bt_hfp_send_command(handle, (uint8_t *)"ATA", (uint16_t)bt_sink_srv_strlen("ATA"))
#define BT_SINK_SRV_HF_REJECT_CALL(handle) bt_hfp_send_command(handle, (uint8_t *)"AT+CHUP", (uint16_t)bt_sink_srv_strlen("AT+CHUP"))
#define BT_SINK_SRV_HF_TERMINATE_CALL(handle) bt_hfp_send_command(handle, (uint8_t *)"AT+CHUP", (uint16_t)bt_sink_srv_strlen("AT+CHUP"))
#define BT_SINK_SRV_HF_HOLD_CALL(handle, command) bt_hfp_send_command(handle, (uint8_t *)command, (uint16_t)bt_sink_srv_strlen(command))
#define BT_SINK_SRV_HF_SYNC_SPEAKER_GAIN(handle, command) bt_hfp_send_command(handle, (uint8_t *)command, (uint16_t)bt_sink_srv_strlen(command))
#define BT_SINK_SRV_HF_VOICE_RECOGNITION(handle, command) bt_hfp_send_command(handle, (uint8_t *)command, (uint16_t)bt_sink_srv_strlen(command))
#define BT_SINK_SRV_HF_DIAL_NUMBER(handle, command) bt_hfp_send_command(handle, (uint8_t *)command, (uint16_t)bt_sink_srv_strlen(command))
#define BT_SINK_SRV_HF_SEND_DTMF(handle, command) bt_hfp_send_command(handle, (uint8_t *)command, (uint16_t)bt_sink_srv_strlen(command))
#define BT_SINK_SRV_HF_ENABLE_CUSTOM_CMDS(handle, command) bt_hfp_send_command(handle, (uint8_t *)command, (uint16_t)bt_sink_srv_strlen(command))
#ifdef __cplusplus
}
#endif

#endif /* __BT_SINK_SRV_HFP_H__ */
