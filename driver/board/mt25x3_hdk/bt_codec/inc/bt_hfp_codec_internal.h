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

#ifndef __BT_HFP_CODEC_INTERNAL_H__
#define __BT_HFP_CODEC_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "bt_codec.h"
#include "hal_audio_internal_service.h"
#include "hal_audio_internal_pcm2way.h"
#ifndef HAL_AUDIO_MODULE_ENABLED
#error "please turn on audio feature option on hal_feature_config.h"
#endif
#include "hal_audio_fw_sherif.h"
#include "hal_gpt.h"
#include "hal_eint.h"
#include "mt2523.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "syslog.h"
#include <string.h>


#define REMAINED_PKT_CNT(w, r)  ((w) > (r) ? (w) - (r) : (w) < (r) ? (uint32_t)0xFFFFFFFF - (r) + (w) + 1 : 0)

#define BT_HFP_TRUE     1
#define BT_HFP_FALSE    0

#define CVSD_ENCODER_INTERNAL_BUFFER_SIZE   FOUR_BYTE_ALIGNED(80 + 12)
#define CVSD_DECODER_INTERNAL_BUFFER_SIZE   FOUR_BYTE_ALIGNED(12 + 120)
#define CVSD_CODEC_INTERNAL_BUFFER_SIZE     (CVSD_ENCODER_INTERNAL_BUFFER_SIZE + CVSD_DECODER_INTERNAL_BUFFER_SIZE)
#define MSBC_ENCODER_INTERNAL_BUFFER_SIZE   FOUR_BYTE_ALIGNED(1504)
#define MSBC_DECODER_INTERNAL_BUFFER_SIZE   FOUR_BYTE_ALIGNED(1984)
#define MSBC_CODEC_INTERNAL_BUFFER_SIZE     (MSBC_ENCODER_INTERNAL_BUFFER_SIZE + MSBC_DECODER_INTERNAL_BUFFER_SIZE)
#define PLC_INTERNAL_BUFFER_SIZE            FOUR_BYTE_ALIGNED(6480)

#define CVSD_ENCODER_TEMPORARY_BUFFER_SIZE  FOUR_BYTE_ALIGNED(1024)
#define CVSD_DECODER_TEMPORARY_BUFFER_SIZE  FOUR_BYTE_ALIGNED(1024)
#define MSBC_ENCODER_TEMPORARY_BUFFER_SIZE  FOUR_BYTE_ALIGNED(0)
#define MSBC_DECODER_TEMPORARY_BUFFER_SIZE  FOUR_BYTE_ALIGNED(0)
#define PLC_TEMPORARY_BUFFER_SIZE           FOUR_BYTE_ALIGNED(0)
#define TEMPORARY_LAYER_1_BUFFER_SIZE       MAXIMUM(CVSD_ENCODER_TEMPORARY_BUFFER_SIZE, CVSD_DECODER_TEMPORARY_BUFFER_SIZE)
#define TEMPORARY_LAYER_2_BUFFER_SIZE       MAXIMUM(TEMPORARY_LAYER_1_BUFFER_SIZE, MSBC_ENCODER_TEMPORARY_BUFFER_SIZE)
#define TEMPORARY_LAYER_3_BUFFER_SIZE       MAXIMUM(TEMPORARY_LAYER_2_BUFFER_SIZE, MSBC_DECODER_TEMPORARY_BUFFER_SIZE)
#define TEMPORARY_LAYER_4_BUFFER_SIZE       MAXIMUM(TEMPORARY_LAYER_3_BUFFER_SIZE, PLC_TEMPORARY_BUFFER_SIZE)

#define BT_HFP_CODEC_INTERNAL_BUFFER_SIZE   MAXIMUM(CVSD_CODEC_INTERNAL_BUFFER_SIZE, MSBC_CODEC_INTERNAL_BUFFER_SIZE)
#define BT_HFP_PLC_INTERNAL_BUFFER_SIZE     PLC_INTERNAL_BUFFER_SIZE
#define BT_HFP_TEMPORARY_BUFFER_SIZE        TEMPORARY_LAYER_4_BUFFER_SIZE

