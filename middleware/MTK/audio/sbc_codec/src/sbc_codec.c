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

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "sbc_codec.h"
#include "bli_exp.h"
#include "sbc_encoder.h"

#include "hal_audio_internal_service.h"

#include "syslog.h"
#include "hal_log.h"

/**
 * Dump data into file. define SBC_CODEC_DUMP_TO_FILE to enable dump
 *  - define SBC_CODEC_DUMP_TO_SBC_FILE to dump encoded sbc frame
 *  - define SBC_CODEC_DUMP_RAW_TO_PCM_FILE to dump pcm data from hal audio
 * E.g. dump encoded sbc frame
 *     define SBC_CODEC_DUMP_TO_FILE & SBC_CODEC_DUMP_TO_SBC_FILE
 */
// #define SBC_CODEC_DUMP_TO_FILE
// #define SBC_CODEC_DUMP_TO_SBC_FILE
// #define SBC_CODEC_DUMP_RAW_TO_PCM_FILE
/**
 * Log level control
 */
#define TASK_ERROR_LOGGING
#define TASK_WARNING_LOGGING
#define TASK_PROCESS_LOGGING
// #define TASK_DEBUG_LOGGING

#if defined(TASK_ERROR_LOGGING)
#define TASK_LOG_E(fmt,arg...)      log_hal_info("[SBC_CODEC][Error] " fmt,##arg)
#else
#define TASK_LOG_E(fmt,arg...)
#endif

#if defined(TASK_WARNING_LOGGING)
#define TASK_LOG_W(fmt,arg...)      log_hal_info("[SBC_CODEC][Warning] " fmt,##arg)
#else
#define TASK_LOG_W(fmt,arg...)
#endif

#if defined(TASK_PROCESS_LOGGING)
#define TASK_LOG_I(fmt,arg...)      log_hal_info("[SBC_CODEC][Info] " fmt,##arg)
#else
#define TASK_LOG_I(fmt,arg...)
#endif

#if defined(TASK_DEBUG_LOGGING)
#define TASK_LOG_D(fmt,arg...)      log_hal_info("[SBC_CODEC][Debug] " fmt,##arg)
#else
#define TASK_LOG_D(fmt,arg...)
#endif

#ifdef SBC_CODEC_DUMP_TO_FILE
#include "ff.h"
#define SBC_CODEC_DUMP_BUFFER (40*1024)
static FIL sbc_codec_fdst;
static UINT sbc_codec_length_write;
uint32_t sbc_codec_dump_buffer_size = 0;
_Pragma("data_alignment=4") uint8_t sbc_codec_dump_buffer[SBC_CODEC_DUMP_BUFFER] = {0};
#endif

#define SBC_CODEC_MAX_FUNCTIONS 5

/** @brief audio codec state. */
typedef enum {
    SBC_CODEC_STATUS_NONE          = 0,
    SBC_CODEC_STATUS_ERROR         = 1 << 0,
    SBC_CODEC_STATUS_IN_TASK_LIST  = 1 << 1,
    SBC_CODEC_STATUS_IN_CALLBACK   = 1 << 2,
} sbc_codec_status_t;

typedef struct {
    uint32_t sampling_frequency; /* in kHz */
    uint32_t blocks;
    uint32_t channels;
    uint32_t subbands;
    uint32_t channel_mode;
    uint32_t allocation_method;
    uint32_t bitpool;
} sbc_codec_sbc_frame_t;

typedef struct {
    sbc_codec_media_handle_t        handle;
    sbc_codec_initial_parameter_t   codec_info;
    sbc_codec_status_t              status;
    void                            *hdl;
    SemaphoreHandle_t               xSemaphore; // sync close driver & encode task
    /**
     * Total 6 buffers
     * IP working buffer :
     *     Bli (bli_working_buffer)                 [BLI_GetMemSize]                        bli_working_buffer_size
     *     SBC (sbc_working_buffer)                 [sbc_encoder_get_buffer_size]           sbc_working_buffer_size
     * driver working buffer :
     *     BLI out / SBC in (p_sbc_subband_buffer)  [SBC_ENCODE_BUFFER_SIZE]                SBC_ENCODE_BUFFER_SIZE
     *     SBC out (sbc_output)                     [dual channel max bitpool frame size]   sbc_output_size
     * driver output
     *     SBC frame (p_sbc_frame_ring)             [SBC_FRAME_RING_SIZE]                   SBC_FRAME_RING_SIZE
     * driver input
     *     pcm data / BLI in (p_pcm_ring)           [PCM_DATA_RING_SIZE]                    PCM_DATA_RING_SIZE
     */
    uint32_t                        total_size;
    /* SRC information */
    unsigned int                    bli_in_sr;
    unsigned int                    bli_out_sr;
    unsigned int                    bli_in_ch;
    unsigned int                    bli_out_ch;
    unsigned int                    bli_working_buffer_size; // initial working buffer
    BLI_HANDLE                      *bli_handle;
    uint8_t                         *bli_working_buffer;
    /* SBC encoder */
    void                            *sbc_encoder_handle;
    sbc_encoder_initial_parameter_t initial_parameter;
    sbc_codec_sbc_frame_t           sbc_frame;
    uint8_t                         *sbc_output;
    uint8_t                         *sbc_working_buffer;
    uint32_t                        frame_count; // for debug
    uint32_t                        sbc_working_buffer_size;
    uint32_t                        sbc_output_size;
    /* SBC information */
    uint32_t                        source_channel;
    uint32_t                        source_sample_rate;
    uint32_t                        bit_rate;
    uint32_t                        current_frame_size;
    uint32_t                        smaple_count; // fixed
    ring_buffer_information_t       p_sbc_subband_buffer; // MCU_MIN_SB_BUFFER_SIZE
    int32_t                         subband_buffer_size;
    /* payload */
    uint32_t                        total_payload_size;
    bool                            fragment_flag;
    uint8_t                         fragment_left;
    uint32_t                        fragment_size;
    uint32_t                        fragment_left_bytes;
    /* MCU ring buffer for SBC frame */
    ring_buffer_information_t p_sbc_frame_ring;
    /* MCU ring buffer for pcm data */
    ring_buffer_information_t p_pcm_ring;
} sbc_codec_internal_handle_t;

typedef void (*sbc_codec_internal_callback_t)(sbc_codec_media_handle_t *handle, void *parameter);

QueueHandle_t sbc_codec_queue_handle = NULL;
TaskHandle_t sbc_codec_task_handle = NULL;
sbc_codec_internal_callback_t sbc_codec_queue_cb_array[SBC_CODEC_MAX_FUNCTIONS];
sbc_codec_media_handle_t *sbc_codec_queue_handle_array[SBC_CODEC_MAX_FUNCTIONS];

/**
 * Utility function.
 */
#ifdef SBC_CODEC_DUMP_TO_FILE
static void sbc_codec_dump_to_file(uint8_t *buffer, uint32_t size, const TCHAR *path)
{
    FRESULT res = f_open(&sbc_codec_fdst, path, FA_OPEN_APPEND | FA_WRITE | FA_READ);
    if (res) {
        TASK_LOG_E("fopen error %d \r\n", res);
    }
    res = f_write(&sbc_codec_fdst, buffer, size, &sbc_codec_length_write);
    if ( res != FR_OK )
        TASK_LOG_E("f_write fail. %d", res);
    res = f_close(&sbc_codec_fdst);
    if (res) {
        TASK_LOG_I("fclose error %d \r\n", res);
    }
}
#endif /*SBC_CODEC_DUMP_TO_FILE*/

