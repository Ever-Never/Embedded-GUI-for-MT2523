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

#ifndef DRVFLASH_H
#define DRVFLASH_H

/*---------------------------------------------
 * Included Header Files
 *---------------------------------------------*/
#include "hal_flash.h"
#ifdef HAL_FLASH_MODULE_ENABLED
#include "hal_flash_fs_type.h"
#include "hal_flash_general_types.h"
#include "hal_flash_kal_public_defs.h"
//#include "kal_internal_api.h"
#include "hal_flash_custom_memorydevice.h"

// default turn on FDM4 enhancement
#define __NOR_FDM_4_FLIPPING_TOLERABLE__

#ifdef FLASH_DISKDRV_DEBUG
#define ONE_BYTE_ALIGN_ADS
#endif

// MTD driver
#define REGION_NUM 8
#define NON_BLOCK_STATUS  (0x8) //4 bit pattern for version check

//FOR FDM5
#define INVALID_BLOCK_ID 0xffff
#define INVALID_TABLE_ID 0xffff
#define INVALID_LOGICAL_NUM 0xffffff
#define INVALID_NOR_ADDR 0xffffffff
#define INVALID_PHY_PAGE 0xffffffff
#define INVALID_ENTRY 0xffff

//FOR FDM4
#define INVALID_BLOCK_INDEX   0xFFFFFFFF
#define INVALID_SECTOR_INDEX  0xFFFFFFFF
#define INVALID_ERASE_COUNT   0x00FFFFFF // we only have 24 bits
#define MAX_ERASE_COUNT       (INVALID_ERASE_COUNT-1)

/* Global defines */
#define SPARE_RESERVED_RATIO                    (10)
#define MINIMUM_SPARE_AMOUNT                    (2)
#define SLEEP_MODE_SPARE_AMOUNT                 (4)
#define GC_THRESHOLD_DIVISION                   (3)
#define GC_THRESHOLD_GAP                        (1)
#define RESERVED_SECTOR_FOR_MINIMUM_SPARE_SPACE (4) ///< The number of additional reserved sectors if erase queue size is set to the minimum value (2)
#define RESERVED_SECTOR_FOR_REPEATE_POWER_LOSS_DURING_RECLAIM (6) ///< The number of additional reserved sectors if erase queue size is set to the minimum value (2)
#define MINIMUM_SYSTEM_DRIVE_RESERVE_BLOCK      (3) ///< minimum System Drive Reserved Blocks (unit: block)

/******** RESULT for Single Bank ************/
#define RESULT_FLASH_DONE (1)
#define RESULT_FLASH_BUSY (0)
#define RESULT_FLASH_FAIL (-1)


typedef ONE_BYTE_ALIGN_ADS struct {
    uint32_t TotalBlocks;
    uint32_t BlockSize[REGION_NUM];
    uint32_t AvailInRegion[REGION_NUM];       ///< empty and the number of blocks in erase queue
    uint32_t RegionBlocks[REGION_NUM];
    uint32_t ActualRegionNum;
    WORD  PageSize;
    uint32_t baseUnlockBlock;                 ///< Add because FOTA need to unlock different region in INTEL flash
    uint32_t endUnlockBlock;                  ///< Add because FOTA need to unlock different region in INTEL flash
} NOR_MTD_FlashInfo;


/* add ONE_BYTE_ALIGN_ADS keyword to this struct so that it won't violate ARM's ADS compiler rule.
 * No need, since this structure is included into another ONE_BYTE_ALIGN_ADS structure in pointer form
 */
typedef struct {
    int     (* MountDevice)  (void *DriveData, void *Info);
    int     (* ShutDown)     (void *DriveData);
    void   *(* MapWindow)    (void *DriveData, uint32_t BlockIndex, uint32_t WindowIndex);
    int     (* EraseBlock)   (void *DriveData, uint32_t BlockIndex);
    int     (* ProgramData)  (void *DriveData, void *Address, void *Data, uint32_t Length);
    int     (*NonBlockEraseBlock) (void *DriveData, uint32_t  BlockIndex);  /* Added by Eric */
    int32_t     (*CheckDeviceReady)   (void *DriveData, uint32_t BlockIndex);  /* Added by Eric */
    int     (*SuspendErase)       (void *DriveData, uint32_t BlockIndex);  /* Added by Eric */
    int     (*ResumeErase)        (void *DriveData, uint32_t BlockIndex);  /* Added by Eric */
    int     (*BlankCheck)             (void *DriveData, uint32_t BlockIndex);
    int     (*OTPAccess)           (void *DriveData, int accesstype, UINT Offset, void *BufferPtr, UINT Length);
    int     (*OTPQueryLength)      (void *DriveData, UINT *Length);
    int     (*LockEraseBlkAddr)         (void *DriveData, void *Address, uint32_t Action);
    bool (*IsEraseSuspended)     (void *DriveData, uint32_t BlockIndex);
    int     (*IOCtrl)        (void *DriveData, uint32_t CtrlAction, void *CtrlData);   // For device IO control
} NOR_MTD_Driver;

