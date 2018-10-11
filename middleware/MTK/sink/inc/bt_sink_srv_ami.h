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

#ifndef __AM_INTERFACE_H__
#define __AM_INTERFACE_H__

#ifdef MTK_AUDIO_MP3_ENABLED
#define __AUDIO_MP3_ENABLE__
#endif

#ifdef MTK_WAV_DECODER_ENABLE
#define __AUDIO_COMMON_CODEC_ENABLE__
#endif

#ifdef MTK_WAV_DECODER_ENABLE
#define __AUDIO_WAV_ENABLE__
#endif

#define __BT_SINK_SRV_ACF_MODE_SUPPORT__

#define STANDALONE_TEST 0
//#define WIN32_UT
#ifndef  WIN32_UT
#define RTOS_TIMER
#define HAL_AUDIO_MODULE_ENABLED
#define __AM_DEBUG_INFO__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#else
#include <stdlib.h>
#include <corecrt_malloc.h>
#endif

#include "bt_codec.h"
#include "bt_a2dp.h"
#include "bt_hfp.h"
#include "hal_audio.h"
#include "hal_audio_enhancement.h"
#include "audio_coefficient.h"
#include "audio_nvdm.h"
#include "bt_type.h"
#include "bt_sink_srv.h"

#ifdef __BT_AWS_SUPPORT__
#include "bt_aws.h"
#include "bt_sink_srv_aws.h"
#endif
#ifdef __AUDIO_MP3_ENABLE__
#include "mp3_codec.h"
#endif
#ifdef __AUDIO_COMMON_CODEC_ENABLE__
#include "audio_codec.h"
#endif
#include "bt_sink_srv_resource.h"


#define FALSE                  0
#define TRUE                   1
#define AM_REGISTER_ID_TOTAL   8
#define DEVICE_IN_LIST        (HAL_AUDIO_DEVICE_MAIN_MIC | HAL_AUDIO_DEVICE_HEADSET_MIC | HAL_AUDIO_DEVICE_LINE_IN | HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC | HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC)
#define DEVICE_OUT_LIST       (HAL_AUDIO_DEVICE_HANDSET | HAL_AUDIO_DEVICE_HANDS_FREE_MONO | HAL_AUDIO_DEVICE_HANDS_FREE_STEREO | HAL_AUDIO_DEVICE_HEADSET | HAL_AUDIO_DEVICE_HEADSET_MONO)
#define DEVICE_LOUDSPEAKER    (HAL_AUDIO_DEVICE_HANDSET | HAL_AUDIO_DEVICE_HANDS_FREE_MONO | HAL_AUDIO_DEVICE_HANDS_FREE_STEREO)
#define DEVICE_EARPHONE       (HAL_AUDIO_DEVICE_HEADSET | HAL_AUDIO_DEVICE_HEADSET_MONO)

#define SPEECH_MODE_HEADSET 0
#define SPEECH_MODE_HANDSFREE 1
#define SPEECH_MODE_HEADSET_DUALMIC	0
/*************************************
*         HAL Struct & Enum
**************************************/
typedef hal_audio_status_t          bt_sink_srv_am_hal_result_t;
typedef hal_audio_event_t           bt_sink_srv_am_event_result_t;
typedef hal_audio_device_t          bt_sink_srv_am_device_set_t;
typedef hal_audio_sampling_rate_t   bt_sink_srv_am_sample_rate_t;
typedef hal_audio_bits_per_sample_t bt_sink_srv_am_bit_per_sample_t;
typedef hal_audio_channel_number_t  bt_sink_srv_am_channel_number_t;
typedef hal_audio_active_type_t     bt_sink_srv_am_active_type_t;
typedef bt_codec_media_event_t      bt_sink_srv_am_bt_event_t;
typedef bt_codec_hfp_audio_t        bt_sink_srv_am_hfp_codec_t;
//typedef bt_codec_a2dp_audio_t       bt_sink_srv_am_a2dp_codec_t;
typedef int8_t                      bt_sink_srv_am_id_t;
typedef uint8_t                     bt_sink_srv_am_volume_level_t;


typedef wchar_t bt_sink_srv_file_path_t;
#define FILE_PATH_TYPE_LEN          (2)

