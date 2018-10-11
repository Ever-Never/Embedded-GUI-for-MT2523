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

#ifndef NAND_DAL_H
#define NAND_DAL_H

#include "stdint.h"
#include "stddef.h"

/****************************
 * Include Header Files     *
 ****************************/
//#include "fota_error.h"
// Add for RHR
// RHR Remove    #include "kal_non_specific_general_types.h"
//#include "kal_general_types.h"
// Add for RHR
/*******************************************************************************
 *  Operation error code for NFB functions
 *******************************************************************************/
#define ERROR_NFB_SUCCESS             (ERROR_FOTA_SUCCESS)
#define ERROR_NFB_READ                (ERROR_FOTA_READ)
#define ERROR_NFB_PROGRAM             (ERROR_FOTA_PROGRAM)
#define ERROR_NFB_ERASE               (ERROR_FOTA_ERASE)
#define ERROR_NFB_BAD_BLOCK           (ERROR_FOTA_BAD_BLOCK)
#define ERROR_NFB_CHECKSUM            (ERROR_FOTA_NFB_CHECKSUM)
#define ERROR_NFB_ECC_CORRECTED       (ERROR_FOTA_ECC_CORRECTED)

/**************************************
 * Returns:  FS_NO_ERROR
 *              FS_NAND_DEVICE_NOT_SUPPORTED
 *              FS_FLASH_MOUNT_ERROR
 *************************************/
uint32_t DAL_init (void);
uint32_t DAL_GetBlockSize(void);
uint32_t DAL_GetPageSize(void);
uint8_t NANDsanitycheck_Factory(void);
void DAL_Test (void);

int8_t init_MTD_lock(void);

int8_t get_MTD_lock(void);

int8_t free_MTD_lock(void);


#if 0
#ifdef _NAND_FLASH_BOOTING_

int NFB_ProgramPhysicalPage(uint32_t PhyBlock, uint32_t PhyPage, void *Data, uint8_t DALRemap);

int NFB_ProgramPhysicalSpare(uint32_t PhyBlock, uint32_t PhyPage, void *Data, uint8_t chksum, uint8_t DALRemap);

int NFB_ErasePhysicalBlock(uint32_t PhyBlock, uint8_t DALRemap);

int	NFB_MarkBadBlock(uint32_t PhyBlock, uint8_t DALRemap);

int	NFB_ProgramPhysicalPageWithSpare(uint32_t PhyBlock, uint32_t PhyPage, void *Data, void *Spare, uint8_t DALRemap);

int32_t NFB_ReadPhysicalPage(uint32_t PhyBlock, uint32_t PhyPage, void *Data);

int NFB_ReadPhysicalSpare(uint32_t PhyBlock, uint32_t PhyPage, void *Data, uint8_t chksum);

#if defined(__FOTA_DM__)
int32_t NFB_ReadPhysicalPageWithSpare(uint32_t PhyBlock, uint32_t PhyPage, uint8_t *Data);
#endif // defined(__FOTA_DM__)

int32_t NFB_CheckGoodBlock(uint32_t Block);

int32_t NFB_CheckGoodBlock2(uint32_t Block, uint8_t DALRemap);

int32_t demp_CheckGoodBlock(uint32_t Block);

int32_t demp_ReadPhysicalPage(uint32_t PhyBlock, uint32_t PhyPage, void *Data);

#endif //_NAND_FLASH_BOOTING_
#endif

#endif /*NAND_DAL_H*/
