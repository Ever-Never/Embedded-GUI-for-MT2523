/*
** $Id: //Department/DaVinci/TRUNK/MT6620_WiFi_Firmware/mcu/wifi/include/nic/nic_dma.h#11 $
*/

/*! \file   nic_dma.h
    \brief  HAL-related macros and function prototypes for enabling HIF/MAC TX/RX DMA

    In this file, macros and function prototypes are defined for HAL to manipulate MSDU_INFOs and
    SW_RFBs or send indications for HIF/MAC TX/RX DMA operations.
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
** $Log: nic_dma.h $
**
** 09 15 2014 ted.huang
** [BORA00002807] [MT7636]
** merge MT7636 back main trunk.
**
** 07 15 2014 tiger.hung
** [BORA00003241] [MT7636] sync MT7636 E1 MP source code
**
** 03 07 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** ADD TX done handling
**
** 03 05 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** Unify TX Q index definition
**
** 01 15 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** Update txmSendHifDataPackets
**
** 01 07 2013 tsaiyuan.hsu
** [BORA00002222] MT6630 unified MAC RXM
** add defrag.
**
** 12 12 2012 tsaiyuan.hsu
** [BORA00002222]
** [MT6630] initialize nicMacRxIndicateRxDone for RX packets.
**
** 10 22 2012 cp.wu
** [BORA00002220] [MT6630 Wi-Fi] Draft FW for HIFSYS update and ROM code logic regarding HIFSYS access
** fix for emulation build success
**
** 10 17 2012 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** Marked by avoid 6630 normal fw build error.
**
** 10 09 2012 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** RAM code include ROM code symbol(gorm_ref.sym)
**
** 10 09 2012 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Establish HIF CMD path
**
** 10 04 2012 yarco.yang
** [BORA00002189] [MT6630]Synch. Davinc 20120927 FW to Bora
** .
 *
 * 10 25 2011 puff.wen
 * NULL
 * Mark nicHifTxInitDma and nicMacRxInitDma with macro #if (WIFI_BUILD_OPTION != 1)
 *
 * 10 20 2011 cm.chang
 * NULL
 * Remove compiling warning in hem.c
 *
 * 10 14 2011 tsaiyuan.hsu
 * [WCXRP00001043] [MT6628 Wi-Fi][FW] Add security PN
 * add security PN support.
 *
 * 08 29 2011 cp.wu
 * [WCXRP00000872] [MT6628][Wi-Fi] Modify HIF HAL to get rid off dependency on non-ROM/non-HAL symbols for ROM HAL
 * move 4 functions from HIF HAL to NIC due to their characteristics are more like a NIC function instead of a HAL one
 *
 * 08 27 2011 puff.wen
 * NULL
 * Move nic related macro from HAL_ROM.H to NIC_DMA.H
 *
 * 08 27 2011 puff.wen
 * NULL
 * rename halMacRxReturnBuffers to nicMacRxReturnBuffers , halProcessRxQueuesReset to nicProcessRxQueuesReset, halProcessRxDataDone to nicProcessRxDataDone and move them to nic.c
 *
 * 08 11 2011 puff.wen
 * [WCXRP00000924] [MT6628 Wi-Fi][FW] Code refinement for ROM / RAM module dependency
 * [MT6628 Wi-Fi][FW] Code refinement for ROM/ RAM module dependency
 *
 * 08 01 2011 cp.wu
 * [WCXRP00000891] [MT5931][MCUSYS] move code to SYSRAM to enable ASSERT_DUMP
 * reduce assert usage by
 * 1. eliminate unused assert
 * 2. assert combine
 * 3. replace some assert by lite_assert
 *
 * 08 09 2010 cm.chang
 * NULL
 * Support DPD and other new RF test functions based on
 * 1. PHY_programming_guide 20100803.xlsx
 * 2. RFAFE_programming_guide 20100803.xlsx
 *
 * 07 12 2010 cp.wu
 *
 * commit Wi-Fi firmware part for MT6620
 *
 * 04 29 2010 tehuang.liu
 * [BORA00000605][WIFISYS] Phase3 Integration
 * Fixed macros for INT handling
 *
 * 04 01 2010 cm.chang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * Modify MFB code to send response frame
 *
 * 02 08 2010 cp.wu
 * [BORA00000368]Integrate HIF part into BORA
 * 1) Wi-Fi Regions are no longer initialized by cos part but by wifi part itself
 *  *  *  * 2) change common hif test code due to now rx buffer size is larger than tx buffer size
 *  *  *  * 3) correct NIC_HIF_RX_GET_FLAGS_I_UT() macro
 *
 * Dec 7 2009 MTK02468
 * [BORA00000337] To check in codes for FPGA emulation
 * Refined macros
 *
 * Nov 25 2009 mtk02468
 * [BORA00000337] To check in codes for FPGA emulation
 * Fixed HIF RX HEADER manipulating macros
 *
 * Nov 24 2009 mtk02468
 * [BORA00000337] To check in codes for FPGA emulation
 * Fixed HIF TX HDR macros
 *
 * Nov 24 2009 mtk02752
 * [BORA00000368] Integrate HIF part into BORA
 * eliminate NIC_HIF_RX_HEADER_T when SD1_SD3_DATAPATH_INTEGRATION is set to 1
 *
 * Nov 20 2009 mtk02468
 * [BORA00000337] To check in codes for FPGA emulation
 * Fixed indenting
 *
 * Nov 20 2009 mtk02468
 * [BORA00000337] To check in codes for FPGA emulation
 * Modified NIC_HIF_RX_* macros to use the correct HIF_RX_HEADER
 *
 * Nov 13 2009 mtk02468
 * [BORA00000337] To check in codes for FPGA emulation
 * Removed ENUM_MAC_TX_QUEUE_INDEX_T and added HIF_TX_MAC_HEADER_TRANSLATION_BUFFER_OFFSET
 *
 * Nov 5 2009 mtk02752
 * [BORA00000368] Integrate HIF part into BORA
 * revert
 * (the first 40 bytes of RX buffer is reserved for FW processing, while HIF HW will write TX data from 41th byte by descriptor pointer)
 *
 * Nov 5 2009 mtk02752
 * [BORA00000368] Integrate HIF part into BORA
 * add HIF_TX_LOOPBACK handling
 *
 * Nov 4 2009 mtk01084
 * [BORA00000018] Integrate WIFI part into BORA for the 1st time
 * modify NIC_HIF_TX_HEADER_T data structure
 *
 * Nov 4 2009 mtk02752
 * [BORA00000368] Integrate HIF part into BORA
 * fix following macros:
 *  NIC_HIF_RX_GET_FLAGS_I_UT()
 *  NIC_HIF_RX_GET_FLAG_I()
 *  NIC_HIF_RX_GET_FLAG_UT()
 *
 *
 * Oct 30 2009 mtk01084
 * [BORA00000018] Integrate WIFI part into BORA for the 1st time
 * modify SW packet format
 *
 * Oct 23 2009 mtk02468
 * [BORA00000337] To check in codes for FPGA emulation
 * Fixed HIF RX macros: NIC_HIF_RX_HEADER_T --> P_NIC_HIF_RX_HEADER_T
 *
 * Oct 21 2009 mtk02468
 * [BORA00000337] To check in codes for FPGA emulation
 * Replaced tabs with spaces in compliance with the coding style
 *
 * Oct 16 2009 mtk02468
 * [BORA00000337] To check in codes for FPGA emulation
 * Added macros for HAL to obtain info in HIF RX header
 * Added functions for MAC/HIF TX/RX DMA initialization
 *
 * Oct 16 2009 mtk01725
 * [BORA00000343] [MT6620] Emulation For TX
 *
 *
 * Oct 12 2009 mtk02468
 * [BORA00000337] To check in codes for FPGA emulation
 * Used _SW_RX_VECTOR_DESC_T defined in hal.h
 *
 * Oct 9 2009 mtk02468
 * [BORA00000337] To check in codes for FPGA emulation
 *
 *
**
*/

