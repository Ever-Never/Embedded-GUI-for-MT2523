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


#ifndef NAND_FDM_H
#define NAND_FDM_H


/*******************************************************************************
 * Include Header Files
 * FLASH_DISKDRV_DEBUG indicates simulation environment
 *******************************************************************************/
#include "fs_port.h"

#if 0
#if defined(__MTK_TARGET__)
#ifdef __DYNAMIC_SWITCH_CACHEABILITY__
#define __nandfdm_cache_align(x)   __attribute__ ((aligned(x), zero_init, section ("DYNAMICCACHEABLEZI_C")))
#else
#define __nandfdm_cache_align(x)
#endif
#else /* __MTK_TARGET__ */
#define __nandfdm_cache_align(x)
#endif /* __MTK_TARGET__ */
#endif

/*******************************************************************************
 * parameter definition
 *******************************************************************************/

//#define POWERLOSS_TEST             // power recovery test in simulation
//#define ERROR_HANDLING_TEST   // error handling test in simulation
#define DDWORD_INVALID           ((uint64_t)~0)
#define DWORD_INVALID            0xFFFFFFFF
#define WORD_INVALID             0xFFFF
#define BYTE_INVALID             0xFF
#define FDM_ERROR                1
#define FDM_NO_ERROR             0

/* Public drive size customization constraints */
#define NAND_MINIMUM_PARTITION_SIZE     (400)
#define MAX_DATA_SIZE            2048  // Max NAND page size used for MMU.

#if defined(__NAND_FDM_50__)

#include "NAND_MTD_FDM50_internal.h"
#include "NAND_DAL.h"
#include "NAND_DAL_internal.h"
// If support RAW disk
#define RAW_DISK_INDEX    5

#define NUM_ACTIVESLOT    5
#define NUM_ACTIVESECT    4  // This number should be one less than NUM_ACTIVESLOT because need to consider the power-loss.

// Block mark in spare area.
#define BM_LBM       0x6688
#define BM_PMT       0x8866
#define BM_NONBMR    0xf0d0
#define BM_BRMT      0x0d0f
#define BM_FDM4XCTL  0xf00f
#define BRMT_FDM52   0x5200
#define BM_RAW       0x7264 // Raw disk, rd

// Newest data location.
#define SOURCEDATA_NONBMR    0
#define SOURCEDATA_BMR       1
#define SOURCEDATA_DATA      2

#define R_SHIFT           10    // Region shift bit number.
#define LSMT_SECTORNUM    5     // 2K for LSMT and 512 for active block information.
#define LSMT_SIZE         1024
#define BRMT_SIZE         256   // The number of entries in BRMT load into RAM one time.
#define PMT_SIZE          512   // The number of entries in PMT.
#define NUM_OF_ZONES      8
#define NUM_OF_BANK       8
#define NUM_OF_REGION     8  //** Max number of regions per zone. This is used to save LSMT block address.
#define PSECTION_NUM_R    1024
#define LSECTION_NUM_R    1010
#define NUM_ERASE_QUEUE   14
#define BMR_THRESHOLD     16

#define LSMT_EMPTY        0x8000
#define LSMT_NOT_EMPTY    0x7fff
#define LSMT_NOT_REMAP    0x4000
#define LSMT_REMAP        0xbfff
#define LSMT_SS           0x3000
#define LSMT_VALID        0x0800
#define LSMT_PSA          0x07ff


#define NONBMR_INVALID    0
#define NONBMR_ONLYVALID_AFTER_BMRLAST	1
#define NONBMR_VALID     2

//#ifdef __MTK_TARGET__
//extern uint8_t INT_QueryExceptionStatus(void);
//#endif


// Structure definition for FDM

typedef struct {
    uint16_t	LSectionAddr;
    uint16_t	BMR_PSection;
    uint16_t	BMR_LastPage;
    uint16_t	NonBMR_PSection;
    uint16_t	NonBMR_LastPage;
    uint16_t	NonBMR_TablePage;
    uint16_t	NumNoUse;
} ACTIVE_INFO;

typedef struct {
    uint16_t	PSectionAddr;
    uint16_t	EmptyPageAddr;
    uint8_t	CurActive;
} PMT_INFO;

typedef struct {
    uint32_t     total_FAT_sectors;     // total sectors that FAT can use
    uint32_t     partition_sectors;     // partiton sectors that FAT can use
} FAT_INFO;

typedef struct {
    uint16_t PSect;
    uint16_t Page;
} LSMT_INFO;

typedef struct {
    uint16_t SectionBuf[16];
    uint8_t  Head;
    uint8_t  Tail;
} ERASE_QUEUE;



typedef struct {
    uint8_t  Zone;
    uint8_t  NewZone;
    uint8_t  Region;
    uint8_t  NewRegion;
    uint16_t LSect;
    uint16_t PSect;
    uint16_t LPage;
    uint16_t PPage;
} ADDR_INFO;

typedef struct {
    uint16_t LSectAddr;
    uint16_t NumCycles;
} WEAR_INFO;


#if defined(__NANDFDM_TOTAL_BBM__)
#if 0
typedef union _FDMTABLE {
struct {
uint32_t	Sector[1024];
} All;
struct {
uint16_t	LSM[LSMT_SIZE];     // Map logical to physical block.
uint16_t	PMT[PMT_SIZE];		// Map logical to physical page. Need to enlarge in case of MLC+interleave+2P (128*2*2)
//		uint16_t	BRMT[BRMT_SIZE];	// Map bad block into good block in replacement area.
uint16_t	RESERVRD[BRMT_SIZE];	// Map bad block into good block in replacement area.
uint16_t	Buffer[256];		// Temp buffer.
} T;
struct {
uint16_t	BRMT[2048];   // Low level format use. One time 4K write for future 4K page flash.
} A;
struct {
uint8_t	Sector[8][512];
} S;
} FDM_TABLE;

typedef struct {
    uint8_t	  RegionNum;
    uint16_t	LSectionNum;
    uint16_t	PSectionNumLastR;
    uint16_t	LSectionNumLastR;
    uint16_t	RegionInitMask;
    LSMT_INFO	  LSMT[NUM_OF_REGION];
} ZONE_INFO;

