/*
** $Id:
*/

/*! \file   "mac.h"
    \brief  Brief description.

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
** $Log: mac.h $
**
** 07 24 2015 tiger.hung
** [BORA00004558] [MT7637][E2] Checkin
** Sync E1 Ram code(CL28812) to E2
**
** 07 15 2014 tiger.hung
** [BORA00003241] [MT7636] sync MT7636 E1 MP source code
**
** 01 08 2014 ted.huang
** [BORA00002807] [MT7636]
** Merge 6630E2 RAM code.
**
** 12 09 2013 tsaiyuan.hsu
** [BORA00002911] MT6630 unified MAC RTT
** Add rtt module for timing measureemnt.
**
** 10 01 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** the change for new ar
**
** 08 08 2013 jeffrey.chang
** [BORA00002383] [WIFISYS][6630] HEM/PM development
** 1) fix the bug of partial virtual bitmap (AP mode)
**
** 07 29 2013 yuche.tsai
** [BORA00000343] [MT6620] Emulation For TX
** Bug fix for beacon frame TIM format.
**
** 07 08 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** Fix the rate index (CCK, OFDM)
**
** 07 05 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** Update LMAC timing CR for S4 FPGA
**
** 07 03 2013 jeffrey.chang
** [BORA00002383] [WIFISYS][6630] HEM/PM development
** add LP advanced feature to trunk
**
** 06 19 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** Tx AMPDU implementation
**
** 06 14 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** 1. Add initial rate seting. 2. Add more information in sta command 3. add VHT info
**
** 06 04 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** .
**
** 05 22 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** 1. Add Timing Advertisment frame type definition
** 2. Add WTBL PCR/PCSR defintion
** 3. Add RX Vectore other antenna bit definition
**
** 05 13 2013 bruce.kang
** [BORA00002409] [MT6630 Wi-Fi]
** 1. Add interfaces between WMT and Wi-Fi calibration module (rlm)
** 2. Add Rom to Ram API
** 3. Add debug count in bcm module
** 4. Add TX CF-End in bss module
**
** 04 30 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** update rlm statistic and rlm update wtbl function
**
** 04 01 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** 1.Update BSSID basic reate HAL function
** 2.Change emulTX packet size from 2000 to 1600 bytes
** 3.Synth. emulation TX test cases
**
** 03 13 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** Add utility function for frame/QoS control field
**
** 03 04 2013 pat.lu
** [BORA00002365] [WLAN] Sec. and WTBL FPGA Emulation Modification
** .
** 1. Modify and correct RX filter cases and SW RF filter criteria.
** 2. Modify CTRL frame reserved sub-type defintion. 
** (For 11ac, it adds two new CTRL frames: BF Report Poll, and VHT NDP Announcement)
**
** 12 12 2012 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** .
**
** 11 22 2012 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Update RMAC emulation code
** 
** 10 22 2012 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** update ram code for rlm
** 
** 10 04 2012 yarco.yang
** [BORA00002189] [MT6630]Synch. Davinc 20120927 FW to Bora
** .
 *
 * 05 21 2012 cm.chang
 * NULL
 * Do exception handle for illegal IE instead of original assertion checking
 *
 * 11 02 2011 chinghwa.yu
 * [WCXRP00000612] [MT6620 Wi-Fi] [FW] CSD update SWRDD algorithm
 * Fixed typo.
 *
 * 11 02 2011 chinghwa.yu
 * [WCXRP00000612] [MT6620 Wi-Fi] [FW] CSD update SWRDD algorithm
 * Add RDD DFS certification features.
 *
 * 10 12 2011 tsaiyuan.hsu
 * [WCXRP00000966] [MT6620 Wi-Fi][FW] Add auto bandwidth support
 * update and enable auto bandwidth.
 *
 * 09 29 2011 cm.chang
 * NULL
 * 1. Enlarge max operation channel list in scan module
 * 2. Change function prototype of rlmDomainGetChnlList
 *
 * 08 02 2011 terry.wu
 * [WCXRP00000891] [MT5931][MCUSYS] move code to SYSRAM to enable ASSERT_DUMP
 * Replace ASSERT check of linking list and queue macro with LITE_ASSERT.
 *
 * 06 27 2011 wh.su
 * [WCXRP00000817] [MT6620 Wi-Fi][FW] Add check for drop rx non-encrypted packets at security-based setting
 * add check for non-encrypted packet and drop for security-based network setting.
 *
 * 05 17 2011 yuche.tsai
 * [WCXRP00000729] [Volunteer Patch][MT6620][FW] Fix Klocwork Issue.
 * Fix clock work issue.
 *
 * 04 15 2011 cm.chang
 * [WCXRP00000634] [MT6620 Wi-Fi][Driver][FW] 2nd BSS will not support 40MHz bandwidth for concurrency
 * 1. Support PCO action frame
 * 2. Not pass Beacon to host if not in scanning and module decision
 *
 * 01 25 2011 george.huang
 * [WCXRP00000152] [MT6620 Wi-Fi] AP mode power saving function
 * 1. support (in-)activate PS profile function
 * 2. design enhancement for NOA timing determination for GC
 * 3. support NOA action frame parsing
 * 4. modify absence req handler, for GO degrade to AP due to legacy client connection
 *
 * 12 29 2010 puff.wen
 * NULL
 * .
 *
 * 12 10 2010 yarco.yang
 * [WCXRP00000249] [MT6620 Wi-Fi][FW/Driver]FIxed Clockwork warning message
 * .
 *
 * 07 27 2010 george.huang
 * NULL
 * 1. Add P2P IE processing by received beacon in FW - for P2P PM
 *
 * 2. process NOA attribute by processed P2P IE
 *
 * 07 12 2010 cp.wu
 *
 * commit Wi-Fi firmware part for MT6620
 *
 * 05 14 2010 kevin.huang
 * [BORA00000794][WIFISYS][New Feature]Power Management Support
 * Fix compile warning - macro > 10 line, initial value of an array
 *
 * 05 12 2010 kevin.huang
 * [BORA00000794][WIFISYS][New Feature]Power Management Support
 * Add Power Management - Legacy PS-POLL support.
 *
 * 05 05 2010 cm.chang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * First draft support for 20/40M bandwidth for AP mode
 *
 * 04 22 2010 cm.chang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * First draft code to support protection in AP mode
 *
 * 04 19 2010 kevin.huang
 * [BORA00000714][WIFISYS][New Feature]Beacon Timeout Support
 * Add Beacon Timeout Support
 *  *  *  *  *  and will send Null frame to diagnose connection
 *
 * 04 09 2010 tehuang.liu
 * [BORA00000605][WIFISYS] Phase3 Integration
 * .
 *
 * 04 01 2010 cm.chang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * Modify MFB code to send response frame
 *
 * 03 30 2010 cm.chang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * Support 2.4G OBSS scan
 *
 * 03 15 2010 cm.chang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * Provide draft measurement and quiet functions
 *
 * 03 03 2010 cm.chang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * Move default value of HT capability to rlm.h
 *
 * 03 02 2010 cm.chang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * PCO transition time is set to 0 from 400us
 *
 * 02 23 2010 kevin.huang
 * [BORA00000603][WIFISYS] [New Feature] AAA Module Support
 * Add define for DTIM
 *
 * 02 22 2010 tehuang.liu
 * [BORA00000569][WIFISYS] Phase 2 Integration Test
 * The status code in the WMM ADDTS_RSP frame is of one byte length.
 *
 * 02 21 2010 tehuang.liu
 * [BORA00000569][WIFISYS] Phase 2 Integration Test
 * Corrected the prefixes of the fields of struct ACTION_ADDTS_REQ_FRAME
 *
 * 02 21 2010 tehuang.liu
 * [BORA00000569][WIFISYS] Phase 2 Integration Test
 * Added VENDOR_OUI_SUBTYPE_WMM_TSPEC
 *
 * 02 13 2010 tehuang.liu
 * [BORA00000569][WIFISYS] Phase 2 Integration Test
 * Added 7.3.2.30 TSPEC element related masks and offsets
 *
 * 02 12 2010 cm.chang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * Define public and radio measurement action frames
 *
 * 02 12 2010 tehuang.liu
 * [BORA00000569][WIFISYS] Phase 2 Integration Test
 * Added QoS Action frame related definitions
 *
 * 01 27 2010 wh.su
 * [BORA00000476][Wi-Fi][firmware] Add the security module initialize code
 * add and fixed some security function.
 *
 * 01 13 2010 tehuang.liu
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * .
 *
 * 01 06 2010 tehuang.liu
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * Modified BA agreement handling FSM code
 *
 * 01 04 2010 js.wan
 * [BORA00000374]Check in A-MPDU test code
 * Add __packed for +HTC frame.
 *
 * 01 04 2010 tehuang.liu
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * For working out the first connection Chariot-verified version
 *
 * 12 29 2009 yuche.tsai
 * [BORA00000343][MT6620] Emulation For TX
 * .u1rwduu`wvpghlqg||pfm`+qvdl
 *
 * 12 25 2009 tehuang.liu
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * Integrated modifications for 1st connection (mainly on FW modules MQM, TXM, and RXM)
 *  *  * MQM: BA handling
 *  *  * TXM: Macros updates
 *  *  * RXM: Macros/Duplicate Removal updates
 *
 * Dec 12 2009 mtk01104
 * [BORA00000018] Integrate WIFI part into BORA for the 1st time
 * Define ELEM_MAX_LEN_HT_CAP as payload length
 *
 * Dec 11 2009 mtk01426
 * [BORA00000018] Integrate WIFI part into BORA for the 1st time
 * Add WLAN_MAC_MGMT_HEADER_HT_T data structure
 *
 * Dec 9 2009 MTK02468
 * [BORA00000337] To check in codes for FPGA emulation
 * Added ADDBA_REQ/RSP frame structures
 *
 * Dec 9 2009 mtk01104
 * [BORA00000018] Integrate WIFI part into BORA for the 1st time
 * Add HT related IE structure
 *
 * Dec 7 2009 mtk01088
 * [BORA00000476] [Wi-Fi][firmware] Add the security module initialize code
 * adding the max wapi ie length define
 *
 * Dec 4 2009 mtk01088
 * [BORA00000476] [Wi-Fi][firmware] Add the security module initialize code
 * adding the wpa ie max length define for assoc req
 *
 * Dec 2 2009 MTK02468
 * [BORA00000337] To check in codes for FPGA emulation
 * Added ACTION_ADDBA_RSP_FRAME_T and ACTION_ADDBA_REQ_FRAME_T
 *
 * Nov 30 2009 mtk01230
 * [BORA00000374] Check in A-MPDU test code
 *
 *
 * Nov 29 2009 MTK02468
 * [BORA00000337] To check in codes for FPGA emulation
 * Added Ethernet packet max/min size definitions
 *
 * Nov 25 2009 mtk01725
 * [BORA00000343] [MT6620] Emulation For TX
 *
*/

#ifndef _MAC_H
#define _MAC_H
#include "debug.h"
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
//3 /* --------------- Constants for Ethernet/802.11 MAC --------------- */
/* MAC Address */
#define MAC_ADDR_LEN                            6

#define MAC_ADDR_LOCAL_ADMIN                    BIT(1)

#define ETH_P_IPV4                              0x0800
#define ETH_P_IPX                               0x8137 // Novell IPX
#define ETH_P_AARP                              0x80F3 // AppleTalk Address Resolution Protocol (AARP)
#define ETH_P_IPV6                              0x86DD
#define ETH_P_VLAN                              0x8100

#define ETH_P_1X                                0x888e // 802.1x
#define ETH_P_WAPI_1X                           0x88b4 // WAPI 802.1x

#define IP_VERSION_4                            4
#define IP_VERSION_6                            6

#define IP_PROTOCOL_TCP                         6
#define IP_PROTOCOL_UDP                         17

#define IPV4_HDR_IP_PROTOCOL_OFFSET             9
#define IPV4_HDR_IP_CSUM_OFFSET                 10

#define IPV6_HDR_IP_PROTOCOL_OFFSET             6

#define TCP_HDR_TCP_CSUM_OFFSET                 16
#define UDP_HDR_UDP_CSUM_OFFSET                 6

#define LLC_LEN                                 8 // LLC(3) + SNAP(3) + EtherType(2)

#define NULL_MAC_ADDR                           {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define BC_MAC_ADDR                             {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}

/* Ethernet Frame Field Size, in byte */
#define ETHER_HEADER_LEN                        14
#define ETHER_TYPE_LEN                          2
#define ETHER_MIN_PKT_SZ                        60
#define ETHER_MAX_PKT_SZ                        1514

/* IEEE 802.11 WLAN Frame Field Size, in byte */
#define WLAN_MAC_HEADER_LEN                     24 /* Address 4 excluded */
#define WLAN_MAC_HEADER_A4_LEN                  30 /* Address 4 included */
#define WLAN_MAC_HEADER_QOS_LEN                 26 /* QoS Control included */
#define WLAN_MAC_HEADER_QOS_HTC_LEN             30 /* QoS Control and HTC included */
#define WLAN_MAC_HEADER_A4_QOS_LEN              32 /* Address 4 and QoS Control included */
#define WLAN_MAC_HEADER_A4_QOS_HTC_LEN          36 /* Address 4, QoS Control and HTC included */
#define WLAN_MAC_MGMT_HEADER_LEN                24 /* Address 4 excluded */
#define WLAN_MAC_MGMT_HEADER_HTC_LEN            28 /* HTC included */

#define QOS_CTRL_LEN                            2
#define HT_CTRL_LEN                             4

#define WLAN_MAC_CTS_ACK_LEN         WLAN_MAC_CTS_ACK_FRAME_HEADER_LEN + FCS_LEN

/* 6.2.1.1.2 Semantics of the service primitive */
#define MSDU_MAX_LENGTH                         2304

/* 7.1.3.3.3 Broadcast BSSID */
#define BC_BSSID                                BC_MAC_ADDR

/* 7.1.3.7 FCS field */
#define FCS_LEN                                 4

/* 7.3.1.6 Listen Interval field */
#define DEFAULT_LISTEN_INTERVAL_BY_DTIM_PERIOD  2 // In unit of AP's DTIM interval,
#define DEFAULT_LISTEN_INTERVAL                 10

/* 7.3.2.1 Broadcast(Wildcard) SSID */
#define BC_SSID                                 ""
#define BC_SSID_LEN                             0

/* 7.3.2.2 Data Rate Value */
#define RATE_1M                                 2   /* 1M in unit of 500kb/s */
#define RATE_2M                                 4   /* 2M */
#define RATE_5_5M                               11  /* 5.5M */
#define RATE_11M                                22  /* 11M */
#define RATE_22M                                44  /* 22M */
#define RATE_33M                                66  /* 33M */
#define RATE_6M                                 12  /* 6M */
#define RATE_9M                                 18  /* 9M */
#define RATE_12M                                24  /* 12M */
#define RATE_18M                                36  /* 18M */
#define RATE_24M                                48  /* 24M */
#define RATE_36M                                72  /* 36M */
#define RATE_48M                                96  /* 48M */
#define RATE_54M                                108 /* 54M */
/* 7.3.2.14 BSS membership selector */
#define RATE_HT_PHY                             127 /* BSS Selector - Clause 20. HT PHY */
#define RATE_MASK                               BITS(0,6)   /* mask bits for the rate */
#define RATE_BASIC_BIT                          BIT(7)      /* mask bit for the rate belonging to the BSSBasicRateSet */

/* 8.3.2.2 TKIP MPDU formats */
#define TKIP_MIC_LEN                            8

/* 9.2.10 DIFS */
#define DIFS                                    2   /* 2 x aSlotTime */

/* 11.3 STA Authentication and Association */
#define STA_STATE_1                             0 /* Accept Class 1 frames */
#define STA_STATE_2                             1 /* Accept Class 1 & 2 frames */
#define STA_STATE_3                             2 /* Accept Class 1,2 & 3 frames */

/* 15.4.8.5 802.11k RCPI-dBm mapping*/
#define NDBM_LOW_BOUND_FOR_RCPI                 110
#define RCPI_LOW_BOUND                          0
#define RCPI_HIGH_BOUND                         220
#define RCPI_MEASUREMENT_NOT_AVAILABLE          255


/* PHY characteristics */
/* 17.4.4/18.3.3/19.8.4 Slot Time (aSlotTime) */
#if CFG_MAC_2US_TIMING_UNIT
#define SLOT_TIME_LONG                          10  /* Long Slot Time */
#define SLOT_TIME_SHORT                         4   /* Short Slot Time */
#else
#define SLOT_TIME_LONG                          20  /* Long Slot Time */
#define SLOT_TIME_SHORT                         9   /* Short Slot Time */
#endif

#define SLOT_TIME_HR_DSSS                       SLOT_TIME_LONG  /* 802.11b aSlotTime */
#define SLOT_TIME_OFDM                          SLOT_TIME_SHORT /* 802.11a aSlotTime(20M Spacing) */
#define SLOT_TIME_OFDM_10M_SPACING              13              /* 802.11a aSlotTime(10M Spacing) */
#define SLOT_TIME_ERP_LONG                      SLOT_TIME_LONG  /* 802.11g aSlotTime(Long) */
#define SLOT_TIME_ERP_SHORT                     SLOT_TIME_SHORT /* 802.11g aSlotTime(Short) */

/* 17.4.4/18.3.3/19.8.4 Contention Window (aCWmin & aCWmax) */
#define CWMIN_OFDM                              15      /* 802.11a aCWmin */
#define CWMAX_OFDM                              1023    /* 802.11a aCWmax */

#define CWMIN_HR_DSSS                           31      /* 802.11b aCWmin */
#define CWMAX_HR_DSSS                           1023    /* 802.11b aCWmax */

#define CWMIN_ERP_0                             31      /* 802.11g aCWmin(0) - for only have 1/2/5/11Mbps Rates */
#define CWMIN_ERP_1                             15      /* 802.11g aCWmin(1) */
#define CWMAX_ERP                               1023    /* 802.11g aCWmax */

/* Short Inter-Frame Space (aSIFSTime) */
/* 15.3.3 802.11b aSIFSTime */
#define SIFS_TIME_HR_DSSS                       10
/* 17.4.4 802.11a aSIFSTime */
#define SIFS_TIME_OFDM                          16
/* 19.8.4 802.11g aSIFSTime */
#define SIFS_TIME_ERP                           10

