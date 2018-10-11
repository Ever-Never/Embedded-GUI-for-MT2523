/*
** $Id: //Department/DaVinci/TRUNK/MT6620_WiFi_Firmware/mcu/wifi/include/mgmt/wlan_def.h#13 $
*/

/*! \file   "wlan_def.h"
    \brief  This file includes the basic definition of WLAN

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
** $Log: $
 *
 * 05 21 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Remove RAM header file dependancy in ROM code.
 * Created parallel header files for RAM files which were being used in ROM code.
**
** 02 14 2015 ted.huang
** [BORA00003379] [MT7637]
** Merge 7615 FW RAM change .
**
** 09 15 2014 ted.huang
** [BORA00002807] [MT7636]
** merge MT7636 back main trunk.
**
** 07 15 2014 tiger.hung
** [BORA00003241] [MT7636] sync MT7636 E1 MP source code
**
** 01 08 2014 ted.huang
** [BORA00002807] [MT7636]
** Merge 6630E2 RAM code.
**
** 10 25 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** Fix the protection rate selection
**
** 10 14 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** update 11j definition
**
** 10 09 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** udpate macro for j mode
**
** 10 01 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** the change for new ar
**
** 07 08 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** Fix the rate index (CCK, OFDM)
**
** 06 14 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** 1. Add initial rate seting. 2. Add more information in sta command 3. add VHT info
**
** 05 28 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** .
**
** 05 20 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** add rx security emulation code.
**
** 02 25 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** Reserve Tx desc length for packets generated in FW
**
** 01 24 2013 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Add MBSS network type to sync driver
**
** 01 21 2013 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Support 80/160 MHz bandwidth channel for channel privilege
**
** 01 16 2013 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Use ucBssIndex to replace eNetworkTypeIndex
**
** 10 22 2012 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** update ram code for rlm
**
** 10 04 2012 yarco.yang
** [BORA00002189] [MT6630]Synch. Davinc 20120927 FW to Bora
** .
**
** 10 01 2012 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Draft version with new BSS_INFO, NETWORK_TYPE_INDEX, pkt/mem alloc/free and so on
*/

#ifndef _WLAN_DEF_H
#define _WLAN_DEF_H

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
/* PHY TYPE bit definitions */
//#define PHY_TYPE_BIT_DSSS       BIT(PHY_TYPE_DSSS_INDEX)    /* DSSS PHY (clause 15) */
#define PHY_TYPE_BIT_HR_DSSS    BIT(PHY_TYPE_HR_DSSS_INDEX) /* HR/DSSS PHY (clause 18) */
#define PHY_TYPE_BIT_ERP        BIT(PHY_TYPE_ERP_INDEX)     /* ERP PHY (clause 19) */
#define PHY_TYPE_BIT_OFDM       BIT(PHY_TYPE_OFDM_INDEX)    /* OFDM 5 GHz PHY (clause 17) */
#define PHY_TYPE_BIT_HT         BIT(PHY_TYPE_HT_INDEX)      /* HT PHY (clause 20) */
#define PHY_TYPE_BIT_VHT        BIT(PHY_TYPE_VHT_INDEX)     /* HT PHY (clause 20) */

/* PHY TYPE set definitions */
#define PHY_TYPE_SET_802_11ABGN (PHY_TYPE_BIT_OFDM | \
                                 PHY_TYPE_BIT_HR_DSSS | \
                                 PHY_TYPE_BIT_ERP | \
                                 PHY_TYPE_BIT_HT)

#define PHY_TYPE_SET_802_11BGN  (PHY_TYPE_BIT_HR_DSSS | \
                                 PHY_TYPE_BIT_ERP | \
                                 PHY_TYPE_BIT_HT)

#define PHY_TYPE_SET_802_11GN   (PHY_TYPE_BIT_ERP | \
                                 PHY_TYPE_BIT_HT)

#define PHY_TYPE_SET_802_11AN   (PHY_TYPE_BIT_OFDM | \
                                 PHY_TYPE_BIT_HT)

