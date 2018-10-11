/*
** $Id: //Department/DaVinci/TRUNK/MT6628_FPGA_BORA_BASE_20111109/mcu/wifi/ROM/include/nic/hal/MT6628/hal_rom.h#2 $
*/

/*! \file   "hal.h"
    \brief  The declaration of hal functions, for abstractiion of HW access procedures

    N/A
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
** $Log: hal.h $
**
** 05 22 2015 changyi.hsu
** [BORA00004545] [MT7637E2]
** 1. Pass all SEC SPP A-MSDU test cases.
** 2. Fix WTBL array size issue.
**
** 01 12 2015 tiger.hung
** [BORA00003732] [MT7637][E1][MP][WIFI]
** - Update Reg definitions 
** - Add rom API
**
** 01 05 2015 tiger.hung
** [BORA00003732] [MT7637][E1][MP][WIFI]
** Move Ram API to ROM
**
** 01 04 2015 ted.huang
** [BORA00003379] [MT7637]
** add smart mesh hal API and emulation code.
**
** 01 02 2015 rick.wen
** [BORA00003725] [WIFI] ROM code implementation
** - Remove some WiFi APIs to ROM code.
**
** 11 20 2014 changyi.hsu
** [BORA00003214] [MT7637]
** 1. Move DSW related HAL functions to "hal_dynamic_sw_rom.c".
**
** 11 19 2014 changyi.hsu
** [BORA00003214] [MT7637]
** 1. Move WTBL related HAL functions to "hal_wtbl_rom.c".
**
** 11 15 2014 ted.huang
** [BORA00003379] [MT7637]
** move RAM hal to ROM.
**
** 11 03 2014 ted.huang
** [BORA00003379] [MT7637]
** 1)WTBL entry support 128, 2) add repeater/MBSS/Indium emulation cases.
**
** 10 31 2014 changyi.hsu
** [BORA00003214] [MT7637/MT7615]
** 1. Merge form RAM to ROM HAL functions.
**
** 10 08 2014 changyi.hsu
** [BORA00003214] [MT7637/MT7615]
** 1. Fix bandwidth and center frequency bug in "bcmUpdateChInfo" function.
** 2. Fix Tx mode bug in "emulTxRspRateLookup" function.
** 3. Add new TMAC CR address for dynamic silemce window (DSW).
** 4. Add new TMAC HAL function for dynamic silemce window (DSW).
** 5. Add two kinds of Rx function for DSW.
** 6. Add 10 DSW test cases.
**
** 09 24 2014 ted.huang
** [BORA00003379] [MT7637]
** remove redundant WIFI inband command.
**
** 01 01 2014 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Merge MT6630 E2 Change and Refine Code
**
** 12 30 2013 ted.huang
** [BORA00002807] [MT7636]
** Add new duplicated RFB hal API.
**
** 12 30 2013 chiahsuan.chuang
** [BORA00002663] [WiFi][HIF]
** .add sdio transport detection.
**
** 12 17 2013 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Delete Unused Code
**
** 12 09 2013 ted.huang
** [BORA00002807] [MT7636]
** update 7636 WIFI ROM code.
**
** 11 24 2013 ted.huang
** [BORA00002807] [MT7636]
** 1. add RX SMPS emulation code 2. merge ROM from 6630E2.
**
** 11 19 2013 ted.huang
** [BORA00002807] [MT7636]
** Add RMAC/LP design change emulation code.
**
** 11 13 2013 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Tx AMPDU Emulation Change and Bug Fix
**
** 06 20 2013 chia-hsiang.hsu
** [BORA00002524] [MT6630 Wi-Fi] hal function and emulation code update
** Add hal functions
**
** 06 18 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** Add rate to phyrate API
**
** 06 14 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Move Measurement HAL functions to RAM code
**
** 06 14 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** Add utility rom API and remove unused API
**
** 06 14 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** Change rate1~rate8 to array
**
** 06 13 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** .
**
** 06 10 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Add halTxRateIndex2RateCode() API
**
** 06 09 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** Refine ar api
**
** 06 09 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** Refine the ar hal api
**
** 06 07 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** .
**
** 06 07 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** add Api to get all TRX statistic
**
** 06 04 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** 1. Remove unused code 2. move the API to RAM 3. enable crypto
**
** 06 04 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Modified Measurement & Quiet HAL APIs
**
** 05 27 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** 1. Add VLAN/Mesh/Full score board/secondary 40 CCA related defintions and APIs
** 2. Fix duplicate RFB configuration hal function
** 3. Add/modify duplicate RFB/Blcok Ack policy/LQ/perBSS PPDU counter/RTS BW signaling/PSM test cases
**
** 05 27 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Add Beacon Queue Control API
**
** 05 20 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** Update RDG emulation
**
** 05 20 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** add rx security emulation code.
**
** 05 16 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Update TX emulation code
**
** 05 13 2013 bruce.kang
** [BORA00002409] [MT6630 Wi-Fi]
** 1. Add interfaces between WMT and Wi-Fi calibration module (rlm)
** 2. Add Rom to Ram API
** 3. Add debug count in bcm module
** 4. Add TX CF-End in bss module
**
** 05 10 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Modify RCPI and LQ related codes and defintion according to new release CR spec.
**
** 05 10 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** Update Get/Set SN HAL
**
** 05 08 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** 1. Update halMacTxSetBmcCnt API
** 2. Add halConfigEnableMCS32To0 API
** 3. Update TMAC emulation code
**
** 05 07 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** Update AMPDU MIB counter HAL
**
** 05 03 2013 bruce.kang
** [BORA00002409] [MT6630 Wi-Fi]
** 1. Move bcm rom code to file bcm_rom.c
** 2. Fix multiple BT profile message settings
**
** 05 03 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** update AR emulation code
**
** 05 02 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** .
**
** 04 30 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add halMacRxConfigTSFUpdateFrames to config frames for tsf sync.
**
** 04 30 2013 bruce.kang
** [BORA00002409] [MT6630 Wi-Fi]
** Apply Wi-Fi API interface in rom code for WMT to achieve dynamic NAV/RWP scheme.
**
** 04 29 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** 1. Add HAL APIs for BMC queue counter set /clear
** 2. Update TX header translation emulation code
**
** 04 22 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** 1. Update and fixed TX Queue flush APIs
** 2. Update Trap timer API
**
** 04 17 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** 1. New per-BSS ERP protection.
** 2. New TX rate code definition
** 3. RTS retry limit extended to 32
**
** 04 16 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Fix macheader compose function in A4 case
** Add ACI defintion for new ACI design
** Modify LNA enum value for new ACI design
**
** 04 15 2013 chia-hsiang.hsu
** [BORA00002524] [MT6630 Wi-Fi] hal function and emulation code update
** Fix the typo
**
** 04 15 2013 chia-hsiang.hsu
** [BORA00002524] [MT6630 Wi-Fi] hal function and emulation code update
** Update the hal functions (MT6630 ARB CR v0.50)
**
** 04 09 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Update WTBL1 BC/MC and not-Matched MUAR defintion according to updated WTBL doc.
**
** 04 09 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Modify halWtblUpdateKeyMaterial and halWtblGetKeyMaterial for WTBL3 configuration
** Modify SW WLAN index search for new desing of searching rule
**
** 04 05 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** 1. Modify codes and definition for new ACI design
** 2. Modify WLAN table search API
** 3. Modify TFT test cases according to new ACI design
**
** 04 02 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** remove redundant definition about RX AMPDU MIB counter
** fix RX total byte MIB counter read action
**
** 04 02 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** 1. Add TX/RX counters and byte counters for perBSS usage
** 2. Add Hal_ini about AMPDU and AMSDU de-aggregation enable
** 3. Modify MIB counter get API to clear the read data
**
** 04 01 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Fixed halMacTxConfigBssidBasicRate() definition.
**
** 04 01 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** 1.Update BSSID basic reate HAL function
** 2.Change emulTX packet size from 2000 to 1600 bytes
** 3.Synth. emulation TX test cases
**
** 03 27 2013 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Modify the initial procedures of RAM code
**
** 03 26 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** 1. Remove AC guard time setting from halTxConfigACParameters
** 2. Add halTxConfigACGuardTime() to set guard time
**
** 03 25 2013 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Enable more initial functions in hem.c
**
** 03 19 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** .
**
** 03 18 2013 tsaiyuan.hsu
** [BORA00002222] MT6630 unified MAC RXM
** extend WISR from 2 to 4.
**
** 03 12 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Use Ratecode to set TX rate
**
** 03 05 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** Unify TX Q index definition
**
** 02 25 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** 1. Add AMPDU aggregate count MIB conuter HAL
**
** 02 25 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** .
**
** 02 22 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** Update BA related HAL.
**
** 02 19 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [mt6630] follow indirect reference method to add PSE related ROM API
** for RAM code reference.
**
** 02 14 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** 1. Remvoe emulation related code from hal_rom.c and wrapper it in emulrxhal.c
** 2. Fixed RX emul on test case 4
**
** 02 05 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Fixed build error
**
** 02 05 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Update TX emulation code
**
** 01 28 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** update ar emulaiton code
**
** 01 23 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .1. Sync. RX emulation codes
**  2. Sync. RMAC/DMA/TOP config/RCPI/MIB conter CR definition and addresses
**  3. Fixed ld file in bss section size calculation on always on region.
**  4. Enable UART port debug
**
** 01 21 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Enable CFG_TEST_TX_EMULATION flag
**
** 12 12 2012 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** .
**
** 11 22 2012 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Update RMAC emulation code
**
** 11 12 2012 eason.tsai
** [BORA00002212] [MT6630 Wi-Fi][Rom]develop
** convert tab to space
**
** 11 05 2012 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** .
**
** 10 29 2012 eason.tsai
** [BORA00002212] [MT6630 Wi-Fi][Rom]develop
** auto rate hal code
**
** 10 26 2012 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Update HAL APIs and emulation code
**
** 10 23 2012 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Avoid duplicated definition or type in ROM/RAM header files
**
*/

#ifndef _HAL_ROM_H
#define _HAL_ROM_H

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
#if (CFG_HW_SMESH_SUPPORT == 1)
#define MAX_NUM_GROUP_ADDR                      48
#define MAX_NUM_NORMAL_MUAR_ADDR                32
#define MAX_NUM_SMESH_EN                        8
#else
#define MAX_NUM_GROUP_ADDR                      32
#define MAX_NUM_NORMAL_MUAR_ADDR                MAX_NUM_GROUP_ADDR
#endif

#define TX_DESC_LIFE_TIME_NO_LIMIT                  0

/*
 * Unit of life time calculation of TXD
 */