static int32_t sbc_codec_event_deregister_callback(sbc_codec_media_handle_t *handle)
{
    uint32_t id_idx;
    int32_t status = -1;
    for (id_idx = 0; id_idx < SBC_CODEC_MAX_FUNCTIONS; id_idx++) {
        if (sbc_codec_queue_handle_array[id_idx] == handle) {
            sbc_codec_queue_handle_array[id_idx] = NULL;
            status = 0;
            break;
        }
    }
    return status;
}

static int32_t sbc_codec_event_send_from_isr (sbc_codec_media_handle_t *handle, void *parameter)
{
    sbc_codec_internal_handle_t *internal_handle = (sbc_codec_internal_handle_t *)handle;
    if ( internal_handle->status&SBC_CODEC_STATUS_IN_TASK_LIST || handle->state != SBC_CODEC_STATE_PLAY) {
        if ( handle->state != SBC_CODEC_STATE_PLAY )
            TASK_LOG_I("handle->state != SBC_CODEC_STATE_PLAY %d \r\n", handle->state );
        return 0;
    }

    sbc_codec_queue_event_t event;
    event.handle    = handle;
    event.parameter = parameter;
    internal_handle->status |= SBC_CODEC_STATUS_IN_TASK_LIST;
    if (xQueueSendFromISR(sbc_codec_queue_handle, &event, 0) != pdPASS) {
        TASK_LOG_E("send fail.");
        internal_handle->status &= ~SBC_CODEC_STATUS_IN_TASK_LIST;
        return -1;
    }

    return 0;
}

static void sbc_codec_task_main(void *arg)
{
    TASK_LOG_I("sbc_codec_task_main");
    sbc_codec_queue_event_t event;

    while (1) {
        if (xQueueReceive(sbc_codec_queue_handle, &event, portMAX_DELAY)) {
            // log_hal_info("uxQueueSpacesAvailable %d \n", uxQueueSpacesAvailable(sbc_codec_queue_handle));
            sbc_codec_media_handle_t *handle = event.handle;
            uint32_t id_idx;
            for (id_idx = 0; id_idx < SBC_CODEC_MAX_FUNCTIONS; id_idx++) {
                if (sbc_codec_queue_handle_array[id_idx] == handle) {
                    sbc_codec_queue_cb_array[id_idx](handle, event.parameter);
                    break;
                }
            }
        }
    }
}

static void sbc_codec_reset_buffer(ring_buffer_information_t *pring)
{
    pring->write_pointer = 0;
    pring->read_pointer  = 0;
}

static uint32_t sbc_codec_calc_bitrate_from_bitpool( uint32_t bitpool, uint8_t nrof_subbands, uint8_t nrof_blocks, uint8_t channel_mode, uint32_t fs, uint32_t *frame_length )
{
    uint8_t nrof_channels, join;
    uint32_t frame_length_t, bitrate;

    nrof_channels = ( channel_mode==SBC_ENCODER_MONO )? 1: 2;
    join = ( channel_mode==SBC_ENCODER_JOINT_STEREO)? 1: 0;

    frame_length_t = 4 + ( 4 * nrof_subbands * nrof_channels ) / 8;
    if( channel_mode==SBC_ENCODER_MONO || channel_mode==SBC_ENCODER_DUAL_CHANNEL ) {
       frame_length_t += (nrof_blocks * nrof_channels * bitpool) / 8;
    } else if( channel_mode==SBC_ENCODER_STEREO || channel_mode==SBC_ENCODER_JOINT_STEREO ) {
       frame_length_t += ( join * nrof_subbands + nrof_blocks * bitpool ) / 8;
    } else {
        return 0;
    }

    bitrate = ( 8 * frame_length_t * fs ) / ( nrof_subbands * nrof_blocks );
    bitrate = (bitrate + 999) / 1000; // in kb/s with rounding

    if( frame_length!=NULL )
       *frame_length = frame_length_t;
    TASK_LOG_D("frame_length_t %d \r\n", frame_length_t);

    return bitrate;
}

static uint32_t sbc_codec_bitpool_from_bitrate( uint32_t bitrate, uint8_t nrof_subbands, uint8_t nrof_blocks, uint8_t channel_mode, uint32_t fs, uint32_t *frame_length )
{
    uint8_t nrof_channels = 0, join = 0;
    uint32_t bitpool = 0;
    uint32_t frame_length_t = 0;

    nrof_channels = ( channel_mode==SBC_ENCODER_MONO )? 1: 2;
    join = ( channel_mode==SBC_ENCODER_JOINT_STEREO)? 1: 0;

    frame_length_t = ( bitrate * 1000 * nrof_subbands * nrof_blocks ) / ( 8 * fs );
    if( frame_length != NULL ) {
        *frame_length = frame_length_t;
    }

    frame_length_t -= 4 + ( 4 * nrof_subbands * nrof_channels ) / 8;
    if( frame_length_t<=0 ) {
       return 0;
    }
    if( channel_mode==SBC_ENCODER_MONO || channel_mode==SBC_ENCODER_DUAL_CHANNEL ) {
       bitpool = ( 8 * frame_length_t ) / ( nrof_blocks * nrof_channels );
    } else if( channel_mode == SBC_ENCODER_STEREO || channel_mode == SBC_ENCODER_JOINT_STEREO ) {
       bitpool = ( 8 * frame_length_t - join * nrof_subbands ) / nrof_blocks;
    } else {
       return 0;
    }

    return bitpool;
}

uint32_t sbc_codec_get_buffer_size(void)
{
    // maximum size
    // sbc_codec_internal_handle_t *internal_handle = (sbc_codec_internal_handle_t *)handle;
    uint32_t total_size         = 0;
    uint32_t encoder_size       = 0;
    uint32_t sbc_max_frame_size = 0;
    unsigned int bli_size       = 0;

    BLI_GetMemSize(48000, 2, 44100/*output_sr*/, 2, &bli_size);//?
    encoder_size = sbc_encoder_get_buffer_size();
    sbc_codec_calc_bitrate_from_bitpool( /*bitpool*/75,
        /*subbands*/8, /*blocks*/16, /*channel*/SBC_ENCODER_DUAL_CHANNEL, /*fs*/48000,
        /*out*/&sbc_max_frame_size );

    total_size += SBC_ENCODE_BUFFER_SIZE;
    total_size += SBC_FRAME_RING_SIZE;
    total_size += PCM_DATA_RING_SIZE;
    total_size += bli_size;
    total_size += encoder_size;
    total_size += sbc_max_frame_size;

    TASK_LOG_I("sbc codec memory : total_size:%d(bytes) encoder_size %d sbc_max_frame_size %d bli_size %d SBC_ENCODE_BUFFER_SIZE %d SBC_FRAME_RING_SIZE %d PCM_DATA_RING_SIZE %d",
        total_size, encoder_size, sbc_max_frame_size, bli_size, SBC_ENCODE_BUFFER_SIZE, SBC_FRAME_RING_SIZE, PCM_DATA_RING_SIZE);

    return total_size;
}