typedef struct {
    uint8_t        ParameterReady;
    uint8_t	      NoReplace;
    uint16_t      BRMT_Addr;
    uint16_t      BRMT_LastPSection;             // Global , must be (0x8000|xxxx)
    uint16_t      ReplaceBeginSection;            // Global , must be (0x8000|xxxx)
    uint16_t      ReplaceBlockBegin[NUM_OF_BANK]; // Global , must be (0x8000|xxxx)
    uint16_t      CurSectorInRAM;
    uint16_t      LastPage;
    uint16_t	    BRMT_Table[BRMT_SIZE]; // Map bad block into good block in replacement area.
} BBM_BRMT_INFO;

typedef struct {
    uint8_t	  	PageNumInBuffer;
    uint8_t    	CurActive;
    uint8_t        NeedRebuildAll;
    uint8_t	  	WriteProtect;
    uint8_t	  	PMTInRAMModified;
    uint8_t	  	NewActive;
    uint16_t       	TotalPSectionNum;
    ERASE_QUEUE	  	EraseQueue;
    ZONE_INFO	  	ZoneInfo[NUM_OF_ZONES];
    ACTIVE_INFO	  	ActiveInfo[NUM_ACTIVESLOT];
    FDM_TABLE	  	FDMTable;
    PMT_INFO	  	PMTInfo;
    WEAR_INFO	  	WearInfo;
    uint16_t	  	DATA_PSection;
    uint16_t	  	DataBank[NUM_OF_BANK];
    uint16_t	  	NewBank[NUM_OF_BANK];
    uint16_t	  	BMRBank[NUM_OF_BANK];
    uint16_t	  	NonBMRBank[NUM_OF_BANK];
    uint16_t	  	NewPSect;
    uint16_t	  	ActiveOrder;
#if defined(__NANDFDM_MULTI_INSTANCE__)
uint8_t    	DiskIndex;
uint8_t	  	is_mount;
#endif // defined(__NANDFDM_MULTI_INSTANCE__)
} NANDFDM5_DATA;
#endif
#else // defined(__NANDFDM_TOTAL_BBM__)
typedef union _FDMTABLE {
struct {
uint32_t	Sector[1024];
} All;
struct {
uint16_t	LSM[LSMT_SIZE];     // Map logical to physical block.
uint16_t	PMT[PMT_SIZE];  // Map logical to physical page. Need to enlarge in case of MLC+interleave+2P (128*2*2)
uint16_t	BRMT[BRMT_SIZE]; // Map bad block into good block in replacement area.
uint16_t	Buffer[256];  // Temp buffer.
} T;
struct {
uint16_t	BRMT[2048];   // Low level format use. One time 4K write for future 4K page flash.
} A;
struct {
uint8_t	Sector[8][512];
} S;
} FDM_TABLE;

typedef struct {
    uint16_t	BRMT_Addr[NUM_OF_ZONES];
    uint16_t	ReplaceBlockBegin[NUM_OF_BANK];
    uint16_t	CurSectorInRAM;
    uint16_t  LastPage[NUM_OF_ZONES];
} BRMT_INFO;

typedef struct {
    uint16_t	LSectionNum;
    uint8_t	NoReplace;
    uint8_t	ReplaceBeginRegion;
    uint16_t	ReplaceBeginSection;
    uint16_t	PSectionNumLastR;
    uint16_t	LSectionNumLastR;
    uint8_t	RegionNum;
    uint16_t	RegionInitMask;
    LSMT_INFO	LSMT[NUM_OF_REGION];
} ZONE_INFO;

typedef struct {
    uint8_t	  	PageNumInBuffer;
    uint8_t    	CurActive;
    uint8_t	  	WriteProtect;
    uint8_t	  	PMTInRAMModified;
    uint8_t	  	NewActive;
    uint16_t       	TotalPSectionNum;
    ERASE_QUEUE	  	EraseQueue;
    ZONE_INFO	  	ZoneInfo[NUM_OF_ZONES];
    BRMT_INFO	  	BRMTInfo;
    ACTIVE_INFO	  	ActiveInfo[NUM_ACTIVESLOT];
    FDM_TABLE	  	FDMTable;
    PMT_INFO	  	PMTInfo;
    WEAR_INFO	  	WearInfo;
    uint16_t	  	DATA_PSection;
    uint16_t	  	DataBank[NUM_OF_BANK];
    uint16_t	  	NewBank[NUM_OF_BANK];
    uint16_t	  	BMRBank[NUM_OF_BANK];
    uint16_t	  	NonBMRBank[NUM_OF_BANK];
    uint16_t	  	NewPSect;
    uint16_t	  	ActiveOrder;
#if defined(__NANDFDM_MULTI_INSTANCE__)
uint8_t    	DiskIndex;
uint8_t	  	is_mount;
#endif // defined(__NANDFDM_MULTI_INSTANCE__)
} NANDFDM5_DATA;

#endif // defined(__NANDFDM_TOTAL_BBM__)

    typedef struct {
#ifndef __MTK_TARGET__
//const char	   *file_name;
//void	    *flash_device;
#endif
//	uint32_t	   *MainBuf;
//	uint32_t	   *SpareBuf;
FAT_INFO	  	flash_info;
#if defined(__NANDFDM_MULTI_INSTANCE__)
NANDFDM5_DATA   	NAND_FDMData;
FLASH_INFO    	FlashInfo;
ADDR_INFO    	CurAddr;
#endif // defined(__NANDFDM_MULTI_INSTANCE__)
} NAND_FLASH_DRV_DATA;

// FDM I/F function declaration



        int32_t FDM5_MountDevice	 (void *DriveData, int32_t DeviceNumber, int32_t DeviceType, uint32_t Flags);
        int32_t FDM5_ShutDown	  (void *DriveData);
        int32_t FDM5_ReadSectors	 (void *DriveData, uint32_t Sector, uint32_t Sectors, void *Data);
        int32_t FDM5_WriteSectors	 (void *DriveData, uint32_t Sector, uint32_t Sectors, void *Data);
        int32_t FDM5_MediaChanged	 (void *DriveData);
        int32_t FDM5_DiscardSectors	(void *DriveData, uint32_t Sector, uint32_t Sectors);
        int32_t FDM5_GetDiskGeometry	(void *DriveData, FS_PartitionRecord *DiskGeometry, uint8_t *MediaDescriptor);
        int32_t FDM5_LowLevelFormat	(void *DriveData, const char *DeviceName, FS_FormatCallback Progress, uint32_t Flags);
