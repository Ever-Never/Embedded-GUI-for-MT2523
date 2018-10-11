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

#include "hal_flash_custom_memorydevice.h"
//#include "custom_FeatureConfig.h"
#include "hal_flash_opt_gen.h"
#if defined(__SECURE_DATA_STORAGE__)
#include "custom_secure_config.h"
#endif

/*
 *******************************************************************************
 PART 1:
   FLASH CONFIG Options Definition here
 *******************************************************************************
*/
#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
#ifdef NOR_FLASH_TYPE_RENESAS_SERIES
#error "custom\system\{project}\hal_flash_custom_memorydevice.h: Error! Enhanced single bank(ESB) does not support Renesas series flash! Please change device!"
#endif
#endif

/*
 *******************************************************************************
 PART 2:
   FLASH FDM FEATURE CONFIG PARAMETERS translated from Manual custom_Memorydevice.h
 *******************************************************************************
*/
#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)

/* NOTE:
   BUFFER_PROGRAM_ITERATION_LENGTH specifies the maximum interrupt
   disabled period while buffer program operation is applied
*/

/* NOTE:
   ERASE_QUEUE_SIZE specifies the number of reserved blocks,
   to trade off between write throughput and available FAT space
*/
#ifndef NOR_FDM4_ESB_PARAMETER_ERASE_QUEUE_SIZE
#define SNOR_ERASE_QUEUE_SIZE   (5)
#else  /* NOR_FDM4_ESB_PARAMETER_ERASE_QUEUE_SIZE */
#define SNOR_ERASE_QUEUE_SIZE   (NOR_FDM4_ESB_PARAMETER_ERASE_QUEUE_SIZE)
#endif /* NOR_FDM4_ESB_PARAMETER_ERASE_QUEUE_SIZE */

#else  /* !__SINGLE_BANK_NOR_FLASH_SUPPORT__ */
#ifndef NOR_PARAMETER_SYSTEM_DRIVE_RESERVED_BLOCK
#define NOR_SYSTEM_DRIVE_RESERVED_BLOCK   (3)
#else
#define NOR_SYSTEM_DRIVE_RESERVED_BLOCK   (NOR_PARAMETER_SYSTEM_DRIVE_RESERVED_BLOCK)
#endif

#endif /* __SINGLE_BANK_NOR_FLASH_SUPPORT__ */


/*
 *******************************************************************************
 PART 3:
   FLASH GEOMETRY translated from MEMORY DEVICE DATABASE
 *******************************************************************************
*/


/*
 *******************************************************************************
 PART 4:
   FLASH FAT CONFIG translated from Manual custom_Memorydevice.h
 *******************************************************************************
*/

/* Generic NOR-XIP Case */

/*-----------------------------------------------------------------------------------------
 * FS base address is automatically adjusted with update package storage size as
 * NOR_BOOTING_NOR_FS_BASE_ADDRESS + FOTA_PACKAGE_STORAGE_SIZE (defined in custom_img_config.h)
 *-----------------------------------------------------------------------------------------*/

#ifndef NOR_BOOTING_NOR_FS_BASE_ADDRESS
#define NOR_FLASH_BASE_ADDRESS     (NOR_FLASH_BASE_ADDRESS_DEFAULT + FOTA_DM_FS_OFFSET)
#else
#define NOR_FLASH_BASE_ADDRESS     (NOR_BOOTING_NOR_FS_BASE_ADDRESS + FOTA_DM_FS_OFFSET)
#endif   /* !NOR_FLASH_BASE_ADDRESS */

/*-----------------------------------------------------------------------------------------
 * FS size is automatically decreased by update package storage size as
 * NOR_BOOTING_NOR_FS_SIZE - FOTA_PACKAGE_STORAGE_SIZE (defined in custom_img_config.h)
 *-----------------------------------------------------------------------------------------*/
#ifdef __NVRAM_BACKUP_PARTITION__
#include "nvram_lid_statistics.h"

#ifndef NOR_BOOTING_NOR_DISK_NUM
#define NOR_BOOTING_NOR_DISK_NUM   (1)
#endif

#ifndef NOR_BOOTING_NOR_DISK0_SIZE
#define NOR_BOOTING_NOR_DISK0_SIZE  ((NVRAM_BACKUP_PARTITION_SIZE + NOR_DISK0_BLOCK_SIZE - 1) & ~(NOR_DISK0_BLOCK_SIZE - 1))
#else
#if NOR_BOOTING_NOR_DISK0_SIZE < ((NVRAM_BACKUP_PARTITION_SIZE + NOR_DISK0_BLOCK_SIZE - 1) & ~(NOR_DISK0_BLOCK_SIZE - 1))
#error "Backup partition size is not enough, please modify custom_memoryDevice.h and enlarge NOR_BOOTING_NOR_DISK0_SIZE."
#endif
#endif

