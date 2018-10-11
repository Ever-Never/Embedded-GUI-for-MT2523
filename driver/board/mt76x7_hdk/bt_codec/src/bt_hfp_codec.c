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

#include <stdio.h>

#include "bt_codec.h"

#include "hal_i2s.h"
#include "hal_platform.h"

#include "bt_hfp.h"
#include "hal_gpt.h"
#include "mt7687_cm4_hw_memmap.h"
#include "hal_nvic.h"
#include "hal_log.h"
#include "FreeRTOS.h"
#include "syslog.h"



/*
N9 Address                    CM4 Address
0x00404000  Tx buffer[0][v]   0x25004000
0x004040F0  Tx buffer[1][v]   0x250040F0
0x004041E0  Tx buffer[2]      0x250041E0
0x004042D0  Tx buffer[3]      0x250042D0
0x004043C0  Tx buffer[4][v]   0x250043C0
0x004044B0  Tx buffer[5][v]   0x250044B0
0x004045A0  Tx buffer[6]      0x250045A0
0x00404690  Tx buffer[7]      0x25004690
0x00404780  Rx buffer[0][v]   0x25004780
0x00404870  Rx buffer[1][v]   0x25004870
0x00404960  Rx buffer[2]      0x25004960
0x00404A50  Rx buffer[3]      0x25004A50
0x00404B40  Rx buffer[4][v]   0x25004B40
0x00404C30  Rx buffer[5][v]   0x25004C30
0x00404D20  Rx buffer[6]      0x25004D20
0x00404E10  Rx buffer[7]      0x25004E10

0. Each buffer size is 240 bytes.
1. N9 will copy one Tx buffer to TX_Path_AirSCO_this_T_eSCO every 7.5ms.
2. N9 will interrupt CM4 every 15ms.
3. CM4 have to update two Tx buffer in ISR handle.
   a. Tx buffer[0]&[1]     < 1st ISR, for ping pong         // note: in CVSD it may Tx buffer[0]    < 1st ISR, for ping pong
   b. Tx buffer[4]&[5]     < 2nd ISR, for ping pong         // note: in CVSD it may Tx buffer[4]    < 2nd ISR, for ping pong
*/

#define BT_HFP_TX_N9_EACH_BUFFER_SIZE  240
#define BT_HFP_TX_N9_EACH_ISR_BUFFER_SIZE  BT_HFP_TX_N9_EACH_BUFFER_SIZE * 2
#define BT_HFP_TX_FOR_BT_RING_BUFFER_CONTAIN_N9_EACH_ISR_BUFFER_AMOUNT   2 * 2   //ping pong (Tx buffer[0]&[1], Tx buffer[4]&[5])* let it can contain 2 times ping pong
#define BT_HFP_TX_FOR_BT_RING_BUFFER_SIZE   BT_HFP_TX_N9_EACH_ISR_BUFFER_SIZE * BT_HFP_TX_FOR_BT_RING_BUFFER_CONTAIN_N9_EACH_ISR_BUFFER_AMOUNT // one buffer size * 2 buffer(Tx buffer[0]&[1]) * ping pong (Tx buffer[0]&[1], Tx buffer[4]&[5])* let it can contain 2 times ping pong
#define BT_HFP_TX_CVSD_N9_EACH_ISR_BUFFER_SIZE  BT_HFP_TX_N9_EACH_BUFFER_SIZE
#define BT_HFP_TX_CVSD_FOR_BT_RING_BUFFER_CONTAIN_N9_EACH_ISR_BUFFER_AMOUNT   2 * 2   //ping pong (Tx buffer[0], Tx buffer[4])* let it can contain 2 times ping pong
#define BT_HFP_TX_CVSD_FOR_BT_RING_BUFFER_SIZE   BT_HFP_TX_CVSD_N9_EACH_ISR_BUFFER_SIZE * BT_HFP_TX_CVSD_FOR_BT_RING_BUFFER_CONTAIN_N9_EACH_ISR_BUFFER_AMOUNT // one buffer size * buffer(Tx buffer[0]) * ping pong (Tx buffer[0], Tx buffer[4])* let it can contain 2 times ping pong

#define BT_HFP_RX_N9_EACH_BUFFER_SIZE  240
#define BT_HFP_RX_N9_EACH_ISR_BUFFER_SIZE  BT_HFP_RX_N9_EACH_BUFFER_SIZE * 2
#define BT_HFP_RX_FOR_BT_RING_BUFFER_CONTAIN_N9_EACH_ISR_BUFFER_AMOUNT   2 * 2   //ping pong (Tx buffer[0]&[1], Tx buffer[4]&[5])* let it can contain 2 times ping pong
#define BT_HFP_RX_FOR_BT_RING_BUFFER_SIZE   BT_HFP_RX_N9_EACH_ISR_BUFFER_SIZE * BT_HFP_RX_FOR_BT_RING_BUFFER_CONTAIN_N9_EACH_ISR_BUFFER_AMOUNT // one buffer size * 2 buffer(Tx buffer[0]&[1]) * ping pong (Tx buffer[0]&[1], Tx buffer[4]&[5])* let it can contain 2 times ping pong
#define BT_HFP_RX_CVSD_N9_EACH_ISR_BUFFER_SIZE  BT_HFP_RX_N9_EACH_BUFFER_SIZE
#define BT_HFP_RX_CVSD_FOR_BT_RING_BUFFER_CONTAIN_N9_EACH_ISR_BUFFER_AMOUNT   2 * 2   //ping pong (Tx buffer[0], Tx buffer[4])* let it can contain 2 times ping pong
#define BT_HFP_RX_CVSD_FOR_BT_RING_BUFFER_SIZE   BT_HFP_RX_CVSD_N9_EACH_ISR_BUFFER_SIZE * BT_HFP_RX_CVSD_FOR_BT_RING_BUFFER_CONTAIN_N9_EACH_ISR_BUFFER_AMOUNT // one buffer size * buffer(Tx buffer[0]) * ping pong (Tx buffer[0], Tx buffer[4])* let it can contain 2 times ping pong


#define I2S_TX_VFIFO_LENGTH   960   // 960 tones =  2(I2S_TX_VFIFO_LENGTH/2). (I2S_TX_VFIFO_LENGTH/2)=480,    480 tone = 30ms will interrupt in 16k sampling rate
#define I2S_RX_VFIFO_LENGTH   960   // 960 tones =  2(I2S_TX_VFIFO_LENGTH/2). (I2S_TX_VFIFO_LENGTH/2)=480,    480 tone = 30ms will interrupt in 16k sampling rate
#define I2S_TX_CVSD_VFIFO_LENGTH   480   // 480 tones =  2(I2S_TX_VFIFO_LENGTH/2). (I2S_TX_VFIFO_LENGTH/2)=240,    240 tone = 30ms will interrupt in 8k sampling rate
#define I2S_RX_CVSD_VFIFO_LENGTH   480   // 480 tones =  2(I2S_TX_VFIFO_LENGTH/2). (I2S_TX_VFIFO_LENGTH/2)=240,    240 tone = 30ms will interrupt in 8k sampling rate


#define BT_HFP_PING_PONG_INDEX_FIRST 0
#define BT_HFP_PING_PONG_INDEX_SECOND 1

#define BT_HFP_TRUE     1
#define BT_HFP_FALSE    0

#define MINIMUM(a,b)            ((a) < (b) ? (a) : (b))