#ifndef _NIC_DMA_H_
#define _NIC_DMA_H_


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
#if 0
/* The initial values of TC counters for Port 0 and Port 1.
*   TC0~3: AC0~3, TC4: BMCAST, aggregate = CFG_NUM_OF_TX0_HIF_DESC
*   TC5: for TX Port1 = CFG_NUM_OF_TX1_HIF_DESC
*/
#define HIF_TX_DMA_TC0_BUF_NUM    4
#define HIF_TX_DMA_TC1_BUF_NUM    8
#define HIF_TX_DMA_TC2_BUF_NUM    8
#define HIF_TX_DMA_TC3_BUF_NUM    8
#define HIF_TX_DMA_TC4_BUF_NUM    4
#define HIF_TX_DMA_TC5_BUF_NUM    2
#endif

/* TXM shall specify the TC counter that HAL should increment upon receipt of a returned buffer for HIF TX
*  DMA. The tag is included in MSDU_INFO. TRAFFIC_CLASS_TAG_NULL is used for initialization only.
*  TX0~TC3, TC5: HIF TX0, TC4: 802.1x or command packet (HIF TX1)
*/
#define TRAFFIC_CLASS_TAG_TC0       0
#define TRAFFIC_CLASS_TAG_TC1       1
#define TRAFFIC_CLASS_TAG_TC2       2
#define TRAFFIC_CLASS_TAG_TC3       3
#define TRAFFIC_CLASS_TAG_TC4       4
#define TRAFFIC_CLASS_TAG_TC5       5
#define TRAFFIC_CLASS_TAG_NULL      0xFF