//int32_t FDM5_CopySectors	  (void *DriveData, uint32_t SrcSector , uint32_t DstSector, uint32_t Sectors);
//int32_t FDM5_DiskNeedFormat	(void * DriveData);
//int32_t FDM5_OTPAccess	 (void * DriveData, int32_t accesstype, uint32_t Offset, void * BufferPtr, uint32_t Length);
//int32_t FDM5_OTPQueryLength	(void * DriveData, uint32_t *LengthPtr);
        int32_t FDM5_IOCtrl(void *DriveData, uint32_t CtrlAction, void *CtrlData);
//int32_t FDM5_LSN2PPN(void * DriveData, uint32_t Sector);
//#if defined(_NAND_FLASH_BOOTING_)
//uint32_t EraseRawDataItem(uint32_t block_idx);
//uint32_t ReadRawDataItem(uint32_t block_idx, uint32_t page_idx, uint32_t length, uint8_t* data_ptr);
//uint32_t WriteRawDataItem(uint32_t block_idx, uint32_t page_idx, uint32_t length, uint8_t* data_ptr);
//#endif //#if defined(_NAND_FLASH_BOOTING_)
#if defined(__AUDIO_DSP_LOWPOWER__)
#if 0
void FDM5_Fill_PSTMap(void *DriveData, FS_NANDPMapQuery *pPSTMapQuery);
#endif //#if defined(__AUDIO_DSP_LOWPOWER__)
typedef enum {
BLBRMT_ERRCODE_NOERR             = 0,
BLBRMT_ERRCODE_BRMT_NOT_FOUND    = 0x01,
BLBRMT_ERRCODE_INVALID_BUFSIZE   = 0x02,
BLBRMT_ERRCODE_BRMT_READ_ECCERR  = 0x03,
BLBRMT_ERRCODE_BRMT_ADDR_ERR     = 0x04,
BLBRMT_ERRCODE_BRMT_NO_FREE_BLK  = 0x05,
BLBRMT_ERRCODE_BRMT_FDMVER_ERR   = 0x06,
BLBRMT_ERRCODE_BRMT_PROGRAM_FAIL = 0x07

    } BLBRMT_Error_Code;
    uint32_t FDM5_BLReadBRMT(uint8_t *pBuf, uint32_t BufLen, uint32_t *ReturnLen);
    uint32_t FDM5_BLWriteBRMT(uint8_t *pBuf, uint32_t BufLen);

#endif

#else // __NAND_FDM_50__


#include "NAND_MTD.h"
#include "NAND_MTD_FDM50_internal.h"


#if !defined __NFI_VERSION2__
#define LSN_INVALID           (DWORD_INVALID)
#else
#define LSN_INVALID           (0x00FFFFFF)
#endif


// FIXME - Remember to rocover correct value check in.
#define POOL_SIZE                12    // used for non-data blocks in a region (double in first region)
#define GC_THRESHOLD             25
#define TRY_TIMES                3


#define MAX_REGION_NUM           72    // max disk volumn = 2GB
#define MAX_PAGES_PER_BLOCK      64
#define MAX_SECTORS_PER_PAGE     4

#define MAX_PAGE_SIZE            2112
#define MAX_SPARE_SIZE           64
#define SECTOR_SIZE              512
#define MAX_CTRL_LOG_NUM         5   /*Max number of control_log block*/
/* page size */
enum {
    PAGE_512,
    PAGE_2K
};


/*******************************************************************************
 * block/page/sector state ID
 *******************************************************************************/
#define BLOCK_EMPTY              0xFF
#define BLOCK_CONTROL            0xFC
#define BLOCK_ALLOCATED          0xAF    /*use to avoid double allocate at LocateBlock or LocateEmptyBlock*/
#define BLOCK_ACTIVE             0xAE
#define BLOCK_DATA               0xAC
#define BLOCK_BAD                0x00
#define BLOCK_NG_CONTROL         0xC3
#define BLOCK_CTRL_LOG           0xC0    // Old control block, reserve as a record
#define BLOCK_COPY_DST           0xAA    /*copy destination block*/
#define BLOCK_MERGE_BMR0         0xA9    /*active merge destination when BMR = 0*/
#define BLOCK_MERGE_BMR1         0xA8    /*active merge destination when BMR = 1*/
#define BLOCK_ERASING            0x07    /*Ready to erase*/

#define PAGE_EMPTY               0xFF
#define PAGE_DATA                0x0F
#define PAGE_DELETED             0x00
#define PAGE_LOG                 0x3F   /*For log section - 2007/05/29 Yehudi*/

#define SECTOR_EMPTY             0xFF
#define SECTOR_DATA              0x0F
#define SECTOR_CONTROL           0xF0    // control block header
#define SECTOR_CTRL_LOG          0xC0    // Old control block, reserve as a record
#define SECTOR_BST               0x33    // block status table
#define SECTOR_LBM               0xCC    // logical block mapping
#define SECTOR_SPOT              0x55   // yoda add for reserving spot
#define SECTOR_DELETED           0x00
#define SECTOR_LOG               0x3F   /*For log section - 2007/05/29 Yehudi*/

/*******************************************************************************
 * Macro definition used for code simplification
 *******************************************************************************/
#define R_BLOCKS                256   /* Default(Maximun) value of block number of region. REGION_B(b) is the real block number of region(b)*/
#define B_PAGES                 (D->flash_info.pages_per_block)
#define P_SECTORS               ((D->flash_info.page_type==PAGE_2K) ? 4 : 1)
#define DATA_SIZE               ((D->flash_info.page_type==PAGE_2K) ? 2048 : 512)
#define SPARE_SIZE              ((D->flash_info.page_type==PAGE_2K) ?   64 :  16)
#define LOG_BLOCKS              ((D->flash_info.page_type==PAGE_2K) ? 3 : 6)    /*Block number of log section - Yehudi*/
#define PAGE_SIZE               ((DATA_SIZE)+(SPARE_SIZE))
#define P_SHIFTS                ((D->flash_info.page_type==PAGE_2K) ? 2 : 0)

