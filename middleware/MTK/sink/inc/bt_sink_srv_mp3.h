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

#ifndef BT_SINK_SRV_MP3_H
#define BT_SINK_SRV_MP3_H


#include <stdbool.h>
#include <stdint.h>
#include "ff.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_ami.h"
#include "task_def.h"
#include "audio_src_srv.h"


#define BT_SINK_SRV_MP3_FILE_PATH      _T("0:/music")
#define MAX_FILE_PATH_LEN              (256)

#if _USE_LFN
#define BT_SINK_SRV_MP3_PATH_LENGTH    (255)

#define BT_SINK_SRV_MP3_FILE_LENGTH    (127)
#else
#define BT_SINK_SRV_MP3_PATH_LENGTH    (128)

#define BT_SINK_SRV_MP3_FILE_LENGTH    (15)
#endif

#define AUIDO_SKIP_ID3_FILE_LENGTH  (3 * 1024)

#define BT_SINK_SRV_MP3_FILE_NUMBER    (300)
#define BT_SINK_SRV_MP3_ID3v2_HEADER   (10)


#define BT_SINK_SRV_MP3_TASK_NAME              "mp3_task"
#define BT_SINK_SRV_MP3_TASK_STACKSIZE         (1024 * 4) /* unit byte */
#define BT_SINK_SRV_MP3_TASK_PRIORITY          TASK_PRIORITY_NORMAL

#define BT_SINK_SRV_MP3_RING_BUFFER_SIZE       (20 * 1024) /* audio ring buffer size */
#define BT_SINK_SRV_MP3_FILE_READ_MAX_SIZE     (1 * 1024) /* max read file buffer size */
#define BT_SINK_SRV_MP3_FILE_READ_COUNT        (8) /* read file count each task schedule */
#define BT_SINK_SRV_MP3_BUFFER_THRESHOLD       (8 * 1024 - 1) /* start playing threshold */
#define BT_SINK_SRV_MP3_FRAME_NUM_THRESHOLD       (8) /* start playing accumulate frame number threshold */

#define BT_SINK_SRV_MP3_AWS_RESYNC_THRESHOLD        (10) /* suspend music if reached resync attempt */
#define BT_SINK_SRV_MP3_MP3_HEADER_MASK             (0x000CE7FF) /* Dont care padding bit. Byte reversed because of little endian */
#define BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN        (4)
#define BT_SINK_SRV_MP3_DEFAULT_SILENCE_NUM         (5) /* default number to fill silence frame one time */
#define BT_SINK_SRV_MP3_INVALID_OFFSET          0xFFFFFFFF

#define BT_SINK_SRV_MP3_DEFAULT_VOLUME  (AUD_VOL_OUT_LEVEL4)

#define BT_SINK_SRV_MP3_BREAKPOINT_SUPPORT      0
#define BT_SINK_SRV_MP3_BITRATE_MINIMUM         256000
#define BT_SINK_SRV_MP3_FREQ_L_BOUND            208000

typedef enum {
    BT_SINK_SRV_MP3_EVENT_GETFILENUM          = 0,
    BT_SINK_SRV_MP3_EVENT_GETFILEPATHBYIDX    = 1,
    BT_SINK_SRV_MP3_EVENT_OPEN                = 2,
    BT_SINK_SRV_MP3_EVENT_LSEEK               = 3,
    BT_SINK_SRV_MP3_EVENT_READ                = 4,
    BT_SINK_SRV_MP3_EVENT_TELL                = 5,
    BT_SINK_SRV_MP3_EVENT_EOF                 = 6,
    BT_SINK_SRV_MP3_EVENT_SIZE                = 7,
    BT_SINK_SRV_MP3_EVENT_SET_BREAKPOINT      = 8,
    BT_SINK_SRV_MP3_EVENT_GET_BREAKPOINT      = 9
} bt_sink_srv_mp3_file_event_type_t;


typedef enum {
    MP3_VOLUME_UP     = 0,
    MP3_VOLUME_DOWN   = 1,

    MP3_VOLUME_TOTAL
} bt_sink_srv_mp3_volume_change_type_t;

typedef enum {
    MP3_NEXT_TRACK      = 0,
    MP3_PREVIOUS_TRACK  = 1,
    MP3_NORMAL_TRACK = 2,

    MP3_TRACK_TOTAL
} bt_sink_srv_mp3_track_change_type_t;

