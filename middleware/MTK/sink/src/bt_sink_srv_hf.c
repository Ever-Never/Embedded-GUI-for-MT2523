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

#include <stdbool.h>
#include "bt_sink_srv_resource.h"
#include "bt_hfp.h"
#include "bt_sink_srv_hf.h"
#include "bt_sink_srv_conmgr.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_common.h"
#include "bt_sink_srv_music.h"
#ifdef __BT_SINK_SRV_AM_SUPPORT__
#include "bt_sink_srv_ami.h"
#endif /* __BT_SINK_SRV_AM_SUPPORT__ */
#include "bt_sink_srv_pbapc.h"
#include "hal_feature_config.h"

extern void bt_sink_srv_hf_dev_play_int(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p, bt_sink_srv_hf_audio_type_t audio_type);
extern void bt_sink_srv_hf_dev_codec_open(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p);
extern void bt_sink_srv_hf_dev_codec_close(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p);
#ifdef __BT_SINK_SRV_VOICE_PROMPT__
extern void bt_sink_srv_vp_dev_set_action(bt_sink_srv_action_t action);
#endif

#ifdef __BT_SINK_SRV_AM_SUPPORT__
#if defined(__AFE_HS_DC_CALIBRATION__)
    #define BT_SINK_SRV_HF_OUTPUT_DEVICE HAL_AUDIO_DEVICE_HANDSET
#else
    #define BT_SINK_SRV_HF_OUTPUT_DEVICE HAL_AUDIO_DEVICE_HEADSET
#endif
#ifdef MTK_DEVELOPMENT_BOARD_HDK
    #define BT_SINK_SRV_HF_INPUT_DEVICE HAL_AUDIO_DEVICE_MAIN_MIC
#else
    #define BT_SINK_SRV_HF_INPUT_DEVICE HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC
#endif
#endif /* __BT_SINK_SRV_AM_SUPPORT__ */

bt_sink_srv_hf_context_t g_sink_srv_hf_context[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
bt_sink_srv_hf_context_t *g_sink_srv_hf_missed_call_device_p;
bt_hfp_audio_codec_type_t g_sink_srv_hf_audio_codec = (bt_hfp_audio_codec_type_t)(BT_HFP_CODEC_TYPE_CVSD | BT_HFP_CODEC_TYPE_MSBC);

static const uint16_t bt_sink_srv_hf_tone_16ksr[] = {
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6,
    0x0000, 0xffff, 0xe475, 0xcd1a, 0xcd1a, 0xb805, 0xbd80, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e6, 0xcd1b, 0x47fb, 0xe475, 0x32e5,
    0x0000, 0x0000, 0x1b8c, 0xcd1a, 0x32e5, 0xb805, 0x4281, 0xcd1b, 0x47fa, 0x0000, 0x4280, 0x32e5, 0x32e6, 0x47fb, 0x1b8b, 0x32e5,
    0x0000, 0x0000, 0xe475, 0xcd1b, 0xcd1b, 0xb805, 0xbd81, 0xcd1b, 0xb806, 0x0000, 0xbd80, 0x32e5, 0xcd1b, 0x47fb, 0xe474, 0x32e5,
    0x0001, 0x0000, 0x1b8c, 0xcd1b, 0x32e5, 0xb806, 0x4280, 0xcd1a, 0x47fb, 0xffff, 0x427f, 0x32e5, 0x32e5, 0x47f9, 0x1b8c, 0x32e6
};

#if defined( __BT_SINK_SRV_DEBUG_INFO__ ) &&  defined(__BT_SINK_SRV_TRACE__)
const static char *g_sink_hf_event_string[] = {
    "BT_HFP_SLC_CONNECTING_IND",                 /**< This event indicates when service level connection is connecting. */
    "BT_HFP_SLC_CONNECTED_IND",                 /**< This event indicates when service level connection has been connected. */
    "BT_HFP_CONNECT_REQUEST_IND",                 /**< This event indicates when AG want to connect with HF. */
    "BT_HFP_DISCONNECT_IND",                 /**< This event indicates RFCOMM link has been disconnected. */
    "BT_HFP_AUDIO_CONNECT_IND",                 /**< This event indicates the SCO has been connected. */
    "BT_HFP_AUDIO_DISCONNECT_IND",                 /**< This event indicates the SCO has been disconnected. */
    "BT_HFP_BRSF_FEATURES_IND",                 /**< This event indicates the AG supported features. */
    "BT_HFP_CALL_HOLD_FEATURES_IND",                 /**< This event indicates the call held actions which AG supports. */
    "BT_HFP_CIEV_CALL_SETUP_IND",                 /**< This event indicates the call setup status when changed. */
    "BT_HFP_CIEV_CALL_IND",                 /**< This event indicates the call status when changed. */
    "BT_HFP_CIEV_CALL_HOLD_IND",                 /**< This event indicates call hold status when changed. */
    "BT_HFP_CIEV_SERVICE_IND",                 /**< This event indicates service roam status when changed. */
    "BT_HFP_CIEV_SIGNAL_IND",                 /**< This event indicates the signal value when changed. */
    "BT_HFP_CIEV_ROAMING_IND",                 /**< This event indicates the roam status when changed. */
    "BT_HFP_CIEV_BATTCHG_IND",                 /**< This event indicates the battery value when changed. */
    "BT_HFP_RING_IND",                 /**< This event indicates the ring has happened. */
    "BT_HFP_CALL_WAITING_IND",                 /**< This event indicates the waiting call information when get the waiting call. */
    "BT_HFP_CALLER_ID_IND",                 /**< This event indicates the caller information when get incoming call. */
    "BT_HFP_CURRENT_CALLS_IND",                 /**< This event indicates the current call information when query current calls. */
    "BT_HFP_VOICE_RECOGNITION_IND",                 /**< This event indicates the voice recognition feature has changed. */
    "BT_HFP_VOLUME_SYNC_SPEAKER_GAIN_IND",                 /**< This event indicates the value of remote speaker volume when changed. */
    "BT_HFP_VOLUME_SYNC_MIC_GAIN_IND",                 /**< This event indicates the value of remote microphone volume when changed. */
    "BT_HFP_IN_BAND_RING_IND",                 /**< This event indicates the in band ring feature has changed. */
    "BT_HFP_ACTION_CMD_CNF",
    "BT_HFP_AUDIO_STATUS_CHANGE_IND"
};

const static char *g_sink_hf_call_state[] = {
    "IDLE",
    NULL,
    NULL,
    NULL,
    "INCOMING",
    "OUTGOING",
    "ACTIVE",
    "TWC_INCOMING",
    "TWC_OUTGOING",
    "HELD_ACTIVE",
    "HELD_REMAINING",
    "MULTIPARTY"
};

#if 0
const static char *sink_hf_audio_type[] = {
    "BT_SINK_SRV_HF_AUDIO_CALL",
    "BT_SINK_SRV_HF_AUDIO_SCO",
    "BT_SINK_SRV_HF_AUDIO_RING",
    "BT_SINK_SRV_HF_AUDIO_TOTAL"
};

const static char *sink_hf_audio_msg[] = {
    "AUD_SELF_CMD_REQ",
    "AUD_RESUME_IND",
    "AUD_SUSPEND_IND",
    "AUD_SUSPEND_BY_IND",
    "AUD_A2DP_PROC_IND",
    "AUD_STREAM_EVENT_IND",
    "AUD_TIMER_IND",
    "AUD_SINK_OPEN_CODEC"
};

const static char *sink_hf_audio_sub_msg[] = {
    "AUD_EMPTY",
    "AUD_SINK_PROC_PTR",
    "AUD_CMD_FAILURE",
    "AUD_CMD_COMPLETE",
    "AUD_SUSPEND_BY_NONE",
    "AUD_SUSPEND_BY_PCM",
    "AUD_SUSPEND_BY_HFP",
    "AUD_SUSPEND_BY_A2DP"
};
#endif

#endif /*defined( __BT_SINK_SRV_DEBUG_INFO__ ) &&  defined(__BT_SINK_SRV_TRACE__)*/

bt_sink_srv_hf_context_t *bt_sink_srv_hf_get_device(audio_src_srv_handle_t *audio_src)
{
    uint8_t i = 0;
    for(i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++)
    {
        if (g_sink_srv_hf_context[i].audio_src == audio_src) {
            return &g_sink_srv_hf_context[i];
        }
    }

    return NULL;
}

bt_sink_srv_hf_context_t *bt_sink_srv_hf_get_device_by_handle(uint32_t handle)
{
    uint8_t i = 0;
    for(i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++)
    {
        if (g_sink_srv_hf_context[i].link.handle == handle) {
            return &g_sink_srv_hf_context[i];
        }
    }

    return NULL;
}

bt_sink_srv_hf_context_t *bt_sink_srv_hf_get_available_device()
{
    uint8_t i = 0;
    for(i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++)
    {
        if (g_sink_srv_hf_context[i].link.handle == 0) {
            return &g_sink_srv_hf_context[i];
        }
    }

    return NULL;
}

#ifdef __BT_SINK_SRV_AM_SUPPORT__
void bt_sink_srv_hf_audio_pcm_parameter_init(bt_sink_srv_am_audio_capability_t *audio_capability)
{
    if (NULL != audio_capability) {
        bt_sink_srv_hf_stored_data_t stored_data;
        bt_sink_srv_cm_get_profile_data(&audio_capability->dev_addr, BT_SINK_SRV_PROFILE_HFP, &stored_data);
        bt_sink_srv_report("[Sink][HF]init voulume level:%x", stored_data.speaker_volume);
        audio_capability->type = PCM;
        audio_capability->audio_stream_out.audio_device = BT_SINK_SRV_HF_OUTPUT_DEVICE;
        audio_capability->audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t ) stored_data.speaker_volume;
        audio_capability->codec.pcm_format.stream.stream_sample_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
        audio_capability->codec.pcm_format.stream.stream_channel = HAL_AUDIO_STEREO;
        audio_capability->codec.pcm_format.stream.buffer = (void *)bt_sink_srv_hf_tone_16ksr;
        audio_capability->codec.pcm_format.stream.size = sizeof(bt_sink_srv_hf_tone_16ksr);
        audio_capability->codec.pcm_format.in_out = STREAM_OUT;
        audio_capability->audio_path_type = HAL_AUDIO_PLAYBACK_MUSIC;
    }
}