#define BTSRAM_BASE                (0xA3000000)
#define BT_HFP_HW_REG_PACKET_R     ((volatile uint32_t *)(PKV_BASE + 0x0FD0))
#define BT_HFP_HW_REG_PACKET_W     ((volatile uint32_t *)(PKV_BASE + 0x0FD4))
#define BT_HFP_HW_REG_CONTROL      ((volatile uint32_t *)(PKV_BASE + 0x0FD8))

#define BT_HFP_HW_TX_NREADY    ((uint32_t)(1<<21))
#define BT_HFP_HW_RX_READY     ((uint32_t)(1<<22))
#define BT_HFP_HW_TX_UNDERFLOW ((uint32_t)(1<<23))
#define BT_HFP_HW_RX_OVERFLOW  ((uint32_t)(1<<24))
#define BT_HFP_HW_INTERRUPT    ((uint32_t)(1U<<31))
#define BT_HFP_HW_CTRL_MASK    (BT_HFP_HW_TX_NREADY | BT_HFP_HW_RX_READY | BT_HFP_HW_TX_UNDERFLOW | BT_HFP_HW_RX_OVERFLOW | BT_HFP_HW_INTERRUPT)

#define BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET             (60)
#define BT_HFP_TX_PACKET_NUM                            (1 << 3)
#define BT_HFP_TX_PACKET_MASK                           (BT_HFP_TX_PACKET_NUM - 1)
#define BT_HFP_TX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET  (BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET * sizeof(int16_t))
#define BT_HFP_TX_CVSD_PCM_64KHZ_SAMPLE_PER_PACKET      (BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET * 8)
#define BT_HFP_TX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET (BT_HFP_TX_CVSD_PCM_64KHZ_SAMPLE_PER_PACKET * sizeof(int16_t))
#define BT_HFP_TX_CVSD_PCM_TEMP_BUFFER_SIZE_PER_PACKET  (BT_HFP_TX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET)
#define BT_HFP_TX_MSBC_PCM_16KHZ_SAMPLE_PER_FRAME       (120)                                                         /* 120 samples @ 16kHz (= 7.5 ms) */
#define BT_HFP_TX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME  (BT_HFP_TX_MSBC_PCM_16KHZ_SAMPLE_PER_FRAME * sizeof(int16_t)) /* 120 samples @ 16kHz (= 7.5 ms) */
#define BT_HFP_TX_MSBC_BITSTREAM_SIZE_PER_FRAME         (57)                                                          /* 120 samples @ 16kHz (= 7.5 ms) */
#define BT_HFP_TX_MSBC_BITSTREAM_BUFFER_SIZE_PER_FRAME  FOUR_BYTE_ALIGNED(BT_HFP_TX_MSBC_BITSTREAM_SIZE_PER_FRAME)    /* 60 bytes */
#define BT_HFP_TX_BITSTREAM_BUFFER_SIZE                 (BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET * BT_HFP_TX_PACKET_NUM)
#define BT_HFP_TX_PCM_RING_BUFFER_SIZE                  (2560)
#define BT_HFP_TX_PCM_RING_BUFFER_MIRROR_END            (BT_HFP_TX_PCM_RING_BUFFER_SIZE << 1)

