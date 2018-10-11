/*
** $Id: //Department/DaVinci/TRUNK/MT6620_WiFi_Firmware/mcu/wifi/include/nic/nic.h#16 $
*/

/*! \file   "nic.h"
    \brief  The declaration of nic functions

    Detail description.
*/

/*******************************************************************************
* Copyright (c) 2007 MediaTek Inc.
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
** $Log: nic.h $
**
** 02 04 2016 tiger.hung
** [BORA00004558] [MT7637][E2] Checkin
** PCIe MACCR value changes for veriwave T-put test
 *
 * 10 13 2015 hasanali.stationwala
 * [BORA00005255] [MT7637E2][IoT][FW]Low throughput in case of data forwarding
 * Modified maxQuota macro.
**
** 08 27 2015 tiger.hung
** [BORA00004558] [MT7637][E2] Checkin
** - PSE buffer modification for MT7657U T-Put
** - PMU POS setting rollback, workaround solution for unstable connection
**
** 08 26 2015 tiger.hung
** [BORA00004558] [MT7637][E2] Checkin
** 1) T-put enhancement support
** 2) correct MAC init. value
**
** 07 24 2015 tiger.hung
** [BORA00004558] [MT7637][E2] Checkin
** Sync E1 Ram code(CL28812) to E2
**
** 07 06 2015 carter.chen
** [BORA00004697] [MT7637] porting BCN_OFFLOAD to N9
** 	1. porting BCN OFFLOAD function from MT7615 to MT7637
**
** 03 13 2015 litien.chang
** [BORA00003962] [MT7637]
** [Wifi] Add MAC init offload
**
** 02 14 2015 ted.huang
** [BORA00003379] [MT7637]
** Merge 7615 FW RAM change .
**
** 02 11 2015 chiahsuan.chuang
** [BORA00003351] [General]
** [PSE] Fix build error in w=1
**
** 02 10 2015 chiahsuan.chuang
** [BORA00003351] [General]
** [PSE] Update PSE allocation to pse_buffer_size_20140920.
**
** 09 15 2014 ted.huang
** [BORA00002807] [MT7636]
** merge MT7636 back main trunk.
**
** 07 15 2014 tiger.hung
** [BORA00003241] [MT7636] sync MT7636 E1 MP source code
**
** 05 14 2014 pierre.chang
** [BORA00003110] [MT7637] Setup for MT7637 and Andes 968
** Add 7637 project compile option and Andes 968.
**
** 03 10 2014 chiahsuan.chuang
** [BORA00002977] [MT7636][WiFi]
** . replace NIC_WIFI_REC_INTR_STATUS() with a function, not a macro.
**
** 02 11 2014 chiahsuan.chuang
** [BORA00002977] [MT7636][WiFi]
** .Update 7636 PSE buffer arrangement.
**
** 02 05 2014 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Add SDIO Feature Option
**
** 02 05 2014 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Add MT7636 PSE Memory Size
**
** 01 11 2014 ted.huang
** [BORA00002807] [MT7636]
** Support extension inband command.
**
** 01 08 2014 ted.huang
** [BORA00002807] [MT7636]
** Merge 6630E2 RAM code.
**
*/

#ifndef _NIC_H
#define _NIC_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*--------------------------------------------------------------------
 *    PSE related definition
 *--------------------------------------------------------------------
 */
/* Define PSE page size and its Index: Total buffer size */
#if (PRODUCT_VERSION == 6630)
#define NIC_PSE_SZ                      (133*1024)
#elif (PRODUCT_VERSION == 7636)
#define NIC_PSE_SZ                      (160*1024)
#elif (PRODUCT_VERSION == 7637) 
#define NIC_PSE_SZ                      (228*1024)
#else
#define NIC_PSE_SZ                      (160*1024)
#endif

#define NIC_PSE_PAGE_SZ                 PSE_PAGE_LEN_128B_SIZE
#define NIC_PSE_PAGE_IDX                PSE_PAGE_LEN_128B_IDX

