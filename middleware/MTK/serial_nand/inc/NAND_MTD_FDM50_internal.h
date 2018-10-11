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

#ifndef NAND_MTD_FDM50_INTERNAL_H
#define NAND_MTD_FDM50_INTERNAL_H

/*******************************************************************************
 * Include Header Files
 *******************************************************************************/
//#include "mt2523.h"
#include "NAND_DevConfig.h"

/*******************************************************************************
 * Compiler definition
 *******************************************************************************/
#if 0
#ifdef __MTK_TARGET__
#if defined (__ARMCC_VERSION)
#define __NAND_INLINE __inline
#elif defined (__GNUC__)
#define __NAND_INLINE inline
#endif /* ARMCC , GNUC */
#else  /* __MTK_TARGET__ */
#define __NAND_INLINE __inline
#endif /* __MTK_TARGET__ */
#endif

/*******************************************************************************
 * Message Log definition
 *******************************************************************************/
#if 0
#if defined(__UBL__) || defined(__FUE__)
#define nandlog_print
#elif defined(__MAUI_BASIC__)
#define nandlog_print dbg_print
#else
#define nandlog_print tst_sys_trace
#endif
#endif

/*******************************************************************************
 * define
 *******************************************************************************/

#ifndef NODEBUG
#define DEBUG
#endif

#if 0
#ifdef _MSC_VER
#define INTERN _fastcall
#define WATCOM_VOLATILE
#elif defined __BORLANDC__
#define INTERN __fastcall
#define WATCOM_VOLATILE
#else
#define INTERN
#define WATCOM_VOLATILE volatile   // Watcom compiler bug workaround
#endif
#endif

/*MTD status value must keep the low 16 bit to zero*/
#define MTD_STATUS_NO_ERROR           (0x00000000)
#define MTD_STATUS_ERROR              (0x00010000)
#define MTD_STATUS_CORRECTED          (0x00020000)
#define MTD_STATUS_EARLY_BAD          (0x00040000)
#define MTD_STATUS_FDM_BAD            (0x00080000)
#define MTD_STATUS_ID_NOT_SUPPORT     (0x00100000)
#define MTD_STATUS_PARA_ERROR         (0x00200000)

/* NAND flash plane setting */
#define NAND_PLANE_BLOCK_BIT0 (1 << 0)
#define NAND_PLANE_BLOCK_BIT1 (1 << 1)
#define NAND_PLANE_BLOCK_BIT2 (1 << 2)
#define NAND_PLANE_BLOCK_BIT3 (1 << 3)
#define NAND_PLANE_BLOCK_BIT4 (1 << 4)
#define NAND_PLANE_BLOCK_BIT5 (1 << 5)
#define NAND_PLANE_BLOCK_BIT6 (1 << 6)
#define NAND_PLANE_BLOCK_BIT7 (1 << 7)
#define NAND_PLANE_BLOCK_BIT8 (1 << 8)
#define NAND_PLANE_BLOCK_BIT9 (1 << 9)
#define NAND_PLANE_BLOCK_BIT10 (1 << 10)
#define NAND_PLANE_BLOCK_BIT11 (1 << 11)
#define NAND_PLANE_BLOCK_BIT12 (1 << 12)
#define NAND_PLANE_BLOCK_BIT13 (1 << 13)
#define NAND_PLANE_BLOCK_BIT14 (1 << 14)
#define NAND_PLANE_BLOCK_BIT15 (1 << 15)
#define NAND_PLANE_BLOCK_BIT16 (1 << 16)

#if 0
/*******************************************************************************
 * NFI address definition
 *******************************************************************************/

/*******************************************************************************
 * NFI register definition
 *******************************************************************************/
/* NFI_ACCCON - example
	*NFI_ACCCON = ACCCON | (((NFI_W2R<<6)|(NFI_WH<<4)) | ((NFI_WST<<2)|NFI_RLC));

	*NFI_ACCCON = 0x07C6	// C2R=111, W2R=11, WH=00, WST=00, RLT=10

*/
//#define ACCCON						0x0700	// C2R=111
//#define NFI_W2R					3			// 0:2T, 1:4T, 2:6T, 3:8T ; recommanded value=3
//#define NFI_WH						3			// 0:0T, 1:1T, 2:2T, 3:3T ; recommanded value=1
//#define NFI_WST					3			// 0:0T, 1:1T, 2:2T, 3:3T ; recommanded value=1
//#define NFI_RLC					3			// 0:0T, 1:1T, 2:2T, 3:3T ; recommanded value=2