/*LockEraseBlkAddr Action*/
#define ACTION_UNLOCK 0
#define ACTION_LOCK 1
#define ACTION_ERASE 2


// This is the file MTD for testing only

typedef struct {
    const  char *FileName;
    uint32_t  FileSize;
    uint32_t  BlockSize;
    uint32_t  WindowSize;
    HANDLE H;
    BYTE *BaseAddr;
    void *CurrAddr;
} NORMtdFileData;


// Flash Info
typedef struct {
    uint32_t BlockSize;
    uint32_t RegionBlocks;
} FlashRegionInfo;
#define EndRegionInfo  {0, 0}

typedef struct {
    uint32_t BankSize;
    uint32_t Banks;
} FlashBankInfo;
#define EndBankInfo  {0, 0}

// Erase block Info
/* add ONE_BYTE_ALIGN_ADS keyword to this struct so that it won't violate ARM's ADS compiler rule.
 * No need, since this structure is included into another ONE_BYTE_ALIGN_ADS structure in pointer form
 */
typedef struct {
    uint32_t BlockIndex;
    uint32_t EraseCount;
} NOR_EraseInfo;

#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
// address look up buffer
#define LOOKUP_TABLE_SIZE  (8)
typedef struct {
    uint32_t BlockIndex;
    uint32_t BaseAddress;
} NOR_AddrLookup;
#endif

typedef struct {
    uint32_t  Signature;
    BYTE *BaseAddr;
    kal_semid          DeviceLock;       // The device lock that keeps driver resource consistent
    kal_taskid         DeviceLockOwner;  // The owner of the deivce lock
    int8_t           DeviceLockDepth;  // The depth of the deivce lock

    FlashRegionInfo *RegionInfo;
    BYTE *CurrAddr;
    FlashBankInfo *BankInfo;
    BYTE *CurrBankAddr;
    uint32_t CurrBankSize;
    BYTE *CurrPollAddr;

#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
    NOR_AddrLookup  lookup_tbl[LOOKUP_TABLE_SIZE][2];
#endif
} NOR_Flash_MTD_Data;


/* Multi-sector Protection Entry */

#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
/* Erase queue data structure */
typedef ONE_BYTE_ALIGN_ADS struct {
    uint32_t head;
    uint32_t tail;
    uint32_t *queue;
} NOR_EraseQueue;
/* Flash state maintainence */
typedef enum {
    NOR_IN_READY = 0xF0,
    NOR_IN_PROGRAM,
    NOR_IN_SUSPENDED_PROGRAM,
    NOR_IN_ERASE,
    NOR_IN_SUSPENDED_ERASE,
    NOR_IN_ERROR
} NOR_Flash_State;

extern NOR_Flash_State FlashState;

#define Query_Flash_State(a) \
   {\
      a = FlashState; \
   }

#define Set_Flash_State(a) \
   {\
      FlashState = a; \
   }

#endif /* __SINGLE_BANK_NOR_FLASH_SUPPORT__ */

typedef ONE_BYTE_ALIGN_ADS struct {
    uint32_t LogicalSectorID;
    uint32_t PysicalSectorID_new;
} MS_ENTRY;


#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__) && !defined(__NOR_IDLE_SWEEP__)
#define __ERASE_QUEUE_ENABLE__
#endif // __SINGLE_BANK_NOR_FLASH_SUPPORT__ && !__NOR_IDLE_SWEEP__