#ifndef POWER_OF_2
#define POWER_OF_2(n)                           (1 << (n))
#endif
#define TX_DESC_LIFE_TIME_UNIT_IN_POWER_OF_2        5
#define TX_DESC_LIFE_TIME_UNIT                      POWER_OF_2(TX_DESC_LIFE_TIME_UNIT_IN_POWER_OF_2)

#if (CFG_RX_LOG_TSF_ENABLE == 1)
#define RXM_LOG_TSF_CNT                         1000
#endif

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
#define HAL_STATUS_SUCCESS                     ((HAL_STATUS) 0x00000000L)
#define HAL_STATUS_INVALID_PARMS               ((HAL_STATUS) 0x00000001L)
#define HAL_STATUS_FAILURE                      ((HAL_STATUS) 0x00000002L)
#define HAL_STATUS_TIMEOUT                      ((HAL_STATUS) 0x00000004L)
#define HAL_STATUS_ABORT                      ((HAL_STATUS) 0x00000008L)
#define HAL_STATUS_BIP_ERROR                    ((HAL_STATUS) 0x00000010L)

typedef enum _ENUM_HW_RX_QUE_T {
    RX_DATA_Q = 0,
    RX_VECTOR_Q,
    HW_RX_QUE_NUM
} ENUM_HW_RX_QUE_T;


typedef enum _ENUM_HW_BSSID_T {
    BSSID_0 = 0,
    BSSID_1,
    BSSID_2,
    BSSID_3,
    BSSID_NUM
} ENUM_HW_BSSID_T;

typedef enum _ENUM_HW_EXT_BSSID_T {
    EXT_BSSID_0 = 0x10,
    EXT_BSSID_1,
    EXT_BSSID_2,
    EXT_BSSID_3,
    EXT_BSSID_4,
    EXT_BSSID_5,
    EXT_BSSID_6,
    EXT_BSSID_7,
    EXT_BSSID_8,
    EXT_BSSID_9,
    EXT_BSSID_10,
    EXT_BSSID_11,
    EXT_BSSID_12,
    EXT_BSSID_13,
    EXT_BSSID_14,
    EXT_BSSID_15,
    EXT_BSSID_END
} ENUM_HW_EXT_BSSID_T;


typedef enum _ENUM_RFB_BSSID_INDEX_T {
    BSSID_INDEX_BSSID0,
    BSSID_INDEX_BSSID1,
    BSSID_INDEX_BSSID2,
    BSSID_INDEX_BSSID3,
    BSSID_INDEX_UNMATCHED = 0xF
} ENUM_RFB_BSSID_INDEX_T;

typedef enum _ENUM_RCPI_MODULATION_T {
    RCPI_MOD_CCK = 0,
    RCPI_MOD_OFDM,
    RCPI_MOD_NUM
} ENUM_RCPI_MODULATION_T;

typedef enum _ENUM_BA_RESET_SEL_T {
    MAC_ADDR_TID_MATCH = 0,
    MAC_ADDR_MATCH,
    ALWAYS_MATCH,
    MATCH_NUM
} ENUM_BA_RESET_SEL_T;


typedef enum _ENUM_HW_MAC_ADDR_T {
    MAC_ADDR_0 = 0,
    MAC_ADDR_1,
    MAC_ADDR_2,
    MAC_ADDR_3,
    MAC_ADDR_4,
    MAC_ADDR_NUM
} ENUM_HW_MAC_ADDR_T;


typedef enum _ENUM_MUAR_MODE_SEL_T {
    MUAR_MODE_NORMAL = 0,
    MUAR_MODE_REPEATER,
    MUAR_MODE_HASH,
    MUAR_MODE_NUM
} ENUM_MUAR_MODE_SEL_T;

typedef enum _ENUM_LG_BASIC_RATE_BMAP_T {
    BASIC_RATE_1M = BIT(0),
    BASIC_RATE_2M = BIT(1),
    BASIC_RATE_5_5M = BIT(2),
    BASIC_RATE_11M = BIT(3),
    BASIC_RATE_6M = BIT(4),
    BASIC_RATE_9M = BIT(5),
    BASIC_RATE_12M = BIT(6),
    BASIC_RATE_18M = BIT(7),
    BASIC_RATE_24M = BIT(8),
    BASIC_RATE_36M = BIT(9),
    BASIC_RATE_48M = BIT(10),
    BASIC_RATE_54M = BIT(11)
} ENUM_LG_BASIC_RATE_BMAP_T , *P_ENUM_LG_BASIC_RATE_BMAP_T ;

typedef enum _ENUM_HT_BASIC_RATE_BMAP_T {
    BASIC_RATE_HT_MCS32 = BIT(0),
    BASIC_RATE_HT_MCS0 = BIT(1),
    BASIC_RATE_HT_MCS1 = BIT(2),
    BASIC_RATE_HT_MCS2 = BIT(3),
    BASIC_RATE_HT_MCS3 = BIT(4),
    BASIC_RATE_HT_MCS4 = BIT(5),
    BASIC_RATE_HT_MCS5 = BIT(6),
    BASIC_RATE_HT_MCS6 = BIT(7),
    BASIC_RATE_HT_MCS7 = BIT(8),
} ENUM_HT_BASIC_RATE_BMAP_T , *P_ENUM_HT_BASIC_RATE_BMAP_T ;


typedef enum _ENUM_MUAR_ADDR_MODE_T {
    MUAR_ADDR_MODE_NORMAL = 0,
    MUAR_ADDR_MODE_HASH_BSSID1,
    MUAR_ADDR_MODE_HASH_BSSID2,
    MUAR_ADDR_MODE_HASH_BSSID3,
    MUAR_ADDR_MODE_NUM
} ENUM_MUAR_ADDR_MODE_T;

enum ENUM_RX_FILTER_T {
    RXFILTER_DROP_STBC_BCN_BC_MC,
    RXFILTER_DROP_FCS_ERR,
    RXFILTER_PROMISCUOUS_MODE,
    RXFILTER_DROP_VER_NOT_0,
    RXFILTER_DROP_PROBE_REQ,
    RXFILTER_DROP_MC_FRAME,
    RXFILTER_DROP_BC_FRAME,
    RXFILTER_DROP_NOT_IN_MC_TABLE,
    RXFILTER_DROP_ADDR3_OWN_MAC,
    RXFILTER_DROP_DIFF_BSSID_A3,
    RXFILTER_DROP_DIFF_BSSID_A2,
    RXFILTER_DROP_DIFF_BSSID_BCN,
    RXFILTER_RM_FRAME_REPORT_EN,    //Bit12
    RXFILTER_DROP_CTRL_RSV,
    RXFILTER_DROP_CTS,
    RXFILTER_DROP_RTS,
    RXFILTER_DROP_DUPLICATE,
    RXFILTER_DROP_NOT_MY_BSSID,
    RXFILTER_DROP_NOT_UC2ME,  //Bit 18
    RXFILTER_DROP_DIFF_BSSID_BTIM,
    RXFILTER_NUM
};

/* (CFG_SUPPORT_SMART_CONNECTION == 1) */
typedef enum _ENUM_RX_FILTER_EXT_T {
    RXFILTER_EXT_DROP_NOT_IN_MC_TABLE_BSSID1 = 0,
    RXFILTER_EXT_DROP_NOT_IN_MC_TABLE_BSSID2,
    RXFILTER_EXT_DROP_NOT_IN_MC_TABLE_BSSID3,
    RXFILTER_EXT_DROP_NOT_IN_MC_TABLE_BSSIDX,
    RXFILTER_EXT_SMART_CONNECTION_EN         = 18
} ENUM_RX_FILTER_EXT_T ;

typedef enum _ENUM_DUP_RFB_MODE_T {
    DUP_RFB_MODE_DISABLE = 0,
    DUP_RFB_MODE_ALWAYS = 1,
    DUP_RFB_MODE_PM_SET_OR_CHANGE = 2,
    DUP_RFB_MODE_NUM
} ENUM_DUP_RFB_MODE_T;

typedef enum _ENUM_RX2M_QIDX_T {
    RX2M_QIDX_0 = 0,
    RX2M_QIDX_1 = 1,
    RX2M_QIDX_2 = 2,
    RX2M_QIDX_3 = 3,
    RX2M_QIDX_NUM
} ENUM_RX2M_QIDX_T;

typedef enum _ENUM_RX2H_QIDX_T {
    RX2H_QIDX_0 = 0,
    RX2H_QIDX_1 = 1,
    RX2H_QIDX_NUM
} ENUM_RX2H_QIDX_T;


enum ENUM_RX_PKT_TYPE_2M_T {
    RX_PKT_TYPE_2M_N_NULL = BIT(0),
    RX_PKT_TYPE_2M_NULL = BIT(1),
    RX_PKT_TYPE_2M_MGT = BIT(2),
    RX_PKT_TYPE_2M_CTRL_N_BAR = BIT(3),
    RX_PKT_TYPE_2M_CTRL_BAR = BIT(4),
    RX_PKT_TYPE_2M_CLS = BITS(5,14),
    RX_PKT_TYPE_2M_WOL = BIT(15),
    RX_PKT_TYPE_2M_EAPOL = BIT(16),
    RX_PKT_TYPE_2M_ARP_NS = BIT(17),
    RX_PKT_TYPE_2M_TDLS = BIT(18),
    RX_PKT_TYPE_2M_MGC = BIT(19),
    RX_PKT_TYPE_2M_BYPASS = BIT(20)
};

typedef enum _ENUM_HW_ETBL_T {
    ETBL_0 = 0,
    ETBL_1,
    ETBL_2,
    ETBL_3,
    ETBL_4,
    ETBL_5,
    ETBL_6,
    ETBL_7,
    ETBL_NUM
} ENUM_HW_ETBL_T;

typedef enum _ENUM_VLAN_TCI_T {
    BSSID00_TCI = 0,
    BSSID01_TCI,
    BSSID02_TCI,
    BSSID03_TCI,
    BSSID11_TCI,
    BSSID12_TCI,
    BSSID13_TCI,
    BSSID14_TCI,
    BSSID15_TCI,
    BSSID16_TCI,
    BSSID17_TCI,
    BSSID18_TCI,
    BSSID19_TCI,
    BSSID1A_TCI,
    BSSID1B_TCI,
    BSSID1C_TCI,
    BSSID1D_TCI,
    BSSID1F_TCI,
    VLAN_TCI_NUM
}ENUM_VLAN_TCI_T;