#ifndef WIN32_UT
extern xSemaphoreHandle g_xSemaphore_ami;
#endif
extern bt_sink_srv_am_id_t g_aud_id_num;
#define AUD_ID_INVALID  -1

#ifndef __AUDIO_MP3_ENABLE__
/** @brief This enumeration defines the MP3 codec events. */
typedef enum {
    MP3_CODEC_MEDIA_OK = 0,       /**< The codec operation was successful.   */
    MP3_CODEC_MEDIA_REQUEST,      /**< The MP3 codec requested a bitstream. */
    MP3_CODEC_MEDIA_UNDERFLOW,    /**< The bitstream has an underflow. */
    MP3_CODEC_MEDIA_JUMP_FILE_TO, /**< The MP3 codec requested to jump to specific position on the file. The position is jump_file_to_specified_position which is a member of #mp3_media_handle_s. */
    MP3_CODEC_AWS_CHECK_CLOCK_SKEW,
    MP3_CODEC_AWS_CHECK_UNDERFLOW,
	MP3_CODEC_MEDIA_BITSTREAM_END,/**< Tha MP3 codec play constant array end. */
} bt_sink_srv_mp3_codec_event_t;
#endif


#define BT_SINK_SRV_AWS_SKEW_LOOP_COUNT     (100)

#define BT_SINK_SRV_AWS_SKEW_LOOP_1ST_COUNT     (95)


#define BT_SINK_SRV_AM_PCM_RING_BUFFER_SIZE         (1024 * 3)


/**
 * @defgroup am_enum Enum
 * @{
 */

/**
 *  @brief This enum defines audio handler ID state type.
 */
typedef enum {
    ID_CLOSE_STATE   = 0,
    ID_IDLE_STATE    = 1,
    ID_PLAY_STATE    = 2,
    ID_SUSPEND_STATE = 3,
    ID_RESUME_STATE  = 4,
    ID_STOPING_STATE = 5,
} bt_sink_srv_am_reg_id_state_t;

/**
 *  @brief This enum defines audio handler prioirty type.
 */
typedef enum {
    AUD_LOW     = 0,
    AUD_MIDDLE  = 1,
    AUD_HIGH    = 2
} bt_sink_srv_am_priority_t;

/**
 *  @brief This enum defines audio handler instruction type.
 */
typedef enum {
    AUD_SELF_CMD_REQ      = 0,
    AUD_RESUME_IND        = 1,
    AUD_SUSPEND_IND       = 2,
    AUD_SUSPEND_BY_IND    = 3,
    AUD_A2DP_PROC_IND     = 4,
    AUD_STREAM_EVENT_IND  = 5,
    AUD_TIMER_IND         = 6,
    AUD_SINK_OPEN_CODEC   = 7,
    AUD_HFP_EVENT_IND     = 8,
    AUD_FILE_OPEN_CODEC   = 9,
    AUD_FILE_PROC_IND     = 10,
    /* AWS Group */
    AUD_AWS_SINK_OPEN_CODEC      = 30,
    AUD_AWS_A2DP_PROC_IND        = 31,

    AUD_CB_MSG_TOTAL
} bt_sink_srv_am_cb_msg_class_t;

/**
 *  @brief This enum defines audio handler result of instruction type.
 */
typedef enum {
    AUD_EMPTY                          = 0,
    AUD_SINK_PROC_PTR                  = 1,
    AUD_CMD_FAILURE                    = 2,
    AUD_CMD_COMPLETE                   = 3,
    AUD_SUSPEND_BY_NONE                = 4,
    AUD_SUSPEND_BY_PCM                 = 5,
    AUD_SUSPEND_BY_HFP                 = 6,
    AUD_SUSPEND_BY_A2DP                = 7,
    AUD_SUSPEND_BY_FILES               = 8,
    AUD_SUSPEND_BY_AWS                 = 9,
    AUD_FILE_PROC_PTR                  = 10,
    AUD_A2DP_CODEC_RESTART             = 30,
    AUD_A2DP_AWS_UNDERFLOW             = 35,
    AUD_FILE_EVENT_BASE                = 45,
    AUD_FILE_EVENT_DATA_REQ            = AUD_FILE_EVENT_BASE + 1,
    AUD_FILE_EVENT_JUMP_INFO,
    AUD_FILE_EVENT_DATA_END,
    AUD_FILE_EVENT_UNDERFLOW,
    AUD_FILE_MP3_BITSTREAM_END,
    AUD_STREAM_EVENT_ERROR             = BT_CODEC_MEDIA_ERROR,
    AUD_STREAM_EVENT_NONE              ,
    AUD_STREAM_EVENT_UNDERFLOW         ,
    AUD_STREAM_EVENT_DATA_REQ          ,
    AUD_STREAM_EVENT_DATA_NOTIFICATION ,
    AUD_STREAM_EVENT_TERMINATED        ,
    AUD_CODEC_MEDIA_AWS_CHECK_UNDERFLOW,
    AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW,
    AUD_RESUME_PLAY_STATE              ,
    AUD_RESUME_IDLE_STATE
} bt_sink_srv_am_cb_sub_msg_t;

