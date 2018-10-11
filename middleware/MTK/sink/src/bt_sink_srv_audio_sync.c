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
#include "bt_sink_srv_aws.h"
#include "bt_sink_srv_audio_sync.h"
#include "bt_codec.h"
#include "hal_audio.h"
#include "bt_aws.h"
#include "bt_sink_srv_music.h"

/**** Definition ****/
#define MEDIA_PACKET_HEADER_LEN         (12)
#define A2DP_SERIAL_TIME_UPPER_BOUND    (0x10000000)

#define AAC_PAYLOAD_HEADER_LEN (9)
#define AAC_ADTS_HEADER_LEN (7)
#define AAC_PAYLOAD_SILENCE_HEADER (AAC_PAYLOAD_HEADER_LEN + 1)

static const uint8_t s_aac_payload_header[AAC_PAYLOAD_HEADER_LEN] = {0x47, 0xFC, 0x00, 0x00, 0xB0, 0x90, 0x80, 0x03, 0x00};
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
#ifdef __MEDIA_DUMP_DEBUG__
#define DUMP_BUFFER_SIZE (4096)
static uint8_t g_a2dp_dump_buf[DUMP_BUFFER_SIZE];
#endif
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
    *retSilenceNum = 0;
    *retDrop = false;
    return;
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

typedef struct _clock_skew_context{
    bt_aws_clock_offset_t last_offset;
    int32_t intra_inner_product_twice;
} clock_skew_context_t;

clock_skew_context_t g_clk_skew_cntx;
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
}


