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

#ifndef BT_SINK_SRV_AWS_MP3_H
#define BT_SINK_SRV_AWS_MP3_H
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_ami.h"

#ifdef __BT_AWS_SUPPORT__
#define BT_SINK_SRV_MP3_AWS_TIMER_DUR            (100 * 1000)  /* ums*/
#define BT_SINK_SRV_MP3_AWS_INVALID_DUR          0xFFFFFFFF
#define BT_SINK_SRV_MP3_AWS_COMMAND_WAIT_DUR     (3000)

// notification id
#define BT_SINK_SRV_MP3_AWS_STREAMING_DATA       0

#define BT_SINK_SRV_AWS_NOTIFY_START      (BT_SINK_SRV_AWS_COMMAND_END + BT_SINK_SRV_AWS_COMMAND_ID_START)
#define BT_SINK_SRV_AWS_NOTIFY_STOP       (BT_SINK_SRV_AWS_COMMAND_END + BT_SINK_SRV_AWS_COMMAND_ID_STOP)
#define BT_SINK_SRV_AWS_NOTIFY_NEXT       (BT_SINK_SRV_AWS_COMMAND_END + BT_SINK_SRV_AWS_COMMAND_ID_NEXT)
#define BT_SINK_SRV_AWS_NOTIFY_PREV       (BT_SINK_SRV_AWS_COMMAND_END + BT_SINK_SRV_AWS_COMMAND_ID_PREV)
#define BT_SINK_SRV_AWS_NOTIFY_SET_VOLUME (BT_SINK_SRV_AWS_COMMAND_END + BT_SINK_SRV_AWS_COMMAND_ID_SET_VOLUME)
#define BT_SINK_SRV_AWS_NOTIFY_SET_ROLE   (BT_SINK_SRV_AWS_COMMAND_END + BT_SINK_SRV_AWS_COMMAND_ID_SET_ROLE)
#define BT_SINK_SRV_AWS_NOTIFY_SET_MODE   (BT_SINK_SRV_AWS_COMMAND_END + BT_SINK_SRV_AWS_COMMAND_ID_SET_MODE)

typedef struct {
    uint32_t handle;
    union {
        uint8_t data[4];
        bt_status_t status;
        bt_aws_role_t role;
        bt_sink_srv_music_mode_t mode;
        bt_sink_srv_am_volume_level_out_t vol_lev;
    };
} bt_sink_srv_aws_mp3_packet;

/* AWS MP3 APIs */
int32_t bt_sink_srv_aws_mp3_set_role_request(uint32_t handle, bt_aws_role_t role);
int32_t bt_sink_srv_aws_mp3_set_role_response(uint32_t handle, bt_status_t status);
int32_t bt_sink_srv_aws_mp3_start_request(uint32_t handle);
int32_t bt_sink_srv_aws_mp3_start_response(uint32_t handle, bt_status_t status);
int32_t bt_sink_srv_aws_mp3_stop_request(uint32_t handle);
int32_t bt_sink_srv_aws_mp3_stop_response(uint32_t handle, bt_status_t status);
int32_t bt_sink_srv_aws_mp3_change_track_request(uint32_t handle);
int32_t bt_sink_srv_aws_mp3_change_track_response(uint32_t handle, bt_status_t status);
int32_t bt_sink_srv_aws_mp3_notify_vol_level(uint32_t handle, bt_sink_srv_am_volume_level_out_t level);
int32_t bt_sink_srv_aws_mp3_notify_start(uint32_t handle);
int32_t bt_sink_srv_aws_mp3_notify_stop(uint32_t handle);
int32_t bt_sink_srv_aws_mp3_notify_change_ntrack(uint32_t handle);
int32_t bt_sink_srv_aws_mp3_notify_change_ptrack(uint32_t handle);

/* MP3 Common callback */
bt_status_t bt_sink_srv_aws_mp3_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);

/* streaming handlers */
int32_t bt_sink_srv_aws_mp3_build_streaming_packet(uint8_t *mp3_hdr,
                  uint8_t *data, int32_t data_len, bt_aws_streaming_packet_t *packet);

int32_t bt_sink_srv_aws_mp3_handle_streaming_ind(bt_aws_custom_packet_t *packet);

void bt_sink_srv_local_mode_check(bt_sink_srv_music_mode_t mode);

void bt_sink_srv_aws_mp3_send_sync_time(void);

int32_t bt_sink_srv_aws_mp3_wait_sync_time(void);
#endif // __BT_AWS_SUPPORT__

#endif /* BT_SINK_SRV_AWS_MP3_H */