/**
 *  @brief This enum defines audio layer type.
 */
typedef enum {
    MOD_AMI   = 0,
    MOD_AM    = 1,
    MOD_TMR   = 2,
    MOD_L1SP  = 3
} bt_sink_srv_am_module_t;

/**
 *  @brief This enum defines audio handler message type.
 */
typedef enum {
    MSG_ID_AM_CODE_BEGIN                       = 0,
    MSG_ID_STREAM_OPEN_REQ                     = 1,
    MSG_ID_STREAM_PLAY_REQ                     = 2,
    MSG_ID_STREAM_STOP_REQ                     = 3,
    MSG_ID_STREAM_CLOSE_REQ                    = 4,
    MSG_ID_STREAM_SET_VOLUME_REQ               = 5,
    MSG_ID_STREAM_MUTE_DEVICE_REQ              = 6,
    MSG_ID_STREAM_CONFIG_DEVICE_REQ            = 7,
    MSG_ID_STREAM_READ_WRITE_DATA_REQ          = 8,
    MSG_ID_STREAM_GET_LENGTH_REQ               = 9,
    MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ        = 10,
    MSG_ID_MEDIA_HFP_EVENT_CALL_EXT_REQ        = 11,
    MSG_ID_MEDIA_EVENT_STREAM_OUT_CALL_EXT_REQ = 12,
    MSG_ID_MEDIA_EVENT_STREAM_IN_CALL_EXT_REQ  = 13,
    MSG_ID_TIMER_OUT_CALL_EXT_REQ              = 14,
    MSG_ID_MEDIA_FILE_PROCE_CALL_EXT_REQ       = 15,
    /* NWS Group */
    MSG_ID_MEDIA_AWS_A2DP_PROC_CALL_EXT_REQ    = 30,

    MSG_ID_AM_CODE_END
} bt_sink_srv_am_msg_id_t;

/**
 *  @brief This enum defines audio handler return result.
 */
typedef enum {
    AUD_EXECUTION_FAIL    = -1,
    AUD_EXECUTION_SUCCESS =  0,
} bt_sink_srv_am_result_t;

/**
 *  @brief This enum defines audio stream type.
 */
typedef enum {
    STREAM_IN   = 0x0001,
    STREAM_OUT  = 0x0002
} bt_sink_srv_am_stream_type_t;

/**
 *  @brief This enum defines audio type.
 */
typedef enum {
    PCM   = 0,
    A2DP  = 1,
    HFP   = 2,
    NONE  = 3,
    FILES = 4,
    AWS   = 5,

    AM_TYPE_TOTAL
} bt_sink_srv_am_type_t;

/**
 *  @brief This enum defines file type.
 */
typedef enum {
    FILE_NONE   = 0,
    FILE_MP3    = 1,
    FILE_WAV    = 2,

    FILE_TOAL
} am_file_type_t;

/**
 *  @brief This enum defines audio volume type.
 */
typedef enum {
    AUD_VOL_AUDIO   = 0,
    AUD_VOL_SPEECH  = 1,
    AUD_VOL_TYPE
} bt_sink_srv_am_volume_type_t;


/**
 *  @brief This struct defines audio sink status detail info.
 */
typedef enum a2dp_sink_state {
    A2DP_SINK_CODEC_CLOSE  = 0,
    A2DP_SINK_CODEC_OPEN   = 1,
    A2DP_SINK_CODEC_PLAY   = 2,
    A2DP_SINK_CODEC_STOP   = 3,
    A2DP_SINK_CODEC_TOTAL  = 4
} bt_sink_srv_am_sink_state_t;