typedef enum _ENUM_MUAR_INDEX_T {
    MUAR_INDEX_OWN_MAC_ADDR_0 = 0,
    MUAR_INDEX_OWN_MAC_ADDR_1,
    MUAR_INDEX_OWN_MAC_ADDR_2,
    MUAR_INDEX_OWN_MAC_ADDR_3,
    MUAR_INDEX_OWN_MAC_ADDR_4,
    MUAR_INDEX_OWN_MAC_ADDR_BC_MC = 0xE,
    MUAR_INDEX_UNMATCHED = 0xF,
    MUAR_INDEX_OWN_MAC_ADDR_11 = 0x11,
    MUAR_INDEX_OWN_MAC_ADDR_12,
    MUAR_INDEX_OWN_MAC_ADDR_13,
    MUAR_INDEX_OWN_MAC_ADDR_14,
    MUAR_INDEX_OWN_MAC_ADDR_15,
    MUAR_INDEX_OWN_MAC_ADDR_16,
    MUAR_INDEX_OWN_MAC_ADDR_17,
    MUAR_INDEX_OWN_MAC_ADDR_18,
    MUAR_INDEX_OWN_MAC_ADDR_19,
    MUAR_INDEX_OWN_MAC_ADDR_1A,
    MUAR_INDEX_OWN_MAC_ADDR_1B,
    MUAR_INDEX_OWN_MAC_ADDR_1C,
    MUAR_INDEX_OWN_MAC_ADDR_1D,
    MUAR_INDEX_OWN_MAC_ADDR_1E,
    MUAR_INDEX_OWN_MAC_ADDR_1F,
    MUAR_INDEX_OWN_MAC_ADDR_20,
    MUAR_INDEX_OWN_MAC_ADDR_21,
    MUAR_INDEX_OWN_MAC_ADDR_22,
    MUAR_INDEX_OWN_MAC_ADDR_23,
    MUAR_INDEX_OWN_MAC_ADDR_24,
    MUAR_INDEX_OWN_MAC_ADDR_25,
    MUAR_INDEX_OWN_MAC_ADDR_26,
    MUAR_INDEX_OWN_MAC_ADDR_27,
    MUAR_INDEX_OWN_MAC_ADDR_28,
    MUAR_INDEX_OWN_MAC_ADDR_29,
    MUAR_INDEX_OWN_MAC_ADDR_2A,
    MUAR_INDEX_OWN_MAC_ADDR_2B,
    MUAR_INDEX_OWN_MAC_ADDR_2C,
    MUAR_INDEX_OWN_MAC_ADDR_2D,
    MUAR_INDEX_OWN_MAC_ADDR_2E,
    MUAR_INDEX_OWN_MAC_ADDR_2F
} ENUM_MUAR_INDEX_T;


/*
 * Posh 20131110, Re-map due to HW change
 */
enum ENUM_MIB_COUNTER_T
{
    MIB_CNT_RX_FCS_ERR = 0,
    MIB_CNT_RX_FIFO_OVERFLOW,
    MIB_CNT_RX_MPDU,
    MIB_CNT_CHANNEL_IDLE,
    MIB_CNT_VEC_DROP,
    MIB_CNT_DELIMITER_FAIL,
    MIB_CNT_VEC_MISMATCH,
    MIB_CNT_MDRDY,
    MIB_CNT_PF_DROP,
    MIB_CNT_LEN_MISMATCH,
    MIB_CNT_AMDPU_RX_COUNT,
    MIB_CNT_P_CCA_TIME = 11,
    MIB_CNT_S_CCA_TIME,
    MIB_CNT_CCA_NAV_TX_TIME = 14,
    MIB_CNT_BCN_TX = 17,
    MIB_CNT_ARB_RWP_NEED,
    MIB_CNT_ARB_RWP_FAIL,
    MIB_CNT_TX_BW,
    MIB_CNT_TX_AGG_RANGE,
    MIB_CNT_AMPDU_TX_COUNT,
    MIB_CNT_PHY_MIB_COUNTER0,
    MIB_CNT_PHY_MIB_COUNTER1,

    MIB_CNT_RX_CCK_MDRDY_TIME = 25,
    MIB_CNT_P_ED_TIME = 28,
    MIB_CNT_RX_TOTAL_BYTE,
    MIB_CNT_RX_VALID_AMPDU_SF,
    MIB_CNT_RX_VALID_BYTE,

    /* PerBSS MIB Counter*/
    MIB_CNT_BSS0_RTS_TX_CNT,
    MIB_CNT_BSS0_RTS_RETRY,
    MIB_CNT_BSS0_BA_MISS,
    MIB_CNT_BSS0_ACK_FAIL,
    MIB_CNT_BSS0_FRAME_RETRY,
    MIB_CNT_BSS0_FRAME_RETRY_2,
    MIB_CNT_BSS0_FRAME_RETRY_3,     //Posh 20131208, Add MIB retry 3 count

    MIB_CNT_BSS1_RTS_TX_CNT,
    MIB_CNT_BSS1_RTS_RETRY,
    MIB_CNT_BSS1_BA_MISS,
    MIB_CNT_BSS1_ACK_FAIL,
    MIB_CNT_BSS1_FRAME_RETRY,
    MIB_CNT_BSS1_FRAME_RETRY_2,
    MIB_CNT_BSS1_FRAME_RETRY_3,     //Posh 20131208, Add MIB retry 3 count

    MIB_CNT_BSS2_RTS_TX_CNT,
    MIB_CNT_BSS2_RTS_RETRY,
    MIB_CNT_BSS2_BA_MISS,
    MIB_CNT_BSS2_ACK_FAIL,
    MIB_CNT_BSS2_FRAME_RETRY,
    MIB_CNT_BSS2_FRAME_RETRY_2,
    MIB_CNT_BSS2_FRAME_RETRY_3,     //Posh 20131208, Add MIB retry 3 count

    MIB_CNT_BSS3_RTS_TX_CNT,
    MIB_CNT_BSS3_RTS_RETRY,
    MIB_CNT_BSS3_BA_MISS,
    MIB_CNT_BSS3_ACK_FAIL,
    MIB_CNT_BSS3_FRAME_RETRY,
    MIB_CNT_BSS3_FRAME_RETRY_2,
    MIB_CNT_BSS3_FRAME_RETRY_3,     //Posh 20131208, Add MIB retry 3 count

    MIB_CNT_RX_OFDM_LG_MIXED_MDRDY_TIME = 64,
    MIB_CNT_RX_OFDM_GREEN_MDRDY_TIME
};


typedef struct _HW_TX_AMPDU_METRICS_T {
    UINT_32 u4TxSfCnt;
    UINT_32 u4TxAckSfCnt;
    UINT_16 u2TxAmpduCnt;
    UINT_16 u2TxRspBaCnt;
    UINT_16 u2TxEarlyStopCnt;
    UINT_16 u2TxRange1AmpduCnt;
    UINT_16 u2TxRange2AmpduCnt;
    UINT_16 u2TxRange3AmpduCnt;
    UINT_16 u2TxRange4AmpduCnt;
} HW_TX_AMPDU_METRICS_T, *P_HW_TX_AMPDU_METRICS_T;

typedef struct _HW_MIB_COUNTER_T  {
    UINT_32 u4RxFcsErrCnt;
    UINT_32 u4RxFifoFullCnt;
    UINT_32 u4RxMpduCnt;
    UINT_32 u4RxAMPDUCnt;
    UINT_32 u4RxTotalByte;
    UINT_32 u4RxValidAMPDUSF;
    UINT_32 u4RxValidByte;
    UINT_32 u4ChannelIdleCnt;
    UINT_32 u4RxVectorDropCnt;
    UINT_32 u4DelimiterFailedCnt;
    UINT_32 u4RxVectorMismatchCnt;
    UINT_32 u4MdrdyCnt;
    UINT_32 u4CCKMdrdyCnt;
    UINT_32 u4OFDMLGMixMdrdy;
    UINT_32 u4OFDMGreenMdrdy;
    UINT_32 u4PFDropCnt;
    UINT_32 u4RxLenMismatchCnt;
    UINT_32 u4PCcaTime;
    UINT_32 u4SCcaTime;
    UINT_32 u4CcaNavTx;
    UINT_32 u4PEDTime;
    UINT_32 u4BeaconTxCnt;
    UINT_32 au4BaMissedCnt[BSSID_NUM];
    UINT_32 au4RtsTxCnt[BSSID_NUM];
    UINT_32 au4FrameRetryCnt[BSSID_NUM];
    UINT_32 au4FrameRetry2Cnt[BSSID_NUM];
    UINT_32 au4RtsRetryCnt[BSSID_NUM];
    UINT_32 au4AckFailedCnt[BSSID_NUM];
}HW_MIB_COUNTER_T, *P_HW_MIB_COUNTER_T;

typedef enum _ENUM_RF_BANDWIDTH_T {
    RF_BW_20 = 0,
    RF_BW_40,
    RF_BW_80,
    RF_BW_160
} ENUM_RF_BANDWIDTH_T, *P_ENUM_RF_BANDWIDTH_T;

typedef enum _ENUM_MAC_GI_INFO_T {
    MAC_GI_NORMAL = 0,
    MAC_GI_SHORT
} ENUM_MAC_GI_INFO_T, *P_ENUM_MAC_GI_INFO_T;

typedef enum _ENUM_MAC_BANDWIDTH_T {
    MAC_BW_20 = 0,
    MAC_BW_40,
    MAC_BW_80,
    MAC_BW_160
} ENUM_MAC_BANDWIDTH_T, *P_ENUM_MAC_BANDWIDTH_T;


typedef enum _ENUM_PCO_PHASE_T {
    PCO_PHASE_20 = 0,
    PCO_PHASE_40
} ENUM_PCO_PHASE_T, *P_ENUM_PCO_PHASE_T;


typedef enum _ENUM_LQ_SOURCE_T {
    LQ_SOURCE_ALL_UC = 1,
    LQ_SOURCE_ALL_BEACON,
    LQ_SOURCE_ALL_BEACON_AND_UC,
    LQ_SOURCE_ALL_FRAME
} ENUM_LQ_SOURCE_T, *P_ENUM_LQ_SOURCE_T;

typedef enum _ENUM_LQ_PARMS_T {
    LQ_PARMS_1_OF_16 = 0,
    LQ_PARMS_1_OF_8,
    LQ_PARMS_1_OF_4
} ENUM_LQ_PARMS_T , *P_ENUM_LQ_PARMS_T ;

typedef enum _ENUM_AR_PARMS_T {
    AR_PARMS_1_OF_16 = 0,
    AR_PARMS_1_OF_8,
    AR_PARMS_1_OF_4,
    AR_PARMS_1_OF_1
} ENUM_AR_PARMS_T , *P_ENUM_AR_PARMS_T ;

typedef enum _ENUM_THERMAL_AR_PARMS_T {
    THERMAL_AR_PARMS_1_OF_32 = 0,
    THERMAL_AR_PARMS_1_OF_16,
    THERMAL_AR_PARMS_1_OF_4,
    THERMAL_AR_PARMS_1_OF_1
} ENUM_THERMAL_AR_PARMS_T , *P_ENUM_THERMAL_AR_PARMS_T ;

