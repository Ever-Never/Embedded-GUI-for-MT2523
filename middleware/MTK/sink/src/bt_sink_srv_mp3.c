/* Copyright Statement:
 *
 * (C) 2005-2017  MediaTek Inc. All rights reserved.
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

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include "syslog.h"
#include "ff.h"
#include "bt_sink_srv_ami.h"
#include "bt_sink_srv.h"
#include "bt_sink_app_event.h"
#include "hal_audio.h"
#include "mp3_codec.h"
#include "portable.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_avrcp.h"
#include "bt_sink_srv_aws.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_mp3.h"
#include "bt_sink_srv_aws_mp3.h"
#include "bt_sink_srv_mp3_state_machine.h"
#include "bt_sink_srv_audio_sync.h"
#include "bt_sink_srv_state_notify.h"
#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs.h"
#endif
#include "hal_feature_config.h"
#include "nvdm.h"

extern int32_t bt_sink_srv_mp3_file_callback(bt_sink_srv_mp3_file_event_type_t msg,  bt_sink_srv_mp3_event_t event);


int32_t default_bt_sink_srv_mp3_file_callback(bt_sink_srv_mp3_file_event_type_t msg,  bt_sink_srv_mp3_event_t event)
{
    bt_sink_srv_report("[MP3] %s", __func__);
    return 0;
}


#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_sink_srv_mp3_file_callback=_default_bt_sink_srv_mp3_file_callback")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_sink_srv_mp3_file_callback = default_bt_sink_srv_mp3_file_callback
#else
#error "Unsupported Platform"
#endif


extern uint32_t sink_loc_play_nclk;
extern uint32_t sink_loc_play_nclk_intra;

static bt_sink_srv_mp3_context_t g_aud_cntx;
static bt_sink_srv_am_files_media_handle_t *g_file_med_hd = NULL;
//static FATFS g_aud_fatfs;
static FIL g_aud_fdst;

static uint8_t g_mp3_frame_log_enable = 0;
static SemaphoreHandle_t g_aud_event_semaphore = NULL;
static SemaphoreHandle_t g_file_hd_semaphore = NULL;
static uint8_t g_silence_frame[1200];
static uint32_t g_silence_frame_size = 0;
static bool mp3_flag = false;

static uint8_t g_aud_ring_buff[BT_SINK_SRV_MP3_RING_BUFFER_SIZE];

/* frame information manager */
static bt_sink_srv_mp3_frame_info_t g_finfo;

/* ring buffer manager */
static bt_sink_srv_mp3_buffer_t g_aud_buff_mgr;
/* static function declare */
static bt_sink_srv_mp3_context_t *bt_sink_srv_mp3_get_context(void);

static void bt_sink_srv_mp3_scan_file(void);

//static FRESULT scan_files_recursion(TCHAR *path);

static int32_t bt_sink_srv_mp3_open_file(FIL *fp, TCHAR *path);

static FRESULT bt_sink_srv_mp3_read_file(FIL *fp, void *buf, uint32_t bytetoread, UINT *byteread);

static uint32_t bt_sink_srv_mp3_tell_file(FIL *fp);

static bool bt_sink_srv_mp3_eof_file(FIL *fp);

static uint32_t bt_sink_srv_mp3_size_file(FIL *fp);

static FRESULT bt_sink_srv_mp3_lseek_file(FIL *fp, DWORD offset);

static int32_t bt_sink_srv_mp3_get_breakpoint(bt_sink_srv_mp3_breakpoint_data_t *bp_data);

static int32_t bt_sink_srv_mp3_set_breakpoint(bt_sink_srv_mp3_breakpoint_data_t *bp_data);

/* media handler */
static void bt_sink_srv_mp3_ami_hdr(bt_sink_srv_am_id_t aud_id,
    bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg,
    void *parm);

static void bt_sink_srv_mp3_local_play(audio_src_srv_handle_t *handle);

static void bt_sink_srv_mp3_local_stop(audio_src_srv_handle_t *handle);

static void bt_sink_srv_mp3_suspend(audio_src_srv_handle_t *handle1, audio_src_srv_handle_t *handle2);

static void bt_sink_srv_mp3_reject(audio_src_srv_handle_t *handle);

static void bt_sink_srv_mp3_exception(audio_src_srv_handle_t *handle, int32_t event, void *param);

static int32_t bt_sink_srv_mp3_local_change_track(uint8_t type);

static int32_t bt_sink_srv_mp3_local_change_volume(uint8_t type);

static int32_t bt_sink_srv_mp3_read_data(void);

static void bt_sink_srv_mp3_write_drv_data(void);
static void bt_sink_srv_wav_write_drv_data(void);

static uint8_t bt_sink_srv_mp3_is_stop_and_empty();

static void bt_sink_srv_mp3_check_threshold(void);

static void bt_sink_srv_mp3_src_handle_data_req_ind(bt_sink_srv_am_cb_sub_msg_t sub_msg, void *parm);

#ifdef __BT_AWS_SUPPORT__
static void bt_sink_srv_aws_mp3_sink_handle_data_req_ind(void);
#endif
/* buffer manager */
static bt_sink_srv_mp3_buffer_t *buff_mgr_get_context(void);

static void buff_mgr_init(uint8_t *buffer, uint32_t length, bool ex_wr_enable);

static void buff_mgr_reset_share_buffer(void);

static void buff_mgr_get_write_buffer(uint8_t **buffer, uint32_t *length);

static void buff_mgr_get_read_buffer(uint8_t **buffer, uint32_t *length);

static void buff_mgr_get_read_data(uint8_t *buffer, uint32_t offset,uint32_t length);

static void buff_mgr_write_data_done(uint32_t length);

static void buff_mgr_read_data_done(uint32_t length);

#ifdef __BT_AWS_SUPPORT__
static void buff_mgr_rollback_write_buffer(uint32_t length);
#endif

static int32_t buff_mgr_get_data_count(void);

/* frame manager */
static bt_sink_srv_mp3_frame_info_t *bt_sink_srv_mp3_local_file_info_get_context(void);

static int32_t bt_sink_srv_mp3_update_frame_info(uint8_t **data, int32_t *data_len);

static int32_t bt_sink_srv_mp3_write_aws_header(bt_sink_srv_mp3_media_header *header, uint8_t *data);

#ifdef __BT_AWS_SUPPORT__
static bool bt_sink_srv_mp3_fill_first_frame_info(bt_sink_srv_mp3_media_header *hdr);

static void bt_sink_srv_mp3_fill_silence_frame(uint8_t num);
#endif

static uint32_t bt_sink_srv_mp3_get_length_by_frame(uint32_t len);

static uint32_t bt_sink_srv_mp3_search_mp3_header(uint8_t *buf, uint32_t buf_len, uint32_t *frame_start);

static bool bt_sink_srv_mp3_get_mp3_frame_offset(bt_sink_srv_mp3_id3v2_information_t *info, FIL *fp, uint32_t total_size);


/*************************/


//#define DEBUG_USE_PRINT
//#define DEBUG_ENABLE

#ifdef DEBUG_ENABLE
#define DEBUG_PRINT(fmt, arg...)    printf(fmt, ##arg)
#else
#define DEBUG_PRINT(fmt, arg...)
#endif

#ifndef DEBUG_USE_PRINT
#define LOGI(fmt, arg...)          bt_sink_srv_report(fmt, ##arg);
#define LOGW(fmt, arg...)          bt_sink_srv_report(fmt, ##arg);
#define LOGE(fmt, arg...)          bt_sink_srv_report(fmt, ##arg);
#else
#define LOGI(fmt, arg...)          DEBUG_PRINT(fmt, ##arg);
#define LOGW(fmt, arg...)          DEBUG_PRINT(fmt, ##arg);
#define LOGE(fmt, arg...)          DEBUG_PRINT(fmt, ##arg);
#endif

#if 0
static TCHAR *app_ucs2_wcscpy(TCHAR *str_dst, const TCHAR *str_src)
{
    TCHAR *str_tmp = str_dst;

    *str_tmp = *str_src;

    while(*str_src)
    {
        *(++str_tmp) = *(++str_src); /* Copy str_src over str_dst */
    }

    return str_dst;
}


static TCHAR *app_ucs2_wcsncpy(TCHAR *str_dst, const TCHAR *str_src, uint32_t count)
{

    TCHAR *str_tmp = str_dst;

    while (count && *str_src)
    {
        *str_tmp = *str_src; /* copy string */
        ++str_tmp, ++str_src;
        --count;
    }

    *str_tmp = L'\0';

    return str_dst;
}


static int32_t app_ucs2_wcsncmp(const TCHAR *str_src,
                           const TCHAR *str_dst,
                           uint32_t count)
{
    if (!count)
    {
        return 0;
    }

    while ((--count) && (*str_src) && (*str_src == *str_dst))
    {
        ++str_src, ++str_dst;
    }

    return (int32_t)(*str_src - *str_dst);
}


static int32_t app_ucs2_wcslen(const TCHAR *string)
{
    const TCHAR *str_tmp = string;

    while(*str_tmp)
    {
        ++str_tmp;
    }

    return (int32_t) (str_tmp - string);
}


static void reverse(WCHAR *start, int len)
{
    int32_t i, j;
    WCHAR t;

    for (i = 0, j = len-1; i < j; i++, j--)
    {
        t = start[i];
        start[i] = start[j];
        start[j] = t;
    }
}


static WCHAR* itow(WCHAR *out, uint32_t *max_len, int32_t ival, char zflag, char sflag, int width, int x, char u)
{
    int i, rem, len;
    unsigned int val, oldval;
    WCHAR *start;

    oldval = val = (unsigned int) ((sflag == 0) ? -ival : ival);

    // calculate the result length in wchar
    for (len =(sflag == 0)? 2 : 1  ; (val /= x) > 0 ; len++)
      ;

    if (len > *max_len || width > *max_len)
    {
        *max_len = 0;
        return out;
    }

    if ((sflag == 0) && (0 != zflag))
    {
        *out++ = (WCHAR)'-';
    }
    if (len < width) // insert "space" or 0's before real value
    {
        for (i=0; i <width-len ; i++)
        {
            *out++ = zflag ? '0' : ' ';
        }
    }

    val = oldval;
    start = out;
    do
    {
        rem = val % x;
        if (rem < 10)
        {
            *out = rem + '0';
        }
        else // hexadecimal
        {
            rem -= 10;
            if (u)
            {
                *out = rem + 'A';
            }
            else
            {
                *out = rem + 'a';
            }
        }
        out++;
    } while ((val /= x) > 0);

    if ((sflag == 0) && (0 == zflag))
    {
        *out++ = (WCHAR)'-';
    }

    reverse(start, (len-(((sflag == 0) && (0!=zflag)) ? 1 : 0)));
    *max_len -= len;

    return out;
}


static void app_ucs2_wsprintf_int(WCHAR *str_buff, uint32_t buf_size, const WCHAR *fmt, va_list args)
{
    int dval, width = 0;
    unsigned int udval;
    char zflag = 0, sflag = 1;
    char *sval, cval;
    WCHAR *out = str_buff, *wval;
    const WCHAR *p;

    buf_size /= sizeof(WCHAR);

    configASSERT(buf_size >= 1);

    /* Reserve string ending */
    buf_size--;

    //printf("ucs2_wsprintf_int enter\r\n");
    //printf("w char value is 0x%x\r\n", 'w');
    //printf("%% char value is 0x%x\r\n", '%');
    //printf("%% wide char value is 0x%x\r\n", L'%');
    for (p = fmt; *p && buf_size; p++, width = 0, zflag = 0, sflag = 1)
    {
        //printf("current char: 0x%.4x\r\n", *p);
        if (*p != '%')
        {
            *out++ = *p;
            buf_size--;
            continue;
        }

again:
        switch (*++p) {
        case 'c':
            cval= va_arg(args, int);
            *out++ = (WCHAR)cval;
            buf_size--;
            break;

        case 'd':
            dval= va_arg(args, int);
            sflag = ((dval<0) ? 0 : 1);
            out = itow(out, &buf_size, dval, zflag, sflag, width, 10, 0);
            break;

        case 'u':
            udval= va_arg(args, unsigned int);
            out = itow(out, &buf_size, udval, zflag, sflag, width, 10, 1);
            break;

        case 's':
            for (sval = va_arg(args, char *); *sval && buf_size; sval++ )
            {
                *out++ = (WCHAR)(*sval);
                buf_size--;
            }
            break;

        case 'w':
            //printf("wide enter");
            for (wval = va_arg(args, WCHAR *); *wval && buf_size; wval++ )
            {
                *out++ = *wval;
                buf_size--;
            }
            break;

        case 'x':
            dval= va_arg(args, int);
            out = itow(out, &buf_size, dval, zflag, sflag, width, 16, 0);
            break;

        case 'X':
            dval= va_arg(args, int);
            out = itow(out, &buf_size, dval, zflag, sflag, width, 16, 1);
            break;

        case '%':
            *out++ = '%';
            buf_size--;
            break;

        default:
             if (('0' == *p) && (width == 0) && (zflag == 0))
                zflag = 1;

             width = width * 10 + (*p - '0');

             goto again;
        }
    }

    *out = 0;
}


static WCHAR* app_ucs2_wsprintf_ex(WCHAR *str_buff, uint32_t buf_size, const WCHAR *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    //printf("ucs2_wsprintf_ex enter\r\n");
    app_ucs2_wsprintf_int(str_buff, buf_size, fmt, args);

    va_end(args);

    return str_buff;
}
#endif

// ======== DVFS related part ========
#ifdef HAL_DVFS_MODULE_ENABLED
static bool bt_sink_srv_mp3_dvfs_valid(uint32_t voltage, uint32_t frequency)
{
    if (frequency < BT_SINK_SRV_MP3_FREQ_L_BOUND) {
        return false;
    } else {
        return true;
    }
}

static dvfs_notification_t bt_sink_srv_mp3_dvfs_desc = {
    .domain = "VCORE",
    .module = "CM_CK0",
    .addressee = "bt_sink_srv_mp3_dvfs",
    .ops = {
        .valid = bt_sink_srv_mp3_dvfs_valid,
    }
};

static void bt_sink_srv_mp3_register_dvfs(bool flag)
{
    static bool registered = false;
    if (flag) {
        if (!registered) {
            dvfs_register_notification(&bt_sink_srv_mp3_dvfs_desc);
            hal_dvfs_target_cpu_frequency(BT_SINK_SRV_MP3_FREQ_L_BOUND, HAL_DVFS_FREQ_RELATION_L);
            bt_sink_srv_report("register dvfs");
            registered = true;
        }
    } else {
        if (registered) {
            dvfs_deregister_notification(&bt_sink_srv_mp3_dvfs_desc);
            hal_dvfs_target_cpu_frequency(104000, HAL_DVFS_FREQ_RELATION_L);
            bt_sink_srv_report("deregister dvfs");
            registered = false;
        }
    }
}
#endif /*HAL_DVFS_MODULE_ENABLED*/

static bt_sink_srv_mp3_context_t *bt_sink_srv_mp3_get_context(void)
{
    return &g_aud_cntx;
}