/* Size of the reserved area of the HIF TX data buffer (MSDU_INFO->pucBuffer) for header translation */
#define HIF_TX_BUFFER_RESERVED_FIELD_SIZE   0

/* The actual start address for header translation (relative to MSDU_INFO->pucBuffer) */
#define HIF_TX_MAC_HEADER_TRANSLATION_BUFFER_OFFSET  0

/* HIF RX Header Masks and Offsets*/
#define HIF_RX_HDR_FLAG_I_MASK      BIT(0)
#define HIF_RX_HDR_FLAG_UT_MASK     BIT(1)
#define HIF_RX_HDR_FLAGS_I_UT_MASK  BITS(0,1)

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

#if (SD1_SD3_DATAPATH_INTEGRATION == 0)

/* Packet info to the host driver. The first DW is consistent with the first DW of the HIF RX header. */
typedef struct _NIC_HIF_RX_HEADER_T{
    UINT_16     u2PacketLength; //in Bytes
    UINT_8      ucEetherTypeOffset; //in Words
    /* flag I: IP Checksum Check Requirement; flag UT: UDP/TCP Checksum Check Requirement */
    UINT_8      ucI_UT_Flags;
    /* TODO: Add other fields below*/
    UINT_16     u2PacketType;
    UINT_16     u2SeqNoTid;
    UINT_8      aucTA[6];
    UINT_16     u2Reserved;
} NIC_HIF_RX_HEADER_T, *P_NIC_HIF_RX_HEADER_T;

/* Packet info from the host driver. The first DW is consistent with the first DW of the HIF RX header. */
typedef struct _NIC_HIF_TX_HEADER_T{
#if 0 // align to existed driver format
    UINT_16 u2PacketLength; //in Bytes
    UINT_8 ucEetherTypeOffset; //in Words
    /* flag I: IP Checksum Check Requirement; flag UT: UDP/TCP Checksum Check Requirement */
    UINT_8 ucI_UT_Flags;
#else
    UINT_16     u2TxByteCount_UserPriority;
    UINT_8      ucEtherTypeOffset;
    UINT_8      ucResource_PktType_CSflags;
    UINT_8      ucWlanHeaderLength;
    UINT_8      ucPktFormtId_Flags;
#endif
    /* TODO: Add other fields below*/
} NIC_HIF_TX_HEADER_T, *P_NIC_HIF_TX_HEADER_T;

#endif


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
/* To concatenate two MSDU_INFOs
*   This macro is mainly used by HAL to form an MSDU_INFO list.
*/
#define NIC_HIF_TX_SET_NEXT_MSDU_INFO(_prMsduInfoPreceding, _prMsduInfoNext) \
    ((((_prMsduInfoPreceding)->rLinkEntry).prNext) = (P_LINK_ENTRY_T)(_prMsduInfoNext))