typedef enum _ENUM_LQ_BUF_SIZE_T {
    LQ_BUF_SIZE_8 = 0,
    LQ_BUF_SIZE_16,
    LQ_BUF_SIZE_32,
    LQ_BUF_SIZE_255
} ENUM_LQ_BUF_SIZE_T , *P_ENUM_LQ_BUF_SIZE_T ;

typedef enum _ENUM_LQ_CRC_BUF_SIZE_T {
    LQ_CRC_BUF_ZISE_8 = 0,
    LQ_CRC_BUF_ZISE_16,
    LQ_CRC_BUF_ZISE_32,
    LQ_CRC_BUF_ZISE_255
} ENUM_LQ_CRC_BUF_SIZE_T , *P_ENUM_LQ_CRC_BUF_SIZE_T ;

typedef enum _ENUM_EXT_CH_POS_T {
    EXT_CH_POS_ABOVE = 1,
    EXT_CH_POS_BELOW = 3
} ENUM_EXT_CH_POS_T, *P_ENUM_EXT_CH_POS_T;


typedef struct _HAL_RX_CTRL_T  {
    QUE_T                   rMacRxFreeList;
    QUE_T                   rMacRxRfbList;
    QUE_T                   rRxVectorFreeList;
    QUE_T                   rRxVectorList;
    BOOL                    fgHwDeAmpduEn;
    BOOL                    fgHwDeAmsduEn;
    BOOL                    fgPromiscuousMode;
    UINT_8                  ucChanFreq;
}HAL_RX_CTRL_T, *P_HAL_RX_CTRL_T;


typedef enum _ENUM_HW_WIFI_CLOCK_RATIO_T {
    HW_WIFI_CLOCK_RATIO_FULL = 0,
    HW_WIFI_CLOCK_RATIO_HALF,
    HW_WIFI_CLOCK_RATIO_QUARTER,
    HW_WIFI_CLOCK_RATIO_NUM
} ENUM_HW_WIFI_CLOCK_RATIO_T;

typedef enum _ENUM_HW_TX_PIDX_T {
    PORT_LMAC = 0,
    PORT_MCU = 1,
    PORT_NUM
} ENUM_HW_TX_PIDX_T;

typedef enum _ENUM_HW_TX_QUE_T {
    TX_Q_AC0 = 0,
    TX_Q_AC1,
    TX_Q_AC2,
    TX_Q_AC3,
    TX_Q_AC4,
    TX_Q_AC5,
    TX_Q_AC6,
    TX_Q_BCN,
    TX_Q_BMC,
    TX_Q_AC10,
    TX_Q_AC11,
    TX_Q_AC12,
    TX_Q_AC13,
    TX_Q_AC14,
    HW_TX_QUE_NUM
} ENUM_HW_TX_QUE_T;

typedef enum _ENUM_HW_OP_MODE_T {
    HW_OP_MODE_STA = 0,
    HW_OP_MODE_AP,
    HW_OP_MODE_ADHOC,
    HW_OP_MODE_NUM
} ENUM_HW_OP_MODE_T;

typedef enum _ENUM_IFS_TYPE_T {
    IFS_SLOT_TIME = 0,
    IFS_SIFS_TIME,
    IFS_RIFS_TIME,
    IFS_EIFS_TIME,
    IFS_PIFS_TIME,
    IFS_AIFS_TIME,
    IFS_DIFS_TIME,
    IFS_DIFS_SLOT_TIME,
    IFS_IFS_NUM
} ENUM_IFS_TYPE_T;

typedef enum _ENUM_AUTO_RATE_NUM_T {
    AR_RATE_1 = 0,
    AR_RATE_2,
    AR_RATE_3,
    AR_RATE_4,
    AR_RATE_5,
    AR_RATE_6,
    AR_RATE_7,
    AR_RATE_8,
    AR_RATE_NUM
} ENUM_AUTO_RATE_NUM_T;


typedef struct _HAL_MEASUREMENT_QUIET_CTRL_T {
    UINT_32 u4CounterItem0;
    UINT_32 u4CounterItem1;
    UINT_32 u4CounterItem2;
    UINT_32 u4CounterItem3;
} HAL_MEASUREMENT_QUIET_CTRL_T, *P_HAL_MEASUREMENT_QUIET_CTRL_T;


typedef struct _TX_AC_PARAM_AIFS_CW_T {
    UINT_16     u2CWmin;            /*!< CWmin */
    UINT_16     u2CWmax;            /*!< CWmax */
    UINT_16     u2TxopLimit;        /*!< TXOP limit */
    UINT_16     u2Aifsn;            /*!< AIFSN */
    UINT_8       ucGuradTime;   /*!< GuardTime for STOP/FLUSH. */
} TX_AC_PARAM_AIFS_CW_T, *P_TX_AC_PARAM_AIFS_CW_T;

typedef struct _HAL_WTBL_PEER_CAP_T  {
    UINT_8                  ucTxPowerOffset;
    UINT_8                  ucAntnnaPriority;
    UINT_8                  ucCounterBWSelector;
    UINT_8                  ucChangeBWAfterRateN;
    UINT_8                  ucFrequencyCapability;
    BOOL                    fgSpatialExtension;
    BOOL                    fgG2;
    BOOL                    fgG4;
    BOOL                    fgG8;
    BOOL                    fgG16;

    /*
     * Posh 20131112, Merge code from MT6630 E2
     */
    UINT_8                  ucMMSS;
    UINT_8                  ucAmpduFactor;
}HAL_WTBL_PEER_CAP_T,*P_HAL_WTBL_PEER_CAP_T;

typedef struct _HAL_WTBL_PEER_RATE_INFO_T {
    UINT_8                  ucCounterMPDUFail;
    UINT_8                  ucCounterMPDUTx;
    UINT_8                  ucRateIdx;
    UINT_8                  ucReserved[1];
    UINT_16                 au2RateCode[AR_RATE_NUM];
}HAL_WTBL_PEER_RATE_INFO_T, *P_HAL_WTBL_PEER_RATE_INFO_T;

typedef struct _HAL_WTBL2_TX_COUNTER_ALL_T {
    UINT_16     u2Rate1TxCount;
    UINT_16     u2Rate1FailCount;
    UINT_8      ucRate2TxCount;
    UINT_8      ucRate3TxCount;
    UINT_8      ucRate4TxCount;
    UINT_8      ucRate5TxCount;
    UINT_16     u2CurrentBWTxCount;
    UINT_16     u2CurrentBWFailCount;
    UINT_16     u2OtherBWTxCount;
    UINT_16     u2OtherBWFailCount;
}HAL_WTBL2_TX_COUNTER_ALL_T,*P_HAL_WTBL2_TX_COUNTER_ALL_T;

typedef struct _HAL_WTBL2_RX_COUNTER_ALL_T {
    UINT_8     ucRxRcpi0;
    UINT_8     ucRxRcpi1;
    UINT_8     ucRxRcpi2;
    UINT_8     ucReserve[1];
    UINT_8     ucRxCC0;
    UINT_8     ucRxCC1;
    UINT_8     ucRxCC2;
    UINT_8     ucCeRmsd;
}HAL_WTBL2_RX_COUNTER_ALL_T,*P_HAL_WTBL2_RX_COUNTER_ALL_T;


typedef struct _HAL_WTBL_SEC_CONFIG_T  {
    BOOL                   fgRCA2;
    BOOL                   fgRV;
    BOOL                   fgIKV;
    BOOL                   fgRKV;
    BOOL                   fgRCID;
    BOOL                   fgRCA1;
    BOOL                   fgEvenPN;
    UINT_8                 ucKeyID;
    UINT_8                 ucMUARIdx;
    UINT_8                 ucCipherSuit;
}HAL_WTBL_SEC_CONFIG_T, *P_HAL_WTBL_SEC_CONFIG_T;


typedef struct _HAL_WTBL_TX_CONFIG_T  {
    UINT_8                 aucPA[6];
    BOOL                    fgIsToDS;
    BOOL                    fgIsFromDS;
    BOOL                   fgDisRxHdrTran;
    BOOL                    fgIsPwrMgt;
    BOOL                   fgRDG;
    BOOL                   fgRTS;
    BOOL                   fgCFAck;
    BOOL                   fgRdgBA;
    BOOL                   fgSMPS;
    BOOL                   fgBAFEn;
    BOOL                   fgDynBw;
    BOOL                   fgLDPC;
    BOOL                   fgTIBF;
    BOOL                   fgTEBF;
    BOOL                   fgTxopPS;
    BOOL                   fgIsQoS;
    BOOL                   fgIsHT;
    BOOL                   fgIsVHT;
    BOOL                   fgADMEn;
    BOOL                   fgCheckPER;
    BOOL                   fgIsGID63;
    BOOL                   fgDonotUpdateIPSM;
    BOOL                   fgIPSM;
    BOOL                   fgIsPSM;
    BOOL                   fgSkipTx;
    BOOL                   fgSW;
    BOOL                   fgMesh;

#if 0   //Posh 20131112, Old design on MT6630 E1
    UINT_8                 ucMMSS;
    UINT_8                 ucAmpduFactor;
//#elif (CFG_SUPPORT_VHT == 1)
#else
    BOOL                   fgVhtLDPC;
    BOOL                   fgVhtTIBF;
    BOOL                   fgVhtTEBF;
#endif
}HAL_WTBL_TX_CONFIG_T, *P_HAL_WTBL_TX_CONFIG_T;


typedef struct _HAL_WTBL_KEY_CONFIG_T  {
    UINT_8                 aucCipherKey[16];
    UINT_8                 aucRxMicKey[8];
    UINT_8                 aucTxMicKey[8];
}HAL_WTBL_KEY_CONFIG_T, *P_HAL_WTBL_KEY_CONFIG_T;


typedef struct _HAL_WTBL_FID_EID_T  {
    UINT_16                u2Tbl2Fid;
    UINT_16                u2Tbl3Fid;
    UINT_16                u2Tbl4Fid;
    UINT_16                u2PartialAID;
    UINT_8                 ucTbl2Eid;
    UINT_8                 ucTbl3Eid;
    UINT_8                 ucTbl4Eid;
}HAL_WTBL_FID_EID_T, *P_HAL_WTBL_FID_EID_T;


#if 0 /* 6630 */
typedef struct _HAL_WTBL_TBL2_DW11_T  {
    UINT_8                ucTxPowerOffset;
    UINT_8                ucAntPri;
    UINT_8                ucCcBwSel;
    UINT_8                ucCBRN;
    UINT_8                ucFCap;
    UINT_8                ucRateNSel;
    UINT_8                ucMpduFailCnt;
    UINT_8                ucMpduTxCnt;
    UINT_8                ucRateIdx;
    BOOL                  fgSpeEn;
    BOOL                  fgSGI160;
    BOOL                  fgSGI80;
    BOOL                  fgSGI40;
    BOOL                  fgSGI20;
}HAL_WTBL_TBL2_DW11_T, *P_HAL_WTBL_TBL2_DW11_T;
#endif

