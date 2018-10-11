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

#include "hal_flash.h"
#ifdef HAL_FLASH_MODULE_ENABLED

#include <string.h>
#include "hal_flash_custom_memorydevice.h"
#include "hal_flash_opt.h"

//#include "kal_release.h"
#include "hal_flash_combo_init.h"
//#include "fat_fs.h"
#include "hal_flash_combo_defs.h"

#include "hal_flash_mtd_sf_dal.h"
//#include "flash_mtd_pf_dal.h"

#if defined(__COMBO_MEMORY_SUPPORT__) || defined(__SERIAL_FLASH__)
#include "hal_flash_sf.h"
#include "hal_flash_drvflash.h"
#include "hal_flash_disk_internal.h"

//#include "drvflash_ut.h" //locateSector_ext for basic load
#include "hal_flash_mtd_ut.h" //__BASIC_LOAD_FLASH_TEST__

//#include "custom_nvram_int_config.h"  /* To get NVRAM_CUSTOM_CFG_MAX_RECORD_SECTOR_NUM for MSTABLE_ENTRY_NUM */

#include "hal_flash_opt.h"
#include "hal_flash_general_types.h"

typedef struct {
    GFH_Header        m_gfh_hdr;
    CMEMEntryNORList  m_data;
} GFH_CMEM_NOR_INFO_v1;

/*-------------------------------------------------------------------
 * MS table size
 *
 * MS table size should be synchronized with user's requirement. The only one user is NVRAM.
 *
 * NVRAM_CUSTOM_CFG_MAX_RECORD_SECTOR_NUM defines the maximum record size in NVRAM
 * (in custom_nvram_config.h). NVRAM should call NOR_ResumeSectorState (by FS_Commit)
 * after it writes maximum NVRAM_CUSTOM_CFG_MAX_RECORD_SECTOR_NUM sectors with protection
 * mode to ensure the integrity of a record.
 *
 * MSTABLE_ENTRY_NUM number is based on the worst senario: All FAT entries of clusters
 * of logical sectors are located in different physical sectors. In other word, we need
 * to write additional N sectors to update FAT after we write N data sectors. The other
 * 1 sector is for directory entry update.
 *
 * TODO: Use auto-gen to compute most suitable MS table size. Because the number of
 * sectors for FAT may be smaller than NVRAM_CUSTOM_CFG_MAX_RECORD_SECTOR_NUM. For example,
 * 64KB disk only needs 1 sector to store FAT.
 *---------------------------------------------------------- W08.50 --*/

#define MS_TABLE_ENTRY_NUM    (4 * 2 + 1)//(NVRAM_CUSTOM_CFG_MAX_RECORD_SECTOR_NUM * 2 + 1)


//-----------------------------------------------------------------------------
// Combo MCP related
//-----------------------------------------------------------------------------
static int32_t cmem_nor_index;        // Do not use this variabe during EMI init stage
const CMEMEntryID *cmem_id;
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
static int32_t cmem_nor_index_CS1;        // Do not use this variabe during EMI init stage
const CMEMEntryID *cmem_id_CS1;
#endif

//-----------------------------------------------------------------------------
// MCP Serial/NOR Flash Driver settings
//-----------------------------------------------------------------------------
#define  COMBO_MEM_TYPE_NAME  GFH_CMEM_NOR_INFO_v1

#define COMBO_MEM_INST_NAME combo_mem_sw_list
#define COMBO_MEM_TYPE_MODIFIER  static const

#define COMBO_MEM_STRUCT_HEAD  COMBO_MEM_NOR_GFH_HEADER, { COMBO_MEM_NOR_VER, COMBO_MEM_DEVICE_COUNT, {
#define COMBO_MEM_STRUCT_FOOT  } }

#include "hal_flash_combo_config.h"    // ==== Instantiate NOR flash table

static const CMEMEntryNOR *nor_list = NULL;


//-----------------------------------------------------------------------------
// Driver Interface and Driver Data
//-----------------------------------------------------------------------------
// System Info
#if !((defined(__FUE__) && defined(__FOTA_DM__)) || defined(__UBL__))
//---------------------------------------
// Part 1: NOR FDM Driver
//---------------------------------------
//FS_Driver NORFlashDriver;           // FDM driver interface
#if !defined(__NOR_FDM5__)
NOR_FLASH_DRV_Data FlashDriveData;  // FDM driver data
#endif
uint32_t TOTAL_BLOCKS;            // Local define
uint32_t TOTAL_SECTORS;           // Local define
uint32_t NOR_LARGEST_BLOCK_SIZE;  // Local define (to replace NOR_BLOCK_SIZE)
static uint16_t PAGE_SIZE;        // Local define (CMEM only)
#ifdef __INTEL_SIBLEY__
uint8_t FDMBuffer[1024];          // FDM buffer
#else
uint8_t FDMBuffer[512];           // FDM buffer
#endif
//static uint16_t AVAILSECTORS[CMEM_MAX_BLOCKS];     // Number of avail sectors in each block
//static uint16_t VALIDSECTORS[CMEM_MAX_BLOCKS];     // Number of valid sectors in each block
//static uint8_t  SectorMap[CMEM_MAX_SECTORS];       // Address translation map

#if defined(__ERASE_QUEUE_ENABLE__)
//static NOR_EraseInfo EraseBlockQueue[SNOR_ERASE_QUEUE_SIZE];   // Erase queue
#endif

static FlashRegionInfo RegionInfo[CMEM_REGION_INFO_LEN]; // RegionInfo[] assigned to FDM data

#elif (defined(__FUE__) || defined(__EXT_BOOTLOADER__)) && defined(__FOTA_DM__)
NOR_FLASH_DRV_Data FlashDriveData;
#endif // !((defined(__FUE__) && defined(__FOTA_DM__)) || defined(__UBL__))