static void bt_sink_srv_mp3_scan_file(void)
{
    int32_t err = 0;
    int32_t ret = 0;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    bt_sink_srv_mp3_breakpoint_data_t bp_data;

    uint16_t file_total = 0;
    bt_sink_srv_mp3_event_t event;
    mp3_ctx = bt_sink_srv_mp3_get_context();
    mp3_ctx->file_index = -1;
    //err = audio_file_manager_get_file_num_by_idx(&file_total, 1);
    event.get_file_num = &file_total;
    err = bt_sink_srv_mp3_file_callback(BT_SINK_SRV_MP3_EVENT_GETFILENUM, event);
    if (!err) {
        mp3_ctx->file_total = file_total;

        // update offset
        ret = bt_sink_srv_mp3_get_breakpoint(&bp_data);
        mp3_ctx->folder_index = bp_data.folder_id;
        mp3_ctx->file_index = bp_data.file_id;
        mp3_ctx->offset = bp_data.offset;
        bt_sink_srv_report("get folder id %d file id %d offset %d",
                    bp_data.folder_id, bp_data.file_id, bp_data.offset);
    }
    bt_sink_srv_report("[MP3] scan file, ret: %d, err %d, file_total: %d", ret, err, file_total);
}

#if 0
/* scan all files recurisive, but not implmented. only support scan files under root path*/
static FRESULT scan_files_recursion(TCHAR *path)
{
    FRESULT res;
    DIR dir;
    int32_t len = 0;
    int32_t cp_len = 0;
    static FILINFO file_info = {0};
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;

    mp3_ctx = bt_sink_srv_mp3_get_context();

    #if _USE_LFN
    //file_info.lfname = g_aud_path;
    //file_info.lfsize = sizeof(g_aud_path);
    #endif

/* create a directory object */
    res = f_opendir(&dir, path);
    LOGI("[MP3]scan_files_recursion-open res: %d", res);

    if (FR_OK == res) {
        while(1) {
            res = f_readdir(&dir, &file_info);
            if (res != FR_OK || file_info.fname[0] == 0) {
                break;
            }

            if (file_info.fattrib & AM_DIR) {
                if (sizeof(TCHAR) == 2) {
                    len = app_ucs2_wcslen(path);
                    //swprintf((wchar_t *)&path[len], (BT_SINK_SRV_MP3_PATH_LENGTH - len), L"/%s", (wchar_t *)file_info.fname);
                    app_ucs2_wsprintf_ex((WCHAR *)path, (BT_SINK_SRV_MP3_PATH_LENGTH - len), (WCHAR *)(L"/%w"), file_info.fname);
                } else if (sizeof(TCHAR) == 1) {
                    len = strlen(path);
                    sprintf(&path[len], "/%s", file_info.fname);
                } else {
                }
                #if 0
                res = scan_files_recursion(path);
                if (res != FR_OK) {
                    break;;
                }
                #endif
                path[len] = '\0';
            } else {
                if (mp3_ctx->file_total < BT_SINK_SRV_MP3_FILE_NUMBER) {
                    #if _USE_LFN
                    LOGI("[MP3]scan_recursion(ext)-file: %s, len: %d\n", file_info.fname, app_ucs2_wcslen(file_info.fname));
                    if (sizeof(TCHAR) == 1) {
                        cp_len = strlen(file_info.fname);
                        if ((cp_len < 4) ||
                            (strncmp(file_info.fname + (cp_len - 4), ".mp3", 4) != 0) &&
                            (strncmp(file_info.fname + (cp_len - 4), ".MP3", 4) != 0)) {
                            continue;
                        }
                    } else if (sizeof(TCHAR) == 2) {
                        cp_len = app_ucs2_wcslen(file_info.fname);
                        if ((cp_len < 4) ||
                            (app_ucs2_wcsncmp(file_info.fname + (cp_len - 4), _T(".mp3"), 4) != 0) &&
                            (app_ucs2_wcsncmp(file_info.fname + (cp_len - 4), _T(".MP3"), 4) != 0)) {
                            continue;
                        }
                    }
                    if (cp_len > BT_SINK_SRV_MP3_FILE_LENGTH) {
                        cp_len = BT_SINK_SRV_MP3_FILE_LENGTH;
                    }

                    if (sizeof(TCHAR) == 1) {
                        strncpy(mp3_ctx->file_name[mp3_ctx->file_total++],
                            file_info.fname, cp_len);
                    } else if (sizeof(TCHAR) == 2) {
                        app_ucs2_wcsncpy(mp3_ctx->file_name[mp3_ctx->file_total++],
                            file_info.fname, cp_len);
                    }
                    #else
                    LOGI("[MP3]scan_recursion-file: %s, size: %d\n", file_info.fname, sizeof(TCHAR));
                    if (sizeof(TCHAR) == 1) {
                        cp_len = strlen(file_info.fname);
                        if ((cp_len < 4) ||
                            ((strncmp(file_info.fname + (cp_len - 4), ".mp3", 4) != 0) &&
                            (strncmp(file_info.fname + (cp_len - 4), ".MP3", 4) != 0))) {
                            continue;
                        }

                        if (cp_len > BT_SINK_SRV_MP3_FILE_LENGTH) {
                            cp_len = BT_SINK_SRV_MP3_FILE_LENGTH;
                        }
                        strncpy(mp3_ctx->file_name[mp3_ctx->file_total++],
                            file_info.fname, cp_len);
                    } else if (sizeof(TCHAR) == 2) {
                        cp_len = app_ucs2_wcslen(file_info.fname);
                        if ((cp_len < 4) ||
                            ((app_ucs2_wcsncmp(file_info.fname + (cp_len - 4), _T(".mp3"), 4) != 0) &&
                            (app_ucs2_wcsncmp(file_info.fname + (cp_len - 4), _T(".MP3"), 4) != 0))) {
                            continue;
                        }

                        if (cp_len > BT_SINK_SRV_MP3_FILE_LENGTH) {
                            cp_len = BT_SINK_SRV_MP3_FILE_LENGTH;
                        }
                        app_ucs2_wcsncpy(mp3_ctx->file_name[mp3_ctx->file_total++],
                            file_info.fname, cp_len);
                    }
                    #endif
                } else {
                    break;
                }
            }
        }
        f_closedir(&dir);
    }

    LOGI("[MP3]scan_files_recursion-res: %d, fl_total: %d\n", res,
        mp3_ctx->file_total);

    return res;
}
#endif

static void bt_sink_srv_mp3_ami_hdr(bt_sink_srv_am_id_t aud_id,
    bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg,
    void *parm)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    bt_sink_srv_am_files_media_handle_t *media = NULL;
    bt_sink_srv_music_device_t *dev = NULL;

    mp3_ctx = bt_sink_srv_mp3_get_context();
    bt_sink_srv_music_disable_log();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    bt_sink_srv_music_enable_log();

    {
        // LOGI("[MP3]ami_hdr[s]-aid: %d, aud_id: %d, msg_id: %d, sub_msg: %d\n",
                           // mp3_ctx->aud_id, aud_id, msg_id, sub_msg);
    }

    if (aud_id == mp3_ctx->aud_id) {
        switch (msg_id) {
            case AUD_FILE_OPEN_CODEC: {
                media = (bt_sink_srv_am_files_media_handle_t *)parm;
                memcpy(&(mp3_ctx->med_hd), media, sizeof(bt_sink_srv_am_files_media_handle_t));
                g_file_med_hd = &(mp3_ctx->med_hd);
                BT_SINK_SRV_SET_FLAG(dev->op, BT_SINK_SRV_MUSIC_OP_CODEC_OPEN);
    #ifdef __BT_AWS_SUPPORT__
                if (dev->aws_role == BT_AWS_ROLE_SINK && mp3_ctx->supportAws ) {
                    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK) {
                        ret = bt_sink_srv_aws_mp3_change_track_response(dev->aws_hd, BT_STATUS_SUCCESS);
                        bt_sink_srv_report("[sink][aws]change track resp-ret: %d", ret);
                    } else {
                        ret = bt_sink_srv_aws_mp3_start_response(dev->aws_hd, BT_STATUS_SUCCESS);
                        bt_sink_srv_report("[sink][aws]start streaming resp-ret: %d", ret);
                    }
                }
    #endif

                /* clear med node */
                if (BT_STATUS_SUCCESS != ret) {
                    bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
                }

                bt_sink_srv_music_mp3_state_process();
                break;
            }

            case AUD_FILE_PROC_IND: {
                if ( sub_msg == AUD_FILE_EVENT_DATA_REQ && g_file_med_hd->type != FILE_MP3 ) {
                    // if (bt_sink_srv_mp3_is_stop_and_empty()) {
                        // bt_sink_srv_report("[MP3]buffer is empty, pause music");
                        // if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK)) {
                            // bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
                        // }
                        // bt_sink_srv_music_mp3_state_process();
                        // return;
                    // }
                    bt_sink_srv_wav_write_drv_data();
                    /* send msg need write ring buffer */
                    // bt_sink_srv_mp3_local_give_semaphore();
                } else if ( sub_msg == AUD_FILE_EVENT_DATA_END ) {
                    if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK)) {
                            bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
                        }
                    bt_sink_srv_music_mp3_state_process();
                }
        #ifdef __BT_AWS_SUPPORT__
                else if ( mp3_ctx->supportAws ) {
                    if ((sub_msg == AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW) &&
                             (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) &&
                             dev->aws_hd) {
                        bt_sink_srv_report("check clock skew");
                        bt_aws_fetch_bt_time_offset(dev->aws_hd);
                    } else if (dev->aws_role == BT_AWS_ROLE_SINK) {
                        if (sub_msg == AUD_FILE_MP3_BITSTREAM_END) {

                            bt_sink_srv_report("[MP3][Abnormal]sink change track");
                            bt_sink_srv_music_mp3_set_target_and_process(AUDIO_SRC_SRV_STATE_READY);

                        } else {

                        bt_sink_srv_aws_mp3_sink_handle_data_req_ind();

                        }
                    } else {
                        bt_sink_srv_mp3_src_handle_data_req_ind(sub_msg, parm);
                    }
                } else if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_LOW_POWER_TEST) {
                    bt_sink_srv_mp3_src_handle_data_req_ind(sub_msg, parm);
                }
        #else
                bt_sink_srv_mp3_src_handle_data_req_ind(sub_msg, parm);
        #endif
                break;
            }

            case AUD_SUSPEND_BY_IND: {
                bt_sink_srv_report("[MP3]suspend ind, flag: 0x%x", dev->flag);
                bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
                bt_sink_srv_music_mp3_state_process();
                break;
            }

            case AUD_SELF_CMD_REQ: {
#if 1
                if ( sub_msg == AUD_FILE_EVENT_DATA_END && mp3_flag == false ) {
                    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK) {
                        bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_PLAYING);
                    } else {
                        
                        bt_sink_srv_state_reset(BT_SINK_SRV_STATE_LOCAL_PLAYING);
                    }
                    bt_sink_srv_music_mp3_state_process();
                }
#endif
                break;
            }

            default:
                break;
        }
    }
}

#define AUDPLY_IS_MP3_HEAD(head) (!( (((head & 0xfff00000) != 0xfff00000) && ((head & 0xfff80000) != 0xffe00000) ) || \
(((head>>17)&3)== 0) || \
( ((head>>12)&0xf) == 0xf) || ( ((head>>12)&0xf) == 0x0) || \
( ((head>>10)&0x3) == 0x3 ) || \
( ((head)& 0x3) == 0x2 )))

static const uint16_t bt_sink_srv_mp3_mp3_frame_bitpool[16][6] = {
             /* V1L1,V1L2,V1L3,V2L1,V2L2,V2L3 */
/* 0000 */    {0,0,0,0,0,0},
/* 0001 */    {32,32,32,32,8,8},
/* 0010 */    {64,48,40,48,16,16},
/* 0011 */    {96,56,48,56,24,24},
/* 0100 */    {128,64,56,64,32,32},
/* 0101 */    {160,80,64,80,40,40},
/* 0110 */    {192,96,80,96,48,48},
/* 0111 */    {224,112,96,112,56,56},
/* 1000 */    {256,128,112,128,64,64},
/* 1001 */    {288,160,128,144,80,80},
/* 1010 */    {320,192,160,160,96,96},
/* 1011 */    {352,224,192,176,112,112},
/* 1100 */    {384,256,224,192,128,128},
/* 1101 */    {416,320,256,224,144,144},
/* 1110 */    {448,384,320,256,160,160},
/* 1111 */    {0,0,0,0,0}
};

static const uint16_t bt_sink_srv_mp3_mp3_frame_samplerate_pool[][3] = {
                /* 0x00, 0x01, 0x10 */
/* MPEG-1 */    {44100, 48000, 32000},
/* MPEG-2 */    {22050, 24000, 16000},
/* MPEG-2.5 */  {11025, 12000, 8000}
};

/* The value is via Byte */
static const uint16_t bt_sink_srv_mp3_mp3_frame_count_per_frame_pool[][3] = {
           /* Layer I, Layer II, Layer III */
/* MPEG-1 */      {48,144,144},
/* MPEG-2/2.5 */  {48,144,72},
};

static uint32_t bt_sink_srv_mp3_get_mp3_frame_length(uint32_t header)
{
    bt_sink_srv_music_context_t *ctx = NULL;
    uint32_t bitrate = 0, sample_rate = 0, frame_len = 0;
    uint16_t count_per_frame = 0;
    uint8_t layer, index, index_v_l, padding, version, sample_rate_index;

    ctx = bt_sink_srv_music_get_context();
    version = (header>>19) & 0x3;
    layer = (header>>17) & 0x3;
    index = (header>>12) & 0xf;
    sample_rate_index =  (header>>10) & 0x3;
    padding = (header>>9) & 0x1;

    //bt_sink_srv_report("Frame(%x) version(%d), layer(%d), index(%d), sample_index(%d), padding(%d)", header, version, layer, index, sample_rate_index, padding);

    layer = 3- layer; /* to get the index of layer*/

    switch(version) {
        case 0:
        {
            /* MPEG2.5 */
           index_v_l = 3 + layer; /* 0,1,2 is for MPEG1 */
           bitrate = bt_sink_srv_mp3_mp3_frame_bitpool[index][index_v_l] * 1000;
           sample_rate = bt_sink_srv_mp3_mp3_frame_samplerate_pool[2][sample_rate_index];
           count_per_frame = bt_sink_srv_mp3_mp3_frame_count_per_frame_pool[1][layer];
           frame_len = (count_per_frame * bitrate) / sample_rate + padding;
           break;
        }
        case 1:
        {
            /* Reserved */
            bt_sink_srv_report("Wronly version happen");
            break;
        }
        case 2:
        {
           /* MPEG 2 */
           index_v_l = 3 + layer; /* 0,1,2 is for MPEG1 */
           bitrate = bt_sink_srv_mp3_mp3_frame_bitpool[index][index_v_l] * 1000;
           sample_rate = bt_sink_srv_mp3_mp3_frame_samplerate_pool[1][sample_rate_index];
           count_per_frame = bt_sink_srv_mp3_mp3_frame_count_per_frame_pool[1][layer];
           frame_len = (count_per_frame * bitrate) / sample_rate + padding;
           break;
        }
        case 3:
        {
           /* MPEG 1 */
           index_v_l = layer; /* 0,1,2 is for MPEG1 */
           bitrate = bt_sink_srv_mp3_mp3_frame_bitpool[index][index_v_l] * 1000;
           sample_rate = bt_sink_srv_mp3_mp3_frame_samplerate_pool[0][sample_rate_index];
           count_per_frame = bt_sink_srv_mp3_mp3_frame_count_per_frame_pool[0][layer];
           /* MP3 file may not handle the padding *4, first set to 1 */
           frame_len = (count_per_frame * bitrate) / sample_rate + padding;
           break;
        }
        default:
        {
            bt_sink_srv_report("Wronly version happen 2 %d", version);
            break;
        }
    }

    ctx->med_play_info.sample_rate = sample_rate;
    g_finfo.sample_count_per_frame = count_per_frame * 8;
#ifdef HAL_DVFS_MODULE_ENABLED
    if (bitrate >= BT_SINK_SRV_MP3_BITRATE_MINIMUM) {
        bt_sink_srv_mp3_register_dvfs(true);
    }
#endif /*HAL_DVFS_MODULE_ENABLED*/
    //bt_sink_srv_report("Frame bitrate(%d), Sample_rate(%d), Count_per_frame(%d), Frame_len(%d)", bitrate, sample_rate, count_per_frame, frame_len);
    return frame_len;
}