#define BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET             (30)
#define BT_HFP_RX_PACKET_NUM                            (1 << 4)
#define BT_HFP_RX_PACKET_MASK                           (BT_HFP_RX_PACKET_NUM - 1)
#define BT_HFP_RX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET (BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET * sizeof(int16_t) * 8)
#define BT_HFP_RX_CVSD_PCM_TEMP_BUFFER_SIZE_PER_PACKET  (BT_HFP_RX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET)
#define BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET  (BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET * sizeof(int16_t)) /* 30 samples @ 8kHz (= 3.75 ms) */
#define BT_HFP_RX_CVSD_PCM_64KHZ_HALF_WORD_PER_PACKET   (BT_HFP_RX_CVSD_PCM_64KHZ_BUFFER_SIZE_PER_PACKET / sizeof(int16_t))
#define BT_HFP_RX_CVSD_PCM_TEMP_HALF_WORD_PER_PACKET    (BT_HFP_RX_CVSD_PCM_TEMP_BUFFER_SIZE_PER_PACKET / sizeof(int16_t))
#define BT_HFP_RX_CVSD_PCM_8KHZ_HALF_WORD_PER_PACKET    (BT_HFP_RX_CVSD_PCM_8KHZ_BUFFER_SIZE_PER_PACKET / sizeof(int16_t))
#define BT_HFP_RX_MSBC_PCM_16KHZ_BUFFER_SIZE_PER_FRAME  (120 * sizeof(int16_t)) /* 120 samples @ 16kHz (= 7.5 ms) */
#define BT_HFP_RX_MSBC_BITSTREAM_SIZE_PER_FRAME         (57)                    /* 120 samples @ 16kHz (= 7.5 ms) */
#define BT_HFP_RX_MSBC_BITSTREAM_BUFFER_SIZE_PER_FRAME  FOUR_BYTE_ALIGNED(BT_HFP_RX_MSBC_BITSTREAM_SIZE_PER_FRAME)  /* 60 bytes */
#define BT_HFP_RX_BITSTREAM_BUFFER_SIZE                 (BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET * BT_HFP_RX_PACKET_NUM)
#define BT_HFP_RX_PCM_RING_BUFFER_SIZE                  (1280)
#define BT_HFP_RX_PCM_RING_BUFFER_MIRROR_END            (BT_HFP_RX_PCM_RING_BUFFER_SIZE << 1)

#define BT_HFP_HW_SRAM_SIZE         (180)
#define BT_HFP_HW_SRAM_WORD         (BT_HFP_HW_SRAM_SIZE / sizeof(uint32_t))
#define BT_HFP_HW_SRAM_PKT_60_SIZE  (60)
#define BT_HFP_HW_SRAM_PKT_30_SIZE  (30)
#define BT_HFP_HW_SRAM_PKT_20_SIZE  (20)
#define BT_HFP_HW_SRAM_PKT_10_SIZE  (10)
#define BT_HFP_HW_SRAM_PKT_60_NUM   (BT_HFP_HW_SRAM_SIZE / BT_HFP_HW_SRAM_PKT_60_SIZE)
#define BT_HFP_HW_SRAM_PKT_30_NUM   (BT_HFP_HW_SRAM_SIZE / BT_HFP_HW_SRAM_PKT_30_SIZE)
#define BT_HFP_HW_SRAM_PKT_20_NUM   (BT_HFP_HW_SRAM_SIZE / BT_HFP_HW_SRAM_PKT_20_SIZE)
#define BT_HFP_HW_SRAM_PKT_10_NUM   (BT_HFP_HW_SRAM_SIZE / BT_HFP_HW_SRAM_PKT_10_SIZE)
#define BT_HFP_HW_SRAM_TX_PKT_CNT   (BT_HFP_HW_SRAM_SIZE / BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET)
#define BT_HFP_HW_SRAM_RX_PKT_CNT   (BT_HFP_HW_SRAM_SIZE / BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET)
#define BT_HFP_PKT_TYPE_IDX_MAX 4

#define BT_HFP_DSP_FRAME_SIZE       (640)   /* (20 * 16 * sizeof(uint16_t)) */
#define BT_HFP_DSP_FRAME_HALF_WORD  (BT_HFP_DSP_FRAME_SIZE / sizeof(uint16_t))

#define BT_CODEC_TASK_PRIORITY      (6)
#define BT_CODEC_TASK_STACK_SIZE    (1024)
#define BT_CODEC_QUEUE_SIZE         20
#define MAX_BT_CODEC_FUNCTIONS      5

