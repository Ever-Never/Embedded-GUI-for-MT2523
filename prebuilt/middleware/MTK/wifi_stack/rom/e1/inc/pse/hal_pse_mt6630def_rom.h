/*
** $Id: @(#) hal_pse_mt6630def_rom.h $
*/

/*! \file   hal_pse_mt6630def_rom.h
    \brief  This file defines all register address of PSE.
            Data type for PSE register access are all defined in this file.

    PSE data sheet level definitions are all in this file.

*/
/*******************************************************************************
* Copyright (c) 2012 MediaTek Inc.
*
* All rights reserved. Copying, compilation, modification, distribution
* or any other use whatsoever of this material is strictly prohibited
* except in accordance with a Software License Agreement with
* MediaTek Inc.
********************************************************************************
*/

/*******************************************************************************
* LEGAL DISCLAIMER
*
* BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND
* AGREES THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK
* SOFTWARE") RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE
* PROVIDED TO BUYER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY
* DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT
* LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE
* ANY WARRANTY WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY
* WHICH MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK
* SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY
* WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE
* FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION OR TO
* CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
* BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
* LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL
* BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT
* ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
* BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
* WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT
* OF LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING
* THEREOF AND RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN
* FRANCISCO, CA, UNDER THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE
* (ICC).
********************************************************************************
*/

/*
** $Log: hal_pse_mt6630def_rom.h $
**
** 01 12 2015 tiger.hung
** [BORA00003732] [MT7637][E1][MP][WIFI]
** - Update Reg definitions 
** - Add rom API
**
** 12 10 2014 tiger.hung
** [BORA00003582] [MT7637] Rom code modification
** update ILM/DLM boundary for check
**
** 06 20 2014 shuojen.hsu
** [BORA00003152] [MT7637][WiFi] Merge emulation code from MT7636_E2_MP
** .
**
** 12 03 2013 ted.huang
** [BORA00002807] [MT7636]
** update WIFI ROM low power API.
**
** 10 28 2013 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Change PSE Direct Memory Address From 0xa5000000 to 0xa8000000
**
** 10 17 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] [MT7636] [PSE] support disable/enable pse_hif_ffa_delta_token and pse_hif_ffa_token_vld signaling
**
** 04 10 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] [PSE] Update PSE code to support share buffer new design change.
**
** 02 19 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [mt6630] follow indirect reference method to add PSE related ROM API
** for RAM code reference.
**
** 01 21 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] base on 20130102 pse datasheet to support page size 128 bytes
** and all emulation cases in wifi ram code test
**
** 12 20 2012 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] [PSE] Base on MT6630 PSE FPGA release on 2012.12.17 to sync Lib, HAL, and emulation items in firmware
**
** 11 30 2012 wayne.guo
** [BORA00002164] MT6630 unified MAC
** 11 30 2012 wayne.guo
** [MT6630] Base on 2012.11.28 released PSE FPGA features to update PSE firmware code
**
** 10 24 2012 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] Base on MT6630 PSE datasheet release on date 2012.10.23 to add debug port feature support.
**
** 10 22 2012 wayne.guo
** [BORA00002164] MT6630 unified MAC
** 10 22 2012 wayne.guo
** [MT6630] Add PSE ROM code support
**
*/

#ifndef _HAL_PSE_MT6630_DEF_ROM_H
#define _HAL_PSE_MT6630_DEF_ROM_H


/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

//#define     PSE_BASE_ADDR                   0xa0000000      /* provided from designer */
#define     PSE_PAGE_POOL_OFFSET            0x00000800      /* provided from designer */

#if 0 //(PRODUCT_VERSION == 7636)
#define     PSE_MT6630_DIRECT_ACCESS_ADDR   0xa8000000      /* PSE Direct Access Starting Address */
#define     PSE_MT6630_DSCOPE_ADDR          0xa8008000      /* PSE Dscope direct access address */
#define     PSE_MEMORY_SIZE                 0x00028000      /* PSE Memory size. 7636: 160K */
// #elif (PRODUCT_VERSION == 7637)
#define     PSE_MT6630_DIRECT_ACCESS_ADDR   0xa8000000      /* PSE Direct Access Starting Address */
#define     PSE_MT6630_DSCOPE_ADDR          0xa8008000      /* PSE Dscope direct access address */
#define     PSE_MEMORY_SIZE                 0x00039000      /* PSE Memory size. 7637: 228K */
// #else
#define     PSE_MT6630_DIRECT_ACCESS_ADDR   0xa5000000      /* PSE Direct Access Starting Address */
#define     PSE_MT6630_DSCOPE_ADDR          0xa5008000      /* PSE Dscope direct access address */
#define     PSE_MEMORY_SIZE                 0x00028000      /* PSE Memory size. */
#endif

#define     PSE_GLOBAL_CTRL                 (PSE_BASE_ADDR + 0x00000000)
#define     PSE_INT_EN_MASK                 (PSE_BASE_ADDR + 0x00000004)
#define     PSE_INT_STS                     (PSE_BASE_ADDR + 0x00000008)
#define     PSE_INT_ERR_REASON              (PSE_BASE_ADDR + 0x0000000c)
#define     PSE_INT_ERR_STS_MASK            (PSE_BASE_ADDR + 0x00000010)


#define     PSE_BUF_CTRL                    (PSE_BASE_ADDR + 0x00000014)

#define     PSE_VER_CTRL                    (PSE_BASE_ADDR + 0x00000018)

#define     CIF_MISC_CTRL                   (PSE_BASE_ADDR + 0x0000001c)
#define     CIF_VM_ADDR_CTRL                (PSE_BASE_ADDR + 0x00000020)

#define     CIF_GET_FIRST_FID               (PSE_BASE_ADDR + 0x00000024)
#define     CIF_GET_FID                     (PSE_BASE_ADDR + 0x00000028)

#define     CIF_DEQ_SIG                     (PSE_BASE_ADDR + 0x0000002c)
#define     CIF_ENQ_SIG                     (PSE_BASE_ADDR + 0x00000030)
#define     CIF_ENQ_LIST                    (PSE_BASE_ADDR + 0x00000034)
#define     CIF_DENQ_LIST_0                 (PSE_BASE_ADDR + 0x00000038)
#define     CIF_DENQ_LIST_1                 (PSE_BASE_ADDR + 0x0000003c)

#define     CIF_OODEQ_SIG                   (PSE_BASE_ADDR + 0x00000040)
#define     CIF_OOENQ_SIG                   (PSE_BASE_ADDR + 0x00000044)
#define     CIF_OODEQ_LIST                  (PSE_BASE_ADDR + 0x00000048)
#define     CIF_OOENQ_LIST_0                (PSE_BASE_ADDR + 0x0000004c)
#define     CIF_OOENQ_LIST_1                (PSE_BASE_ADDR + 0x00000050)