/**
 *  @brief This struct defines audio file status.
 */
typedef enum {
    FILE_CODEC_CLOSE  = 0,
    FILE_CODEC_OPEN   = 1,
    FILE_CODEC_PLAY   = 2,
    FILE_CODEC_STOP   = 3,
    FILE_CODEC_PAUSE  = 4,

    FILE_CODEC_TOTAL
} bt_sink_srv_am_file_state_t;

/**
 *  @brief This struct defines audio volume-out level info.
 */
typedef enum {
    AUD_VOL_OUT_LEVEL0 = 0,
    AUD_VOL_OUT_LEVEL1 = 1,
    AUD_VOL_OUT_LEVEL2 = 2,
    AUD_VOL_OUT_LEVEL3 = 3,
    AUD_VOL_OUT_LEVEL4 = 4,
    AUD_VOL_OUT_LEVEL5 = 5,
    AUD_VOL_OUT_LEVEL6 = 6,

    AUD_VOL_OUT_LEVEL7,
    AUD_VOL_OUT_LEVEL8,
    AUD_VOL_OUT_LEVEL9,
    AUD_VOL_OUT_LEVEL10,
    AUD_VOL_OUT_LEVEL11,
    AUD_VOL_OUT_LEVEL12,
    AUD_VOL_OUT_LEVEL13,
    AUD_VOL_OUT_LEVEL14,
    AUD_VOL_OUT_LEVEL15,

    AUD_VOL_OUT_MAX
} bt_sink_srv_am_volume_level_out_t;

/**
 *  @brief This struct defines audio volume-in level info.
 */
typedef enum {
    AUD_VOL_IN_LEVEL0  = 0,
    AUD_VOL_IN_MAX     = 1
} bt_sink_srv_am_volume_level_in_t;

/**
 *  @brief This struct defines audio volume-out device info.
 */
typedef enum {
    LOUDSPEAKER_STREAM_OUT  = 0,
    EARPHONE_STREAM_OUT     = 1,
    DEVICE_OUT_MAX          = 2
} bt_sink_srv_am_device_out_t;

/**
 *  @brief This struct defines audio volume-in device info.
 */
typedef enum {
    MICPHONE_STREAM_IN  = 0,
    DEVICE_IN_MAX       = 1
} bt_sink_srv_am_device_in_t;

typedef enum {
    STREAM_OUT_MIX_METHOD_NO_MIX = BT_CODEC_MEDIA_HFP_MIX_STREAM_OUT_METHOD_NO_MIX,
    STREAM_OUT_MIX_METHOD_REPLACE = BT_CODEC_MEDIA_HFP_MIX_STREAM_OUT_METHOD_REPLACE,
    STREAM_OUT_MIX_METHOD_WEIGHTED_MEAN = BT_CODEC_MEDIA_HFP_MIX_STREAM_OUT_METHOD_WEIGHTED_MEAN,

    STREAM_OUT_MIX_METHOD_END
} bt_sink_srv_stream_out_mix_t;

typedef enum {
    BT_SINK_SRV_VOICE_SYS_POWER_ON = 0,
    BT_SINK_SRV_VOICE_SYS_POWER_OFF,
    BT_SINK_SRV_VOICE_BT_POWER_ON,
    BT_SINK_SRV_VOICE_BT_POWER_OFF,
    BT_SINK_SRV_VOICE_BT_CONNECT,
    BT_SINK_SRV_VOICE_BT_DISCONNECT,

    BT_SINK_SRV_VOICE_END
} bt_sink_srv_voice_t;

/**
 * @}
 */

/**
 * @defgroup am_struct Struct
 * @{
 */

/**
 *  @brief This struct defines audio stream node detail info.
 */
typedef struct audio_stream_node {
    bt_sink_srv_am_sample_rate_t    stream_sample_rate;
    bt_sink_srv_am_bit_per_sample_t stream_bit_rate;
    bt_sink_srv_am_channel_number_t stream_channel;
    void                        *buffer;
    uint32_t                    size;
} bt_sink_srv_am_stream_node_t;

typedef struct {
    bt_a2dp_codec_capability_t codec_cap;  /**< The capabilities of Bluetooth codec */
    bt_a2dp_role_t             role;       /**< The Bluetooth codec roles */
} bt_sink_srv_am_a2dp_codec_t;