#ifndef NOR_BOOTING_NOR_DISK0_BASE_ADDRESS
#define NOR_BOOTING_NOR_DISK0_BASE_ADDRESS  (NOR_FLASH_BASE_ADDRESS_DEFAULT + NOR_ALLOCATED_FAT_SPACE_DEFAULT - NOR_BOOTING_NOR_DISK0_SIZE)
#endif



#if   defined(NOR_BOOTING_NOR_FS_SIZE) && (NOR_FLASH_BASE_ADDRESS + NOR_BOOTING_NOR_FS_SIZE > NOR_BOOTING_NOR_DISK0_BASE_ADDRESS)
#define FS_BKP_OVERLAP_SIZE (NOR_FLASH_BASE_ADDRESS + NOR_BOOTING_NOR_FS_SIZE - NOR_BOOTING_NOR_DISK0_BASE_ADDRESS)
#elif !defined(NOR_BOOTING_NOR_FS_SIZE) && (NOR_FLASH_BASE_ADDRESS + NOR_ALLOCATED_FAT_SPACE_DEFAULT > NOR_BOOTING_NOR_DISK0_BASE_ADDRESS )
#define FS_BKP_OVERLAP_SIZE (NOR_FLASH_BASE_ADDRESS + NOR_ALLOCATED_FAT_SPACE_DEFAULT - NOR_BOOTING_NOR_DISK0_BASE_ADDRESS)
#else
#define FS_BKP_OVERLAP_SIZE 0
#endif

#ifdef DISK0_REGION_INFO_LAYOUT
#undef DISK0_REGION_INFO_LAYOUT
#endif

#define DISK0_REGION_INFO_LAYOUT {(NOR_FLASH_BASE_ADDRESS_DEFAULT + NOR_ALLOCATED_FAT_SPACE_DEFAULT - ((NVRAM_BACKUP_PARTITION_SIZE + NOR_BLOCK_SIZE - 1) & ~(NOR_BLOCK_SIZE - 1))), NOR_DISK0_BLOCK_SIZE}
#endif /* __NVRAM_BACKUP_PARTITION__ */



#if defined(__SECURE_DATA_STORAGE__)

//Current FAT end address: NOR_FLASH_BASE_ADDRESS + NOR_BOOTING_NOR_FS_SIZE
//Expect FAT end address : NOR_FLASH_BASE_ADDRESS + NOR_ALLOCATED_FAT_SPACE_DEFAULT - SDS_TOTAL_SIZE

#define NOR_SDS_PARTITION_BASE_ADDRESS (NOR_FLASH_BASE_ADDRESS_DEFAULT + NOR_ALLOCATED_FAT_SPACE_DEFAULT - SDS_TOTAL_SIZE)


//if Current address > Expect FAT
//Overlap = Current address - Expect FAT

#if defined(NOR_BOOTING_NOR_FS_SIZE) && (NOR_FLASH_BASE_ADDRESS + NOR_BOOTING_NOR_FS_SIZE > NOR_SDS_PARTITION_BASE_ADDRESS)
#define FS_SDS_OVERLAP_SIZE (NOR_FLASH_BASE_ADDRESS + NOR_BOOTING_NOR_FS_SIZE - NOR_SDS_PARTITION_BASE_ADDRESS)
#elif !defined(NOR_BOOTING_NOR_FS_SIZE)
#define FS_SDS_OVERLAP_SIZE SDS_TOTAL_SIZE
#else
#define FS_SDS_OVERLAP_SIZE 0
#endif

#endif //__SECURE_DATA_STORAGE__


