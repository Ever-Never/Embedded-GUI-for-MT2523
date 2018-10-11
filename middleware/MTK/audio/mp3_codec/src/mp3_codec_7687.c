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


#include <string.h>


#include "memory_attribute.h"

#include "task_def.h"


#if defined(MTK_I2S_ENABLED)
#include "hal_i2s.h"
#include "hal_platform.h"
#include "nau8810.h"
#include "hal_i2c_master.h"
#include "hal_gpt.h"
#include "mp3_codec_internal_7687.h"
#include "mp3_codec.h"
#else
#include "mp3_codec_internal.h"
#include "mp3_codec.h"
#include "hal_audio_internal_service.h"
#endif


#define MAX_MP3_CODEC_FUNCTIONS      3

static mp3_codec_internal_handle_t *mp3_codec_internal_handle = NULL;
PRIVILEGED_DATA static QueueHandle_t mp3_codec_queue_handle = NULL;
static uint32_t mp3_codec_queue_reg_num = 0;
static mp3_codec_queue_event_id_t mp3_codec_queue_id_array[MAX_MP3_CODEC_FUNCTIONS];
static mp3_codec_internal_callback_t mp3_codec_queue_handler[MAX_MP3_CODEC_FUNCTIONS];



/*
#ifdef __ICCARM__
_Pragma("data_alignment=4") static uint8_t mp3_decode_buffer[41000] = {0};
#else
static __attribute__((__aligned__(4))) uint8_t mp3_decode_buffer[41000] = {0};
#endif
*/

#define MP3_DECODE_BUFFER_LENGTH 41000
static uint8_t *mp3_decode_buffer = NULL;


#if defined(MTK_I2S_ENABLED)
#define MINIMUM(a,b)            ((a) < (b) ? (a) : (b))


/*
#ifdef __ICCARM__
#define I2S_TX_BUFFER_LENGTH 4608
_Pragma("data_alignment=4") static uint32_t i2s_tx_buffer[I2S_TX_BUFFER_LENGTH] = {0};
#else
#define I2S_TX_BUFFER_LENGTH 4608 //4608
static __attribute__((__aligned__(4))) uint32_t i2s_tx_buffer[I2S_TX_BUFFER_LENGTH] = {0};
#endif
*/


#define I2S_TX_BUFFER_LENGTH 4608
static uint32_t *i2s_tx_buffer = NULL;
#else
//pcm playback use
static uint8_t audio_internal_ring_buffer[4096];
//mp3 decoder lib use
#endif


//static uint32_t mp3_task_stack_buf[1024];


/* for calculate MCPS
volatile int *DWT_CONTROL = (int *)0xE0001000;
volatile int *DWT_CYCCNT = (int *)0xE0001004;
volatile int *DEMCR = (int *)0xE000EDFC;
volatile uint32_t count_test = 0;
volatile uint32_t offset = 0;
#define CPU_RESET_CYCLECOUNTER do { *DEMCR = *DEMCR | 0x01000000; \
*DWT_CYCCNT = 0; \
*DWT_CONTROL = *DWT_CONTROL | 1 ; } while(0)
*/

#define ID3V2_HEADER_LENGTH 10  // in bytes
#define MPEG_AUDIO_FRAME_HEADER_LENGTH 4 // in bytes

/*
#define IS_MP3_HEAD(head) (!( ((head & 0xfff80000) != 0xfff80000) || \
  ( ((head>>17)&3)!= 1) || \
  ( ((head>>12)&0xf) == 0xf) || ( ((head>>12)&0xf) == 0x0) || \
( ((head>>10)&0x3) == 0x3 )))
*/

#define IS_MP3_HEAD(head) (!( (((head & 0xfff00000) != 0xfff00000) && ((head & 0xfff80000) != 0xffe00000) ) || \
  ( ((head>>17)&3)== 3) || (((head>>17)&3)== 0) || \
  ( ((head>>12)&0xf) == 0xf) || ( ((head>>12)&0xf) == 0x0) || \
( ((head>>10)&0x3) == 0x3 )))



#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_NA    0
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_32    1
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_40    2
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_48    3
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_56    4
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_64    5
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_80    6
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_96    7
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_112    8
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_128    9
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_160    0xa
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_192    0xb
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_224    0xc
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_256    0xd
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_320    0xe
#define MP3_MPEG_AUDIO_FRAME_BIT_RATE_NA2    0xf


#if defined(MTK_I2S_ENABLED)

/* Example of i2s_close */
static void mp3_codec_i2s_close(void)
{
    hal_i2s_disable_tx();
    hal_i2s_disable_audio_top();
    hal_i2s_deinit();
    hal_i2s_stop_tx_vfifo();
}

/* Example of i2s_open */
static void mp3_codec_i2s_open(void)
{
    hal_i2s_enable_tx();
    hal_i2s_enable_audio_top();
}
#if 0
static hal_i2s_channel_number_t mp3_codec_translate_decoder_ip_channel_number_to_i2s_channel_number(uint16_t channel_number)
{
    hal_i2s_channel_number_t hal_i2s_channel_number = HAL_I2S_STEREO;

    hal_i2s_channel_number = (channel_number == 1) ? HAL_I2S_MONO : HAL_I2S_STEREO;

    return hal_i2s_channel_number;
}
#endif
static AUCODEC_BCLK_RATE_e mp3_codec_translate_decoder_ip_channel_number_to_nau8810_bclk(uint16_t channel_number)
{
    AUCODEC_BCLK_RATE_e bclk = e32xFS;

    bclk = (channel_number == 1) ? e16xFS : e32xFS;

    return bclk;
}


static AUCODEC_SAMPLERATE_SEL_e mp3_codec_translate_decoder_ip_sample_rate_index_to_nau8810_index(uint16_t sample_rate_index)
{

    AUCODEC_SAMPLERATE_SEL_e hal_i2s_sample_rate = eSR44K1Hz;

    switch (sample_rate_index) {
        case 0: // mp3 decoder SWIP = 44.1k
            hal_i2s_sample_rate = eSR44K1Hz;  
            break;
        case 1: // mp3 decoder SWIP = 48k
            hal_i2s_sample_rate = eSR48KHz;
            break;
        case 2: // mp3 decoder SWIP = 32k
            hal_i2s_sample_rate = eSR32KHz;
            break;
        case 3: // mp3 decoder SWIP = 22.05k
            hal_i2s_sample_rate = eSR22_05KHz;
            break;
        case 4: // mp3 decoder SWIP = 24k
            hal_i2s_sample_rate = eSR24KHz;
            break;
        case 5: // mp3 decoder SWIP = 16k
            hal_i2s_sample_rate = eSR16KHz;
            break;
        case 6: // mp3 decoder SWIP = 11.025k
            hal_i2s_sample_rate = eSR11_025KHz;
            break;
        case 7: // mp3 decoder SWIP = 12k
            hal_i2s_sample_rate = eSR12KHz;
            break;
        case 8: // mp3 decoder SWIP = 8k
            hal_i2s_sample_rate = eSR8KHz;
            break;
    }
    return hal_i2s_sample_rate;
}
#if 0
static hal_i2s_sample_rate_t mp3_codec_translate_decoder_ip_sample_rate_index_to_i2s_index(uint16_t sample_rate_index)
{

    hal_i2s_sample_rate_t hal_i2s_sample_rate = HAL_I2S_SAMPLE_RATE_48K;

    switch (sample_rate_index) {
        case 0: // mp3 decoder SWIP = 44.1k
            hal_i2s_sample_rate = HAL_I2S_SAMPLE_RATE_48K;  // we have no 44.1k in 7687
            break;
        case 1: // mp3 decoder SWIP = 48k
            hal_i2s_sample_rate = HAL_I2S_SAMPLE_RATE_48K;
            break;
        case 2: // mp3 decoder SWIP = 32k
            hal_i2s_sample_rate = HAL_I2S_SAMPLE_RATE_32K;
            break;
        case 3: // mp3 decoder SWIP = 22.05k
            hal_i2s_sample_rate = HAL_I2S_SAMPLE_RATE_24K;  // we have no 22.05k in 7687
            break;
        case 4: // mp3 decoder SWIP = 24k
            hal_i2s_sample_rate = HAL_I2S_SAMPLE_RATE_24K;
            break;
        case 5: // mp3 decoder SWIP = 16k
            hal_i2s_sample_rate = HAL_I2S_SAMPLE_RATE_16K;
            break;
        case 6: // mp3 decoder SWIP = 11.025k
            hal_i2s_sample_rate = HAL_I2S_SAMPLE_RATE_12K;  // we have no 11.025k in 7687
            break;
        case 7: // mp3 decoder SWIP = 12k
            hal_i2s_sample_rate = HAL_I2S_SAMPLE_RATE_12K;
            break;
        case 8: // mp3 decoder SWIP = 8k
            hal_i2s_sample_rate = HAL_I2S_SAMPLE_RATE_8K;
            break;
    }
    return hal_i2s_sample_rate;
}
#endif