static const FlashBlockLayout *BlockLayout;              // BlockLayout read from MCP list
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
static const FlashBlockLayout *BlockLayout_CS1;          // SF2 BlockLayout read from MCP list
#endif
uint32_t CS0_SIZE;
uint32_t CS1_SIZE;

#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)           // BankInfo[] assigned to FDM data
static FlashBankInfo BankInfo[CMEM_REGION_INFO_LEN];
#else
const static FlashBankInfo *BankInfo;
#endif
#if !defined(__UBL__)
//static MS_ENTRY MSEntryTable[MS_TABLE_ENTRY_NUM];
#endif

kal_eventgrpid  nor_egid = NULL;

//---------------------------------------
// Part 2: NOR MTD driver
//---------------------------------------
uint32_t PAGE_BUFFER_SIZE;              // Global Var. used by mtd driver

#ifdef __SERIAL_FLASH__
NOR_MTD_Driver NORFlashMtd;           // SF driver interface
SF_MTD_Data mtdflash;                 // SF driver data
SF_Status StatusMap[SF_SR_COUNT];     // status map assigned to SF data (shall be read only after initialziation, it's shareable)
SF_MTD_CMD sf_dal_data_cmd;           // command set assiged to SF data (shall be read only after initialziation, it's shareable)
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
SF_MTD_Data DualMtdflash[CS_COUNT];                 // SF driver data
SF_Status DualStatusMap[CS_COUNT][SF_SR_COUNT];     // status map assigned to SF data (shall be read only after initialziation, it's shareable)
SF_MTD_CMD Dual_sf_dal_data_cmd[CS_COUNT];           // command set assiged to SF data (shall be read only after initialziation, it's shareable)
#endif //defined(__DUAL_SERIAL_FLASH_SUPPORT__)
#else
NOR_MTD_Driver NORFlashMtd;           // ADM driver interface
PF_MTD_Data mtdflash;                 // ADM driver data was instantiated in flash_mtd.amd.c or flash_mtd.intel.c.
PF_Status StatusMap[PF_SR_COUNT];
PF_MTD_CMD pf_dal_data_cmd; 		  // command set assiged to PF data (shall be read only after initialziation, it's shareable)
#endif

//---------------------------------------
// Part 3: NOR RAW Disk FDM/MTD driver
//---------------------------------------

//---------------------------------------
// Part 4: FOTA
//---------------------------------------

//---------------------------------------
// Part 5: NOR Full Driver
//---------------------------------------
// FDM driver data
static FlashRegionInfo EntireDiskRegionInfo[CMEM_REGION_INFO_LEN];
NOR_FLASH_DISK_Data EntireDiskDriveData;

#ifdef __SERIAL_FLASH__
SF_MTD_Data EntireDiskMtdData;   // SF MTD driver data
#else
PF_MTD_Data EntireDiskMtdData;   // ADM MTD driver data
#endif

//-----------------------------------------------------------------------------
// Internal Functions
//-----------------------------------------------------------------------------
// NOR Flash Driver Initialization

// 1. Entry Function
int32_t CMEM_Init_NOR(void);        // MAUI
int32_t CMEM_Init_FullDriver(void); // MAUI or BOOTLOADER (for Card Download and RAW disk)

// 1.1 Common initilaization of the entry function
void      CMEM_Init_nor_list(const uint32_t CS);

// ---<NOR>---
// 2. NOR FDM Initialization

// 3. NOR MTD Initialization
int32_t CMEM_Init_NOR_MTD_Common(void);
int32_t CMEM_Init_NOR_MTD_SF(void);
extern int32_t NOR_Construct_DualRegionInfo(uint32_t baseaddr, uint32_t length, FlashRegionInfo *regioninfo);

// 4. NOR RAW Disk Initialization

// 5. EMI-INIT-NOR-driver
uint32_t EMIINIT_CMEM_BlockSize(uint32_t address);
uint32_t CMEM_NOR_Construct_RegionInfo_Internal(
    uint32_t baseaddr,
    uint32_t length,
    FlashRegionInfo *regioninfo,
    const FlashBlockLayout *blocklayout);
int32_t NOR_Construct_RegionInfo(uint32_t baseaddr, uint32_t length, FlashRegionInfo *regioninfo);
int32_t EMIINIT_CMEM_NOR_Construct_RegionInfo(uint32_t baseaddr, uint32_t length, FlashRegionInfo *regioninfo);
int32_t EMIINIT_CMEM_NOR_Construct_DualRegionInfo(uint32_t baseaddr, uint32_t length, FlashRegionInfo *regioninfo);

//-----------------------------------------------------------------------------
// External Function
//-----------------------------------------------------------------------------
extern uint32_t custom_get_fat_addr(void);
extern uint32_t INT_RetrieveFlashBaseAddr(void);
extern uint32_t custom_get_fat_len(void);
extern uint32_t custom_get_fat_addr(void);

uint32_t INT_RetrieveFlashBaseAddr(void)
{
    EntireDiskMtdData.BaseAddr = (BYTE *)(SFC_GENERIC_FLASH_BANK_MASK);
    return SFC_GENERIC_FLASH_BANK_MASK;
}

#if !((defined(__FUE__) && defined(__FOTA_DM__)) || defined(__UBL__))