#ifndef NOR_BOOTING_NOR_FS_SIZE
#if defined(__SECURE_DATA_STORAGE__)
#define NOR_ALLOCATED_FAT_SPACE    (NOR_ALLOCATED_FAT_SPACE_DEFAULT - FOTA_DM_FS_OFFSET - FS_SDS_OVERLAP_SIZE)
#elif defined(__NVRAM_BACKUP_PARTITION__)
#define NOR_ALLOCATED_FAT_SPACE     (NOR_ALLOCATED_FAT_SPACE_DEFAULT - FOTA_DM_FS_OFFSET - FS_BKP_OVERLAP_SIZE)
#else
#define NOR_ALLOCATED_FAT_SPACE    (NOR_ALLOCATED_FAT_SPACE_DEFAULT - FOTA_DM_FS_OFFSET)
#endif
#else
#if defined(__SECURE_DATA_STORAGE__)
#define NOR_ALLOCATED_FAT_SPACE    (NOR_BOOTING_NOR_FS_SIZE - FOTA_DM_FS_OFFSET - FS_SDS_OVERLAP_SIZE)
#elif defined(__NVRAM_BACKUP_PARTITION__)
#define NOR_ALLOCATED_FAT_SPACE     (NOR_BOOTING_NOR_FS_SIZE - FOTA_DM_FS_OFFSET - FS_BKP_OVERLAP_SIZE)
#else
#define NOR_ALLOCATED_FAT_SPACE    (NOR_BOOTING_NOR_FS_SIZE - FOTA_DM_FS_OFFSET)
#endif
#endif   /* !NOR_ALLOCATED_FAT_SPACE */

/*-----------------------------------------------------------------------------------------
 * FS first drive size is automatically decreased by update package storage size as
 * NOR_BOOTING_NOR_FS_FIRST_DRIVE_SECTORS - FOTA_PACKAGE_STORAGE_SIZE / 512 (defined in custom_img_config.h)
 *-----------------------------------------------------------------------------------------*/

#ifndef NOR_BOOTING_NOR_FS_FIRST_DRIVE_SECTORS
#define NOR_PARTITION_SECTORS      (0)
#else
#define NOR_PARTITION_SECTORS      (NOR_BOOTING_NOR_FS_FIRST_DRIVE_SECTORS - FOTA_DM_FS_SECTOR_OFFSET)
#endif   /* !NOR_FAT_PARTITION_SECTORS */


/*
 * Define __PUBLIC_DRIVE_AVAILABLE__ if public drive is available.
 * This definition will be referenced by File System. Please inform FS owner if any changes.
 */

#if defined(NOR_PARTITION_SECTORS)  // NOR flash has public drive
#if (NOR_PARTITION_SECTORS > 0)
#if ((NOR_BOOTING_NOR_FS_FIRST_DRIVE_SECTORS - 0) < 21)
#error "custom\system\{project}\hal_flash_custom_memorydevice.h: NOR_BOOTING_NOR_FS_FIRST_DRIVE_SECTORS should be zero Or (NOR_BOOTING_NOR_FS_FIRST_DRIVE_SECTORS - 0) should be more than 20 sectors."
#endif

#ifndef __PUBLIC_DRIVE_AVAILABLE__
#define __PUBLIC_DRIVE_AVAILABLE__
#endif
#endif
#endif


/*
 *******************************************************************************
 PART 5:
   FLASH TYPE CATEGORY below to configure flash memory command
 *******************************************************************************
*/
#ifdef NOR_FLASH_TYPE_INTEL_SERIES
#define __INTEL_SERIES_NOR__
#endif

#ifdef NOR_FLASH_TYPE_INTEL
#define __INTEL_SERIES_NOR__
#endif

#ifdef NOR_FLASH_TYPE_RENESAS_SERIES
#define __RENESAS_SERIES_NOR__
#endif

#ifdef NOR_FLASH_TYPE_RENESAS
#define __RENESAS_SERIES_NOR__
#endif

#ifdef NOR_FLASH_TYPE_INTEL_SIBLEY
#define __INTEL_SERIES_NOR__
#define __INTEL_SIBLEY__
#endif

#ifdef NOR_FLASH_TYPE_RAM_DISK
#define __RAMDISK__
#endif

#ifdef NOR_FLASH_TYPE_SST
#define __AMD_SERIES_NOR__
#define __SST_NOR__
#endif

#ifdef NOR_FLASH_TYPE_ST_AMD_LIKE
#define __AMD_SERIES_NOR__
#endif

#ifdef NOR_FLASH_TYPE_ST_INTEL_LIKE
#define __INTEL_SERIES_NOR__
#endif

#ifdef NOR_FLASH_TYPE_AMD_SERIES
#define __AMD_SERIES_NOR__
#endif

#ifdef NOR_FLASH_TYPE_SPANSION_PL_J
#define __AMD_SERIES_NOR__
#define NOR_SUSPEND_LATENCY         (35)
#endif

#ifdef NOR_FLASH_TYPE_SPANSION_PL_N
#define __AMD_SERIES_NOR__
#define __SPANSION_PL_N__
#define NOR_SUSPEND_LATENCY         (20)
#endif

