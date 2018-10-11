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
#include "bt_sink_srv_ami.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "hal_gpt.h"
#ifdef __BT_AWS_SUPPORT__
#include "bt_aws.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BT_SINK_SRV_MUSIC_MAX_LOG_COUNT              (10)
#define BT_SINK_SRV_MUSIC_A2DP_CONN_BIT              (1 << 0)
#define BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT             (1 << 1)
#define BT_SINK_SRV_MUSIC_AWS_CONN_BIT               (1 << 2)


#define BT_SINK_SRV_MUSIC_FLAG_PLAYING                      (1 << 0)
#define BT_SINK_SRV_MUSIC_FLAG_A2DP_CON_IND                 (1 << 1)
#define BT_SINK_SRV_MUSIC_FLAG_INTERRUPT                    (1 << 2)
#define BT_SINK_SRV_MUSIC_FLAG_A2DP_STREAMING               (1 << 3)
#define BT_SINK_SRV_MUSIC_FLAG_AVRCP_PLAY                   (1 << 4)
#define BT_SINK_SRV_MUSIC_FLAG_AVRCP_PAUSE                  (1 << 5)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_CON_IND                  (1 << 6)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_STREAMING                (1 << 7)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_PLAY                     (1 << 8)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_PAUSE                    (1 << 9)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_START_STREAMING_CNF      (1 << 10)
#define BT_SINK_SRV_MUSIC_FLAG_AMI_OPEN_CODEC               (1 << 11)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_PLAYING                  (1 << 12)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_SRC_RESYNC               (1 << 13)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_SEND_SUSPEND             (1 << 14)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_SEND_START_STREAMING     (1 << 15)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_WAIT_ROLE_STREAMING      (1 << 16)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_WAIT_ROLE_STREAMING_MP3  (1 << 17)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_MODE_CHANGING            (1 << 18)
#define BT_SINK_SRV_MUSIC_FALG_A2DP_DISCARD_DATA            (1 << 20)


#define BT_SINK_SRV_MUSIC_FLAG_AAC                   (1 << 18)
#define BT_SINK_SRV_MUSIC_FLAG_AWS_AAC               (1 << 19)

#define BT_SINK_SRV_MUSIC_AVRCP_OP_PLAY              (1 << 0)
#define BT_SINK_SRV_MUSIC_AVRCP_OP_PAUSE             (1 << 1)

#define BT_SINK_SRV_AVRCP_OP_TIMER_DUR               (500)

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

// Add for avrcp pasue, a2dp pause.
#define SINK_MUSIC_STATE_PAUSE_NONE     (0)
#define SINK_MUSIC_STATE_PAUSE_AVRCP    (1)
#define SINK_MUSIC_STATE_PAUSE_A2DP     (2)

typedef enum {
    BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, /* 0 */
    BT_SINK_SRV_MUSIC_DEVICE_AVRCP_HD, /* 1 */
    BT_SINK_SRV_MUSIC_DEVICE_ADDR, /* 2 */
    BT_SINK_SRV_MUSIC_DEVICE_UNUSED, /* 3 */
    BT_SINK_SRV_MUSIC_DEVICE_USED, /* 4 */
    BT_SINK_SRV_MUSIC_DEVICE_LATEST,/* 5 */
    BT_SINK_SRV_MUSIC_DEVICE_INTERRUPT, /* 6 */
    BT_SINK_SRV_MUSIC_DEVICE_FOCUS, /* 7 */
    BT_SINK_SRV_MUSIC_DEVICE_AWS_HD, /* 8 */
    BT_SINK_SRV_MUSIC_DEVICE_AWS, /* 9 */

    BT_SINK_SRV_MUSIC_DEVICE_TOTAL
} bt_sink_srv_music_device_type_t;

typedef enum {
    BT_SINK_SRV_MUSIC_IDLE,
    BT_SINK_SRV_MUSIC_READY,
    BT_SINK_SRV_MUSIC_PLAY,

    /* AMI interaction */
    BT_SINK_SRV_MUSIC_AMI_PLAY,
    BT_SINK_SRV_MUSIC_AMI_STOP,

    /* AWS interaction */
    BT_SINK_SRV_MUSIC_AMI_AWS_PLAY,
    BT_SINK_SRV_MUSIC_AWS_PLAY,

    BT_SINK_SRV_MUSIC_TOTAL
} bt_sink_srv_music_state_t;