/* 15.4.6.2 Number of operating channels */
#define CH_1                                    0x1
#define CH_2                                    0x2
#define CH_3                                    0x3
#define CH_4                                    0x4
#define CH_5                                    0x5
#define CH_6                                    0x6
#define CH_7                                    0x7
#define CH_8                                    0x8
#define CH_9                                    0x9
#define CH_10                                   0xa
#define CH_11                                   0xb
#define CH_12                                   0xc
#define CH_13                                   0xd
#define CH_14                                   0xe

#if CFG_SUPPORT_5G_BAND || CFG_SUPPORT_4G_BAND

#if CFG_SUPPORT_RDM
    #define MAXIMUM_OPERATION_CHANNEL_LIST     54
#else
    #define MAXIMUM_OPERATION_CHANNEL_LIST     46
#endif

#else
    #define MAXIMUM_OPERATION_CHANNEL_LIST      14
#endif

//3 /* --------------- IEEE 802.11 PICS --------------- */
/* Annex D - dot11OperationEntry 2 */
#define DOT11_RTS_THRESHOLD_MIN                 0
#define DOT11_RTS_THRESHOLD_MAX                 2347 // from Windows DDK
//#define DOT11_RTS_THRESHOLD_MAX                 3000 // from Annex D

#define DOT11_RTS_THRESHOLD_DEFAULT             \
            DOT11_RTS_THRESHOLD_MAX

/* Annex D - dot11OperationEntry 5 */
#define DOT11_FRAGMENTATION_THRESHOLD_MIN       256
#define DOT11_FRAGMENTATION_THRESHOLD_MAX       2346 // from Windows DDK
//#define DOT11_FRAGMENTATION_THRESHOLD_MAX       3000 // from Annex D

#define DOT11_FRAGMENTATION_THRESHOLD_DEFAULT   \
            DOT11_FRAGMENTATION_THRESHOLD_MAX

/* Annex D - dot11OperationEntry 6 */
#define DOT11_TRANSMIT_MSDU_LIFETIME_TU_MIN     1
#define DOT11_TRANSMIT_MSDU_LIFETIME_TU_MAX     0xFFFFffff
#define DOT11_TRANSMIT_MSDU_LIFETIME_TU_DEFAULT 4095 // 802.11 define 512
                                                     // MT5921 only aceept N <= 4095

/* Annex D - dot11OperationEntry 7 */
#define DOT11_RECEIVE_LIFETIME_TU_MIN           1
#define DOT11_RECEIVE_LIFETIME_TU_MAX           0xFFFFffff
#define DOT11_RECEIVE_LIFETIME_TU_DEFAULT       4096 // 802.11 define 512

/* Annex D - dot11StationConfigEntry 12 */
#define DOT11_BEACON_PERIOD_MIN                 1 // TU.
#define DOT11_BEACON_PERIOD_MAX                 0xffff // TU.
#define DOT11_BEACON_PERIOD_DEFAULT             100 // TU.

/* Annex D - dot11StationConfigEntry 13 */
#define DOT11_DTIM_PERIOD_MIN                   1 // TU.
#define DOT11_DTIM_PERIOD_MAX                   255 // TU.
#define DOT11_DTIM_PERIOD_DEFAULT               1 // TU.

/* Annex D - dot11RegDomainsSupportValue */
#define REGULATION_DOMAIN_FCC                   0x10        /* FCC (US) */
#define REGULATION_DOMAIN_IC                    0x20        /* IC or DOC (Canada) */
#define REGULATION_DOMAIN_ETSI                  0x30        /* ETSI (Europe) */
#define REGULATION_DOMAIN_SPAIN                 0x31        /* Spain */
#define REGULATION_DOMAIN_FRANCE                0x32        /* France */
#define REGULATION_DOMAIN_JAPAN                 0x40        /* MKK (Japan) */
#define REGULATION_DOMAIN_CHINA                 0x50        /* China */
#define REGULATION_DOMAIN_OTHER                 0x00        /* Other */



//3 /* --------------- IEEE 802.11 MAC header fields --------------- */
/* 7.1.3.1 Masks for the subfields in the Frame Control field */
#define MASK_FC_PROTOCOL_VER                    BITS(0,1)
#define MASK_FC_TYPE                            BITS(2,3)
#define MASK_FC_SUBTYPE                         BITS(4,7)
#define MASK_FC_SUBTYPE_CF_ACK_DATA             BIT(4)
#define MASK_FC_SUBTYPE_CF_POLL_DATA            BIT(5)
#define MASK_FC_SUBTYPE_NULL_DATA               BIT(6)
#define MASK_FC_SUBTYPE_QOS_DATA                BIT(7)
#define MASK_FC_TO_DS                           BIT(8)
#define MASK_FC_FROM_DS                         BIT(9)
#define MASK_FC_MORE_FRAG                       BIT(10)
#define MASK_FC_RETRY                           BIT(11)
#define MASK_FC_PWR_MGT                         BIT(12)
#define MASK_FC_MORE_DATA                       BIT(13)
#define MASK_FC_PROTECTED_FRAME                 BIT(14)
#define MASK_FC_ORDER                           BIT(15)

#define MASK_FRAME_TYPE                         (MASK_FC_TYPE | MASK_FC_SUBTYPE)
#define MASK_TO_DS_FROM_DS                      (MASK_FC_TO_DS | MASK_FC_FROM_DS)

#define MAX_NUM_OF_FC_SUBTYPES                  16
#define OFFSET_OF_FC_SUBTYPE                    4


/* 7.1.3.1.2 MAC frame types and subtypes */
#define MAC_FRAME_TYPE_MGT                      0
#define MAC_FRAME_TYPE_CTRL                     BIT(2)
#define MAC_FRAME_TYPE_DATA                     BIT(3)
#define MAC_FRAME_TYPE_QOS_DATA                 (MAC_FRAME_TYPE_DATA | MASK_FC_SUBTYPE_QOS_DATA)

#define MAC_FRAME_ASSOC_REQ                     (MAC_FRAME_TYPE_MGT | 0x0000)
#define MAC_FRAME_ASSOC_RSP                     (MAC_FRAME_TYPE_MGT | 0x0010)
#define MAC_FRAME_REASSOC_REQ                   (MAC_FRAME_TYPE_MGT | 0x0020)
#define MAC_FRAME_REASSOC_RSP                   (MAC_FRAME_TYPE_MGT | 0x0030)
#define MAC_FRAME_PROBE_REQ                     (MAC_FRAME_TYPE_MGT | 0x0040)
#define MAC_FRAME_PROBE_RSP                     (MAC_FRAME_TYPE_MGT | 0x0050)
#define MAC_FRAME_TIMING_ADV                    (MAC_FRAME_TYPE_MGT | 0x0060)
#define MAC_FRAME_BEACON                        (MAC_FRAME_TYPE_MGT | 0x0080)
#define MAC_FRAME_ATIM                          (MAC_FRAME_TYPE_MGT | 0x0090)
#define MAC_FRAME_DISASSOC                      (MAC_FRAME_TYPE_MGT | 0x00A0)
#define MAC_FRAME_AUTH                          (MAC_FRAME_TYPE_MGT | 0x00B0)
#define MAC_FRAME_DEAUTH                        (MAC_FRAME_TYPE_MGT | 0x00C0)
#define MAC_FRAME_ACTION                        (MAC_FRAME_TYPE_MGT | 0x00D0)
#define MAC_FRAME_ACTION_NO_ACK                 (MAC_FRAME_TYPE_MGT | 0x00E0)

#define MAC_FRAME_BF_REPORT_POLL                (MAC_FRAME_TYPE_CTRL | 0x0040)
#define MAC_FRAME_VHT_NDP_ANNOUNCE              (MAC_FRAME_TYPE_CTRL | 0x0050)
#define MAC_FRAME_CONTRL_WRAPPER                (MAC_FRAME_TYPE_CTRL | 0x0070)
#define MAC_FRAME_BLOCK_ACK_REQ                 (MAC_FRAME_TYPE_CTRL | 0x0080)
#define MAC_FRAME_BLOCK_ACK                     (MAC_FRAME_TYPE_CTRL | 0x0090)
#define MAC_FRAME_PS_POLL                       (MAC_FRAME_TYPE_CTRL | 0x00A0)
#define MAC_FRAME_RTS                           (MAC_FRAME_TYPE_CTRL | 0x00B0)
#define MAC_FRAME_CTS                           (MAC_FRAME_TYPE_CTRL | 0x00C0)
#define MAC_FRAME_ACK                           (MAC_FRAME_TYPE_CTRL | 0x00D0)
#define MAC_FRAME_CF_END                        (MAC_FRAME_TYPE_CTRL | 0x00E0)
#define MAC_FRAME_CF_END_CF_ACK                 (MAC_FRAME_TYPE_CTRL | 0x00F0)
#define MAC_FRAME_CTRL_RESERVED_0               (MAC_FRAME_TYPE_CTRL | 0x0000)
#define MAC_FRAME_CTRL_RESERVED_1               (MAC_FRAME_TYPE_CTRL | 0x0010)
#define MAC_FRAME_CTRL_RESERVED_2               (MAC_FRAME_TYPE_CTRL | 0x0020)
#define MAC_FRAME_CTRL_RESERVED_3               (MAC_FRAME_TYPE_CTRL | 0x0030)
#define MAC_FRAME_CTRL_RESERVED_6               (MAC_FRAME_TYPE_CTRL | 0x0060)



#define MAC_FRAME_DATA                          (MAC_FRAME_TYPE_DATA | 0x0000)
#define MAC_FRAME_DATA_CF_ACK                   (MAC_FRAME_TYPE_DATA | 0x0010)
#define MAC_FRAME_DATA_CF_POLL                  (MAC_FRAME_TYPE_DATA | 0x0020)
#define MAC_FRAME_DATA_CF_ACK_CF_POLL           (MAC_FRAME_TYPE_DATA | 0x0030)
#define MAC_FRAME_NULL                          (MAC_FRAME_TYPE_DATA | 0x0040)
#define MAC_FRAME_CF_ACK                        (MAC_FRAME_TYPE_DATA | 0x0050)
#define MAC_FRAME_CF_POLL                       (MAC_FRAME_TYPE_DATA | 0x0060)
#define MAC_FRAME_CF_ACK_CF_POLL                (MAC_FRAME_TYPE_DATA | 0x0070)
#define MAC_FRAME_QOS_DATA                      (MAC_FRAME_TYPE_DATA | 0x0080)
#define MAC_FRAME_QOS_DATA_CF_ACK               (MAC_FRAME_TYPE_DATA | 0x0090)
#define MAC_FRAME_QOS_DATA_CF_POLL              (MAC_FRAME_TYPE_DATA | 0x00A0)
#define MAC_FRAME_QOS_DATA_CF_ACK_CF_POLL       (MAC_FRAME_TYPE_DATA | 0x00B0)
#define MAC_FRAME_QOS_NULL                      (MAC_FRAME_TYPE_DATA | 0x00C0)
#define MAC_FRAME_QOS_CF_POLL                   (MAC_FRAME_TYPE_DATA | 0x00E0)
#define MAC_FRAME_QOS_CF_ACK_CF_POLL            (MAC_FRAME_TYPE_DATA | 0x00F0)

/* 7.1.3.2 Mask for the AID value in the Duration/ID field */
#define MASK_DI_DURATION                        BITS(0,14)
#define MASK_DI_AID                             BITS(0,13)
#define MASK_DI_AID_MSB                         BITS(14,15)
#define MASK_DI_CFP_FIXED_VALUE                 BIT(15)

/* 7.1.3.4 Masks for the subfields in the Sequence Control field */
#define MASK_SC_SEQ_NUM                         BITS(4,15)
#define MASK_SC_SEQ_NUM_OFFSET                       4
#define MASK_SC_FRAG_NUM                        BITS(0,3)
#define INVALID_SEQ_CTRL_NUM                    0x000F /* According to 6.2.1.1.2
                                                         * FRAG_NUM won't equal to 15
                                                         */

/* 7.1.3.5 QoS Control field */
#define TID_NUM                                 16
#if 1
#define TID_MASK                                BITS(0,3)
#define EOSP                                    BIT(4)
#define ACK_POLICY                              BITS(5,6)
#define A_MSDU_PRESENT                          BIT(7)
#endif

#define MASK_QC_TID                  BITS(0,3)
#define MASK_QC_EOSP                 BIT(4)
#define MASK_QC_EOSP_OFFSET          4
#define MASK_QC_ACK_POLICY           BITS(5, 6)
#define MASK_QC_ACK_POLICY_OFFSET    5
#define MASK_QC_A_MSDU_PRESENT       BIT(7)
#define MASK_QC_MESH_CONTROL_PRESENT BIT(8)


/* 7.1.3.5a HT Control field */
#define HT_CTRL_VHT_VARIANT                     BIT(0)
#define HT_CTRL_VARIANT_FIELD                   BITS(1,29)

#define HT_CTRL_LINK_ADAPTATION_CTRL            BITS(0,15)
#define HT_CTRL_CALIBRATION_POSITION            BITS(16,17)
#define HT_CTRL_CALIBRATION_SEQUENCE            BITS(18,19)
#define HT_CTRL_CSI_STEERING                    BITS(22,23)
#define HT_CTRL_NDP_ANNOUNCEMENT                BIT(24)
#define HT_CTRL_AC_CONSTRAINT                   BIT(30)
#define HT_CTRL_RDG_MORE_PPDU                   BIT(31)

#define LINK_ADAPTATION_CTRL_TRQ                BIT(1)
#define LINK_ADAPTATION_CTRL_MAI_MRQ            BIT(2)
#define LINK_ADAPTATION_CTRL_MAI_MSI            BITS(3,5)
#define LINK_ADAPTATION_CTRL_MFSI               BITS(6,8)
#define LINK_ADAPTATION_CTRL_MFB_ASELC_CMD      BITS(9,11)
#define LINK_ADAPTATION_CTRL_MFB_ASELC_DATA     BITS(12,15)

#define VHT_VARIANT_MFB_NUM_STS                 BITS(9,11)
#define VHT_VARIANT_MFB_MCS                     BITS(12,15)
#define VHT_VARIANT_MFB_BW                      BITS(16,17)
#define VHT_VARIANT_MFB_SNR                     BITS(18,23)
#define VHT_VARIANT_GID_H                       BITS(24,26)
#define VHT_VARIANT_CODING_TYPE                 BIT(27)
#define VHT_VARIANT_FB_TX_TYPE                  BIT(28)
#define VHT_VARIANT_UNSOLICITED_MFB             BIT(29)

/* 7.1.3.5.3 Ack Policy subfield*/
#define ACK_POLICY_NORMAL_ACK_IMPLICIT_BA_REQ 0
#define ACK_POLICY_NO_ACK 1
#define ACK_POLICY_NO_EXPLICIT_ACK_PSMP_ACK 2
#define ACK_POLICY_BA 3

/* 7.1.3.7 FCS field */
#define FCS_LEN                                 4

/* 7.2.1.4 WLAN Control Frame - PS-POLL Frame */
#define CTS_FRAME_LEN                           10 /* w/o FCS */
#define PSPOLL_FRAME_LEN                        16 /* w/o FCS */
#define CFEND_FRAME_LEN                         16 /* w/o FCS */

/* 8.3.2.2 TKIP MPDU formats */
#define TKIP_MIC_LEN                            8

/* 2009.11.30 mtk02468: Moved these definitions to the right place */
#if 0
/* Block Ack Parameter Set field */
#define BA_PARM_BA_POLICY                       BIT(1)
#define BA_PARM_TID                             BITS(2,5)
#define BA_PARM_BUFFER_SIZE                     BITS(6,15)
#endif

#define BA_POLICY_IMMEDIATE                     BIT(1)

/* Block Ack Starting Sequence Control field */
#define BA_START_SEQ_CTL_FRAG_NUM               BITS(0,3)
#define BA_START_SEQ_CTL_SSN                    BITS(4,15)

/* BAR Control field */
#define BAR_CONTROL_NO_ACK_POLICY               BIT(0)
#define BAR_CONTROL_MULTI_TID                   BIT(1)
#define BAR_CONTROL_COMPRESSED_BA               BIT(2)
#define BAR_CONTROL_TID_INFO                    BITS(12,15)
#define BAR_CONTROL_TID_INFO_OFFSET             12

/* TID Value */
#define BAR_INFO_TID_VALUE                      BITS(12,15)

#define BAR_COMPRESSED_VARIANT_FRAME_LEN        (16 + 4)

//3 /* --------------- IEEE 802.11 frame body fields --------------- */
//3 Management frame body components (I): Fixed Fields.
/* 7.3.1.1 Authentication Algorithm Number field */
#define AUTH_ALGORITHM_NUM_FIELD_LEN                2

#define AUTH_ALGORITHM_NUM_OPEN_SYSTEM              0   /* Open System */
#define AUTH_ALGORITHM_NUM_SHARED_KEY               1   /* Shared Key */
#define AUTH_ALGORITHM_NUM_FAST_BSS_TRANSITION      2   /* Fast BSS Transition */

/* 7.3.1.2 Authentication Transaction Sequence Number field */
#define AUTH_TRANSACTION_SEQENCE_NUM_FIELD_LEN      2
#define AUTH_TRANSACTION_SEQ_1                      1
#define AUTH_TRANSACTION_SEQ_2                      2
#define AUTH_TRANSACTION_SEQ_3                      3
#define AUTH_TRANSACTION_SEQ_4                      4

/* 7.3.1.3 Beacon Interval field */
#define BEACON_INTERVAL_FIELD_LEN                   2

/* 7.3.1.4 Capability Information field */
#define CAP_INFO_FIELD_LEN                          2
#define CAP_INFO_ESS                                BIT(0)
#define CAP_INFO_IBSS                               BIT(1)
#define CAP_INFO_BSS_TYPE                           (CAP_INFO_ESS | CAP_INFO_IBSS)
#define CAP_INFO_CF_POLLABLE                        BIT(2)
#define CAP_INFO_CF_POLL_REQ                        BIT(3)
#define CAP_INFO_CF                                 (CAP_INFO_CF_POLLABLE | CAP_INFO_CF_POLL_REQ)
#define CAP_INFO_PRIVACY                            BIT(4)
#define CAP_INFO_SHORT_PREAMBLE                     BIT(5)
#define CAP_INFO_PBCC                               BIT(6)
#define CAP_INFO_CH_AGILITY                         BIT(7)
#define CAP_INFO_SPEC_MGT                           BIT(8)
#define CAP_INFO_QOS                                BIT(9)
#define CAP_INFO_SHORT_SLOT_TIME                    BIT(10)
#define CAP_INFO_APSD                               BIT(11)
#define CAP_INFO_RESERVED                           BIT(12)
#define CAP_INFO_DSSS_OFDM                          BIT(13)
#define CAP_INFO_DELAYED_BLOCK_ACK                  BIT(14)
#define CAP_INFO_IMM_BLOCK_ACK                      BIT(15)
/* STA usage of CF-Pollable and CF-Poll Request subfields */
/* STA: not CF-Pollable */
#define CAP_CF_STA_NOT_POLLABLE                     0x0000
/* STA: CF-Pollable, not requesting on the CF-Polling list */
#define CAP_CF_STA_NOT_ON_LIST                      CAP_INFO_CF_POLL_REQ
/* STA: CF-Pollable, requesting on the CF-Polling list */
#define CAP_CF_STA_ON_LIST                          CAP_INFO_CF_POLLABLE
/* STA: CF-Pollable, requesting never to be polled */
#define CAP_CF_STA_NEVER_POLLED                     (CAP_INFO_CF_POLLABLE | CAP_INFO_CF_POLL_REQ)