#define NIC_WTBL1_ENTRY_SZ              (5*4)
#define NIC_WTBL2_ENTRY_SZ              (16*4)
#define NIC_WTBL3_ENTRY_SZ              (16*4)
#define NIC_WTBL4_ENTRY_SZ              (8*4)

#define NIC_WTBL_PAGE_NUM \
        (((MAX_WTBL_ENTRY_NUM  + ((NIC_PSE_PAGE_SZ / NIC_WTBL2_ENTRY_SZ) -1)) / \
          (NIC_PSE_PAGE_SZ / NIC_WTBL2_ENTRY_SZ)) + \
         ((MAX_WTBL_ENTRY_NUM  + ((NIC_PSE_PAGE_SZ / NIC_WTBL3_ENTRY_SZ) -1)) / \
          (NIC_PSE_PAGE_SZ / NIC_WTBL3_ENTRY_SZ)) + \
         ((MAX_WTBL_ENTRY_NUM  + ((NIC_PSE_PAGE_SZ / NIC_WTBL4_ENTRY_SZ) -1)) / \
          (NIC_PSE_PAGE_SZ / NIC_WTBL4_ENTRY_SZ)))

#define NIC_BCN_MAX_SZ                  (1*1024)
#define NIC_BCN_NUM                     2
#define NIC_BCN_PSE_PAGE_NUM            ((NIC_BCN_NUM * NIC_BCN_MAX_SZ)/NIC_PSE_PAGE_SZ)

#if (PRODUCT_VERSION == 7636 || PRODUCT_VERSION == 7637)
#define NIC_CMDBT_PSESRAM_SZ            8192    //8K Bytes
#elif (PRODUCT_VERSION == 6630)
#define NIC_CMDBT_PSESRAM_SZ            5120    //Bytes
#else
#define NIC_CMDBT_PSESRAM_SZ            8192    //8K Bytes
#endif

#define NIC_PAGE_NUM_TOTOAL             (NIC_PSE_SZ / NIC_PSE_PAGE_SZ)

/* TX/RX typical max length is calculated by MS native 802.11 format */
#define NIC_PAGE_NUM_FOR_TX_PKT         ((NIC_TX_DESC_AND_PADDING_LENGTH + NIC_TX_DESC_HEADER_PADDING_LENGTH + 1532 + NIC_PSE_PAGE_SZ - 1) / NIC_PSE_PAGE_SZ)
#define NIC_PAGE_NUM_FOR_RX_PKT         ((20*4 + 1532 + NIC_PSE_PAGE_SZ - 1) / NIC_PSE_PAGE_SZ)

/*
 * Define max aggregated number of TX status.
 * Max vaule is 24 for MT6630 and min value is 1 by HW spec
 */
#define NIC_TX_STS_MAX_AGG_NUM          24

#if (PRODUCT_VERSION == 7636)
/* Page number for HIF TX (based on A-MPDU length = 20) */
#define TX_BUFFER_PAGE_CNT              ((20*2+2*5) * NIC_PAGE_NUM_FOR_TX_PKT)
#define MCU_CMD_BUFFER_PAGE_CNT         ((0 + NIC_PSE_PAGE_SZ - 1)/NIC_PSE_PAGE_SZ) // 0: SDIO driver has already make this reservation in TX BUFFER PAGE_CNT
#define BC_MC_MANAGE_FRAME_PAGE_CNT     (2 * NIC_PAGE_NUM_FOR_TX_PKT)
#define NIC_PSE_P0_MIN                  (TX_BUFFER_PAGE_CNT + MCU_CMD_BUFFER_PAGE_CNT + BC_MC_MANAGE_FRAME_PAGE_CNT)
#elif (PRODUCT_VERSION == 7637)
/* Page number for HIF TX (based on A-MPDU length = 45 for USB and SDIO, A-MPDU length = 29 for SDIO) */
#define TX_BUFFER_PAGE_CNT_USB_MAX      ((45*2+2*5) * NIC_PAGE_NUM_FOR_TX_PKT)
#define TX_BUFFER_PAGE_CNT_USB_MIN      ((16*2+2*5) * NIC_PAGE_NUM_FOR_TX_PKT)