#define PHY_TYPE_SET_802_11ABG  (PHY_TYPE_BIT_OFDM | \
                                 PHY_TYPE_BIT_HR_DSSS | \
                                 PHY_TYPE_BIT_ERP)

#define PHY_TYPE_SET_802_11BG   (PHY_TYPE_BIT_HR_DSSS | \
                                 PHY_TYPE_BIT_ERP)

#define PHY_TYPE_SET_802_11A    (PHY_TYPE_BIT_OFDM)

#define PHY_TYPE_SET_802_11G    (PHY_TYPE_BIT_ERP)

#define PHY_TYPE_SET_802_11B    (PHY_TYPE_BIT_HR_DSSS)

#define PHY_TYPE_SET_802_11N    (PHY_TYPE_BIT_HT)

#define PHY_TYPE_SET_802_11AC    (PHY_TYPE_BIT_VHT)

#define PHY_TYPE_SET_802_11ABGNAC  (PHY_TYPE_BIT_ERP | \
                                    PHY_TYPE_BIT_HR_DSSS | \
                                    PHY_TYPE_BIT_OFDM | \
                                    PHY_TYPE_BIT_HT | \
                                    PHY_TYPE_BIT_VHT)

#define PHY_TYPE_SET_802_11ANAC    (PHY_TYPE_BIT_OFDM | \
                                    PHY_TYPE_BIT_HT | \
                                    PHY_TYPE_BIT_VHT)

#define PHY_TYPE_SET_802_11BGNAC   (PHY_TYPE_BIT_HR_DSSS | \
                                    PHY_TYPE_BIT_OFDM | \
                                    PHY_TYPE_BIT_HT | \
                                    PHY_TYPE_BIT_VHT)

typedef enum _ENUM_SW_RATE_INDEX_T
{
    RATE_1M_SW_INDEX = 0,      /* 1M */
    RATE_2M_SW_INDEX,          /* 2M */
    RATE_5_5M_SW_INDEX,        /* 5.5M */
    RATE_11M_SW_INDEX,         /* 11M */
    RATE_22M_SW_INDEX,         /* 22M */
    RATE_33M_SW_INDEX,         /* 33M */
    RATE_6M_SW_INDEX,          /* 6M */
    RATE_9M_SW_INDEX,          /* 9M */
    RATE_12M_SW_INDEX,         /* 12M */
    RATE_18M_SW_INDEX,         /* 18M */
    RATE_24M_SW_INDEX,         /* 24M */
    RATE_36M_SW_INDEX,         /* 36M */
    RATE_48M_SW_INDEX,         /* 48M */
    RATE_54M_SW_INDEX,         /* 54M */
    RATE_HT_PHY_SW_INDEX,      /* BSS Selector - HT PHY */
    RATE_NUM_SW // 15
} ENUM_SW_RATE_INDEX_T, *P_ENUM_SW_RATE_INDEX_T;

typedef enum _ENUM_HT_SW_RATE_INDEX_T
{
    HT_RATE_MCS0_SW_INDEX,
    HT_RATE_MCS1_SW_INDEX,
    HT_RATE_MCS2_SW_INDEX,
    HT_RATE_MCS3_SW_INDEX,
    HT_RATE_MCS4_SW_INDEX,
    HT_RATE_MCS5_SW_INDEX,
    HT_RATE_MCS6_SW_INDEX,
    HT_RATE_MCS7_SW_INDEX,
    HT_RATE_MCS32_SW_INDEX = 32,
    HT_RATE_NUM_SW // 9
} ENUM_HT_RATE_SW_INDEX_T, *P_ENUM_HT_RATE_SW_INDEX_T;