#ifdef __BT_AWS_SUPPORT__
typedef struct {
    bt_aws_codec_capability_t codec_cap;  /**< The capabilities of Bluetooth codec */
    bt_aws_role_t             role;       /**< The Bluetooth codec roles */
} bt_sink_srv_am_aws_codec_t;
#endif

/**
 *  @brief This struct defines audio pcm format detail info.
 */
typedef struct am_pcm {
    bt_sink_srv_am_stream_node_t    stream;
    bt_sink_srv_am_stream_type_t    in_out;
    bt_sink_srv_am_event_result_t   event;
} bt_sink_srv_am_pcm_format_t;


/**
 *  @brief This struct defines audio a2dp format detail info.
 */
typedef struct am_a2dp {
    bt_sink_srv_am_a2dp_codec_t    a2dp_codec;
    bt_sink_srv_am_bt_event_t      a2dp_event;
} bt_sink_srv_am_a2dp_format_t;


#ifdef __BT_AWS_SUPPORT__
/**
 *  @brief This struct defines audio aws format detail info.
 */
typedef struct am_aws {
    bt_sink_srv_am_aws_codec_t    aws_codec;
    bt_sink_srv_am_bt_event_t      aws_event;
} bt_sink_srv_am_aws_format_t;
#endif


/**
 *  @brief This struct defines audio hfp format detail info.
 */
typedef struct am_hfp {
    bt_sink_srv_am_hfp_codec_t     hfp_codec;
    bt_sink_srv_am_bt_event_t      hfp_event;
} bt_sink_srv_am_hfp_format_t;

/**
 *  @brief This struct defines file mp3 event type.
 */
 typedef enum {
    AM_MP3_CODEC_MEDIA_OK = MP3_CODEC_MEDIA_OK,                     /**< The codec operation was successful.   */
    AM_MP3_CODEC_MEDIA_REQUEST = MP3_CODEC_MEDIA_REQUEST,           /**< The MP3 codec requested a bitstream. */
    AM_MP3_CODEC_MEDIA_UNDERFLOW = MP3_CODEC_MEDIA_UNDERFLOW,       /**< The bitstream has an underflow. */
    AM_MP3_CODEC_MEDIA_JUMP_FILE_TO = MP3_CODEC_MEDIA_JUMP_FILE_TO,
    AM_MP3_CODEC_AWS_CHECK_CLOCK_SKEW = MP3_CODEC_AWS_CHECK_CLOCK_SKEW,
    AM_MP3_CODEC_AWS_CHECK_UNDERFLOW = MP3_CODEC_AWS_CHECK_UNDERFLOW,
	AM_MP3_CODEC_MEDIA_BITSTREAM_END = MP3_CODEC_MEDIA_BITSTREAM_END,/**< Tha MP3 codec play constant array end. */

    AM_ME3_CODEC_MEDIA_TOTAL
} am_mp3_event_type_t;

/**
 *  @brief This struct defines file mp3 event struct.
 */
 typedef struct {
    am_mp3_event_type_t event;
    void *param;
} am_mp3_event_t;


/**
 *  @brief This struct defines file mp3 event type.
 */
#ifdef __AUDIO_COMMON_CODEC_ENABLE__
typedef enum {
    AM_AUDIO_CODEC_MEDIA_OK        = AUDIO_CODEC_MEDIA_OK,          /**< The codec operation was successful.   */
    AM_AUDIO_CODEC_MEDIA_REQUEST   = AUDIO_CODEC_MEDIA_REQUEST,     /**< The audio codec requested a bitstream. */
    AM_AUDIO_CODEC_MEDIA_UNDERFLOW = AUDIO_CODEC_MEDIA_UNDERFLOW,   /**< The bitstream has an underflow. */
    AM_AUDIO_CODEC_MEDIA_EVENT_END = AUDIO_CODEC_MEDIA_EVENT_END,   /**< The bitstream had been totally streamed out. */

    AM_AUDIO_CODEC_MEDIA_TOTAL
} am_audio_event_type_t;
#else
typedef enum {
    AM_AUDIO_CODEC_MEDIA_OK        = 0,                             /**< The codec operation was successful.   */
    AM_AUDIO_CODEC_MEDIA_REQUEST   = 1,                             /**< The audio codec requested a bitstream. */
    AM_AUDIO_CODEC_MEDIA_UNDERFLOW = 2,                             /**< The bitstream has an underflow. */
    AM_AUDIO_CODEC_MEDIA_EVENT_END = 3,                             /**< The bitstream had been totally streamed out. */

    AM_AUDIO_CODEC_MEDIA_TOTAL
} am_audio_event_type_t;
#endif


