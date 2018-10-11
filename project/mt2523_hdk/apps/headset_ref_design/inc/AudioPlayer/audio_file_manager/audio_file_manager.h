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

#ifndef __AUDIO_FILE_MANAGER_H__
#define __AUDIO_FILE_MANAGER_H__

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "ff.h"

#define MAX_FILE_PATH_LEN               (256)
#define MAX_ALBUM_NUM                   (100)
#define MAX_FILE_NUM                    (1000)
#define MAX_FILE_NUM_IN_ONE_ALBUM       (128)

#define MAX_NAME_LEN                    (80 + 1)

#define AUDIO_FILE_MANAGER_FOLDER_BREAK_POINT_LEN   8
#define AUDIO_FILE_MANAGER_BREAKPOINT_FILE_NAME        (_T("BP_DATA"))
#define AUDIO_FILE_MANAGER_BREAKPOINT_CONTROL_PATH     (_T("SD:/BP_DATA"))

#define AUDIO_FILE_MANAGER_DEFAULT_FOLDER_ID 0x01
#define AUDIO_FILE_MANGER_DEFAULT_FILE_ID 0x01


typedef struct {
    uint16_t album_idx;
    TCHAR album_name[MAX_NAME_LEN];
}album_info_t;

typedef struct {
    album_info_t info;
    uint16_t file_num;
}album_rec_t;

typedef struct {
    uint16_t file_idx;
    TCHAR file_name[MAX_NAME_LEN];
}aud_file_rec_t;

typedef enum {
    AUD_STATUS_CODE_NOT_READY = -6,
    AUD_STATUS_CODE_FS_ERROR = -5,
    AUD_STATUS_CODE_UNEXPECTED_ERROR = -4,
    AUD_STATUS_CODE_FOLDER_NOT_EXIST = -3,
    AUD_STATUS_CODE_FILE_NOT_EXIST = -2,
    AUD_STATUS_CODE_INVALID_CMD = -1,
    AUD_STATUS_CODE_SUCCESS = 0,
    AUD_STATUS_CODE_SUCCESS_WITHOUT_RSP_STATUS = 1
}aud_status_code_t;

/**
 * @brief    get album information list, including index and name for each album.
 * @param[out] album_info_list    pointer to user buffer that stores the album information throught structure #album_rec_t.
 * @param[in] buf_len    is length of user buffer, will decide how many album information can be stored in user buffer. 
 * @param[in] album_start_idx    is the start index that user want to get album list from. 
 * @param[in,out] album_num    pointer to the number of album user want to get as input, and pointer to the actual retrieved number of album as output.
 * @return 0 means success, <0 means failure. 
 */
extern aud_status_code_t audio_file_manager_get_album_list(album_rec_t *album_info_list, uint32_t buf_len, uint16_t album_start_idx, uint32_t *album_num);

/**
 * @brief    get album name by index.
 * @param[out] album_name    pointer to user buffer that stores the album name, the maxium lenght of name is 80 bytes, suggest to prepare 81 bytes user buffer to store.
 * @param[in] buf_len    is length of user buffer. 
 * @param[in] album_idx    is album index, start from 1.
 * @return 0 means success, <0 means failure. 
 */
extern aud_status_code_t audio_file_manager_get_album_name_by_idx(TCHAR *album_name, uint32_t buf_len, uint16_t album_idx);


/**
 * @brief    get album path by index.
 * @param[out] album_path    pointer to user buffer that stores the album fulel path.
 * @param[in] buf_len    is length of user buffer. 
 * @param[in] album_idx    is album index, start from 1.
 * @return 0 means success, <0 means failure. 
 */
extern aud_status_code_t audio_file_manager_get_album_path_by_idx(TCHAR *album_path, uint32_t buf_len, uint16_t album_idx);

/**
 * @brief    get the number of album.
 * @param[out] album_num    pointer to user buffer that stores the number of album.
 * @return 0 means success, <0 means failure. 
 */
extern aud_status_code_t audio_file_manager_get_album_num(uint16_t *album_num);

