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

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal.h"

/* system log includes */
#include "syslog.h"

/* fatfs includes */
#include "ff.h"
#include "ucs2_util.h"
#include "audio_file_manager.h"
#include "bt_sink_srv_mp3.h"
#ifdef MTK_MP3_ON_FLASH_ENABLE
#include "audio_file_sample.h"

static const uint8_t g_file_data_1[AUDIO_FILE_SAMPLE_FILE_SIZE_1] = AUDIO_FILE_SAMPLE_FILE_DATA_1;
static const uint8_t g_file_data_2[AUDIO_FILE_SAMPLE_FILE_SIZE_2] = AUDIO_FILE_SAMPLE_FILE_DATA_2;
static const uint8_t g_file_data_3[AUDIO_FILE_SAMPLE_FILE_SIZE_3] = AUDIO_FILE_SAMPLE_FILE_DATA_3;
static const uint8_t g_file_data_4[AUDIO_FILE_SAMPLE_FILE_SIZE_4] = AUDIO_FILE_SAMPLE_FILE_DATA_4;
static const uint8_t g_file_data_5[AUDIO_FILE_SAMPLE_FILE_SIZE_5] = AUDIO_FILE_SAMPLE_FILE_DATA_5;
static const uint8_t g_file_data_6[AUDIO_FILE_SAMPLE_FILE_SIZE_6] = AUDIO_FILE_SAMPLE_FILE_DATA_6;

static uint8_t const *g_file_data[7] =
    {NULL, g_file_data_1, g_file_data_2, g_file_data_3, g_file_data_4, g_file_data_5, g_file_data_6};
static uint32_t g_file_size[7] =
    {0, AUDIO_FILE_SAMPLE_FILE_SIZE_1,
        AUDIO_FILE_SAMPLE_FILE_SIZE_2,
        AUDIO_FILE_SAMPLE_FILE_SIZE_3,
        AUDIO_FILE_SAMPLE_FILE_SIZE_4,
        AUDIO_FILE_SAMPLE_FILE_SIZE_5,
        AUDIO_FILE_SAMPLE_FILE_SIZE_6};
#endif

//#define DEBUG_USE_PRINT
#define DEBUG_ENABLE

#ifdef DEBUG_ENABLE
#define DEBUG_PRINT(fmt, arg...)    printf(fmt, ##arg)
#else
#define DEBUG_PRINT(fmt, arg...)
#endif

#ifndef DEBUG_USE_PRINT
log_create_module(aud_fmr, PRINT_LEVEL_INFO);

#define LOGI(fmt, arg...)          LOG_I(AudPly, "[AUD_FMR]"fmt, ##arg);
#define LOGW(fmt, arg...)          LOG_W(AudPly, "[AUD_FMR]"fmt, ##arg);
#define LOGE(fmt, arg...)          LOG_E(AudPly, "[AUD_FMR]"fmt, ##arg);

#else
#define LOGI(fmt, arg...)          DEBUG_PRINT("[AUD_FMR]"fmt, ##arg);
#define LOGW(fmt, arg...)          DEBUG_PRINT("[AUD_FMR]"fmt, ##arg);
#define LOGE(fmt, arg...)          DEBUG_PRINT("[AUD_FMR]"fmt, ##arg);
#endif


#define ROOT_PATH           (_T("SD:"))
#define MUSIC_DIR           (_T("MUSIC"))
#define ALBUM_DB            (_T("ALBUM_DB"))
#define AUD_FILE_DB         (_T("FILE_DB"))

typedef int32_t (*cmd_hdlr_t)(void *para1, void *para2, void *para3); 
    
typedef struct {
    char *cmd;
    cmd_hdlr_t cmd_hdlr;
}cmd_item_t;

typedef struct {
    uint16_t album_num;
    album_rec_t *p_album_item;
}album_state_t;

static album_state_t s_album_state;

static TCHAR s_file_path[MAX_FILE_PATH_LEN];

static aud_status_code_t audio_file_manager_load_album_cache();
static aud_status_code_t audio_file_manager_save_album_cache();
static bool audio_file_manager_check_db_ready();
static int32_t audio_file_manager_create_file_db(TCHAR * folder_name);
static void audio_file_manager_dump_album_cache();

static FATFS s_aud_fmr_fatfs; 
static FIL temp_fp;

static SemaphoreHandle_t g_aud_fmr_semaphore;

/* semaphore */
static void audio_file_manager_take_semaphore(void)
{
    LOGI("audio file manager take sem");
    xSemaphoreTake(g_aud_fmr_semaphore, portMAX_DELAY);
}


static void audio_file_manager_give_semaphore(void)
{
    LOGI("audio file manager give sem");
    xSemaphoreGive(g_aud_fmr_semaphore);
}


aud_status_code_t audio_file_manager_init()
{
    FRESULT fres;                     /* fs status infor*/
    aud_status_code_t ret = AUD_STATUS_CODE_SUCCESS;
    
    LOGI("%s enter", __FUNCTION__);
    
    fres = f_mount(&s_aud_fmr_fatfs, _T("0:"), 1);
    if (FR_OK != fres) {
        LOGE("Failed to mount the partition! fres: %d\n\r", fres);
        return AUD_STATUS_CODE_FS_ERROR;
    }

    s_album_state.album_num = 0;
    s_album_state.p_album_item = NULL;
    if (g_aud_fmr_semaphore == NULL) {
        g_aud_fmr_semaphore = xSemaphoreCreateMutex();
    }

    if (!audio_file_manager_check_db_ready()) {
        ret = audio_file_manager_rebuild_db();
    }

    if (ret) {
        LOGE("Failed to create db! ret: %d\n\r", ret);\
        return ret;
    } else {
        ret = audio_file_manager_load_album_cache();
    }

    LOGI("initialize ret: %d", ret);
    return ret;
}