#define BT_TX_RINGBUFFER_FULL_THROW_AWAY_ADDITION_TONES  100    // This amount is we tried. Large amount will have more buffer to avoid meeting the case if(i2s_rx_amount_tones > bt_hfp_tx_for_bt_ring_buffer_space_tone)

#define TASK_ERROR_LOGGING
/* #define TASK_WARNING_LOGGING      */
/* #define TASK_PROCESS_LOGGING      */
#define TASK_CONTROL_LOGGING


//#define BT_HFP_CODEC_DEBUG_MESSAGE_FLAG

log_create_module(BT_HFP_CODEC, PRINT_LEVEL_INFO);

#define TASK_LOG_CTRL_ISR(fmt, arg...)   do { LOG_I(BT_HFP_CODEC, "[HFP CODEC]: "fmt,##arg); } while (0)

#if defined(TASK_ERROR_LOGGING)
#define TASK_LOG_E(fmt, arg...)      do { LOG_E(BT_HFP_CODEC, "[HFP CODEC]: "fmt,##arg); } while (0)
#else
#define TASK_LOG_E(fmt, arg...)
#endif

#if defined(TASK_WARNING_LOGGING)
#define TASK_LOG_W(fmt, arg...)      do { LOG_W(BT_HFP_CODEC, "[HFP CODEC]: "fmt,##arg); } while (0)
#else
#define TASK_LOG_W(fmt, arg...)
#endif

#if defined(TASK_PROCESS_LOGGING)
#define TASK_LOG_I(fmt, arg...)      do { LOG_I(BT_HFP_CODEC, "[HFP CODEC]: "fmt,##arg); } while (0)
#else
#define TASK_LOG_I(fmt, arg...)
#endif

#if defined(TASK_CONTROL_LOGGING)
#define TASK_LOG_CTRL(fmt, arg...)   do { LOG_I(BT_HFP_CODEC, "[HFP CODEC]: "fmt,##arg); } while (0)
#else
#define TASK_LOG_CTRL(fmt, arg...)
#endif


#define RETURN_NULL_IF_FALSE(expr)  do {            \
    if (!(expr)) {                                  \
        TASK_LOG_E("EXCEPTION, return null\r\n");   \
        return NULL;                                \
    }                                               \
} while(0)

#define RETURN_MEDIA_ERROR_IF_FALSE(expr)  do {             \
        if (!(expr)) {                                          \
            TASK_LOG_E("EXCEPTION, return media error\r\n");    \
            return BT_CODEC_MEDIA_STATUS_ERROR;                       \
        }                                                       \
} while(0)


typedef enum {
    BT_HFP_MODE_NONE = 0,
    BT_HFP_MODE_SPEECH,
    BT_HFP_MODE_TX_ONLY,
    BT_HFP_MODE_RX_ONLY,
    BT_HFP_MODE_LOOPBACK_WITH_CODEC,
    BT_HFP_MODE_LOOPBACK_WITHOUT_CODEC
} bt_hfp_mode_t;


typedef struct {
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t buffer_byte_count;
    uint8_t *buffer_base_pointer;
} ring_buffer_information_t;


typedef struct {
    uint32_t                    buffer_size;
    uint8_t                     *buffer_pointer;
    ring_buffer_information_t   for_bt_ring_buffer;
} bt_hfp_speech_tx_information_t;



typedef struct {
    uint32_t                     buffer_size;
    uint8_t                     *buffer_pointer;
    ring_buffer_information_t   for_bt_ring_buffer;
} bt_hfp_speech_rx_information_t;

typedef struct {
    bt_media_handle_t                  handle;
    bt_hfp_audio_codec_type_t          codec_type;
    bt_hfp_mode_t                      mode;
    hal_nvic_irq_t                     bt_nvic_irq_number;
    bt_hfp_speech_tx_information_t     tx_info;
    bt_hfp_speech_rx_information_t     rx_info;
    uint16_t                           last_bt_interrupt_tx_silence_tone;
    uint32_t                           *i2s_tx_vfifo_buffer;
    uint32_t                           *i2s_rx_vfifo_buffer;
    bool                               disable_downlink_path_flag;  // disable i2s tx path. note we can't disable bt rx path in hfp
} bt_hfp_codec_internal_handle_t;

static bt_hfp_codec_internal_handle_t *bt_hfp_codec_internal_handle = NULL;

static uint8_t bt_hfp_ping_pong_index = BT_HFP_PING_PONG_INDEX_SECOND;  // 0 the first ping pong buffer, 1 the second ping pong buffer. we fill the second ping pong buffer first
volatile uint32_t bt_n9_rx_buffer_current_offset = 0;
volatile uint32_t bt_n9_tx_buffer_current_offset = 0;


/*******************************************************************************\
| Private Functions                                                             |
\*******************************************************************************/

uint32_t ring_buffer_get_data_byte_count(ring_buffer_information_t *p_info)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t data_byte_count;
    if (write_pointer >= read_pointer) {
        data_byte_count = write_pointer - read_pointer;
    } else {
        data_byte_count = (buffer_byte_count << 1) - read_pointer + write_pointer;
    }
    return data_byte_count;
}

static uint32_t ring_buffer_get_space_byte_count(ring_buffer_information_t *p_info)
{
    return p_info->buffer_byte_count - ring_buffer_get_data_byte_count(p_info);
}

void ring_buffer_get_read_information(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t data_byte_count   = ring_buffer_get_data_byte_count(p_info);
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t tail_byte_count;
    if (read_pointer < buffer_byte_count) {
        *pp_buffer = buffer_pointer + read_pointer;
        tail_byte_count = buffer_byte_count - read_pointer;
    } else {
        *pp_buffer = buffer_pointer + read_pointer - buffer_byte_count;
        tail_byte_count = (buffer_byte_count << 1) - read_pointer;
    }
    *p_byte_count = MINIMUM(data_byte_count, tail_byte_count);
    return;
}

static void ring_buffer_get_write_information(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t space_byte_count  = ring_buffer_get_space_byte_count(p_info);
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t tail_byte_count;
    if (write_pointer < buffer_byte_count) {
        *pp_buffer = buffer_pointer + write_pointer;
        tail_byte_count = buffer_byte_count - write_pointer;
    } else {
        *pp_buffer = buffer_pointer + write_pointer - buffer_byte_count;
        tail_byte_count = (buffer_byte_count << 1) - write_pointer;
    }
    *p_byte_count = MINIMUM(space_byte_count, tail_byte_count);
    return;
}

static void ring_buffer_write_done(ring_buffer_information_t *p_info, uint32_t write_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t buffer_end        = buffer_byte_count << 1;
    uint32_t write_pointer     = p_info->write_pointer + write_byte_count;
    p_info->write_pointer = write_pointer >= buffer_end ? write_pointer - buffer_end : write_pointer;
    return;
}

static void ring_buffer_read_done(ring_buffer_information_t *p_info, uint32_t read_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t buffer_end        = buffer_byte_count << 1;
    uint32_t read_pointer      = p_info->read_pointer + read_byte_count;
    p_info->read_pointer = read_pointer >= buffer_end ? read_pointer - buffer_end : read_pointer;
    return;
}


static uint32_t bt_codec_get_interrupt_time(void)
{
    /* Check interrupt interval */
    uint32_t curr_cnt = 0;
    hal_gpt_status_t result = hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &curr_cnt);
    if (HAL_GPT_STATUS_OK != result) {
        TASK_LOG_CTRL("gpt result %d\r\n", result);
    }
    return curr_cnt;
}