/* To obtain the next MSDU_INFO pointer given the preceding one
*   This macro is mainly used by HAL to traverse an MSDU_INFO list.
*/
#define NIC_HIF_TX_GET_NEXT_MSDU_INFO(_prMsduInfo) \
    ((P_MSDU_INFO_T)(((_prMsduInfo)->rLinkEntry).prNext))

/* This macro is mainly used by HAL when a free MSDU_INFO is returned to HIF TX. */
#define NIC_HIF_TX_GET_TX_BUF_PTR(_prMsduInfo) \
    ((((_prMsduInfo)->pucBuffer)))

/* To obtain the resource tag (TC counter index) associated with an MSDU_INFO
*   This macro is mainly used by HAL to determine which TC counter should be incremented
*   when a free MSDU_INFO (i.e., a free buffer) is returned to HIF TX.
*/
#define NIC_HIF_TX_GET_TC_INDEX(_prMsduInfo) \
    ((_prMsduInfo)->ucTC)

/* Record the TC index obtained from the HIF TX header */
#define NIC_HIF_TX_RECORD_TC(_prMsduInfo)\
{\
    ASSERT(_prMsduInfo && (_prMsduInfo)->pucBuffer);\
    ((_prMsduInfo)->ucTC) = (HIF_TX_HDR_GET_TC(\
        (P_HIF_TX_HEADER_T)(NIC_HIF_TX_GET_TX_BUF_PTR(_prMsduInfo))));\
}

/* To concatenate two SW_RFBs
*   This macro is mainly used by HAL to form a SW_RFB list.
*/
#define NIC_HIF_RX_SET_NEXT_SW_RFB(_prSwRfbPreceding, _prSwRfbNext) \
    ((((_prSwRfbPreceding)->rLinkEntry).prNext) = (P_LINK_ENTRY_T)(_prSwRfbNext))

/* To obtain the next SW_RFB pointer given the preceding one
*   This macro is mainly used by HAL to traverse a SW_RFB list.
*/
#define NIC_HIF_RX_GET_NEXT_SW_RFB(_prSwRfb) \
    ((P_SW_RFB_T)(((_prSwRfb)->rLinkEntry).prNext))

/* To obtain the pointer to the buffer for the HIF RX packet
*   This macro is mainly used by HAL when a filled SW_RFB is attached to HIF RX.
*/
#define NIC_HIF_RX_GET_RX_BUF_PTR(_prSwRfb) \
    ((_prSwRfb)->pucHifRxPacket)

/* To fill an HIF RX DMA descriptor, HAL may need the following macros to obtain needed info
*    from the HIF RX header, which is a part of the packet indicated to the host driver.
*/
#if SD1_SD3_DATAPATH_INTEGRATION

#define NIC_HIF_RX_GET_RX_BUF_LENGTH(_prSwRfb) \
    (((P_HIF_RX_HEADER_T)((_prSwRfb)->pucHifRxPacket))->u2PacketLen)

#define NIC_HIF_RX_GET_ETHER_TYPE_OFFSET(_prSwRfb) \
    ((_prSwRfb)->ucEtherTypeOffset)

#define NIC_HIF_RX_GET_FLAG_I(_prSwRfb) \
    ((_prSwRfb)->fgIPChecksumCheckRequired)

#define NIC_HIF_RX_GET_FLAG_UT(_prSwRfb) \
    ((_prSwRfb)->fgTUChecksumCheckRequired)

#define NIC_HIF_RX_GET_FLAGS_I_UT(_prSwRfb) \
    ((NIC_HIF_RX_GET_FLAG_I(_prSwRfb) ? HIF_RX_HDR_FLAG_I_MASK : 0) | \
    (NIC_HIF_RX_GET_FLAG_UT(_prSwRfb) ? HIF_RX_HDR_FLAG_UT_MASK : 0))

#else
#define NIC_HIF_RX_GET_RX_BUF_LENGTH(_prSwRfb) \
    (((P_NIC_HIF_RX_HEADER_T)((_prSwRfb)->pucHifRxPacket))->u2PacketLength)

#define NIC_HIF_RX_GET_ETHER_TYPE_OFFSET(_prSwRfb) \
    (((P_NIC_HIF_RX_HEADER_T)((_prSwRfb)->pucHifRxPacket))->ucEetherTypeOffset)

