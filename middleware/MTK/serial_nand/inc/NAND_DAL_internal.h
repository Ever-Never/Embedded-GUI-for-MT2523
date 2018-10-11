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


#ifndef NAND_DAL_INTERNAL_H
#define NAND_DAL_INTERNAL_H

/****************************
 * Include Header Files     *
 ****************************/
//#include "kal_general_types.h"
#include "NAND_MTD_FDM50_internal.h"
//#include "SPI_NAND_MTD_FDM50_internal.h"
//#include "fota_error.h"

typedef struct flash_info_tag_2 {
    flash_list          deviceInfo_CE[2];
    uint8_t           valid_CE[2];    /*flag to identify CE# is valid or not*/
    uint8_t           bank_CE[2];    /*bank number of each device(CE)*/
    uint8_t           zone_CE[2];    /*zone number of each device(CE), [Note] Need to concern CE_interleave_Flag to get total zone number*/
    uint32_t          devicePage[2];
    uint32_t          diePage[2];
    uint32_t          planePage[2];
    uint16_t          supportMPType[2]; /*judged supportMPType */
    uint16_t          supportMEType[2]; /*judged supportMEType */
    uint16_t          pageSize;
    uint16_t          spareSize;
    uint16_t          blockPage;
    uint8_t           bankNum;    /*Total bank number of FDM section or Zone*/
    uint8_t           zoneNum;    /*Total zone number*/
    uint8_t           CE_interleave_Flag;
    uint8_t           CE_cache_interleave_Flag;
    uint8_t           CE_interleave_mOP_Idx[2];
    uint8_t           CE_cache_interleave_mOP_Idx[2];
    mPage_operation     multiProgram[2][MAX_MULTIPAGE_FUNCTION_NUM];
    mPage_operation     multiErase[2][MAX_MULTIPAGE_FUNCTION_NUM];
} flash_info_2;
/*FDM Layer flash information*/
#define NUM_OF_ZONES 8 /*yehudi temp assign*/
#define BBM_GLOBAL_ADDR  0x8000         // The bank address is global not local in region.
typedef struct {
    uint8_t TotalZoneNum;         // This number should exclude the NFB space. That is, only FAT size.
    uint16_t  PSectionNumInZone[NUM_OF_ZONES];  // Section number for each zone. Also, need exclude the NFB space.
    uint8_t BankNumber;
    uint8_t  Plane_PagePair;     /*Flag to identify whether only page pair can process multiplane operation (i.e. samsung\Hynix device)*/
    uint8_t  ExtendBlock;
    uint8_t   SectorNumPerPage;
    uint16_t  PageNumPerSection;
    uint8_t SectionShiftNum;        // For example, 64 pages/section=> shift number 6, 256 pages/section=> shift number 8
#if defined(__NANDFDM_MULTI_INSTANCE__)
    uint32_t  TotalPSectionNumInZone;         // This number is the physical number. No need to care the NFB, FS, etc.
#endif // defined(__NANDFDM_MULTI_INSTANCE__)
} FLASH_INFO;

//#define PROGRAM_WO_WAIT_LAST_BUSY   0x01
//WAIT_LAST_BUSY
//FULL_OPERATION

typedef struct {
    uint8_t  zoneNum;  /*input*/
    uint16_t *bankArry;/*input*/
    uint32_t pageIdx; /*input*/
    uint32_t phyPage; /*output*/
    uint8_t  ce;      /*output*/
} add_convert;

typedef struct {
    uint8_t  zoneNum;  /*input*/
    uint8_t  ce;      /*output*/
    uint16_t blockNum; /*input*/
    uint32_t zoneOffset; /*output*/
    uint32_t bankOffset; /*output*/
} struct_map_cache ;


#if defined(__NFI_VERSION3_1__)

typedef struct {
    uint8_t   BI;
    uint8_t   R; /*reserve*/
    uint16_t    BM_PA;
    uint16_t    LBA;
    uint16_t    PBA;
} FDMData_512;

typedef struct {
    uint16_t    BI;
    uint16_t    BM_PA;
    uint16_t    LBA;
    uint16_t    PBA;
} FDMData_2k;
#else // defined(__NFI_VERSION3_1__)

