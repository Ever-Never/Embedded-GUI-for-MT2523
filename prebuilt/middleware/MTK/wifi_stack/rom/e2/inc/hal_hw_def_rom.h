/*
** $Id: //Department/DaVinci/TRUNK/MT6628_FPGA_BORA_BASE_20111109/mcu/wifi/ROM/include/nic/hal/MT6628/mt6628_hw_rom.h#1 $
*/

/*! \file   mt592x_hw.h
    \brief  Definition of data structures which are CHIP dependent.

    In this file we define the major data structures which are CHIP dependent.

    SW_TFCB - Maintain SW variable that are required by CHIP(MAC).
    HW_TFCB - A data structure that is identical (field offset/width) with HW TX
              Frame Control Block.
              We need to write this Control Block first before fill in entire frame to HW.
    TX_STATUS - A data structure that is identical (field offset/width) with HW
                Transmit Status Structure.

    SW_RFB - Maintain SW variable for receiving path.
    HW_RFB - MT592X series is slave mode only, we'll define HW_RFB only for CHIP
             which has master mode.
    RX_STATUS - A data structure that is identical (field offset/width) with HW
                Receive Status Structure.
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
** $Log: mt6630_hw.h $
**
** 11 17 2014 changyi.hsu
** [BORA00003214] [MT7637]
** 1. Add RXP test cases.
**
** 11 03 2014 ted.huang
** [BORA00003379] [MT7637]
** 1)WTBL entry support 128, 2) add repeater/MBSS/Indium emulation cases.
**
** 11 24 2013 ted.huang
** [BORA00002807] [MT7636]
** 1. add RX SMPS emulation code 2. merge ROM from 6630E2.
**
** 06 14 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** 1. Add initial rate seting. 2. Add more information in sta command 3. add VHT info
**
** 06 10 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** .
**
** 06 07 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** .
**
** 06 07 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** Fix the pf macro and status bit
**
** 06 04 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** .
**
** 05 30 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Add Full score test case
** Add duplicate RFB test case
** Add IP/TCP/UDP check sum flag check conditin in RFB
**
** 05 24 2013 pat.lu
** [BORA00002365] [WLAN] Sec. and WTBL FPGA Emulation Modification
** .
** 1. Enable halgetRFBW API
** 2. Add RX Vectore scrambling seed defintion for signaling RTS BW indication
**
** 05 22 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** 1. Add Timing Advertisment frame type definition
** 2. Add WTBL PCR/PCSR defintion
** 3. Add RX Vectore other antenna bit definition
**
** 05 20 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** Update RDG emulation
**
** 05 20 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** add rx security emulation code.
**
** 05 14 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Modify LQ filed mask definition in RX Vector Group 2 according to updated CFG_TOP_CR
**
** 05 10 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Add AGC Gain defintion
**
** 05 07 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** .
**
** 05 06 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Update TMAC emulation code(Beacon TX OK)
**
** 05 03 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** fix non-auto rate code
**
** 05 03 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** update AR emulation code
**
** 04 22 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** <saved by Perforce>
** Fix Link error
**
** 04 17 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** 1. New per-BSS ERP protection.
** 2. New TX rate code definition
** 3. RTS retry limit extended to 32
**
** 04 16 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** .
**
** 04 10 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Modify RFB data, AMP, ASF bit defition according to new RX foramt document
**
** 04 08 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Modify related codes and defintion for new wtbl search rule and RFB security RX error defintion
** Modify WTBL search codes
**
** 04 01 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** 1.Update BSSID basic reate HAL function
** 2.Change emulTX packet size from 2000 to 1600 bytes
** 3.Synth. emulation TX test cases
**
** 04 01 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** Update Tx descriptor HAL.
**
** 03 14 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** Update Tx remaining life time
**
** 03 13 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** Add utility function for frame/QoS control field
**
** 03 13 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** 1. Fix NAV, rx duplicate filter codes
** 2. Modify codes for header translation, TFT test cases.
** 3. Modify defintion for TFT test cases
**
** 03 12 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Use Ratecode to set TX rate
**
** 03 08 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** Fix compile error ( protection rate )
**
** 03 07 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** ADD TX done handling
**
** 03 06 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** Reconciled offline work
**
** 03 05 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** Unify TX Q index definition
**
** 03 04 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** Reconciled offline work
**
** 02 27 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Fixed TMAC emulation error
**
** 02 25 2013 pat.lu
** [BORA00002365] [WLAN] Sec. and WTBL FPGA Emulation Modification
** .
** 1. Add and correct hal function for multicast address set
** 2. Add definition of RX status about packet format (PF)
** 3. Correct some RX emulation codes
** 4. Correct dataframe.c for AMPDU delimiter compsing
** 5. Add emulation WTBL function.
**
** 02 05 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Update TX emulation code
**
** 01 30 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** . Modify RX Emulation related codes and Fix emulTxHelp.h build warning
**
** 01 24 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Fixed compiler error when enable CFG_TEST_TX_EMULATION
**
** 01 21 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Enable CFG_TEST_TX_EMULATION flag
**
** 01 15 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** Update txmSendHifDataPackets
**
** 01 04 2013 chinglan.wang
** [BORA00002293] [MT6630]Develop TX emulation code
** .
**
** 12 19 2012 chinglan.wang
** [BORA00002293] [MT6630]Develop TX emulation code
** .
**
** 12 14 2012 chinglan.wang
** [BORA00002293] [MT6630]Develop TX emulation code
** .
**
** 12 12 2012 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** .
**
** 11 22 2012 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Update RMAC emulation code
**
** 11 14 2012 eason.tsai
** [BORA00002212] [MT6630 Wi-Fi][Rom]develop
** add TXD macro6
**
** 11 05 2012 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** .
**
** 10 16 2012 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** Add old MACRO for ROM build pass, to be removed
**
** 10 12 2012 terry.wu
** [BORA00002207]
** [MT6630 Wi-Fi] Update TX descriptor MACRO
**
** 10 09 2012 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** RAM code include ROM code symbol(gorm_ref.sym)
**
** 10 05 2012 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Rename file by removing chip ID
**
** 09 21 2012 yarco.yang
** [BORA00002091] [MT6630]Add compiler flag
** .
**
** 08 30 2012 yarco.yang
** [BORA00002091] [MT6630]Add compiler flag
**
*/

#ifndef _MT6630_WIFI_HW_H
#define _MT6630_WIFI_HW_H

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
/* The rate define at MT6630 spec */
/* For Protection Frame/ CF_End/ BAR Rate */

#define TX_MODE_CCK                  0x00
#define TX_MODE_OFDM               0x40
#define TX_MODE_HT_MM             0x80
#define TX_MODE_HT_GF              0xC0
#define TX_MODE_VHT               0x100


#define RATE_CCK_SHORT_PREAMBLE     0x4
//#define RATE_OFDM                                 0x20

#define PHY_RATE_1M                            0x0
#define PHY_RATE_2M                            0x1
#define PHY_RATE_5_5M                        0x2
#define PHY_RATE_11M                          0x3
#define PHY_RATE_6M                            0xB
#define PHY_RATE_9M                            0xF
#define PHY_RATE_12M                          0xA
#define PHY_RATE_18M                          0xE
#define PHY_RATE_24M                          0x9
#define PHY_RATE_36M                          0xD
#define PHY_RATE_48M                          0x8
#define PHY_RATE_54M                          0xC
#define PHY_RATE_MCS0                        0x0
#define PHY_RATE_MCS1                        0x1
#define PHY_RATE_MCS2                        0x2
#define PHY_RATE_MCS3                        0x3
#define PHY_RATE_MCS4                        0x4
#define PHY_RATE_MCS5                        0x5
#define PHY_RATE_MCS6                        0x6
#define PHY_RATE_MCS7                        0x7
#define PHY_RATE_MCS8             0x8
#define PHY_RATE_MCS9             0x9
#define PHY_RATE_MCS32                      0x20


#define RATE_CCK_1M_LONG              (TX_MODE_CCK | PHY_RATE_1M)
#define RATE_CCK_2M_LONG              (TX_MODE_CCK | PHY_RATE_2M)
#define RATE_CCK_5_5M_LONG          (TX_MODE_CCK | PHY_RATE_5_5M)
#define RATE_CCK_11M_LONG            (TX_MODE_CCK | PHY_RATE_11M)
#define RATE_CCK_2M_SHORT            (TX_MODE_CCK | PHY_RATE_2M | RATE_CCK_SHORT_PREAMBLE)
#define RATE_CCK_5_5M_SHORT        (TX_MODE_CCK | PHY_RATE_5_5M | RATE_CCK_SHORT_PREAMBLE)
#define RATE_CCK_11M_SHORT          (TX_MODE_CCK | PHY_RATE_11M | RATE_CCK_SHORT_PREAMBLE)
#define RATE_OFDM_6M                      (TX_MODE_OFDM | PHY_RATE_6M)
#define RATE_OFDM_9M                      (TX_MODE_OFDM | PHY_RATE_9M)
#define RATE_OFDM_12M                    (TX_MODE_OFDM |  PHY_RATE_12M)
#define RATE_OFDM_18M                    (TX_MODE_OFDM |  PHY_RATE_18M)
#define RATE_OFDM_24M                    (TX_MODE_OFDM |  PHY_RATE_24M)
#define RATE_OFDM_36M                    (TX_MODE_OFDM |  PHY_RATE_36M)
#define RATE_OFDM_48M                    (TX_MODE_OFDM |  PHY_RATE_48M)
#define RATE_OFDM_54M                    (TX_MODE_OFDM |  PHY_RATE_54M)



#define RATE_MM_MCS_0                    (TX_MODE_HT_MM | PHY_RATE_MCS0)
#define RATE_MM_MCS_1                    (TX_MODE_HT_MM | PHY_RATE_MCS1)
#define RATE_MM_MCS_2                    (TX_MODE_HT_MM | PHY_RATE_MCS2)
#define RATE_MM_MCS_3                    (TX_MODE_HT_MM | PHY_RATE_MCS3)
#define RATE_MM_MCS_4                    (TX_MODE_HT_MM | PHY_RATE_MCS4)
#define RATE_MM_MCS_5                    (TX_MODE_HT_MM | PHY_RATE_MCS5)
#define RATE_MM_MCS_6                    (TX_MODE_HT_MM | PHY_RATE_MCS6)
#define RATE_MM_MCS_7                    (TX_MODE_HT_MM | PHY_RATE_MCS7)
#define RATE_MM_MCS_32                  (TX_MODE_HT_MM | PHY_RATE_MCS32)

#define RATE_GF_MCS_0                    (TX_MODE_HT_GF | PHY_RATE_MCS0)
#define RATE_GF_MCS_1                    (TX_MODE_HT_GF | PHY_RATE_MCS1)
#define RATE_GF_MCS_2                    (TX_MODE_HT_GF | PHY_RATE_MCS2)
#define RATE_GF_MCS_3                    (TX_MODE_HT_GF | PHY_RATE_MCS3)
#define RATE_GF_MCS_4                    (TX_MODE_HT_GF | PHY_RATE_MCS4)
#define RATE_GF_MCS_5                    (TX_MODE_HT_GF | PHY_RATE_MCS5)
#define RATE_GF_MCS_6                    (TX_MODE_HT_GF | PHY_RATE_MCS6)
#define RATE_GF_MCS_7                    (TX_MODE_HT_GF | PHY_RATE_MCS7)
#define RATE_GF_MCS_32                  (TX_MODE_HT_GF | PHY_RATE_MCS32)

#define RATE_VHT_MCS_0                   (TX_MODE_VHT | PHY_RATE_MCS0)
#define RATE_VHT_MCS_1                   (TX_MODE_VHT | PHY_RATE_MCS1)
#define RATE_VHT_MCS_2                   (TX_MODE_VHT | PHY_RATE_MCS2)
#define RATE_VHT_MCS_3                   (TX_MODE_VHT | PHY_RATE_MCS3)
#define RATE_VHT_MCS_4                   (TX_MODE_VHT | PHY_RATE_MCS4)
#define RATE_VHT_MCS_5                   (TX_MODE_VHT | PHY_RATE_MCS5)
#define RATE_VHT_MCS_6                   (TX_MODE_VHT | PHY_RATE_MCS6)
#define RATE_VHT_MCS_7                   (TX_MODE_VHT | PHY_RATE_MCS7)
#define RATE_VHT_MCS_8                   (TX_MODE_VHT | PHY_RATE_MCS8)
#define RATE_VHT_MCS_9                   (TX_MODE_VHT | PHY_RATE_MCS9)


#define RATE_TX_MODE_MASK           BITS(6,8)
#define RATE_TX_MODE_OFFSET       6
#define RATE_CODE_GET_TX_MODE(_ucRateCode)  ((_ucRateCode & RATE_TX_MODE_MASK) >> RATE_TX_MODE_OFFSET)
#define RATE_PHY_RATE_MASK           BITS(0,5)
#define RATE_PHY_RATE_OFFSET        0
#define RATE_CODE_GET_PHY_RATE(_ucRateCode)  ((_ucRateCode & RATE_PHY_RATE_MASK) >> RATE_PHY_RATE_OFFSET)
#define RATE_PHY_RATE_SHORT_PREAMBLE      BIT(2)
#define RATE_CODE_IS_SHORT_PREAMBLE(_ucRateCode)  ((_ucRateCode & RATE_PHY_RATE_SHORT_PREAMBLE)?TRUE:FALSE)

/*------------------------------------------------------------------------------
 * Cipher define
 *------------------------------------------------------------------------------
*/
#define CIPHER_SUITE_NONE               0
#define CIPHER_SUITE_WEP40              1
#define CIPHER_SUITE_TKIP               2
#define CIPHER_SUITE_TKIP_WO_MIC        3
#define CIPHER_SUITE_CCMP               4
#define CIPHER_SUITE_WEP104             5
#define CIPHER_SUITE_BIP                6
#define CIPHER_SUITE_WEP128             7
#define CIPHER_SUITE_WPI                8
#define CIPHER_SUITE_CCMP_W_CCX         9
#define CIPHER_SUITE_CCMP_256           10
#define CIPHER_SUITE_GCMP_128           11
#define CIPHER_SUITE_GCMP_256           12


/*------------------------------------------------------------------------------
 * Bit fields for HW_MAC_RX_DESC_T
 *------------------------------------------------------------------------------
*/

/*! MAC RX DMA Descriptor */
/* DW 0*/
/* Word 0 */
#define RX_STATUS_RX_BYTE_COUNT_MASK    BITS(0,16)
/* Word 1 */
#define RX_STATUS_ETH_TYPE_OFFSET_MASK  BITS(0,6)
#define RX_STATUS_ETH_TYPE_OFFSET       0
#define RX_STATUS_IP_CHKSUM             BIT(7)
#define RX_STATUS_UDP_TCP_CHKSUM        BIT(8)
#define RX_STATUS_GROUP_VLD_MASK        BITS(9,12)
#define RX_STATUS_GROUP_VLD_OFFSET      9
#define RX_STATUS_PKT_TYPE_MASK         BITS(13,15)
#define RX_STATUS_PKT_TYPE_OFFSET       13