static bool bt_sink_srv_mp3_check_mp3_frame_header(uint8_t *buff, uint32_t *frame_len)
{

    uint32_t temp_mpeg_header = 0;
    bool result = false;



    temp_mpeg_header = *buff;
    temp_mpeg_header = temp_mpeg_header << 8 | *(buff + 1);
    temp_mpeg_header = temp_mpeg_header << 8 | *(buff + 2);
    temp_mpeg_header = temp_mpeg_header << 8 | *(buff + 3);

    if (AUDPLY_IS_MP3_HEAD(temp_mpeg_header)) {
        result = true;
        if (frame_len) {
            *frame_len = bt_sink_srv_mp3_get_mp3_frame_length(temp_mpeg_header);
            if (*frame_len == 0) {
                /* The condistion or calcuate happen error */
                result = false;
            }
        }
    }

    return result;

}

// search mp3 frame header, return mp3 frame length
static uint32_t bt_sink_srv_mp3_search_mp3_header(uint8_t *buf, uint32_t buf_len, uint32_t *frame_start)
{
    uint32_t frame_len = 0;
    *frame_start = 0;
    do {
        if (g_finfo.frame_header != 0) {
            uint32_t tmp_header = *(uint32_t *)(buf + *frame_start) & BT_SINK_SRV_MP3_MP3_HEADER_MASK;
            if (g_finfo.frame_header == tmp_header) {
                bt_sink_srv_mp3_check_mp3_frame_header(buf + *frame_start, &frame_len);
                //bt_sink_srv_report("get MP3 header len = %d retry %d", frame_len, *frame_start);
                return frame_len;
            }
        /* check whether is MP3 frame header */
        } else if (bt_sink_srv_mp3_check_mp3_frame_header(buf + *frame_start, &frame_len)) {
            //bt_sink_srv_report("[MP3]find the next frame1 %x %x", *(buf + *frame_start), *(buf + *frame_start));
            if (frame_len + *frame_start > (buf_len - 4)) {
                /* Noramlly MP3 file each continues frame interval will not more than 3K */
                break;
            }

            /* Check the frame_len + header wheter correct */
            if (!bt_sink_srv_mp3_check_mp3_frame_header(buf + *frame_start + frame_len, NULL))
            {
                continue;
            }
            return frame_len;
        }

        /* not find the header */

    } while((*frame_start)++ <= (buf_len - 4));
    //bt_sink_srv_report("[MP3]Skip failed");

    /* find header should exited before */
    return 0;
}

static bool bt_sink_srv_mp3_get_mp3_frame_offset(bt_sink_srv_mp3_id3v2_information_t *info, FIL *fp, uint32_t total_size)
{
    uint8_t ID3_tag_header_buff[BT_SINK_SRV_MP3_ID3v2_HEADER];
    uint8_t *temp_frame_buff; /*for reach the frist frame of MP3  */
    UINT read_len = 0;
    uint32_t id3v2_remain_tagesize = 0;
    uint32_t id3v2_tage_size = 0;
    FRESULT fs_res = FR_OK;
    bool res = false;

    info->tag_size += bt_sink_srv_mp3_tell_file(fp);
    bt_sink_srv_report("[MP3]offset %d, total %d size %d",
            bt_sink_srv_mp3_tell_file(fp), info->tag_size, bt_sink_srv_mp3_size_file(fp));

    while (1) {

        fs_res = bt_sink_srv_mp3_read_file(fp, ID3_tag_header_buff, BT_SINK_SRV_MP3_ID3v2_HEADER, &read_len);
        LOGI("[MP3]id3v2 f_read %d, len %d", fs_res, read_len);
        if (fs_res != FR_OK || read_len < BT_SINK_SRV_MP3_ID3v2_HEADER) {
            return false; /* could not continue, parse failed */
        }

        if (strncmp((const char *)ID3_tag_header_buff, "ID3", 3) == 0) {
            id3v2_remain_tagesize = ((ID3_tag_header_buff[6] & 0x7f) << 21) | ((ID3_tag_header_buff[7] & 0x7f) << 14) | ((ID3_tag_header_buff[8] & 0x7f) <<  7) | ((ID3_tag_header_buff[9] & 0x7f) <<  0);
            id3v2_tage_size = id3v2_remain_tagesize + BT_SINK_SRV_MP3_ID3v2_HEADER;
            LOGI("[MP3]find id3v2: id3v2_tagesize=%ld, id3v2_remain_tagesize =%ld\n", id3v2_tage_size, id3v2_remain_tagesize);

            if (total_size < id3v2_tage_size) {
                /* the tag size calcuated wrongly */
                return false;
            }

            /* have id3v2 */
            info->have_id3v2 = true;
            info->tag_size += id3v2_tage_size;

            fs_res = bt_sink_srv_mp3_lseek_file(fp, (DWORD)info->tag_size);
            LOGI("[MP3]id3v2 f_lseek %d", fs_res);
        } else {
            LOGI("[MP3]done skip ID3v2, has_id3v2=%d, id3v2_tage_length=%d\n", info->have_id3v2, info->tag_size);
            break;
        }
    }

/* For reach the first frame of MP3, some MP3 may have other data between ID3 header and frame */
    {

        uint32_t frame_len = 0;
        uint32_t frame_start;
        /* ID3 header already skiped*/
        temp_frame_buff = pvPortMalloc(AUIDO_SKIP_ID3_FILE_LENGTH);
        if (temp_frame_buff == NULL) {

            LOGI("[MP3]Buffer allocated failed");
            return false;
        }

        while ((info->tag_size < bt_sink_srv_mp3_size_file(fp)) && (fs_res == FR_OK)) {
            /*Whether have id3v2, it need find the first MP3 frame*/
            memcpy(temp_frame_buff, ID3_tag_header_buff, BT_SINK_SRV_MP3_ID3v2_HEADER);
            /*Try to read 1k buffer to find the frame*/
            fs_res = bt_sink_srv_mp3_read_file(fp, temp_frame_buff+BT_SINK_SRV_MP3_ID3v2_HEADER, AUIDO_SKIP_ID3_FILE_LENGTH - BT_SINK_SRV_MP3_ID3v2_HEADER, &read_len);
            read_len = MINIMUM(read_len, AUIDO_SKIP_ID3_FILE_LENGTH);

            frame_len = bt_sink_srv_mp3_search_mp3_header(temp_frame_buff, read_len, &frame_start);

            if (frame_len != 0) {
                info->tag_size += frame_start;
                res = true;
                bt_sink_srv_report("[MP3]find the next frame %d", info->tag_size);
                break;
            // try to find header in remaining data of file
            } else {
                info->tag_size = bt_sink_srv_mp3_tell_file(fp);
                fs_res = bt_sink_srv_mp3_read_file(fp, ID3_tag_header_buff, BT_SINK_SRV_MP3_ID3v2_HEADER, &read_len);
                bt_sink_srv_report("[MP3] can't find frame %d", info->tag_size);
            }
        }
        vPortFree(temp_frame_buff);
    }

    /* no id3v2 can play noramlly */
    return res;

}

#ifdef __BT_AWS_SUPPORT__
/*
 * find suitable frame and duration for play
 * return duration
 */
uint32_t bt_sink_srv_mp3_find_suitable_frame(void)
{
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_mp3_frame_info_t *finfo;
    bt_sink_srv_music_aws_sync_info *sync_info = NULL;
    sink_srv_clock_mapping_t base;
    uint32_t duration = 0;
    uint32_t play_ts = 0;
    uint64_t play_us = 0;
    bt_aws_clock_t cur_bt_clk;

    ctx = bt_sink_srv_music_get_context();
    mp3_ctx = bt_sink_srv_mp3_get_context();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    finfo = bt_sink_srv_mp3_local_file_info_get_context();
    sync_info = &(dev->local_info);

    // set base play info
    base.time_stamp = sync_info->pkt_info.ts;
    base.seq_idx = sync_info->pkt_info.sn;
    base.piconet_clk.nclk = sync_info->clock.nclk;
    base.piconet_clk.nclk_intra = sync_info->clock.nclk_intra;
    play_ts = finfo->first_frame_ts;

    if ((sync_info->clock.nclk == 0 &&
            sync_info->clock.nclk_intra == 0) ||
            dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET) {
        bt_sink_srv_report("offset or init clock not ready");
        return BT_SINK_SRV_MP3_AWS_INVALID_DUR;
    }

    bt_aws_clock_t cur_local_clk;
    bt_aws_get_bt_local_time(&cur_local_clk);
    bt_aws_get_curr_bt_time(dev->gap_role, dev->offset, &cur_bt_clk);
    bt_sink_srv_report("clk offset %x bt clk %x local clk %x", 
            dev->offset.offset, cur_bt_clk.nclk, cur_local_clk.nclk);
    ctx->med_play_info.aac_codec = 0;
    ctx->med_play_info.count_per_sample = 1;
    duration = sink_srv_get_play_duration(&(ctx->med_play_info), &base, &cur_bt_clk, play_ts);
    duration += BT_SINK_SRV_MP3_AWS_TIMER_DUR;

    // because data already put into mp3 driver, no way to rollback.
    // so, clear mp3 buffer and restart play again
    if (false == bt_sink_srv_music_duration_valid(BT_SINK_SRV_AWS_MODULE_ID_MP3, duration)) {
        bt_sink_srv_report("check duration failed, try to restart play again");
        duration = BT_SINK_SRV_MP3_AWS_INVALID_DUR;
        g_file_med_hd->media_handle.mp3.stop(mp3_ctx->aud_id);
        g_file_med_hd->media_handle.mp3.aws_deinit();
        buff_mgr_reset_share_buffer();
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_PLAY);
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
    }

    // set new play clock
    play_ts = play_ts - base.time_stamp;
    play_us = (((uint64_t)play_ts) * 1000 * 1000 / ctx->med_play_info.sample_rate) +
        BT_SINK_SRV_MP3_AWS_TIMER_DUR;
    dev->play_time.nclk = (base.piconet_clk.nclk + (play_us * 4 / 1250)) & 0x0FFFFFFF;
    dev->play_time.nclk_intra = base.piconet_clk.nclk_intra + (play_us % 1250);
    bt_sink_srv_report("set clk %x intra %x base %x intra %x",
            dev->play_time.nclk, dev->play_time.nclk_intra,
            base.piconet_clk.nclk, base.piconet_clk.nclk_intra);

    bt_sink_srv_report("find frame ts %d duration %d clk offset %x", play_ts, duration, dev->offset);

    return duration;
}

void bt_sink_srv_aws_mp3_gpt_cb(void *user_data)
{
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    uint32_t ret = 0;
    bt_aws_clock_t bt_clock;
    uint64_t curr_clock;
    uint64_t play_clock;

    ctx = bt_sink_srv_music_get_context();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    /* reset gpt */
    hal_gpt_stop_timer(ctx->gpt_port);
    hal_gpt_deinit(ctx->gpt_port);
    ctx->gpt_port = HAL_GPT_MAX_PORT;

    if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_PLAY)) {
        bt_sink_srv_report("[sink][mp3]gpt_cb-start play suspended");
        return;
    }

    play_clock = ((uint64_t)dev->play_time.nclk) * 312 +
                     (dev->play_time.nclk >> 1) +
                     dev->play_time.nclk_intra;
    __disable_irq();
    //hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_HIGH);
    while(1)
    {
        bt_aws_get_curr_bt_time(dev->gap_role, dev->offset, &bt_clock);

        curr_clock = (uint64_t)bt_clock.nclk * 312 + ((uint64_t)bt_clock.nclk >> 1) +
            (uint64_t)bt_clock.nclk_intra;
        if (curr_clock >= play_clock)
        {
            hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_LOW);
            hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_HIGH);
            hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_LOW);

            /*uint8_t log_buff[512];
            sprintf((char *)log_buff, "curr native clock, nclk:%x, nclk_intra:%x, play_clock:%x\n", nclk, nclk_intra, play_clock);
            atci_send_data(log_buff, strlen(log_buff));*/
            break;
        }
    }
    ret = g_file_med_hd->media_handle.mp3.set_aws_initial_sync(dev->mp3->aud_id);
    __enable_irq();
    bt_sink_srv_audio_clock_init(&dev->offset);
    bt_sink_srv_report("[sink][mp3]gpt_cb-start play--ret: %d", ret);
    bt_sink_srv_music_mp3_state_process();
}

// send aws data and return remain length that not able to send
static int32_t bt_sink_srv_mp3_aws_send_streaming_data(uint8_t *data, int32_t total_len)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t remain_len, sent_len;
    uint32_t attempt = 0;
    bt_sink_srv_mp3_media_header mp3_hdr;
    bt_aws_streaming_packet_t packet;
    int32_t ret = 0;

    bt_sink_srv_music_disable_log();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    bt_sink_srv_music_enable_log();
    remain_len = total_len;

    // do not send if it has no aws link or it's aws sink.
    if (!(dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) ||
          (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_AWS_STATE_SYNC) ||
          (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_SOURCE_AVAILABLE) &&
          (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT))) {
        ret = bt_sink_srv_mp3_write_aws_header(&mp3_hdr, data);
        if (ret < 0) {
            /* Find context not belong to mp3 header, switch to next track */
            return -1;
        }
        if (bt_sink_srv_mp3_update_frame_info(&data, &total_len) == 0) {
            return 0;
        } else {
            /* Find context not belong to mp3 header, swtich to next track */
            return -2;
        }
    // aws state is not correct, need to negotiate
    } else if ((dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) &&
          (dev->aws_role != BT_AWS_ROLE_SOURCE)) {
        bt_sink_srv_mp3_update_frame_info(&data, &total_len);
        BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_AWS_STATE_SYNC);
        bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_PLAYING);
        bt_sink_srv_music_mp3_state_process();
        return 0;
    }

    do {
        ret = bt_sink_srv_mp3_write_aws_header(&mp3_hdr, data);
        if (ret < 0) {
            /* Find context not belong to mp3 header, switch to next track */
            return -1;
        }
        bt_sink_srv_aws_mp3_build_streaming_packet((uint8_t *)&mp3_hdr, data, remain_len, &packet);
        if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_STOPPING)) {
            sent_len = bt_aws_send_streaming_packet(dev->aws_hd, BT_SINK_SRV_AWS_MODULE_ID_MP3, &packet);
        } else {
            sent_len = 0;
        }

        remain_len -= sent_len;
        //bt_sink_srv_report("sent %d remain %d flag %x", sent_len, remain_len, dev->flag);
        attempt++;
        if (sent_len != 0) {
            ret = bt_sink_srv_mp3_update_frame_info(&data, &sent_len);
            /* Find context not belong to mp3 header, switch to next track */
            if (ret < 0) {
                return -2;
            }
            data += sent_len;
        }
    } while (sent_len != 0 && remain_len != 0);
    if (remain_len != 0) {
        ret = bt_sink_srv_mp3_update_frame_info(&data, &remain_len);
        /* Find context not belong to mp3 header, switch to next track */
        if (ret < 0) {
            return -3;
        }
        bt_sink_srv_report("bt_sink_srv_mp3_aws_send_streaming_data incomplete remain %d", remain_len);
    }

    return remain_len;
}
#endif

