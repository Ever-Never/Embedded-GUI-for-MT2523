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

#ifndef __BT_SINK_SRV_AUDIO_TUNNING_H__
#define __BT_SINK_SRV_AUDIO_TUNNING_H__

#ifdef MTK_AUDIO_TUNING_ENABLED
#define __BT_SINK_SRV_AUDIO_TUNING__
#endif /* MTK_AUDIO_TUNING_ENABLED */

#ifdef MTK_AUDIO_TUNING_ENABLED
#define __BT_SINK_AUDIO_TUNING__
#endif /* MTK_AUDIO_TUNING_ENABLED */

#ifdef __BT_SINK_AUDIO_TUNING__
#include "atci.h"
#endif

#ifdef __BT_SINK_SRV_AUDIO_TUNING__

#include "stdint.h"
#include "hal_audio.h"

#define TUNNING_FLAG_INIT       (0x0001)
#define TUNNING_FLAG_ON         (0x0100)

#define TUNNING_MASK_VOL        (0x0010)
#define TUNNING_MASK_DEV        (0x1000)

#define TUNNING_DEV_IN          (0x01)
#define TUNNING_DEV_OUT         (0x02)


#define TUNNING_SCENARIO_A2DP       (0)
#define TUNNING_SCENARIO_HF         (1)

#define TUNNING_TYPE_VOL            (0)
#define TUNNING_TYPE_DEV            (1)

typedef uint16_t audio_device_t;

typedef struct {
    uint8_t level;
    uint32_t digital;
    uint32_t analog;
} audio_volume_t;

typedef struct {
    uint32_t mask;
    audio_device_t dev_in;
    audio_volume_t vol_in;
    audio_device_t dev_out;
    audio_volume_t vol_out;
} hf_tunning_info_t;

typedef struct {
    uint32_t mask;
    audio_device_t dev_out;
    audio_volume_t vol_out;
} a2dp_tunning_info_t;

typedef struct {
    uint32_t flag;
    hf_tunning_info_t hf;
    a2dp_tunning_info_t a2dp;
} bt_sink_srv_audio_tunning_context_t;


bt_sink_srv_audio_tunning_context_t *bt_sink_srv_audio_tunning_get_context(void);

void bt_sink_srv_audio_tunning_init(void);

void bt_sink_srv_audio_tunning_atci_init(void);

atci_status_t bt_sink_srv_audio_tuning_atci_handler(atci_parse_cmd_param_t *parse_cmd);

void bt_sink_srv_audio_tunning_update_callback(void *data);

void tunning_stream_out_db_transfer(uint16_t gain, uint32_t *digital, uint32_t *analog);

void tunning_voice_uplink_db_transfer(uint16_t gain, uint32_t *digital, uint32_t *analog);

void bt_sink_srv_audio_tunning_update(uint8_t scen, uint8_t type);

int16_t bt_sink_srv_audio_tuning_command_entry(char *string);

hal_audio_device_t bt_sink_srv_get_tunning_dev(int32_t index, uint8_t type);

#endif /* __BT_SINK_SRV_AUDIO_TUNING__ */

#ifdef __BT_SINK_AUDIO_TUNING__

typedef void (*bt_sink_srv_tunning_func)(void);

atci_status_t bt_sink_srv_audio_tuning_atci_handler(atci_parse_cmd_param_t *parse_cmd);

void bt_sink_hf_audio_tuning_atci_init(void);

void bt_sink_srv_set_tunning_func(bt_sink_srv_tunning_func tunning_on,
                                  bt_sink_srv_tunning_func tunning_off);

#endif /* __BT_SINK_AUDIO_TUNING__ */

#endif /* __BT_SINK_SRV_AUDIO_TUNNING_H__ */