void bt_sink_srv_hf_audio_sco_parameter_init(bt_sink_srv_am_audio_capability_t *audio_capability, bt_hfp_audio_codec_type_t codec)
{
    if (NULL != audio_capability) {
        bt_sink_srv_hf_stored_data_t stored_data;
        bt_sink_srv_cm_get_profile_data(&audio_capability->dev_addr, BT_SINK_SRV_PROFILE_HFP, &stored_data);
        bt_sink_srv_report("[Sink][HF]init voulume level:%x", stored_data.speaker_volume);
        audio_capability->type = HFP;
        audio_capability->codec.hfp_format.hfp_codec.type = codec;
        audio_capability->audio_stream_in.audio_device = BT_SINK_SRV_HF_INPUT_DEVICE;
        audio_capability->audio_stream_in.audio_volume = AUD_VOL_IN_LEVEL0;
        audio_capability->audio_stream_out.audio_device = BT_SINK_SRV_HF_OUTPUT_DEVICE;
        audio_capability->audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t )stored_data.speaker_volume;
        bt_sink_srv_report("[Sink][HF][AM] sco input device: 0x%x", BT_SINK_SRV_HF_INPUT_DEVICE);
    }
}

uint8_t bt_sink_srv_hf_volume_to_bt(uint8_t local_volume)
{
#if 0
    uint8_t sink_hf_vol_mapping[7] = {0, 3, 6, 9, 11, 13, 15};

    bt_sink_srv_assert(7 > local_volume);
    return sink_hf_vol_mapping[local_volume];
#endif
    bt_sink_srv_assert(15 >= local_volume);
    return local_volume;
}

uint8_t bt_sink_srv_hf_volume_to_local(uint8_t bt_volume)
{
#if 0
    uint8_t sink_hf_vol_mapping[16] = {AUD_VOL_OUT_LEVEL0,
                                       AUD_VOL_OUT_LEVEL1,
                                       AUD_VOL_OUT_LEVEL1,
                                       AUD_VOL_OUT_LEVEL2,
                                       AUD_VOL_OUT_LEVEL2,
                                       AUD_VOL_OUT_LEVEL3,
                                       AUD_VOL_OUT_LEVEL3,
                                       AUD_VOL_OUT_LEVEL3,
                                       AUD_VOL_OUT_LEVEL4,
                                       AUD_VOL_OUT_LEVEL4,
                                       AUD_VOL_OUT_LEVEL4,
                                       AUD_VOL_OUT_LEVEL5,
                                       AUD_VOL_OUT_LEVEL5,
                                       AUD_VOL_OUT_LEVEL5,
                                       AUD_VOL_OUT_LEVEL6,
                                       AUD_VOL_OUT_LEVEL6
                                      };
#endif
    bt_sink_srv_assert(15 >= bt_volume);
    return bt_volume;
}