#define NIC_HIF_RX_GET_FLAGS_I_UT(_prSwRfb) \
    ((((P_NIC_HIF_RX_HEADER_T)((_prSwRfb)->pucHifRxPacket))->ucI_UT_Flags) \
    & HIF_RX_HDR_FLAGS_I_UT_MASK)

#define NIC_HIF_RX_GET_FLAG_I(_prSwRfb) \
    ((((P_NIC_HIF_RX_HEADER_T)((_prSwRfb)->pucHifRxPacket))->ucI_UT_Flags) \
    & HIF_RX_HDR_FLAG_I_MASK)

#define NIC_HIF_RX_GET_FLAG_UT(_prSwRfb) \
    (((((P_NIC_HIF_RX_HEADER_T)((_prSwRfb)->pucHifRxPacket))->ucI_UT_Flags) \
    & HIF_RX_HDR_FLAG_UT_MASK)?1:0)
#endif

/* To concatenate two SW_RFBs
*   This macro is mainly used by HAL to form a SW_RFB list.
*/
#define NIC_MAC_RX_SET_NEXT_SW_RFB(_prSwRfbPreceding, _prSwRfbNext) \
    (NIC_HIF_RX_SET_NEXT_SW_RFB((_prSwRfbPreceding), (_prSwRfbNext)))

/* To obtain the next SW_RFB pointer given the preceding one
*   This macro is mainly used by HAL to traverse a SW_RFB list.
*/
#define NIC_MAC_RX_GET_NEXT_SW_RFB(_prSwRfb) \
    (NIC_HIF_RX_GET_NEXT_SW_RFB((_prSwRfb)))

/* To obtain the pointer to the buffer for the MAC RX packet
*   This macro is mainly used by HAL when a free SW_RFB is returned to MAC RX.
*/
#define NIC_MAC_RX_GET_DATA_BUF_PTR(_prSwRfb) \
    (((_prSwRfb)->pucBuffer) + MAC_RX_BUFFER_RESERVED_FIELD_SIZE)

/* To obtain the pointer to the buffer for filling the MAC RX Status */
#define NIC_MAC_RX_GET_RX_STATUS_BUF_PTR(_prSwRfb) \
    ((_prSwRfb)->pucBuffer)

#define nicMacTxIndicationTxDone(_prTxStatus) {\
    txmHandleMacTxDone((_prTxStatus));\
}

#if 0 //IOT_PROCESS_PORTING
#define nicHifRxIndicateRxDone(_ePortIdx, _prSwRfbListHead) {\
    rxmHandleHifRxDone((_ePortIdx),(_prSwRfbListHead));\
}

#define nicMacRxIndicateRxDone(_ePortIdxd) {\
    rxmHandleMacRxDone((_ePortIdxd));\
}
#endif
/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
#if (WIFI_BUILD_OPTION != 1)
VOID
nicHifTxInitDma(
    BOOLEAN     fgRfTestMode
    );
#endif

VOID
nicMacTxInitDma(
    VOID
    );

#if (WIFI_BUILD_OPTION != 1)
VOID
nicMacRxInitDma(
    BOOLEAN     fgRfTestMode
    );
#endif

VOID
nicMacRxFreeSwRfbs(
    P_SW_RFB_T prSwRfbListHead
    );

#if 0 /* Marked by 6630 */
VOID
nicMacRxIndicateRxVts(
    P_SW_RX_VECTOR_DESC_T prSwRxVtDescListHead,
    UINT_8 ucRxVtNum
    );
#endif
#if 0 //IOT_PROCESS_PORTING
/*----------------------------------------------------------------------------*/
/* HIFSYS buffer manipulation                                                 */
/*----------------------------------------------------------------------------*/
HAL_STATUS
nicHifProcessTxDone (
    P_HAL_HIF_CTRL_T prHalHifCtrl,
    UINT_8 ucPortIdx,
    PP_MSDU_INFO_T pprFreeMsduInfo
    );

HAL_STATUS
nicHifTxDmaReturnBuffers (
    P_HAL_HIF_CTRL_T prHalHifCtrl,
    UINT_8 ucPortIdx,
    P_MSDU_INFO_T prMsduInfoListHead
    );
#endif/*IOT_PROCESS_PORTING*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif

