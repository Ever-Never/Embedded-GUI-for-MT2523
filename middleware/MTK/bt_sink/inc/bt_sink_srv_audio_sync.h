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

#include <stdint.h>
#include <stdbool.h>
#include "bt_a2dp.h"
#include "bt_codec.h"
#include "bt_sink_srv_am_med_mgr.h"
#include "bt_aws.h"
#include "hal_audio.h"
#ifndef __BT_SINK_SRV_AUDIO_SYNC_H__
#define __BT_SINK_SRV_AUDIO_SYNC_H__
/**** Definition ****/
#define A2DP_SILENCE_BUF_LEN 516

//--------------------------------------------
// Functionality:
//   Parse the sample count in a packet.
//--------------------------------------------

void bt_aws_clear_gpio(hal_gpio_pin_t gpio_num);

/**** Public API           ****/
// Call this API after set flag.
void bt_a2dp_plh_init(bt_media_handle_t *handle);

// // Call this API after clear flag.
void bt_a2dp_plh_deinit(void);

// Call this API after set flag.
void bt_mp3_plh_init(bt_media_handle_t *handle);

// // Call this API after clear flag.
void bt_mp3_plh_deinit(void);

void bt_a2dp_plh_write_frame_done(bt_hci_packet_t *packet );

void bt_a2dp_plh_check_packet(bt_hci_packet_t *packet, uint32_t *retSilenceNum, bool *retDrop);

uint16_t bt_sink_srv_fill_silence_frame(uint8_t **ptr, uint32_t numFrame);

bt_status_t bt_sink_srv_audio_clock_init(void);
#ifdef __BT_AWS_SUPPORT__
bt_status_t bt_sink_srv_audio_clock_calibrate(
    bt_media_handle_t *media_handle,
    bt_aws_codec_type_t type,
    uint32_t sampling_rate);
#endif
#endif // __BT_SINK_SRV_AUDIO_SYNC_H__