typedef struct _HAL_WTBL_TBL2_DW15_T  {
    UINT_8                aucBaWinSize[8];
    UINT_8                ucBaEnBitmap;
}HAL_WTBL_TBL2_DW15_T, *P_HAL_WTBL_TBL2_DW15_T;


typedef enum _ENUM_WTBL2_RX_COUNTER_SEL_TYPE_T {
    SEL_TYPE_CHANNELCAPACITY_CNT=0,
    SEL_TYPE_NOISE_CNT,
} ENUM_WTBL2_RX_COUNTER_SEL_TYPE_T;


typedef struct _HAL_WTBL2_RATE_CONFIG_T  {
    UINT_16                u2Rate[AR_RATE_NUM];
}HAL_WTBL2_RATE_CONFIG_T, *P_HAL_WTBL2_RATE_CONFIG_T;

typedef struct _AC_ADM_INFO_T {
    UINT_32                u4ACTxTime;
    UINT_32                u4ACTxBytes;
} AC_ADM_INFO_T, *P_AC_ADM_INFO_T;


typedef struct _HAL_WTBL4_TX_ADM_INFO_T  {
    AC_ADM_INFO_T        arTxAdmInfo[4];
}HAL_WTBL4_TX_ADM_INFO_T , *P_HAL_WTBL4_TX_ADM_INFO_T ;



#if 0 /* 6630 */
typedef struct _HAL_WTBL_RATE_CONFIG_T  {
    UINT_8                 ucRate[AR_RATE_NUM];
}HAL_WTBL_RATE_CONFIG_T, *P_HAL_WTBL_RATE_CONFIG_T;
#endif

#if CFG_NEW_AUTO_RATE // NEW Auto Rate
typedef struct _HAL_RATE_TX_CNT_CONFIG_T  {
    UINT_8                 ucRateUpLimit[AR_RATE_NUM];
    UINT_8                 ucRateDownLimit[AR_RATE_NUM];
}HAL_RATE_TX_CNT_CONFIG_T, *P_HAL_RATE_TX_CNT_CONFIG_T;
#else // OLD Auto Rate
typedef struct _HAL_RATE_TX_CNT_CONFIG_T  {
    UINT_8                 ucRateCntLimit[AR_RATE_NUM];
}HAL_RATE_TX_CNT_CONFIG_T, *P_HAL_RATE_TX_CNT_CONFIG_T;
#endif


#define RCPI_MOVING_AVERAGE_ONE          0x3
#define RCPI_MOVING_AVERAGE_TWO         0x2
#define RCPI_MOVING_AVERAGE_FOUR       0x1
#define RCPI_MOVING_AVERAGE_EIGHT       0x0


typedef enum _ENUM_RESP_RCPI_LQ_TYPE_T {
    RESP_RCPI_LQ_BW20_1M = 0,
    RESP_RCPI_LQ_BW20_2M,
    RESP_RCPI_LQ_BW20_55M,
    RESP_RCPI_LQ_BW20_11M,
    RESP_RCPI_LQ_BW20_6M,
    RESP_RCPI_LQ_BW20_12M,
    RESP_RCPI_LQ_BW20_24M,
    RESP_RCPI_LQ_BW40_6M,
    RESP_RCPI_LQ_BW40_12M,
    RESP_RCPI_LQ_BW40_24M,
    RESP_RCPI_LQ_NUM
} ENUM_RESP_RCPI_LQ_TYPE_T;


typedef struct _HAL_RESP_RCPI_LQ_T {
    UINT_8      aucRespRcpiLq[RESP_RCPI_LQ_NUM];
} HAL_RESP_RCPI_LQ_T, *P_HAL_RESP_RCPI_LQ_T;


typedef enum _ENUM_RESP_RCPI_TYPE_T {
    RESP_RCPI0 = 0,
    RESP_RCPI1,
    RESP_RCPI2,
    RESP_RCPI_NUM
} ENUM_RESP_RCPI_TYPE_T;


typedef struct _HAL_RESP_RCPI_T {
    UINT_8      aucRespRcpi[RESP_RCPI_NUM];
} HAL_RESP_RCPI_T, *P_HAL_RESP_RCPI_T;

typedef enum _ENUM_FRAME_POWER_GROUP_T {
    FRAME_POWER_0 = 0,
    //FRAME_POWER_1,    //Posh 20140101, Merge code from MT6630 E2
    FRAME_POWER_NUM
} ENUM_FRAME_POWER_GROUP_T;


typedef struct _HAL_FRAME_POWER_SET_T {
    UINT_8 ucFramePowerDbm;
} HAL_FRAME_POWER_SET_T, *P_HAL_FRAME_POWER_SET_T;


typedef struct _HAL_FRAME_POWER_CONFIG_T {
    HAL_FRAME_POWER_SET_T aucFramePowerConfig[MODULATION_SYSTEM_NUM][MODULATION_TYPE_NUM];
} HAL_FRAME_POWER_CONFIG_T, *P_HAL_FRAME_POWER_CONFIG_T;

typedef struct _HAL_VHT_FRAME_POWER_CONFIG_T {
    HAL_FRAME_POWER_SET_T aucVhtFramePowerConfig[MODULATION_TYPE_VHT_NUM];
} HAL_VHT_FRAME_POWER_CONFIG_T, *P_HAL_VHT_FRAME_POWER_CONFIG_T;

#if 0 /* Move to RAM Code */
typedef enum _ENUM_MEASUREMENT_COUNTER_T {
    MEASUREMENT_DISABLE = 0,
    MEASUREMENT_COUNTER_0,
    MEASUREMENT_COUNTER_1,
    MEASUREMENT_COUNTER_2,
    MEASUREMENT_COUNTER_3,
    MEASUREMENT_COUNTER_ALWAYS_ON = 7
} ENUM_MEASUREMENT_COUNTER_T;

typedef enum _ENUM_MEASUREMENT_BYPASS_TYPE_T {
    MEASUREMENT_BYPASS_DISABLE = 0,
    MEASUREMENT_BYPASS_RXP_DISABLE,
    MEASUREMENT_BYPASS_RXP_DISABLE_OR_RW0,
    MEASUREMENT_BYPASS_RESERVED
} ENUM_MEASUREMENT_BYPASS_TYPE_T;

typedef enum _ENUM_MEASUREMENT_ITEM_T {
    MEASUREMENT_BSSRXSTS_REQ = 0,
    MEASUREMENT_RADAR_REQ,
    MEASUREMENT_CCA_REQ,
    MEASUREMENT_RPI_HISTOGRAM_REQ,
    MEASUREMENT_IPI_HISTOGRAM_REQ,
    MEASUREMENT_BSS_REQUEST_REQ,
    MEASUREMENT_CCANAVTX_REQ,
    MEASUREMENT_P_ED_REQ,
    MEASUREMENT_RX_OFDM_CCK_MDRDY_REQ,
    MEASUREMENT_ITEM_NUM
} ENUM_MEASUREMENT_ITEM_T;
#endif

typedef enum _ENUM_VOLTAGE_INTR_PERIOD_T {
    INTR_PERIOD_1_SEC = 0,
    INTR_PERIOD_500_MS,
    INTR_PERIOD_250_MS,
    INTR_PERIOD_ITEM_NUM
} ENUM_VOLTAGE_INTR_PERIOD_T;



/*********************************************************************
* 11n and PTA HAL APIs (JSWAN)
**********************************************************************/
#define     MAX_AMPDU_LIMIT             64

#define     NUM_AGG_AC_PER_SET          4
#define     NUM_WIN_SIZE_ENTRY          8
#define     NUM_BA_TID                  8


typedef enum _ENUM_AMPDU_NO_BA_RULE {
    NOBA_RETX_AMPDU = 0,
    NOBA_RETX_FIRST_MPDU
} ENUM_AMPDU_NO_BA_RULE, *P_ENUM_AMPDU_NO_BA_RULE;

typedef enum _ENUM_AMPDU_NO_BA_AR_RULE {
    NOBA_AR_ADD_ALL = 0,
    NOBA_AR_ADD_ONE
} ENUM_AMPDU_NO_BA_AR_RULE, *P_ENUM_AMPDU_NO_BA_AR_RULE;

typedef enum _ENUM_AMPDU_FACTOR_T {
    AF_8K_BYTES =0,
    AF_16K_BYTES,
    AF_32K_BYTES,
    AF_64K_BYTES,
    AF_128K_BYTES,
    AF_256K_BYTES,
    AF_512K_BYTES,
    AF_1024K_BYTES
} ENUM_AMPDU_FACTOR_T, *P_ENUM_AMPDU_FACTOR_T;

typedef enum _ENUM_AMPDU_MM_SPACE_T {
    MM_0_US =0,
    MM_1_4_US,
    MM_1_2_US,
    MM_1_US,
    MM_2_US,
    MM_4_US,
    MM_8_US,
    MM_16_US
} ENUM_AMPDU_MM_SPACE_T, *P_ENUM_AMPDU_MM_SPACE_T;

typedef struct _HAL_AMPDU_AGG_LIMIT_T {
    UINT_8      ucAcAggLimitSet1[NUM_AGG_AC_PER_SET];
    UINT_8      ucAcAggLimitSet2[NUM_AGG_AC_PER_SET];
} HAL_AMPDU_AGG_LIMIT_T, *P_HAL_AMPDU_AGG_LIMIT_T;

typedef struct _HAL_AMPDU_WIN_SIZE_LIMIT_T {
    UINT_8      ucWinSizeEntry[NUM_WIN_SIZE_ENTRY];
} HAL_AMPDU_WIN_SIZE_LIMIT_T, *P_HAL_AMPDU_WIN_SIZE_LIMIT_T;

typedef struct _HAL_BA_ENTRY_T {
    UINT_8                  ucTid;
    UINT_8                  ucWlanIdx;
    UINT_8                  ucWinSize;
    UINT_16                 u2AgingLimit;
    UINT_16                 u2StartingSeqNum;
    UINT_32                 u4BitmapLow;
    UINT_32                 u4BitmapHigh;
}HAL_BA_ENTRY_T, *P_HAL_BA_ENTRY_T;

typedef enum _ENUM_RTS_THRESHOLD_UNIT_T {
    RTS_THRESHOLD_IN_PKT_LEN,
    RTS_THRESHOLD_IN_PKT_NUM
} ENUM_RTS_THRESHOLD_UNIT_T, *P_ENUM_RTS_THRESHOLD_UNIT_T;


typedef enum _ENUM_ANT_SEL_MODE_T
{
    ANT_MODE_0 = 0,
    ANT_MODE_1,
    ANT_MODE_BT_TRX,
    ANT_MODE_NOT_BT_TRX
} ENUM_ANT_SEL_MODE_T, *P_ENUM_ANT_SEL_MODE_T;