static void sbc_codec_set_buffer(sbc_codec_media_handle_t *handle, uint8_t  *buffer, uint32_t length)
{
    sbc_codec_internal_handle_t *internal_handle = (sbc_codec_internal_handle_t *)handle;

    if ( handle->state != SBC_CODEC_STATE_IDLE ) {
        return;
    }
    /* Initialize. */
    internal_handle->total_size = sbc_codec_get_buffer_size();
    BLI_GetMemSize(48000, 2, 44100/*output_sr*/, 2, &(internal_handle->bli_working_buffer_size));//?
    internal_handle->sbc_working_buffer_size = sbc_encoder_get_buffer_size();
    sbc_codec_calc_bitrate_from_bitpool( /*bitpool*/75,
        /*subbands*/8, /*blocks*/16, /*channel*/SBC_ENCODER_DUAL_CHANNEL, /*fs*/48000,
        /*out*/&(internal_handle->sbc_output_size) );

    /* Check buffer size is large enough. */
    if( internal_handle->total_size > length ) {
        TASK_LOG_E("buffer size too small.");
    }
    /* Buffer assignment. */
    #define ASSIGN_BUFFER(pointer, u8_buffer, size) do {  \
        u8_buffer = (uint8_t *)pointer;                   \
        pointer += size;                                  \
    } while(0)
    #define ASSIGN_RING(pointer, ring, size) do {         \
        ring.buffer_base_pointer = (uint8_t *)pointer;    \
        ring.buffer_byte_count =  size;                   \
        pointer += size;                                  \
    } while(0)

    ASSIGN_BUFFER(buffer, internal_handle->bli_working_buffer,   internal_handle->bli_working_buffer_size);
    ASSIGN_BUFFER(buffer, internal_handle->sbc_working_buffer,   internal_handle->sbc_working_buffer_size);
    ASSIGN_RING  (buffer, internal_handle->p_sbc_subband_buffer, SBC_ENCODE_BUFFER_SIZE);
    ASSIGN_BUFFER(buffer, internal_handle->sbc_output,           internal_handle->sbc_output_size);
    ASSIGN_RING  (buffer, internal_handle->p_sbc_frame_ring,     SBC_FRAME_RING_SIZE);
    ASSIGN_RING  (buffer, internal_handle->p_pcm_ring,           PCM_DATA_RING_SIZE);

    #undef ASSIGN_RING
    #undef ASSIGN_BUFFER

    handle->state = SBC_CODEC_STATE_READY;
}

static void sbc_codec_write_to_dst_buffer(ring_buffer_information_t *p_src_info, uint8_t *p_dst, uint32_t buf_len)
{
    int32_t  loop_idx = 0;
    uint32_t dst_cnt  = buf_len;
    uint32_t data_cnt = 0;
    data_cnt = ring_buffer_get_data_byte_count(p_src_info); // source
    if ( data_cnt >= dst_cnt ) { // enough source check
        for (loop_idx = 0; loop_idx < 2; loop_idx++) {
            uint8_t *p_src    = NULL;
            uint32_t src_cnt  = 0;
            uint32_t read_cnt = 0;
            ring_buffer_get_read_information(p_src_info, &p_src, &src_cnt);
            read_cnt = MINIMUM(dst_cnt, src_cnt);
            memcpy(p_dst, p_src, read_cnt);
            p_dst   += read_cnt;
            dst_cnt -= read_cnt;
            ring_buffer_read_done(p_src_info, read_cnt);
        }
    } else {
        TASK_LOG_E("source not enough. src:%d dst:%d \r\n", data_cnt, dst_cnt);
    }
}
/**
 * For user to get payload. User will be notified by SBC_CODEC_MEDIA_GET_PAYLOAD
 * After User finish processing payload. Must call sbc_codec_get_payload_done() to enable
 * SBC_CODEC_MEDIA_GET_PAYLOAD notification.
 */
static uint32_t sbc_codec_get_payload(sbc_codec_media_handle_t *handle, uint8_t* buffer, uint32_t size, uint32_t *sample_cnt)
{
    if ( !(handle->state == SBC_CODEC_STATE_PLAY || handle->state == SBC_CODEC_STATE_WAIT_DRAIN) ) {
        TASK_LOG_E("cannot get payload when state is %d \r\n", handle->state);
        sample_cnt = 0;
        return 0;
    }
    /**
     * Note: header for Payload. (Size = 1 byte)
     * b7: fragmented b6: start fragmented b5:last fragmented b4:RFA
     * b3-b0: [if b7=0] number of frames (max 15 frames for single payload)
     * b3-b0: [if b7=1] number of remain fragmented frames (max 15 fragment)
     */
    uint8_t *p_dst     = buffer+1; // 1 byte for header
    uint8_t *p_src     = NULL;
    uint32_t buf_len   = size-1; // 1 byte for header
    uint32_t frame_len = 0, src_cnt = 0, data_cnt = 0;
    int frame_cnt      = 0;
    sbc_codec_internal_handle_t *internal_handle = (sbc_codec_internal_handle_t *)handle;
    ring_buffer_information_t   *p_sbc_info      = &internal_handle->p_sbc_frame_ring; // source

#ifdef SBC_CODEC_DUMP_TO_SBC_FILE
    if (sbc_codec_dump_buffer_size + size > SBC_CODEC_DUMP_BUFFER ) {
        sbc_codec_dump_to_file(&sbc_codec_dump_buffer[0], sbc_codec_dump_buffer_size, _T("0:/music/temp.sbc"));
        sbc_codec_dump_buffer_size = 0;
    }
#endif /*SBC_CODEC_DUMP_TO_SBC_FILE*/

    if ( internal_handle->fragment_flag ) {
        /* second to last fragment. */
        frame_len = (internal_handle->fragment_left_bytes >= internal_handle->fragment_size)
                            ? internal_handle->fragment_size: internal_handle->fragment_left_bytes;
        if(buf_len >= frame_len) {
            data_cnt = frame_len;
            internal_handle->fragment_left--;

            sbc_codec_write_to_dst_buffer(/*src*/p_sbc_info, /*dst*/p_dst, frame_len);
            p_dst                                += frame_len;
            internal_handle->fragment_left_bytes -= frame_len;
            internal_handle->total_payload_size  -= frame_len;

            /* Set payload header. */
            if ( internal_handle->fragment_left == 1 ) { // last fragment
                if( internal_handle->fragment_left_bytes != 0 ) {
                   TASK_LOG_E("last fragment. should not have left bytes.");
                }
                internal_handle->fragment_flag = false;

                *buffer = 0xA0 + internal_handle->fragment_left;
                *sample_cnt = internal_handle->smaple_count;
            } else {
               *buffer = 0x80 + internal_handle->fragment_left;
               *sample_cnt = 0;
            }
        } else {
            TASK_LOG_E("frag number not right. could be payload size change.");
        }
    } else {
        /* first fragment or integral number of frame. */
        bool allow_fragment = true;
        while(1) {
            ring_buffer_get_read_information(p_sbc_info, &p_src, &src_cnt);
            frame_len = (uint8_t)(*p_src);
            TASK_LOG_D("frame_len %d ", frame_len);

            if(buf_len >= frame_len) {
                /* contained integral number of frames. */
                allow_fragment = false;
                ring_buffer_read_done(p_sbc_info, 1); // frame len
                ring_buffer_get_read_information(p_sbc_info, &p_src, &src_cnt);
                /* check sbc frame sync word. */
                if ( *p_src != 0x9c ) {
                    TASK_LOG_E("sync word wrong.frame_len %d ", frame_len);
                }
                sbc_codec_write_to_dst_buffer(/*src*/p_sbc_info, /*dst*/p_dst, /*dst size*/frame_len);
        #ifdef SBC_CODEC_DUMP_TO_SBC_FILE
                memcpy(&sbc_codec_dump_buffer[sbc_codec_dump_buffer_size], p_dst, frame_len);
                sbc_codec_dump_buffer_size += frame_len;
        #endif /*SBC_CODEC_DUMP_TO_SBC_FILE*/
                p_dst    += frame_len;
                buf_len  -= frame_len;
                data_cnt += frame_len;

                internal_handle->total_payload_size -= frame_len;
                src_cnt = ring_buffer_get_data_byte_count(p_sbc_info);
                if ( src_cnt == 0 && internal_handle->total_payload_size != 0 ) {
                    internal_handle->total_payload_size = 0;
                }

                frame_cnt += 1;
                if( (frame_cnt == 15) || (internal_handle->total_payload_size == 0)) {
                    *buffer = frame_cnt; // payload header
                    *sample_cnt = frame_cnt * internal_handle->smaple_count;
                    break;
                }
            } else {
                if( allow_fragment == true ) {
                    /* frame is fragmented. */
                    if( (buf_len*15) >= frame_len) {
                        internal_handle->fragment_flag = true;
                        internal_handle->fragment_size = buf_len;
                        internal_handle->fragment_left = (uint8_t)((frame_len + buf_len - 1)/buf_len);
                        internal_handle->fragment_left_bytes = frame_len - internal_handle->fragment_size;

                        ring_buffer_read_done(p_sbc_info, 1); // frame len
                        ring_buffer_get_read_information(p_sbc_info, &p_src, &src_cnt);
                        /* check sbc frame sync word. */
                        if ( *p_src != 0x9c ) {
                            TASK_LOG_E("sync word wrong.frame_len %d ", frame_len);
                        }
                        sbc_codec_write_to_dst_buffer(/*src*/p_sbc_info, /*dst*/p_dst, /*dst size*/buf_len);
                        p_dst    += buf_len;
                        data_cnt  = buf_len;

                        internal_handle->total_payload_size -= buf_len;

                        *buffer = 0xC0 + internal_handle->fragment_left; // payload header
                        *sample_cnt = 0;
                    } else {
                       TASK_LOG_E("fragment too much");
                    }
                } else {
                    /* contained integral number of frames. remain space is not enough for a frame.*/
                    *buffer = frame_cnt; // payload header
                    *sample_cnt = frame_cnt * internal_handle->smaple_count;
                }
                break;
            }
        }
    }
    if ( data_cnt != 0 ) data_cnt++; // payload header
    TASK_LOG_D("payload info : data_cnt %d fragment %d fragment_size %d fragment_left %d frames %d frame_sample %d",
        data_cnt, internal_handle->fragment_flag, internal_handle->fragment_size, internal_handle->fragment_left, frame_cnt, internal_handle->smaple_count);
    return data_cnt;
}