static bt_codec_media_status_t bt_hfp_msbc_cvsd_set_buffers(bt_hfp_speech_tx_information_t *p_tx_info, bt_hfp_speech_rx_information_t *p_rx_info, bt_hfp_audio_codec_type_t codec_type)
{
    uint32_t tx_buffer_size = 0;
    uint32_t rx_buffer_size = 0;
    uint8_t *tx_buffer_pointer;
    uint8_t *rx_buffer_pointer;


    {   /* Sum up to total internal buffer size */
        tx_buffer_size += (codec_type == BT_HFP_CODEC_TYPE_CVSD) ? BT_HFP_TX_CVSD_FOR_BT_RING_BUFFER_SIZE : BT_HFP_TX_FOR_BT_RING_BUFFER_SIZE;
        rx_buffer_size += (codec_type == BT_HFP_CODEC_TYPE_CVSD) ? BT_HFP_RX_CVSD_FOR_BT_RING_BUFFER_SIZE : BT_HFP_RX_FOR_BT_RING_BUFFER_SIZE;
    }
    {   /* Allocate total internal buffer */
        tx_buffer_pointer = (uint8_t *)pvPortMalloc(tx_buffer_size);
        if (tx_buffer_pointer == NULL) {
            TASK_LOG_E("bt_hfp_msbc_set_buffers: tx_buffer_pointer allocate fail\r\n");
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }

        p_tx_info->buffer_size    = tx_buffer_size;
        p_tx_info->buffer_pointer = tx_buffer_pointer;

        rx_buffer_pointer = (uint8_t *)pvPortMalloc(tx_buffer_size);
        if (rx_buffer_pointer == NULL) {
            TASK_LOG_E("bt_hfp_msbc_set_buffers: rx_buffer_pointer allocate fail\r\n");
            vPortFree(p_tx_info->buffer_pointer);
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }

        p_rx_info->buffer_size    = rx_buffer_size;
        p_rx_info->buffer_pointer = rx_buffer_pointer;
    }
    {   /* Assign internal buffer */
        p_tx_info->for_bt_ring_buffer.buffer_base_pointer = (uint8_t *)tx_buffer_pointer;
        p_tx_info->for_bt_ring_buffer.buffer_byte_count = (codec_type == BT_HFP_CODEC_TYPE_CVSD) ? BT_HFP_TX_CVSD_FOR_BT_RING_BUFFER_SIZE : BT_HFP_TX_FOR_BT_RING_BUFFER_SIZE;
        tx_buffer_pointer += (codec_type == BT_HFP_CODEC_TYPE_CVSD) ? BT_HFP_TX_CVSD_FOR_BT_RING_BUFFER_SIZE : BT_HFP_TX_FOR_BT_RING_BUFFER_SIZE;

        p_rx_info->for_bt_ring_buffer.buffer_base_pointer = (uint8_t *)rx_buffer_pointer;
        p_rx_info->for_bt_ring_buffer.buffer_byte_count = (codec_type == BT_HFP_CODEC_TYPE_CVSD) ? BT_HFP_RX_CVSD_FOR_BT_RING_BUFFER_SIZE : BT_HFP_RX_FOR_BT_RING_BUFFER_SIZE;
        rx_buffer_pointer += (codec_type == BT_HFP_CODEC_TYPE_CVSD) ? BT_HFP_RX_CVSD_FOR_BT_RING_BUFFER_SIZE : BT_HFP_RX_FOR_BT_RING_BUFFER_SIZE;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
}

static uint32_t bt_hfp_check_state(bt_codec_state_t ref_state)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
    uint32_t result = BT_HFP_FALSE;
    if (p_info != NULL) {
        bt_media_handle_t *handle = &p_info->handle;
        bt_codec_state_t cur_state = handle->state;
        if (cur_state == ref_state) {
            result = BT_HFP_TRUE;
        }
    }
    return result;
}


/* // finally, we decide using i2s polling mode not using callback mode
    // if we use callback mode, than we have to care the case ring_buffer_get_data_byte_count(rx_for_bt_ring_buffer) / 2. This is not in intuition
    // in polling mode, we can just put the data amount we have. if i2s have no data it will output silence
static void i2s_tx_callback(hal_i2s_event_t event, void *user_data)
{
    uint32_t loop_idx = 0;
    uint32_t i2s_tx_space_tones_amount;

    ring_buffer_information_t *rx_for_bt_ring_buffer = &bt_hfp_codec_internal_handle->rx_info.for_bt_ring_buffer;

    TASK_LOG_CTRL("[HFP]i2s_tx_callback \r\n");

    switch (event) {
        case HAL_I2S_EVENT_DATA_REQUEST:
            hal_i2s_disable_tx_dma_interrupt();

            hal_i2s_get_tx_sample_count(&i2s_tx_space_tones_amount);



// TODO: maybe we have to think if bt_hfp_rx_for_bt_ring_buffer_data_tone_amount is small



//            uint32_t bt_hfp_rx_bt_ring_buffer_data_tones = 0;

//            bt_hfp_rx_bt_ring_buffer_data_tones = ring_buffer_get_data_byte_count(rx_for_bt_ring_buffer) / 2; // one tone = two bytes.

//            if(i2s_tx_amount_tones > bt_hfp_rx_for_bt_ring_buffer_space_tone) {
                // if we don't throw away remaining data in i2s vfifo, it will contrinue interrupt.
//                log_hal_info("tx_for_bt_ring_buffer over flow, throw away stream in data, i2s_rx_amount_tones=%d, bt_hfp_tx_for_bt_ring_buffer_space_count=%d\r\n", i2s_rx_amount_tones, bt_hfp_tx_for_bt_ring_buffer_space_tone);
//                uint32_t i = 0;
//                uint32_t read_temp = 0;
//                for(i = 0; i < i2s_rx_amount_tones; i++) {
//                    hal_i2s_rx_read(&read_temp);
//                }
//           }



            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                uint8_t *in_buf_ptr = NULL;
                uint32_t bt_hfp_rx_for_bt_ring_buffer_data_count = 0;
                uint32_t i;
                uint32_t write_data_temp_uint32_t = 0;
                uint16_t write_data_temp_uint16_t = 0;

                ring_buffer_get_read_information(rx_for_bt_ring_buffer, &in_buf_ptr, &bt_hfp_rx_for_bt_ring_buffer_data_count);

                uint32_t bt_hfp_rx_for_bt_ring_buffer_data_tone_amount = bt_hfp_rx_for_bt_ring_buffer_data_count / 2;

                uint32_t i2s_tx_tone_amount = MINIMUM(i2s_tx_space_tones_amount, bt_hfp_rx_for_bt_ring_buffer_data_tone_amount);    // TODO: maybe we have to think if bt_hfp_rx_for_bt_ring_buffer_data_tone_amount is small

                for (i = 0; i < i2s_tx_tone_amount; i++) {

                    // hal_i2s_status_t hal_i2s_tx_write(uint32_t data);
                    // The data format for I2S TX is as shown below:
                    //              | <-             32 bits                   -> |
                    // Stereo    R[31:24]    R[23:16]    L[15:8]    L[7:0]
                    // Mono      8'b0         8'b0           L[15:8]    L[7:0]


                    uint16_t *in_buf_uint16_t_ptr = (uint16_t *)in_buf_ptr;
                    write_data_temp_uint16_t = *in_buf_uint16_t_ptr;
                    write_data_temp_uint32_t = (uint32_t)write_data_temp_uint16_t;

                    hal_i2s_tx_write(write_data_temp_uint32_t);

                    in_buf_ptr += 2;
                }

                ring_buffer_read_done(rx_for_bt_ring_buffer, (i2s_tx_tone_amount * 2));

                if ((i2s_tx_space_tones_amount - i2s_tx_tone_amount) == 0) { // if first time i2s_tx_space_tones_amount - i2s_tx_tone_amount = 0 means write tones done from i2s at first loop
                    break;
                } else {
                    i2s_tx_space_tones_amount -= i2s_tx_tone_amount;
                }
            }

            // TODO: maybe we can do bt_hfp_rx_for_bt_ring_buffer_data_tone_amount to less case here, just put slience to i2s tx.

            hal_i2s_enable_tx_dma_interrupt();
            break;
        case HAL_I2S_EVENT_DATA_NOTIFICATION:

            break;
    };
}
*/