aud_status_code_t audio_file_manager_deinit()
{
    FRESULT fres;                     /* fs status infor*/
    aud_status_code_t ret = AUD_STATUS_CODE_SUCCESS;

    LOGI("%s enter", __FUNCTION__);

    fres = f_mount(NULL, _T("0:"), 1);
    if (FR_OK != fres) {
        LOGE("Failed to mount the partition! fres: %d\n\r", fres);
        return AUD_STATUS_CODE_FS_ERROR;
    }
    /* free old cache buffer */
    s_album_state.album_num = 0;
    if (s_album_state.p_album_item != NULL) {
        vPortFree(s_album_state.p_album_item);
    }
    LOGI("deinitialize ret: %d", ret);
    return ret;
}


void audio_file_manager_dump_album_cache()
{
    int32_t i;
    LOGI("album cache dump start:");
    LOGI("album_num: %u", s_album_state.album_num);
    for (i = 0; i < s_album_state.album_num; i++) {
        LOGI("#%d album, album name: %s, file_num: %u", (int)i+1,
            s_album_state.p_album_item[i].info.album_name,
            s_album_state.p_album_item[i].file_num);
    }
}


aud_status_code_t audio_file_manager_load_album_cache()
{
    LOGI("%s enter", __FUNCTION__);
    
    //FIL fp;
    FRESULT fres;
    UINT read;
    UINT to_read;
    uint16_t album_num = 0;

    memset(s_file_path, 0x00, MAX_FILE_PATH_LEN);
    ucs2_wsprintf_ex(s_file_path, MAX_FILE_PATH_LEN, _T("%s/%s"), ROOT_PATH, ALBUM_DB);
    fres = f_open(&temp_fp, s_file_path, FA_OPEN_EXISTING | FA_READ);
    LOGI("album db path: %s", s_file_path);
    if (fres != FR_OK) {
        LOGE("Failed to open album db, fres: %d", fres);
        return AUD_STATUS_CODE_FS_ERROR;
    }

    /* read album num from db file first */
    fres = f_lseek(&temp_fp, 0);
    if ((fres = f_read(&temp_fp, &album_num, sizeof(uint16_t), &read)) != FR_OK) {
        LOGE("Failed to read album db, fres: %d", fres);
        return AUD_STATUS_CODE_FS_ERROR;
    }
    to_read = album_num * sizeof(album_rec_t);

    /* free old cache buffer */
    if (s_album_state.p_album_item != NULL) {
        vPortFree(s_album_state.p_album_item);
    }

    LOGI("malloc cache buffer size: %u", to_read);
    if (to_read == 0) {
        s_album_state.album_num = album_num;
        s_album_state.p_album_item = NULL;
        f_close(&temp_fp);
        return AUD_STATUS_CODE_SUCCESS;
    }
    s_album_state.p_album_item = (album_rec_t*)pvPortMalloc(to_read);
    if (s_album_state.p_album_item == NULL) {
        LOGE("Failed to alloc cache buffer.");
        return AUD_STATUS_CODE_UNEXPECTED_ERROR;   
    }
    if ((fres = f_read(&temp_fp, s_album_state.p_album_item, to_read, &read)) != FR_OK) {
        LOGE("Failed to read album db, fres: %d", fres);
        return AUD_STATUS_CODE_FS_ERROR;
    }
    if (to_read != read) {
        LOGI("File read incomplete, to_read: %u, read: %u", to_read, read);
        return AUD_STATUS_CODE_FS_ERROR;
    }
    s_album_state.album_num = album_num;

    f_close(&temp_fp);

    audio_file_manager_dump_album_cache();
    return AUD_STATUS_CODE_SUCCESS;
}


