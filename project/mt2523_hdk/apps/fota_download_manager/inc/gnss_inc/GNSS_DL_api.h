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

/*******************************************************************************
* Filename:
* ---------
*  GNS_DL_api.h
*
* Project:
* --------
*  GNS Download Library.
*
* Description:
* ------------
*  Exported C interface APIs for GNSS Download Library.
*
* Author:
* -------
*  Kailing Huang
*
*******************************************************************************/
#ifndef _GNSS_DL_API_H_
#define _GNSS_DL_API_H_

//#define __GNSS_FW_UPDATE_SUPPORT__
#ifdef FOTA_FS_ENABLE
#include "ff.h"
#endif
#define MAX_GNSS_IMAGE_NUM 16

#define MAX_FILENAME_LENGTH 256
#define MAX_DA_IMAGE_LENGTH 0x5000


#define GNSS_FW_DEFAULT_FILE_NAME L"Z:\\@fota_iot\\gnss_image.bin"
#define GNSS_FW_DOWNLOAD_OK 0
#define GNSS_FW_DOWNLOAD_FILE_OPEN_ERR -1
#define GNSS_FW_DOWNLOAD_FILE_GTE_FILE_SIZE_ERR -2
#define GNSS_FW_DOWNLOAD_FILE_READ_ERR -3
#define GNSS_FW_DOWNLOAD_UPLOAD_ERR -4

typedef int (*CALLBACK_DOWNLOAD_PROGRESS_INIT)(void *usr_arg);
typedef int (*CALLBACK_DOWNLOAD_PROGRESS)(unsigned char finished_percentage,
        unsigned int finished_bytes,
        unsigned int total_bytes, void *usr_arg);

typedef int (*CALLBACK_CONN_BROM_WRITE_BUF_INIT)(void *usr_arg);
typedef int (*CALLBACK_CONN_BROM_WRITE_BUF)(unsigned char finished_percentage,
        unsigned int sent_bytes,
        unsigned int total_bytes, void *usr_arg);

typedef struct {
    const unsigned char     *m_image;
    unsigned int            m_size;
} GNSS_DA;

#ifndef FOTA_FS_ENABLE
typedef struct {
    const unsigned char     *m_image;
    unsigned int            m_size;
} GNSS_Image;
#else
typedef struct {
    FIL*     pImageFileHandle;
    FSIZE_t  m_size;
} GNSS_Image;
#endif

typedef struct {
    unsigned int    m_num;
    GNSS_Image       m_image_list[MAX_GNSS_IMAGE_NUM];
} GNSS_Image_List;

typedef struct {
    unsigned int      m_packet_length;
    unsigned char     m_num_of_unchanged_data_blocks;
#ifndef FOTA_FS_ENABLE
    const unsigned char     *m_buf;
#else
    FIL*              pImageFileHandle;
#endif
    unsigned int      m_len;
    unsigned int      m_begin_addr;
    unsigned int      m_end_addr;
} ROM_HANDLE_T;

typedef struct {
    int                                 m_bEnableLog;
    int                                *m_p_bootstop;
    CALLBACK_CONN_BROM_WRITE_BUF_INIT   m_cb_download_conn_da_init;
    void                               *m_cb_download_conn_da_init_arg;
    CALLBACK_CONN_BROM_WRITE_BUF        m_cb_download_conn_da;
    void                               *m_cb_download_conn_da_arg;
} GNSS_DA_Arg;

typedef struct {
    CALLBACK_DOWNLOAD_PROGRESS_INIT     m_cb_download_conn_init;
    void                               *m_cb_download_conn_init_arg;
    CALLBACK_DOWNLOAD_PROGRESS          m_cb_download_conn;
    void                               *m_cb_download_conn_arg;
    CALLBACK_CONN_BROM_WRITE_BUF_INIT   m_cb_download_conn_da_init;
    void                               *m_cb_download_conn_da_init_arg;
    CALLBACK_CONN_BROM_WRITE_BUF        m_cb_download_conn_da;
    void                               *m_cb_download_conn_da_arg;
    int                                 m_bEnableLog;
    int                                *m_p_bootstop;
} GNSS_Download_Arg;


int GNSS_DL();

#endif
