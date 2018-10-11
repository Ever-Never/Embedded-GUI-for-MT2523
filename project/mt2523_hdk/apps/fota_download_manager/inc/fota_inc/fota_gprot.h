/* Copyright Statement:
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

#ifndef __FOTA_BT_NOTI_SRV__
#define __FOTA_BT_NOTI_SRV__

#include <stdint.h>
#include "hal_flash.h"
#include "bt_spp.h"

/*****************************************************************************
 * define
 *****************************************************************************/

/* for extend command sender and receiver */
#define FOTA_EXTCMD_UPDATE_BIN_SENDER           "fota_fbin"
#define FOTA_EXTCMD_UPDATE_BIN_RECEIVER         "fota_fbin"
#define FOTA_EXTCMD_GET_VERSION_SENDER          "fota_bt_ver"
#define FOTA_EXTCMD_GET_VERSION_RECEIVER        "fota_bt_ver"
#define FOTA_EXTCMD_CUSTOMER_COMMAND_SENDER     "fota_cust_cmd"
#define FOTA_EXTCMD_CUSTOMER_COMMAND_RECEIVER   "fota_cust_cmd"

#define FOTA_EXTCMD_GNSS_UPDATE_SENDER           "gnss_update"
#define FOTA_EXTCMD_GNSS_UPDATE_RECEIVER         "gnss_update"


/* fota receive firmware data type */
#define FOTA_FILE_DATA_BEGIN                    0
#define FOTA_FILE_DATA_PACK                     1
#define FOTA_FILE_DATA_END                      2

/* fota error code */
#define FOTA_PACK_END_CORRECT                   2
#define FOTA_PACK_END_WRONG                    -4
#define FOTA_UPDATE_SUCCESS                     3
#define FOTA_UPDATE_COMMON_ERROR               -1
#define FOTA_WRITE_FLASH_FAIL                  -2
#define FOTA_UBIN_OVERSIZE                     -3
/*****************************************************************************
 * Typedef
 *****************************************************************************/
/**
 *  memory info for store bin file on flash
 */
typedef struct {
    uint32_t start_address;
    uint32_t end_address;
    uint32_t write_ptr;
    uint32_t reserved_size;
    uint32_t total_received;
    uint32_t block_count;
    uint32_t block_size;
    uint32_t ubin_pack_count;
    hal_flash_block_t block_type;
} fota_mem_info_t;

typedef enum {
    FOTA_UPDATE_NONE,
    FOTA_UPDATE_FBIN,
    FOTA_UPDATE_GNSS,
    FOTA_UPDATE_END
} fota_update_enum_t;

/*****************************************************************************
 * Function
 *****************************************************************************/
extern void fota_download_manager_init(void);
#endif //__FOTA_BT_NOTI_SRV__