static void sbc_codec_get_payload_done(sbc_codec_media_handle_t *handle)
{
    handle->waiting = false;
}

static void sbc_codec_query_payload_size(sbc_codec_media_handle_t *handle, uint32_t *minimum, uint32_t *total)
{
    sbc_codec_internal_handle_t *internal_handle = (sbc_codec_internal_handle_t *)handle;
    ring_buffer_information_t   *p_sbc_info      = &internal_handle->p_sbc_frame_ring;

    uint32_t src_cnt = 0;
    uint8_t *p_src   = NULL;

    if( handle->state != SBC_CODEC_STATE_PLAY ) {
        *minimum = *total = 0;
        return;
    }
    if( internal_handle->fragment_flag == true ) {
        *minimum = *total = ( internal_handle->fragment_size > internal_handle->fragment_left_bytes )?
                           internal_handle->fragment_left_bytes: /* remain fragment size. */
                           internal_handle->fragment_size; /* fix fragment size. */
        (*minimum)++;
        (*total)++;
    } else {
        ring_buffer_get_read_information(p_sbc_info, &p_src, &src_cnt);
        *minimum = 1 + (uint8_t)(*p_src); /* next SBC frame size. */
        *total   = 1 + internal_handle->total_payload_size; /* total payload size. */
   }
}

static uint32_t sbc_codec_config(sbc_codec_media_handle_t *handle, uint32_t bit_rate)
{
    sbc_codec_internal_handle_t *internal_handle = (sbc_codec_internal_handle_t *)handle;
    sbc_codec_sbc_frame_t *pFrame  = &internal_handle->sbc_frame;
    sbc_codec_sbc_t       *pConfig = &internal_handle->codec_info.cap;
    uint32_t min_bit_pool = pConfig->min_bit_pool;
    uint32_t max_bit_pool = pConfig->max_bit_pool;
    uint32_t frame_length_t = 0;
    uint32_t bitpool_t = 0;

    bitpool_t = sbc_codec_bitpool_from_bitrate( bit_rate,
        pFrame->subbands, pFrame->blocks, pFrame->channel_mode, pFrame->sampling_frequency, NULL );
    if( bitpool_t < min_bit_pool ) {
       bitpool_t = min_bit_pool;
    } else if( bitpool_t > max_bit_pool ) {
       bitpool_t = max_bit_pool;
    }

    bit_rate = sbc_codec_calc_bitrate_from_bitpool( bitpool_t,
        pFrame->subbands, pFrame->blocks, pFrame->channel_mode, pFrame->sampling_frequency, &frame_length_t );

    pFrame->bitpool = bitpool_t;
    internal_handle->current_frame_size = frame_length_t;

    return bit_rate;
}

static uint32_t sbc_codec_set_bit_rate(sbc_codec_media_handle_t *handle, uint32_t bit_rate)
{
    sbc_codec_internal_handle_t *internal_handle = (sbc_codec_internal_handle_t *)handle;
    sbc_codec_sbc_frame_t       *pConfig         = &(internal_handle->sbc_frame);
    internal_handle->bit_rate = sbc_codec_config(handle, bit_rate);

    TASK_LOG_I("sbc info : sample_rate %d block %d channel %d(%d) subband %d alloc %d bitpool %d frame length %d \r\n",
        pConfig->sampling_frequency, pConfig->blocks, pConfig->channels, pConfig->channel_mode,
        pConfig->subbands, pConfig->allocation_method, pConfig->bitpool, internal_handle->current_frame_size);

    return bit_rate;
}

static void sbc_codec_set_info(sbc_codec_media_handle_t *handle, uint32_t sample_rate, uint32_t channel) {
    sbc_codec_internal_handle_t *internal_handle = (sbc_codec_internal_handle_t *)handle;
    if ( internal_handle->source_sample_rate != sample_rate || internal_handle->source_channel != channel) {
        TASK_LOG_I("sample_rate %d->%d channel %d->%d \r\n",
            internal_handle->source_sample_rate, sample_rate,
            internal_handle->source_channel, channel);
    }
    internal_handle->source_sample_rate = sample_rate;
    internal_handle->source_channel     = channel;
}


