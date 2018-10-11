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

#ifndef __FLASH_DISK_H__
#define __FLASH_DISK_H__

/*******************************************
*   Include File
********************************************/
#include "hal_flash_custom_memorydevice.h"
#include "hal_flash_opt.h"
#include "hal_flash_mtd.h"
#include "mt7686.h"
#include <stdio.h>
#include <stdlib.h>
#include "hal_flash_drvflash.h"

#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
//#include "intrCtrl.h"
//#include "drv_comm.h"
#endif /*__SINGLE_BANK_NOR_FLASH_SUPPORT__*/

#ifdef __INTEL_SIBLEY__
#include "app_buff_alloc.h" //get control buffer
#endif //__INTEL_SIBLEY__

/*******************************************
*   Error Code Definition
********************************************/

#define RAW_DISK_ERR_NONE 0
#define RAW_DISK_ERR_WRONG_ADDRESS 2
#define RAW_DISK_ERR_NO_SPACE  3

#ifdef __SYSDRV_BACKUP_DISK_RAW__
#define RAW_DISK_ERR_LOCKED 1  //only for backup partition
#define RAW_DISK_ERR_NO_INIT 4  //only for backup partition
#endif //__SYSDRV_BACKUP_DISK_RAW__

#define RAW_DISK_ERR_PROG_FAIL 5
#define RAW_DISK_ERR_ERASE_FAIL 6

#define ERROR_NOR_SUCCESS             	(0)   //(ERROR_FOTA_SUCCESS)
#define ERROR_NOR_READ                	(-13) //(ERROR_FOTA_READ)
#define ERROR_NOR_PROGRAM             	(-14) //(ERROR_FOTA_PROGRAM)
#define ERROR_NOR_ERASE               	(-15) //(ERROR_FOTA_ERASE)
#define ERROR_NOR_OVERRANGE             (-1)  //(ERROR_FOTA_OVERRANGE)

/*******************************************
*   Structure and Macro Definition
********************************************/

#define FLASH_DISK_DONE FS_NO_ERROR

#ifdef __SERIAL_FLASH_EN__
#define NOR_FLASH_PAGE_SIZE 0x200 //default 512B page
#define NOR_FLASH_PAGE_SHIFT 9
#else
#define NOR_FLASH_PAGE_SIZE 0x800 //default 2KB page
#define NOR_FLASH_PAGE_SHIFT 11
#endif

#ifdef __SYSDRV_BACKUP_DISK_RAW__

#define BACKUP_DISK_STATUS_INIT 0
#define BACKUP_DISK_STATUS_EMPTY 1
#define BACKUP_DISK_STATUS_LOCK 2

#endif //__SYSDRV_BACKUP_DISK_RAW__

/*******************************************
*   Function and Variable Definition
********************************************/

/* TC01 security partition API */
#ifdef __CUSTOMER_TC01_DISK__
#define TC01_DISK RAW_DISK1
#define TC01_DISK_BLOCK_SIZE 0x4000
#define TC01_DISK_PAGE_SIZE 0x800 //default 2KB page
#define TC01_DISK_PAGE_SHIFT 11
int32_t EraseRawDataItem(uint32_t block_idx);
int32_t ReadRawDataItem(uint32_t block_idx, uint32_t page_idx, uint32_t length, uint8_t *data_ptr);
int32_t WriteRawDataItem(uint32_t block_idx, uint32_t page_idx, uint32_t length, uint8_t *data_ptr);
#endif //__CUSTOMER_TC01_DISK__

/* Backup partition API*/
#ifdef __SYSDRV_BACKUP_DISK_RAW__
void initBackupPartition(void);
int32_t readBackupPartition(const char *filename, uint8_t *data_ptr, uint32_t len);
int32_t writeBackupPartition(const char *filename, uint8_t *data_ptr, uint32_t len);
void lockBackupPartition(void);
int32_t getBackupPartitionStatus(void);
#endif //__SYSDRV_BACKUP_DISK_RAW__

/* NOR driver API for Secure Storage(in MAUI) */

int32_t NOR_init (void);
//int32_t NOR_QueryInfo (Logical_Flash_info info);
int32_t NOR_ReadPhysicalPage (uint32_t block_idx, uint32_t page_idx, uint8_t *data_ptr);
#ifdef __NOR_FULL_DRIVER__
int32_t NOR_ProgramPhysicalPage (uint32_t block_idx, uint32_t page_idx, uint8_t *data_ptr);
int32_t NOR_ErasePhysicalBlock (uint32_t block_idx);
#endif //__NOR_FULL_DRIVER__
uint32_t NOR_BlockSize(uint32_t block_idx);
uint32_t NOR_BlockIndex(uint32_t block_addr);
int32_t NOR_Addr2BlockIndexOffset(uint32_t block_addr, uint32_t *block_index, uint32_t *offset);
int32_t NOR_BlockIndexOffset2Addr(uint32_t block_index, uint32_t offset, uint32_t *addr);

#ifdef HAL_FLASH_MODULE_ENABLED
hal_flash_block_t get_block_size_from_address(uint32_t address);
#endif

#endif //__FLASH_DISK_H__

