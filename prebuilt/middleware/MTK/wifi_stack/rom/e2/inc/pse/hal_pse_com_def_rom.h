/*
** $Id: @(#) hal_pse_com_def_rom.h $
*/

/*! \file   hal_pse_com_def_rom.h
    \brief  Common use definations for PSE are defined in this file

    Common definitions are defined in this file to be referred.

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
** $Log: hal_pse_com_def_rom.h $
**
** 06 18 2015 tiger.hung
** [BORA00004558] [MT7637][E2] Checkin
** .Correct Tx queue definition
**
** 06 20 2014 shuojen.hsu
** [BORA00003152] [MT7637][WiFi] Merge emulation code from MT7636_E2_MP
** .
**
** 06 16 2014 shuojen.hsu
** [BORA00003152] [MT7637][WiFi] Merge emulation code from MT7636_E2_MP
** .
**
** 12 11 2013 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Add MT7636 PSE Page Count
**
** 06 04 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** 1. Remove unused code 2. move the API to RAM 3. enable crypto
**
** 05 10 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [mt6630] [pse] update PSE emulation test item for share buffer related verifications.
**
** 04 24 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] [PSE] Add a marco to define the size of total PSE buffer in bytes.
**
** 03 07 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] [PSE]
** 1. Re-sync LMAC TX naming and Port2 Queue ID mapping.
** 2. Add some API in ROM code indirect access array.
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
**/


#ifndef _HAL_PSE_COM_DEF_ROM_H
#define _HAL_PSE_COM_DEF_ROM_H

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
#define PSE_CHIP_ID                                     0x763600

#define PSE_MAX_FRAME_ID_RANGE                          0xfff
#define PSE_FRAME_FAULT                                 0xfff

#define PSE_MAX_QUOTA_PAGE_CNT                          0x0fff
#define PSE_MIN_RESERVE_PAGE_CNT                        0x0000


/* Page size of PSE:  2'd0: 128B, 2'd1: 256B, 2'd2: 512B, 2'd3: 1024B */
#define  PSE_PAGE_LEN_128B_IDX	                        0
#define  PSE_PAGE_LEN_256B_IDX                          1
#define  PSE_PAGE_LEN_512B_IDX                          2
#define  PSE_PAGE_LEN_1024B_IDX                         3
//#define  PSE_PAGE_LEN_2048B_IDX                         3

#define  PSE_PAGE_LEN_128B_SIZE                         128
#define  PSE_PAGE_LEN_256B_SIZE                         256
#define  PSE_PAGE_LEN_512B_SIZE                         512
#define  PSE_PAGE_LEN_1024B_SIZE                        1024
//#define  PSE_PAGE_LEN_2048B_SIZE                        2048


/* Max Frame length parameters 3'd0: 2KB, 3'd1: 4KB, 3'd2: 8KB, 3'd3: 16KB */
#define PSE_VM_FRAME_MAX_LEN_2KB_IDX                    0
#define PSE_VM_FRAME_MAX_LEN_4KB_IDX                    1
#define PSE_VM_FRAME_MAX_LEN_8KB_IDX                    2
#define PSE_VM_FRAME_MAX_LEN_16KB_IDX                   3

#define PSE_VM_FRAME_MAX_LEN_2KB_SIZE                   2048
#define PSE_VM_FRAME_MAX_LEN_4KB_SIZE                   4096
#define PSE_VM_FRAME_MAX_LEN_8KB_SIZE                   8192
#define PSE_VM_FRAME_MAX_LEN_16KB_SIZE                 16384

#define PSE_EMUL_4_BYTE_ACCESS                          4
#define PSE_EMUL_2_BYTE_ACCESS                          2
#define PSE_EMUL_1_BYTE_ACCESS                          1

#define PSE_PAGE_CNT_IN_MT6630                          1064    //PSE buffer = 133KB
#define PSE_PAGE_CNT_IN_MT7603                          936
#define PSE_PAGE_CNT_IN_MT7636                          1280    //PSE buffer = 160KB, Posh 20131211, Add MT7636 PSE page count

/* Total Byte count */
#if (PRODUCT_VERSION == 6630)
#define PSE_TOTAL_BUF_BYTE_CNT                          (PSE_PAGE_CNT_IN_MT6630 * PSE_PAGE_LEN_128B_SIZE)
#elif (PRODUCT_VERSION == 7603)
#define PSE_TOTAL_BUF_BYTE_CNT                          (PSE_PAGE_CNT_IN_MT7603 * PSE_PAGE_LEN_128B_SIZE)
#elif (PRODUCT_VERSION == 7636 || PRODUCT_VERSION == 7637)
#define PSE_TOTAL_BUF_BYTE_CNT                          (PSE_PAGE_CNT_IN_MT7636 * PSE_PAGE_LEN_128B_SIZE)
#else
#define PSE_TOTAL_BUF_BYTE_CNT                          (PSE_PAGE_CNT_IN_MT7636 * PSE_PAGE_LEN_128B_SIZE)
#endif

