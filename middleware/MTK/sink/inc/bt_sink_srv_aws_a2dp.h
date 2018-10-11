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

#ifndef __BT_SINK_SRV_AWS_A2DP_H__
#define __BT_SINK_SRV_AWS_A2DP_H__

#include "bt_sink_srv_ami.h"

#ifdef __BT_AWS_A2DP_SUPPORT__
#include "bt_sink_srv_conmgr.h"
#include "bt_aws.h"
#include "bt_sink_srv_action.h"
#include "bt_sink_srv.h"
#include "audio_src_srv.h"
#include "bt_sink_srv_music.h"
#include "hal_feature_config.h"

#if defined(__AFE_HS_DC_CALIBRATION__)
#define BT_SINK_SRV_AWS_A2DP_OUTPUT_DEVICE          (HAL_AUDIO_DEVICE_HANDSET)
#else
#define BT_SINK_SRV_AWS_A2DP_OUTPUT_DEVICE          (HAL_AUDIO_DEVICE_HEADSET)
#endif

#define BT_SINK_SRV_AWS_A2DP_VOL_DEF_LEV            (AUD_VOL_OUT_LEVEL6)

#define BT_SINK_SRV_AWS_A2DP_MAGIC_CODE             (0xa0)

#ifdef __cplusplus
extern "C" {
#endif

void bt_sink_srv_aws_a2dp_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *user);

bt_sink_srv_status_t bt_sink_srv_aws_a2dp_action_handler(bt_sink_srv_action_t action, void *param);

bt_status_t bt_sink_srv_aws_a2dp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);

int32_t bt_sink_srv_aws_a2dp_sync_vol_level(uint32_t handle, uint8_t level);

void bt_sink_srv_aws_a2dp_init();

void bt_sink_srv_aws_a2dp_play(audio_src_srv_handle_t *handle);

void bt_sink_srv_aws_a2dp_stop(audio_src_srv_handle_t *handle);

void bt_sink_srv_aws_a2dp_suspend(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd);

void bt_sink_srv_aws_a2dp_reject(audio_src_srv_handle_t *handle);

void bt_sink_srv_aws_a2dp_exception(audio_src_srv_handle_t *handle, int32_t event, void *param);

void bt_sink_srv_aws_a2dp_create_pse_handle(void);

void bt_sink_srv_aws_a2dp_destroy_pse_handle(void);

void bt_sink_srv_aws_a2dp_drv_play(void *param);

void bt_sink_srv_aws_a2dp_drv_stop(void *param);

int32_t bt_sink_srv_aws_a2dp_start_request(uint32_t handle, bt_sink_srv_music_device_t *dev);

bt_sink_srv_status_t bt_sink_srv_music_aws_a2dp_action_handler(bt_sink_srv_action_t action, void *param);

void bt_sink_srv_aws_a2dp_wait_start_cnf_timeout(uint32_t timer_id, uint32_t data);

#ifdef __cplusplus
}
#endif

#endif /* __BT_AWS_A2DP_SUPPORT__ */
#endif /* __BT_SINK_SRV_AWS_A2DP_H__ */