#define     CIF_OODENQ_LIST_0               (PSE_BASE_ADDR + 0x00000054)
#define     CIF_OODENQ_LIST_1               (PSE_BASE_ADDR + 0x00000058)


#define     CIF_DEQ_ALL                     (PSE_BASE_ADDR + 0x0000005c)

#define     CIF_QRW_P0_Q0                   (PSE_BASE_ADDR + 0x00000060)
#define     CIF_QRW_P0_Q1                   (PSE_BASE_ADDR + 0x00000064)
#define     CIF_QRW_P0_Q2                   (PSE_BASE_ADDR + 0x00000068)

#define     CIF_QRW_P1_Q0                   (PSE_BASE_ADDR + 0x0000006c)
#define     CIF_QRW_P1_Q1                   (PSE_BASE_ADDR + 0x00000070)
#define     CIF_QRW_P1_Q2                   (PSE_BASE_ADDR + 0x00000074)
#define     CIF_QRW_P1_Q3                   (PSE_BASE_ADDR + 0x00000078)

#define     CIF_QRW_P2_Q0                   (PSE_BASE_ADDR + 0x0000007c)
#define     CIF_QRW_P2_Q1                   (PSE_BASE_ADDR + 0x00000080)
#define     CIF_QRW_P2_Q2                   (PSE_BASE_ADDR + 0x00000084)
#define     CIF_QRW_P2_Q3                   (PSE_BASE_ADDR + 0x00000088)
#define     CIF_QRW_P2_Q4                   (PSE_BASE_ADDR + 0x0000008c)
#define     CIF_QRW_P2_Q5                   (PSE_BASE_ADDR + 0x00000090)
#define     CIF_QRW_P2_Q6                   (PSE_BASE_ADDR + 0x00000094)
#define     CIF_QRW_P2_Q7                   (PSE_BASE_ADDR + 0x00000098)
#define     CIF_QRW_P2_Q8                   (PSE_BASE_ADDR + 0x0000009c)
#define     CIF_QRW_P2_Q9                   (PSE_BASE_ADDR + 0x000000a0)
#define     CIF_QRW_P2_Q10                  (PSE_BASE_ADDR + 0x000000a4)
#define     CIF_QRW_P2_Q11                  (PSE_BASE_ADDR + 0x000000a8)
#define     CIF_QRW_P2_Q12                  (PSE_BASE_ADDR + 0x000000ac)
#define     CIF_QRW_P2_Q13                  (PSE_BASE_ADDR + 0x000000b0)


#define     CIF_QRW_P3_Q0                   (PSE_BASE_ADDR + 0x000000b4)
#define     CIF_QRW_P3_Q1                   (PSE_BASE_ADDR + 0x000000b8)
#define     CIF_QRW_P3_Q2                   (PSE_BASE_ADDR + 0x000000bc)
#define     CIF_QRW_P3_Q3                   (PSE_BASE_ADDR + 0x000000c0)
#define     CIF_QRW_P3_Q4                   (PSE_BASE_ADDR + 0x000000c4)
#define     CIF_QRW_P3_Q5                   (PSE_BASE_ADDR + 0x000000c8)
#define     CIF_QRW_P3_Q6                   (PSE_BASE_ADDR + 0x000000cc)
#define     CIF_QRW_P3_Q7                   (PSE_BASE_ADDR + 0x000000d0)
#define     CIF_QRW_P3_Q8                   (PSE_BASE_ADDR + 0x000000d4)


#define     QLEN_P0_Q0Q1                    (PSE_BASE_ADDR + 0x000000d8)
#define     QLEN_P0_Q2                      (PSE_BASE_ADDR + 0x000000dc)

#define     QLEN_P1_Q0Q1                    (PSE_BASE_ADDR + 0x000000e0)
#define     QLEN_P1_Q2Q3                    (PSE_BASE_ADDR + 0x000000e4)


#define     QLEN_P2_Q0Q1                    (PSE_BASE_ADDR + 0x000000e8)
#define     QLEN_P2_Q2Q3                    (PSE_BASE_ADDR + 0x000000ec)
#define     QLEN_P2_Q4Q5                    (PSE_BASE_ADDR + 0x000000f0)
#define     QLEN_P2_Q6Q7                    (PSE_BASE_ADDR + 0x000000f4)
#define     QLEN_P2_Q8Q9                    (PSE_BASE_ADDR + 0x000000f8)
#define     QLEN_P2_Q10Q11                  (PSE_BASE_ADDR + 0x000000fc)
#define     QLEN_P2_Q12Q13                  (PSE_BASE_ADDR + 0x00000100)


#define     QLEN_P3_Q0Q1                    (PSE_BASE_ADDR + 0x00000104)
#define     QLEN_P3_Q2Q3                    (PSE_BASE_ADDR + 0x00000108)
#define     QLEN_P3_Q4Q5                    (PSE_BASE_ADDR + 0x0000010C)
#define     QLEN_P3_Q6Q7                    (PSE_BASE_ADDR + 0x00000110)
#define     QLEN_P3_Q8                      (PSE_BASE_ADDR + 0x00000114)

#define     GET_QLEN_CTRL                   (PSE_BASE_ADDR + 0x00000118)

#define     FC_CTRL_P0                      (PSE_BASE_ADDR + 0x00000120)
#define     FC_CTRL_P1                      (PSE_BASE_ADDR + 0x00000124)

#define     FC_CTRL_P2_RQ0                  (PSE_BASE_ADDR + 0x00000128)
#define     FC_CTRL_P2_RQ1                  (PSE_BASE_ADDR + 0x0000012c)
#define     FC_CTRL_P2_RQ2                  (PSE_BASE_ADDR + 0x00000130)


#define     FC_FFA_FREE_CNT                 (PSE_BASE_ADDR + 0x00000134)
#define     FC_FFA_RSRV_PRI                 (PSE_BASE_ADDR + 0x00000138)


#define     FC_RCNT_P0_P1                   (PSE_BASE_ADDR + 0x0000013c)


#define     FC_RCNT_P2_RQ0RQ1               (PSE_BASE_ADDR + 0x00000140)
#define     FC_RCNT_P2_RQ2                  (PSE_BASE_ADDR + 0x00000144)

#define     FC_SCNT_P0_P1                   (PSE_BASE_ADDR + 0x00000148)

#define     FC_SCNT_P2_RQ0RQ1               (PSE_BASE_ADDR + 0x0000014c)
#define     FC_SCNT_P2_RQ2                  (PSE_BASE_ADDR + 0x00000150)