/* For 2K byte frame size, it 1024*2 ==> so shift 11 bits. */
#define PSE_BASE_SHIFT_NUM_11_BIT						11

#define PSE_BASE_SHIFT_NUM_16_BIT						16


#define     CIF_Q_READ_GENERIC_QUEUE    0
#define     CIF_Q_WRITE_GENERIC_QUEUE   1


 /* for a non exist queue, or without a specific queue id assignment, use 0xff for software */
#define PSE_QUEUE_NO_EXIST          0xff

#define     PSE_INT_STS_LMAC_ENQ    (0x1 << 12)
#define     PSE_INT_STS_HIF_ENQ     (0x1 << 11)

#define     PSE_INT_STS_LEN_OVF     (0x1 << 10)
#define     PSE_INT_STS_PAGE_UDF    (0x1 << 9)
#define     PSE_INT_STS_WD_TO       (0x1 << 8)
#define     PSE_INT_STS_Q_F         (0x1 << 7)

#define     PSE_INT_STS_L_F         (0x1 << 6)

#define     PSE_INT_STS_F_F         (0x1 << 5)
#define     PSE_INT_STS_P_F         (0x1 << 4)
#define     PSE_INT_STS_P1Q3_NE     (0x1 << 3)
#define     PSE_INT_STS_P1Q2_NE     (0x1 << 2)
#define     PSE_INT_STS_P1Q1_NE     (0x1 << 1)
#define     PSE_INT_STS_P1Q0_NE     (0x1 << 0)

#define     PSE_INT_STS_P1_QALL_NE  (PSE_INT_STS_P1Q0_NE | PSE_INT_STS_P1Q1_NE | PSE_INT_STS_P1Q2_NE | PSE_INT_STS_P1Q3_NE)

/* For INT_STS[10:4], they will be record to field ERR_INT_ID of register PSE_INT_ERR_REASON. So, mask related status for handlering in once */
#define     PSE_ERR_INT_ID_MASK     (PSE_INT_STS_P_F|PSE_INT_STS_F_F|PSE_INT_STS_L_F|PSE_INT_STS_Q_F|PSE_INT_STS_WD_TO|PSE_INT_STS_PAGE_UDF|PSE_INT_STS_LEN_OVF)

#define     PSE_INT_EN_MASK_ALL     0xffff

#define     PSE_INT_SHIFT(x)        (0x1 << x)

#define     PSE_IDX_STS_LEN_OVF     10
#define     PSE_IDX_STS_PAGE_UDF    9
#define     PSE_IDX_STS_WD_TO       8
#define     PSE_IDX_STS_Q_F         7

#define     PSE_IDX_STS_L_F         6

#define     PSE_IDX_STS_F_F         5
#define     PSE_IDX_STS_P_F         4
#define     PSE_IDX_STS_P1Q3_NE     3
#define     PSE_IDX_STS_P1Q2_NE     2
#define     PSE_IDX_STS_P1Q1_NE     1
#define     PSE_IDX_STS_P1Q0_NE     0


#define     PSE_MAX_RD_TAB          0xff

/* FFA priority range */
#define     PSE_FFA_PRIORITY_MAX    0x07
#define     PSE_FFA_PRIORITY_MIN    0x00

/* enable/disable Port 2 DST queue count statistic */
#define 	PSE_DCNT_P2_ENABLE  1
#define 	PSE_DCNT_P2_DISABLE 0

/* queue length in frame count mode or page mode */
#define     PSE_QLEN_IN_FRAME_MODE 	0
#define     PSE_QLEN_IN_PAGE_MODE 	1

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/* PSE PORT LIST */
typedef enum _ENUM_PSE_PORT_T {
    ENUM_HIF_PORT_0         = 0,
    ENUM_CPU_PORT_1         = 1,
    ENUM_LMAC_PORT_2        = 2,
    ENUM_SWITCH_PORT_3      = 3,
    ENUM_PSE_PORT_TOTAL_NUM = 4
} ENUM_PSE_PORT_T, *P_ENUM_PSE_PORT_T;


/* HIF QUEUE LIST */
typedef enum _ENUM_PSE_HIF_P_QUEUE_T {
    ENUM_HIF_HTX_LO_Q_0      = 0,
    ENUM_HIF_HTX_HI_Q_1      = 1,
    ENUM_HIF_PAUSE_Q_P0_Q_2  = 2,   /* Pause Queue ID == ( last output queue ID + 1 ) */
    ENUM_HIF_HRX             = 0,   /* Virtual source queue, keep an ID for software programming */
    ENUM_HIF_RX_QUEUE_NUM    = 1,
    ENUM_HIF_TX_QUEUE_NUM    = 2,
    ENUM_HIF_QUEUE_TOTAL_NUM = 3    /* two rx queue, and one pause queue */
} ENUM_PSE_HIF_P_QUEUE_T, *P_ENUM_PSE_HIF_P_QUEUE_T;