aud_status_code_t audio_file_manager_save_album_cache()
{
    LOGI("%s enter", __FUNCTION__);
    
    uint16_t album_num = s_album_state.album_num;
    if (album_num == 0) {
        LOGI("No album to save");
        return AUD_STATUS_CODE_SUCCESS;
    }
    memset(s_file_path, 0x00, MAX_FILE_PATH_LEN);
    ucs2_wsprintf_ex(s_file_path, MAX_FILE_PATH_LEN, _T("%s/%s"), ROOT_PATH, ALBUM_DB);
    //FIL fp;
    FRESULT fres;
    UINT to_write;
    UINT written;
    aud_status_code_t ret = AUD_STATUS_CODE_SUCCESS;
        
    fres = f_open(&temp_fp, s_file_path, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
    if (fres != FR_OK) {
        LOGE("Failed to open album db, fres: %d", fres);
        return AUD_STATUS_CODE_FS_ERROR;
    }

    do {
        if ((fres = f_write(&temp_fp, &album_num, sizeof(uint16_t), &written)) != FR_OK) {
            LOGE("Failed to write file, fres: %d", fres);
            ret = AUD_STATUS_CODE_FS_ERROR;
            break;
        }

        to_write = sizeof(album_rec_t)*album_num;
        if ((fres = f_write(&temp_fp, s_album_state.p_album_item, to_write, &written)) != FR_OK) {
            LOGE("Failed to write file, fres: %d", fres);
            ret = AUD_STATUS_CODE_FS_ERROR;
            break;
        }
        if (to_write != written){
            LOGE("File write incomplete, to_write: %u, written: %u", to_write, written);
            ret = AUD_STATUS_CODE_FS_ERROR;
            break;
        }
    } while(0);

    f_close(&temp_fp);
    return ret;
}


aud_status_code_t audio_file_manager_read_file_db(uint16_t album_idx, uint8_t* buf, uint32_t len, uint32_t offset)
{
    LOGI("%s enter", __FUNCTION__);
    //FIL fp;
    FRESULT fres;
    UINT read;    
    aud_status_code_t ret_val = AUD_STATUS_CODE_SUCCESS;
    
    memset(s_file_path, 0x00, MAX_FILE_PATH_LEN);

    // read album db first
    if (album_idx == 0 || album_idx > MAX_ALBUM_NUM || buf == NULL || len == 0) {
        LOGE("Invalid parameter ");
        return AUD_STATUS_CODE_INVALID_CMD;
    }

    if (album_idx > s_album_state.album_num) {
        LOGE("It's not an available album idx: %u", album_idx);
        return AUD_STATUS_CODE_INVALID_CMD;
    }

    LOGI("start to read file db");
    /* open file db */
    ucs2_wsprintf_ex(s_file_path, MAX_FILE_PATH_LEN, _T("%s/%s/%s"), ROOT_PATH, s_album_state.p_album_item[album_idx-1].info.album_name, AUD_FILE_DB);
    LOGI("file db path:%s", s_file_path);
    fres = f_open(&temp_fp, s_file_path, FA_OPEN_EXISTING | FA_READ);
    if (fres != FR_OK) {
        LOGE("Failed to open file, fres: %d", fres);
        return AUD_STATUS_CODE_FS_ERROR;
    }
    
    do {    
        fres = f_lseek(&temp_fp, offset);
        if ((fres = f_read(&temp_fp, buf, len, &read)) != FR_OK) {
            LOGE("Failed to read file db, fres: %d", fres);
            ret_val = AUD_STATUS_CODE_FS_ERROR;
            break;
        }
        if (read != len) {
            LOGI("File read incomplete, to_read: %u, read: %u", (unsigned int)len, read);
            ret_val = AUD_STATUS_CODE_FS_ERROR;
            break;
        }
    } while(0);
    
    f_close(&temp_fp);

    return ret_val;
}


aud_status_code_t audio_file_manager_create_album_db(TCHAR * folder_name)
{   
    LOGI("%s enter", __FUNCTION__);

    int32_t i;
    int32_t file_num;
    uint16_t album_idx;
    bool is_new = false;
    uint16_t album_num = s_album_state.album_num;
    for (i = 0; i < album_num; i++) {
        if (!ucs2_wcsncmp(folder_name, s_album_state.p_album_item[i].info.album_name, MAX_NAME_LEN - 1)) {
            break;
        }
    }

    album_idx = i + 1;

    if (album_idx > album_num) {
        is_new = true;
        album_num++;
    }
    LOGI("album idx: %u, folder name: %s", album_idx, folder_name);

    file_num = audio_file_manager_create_file_db(folder_name);
    if (file_num < 0) {
        LOGE("Failed to create file db, file_num: %d", file_num);
        return AUD_STATUS_CODE_FS_ERROR;
    }

    if (is_new) {
        s_album_state.p_album_item = pvPortRealloc(s_album_state.p_album_item, sizeof(album_rec_t)*(album_num));
        if (s_album_state.p_album_item) {
            s_album_state.p_album_item[album_idx-1].file_num = file_num;
            s_album_state.p_album_item[album_idx-1].info.album_idx = album_idx;
            ucs2_wcscpy(s_album_state.p_album_item[album_idx-1].info.album_name, folder_name);
        } else {
            LOGE("Failed to allocate heap to cache album info");
            return AUD_STATUS_CODE_UNEXPECTED_ERROR;
        }
        s_album_state.album_num = album_num;
    } else {
        s_album_state.p_album_item[album_idx-1].file_num = file_num;
    }
     
    audio_file_manager_save_album_cache();
    return AUD_STATUS_CODE_SUCCESS;
}


int32_t audio_file_manager_create_file_db(TCHAR * folder_name)
{
    LOGI("%s enter", __FUNCTION__);
    
    FRESULT fres;
    //FIL fp;

    memset(s_file_path, 0x00, MAX_FILE_PATH_LEN);
    ucs2_wsprintf_ex(s_file_path, MAX_FILE_PATH_LEN, _T("%s/%s/%s"), ROOT_PATH, folder_name, AUD_FILE_DB);
    LOGI("file_path: %s", s_file_path);
    fres = f_open(&temp_fp, s_file_path, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    if (fres != FR_OK) {
        LOGE("Failed to open audio db file, fres: %d", fres);
        return -1;
    }

    int32_t file_num = 0;
    uint16_t idx = 1;
    DIR dir;
    FILINFO fno;
    uint32_t file_name_len;

    LOGI("start to scan file:");

    memset(s_file_path, 0x00, MAX_FILE_PATH_LEN);
    ucs2_wsprintf_ex(s_file_path, MAX_FILE_PATH_LEN, _T("%s/%s"), ROOT_PATH, folder_name);
    if ((fres = f_opendir(&dir, s_file_path)) != FR_OK) {
        LOGE("Failed to open directory, fres: %d", fres);
        return -2;
    }
    
    while ((fres = f_readdir(&dir, &fno)) == FR_OK) {
        if (fno.fname[0] == 0)  /* end of directory */
            break;
        if (fno.fattrib & AM_DIR) /* skip directory */
            continue;

        file_name_len = ucs2_wcslen(fno.fname);

        if (file_name_len > MAX_NAME_LEN - 1) {
            LOGI("skip file with long name");
            continue;
        }

        if (!ucs2_wcsncmp(fno.fname, AUD_FILE_DB, file_name_len)
            || (!ucs2_wcsncmp(fno.fname, _T("BP_DATA"), file_name_len))) {
            LOGI("skip control file");
            continue;
        }

        LOGI("scanned file name:%s, file_name_len: %u", fno.fname, file_name_len);
        aud_file_rec_t rec;
        UINT written;
        memset(&rec, 0x00, sizeof(aud_file_rec_t));
        
        rec.file_idx = idx;
        memcpy(rec.file_name, fno.fname, ucs2_wcslen(fno.fname));
        LOGI("scanned file name2: %s, len: %s", rec.file_name, strlen(rec.file_name));
        fres = f_write(&temp_fp, &rec, sizeof(aud_file_rec_t), &written);
        if (fres != FR_OK || written != sizeof(aud_file_rec_t)) {
            LOGE("Failed to write file db, fres: %d", fres);
            f_closedir(&dir);
            f_close(&temp_fp);
            return -3;
        }

        idx++;
        file_num++;
    }
    LOGI("end to scan file, file_num: %d", file_num);
    
    f_closedir(&dir);
    f_close(&temp_fp);
    
    return file_num;
}


bool audio_file_manager_check_db_ready()
{
    //FIL fp;
    FRESULT fres;

    memset(s_file_path, 0x00, MAX_FILE_PATH_LEN);
    ucs2_wsprintf_ex(s_file_path, MAX_FILE_PATH_LEN, _T("%s/%s"), ROOT_PATH, ALBUM_DB);
    fres = f_open(&temp_fp, s_file_path, FA_OPEN_EXISTING);
    if (fres == FR_OK) {
        f_close(&temp_fp);
        return true;
    } else {
        return false;
    }
}


aud_status_code_t audio_file_manager_rebuild_db()
{
    LOGI("%s enter", __FUNCTION__);
    
    FIL fp;
    FRESULT fres;
    int32_t file_num;
    uint16_t album_num = 0;
    UINT written;
    aud_status_code_t ret = AUD_STATUS_CODE_SUCCESS;

    memset(s_file_path, 0x00, MAX_FILE_PATH_LEN);
    ucs2_wsprintf_ex(s_file_path, MAX_FILE_PATH_LEN, _T("%s/%s"), ROOT_PATH, ALBUM_DB);
    fres = f_open(&fp, s_file_path, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    LOGI("Album db path: %s", s_file_path);
    if (fres != FR_OK) {
        LOGE("Failed to create album db file, fres: %d", fres);
        return AUD_STATUS_CODE_FS_ERROR;
    }

#if 1
    if ((fres = f_lseek(&fp, sizeof(uint16_t))) != FR_OK) {
        LOGE("Failed to seek file pointer, fres: %d", fres);
        return AUD_STATUS_CODE_FS_ERROR;
    }
#else
    if ((fres = f_write(&fp, &album_num, sizeof(uint16_t), &written)) != FR_OK) {
        LOGE("Failed to write file, fres: %d", fres);
        f_close(&fp);
        return AUD_STATUS_CODE_FS_ERROR;
    }
#endif

    int32_t idx = 1;
    LOGI("start to search directory:");
    DIR dir;
    FILINFO fno;
    if ((fres = f_opendir(&dir, ROOT_PATH)) != FR_OK) {
        LOGE("Failed to open music root folder, fres: %d", fres);
        f_close(&fp);
        return AUD_STATUS_CODE_FS_ERROR;
    }

    audio_file_manager_take_semaphore();
    
    album_rec_t rec;    
    while ((fres = f_readdir(&dir, &fno)) == FR_OK) {
        if (fno.fname[0] == 0) {
            LOGI("end of the music folder");
            break;
        }

        if (strcmp(fno.fname, MUSIC_DIR) != 0) {
            //LOGI("dir name: %s, attr: 0x%x", fno.fname, fno.fattrib);
            continue;
        }

        LOGI("dir name: %s, attr: 0x%x", fno.fname, fno.fattrib);
        if (fno.fattrib & AM_DIR) {
            memset(&rec, 0x00, sizeof(album_rec_t)); 
            LOGI("folder name: %s", fno.fname);
            rec.info.album_idx = idx;
            memcpy(rec.info.album_name, fno.fname, ucs2_wcslen(fno.fname) > MAX_NAME_LEN-1 ? (MAX_NAME_LEN-1) : ucs2_wcslen(fno.fname));

            file_num = audio_file_manager_create_file_db(fno.fname);
            file_num = file_num >= 0 ? file_num : 0;
            rec.file_num = file_num;
            
            if ((fres = f_write(&fp, &rec, sizeof(album_rec_t), &written)) != FR_OK) {
                LOGE("Failed to write album db file, fres: %d.", fres);
                ret = AUD_STATUS_CODE_FS_ERROR;
                break;
            }

            album_num++;
            idx++;
            break;
        }
    }

    do {
        if (fres != FR_OK) {
            LOGE("Fail to read directory, fres: %d", fres);
            ret = AUD_STATUS_CODE_FS_ERROR;
            break;
        }
        LOGI("end to search directory");
        /* move to head of db file */
        fres = f_lseek(&fp, 0);
        if ((fres = f_write(&fp, &album_num, sizeof(uint16_t), &written)) != FR_OK) {
            LOGE("Failed to write file, fres: %d", fres);
            ret = AUD_STATUS_CODE_FS_ERROR;
            break;
        }
    } while(0);

    audio_file_manager_give_semaphore();

    f_closedir(&dir);
    f_close(&fp);

    return ret;
}


aud_status_code_t audio_file_manager_delete_album_by_idx(uint16_t album_idx)
{
    LOGI("%s enter", __FUNCTION__);
    aud_status_code_t ret = AUD_STATUS_CODE_SUCCESS;
        
    if (album_idx == 0 ) {
        LOGE("Invalid parameter.");
        return AUD_STATUS_CODE_INVALID_CMD;
    }

    uint16_t album_num = s_album_state.album_num;
    if (album_idx > album_num) {
        LOGE("album idx is not available");
        return AUD_STATUS_CODE_FOLDER_NOT_EXIST;
    }

    int32_t i = album_idx-1;
    album_rec_t *p = s_album_state.p_album_item;
    memset(s_file_path, 0x00, MAX_FILE_PATH_LEN);
    TCHAR album_path[MAX_FILE_PATH_LEN] = {0};
    
    ucs2_wsprintf_ex(album_path, MAX_FILE_PATH_LEN, _T("%s/%s"),ROOT_PATH, p[i].info.album_name);
    LOGI("To be deleted directory:%s ", album_path);

    /* delete all files in sub directory first */
    DIR dir;
    FILINFO fno;
    FRESULT fres;
    if ((fres = f_opendir(&dir, album_path)) != FR_OK) {
        LOGE("Failed to open directory, fres: %d", fres);
        return AUD_STATUS_CODE_FS_ERROR;
    }

    audio_file_manager_take_semaphore();
    while ((fres = f_readdir(&dir, &fno)) == FR_OK) {
        if(fno.fname[0] == 0) /* end of directory */
            break;

        ucs2_wsprintf_ex(s_file_path, MAX_FILE_PATH_LEN, _T("%s/%s"), album_path, fno.fname);
        LOGI("Delete file: %s", s_file_path);
        if ((fres = f_unlink(s_file_path)) != FR_OK) {
            LOGE("Failed to remove file in sub-directory, fres: %d", fres);
            audio_file_manager_give_semaphore();
            f_closedir(&dir);
            return AUD_STATUS_CODE_FS_ERROR;
        }
    }

    f_closedir(&dir);

    do {
        if (fres != FR_OK) {
            LOGE("Failed to read directory, fres: %d", fres);
            ret = AUD_STATUS_CODE_FS_ERROR;
            break;
        }

        if ((fres = f_unlink(album_path)) != FR_OK) {
            LOGE("Failed to delete empty directory, fres: %d", fres);
            ret =  AUD_STATUS_CODE_FS_ERROR;
        }
        
        /* Shift array to fill the blank element */
        for (; i < album_num - 1; i++) {
            memcpy(&p[i], &p[i+1], sizeof(album_rec_t)); 
            p[i].info.album_idx = i + 1;
        }
        /* Remove the last album rec in cache */
        memset(&p[i], 0x00, sizeof(album_rec_t));
        s_album_state.album_num--;

        if ((ret = audio_file_manager_save_album_cache()) != AUD_STATUS_CODE_SUCCESS) {
            LOGE("Failed to save album cache, ret: %d", ret);
            break;;
        }
    }while(0);

    audio_file_manager_give_semaphore();
    return ret;    
}

aud_status_code_t audio_file_manager_delete_file_by_idx(uint16_t album_idx, uint16_t file_idx)
{
    LOGI("%s enter", __FUNCTION__);
    if (album_idx == 0 || file_idx == 0) {
        LOGI("Invalid parameter.");
        return AUD_STATUS_CODE_INVALID_CMD;
    }
    
    if (album_idx > s_album_state.album_num) {
        LOGE("album idx is not avaliable.");
        return AUD_STATUS_CODE_FOLDER_NOT_EXIST;
    } 

    if (file_idx > s_album_state.p_album_item[album_idx-1].file_num) {
        LOGE("file idx is not avaliable.");
        LOGE("file idx: %u, real file num: %u.", file_idx, s_album_state.p_album_item[album_idx].file_num);
        return AUD_STATUS_CODE_FILE_NOT_EXIST;
    }

    FRESULT fres;
    aud_file_rec_t rec;
    audio_file_manager_read_file_db(album_idx, (uint8_t*)&rec, sizeof(aud_file_rec_t), (file_idx-1)*sizeof(aud_file_rec_t));
    memset(s_file_path, 0x00, MAX_FILE_PATH_LEN);
    ucs2_wsprintf_ex(s_file_path, MAX_FILE_PATH_LEN, _T("%s/%s/%s"), ROOT_PATH, s_album_state.p_album_item[album_idx-1].info.album_name, rec.file_name);
    LOGI("delete file path:%s", s_file_path);

    aud_status_code_t ret = AUD_STATUS_CODE_SUCCESS;
    audio_file_manager_take_semaphore();
    do {
        fres = f_unlink(s_file_path);
        if (fres != FR_OK) {
            LOGE("Failed to delete file, fres: %d", fres);
            ret = AUD_STATUS_CODE_FS_ERROR;
            break;
        }

        int32_t file_num;
        file_num = audio_file_manager_create_file_db(s_album_state.p_album_item[album_idx-1].info.album_name);
        s_album_state.p_album_item[album_idx-1].file_num = file_num;

        if (audio_file_manager_save_album_cache()) {
            LOGE("Failed to save album cache");
            ret = AUD_STATUS_CODE_FS_ERROR;
            break;
        }
    } while(0);

    audio_file_manager_give_semaphore();
    return ret;
}

aud_status_code_t audio_file_manager_get_file_num_by_idx(uint16_t* file_num, uint16_t album_idx)
{
    LOGI("%s enter", __FUNCTION__);
    if (album_idx == 0 || album_idx > MAX_ALBUM_NUM || file_num == NULL) {
        LOGE("Invalid album idx.");
        return AUD_STATUS_CODE_INVALID_CMD;
    }

    if (album_idx > s_album_state.album_num) {
        LOGE("album idx is not avaliable.");
        return AUD_STATUS_CODE_FOLDER_NOT_EXIST;
    }

    *file_num = s_album_state.p_album_item[album_idx-1].file_num;
    return AUD_STATUS_CODE_SUCCESS;
}


aud_status_code_t audio_file_manager_get_file_path_by_idx(TCHAR *file_path, uint32_t buf_len, uint16_t album_idx, uint16_t file_idx)
{
    LOGI("%s enter", __FUNCTION__);
    LOGI("album_idx: %u, file_idx: %u", album_idx, file_idx);
    if (file_idx > MAX_FILE_NUM || album_idx > MAX_ALBUM_NUM) {
        LOGE("idx exceed the limit.");
        return AUD_STATUS_CODE_INVALID_CMD;
    }

    if (file_path == NULL || buf_len == 0 || album_idx == 0 || file_idx == 0) {
        LOGE("Invalid parameter.");
        return AUD_STATUS_CODE_INVALID_CMD;
    }

    if (album_idx > s_album_state.album_num) {
        LOGE("album idx is not avaliable.");
        return AUD_STATUS_CODE_FOLDER_NOT_EXIST;
    } 

    if (file_idx > s_album_state.p_album_item[album_idx-1].file_num) {
        LOGE("file idx is not avaliable.");
        LOGE("file idx: %u, real file num: %u.", file_idx, s_album_state.p_album_item[album_idx-1].file_num);
        return AUD_STATUS_CODE_FILE_NOT_EXIST;
    }
    
    aud_file_rec_t file_rec;
    uint32_t offset = sizeof(aud_file_rec_t)*(file_idx - 1);
    audio_file_manager_read_file_db(album_idx, (uint8_t*)&file_rec, sizeof(aud_file_rec_t), offset); 
    if (file_rec.file_idx != file_idx) {
        LOGE("File db corruption!");
        return AUD_STATUS_CODE_UNEXPECTED_ERROR;
    }

    ucs2_wsprintf_ex(file_path, buf_len, _T("%s/%s/%s"), ROOT_PATH, s_album_state.p_album_item[album_idx-1].info.album_name, file_rec.file_name);
    return AUD_STATUS_CODE_SUCCESS;
}



aud_status_code_t audio_file_manager_get_file_list_by_album(aud_file_rec_t *file_list, uint32_t buf_len, uint16_t album_idx, uint16_t file_start_idx, uint32_t *file_num)
{
    LOGI("%s enter", __FUNCTION__);
    if (file_list == NULL || buf_len == 0 || file_num == NULL || album_idx > MAX_ALBUM_NUM || file_start_idx == 0) {
        LOGI("Invalid parameter");
        return AUD_STATUS_CODE_INVALID_CMD;
    }

    if (album_idx > s_album_state.album_num) {
        LOGE("album_idx %u is not available", (unsigned int)album_idx);
        return AUD_STATUS_CODE_FOLDER_NOT_EXIST;
    }

    if (file_start_idx > s_album_state.p_album_item[album_idx-1].file_num) {
        LOGE("file_start_idx %u is not available", (unsigned int)file_start_idx);
        return AUD_STATUS_CODE_FILE_NOT_EXIST;
    }
    
    uint32_t to_read = buf_len / sizeof(aud_file_rec_t);
    if (to_read == 0) {
        LOGE("Not enought memory to save file list");
        return AUD_STATUS_CODE_UNEXPECTED_ERROR;
    }
    
    uint16_t available_num = s_album_state.p_album_item[album_idx-1].file_num - file_start_idx + 1;
    to_read = to_read > *file_num ? *file_num : to_read;
    to_read = to_read > available_num ? available_num : to_read;
    
    LOGI("%s, to_read: %u", __FUNCTION__, (unsigned int)to_read);
    uint32_t offset = (file_start_idx - 1)*sizeof(aud_file_rec_t);
    aud_status_code_t ret;
    if ((ret = audio_file_manager_read_file_db(album_idx, (uint8_t*)file_list, to_read*sizeof(aud_file_rec_t), offset)) != AUD_STATUS_CODE_SUCCESS){
        LOGE("Fail to read file db, ret: %d", ret);
        return ret;
    }

    *file_num = to_read;
    
    return AUD_STATUS_CODE_SUCCESS;
}


aud_status_code_t audio_file_manager_get_album_name_by_idx(TCHAR *album_name, uint32_t buf_len, uint16_t album_idx)
{
    LOGI("%s enter", __FUNCTION__);
    
    if (album_idx > s_album_state.album_num || album_idx == 0) {
        LOGE("It's not an available album idx: %u", album_idx);
        return AUD_STATUS_CODE_FOLDER_NOT_EXIST;
    }

    memcpy(album_name, s_album_state.p_album_item[album_idx-1].info.album_name, buf_len);
    return AUD_STATUS_CODE_SUCCESS;
}


aud_status_code_t audio_file_manager_get_album_num(uint16_t *album_num)
{
    LOGI("%s enter", __FUNCTION__);
    if (album_num == NULL) {
        LOGI("Invalid parameter");
        return AUD_STATUS_CODE_UNEXPECTED_ERROR;
    }

    *album_num = s_album_state.album_num;
    return AUD_STATUS_CODE_SUCCESS;
}


aud_status_code_t audio_file_manager_get_album_path_by_idx(TCHAR *album_path, uint32_t buf_len, uint16_t album_idx)
{
    LOGI("%s enter", __FUNCTION__);
    if (album_path == NULL || buf_len == 0 || album_idx == 0) {
        LOGE("Invalid parameter");
        return AUD_STATUS_CODE_INVALID_CMD;
    }

    if (album_idx > s_album_state.album_num) {
        LOGW("This album index is not available");
        return AUD_STATUS_CODE_FOLDER_NOT_EXIST;
    }

    ucs2_wsprintf_ex(album_path, buf_len, _T("%s/%s"), ROOT_PATH, s_album_state.p_album_item[album_idx-1].info.album_name);

    return AUD_STATUS_CODE_SUCCESS;
}


aud_status_code_t audio_file_manager_get_album_list(album_rec_t *album_info_list, uint32_t buf_len, uint16_t album_start_idx, uint32_t *album_num)
{
    LOGI("%s enter", __FUNCTION__);
    
    if (album_info_list == NULL || album_num == NULL || buf_len == 0 || album_start_idx == 0) {
        LOGE("Invalid parameter");
        return AUD_STATUS_CODE_INVALID_CMD;
    }

    if (album_start_idx > s_album_state.album_num) {
        LOGE("start idx is not available, exceed the number of album.");
        return AUD_STATUS_CODE_FOLDER_NOT_EXIST;
    }

    uint16_t available_num;
    uint32_t to_read = buf_len / sizeof(album_rec_t);   
    
    if (to_read == 0) {
        LOGE("Not enough memory to save album info.");
        return AUD_STATUS_CODE_UNEXPECTED_ERROR;
    }

    available_num = s_album_state.album_num - album_start_idx + 1;
    to_read = to_read > *album_num ? *album_num : to_read; 
    to_read = to_read > available_num ? available_num : to_read;
    LOGI("%s, to_read: %u", __FUNCTION__, (unsigned int)to_read);

    memcpy(album_info_list, &s_album_state.p_album_item[album_start_idx-1], sizeof(album_rec_t)*to_read);

    *album_num = to_read;
    
    return AUD_STATUS_CODE_SUCCESS;
}

FRESULT audio_file_manager_open(FIL *fp, TCHAR *path)
{
    FRESULT fs_res = FR_OK;
#ifndef MTK_MP3_ON_FLASH_ENABLE
    fs_res = f_open(fp, path, FA_OPEN_EXISTING | FA_WRITE | FA_READ);
#endif
    return fs_res;
}

FRESULT audio_file_manager_lseek(FIL *fp, DWORD pos, uint32_t *offset)
{
    FRESULT fs_res = FR_OK;
#ifndef MTK_MP3_ON_FLASH_ENABLE
    fs_res = f_lseek(fp, pos);
#else
    *offset = pos;
#endif
    return fs_res;
}

FRESULT audio_file_manager_read(FIL *fp, void *buf, uint32_t bytetoread, UINT *byteread, uint32_t index, uint32_t *offset)
{
    FRESULT fs_res = FR_OK;
#ifndef MTK_MP3_ON_FLASH_ENABLE
    fs_res = f_read(fp, buf, bytetoread, byteread);
#else
    int32_t move_byte = 0;
    if (g_file_size[index] >= *offset) {
        move_byte = MINIMUM(bytetoread, g_file_size[index] - *offset);
    } else {
        move_byte = 0;
    }
    memcpy(buf, &g_file_data[index][*offset], move_byte);
    *offset += move_byte;
    *byteread = (UINT)move_byte;
#endif
    //LOGI("%s %d", __func__, *byteread);
    return fs_res;
}

uint32_t audio_file_manager_tell(FIL *fp, uint32_t *offset)
{
    uint32_t fs_res = 0;
#ifndef MTK_MP3_ON_FLASH_ENABLE
    fs_res = f_tell(fp);
#else
    fs_res = *offset;
#endif
    return fs_res;
}

bool audio_file_manager_eof(FIL *fp, uint32_t index, uint32_t *offset)
{
    bool fs_res = false;
#ifndef MTK_MP3_ON_FLASH_ENABLE
    fs_res = f_eof(fp);
#else
    fs_res = (g_file_size[index] <= *offset);
#endif

    return fs_res;
}

uint32_t audio_file_manager_size(FIL *fp, uint32_t index)
{
    uint32_t fs_res = 0;
#ifndef MTK_MP3_ON_FLASH_ENABLE
    fs_res = f_size(fp);
#else
    fs_res = g_file_size[index];
#endif
    return fs_res;
}

#ifndef MTK_MP3_ON_FLASH_ENABLE
static FRESULT audio_file_manager_set_breakpoint_file_by_folder(bt_sink_srv_mp3_breakpoint_data_t *bp_data)
{
    FRESULT res = FR_OK;
    int32_t file_ret;
    //FIL fp;

    TCHAR album_name[MAX_FILE_PATH_LEN] = {0};
    TCHAR breakpoint_path[MAX_FILE_PATH_LEN] = {0};

   // printf("update bp file\r\n");

    file_ret = audio_file_manager_get_album_path_by_idx(album_name, MAX_FILE_PATH_LEN, bp_data->folder_id);
    if (file_ret < 0) {
        LOGE("get folder path failed\n");
        return FR_NO_PATH;
    }

    ucs2_wsprintf_ex(breakpoint_path, MAX_FILE_PATH_LEN, _T("%s/%s"), album_name, AUDIO_FILE_MANAGER_BREAKPOINT_FILE_NAME);
    //vsnwpritf(breakpoint_path, MAX_FILE_PATH_LEN, "%s/%s", album_name, AUDIO_FILE_MANAGER_BREAKPOINT_FILE_NAME);
    //debug_ucs2_string_dump(breakpoint_path, ucs2_wcslen(breakpoint_path)*2);

    res = f_open(&temp_fp, breakpoint_path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    if (res == FR_OK) {
        UINT write_p;
        //uint8_t *p;
        res = f_write(&temp_fp, bp_data, sizeof(bt_sink_srv_mp3_breakpoint_data_t), &write_p);
        //p = (uint8_t *)bp_data;

        //LOGI(AudPly, "write_p %d, size %d, DUmp %x, %x, %x, %x, %x, %x, %x, %x", write_p, sizeof(bt_sink_srv_mp3_breakpoint_data_t),
        //     *(uint8_t *)(p+ 0), *(uint8_t *)(p+ 1),
        //     *(uint8_t *)(p+ 2),*(uint8_t *)(p+ 3),
        //     *(uint8_t *)(p+ 4),*(uint8_t *)(p+ 5),
        //     *(uint8_t *)(p+ 6),*(uint8_t *)(p+ 7));
        LOGI("update bp file:%d", res);
        f_close(&temp_fp);

    }
    return res;
}

static FRESULT audio_file_manager_get_breakpoint_file_by_folder(bt_sink_srv_mp3_breakpoint_data_t *bp_data)
{
    FRESULT res = FR_OK;
    int32_t file_ret;
    //FIL fp;

    uint8_t share_buf[AUDIO_FILE_MANAGER_FOLDER_BREAK_POINT_LEN] = {0};
    TCHAR album_name[MAX_FILE_PATH_LEN] = {0};
    TCHAR breakpoint_path[MAX_FILE_PATH_LEN] = {0};
    UINT read_len = 0;

    file_ret = audio_file_manager_get_album_path_by_idx(album_name, MAX_FILE_PATH_LEN, bp_data->folder_id);

    //printf("album_name:\n");
    //debug_ucs2_string_dump(album_name, ucs2_wcslen(album_name)*2);
    if (file_ret < 0) {
        LOGE("get folder path failed");
        return FR_NO_PATH;
    }

    ucs2_wsprintf_ex(breakpoint_path, MAX_FILE_PATH_LEN, _T("%s/%s"), album_name, AUDIO_FILE_MANAGER_BREAKPOINT_FILE_NAME);
    //printf("file_name:\n");
    //debug_ucs2_string_dump(breakpoint_path, ucs2_wcslen(breakpoint_path)*2);
    //vsnwpritf(breakpoint_path, MAX_FILE_PATH_LEN, "%s/%s", album_name, AUDIO_FILE_MANAGER_BREAKPOINT_FILE_NAME);

    res = f_open(&temp_fp, breakpoint_path, FA_OPEN_EXISTING | FA_READ | FA_WRITE);
    if (res == FR_OK) {
        res = f_read(&temp_fp, share_buf, AUDIO_FILE_MANAGER_FOLDER_BREAK_POINT_LEN, &read_len);
        if (read_len == AUDIO_FILE_MANAGER_FOLDER_BREAK_POINT_LEN) {
            memcpy(bp_data, share_buf, AUDIO_FILE_MANAGER_FOLDER_BREAK_POINT_LEN);
            LOGI("BP read folder id %d file id %d, offset %d", 
                    bp_data->folder_id, bp_data->file_id, bp_data->offset);
        }
        LOGE("get breakpoint file len %d", read_len);

        f_close(&temp_fp);
    } else {
        bp_data->folder_id = AUDIO_FILE_MANAGER_DEFAULT_FOLDER_ID;
        bp_data->file_id = AUDIO_FILE_MANGER_DEFAULT_FILE_ID;
        bp_data->offset = 0;
        res = f_open(&temp_fp, breakpoint_path, FA_OPEN_ALWAYS | FA_WRITE);

        if (res == FR_OK) {
            UINT write_p;
            f_write(&temp_fp,(void *)bp_data, sizeof(bt_sink_srv_mp3_breakpoint_data_t), &write_p);
            f_close(&temp_fp);
        }

        LOGI("create breakpoint file:%d", res);
    }

    return res;
}
#endif

FRESULT audio_file_manager_get_breakpoint(void *data)
{
    FRESULT res = FR_OK;
    bt_sink_srv_mp3_breakpoint_data_t *bp_data = (bt_sink_srv_mp3_breakpoint_data_t *)data;
#ifndef MTK_MP3_ON_FLASH_ENABLE
    //FIL fp;
    UINT read_len;
    uint16_t temp_folder_id;
    int32_t ret;
    res = f_open(&temp_fp, AUDIO_FILE_MANAGER_BREAKPOINT_CONTROL_PATH, FA_OPEN_EXISTING | FA_READ | FA_WRITE);
    //debug_ucs2_string_dump(AUDIO_FILE_MANAGER_BREAKPOINT_CONTROL_PATH, ucs2_wcslen(AUDIO_FILE_MANAGER_BREAKPOINT_CONTROL_PATH)*2);

    if (res == FR_OK) {
        /* get the currenlty folder id from the root path */
        res = f_read(&temp_fp, &temp_folder_id, sizeof(uint16_t), &read_len);
        LOGI("get breakpoint len res %d, folder_id %d", res, temp_folder_id);
        if (read_len == sizeof(uint16_t)) {
            bp_data->folder_id = temp_folder_id;
            ret = audio_file_manager_get_breakpoint_file_by_folder(bp_data);
            LOGI("get breakpoint data: %d", ret);
        } else {
            LOGE("get breakpoint len error %d", read_len);
        }
        f_close(&temp_fp);
    }

    // rewrite file whatever open or read failed
    if (res != FR_OK) {
        bp_data->folder_id = AUDIO_FILE_MANAGER_DEFAULT_FOLDER_ID;
        bp_data->file_id = AUDIO_FILE_MANGER_DEFAULT_FILE_ID;
        bp_data->offset = 0;
        res = f_open(&temp_fp, AUDIO_FILE_MANAGER_BREAKPOINT_CONTROL_PATH, FA_OPEN_ALWAYS | FA_WRITE);
        LOGW("create breakpoint data:%d", res);
        if (res == FR_OK) {
            UINT write_p;
            res = f_write(&temp_fp, (void *) &bp_data->folder_id, 2, &write_p);
            f_close(&temp_fp);
            LOGW("write created breakpoint data:%d", res, write_p);
        }
    }
#else
    bp_data->folder_id = AUDIO_FILE_MANAGER_DEFAULT_FOLDER_ID;
    bp_data->file_id = AUDIO_FILE_MANGER_DEFAULT_FILE_ID;
    bp_data->offset = 0;
#endif

    return res;
}

FRESULT audio_file_manager_set_breakpoint(void *data)
{
    FRESULT res = FR_OK;
#ifndef MTK_MP3_ON_FLASH_ENABLE
    bt_sink_srv_mp3_breakpoint_data_t *bp_data = (bt_sink_srv_mp3_breakpoint_data_t *)data;
#if 0
    //FIL fp;
    res = f_open(&temp_fp, AUDIO_FILE_MANAGER_BREAKPOINT_CONTROL_PATH, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
    UINT write_p;
    if (res == FR_OK) {
        f_write(&temp_fp, (void *)&bp_data->folder_id, 2,  &write_p);
        f_close(&temp_fp);
    } else {
        LOGE("open bp data parameter fail res %d", res);
    }
#endif

    res = audio_file_manager_set_breakpoint_file_by_folder(bp_data);
#endif
    return res;
}