#define     FC_DCNT_P0_P1                   (PSE_BASE_ADDR + 0x00000154)
#define     FC_DCNT_P2_P3                   (PSE_BASE_ADDR + 0x00000158)
#define     FC_DCNT_P2_Q0Q1                 (PSE_BASE_ADDR + 0x0000015c)
#define     FC_DCNT_P2_Q2Q3                 (PSE_BASE_ADDR + 0x00000160)
#define     FC_DCNT_P2_Q4Q5                 (PSE_BASE_ADDR + 0x00000164)
#define     FC_DCNT_P2_Q6Q7                 (PSE_BASE_ADDR + 0x00000168)
#define     FC_DCNT_P2_Q8Q9                 (PSE_BASE_ADDR + 0x0000016c)
#define     FC_DCNT_P2_Q10Q11               (PSE_BASE_ADDR + 0x00000170)
#define     FC_DCNT_P2_Q12Q13               (PSE_BASE_ADDR + 0x00000174)


#define 	BUF_SHARE_CTRL					(PSE_BASE_ADDR + 0x00000160)
#define		BUF_SHARE_DIRECT_ACCESS_ADDR	(PSE_BASE_ADDR + 0x00000164)
#define		BUF_SHARE_LP_ADDR				(PSE_BASE_ADDR + 0x00000168)
#define		BUF_SHARE_SW_RDD_ADDR			(PSE_BASE_ADDR + 0x0000016c)
#define		BUF_SHARE_HW_RDD_ADDR			(PSE_BASE_ADDR + 0x00000170)
#define		BUF_SHARE_BACKUP_TBL_ADDR		(PSE_BASE_ADDR + 0x00000174)
#define		BUF_SHARE_BACKUP_DATA_ADDR		(PSE_BASE_ADDR + 0x00000178)
#define		BUF_SHARE_SW_ONLY_ADDR			(PSE_BASE_ADDR + 0x0000017c)
#define		BUF_SHARE_DSCOPE_ADDR			(PSE_BASE_ADDR + 0x00000180)

#define		BUF_SHARE_FREEPG_START_ADDR		(PSE_BASE_ADDR + 0x00000184)



#if 0   /* below flow control registers were deleted by HG */
#define     FC_DELTA_TOKEN_LIMIT_0_1        (PSE_BASE_ADDR + 0x00000170)
#define     FC_DELTA_TOKEN_LIMIT_2_3        (PSE_BASE_ADDR + 0x00000174)
#define     FC_DELTA_TOKEN_LIMIT_4_5        (PSE_BASE_ADDR + 0x00000178)
#define     FC_DELTA_TOKEN_LIMIT_6_7        (PSE_BASE_ADDR + 0x0000017c)
#define     FC_DELTA_TOKEN_LIMIT_8_9        (PSE_BASE_ADDR + 0x00000180)
#define     FC_DELTA_TOKEN_LIMIT_10_11      (PSE_BASE_ADDR + 0x00000184)
#define     FC_DELTA_TOKEN_LIMIT_12_13      (PSE_BASE_ADDR + 0x00000188)
#define     FC_DELTA_TOKEN_LIMIT_CTX_FFA    (PSE_BASE_ADDR + 0x0000018c)
#endif  /* disabled for flow control */


#define     PSE_PAUSE_RD_CTRL               (PSE_BASE_ADDR + 0x00000190)
#define     PSE_RD_TAB_ACCESS               (PSE_BASE_ADDR + 0x00000194)

#define     PSE_RL_INFO_ACCESS              (PSE_BASE_ADDR + 0x000001a0)
#define     PSE_RL_INFO_DATA                (PSE_BASE_ADDR + 0x000001a4)

//#define     PSE_FL_CTRL_MEM_0               (PSE_BASE_ADDR + 0x000001b0)
//#define     PSE_PL_CTRL_MEM_0               (PSE_BASE_ADDR + 0x000001c0)

#define     PSE_FL_TAB_ACCESS               (PSE_BASE_ADDR + 0x000001b0)
#define     PSE_FL_TAB_DATA                 (PSE_BASE_ADDR + 0x000001b4)
#define     PSE_PL_TAB_ACCESS               (PSE_BASE_ADDR + 0x000001b8)
#define     PSE_HT_QPTR_ACCESS              (PSE_BASE_ADDR + 0x000001bc)


#define 	LP_PSE_CTRL						(PSE_BASE_ADDR + 0x000001c0)
#define     LP_PSE_FL_BKRT					(PSE_BASE_ADDR + 0x000001c4)

#define     CIF_BURST_ACCESS				(PSE_BASE_ADDR + 0x000001d0)
#define     CIF_BURST_DATA_1				(PSE_BASE_ADDR + 0x000001d4)


/* Below registers are created for PSE Debug Port Support; they will be used for PSE emulation test cases. */

#define     TPH_CTRL_QEPTY                  (PSE_BASE_ADDR + 0x00000200)
#define     TPH_Q_OPER_0                    (PSE_BASE_ADDR + 0x00000204)
#define     TPH_Q_OPER_1                    (PSE_BASE_ADDR + 0x00000208)
#define     TPH_WR_BUF_ALLOC                (PSE_BASE_ADDR + 0x00000210)
#define     TPH_DATA_OPER                   (PSE_BASE_ADDR + 0x00000214)
#define     TPH_DATA_RW                     (PSE_BASE_ADDR + 0x00000218)
#define     TPH_DELTA_TOKEN_ACCESS          (PSE_BASE_ADDR + 0x0000021c)


#define     TPL_CTRL_QEPTY                  (PSE_BASE_ADDR + 0x00000220)
#define     TPL_Q_OPER_0                    (PSE_BASE_ADDR + 0x00000224)
#define     TPL_Q_OPER_1                    (PSE_BASE_ADDR + 0x00000228)
#define     TPL_WR_BUF_ALLOC                (PSE_BASE_ADDR + 0x00000230)
#define     TPL_DATA_OPER                   (PSE_BASE_ADDR + 0x00000234)
#define     TPL_DATA_RW                     (PSE_BASE_ADDR + 0x00000238)


#define     LMAC_ENQ_RECORD                	(PSE_BASE_ADDR + 0x00000240)

#define     LMAC_DEQ_RECORD                	(PSE_BASE_ADDR + 0x00000244)
#define     LMAC_GETBUF_RECORD             	(PSE_BASE_ADDR + 0x00000248)

/* Base on PSE datasheet 2013.01.22 release, register 0x0000024c ~ 0x000002b4 are for performance debug. */
#define     LMAC_DATARW_RECORD              (PSE_BASE_ADDR + 0x0000024c)
#define     HIF_ENQ_RECORD                	(PSE_BASE_ADDR + 0x00000250)
#define     HIF_DEQ_RECORD                	(PSE_BASE_ADDR + 0x00000254)
#define     HIF_GETBUF_RECORD               (PSE_BASE_ADDR + 0x00000258)
#define     HIF_DATARW_RECORD               (PSE_BASE_ADDR + 0x0000025c)
#define     CPU_ENQ_RECORD                	(PSE_BASE_ADDR + 0x00000260)
#define     CPU_DEQ_RECORD                	(PSE_BASE_ADDR + 0x00000264)
#define     CPU_GETBUF_RECORD               (PSE_BASE_ADDR + 0x00000268)