static bool bt_sink_srv_hf_change_volume(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p, uint8_t volume)
{
    bool result = false;
    bt_sink_srv_hf_stored_data_t stored_data;
    bt_bd_addr_t *address_p;
    bt_sink_srv_hf_link_context_t *link_cntx = &bt_sink_srv_hf_context_p->link;
    address_p = bt_hfp_get_bd_addr_by_handle(link_cntx->handle);
    if (NULL != address_p
            && bt_sink_srv_cm_get_profile_data(address_p, BT_SINK_SRV_PROFILE_HFP, &stored_data)
            && stored_data.speaker_volume != volume) {

        stored_data.speaker_volume = volume;
        bt_sink_srv_cm_set_profile_data(address_p, BT_SINK_SRV_PROFILE_HFP, &stored_data);

        if (NULL != bt_sink_srv_hf_context_p) {
            bt_sink_srv_report("[Sink][HF] Volume Change: handle:%d, volume:%d, audio_id:%d, audio_type:%d", 
                link_cntx->handle, 
                volume,
                link_cntx->audio_id,
                link_cntx->audio_type);

            if (link_cntx->audio_type == BT_SINK_SRV_HF_AUDIO_SCO) {
                if (BT_SINK_SRV_HF_INVALID_AUDIO_ID != link_cntx->audio_id) {
                    bt_sink_srv_ami_audio_set_volume(link_cntx->audio_id,
                                                     volume, STREAM_OUT);
                    result = true;
                }
            } else if (link_cntx->audio_type == BT_SINK_SRV_HF_AUDIO_RING) {
                if (BT_SINK_SRV_HF_INVALID_AUDIO_ID != link_cntx->audio_id) {
                    bt_sink_srv_ami_audio_set_volume(link_cntx->audio_id,
                                                     volume, STREAM_OUT);
                    result = true;
                }
            }
        }
    }

    if (result) {
        bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));;

        if (NULL != event) {
            bt_sink_srv_memcpy((void *)&event->volume_change.address, (void *)address_p, sizeof(bt_bd_addr_t));
            event->volume_change.current_volume = volume;
            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_SPEAKER_VOLUME_CHANGE, event);
            bt_sink_srv_memory_free(event);
        }
    }

    return result;
}

bool bt_sink_srv_hf_volume_change(bt_sink_srv_hf_volume_act_t vol_act, bool min_max)
{
    bool result = false;
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_highlight_device();
    bt_sink_srv_hf_link_context_t *link_cntx = &bt_sink_srv_hf_context_p->link;

    if (NULL != bt_sink_srv_hf_context_p &&
            link_cntx->call_state > BT_SINK_SRV_HF_CALL_STATE_IDLE) {
        bt_sink_srv_hf_stored_data_t stored_data;
        bt_bd_addr_t *address_p = bt_hfp_get_bd_addr_by_handle(link_cntx->handle);
        bt_sink_srv_cm_get_profile_data(address_p, BT_SINK_SRV_PROFILE_HFP, &stored_data);

        if (BT_SINK_SRV_HF_VOL_ACT_UP == vol_act) {
            if (min_max) {
                stored_data.speaker_volume = BT_SINK_SRV_HF_MAX_VOLUME;
                result = bt_sink_srv_hf_change_volume(bt_sink_srv_hf_context_p, stored_data.speaker_volume);
            } else {
                if (BT_SINK_SRV_HF_MAX_VOLUME > stored_data.speaker_volume) {
                    stored_data.speaker_volume++;
                    result = bt_sink_srv_hf_change_volume(bt_sink_srv_hf_context_p, stored_data.speaker_volume);
                }
            }
        } else if (BT_SINK_SRV_HF_VOL_ACT_DOWN == vol_act) {
            if (min_max) {
                stored_data.speaker_volume = BT_SINK_SRV_HF_MIN_VOLUME;
                result = bt_sink_srv_hf_change_volume(bt_sink_srv_hf_context_p, stored_data.speaker_volume);
            } else {
                if (BT_SINK_SRV_HF_MIN_VOLUME < stored_data.speaker_volume) {
                    stored_data.speaker_volume--;
                    result = bt_sink_srv_hf_change_volume(bt_sink_srv_hf_context_p, stored_data.speaker_volume);
                }
            }
        }
        if (result) {
            char buffer[BT_SINK_SRV_HF_CMD_LENGTH];

            snprintf((char *)buffer,
                     BT_SINK_SRV_HF_CMD_LENGTH,
                     "AT+VGS=%d",
                     bt_sink_srv_hf_volume_to_bt(stored_data.speaker_volume));
            BT_SINK_SRV_HF_SYNC_SPEAKER_GAIN(link_cntx->handle, buffer);
        }
    }
    return result;
}
#endif /* __BT_SINK_SRV_AM_SUPPORT__ */


void bt_sink_srv_hf_call_state_change(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p,
                                      bt_sink_srv_hf_call_state_t previous_state,
                                      bt_sink_srv_hf_call_state_t new_state)
{

#ifdef __BT_SINK_SRV_DEBUG_INFO__
    uint32_t index, previous = 0, now = 0;
    for (index = 3; index < 11; index++) {
        if ((1 << index) & previous_state) {
            previous = index;
        } else if ((1 << index) & new_state) {
            now = index;
        }
    }
    if (bt_sink_srv_hf_context_p) {
        bt_sink_srv_report("[Sink][HF] Call state change, handle:0x%x, prev:%s, new:%s",
                       bt_sink_srv_hf_context_p->link.handle,
                       g_sink_hf_call_state[previous],
                       g_sink_hf_call_state[now]);
        }
#endif /* __BT_SINK_SRV_DEBUG_INFO__ */

    if (NULL != bt_sink_srv_hf_context_p) {
    bt_sink_srv_hf_link_context_t *link_cntx = &bt_sink_srv_hf_context_p->link;
     #ifdef __BT_SINK_SRV_VOICE_PROMPT__
        if (BT_SINK_SRV_HF_CALL_STATE_ACTIVE == previous_state
                       && BT_SINK_SRV_HF_CALL_STATE_IDLE == new_state) {
            bt_sink_srv_vp_dev_set_action(BT_SINK_SRV_ACTION_HANG_UP);
        }
    #endif

        if (BT_SINK_SRV_HF_CALL_STATE_IDLE != previous_state &&
                BT_SINK_SRV_HF_CALL_STATE_IDLE == new_state) {
            if (bt_sink_srv_hf_context_p->dev_state == BT_SINK_SRV_HF_DEV_STATE_PLAYING ||
                bt_sink_srv_hf_context_p->dev_sub_state == BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_PLAY) {
                bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_STOP);
            } else {
                bt_sink_srv_report("[Sink][HF] Call state change, state:%d, sub state:%d", bt_sink_srv_hf_context_p->dev_state, bt_sink_srv_hf_context_p->dev_sub_state);
            }
         } else if (BT_SINK_SRV_HF_CALL_STATE_IDLE == previous_state &&
                   BT_SINK_SRV_HF_CALL_STATE_IDLE != new_state) {
             if (bt_sink_srv_hf_context_p->dev_state == BT_SINK_SRV_HF_DEV_STATE_READY) {
                 bt_sink_srv_hf_context_p->link.audio_type = BT_SINK_SRV_HF_AUDIO_NULL;
                 bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_PLAY);
             } else {
                 bt_sink_srv_report("[Sink][HF]dev state:%d", bt_sink_srv_hf_context_p->dev_state);
             }
        #if 0     
        } else if (BT_SINK_SRV_HF_CALL_STATE_INCOMING == previous_state &&
            BT_SINK_SRV_HF_CALL_STATE_INCOMING != new_state) {
            if (link_cntx->audio_type == BT_SINK_SRV_HF_AUDIO_RING &&
                bt_sink_srv_hf_context_p->dev_state == BT_SINK_SRV_HF_DEV_STATE_PLAYING) {
                bt_sink_srv_hf_dev_stop_int(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_AUDIO_RING);
            }
        #endif    
        }

        if (BT_SINK_SRV_HF_CALL_STATE_INCOMING == previous_state
                && BT_SINK_SRV_HF_CALL_STATE_IDLE == new_state
                && (!(link_cntx->flag & BT_SINK_SRV_HF_FLAG_USER_REJECT))
                && bt_sink_srv_strlen((char *)link_cntx->caller.number) > 0) {
            bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));;

            g_sink_srv_hf_missed_call_device_p = bt_sink_srv_hf_context_p;
            bt_sink_srv_memcpy((void *)&event->caller_info,
                               (void *)&link_cntx->caller,
                               sizeof(bt_sink_srv_caller_information_t));
            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_MISSED_CALL, event);
            bt_sink_srv_memory_free(event);
        }

        if (BT_SINK_SRV_HF_CALL_STATE_IDLE == new_state) {
            link_cntx->flag &= (~BT_SINK_SRV_HF_FLAG_USER_REJECT);
        }
    }
}