#define TX_BUFFER_PAGE_CNT_PCIE         ((35*2+2*5) * NIC_PAGE_NUM_FOR_TX_PKT)
#define TX_BUFFER_PAGE_CNT_SDIO         ((29*2+2*5) * NIC_PAGE_NUM_FOR_TX_PKT)

#define MCU_CMD_BUFFER_PAGE_CNT         ((0 + NIC_PSE_PAGE_SZ - 1)/NIC_PSE_PAGE_SZ) // 0: SDIO driver has already make this reservation in TX BUFFER PAGE_CNT
#define BC_MC_MANAGE_FRAME_PAGE_CNT     (2 * NIC_PAGE_NUM_FOR_TX_PKT)

#define NIC_PSE_P0_MIN_SDIO                  (TX_BUFFER_PAGE_CNT_SDIO + MCU_CMD_BUFFER_PAGE_CNT + BC_MC_MANAGE_FRAME_PAGE_CNT)
#define NIC_PSE_P0_MIN_USB                  (TX_BUFFER_PAGE_CNT_USB_MIN + MCU_CMD_BUFFER_PAGE_CNT + BC_MC_MANAGE_FRAME_PAGE_CNT)
//#define NIC_PSE_P0_MAX_USB                  (TX_BUFFER_PAGE_CNT_USB_MAX + MCU_CMD_BUFFER_PAGE_CNT + BC_MC_MANAGE_FRAME_PAGE_CNT)

#define NIC_PSE_P0_MIN_PCIE                  (TX_BUFFER_PAGE_CNT_PCIE + MCU_CMD_BUFFER_PAGE_CNT + BC_MC_MANAGE_FRAME_PAGE_CNT)

#define NIC_PSE_P0_MIN  NIC_PSE_P0_MIN_USB
#elif (PRODUCT_VERSION == 6630)
/* Page number for HIF TX (based on A-MPDU length = 16) */
#define NIC_PSE_P0_MIN                  ((16*2+2*5) * NIC_PAGE_NUM_FOR_TX_PKT)
#else
/* Page number for HIF TX (based on A-MPDU length = 20) */
#define TX_BUFFER_PAGE_CNT              ((20*2+2*5) * NIC_PAGE_NUM_FOR_TX_PKT)
#define MCU_CMD_BUFFER_PAGE_CNT         ((0 + NIC_PSE_PAGE_SZ - 1)/NIC_PSE_PAGE_SZ) // 0: SDIO driver has already make this reservation in TX BUFFER PAGE_CNT
#define BC_MC_MANAGE_FRAME_PAGE_CNT     (2 * NIC_PAGE_NUM_FOR_TX_PKT)
#define NIC_PSE_P0_MIN                  (TX_BUFFER_PAGE_CNT + MCU_CMD_BUFFER_PAGE_CNT + BC_MC_MANAGE_FRAME_PAGE_CNT)
#endif
#if (PRODUCT_VERSION == 7637)
//#define NIC_PSE_P0_MAX_USB     NIC_PSE_P0_MIN_USB
#define NIC_PSE_P0_MAX_USB    (TX_BUFFER_PAGE_CNT_USB_MAX + MCU_CMD_BUFFER_PAGE_CNT + BC_MC_MANAGE_FRAME_PAGE_CNT)

#define NIC_PSE_P0_MAX_PCIE    NIC_PSE_P0_MIN_PCIE
#define NIC_PSE_P0_MAX_SDIO    NIC_PSE_P0_MIN_SDIO
//#define NIC_PSE_P0_MAX         NIC_PSE_P0_MIN_USB
#define NIC_PSE_P0_MAX         NIC_PSE_P0_MAX_USB

