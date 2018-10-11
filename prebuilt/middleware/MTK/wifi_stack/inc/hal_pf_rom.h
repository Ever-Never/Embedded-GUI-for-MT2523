
/* Copyright Statement:
*
* (C) 2005-2017  MediaTek Inc. All rights reserved.
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
#ifndef _MT6630_WIFI_PF_H
#define _MT6630_WIFI_PF_H

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

/*------------------------------------------------------------------------------
 * VALUE DEFINE
 *------------------------------------------------------------------------------
*/
#define PF_BITMAP_MAX_NUM                  16
#define PF_BITMAP_CR_OFFSET                16
#define PF_BITMAP_MAX_DW_OFFSET            32
#define PF_CF_FILTER_CR_OFFSET             0x100
#define PF_CF_MASK_PATTERN_CR_OFFSET       0x10
#define PF_TDLS_MATCH_LEN                  2
#define PF_BSSID0_BIT                      BIT(0)
#define PF_BSSID1_BIT                      BIT(1)
#define PF_BSSID2_BIT                      BIT(2)
#define PF_BSSID3_BIT                      BIT(3)
#define PF_BSSID_ALL_BIT                   (PF_BSSID0_BIT |PF_BSSID1_BIT |PF_BSSID2_BIT |PF_BSSID3_BIT)

#define PF_PKT_TYPE_BIT_UC                 BIT(0)
#define PF_PKT_TYPE_BIT_MC                 BIT(1)
#define PF_PKT_TYPE_BIT_BC                 BIT(2)
#define PF_PKT_TYPE_BIT_ALL                (PF_PKT_TYPE_BIT_UC|PF_PKT_TYPE_BIT_MC|PF_PKT_TYPE_BIT_BC)
/*------------------------------------------------------------------------------
 * Bit fields for Packet Filter Global Configuration
 *------------------------------------------------------------------------------
*/
#define PF_PFCR_BSSID_EN_MASK              BITS(20,23)
#define PF_PFCR_BSSID_EN_OFFSET            20
#define PF_PFCR_PF_MODE_BIT                BIT(17)
#define PF_PFCR_PF_EN_BIT                  BIT(16)
#define PF_PFCR_MAGIC_BSSID_EN_MASK        BITS(12,15)
#define PF_PFCR_MAGIC_BSSID_EN_OFFSET      12
#define PF_PFCR_MAGIC_EN_BIT               BIT(10)
#define PF_PFCR_TDLS_EN_BIT                BIT(9)
#define PF_PFCR_EAPOL_EN_BIT               BIT(8)
#define PF_PFCR_ARP_NS_EN_BIT              BIT(2)
#define PF_PFCR_BITMAP_EN_BIT              BIT(1)
#define PF_PFCR_CF_EN_BIT                  BIT(0)

/*------------------------------------------------------------------------------
 * Bit fields for Packet Filter result register
 *------------------------------------------------------------------------------
*/
#define PF_PFRR_PF_STATUS_BIT              BIT(31)

/*------------------------------------------------------------------------------
 * Bit fields for Packet Filter IPv6 Configuration
 *------------------------------------------------------------------------------
*/
#define PF_IPV6ECR_EXT_HDR_EN_MASK         BITS(16,20)
#define PF_IPV6ECR_EXT_HDR_EN_OFFSET       16
#define PF_IPV6ECR_USER_DEF_NEXTHDR_MASK   BITS(8,15)
#define PF_IPV6ECR_USER_DEF_NEXTHDR_OFFSET 8
#define PF_IPV6ECR_USER_DEF_JUMP_DW_MASK   BITS(6,7)
#define PF_IPV6ECR_USER_DEF_JUMP_DW_OFFSET 6
#define PF_IPV6ECR_USER_DEF_JUMP_WT_MASK   BITS(4,5)
#define PF_IPV6ECR_USER_DEF_JUMP_WT_OFFSET 4
#define PF_IPV6ECR_USER_DEF_EN_BIT         BIT(0)

