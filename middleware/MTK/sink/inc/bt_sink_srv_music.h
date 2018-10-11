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

#ifndef __BT_SINK_SRV_MUSIC_H__
#define __BT_SINK_SRV_MUSIC_H__

#include "stdint.h"
#include "bt_type.h"
#include "bt_sink_srv_conmgr.h"
#include "bt_sink_srv_mp3.h"
#include "bt_sink_srv_ami.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "hal_gpt.h"
#ifdef __BT_AWS_SUPPORT__
#include "bt_aws.h"
#endif
#include "bt_sink_srv_audio_sync.h"
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
#include "bt_timer_external.h"
#endif

#include "bt_sink_srv_resource.h"

/**< Deveice flag */
/* Stable flag */
#define BT_SINK_SRV_MUSIC_FLAG_A2DP_PLAY                    (1 << 0)
#define BT_SINK_SRV_MUSIC_FLAG_A2DP_PLAYING                 (1 << 1)
#define BT_SINK_SRV_MUSIC_FLAG_A2DP_CON_IND                 (1 << 2)
#define BT_SINK_SRV_MUSIC_FLAG_A2DP_RECOVER                 (1 << 3)
#define BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT               (1 << 4)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_A2DP_PLAY                (1 << 5)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_A2DP_PLAYING             (1 << 6)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_AUTO_SYNC                (1 << 7)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_A2DP_END                 (1 << 8)

/* MP3 flag */
#define BT_SINK_SRV_MUSIC_FLAG_MP3_START                          BT_SINK_SRV_MUSIC_FLAG_AWS_A2DP_END
     //1<<8
#define BT_SINK_SRV_MUSIC_FLAG_MP3_PLAY                           ((BT_SINK_SRV_MUSIC_FLAG_MP3_START) << 0)
#define BT_SINK_SRV_MUSIC_FLAG_MP3_PLAYING                        ((BT_SINK_SRV_MUSIC_FLAG_MP3_START) << 1)
#define BT_SINK_SRV_MUSIC_FLAG_MP3_FILE_OPEN                      ((BT_SINK_SRV_MUSIC_FLAG_MP3_START) << 2)
#define BT_SINK_SRV_MUSIC_FLAG_MP3_STOPPING                       ((BT_SINK_SRV_MUSIC_FLAG_MP3_START) << 3)  // indicate in stopping music
#define BT_SINK_SRV_MUSIC_FLAG_MP3_SOURCE_AVAILABLE               ((BT_SINK_SRV_MUSIC_FLAG_MP3_START) << 4)  // indicate it's audio source device
#define BT_SINK_SRV_MUSIC_FLAG_MP3_NEXT                           ((BT_SINK_SRV_MUSIC_FLAG_MP3_START) << 5)  // indicate it's changing next track
#define BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT                      ((BT_SINK_SRV_MUSIC_FLAG_MP3_START) << 6)  // indicate mp3 is interrupted
#define BT_SINK_SRV_MUSIC_FLAG_MP3_LOW_POWER_TEST                 ((BT_SINK_SRV_MUSIC_FLAG_MP3_START) << 7)  // for low power test
#define BT_SINK_SRV_MUSIC_FLAG_MP3_END                            ((BT_SINK_SRV_MUSIC_FLAG_MP3_START) << 8)
#define BT_SINK_SRV_MUSIC_FLAG_MP3_ALL                \
                ((BT_SINK_SRV_MUSIC_FLAG_MP3_PLAY) |    \
                 (BT_SINK_SRV_MUSIC_FLAG_MP3_PLAYING) | \
                 (BT_SINK_SRV_MUSIC_FLAG_MP3_FILE_OPEN) | \
                 (BT_SINK_SRV_MUSIC_FLAG_MP3_STOPPING) | \
                 (BT_SINK_SRV_MUSIC_FLAG_MP3_NEXT))




