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

#ifndef __AUDIO_SRC_SRV_INTERNAL_H__
#define __AUDIO_SRC_SRV_INTERNAL_H__

#include <stdint.h>
#include <syslog.h>
#include "audio_src_srv.h"


typedef enum {
    AUDIO_SRC_SRV_MSG_START = 0,
    AUDIO_SRC_SRV_MSG_UNAVAILABLE,
    AUDIO_SRC_SRV_MSG_READY,
    AUDIO_SRC_SRV_MSG_PLAY,
    AUDIO_SRC_SRV_MSG_STOP,
    AUDIO_SRC_SRV_MSG_PLAYING,
} audio_src_srv_message_t;

typedef struct {
    audio_src_srv_state_t state;
    audio_src_srv_transient_state_t substate;
    uint32_t play_count;

    audio_src_srv_handle_t *running;
} audio_src_srv_context_t;



#define AUDIO_SRC_SRV_SET_FLAG(HANDLE, FLAG) ((HANDLE->flag) |= (FLAG))
#define AUDIO_SRC_SRV_RESET_FLAG(HANDLE, FLAG) ((HANDLE->flag) &= ~(FLAG))

#ifndef NULL
#define NULL ((void *)0)       /**<Default value of the pointer.*/
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define audio_src_srv_report(_message,...) LOG_I(AudSrc, (_message), ##__VA_ARGS__)

audio_src_srv_context_t *audio_src_srv_get_ctx(void);

void audio_src_srv_update_psedev_state(audio_src_srv_handle_t *handle, audio_src_srv_state_t state);

void audio_src_srv_state_machine_handle(audio_src_srv_handle_t *handle, audio_src_srv_message_t msg_id, void *param);

audio_src_srv_handle_t *audio_src_srv_check_waiting_list(void);

void audio_src_srv_update_psedev_play_count(audio_src_srv_handle_t *handle);

void audio_src_srv_process_psedev_event(audio_src_srv_handle_t *handle, audio_src_srv_event_t evt_id);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_SRC_SRV_INTERNAL_H__ */