/* AP usage of CF-Pollable and CF-Poll Request subfields */
/* AP: No point coordinator (PC) */
#define CAP_CF_AP_NO_PC                             0x0000
/* AP: PC at AP for delivery only (no polling) */
#define CAP_CF_AP_DELIVERY_ONLY                     CAP_INFO_CF_POLL_REQ
/* AP: PC at AP for delivery and polling */
#define CAP_CF_AP_DELIVERY_POLLING                  CAP_INFO_CF_POLLABLE

/* 7.3.1.5 Current AP Address field */
#define CURR_AP_ADDR_FIELD_LEN                      MAC_ADDR_LEN

/* 7.3.1.6 Listen Interval field */
#define LISTEN_INTERVAL_FIELD_LEN                   2

/* 7.3.1.7 Reason Code field */
#define REASON_CODE_FIELD_LEN                       2

#define REASON_CODE_RESERVED                        0   /* Reseved */
#define REASON_CODE_UNSPECIFIED                     1   /* Unspecified reason */
#define REASON_CODE_PREV_AUTH_INVALID               2   /* Previous auth no longer valid */
#define REASON_CODE_DEAUTH_LEAVING_BSS              3   /* Deauth because sending STA is leaving BSS */
#define REASON_CODE_DISASSOC_INACTIVITY             4   /* Disassoc due to inactivity */
#define REASON_CODE_DISASSOC_AP_OVERLOAD            5   /* Disassoc because AP is unable to handle all assoc STAs */
#define REASON_CODE_CLASS_2_ERR                     6   /* Class 2 frame rx from nonauth STA */
#define REASON_CODE_CLASS_3_ERR                     7   /* Class 3 frame rx from nonassoc STA */
#define REASON_CODE_DISASSOC_LEAVING_BSS            8   /* Disassoc because sending STA is leaving BSS */
#define REASON_CODE_ASSOC_BEFORE_AUTH               9   /* STA requesting (re)assoc is not auth with responding STA */
#define REASON_CODE_DISASSOC_PWR_CAP_UNACCEPTABLE   10  /* Disassoc because the info in Power Capability is unacceptable */
#define REASON_CODE_DISASSOC_SUP_CHS_UNACCEPTABLE   11  /* Disassoc because the info in Supported Channels is unacceptable */
#define REASON_CODE_INVALID_INFO_ELEM               13  /* Invalid information element */
#define REASON_CODE_MIC_FAILURE                     14  /* MIC failure */
#define REASON_CODE_4_WAY_HANDSHAKE_TIMEOUT         15  /* 4-way handshake timeout */
#define REASON_CODE_GROUP_KEY_UPDATE_TIMEOUT        16  /* Group key update timeout */
#define REASON_CODE_DIFFERENT_INFO_ELEM             17  /* Info element in 4-way handshake different from (Re-)associate request/Probe response/Beacon */
#define REASON_CODE_MULTICAST_CIPHER_NOT_VALID      18  /* Multicast Cipher is not valid */
#define REASON_CODE_UNICAST_CIPHER_NOT_VALID        19  /* Unicast Cipher is not valid */
#define REASON_CODE_AKMP_NOT_VALID                  20  /* AKMP is not valid */
#define REASON_CODE_UNSUPPORTED_RSNE_VERSION        21  /* Unsupported RSNE version */
#define REASON_CODE_INVALID_RSNE_CAPABILITIES       22  /* Invalid RSNE Capabilities */
#define REASON_CODE_IEEE_802_1X_AUTH_FAILED         23  /* IEEE 802.1X Authentication failed */
#define REASON_CODE_CIPHER_REJECT_SEC_POLICY        24  /* Cipher suite rejected because of the security policy */
#define REASON_CODE_DISASSOC_UNSPECIFIED_QOS        32  /* Disassoc for unspecified, QoS-related reason */
#define REASON_CODE_DISASSOC_LACK_OF_BANDWIDTH      33  /* Disassoc because QAP lacks sufficient bandwidth for this QSTA */
#define REASON_CODE_DISASSOC_ACK_LOST_POOR_CHANNEL  34  /* Disassoc because of too many ACKs lost for AP transmissions and/or poor channel conditions */
#define REASON_CODE_DISASSOC_TX_OUTSIDE_TXOP_LIMIT  35  /* Disassoc because QSTA is transmitting outside the limits of its TXOPs */
#define REASON_CODE_PEER_WHILE_LEAVING              36  /* QSTA is leaving the QBSS or resetting */
#define REASON_CODE_PEER_REFUSE_DLP                 37  /* Peer does not want to use this mechanism */
#define REASON_CODE_PEER_SETUP_REQUIRED             38  /* Frames received but a setup is reqired */
#define REASON_CODE_PEER_TIME_OUT                   39  /* Time out */
#define REASON_CODE_PEER_CIPHER_UNSUPPORTED         45  /* Peer does not support the requested cipher suite */

/* 7.3.1.8 AID field */
#define AID_FIELD_LEN                               2
#define AID_MASK                                    BITS(0,13)
#define AID_MSB                                     BITS(14,15)
#define AID_MIN_VALUE                               1
#define AID_MAX_VALUE                               2007

/* 7.3.1.9 Status Code field */
#define STATUS_CODE_FIELD_LEN                       2

#define STATUS_CODE_RESERVED                        0   /* Reserved - Used by TX Auth */
#define STATUS_CODE_SUCCESSFUL                      0   /* Successful */
#define STATUS_CODE_UNSPECIFIED_FAILURE             1   /* Unspecified failure */
#define STATUS_CODE_CAP_NOT_SUPPORTED               10  /* Cannot support all requested cap in the Cap Info field */
#define STATUS_CODE_REASSOC_DENIED_WITHOUT_ASSOC    11  /* Reassoc denied due to inability to confirm that assoc exists */
#define STATUS_CODE_ASSOC_DENIED_OUTSIDE_STANDARD   12  /* Assoc denied due to reason outside the scope of this std. */
#define STATUS_CODE_AUTH_ALGORITHM_NOT_SUPPORTED    13  /* Responding STA does not support the specified auth algorithm */
#define STATUS_CODE_AUTH_OUT_OF_SEQ                 14  /* Rx an auth frame with auth transaction seq num out of expected seq */
#define STATUS_CODE_AUTH_REJECTED_CHAL_FAIL         15  /* Auth rejected because of challenge failure */
#define STATUS_CODE_AUTH_REJECTED_TIMEOUT           16  /* Auth rejected due to timeout waiting for next frame in sequence */
#define STATUS_CODE_ASSOC_DENIED_AP_OVERLOAD        17  /* Assoc denied because AP is unable to handle additional assoc STAs */
#define STATUS_CODE_ASSOC_DENIED_RATE_NOT_SUPPORTED 18  /* Assoc denied due to requesting STA not supporting all of basic rates */
#define STATUS_CODE_ASSOC_DENIED_NO_SHORT_PREAMBLE  19  /* Assoc denied due to requesting STA not supporting short preamble */
#define STATUS_CODE_ASSOC_DENIED_NO_PBCC            20  /* Assoc denied due to requesting STA not supporting PBCC */
#define STATUS_CODE_ASSOC_DENIED_NO_CH_AGILITY      21  /* Assoc denied due to requesting STA not supporting channel agility */
#define STATUS_CODE_ASSOC_REJECTED_NO_SPEC_MGT      22  /* Assoc rejected because Spectrum Mgt capability is required */
#define STATUS_CODE_ASSOC_REJECTED_PWR_CAP          23  /* Assoc rejected because the info in Power Capability is unacceptable */
#define STATUS_CODE_ASSOC_REJECTED_SUP_CHS          24  /* Assoc rejected because the info in Supported Channels is unacceptable */
#define STATUS_CODE_ASSOC_DENIED_NO_SHORT_SLOT_TIME 25  /* Assoc denied due to requesting STA not supporting short slot time */
#define STATUS_CODE_ASSOC_DENIED_NO_DSSS_OFDM       26  /* Assoc denied due to requesting STA not supporting DSSS-OFDM */
#define STATUS_CODE_UNSPECIFIED_QOS_FAILURE         32  /* Unspecified, QoS-related failure */
#define STATUS_CODE_ASSOC_DENIED_BANDWIDTH          33  /* Assoc denied due to insufficient bandwidth to handle another QSTA */
#define STATUS_CODE_ASSOC_DENIED_POOR_CHANNEL       34  /* Assoc denied due to excessive frame loss rates and/or poor channel conditions */
#define STATUS_CODE_ASSOC_DENIED_NO_QOS_FACILITY    35  /* Assoc denied due to requesting STA not supporting QoS facility */
#define STATUS_CODE_REQ_DECLINED                    37  /* Request has been declined */
#define STATUS_CODE_REQ_INVALID_PARAMETER_VALUE     38  /* Request has not been successful as one or more parameters have invalid values */
#define STATUS_CODE_REQ_NOT_HONORED_TSPEC           39  /* TS not created because request cannot be honored. Suggested TSPEC provided. */
#define STATUS_CODE_INVALID_INFO_ELEMENT            40  /* Invalid information element */
#define STATUS_CODE_INVALID_GROUP_CIPHER            41  /* Invalid group cipher */
#define STATUS_CODE_INVALID_PAIRWISE_CIPHER         42  /* Invalid pairwise cipher */
#define STATUS_CODE_INVALID_AKMP                    43  /* Invalid AKMP */
#define STATUS_CODE_UNSUPPORTED_RSN_IE_VERSION      44  /* Unsupported RSN information element version */
#define STATUS_CODE_INVALID_RSN_IE_CAP              45  /* Invalid RSN information element capabilities */
#define STATUS_CODE_CIPHER_SUITE_REJECTED           46  /* Cipher suite rejected because of security policy */
#define STATUS_CODE_REQ_NOT_HONORED_TS_DELAY        47  /* TS not created becasue request cannot be honored. Attempt to create a TS later. */
#define STATUS_CODE_DIRECT_LINK_NOT_ALLOWED         48  /* Direct Link is not allowed in the BSS by policy */
#define STATUS_CODE_DESTINATION_STA_NOT_PRESENT     49  /* Destination STA is not present within this QBSS */
#define STATUS_CODE_DESTINATION_STA_NOT_QSTA        50  /* Destination STA is not a QSTA */
#define STATUS_CODE_ASSOC_DENIED_LARGE_LIS_INTERVAL 51  /* Association denied because the ListenInterval is too large */

/* proprietary definition of reserved field of Status Code */
#define STATUS_CODE_JOIN_FAILURE                    0xFFF0  /* Join failure */
#define STATUS_CODE_JOIN_TIMEOUT                    0xFFF1  /* Join timeout */
#define STATUS_CODE_AUTH_TIMEOUT                    0xFFF2  /* Authentication timeout */
#define STATUS_CODE_ASSOC_TIMEOUT                   0xFFF3  /* (Re)Association timeout */
#define STATUS_CODE_CCX_CCKM_REASSOC_FAILURE        0xFFF4  /* CCX CCKM reassociation failure */


/* 7.3.1.10 Timestamp field */
#define TIMESTAMP_FIELD_LEN                         8

/* 7.3.1.11 Category of Action field */
#define CATEGORY_SPEC_MGT                           0
#define CATEGORY_QOS_ACTION                         1   /* QoS action */
#define CATEGORY_DLS_ACTION                         2   /* Direct Link Protocol (DLP) action */
#define CATEGORY_BLOCK_ACK_ACTION                   3   /* Block ack action */
#define CATEGORY_PUBLIC_ACTION                      4   /* Public action */
#define CATEGORY_RM_ACTION                          5   /* Radio measurement action */
#define CATEGORY_HT_ACTION                          7
#define CATEGORY_WNM_ACTION                         10
#define CATEGORY_UNPROTECTED_WNM_ACTION             11
#define CATEGORY_VHT_ACTION                          21
#define CATEGORY_WME_MGT_NOTIFICATION               17  /* WME management notification */
#define CATEGORY_VENDOR_SPECIFIC_ACTION         127 /* vendor specific action */

/* 7.3.1.14 Block Ack Parameter Set field */
#define BA_PARAM_SET_ACK_POLICY_MASK                BIT(1)
#define BA_PARAM_SET_ACK_POLICY_MASK_OFFSET         1
#define BA_PARAM_SET_TID_MASK                       BITS(2,5)
#define BA_PARAM_SET_TID_MASK_OFFSET                2
#define BA_PARAM_SET_BUFFER_SIZE_MASK               BITS(6,15)
#define BA_PARAM_SET_BUFFER_SIZE_MASK_OFFSET        6

#define BA_PARAM_SET_ACK_POLICY_IMMEDIATE_BA        1
#define BA_PARAM_SET_ACK_POLICY_DELAYED_BA          0


//3 Management frame body components (II): Information Elements.
/* 7.3.2 Element IDs of information elements */
#define ELEM_HDR_LEN                                2

#define ELEM_ID_SSID                                0   /* SSID */
#define ELEM_ID_SUP_RATES                           1   /* Supported rates */
#define ELEM_ID_FH_PARAM_SET                        2   /* FH parameter set */
#define ELEM_ID_DS_PARAM_SET                        3   /* DS parameter set */
#define ELEM_ID_CF_PARAM_SET                        4   /* CF parameter set */
#define ELEM_ID_TIM                                 5   /* TIM */
#define ELEM_ID_IBSS_PARAM_SET                      6   /* IBSS parameter set */
#define ELEM_ID_COUNTRY_INFO                        7   /* Country information */
#define ELEM_ID_HOPPING_PATTERN_PARAM               8   /* Hopping pattern parameters */
#define ELEM_ID_HOPPING_PATTERN_TABLE               9   /* Hopping pattern table */
#define ELEM_ID_REQUEST                             10  /* Request */
#define ELEM_ID_BSS_LOAD                            11  /* BSS load */
#define ELEM_ID_EDCA_PARAM_SET                      12  /* EDCA parameter set */
#define ELEM_ID_TSPEC                               13  /* Traffic specification (TSPEC) */
#define ELEM_ID_TCLAS                               14  /* Traffic classification (TCLAS) */
#define ELEM_ID_SCHEDULE                            15  /* Schedule */
#define ELEM_ID_CHALLENGE_TEXT                      16  /* Challenge text */

#define ELEM_ID_PWR_CONSTRAINT                      32  /* Power constraint */
#define ELEM_ID_PWR_CAP                             33  /* Power capability */
#define ELEM_ID_TPC_REQ                             34  /* TPC request */
#define ELEM_ID_TPC_REPORT                          35  /* TPC report */
#define ELEM_ID_SUP_CHS                             36  /* Supported channels */
#define ELEM_ID_CH_SW_ANNOUNCEMENT                  37  /* Channel switch announcement */
#define ELEM_ID_MEASUREMENT_REQ                     38  /* Measurement request */
#define ELEM_ID_MEASUREMENT_REPORT                  39  /* Measurement report */
#define ELEM_ID_QUIET                               40  /* Quiet */
#define ELEM_ID_IBSS_DFS                            41  /* IBSS DFS */
#define ELEM_ID_ERP_INFO                            42  /* ERP information */
#define ELEM_ID_TS_DELAY                            43  /* TS delay */
#define ELEM_ID_TCLAS_PROCESSING                    44  /* TCLAS processing */
#define ELEM_ID_HT_CAP                              45  /* HT Capabilities subelement */
#define ELEM_ID_QOS_CAP                             46  /* QoS capability */
#define ELEM_ID_RSN                                 48  /* RSN IE */
#define ELEM_ID_EXTENDED_SUP_RATES                  50  /* Extended supported rates */
#define ELEM_ID_HT_OP                               61  /* HT Operation */
#define ELEM_ID_SCO                                 62  /* Secondary Channel Offset */
#define ELEM_ID_RRM_ENABLED_CAP                     70  /* Radio Resource Management Enabled Capabilities */
#define ELEM_ID_20_40_BSS_COEXISTENCE               72  /* 20/40 BSS Coexistence */
#define ELEM_ID_20_40_INTOLERANT_CHNL_REPORT        73  /* 20/40 BSS Intolerant Channel Report */
#define ELEM_ID_OBSS_SCAN_PARAMS                    74  /* Overlapping BSS Scan Parameters */
#define ELEM_ID_FMS                    				86  /* FMS */

#define ELEM_ID_EXTENDED_CAP                        127 /* Extended capabilities */

#define ELEM_ID_VHT_CAP                             191  /* VHT Capabilities subelement */
#define ELEM_ID_VHT_OP                              192  /*VHT Operation information*/


#define ELEM_ID_VENDOR                              221 /* Vendor specific IE */
#define ELEM_ID_WPA                                 ELEM_ID_VENDOR /* WPA IE */
#define ELEM_ID_WMM                                 ELEM_ID_VENDOR /* WMM IE */
#define ELEM_ID_P2P                                   ELEM_ID_VENDOR /* WiFi Direct */
#define ELEM_ID_WFD                                 ELEM_ID_VENDOR

#define ELEM_ID_RESERVED                            255 /* Reserved */


/* 7.3.2.1 SSID element */
#define ELEM_MAX_LEN_SSID                           32

/* 7.3.2.2 Supported Rates */
#define ELEM_MAX_LEN_SUP_RATES                      8


#define ELEM_MAX_LEN_EXT_SUP_RATES                      (RATE_NUM_SW-ELEM_MAX_LEN_SUP_RATES)

/* 7.3.2.4 DS Parameter Set */
#define ELEM_MAX_LEN_DS_PARAMETER_SET               1

/* 7.3.2.5 CF Parameter Set */
#define ELEM_CF_PARM_LEN                            8

/* 7.3.2.6 TIM */
#define ELEM_MIX_LEN_TIM                            4
#define ELEM_MAX_LEN_TIM                            254

