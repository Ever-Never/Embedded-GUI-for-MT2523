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

#define FLASHCONF_C

#include "hal_flash_general_types.h"
#include "hal_flash_fs_type.h"
#include "hal_flash_custom_memorydevice.h"
#include "hal_flash_sf.h"
#include "string.h"

#ifdef FLASH_DISKDRV_DEBUG
#include <stdlib.h>
#endif

#include "hal_flash_opt.h"
#include "hal_flash_drvflash.h"
#include "hal_flash_disk.h"
#include "hal_flash_disk_internal.h"
#include "hal_flash_combo_init.h"

#include "hal_flash_custom.h"

#include "hal_flash_mtd_ut.h"  //__BASIC_LOAD_FLASH_TEST__

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

/*
 ****************************************************************************
 PART 1:
 Rules to Prevent Wrong Configuration
 ****************************************************************************
*/

#if defined(__SINGLE_BANK_NOR_DEVICE__)

#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
#else
#error "custom\system\{project}\hal_flash_custom_memorydevice.h: Error! Single Bank device with non Single Bank load! Please modify device or makefile option!"
#endif /* __SINGLE_BANK_NOR_FLASH_SUPPORT__ */

#endif /* __SINGLE_BANK_NOR_DEVICE__ */


/*
 ****************************************************************************
 PART 2:
 Essential Information of NOR Flash Geometry Layout Information
 ****************************************************************************
*/
/*******************************************************************************
   NOTICE: Fill the flash region information table, a region is the memory space
           that contains continuous sectors of equal size. Each region element
           in the table is the format as below:
           {S_sector, N_sector},
               S_sector: the size of sector in the region
               N_sector: the number of sectors in the region
 *******************************************************************************/

#if !(defined(__FUE__) || defined(__UBL__) || defined(__EXT_BOOTLOADER__))
#define __MAUI_LOAD__
#endif //!__FUE__ && !__UBL__ && !__EXT_BOOTLOADER__

#define _NOR_FLASH_BOOTING_

#ifdef _NOR_FLASH_BOOTING_

#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
#define FLASH_REGIONINFO_VAR_MODIFIER  static
#else /* __SINGLE_BANK_NOR_FLASH_SUPPORT__ */
#define FLASH_REGIONINFO_VAR_MODIFIER  static const
#endif /* __SINGLE_BANK_NOR_FLASH_SUPPORT__ */

#if (defined(__MAUI_LOAD__) && defined(_NOR_FLASH_BOOTING_) && !defined(__NOR_FDM5__))
#define __MAUI_NOR_FDM4__
#endif

/* Serial Flash with FDM5.0 enable need RegionInfo for Cksysdrv to check system drive */
#if defined(__MAUI_NOR_FDM4__) || (defined(__SERIAL_FLASH_EN__) && defined(__NOR_FDM5__))
#if 0
FLASH_REGIONINFO_VAR_MODIFIER FlashRegionInfo RegionInfo[] = /* Don't modify this line */
{
    REGION_INFO_LAYOUT
    EndRegionInfo /* Don't modify this line */
};
#endif
#endif //__MAUI_NOR_FDM4__ 

#if (!defined(__COMBO_MEMORY_SUPPORT__) && !defined(__SERIAL_FLASH__))
#ifdef __NOR_SUPPORT_RAW_DISK__

#ifndef NOR_BOOTING_NOR_DISK_NUM
#error "custom\system\{project}\hal_flash_custom_memorydevice.h: Error! NOR_BOOTING_NOR_DISK_NUM be defined when __NOR_SUPPORT_RAW_DISK__ is defined."
#endif //NOR_BOOTING_NOR_DISK_NUM

#if (NOR_BOOTING_NOR_DISK_NUM>0)
FLASH_REGIONINFO_VAR_MODIFIER FlashRegionInfo Disk0RegionInfo[] = /* Don't modify this line */
{
    DISK0_REGION_INFO_LAYOUT
    EndRegionInfo /* Don't modify this line */
};
#endif /* NOR_BOOTING_NOR_DISK_NUM>0 */

#if (NOR_BOOTING_NOR_DISK_NUM>1)
FLASH_REGIONINFO_VAR_MODIFIER FlashRegionInfo Disk1RegionInfo[] = /* Don't modify this line */
{
    DISK1_REGION_INFO_LAYOUT
    EndRegionInfo /* Don't modify this line */
};
#endif /* NOR_BOOTING_NOR_DISK_NUM>1 */
#endif // __NOR_SUPPORT_RAW_DISK__

FLASH_REGIONINFO_VAR_MODIFIER FlashRegionInfo EntireDiskRegionInfo[] = /* Don't modify this line */
{
    ENTIRE_DISK_REGION_INFO_LAYOUT
    EndRegionInfo /* Don't modify this line */
};

NOR_FLASH_DISK_Data EntireDiskDriveData;
static NOR_Flash_MTD_Data EntireDiskMtdData;

/*******************************************************************************
   NOTICE. Modify the value of page buffer size in WORD for page buffer program
 *******************************************************************************/
uint32_t PAGE_BUFFER_SIZE = BUFFER_PROGRAM_ITERATION_LENGTH;