/* DW 1 */
/* Byte 0 */
#define RX_STATUS_HTC                   BIT(0)
#define RX_STATUS_UC2ME                 BIT(1)
#define RX_STATUS_MC_FRAME              BIT(2)
#define RX_STATUS_BC_FRAME              BIT(3)
#define RX_STATUS_BCN_WITH_BMC          BIT(4)
#define RX_STATUS_BCN_WITH_UC           BIT(5)
#define RX_STATUS_KEYID_MASK            BITS(6,7)
#define RX_STATUS_KEYID_OFFSET          6

/* Byte 1 */
#define RX_STATUS_CHAN_FREQ_MASK            BITS(0,7)
/* Byte 2 */
#define RX_STATUS_HEADER_LEN_MASK           BITS(0,5)
#define RX_STATUS_HEADER_OFFSET             BIT(6)
#define RX_STATUS_HEADER_TRAN               BIT(7)
/* Byte 3 */
#define RX_STATUS_PAYLOAD_FORMAT_MASK       BITS(0,1)
#define RX_STATUS_PAYLOAD_FORMAT_OFFSET     0
#define RX_STATUS_BSSID_MASK                BITS(2,7)
#define RX_STATUS_BSSID_OFFSET              2
#define PAYLOAD_FORMAT_IS_MSDU_FRAME        0
#define PAYLOAD_FORMAT_1ST_AMSDU_SUBFRAME   3
#define PAYLOAD_FORMAT_MID_AMSDU_SUBFRAME   2
#define PAYLOAD_FORMAT_FIN_AMSDU_SUBFRAME   1


/* DW 2 */
/* Byte 1 */
#define RX_STATUS_TID_MASK              BITS(0,3)
#define RX_STATUS_SEC_MASK              BITS(4,7)
#define RX_STATUS_SEC_OFFSET            4
/* Byte 2-3 */
#define RX_STATUS_SW_BIT                BIT(0)
#define RX_STATUS_FLAG_FCS_ERROR        BIT(1)
//#define RX_STATUS_FLAG_FORMAT_ERROR     BIT(3)
#define RX_STATUS_FLAG_CIPHER_MISMATCH  BIT(2)
#define RX_STATUS_FLAG_CLM_ERROR        BIT(3)
#define RX_STATUS_FLAG_ICV_ERROR        BIT(4)
#define RX_STATUS_FLAG_TKIPMIC_ERROR    BIT(5)
#define RX_STATUS_FLAG_LEN_MISMATCH     BIT(6)
#define RX_STATUS_FLAG_DE_AMSDU_FAIL    BIT(7)
#define RX_STATUS_FLAG_EXCEED_LEN       BIT(8)
#define RX_STATUS_LLC_MIS               BIT(9)
#define RX_STATUS_UDF_VLT               BIT(10)
#define RX_STATUS_FRAG                  BIT(11)
#define RX_STATUS_NULL                  BIT(12)
#define RX_STATUS_DATA                  BIT(13)
#define RX_STATUS_AMPDU_SUB_FRAME       BIT(14)
#define RX_STATUS_AMPDU_FORMAT          BIT(15)
#define RX_STATUS_FLAG_ERROR_MASK      (RX_STATUS_FLAG_FCS_ERROR | RX_STATUS_FLAG_ICV_ERROR | \
                                        RX_STATUS_FLAG_CLM_ERROR) // No TKIP MIC error

/* DW 3 */
#define RX_STATUS_RXV_SEQ_NO_MASK       BITS(0,7)
#define RX_STATUS_TCL                   BIT(8)
#define RX_STATUS_DROP                  BIT(9)
#define RX_STATUS_CLS                   BIT(10)
#define RX_STATUS_OFLD_MASK             BITS(11,12)
#define RX_STATUS_OFLD_OFFSET           11
#define RX_STATUS_EAPOL_PACKET          BIT(11)
#define RX_STATUS_ARP_NS_PACKET         BIT(12)
#define RX_STATUS_TDLS_PACKET           BITS(11,12)
#define RX_STATUS_MGC                   BIT(13)
#define RX_STATUS_WOL_MASK              BITS(14,18)
#define RX_STATUS_WOL_OFFSET            14
#define RX_STATUS_CLS_BITMAP_MASK       BITS(19,28)
#define RX_STATUS_CLS_BITMAP_OFFSET     19
#define RX_STATUS_PF_MODE               BIT(29)
#define RX_STATUS_PF_STS_MASK           BITS(30,31)
#define RX_STATUS_PF_STS_OFFSET         30


/* DW 12 */
#define RX_STATUS_FRAG_NUM_MASK         BITS(0,3)
#define RX_STATUS_SEQ_NUM_MASK          BITS(4,15)
#define RX_STATUS_SEQ_NUM_OFFSET        4


#define RX_STATUS_GROUP1_VALID    BIT(0)
#define RX_STATUS_GROUP2_VALID    BIT(1)
#define RX_STATUS_GROUP3_VALID    BIT(2)
#define RX_STATUS_GROUP4_VALID    BIT(3)

#define RX_STATUS_FIXED_LEN       16

#define RX_STATUS_CHAN_FREQ_MASK_FOR_BY_PASS_MPDE      BITS(0,7)
#define RX_STATUS_FLAG_FCS_ERROR_FOR_BY_PASS_MODE      BIT(16)


/* Timing Measurement Report */
/* DW0 Word 1 */
#define RX_TMR_TOA_VALID          BIT(11)
#define RX_TMR_TOD_VALID          BIT(10)
#define RX_TMR_TYPE_MASK          BITS(8,9)
#define RX_TMR_TYPE_OFFSET        8
#define RX_TMR_SUBTYPE_MASK       BITS(4,7)
#define RX_TMR_SUBTYPE_OFFSET     4

/* DW0 Byte 1*/
#define RX_TMR_TM_FAILED          BIT(2)
#define RX_TMR_NOISY_CHAN         BIT(1)
#define RX_TMR_RESPONDER          BIT(0)



//TBD
#if 0
#define DMA_OWN_TO_HW          BIT(0)
#define DMA_OWN_TO_FW_PENDING  BIT(1)
#define STATUS_IS_OWN_TO_FW(flag)   (((flag) & DMA_OWN_TO_HW) ? FALSE : TRUE)
#define STATUS_IS_FW_PENDING(flag)  (((flag) & DMA_OWN_TO_FW_PENDING) ? TRUE : FALSE)
#endif

/* DW 2 */
#define RX_STATUS_PACKET_LENGTH_MASK    BITS(0,16)


#define RX_STATUS_HEADER_TRAN_MASK          BIT(7)
#define RX_STATUS_HEADER_TRAN_OFFSET        7
#define RX_STATUS_HEADER_TRAN_BSS0_MASK     BIT(6)
#define RX_STATUS_HEADER_TRAN_BSS0_OFFSET   6
#define RX_STATUS_HEADER_TRAN_BSS1_MASK     BIT(7)
#define RX_STATUS_HEADER_TRAN_BSS1_OFFSET   7


/* DW 4 */
#define RX_STATUS_MATCH_PACKET        BIT(4)

#define RX_STATUS_HEADER_OFFSET_MASK  0xC0
#define RX_STATUS_HEADER_OFFSET_OFFSET  6


/*------------------------------------------------------------------------------
 * Bit fields for HW_RXV_DESC_T
 *------------------------------------------------------------------------------
*/
#define RXV_DES_RXV_CNT_MASK        BITS(0, 4)


/*------------------------------------------------------------------------------
 * Bit fields for HW_RX_VECTOR_DESC_T
 *------------------------------------------------------------------------------
*/
/* DW 1 */
#define RX_VECTOR_SN_MACK           BITS(0, 7)
#define RX_VECTOR_FOR_BA_ACK        BIT(8)


/*! HIF RX DMA Descriptor */
/* DW 2 */
#define HIF_RX_DESC_BUFFER_LEN                  BITS(0, 15)
#define HIF_RX_DESC_ETHER_TYPE_OFFSET_MASK      BITS(16,23)
#define HIF_RX_DESC_ETHER_TYPE_OFFSET_OFFSET    16
#define HIF_RX_DESC_IP_CHKSUM_CHECK             BIT(24)
#define HIF_RX_DESC_TCP_UDP_CHKSUM_CHECK        BIT(25)

#define HIF_RX_DATA_QUEUE       0
#define HIF_RX_EVENT_QUEUE      1


/*------------------------------------------------------------------------------
 * Bit fields for HW_TX_DESC_T
 *------------------------------------------------------------------------------
*/

/*! MAC TX Descriptor */
#define TX_DESC_LENGTH                          DWORD_TO_BYTE(7)

/*------------------------------------------------------------------------*/
/* Tx descriptor field related information                                                                 */
/*------------------------------------------------------------------------*/
/* DW 0 */
#define TX_DESC_TX_BYTE_COUNT_MASK              BITS(0, 15)
#define TX_DESC_TX_BYTE_COUNT_OFFSET            0

#define TX_DESC_ETHER_TYPE_OFFSET_MASK          BITS(0, 6)
#define TX_DESC_ETHER_TYPE_OFFSET_OFFSET        0
#define TX_DESC_IP_CHKSUM_OFFLOAD               BIT(7)

#define TX_DESC_TCP_UDP_CHKSUM_OFFLOAD          BIT(0)
#define TX_DESC_USB_NEXT_VLD                    BIT(1)
#define TX_DESC_USB_TX_BURST                    BIT(2)
#define TX_DESC_QUEUE_INDEX_MASK                BITS(3, 6)
#define TX_DESC_QUEUE_INDEX_OFFSET              3
#define TX_DESC_PORT_INDEX                      BIT(7)
#define TX_DESC_PORT_INDEX_OFFSET               7

#define PORT_INDEX_LMAC                         0
#define PORT_INDEX_MCU                          1


/* DW 1 */
#define TX_DESC_WLAN_INDEX_MASK                 BITS(0, 7)
#define TX_DESC_WLAN_INDEX_OFFSET               0

#define TX_DESC_HEADER_FORMAT_MASK              BITS(5, 6)
#define TX_DESC_HEADER_FORMAT_OFFSET            5

#define HEADER_FORMAT_NON_802_11                0   /* Non-802.11 */
#define HEADER_FORMAT_COMMAND                   1   /* Command */
#define HEADER_FORMAT_802_11_NORMAL_MODE        2   /* 802.11 (normal mode) */
#define HEADER_FORMAT_802_11_ENHANCE_MODE       3   /* 802.11 (Enhancement mode)*/
#define HEADER_FORMAT_802_11_MASK               BIT(1)

#define TX_DESC_NON_802_11_MORE_DATA            BIT(0)
#define TX_DESC_NON_802_11_EOSP                 BIT(1)
#define TX_DESC_NON_802_11_REMOVE_VLAN          BIT(2)
#define TX_DESC_NON_802_11_VLAN_FIELD           BIT(3)
#define TX_DESC_NON_802_11_ETHERNET_II          BIT(4)
#define TX_DESC_NOR_802_11_HEADER_LENGTH_MASK   BITS(0, 4)
#define TX_DESC_NOR_802_11_HEADER_LENGTH_OFFSET 0
#define TX_DESC_ENH_802_11_EOSP                 BIT(1)
#define TX_DESC_ENH_802_11_AMSDU                BIT(2)

#define TX_DESC_FORMAT                          BIT(7)
#define TX_DESC_SHORT_FORMAT                    0
#define TX_DESC_LONG_FORMAT                     1
#define TX_DESC_HEADER_PADDING_LENGTH_MASK      BITS(0, 1)
#define TX_DESC_HEADER_PADDING_LENGTH_OFFSET    0
#define TX_DESC_HEADER_PADDING_MODE             BIT(2)
#define TX_DESC_NO_ACK                          BIT(3)
#define TX_DESC_TID_MASK                        BITS(4, 6)
#define TX_DESC_TID_OFFSET                      4
#define TX_DESC_TID_NUM                         8
#define TX_DESC_PROTECTED_FRAME                 BIT(7)

#define TX_DESC_PACKET_TYPE_MASK                BITS(0, 1)  /* SW Field */
#define TX_DESC_PACKET_TYPE_OFFSET              0
#define TX_DESC_OWN_MAC_MASK                    BITS(2, 7)
#define TX_DESC_OWN_MAC_OFFSET                  2

/* DW 2 */
#define TX_DESC_SUB_TYPE_MASK                   BITS(0, 3)
#define TX_DESC_SUB_TYPE_OFFSET                 0
#define TX_DESC_TYPE_MASK                       BITS(4, 5)
#define TX_DESC_TYPE_OFFSET                     4
#define TX_DESC_NDP                             BIT(6)
#define TX_DESC_NDPA                            BIT(7)

#define TX_DESC_SOUNDING                        BIT(0)
#define TX_DESC_FORCE_RTS_CTS                   BIT(1)
#define TX_DESC_BROADCAST_MULTICAST             BIT(2)
#define TX_DESC_BIP_PROTECTED                   BIT(3)
#define TX_DESC_DURATION_FIELD_CONTROL          BIT(4)
#define TX_DESC_HTC_EXISTS                      BIT(5)
#define TX_DESC_FRAGMENT_MASK                   BITS(6, 7)
#define TX_DESC_FRAGMENT_OFFSET                 6
#define FRAGMENT_FISRT_PACKET                   1
#define FRAGMENT_MIDDLE_PACKET                  2
#define FRAGMENT_LAST_PACKET                    3

#define TX_DESC_REMAINING_MAX_TX_TIME           BITS(0, 7)
#define TX_DESC_POWER_OFFSET_MASK               BITS(0, 4)
#define TX_DESC_BA_DISABLE                      BIT(5)
#define TX_DESC_TIMING_MEASUREMENT              BIT(6)
#define TX_DESC_FIXED_RATE                      BIT(7)

/* DW 3 */
#define TX_DESC_TX_COUNT_MASK                   BITS(6, 10)
#define TX_DESC_TX_COUNT_OFFSET                 6
#define TX_DESC_TX_COUNT_NO_ATTEMPT             0
#define TX_DESC_TX_COUNT_NO_LIMIT               31
#define TX_DESC_REMAINING_TX_COUNT_MASK         BITS(11, 15)
#define TX_DESC_REMAINING_TX_COUNT_OFFSET       11
#define TX_DESC_SEQUENCE_NUMBER                 BITS(0, 11)
#define TX_DESC_HW_RESERVED_MASK                BITS(12,13)
#define TX_DESC_HW_RESERVED_OFFSET              12
#define TX_DESC_PN_IS_VALID                     BIT(14)
#define TX_DESC_SN_IS_VALID                     BIT(15)

/* DW 4 */
#define TX_DESC_PN_PART1                        BITS(0, 31)

/* DW 5 */
#define TX_DESC_PACKET_ID                       BIT(0, 7)
#define TX_DESC_TX_STATUS_FORMAT                BIT(0)
#define TX_DESC_TX_STATUS_FORMAT_OFFSET         0
#define TX_DESC_TX_STATUS_TO_MCU                BIT(1)
#define TX_DESC_TX_STATUS_TO_HOST               BIT(2)
#define TX_DESC_DA_SOURCE                       BIT(3)
#define TX_DESC_BAR_SSN_CONTROL                 BIT(4)
#define TX_DESC_POWER_MANAGEMENT_CONTROL        BIT(5)
#define TX_DESC_PN_PART2                        BITS(0, 15)

