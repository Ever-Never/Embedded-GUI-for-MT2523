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

#include "hal_gpio.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_audio_sync.h"
#include "bt_codec.h"
#include "hal_audio.h"
#include "bt_aws.h"
#include "hal_audio_internal_service.h"

/**** Definition ****/
#define MEDIA_PACKET_HEADER_LEN         (12)
#define A2DP_SERIAL_TIME_UPPER_BOUND    (0x10000000)

/**** Static Structure ****/
static struct {
   // MHdl
   bt_media_handle_t *mhdl;
   bt_codec_a2dp_bitstream_t silence_bs_info;

   void * last_pkt;
   // Time information
   uint32_t uRecordedTimeStamp; // For send on BTD1, the accumulated time
   uint32_t uReceiveTimeStamp;  // For send on others, the received time stamp
   bool fIsRecordedTimeStamp;

   // Audio Clock Calibration
   uint8_t sample_count_roll_time;
   bool first_set;
   uint32_t first_sample_count;      
   uint32_t last_cali_nclk;
   uint32_t last_cali_nclk_intra;
   uint64_t total_bt_clock;
} bt_a2dp_audio_sync_hdl;

static uint8_t g_a2dp_silence_buf[A2DP_SILENCE_BUF_LEN];
/**** Packet Loss Handling ****/

//--------------------------------------------
// Functionality:
//   Parse the sample count in a packet.
//--------------------------------------------
uint32_t bt_a2dp_plh_parse_sample_count(uint8_t *ptr, uint32_t uLength)
{
   bt_codec_a2dp_buffer_t raw_data;
   bt_codec_a2dp_bitstream_t sample_data;
   raw_data.buffer = ptr;
   raw_data.byte_count = uLength;
   
   bt_codec_a2dp_aws_parse_data_information(bt_a2dp_audio_sync_hdl.mhdl, &raw_data, &sample_data);

   return sample_data.sample_count;
}

void bt_a2dp_plh_get_ts_sample_counts(bt_hci_packet_t *packet, uint32_t *sample_count, uint32_t *ts)
{
    uint8_t *media_header = (uint8_t *)packet + packet->offset;
    uint32_t payload_len = packet->packet_length - (packet->offset+MEDIA_PACKET_HEADER_LEN);

    *ts = (media_header[4] << 24) | (media_header[5] << 16) | (media_header[6] << 8) | (media_header[7]);
    *sample_count = bt_a2dp_plh_parse_sample_count(media_header+MEDIA_PACKET_HEADER_LEN, payload_len);
}

void bt_a2dp_clear_gpio_12(void)
{
    uint32_t mode = 0;
    hal_gpio_init(HAL_GPIO_12);
    hal_pinmux_set_function(HAL_GPIO_12, mode);
    hal_gpio_set_direction(HAL_GPIO_12, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_LOW);
}