typedef struct {
    //TCHAR file_name[BT_SINK_SRV_MP3_FILE_NUMBER][BT_SINK_SRV_MP3_FILE_LENGTH + 1];
    TCHAR curr_file_path[MAX_FILE_PATH_LEN];

    int16_t folder_index;
    int16_t file_index;
    int16_t file_total;
    uint32_t offset;
    uint32_t max_read_size;
    uint32_t threshold;

    audio_src_srv_handle_t *audio_src;

    bt_sink_srv_am_id_t aud_id;
    uint8_t vol_lev;
    uint8_t dev;
    bt_sink_srv_am_files_media_handle_t med_hd;
    am_file_type_t type;
    bool supportAws;
    uint8_t retry_cnt;  // for retry to play tag in middle file
} bt_sink_srv_mp3_context_t;


typedef struct {
    uint32_t frame_header;      /** mp3 header after masked */
    uint32_t first_frame_sn;
    uint32_t first_frame_ts;
    uint32_t last_frame_len;    /** last count frame length */
    uint32_t last_frame_sn;
    uint32_t remain_data_size;
    uint32_t acc_frame_ts;
    uint32_t frame_start_offset;
    uint32_t sample_count_per_frame;
    bool last_header_exist;
} bt_sink_srv_mp3_frame_info_t;

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
} bt_sink_srv_mp3_buffer_t;

typedef struct {
    uint8_t nfa[2];
    uint16_t frame_start_num;
    uint32_t timestamp;
    uint32_t frame_start_offset;
} bt_sink_srv_mp3_media_header;

typedef struct{
    bool have_id3v2;
    uint32_t tag_size;
} bt_sink_srv_mp3_id3v2_information_t;

typedef struct {
    TCHAR *path;
    uint32_t idx;
} bt_sink_srv_mp3_event_get_file_path_by_idx_t;

typedef struct {
    FIL *fp;
    TCHAR *path;
    void *buf;
    uint32_t bytetoread;
    UINT *byteread;
    uint32_t index;
    uint32_t pos;
    uint32_t *offset;
} bt_sink_srv_mp3_file_event_t;

typedef struct {
    int16_t folder_id;
    int16_t file_id;
    uint32_t offset;
} bt_sink_srv_mp3_breakpoint_data_t;

typedef struct {
    union {
        uint16_t *get_file_num;
        bt_sink_srv_mp3_event_get_file_path_by_idx_t get_file_path_by_idx;
        bt_sink_srv_mp3_file_event_t file_event;
        bt_sink_srv_mp3_breakpoint_data_t *bp_data;
    };
} bt_sink_srv_mp3_event_t;


/* APIs */
void bt_sink_srv_mp3_init(void);

void bt_sink_srv_mp3_deinit(void);

void bt_sink_srv_mp3_set_max_read_size(uint32_t size);

int32_t bt_sink_srv_mp3_play(void);

int32_t bt_sink_srv_mp3_play_process(void);

int32_t bt_sink_srv_mp3_pause(void);

void wav_pause_test(void);

void wav_resume_test(void);

int32_t bt_sink_srv_mp3_prev_track(void);

int32_t bt_sink_srv_mp3_next_track(void);

int32_t bt_sink_srv_mp3_stop(void);

int32_t bt_sink_srv_mp3_volume_up(void);

int32_t bt_sink_srv_mp3_volume_down(void);

void bt_sink_srv_mp3_change_frame_log(void);

void bt_sink_srv_mp3_processing(void);

bool bt_sink_srv_mp3_over_threshold();

bt_sink_srv_status_t bt_sink_srv_mp3_event_callback(bt_sink_srv_event_t event_id, void *parameters);

void bt_sink_srv_mp3_local_take_semaphore(void);

void bt_sink_srv_mp3_local_give_semaphore(void);

uint32_t bt_sink_srv_mp3_find_suitable_frame(void);

void bt_sink_srv_aws_mp3_gpt_cb(void *user_data);

void bt_sink_srv_mp3_read_nvdm();

void bt_sink_srv_mp3_write_nvdm();


#endif /* BT_SINK_SRV_MP3_H */