extern int32_t ucs2_wcslen(const TCHAR *string);
extern int32_t ucs2_wcsncmp(const TCHAR *str_src,
                           const TCHAR *str_dst,
                           uint32_t count);
am_file_type_t get_file_type()
{
    am_file_type_t type;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    mp3_ctx = bt_sink_srv_mp3_get_context();

#ifdef MTK_MP3_ON_FLASH_ENABLE
    type = FILE_MP3;
    mp3_ctx->supportAws = true;
    bt_sink_srv_report("[MP3]get_file_type(1)--type: %d, name: %s", type, mp3_ctx->curr_file_path);
#else
    int32_t len = 0;
    len = ucs2_wcslen(mp3_ctx->curr_file_path);
    if ((ucs2_wcsncmp(&mp3_ctx->curr_file_path[len - 4], (const TCHAR *)".MP3", 4)) == 0) {
        type = FILE_MP3;
        mp3_ctx->supportAws = true;
    } else if ((ucs2_wcsncmp(&mp3_ctx->curr_file_path[len - 4], (const TCHAR *)".WAV", 4)) == 0) {
        type = FILE_WAV;
        mp3_ctx->supportAws = false;
    } else {
        type = FILE_NONE;
        mp3_ctx->supportAws = false;
    }

    // for low power test
    bt_sink_srv_music_device_t *dev = NULL;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_LOW_POWER_TEST) {
        mp3_ctx->supportAws = false;
    }

    bt_sink_srv_report("[MP3]get_file_type(2)--type: %d, name: %s", type, mp3_ctx->curr_file_path);
#endif

    return type;
}

static void bt_sink_srv_mp3_local_play(audio_src_srv_handle_t *handle)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t err = 0;
    bt_sink_srv_am_id_t ami_ret = 0;
    bt_sink_srv_am_audio_capability_t  aud_cap;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    bt_sink_srv_mp3_id3v2_information_t id3v2_info = {0};
    bt_sink_srv_mp3_event_t event;
    FRESULT fs_res = FR_OK;
    bool result;
    am_file_type_t type = FILE_NONE;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    mp3_ctx = bt_sink_srv_mp3_get_context();

    if (mp3_ctx->audio_src != handle) {
        bt_sink_srv_report("[MP3] invalid handle %x, mp3 handle %x",
                handle, mp3_ctx->audio_src);
        return;
    }

    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT) {
        bt_sink_srv_report("[MP3] interrupt resume");
        if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK)) {
            BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
            bt_sink_srv_music_mp3_set_target_and_process(AUDIO_SRC_SRV_STATE_PLAYING);
        }
        return;
    }

    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_STOPPING);

    /* open file */
    if (mp3_ctx->file_total > 0
#ifdef __BT_AWS_SUPPORT__
            && dev->aws_role != BT_AWS_ROLE_SINK
#endif
            ) {
        if (mp3_ctx->offset > 0) {
            /* open file if file closed */
            if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_FILE_OPEN)) {
                // get the file path name.
                //int32_t err = audio_file_manager_get_file_path_by_idx(new_file_path, MAX_FILE_PATH_LEN, 1, mp3_ctx->file_index);
                event.get_file_path_by_idx.path = mp3_ctx->curr_file_path;
                event.get_file_path_by_idx.idx = mp3_ctx->file_index;
                err = bt_sink_srv_mp3_file_callback(BT_SINK_SRV_MP3_EVENT_GETFILEPATHBYIDX, event);
                if (err < 0 ) {
                    bt_sink_srv_report("[MP3]check file idx error %d", err);
                    return;
                }
                bt_sink_srv_report("[MP3]bt_sink_srv_mp3_local_play(1)--idx: %d, name: %s",
                                                        mp3_ctx->file_index, mp3_ctx->curr_file_path);
                ret = bt_sink_srv_mp3_open_file(&g_aud_fdst, mp3_ctx->curr_file_path);
                type = get_file_type();
                // prevent over size offset
                if (( type == FILE_MP3) &&
                        (mp3_ctx->offset >= bt_sink_srv_mp3_size_file(&g_aud_fdst))) {
                    mp3_ctx->offset = 0;
                }
                fs_res = bt_sink_srv_mp3_lseek_file(&g_aud_fdst, (DWORD)mp3_ctx->offset);
            }
        } else {
            /* merge file path */
            //int32_t err = audio_file_manager_get_file_path_by_idx(new_file_path, MAX_FILE_PATH_LEN, 1, mp3_ctx->file_index);
            event.get_file_path_by_idx.path = mp3_ctx->curr_file_path;
            event.get_file_path_by_idx.idx = mp3_ctx->file_index;
            err = bt_sink_srv_mp3_file_callback(BT_SINK_SRV_MP3_EVENT_GETFILEPATHBYIDX, event);
            if (err < 0 ) {
               bt_sink_srv_report("[MP3]check file idx error %d", err);
               return;
            }
            bt_sink_srv_report("[MP3]bt_sink_srv_mp3_local_play(2)--idx: %d, name: %s", mp3_ctx->file_index, mp3_ctx->curr_file_path);
            ret = bt_sink_srv_mp3_open_file(&g_aud_fdst, mp3_ctx->curr_file_path);
            bt_sink_srv_report("[MP3]bt_sink_srv_mp3_local_play fs size: %d, %d",  (uint32_t)bt_sink_srv_mp3_size_file(&g_aud_fdst), g_aud_fdst.obj.objsize);
            type = get_file_type();
        }
        if ( type == FILE_MP3 ) {
            result = bt_sink_srv_mp3_get_mp3_frame_offset(&id3v2_info, &g_aud_fdst, bt_sink_srv_mp3_size_file(&g_aud_fdst));

            if (result) {
                mp3_ctx->offset = id3v2_info.tag_size;
                bt_sink_srv_mp3_lseek_file(&g_aud_fdst, (DWORD)id3v2_info.tag_size);
            } else {
                bt_sink_srv_report("[MP3]bt_sink_srv_mp3_local_play- failed");
#ifndef MTK_MP3_ON_FLASH_ENABLE
                f_close(&g_aud_fdst);
#endif

                if (mp3_ctx->retry_cnt < 3) {
                    mp3_ctx->retry_cnt++;
                    bt_sink_srv_mp3_local_change_track(MP3_NEXT_TRACK);
                    bt_sink_srv_mp3_local_play(handle);
                } else {
                    mp3_ctx->retry_cnt = 0;
                    bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
                    bt_sink_srv_music_mp3_state_process();
                }

	            return;
            }
        }
    } else {
        bt_sink_srv_report("[MP3]bt_sink_srv_mp3_local_play-no mp3 file");
        type = FILE_MP3;
        mp3_ctx->supportAws = true;
    }

    /* ami play */
    if (mp3_ctx->file_total > 0 ||
            dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT) {
        /* AM fill */
        memset(&aud_cap, 0x00, sizeof(bt_sink_srv_am_audio_capability_t));
        buff_mgr_reset_share_buffer();

        aud_cap.type = FILES;
        aud_cap.codec.files_format.file_type = type;
        aud_cap.audio_stream_out.audio_device = (bt_sink_srv_am_device_set_t)mp3_ctx->dev;
        aud_cap.audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t)mp3_ctx->vol_lev;
        aud_cap.audio_stream_out.audio_mute = false;
        /* play will triiger the callback that send in the open function */
        ami_ret = bt_sink_srv_ami_audio_play(mp3_ctx->aud_id, &aud_cap);

        bt_sink_srv_report("[MP3]bt_sink_srv_mp3_local_play-ret: %d, err: %d, file_index: %d, ami_ret: %d, fs_res: %d vol: %d",
                           ret, err, mp3_ctx->file_index, ami_ret, fs_res, mp3_ctx->vol_lev);
    }
    return;
}


static void bt_sink_srv_mp3_local_stop(audio_src_srv_handle_t *handle)
{
    int32_t ret = 0;
    int32_t err = 0;

    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    bt_sink_srv_mp3_breakpoint_data_t bp_data;

    mp3_ctx = bt_sink_srv_mp3_get_context();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);

    if (mp3_ctx->audio_src != handle) {
        bt_sink_srv_report("[MP3] invalid handle %x, mp3 handle %x",
                handle, mp3_ctx->audio_src);
        return;
    }
	xSemaphoreTake(g_file_hd_semaphore, portMAX_DELAY);

#ifdef __BT_AWS_SUPPORT__
    bt_sink_srv_music_context_t *ctx = NULL;
    ctx = bt_sink_srv_music_get_context();
    // clear timer
    if ( mp3_ctx->supportAws ) {
        if (ctx->gpt_port != HAL_GPT_MAX_PORT) {
            hal_gpt_stop_timer(ctx->gpt_port);
            hal_gpt_deinit(ctx->gpt_port);
            bt_sink_srv_report("[MP3] stop untriggered timer %d ret = %d", ctx->gpt_port, ret);
            ctx->gpt_port = HAL_GPT_MAX_PORT;
        }
        bt_sink_srv_memset(&(dev->local_info), 0, sizeof(bt_sink_srv_music_aws_sync_info));
    }
#endif

    if (g_file_med_hd) {
        if ( g_file_med_hd->type == FILE_MP3 ) {
            mp3_flag = 1;
        }
        else {
            mp3_flag = 0;
        }

        uint32_t buffer_remain_data = buff_mgr_get_data_count() + 
                    g_file_med_hd->media_handle.mp3.get_data_count(mp3_ctx->aud_id);
        g_file_med_hd->media_handle.mp3.stop(mp3_ctx->aud_id);
#ifdef __BT_AWS_SUPPORT__
        if ( mp3_ctx->supportAws ) {
            g_file_med_hd->media_handle.mp3.aws_deinit();
        }
#endif
        if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK)) {
            mp3_ctx->offset = bt_sink_srv_mp3_tell_file(&g_aud_fdst);
            if (mp3_ctx->offset > buffer_remain_data) {
                mp3_ctx->offset -= buffer_remain_data;
            }
            bt_sink_srv_report("[MP3] file offset %d remain %d",
                mp3_ctx->offset, buffer_remain_data);
        }
#ifndef MTK_MP3_ON_FLASH_ENABLE
        if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_FILE_OPEN) {
            f_close(&g_aud_fdst);
        }
#endif

        bp_data.folder_id = mp3_ctx->folder_index;
        bp_data.file_id = mp3_ctx->file_index;
        bp_data.offset = mp3_ctx->offset;
        bt_sink_srv_mp3_set_breakpoint(&bp_data);

        g_file_med_hd = NULL;
    } else {
        err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

    bt_sink_srv_ami_audio_stop(mp3_ctx->aud_id);
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_ALL);
    BT_SINK_SRV_REMOVE_FLAG(dev->op, BT_SINK_SRV_MUSIC_OP_CODEC_OPEN);
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
    buff_mgr_reset_share_buffer();

    bt_sink_srv_report("[MP3]bt_sink_srv_mp3_local_stop-ret: %d, err: %d, file_index: %d",
                       ret, err, mp3_ctx->file_index);
	xSemaphoreGive(g_file_hd_semaphore);

    if ( mp3_flag ) {
        bt_sink_srv_music_mp3_state_process();
    }
    else {
        //do nothing
    }
#ifdef HAL_DVFS_MODULE_ENABLED
    bt_sink_srv_mp3_register_dvfs(false);
#endif /*HAL_DVFS_MODULE_ENABLED*/

    return;
}


/* dummy */
static void bt_sink_srv_mp3_suspend(audio_src_srv_handle_t *handle1, audio_src_srv_handle_t *handle2)
{
    bt_sink_srv_music_device_t *dev = NULL;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    bt_sink_srv_report("[MP3] bt_sink_srv_mp3_suspend");

#ifdef __BT_AWS_SUPPORT__
    // set flag to send notify if it's aws sink
    if (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT &&
            dev->aws_role == BT_AWS_ROLE_SINK) {
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
        bt_sink_srv_aws_mp3_notify_stop(dev->aws_hd);
    }
#endif
    BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT);
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK);
    bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
    audio_src_srv_add_waiting_list(handle1);
    bt_sink_srv_music_mp3_state_process();
}


static void bt_sink_srv_mp3_reject(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    mp3_ctx = bt_sink_srv_mp3_get_context();

    if (mp3_ctx->audio_src != handle) {
        bt_sink_srv_report("[MP3] invalid handle %x, mp3 handle %x",
                handle, mp3_ctx->audio_src);
        return;
    }

    bt_sink_srv_report("[Sink][MP3] reject");
    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_ALL);
    bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
    bt_sink_srv_music_mp3_state_process();
}


static void bt_sink_srv_mp3_exception(audio_src_srv_handle_t *handle, int32_t event, void *param)
{
}


static int32_t bt_sink_srv_mp3_local_change_track(uint8_t type)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t index = 0;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    int32_t ret = 0;
    int32_t err = 0;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    mp3_ctx = bt_sink_srv_mp3_get_context();

    index = mp3_ctx->file_index;

    if (MP3_PREVIOUS_TRACK != type) {
        if (index == mp3_ctx->file_total) {
            index = 1;
        } else {
            index = index + 1;
        }
    } else {
        if (index == 1) {
            index = mp3_ctx->file_total;
        } else {
            index = index - 1;
        }
    }

    mp3_ctx->file_index = index;

    mp3_ctx->offset = 0;
    //BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_STOPPING);
    BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK);



    bt_sink_srv_report("[MP3]bt_sink_srv_mp3_local_change_track-ret: %d, err: %d, file_index: %d, type: %d",
                   ret, err, mp3_ctx->file_index, type);

    return ret;
}


static int32_t bt_sink_srv_mp3_local_change_volume(uint8_t type)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    bt_sink_srv_am_id_t ami_ret = 0;
    uint8_t vol = AUD_VOL_OUT_LEVEL0;

    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;

    mp3_ctx = bt_sink_srv_mp3_get_context();

    vol = mp3_ctx->vol_lev;
    /* volume up */
    if (MP3_VOLUME_UP == type) {
       if (vol < AUD_VOL_OUT_LEVEL15) {
           vol = vol + 1;
       } else {
           ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
       }
    } else if (MP3_VOLUME_DOWN == type) {
        if (vol > AUD_VOL_OUT_LEVEL0) {
            vol = vol - 1;
        } else {
            ret = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
        }
    }

    if (vol != mp3_ctx->vol_lev) {
        ami_ret = bt_sink_srv_ami_audio_set_volume(mp3_ctx->aud_id, vol, STREAM_OUT);
        mp3_ctx->vol_lev = vol;
    }

    bt_sink_srv_report("[MP3]change_volume-ami_ret: %d, ret: %d, vol: %d",
        ami_ret, ret, mp3_ctx->vol_lev);

    return mp3_ctx->vol_lev;
}