static void i2s_rx_callback(hal_i2s_event_t event, void *user_data)
{
    uint32_t loop_idx = 0;
    uint32_t i2s_rx_amount_tones;

    ring_buffer_information_t *tx_for_bt_ring_buffer = &bt_hfp_codec_internal_handle->tx_info.for_bt_ring_buffer;

    //TASK_LOG_CTRL("[HFP]i2s_rx_callback \r\n");

    switch (event) {
        case HAL_I2S_EVENT_DATA_REQUEST:
            break;
        case HAL_I2S_EVENT_DATA_NOTIFICATION:
            //TASK_LOG_CTRL("[HFP]i2s_rx_callback 2\r\n");
            NVIC_DisableIRQ(bt_hfp_codec_internal_handle->bt_nvic_irq_number);  // disable bt interrupt to avoid bt interrupt prompt that may interrupt during get samples from i2s
            hal_i2s_disable_rx_dma_interrupt();

#if 0 // test record from mic and directly to earphone
            uint32_t tx_vfifo_free_count = 0;
            uint32_t rx_vfifo_data_count = 0;
            uint32_t consume_count = 0;
            uint32_t i = 0;
            uint32_t read_temp = 0;

            hal_i2s_get_tx_sample_count(&tx_vfifo_free_count);
            hal_i2s_get_rx_sample_count(&rx_vfifo_data_count);
            //printf("i2s_rx_callback, time=%d, rx_vfifo_data_count=%d\r\n", bt_codec_get_interrupt_time(), rx_vfifo_data_count);
            while (rx_vfifo_data_count != 0) {
                consume_count = MINIMUM(tx_vfifo_free_count, rx_vfifo_data_count);
                for (i = 0; i < consume_count; i++) {
                    hal_i2s_rx_read(&read_temp);
                    hal_i2s_tx_write(read_temp);
                }
                rx_vfifo_data_count -= consume_count;
                hal_i2s_get_tx_sample_count(&tx_vfifo_free_count);
            }
            printf("i2s_rx_callback, time=%d, rx_vfifo_data_count=%d\r\n", bt_codec_get_interrupt_time(), rx_vfifo_data_count);
#else


            hal_i2s_get_rx_sample_count(&i2s_rx_amount_tones);

#if defined(BT_HFP_CODEC_DEBUG_MESSAGE_FLAG)
            {
                uint32_t curr_cnt=0;
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
                TASK_LOG_CTRL_ISR("++: curr_cnt=%d, tx_for_bt_ring_buffer data byte=%d, tx_for_bt_ring_buffer space byte=%d; i2s_rx_amount_tones=%d\r\n", curr_cnt, ring_buffer_get_data_byte_count(tx_for_bt_ring_buffer), ring_buffer_get_space_byte_count(tx_for_bt_ring_buffer), i2s_rx_amount_tones);
            }
#endif
            uint32_t bt_hfp_tx_for_bt_ring_buffer_space_tone = 0;
            bt_hfp_tx_for_bt_ring_buffer_space_tone = ring_buffer_get_space_byte_count(tx_for_bt_ring_buffer) / 2;

            if(i2s_rx_amount_tones > bt_hfp_tx_for_bt_ring_buffer_space_tone) {
                // if tx_for_bt_ring_buffer over flow we will throw away old tones got before and keep get new tone to tx_for_bt_ring_buffer.
                uint32_t throw_away_tones = ((i2s_rx_amount_tones - bt_hfp_tx_for_bt_ring_buffer_space_tone + BT_TX_RINGBUFFER_FULL_THROW_AWAY_ADDITION_TONES) >=  i2s_rx_amount_tones) ? i2s_rx_amount_tones : (i2s_rx_amount_tones - bt_hfp_tx_for_bt_ring_buffer_space_tone + BT_TX_RINGBUFFER_FULL_THROW_AWAY_ADDITION_TONES);  // This amount is we tried. Large amount will have more buffer to avoid meeting the case if(i2s_rx_amount_tones > bt_hfp_tx_for_bt_ring_buffer_space_tone)
                uint32_t throw_away_bytes = throw_away_tones * sizeof(uint16_t);
                uint32_t remained_data_bytes = ring_buffer_get_data_byte_count(tx_for_bt_ring_buffer);
                throw_away_bytes = MINIMUM(throw_away_bytes, remained_data_bytes);  // here we have asume tx_for_bt_ring_buffer size > I2S_TX_VFIFO_LENGTH. Thus when we throw away with amount remained_data_bytes, the space will > I2S_TX_VFIFO_LENGTH. This assume is reasonable.
                ring_buffer_read_done(tx_for_bt_ring_buffer, throw_away_bytes);
                TASK_LOG_CTRL_ISR("tx_for_bt_ring_buffer over flow throw old tones, space tone before=%d, now space tone =%d, i2s_rx_amount_tones=%d\r\n", bt_hfp_tx_for_bt_ring_buffer_space_tone, ring_buffer_get_space_byte_count(tx_for_bt_ring_buffer) / 2, i2s_rx_amount_tones);
            }


            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                uint8_t *out_buf_ptr = NULL;
                uint32_t bt_hfp_tx_for_bt_ring_buffer_data_count = 0;
                uint32_t i;
                uint32_t read_data_temp_uint32_t = 0;
                uint16_t read_data_temp_uint16_t = 0;

                ring_buffer_get_write_information(tx_for_bt_ring_buffer, &out_buf_ptr, &bt_hfp_tx_for_bt_ring_buffer_data_count);

                uint32_t bt_hfp_tx_for_bt_ring_buffer_data_tone_amount = bt_hfp_tx_for_bt_ring_buffer_data_count / 2;

                uint32_t bt_hfp_tx_tone_amount = MINIMUM(i2s_rx_amount_tones, bt_hfp_tx_for_bt_ring_buffer_data_tone_amount);

                for (i = 0; i < bt_hfp_tx_tone_amount; i++) {

                    // hal_i2s_status_t hal_i2s_rx_read(uint32_t *data);
                    // The data format for I2S TX is as shown below:
                    //              | <-             32 bits                   -> |
                    // Stereo    R[31:24]    R[23:16]    L[15:8]    L[7:0]
                    // Mono      8'b0         8'b0           L[15:8]    L[7:0]

                    hal_i2s_rx_read(&read_data_temp_uint32_t);
                    read_data_temp_uint16_t = (uint16_t)(read_data_temp_uint32_t & 0xffff);

                    uint16_t *out_buf_uint16_t_ptr = (uint16_t *)out_buf_ptr;

                    *out_buf_uint16_t_ptr = read_data_temp_uint16_t;

                    out_buf_ptr += 2;
                }

                ring_buffer_write_done(tx_for_bt_ring_buffer, (bt_hfp_tx_tone_amount * 2));

                if ((i2s_rx_amount_tones - bt_hfp_tx_tone_amount) == 0) { // if first time i2s_rx_amount_tones - bt_hfp_tx_tone_amount = 0 means read tone done from i2s at first loop
                    break;
                } else {
                    i2s_rx_amount_tones -= bt_hfp_tx_tone_amount;
                }
            }
#endif

#if defined(BT_HFP_CODEC_DEBUG_MESSAGE_FLAG)
            {
                hal_i2s_get_rx_sample_count(&i2s_rx_amount_tones);
                uint32_t curr_cnt=0;
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
                TASK_LOG_CTRL_ISR("--: curr_cnt=%d, tx_for_bt_ring_buffer data byte=%d, tx_for_bt_ring_buffer space byte=%d; i2s_rx_amount_tones=%d\r\n", curr_cnt, ring_buffer_get_data_byte_count(tx_for_bt_ring_buffer), ring_buffer_get_space_byte_count(tx_for_bt_ring_buffer), i2s_rx_amount_tones);
            }
#endif

            hal_i2s_enable_rx_dma_interrupt();
            NVIC_EnableIRQ(bt_hfp_codec_internal_handle->bt_nvic_irq_number);
            break;

    };
}