#elif ((PRODUCT_VERSION == 6630) || (PRODUCT_VERSION == 7636))
#define NIC_PSE_P0_MAX                  NIC_PSE_P0_MIN
#else
#define NIC_PSE_P0_MAX                  NIC_PSE_P0_MIN
#endif

#if ((PRODUCT_VERSION == 7636) || (PRODUCT_VERSION == 7637))
/* Page number for MCU event/mgt buffer + beacon offload buffer */
#define BC_MC_MANAGE_FRAME_2K_PAGE_CNT  ((2*1024 + NIC_PSE_PAGE_SZ - 1)/NIC_PSE_PAGE_SZ) // 2K Bytes
#define MCU_EVENT_BUFFER_PAGE_CNT       ((256*3 + NIC_PSE_PAGE_SZ - 1)/NIC_PSE_PAGE_SZ)      // 256*3 Bytes
#define NIC_PSE_P1_SW_MIN               (BC_MC_MANAGE_FRAME_2K_PAGE_CNT + MCU_EVENT_BUFFER_PAGE_CNT + NIC_BCN_PSE_PAGE_NUM)
#elif (PRODUCT_VERSION == 6630)
/* Page number for MCU event/mgt buffer */
#define NIC_PSE_P1_SW_MIN               ((2*1024) / NIC_PSE_PAGE_SZ)
#else
/* Page number for MCU event/mgt buffer */
#define BC_MC_MANAGE_FRAME_2K_PAGE_CNT  ((2*1024 + NIC_PSE_PAGE_SZ - 1)/NIC_PSE_PAGE_SZ) // 2K Bytes
#define MCU_EVENT_BUFFER_PAGE_CNT       ((256*3 + NIC_PSE_PAGE_SZ - 1)/NIC_PSE_PAGE_SZ)      // 256*3 Bytes
#define NIC_PSE_P1_SW_MIN               (BC_MC_MANAGE_FRAME_2K_PAGE_CNT + MCU_EVENT_BUFFER_PAGE_CNT)
#endif
#if (BUILD_FOR_PATCH == 5)
#include "patch_config.h"
#if (MT7687_WCNCR00047582 == 1)
#define NIC_PSE_P1_SW_MAX               ((32*1024) / NIC_PSE_PAGE_SZ)
#else
#define NIC_PSE_P1_SW_MAX               ((8*1024) / NIC_PSE_PAGE_SZ)
#endif
#else
#define NIC_PSE_P1_SW_MAX               ((8*1024) / NIC_PSE_PAGE_SZ)
#endif
/* Page number for LMAC RX Data+DRFB, RX vector and TX status */
#if (PRODUCT_VERSION == 7636)
#define NIC_PSE_P2_DATA_MIN             ((20+10) * NIC_PAGE_NUM_FOR_RX_PKT) //SDIO: 48.75K Bytes
#define NIC_PSE_P2_DATA_MIN_USB         (20 * NIC_PAGE_NUM_FOR_RX_PKT) //USB: 32.5K Bytes
#elif (PRODUCT_VERSION == 7637)
#define NIC_PSE_P2_DATA_MIN_SDIO        ((29+15) * NIC_PAGE_NUM_FOR_RX_PKT) //SDIO: 48.75K Bytes
#define NIC_PSE_P2_DATA_MIN_USB         (24 * NIC_PAGE_NUM_FOR_RX_PKT)  //USB: 32.5K Bytes
//#define NIC_PSE_P2_DATA_MIN_USB         (4 * NIC_PAGE_NUM_FOR_RX_PKT)  //USB: 32.5K Bytes
#define NIC_PSE_P2_DATA_MIN_PCIE        (24 * NIC_PAGE_NUM_FOR_RX_PKT) //PCIE: 32.5K Bytes