static void bt_sink_srv_hf_get_name_callback(bt_bd_addr_t *address,
        bt_sink_srv_pbapc_record_t *record,
        void *user_data)
{
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = NULL;
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)user_data;

    if (bt_sink_srv_strlen((char *)record->name) > 0) {
        bt_sink_srv_memcpy((void *)event->caller_info.name, (void *)record->name, BT_SINK_SRV_MAX_NAME);
    }

    bt_sink_srv_hf_context_p = bt_sink_srv_cm_get_profile_info(address, BT_SINK_SRV_PROFILE_HFP);

    if (NULL != bt_sink_srv_hf_context_p) {
        bt_sink_srv_memcpy((void *)&bt_sink_srv_hf_context_p->link.caller,
                           (void *)&event->caller_info,
                           sizeof(bt_sink_srv_caller_information_t));
    }
    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION, event);
    bt_sink_srv_memory_free(event);
}

static void bt_sink_srv_hf_sco_state_change_notify(bt_sink_srv_hf_context_t *hf_context, bt_sink_srv_sco_link_state_t state)
{
    bt_sink_srv_report("[Sink][HF] codec: 0x%x, sco state:%d", hf_context->link.codec, state);
    bt_bd_addr_t *address_p = bt_sink_srv_cm_get_address_by_profile_info((void *) hf_context);
    bt_sink_srv_event_param_t *event = NULL;
    event = bt_sink_srv_memory_alloc(sizeof(*event));
    bt_sink_srv_memcpy(&event->hf_sco_state.address, address_p, sizeof(*address_p));
    event->hf_sco_state.codec_type = hf_context->link.codec;
    event->hf_sco_state.state = state;
    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE, event);
    bt_sink_srv_memory_free(event);
    return;
}

void bt_sink_srv_hf_reset_link_context(bt_sink_srv_hf_link_context_t* link_ctx)
{
    if (link_ctx != NULL) {
        bt_sink_srv_memset(link_ctx, 0, sizeof(bt_sink_srv_hf_link_context_t));
        link_ctx->audio_id = BT_SINK_SRV_HF_INVALID_AUDIO_ID;
        link_ctx->audio_type = BT_SINK_SRV_HF_AUDIO_NONE;
    }
    return;
}

bt_status_t bt_sink_srv_hf_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_bd_addr_t *address_p = NULL;
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = NULL;

#ifdef __BT_SINK_SRV_DEBUG_INFO__
    if (msg >= BT_HFP_SLC_CONNECTING_IND && msg <= BT_HFP_AUDIO_STATUS_CHANGE_IND) {
        bt_sink_srv_report("[Sink][HF] SDK msg:%s", g_sink_hf_event_string[msg - BT_HFP_SLC_CONNECTING_IND]);
    } else {
        bt_sink_srv_report("[Sink][HF] SDK msg:0x%x", msg);
    }
#else
    bt_sink_srv_report("[Sink][HF] SDK msg:0x%x", msg);