#if 0
volatile uint32_t rx_buffer_current_offset;
volatile uint32_t tx_buffer_current_offset;
volatile uint8_t buffer_type =1;
volatile int16_t *RX_audio_Buffer;//[120] = {0};
volatile uint32_t V2 =0 ;
volatile int16_t *TX_audio_Buffer;//[120] = {0};
volatile uint32_t buffer_size = 480;//copy 2 buffer
volatile uint32_t buffer_index = 0;

volatile uint32_t buffer_index2 = 0;
#endif

static void bt_hfp_bt_interrupt_callback(void)
{
    if (bt_hfp_ping_pong_index == BT_HFP_PING_PONG_INDEX_SECOND) {
        bt_n9_rx_buffer_current_offset = 0x25004B40;
        bt_n9_tx_buffer_current_offset = 0x250043C0;
        bt_hfp_ping_pong_index = BT_HFP_PING_PONG_INDEX_FIRST;
    } else {
        bt_n9_rx_buffer_current_offset = 0x25004780;
        bt_n9_tx_buffer_current_offset = 0x25004000;
        bt_hfp_ping_pong_index = BT_HFP_PING_PONG_INDEX_SECOND;
    }

#if defined(BT_HFP_CODEC_DEBUG_MESSAGE_FLAG)
    {
        ring_buffer_information_t *tx_for_bt_ring_buffer_temp = &bt_hfp_codec_internal_handle->tx_info.for_bt_ring_buffer;
        uint32_t curr_cnt=0;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
        uint32_t tx_vfifo_free_tones_temp = 0;
        hal_i2s_get_tx_sample_count(&tx_vfifo_free_tones_temp);
        uint32_t bt_n9_data_tones_temp = ((bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) ? BT_HFP_RX_CVSD_N9_EACH_ISR_BUFFER_SIZE : BT_HFP_RX_N9_EACH_ISR_BUFFER_SIZE) / 2;
        uint32_t i2s_rx_amount_tones = 0;
        hal_i2s_get_rx_sample_count(&i2s_rx_amount_tones);

        TASK_LOG_CTRL_ISR("+++: curr_cnt=%d, tx_for_bt_ring_buffer data byte=%d, tx_for_bt_ring_buffer space byte=%d; tx_vfifo_free_tones=%d, bt_n9_data_tones=%d, i2s_rx_amount_tones=%d\r\n", curr_cnt, ring_buffer_get_data_byte_count(tx_for_bt_ring_buffer_temp), ring_buffer_get_space_byte_count(tx_for_bt_ring_buffer_temp), tx_vfifo_free_tones_temp, bt_n9_data_tones_temp, i2s_rx_amount_tones);
    }
#endif

#if 0  // test bt rx to bt tx directly
    volatile uint8_t *bt_n9_tx_buffer;
    bt_n9_tx_buffer = (volatile uint8_t *)bt_n9_tx_buffer_current_offset;

    volatile uint8_t *bt_n9_rx_buffer;
    bt_n9_rx_buffer = (volatile uint8_t *)bt_n9_rx_buffer_current_offset;


    memcpy(bt_n9_tx_buffer, bt_n9_rx_buffer, BT_HFP_RX_N9_EACH_ISR_BUFFER_SIZE);
#endif

    {   // bt tx
        volatile uint8_t *bt_n9_tx_buffer;
        bt_n9_tx_buffer = (volatile uint8_t *)bt_n9_tx_buffer_current_offset;

        ring_buffer_information_t *tx_for_bt_ring_buffer = &bt_hfp_codec_internal_handle->tx_info.for_bt_ring_buffer;

        uint32_t temp_bt_hf_tx_n9_each_isr_buffer_size = (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) ? BT_HFP_TX_CVSD_N9_EACH_ISR_BUFFER_SIZE : BT_HFP_TX_N9_EACH_ISR_BUFFER_SIZE;

        if (ring_buffer_get_data_byte_count(tx_for_bt_ring_buffer) < temp_bt_hf_tx_n9_each_isr_buffer_size) {
            uint32_t i = 0;
            TASK_LOG_CTRL_ISR("[HFP]: bt_hfp_bt_interrupt_callback:: tx underflow. ring_buffer_get_data_byte_count(tx_for_bt_ring_buffer)=%d, temp_bt_hf_tx_n9_each_isr_buffer_size=%d\r\n", ring_buffer_get_data_byte_count(tx_for_bt_ring_buffer), temp_bt_hf_tx_n9_each_isr_buffer_size);
            for (i = 0; i < temp_bt_hf_tx_n9_each_isr_buffer_size; i++) {
                *bt_n9_tx_buffer++ = 0x0;
            }
        } else {
            uint32_t bt_n9_need_bytes = (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) ? BT_HFP_TX_CVSD_N9_EACH_ISR_BUFFER_SIZE : BT_HFP_TX_N9_EACH_ISR_BUFFER_SIZE;
            uint32_t filled_data_byte_count = 0;
            uint32_t loop_idx;
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                uint8_t *in_buf_ptr    = NULL;
                uint32_t tx_for_bt_ring_buffer_byte_count = 0;
                ring_buffer_get_read_information(tx_for_bt_ring_buffer, &in_buf_ptr, &tx_for_bt_ring_buffer_byte_count);

                uint32_t consumed_byte_cnt = MINIMUM(bt_n9_need_bytes, tx_for_bt_ring_buffer_byte_count);
                uint8_t *out_base_ptr         = (uint8_t *)bt_n9_tx_buffer;
                uint8_t *out_buf_ptr          = out_base_ptr + filled_data_byte_count;
                memcpy(out_buf_ptr, in_buf_ptr, consumed_byte_cnt);
                filled_data_byte_count  += consumed_byte_cnt;
                bt_n9_need_bytes -= consumed_byte_cnt;
                ring_buffer_read_done(tx_for_bt_ring_buffer, consumed_byte_cnt);
            }
        }

        uint32_t bt_hfp_tx_n9_each_isr_buffer_size_in_bytes = (bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) ? BT_HFP_TX_CVSD_N9_EACH_ISR_BUFFER_SIZE : BT_HFP_TX_N9_EACH_ISR_BUFFER_SIZE;
        bt_hfp_codec_internal_handle->last_bt_interrupt_tx_silence_tone = *((uint16_t *)(bt_n9_tx_buffer + bt_hfp_tx_n9_each_isr_buffer_size_in_bytes - 2));
    }

    {   // bt rx
        if (!bt_hfp_codec_internal_handle->disable_downlink_path_flag){
            volatile uint8_t *bt_n9_rx_buffer; // where we use word not using byte, since we want improve performance: get one tone at once
            bt_n9_rx_buffer = (volatile uint8_t *)bt_n9_rx_buffer_current_offset;

            uint16_t *in_buf_uint16_t_ptr = (uint16_t *)bt_n9_rx_buffer;
            uint32_t tx_vfifo_free_tones = 0;
            uint32_t consume_tones = 0;
            uint32_t i = 0;
            uint32_t write_data_temp_uint32_t = 0;
            uint16_t write_data_temp_uint16_t = 0;

            hal_i2s_get_tx_sample_count(&tx_vfifo_free_tones);

            uint32_t bt_n9_data_tones = ((bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) ? BT_HFP_RX_CVSD_N9_EACH_ISR_BUFFER_SIZE : BT_HFP_RX_N9_EACH_ISR_BUFFER_SIZE) / 2;

            if (bt_n9_data_tones > tx_vfifo_free_tones) {
                TASK_LOG_CTRL_ISR("[HFP]: bt_hfp_bt_interrupt_callback:: bt_n9_data_tones(%d) > tx_vfifo_free_tones(%d), throw some tones.\r\n", bt_n9_data_tones, tx_vfifo_free_tones);
            }


            consume_tones = MINIMUM(tx_vfifo_free_tones, bt_n9_data_tones);
            for (i = 0; i < consume_tones; i++) {
                write_data_temp_uint16_t = *in_buf_uint16_t_ptr;
                write_data_temp_uint32_t = (uint32_t)write_data_temp_uint16_t;

                hal_i2s_tx_write(write_data_temp_uint32_t);

                in_buf_uint16_t_ptr++;
            }
        }
    }


