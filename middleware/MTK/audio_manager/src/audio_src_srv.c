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

#include <string.h>
#include "stdint.h"
#include "audio_src_srv.h"
#include "audio_src_srv_internal.h"


/* multi source handle */
audio_src_srv_handle_t g_audio_src_srv_handle[AUDIO_SRC_SRV_PSEUDO_DEVICE_NUM];


static void audio_src_srv_reset_handle(audio_src_srv_handle_t *handle)
{
    if (handle) {
        memset(handle, 0x00, sizeof(audio_src_srv_handle_t));
    }
}


audio_src_srv_handle_t *audio_src_srv_construct_handle(audio_src_srv_pseudo_device_t type)
{
    int32_t i = 0;
    audio_src_srv_handle_t *hd = NULL;
    // mutex lock
    audio_src_srv_mutex_lock();
    for (i = 0; i < AUDIO_SRC_SRV_PSEUDO_DEVICE_NUM; ++i) {
        if (!(g_audio_src_srv_handle[i].flag & AUDIO_SRC_SRV_FLAG_USED)) {
            hd = &g_audio_src_srv_handle[i];
            // audio_src_srv_reset_handle(handle);
            hd->type = type;
            //hd->flag |= AUDIO_SRC_SRV_FLAG_USED;
            AUDIO_SRC_SRV_SET_FLAG(hd, AUDIO_SRC_SRV_FLAG_USED);
            break;
        }
    }

    if (!hd) {
        // exception: no source handle, check
        audio_src_srv_report("[AudSrc]construct_handle--error\n");
    }

    // mutex unlock
    audio_src_srv_mutex_unlock();

    return hd;
}


void audio_src_srv_destruct_handle(audio_src_srv_handle_t *handle)
{
    // mutex lock
    audio_src_srv_mutex_lock();
    audio_src_srv_reset_handle(handle);
    // mutex unlock
    audio_src_srv_mutex_unlock();
}


void audio_src_srv_add_waiting_list(audio_src_srv_handle_t *handle)
{
    // mutex lock
    audio_src_srv_mutex_lock();
    if (handle) {
        audio_src_srv_report("[AudSrc]add_waiting_list--hd: 0x%x, type: %d, state: %d\n", handle, handle->type, handle->state);
        AUDIO_SRC_SRV_SET_FLAG(handle, AUDIO_SRC_SRV_FLAG_WAITING);
    } else {
        audio_src_srv_report("[AudSrc]add_waiting_list(err)\n");
    }
    // mutex unlock
    audio_src_srv_mutex_unlock();
}


void audio_src_srv_del_waiting_list(audio_src_srv_handle_t *handle)
{
    // mutex lock
    audio_src_srv_mutex_lock();
    if (handle) {
        audio_src_srv_report("[AudSrc]del_waiting_list--hd: 0x%x, type: %d, state: %d\n", handle, handle->type, handle->state);
        AUDIO_SRC_SRV_RESET_FLAG(handle, AUDIO_SRC_SRV_FLAG_WAITING);
    } else {
        audio_src_srv_report("[AudSrc]del_waiting_list(err)\n");
    }
    // mutex unlock
    audio_src_srv_mutex_unlock();
}


void audio_src_srv_update_state(audio_src_srv_handle_t *handle, audio_src_srv_event_t evt_id)
{
    // mutex lock
    audio_src_srv_mutex_lock();
    if (handle) {
        audio_src_srv_process_psedev_event(handle, evt_id);
    } else {
        audio_src_srv_report("[AudSrc]update_state(err)\n");
    }
    // mutex unlock
    audio_src_srv_mutex_unlock();
}


const audio_src_srv_handle_t *audio_src_srv_get_pseudo_device(void)
{
    return g_audio_src_srv_handle;
}

