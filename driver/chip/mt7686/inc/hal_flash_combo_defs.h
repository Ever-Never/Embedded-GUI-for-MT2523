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

#ifndef _COMBO_FLASH_DEFS_H_
#define _COMBO_FLASH_DEFS_H_

//#include "kal_release.h"
#include "hal_flash_custom_memorydevice.h"
#include "hal_flash_drvflash.h"
#include "hal_flash_opt.h"

#if defined(__COMBO_MEMORY_SUPPORT__) || defined(__SERIAL_FLASH__) || defined(__SPI_NAND_SUPPORT__)

#include "hal_flash_general_types.h"

#define CMEM_FLASH_ID_LEN_NOR  4
#define CMEM_FLASH_ID_LEN_NAND 8
#define CMEM_REGION_INFO_LEN   8
#define CMEM_BLOCK_INFO_LEN    8
#define CMEM_BANK_INFO_LEN     8
#define CMEM_BLOCK_INFO_LEN    8
#define COMBO_MEM_ID_VER       1
#define COMBO_MEM_NOR_VER      2
#define COMBO_MEM_IDENTITY_ID     "COMBOMEM_ID"
#define COMBO_MEM_IDENTITY_NOR    "COMBOMEM_NOR"
#define COMBO_MEM_ID_GFH_HEADER   GFH_HEADER(GFH_CMEM_ID_INFO, 1)
#define COMBO_MEM_NOR_GFH_HEADER  GFH_HEADER(GFH_CMEM_NOR_INFO, 1)

#ifdef __SERIAL_FLASH__
#include "hal_flash_combo_sfi_defs.h"
#endif

#if defined(__COMBO_MEMORY_SUPPORT__)
#if defined (SFI_COMBO_COUNT)
#define COMBO_MEM_DEVICE_COUNT SFI_COMBO_COUNT
#else
#ifndef COMBO_MEM_ENTRY_COUNT
#error "COMBO_MEM_ENTRY_COUNT was not defined"
#endif
#if !defined(__DUAL_SERIAL_FLASH_SUPPORT__)
#define COMBO_MEM_DEVICE_COUNT COMBO_MEM_ENTRY_COUNT
#else
#define COMBO_MEM_DEVICE_COUNT DUAL_COMBO_MEM_ENTRY_COUNT
#endif
#endif
#else
#define COMBO_MEM_DEVICE_COUNT 1
#endif

//-----------------------------------------------------------------------------
// Combo MCP ID list
//-----------------------------------------------------------------------------

// Union: CMEMFlashID
typedef union   {
    uint8_t  NAND[CMEM_FLASH_ID_LEN_NAND];
    uint16_t NOR[CMEM_FLASH_ID_LEN_NOR];
} CMEMFlashID;

// Type: CMEMEntryID
typedef struct {
    uint8_t       DeviceType;  // rename to Flash Type: NOR / NAND /EMMC
    uint8_t       IDLength;
#if defined(_NAND_FLASH_BOOTING_)|| defined(__SERIAL_FLASH__)
    uint8_t       ID[CMEM_FLASH_ID_LEN_NAND];
#else
    uint16_t      ID[CMEM_FLASH_ID_LEN_NOR];
#endif
} CMEMEntryID;

// Type: CMEMEntryIDList
typedef struct {
    unsigned int       m_ver;
    unsigned int       Count;
    CMEMEntryID        List[COMBO_MEM_DEVICE_COUNT];
} CMEMEntryIDList;

//-----------------------------------------------------------------------------
// Combo MCP SW Settings
//-----------------------------------------------------------------------------
// Type: FlashBlockLayout
typedef struct  {
    uint32_t  Offset;
    uint32_t  Size;
} FlashBlockLayout;

// Type: CMEMEntryNOR
typedef struct {
    uint16_t          FDMType;       // rename to Device Type: DEFAULT / SIB
    uint16_t          PageBufferSize;   // Autogen from MDL
    uint32_t          UniformBlocks;    // Uniform Block Layout
    FlashBlockLayout    BlockLayout[CMEM_REGION_INFO_LEN];
    FlashBankInfo       BankInfo[CMEM_BANK_INFO_LEN];
} CMEMEntryNOR;

// Type: CMEMEntryNORList
typedef struct {
    unsigned int       m_ver;
    unsigned int       Count;
    CMEMEntryNOR       List[COMBO_MEM_DEVICE_COUNT];
} CMEMEntryNORList;


#endif // defined(__COMBO_MEMORY_SUPPORT__) || defined(__SERIAL_FLASH__)

#endif // ifndef _COMBO_FLASH_DEFS_H_