static void sbc_codec_data_handle(hal_audio_event_t event, hal_audio_copy_stream_t *data) {
    switch(event) {
        case HAL_AUDIO_EVENT_DATA_NOTIFICATION: {
            sbc_codec_media_handle_t    *handle          = (sbc_codec_media_handle_t *)(data->user_data);
            sbc_codec_internal_handle_t *internal_handle = (sbc_codec_internal_handle_t *)handle;
            ring_buffer_information_t   *p_pcm_info      = &internal_handle->p_pcm_ring; // source
            sbc_codec_set_info(handle, data->sample_rate, data->channel);
        #ifdef SBC_CODEC_DUMP_RAW_TO_PCM_FILE
                {
                    if (sbc_codec_dump_buffer_size + data->buffer_size > SBC_CODEC_DUMP_BUFFER ) {
                        sbc_codec_dump_to_file(&sbc_codec_dump_buffer[0], sbc_codec_dump_buffer_size, _T("0:/music/temp.pcm"));
                        sbc_codec_dump_buffer_size = 0;
                    }
                    memcpy(&sbc_codec_dump_buffer[sbc_codec_dump_buffer_size], data->buffer, data->buffer_size);
                    sbc_codec_dump_buffer_size += data->buffer_size;
                }
        #endif /*SBC_CODEC_DUMP_TO_PCM_FILE*/
            if ( data->buffer_size != 0 ) {
                uint8_t *p_src    = data->buffer;
                uint32_t src_cnt  = data->buffer_size;
                int32_t  loop_idx = 0;
                uint32_t spce_cnt = 0;
                spce_cnt = ring_buffer_get_space_byte_count(p_pcm_info); // source
                if ( spce_cnt >= src_cnt ) { // enough dst check
                    // TASK_LOG_I("size:%d write:%d", data->buffer_size, p_pcm_info->write_pointer);
                    for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                        uint8_t *p_dst    = NULL;
                        uint32_t dst_cnt  = 0;
                        uint32_t read_cnt = 0;
                        ring_buffer_get_write_information(p_pcm_info, &p_dst, &dst_cnt);
                        read_cnt = MINIMUM(dst_cnt, src_cnt);
                        memcpy(p_dst, p_src, read_cnt);
                        src_cnt -= read_cnt;
                        p_src   += read_cnt;
                        ring_buffer_write_done(p_pcm_info, read_cnt);
                    }
                } else {
                    TASK_LOG_E("space not enough. %d < %d", spce_cnt, src_cnt);
                }
            }
            sbc_codec_event_send_from_isr(handle, NULL);
        } break;
    }
}

static void sbc_bli_reconfig(sbc_codec_internal_handle_t *internal_handle)
{
    if ( internal_handle->bli_handle != NULL ) {
        BLI_Close(internal_handle->bli_handle, NULL);
    }

    sbc_codec_sbc_frame_t *pFrame  = &internal_handle->sbc_frame;
    unsigned int buffer_size = 0;
    BLI_GetMemSize(internal_handle->source_sample_rate, internal_handle->source_channel,
            pFrame->sampling_frequency, pFrame->channels, &buffer_size);
    if ( internal_handle->bli_working_buffer_size < buffer_size) {
        // buffer size too large
        TASK_LOG_E("buffer size too large. buffer_size %d > %d internal_handle->bli_working_buffer_size",
            buffer_size, internal_handle->bli_working_buffer_size);
    }

    internal_handle->bli_handle = BLI_Open(internal_handle->source_sample_rate, internal_handle->source_channel,
        pFrame->sampling_frequency, pFrame->channels, (char*)internal_handle->bli_working_buffer, NULL);
    internal_handle->bli_in_sr  = internal_handle->source_sample_rate;
    internal_handle->bli_in_ch  = internal_handle->source_channel;
    internal_handle->bli_out_sr = pFrame->sampling_frequency;
    internal_handle->bli_out_ch = pFrame->channels;
    TASK_LOG_I("Config BLI sr:%d->%d ch:%d->%d \r\n",
        internal_handle->source_sample_rate, pFrame->sampling_frequency,
        internal_handle->source_channel, pFrame->channels);
}