/* 7.3.2.7 IBSS Parameter Set element */
#define ELEM_MAX_LEN_IBSS_PARAMETER_SET             2

/* 7.3.2.8 Challenge Text element */
#define ELEM_MIN_LEN_CHALLENGE_TEXT                 1
#define ELEM_MAX_LEN_CHALLENGE_TEXT                 253

/* 7.3.2.9 Country Information element */
/* Country IE should contain at least 3-bytes country code string and one subband triplet. */
#define ELEM_MIN_LEN_COUNTRY_INFO                   6

#define ELEM_ID_COUNTRY_INFO_TRIPLET_LEN_FIXED              3
#define ELEM_ID_COUNTRY_INFO_SUBBAND_TRIPLET_LEN_FIXED      3
#define ELEM_ID_COUNTRY_INFO_REGULATORY_TRIPLET_LEN_FIXED   3


/* 7.3.2.13 ERP Information element */
#define ELEM_MAX_LEN_ERP                            1
/* -- bits in the ERP Information element */
#define ERP_INFO_NON_ERP_PRESENT                    BIT(0)  /* NonERP_Present bit */
#define ERP_INFO_USE_PROTECTION                     BIT(1)  /* Use_Protection bit */
#define ERP_INFO_BARKER_PREAMBLE_MODE               BIT(2)  /* Barker_Preamble_Mode bit */


/* 7.3.2.14 Extended Supported Rates */
#define ELEM_MAX_LEN_EXTENDED_SUP_RATES             255

/* 7.3.2.21 Measurement Request element */
#define ELEM_RM_TYPE_BASIC_REQ                      0
#define ELEM_RM_TYPE_CCA_REQ                        1
#define ELEM_RM_TYPE_RPI_HISTOGRAM_REQ              2
#define ELEM_RM_TYPE_CHNL_LOAD_REQ                  3
#define ELEM_RM_TYPE_NOISE_HISTOGRAM_REQ            4
#define ELEM_RM_TYPE_BEACON_REQ                     5
#define ELEM_RM_TYPE_FRAME_REQ                      6
#define ELEM_RM_TYPE_STA_STATISTICS_REQ             7
#define ELEM_RM_TYPE_LCI_REQ                        8
#define ELEM_RM_TYPE_TS_REQ                         9
#define ELEM_RM_TYPE_MEASURE_PAUSE_REQ              255

/* 7.3.2.22 Measurement Report element */
#define ELEM_RM_TYPE_BASIC_REPORT                   0
#define ELEM_RM_TYPE_CCA_REPORT                     1
#define ELEM_RM_TYPE_RPI_HISTOGRAM_REPORT           2
#define ELEM_RM_TYPE_CHNL_LOAD_REPORT               3
#define ELEM_RM_TYPE_NOISE_HISTOGRAM_REPORT         4
#define ELEM_RM_TYPE_BEACON_REPORT                  5
#define ELEM_RM_TYPE_FRAME_REPORT                   6
#define ELEM_RM_TYPE_STA_STATISTICS_REPORT          7
#define ELEM_RM_TYPE_LCI_REPORT                     8
#define ELEM_RM_TYPE_TS_REPORT                      9


/* 7.3.2.25 RSN information element */
#define ELEM_MAX_LEN_WPA                            24 /* one pairwise, one AKM suite, one PMKID */
#define ELEM_MAX_LEN_RSN                            38 /* one pairwise, one AKM suite, one PMKID */
#define ELEM_MAX_LEN_WAPI                           38 /* one pairwise, one AKM suite, one BKID */

/* 7.3.2.27 Extended Capabilities information element */
#define ELEM_EXT_CAP_20_40_COEXIST_SUPPORT          BIT(0)
#define ELEM_EXT_CAP_PSMP_CAP                       BIT(4)
#define ELEM_EXT_CAP_SERVICE_INTERVAL_GRANULARITY   BIT(5)
#define ELEM_EXT_CAP_SCHEDULE_PSMP                  BIT(6)

#define ELEM_MAX_LEN_EXT_CAP                        (3 - ELEM_HDR_LEN)

/* 7.3.2.30 TSPEC element */
#define TS_INFO_TRAFFIC_TYPE_MASK                   BIT(0)      //WMM: 0 (Asynchronous TS of low-duty cycles)
#define TS_INFO_TID_OFFSET                          1
#define TS_INFO_TID_MASK                            BITS(1,4)
#define TS_INFO_DIRECTION_OFFSET                    5
#define TS_INFO_DIRECTION_MASK                      BITS(5,6)
#define TS_INFO_ACCESS_POLICY_OFFSET                7
#define TS_INFO_ACCESS_POLICY_MASK                  BITS(7,8)   //WMM: Bit(7,8) = (1,0) = EDCA
#define TS_INFO_AGGREGATION_MASK                    BIT(9)      //WMM: 0
#define TS_INFO_APSD_MASK                           BIT(10)
#define TS_INFO_UP_OFFSET                           11
#define TS_INFO_UP_MASK                             BITS(11,13)
#define TS_INFO_ACK_POLICY_OFFSET                   14
#define TS_INFO_ACK_POLICY_MASK                     BITS(14,15)
#define TS_INFO_SCHEDULE_MASK                       16

/* 7.3.2.56 HT capabilities element */
#define ELEM_MAX_LEN_HT_CAP                         (28 - ELEM_HDR_LEN) /* sizeof(IE_HT_CAP_T)-2 */

/* 7.3.2.56.2 HT capabilities Info field */
#define HT_CAP_INFO_LDPC_CAP                        BIT(0)
#define HT_CAP_INFO_SUP_CHNL_WIDTH                  BIT(1)
#define HT_CAP_INFO_SM_POWER_SAVE                   BITS(2,3)
#define HT_CAP_INFO_HT_GF                           BIT(4)
#define HT_CAP_INFO_SHORT_GI_20M                    BIT(5)
#define HT_CAP_INFO_SHORT_GI_40M                    BIT(6)
#define HT_CAP_INFO_TX_STBC                         BIT(7)
#define HT_CAP_INFO_RX_STBC                         BITS(8,9)
#define HT_CAP_INFO_HT_DELAYED_BA                   BIT(10)
#define HT_CAP_INFO_MAX_AMSDU_LEN                   BIT(11)
#define HT_CAP_INFO_DSSS_CCK_IN_40M                 BIT(12)
#define HT_CAP_INFO_40M_INTOLERANT                  BIT(14)
#define HT_CAP_INFO_LSIG_TXOP_SUPPORT               BIT(15)

#define HT_CAP_INFO_RX_STBC_NO_SUPPORTED            0
#define HT_CAP_INFO_RX_STBC_1_SS                    BIT(8)
#define HT_CAP_INFO_RX_STBC_2_SS                    BIT(9)
#define HT_CAP_INFO_RX_STBC_3_SS                    HT_CAP_INFO_RX_STBC

/*8.4.2.160.2 VHT capability Info field*/
#define VHT_CAP_INFO_MAX_MPDU_LTNGTH                BITS(0,1)
#define VHT_CAP_INFO_SUP_CHNL_WIDTH                 BITS(2,3)
#define VHT_CAP_INFO_RX_LDPC                        BIT(4)
#define VHT_CAP_INFO_SHORT_GI_80M                   BIT(5)
#define VHT_CAP_INFO_SHORT_GI_160M                  BIT(6)
#define VHT_CAP_INFO_TX_STBC                        BIT(7)
#define VHT_CAP_INFO_RX_STBC                        BITS(8,10)
#define VHT_CAP_INFO_SU_BEAMFORMER                  BIT(11)
#define VHT_CAP_INFO_SU_BEAMFORMEE                  BIT(12)
#define VHT_CAP_INFO_CMPS_BF_ANTENNAL               BITS(13,15)
#define VHT_CAP_INFO_SOUNDINGS_DIMENSIONS           BITS(16,18)
#define VHT_CAP_INFO_MU_BEAMFORMER                  BIT(19)
#define VHT_CAP_INFO_MU_BEAMFORMEE                  BIT(20)
#define VHT_CAP_INFO_TXOP_PS                        BIT(21)
#define VHT_CAP_INFO_VHT_VARIANT_CAP                BIT(22)
#define VHT_CAP_INFO_MAX_AMPDU_LENGTH               BITS(23,25)
#define VHT_CAP_INFO_LA_CAP                         BITS(26,27)
#define VHT_CAP_INFO_RX_PATTERN                     BIT(28)
#define VHT_CAP_INFO_TX_PATTERN                     BIT(29)

#define VHT_CAP_NOT_SUPPORT_ABOVE_BW_80     0 
#define VHT_CAP_SUPPORT_BW_160              1 
#define VHT_CAP_SUPPORT_BW_160_80_80        2 

#define VHT_CAP_MAX_AMPDU_3895_OCTETS      0
#define VHT_CAP_MAX_AMPDU_7991_OCTETS      1
#define VHT_CAP_MAX_AMPDU_11454_OCTETS     2

#define VHT_OP_BW_20_40   0
#define VHT_OP_BW_80      1
#define VHT_OP_BW_160     2
#define VHT_OP_BW_80_80   3

#define VHT_OP_INFO_CHANNEL_WIDTH_BYTE_INDEX        0
#define VHT_OP_INFO_CHANNEL_CENTER_FREQ1_BYTE_INDEX 1
#define VHT_OP_INFO_CHANNEL_CENTER_FREQ2_BYTE_INDEX 2

/*8.4.2.160.3 Support VHT-MCS and NSS Set field*/

#define VHT_MCS_TX_RX_MAX_1SS BITS(0,1)
#define VHT_MCS_TX_RX_MAX_2SS BITS(2,3)
#define VHT_MCS_TX_RX_MAX_3SS BITS(4,5)
#define VHT_MCS_TX_RX_MAX_4SS BITS(6,7)

#define VHT_MCS_SUPPORT_MCS_0_9 2
#define VHT_MCS_SUPPORT_MCS_0_8 1
#define VHT_MCS_SUPPORT_MCS_0_7 0 
#define VHT_MCS_SUPPORT_MCS_NONE 3 



/* 7.3.2.56.3 A-MPDU Parameters field */
#define AMPDU_PARAM_MAX_AMPDU_LEN_EXP               BITS(0,1)
#define AMPDU_PARAM_MIN_START_SPACING               BITS(2,4)

#define AMPDU_PARAM_MAX_AMPDU_LEN_8K                0
#define AMPDU_PARAM_MAX_AMPDU_LEN_16K               BIT(0)
#define AMPDU_PARAM_MAX_AMPDU_LEN_32K               BIT(1)
#define AMPDU_PARAM_MAX_AMPDU_LEN_64K               BITS(0,1)

#define AMPDU_PARAM_MSS_NO_RESTRICIT                0
#define AMPDU_PARAM_MSS_1_4_US                      BIT(2)
#define AMPDU_PARAM_MSS_1_2_US                      BIT(3)
#define AMPDU_PARAM_MSS_1_US                        BITS(2,3)
#define AMPDU_PARAM_MSS_2_US                        BIT(4)
#define AMPDU_PARAM_MSS_4_US                        (BIT(4) | BIT(2))
#define AMPDU_PARAM_MSS_8_US                        (BIT(4) | BIT(3))
#define AMPDU_PARAM_MSS_16_US                       BITS(2,4)

/* 7.3.2.56.4 Supported MCS Set field (TX rate: octects 12~15) */
#define SUP_MCS_TX_SET_DEFINED                      BIT(0)
#define SUP_MCS_TX_RX_SET_NOT_EQUAL                 BIT(1)
#define SUP_MCS_TX_MAX_NUM_SS                       BITS(2,3)
#define SUP_MCS_TX_UNEQUAL_MODULATION               BIT(4)

#define SUP_MCS_TX_MAX_NUM_1_SS                     0
#define SUP_MCS_TX_MAX_NUM_2_SS                     BIT(2)
#define SUP_MCS_TX_MAX_NUM_3_SS                     BIT(3)
#define SUP_MCS_TX_MAX_NUM_4_SS                     BITS(2,3)

#define SUP_MCS_RX_BITMASK_OCTET_NUM                10
#define SUP_MCS_RX_DEFAULT_HIGHEST_RATE             0   /* Not specify */

/* 7.3.2.56.5 HT Extended Capabilities field */
#define HT_EXT_CAP_PCO                              BIT(0)
#define HT_EXT_CAP_PCO_TRANSITION_TIME              BITS(1,2)
#define HT_EXT_CAP_MCS_FEEDBACK                     BITS(8,9)
#define HT_EXT_CAP_HTC_SUPPORT                      BIT(10)
#define HT_EXT_CAP_RD_RESPONDER                     BIT(11)

#define HT_EXT_CAP_PCO_TRANS_TIME_NONE              0
#define HT_EXT_CAP_PCO_TRANS_TIME_400US             BIT(1)
#define HT_EXT_CAP_PCO_TRANS_TIME_1_5MS             BIT(2)
#define HT_EXT_CAP_PCO_TRANS_TIME_5MS               BITS(1,2)

#define HT_EXT_CAP_MCS_FEEDBACK_NO_FB               0
#define HT_EXT_CAP_MCS_FEEDBACK_UNSOLICITED         BIT(9)
#define HT_EXT_CAP_MCS_FEEDBACK_BOTH                BITS(8,9)

/* 7.3.2.56.6 Transmit Beamforming Capabilities field */

/* 7.3.2.56.7 Antenna Selection Capability field */
#define ASEL_CAP_CAPABLE                            BIT(0)
#define ASEL_CAP_CSI_FB_BY_TX_ASEL_CAPABLE          BIT(1)
#define ASEL_CAP_ANT_INDICES_FB_BY_TX_ASEL_CAPABLE  BIT(2)
#define ASEL_CAP_EXPLICIT_CSI_FB_CAPABLE            BIT(3)
#define ASEL_CAP_ANT_INDICES_CAPABLE                BIT(4)
#define ASEL_CAP_RX_ASEL_CAPABLE                    BIT(5)
#define ASEL_CAP_TX_SOUNDING_CAPABLE                BIT(6)

/* 7.3.2.57 HT Operation element */
#define ELEM_MAX_LEN_HT_OP                          (24 - ELEM_HDR_LEN) /* sizeof(IE_HT_OP_T)-2 */

#define HT_OP_INFO1_SCO                             BITS(0,1)
#define HT_OP_INFO1_STA_CHNL_WIDTH                  BIT(2)
#define HT_OP_INFO1_RIFS_MODE                       BIT(3)

#define HT_OP_INFO2_HT_PROTECTION                   BITS(0,1)
#define HT_OP_INFO2_NON_GF_HT_STA_PRESENT           BIT(2)
#define HT_OP_INFO2_OBSS_NON_HT_STA_PRESENT         BIT(4)

#define HT_OP_INFO3_DUAL_BEACON                     BIT(6)
#define HT_OP_INFO3_DUAL_CTS_PROTECTION             BIT(7)
#define HT_OP_INFO3_STBC_BEACON                     BIT(8)
#define HT_OP_INFO3_LSIG_TXOP_FULL_SUPPORT          BIT(9)
#define HT_OP_INFO3_PCO_ACTIVE                      BIT(10)
#define HT_OP_INFO3_PCO_PHASE                       BIT(11)

/* 8.4.1.49 VHT Operating Mode field*/
#define VHT_ACTION_OPMODE_CHANNELWIDTH				BITS(0,1)
#define VHT_ACTION_RXNSS							BITS(4,6)
#define VHT_ACTION_MAX_NSS_SU_PRESENT				BIT(7)
/* 7.3.2.59 OBSS Scan Parameter element */
#define ELEM_MAX_LEN_OBSS_SCAN                      (16 - ELEM_HDR_LEN)

/* 7.3.2.60 20/40 BSS Coexistence element */
#define ELEM_MAX_LEN_20_40_BSS_COEXIST              (3 - ELEM_HDR_LEN)

#define BSS_COEXIST_INFO_REQ                        BIT(0)
#define BSS_COEXIST_40M_INTOLERANT                  BIT(1)
#define BSS_COEXIST_20M_REQ                         BIT(2)
#define BSS_COEXIST_OBSS_SCAN_EXEMPTION_REQ         BIT(3)
#define BSS_COEXIST_OBSS_SCAN_EXEMPTION_GRANT       BIT(4)


//3 Management frame body components (III): 7.4 Action frame format details.
/* 7.4.1 Spectrum Measurement Action frame details */
#define ACTION_MEASUREMENT_REQ                      0   /* Spectrum measurement request */
#define ACTION_MEASUREMENT_REPORT                   1   /* Spectrum measurement report */
#define ACTION_TPC_REQ                              2   /* TPC request */
#define ACTION_TPC_REPORT                           3   /* TPC report */
#define ACTION_CHNL_SWITCH                          4   /* Channel Switch Announcement */

/* 7.4.2 QoS Action frame details */
#define ACTION_ADDTS_REQ                            0   /* ADDTS request */
#define ACTION_ADDTS_RSP                            1   /* ADDTS response */
#define ACTION_DELTS                                2   /* DELTS */
#define ACTION_SCHEDULE                             3   /* Schedule */

#define ACTION_ADDTS_REQ_FRAME_LEN                  (24+3+63)  /* WMM TSPEC IE: 63 */
#define ACTION_ADDTS_RSP_FRAME_LEN                  (24+4+63)  /* WMM Status Code: 1; WMM TSPEC IE: 63*/

/* 7.4.3 DLS Action frame details */
#define ACTION_DLS_REQ                              0   /* DLS request */
#define ACTION_DLS_RSP                              1   /* DLS response */
#define ACTION_DLS_TEARDOWN                         2   /* DLS teardown */

/* 7.4.4 Block ack  Action frame details */
#define ACTION_ADDBA_REQ                            0   /* ADDBA request */
#define ACTION_ADDBA_RSP                            1   /* ADDBA response */
#define ACTION_DELBA                                2   /* DELBA */

#define ACTION_ADDBA_REQ_FRAME_LEN                  (24+9)
#define ACTION_ADDBA_RSP_FRAME_LEN                  (24+9)

#define ACTION_DELBA_INITIATOR_MASK                 BIT(11)
#define ACTION_DELBA_TID_MASK                       BITS(12,15)
#define ACTION_DELBA_TID_OFFSET                     12
#define ACTION_DELBA_FRAME_LEN                      (24+6)

/* 7.4.6 Radio Measurement Action frame details */
#define ACTION_RM_REQ                               0   /* Radio measurement request */
#define ACTION_RM_REPORT                            1   /* Radio measurement report */
#define ACTION_LM_REQ                               2   /* Link measurement request */
#define ACTION_LM_REPORT                            3   /* Link measurement report */
#define ACTION_NEIGHBOR_REPORT_REQ                  4   /* Neighbor report request */
#define ACTION_NEIGHBOR_REPORT_RSP                  5   /* Neighbor report response */