typedef enum _ENUM_SMPS_MODE_T
{
    SMPS_DISABLE = 0,
    SMPS_STATIC_MODE,
    SMPS_DYNAMIC_MODE,
    SMPS_DISABLE_MODE
} ENUM_SMPS_MODE_T, *P_ENUM_SMPS_MODE_T;

typedef enum _ENUM_SMPS_ANT_T
{
    SMPS_ANT_0 = 1,
    SMPS_ANT_1 = 2,
    SMPS_ANT_2 = 4,
} ENUM_SMPS_ANT_T, *P_ENUM_SMPS_ANT_T;



typedef struct _PTA_ARB_MODE_T
{
    BOOL fg_BtTx_WfTx_Ok;
    BOOL fg_BtRx_WfRx_Ok;
    BOOL fg_BtRx_WfTx_Ok;
    BOOL fg_BtTx_WfRx_Ok;

    BOOL fg_WfTx_ExtTx_Ok;
    BOOL fg_WfRx_ExtRx_Ok;
    BOOL fg_WfRx_ExtTx_Ok;
    BOOL fg_WfTx_ExtRx_Ok;

    BOOL fg_ExtTx_BtTx_Ok;
    BOOL fg_ExtRx_BtRx_Ok;
    BOOL fg_ExtRx_BtTx_Ok;
    BOOL fg_ExtTx_BtRx_Ok;
} PTA_ARB_MODE_T, *P_PTA_ARB_MODE_T;

typedef struct _PTA_EXT_ARB_CON_T
{
    UINT_8 ucExtPtaMode;
    UINT_8 ucExtPriCtrl;
    UINT_8 ucExtTrCtrl;
} PTA_EXT_ARB_CON_T, *P_PTA_EXT_ARB_CON_T;


typedef enum _ENUM_PTA_COEX_DEVICE{
    COEX_BT = 0,
    COEX_WIMAX
} ENUM_PTA_COEX_DEVICE, *P_ENUM_PTA_COEX_DEVICE;
typedef enum _ENUM_PTA_PRIORITY_T{
    PTA_PRIORITY_0 = 0,
    PTA_PRIORITY_1,
    PTA_PRIORITY_2,
    PTA_PRIORITY_3,
    PTA_PRIORITY_4,
    PTA_PRIORITY_5,
    PTA_PRIORITY_6,
    PTA_PRIORITY_7,
    PTA_PRIORITY_8,
    PTA_PRIORITY_9,
    PTA_PRIORITY_10,
    PTA_PRIORITY_11,
    PTA_PRIORITY_12,
    PTA_PRIORITY_13,
    PTA_PRIORITY_14,
    PTA_PRIORITY_15
} ENUM_PTA_PRIORITY_T, *P_ENUM_PTA_PRIORITY_T;

typedef enum _ENUM_PTA_RW_MODE_T
{
    RW_MODE_DISABLE = 0x0,
    RW_MODE_CH_RSV_BSS0_BSS1_BY_0 = (BIT(0)),
    RW_MODE_CH_RSV_BSS0 = (BIT(1)),
    RW_MODE_CH_RSV_BSS1 = (BIT(1)+BIT(0)),
    RW_MODE_CH_RSV_BSS0_BSS1_BY_1 = (BIT(2)),
    RW_MODE_FAST_PS_BSS0 = (BIT(2)+BIT(0)),
    RW_MODE_FAST_PS_BSS1 = (BIT(2)+BIT(1)),
    RW_MODE_FAST_PS_BSS0_BSS1 = (BIT(2)+BIT(1)+BIT(0))
} ENUM_PTA_RW_MODE_T, *P_ENUM_PTA_RW_MODE_T;

typedef enum _ENUM_PTA_CEASE_RWP_MODE_T
{
    CEASE_RWP_DISABLE =0x0,
    CEASE_RWP_BSS0 = (BIT(0)),
    CEASE_RWP_BSS1 = (BIT(1)),
    CEASE_RWP_BSS0_BSS1 = (BIT(0)+BIT(1))
} ENUM_PTA_CEASE_RWP_MODE_T, *P_ENUM_PTA_CEASE_RWP_MODE_T;

typedef struct  _HAL_PTA_PRI_TAG_STRUCT_T{
    ENUM_PTA_PRIORITY_T          aeTxQTxTag[HW_TX_QUE_NUM];
    ENUM_PTA_PRIORITY_T          aeTxQRxTag[HW_TX_QUE_NUM];

    ENUM_PTA_PRIORITY_T          eTxRspTag;
    ENUM_PTA_PRIORITY_T          eTxPtaTag;

    ENUM_PTA_PRIORITY_T          eRxNswTag;
    ENUM_PTA_PRIORITY_T          eRxSpTag;
    ENUM_PTA_PRIORITY_T          eRxPtaTag;
} HAL_PTA_PRI_TAG_STRUCT_T, *P_HAL_PTA_PRI_TAG_STRUCT_T;

typedef enum _ENUM_PTA_NSW_MODE_T{
    NSW_MODE_MDRDY = 0,
    NSW_MODE_CCA_CS,
    NSW_MODE_CCA,
    NSW_MODE_RX_PE
} ENUM_PTA_NSW_MODE_T, *P_ENUM_PTA_NSW_MODE_T;

typedef enum _ENUM_BT_EXT_MODE_T
{
    BT_EXT_1_WIRE_MODE = 0x0,
    BT_EXT_2_WIRE_MODE,
    BT_EXT_3_WIRE_MODE,
    BT_EXT_4_WIRE_MODE,
    BT_EXT_1_WIRE_EXTEND_MODE,
    BT_EXT_2_WIRE_ESI_MODE,
    BT_EXT_1_WIRE_ESI_MODE,
    BT_EXT_RSVD
} ENUM_BT_EXT_MODE_T, *P_ENUM_BT_EXT_MODE_T;

typedef struct _BT_PERIODIC_PERIOD_T
{
    UINT_16 u2BtPeriod0;
    UINT_16 u2BtPeriod1;
    UINT_16 u2BtPeriod2;
    UINT_16 u2BtPeriod3;
    UINT_16 u2BtPeriod4;
    UINT_16 u2BtPeriod5;
    UINT_16 u2BtPeriod6;
    UINT_16 u2BtPeriod7;
} BT_PERIODIC_PERIOD_T, *P_BT_PERIODIC_PERIOD_T;

typedef struct _BT_WIFI_PERIOD_T
{
    UINT_16 u2BtWiFiPeriod0;
    UINT_16 u2BtWiFiPeriod1;
    UINT_16 u2BtWiFiPeriod2;
    UINT_16 u2BtWiFiPeriod3;
    UINT_16 u2BtWiFiPeriod4;
    UINT_16 u2BtWiFiPeriod5;
    UINT_16 u2BtWiFiPeriod6;
    UINT_16 u2BtWiFiPeriod7;
} BT_WIFI_PERIOD_T, *P_BT_WIFI_PERIOD_T;

typedef enum _ENUM_BIP_SUB_TYPE_T
{
    BIP_SUB_TYPE_0 =0,
    BIP_SUB_TYPE_1,
    BIP_SUB_TYPE_2,
    BIP_SUB_TYPE_3,
    BIP_SUB_TYPE_4,
    BIP_SUB_TYPE_5,
    BIP_SUB_TYPE_NUM
} ENUM_BIP_SUB_TYPE_T, *P_ENUM_BIP_SUB_TYPE_T;


typedef enum _ENUM_WTBL_TX_INFO_TYPE_T {
    TX_MPDU_CNT_INFO = 0,
    RATE1_PWL0_BW20,
    RATE1_PWL1_BW40,
    RATE1_PWL2,
    RATE2_PWL0_BW20,
    RATE2_PWL1_BW40,
    RATE2_PWL2,
    TOTAL_SGI_INFO
} ENUM_WTBL_TX_INFO_TYPE_T;

typedef enum _ENUM_WTBL2_SN_SOURCE_TYPE_T {
    TID0_AC0_SN = 0,
    TID1_AC1_SN,
    TID2_AC2_SN,
    TID3_AC3_SN,
    TID4_SN,
    TID5_SN,
    TID6_SN,
    TID7_SN,
    COMMON_SN,

    MAX_NUM_SN_SOURCE
} ENUM_WTBL2_SN_SOURCE_TYPE_T;

typedef enum _EMUM_WTBL2_CLEAR_UPDATE_TYPE_T {
    RX_COUNTER = 0,
    TX_COUNTER,
    RATE_UPDATE,
    ADM_COUNTER,
    TOTAL_COUNTER_CNT
} EMUM_WTBLE2_CLEAR_UPDATE_TYPE_T;

/*
typedef enum _EMUM_WTBL2_TX_COUNTER_THRESHOLD_T {
    TX_CNT = 0,
    FAIL_CNT,
    TOTAL_TX_THRESHOLD_CNT
} ENUM_WTBL2_TX_COUNTER_THRESHOLD_T;
*/

typedef enum _ENUM_WTBL2_CLEAR_COUNTER_TYPE_T {
    RX_CNT = 0,
    TX_CNT ,
    ADM_CNT,
    TOTAL_CLEAR_CNT
}ENUM_WTBL2_CLEAR_COUNTER_TYPE_T;

#if 0
typedef enum _ENUM_WTBL_TX_8B_INFO_TYPE_T {
    RATE1_PWL0_BW20_8B=0,
    RATE3_PWL0_BW20_8B,
    RATE1_PWL1_BW40_8B,
    RATE3_PWL1_BW40_8B,
    RATE1_PWL2_8B,
    RATE3_PWL2_8B,
    RATE2_PWL0_BW20_8B,
    RATE4_PWL0_BW20_8B,
    RATE2_PWL1_BW40_8B,
    RATE4_PWL1_BW40_8B,
    RATE2_PWL2_8B,
    RATE4_PWL2_8B,
    RATE_TOTAL_NUM
} ENUM_WTBL_TX_8B_INFO_TYPE_T;
#endif

#if 0
typedef enum _ENUM_AR_WCF {
    AR_WCF_16B=0,
    AR_WCF_8B,
    AR_WCF_NUM
} ENUM_AR_WCF_T;
#endif

#if 0
typedef enum _ENUM_AR_MODE {
    AR_MODE_POWER,
    AR_MODE_BANDWIDTH,
    AR_MODE_WXTA,
    AR_MODE_NUM
} ENUM_AR_MODE_T;
#endif

typedef enum _ENUM_DBG_LEVEL_SELECT_T {
    DBG_LEVEL_SELECT_1 = 0,
    DBG_LEVEL_SELECT_2,
    DBG_LEVEL_SELECT_NUM
} ENUM_DBG_LEVEL_SELECT_T;