static void sbc_codec_encode_process ( sbc_codec_media_handle_t *handle, void *parameter )
{
    TASK_LOG_D("sbc_codec_encode_process ++");
    /**
     * Brief steps
     * 1.Get enough pcm input
     *   - input size = subband*channel*block*2 bytes = internal_handle->subband_buffer_size
     *   - SRC
     *   - p_pcm_ring -> p_sbc_subband_buffer
     * 2.Do sbc encode if p_sbc_subband_buffer if full
     *   - p_sbc_subband_buffer -> sbc_output
     * 3.Write encoded SBC frame to ring buffer
     *   - an extra byte in front to record sbc frame length (unit:byte)
     *   - total size will be : internal_handle->current_frame_size + 1 bytes
     *   - sbc_output -> p_sbc_frame_ring
     */
    sbc_encoder_runtime_parameter_t runtime;
    sbc_codec_internal_handle_t *internal_handle = (sbc_codec_internal_handle_t *)handle;

    sbc_codec_sbc_frame_t     *pFrame     = &internal_handle->sbc_frame;
    ring_buffer_information_t *p_sbc_info = &internal_handle->p_sbc_frame_ring;
    ring_buffer_information_t *p_pcm_info = &internal_handle->p_pcm_ring;
    ring_buffer_information_t *p_subband  = &internal_handle->p_sbc_subband_buffer;

    uint32_t pcm_requirement   = 0;
    uint32_t sbc_requirement   = 1024;
    uint32_t loop_idx          = 0;
    uint32_t data_count        = 0;
    uint32_t output_byte_count = 0;
    uint32_t input_size        = pFrame->blocks * pFrame->channels * pFrame->subbands * 2;
    uint32_t sbc_byte_count    = 0;
    int32_t  status            = 0;

    xSemaphoreTake( internal_handle->xSemaphore, portMAX_DELAY );

    /* check for src re-config. */
    if ( internal_handle->source_sample_rate != internal_handle->bli_in_sr
        || internal_handle->source_channel != internal_handle->bli_in_ch
        || internal_handle->bli_handle == NULL) {
        TASK_LOG_I("BLI(sr,ch) src(%d, %d) now(%d, %d)",
                internal_handle->source_sample_rate, internal_handle->source_channel,
                internal_handle->bli_in_sr, internal_handle->bli_in_ch);
        sbc_bli_reconfig(internal_handle);
    }
    if ( internal_handle->subband_buffer_size == -1 ) { /* first init. */
        internal_handle->subband_buffer_size = input_size;
    }

    for (loop_idx = 0; loop_idx < 2; loop_idx++) {
        uint32_t subband_size   = 0;
        uint32_t consumed_input = 0;
        uint32_t read_cnt       = 0;
        uint32_t dst_cnt        = 0;
        uint32_t src_cnt        = 0;
        uint8_t *p_pcm_src      = NULL;
        uint8_t *p_src          = NULL;
        uint8_t *p_dst          = NULL;

        ring_buffer_get_read_information(p_pcm_info, &p_pcm_src, &data_count);
        while(data_count) {
            {   /* Check if p_sbc_frame_ring have enough space for a sbc frame + 1 byte data. */
                dst_cnt = ring_buffer_get_space_byte_count(p_sbc_info);
                TASK_LOG_D("start : pcm %d dst %d", data_count, dst_cnt);
                if ( dst_cnt < (internal_handle->current_frame_size+1) ) {
                    TASK_LOG_I("dst space not enough %d < %d", dst_cnt, internal_handle->current_frame_size+1);
                    break;
                }
                /* Check state. */
                if( handle->state == SBC_CODEC_STATE_STOP ) {
                    TASK_LOG_I("state not correct. %d", handle->state);
                    break;
                }
                /* Check payload size */
               // sbc_byte_count = ring_buffer_get_data_byte_count(p_sbc_info);
               // if(sbc_byte_count > sbc_requirement) {
               //     TASK_LOG_D("encode max %d \r\n", sbc_byte_count);
               //     break;
               // }
            }
            /* fill data into subband buffer. */
            if ( internal_handle->bli_in_sr != internal_handle->bli_out_sr ||
                 internal_handle->bli_in_ch != internal_handle->bli_out_ch ) {
                /* need SRC */
                dst_cnt = internal_handle->subband_buffer_size;
                TASK_LOG_D("BLI_Convert in %d out %d (sr %d -> %d ch %d->%d)", data_count, dst_cnt,
                    internal_handle->bli_in_sr, internal_handle->bli_out_sr, internal_handle->bli_in_ch, internal_handle->bli_out_ch);

                /*work around for BLI crash when same sample rate convert.*/
                if ( internal_handle->bli_in_sr == internal_handle->bli_out_sr ) {
                    /*same sampe rate case*/
                    uint32_t i = 0, sample_count = 0, dst_count = 0, src_count = 0;
                    int16_t *dst = (int16_t *)(p_subband->buffer_base_pointer + p_subband->write_pointer);
                    int16_t *src = (int16_t *)p_pcm_src;
                    if ( internal_handle->bli_in_ch == 1 ) { /*mono to stereo*/
                        src_count    = data_count >> 1;
                        dst_count    = dst_cnt >> 2;
                        sample_count = MINIMUM(src_count, dst_count);
                        for (i = sample_count; i > 0; i--) {
                            int16_t temp = *src++;
                            *dst++       = temp;
                            *dst++       = temp;
                        }
                        dst_cnt        = sample_count << 2;
                        consumed_input = sample_count << 1;
                        data_count    -= sample_count << 1;
                    } else { /*stereo to mono*/
                        src_count    = data_count >> 2;
                        dst_count    = dst_cnt >> 1;
                        sample_count = MINIMUM(src_count, dst_count);
                        for (i = sample_count; i > 0; i--) {
                            int32_t first  = *src++;
                            int32_t second = *src++;
                            int32_t temp   = (first + second) >> 1;
                            *dst++ = (int16_t)temp;
                        }
                        dst_cnt        = sample_count << 1;
                        consumed_input = sample_count << 2;
                        data_count    -= sample_count << 2;
                    }
                } else { /*different sample rate and channel*/
                    consumed_input = BLI_Convert(internal_handle->bli_handle,
                        (short *)(p_pcm_src), (unsigned int *)&data_count, (short *)(p_subband->buffer_base_pointer + p_subband->write_pointer), (unsigned int *)&dst_cnt);
                }

                TASK_LOG_D("BLI_Convert left %d out %d consume %d", data_count, dst_cnt, consumed_input);

                p_subband->write_pointer             += dst_cnt;
                internal_handle->subband_buffer_size -= dst_cnt;
                // data_count                        -= consumed_input; // done in IP
                read_cnt                             += consumed_input;
                p_pcm_src                            += consumed_input;
            } else { /* no need src */
                TASK_LOG_D("memcpy flow. no SRC");
                dst_cnt = internal_handle->subband_buffer_size;
                dst_cnt = MINIMUM(data_count, dst_cnt);
                memcpy(p_subband->buffer_base_pointer + p_subband->write_pointer, p_pcm_src, dst_cnt);
                p_subband->write_pointer             += dst_cnt;
                internal_handle->subband_buffer_size -= dst_cnt;
                data_count                           -= dst_cnt;
                read_cnt                             += dst_cnt;
                p_pcm_src                            += dst_cnt;
            }
            ring_buffer_read_done(p_pcm_info, read_cnt);
            read_cnt = 0;

            /* When subband buffer is full, execute SBC encode. */
            /* p_subband -> sbc_output -> p_sbc_info */
            subband_size = internal_handle->subband_buffer_size;
            if ( subband_size == 0 ) {
                runtime.bitpool_value = pFrame->bitpool;
                TASK_LOG_D("start encode bitpool %d in size %d out size %d", pFrame->bitpool, input_size, internal_handle->sbc_output_size);
                p_src             = p_subband->buffer_base_pointer;
                subband_size      = input_size; // input
                p_dst             = internal_handle->sbc_output;
                output_byte_count = internal_handle->sbc_output_size; // output
                status = sbc_encoder_process (internal_handle->sbc_encoder_handle,
                    /* src data */ (int16_t *)p_src, &subband_size,
                    /* dst data */ p_dst, &output_byte_count,
                    &runtime);
                TASK_LOG_D("sbc_encoder_process in %d out %d consume %d",
                    input_size, output_byte_count, subband_size );
                if ( status < 0 ) {
                    TASK_LOG_E("encode fail.");
                }

                /* Check size and sbc frame sync word. */
                if ( output_byte_count != internal_handle->current_frame_size || *(internal_handle->sbc_output) != 0x9c) {
                    TASK_LOG_E("output size(%d) != frame size(%d). or sync word wrong 0x%x. bitpool %d in %d(%p) out %d(%p) consume %d ",
                        output_byte_count, internal_handle->current_frame_size, *(internal_handle->sbc_output),
                        pFrame->bitpool, input_size, p_src, internal_handle->sbc_output_size, p_dst, subband_size);
                }
                /* reset buffer info. */
                sbc_codec_reset_buffer(p_subband);
                internal_handle->subband_buffer_size = input_size;

                internal_handle->frame_count++;
                internal_handle->total_payload_size += output_byte_count;

                {   /* write to mcu sbc frame ring buffer. */
                    dst_cnt = ring_buffer_get_space_byte_count(p_sbc_info);
                    if (dst_cnt >= output_byte_count+1) {
                        /* put current frame size in first byte. */
                        ring_buffer_get_write_information(p_sbc_info, &p_dst, &dst_cnt);
                        *p_dst = (uint8_t)internal_handle->current_frame_size;
                        ring_buffer_write_done(p_sbc_info, 1);
                        /* start sbc frame in second byte. */
                        int32_t loop_cnt;
                        p_src   = internal_handle->sbc_output;
                        src_cnt = output_byte_count;
                        for (loop_cnt = 0; loop_cnt < 2; loop_cnt++) {
                            uint32_t write_cnt;
                            ring_buffer_get_write_information(p_sbc_info, &p_dst, &dst_cnt);
                            write_cnt = MINIMUM(dst_cnt, src_cnt);
                            memcpy(p_dst, p_src, write_cnt);
                            p_src   += write_cnt;
                            src_cnt -= write_cnt;
                            ring_buffer_write_done(p_sbc_info, write_cnt);
                        }
                    } else {
                        TASK_LOG_E("sbc frame ring overflow. dst_cnt %d output_byte_count+1=%d", dst_cnt, output_byte_count+1);
                    }
                }
            } /*end sbc encode*/
        } /*end while loop*/
    } /*end for loop*/

    /* Notify get payload */
    sbc_byte_count = ring_buffer_get_data_byte_count(p_sbc_info);
    if ( handle->state == SBC_CODEC_STATE_WAIT_DRAIN ) { /*start clearing all remaining data.*/
        sbc_requirement = 0;
    }
    //if ( sbc_byte_count > sbc_requirement && !handle->waiting ) {
     if ( sbc_byte_count > sbc_requirement ) {
        internal_handle->status |= SBC_CODEC_STATUS_IN_CALLBACK;
        handle->waiting = true;
        handle->handler(internal_handle->hdl, SBC_CODEC_MEDIA_GET_PAYLOAD);
        internal_handle->status &= ~SBC_CODEC_STATUS_IN_CALLBACK;
    }

    data_count     = ring_buffer_get_data_byte_count(p_pcm_info);
    sbc_byte_count = ring_buffer_get_data_byte_count(p_sbc_info);
    internal_handle->status &= ~SBC_CODEC_STATUS_IN_TASK_LIST;
    if ( (data_count > pcm_requirement || sbc_byte_count > sbc_requirement)
        && handle->state == SBC_CODEC_STATE_PLAY ) {
        sbc_codec_event_send_from_isr(handle, NULL);
    }

    TASK_LOG_D("sbc_codec_encode_process (data,space) pcm(%d,%d) sbc_frame(%d,%d) --",
        data_count, ring_buffer_get_space_byte_count(p_pcm_info),
        sbc_byte_count, ring_buffer_get_space_byte_count(p_sbc_info));
    xSemaphoreGiveFromISR( internal_handle->xSemaphore, NULL );
}