bt_status_t bt_sink_srv_audio_clock_init(bt_aws_clock_offset_t *clk_offset)
{
    //uint32_t curr_bt_nclk;
    //uint32_t curr_bt_nclk_intra;
    bt_a2dp_audio_sync_hdl.first_set = true;
    if (clk_offset) {
        g_clk_skew_cntx.last_offset.offset = clk_offset->offset;
        g_clk_skew_cntx.last_offset.offset_intra = clk_offset->offset_intra;
        g_clk_skew_cntx.intra_inner_product_twice = 0;
    }
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

int32_t bt_clock_us_diff_twice(bt_aws_clock_offset_t *dst, bt_aws_clock_offset_t *src)
{
    uint32_t gap_nclk = (dst->offset - src->offset) & 0x0FFFFFFF;
    uint32_t gap_nclk_intra = dst->offset_intra - src->offset_intra;
    if (gap_nclk & 0x08000000) {
        gap_nclk |= 0xF0000000;
    }

    return (int32_t)(gap_nclk * 625 + (gap_nclk_intra << 1));
}

void bt_sink_srv_fetch_bt_offset()
{
    bt_sink_srv_music_device_t *aws_dev = NULL;
    
    aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
    if (aws_dev == NULL) {
        bt_sink_srv_report("audio_sync,no aws dev, return fail");
        return;
    }

    if (aws_dev->gap_role == BT_ROLE_SLAVE)
    {
        bt_aws_fetch_bt_time_offset(aws_dev->aws_hd);
    }
}

#if 1
int32_t bt_sink_srv_audio_clock_get_skew(bt_sink_srv_audio_sync_calc_t *audio_calc)
{
    int32_t bt_sample = 0;
    bt_sink_srv_music_device_t *aws_dev = NULL;
    int32_t twice_us_count = 0;

    if (audio_calc->type == BT_AWS_CODEC_TYPE_MP3) {
        aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    } else {
        aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
    }
    if (aws_dev == NULL) {
        bt_sink_srv_report("audio_sync,no aws dev, return fail");
        return 0;
    }

    twice_us_count = bt_clock_us_diff_twice(&aws_dev->offset, &g_clk_skew_cntx.last_offset);
    {
        int64_t iner_product = (int64_t)twice_us_count * audio_calc->sampling_rate + (int64_t)g_clk_skew_cntx.intra_inner_product_twice;
        bt_sample = (uint32_t)(iner_product/1000000/2);
        g_clk_skew_cntx.intra_inner_product_twice = iner_product % 2000000;
        g_clk_skew_cntx.last_offset.offset = aws_dev->offset.offset;
        g_clk_skew_cntx.last_offset.offset_intra= aws_dev->offset.offset_intra;
    }

    return bt_sample;
}
#else
int32_t bt_sink_srv_audio_clock_get_skew(bt_sink_srv_audio_sync_calc_t *audio_calc)
{
    uint32_t bt_clock_gap = 0;
    bt_codec_aws_clock_skew_status_t skew_status;
    uint32_t audio_sample_count;
    uint64_t total_sample_count = 0;
    int32_t sample_count_gap;
    bt_status_t status;
    bt_aws_clock_t curr_clock;
  
    bt_sink_srv_music_device_t *aws_dev = NULL;
    if (audio_calc->type == BT_AWS_CODEC_TYPE_MP3) {
        aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    } else {
        aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
    }
    if (aws_dev == NULL) {
        bt_sink_srv_report("audio_sync,no aws dev, return fail");
        return 0;
    }

    
    if (audio_calc->type == BT_AWS_CODEC_TYPE_MP3) {
        status = aws_dev->mp3->med_hd.media_handle.mp3.aws_get_clock_skew_status((int32_t *)&skew_status);
    } else {
        status = bt_codec_a2dp_aws_get_clock_skew_status(audio_calc->media_handle, &skew_status);
    }
    if (status != BT_STATUS_SUCCESS || skew_status != BT_CODEC_AWS_CLOCK_SKEW_STATUS_IDLE)
    {
        return 0;
    }
    
     __disable_irq();
    bt_aws_get_curr_bt_time(aws_dev->gap_role,aws_dev->offset, &curr_clock);
    audio_dsp_update_audio_counter(0);
    status = hal_audio_get_audio_clock(&audio_sample_count);    
    __enable_irq();
    bt_sink_srv_report("audio_sync, local_clock, role:%d, nclk:%d, nclk_intra:%d", aws_dev->gap_role, curr_clock.nclk, curr_clock.nclk_intra);
    bt_sink_srv_report("audio_sync, clock_status:%d, sample_cout:%d", status, audio_sample_count);
    if (status != BT_STATUS_SUCCESS)
    {
        return 0;
    }
    if (!bt_a2dp_audio_sync_hdl.first_set)
    {
        bt_a2dp_audio_sync_hdl.sample_count_roll_time = 0;
        bt_a2dp_audio_sync_hdl.first_sample_count = audio_sample_count;
        bt_a2dp_audio_sync_hdl.first_set = true;

        bt_a2dp_audio_sync_hdl.last_cali_nclk = curr_clock.nclk;
        bt_a2dp_audio_sync_hdl.last_cali_nclk_intra = curr_clock.nclk_intra;
        bt_a2dp_audio_sync_hdl.total_bt_clock = 0;
        return 0;
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
        curr_clock.nclk,
        curr_clock.nclk_intra); 
    bt_a2dp_audio_sync_hdl.total_bt_clock += bt_clock_gap;

    bt_a2dp_audio_sync_hdl.last_cali_nclk =  curr_clock.nclk;
    bt_a2dp_audio_sync_hdl.last_cali_nclk_intra = curr_clock.nclk_intra;
    
    uint64_t bt_sample = (bt_a2dp_audio_sync_hdl.total_bt_clock * (uint64_t)audio_calc->sampling_rate)/1000000/2;
    sample_count_gap = (int32_t)(total_sample_count - bt_sample);

    return sample_count_gap;
}
#endif


bt_status_t bt_sink_srv_audio_clock_calibrate(
    bt_sink_srv_audio_sync_calc_t *audio_calc)
{
    int32_t skew_count;
    bt_status_t status = BT_STATUS_FAIL;
    bt_sink_srv_music_device_t *aws_dev = NULL;

    skew_count = bt_sink_srv_audio_clock_get_skew(audio_calc);
    bt_sink_srv_report("bt_sink_srv_audio_clock_calibrate,clock_skew:%d", skew_count);
    if (skew_count != 0)
    {
        if (audio_calc->type == BT_AWS_CODEC_TYPE_MP3) {
            aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
            if (aws_dev != NULL) {
                status = aws_dev->mp3->med_hd.media_handle.mp3.aws_set_clock_skew_compensation_value(0-skew_count);
            }
        } else {
            status = bt_codec_a2dp_aws_set_clock_skew_compensation_value(audio_calc->media_handle, 0-skew_count);
        }
        bt_sink_srv_report("set_compensation, status:%d, clock_skew:%d", status, skew_count);
    }

    return status;
}
#endif

#define __AWS_PHASE_II_IMPL__

#define BT_MAIN_COLOK_SCOPE  (0x0FFFFFFF)
#define BT_FRAC_CLOCK_UPPER (312)
// According to A2DP spec, the max number of sbc frames in a packet should be no more than 15.
#define MAX_SBC_FRAME_NUM_PER_PACKET    (15)

static uint32_t sink_srv_util_sample_index_2_freq(bt_a2dp_codec_type_t type, uint32_t sample_index);
static void sink_srv_util_update_silience_a2dp_header(bt_hci_packet_t *pkt, uint16_t sn, uint32_t ts);

static uint32_t sink_srv_util_sample_index_2_freq(bt_a2dp_codec_type_t type, uint32_t sample_index)
{
    const uint32_t freq_sbc_arr[4] = {0x3E8008, 0x7D0004, 0xAC4402, 0xBB8001};
    const uint32_t freq_aac_arr[12] = {0x1F40080, 0x2B11040, 0x2EE0020, 0x3E80010, 0x5622008, 0x5DC0004, 0x7D00002, 
                                        0xAC44001, 0xBB80800, 0xFA00400, 0x15888200, 0x17700100};
    uint32_t idx = 0;
    uint32_t freq = 0xAC44;
    if (type == BT_A2DP_CODEC_SBC) {
        freq = 0x3E80;
        while (idx < 4) {
            if ((freq_sbc_arr[idx] & sample_index) != 0) {
                freq = freq_sbc_arr[idx] >> 8;
                break;
            }
            ++idx;
        }
    } else if (type == BT_A2DP_CODEC_AAC) {
        freq = 0xAC44;
        while (idx < 12) {
            if ((freq_aac_arr[idx] & sample_index) != 0) {
                freq = freq_aac_arr[idx] >> 12;
                break;
            }
            ++idx;
        }
    } 
    bt_sink_srv_report("index to freq, type: %d, index: %d, freq: %d", type, sample_index, freq);

    return freq;
}

static void sink_srv_util_update_silience_a2dp_header(bt_hci_packet_t *pkt, uint16_t sn, uint32_t ts)
{
    uint8_t *media_header = (uint8_t *)pkt + pkt->offset;

    media_header[0] = 0x80;
    media_header[1] = 0x60;

    media_header[2] = (sn >> 8);
    media_header[3] = (sn & 0xFF);

    media_header[4] = (ts >> 24);
    media_header[5] = ((ts >> 16) & 0xFF);
    media_header[6] = ((ts >> 8) & 0xFF);
    media_header[7] = (ts & 0xFF);
}

void sink_srv_init_media_play_info(sink_srv_media_play_info_t * media_info, bt_a2dp_codec_capability_t* codec, 
                                                        bt_media_handle_t *handle)
{
    bt_codec_a2dp_bitstream_t silence_bs_info;
    bt_sink_srv_assert((NULL != media_info && NULL != codec && NULL != handle) && "play media info NULL");

    switch (codec->type) {
        case BT_A2DP_CODEC_SBC:
        {
            bt_a2dp_sbc_codec_t * sbc = &codec->codec.sbc;
            media_info->sample_rate = sink_srv_util_sample_index_2_freq(BT_A2DP_CODEC_SBC, sbc->sample_freq);
            media_info->count_per_sample = 1;
            media_info->aac_codec = 0;
            media_info->sequence_idx = 0;
            break;
        }
        case BT_A2DP_CODEC_AAC:
        {
            bt_a2dp_aac_codec_t * aac = &codec->codec.aac;
            uint32_t freq_idx = (aac->freq_h | (aac->freq_l << 8));
            media_info->sample_count_per_frame = 1024;
            media_info->sample_rate = sink_srv_util_sample_index_2_freq(BT_A2DP_CODEC_AAC, freq_idx);
            media_info->count_per_sample = 4;
            media_info->aac_codec = 1;
            media_info->sequence_idx = 0;
            break;

        }  
    }
#ifdef __BT_AWS_SUPPORT__
    bt_codec_a2dp_aws_get_silence_frame_information(handle, &silence_bs_info);
    media_info->silence_byte_per_frame = silence_bs_info.byte_count;
    media_info->sample_count_per_frame = silence_bs_info.sample_count;
#endif
    bt_sink_srv_report("[PLH] med init, media_info: 0x%08x, silence size:%d, sample_count:%d", 
                media_info, silence_bs_info.byte_count, silence_bs_info.sample_count);
}

uint32_t sink_srv_util_get_a2dp_time_stamp(bt_hci_packet_t *packet)
{
    uint8_t *media_header = (uint8_t *)packet + packet->offset;
    uint32_t ts = (media_header[4] << 24) | (media_header[5] << 16) | (media_header[6] << 8) | (media_header[7]);
    return ts;
}

uint16_t sink_srv_util_get_a2dp_sn(bt_hci_packet_t *packet)
{
    uint8_t *media_header = (uint8_t *)packet + packet->offset;
    uint16_t sn = (uint16_t)((media_header[2] << 8) | (media_header[3]));
    return sn;
}

float sinv_srv_util_sample_2_us_count(uint32_t sample_count, uint32_t sample_rate)
{
    float result = 1.0 * sample_count * 1000 * 1000 / sample_rate;
    return result;
}

uint32_t sinv_srv_util_us_2_sample_count(uint32_t us_count, uint32_t sample_rate)
{
    float result = 1.0 * us_count * sample_rate / 1000 / 1000;
    //return us_count * sample_rate / 1000 / 1000;
    return (uint32_t)result;
}

uint32_t sink_srv_util_get_packet_sample_counts(bt_media_handle_t *handle, bt_hci_packet_t *packet)
{
    bt_codec_a2dp_buffer_t raw_data;
    bt_codec_a2dp_bitstream_t sample_data;
    uint8_t *media_header = (uint8_t *)packet + packet->offset;
    uint32_t payload_len = packet->packet_length - (packet->offset+MEDIA_PACKET_HEADER_LEN);

    raw_data.buffer = media_header + MEDIA_PACKET_HEADER_LEN;
    raw_data.byte_count = payload_len;
    
    bt_codec_a2dp_aws_parse_data_information(handle, &raw_data, &sample_data);

    return sample_data.sample_count;
}

void bt_clock_unitializate(bt_aws_clock_t *bt_clk)
{
    if (bt_clk->nclk_intra > 312) {
        bt_clk->nclk = (bt_clk->nclk + ((bt_clk->nclk_intra<<1)/625)) & 0x0FFFFFFF;
        bt_clk->nclk_intra = ((bt_clk->nclk_intra<<1)%625)>>1;
    }
}

void bt_clock_op_add(bt_aws_clock_t *dst, const bt_aws_clock_t *src, double gap)
{
    uint64_t duration_us = (uint64_t)gap;
    uint32_t nclk = (duration_us<<1)/625;
    uint32_t nclk_intra = ((duration_us<<1)%625)>>1;

    dst->nclk_intra = (src->nclk_intra + nclk_intra);
    dst->nclk = src->nclk + nclk;
    bt_clock_unitializate(dst);
}


void sink_srv_calc_target_clock(sink_srv_media_play_info_t *media_info, sink_srv_clock_mapping_t *base, sink_srv_clock_mapping_t *target)
{
    uint64_t time_diff;

    bt_sink_srv_assert((NULL != media_info && NULL != base && NULL != target) && "Error: args NULL");
    //  duration on ts2 - ts0
    {
        uint32_t samples_diff = (target->time_stamp - base->time_stamp) / media_info->count_per_sample;
        if (media_info->aac_codec && (media_info->ts_diff_by_packet & 0xfff)) {
            samples_diff = media_info->sample_count_per_frame * (target->seq_idx - base->seq_idx);
        }
        time_diff = (uint64_t)sinv_srv_util_sample_2_us_count(samples_diff, media_info->sample_rate);
    }

    // duration elaplsed between bt0 and bt1.
    bt_clock_op_add(&target->piconet_clk, &base->piconet_clk, time_diff);
    bt_clock_op_add(&target->local_clk, &base->local_clk, time_diff);
}

// ---|-------------------cur==============x----------------
//   (bt0, ts0)          (bt1)            (ts2)
// two things should be completed before this api is called.
// 1. must get the current piconect bt clock & bt local time.
// 2. must set the time_stamp.
uint32_t sink_srv_get_play_duration(sink_srv_media_play_info_t * media_info, sink_srv_clock_mapping_t *base, 
                                bt_aws_clock_t *cur_bt_clk, uint32_t time_stamp)
{
    bt_aws_clock_t base_spec_clk;
    uint64_t elapsed_time;
    uint64_t time_diff;
    uint32_t duration;

    bt_sink_srv_assert((NULL != media_info && NULL != base && NULL != cur_bt_clk) && "play media info NULL");
    //  duration on ts2 - ts0
    {
        uint32_t samples_diff = (time_stamp - base->time_stamp) / media_info->count_per_sample;
        if (media_info->aac_codec && (media_info->ts_diff_by_packet & 0xfff)) {
            samples_diff = media_info->sample_count_per_frame * (media_info->sequence_idx - base->seq_idx);
        }
        time_diff = (uint64_t)sinv_srv_util_sample_2_us_count(samples_diff, media_info->sample_rate);
    }

    // duration elaplsed between bt0 and bt1.
    memcpy(&base_spec_clk, &base->piconet_clk, sizeof(bt_aws_clock_t));
    base_spec_clk.nclk += (base_spec_clk.nclk_intra << 1) / 625;
    base_spec_clk.nclk_intra = (((base_spec_clk.nclk_intra << 1) % 625) >> 1);

    {
        uint32_t elapsed_main_clk = (cur_bt_clk->nclk - base_spec_clk.nclk - 1) & BT_MAIN_COLOK_SCOPE;
        uint32_t elapsed_frac_clk = (BT_FRAC_CLOCK_UPPER + cur_bt_clk->nclk_intra - base_spec_clk.nclk_intra);
        elapsed_time = ((uint64_t)elapsed_main_clk * 312) + (elapsed_main_clk >> 1) + elapsed_frac_clk;
    }

    duration = (uint32_t)(time_diff - elapsed_time);
    bt_sink_srv_report("[PLH] check_dur, b_clk(0x%08x, 0x%08x), b_ts:0x%08x, b_sn: 0x%08x, cur_clk(0x%08x, 0x%08x), t_ts:0x%08x, t_sn: 0x%08x, dur: %d", 
                        base_spec_clk.nclk, base_spec_clk.nclk_intra, base->time_stamp, base->seq_idx,
                        cur_bt_clk->nclk, cur_bt_clk->nclk_intra, time_stamp, media_info->sequence_idx, duration);

    return duration;
}

uint32_t sink_srv_a2dp_check_media_continuous(bt_media_handle_t *handle, sink_srv_media_play_info_t *media_info, 
                                sink_media_packet_info_t *last_info, bt_hci_packet_t *next_packet)
{
    uint16_t last_sn, next_sn, sn_diff;
    uint32_t last_ts, next_ts, ts_diff;
    uint32_t ret_val;

    last_sn = last_info->seq_num;
    next_sn = sink_srv_util_get_a2dp_sn(next_packet);
    sn_diff = next_sn - last_sn;
    bt_sink_srv_assert(sn_diff != 0 && "Error: sn_diff identical.");

    last_ts = last_info->time_stamp;
    next_ts = sink_srv_util_get_a2dp_time_stamp(next_packet);
    ts_diff = next_ts - last_ts;
    if (last_ts == (~0)) {
        return SINK_SRV_PLH_TYPE_CONTINUOUS;
    }

    if (sn_diff > 0x7fff) {
        ret_val = SINK_SRV_PLH_TYPE_DROP;
    } else if (sn_diff > MAX_SBC_FRAME_NUM_PER_PACKET) {
        ret_val = SINK_SRV_PLH_TYPE_CONTINUOUS;
    } else if (sn_diff >1) {
        ret_val = SINK_SRV_PLH_TYPE_LACK;
    } else {
        // for Iphone case, ts_diff is not 4096.
        ret_val = SINK_SRV_PLH_TYPE_CONTINUOUS;
    }

    if (ret_val == SINK_SRV_PLH_TYPE_CONTINUOUS) {
        media_info->ts_diff_by_packet = ts_diff;
    }

    if (ret_val != SINK_SRV_PLH_TYPE_CONTINUOUS) {
        bt_sink_srv_report("[PLH] check_cont, last_sn: 0x%08x, next_sn: 0x%08x, last_ts: 0x%08x, next_ts: 0x%08x,result: %d", 
                        last_sn, next_sn, last_ts, next_ts, ret_val);
    }

    return ret_val;
}

void * sink_srv_a2dp_fill_silence_frame(bt_media_handle_t *handle, sink_srv_media_play_info_t *media_info,
                                        sink_media_packet_info_t *last_info, bt_hci_packet_t *next_packet, bt_a2dp_codec_type_t type)
{
    uint32_t silence_ts;
    uint32_t next_ts;
    uint16_t silence_sn;
    uint16_t next_sn;
    uint32_t this_sample_count;
    uint32_t frame_num;
    uint32_t silence_bytes = 0;

    bt_hci_packet_t *pkt_buff = NULL;
    uint8_t *media_ptr = NULL;

    bt_codec_a2dp_buffer_t silence_buf = {NULL, 0};

    bt_sink_srv_assert((next_packet!=NULL) && "the last packet should not be NULL");

    // Calculate the sample count and frame num lost.
    next_ts = sink_srv_util_get_a2dp_time_stamp(next_packet);
    next_sn = sink_srv_util_get_a2dp_sn(next_packet);

    if (media_info->aac_codec && (media_info->ts_diff_by_packet & 0xfff)) { // for Iphone case.
        silence_ts = last_info->time_stamp + media_info->ts_diff_by_packet;
        this_sample_count = media_info->sample_count_per_frame;
        frame_num = next_sn - last_info->seq_num - 1;
    } else {
        silence_ts = last_info->time_stamp + last_info->sample_counts * media_info->count_per_sample;
        this_sample_count = (next_ts - silence_ts) / media_info->count_per_sample;
        frame_num = this_sample_count / media_info->sample_count_per_frame;
    }

    if (frame_num > MAX_SBC_FRAME_NUM_PER_PACKET) {
        return NULL;
    }

    // SBC case.
    if (type == BT_A2DP_CODEC_SBC) {
        silence_bytes = frame_num * media_info->silence_byte_per_frame + 1;
    } else if (type == BT_A2DP_CODEC_AAC) {
        silence_bytes = frame_num * media_info->silence_byte_per_frame + (AAC_PAYLOAD_SILENCE_HEADER - AAC_ADTS_HEADER_LEN);
    }

    if(frame_num == 0) {
        return NULL;
    }
    bt_sink_srv_assert(silence_bytes >3 && "silence len error");

    // Alloc memory from BT Rx_Buf and fill silence data into it.    
    pkt_buff = (bt_hci_packet_t *)bt_alloc_rx_buffer_from_external(MEDIA_PACKET_HEADER_LEN + silence_bytes);
    bt_sink_srv_assert(pkt_buff != NULL && "No Rx Buffer available");

    media_ptr = (uint8_t *)pkt_buff + pkt_buff->offset;

    silence_buf.byte_count = silence_bytes;
    if (type == BT_A2DP_CODEC_SBC) {
        silence_buf.buffer = media_ptr + MEDIA_PACKET_HEADER_LEN;
    } else if (type == BT_A2DP_CODEC_AAC) {
        silence_buf.buffer = media_ptr + MEDIA_PACKET_HEADER_LEN + (AAC_PAYLOAD_SILENCE_HEADER - AAC_ADTS_HEADER_LEN);
    } else {
        bt_sink_srv_assert(0 && "codec error");
    }

    bt_codec_a2dp_aws_fill_silence_frame(handle, &silence_buf, frame_num);

    if (type == BT_A2DP_CODEC_AAC) {
        memcpy(media_ptr + MEDIA_PACKET_HEADER_LEN, s_aac_payload_header, AAC_PAYLOAD_HEADER_LEN);
        media_ptr[MEDIA_PACKET_HEADER_LEN + AAC_PAYLOAD_HEADER_LEN] = 0x09;
    }
    // update the media header.
    silence_sn = last_info->seq_num + 1;
    if (next_sn - silence_sn > 1) {
        silence_sn = next_sn - 1;
    }

    sink_srv_util_update_silience_a2dp_header(pkt_buff, silence_sn, silence_ts);

    bt_sink_srv_report("[PLH] fill silence, pkt:0x%08x, sn: %d, ts: 0x%08x, frame_num: %d, silence_len: %d", 
                        pkt_buff, silence_sn, silence_ts, frame_num, silence_bytes);
    return pkt_buff;
}

#ifdef __MEDIA_DUMP_DEBUG__
void sink_srv_dump_dsp_data(uint8_t *buff, uint32_t len)
{
    static uint32_t dump_index = 0;
    if (dump_index + len < DUMP_BUFFER_SIZE) {
        memcpy(g_a2dp_dump_buf+dump_index, buff, len);
        dump_index += len;
    } else {
        uint32_t tail_len = DUMP_BUFFER_SIZE - dump_index;
        memcpy(g_a2dp_dump_buf+dump_index, buff, tail_len);
        memcpy(g_a2dp_dump_buf, buff, len-tail_len);
        dump_index = len - tail_len;
    }
}
#endif