static void bt_sink_srv_wav_write_drv_data(void)
{
    int32_t loop_idx;
    int32_t loop_cnt = 0;
    uint32_t read_byte_cnt  = 0;
    uint32_t write_byte_cnt = 0;
    uint32_t move_byte_cnt = 0;
    uint32_t read_size = 0;//, write_size = 0;
    //uint32_t check_size = 0;
    uint8_t *p_mcu_buf = NULL;
    uint8_t *p_dsp_buf = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    mp3_ctx = bt_sink_srv_mp3_get_context();

    if (g_file_med_hd) {
    // read file if no data to mp3 buffer
    read_size = buff_mgr_get_data_count();
    if (read_size == 0) {
        bt_sink_srv_mp3_read_data();
    }

        //write_size = g_file_med_hd->media_handle.mp3.get_free_space(mp3_ctx->aud_id);
        //check_size =
            //g_file_med_hd->media_handle.mp3.get_data_count(mp3_ctx->aud_id) + read_size;

        loop_cnt = 4;
        for (loop_idx = 0; loop_idx < loop_cnt ; loop_idx++) {
            g_file_med_hd->media_handle.mp3.get_write_buffer(mp3_ctx->aud_id, &p_dsp_buf, &write_byte_cnt);
            buff_mgr_get_read_buffer(&p_mcu_buf, &read_byte_cnt);
            //bt_sink_srv_report("[MP3]write drv %d--bt %d mp3 %d frame byte %d %d %d",
            //        loop_idx, buff_mgr_get_data_count(),
            //        g_file_med_hd->media_handle.mp3.get_data_count(mp3_ctx->aud_id),
            //        frame_byte_cnt, read_byte_cnt, write_byte_cnt);
            move_byte_cnt = MINIMUM(write_byte_cnt, read_byte_cnt);
            /* write data to driver */
            memcpy(p_dsp_buf, p_mcu_buf, move_byte_cnt);

            /* update ring buffer */
            if (move_byte_cnt > 0) {
                buff_mgr_read_data_done(move_byte_cnt);
                g_file_med_hd->media_handle.mp3.write_data_done(mp3_ctx->aud_id, move_byte_cnt);
            }
        }
        //bt_sink_srv_report("[MP3]write drv--bt %d mp3 %d", buff_mgr_get_data_count(), g_file_med_hd->media_handle.mp3.get_data_count(mp3_ctx->aud_id));
        g_file_med_hd->media_handle.mp3.finish_write_data(mp3_ctx->aud_id);

    } else {
        bt_sink_srv_report("[MP3]write_drv_data(err)--flag: 0x%x", dev->flag);
    }
}

static void bt_sink_srv_mp3_write_drv_data(void)
{
    int32_t loop_idx;
    int32_t loop_cnt = 0;
    uint32_t read_byte_cnt  = 0;
    uint32_t write_byte_cnt = 0;
    uint32_t move_byte_cnt = 0;
    uint32_t frame_byte_cnt = 0;
    uint32_t read_size = 0, write_size = 0;
    uint8_t *p_mcu_buf = NULL;
    uint8_t *p_dsp_buf = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    bt_sink_srv_mp3_frame_info_t *finfo;

    bt_sink_srv_music_disable_log();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    bt_sink_srv_music_enable_log();
    mp3_ctx = bt_sink_srv_mp3_get_context();
    finfo = bt_sink_srv_mp3_local_file_info_get_context();

    if (g_file_med_hd) {
        read_size = buff_mgr_get_data_count();
        write_size = g_file_med_hd->media_handle.mp3.get_free_space(mp3_ctx->aud_id);

        #ifdef __BT_AWS_SUPPORT__
        uint32_t check_size =
            //g_file_med_hd->media_handle.mp3.get_data_count(mp3_ctx->aud_id) + read_size;
            g_file_med_hd->media_handle.mp3.get_data_count(mp3_ctx->aud_id);
        // no data to fill but driver is going to underflow, put silence data to share buffer
        if (check_size < (finfo->last_frame_len * 2) &&
                    read_size < finfo->last_frame_len &&
                    (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_PLAYING)
                    && !(dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_STOPPING)
                    && mp3_ctx->supportAws
                    && (dev->aws_role == BT_AWS_ROLE_SINK)
            ) {
            bt_sink_srv_report("[sink][aws]check %d read %d frame %d",
                    check_size, read_size, finfo->last_frame_len);
            buff_mgr_rollback_write_buffer(MINIMUM(read_size, finfo->remain_data_size));
            finfo->remain_data_size -= MINIMUM(read_size, finfo->remain_data_size);
            bt_sink_srv_mp3_fill_silence_frame(BT_SINK_SRV_MP3_DEFAULT_SILENCE_NUM);
            read_size = buff_mgr_get_data_count();
            frame_byte_cnt = read_size;
        } else if (read_size < finfo->last_frame_len || write_size < finfo->last_frame_len) {
        #else
        if (read_size < finfo->last_frame_len || write_size < finfo->last_frame_len) {
        #endif
            // no data to fill and driver still has enough data
            //bt_sink_srv_report("[MP3] not enough 1 frame %d %d %d", read_size, write_size, check_size);
            g_file_med_hd->media_handle.mp3.finish_write_data(mp3_ctx->aud_id);
            return;
        } else {
            // must be multible of frame size
            frame_byte_cnt = MINIMUM(write_size, read_size);
            frame_byte_cnt = bt_sink_srv_mp3_get_length_by_frame(frame_byte_cnt);
        }

        loop_cnt = 4;
        for (loop_idx = 0; loop_idx < loop_cnt && frame_byte_cnt > 0; loop_idx++) {
            g_file_med_hd->media_handle.mp3.get_write_buffer(mp3_ctx->aud_id, &p_dsp_buf, &write_byte_cnt);
            buff_mgr_get_read_buffer(&p_mcu_buf, &read_byte_cnt);
            //bt_sink_srv_report("[MP3]write drv %d--bt %d mp3 %d frame byte %d %d %d",
            //        loop_idx, buff_mgr_get_data_count(),
            //        g_file_med_hd->media_handle.mp3.get_data_count(mp3_ctx->aud_id),
            //        frame_byte_cnt, read_byte_cnt, write_byte_cnt);
            move_byte_cnt = MINIMUM(write_byte_cnt, read_byte_cnt);
            move_byte_cnt = MINIMUM(move_byte_cnt, frame_byte_cnt);
            /* write data to driver */
            memcpy(p_dsp_buf, p_mcu_buf, move_byte_cnt);

            /* update ring buffer */
            if (move_byte_cnt > 0) {
                buff_mgr_read_data_done(move_byte_cnt);
                g_file_med_hd->media_handle.mp3.write_data_done(mp3_ctx->aud_id, move_byte_cnt);
                frame_byte_cnt -= move_byte_cnt;
            }
        }
        //bt_sink_srv_report("[MP3]write drv--bt %d mp3 %d", buff_mgr_get_data_count(), g_file_med_hd->media_handle.mp3.get_data_count(mp3_ctx->aud_id));
        g_file_med_hd->media_handle.mp3.finish_write_data(mp3_ctx->aud_id);

    } else {
        bt_sink_srv_report("[MP3]write_drv_data(err)--flag: 0x%x", dev->flag);
    }
}

// check if paused and buffer all cleared
static uint8_t bt_sink_srv_mp3_is_stop_and_empty()
{
    uint8_t ret = 0;
    uint32_t len1 = 0, len2 = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    bt_sink_srv_mp3_frame_info_t *finfo;
    bt_sink_srv_music_disable_log();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    bt_sink_srv_music_enable_log();
    mp3_ctx = bt_sink_srv_mp3_get_context();
    finfo = bt_sink_srv_mp3_local_file_info_get_context();

    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_STOPPING) {
        len1 = buff_mgr_get_data_count();
        len2 = g_file_med_hd->media_handle.mp3.get_data_count(mp3_ctx->aud_id);
        //finfo = bt_sink_srv_mp3_local_file_info_get_context();
        bt_sink_srv_report(" total remain data = %d %d %d",
                len1, len2, (finfo->last_frame_len * 2));
        ret = ((len1 + len2) < (finfo->last_frame_len * 2)) ? 0 : 1;
    }

    return ret;
}


static int32_t bt_sink_srv_mp3_open_file(FIL *fp, TCHAR *path)
{
    int32_t res = FR_OK;
    int32_t ret = 0, err = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_mp3_event_t event;

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);

    event.file_event.fp = fp;
    event.file_event.path = path;
    res = bt_sink_srv_mp3_file_callback(BT_SINK_SRV_MP3_EVENT_OPEN, event);
    if (!res) {
        ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_1ST;
        BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_FILE_OPEN);
    } else {
        err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }

    bt_sink_srv_report("[MP3]open_file-ret: %d, err:%d, fs_result: %d", ret, err, res);

    return ret;
}


static FRESULT bt_sink_srv_mp3_lseek_file(FIL *fp, DWORD offset)
{
    int32_t ret = FR_OK;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    bt_sink_srv_mp3_event_t event;

    mp3_ctx = bt_sink_srv_mp3_get_context();
    event.file_event.fp = fp;
    event.file_event.pos = offset;
    event.file_event.offset = &mp3_ctx->offset;

    ret = bt_sink_srv_mp3_file_callback(BT_SINK_SRV_MP3_EVENT_LSEEK, event);
    return (FRESULT)ret;
}


static FRESULT bt_sink_srv_mp3_read_file(FIL *fp, void *buf, uint32_t bytetoread, UINT *byteread)
{
    int32_t ret = FR_OK;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    bt_sink_srv_mp3_event_t event;

    mp3_ctx = bt_sink_srv_mp3_get_context();
    event.file_event.fp = fp;
    event.file_event.buf = buf;
    event.file_event.bytetoread = bytetoread;
    event.file_event.byteread = byteread;
    event.file_event.index = mp3_ctx->file_index;
    event.file_event.offset = &mp3_ctx->offset;
    ret = bt_sink_srv_mp3_file_callback(BT_SINK_SRV_MP3_EVENT_READ, event);

    return (FRESULT)ret;
}


static uint32_t bt_sink_srv_mp3_tell_file(FIL *fp)
{
    uint32_t ret = 0;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    bt_sink_srv_mp3_event_t event;

    mp3_ctx = bt_sink_srv_mp3_get_context();
    event.file_event.fp = fp;
    event.file_event.offset = &mp3_ctx->offset;
    ret = bt_sink_srv_mp3_file_callback(BT_SINK_SRV_MP3_EVENT_TELL, event);
    return ret;
}


static bool bt_sink_srv_mp3_eof_file(FIL *fp)
{
    bool ret = false;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    bt_sink_srv_mp3_event_t event;

    mp3_ctx = bt_sink_srv_mp3_get_context();
    event.file_event.fp = fp;
    event.file_event.index = mp3_ctx->file_index;
    event.file_event.offset = &mp3_ctx->offset;
    ret = bt_sink_srv_mp3_file_callback(BT_SINK_SRV_MP3_EVENT_EOF, event);
    return ret;
}


static uint32_t bt_sink_srv_mp3_size_file(FIL *fp)
{
    uint32_t ret = 0;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    bt_sink_srv_mp3_event_t event;

    mp3_ctx = bt_sink_srv_mp3_get_context();
    event.file_event.fp = fp;
    event.file_event.index = mp3_ctx->file_index;
    ret = bt_sink_srv_mp3_file_callback(BT_SINK_SRV_MP3_EVENT_SIZE, event);
    return ret;
}


static int32_t bt_sink_srv_mp3_set_breakpoint(bt_sink_srv_mp3_breakpoint_data_t *bp_data)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    bt_sink_srv_mp3_event_t event;

    event.bp_data = bp_data;
    ret = bt_sink_srv_mp3_file_callback(BT_SINK_SRV_MP3_EVENT_SET_BREAKPOINT, event);

    return ret;
}


static int32_t bt_sink_srv_mp3_get_breakpoint(bt_sink_srv_mp3_breakpoint_data_t *bp_data)
{
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    bt_sink_srv_mp3_event_t event;

    event.bp_data = bp_data;
    ret = bt_sink_srv_mp3_file_callback(BT_SINK_SRV_MP3_EVENT_GET_BREAKPOINT, event);

    return ret;
}


void bt_sink_srv_mp3_processing(void)
{
    bt_sink_srv_mp3_local_take_semaphore();
    //events = bt_sink_srv_mp3_get_events();
    /* switch events */

    /* read file & write to ring buffer */
    bt_sink_srv_mp3_read_data();
    /* reach playing threshold */
    bt_sink_srv_mp3_check_threshold();
}


static int32_t bt_sink_srv_mp3_read_data(void)
{
    int32_t ret = 0;
    int32_t i = 0, j = 0, ring_cnt = 0;
    int32_t loop_cnt = 0;
    uint32_t write_byte = 0;
    int32_t move_byte = 0;
    int32_t read_byte = 0;
#ifdef __BT_AWS_SUPPORT__
    int32_t remain_byte = 0;
#endif
    uint8_t *ring_buf = NULL;
    int32_t total_byte = 0;
    FRESULT fs_res = FR_OK;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;

    bt_sink_srv_music_disable_log();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    bt_sink_srv_music_enable_log();
    mp3_ctx = bt_sink_srv_mp3_get_context();

    // check if it is not aws source or bt mode, skip it
    if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_SOURCE_AVAILABLE) ||
        (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_STOPPING)
#ifdef __BT_AWS_SUPPORT__
        || (dev->aws_role == BT_AWS_ROLE_SINK)
#endif
        ) {
        //bt_sink_srv_report("read file skip, flag 0x%x", dev->flag);
        return ret;
    }

    if (!bt_sink_srv_mp3_eof_file(&g_aud_fdst)) {
        loop_cnt = BT_SINK_SRV_MP3_FILE_READ_COUNT;
        ring_cnt = 2;
        for (i = 0; (i < loop_cnt) && (!bt_sink_srv_mp3_eof_file(&g_aud_fdst)); ++i) {
            for (j = 0; (j < ring_cnt) && (!bt_sink_srv_mp3_eof_file(&g_aud_fdst)); j++) {
                total_byte = buff_mgr_get_data_count();
                if (total_byte > mp3_ctx->threshold) {
                    //bt_sink_srv_report("dont need to read %d", total_byte);
                    break;
                }
                buff_mgr_get_write_buffer(&ring_buf, &write_byte);
                move_byte = MINIMUM(write_byte, mp3_ctx->max_read_size);
                fs_res = bt_sink_srv_mp3_read_file(&g_aud_fdst, (void *)ring_buf, move_byte, (UINT *)&read_byte);
                // check file operation is OK
                if (fs_res != 0 || !(dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_FILE_OPEN)) {
                    bt_sink_srv_report("[MP3] File operation failed %d", fs_res);
                    buff_mgr_reset_share_buffer();
                    if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK)) {
                        bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
                        bt_sink_srv_music_mp3_state_process();
                    }
                    return -1;
                }
        #ifdef __BT_AWS_SUPPORT__
                if ( mp3_ctx->supportAws ) {
                    // send aws data to aws sink
                    if (read_byte > 0) {
                        remain_byte = bt_sink_srv_mp3_aws_send_streaming_data(ring_buf, read_byte);
                    }
                    if (remain_byte > 0) {
                        bt_sink_srv_report("[MP3]send aws data %d failed", remain_byte);
                    } else if (remain_byte < 0) {
                        bt_sink_srv_report( "[Audply] post check eof for find invalid mp3 header");
                        if (dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN) {
                            // eof change track
                            if (bt_sink_srv_mp3_eof_file(&g_aud_fdst)) {
                                bt_sink_srv_mp3_next_track();
                            // try to play remaining file for id3 in middle case
                            } else {
                                mp3_ctx->offset = bt_sink_srv_mp3_tell_file(&g_aud_fdst);
                                BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK);
                                bt_sink_srv_music_mp3_set_target_and_process(AUDIO_SRC_SRV_STATE_PLAYING);
                            }
                        }
                        ret = -6;
                        return ret;
                    }
                } else if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_LOW_POWER_TEST) {
                    bt_sink_srv_mp3_update_frame_info(&ring_buf, &read_byte);
                }
        #else
                bt_sink_srv_mp3_media_header mp3_hdr;
                bt_sink_srv_mp3_write_aws_header(&mp3_hdr, ring_buf);
                if ((bt_sink_srv_mp3_update_frame_info(&ring_buf, &read_byte) < 0) &&
                    (dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN)) {
                    // eof change track
                    if (bt_sink_srv_mp3_eof_file(&g_aud_fdst)) {
                        bt_sink_srv_mp3_next_track();
                    // try to play remaining file for id3 in middle case
                    } else {
                        mp3_ctx->offset = bt_sink_srv_mp3_tell_file(&g_aud_fdst);
                        BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK);
                        xSemaphoreGive(g_file_hd_semaphore);
                        bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
                        bt_sink_srv_music_mp3_state_process();
                    }
                    ret = -7;
                    return ret;

                }
        #endif
                buff_mgr_write_data_done(read_byte);
            }
        }
    }
    // post check
    if (bt_sink_srv_mp3_eof_file(&g_aud_fdst)) {
        bt_sink_srv_report( "[Audply] post check eof");
        if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_PLAYING) {
            BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_STOPPING);
            bt_sink_srv_mp3_local_change_track(MP3_NEXT_TRACK);
        } else {
            bt_sink_srv_mp3_next_track();
        }
        ret = -5;
    }

    return ret;
}