// Flash driver's data
typedef ONE_BYTE_ALIGN_ADS struct NOR_FLASH_DRV_Data {
    uint32_t    TotalFATSectors;        ///< Total number of FAT sectors in flash (i.e., TotalPhysicalSectors - reserved sectors)
    NOR_MTD_Driver *MTDDriver;       ///< MTD driver
    void    *MTDData;                ///< MTD data
    WORD    *AvailSectorsInBlock;    ///< The number of SECTOR_AVAIL in each block
    WORD    *ValidSectorsInBlock;    ///< The number of valid sectors (not SECTOR_AVAIL && not SECTOR_DELETED)
    NOR_MTD_FlashInfo FlashInfo;     ///< Flash information
    uint32_t    HeaderSize[REGION_NUM]; ///< Size (bytes) from the beginning of a block address to the first data sector (i.e., contains both block and sector headers)
    uint32_t    SectorsPerBlock[REGION_NUM];    ///< Number of "physical" sectors (exclude sectors for header) in each block of the same region
    uint32_t    ActiveBlock;            ///< Current active block ID
    uint32_t    ReclaimEraseCount;      ///< Erase count of the reclaimed block
    uint32_t    ReclaimBlockID;         ///< Block ID of the reclaimed block
    uint32_t    TotalPhysicalSectors;   ///< Total number of "physical" sector (exclude sectors for header) in flash. (For each region, TotalPhysicalSectors += RegionBlocks[region] * SectorsPerBlock[region])
    uint32_t    PartitionSectors;       ///< Size of the first FAT partition (unit: sectors)
#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
    uint32_t    GCBlockSectors;         ///< The maximum number of sectors of a block in flash (i.e., MaxSectorsPerBlock)
#endif
#ifdef __ERASE_QUEUE_ENABLE__
    uint32_t    GCHighThreshold;        ///< Middle GC Threshold + GCThresholdDiff
    uint32_t    GCMiddleThreshold;      ///< Low GC threshold + GCThresholdDiff, GCThresholDiff = (The max capacity of erase queue - GCLowThreshold) / 3
    uint32_t    GCLowThreshold;         ///< Low GC threshold: 2 blocks (1 for reclaim and 1 for program fail) + sectors for MSP
#else
    uint32_t    GCThreshold;            ///< GC threshold for Multi-Bank (2 maximum block + sectors for MSP)
#endif /* __ERASE_QUEUE_ENABLE__*/
    uint32_t    AvailSectors;           ///< Total SECTOR_AVAIL sectors in flash (of course, exclude sectors for headers)
    uint32_t    DeletedSectors;         ///< The number of deleted sectors in system (Increased in DeletePhysicalSector, MountDevice, ResumeSectorStates)
    uint32_t    StartSector;            ///< Start "logical sector ID" for write or marking to process
    uint32_t    Sectors;                ///< Total number of logical sectors to be updated (set in MarkToProcess)
    uint32_t    SectorsLeft;            ///< The number of logical sectors left to be updated. i.e., The number of sectors whose update bit is set. (set in MarkToProcess and be decreased in MarkProcessed)
    uint32_t    BLOCKID_OFFSET;         ///< Block offset, calculated by MaxSectorsPerBlock
    uint32_t    SECTORIDX_MASK;         ///< Mask for retrieve sector index inside a block
    uint32_t    PHY_SECTOR_OFFSET;      ///< Offset for retriving first 7 bits of PhysicalSector (i.e., Block offset + Local sector offset - 7)

    uint32_t    MSTABLE_ENTRY_NUM;
    uint32_t    MS_count;               ///< The number of valid entries in MS Entry Table
    MS_ENTRY *MSEntryTable;
    BYTE    *SectorMap;              ///< Sector map, built in MountDevice
    BYTE    *Buffer;                 ///< FDM Buffer: SIBLEY(1024 bytes) / Others (512 bytes), used when source data is located in the same bank (ESB, movee sectors in ReclaimBlock_pre...)

    void (*CompleteErase)(struct NOR_FLASH_DRV_Data *D);

    uint32_t    RegionMaxBlock;         ///< (The first) region ID which contains blocks that have maximum number of sectors (to not let such block is selected as active block when the block is the only one left in this region, which makes Program Fail handle properly)

    void    (*ProgramFailHandle)  (struct NOR_FLASH_DRV_Data *D);
    void    (*ReclaimBlock_post)  (struct NOR_FLASH_DRV_Data *D);
#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__) || defined(__NOR_IDLE_SWEEP__)
    uint32_t    IdleReclaimBlockID;     ///< Block ID to be reclaimed in IdleReclaim task (IDLER)
    uint32_t   (*SelectReclaimTarget)(struct NOR_FLASH_DRV_Data *D);
    uint32_t   (* ReclaimOneSector)  (struct NOR_FLASH_DRV_Data *D, uint32_t Target, uint32_t sector);
#endif //__SINGLE_BANK_NOR_FLASH_SUPPORT__ || __NOR_IDLE_SWEEP__

#ifdef __NOR_IDLE_SWEEP__
    uint32_t    IdleSweepThreshold;     ///< Threshold to trigger Idle Sweep
#endif

#ifdef __ERASE_QUEUE_ENABLE__
    NOR_EraseInfo  *EraseQueue;      ///< Pointer to EraseBlockQueue, default size = 5 (SNOR_ERASE_QUEUE_SIZE)
    // Sleep mode support -- may need to be removed to internal RAM when DCM is enabled
    uint32_t    processedBankAddr;      ///< Bank address for checking if device is ready (set to MTD.CurrPollAddr in ProcessEraseQueue)
    uint32_t    processedBlock;         ///< The target block to be erased in sleep mode (set in ProcessEraseQueue only)
    uint32_t    processedEraseCount;    ///< Erase count of D->processedBlock
    WORD     eraseItems;             ///< The number of items(blocks) in EraseQueue
#endif //__ERASE_QUEUE_ENABLE__

#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
    WORD     queueSize;              ///< The size of EraseQueue, default size = 5 (SNOR_ERASE_QUEUE_SIZE)
#else
    WORD     SystemDriveReservedUnits;   ///< The number of reserved unit (1 unit = 0.5 block), default size = 6 (NOR_SYSTEM_DRIVE_RESERVED_BLOCK * 2)
#endif /* __SINGLE_BANK_NOR_FLASH_SUPPORT__ */

#ifdef __NOR_IDLE_SWEEP__
    BYTE          BlockReclaimingMark;
#endif
    bool    ProgramFailRetry;

#ifdef __ERASE_QUEUE_ENABLE__
    bool IsEraseSuspended;       ///< A flag to indicate whether a erase operation issued by sleep mode is suspended (only set to TRUE in SuspendEraseQueue)
#endif //__ERASE_QUEUE_ENABLE__


} NOR_FLASH_DRV_Data;