/**
 * @brief    get file information list in specified album.
 * @param[out] file_list    pointer to user buffer that stores the file information throught structure #aud_file_rec_t.
 * @param[in] buf_len    is length of user buffer, will decide how many file information can be stored in user buffer. 
 * @param[in] album_idx    is album index, start from 1.
 * @param[in] file_start_idx    is the start index that user want to get file list from.
 * @param[in,out] file_num    pointer to the number of file user want to get as input, and pointer to the actual retrieved number of file in specified album as output.
 
 * @return 0 means success, <0 means failure. 
 */
extern aud_status_code_t audio_file_manager_get_file_list_by_album(aud_file_rec_t *file_list, uint32_t buf_len, uint16_t album_idx, uint16_t file_start_idx, uint32_t *file_num);

/**
 * @brief    get full path of the specifed file, located by both album index and file index.
 * @param[out] file_path    pointer to user buffer that stores the file path, the maxium length of full path is 256 bytes.
 * @param[in] buf_len    is length of user buffer. 
 * @param[in] album_idx    is album index, start from 1.
 * @param[in] file_idx    is file index, start from 1.
 * @return 0 means success, <0 means failure. 
 */
extern aud_status_code_t audio_file_manager_get_file_path_by_idx(TCHAR *file_path, uint32_t buf_len, uint16_t album_idx, uint16_t file_idx);

/**
 * @brief    get the number of files in the specified album.
 * @param[out] file_num    pointer to user buffer that stores the number of files in spepcified album folder.
 * @param[in] album_idx    is album index, start from 1.
 * @return 0 means success, <0 means failure. 
 */
extern aud_status_code_t audio_file_manager_get_file_num_by_idx(uint16_t* file_num, uint16_t album_idx);

/**
 * @brief    delete the specifed album, also including all files in this album.
 * @param[in] album_idx    is album index, start from 1.
 * @return 0 means success, <0 means failure. 
 */
extern aud_status_code_t audio_file_manager_delete_album_by_idx(uint16_t album_idx);

/**
 * @brief    delete the specifed file, located by both album index and file index.
 * @param[in] album_idx    is album index, start from 1. 
 * @param[in] file_idx    is file index, start from 1.
 * @return 0 means success, <0 means failure. 
 */
extern aud_status_code_t audio_file_manager_delete_file_by_idx(uint16_t album_idx, uint16_t file_idx);

/**
 * @brief    create the indexing for albums and files. if download any new album, should call this function to rebuild.
 * @return 0 means success, <0 means failure. 
 */
extern aud_status_code_t audio_file_manager_rebuild_db();


/**
 * @brief    create the indexing for the specified albums
 * @param[in] folder_name    is ucs2 encoding of folder name, each character occupies two bytes.
 * @return 0 means success, <0 means failure. 
 */
extern aud_status_code_t audio_file_manager_create_album_db(TCHAR * folder_name);

/**
 * @brief    initialize the audio file manager module, should call this function first before using any other APIs in this module.
 * @return 0 means success, <0 means failure. 
 */
extern aud_status_code_t audio_file_manager_init();

/**
 * @brief    deinitialize the audio file manager module, should call this function while want to stop audio file manager.
 * @return 0 means success, <0 means failure.
 */
extern aud_status_code_t audio_file_manager_deinit();

/**
 * @brief    callbacks for lower layers
 */
FRESULT audio_file_manager_open(FIL *fp, TCHAR *path);
FRESULT audio_file_manager_lseek(FIL *fp, DWORD pos, uint32_t *offset);
FRESULT audio_file_manager_read(FIL *fp, void *buf, uint32_t bytetoread, UINT *byteread,
        uint32_t index, uint32_t *offset);
uint32_t audio_file_manager_tell(FIL *fp, uint32_t *offset);
bool audio_file_manager_eof(FIL *fp, uint32_t index, uint32_t *offset);
uint32_t audio_file_manager_size(FIL *fp, uint32_t index);
FRESULT audio_file_manager_set_breakpoint(void *bp_data);
FRESULT audio_file_manager_get_breakpoint(void *bp_data);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_FILE_MANAGER_H__ */

