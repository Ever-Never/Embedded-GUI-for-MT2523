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

#include "fota_config.h"

#define FLASH_BLOCK_SIZE        (4096)

#ifdef MTK_FOTA_ON_7687
#include "flash_map.h"
#ifdef MTK_FOTA_DUAL_IMAGE_ENABLE
#include "flash_map_dual.h"
static fota_partition_t s_flash_table[] =
{
    {
        .id         = FOTA_PARTITION_LOADER,
        .address    = 0x00000,
        .length     = LOADER_LENGTH
    },
    {
        .id         = FOTA_PARTITION_N9_A,
        .address    = N9_RAMCODE_BASE_A,
        .length     = N9_RAMCODE_LENGTH_A
    },
    {
        .id         = FOTA_PARTITION_CM4_A,
        .address    = CM4_CODE_BASE_A,
        .length     = CM4_CODE_LENGTH_A
    },
    {
        .id         = FOTA_PARTITION_N9_B,
        .address    = N9_RAMCODE_BASE_B,
        .length     = N9_RAMCODE_LENGTH_B
    },
    {
        .id         = FOTA_PARTITION_CM4_B,
        .address    = CM4_CODE_BASE_B,
        .length     = CM4_CODE_LENGTH_B
    }      
};
#else
static fota_partition_t s_flash_table[] =
{
    {
        .id         = FOTA_PARITION_LOADER,
        .address    = 0x00000,
        .length     = LOADER_LENGTH
    },
    {
        .id         = FOTA_PARITION_NCP,
        .address    = N9_RAMCODE_BASE,
        .length     = N9_RAMCODE_LENGTH
    },
    {
        .id         = FOTA_PARITION_CM4,
        .address    = CM4_CODE_BASE,
        .length     = CM4_CODE_LENGTH
    },
    {
        .id         = FOTA_PARITION_TMP,
        .address    = FOTA_BASE,
        .length     = FOTA_LENGTH  /* the last block of fota temp partition is reserved for triggered flag & update info */
    }
};
#endif
#else /* Not 87/97 partition setting */
#include "memory_map.h"
#ifdef ROM_BASE
#define FLASH_CONFIG_BASE    (ROM_BASE)
#else
#define FLASH_CONFIG_BASE    (BL_BASE)
#endif
static fota_partition_t s_flash_table[] =
{
    {
        .id         = FOTA_PARITION_LOADER,
        .address    = FLASH_CONFIG_BASE,
        .length     = BL_LENGTH
    },
    {
        .id         = FOTA_PARITION_NCP,
        .address    = 0,
        .length     = 0
    },
    {
        .id         = FOTA_PARITION_CM4,
        .address    = CM4_BASE - FLASH_CONFIG_BASE,
        .length     = CM4_LENGTH
    },
    {
        .id         = FOTA_PARITION_TMP,
        .address    = FOTA_RESERVED_BASE - FLASH_CONFIG_BASE,
        .length     = FOTA_RESERVED_LENGTH  /* the last block of fota temp partition is reserved for triggered flag & update info */
    }
};

#endif /* MTK_FOTA_ON_7687 */

#define FLASH_TABLE_ENTRIES (sizeof(s_flash_table) / sizeof(fota_partition_t))


fota_flash_t fota_flash_default_config =
{
    .table          = &s_flash_table[0],
    .table_entries  = FLASH_TABLE_ENTRIES,
    .block_size     = 4096
};