/* NFI_PAGEFMT; support for PAGEFMT_PageSize_BusWidth */
#if defined(__NFI_VERSION3_1__)
// Set FDM_ECC_NUM to 8 and FDM_NUM to 8 by default.
#define PAGEFMT_512_8BIT   	0x0000
#define PAGEFMT_512_16BIT  	0x0008
#define PAGEFMT_2K_8BIT		0x0001
#define PAGEFMT_2K_16BIT	0x0009
#define PAGEFMT_4K_8BIT		0x0002
#define PAGEFMT_4K_16BIT	0x000A
#elif defined(__NFI_VERSION2__)
#define PAGEFMT_512_8BIT   	0x0010  /*Enable ECC block size = 256*/
#define PAGEFMT_512_16BIT  	0x0110  /*Enable ECC block size = 256*/
#define PAGEFMT_2K_8BIT		0x0015  /*Enable ECC block size = 256*/
#define PAGEFMT_2K_16BIT	0x0119  /*Enable ECC block size = 256*/
#else
#define PAGEFMT_512_8BIT   	0x0020
#define PAGEFMT_512_16BIT  	0x0120
#define PAGEFMT_2K_8BIT			0x0025
#define PAGEFMT_2K_16BIT		0x0125
#endif

#define	IO_ACCESS_8BIT			8
#define	IO_ACCESS_16BIT		16

#if defined(__NFI_VERSION3_1__)

#else

/* NFI_OPCON */
#define BURST_RD           	0x0001
#define BURST_WR           	0x0002
#define ECC_RD_INIT        	0x0010
#define ECC_WR_INIT        	0x0020
#define SINGLE_RD          	0x0100
#define NOB_BYTE           	0x1000
#define NOB_WORD           	0x2000
#define NOB_DWORD          	0x0000
#if defined(__NFI_VERSION2__)
#define OPCON_RESET 0x0C00
#endif
#endif

#endif

/* Nand flash command */
#define RD_1ST_CMD         	0x00
#define RD_2ND_HALF_CMD     0x01   	// only for 512B page-size
#define RD_SPARE_CMD       	0x50	   // only for 512B page-size
#define RD_2ND_CYCLE_CMD		  0x30	   // only for 2KB  page-size
#define INPUT_DATA_CMD     	0x80
#define PROG_DATA_CMD      	0x10
#define BLOCK_ERASE1_CMD   	0x60
#define BLOCK_ERASE2_CMD   	0xD0
#define RD_ID_CMD          	0x90
#define RD_STATUS_CMD      	0x70
#define RESET_CMD          	0xFF
#define CP_READ_CMD        	0x35
#define CP_INPUT_CMD_2K    	0x85
#define CP_INPUT_CMD_512   	0x8A
#define CP_PROG_CMD_TOSHIBA	0x15

#if 0
/* NFI_PSTA */
#define STATUS_CMD         	0x1
#define STATUS_ADDR        	0x2
#define STATUS_DATAR       	0x4
#define STATUS_DATAW       	0x8
#define STATUS_BUSY        	0x100
#endif

#if 0
/* NFI_FIFOCON */
#if defined(__NFI_VERSION3_1__)
// Nothing here.
#elif defined(__NFI_VERSION2__)

#define RD_EMPTY_MASK  0x40
#define RD_FULL_MASK     0x80
#define WR_EMPTY_MASK 	0x4000
#define WR_FULL_MASK    0x8000
#if defined(FLASH_DISKDRV_DEBUG) && !defined(__MTK_TARGET__)
#define FIFO_RD_REMAIN(x) (16) /*simulate the fifo is full in read*/
#define FIFO_WR_REMAIN(x) (0)  /*simulate the fifo is empty in write*/
#else
#define FIFO_RD_REMAIN(x) (0x1F&(x))
#define FIFO_WR_REMAIN(x) ((0x1F00&(x))>>8)
#endif