#if defined(BT_HFP_CODEC_DEBUG_MESSAGE_FLAG)
    {
        ring_buffer_information_t *tx_for_bt_ring_buffer_temp = &bt_hfp_codec_internal_handle->tx_info.for_bt_ring_buffer;
        uint32_t curr_cnt=0;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
        uint32_t tx_vfifo_free_tones_temp = 0;
        hal_i2s_get_tx_sample_count(&tx_vfifo_free_tones_temp);
        uint32_t bt_n9_data_tones_temp = ((bt_hfp_codec_internal_handle->codec_type == BT_HFP_CODEC_TYPE_CVSD) ? BT_HFP_RX_CVSD_N9_EACH_ISR_BUFFER_SIZE : BT_HFP_RX_N9_EACH_ISR_BUFFER_SIZE) / 2;
        uint32_t i2s_rx_amount_tones = 0;
        hal_i2s_get_rx_sample_count(&i2s_rx_amount_tones);
        TASK_LOG_CTRL_ISR("---: curr_cnt=%d, tx_for_bt_ring_buffer data byte=%d, tx_for_bt_ring_buffer space byte=%d; tx_vfifo_free_tones=%d, bt_n9_data_tones=%d, i2s_rx_amount_tones=%d\r\n", curr_cnt, ring_buffer_get_data_byte_count(tx_for_bt_ring_buffer_temp), ring_buffer_get_space_byte_count(tx_for_bt_ring_buffer_temp), tx_vfifo_free_tones_temp, bt_n9_data_tones_temp, i2s_rx_amount_tones);
    }
#endif

    HAL_REG_32(0x81080080) &= (~BIT(WDT_DUAL_RST_SW_INT_OFFSET));   //clear N9 CR 0x8108_0080[31]N9 to CM4 software interrupt



}

static void i2s_open(void)
{
    TASK_LOG_CTRL("[HFP]i2s_open \n");

    if (!bt_hfp_codec_internal_handle->disable_downlink_path_flag){
        hal_i2s_enable_tx();
    }

    hal_i2s_enable_rx();
    hal_i2s_enable_audio_top();
}

static int8_t i2s_configure(void)
{
    TASK_LOG_CTRL("[HFP]i2s_configure \n");
    hal_i2s_config_t i2s_config;
    hal_i2s_status_t result = HAL_I2S_STATUS_OK;

    result = hal_i2s_init(HAL_I2S_TYPE_EXTERNAL_MODE);
    if (HAL_I2S_STATUS_OK != result) {
        TASK_LOG_E("[HFP]i2s_configure: hal_i2s_init failed\r\n");
        return -1;
    }

    /* Configure I2S  */
    i2s_config.clock_mode = HAL_I2S_SLAVE;
    i2s_config.rx_down_rate = HAL_I2S_RX_DOWN_RATE_DISABLE;
    i2s_config.tx_mode = HAL_I2S_TX_MONO_DUPLICATE_DISABLE;
    i2s_config.i2s_out.channel_number = HAL_I2S_STEREO;

    i2s_config.i2s_out.sample_rate = HAL_I2S_SAMPLE_RATE_16K;   // in HAL_I2S_SLAVE mode, we don't have to care this sampling rate, it will be ignored
    i2s_config.i2s_in.sample_rate = HAL_I2S_SAMPLE_RATE_16K;   // in HAL_I2S_SLAVE mode, we don't have to care this sampling rate, it will be ignored
    i2s_config.i2s_in.msb_offset = 0;
    i2s_config.i2s_out.msb_offset = 0;
    i2s_config.i2s_in.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_DISABLE;
    i2s_config.i2s_out.word_select_inverse = HAL_I2S_WORD_SELECT_INVERSE_DISABLE;
    i2s_config.i2s_in.lr_swap = HAL_I2S_LR_SWAP_DISABLE;
    i2s_config.i2s_out.lr_swap = HAL_I2S_LR_SWAP_DISABLE;

    result = hal_i2s_set_config(&i2s_config);
    if (HAL_I2S_STATUS_OK != result) {
        TASK_LOG_E("hal_i2s_set_config failed\r\n");
        return -1;
    }

    return 1;
}

