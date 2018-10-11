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
 *  flashtool.h
 *
 * Project:
 * --------
 *  Flash Download/Format Library.
 *
 * Description:
 * ------------
 *  Exported C interface APIs for FlashTool Library.
 *
 *******************************************************************************/
#ifndef _FLASHTOOL_H_
#define _FLASHTOOL_H_

#include "brom.h"
#include "DOWNLOAD.H"
#include "GNSS_DL_api.h"
#include <stdio.h>
#include "gnss_types.h"
#include "FreeRTOS.h"

#ifdef   __cplusplus
extern "C" {
#endif

//------------------------------------------------------------------------------
// return code
//------------------------------------------------------------------------------

#define FT_RET(ret)  (ret&0x000000FF)

#define FT_OK                          0x000000
#define FT_ERROR                       0x000001
#define FT_INVALID_ARGUMENTS           0x000002
#define FT_COM_PORT_OPEN_ERR           0x000003
#define FT_DA_HANDLE_ERROR             0x000004
#define FT_DL_HANDLE_ERROR             0x000005
#define FT_BROM_ERROR                  0x000007
#define FT_COM_PORT_SET_TIMEOUT_ERR    0x000008
#define FT_DA_NO_RESPONSE              0x000009
#define FT_DA_SYNC_INCORRECT           0x00000A
#define FT_DA_VERSION_INCORRECT        0x00000B
#define FT_DA_UNKNOWN_FLASH_DEVICE     0x00000C
#define FT_DA_SET_EXT_CLOCK_FAIL       0x00000D
#define FT_DA_SET_BBCHIP_TYPE_FAIL     0x00000E
#define FT_DA_CHANGE_BAUDRATE_FAIL     0x00000F
#define FT_DA_SET_DOWNLOAD_BLOCK_FAIL  0x000010
#define FT_DA_DOWNLOAD_FAIL            0x000011
#define FT_DA_FORMAT_FAIL              0x000013
#define FT_DA_FINISH_CMD_FAIL          0x000014
#define FT_DA_SOC_CHECK_FAIL           0x000015
#define FT_DA_BBCHIP_DSP_VER_INCORRECT 0x000016
#define FT_SKIP_AUTO_FORMAT_FAT        0x000017
#define FT_DA_HW_ERROR                 0x000018
#define FT_DA_ENABLE_WATCHDOG_FAIL     0x000019
#define FT_CALLBACK_ERROR              0x000020



//------------------------------------------------------------------------------
// accuracy enum
//------------------------------------------------------------------------------
typedef enum {
    ACCURACY_AUTO     = 0,     // auto detect by baudrate
    ACCURACY_1_3      = 3,     //   33%
    ACCURACY_1_4      = 4,     //   25%
    ACCURACY_1_10     = 10,    //   10%
    ACCURACY_1_100    = 100,   //    1%
    ACCURACY_1_1000   = 1000,  //  0.1%
    ACCURACY_1_10000  = 10000  // 0.01%
} ACCURACY;


//------------------------------------------------------------------------------
// com port setting
//------------------------------------------------------------------------------
#define  COM_NO_TIMEOUT       0

#define TIMEOUT  50000

//------------------------------------------------------------------------------
// DA report structure
//------------------------------------------------------------------------------
typedef  struct DA_REPORT_T {
    unsigned char  expected_da_major_ver;
    unsigned char  expected_da_minor_ver;
    unsigned char  da_major_ver;
    unsigned char  da_minor_ver;
    DEVICE_INFO    flash_device_id;
    unsigned int   flash_size;
    unsigned int   fat_begin_addr;
    unsigned int   fat_length;
    unsigned int   ext_sram_size;
} s_DA_REPORT_T;


//------------------------------------------------------------------------------
// FLASHTOOL_ARG structure
//------------------------------------------------------------------------------

#define  WAIT_STATE_AUTO_DETECT_BY_FREQUENCY NULL

typedef  struct {
    int   ext_clock;

    CALLBACK_DOWNLOAD_PROGRESS_INIT m_cb_download_conn_init;
    void  *m_cb_download_conn_init_arg;

    CALLBACK_CONN_BROM_WRITE_BUF_INIT m_cb_download_conn_da_init;
    void *m_cb_download_conn_da_init_arg;

    unsigned int   ms_boot_timeout;
    unsigned int   max_start_cmd_retry_count;

    // [OUT] the PC side bbchip name
    const char  *p_bbchip_name;

    // This callback function will be invoke after BootROM start cmd is passed.
    // You can issue other BootROM command by brom_handle and hCOM which provides callback arguments,
    // or do whatever you want otherwise.
    CALLBACK_DOWNLOAD_PROGRESS  m_cb_download_conn;
    void  *m_cb_download_conn_arg;

    CALLBACK_CONN_BROM_WRITE_BUF  m_cb_download_conn_da;
    void *m_cb_download_conn_da_arg;

} FLASHTOOL_ARG;

//------------------------------------------------------------------------------
// flash download
//------------------------------------------------------------------------------

extern int  FlashDownload(
    GNSS_DA *GNSS_da,
    GNSS_Image_List *GNSS_image_list,
    GNSS_Download_Arg arg);

extern int GNSS_DL();


#ifdef   __cplusplus
}
#endif

#endif