extern kal_mutexid fdm_reclaim_mutex;
extern kal_eventgrpid  nor_egid;
#define NOR_BRECL_EVENT 0x1
#define NOR_FRECL_EVENT 0x2
#define NOR_DMAN_EVENT 0x4


/* Flash Bank Info */
typedef struct {
    uint32_t BankSize;
    uint32_t Banks;
} NORBankInfo;
#define EndBankInfo  {0, 0}

/* Flash Layout Info */
typedef struct {
    uint32_t TotalLSMT;
    uint32_t BlkSize;
    uint32_t PageSize;
    NORBankInfo *BankInfo;
    WORD TblEntrys;
    WORD TotalBlks;
} NORLayoutInfo;

typedef struct {
    uint32_t  Signature;
    BYTE *BaseAddr;
    kal_semid          DeviceLock;       // The device lock that protects driver resource
    kal_taskid         DeviceLockOwner;  // The owner of the deivce lock
    int8_t           DeviceLockDepth;  // The depth of the deivce lock

    NORLayoutInfo *LayoutInfo;
} NOR_MTD_DATA;

typedef ONE_BYTE_ALIGN_ADS struct {
    uint32_t MSCount;
    uint32_t *LogPageID;
    WORD *NewEntryID;
    WORD *OldEntryID;
} MS_TABLE;

typedef ONE_BYTE_ALIGN_ADS struct {
    WORD LogBlkID;
    WORD TblIDInBlk;
} LSMGEntry;