static bt_codec_media_status_t bt_hfp_play(bt_media_handle_t *handle)
{
    {
        uint32_t isr_time = bt_codec_get_interrupt_time();
        TASK_LOG_CTRL("+play, %ld\r\n", isr_time);
    }
    {   /* PLAY */
        bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
        bt_hfp_mode_t mode = p_info->mode;
        //bt_hfp_audio_codec_type_t codec_type = p_info->codec_type;
        /******************[HEAD] Check State *******************************************************/
        if (bt_hfp_check_state(BT_CODEC_STATE_READY) == BT_HFP_FALSE) {
            TASK_LOG_E("play not ready\r\n");
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }


        /******************[HEAD] Set PLAY State ****************************************************/
        handle->state = BT_CODEC_STATE_PLAY;
        /******************[TAIL] Set PLAY State ****************************************************/
        /******************[HEAD] Unmask BT Interrupt ***********************************************/
        HAL_REG_32(0x81080080) &= (~BIT(WDT_DUAL_RST_SW_INT_OFFSET));//clear N9 CR 0x8108_0080[31]N9 to CM4 software interrupt. in some race condition, hfp codec or N9 setted this flag (flag =1)and didn't clean, then when we play, N9 will set this bit flag, but it is no change 1->1, thus we can't get interrupt. although put this here may not a good place. 
        NVIC_EnableIRQ(p_info->bt_nvic_irq_number);
        /******************[TAIL] Unmask BT Interrupt ***********************************************/
        /******************[HEAD] I2S Start *****************************************************/
        if (mode == BT_HFP_MODE_SPEECH) {
            /* i2s config*/
            if (i2s_configure() < 0) {
                TASK_LOG_E("[HFP]bt_hfp_play: audio codec config fail\r\n");
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }

            uint32_t i2s_tx_vfifo_allocat_length = (p_info->codec_type == BT_HFP_CODEC_TYPE_CVSD) ? I2S_TX_CVSD_VFIFO_LENGTH : I2S_TX_VFIFO_LENGTH;
            uint32_t i2s_rx_vfifo_allocat_length = (p_info->codec_type == BT_HFP_CODEC_TYPE_CVSD) ? I2S_RX_CVSD_VFIFO_LENGTH : I2S_RX_VFIFO_LENGTH;

            p_info->i2s_tx_vfifo_buffer = (uint32_t *)pvPortMalloc(i2s_tx_vfifo_allocat_length * sizeof(uint32_t));
            if (p_info->i2s_tx_vfifo_buffer == NULL) {
                TASK_LOG_CTRL("bt_hfp_play: p_info->i2s_tx_vfifo_buffer = NULL, needed_memory_size=%d, remain heap size=%d\r\n", (I2S_TX_VFIFO_LENGTH * sizeof(uint32_t)), xPortGetFreeHeapSize());
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }

            p_info->i2s_rx_vfifo_buffer = (uint32_t *)pvPortMalloc(i2s_rx_vfifo_allocat_length * sizeof(uint32_t));
            if (p_info->i2s_rx_vfifo_buffer == NULL) {
                TASK_LOG_CTRL("bt_hfp_play: p_info->i2s_rx_vfifo_buffer = NULL, needed_memory_size=%d, remain heap size=%d\r\n", (I2S_RX_VFIFO_LENGTH * sizeof(uint32_t)), xPortGetFreeHeapSize());
                vPortFree(p_info->i2s_rx_vfifo_buffer);
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }


            if (hal_i2s_setup_tx_vfifo(p_info->i2s_tx_vfifo_buffer, i2s_tx_vfifo_allocat_length / 2, i2s_tx_vfifo_allocat_length) != HAL_I2S_STATUS_OK) {
                TASK_LOG_E("hal_i2s_setup_tx_vfifo failed\r\n");
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }

            if (hal_i2s_setup_rx_vfifo(p_info->i2s_rx_vfifo_buffer, i2s_rx_vfifo_allocat_length / 2, i2s_rx_vfifo_allocat_length) != HAL_I2S_STATUS_OK) {
                TASK_LOG_E("hal_i2s_setup_rx_vfifo failed\r\n");
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }

            //hal_i2s_register_tx_vfifo_callback(i2s_tx_callback, NULL);    // we decided using polling mode not callback mode
            hal_i2s_register_rx_vfifo_callback(i2s_rx_callback, NULL);


            TASK_LOG_CTRL(" play\r\n");
            /*
                        {
                            uint16_t device = afe_audio_get_output_device();
                            hal_speech_device_mode_t enh_mode;
                            if (device == HAL_AUDIO_DEVICE_HEADSET || device == HAL_AUDIO_DEVICE_HEADSET_MONO) {
                                enh_mode = SPH_MODE_BT_EARPHONE;
                            } else {
                                enh_mode = SPH_MODE_BT_LOUDSPEAKER;
                            }
                            speech_set_mode(enh_mode);
                        }
            */
            /* enable dma interrupt */
            hal_i2s_enable_rx_dma_interrupt();
            //hal_i2s_enable_tx_dma_interrupt();

            i2s_open();
            //speech_set_enhancement(true);
        }
        /******************[TAIL] I2S Start *****************************************************/
    }
    {
        uint32_t isr_time = bt_codec_get_interrupt_time();
        TASK_LOG_CTRL("-play, %ld\r\n", isr_time);
    }
    return BT_CODEC_MEDIA_STATUS_OK;
}

/* Example of i2s_close */
static void bt_hfp_i2s_close(void)
{
    hal_i2s_disable_rx();
    if (!bt_hfp_codec_internal_handle->disable_downlink_path_flag){
        hal_i2s_disable_tx();
    }
    hal_i2s_disable_audio_top();
    hal_i2s_deinit();
    hal_i2s_stop_rx_vfifo();
}