/* Wait flag */
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_START                   BT_SINK_SRV_MUSIC_FLAG_MP3_END
 // 1<<16
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC          ((BT_SINK_SRV_MUSIC_FLAG_WAIT_START) << 0)
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_PLAY                ((BT_SINK_SRV_MUSIC_FLAG_WAIT_START) << 1)
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP                ((BT_SINK_SRV_MUSIC_FLAG_WAIT_START) << 2)
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START              ((BT_SINK_SRV_MUSIC_FLAG_WAIT_START) << 3)
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_SUSPEND            ((BT_SINK_SRV_MUSIC_FLAG_WAIT_START) << 4)
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_AWS_ACTIONS             ((BT_SINK_SRV_MUSIC_FLAG_WAIT_START) << 5)    /**< Wait for aws source actions */
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK        ((BT_SINK_SRV_MUSIC_FLAG_WAIT_START) << 6)    /**< indicate change track */
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_AWS_CHANGE_MODE         ((BT_SINK_SRV_MUSIC_FLAG_WAIT_START) << 7)    /**< indicate change mode */
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_PSEDEV_PLAYING          ((BT_SINK_SRV_MUSIC_FLAG_WAIT_START) << 8)
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET           ((BT_SINK_SRV_MUSIC_FLAG_WAIT_START) << 9)
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER          ((BT_SINK_SRV_MUSIC_FLAG_WAIT_START) << 10)
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_AWS_STATE_SYNC      ((BT_SINK_SRV_MUSIC_FLAG_WAIT_START) << 11)
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_GPT_TIMEOUT             ((BT_SINK_SRV_MUSIC_FLAG_WAIT_START) << 12)
#define BT_SINK_SRV_MUSIC_FLAG_WAIT_ALL                     ((BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC) | \
                                                              (BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_PLAY)     | \
                                                              (BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_STOP)     | \
                                                              (BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START)   | \
                                                              (BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_SUSPEND) | \
                                                              (BT_SINK_SRV_MUSIC_FLAG_WAIT_AWS_ACTIONS)  | \
                                                              (BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK))

/**< Operation flag */
#define BT_SINK_SRV_MUSIC_OP_CODEC_OPEN                     (1 << 0)    /**< Codec open */
#define BT_SINK_SRV_MUSIC_OP_AWS_START                      (1 << 1)    /**< AWS operation */
#define BT_SINK_SRV_MUSIC_OP_DRV_PLAY                       (1 << 2)    /**< DRV play done */

/* Next action operation flag */
#define BT_SINK_SRV_MUSIC_OP_AUTO_SYNC                      (1 << 16)   /**< Pending auto sync */
#ifdef __BT_SINK_SRV_VOICE_PROMPT__
#define BT_SINK_SRV_MUSIC_WAIT_VAP_RESUME                   (1 << 17)   /**< Pending VPA handle */
#endif
#define BT_SINK_SRV_MUSIC_AVRCP_PLAY_TRIGGER                (1 << 18)   /**< AVRCP trigger, no need RSP */


#define BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT                 (1 << 25)   /**< HF interrupt A2DP, but no avrcp pause status change */


#define BT_SINK_SRV_MUSIC_PREPARE_BUF_THRESHOLD             (120 * 1000) /**< 120ms cache */

#define BT_SINK_SRV_MUSIC_AWS_PREPARE_BUF_THRESHOLD         (30 * 1000) /**< Start play 1st packet + 70ms gpt time */

#define BT_SINK_SRV_MUSIC_MAX_LOG_COUNT              (10)
#define BT_SINK_SRV_MUSIC_A2DP_CONN_BIT              (1 << 0)
#define BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT             (1 << 1)
#define BT_SINK_SRV_MUSIC_AWS_CONN_BIT               (1 << 2)

#define BT_SINK_SRV_MUSIC_INVALID_HD                 (0x00)

#define BT_SINK_SRV_INVALID_AID                 (-1)

#define BT_SINK_SRV_MEDIA_PKT_HEADER_LEN        (12)
#define BT_SINK_SRV_MEDIA_SBC_SYNC_WORD         (0x3453)
#define BT_SINK_SRV_MEDIA_SBC_SYNC_WORD_LEN     (4)

#define BT_SINK_SRV_AAC_ADTS_LENGTH             (7)