//flash driver data for FDM5.0
typedef ONE_BYTE_ALIGN_ADS struct {
    uint32_t    TotalFATSectors;
    NOR_MTD_Driver *MTDDriver;
    void       *MTDData;
    int  (* DiscardSectors) (void *DriveData, UINT Sector, UINT Sectors);
    void (*ReclaimBlock) (void *DriveData, bool isBackground);
#if defined(__SERIAL_FLASH_EN__)
    void    (*ProgramFailHandle)  (void *DriveData);
    NOR_MTD_FlashInfo FlashInfo;     ///< Flash information
#endif
    uint32_t    SecondPartitionSectors;
    uint32_t    HeaderSize[2];  //HeaderSize[0]: data block header size
    //HeaderSize[1]: table block header size
    uint32_t    *InvalidEntryInTblBlk; //valid entry in table block
    uint32_t    TotalAvail[2];   // TotalAvail[0]: total available pages in data block
    //TotalAvail[0]: total available tables in table block
    //not include empty block (2 spare block)
    uint32_t    TotalInvalid[2]; // TotalInvalid[0]: total invalid pages in data block
    //TotalInvalid[0]: total invalid tables in table block
    uint32_t    ReclHighThreshold[2]; //unit ReclHighThreshold[0]: Pages ReclHighThreshold[1]: Tables
    uint32_t    ReclLowThreshold[2];
    uint32_t    SystemThreshold[2];
    bool   NeedResumeFlag;
    bool   NeedFRecl;
    LSMGEntry    *LSMG;            // logical sector mapping group table
    WORD      ReclType;
    WORD      *LBM;              //logical block mapping table
    WORD      *AvailInBlk;     //available page in data block, available table in table block
    WORD      *InvalidInBlk;     //valid  page in data block, valid table in table block
    WORD      ActiveBlk[2];    // ActiveBlk[0]: active data block
    //ActiveBlk[1]: active table block
    WORD      ReclLogBlkID;  //ReclLogBlkID == INVALID_BLOCK_ID means no block need reclamation
    WORD      ErasingPhyBlk;
    WORD      ReclToPhyBlkID;
    WORD      ReclFromPhyBlkID;
    WORD      PageSizeShift;
    WORD      BlkIDShift;
    WORD      TblSizeShift;
    WORD      TblIDShift;
    WORD      PagesPerBlk;
    WORD      TblsPerBlk;
    WORD      TblBlks;
    WORD      SetTblBlks;
    WORD      DataBlks;
    uint32_t MSTABLE_ENTRY_NUM;
    MS_TABLE  MSTable;
    BYTE      *Buffer;
    BYTE      *CopyBuffer;
} NOR_FTL_DATA;


typedef struct {
    uint32_t Blocks;
    uint32_t BlockSize;
    uint32_t EraseCountMax;
    uint32_t EraseCountMin;
    uint32_t EraseCountAverage;
    uint32_t SectorsInUse;
    uint32_t SectorsDeleted;
    uint32_t SectorsAvail;
} NORFlashInfo;

/* Definition for power loss test */
#if defined(FLASH_DISKDRV_DEBUG) && defined(POWERLOSS_TEST)
#include <setjmp.h>
extern jmp_buf mark;
extern int Test_CD_Value;
#ifndef __NOR_FDM5__
#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
enum {
    /* 1*/WriteSector_SECTOR_WRITING = 1,
    /* 2*/WriteSector_SECTOR_WRITTEN,
    /* 3*/WriteSector_SECTOR_DELETED,
    /* 4*/WriteSector_SECTOR_VALID,
    /* 5*/WriteSector_SECTOR_MS_WRITTEN,
    /* 6*/ReclaimBlock_pre_BLOCK_RECLAIMING,
    /* 7*/ReclaimBlock_pre_BLOCK_RECLAIMED,
    /* 8*/ReclaimBlock_pre_BLOCK_VIRGINE,
    /* 9*/ReclaimBlock_post_BLOCK_EMPTY,
    /*10*/SetActiveBlock_BLOCK_FULL,
    /*11*/SetActiveBlock_BLOCK_ACTIVE,
    /*12*/ResumeSectorStates_SECTOR_MS_BEINGVALIDATED,
    /*13*/WriteSector_SECTOR_MOVING,
    /*14*/WriteSectors_Validate_MS_Sector,
    /*15*/WriteSectors_Validate_MS_VALID_Sector,
    /*16*/ReclaimEraseQueueItem_BLOCK_VIRGINE,
    /*17*/IdleReclaimBlocks_BLOCK_RECLAIMING,
    /*18*/IdleReclaimBlocks_BLOCK_RECLAIMING_COPYSECTOR,
    /*19*/IdleReclaimBlocks_BLOCK_VIRGINE,
    /*20*/IdleReclaimBlocks_RECALIM_ONE_MORE_BLOCK,
    /*21*/IdleReclaimBlocks_BLOCK_RECLAIMED
};