#define ASSIGN_HANDLE_UPDATE_POINTER(pointer, handle, size) do {    \
    handle = (void *)pointer;                                       \
    pointer += size;                                                \
} while(0)
#define ASSIGN_U16_BUFFER_UPDATE_POINTER(pointer, u16_buffer, size) do {    \
    u16_buffer = (uint16_t *)pointer;                                       \
    pointer += size;                                                        \
} while(0)
#define ASSIGN_S16_BUFFER_UPDATE_POINTER(pointer, s16_buffer, size) do {    \
    s16_buffer = (int16_t *)pointer;                                        \
    pointer += size;                                                        \
} while(0)
#define ASSIGN_U8_BUFFER_UPDATE_POINTER(pointer, u8_buffer, size) do {  \
    u8_buffer = (uint8_t *)pointer;                                     \
    pointer += size;                                                    \
} while(0)
#define ASSIGN_U8_TX_2D_BUFFER_UPDATE_POINTER(pointer, u8_tx_2d_buffer, size) do {  \
    u8_tx_2d_buffer = (uint8_t (*)[BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET])pointer;    \
    pointer += size;                                                                \
} while(0)
#define ASSIGN_U8_RX_2D_BUFFER_UPDATE_POINTER(pointer, u8_rx_2d_buffer, size) do {  \
    u8_rx_2d_buffer = (uint8_t (*)[BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET])pointer;    \
    pointer += size;                                                                \
} while(0)

typedef enum {
    BT_HFP_PKT_LEN_30  = 0, /* HV3   --> MTK SCO*/
    /* EV3   --> MTK eSCO */
    BT_HFP_PKT_LEN_60  = 1, /* 2-EV3 --> MTK eSCO */
    BT_HFP_PKT_LEN_90  = 2, /* 3-EV3 --> N/A */
    BT_HFP_PKT_LEN_120 = 3, /* EV4   --> N/A */
    /* EV5   --> N/A */
    /* 2-EV5 --> N/A */
    /* 3-EV5 --> N/A */
    BT_HFP_PKT_LEN_10  = 4, /* HV1   --> MTK SCO */
    BT_HFP_PKT_LEN_20  = 5  /* HV2   --> MTK SCO */
} bt_hfp_packet_length_t;

/** @brief Bluetooth HFP loopback mode */
typedef enum {
    BT_HF_LOOPBACK_WITHOUT_CODEC,   /**< Bluetooth handsfree profile loopback without codec */
    BT_HF_LOOPBACK_WITH_CODEC       /**< Bluetooth handsfree profile loopback with codec */
} bt_hf_loopback_mode_t;

typedef enum {
    BT_HFP_MODE_NONE = 0,
    BT_HFP_MODE_SPEECH,
    BT_HFP_MODE_TX_ONLY,
    BT_HFP_MODE_RX_ONLY,
    BT_HFP_MODE_LOOPBACK_WITH_CODEC,
    BT_HFP_MODE_LOOPBACK_WITHOUT_CODEC
} bt_hfp_mode_t;

typedef enum {
    BT_CODEC_QUEUE_EVENT_NONE = 0,
    BT_CODEC_QUEUE_EVENT_HFP_RX_PROCESS,
    BT_CODEC_QUEUE_EVENT_DSP_TX_PROCESS,
    BT_CODEC_QUEUE_EVENT_DSP_RX_PROCESS,
    BT_CODEC_QUEUE_EVENT_PLAYBACK_PROCESS,
    BT_CODEC_QUEUE_EVENT_RECORD_PROCESS,
    BT_CODEC_QUEUE_EVENT_TOTAL
} bt_codec_queue_event_id_t;

typedef void (*bt_codec_callback_t)(void *parameter);

/** @brief HFP loopback structure */
typedef struct {
    bt_hfp_audio_codec_type_t type;   /**< 1: CVSD, 2: mSBC */
    bt_hf_loopback_mode_t mode;
} bt_hf_loopback_t;

typedef struct {
    uint32_t packet_size;
    uint32_t packet_number;
    uint32_t tx_packet_count_per_interrupt;
    uint32_t rx_packet_count_per_interrupt;
} bt_hfp_packet_information_t;