/* region info */
#define CUR_R                   (D->region_info.current_region)
#define BASE_B                  (D->region_info.base_block)
#define REGION_B(b)             (D->region_info.region_block[(b)])              /*2007/05/26 Yehudi*/

/* control info */
#define LBM(b)                  (D->control_table.block_mapping[(b)])
#define BST(b)                  (D->control_table.block_state[(b)])
#define CNT_PB                  (D->control_info.physical_block)

/* active info */
#define ACT_LB                  (D->active_info.logical_block)
#define ACT_PB                  (D->active_info.physical_block)
#define ACT_LPM(p)              (D->active_info.page_mapping[(p)])

#define IS_VALID_DATA_BLOCK(b)  (LBM(b)!=BYTE_INVALID || (LBM(b)==255 && LBM(255)==b))
#define IS_INVALID_DATA_BLOCK(b)(LBM(b)==BYTE_INVALID && LBM(255) != b)
#define SET_FLAG_FOR_PHY255(log_b,phy_b)\
{\
if( ((uint8_t)(phy_b)) == 255)\
      LBM(255) = (uint8_t)(log_b);\
}

#define IS_2K_CNTL_TABLE(half,ptr)\
   ( ((half)==0) && (((spare_2k*)(ptr))->spare[0].SS == SECTOR_BST)\
      && (((spare_2k*)(ptr))->spare[1].SS == SECTOR_LBM) )\
   || ( ((half)==1) && (((spare_2k*)(ptr))->spare[2].SS == SECTOR_BST)\
      && (((spare_2k*)(ptr))->spare[3].SS == SECTOR_LBM) )

#define IS_2K_CNTL_SPOT(half,ptr)\
   ( ((half)==0) && (((spare_2k*)(ptr))->spare[0].SS == SECTOR_SPOT))\
   || ( ((half)==1) && (((spare_2k*)(ptr))->spare[2].SS == SECTOR_SPOT))

#define IS_2K_NEXT_CNTL_TABLE(half,ptr)\
   ( ((half)==0) && (((spare_2k*)(ptr))->spare[0].SS == SECTOR_EMPTY)\
      && (((spare_2k*)(ptr))->spare[1].SS == SECTOR_EMPTY) )\
   || ( ((half)==1) && (((spare_2k*)(ptr))->spare[2].SS == SECTOR_EMPTY)\
      && (((spare_2k*)(ptr))->spare[3].SS == SECTOR_EMPTY) )

#define IS_512_CNTL_TABLE(spare0,spare1)\
   spare_512_PS(spare0)==SECTOR_BST && spare_512_PS(spare1)==SECTOR_LBM

#define IS_512_CNTL_SPOT(spare0,spare1)\
   spare_512_PS(spare0)==SECTOR_SPOT


#define IS_512_NEXT_CNTL_TABLE(spare0,spare1)\
   spare_512_PS(spare0)==SECTOR_EMPTY && spare_512_PS(spare1)==SECTOR_EMPTY

/* MTK FDM signature mark */
#define BAD_BLOCK_MARK_2K   (0x4242)
#define BAD_BLOCK_MARK_512  (0x42)
#define MTK_SIGNATURE_MARK   (0x4B544D)
#define FDM_SIGNATURE_MARK   (0x4D4446)
#define FDM_CHECK_MARK_1  (0xF0)
#define FDM_CHECK_MARK_2  (0x0F)
#define FDM_NG_CONTROL_BLOCK_MARK  (0x8000)      /*This mark means the correct control block does not exist - Yehudi*/

#define FDM_TABLE_MARK_POSITION         (2)      /*Use by Control Table*/

/* MTK NAND FDM control block header layout */
#define FDM_CONTROL_BACKUP_POSITION     (256)
#define FDM_RESERVED_POOL_SIZE_POSITION (64)    /*Reserve 4 byte - Yehudi*/
#define FDM_VERSION_POSITION (68)               /*Reserve 4 byte - Yehudi*/

//#define FDM_REGION_BLOCK_POSITION (72)          /*Reserve 4 byte - Yehudi*/
//#define FDM_TIME_INFO_POSITION (76)             /*Reserve 8 byte - Yehudi*/
//#define FDM_CBLK_PROGRAM_COUNT_POSITION (80)    /*Reserve 4 byte - Yehudi*/
/*******************************************************************************
 * structure definition for spare area of 2k/512 page-size
 *******************************************************************************/
//#ifdef __MTK_TARGET__
//extern uint8_t INT_QueryExceptionStatus(void);
//#else
//#define INT_QueryExceptionStatus() (KAL_FALSE)

// pragma is used for specific bytes in a structure
//    #pragma pack(1)
//#endif

typedef struct control_block_header_tag {
    uint8_t   mtk_fdm_mark[64];
    uint8_t   erase_pool_size;    /*position 64*/
    uint8_t   reserve1[3];
    uint16_t  version_info;       /*position 68*/
    uint8_t   reserve2[2];
    uint16_t  cur_region_block;   /*position 72*/
    uint16_t  next_region_block;  /*position 74*/
    uint32_t  create_time;        /*position 76*/
    uint64_t  program_count;      /*position 80 (position at 8 byte alignment for PC )*/
} control_block_header; /*The total size should not over FDM_CONTROL_BACKUP_POSITION - Yehudi*/

#if !defined __NFI_VERSION2__
typedef struct spare_2k_component_tag {
    uint16_t     BI;               // block info
    uint8_t     SS;                // sector state
    uint8_t     Reserved0;
    uint32_t    LSN;           // logical sector number
    uint32_t    ECC;
    uint16_t     Reserved;     /*In FDM 4.3, spare[2].Reserved & spare[3].Reserved will put fdm_data_time_info_struct*/
    uint16_t     LSN_Chk;
} spare_2k_component;