/* 7.4.7 Public Action frame details */
#define ACTION_PUBLIC_20_40_COEXIST                 0   /* 20/40 BSS coexistence */
#define ACTION_PUBLIC_WIFI_DIRECT                   9

/* 7.4.10.1 HT action frame details */
#define ACTION_HT_NOTIFY_CHANNEL_WIDTH              0   /* Notify Channel Width */
#define ACTION_HT_SM_POWER_SAVE                     1   /* SM Power Save */
#define ACTION_HT_PSMP                              2   /* PSMP */
#define ACTION_HT_SET_PCO_PHASE                     3   /* Set PCO Phase */
#define ACTION_HT_CSI                               4   /* CSI */
#define ACTION_HT_NON_COMPRESSED_BEAMFORM           5   /* Non-compressed Beamforming */
#define ACTION_HT_COMPRESSED_BEAMFORM               6   /* Compressed Beamforming */
#define ACTION_HT_ANT_SEL_INDICES_FB                7   /* Antenna Selection Indices Feedback */


/*8.5.23.1 VHT Action field*/
#define ACTION_VHT_COMPRESSED_BEAMFORMING			0
#define ACTION_VHT_GROUP_ID_MANAGEMENT				1
#define ACTION_VHT_OPERATING_MODE_NOTIFICATION		2

/*8.5.14 WNM Action field*/
#define ACTION_TIMING_MEASUREMENT_REQUEST 			25
#define ACTION_TIMING_MEASUREMENT_REQUEST_FRAME_LEN (24+3)

/*8.5.11.1 UNprotected WNM Action field*/
#define ACTION_TIM                      			0
#define ACTION_TIMING_MEASUREMENT          			1
#define ACTION_TIMING_MEASUREMENT_FRAME_LEN         (24+14)


//3 /* --------------- WFA  frame body fields --------------- */
#define VENDOR_OUI_WFA                              { 0x00, 0x50, 0xF2 }
#define VENDOR_OUI_WFA_SPECIFIC                     { 0x50, 0x6F, 0x9A }
#define VENDOR_OUI_TYPE_WPA                         1
#define VENDOR_OUI_TYPE_WMM                         2
#define VENDOR_OUI_TYPE_WPS                         4
#define VENDOR_OUI_TYPE_P2P                         9
#define VENDOR_OUI_TYPE_WFD                         10

#define VENDOR_OUI_TYPE_LEN                         4    /* Length of OUI and Type */

/* VERSION(2 octets for WPA) / SUBTYPE(1 octet)-VERSION(1 octet) fields for WMM in WFA IE */
#define VERSION_WPA                                 0x0001 /* Little Endian Format */
#define VENDOR_OUI_SUBTYPE_VERSION_WMM_INFO         0x0100
#define VENDOR_OUI_SUBTYPE_VERSION_WMM_PARAM        0x0101

/* SUBTYPE(1 octet) for WMM */
#define VENDOR_OUI_SUBTYPE_WMM_INFO                 0x00 /* WMM Spec version 1.1 */
#define VENDOR_OUI_SUBTYPE_WMM_PARAM                0x01
#define VENDOR_OUI_SUBTYPE_WMM_TSPEC                0x02

/* VERSION(1 octet) for WMM */
#define VERSION_WMM                                 0x01 /* WMM Spec version 1.1 */

/* WMM-2.1.6 QoS Control Field */
#define WMM_QC_UP_MASK                              BITS(0,2)
#define WMM_QC_EOSP                                 BIT(4)
#define WMM_QC_ACK_POLICY_MASK                      BITS(5, 6)
#define WMM_QC_ACK_POLICY_OFFSET                    5
#define WMM_QC_ACK_POLICY_ACKNOWLEDGE               0
#define WMM_QC_ACK_POLICY_NOT_ACKNOWLEDGE           (1 << WMM_QC_ACK_POLICY_OFFSET)


/* WMM-2.2.1 WMM Information Element */
#define ELEM_MIN_LEN_WFA_OUI_TYPE_SUBTYPE           6


//3 Control frame body
/* 7.2.1.7 BlockAckReq */
#define CTRL_BAR_BAR_CONTROL_OFFSET                 16
#define CTRL_BAR_BAR_INFORMATION_OFFSET             18


#define ACTION_PUBLIC_WIFI_DIRECT                   9
#define ACTION_GAS_INITIAL_REQUEST                 10
#define ACTION_GAS_INITIAL_RESPONSE               11
#define ACTION_GAS_COMEBACK_REQUEST           12
#define ACTION_GAS_COMEBACK_RESPONSE         13


/* P2P 4.2.8.1 - P2P Public Action Frame Type. */
#define P2P_PUBLIC_ACTION_GO_NEGO_REQ               0
#define P2P_PUBLIC_ACTION_GO_NEGO_RSP               1
#define P2P_PUBLIC_ACTION_GO_NEGO_CFM               2
#define P2P_PUBLIC_ACTION_INVITATION_REQ            3
#define P2P_PUBLIC_ACTION_INVITATION_RSP            4
#define P2P_PUBLIC_ACTION_DEV_DISCOVER_REQ          5
#define P2P_PUBLIC_ACTION_DEV_DISCOVER_RSP          6
#define P2P_PUBLIC_ACTION_PROV_DISCOVERY_REQ        7
#define P2P_PUBLIC_ACTION_PROV_DISCOVERY_RSP        8

/* P2P 4.2.9.1 - P2P Action Frame Type */
#define P2P_ACTION_NOTICE_OF_ABSENCE                0
#define P2P_ACTION_P2P_PRESENCE_REQ                 1
#define P2P_ACTION_P2P_PRESENCE_RSP                 2
#define P2P_ACTION_GO_DISCOVER_REQ                  3

#define P2P_PUBLIC_ACTION_FRAME_LEN                (WLAN_MAC_MGMT_HEADER_LEN+8)
#define P2P_ACTION_FRAME_LEN                       (WLAN_MAC_MGMT_HEADER_LEN+7)



#define WFD_DEV_INFO_TYPE_MASK              BITS(0,1)
#define WFD_DEV_INFO_TYPE_SOURCE            0
#define WFD_DEV_INFO_TYPE_PRI_SINK          1
#define WFD_DEV_INFO_TYPE_SEC_SINK          2
#define WFD_DEV_INFO_TYPE_SOURCE_PRI_SINK   3

#define WFD_DEV_INFO_COUPLED_SINK_SUPPORTTRD_SOURCE     BIT(2)
#define WFD_DEV_INFO_COUPLED_SINK_SUPPORTTRD_SINK       BIT(3)
#define WFD_DEV_INFO_AVAILABLE_SESSION                  BITS(4,5)
#define WFD_DEV_INFO_SERVICE_DISCOVERY                  BIT(6)
#define WFD_DEV_INFO_PREFERRED_TLS                      BIT(7)
#define WFD_DEV_INFO_CONTENT_PROTECTION                 BIT(8)
#define WFD_DEV_INFO_TIME_SYNCHORNIZATION               BIT(9)

#define WFD_SINK_STATUS_TYPE                            BITS(0,1)
#define WFD_SINK_STATUS_NOT_COUPLED                     0
#define WFD_SINK_STATUS_COUPLED                         1
#define WFD_SINK_STATUS_TEARDOWN_COUPLING               2


#define WFD_OUI_TYPE_LEN                            4
#define WFD_IE_OUI_HDR                              (ELEM_HDR_LEN + WFD_OUI_TYPE_LEN) 

#define WFD_SESSION_INFO_DESC_HDR_LEN                       1 /* Length(1 octets) */
#define WFD_ATTRI_HDR_LEN                                   3 /* ID(1 octet) + Length(2 octet) */
#define WFD_SUBELEMENT_ID_WFD_DEV_INFO                      0
#define WFD_SUBELEMENT_ID_ASSOCIATED_BSSID                  1
#define WFD_SUBELEMENT_ID_COUPLED_SINK_INFO                 6
#define WFD_SUBELEMENT_ID_EXT_CAPA_BITMAP                   7
#define WFD_SUBELEMENT_ID_WFD_SESSION_INFO                  9

#define WFD_ATTRI_MAX_LEN_WFD_DEV_INFO                      6
#define WFD_ATTRI_MAX_LEN_ASSOCIATED_BSSID                  6
#define WFD_ATTRI_MAX_LEN_COUPLED_SINK_INFO                 7
#define WFD_ATTRI_MAX_LEN_EXT_CAPA_BITMAP                   2
#define WFD_ATTRI_MAX_LEN_WFD_SESSION_INFO                  23  /* TODO */



/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

typedef struct _LLC_SNAP_HEADER_T {
    UINT_8      ucDSAP;
    UINT_8      ucSSAP;
    UINT_8      ucControl;
    UINT_8      aucCode[3];
    UINT_16     u2Type;
} LLC_SNAP_HEADER_T, *P_LLC_SNAP_HEADER_T;

//3 MAC Header.
/* Ethernet Frame Header */
typedef struct _ETH_FRAME_HEADER_T {
    UINT_8      aucDestAddr[MAC_ADDR_LEN];
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];
    UINT_16     u2TypeLen;
} ETH_FRAME_HEADER_T, *P_ETH_FRAME_HEADER_T;

/* Ethernet Frame Structure */
typedef struct _ETH_FRAME_T {
    UINT_8      aucDestAddr[MAC_ADDR_LEN];
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];
    UINT_16     u2TypeLen;
    UINT_8      aucData[1];
} ETH_FRAME_T, *P_ETH_FRAME_T;


typedef struct _ETH_VLAN_FRAME_T {
    UINT_8      aucDestAddr[MAC_ADDR_LEN];
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];
    UINT_16     u2VlanTag;
    UINT_16     u2TCI;
    UINT_16     u2TypeLen;
    UINT_8      aucData[1];
} ETH_VLAN_FRAME_T, *P_ETH_VLAN_FRAME_T;



/* IEEE 802.11 WLAN Frame Structure */
/* WLAN MAC Header (without Address 4 and QoS Control fields) */
typedef struct _WLAN_MAC_HEADER_T {
    UINT_16     u2FrameCtrl;
    UINT_16     u2DurationID;
    UINT_8      aucAddr1[MAC_ADDR_LEN];
    UINT_8      aucAddr2[MAC_ADDR_LEN];
    UINT_8      aucAddr3[MAC_ADDR_LEN];
    UINT_16     u2SeqCtrl;
} WLAN_MAC_HEADER_T, *P_WLAN_MAC_HEADER_T;


/* WLAN MAC Header (QoS Control fields included) */
typedef struct _WLAN_MAC_HEADER_QOS_T {
    UINT_16     u2FrameCtrl;
    UINT_16     u2DurationID;
    UINT_8      aucAddr1[MAC_ADDR_LEN];
    UINT_8      aucAddr2[MAC_ADDR_LEN];
    UINT_8      aucAddr3[MAC_ADDR_LEN];
    UINT_16     u2SeqCtrl;
    UINT_16     u2QosCtrl;
} WLAN_MAC_HEADER_QOS_T, *P_WLAN_MAC_HEADER_QOS_T;


/* WLAN MAC Header (HT Control fields included) */

#ifndef __GNUC__

typedef __packed struct _WLAN_MAC_HEADER_HT_T {
    UINT_16     u2FrameCtrl;
    UINT_16     u2DurationID;
    UINT_8      aucAddr1[MAC_ADDR_LEN];
    UINT_8      aucAddr2[MAC_ADDR_LEN];
    UINT_8      aucAddr3[MAC_ADDR_LEN];
    UINT_16     u2SeqCtrl;
    UINT_16     u2QosCtrl;
    UINT_32     u4HtCtrl;
} WLAN_MAC_HEADER_HT_T, *P_WLAN_MAC_HEADER_HT_T;

#else
typedef struct _WLAN_MAC_HEADER_HT_T {
    UINT_16     u2FrameCtrl;
    UINT_16     u2DurationID;
    UINT_8      aucAddr1[MAC_ADDR_LEN];
    UINT_8      aucAddr2[MAC_ADDR_LEN];
    UINT_8      aucAddr3[MAC_ADDR_LEN];
    UINT_16     u2SeqCtrl;
    UINT_16     u2QosCtrl;
    UINT_32     u4HtCtrl;
} __attribute__((__packed__)) WLAN_MAC_HEADER_HT_T, *P_WLAN_MAC_HEADER_HT_T;


#endif



/* WLAN MAC Header (Address 4 included) */
typedef struct _WLAN_MAC_HEADER_A4_T {
    UINT_16     u2FrameCtrl;
    UINT_16     u2DurationID;
    UINT_8      aucAddr1[MAC_ADDR_LEN];
    UINT_8      aucAddr2[MAC_ADDR_LEN];
    UINT_8      aucAddr3[MAC_ADDR_LEN];
    UINT_16     u2SeqCtrl;
    UINT_8      aucAddr4[MAC_ADDR_LEN];
} WLAN_MAC_HEADER_A4_T, *P_WLAN_MAC_HEADER_A4_T;


/* WLAN MAC Header (Address 4 and QoS Control fields included) */
typedef struct _WLAN_MAC_HEADER_A4_QOS_T {
    UINT_16     u2FrameCtrl;
    UINT_16     u2DurationID;
    UINT_8      aucAddr1[MAC_ADDR_LEN];
    UINT_8      aucAddr2[MAC_ADDR_LEN];
    UINT_8      aucAddr3[MAC_ADDR_LEN];
    UINT_16     u2SeqCtrl;
    UINT_8      aucAddr4[MAC_ADDR_LEN];
    UINT_16     u2QosCtrl;
} WLAN_MAC_HEADER_A4_QOS_T, *P_WLAN_MAC_HEADER_A4_QOS_T;


typedef struct _WLAN_MAC_HEADER_A4_HT_T {
    UINT_16     u2FrameCtrl;
    UINT_16     u2DurationID;
    UINT_8      aucAddr1[MAC_ADDR_LEN];
    UINT_8      aucAddr2[MAC_ADDR_LEN];
    UINT_8      aucAddr3[MAC_ADDR_LEN];
    UINT_16     u2SeqCtrl;
    UINT_8      aucAddr4[MAC_ADDR_LEN];
    UINT_16     u2QosCtrl;
    UINT_32     u4HtCtrl;
} WLAN_MAC_HEADER_A4_HT_T, *P_WLAN_MAC_HEADER_A4_HT_T;


/* 7.2.3 WLAN MAC Header for Management Frame - MMPDU */
typedef struct _WLAN_MAC_MGMT_HEADER_T {
    UINT_16     u2FrameCtrl;
    UINT_16     u2Duration;
    UINT_8      aucDestAddr[MAC_ADDR_LEN];
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];
    UINT_8      aucBSSID[MAC_ADDR_LEN];
    UINT_16     u2SeqCtrl;
} WLAN_MAC_MGMT_HEADER_T, *P_WLAN_MAC_MGMT_HEADER_T;


/* WLAN MAC Header for Management Frame (HT Control fields included) */
typedef struct _WLAN_MAC_MGMT_HEADER_HT_T {
    UINT_16     u2FrameCtrl;
    UINT_16     u2DurationID;
    UINT_8      aucAddr1[MAC_ADDR_LEN];
    UINT_8      aucAddr2[MAC_ADDR_LEN];
    UINT_8      aucAddr3[MAC_ADDR_LEN];
    UINT_16     u2SeqCtrl;
    UINT_32     u4HtCtrl;
} WLAN_MAC_MGMT_HEADER_HT_T, *P_WLAN_MAC_MGMT_HEADER_HT_T;

//3 WLAN CONTROL Frame
/* 7.2.1.4 WLAN Control Frame - PS-POLL Frame */
typedef struct _CTRL_PSPOLL_FRAME_T {
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2AID;                      /* AID */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_8      aucTA[MAC_ADDR_LEN];        /* TA */
} CTRL_PSPOLL_FRAME_T, *P_CTRL_PSPOLL_FRAME_T;

typedef struct _CTRL_CFEND_FRAME_T {
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucRA[MAC_ADDR_LEN];        /* RA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
} CTRL_CFEND_FRAME_T, *P_CTRL_CFEND_FRAME_T;

typedef struct _CTRL_CTS_FRAME_T {
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucRA[MAC_ADDR_LEN];        /* RA */
} CTRL_CTS_FRAME_T, *P_CTRL_CTS_FRAME_T;

/* BAR */
typedef struct _CTRL_BAR_FRAME_T {
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* RA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* TA */
    UINT_16     u2BarControl;
    UINT_8      aucBarInfo[2];             /* Variable size */
} CTRL_BAR_FRAME_T, *P_CTRL_BAR_FRAME_T;

//3 WLAN Management Frame.
/* 7.2.3.1 WLAN Management Frame - Beacon Frame */
typedef struct _WLAN_BEACON_FRAME_T {
    /* Beacon header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Beacon frame body */
    UINT_32     au4Timestamp[2];            /* Timestamp */
    UINT_16     u2BeaconInterval;           /* Beacon Interval */
    UINT_16     u2CapInfo;                  /* Capability */
    UINT_8      aucInfoElem[1];             /* Various IEs, start from SSID */
} WLAN_BEACON_FRAME_T, *P_WLAN_BEACON_FRAME_T;


typedef struct _WLAN_BEACON_FRAME_BODY_T {
    /* Beacon frame body */
    UINT_32     au4Timestamp[2];            /* Timestamp */
    UINT_16     u2BeaconInterval;           /* Beacon Interval */
    UINT_16     u2CapInfo;                  /* Capability */
    UINT_8      aucInfoElem[1];             /* Various IEs, start from SSID */
} WLAN_BEACON_FRAME_BODY_T, *P_WLAN_BEACON_FRAME_BODY_T;


/* 7.2.3.3 WLAN Management Frame - Disassociation Frame */
typedef struct _WLAN_DISASSOC_FRAME_T {
    /* Authentication MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Disassociation frame body */
    UINT_16     u2ReasonCode;               /* Reason code */
} WLAN_DISASSOC_FRAME_T, *P_WLAN_DISASSOC_FRAME_T;


/* 7.2.3.4 WLAN Management Frame - Association Request frame */
typedef struct _WLAN_ASSOC_REQ_FRAME_T {
    /* Association Request MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Association Request frame body */
    UINT_16     u2CapInfo;                  /* Capability information */
    UINT_16     u2ListenInterval;           /* Listen interval */
    UINT_8      aucInfoElem[1];             /* Information elements, include WPA IE */
} WLAN_ASSOC_REQ_FRAME_T, *P_WLAN_ASSOC_REQ_FRAME_T;