/**
 *  @brief This struct defines file audio event struct.
 */
 typedef struct {
    am_audio_event_type_t event;
    void *param;
} am_audio_event_t;


/**
 *  @brief This struct defines file type event struct.
 */
typedef struct {
    uint8_t type;
    union {
        am_mp3_event_t mp3;
        am_audio_event_t audio;
    } event;
} am_file_event_t;

/**
 *  @brief This struct defines audio local music format detail info.
 */
typedef struct am_files {
    ///bt_sink_srv_file_path_t *path;
    am_file_type_t file_type;
    am_file_event_t file_event;
} bt_sink_srv_am_files_format_t;

/**
 *  @brief This struct defines audio codec type detail info.
 */
typedef union audio_codec {
    bt_sink_srv_am_pcm_format_t   pcm_format;
    bt_sink_srv_am_a2dp_format_t  a2dp_format;
    bt_sink_srv_am_hfp_format_t   hfp_format;
    bt_sink_srv_am_files_format_t files_format;
    #ifdef __BT_AWS_SUPPORT__
    bt_sink_srv_am_aws_format_t   aws_format;
    #endif
} bt_sink_srv_am_codec_t;

/**
 *  @brief This struct defines sink media handle detail info.
 */
typedef struct {
    bt_status_t (*get_write_buffer)(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length);
    bt_status_t (*write_data_done)(bt_sink_srv_am_id_t aud_id, uint32_t length);
    bt_status_t (*finish_write_data)(bt_sink_srv_am_id_t aud_id);
    int32_t (*get_free_space)(bt_sink_srv_am_id_t aud_id);
    void (*reset_share_buffer)(bt_sink_srv_am_id_t aud_id);
    bt_status_t (*play)(bt_sink_srv_am_id_t aud_id);
    bt_status_t (*stop)(bt_sink_srv_am_id_t aud_id);
    #ifdef __BT_AWS_SUPPORT__
    int32_t (*set_aws_flag)(bt_sink_srv_am_id_t aud_id, bool flag);
    int32_t (*set_aws_initial_sync)(bt_sink_srv_am_id_t aud_id);
    void (*aws_plh_init)(bt_sink_srv_am_id_t aud_id);
    void (*aws_plh_deinit)(void);
    #endif
    bt_media_handle_t *med_hd;
} bt_sink_srv_am_media_handle_t;

/**
 *  @brief This struct defines mp3 media handle detailed information.
 */
typedef struct {
    void (*get_write_buffer)(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length);                 /**< Get the available length to write into a shared buffer and a pointer to the shared buffer. */
    void (*get_read_buffer)(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length);                 /**< Get the available length to write into a shared buffer and a pointer to the shared buffer. */
    void (*write_data_done)(bt_sink_srv_am_id_t aud_id, uint32_t length);                                     /**< Update the write pointer to the shared buffer. */
    void (*finish_write_data)(bt_sink_srv_am_id_t aud_id);                                                    /**< Indicate last data transfer. */
    int32_t (*get_data_count)(bt_sink_srv_am_id_t aud_id);                                          /**< Get the data length from the shared buffer. */
    int32_t (*get_free_space)(bt_sink_srv_am_id_t aud_id);                                          /**< Get the available length from the shared buffer. */
    int32_t (*play)(bt_sink_srv_am_id_t aud_id);                                                    /**< The MP3 codec play function. */
    int32_t (*pause)(bt_sink_srv_am_id_t aud_id);                                                   /**< The MP3 codec pause function. */
    int32_t (*resume)(bt_sink_srv_am_id_t aud_id);                                                  /**< The MP3 codec resume function. */
    int32_t (*stop)(bt_sink_srv_am_id_t aud_id);                                                    /**< The MP3 codec stop function. */
    int32_t (*close_codec)(bt_sink_srv_am_id_t aud_id);                                             /**< The MP3 codec close_codec function. */
    int32_t (*skip_id3v2_and_reach_next_frame)(bt_sink_srv_am_id_t aud_id, uint32_t file_size);     /**< Skip id3v2 header and reach next frame */
    int32_t (*set_silence_frame_information)(bt_sink_srv_am_id_t aud_id, silence_frame_information_t *frm_info);    /**< Set the silence frame information. */
    int32_t (*fill_silence_frame)(bt_sink_srv_am_id_t aud_id, uint8_t *buffer);                                     /**< Get the silence frame. */
    int32_t (*get_data_status)(bt_sink_srv_am_id_t aud_id, mp3_codec_data_type_t type, int32_t *status);
    int32_t (*flush)(bt_sink_srv_am_id_t aud_id, int32_t flush_data_flag);

#ifdef __BT_AWS_SUPPORT__
    int32_t (*set_aws_flag)(bt_sink_srv_am_id_t aud_id, bool flag);
    int32_t (*set_aws_initial_sync)(bt_sink_srv_am_id_t aud_id);
    int32_t (*aws_init)(void);
    int32_t (*aws_deinit)(void);
    int32_t (*aws_set_clock_skew_compensation_value)(int32_t sample_count);
    int32_t (*aws_get_clock_skew_status)(int32_t *status);
    int32_t (*aws_set_clock_skew)(bool flag);
#endif /* __BT_AWS_SUPPORT__ */
} bt_sink_srv_am_mp3_media_handle_t;