#ifdef __BT_AWS_SUPPORT__
static void buff_mgr_debug_dump()
{
    uint8_t tmp_buf[8];
    uint32_t tmp_len  = 8;
    buff_mgr_get_read_data(tmp_buf, 0, tmp_len);
    bt_sink_srv_report("buf len %d, data %02x %02x %02x %02x %02x %02x %02x %02x", tmp_len,
            tmp_buf[0],
            tmp_buf[1],
            tmp_buf[2],
            tmp_buf[3],
            tmp_buf[4],
            tmp_buf[5],
            tmp_buf[6],
            tmp_buf[7]);
}
#endif

//extern void bt_a2dp_clear_gpio_12(void);

static void bt_sink_srv_mp3_check_threshold(void)
{
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    int32_t read_byte = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t res;

    mp3_ctx = bt_sink_srv_mp3_get_context();
    bt_sink_srv_music_disable_log();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    bt_sink_srv_music_enable_log();


    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_PLAY) {
        /* ignore */
    } else {
        read_byte = buff_mgr_get_data_count();
        bt_sink_srv_report("check threshold %d, cur %d", mp3_ctx->threshold, read_byte);

        xSemaphoreTake(g_file_hd_semaphore, portMAX_DELAY);
        if (bt_sink_srv_mp3_over_threshold() && g_file_med_hd) {
            // hal_audio_set_stream_out_device(HAL_AUDIO_DEVICE_HEADSET);
            //hal_audio_set_stream_out_volume(0x0, 0xFFFFFB50);
            //bt_a2dp_clear_gpio_12();

        #ifdef __BT_AWS_SUPPORT__
            // for aws, send start time and set flag
            if (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT && mp3_ctx->supportAws) {
                // make sure bt offset already got thus init time can be calculated
                if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_BT_CLK_OFFSET) {
                    bt_sink_srv_report("no bt offset, try it latter");
                    bt_sink_srv_mp3_local_give_semaphore();
                    return;
                }
                g_file_med_hd->media_handle.mp3.aws_init();
                res = g_file_med_hd->media_handle.mp3.set_aws_flag(mp3_ctx->aud_id, true);
                bt_sink_srv_report("src aws set flag %d", res);

                buff_mgr_debug_dump();

                bt_sink_srv_mp3_write_drv_data();
                if ((dev->aws_role == BT_AWS_ROLE_SOURCE) && (bt_sink_srv_mp3_read_data() != 0)) {
                    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK) {
                        buff_mgr_reset_share_buffer();
                        bt_sink_srv_mp3_local_give_semaphore();
                    } else {
                        bt_sink_srv_report("read file failed, try it later");
                        return;
                    }
                }
                if (dev->gap_role == BT_SINK_SRV_GAP_ROLE_MASTER) {
                    res = g_file_med_hd->media_handle.mp3.aws_set_clock_skew(false);
                    bt_sink_srv_report("turn off clock skew res %d", res);
                }
                res = g_file_med_hd->media_handle.mp3.play(mp3_ctx->aud_id);
            // local play
            } else
        #endif
            {
                if ( g_file_med_hd->type == FILE_MP3 ) {
                    bt_sink_srv_mp3_write_drv_data();
                }
                else {
                    bt_sink_srv_wav_write_drv_data();
                }
                if (bt_sink_srv_mp3_read_data() != 0) {
                    bt_sink_srv_report("read file failed, stop play procedure");
                    return;
                }
#ifdef __BT_AWS_SUPPORT__
                if ( mp3_ctx->supportAws ) {
                    g_file_med_hd->media_handle.mp3.aws_init();
                    res = g_file_med_hd->media_handle.mp3.set_aws_flag(mp3_ctx->aud_id, true);
                }
#endif
                bt_sink_srv_report("---play+++\n");
                res = g_file_med_hd->media_handle.mp3.play(mp3_ctx->aud_id);
                bt_sink_srv_report("--play---\n");
            }
            bt_sink_srv_report("play res %d flag 0x%x", res, dev->flag);
            xSemaphoreGive(g_file_hd_semaphore);
            // stop audio if interrupt comes during playing process
            if (MP3_CODEC_RETURN_ERROR == res) {
                bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
            } else {
                bt_sink_srv_ami_audio_set_volume(mp3_ctx->aud_id, mp3_ctx->vol_lev, STREAM_OUT);
                BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK);
                BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_PLAY);
                bt_sink_srv_state_set(BT_SINK_SRV_STATE_LOCAL_PLAYING);
            }
            BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_START_GPT_TIMER);
            bt_sink_srv_music_mp3_state_process();
        } else {
            xSemaphoreGive(g_file_hd_semaphore);
        }
    }
}


/* semaphore */
void bt_sink_srv_mp3_local_take_semaphore(void)
{
    xSemaphoreTake(g_aud_event_semaphore, portMAX_DELAY);
}


void bt_sink_srv_mp3_local_give_semaphore(void)
{
    xSemaphoreGive(g_aud_event_semaphore);
}


/* ring buffer manager function */
static bt_sink_srv_mp3_buffer_t *buff_mgr_get_context(void)
{
    return &g_aud_buff_mgr;
}


static void bt_sink_drv_mp3_audio_src_init(bt_sink_srv_mp3_context_t *ctx)
{
    if (ctx->audio_src == NULL) {
        ctx->audio_src = audio_src_srv_construct_handle(AUDIO_SRC_SRV_PSEUDO_DEVICE_MP3);
        ctx->audio_src->priority = AUDIO_SRC_SRV_PRIORITY_NORMAL;
        ctx->audio_src->play = bt_sink_srv_mp3_local_play;
        ctx->audio_src->stop = bt_sink_srv_mp3_local_stop;
        ctx->audio_src->suspend = bt_sink_srv_mp3_suspend;
        ctx->audio_src->reject = bt_sink_srv_mp3_reject;
        ctx->audio_src->exception_handle = bt_sink_srv_mp3_exception;
    }
}


static void buff_mgr_init(uint8_t *buffer, uint32_t length, bool ex_wr_enable)
{
    g_aud_buff_mgr.buffer_base = buffer;
    g_aud_buff_mgr.buffer_size = length;
    g_aud_buff_mgr.write = 0;
    g_aud_buff_mgr.read = 0;
    g_aud_buff_mgr.write_ex = 0;
    g_aud_buff_mgr.read_ex = 0;
    g_aud_buff_mgr.param = NULL;
    g_aud_buff_mgr.ex_wr_enable = ex_wr_enable;

    // aws
    memset(&g_finfo, 0, sizeof(bt_sink_srv_mp3_frame_info_t));
}


static void buff_mgr_reset_share_buffer(void)
{
    bt_sink_srv_mp3_buffer_t *pbuff_mgr = NULL;

    pbuff_mgr = buff_mgr_get_context();

    pbuff_mgr->write = 0;
    pbuff_mgr->read = 0;
    pbuff_mgr->write_ex = 0;
    pbuff_mgr->read_ex = 0;
    // aws
    bt_sink_srv_report("%s", __func__);
    memset(&g_finfo, 0, sizeof(bt_sink_srv_mp3_frame_info_t));
}


static void buff_mgr_get_write_buffer(uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;
    bt_sink_srv_mp3_buffer_t *pbuff_mgr = NULL;

    pbuff_mgr = buff_mgr_get_context();
    if (pbuff_mgr->ex_wr_enable) {
        ;
    } else {
        if (pbuff_mgr->read > pbuff_mgr->write) {
            count = pbuff_mgr->read - pbuff_mgr->write - 1 /* -1 make sure w < r*/;
        } else if (pbuff_mgr->read == 0) { /* init case */
            count = pbuff_mgr->buffer_size - pbuff_mgr->write - 1;
        } else {
            count = pbuff_mgr->buffer_size - pbuff_mgr->write;
        }

        *buffer = pbuff_mgr->buffer_base + pbuff_mgr->write;
        *length = count;
    }
}


static void buff_mgr_get_read_buffer(uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;
    bt_sink_srv_mp3_buffer_t *pbuff_mgr = NULL;

    pbuff_mgr = buff_mgr_get_context();

    if (pbuff_mgr->ex_wr_enable) {
        ;
    } else {
        if (pbuff_mgr->write >= pbuff_mgr->read) {
            count = pbuff_mgr->write - pbuff_mgr->read;
        } else {
            count = pbuff_mgr->buffer_size - pbuff_mgr->read;
        }
        *buffer = pbuff_mgr->buffer_base + pbuff_mgr->read;
        *length = count;
    }
}


static void buff_mgr_get_read_data(uint8_t *buffer, uint32_t offset,uint32_t length)
{
    bt_sink_srv_mp3_buffer_t *pbuff_mgr = NULL;
    uint32_t i = 0;
    uint32_t pos = 0;

    pbuff_mgr = buff_mgr_get_context();

    if (pbuff_mgr->ex_wr_enable) {
        ;
    } else {
        if (buff_mgr_get_data_count() < length) {
            return;
        }
        for (i = 0; i < length; ++i) {
            pos = (pbuff_mgr->read + offset + i) % pbuff_mgr->buffer_size;
            buffer[i] = pbuff_mgr->buffer_base[pos];
        }
    }
}


static void buff_mgr_write_data_done(uint32_t length)
{
    bt_sink_srv_mp3_buffer_t *pbuff_mgr = NULL;

    pbuff_mgr = buff_mgr_get_context();

    if (pbuff_mgr->ex_wr_enable) {
        ;
    } else {
        pbuff_mgr->write += length;
        if (pbuff_mgr->write == pbuff_mgr->buffer_size) {
            pbuff_mgr->write = 0;
        }
    }
}


static void buff_mgr_read_data_done(uint32_t length)
{
    bt_sink_srv_mp3_buffer_t *pbuff_mgr = NULL;

    pbuff_mgr = buff_mgr_get_context();

    if (pbuff_mgr->ex_wr_enable) {
        ;
    } else {
        pbuff_mgr->read += length;
        if (pbuff_mgr->read == pbuff_mgr->buffer_size) {
            pbuff_mgr->read = 0;
        }
    }
}


static void buff_mgr_move_forward(uint8_t **ptr, uint32_t length)
{
    uint32_t offset = 0;
    bt_sink_srv_mp3_buffer_t *pbuff_mgr = NULL;

    pbuff_mgr = buff_mgr_get_context();

    if (pbuff_mgr->ex_wr_enable) {
        ;
    } else {
        // if ptr is in share buffer move as ring buffer
        if (*ptr >= pbuff_mgr->buffer_base &&
                *ptr < pbuff_mgr->buffer_base + pbuff_mgr->buffer_size) {
            offset = *ptr - pbuff_mgr->buffer_base;
            if (offset < pbuff_mgr->buffer_size) {
                offset = (offset + length) % pbuff_mgr->buffer_size;
                *ptr = pbuff_mgr->buffer_base + offset;
            }
        // else move as liner buffer
        } else {
            *ptr += length;
        }
    }
}


#ifdef __BT_AWS_SUPPORT__
static void buff_mgr_rollback_write_buffer(uint32_t length)
{
    bt_sink_srv_mp3_buffer_t *pbuff_mgr = NULL;

    pbuff_mgr = buff_mgr_get_context();

    if (pbuff_mgr->ex_wr_enable) {
        ;
    } else {
        if (pbuff_mgr->write >= length) {
            pbuff_mgr->write -= length;
        } else {
            pbuff_mgr->write = pbuff_mgr->buffer_size - (length - pbuff_mgr->write);
        }
    }
}
#endif


static int32_t buff_mgr_get_data_count(void)
{
    int32_t count = 0;
    bt_sink_srv_mp3_buffer_t *pbuff_mgr = NULL;

    pbuff_mgr = buff_mgr_get_context();

    count = pbuff_mgr->write - pbuff_mgr->read;
    if (count < 0) {
        count += pbuff_mgr->buffer_size;
    }

    return count;
}


/* frame information manager function */
static bt_sink_srv_mp3_frame_info_t *bt_sink_srv_mp3_local_file_info_get_context(void)
{
    return &g_finfo;
}


static int32_t bt_sink_srv_mp3_write_aws_header(bt_sink_srv_mp3_media_header *header, uint8_t *data)
{
    bt_sink_srv_mp3_frame_info_t *finfo;
    uint8_t temp_hdr[BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN] = {0};
    uint32_t offset = 0;

    finfo = bt_sink_srv_mp3_local_file_info_get_context();

    memset(header, 0x00, sizeof(bt_sink_srv_mp3_media_header));
    header->frame_start_num = finfo->last_frame_sn;
    header->timestamp = finfo->acc_frame_ts;
    // update offset if previous offset is invalid
    if (finfo->frame_start_offset == BT_SINK_SRV_MP3_INVALID_OFFSET &&
            finfo->remain_data_size < BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN) {
        offset = buff_mgr_get_data_count();
        buff_mgr_get_read_data(temp_hdr, offset - finfo->remain_data_size, finfo->remain_data_size);
        memcpy(temp_hdr + finfo->remain_data_size, data,
                BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN - finfo->remain_data_size);
        finfo->last_frame_len = bt_sink_srv_mp3_search_mp3_header(temp_hdr,
            BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN, &offset);
        /* Not a MP3 header */
        if (finfo->last_frame_len == 0) {
            return -1;
        }
        finfo->frame_start_offset = finfo->last_frame_len - finfo->remain_data_size;
        bt_sink_srv_report("Update offset %d frm_len %d", finfo->frame_start_offset, finfo->last_frame_len);
    }
    header->frame_start_offset = finfo->frame_start_offset;
    return 0;
}