#define BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH            (7)
#define BT_SINK_SRV_MUSIC_ERR_SUCCESS_6TH            (6)
#define BT_SINK_SRV_MUSIC_ERR_SUCCESS_5TH            (5)
#define BT_SINK_SRV_MUSIC_ERR_SUCCESS_4TH            (4)
#define BT_SINK_SRV_MUSIC_ERR_SUCCESS_3RD            (3)
#define BT_SINK_SRV_MUSIC_ERR_SUCCESS_2ND            (2)
#define BT_SINK_SRV_MUSIC_ERR_SUCCESS_1ST            (1)
#define BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK             (0)
#define BT_SINK_SRV_MUSIC_ERR_FAIL_1ST               (-1)
#define BT_SINK_SRV_MUSIC_ERR_FAIL_2ND               (-2)
#define BT_SINK_SRV_MUSIC_ERR_FAIL_3RD               (-3)
#define BT_SINK_SRV_MUSIC_ERR_FAIL_4TH               (-4)
#define BT_SINK_SRV_MUSIC_ERR_FAIL_5TH               (-5)
#define BT_SINK_SRV_MUSIC_ERR_FAIL_6TH               (-6)
#define BT_SINK_SRV_MUSIC_ERR_FAIL_7TH               (-7)

#ifndef MAXIMUM
#define     MAXIMUM(A, B)                  (((A)>(B))?(A):(B))
#define     MINIMUM(A, B)                  (((A)<(B))?(A):(B))
#endif

/**
 * @brief This structure defines the AWS role.
 */

#define BT_SINK_SRV_SET_FLAG(FLAG, MASK) do { \
    (FLAG) |= (MASK); \
} while(0);

#define BT_SINK_SRV_REMOVE_FLAG(FLAG, MASK) do { \
    (FLAG) &= ~(MASK); \
} while(0);


#define BT_SINK_SRV_MUISC_DEV_COUNT     (AUDIO_SRC_SRV_PSEUDO_DEVICE_NUM)     /**< Max device count */

#define BT_SINK_SRV_A2DP_DATA_RCE_LOOP_COUNT                       (300)

#define BT_SINK_SRV_A2DP_DATA_ERROR_LOOP_COUNT                     (100)

#define BT_SINK_SRV_A2DP_AWS_TIMER_DUR                             (120 * 1000)  /* ums*/

#define BT_SINK_SRV_MUSIC_DUR_MIN                                  (70 * 1000)
#define BT_SINK_SRV_MUSIC_DUR_MAX                                  (500 * 1000)
#define BT_SINK_SRV_MUSIC_MP3_DUR_MIN                              (10 * 1000)
#define BT_SINK_SRV_MUSIC_MP3_DUR_MAX                              (3000 * 1000)

#define BT_SINK_SRV_MUSIC_NEXT_DUR_MIN                             (10 * 1000)
#define BT_SINK_SRV_MUSIC_NEXT_DUR_MAX                             (1000 * 1000)

#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
#define BT_SINK_SRV_A2DP_WAIT_AWS_CNF_TIMER_DUR     (1000)
#define BT_SINK_SRV_A2DP_WAIT_AWS_CNF_TIMER_ID      (BT_TIMER_EXT_GROUP_SINK(1))

#ifdef __BT_SINK_SRV_VOICE_PROMPT__
#define BT_SINK_SRV_A2DP_WAIT_VPA_TIMER_DUR         (1500)
#define BT_SINK_SRV_A2DP_WAIT_VPA_TIMER_ID          (BT_TIMER_EXT_GROUP_SINK(2))
#endif

#ifdef __BT_SINK_SRV_MUSIC_MODE_SUPPORT__
#define BT_SINK_SRV_MUSIC_MODE_TIMER_DUR            (10 * 1000)
#define BT_SINK_SRV_MUSIC_MODE_TIMER_ID             (BT_TIMER_EXT_GROUP_SINK(3))
#endif
#endif