static int32_t sbc_codec_play(sbc_codec_media_handle_t *handle)
{
    TASK_LOG_D("play sbc codec ++");
    sbc_codec_internal_handle_t *internal_handle = (sbc_codec_internal_handle_t *)handle;

    if ( handle->state != SBC_CODEC_STATE_READY) {
        return -1;
    }

    /* check configuration. */
    if ( internal_handle->bli_working_buffer == NULL ) {
        TASK_LOG_E("buffer not set.");
        return -1;
    }

    if ( sbc_encoder_init ( &(internal_handle->sbc_encoder_handle),
                internal_handle->sbc_working_buffer, &(internal_handle->initial_parameter)) ) {
        TASK_LOG_E("encoder init fail");
        return -1;
    }

    /* initialize. */
    internal_handle->bli_in_sr           = 0;
    internal_handle->bli_in_ch           = 0;
    internal_handle->status              = SBC_CODEC_STATUS_NONE;
    internal_handle->total_payload_size  = 0;
    internal_handle->fragment_flag       = false;
    internal_handle->fragment_left       = 0;
    internal_handle->fragment_size       = 0;
    internal_handle->fragment_left_bytes = 0;

    handle->state = SBC_CODEC_STATE_PLAY;

    /* register hal audio copy flow. */
    hal_audio_register_copied_stream_out_callback(sbc_codec_data_handle, handle);

    TASK_LOG_D("play sbc codec --");
    return 0;
}

static int32_t sbc_codec_pause(sbc_codec_media_handle_t *handle) {
    return -1;
}

static int32_t sbc_codec_resume(sbc_codec_media_handle_t *handle) {
    return -1;
}

static int32_t sbc_codec_stop(sbc_codec_media_handle_t *handle) {
    TASK_LOG_I("stop sbc codec ++");
    sbc_codec_internal_handle_t *internal_handle = (sbc_codec_internal_handle_t *)handle;
    // uint32_t sbc_byte_count = 0;
    // uint32_t pcm_byte_count = 0;

    if ( handle->state != SBC_CODEC_STATE_PLAY ) {
        return -1;
    }
    hal_audio_register_copied_stream_out_callback(NULL, NULL);
    /*Wait encode process stop & clear all data.*/
    /*TASK_LOG_I("flush sbc codec (status:%d)++ \r\n", internal_handle->status);
    hal_audio_register_copied_stream_out_callback(NULL, NULL);
    handle->state = SBC_CODEC_STATE_WAIT_DRAIN;
    if( internal_handle->status&SBC_CODEC_STATUS_IN_TASK_LIST ) {
        TASK_LOG_I("waiting for encode finish processing ++\n");
        if ( internal_handle->status&SBC_CODEC_STATUS_IN_CALLBACK )
            TASK_LOG_W("GET_PAYLOAD callback not finished. \r\n");
        xSemaphoreTake( internal_handle->xSemaphore, portMAX_DELAY );
        TASK_LOG_I("waiting for encode finish processing --\n");
        xSemaphoreGive( internal_handle->xSemaphore );
    }*/
    /*Wait all data drain. (all pcm input need to be encoded & all encoded sbc frames need to be taken).*/
    /*sbc_byte_count = ring_buffer_get_data_byte_count(&internal_handle->p_sbc_frame_ring);
    pcm_byte_count = ring_buffer_get_data_byte_count(&internal_handle->p_pcm_ring);
    if ( sbc_byte_count != 0 || pcm_byte_count != 0) {
        do {
            TASK_LOG_I("sbc_frame:%d pcm_data:%d", sbc_byte_count, pcm_byte_count);
            sbc_codec_encode_process ( handle, NULL ); //force drain
            sbc_byte_count = ring_buffer_get_data_byte_count(&internal_handle->p_sbc_frame_ring);
            pcm_byte_count = ring_buffer_get_data_byte_count(&internal_handle->p_pcm_ring);
        } while( sbc_byte_count != 0 || pcm_byte_count != 0 );
    }
    TASK_LOG_I("flush sbc codec --");
*/
    /*Stop.*/
    handle->state = SBC_CODEC_STATE_STOP;
    sbc_codec_reset_buffer(&internal_handle->p_sbc_frame_ring);
    sbc_codec_reset_buffer(&internal_handle->p_pcm_ring);

    TASK_LOG_I("stop sbc codec --");
    return 0;
}

int32_t sbc_codec_close(sbc_codec_media_handle_t *handle)
{
    TASK_LOG_D("close sbc codec ++ %d \n", handle->state);
    sbc_codec_internal_handle_t *internal_handle = (sbc_codec_internal_handle_t *)handle;

    if ( internal_handle->bli_handle != NULL ) {
        BLI_Close(internal_handle->bli_handle, NULL);
    }
    internal_handle->sbc_encoder_handle = NULL;

    /* deregister */
    sbc_codec_event_deregister_callback( handle );

    if ( sbc_codec_task_handle != NULL) {
        vTaskDelete(sbc_codec_task_handle);
        sbc_codec_task_handle = NULL;
    }
    if ( sbc_codec_queue_handle != NULL) {
        vQueueDelete(sbc_codec_queue_handle);
        sbc_codec_queue_handle = NULL;
    }
    if ( internal_handle->xSemaphore != NULL ) {
        vSemaphoreDelete(internal_handle->xSemaphore);
        internal_handle->xSemaphore = NULL;
    }

    handle->state = SBC_CODEC_STATE_IDLE;

    /*free handle*/
    vPortFree(internal_handle);

    TASK_LOG_D("close sbc codec --");
    return 0;
}