#define	    REC_LMAC_C						(PSE_BASE_ADDR + 0x00000240)
#define	    REC_LMACENQ_0                   (PSE_BASE_ADDR + 0x00000244)
#define	    REC_LMACENQ_1                   (PSE_BASE_ADDR + 0x00000248)
#define	    REC_LMACDEQ_0                   (PSE_BASE_ADDR + 0x0000024c)
#define	    REC_LMACDEQ_1                   (PSE_BASE_ADDR + 0x00000250)
#define	    REC_LMACBUF_0                   (PSE_BASE_ADDR + 0x00000254)
#define	    REC_LMACBUF_1                   (PSE_BASE_ADDR + 0x00000258)
#define	    REC_LMACDATA_0                  (PSE_BASE_ADDR + 0x0000025c)
#define	    REC_LMACDATA_1                  (PSE_BASE_ADDR + 0x00000260)
#define	    REC_LMAC_LAST                   (PSE_BASE_ADDR + 0x00000264)

#define	    REC_HIF_C     					(PSE_BASE_ADDR + 0x00000268)
#define	    REC_HIFENQ_0                    (PSE_BASE_ADDR + 0x0000026c)
#define	    REC_HIFENQ_1                    (PSE_BASE_ADDR + 0x00000270)
#define	    REC_HIFDEQ_0                    (PSE_BASE_ADDR + 0x00000274)
#define	    REC_HIFDEQ_1                    (PSE_BASE_ADDR + 0x00000278)
#define	    REC_HIFBUF_0                    (PSE_BASE_ADDR + 0x0000027c)
#define	    REC_HIFBUF_1                    (PSE_BASE_ADDR + 0x00000280)
#define	    REC_HIFDATA_0                   (PSE_BASE_ADDR + 0x00000284)
#define	    REC_HIFDATA_1                   (PSE_BASE_ADDR + 0x00000288)
#define	    REC_HIF_LAST                    (PSE_BASE_ADDR + 0x0000028c)

#define	    REC_CPU_C     					(PSE_BASE_ADDR + 0x00000290)
#define	    REC_CPUENQ_0                    (PSE_BASE_ADDR + 0x00000294)
#define	    REC_CPUENQ_1                    (PSE_BASE_ADDR + 0x00000298)
#define	    REC_CPUDEQ_0                    (PSE_BASE_ADDR + 0x0000029c)
#define	    REC_CPUDEQ_1                    (PSE_BASE_ADDR + 0x000002a0)
#define	    REC_CPUBUF_0                    (PSE_BASE_ADDR + 0x000002a4)
#define	    REC_CPUBUF_1                    (PSE_BASE_ADDR + 0x000002a8)
#define	    REC_CPUDATA_0                   (PSE_BASE_ADDR + 0x000002ac)
#define	    REC_CPUDATA_1                   (PSE_BASE_ADDR + 0x000002b0)
#define	    REC_CPU_LAST                    (PSE_BASE_ADDR + 0x000002b4)






#define     PSE_DATA_MEM_0                  (PSE_BASE_ADDR + 0x00000300)


#define     PSE_BUF_CTRL_PAGE_SIZE_CFG_OFFSET_BIT          14
#define     PSE_CIF_FRAME_DW_MAX                         BITS(13,0)

#define     CIF_ACCESS_NBLK_BIT             14
#define     CIF_BUSY_BIT                    15


/* Debug port usage constant start */
#define     HIF_PSE_D_OPER_READ             0
#define     HIF_PSE_D_OPER_WRITE            1

#define     HIF_PSE_D_SINGLE_DEFAULT_0      0
#define     HIF_PSE_D_LAST_DEFAULT_0        0

#define     HIF_PSE_QUERY_OPER_0_RESET                      0   //0,1,4
#define     HIF_PSE_QUERY_OPER_1_GET_NEXT                   1   //0,1,4
#define     HIF_PSE_QUERY_OPER_4_LOAD_FID                   4   //0,1,4

#define     HIF_PSE_Q_OPER_2_EN_QUEUE                       2   //2,3,5     from a virtual queue to be enqueue to a output queue
#define     HIF_PSE_Q_OPER_3_DE_QUEUE_THEN_EN_QUEUE         3   //2,3,5     from an output queue to be dequeue and then enqueue to another output queue.
#define     HIF_PSE_Q_OPER_5_DELAYED_EN_QUEUE               5   //2,3,5     from a virtual queue to be enqueue to a output queue with delay



#define     WF_PSE_D_OPER_READ              0
#define     WF_PSE_D_OPER_WRITE             1

#define     WF_PSE_D_SINGLE_DEFAULT_0       0
#define     WF_PSE_D_LAST_DEFAULT_0         0

#define     WF_PSE_QUERY_OPER_0_RESET                       0   //0,1,4
#define     WF_QUERY_OPER_1_GET_NEXT                        1   //0,1,4
#define     WF_PSE_QUERY_OPER_4_LOAD_FID                    4   //0,1,4

#define     WF_PSE_Q_OPER_2_EN_QUEUE                        2   //2,3,5     from a virtual queue to be enqueue to a output queue
#define     WF_PSE_Q_OPER_3_DE_QUEUE_THEN_EN_QUEUE          3   //2,3,5     from an output queue to be dequeue and then enqueue to another output queue.
#define     WF_PSE_Q_OPER_5_DELAYED_EN_QUEUE                5   //2,3,5     from a virtual queue to be enqueue to a output queue with delay


/* PSE Performance related definitions */

#define 	PSE_ENABLE_PER_REC					1
#define 	PSE_DISABLE_PER_REC					0

#define 	PSE_PER_REG_RANGE					0x28
#define 	PSE_PER_LMAC_REG_GROUP_0			0x0*(PSE_PER_REG_RANGE)
#define 	PSE_PER_HIF_REG_GROUP_1				0x1*(PSE_PER_REG_RANGE)
#define 	PSE_PER_CPU_REG_GROUP_2				0x2*(PSE_PER_REG_RANGE)


#define 	RECORD_EN_MASK 						0x1
#define 	DATARW_THD_MASK						0x7f
#define 	GETBUF_THD_MASK						0x7f
#define 	DEQ_THD_MASK						0x7f
#define 	ENQ_THD_MASK						0x7f

#define 	RECORD_EN_SHIFT_BIT 				31
#define 	DATARW_THD_SHIFT_BIT				24
#define 	GETBUF_THD_SHIFT_BIT				16
#define 	DEQ_THD_SHIFT_BIT					8
#define 	ENQ_THD_SHIFT_BIT					0

#define 	PSE_PER_REG_BYTES					4

#define 	PSE_REC_ENQ_0_OFFSET				1*(PSE_PER_REG_BYTES)
#define 	PSE_REC_ENQ_1_OFFSET				2*(PSE_PER_REG_BYTES)