/* DW 6 */ /* FR = 1 */
#define TX_DESC_FIXED_RATE_MODE                 BIT(0)
#define TX_DESC_ANTENNA_INDEX_MASK              BITS(2, 7)
#define TX_DESC_ANTENNA_INDEX_OFFSET            2

#define TX_DESC_BANDWIDTH_MASK                  BITS(0, 2)
#define TX_DESC_SPATIAL_EXTENSION               BIT(3)
#define TX_DESC_ANTENNA_PRIORITY                BITS(4, 6)
#define TX_DESC_DYNAMIC_BANDWIDTH               BIT(7)

#define TX_DESC_EXPLICIT_BEAMFORMING            BIT(0)
#define TX_DESC_IMPLICIT_BEAMFORMING            BIT(1)
#define TX_DESC_FIXDE_RATE_MASK                 BITS(2, 13)
#define TX_DESC_FIXDE_RATE_OFFSET               2
#define TX_DESC_LDPC                            BIT(14)
#define TX_DESC_GUARD_INTERVAL                  BIT(15)

/*! MAC TX Status */
/* Header */
#define TX_STATUS_HEADER_PKT_TYPE               BITS(5, 7)
#define TX_STATUS_HEADER_PKT_TYPE_OFFSET        5
#define TX_STATUS_HEADER_TXS_COUNT              BITS(0, 4)
#define TX_STATUS_HEADER_TXS_COUNT_OFFSET       0
#define TX_STATUS_HEADER_TOTAL_BYTE_COUNT       BITS(0, 15)

/* DW 0 */
#define TX_STATUS_TX_RATE                       BITS(0, 11)
#define TX_STATUS_FIXED_RATE                    BIT(12)
#define TX_STATUS_TX_STATUS_FORMAT BIT(13)
#define TX_STATUS_TX_STATUS_TO_MCU BIT(14)
#define TX_STATUS_TX_STATUS_TO_HOST BIT(15)
#define TX_STATUS_MPDU_ERROR BIT(0)
#define TX_STATUS_RTS_ERROR BIT(1)
#define TX_STATUS_LIFETIME_ERROR BIT(2)
#define TX_STATUS_BIP_ERROR BIT(3)
#define TX_STATUS_TXOP_EXCEED_WARNING BIT(4)
#define TX_STATUS_PS_FIELD BIT(5)
#define TX_STATUS_BA_UNAVAILABLE BIT(6)
#define TX_STATUS_ERROR_BITS                    BITS(0,4)
#define TX_STATUS_TID_MASK                      BITS(7, 9)
#define TX_STATUS_TID_MASK_OFFSET 7
#define TX_STATUS_ANTENNA_INDEX_MASK BITS(10, 15)
#define TX_STATUS_ANTENNA_INDEX_OFFSET 10

/* DW 1 */
#define TX_STATUS_NOISE0 BITS(0, 7)
#define TX_STATUS_NOISE0_OFFSET 0
#define TX_STATUS_NOISE1 BITS(8, 15)
#define TX_STATUS_NOISE1_OFFSET 8
#define TX_STATUS_NOISE2 BITS(16, 23)
#define TX_STATUS_NOISE2_OFFSET 16

/* DW 2 */
#define TX_STATUS_PKT_FRONT_TIME_PART BITS(0,24)
#define TX_STATUS_TX_POWER_DBM                  BITS(25, 31)
#define TX_STATUS_TX_POWER_DBM_OFFSET 25
#define TX_STATUS_RCPI0  BITS(0,7)
#define TX_STATUS_RCPI0_OFFSET 0
#define TX_STATUS_RCPI1  BITS(8,15)
#define TX_STATUS_RCPI1_OFFSET 8
#define TX_STATUS_RCPI2  BITS(16,23)
#define TX_STATUS_RCPI2_OFFSET 16

/* DW 3 */

/* DW4 */
#define TX_STATUS_SN                          BITS(0, 11)
#define TX_STATUS_SN_OFFSET 0
#define TX_STATUS_TSSI                          BITS(0, 11)
#define TX_STATUS_TSSI_OFFSET 0
#define TX_STATUS_TX_BW                         BITS(12, 13)
#define TX_STATUS_TX_BW_OFFSET  12
#define TX_STATUS_PID BITS(14,21)
#define TX_STATUS_PID_OFFSET 14
#define TX_STATUS_FM                            BIT(22)
#define TX_STATUS_FM_OFFSET 22
#define TX_STATUS_AM   BIT(23)
#define TX_STATUS_AM_OFFSET 23
#define TX_STATUS_LAST_MPDU_CNT BITS(24,28)
#define TX_STATUS_LAST_MPDU_CNT_OFFSET 24
#define TX_STATUS_LAST_MCS_IDX BITS(29,31)
#define TX_STATUS_LAST_MCS_IDX_OFFSET 29


/*------------------------------------------------------------------------------
 * Bit fields for PHY Vector
 *------------------------------------------------------------------------------
*/

/* RX Vector, 1st Cycle */
#define RX_VT_RX_RATE_MASK         BITS(0,6)
#define RX_VT_RX_RATE_OFFSET       0
#define RX_VT_STBC_MASK            BITS(7,8)
#define RX_VT_STBC_OFFSET          7

#define RX_VT_LDPC                 BIT(9)
#define RX_VT_RX_MODE_MASK         BITS(12,14)
#define RX_VT_RX_MODE_OFFSET       12
#define RX_VT_RX_MODE_VHT          BIT(14)

#define RX_VT_FR_MODE_MASK         BITS(15,16)
#define RX_VT_FR_MODE_OFFSET       15
#define RX_VT_TXOP_PS_NOT_ALLOWED  BIT(17)
#define RX_VT_AGGREGATION          BIT(18)
#define RX_VT_SHORT_GI             BIT(19)
#define RX_VT_SMOOTH               BIT(20)
#define RX_VT_NO_SOUNDING          BIT(21)
#define RX_VT_SOUNDING             BIT(21)


#define RX_VT_RX_RATE_1M      0x0
#define RX_VT_RX_RATE_2M      0x1
#define RX_VT_RX_RATE_5M      0x2
#define RX_VT_RX_RATE_11M     0x3
#define RX_VT_RX_RATE_6M      0xB
#define RX_VT_RX_RATE_9M      0xF
#define RX_VT_RX_RATE_12M     0xA
#define RX_VT_RX_RATE_18M     0xE
#define RX_VT_RX_RATE_24M     0x9
#define RX_VT_RX_RATE_36M     0xD
#define RX_VT_RX_RATE_48M     0x8
#define RX_VT_RX_RATE_54M     0xC

#define RX_VT_RX_RATE_MCS0    0
#define RX_VT_RX_RATE_MCS1    1
#define RX_VT_RX_RATE_MCS2    2
#define RX_VT_RX_RATE_MCS3    3
#define RX_VT_RX_RATE_MCS4    4
#define RX_VT_RX_RATE_MCS5    5
#define RX_VT_RX_RATE_MCS6    6
#define RX_VT_RX_RATE_MCS7    7
#define RX_VT_RX_RATE_MCS8    8
#define RX_VT_RX_RATE_MCS9    9
#define RX_VT_RX_RATE_MCS32   32

#define RX_VT_LEGACY_CCK      0
#define RX_VT_LEGACY_OFDM     1
#define RX_VT_MIXED_MODE      2
#define RX_VT_GREEN_MODE      3
#define RX_VT_VHT_MODE        4

#if 0 /* 6630 */
#define RX_VT_DL40_HT40       1
#define RX_VT_U20             2
#define RX_VT_L20             3
#endif

#define RX_VT_FR_MODE_20      0
#define RX_VT_LG20_HT20       RX_VT_FR_MODE_20

#define RX_VT_FR_MODE_40      1
#define RX_VT_FR_MODE_80      2
#define RX_VT_FR_MODE_160     3


#define RX_VT_CCK_SHORT_PREAMBLE   BIT(2)


/* RX Vector, 2nd Cycle */
#define RX_VT_RX_LEN_HT_MASK       BITS(0,15)
#define RX_VT_RX_LEN_LEACY_MASK    BITS(0,11)
#define RX_VT_RX_LEN_VHT_MASK      BITS(0,20)

/* RX Vector, 3rd Cycle */
#define RX_VT_RCPI0_MASK       BITS(8,15)
#define RX_VT_RCPI0_OFFSET     8
#define RX_VT_OFDM_FREQ_TRANS_DET  BIT(5)
#define RX_VT_SEL_ANT              BIT(7)
#define RX_VT_FAGC0_EQ_CAL         BIT(16)
#define RX_VT_FAGC0_GAIN_OFFSET    17
#define RX_VT_FAGC0_CAL_GAIN       BITS(17,19)
#define RX_VT_RCPI1_MASK       BITS(20,27)
#define RX_VT_RCPI1_OFFSET     20


/* RX Vector, 4th Cycle */
#define RX_VT_IB_RSSI_MASK       BITS(0,7)
#define RX_VT_WB_RSSI_MASK       BITS(8,15)
#define RX_VT_WB_RSSI_OFFSET    8
#define RX_VT_RCPI2_MASK       BITS(20,27)
#define RX_VT_RCPI2_OFFSET     20

/*RX Vector, 6th Cycle*/
#define RX_VT_NF0_MASK      BITS(1,7)
#define RX_VT_NF0_OFFSET     1

#define RX_VT_NF1_MASK      BITS(9,15)
#define RX_VT_NF1_OFFSET     9


#define RX_VT_NF2_MASK      BITS(17,23)
#define RX_VT_NF2_OFFSET     17

#define RX_VT_NSTS_MASK     BITS(25,27)
#define RX_VT_NSTS_OFFSET   25


/* RX Vector Group 2, the 1st cycle */
#define RX_VT_CCK_LQ            BITS(4,13)
#define RX_VT_OFDM_LQ_BPSK      BITS(4,10)
#define RX_VT_LQ_OFFSET         4
#define RX_VT_PRIM_ITFR_ENV     BIT(0)
#define RX_VT_SEC_ITFR_ENV      BIT(1)
#define RX_VT_SEC40_ITFR_ENV    BIT(2)


/* RX Vector Group 2, the 2nd cycle */
#define RX_VT_DYNA_BW_IN_NON_HT_DYNA   BIT(29)
#define RX_VT_CH_BW_IN_NON_HT_CBW40    BIT(30)
#define RX_VT_CH_BW_IN_NON_HT_CBW80    BIT(31)
#define RX_VT_CH_BW_IN_NON_HT_CBW160   BITS(30,31)
#define RX_VT_CH_BW_IN_NON_HT_MASK     BITS(30,31)
#define RX_VT_CH_BW_IN_NON_HT_OFFSET   30



/* RX Data Type */
#define RX_DATA_TYPE_RX_VECTOR  0
#define RX_DATA_TYPE_RX_DATA    1



/* TX Vector, 1st Cycle */
#define TX_VT_TX_RATE_MASK BITS(0,6)
#define TX_VT_TX_RATE_OFFSET 0
#define TX_VT_HT_STBC_MASK BITS(7,8)
#define TX_VT_HT_STBC_OFFSET 7
#define TX_VT_HT_ADCODE BIT(9)
#define TX_VT_HT_EXTLTF_MASK BITS(10,11)
#define TX_VT_HT_EXTLTF_OFFSET 10
#define TX_VT_TX_MODE_MASK BITS(12,14)
#define TX_VT_TX_MODE_OFFSET 12
#define TX_VT_FREQ_MODE_MASK BITS(15,16)
#define TX_VT_FREQ_MODE_OFFSET 15
#define TX_VT_TX_CHAIN BIT(18, 21)
#define TX_VT_TX_POWERDBM_MASK BITS(22,28)
#define TX_VT_TX_POWERDBM_OFFSET 22

#define TX_VT_TX_MODE_LG_CCK 0x00
#define TX_VT_TX_MODE_LG_OFDM 0x1
#define TX_VT_TX_MODE_HT_MM 0x2
#define TX_VT_TX_MODE_HT_GF 0x3
#define TX_VT_TX_MODE_VHT 0x4

#if 0 /* 6630 */
#define TX_VT_FREQ_MODE_LG_HT_20 0x0
#define TX_VT_FREQ_MODE_DL_HT_40 0x1
#define TX_VT_FREQ_MODE_U20 0x2
#define TX_VT_FREQ_MODE_L20 0x3
#endif

#define TX_VT_FREQ_MODE_20      0x0
#define TX_VT_FREQ_MODE_40      0x1
#define TX_VT_FREQ_MODE_80      0x2
#define TX_VT_FREQ_MODE_160     0x3

#define TX_VT_TX_RATE_1M      0x0
#define TX_VT_TX_RATE_2M      0x1
#define TX_VT_TX_RATE_5_5M      0x2
#define TX_VT_TX_RATE_11M     0x3
#define TX_VT_TX_RATE_6M      0xB
#define TX_VT_TX_RATE_9M      0xF
#define TX_VT_TX_RATE_12M     0xA
#define TX_VT_TX_RATE_18M     0xE
#define TX_VT_TX_RATE_24M     0x9
#define TX_VT_TX_RATE_36M     0xD
#define TX_VT_TX_RATE_48M     0x8
#define TX_VT_TX_RATE_54M     0xC

#define TX_VT_TX_RATE_MCS0    0
#define TX_VT_TX_RATE_MCS1    1
#define TX_VT_TX_RATE_MCS2    2
#define TX_VT_TX_RATE_MCS3    3
#define TX_VT_TX_RATE_MCS4    4
#define TX_VT_TX_RATE_MCS5    5
#define TX_VT_TX_RATE_MCS6    6
#define TX_VT_TX_RATE_MCS7    7
#define TX_VT_TX_RATE_MCS8    8
#define TX_VT_TX_RATE_MCS9    79
#define TX_VT_TX_RATE_MCS32   32

#define TX_VT_HT_HTLTF_NO_EXT_SS 0x0
#define TX_VT_HT_HTLTF_1_EXT_SS 0x1
#define TX_VT_HT_HTLTF_2_EXT_SS 0x2
#define TX_VT_HT_HTLTF_3_EXT_SS 0x3

/* TX Vector, 2nd Cycle */
#define TX_VT_HT_AGGREGATION BIT(0)
#define TX_VT_HT_SHORT_GI BIT(1)
#define TX_VT_HT_SMOOTH BIT(2)
#define TX_VT_HT_SOUND BIT(3)
#define TX_VT_HT_BF_IM  BIT(4)
#define TX_VT_HT_BF_EM  BIT(5)
#define TX_VT_TX_BF                 BITS(4,10)
#define TX_VT_TX_BF_OFFSET 4
#define TX_VT_TX_BF_SPE_EN  BIT(6)

#define TX_VT_CH_BW_NON_HT_SET BIT(11)
#define TX_VT_VHT_GROUP_ID_OFFSET 12
#define TX_VT_VHT_GROUP_ID_MASK BITS(12,17)
#define TX_VT_VHT_SU_NSTS_OFFSET 18
#define TX_VT_VHT_SU_NSTS_MASK BITS(18,20)
#define TX_VT_VHT_PARTIAL_AID_OFFSET 21
#define TX_VT_VHT_PARTIAL_AID_MASK BITS(21,29)
#define TX_VT_VHT_IS_TXOP_PS_NOT_ALLOWED BIT(30)

