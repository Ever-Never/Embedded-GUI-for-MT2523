/*! \file   packet_filter.h
    \brief  Packet Filter structure and parameters

    This file provides PF support.
*/

/*******************************************************************************
* Copyright (c) 2014 MediaTek Inc.
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
** $Log: packet_filter.h $
 *
 * 05 21 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Remove RAM header file dependancy in ROM code.
 * Created parallel header files for RAM files which were being used in ROM code.
**
** 05 15 2015 raju.singh
** [BORA00004254] [MT7637E2][IOT][FW]
** IOT code with STA+promiscus mode and AP mode with APCLI and Repeater feature.
**
** 02 14 2015 ted.huang
** [BORA00003379] [MT7637]
** Merge 7615 FW RAM change .
**
** 09 05 2014 ted.huang
** [BORA00002807] [MT7636]
** Support ARP offload.
**
** 03 03 2014 Bennett Ou
** Create file
**
*/

#ifndef _PACKET_FILTER_H
#define _PACKET_FILTER_H
#define CFG_PF_ARPNS_SUPPORT    	                1

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
/* PF feature option */
#define PF_DROP_LLC_NOT_MATCH           (1)

/* General */
#define PF_CMD_OP_CODE_LEN              (4)

/* bitmap pattern size */
#define PF_NUM_OF_BITMAP_PATTERNS       (16)
#define PF_SIZE_OF_BITMAP_MASK          (16)
#define PF_SIZE_OF_BITMAP_PATTERN       (128)

/* ARP/NS address size */
#define PF_SIZE_OF_ARPNS_SETTING        (3)
#define PF_SIZE_OF_ARPNS_ADDRESS_MAX    (16)
/* IPv4/IPv6 address */
#define PF_IPV4_ADDR_LEN                (4)
#define PF_IPV6_ADDR_LEN                (16)

/* coalecing filter size */
#define PF_CF_PATTERN_LEN_MAX           (16)

#define ARP_OFFSET_OPCODE               6
#define ARP_OFFSET_SA                   8
#define ARP_OFFSET_SPA                  14
#define ARP_OFFSET_DA                   18
#define ARP_OFFSET_TPA                  24
#define ARP_MSG_SIZE                    28

#define LENGTH_802_3                    14

#define PF_DATA_FRAME_LLC_LEN           6
#define PF_DATA_FRAME_ETH_TYPE_LEN      2

#define ARP_OFFSET_PROTOCO_START        (PF_DATA_FRAME_LLC_LEN + PF_DATA_FRAME_ETH_TYPE_LEN)

/* ARP frame */
#define ARP_FRM_HWTYPE                 0x0001
#define ARP_FRM_PROTOCOL               0x0800
#define ARP_FRM_HW_SIZE                6
#define ARP_FRM_PRO_SIZE               4
#define ARP_FRM_RESPONSE               2
#define ARP_FRM_REQUEST                1

/* PF command feature bit define */
#define PF_CMD_FEATURE_BIT0_WAKEUP_TYPE     BIT(0)
#define PF_CMD_FEATURE_BIT1_EAPOL           BIT(1)
#define PF_CMD_FEATURE_BIT2_ARPNS           BIT(2)
#define PF_CMD_FEATURE_BIT3_IPV4            BIT(3)
#define PF_CMD_FEATURE_BIT4_IPV6            BIT(4)
#define PF_CMD_FEATURE_BIT5_CONFIG_BY_CF    BIT(5)
#define PF_CMD_FEATURE_BIT6_PKT_TO_HIF_MCU  BIT(6)

/* Wakeup command bit define */
#define PF_WAKEUP_CMD_BIT0_OUTPUT_MODE_EN   BIT(0)
#define PF_WAKEUP_CMD_BIT1_OUTPUT_DATA      BIT(1)
#define PF_WAKEUP_CMD_BIT2_WAKEUP_LEVEL     BIT(2)

/* The length is the EAPoL-Key frame except key data field.
   Please refer to 802.11i-2004 ,Figure 43u in p.78 */
#define MIN_LEN_OF_EAPOL_KEY_MSG	    95

/* The related length of the EAPOL Key frame */
#define LEN_KEY_DESC_NONCE			    32
#define LEN_KEY_DESC_IV				    16
#define LEN_KEY_DESC_RSC			    8
#define LEN_KEY_DESC_ID				    8
#define LEN_KEY_DESC_REPLAY			    8
#define LEN_KEY_DESC_MIC			    16

/* EAP Code Type */
#define EAP_CODE_REQUEST                1
#define EAP_CODE_RESPONSE               2
#define EAP_CODE_SUCCESS                3
#define EAP_CODE_FAILURE                4

/* EAPOL frame Protocol Version */
#define	EAPOL_VER                       1
#define	EAPOL_VER2                      2

/* EAPOL-KEY Descriptor Type */
#define	WPA1_KEY_DESC                   0xfe
#define WPA2_KEY_DESC                   0x02

/* Key Descriptor Version of Key Information */
#define	KEY_DESC_TKIP			        1
#define	KEY_DESC_AES			        2
#define KEY_DESC_EXT			        3

#define IE_WPA					        221
#define IE_RSN					        48

#define WPA_KDE_TYPE			        0xdd

/* EAP Offset define */
#define EAPOFFSET_CODE                  0
#define EAPOFFSET_ID                    1
#define EAPOFFSET_LENGTH                2
#define EAPOFFSET_TYPE                  4

/* EAP Packet Type */
#define	EAPPacket                       0
#define	EAPOLStart                      1
#define	EAPOLLogoff                     2
#define	EAPOLKey                        3
#define	EAPOLASFAlert                   4
#define	EAPTtypeMax                     5

/* EAP Code */
#define	EAPCodeRequest                  1
#define	EAPCodeResponse                 2
/* EAP Type */
#define	EAPTypeIdentity                 1

#define PAIRWISEKEY					    1
#define GROUPKEY					    0

#define	EAPOL_MSG_INVALID               0
#define	EAPOL_PAIR_MSG_1                1
#define	EAPOL_PAIR_MSG_2                2
#define	EAPOL_PAIR_MSG_3                3
#define	EAPOL_PAIR_MSG_4                4
#define	EAPOL_GROUP_MSG_1               5
#define	EAPOL_GROUP_MSG_2               6

/* RSN IE Length definition */
#define MAX_LEN_OF_RSNIE         	    255
#define MIN_LEN_OF_RSNIE         	    18
#define MAX_LEN_GTK					    32
#define MIN_LEN_GTK					    5

#define LEN_PMK						    32
#define LEN_PMKID					    16
#define LEN_PMK_NAME				    16

#define LEN_GMK						    32

#define LEN_PTK_KCK					    16
#define LEN_PTK_KEK					    16
#define LEN_TK						    16	// The length Temporal key.
#define LEN_TKIP_MIC				    8	// The length of TX/RX Mic of TKIP
#define LEN_TK2						    (2 * LEN_TKIP_MIC)
#define LEN_PTK						    (LEN_PTK_KCK + LEN_PTK_KEK + LEN_TK + LEN_TK2)

#define LEN_TKIP_PTK				    LEN_PTK
#define LEN_AES_PTK					    (LEN_PTK_KCK + LEN_PTK_KEK + LEN_TK)
#define LEN_TKIP_GTK				    (LEN_TK + LEN_TK2)
#define LEN_AES_GTK					    LEN_TK
#define LEN_TKIP_TK					    (LEN_TK + LEN_TK2)
#define LEN_AES_TK					    LEN_TK

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/* Packet filter command first field: type (OP code) */
typedef enum _ENUM_PF_TYPE_T
{
    ENUM_PF_CMD_TYPE_MAGIC                         = 0,
    ENUM_PF_CMD_TYPE_BITMAP                        = 1,
    ENUM_PF_CMD_TYPE_ARPNS                         = 2,
    ENUM_PF_CMD_TYPE_GTK_REKEY                     = 3,
    ENUM_PF_CMD_TYPE_COALESCING_FILTER             = 4,
    ENUM_PF_CMD_TYPE_HW_GLOBAL_ENABLE              = 5,
    ENUM_PF_CMD_TYPE_TCP_SYN                       = 6,
    ENUM_PF_CMD_TYPE_TDLS                          = 7,
} ENUM_PF_TYPE_T, *P_ENUM_PF_TYPE_T;

/* Packet filter global enable command function select */
typedef enum _ENUM_PF_GLOBAL_FUNCTION_SEL_T
{
    ENUM_PF_CMD_FUNCTION_PF                        = 0x0000,
    ENUM_PF_CMD_FUNCTION_GLOBAL_MAGIC              = 0x0001,
    ENUM_PF_CMD_FUNCTION_GLOBAL_BITMAP             = 0x0002,
    ENUM_PF_CMD_FUNCTION_GLOBAL_EAPOL              = 0x0003,
    ENUM_PF_CMD_FUNCTION_GLOBAL_TDLS               = 0x0004,
    ENUM_PF_CMD_FUNCTION_GLOBAL_ARPNS              = 0x0005,
    ENUM_PF_CMD_FUNCTION_GLOBAL_COALESCING_FILTER  = 0x0006,
    ENUM_PF_CMD_FUNCTION_GLOBAL_MODE               = 0x0007,
    ENUM_PF_CMD_FUNCTION_GLOBAL_BSSID              = 0x0008,
    ENUM_PF_CMD_FUNCTION_GLOBAL_MGMT               = 0x0009,
    ENUM_PF_CMD_FUNCTION_GLOBAL_BMC_MISMATCH_DROP  = 0x000A,
    ENUM_PF_CMD_FUNCTION_GLOBAL_UC_MISMATCH_DROP   = 0x000B,
    ENUM_PF_CMD_FUNCTION_GLOBAL_ALL_TO_MCU         = 0x000C,
    ENUM_PF_CMD_FUNCTION_GLOBAL_WOW_EN             = 0x0010
} ENUM_PF_GLOBAL_FUNCTION_SEL_T, *P_ENUM_PF_GLOBAL_FUNCTION_SEL_T;

/* Packet filter reserved CF index */
typedef enum _ENUM_PF_CF_INDEX_T
{
    ENUM_PF_CF_IDX_ARP                             = 0,
    ENUM_PF_CF_IDX_NS                              = 1,
    ENUM_PF_CF_IDX_IPV4_TCP_SYN_                   = 2,
    ENUM_PF_CF_IDX_IPV6_TCP_SYN_                   = 3,
    ENUM_PF_CF_IDX_MAX                             = 4,
} ENUM_PF_CF_INDEX_T, *P_ENUM_PF_CF_INDEX_T;

/* For IPv4/IPv6 option */
typedef enum _ENUM_PF_IP_VER_T
{
    ENUM_PF_IP_VER_IP4 = 0,
    ENUM_PF_IP_VER_IP6 = 1,
    ENUM_PF_IP_VER_NUM
} ENUM_PF_IP_VER_T, *P_ENUM_PF_IP_VER_T;

/* For wkaeup option */
typedef enum _ENUM_PF_WAKEUP_OPT_T
{
    ENUM_PF_WAKEUP_PCIE = 0,
    ENUM_PF_WAKEUP_USB = 1,
    ENUM_PF_WAKEUP_GPIO =2,
    ENUM_PF_WAKEUP_SDIO =3,
    ENUM_PF_WAKEUP_NUM  =4,
} ENUM_PF_WAKEUP_OPT_T, *P_ENUM_PF_WAKEUP_OPT_T;

/* Packet filter offload type */
typedef enum _ENUM_PF_OFFLOAD_TYPE_T
{
    PF_OFFLOAD_MISMATCH = 0,
    PF_OFFLOAD_EAPOL    = 1,
    PF_OFFLOAD_ARPNS    = 2,
    PF_OFFLOAD_TDLS     = 3,
    PF_OFFLOAD_TYPE_NUM
} ENUM_PF_OFFLOAD_TYPE_T;

/*-- Packet filter command --*/
/* magic packet command */
typedef struct _EXT_CMD_PF_MAGIC_PACKET_T
{
	UINT_32			u4Bssid;
} EXT_CMD_PF_MAGIC_PACKET_T, *P_EXT_CMD_PF_MAGIC_PACKET_T;

/* bitmap pattern command */
#if (CFG_PF_BITMAP_SUPPORT == 1)
typedef struct _EXT_CMD_PF_BITMAP_PATTERN_T
{
	UINT_32			u4Index;
	UINT_32			u4Enable;
	UINT_32			u4Bssid;
	UINT_32			u4Offset;
	UINT_32			u4FeatureBit;
	UINT_32			u4Resv;
	UINT_32			u4DataLen;
	UINT_8			aucMaskBit[PF_SIZE_OF_BITMAP_MASK];
	UINT_8			aucPattern[PF_SIZE_OF_BITMAP_PATTERN];
} EXT_CMD_PF_BITMAP_PATTERN_T, *P_EXT_CMD_PF_BITMAP_PATTERN_T;

/* structure for local store */
typedef struct _PF_BITMAP_PATTERN_T
{
	UINT_8			ucIndex;
	UINT_8			fgEnable;
	UINT_8			ucOffset;
	UINT_8			ucDataLen;
	UINT_8			ucFeatureBit;
	UINT_8			ucBssid;
	UINT_8			resv[2];
	UINT_8			aucMaskByte[PF_SIZE_OF_BITMAP_PATTERN];
	UINT_8			aucPattern[PF_SIZE_OF_BITMAP_PATTERN];
} PF_BITMAP_PATTERN_T, *P_PF_BITMAP_PATTERN_T;
#endif /* CFG_PF_BITMAP_SUPPORT */

/* ARP/NS command */
#if (CFG_PF_ARPNS_SUPPORT == 1)
typedef struct _EXT_CMD_PF_ARP_NS_T
{
	UINT_32			u4Index;
	UINT_32			u4Enable;
	UINT_32			u4Bssid;
	UINT_32			u4Version; /* 0:IPv4, 1:IPv6 */
	UINT_32			u4PacketType;
	UINT_32			u4FeatureBit;
	UINT_32			u4Resv;
	UINT_8			aucAddr[PF_SIZE_OF_ARPNS_ADDRESS_MAX];
} EXT_CMD_PF_ARP_NS_T, *P_EXT_CMD_PF_ARP_NS_T;

#ifndef __GNUC__
#ifdef IAR_COMPILE
__packed typedef struct  _ARP_PKT {
#else
typedef	struct __packed _ARP_PKT {
#endif

	UINT_16	 			HwType;
	UINT_16	 			Protocol;
	UINT_8	 			HwSize;
	UINT_8	 			ProSize;
	UINT_16	 			Opcode;
	UINT_8				SA[6];
	UINT_8				SPA[4];
	UINT_8				DA[6];
	UINT_8				TPA[4];
}	ARP_PKT;
#else
typedef	struct _ARP_PKT
{
	UINT_16	 			HwType;
	UINT_16	 			Protocol;
	UINT_8	 			HwSize;
	UINT_8	 			ProSize;
	UINT_16	 			Opcode;
	UINT_8				SA[6];
	UINT_8				SPA[4];
	UINT_8				DA[6];
	UINT_8				TPA[4];
}__attribute__((__packed__)) ARP_PKT;
#endif

#endif /* CFG_PF_ARPNS_SUPPORT */


#define	CONV_ARRARY_TO_UINT16(_V)	((_V[0]<<8) | (_V[1]))
#define	SET_UINT16_TO_ARRARY(_V, _LEN)		\
{											\
	_V[0] = ((UINT_16)_LEN) >> 8;			\
	_V[1] = ((UINT_16)_LEN & 0xFF);					\
}

#ifndef __GNUC__
#if 0
/* EAPOL Key Information definition within Key descriptor format */
typedef	struct __packed _KEY_INFO
{
	UCHAR	KeyMic:1;
	UCHAR	Secure:1;
	UCHAR	Error:1;
	UCHAR	Request:1;
	UCHAR	EKD_DL:1;       // EKD for AP; DL for STA
	UCHAR	Rsvd:3;
	UCHAR	KeyDescVer:3;
	UCHAR	KeyType:1;
	UCHAR	KeyIndex:2;
	UCHAR	Install:1;
	UCHAR	KeyAck:1;
}	KEY_INFO, *PKEY_INFO;

/* EAPOL Key descriptor format */
typedef	struct __packed _KEY_DESCRIPTER
{
	UCHAR		Type;
	KEY_INFO	KeyInfo;
	UCHAR		KeyLength[2];
	UCHAR		ReplayCounter[LEN_KEY_DESC_REPLAY];
	UCHAR		KeyNonce[LEN_KEY_DESC_NONCE];
	UCHAR		KeyIv[LEN_KEY_DESC_IV];
	UCHAR		KeyRsc[LEN_KEY_DESC_RSC];
	UCHAR		KeyId[LEN_KEY_DESC_ID];
	UCHAR		KeyMic[LEN_KEY_DESC_MIC];
	UCHAR		KeyDataLen[2];
	UCHAR		KeyData[0];
}	KEY_DESCRIPTER, *PKEY_DESCRIPTER;

typedef	struct __packed _EAPOL_PACKET
{
	UCHAR	 			ProVer;
	UCHAR	 			ProType;
	UCHAR	 			Body_Len[2];
	KEY_DESCRIPTER		KeyDesc;
}	EAPOL_PACKET, *PEAPOL_PACKET;

typedef struct __packed _KDE_HDR
{
    UCHAR               Type;
    UCHAR               Len;
    UCHAR               OUI[3];
    UCHAR               DataType;
	UCHAR				octet[0];
}   KDE_HDR, *PKDE_HDR;

/* 802.11i D10 page 83 */
typedef struct __packed _GTK_KDE
{
    UCHAR               Kid:2;
    UCHAR               tx:1;
    UCHAR               rsv:5;
    UCHAR               rsv1;
    UCHAR               GTK[0];
}   GTK_KDE, *PGTK_KDE;
#endif
#else
/* EAPOL Key Information definition within Key descriptor format */
#if 0
typedef	struct _KEY_INFO
{
	UCHAR	KeyMic:1;
	UCHAR	Secure:1;
	UCHAR	Error:1;
	UCHAR	Request:1;
	UCHAR	EKD_DL:1;       // EKD for AP; DL for STA
	UCHAR	Rsvd:3;
	UCHAR	KeyDescVer:3;
	UCHAR	KeyType:1;
	UCHAR	KeyIndex:2;
	UCHAR	Install:1;
	UCHAR	KeyAck:1;
}__attribute__((__packed__)) KEY_INFO, *PKEY_INFO;

/* EAPOL Key descriptor format */
typedef	struct _KEY_DESCRIPTER
{
	UCHAR		Type;
	KEY_INFO	KeyInfo;
	UCHAR		KeyLength[2];
	UCHAR		ReplayCounter[LEN_KEY_DESC_REPLAY];
	UCHAR		KeyNonce[LEN_KEY_DESC_NONCE];
	UCHAR		KeyIv[LEN_KEY_DESC_IV];
	UCHAR		KeyRsc[LEN_KEY_DESC_RSC];
	UCHAR		KeyId[LEN_KEY_DESC_ID];
	UCHAR		KeyMic[LEN_KEY_DESC_MIC];
	UCHAR		KeyDataLen[2];
	UCHAR		KeyData[0];
}__attribute__((__packed__)) KEY_DESCRIPTER, *PKEY_DESCRIPTER;

typedef	struct _EAPOL_PACKET
{
	UCHAR	 			ProVer;
	UCHAR	 			ProType;
	UCHAR	 			Body_Len[2];
	KEY_DESCRIPTER		KeyDesc;
}__attribute__((__packed__)) EAPOL_PACKET, *PEAPOL_PACKET;

typedef struct _KDE_HDR
{
    UCHAR               Type;
    UCHAR               Len;
    UCHAR               OUI[3];
    UCHAR               DataType;
	UCHAR				octet[0];
}__attribute__((__packed__)) KDE_HDR, *PKDE_HDR;


/* 802.11i D10 page 83 */
typedef struct _GTK_KDE
{
    UCHAR               Kid:2;
    UCHAR               tx:1;
    UCHAR               rsv:5;
    UCHAR               rsv1;
    UCHAR               GTK[0];
}__attribute__((__packed__)) GTK_KDE, *PGTK_KDE;
#endif
#endif


#if (CFG_PF_EAPOL_SUPPORT == 1)
typedef struct _EXT_CMD_PF_REKEY_T
{
	UINT_32			u4WPA2; /* 0: WPA, 1: WPA2 */
	UINT_8			aucPTK[LEN_PTK];
	UINT_8			aucReplayCnt[LEN_KEY_DESC_REPLAY];
	UINT_32			u4PairwiseWidx;
	UINT_32			u4GroupWidx;
	UINT_32			u4BssidIdx;
	UINT_32			u4OwnMacIdx;
	UINT_32			u4WmmIdx;
	UINT_32			u4Resv;
} EXT_CMD_PF_REKEY_T, *P_EXT_CMD_PF_REKEY_T;

typedef struct _PF_GTK_REKEY_INFO_T
{
	BOOLEAN			fgWpa2;
	UINT_8			ucPairwiseWidx;
	UINT_8			ucPairwiseCipher;
	UINT_8			ucGroupWidx;
	UINT_8			ucGroupCipher;
	UINT_8			ucDefaultKeyId;
	UINT_8			ucBssidIdx;
	UINT_8			ucOwnMacIdx;
	UINT_8			ucWmmIdx;
	UINT_8			ucResv[3];
	UINT_8			aucReplayCnt[LEN_KEY_DESC_REPLAY];
	UINT_8			aucPTK[LEN_PTK];
    UINT_8          aucGTK[MAX_LEN_GTK]; /* store GTK in group1 message */
} PF_GTK_REKEY_INFO_T, *P_PF_GTK_REKEY_INFO_T;

#endif /* CFG_PF_EAPOL_SUPPORT */

/* Coalescing Filter command */
#if (CFG_PF_CF_COMMAND_SUPPORT == 1)
/* field setting */
typedef struct _EXT_CMD_PF_CF_FIELD_T
{
	UINT_8			ucFieldId;
	UINT_8			ucCompare;
	UINT_8			ucFieldLen;
	UINT_8			ucResv;
	UINT_8			aucMask[PF_CF_PATTERN_LEN_MAX];
	UINT_8			aucPattern[PF_CF_PATTERN_LEN_MAX];
} EXT_CMD_PF_CF_FIELD_T, *P_EXT_CMD_PF_CF_FIELD_T;

/* filter setting */
typedef struct _EXT_CMD_PF_CF_FILTER_T
{
	UINT_32			u4FilterIdx;
	UINT_32			u4Enable;
	UINT_32			u4Bssid;
	UINT_32			u4PacketType;
	UINT_32			u4CoalsceOp;
	UINT_32			u4FeatureBit;
} EXT_CMD_PF_CF_FILTER_T, *P_EXT_CMD_PF_CF_FILTER_T;

typedef struct _EXT_CMD_PF_CF_T
{
    /* filter setting */
    EXT_CMD_PF_CF_FILTER_T  rFilter;
    /* field setting */
    EXT_CMD_PF_CF_FIELD_T   arField[PF_CF_FIELD_IDX_NUM];

} EXT_CMD_PF_CF_T, *P_EXT_CMD_PF_T;
/* CFG_PF_CF_COMMAND_SUPPORT */

#endif /* CFG_PF_CF_COMMAND_SUPPORT */

/* CF settting for local store */
typedef struct _PF_FILTER_SETTING_T
{
	UINT_32			u4Enable;
	UINT_32			u4FeatureBit;
} PF_FILTER_SETTING_T, *P_PF_FILTER_SETTING_T;

#if (CFG_PF_TCP_SYN_SUPPORT == 1)
typedef struct _EXT_CMD_PF_TCPSYN_T
{
	UINT_32			u4Version;
	UINT_32			u4Enable;
	UINT_32			u4Bssid;
	UINT_32			u4PacketType;
	UINT_32			u4FeatureBit;
	UINT_32			u4TcpSrcPort;
	UINT_32			u4TcpDstPort;
	UINT_8			aucSrcAddr[PF_IPV6_ADDR_LEN]; /* 4 or 16 */
	UINT_8			aucDstAddr[PF_IPV6_ADDR_LEN]; /* 4 or 16 */
} EXT_CMD_PF_TCPSYN_T, *P_EXT_CMD_PF_TCPSYN_T;
#endif /* CFG_PF_TCP_SYN_SUPPORT */

typedef struct _EXT_CMD_PF_GLOBAL_FUN_T
{
	UINT_32			u4Function;
	UINT_32			u4Parameter;
	UINT_32			u4Resv;
} EXT_CMD_PF_GLOBAL_FUN_T, *P_EXT_CMD_PF_GLOBAL_FUN_T;

typedef struct _EXT_EVENT_PF_GENERAL_T
{
	UINT_32			u4PfCmdType;
	UINT_32			u4Status;
	UINT_32			u4Resv;
} EXT_EVENT_PF_GENERAL_T, *P_EVENT_PF_GENERAL_T;

typedef struct _EXT_CMD_PF_WAKEUP_HOST_T
{
	UINT_32			u4HifType;  /* refer to ENUM_PF_WAKEUP_OPT_T */
	UINT_32			u4Gpio;
	UINT_32			u4GpioTimer;
	UINT_32			u4Resv;
} EXT_CMD_PF_WAKEUP_HOST_T, *P_EXT_CMD_PF_WAKEUP_HOST_T;


/* Packet filter mode */
/* refer to ENUM_PF_MODE_T */

/* Packet filter BSSID define */
/* refer to PF_BSSID_ALL_BIT */

/* Packet filter packet type */
/* refer to PF_PKT_TYPE_BIT_ALL */

/* local save for bitmap setting */
/* local save for ARPNS setting */
typedef struct _PF_CONFIG_T
{
#if (CFG_PF_BITMAP_SUPPORT == 1)
 //   PF_BITMAP_PATTERN_T         arBitmapPattern[PF_NUM_OF_BITMAP_PATTERNS];
#endif
#if (CFG_PF_EAPOL_SUPPORT == 1)
  //  PF_GTK_REKEY_INFO_T         rRekeyInfo;
#endif
#if (CFG_PF_ARPNS_SUPPORT == 1)
    EXT_CMD_PF_ARP_NS_T         arpNs[PF_SIZE_OF_ARPNS_SETTING];
#endif
    /* CF setting: ARPNS/TCPSYN/CF_cmd */
//    PF_FILTER_SETTING_T         arCfSetting[PF_CF_FILTER_IDX_NUM];

    /* wakeup HIF host type */
 //   EXT_CMD_PF_WAKEUP_HOST_T    rWakeupHost;

    /* if TRUE, means WOW enable */
   // BOOL                        fgWowEnable;
} PF_CONFIG_T, *P_PF_CONFIG_T;

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
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/
#if (CFG_PF_EAPOL_SUPPORT == 1)
UINT_32 pfSecurityUpdateGroupKey(
    IN PUINT_8 pucGTK,
    IN UINT_8 ucWlanIdx,
    IN UINT_8 ucKeyId
    );

WLAN_STATUS
pfRekeyConfig (
    P_EXT_CMD_PF_REKEY_T    prRekeyCmd
    );

#endif

VOID
pfInit(
    VOID
    );

BOOLEAN
pfHandleMacRxOffload(
    IN P_SW_RFB_T prSwRfb
    );

WLAN_STATUS
hemExtCmdWakeupOption(
    P_MSDU_INFO_T   prMsduInfo
    );

WLAN_STATUS
hemExtCmdPacketFilter (
    P_MSDU_INFO_T   prMsduInfo
    );

WLAN_STATUS
pfGlobalFuncCtrl (
    UINT_32 u4Funcion, UINT_32 u4Parameter
    );

WLAN_STATUS
pfGlobalFuncCmd (
    P_EXT_CMD_PF_GLOBAL_FUN_T   prGlobalCmd
    );

WLAN_STATUS
pfMagicConfig (
    UINT_32   u4Bssid
    );

#if (CFG_PF_BITMAP_SUPPORT == 1)
WLAN_STATUS
pfBitmapConfig (
    P_EXT_CMD_PF_BITMAP_PATTERN_T   prBitmapConfig
    );
#endif

#if (CFG_PF_ARPNS_SUPPORT == 1)
WLAN_STATUS
pfArpNsConfig (
    P_EXT_CMD_PF_ARP_NS_T   prArpNSConfig
    );
#endif


/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _PACKET_FILTER_H  */