#else

#define RD_EMPTY_MASK      	0x001
#define RD_FULL_MASK       	0x002
#define WR_EMPTY_MASK      	0x004
#define WR_FULL_MASK      		0x008
#define FIFO_FLUSH         	0x010
#define RESET              	0x020

#endif


/* NFI_CON */
#define READ_CON_EN				0x0400
#define PROGRAM_CON_EN			0X0200
#define ERASE_CON_EN				0X0100
#define SW_PROGSPARE_EN    	0x0020
#define MULTI_PAGE_RD_EN   	0x0010
#define AUTOECC_ENC_EN     	0x0008
#define AUTOECC_DEC_EN     	0x0004
#define DMA_WR_EN          	0x0002
#define DMA_RD_EN          	0x0001

#if defined(__NFI_VERSION2__)
#define MAIN_ECC_EN      0x200
#define SPARE_ECC_EN     0x100
#define AUTO_ECC_ENC    0x8
#define AUTO_ECC_DEC    0x4
#define ECC_FALSE      0x0
#endif

/* NFI_ERRDET */

/* Status register */
#define RDY_BUSY           	0x40
#define INTERNAL_BUSY      	0x20
#define PASS_FAIL          	0x01


#define IS_NFI_DMA_RUN 	 *(volatile uint32_t *)DMA_GLBSTA & (uint32_t)DMA_GLBSTA_RUN(nfi_dmaport)

/* NFI_INTR_EN */

/* NFI_INTR */
#if defined(__NFI_VERSION3_1__)
#define RD_COMPLETE_EN		(0x0001)
#define WR_COMPLETE_EN		(0x0002)
#define RESET_COMPLETE_EN	(0x0004)
#define ERASE_COMPLETE_EN	(0x0008)
#define BUSY_RETURN_EN		(0x0010)
#define ACC_LOCK_EN			(0x0020)
#define AHB_DONE_EN			(0x0040)
#define ALL_EN             	(0x007F)
#elif defined(__NFI_VERSION2__)
#define RD_COMPLETE_EN     0x1000
#define WR_COMPLETE_EN     0x2000
#define RESET_COMPLETE_EN  0x4000
#define ERASE_COMPLETE_EN  0x8000
#define BUSY_RETURN_EN     0x10000000
#define ALL_EN         0x1000f000
#else
#define RD_COMPLETE_EN     	0x01
#define WR_COMPLETE_EN     	0x02
#define RESET_COMPLETE_EN  	0x04
#define ERASE_COMPLETE_EN  	0x08
#define ERR_DET_EN         	0x10
#define ERR_COR_EN         	0x20
#define BUSY_RETURN_EN     	0x40
#define ALL_EN             	0x7F
#endif



/* NFI_INTR */
#if defined(__NFI_VERSION3_1__)
#define RD_COMPLETE			(0x0001)
#define WR_COMPLETE			(0x0002)
#define RESET_COMPLETE		(0x0004)
#define ERASE_COMPLETE		(0x0008)
#define BUSY_RETURN			(0x0010)
#define ACC_LOCK		    (0x0020)
#define AHB_DONE			(0x0040)
#elif defined(__NFI_VERSION2__)
#define RD_COMPLETE        0x1000
#define WR_COMPLETE       0x2000
#define RESET_COMPLETE   0x4000
#define ERASE_COMPLETE  0x8000
#define BUSY_RETURN        0x10000000
#else
#define RD_COMPLETE        0x0001
#define WR_COMPLETE        0x0002
#define RESET_COMPLETE     0x0004
#define ERASE_COMPLETE     0x0008
#define ERR_DET0           0x0010
#define ERR_DET1           0x0020
#define ERR_DET2           0x0040
#define ERR_DET3           0x0080
#define ERR_COR0           0x0100
#define ERR_COR1           0x0200
#define ERR_COR2           0x0400
#define ERR_COR3           0x0800
#define BUSY_RETURN        0x1000
#endif /*__NFI_VERSION2__*/