/* Example of i2s_configure */
static int8_t i2s_configure(hal_i2s_channel_number_t hal_i2s_channel_number, hal_i2s_sample_rate_t hal_i2s_sample_rate)
{
    hal_i2s_config_t i2s_config;
    hal_i2s_status_t result = HAL_I2S_STATUS_OK;

    /* Set I2S as internal loopback mode */
    result = hal_i2s_init(HAL_I2S_TYPE_EXTERNAL_MODE);
    if (HAL_I2S_STATUS_OK != result) {
        log_hal_info("\r\n ---i2s:    hal_i2s_init failed---\r\n");
        return -1;
    }

    /* Configure I2S  */
    i2s_config.clock_mode = HAL_I2S_SLAVE;
    i2s_config.rx_down_rate = HAL_I2S_RX_DOWN_RATE_DISABLE;
    i2s_config.tx_mode = HAL_I2S_TX_MONO_DUPLICATE_DISABLE;
    i2s_config.i2s_out.channel_number = hal_i2s_channel_number;

    i2s_config.i2s_out.sample_rate = hal_i2s_sample_rate;
    i2s_config.i2s_in.sample_rate = hal_i2s_sample_rate;
    i2s_config.i2s_in.msb_offset = 0;
    i2s_config.i2s_out.msb_offset = 0;
    i2s_config.i2s_in.word_select_inverse = 0;
    i2s_config.i2s_out.word_select_inverse = 0;
    i2s_config.i2s_in.lr_swap = 0;
    i2s_config.i2s_out.lr_swap = 0;

    result = hal_i2s_set_config(&i2s_config);
    if (HAL_I2S_STATUS_OK != result) {
        log_hal_info("\r\n ---i2s:    hal_i2s_set_config failed---\r\n");
        return -1;
    }

    return 1;
}

#endif
/*	share buffer operation function */
static void mp3_codec_set_share_buffer(mp3_codec_media_handle_t *handle, uint8_t *buffer, uint32_t length)
{
    handle->share_buff.buffer_base = buffer;
    //length &= ~0x1; // make buffer size even
    handle->share_buff.buffer_size = length;
    handle->share_buff.write = 0;
    handle->share_buff.read = 0;
    handle->waiting = false;
    handle->underflow = false;
}

static void mp3_codec_get_share_buffer_write_information(mp3_codec_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->share_buff.read > handle->share_buff.write) {
        count = handle->share_buff.read - handle->share_buff.write - 1;
    } else if (handle->share_buff.read == 0) {
        count = handle->share_buff.buffer_size - handle->share_buff.write - 1;
    } else {
        count = handle->share_buff.buffer_size - handle->share_buff.write;
    }
    *buffer = handle->share_buff.buffer_base + handle->share_buff.write;
    *length = count;
}


static void mp3_codec_get_share_buffer_read_information(mp3_codec_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->share_buff.write >= handle->share_buff.read) {
        count = handle->share_buff.write - handle->share_buff.read;
    } else {
        count = handle->share_buff.buffer_size - handle->share_buff.read;
    }
    *buffer = handle->share_buff.buffer_base + handle->share_buff.read;
    *length = count;
}


static void mp3_codec_share_buffer_write_data_done(mp3_codec_media_handle_t *handle, uint32_t length)
{
    handle->share_buff.write += length;
    if (handle->share_buff.write == handle->share_buff.buffer_size) {
        handle->share_buff.write = 0;
    }
#if 0
    log_hal_info("[MP3 Codec ]mp3_codec_share_buffer_write_data_done:: handle->share_buff.write=%d\n", handle->share_buff.write);
#endif
}

static void mp3_codec_finish_write_data(mp3_codec_media_handle_t *handle)
{
    handle->waiting = false;
    handle->underflow = false;
}

static void mp3_codec_reset_share_buffer(mp3_codec_media_handle_t *handle)
{
    handle->share_buff.write = 0;
    handle->share_buff.read = 0;
    handle->waiting = false;
    handle->underflow = false;
}


static void mp3_codec_share_buffer_read_data_done(mp3_codec_media_handle_t *handle, uint32_t length)
{
    handle->share_buff.read += length;
    if (handle->share_buff.read == handle->share_buff.buffer_size) {
        handle->share_buff.read = 0;
    }
}


static int32_t mp3_codec_get_share_buffer_free_space(mp3_codec_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->share_buff.read - handle->share_buff.write - 2;
    if (count < 0) {
        count += handle->share_buff.buffer_size;
    }
    return count;
}

static int32_t mp3_codec_get_share_buffer_data_count(mp3_codec_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->share_buff.write - handle->share_buff.read;
    if (count < 0) {
        count += handle->share_buff.buffer_size;
    }
    return count;
}

static void mp3_codec_reset_stream_out_pcm_buffer(void)
{
    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;
    internal_handle->stream_out_pcm_buff.read_pointer = 0;
    internal_handle->stream_out_pcm_buff.write_pointer = 0;
}

static void mp3_codec_buffer_function_init(mp3_codec_media_handle_t *handle)
{
    handle->set_share_buffer   = mp3_codec_set_share_buffer;
    handle->get_write_buffer   = mp3_codec_get_share_buffer_write_information;
    handle->get_read_buffer    = mp3_codec_get_share_buffer_read_information;
    handle->write_data_done    = mp3_codec_share_buffer_write_data_done;
    handle->finish_write_data  = mp3_codec_finish_write_data;
    handle->reset_share_buffer = mp3_codec_reset_share_buffer;
    handle->read_data_done     = mp3_codec_share_buffer_read_data_done;
    handle->get_free_space     = mp3_codec_get_share_buffer_free_space;
    handle->get_data_count     = mp3_codec_get_share_buffer_data_count;
}


static uint32_t mp3_codec_get_bytes_from_share_buffer(mp3_codec_media_handle_t *handle, uint8_t *destination_buffer, uint32_t get_bytes_amount, bool want_move_read_ptr)
{
    uint8_t *share_buffer_read_address;
    uint32_t share_buffer_data_length;
    uint32_t share_buffer_read_index_original = 0;
    uint32_t bytes_amount_temp = get_bytes_amount;
    uint32_t got_bytes_amount = 0;  // real got bytes amount from share buffer

    share_buffer_read_index_original = handle->share_buff.read; // store original share_buffer read pointer


    uint16_t loop_idx = 0;
    for (loop_idx = 0; loop_idx < 2; loop_idx++) {
        mp3_codec_get_share_buffer_read_information(handle, &share_buffer_read_address, &share_buffer_data_length);
        if (share_buffer_data_length > 0) {
            uint32_t get_bytes_amount = MINIMUM(bytes_amount_temp, share_buffer_data_length);
            memcpy(destination_buffer, share_buffer_read_address, get_bytes_amount);
            bytes_amount_temp -= get_bytes_amount;
            destination_buffer += get_bytes_amount;
            mp3_codec_share_buffer_read_data_done(handle, get_bytes_amount);

            if (bytes_amount_temp == 0) {
                break;
            }
        } else {
            // share buffer empty
            break;
        }
    }


    got_bytes_amount = get_bytes_amount - bytes_amount_temp;  // real read amount

    if (got_bytes_amount != get_bytes_amount) {
        log_hal_info("[MP3 Codec]mp3_codec_get_bytes_from_share_buffer: got_bytes_amount(%ld) != get_bytes_amount(%ld)\n", got_bytes_amount, get_bytes_amount);
    }


    if (want_move_read_ptr == false) {
        handle->share_buff.read = share_buffer_read_index_original;
    }

    return got_bytes_amount;
}