static bt_codec_media_status_t bt_hfp_stop(bt_media_handle_t *handle)
{
    {
        uint32_t isr_time = bt_codec_get_interrupt_time();
        TASK_LOG_CTRL("+stop, %ld\r\n", isr_time);
    }
    {   /* STOP */
        bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
        bt_hfp_mode_t mode = p_info->mode;
        /******************[HEAD] Check State *******************************************************/
        if (bt_hfp_check_state(BT_CODEC_STATE_PLAY) == BT_HFP_FALSE) {
            TASK_LOG_E("stop fail, state is not play\r\n");
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
        /******************[TAIL] Check State *******************************************************/
        /******************[HEAD] Set STOP State ****************************************************/
        handle->state = BT_CODEC_STATE_STOP;
        /******************[TAIL] Set STOP State ****************************************************/
        /******************[HEAD] PCM2WAY Stop ******************************************************/
        if (mode == BT_HFP_MODE_SPEECH) {
            //speech_set_enhancement(false);
            hal_i2s_disable_rx_dma_interrupt();
            bt_hfp_i2s_close();
            vPortFree(p_info->i2s_rx_vfifo_buffer);
            vPortFree(p_info->i2s_tx_vfifo_buffer);
            TASK_LOG_CTRL(" stop, hfp\r\n");
        }
        /******************[TAIL] PCM2WAY Stop ******************************************************/
        /******************[HEAD] Mask BT Interrupt *************************************************/
        NVIC_DisableIRQ(p_info->bt_nvic_irq_number);
        /******************[TAIL] Mask BT Interrupt *************************************************/
        /******************[HEAD] Unhook BT Interrupt Handler ***************************************/
        // NVIC has no unregister function
        /******************[TAIL] Unhook BT Interrupt Handler ***************************************/
        bt_hfp_codec_internal_handle->last_bt_interrupt_tx_silence_tone = 0;
    }
    {
        uint32_t isr_time = bt_codec_get_interrupt_time();
        TASK_LOG_CTRL("-stop, %ld\r\n", isr_time);
    }
    return BT_CODEC_MEDIA_STATUS_OK;
}


static bt_codec_media_status_t bt_hfp_set_disable_downlink_path_flag(bt_media_handle_t *handle, bool want_disable)
{
    if (bt_hfp_check_state(BT_CODEC_STATE_READY) == BT_HFP_FALSE) {
        TASK_LOG_E("bt_codec_set_disable_bt_rx_flag: not in ready state\r\n");
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    if (bt_hfp_codec_internal_handle != NULL){
        bt_hfp_codec_internal_handle->disable_downlink_path_flag = want_disable ? true : false;
        TASK_LOG_CTRL("bt_codec_set_disable_downlink_path_flag: want_disable=%d\r\n", want_disable);
    } else {
        TASK_LOG_E("bt_codec_set_disable_bt_rx_flag: bt_hfp_codec_internal_handle = NULL\r\n");
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    return BT_CODEC_MEDIA_STATUS_OK;
}

/* Should not use this API */
static bt_codec_media_status_t bt_hfp_process(bt_media_handle_t *handle, bt_codec_media_event_t event)
{
    TASK_LOG_E("process\r\n");
    return BT_CODEC_MEDIA_STATUS_ERROR;
}

static bt_media_handle_t *bt_hfp_open_codec_internal(bt_hfp_audio_codec_type_t codec_type, bt_hfp_mode_t mode)
{
    bt_hfp_codec_internal_handle_t *p_info;
    bt_media_handle_t *handle;
    TASK_LOG_CTRL("+open\r\n");
    {   /* Allocate Internal Handle */
        bt_hfp_speech_tx_information_t *p_tx_info;
        bt_hfp_speech_rx_information_t *p_rx_info;
        p_info = (bt_hfp_codec_internal_handle_t *)pvPortMalloc(sizeof(bt_hfp_codec_internal_handle_t));
        if (p_info == NULL) {
            TASK_LOG_CTRL("bt_hfp_open_codec_internal: p_info allocate fail\r\n");
            return NULL;
        }

        memset(p_info, 0, sizeof(bt_hfp_codec_internal_handle_t));
        handle = &p_info->handle;
        handle->play    = bt_hfp_play;
        handle->stop    = bt_hfp_stop;
        handle->process = bt_hfp_process;
        handle->set_disable_downlink_path_flag = bt_hfp_set_disable_downlink_path_flag;
        bt_hfp_codec_internal_handle = p_info;
        p_info->codec_type  = codec_type;
        p_info->mode        = mode;
        p_info->disable_downlink_path_flag = true;  // TODO: we will set false as default after bt remote control project setted.
        p_tx_info = &p_info->tx_info;
        p_rx_info = &p_info->rx_info;

        bt_hfp_ping_pong_index = BT_HFP_PING_PONG_INDEX_SECOND;

        if (mode == BT_HFP_MODE_SPEECH || mode == BT_HFP_MODE_LOOPBACK_WITH_CODEC) {
            /* Codec Setting */
            if (codec_type != BT_HFP_CODEC_TYPE_NONE) {
                if (bt_hfp_msbc_cvsd_set_buffers(p_tx_info, p_rx_info, codec_type) == BT_CODEC_MEDIA_STATUS_ERROR) {
                    TASK_LOG_CTRL("bt_hfp_open_codec_internal: bt_hfp_msbc_cvsd_set_buffers fail\r\n");
                    vPortFree(p_info);
                    return NULL;
                }
            } else {
                TASK_LOG_E("[HFP]bt_hfp_open_codec_internal: Wrong: codec_type = BT_HFP_CODEC_TYPE_NONE, return NULL\r\n");
                return NULL;
            }
        }
    }

    {   /* NVIC Settings */

        p_info->bt_nvic_irq_number = CM4_HIF_DMA_IRQ;

        // disable IRQ
        NVIC_DisableIRQ(p_info->bt_nvic_irq_number);
        NVIC_SetPriority(p_info->bt_nvic_irq_number, 4);
        if (hal_nvic_register_isr_handler(p_info->bt_nvic_irq_number, (hal_nvic_isr_t)bt_hfp_bt_interrupt_callback) != HAL_NVIC_STATUS_OK) {
            TASK_LOG_E("[HFP]bt_hfp_open_codec_internal: nvic register fail\r\n");
            return NULL;
        }
    }

    handle->state = BT_CODEC_STATE_READY;
    TASK_LOG_CTRL("-open\r\n");
    return handle;
}



static void bt_hfp_msbc_cvsd_free_buffer(bt_hfp_speech_tx_information_t *p_tx_info, bt_hfp_speech_rx_information_t *p_rx_info)
{
    vPortFree(p_tx_info->buffer_pointer);
    vPortFree(p_rx_info->buffer_pointer);
    p_tx_info->buffer_pointer = NULL;
    p_rx_info->buffer_pointer = NULL;
    return;
}
static bt_codec_media_status_t bt_hfp_close_codec_internal(void)
{
    bt_hfp_codec_internal_handle_t *p_info = bt_hfp_codec_internal_handle;
    bt_media_handle_t *handle;
    bt_codec_state_t state;
    bt_hfp_mode_t mode;

    {   /* Basic Settings & Check */
        RETURN_MEDIA_ERROR_IF_FALSE(p_info != NULL);
        handle = &p_info->handle;
        state = handle->state;
        mode = p_info->mode;
        RETURN_MEDIA_ERROR_IF_FALSE(state == BT_CODEC_STATE_STOP);
    }
    TASK_LOG_CTRL("+close\r\n");
    if (mode == BT_HFP_MODE_SPEECH || mode == BT_HFP_MODE_LOOPBACK_WITH_CODEC) {
        /* Codec Settings */
        bt_hfp_audio_codec_type_t codec_type = p_info->codec_type;
        bt_hfp_speech_tx_information_t *p_tx_info = &p_info->tx_info;
        bt_hfp_speech_rx_information_t *p_rx_info = &p_info->rx_info;
        RETURN_MEDIA_ERROR_IF_FALSE(codec_type == BT_HFP_CODEC_TYPE_CVSD || codec_type == BT_HFP_CODEC_TYPE_MSBC);
        if (codec_type != BT_HFP_CODEC_TYPE_NONE) {
            bt_hfp_msbc_cvsd_free_buffer(p_tx_info, p_rx_info);
        } else {
            TASK_LOG_E("[HFP]bt_hfp_close_codec_internal: Wrong: codec_type = BT_HFP_CODEC_TYPE_NONE, return fail\r\n");
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
    }
    {   /* NVIC Deinit */
        // TODO: since we don't have unregister nvic api, so do nothing here. we will add when it has.
    }
    handle->state = BT_CODEC_STATE_IDLE;
    vPortFree(p_info);
    p_info = NULL;
    TASK_LOG_CTRL("-close\r\n");
    return BT_CODEC_MEDIA_STATUS_OK;
}


/*******************************************************************************\
| Public Functions                                                              |
\*******************************************************************************/

/*
void bt_codec_task_create(void)
{
    xTaskCreate(bt_codec_task_main, BT_CODEC_TASK_NAME, BT_CODEC_TASK_STACKSIZE/((uint32_t)sizeof(StackType_t)), NULL, BT_CODEC_TASK_PRIO, NULL);
    return;
}
*/

bt_media_handle_t *bt_codec_hfp_open(bt_codec_hfp_callback_t callback, const bt_codec_hfp_audio_t *param)
{
    bt_hfp_audio_codec_type_t codec_type;
    bt_hfp_mode_t mode;
    RETURN_NULL_IF_FALSE(param != NULL);
    codec_type = param->type;
    RETURN_NULL_IF_FALSE(codec_type == BT_HFP_CODEC_TYPE_CVSD || codec_type == BT_HFP_CODEC_TYPE_MSBC);
    mode = BT_HFP_MODE_SPEECH;
    TASK_LOG_CTRL("open_codec\r\n");
    return bt_hfp_open_codec_internal(codec_type, mode);
}

bt_codec_media_status_t bt_codec_hfp_close(bt_media_handle_t *handle)
{
    TASK_LOG_CTRL("close_codec\r\n");
    return bt_hfp_close_codec_internal();
}