#endif

#define RDY_BUSY           	0x40
#define INTERNAL_BUSY      	0x20
#define PASS_FAIL          	0x01

/*******************************************************************************
 * NAND MTD declaration
 *******************************************************************************/
// NAND flash maker ID
#define TOSHIBA  0x98
#define SAMSUNG  0xEC
#define HYNIX  0xAD
#define ST     0x20
#define MICRON  0x2C
#define	INTEL   0x89
#define SPANSION  0x01
#define POWERFLASH  0x92
#define ESMT        0xC8
#define GD  0xC8
typedef struct NAND_ID_tag {
    uint32_t mID: 8;           /*[Byte1]*/
    uint32_t dID: 8;           /*[Byte2]*/
    uint32_t dieNum: 2;        /*[Byte3] number of Die (per CE) = (1<<value)*/
    uint32_t cellType: 2;      /*[Byte3] Cell Type = (2<<value) Level cell, value >= 1 is MLC*/
    uint32_t simultNum: 2;     /*[Byte3] Number of Simutaneously Programmed Page = (1<<value), for each die*/
    uint32_t dieInterleave: 1; /*[Byte3] die Interleave support Flag: 1 support; 0 Not Support*/
    uint32_t cacheProgram: 1;  /*[Byte3] cache Program support Flag: 1 support; 0 Not Support*/
    uint32_t pageSize: 2;      /*[Byte4] page Size = (1024<<value)*/
    uint32_t spareSize: 1;     /*[Byte4] spare Size per 512 = (8<<value) */
    uint32_t accessTime0: 1;   /*[Byte4]*/
    uint32_t blockSize: 2;     /*[Byte4] block Size = (64k<<value)*/
    uint32_t IOWidth: 1;       /*[Byte4] IOWidth = (8<<value)*/
    uint32_t accessTime1: 1;   /*[Byte4]*/
    uint32_t byte5Rev0: 1;     /*[Byte5]*/
    uint32_t ToshibaExt: 1;    /*[Byte5] [Toshiba] Extended block flag*/
    uint32_t planeNum: 2;      /*[Byte5] number of plane (per CE) = (1<<value) (Toshiba does not suuport)*/
    uint32_t planeSize: 3;     /*[Byte5] plane Size= (64Mb<<value) (Toshiba does not suuport)*/
    uint32_t byte5Rev1: 1;     /*[Byte5]*/
    uint32_t Reserve: 24;      /*[Byte6~8]*/
} nAND_ID;


typedef struct {
    uint8_t  cmd[2];
    uint8_t  cmd_len;
    uint32_t add[2]; /*Max 8 x uint8 address cycle*/
    uint8_t  add_cycle;
    uint8_t  *buff;
    uint32_t buff_len;
    uint8_t  *spare;
    uint32_t spare_len;
    uint32_t intFlag;
    uint8_t  ECCFlag;
#if defined(__NFI_VERSION3_1__)
    uint8_t  CustomMode;
#endif // defined(__NFI_VERSION3_1__)
} nAND_Cmd_info;

///TODO:: redefine
typedef struct mtd_lock_tag {
    uint32_t  lock_count;
    //kal_taskid  owner_id;
    //kal_enhmutexid mtd_enhmutex;
} NAND_MTD_LOCK;



/*******************************************************************************
 * structure definition for flash supply list and MTD
 *******************************************************************************/

typedef struct {
    uint32_t *page_p;
    uint8_t  *buff;
    uint8_t  *spare;
    uint16_t pageOffset; /*Pnysical PageIndex = (*page_p)+pageOffset*/
    uint8_t  addCycle;
    uint8_t  opFlag;
} mtd_drv_para;