static uint32_t mp3_codec_discard_bytes_of_share_buffer(mp3_codec_media_handle_t *handle, uint32_t discard_bytes_amount)
{
    uint8_t *share_buffer_read_address;
    uint32_t share_buffer_data_length;
    uint32_t bytes_amount_temp = discard_bytes_amount;
    uint32_t discarded_bytes_amount = 0;


    uint16_t loop_idx = 0;
    for (loop_idx = 0; loop_idx < 2; loop_idx++) {
        mp3_codec_get_share_buffer_read_information(handle, &share_buffer_read_address, &share_buffer_data_length);
        if (share_buffer_data_length > 0) {
            uint32_t get_bytes_amount = MINIMUM(bytes_amount_temp, share_buffer_data_length);
            bytes_amount_temp -= get_bytes_amount;
            mp3_codec_share_buffer_read_data_done(handle, get_bytes_amount);

            if (bytes_amount_temp == 0) {
                break;
            }
        } else {
            // share buffer empty
            break;
        }
    }

    discarded_bytes_amount = discard_bytes_amount - bytes_amount_temp;  // real read amount

    if (discarded_bytes_amount != discard_bytes_amount) {
        log_hal_info("[MP3 Codec]mp3_codec_discard_bytes_of_share_buffer : discarded_bytes_amount(%ld) != discard_bytes_amount(%ld)\n", discarded_bytes_amount, discard_bytes_amount);
    }


    return discarded_bytes_amount;
}

static mp3_codec_function_return_state_t mp3_codec_request_data_to_share_buffer(mp3_codec_media_handle_t *handle)
{
    // return MP3_CODEC_RETURN_OK:          request success
    // return  MP3_CODEC_RETURN_ERROR:    already request and waiting feed back

    if (!handle->waiting) {
        handle->waiting = true;
        handle->handler(handle, MP3_CODEC_MEDIA_REQUEST);
        return MP3_CODEC_RETURN_OK;
    } else {
        log_hal_info("[MP3 Codec] mp3_codec_request_data_to_share_buffer: already request and waiting feed back\n");
        return MP3_CODEC_RETURN_ERROR;
    }
}

static uint32_t mp3_codec_combine_four_bytes_buffer_to_uint32_value(uint8_t *buffer)
{
    uint32_t uint32_value = 0;

    uint32_value = *buffer;
    uint32_value = uint32_value << 8 | *(buffer + 1);
    uint32_value = uint32_value << 8 | *(buffer + 2);
    uint32_value = uint32_value << 8 | *(buffer + 3);

    return uint32_value;
}

static mp3_codec_function_return_state_t mp3_codec_reach_next_frame_and_get_audio_frame_header(mp3_codec_media_handle_t *handle, uint32_t *audio_frame_header, uint32_t maximum_check_bytes, uint32_t want_skip_frame_after_got_header)
{
    uint8_t check_mpeg_header_buffer[MPEG_AUDIO_FRAME_HEADER_LENGTH] = {0};
    uint32_t temp_mpeg_header = 0;
    uint32_t discard_bytes_amount = 0;
    uint32_t temp_uint32_t = 0;
    uint32_t temp_maximum_check_bytes = maximum_check_bytes;
    uint32_t maximum_request_data_time = maximum_check_bytes / handle->share_buff.buffer_size + 2;  // 2: arbitrarily selected


    do {
        if (mp3_codec_get_share_buffer_data_count(handle) < MPEG_AUDIO_FRAME_HEADER_LENGTH) {

            if (mp3_codec_request_data_to_share_buffer(handle) == MP3_CODEC_RETURN_OK) {
                maximum_request_data_time--;
            }

            if (mp3_codec_get_share_buffer_data_count(handle) < MPEG_AUDIO_FRAME_HEADER_LENGTH) {
                return MP3_CODEC_RETURN_ERROR;
            }
        }

        mp3_codec_get_bytes_from_share_buffer(handle, check_mpeg_header_buffer, MPEG_AUDIO_FRAME_HEADER_LENGTH, 0);
        temp_mpeg_header = mp3_codec_combine_four_bytes_buffer_to_uint32_value(check_mpeg_header_buffer);

        if (IS_MP3_HEAD(temp_mpeg_header)) {
            // find MP3 HEAD
            *audio_frame_header = temp_mpeg_header;

            if (want_skip_frame_after_got_header) {
                discard_bytes_amount = 4;
                temp_uint32_t = mp3_codec_discard_bytes_of_share_buffer(handle, discard_bytes_amount);
                if (temp_uint32_t < discard_bytes_amount) {  // share buffer didn't have enoungh data to discared
                    return MP3_CODEC_RETURN_ERROR;
                }
            }
            log_hal_info("[MP3 Codec]mp3_codec_reach_next_frame: find mp3 header=%x\n", *audio_frame_header);
            return MP3_CODEC_RETURN_OK;
        }

        discard_bytes_amount = 1;
        temp_uint32_t = MINIMUM(discard_bytes_amount, temp_maximum_check_bytes);
        temp_uint32_t = mp3_codec_discard_bytes_of_share_buffer(handle, temp_uint32_t);
        if (temp_uint32_t < discard_bytes_amount) {  // share buffer didn't have enoungh data to discared
            return MP3_CODEC_RETURN_ERROR;
        }

        temp_maximum_check_bytes -= temp_uint32_t;

    } while (temp_maximum_check_bytes != 0 && maximum_request_data_time != 0);


    log_hal_info("[MP3 Codec]mp3_codec_reach_next_frame: not find mp3 header\n");
    *audio_frame_header = 0;

    return MP3_CODEC_RETURN_ERROR;
}


static void mp3_codec_get_id3v2_info_and_skip(mp3_codec_media_handle_t *handle, uint32_t file_size)
{
    uint32_t want_get_bytes = 0;
    uint8_t temp_buffer[10] = {0};
    uint32_t id3v2_remain_tagesize = 0; // not include ID3v2 header size, refert to ID3v2 spec
    uint32_t id3v2_tage_size = 0;   // total ID3v2 tage size which include ID3v2 header
    uint32_t remain_file_data_size = file_size; // in bytes

    handle->jump_file_to_specified_position = 0;    // asume from file begin
    handle->id3v2_information.has_id3v2 = false;
    handle->id3v2_information.id3v2_tage_length = 0;

    while (1) {
        want_get_bytes = ID3V2_HEADER_LENGTH;
        if (mp3_codec_get_bytes_from_share_buffer(handle, temp_buffer, want_get_bytes, 0) != want_get_bytes) {
            log_hal_info("[MP3 Codec]mp3_codec_get_id3v2_info_and_skip: share buffer data amount less than ID3v2 header length\n");
            return;    // just return
        }

        if (strncmp((const char *)temp_buffer, "ID3", 3) == 0) {
            id3v2_remain_tagesize = ((temp_buffer[6] & 0x7f) << 21) | ((temp_buffer[7] & 0x7f) << 14) | ((temp_buffer[8] & 0x7f) <<  7) | ((temp_buffer[9] & 0x7f) <<  0);
            id3v2_tage_size = id3v2_remain_tagesize + ID3V2_HEADER_LENGTH;
            log_hal_info("[MP3 Codec]find id3v2: id3v2_tagesize=%ld, id3v2_remain_tagesize =%ld\n", id3v2_tage_size, id3v2_remain_tagesize);


            if (remain_file_data_size < id3v2_tage_size) {
                // the tag size calculate form ID3v2 may wrong
                return;
            }


            handle->id3v2_information.has_id3v2 = true;
            handle->id3v2_information.id3v2_tage_length += id3v2_tage_size;


            // Although the remaing data in share buffer can be used,
            // but the fast and clear way to skip ID3v2 is just ask user to jump file to specific position and refill the share buffer
            mp3_codec_reset_share_buffer(handle);   // since we want to ask user to jump file to specific position and get data, thus remaining data is no use.
            handle->jump_file_to_specified_position += id3v2_tage_size;
            handle->handler(handle, MP3_CODEC_MEDIA_JUMP_FILE_TO);

            mp3_codec_request_data_to_share_buffer(handle);


            remain_file_data_size -= id3v2_tage_size;


        } else {
            log_hal_info("[MP3 Codec]done skip ID3v2, has_id3v2=%d, id3v2_tage_length=%d\n", (uint32_t)handle->id3v2_information.has_id3v2, handle->id3v2_information.id3v2_tage_length);
            return;
        }

    }
}


#if 0