/**
 *  @brief This struct defines file media handle detailed information.
 */
typedef struct {
    am_file_type_t type;
    union {
        bt_sink_srv_am_mp3_media_handle_t mp3;
    } media_handle;
} bt_sink_srv_am_files_media_handle_t;

/**
 *  @brief This struct defines audio common stream-in info.
 */
typedef struct {
    bt_sink_srv_am_device_set_t        audio_device;
    bt_sink_srv_am_volume_level_in_t   audio_volume;
    bool                           audio_mute;
} bt_sink_srv_am_audio_stream_in_t;

/**
 *  @brief This struct defines audio common stream-out info.
 */
typedef struct {
    bt_sink_srv_am_device_set_t         audio_device;
    bt_sink_srv_am_volume_level_out_t   audio_volume;
    bool                            audio_mute;
} bt_sink_srv_am_audio_stream_out_t;


/**
 *  @brief This struct defines audio capability detail info.
 */
typedef struct audio_capability {
    bt_sink_srv_am_type_t               type;
    bt_sink_srv_am_codec_t 		        codec;
    bt_sink_srv_am_audio_stream_in_t    audio_stream_in;
    bt_sink_srv_am_audio_stream_out_t   audio_stream_out;
    bt_sink_srv_am_active_type_t        audio_path_type;
    bt_bd_addr_t dev_addr;
} bt_sink_srv_am_audio_capability_t;

/** @brief define audio callback function prototype for notification */
typedef void(*bt_sink_srv_am_notify_callback)(bt_sink_srv_am_id_t aud_id,
        bt_sink_srv_am_cb_msg_class_t msg_id,
        bt_sink_srv_am_cb_sub_msg_t sub_msg,
        void *parm);

/**
 *  @brief This struct defines audio handler in the background detail info.
 */
typedef struct am_background {
    bt_sink_srv_am_id_t                 aud_id;
    bt_sink_srv_am_type_t               type;
    bt_sink_srv_am_priority_t           priority;
    bt_sink_srv_am_stream_type_t        in_out;
    bt_sink_srv_am_audio_stream_in_t    audio_stream_in;
    bt_sink_srv_am_audio_stream_out_t   audio_stream_out;
    bt_sink_srv_am_active_type_t        audio_path_type;
    bt_sink_srv_am_notify_callback      notify_cb;
    uint32_t                        *data_length_ptr;
    struct am_background            *prior;
    struct am_background            *next;
    bt_sink_srv_am_codec_t              local_context;
} bt_sink_srv_am_background_t;
extern bt_sink_srv_am_background_t *g_prCurrent_player;

/**
 *  @brief This struct defines audio ID detail info.
 */
typedef struct {
    bt_sink_srv_am_reg_id_state_t   use;
    struct am_background        *contain_ptr;
} bt_sink_srv_am_aud_id_type_t;
extern bt_sink_srv_am_aud_id_type_t g_rAm_aud_id[AM_REGISTER_ID_TOTAL];

/**
 *  @brief This struct defines message of audio manager detail info.
 */
typedef struct amm {
    bt_sink_srv_am_cb_msg_class_t   cb_msg_id;
    bt_sink_srv_am_module_t         src_mod_id;      /* Source module ID of the message. */
    bt_sink_srv_am_module_t         dest_mod_id;     /* Destination module ID of the message. */
    bt_sink_srv_am_msg_id_t         msg_id;          /* Message identifier */
    bt_sink_srv_am_background_t     background_info; /* background information*/
} bt_sink_srv_am_amm_struct;
#ifdef WIN32_UT
extern bt_sink_srv_am_amm_struct *g_prAmm_current;
#endif
extern bt_sink_srv_am_amm_struct *ptr_callback_amm;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief                   This function is employed to open a new audio handler to get the registered ID.
 * @param[in] priority      Priority level
 * @param[in] handler       Callback function for A.M. notification
 * @return                  A valid registered ID that is 0~(AM_REGISTER_ID_TOTAL-1) on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_id_t bt_sink_srv_ami_audio_open(bt_sink_srv_am_priority_t priority, bt_sink_srv_am_notify_callback handler);

/**
 * @brief                   This function is employed to play the specified audio handler.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] capability_t  Representing the audio content format
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_play(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_audio_capability_t *capability_t);

/**
 * @brief                   This function is employed to stop playing the specified audio handler.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_stop(bt_sink_srv_am_id_t aud_id);

/**
 * @brief                   This function is employed to close the opened audio handler.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_close(bt_sink_srv_am_id_t aud_id);

/**
 * @brief                   This function is employed to set audio in/out volume.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] volume        Representing the audio volume level
 * @param[in] in_out        STREAM_IN / STREAM_OUT
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_set_volume(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_volume_level_t volume_level, bt_sink_srv_am_stream_type_t in_out);

/**
 * @brief                   This function is employed to mute on audio stream in/out.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] mute          TRUE/FALSE
 * @param[in] in_out        STREAM_IN / STREAM_OUT
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_set_mute(bt_sink_srv_am_id_t aud_id, bool mute, bt_sink_srv_am_stream_type_t in_out);

/**
 * @brief                   This function is employed to set audio device.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] device        HAL_AUDIO_DEVICE_NONE ~ HAL_AUDIO_DEVICE_LINEIN
 * @param[in] in_out        STREAM_IN / STREAM_OUT
 * @return	                AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_set_device(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_device_set_t device);

/**
 * @brief                   This function is employed to write data for palyback / Read data for record.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] buffer        Pointer to user's data buffer for writing or reading usage
 * @param[in] data_count    Output data count for writing; recieve data count for reading
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_continue_stream(bt_sink_srv_am_id_t aud_id, void *buffer, uint32_t data_count);

/**
 * @brief                   This function is employed to query available input/output data length.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[out]data_length   Available input/output data length
 * @param[in] in_out        STREAM_IN / STREAM_OUT
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_get_stream_length(bt_sink_srv_am_id_t aud_id, uint32_t *data_length, bt_sink_srv_am_stream_type_t in_out);

//extern void bt_sink_srv_ami_send_amm(bt_sink_srv_am_module_t dest_id, bt_sink_srv_am_module_t src_id, bt_sink_srv_am_cb_msg_class_t cb_msg_id, bt_sink_srv_am_msg_id_t msg_id, bt_sink_srv_am_background_t *background_info);

extern void bt_sink_srv_ami_send_amm(bt_sink_srv_am_module_t dest_id,
                                     bt_sink_srv_am_module_t src_id,
                                     bt_sink_srv_am_cb_msg_class_t cb_msg_id,
                                     bt_sink_srv_am_msg_id_t msg_id,
                                     bt_sink_srv_am_background_t *background_info,
                                     uint8_t fromISR,
                                     bt_sink_srv_am_amm_struct *pr_Amm);

#ifdef __BT_SINK_SRV_ACF_MODE_SUPPORT__
void bt_sink_srv_set_acf_mode(uint8_t mode);
#endif

#ifdef __cplusplus
}
#endif

extern bt_bd_addr_t g_int_dev_addr;

#endif /*__AM_INTERFACE_H__*/