#endif /* __BT_SINK_SRV_DEBUG_INFO__ */

    switch (msg) {
        case BT_HFP_SLC_CONNECTED_IND: {
            bt_hfp_slc_connected_ind_t *message = (bt_hfp_slc_connected_ind_t *)buffer;
            const bt_sink_srv_features_config_t *config_p = bt_sink_srv_get_features_config();

            if(NULL == (address_p = bt_hfp_get_bd_addr_by_handle(message->handle))) {
                break;
            }
            bt_sink_srv_report("[Sink][HF] CONNECT_IND, handle:0x%x", message->handle);
            
            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(message->handle);
            if (NULL != bt_sink_srv_hf_context_p) {    
            bt_sink_srv_hf_context_p->audio_src->dev_id = bt_sink_srv_music_convert_btaddr_to_devid(address_p);
            bt_sink_srv_hf_dev_codec_open(bt_sink_srv_hf_context_p);
            bt_sink_srv_hf_dev_set_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_STATE_READY);
#ifdef __BT_SINK_SRV_AM_SUPPORT__
                bt_sink_srv_hf_stored_data_t stored_data;

                if (bt_sink_srv_cm_get_profile_data(address_p, BT_SINK_SRV_PROFILE_HFP, &stored_data)) {
                    if (stored_data.speaker_volume == AUD_VOL_OUT_LEVEL0) {
                        stored_data.speaker_volume = BT_SINK_SRV_HF_DEFAULT_VOLUME;
                        bt_sink_srv_cm_set_profile_data(address_p, BT_SINK_SRV_PROFILE_HFP, &stored_data);
                    }
                }
#endif /* __BT_SINK_SRV_AM_SUPPORT__ */
                if (BT_SINK_SRV_FEATURE_IOS_HFP_CUSTOM_AT_COMMAND & config_p->features) {
                    bt_sink_srv_ios_hf_features_t ios_features = bt_sink_srv_get_ios_hf_features_params();
                    bt_sink_srv_report("[Sink][HF] ios_features:0x%x", ios_features);
                    bt_sink_srv_hf_enable_apl_custom_commands(bt_sink_srv_hf_context_p->link.handle, ios_features);
                }
                if (BT_SINK_SRV_FEATURE_AUTO_CONNECT_PBAPC & config_p->features) {
                    bt_sink_srv_pbapc_connect(address_p);
                }
                bt_sink_srv_cm_profile_status_notify(address_p, BT_SINK_SRV_PROFILE_HFP, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED);                
            }
        }
        break;

        case BT_HFP_CONNECT_REQUEST_IND: {
            bt_hfp_connect_request_ind_t *message = (bt_hfp_connect_request_ind_t *)buffer;
           //bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_available_device();
            bt_sink_srv_hf_context_p = bt_sink_srv_cm_get_profile_info(message->address, BT_SINK_SRV_PROFILE_HFP);
            if (NULL != bt_sink_srv_hf_context_p) {
                bt_sink_srv_hf_context_p->link.handle = message->handle;
                bt_hfp_connect_response(message->handle, true);
                bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_CONNECTING);
                bt_sink_srv_cm_profile_status_notify(message->address, BT_SINK_SRV_PROFILE_HFP, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTING);
            }    
        }
        break;

        case BT_HFP_DISCONNECT_IND: {
            bt_hfp_disconnect_ind_t *message = (bt_hfp_disconnect_ind_t *)buffer;
            bt_sink_srv_report("[Sink][HF] DISCONNECT_IND, handle:0x%x", message->handle);

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(message->handle);
            
            if (NULL != bt_sink_srv_hf_context_p) {
                if (bt_sink_srv_hf_context_p->dev_state == BT_SINK_SRV_HF_DEV_STATE_PLAYING ||
                    bt_sink_srv_hf_context_p->dev_sub_state == BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_PLAY) {
                    bt_sink_srv_hf_context_p->link.flag |= BT_SINK_SRV_HF_FLAG_DESTROY_CONTEXT;
                    bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_STOP);
                } else {
                    bt_sink_srv_hf_dev_set_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_STATE_NONE);
                    bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_NONE);
                    bt_sink_srv_hf_reset_link_context(&(bt_sink_srv_hf_context_p->link));
                }
                
                if (bt_sink_srv_hf_get_highlight_device() == bt_sink_srv_hf_context_p) {
                    bt_sink_srv_hf_mp_state_change(bt_sink_srv_hf_context_p);
                }
                
                bt_sink_srv_hf_context_p->link.handle = 0;
            }
            if(NULL != (address_p = bt_hfp_get_bd_addr_by_handle(message->handle))) {
                bt_sink_srv_cm_profile_status_notify(address_p, BT_SINK_SRV_PROFILE_HFP, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED);
            } 
        }
        break;

        case BT_HFP_BRSF_FEATURES_IND: {
            bt_hfp_feature_ind_t *message = (bt_hfp_feature_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(message->handle);

            if (NULL != bt_sink_srv_hf_context_p) {
                bt_sink_srv_hf_context_p->link.ag_featues = message->ag_feature;
            }
        }
        break;

        case BT_HFP_CALL_HOLD_FEATURES_IND: {
            bt_hfp_hold_feature_ind_t *message = (bt_hfp_hold_feature_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(message->handle);

            if (NULL != bt_sink_srv_hf_context_p) {
                bt_sink_srv_hf_context_p->link.ag_chld_feature = message->feature;
            }
        }
        break;

        case BT_HFP_AUDIO_CONNECT_IND: {
            bt_hfp_audio_connect_ind_t *message = (bt_hfp_audio_connect_ind_t *)buffer;

            bt_sink_srv_report("[Sink][HF] AUDIO_CONNECT_IND, codec:%02x", message->codec);
            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(message->handle);

            if (NULL != bt_sink_srv_hf_context_p) {
                bt_sink_srv_hf_context_p->link.codec = message->codec;
                bt_sink_srv_hf_context_p->link.flag |= BT_SINK_SRV_HF_FLAG_SCO_CREATED;
                bt_sink_srv_hf_mp_set_sco(bt_sink_srv_hf_get_highlight_device(), bt_sink_srv_hf_context_p);
                bt_sink_srv_hf_sco_state_change_notify(bt_sink_srv_hf_context_p, BT_SINK_SRV_SCO_LINK_STATE_CONNECTED);
            }
        }
        break;

        case BT_HFP_AUDIO_DISCONNECT_IND: {
            bt_hfp_audio_disconnect_ind_t *message = (bt_hfp_audio_disconnect_ind_t *)buffer;
            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(message->handle);
            if (bt_sink_srv_hf_context_p) {
                 bt_sink_srv_hf_context_p->link.flag &= (~BT_SINK_SRV_HF_FLAG_SCO_ACTIVE);
                 bt_sink_srv_hf_context_p->link.flag &= (~BT_SINK_SRV_HF_FLAG_SCO_CREATED); 
                 if (bt_sink_srv_hf_context_p->link.audio_type != BT_SINK_SRV_HF_AUDIO_NONE) {
                    // Call state is idle before sco disconnected.
                    if(bt_sink_srv_hf_context_p->link.call_state == BT_SINK_SRV_HF_CALL_STATE_IDLE) {
                        if (bt_sink_srv_hf_context_p->dev_state == BT_SINK_SRV_HF_DEV_STATE_PLAYING ||
                            bt_sink_srv_hf_context_p->dev_sub_state == BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_PLAY) {
                            bt_sink_srv_report("[Sink][HF] SCO disconnected after call is idle, state:%d", bt_sink_srv_hf_context_p->dev_state);
                            bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_STOP);
                        }
                    } else {
                        bt_sink_srv_hf_dev_play_int(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_AUDIO_NULL);
                    }
                 }
                bt_sink_srv_hf_sco_state_change_notify(bt_sink_srv_hf_context_p, BT_SINK_SRV_SCO_LINK_STATE_DISCONNECTED);
            }
        }
        break;

        case BT_HFP_CIEV_CALL_SETUP_IND: {
            bt_hfp_call_setup_ind_t *callsetup = (bt_hfp_call_setup_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(callsetup->handle);

            if (NULL != bt_sink_srv_hf_context_p) {
                bt_sink_srv_hf_handle_setup_ind(bt_sink_srv_hf_context_p, callsetup->state);
                if (BT_HFP_CIEV_CALL_SETUP_STATE_INCOMING == callsetup->state) {
                    bt_sink_srv_hf_context_p->link.flag |= BT_SINK_SRV_HF_FLAG_QUERY_NAME;
                }
            }
        }
        break;

        case BT_HFP_CIEV_CALL_IND: {
            bt_hfp_call_ind_t *call = (bt_hfp_call_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(call->handle);

            bt_sink_srv_hf_handle_call_ind((bt_sink_srv_hf_context_t *)bt_sink_srv_hf_context_p, call->state);
        }

        break;

        case BT_HFP_CIEV_CALL_HOLD_IND: {
            bt_hfp_call_hold_ind_t *callheld = (bt_hfp_call_hold_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(callheld->handle);

            bt_sink_srv_hf_handle_held_ind(bt_sink_srv_hf_context_p, callheld->state);
        }
        break;

        case BT_HFP_CURRENT_CALLS_IND: {
            bt_hfp_call_list_ind_t *message = (bt_hfp_call_list_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(message->handle);

            bt_sink_srv_hf_handle_call_info_ind((bt_sink_srv_hf_context_t *)bt_sink_srv_hf_context_p, message);

            if (bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_QUERY_LIST) {
                bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));
                address_p = bt_hfp_get_bd_addr_by_handle(message->handle);
                //(bt_sink_srv_hf_context_p->link.flag &= (~(BT_SINK_SRV_HF_FLAG_QUERY_LIST));
                if (NULL != event && NULL != address_p) {
                    bt_sink_srv_memcpy(&event->call_list.address, address_p, sizeof(*address_p));
                    event->call_list.index = message->index;
                    event->call_list.director = message->director;
                    event->call_list.state = message->state;
                    event->call_list.mode = message->mode;
                    event->call_list.multiple_party = message->multiple_party;
                    event->call_list.num_size = message->num_size;
                    event->call_list.final = false;
                    bt_sink_srv_memcpy(event->call_list.number, message->number, BT_SINK_SRV_MAX_PHONE_NUMBER);
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_CALL_LIST_INFORMATION, event);
                }
            bt_sink_srv_memory_free(event);
            }
        }
        break;

#ifdef __BT_SINK_SRV_AM_SUPPORT__
        case BT_HFP_RING_IND: {
            bt_hfp_ring_ind_t *ringtone = (bt_hfp_ring_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(ringtone->handle);
            
            /*SP exception case: if the RING_IND comes before SLC_CONNECTED_IND, discard the RIND_IND.*/
            if ((bt_sink_srv_hf_context_p != NULL) && (bt_sink_srv_hf_context_p->dev_state == BT_SINK_SRV_HF_DEV_STATE_NONE)) {
                bt_sink_srv_report("[Sink][HF] Discard the RING_IND.");
                break;
            }

            // Don't support in-band ringtone, need to play local ringtone.
            if (bt_sink_srv_hf_context_p && (!(bt_sink_srv_hf_context_p->link.ag_featues & BT_HFP_AG_FEATURE_IN_BAND_RING))) {
                if (bt_sink_srv_hf_context_p == bt_sink_srv_hf_get_highlight_device()) {
                    uint8_t current_id = bt_sink_srv_hf_context_p->link.audio_id;

                    if (bt_sink_srv_hf_context_p->link.audio_type == BT_SINK_SRV_HF_AUDIO_RING) {
                        bt_sink_srv_ami_audio_continue_stream(current_id, (void *)bt_sink_srv_hf_tone_16ksr, sizeof(bt_sink_srv_hf_tone_16ksr));
                    } else if (bt_sink_srv_hf_context_p->link.audio_type == BT_SINK_SRV_HF_AUDIO_NONE) {
                        bt_sink_srv_hf_context_p->link.audio_type = BT_SINK_SRV_HF_AUDIO_RING;
                        bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_PLAY);
                    } else if (bt_sink_srv_hf_context_p->link.audio_type == BT_SINK_SRV_HF_AUDIO_NULL) {
                        bt_sink_srv_hf_dev_play_int(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_AUDIO_RING);
                    } else {
                        bt_sink_srv_report("[Sink][HF] Not handle this RING_IND, audio_type: 0x%x", bt_sink_srv_hf_context_p->link.audio_type);
                    }
                }
            }
        }
        break;
#endif /* __BT_SINK_SRV_AM_SUPPORT__ */

        case BT_HFP_CALLER_ID_IND: {
            bt_hfp_caller_id_ind_t *message = (bt_hfp_caller_id_ind_t *)buffer;

            if (NULL == (bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(message->handle))) {
                break;
            }
            if (NULL == (address_p = bt_hfp_get_bd_addr_by_handle(message->handle))) {
                break;
            }
            bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));
            bt_sink_srv_memcpy(&event->caller_info.address, address_p, sizeof(*address_p));
            event->caller_info.num_size = message->num_size;
            event->caller_info.type = message->type;
            event->caller_info.waiting = false;
            bt_sink_srv_memcpy(event->caller_info.number, message->number, sizeof(event->caller_info.number));

            if ((bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_QUERY_NAME)
                    && (bt_sink_srv_pbapc_get_name_from_remote(address_p,
                            message->number,
                            (void *)event,
                            bt_sink_srv_hf_get_name_callback) == BT_STATUS_SUCCESS)) {
                bt_sink_srv_hf_context_p->link.flag &= (~BT_SINK_SRV_HF_FLAG_QUERY_NAME);
            } else {
                bt_sink_srv_pbapc_record_t *pbapc_record =
                    (bt_sink_srv_pbapc_record_t *)bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_pbapc_record_t));

                bt_sink_srv_memset((void *)pbapc_record, 0, sizeof(bt_sink_srv_pbapc_record_t));
                bt_sink_srv_memcpy((void *)pbapc_record->number, (void *)message->number, BT_SINK_SRV_MAX_PHONE_NUMBER);

                if (BT_STATUS_SUCCESS == bt_sink_srv_pbapc_get_name_from_local(address_p, pbapc_record)) {
                    bt_sink_srv_memcpy(event->caller_info.name,
                                       pbapc_record->name,
                                       BT_SINK_SRV_MAX_NAME);
                }
                bt_sink_srv_memory_free(pbapc_record);
                bt_sink_srv_memcpy((void *)&bt_sink_srv_hf_context_p->link.caller,
                                   (void *)&event->caller_info,
                                   sizeof(bt_sink_srv_caller_information_t));
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION, event);
                bt_sink_srv_memory_free(event);
            }
        }
        break;

        case BT_HFP_CALL_WAITING_IND: {
            bt_hfp_call_waiting_ind_t *message = (bt_hfp_call_waiting_ind_t *)buffer;
            if (NULL == (bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(message->handle))) {
                break;
            }
            if (NULL == (address_p = bt_hfp_get_bd_addr_by_handle(message->handle))) {
                break;
            }

            //bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));
            bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));
            bt_sink_srv_memcpy(&event->caller_info.address, address_p, sizeof(*address_p));
            event->caller_info.num_size = message->num_size;
            event->caller_info.type = message->type;
            event->caller_info.waiting = true;
            bt_sink_srv_memcpy(event->caller_info.number, message->number,  event->caller_info.num_size);

            if ((bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_QUERY_NAME)
                    && (bt_sink_srv_pbapc_get_name_from_remote(address_p,
                            message->number,
                            (void *)event,
                            bt_sink_srv_hf_get_name_callback) == BT_STATUS_SUCCESS)) {
                bt_sink_srv_hf_context_p->link.flag &= (~BT_SINK_SRV_HF_FLAG_QUERY_NAME);
            } else {
                bt_sink_srv_pbapc_record_t *pbapc_record =
                    (bt_sink_srv_pbapc_record_t *)bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_pbapc_record_t));

                bt_sink_srv_memset((void *)pbapc_record, 0, sizeof(bt_sink_srv_pbapc_record_t));
                bt_sink_srv_memcpy((void *)pbapc_record->number, (void *)message->number, BT_SINK_SRV_MAX_PHONE_NUMBER);

                if (BT_STATUS_SUCCESS == bt_sink_srv_pbapc_get_name_from_local(address_p, pbapc_record)) {
                    bt_sink_srv_memcpy(event->caller_info.name,
                                       pbapc_record->name,
                                       BT_SINK_SRV_MAX_NAME);
                }
                bt_sink_srv_memory_free(pbapc_record);
                bt_sink_srv_memcpy((void *)&bt_sink_srv_hf_context_p->link.caller,
                                   (void *)&event->caller_info,
                                   sizeof(bt_sink_srv_caller_information_t));
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION, event);
                bt_sink_srv_memory_free(event);
            }
        }
        break;

        case BT_HFP_VOLUME_SYNC_SPEAKER_GAIN_IND: {
            bt_hfp_volume_sync_speaker_gain_ind_t *message = (bt_hfp_volume_sync_speaker_gain_ind_t *)buffer;
            bt_sink_srv_report("[Sink][HF] volume:%d", message->data);
#ifdef __BT_SINK_SRV_AM_SUPPORT__
            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(message->handle);
            if (NULL == (bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(message->handle))) {
                break;
            }
            bt_sink_srv_hf_change_volume(bt_sink_srv_hf_context_p, bt_sink_srv_hf_volume_to_local(message->data));
#endif /* __BT_SINK_SRV_AM_SUPPORT__ */
        }
        break;

        case BT_HFP_IN_BAND_RING_IND: {
            bt_hfp_in_band_ring_status_ind_t *message = (bt_hfp_in_band_ring_status_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(message->handle);

            if (message->enable) {
                bt_sink_srv_hf_context_p->link.ag_featues |= BT_HFP_AG_FEATURE_IN_BAND_RING;
            } else {
                bt_sink_srv_hf_context_p->link.ag_featues &= (~BT_HFP_AG_FEATURE_IN_BAND_RING);
            }
        }
        break;

        case BT_HFP_ACTION_CMD_CNF: {
            bt_hfp_action_cnf_t *message = (bt_hfp_action_cnf_t *)buffer;
            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(message->handle);
            if (NULL != bt_sink_srv_hf_context_p && (bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_QUERY_LIST)) {
                bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));
                bt_sink_srv_hf_context_p->link.flag &= (~(BT_SINK_SRV_HF_FLAG_QUERY_LIST));
                if (NULL != event) {
                    address_p = bt_hfp_get_bd_addr_by_handle(message->handle);
                    if (address_p) {
                        bt_sink_srv_memcpy(&event->call_list.address, address_p, sizeof(*address_p));
                    }
                    event->call_list.final = true;
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_CALL_LIST_INFORMATION, event);
                }
                bt_sink_srv_memory_free(event);
            }
        }
        break;

        case BT_HFP_VOICE_RECOGNITION_IND: {
            bt_hfp_voice_recognition_ind_t *message = (bt_hfp_voice_recognition_ind_t *)buffer;
            bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));

            if (NULL != event) {
                address_p = bt_hfp_get_bd_addr_by_handle(message->handle);
                if (address_p) {
                    bt_sink_srv_memcpy(&event->voice_recognition.address, address_p, sizeof(*address_p));
                }
                event->voice_recognition.enable = message->enable;
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_VOICE_RECOGNITION_CHANGED, event);
            }
            bt_sink_srv_memory_free(event);
        }
        break;

        case BT_HFP_AUDIO_STATUS_CHANGE_IND: {
            bt_hfp_audio_status_change_ind_t *message = (bt_hfp_audio_status_change_ind_t *)buffer;

            bt_sink_srv_report("[Sink][HF]audio changed, handle:0x%x, status:0x%x", message->handle, message->audio_status);

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_device_by_handle(message->handle);

            if (NULL != bt_sink_srv_hf_context_p) {
                if (BT_HFP_AUDIO_STATUS_ACTIVE == message->audio_status) {
                    if (bt_sink_srv_hf_context_p->dev_state == BT_SINK_SRV_HF_DEV_STATE_READY) {
                        bt_sink_srv_hf_context_p->link.audio_type = BT_SINK_SRV_HF_AUDIO_SCO;
                        bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_PREPARE_PLAY);
                    } else if (bt_sink_srv_hf_context_p->dev_state == BT_SINK_SRV_HF_DEV_STATE_PLAYING) {
                        bt_sink_srv_hf_dev_play_int(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_AUDIO_SCO);
                    } else {
                        bt_sink_srv_report("[Sink][HF]dev state:%d", bt_sink_srv_hf_context_p->dev_state);
                    }
                } else {
                    bt_sink_srv_hf_context_t *highlight = bt_sink_srv_hf_get_highlight_device();
                    bt_sink_srv_hf_mp_set_sco(highlight, highlight);
                }
            }
        }
        break;

        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