typedef enum _ENUM_DBG_MODULE_SELECT_T {
    DBG_MODULE_WF_SEC = 0,
    DBG_MODULE_WF_SIF,
    DBG_MODULE_WF_TRB,
    DBG_MODULE_WF_DMA,
    DBG_MODULE_WF_ARB,
    DBG_MODULE_WF_AGG,
    DBG_MODULE_WF_RMAC,
    DBG_MODULE_WF_TMAC,
    DBG_MODULE_WF_NUM,
    DBG_MODULE_WF_NONE = 255
} ENUM_DBG_MODULE_SELECT_T;

typedef enum _ENUM_DBG_SOURCE_SELECT_T {
    DBG_SOURCE_DBG_LEVEL_1 = 0, /* for LMAC, always select this one */
    DBG_SOURCE_WF_LP,
    DBG_SOURCE_WF_WTBL,
    DBG_SOURCE_WF_CMDBT,
    DBG_SOURCE_WF_ON_SIF,
    DBG_SOURCE_WF_INT_WAKEUP,
    DBG_SOURCE_WF_AHBSMUX,
    DBG_SOURCE_WF_MIB,
    DBG_SOURCE_WF_THERM,
    DBG_SOURCE_WF_CFG,
    DBG_SOURCE_WF_NUM
} ENUM_DBG_SOURCE_SELECT_T;

typedef enum _ENUM_DBG_NUM_SELECT_T {
    DBG_NUM_SELECT_0 = 0,
    DBG_NUM_SELECT_1,
    DBG_NUM_SELECT_2,
    DBG_NUM_SELECT_3,
    DBG_NUM_SELECT_NUM
} ENUM_DBG_NUM_SELECT_T;

typedef enum _ENUM_DBG_BYTES_SEL_T {
    DBG_BYTES_SEL_0 = 0,
    DBG_BYTES_SEL_1,
    DBG_BYTES_SEL_NUM
} ENUM_DBG_BYTES_SEL_T;

typedef enum _ENUM_DBG_FLAG_BIT_T {
    DBG_FLAG_0_BIT_0 = 0,
    DBG_FLAG_0_BIT_1,
    DBG_FLAG_0_BIT_2,
    DBG_FLAG_0_BIT_3,
    DBG_FLAG_0_BIT_4,
    DBG_FLAG_0_BIT_5,
    DBG_FLAG_0_BIT_6,
    DBG_FLAG_0_BIT_7,
    DBG_FLAG_1_BIT_0,
    DBG_FLAG_1_BIT_1,
    DBG_FLAG_1_BIT_2,
    DBG_FLAG_1_BIT_3,
    DBG_FLAG_1_BIT_4,
    DBG_FLAG_1_BIT_5,
    DBG_FLAG_1_BIT_6,
    DBG_FLAG_1_BIT_7,
    DBG_FLAG_BIT_NUM
#if 0 /* Only DBG_FLAG_0 and DBG_FLAG_1 are valid */
    DBG_FLAG_2_BIT_0,
    DBG_FLAG_2_BIT_1,
    DBG_FLAG_2_BIT_2,
    DBG_FLAG_2_BIT_3,
    DBG_FLAG_2_BIT_4,
    DBG_FLAG_2_BIT_5,
    DBG_FLAG_2_BIT_6,
    DBG_FLAG_2_BIT_7,
    DBG_FLAG_3_BIT_0,
    DBG_FLAG_3_BIT_1,
    DBG_FLAG_3_BIT_2,
    DBG_FLAG_3_BIT_3,
    DBG_FLAG_3_BIT_4,
    DBG_FLAG_3_BIT_5,
    DBG_FLAG_3_BIT_6,
    DBG_FLAG_3_BIT_7,
    DBG_FLAG_BIT_NUM
#endif
} ENUM_DBG_FLAG_BIT_T;

typedef enum _ENUM_DBG_EDGE_TRIGGER_T {
    DBG_EDGE_TRIGGER_FALLING = 0,
    DBG_EDGE_TRIGGER_RAISING
} ENUM_DBG_EDGE_TRIGGER_T;

typedef enum _ENUM_CIPHER_SUIT_T
{
    CIPHER_SUIT_NONE = 0,
    CIPHER_SUIT_WEP_40,
    CIPHER_SUIT_TKIP_W_MIC,
    CIPHER_SUIT_TKIP_WO_MIC,
    CIPHER_SUIT_CCMP_W_MIC, //for DFP or 802.11w MFP
    CIPHER_SUIT_WEP_104,
    CIPHER_SUIT_BIP,
    CIPHER_SUIT_WEP_128 = 7,
    CIPHER_SUIT_WPI,
    CIPHER_SUIT_CCMP_W_MIC_FOR_CCX = 9,  //for DFP or CCX MFP
    CIPHER_SUIT_CCMP_256,
    CIPHER_SUIT_GCMP_128,
    CIPHER_SUIT_GCMP_256
} ENUM_CIPHER_SUIT_T256, *P_ENUM_CIPHER_SUIT_T;


typedef enum _ENUM_PROCESS_DELAY_T {
    ENUM_CCK_20M_PDLY = 0,
    ENUM_OFDM_20M_PDLY,
    ENUM_OFDM_40M_PDLY,
    ENUM_OFDM_80M_PDLY,
    ENUM_OFDM_160M_PDLY,
    ENUM_PROCESS_DELAY_NUM
} ENUM_PROCESS_DELAY_T, *P_ENUM_PROCESS_DELAY_T;


typedef enum _ENUM_TRAPPER_T {
    ENUM_TRAPPER_0,
    ENUM_TRAPPER_1,
    ENUM_TRAPPER_2,
    ENUM_TRAPPER_3,
    ENUM_TRAPPER_NUM
} ENUM_TRAPPER_T, *P_ENUM_TRAPPER_T;


typedef enum _ENUM_BACKOFF_POLICY_T {
    BACKOFF_POLICY_RELOAD_TX_BUSY,
    BACKOFF_POLICY_RELOAD_AIR_END,
    BACKOFF_POLICY_NUM
} ENUM_BACKOFF_POLICY_T, *P_ENUM_BACKOFF_POLICY_T;

/*********************************************************************
* TFT_ACI_LNA (Puff)
**********************************************************************/
typedef enum _ENUM_LNA_RESULT_T {
    ENUM_LNA_RESULT_UL = 1,
    ENUM_LNA_RESULT_L   = 2,
    ENUM_LNA_RESULT_M   = 4,
    ENUM_LNA_RESULT_H   = 6
} ENUM_LNA_RESULT_T, *P_ENUM_LNA_RESULT_T;

typedef enum _ENUM_TFT_ACI_LNA_THRESHOLD {
    ENUM_TFT_ACI_LNA_THR_1_2,
    ENUM_TFT_ACI_LNA_THR_1_4,
    ENUM_TFT_ACI_LNA_THR_1_8,
    ENUM_TFT_ACI_LNA_THR_1_16
} ENUM_TFT_ACI_LNA_THRESHOLD, *P_ENUM_TFT_ACI_LNA_THRESHOLD;

typedef enum _ENUM_TFT_ACI_LNA_TIMEOUT_SEL {
    ENUM_TFT_ACI_LNA_TIMEOUT_50,
    ENUM_TFT_ACI_LNA_TIMEOUT_100,
    ENUM_TFT_ACI_LNA_TIMEOUT_250,
    ENUM_TFT_ACI_LNA_TIMEOUT_500
} ENUM_TFT_ACI_LNA_TIMEOUT_SEL, *P_ENUM_TFT_ACI_LNA_TIMEOUT_SEL;

typedef struct _STRUCT_TFT_ACI_LNA_RESULT {
    BOOL fgTftResult;
    BOOL fgAciLnaResult;
    ENUM_LNA_RESULT_T eLnaResult;
} STRUCT_TFT_ACI_LNA_RESULT, *P_STRUCT_TFT_ACI_LNA_RESULT;

typedef struct _STRUCT_TFT_ACI_LNA_CNT {
    UINT_8  ucCntFilterPkt;
    UINT_8  ucTftCnt;
    UINT_8  ucAciLnaCnt;
    UINT_8  ucLnaHCnt;
    UINT_8  ucLnaMCnt;
    UINT_8  ucLnaLCnt;
    UINT_8  ucLnaUlCnt;
} STRUCT_TFT_ACI_LNA_CNT, *P_STRUCT_TFT_ACI_LNA_CNT;

typedef struct _HAL_MAC_RATECODE_DATARATE_MCS_INFO_T
{
    UINT_8          ucPhyRateCode;
    UINT_32         u4PhyRate[4 /*BW*/][2 /*GI*/];

}HAL_MAC_RATECODE_DATARATE_MCS_INFO_T,*P_HAL_MAC_RATECODE_DATARATE_MCS_INFO_T;

typedef struct _HAL_MAC_RATECODE_DATARATE_INFO_T
{
    UINT_8          ucPhyRateCode;
    UINT_16         u2PhyRate;

}HAL_MAC_RATECODE_DATARATE_INFO_T,*P_HAL_MAC_RATECODE_DATARATE_INFO_T;

#if (CFG_HW_SMESH_SUPPORT == 1)
/* SMESH */
typedef enum _ENUM_SMESH_FILTER_T {
    SMESH_FILTER_RX_A1 = 0,
    SMESH_FILTER_RX_A2,
    SMESH_FILTER_RX_DATA,
    SMESH_FILTER_RX_MGMT,
    SMESH_FILTER_RX_CTRL,
    SMESH_FILTER_NUM
} ENUM_SMESH_FILTER_T;

typedef enum _ENUM_SMESH_DISABLE_FILTER_T {
    SMESH_FILTER_DISABLE_RX_A1 = 0,
    SMESH_FILTER_DISABLE_RX_A2,
    SMESH_FILTER_DISABLE_RX_A1_A2,
    SMESH_FILTER_DISABLE_NUM
} ENUM_SMESH_DISABLE_FILTER_T;
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
#define HAL_SET_EVENT(_eventBits) \
            HAL_MCR_WR(MCR_HWFSCR, _eventBits)

#define HAL_SET_RESPONSE(_prRspBuf) \
            halSetResponse(_prRspBuf)

#define HAL_WIFI_INTR_UNMASK() \
        { \
            /* for WiFi */ \
            IRQClearInt(IRQ_WIFI_CODE); \
            IRQUnmask(IRQ_WIFI_CODE); \
            if (WIFI_TRANS_SDIO == wifi_get_hif_mode()){\
            /* for HIF */ \
            HAL_REG_32(HIF_HGFIER) |= HIF_HGFIER_HWF_INT;} \
            /* for PSE */ \
            IRQClearInt(IRQ_WIFI_PSE_CODE); \
            IRQUnmask(IRQ_WIFI_PSE_CODE); \
        }