typedef struct {
    uint32_t internal_buffer_size;
    void *handle;
} bt_hfp_library_information_t;

typedef struct {
    uint32_t base_address;
    volatile uint32_t *p_offset_r;
    volatile uint32_t *p_offset_w;
    volatile uint32_t *p_control;
} bt_hfp_shared_memory_information_t;

typedef struct {
    bt_codec_hfp_customer_enhancement_function_callback_t callback_function;
    void *data;
    bool waiting;
} bt_hfp_customer_enhancement_function_callback_t;

typedef struct {
    bt_codec_hfp_get_stream_out_data_callback_t callback_function;
    void *data;
    bool waiting;
} bt_hfp_get_stream_out_data_callback_t;

typedef struct {
    bt_codec_hfp_get_stream_in_data_callback_t callback_function;
    void *data;
    bool waiting;
} bt_hfp_get_stream_in_data_callback_t;

typedef struct {
    bt_codec_hfp_mix_stream_out_data_callback_t callback_function;
    void *data;
    bool waiting;
} bt_hfp_mix_stream_out_data_callback_t;


typedef struct {
    uint32_t                     buffer_size;
    uint8_t                     *buffer_pointer;
    bt_hfp_library_information_t encoder;
    bt_hfp_library_information_t src_up_sampling;
    uint32_t                     pkt_w; /* Unit: packet */
    uint32_t                     pkt_r; /* Unit: packet */
    uint8_t                    (*pkt_buf)[BT_HFP_TX_BITSTREAM_SIZE_PER_PACKET];
    uint32_t                     pkt_underflow;
    uint32_t                     pcm_w; /* Unit: byte */
    int16_t                     *pcm_8khz_buf;
    int16_t                     *pcm_64khz_buf;
    int16_t                     *pcm_tmp_buf;
    int16_t                     *pcm_16khz_buf;
    ring_buffer_information_t    pcm_ring_info;
    uint8_t                     *pcm_ring_buf;
    uint32_t                     pcm_overflow;
    uint16_t                    *pcm_tx_tmp_buf;
} bt_hfp_speech_tx_information_t;

typedef struct {
    uint32_t                     buffer_size;
    uint8_t                     *buffer_pointer;
    bt_hfp_library_information_t decoder;
    bt_hfp_library_information_t src_down_sampling;
    bt_hfp_library_information_t plc;
    uint32_t                     pkt_w; /* Unit: packet */
    uint32_t                     pkt_r; /* Unit: packet */
    uint8_t                    (*pkt_buf)[BT_HFP_RX_BITSTREAM_SIZE_PER_PACKET];
    uint8_t                     *pkt_tmp_buf;
    uint32_t                     pkt_overflow;
    uint32_t                     pkt_valid[BT_HFP_RX_PACKET_NUM];
    uint32_t                     pkt_lost_cnt;
    uint32_t                     pcm_r; /* Unit: byte */
    int16_t                     *pcm_64khz_buf;
    int16_t                     *pcm_8khz_buf;
    int16_t                     *pcm_tmp_buf;
    int16_t                     *pcm_16khz_buf;
    ring_buffer_information_t    pcm_ring_info;
    uint8_t                     *pcm_ring_buf;
    uint32_t                     pcm_underflow;
    uint16_t                    *pcm_rx_tmp_buf;
    uint32_t                     acc_bad;
    uint32_t                     acc_good;
    uint32_t                     acc_headfail;
} bt_hfp_speech_rx_information_t;