// choose USB case as P2 MIN/MAX to PSE buffer check
#define NIC_PSE_P2_DATA_MIN             NIC_PSE_P2_DATA_MIN_USB
#elif (PRODUCT_VERSION == 6630)
#define NIC_PSE_P2_DATA_MIN            (16 * NIC_PAGE_NUM_FOR_RX_PKT + ((20*4 + NIC_PSE_PAGE_SZ - 1) / NIC_PSE_PAGE_SZ) * 2)
#else
#define NIC_PSE_P2_DATA_MIN             ((20+10) * NIC_PAGE_NUM_FOR_RX_PKT) //SDIO: 48.75K Bytes
#define NIC_PSE_P2_DATA_MIN_USB         (20 * NIC_PAGE_NUM_FOR_RX_PKT) //USB: 32.5K Bytes
#endif

#define NIC_PSE_P2_DATA_MAX             PSE_MAX_FRAME_ID_RANGE

#if (PRODUCT_VERSION == 7636 || PRODUCT_VERSION == 7637)
#define NIC_PSE_P2_VECTOR_MIN           ((3*1024 + NIC_PSE_PAGE_SZ - 1)/NIC_PSE_PAGE_SZ) // 3K Bytes
#elif (PRODUCT_VERSION == 6630)
#define NIC_PSE_P2_VECTOR_MIN           0       /* 0=Disabled */
#else
#define NIC_PSE_P2_VECTOR_MIN           ((3*1024 + NIC_PSE_PAGE_SZ - 1)/NIC_PSE_PAGE_SZ) // 3K Bytes
#endif

#define NIC_PSE_P2_VECTOR_MAX           PSE_MAX_FRAME_ID_RANGE

#if (PRODUCT_VERSION == 7636 || PRODUCT_VERSION == 7637)
#define NIC_PSE_P2_STS_MIN              ((2*1024 + NIC_PSE_PAGE_SZ - 1)/NIC_PSE_PAGE_SZ) // 2K Bytes
#elif (PRODUCT_VERSION == 6630)
#define NIC_PSE_P2_STS_MIN              ((5*4*NIC_TX_STS_MAX_AGG_NUM + NIC_PSE_PAGE_SZ -1) / NIC_PSE_PAGE_SZ * 2)
#else
#define NIC_PSE_P2_STS_MIN              ((2*1024 + NIC_PSE_PAGE_SZ - 1)/NIC_PSE_PAGE_SZ) // 2K Bytes
#endif
#define NIC_PSE_P2_STS_MAX              PSE_MAX_FRAME_ID_RANGE


/*
 * Todo: find real WTBL/BCN/RADAR/BACKUP size to calculate
 * NIC_PAGE_NUM_SHARED 20130227
 */
#if ((PRODUCT_VERSION == 6630) || (PRODUCT_VERSION == 7636))
#define NIC_PAGE_NUM_SHARED             (NIC_PAGE_NUM_TOTOAL - NIC_WTBL_PAGE_NUM - \
                                        ((NIC_CMDBT_PSESRAM_SZ + NIC_PSE_PAGE_SZ -1) / NIC_PSE_PAGE_SZ) - \
                                        ((NIC_BCN_MAX_SZ +NIC_PSE_PAGE_SZ-1) / NIC_PSE_PAGE_SZ * NIC_BCN_NUM))
#elif (PRODUCT_VERSION == 7637)
#define NIC_PAGE_NUM_SHARED             (NIC_PAGE_NUM_TOTOAL - NIC_WTBL_PAGE_NUM - \
                                        ((NIC_CMDBT_PSESRAM_SZ + NIC_PSE_PAGE_SZ -1) / NIC_PSE_PAGE_SZ))
#else
#define NIC_PAGE_NUM_SHARED             (NIC_PAGE_NUM_TOTOAL - NIC_WTBL_PAGE_NUM - \
                                        ((NIC_CMDBT_PSESRAM_SZ + NIC_PSE_PAGE_SZ -1) / NIC_PSE_PAGE_SZ) - \
                                        ((NIC_BCN_MAX_SZ +NIC_PSE_PAGE_SZ-1) / NIC_PSE_PAGE_SZ * NIC_BCN_NUM))