typedef enum _ENUM_VHT_SW_RATE_INDEX_T
{
    VHT_RATE_MCS0_SW_INDEX = 0,
    VHT_RATE_MCS1_SW_INDEX,
    VHT_RATE_MCS2_SW_INDEX,
    VHT_RATE_MCS3_SW_INDEX,
    VHT_RATE_MCS4_SW_INDEX,
    VHT_RATE_MCS5_SW_INDEX,
    VHT_RATE_MCS6_SW_INDEX,
    VHT_RATE_MCS7_SW_INDEX,
    VHT_RATE_MCS8_SW_INDEX,
    VHT_RATE_MCS9_SW_INDEX,
    VHT_RATE_NUM_SW // 10
} ENUM_VHT_RATE_SW_INDEX_T, *P_ENUM_VHT_RATE_SW_INDEX_T;

/* Rate set bit definitions */
#define RATE_SET_BIT_1M         BIT(RATE_1M_SW_INDEX)      /* Bit 0: 1M */
#define RATE_SET_BIT_2M         BIT(RATE_2M_SW_INDEX)      /* Bit 1: 2M */
#define RATE_SET_BIT_5_5M       BIT(RATE_5_5M_SW_INDEX)    /* Bit 2: 5.5M */
#define RATE_SET_BIT_11M        BIT(RATE_11M_SW_INDEX)     /* Bit 3: 11M */
#define RATE_SET_BIT_22M        BIT(RATE_22M_SW_INDEX)     /* Bit 4: 22M */
#define RATE_SET_BIT_33M        BIT(RATE_33M_SW_INDEX)     /* Bit 5: 33M */
#define RATE_SET_BIT_6M         BIT(RATE_6M_SW_INDEX)      /* Bit 6: 6M */
#define RATE_SET_BIT_9M         BIT(RATE_9M_SW_INDEX)      /* Bit 7: 9M */
#define RATE_SET_BIT_12M        BIT(RATE_12M_SW_INDEX)     /* Bit 8: 12M */
#define RATE_SET_BIT_18M        BIT(RATE_18M_SW_INDEX)     /* Bit 9: 18M */
#define RATE_SET_BIT_24M        BIT(RATE_24M_SW_INDEX)     /* Bit 10: 24M */
#define RATE_SET_BIT_36M        BIT(RATE_36M_SW_INDEX)     /* Bit 11: 36M */
#define RATE_SET_BIT_48M        BIT(RATE_48M_SW_INDEX)     /* Bit 12: 48M */
#define RATE_SET_BIT_54M        BIT(RATE_54M_SW_INDEX)     /* Bit 13: 54M */

/* Rate set definitions */
#define RATE_SET_HR_DSSS            (RATE_SET_BIT_1M | \
                                     RATE_SET_BIT_2M | \
                                     RATE_SET_BIT_5_5M | \
                                     RATE_SET_BIT_11M)

#define RATE_SET_ERP                (RATE_SET_BIT_1M | \
                                     RATE_SET_BIT_2M | \
                                     RATE_SET_BIT_5_5M | \
                                     RATE_SET_BIT_11M | \
                                     RATE_SET_BIT_6M | \
                                     RATE_SET_BIT_9M | \
                                     RATE_SET_BIT_12M | \
                                     RATE_SET_BIT_18M | \
                                     RATE_SET_BIT_24M | \
                                     RATE_SET_BIT_36M | \
                                     RATE_SET_BIT_48M | \
                                     RATE_SET_BIT_54M)

#define RATE_SET_ERP_P2P            (RATE_SET_BIT_6M | \
                                     RATE_SET_BIT_9M | \
                                     RATE_SET_BIT_12M | \
                                     RATE_SET_BIT_18M | \
                                     RATE_SET_BIT_24M | \
                                     RATE_SET_BIT_36M | \
                                     RATE_SET_BIT_48M | \
                                     RATE_SET_BIT_54M)

#define RATE_SET_OFDM               (RATE_SET_BIT_6M | \
                                     RATE_SET_BIT_9M | \
                                     RATE_SET_BIT_12M | \
                                     RATE_SET_BIT_18M | \
                                     RATE_SET_BIT_24M | \
                                     RATE_SET_BIT_36M | \
                                     RATE_SET_BIT_48M | \
                                     RATE_SET_BIT_54M)

#define RATE_SET_HT                 (RATE_SET_ERP)
//#define RATE_SET_HT                 (RATE_SET_ERP | RATE_SET_BIT_HT_PHY) /* NOTE(Kevin): TBD */


#define RATE_SET_ALL_ABG             RATE_SET_ERP

#define BASIC_RATE_SET_HR_DSSS      (RATE_SET_BIT_1M | \
                                     RATE_SET_BIT_2M)

#define BASIC_RATE_SET_HR_DSSS_ERP  (RATE_SET_BIT_1M | \
                                     RATE_SET_BIT_2M | \
                                     RATE_SET_BIT_5_5M | \
                                     RATE_SET_BIT_11M)

#define BASIC_RATE_SET_ERP          (RATE_SET_BIT_1M | \
                                     RATE_SET_BIT_2M | \
                                     RATE_SET_BIT_5_5M | \
                                     RATE_SET_BIT_11M | \
                                     RATE_SET_BIT_6M | \
                                     RATE_SET_BIT_12M | \
                                     RATE_SET_BIT_24M)

#define BASIC_RATE_SET_OFDM         (RATE_SET_BIT_6M | \
                                     RATE_SET_BIT_12M | \
                                     RATE_SET_BIT_24M)

#define BASIC_RATE_SET_ERP_P2P      (RATE_SET_BIT_6M | \
                                     RATE_SET_BIT_12M | \
                                     RATE_SET_BIT_24M)

#define INITIAL_RATE_SET_RCPI_100    RATE_SET_ALL_ABG

#define INITIAL_RATE_SET_RCPI_80    (RATE_SET_BIT_1M | \
                                     RATE_SET_BIT_2M | \
                                     RATE_SET_BIT_5_5M | \
                                     RATE_SET_BIT_11M | \
                                     RATE_SET_BIT_6M | \
                                     RATE_SET_BIT_9M | \
                                     RATE_SET_BIT_12M | \
                                     RATE_SET_BIT_24M)

#define INITIAL_RATE_SET_RCPI_60    (RATE_SET_BIT_1M | \
                                     RATE_SET_BIT_2M | \
                                     RATE_SET_BIT_5_5M | \
                                     RATE_SET_BIT_11M | \
                                     RATE_SET_BIT_6M)

#define INITIAL_RATE_SET(_rcpi)     (INITIAL_RATE_SET_ ## _rcpi)

#define RCPI_100                    100 /* -60 dBm */
#define RCPI_80                     80  /* -70 dBm */
#define RCPI_60                     60  /* -80 dBm */


/* The number of RCPI records used to calculate their average value */
#define MAX_NUM_RCPI_RECORDS        10

/* The number of RCPI records used to calculate their average value */
#define NO_RCPI_RECORDS             -128
#define MAX_RCPI_DBM                0
#define MIN_RCPI_DBM                -100

#define MAX_ASSOC_ID                (CFG_STA_REC_NUM)   /* Available AID: 1 ~ 20(STA_REC_NUM) */

#define MAX_DEAUTH_INFO_COUNT       4       /* NOTE(Kevin): Used in auth.c */
#define MIN_DEAUTH_INTERVAL_MSEC    500     /* The minimum interval if continuously send Deauth Frame */

#define MAX_IE_LENGTH       (600)
#define IE_RESERVED_LENGTH          (32)    /* for IE adding in FW, not from IE template*/
#define MAX_BEACON_PROBE_RSP_LENGTH  (MAC_TX_RESERVED_FIELD + \
                                      WLAN_MAC_MGMT_HEADER_LEN + \
                                      TIMESTAMP_FIELD_LEN + \
                                      BEACON_INTERVAL_FIELD_LEN + \
                                      CAP_INFO_FIELD_LEN + \
                                      MAX_IE_LENGTH + \
                                      IE_RESERVED_LENGTH)

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef UINT_16 PHY_TYPE, *P_PHY_TYPE;
typedef UINT_8 RCPI, *P_RCPI;
typedef UINT_8 ALC_VAL, *P_ALC_VAL;

typedef enum _ENUM_AC_T
{
    AC0 = 0,
    AC1,
    AC2,
    AC3,
    AC_NUM
} ENUM_AC_T, *P_ENUM_AC_T;


/* Station role */
typedef enum _ENUM_STA_ROLE_T
{
    STA_ROLE_UNKNOWN        = 0,
    STA_ROLE_STA            = BIT(0),
    STA_ROLE_AP             = BIT(1),
    STA_ROLE_ADHOC          = BIT(2),
    STA_ROLE_TDLS           = BIT(3),

    STA_ROLE_MASK           = BITS(0,15)
} ENUM_STA_ROLE_T;


/* Network type */
typedef enum _ENUM_NETWORK_TYPE_T
{
    NETWORK_TYPE_UNKNOWN    = 0,
    NETWORK_TYPE_INFRA      = BIT(16),
    NETWORK_TYPE_P2P        = BIT(17),
    NEWTORK_TYPE_IBSS       = BIT(18),
    NETWORK_TYPE_MESH       = BIT(19),
    NETWORK_TYPE_BOW        = BIT(20),

    NETWORK_TYPE_MASK       = BITS(16,31)
} ENUM_NETWORK_TYPE_T;

/* Connection type */
#define CONNECTION_INFRA_STA		    (STA_ROLE_STA|NETWORK_TYPE_INFRA)
#define CONNECTION_INFRA_AP		        (STA_ROLE_AP|NETWORK_TYPE_INFRA)
#define CONNECTION_P2P_GC			    (STA_ROLE_STA|NETWORK_TYPE_P2P)
#define CONNECTION_P2P_GO			    (STA_ROLE_AP|NETWORK_TYPE_P2P)
#define CONNECTION_MESH_STA		        (STA_ROLE_STA|NETWORK_TYPE_MESH)
#define CONNECTION_MESH_AP		        (STA_ROLE_AP|NETWORK_TYPE_MESH)
#define CONNECTION_BOW_CLIENT		    (STA_ROLE_STA|NETWORK_TYPE_BOW)
#define CONNECTION_BOW_AP		        (STA_ROLE_AP|NETWORK_TYPE_BOW)
#define CONNECTION_IBSS_ADHOC		    (STA_ROLE_ADHOC|NEWTORK_TYPE_IBSS)
#define CONNECTION_TDLS			        (STA_ROLE_TDLS|NETWORK_TYPE_INFRA)

/* station type check MARCO */
#define IS_STA_INFRA_TYPE(_prStaRec)    ((_prStaRec->ePeerStaType) & NETWORK_TYPE_INFRA)
#define IS_STA_P2P_TYPE(_prStaRec)      ((_prStaRec->ePeerStaType) & NETWORK_TYPE_P2P)
#define IS_STA_BOW_TYPE(_prStaRec)      ((_prStaRec->ePeerStaType) & NETWORK_TYPE_BOW)
#define IS_ADHOC_STA(_prStaRec)         ((_prStaRec->ePeerStaType) & NEWTORK_TYPE_IBSS)
#define IS_CLIENT_STA(_prStaRec)        ((_prStaRec->ePeerStaType) & STA_ROLE_STA)
#define IS_AP_STA(_prStaRec)            ((_prStaRec->ePeerStaType) & STA_ROLE_AP)
#define IS_DLS_STA(_prStaRec)           ((_prStaRec->ePeerStaType) & STA_ROLE_TDLS)

/* Main bss type */
#define MAIN_BSS_TYPE                   (NETWORK_TYPE_INFRA | \
                                         NETWORK_TYPE_P2P | \
                                         NETWORK_TYPE_BOW | \
                                         NEWTORK_TYPE_IBSS)


/*
 * The ENUM_DEV_OP_TYPE_T accounts for ENUM_NETWORK_TYPE_T and ENUM_STA_ROLE_INDEX_T.
 * It is a merged version of Network Type and STA Role.
 */
typedef enum _ENUM_DEV_OP_TYPE_T
{
    DEV_OP_TYPE_UNKNOWN = 0,
    DEV_OP_TYPE_INFRA_AP       = CONNECTION_INFRA_AP,
    DEV_OP_TYPE_INFRA_STA      = CONNECTION_INFRA_STA,
    DEV_OP_TYPE_ADHOC_IBSS     = CONNECTION_IBSS_ADHOC,
    DEV_OP_TYPE_P2P_GO         = CONNECTION_P2P_GO,
    DEV_OP_TYPE_P2P_GC         = CONNECTION_P2P_GC,
    DEV_OP_TYPE_BOW_AP         = CONNECTION_BOW_AP,
    DEV_OP_TYPE_BOW_CLIENT     = CONNECTION_BOW_CLIENT,
    DEV_OP_TYPE_DLS_PEER       = CONNECTION_TDLS
} ENUM_DEV_OP_TYPE_T, *P_ENUM_DEV_OP_TYPE_T;




typedef enum  _ENUM_STA_STATE_T
{

     STA_STATE_UNKNOWN = 0,
     STA_STATE_SU,
     STA_STATE_MU,
     // put MU state below STA_STATE_SU, since we will compare STA_STATE_SU to
     // determinte we are in MU_XXX or SU state
     STA_STATE_NUM
} ENUM_STA_STATE_T, *P_ENUM_STA_STATE_T;

/*
 * The ENUM_STA_TYPE_T accounts for ENUM_NETWORK_TYPE_T and ENUM_STA_ROLE_INDEX_T.
 * It is a merged version of Network Type and STA Role.
 */
typedef enum _ENUM_STA_TYPE_T
{
    STA_TYPE_LEGACY_AP      = CONNECTION_INFRA_AP,
    STA_TYPE_LEGACY_CLIENT  = CONNECTION_INFRA_STA,
    STA_TYPE_ADHOC_PEER     = CONNECTION_IBSS_ADHOC,
    STA_TYPE_P2P_GO         = CONNECTION_P2P_GO,
    STA_TYPE_P2P_GC         = CONNECTION_P2P_GC,
    STA_TYPE_BOW_AP         = CONNECTION_BOW_AP,
    STA_TYPE_BOW_CLIENT     = CONNECTION_BOW_CLIENT,
    STA_TYPE_DLS_PEER       = CONNECTION_TDLS
} ENUM_STA_TYPE_T, *P_ENUM_STA_TYPE_T;

typedef enum _ENUM_PHY_TYPE_INDEX_T
{
    //PHY_TYPE_DSSS_INDEX,      /* DSSS PHY (clause 15) */ /* NOTE(Kevin): We don't use this now */
    PHY_TYPE_HR_DSSS_INDEX = 0, /* HR/DSSS PHY (clause 18) */
    PHY_TYPE_ERP_INDEX,         /* ERP PHY (clause 19) */
    PHY_TYPE_ERP_P2P_INDEX,     /* ERP PHY (clause 19) w/o HR/DSSS */
    PHY_TYPE_OFDM_INDEX,        /* OFDM 5 GHz PHY (clause 17) */
    PHY_TYPE_HT_INDEX,          /* HT PHY (clause 20) */
    PHY_TYPE_VHT_INDEX,
    PHY_TYPE_INDEX_NUM // 5
} ENUM_PHY_TYPE_INDEX_T, *P_ENUM_PHY_TYPE_INDEX_T;

typedef enum _ENUM_CHNL_EXT_T
{
    CHNL_EXT_SCN = 0,
    CHNL_EXT_SCA = 1,
    CHNL_EXT_RES = 2,
    CHNL_EXT_SCB = 3
} ENUM_CHNL_EXT_T, *P_ENUM_CHNL_EXT_T;

typedef enum _ENUM_CHANNEL_WIDTH_T
{
    CW_20_40MHZ = 0,
    CW_80MHZ    = 1,
    CW_160MHZ   = 2,
    CW_80P80MHZ = 3,
    CW_5MHZ     = 4,
    CW_10MHZ    = 5,
} ENUM_CHANNEL_WIDTH_T, *P_ENUM_CHANNEL_WIDTH_P;

/*
 * Provide supported channel list to other components in array format
 * Todo: ucBand is same definition with eBand (ENUM_BAND_T), but define it as a byte here to save memory
 */
typedef struct _RF_CHANNEL_INFO_T
{
    ENUM_BAND_T eBand;
    UINT_8 ucChannelNum;
} RF_CHANNEL_INFO_T, *P_RF_CHANNEL_INFO_T;

typedef struct _MAC_HEADER_INFO_T
{
    UINT_16 u2FrameCtrl;
    UINT_16 u2DurationID;
    UINT_8 aucRA[MAC_ADDR_LEN];
    UINT_8 aucTA[MAC_ADDR_LEN];
    UINT_8 aucDA[MAC_ADDR_LEN];
    UINT_8 aucSA[MAC_ADDR_LEN];
    UINT_8 aucBSSID[MAC_ADDR_LEN];
    UINT_16 u2SeqCtrl;
    UINT_16 u2QosCtrl;
    UINT_32 u4HtCtrl;
} __KAL_ATTRIB_PACKED__ MAC_HEADER_INFO_T, *P_MAC_HEADER_INFO_T;

typedef struct _DEAUTH_INFO_T
{
    UINT_8 aucRxAddr[MAC_ADDR_LEN];
    OS_SYSTIME rLastSendTime;
} DEAUTH_INFO_T, *P_DEAUTH_INFO_T;

typedef VOID (*PFN_APPEND_IE_FUNC)(P_MSDU_INFO_T);
typedef VOID (*PFN_HANDLE_IE_FUNC)(P_SW_RFB_T, P_IE_HDR_T);
typedef VOID (*PFN_VERIFY_IE_FUNC)(P_SW_RFB_T, P_IE_HDR_T, PUINT_16);
typedef UINT_32 (*PFN_CALCULATE_VAR_IE_LEN_FUNC)(UINT_8 /*ucBssIndex*/, P_STA_RECORD_T);

typedef struct _APPEND_IE_ENTRY_T
{
    UINT_16             u2EstimatedIELen;
    PFN_APPEND_IE_FUNC  pfnAppendIE;
} APPEND_IE_ENTRY_T, *P_APPEND_IE_ENTRY_T;

typedef struct _APPEND_VAR_IE_ENTRY_T
{
    UINT_16                         u2EstimatedFixedIELen; /* For Fixed Length */
    PFN_CALCULATE_VAR_IE_LEN_FUNC   pfnCalculateVariableIELen;
    PFN_APPEND_IE_FUNC              pfnAppendIE;
} APPEND_VAR_IE_ENTRY_T, *P_APPEND_VAR_IE_ENTRY_T;

typedef struct _HANDLE_IE_ENTRY_T
{
    UINT_8              ucElemID;
    PFN_HANDLE_IE_FUNC  pfnHandleIE;
} HANDLE_IE_ENTRY_T, *P_HANDLE_IE_ENTRY_T;

typedef struct _VERIFY_IE_ENTRY_T
{
    UINT_8              ucElemID;
    PFN_VERIFY_IE_FUNC  pfnVarifyIE;
} VERIFY_IE_ENTRY_T, *P_VERIFY_IE_ENTRY_T;

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
/* Macros to get and set the wireless LAN frame fields those are 16/32 bits in length */
#define WLAN_GET_FIELD_16(_memAddr_p, _value_p) \
{ \
    PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
    *(PUINT_16)(_value_p) = ((UINT_16) __cp[0]) | ((UINT_16) __cp[1] << 8); \
}