/* TX Vector, 3rd Cycle */
#define TX_VT_DYN_BW_NON_HT_SET BIT(9)
#define TX_VT_DYN_BW_IN_NON_HT BIT(10)

/* TX Vector, 4th Cycle */
#define TX_VT_BF_MAC_ADDR BIT(0,31)

/* TX Vector, 5th Cycle */
#define TX_VT_LG_TX_LEN_MASK BITS(0,11)
#define TX_VT_LG_TX_LEN_OFFSET 0
#define TX_VT_VHT_PSDU_LEN_MASK BITS(12,31)
#define TX_VT_VHT_PSDU_LEN_OFFSET 12

/* TX Vector, 6th Cycle */
#define TX_VT_VHT_APEP_LEN_MASK     BITS(0,19)
#define TX_VT_VHT_APEP_LEN_OFFSET 0
#define TX_VT_HT_TX_LEN_MASK BITS(0,15)
#define TX_VT_HT_TX_LEN_OFFSET 0


//TBD
#define ADDR_ILM_START  0x00060000
#define SIZE_ILM_PER_SUB_BANK  (16 * 1024)  //16KB
#define SIZE_ILM_TOTAL  (148 * 1024)  //148KB

#define ADDR_DLM_START  0xF0098000
#define SIZE_DLM_PER_SUB_BANK  (16 * 1024)  //16KB
#define SIZE_DLM_TOTAL  (64 * 1024)  //64KB

#define ADDR_SRAM_START  0x00200000
#define SIZE_SRAM_PER_SUB_BANK  (32 * 1024)  //32KB
#define SIZE_SRAM_TOTAL  (90 * 1024)  //90KB
/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/* Number of Interrupt status registers */
// mapping with g_au4IntStatusRegIndexToOffset, defined in wifi_var.c
typedef enum _ENUM_INT_T {
    INT_HWFISR,     //under MCUSYS
    INT_WIS0R,      //under WIFISYS
    INT_WIS1R,      //under WIFISYS
    INT_NUM
} ENUM_INT_T, *P_ENUM_INT_T;

#if 0
typedef enum _ENUM_RATE_INDEX_T {
    RATE_1M_INDEX = 0,      /* 1M */
    RATE_2M_INDEX,          /* 2M */
    RATE_5_5M_INDEX,        /* 5.5M */
    RATE_11M_INDEX,         /* 11M */
    RATE_22M_INDEX,         /* 22M */
    RATE_33M_INDEX,         /* 33M */
    RATE_6M_INDEX,          /* 6M */
    RATE_9M_INDEX,          /* 9M */
    RATE_12M_INDEX,         /* 12M */
    RATE_18M_INDEX,         /* 18M */
    RATE_24M_INDEX,         /* 24M */
    RATE_36M_INDEX,         /* 36M */
    RATE_48M_INDEX,         /* 48M */
    RATE_54M_INDEX,         /* 54M */
    RATE_HT_PHY_INDEX,      /* BSS Selector - HT PHY */
    RATE_NUM // 15
} ENUM_RATE_INDEX_T, *P_ENUM_RATE_INDEX_T;
#endif

#define RATE_NUM        15

typedef enum _ENUM_CCK_RATE_INDEX_T {
    RATE_1M_INDEX = 0,      /* 1M */
    RATE_2M_INDEX,          /* 2M */
    RATE_5_5M_INDEX,        /* 5.5M */
    RATE_11M_INDEX,         /* 11M */
    CCK_RATE_NUM // 4
} ENUM_CCK_RATE_INDEX_T, *P_ENUM_CCK_RATE_INDEX_T;



typedef enum _ENUM_OFDM_RATE_INDEX_T {
    RATE_6M_INDEX = 0,      /* 6M */
    RATE_9M_INDEX,          /* 9M */
    RATE_12M_INDEX,         /* 12M */
    RATE_18M_INDEX,         /* 18M */
    RATE_24M_INDEX,         /* 24M */
    RATE_36M_INDEX,         /* 36M */
    RATE_48M_INDEX,         /* 48M */
    RATE_54M_INDEX,         /* 54M */
    OFDM_RATE_NUM // 8
} ENUM_OFDM_RATE_INDEX_T, *P_ENUM_OFDM_RATE_INDEX_T;

typedef enum _ENUM_HT_RATE_INDEX_T {
    HT_RATE_MCS32_INDEX = 0,
    HT_RATE_MCS0_INDEX,
    HT_RATE_MCS1_INDEX,
    HT_RATE_MCS2_INDEX,
    HT_RATE_MCS3_INDEX,
    HT_RATE_MCS4_INDEX,
    HT_RATE_MCS5_INDEX,
    HT_RATE_MCS6_INDEX,
    HT_RATE_MCS7_INDEX,
    HT_RATE_NUM // 9
} ENUM_HT_RATE_INDEX_T, *P_ENUM_HT_RATE_INDEX_T;

typedef enum _ENUM_VHT_RATE_INDEX_T {
    VHT_RATE_MCS0_INDEX = 0,
    VHT_RATE_MCS1_INDEX,
    VHT_RATE_MCS2_INDEX,
    VHT_RATE_MCS3_INDEX,
    VHT_RATE_MCS4_INDEX,
    VHT_RATE_MCS5_INDEX,
    VHT_RATE_MCS6_INDEX,
    VHT_RATE_MCS7_INDEX,
    VHT_RATE_MCS8_INDEX,
    VHT_RATE_MCS9_INDEX,
    VHT_RATE_NUM // 10
} ENUM_VHT_RATE_INDEX_T, *P_ENUM_VHT_RATE_INDEX_T;


/* Used for HAL */
typedef enum _ENUM_PREMABLE_OPTION_T {
    PREAMBLE_DEFAULT_LONG_NONE = 0, /* LONG for PHY_TYPE_HR_DSSS, NONE for PHY_TYPE_OFDM */
    PREAMBLE_OPTION_SHORT, /* SHORT mandatory for PHY_TYPE_ERP, SHORT option for PHY_TYPE_HR_DSSS */
    PREAMBLE_OFDM_MODE,
    PREAMBLE_HT_MIXED_MODE,
    PREAMBLE_HT_GREEN_FIELD,
    PREAMBLE_VHT_FIELD,
    PREAMBLE_OPTION_NUM
} ENUM_PREMABLE_OPTION_T, *P_ENUM_PREMABLE_OPTION_T;


/* Used for HAL */
typedef enum _ENUM_MODULATION_SYSTEM_T {
    MODULATION_SYSTEM_CCK = 0,
    MODULATION_SYSTEM_OFDM,
    MODULATION_SYSTEM_HT20,
    MODULATION_SYSTEM_HT40,
    MODULATION_SYSTEM_NUM
} ENUM_MODULATION_SYSTEM_T, *P_ENUM_MODULATION_SYSTEM_T;


/* Used for HAL */
typedef enum _ENUM_MODULATION_TYPE_T {
    MODULATION_TYPE_CCK_BPSK = 0, // including 5.5M, 11M
    MODULATION_TYPE_DSSS_QPSK,    // including 1M, 2M
    MODULATION_TYPE_16QAM,
    MODULATION_TYPE_64QAM,
    MODULATION_TYPE_MCS6,
    MODULATION_TYPE_54M_MCS7,
    MODULATION_TYPE_MCS32,
    MODULATION_TYPE_NUM
} ENUM_MODULATION_TYPE_T, *P_ENUM_MODULATION_TYPE_T;

/* for TX power setting */
typedef enum _ENUM_VHT_MODULATION_TYPE_T {
    MODULATION_TYPE_VHT_BPSK = 0,
    MODULATION_TYPE_VHT_QPSK,
    MODULATION_TYPE_VHT_16QAM,
    MODULATION_TYPE_VHT_64QAM_MSC5_6,
    MODULATION_TYPE_VHT_64QAM_MSC7,
    MODULATION_TYPE_VHT_64QAM_MSC8,
    MODULATION_TYPE_VHT_64QAM_MSC9,
    MODULATION_TYPE_VHT_NUM
} ENUM_VHT_MODULATION_TYPE_T, *P_ENUM_VHT_MODULATION_TYPE_T;

typedef enum _ENUM_BTIM_IDX_T {
    BTIM_0_0 = 0,
    BTIM_1,
    BTIM_2,
    BTIM_3,
    BTIM_0_1 = 17,
    BTIM_0_2,
    BTIM_0_3,
    BTIM_0_4,
    BTIM_0_5,
    BTIM_0_6,
    BTIM_0_7,
    BTIM_0_8,
    BTIM_0_9,
    BTIM_0_10,
    BTIM_0_11,
    BTIM_0_12,
    BTIM_0_13,
    BTIM_0_14,
    BTIM_0_15
} ENUM_BTIM_IDX_T, *P_ENUM_BTIM_IDX_T;

/* PS_FORWARDING_TYPE_NON_PS means that the receiving STA is in Active Mode
*   from the perspective of host driver (maybe not synchronized with FW --> SN is needed)
*/
typedef enum _ENUM_PS_FORWARDING_TYPE_T {
    PS_FORWARDING_TYPE_NON_PS = 0,
    PS_FORWARDING_TYPE_DELIVERY_ENABLED,
    PS_FORWARDING_TYPE_NON_DELIVERY_ENABLED,
    PS_FORWARDING_MORE_DATA_ENABLED,
    PS_FORWARDING_TYPE_NUM
} ENUM_PS_FORWARDING_TYPE_T, *P_ENUM_PS_FORWARDING_TYPE_T;


/* Used for HAL */
typedef enum _ENUM_SEC_CH_IDLE_SEL_T {
    SEC_CH_IDLE_SEL_SIFS = 0,
    SEC_CH_IDLE_SEL_PIFS,    // including 1M, 2M
    SEC_CH_IDLE_SEL_DIFS,
    SEC_CH_IDLE_SEL_SIFS_PLUS_3_SLOT,
    SEC_CH_IDLE_SEL_NUM
} ENUM_SEC_CH_IDLE_SEL_T, *P_ENUM_SEC_CH_IDLE_SEL_T;


/* Used for MAC RX */
typedef enum _ENUM_MAC_RX_PKT_TYPE_T {
    RX_PKT_TYPE_TX_STATUS = 0,
    RX_PKT_TYPE_RX_VECTOR,
    RX_PKT_TYPE_RX_DATA,
    RX_PKT_TYPE_DUP_RFB,
    RX_PKT_TYPE_TM_REPORT,
    RX_PKT_TYPE_SW_DEFINED = 7
} ENUM_MAC_RX_PKT_TYPE_T;


typedef enum _ENUM_TX_HEADER_FORMAT_T {
    HF_NON_802_11 = 0,
    HF_CMD_FRAME,
    HF_802_11,
    HF_ENHANCE_802_11,
    HF_NUM
} ENUM_TX_HEADER_FORMAT_T;


typedef enum _ENUM_TX_FRAG_FORMAT_T {
    FRAG_NO_FRAG = 0,
    FRAG_FIRST_FRAG,
    FRAG_MIDDLE_FRAG,
    FRAG_LAST_FRAG,
    FRAG_NUM
} ENUM_TX_FRAG_FORMAT_T;

typedef enum _ENUM_FIX_BW_T {
    FIX_BW_NO_FIXED = 0,
    FIX_BW_20 = 4,
    FIX_BW_40,
    FIX_BW_80,
    FIX_BW_160,
    FIX_BW_NUM
} ENUM_FIX_BW_T;

typedef enum _ENUM_TX_STS_FORMAT_T {
    TXSFM_TIMESTAMP_FRONT_TIME_MODE = 0,
    TXSFM_NOISY_RCPI_MODE,
    TXSFM_NUM
} ENUM_TX_STS_FORMAT_T;

typedef enum _ENUM_AVG_PARAM_T {
    AVERAGE_8 = 0,
    AVERAGE_4,
    AVERAGE_2,
    AVERAGE_1,
    AVERAGE_NUM
}ENUM_AVG_PARAM_T;
/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/
/*! A data structure which is identical with MAC RX DMA Descriptor */
typedef struct _HW_MAC_RX_DESC_T {
    UINT_16    u2RxByteCount;       /* DW 0 */
    UINT_16    u2PktTYpe;
    UINT_8     ucMatchPacket;       /* DW 1 */
    UINT_8     ucChanFreq;
    UINT_8     ucHeaderLen;
    UINT_8     ucBssid;
    UINT_8     ucWlanIdx;           /* DW 2 */
    UINT_8     ucTidSecMode;
    UINT_16    u2StatusFlag;
    UINT_32    u4PatternFilterInfo; /* DW 3 */
}  HW_MAC_RX_DESC_T, *P_HW_MAC_RX_DESC_T;

typedef struct _HW_MAC_RX_STS_GROUP_1_T {
    UINT_8     aucPN[16];
}HW_MAC_RX_STS_GROUP_1_T, *P_HW_MAC_RX_STS_GROUP_1_T;


typedef struct _HW_MAC_RX_STS_GROUP_2_T {
    UINT_32    u4Timestamp;      /* DW 12 */
    UINT_32    u4CRC;            /* DW 13 */
}HW_MAC_RX_STS_GROUP_2_T, *P_HW_MAC_RX_STS_GROUP_2_T;


typedef struct _HW_MAC_RX_STS_GROUP_4_T {
    /* For HDR_TRAN */
    UINT_16    u2FrameCtl;          /* DW 4 */
    UINT_8     aucTA[6];            /* DW 4~5 */
    UINT_16    u2SeqFrag;           /* DW 6 */
    UINT_16    u2Qos;               /* DW 6 */
    UINT_32    u4HTC;               /* DW 7 */
}HW_MAC_RX_STS_GROUP_4_T, *P_HW_MAC_RX_STS_GROUP_4_T;


typedef struct _HW_MAC_RX_STS_GROUP_3_T {
    /*!  RX Vector Info*/
    UINT_32    u4RxVector[6];       /* DW 14~19 */
}HW_MAC_RX_STS_GROUP_3_T, *P_HW_MAC_RX_STS_GROUP_3_T;

typedef struct _HW_MAC_RX_TMRI_PKT_FORMAT_T {
    UINT_8     ucPID;
    UINT_8     ucStatus;
    UINT_16    u2PktTYpe;
    UINT_32    u4Reserved[2];
    UINT_32    u4ToA;
    UINT_32    u4ToD;
}HW_MAC_RX_TMRI_PKT_FORMAT_T, *P_HW_MAC_RX_TMRI_PKT_FORMAT_T;

typedef struct _HW_MAC_RX_TMRR_PKT_FORMAT_T {
    UINT_8     ucVtSeq;
    UINT_8     ucStatus;
    UINT_16    u2PktTYpe;
    UINT_8     aucTALow[2];
    UINT_16    u2SnField;
    UINT_8     aucTAHigh[4];
    UINT_32    u4ToA;
    UINT_32    u4ToD;
}HW_MAC_RX_TMRR_PKT_FORMAT_T, *P_HW_MAC_RX_TMRR_PKT_FORMAT_T;