void bt_a2dp_clear_gpio(hal_gpio_pin_t gpio_num)
{
    uint32_t mode = 0;
    hal_gpio_init(gpio_num);
    hal_pinmux_set_function(gpio_num, mode);
    hal_gpio_set_direction(gpio_num, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(gpio_num, HAL_GPIO_DATA_LOW);
}

/**** Public API           ****/

//--------------------------------------------
// Functionality:
//   After A2DP codec opening, we reset variables
//--------------------------------------------

void bt_a2dp_plh_init(bt_media_handle_t *handle)
{
   // Reset variable
   bt_a2dp_clear_gpio_12();
   bt_a2dp_audio_sync_hdl.uRecordedTimeStamp = 0;
   bt_a2dp_audio_sync_hdl.uReceiveTimeStamp = 0;
   bt_a2dp_audio_sync_hdl.fIsRecordedTimeStamp = false;

   bt_a2dp_audio_sync_hdl.mhdl = handle;
   bt_codec_a2dp_aws_get_silence_frame_information(handle, &bt_a2dp_audio_sync_hdl.silence_bs_info);
}

void bt_a2dp_plh_deinit(void)
{
    bt_a2dp_clear_gpio_12();
    memset((void*)&bt_a2dp_audio_sync_hdl, 0, sizeof(bt_a2dp_audio_sync_hdl));
}

//--------------------------------------------
// Functionality:
//   Check timestamp, and return the result:
//   (A) whether the packet should be dropped.
//   (B) How many silence frame should be added.
//--------------------------------------------
void bt_a2dp_plh_check_packet(bt_hci_packet_t *packet, uint32_t *retSilenceNum, bool *retDrop)
{
    return;
    /****

    uint32_t uDiffTime = 0;
    //uint32_t uFrameTime;
    uint32_t uTimeA;
    uint32_t uTimeB;
    uint32_t ts = 0;
    uint32_t sample_count = 0;
    *retSilenceNum = 0;
    *retDrop = false;

    bt_sink_srv_assert(packet != bt_a2dp_audio_sync_hdl.last_pkt);
    bt_a2dp_audio_sync_hdl.last_pkt = (void *)packet;

    bt_a2dp_plh_get_ts_sample_counts(packet, &sample_count, &ts);


    bt_a2dp_audio_sync_hdl.uReceiveTimeStamp = sample_count;

    if (!bt_a2dp_audio_sync_hdl.fIsRecordedTimeStamp) {
        bt_a2dp_audio_sync_hdl.uRecordedTimeStamp = ts;
        bt_a2dp_audio_sync_hdl.fIsRecordedTimeStamp = true;
        return;
    }

    if (ts < bt_a2dp_audio_sync_hdl.uRecordedTimeStamp) {
        uTimeA = bt_a2dp_audio_sync_hdl.uRecordedTimeStamp - ts;
        uTimeB = A2DP_SERIAL_TIME_UPPER_BOUND - uTimeA;
        if (uTimeA < uTimeB) {
            *retDrop = true;
        } else {
            uDiffTime = uTimeB;
        }
    }
    else if (ts > bt_a2dp_audio_sync_hdl.uRecordedTimeStamp) {
        uTimeA = ts - bt_a2dp_audio_sync_hdl.uRecordedTimeStamp;
        uTimeB = A2DP_SERIAL_TIME_UPPER_BOUND - uTimeA;
        if (uTimeA < uTimeB) {
            uDiffTime = uTimeA;
        } else {
            *retDrop = true;
        }
    }

    if (uDiffTime) {
        *retSilenceNum = uDiffTime / bt_a2dp_audio_sync_hdl.silence_bs_info.sample_count/ bt_a2dp_audio_sync_hdl.silence_bs_info.byte_count;
    }

    return;
    **/
}

void bt_mp3_plh_init(bt_media_handle_t *handle)
{
   // Reset variable
#if 0
   bt_a2dp_audio_sync_hdl.uRecordedTimeStamp = 0;
   bt_a2dp_audio_sync_hdl.uReceiveTimeStamp = 0;
   bt_a2dp_audio_sync_hdl.fIsRecordedTimeStamp = false;

   bt_a2dp_audio_sync_hdl.mhdl = handle;
   bt_codec_a2dp_aws_get_silence_frame_information(handle, &bt_a2dp_audio_sync_hdl.silence_bs_info);
#endif
}

void bt_mp3_plh_deinit(void)
{
#if 0
    memset((void*)&bt_a2dp_audio_sync_hdl, 0, sizeof(bt_a2dp_audio_sync_hdl));
#endif
}


//--------------------------------------------
// Functionality:
//   Query the pattern of silence frame.
//--------------------------------------------
uint16_t bt_sink_srv_fill_silence_frame(uint8_t **ptr, uint32_t numFrame)
{
   //uint32_t uFrameLength, uFrameTime;
   bt_codec_a2dp_buffer_t silence_buf;

   silence_buf.buffer = g_a2dp_silence_buf;   
   bt_codec_a2dp_aws_fill_silence_frame(bt_a2dp_audio_sync_hdl.mhdl, &silence_buf, numFrame);

   bt_a2dp_audio_sync_hdl.uRecordedTimeStamp += (bt_a2dp_audio_sync_hdl.silence_bs_info.sample_count * numFrame);
   
   *ptr = (uint8_t*)&g_a2dp_silence_buf;
   return silence_buf.byte_count;
}


void bt_a2dp_plh_write_frame_done(bt_hci_packet_t *packet )
{
    return;
    //bt_sink_srv_assert(packet == bt_a2dp_audio_sync_hdl.last_pkt);
    //bt_a2dp_audio_sync_hdl.uRecordedTimeStamp += bt_a2dp_audio_sync_hdl.uReceiveTimeStamp;
}


bt_status_t bt_sink_srv_audio_clock_init(void)
{
    //uint32_t curr_bt_nclk;
    //uint32_t curr_bt_nclk_intra;
    bt_a2dp_audio_sync_hdl.first_set = false;

    return BT_STATUS_SUCCESS;
}


uint32_t bt_sink_srv_get_bt_clock_gap(uint32_t ori_bt_nclk, uint32_t ori_bt_nclk_intra, uint32_t curr_bt_nclk, uint32_t curr_bt_nclk_intra)
{
    uint32_t clock_gap;
    if ((curr_bt_nclk > ori_bt_nclk) || ((curr_bt_nclk == ori_bt_nclk) && (curr_bt_nclk_intra > ori_bt_nclk_intra)))
    {
        /*the bt clock didn't flip*/
        if (curr_bt_nclk == ori_bt_nclk)
        {
            clock_gap = ((curr_bt_nclk_intra - ori_bt_nclk_intra) << 1);
        }
        else
        {
            clock_gap = (uint32_t)(curr_bt_nclk - ori_bt_nclk - 1) * 625 + ((curr_bt_nclk_intra<<1) + 625 - (ori_bt_nclk_intra<<1));
        }
    }
    else
    {
        /*the bt clock didn't flip*/
        if (curr_bt_nclk == ori_bt_nclk)
        {
            clock_gap = ((ori_bt_nclk_intra - curr_bt_nclk_intra) << 1);
        }
        else
        {
            clock_gap = (uint32_t)(ori_bt_nclk - curr_bt_nclk - 1) * 625 + ((ori_bt_nclk_intra<<1) + 625 - (curr_bt_nclk_intra<<1));
        }
    }

    return clock_gap;
}


uint32_t bt_sink_srv_get_sample_count_gap(uint32_t ori_count, uint32_t curr_count)
{
    uint32_t count_gap;
    if (ori_count < curr_count)
    {
        count_gap = curr_count - ori_count;
    }
    else
    {
        count_gap = curr_count + (0xFFFFFFFF - ori_count);
    }

    return count_gap;
}
#ifdef __BT_AWS_SUPPORT__
bt_status_t bt_sink_srv_audio_clock_calibrate(
    bt_media_handle_t *media_handle, 
    bt_aws_codec_type_t type,
    uint32_t sampling_rate)
{
    uint32_t curr_bt_nclk;
    uint32_t curr_bt_nclk_intra;
    //uint32_t sampling_rate_value;
    uint32_t bt_clock_gap = 0;
    //uint32_t audio_sample_gap = 0;
    bt_codec_aws_clock_skew_status_t skew_status;
    uint32_t audio_sample_count;
    uint64_t total_sample_count = 0;
    int32_t sample_count_gap;
    bt_status_t status;
    
    if (type == BT_AWS_CODEC_TYPE_MP3) {
    } else {
        status = bt_codec_a2dp_aws_get_clock_skew_status(media_handle, &skew_status);
        //bt_sink_srv_report("audio_sync, skew_status:%d", status);
        if (status != BT_STATUS_SUCCESS || skew_status != BT_CODEC_AWS_CLOCK_SKEW_STATUS_IDLE)
        {
            return BT_STATUS_FAIL;
        }
    }
    __disable_irq();    
    bt_aws_get_bt_local_time(&curr_bt_nclk, &curr_bt_nclk_intra);
    audio_dsp_update_audio_counter(0);
    status = hal_audio_get_audio_clock(&audio_sample_count);    
    __enable_irq();
    //bt_sink_srv_report("audio_sync, local_clock, nclk:%d, nclk_intra:%d", curr_bt_nclk, curr_bt_nclk_intra);
    //bt_sink_srv_report("audio_sync, clock_status:%d, sample_cout:%d", status, audio_sample_count);
    if (status != BT_STATUS_SUCCESS)
    {
        return status;
    }
    if (!bt_a2dp_audio_sync_hdl.first_set)
    {
        bt_a2dp_audio_sync_hdl.sample_count_roll_time = 0;
        bt_a2dp_audio_sync_hdl.first_sample_count = audio_sample_count;
        bt_a2dp_audio_sync_hdl.first_set = true;

        bt_a2dp_audio_sync_hdl.last_cali_nclk = curr_bt_nclk;
        bt_a2dp_audio_sync_hdl.last_cali_nclk_intra = curr_bt_nclk_intra;
        bt_a2dp_audio_sync_hdl.total_bt_clock = 0;
        return BT_STATUS_SUCCESS;
    }

    if (audio_sample_count > bt_a2dp_audio_sync_hdl.first_sample_count)
    {
        total_sample_count = (audio_sample_count - bt_a2dp_audio_sync_hdl.first_sample_count);
        if (bt_a2dp_audio_sync_hdl.sample_count_roll_time > 0)
        {
            total_sample_count += ((uint64_t)bt_a2dp_audio_sync_hdl.sample_count_roll_time * (uint64_t)0xFFFFFFF);
        }
    }
    else
    {
        total_sample_count = (bt_a2dp_audio_sync_hdl.first_sample_count - audio_sample_count);
        bt_a2dp_audio_sync_hdl.sample_count_roll_time++;        
        total_sample_count = (((uint64_t)bt_a2dp_audio_sync_hdl.sample_count_roll_time * (uint64_t)0xFFFFFFF) - total_sample_count);
    }

    bt_clock_gap = bt_sink_srv_get_bt_clock_gap(
        bt_a2dp_audio_sync_hdl.last_cali_nclk,
        bt_a2dp_audio_sync_hdl.last_cali_nclk_intra,
        curr_bt_nclk,
        curr_bt_nclk_intra); 
    bt_a2dp_audio_sync_hdl.total_bt_clock += bt_clock_gap;
    //bt_sink_srv_report("audio_sync,bt_clock_gap:0x%08X", bt_clock_gap);

    bt_a2dp_audio_sync_hdl.last_cali_nclk = curr_bt_nclk;
    bt_a2dp_audio_sync_hdl.last_cali_nclk_intra = curr_bt_nclk_intra;
    
    uint64_t bt_sample = (bt_a2dp_audio_sync_hdl.total_bt_clock * (uint64_t)sampling_rate)/1000000/2;
    sample_count_gap = (int32_t)(bt_sample - total_sample_count);
    //bt_sink_srv_report("audio_sync, clock_skew:%d", sample_count_gap);
    if (sample_count_gap > 40 || sample_count_gap < -40)
    {
        if (type == BT_AWS_CODEC_TYPE_MP3) {
        } else {
            status = bt_codec_a2dp_aws_set_clock_skew_compensation_value(media_handle, sample_count_gap);
        }
    }

    return status;
}
#endif