extern void bt_sink_srv_hf_dev_play(audio_src_srv_handle_t *handle);
extern void bt_sink_srv_hf_dev_stop(audio_src_srv_handle_t *handle);
extern void bt_sink_srv_hf_dev_suspend(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd);
extern void bt_sink_srv_hf_dev_reject(audio_src_srv_handle_t *handle);
extern void bt_sink_srv_hf_dev_exception_handle(audio_src_srv_handle_t *handle, int32_t event, void *param);


bt_sink_srv_status_t bt_sink_srv_hf_action_handler(bt_sink_srv_action_t action, void *parameters)
{
    bt_sink_srv_status_t result = BT_SINK_SRV_STATUS_SUCCESS;
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = NULL;

    bt_sink_srv_report("[Sink][HF] bt_sink_srv_hf_action_handler, action:%x", action);

    switch (action) {
        case BT_SINK_SRV_ACTION_PROFILE_INIT: {
            uint32_t i;

            bt_sink_srv_memset(g_sink_srv_hf_context, 0, sizeof(g_sink_srv_hf_context));

            for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
                bt_sink_srv_hf_reset_link_context(&(g_sink_srv_hf_context[i].link));
                g_sink_srv_hf_context[i].audio_src = audio_src_srv_construct_handle(AUDIO_SRC_SRV_PSEUDO_DEVICE_HFP);
                g_sink_srv_hf_context[i].audio_src->priority = AUDIO_SRC_SRV_PRIORITY_HIGH;
                g_sink_srv_hf_context[i].audio_src->play = bt_sink_srv_hf_dev_play;
                g_sink_srv_hf_context[i].audio_src->stop = bt_sink_srv_hf_dev_stop;
                g_sink_srv_hf_context[i].audio_src->suspend = bt_sink_srv_hf_dev_suspend;
                g_sink_srv_hf_context[i].audio_src->reject = bt_sink_srv_hf_dev_reject;
                g_sink_srv_hf_context[i].audio_src->exception_handle = bt_sink_srv_hf_dev_exception_handle;
            }
        }
        break;

        case BT_SINK_SRV_ACTION_PROFILE_DEINIT: {
        }
        break;

        case BT_SINK_SRV_ACTION_PROFILE_CONNECT: {
            bt_sink_srv_profile_connection_action_t *action_param = (bt_sink_srv_profile_connection_action_t *)parameters;
            if (action_param->profile_connection_mask & BT_SINK_SRV_PROFILE_HFP) {
                bt_sink_srv_hf_context_p = bt_sink_srv_cm_get_profile_info(&action_param->address, BT_SINK_SRV_PROFILE_HFP);

                //bt_sink_srv_hf_context_p->link.profile_state = BT_SINK_SRV_PROFILE_STATE_CONNECTING;
                if (NULL != bt_sink_srv_hf_context_p) {
                    bt_status_t status;
                    status = bt_hfp_connect(&bt_sink_srv_hf_context_p->link.handle, &action_param->address);
                    if (status == BT_STATUS_SUCCESS)
                    {
                        bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_CONNECTING);
                        bt_sink_srv_cm_profile_status_notify(&action_param->address, BT_SINK_SRV_PROFILE_HFP, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTING);
                    }
                }
            }
        }
        break;

        case BT_SINK_SRV_ACTION_PROFILE_DISCONNECT: {
            bt_sink_srv_profile_connection_action_t *action_param = (bt_sink_srv_profile_connection_action_t *)parameters;
            if (action_param->profile_connection_mask & BT_SINK_SRV_PROFILE_HFP) {
                bt_sink_srv_hf_context_p = bt_sink_srv_cm_get_profile_info(&action_param->address, BT_SINK_SRV_PROFILE_HFP);
                //bt_sink_srv_hf_p->state = BT_SINK_SRV_PROFILE_STATE_CONNECTING;
                if (NULL != bt_sink_srv_hf_context_p && bt_sink_srv_hf_context_p->link.handle !=0) {
                    bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_DISCONNECTING);
                    if(BT_STATUS_SUCCESS != bt_hfp_disconnect(bt_sink_srv_hf_context_p->link.handle)) {
                        bt_sink_srv_hf_dev_set_sub_state(bt_sink_srv_hf_context_p, BT_SINK_SRV_HF_DEV_SUB_STATE_NONE);
                    }
                }
            }
        }
        break;

        /* User Event */
        case BT_SINK_SRV_ACTION_ANSWER: {
            bt_sink_srv_hf_answer_call(true);
        }
        break;

        case BT_SINK_SRV_ACTION_REJECT: {
            #ifdef __BT_SINK_SRV_VOICE_PROMPT__
                bt_sink_srv_vp_dev_set_action(BT_SINK_SRV_ACTION_REJECT);
            #endif /*__BT_SINK_SRV_VOICE_PROMPT__*/
            bt_sink_srv_hf_answer_call(false);
        }
        break;

        case BT_SINK_SRV_ACTION_HANG_UP: {
            bt_sink_srv_hf_terminate_call();
        }
        break;

        case BT_SINK_SRV_ACTION_DIAL_NUMBER: {
            bt_sink_srv_hf_dial_number((char *)parameters);
        }
        break; 

        case BT_SINK_SRV_ACTION_DIAL_LAST: {
            bt_sink_srv_hf_dial_last();
        }
        break;

        case BT_SINK_SRV_ACTION_DIAL_MISSED: {
            bt_sink_srv_hf_dial_missed();
        }
        break;

        case BT_SINK_SRV_ACTION_3WAY_RELEASE_ALL_HELD: {
            bt_sink_srv_hf_release_all_held_call();
        }
        break;

        case BT_SINK_SRV_ACTION_3WAY_RELEASE_ACTIVE_ACCEPT_OTHER: {
            bt_sink_srv_hf_release_all_active_accept_others();
        }
        break;

        case BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER: {
            bt_sink_srv_hf_hold_all_active_accept_others();
        }
        break;

        case BT_SINK_SRV_ACTION_3WAY_ADD_HELD_CALL_TO_CONVERSATION: {
            bt_sink_srv_hf_add_held_to_conversation();
        }
        break;

        case BT_SINK_SRV_ACTION_3WAY_EXPLICIT_CALL_TRANSFER: {
            bt_sink_srv_hf_explicit_call_transfer();
        }
        break;

        case BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE: {
            bt_sink_srv_hf_voice_recognition_activate(*((bool *)parameters));
        }
        break;

        case BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH: {
            bt_sink_srv_hf_switch_audio_path();
        }
        break;

        case BT_SINK_SRV_ACTION_SWITCH_AUDIO_DEVICE: {
            bt_sink_srv_hf_switch_audio_device();
        }
        break;

        case BT_SINK_SRV_ACTION_3WAY_RELEASE_SPECIAL: {
            bt_sink_srv_hf_release_special(*((uint8_t *)parameters));
        }
        break;

        case BT_SINK_SRV_ACTION_3WAY_HOLD_SPECIAL: {
            bt_sink_srv_hf_hold_special(*((uint8_t *)parameters));
        }
        break;