#define MP3_FRAME_DEBUG
#ifdef MP3_FRAME_DEBUG
#define MP3_FRAME_DEBUG_PRINTF(message...)    if (g_mp3_frame_log_enable) { bt_sink_srv_report(message); }
#else
#define MP3_FRAME_DEBUG_PRINTF(message...)
#endif
static int32_t bt_sink_srv_mp3_update_frame_info(uint8_t **data, int32_t *data_len)
{
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    bt_sink_srv_mp3_frame_info_t *finfo;
    uint32_t frame_offset = 0;
    uint8_t *ptr = *data;

    bt_sink_srv_music_disable_log();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    bt_sink_srv_music_enable_log();
    mp3_ctx = bt_sink_srv_mp3_get_context();
    finfo = bt_sink_srv_mp3_local_file_info_get_context();
    MP3_FRAME_DEBUG_PRINTF("frame num %d acc count %d data len %d\r\n",
            finfo->last_frame_sn, finfo->acc_frame_ts, *data_len);

    // set frame header & silence frame at 1st frame
    if ((finfo->frame_header == 0) && (dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN)) {
        // move to frame start and update frame caculation information
        if (finfo->frame_start_offset > 0 &&
                (finfo->frame_start_offset <= (*data_len - BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN))) {
            *data += finfo->frame_start_offset;
            *data_len -= finfo->frame_start_offset;
            ptr = *data;
            bt_sink_srv_mp3_search_mp3_header(*data, *data_len, &frame_offset);
            finfo->last_frame_sn++;
            finfo->acc_frame_ts += finfo->sample_count_per_frame;
            finfo->first_frame_ts += finfo->sample_count_per_frame;
            finfo->frame_start_offset = 0;
            finfo->remain_data_size = 0;
        } else if (finfo->frame_start_offset > 0 &&
                (finfo->frame_start_offset > (*data_len - BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN))) {
            bt_sink_srv_report("did not find complete header offset %d len %d", finfo->frame_start_offset, *data_len);
            *data_len = 0;
            return 0;
        }
        finfo->frame_header = *(uint32_t *)(*data) & BT_SINK_SRV_MP3_MP3_HEADER_MASK;
        finfo->last_header_exist = true;
        bt_sink_srv_report("set file header %x data %x", finfo->frame_header, *(uint32_t *)(*data));
        // set silence frame information if it's sink
        mp3_ctx->threshold = bt_sink_srv_mp3_search_mp3_header(*data, *data_len, &frame_offset);
        mp3_ctx->threshold *= BT_SINK_SRV_MP3_FRAME_NUM_THRESHOLD;
        mp3_ctx->threshold = MAXIMUM(mp3_ctx->threshold, BT_SINK_SRV_MP3_BUFFER_THRESHOLD);
    #ifdef __BT_AWS_SUPPORT__
        if (dev->aws_role == BT_AWS_ROLE_SINK && mp3_ctx->supportAws ) {
            silence_frame_information_t silence_info;
            silence_info.frame_size = bt_sink_srv_mp3_search_mp3_header(*data, *data_len, &frame_offset);
            silence_info.frame = *data;
            if (silence_info.frame_size > 1200) {
                bt_sink_srv_report("set silence overflow %d", silence_info.frame_size);
                silence_info.frame_size = 1200;
            }
            g_file_med_hd->media_handle.mp3.set_silence_frame_information(mp3_ctx->aud_id, &silence_info);
            g_file_med_hd->media_handle.mp3.fill_silence_frame(mp3_ctx->aud_id, g_silence_frame);
            g_silence_frame_size = silence_info.frame_size;
        }
    #endif
    }

#ifdef __BT_AWS_SUPPORT__
    // check whether offset is correct
    if (dev->aws_role == BT_AWS_ROLE_SINK && mp3_ctx->supportAws ) {
        if (finfo->frame_start_offset != BT_SINK_SRV_MP3_INVALID_OFFSET &&
                finfo->frame_start_offset <= *data_len) {
            if (!finfo->last_header_exist) {
                frame_offset = finfo->frame_start_offset;
                finfo->last_header_exist = true;
                finfo->remain_data_size = *data_len - frame_offset;
                bt_sink_srv_mp3_fill_silence_frame(1);
                *data += frame_offset;
                ptr = *data;
                MP3_FRAME_DEBUG_PRINTF("5.remain %d offset %d num %d len %d data %x %x %x %x\r\n",
                    finfo->remain_data_size,
                    frame_offset,
                    finfo->last_frame_sn, *data_len,
                    ptr[0], ptr[1], ptr[2], ptr[3]);
                *data_len -= frame_offset;
            } else {
                if (finfo->remain_data_size >= BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN) {
                    frame_offset = finfo->last_frame_len - finfo->remain_data_size;
                } else if (finfo->remain_data_size > 0) {
                    frame_offset = finfo->frame_start_offset;
                } else {
                    frame_offset = 0;
                }
                if (frame_offset == finfo->frame_start_offset) {
                    if (*data_len < frame_offset) {
                        finfo->remain_data_size += *data_len;
                        MP3_FRAME_DEBUG_PRINTF("7.remain %d num %d offset %d\r\n",
                            finfo->remain_data_size,
                            finfo->last_frame_sn,
                            frame_offset);
                        return 0;
                    }
                    finfo->remain_data_size = *data_len - frame_offset;
                    buff_mgr_move_forward(&ptr, frame_offset);
                    if (frame_offset > 0) {
                        finfo->last_frame_sn++;
                        finfo->acc_frame_ts += finfo->sample_count_per_frame;
                    }
                    MP3_FRAME_DEBUG_PRINTF("6.remain %d offset %d num %d data %x %x %x %x %x %x\r\n",
                        finfo->remain_data_size,
                        frame_offset,
                        finfo->last_frame_sn,
                        ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);
                } else {
                    MP3_FRAME_DEBUG_PRINTF("expected offset is not correct %d %d len %d rem %d\r\n",
                            frame_offset, finfo->frame_start_offset,
                            finfo->last_frame_len, finfo->remain_data_size);
                    finfo->last_header_exist = true;
                    buff_mgr_rollback_write_buffer(finfo->remain_data_size);
                    frame_offset = finfo->frame_start_offset;
                    finfo->remain_data_size = *data_len - frame_offset;
                    bt_sink_srv_mp3_fill_silence_frame(1);
                    *data += frame_offset;
                    ptr = *data;
                    *data_len -= frame_offset;
                }
            }
        } else {
            // drop whole packet if no header
            if (!finfo->last_header_exist) {
                bt_sink_srv_report("Can not find MP3 header");
                *data_len = 0;
                return 0;
            } else {
                finfo->remain_data_size += *data_len;
                MP3_FRAME_DEBUG_PRINTF("4.remain %d offset %d\r\n",
                    finfo->remain_data_size,
                    frame_offset);
                return 0;
            }
        }
    } else
#endif
    {
        if (finfo->remain_data_size > 0) {
            frame_offset = finfo->last_frame_len - finfo->remain_data_size;
        } else {
            frame_offset = 0;
        }
        if (frame_offset <= *data_len) {
            finfo->remain_data_size = *data_len - frame_offset;
            buff_mgr_move_forward(&ptr, frame_offset);
            if (frame_offset > 0) {
                finfo->last_frame_sn++;
                finfo->acc_frame_ts += finfo->sample_count_per_frame;
            }
            MP3_FRAME_DEBUG_PRINTF("1.remain %d offset %d num %d data %x %x %x %x %x %x\r\n",
                finfo->remain_data_size,
                frame_offset,
                finfo->last_frame_sn,
                ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);
        // data received not enough one frame
        } else {
            finfo->remain_data_size += *data_len;
            finfo->frame_start_offset = finfo->last_frame_len - finfo->remain_data_size;
            MP3_FRAME_DEBUG_PRINTF("end num %d acc count %d data len %d offset %d\r\n",
                finfo->last_frame_sn, finfo->acc_frame_ts, *data_len, finfo->frame_start_offset);
            return 0;
        }
    }

    // update last frame header
    if(finfo->remain_data_size >= BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN) {
        finfo->last_frame_len = bt_sink_srv_mp3_search_mp3_header(ptr,
                finfo->remain_data_size, &frame_offset);
        /* Not a MP3 header */
        if (finfo->last_frame_len == 0) {
            bt_sink_srv_report("[MP3][WARNING] not a mp3 header");
            return -1;
        }
    }
    // count frame amount in packet
    while (((finfo->last_frame_len + frame_offset) <= finfo->remain_data_size) &&
            (dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN)) {
        finfo->last_frame_sn++;
        finfo->acc_frame_ts += finfo->sample_count_per_frame;
        finfo->remain_data_size -= (finfo->last_frame_len + frame_offset);
        buff_mgr_move_forward(&ptr, finfo->last_frame_len + frame_offset);

        if(finfo->remain_data_size >= BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN) {
            finfo->last_frame_len = bt_sink_srv_mp3_search_mp3_header(ptr,
                finfo->remain_data_size, &frame_offset);
            /* Not a MP3 header */
            if (finfo->last_frame_len == 0) {
                return -2;
            }
        }

        MP3_FRAME_DEBUG_PRINTF("3.remain %d frame %d num %d data %x %x %x %x\r\n",
            finfo->remain_data_size,
            finfo->last_frame_len,
            finfo->last_frame_sn, ptr[0], ptr[1], ptr[2], ptr[3]);
    }

    // src predict next frame start offset if it's not sink
    // it's predict because app can not know the exactly sent data amount in bt
#ifdef __BT_AWS_SUPPORT__
    if (dev->aws_role != BT_AWS_ROLE_SINK && mp3_ctx->supportAws)
#endif
    {
        // common case
        if (finfo->remain_data_size >= BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN) {
            if (finfo->last_frame_len < finfo->remain_data_size) {
                bt_sink_srv_report("[MP3][WARNING] offset parsing failed");
                return -3;
            }
            finfo->frame_start_offset = finfo->last_frame_len - finfo->remain_data_size;
        // exactly to a complete frame
        } else if (finfo->remain_data_size == 0) {
            finfo->frame_start_offset = 0;
        // remaining data not enough one header, can't get next frame length
        // from current information
        } else {
            finfo->frame_start_offset = BT_SINK_SRV_MP3_INVALID_OFFSET;
        }
        MP3_FRAME_DEBUG_PRINTF("end num %d acc count %d data len %d offset %d\r\n",
            finfo->last_frame_sn, finfo->acc_frame_ts, *data_len, finfo->frame_start_offset);
    }
    return 0;
}

#ifdef __BT_AWS_SUPPORT__
static bool bt_sink_srv_mp3_fill_first_frame_info(bt_sink_srv_mp3_media_header *hdr)
{
    bt_sink_srv_mp3_frame_info_t *finfo;
    bt_sink_srv_music_device_t *dev;
    uint8_t *p_mcu_buf = NULL;
    uint32_t read_byte_cnt  = 0;

    finfo = bt_sink_srv_mp3_local_file_info_get_context();
    buff_mgr_get_read_buffer(&p_mcu_buf, &read_byte_cnt);
    bt_sink_srv_music_disable_log();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    bt_sink_srv_music_enable_log();

    if (read_byte_cnt == 0) {
        if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK &&
                hdr->frame_start_num != 0) {
            bt_sink_srv_report("[MP3][WARNING] unexpected packet received %d", hdr->frame_start_num);
            return false;
        }
        finfo->first_frame_sn = hdr->frame_start_num;
        finfo->last_frame_sn = hdr->frame_start_num;
        finfo->first_frame_ts = hdr->timestamp;
        finfo->acc_frame_ts = hdr->timestamp;
    }
    return true;
}

// return false if it needs to skip this packet
static bool bt_sink_srv_mp3_frame_num_check(bt_sink_srv_mp3_media_header *hdr)
{
    bool ret = true;
    int32_t lost_frame_cnt = 0;
    bt_sink_srv_mp3_frame_info_t *finfo;

    finfo = bt_sink_srv_mp3_local_file_info_get_context();
    finfo->frame_start_offset = hdr->frame_start_offset;
    lost_frame_cnt = hdr->frame_start_num - finfo->last_frame_sn;
    // no frame header, no need to check
    if (finfo->frame_header == 0) {
        bt_sink_srv_report("no frame header, no need to check");
        return ret;
    }
    // not continuous, need to fill silence frame
    if (lost_frame_cnt > 0 ||
            finfo->acc_frame_ts < hdr->timestamp) {
        bt_sink_srv_report("frame is not continuous ts %x %x, fill %d silence frame %d %d",
                finfo->acc_frame_ts, hdr->timestamp,
                lost_frame_cnt, finfo->last_frame_sn, hdr->frame_start_num);
        finfo->last_header_exist = false;
        buff_mgr_rollback_write_buffer(finfo->remain_data_size);
        bt_sink_srv_mp3_fill_silence_frame(lost_frame_cnt);
    // prefilled silence frame
    } else if (lost_frame_cnt < 0 ||
        finfo->acc_frame_ts > hdr->timestamp) {
        ret = false;
    }
    return ret;
}

static void bt_sink_srv_mp3_fill_silence_frame(uint8_t num)
{
    bt_sink_srv_mp3_frame_info_t *finfo;
    uint32_t i = 0;
    uint8_t *read_data = NULL;
    uint8_t *ring_buf = NULL;
    uint32_t read_byte = 0, write_byte = 0, move_byte = 0;
    uint32_t attempt = 0;

    finfo = bt_sink_srv_mp3_local_file_info_get_context();

    finfo->last_frame_sn += num;
    finfo->acc_frame_ts += (num * finfo->sample_count_per_frame);
    for (i = 0; i < num; ++i) {
        read_byte = g_silence_frame_size;
        read_data = g_silence_frame;
        while (read_byte > 0) {
            buff_mgr_get_write_buffer(&ring_buf, &write_byte);
            move_byte = MINIMUM(write_byte, read_byte);
            memcpy(ring_buf, read_data, move_byte);
            buff_mgr_write_data_done(move_byte);

            attempt++;
            read_byte -= move_byte;
            read_data += move_byte;
            // write to driver if share buffer is full
            if (write_byte == 0) {
                bt_sink_srv_mp3_write_drv_data();
            }
            if (attempt > 20) {
                bt_sink_srv_report("Write silence failed read_byte %d write_byte %d\r\n", read_byte, write_byte);
                break;
            }
        };
    }
    bt_sink_srv_report("fill %d silence data %d sn %d\r\n", num, buff_mgr_get_data_count(), finfo->last_frame_sn);
}
#endif

// get the length in share buffer that is smaller than total_len but it's
// summary of frame length
// |frm1|frm2|frm3|frm4|
// |----total_len----|
// |----return----|
static uint32_t bt_sink_srv_mp3_get_length_by_frame(uint32_t total_len)
{
    uint8_t buf[BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN];
    uint32_t total_frame_len = 0, current_frame_len = 0;
    uint32_t offset = 0;

    while (total_len > total_frame_len + current_frame_len) {
        buff_mgr_get_read_data(buf, total_frame_len, BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN);
        current_frame_len = bt_sink_srv_mp3_search_mp3_header(buf,
                BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN, &offset);

        if (current_frame_len <= 0) {
            // check whether it's silence
            if (memcmp(g_silence_frame, buf, BT_SINK_SRV_MP3_MP3_FRAME_HEADER_LEN)) {
                current_frame_len = g_silence_frame_size;
            } else {
                bt_sink_srv_report("[MP3]frame length parse failed");
                return 0;
            }
        }
        if (total_frame_len + current_frame_len <= total_len) {
            total_frame_len += current_frame_len;
        }
    }
        //bt_sink_srv_report("frame len to write = %d data %02x %02x %02x %02x",
        //        total_frame_len,
        //        buf[0], buf[1], buf[2], buf[3]);

    return total_frame_len;
}