/*!A data structure which is identical with HW MAC TX DMA Descriptor */
typedef struct _HW_MAC_TX_DESC_T {
    /* DW 0 */
    UINT_16     u2TxByteCount;
    UINT_8      ucEtherOffset;          /* Ether-Type Offset,  IP checksum offload*/
    UINT_8      ucPortIdx_QueueIdx;     /* UDP/TCP checksum offload,  USB NextVLD/TxBURST, Queue index, Port index*/
    /* DW 1 */
    UINT_8      ucWlanIdx;
    UINT_8      ucHeaderFormat;         /* Header format, TX descriptor format */
    UINT_8      ucHeaderPadding;        /* Header padding, no ACK, TID, Protect frame */
    UINT_8      ucOwnMAC;
    /* DW 2 */
    UINT_8      ucType_SubType;         /* Type, Sub-type, NDP, NDPA */
    UINT_8      ucFrag;                 /* Sounding, force RTS/CTS, BMC, BIP, Duration, HTC exist, Fragment */
    UINT_8      ucRemainingMaxTxTime;
    UINT_8      ucPowerOffset;          /* Power offset, Disable BA, Timing measurement, Fixed rate*/
    /* DW 3 */
    UINT_16     u2TxCountLimit;         /* TX count limit, Remaining TX Count */
    UINT_16     u2SN;                   /* SN, HW own, PN valid, SN valid */
    /* DW 4 */
    UINT_32     u4PN1;
    /* DW 5 */
    UINT_8      ucPID;
    UINT_8      ucTxStatus;             /* TXS format, TXS to mcu, TXS to host, DA source, BAR SSN, Power management*/
    UINT_16     u2PN2;
    /* DW 6 */
    UINT_8      ucAntID;                /* Fixed rate, Antenna ID */
    UINT_8      ucBandwidth;            /* Bandwidth,  Spatial Extension, Antenna priority, Dynamic bandwidth */
    UINT_16     u2FixedRate;            /* Explicit/implicit beamforming, Fixed rate table, LDPC, GI */
} HW_MAC_TX_DESC_T, *P_HW_MAC_TX_DESC_T, **PP_HW_MAC_TX_DESC_T;


/*!A data structure which is identical with HW MAC TX STATUS */
typedef struct _HW_MAC_TX_STATUS_T {
    UINT_16 u2TxRate;       /* DW 0 */
    UINT_16 u2ErrStatus;
    UINT_32 u4TimeStamp;    /* DW 1 */
    UINT_32 u4PktFrontTime; /* DW 2 */
    UINT_16 u2TxDelay; /* DW 3 */
    UINT_8  ucTxRXVSeqNum;
    UINT_8  ucWLANIdex;
    UINT_32 u4Tssi;         /* DW 4 */
} HW_MAC_TX_STATUS_T, *P_HW_MAC_TX_STATUS_T, **PP_HW_MAC_TX_STATUS_T;


typedef struct _HW_MAC_AGG_TX_STATUS_T {
    UINT_16 u2RxByteCnt;
    UINT_8 ucTxStsCnt;
    UINT_8 ucPktType;
    HW_MAC_TX_STATUS_T aurHwTxStstus[1];
} HW_MAC_AGG_TX_STATUS_T, *P_HW_MAC_AGG_TX_STATUS_T;



/*! A data structure which is identical with MAC RX Vector DMA Descriptor */
typedef struct _HW_RXV_DESC_T {
    UINT_16    u2RxByteCount;       /* DW 0 */
    UINT_16    u2PktTYpe;
}  HW_RXV_DESC_T, *P_HW_RXV_DESC_T;

typedef struct _HW_RX_VECTOR_T {
    UINT_8     aucTA[6];        /* DW 0~1 */
    UINT_16    u2RxVtSeqNo;
    /*!  RX Vector Info*/
    UINT_32    u4RxVector[9];     /* DW 2~10 */

}  HW_RX_VECTOR_T, *P_HW_RX_VECTOR_T;


/*! A data structure which is identical with HIF RX DMA Descriptor */
#if (PRODUCT_VERSION == 6630 || PRODUCT_VERSION == 7636 || PRODUCT_VERSION == 7637)
typedef struct _HW_HIF_RX_DESC_T {
    UINT_8     ucOwn;
    UINT_8     ucDescChksum;
    UINT_8     ucEtherTypeOffset;
    UINT_8     ucChkSumInfo;
    UINT_32    u4NextDesc;
    UINT_32    u4BufStartAddr;
    UINT_16    u2RxBufLen;
    UINT_16    u2Rsrv1;
} HW_HIF_RX_DESC_T, *P_HW_HIF_RX_DESC_T;
#else
typedef struct _HW_HIF_RX_DESC_T {
    UINT_8     ucOwn;
    UINT_8     ucDescChksum;
    UINT_16    u2Rsrv1;
    UINT_32    u4NextDesc;
    UINT_32    u4BufStartAddr;
    UINT_16    u2RxBufLen;
    UINT_8     ucEtherTypeOffset;
    UINT_8     ucChkSumInfo;
} HW_HIF_RX_DESC_T, *P_HW_HIF_RX_DESC_T;
#endif

#if (PRODUCT_VERSION == 6630 || PRODUCT_VERSION == 7636 || PRODUCT_VERSION == 7637)
typedef struct _HW_SEC_TKIP_CCMP_TCS_T {
    UINT_32     u4TxTcs_31_0;
    UINT_16     u2TxTcs_47_32;
} HW_SEC_TKIP_CCMP_TCS_T, *P_HW_SEC_TKIP_CCMP_TCS_T;

typedef struct _HW_SEC_WPI_PN_T {
    UINT_32     u4TxPn_31_0;
    UINT_32     u4TxPn_63_32;
    UINT_32     u4TxPn_95_64;
    UINT_32     u4TxPn_127_96;
} HW_SEC_WPI_PN_T, *P_HW_SEC_WPI_PN_T;
#endif


/*! A data structure which is identical with HIF TX DMA Descriptor */
typedef struct _HW_HIF_TX_DESC_T {
    UINT_8     ucOwn;
    UINT_8     ucDescChksum;
    UINT_16    u2Rsrv1;
    UINT_32    u4NextDesc;
    UINT_32    u4BufStartAddr;
    UINT_32    u4Rsrv2;
} HW_HIF_TX_DESC_T, *P_HW_HIF_TX_DESC_T;


typedef struct _DMA_DESC_COMM_T {
    QUE_ENTRY_T     rQueEntry;
    UINT_32         u4BufStartAddr;
} DMA_DESC_COMM_T, *P_DMA_DESC_COMM_T;
/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/
/*------------------------------------------------------------------------------
 * MACRO for HW_MAC_RX_DESC_T
 *------------------------------------------------------------------------------
*/
/* DW 0 */
#define HAL_RX_STATUS_GET_RX_BYTE_CNT(_prHwMacRxDesc) ((_prHwMacRxDesc)->u2RxByteCount)
#define HAL_RX_STATUS_GET_GROUP_VLD(_prHwMacRxDesc) (((_prHwMacRxDesc)->u2PktTYpe & RX_STATUS_GROUP_VLD_MASK) >> RX_STATUS_GROUP_VLD_OFFSET)
#define HAL_RX_STATUS_GET_PKT_TYPE(_prHwMacRxDesc) (((_prHwMacRxDesc)->u2PktTYpe & RX_STATUS_PKT_TYPE_MASK) >> RX_STATUS_PKT_TYPE_OFFSET)
#define HAL_RX_STATUS_IS_IP_CHECKSUM(_prHwMacRxDesc) ((((_prHwMacRxDesc)->u2PktTYpe & RX_STATUS_IP_CHKSUM)!=0) ? TRUE:FALSE )
#define HAL_RX_STATUS_IS_UT_CHECKSUM(_prHwMacRxDesc) ((((_prHwMacRxDesc)->u2PktTYpe & RX_STATUS_UDP_TCP_CHKSUM) != 0 )? TRUE:FALSE )
#define HAL_RX_STATUS_GET_ETHER_TYPE_OFFSET(_prHwMacRxDesc) (((_prHwMacRxDesc)->u2PktTYpe & RX_STATUS_ETH_TYPE_OFFSET_MASK) >> RX_STATUS_ETH_TYPE_OFFSET)


/* DW 1 */
#define HAL_RX_STATUS_IS_HTC_EXIST(_prHwMacRxDesc)        (((_prHwMacRxDesc)->ucMatchPacket & RX_STATUS_HTC)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_UC2ME(_prHwMacRxDesc)        (((_prHwMacRxDesc)->ucMatchPacket & RX_STATUS_UC2ME)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_MC(_prHwMacRxDesc)        (((_prHwMacRxDesc)->ucMatchPacket & RX_STATUS_MC_FRAME)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_BC(_prHwMacRxDesc)        (((_prHwMacRxDesc)->ucMatchPacket & RX_STATUS_BC_FRAME)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_BCN_WITH_BMC(_prHwMacRxDesc)     (((_prHwMacRxDesc)->ucMatchPacket & RX_STATUS_BCN_WITH_BMC)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_BCN_WITH_UC(_prHwMacRxDesc)      (((_prHwMacRxDesc)->ucMatchPacket & RX_STATUS_BCN_WITH_UC)?TRUE:FALSE)
#define HAL_RX_STATUS_GET_KEY_ID(_prHwMacRxDesc)          (((_prHwMacRxDesc)->ucMatchPacket & RX_STATUS_KEYID_MASK) >> RX_STATUS_KEYID_OFFSET)
#define HAL_RX_STATUS_GET_CHAN_FREQ(_prHwMacRxDesc) ((_prHwMacRxDesc)->ucChanFreq)
#define HAL_RX_STATUS_GET_HEADER_LEN(_prHwMacRxDesc) ((_prHwMacRxDesc)->ucHeaderLen & RX_STATUS_HEADER_LEN_MASK)
#define HAL_RX_STATUS_IS_HEADER_OFFSET(_prHwMacRxDesc) (((_prHwMacRxDesc)->ucHeaderLen & RX_STATUS_HEADER_OFFSET)?TRUE:FALSE)
#define HAL_RX_STATUS_GET_HEADER_OFFSET(_prHwMacRxDesc) (((_prHwMacRxDesc)->ucHeaderLen & RX_STATUS_HEADER_OFFSET)? 2 : 0)
#define HAL_RX_STATUS_IS_HEADER_TRAN(_prHwMacRxDesc) (((_prHwMacRxDesc)->ucHeaderLen & RX_STATUS_HEADER_TRAN)?TRUE:FALSE)
#define HAL_RX_STATUS_GET_HEADER_TRAN(_prHwMacRxDesc) HAL_RX_STATUS_IS_HEADER_TRAN(_prHwMacRxDesc)
#define HAL_RX_STATUS_GET_PAYLOAD_FORMAT(_prHwMacRxDesc)  (((_prHwMacRxDesc)->ucBssid & RX_STATUS_PAYLOAD_FORMAT_MASK) >> RX_STATUS_PAYLOAD_FORMAT_OFFSET)
#define HAL_RX_STATUS_GET_BSSID(_prHwMacRxDesc) (((_prHwMacRxDesc)->ucBssid & RX_STATUS_BSSID_MASK)>> RX_STATUS_BSSID_OFFSET)

/* DW 2 */
#define HAL_RX_STATUS_GET_WLAN_IDX(_prHwMacRxDesc) ((_prHwMacRxDesc)->ucWlanIdx)
#define HAL_RX_STATUS_GET_TID(_prHwMacRxDesc)        (((_prHwMacRxDesc)->ucTidSecMode & RX_STATUS_TID_MASK))
#define HAL_RX_STATUS_GET_SEC_MODE(_prHwMacRxDesc)        (((_prHwMacRxDesc)->ucTidSecMode & RX_STATUS_SEC_MASK) >> RX_STATUS_SEC_OFFSET)
#define HAL_RX_STATUS_GET_SW_BIT(_prHwMacRxDesc)        (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_SW_BIT)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_FCS_ERROR(_prHwMacRxDesc)        (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_FCS_ERROR)?TRUE:FALSE)
//#define HAL_RX_STATUS_IS_FORMAT_ERROR(_prHwMacRxDesc)     (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_FORMAT_ERROR)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_CLM_ERROR(_prHwMacRxDesc)     (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_CLM_ERROR)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_CIPHER_MISMATCH(_prHwMacRxDesc)     (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_CIPHER_MISMATCH)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_ICV_ERROR(_prHwMacRxDesc)        (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_ICV_ERROR)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_TKIP_MIC_ERROR(_prHwMacRxDesc)   (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_TKIPMIC_ERROR)>0?TRUE:FALSE)
#define HAL_RX_STATUS_IS_ERROR(_prHwMacRxDesc)            (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_ERROR_MASK)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_LEN_MISMATCH(_prHwMacRxDesc)     (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_LEN_MISMATCH)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_DE_AMSDU_FAIL(_prHwMacRxDesc)    (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_DE_AMSDU_FAIL)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_EXCEED_LEN(_prHwMacRxDesc)       (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FLAG_EXCEED_LEN)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_LLC_MIS(_prHwMacRxDesc) (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_LLC_MIS)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_UDF_VLT(_prHwMacRxDesc) (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_UDF_VLT)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_FRAG(_prHwMacRxDesc)        (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_FRAG)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_NULL(_prHwMacRxDesc)        (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_NULL)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_DATA(_prHwMacRxDesc)        (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_DATA)?FALSE:TRUE)
#define HAL_RX_STATUS_IS_AMPDU_SUB_FRAME(_prHwMacRxDesc)  (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_AMPDU_SUB_FRAME)?FALSE:TRUE)
#define HAL_RX_STATUS_IS_AMPDU_FORMAT(_prHwMacRxDesc)     (((_prHwMacRxDesc)->u2StatusFlag & RX_STATUS_AMPDU_FORMAT)?FALSE:TRUE)

/* DW 3 */
#define HAL_RX_STATUS_IS_RV_VALID(_prHwMacRxDesc)         (((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_RXV_SEQ_NO_MASK)?TRUE:FALSE)
#define HAL_RX_STATUS_GET_RXV_SEQ_NO(_prHwMacRxDesc)      ((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_RXV_SEQ_NO_MASK)
#define HAL_RX_STATUS_GET_TCL(_prHwMacRxDesc)            (((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_TCL)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_DROP(_prHwMacRxDesc)              (((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_DROP)?TRUE:FALSE)
#define HAL_RX_STATUS_IS_CLS(_prHwMacRxDesc)              (((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_CLS)?TRUE:FALSE)
#define HAL_RX_STATUS_GET_OFLD(_prHwMacRxDesc)            (((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_OFLD_MASK)>> RX_STATUS_OFLD_OFFSET)
#define HAL_RX_STATUS_IS_MGC(_prHwMacRxDesc)              (((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_MGC)?TRUE:FALSE)
#define HAL_RX_STATUS_GET_WOL(_prHwMacRxDesc)             (((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_WOL_MASK)>> RX_STATUS_WOL_OFFSET)
#define HAL_RX_STATUS_GET_CLS_BITMAP(_prHwMacRxDesc)      (((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_CLS_BITMAP_MASK)>> RX_STATUS_CLS_BITMAP_OFFSET)
#define HAL_RX_STATUS_IS_PF_WHITE_LIST(_prHwMacRxDesc)    (((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_PF_MODE)?TRUE:FALSE)
#define HAL_RX_STATUS_GET_PF(_prHwMacRxDesc)              (((_prHwMacRxDesc)->u4PatternFilterInfo & RX_STATUS_PF_STS_MASK) >> RX_STATUS_PF_STS_OFFSET)