/* Use spare[3]'s Reserved0 as page state */
#define spare_2k_PS(p)          ( ((spare_2k*)(p))->spare[3].Reserved0 )
#define spare_2k_LCHK(p, s)     ( ((spare_2k*)(p))->spare[s].LSN_Chk )
#define spare_2k_VER(p)         ( ((spare_2k*)(p))->spare[0].LSN_Chk )
#define spare_2k_HCHK(p)        ( ((spare_2k*)(p))->spare[0].Reserved0 )

#else
typedef struct spare_2k_component_tag {
    uint16_t     BI;               // block info
    uint8_t     PC;                // Page state/FDM_Chk_MARK
    uint8_t     SS;                // sector state
    uint32_t    LSN: 24;          // logical sector number
    uint32_t    LSN_Chk: 8;          // logical sector number chksum
    uint32_t    ECC[2];
} spare_2k_component;

#define spare_2k_PS(p)          ( ((spare_2k*)(p))->spare[3].PC )
#define spare_2k_LCHK(p, s)     ( ((spare_2k*)(p))->spare[s].LSN_Chk )
#define spare_2k_HCHK(p)        ( ((spare_2k*)(p))->spare[0].PC )

#endif

typedef struct spare_2k_tag {
    spare_2k_component spare[4];
} spare_2k;

#if !defined __NFI_VERSION2__
typedef struct spare_512_tag {
    uint32_t    ECC;
    uint8_t     SS;         // sector state
    uint8_t     BI;         // block info
    uint8_t     Reserved[4]; /*In FDM 4.3, it will put fdm_data_time_info_struct*/
    uint16_t    LSN_Chk;
    uint32_t    LSN;        // logical sector number
} spare_512;

#define spare_512_PS(p)                 ( ((spare_512*)(p))->SS )
#define spare_512_LCHK(p)               ( ((spare_512*)(p))->LSN_Chk )
#define spare_512_VER(p)                ( ((spare_512*)(p))->LSN_Chk )
#define spare_512_HCHK(p)               ( ((spare_512*)(p))->Reserved[0] )

#else
typedef struct spare_512_tag {
    uint32_t    LSN: 24;          // logical sector number
    uint32_t    LSN_Chk: 8;      // logical sector number chksum
    uint8_t     Reserved[1];
    uint8_t     BI;              // block info
    uint8_t     PC;              // FDM_Chk_MARK
    uint8_t     SS;              // sector state
    uint32_t    ECC[2];
} spare_512;

#define spare_512_PS(p)                 ( ((spare_512*)(p))->SS )
#define spare_512_LCHK(p)               ( ((spare_512*)(p))->LSN_Chk )
#define spare_512_HCHK(p)               ( ((spare_512*)(p))->PC )

#endif

//#ifndef __MTK_TARGET__
//#pragma pack()
//#endif

/*******************************************************************************
 * structure definition for driver data
 *******************************************************************************/
typedef enum {
    BBMARK_1, // First and second page of each block.
    BBMARK_2	// First and last page of each block.
} BBMARK_STYLE;

typedef struct flash_info_tag {
    uint32_t    id_data[2];
    uint16_t     block_num;            /*total FAT block number, not include log block - Yehudi*/
    uint8_t     region_num;
    uint8_t     pages_per_block;
    uint8_t     page_type;
    uint8_t       address_cycle;
    uint8_t       io_width;
    uint8_t       log_block;           /*block number of log section - Yehudi*/
    uint16_t     bad_num;
    uint32_t    total_FAT_sectors;     // total sectors that FAT can use
    uint32_t    partition_sectors;     // partiton sectors that FAT can use
    uint32_t    region_sectors[MAX_REGION_NUM];  // LBB design
    uint32_t	  TotalSize;
    BBMARK_STYLE  BBMarkStyle;
} flash_info_struct;


typedef struct region_info_tag {

    uint16_t     control_block[MAX_REGION_NUM];
    uint16_t     region_block[MAX_REGION_NUM + 1]; /*block number of the region - Yehudi*/
    uint32_t     base_block;
    /* Adjust to be 4 byte alignment */

    uint8_t     control_next_index[MAX_REGION_NUM]; // next programmable page
    uint8_t     current_region;
    uint8_t     erase_pool[MAX_REGION_NUM];   // LBB design
    uint32_t    base_LSN;
    uint32_t    end_LSN;
} region_info_struct;


typedef struct control_info_tag {
    uint16_t     physical_block;
    uint8_t     next_index; // next programmable page index ?? -- kirk
    uint8_t     modified_flag;
    uint16_t     version;
} control_info_struct;


typedef struct control_table_tag {
    uint8_t     block_state[R_BLOCKS];        // physical block state
    uint8_t     erase_count[R_BLOCKS];        // erase count of physical block
    uint8_t     block_mapping[R_BLOCKS];      // map logical to physical block
} control_table_struct;

typedef struct ctrl_table_extra_info_tag {
    uint8_t   reserve1[2];
    uint8_t   table_mark;             /* controbl table mark, must at position FDM_TABLE_MARK_POSITION*/
    uint8_t   reserve2[9];
    uint32_t  create_time;            /*position 12*/
    uint64_t  program_count;          /*position 16 (position at 8 byte alignment for PC )*/
    uint16_t  active_logical_block;
    uint16_t  active_physical_block;
    uint8_t   active_bmr_flag;
    uint8_t   active_next_index;                            // next programmable page index
    uint32_t  code_md5[16 / 4];        /*position 32: MD5 of BST and LBM, size must match 2rd parameter of CHE_md5_finish() */ /*MAUI_00618203*/
} ctrl_table_extra_info;

typedef enum {
    CLOSE_REGION_MARK      = 0x20,
    NORMAL_SHUTDOWN_MARK   = 0x31,
    MERGEACTIVE_MARK1      = 0x40,
    MERGEACTIVE_MARK2      = 0x42,
    MERGEACTIVE_MARK3      = 0x44,
    RECONSTRUCTED_MARK     = 0x52,
    COPY_MARK              = 0x60,
    SPOT_MARK              = 0x62,
    NONE_MARK              = 0xFF
} ControblTableMark;