/* public functions */
extern void bt_sink_srv_mp3_get_local_device_role();
void bt_sink_srv_mp3_init(void)
{
    bt_sink_srv_music_device_t *dev;

    memset(&g_aud_cntx, 0x00, sizeof(bt_sink_srv_mp3_context_t));
    g_aud_cntx.retry_cnt = 0;

    /* default setting */
    bt_sink_srv_mp3_read_nvdm();
#if defined(__AFE_HS_DC_CALIBRATION__)
    g_aud_cntx.dev = (uint8_t)HAL_AUDIO_DEVICE_HANDSET;
#else
    g_aud_cntx.dev = (uint8_t)HAL_AUDIO_DEVICE_HEADSET;
#endif
    g_aud_cntx.offset = 0;
    g_aud_cntx.threshold = BT_SINK_SRV_MP3_BUFFER_THRESHOLD;
    bt_sink_drv_mp3_audio_src_init(&g_aud_cntx);

    /* ring buffer init */
    buff_mgr_init(g_aud_ring_buff, BT_SINK_SRV_MP3_RING_BUFFER_SIZE, false);

    bt_sink_srv_mp3_set_max_read_size(BT_SINK_SRV_MP3_FILE_READ_MAX_SIZE);

    bt_sink_srv_mp3_scan_file();

    g_aud_cntx.aud_id = bt_sink_srv_ami_audio_open(AUD_MIDDLE,
        bt_sink_srv_mp3_ami_hdr);

    if (g_aud_event_semaphore == NULL) {
        g_aud_event_semaphore = xSemaphoreCreateBinary();
    }

    if (g_file_hd_semaphore == NULL) {
        g_file_hd_semaphore = xSemaphoreCreateBinary();
        xSemaphoreGive(g_file_hd_semaphore);
    }
    /* check music device if it's already allocated */
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    if (dev == NULL) {
        /* set music device if it's 1st time init */
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, NULL);
        bt_sink_srv_mp3_get_local_device_role();
        if (dev == NULL) {
            bt_sink_srv_report("mp3 register audio device failed");
            return;
        }
        dev->mp3 = &g_aud_cntx;
    }

    if (dev->mp3->file_total > 0) {
        BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_SOURCE_AVAILABLE);
        bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
        bt_sink_srv_music_mp3_state_process();
    }
    bt_sink_srv_report("mp3 init--aid: %d, sem: 0x%x dev %x", g_aud_cntx.aud_id, g_aud_event_semaphore, dev);
}

void bt_sink_srv_mp3_deinit(void)
{
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    mp3_ctx = bt_sink_srv_mp3_get_context();
    bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_NONE);
    bt_sink_srv_music_mp3_state_process();
    if (mp3_ctx->audio_src != NULL) {
        audio_src_srv_destruct_handle(mp3_ctx->audio_src);
    }
    if (mp3_ctx->aud_id != BT_SINK_SRV_INVALID_AID) {
        bt_sink_srv_ami_audio_close(mp3_ctx->aud_id);
    }
}

void bt_sink_srv_mp3_set_lp(void)
{
    bt_sink_srv_music_device_t *dev = NULL;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_LOW_POWER_TEST);
}

void bt_sink_srv_mp3_set_max_read_size(uint32_t size)
{
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    mp3_ctx = bt_sink_srv_mp3_get_context();
    mp3_ctx->max_read_size = size;
}

int32_t bt_sink_srv_mp3_play()
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK)) {
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_MP3_INTERRUPT);
        bt_sink_srv_music_mp3_set_target_and_process(AUDIO_SRC_SRV_STATE_PLAYING);
    }
    return ret;
}

int32_t bt_sink_srv_mp3_play_process()
{
    bt_sink_srv_report("[MP3] play process\r\n");
    bt_sink_srv_music_mp3_set_target_and_process(AUDIO_SRC_SRV_STATE_PLAYING);
    return 0;
}

int32_t bt_sink_srv_mp3_pause()
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    bt_sink_srv_report("[MP3] bt_sink_srv_mp3_pause, flag = %x", dev->flag);
#ifdef __BT_AWS_SUPPORT__
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    mp3_ctx = bt_sink_srv_mp3_get_context();
    // set flag to send notify if it's aws sink
    if (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT &&
            dev->aws_role == BT_AWS_ROLE_SINK &&
            mp3_ctx->supportAws) {
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_SINK_START);
        bt_sink_srv_aws_mp3_notify_stop(dev->aws_hd);
    } else
#endif
    {
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK);
        bt_sink_srv_music_mp3_set_target_and_process(AUDIO_SRC_SRV_STATE_READY);
        //bt_sink_srv_music_mp3_state_process();
    }

    return ret;
}

void wav_pause_test()
{
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    mp3_ctx = bt_sink_srv_mp3_get_context();

    bt_sink_srv_report("media_handle.mp3.pause\n");
    g_file_med_hd->media_handle.mp3.pause(mp3_ctx->aud_id);
}

void wav_resume_test()
{
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    mp3_ctx = bt_sink_srv_mp3_get_context();

    bt_sink_srv_report("media_handle.mp3.resume\n");
    g_file_med_hd->media_handle.mp3.resume(mp3_ctx->aud_id);
}

int32_t bt_sink_srv_mp3_next_track()
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *dev;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);

    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK) {
        bt_sink_srv_report("[MP3] already changing track");
        return ret;
    }
#ifdef __BT_AWS_SUPPORT__
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    mp3_ctx = bt_sink_srv_mp3_get_context();
    if (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT &&
            dev->aws_role == BT_AWS_ROLE_SINK &&
            mp3_ctx->supportAws) {
        bt_sink_srv_aws_mp3_notify_change_ntrack(dev->aws_hd);
    } else
#endif
    {
        bt_sink_srv_mp3_local_change_track(MP3_NEXT_TRACK);
        bt_sink_srv_music_mp3_set_target_and_process(AUDIO_SRC_SRV_STATE_PLAYING);
        //bt_sink_srv_music_mp3_state_process();
    }

    return ret;
}


int32_t bt_sink_srv_mp3_prev_track()
{
    int32_t ret = 0;
    bt_sink_srv_music_device_t *dev;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);

    if (dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_MP3_CHANGE_TRACK) {
        bt_sink_srv_report("[MP3] already changing track");
        return ret;
    }
#ifdef __BT_AWS_SUPPORT__
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    mp3_ctx = bt_sink_srv_mp3_get_context();
    if (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT &&
            dev->aws_role == BT_AWS_ROLE_SINK &&
            mp3_ctx->supportAws) {
        bt_sink_srv_aws_mp3_notify_change_ptrack(dev->aws_hd);
    } else
#endif
    {
        bt_sink_srv_mp3_local_change_track(MP3_PREVIOUS_TRACK);
        bt_sink_srv_music_mp3_set_target_and_process(AUDIO_SRC_SRV_STATE_PLAYING);
        //bt_sink_srv_music_mp3_state_process();
    }

    return ret;
}


int32_t bt_sink_srv_mp3_volume_up()
{
    int32_t err = 0;
    bt_sink_srv_am_volume_level_out_t vol_lev;

    vol_lev = 
        (bt_sink_srv_am_volume_level_out_t)bt_sink_srv_mp3_local_change_volume((uint8_t)MP3_VOLUME_UP);
#ifdef __BT_AWS_SUPPORT__
    bt_sink_srv_music_device_t *dev;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    mp3_ctx = bt_sink_srv_mp3_get_context();
    // if it has aws link, send volume up notify to remote
    if (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT && mp3_ctx->supportAws) {
        err = bt_sink_srv_aws_mp3_notify_vol_level(dev->aws_hd, vol_lev);
    }
#endif
    bt_sink_srv_report("bt_sink_srv_aws_set_vol_up vol: %d-err: %d", vol_lev, err);
    return err;
}


int32_t bt_sink_srv_mp3_volume_down()
{
    int32_t err = 0;
    bt_sink_srv_am_volume_level_out_t vol_lev;

    vol_lev = 
        (bt_sink_srv_am_volume_level_out_t)bt_sink_srv_mp3_local_change_volume((uint8_t)MP3_VOLUME_DOWN);
#ifdef __BT_AWS_SUPPORT__
    bt_sink_srv_music_device_t *dev;
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    mp3_ctx = bt_sink_srv_mp3_get_context();
    // if it has aws link, send volume down notify to remote
    if (dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT && mp3_ctx->supportAws) {
        err = bt_sink_srv_aws_mp3_notify_vol_level(dev->aws_hd, vol_lev);
    }
#endif
    bt_sink_srv_report("bt_sink_srv_aws_set_vol_down vol: %d-err: %d", vol_lev, err);
    return err;
}


void bt_sink_srv_mp3_read_nvdm()
{
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    mp3_ctx = bt_sink_srv_mp3_get_context();
    uint32_t size = sizeof(mp3_ctx->vol_lev);
    uint8_t temp_vol = 0;
    nvdm_status_t result = nvdm_read_data_item("LOCAL", "volume",
                        (uint8_t *)&temp_vol, &size);
    bt_sink_srv_report("read nvdm result:%d, size:%d, vol:%d", result, size, mp3_ctx->vol_lev);

    if (NVDM_STATUS_OK == result) {
        mp3_ctx->vol_lev = temp_vol;
    } else {
        mp3_ctx->vol_lev = (uint8_t)BT_SINK_SRV_MP3_DEFAULT_VOLUME;
    }
    return;

}


void bt_sink_srv_mp3_write_nvdm()
{
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    mp3_ctx = bt_sink_srv_mp3_get_context();
    nvdm_status_t result =
        nvdm_write_data_item("LOCAL", "volume", NVDM_DATA_ITEM_TYPE_RAW_DATA,
                                            (const uint8_t *)&mp3_ctx->vol_lev,sizeof(mp3_ctx->vol_lev));
    bt_sink_srv_report("write nvdm result:%d, vol:%d", result, mp3_ctx->vol_lev);
    return;
}


int32_t bt_sink_srv_mp3_stop(void)
{
    bt_sink_srv_music_mp3_set_target_state(AUDIO_SRC_SRV_STATE_READY);
    bt_sink_srv_music_mp3_state_process();

    return 0;
}


void bt_sink_srv_mp3_change_frame_log()
{
    g_mp3_frame_log_enable ^= 1;
}

bool bt_sink_srv_mp3_over_threshold()
{
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    int32_t read_byte = 0;
    mp3_ctx = bt_sink_srv_mp3_get_context();
    read_byte = buff_mgr_get_data_count();

    return (read_byte > mp3_ctx->threshold);
}


static void bt_sink_srv_mp3_src_handle_data_req_ind(bt_sink_srv_am_cb_sub_msg_t sub_msg, void *parm)
{
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;
    bt_sink_srv_music_device_t *dev = NULL;
    am_file_event_t *file_event;
    FRESULT fs_res = FR_OK;

    file_event = (am_file_event_t *)parm;
    mp3_ctx = bt_sink_srv_mp3_get_context();
    bt_sink_srv_music_disable_log();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_LOCAL, NULL);
    bt_sink_srv_music_enable_log();
    if (AUD_FILE_EVENT_DATA_REQ == sub_msg) {
    #ifdef __BT_AWS_SUPPORT__
        /*bt_sink_srv_audio_clock_calibrate(NULL, BT_AWS_CODEC_TYPE_MP3,
                g_finfo.sample_rate);*/
    #endif
        if (!(dev->flag & BT_SINK_SRV_MUSIC_FLAG_MP3_PLAYING)) {
            bt_sink_srv_report("[MP3]not playing");
            g_file_med_hd->media_handle.mp3.finish_write_data(mp3_ctx->aud_id);
            bt_sink_srv_mp3_local_give_semaphore();
            return;
        }

        if (bt_sink_srv_mp3_is_stop_and_empty()) {

            g_file_med_hd->media_handle.mp3.flush(mp3_ctx->aud_id, 1);
            bt_sink_srv_report("[MP3]buffer is empty, pause music");
            return;
        }
        /* write driver media data */
        bt_sink_srv_mp3_write_drv_data();

        /* send msg need write ring buffer */
        bt_sink_srv_mp3_local_give_semaphore();
    } else if (AUD_FILE_EVENT_JUMP_INFO == sub_msg) {
        /* To skip the header of MP3 */
        mp3_ctx->offset = (uint32_t)(file_event->event.mp3.param);
        fs_res = bt_sink_srv_mp3_lseek_file(&g_aud_fdst, (DWORD)mp3_ctx->offset);
        LOGI("[MP3]ami_hdr(jump)--offset: %d, res: %d, ptr: 0x%x, fsize(%d)",
                           mp3_ctx->offset, fs_res, bt_sink_srv_mp3_tell_file(&g_aud_fdst), bt_sink_srv_mp3_size_file(&g_aud_fdst));
        /* send msg need write ring buffer */
        bt_sink_srv_mp3_local_give_semaphore();
    } else if (AUD_FILE_EVENT_UNDERFLOW == sub_msg) {
        bt_sink_srv_report("[MP3] underflow");
    } else if (AUD_FILE_MP3_BITSTREAM_END == sub_msg) {
        bt_sink_srv_report("[MP3]end of streming, change next track");
        bt_sink_srv_music_mp3_set_target_and_process(AUDIO_SRC_SRV_STATE_READY);
    }
}


#ifdef __BT_AWS_SUPPORT__
int32_t bt_sink_srv_aws_mp3_handle_streaming_ind(bt_aws_custom_packet_t *packet)
{
    uint8_t *ring_buf = NULL;
    uint8_t *read_data = packet->data;
    uint32_t write_byte = 0;
    uint32_t move_byte = 0;
    int32_t read_byte = packet->data_length;
    uint32_t attempt = 0;
    bt_sink_srv_mp3_media_header *mp3_hdr;

    if (packet->media_header_len == sizeof(bt_sink_srv_mp3_media_header)) {
        mp3_hdr = (bt_sink_srv_mp3_media_header *)packet->media_header;
    } else {
        bt_sink_srv_report("mp3 streaming ind, header length error");
        return BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
    }
    // check frame is continuous and packet length available
    if (!bt_sink_srv_mp3_frame_num_check(mp3_hdr) ||
            (packet->data_length > (BT_SINK_SRV_MP3_RING_BUFFER_SIZE - buff_mgr_get_data_count())) ||
            packet->data_length == 0) {
        return 0;
    }

    // fill first frame info for init sync calculation
    if (!bt_sink_srv_mp3_fill_first_frame_info(mp3_hdr)) {
        return 0;
    }

    bt_sink_srv_mp3_update_frame_info(&read_data, &read_byte);

    while (read_byte > 0) {
        buff_mgr_get_write_buffer(&ring_buf, &write_byte);
        move_byte = MINIMUM(write_byte, read_byte);
        memcpy(ring_buf, read_data, move_byte);
        buff_mgr_write_data_done(move_byte);

        //bt_sink_srv_report("[MP3]cust data ind--move %d remain%d", move_byte, read_byte);
        attempt++;
        read_byte -= move_byte;
        read_data += move_byte;
        // write to driver if share buffer is full
        if (write_byte == 0) {
            bt_sink_srv_mp3_write_drv_data();
        }
        if (attempt > 20) {
            break;
        }
    };

    // debug for wrong state if there is something wrong in aws data receiving
    if (read_byte < 0 || attempt > 20) {
        bt_sink_srv_report("[Audply] read aws data failed %d %d %d", move_byte, write_byte, read_byte);
    }
    bt_sink_srv_mp3_local_give_semaphore();
    return 0;
}


static void bt_sink_srv_aws_mp3_sink_handle_data_req_ind(void)
{
    bt_sink_srv_mp3_context_t *mp3_ctx = NULL;

    mp3_ctx = bt_sink_srv_mp3_get_context();
    bt_sink_srv_mp3_write_drv_data();
    /*bt_sink_srv_audio_clock_calibrate(NULL, BT_AWS_CODEC_TYPE_MP3,
            g_finfo.sample_rate);*/
    if (bt_sink_srv_mp3_is_stop_and_empty()) {
        /* Sink role should not come here */
        g_file_med_hd->media_handle.mp3.flush(mp3_ctx->aud_id, 1);
        bt_sink_srv_report("[MP3][Abnormal]buffer is empty, pause music");
        //bt_sink_srv_music_mp3_set_target_and_process(AUDIO_SRC_SRV_STATE_READY);
    }
    return;
}
#endif //    #ifdef __BT_AWS_SUPPORT__