#define WLAN_GET_FIELD_BE16(_memAddr_p, _value_p) \
{ \
    PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
    *(PUINT_16)(_value_p) = ((UINT_16) __cp[0] << 8) | ((UINT_16) __cp[1]); \
}

#define WLAN_GET_FIELD_BE32(_memAddr_p, _value_p) \
{ \
    PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
    *(PUINT_32)(_value_p) = ((UINT_32) __cp[0] << 24) | ((UINT_32) __cp[1] << 16) | \
                            ((UINT_32) __cp[2] <<  8) | ((UINT_32) __cp[3]) ; \
}

#define WLAN_GET_FIELD_32(_memAddr_p, _value_p) \
{ \
    PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
    *(PUINT_32)(_value_p) = ((UINT_32) __cp[0])       | ((UINT_32) __cp[1] << 8) | \
                            ((UINT_32) __cp[2] << 16) | ((UINT_32) __cp[3] << 24); \
}

#define WLAN_GET_FIELD_64(_memAddr_p, _value_p) \
{ \
    PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
    *(PUINT_64)(_value_p) = \
        ((UINT_64) __cp[0])       | ((UINT_64) __cp[1] << 8)  | \
        ((UINT_64) __cp[2] << 16) | ((UINT_64) __cp[3] << 24) | \
        ((UINT_64) __cp[4] << 32) | ((UINT_64) __cp[5] << 40) | \
        ((UINT_64) __cp[6] << 48) | ((UINT_64) __cp[7] << 56); \
}

#define WLAN_SET_FIELD_BE16(_memAddr_p, _value)            \
{ \
    PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
    __cp[0] = (UINT_8) ((_value) >> 8); \
    __cp[1] = (UINT_8) (_value); \
}

#define WLAN_SET_FIELD_16(_memAddr_p, _value) \
{ \
    PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
    __cp[0] = (UINT_8) (_value); \
    __cp[1] = (UINT_8) ((_value) >> 8); \
}

#define WLAN_SET_FIELD_32(_memAddr_p, _value) \
{ \
    PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
    __cp[0] = (UINT_8) (_value); \
    __cp[1] = (UINT_8) ((_value) >> 8); \
    __cp[2] = (UINT_8) ((_value) >> 16); \
    __cp[3] = (UINT_8) ((_value) >> 24); \
}

#define WLAN_SET_FIELD_BE32(_memAddr_p, _value) \
{ \
    PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
    __cp[0] = (UINT_8) ((_value) >> 24); \
    __cp[1] = (UINT_8) ((_value) >> 16); \
    __cp[2] = (UINT_8) ((_value) >>  8); \
    __cp[3] = (UINT_8) (_value); \
}

#define WLAN_SET_FIELD_64(_memAddr_p, _value) \
{ \
    PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
    __cp[0] = (UINT_8) (_value); \
    __cp[1] = (UINT_8) ((_value) >> 8); \
    __cp[2] = (UINT_8) ((_value) >> 16); \
    __cp[3] = (UINT_8) ((_value) >> 24); \
    __cp[4] = (UINT_8) ((_value) >> 32); \
    __cp[5] = (UINT_8) ((_value) >> 40); \
    __cp[6] = (UINT_8) ((_value) >> 48); \
    __cp[7] = (UINT_8) ((_value) >> 56); \
}

#define WLAN_SET_FIELD_BE64(_memAddr_p, _value) \
{ \
    PUINT_8 __cp = (PUINT_8) (_memAddr_p); \
    __cp[0] = (UINT_8) ((_value) >> 56); \
    __cp[1] = (UINT_8) ((_value) >> 48); \
    __cp[2] = (UINT_8) ((_value) >> 40); \
    __cp[3] = (UINT_8) ((_value) >> 32); \
    __cp[4] = (UINT_8) ((_value) >> 24); \
    __cp[5] = (UINT_8) ((_value) >> 16); \
    __cp[6] = (UINT_8) ((_value) >>  8); \
    __cp[7] = (UINT_8) (_value); \
}

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif  /* _WLAN_DEF_H */