/* 7.2.3.5 WLAN Management Frame - Association Response frame */
typedef struct _WLAN_ASSOC_RSP_FRAME_T {
    /* Association Response MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Association Response frame body */
    UINT_16     u2CapInfo;                  /* Capability information */
    UINT_16     u2StatusCode;               /* Status code */
    UINT_16     u2AssocId;                  /* Association ID */
    UINT_8      aucInfoElem[1];             /* Information elements, such as
                                               supported rates, and etc. */
} WLAN_ASSOC_RSP_FRAME_T, *P_WLAN_ASSOC_RSP_FRAME_T;


/* 7.2.3.6 WLAN Management Frame - Reassociation Request frame */
typedef struct _WLAN_REASSOC_REQ_FRAME_T {
    /* Reassociation Request MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Reassociation Request frame body */
    UINT_16     u2CapInfo;                  /* Capability information */
    UINT_16     u2ListenInterval;           /* Listen interval */
    UINT_8      aucCurrentAPAddr[MAC_ADDR_LEN]; /* Current AP address */
    UINT_8      aucInfoElem[1];             /* Information elements, include WPA IE */
} WLAN_REASSOC_REQ_FRAME_T, *P_WLAN_REASSOC_REQ_FRAME_T;


/* 7.2.3.7 WLAN Management Frame - Reassociation Response frame
   (the same as Association Response frame) */
typedef WLAN_ASSOC_RSP_FRAME_T WLAN_REASSOC_RSP_FRAME_T, *P_WLAN_REASSOC_RSP_FRAME_T;

/* 7.2.3.9 WLAN Management Frame - Probe Response Frame */
typedef WLAN_BEACON_FRAME_T WLAN_PROBE_RSP_FRAME_T, *P_WLAN_PROBE_RSP_FRAME_T;

/* 7.2.3.10 WLAN Management Frame - Authentication Frame */
typedef struct _WLAN_AUTH_FRAME_T {
    /* Authentication MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Authentication frame body */
    UINT_16     u2AuthAlgNum;               /* Authentication algorithm number */
    UINT_16     u2AuthTransSeqNo;           /* Authentication transaction sequence number */
    UINT_16     u2StatusCode;               /* Status code */
    UINT_8      aucInfoElem[1];             /* Various IEs for Fast BSS Transition */
} WLAN_AUTH_FRAME_T, *P_WLAN_AUTH_FRAME_T;


/* 7.2.3.11 WLAN Management Frame - Deauthentication Frame */
typedef struct _WLAN_DEAUTH_FRAME_T {
    /* Authentication MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Deauthentication frame body */
    UINT_16     u2ReasonCode;               /* Reason code */
} WLAN_DEAUTH_FRAME_T, *P_WLAN_DEAUTH_FRAME_T;



//3 Information Elements.
/* 7.3.2 Generic element format */
typedef struct _IE_HDR_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      aucInfo[1];
} IE_HDR_T, *P_IE_HDR_T;

/* 7.3.2.1 SSID element */
typedef struct _IE_SSID_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      aucSSID[ELEM_MAX_LEN_SSID];
} IE_SSID_T, *P_IE_SSID_T;

/* 7.3.2.2 Supported Rates element */
typedef struct _IE_SUPPORTED_RATE_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      aucSupportedRates[ELEM_MAX_LEN_SUP_RATES];
} IE_SUPPORTED_RATE_T, *P_IE_SUPPORTED_RATE_T;

/* 7.3.2.2 Supported Rates element - For Buffalo WHR-G54S AP */
typedef struct _IE_SUPPORTED_RATE_EX_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      aucSupportedRates[255];
} IE_SUPPORTED_RATE_EX_T, *P_IE_SUPPORTED_RATE_EX_T;


/* 7.3.2.4 DS Parameter Set element */
typedef struct _IE_DS_PARAM_SET_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      ucCurrChnl;
} IE_DS_PARAM_SET_T, *P_IE_DS_PARAM_SET_T;

/* 7.3.2.5 CF Parameter Set element */

#ifndef __GNU_C__

__packed struct _IE_CF_PARAM_SET_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      ucCFPCount;
    UINT_8      ucCFPPeriod;
    UINT_16     u2CFPMaxDur;
    UINT_16     u2DurRemaining;
};

typedef struct _IE_CF_PARAM_SET_T IE_CF_PARAM_SET_T, *P_IE_CF_PARAM_SET_T;

#else

typedef struct _IE_CF_PARAM_SET_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      ucCFPCount;
    UINT_8      ucCFPPeriod;
    UINT_16     u2CFPMaxDur;
    UINT_16     u2DurRemaining;
} __attribute__((__packed__)) IE_CF_PARAM_SET_T, *P_IE_CF_PARAM_SET_T;


#endif

/* FMS */
typedef struct _IE_FMS_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      ucNumberOfFmsCnt;
    UINT_8      aucFmsCounterAndFmsId[1];
} IE_FMS_T, *P_IE_FMS_T;


/* 7.3.2.6 TIM */
typedef struct _IE_TIM_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      ucDTIMCount;
    UINT_8      ucDTIMPeriod;
    UINT_8      ucBitmapControl;
    //UINT_8      aucPartialVirtualMap[4];
    UINT_8      aucPartialVirtualMap[1];
} IE_TIM_T, *P_IE_TIM_T;

/* 7.3.2.7 IBSS Parameter Set element */
typedef struct _IE_IBSS_PARAM_SET_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_16     u2ATIMWindow;
} IE_IBSS_PARAM_SET_T, *P_IE_IBSS_PARAM_SET_T;

/* 7.3.2.8 Challenge Text element */
typedef struct _IE_CHALLENGE_TEXT_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      aucChallengeText[ELEM_MAX_LEN_CHALLENGE_TEXT];
} IE_CHALLENGE_TEXT_T, *P_IE_CHALLENGE_TEXT_T;

/* 7.3.2.9 Country information element */
#if CFG_SUPPORT_802_11D
/*! \brief COUNTRY_INFO_TRIPLET is defined for the COUNTRY_INFO_ELEM structure. */
typedef struct _COUNTRY_INFO_TRIPLET_T {
    UINT_8      ucParam1;                 /*!< If param1 >= 201, this triplet is referred to as
                                             Regulatory Triplet in 802_11J. */
    UINT_8      ucParam2;
    UINT_8      ucParam3;
} COUNTRY_INFO_TRIPLET_T, *P_COUNTRY_INFO_TRIPLET_T;

typedef struct _COUNTRY_INFO_SUBBAND_TRIPLET_T {
    UINT_8      ucFirstChnlNum;        /*!< First Channel Number */
    UINT_8      ucNumOfChnl;            /*!< Number of Channels */
    INT_8       cMaxTxPwrLv;        /*!< Maximum Transmit Power Level */
} COUNTRY_INFO_SUBBAND_TRIPLET_T, *P_COUNTRY_INFO_SUBBAND_TRIPLET_T;

typedef struct _COUNTRY_INFO_REGULATORY_TRIPLET_T {
    UINT_8      ucRegExtId;               /*!< Regulatory Extension Identifier, should
                                             be greater than or equal to 201 */
    UINT_8      ucRegClass;               /*!< Regulatory Class */
    UINT_8      ucCoverageClass;          /*!< Coverage Class, unsigned 1-octet value 0~31
                                           , 32~255 reserved */
} COUNTRY_INFO_REGULATORY_TRIPLET_T, *P_COUNTRY_INFO_REGULATORY_TRIPLET_T;

typedef struct _IE_COUNTRY_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      aucCountryStr[3];
    COUNTRY_INFO_SUBBAND_TRIPLET_T arCountryStr[1];
} IE_COUNTRY_T, *P_IE_COUNTRY_T;
#endif /* CFG_SUPPORT_802_11D */

/* 7.3.2.13 ERP element */
typedef struct _IE_ERP_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      ucERP;
} IE_ERP_T, *P_IE_ERP_T;

/* 7.3.2.14 Extended Supported Rates element */
typedef struct _IE_EXT_SUPPORTED_RATE_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      aucExtSupportedRates[ELEM_MAX_LEN_EXTENDED_SUP_RATES];
} IE_EXT_SUPPORTED_RATE_T, *P_IE_EXT_SUPPORTED_RATE_T;

/* 7.3.2.15 Power Constraint element */
typedef struct _IE_POWER_CONSTRAINT_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      ucLocalPowerConstraint;     /* Unit: dBm */
} IE_POWER_CONSTRAINT_T, *P_IE_POWER_CONSTRAINT_T;

/* 7.3.2.16 Power Capability element */
typedef struct _IE_POWER_CAP_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    INT_8       cMinTxPowerCap;     /* Unit: dBm */
    INT_8       cMaxTxPowerCap;     /* Unit: dBm */
} IE_POWER_CAP_T, *P_IE_POWER_CAP_T;

/* 7.3.2.17 TPC request element */
typedef struct _IE_TPC_REQ_T {
    UINT_8      ucId;
    UINT_8      ucLength;
} IE_TPC_REQ_T, *P_IE_TPC_REQ_T;

/* 7.3.2.18 TPC report element */
typedef struct _IE_TPC_REPORT_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    INT_8       cTxPower;       /* Unit: dBm */
    INT_8       cLinkMargin;    /* Unit: dB */
} IE_TPC_REPORT_T, *P_IE_TPC_REPORT_T;

/* 7.3.2.20 Channel Switch Announcement element */
typedef struct _IE_CHNL_SWITCH_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      ucSwitchMode;
    UINT_8      ucNewChannel;
    UINT_8      ucSwitchCount;
} IE_CHNL_SWITCH_T, *P_IE_CHNL_SWITCH_T;

/* 7.3.2.21 Measurement Request element */
typedef struct _IE_MEASUREMENT_REQ_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      ucToken;
    UINT_8      ucRequestMode;
    UINT_8      ucMeasurementType;
    UINT_8      aucRequestFields[1];
} IE_MEASUREMENT_REQ_T, *P_IE_MEASUREMENT_REQ_T;


#ifndef __GNUC__

typedef __packed struct _SM_BASIC_REQ_T {
    UINT_8      ucChannel;
    UINT_32     au4StartTime[2];
    UINT_16     u2Duration;
} SM_BASIC_REQ_T, *P_SM_BASIC_REQ_T;
#else

typedef struct _SM_BASIC_REQ_T {
    UINT_8      ucChannel;
    UINT_32     au4StartTime[2];
    UINT_16     u2Duration;
} __attribute__((__packed__)) SM_BASIC_REQ_T, *P_SM_BASIC_REQ_T;



#endif


/* SM_COMMON_REQ_T is not specified in Spec. Use it as common structure of SM */
typedef SM_BASIC_REQ_T      SM_REQ_COMMON_T, *P_SM_REQ_COMMON_T;
typedef SM_BASIC_REQ_T      SM_CCA_REQ_T, *P_SM_CCA_REQ_T;
typedef SM_BASIC_REQ_T      SM_RPI_HISTOGRAM_REQ_T, *P_SM_RPI_HISTOGRAM_REQ_T;


#ifndef __GNUC__

typedef __packed struct _RM_CHNL_LOAD_REQ_T {
    UINT_8      ucRegulatoryClass;
    UINT_8      ucChannel;
    UINT_16     u2RandomInterval;
    UINT_16     u2Duration;
    UINT_8      aucSubElements[1];
} RM_CHNL_LOAD_REQ_T, *P_RM_CHNL_LOAD_REQ_T;

#else
typedef struct _RM_CHNL_LOAD_REQ_T {
    UINT_8      ucRegulatoryClass;
    UINT_8      ucChannel;
    UINT_16     u2RandomInterval;
    UINT_16     u2Duration;
    UINT_8      aucSubElements[1];
} __attribute__((__packed__)) RM_CHNL_LOAD_REQ_T, *P_RM_CHNL_LOAD_REQ_T;

#endif


typedef RM_CHNL_LOAD_REQ_T \
    RM_NOISE_HISTOGRAM_REQ_T, *P_RM_NOISE_HISTOGRAM_REQ_T;


#ifndef __GNUC__


typedef __packed struct _RM_BCN_REQ_T {
    UINT_8      ucRegulatoryClass;
    UINT_8      ucChannel;
    UINT_16     u2RandomInterval;
    UINT_16     u2Duration;
    UINT_8      ucMeasurementMode;
    UINT_8      aucBssid[6];
    UINT_8      aucSubElements[1];
} RM_BCN_REQ_T, *P_RM_BCN_REQ_T;

#else

typedef struct _RM_BCN_REQ_T {
    UINT_8      ucRegulatoryClass;
    UINT_8      ucChannel;
    UINT_16     u2RandomInterval;
    UINT_16     u2Duration;
    UINT_8      ucMeasurementMode;
    UINT_8      aucBssid[6];
    UINT_8      aucSubElements[1];
} __attribute__((__packed__)) RM_BCN_REQ_T, *P_RM_BCN_REQ_T;

#endif


#ifndef __GNUC__

typedef __packed struct _RM_FRAME_REQ_T {
    UINT_8      ucRegulatoryClass;
    UINT_8      ucChannel;
    UINT_16     u2RandomInterval;
    UINT_16     u2Duration;
    UINT_8      ucFrameReqType;
    UINT_8      aucMacAddr[6];
    UINT_8      aucSubElements[1];
} RM_FRAME_REQ_T, *P_RM_FRAME_REQ_T;

#else
typedef struct _RM_FRAME_REQ_T {
    UINT_8      ucRegulatoryClass;
    UINT_8      ucChannel;
    UINT_16     u2RandomInterval;
    UINT_16     u2Duration;
    UINT_8      ucFrameReqType;
    UINT_8      aucMacAddr[6];
    UINT_8      aucSubElements[1];
} __attribute__((__packed__)) RM_FRAME_REQ_T, *P_RM_FRAME_REQ_T;


#endif


#ifndef __GNUC__

typedef __packed struct _RM_STA_STATS_REQ_T {
    UINT_8      aucPeerMacAddr[6];
    UINT_16     u2RandomInterval;
    UINT_16     u2Duration;
    UINT_8      ucGroupID;
    UINT_8      aucSubElements[1];
} RM_STA_STATS_REQ_T, *P_RM_STA_STATS_REQ_T;

#else
typedef struct _RM_STA_STATS_REQ_T {
    UINT_8      aucPeerMacAddr[6];
    UINT_16     u2RandomInterval;
    UINT_16     u2Duration;
    UINT_8      ucGroupID;
    UINT_8      aucSubElements[1];
} __attribute__((__packed__)) RM_STA_STATS_REQ_T, *P_RM_STA_STATS_REQ_T;


#endif


#ifndef __GNUC__

typedef __packed struct _RM_LCI_REQ_T {
    UINT_8      ucLocationSubject;
    UINT_8      ucLatitudeResolution;
    UINT_8      ucLongitudeResolution;
    UINT_8      ucAltitudeResolution;
    UINT_8      aucSubElements[1];
} RM_LCI_REQ_T, *P_RM_LCI_REQ_T;


#else
typedef struct _RM_LCI_REQ_T {
    UINT_8      ucLocationSubject;
    UINT_8      ucLatitudeResolution;
    UINT_8      ucLongitudeResolution;
    UINT_8      ucAltitudeResolution;
    UINT_8      aucSubElements[1];
} __attribute__((__packed__)) RM_LCI_REQ_T, *P_RM_LCI_REQ_T;



#endif


#ifndef __GNUC__

typedef __packed struct _RM_TS_MEASURE_REQ_T {
    UINT_16     u2RandomInterval;
    UINT_16     u2Duration;
    UINT_8      aucPeerStaAddr[6];
    UINT_8      ucTrafficID;
    UINT_8      ucBin0Range;
    UINT_8      aucSubElements[1];
} RM_TS_MEASURE_REQ_T, *P_RM_TS_MEASURE_REQ_T;

#else
typedef struct _RM_TS_MEASURE_REQ_T {
    UINT_16     u2RandomInterval;
    UINT_16     u2Duration;
    UINT_8      aucPeerStaAddr[6];
    UINT_8      ucTrafficID;
    UINT_8      ucBin0Range;
    UINT_8      aucSubElements[1];
} __attribute__((__packed__)) RM_TS_MEASURE_REQ_T, *P_RM_TS_MEASURE_REQ_T;

#endif

#ifndef __GNU_C__

__packed struct _RM_MEASURE_PAUSE_REQ_T {
    UINT_16     u2PauseTime;
    UINT_8      aucSubElements[1];
};
typedef struct _RM_MEASURE_PAUSE_REQ_T  RM_MEASURE_PAUSE_REQ_T, *P_RM_MEASURE_PAUSE_REQ_T;


#else

typedef struct _RM_MEASURE_PAUSE_REQ_T {
    UINT_16     u2PauseTime;
    UINT_8      aucSubElements[1];
} __attribute__((__packed__)) RM_MEASURE_PAUSE_REQ_T, *P_RM_MEASURE_PAUSE_REQ_T;


#endif


/* 7.3.2.22 Measurement Report element */
typedef struct _IE_MEASUREMENT_REPORT_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      ucToken;
    UINT_8      ucReportMode;
    UINT_8      ucMeasurementType;
    UINT_8      aucReportFields[1];
} IE_MEASUREMENT_REPORT_T, *P_IE_MEASUREMENT_REPORT_T;


#ifndef __GNUC__

typedef __packed struct _SM_BASIC_REPORT_T {
    UINT_8      ucChannel;
    UINT_32     u4StartTime[2];
    UINT_16     u2Duration;
    UINT_8      ucMap;
} SM_BASIC_REPORT_T, *P_SM_BASIC_REPORT_T;

#else

typedef struct _SM_BASIC_REPORT_T {
    UINT_8      ucChannel;
    UINT_32     u4StartTime[2];
    UINT_16     u2Duration;
    UINT_8      ucMap;
} __attribute__((__packed__)) SM_BASIC_REPORT_T, *P_SM_BASIC_REPORT_T;


#endif


#ifndef __GNUC__

typedef __packed struct _SM_CCA_REPORT_T {
    UINT_8      ucChannel;
    UINT_32     u4StartTime[2];
    UINT_16     u2Duration;
    UINT_8      ucCcaBusyFraction;
} SM_CCA_REPORT_T, *P_SM_CCA_REPORT_T;

#else

typedef  struct _SM_CCA_REPORT_T {
    UINT_8      ucChannel;
    UINT_32     u4StartTime[2];
    UINT_16     u2Duration;
    UINT_8      ucCcaBusyFraction;
} __attribute__((__packed__)) SM_CCA_REPORT_T, *P_SM_CCA_REPORT_T;