#endif

#if NIC_PAGE_NUM_SHARED < (NIC_PSE_P0_MIN + NIC_PSE_P1_SW_MIN + \
                           NIC_PSE_P2_DATA_MIN + NIC_PSE_P2_VECTOR_MIN + \
                           NIC_PSE_P2_STS_MIN)
//#error Min reserved PSE page number of all ports is too large !!
#endif


/* Define default IER of LMAC and PSE */
#define DEFAULT_WIE0R_SETTING           0
#define DEFAULT_WIE1R_SETTING           0
#define DEFAULT_WIE2R_SETTING           0
#define DEFAULT_WIE3R_SETTING           0

#define DEFAULT_PSE_IER_SETTING         (PSE_INT_STS_P1Q0_NE | PSE_INT_STS_P1Q1_NE | PSE_INT_STS_P1Q2_NE | PSE_INT_STS_P1Q3_NE)


/* Define CPU queue type */
#define NIC_HIF_TO_CPU_CMD_QUE          ENUM_CTX_Q_0
#define NIC_HIF_TO_CPU_PKT_QUE          ENUM_CTX_Q_1

#if (PRODUCT_VERSION == 6630)
#define NIC_RDD_SW_BUFFER_SIZE          0
#define NIC_RDD_HW_BUFFER_SIZE          0
#elif (PRODUCT_VERSION == 7636 || PRODUCT_VERSION == 7637)
#define NIC_RDD_SW_BUFFER_SIZE          512
#define NIC_RDD_HW_BUFFER_SIZE          512
#else
#define NIC_RDD_SW_BUFFER_SIZE          512
#define NIC_RDD_HW_BUFFER_SIZE          512
#endif
/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
/*
 * Number of Interrupt status registers
 * Mapping with g_arIntrIndexToRegs[], defined in nic.c
 */
typedef enum _ENUM_INTR_IDX_T
{
#if (CFG_SDIO_SUPPORTED_WIFI == 1)  //Posh 20140205, Add SDIO feature option
    INTR_IDX_HWFISR,        /* under HIFSYS */
#endif

    INTR_IDX_WIS0R,         /* under WIFISYS */
    INTR_IDX_WIS1R,         /* under WIFISYS */
    INTR_IDX_WIS2R,         /* under WIFISYS */
//  INTR_IDX_WIS3R,         /* under WIFISYS */
    INTR_IDX_PSE,           /* under PSE */
    INTR_IDX_NUM
} ENUM_INTR_T, *P_ENUM_INTR_T;

typedef struct _INTR_STS_MASK_REG_T
{
    UINT_32             u4StsReg;   /* Interrupt status register address */
    UINT_32             u4MaskReg;  /* Interrupt enable register address */
} INTR_STS_MASK_REG_T, *P_INTR_STS_MASK_REG_T;


/* ISR event handle except TX/RX done */
typedef struct _INT_EVENT_MAP_T
{
    UINT_32             u4RegIdx;   /* Index in ENUM_INTR_T */
    UINT_32             u4IntBit;   /* Bit defintion of interrupt status */
    IST_EVENT_FUNCTION  pfEventHandle;
} INT_EVENT_MAP_T, *P_INT_EVENT_MAP_T;

#if 0   /* Marked for MT6630 */
enum ENUM_INT_EVENT_T
{
    INT_EVENT_RX1_UNDERFLOW,
    INT_EVENT_RX0_UNDERFLOW,
    INT_EVENT_TX1_OVERFLOW,
    INT_EVENT_TX0_OVERFLOW,
    INT_EVENT_DRV_CLR_FW_OWN,
    INT_EVENT_DRV_SET_FW_OWN,
    INT_EVENT_RXQ1_EMPTY,
    INT_EVENT_RXQ0_EMPTY,
    INT_EVENT_TXQ1_FULL,
    INT_EVENT_TXQ0_FULL,
    INT_EVENT_MAC_TX_DONE,
    INT_EVENT_MAC_RX_DONE,
    INT_EVENT_HIF_TX_DONE,
    INT_EVENT_HIF_RX_DONE,