/*For mtd_drv_para.opFlag*/
#define FIRST_PROCESS            0x01
#define SECOND_BUSY              0x02
#define FULL_OPERATION           (FIRST_PROCESS|SECOND_BUSY)
#define ONE_SPARE			            	0x04
#define PROGRAM_CACHED           0x10
#define PROGRAM_FINAL            0x20
#define SAMSUNG_D0_ILEAVE_OP     0x40 /*samsung die 0 interleave operation*/
#define SAMSUNG_D1_ILEAVE_OP     0x80 /*samsung die 1 interleave operation*/
typedef struct multiPage_operation_tag {
    uint32_t (*func_p) (mtd_drv_para *Para);          /**/
    uint8_t startAlign;       /*start page alignment limitation*/
    uint8_t multiPageNum;    /*the max page numner func_p can access. 0xFF means continue to end of block*/
} mPage_operation;

#define MAX_MULTIPAGE_FUNCTION_NUM 6
#define MAX_MULTIPAGE_NUM 8 /*max MULTI-PAGE num (CE0+CE1)*//*It should <= 16 due to MTD return value */

typedef struct {
    uint8_t  *buff;
    uint8_t  *spare;
    uint32_t pageIdx[MAX_MULTIPAGE_NUM];
    uint16_t length;
    uint8_t  ce;
    uint8_t  m_idx;  /*index of Flash_Info.multiProgram*/
    uint8_t  opFlag;
} mtd_if_para;


typedef struct {
    uint32_t     (* ProgramPage)                   (mtd_drv_para *Para);
    uint32_t     (* ReadPage)                      (mtd_drv_para *Para);
    uint32_t     (* CopyPage)                      (mtd_drv_para *Para);
    uint32_t     (* EraseBlock)                    (mtd_drv_para *Para);
    /*Multi Page Program functions*/
    uint32_t     (* TwoPlaneProgram)               (mtd_drv_para *Para);/*multiple 2*/
    uint32_t     (* TwoDieProgram)                 (mtd_drv_para *Para);/*multiple 2*/
    uint32_t     (* FourPlaneProgram)              (mtd_drv_para *Para);/*multiple 4*/
    uint32_t     (* TwoDieTwoPlaneProgram)         (mtd_drv_para *Para);/*multiple 4*/
    uint32_t     (* CacheProgram)                  (mtd_drv_para *Para);
    uint32_t     (* TwoPlaneCacheProgram)          (mtd_drv_para *Para);
    uint32_t     (* TwoDieCacheProgram)            (mtd_drv_para *Para);
    uint32_t     (* FourPlaneCacheProgram)         (mtd_drv_para *Para);
    uint32_t     (* TwoDieTwoPlaneCacheProgram)    (mtd_drv_para *Para);
    /*Multi Block Erase functions*/
    uint32_t     (* TwoPlaneErase)               (mtd_drv_para *Para);/*multiple 2*/
    uint32_t     (* TwoDieErase)                 (mtd_drv_para *Para);/*multiple 2*/
    uint32_t     (* FourPlaneErase)              (mtd_drv_para *Para);/*multiple 4*/
    uint32_t     (* TwoDieTwoPlaneErase)         (mtd_drv_para *Para);/*multiple 4*/
    /* OTP related function */
    int32_t  	   (* OTPAccess)                   (int32_t accesstype, uint32_t PageAddr, void *BufferPtr, void *SparePtr);
    int32_t      (* OTPQueryLength)              (uint32_t *Length);
} NAND_MTD_SUB_DRIVER;

typedef struct {
    NAND_NAME_T           DevName_enum;      /*Device name*/
    uint8_t               ID_info[8];        /*equal sizeof(nAND_ID)*/
    uint8_t               ID_valid_mask[8];  /*valid-bit mask of ID_info*/
    uint8_t               IOWidth;
    uint8_t               address_cycle; /*row add cycle + column add cycle*/
    uint8_t               planeNum;      /*number of plane per die, min 1*/
    uint8_t               dieNum;        /*number of die per device(CE), min 1*/
    uint16_t              pageSize;
    uint16_t              spareSize;
    uint16_t              blockPage;
    uint16_t              deviceSize;    /*unit by MByte (so max < 64GB), exclude the extended block size*/
    uint16_t              supportMPType; /*Supported Multi-page program type*//*supportOpType*/
    uint16_t              supportMEType; /*Supported Multi-Block Erase type*//*supportOpType*/
    uint16_t              CopyBackType;  /*Type of Copyback*/
    NAND_MTD_SUB_DRIVER  *driverSet;     /*driverSet + supportOpType = supported driver*/
} flash_list /*device_info*/;