typedef struct active_info_tag {
    uint16_t     logical_block;                         // logical number of active block
    uint16_t     physical_block;                        // physical number of active block
    uint8_t     next_index;                            // next programmable page index
    uint8_t     bmr_flag;                              // indicate if active block fits block mappingn rule
    /*uint8_t     first_page;*/
    uint8_t     page_mapping[MAX_PAGES_PER_BLOCK];     // map logical to physical page
} active_info_struct;


typedef struct sequential_info_tag {
    uint16_t     physical_block; // which physical block this information is for.
    uint8_t     last_index;     // The page offset on which the last programming operation is performed.
} seq_info_struct;


typedef struct dup_block_tag {
    uint16_t l_block;
    uint16_t p_block;
} dup_block_struct;

typedef struct {
    int32_t     (* MountDevice)    (void *DriveData);
    int32_t     (* ShutDown)       (void *DriveData);
    int32_t     (* EraseBlock)     (void *DriveData, uint32_t BlockIndex);
    int32_t     (* ProgramPage)    (void *DriveData, uint32_t PageIndex, uint8_t *Data, uint8_t *Spare, uint8_t ECCFlag);
    int32_t     (* ReadPage)       (void *DriveData, uint32_t PageIndex, uint8_t *Data, uint8_t *Spare, uint8_t SpareFlag, uint8_t ECCFlag);
    int32_t     (* ProgramSpare)   (void *DriveData, uint32_t PageIndex, uint8_t *Data);
    int32_t     (* ReadSpare)      (void *DriveData, uint32_t PageIndex, uint8_t *Data);
    int32_t     (* CopyPage)       (void *DriveData, uint32_t SrcPageIndex, uint32_t DestPageIndex);
    int32_t     (* OTPAccess)      (void *DriveData, int32_t accesstype, uint32_t Offset, void *BufferPtr, uint32_t Length);
    int32_t     (* OTPQueryLength) (void *DriveData, uint32_t *LengthPtr);
} NAND_MTD_DRIVER;


typedef  struct {
    NAND_MTD_DRIVER          *MTD;
#if 0
#ifndef __MTK_TARGET__
    const char               *file_name;
    void                     *flash_device;
#endif
#endif
    flash_info_struct        flash_info;
    region_info_struct       region_info;
    control_info_struct      control_info;
    control_table_struct     control_table;
    active_info_struct       active_info;
    seq_info_struct          seq_info;

    uint32_t                 buffer[MAX_PAGE_SIZE / sizeof(uint32_t)];
    uint32_t                 spare0[MAX_SPARE_SIZE / sizeof(uint32_t)];
    uint32_t                 spare1[MAX_SPARE_SIZE / sizeof(uint32_t)];
    //uint32_t                     badspare[MAX_SPARE_SIZE/sizeof(uint32_t)];//TODO: should check whether this is necessary

} NAND_FLASH_DRV_DATA;

typedef struct fdm_global_time_info_tag {
    uint32_t Second: 6;
    uint32_t Minute: 6;
    uint32_t Hour: 5;
    uint32_t Day: 5;
    uint32_t Month: 4;
    uint32_t Year2000: 6;
} fdm_global_time_info_struct;


typedef struct fdm_data_time_info_tag {
    uint32_t Count: 9;
    uint32_t Second: 6;
    uint32_t Minute: 6;
    uint32_t Hour: 5;
    uint32_t Day: 5;
    uint32_t Month: 1;
} fdm_data_time_info_struct;

typedef struct fdm_log_section_info_tag {
    uint8_t     is_init;
    uint32_t    log_start_page;
    uint32_t    total_page_num;
    uint32_t    cur_page;
    uint64_t    program_count;  /*position 16 (position at 8 byte alignment for PC simulator) */
    uint32_t    buffer[MAX_PAGE_SIZE / sizeof(uint32_t)];
    uint32_t    spare0[MAX_SPARE_SIZE / sizeof(uint32_t)];
    uint32_t    spare1[MAX_SPARE_SIZE / sizeof(uint32_t)];
} fdm_log_section_info_struct;
/*******************************************************************************
 * Definition for power loss test
 *******************************************************************************/
#if defined(FLASH_DISKDRV_DEBUG) && defined(POWERLOSS_TEST)

enum {
    MERGE_ACTIVE = 1,
    COPY_BLOCK,
    WRITE_SECTORS,
    FLUSH_TABLE,
};

#include <setjmp.h>
extern jmp_buf mark;
extern int32_t Test_CD_Value;
#define TEST_POINT(a)\
   {\
      switch(a)\
      {\
         case MERGE_ACTIVE:\
            Test_CD_Value-=1;\
                break;\
         case COPY_BLOCK:\
            Test_CD_Value-=1;\
                break;\
         case WRITE_SECTORS:\
            Test_CD_Value-=1;\
                break;\
         case FLUSH_TABLE:\
            Test_CD_Value-=1;\
                break;\
         default:\
            Test_CD_Value-=1;\
            break;\
       }\
      if(Test_CD_Value <= 0)\
         longjmp(mark, a);\
   }

#define START_POINT  setjmp(mark)

#else
#define TEST_POINT(a)
#define START_POINT
#endif

#if defined(FLASH_DISKDRV_DEBUG) && defined(ERROR_HANDLING_TEST)

/* for error handling test */

extern void ProgramPageFailure(int32_t times);
extern void ProgramSpareFailure(int32_t times);
extern void BlockEraseFailure(int32_t times);
extern void ReadPageFailure(int32_t times);
extern void ReadSpareFailure(int32_t times);

typedef enum {
    PAGE_WRITE,
    SPARE_WRITE,
    PAGE_READ,
    SPARE_READ,
    BLOCK_ERASE,
    TOTAL_OPERATIONS
} Operation_Type;

typedef enum {
    WRITEDATA_ACTIVE = 0,
    WRITEDATA_BLOCK,
    COPY_BLOCK,
    FLUSH_TABLE,
    MODIFY_TABLE,
    MERGE_ACTIVE,
    DELETE_PAGE,
    MARK_BAD_BLOCK,
    DISCARD_SECTOR,
    INITIALIZE_REGION_DATA,
    READSECTOR_512,
    READ_SECTOR_2K,
    READ_TABLE,
    READDATA_ACTIVE,
    READDATA_BLOCK,
    FIND_CONTROL_BAD_BLOCK,
    ERASE_BLOCK,
    FIND_TABLE,
    BLOCK_BMR,
    DELETEDATA_ACTIVE,
    DELETEDATA_BLOCK,
    POWER_RECOVERY,
    RECONSTRUCT_TABLE,
    RESTORE_DD,
    TOTAL_FUNCTIONS
} ErrorHandling_TestCase;