#define 	PSE_REC_DEQ_0_OFFSET				3*(PSE_PER_REG_BYTES)
#define 	PSE_REC_DEQ_1_OFFSET				4*(PSE_PER_REG_BYTES)

#define 	PSE_REC_BUF_0_OFFSET				5*(PSE_PER_REG_BYTES)
#define 	PSE_REC_BUF_1_OFFSET				6*(PSE_PER_REG_BYTES)

#define 	PSE_REC_DATA_0_OFFSET				7*(PSE_PER_REG_BYTES)
#define 	PSE_REC_DATA_1_OFFSET				8*(PSE_PER_REG_BYTES)

#define 	PSE_REC_LAST_OFFSET				    9*(PSE_PER_REG_BYTES)

/* Debug port usage constant end */



/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/


typedef union   _PSE_VER_CTRL_STRUC_T {
    struct  {
        UINT_32     pse_id:8;
        UINT_32     chip_id:24;
    }   field;
    UINT_32         word;
} PSE_VER_CTRL_STRUC_T, *P_PSE_VER_CTRL_STRUC_T;


typedef union   _PSE_INT_STS_STRUC_T {
    struct  {
        UINT_32     int_en_p1q0_ne:1;
        UINT_32     int_en_p1q1_ne:1;
        UINT_32     int_en_p1q2_ne:1;
        UINT_32     int_en_p1q3_ne:1;
        UINT_32     int_en_p_f:1;
        UINT_32     int_en_f_f:1;
        UINT_32     int_en_q_f:1;
        UINT_32     int_en_wd_to:1;
        UINT_32     int_en_page_udf:1;
        UINT_32     int_en_len_ovf:1;
        UINT_32     int_en_hif_enq:1;
        UINT_32     int_en_lmac_enq:1;

        UINT_32     reserve10_31:20;
    }   field;
    UINT_32         word;
} PSE_INT_STS_STRUC_T, *P_PSE_INT_STS_STRUC_T;



typedef union   _PSE_INT_ERR_REASON_STRUC_T {
    struct  {
        UINT_32     err_int_id:5;
        UINT_32     reserve5_7:3;
        UINT_32     err_type:4;
        UINT_32     reserve12_15:4;
        UINT_32     err_pid:2;
        UINT_32     reserve18_23:6;
        UINT_32     err_cif_reg_idx:6;
        UINT_32     reserve30_31:2;
    }   field;
    UINT_32         word;
} PSE_INT_ERR_REASON_STRUC_T, *P_PSE_INT_ERR_REASON_STRUC_T;



typedef union   _PSE_BUF_CTRL_STRUC_T {
    struct  {
        UINT_32     total_page_num:12;
        UINT_32     page_num_lmt_en:1;
        UINT_32     reserve12:1;
        UINT_32     page_size_cfg:2;
        UINT_32     mem_bank_cfg:16;
    }   field;
    UINT_32         word;
} PSE_BUF_CTRL_STRUC_T, *P_PSE_BUF_CTRL_STRUC_T;


typedef union   _CIF_MISC_CTRL_STRUC_T {
    struct  {
        UINT_32     cif_frame_dw:14;
        UINT_32     cif_access_nblk:1;
        UINT_32     cif_busy:1;
        UINT_32     reserve16_31:16;
    }   field;
    UINT_32         word;
} CIF_MISC_CTRL_STRUC_T, *P_CIF_MISC_CTRL_STRUC_T;


typedef union   _CIF_VM_ADDR_CTRL_STRUC_T {
    struct  {
        UINT_32     vm_frame_max_len:3;
        UINT_32     reserve3_10:8;
        UINT_32     vm_frame_base:21;
    }   field;
    UINT_32         word;
} CIF_VM_ADDR_CTRL_STRUC_T, *P_CIF_VM_ADDR_CTRL_STRUC_T;


typedef union   _CIF_GET_FIRST_FID_STRUC_T {
    struct  {
        UINT_32     get_first_fid:12;
        UINT_32     reserve12_15:4;
        UINT_32     get_qid:5;
        UINT_32     get_pid:2;
        UINT_32     get_rvse:1;
        UINT_32     reserve24_31:8;
    }   field;
    UINT_32         word;
} CIF_GET_FIRST_FID_STRUC_T, *P_CIF_GET_FIRST_FID_STRUC_T;


typedef union   _CIF_GET_FID_STRUC_T {
    struct  {
        UINT_32     get_return_fid:12;
        UINT_32     reserve12_15:4;
        UINT_32     curr_fid:12;
        UINT_32     get_prev:1;
        UINT_32     get_auto:1;
        UINT_32     reserve30_31:2;
    }   field;
    UINT_32         word;
} CIF_GET_FID_STRUC_T, *P_CIF_GET_FID_STRUC_T;


typedef union   _CIF_DEQ_SIG_STRUC_T {
    struct  {
        UINT_32     deq_sig_return_fid:12;
        UINT_32     reserve12_15:4;
        UINT_32     deq_sig_qid:5;
        UINT_32     deq_sig_pid:2;
        UINT_32     reserve23_30:8;
        UINT_32     deq_sig_rvse:1;
    }   field;
    UINT_32         word;
} CIF_DEQ_SIG_STRUC_T, *P_CIF_DEQ_SIG_STRUC_T;


typedef union   _CIF_ENQ_SIG_STRUC_T {
    struct  {
        UINT_32     enq_sig_fid:12;
        UINT_32     reserve12_15:4;
        UINT_32     enq_sig_qid:5;
        UINT_32     enq_sig_pid:2;
        UINT_32     reserve23_30:8;
        UINT_32     enq_sig_rvse:1;
    }   field;
    UINT_32         word;
} CIF_ENQ_SIG_STRUC_T, *P_CIF_ENQ_SIG_STRUC_T;


typedef union   _CIF_ENQ_LIST_STRUC_T {
    struct  {
        UINT_32     enq_list_fid_end:12;
        UINT_32     enq_list_fid_start:12;
        UINT_32     enq_list_qid:5;
        UINT_32     enq_list_pid:2;
        UINT_32     enq_list_rvse:1;
    }   field;
    UINT_32         word;
} CIF_ENQ_LIST_STRUC_T, *P_CIF_ENQ_LIST_STRUC_T;


typedef union   _CIF_OODEQ_SIG_STRUC_T {
    struct  {
        UINT_32     oodeq_sig_fid:12;
        UINT_32     reserve12_15:4;
        UINT_32     oodeq_sig_src_qid:5;
        UINT_32     oodeq_sig_src_pid:2;
        UINT_32     reserve23_31:9;
    }   field;
    UINT_32         word;
} CIF_OODEQ_SIG_STRUC_T, *P_CIF_OODEQ_SIG_STRUC_T;