typedef struct {
    uint8_t version; /* RTP Version */

    uint8_t padding; /*
                 * If the padding bit is set, the packet contains
                 * one or more additional padding octets at the end,
                 * which are not parts of the payload.  The last
                 * octet of the padding contains a count of how many
                 * padding octets should be ignored.
                 */
    uint8_t extension;
    uint8_t marker;  /*
                 * Profile dependent.  Used to mark significant events
                 * such as frame boundaries in the packet stream.
                 */

    uint16_t payload_type; /* Profile dependent.  Identifies the RTP payload 
type. */

    uint16_t sequence_number; /* Incremented by one for each packet sent */

    uint32_t timestamp;  /* Time stamp of the sample */

    uint32_t ssrc;   /* Synchronization source */

    uint8_t csrc_count;       /*
                         * The number of CSRC (Contributing Source) identifiers
                         * that follow the fixed header.
                         */
    uint32_t csrc_list[15];   /* List of CSRC identifiers */

} bt_sink_srv_a2dp_media_header_t;

typedef struct {
    bt_sink_srv_profile_t profile;
    bt_a2dp_codec_capability_t codec;
    bt_sink_srv_am_media_handle_t med_hd;
    bt_sink_srv_a2dp_media_header_t header;
} SinkProfileA2dp;

typedef struct {
    bt_sink_srv_profile_t profile;
} SinkProfileAvrcp;

typedef struct {
    uint8_t version; /* RTP Version */

    uint8_t padding; /*
                 * If the padding bit is set, the packet contains
                 * one or more additional padding octets at the end,
                 * which are not parts of the payload.  The last
                 * octet of the padding contains a count of how many
                 * padding octets should be ignored.
                 */
    uint8_t extension;
    uint8_t marker;  /*
                 * Profile dependent.  Used to mark significant events
                 * such as frame boundaries in the packet stream.
                 */

    uint16_t payload_type; /* Profile dependent.  Identifies the RTP payload type. */

    uint16_t sequence_number; /* Incremented by one for each packet sent */

    uint32_t timestamp;  /* Time stamp of the sample */

    uint32_t ssrc;   /* Synchronization source */

    uint8_t csrc_count;       /*
                         * The number of CSRC (Contributing Source) identifiers
                         * that follow the fixed header.
                         */
    uint32_t csrc_list[15];   /* List of CSRC identifiers */

} bt_sink_srv_aws_media_header_t;

#ifdef __BT_AWS_SUPPORT__
typedef struct {
    bt_sink_srv_profile_t profile;
    bt_aws_codec_capability_t codec;
    bt_sink_srv_am_media_handle_t med_hd;
    bt_sink_srv_am_files_media_handle_t f_med_hd; /* for mp3 + aws */
    bt_sink_srv_aws_media_header_t header;
} SinkProfileAws;
#endif

typedef struct {
    SinkProfileAvrcp *avrcp;
    SinkProfileA2dp *a2dp;
    #ifdef __BT_AWS_SUPPORT__
    SinkProfileAws *aws;
    #endif
    bt_bd_addr_t dev_addr;
    uint32_t a2dp_hd;
    uint32_t avrcp_hd;
    uint32_t aws_hd;
    uint16_t conn_bit;
    uint32_t flag;
    bt_a2dp_role_t role;
    #ifdef __BT_AWS_SUPPORT__
    bt_aws_role_t aws_role;
    #endif
    bt_role_t gap_role;
    uint32_t mp3_aws_attempt;
} bt_sink_srv_music_device_t;

typedef struct {
    uint8_t partymode;
    uint8_t state;
    int8_t aud_id;
    int8_t aws_aid;
    uint32_t a2dp_hd;
    uint32_t aws_hd;
    uint16_t write_len;
    uint32_t flag;
    bt_sink_srv_music_device_t sink_dev[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
    uint32_t log_cnt;
    bt_sink_srv_music_device_t *latest_dev;
    bt_sink_srv_music_device_t *focus_dev;
    uint32_t avrcp_op;
    TimerHandle_t timer;
    uint32_t pkt_cnt;
    hal_gpt_port_t gpt_port;
    uint8_t vol_lev;
} bt_sink_srv_music_context_t;


void bt_sink_srv_music_init(void);

bt_sink_srv_music_context_t *bt_sink_srv_music_get_context(void);

bt_sink_srv_music_device_t *bt_sink_srv_music_get_device(bt_sink_srv_music_device_type_t type, const void *param);

void bt_sink_srv_music_reset_device(bt_sink_srv_music_device_t *dev);

void bt_sink_srv_music_set_focus_device(bt_sink_srv_music_device_t *dev);

#ifdef __cplusplus
}
#endif

#endif