typedef struct {
    uint32_t                triggered;  // test enabled or not
    uint32_t                step_id;    // determine which step is allowed to go
    ErrorHandling_TestCase    case_type;
    Operation_Type            op_type;
} test_command;

extern test_command test_item[];

#define TEST_ERROR_ENABLE(function, operation, step)\
{\
   test_item[function].triggered = 1;\
   test_item[function].op_type = operation;\
   test_item[function].case_type = function;\
   test_item[function].step_id = step;\
}

#define TEST_ERROR_DISABLE(function, operation, step)\
{\
   if( (function == test_item[function].case_type) &&\
       (step == test_item[function].step_id) )\
   {\
      test_item[function].triggered = 0;\
      test_item[function].op_type = TOTAL_OPERATIONS;\
      test_item[function].case_type = TOTAL_FUNCTIONS;\
      test_item[function].step_id = 0;\
   }\
}

/*   assert( function == test_item[function].case_type);\*/
#define TEST_ERROR_EVENT(function, step)\
{\
   if( test_item[function].triggered )\
   {\
      if( step == test_item[function].step_id )\
      {\
         if( PAGE_WRITE == test_item[function].op_type )\
         {\
            LOG_I(common, "Page program error is triggered!\n");\
            ProgramPageFailure(1);\
            test_item[function].triggered = 0;\
         }else if( SPARE_WRITE == test_item[function].op_type )\
         {\
            LOG_I(common,"Spare program error is triggered!\n");\
            ProgramSpareFailure(1);\
            test_item[function].triggered = 0;\
         } else if( PAGE_READ == test_item[function].op_type )\
         {\
            LOG_I(common,"Page read error is triggered!\n");\
            ReadPageFailure(1);\
            test_item[function].triggered = 0;\
         } else if( SPARE_READ == test_item[function].op_type )\
         {\
            LOG_I(common,"Spare read error is triggered!\n");\
            ReadSpareFailure(1);\
            test_item[function].triggered = 0;\
         } else if (BLOCK_ERASE == test_item[function].op_type)\
         {\
            LOG_I(common,"Block erase error is triggered!\n");\
            BlockEraseFailure(1);\
            test_item[function].triggered = 0;\
         }\
      }\
   }\
}

#else
#define TEST_ERROR_EVENT(function, step)
#define TEST_ERROR_ENABLE(function, operation, step) //Keneng
#define TEST_ERROR_DISABLE(function, operation, step) //Keneng
#endif


/*******************************************************************************
 *  FDM function declaration
 *******************************************************************************/
int32_t MountDevice       (void *DriveData, int32_t DeviceNumber, int32_t DeviceType, uint32_t Flags);
int32_t ShutDown          (void *DriveData);
int32_t ReadSectors       (void *DriveData, uint32_t Sector, uint32_t Sectors, void *Data);
int32_t WriteSectors      (void *DriveData, uint32_t Sector, uint32_t Sectors, void *Data);
int32_t MediaChanged      (void *DriveData);
int32_t DiscardSectors    (void *DriveData, uint32_t Sector, uint32_t Sectors);
int32_t GetDiskGeometry   (void *DriveData, FS_PartitionRecord *DiskGeometry, uint8_t *MediaDescriptor);
int32_t LowLevelFormat    (void *DriveData, const char *DeviceName, FS_FormatCallback Progress, uint32_t Flags);
int32_t PowerOnAssert_DriveFormat(void *DriveData);
int32_t CopySectors(void *DriveData, uint32_t SrcSector , uint32_t DstSector, uint32_t Sectors);
int32_t FDM4_OTPAccess	 (void *DriveData, int32_t accesstype, uint32_t Offset, void *BufferPtr, uint32_t Length);
int32_t FDM4_OTPQueryLength	(void *DriveData, uint32_t *LengthPtr);
int32_t FDM4_IOCtrl(void *DriveData, uint32_t CtrlAction, void *CtrlData);


/*******************************************************************************
 *  Sub-function declaration
 *******************************************************************************/