typedef union   _CIF_OOENQ_SIG_STRUC_T {
    struct  {
        UINT_32     ooenq_sig_ref_fid:12;
        UINT_32     ooenq_sig_fid:12;
        UINT_32     oodeq_sig_ref_qid:5;
        UINT_32     oodeq_sig_ref_pid:2;
        UINT_32     ooenq_sig_bfre:1;
    }   field;
    UINT_32         word;
} CIF_OOENQ_SIG_STRUC_T, *P_CIF_OOENQ_SIG_STRUC_T;


typedef union   _CIF_OODEQ_LIST_STRUC_T {
    struct  {
        UINT_32     oodeq_list_fid_end:12;
        UINT_32     oodeq_list_fid_start:12;
        UINT_32     oodeq_list_src_qid:5;
        UINT_32     oodeq_list_src_pid:2;
        UINT_32     reserve31:1;
    }   field;
    UINT_32         word;
} CIF_OODEQ_LIST_STRUC_T, *P_CIF_OODEQ_LIST_STRUC_T;


typedef union   _CIF_OOENQ_LIST_0_STRUC_T {
    struct  {
        UINT_32     ooenq_list_fid_end:12;
        UINT_32     reserve12_15:4;
        UINT_32     ooenq_list_fid_start:12;
        UINT_32     reserve28_30:3;
        UINT_32     ooenq_list_bfre:1;
    }   field;
    UINT_32         word;
} CIF_OOENQ_LIST_0_STRUC_T, *P_CIF_OOENQ_LIST_0_STRUC_T;


typedef union   _CIF_OOENQ_LIST_1_STRUC_T {
    struct  {
        UINT_32     ooenq_list_ref_fid:12;
        UINT_32     reserve12_15:4;
        UINT_32     ooenq_list_ref_qid:5;
        UINT_32     ooenq_list_ref_pid:2;
        UINT_32     reserve23_31:9;
    }   field;
    UINT_32         word;
} CIF_OOENQ_LIST_1_STRUC_T, *P_CIF_OOENQ_LIST_1_STRUC_T;


typedef union   _CIF_DEQ_ALL_STRUC_T {
    struct  {
        UINT_32     deq_all_return_fid_end:12;
        UINT_32     deq_all_return_fid_start:12;
        UINT_32     deq_all_qid:5;
        UINT_32     deq_all_pid:2;
        UINT_32     reserve31:1;
    }   field;
    UINT_32         word;
} CIF_DEQ_ALL_STRUC_T, *P_CIF_DEQ_ALL_STRUC_T;


typedef union   _CIF_QRW_GENERIC_STRUC_T {
    struct  {
        UINT_32     rw_fid_p_x__q_y:12;
        UINT_32     reserve12_31:19;
        UINT_32     rw:1;
    }   field;
    UINT_32         word;
} CIF_QRW_GENERIC_STRUC_T, *P_CIF_QRW_GENERIC_STRUC_T;


typedef union   _BUF_SHARE_CTRL_STRUC_T {
    struct  {
        UINT_32     hw_rdd_cfg:2;
        UINT_32     sw_rdd_cfg:2;
        UINT_32     dscope_mode_cfg:3;
        UINT_32     dscope_mode_en:1;
        UINT_32     cif_lp_aloc_pgnm:7;
        UINT_32     backup_aloc_pgnm:7;
        UINT_32     sw_only_aloc_pgnm:10;
    }   field;
    UINT_32         word;
} BUF_SHARE_CTRL_STRUC_T, *P_BUF_SHARE_CTRL_STRUC_T;

typedef union   _PSE_PAUSE_RD_CTRL_STRUC_T {
    struct  {
        UINT_32      sp_pause_en:3;
        UINT_32      rsv1:1;
        UINT_32      sp_rd_en:3;
        UINT_32      rsv2:1;
        UINT_32      tx_rd_all_qid:5;
        UINT_32      tx_rd_all_pid:2;
        UINT_32      tx_rd_all_en:1;
        UINT_32      rsv3:16;
    }   field;
    UINT_32          word;
} PSE_PAUSE_RD_CTRL_STRUC_T, *P_PSE_PAUSE_RD_CTRL_STRUC_T;

typedef union   _PSE_RD_TAB_ACCESS_STRUC_T {
    struct  {
        UINT_32      rd_rule_qid:5;
        UINT_32      rd_rule_pid:2;
        UINT_32      rd_rule_force:1;
        UINT_32      rd_tag:8;
        UINT_32      rd_rw:1;
        UINT_32      reserve17_30:14;
        UINT_32      rd_kick_busy:1;
    }   field;
    UINT_32          word;
} PSE_RD_TAB_ACCESS_STRUC_T, *P_PSE_RD_TAB_ACCESS_STRUC_T;



typedef union   _PSE_RL_INFO_ACCESS_STRUC_T {
    struct  {
        UINT_32      rl_addr:12;
        UINT_32      reserve12_15:4;
        UINT_32      rl_rw:1;
        UINT_32      reserve17_30:14;
        UINT_32      rl_kick_busy:1;
    }   field;
    UINT_32          word;
} PSE_RL_INFO_ACCESS_STRUC_T, *P_PSE_RL_INFO_ACCESS_STRUC_T;


typedef union   _PSE_RL_INFO_DATA_STRUC_T {
    struct  {
        UINT_32      rl_dst_qid:5;
        UINT_32      rl_dst_pid:2;
        UINT_32      rl_srcpidqid_tag:8;
        UINT_32      rl_page_num:9;
        UINT_32      rl_pause:1;
        UINT_32      rl_fid_vld:1;
        UINT_32      reserve26_31:6;
    }   field;
    UINT_32         word;
} PSE_RL_INFO_DATA_STRUC_T, *P_PSE_RL_INFO_DATA_STRUC_T;


typedef union   _CIF_DENQ_LIST_0_STRUC_T {
    struct  {
        UINT_32     denq_list_fid_end:12;
        UINT_32     denq_list_fid_start:12;
        UINT_32     denq_list_qid:5;
        UINT_32     denq_list_pid:2;
        UINT_32     denq_list_rvse:1;

    }   field;
    UINT_32         word;
} CIF_DENQ_LIST_0_STRUC_T, *P_CIF_DENQ_LIST_0_STRUC_T;

typedef union   _CIF_DENQ_LIST_1_STRUC_T {
    struct  {
        UINT_32     denq_list_src_qid:5;
        UINT_32     denq_list_src_pid:2;
        UINT_32     reserve7_31:25;
    }   field;
    UINT_32         word;
} CIF_DENQ_LIST_1_STRUC_T, *P_CIF_DENQ_LIST_1_STRUC_T;


typedef union   _CIF_OODENQ_LIST_0_STRUC_T {
    struct  {
        UINT_32     oodenq_list_fid_end:12;
        UINT_32     reserve12_15:4;
        UINT_32     oodenq_list_fid_start:12;
        UINT_32     reserve28_30:3;
        UINT_32     oodenq_list_rvse:1;

    }   field;
    UINT_32         word;
} CIF_OODENQ_LIST_0_STRUC_T, *P_CIF_OODENQ_LIST_0_STRUC_T;


typedef union   _CIF_OODENQ_LIST_1_STRUC_T {
    struct  {
        UINT_32     oodenq_list_ref_fid:12;
        UINT_32     reserve12_15:4;
        UINT_32     oodenq_list_src_qid:5;
        UINT_32     oodenq_list_src_pid:2;
        UINT_32     reserve23:1;
        UINT_32     oodenq_list_ref_qid:5;
        UINT_32     oodenq_list_ref_pid:2;
        UINT_32     reserve31:1;
    }   field;
    UINT_32         word;
} CIF_OODENQ_LIST_1_STRUC_T, *P_CIF_OODENQ_LIST_1_STRUC_T;


typedef union   _GET_QLEN_CTRL_STRUC_T {
    struct  {
        UINT_32     qlen_return_value:12;
        UINT_32     reserve12_15:4;
        UINT_32     get_qlen_qid:5;
        UINT_32     get_qlen_pid:2;
        UINT_32     qlen_in_page:1;
        UINT_32     reserve24_31:8;
    }   field;
    UINT_32         word;
} GET_QLEN_CTRL_STRUC_T, *P_GET_QLEN_CTRL_STRUC_T;



typedef union   _FC_FFA_RSRV_PRI_STRUC_T {
    struct  {
        UINT_32     rsrv_pri_p0:3;
        UINT_32     rsrv_pri_p1:3;
        UINT_32     rsrv_pri_p2_rq0:3;
        UINT_32     rsrv_pri_p2_rq1:3;
        UINT_32     rsrv_pri_p2_rq2:3;
        UINT_32     en_dcnt_p2_qx:1;
        UINT_32     reserve16_31:16;
    }   field;
    UINT_32         word;
} FC_FFA_RSRV_PRI_STRUC_T, *P_FC_FFA_RSRV_PRI_STRUC_T;



typedef union   _PSE_RELAY_INFO_0_STRUC_T {
    struct  {
        UINT_32     dst_qid:5;
        UINT_32     dst_pid:2;
        UINT_32     dst_vld:1;
        UINT_32     srcpidqid_tag:8;
        UINT_32     page_num:8;
        UINT_32     pause:1;
        UINT_32     fid_vld:1;
        UINT_32     reserve26_31:6;
    }   field;
    UINT_32         word;
} PSE_RELAY_INFO_0_STRUC_T, *P_PSE_RELAY_INFO_0_STRUC_T;


typedef union   _PSE_FL_TAB_ACCESS_STRUC_T {
    struct  {
        UINT_32     fl_addr:12;
        UINT_32     reserve12_30:19;
        UINT_32     fl_busy:1;
    }   field;
    UINT_32         word;
} PSE_FL_TAB_ACCESS_STRUC_T, *P_PSE_FL_TAB_ACCESS_STRUC_T;


typedef union   _PSE_FL_TAB_DATA_STRUC_T {
    struct  {
        UINT_32     fl_next:12;
        UINT_32     reserve12_15:4;
        UINT_32     fl_prev:12;
        UINT_32     reserve28_31:4;
    }   field;
    UINT_32         word;
} PSE_FL_TAB_DATA_STRUC_T, *P_PSE_FL_TAB_DATA_STRUC_T;


typedef union   _PSE_PL_TAB_ACCESS_STRUC_T {
    struct  {
        UINT_32     pl_data:12;
        UINT_32     reserve12_15:4;
        UINT_32     pl_addr:12;
        UINT_32     reserve28_30:3;
        UINT_32     pl_busy:1;
    }   field;
    UINT_32         word;
} PSE_PL_TAB_ACCESS_STRUC_T, *P_PSE_PL_TAB_ACCESS_STRUC_T;


#if 0
typedef union   _LP_PSE_CTRL_STRUC_T {
    struct  {
        UINT_32     reserve0_6:7;
        UINT_32     pse_lp_wakeup:1;
        UINT_32     pse2hif_pass_en:1;
        UINT_32     retention_fl_en:1;
        UINT_32     reserve10_15:6;
        UINT_32		retention_qid:5;
        UINT_32     reserve21_31:11;
    }   field;
    UINT_32         word;
} LP_PSE_CTRL_STRUC_T, *P_LP_PSE_CTRL_STRUC_T;
#endif

typedef union   _LP_PSE_CTRL_STRUC_T {
    struct  {
        UINT_32     reserve0_3:4;
        UINT_32     pse_lp_idle_d_legacy:1;
        UINT_32     pse_dis_clk_gating:1;
        UINT_32     pse_en_ffa_info:1;
        UINT_32     pse_lp_wakeup:1;
        UINT_32     pse2hif_pass_en:1;
        UINT_32     retention_fl_en:1;
        UINT_32     reserve10_15:6;
        UINT_32		retention_qid:5;
        UINT_32     reserve21_31:11;
    }   field;
    UINT_32         word;
} LP_PSE_CTRL_STRUC_T, *P_LP_PSE_CTRL_STRUC_T;


typedef union   _CIF_BURST_ACCESS_STRUC_T {
    struct  {
        UINT_32     cif_burst_dw_cnt:3;
        UINT_32     reserve3:1;
        UINT_32     cif_burst_wr_1dw_rls:1;
        UINT_32     cif_burst_wr_en:1;
        UINT_32     cif_burst_rd_1dw_rls:1;
        UINT_32     cif_burst_rd_en:1;
        UINT_32     cif_burst_rdy_map:8;
        UINT_32     reserve16_31:16;
    }   field;
    UINT_32         word;
} CIF_BURST_ACCESS_STRUC_T, *P_CIF_BURST_ACCESS_STRUC_T;



/* Test Port HIF Ctrl and Queue Empty Signaling */
typedef union   _TPH_CTRL_QEPTY_STRUC_T {
    struct  {
        UINT_32     hif_q_empty:2;
        UINT_32     reserve2_30:29;
        UINT_32     tp_hif_en:1;
    }   field;
    UINT_32         word;
} TPH_CTRL_QEPTY_STRUC_T, *P_TPH_CTRL_QEPTY_STRUC_T;


/* Test Port HIF Queue Operation Register 0 */
typedef union   _TPH_Q_OPER_0_STRUC_T {
    struct  {
        UINT_32     pse_hif_curr_fid:12;
        UINT_32     reserve12_14:3;
        UINT_32     pse_hif_q_end:1;
        UINT_32     hif_pse_src_qid:5;
        UINT_32     reserve21_23:3;
        UINT_32     hif_pse_q_oper:3;
        UINT_32     reserve27_30:4;
        UINT_32     hif_pse_q_oper_req:1;
    }   field;
    UINT_32         word;
} TPH_Q_OPER_0_STRUC_T, *P_TPH_Q_OPER_0_STRUC_T;


/* Test Port HIF Queue Operation Register 1 */
typedef union   _TPH_Q_OPER_1_STRUC_T {
    struct  {
        UINT_32     hif_pse_enq_tag:8;
        UINT_32     hif_pse_dst_qid:5;
        UINT_32     hif_pse_dst_pid:2;
        UINT_32     reserve15:1;
        UINT_32     hif_pse_src_fid:12;
        UINT_32     reserve28_31:4;
    }   field;
    UINT_32         word;
} TPH_Q_OPER_1_STRUC_T, *P_TPH_Q_OPER_1_STRUC_T;


/* Test Port HIF Write Buffer Allocate Register */
typedef union   _TPH_WR_BUF_ALLOC_STRUC_T {
    struct  {
        UINT_32     pse_hif_wbuf_fid:12;
        UINT_32     hif_pse_wbuf_qid:5;
        UINT_32     hif_pse_wbuf_size:14;
        UINT_32     hif_pse_wbuf_req:1;
    }   field;
    UINT_32         word;
} TPH_WR_BUF_ALLOC_STRUC_T, *P_TPH_WR_BUF_ALLOC_STRUC_T;


/* Test Port HIF Data Operation Register */
typedef union   _TPH_DATA_OPER_STRUC_T {
    struct  {
        UINT_32     hif_pse_d_ofst:14;
        UINT_32     hif_pse_d_last:1;
        UINT_32     hif_pse_d_single:1;
        UINT_32     hif_pse_d_fid:12;
        UINT_32     hif_pse_d_bst_len:3;
        UINT_32     hif_pse_d_oper:1;
    }   field;
    UINT_32         word;
} TPH_DATA_OPER_STRUC_T, *P_TPH_DATA_OPER_STRUC_T;

/* Test Port HIF Data RW Register */
typedef union   _TPH_DATA_RW_STRUC_T {
    struct  {
        UINT_32     hif_rw_data:32;
    }   field;
    UINT_32         word;
} TPH_DATA_RW_STRUC_T, *P_TPH_DATA_RW_STRUC_T;

/* Test Port HIF Delta Token Access Register */
typedef union   _TPH_DELTA_TOKEN_ACCESS_STRUC_T {
    struct  {
        UINT_32     hif_dltkn_sum:12;
        UINT_32     reserve12_15:4;
        UINT_32     hif_dltkn_id:5;
        UINT_32     reserve21_30:10;
        UINT_32     hif_dltkn_rd_req:1;
    }   field;
    UINT_32         word;
} TPH_DELTA_TOKEN_ACCESS_STRUC_T, *P_TPH_DELTA_TOKEN_ACCESS_STRUC_T;


/* Test Port LMAC Ctrl and Queue Empty Signaling */
typedef union   _TPL_CTRL_QEPTY_STRUC_T {
    struct  {
        UINT_32     wf_q_empty:14;
        UINT_32     reserve14_30:17;
        UINT_32     tp_wf_en:1;
    }   field;
    UINT_32         word;
} TPL_CTRL_QEPTY_STRUC_T, *P_TPL_CTRL_QEPTY_STRUC_T;



/* Test Port LMAC Queue Operation Register 0 */
typedef union   _TPL_Q_OPER_0_STRUC_T {
    struct  {
        UINT_32     pse_wf_curr_fid:12;
        UINT_32     reserve12_13:3;
        UINT_32     pse_wf_q_end:1;
        UINT_32     wf_pse_src_qid:5;
        UINT_32     reserve21_23:3;
        UINT_32     wf_pse_q_oper:3;
        UINT_32     reserve27_30:4;
        UINT_32     wf_pse_q_oper_req:1;
    }   field;
    UINT_32         word;
} TPL_Q_OPER_0_STRUC_T, *P_TPL_Q_OPER_0_STRUC_T;


/* Test Port LMAC Queue Operation Register 1 */
typedef union   _TPL_Q_OPER_1_STRUC_T {
    struct  {
        UINT_32     wf_pse_enq_tag:8;
        UINT_32     wf_pse_dst_qid:5;
        UINT_32     wf_pse_dst_pid:2;
        UINT_32     reserve15:1;
        UINT_32     wf_pse_src_fid:12;
        UINT_32     reserve28_31:4;
    }   field;
    UINT_32         word;
} TPL_Q_OPER_1_STRUC_T, *P_TPL_Q_OPER_1_STRUC_T;


/* Test Port LMAC Write Buffer Allocate Register */
typedef union   _TPL_WR_BUF_ALLOC_STRUC_T {
    struct  {
        UINT_32     pse_wf_wbuf_fid:12;
        UINT_32     wf_pse_wbuf_qid:5;
        UINT_32     wf_pse_wbuf_size:14;
        UINT_32     wf_pse_wbuf_req:1;
    }   field;
    UINT_32         word;
} TPL_WR_BUF_ALLOC_STRUC_T, *P_TPL_WR_BUF_ALLOC_STRUC_T;


/* Test Port LMAC Data Operation Register */
typedef union   _TPL_DATA_OPER_STRUC_T {
    struct  {
        UINT_32     wf_pse_d_ofst:14;
        UINT_32     wf_pse_d_last:1;
        UINT_32     wf_pse_d_single:1;
        UINT_32     wf_pse_d_fid:12;
        UINT_32     wf_pse_d_bst_len:3;
        UINT_32     wf_pse_d_oper:1;
    }   field;
    UINT_32         word;
} TPL_DATA_OPER_STRUC_T, *P_TPL_DATA_OPER_STRUC_T;



/* Test Port LMAC Data RW Register */
typedef union   _TPL_DATA_RW_STRUC_T {
    struct  {
        UINT_32     wf_rw_data:32;
    }   field;
    UINT_32         word;
} TPL_DATA_RW_STRUC_T, *P_TPL_DATA_RW_STRUC_T;


/* The gernel structure for recording performance for PSE. */

typedef union   _REC_GENERAL_C_STRUC_T {
    struct  {
        UINT_32     general_enq_thd:7;
        UINT_32     reserve8:1;
        UINT_32     general_deq_thd:7;
        UINT_32     reserve15:1;
        UINT_32     general_getbuf_thd:7;
        UINT_32     reserve23:1;
        UINT_32     general_datarw_thd:7;
        UINT_32     general_record_en:1;
    }   field;
    UINT_32         word;
} REC_GENERAL_C_STRUC_T, P_REC_GENERAL_C_STRUC_T;



typedef union   _REC_GENERAL_LAST_STRUC_T {
    struct  {
        UINT_32     reserve0_4:5;
        UINT_32     general_last_time_0:7;
        UINT_32     general_last_cmd_0:2;
        UINT_32     general_last_time_1:7;
        UINT_32     general_last_cmd_1:2;
        UINT_32     general_last_time_2:7;
        UINT_32     general_last_cmd_2:2;
    }   field;
    UINT_32         word;
} REC_GENERAL_LAST_STRUC_T, P_REC_GENERAL_LAST_STRUC_T;


/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _HAL_PSE_MT6630_DEF_ROM_H */