#endif


#ifndef __GNUC__


typedef __packed struct _SM_RPI_REPORT_T {
    UINT_8      ucChannel;
    UINT_32     u4StartTime[2];
    UINT_16     u2Duration;
    UINT_8      aucRPI[8];
} SM_RPI_REPORT_T, *P_SM_RPI_REPORT_T;

#else
typedef struct _SM_RPI_REPORT_T {
    UINT_8      ucChannel;
    UINT_32     u4StartTime[2];
    UINT_16     u2Duration;
    UINT_8      aucRPI[8];
} __attribute__((__packed__)) SM_RPI_REPORT_T, *P_SM_RPI_REPORT_T;


#endif


#ifndef __GNUC__

typedef __packed struct _RM_CHNL_LOAD_REPORT_T {
    UINT_8      ucRegulatoryClass;
    UINT_8      ucChannel;
    UINT_32     u4StartTime[2];
    UINT_16     u2Duration;
    UINT_8      ucChnlLoad;
} RM_CHNL_LOAD_REPORT_T, *P_RM_CHNL_LOAD_REPORT_T;

#else

typedef struct _RM_CHNL_LOAD_REPORT_T {
    UINT_8      ucRegulatoryClass;
    UINT_8      ucChannel;
    UINT_32     u4StartTime[2];
    UINT_16     u2Duration;
    UINT_8      ucChnlLoad;
} __attribute__((__packed__)) RM_CHNL_LOAD_REPORT_T, *P_RM_CHNL_LOAD_REPORT_T;


#endif


#ifndef __GNUC__

typedef __packed struct _RM_IPI_REPORT_T {
    UINT_8      ucRegulatoryClass;
    UINT_8      ucChannel;
    UINT_32     u4StartTime[2];
    UINT_16     u2Duration;
    UINT_8      ucAntennaId;
    INT_8       cANPI;
    UINT_8      aucIPI[11];
} RM_IPI_REPORT_T, *P_RM_IPI_REPORT_T;

#else
typedef struct _RM_IPI_REPORT_T {
    UINT_8      ucRegulatoryClass;
    UINT_8      ucChannel;
    UINT_32     u4StartTime[2];
    UINT_16     u2Duration;
    UINT_8      ucAntennaId;
    INT_8       cANPI;
    UINT_8      aucIPI[11];
} __attribute__((__packed__)) RM_IPI_REPORT_T, *P_RM_IPI_REPORT_T;


#endif


/* 7.3.2.23 Quiet element */

#ifndef __GNUC__

typedef __packed struct _IE_QUIET_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      ucCount;
    UINT_8      ucPeriod;
    UINT_16     u2Duration;
    UINT_16     u2Offset;
} IE_QUIET_T, *P_IE_QUIET_T;
#else
typedef struct _IE_QUIET_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      ucCount;
    UINT_8      ucPeriod;
    UINT_16     u2Duration;
    UINT_16     u2Offset;
} __attribute__((__packed__)) IE_QUIET_T, *P_IE_QUIET_T;


#endif


/* 7.3.2.27 Extended Capabilities element */
typedef struct _IE_EXT_CAP_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      aucCapabilities[1];
} IE_EXT_CAP_T, *P_EXT_CAP_T;

/* 7.3.2.27 Extended Capabilities element */
typedef struct _IE_RRM_ENABLED_CAP_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      aucCap[5];
} IE_RRM_ENABLED_CAP_T, *P_IE_RRM_ENABLED_CAP_T;


/* 7.3.2.56 HT Capabilities element */

#ifndef __GNUC__

typedef __packed struct _SUP_MCS_SET_FIELD {
    UINT_8      aucRxMcsBitmask[SUP_MCS_RX_BITMASK_OCTET_NUM];
    UINT_16     u2RxHighestSupportedRate;
    UINT_32     u4TxRateInfo;
} SUP_MCS_SET_FIELD, *P_SUP_MCS_SET_FIELD;
#else
typedef struct _SUP_MCS_SET_FIELD {
    UINT_8      aucRxMcsBitmask[SUP_MCS_RX_BITMASK_OCTET_NUM];
    UINT_16     u2RxHighestSupportedRate;
    UINT_32     u4TxRateInfo;
} __attribute__((__packed__)) SUP_MCS_SET_FIELD, *P_SUP_MCS_SET_FIELD;

#endif


#ifndef __GNUC__

typedef __packed struct _IE_HT_CAP_T {
    UINT_8              ucId;
    UINT_8              ucLength;
    UINT_16             u2HtCapInfo;
    UINT_8              ucAmpduParam;
    SUP_MCS_SET_FIELD   rSupMcsSet;
    UINT_16             u2HtExtendedCap;
    UINT_32             u4TxBeamformingCap;
    UINT_8              ucAselCap;
} IE_HT_CAP_T, *P_IE_HT_CAP_T;

#else
typedef struct _IE_HT_CAP_T {
    UINT_8              ucId;
    UINT_8              ucLength;
    UINT_16             u2HtCapInfo;
    UINT_8              ucAmpduParam;
    SUP_MCS_SET_FIELD   rSupMcsSet;
    UINT_16             u2HtExtendedCap;
    UINT_32             u4TxBeamformingCap;
    UINT_8              ucAselCap;
} __attribute__((__packed__)) IE_HT_CAP_T, *P_IE_HT_CAP_T;


#endif


/* 7.3.2.57 HT Operation element */

#ifndef __GNUC__

typedef __packed struct _IE_HT_OP_T {
    UINT_8              ucId;
    UINT_8              ucLength;
    UINT_8              ucPrimaryChannel;
    UINT_8              ucInfo1;
    UINT_16             u2Info2;
    UINT_16             u2Info3;
    UINT_8              aucBasicMcsSet[16];
} IE_HT_OP_T, *P_IE_HT_OP_T;

#else
typedef struct _IE_HT_OP_T {
    UINT_8              ucId;
    UINT_8              ucLength;
    UINT_8              ucPrimaryChannel;
    UINT_8              ucInfo1;
    UINT_16             u2Info2;
    UINT_16             u2Info3;
    UINT_8              aucBasicMcsSet[16];
} __attribute__((__packed__)) IE_HT_OP_T, *P_IE_HT_OP_T;


#endif


#ifndef __GNUC__

/*8.4.2.160.3 VHT Supported MCS Set field*/
typedef __packed struct _VHT_SUPPORTED_MCS_FIELD {
    UINT_16             u2RxMcsMap;
    UINT_16             u2RxHighestSupportedDataRate;
    UINT_16             u2TxMcsMap;
    UINT_16             u2TxHighestSupportedDataRate;
} VHT_SUPPORTED_MCS_FIELD,*P_VHT_SUPPORTED_MCS_FIELD;

typedef __packed struct _IE_VHT_CAP_T {
    UINT_8              ucId;
    UINT_8              ucLength;
    UINT_32             u4VhtCapInfo;
    VHT_SUPPORTED_MCS_FIELD     rVhtSupportedMcsSet;
}  IE_VHT_CAP_T, *P_IE_VHT_CAP_T;


/*8.4.2.161 VHT Operation element*/
typedef __packed struct _IE_VHT_OP_T {
    UINT_8              ucId;
    UINT_8              ucLength;
    UINT_8              ucVhtOperation[3];
    UINT_16             u2VhtBasicMcsSet;
} IE_VHT_OP_T, *P_IE_VHT_OP_T;

#else


/*8.4.2.160.3 VHT Supported MCS Set field*/
typedef struct _VHT_SUPPORTED_MCS_FIELD {
    UINT_16             u2RxMcsMap;
    UINT_16             u2RxHighestSupportedDataRate;
    UINT_16             u2TxMcsMap;
    UINT_16             u2TxHighestSupportedDataRate;
} __attribute__((__packed__)) VHT_SUPPORTED_MCS_FIELD,*P_VHT_SUPPORTED_MCS_FIELD;

typedef struct _IE_VHT_CAP_T {
    UINT_8              ucId;
    UINT_8              ucLength;
    UINT_32             u4VhtCapInfo;
    VHT_SUPPORTED_MCS_FIELD     rVhtSupportedMcsSet;
}__attribute__((__packed__))   IE_VHT_CAP_T, *P_IE_VHT_CAP_T;


/*8.4.2.161 VHT Operation element*/
typedef struct _IE_VHT_OP_T {
    UINT_8              ucId;
    UINT_8              ucLength;
    UINT_8              ucVhtOperation[3];
    UINT_16             u2VhtBasicMcsSet;
} __attribute__((__packed__)) IE_VHT_OP_T, *P_IE_VHT_OP_T;

#endif
/* 7.3.2.58 20/40 BSS Intolerant Channel Report element */
__packed struct _IE_INTOLERANT_CHNL_REPORT_T {
    UINT_8              ucId;
    UINT_8              ucLength;
    UINT_8              ucRegulatoryClass;
    UINT_8              aucChannelList[1];
};
typedef  struct _IE_INTOLERANT_CHNL_REPORT_T IE_INTOLERANT_CHNL_REPORT_T, *P_IE_INTOLERANT_CHNL_REPORT_T;

/* 7.3.2.59 OBSS Scan Parameters element */

#ifndef __GNUC__
typedef __packed struct _IE_OBSS_SCAN_PARAM_T {
    UINT_8              ucId;
    UINT_8              ucLength;
    UINT_16             u2ScanPassiveDwell;
    UINT_16             u2ScanActiveDwell;
    UINT_16             u2TriggerScanInterval;
    UINT_16             u2ScanPassiveTotalPerChnl;
    UINT_16             u2ScanActiveTotalPerChnl;
    UINT_16             u2WidthTransDelayFactor;
    UINT_16             u2ScanActivityThres;
} IE_OBSS_SCAN_PARAM_T, *P_IE_OBSS_SCAN_PARAM_T;

#else

typedef struct _IE_OBSS_SCAN_PARAM_T {
    UINT_8              ucId;
    UINT_8              ucLength;
    UINT_16             u2ScanPassiveDwell;
    UINT_16             u2ScanActiveDwell;
    UINT_16             u2TriggerScanInterval;
    UINT_16             u2ScanPassiveTotalPerChnl;
    UINT_16             u2ScanActiveTotalPerChnl;
    UINT_16             u2WidthTransDelayFactor;
    UINT_16             u2ScanActivityThres;
} __attribute__((__packed__)) IE_OBSS_SCAN_PARAM_T, *P_IE_OBSS_SCAN_PARAM_T;


#endif


/* 7.3.2.60 20/40 BSS Coexistence element */
__packed struct _IE_20_40_COEXIST_T {
    UINT_8              ucId;
    UINT_8              ucLength;
    UINT_8              ucData;
};
typedef  struct _IE_20_40_COEXIST_T IE_20_40_COEXIST_T, *P_IE_20_40_COEXIST_T;


//3 7.4 Action Frame.
/* 7.4 Action frame format */
__packed struct _WLAN_ACTION_FRAME {
    /* Action MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Action frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_8      ucActionDetails[1];         /* Action details */
};
typedef  struct _WLAN_ACTION_FRAME WLAN_ACTION_FRAME, *P_WLAN_ACTION_FRAME;

/* 7.4.1.1 Spectrum Measurement Request frame format */
typedef struct _ACTION_SM_REQ_FRAME {
    /* ADDTS Request MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2Duration;                 /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* ADDTS Request frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_8      ucDialogToken;              /* Dialog Token */
    UINT_8      aucInfoElem[1];             /* Information elements  */
} ACTION_SM_REQ_FRAME, *P_ACTION_SM_REQ_FRAME;

/* 7.4.1.2 Spectrum Measurement Report frame format */
typedef ACTION_SM_REQ_FRAME ACTION_SM_REPORT_FRAME, *P_ACTION_SM_REPORT_FRAME;

/* 7.4.2.1 ADDTS Request frame format */
typedef struct _ACTION_ADDTS_REQ_FRAME {
    /* ADDTS Request MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2Duration;                 /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* ADDTS Request frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_8      ucDialogToken;              /* Dialog Token */
    UINT_8      aucInfoElem[1];             /* Information elements, such as
                                               TS Delay, and etc. */
} ACTION_ADDTS_REQ_FRAME, *P_ACTION_ADDTS_REQ_FRAME;


/* 7.4.2.2 ADDTS Response frame format */
typedef struct _ACTION_ADDTS_RSP_FRAME {
    /* ADDTS Response MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2Duration;                 /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* ADDTS Response frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_8      ucDialogToken;              /* Dialog Token */
    UINT_8      ucStatusCode;               /* WMM Status Code is of one byte */
    UINT_8      aucInfoElem[1];             /* Information elements, such as
                                               TS Delay, and etc. */
} ACTION_ADDTS_RSP_FRAME, *P_ACTION_ADDTS_RSP_FRAME;


/* 7.4.2.3 DELTS frame format */
typedef struct _ACTION_DELTS_FRAME {
    /* DELTS MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* DELTS frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_8      aucTsInfo[3];               /* TS Info */
} ACTION_DELTS_FRAME, *P_ACTION_DELTS_FRAME;


/* 7.4.4.1 ADDBA Request frame format */
typedef struct _ACTION_ADDBA_REQ_FRAME_T {
    /* Action MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Action frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_8      ucDialogToken;              /* Dialog Token chosen by the sender */

    /* for Clockwork check */
    UINT_8      aucBAParameterSet[6];       /* BA policy, TID, buffer size */
    //UINT_8      aucBAParameterSet[2];       /* BA policy, TID, buffer size */
    //UINT_8      aucBATimeoutValue[2];
    //UINT_8      aucBAStartSeqCtrl[2];       /* SSN */
} ACTION_ADDBA_REQ_FRAME_T, *P_ACTION_ADDBA_REQ_FRAME_T;

typedef struct _ACTION_ADDBA_REQ_BODY_T{
    UINT_16      u2BAParameterSet;       /* BA policy, TID, buffer size */
    UINT_16      u2BATimeoutValue;
    UINT_16      u2BAStartSeqCtrl;       /* SSN */
}ACTION_ADDBA_REQ_BODY_T, *P_ACTION_ADDBA_REQ_BODY_T;

/* 7.4.4.2 ADDBA Response frame format */
typedef struct _ACTION_ADDBA_RSP_FRAME_T {
    /* Action MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Action frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_8      ucDialogToken;              /* Dialog Token chosen by the sender */

    /* for Clockwork check */
    UINT_8      aucStatusCode[6];
    //UINT_8      aucStatusCode[2];
    //UINT_8      aucBAParameterSet[2];       /* BA policy, TID, buffer size */
    //UINT_8      aucBATimeoutValue[2];

} ACTION_ADDBA_RSP_FRAME_T, *P_ACTION_ADDBA_RSP_FRAME_T;

typedef struct _ACTION_ADDBA_RSP_BODY_T {
    UINT_16      u2StatusCode;
    UINT_16      u2BAParameterSet;          /* BA policy, TID, buffer size */
    UINT_16      u2BATimeoutValue;
}ACTION_ADDBA_RSP_BODY_T, *P_ACTION_ADDBA_RSP_BODY_T;

/* 7.4.4.3 DELBA frame format */
typedef struct _ACTION_DELBA_FRAME_T {
    /* Action MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Action frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_16     u2DelBaParameterSet;        /* Bit 11 Initiator, Bits 12-15 TID */
    UINT_16     u2ReasonCode;               /* 7.3.1.7 */
} ACTION_DELBA_FRAME_T, *P_ACTION_DELBA_FRAME_T;

/* 7.4.5 Vendor-specific action frame format */
typedef struct _WLAN_VENDOR_SPECIFIC_ACTION_FRAME {
    /* Action MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2DurationID;               /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Action frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      aucOui[3];                   /* OUI */
    UINT_8      ucVendorSpecificContent[1];         /* Action details */
} WLAN_VENDOR_SPECIFIC_ACTION_FRAME, *P_WLAN_VENDOR_SPECIFIC_ACTION_FRAME;


/* 7.4.6.1 Radio Measurement Request frame format */

#ifndef __GNUC__


typedef __packed struct _ACTION_RM_REQ_FRAME {
    /* MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2Duration;                 /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Radio Measurement Request frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_8      ucDialogToken;              /* Dialog Token */
    UINT_16     u2Repetitions;              /* Number of repetitions */
    UINT_8      aucInfoElem[1];             /* Measurement Request elements, such as
                                               channel load request, and etc. */
} ACTION_RM_REQ_FRAME, *P_ACTION_RM_REQ_FRAME;

#else
typedef struct _ACTION_RM_REQ_FRAME {
    /* MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2Duration;                 /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Radio Measurement Request frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_8      ucDialogToken;              /* Dialog Token */
    UINT_16     u2Repetitions;              /* Number of repetitions */
    UINT_8      aucInfoElem[1];             /* Measurement Request elements, such as
                                               channel load request, and etc. */
} __attribute__((__packed__)) ACTION_RM_REQ_FRAME, *P_ACTION_RM_REQ_FRAME;


#endif

#ifndef __GNUC__
/* 7.4.6.2 Radio Measurement Report frame format */
__packed struct _ACTION_RM_REPORT_FRAME {
    /* MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2Duration;                 /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Radio Measurement Report frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_8      ucDialogToken;              /* Dialog Token */
    UINT_8      aucInfoElem[1];             /* Measurement Report elements, such as
                                               channel load report, and etc. */
};
typedef _ACTION_RM_REPORT_FRAME ACTION_RM_REPORT_FRAME, *P_ACTION_RM_REPORT_FRAME; 

/* 7.4.7.1a 20/40 BSS Coexistence Management frame format */
__packed struct _ACTION_20_40_COEXIST_FRAME {
    /* MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2Duration;                 /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* BSS Coexistence Management frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */

    IE_20_40_COEXIST_T          rBssCoexist;/* 20/40 BSS coexistence element */
    IE_INTOLERANT_CHNL_REPORT_T rChnlReport;/* Intolerant channel report */

};
typedef struct _ACTION_20_40_COEXIST_FRAME  ACTION_20_40_COEXIST_FRAME, *P_ACTION_20_40_COEXIST_FRAME;

#else

/* 7.4.6.2 Radio Measurement Report frame format */
typedef struct _ACTION_RM_REPORT_FRAME {
    /* MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2Duration;                 /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* Radio Measurement Report frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_8      ucDialogToken;              /* Dialog Token */
    UINT_8      aucInfoElem[1];             /* Measurement Report elements, such as
                                               channel load report, and etc. */
} __attribute__((__packed__)) ACTION_RM_REPORT_FRAME, *P_ACTION_RM_REPORT_FRAME; 