    INT_EVENT_ABNORMAL,
    INT_EVENT_WATCH_DOG,
    INT_EVENT_TSF_DRIFT,
    INT_EVENT_SCAN_DONE,
    INT_EVENT_T0_TIMEUP,
    INT_EVENT_T1_TIMEUP,
    INT_EVENT_T2_TIMEUP,
    INT_EVENT_T3_TIMEUP,
    INT_EVENT_ADMIT_TIME_MET,
    INT_EVENT_BEACON_TIMEOUT,
    INT_EVENT_HW_TX_FAIL,
    INT_EVENT_BMC_TIMEOUT,
    INT_EVENT_APSD_TIMEOUT,
    INT_EVENT_PSPOLL_TIMEOUT,
    INT_EVENT_TBTT,
    INT_EVENT_BCN_TR_OK,
    INT_EVENT_ATIM,
    INT_EVENT_RCPI,
    INT_EVENT_ALC,
    INT_EVENT_NUM
};

enum ENUM_WIFI_EVENT_T
{
    WIFI_EVENT_FW_OWN,
    WIFI_EVENT_DRIVER_OWN,
    WIFI_EVENT_IST,
    WIFI_EVENT_SDIO_CLR_WLAN_IOE
};
#endif

typedef enum _ENUM_IE_UPD_METHOD_T
{
    IE_UPD_METHOD_UPDATE_RANDOM,
    IE_UPD_METHOD_UPDATE_ALL,
    IE_UPD_METHOD_DELETE_ALL
} ENUM_IE_UPD_METHOD_T, *P_ENUM_IE_UPD_METHOD_T;

#if 0
#define WIFI_EVENT_BIT_FW_OWN               BIT(WIFI_EVENT_FW_OWN)
#define WIFI_EVENT_BIT_DRIVER_OWN           BIT(WIFI_EVENT_DRIVER_OWN)
#define WIFI_EVENT_BIT_IST                  BIT(WIFI_EVENT_IST)
#define WIFI_EVENT_BIT_SDIO_CLR_WLAN_IOE    BIT(WIFI_EVENT_SDIO_CLR_WLAN_IOE)
#endif

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
extern const INTR_STS_MASK_REG_T g_arIntrIndexToRegs[INTR_IDX_NUM];

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
//Chiahsuan: use a function, instead a macro.
#define NIC_WIFI_REC_INTR_STATUS(_au4IntStatus, _au4IntEnable) nicWifiRecIntrStatus(_au4IntStatus, _au4IntEnable)

#define NIC_WIFI_CLR_HW_ISR(_intrIdx, _intBits) \
    { \
        HAL_REG_32(g_arIntrIndexToRegs[_intrIdx].u4StsReg) = (_intBits); \
    }

#define NIC_WIFI_CLR_SW_ISR(_au4IntStatus, _intrIdx, _intBits) \
    { \
        (_au4IntStatus)[_intrIdx] &= ~(_intBits); \
    }

#define NIC_WIFI_ACK_INTR_STATUS(_au4IntStatus, _intrIdx, _intBits) \
    { \
        NIC_WIFI_CLR_HW_ISR(_intrIdx, _intBits); \
        NIC_WIFI_CLR_SW_ISR(_au4IntStatus, _intrIdx, _intBits); \
    }

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
#if 0 //IOT_PROCESS_PORTING
VOID
nicPseInit(
    ENUM_FWOP_STATE_T eTestMode
    );

VOID
nicPseWtblInit(
    ENUM_FWOP_STATE_T eTestMode
    );