typedef struct {
    uint16_t    LBA;
    uint16_t    BM_PA;
    uint8_t   R; /*reserve*/
    uint8_t   BI;
    uint16_t    PBA;
} FDMData_512;

typedef struct {
    uint16_t    BI;
    uint16_t    BM_PA;
    uint16_t    LBA;
    uint16_t    PBA;
} FDMData_2k;

#endif // defined(__NFI_VERSION3_1__)


typedef struct {
    uint8_t     Reserved1[4];     // reserved area (non-ECC protected)
    uint16_t    BI;               // byte 4, 5
    uint16_t    BM_PA;            // byte 6, 7
    uint16_t    LBA;              // byte 8, 9
    uint16_t    PBA;              // byte A, B
    uint8_t     ECC_0[4];
    uint8_t     Unused[16 * 3];
} FDMDataU_2K_MTK_1;

typedef struct {
    uint8_t     Reserved1[8];     // reserved area (non-ECC protected)
    uint16_t    BI;               // byte 8, 9
    uint16_t    BM_PA;            // byte A, B
    uint16_t    LBA;              // byte C, D
    uint16_t    PBA;              // byte E, F
    uint8_t     Unused[16 * 3];
} FDMDataU_2K_MTK_2;

typedef enum {
    CODE_ACC,
    FAT_ACC,
    RAW_ACC
} DAL_ACC_Type;

typedef enum {
    SNAND_TRR_NO_ERROR                         = 0
    , SNAND_TRR_ERR_GOOD_BLOCK_UNAVAILABLE       = (-1)
    , SNAND_TRR_ERR_LOG_BLOCK_MARK_NOT_FOUND     = (-2)
    , SNAND_TRR_ERR_SRC_BLOCK_NOT_FOUND          = (-3)
    , SNAND_TRR_ERR_DISABLED                     = (-4)
} SNAND_TRR_ERROR_CODE;

#define FDM_BAD_BLOCK_MARK_2K   (0xF0F0)
#define FDM_BAD_BLOCK_MARK_512  (0xF0)
#define ZERO_OF_FDM_BAD_BLOCK_MARK_2K   (8)
#define ZERO_OF_FDM_BAD_BLOCK_MARK_512  (4)

#define FDMDATA_BM		0x01
#define FDMDATA_PA		0x02
#define FDMDATA_LBA		0x04
#define FDMDATA_BI		0x08
#define FDMDATA_BBPBA	0x10
#define FDMDATA_4XCTL	0x20
#define FDMDATA_EMPTY	0x80




//#define __EMULATE_NFB_BIT_ERROE__
#if ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) && !defined(__UBL__) )
//#define ECC_BIT_ERROR_TEST
#endif // ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) && !defined(__UBL__) )
//#define ENABLE_ADVANCE_OPERATION

#if !defined DWORD_INVALID
#define DWORD_INVALID       0xFFFFFFFF
#endif
#if !defined WORD_INVALID
#define WORD_INVALID            0xFFFF
#endif
#if !defined BYTE_INVALID
#define BYTE_INVALID            0xFF
#endif

uint32_t DAL_FDM_Flash_Info_Init ( FLASH_INFO *fdm_flash_info, uint32_t DiskIndex);

/**************************************
 * Returns:     Low 2 byte: error Page
 *              High 2 byte: error code
 *************************************/
uint32_t DAL_ProgramPage(DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *Bank, uint16_t PageAddr,
                         uint16_t PageLength, uint8_t *Data, uint8_t *Spare);

uint32_t DAL_ReadPage(DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *Bank, uint16_t PageAddr, uint16_t PageLength,
                      uint8_t *Data, uint8_t *Spare);

uint32_t DAL_CopyPage(DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *srcBank, uint16_t *dstBank, uint16_t srcPageAddr,
                      uint16_t dstPageAddr, uint16_t PageLength, uint8_t *Data_buff, uint8_t *Spare_buff, uint32_t buff_len,
                      uint8_t SDataNumber);

uint32_t DAL_EraseBlock(DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *Bank, uint8_t start, uint8_t length);

uint32_t DAL_MarkBlockBad(DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *Bank, uint8_t Block_offset, uint8_t EraseBlock,
                          uint32_t *Data_buff, uint32_t *Spare_buff);