/* 7.4.7.1a 20/40 BSS Coexistence Management frame format */
typedef struct _ACTION_20_40_COEXIST_FRAME {
    /* MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2Duration;                 /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* BSS Coexistence Management frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */

    IE_20_40_COEXIST_T          rBssCoexist;/* 20/40 BSS coexistence element */
    IE_INTOLERANT_CHNL_REPORT_T rChnlReport;/* Intolerant channel report */

} __attribute__((__packed__)) ACTION_20_40_COEXIST_FRAME, *P_ACTION_20_40_COEXIST_FRAME;
#endif

/* 7.4.10 Notify Channel Width Management frame format */
__packed struct _ACTION_NOTIFY_CHNL_WIDTH_FRAME {
    /* MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2Duration;                 /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* BSS Coexistence Management frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_8      ucChannelWidth;             /* Channel Width */
} __KAL_ATTRIB_PACKED__;
typedef struct _ACTION_NOTIFY_CHNL_WIDTH_FRAME ACTION_NOTIFY_CHNL_WIDTH_FRAME, *P_ACTION_NOTIFY_CHNL_WIDTH_FRAME;

__packed struct _ACTION_SET_PCO_PHASE_FRAME {
    /* MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2Duration;                 /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* BSS Coexistence Management frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_8      ucPcoPhaseCtrl;             /* PCO Phase Control */
} __KAL_ATTRIB_PACKED__;
typedef struct _ACTION_SET_PCO_PHASE_FRAME ACTION_SET_PCO_PHASE_FRAME, *P_ACTION_SET_PCO_PHASE_FRAME;

__packed struct _ACTION_SET_VHT_OPERATING_MODE_FRAME {
    /* MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2Duration;                 /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* VHT Operating Mode Management frame body */
    UINT_8      ucCategory;                 /* Category */
    UINT_8      ucAction;                   /* Action Value */
    UINT_8      ucVhtOperatingMode;             /* VHT opeating Mode */
} __KAL_ATTRIB_PACKED__;
typedef struct _ACTION_SET_VHT_OPERATING_MODE_FRAME  ACTION_SET_VHT_OPERATING_MODE_FRAME, *P_ACTION_SET_VHT_OPERATING_MODE_FRAME;


//3 Information Elements from WFA.
__packed struct _IE_WFA_T {
    UINT_8      ucId;
    UINT_8      ucLength;
    UINT_8      aucOui[3];
    UINT_8      ucOuiType;
    UINT_8      aucOuiSubTypeVersion[2];
    /*!< Please be noted. WPA defines a 16 bit field version
      instead of one subtype field and one version field*/
}__KAL_ATTRIB_PACKED__;
typedef struct _IE_WFA_T IE_WFA_T, *P_IE_WFA_T;


/* CFG_ENABLE_WIFI_DISPLAY */
/* WFD 1.10 5.1.1 */

__packed struct _IE_WFD_T {
    UINT_8      ucId;                   /* Element ID */
    UINT_8      ucLength;               /* Length */
    UINT_8      aucOui[3];              /* OUI */
    UINT_8      ucOuiType;              /* OUI Type */
    UINT_8      aucWFDAttributes[1];    /* WFD Subelement */
}__KAL_ATTRIB_PACKED__;
typedef  struct _IE_WFD_T  IE_WFD_T, *P_IE_WFD_T; 

__packed struct _WFD_ATTRIBUTE_T {
    UINT_8     ucId;                   /* Subelement ID */
    UINT_16    u2Length;               /* Length */
    UINT_8     aucBody[1];             /* Body field */
}__KAL_ATTRIB_PACKED__;
typedef struct _WFD_ATTRIBUTE_T WFD_ATTRIBUTE_T, *P_WFD_ATTRIBUTE_T;

/* WFD 1.10 5.1.2 */
__packed struct _WFD_ATTRI_WFD_DEV_INFO_T {
    UINT_8     ucId;                   /* Subelement ID */
    UINT_16    u2Length;               /* Length */
    UINT_16    u2DevInfo;
    UINT_16    u2ControlPort;
    UINT_16    u2MaximumTp;
}__KAL_ATTRIB_PACKED__;
typedef struct _WFD_ATTRI_WFD_DEV_INFO_T WFD_ATTRI_WFD_DEV_INFO_T, *P_WFD_ATTRI_WFD_DEV_INFO_T;

/* WFD 1.10 5.1.3 */
__packed struct _WFD_ATTRI_ASSOCIATED_BSSID_T {
    UINT_8     ucId;                   /* Subelement ID */
    UINT_16    u2Length;               /* Length */
    UINT_8     aucAssociatedBssid[MAC_ADDR_LEN];
}__KAL_ATTRIB_PACKED__;
typedef struct _WFD_ATTRI_ASSOCIATED_BSSID_T  WFD_ATTRI_ASSOCIATED_BSSID_T, *P_WFD_ATTRI_ASSOCIATED_BSSID_T;

/* WFD 1.10 5.1.4 */
__packed struct _WFD_ATTRI_COUPLED_SINK_STATUS_T {
    UINT_8     ucId;                   /* Subelement ID */
    UINT_16    u2Length;               /* Length */
    UINT_8     ucCoupledSinkStatus;
    UINT_8     aucCoupledSinkAddress[MAC_ADDR_LEN];
}__KAL_ATTRIB_PACKED__;
typedef struct _WFD_ATTRI_COUPLED_SINK_STATUS_T WFD_ATTRI_COUPLED_SINK_STATUS_T, *P_WFD_ATTRI_COUPLED_SINK_STATUS_T;

__packed struct _WFD_ATTRI_EXT_CAPA_BITMAPT {
    UINT_8     ucId;                   /* Subelement ID */
    UINT_16    u2Length;               /* Length */
    UINT_16    u2ExtCapaBitMap;
} __KAL_ATTRIB_PACKED__;
typedef struct _WFD_ATTRI_EXT_CAPA_BITMAPT WFD_ATTRI_EXT_CAPA_BITMAP_T, *P_WFD_ATTRI_EXT_CAPA_BITMAP_T;

/* WFD 1.10 5.1.14 */
__packed struct _WFD_DEV_INFO_DESC_T {
    UINT_8     ucLength;               /* Length = 23*/
    UINT_8     aucDeviceAddress[MAC_ADDR_LEN];
    UINT_8     aucAssociatedBssid[MAC_ADDR_LEN];
    UINT_16    u2DevInfo;
    UINT_16    u2MaximumTp;
    UINT_8     ucCoupledSinkStatus;
    UINT_8     aucCoupledSinkAddress[MAC_ADDR_LEN];
}__KAL_ATTRIB_PACKED__;
typedef struct _WFD_DEV_INFO_DESC_T WFD_DEV_INFO_DESC_T, *P_WFD_DEV_INFO_DESC_T;


__packed struct _WFD_ATTRI_WFD_SESSION_INFO_T {
    UINT_8     ucId;                   /* Subelement ID */
    UINT_16      u2Length;               /* Length */
    WFD_DEV_INFO_DESC_T  arWfdDevDesc[1];
}__KAL_ATTRIB_PACKED__;
typedef struct _WFD_ATTRI_WFD_SESSION_INFO_T  WFD_ATTRI_WFD_SESSION_INFO_T, *P_WFD_ATTRI_WFD_SESSION_INFO_T;


/* P2P 4.2.8.2 P2P Public Action Frame Format */
typedef struct _P2P_PUBLIC_ACTION_FRAME_T {
    /* MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2Duration;                 /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* P2P Public Action Frame Body */
    UINT_8      ucCategory;                 /* Category, 0x04 */
    UINT_8      ucAction;                   /* Action Value, 0x09 */
    UINT_8      aucOui[3];               /* 0x50, 0x6F, 0x9A */
    UINT_8      ucOuiType;              /* 0x09 */
    UINT_8      ucOuiSubtype;         /* GO Nego Req/Rsp/Cfm, P2P Invittion Req/Rsp, Device Discoverability Req/Rsp */
    UINT_8      ucDialogToken;        /* Dialog Token. */
    UINT_8      aucInfoElem[1];             /* P2P IE, WSC IE. */
} __KAL_ATTRIB_PACKED__ P2P_PUBLIC_ACTION_FRAME_T, *P_P2P_PUBLIC_ACTION_FRAME_T;


/* P2P 4.2.9.1 -  General Action Frame Format. */
typedef struct _P2P_ACTION_FRAME_T {
    /* MAC header */
    UINT_16     u2FrameCtrl;                /* Frame Control */
    UINT_16     u2Duration;                 /* Duration */
    UINT_8      aucDestAddr[MAC_ADDR_LEN];  /* DA */
    UINT_8      aucSrcAddr[MAC_ADDR_LEN];   /* SA */
    UINT_8      aucBSSID[MAC_ADDR_LEN];     /* BSSID */
    UINT_16     u2SeqCtrl;                  /* Sequence Control */
    /* P2P Action Frame Body */
    UINT_8 ucCategory;             // 0x7F
    UINT_8 aucOui[3];               // 0x50, 0x6F, 0x9A
    UINT_8 ucOuiType;              // 0x09
    UINT_8 ucOuiSubtype;         //
    UINT_8 ucDialogToken;
    UINT_8 aucInfoElem[1];
} __KAL_ATTRIB_PACKED__ P2P_ACTION_FRAME_T, *P_P2P_ACTION_FRAME_T;


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
/* Convert the ECWmin(max) to CWmin(max) */
#define ECW_TO_CW(_ECW)         ((1 << (_ECW)) - 1)

/* Convert the RCPI to dBm */
#define RCPI_TO_dBm(_rcpi)                          \
    ((PARAM_RSSI)(((_rcpi) > RCPI_HIGH_BOUND ? RCPI_HIGH_BOUND : (_rcpi)) >> 1) - NDBM_LOW_BOUND_FOR_RCPI)

#if 0
/* Convert the dBm to RCPI */
#define dBm_TO_RCPI(_dbm)                           \
    (RCPI)( ( (((PARAM_RSSI)(_dbm) + NDBM_LOW_BOUND_FOR_RCPI) << 1) > RCPI_HIGH_BOUND) ? RCPI_HIGH_BOUND : \
            ( (((PARAM_RSSI)(_dbm) + NDBM_LOW_BOUND_FOR_RCPI) << 1) < RCPI_LOW_BOUND ? RCPI_LOW_BOUND : \
             (((PARAM_RSSI)(_dbm) + NDBM_LOW_BOUND_FOR_RCPI) << 1) ) )
#endif

/* Convert an unsigned char pointer to an information element pointer */
#define IE_ID(fp)               (((P_IE_HDR_T) fp)->ucId)
#define IE_LEN(fp)              (((P_IE_HDR_T) fp)->ucLength)
#define IE_SIZE(fp)             (ELEM_HDR_LEN + IE_LEN(fp))

#define SSID_IE(fp)             ((P_IE_SSID_T) fp)

#define SUP_RATES_IE(fp)        ((P_IE_SUPPORTED_RATE_T) fp)

#define DS_PARAM_IE(fp)         ((P_IE_DS_PARAM_SET_T) fp)

#define TIM_IE(fp)              ((P_IE_TIM_T) fp)

#define IBSS_PARAM_IE(fp)       ((P_IE_IBSS_PARAM_SET_T) fp)

#define ERP_INFO_IE(fp)         ((P_IE_ERP_T) fp)

#define EXT_SUP_RATES_IE(fp)    ((P_IE_EXT_SUPPORTED_RATE_T) fp)

#define WFA_IE(fp)              ((P_IE_WFA_T) fp)

#if CFG_SUPPORT_802_11D
#define COUNTRY_IE(fp)          ((P_IE_COUNTRY_T) fp)
#endif

#define EXT_CAP_IE(fp)          ((P_EXT_CAP_T) fp)

#define HT_CAP_IE(fp)           ((P_IE_HT_CAP_T) fp)

#define HT_OP_IE(fp)            ((P_IE_HT_OP_T) fp)

#define OBSS_SCAN_PARAM_IE(fp)  ((P_IE_OBSS_SCAN_PARAM_T) fp)

#define BSS_20_40_COEXIST_IE(fp) ((P_IE_20_40_COEXIST_T) fp)

#define QUIET_IE(fp)            ((P_IE_QUIET_T) fp)



/* The macro to check if the MAC address is B/MCAST Address */
#define IS_BMCAST_MAC_ADDR(_pucDestAddr)            \
    ((BOOLEAN) ( ((PUINT_8)(_pucDestAddr))[0] & BIT(0) ))

/* The macro to check if the MAC address is UCAST Address */
#define IS_UCAST_MAC_ADDR(_pucDestAddr)             \
    ((BOOLEAN) !( ((PUINT_8)(_pucDestAddr))[0] & BIT(0) ))

/* The macro to copy the MAC address */
#define COPY_MAC_ADDR(_pucDestAddr, _pucSrcAddr)    \
	rom_memcpy(_pucDestAddr, _pucSrcAddr, MAC_ADDR_LEN)

/* The macro to check if two MAC addresses are equal */
#define EQUAL_MAC_ADDR(_pucDestAddr, _pucSrcAddr)   \
    (!rom_memcmp(_pucDestAddr, _pucSrcAddr, MAC_ADDR_LEN))

/* The macro to check if two MAC addresses are not equal */
#define UNEQUAL_MAC_ADDR(_pucDestAddr, _pucSrcAddr) \
    (rom_memcmp(_pucDestAddr, _pucSrcAddr, MAC_ADDR_LEN))


/* The macro to check whether two SSIDs are equal */
#define EQUAL_SSID(pucSsid1, ucSsidLen1, pucSsid2, ucSsidLen2) \
    (((ucSsidLen1) == (ucSsidLen2)) && \
        !rom_memcmp(pucSsid1, pucSsid2, ucSsidLen1))

/* The macro to check whether two SSIDs are equal */
#define UNEQUAL_SSID(pucSsid1, ucSsidLen1, pucSsid2, ucSsidLen2) \
    (((ucSsidLen1) != (ucSsidLen2)) || \
        rom_memcmp(pucSsid1, pucSsid2, ucSsidLen1))

/* The macro to copy the SSID, the length of pucDestSsid should have at least 32 bytes */
#define COPY_SSID(pucDestSsid, ucDestSsidLen, pucSrcSsid, ucSrcSsidLen) \
    do { \
        ucDestSsidLen = ucSrcSsidLen; \
        if (ucSrcSsidLen) { \
            LITE_ASSERT(ucSrcSsidLen <= ELEM_MAX_LEN_SSID); \
            rom_memcpy(pucDestSsid, pucSrcSsid, ((ucSrcSsidLen > ELEM_MAX_LEN_SSID) ? ELEM_MAX_LEN_SSID : ucSrcSsidLen)); \
        } \
    } while (FALSE)

/* The macro to copy the IE */
#define COPY_IE(pucDestIE, pucSrcIE) \
    do { \
        kalMemCopy((PUINT_8)pucDestIE, \
                   (PUINT_8)pucSrcIE,\
                   IE_SIZE(pucSrcIE)); \
    } while (FALSE)

#define IE_FOR_EACH(_pucIEsBuf, _u2IEsBufLen, _u2Offset) \
    for ((_u2Offset) = 0; ((_u2Offset) < (_u2IEsBufLen)); \
        (_u2Offset) += IE_SIZE(_pucIEsBuf), ((_pucIEsBuf) += IE_SIZE(_pucIEsBuf)) )

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
#ifndef _lint
/* Kevin: we don't have to call following function to inspect the data structure.
 * It will check automatically while at compile time.
 * We'll need this for porting driver to different RTOS.
 */
__KAL_INLINE__ VOID
macDataTypeCheck (
    VOID
    )
{
    DATA_STRUC_INSPECTING_ASSERT(sizeof(ETH_FRAME_T) == (15 +(1)));
    DATA_STRUC_INSPECTING_ASSERT(sizeof(WLAN_MAC_HEADER_T) == 24);
    DATA_STRUC_INSPECTING_ASSERT(sizeof(WLAN_MAC_HEADER_QOS_T) == 26);
    DATA_STRUC_INSPECTING_ASSERT(sizeof(WLAN_MAC_HEADER_A4_T) == 30);
    DATA_STRUC_INSPECTING_ASSERT(sizeof(WLAN_MAC_HEADER_A4_QOS_T) == 32);
    DATA_STRUC_INSPECTING_ASSERT(sizeof(WLAN_MAC_MGMT_HEADER_T) == 24);
    DATA_STRUC_INSPECTING_ASSERT(sizeof(WLAN_BEACON_FRAME_T) == (24+13 +(3)));
    DATA_STRUC_INSPECTING_ASSERT(sizeof(WLAN_BEACON_FRAME_BODY_T) == (13 +(3)));
    DATA_STRUC_INSPECTING_ASSERT(sizeof(WLAN_ASSOC_REQ_FRAME_T) == (24+5 +(1)));
    DATA_STRUC_INSPECTING_ASSERT(sizeof(WLAN_ASSOC_RSP_FRAME_T) == (24+7 +(1)));
    DATA_STRUC_INSPECTING_ASSERT(sizeof(WLAN_REASSOC_REQ_FRAME_T) == (24+11 +(1)));
    DATA_STRUC_INSPECTING_ASSERT(sizeof(WLAN_AUTH_FRAME_T) == (24+7 +(1)));
    DATA_STRUC_INSPECTING_ASSERT(sizeof(WLAN_DEAUTH_FRAME_T) == (24+2));


    DATA_STRUC_INSPECTING_ASSERT(sizeof(IE_HDR_T) == (2+1));
    DATA_STRUC_INSPECTING_ASSERT(sizeof(IE_SSID_T) == (2+32));
    DATA_STRUC_INSPECTING_ASSERT(sizeof(IE_SUPPORTED_RATE_T) == (2+8));
    DATA_STRUC_INSPECTING_ASSERT(sizeof(IE_DS_PARAM_SET_T) == (2+1));
    DATA_STRUC_INSPECTING_ASSERT(sizeof(IE_IBSS_PARAM_SET_T) == (2+2));
    DATA_STRUC_INSPECTING_ASSERT(sizeof(IE_CHALLENGE_TEXT_T) == (2+253));
    DATA_STRUC_INSPECTING_ASSERT(sizeof(IE_ERP_T) == (2+1));
    DATA_STRUC_INSPECTING_ASSERT(sizeof(IE_EXT_SUPPORTED_RATE_T) == (2+255));

    DATA_STRUC_INSPECTING_ASSERT(sizeof(IE_WFA_T) == (2+6));

    return;
}
#endif /* _lint */

#endif /* _MAC_H */