//-----------------------------------------------------------------------------
/*!
  @brief
    NOR Flash MTD interface/data common initialization
  @retval
    CMEM_NO_ERROR: Success
*/
int32_t CMEM_Init_NOR_MTD_Common(void)
{
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    uint8_t i = 0;
#endif
    // 1. <DATA> Signature
    mtdflash.Signature = ~((uint32_t)RegionInfo);

    // 2. <DATA> Page Buffer and Base Address
    PAGE_BUFFER_SIZE = nor_list[cmem_nor_index].PageBufferSize;

#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
    memset(mtdflash.lookup_tbl, 0xFF, sizeof(NOR_AddrLookup)*LOOKUP_TABLE_SIZE * 2);
#endif // __SINGLE_BANK_NOR_FLASH_SUPPORT_
    mtdflash.BaseAddr = (BYTE *)(INT_RetrieveFlashBaseAddr() + NOR_FLASH_BASE_ADDRESS);

    // 3. <DATA> RegionInfo and BankInfo
    mtdflash.RegionInfo = (FlashRegionInfo *)RegionInfo;
#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__) && (defined(__MULTI_BANK_NOR_DEVICE__) || defined(__ARM9_MMU__) || defined(__ARM11_MMU__))
    mtdflash.BankInfo = (FlashBankInfo *)BankInfo;
#endif  // __SINGLE_BANK_NOR_FLASH_SUPPORT__

#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)

    for (; i < CS_COUNT; i++) {
        // 1. <DATA> Signature
        DualMtdflash[i].Signature = ~((uint32_t)RegionInfo);

        // 2. <DATA> Page Buffer and Base Address
        if (i == 0) {
            PAGE_BUFFER_SIZE = nor_list[cmem_nor_index].PageBufferSize;
        } else {
            if (PAGE_BUFFER_SIZE > nor_list[cmem_nor_index_CS1].PageBufferSize) { //IF SF2's page buffer size < SF1's, use SF2's page buffer size to program.
                PAGE_BUFFER_SIZE = nor_list[cmem_nor_index_CS1].PageBufferSize;
            }
        }


#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
        memset(DualMtdflash[i].lookup_tbl, 0xFF, sizeof(NOR_AddrLookup)*LOOKUP_TABLE_SIZE * 2);
#endif // __SINGLE_BANK_NOR_FLASH_SUPPORT_
        DualMtdflash[i].BaseAddr = (BYTE *)(INT_RetrieveFlashBaseAddr() + NOR_FLASH_BASE_ADDRESS);

        // 3. <DATA> RegionInfo and BankInfo
        DualMtdflash[i].RegionInfo = (FlashRegionInfo *)RegionInfo;
#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__) && (defined(__MULTI_BANK_NOR_DEVICE__) || defined(__ARM9_MMU__) || defined(__ARM11_MMU__))
        DualMtdflash[i].BankInfo = (FlashBankInfo *)BankInfo;
#endif  // __SINGLE_BANK_NOR_FLASH_SUPPORT__
    }

#endif //defined(__DUAL_SERIAL_FLASH_SUPPORT__)

    return CMEM_NO_ERROR;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Serial NOR Flash MTD interface/data initialization
  @retval
    CMEM_NO_ERROR: Success
    CMEM_ERR_MTD_INIT_FAIL: Fail
*/
int32_t CMEM_Init_NOR_MTD_SF(void)
{
    int32_t result;
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    int32_t temp;
    uint8_t i = 0;
#endif
    // Assign MTD driver to FDM data
    FlashDriveData.MTDDriver = &NORFlashMtd;

    // Allocate status map and command for MTD data
    mtdflash.StatusMap = &StatusMap[0];
    mtdflash.CMD = &sf_dal_data_cmd;
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    mtdflash.CS_Size = CS0_SIZE;
#endif
    mtdflash.CS = 0;
    // serial Flash driver will initialize the interface/data by its own.
    result = SF_DAL_Init_Driver(
                 &NORFlashMtd,   // Driver Interface (to FDM)
                 &mtdflash,      // Driver Data
                 (INT_RetrieveFlashBaseAddr() + NOR_FLASH_BASE_ADDRESS),   // Base Address
                 nor_list[cmem_nor_index].UniformBlocks);                  // Uniform Block

    if (result == FS_FLASH_MOUNT_ERROR)    {
        return CMEM_ERR_MTD_INIT_FAIL;
    }

#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)

    for (; i < CS_COUNT; i++) {
        // Assign MTD driver to FDM data
        FlashDriveData.MTDDriver = &NORFlashMtd;

        // Allocate status map and command for MTD data
        DualMtdflash[i].StatusMap = &DualStatusMap[i][0];
        DualMtdflash[i].CMD = &Dual_sf_dal_data_cmd[i];
        // Assign CS number
        DualMtdflash[i].CS = i;

        if (i == 0) {
            DualMtdflash[i].CS_Size = CS0_SIZE;
            temp = cmem_nor_index;
        } else {
            DualMtdflash[i].CS_Size = CS1_SIZE;
            temp = cmem_nor_index_CS1;
        }
        if (DualMtdflash[i].CS_Size != 0x0) { //If SF2 is Dummy, do not do Driver Init
            // serial Flash driver will initialize the interface/data by its own.
            result = SF_DAL_Init_Driver(
                         &NORFlashMtd,   // Driver Interface (to FDM)
                         &DualMtdflash[i],      // Driver Data
                         (INT_RetrieveFlashBaseAddr() + NOR_FLASH_BASE_ADDRESS),   // Base Address
                         nor_list[temp].UniformBlocks);                  // Uniform Block

            if (result == FS_FLASH_MOUNT_ERROR)    {
                return CMEM_ERR_MTD_INIT_FAIL;
            }
        }
    }
#endif //defined(__DUAL_SERIAL_FLASH_SUPPORT__)

    return CMEM_NO_ERROR;
}

#endif // !((defined(__FUE__) && defined(__FOTA_DM__)) || defined(__UBL__))

//-----------------------------------------------------------------------------
/*!
  @brief
    Returns number of Serial Flash: Single SF: 1   Dual SF: 2
  @remarks
    For EMI init stage only .
*/
uint8_t EMINIT_CMEM_GET_NOR_FLASH_COUNT(void)
{
    return CS_COUNT;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Returns the Flash size of spedified CS number.
  @remarks
    For EMI init stage only, it will compare ID and match the correct settings.
*/

#if defined(__SERIAL_FLASH_EN__)
uint32_t EMINIT_CMEM_GET_NOR_FLASH_SIZE(uint8_t CS)
{
    uint32_t index;
    const CMEMEntryNOR *list = NULL;
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    index = CMEM_EMIINIT_Index(CS);
#else
    index = CMEM_EMIINIT_Index();
#endif
    list = combo_mem_sw_list.m_data.List;
    return list[index].BankInfo->BankSize;

}
#endif //defined(__SERIAL_FLASH_EN__)

//-----------------------------------------------------------------------------
/*!
  @brief
    Returns the size of the block located at given address.
  @remarks
    This is a internal function placed in SECOND part, which can be called by FOTA, MAUI, and Bootloader
*/
uint32_t CMEM_BlockSize_Internal(uint32_t address, const FlashBlockLayout *blocklayout)
{
    uint16_t i;

    address = address & (SFC_GENERIC_FLASH_BANK_MASK - 1);

    for (i = 0; blocklayout[i + 1].Size != 0; i++)   {
        if (address >= blocklayout[i].Offset &&
                address < blocklayout[i + 1].Offset)  {
            break;
        }
    }
    return blocklayout[i].Size;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Returns the size of the block located at given address.
*/
uint32_t CMEM_BlockSize(uint32_t address)
{
    return CMEM_BlockSize_Internal(address, BlockLayout);
}

#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
//-----------------------------------------------------------------------------
/*!
  @brief
    Returns the size of the block located at given address.
*/
uint32_t CMEM_BlockSize_CS1(uint32_t address)
{
    return CMEM_BlockSize_Internal(address, BlockLayout_CS1);
}
#endif //defined(__DUAL_SERIAL_FLASH_SUPPORT__)

//-----------------------------------------------------------------------------
/*!
  @brief
    Returns the size of the block located at given address.
  @remarks
    For EMI init stage only, it will compare ID and match the correct settings.
*/
#if defined(__SERIAL_FLASH_EN__)
uint32_t EMIINIT_CMEM_BlockSize(uint32_t address)
{
#if !defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    uint32_t index = CMEM_EMIINIT_Index();
    const CMEMEntryNOR *list = NULL;

    list = combo_mem_sw_list.m_data.List;

    return CMEM_BlockSize_Internal(address, list[index].BlockLayout);
#else
    uint32_t index;
    const CMEMEntryNOR *list = NULL;
    if ((address & SFC_GENERIC_0x0FFFFFFF_MASK) >= EMINIT_CMEM_GET_NOR_FLASH_SIZE(0)) {
        index = CMEM_EMIINIT_Index(1);
        address = address - EMINIT_CMEM_GET_NOR_FLASH_SIZE(0);
    } else {
        index = CMEM_EMIINIT_Index(0);
    }

    list = combo_mem_sw_list.m_data.List;

    return CMEM_BlockSize_Internal(address, list[index].BlockLayout);
#endif
}
#endif //defined(__SERIAL_FLASH_EN__)
//-----------------------------------------------------------------------------
/*!
  @brief
    Construct region info by combo MCP block info
*/
int32_t NOR_Get_FlashSizeFromBankInfo(const FlashBankInfo *bankinfo)
{
    uint32_t result = 0, i;

    // For all entries in the geometry info
    for (i = 0; bankinfo[i].BankSize != 0; i++)   {
        result += bankinfo[i].Banks * bankinfo[i].BankSize;
    }
    return result;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Construct region info by combo MCP block info
  @remarks
    This is a internal function placed in SECOND part, which can be called by FOTA, MAUI, and Bootloader
*/
uint32_t CMEM_NOR_Construct_RegionInfo_Internal(
    uint32_t baseaddr,
    uint32_t length,
    FlashRegionInfo *regioninfo,
    const FlashBlockLayout *blocklayout)
{
    uint32_t addr, ptr, blksize, total_blocks;
    uint32_t endaddr = baseaddr + length;

#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    ptr = 0;
    //If RegionInfo of SF1 is constructed, we skip these RegionInfo and continue to construct SF2's RegionInfo
    while (regioninfo[ptr].BlockSize != 0) {
        ptr++;
        if (ptr == (CMEM_REGION_INFO_LEN + 1))  {
            return CMEM_ERR_FDM_REGION_INFO_OVERFLOW;
        }
    }
    for (addr = baseaddr, total_blocks = 0; addr < endaddr;)
#else

    // iterates all blocks
    for (addr = baseaddr, ptr = 0, total_blocks = 0; addr < endaddr;)
#endif
    {
        blksize = CMEM_BlockSize_Internal(addr, blocklayout);
        // the block is the first entry of the first region
        if (total_blocks == 0)   {
            regioninfo[ptr].BlockSize = blksize;
            regioninfo[ptr].RegionBlocks = 1;
        }
        // the block belongs to the same region
        else if (blksize == regioninfo[ptr].BlockSize)    {
            regioninfo[ptr].RegionBlocks++;
        }
        // the block belongs to a new region
        else {
            ptr++;
            if (ptr == (CMEM_REGION_INFO_LEN + 1))  {
                return CMEM_ERR_FDM_REGION_INFO_OVERFLOW;
            }
            regioninfo[ptr].BlockSize = blksize;
            regioninfo[ptr].RegionBlocks = 1;
        }
        addr += blksize;
        total_blocks++;
    }

    // TODO: Add bank boundary check and block boundary check

    return CMEM_NO_ERROR;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Construct region info by combo MCP block info
*/
int32_t NOR_Construct_RegionInfo(uint32_t baseaddr, uint32_t length, FlashRegionInfo *regioninfo)
{
    memset(regioninfo, 0, sizeof(FlashRegionInfo)*CMEM_REGION_INFO_LEN);

    return CMEM_NOR_Construct_RegionInfo_Internal(baseaddr, length, regioninfo, BlockLayout);
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Construct region info by combo MCP block info
*/

int32_t NOR_Construct_DualRegionInfo(uint32_t baseaddr, uint32_t length, FlashRegionInfo *regioninfo)
{
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)&& !defined(__MINI_BOOTLOADER__)
    memset(regioninfo, 0, sizeof(FlashRegionInfo)*CMEM_REGION_INFO_LEN);
    //FAT in first SF
    if (((baseaddr & SFC_GENERIC_0x0FFFFFFF_MASK) < CS0_SIZE) && (((baseaddr + length) & SFC_GENERIC_0x0FFFFFFF_MASK) <= CS0_SIZE)) {
        return CMEM_NOR_Construct_RegionInfo_Internal(baseaddr, length, regioninfo, BlockLayout);
    }
    //FAT in second SF
    else if ((baseaddr & SFC_GENERIC_0x0FFFFFFF_MASK) >= CS0_SIZE) {
        return CMEM_NOR_Construct_RegionInfo_Internal(baseaddr - CS0_SIZE, length, regioninfo, BlockLayout_CS1);
    }
    //FAT in SF1 and SF2
    else {
        CMEM_NOR_Construct_RegionInfo_Internal(baseaddr, length - (((baseaddr + length) & SFC_GENERIC_0x0FFFFFFF_MASK) - CS0_SIZE), regioninfo, BlockLayout);
        return CMEM_NOR_Construct_RegionInfo_Internal(0, ((baseaddr + length) & SFC_GENERIC_0x0FFFFFFF_MASK) - CS0_SIZE, regioninfo, BlockLayout_CS1);
    }
#else
    return CMEM_NO_ERROR;
#endif
}


//-----------------------------------------------------------------------------
/*!
  @brief
    Construct region info by combo MCP block info
  @remarks
    For EMI init stage only, it will compare ID and match the correct settings.
*/
#if defined(__SERIAL_FLASH_EN__)
int32_t EMIINIT_CMEM_NOR_Construct_RegionInfo(uint32_t baseaddr, uint32_t length, FlashRegionInfo *regioninfo)
{
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    uint32_t index = CMEM_EMIINIT_Index(0);
#else
    uint32_t index = CMEM_EMIINIT_Index();
#endif
    const CMEMEntryNOR *list = NULL;

    list = combo_mem_sw_list.m_data.List;

    return CMEM_NOR_Construct_RegionInfo_Internal(baseaddr, length, regioninfo, list[index].BlockLayout);
}
//-----------------------------------------------------------------------------
/*!
  @brief
    Construct region info by combo MCP block info
  @remarks
    For EMI init stage only, it will compare ID and match the correct settings.
*/

int32_t EMIINIT_CMEM_NOR_Construct_DualRegionInfo(uint32_t baseaddr, uint32_t length, FlashRegionInfo *regioninfo)
{
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)&& !defined(__MINI_BOOTLOADER__)
    uint32_t index, i = 0;
    const CMEMEntryNOR *list = NULL;
    //FAT in first SF
    if (((baseaddr & SFC_GENERIC_0x0FFFFFFF_MASK) < EMINIT_CMEM_GET_NOR_FLASH_SIZE(0)) && (((baseaddr + length) & SFC_GENERIC_0x0FFFFFFF_MASK) <= EMINIT_CMEM_GET_NOR_FLASH_SIZE(0))) {
        index = CMEM_EMIINIT_Index(0);
        list = combo_mem_sw_list.m_data.List;
        return CMEM_NOR_Construct_RegionInfo_Internal(baseaddr, length, regioninfo, list[index].BlockLayout);
    }
    //FAT in second SF
    else if ((baseaddr & SFC_GENERIC_0x0FFFFFFF_MASK) >= EMINIT_CMEM_GET_NOR_FLASH_SIZE(0)) {
        index = CMEM_EMIINIT_Index(1);
        list = combo_mem_sw_list.m_data.List;
        return CMEM_NOR_Construct_RegionInfo_Internal(baseaddr - EMINIT_CMEM_GET_NOR_FLASH_SIZE(0), length, regioninfo, list[index].BlockLayout);
    }
    //FAT in SF1 and SF2
    else {
        for (; i < CS_COUNT; i++) {
            index = CMEM_EMIINIT_Index(i);
            list = combo_mem_sw_list.m_data.List;
            if (i == 0) {
                CMEM_NOR_Construct_RegionInfo_Internal(baseaddr, length - (((baseaddr + length) & SFC_GENERIC_0x0FFFFFFF_MASK) - EMINIT_CMEM_GET_NOR_FLASH_SIZE(0)), regioninfo, list[index].BlockLayout);
            } else {
                return CMEM_NOR_Construct_RegionInfo_Internal(0 , ((baseaddr + length) & SFC_GENERIC_0x0FFFFFFF_MASK) - EMINIT_CMEM_GET_NOR_FLASH_SIZE(0), regioninfo, list[index].BlockLayout);
            }
        }
    }
#else
    return CMEM_NO_ERROR;
#endif
}


//-----------------------------------------------------------------------------
/*!
  @brief
    Retrieve Uniform block size
  @remarks
    For EMI init stage only, it will compare ID and match the correct settings.
*/
uint32_t EMIINIT_CMEM_NOR_GetUniformBlock(const uint16_t CS)
{
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    uint32_t index = CMEM_EMIINIT_Index(CS);
#else
    uint32_t index = CMEM_EMIINIT_Index();
#endif
    const CMEMEntryNOR *list = NULL;

    list = combo_mem_sw_list.m_data.List;

    return list[index].UniformBlocks;
}
#endif //__SERIAL_FLASH_EN__

//-----------------------------------------------------------------------------
/*!
  @brief
    This function is used to construct bank info, and the caller is
    CMEM_NOR_Construct_BankInfo()
*/
void CMEM_NOR_AddBankInfo(FlashBankInfo *bankInfo, uint32_t *index, uint32_t bankSize)
{
    if (bankInfo[*index].BankSize == bankSize) {
        bankInfo[*index].Banks ++;
    } else {
        (*index) ++;
        bankInfo[*index].Banks = 1;
        bankInfo[*index].BankSize = bankSize;
    }

}

//-----------------------------------------------------------------------------
/*!
  @brief
    Construct bank info of the given address/lenth according to the
    combo MCP bank info
*/
uint32_t CMEM_NOR_Construct_BankInfo(
    uint32_t baseaddr,
    uint32_t length,
    FlashBankInfo *bankInfo,
    const FlashBankInfo *cmemBankInfo)
{
    uint32_t Partition = 0;
    int32_t Index = 0;
    uint32_t bankSize;
    uint32_t NextBankAddr, srcBankIdx;

    memset(bankInfo, 0, sizeof(FlashBankInfo)*CMEM_BANK_INFO_LEN);

    for (srcBankIdx = 0, NextBankAddr = 0; length > 0; srcBankIdx++) {
        if (srcBankIdx >= cmemBankInfo[Partition].Banks) {
            srcBankIdx = 0; //BankIndex in current partition
            Partition++;
        }

        NextBankAddr += cmemBankInfo[Partition].BankSize;

        if (baseaddr <= NextBankAddr) {
            bankSize = NextBankAddr - baseaddr;
            if (length < bankSize) {
                bankSize = length;
            }

            //Add One Bank Info
            if (bankInfo[Index].BankSize == bankSize) {
                bankInfo[Index].Banks ++;
            } else {
                if (bankInfo[Index].BankSize != 0) {
                    Index++;
                }
                bankInfo[Index].Banks = 1;
                bankInfo[Index].BankSize = bankSize;
            }

            baseaddr += bankSize;
            length -= bankSize;
        }

    }


    return CMEM_NO_ERROR;
}



#if !((defined(__FUE__) && defined(__FOTA_DM__)) || defined(__UBL__))

//-----------------------------------------------------------------------------
/*!
  @brief
    NOR Flash initialization of MAUI
*/
int32_t CMEM_Init_NOR(void)
{
    int32_t result;
    int32_t i;

    // Total Blocks := Blocks in Region Info
    // Block Size := MAX(RegionInfo[i].BlockSize)
    BlockLayout = nor_list[cmem_nor_index].BlockLayout;
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    /* It is possible that we combo (256,0) and (128,128) dual SF pair, if SF1's size equals to total dual SF size,*/
    /* We need to search for MTK_DUMMY_PART_NUMBER's(ID: 0x00,0x34,0x56) entry number*/
    if (nor_list[cmem_nor_index].BankInfo->BankSize == DUAL_NOR_FLASH_SIZE) {
        cmem_nor_index_CS1 = CMEM_Index(2);
        if (nor_list[cmem_nor_index_CS1].BankInfo->BankSize != 0x0) {
            ASSERT_RET(0, 0);    //return CMEM_ERR_ID_NOT_FOUND;
        }
    }
    CS0_SIZE = nor_list[cmem_nor_index].BankInfo->BankSize;
    CS1_SIZE = nor_list[cmem_nor_index_CS1].BankInfo->BankSize;
    if ((CS0_SIZE + CS1_SIZE) != DUAL_NOR_FLASH_SIZE) {
        ASSERT_RET(0, 0);    //SF1 + SF2 size must equal to DUAL_NOR_FLASH_SIZE in hal_flash_opt_gen.h
    }

    BlockLayout_CS1 = nor_list[cmem_nor_index_CS1].BlockLayout;
#endif
#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
    CMEM_NOR_Construct_BankInfo(custom_get_fat_addr(), custom_get_fat_len(), BankInfo, nor_list[cmem_nor_index].BankInfo);
#else
    BankInfo = nor_list[cmem_nor_index].BankInfo;
#endif

    // construct RegionInfo from block info and bank info
#if !defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    result = NOR_Construct_RegionInfo(custom_get_fat_addr(), custom_get_fat_len(), RegionInfo);
#else
    result = NOR_Construct_DualRegionInfo(custom_get_fat_addr(), custom_get_fat_len(), RegionInfo);
#endif

    if (result != CMEM_NO_ERROR)   {
        return result;
    }

    for (i = 0, TOTAL_BLOCKS = 0, NOR_LARGEST_BLOCK_SIZE = 0 ; RegionInfo[i].BlockSize != 0; i++)    {
        TOTAL_BLOCKS += RegionInfo[i].RegionBlocks;
        if (RegionInfo[i].BlockSize > NOR_LARGEST_BLOCK_SIZE) {
            NOR_LARGEST_BLOCK_SIZE = RegionInfo[i].BlockSize;
        }
    }

    // 1. FDM data/interface initialization
    //result=CMEM_Init_NOR_FDM_Common();
    //if (result!=CMEM_NO_ERROR) return result;

    switch (nor_list[cmem_nor_index].FDMType)    {
#if defined(__INTEL_SIBLEY__)
        case CMEM_NOR_CS_INTEL_SIBLEY:
            PAGE_SIZE = 0x400;
            result = CMEM_Init_NOR_FDM_SIB();
            break;
#endif
#if defined(__NON_INTEL_SIBLEY__)
        default:
            PAGE_SIZE = 0x200;
            //result=CMEM_Init_NOR_FDM_NonSIB();
            break;
#endif
#if !(defined(__INTEL_SIBLEY__) || defined(__NON_INTEL_SIBLEY__))
        default:
            result = CMEM_ERR_FDM_MISMATCH;
            break;
#endif
    }

    // Total Sectors := Flash Area / Page Size
#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
    TOTAL_SECTORS = ((NOR_ALLOCATED_FAT_SPACE - NOR_LARGEST_BLOCK_SIZE * SNOR_ERASE_QUEUE_SIZE) / PAGE_SIZE);
#else
    TOTAL_SECTORS = (NOR_ALLOCATED_FAT_SPACE / PAGE_SIZE);
#endif

    if (result != CMEM_NO_ERROR) {
        return result;
    }

    // 2. MTD data/interface initialization

    result = CMEM_Init_NOR_MTD_Common();
    if (result != CMEM_NO_ERROR) {
        return result;
    }

    switch (cmem_id->DeviceType) {
        case CMEM_TYPE_SERIAL_NOR_FLASH:
            result = CMEM_Init_NOR_MTD_SF();
            break;
        default:
            result = CMEM_ERR_MTD_MISMATCH;
            break;
    }

    if (result != CMEM_NO_ERROR) {
        return result;
    }


    return result;
}

#endif // !((defined(__FUE__) && defined(__FOTA_DM__)) || defined(__UBL__))


//-----------------------------------------------------------------------------
/*!
  @brief
    Compare device ID and retrieve the correspond NOR flash settings
*/
void CMEM_Init_nor_list(const uint32_t CS)
{
    if (CS == 0) {
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
        cmem_nor_index = CMEM_Index(CS);
#else
        cmem_nor_index = CMEM_Index();
#endif
        if (cmem_nor_index < 0) {
            ASSERT(0);    //return CMEM_ERR_ID_NOT_FOUND;
        }
        cmem_id = CMEM_GetIDEntry(cmem_nor_index);
    }
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    else {
        cmem_nor_index_CS1 = CMEM_Index(CS);
        if (cmem_nor_index_CS1 < 0) {
            ASSERT(0);    //return CMEM_ERR_ID_NOT_FOUND;
        }
        cmem_id_CS1 = CMEM_GetIDEntry(cmem_nor_index_CS1);
    }
#endif

    nor_list = combo_mem_sw_list.m_data.List;
}


//-----------------------------------------------------------------------------
// MCP Initialization Function
//-----------------------------------------------------------------------------
#if !((defined(__FUE__) && defined(__FOTA_DM__)) || defined(__UBL__))
//-----------------------------------------------------------------------------
/*!
  @brief
    Combo MCP intialization function for MAUI
  @param[in] type The type of initialization (reserved)
  @retval
    CMEM_NO_ERROR: Success
    otherwise: Fail
*/
int32_t ComboMem_Initialize(void)
{
#if !defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    CMEM_Init_nor_list(0);
    return CMEM_Init_NOR();
#else
    CMEM_Init_nor_list(0);
    CMEM_Init_nor_list(1);
    return CMEM_Init_NOR();
#endif // !defined(__DUAL_SERIAL_FLASH_SUPPORT__)
}
#endif // !((defined(__FUE__) && defined(__FOTA_DM__)) || defined(__UBL__))



//-----------------------------------------------------------------------------
/*!
  @brief
    Full driver  initialization Entry Function
  @remarks
    EntireFlashDisk (SecurDisk, Card Download)
*/
int32_t CMEM_Init_FullDriver(void)
{
    int32_t result;
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    uint8_t i = 0;
    int32_t temp;
#endif
    CMEM_Init_nor_list(0);
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    CMEM_Init_nor_list(1);
#endif

    PAGE_BUFFER_SIZE = nor_list[cmem_nor_index].PageBufferSize;
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__) && !defined(__MINI_BOOTLOADER__)// IF SF1's program page size > SF2. Then PAGE_BUFFER_SIZE should get SF2's page size.
    /* It is possible that we combo (256,0) and (128,128) dual SF pair, if SF1's size equals to total dual SF size,*/
    /* We need to search for MTK_DUMMY_PART_NUMBER's(ID: 0x00,0x34,0x56) entry number*/
    if (nor_list[cmem_nor_index].BankInfo->BankSize == DUAL_NOR_FLASH_SIZE) {
        cmem_nor_index_CS1 = CMEM_Index(2);
        if (nor_list[cmem_nor_index_CS1].BankInfo->BankSize != 0x0) {
            ASSERT_RET(0, 0);    //return CMEM_ERR_ID_NOT_FOUND;
        }
    }

    if (PAGE_BUFFER_SIZE > nor_list[cmem_nor_index_CS1].PageBufferSize) {
        PAGE_BUFFER_SIZE = nor_list[cmem_nor_index_CS1].PageBufferSize;
    }
#endif

    BlockLayout = nor_list[cmem_nor_index].BlockLayout;
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)&& !defined(__MINI_BOOTLOADER__)
    CS0_SIZE = nor_list[cmem_nor_index].BankInfo->BankSize;
    CS1_SIZE = nor_list[cmem_nor_index_CS1].BankInfo->BankSize;
    if ((CS0_SIZE + CS1_SIZE) != DUAL_NOR_FLASH_SIZE) {
        ASSERT_RET(0, 0);    //SF1 + SF2 size must equal to DUAL_NOR_FLASH_SIZE in hal_flash_opt_gen.h
    }

    BlockLayout_CS1 = nor_list[cmem_nor_index_CS1].BlockLayout;
#endif

#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
    memcpy(BankInfo, nor_list[cmem_nor_index].BankInfo, sizeof(FlashBankInfo)*CMEM_REGION_INFO_LEN);
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)&& !defined(__MINI_BOOTLOADER__)
    /* Serial Flash is single bank, add SF1 and SF2's bnak size. Treat them as one single bank. */
    BankInfo[0].BankSize += nor_list[cmem_nor_index_CS1].BankInfo->BankSize;
#endif
#else
    BankInfo = nor_list[cmem_nor_index].BankInfo;
#endif

    // Allocate status map and command for MTD data
    EntireDiskMtdData.StatusMap = &StatusMap[0];
#ifdef __SERIAL_FLASH__
    EntireDiskMtdData.CMD = &sf_dal_data_cmd;
    EntireDiskMtdData.CS = 0;
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    EntireDiskMtdData.CS_Size = CS0_SIZE; //Assign first SF's size, this field will be used in program/erase function wrapper in flash_mtd_sf_dal.c
#endif
#if defined(__NOR_FDM5__) // Assign FDM5.0 LayoutInfo to avoid fatal error happens in mtd mountDevice().
    LayoutInfo.TotalBlks =   NOR_TOTAL_BLOCKS;
    LayoutInfo.BlkSize = NOR_BLOCK_SIZE;
    LayoutInfo.PageSize = NOR_PAGE_SIZE;
    LayoutInfo.TotalLSMT = NOR_TOTAL_LSMT;
    EntireDiskMtdData.LayoutInfo = (NORLayoutInfo *)&LayoutInfo;
#endif
#else
    EntireDiskMtdData.CMD = &pf_dal_data_cmd;
#endif

    EntireDiskMtdData.Signature = ~((uint32_t)EntireDiskRegionInfo);

    EntireDiskMtdData.BaseAddr = (BYTE *)SFC_GENERIC_FLASH_BANK_MASK;

#if defined(__SERIAL_FLASH__)
    // serial Flash driver will initialize the interface/data by its own.
    result = SF_DAL_Init_Driver(
                 &NORFlashMtd,            // Driver Interface (to FDM)
                 &EntireDiskMtdData,      // Driver Data
                 (uint32_t)EntireDiskMtdData.BaseAddr,   // Base Address
                 nor_list[cmem_nor_index].UniformBlocks);                 // Uniform Block
#else //ADMUX flash

    result = ADMUX_DAL_Init_Driver(
                 &NORFlashMtd,            // Driver Interface (to FDM)
                 &EntireDiskMtdData,      // Driver Data
                 (uint32_t)EntireDiskMtdData.BaseAddr,   // Base Address
                 nor_list[cmem_nor_index].FDMType);                      // Reserved: Driver Type

#endif // __SERIAL_FLASH__

    ASSERT_RET(result != FS_FLASH_MOUNT_ERROR, 0);

#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)&& !defined(__MINI_BOOTLOADER__)
    for (; i < CS_COUNT; i++) {
        DualMtdflash[i].CS = i;
        // Allocate status map and command for MTD data
        DualMtdflash[i].StatusMap = &DualStatusMap[i][0];

        DualMtdflash[i].CMD = &Dual_sf_dal_data_cmd[i];

        DualMtdflash[i].Signature = ~((uint32_t)EntireDiskRegionInfo);

        DualMtdflash[i].BaseAddr = (BYTE *)INT_RetrieveFlashBaseAddr();

        if (i == 0) {
            DualMtdflash[i].CS_Size = CS0_SIZE;
            temp = cmem_nor_index;
        } else {
            DualMtdflash[i].CS_Size = CS1_SIZE;
            temp = cmem_nor_index_CS1;
        }
        if (DualMtdflash[i].CS_Size != 0x0) { //If SF2 is Dummy, do not do Driver Init
            // serial Flash driver will initialize the interface/data by its own.
            result = SF_DAL_Init_Driver(
                         &NORFlashMtd,            // Driver Interface (to FDM)
                         &DualMtdflash[i],      // Driver Data
                         (uint32_t)DualMtdflash[i].BaseAddr,   // Base Address
                         nor_list[temp].UniformBlocks);                 // Uniform Block

            ASSERT_RET(result != FS_FLASH_MOUNT_ERROR, 0);
        }
    }
#endif // defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    EntireDiskDriveData.DiskSize = NOR_Get_FlashSizeFromBankInfo(BankInfo);

#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    NOR_Construct_DualRegionInfo(0, EntireDiskDriveData.DiskSize, EntireDiskRegionInfo);
#else
    NOR_Construct_RegionInfo(0, EntireDiskDriveData.DiskSize, EntireDiskRegionInfo);
#endif
    EntireDiskMtdData.RegionInfo = (FlashRegionInfo *)EntireDiskRegionInfo;
#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)&& !defined(__MINI_BOOTLOADER__)
    DualMtdflash[0].RegionInfo = (FlashRegionInfo *)EntireDiskRegionInfo;
    DualMtdflash[1].RegionInfo = (FlashRegionInfo *)EntireDiskRegionInfo;
#endif

    EntireDiskDriveData.MTDDriver = &NORFlashMtd;
    EntireDiskDriveData.MTDData = &EntireDiskMtdData;
    EntireDiskDriveData.is_mount = false;

    return CMEM_NO_ERROR;
}


#endif // (__COMBO_MEMORY_SUPPORT__) || defined(__SERIAL_FLASH__)

#endif //#ifdef HAL_FLASH_MODULE_ENABLED