int32_t FindControlBadBlock(NAND_FLASH_DRV_DATA *D, uint8_t *control_num);
uint8_t IsGoodBlock(NAND_FLASH_DRV_DATA *D, void *Spare);
uint8_t ReadSector_512(NAND_FLASH_DRV_DATA *D, uint32_t Sector, uint8_t *Data);
uint8_t ReadSector_2k(NAND_FLASH_DRV_DATA *D, uint32_t Sector, uint32_t read_sectors, uint8_t *Data);
uint8_t WriteSector_512(NAND_FLASH_DRV_DATA *D, uint32_t Sector, uint8_t *Data);
uint8_t WriteSector_2k(NAND_FLASH_DRV_DATA *D, uint32_t Sector, uint32_t write_sectors, uint8_t *Data);
uint32_t MapSector(NAND_FLASH_DRV_DATA *D, uint32_t Sector);
void  LocateRegion(NAND_FLASH_DRV_DATA *D, uint32_t Sector);
uint8_t InitializeRegionData_byReadTableInfo(NAND_FLASH_DRV_DATA *D);
void  InitializeRegionData(NAND_FLASH_DRV_DATA *D);
void  SelfFormat(NAND_FLASH_DRV_DATA *D, int32_t DeviceType);
uint8_t  DeletePage(NAND_FLASH_DRV_DATA *D, uint32_t PageIndex);
uint8_t  EraseBlock(NAND_FLASH_DRV_DATA *D, uint32_t BlockIndex);
uint8_t  MarkBadBlock(NAND_FLASH_DRV_DATA *D, uint32_t BlockIndex);
uint8_t  LocateBlock(NAND_FLASH_DRV_DATA *D);
uint8_t  LocateEmptyBlock(NAND_FLASH_DRV_DATA *D);
uint8_t  CopyBlock(NAND_FLASH_DRV_DATA *D, uint8_t sourceB, uint8_t sourceP, uint8_t *destinationB, uint8_t destinationP, uint8_t num);
int32_t  FlushTable(NAND_FLASH_DRV_DATA *D, ControblTableMark mark_flag);
uint8_t  ModifyTable(NAND_FLASH_DRV_DATA *D);
uint8_t  FindTable(NAND_FLASH_DRV_DATA *D);
uint8_t  ReadTable(NAND_FLASH_DRV_DATA *D);
uint8_t  Read_FoundTable(NAND_FLASH_DRV_DATA *D);
void  LocatePage(NAND_FLASH_DRV_DATA *D, uint32_t Sector, uint8_t readback);
uint8_t  BlockBMR(NAND_FLASH_DRV_DATA *D, uint16_t Block , uint8_t Offset);
uint32_t Writedata_Active(NAND_FLASH_DRV_DATA *D, uint8_t Offset, uint8_t *Data, uint8_t *Spare);
uint32_t Writedata_Block(NAND_FLASH_DRV_DATA *D, uint16_t Block, uint8_t Offset, uint8_t *Data, uint8_t *Spare);
void  Deletedata_Active(NAND_FLASH_DRV_DATA *D, uint8_t Offset);
void  Deletedata_Block(NAND_FLASH_DRV_DATA *D, uint16_t Block, uint8_t Offset);
void  Readdata_Active(NAND_FLASH_DRV_DATA *D, uint8_t Offset);
void  Readdata_Block(NAND_FLASH_DRV_DATA *D, uint16_t Block, uint8_t Offset);
void  MergeActive(NAND_FLASH_DRV_DATA *D);
void  PowerRecovery(NAND_FLASH_DRV_DATA *D);
void  ReconstructTable(NAND_FLASH_DRV_DATA *D, dup_block_struct dupB[2][2]);
void  DeleteAllControlBlock(NAND_FLASH_DRV_DATA *D);
void  ReconstructAllTable(NAND_FLASH_DRV_DATA *D);
void  RestoreAD(NAND_FLASH_DRV_DATA *D, dup_block_struct *dupB, uint8_t PageState0);
void  RestoreDD(NAND_FLASH_DRV_DATA *D, dup_block_struct *dupB);
uint8_t CreateControlBlock (NAND_FLASH_DRV_DATA *D);
uint8_t LimitControlLogBlockNumber (NAND_FLASH_DRV_DATA *D);
uint8_t MarkControlLogBlock(NAND_FLASH_DRV_DATA *D, uint32_t BlockOffset);
uint8_t Find_TableNewestProgramCounter (NAND_FLASH_DRV_DATA *D, uint8_t region);
uint8_t LogSectionInit (NAND_FLASH_DRV_DATA *D);
uint8_t WriteLog (NAND_FLASH_DRV_DATA *D, char *message);
void  SC(NAND_FLASH_DRV_DATA *D);
void  SC_UT(NAND_FLASH_DRV_DATA *D);

#endif   //__NAND_FDM_50__


    /******************************************************************************
     * SuperAND flash device definition                                           *
     ******************************************************************************/
#if defined(WIN32) && defined(TRACE_ON)
#define FDM_TRACE   printf
#else
#define FDM_TRACE
#endif

#define SUPERAND_SECTOR_SIZE    2048
#define SUPERAND_FAT_API_ENTRANCE \
    Sector += D->first_fat_sector_offset;

#if 0
    typedef struct {
        SuperAND_MTD_DRIVER      *MTD;

#ifndef __MTK_TARGET__
        const char               *file_name;
        void                     *flash_device;
#endif
        uint8_t                    io_width;
        uint32_t                   pages_of_first_device;
        uint32_t                   pages_of_block;
        uint32_t                  block_mask;
        uint32_t                   total_pages;

        uint32_t                   first_fat_sector_offset;
        uint32_t                   total_fat_sectors;
        uint32_t                   partition_sectors;

        uint32_t                   buffer[SUPERAND_SECTOR_SIZE / sizeof(uint32_t)];
    } SuperAND_FLASH_DRV_DATA;

#define SUPERAND_MOUNTED    0x1000

    void SuperAND_FDM_Init(void);
    void SuperAND_SelfFormat(SuperAND_FLASH_DRV_DATA *D, int32_t DeviceType);

    int32_t SuperAND_FDM_MountDevice(void *DriveData, int32_t DeviceNumber, int32_t DeviceType, uint32_t Flags);
    int32_t SuperAND_FDM_ShutDown(void *DriveData);
    int32_t SuperAND_FDM_ReadSectors(void *DriveData, uint32_t Sector, uint32_t Sectors, void *Buffer);
    int32_t SuperAND_FDM_WriteSectors(void *DriveData, uint32_t Sector, uint32_t Sectors, void *Buffer);
    int32_t SuperAND_FDM_MediaChanged(void *DriveData);
    int32_t SuperAND_FDM_DiscardSectors(void *DriveData, uint32_t Sector, uint32_t Sectors);
    int32_t SuperAND_FDM_GetDiskGeometry(void *DriveData, FS_PartitionRecord *DiskGeometry, uint8_t *MediaDescriptor);
    int32_t SuperAND_FDM_GetDiskGeometry(void *DriveData, FS_PartitionRecord *DiskGeometry, uint8_t *MediaDescriptor);
    int32_t SuperAND_FDM_LowLevelFormat(void *DriveData, const char *DeviceName, FS_FormatCallback Progress, uint32_t Flags);
    int32_t SuperAND_FDM_NonBlockWriteSectors(void *DriveData, uint32_t Sector, uint32_t Sectors, void *Buffer);
    int32_t SuperAND_FDM_RecoverableWriteSectors(void *DriveData, uint32_t Sector, uint32_t Sectors, void *Buffer);
    int32_t SuperAND_FDM_ResumeSectorStates(void   *DriveData);
    int32_t SuperAND_FDM_HighLevelFormat(void);
    int32_t SuperAND_FDM_FlushData(void *DriveData);
    int32_t SuperAND_FDM_CopySectors(void *DriveData, uint32_t SrcSector, uint32_t DstSector, uint32_t Sectors);
#endif

#endif /* NAND_FDM_H */