/*------------------------------------------------------------------------------
 * Bit fields for Packet Filter Bitmap Parameter Configuration
 *------------------------------------------------------------------------------
*/
#define PF_WBNPR_DW_OFFSET_MASK            BITS(16,20)
#define PF_WBNPR_DW_OFFSET                 16
#define PF_WBNPR_BYTEMASK_MASK             BITS(8,11)
#define PF_WBNPR_BYTEMASK_OFFSET           8
#define PF_WBNPR_BSSID_MASK                BITS(4,7)
#define PF_WBNPR_BSSID_OFFSET              4
#define PF_WBNPR_COALESE_BIT               BIT(1)
#define PF_WBNPR_EN_BIT                    BIT(0)

/*------------------------------------------------------------------------------
 * Bit fields for Packet Filter ARPNS Parameter Configuration
 *------------------------------------------------------------------------------
*/
#define PF_ANOCR_IDX3_OFFSET               16
#define PF_ANOCR_REMOTE_IPV6_EN            BIT(17)
#define PF_ANOCR_REMOTE_IPV4_EN            BIT(16)
#define PF_ANOCR_IP_VER                    BIT(12)
#define PF_ANOCR_IP_VER_OFFSET             12
#define PF_ANOCR_PACKET_TYPE_MASK          BITS(8,10)
#define PF_ANOCR_PACKET_TYPE_OFFSET        8
#define PF_ANOCR_BSSID_MASK                BITS(4,7)
#define PF_ANOCR_BSSID_OFFSET              4
#define PF_ANOCR_COALESE_BIT               BIT(1)
#define PF_ANOCR_EN_BIT                    BIT(0)

/*------------------------------------------------------------------------------
 * Bit fields for Packet Filter Coalescing Filter Parameter Configuration
 *------------------------------------------------------------------------------
*/
#define PF_CFNFPR1_FIELD_ID_1_MASK           BITS(26,31)
#define PF_CFNFPR1_FIELD_ID_1_OFFSET         26
#define PF_CFNFPR1_CHECK_LEN_1_MASK          BITS(24,25)
#define PF_CFNFPR1_CHECK_LEN_1_OFFSET        24
#define PF_CFNFPR1_OPERATION_5               BIT(20)
#define PF_CFNFPR1_OPERATION_4               BIT(19)
#define PF_CFNFPR1_OPERATION_3               BIT(18)
#define PF_CFNFPR1_OPERATION_2               BIT(17)
#define PF_CFNFPR1_OPERATION_1               BIT(16)
#define PF_CFNFPR1_PACKET_TYPE_MASK          BITS(8,10)
#define PF_CFNFPR1_PACKET_TYPE_OFFSET        8
#define PF_CFNFPR1_BSSID_MASK                BITS(4,7)
#define PF_CFNFPR1_BSSID_OFFSET              4
#define PF_CFNFPR1_COALESE_BIT               BIT(1)
#define PF_CFNFPR1_EN_BIT                    BIT(0)


#define PF_CFNFPR2_FIELD_ID_5_MASK           BITS(26,31)
#define PF_CFNFPR2_FIELD_ID_5_OFFSET         26
#define PF_CFNFPR2_CHECK_LEN_5_MASK          BITS(24,25)
#define PF_CFNFPR2_CHECK_LEN_5_OFFSET        24
#define PF_CFNFPR2_FIELD_ID_4_MASK           BITS(18,23)
#define PF_CFNFPR2_FIELD_ID_4_OFFSET         18
#define PF_CFNFPR2_CHECK_LEN_4_MASK          BITS(16,17)
#define PF_CFNFPR2_CHECK_LEN_4_OFFSET        16
#define PF_CFNFPR2_FIELD_ID_3_MASK           BITS(10,15)
#define PF_CFNFPR2_FIELD_ID_3_OFFSET         10
#define PF_CFNFPR2_CHECK_LEN_3_MASK          BITS(8,9)
#define PF_CFNFPR2_CHECK_LEN_3_OFFSET        8
#define PF_CFNFPR2_FIELD_ID_2_MASK           BITS(2,7)
#define PF_CFNFPR2_FIELD_ID_2_OFFSET         2
#define PF_CFNFPR2_CHECK_LEN_2_MASK          BITS(0,1)
#define PF_CFNFPR2_CHECK_LEN_2_OFFSET        0