typedef enum {
    BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD,                       /* 0 */
    BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD,                      /* 1 */
    BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP,                     /* 2 */
    BT_SINK_SRV_MUSIC_DEVICE_ADDR_AVRCP,                    /* 3 */
    BT_SINK_SRV_MUSIC_DEVICE_ADDR_AWS_MP3,                  /* 4 */
    BT_SINK_SRV_MUSIC_DEVICE_UNUSED,                        /* 5 */
    BT_SINK_SRV_MUSIC_DEVICE_USED,                          /* 6 */
    BT_SINK_SRV_MUSIC_DEVICE_LATEST,                        /* 7 */
    BT_SINK_SRV_MUSIC_DEVICE_INTERRUPT,                     /* 8 */
    BT_SINK_SRV_MUSIC_DEVICE_FOCUS,                         /* 9 */
    BT_SINK_SRV_MUSIC_DEVICE_AWS_A2DP_HD,                   /**< 10 AWS handle is A2DP pseudo devcie */
    BT_SINK_SRV_MUSIC_DEVICE_AWS_MP3_HD,                    /**< 11 */
    BT_SINK_SRV_MUSIC_DEVICE_AWS,                           /**< 12 same check pseudo type */
    BT_SINK_SRV_MUSIC_DEVICE_PSE_HD,                        /**< 13 Pseudo device handle */
    BT_SINK_SRV_MUSIC_DEVICE_SP,                            /**< 14 */
    BT_SINK_SRV_MUSIC_DEVICE_LOCAL,                         /**< 15 */
} bt_sink_srv_music_device_type_t;

typedef audio_src_srv_state_t bt_sink_srv_music_state_t;

typedef enum {
    BT_SINK_SRV_MUSIC_TRANSIENT_STATE_NONE = 0,
    BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_CONN,
    BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_DISCONN,
    BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_ROLE_SET,    /**< waiting role set */
    BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_START,       /**< waiting role set */
    BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC,
    BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_BUFFER,
    BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_INIT_SYNC,       /**< waiting init sync */
    BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC,
    BT_SINK_SRV_MUSIC_TRANSIENT_STATE_WAIT_AWS_STOP,      /**< waiting stop cnf */
} bt_sink_srv_music_transient_state_t;


#define BT_SINK_SRV_MUSIC_EVT_BASE              (0x800000)


typedef uint32_t bt_sink_srv_music_event_t;

#define BT_SINK_SRV_MUSIC_EVT_START             (BT_SINK_SRV_MUSIC_EVT_BASE)
#define BT_SINK_SRV_MUSIC_EVT_UNAVAILABLE       (BT_SINK_SRV_MUSIC_EVT_START + 1)
#define BT_SINK_SRV_MUSIC_EVT_READY             (BT_SINK_SRV_MUSIC_EVT_START + 2)
#define BT_SINK_SRV_MUSIC_EVT_PLAYING           (BT_SINK_SRV_MUSIC_EVT_START + 3)
#define BT_SINK_SRV_MUSIC_EVT_REJECT            (BT_SINK_SRV_MUSIC_EVT_START + 4)
#define BT_SINK_SRV_MUSIC_EVT_CODEC_OPEN        (BT_SINK_SRV_MUSIC_EVT_START + 5)   
#define BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL      (BT_SINK_SRV_MUSIC_EVT_START + 6)
#define BT_SINK_SRV_MUSIC_EVT_CODEC_CLEAR       (BT_SINK_SRV_MUSIC_EVT_START + 7)
#define BT_SINK_SRV_MUSIC_EVT_RECOVER           (BT_SINK_SRV_MUSIC_EVT_START + 8)
#define BT_SINK_SRV_MUSIC_EVT_RESUME            (BT_SINK_SRV_MUSIC_EVT_START + 9)
#define BT_SINK_SRV_MUSIC_EVT_START_IND         (BT_SINK_SRV_MUSIC_EVT_START + 10)
#define BT_SINK_SRV_MUSIC_EVT_START_CNF         (BT_SINK_SRV_MUSIC_EVT_START + 11)
#define BT_SINK_SRV_MUSIC_EVT_SUSPEND_IND       (BT_SINK_SRV_MUSIC_EVT_START + 12)
#define BT_SINK_SRV_MUSIC_EVT_SUSPEND_CNF       (BT_SINK_SRV_MUSIC_EVT_START + 13)

#define BT_SINK_SRV_MUSIC_EVT_AVRCP_PLAYING     (BT_SINK_SRV_MUSIC_EVT_START + 20)
#define BT_SINK_SRV_MUSIC_EVT_AVRCP_PAUSED      (BT_SINK_SRV_MUSIC_EVT_START + 21)


/**
 *  @brief This structure defines the BT_AWS_MODE_SET_CNF result.
 */
typedef struct {
    uint32_t handle;           /**< AWS handle. */
    bt_status_t status;        /**< set mode status. */
} bt_sink_srv_music_aws_mode_set_cnf_t;

/**
 *  @brief This structure defines the #BT_AWS_START_STREAMING_IND result.
 */
typedef struct {
    uint32_t handle;                             /**< AWS handle. */
    void *codec_cap;        /**< Streaming channel codec capability. */
} bt_sink_srv_music_aws_start_streaming_ind_t;

/**
 *  @brief This structure defines the #BT_AWS_START_STREAMING_CNF result.
 */
typedef struct {
    uint32_t handle;                            /**< AWS handle. */
    uint8_t accept;                             /**< Start streaming status. */
} bt_sink_srv_music_aws_start_streaming_cnf_t;

/**
 *  @brief This structure defines the #BT_AWS_SUSPEND_STREAMING_IND result.
 */
typedef struct {
    uint32_t handle;                           /**< AWS handle. */
} bt_sink_srv_music_aws_suspend_streaming_ind_t;

/**
 *  @brief This structure defines the #BT_AWS_SUSPEND_STREAMING_CNF result.
 */
typedef struct {
    uint32_t handle;                            /**< AWS handle. */
    uint8_t accept;                         /**< Suspend streaming status. */
} bt_sink_srv_music_aws_suspend_streaming_cnf_t;


#ifdef __BT_AWS_SUPPORT__
typedef struct {
    uint32_t sn;
    uint32_t ts;
} bt_sink_srv_music_pkt_info;

typedef struct {
    bt_aws_clock_t clock;
    bt_sink_srv_music_pkt_info pkt_info;
    uint32_t ts_diff_by_packet;
} bt_sink_srv_music_aws_sync_info; 
#endif

typedef void(* bt_sink_srv_music_callback_t)(uint32_t evt_id, void *param, void *user_data);

typedef struct {
    bt_sink_srv_music_state_t state;
    bt_sink_srv_music_state_t target_state;
    bt_sink_srv_music_transient_state_t substate;
    bt_bd_addr_t dev_addr;
    uint16_t conn_bit;
    uint32_t flag;                                          /**< Device flag */
    uint32_t op;                                            /**< Operation flag */
    audio_src_srv_handle_t *handle;                         /**< Pseudo device handle */

    bt_sink_srv_am_media_handle_t med_handle;
    bt_a2dp_codec_capability_t codec;
    /* A2DP members */
    uint32_t a2dp_hd;
    bt_a2dp_role_t role;
    /* AVRCP members */
    uint32_t avrcp_hd;
    uint8_t avrcp_status;
    /* AWS members */
    #ifdef __BT_AWS_SUPPORT__
    uint32_t aws_hd;
    bt_aws_role_t aws_role;
    bt_role_t gap_role;
    bt_aws_clock_offset_t offset;
    bt_sink_srv_music_aws_sync_info local_info;
    bt_aws_clock_t play_time;
    #endif
    /* MP3 members */
    bt_sink_srv_mp3_context_t *mp3;
} bt_sink_srv_music_device_t;