#define TEST_POINT(a)\
   {\
      switch(a)\
      {\
         case WriteSector_SECTOR_MS_WRITTEN:\
         case ReclaimBlock_pre_BLOCK_RECLAIMING:\
         case ReclaimBlock_pre_BLOCK_RECLAIMED:\
         case ReclaimBlock_pre_BLOCK_VIRGINE:\
         case ReclaimBlock_post_BLOCK_EMPTY:\
         case SetActiveBlock_BLOCK_FULL:\
         case SetActiveBlock_BLOCK_ACTIVE:\
         case WriteSectors_Validate_MS_Sector:\
            Test_CD_Value -= 50;\
            break;\
         case ResumeSectorStates_SECTOR_MS_BEINGVALIDATED:\
            Test_CD_Value -= 50;\
            break;\
         case IdleReclaimBlocks_BLOCK_RECLAIMING:\
         case IdleReclaimBlocks_BLOCK_RECLAIMING_COPYSECTOR:\
         case IdleReclaimBlocks_BLOCK_VIRGINE:\
         case IdleReclaimBlocks_RECALIM_ONE_MORE_BLOCK:\
         case IdleReclaimBlocks_BLOCK_RECLAIMED:\
            Test_CD_Value -= 50;\
            break;\
         case WriteSectors_Validate_MS_Sector:\
         case WriteSectors_Validate_MS_VALID_Sector:\
            Test_CD_Value -= 50;\
            break;\
         default:\
            Test_CD_Value--;\
            break;\
      }\
      if(Test_CD_Value <= 0)\
         longjmp(mark, a);\
   }
#else /*!__SINGLE_BANK_NOR_FLASH_SUPPORT__*/
enum {
    /* 1*/WriteSector_SECTOR_WRITING = 1,
    /* 2*/WriteSector_SECTOR_WRITTEN,
    /* 3*/WriteSector_SECTOR_DELETED,
    /* 4*/WriteSector_SECTOR_VALID,
    /* 5*/WriteSector_SECTOR_MS_WRITTEN,
    /* 6*/ReclaimBlock_pre_BLOCK_RECLAIM,
    /* 7*/ReclaimBlock_pre_BLOCK_VIRGINE,
    /* 8*/ReclaimBlock_post_BLOCK_EMPTY,
    /* 9*/SetActiveBlock_BLOCK_FULL,
    /*10*/SetActiveBlock_BLOCK_ACTIVE,
    /*11*/ResumeSectorStates_SECTOR_MS_BEINGVALIDATED,
    /*12*/WriteSector_SECTOR_MOVING,
    /*13*/WriteSectors_Validate_MS_Sector
};

#define TEST_POINT(a)\
   {\
      switch(a)\
      {\
         case WriteSector_SECTOR_MS_WRITTEN:\
         case ReclaimBlock_pre_BLOCK_RECLAIM:\
         case ReclaimBlock_pre_BLOCK_VIRGINE:\
         case ReclaimBlock_post_BLOCK_EMPTY:\
         case SetActiveBlock_BLOCK_FULL:\
         case SetActiveBlock_BLOCK_ACTIVE:\
         case WriteSectors_Validate_MS_Sector:\
            Test_CD_Value -= 50;\
            break;\
         case ResumeSectorStates_SECTOR_MS_BEINGVALIDATED:\
            Test_CD_Value -= 50;\
            break;\
         default:\
            Test_CD_Value--;\
            break;\
      }\
      if(Test_CD_Value <= 0)\
         longjmp(mark, a);\
   }
#endif /*__SINGLE_BANK_NOR_FLASH_SUPPORT__*/
#else /*FDM5*/
enum {
    /* 1*/ReclaimBlock_MARK_BLOCK_RECLAIM = 1,
    /* 2*/ReclaimBlock_MARK_BLOCK_COPYING,
    /* 3*/ReclaimBlock_TABLE_BLOCK_COPYING,
    /* 4*/ReclaimBlock_DATA_BLOCK_COPYING,
    /* 5*/ReclaimBlock_MARK_BLOCK_ERASING,
    /* 6*/ReclaimBlock_AFTER_ERASED,
    /* 7*/CopyTable_MARK_COPYING,
    /* 8*/CopyTable_MARK_ALLOCATING,
    /* 9*/CopyTable_MARK_VALID,
    /*10*/WritePage_MARK_TABLE_ENTRY_WRITING,
    /*11*/WritePage_MARK_TABLE_ENTRY_WRITTEN,
    /*12*/WritePage_MARK_PAGE_VALID,
    /*13*/WritePage_MARD_TABLE_ENTRY_MS_WRITING,
    /*14*/WritePage_MARD_TABLE_ENTRY_MS_BEGIN_VALID,
    /*15*/SetActiveBlock_BLOCK_FULL,
    /*16*/SetActiveBlock_BLOCK_ACTIVE
};