/* DW 4~7 */
#define HAL_RX_STATUS_GET_FRAME_CTL_FIELD(_prHwMacRxStsGroup4) ((_prHwMacRxStsGroup4)->u2FrameCtl)
#define HAL_RX_STATUS_GET_TA(_prHwMacRxStsGroup4, pucTA)   \
{\
    kalMemCopy(pucTA, &(_prHwMacRxStsGroup4)->aucTA[0], 6); \
}
#define HAL_RX_STATUS_GET_SEQ_FRAG_NUM(_prHwMacRxStsGroup4)   ((_prHwMacRxStsGroup4)->u2SeqFrag)
#define HAL_RX_STATUS_GET_QOS_CTL_FIELD(_prHwMacRxStsGroup4) ((_prHwMacRxStsGroup4)->u2Qos)

#define HAL_RX_STATUS_GET_SEQFrag_NUM(_prHwMacRxStsGroup4)   ((_prHwMacRxStsGroup4)->u2SeqFrag)
#define HAL_RX_STATUS_GET_HTC(_prHwMacRxStsGroup4) ((_prHwMacRxStsGroup4)->u4HTC)

/* DW 8~11 */
#define HAL_RX_STATUS_GET_RSC(_prHwMacRxStsGroup1, pucRSC)   \
{\
    kalMemCopy(pucRSC, &(_prHwMacRxStsGroup1)->aucPN[0], 6); \
}

#define HAL_RX_STATUS_GET_PN(_prHwMacRxStsGroup1, pucPN)   \
{\
    kalMemCopy(pucPN, &(_prHwMacRxStsGroup1)->aucPN[0], 16); \
}

/* DW 12~13 */
#define HAL_RX_STATUS_GET_TIMESTAMP(_prHwMacRxStsGroup2, _ucIdx) ((_prHwMacRxStsGroup2)->u4Timestamp)
#define HAL_RX_STATUS_GET_FCS32(_prHwMacRxStsGroup2)             ((_prHwMacRxStsGroup2)->u4CRC)


/* DW 14~19 */
#define HAL_RX_STATUS_GET_RX_VECTOR(_prHwMacRxStsGroup3, _ucIdx) ((_prHwMacRxStsGroup3)->u4RxVector[_ucIdx])

#define HAL_RX_STATUS_GET_RCPI(_prHwMacRxStsGroup3) (((_prHwMacRxStsGroup3)->u4RxVector[2] & RX_VT_RCPI0_MASK) >> RX_VT_RCPI0_OFFSET)


//TBD
#define HAL_RX_STATUS_GET_RX_PACKET_LEN(_prHwMacRxDesc)
#define HAL_RX_STATUS_IS_MATCH_PACKET(_prHwMacRxDesc)



/*------------------------------------------------------------------------------
 * MACRO for HW_RX_VECTOR_DESC_T
 *------------------------------------------------------------------------------
*/
#define HAL_RX_VECTOR_GET_TA(_prHwRxVector, pucTA)   \
{\
    kalMemCopy(pucTA, &(_prHwRxVector)->aucTA[0], 6); \
}

#define HAL_RX_VECTOR_GET_SEQ_NO(_prHwRxVector)       ((_prHwRxVector)->u2RxVtSeqNo & RX_VECTOR_SN_MACK)
#define HAL_RX_VECTOR_IS_FOR_BA_ACK(_prHwRxVector)    (((_prHwRxVector)->u2RxVtSeqNo & RX_VECTOR_FOR_BA_ACK)?TRUE:FALSE)
#define HAL_RX_VECTOR_GET_RX_VECTOR(_prHwRxVector, _ucIdx) ((_prHwRxVector)->u4RxVector[_ucIdx])


/*------------------------------------------------------------------------------
 * MACRO for HW_MAC_TX_DESC_T
 *------------------------------------------------------------------------------
*/
#define TX_DESC_GET_FIELD(_rHwMacTxDescField, _mask, _offset) \
                (((_rHwMacTxDescField) & (_mask)) >> (_offset))
#define TX_DESC_SET_FIELD(_rHwMacTxDescField, _value, _mask, _offset)  \
{\
                (_rHwMacTxDescField) &= ~(_mask); \
                (_rHwMacTxDescField) |= (((_value) << (_offset)) & (_mask)); \
}

#define HAL_MAC_TX_DESC_SET_DW(_prHwMacTxDesc, _ucOffsetInDw, _ucLengthInDw, _pucValueAddr) \
        kalMemCopy((PUINT_32)(_prHwMacTxDesc) + (_ucOffsetInDw), (PUINT_8)(_pucValueAddr), DWORD_TO_BYTE(_ucLengthInDw));
#define HAL_MAC_TX_DESC_GET_DW(_prHwMacTxDesc, _ucOffsetInDw, _ucLengthInDw, _pucValueAddr) \
        kalMemCopy((PUINT_8)(_pucValueAddr), (PUINT_32)(_prHwMacTxDesc) + (_ucOffsetInDw), DWORD_TO_BYTE(_ucLengthInDw));

/* DW 0 */
#define HAL_MAC_TX_DESC_GET_TX_BYTE_COUNT(_prHwMacTxDesc) ((_prHwMacTxDesc)->u2TxByteCount)
#define HAL_MAC_TX_DESC_SET_TX_BYTE_COUNT(_prHwMacTxDesc, _u2TxByteCount) \
        ((_prHwMacTxDesc)->u2TxByteCount) = ((UINT_16)_u2TxByteCount)

#define HAL_MAC_TX_DESC_GET_ETHER_TYPE_OFFSET(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucEtherOffset, TX_DESC_ETHER_TYPE_OFFSET_MASK, TX_DESC_ETHER_TYPE_OFFSET_OFFSET)
#define HAL_MAC_TX_DESC_SET_ETHER_TYPE_OFFSET(_prHwMacTxDesc, _ucEtherTypeOffset) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucEtherOffset), ((UINT_8)_ucEtherTypeOffset), TX_DESC_ETHER_TYPE_OFFSET_MASK, TX_DESC_ETHER_TYPE_OFFSET_OFFSET)

#define HAL_MAC_TX_DESC_IS_IP_CHKSUM_ENABLED(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucEtherOffset & TX_DESC_IP_CHKSUM_OFFLOAD)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_IP_CHKSUM(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucEtherOffset |= TX_DESC_IP_CHKSUM_OFFLOAD)
#define HAL_MAC_TX_DESC_UNSET_IP_CHKSUM(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucEtherOffset &= ~TX_DESC_IP_CHKSUM_OFFLOAD)

#define HAL_MAC_TX_DESC_IS_TCP_UDP_CHKSUM_ENABLED(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucPortIdx_QueueIdx & TX_DESC_TCP_UDP_CHKSUM_OFFLOAD)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_TCP_UDP_CHKSUM(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucPortIdx_QueueIdx |= TX_DESC_TCP_UDP_CHKSUM_OFFLOAD)
#define HAL_MAC_TX_DESC_UNSET_TCP_UDP_CHKSUM(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucPortIdx_QueueIdx &= ~TX_DESC_TCP_UDP_CHKSUM_OFFLOAD)

#define HAL_MAC_TX_DESC_IS_USB_NEXT_VLD_ENABLED(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucEtherOffset & TX_DESC_USB_NEXT_VLD)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_USB_NEXT_VLD(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucEtherOffset |= TX_DESC_USB_NEXT_VLD)
#define HAL_MAC_TX_DESC_UNSET_USB_NEXT_VLD(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucEtherOffset &= ~TX_DESC_USB_NEXT_VLD)

#define HAL_MAC_TX_DESC_IS_USB_TX_BURST_ENABLED(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucPortIdx_QueueIdx & TX_DESC_USB_TX_BURST)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_USB_TX_BURST(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucPortIdx_QueueIdx |= TX_DESC_USB_TX_BURST)
#define HAL_MAC_TX_DESC_UNSET_USB_TX_BURST(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucPortIdx_QueueIdx &= ~TX_DESC_USB_TX_BURST)

#define HAL_MAC_TX_DESC_GET_QUEUE_INDEX(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucPortIdx_QueueIdx, TX_DESC_QUEUE_INDEX_MASK, TX_DESC_QUEUE_INDEX_OFFSET)
#define HAL_MAC_TX_DESC_SET_QUEUE_INDEX(_prHwMacTxDesc, _ucQueueIndex) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucPortIdx_QueueIdx), ((UINT_8)_ucQueueIndex), TX_DESC_QUEUE_INDEX_MASK, TX_DESC_QUEUE_INDEX_OFFSET)

#define HAL_MAC_TX_DESC_GET_PORT_INDEX(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucPortIdx_QueueIdx, TX_DESC_PORT_INDEX, TX_DESC_PORT_INDEX_OFFSET)
#define HAL_MAC_TX_DESC_SET_PORT_INDEX(_prHwMacTxDesc, _ucPortIndex) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucPortIdx_QueueIdx), ((UINT_8)_ucPortIndex), TX_DESC_PORT_INDEX, TX_DESC_PORT_INDEX_OFFSET)

/* DW 1 */
#define HAL_MAC_TX_DESC_GET_WLAN_INDEX(_prHwMacTxDesc) \
        (_prHwMacTxDesc)->ucWlanIdx
#define HAL_MAC_TX_DESC_SET_WLAN_INDEX(_prHwMacTxDesc, _ucWlanIdx) \
        ((_prHwMacTxDesc)->ucWlanIdx) = (_ucWlanIdx)

#define HAL_MAC_TX_DESC_IS_LONG_FORMAT(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucHeaderFormat & TX_DESC_FORMAT)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_LONG_FORMAT(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderFormat |= TX_DESC_FORMAT)
#define HAL_MAC_TX_DESC_SET_SHORT_FORMAT(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderFormat &= ~TX_DESC_FORMAT)

#define HAL_MAC_TX_DESC_GET_HEADER_FORMAT(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucHeaderFormat, TX_DESC_HEADER_FORMAT_MASK, TX_DESC_HEADER_FORMAT_OFFSET)
#define HAL_MAC_TX_DESC_SET_HEADER_FORMAT(_prHwMacTxDesc, _ucHdrFormat) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucHeaderFormat), ((UINT_8)_ucHdrFormat), TX_DESC_HEADER_FORMAT_MASK, TX_DESC_HEADER_FORMAT_OFFSET)
#define HAL_MAC_TX_DESC_IS_80211_FORMAT(_prHwMacTxDesc) \
        ((HAL_MAC_TX_DESC_GET_HEADER_FORMAT(_prHwMacTxDesc) & HEADER_FORMAT_802_11_MASK)?TRUE:FALSE)


/* HF = 0x00, 802.11 normal mode */
#define HAL_MAC_TX_DESC_IS_MORE_DATA(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucHeaderFormat & TX_DESC_NON_802_11_MORE_DATA)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_MORE_DATA(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderFormat |= TX_DESC_NON_802_11_MORE_DATA)
#define HAL_MAC_TX_DESC_UNSET_MORE_DATA(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderFormat &= ~TX_DESC_NON_802_11_MORE_DATA)

#define HAL_MAC_TX_DESC_IS_REMOVE_VLAN(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucHeaderFormat & TX_DESC_NON_802_11_REMOVE_VLAN)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_REMOVE_VLAN(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderFormat |= TX_DESC_NON_802_11_REMOVE_VLAN)
#define HAL_MAC_TX_DESC_UNSET_REMOVE_VLAN(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderFormat &= ~TX_DESC_NON_802_11_REMOVE_VLAN)

#define HAL_MAC_TX_DESC_IS_VLAN(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucHeaderFormat & TX_DESC_NON_802_11_VLAN_FIELD)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_VLAN(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderFormat |= TX_DESC_NON_802_11_VLAN_FIELD)
#define HAL_MAC_TX_DESC_UNSET_VLAN(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderFormat &= ~TX_DESC_NON_802_11_VLAN_FIELD)

#define HAL_MAC_TX_DESC_IS_ETHERNET_II(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucHeaderFormat & TX_DESC_NON_802_11_ETHERNET_II)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_ETHERNET_II(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderFormat |= TX_DESC_NON_802_11_ETHERNET_II)
#define HAL_MAC_TX_DESC_UNSET_ETHERNET_II(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderFormat &= ~TX_DESC_NON_802_11_ETHERNET_II)

/* HF = 0x00/0x11, 802.11 normal/enhancement mode */
#define HAL_MAC_TX_DESC_IS_EOSP(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucHeaderFormat & TX_DESC_NON_802_11_EOSP)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_EOSP(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderFormat |= TX_DESC_NON_802_11_EOSP)
#define HAL_MAC_TX_DESC_UNSET_EOSP(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderFormat &= ~TX_DESC_NON_802_11_EOSP)

/* HF = 0x11, 802.11 enhancement mode */
#define HAL_MAC_TX_DESC_IS_AMSDU(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucHeaderFormat & TX_DESC_ENH_802_11_AMSDU)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_AMSDU(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderFormat |= TX_DESC_ENH_802_11_AMSDU)
#define HAL_MAC_TX_DESC_UNSET_AMSDU(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderFormat &= ~TX_DESC_ENH_802_11_AMSDU)

/* HF = 0x10, non-802.11 */
#define HAL_MAC_TX_DESC_GET_802_11_HEADER_LENGTH(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucHeaderFormat, TX_DESC_NOR_802_11_HEADER_LENGTH_MASK, TX_DESC_NOR_802_11_HEADER_LENGTH_OFFSET)
#define HAL_MAC_TX_DESC_SET_802_11_HEADER_LENGTH(_prHwMacTxDesc, _ucHdrLength) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucHeaderFormat), ((UINT_8)_ucHdrLength), TX_DESC_NOR_802_11_HEADER_LENGTH_MASK, TX_DESC_NOR_802_11_HEADER_LENGTH_OFFSET)


#define HAL_MAC_TX_DESC_GET_HEADER_PADDING(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucHeaderPadding, TX_DESC_HEADER_PADDING_LENGTH_MASK, TX_DESC_HEADER_PADDING_LENGTH_OFFSET)
#define HAL_MAC_TX_DESC_SET_HEADER_PADDING(_prHwMacTxDesc, _ucHdrPadding) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucHeaderPadding), ((UINT_8)_ucHdrPadding), TX_DESC_HEADER_PADDING_LENGTH_MASK, TX_DESC_HEADER_PADDING_LENGTH_OFFSET)

#define HAL_MAC_TX_DESC_IS_HEADER_PADDING_IN_THE_HEAD(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucHeaderPadding & TX_DESC_HEADER_PADDING_MODE)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_HEADER_PADDING_IN_THE_HEAD(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderPadding |= TX_DESC_HEADER_PADDING_MODE)
#define HAL_MAC_TX_DESC_SET_HEADER_PADDING_IN_THE_TAIL(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderPadding &= ~TX_DESC_HEADER_PADDING_MODE)

#define HAL_MAC_TX_DESC_IS_NO_ACK(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucHeaderPadding & TX_DESC_NO_ACK)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_NO_ACK(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderPadding |= TX_DESC_NO_ACK)
#define HAL_MAC_TX_DESC_UNSET_NO_ACK(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderPadding &= ~TX_DESC_NO_ACK)