/*******************************************************************************
   NOTICE. This is for the Enhanced Signle Bank Support, when this feature is
           turned on and still use multi-bank device, this table should be filled
           with correct value.

           This Table define the flash bank information which starts from
           FLASH_BASE_ADDRESS, please fill the flash bank information table, every
           entry defines the memory space that contains continuous banks of equal size.
           Each entry element in the table is the format as below:
           {S_Bank, N_Bank},
               S_Bank: the size of bank in the entry
               N_Bank: the number of banks in the entry
 *******************************************************************************/
#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__) && defined(__MULTI_BANK_NOR_DEVICE__)

#ifdef __MAUI_LOAD__
FLASH_REGIONINFO_VAR_MODIFIER FlashBankInfo BankInfo[] = {
    BANK_INFO_LAYOUT
    EndBankInfo /* Don't modify this line */
};
#endif //__MAUI_LOAD__

#endif //__SINGLE_BANK_NOR_FLASH_SUPPORT__ && __MULTI_BANK_NOR_DEVICE__
#endif /* !__COMBO_MEMORY_SUPPORT__ && !__SERIAL_FLASH__ */

#endif // _NOR_FLASH_BOOTING_

/*
 ****************************************************************************
 PART 3:
 Essential Declarations for NOR-Flash Disk
 ****************************************************************************
*/


/*
 ****************************************************************************
 PART 4:
 Public Functions For NOR Flash Information Retrieve, Initial routine, and
 other misc routines.
 ****************************************************************************
*/
uint32_t custom_get_NORFLASH_Base(void);

/*
 ****************************************************************************
 PART 5:
 Essential Declarations for NAND-Flash Disk
 ****************************************************************************
*/

/*************************************************************************
* FUNCTION
*  custom_get_NORFLASH_ROMSpace()
*
* DESCRIPTION
*  Query the of space configured for NORFLASH ROM
*
* PARAMETERS
*
* RETURNS
*  BASE ADDRESS
*
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t
custom_get_NORFLASH_ROMSpace(void)
{
    return NOR_FLASH_BASE_ADDRESS;
}


/*************************************************************************
* FUNCTION
*  custom_get_NORFLASH_Size()
*
* DESCRIPTION
*  Query the size of NORFLASH ROM
*
* PARAMETERS
*
* RETURNS
*  SIZE
*
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t
custom_get_NORFLASH_Size(void)
{
    return NOR_FLASH_SIZE;

}

/*************************************************************************
* FUNCTION
*  Initialize_FDD_tables
*
* DESCRIPTION
*  Initialize important information for NOR-flash disk
*
* PARAMETERS
*
* RETURNS
*  None
*
* GLOBALS AFFECTED
*
*************************************************************************/
#if defined(__MAUI_LOAD__)

#if ((defined(__COMBO_MEMORY_SUPPORT__) || defined(__SERIAL_FLASH__)) && defined(_NOR_FLASH_BOOTING_))
void Initialize_FDD_tables(void)
{
    ComboMem_Initialize();
    return;
}

#else /* (!(__COMBO_MEMORY_SUPPORT__) && !(__SERIAL_FLASH__)) && !(_NOR_FLASH_BOOTING_) */

#endif /* !(__COMBO_MEMORY_SUPPORT__) && !(__SERIAL_FLASH__) */

void nor_sweep_device(void)
{
#ifdef __NOR_FDM5__
    kal_set_eg_events(nor_egid, NOR_DMAN_EVENT, KAL_OR);
#endif
    return;
}

void nor_manual_reclaim(void)
{
#ifdef __NOR_FDM5__
    kal_set_eg_events(nor_egid, NOR_BRECL_EVENT, KAL_OR);
#endif
    return;
}

/*************************************************************************
* FUNCTION
*  custom_get_fat_addr()
*
* DESCRIPTION
*  This function gets the start address of FAT.
*
* PARAMETERS
*  none
*
* RETURNS
*  FAT start address
*
*************************************************************************/
uint32_t custom_get_fat_addr()
{
    return NOR_FLASH_BASE_ADDRESS;
}

/*************************************************************************
* FUNCTION
*  custom_get_fat_len()
*
* DESCRIPTION
*  This function gets the len of FAT.
*
* PARAMETERS
*  none
*
* RETURNS
*  FAT length
*
*************************************************************************/
uint32_t custom_get_fat_len()
{
    return NOR_ALLOCATED_FAT_SPACE;
}
#endif // __MAUI_LOAD__

/*************************************************************************
* FUNCTION
*  Custom_NOR_Init
*
* DESCRIPTION
*  Initialize important information for NOR-flash disk
*
* PARAMETERS
*
* RETURNS
*  None
*
* GLOBALS AFFECTED
*
*************************************************************************/
void Custom_NOR_Init(void)
{
    CMEM_Init_FullDriver();
}


/*************************************************************************
* FUNCTION
*  custom_get_NORFLASH_Base()
*
* DESCRIPTION
*  Query the of space configured for NORFLASH ROM
*
* PARAMETERS
*
* RETURNS
*  BASE ADDRESS
*
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t custom_get_NORFLASH_Base(void)
{
    return SFC_GENERIC_FLASH_BANK_MASK;
}


/*************************************************************************
* FUNCTION
*  custom_get_FLASH_Size()
*
* DESCRIPTION
*  Query the of space configured for ROM
*
* PARAMETERS
*
* RETURNS
*  BASE ADDRESS
*
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t custom_get_FLASH_Size(void)
{
    return NOR_FLASH_SIZE;
}

#endif //#ifdef HAL_FLASH_MODULE_ENABLED

