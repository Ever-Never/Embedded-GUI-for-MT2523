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

#ifndef __BT_SINK_SRV_AUDIO_SYNC_H__
#define __BT_SINK_SRV_AUDIO_SYNC_H__

#include <stdint.h>
#include <stdbool.h>
#include "bt_a2dp.h"
#include "bt_codec.h"
#include "bt_aws.h"
#include "bt_sink_srv_aws.h"
#include "bt_sink_srv_media_mgr.h"
#include "hal_audio.h"
/**** Definition ****/
#define A2DP_SILENCE_BUF_LEN 516

//#define __MEDIA_DUMP_DEBUG__ (1)

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __BT_AWS_SUPPORT__

typedef struct {
    bt_media_handle_t *media_handle;
    bt_aws_codec_type_t type;
    uint32_t sampling_rate;
} bt_sink_srv_audio_sync_calc_t;

#endif

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

bt_status_t bt_sink_srv_audio_clock_init(bt_aws_clock_offset_t *clk_offset);
#ifdef __BT_AWS_SUPPORT__
bt_status_t bt_sink_srv_audio_clock_calibrate(
    bt_sink_srv_audio_sync_calc_t *audio_calc);
#endif
#define __AWS_PHASE_II__
/**
 * @brief This structure defines multi-source state.
 */
typedef struct _bt_clock_struct {
    uint32_t    clock;          // 312.5 us per uinit
    uint32_t    clock_intra;    // 1 us us per uint
} bt_clock_struct_t;

typedef struct _sink_srv_clock_mapping_t {
    bt_aws_clock_t piconet_clk;
    bt_aws_clock_t local_clk;
    uint32_t time_stamp;            // the timestamp from media packet which should be play after initial sync.
    uint16_t seq_idx;
} sink_srv_clock_mapping_t;

typedef struct _sink_media_packet_info_t {
    uint16_t seq_num;
    uint32_t time_stamp;            // the timestamp from media packet which should be play after initial sync.
    uint32_t sample_counts;
} sink_media_packet_info_t;

/**
 * @brief This structure defines multi-source transient state.
 */
typedef struct _sink_srv_media_play_info {
    uint32_t sample_rate;               // the sample rate for media codec.
    uint32_t sample_count_per_frame;    // the sample count per frame.
    uint32_t silence_byte_per_frame;      // how many silence bytes each frame contains.
    uint32_t count_per_sample;          // the unit per sample.
    uint32_t ts_diff_by_packet;
    uint16_t aac_codec;
    uint16_t sequence_idx;
} sink_srv_media_play_info_t;

typedef uint32_t sink_srv_plh_type_t;
#define SINK_SRV_PLH_TYPE_CONTINUOUS   (0)
#define SINK_SRV_PLH_TYPE_LACK         (1)
#define SINK_SRV_PLH_TYPE_DROP         (2)

void sink_srv_init_media_play_info(sink_srv_media_play_info_t * media_info, bt_a2dp_codec_capability_t* codec, 
                                                        bt_media_handle_t *handle);

uint32_t sink_srv_util_get_a2dp_time_stamp(bt_hci_packet_t *packet);


uint16_t sink_srv_util_get_a2dp_sn(bt_hci_packet_t *packet);

float sinv_srv_util_sample_2_us_count(uint32_t sample_count, uint32_t sample_rate);

uint32_t sinv_srv_util_us_2_sample_count(uint32_t us_count, uint32_t sample_rate);

uint32_t sink_srv_util_get_packet_sample_counts(bt_media_handle_t *handle, bt_hci_packet_t *packet);

// ---|-------------------cur-----------------x----------------
//   (bt0, ts0)          (bt1)              (ts2)
// duration = dur(ts2-ts0) - dur(bt1-bt0)
// must get the current piconect bt clock & bt local time before this API is called.
void sink_srv_calc_target_clock(sink_srv_media_play_info_t * media_info, sink_srv_clock_mapping_t *base, 
                                sink_srv_clock_mapping_t *target);

uint32_t sink_srv_get_play_duration(sink_srv_media_play_info_t * media_info, sink_srv_clock_mapping_t *base, 
                                bt_aws_clock_t *cur_bt_clk, uint32_t time_stamp);

uint32_t sink_srv_a2dp_check_media_continuous(bt_media_handle_t *handle, sink_srv_media_play_info_t *media_info, 
                                        sink_media_packet_info_t *last_info, bt_hci_packet_t *next_packet);

void * sink_srv_a2dp_fill_silence_frame(bt_media_handle_t *handle, sink_srv_media_play_info_t *media_info,
                                        sink_media_packet_info_t *last_info, bt_hci_packet_t *next_packet, bt_a2dp_codec_type_t type);

#ifdef __MEDIA_DUMP_DEBUG__
void sink_srv_dump_dsp_data(uint8_t *buff, uint32_t len);
#endif

#ifdef __cplusplus
}
#endif

#endif // __BT_SINK_SRV_AUDIO_SYNC_H__