#ifdef __BT_SINK_SRV_AM_SUPPORT__
        case BT_SINK_SRV_ACTION_CALL_VOLUME_UP: {
            if (bt_sink_srv_hf_volume_change(BT_SINK_SRV_HF_VOL_ACT_UP, false)) {
                result = BT_SINK_SRV_STATUS_EVENT_STOP;
            }
        }
        break;

        case BT_SINK_SRV_ACTION_CALL_VOLUME_DOWN: {
            if (bt_sink_srv_hf_volume_change(BT_SINK_SRV_HF_VOL_ACT_DOWN, false)) {
                result = BT_SINK_SRV_STATUS_EVENT_STOP;
            }
        }
        break;

        case BT_SINK_SRV_ACTION_CALL_VOLUME_MAX: {
            if (bt_sink_srv_hf_volume_change(BT_SINK_SRV_HF_VOL_ACT_UP, true)) {
                result = BT_SINK_SRV_STATUS_EVENT_STOP;
            }
        }
        break;

        case BT_SINK_SRV_ACTION_CALL_VOLUME_MIN: {
            if (bt_sink_srv_hf_volume_change(BT_SINK_SRV_HF_VOL_ACT_DOWN, true)) {
                result = BT_SINK_SRV_STATUS_EVENT_STOP;
            }
        }
        break;

        case BT_SINK_SRV_ACTION_QUERY_CALL_LIST: {
            bt_sink_srv_hf_query_call_list((bt_bd_addr_t *)parameters);
        }
        break;

        case BT_SINK_SRV_ACTION_DTMF: {
            bt_sink_srv_hf_send_dtmf((bt_sink_srv_send_dtmf_t *)parameters);
        }
        break;