/*device feature/operation support (for flash_list.supportOpType)*/
#define SIMPLE_OP               (0x0000)
/*Do not use (0x0001), it use by AUTO_NAND_DETECT (0x0011)*/
#define TWO_PLANE_OP            (0x0002)
#define FOUR_PLANE_OP           (0x0004)
#define TWO_DIE_INTERLEAVE_OP   (0x0008)
/*Do not use (0x0010), it use by AUTO_NAND_DETECT (0x0011)*/
#define CACHE_PROGRAM           (0x0020)
#define EXTENDED_BLOCK          (0x0040)
#define PLANE_INTERLACED        (0x0080) /*block number is interlaced between plane*/
#define PLANE_CONTINUOUS        (0x0100) /*block number is continuous in plane*/

#define AUTO_NAND_DETECT        (0x0011) /*auto-detection definition of all excepte (supportMPType and supportMEType)*/


#if 0
/* indicate if write ECC fields while program page */
enum {
    ECC_SKIP = 0,            // don't program spare area
    ECC_FIRST_HALF,      // only program first and second ECC fields
    ECC_SECOND_HALF,     // only program third and fourth ECC fields
    ECC_WHOLE            // program whole ECC fields
};
#endif

/*******************************************************************************
 *  MTD function declaration
 *******************************************************************************/
//int32_t MTD_MountDevice	(void * DriveData);
int32_t MTD_MountDevice( void);
uint32_t MTD_CE_Interleave_Program (mtd_if_para *Para);
uint32_t MTD_CE_Interleave_CacheProgram (mtd_if_para *Para);
uint32_t MTD_Program (mtd_if_para *Para);
uint32_t MTD_CacheProgram (mtd_if_para *Para);
uint32_t MTD_Read(mtd_if_para *Para);
uint32_t MTD_Erase(mtd_if_para *Para);
uint32_t MTD_MarkBlockBad (mtd_if_para *Para);
uint32_t MTD_CheckBlockBad (mtd_if_para *Para);
uint32_t MTD_NAND_Reset(mtd_if_para *Para);
int32_t MTD_ShutDown	(void);

#if defined(FLASH_DISKDRV_DEBUG) && !defined(__MTK_TARGET__)
//void MTD_Simu_Create_Device_File (void);
//void MTD_Simu_SetUserDefID(uint32_t cs0_ID_0_3,uint32_t cs0_ID_4_7,uint32_t cs1_ID_0_3,uint32_t cs1_ID_4_7);
//void MTD_Simu_SetIDData(uint32_t* id_data,uint8_t cs);
#if defined DAL_DLL_VERSION
void MTD_WriteErrMsg(char *title, char *exp, char *f, uint32_t line, uint32_t e1, uint32_t e2, uint32_t e3);
#endif

#endif
//int32_t MTD_EraseBlock	(void * DriveData, uint32_t BlockIndex);
/*******************************************************************************
 *  Sub-function declaration
 *******************************************************************************/
uint32_t single_Page_Program (mtd_drv_para *para);
uint32_t single_Page_CacheProgram (mtd_drv_para *para);
uint32_t single_Page_Read (mtd_drv_para *para);
uint32_t single_Block_Erase (mtd_drv_para *para);