#define HAL_MAC_TX_DESC_GET_TID(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucHeaderPadding, TX_DESC_TID_MASK, TX_DESC_TID_OFFSET)
#define HAL_MAC_TX_DESC_SET_TID(_prHwMacTxDesc, _ucTID) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucHeaderPadding), ((UINT_8)_ucTID), TX_DESC_TID_MASK, TX_DESC_TID_OFFSET)

#define HAL_MAC_TX_DESC_IS_PROTECTION(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucHeaderPadding & TX_DESC_PROTECTED_FRAME)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_PROTECTION(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderPadding |= TX_DESC_PROTECTED_FRAME)
#define HAL_MAC_TX_DESC_UNSET_PROTECTION(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucHeaderPadding &= ~TX_DESC_PROTECTED_FRAME)

#define HAL_MAC_TX_DESC_GET_OWN_MAC_INDEX(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucOwnMAC, TX_DESC_OWN_MAC_MASK, TX_DESC_OWN_MAC_OFFSET)
#define HAL_MAC_TX_DESC_SET_OWN_MAC_INDEX(_prHwMacTxDesc, _ucOwnMacIdx) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucOwnMAC), ((UINT_8)_ucOwnMacIdx), TX_DESC_OWN_MAC_MASK, TX_DESC_OWN_MAC_OFFSET)

/* DW 2 */
#define HAL_MAC_TX_DESC_GET_SUB_TYPE(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucType_SubType, TX_DESC_SUB_TYPE_MASK, TX_DESC_SUB_TYPE_OFFSET)
#define HAL_MAC_TX_DESC_SET_SUB_TYPE(_prHwMacTxDesc, _ucSubType) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucType_SubType), ((UINT_8)_ucSubType), TX_DESC_SUB_TYPE_MASK, TX_DESC_SUB_TYPE_OFFSET)

#define HAL_MAC_TX_DESC_GET_TYPE(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucType_SubType, TX_DESC_TYPE_MASK, TX_DESC_TYPE_OFFSET)
#define HAL_MAC_TX_DESC_SET_TYPE(_prHwMacTxDesc, _ucType) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucType_SubType), ((UINT_8)_ucType), TX_DESC_TYPE_MASK, TX_DESC_TYPE_OFFSET)

#define HAL_MAC_TX_DESC_IS_NDP(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucType_SubType & TX_DESC_NDP)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_NDP(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucType_SubType |= TX_DESC_NDP)
#define HAL_MAC_TX_DESC_UNSET_NDP(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucType_SubType &= ~TX_DESC_NDP)

#define HAL_MAC_TX_DESC_IS_NDPA(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucType_SubType & TX_DESC_NDPA)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_NDPA(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucType_SubType |= TX_DESC_NDPA)
#define HAL_MAC_TX_DESC_UNSET_NDPA(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucType_SubType &= ~TX_DESC_NDPA)

#define HAL_MAC_TX_DESC_IS_SOUNDING_FRAME(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucFrag & TX_DESC_SOUNDING)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_SOUNDING_FRAME(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucFrag |= TX_DESC_SOUNDING)
#define HAL_MAC_TX_DESC_UNSET_SOUNDING_FRAME(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucFrag &= ~TX_DESC_SOUNDING)

#define HAL_MAC_TX_DESC_IS_FORCE_RTS_CTS_EN(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucFrag & TX_DESC_FORCE_RTS_CTS)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_FORCE_RTS_CTS(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucFrag |= TX_DESC_FORCE_RTS_CTS)
#define HAL_MAC_TX_DESC_UNSET_FORCE_RTS_CTS(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucFrag &= ~TX_DESC_FORCE_RTS_CTS)

#define HAL_MAC_TX_DESC_IS_BMC(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucFrag & TX_DESC_BROADCAST_MULTICAST)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_BMC(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucFrag |= TX_DESC_BROADCAST_MULTICAST)
#define HAL_MAC_TX_DESC_UNSET_BMC(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucFrag &= ~TX_DESC_BROADCAST_MULTICAST)

#define HAL_MAC_TX_DESC_IS_BIP(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucFrag & TX_DESC_BIP_PROTECTED)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_BIP(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucFrag |= TX_DESC_BIP_PROTECTED)
#define HAL_MAC_TX_DESC_UNSET_BIP(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucFrag &= ~TX_DESC_BIP_PROTECTED)

#define HAL_MAC_TX_DESC_IS_DURATION_CONTROL_BY_SW(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucFrag & TX_DESC_DURATION_FIELD_CONTROL)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_DURATION_CONTROL_BY_SW(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucFrag |= TX_DESC_DURATION_FIELD_CONTROL)
#define HAL_MAC_TX_DESC_SET_DURATION_CONTROL_BY_HW(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucFrag &= ~TX_DESC_DURATION_FIELD_CONTROL)

#define HAL_MAC_TX_DESC_IS_HTC_EXIST(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucFrag & TX_DESC_HTC_EXISTS)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_HTC_EXIST(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucFrag |= TX_DESC_HTC_EXISTS)
#define HAL_MAC_TX_DESC_UNSET_HTC_EXIST(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucFrag &= ~TX_DESC_HTC_EXISTS)

#define HAL_MAC_TX_DESC_IS_FRAG_PACKET(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucFrag & TX_DESC_FRAGMENT_MASK)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_GET_FRAG_PACKET_POS(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucFrag, TX_DESC_FRAGMENT_MASK, TX_DESC_FRAGMENT_OFFSET)
#define HAL_MAC_TX_DESC_SET_FRAG_PACKET_POS(_prHwMacTxDesc, _ucFragPos) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucFrag), ((UINT_8)_ucFragPos), TX_DESC_FRAGMENT_MASK, TX_DESC_FRAGMENT_OFFSET)

/* (FW side) Remaining Tx time field in TXD => |<--Life timeout delta time-->|<--free run counter-->| */
/* For packets generated by FW, the Tx life timeout field should be added by a free run counter(WF_LP_TOP.FRCR). */
/* (Driver side) Remaining Tx time field in TXD => |<--Life timeout delta time-->| */
/* For packets generated by Driver, free run counter will be added to Tx life timeout filed by HIF. */

/* in unit of 32TU */
#define HAL_MAC_TX_DESC_GET_MAX_TX_TIME(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucRemainingMaxTxTime)
#define HAL_MAC_TX_DESC_SET_MAX_TX_TIME(_prHwMacTxDesc, _ucTxTime) (_prHwMacTxDesc)->ucRemainingMaxTxTime = (UINT_8)(_ucTxTime)

#define HAL_MAC_TX_DESC_GET_POWER_OFFSET(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucPowerOffset, TX_DESC_POWER_OFFSET_MASK, 0)
#define HAL_MAC_TX_DESC_SET_POWER_OFFSET(_prHwMacTxDesc, _ucPowerOffset) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucPowerOffset), ((UINT_8)_ucPowerOffset), TX_DESC_POWER_OFFSET_MASK, 0)

#define HAL_MAC_TX_DESC_IS_BA_DISABLE(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucPowerOffset & TX_DESC_BA_DISABLE)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_BA_DISABLE(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucPowerOffset |= TX_DESC_BA_DISABLE)
#define HAL_MAC_TX_DESC_SET_BA_ENABLE(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucPowerOffset &= ~TX_DESC_BA_DISABLE)

#define HAL_MAC_TX_DESC_IS_TIMING_MEASUREMENT(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucPowerOffset & TX_DESC_TIMING_MEASUREMENT)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_TIMING_MEASUREMENT(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucPowerOffset |= TX_DESC_TIMING_MEASUREMENT)
#define HAL_MAC_TX_DESC_UNSET_TIMING_MEASUREMENT(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucPowerOffset &= ~TX_DESC_TIMING_MEASUREMENT)

#define HAL_MAC_TX_DESC_IS_FIXED_RATE_ENABLE(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucPowerOffset & TX_DESC_FIXED_RATE)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_FIXED_RATE_ENABLE(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucPowerOffset |= TX_DESC_FIXED_RATE)
#define HAL_MAC_TX_DESC_SET_FIXED_RATE_DISABLE(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucPowerOffset &= ~TX_DESC_FIXED_RATE)

/* DW 3 */
#define HAL_MAC_TX_DESC_GET_TX_COUNT(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->u2TxCountLimit, TX_DESC_TX_COUNT_MASK, TX_DESC_TX_COUNT_OFFSET)
#define HAL_MAC_TX_DESC_SET_TX_COUNT(_prHwMacTxDesc, _ucTxCountLimit) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->u2TxCountLimit), ((UINT_8)_ucTxCountLimit), TX_DESC_TX_COUNT_MASK, TX_DESC_TX_COUNT_OFFSET)
#define HAL_MAC_TX_DESC_GET_REMAINING_TX_COUNT(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->u2TxCountLimit, TX_DESC_REMAINING_TX_COUNT_MASK, TX_DESC_REMAINING_TX_COUNT_OFFSET)
#define HAL_MAC_TX_DESC_SET_REMAINING_TX_COUNT(_prHwMacTxDesc, _ucTxCountLimit) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->u2TxCountLimit), ((UINT_8)_ucTxCountLimit), TX_DESC_REMAINING_TX_COUNT_MASK, TX_DESC_REMAINING_TX_COUNT_OFFSET)
#define HAL_MAC_TX_DESC_GET_SEQUENCE_NUMBER(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->u2SN, TX_DESC_SEQUENCE_NUMBER, 0)
#define HAL_MAC_TX_DESC_SET_SEQUENCE_NUMBER(_prHwMacTxDesc, _u2SN) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->u2SN), ((UINT_16)_u2SN), TX_DESC_SEQUENCE_NUMBER, 0)
#define HAL_MAC_TX_DESC_SET_HW_RESERVED(_prHwMacTxDesc, _ucHwReserved) \
            TX_DESC_SET_FIELD(((_prHwMacTxDesc)->u2SN), ((UINT_8)_ucHwReserved), TX_DESC_HW_RESERVED_MASK, TX_DESC_HW_RESERVED_OFFSET)
#define HAL_MAC_TX_DESC_IS_TXD_SN_VALID(_prHwMacTxDesc) (((_prHwMacTxDesc)->u2SN & TX_DESC_SN_IS_VALID)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_TXD_SN_VALID(_prHwMacTxDesc) ((_prHwMacTxDesc)->u2SN |= TX_DESC_SN_IS_VALID)
#define HAL_MAC_TX_DESC_SET_TXD_SN_INVALID(_prHwMacTxDesc) ((_prHwMacTxDesc)->u2SN &= ~TX_DESC_SN_IS_VALID)

#define HAL_MAC_TX_DESC_IS_TXD_PN_VALID(_prHwMacTxDesc) (((_prHwMacTxDesc)->u2SN & TX_DESC_PN_IS_VALID)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_TXD_PN_VALID(_prHwMacTxDesc) ((_prHwMacTxDesc)->u2SN |= TX_DESC_PN_IS_VALID)
#define HAL_MAC_TX_DESC_SET_TXD_PN_INVALID(_prHwMacTxDesc) ((_prHwMacTxDesc)->u2SN &= ~TX_DESC_PN_IS_VALID)

#define HAL_MAC_TX_DESC_ASSIGN_SN_BY_SW(_prHwMacTxDesc, _u2SN) \
{ \
        HAL_MAC_TX_DESC_SET_SEQUENCE_NUMBER(_prHwMacTxDesc, _u2SN); \
        HAL_MAC_TX_DESC_SET_TXD_SN_VALID(_prHwMacTxDesc); \
}
#define HAL_MAC_TX_DESC_ASSIGN_SN_BY_HW(_prHwMacTxDesc) \
{ \
        HAL_MAC_TX_DESC_SET_SEQUENCE_NUMBER(_prHwMacTxDesc, 0); \
        HAL_MAC_TX_DESC_SET_TXD_SN_INVALID(_prHwMacTxDesc); \
}

/* DW 4 */
#define HAL_MAC_TX_DESC_GET_PN(_prHwMacTxDesc, _u4PN_0_31, _u2PN_32_47) \
{ \
        ((UINT_32)_u4PN_0_31) = (_prHwMacTxDesc)->u4PN1; \
        ((UINT_16)_u2PN_32_47) = (_prHwMacTxDesc)->u2PN2; \
}
#define HAL_MAC_TX_DESC_SET_PN(_prHwMacTxDesc, _u4PN_0_31, _u2PN_32_47) \
{ \
        (_prHwMacTxDesc)->u4PN1 = ((UINT_32)_u4PN_0_31); \
        (_prHwMacTxDesc)->u2PN2 = ((UINT_16)_u2PN_32_47); \
}

#define HAL_MAC_TX_DESC_ASSIGN_PN_BY_SW(_prHwMacTxDesc, _u4PN_0_31, _u2PN_32_47) \
{ \
        HAL_MAC_TX_DESC_SET_PN(_prHwMacTxDesc, _u4PN_0_31, _u2PN_32_47); \
        HAL_MAC_TX_DESC_SET_TXD_PN_VALID(_prHwMacTxDesc); \
}
#define HAL_MAC_TX_DESC_ASSIGN_PN_BY_HW(_prHwMacTxDesc) \
{ \
        HAL_MAC_TX_DESC_SET_PN(_prHwMacTxDesc, 0, 0); \
        HAL_MAC_TX_DESC_SET_TXD_PN_INVALID(_prHwMacTxDesc); \
}


/* DW 5 */
#define HAL_MAC_TX_DESC_GET_PID(_prHwMacTxDesc) \
        (_prHwMacTxDesc)->ucPID
#define HAL_MAC_TX_DESC_SET_PID(_prHwMacTxDesc, _ucPID) \
        ((_prHwMacTxDesc)->ucPID) = (_ucPID)

#define HAL_MAC_TX_DESC_GET_TXS_FORMAT(_prHwMacTxDesc) \
        TX_DESC_GET_FIELD((_prHwMacTxDesc)->ucTxStatus, TX_DESC_TX_STATUS_FORMAT, TX_DESC_TX_STATUS_FORMAT_OFFSET)
#define HAL_MAC_TX_DESC_SET_TXS_FORMAT(_prHwMacTxDesc, _ucTXSFormat) \
        TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucTxStatus), ((UINT_8)_ucTXSFormat), TX_DESC_TX_STATUS_FORMAT, TX_DESC_TX_STATUS_FORMAT_OFFSET)

#define HAL_MAC_TX_DESC_IS_TXS_TO_MCU(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucTxStatus & TX_DESC_TX_STATUS_TO_MCU)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_TXS_TO_MCU(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucTxStatus |= TX_DESC_TX_STATUS_TO_MCU)
#define HAL_MAC_TX_DESC_UNSET_TXS_TO_MCU(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucTxStatus &= ~TX_DESC_TX_STATUS_TO_MCU)

#define HAL_MAC_TX_DESC_IS_TXS_TO_HOST(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucTxStatus & TX_DESC_TX_STATUS_TO_HOST)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_TXS_TO_HOST(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucTxStatus |= TX_DESC_TX_STATUS_TO_HOST)
#define HAL_MAC_TX_DESC_UNSET_TXS_TO_HOST(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucTxStatus &= ~TX_DESC_TX_STATUS_TO_HOST)