typedef struct {
    bt_sink_srv_music_state_t state;
    bt_sink_srv_music_device_t *run_dev;

    /* Audio manager ID */
    int8_t a2dp_aid;
    int8_t aws_aid;
    /* Media info */
    uint32_t simple_count;
    uint32_t threshold_count;
    sink_srv_media_play_info_t med_play_info;
    sink_media_packet_info_t med_pkt_info;
    uint8_t vol_lev;

#ifdef __BT_AWS_SUPPORT__
    bt_sink_srv_music_aws_sync_info local_info;
    bt_sink_srv_music_aws_sync_info piconet_info;
    bt_aws_clock_offset_t clk_offset;
    bt_aws_clock_t play_time;
#endif

    /* Get bt clock offset */
    bt_sink_srv_music_callback_t callback;
    void *user_data;

    bool trans;                                             /**< Enable transfer AWS data */
    bool first_trans;
    bt_sink_srv_music_device_t sink_dev[BT_SINK_SRV_MUISC_DEV_COUNT];
    bt_sink_srv_music_device_t *latest_dev;
    bt_sink_srv_music_device_t *focus_dev;
    hal_gpt_port_t gpt_port;
} bt_sink_srv_music_context_t;


#ifdef __cplusplus
extern "C" {
#endif

void bt_sink_srv_music_init(void);

bt_sink_srv_music_context_t *bt_sink_srv_music_get_context(void);

bt_sink_srv_music_device_t *bt_sink_srv_music_get_device(bt_sink_srv_music_device_type_t type, const void *param);

int32_t bt_sink_srv_music_start_gpt_timer(hal_gpt_callback_t callback, void *user_data, uint32_t duration);

void bt_sink_srv_music_reset_device(bt_sink_srv_music_device_t *dev);

void bt_sink_srv_music_set_focus_device(bt_sink_srv_music_device_t *dev);

void bt_sink_srv_music_update_run_device(bt_sink_srv_music_device_t *dev);

void bt_sink_srv_music_state_machine_handle(bt_sink_srv_music_device_t *dev, uint32_t evt_id, void *param);

void BT_A2DP_MAKE_SBC_CODEC(bt_a2dp_codec_capability_t *codec,
                            bt_a2dp_role_t role,
                            uint8_t min_bit_pool, uint8_t max_bit_pool,
                            uint8_t block_length, uint8_t subband_num,
                            uint8_t alloc_method, uint8_t sample_rate,
                            uint8_t channel_mode);

void BT_A2DP_MAKE_AAC_CODEC(bt_a2dp_codec_capability_t *codec,
                            bt_a2dp_role_t role, bool vbr, uint8_t object_type,
                            uint8_t channels, uint16_t sample_rate,
                            uint32_t bit_rate);

void BT_A2DP_CONVERT_SBC_CODEC(bt_codec_capability_t *dst_codec,
                               bt_a2dp_codec_capability_t *src_codec);

void BT_A2DP_CONVERT_AAC_CODEC(bt_codec_capability_t *dst_codec,
                               bt_a2dp_codec_capability_t *src_codec);

void bt_sink_srv_music_fill_audio_src_callback(audio_src_srv_handle_t *handle);

void bt_sink_srv_music_disable_log(void);

void bt_sink_srv_music_enable_log(void);

int32_t bt_sink_srv_music_get_bt_clk_offset(uint32_t handle, bt_sink_srv_music_callback_t callback, void *user_data);

void bt_sink_srv_music_disable_transfer_packet(void);

void bt_sink_srv_music_enable_transfer_packet(void);

void *bt_sink_srv_music_find_suitable_packet(bt_sink_srv_music_device_t *dev, uint32_t *time_dur, uint8_t type);

void bt_sink_srv_music_update_aws_sync_info(bt_sink_srv_music_device_t *dev, bt_hci_packet_t *packet, uint8_t type);

bool bt_sink_srv_music_duration_valid(bt_aws_module_id_t module_id, uint32_t duration);

uint64_t bt_sink_srv_music_convert_btaddr_to_devid(bt_bd_addr_t *bd_addr);

#ifdef __BT_SINK_SRV_MUSIC_MODE_SUPPORT__
/**
 * @brief                   This function is employed to set music mode.
 * @param[in] mode          Specified music mode
 * @return
 */
int32_t bt_sink_srv_set_music_mode(bt_sink_srv_music_mode_t mode);


/**
 * @brief                   This function is employed to get music mode.
 * @param[in] void
 * @return                  music mode (BT or Local)
 */
bt_sink_srv_music_mode_t bt_sink_srv_get_music_mode(void);
#endif

#ifdef __cplusplus
}
#endif

#endif