#define TEST_POINT(a)\
   {\
      switch(a)\
      {\
         case ReclaimBlock_MARK_BLOCK_RECLAIM:\
         case ReclaimBlock_MARK_BLOCK_COPYING:\
         case ReclaimBlock_TABLE_BLOCK_COPYING:\
         case ReclaimBlock_DATA_BLOCK_COPYING:\
         case ReclaimBlock_MARK_BLOCK_ERASING:\
         case ReclaimBlock_AFTER_ERASED:\
         case CopyTable_MARK_COPYING:\
         case CopyTable_MARK_ALLOCATING:\
         case CopyTable_MARK_VALID:\
         case WritePage_MARK_TABLE_ENTRY_WRITING:\
         case WritePage_MARK_TABLE_ENTRY_WRITTEN:\
         case WritePage_MARK_PAGE_VALID:\
         case WritePage_MARD_TABLE_ENTRY_MS_WRITING:\
         case WritePage_MARD_TABLE_ENTRY_MS_BEGIN_VALID:\
         case SetActiveBlock_BLOCK_FULL:\
         case SetActiveBlock_BLOCK_ACTIVE:\
            Test_CD_Value -= 50;\
            break;\
         default:\
            Test_CD_Value--;\
            break;\
      }\
      if(Test_CD_Value <= 0)\
         longjmp(mark, a);\
   }
#endif /*__NOR_FDM5__*/
#define START_POINT  setjmp(mark)
#else
#define TEST_POINT(a)
#define START_POINT
#endif /* FLASH_DISKDRV_DEBUG */




int OTPAccess(void *DriveData, int accesstype, UINT Offset, void *BufferPtr, UINT Length);
int OTPQueryLength(void *DriveData, UINT *Length);



#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__) || defined(__NOR_IDLE_SWEEP__)
extern hal_flash_status_t GetFDMLock(void);
extern void FreeFDMLock(void);
#define retriveFDMLock GetFDMLock
#define releaseFDMLock FreeFDMLock
#else // !__SINGLE_BANK_NOR_FLASH_SUPPORT__ && !__NOR_IDLE_SWEEP__
extern void FDM_LOCK(void);
extern void FDM_UNLOCK(void);
#define retriveFDMLock FDM_LOCK
#define releaseFDMLock FDM_UNLOCK

#endif // __SINGLE_BANK_NOR_FLASH_SUPPORT__ || __NOR_IDLE_SWEEP__


#if defined(__NOR_IDLE_SWEEP__)
extern void setIdleSweepThreshold(NOR_FLASH_DRV_Data *D, uint32_t MaxSectorsPerBlock);  //flash_mtd.c
#endif



#if ( defined(__NOR_FLASH_HARDWARE_TEST__) && !defined(__UBL__))
uint32_t SIB_LocateSector(NOR_FLASH_DRV_Data *D, uint32_t LogicalSector);
uint32_t LocateSector(NOR_FLASH_DRV_Data *D, uint32_t LogicalSector);
#endif

// Use fatal error handler instead assert, because assert will write data to FAT which is not suitable when flash error happens
// write a special code: KAL_ERROR_FILESYS_NOR_FDM_INTERNAL_FAILED  to kal_fatal_error_handler to not do any FAT write during fatal error
#if (defined __UBL__) || (defined __FUE__)
#define NORFDM_EXT_ASSERT(expr, e1, e2, e3) EXT_ASSERT(expr, e1, e2, e3)
#define NORFDM_ASSERT(expr) ASSERT(expr)
#else
#define NORFDM_EXT_ASSERT(expr, e1, e2, e3) {if(!(expr)) {kal_fatal_error_handler(KAL_ERROR_FILESYS_NOR_FDM_INTERNAL_FAILED, __current_pc());}}
#if defined(DEBUG_KAL)
#define NORFDM_ASSERT(x) {if(!(x)) {kal_fatal_error_handler(KAL_ERROR_FILESYS_NOR_FDM_INTERNAL_FAILED, __current_pc());}}
#else
#define NORFDM_ASSERT(x)
#endif
#endif // __UBL__ || __FUE__

#endif
#endif   /* !DRVFLASH_H */