#ifdef NOR_FLASH_TYPE_SPANSION_WS_N
#define __AMD_SERIES_NOR__
#define __SPANSION_WS_N__
#define __SPANSION_PL_N__
#define NOR_SUSPEND_LATENCY         (20)
#endif

#ifdef NOR_FLASH_TYPE_SPANSION_WS_P
#define __AMD_SERIES_NOR__
#define __SPANSION_WS_P__
#define NOR_SUSPEND_LATENCY         (40)
#define NOR_RESUME_SUSPEND_INTERVAL (40)
#endif

#ifdef NOR_FLASH_TYPE_SPANSION_GL_A
#define __AMD_SERIES_NOR__
#define __SPANSION_GL_A__
#endif

#ifdef NOR_FLASH_TYPE_SPANSION_GL_N
#define __AMD_SERIES_NOR__
#define __SPANSION_GL_N__
#define NOR_SUSPEND_LATENCY         (20)
#endif

#ifdef NOR_FLASH_TYPE_SPANSION_NS_N
#define __AMD_SERIES_NOR__
#define __SPANSION_NS_N__
#define NOR_SUSPEND_LATENCY         (35)
#define NOR_RESUME_SUSPEND_INTERVAL (30)
#endif

#ifdef NOR_FLASH_TYPE_SPANSION_NS_P
#define __AMD_SERIES_NOR__
#define __SPANSION_NS_N__
#define NOR_SUSPEND_LATENCY         (20)
#define NOR_RESUME_SUSPEND_INTERVAL (30)
#endif

#ifdef NOR_FLASH_TYPE_SPANSION_NS_J
#define __AMD_SERIES_NOR__
#define __SPANSION_NS_J__
#define NOR_SUSPEND_LATENCY         (35)
#endif

#ifdef NOR_FLASH_TYPE_TOSHIBA
#define __AMD_SERIES_NOR__
#endif

#ifdef NOR_FLASH_TYPE_TOSHIBA_TV
#define __AMD_SERIES_NOR__
#define __TOSHIBA_TV__
#endif

#ifdef NOR_FLASH_TYPE_TOSHIBA_TY
#define __AMD_SERIES_NOR__
#define __TOSHIBA_TY__
#endif

#ifdef NOR_FLASH_TYPE_SILICON7
#define __RENESAS_SERIES_NOR__
#endif

#ifdef NOR_FLASH_TYPE_SAMSUNG_SPANSION_NS_J_LIKE
#define __AMD_SERIES_NOR__
#define __SAMSUNG_SPANSION_NS_J_LIKE__
#define NOR_SUSPEND_LATENCY         (20)
#define NOR_RESUME_SUSPEND_INTERVAL (30)
#endif

#ifdef NOR_FLASH_TYPE_SPANSION_VS_R
#define __AMD_SERIES_NOR__
#define __SPANSION_VS_R__
#define NOR_SUSPEND_LATENCY         (30)
#define NOR_RESUME_SUSPEND_INTERVAL (30)
#define __NOR_FDM_4_FLIPPING_TOLERABLE__
#endif

#ifdef NOR_FLASH_TYPE_SPANSION_VS_R64
#define __AMD_SERIES_NOR__
#define __SPANSION_VS_R64__
#define __SPANSION_NS_N__
#define NOR_SUSPEND_LATENCY         (30)
#define __NOR_FDM_4_FLIPPING_TOLERABLE__
#endif

#ifdef NOR_FLASH_TYPE_SPANSION_WS_R
#define __AMD_SERIES_NOR__
#define __SPANSION_WS_R__
#define NOR_SUSPEND_LATENCY         (30)
#define NOR_RESUME_SUSPEND_INTERVAL (30)
#define __NOR_FDM_4_FLIPPING_TOLERABLE__
#endif

/*
 *******************************************************************************
 PART 6:
   FOTA UPDATABLE FLASH AREA
 *******************************************************************************
*/
#ifdef __FOTA_DM__
#ifndef CONFIG_FOTA_UE_FLASH_SPACE
#error "custom\system\{project}\custom_FeatureConfig.h: Error! Please define CONFIG_FOTA_UE_FLASH_SPACE in custom_FeatureConfig.h!"
#endif /* CONFIG_FOTA_UE_FLASH_SPACE */

#ifndef CONFIG_FOTA_PACKAGE_BLOCK_NUMBER
#error "custom\system\{project}\custom_FeatureConfig.h: Error! Please define CONFIG_FOTA_PACKAGE_BLOCK_NUMBER in custom_FeatureConfig.h!"
#endif /* CONFIG_FOTA_PACKAGE_BLOCK_NUMBER */
#endif /* __FOTA_DM__ */