/*------------------------------------------------------------------------------
 * Field ID Define in Packet Parser
 *------------------------------------------------------------------------------
*/
#define PF_CF_FIELD_ID_00_UNDEFINED           0x00
#define PF_CF_FIELD_ID_01_ETH_TYPE            0x01
#define PF_CF_FIELD_ID_02_IPV4_VER_IHL        0x02
#define PF_CF_FIELD_ID_03_IPV4_FRAGMENT       0x03
#define PF_CF_FIELD_ID_04_IPV4_PROTOCOL       0x04
#define PF_CF_FIELD_ID_05_IPV4_SRCIP          0x05
#define PF_CF_FIELD_ID_06_IPV4_DSTIP          0x06
#define PF_CF_FIELD_ID_07_IPV6_QOS            0x07
#define PF_CF_FIELD_ID_08_IPV6_LEN_NEXT_HDR   0x08
#define PF_CF_FIELD_ID_09_IPV6_SRCIP          0x09
#define PF_CF_FIELD_ID_0A_IPV6_DSTIP          0x0A
#define PF_CF_FIELD_ID_0B_TCP_SRC_DST_PORT    0x0B
#define PF_CF_FIELD_ID_0C_TCP_SEQ             0x0C
#define PF_CF_FIELD_ID_0D_TCP_ACK             0x0D
#define PF_CF_FIELD_ID_0E_TCP_CONTROL_BITS    0x0E
#define PF_CF_FIELD_ID_0F_TCP_CHK_SUM         0x0F
#define PF_CF_FIELD_ID_10_UDP_SRC_DST_PORT    0x10
#define PF_CF_FIELD_ID_11_UDP_LEN             0x11
#define PF_CF_FIELD_ID_12_ARP_PROTOCOL        0x12
#define PF_CF_FIELD_ID_13_ARP_OP_CODE         0x13
#define PF_CF_FIELD_ID_14_ARP_SRC_HW_ADDR     0x14
#define PF_CF_FIELD_ID_15_ARP_SPA             0x15
#define PF_CF_FIELD_ID_16_ARP_DST_HW_ADDR     0x16
#define PF_CF_FIELD_ID_17_ARP_DST_HW_ADDR2    0x17
#define PF_CF_FIELD_ID_18_ARP_TPA             0x18
#define PF_CF_FIELD_ID_19_ICMPV6_TYPE         0x19
#define PF_CF_FIELD_ID_1A_ICMPV6_RESV         0x1A
#define PF_CF_FIELD_ID_1B_ICMPV6_TARGET_ADDR  0x1B


/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef enum _ENUM_PF_CF_MASK_OPERATION_T {
    PF_CF_MASK_OP_EQUAL = 0,
    PF_CF_MASK_OP_NOT_EQUAL = 1,
    PF_CF_MASK_OP_NUM
} ENUM_PF_CF_MASK_OPERATION_T;

typedef enum _ENUM_PF_CF_FIELD_LENGTH_T {
    PF_CF_FIELD_LENTH_0 = 0,
    PF_CF_FIELD_LENTH_4 = 1,
    PF_CF_FIELD_LENTH_8 = 2,
    PF_CF_FIELD_LENTH_16 = 3,
    PF_CF_FIELD_LENGTH_NUM
} ENUM_PF_CF_FIELD_LENGTH_T;

/* Filed ID is different from index */
typedef enum _ENUM_PF_CF_FIELD_IDX_T {
    PF_CF_FIELD_1 = 0,
    PF_CF_FIELD_2 = 1,
    PF_CF_FIELD_3 = 2,
    PF_CF_FIELD_4 = 3,
    PF_CF_FIELD_5 = 4,
    PF_CF_FIELD_IDX_NUM
} ENUM_PF_CF_FIELD_IDX_T;

