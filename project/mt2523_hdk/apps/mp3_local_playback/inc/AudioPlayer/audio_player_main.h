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

#ifndef AUDIO_PLAYER_MAIN_H
#define AUDIO_PLAYER_MAIN_H

#include "audio_player.h"

#ifdef __LOCAL_PLAYER_ENABLE__

#include <stdbool.h>
#include <stdint.h>
#include "ff.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_ami.h"
#include "task_def.h"

/* debug trace error code */
#define AUDIO_PLAYER_ERR_SUCCESS_7TH            (7)
#define AUDIO_PLAYER_ERR_SUCCESS_6TH            (6)
#define AUDIO_PLAYER_ERR_SUCCESS_5TH            (5)
#define AUDIO_PLAYER_ERR_SUCCESS_4TH            (4)
#define AUDIO_PLAYER_ERR_SUCCESS_3RD            (3)
#define AUDIO_PLAYER_ERR_SUCCESS_2ND            (2)
#define AUDIO_PLAYER_ERR_SUCCESS_1ST            (1)
#define AUDIO_PLAYER_ERR_SUCCESS_OK             (0)
#define AUDIO_PLAYER_ERR_FAIL_1ST               (-1)
#define AUDIO_PLAYER_ERR_FAIL_2ND               (-2)
#define AUDIO_PLAYER_ERR_FAIL_3RD               (-3)
#define AUDIO_PLAYER_ERR_FAIL_4TH               (-4)
#define AUDIO_PLAYER_ERR_FAIL_5TH               (-5)
#define AUDIO_PLAYER_ERR_FAIL_6TH               (-6)
#define AUDIO_PLAYER_ERR_FAIL_7TH               (-7)


#define AUDIO_PLAYER_FILE_PATH      ("0:/music")

#if _USE_LFN
#define AUDIO_PLAYER_PATH_LENGTH    (255)

#define AUDIO_PLAYER_FILE_LENGTH    (63)
#else
#define AUDIO_PLAYER_PATH_LENGTH    (128)

#define AUDIO_PLAYER_FILE_LENGTH    (15)
#endif

#define AUDIO_PLAYER_FILE_NUMBER    (32)
#define AUDIO_PLAYER_ID3v2_HEADER   (10)

#define MINIMUM(a, b)            ((a) < (b) ? (a) : (b))

#define AUDIO_PLAYER_FLAG_START_PLAY        (1 << 0)
#define AUDIO_PLAYER_FLAG_NEED_RESUME       (1 << 1)
#define AUDIO_PLAYER_FLAG_FILE_OPEN         (1 << 2)

#define AUDIO_PLAYER_FLAG_SET(CTX, FLAG) (CTX->flag |= (FLAG))
#define AUDIO_PLAYER_FLAG_RESET(CTX, FLAG) (CTX->flag &= ~(FLAG))

#define AUDIO_PLAYER_TASK_NAME              "audply_task"
#define AUDIO_PLAYER_TASK_STACKSIZE         (1024 * 4) /* unit byte */
#define AUDIO_PLAYER_TASK_PRIORITY          TASK_PRIORITY_NORMAL

#define AUDIO_PLAYER_RING_BUFFER_SIZE       (20 * 1024) /* audio ring buffer size */
#define AUDIO_PLAYER_FILE_READ_MAX_SIZE     (4 * 1024) /* max read file buffer size */
#define AUDIO_PLAYER_FILE_READ_COUNT        (2) /* read file count each task schedule */
#define AUDIO_PLAYER_BUFFER_THRESHOLD       (8 * 1024) /* start playing threshold */

/* volume control */
#define AUDIO_PLAYER_ACTION_VOLUME_UP            (0)
#define AUDIO_PLAYER_ACTION_VOLUME_DOWN          (1)
#define AUDIO_PLAYER_ACTION_VOLUME_MIN           (2)
#define AUDIO_PLAYER_ACTION_VOLUME_MAX           (3)
/* music control */
#define AUDIO_PLAYER_ACTION_PLAY                 (4)
#define AUDIO_PLAYER_ACTION_PAUSE                (5)
#define AUDIO_PLAYER_ACTION_NEXT_TRACK           (6)
#define AUDIO_PLAYER_ACTION_PRE_TRACK            (7)



#define AUDIO_PLAYER_ACTION_ALL                  (0xffff)


typedef enum {
    VOLUME_UP     = 0,
    VOLUME_DOWN   = 1,

    VOLUME_TOTAL
} volume_change_type_t;

typedef enum {
    NEXT_TRACK      = 0,
    PREVIOUS_TRACK  = 1,
    NORMAL_TRACK = 2,

    TRACK_TOTAL
} track_change_type_t;

typedef struct {
    uint32_t flag;

    TCHAR file_name[AUDIO_PLAYER_FILE_NUMBER][AUDIO_PLAYER_FILE_LENGTH + 1];
    int16_t file_index;
    int16_t file_total;

    bt_sink_srv_am_id_t aud_id;
    uint8_t vol_lev;
    uint8_t dev;
    bt_sink_srv_am_files_media_handle_t med_hd;
    uint32_t offset;

    bt_sink_srv_state_t sink_state;
} audio_player_context_t;

/** @deprecated share buffer structure */
typedef struct {
    uint8_t *buffer_base;         /**< Pointer to the ring buffer. */
    int32_t buffer_size;          /**< Size of the ring buffer. */
    int32_t write;                /**< Index of the ring buffer to write the data. */
    int32_t read;                 /**< Index of the ring buffer to read the data. */
    int32_t write_ex;             /**< Index of the ring buffer to write the data. */
    int32_t read_ex;              /**< Index of the ring buffer to read the data. */
    bool ex_wr_enable;            /**< read_ex feature enable. */
    void *param;                  /**< Extended parameter or information. */
} audio_player_buffer_t;

void audio_player_init(void);

typedef struct{
    bool have_id3v2;
    uint32_t tag_size;    
} audply_player_id3v2_information_t;

#endif /* __LOCAL_PLAYER_ENABLE__ */
#endif /* AUDIO_PLAYER_MAIN_H */