static int32_t sbc_codec_event_register_callback(sbc_codec_media_handle_t *handle, sbc_codec_internal_callback_t callback)
{
    uint32_t id_idx;
    int32_t status = -1;
    for (id_idx = 0; id_idx < SBC_CODEC_MAX_FUNCTIONS; id_idx++) {
        if (sbc_codec_queue_handle_array[id_idx] == NULL) {
            sbc_codec_queue_handle_array[id_idx] = handle;
            sbc_codec_queue_cb_array[id_idx] = callback;
            status = 0;
            break;
        }
    }
    return status;
}

sbc_codec_media_handle_t *sbc_codec_open (sbc_codec_callback_t callback, sbc_codec_initial_parameter_t *param, void *user_data)
{
    TASK_LOG_D("open sbc codec ++");
    uint8_t channel_mode, sample_rate, block_number, subband_num, alloc_method;
    sbc_codec_media_handle_t        *handle;
    sbc_codec_internal_handle_t     *internal_handle;
    sbc_encoder_initial_parameter_t *pParam;
    sbc_codec_sbc_frame_t           *pConfig;
    //uint32_t bit_rate = 400;

    uint32_t bit_rate = 229;
    /* alloc internal handle space */
    internal_handle            = (sbc_codec_internal_handle_t *)pvPortMalloc(sizeof(sbc_codec_internal_handle_t));
    memset(internal_handle, 0, sizeof(sbc_codec_internal_handle_t));
    handle                     = &internal_handle->handle;
    handle->handler            = callback;
    handle->set_buffer         = sbc_codec_set_buffer;
    handle->get_payload        = sbc_codec_get_payload;
    handle->get_payload_done   = sbc_codec_get_payload_done;
    handle->query_payload_size = sbc_codec_query_payload_size;
    handle->set_bit_rate       = sbc_codec_set_bit_rate;
    handle->play               = sbc_codec_play;
    handle->pause              = sbc_codec_pause;
    handle->resume             = sbc_codec_resume;
    handle->stop               = sbc_codec_stop;
    internal_handle->hdl       = (user_data == NULL) ? handle : user_data;

    /* handle sbc information. */
    memcpy(&(internal_handle->codec_info), param, sizeof(sbc_codec_initial_parameter_t));
    pParam  = &(internal_handle->initial_parameter);
    pConfig = &(internal_handle->sbc_frame);

    channel_mode = param->cap.channel_mode;
    sample_rate  = param->cap.sample_rate;
    block_number = param->cap.block_length;
    subband_num  = param->cap.subband_num;
    alloc_method = param->cap.alloc_method;

    switch (channel_mode) {
        case 8:
            pConfig->channel_mode = pParam->channel_mode = SBC_ENCODER_MONO;
            pConfig->channels = 1;
            break;
        case 4:
            pConfig->channel_mode = pParam->channel_mode = SBC_ENCODER_DUAL_CHANNEL;
            pConfig->channels = 2;
            break;
        case 2:
            pConfig->channel_mode = pParam->channel_mode = SBC_ENCODER_STEREO;
            pConfig->channels = 2;
            break;
        case 1:
            pConfig->channel_mode = pParam->channel_mode = SBC_ENCODER_JOINT_STEREO;
            pConfig->channels = 2;
            break;
        default:
            TASK_LOG_E("unknown channel mode %d\r\n", channel_mode);
            vPortFree(internal_handle);
            return 0;
    }

    switch (sample_rate) {
        case 8:
            pParam->sampling_rate = SBC_ENCODER_SAMPLING_RATE_16000HZ;
            pConfig->sampling_frequency = 16000;
            break;
        case 4:
            pParam->sampling_rate = SBC_ENCODER_SAMPLING_RATE_32000HZ;
            pConfig->sampling_frequency = 32000;
            break;
        case 2:
            pParam->sampling_rate = SBC_ENCODER_SAMPLING_RATE_44100HZ;
            pConfig->sampling_frequency = 44100;
            break;
        case 1:
            pParam->sampling_rate = SBC_ENCODER_SAMPLING_RATE_48000HZ;
            pConfig->sampling_frequency = 48000;
            break;
        default:
            TASK_LOG_E("unknown sample rate %d\r\n", sample_rate);
            vPortFree(internal_handle);
            return 0;
    }

    switch (block_number) {
        case 8:
            pParam->block_number = SBC_ENCODER_BLOCK_NUMBER_4;
            pConfig->blocks = 4;
            break;
        case 4:
            pParam->block_number = SBC_ENCODER_BLOCK_NUMBER_8;
            pConfig->blocks = 8;
            break;
        case 2:
            pParam->block_number = SBC_ENCODER_BLOCK_NUMBER_12;
            pConfig->blocks = 12;
            break;
        case 1:
            pParam->block_number = SBC_ENCODER_BLOCK_NUMBER_16;
            pConfig->blocks = 16;
            break;
        default:
            TASK_LOG_E("unknown block %d\r\n", block_number);
            vPortFree(internal_handle);
            return 0;
    }
    pConfig->subbands          = (subband_num == 1) ? 8 : 4;
    pParam->subband_number     = (subband_num == 1) ? SBC_ENCODER_SUBBAND_NUMBER_8 : SBC_ENCODER_SUBBAND_NUMBER_4;
    pParam->allocation_method  = (alloc_method == 1) ? SBC_ENCODER_LOUDNESS : SBC_ENCODER_SNR;
    pConfig->allocation_method = pParam->allocation_method;

    internal_handle->bit_rate = sbc_codec_config(handle, bit_rate);
    TASK_LOG_I("sbc info : sample_rate %d block %d channel %d(%d) subband %d alloc %d bitpool %d frame length %d \r\n",
        pConfig->sampling_frequency, pConfig->blocks, pConfig->channels, pConfig->channel_mode,
        pConfig->subbands, pConfig->allocation_method, pConfig->bitpool, internal_handle->current_frame_size);

    /* initialize */
    handle->waiting                      = false;
    handle->state                        = SBC_CODEC_STATE_IDLE;
    internal_handle->bli_out_sr          = pConfig->sampling_frequency;
    internal_handle->bli_out_ch          = pConfig->channels;
    internal_handle->bli_handle          = NULL;
    internal_handle->subband_buffer_size = -1;
    internal_handle->frame_count         = 0;
    internal_handle->smaple_count        = pConfig->subbands * pConfig->blocks; // 8*16
    sbc_codec_reset_buffer(&internal_handle->p_sbc_frame_ring);
    sbc_codec_reset_buffer(&internal_handle->p_pcm_ring);
    vSemaphoreCreateBinary( internal_handle->xSemaphore );
    sbc_codec_queue_handle = xQueueCreate(20, sizeof(sbc_codec_queue_event_t));
    xTaskCreate(sbc_codec_task_main,
        "sbc_codec_task", 1024 / sizeof(StackType_t), NULL, 7, &sbc_codec_task_handle);
    {   /* Initialize queue registration */
        uint32_t id_idx;
        for (id_idx = 0; id_idx < SBC_CODEC_MAX_FUNCTIONS; id_idx++) {
            sbc_codec_queue_handle_array[id_idx] = NULL;
        }
    }
    if ( sbc_codec_task_handle == NULL || sbc_codec_queue_handle == NULL) {
        TASK_LOG_E("task(%p) or queue(%p) create fail.", sbc_codec_task_handle, sbc_codec_queue_handle);
        return 0;
    }

    /* register */
    sbc_codec_event_register_callback( handle, sbc_codec_encode_process ); // encode task

    TASK_LOG_D("open sbc codec --");
    return handle;
}