#endif /* __BT_SINK_SRV_AM_SUPPORT__ */

        case BT_SINK_SRV_ACTION_HF_REPORT_BATTERY: {
            if (BT_SINK_SRV_STATUS_SUCCESS != bt_sink_srv_hf_apl_report_battery(*((uint8_t *)parameters))) {
                result = BT_SINK_SRV_STATUS_EVENT_STOP;
            };
        }
        break;
        
        default:
            bt_sink_srv_report("[Sink][HF] Unexcepted action:%x", action);
            break;
    }
    return result;
}

bt_status_t bt_sink_srv_hf_get_init_params(bt_hfp_init_param_t *param)
{
    // for low power test, add cmd to modify hf audio codec
    param->supported_codecs = g_sink_srv_hf_audio_codec;
    param->indicators.service = BT_HFP_INDICATOR_OFF;
    param->indicators.signal = BT_HFP_INDICATOR_OFF;
    param->indicators.roaming = BT_HFP_INDICATOR_OFF;
    param->indicators.battery = BT_HFP_INDICATOR_OFF;
    param->support_features = (bt_hfp_init_support_feature_t)(BT_HFP_INIT_SUPPORT_3_WAY | BT_HFP_INIT_SUPPORT_CODEC_NEG);

    param->disable_nrec = true;
    param->enable_call_waiting = true;
    param->enable_cli = true;
    return BT_STATUS_SUCCESS;
}
void* bt_sink_srv_cm_get_hf_info(uint8_t device_idx)
{
    return &g_sink_srv_hf_context[device_idx];
}