#if 0
uint32_t toshiba_twoPlane_Program (mtd_drv_para *para);
uint32_t toshiba_twoPlane_Erase (mtd_drv_para *para);
uint32_t toshiba_fourPlane_Program (mtd_drv_para *para);
uint32_t toshiba_fourPlane_Erase (mtd_drv_para *para);
uint32_t toshiba_twoPlane_CacheProgram (mtd_drv_para *para);
uint32_t toshiba_fourPlane_CacheProgram (mtd_drv_para *para);
uint32_t samsung_twoPlane_Program (mtd_drv_para *para);
uint32_t samsung_twoPlane_Erase (mtd_drv_para *para);
uint32_t samsung_twoDie_Program (mtd_drv_para *para);
uint32_t samsung_twoDie_Erase (mtd_drv_para *para);
uint32_t samsung_twoDie_twoPlane_Program (mtd_drv_para *para);
uint32_t samsung_twoDie_twoPlane_Erase (mtd_drv_para *para);
int32_t  samsung_OTPAccess(int32_t accesstype, uint32_t PageAddr, void *BufferPtr, void *SparePtr);
int32_t  samsung_OTPQueryLength(uint32_t *Length);
int32_t  hynix_OTPAccess(int32_t accesstype, uint32_t PageAddr, void *BufferPtr, void *SparePtr);
int32_t  hynix_OTPQueryLength(uint32_t *Length);
int32_t  ESMT_OTPAccess(int32_t accesstype, uint32_t PageAddr, void *BufferPtr, void *SparePtr);
int32_t  ESMT_OTPQueryLength(uint32_t *Length);
int32_t  micron_OTPAccess(int32_t accesstype, uint32_t PageAddr, void *BufferPtr, void *SparePtr);
int32_t  micron_OTPQueryLength(uint32_t *Length);
int32_t  NAND_OTPAccess(int32_t accesstype, uint32_t PageAddr, void *BufferPtr, void *SparePtr);
int32_t  NAND_OTPQueryLength(uint32_t *Length);
uint32_t micron_twoPlane_Program (mtd_drv_para *para);
uint32_t micron_twoPlane_Erase (mtd_drv_para *para);
uint32_t micron_twoPlane_CacheProgram (mtd_drv_para *para);
uint32_t micron_twoDie_Program (mtd_drv_para *para);
uint32_t micron_twoDie_Erase (mtd_drv_para *para);
uint32_t micron_twoDie_twoPlane_Program (mtd_drv_para *para);
uint32_t micron_twoDie_twoPlane_Erase (mtd_drv_para *para);
uint32_t micron_twoDie_CacheProgram (mtd_drv_para *para);
uint32_t micron_twoDie_twoPlane_CacheProgram (mtd_drv_para *para);
#endif

uint32_t FlashCheckID(const flash_list *ID_table);

uint8_t read_status(uint8_t cmd1, nAND_Cmd_info *info );
int32_t InitializeMTD(void *DriveData);

#if 0
void nfi_lisr(void);
void nfi_hisr(void);
void NFI_DMA_Callback(void);
void pdn_nfi(uint8_t ON);
void NAND_GPIO_InitIO(char direction, char port);
char NAND_GPIO_ReturnMode(char port);
char NAND_GPIO_ReadIO(char port);
void NAND_GPIO_WriteIO(char data, char port);
uint8_t NAND_INT_USBBoot(void);
#endif

#if 0  //superAnd
/******************************************************************************
 * SuperAND flash device definition                                           *
 ******************************************************************************/
#define SUPERAND_PAGE_READ_CMD  0x00    /* Can be recognized by NFI */
#define SUPERAND_SEQ_READ_CMD   0x0F    /* Can't be recognized by NFI */
#define SUPERAND_READ_END_CMD   0xF0    /* Can't be recognized by NFI */
#define SUPERAND_REWRITE_CMD    0x1F    /* Can't be recognized by NFI */
#define SUPERAND_PROG_CMD       0x10    /* Can be recognized by NFI */

#define SUPERAND_ADDR_CYCLE     0x04
#define SUPERAND_BYTES_PER_SECTOR   (SUPERAND_PAGES_PER_SECTOR << 9)
#define SUPERAND_PAGES_PER_SECTOR   0x04
#define SUPERAND_SECTOR_MASK    (SUPERAND_PAGES_PER_SECTOR - 1)
#define SUPERAND_PAGES_PER_EXT_SECTOR   0x08
#define SUPERAND_EXT_SSECTOR_MASK    (SUPERAND_PAGES_PER_EXT_SECTOR - 1)


#define SUPERAND_PAGEFMT_8BIT   0x0005
#define SUPERAND_PAGEFMT_16BIT  0x0105

#define SUPERAND_ACCCON         0x07FF

#define PAGES2BYTES(pages)      ((pages) << 9)
#define PAGES2DWORDS(pages)     ((pages) << 7)