typedef struct {
    bt_media_handle_t                  handle;
    bt_hfp_audio_codec_type_t          codec_type;
    bt_hfp_mode_t                      mode;
    hal_eint_number_t                  eint_number;
    hal_eint_config_t                  eint_config;
    hal_eint_status_t                  eint_status;
    hal_eint_callback_t                eint_callback;
    bt_hfp_speech_tx_information_t     tx_info;
    bt_hfp_speech_rx_information_t     rx_info;
    bt_hfp_shared_memory_information_t mem_info;
    bt_codec_hfp_device_path_t         device_path;
    bt_hfp_customer_enhancement_function_callback_t    customer_enhancement_callback;
    bt_hfp_get_stream_out_data_callback_t    get_stream_out_callback;
    bt_hfp_get_stream_in_data_callback_t    get_stream_in_callback;
    bt_hfp_mix_stream_out_data_callback_t    mix_stream_out_callback;
    bt_codec_media_hfp_mix_stream_out_method_t mix_stream_out_method;
    uint8_t                            *stream_out_data;
    uint8_t                            *stream_in_data;
    uint8_t                            *customer_enhanced_stream_in_data;   /* this is for customer enhancement used*/
    uint8_t                            *mix_stream_out_data;
    uint32_t                          *pkt_tmp_buf;
    uint32_t                           control_reg;
    uint32_t                           isr_time;
} bt_hfp_codec_internal_handle_t;

typedef struct {
    bt_codec_queue_event_id_t id;
    void *parameter;
} bt_codec_queue_event_t;

/**
 * @brief     Open HFP loopback test
 * @param[in] param is the HFP loopback settings
 * @return    Media handle of HFP
 * @sa        #bt_hf_close_loopback()
 */
bt_media_handle_t *bt_hf_open_loopback(const bt_hf_loopback_t *param);

/**
 * @brief     Close HFP loopback
 * @return    #BT_CODEC_MEDIA_STATUS_MEDIA_OK, if OK
 * @sa        #bt_hf_open_loopback()
 */
bt_codec_media_status_t bt_hf_close_loopback(void);

void bt_codec_task_create(void);

void bt_hfp_set_shared_memory_information (uint32_t base_address, volatile uint32_t *p_offset_r, volatile uint32_t *p_offset_w, volatile uint32_t *p_control);
void bt_hfp_ut_process (void);

void bt_codec_event_register_callback (bt_codec_queue_event_id_t reg_id, bt_codec_callback_t callback);
void bt_codec_event_deregister_callback (bt_codec_queue_event_id_t dereg_id);
void bt_codec_event_send_from_isr (bt_codec_queue_event_id_t id, void *parameter);

bool bt_hfp_codec_query_is_running(void);
hal_audio_sampling_rate_t bt_hfp_codec_query_sampling_rate(void);
hal_audio_channel_number_t bt_hfp_codec_query_channel_number(void);

#if defined(MTK_BT_HFP_CODEC_DEBUG)
typedef enum {
    BT_HFP_CODEC_DEBUG_FLAG_NONE = 0,
    BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_ESCO_RAW_DATA = 1 << 0,
    BT_HFP_CODEC_DEBUG_FLAG_DOWNLINK_STREAM_OUT_PCM = 1 << 1,
    BT_HFP_CODEC_DEBUG_FLAG_VM_LOG = 1 << 2
} bt_hfp_codec_debug_flag_t;

typedef enum {
    BT_HFP_CODEC_DEBUG_SAVE_OR_PRINT_METHOD_NONE = 0,
    BT_HFP_CODEC_DEBUG_SAVE_OR_PRINT_METHOD_SAVE_TO_SDCARD = 1 << 0,
    BT_HFP_CODEC_DEBUG_SAVE_OR_PRINT_METHOD_PRINT_TO_USB_DEBUG_PORT = 1 << 1,
    BT_HFP_CODEC_DEBUG_SAVE_OR_PRINT_METHOD_PRINT_TO_HCI_PORT = 1 << 2
} bt_hfp_codec_debug_save_or_print_method_t;


void bt_hfp_codec_debug_open(bt_hfp_codec_debug_flag_t bt_hfp_codec_debug_flag, bt_hfp_codec_debug_save_or_print_method_t bt_hfp_codec_debug_save_or_print_method);
void bt_hfp_codec_debug_close(void);

#endif

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /*__BT_HFP_CODEC_INTERNAL_H__*/