#define HAL_WIFI_INTR_MASK() \
        { \
            /* for WiFi */ \
            IRQMask(IRQ_WIFI_CODE); \
            if (WIFI_TRANS_SDIO == wifi_get_hif_mode()){\
            /* for HIF */ \
            HAL_REG_32(HIF_HGFIER) &= ~HIF_HGFIER_HWF_INT;} \
            /* for PSE */ \
            IRQMask(IRQ_WIFI_PSE_CODE); \
        }

#define HAL_POWER_ON_WIFISYS() \
    { \
        HAL_REG_32(MCU_REG_WCR) = WIFI_SOFT_ISO_EN; \
        HAL_REG_32(MCU_REG_WCR) = WIFI_SOFT_PD_B | WIFI_SOFT_ISO_EN; \
        DELAY_US(10); \
        HAL_REG_32(MCU_REG_WCR) = ROMW_SOFT_PD_B; \
        DELAY_US(1); \
        HAL_REG_32(MCU_REG_WCR) = ROMW_SOFT_PD_B | WIFI_SOFT_RST_B; \
    }

#define HAL_POWER_OFF_WIFISYS() \
    { \
        HAL_REG_32(MCU_REG_WCR) = WIFI_SOFT_PD_B | WIFI_SOFT_RST_B; \
        HAL_REG_32(MCU_REG_WCR) = WIFI_SOFT_PD_B | WIFI_SOFT_RST_B | WIFI_SOFT_ISO_EN; \
        DELAY_US(1); \
        HAL_REG_32(MCU_REG_WCR) = WIFI_SOFT_ISO_EN; \
    }

#define HAL_POWER_ON_WIFI_MEMORY() \
    { \
        /* ROMW */ \
        HAL_REG_32(MCU_REG_ROMWCR) = 0; \
        HAL_REG_32(MCU_REG_ROMWCR) = ROMW_SOFT_PD_B; \
        DELAY_US(10); \
        HAL_REG_32(MCU_REG_ROMWCR) = ROMW_SOFT_PD_B | ROMW_SOFT_RST_B; \
        /* RAMAW */ \
        HAL_REG_32(MCU_REG_RAMAWCR) = 0; \
        HAL_REG_32(MCU_REG_RAMAWCR) = RAMAW_SOFT_PD_B; \
        DELAY_US(10); \
        HAL_REG_32(MCU_REG_RAMAWCR) = RAMAW_SOFT_PD_B | RAMAW_SOFT_RST_B; \
        /* RAMW */ \
        HAL_REG_32(MCU_REG_RAMWCR) = 0; \
        HAL_REG_32(MCU_REG_RAMWCR) = RAMW_SOFT_PD_B; \
        DELAY_US(10); \
        HAL_REG_32(MCU_REG_RAMWCR) = RAMW_SOFT_PD_B | RAMW_SOFT_RST_B; \
    }

#define HAL_POWER_OFF_WIFI_MEMORY() \
    { \
        /* ROMW */ \
        HAL_REG_32(MCU_REG_ROMWCR) = 0; \
        /* RAMAW */ \
        HAL_REG_32(MCU_REG_RAMAWCR) = 0; \
        /* RAMW */ \
        HAL_REG_32(MCU_REG_RAMWCR) = 0; \
    }

#define HAL_ACKNOWLEDGE_DRIVER_OWN_REQUEST() \
    { \
        HAL_REG_32(MCU_REG_HWFSCR) = HWFSCR_FW_OWN_BACK_INT_SET; \
    }

//Enable the corresponding MASK bit (Interrupt is disabled)
#define HAL_CIRQ_MASK_SET(_irqSel) \
    { \
        HAL_REG_32(MCU_REG_IRQ_MASK_EN) = BIT(_irqSel); \
    }

//Disable the corresponding MASK bit (Interrupt is enabled)
#define HAL_CIRQ_MASK_CLR(_irqSel) \
    { \
        HAL_REG_32(MCU_REG_IRQ_MASK_DIS) = BIT(_irqSel); \
    }


#define HAL_MAC_TX_SET_MSDU_INFO_PTR(_prMacTxDesc, _prMsduInfo) \
    { \
        ((P_EXT_MAC_TX_DESC_T)_prMacTxDesc)->prMsduInfo = _prMsduInfo; \
    }

#define HAL_MAC_TX_GET_MSDU_INFO_PTR(_prMacTxDesc) ((P_EXT_MAC_TX_DESC_T)_prMacTxDesc)->prMsduInfo


#define HAL_MAC_THERMAL_BUSY(_u4Value) ((_u4Value) & THERMCR0_THERM_BUSY)

#define THERM_ADCOUT_DONE(_u4Value) \
            ((_u4Value) & TSTCS_TEST_MODE_ADCOUNT_DONE)

#define HAL_MAC_THERMAL_MAX_INT_EN(_fgMaxInt) \
    { \
        HAL_REG_32(THERM_THERMCR3) = (_fgMaxInt)? \
        (HAL_REG_32(THERM_THERMCR3) | THERMCR3_THERM_MAX_INT_EN) : \
        (HAL_REG_32(THERM_THERMCR3) & ~THERMCR3_THERM_MAX_INT_EN); \
    }

#define HAL_MAC_THERMAL_SCND_MAX_INT_EN(_fgScndMaxInt) \
    { \
        HAL_REG_32(THERM_THERMCR3) = (_fgScndMaxInt)? \
        (HAL_REG_32(THERM_THERMCR3) | THERMCR3_THERM_SCND_MAX_INT_EN) : \
        (HAL_REG_32(THERM_THERMCR3) & ~THERMCR3_THERM_SCND_MAX_INT_EN); \
    }

#define HAL_MAC_THERMAL_MIN_INT_EN(_fgMinInt) \
    { \
        HAL_REG_32(THERM_THERMCR3) = (_fgMinInt)? \
        (HAL_REG_32(THERM_THERMCR3) | THERMCR3_THERM_MIN_INT_EN) : \
        (HAL_REG_32(THERM_THERMCR3) & ~THERMCR3_THERM_MIN_INT_EN); \
    }

#define HAL_MAC_THERMAL_TRIGGER_EN(_fgTrigger) \
    { \
        HAL_REG_32(THERM_THERMCR0) = (_fgTrigger)? \
        (HAL_REG_32(THERM_THERMCR0) | THERMCR0_THERM_TRIGGER) : \
        (HAL_REG_32(THERM_THERMCR0) & ~THERMCR0_THERM_TRIGGER); \
    }

#define HAL_MAC_THERMAL_MAX_INT_IS_ASSERT() \
        HAL_REG_32(THERM_THERMISR) & THERMISR_THERM_MAX_INT

#define HAL_MAC_THERMAL_SCND_MAX_INT_IS_ASSERT() \
        HAL_REG_32(THERM_THERMISR) & THERMISR_THERM_SCND_MAX_INT

#define HAL_MAC_THERMAL_MIN_INT_IS_ASSERT() \
        HAL_REG_32(THERM_THERMISR) & THERMISR_THERM_MIN_INT

#define HAL_MAC_THERMAL_MAX_INT_CLEAR() \
        HAL_REG_32(THERM_THERMISR) = THERMISR_THERM_MAX_INT

#define HAL_MAC_THERMAL_SCND_MAX_INT_CLEAR() \
        HAL_REG_32(THERM_THERMISR) = THERMISR_THERM_SCND_MAX_INT

#define HAL_MAC_THERMAL_MIN_INT_CLEAR() \
        HAL_REG_32(THERM_THERMISR) = THERMISR_THERM_MIN_INT

#define HAL_MAC_THERMAL_OSC_CLK_IS_52M(_fgTrigger) \
    { \
        HAL_REG_32(THERM_THERMCR2) = (_fgTrigger)? \
        (HAL_REG_32(THERM_THERMCR2) | THERMCR2_RG_THAD_CLK_DIV_16) : \
        (HAL_REG_32(THERM_THERMCR2) & ~THERMCR2_RG_THAD_CLK_DIV_16); \
    }

#define HAL_MAC_TFT_ACI_LNA_RX_FILTER_PASS_EN(_fgTrigger) \
    { \
        HAL_REG_32(RMAC_TFTACI0) = (_fgTrigger)? \
        (HAL_REG_32(RMAC_TFTACI0) | TFTACI0_RX_FILTER_PASS) : \
        (HAL_REG_32(RMAC_TFTACI0) & ~TFTACI0_RX_FILTER_PASS); \
    }

#define HAL_MAC_TFT_ACI_LNA_RESET_CNT_EN(_fgTrigger) \
    { \
        HAL_REG_32(RMAC_TFTACI0) = (_fgTrigger)? \
        (HAL_REG_32(RMAC_TFTACI0) | TFTACI0_RESET_CNT) : \
        (HAL_REG_32(RMAC_TFTACI0) & ~TFTACI0_RESET_CNT); \
    }

#define HAL_MAC_TFT_ACI_LNA_RESET_ALL_EN(_fgTrigger) \
    { \
        HAL_REG_32(RMAC_TFTACI0) = (_fgTrigger)? \
        (HAL_REG_32(RMAC_TFTACI0) | TFTACI0_RESET_ALL) : \
        (HAL_REG_32(RMAC_TFTACI0) & ~TFTACI0_RESET_ALL); \
    }

#define HAL_MAC_RATECODE_DATARATE_INFO(_RateCode, _Bw20, _Bw20SGI, _Bw40, _BW40SGI, _Bw80, _Bw80SGI, _Bw160, _Bw160SGI) \
    { \
        .ucPhyRateCode                            = (_RateCode), \
        .u4PhyRate[RF_BW_20][MAC_GI_NORMAL]       = (_Bw20), \
        .u4PhyRate[RF_BW_20][MAC_GI_SHORT]        = (_Bw20SGI), \
        .u4PhyRate[RF_BW_40][MAC_GI_NORMAL]       = (_Bw40), \
        .u4PhyRate[RF_BW_40][MAC_GI_SHORT]        = (_BW40SGI), \
        .u4PhyRate[RF_BW_80][MAC_GI_NORMAL]       = (_Bw80), \
        .u4PhyRate[RF_BW_80][MAC_GI_SHORT]        = (_Bw80SGI), \
        .u4PhyRate[RF_BW_160][MAC_GI_NORMAL]      = (_Bw160), \
        .u4PhyRate[RF_BW_160][MAC_GI_SHORT]       = (_Bw160SGI), \
    }


/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
	extern HAL_STATUS
	(*halSetBSSID)(
		IN UINT_8		ucBssidIdx,
		IN BOOL 		fgEnable,
		IN PUINT_8		pucBSSID
		);
	
	extern HAL_STATUS
	(*halSetOwnMACAddr)(
		IN UINT_8		ucIdx,
		IN BOOL 		fgEnable,
		IN PUINT_8		pucMACAddr
		);

#endif /* _HAL_ROM_H */

