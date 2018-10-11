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

#ifndef __BT_SINK_SRV_AUDIO_SETTING_H__
#define __BT_SINK_SRV_AUDIO_SETTING_H__


#ifndef MTK_NO_PSRAM_ENABLE
#define __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
#endif

#include "stdint.h"
#include "audio_coefficient.h"


#define BT_SINK_SRV_AUDIO_SETTING_FALG_GEN_VOL_READY                    (0x0001)


/* EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE */
#if PRODUCT_VERSION == 2533
#define BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV                   (16)
#else
#define BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV                   (16)
#endif

/* EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE */
#define BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV                    (1)


typedef uint16_t device_t;

typedef enum {
    VOL_A2DP = 0,
    VOL_HFP,
    VOL_PCM,
    VOL_MP3,

    VOL_DEF, /* stream out only */
    VOL_TOTAL
} vol_type_t;

typedef struct {
    uint32_t digital;
    uint32_t analog;
} vol_t;

typedef struct {
    device_t dev;
    uint8_t lev;
} a2dp_vol_info_t;

typedef struct {
    vol_t vol;
} a2dp_vol_t;

typedef struct {
    uint8_t codec;
    device_t dev_in;
    uint8_t lev_in;
    device_t dev_out;
    uint8_t lev_out;
} hfp_vol_info_t;

typedef struct {
    vol_t vol_in;
    vol_t vol_out;
} hfp_vol_t;

typedef struct {
    device_t dev;
    uint8_t lev;
} pcm_vol_info_t;

typedef struct {
    vol_t vol;
} pcm_vol_t;

typedef struct {
    device_t dev;
    uint8_t lev;
} mp3_vol_info_t;

typedef struct {
    vol_t vol;
} mp3_vol_t;

typedef struct {
    device_t dev;
    uint8_t lev;
} def_vol_info_t;

typedef struct {
    vol_t vol;
} def_vol_t;

typedef struct {
    vol_type_t type;
    union {
        a2dp_vol_info_t a2dp_vol_info;
        hfp_vol_info_t hfp_vol_info;
        pcm_vol_info_t pcm_vol_info;
        mp3_vol_info_t mp3_vol_info;
        def_vol_info_t def_vol_info;
    } vol_info;
} bt_sink_srv_audio_setting_vol_info_t;

typedef struct {
    vol_type_t type;
    union {
        a2dp_vol_t a2dp_vol;
        hfp_vol_t hfp_vol;
        pcm_vol_t pcm_vol;
        mp3_vol_t mp3_vol;
        def_vol_t def_vol;
    } vol;
} bt_sink_srv_audio_setting_vol_t;

typedef struct {
    uint32_t flag;
} bt_sink_srv_audio_setting_context_t;

void bt_sink_srv_audio_setting_init(void);

int32_t bt_sink_srv_audio_setting_get_vol(bt_sink_srv_audio_setting_vol_info_t *vol_info,
            bt_sink_srv_audio_setting_vol_t *vol);

void bt_sink_srv_audio_setting_update_voice_fillter_setting(bt_sink_srv_audio_setting_vol_info_t *vol_info,
        const audio_eaps_t *am_speech_eaps);

#endif /* __BT_SINK_SRV_AUDIO_SETTING_H__ */