/**************************************
 * Returns:      MTD_STATUS_NO_ERROR - The block is a Good block
 *               MTD_STATUS_EARLY_BAD - The block is a bad block of early bad
 *               MTD_STATUS_FDM_BAD - The block is a bad block of FDM bad
 *               (MTD_STATUS_EARLY_BAD|MTD_STATUS_FDM_BAD)
 *************************************/
uint32_t DAL_CheckBadBlock (DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *Bank, uint8_t Block_offset,
                            uint8_t newFlash, uint32_t *Data_buff, uint32_t *Spare_buff);

uint32_t DAL_NAND_Reset(void);

#if defined(FLASH_DISKDRV_DEBUG) || (defined _NAND_FLASH_BOOTING_)
int32_t DAL_NFB_AddressConvert(uint32_t Block, uint32_t Page, uint8_t *cs_p, uint32_t *page_idx_p);
#endif
/*=====*/
uint16_t DAL_GetFDMData(uint8_t FDMType, uint32_t *Spare_buff) ;

void DAL_SetFDMData(uint16_t BMPA, uint16_t LBA, uint8_t Option, uint32_t *Spare_buff);

uint8_t DAL_GetDALVersion(void);

uint8_t DAL_FormatQuery(uint8_t dal_ver);

void DAL_GetDeviceInfo (uint8_t cs, flash_list *p);


uint32_t addressMap(add_convert *AddInfo);
int32_t DAL_ShutDown (void);

uint32_t DAL_OTPAccess(uint32_t accesstype, uint32_t Offset, void *BufferPtr, uint32_t Length);
uint32_t DAL_OTPQueryLength(uint32_t *LengthPtr);
uint32_t DAL_GetDeviceIDInfo(void *DeviceInfo);
uint32_t DAL_LPage2PPage(DAL_ACC_Type Acc_Type, uint8_t ZoneNo, uint16_t *Bank, uint16_t PageAddr);

#if ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) )

typedef struct {
    uint8_t				Zone;
    uint16_t				Block;
    uint16_t				Bank;
    uint16_t				Page;
} BAD_ADDR;

#define PF_DAL_TRIGGER	0x00000001
#define PF_MTD_TRIGGER  0x00000002
#define PF_BRMT_UPDATE	0x80000000
#define PF_LOW_FORMAT	0x40000000
#define PF_MERGE_ALL	0x20000000
#define PF_ACT_MERGEBMR 0x10000000
#define PF_NONBMR_INIT	0x08000000
#define PF_BMR_INIT		0x04000000
#define PF_WRITE_LSA	0x02000000
#define PF_PMT_SAVE		0x01000000
#define PF_REG_SAVE		0x00800000
#define PF_ACT_WRITE_1	0x00400000
#define PF_ACT_WRITE_2	0x00200000
#define PF_ACT_WRITE_3	0x00100000
#define PF_ACT_MOVENEW	0x00080000
#define PF_WL_MOVENEW	0x00040000
#define PF_PMT_SAVE_1	0x00020000
#define PF_MERGEBMR_ALL 0x00010000
#define PF_RAW_WRITE    0x00008000
#define PF_RAW_COPY     0x00004000


#define EF_DAL_TRIGGER	0x00000001
#define EF_MTD_TRIGGER  0x00000002
#define EF_BRMT_UPDATE1	0x80000000
#define EF_BRMT_UPDATE2	0x40000000
#define EF_LOW_FORMAT	0x20000000
#define EF_ERASE_RETURN 0x10000000
#define EF_WRITE_LSA	0x08000000
#define EF_RAW_ERASE    0x04000000
#define EF_RAW_WR_ERASE 0x02000000

extern uint8_t PFTrigger;
extern uint8_t EFTrigger;
extern uint32_t PFTestCase;
extern uint32_t EFTestCase;
// Program fail and Erase fail auto test . kuohong 2010.1.16
extern uint8_t PF_EF_AutoTest;
extern uint32_t PFTrigerByTestCase;
extern uint32_t EFTrigerByTestCase;



extern BAD_ADDR EraseBadAddr[];
extern BAD_ADDR ProgBadAddr[];

#endif // #if ( defined(__NAND_FLASH_HARDWARE_TEST__) && defined(__MAUI_BASIC__) && !defined(__UBL__))

#endif //NAND_DAL_INTERNAL_H