#endif
VOID
nicHifReset(
    VOID
    );

VOID
nicProcessIST(
    VOID
    );

VOID
nicProcessHifTxDoneInterrupt(
    ENUM_PSE_CPU_P_QUEUE_T eCqID
    );

VOID
nicProcessMacTxDoneInterrupt(
    ENUM_PSE_CPU_P_QUEUE_T eCqID
    );

VOID
nicSyncBeacon(
    P_BSS_INFO_T prBssInfo
    );

VOID
nicEnableBeaconTimeout(
    P_BSS_INFO_T prBssInfo
    );

VOID
nicDisableBeaconTimeout(
    P_BSS_INFO_T prBssInfo
    );

VOID
nicProcessTsfDriftInterrupt(
    UINT_8 ucIntRegIdx,
    UINT_32 u4IntStatus
    );

VOID
nicProcessBeaconTimeoutInterrupt(
    UINT_8 ucIntRegIdx,
    UINT_32 u4IntStatus
    );

VOID
nicStartBeacon(
    P_BSS_INFO_T prBssInfo,
    BOOLEAN fgResetTsf
    );

VOID
nicStopBeacon(
    P_BSS_INFO_T prBssInfo
    );

VOID
nicProcessBeaconTxOkInterrupt(
    UINT_8          ucIntRegIdx,
    UINT_32         u4IntStatus
    );

VOID
nicPreTBTTInterruptForIbss(
    P_BSS_INFO_T    prBssInfo
    );

VOID
nicPreTBTTInterruptForAP(
    P_BSS_INFO_T    prBssInfo
    );

VOID
nicProcessPreTBTTInterrupt(
    UINT_8          ucIntRegIdx,
    UINT_32         u4IntStatus
    );

VOID
nicProcessPreTTTTInterrupt(
    UINT_8          ucIntRegIdx,
    UINT_32         u4IntStatus
    );

VOID
nicProcessTxTrapInterrupt(
    UINT_8          ucIntRegIdx,
    UINT_32         u4IntStatus
    );

VOID
nicProcessTBTTInterrupt(
    UINT_8          ucIntRegIdx,
    UINT_32         u4IntStatus
    );

VOID
nicProcessTimerInterrupt(
    UINT_8          ucIntRegIdx,
    UINT_32         u4IntStatus
    );

#if 0 //IOT_PROCESS_PORTING
VOID
nicPmInitParam(
    P_HAL_PWR_MGT_CTRL_T prPwrMgtCtrl
    );
#endif
HAL_STATUS
nicMacRxReturnBuffers(
    IN P_SW_RFB_T prSwRfbListHead
    );

VOID
nicWifiRecIntrStatus(
    UINT_32 *_au4IntStatus,
    UINT_32 *_au4IntEnable
    );

#if (WIFI_BUILD_OPTION != 1)
VOID
nicHifTxInitDma(
    BOOLEAN fgRfTestMode
    );

VOID
nicMacRxInitDma(
    BOOLEAN fgRfTestMode
    );
#endif

#if (CFG_BCN_OFFLOAD_SUPPORT == 1)
VOID
nicPreTBTTInterruptForBcnUpdate(
    UINT_8 ucHwBssidIdx
    );

VOID
nicPreTTTTInterruptForTimUpdate(
    UINT_8 ucHwBssidIdx
    );
#endif

VOID
halPseSetFlowCtrlMaxQuotaMinRsrvSet_P0 (
    VOID
    );

VOID
halPseSetFlowCtrlMaxQuotaMinRsrvSet_P2Q0 (
    VOID
    );


UINT_32
halPseGetFlowCtrlMinRsrvPageCount_P0 (
    VOID
    );

UINT_32
halPseGetFlowCtrlMinRsrvPageCount_P2Q0 (
    VOID
    );

#if (CFG_MAC_INIT_SUPPORT == 1)
VOID
nicSecInit(
    VOID
    );
#endif

#endif  /* _NIC_H */