#define MAX_DEVICE_NUM          2

typedef struct {
    uint16_t     id;
    uint16_t     plane_size;      // in MBytes
    uint16_t     block_size;      // in KBytes
    uint8_t      io_width;        // 8 or 16 bits
}  superand_flash_list;
#define SUPERAND_FLASH_LIST_END {0xFFFF, 0, 0, 0}

typedef struct {
    int32_t     (* MountDevice)    (void *DriveData);
    int32_t     (* ShutDown)       (void *DriveData);
    int32_t     (* ReadPages)      (void *DriveData, uint32_t PageIndex, uint32_t Pages, uint8_t *Data);
    int32_t     (* RewritePages)   (void *DriveData, uint32_t PageIndex, uint32_t Pages, uint8_t *Data);
    int32_t     (* ProgramData)    (void *DriveData, uint32_t PageIndex, uint32_t Bytes, uint8_t *Data);
    int32_t     (* EraseBlock)     (void *DriveData, uint32_t PageIndex);
    int32_t     (* ReadGoodBlkCnt) (void *DriveData);
} SuperAND_MTD_DRIVER;

#if (defined(MT6218B) || defined(MT6219))
#define COMPUTE_ADDRESS_AND_SET_CSEL \
{\
    uint32_t page_idx = PageIndex;\
    uint16_t *paddr = (uint16_t*)&addr;\
    paddr[0] = (D->io_width == 8) ? (uint16_t)((page_idx&0x3) << 9) : (uint16_t)((page_idx&0x3) << 8);\
    paddr[1] = page_idx >> 2;\
}
#else
#define COMPUTE_ADDRESS_AND_SET_CSEL \
{\
    uint32_t page_idx = PageIndex;\
    uint16_t *paddr = (uint16_t*)&addr;\
    if(page_idx >= D->pages_of_first_device)\
    {\
        *NFI_CSEL = 0x1;\
        page_idx -= D->pages_of_first_device;\
    }else{\
        *NFI_CSEL = 0x0;\
    }\
    paddr[0] = (D->io_width == 8) ? (uint16_t)((page_idx&0x3) << 9) : (uint16_t)((page_idx&0x3) << 8);\
    paddr[1] = page_idx >> 2;\
}
#endif

void SuperAND_MTD_Init(void);
int32_t SuperAND_MTD_MountDevice(void *DriveData);
int32_t SuperAND_MTD_ShutDown(void *DriveData);
int32_t SuperAND_MTD_ReadPages(void *DriveData, uint32_t PageIndex, uint32_t Pages, uint8_t *Data);
int32_t SuperAND_MTD_RewritePages(void *DriveData, uint32_t PageIndex, uint32_t Pages, uint8_t *Data);
int32_t SuperAND_MTD_ProgramData(void *DriveData, uint32_t PageIndex, uint32_t Bytes, uint8_t *Data);
int32_t SuperAND_MTD_EraseBlock(void *DriveData, uint32_t PageIndex);
int32_t SuperAND_MTD_ReadGoodBlkCnt(void *DriveData);

uint16_t superand_read_device_id(uint16_t csel);
uint8_t superand_checkdevice(void *DriveData);
int32_t superand_writefifo(void *DriveData, uint32_t PageIndex, uint8_t *Data, uint32_t Bytes, uint8_t rewrite);
void superand_readfifo(uint8_t *Data, uint32_t Pages);
uint8_t superand_read_status(void);
#endif


//extern NAND_MTD_SUB_DRIVER SAMSUNG_SINGLE_CACHE_SUB_DRIVER;
//extern NAND_MTD_SUB_DRIVER SAMSUNG_SUB_DRIVER;
//extern NAND_MTD_SUB_DRIVER HYNIX_ST_SUB_DRIVER;
//extern NAND_MTD_SUB_DRIVER TOSHIBA_SUB_DRIVER;
//extern NAND_MTD_SUB_DRIVER MICRON_SUB_DRIVER;
extern NAND_MTD_SUB_DRIVER GD_SUB_DRIVER;

#endif //NAND_MTD_FDM50_INTERNAL_H