typedef enum _ENUM_PF_CF_FILTER_IDX_T {
    PF_CF_FILTER_IDX_0 = 0,
    PF_CF_FILTER_IDX_1 = 1,
    PF_CF_FILTER_IDX_2 = 2,
    PF_CF_FILTER_IDX_3 = 3,
    PF_CF_FILTER_IDX_4 = 4,
    PF_CF_FILTER_IDX_5 = 5,
    PF_CF_FILTER_IDX_6 = 6,
    PF_CF_FILTER_IDX_7 = 7,
    PF_CF_FILTER_IDX_8 = 8,
    PF_CF_FILTER_IDX_9 = 9,
    PF_CF_FILTER_IDX_NUM
} ENUM_PF_CF_FILTER_IDX_T;

typedef enum _ENUM_PF_ARPNS_IP_VER_T {
    PF_ARPNS_IP_VER_IP4 = 0,
    PF_ARPNS_IP_VER_IP6 = 1,
    PF_ARPNS_IP_VER_NUM
} _ENUM_PF_ARPNS_IP_VER_T;

typedef enum _ENUM_PF_ARPNS_IDX_T {
    PF_ARPNS_IDX_0 = 0,
    PF_ARPNS_IDX_1 = 1,
    PF_ARPNS_IDX_2 = 2,
    PF_ARPNS_IDX_NUM
} ENUM_PF_ARPNS_IDX_T;

/* Used for PF_MODE */
typedef enum _ENUM_PF_MODE_T {
    PF_MODE_WHITE_LIST = 0,
    PF_MODE_BLACK_LIST,
    PF_MODE_NUM
} ENUM_PF_MODE_T;


/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/
#if 0
typedef struct _HW_MAC_RX_DESC_T {
    UINT_32    u4BufStartAddr;      /* DW 0 */
    UINT_32    u4NextDesc;
    UINT_16    u2PacketLength;      /* DW 2 */
    UINT_8     ucRxVtSeqNo;
    UINT_8     ucHeaderLen;
    UINT_8     ucChanFreq;          /* DW 3 */
    UINT_8     ucWlanIdx;
    UINT_16    u2StatusFlag;
    UINT_8     ucMatchPacket;       /* DW 4 */
    UINT_8     ucKIdxSecMode;
    UINT_8     aucRSC[6];           /* DW 4~5 */
    UINT_32    au4PN[3];            /* DW 6~8 */
    UINT_32    u4Timestamp[2];      /* DW 9~10 */
    UINT_32    u4CRC;               /* DW 11 */

//#if defined(MT6628_E1)
    /* For HDR_TRAN */
    UINT_16    u2FrameCtl;          /* DW12 */
    UINT_8     aucTA[6];            /* DW12~13 */
    UINT_16    u2SeqFrag;           /* DW14 */
    UINT_16    u2Qos;               /* DW14 */
    UINT_32    u4HTC;               /* DW15 */
//#endif
    /*!  RX Vector Info*/
    UINT_32    u4RxVector[4];       /* DW 16~19 */
}  HW_MAC_RX_DESC_T, *P_HW_MAC_RX_DESC_T;
#endif

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

#if (WIFI_UNI_DRIVER_RAM == 0)
/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*----------------------------------------------------------------------------*/
/*!
* Coalescing Filter HAL
*
*/
/*----------------------------------------------------------------------------*/
HAL_STATUS
halPfCoalescFieldSet (
    IN ENUM_PF_CF_FILTER_IDX_T eFilterIdx, IN ENUM_PF_CF_FIELD_IDX_T eFieldIdx, IN ENUM_PF_CF_MASK_OPERATION_T fgCompareOP,
    IN UINT_8 ucFieldId, IN ENUM_PF_CF_FIELD_LENGTH_T ucCompareLen, IN PUINT_8 pucMask, IN PUINT_8 pucPattern
    );

HAL_STATUS
halPfCoalescFilterSet (
    IN ENUM_PF_CF_FILTER_IDX_T eFilterIdx,
    IN UINT_8 ucPktType, IN UINT_8 ucBssid, IN BOOL fgCoalesceOP
    );