#define HAL_MAC_TX_DESC_IS_DA_FROM_WTBL(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucTxStatus & TX_DESC_DA_SOURCE)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_DA_FROM_WTBL(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucTxStatus |= TX_DESC_DA_SOURCE)
#define HAL_MAC_TX_DESC_SET_DA_FROM_MSDU(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucTxStatus &= ~TX_DESC_DA_SOURCE)

#define HAL_MAC_TX_DESC_IS_SW_BAR_SSN(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucTxStatus & TX_DESC_BAR_SSN_CONTROL)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_SW_BAR_SSN(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucTxStatus |= TX_DESC_BAR_SSN_CONTROL)
#define HAL_MAC_TX_DESC_SET_HW_BAR_SSN(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucTxStatus &= ~TX_DESC_BAR_SSN_CONTROL)

#define HAL_MAC_TX_DESC_IS_SW_PM_CONTROL(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucTxStatus & TX_DESC_POWER_MANAGEMENT_CONTROL)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_SW_PM_CONTROL(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucTxStatus |= TX_DESC_POWER_MANAGEMENT_CONTROL)
#define HAL_MAC_TX_DESC_SET_HW_PM_CONTROL(_prHwMacTxDesc) ((_prHwMacTxDesc)->ucTxStatus &= ~TX_DESC_POWER_MANAGEMENT_CONTROL)


/* DW 6 */
#define HAL_MAC_TX_DESC_IS_CR_FIXED_RATE_MODE(_prHwMacTxDesc) (((_prHwMacTxDesc)->ucAntID & TX_DESC_FIXED_RATE_MODE)?TRUE:FALSE)
#define HAL_MAC_TX_DESC_SET_FIXED_RATE_MODE_TO_DESC(_prHwMacTxDesc) \
    ((_prHwMacTxDesc)->ucAntID &= ~TX_DESC_FIXED_RATE_MODE)
#define HAL_MAC_TX_DESC_SET_FIXED_RATE_MODE_TO_CR(_prHwMacTxDesc) \
    ((_prHwMacTxDesc)->ucAntID |= TX_DESC_FIXED_RATE_MODE)

#define HAL_MAC_TX_DESC_SET_FR_ANTENNA_ID(_prHwMacTxDesc, _ucAntId) \
    TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucAntID), ((UINT_8)_ucAntId), TX_DESC_ANTENNA_INDEX_MASK, 2)

#define HAL_MAC_TX_DESC_SET_FR_BW(_prHwMacTxDesc, ucBw) \
    TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucBandwidth), ((UINT_8)ucBw), TX_DESC_BANDWIDTH_MASK, 0)

#define HAL_MAC_TX_DESC_SET_FR_SPE_EN(_prHwMacTxDesc) \
    ((_prHwMacTxDesc)->ucBandwidth |= TX_DESC_BANDWIDTH_MASK)

#define HAL_MAC_TX_DESC_SET_FR_ANTENNA_PRIORITY(_prHwMacTxDesc, _ucAntPri) \
    TX_DESC_SET_FIELD(((_prHwMacTxDesc)->ucBandwidth), ((UINT_8)_ucAntPri), TX_DESC_ANTENNA_PRIORITY, 4)

#define HAL_MAC_TX_DESC_SET_FR_DYNAMIC_BW_RTS(_prHwMacTxDesc) \
     ((_prHwMacTxDesc)->ucBandwidth |= TX_DESC_DYNAMIC_BANDWIDTH)

#define HAL_MAC_TX_DESC_SET_FR_ETX_BF(_prHwMacTxDesc) \
     ((_prHwMacTxDesc)->u2FixedRate |= TX_DESC_EXPLICIT_BEAMFORMING)

#define HAL_MAC_TX_DESC_SET_FR_ITX_BF(_prHwMacTxDesc) \
     ((_prHwMacTxDesc)->u2FixedRate |= TX_DESC_IMPLICIT_BEAMFORMING)

#define HAL_MAC_TX_DESC_SET_FR_RATE(_prHwMacTxDesc, _u2RatetoFixed) \
    TX_DESC_SET_FIELD(((_prHwMacTxDesc)->u2FixedRate), ((UINT_16)_u2RatetoFixed), TX_DESC_FIXDE_RATE_MASK, 2)

#define HAL_MAC_TX_DESC_SET_FR_LDPC(_prHwMacTxDesc) \
    ((_prHwMacTxDesc)->u2FixedRate |= TX_DESC_LDPC)

#define HAL_MAC_TX_DESC_SET_FR_SHORT_GI(_prHwMacTxDesc) \
    ((_prHwMacTxDesc)->u2FixedRate |= TX_DESC_GUARD_INTERVAL)

#define HAL_MAC_TX_DESC_SET_FR_NORMAL_GI(_prHwMacTxDesc) \
    ((_prHwMacTxDesc)->u2FixedRate &= ~TX_DESC_GUARD_INTERVAL)


//Old MACRO, for ROM build pass, to be removed
#if 0
#define HAL_MAC_TX_DESC_GET_NEXT_DESC(_prHwMacTxDesc) NULL
#define HAL_MAC_TX_DESC_SET_NEXT_DESC(_prHwMacTxDesc, _u4NextDescAddr)
#define HAL_MAC_TX_DESC_IS_FW_OWN(_prHwMacTxDesc) NULL
#define HAL_MAC_TX_DESC_IS_MAC_DONE(_prHwMacTxDesc) NULL
#endif
//Old MACRO END

/* TX Status */
#define HAL_MAC_TX_STATUS_HEADER_GET_PKT_TYPE(_prHwMacTxStatusPacket) \
    (((_prHwMacTxStatusPacket)->ucPktType & TX_STATUS_HEADER_PKT_TYPE) >> TX_STATUS_HEADER_PKT_TYPE_OFFSET)
#define HAL_MAC_TX_STATUS_HEADER_GET_TXS_COUNT(_prHwMacTxStatusPacket) \
    (((_prHwMacTxStatusPacket)->ucTxStsCnt & TX_STATUS_HEADER_TXS_COUNT) >> TX_STATUS_HEADER_TXS_COUNT_OFFSET)
#define HAL_MAC_TX_STATUS_HEADER_GET_TOTAL_BYTE_COUNT(_prHwMacTxStatusPacket) \
    ((_prHwMacTxStatusPacket)->u2RxByteCnt & TX_STATUS_HEADER_TOTAL_BYTE_COUNT)

/* DW 0 */
#define HAL_MAC_TX_STATUS_GET_TX_RATE(_prHwMacTxStatus) ((_prHwMacTxStatus)->u2TxRate & TX_STATUS_TX_RATE)
#define HAL_MAC_TX_STATUS_IS_FIXED_RATE(_prHwMacTxStatus) (((_prHwMacTxStatus)->u2TxRate & TX_STATUS_FIXED_RATE)?TRUE:FALSE)
#define HAL_MAC_TX_STATUS_IS_TX_STATUS_FORMAT(_prHwMacTxStatus) (((_prHwMacTxStatus)->u2TxRate & TX_STATUS_TX_STATUS_FORMAT)?TRUE:FALSE)
#define HAL_MAC_TX_STATUS_IS_TX_STATUS_TO_MCU(_prHwMacTxStatus) (((_prHwMacTxStatus)->u2TxRate & TX_STATUS_TX_STATUS_TO_MCU)?TRUE:FALSE)
#define HAL_MAC_TX_STATUS_IS_TX_STATUS_TO_HOST(_prHwMacTxStatus) (((_prHwMacTxStatus)->u2TxRate & TX_STATUS_TX_STATUS_TO_HOST)?TRUE:FALSE)
#define HAL_MAC_TX_STATUS_IS_MPDU_ERROR(_prHwMacTxStatus) (((_prHwMacTxStatus)->u2ErrStatus & TX_STATUS_MPDU_ERROR)?TRUE:FALSE)
#define HAL_MAC_TX_STATUS_IS_RTS_ERROR(_prHwMacTxStatus) (((_prHwMacTxStatus)->u2ErrStatus & TX_STATUS_RTS_ERROR)?TRUE:FALSE)
#define HAL_MAC_TX_STATUS_IS_LIFETIME_ERROR(_prHwMacTxStatus) (((_prHwMacTxStatus)->u2ErrStatus & TX_STATUS_LIFETIME_ERROR)?TRUE:FALSE)
#define HAL_MAC_TX_STATUS_IS_BIP_ERROR(_prHwMacTxStatus) (((_prHwMacTxStatus)->u2ErrStatus & TX_STATUS_BIP_ERROR)?TRUE:FALSE)
#define HAL_MAC_TX_STATUS_IS_TXOP_EXCEED(_prHwMacTxStatus) (((_prHwMacTxStatus)->u2ErrStatus & TX_STATUS_TXOP_EXCEED_WARNING)?TRUE:FALSE)
#define HAL_MAC_TX_STATUS_IS_TX_PS_BIT_SET(_prHwMacTxStatus) ((_prHwMacTxStatus)->u2ErrStatus & TX_STATUS_PS_FIELD?TRUE:FALSE)
#define HAL_MAC_TX_STATUS_IS_BA_UNAVAILABLE(_prHwMacTxStatus) (((_prHwMacTxStatus)->u2ErrStatus & TX_STATUS_BA_UNAVAILABLE)?TRUE:FALSE)
#define HAL_MAC_TX_STATUS_GET_TID(_prHwMacTxStatus) (((_prHwMacTxStatus)->u2ErrStatus & TX_STATUS_TID_MASK)>> TX_STATUS_TID_MASK_OFFSET)
#define HAL_MAC_TX_STATUS_GET_ANTENNA_INDEX(_prHwMacTxStatus) (((_prHwMacTxStatus)->u2ErrStatus & TX_STATUS_ANTENNA_INDEX_MASK)>> TX_STATUS_ANTENNA_INDEX_OFFSET)
#define HAL_MAC_TX_STATUS_IS_STATUS_OK(_prHwMacTxStatus) (((_prHwMacTxStatus)->u2ErrStatus & TX_STATUS_ERROR_BITS)?FALSE:TRUE)

/* DW 1 */
#define HAL_MAC_TX_STATUS_GET_FMT0_TIMESTAMP(_prHwMacTxStatus) ((_prHwMacTxStatus)->u4TimeStamp)
#define HAL_MAC_TX_STATUS_GET_FMT1_NOISE0(_prHwMacTxStatus) (((_prHwMacTxStatus)->u4TimeStamp & TX_STATUS_NOISE0) >> TX_STATUS_NOISE0_OFFSET)
#define HAL_MAC_TX_STATUS_GET_FMT1_NOISE1(_prHwMacTxStatus) (((_prHwMacTxStatus)->u4TimeStamp & TX_STATUS_NOISE1) >> TX_STATUS_NOISE1_OFFSET)
#define HAL_MAC_TX_STATUS_GET_FMT1_NOISE2(_prHwMacTxStatus) (((_prHwMacTxStatus)->u4TimeStamp & TX_STATUS_NOISE2) >> TX_STATUS_NOISE2_OFFSET)

/* DW 2 */
#define HAL_MAC_TX_STATUS_GET_FMT0_PKTFRONTTIME(_prHwMacTxStatus)   ((_prHwMacTxStatus)->u4PktFrontTime & TX_STATUS_PKT_FRONT_TIME_PART)
#define HAL_MAC_TX_STATUS_GET_FMT1_RCPI0(_prHwMacTxStatus) (((_prHwMacTxStatus)->u4PktFrontTime & TX_STATUS_RCPI0) >> TX_STATUS_RCPI0_OFFSET)
#define HAL_MAC_TX_STATUS_GET_FMT1_RCPI1(_prHwMacTxStatus) (((_prHwMacTxStatus)->u4PktFrontTime & TX_STATUS_RCPI1) >> TX_STATUS_RCPI1_OFFSET)
#define HAL_MAC_TX_STATUS_GET_FMT1_RCPI2(_prHwMacTxStatus) (((_prHwMacTxStatus)->u4PktFrontTime & TX_STATUS_RCPI2) >> TX_STATUS_RCPI2_OFFSET)
#define HAL_MAC_TX_STATUS_GET_TX_POWER_DBM(_prHwMacTxStatus) (((_prHwMacTxStatus)->u4PktFrontTime & TX_STATUS_TX_POWER_DBM) >> TX_STATUS_TX_POWER_DBM_OFFSET)

/* DW 3 */
#define HAL_MAC_TX_STATUS_GET_AIR_DELAY(_prHwMacTxStatus) ((_prHwMacTxStatus)->u2TxDelay)
#define HAL_MAC_TX_STATUS_GET_RV_SEQ_NUM(_prHwMacTxStatus) ((_prHwMacTxStatus)->ucTxRXVSeqNum)
#define HAL_MAC_TX_STATUS_GET_WLAN_INDEX(_prHwMacTxStatus) ((_prHwMacTxStatus)->ucWLANIdex)

/* DW 4 */
#define HAL_MAC_TX_STATUS_GET_FMT0_SN(_prHwMacTxStatus) ((_prHwMacTxStatus)->u4Tssi & TX_STATUS_SN)
#define HAL_MAC_TX_STATUS_GET_FMT1_TSSI(_prHwMacTxStatus) ((_prHwMacTxStatus)->u4Tssi & TX_STATUS_TSSI)
#define HAL_MAC_TX_STATUS_GET_TX_BW(_prHwMacTxStatus) (((_prHwMacTxStatus)->u4Tssi & TX_STATUS_TX_BW) >> TX_STATUS_TX_BW_OFFSET)
#define HAL_MAC_TX_STATUS_GET_PID(_prHwMacTxStatus) (((_prHwMacTxStatus)->u4Tssi & TX_STATUS_PID) >> TX_STATUS_PID_OFFSET)
#define HAL_MAC_TX_STATUS_GET_FM(_prHwMacTxStatus)     (((_prHwMacTxStatus)->u4Tssi & TX_STATUS_FM)>> TX_STATUS_FM_OFFSET)
#define HAL_MAC_TX_STATUS_GET_AM(_prHwMacTxStatus)     (((_prHwMacTxStatus)->u4Tssi & TX_STATUS_AM)>> TX_STATUS_AM_OFFSET)
#define HAL_MAC_TX_STATUS_GET_MPDU_TX_CNT(_prHwMacTxStatus) (((_prHwMacTxStatus)->u4Tssi & TX_STATUS_LAST_MPDU_CNT) >> TX_STATUS_LAST_MPDU_CNT_OFFSET)
#define HAL_MAC_TX_STATUS_GET_LAST_MCS_IDX(_prHwMacTxStatus) (((_prHwMacTxStatus)->u4Tssi & TX_STATUS_LAST_MCS_IDX) >> TX_STATUS_LAST_MCS_IDX_OFFSET)

#define PHY_RX_VECTOR_GET_RCPI(_u4Value) (((_u4Value) & RX_VT_RCPI0_MASK) >> RX_VT_RCPI0_OFFSET)

#define PHY_RX_VECTOR_GET_CCK_LQ(_u4Value)         (((_u4Value) & RX_VT_CCK_LQ) >>RX_VT_LQ_OFFSET)
#define PHY_RX_VECTOR_GET_OFDM_LQ_BPSK(_u4Value)   (((_u4Value) & RX_VT_OFDM_LQ_BPSK)>>RX_VT_LQ_OFFSET)


/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _MT6630_WIFI_HW_H */