/* CPU(CTX) QUEUE LIST */
typedef enum _ENUM_PSE_CPU_P_QUEUE_T {
    ENUM_CTX_Q_0 = 0,
    ENUM_CTX_Q_1 = 1,
    ENUM_CTX_Q_2 = 2,
    ENUM_CTX_Q_3 = 3,
    ENUM_CRX     = 0,               /* Virtual source queue, keep an ID for software programming */
    ENUM_CIF_QUEUE_TOTAL_NUM = 4    /* 4 tx queue to CPU */
} ENUM_PSE_CPU_P_QUEUE_T, *P_ENUM_PSE_CPU_P_QUEUE_T;


/* LMAC TX QUEUE LIST */
typedef enum _ENUM_PSE_LMAC_TX_P_QUEUE_T {
    ENUM_LMAC_TX_AC0_Q_0        = 0,
    ENUM_LMAC_TX_AC1_Q_1        = 1,
    ENUM_LMAC_TX_AC2_Q_2        = 2,
    ENUM_LMAC_TX_AC3_Q_3        = 3,
    ENUM_LMAC_TX_AC4_Q_4        = 4,
    ENUM_LMAC_TX_AC5_Q_5        = 5,
    ENUM_LMAC_TX_AC6_Q_6        = 6,
    ENUM_LMAC_TX_BEACON7_Q_7    = 7,
    ENUM_LMAC_TX_BCMC8_Q_8      = 8,
    ENUM_LMAC_TX_AC10_Q_9       = 9,
    ENUM_LMAC_TX_AC11_Q_10      = 10,
    ENUM_LMAC_TX_AC12_Q_11      = 11,
    ENUM_LMAC_TX_AC13_Q_12      = 12,
    ENUM_LMAC_TX_AC14_Q_13      = 13,
    ENUM_LMAC_TX_TOTAL_NUM      = 14
} ENUM_PSE_LMAC_TX_P_QUEUE_T, *P_ENUM_PSE_LMAC_TX_P_QUEUE_T;



/* LMAC RX QUEUE LIST */
typedef enum _ENUM_PSE_LMAC_RX_P_QUEUE_T {
    ENUM_LMAC_RX_DATA_Q_0       = 0,    /* virtual queue */
    ENUM_LMAC_RX_VECTOR_Q_1     = 1,    /* virtual queue */
    ENUM_LMAC_RX_TX_STS_Q_2     = 2,    /* virtual queue */
//    ENUM_LMAC_PAUSE_Q_P2_Q_14   = 14,   /* puase queue */
//    ENUM_LMAC_WRX_QS              = 4,    /* Virtual source queue, keep an ID for software programming,  */
                                        /*
                                         * Due to Source Rsv Cnt support for LMAC Virtual Port, So, we assign ENUM_LMAC_WRX_QS = 4
                                         * Otherwise, ENUM_LMAC_WRX_QS was assigned to 0,
                                         */
    ENUM_LMAC_RX_TOTAL_NUM      = 3
} ENUM_PSE_LMAC_RX_P_QUEUE_T, *P_ENUM_PSE_LMAC_RX_P_QUEUE_T;


/* PSE SWITCH QUEUE LIST, INCLUDING FREE QUEUE */
typedef enum _ENUM_PSE_SWITCH_P_QUEUE_T {
    ENUM_SWITCH_SW_Q_0          = 0,
    ENUM_SWITCH_SW_Q_1          = 1,
    ENUM_SWITCH_SW_Q_2          = 2,
    ENUM_SWITCH_SW_Q_3          = 3,
    ENUM_SWITCH_SW_Q_4          = 4,
    ENUM_SWITCH_SW_Q_5          = 5,
    ENUM_SWITCH_SW_Q_6          = 6,
    ENUM_SWITCH_SW_Q_7          = 7,
    ENUM_SWITCH_FREE_Q_8        = 8,
    ENUM_SWITCH_TOTAL_NUM       = 9
} ENUM_PSE_SWITCH_P_QUEUE_T, *P_ENUM_PSE_SWITCH_P_QUEUE_T;

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

#if (CFG_TEST_FPGA_DVT == 1)
#define PSE_ASSERT   ASSERT
#else
#define PSE_ASSERT(x)
#endif /* PSE_DBG */

#if (CFG_TEST_FPGA_DVT == 1)
#define pse_printf                  kalPrint
#define pse_err_printf              kalPrint
#define pse_int_printf              kalPrint
#else
#define pse_printf //printf                   /* Used to show Info msg */
#define pse_err_printf //printf               /* Used to show Error msg */
#define pse_int_printf //printf               /* Used to show Error msg */
#endif

#ifndef BIT
#define BIT(n)                          ((UINT_32) 1 << (n))
#define BITS2(m,n)                      (BIT(m) | BIT(n) )
#define BITS3(m,n,o)                    (BIT(m) | BIT (n) | BIT (o))
#define BITS4(m,n,o,p)                  (BIT(m) | BIT (n) | BIT (o) | BIT(p))

/* bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */
#define BITS(m,n)                       (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /* BIT */

/* bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/


#endif /* _HAL_PSE_COM_DEF_ROM_H */
