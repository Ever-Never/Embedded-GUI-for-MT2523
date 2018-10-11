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

#ifndef __BT_SINK_SRV_AWS_H__
#define __BT_SINK_SRV_AWS_H__

#include "bt_sink_srv_ami.h"

#ifdef __BT_AWS_SUPPORT__
#include "bt_sink_srv_conmgr.h"
#include "bt_aws.h"
#include "bt_sink_srv_action.h"
#include "bt_sink_srv.h"


#define BT_SINK_SRV_AWS_DATA_THRESHHOLD    (2 * 1024)

#define BT_SINK_SRV_AWS_PKT_THRESHHOLD     (2)

#define BT_SINK_SRV_AWS_INVALID_TIME       0xFFFFFFFF


/* custom SNK cmd following TLV */
#define BT_SINK_SRV_AWS_CMD_SNK_INTERRUPT              (0x01)
#define BT_SINK_SRV_AWS_CMD_SNK_RESUME                 (0x02)


#define BT_SINK_SRV_AWS_CMD_MODE_SWITCH                (0xf0)
#define BT_SINK_SRV_AWS_CMD_VOLUME_SET                 (0xf1)



#ifdef __cplusplus
extern "C" {
#endif

#define __BT_SINK_SRV_AWS_LIST_DEBUG__


void bt_sink_srv_aws_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *user);

bt_sink_srv_status_t bt_sink_srv_aws_action_handler(bt_sink_srv_action_t action, void *param);

bt_status_t bt_sink_srv_aws_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);

int32_t bt_sink_srv_aws_set_pause(uint32_t handle);

int32_t bt_sink_srv_aws_set_play(uint32_t handle, bt_aws_role_t role, uint8_t change_track);

int32_t bt_sink_srv_aws_set_mode_change(uint32_t handle);

int32_t bt_sink_srv_aws_set_resync(uint32_t handle);

int32_t bt_sink_srv_aws_notify_play(uint32_t handle);

int32_t bt_sink_srv_aws_notify_stop(uint32_t handle);

int32_t bt_sink_srv_aws_notify_change_ntrack(uint32_t handle);

int32_t bt_sink_srv_aws_notify_change_ptrack(uint32_t handle);

int32_t bt_sink_srv_aws_notify_resync(uint32_t handle);

int32_t bt_sink_srv_aws_notify_vol_level(uint32_t handle, uint8_t level);

int32_t bt_sink_srv_aws_notify_vol_up(uint32_t handle);

int32_t bt_sink_srv_aws_notify_vol_down(uint32_t handle);

int32_t bt_sink_srv_aws_notify_mode_change(uint32_t handle, uint8_t mode);

void bt_sink_srv_local_mode_check(bt_sink_srv_music_mode_t mode);

#ifdef __cplusplus
}
#endif

#endif
#endif /* __BT_AWS_SUPPORT__ */