HAL_STATUS
halPfCoalescFilterEnable (
    IN ENUM_PF_CF_FILTER_IDX_T eFilterIdx, IN BOOL fgEnable
    );

/*----------------------------------------------------------------------------*/
/*!
* ARP/NS Offload HAL
*
*/
/*----------------------------------------------------------------------------*/
HAL_STATUS
halPfArpNsRemoteIpAddrSet (
    IN _ENUM_PF_ARPNS_IP_VER_T eArpNsIpVer, IN PUINT_8 pucIpAddr
    );

HAL_STATUS
halPfArpNsRemoteIpAddrEn (
    IN _ENUM_PF_ARPNS_IP_VER_T eArpNsIpVer, IN BOOL fgEnable
    );

HAL_STATUS
halPfArpNsParamClear (
    IN ENUM_PF_ARPNS_IDX_T eArpNsIdx
    );

HAL_STATUS
halPfArpNsParamSet (
    IN ENUM_PF_ARPNS_IDX_T eArpNsIdx, IN _ENUM_PF_ARPNS_IP_VER_T eArpNsIpVer,
    IN UINT_8 ucPktType, IN UINT_8 ucBssid, IN BOOL fgCoalesc, IN PUINT_8 pucIpAddr, IN BOOL fgClear
    );

HAL_STATUS
halPfArpNsEn (
    IN ENUM_PF_ARPNS_IDX_T eArpNsIdx, IN BOOL fgEnable
    );

/*----------------------------------------------------------------------------*/
/*!
* Bitmap Pattern HAL
*
*/
/*----------------------------------------------------------------------------*/
HAL_STATUS
halPfBitmapParamSet (
    IN UINT_8 ucBitmapIdx, IN UINT_8 ucDwOffset, IN UINT_8 ucByteMask, IN UINT_8 ucBssid, IN BOOL fgCoalesc, IN PUINT_8 pucPattern
    );

HAL_STATUS
halPfBitmapParamEn (
    IN UINT_8 ucBitmapIdx, IN BOOL fgEnable
    );

/*----------------------------------------------------------------------------*/
/*!
* IPv6 Extension Header HAL
*
*/
/*----------------------------------------------------------------------------*/
VOID
halPfIPv6ExtHdrChkEn (
    IN UINT_8 ucExtHdrChkEn
    );

VOID
halPfIPv6UserDefNextHdrConfig (
    IN UINT_8 ucNextHdr, IN UINT_8 ucJumpDw, IN UINT_8 ucJumpWeight
    );

VOID
halPfIPv6UserDefNextHdrEn (
    IN BOOL fgEnable
    );

/*----------------------------------------------------------------------------*/
/*!
* Global Enable/Disable
*
*/
/*----------------------------------------------------------------------------*/
VOID
halPfGlobalBssidEn (
    IN UINT_8 ucBssid
    );

HAL_STATUS
halPfGlobalModeSet (
    IN ENUM_PF_MODE_T ePfMode
    );

/*
*** We need to polling PF_RR[31] to make sure PF is stopped ***
*/
VOID
halPfGlobalEn (
    IN BOOLEAN fgEnable
    );

VOID
halPfMagicBssidEn (
    IN UINT_8 ucMagicBssid
    );

VOID
halPfGlobalMagicEn (
    IN BOOLEAN fgEnable
    );

VOID
halPfGlobalTdlslEn (
    IN BOOLEAN fgEnable
    );

VOID
halPfGlobalEapolEn (
    IN BOOLEAN fgEnable
    );

VOID
halPfGlobalArpNsEn (
    IN BOOLEAN fgEnable
    );

VOID
halPfGlobalBitmapEn (
    IN BOOLEAN fgEnable
    );

VOID
halPfGlobalCfEn (
    IN BOOLEAN fgEnable
    );

#endif /* (WIFI_UNI_DRIVER_RAM == 0) */

#endif /* _MT6630_WIFI_PF_H */