mp3_codec_function_return_state_t  mp3_codec_set_memory(uint8_t *memory)
{

    if (mp3_codec_internal_handle == NULL) {
        log_hal_error("[MP3 Codec]mp3_codec_internal_handle = NULL\n");
        return MP3_CODEC_RETURN_ERROR;
    }
    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;
    mp3_codec_media_handle_t *handle = &internal_handle->handle;

    if (handle->state != MP3_CODEC_STATE_READY) {
        return MP3_CODEC_RETURN_ERROR;
    }
    uint8_t *memory_base = NULL;
    uint32_t totl_memory_size = 0;
    internal_handle->memory_pool = memory_base = memory;

    totl_memory_size = internal_handle->share_buff_size
                       + internal_handle->decode_pcm_buffer_size
                       + internal_handle->working_buff1_size
                       + internal_handle->working_buff2_size
                       + internal_handle->stream_out_pcm_buff_size;

    //set share buffer
    mp3_codec_set_share_buffer(handle, memory_base, internal_handle->share_buff_size);
    memory_base += internal_handle->share_buff_size;

    // set PCM buffer
    internal_handle->decode_pcm_buff.buffer_base_pointer = memory_base;
    internal_handle->decode_pcm_buff.buffer_byte_count = internal_handle->decode_pcm_buffer_size;
    internal_handle->decode_pcm_buff.read_pointer = 0;
    internal_handle->decode_pcm_buff.write_pointer = 0;
    memory_base += internal_handle->decode_pcm_buffer_size;

    //set working buffer
    internal_handle->working_buff1 = memory_base;
    memory_base += internal_handle->working_buff1_size;
    internal_handle->working_buff2 = memory_base;
    memory_base += internal_handle->working_buff2_size;


    // set PCM buffer   // piter add
    internal_handle->stream_out_pcm_buff.buffer_base_pointer = memory_base;
    internal_handle->stream_out_pcm_buff.buffer_byte_count = internal_handle->stream_out_pcm_buff_size;
    internal_handle->stream_out_pcm_buff.read_pointer = 0;
    internal_handle->stream_out_pcm_buff.write_pointer = 0;


    // STEP 2 : Get MP3 Handler
    internal_handle->mp3_handle = MP3Dec_Init(internal_handle->working_buff1, internal_handle->working_buff2);
    if (internal_handle->mp3_handle == NULL) {
        log_hal_error("[MP3 Codec]MP3Dec_Init fail");
    }

#if 1
    log_hal_info("[MP3 Codec]set_memory memory range : start=%08x end=%08x",
                 memory, (memory + totl_memory_size));
    log_hal_info("[MP3 Codec]set_memory share buffer : base=%08x size=%u",
                 handle->share_buff.buffer_base, (unsigned int)handle->share_buff.buffer_size);
    log_hal_info("[MP3 Codec]set_memory decode_pcm_buff.buffer : base=%08x size=%u",
                 internal_handle->decode_pcm_buff.buffer_base_pointer, (unsigned int)internal_handle->decode_pcm_buff.buffer_byte_count);
    log_hal_info("[MP3 Codec]set_memory work1 buffer : base=%08x size=%u",
                 internal_handle->working_buff1, (unsigned int)internal_handle->working_buff1_size);
    log_hal_info("[MP3 Codec]set_memory work2 buffer : base=%08x size=%u",
                 internal_handle->working_buff2, (unsigned int)internal_handle->working_buff2_size);
    log_hal_info("[MP3 Codec]set_memory stream_out_pcm_buff : base=%08x size=%u",
                 internal_handle->stream_out_pcm_buff.buffer_base_pointer, (unsigned int)internal_handle->stream_out_pcm_buff.buffer_byte_count);
#endif



    return MP3_CODEC_RETURN_OK;

}

mp3_codec_function_return_state_t mp3_codec_get_memory_size(uint32_t *memory_size)
{

    if (mp3_codec_internal_handle == NULL) {
        log_hal_error("[MP3 Codec]mp3_codec_internal_handle = NULL\n");
        return MP3_CODEC_RETURN_ERROR;
    }
    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;
    mp3_codec_media_handle_t *handle = &internal_handle->handle;

    if (handle->state != MP3_CODEC_STATE_READY) {
        return MP3_CODEC_RETURN_ERROR;
    }

    int working_buff1_size,         /* the required Working buffer1 size in byte    */
        working_buff2_size,         /* the required Working buffer2 size in byte    */
        decode_pcm_buffer_size,              /* the required pcm buffer size in byte          */
        share_buff_size;            /* the share buffer size      */

    *memory_size = 0;

    /*STEP 1 : Allocate data memory for MP3 decoder*/
    MP3Dec_GetMemSize(&share_buff_size, &decode_pcm_buffer_size, &working_buff1_size, &working_buff2_size);

    //share_buff_size += 1024;  // piter delete

    //4bytes aligned
    share_buff_size = (share_buff_size + 3) & ~0x3;
    decode_pcm_buffer_size = (decode_pcm_buffer_size + 3) & ~0x3;
    working_buff1_size = (working_buff1_size + 3) & ~0x3;
    working_buff2_size = (working_buff2_size + 3) & ~0x3;


    internal_handle->share_buff_size = share_buff_size;
    internal_handle->decode_pcm_buffer_size = decode_pcm_buffer_size;
    internal_handle->working_buff1_size = working_buff1_size;
    internal_handle->working_buff2_size = working_buff2_size;
    internal_handle->stream_out_pcm_buff_size = decode_pcm_buffer_size * 3;

    //get total memory size for mp3
    *memory_size = internal_handle->share_buff_size + internal_handle->decode_pcm_buffer_size + internal_handle->working_buff1_size + internal_handle->working_buff2_size + internal_handle->stream_out_pcm_buff_size;

#if 0
    log_hal_info("[MP3 Codec]internal_handle->share_buff_size=%u\n", internal_handle->share_buff_size);
    log_hal_info("[MP3 Codec]internal_handle->pcm_buff_size=%u\n", internal_handle->pcm_buff_size);
    log_hal_info("[MP3 Codec]internal_handle->working_buff1_size=%u\n", internal_handle->working_buff1_size);
    log_hal_info("[MP3 Codec]internal_handle->working_buff2_size=%u\n", internal_handle->working_buff2_size);
#endif

    return MP3_CODEC_RETURN_OK;

}
#endif


#if 1
mp3_codec_function_return_state_t mp3_codec_set_memory2(void)
{
    if (mp3_codec_internal_handle == NULL) {
        log_hal_error("[MP3 Codec]mp3_codec_internal_handle = NULL\n");
        return MP3_CODEC_RETURN_ERROR;
    }

    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;
    mp3_codec_media_handle_t *handle = &internal_handle->handle;

    if (handle->state != MP3_CODEC_STATE_READY) {
        return MP3_CODEC_RETURN_ERROR;
    }

    uint8_t *memory_base = NULL;
    int working_buff1_size,         /* the required Working buffer1 size in byte    */
        working_buff2_size,         /* the required Working buffer2 size in byte    */
        decode_pcm_buffer_size,              /* the required pcm buffer size in byte          */
        share_buff_size;            /* the share buffer size      */

    /*STEP 1 : Allocate data memory for MP3 decoder*/
    MP3Dec_GetMemSize(&share_buff_size, &decode_pcm_buffer_size, &working_buff1_size, &working_buff2_size);

    //share_buff_size += 1024;     // piter delete

    //4bytes aligned
    share_buff_size = (share_buff_size + 3) & ~0x3;
    decode_pcm_buffer_size = (decode_pcm_buffer_size + 3) & ~0x3;
    working_buff1_size = (working_buff1_size + 3) & ~0x3;
    working_buff2_size = (working_buff2_size + 3) & ~0x3;

    internal_handle->share_buff_size = share_buff_size;
    internal_handle->decode_pcm_buffer_size = decode_pcm_buffer_size;
    internal_handle->working_buff1_size = working_buff1_size;
    internal_handle->working_buff2_size = working_buff2_size;
    internal_handle->stream_out_pcm_buff_size = decode_pcm_buffer_size * 3;

    //specify memory pool
    internal_handle->memory_pool = memory_base = mp3_decode_buffer;

    //set share buffer
    mp3_codec_set_share_buffer(handle, memory_base, share_buff_size);
    memory_base += share_buff_size;

    // set decode_pcm_buffer
    internal_handle->decode_pcm_buff.buffer_base_pointer = memory_base;
    internal_handle->decode_pcm_buff.buffer_byte_count = internal_handle->decode_pcm_buffer_size;
    internal_handle->decode_pcm_buff.read_pointer = 0;
    internal_handle->decode_pcm_buff.write_pointer = 0;
    memory_base += internal_handle->decode_pcm_buffer_size;


    //set working buffer
    internal_handle->working_buff1 = memory_base;
    memory_base += internal_handle->working_buff1_size;
    internal_handle->working_buff2 = memory_base;
    memory_base += internal_handle->working_buff2_size;

    // set PCM buffer
    internal_handle->stream_out_pcm_buff.buffer_base_pointer = memory_base;
    internal_handle->stream_out_pcm_buff.buffer_byte_count = internal_handle->stream_out_pcm_buff_size;
    internal_handle->stream_out_pcm_buff.read_pointer = 0;
    internal_handle->stream_out_pcm_buff.write_pointer = 0;




    /*STEP 2 : Get MP3 Handler */
    internal_handle->mp3_handle = MP3Dec_Init(internal_handle->working_buff1, internal_handle->working_buff2);
    if (internal_handle->mp3_handle == NULL) {
        log_hal_error("[MP3 Codec]MP3Dec_Init fail");
    }

#if 0
    log_hal_info("[MP3 Codec]set_memory memory range : start=%08x end=%08x",
                 &mp3_decode_buffer[0], &mp3_decode_buffer[40999]);
    log_hal_info("[MP3 Codec]set_memory share buffer : base=%08x size=%u",
                 handle->share_buff.buffer_base, (unsigned int)handle->share_buff.buffer_size);
    log_hal_info("[MP3 Codec]set_memory decode_pcm_buff.buffer : base=%08x size=%u",
                 internal_handle->decode_pcm_buff.buffer_base_pointer, (unsigned int)internal_handle->decode_pcm_buff.buffer_byte_count);
    log_hal_info("[MP3 Codec]set_memory work1 buffer : base=%08x size=%u",
                 internal_handle->working_buff1, (unsigned int)internal_handle->working_buff1_size);
    log_hal_info("[MP3 Codec]set_memory work2 buffer : base=%08x size=%u",
                 internal_handle->working_buff2, (unsigned int)internal_handle->working_buff2_size);
    log_hal_info("[MP3 Codec]set_memory stream_out_pcm_buff : base=%08x size=%u",
                 internal_handle->stream_out_pcm_buff.buffer_base_pointer, (unsigned int)internal_handle->stream_out_pcm_buff.buffer_byte_count);
#endif

    return MP3_CODEC_RETURN_OK;

}
#endif


static void mp3_codec_event_send_from_isr(mp3_codec_queue_event_id_t id, void *parameter)
{
    mp3_codec_queue_event_t event;
    event.id        = id;
    event.parameter = parameter;
    if (xQueueSendFromISR(mp3_codec_queue_handle, &event, 0) != pdPASS) {
        log_hal_info("[MP3 Codec]queue not pass %d\r\n", id);
        return;
    }

    return;
}

static void mp3_codec_event_register_callback(mp3_codec_queue_event_id_t reg_id, mp3_codec_internal_callback_t callback)
{
    uint32_t id_idx;
    for (id_idx = 0; id_idx < MAX_MP3_CODEC_FUNCTIONS; id_idx++) {
        if (mp3_codec_queue_id_array[id_idx] == MP3_CODEC_QUEUE_EVENT_NONE) {
            mp3_codec_queue_id_array[id_idx] = reg_id;
            mp3_codec_queue_handler[id_idx] = callback;
            mp3_codec_queue_reg_num++;
            break;
        }
    }
    return;
}

static void mp3_codec_event_deregister_callback(mp3_codec_queue_event_id_t dereg_id)
{
    uint32_t id_idx;
    for (id_idx = 0; id_idx < MAX_MP3_CODEC_FUNCTIONS; id_idx++) {
        if (mp3_codec_queue_id_array[id_idx] == dereg_id) {
            mp3_codec_queue_id_array[id_idx] = MP3_CODEC_QUEUE_EVENT_NONE;
            mp3_codec_queue_reg_num--;
            break;
        }
    }
    return;
}


static void mp3_codec_task_main(void *arg)
{
    log_hal_info("[MP3 Codec]mp3_codec_task_main create\n");

    mp3_codec_queue_event_t event;

    while (1) {
        if (xQueueReceive(mp3_codec_queue_handle, &event, portMAX_DELAY)) {
            //log_hal_info("[MP3 Codec]xQueueReceive event\n");
            mp3_codec_queue_event_id_t rece_id = event.id;
            uint32_t id_idx;
            for (id_idx = 0; id_idx < MAX_MP3_CODEC_FUNCTIONS; id_idx++) {
                if (mp3_codec_queue_id_array[id_idx] == rece_id) {
                    //log_hal_info("[MP3 Codec]xQueueReceive find event_id mp3_codec_queue_id_array[%d]=%d\n",id_idx,event.id);
                    mp3_codec_queue_handler[id_idx](event.parameter);
                    break;
                }
            }
        }
    }
}



static void mp3_codec_task_create(void)
{
    xTaskCreate(mp3_codec_task_main, MP3_CODEC_TASK_NAME, MP3_CODEC_TASK_STACKSIZE / sizeof(StackType_t), NULL, MP3_CODEC_TASK_PRIO, NULL);    
}

#if defined(MTK_I2S_ENABLED)
#else
static hal_audio_channel_number_t mp3_codec_translate_decoder_ip_channel_number(uint16_t channel_number)
{
    hal_audio_channel_number_t hal_audio_channel_number = HAL_AUDIO_STEREO;

    hal_audio_channel_number = (channel_number == 1) ? HAL_AUDIO_MONO : HAL_AUDIO_STEREO;

    return hal_audio_channel_number;
}
#endif
#if defined(MTK_I2S_ENABLED)
#else
static hal_audio_sampling_rate_t mp3_codec_translate_decoder_ip_sample_rate_index(uint16_t sample_rate_index)
{

    hal_audio_sampling_rate_t hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;

    switch (sample_rate_index) {
        case 0:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
            break;
        case 1:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
            break;
        case 2:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_32KHZ;
            break;
        case 3:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_22_05KHZ;
            break;
        case 4:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_24KHZ;
            break;
        case 5:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
            break;
        case 6:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_11_025KHZ;
            break;
        case 7:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_12KHZ;
            break;
        case 8:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_8KHZ;
            break;
    }
    return hal_audio_sampling_rate;
}
#endif

#if defined(MTK_I2S_ENABLED)
static void mp3_codec_i2s_tx_isr_callback(hal_i2s_event_t event, void *user_data)
{
    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;
    uint32_t loop_idx = 0;
    uint32_t i = 0;
    hal_i2s_config_t i2s_config;
    uint32_t i2s_amount_space_tones;
    uint32_t i2s_tone_data = 0;

    hal_i2s_get_config(&i2s_config);
    switch (event) {
        case HAL_I2S_EVENT_DATA_REQUEST:

            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                uint8_t *out_buf_ptr    = NULL;
                uint32_t stream_out_pcm_buffer_data_count = 0;
                hal_i2s_get_tx_sample_count(&i2s_amount_space_tones);
                ring_buffer_get_read_information(&internal_handle->stream_out_pcm_buff, &out_buf_ptr, &stream_out_pcm_buffer_data_count);



                uint32_t stream_out_pcm_buffer_data_tone_amount = (internal_handle->mp3_handle->CHNumber == 1) ? (stream_out_pcm_buffer_data_count / 2) : (stream_out_pcm_buffer_data_count / 4);

                if(stream_out_pcm_buffer_data_tone_amount > 0) {
                    uint32_t stream_out_tone_amount = MINIMUM(i2s_amount_space_tones, stream_out_pcm_buffer_data_tone_amount);

                    for (i = 0; i < stream_out_tone_amount; i++) {
                        i2s_tone_data = 0;


                        // hal_i2s_status_t hal_i2s_tx_write(uint32_t data);
                        // The data format for I2S TX is as shown below:
                        //              | <-             32 bits                   -> |
                        // Stereo    R[31:24]    R[23:16]    L[15:8]    L[7:0]
                        // Mono      8'b0         8'b0           L[15:8]    L[7:0]

                        //                    Byte1          Byte2           Byte3          Byte4
                        // MP3 Stereo    L[7:0]         L[15:8]         R[7:0]         R[15:8]
                        // MP3 Mono      Tone1[7:0]  Tone1[15:8]  Tone2[7:0]  Tone2[15:8]

                        if ((internal_handle->mp3_handle->CHNumber == 1)){  // mono
                            i2s_tone_data = ((uint32_t)*(out_buf_ptr)) << 0;
                            i2s_tone_data = ((uint32_t)*(out_buf_ptr)) << 16;
                            out_buf_ptr++;
                            i2s_tone_data = i2s_tone_data | ((uint32_t)(*(out_buf_ptr)) << 8);
                            i2s_tone_data = i2s_tone_data | ((uint32_t)(*(out_buf_ptr)) << 24);
                            out_buf_ptr++;
                        } else {    // stereo
                            i2s_tone_data = *((uint32_t *)out_buf_ptr);
                            out_buf_ptr+=4;
                        }

                        hal_i2s_tx_write(i2s_tone_data);
                    }

                    if (internal_handle->mp3_handle->CHNumber == 1) {
                        ring_buffer_read_done(&internal_handle->stream_out_pcm_buff, (stream_out_tone_amount * 2));
                    } else {
                        ring_buffer_read_done(&internal_handle->stream_out_pcm_buff, (stream_out_tone_amount * 4));
                    }

                    if ((i2s_amount_space_tones - stream_out_tone_amount) == 0) { // if first time i2s_amount_space_tones < stream_out_tone_amount, then this will avoid loop again
                        break;
                    }
                } else {
                    // stream_out_pcm_buff no data
                    log_hal_info("stream_out_pcm_buff no data\n");
                    break;
                }
            }

            if (ring_buffer_get_space_byte_count(&internal_handle->stream_out_pcm_buff) >= internal_handle->decode_pcm_buffer_size) {
                mp3_codec_event_send_from_isr(MP3_CODEC_QUEUE_EVENT_DECODE, NULL);
            }

            break;
    }
}
#endif

#if defined(MTK_I2S_ENABLED)
#else
static void mp3_codec_pcm_out_isr_callback(hal_audio_event_t event, void *data)
{
    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;
    uint32_t sample_count = 0;
    uint32_t stream_out_amount = 0;
    uint32_t loop_idx = 0;

    switch (event) {
        case HAL_AUDIO_EVENT_UNDERFLOW:
        case HAL_AUDIO_EVENT_DATA_REQUEST:

            //uint32_t loop_idx = 0;
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                uint8_t *out_buf_ptr    = NULL;
                uint32_t stream_out_pcm_buffer_data_count = 0;
                hal_audio_get_stream_out_sample_count(&sample_count);
                ring_buffer_get_read_information(&internal_handle->stream_out_pcm_buff, &out_buf_ptr, &stream_out_pcm_buffer_data_count);

                if (stream_out_pcm_buffer_data_count > 0) {
                    stream_out_amount = MINIMUM(sample_count, stream_out_pcm_buffer_data_count);
                    hal_audio_write_stream_out(out_buf_ptr, stream_out_amount);
                    ring_buffer_read_done(&internal_handle->stream_out_pcm_buff, stream_out_amount);

                    if ((sample_count - stream_out_amount) == 0) { // if first time sample_count < stream_out_pcm_buffer_data_count, then this will avoid loop again
                        break;
                    }
                } else {    /* 16kHz speech Tx buffer is empty */
                    // stream_out_pcm_buff no data
                    log_hal_info("stream_out_pcm_buff no data\n");
                    hal_audio_write_stream_out(out_buf_ptr, 0);     // must have this or the isr will not to request again
                    break;
                }

            }


            if (ring_buffer_get_space_byte_count(&internal_handle->stream_out_pcm_buff) >= internal_handle->decode_pcm_buffer_size) {
                mp3_codec_event_send_from_isr(MP3_CODEC_QUEUE_EVENT_DECODE, NULL);
            }

            break;
        case HAL_AUDIO_EVENT_ERROR:
        case HAL_AUDIO_EVENT_NONE:
        case HAL_AUDIO_EVENT_DATA_NOTIFICATION:
            break;
    }

}
#endif

#if 0
static void mp3_codec_fill_data_hisr_handler(void *data)
{
    log_hal_info("[MP3 Codec]mp3_codec_fill_data_hisr_handler\n");

    uint32_t sample_count = 0;
    uint32_t PCM_length = 0;
    uint32_t segment = 0;
    uint8_t *PCM_Src = NULL;
    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;
    PCM_Src = internal_handle->PCM_ring.buffer_base_pointer;
    PCMSmples = internal_handle->decHandle->PCMSamplesPerCH * internal_handle->decHandle->CHNumber * 2;
    log_hal_info("[MP3 Codec]PCMSmples=%u\n", PCMSmples);
    do {
        hal_audio_get_stream_out_sample_count(&sample_count);
        log_hal_info("[MP3 Codec]sample_count=%u\n", sample_count);
        segment = MINIMUM(PCM_length, sample_count);
        hal_audio_write_stream_out(PCM_Src, segment);
        PCM_length -= segment;
        PCM_Src += segment;
        //log_hal_info("[MP3 Codec]PCMSmples=%u\n",PCMSmples);
    } while (PCM_length != 0);

    //mp3_codec_event_send_from_isr(MP3_CODEC_QUEUE_EVENT_DECODE, NULL);
}
#endif

static void mp3_codec_deocde_hisr_handler(void *data)
{
    int32_t consumeBS = -1;
    uint8_t *share_buff_read_ptr = NULL;

    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;
    mp3_codec_media_handle_t *handle = &internal_handle->handle;


    //update read ptr to share buffer
    share_buff_read_ptr = handle->share_buff.buffer_base + handle->share_buff.read;


    //taskENTER_CRITICAL();
    // calculate mcps
    //CPU_RESET_CYCLECOUNTER;
    //__asm volatile("nop");
    //count_test = *DWT_CYCCNT;
    //offset = count_test - 1;
    //CPU_RESET_CYCLECOUNTER;


    /*   Deocde mp3 frame
        *   return: The consumed data size of Bitsream buffer for this  frame
      */
    consumeBS = MP3Dec_Decode(internal_handle->mp3_handle,
                              internal_handle->decode_pcm_buff.buffer_base_pointer,
                              handle->share_buff.buffer_base,
                              handle->share_buff.buffer_size,
                              share_buff_read_ptr);

    //count_test = *DWT_CYCCNT - offset;
    //log_hal_info("%d\n", count_test);
    //taskEXIT_CRITICAL();


    int32_t share_data_amount = mp3_codec_get_share_buffer_data_count(handle);

    if (consumeBS < 0) {
        log_hal_info("[MP3 Codec] Invalid return , consumeBS= %d\n", (int)consumeBS);
        //todo, error frame handle
    } else if (consumeBS >= 0 && consumeBS <= share_data_amount) {
        //update read index to share buffer
        handle->share_buff.read += consumeBS;
        if (handle->share_buff.read >= handle->share_buff.buffer_size) {    // although share buffer is a ring buffer, the mp3 decoder ip only code to the end of buffer at most.
            handle->share_buff.read -= handle->share_buff.buffer_size;
        }
    } else if (consumeBS > share_data_amount) {
        consumeBS = share_data_amount;  // It strange here, in fact, mp3 SWIP consumBS must not > share_data_amount. we met consumBS > share_data_amount in playing combine two songs in one mp3 file, so just workaround here.
        handle->share_buff.read += consumeBS;
        if (handle->share_buff.read >= handle->share_buff.buffer_size) {    // although share buffer is a ring buffer, the mp3 decoder ip only code to the end of buffer at most.
            handle->share_buff.read -= handle->share_buff.buffer_size;
        }

        uint32_t decoded_data_count = internal_handle->mp3_handle->PCMSamplesPerCH * internal_handle->mp3_handle->CHNumber * 2;

        decoded_data_count = ((int32_t)decoded_data_count) < 0 ? 0 : decoded_data_count;

        memset(internal_handle->decode_pcm_buff.buffer_base_pointer, 0, decoded_data_count);
    } else {
        log_hal_info("[MP3 Codec] never hear\n");
    }


    if ((internal_handle->mp3_handle->CHNumber != -1) && (internal_handle->mp3_handle->sampleRateIndex != -1)) {
        uint32_t loop_idx = 0;
        uint32_t decoded_data_count = internal_handle->mp3_handle->PCMSamplesPerCH * internal_handle->mp3_handle->CHNumber * 2;
        uint32_t decoded_data_index = 0;
        for (loop_idx = 0; loop_idx < 2; loop_idx++) {
            uint8_t *out_buf_ptr    = NULL;
            uint32_t out_byte_count = 0;
            ring_buffer_get_write_information(&internal_handle->stream_out_pcm_buff, &out_buf_ptr, &out_byte_count);
            if (out_byte_count > 0) {
                uint32_t consumed_byte_count = MINIMUM(decoded_data_count, out_byte_count);
                uint8_t *p_in_base         = internal_handle->decode_pcm_buff.buffer_base_pointer;
                uint8_t *p_in_buf          = p_in_base + decoded_data_index;
                memcpy(out_buf_ptr, p_in_buf, consumed_byte_count);
                decoded_data_index += consumed_byte_count;
                decoded_data_count  -= consumed_byte_count;
                ring_buffer_write_done(&internal_handle->stream_out_pcm_buff, consumed_byte_count);
            } else {
                // stream_out_pcm_buffer full, do nothing.
                break;
            }
        }
    }

    if (mp3_codec_get_share_buffer_data_count(handle) <= 1566) {    // share buufer less than decode bs * 3, we request user to fill data
        mp3_codec_request_data_to_share_buffer(handle);
    }

    if (ring_buffer_get_space_byte_count(&internal_handle->stream_out_pcm_buff) >= internal_handle->decode_pcm_buffer_size) {
        mp3_codec_event_send_from_isr(MP3_CODEC_QUEUE_EVENT_DECODE, NULL);
    }
}




static mp3_codec_function_return_state_t mp3_codec_skip_id3v2_and_reach_next_frame(mp3_codec_media_handle_t *handle, uint32_t file_size)
{
    uint32_t auido_frame_header = 0;
    if (mp3_codec_get_share_buffer_data_count(handle) < ID3V2_HEADER_LENGTH) {
        return MP3_CODEC_RETURN_ERROR;
    }

    mp3_codec_get_id3v2_info_and_skip(handle, file_size);

    if (mp3_codec_reach_next_frame_and_get_audio_frame_header(handle, &auido_frame_header, 2048, 0) != MP3_CODEC_RETURN_OK) {
        return MP3_CODEC_RETURN_ERROR;
    }

    return MP3_CODEC_RETURN_OK;
}

static mp3_codec_function_return_state_t mp3_codec_play_internal(mp3_codec_media_handle_t *handle)
{
    int32_t consumeBS = -1;
    uint8_t *share_buff_read_ptr = NULL;
    int16_t max_loop_times = 20;    // arbitrarily selected

    log_hal_info("[MP3 Codec] mp3_codec_play_internal ++\n");


    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;


    mp3_codec_event_register_callback(MP3_CODEC_QUEUE_EVENT_DECODE, mp3_codec_deocde_hisr_handler);

#if defined(MTK_I2S_ENABLED)

#else
    //lock sleep
    audio_service_setflag(handle->audio_id);
#endif

    do {
        //update read ptr to share buffer
        share_buff_read_ptr = handle->share_buff.buffer_base + handle->share_buff.read;


        consumeBS = MP3Dec_Decode(internal_handle->mp3_handle,
                                  internal_handle->decode_pcm_buff.buffer_base_pointer,
                                  handle->share_buff.buffer_base,
                                  handle->share_buff.buffer_size,
                                  share_buff_read_ptr);


        if (consumeBS < 0) {
            log_hal_info("[MP3 Codec] Invalid return , consumeBS= %d\n", (int)consumeBS);
        }

        if ((internal_handle->mp3_handle->CHNumber != -1) && (internal_handle->mp3_handle->sampleRateIndex != -1)) {
            {
                //update read index to share buffer
                handle->share_buff.read += consumeBS;
                if (handle->share_buff.read >= handle->share_buff.buffer_size) {
                    handle->share_buff.read -= handle->share_buff.buffer_size;
                }
            }

            uint32_t loop_idx = 0;
            uint32_t decoded_data_count = internal_handle->mp3_handle->PCMSamplesPerCH * internal_handle->mp3_handle->CHNumber * 2;
            uint32_t decoded_data_index = 0;

            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                uint8_t *out_buf_ptr    = NULL;
                uint32_t out_byte_count = 0;
                ring_buffer_get_write_information(&internal_handle->stream_out_pcm_buff, &out_buf_ptr, &out_byte_count);
                if (out_byte_count > 0) {
                    uint32_t consumed_byte_count = MINIMUM(decoded_data_count, out_byte_count);
                    uint8_t *p_in_base         = internal_handle->decode_pcm_buff.buffer_base_pointer;
                    uint8_t *p_in_buf          = p_in_base + decoded_data_index;
                    memcpy(out_buf_ptr, p_in_buf, consumed_byte_count);
                    decoded_data_index += consumed_byte_count;
                    decoded_data_count  -= consumed_byte_count;
                    ring_buffer_write_done(&internal_handle->stream_out_pcm_buff, consumed_byte_count);
                } else {
                    // stream_out_pcm_buffer full, do nothing.
                    break;
                }

            }



        }

        max_loop_times--;

        if (max_loop_times < 0) {
            return MP3_CODEC_RETURN_ERROR;
        }
    } while (ring_buffer_get_space_byte_count(&internal_handle->stream_out_pcm_buff) >= 4608);

    mp3_codec_request_data_to_share_buffer(handle); // since we decoded some data, thus we request again to fill share buffer.


#if defined(MTK_I2S_ENABLED)
    hal_i2s_status_t result = HAL_I2S_STATUS_OK;

    //hal_i2s_sample_rate_t hal_i2s_sample_rate = mp3_codec_translate_decoder_ip_sample_rate_index_to_i2s_index((uint16_t)internal_handle->mp3_handle->sampleRateIndex);
    //hal_i2s_channel_number_t hal_i2s_channel_number = mp3_codec_translate_decoder_ip_channel_number_to_i2s_channel_number(internal_handle->mp3_handle->CHNumber);

    hal_i2c_port_t i2c_port;
    hal_i2c_frequency_t frequency;
    i2c_port = HAL_I2C_MASTER_0;
    frequency = HAL_I2C_FREQUENCY_50K;
    AUCODEC_SAMPLERATE_SEL_e mp3_sampling_rate = mp3_codec_translate_decoder_ip_sample_rate_index_to_nau8810_index((uint16_t)internal_handle->mp3_handle->sampleRateIndex);
    AUCODEC_BCLK_RATE_e bclk = mp3_codec_translate_decoder_ip_channel_number_to_nau8810_bclk(internal_handle->mp3_handle->CHNumber);

    aucodec_i2c_init(i2c_port, frequency); //init codec
    aucodec_softreset();//soft reset
    aucodec_init();
    aucodec_set_dai_fmt(eI2S, e16Bit, eBCLK_NO_INV);//set DAI format

    log_hal_info("mp3_sampling_rate=%d, bclk=%d\r\n", mp3_sampling_rate, bclk);

    aucodec_set_dai_sysclk(mp3_sampling_rate, eMASTER, bclk, 16000000, ePLLEnable);

    aucodec_set_output(eSpkOut);//Input: DACIN, Output:  speaker out
    aucodec_set_output(eLineOut);//Input: DACIN, Output:  aux out


    if (i2s_configure(HAL_I2S_STEREO, HAL_I2S_SAMPLE_RATE_48K) < 0) { // in slave mode seems, channel number and sample rate is no use
        log_hal_info("[MP3 Codec] i2s_configure failed\r\n");
    }

    result = hal_i2s_setup_tx_vfifo(i2s_tx_buffer, I2S_TX_BUFFER_LENGTH / 2, I2S_TX_BUFFER_LENGTH);
    if (HAL_I2S_STATUS_OK != result) {
        log_hal_info("[MP3 Codec] hal_i2s_setup_tx_vfifo failed\r\n");
        return -1;
    }


    hal_i2s_register_tx_vfifo_callback(mp3_codec_i2s_tx_isr_callback, NULL);

    /* enable dma interrupt */
    hal_i2s_enable_tx_dma_interrupt();

    mp3_codec_i2s_open();
#else

    hal_audio_set_stream_out_sampling_rate(mp3_codec_translate_decoder_ip_sample_rate_index((uint16_t)internal_handle->mp3_handle->sampleRateIndex));
    hal_audio_set_stream_out_channel_number(mp3_codec_translate_decoder_ip_channel_number(internal_handle->mp3_handle->CHNumber));

    hal_audio_register_stream_out_callback(mp3_codec_pcm_out_isr_callback, NULL);
    hal_audio_set_memory(&audio_internal_ring_buffer);

    // We don't put data (using hal_audio_write_stream_out) first, since we want  stream_out_pcm_buff have maximum amount of data

    hal_audio_start_stream_out(HAL_AUDIO_PLAYBACK_MUSIC);
#endif

    handle->state = MP3_CODEC_STATE_PLAY;


    log_hal_info("[MP3 Codec] mp3_codec_play_internal --\n");
    return MP3_CODEC_RETURN_OK;
}


static mp3_codec_function_return_state_t mp3_codec_resume(mp3_codec_media_handle_t *handle)
{
    if (handle->state != MP3_CODEC_STATE_PAUSE) {
        return MP3_CODEC_RETURN_ERROR;
    }

#if defined(MTK_I2S_ENABLED)
    hal_i2s_enable_tx_dma_interrupt();

    hal_i2s_enable_tx();
    hal_i2s_enable_audio_top();

#else
    hal_audio_start_stream_out(HAL_AUDIO_PLAYBACK_MUSIC);
#endif

    handle->state = MP3_CODEC_STATE_PLAY;
    return MP3_CODEC_RETURN_OK;
}

static mp3_codec_function_return_state_t mp3_codec_pause(mp3_codec_media_handle_t *handle)
{
    if (handle->state != MP3_CODEC_STATE_PLAY) {
        return MP3_CODEC_RETURN_ERROR;
    }

#if defined(MTK_I2S_ENABLED)
    hal_i2s_disable_tx_dma_interrupt();

    hal_i2s_disable_tx();
    hal_i2s_disable_audio_top();
#else
    hal_audio_stop_stream_out();
#endif

    handle->state = MP3_CODEC_STATE_PAUSE;
    return MP3_CODEC_RETURN_OK;
}

static mp3_codec_function_return_state_t mp3_codec_play(mp3_codec_media_handle_t *handle)
{
    if (handle->state != MP3_CODEC_STATE_READY && handle->state != MP3_CODEC_STATE_STOP) {
        return MP3_CODEC_RETURN_ERROR;
    }

    mp3_codec_internal_handle_t *internal_handle = mp3_codec_internal_handle;

    internal_handle->mp3_handle = MP3Dec_Init(internal_handle->working_buff1, internal_handle->working_buff2);  // must do this, or SW mp3 IP will work wrong
    if (internal_handle->mp3_handle == NULL) {
        return MP3_CODEC_RETURN_ERROR;
    }

    return mp3_codec_play_internal(handle);
}


static mp3_codec_function_return_state_t mp3_codec_stop(mp3_codec_media_handle_t *handle)
{
    //if (handle->state != MP3_CODEC_STATE_PLAY && handle->state != MP3_CODEC_STATE_PAUSE) {
    //    return MP3_CODEC_RETURN_ERROR;
    //}

#if defined(MTK_I2S_ENABLED)
    mp3_codec_i2s_close();
    aucodec_i2c_deinit();
#else
    hal_audio_stop_stream_out();
#endif
    mp3_codec_event_deregister_callback(MP3_CODEC_QUEUE_EVENT_DECODE);

#if defined(MTK_I2S_ENABLED)
#else
    audio_service_clearflag(handle->audio_id);
#endif

    mp3_codec_reset_share_buffer(handle);
    mp3_codec_reset_stream_out_pcm_buffer();    // if don't do this it will have residue data, and it will be played if you play again

    if (handle->state != MP3_CODEC_STATE_PLAY && handle->state != MP3_CODEC_STATE_PAUSE) {
        return MP3_CODEC_RETURN_ERROR;
    }

    handle->state = MP3_CODEC_STATE_STOP;


    return MP3_CODEC_RETURN_OK;
}


mp3_codec_function_return_state_t mp3_codec_close(mp3_codec_media_handle_t *handle)
{
    mp3_codec_internal_handle_t *internal_handle = (mp3_codec_internal_handle_t *) handle;

    log_hal_info("[MP3 Codec]Close codec\n");
    if (handle->state != MP3_CODEC_STATE_STOP) {
        return MP3_CODEC_RETURN_ERROR;
    }
    handle->state = MP3_CODEC_STATE_IDLE;
#if defined(MTK_I2S_ENABLED)
#else
    audio_free_id(handle->audio_id);
#endif
    //vPortFree(internal_handle->memory_pool);  // since now we are using static memory, so can't free, but even using dynamic, i think free function also act at app site
    internal_handle->memory_pool = NULL;

    vPortFree(i2s_tx_buffer);
    i2s_tx_buffer = NULL;

    vPortFree(mp3_decode_buffer);
    mp3_decode_buffer = NULL;

    vPortFree(internal_handle);
    internal_handle = NULL;

    mp3_codec_internal_handle = NULL;
    return MP3_CODEC_RETURN_OK;
}


mp3_codec_media_handle_t *mp3_codec_open(mp3_codec_callback_t mp3_codec_callback)
{
    log_hal_info("[MP3 Codec]Open codec\n");

    mp3_codec_media_handle_t *handle;
    mp3_codec_internal_handle_t *internal_handle; /*internal handler*/

#if defined(MTK_I2S_ENABLED)
    uint16_t audio_id = 0;
#else
    //get audio id
    uint16_t audio_id = audio_get_id();

    if (audio_id > MAX_AUDIO_FUNCTIONS) {
        log_hal_info("[MP3 Codec]Audio ID > MAX AUDIO FUNCTIONS\n");
        return NULL;
    }
#endif

    /* alloc internal handler space */
    internal_handle = (mp3_codec_internal_handle_t *)pvPortMalloc(sizeof(mp3_codec_internal_handle_t));
    memset(internal_handle, 0, sizeof(mp3_codec_internal_handle_t));

    /* assign internal handler to be global and static handler*/
    mp3_codec_internal_handle = internal_handle;

    /* initialize internal handle*/
    internal_handle->share_buff_size = 0;
    internal_handle->decode_pcm_buffer_size = 0;
    internal_handle->stream_out_pcm_buff_size = 0;
    internal_handle->working_buff1_size = 0;
    internal_handle->working_buff2_size = 0;
    i2s_tx_buffer = pvPortMalloc(sizeof(uint32_t) * I2S_TX_BUFFER_LENGTH);
    if (i2s_tx_buffer == NULL) {
        log_hal_error("[MP3 Codec] i2s_tx_buffer cannot allocate memory\r\n");
        return NULL;
    } else {
        log_hal_info("[MP3 Codec] i2s_tx_buffer = 0x%X\r\n", i2s_tx_buffer);
    }
    mp3_decode_buffer = pvPortMalloc(sizeof(uint8_t) * MP3_DECODE_BUFFER_LENGTH);
    if (mp3_decode_buffer == NULL) {
        log_hal_error("[MP3 Codec] mp3_decode_buffer cannot allocate memory\r\n");
        return NULL;
    } else {
        log_hal_info("[MP3 Codec] mp3_decode_buffer = 0x%X\r\n", mp3_decode_buffer);
    }

    handle = &internal_handle->handle;
    handle->audio_id = audio_id;
    handle->handler  = mp3_codec_callback;
    handle->play     = mp3_codec_play;
    handle->pause    = mp3_codec_pause;
    handle->resume   = mp3_codec_resume;
    handle->stop     = mp3_codec_stop;
    handle->close_codec = mp3_codec_close;
    handle->skip_id3v2_and_reach_next_frame = mp3_codec_skip_id3v2_and_reach_next_frame;
    handle->state    = MP3_CODEC_STATE_READY;
    mp3_codec_buffer_function_init(handle);


    mp3_codec_queue_handle = xQueueCreate(MP3_CODEC_QUEUE_LENGTH, sizeof(mp3_codec_queue_event_t));
    {   /* Initialize queue registration */
        uint32_t id_idx;
        for (id_idx = 0; id_idx < MAX_MP3_CODEC_FUNCTIONS; id_idx++) {
            mp3_codec_queue_id_array[id_idx] = MP3_CODEC_QUEUE_EVENT_NONE;
        }
    }

    //create decode task
    mp3_codec_task_create();

    return handle;
}
