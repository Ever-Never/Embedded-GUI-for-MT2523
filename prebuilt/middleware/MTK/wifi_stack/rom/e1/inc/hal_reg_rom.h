/*
** $Id: //Department/DaVinci/TRUNK/MT6628_FPGA_BORA_BASE_20111109/mcu/wifi/ROM/include/nic/hal/MT6628/mt6628_wifi_regv1_rom.h#2 $
*/

/*! \file   "mt592x_reg.h"
    \brief  The common register definition of mt592x series

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
** $Log: mt6630_wifi_regv1.h $
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
** 12 17 2014 changyi.hsu
** [BORA00003214] [MT7637]
** 1. Refine and add RXP test cases.
**
** 12 16 2014 rick.wen
** [BORA00003651] [MT7637] TMAC emulation failed on PIFS measurement (emulTxTestCase_10_2_2)
** - Modify the emulation code for PIFS measurement.
**   - A new TMAC CR (SPCR0, 0x00FC) was introduced to control the TX Burst option.
** - Correct the IFS counter for MT7637 FPGA configuration (WF_1X = 20MHz, HCLK = 30MHz)
**
** 12 12 2014 danielk.lu
** [BORA00003507] [MT7637][Wi-Fi]
** 1. Sync MT7637_WIFI_RF_Programming_Guide_20141207
** 2. Add CFG_WAIT_CALIBRATION_AND_JUMP_TO_RAM_CODE
**
** 11 21 2014 danielk.lu
** [BORA00003507] [MT7637][Wi-Fi]
** Check-in Wi-Fi Calibration Sequence
**
** 11 19 2014 changyi.hsu
** [BORA00003214] [MT7637]
** 1. Add RXP related functions.
**
** 11 19 2014 changyi.hsu
** [BORA00003214] [MT7637]
** 1. Move WTBL related HAL functions to "hal_wtbl_rom.c".
**
** 11 17 2014 changyi.hsu
** [BORA00003214] [MT7637]
** 1. Add RXP test cases.
**
** 11 15 2014 ted.huang
** [BORA00003379] [MT7637]
** move RAM hal to ROM.
**
** 11 07 2014 changyi.hsu
** [BORA00003214] [MT7637/MT7615]
** 1. Rename PTA functions to avoid compiler error.
** 2. Add new ARB CR address for Rx protection (RXP).
** 3. Add new ARB HAL function for RXP.
** 4. Add one Rx function for RXP.
**
** 11 03 2014 ted.huang
** [BORA00003379] [MT7637]
** 1)WTBL entry support 128, 2) add repeater/MBSS/Indium emulation cases.
**
** 10 31 2014 changyi.hsu
** [BORA00003214] [MT7637/MT7615]
** 1. Merge form RAM to ROM HAL functions.
**
** 10 20 2014 changyi.hsu
** [BORA00003214] [MT7637/MT7615]
** 1. Remove SDA base optimization to pass WTBL regression.
** 2. Enable WiFi power on sequence to pass WTBL regression.
** 3. Add IPSM HAL related functions to pass WTBL regression for HW modification.
**
** 10 15 2014 changyi.hsu
** [BORA00003214] [MT7637/MT7615]
** 1. Add 3 DSW test cases.
** 2. Add MIB related HAL function for DSW.
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
** 09 15 2014 ted.huang
** [BORA00002807] [MT7636]
** merge MT7636 back main trunk.
**
** 07 03 2014 chiahsuan.chuang
** [BORA00002977] [MT7636][WiFi]
** . add default DMA scheduler bypass mode with EP4 OUT in wifi firmware download.
**
** 06 26 2014 chiahsuan.chuang
** [BORA00002977] [MT7636][WiFi]
** . add dma scheduler reset in firmare download state to sync with PSE reset.
**
** 04 10 2014 yi-cheng.huang
** [BORA00003036] [WIFI][MT7636] Modify MT7636 LED Emulation
** Merge emulation code from 7603
**
** 01 13 2014 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Add HW Register Map
**
** 01 01 2014 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Merge MT6630 E2 Change and Refine Code
**
** 12 31 2013 ted.huang
** [BORA00002807] [MT7636]
** fix VTR9 register definition.
**
** 12 30 2013 ted.huang
** [BORA00002807] [MT7636]
** Add new duplicated RFB hal API.
**
** 12 27 2013 chiahsuan.chuang
** [BORA00002663] [WiFi][HIF]
** . set HIF DMA scheduler bypass mode when in HIF loopback.
**
** 12 12 2013 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Add DMA Scheduler Test Option
**
** 12 12 2013 ted.huang
** [BORA00002807] [MT7636]
** LP emulation: add 1us tick setting.
**
** 12 09 2013 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** MIB Retry3 Emulation Bug Fix
**
** 12 08 2013 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Add MIB Retry3 Emulation and Solve MIB Retry Bug
**
** 12 03 2013 ted.huang
** [BORA00002807] [MT7636]
** update 7636 power on sequence 1203.
**
** 11 28 2013 ted.huang
** [BORA00002807] [MT7636]
** update LP emulation sleep/wakeup timing.
**
** 11 25 2013 ted.huang
** [BORA00002807] [MT7636]
** update LP config setting.
**
** 11 24 2013 ted.huang
** [BORA00002807] [MT7636]
** 1. add RX SMPS emulation code 2. merge ROM from 6630E2.
**
** 11 19 2013 ted.huang
** [BORA00002807] [MT7636]
** Add RMAC/LP design change emulation code.
**
** 11 18 2013 chiahsuan.chuang
** [BORA00002663] [WiFi][HIF]
** . update for firmware download.
**
** 11 14 2013 ted.huang
** [BORA00002807] [MT7636]
** fix LP/COEX emulation warning.
**
** 11 13 2013 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Tx AMPDU Emulation Change and Bug Fix
**
** 11 10 2013 ted.huang
** [BORA00002807] [MT7636]
** update LP/CMDBT emulation code.
**
** 11 07 2013 ted.huang
** [BORA00002807] [MT7636]
** update lp/PTA emulation.
**
** 11 05 2013 chiahsuan.chuang
** [BORA00002663] [WiFi][HIF]
** . Add firmware sync. interface with driver.
**
** 10 21 2013 ted.huang
** [BORA00002807] [MT7636]
** Update RMAC LQ test and PSMP test.
**
** 10 14 2013 ted.huang
** [BORA00002807] [MT7636]
** 1. 7636 RMAC Hal 2. fix RMAC emulation coding bug.
**
** 08 07 2013 tsaiyuan.hsu
** [BORA00002222] MT6630 unified MAC RXM
** move txbf register definition to RAM base.
**
** 08 07 2013 tsaiyuan.hsu
** [BORA00002222] MT6630 unified MAC RXM
** add TxBF control.
**
** 07 24 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add rom patch for "Enter LP assertion check" and fix getCurrentTsf function.
**
** 07 18 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add mib counter into backup/restore table.
**
** 07 04 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add reg definition for ndpa sp.
**
** 07 03 2013 puff.wen
** [BORA00002679] [MT6630] Update emulation of PSMP/Thermal/Voltage
** Add PSMP/Thermal/Voltage
**
** 06 26 2013 bennett.ou
** [BORA00002661] [mt6630][Wi-Fi] Design for PF and LED
** Add LED register define
**
** 06 20 2013 chia-hsiang.hsu
** [BORA00002524] [MT6630 Wi-Fi] hal function and emulation code update
** Add hal functions
**
** 06 19 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** update hal for pm.
**
** 06 14 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Move Measurement HAL functions to RAM code
**
** 06 11 2013 cp.wu
** [BORA00002220] [MT6630 Wi-Fi] Draft FW for HIFSYS update and ROM code logic regarding HIFSYS access
** add new HAL API for HIF Client module
**
** 06 11 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add hal to config wakeup on next dtim or tbtt as rx critical beacon.
**
** 06 07 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** Fix the pf macro and status bit
**
** 06 07 2013 jeffrey.chang
** [BORA00002471] [MT6630 Wi-Fi] CMDBT emulation code
** update cmdbt emulation code
**
** 06 05 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** Fix compile error after moving API to RAM
**
** 06 04 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** 1. Remove unused code 2. move the API to RAM 3. enable crypto
**
** 06 04 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** remove obsolete pattern filter.
**
** 06 04 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Modified Measurement & Quiet HAL APIs
**
** 06 03 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** update tttt earlier interval hal functions.
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
** 05 24 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** update wifi power on sequence.
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
** 05 20 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** update backup/restore table.
**
** 05 17 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Fix LQ related CR defition and related HAL codes
**
** 05 16 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Update TX emulation code
**
** 05 16 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** update cmdbt hal functions.
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
** 05 08 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** .
**
** 05 07 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add programmable beacon filters.
**
** 05 07 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** Update AMPDU MIB counter HAL
**
** 05 03 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** update AR emulation code
**
** 05 02 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** update LP registers.
**
** 05 02 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** .
**
** 05 02 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add sleep control.
**
** 05 02 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** check in the definition of NPS register.
**
** 04 30 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add halMacRxConfigTSFUpdateFrames to config frames for tsf sync.
**
** 04 30 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** update rlm statistic and rlm update wtbl function
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
** 04 19 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Fix RMAC MAXMIN Length register definition according to new spec.
**
** 04 17 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** 1. New per-BSS ERP protection.
** 2. New TX rate code definition
** 3. RTS retry limit extended to 32
**
** 04 16 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add test cases for timers.
**
** 04 16 2013 chia-hsiang.hsu
** [BORA00002524] [MT6630 Wi-Fi] hal function and emulation code update
** Add PTMR7 definition
**
** 04 16 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Fix macheader compose function in A4 case
** Add ACI defintion for new ACI design
** Modify LNA enum value for new ACI design
**
** 04 16 2013 chia-hsiang.hsu
** [BORA00002524] [MT6630 Wi-Fi] hal function and emulation code update
** Fix the typo
**
** 04 15 2013 chia-hsiang.hsu
** [BORA00002524] [MT6630 Wi-Fi] hal function and emulation code update
** Update coexistence CR definition (MT6630 ARB CR v0.50)
**
** 04 11 2013 cp.wu
** [BORA00002220] [MT6630 Wi-Fi] Draft FW for HIFSYS update and ROM code logic regarding HIFSYS access
** correct typo
**
** 04 09 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** update lp top registers.
**
** 04 09 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** update lp registers.
**
** 04 09 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** update definition of wifi interrupts.
**
** 04 08 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Change the defintion of 0x60300044 from DTIM count to TSF selectio register according to new LP_TOP doc
**
** 04 05 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** 1. Modify codes and definition for new ACI design
** 2. Modify WLAN table search API
** 3. Modify TFT test cases according to new ACI design
**
** 04 03 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** <saved by Perforce>
** Update PCR1 based on AGG CR version 0.23
**
** 04 02 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** 1. Add TX/RX counters and byte counters for perBSS usage
** 2. Add Hal_ini about AMPDU and AMSDU de-aggregation enable
** 3. Modify MIB counter get API to clear the read data
**
** 04 02 2013 eason.tsai
** [BORA00002212] [MT6630 Wi-Fi][Rom]develop
** .
**
** 03 29 2013 jeffrey.chang
** [BORA00002471] [MT6630 Wi-Fi] CMDBT emulation code
** move CRs need backup to another power domain
**
** 03 18 2013 cp.wu
** [BORA00002220] [MT6630 Wi-Fi] Draft FW for HIFSYS update and ROM code logic regarding HIFSYS access
** update HIF HAL API for HIF Client Interrupt initialization
**
** 03 18 2013 cp.wu
** [BORA00002220] [MT6630 Wi-Fi] Draft FW for HIFSYS update and ROM code logic regarding HIFSYS access
** update MT6630 HIFSYS HAL APIs
** 1. change upon checksum offload datasheet update
** 2. add API for control interrupt-to-host masking control for LP scenario
** 3. update RX coalescing API, now RX0 and RX1 can be controlled separately
**
** 03 14 2013 tsaiyuan.hsu
** [BORA00002222] MT6630 unified MAC RXM
** add wifi power on sequence.
**
** 03 13 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** fix WTBL 1 entry size offset defintion
**
** 03 06 2013 pat.lu
** [BORA00002365] [WLAN] Sec. and WTBL FPGA Emulation Modification
** .
** 1. Modify RX Filter defintion accourding to fpga2 bit file from 0305 on
**    (Default value which SW does not need to change is set to 0)
** 2. Modify default rx filter setting (adding drop RTS and CTS)
** 3. Modify NAV and Duplicate filter test codes
**
** 03 04 2013 cp.wu
** [BORA00002220] [MT6630 Wi-Fi] Draft FW for HIFSYS update and ROM code logic regarding HIFSYS access
** 1. implementation of HIFSYS HAL API for checksum offload enable/disable
** 2. add basic framework for UART firmware download path (SLT purpose)
**
** 02 27 2013 pat.lu
** [BORA00002365] [WLAN] Sec. and WTBL FPGA Emulation Modification
** .
** 1. Add MUAR hash mode test cases 10_3_2 and rename 10_3 to 10_3_1
** 2. Add RX filter default value defintion
** 3. fix promiscuous mode filter
**
** 02 25 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** 1. Add AMPDU aggregate count MIB conuter HAL
**
** 02 22 2013 terry.wu
** [BORA00002226] [MT6630 Wi-Fi] [Emulation] VHT & AMPDU emulation
** Update BA related HAL.
**
** 02 20 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add WISR for mt6630.
**
** 02 20 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** modify WIS1R.
**
** 02 20 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add WIS3R for LP and change Timer4 to Timer8.
**
** 02 19 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** extend bssid number for LP functions.
**
** 02 18 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** fix LP redifinition.
**
** 02 18 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** .
**
** 02 18 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add LP register fields.
**
** 02 18 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add LP register definition.
**
** 02 07 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** remove LP_OFF module.
**
** 02 07 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** .
**
** 02 07 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** .
**
** 02 07 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Update CR base.  Correct WTBL offset definition.  Update CR naming.
**
** 02 06 2013 cp.wu
** [BORA00002220] [MT6630 Wi-Fi] Draft FW for HIFSYS update and ROM code logic regarding HIFSYS access
** complete firmware download agent design
**
** 02 05 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Update TX emulation code
**
** 01 31 2013 cp.wu
** [BORA00002220] [MT6630 Wi-Fi] Draft FW for HIFSYS update and ROM code logic regarding HIFSYS access
** update firmware download code for matching with PDA+SEC+HIF Client architecture
**
** 01 28 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** fix build error for LP.
**
** 01 28 2013 cp.wu
** [BORA00002220] [MT6630 Wi-Fi] Draft FW for HIFSYS update and ROM code logic regarding HIFSYS access
** add register definitions for PDA. (Patch Decryption Accelerator)
**
** 01 28 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .move UART dump code to ROM for modualizing RAM and ROM codes
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
** 01 04 2013 chinglan.wang
** [BORA00002293] [MT6630]Develop TX emulation code
** .
**
** 12 12 2012 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** .
**
** 12 10 2012 chinglan.wang
** [BORA00002293] [MT6630]Develop TX emulation code
** .
**
** 12 10 2012 chinglan.wang
** [BORA00002293] [MT6630]Develop TX emulation code
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
** 11 09 2012 cp.wu
** [BORA00002220] [MT6630 Wi-Fi] Draft FW for HIFSYS update and ROM code logic regarding HIFSYS access
** HAL API implementation for datasheet update
**
** 10 30 2012 eason.tsai
** [BORA00002212] [MT6630 Wi-Fi][Rom]develop
** add lost macro
**
** 10 29 2012 eason.tsai
** [BORA00002212] [MT6630 Wi-Fi][Rom]develop
** auto rate hal code
**
** 10 26 2012 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Update HAL APIs and emulation code
**
** 10 17 2012 eason.tsai
** [BORA00002212] [MT6630 Wi-Fi][Rom]develop
** update wtbl2 macro
**
** 10 15 2012 cp.wu
** [BORA00002097] [MT6582 Wi-Fi][Firmware] Modify makefile for MT6582 and MT6630 build
** add MT6630 HIFSYS Client CR HAL API implementation.
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
** 08 06 2012 yarco.yang
** [BORA00002091] [MT6630]Support 6630 complier flag
**
*/
#ifndef _MT6630_WIFI_REGV1_ROM_H
#define _MT6630_WIFI_REGV1_ROM_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

#define CFG_COEX_SUPPORT_TPC                    0
#define CFG_COEX_SUPPORT_EXT_WIMAX_IF           0
#define CFG_COEX_SUPPORT_EXT_BT_IF              0

#define DUAL_NETWORK_SUPPORT    1

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

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
/* the base address of the memory map */

#define WIFI_CONFG_BASE         g_WIFI_CONFG_BASE
#define WIFI_MAC_BASE           g_WIFI_MAC_BASE  
#define WIFI_PHY_BASE           g_WIFI_PHY_BASE  
#define WIFI_LP_BASE            g_WIFI_LP_BASE   
#define WIFI_LPAON_BASE         g_WIFI_LPAON_BASE
#define WIFI_INT_BASE           g_WIFI_INT_BASE  
#define WIFI_WTBL_BASE          g_WIFI_WTBL_BASE 
#define WIFI_MIB_BASE           g_WIFI_MIB_BASE  
#define WIFI_AON_BASE           g_WIFI_AON_BASE  
#define TOP_SADC_BASE           g_TOP_SADC_BASE  
#define TOP_THERM_BASE          g_TOP_THERM_BASE 
#define WIFI_THERM_BASE         TOP_THERM_BASE
#define WIFI_PDA_BASE           g_WIFI_PDA_BASE     
#define MCU_CONFIG_BASE         g_MCU_CONFIG_BASE   
#define TOP_RGU_BASE            g_TOP_RGU_BASE      
#define TOP_CFG_AON_BASE        g_TOP_CFG_AON_BASE  
#define WIFI_CMDBT_ON_BASE      g_WIFI_CMDBT_ON_BASE
#define WIFI_CMDBT_BASE         g_WIFI_CMDBT_BASE   
#define TOP_GPIO_BASE           g_TOP_GPIO_BASE     
#define WF_WTBL_ON_BASE	g_WF_WTBL_ON_BASE
#define HIFCR_BASE 			g_HIFCR_BASE
#define PSE_BASE_ADDR		g_PSE_BASE_ADDR
/* the base address of the Wi-Fi modules */
#define WIFI_TRB_BASE           (WIFI_MAC_BASE + 0x00000000)
#define WIFI_AGG_BASE           (WIFI_MAC_BASE + 0x00010000)
#define WIFI_ARB_BASE           (WIFI_MAC_BASE + 0x00020000)
#define WIFI_TMAC_BASE          (WIFI_MAC_BASE + 0x00030000)
#define WIFI_RMAC_BASE          (WIFI_MAC_BASE + 0x00040000)
#define WIFI_SEC_BASE           (WIFI_MAC_BASE + 0x00050000)
#define WIFI_DMA_BASE           (WIFI_MAC_BASE + 0x00060000)
#define WIFI_CONFG_OFF_BASE     (WIFI_MAC_BASE + 0x00070000)
#define WIFI_PF_BASE            (WIFI_MAC_BASE + 0x00080000)
//#define WIFI_DBG_BASE           (WIFI_MAC_BASE + 0x00090000)
#define WIFI_WTBLOFF_TOP        (WIFI_MAC_BASE + 0x00090000)
#define WIFI_ETBF_BASS          (WIFI_MAC_BASE + 0x000A0000)


//3 WIS0R
#define WIS0R_BTIM_BMC_TIMEOUT0     BIT(31)
#define WIS0R_ABNORMAL_INT          BIT(30)
#define WIS0R_BTIM_TIMEOUT0         BIT(29)
#define WIS0R_WLANON_TXCNT_OVERFLOW BIT(28)
#define WIS0R_SP_END                BIT(27)
#define WIS0R_TSF1_DRIFT            BIT(26)
#define WIS0R_TSF0_DRIFT            BIT(25)
#define WIS0R_BMC_TIMEOUT0          BIT(24)
#define WIS0R_BCN_TIMEOUT0          BIT(23)
#define WIS0R_PRETBTT0              BIT(22)
#define WIS0R_TBTT0                 BIT(21)
#define WIS0R_T8_TIME               BIT(20)
#define WIS0R_T3_TIME               BIT(19)
#define WIS0R_T2_TIME               BIT(18)
#define WIS0R_T1_TIME               BIT(17)
#define WIS0R_T0_TIME               BIT(16)
#define WIS0R_PRETTTT0              BIT(15)
#define WIS0R_QUIET_DONE            BIT(14)
#define WIS0R_MEASUREMENT_DONE      BIT(13)
#define WIS0R_ROAM1_INT             BIT(12)
#define WIS0R_BEACON_T_OK           BIT(11)
#define WIS0R_PRE_RX_DONE           BIT(10)
#define WIS0R_TTTT0                 BIT(9)
#define WIS0R_ROAM0_INT             BIT(8)
#define WIS0R_TRAP3_INT             BIT(7)
#define WIS0R_TRAP2_INT             BIT(6)
#define WIS0R_TSF3_DRIFT            BIT(5)
#define WIS0R_TSF2_DRIFT            BIT(4)
#define WIS0R_T7_TIME               BIT(3)
#define WIS0R_T6_TIME               BIT(2)
#define WIS0R_T5_TIME               BIT(1)
#define WIS0R_T4_TIME               BIT(0)
//#define WIS0R_AC_TX_DONE            BITS(0, 6)

//3 WIS1R
#define WIS1R_WNM_SLEEP_INT         BIT(31)
#define WIS1R_DMA_ERR6_INT          BIT(30)
#define WIS1R_DMA_ERR5_INT          BIT(29)
#define WIS1R_DMA_ERR4_INT          BIT(28)
#define WIS1R_DMA_ERR3_INT          BIT(27)
#define WIS1R_DMA_ERR2_INT          BIT(26)
#define WIS1R_DMA_ERR1_INT          BIT(25)
#define WIS1R_BMC_TIMEOUT2          BIT(24)
#define WIS1R_BCN_TIMEOUT2          BIT(23)
#define WIS1R_PRETBTT2              BIT(22)
#define WIS1R_TBTT2                 BIT(21)
#define WIS1R_NDPA_TIMEOUT          BIT(20)
#define WIS1R_RESERVED1             BIT(19)
#define WIS1R_TRAP0_INT             BIT(18)
#define WIE1R_NAN_2G4_DW_END_INT    BIT(17)
#define WIS1R_BTIM_BMC_TIMEOUT1     BIT(16)
#define WIS1R_BTIM_TIMEOUT1         BIT(15)
#define WIS1R_PRETTTT1              BIT(14)
#define WIS1R_ARB_RW_EXIT_PROTECT   BIT(13)
#define WIS1R_ARB_RW_ENTER_PROTECT  BIT(12)
#define WIS1R_BMC_TIMEOUT1          BIT(11)
#define WIS1R_BCN_TIMEOUT1          BIT(10)
#define WIS1R_PRETBTT1              BIT(9)
#define WIS1R_TBTT1                 BIT(8)
#define WIS1R_MACON_NO_DEFINE       BIT(7)
#define WIS1R_MACOFF_NO_DEFINE      BIT(6)
#define WIS1R_DMA_ERR0_INT          BIT(5)
#define WIS1R_RMAC_PHYRX_ERR_INT    BIT(4)
#define WIS1R_RCPI_INT              BIT(3)
#define WIS1R_MAC_TX_RDY            BIT(2)
#define WIS1R_MAC_RX_RDY            BIT(1)
#define WIS1R_TTTT1                 BIT(0)

//3 WIS2R
#define WIS2R_NAN_5G_DW_END_INT BIT(31)
#define WIS2R_ROAM3_INT         BIT(30)
#define WIS2R_ROAM2_INT         BIT(29)
#define WIS2R_ROAMING_CRC_IRQ3  BIT(28)
#define WIS2R_ROAMING_LQ_IRQ3   BIT(27)
#define WIS2R_ROAMING_LQ_IRQ3_HI  BIT(26)
#define WIS2R_ROAMING_CRC_IRQ2  BIT(25)
#define WIS2R_ROAMING_LQ_IRQ2   BIT(24)
#define WIS2R_ROAMING_LQ_IRQ2_HI BIT(23)
#define WIS2R_RMAC_TFT_ACI_IRQ  BIT(22)
#define WIS2R_ABNOR_TX_INT      BIT(21)
#define WIS2R_ARB_MDRDY_PPDUTIME BIT(20)
#define WIS2R_RM_CRC_IRQ0       BIT(19)
#define WIS2R_RM_LQ_IRQ0        BIT(18)
#define WIS2R_RM_LQ_IRQ0_HI     BIT(17)
#define WIS2R_RM_CRC_IRQ1       BIT(16)
#define WIS2R_RM_LQ_IRQ1        BIT(15)
#define WIS2R_RM_LQ_IRQ1_HI     BIT(14)
#define WIS2R_BMC_TIMEOUT3      BIT(13)
#define WIS2R_BCN_TIMEOUT3      BIT(12)
#define WIS2R_PRETBTT3          BIT(11)
#define WIS2R_TBTT3             BIT(10)
#define WIS2R_BTIM_BMC_TIMEOUT3 BIT(9)
#define WIS2R_BTIM_TIMEOUT3     BIT(8)
#define WIS2R_PRETTTT3          BIT(7)
#define WIS2R_TRAP1_INT         BIT(6)
#define WIS2R_TTTT3             BIT(5)
#define WIS2R_BTIM_BMC_TIMEOUT2 BIT(4)
#define WIS2R_BTIM_TIMEOUT2     BIT(3)
#define WIS2R_PRETTTT2          BIT(2)
#define WIS2R_TTTT2             BIT(1)
#define WIS2R_TIMEUP_CHK_LP     BIT(0)

//WIS3R
#define WIS3R_TBTT0_1           BIT(0)
#define WIS3R_PRETBTT0_1        BIT(16)

//WIS4R
#define WIS4R_TTTT0_1           BIT(0)
#define WIS4R_PRETTTT0_1        BIT(16)

#if 0
#define WIS3R_BTIM_BMC_TIMEOUT3 BIT(31)
#define WIS3R_BTIM_TIMEOUT3     BIT(30)
#define WIS3R_PRETTTT3          BIT(29)
#define WIS3R_TTTT3             BIT(28)
#define WIS3R_BTIM_BMC_TIMEOUT2 BIT(27)
#define WIS3R_BTIM_TIMEOUT2     BIT(26)
#define WIS3R_PRETTTT2          BIT(25)
#define WIS3R_TTTT2             BIT(24)
#define WIS3R_BTIM_BMC_TIMEOUT1 BIT(23)
#define WIS3R_BTIM_TIMEOUT1     BIT(22)
#define WIS3R_PRETTTT1          BIT(21)
#define WIS3R_TTTT1             BIT(20)
#define WIS3R_BTIM_BMC_TIMEOUT0 BIT(19)
#define WIS3R_BTIM_TIMEOUT0     BIT(18)
#define WIS3R_PRETTTT0          BIT(17)
#define WIS3R_TTTT0             BIT(16)
#define WIS3R_TRAP3_INT         BIT(15)
#define WIS3R_TRAP2_INT         BIT(14)
#define WIS3R_TSF3_DRIFT        BIT(13)
#define WIS3R_TSF2_DRIFT        BIT(12)
#define WIS3R_BMC_TIMEOUT3      BIT(11)
#define WIS3R_BCN_TIMEOUT3      BIT(10)
#define WIS3R_PRETBTT3          BIT(9)
#define WIS3R_TBTT3             BIT(8)
#define WIS3R_BMC_TIMEOUT2      BIT(7)
#define WIS3R_BCN_TIMEOUT2      BIT(6)
#define WIS3R_PRETBTT2          BIT(5)
#define WIS3R_TBTT2             BIT(4)
#define WIS3R_T7_TIME           BIT(3)
#define WIS3R_T6_TIME           BIT(2)
#define WIS3R_T5_TIME           BIT(1)
#define WIS3R_T4_TIME           BIT(0)
#endif

//3 WIE0R
#define WIE0R_BTIM_BMC_TIMEOUT0     BIT(31)
#define WIE0R_ABNORMAL_INT          BIT(30)
#define WIE0R_BTIM_TIMEOUT0         BIT(29)
#define WIE0R_WLANON_TXCNT_OVERFLOW BIT(28)
#define WIE0R_SP_END                BIT(27)
#define WIE0R_TSF1_DRIFT            BIT(26)
#define WIE0R_TSF0_DRIFT            BIT(25)
#define WIE0R_BMC_TIMEOUT0          BIT(24)
#define WIE0R_BCN_TIMEOUT0          BIT(23)
#define WIE0R_PRETBTT0              BIT(22)
#define WIE0R_TBTT0                 BIT(21)
#define WIE0R_T8_TIME               BIT(20)
#define WIE0R_T3_TIME               BIT(19)
#define WIE0R_T2_TIME               BIT(18)
#define WIE0R_T1_TIME               BIT(17)
#define WIE0R_T0_TIME               BIT(16)
#define WIE0R_PRETTTT0              BIT(15)
#define WIE0R_QUIET_DONE            BIT(14)
#define WIE0R_MEASUREMENT_DONE      BIT(13)
#define WIE0R_ROAM1_INT             BIT(12)
#define WIE0R_BEACON_T_OK           BIT(11)
#define WIE0R_PRE_RX_DONE           BIT(10)
#define WIE0R_TTTT0                 BIT(9)
#define WIE0R_ROAM0_INT             BIT(8)
#define WIE0R_TRAP3_INT             BIT(7)
#define WIE0R_TRAP2_INT             BIT(6)
#define WIE0R_TSF3_DRIFT            BIT(5)
#define WIE0R_TSF2_DRIFT            BIT(4)
#define WIE0R_T7_TIME               BIT(3)
#define WIE0R_T6_TIME               BIT(2)
#define WIE0R_T5_TIME               BIT(1)
#define WIE0R_T4_TIME               BIT(0)
//#define WIE0R_AC_TX_DONE            BITS(0, 6)

#define WIE0R_ALL_INTS              BITS(0,31)

//3 WIE1R
#define WIE1R_WNM_SLEEP_INT         BIT(31)
#define WIE1R_DMA_ERR6_INT          BIT(30)
#define WIE1R_DMA_ERR5_INT          BIT(29)
#define WIE1R_DMA_ERR4_INT          BIT(28)
#define WIE1R_DMA_ERR3_INT          BIT(27)
#define WIE1R_DMA_ERR2_INT          BIT(26)
#define WIE1R_DMA_ERR1_INT          BIT(25)
#define WIE1R_BMC_TIMEOUT2          BIT(24)
#define WIE1R_BCN_TIMEOUT2          BIT(23)
#define WIE1R_PRETBTT2              BIT(22)
#define WIE1R_TBTT2                 BIT(21)
#define WIE1R_NDPA_TIMEOUT          BIT(20)
#define WIE1R_RESERVED1             BIT(19)
#define WIE1R_TRAP0_INT             BIT(18)
#define WIE1R_NAN_2G4_DW_END_INT    BIT(17)
#define WIE1R_BTIM_BMC_TIMEOUT1     BIT(16)
#define WIE1R_BTIM_TIMEOUT1         BIT(15)
#define WIE1R_PRETTTT1              BIT(14)
#define WIE1R_ARB_RW_EXIT_PROTECT   BIT(13)
#define WIE1R_ARB_RW_ENTER_PROTECT  BIT(12)
#define WIE1R_BMC_TIMEOUT1          BIT(11)
#define WIE1R_BCN_TIMEOUT1          BIT(10)
#define WIE1R_PRETBTT1              BIT(9)
#define WIE1R_TBTT1                 BIT(8)
#define WIE1R_MACON_NO_DEFINE       BIT(7)
#define WIE1R_MACOFF_NO_DEFINE      BIT(6)
#define WIE1R_DMA_ERR0_INT          BIT(5)
//#define WIE1R_OWN_BACK_INT          BIT(5)
#define WIE1R_RMAC_PHYRX_ERR_INT    BIT(4)
#define WIE1R_RCPI                  BIT(3)
#define WIE1R_MAC_TX_RDY            BIT(2)
#define WIE1R_MAC_RX_RDY            BIT(1)
#define WIE1R_TTTT1     BIT(0)

#define WIE1R_ALL_INTS              BITS(0,31)


//3 WIE2R

#define WIE2R_NAN_5G_DW_END_INT BIT(31)
#define WIE2R_ROAM3_INT         BIT(30)
#define WIE2R_ROAM2_INT         BIT(29)
#define WIE2R_ROAMING_CRC_IRQ3  BIT(28)
#define WIE2R_ROAMING_LQ_IRQ3   BIT(27)
#define WIE2R_ROAMING_LQ_IRQ3_HI  BIT(26)
#define WIE2R_ROAMING_CRC_IRQ2  BIT(25)
#define WIE2R_ROAMING_LQ_IRQ2   BIT(24)
#define WIE2R_ROAMING_LQ_IRQ2_HI BIT(23)
#define WIE2R_RM23_INT_STATUS   BITS(23,28)
#define WIE2R_RMAC_TFT_ACI_IRQ  BIT(22)
#define WIE2R_ABNOR_TX_INT      BIT(21)
#define WIE2R_ARB_MDRDY_PPDUTIME BIT(20)
#define WIE2R_RMAC_TFT_ACI_IRQ  BIT(22)
#define WIE2R_ABNOR_TX_INT      BIT(21)
#define WIE2R_ARB_MDRDY_PPDUTIME BIT(20)
#define WIE2R_RM_CRC_IRQ0       BIT(19)
#define WIE2R_RM_LQ_IRQ0        BIT(18)
#define WIE2R_RM_LQ_IRQ0_HI     BIT(17)
#define WIE2R_RM_CRC_IRQ1       BIT(16)
#define WIE2R_RM_LQ_IRQ1        BIT(15)
#define WIE2R_RM_LQ_IRQ1_HI     BIT(14)
#define WIE2R_RM_INT_STATUS     BITS(14,19)
#define WIE2R_BMC_TIMEOUT3      BIT(13)
#define WIE2R_BCN_TIMEOUT3      BIT(12)
#define WIE2R_PRETBTT3          BIT(11)
#define WIE2R_TBTT3             BIT(10)
#define WIE2R_BTIM_BMC_TIMEOUT3 BIT(9)
#define WIE2R_BTIM_TIMEOUT3     BIT(8)
#define WIE2R_PRETTTT3          BIT(7)
#define WIE2R_TRAP1_INT         BIT(6)
#define WIE2R_TTTT3             BIT(5)
#define WIE2R_BTIM_BMC_TIMEOUT2 BIT(4)
#define WIE2R_BTIM_TIMEOUT2     BIT(3)
#define WIE2R_PRETTTT2          BIT(2)
#define WIE2R_TTTT2             BIT(1)
#define WIE2R_TIMEUP_CHK_LP     BIT(0)

#define WIE2R_ALL_INTS          BITS(0,31)

//WIE3R
#if 0
#define WIE3R_BTIM_BMC_TIMEOUT3 BIT(31)
#define WIE3R_BTIM_TIMEOUT3     BIT(30)
#define WIE3R_PRETTTT3          BIT(29)
#define WIE3R_TTTT3             BIT(28)
#define WIE3R_BTIM_BMC_TIMEOUT2 BIT(27)
#define WIE3R_BTIM_TIMEOUT2     BIT(26)
#define WIE3R_PRETTTT2          BIT(25)
#define WIE3R_TTTT2             BIT(24)
#define WIE3R_BTIM_BMC_TIMEOUT1 BIT(23)
#define WIE3R_BTIM_TIMEOUT1     BIT(22)
#define WIE3R_PRETTTT1          BIT(21)
#define WIE3R_TTTT1             BIT(20)
#define WIE3R_BTIM_BMC_TIMEOUT0 BIT(19)
#define WIE3R_BTIM_TIMEOUT0     BIT(18)
#define WIE3R_PRETTTT0          BIT(17)
#define WIE3R_TTTT0             BIT(16)
#define WIE3R_TRAP3_INT         BIT(15)
#define WIE3R_TRAP2_INT         BIT(14)
#define WIE3R_TSF3_DRIFT        BIT(13)
#define WIE3R_TSF2_DRIFT        BIT(12)
#define WIE3R_BMC_TIMEOUT3      BIT(11)
#define WIE3R_BCN_TIMEOUT3      BIT(10)
#define WIE3R_PRETBTT3          BIT(9)
#define WIE3R_TBTT3             BIT(8)
#define WIE3R_BMC_TIMEOUT2      BIT(7)
#define WIE3R_BCN_TIMEOUT2      BIT(6)
#define WIE3R_PRETBTT2          BIT(5)
#define WIE3R_TBTT2             BIT(4)
#define WIE3R_T7_TIME           BIT(3)
#define WIE3R_T6_TIME           BIT(2)
#define WIE3R_T5_TIME           BIT(1)
#define WIE3R_T4_TIME           BIT(0)

#define WIE3R_ALL_INTS          BITS(0,31)
#endif

/* WTBL */
#define WTBL_ENTRY0_BASE        (WIFI_WTBL_BASE + 0x0000)
#define WTBL1_ENTRY_OFFSET      (0x20)
#define WTBL2_ENTRY_OFFSET      (64)
#define WTBL34_ENTRY_OFFSET     (32)

#define WTBL_TBL1DW0            (WIFI_WTBL_BASE + 0x0000)
#define WTBL_TBL1DW1            (WIFI_WTBL_BASE + 0x0004)
#define WTBL_TBL1DW2            (WIFI_WTBL_BASE + 0x0008)
#define WTBL_TBL1DW3            (WIFI_WTBL_BASE + 0x000C)
#define WTBL_TBL1DW4            (WIFI_WTBL_BASE + 0x0010)

/* WTBL ON */
#define WTBL_RIUCR0             (WIFI_WTBL_BASE + 0x2200)
#define WTBL_RIUCR1             (WIFI_WTBL_BASE + 0x2204)
#define WTBL_RIUCR2             (WIFI_WTBL_BASE + 0x2208)
#define WTBL_RIUCR3             (WIFI_WTBL_BASE + 0x220C)
#define WTBL_WTBL1OR            (WIFI_WTBL_BASE + 0x2300)
#define WTBL_TCGSBR             (WIFI_WTBL_BASE + 0x2210)

/*WTBL OFF */

#define WTBL_WIUCR              (WIFI_WTBLOFF_TOP + 0x0000)
#define WTBL_BCR                (WIFI_WTBLOFF_TOP + 0x0004)
#define WTBL_RMVTCR             (WIFI_WTBLOFF_TOP + 0x0008)
#define WTBL_PCSR               (WIFI_WTBLOFF_TOP + 0x000C)
#define WTBL_PCR                (WIFI_WTBLOFF_TOP + 0x0010)
//#define WTBL_TCGSBR             (WIFI_WTBL_BASE + 0x4008)

//TBD
#define  WTBL_SGCR              (WIFI_WTBL_BASE + 0xFF08)
#define  WTBL_ARCR              (WIFI_WTBL_BASE + 0xFF0C)

#define WTBL_DWER              (WIFI_WTBL_BASE + 0xFF44)
#define WTBL_DWER2             (WIFI_WTBL_BASE + 0xFF48)

/* BATBL JSWAN */
#define BATBL_ENTRY0_BASE       (WIFI_WTBL_BASE + 0x8000)
#define BA0_CR                  (BATBL_ENTRY0_BASE + 0x0000)
    #define     BA_AGING_LIMIT_MASK         BITS(0,15)
    #define     BA_AGING_LIMIT_OFFSET       0
    #define     BA_WLAN_IDX_MASK            BITS(16,23)
    #define     BA_WLAN_IDX_OFFSET          16
    #define     BA_TID_MASK                 BITS(28,30)
    #define     BA_TID_OFFSET               28
    #define     BA_EN_MASK                  BIT(31)
    #define     BA_EN_OFFSET                31

#define BA0_SR                  (BATBL_ENTRY0_BASE + 0x0004)
    #define     BA_SSN_MASK                 BITS(0,11)
    #define     BA_SSN_OFFSET               0
    #define     BA_WIN_SIZE_MASK            BITS(16,21)
    #define     BA_WIN_SIZE_OFFSET          16
    #define     BA_START_MASK               BIT(24)
    #define     BA_START_OFFSET             24

#define BA0_AL                  (BATBL_ENTRY0_BASE + 0x0008)
#define BA0_AH                  (BATBL_ENTRY0_BASE + 0x000C)
#define BA_ENTRY_SIZE           (0x10)

#define BA_TSR                  (WIFI_WTBL_BASE + 0xFF00)

/* CONFIG */
#define  CONFG_CCR              (WIFI_CONFG_BASE + 0x000)
//#define  CONFG_LPCR         (WIFI_CONFG_BASE + 0x0004)
//#define  CONFG_LPCR_EN      (WIFI_CONFG_BASE + 0x0008)
//#define  CONFG_LPCR_DIS     (WIFI_CONFG_BASE + 0x000C)
//#define  CONFG_DCR          (WIFI_CONFG_BASE + 0x0010)
//#define  CONFG_DBG_SEL0     (WIFI_CONFG_BASE + 0x0014)
//#define  CONFG_DBG_SEL1     (WIFI_CONFG_BASE + 0x0018)
//#define  CONFG_DBG_SEL2     (WIFI_CONFG_BASE + 0x001C)
//#define  CONFG_DBG_SEL3     (WIFI_CONFG_BASE + 0x0020)
#define  CNFG_SWRST_EN          (WIFI_CONFG_BASE + 0x0004)
#define  CONFG_DBG_EN           (WIFI_CONFG_BASE + 0x0014)
#define  CONFG_DBG_L1_MODSEL    (WIFI_CONFG_BASE + 0x0018)
#define  CONFG_DBG_L2_SEL       (WIFI_CONFG_BASE + 0x001C)
#define  CONFG_DBG_L2_MODSEL    (WIFI_CONFG_BASE + 0x0020)
#define  CONFG_DBG0             (WIFI_CONFG_BASE + 0x0024)
#define  CONFG_DBG1             (WIFI_CONFG_BASE + 0x0028)
#define  CONFG_CNTR_SEL0        (WIFI_CONFG_BASE + 0x002C)
#define  CONFG_CNTR_SEL1        (WIFI_CONFG_BASE + 0x0030)
#define  CONFG_CNTR_CTRL        (WIFI_CONFG_BASE + 0x0034)
#define  CONFG_CNTR_NUM         (WIFI_CONFG_BASE + 0x0038)
#define  CONFG_CR0              (WIFI_CONFG_BASE + 0x003C)
#define  CONFG_CR1              (WIFI_CONFG_BASE + 0x0040)
#define  CONFG_CR2              (WIFI_CONFG_BASE + 0x0044)
#define  CONFG_CR3              (WIFI_CONFG_BASE + 0x0048)
//#define  CONFG_RCPIR        (WIFI_CONFG_BASE + 0x004C)

//#define CONFIG_PLL_CR       (WIFI_CONFG_BASE + 0x0070)
#define  CONFG_L1_SEL           (WIFI_CONFG_BASE + 0x004C)
#define CONFIG_DBG_FLAG_SEL      CONFG_L1_SEL

#define  CONFG_LED0_CTRL        (WIFI_CONFG_BASE + 0x0050)
#define  CONFG_LED0_PR_S0       (WIFI_CONFG_BASE + 0x0054)
#define  CONFG_LED0_PR_S1       (WIFI_CONFG_BASE + 0x0058)

#define  CONFG_RCPI0            (WIFI_CONFG_BASE + 0x00A0)
#define  CONFG_RCPI2            (WIFI_CONFG_BASE + 0x00A8)
#define  CONFG_RCPI3            (WIFI_CONFG_BASE + 0x00AC)
#define  CONFG_RCPI4            (WIFI_CONFG_BASE + 0x00B0)
#define  CONFG_RCPI6            (WIFI_CONFG_BASE + 0x00B8)
#define  CONFG_RCPI7            (WIFI_CONFG_BASE + 0x00BC)
#define  CONFG_RCPI9            (WIFI_CONFG_BASE + 0x00C4)
#define  CONFG_RCPI11           (WIFI_CONFG_BASE + 0x00CC)
#define  CONFG_LQR0             (WIFI_CONFG_BASE + 0x00D0)
#define  CONFG_LQR1             (WIFI_CONFG_BASE + 0x00D4)
#define  CONFG_DBG1_LQ_AVG      (WIFI_CONFG_BASE + 0x00D8)
#define  CONFG_DBG2_LQ_AVG      (WIFI_CONFG_BASE + 0x00DC)
#define  CONFG_LQR2             (WIFI_CONFG_BASE + 0x00E0)
#define  CONFG_LQR3             (WIFI_CONFG_BASE + 0x00E4)

#define  CONFG_LQ_BUF_CFG       (WIFI_CONFG_BASE + 0x00EC)
#define  CONFG_LQR4             (WIFI_CONFG_BASE + 0x00F0)
#define  CONFG_LQR5             (WIFI_CONFG_BASE + 0x00F4)
#define  CONFG_LQR6             (WIFI_CONFG_BASE + 0x00F8)
#define  CONFG_LQR7             (WIFI_CONFG_BASE + 0x00FC)
#define  CONFG_DBG3_LQ_AVG      (WIFI_CONFG_BASE + 0x0100)
#define  CONFG_DBG4_LQ_AVG      (WIFI_CONFG_BASE + 0x0104)
#define  CONFG_LQR8             (WIFI_CONFG_BASE + 0x0108)
#define  CONFG_LQR9             (WIFI_CONFG_BASE + 0x010C)
#define  CONFG_DBG6_LQ_BUF      (WIFI_CONFG_BASE + 0x0110)
#define  CONFG_LQR10            (WIFI_CONFG_BASE + 0x0114)
#define  CONFG_LQR11            (WIFI_CONFG_BASE + 0x0118)

#define  CONFG_LQBCR            (WIFI_CONFG_BASE + 0x0120)
#define  CONFG_LQBPCR           (WIFI_CONFG_BASE + 0x0124)
#define  CONFG_LQBBUF0R         (WIFI_CONFG_BASE + 0x0128)
#define  CONFG_LQBBUF1R         (WIFI_CONFG_BASE + 0x012C)
#define  CONFG_LQBBUF2R         (WIFI_CONFG_BASE + 0x0130)
#define  CONFG_LQBBUF3R         (WIFI_CONFG_BASE + 0x0134)
#define  CONFG_LQCCK0R          (WIFI_CONFG_BASE + 0x0138)
#define  CONFG_LQCCK1R          (WIFI_CONFG_BASE + 0x013C)
#define  CONFG_LQCCK2R          (WIFI_CONFG_BASE + 0x0140)
#define  CONFG_LQCCK3R          (WIFI_CONFG_BASE + 0x0144)
#define  CONFG_LQOFDM0R         (WIFI_CONFG_BASE + 0x0148)
#define  CONFG_LQOFDM1R         (WIFI_CONFG_BASE + 0x014C)
#define  CONFG_LQOFDM2R         (WIFI_CONFG_BASE + 0x0150)
#define  CONFG_LQOFDM3R         (WIFI_CONFG_BASE + 0x0154)
#define  CONFG_LQRXCR0          (WIFI_CONFG_BASE + 0x0158)
#define  CONFG_LQRXCR1          (WIFI_CONFG_BASE + 0x015C)
#define  CONFG_LQRXCR2          (WIFI_CONFG_BASE + 0x0160)
#define  CONFG_LQRXCR3          (WIFI_CONFG_BASE + 0x0164)
#define  CONFG_LQRXBUF0         (WIFI_CONFG_BASE + 0x0168)
#define  CONFG_LQRXBUF1         (WIFI_CONFG_BASE + 0x016C)
#define  CONFG_LQRXBUF2         (WIFI_CONFG_BASE + 0x0170)
#define  CONFG_LQRXBUF3         (WIFI_CONFG_BASE + 0x0174)
#define  CONFG_LQRXHBUF0        (WIFI_CONFG_BASE + 0x0178)
#define  CONFG_LQRXHBUF1        (WIFI_CONFG_BASE + 0x017C)
#define  CONFG_LQRXHBUF2        (WIFI_CONFG_BASE + 0x0180)
#define  CONFG_LQRXHBUF3        (WIFI_CONFG_BASE + 0x0184)
#define  CONFG_RCPI_CCK0        (WIFI_CONFG_BASE + 0x0188)
#define  CONFG_RCPI_CCK1        (WIFI_CONFG_BASE + 0x018C)
#define  CONFG_RCPI_CCK2        (WIFI_CONFG_BASE + 0x0190)
#define  CONFG_RCPI_CCK3        (WIFI_CONFG_BASE + 0x0194)
#define  CONFG_RCPI_OFDM0       (WIFI_CONFG_BASE + 0x0198)
#define  CONFG_RCPI_OFDM1       (WIFI_CONFG_BASE + 0x019C)
#define  CONFG_RCPI_OFDM2       (WIFI_CONFG_BASE + 0x01A0)
#define  CONFG_RCPI_OFDM3       (WIFI_CONFG_BASE + 0x01A4)
#define  CONFG_RCPI_LQ_MISC     (WIFI_CONFG_BASE + 0x01A8)

#define  CONFG_LQ1BCR           (WIFI_CONFG_BASE + 0x01AC)      //Related to LQ0 0x120
#define  CONFG_LQ1BPCR          (WIFI_CONFG_BASE + 0x01B0)      //Related to LQ0 0x124
#define  CONFG_LQ1BBUF0R        (WIFI_CONFG_BASE + 0x01B4)      //Related to LQ0 0x128
#define  CONFG_LQ1BBUF1R        (WIFI_CONFG_BASE + 0x01B8)      //Related to LQ0 0x12C
#define  CONFG_LQ1BBUF2R        (WIFI_CONFG_BASE + 0x01BC)      //Related to LQ0 0x130
#define  CONFG_LQ1BBUF3R        (WIFI_CONFG_BASE + 0x01C0)      //Related to LQ0 0x134
#define  CONFG_LQ1CCK0R         (WIFI_CONFG_BASE + 0x01C4)      //Related to LQ0 0x138
#define  CONFG_LQ1CCK1R         (WIFI_CONFG_BASE + 0x01C8)      //Related to LQ0 0x13C
#define  CONFG_LQ1CCK2R         (WIFI_CONFG_BASE + 0x01CC)      //Related to LQ0 0x140
#define  CONFG_LQ1CCK3R         (WIFI_CONFG_BASE + 0x01D0)      //Related to LQ0 0x144
#define  CONFG_LQ1OFDM0R        (WIFI_CONFG_BASE + 0x01D4)      //Related to LQ0 0x148
#define  CONFG_LQ1OFDM1R        (WIFI_CONFG_BASE + 0x01D8)      //Related to LQ0 0x14C
#define  CONFG_LQ1OFDM2R        (WIFI_CONFG_BASE + 0x01DC)      //Related to LQ0 0x150
#define  CONFG_LQ1OFDM3R        (WIFI_CONFG_BASE + 0x01E0)      //Related to LQ0 0x154
#define  CONFG_LQ1RXCR0         (WIFI_CONFG_BASE + 0x01E4)      //Related to LQ0 0x158
#define  CONFG_LQ1RXCR1         (WIFI_CONFG_BASE + 0x01E8)      //Related to LQ0 0x15C
#define  CONFG_LQ1RXCR2         (WIFI_CONFG_BASE + 0x01EC)      //Related to LQ0 0x160
#define  CONFG_LQ1RXCR3         (WIFI_CONFG_BASE + 0x01F0)      //Related to LQ0 0x164
#define  CONFG_LQ1RXBUF0        (WIFI_CONFG_BASE + 0x01F4)      //Related to LQ0 0x168
#define  CONFG_LQ1RXBUF1        (WIFI_CONFG_BASE + 0x01F8)      //Related to LQ0 0x16C
#define  CONFG_LQ1RXBUF2        (WIFI_CONFG_BASE + 0x01FC)      //Related to LQ0 0x170
#define  CONFG_LQ1RXBUF3        (WIFI_CONFG_BASE + 0x0200)      //Related to LQ0 0x174
#define  CONFG_LQ1RXHBUF0       (WIFI_CONFG_BASE + 0x0204)      //Related to LQ0 0x178
#define  CONFG_LQ1RXHBUF1       (WIFI_CONFG_BASE + 0x0208)      //Related to LQ0 0x17C
#define  CONFG_LQ1RXHBUF2       (WIFI_CONFG_BASE + 0x020C)      //Related to LQ0 0x180
#define  CONFG_LQ1RXHBUF3       (WIFI_CONFG_BASE + 0x0210)      //Related to LQ0 0x184
#define  CONFG_LQ1_MISC         (WIFI_CONFG_BASE + 0x0214)      //Related to LQ0 0x1A8


#define CONFG_WO_SWRST_EN      (WIFI_CONFG_OFF_BASE + 0x0000)
#define CONFG_TMTCSR           (WIFI_CONFG_OFF_BASE + 0x0008)
#define CONFG_TMTDR            (WIFI_CONFG_OFF_BASE + 0x000C)
#define CONFG_TMRCSR           (WIFI_CONFG_OFF_BASE + 0x0010)
#define CONFG_TMRDR            (WIFI_CONFG_OFF_BASE + 0x0014)
#define CONFG_TMRDTR           (WIFI_CONFG_OFF_BASE + 0x0018)



#if 0 //not define in CFG TOP spec. V0.4
#define CONFIG_HW_SRAM_PWR_CTRL     (WIFI_CONFG_BASE + 0x00B0)
#define CONFIG_HW_SRAM_SLEEP_CTRL   (WIFI_CONFG_BASE + 0x00B4)
#define CONFIG_HW_TCM_PWR_CTRL      (WIFI_CONFG_BASE + 0x00B8)
#define CONFIG_HW_TCM_SLEEP_CTRL    (WIFI_CONFG_BASE + 0x00BC)
#define CONFIG_SRAM_PON             (WIFI_CONFG_BASE + 0x00C0)
#define CONFIG_SRAM_PDN             (WIFI_CONFG_BASE + 0x00C4)
#define CONFIG_SRAM_PDN_STAT        (WIFI_CONFG_BASE + 0x00C8)
#define CONFIG_SRAM_RESET           (WIFI_CONFG_BASE + 0x00CC)
#define CONFIG_SRAM_RESET_STAT      (WIFI_CONFG_BASE + 0x00D0)
#define CONFIG_SRAM_WAKEUP          (WIFI_CONFG_BASE + 0x00D4)
#define CONFIG_SRAM_SLEEP           (WIFI_CONFG_BASE + 0x00D8)
#define CONFIG_SRAM_SLEEP_STAT      (WIFI_CONFG_BASE + 0x00DC)
#define CONFIG_TCM_PON              (WIFI_CONFG_BASE + 0x00E0)
#define CONFIG_TCM_PDN              (WIFI_CONFG_BASE + 0x00E4)
#define CONFIG_TCM_PDN_STAT         (WIFI_CONFG_BASE + 0x00E8)
#define CONFIG_TCM_RESET            (WIFI_CONFG_BASE + 0x00EC)
#define CONFIG_TCM_RESET_STAT       (WIFI_CONFG_BASE + 0x00F0)
#define CONFIG_TCM_WAKEUP           (WIFI_CONFG_BASE + 0x00F4)
#define CONFIG_TCM_SLEEP            (WIFI_CONFG_BASE + 0x00F8)
#define CONFIG_TCM_SLEEP_STAT       (WIFI_CONFG_BASE + 0x00FC)

#define CONFIG_WF_ANT_SEL           (WIFI_CONFG_BASE + 0x0150)
#define     WLAN_ACT_SLP_VALUE                  BIT(6)
#define     WLAN_ACT_SLP_VALUE_OFFSET           6
#define     BT_PRI_PD_SLP_VALUE                 BIT(5)
#define     BT_PRI_PD_SLP_VALUE_OFFSET          5
#define     BT_PRI_PU_SLP_VALUE                 BIT(4)
#define     BT_PRI_PU_SLP_VALUE_OFFSET          4
#define     ANT_SEL1_MODE                       BITS(2,3)
#define     ANT_SEL1_MODE_OFFSET                2
#define     ANT_SEL0_MODE                       BITS(0,1)
#define     ANT_SEL0_MODE_OFFSET                0
#endif

/* TRB */
#define  TRB_GCR                    (WIFI_TRB_BASE + 0x0000)
#define  TRB_TXPTHR                 (WIFI_TRB_BASE + 0x0004)
#define  TRB_RXPTHR                 (WIFI_TRB_BASE + 0x0008)
#define  TRB_TXTMR                  (WIFI_TRB_BASE + 0x000C)
#define  TRB_RXTMR                  (WIFI_TRB_BASE + 0x0010)
#define  TRB_TXBSR                  (WIFI_TRB_BASE + 0x0020)
#define  TRB_TXPSR                  (WIFI_TRB_BASE + 0x0024)
#define  TRB_TXDCSR0                (WIFI_TRB_BASE + 0x0028)
#define  TRB_TXDCSR1                (WIFI_TRB_BASE + 0x002C)
#define  TRB_TXPNSR                 (WIFI_TRB_BASE + 0x0030)
#define  TRB_RXBSR                  (WIFI_TRB_BASE + 0x0040)
#define  TRB_RXPSR0                 (WIFI_TRB_BASE + 0x0044)
#define  TRB_RXPSR1                 (WIFI_TRB_BASE + 0x0048)
#define  TRB_RXPSR2                 (WIFI_TRB_BASE + 0x004C)
#define  TRB_RXDCSR0                (WIFI_TRB_BASE + 0x0050)
#define  TRB_RXDCSR1                (WIFI_TRB_BASE + 0x0054)
#define  TRB_RXPSISR0               (WIFI_TRB_BASE + 0x0058)
#define  TRB_RXPSISR1               (WIFI_TRB_BASE + 0x005C)
#define  TRB_RXPNSR0                (WIFI_TRB_BASE + 0x0060)
#define  TRB_RXPNSR1                (WIFI_TRB_BASE + 0x0064)
#define  TRB_LPIWPR                 (WIFI_TRB_BASE + 0x0070)
#define  TRB_LPIRPR                 (WIFI_TRB_BASE + 0x0074)
#define  TRB_LPIDR                  (WIFI_TRB_BASE + 0x0078)
#define  TRB_TXDUMR0                (WIFI_TRB_BASE + 0x0080)
#define  TRB_TXDUMR1                (WIFI_TRB_BASE + 0x0084)
#define  TRB_RXDUMR0                (WIFI_TRB_BASE + 0x0088)
#define  TRB_RXDUMR1                (WIFI_TRB_BASE + 0x008C)

/* AGG */
#define AGG_TPCR                 (WIFI_AGG_BASE + 0x0000)
#define AGG_TPCR_TXOP_PS_EN_MASK    BITS(0,3)
#define AGG_TPCR_TXOP_PS_FORCE_MD   BIT(15)
#define AGG_ARCR                 (WIFI_AGG_BASE + 0x0010)
#define AGG_ARCR_INIT_RATE1         BIT(0)
#define AGG_ARCR_FW_SGI_DIS         BIT(1)
#define AGG_ARCR_FW_SGI_DIS_OFFSET       1

#define AGG_ARCR_RATE8_DOWN_CTRL    BIT(2)
#define AGG_ARCR_RATE8_DOWN_CTRL_OFFSET 2
#define AGG_ARCR_RETRY_LIMIT         BITS(4,5)
#define AGG_ARCR_RETRY_LIMIT_OFFSET   4


#define AGG_ARUCR                (WIFI_AGG_BASE + 0x0014)
#define AGG_ARDCR                (WIFI_AGG_BASE + 0x0018)
#define AGG_FRM1RR0              (WIFI_AGG_BASE + 0x0020)
#define AGG_FRM1RR1              (WIFI_AGG_BASE + 0x0024)
#define AGG_FRM1RR2              (WIFI_AGG_BASE + 0x0028)
#define AGG_FRM1RR3              (WIFI_AGG_BASE + 0x002C)
#define AGG_FRM1RR4              (WIFI_AGG_BASE + 0x0030)
#define AGG_AALCR                (WIFI_AGG_BASE + 0x0040)
#define AGG_AALCR1               (WIFI_AGG_BASE + 0x0044)
#define AGG_AWSCR                (WIFI_AGG_BASE + 0x0048)
#define AGG_AWSCR1               (WIFI_AGG_BASE + 0x004C)
#define AGG_PCR                  (WIFI_AGG_BASE + 0x0050)
#define AGG_PCR1                 (WIFI_AGG_BASE + 0x0054)
#define AGG_ASRCR                (WIFI_AGG_BASE + 0x0060)
#define AGG_BQCR                 (WIFI_AGG_BASE + 0x0064)
#define AGG_BCR                  (WIFI_AGG_BASE + 0x0068)
#define AGG_ACR                  (WIFI_AGG_BASE + 0x0070)
#define AGG_ACR1                 (WIFI_AGG_BASE + 0x0074)
#define AGG_DSCR0                (WIFI_AGG_BASE + 0x00B0)   //Posh 20131212, Add DMA scheduler
#define AGG_DSCR1                (WIFI_AGG_BASE + 0x00B4)   //Posh 20131212, Add DMA scheduler
#define AGG_DSIRR                (WIFI_AGG_BASE + 0x00B8)   //Posh 20131212, Add DMA scheduler
#define AGG_SCR                  (WIFI_AGG_BASE + 0x00D8)
#define AGG_SCR_DROP_DST_PID        BITS(8,9)
#define AGG_SCR_DROP_DST_PID_OFFSET 8
#define AGG_SCR_DROP_DST_QID        BITS(10,14)
#define AGG_SCR_DROP_DST_QID_OFFSET 10
#define AGG_SMCR                 (WIFI_AGG_BASE + 0x00DC)
#define AGG_BWCR                 (WIFI_AGG_BASE + 0x00EC)
#define AGG_MRCR                 (WIFI_AGG_BASE + 0x00F4)


/***** ARB *****/
#define ARB_GTQR0                 (WIFI_ARB_BASE + 0x0010)
#define ARB_GTQR1                 (WIFI_ARB_BASE + 0x0014)
#define ARB_GTQR2                 (WIFI_ARB_BASE + 0x0018)
#define ARB_GTQR3                 (WIFI_ARB_BASE + 0x001C)
#define  ARB_AIFSR0               (WIFI_ARB_BASE + 0x0020)
#define ARB_AIFSR                   ARB_AIFSR0
#define  ARB_AIFSR1               (WIFI_ARB_BASE + 0x0024)
#define  ARB_ACCWXR0            (WIFI_ARB_BASE + 0x0028)
#define  ARB_ACCWXR1            (WIFI_ARB_BASE + 0x002C)
#define  ARB_ACCWXR2            (WIFI_ARB_BASE + 0x0030)
#define  ARB_ACCWXR3            (WIFI_ARB_BASE + 0x0034)
#define  ARB_ACCWXR4            (WIFI_ARB_BASE + 0x0038)
#define  ARB_ACCWXR5            (WIFI_ARB_BASE + 0x003C)
#define  ARB_ACCWIR0            (WIFI_ARB_BASE + 0x0040)
#define  ARB_ACCWIR1            (WIFI_ARB_BASE + 0x0044)
#define  ARB_ACCWIR2            (WIFI_ARB_BASE + 0x0048)
#define  ARB_ACCWIR3            (WIFI_ARB_BASE + 0x004C)
#define ARB_ACCCWR0          (WIFI_ARB_BASE + 0x0050)
#define ARB_ACCCWR1          (WIFI_ARB_BASE + 0x0054)
#define ARB_ACCCWR2          (WIFI_ARB_BASE + 0x0058)
#define ARB_ACCCWR3          (WIFI_ARB_BASE + 0x005C)
#define ARB_ACCCWR4          (WIFI_ARB_BASE + 0x0060)
#define ARB_ACCCWR5          (WIFI_ARB_BASE + 0x0064)
#define ARB_DRNGR               (WIFI_ARB_BASE + 0x0068)
#define  ARB_RQCR               (WIFI_ARB_BASE + 0x0070)
#define  ARB_SCR                (WIFI_ARB_BASE + 0x0080)
#define ARB_TQCR0                (WIFI_ARB_BASE + 0x0100)
#define ARB_TQCR1                (WIFI_ARB_BASE + 0x0104)
#define ARB_TQCR2                (WIFI_ARB_BASE + 0x0108)
#define ARB_TQCR3                (WIFI_ARB_BASE + 0x010C)
#define ARB_TQCR4                (WIFI_ARB_BASE + 0x0110)
#define ARB_TQCR5                (WIFI_ARB_BASE + 0x0114)
#define ARB_BCNQCR0              (WIFI_ARB_BASE + 0x0118)
#define ARB_BCNQCR1              (WIFI_ARB_BASE + 0x011C)
#define ARB_BMCQCR0              (WIFI_ARB_BASE + 0x0120)
#define ARB_BMCQCR1              (WIFI_ARB_BASE + 0x0124)
#define ARB_BMCQCR2              (WIFI_ARB_BASE + 0x0128)
#define ARB_BMCQCR3              (WIFI_ARB_BASE + 0x012C)
#define ARB_BMCQCR4              (WIFI_ARB_BASE + 0x0130)
#define ARB_TXENTRYR             (WIFI_ARB_BASE + 0x0134)
#define ARB_BTIMCR0              (WIFI_ARB_BASE + 0x0138)
#define ARB_BTIMCR1              (WIFI_ARB_BASE + 0x013C)

/***** MIB Counter *****/
#define ARB_MSCR0              (WIFI_MIB_BASE + 0x0000)
#define ARB_PBSCR              (WIFI_MIB_BASE + 0x0004)
#define ARB_MSCR1              (WIFI_MIB_BASE + 0x0008)

#define ARB_MSDR0              (WIFI_MIB_BASE + 0x0010)
//#define ARB_MSDR1              (WIFI_MIB_BASE + 0x000C)
//#define ARB_MSDR2              (WIFI_MIB_BASE + 0x0010)
//#define ARB_MSDR3              (WIFI_MIB_BASE + 0x0014)
#define ARB_MSDR4              (WIFI_MIB_BASE + 0x0018)
#define ARB_MSDR5              (WIFI_MIB_BASE + 0x001C)
#define ARB_MSDR6              (WIFI_MIB_BASE + 0x0020)
#define ARB_MSDR7              (WIFI_MIB_BASE + 0x0024)
#define ARB_MSDR8              (WIFI_MIB_BASE + 0x0028)
#define ARB_MSDR9              (WIFI_MIB_BASE + 0x002C)
#define ARB_MSDR10             (WIFI_MIB_BASE + 0x0030)
#define ARB_MSDR11             (WIFI_MIB_BASE + 0x0034)
#define ARB_MSDR12              (WIFI_MIB_BASE + 0x0038)
#define ARB_MSDR13              (WIFI_MIB_BASE + 0x003C)
#define ARB_MSDR14              (WIFI_MIB_BASE + 0x0040)
#define ARB_MSDR15              (WIFI_MIB_BASE + 0x0044)
#define ARB_MSDR16              (WIFI_MIB_BASE + 0x0048)
#define ARB_MSDR17              (WIFI_MIB_BASE + 0x004C)
#define ARB_MSDR18              (WIFI_MIB_BASE + 0x0050)
#define ARB_MSDR19              (WIFI_MIB_BASE + 0x0054)
#define ARB_MSDR20              (WIFI_MIB_BASE + 0x0058)
#define ARB_MSDR21              (WIFI_MIB_BASE + 0x005C)
#define ARB_MSDR22             (WIFI_MIB_BASE + 0x0060)	//RX_AMPDU_COUNT
#define ARB_MSDR23             (WIFI_MIB_BASE + 0x0064)	//RX_TOTBYTE_COUNT
#define ARB_MSDR24             (WIFI_MIB_BASE + 0x0068)	//RX_VALIDSF_COUNT
#define ARB_MSDR25             (WIFI_MIB_BASE + 0x006C)	//RX_VALIDBYTE_COUNT
#define ARB_MSDR26             (WIFI_MIB_BASE + 0x0070)
#define ARB_MSDR27             (WIFI_MIB_BASE + 0x0074)
#define ARB_MSDR28             (WIFI_MIB_BASE + 0x0078)
#define ARB_MSDR29             (WIFI_MIB_BASE + 0x007C)
#define ARB_MSDR30              (WIFI_MIB_BASE + 0x0080)
#define ARB_MSDR31              (WIFI_MIB_BASE + 0x0084)


#define ARB_MDR0                (WIFI_MIB_BASE + 0x00A0)
#define ARB_MDR1                (WIFI_MIB_BASE + 0x00A4)
#define ARB_MDR2                (WIFI_MIB_BASE + 0x00A8)
#define ARB_MDR3                (WIFI_MIB_BASE + 0x00AC)

#define ARB_MB0SDR0             (WIFI_MIB_BASE + 0x0100)
#define ARB_MB0SDR1             (WIFI_MIB_BASE + 0x0104)
#define ARB_MB0SDR2             (WIFI_MIB_BASE + 0x0108)
#define ARB_MB0SDR3             (WIFI_MIB_BASE + 0x010C)    //Posh 20131208, Add MIB retry 3 count
#define ARB_MB1SDR0             (WIFI_MIB_BASE + 0x0110)
#define ARB_MB1SDR1             (WIFI_MIB_BASE + 0x0114)
#define ARB_MB1SDR2             (WIFI_MIB_BASE + 0x0118)
#define ARB_MB1SDR3             (WIFI_MIB_BASE + 0x011C)    //Posh 20131208, Add MIB retry 3 count
#define ARB_MB2SDR0             (WIFI_MIB_BASE + 0x0120)
#define ARB_MB2SDR1             (WIFI_MIB_BASE + 0x0124)
#define ARB_MB2SDR2             (WIFI_MIB_BASE + 0x0128)
#define ARB_MB2SDR3             (WIFI_MIB_BASE + 0x012C)    //Posh 20131208, Add MIB retry 3 count
#define ARB_MB3SDR0             (WIFI_MIB_BASE + 0x0130)
#define ARB_MB3SDR1             (WIFI_MIB_BASE + 0x0134)
#define ARB_MB3SDR2             (WIFI_MIB_BASE + 0x0138)
#define ARB_MB3SDR3             (WIFI_MIB_BASE + 0x013C)    //Posh 20131208, Add MIB retry 3 count

//Per BSS TX/RX Counter
#define MIB_TX_CNT_BASE         (WIFI_WTBL_BASE + 0x2000)
#define MIB_TX_BYTE_CNT_BASE    (WIFI_WTBL_BASE + 0x2030)
#define MIB_RX_CNT_BASE         (WIFI_WTBL_BASE + 0x2100)
#define MIB_RX_BYTE_CNT_BASE    (WIFI_WTBL_BASE + 0x2130)


#if 0  //pat removed in MIB CR spec. 0.4
#define ARB_MSDR29             (WIFI_MIB_BASE + 0x007C)
#define ARB_MSDR30             (WIFI_MIB_BASE + 0x0080)
#define ARB_MSDR31             (WIFI_MIB_BASE + 0x0084)
#define ARB_MSDR32             (WIFI_MIB_BASE + 0x0088)
#define ARB_MSDR33             (WIFI_MIB_BASE + 0x008C)
#define ARB_MSDR34             (WIFI_MIB_BASE + 0x0090)
#endif

/* Coexistence CR start */
#define ARB_SXCR0              (WIFI_ARB_BASE + 0x0084)
    #define FIELD_BT_PRE_REQ_TIME                   BITS(24, 29)
    #define FIELD_BT_PRE_REQ_TIME_OFFSET            24
    #define FIELD_SW_VERIFY_CNT_EN                  BIT(8)
    #define FIELD_SW_VERIFY_CNT_EN_OFFSET           8
    #define FIELD_AC_REQ_EXTRA_MODE                 BIT(7)
    #define FIELD_AC_REQ_EXTRA_MODE_OFFSET          7
    #define FIELD_AC_REQ_EXTRA_DIS                  BIT(6)
    #define FIELD_AC_REQ_EXTRA_DIS_OFFSET           6
    #define FIELD_AGG_SX_READY_MODE                 BIT(5)
    #define FIELD_AGG_SX_READY_MODE_OFFSET          5
    #define FIELD_TX_SX_ABORT_MODE                  BIT(4)
    #define FIELD_TX_SX_ABORT_MODE_OFFSET           4
    #define FIELD_SW_PTA_SX_SEL                     BIT(1)
    #define FIELD_SW_PTA_SX_SEL_OFFSET              1
    #define FIELD_PTA_SX_MAN_MODE                   BIT(0)
    #define FIELD_PTA_SX_MAN_MODE_OFFSET            0
#define ARB_SXCR1              (WIFI_ARB_BASE + 0x0088)
    #define FIELD_SX_CCA_SETTLE_TIME                BITS(24, 29)
    #define FIELD_SX_CCA_SETTLE_TIME_OFFSET         24
    #define FIELD_SX_PRE_SETTLE_TIME                BITS(16, 21)
    #define FIELD_SX_PRE_SETTLE_TIME_OFFSET         16
    #define FIELD_SX_SETTLE_TIME                    BITS(8, 13)
    #define FIELD_SX_SETTLE_TIME_OFFSET             8
    #define FIELD_SX_SETTLE_CNT_EN                  BIT(7)
    #define FIELD_SX_SETTLE_CNT_EN_OFFSET           7
    #define FIELD_SX_SETTLE_CNT_RST                 BIT(6)
    #define FIELD_SX_SETTLE_CNT_RST_OFFSET          6
    #define FIELD_REQ_BF_TH                         BITS(0, 3)
    #define FIELD_REQ_BF_TH_OFFSET                  0
#define ARB_SXCR2              (WIFI_ARB_BASE + 0x008C)
    #define FIELD_DIG_PTA_SX_SEL                    BIT(30)
    #define FIELD_DIG_PTA_SX_SEL_OFFSET             30
    #define FIELD_SX_SETTLE_CNT                     BITS(24, 29)
    #define FIELD_SX_SETTLE_CNT_OFFSET              24
    #define FIELD_WF_SX_CCA_READY                   BIT(23)
    #define FIELD_WF_SX_CCA_READY_OFFSET            23
    #define FIELD_WF_SX_PRE_READY                   BIT(22)
    #define FIELD_WF_SX_PRE_READY_OFFSET            22
    #define FIELD_WF_SX_READY                       BIT(21)
    #define FIELD_WF_SX_READY_OFFSET                21
    #define FIELD_PTA_SX_SEL_SYNC                   BIT(20)
    #define FIELD_PTA_SX_SEL_SYNC_OFFSET            20
    #define FIELD_WF_TX_ABORT_CNT                   BITS(12, 15)
    #define FIELD_WF_TX_ABORT_CNT_OFFSET            12
    #define FIELD_WF_REQ_TIME_RST                   BIT(8)
    #define FIELD_WF_REQ_TIME_RST_OFFSET            8
    #define FIELD_WF_REQ_TIME_CNT                   BITS(0, 7)
    #define FIELD_WF_REQ_TIME_CNT_OFFSET            0
#define ARB_WPTAR              (WIFI_ARB_BASE + 0x0090)
    #define FIELD_SILENCE_WIN_AIFS_EN               BIT(28)
    #define FIELD_SILENCE_WIN_AIFS_EN_OFFSET        28
    #define FIELD_SILENCE_WIN_PS_EN                 BITS(24, 27)
    #define FIELD_SILENCE_WIN_PS_EN_OFFSET          24
    #define FIELD_PTA_RW_MODE                       BITS(16, 20)
    #define FIELD_PTA_RW_MODE_OFFSET                16
    #define FIELD_LEGACY_NULL                       BITS(12, 15)
    #define FIELD_LEGACY_NULL_OFFSET                12
    #define FIELD_PTA_TXOP_KEEP_EN                  BIT(9)
    #define FIELD_PTA_TXOP_KEEP_EN_OFFSET           9
    #define FIELD_PTA_ARB_MODE                      BIT(8)
    #define FIELD_PTA_ARB_MODE_OFFSET               8
    #define FIELD_PTA_LTE_RW_EN                     BIT(2)
    #define FIELD_PTA_LTE_RW_EN_OFFSET              2
    #define FIELD_PTA_BT_RW_EN                      BIT(1)
    #define FIELD_PTA_BT_RW_EN_OFFSET               1
    #define FIELD_PTA_ARB_EN                        BIT(0)
    #define FIELD_PTA_ARB_EN_OFFSET                 0
#define ARB_PTR0               (WIFI_ARB_BASE + 0x0094)
    #define FIELD_WLAN_BMC_TX_TAG                   BITS(28, 31)
    #define FIELD_WLAN_BMC_TX_TAG_OFFSET            28
    #define FIELD_WLAN_AC6_TX_TAG                   BITS(24, 27)
    #define FIELD_WLAN_AC6_TX_TAG_OFFSET            24
    #define FIELD_WLAN_AC5_TX_TAG                   BITS(20, 23)
    #define FIELD_WLAN_AC5_TX_TAG_OFFSET            20
    #define FIELD_WLAN_AC4_TX_TAG                   BITS(16, 19)
    #define FIELD_WLAN_AC4_TX_TAG_OFFSET            16
    #define FIELD_WLAN_AC3_TX_TAG                   BITS(12, 15)
    #define FIELD_WLAN_AC3_TX_TAG_OFFSET            12
    #define FIELD_WLAN_AC2_TX_TAG                   BITS(8, 11)
    #define FIELD_WLAN_AC2_TX_TAG_OFFSET            8
    #define FIELD_WLAN_AC1_TX_TAG                   BITS(4, 7)
    #define FIELD_WLAN_AC1_TX_TAG_OFFSET            4
    #define FIELD_WLAN_AC0_TX_TAG                   BITS(0, 3)
    #define FIELD_WLAN_AC0_TX_TAG_OFFSET            0
#define ARB_PTR1               (WIFI_ARB_BASE + 0x0098)
    #define FIELD_WLAN_BFEE_TX_TAG                  BITS(28, 31)
    #define FIELD_WLAN_BFEE_TX_TAG_OFFSET           28
    #define FIELD_WLAN_AC6_RX_TAG                   BITS(24, 27)
    #define FIELD_WLAN_AC6_RX_TAG_OFFSET            24
    #define FIELD_WLAN_AC5_RX_TAG                   BITS(20, 23)
    #define FIELD_WLAN_AC5_RX_TAG_OFFSET            20
    #define FIELD_WLAN_AC4_RX_TAG                   BITS(16, 19)
    #define FIELD_WLAN_AC4_RX_TAG_OFFSET            16
    #define FIELD_WLAN_AC3_RX_TAG                   BITS(12, 15)
    #define FIELD_WLAN_AC3_RX_TAG_OFFSET            12
    #define FIELD_WLAN_AC2_RX_TAG                   BITS(8, 11)
    #define FIELD_WLAN_AC2_RX_TAG_OFFSET            8
    #define FIELD_WLAN_AC1_RX_TAG                   BITS(4, 7)
    #define FIELD_WLAN_AC1_RX_TAG_OFFSET            4
    #define FIELD_WLAN_AC0_RX_TAG                   BITS(0, 3)
    #define FIELD_WLAN_AC0_RX_TAG_OFFSET            0
#define ARB_PTR2               (WIFI_ARB_BASE + 0x009C)
    #define FIELD_TAG_GUARD_TIME                    BITS(28, 31)
    #define FIELD_TAG_GUARD_TIME_OFFSET             28
    #define FIELD_WLAN_SP_HPRI_EN                   BIT(27)
    #define FIELD_WLAN_SP_HPRI_EN_OFFSET            27
    #define FIELD_WLAN_SP_RX_BD_EN                  BIT(26)
    #define FIELD_WLAN_SP_RX_BD_EN_OFFSET           26
    #define FIELD_WLAN_RX_NSW_MODE                  BITS(24, 25)
    #define FIELD_WLAN_RX_NSW_MODE_OFFSET           24
    #define FIELD_WLAN_RX_NSW_TAG                   BITS(20, 23)
    #define FIELD_WLAN_RX_NSW_TAG_OFFSET            20
    #define FIELD_WLAN_TX_RSP_TAG                   BITS(16, 19)
    #define FIELD_WLAN_TX_RSP_TAG_OFFSET            16
    #define FIELD_WLAN_BCN_TX_TAG                   BITS(12, 15)
    #define FIELD_WLAN_BCN_TX_TAG_OFFSET            12
    #define FIELD_WLAN_SP_RX_TAG                    BITS(8, 11)
    #define FIELD_WLAN_SP_RX_TAG_OFFSET             8
    #define FIELD_WLAN_PTA_TX_TAG                   BITS(4, 7)
    #define FIELD_WLAN_PTA_TX_TAG_OFFSET            4
    #define FIELD_WLAN_PTA_RX_TAG                   BITS(0, 3)
    #define FIELD_WLAN_PTA_RX_TAG_OFFSET            0
#define ARB_PTR3               (WIFI_ARB_BASE + 0x00A0)
    #define FIELD_WLAN_PTA_TX_TAG2                  BITS(28, 31)
    #define FIELD_WLAN_PTA_TX_TAG2_OFFSET           28
    #define FIELD_WLAN_PTA_RX_TAG2                  BITS(24, 27)
    #define FIELD_WLAN_PTA_RX_TAG2_OFFSET           24
    #define FIELD_WLAN_AC14_TX_TAG                  BITS(16, 19)
    #define FIELD_WLAN_AC14_TX_TAG_OFFSET           16
    #define FIELD_WLAN_AC13_TX_TAG                  BITS(12, 15)
    #define FIELD_WLAN_AC13_TX_TAG_OFFSET           12
    #define FIELD_WLAN_AC12_TX_TAG                  BITS(8, 11)
    #define FIELD_WLAN_AC12_TX_TAG_OFFSET           8
    #define FIELD_WLAN_AC11_TX_TAG                  BITS(4, 7)
    #define FIELD_WLAN_AC11_TX_TAG_OFFSET           4
    #define FIELD_WLAN_AC10_TX_TAG                  BITS(0, 3)
    #define FIELD_WLAN_AC10_TX_TAG_OFFSET           0
#define ARB_PTR4               (WIFI_ARB_BASE + 0x00A4)
    #define FIELD_WLAN_AC14_RX_TAG                  BITS(16, 19)
    #define FIELD_WLAN_AC14_RX_TAG_OFFSET           16
    #define FIELD_WLAN_AC13_RX_TAG                  BITS(12, 15)
    #define FIELD_WLAN_AC13_RX_TAG_OFFSET           12
    #define FIELD_WLAN_AC12_RX_TAG                  BITS(8, 11)
    #define FIELD_WLAN_AC12_RX_TAG_OFFSET           8
    #define FIELD_WLAN_AC11_RX_TAG                  BITS(4, 7)
    #define FIELD_WLAN_AC11_RX_TAG_OFFSET           4
    #define FIELD_WLAN_AC10_RX_TAG                  BITS(0, 3)
    #define FIELD_WLAN_AC10_RX_TAG_OFFSET           0
#define ARB_PTWR               (WIFI_ARB_BASE + 0x00A8)
    #define FIELD_REQ_TIMEOUT                       BITS(24, 31)
    #define FIELD_REQ_TIMEOUT_OFFSET                24
    #define FIELD_PTA_RXACK_EARLY_TIME              BITS(17, 22)
    #define FIELD_PTA_RXACK_EARLY_TIME_OFFSET       17
    #define FIELD_PTA_RXACK_EARLY_EN                BIT(16)
    #define FIELD_PTA_RXACK_EARLY_EN_OFFSET         16
    #define FIELD_BT_RW_OB_UNIT_MODE                BIT(15)
    #define FIELD_BT_RW_OB_UNIT_MODE_OFFSET         15
    #define FIELD_SILENCE_WIN                       BITS(0, 14)
    #define FIELD_SILENCE_WIN_OFFSET                0
#define ARB_RWPTCR0            (WIFI_ARB_BASE + 0x00AC)
    #define FIELD_RWP_CWMAX                         BITS(16, 31)
    #define FIELD_RWP_CWMAX_OFFSET                  16
    #define FIELD_RWP_CWMIN                         BITS(8, 15)
    #define FIELD_RWP_CWMIN_OFFSET                  8
    #define FIELD_AIFS_RWP                          BITS(0, 3)
    #define FIELD_AIFS_RWP_OFFSET                   0
#define ARB_RWPTCR1            (WIFI_ARB_BASE + 0x00B0)
    #define FIELD_RWP_EXTRA_RSVD                    BITS(16, 25)
    #define FIELD_RWP_EXTRA_RSVD_OFFSET             16
    #define FIELD_RWP_RETRY_LIMIT                   BITS(8, 11)
    #define FIELD_RWP_RETRY_LIMIT_OFFSET            8
#define ARB_RWPTCR2            (WIFI_ARB_BASE + 0x00B4)
    #define FIELD_SILENCE_WIN_NB_PS_EN              BIT(31)
    #define FIELD_SILENCE_WIN_NB_PS_EN_OFFSET       31
    #define FIELD_SILENCE_WIN_AIFS                  BITS(16, 19)
    #define FIELD_SILENCE_WIN_AIFS_OFFSET           16
    #define FIELD_AC_SILENCE_WIN                    BITS(0, 14)
    #define FIELD_AC_SILENCE_WIN_OFFSET             0
#define ARB_RWPTCR3            (WIFI_ARB_BASE + 0x00BC)
    #define FIELD_RWP_FRAME_SPE_EN                  BIT(27)
    #define FIELD_RWP_FRAME_SPE_EN_OFFSET           27
    #define FIELD_RWP_FRAME_ANT_PRI                 BITS(24, 26)
    #define FIELD_RWP_FRAME_ANT_PRI_OFFSET          24
    #define FIELD_RWP_FRAME_ANT_ID                  BITS(16, 21)
    #define FIELD_RWP_FRAME_ANT_ID_OFFSET           16
    #define FIELD_RWP_FRAME_MODE                    BITS(8, 10)
    #define FIELD_RWP_FRAME_MODE_OFFSET             8
    #define FIELD_RWP_FRAME_RATE                    BITS(0, 5)
    #define FIELD_RWP_FRAME_RATE_OFFSET             0
#define ARB_RWPCCWR            (WIFI_ARB_BASE + 0x00B8)
    #define FIELD_CTS_REISSUE_EN                    BIT(31)
    #define FIELD_CTS_REISSUE_EN_OFFSET             31
    #define FIELD_CTS_REISSUE_EARLY_TIME            BITS(24, 30)
    #define FIELD_CTS_REISSUE_EARLY_TIME_OFFSET     24
    #define FIELD_CTS_MAXREMWIN_EN                  BIT(23)
    #define FIELD_CTS_MAXREMWIN_EN_OFFSET           23
    #define FIELD_PSON_MAXREMWIN_EN                 BIT(22)
    #define FIELD_PSON_MAXREMWIN_EN_OFFSET          22
    #define FIELD_RWP_CURR_CW                       BITS(0, 15)
    #define FIELD_RWP_CURR_CW_OFFSET                0
#define ARB_RWPCFR0            (WIFI_ARB_BASE + 0x00C0)
    #define FIELD_RW0_NO_ACK_EN                     BIT(22)
    #define FIELD_RW0_NO_ACK_EN_OFFSET              22
    #define FIELD_TX_RWP_STATISTIC_EN               BIT(21)
    #define FIELD_TX_RWP_STATISTIC_EN_OFFSET        21
    #define FIELD_TX_RWP_INSCO_EN                   BIT(20)
    #define FIELD_TX_RWP_INSCO_EN_OFFSET            20
    #define FIELD_DIS_RX_REQ_INSCO_EN               BIT(19)
    #define FIELD_DIS_RX_REQ_INSCO_EN_OFFSET        19
    #define FIELD_MDRDY_PPDUTIME_INT_EN             BIT(18)
    #define FIELD_MDRDY_PPDUTIME_INT_EN_OFFSET      18
    #define FIELD_LONGPKT_PROTECT_MODE              BIT(17)
    #define FIELD_LONGPKT_PROTECT_MODE_OFFSET       17
    #define FIELD_LONGPKT_PROTECT_EN                BIT(16)
    #define FIELD_LONGPKT_PROTECT_EN_OFFSET         16
    #define FIELD_RX_NSW_TAG2                       BITS(12, 15)
    #define FIELD_RX_NSW_TAG2_OFFSET                12
    #define FIELD_RX_FRAMETIME_TH                   BITS(0, 10)
    #define FIELD_RX_FRAMETIME_TH_OFFSET            0
#define ARB_RWPCFR1            (WIFI_ARB_BASE + 0x00C4)
    #define FIELD_MDRDY_PPDUTIME                    BITS(0, 15)
    #define FIELD_MDRDY_PPDUTIME_OFFSET             0
#define ARB_PTMR0              (WIFI_ARB_BASE + 0x00D0)
    #define FIELD_PTA_INT_TEST_EN                   BIT(31)
    #define FIELD_PTA_INT_TEST_EN_OFFSET            31
    #define FIELD_WIFI_RXPWR_DIS                    BIT(8)
    #define FIELD_WIFI_RXPWR_DIS_OFFSET             8
    #define FIELD_PTA_GNT_DLY                       BITS(0, 7)
    #define FIELD_PTA_GNT_DLY_OFFSET                0
#define ARB_PTMR1              (WIFI_ARB_BASE + 0x00D4)
    #define FIELD_WIFI_PRI_TAG                      BITS(4, 7)
    #define FIELD_WIFI_PRI_TAG_OFFSET               4
    #define FIELD_WIFI_RX_REQ                       BIT(1)
    #define FIELD_WIFI_RX_REQ_OFFSET                1
    #define FIELD_WIFI_TX_REQ                       BIT(0)
    #define FIELD_WIFI_TX_REQ_OFFSET                0
#define ARB_PTMR2              (WIFI_ARB_BASE + 0x00D8)
    #define FIELD_OUTBAND_WIN40_BT                  BITS(16, 30)
    #define FIELD_OUTBAND_WIN40_BT_OFFSET           16
    #define FIELD_OUTBAND_WIN20_BT                  BITS(0, 14)
    #define FIELD_OUTBAND_WIN20_BT_OFFSET           0
#define ARB_PTMR4              (WIFI_ARB_BASE + 0x00E0)
    #define FIELD_REMAIN_WIN                        BITS(0, 14)
    #define FIELD_REMAIN_WIN_OFFSET                 0
#define ARB_PTMR5              (WIFI_ARB_BASE + 0x00E4)
    #define FIELD_PTA_EXT_TEST_EN                   BIT(31)
    #define FIELD_PTA_EXT_TEST_EN_OFFSET            31
    #define FIELD_PTA_RXPWR_DIS                     BIT(9)
    #define FIELD_PTA_RXPWR_DIS_OFFSET              9
    #define FIELD_PTA_GRANT                         BIT(8)
    #define FIELD_PTA_GRANT_OFFSET                  8
    #define FIELD_SW_WIFI_PRI_TAG                   BITS(4, 7)
    #define FIELD_SW_WIFI_PRI_TAG_OFFSET            4
    #define FIELD_SW_WIFI_RX_REQ                    BIT(1)
    #define FIELD_SW_WIFI_RX_REQ_OFFSET             1
    #define FIELD_SW_WIFI_TX_REQ                    BIT(0)
    #define FIELD_SW_WIFI_TX_REQ_OFFSET             0
#define ARB_PTMR7              (WIFI_ARB_BASE + 0x00EC)
    #define FIELD_BT_RW_INTVL                       BITS(24, 31)
    #define FIELD_BT_RW_INTVL_OFFSET                24
    #define FIELD_BT_RW_OW_CNT                      BITS(0, 10)
    #define FIELD_BT_RW_OW_CNT_OFFSET               0
#define ARB_PTMR9              (WIFI_ARB_BASE + 0x00F4)
    #define FIELD_LTE_REMAIN_WIN_REAL               BITS(16, 30)
    #define FIELD_LTE_REMAIN_WIN_REAL_OFFSET        16
    #define FIELD_LTE_REMAIN_WIN                    BITS(0, 14)
    #define FIELD_LTE_REMAIN_WIN_OFFSET             0
#define ARB_PTMR10             (WIFI_ARB_BASE + 0x00F8)
    #define FIELD_BT_REMAIN_WIN_REAL                BITS(0, 17)
    #define FIELD_BT_REMAIN_WIN_REAL_OFFSET         0
#define ARB_RSVD               (WIFI_ARB_BASE + 0x00FC)

#define ARB_RPTDD0             (WIFI_ARB_BASE + 0x0140)
    #define RXP_WL_LPKT_EN_MASK                     BIT(31)
    #define RXP_WL_LPKT_EN_OFFSET                   31
    #define RXP_WL_LRATE_EN_MASK                    BIT(30)
    #define RXP_WL_LRATE_EN_OFFSET                  30
    #define RXP_WL_ELY_RX_REQ_EN_MASK               BIT(29)
    #define RXP_WL_ELY_RX_REQ_EN_OFFSET             29
    #define RXP_WL_LRATE_RTS_EN_MASK                BIT(28)
    #define RXP_WL_LRATE_RTS_EN_OFFSET              28
    #define RXP_WL_PROT_LEGACY_EN_MASK              BIT(27)
    #define RXP_WL_PROT_LEGACY_EN_OFFSET            27
    #define RXP_WL_LPKT_DATA_DUR_THD_MASK           BITS(16, 26)
    #define RXP_WL_LPKT_DATA_DUR_THD_OFFSET         16
    #define RXP_WL_ELY_RX_REQ_CS_TO_CNT_MASK        BITS(8, 15)
    #define RXP_WL_ELY_RX_REQ_CS_TO_CNT_OFFSET      8
    #define RXP_WL_ELY_RX_REQ_SLOT_MASK             BITS(4, 6)
    #define RXP_WL_ELY_RX_REQ_SLOT_OFFSET           4
    #define RXP_WL_PROT_TAG_MASK                    BITS(0, 3)
    #define RXP_WL_PROT_TAG_OFFSET                  0
#define ARB_RPTDD1             (WIFI_ARB_BASE + 0x0144)
    #define RXP_WL_LRATE_RET_MASK                   BITS(24, 31)
    #define RXP_WL_LRATE_RET_OFFSET                 24
    #define RXP_WL_LRATE_TRIG_MASK                  BITS(16, 23)
    #define RXP_WL_LRATE_TRIG_OFFSET                16
    #define RXP_WL_LPKT_RET_MASK                    BITS(8, 15)
    #define RXP_WL_LPKT_RET_OFFSET                  8
    #define RXP_WL_LPKT_TRIG_MASK                   BITS(0, 7)
    #define RXP_WL_LPKT_TRIG_OFFSET                 0
#define ARB_RPTDD2             (WIFI_ARB_BASE + 0x0148)
    #define RXP_VHT_LRATE_RTS_THD_MASK              BITS(28, 31)
    #define RXP_VHT_LRATE_RTS_THD_OFFSET            28
    #define RXP_HT_LRATE_RTS_THD_MASK               BITS(22, 27)
    #define RXP_HT_LRATE_RTS_THD_OFFSET             22
    #define RXP_OFDM_LRATE_RTS_THD_MASK             BITS(18, 21)
    #define RXP_OFDM_LRATE_RTS_THD_OFFSET           18
    #define RXP_CCK_LRATE_RTS_THD_MASK              BITS(16, 17)
    #define RXP_CCK_LRATE_RTS_THD_OFFSET            16
    #define RXP_VHT_LRATE_DATA_THD_MASK             BITS(12, 15)
    #define RXP_VHT_LRATE_DATA_THD_OFFSET           12
    #define RXP_HT_LRATE_DATA_THD_MASK              BITS(6, 11)
    #define RXP_HT_LRATE_DATA_THD_OFFSET            6
    #define RXP_OFDM_LRATE_DATA_THD_MASK            BITS(2, 5)
    #define RXP_OFDM_LRATE_DATA_THD_OFFSET          2
    #define RXP_CCK_LRATE_DATA_THD_MASK             BITS(0, 1)
    #define RXP_CCK_LRATE_DATA_THD_OFFSET           0
#define ARB_RPTDD3             (WIFI_ARB_BASE + 0x014C)
    #define RXP_WL_ELY_RX_REQ_MDRDY_TO_CNT_MASK     BITS(16, 23)
    #define RXP_WL_ELY_RX_REQ_MDRDY_TO_CNT_OFFSET   16
    #define RXP_WL_PROT_COMB_HIT_CNT_MASK           BITS(0, 15)
    #define RXP_WL_PROT_COMB_HIT_CNT_OFFSET         0
/* Coexistence CR end */

/* TMAC */
#define  TMAC_TCR               (WIFI_TMAC_BASE + 0x0000)
#define  TMAC_PSCR              (WIFI_TMAC_BASE + 0x0004)
#define  TMAC_ACTXOPLR0         (WIFI_TMAC_BASE + 0x0010)
#define  TMAC_ACTXOPLR1         (WIFI_TMAC_BASE + 0x0014)
#define  TMAC_ACTXOPLR2         (WIFI_TMAC_BASE + 0x0018)
#define  TMAC_ACTXOPLR3         (WIFI_TMAC_BASE + 0x001C)
#define  TMAC_FP0R0             (WIFI_TMAC_BASE + 0x0020)
#define  TMAC_FP0R1             (WIFI_TMAC_BASE + 0x0024)
#define  TMAC_FP0R2             (WIFI_TMAC_BASE + 0x0028)
#define  TMAC_FP0R3             (WIFI_TMAC_BASE + 0x002C)
#define  TMAC_FP0R4             (WIFI_TMAC_BASE + 0x0030)
#define  TMAC_FP0R5             (WIFI_TMAC_BASE + 0x0034)
#define  TMAC_FP0R6             (WIFI_TMAC_BASE + 0x0038)
#define  TMAC_FP0R7             (WIFI_TMAC_BASE + 0x003C)

#if 0   //Posh 20140101, Merge code from MT6630 E2
#define  TMAC_FP1R0             (WIFI_TMAC_BASE + 0x0040)
#define  TMAC_FP1R1             (WIFI_TMAC_BASE + 0x0044)
#define  TMAC_FP1R2             (WIFI_TMAC_BASE + 0x0048)
#define  TMAC_FP1R3             (WIFI_TMAC_BASE + 0x004C)
#define  TMAC_FP1R4             (WIFI_TMAC_BASE + 0x0050)
#define  TMAC_FP1R5             (WIFI_TMAC_BASE + 0x0054)
#define  TMAC_FP1R6             (WIFI_TMAC_BASE + 0x0058)
#define  TMAC_FP1R7             (WIFI_TMAC_BASE + 0x005C)
#define  TMAC_FP2R0             (WIFI_TMAC_BASE + 0x0060)
#define  TMAC_FP2R1             (WIFI_TMAC_BASE + 0x0064)
#define  TMAC_FP2R2             (WIFI_TMAC_BASE + 0x0068)
#define  TMAC_FP2R3             (WIFI_TMAC_BASE + 0x006C)
#define  TMAC_FP2R4             (WIFI_TMAC_BASE + 0x0070)
#define  TMAC_FP2R5             (WIFI_TMAC_BASE + 0x0074)
#define  TMAC_FP2R6             (WIFI_TMAC_BASE + 0x0078)
#define  TMAC_FP2R7             (WIFI_TMAC_BASE + 0x007C)
#endif

#define  TMAC_FPCR              (WIFI_TMAC_BASE + 0x0080)
#define  TMAC_FP0R8             (WIFI_TMAC_BASE + 0x0084)

#define  TMAC_CDTR              (WIFI_TMAC_BASE + 0x0090)
#define  TMAC_ODTR              (WIFI_TMAC_BASE + 0x0094)
#define  TMAC_ATCR              (WIFI_TMAC_BASE + 0x0098)
#define  TMAC_TRCR              (WIFI_TMAC_BASE + 0x009C)
#define  TMAC_RRCR              (WIFI_TMAC_BASE + 0x00A0)
#define  TMAC_ICR               (WIFI_TMAC_BASE + 0x00A4)
#define  TMAC_PPDR              (WIFI_TMAC_BASE + 0x00A8)
#define  TMAC_BCSR              (WIFI_TMAC_BASE + 0x00AC)

#define  TMAC_BRCR              (WIFI_TMAC_BASE + 0x00B0)
#define  TMAC_PCR               (WIFI_TMAC_BASE + 0x00B4)

#define  TMAC_B0BRR0            (WIFI_TMAC_BASE + 0x00C0)
#define  TMAC_B0BRR1            (WIFI_TMAC_BASE + 0x00C4)
#define  TMAC_B1BRR0            (WIFI_TMAC_BASE + 0x00C8)
#define  TMAC_B1BRR1            (WIFI_TMAC_BASE + 0x00CC)
#define  TMAC_B2BRR0            (WIFI_TMAC_BASE + 0x00D0)
#define  TMAC_B2BRR1            (WIFI_TMAC_BASE + 0x00D4)
#define  TMAC_B3BRR0            (WIFI_TMAC_BASE + 0x00D8)
#define  TMAC_B3BRR1            (WIFI_TMAC_BASE + 0x00DC)

#define  TMAC_QNCR0       (WIFI_TMAC_BASE + 0x00E0)
    #define QN1_INIT_SEQ                        BITS(20, 31)
    #define QN1_INIT_SEQ_OFFSET                 20
    #define QN1_SEQ_RANGE_IN_POWER2             BITS(16, 19)
    #define QN1_SEQ_RANGE_IN_POWER2_OFFSET      16
    #define QN0_INIT_SEQ                        BITS(4, 15)
    #define QN0_INIT_SEQ_OFFSET                 4
    #define QN0_SEQ_RANGE_IN_POWER2             BITS(0, 3)
    #define QN0_SEQ_RANGE_IN_POWER2_OFFSET      0
#define  TMAC_QNCR1       (WIFI_TMAC_BASE + 0x00E4)
    #define QN1_QOS_CTRL                        BITS(16, 31)
    #define QN1_QOS_CTRL_OFFSET                 16
    #define QN0_QOS_CTRL                        BITS(0, 15)
    #define QN0_QOS_CTRL_OFFSET                 0
#define  TMAC_QNCR2       (WIFI_TMAC_BASE + 0x00E8)
    #define QN3_RESET_SEQ                       BIT(3)
    #define QN3_RESET_SEQ_OFFSET                3
    #define QN2_RESET_SEQ                       BIT(2)
    #define QN2_RESET_SEQ_OFFSET                2
    #define QN1_RESET_SEQ                       BIT(1)
    #define QN1_RESET_SEQ_OFFSET                1
    #define QN0_RESET_SEQ                       BIT(0)
    #define QN0_RESET_SEQ_OFFSET                0
#define  TMAC_QNCR3       (WIFI_TMAC_BASE + 0x00EC)
    #define QN3_INIT_SEQ                        BITS(20, 31)
    #define QN3_INIT_SEQ_OFFSET                 20
    #define QN3_SEQ_RANGE_IN_POWER2             BITS(16, 19)
    #define QN3_SEQ_RANGE_IN_POWER2_OFFSET      16
    #define QN2_INIT_SEQ                        BITS(4, 15)
    #define QN2_INIT_SEQ_OFFSET                 4
    #define QN2_SEQ_RANGE_IN_POWER2             BITS(0, 3)
    #define QN2_SEQ_RANGE_IN_POWER2_OFFSET      0
#define  TMAC_QNCR4       (WIFI_TMAC_BASE + 0x00F0)
    #define QN3_QOS_CTRL                        BITS(16, 31)
    #define QN3_QOS_CTRL_OFFSET                 16
    #define QN2_QOS_CTRL                        BITS(0, 15)
    #define QN2_QOS_CTRL_OFFSET                 0
#define  TMAC_SPCR0       (WIFI_TMAC_BASE + 0x00FC)
    #define SPCR0_TX_BURST_OPTION               BIT(16) // 1: follow 7615 mechanism, 0: follow 7628 mechanism
    #define SPCR0_REPORT_OM_MATCH_RSLT          BIT(3)  // 1: report OM matching result, 0: report BSSID UN-Matched
    #define SPCR0_BACKOFF_IGNORE_CCA_ED         BIT(2)  // 1: CCA_ED will NOT pause backoff count down, 0: CCA_ED will pause backoff count down
    #define SPCR0_CCA_BUSY_RELOAD_AIFS          BIT(1)  // 1: CCA_CS/CCS_PLD BUSY will reload AIFS, 0: CCA_CS/CCA_PLD BUSY will NOT reload AIFS
    #define SPCR0_CCA_ED_BUSY_RELOAD_AIFS       BIT(0)  // 1: CCA_ED BUSY will reload AIFS, 0: CCA_ED BUSY will NOT reload AIFS

#define  TMAC_TICMR0     (WIFI_TMAC_BASE + 0x0100)
#define  TMAC_TICMR1     (WIFI_TMAC_BASE + 0x0104)
#define  TMAC_PCTSR       (WIFI_TMAC_BASE + 0x0108)
#define  TMAC_PCTDR       (WIFI_TMAC_BASE + 0x010C)
#define  TMAC_DBGR0       (WIFI_TMAC_BASE + 0x0110)
#define  TMAC_DBGR1       (WIFI_TMAC_BASE + 0x0114)
#define  TMAC_DBGR2       (WIFI_TMAC_BASE + 0x0118)
#define  TMAC_DBGR3       (WIFI_TMAC_BASE + 0x011C)
#define  TMAC_DBGR4       (WIFI_TMAC_BASE + 0x0120)
#define  TMAC_DBGR5       (WIFI_TMAC_BASE + 0x0124)
#define  TMAC_DBGR6       (WIFI_TMAC_BASE + 0x0128)

#define  TMAC_DSWCR0      (WIFI_TMAC_BASE + 0x01A0)
    #define DSW_MODE_SEL_MASK                   BITS(30, 31)
    #define DSW_MODE_SEL_OFFSET                 30
    #define DSW_MV_AVG_WEIGHT_MASK              BITS(24, 26)
    #define DSW_MV_AVG_WEIGHT_OFFSET            24
    #define DSW_CNT_RST_MASK                    BITS(16, 20)
    #define DSW_CNT_RST_OFFSET                  16
    #define DSW_PEER_MAC1_MASK                  BITS(0, 15)
    #define DSW_PEER_MAC1_OFFSET                0
#define  TMAC_DSWCR1      (WIFI_TMAC_BASE + 0x01A4)
    #define DSW_PEER_MAC0_MASK                  BITS(0, 31)
    #define DSW_PEER_MAC0_OFFSET                0
#define  TMAC_DSWCR2      (WIFI_TMAC_BASE + 0x01A8)
    #define DSW_PREFACE_SIZE_MASK               BITS(16, 31)
    #define DSW_PREFACE_SIZE_OFFSET             16
    #define DSW_PREFACE_ACK_SIZE_MASK           BITS(0, 15)
    #define DSW_PREFACE_ACK_SIZE_OFFSET         0
#define  TMAC_DSWCR3      (WIFI_TMAC_BASE + 0x01AC)
    #define DSW_PREFACE_ACK_MAX_SIZE_MASK       BITS(16, 31)
    #define DSW_PREFACE_ACK_MAX_SIZE_OFFSET     16
    #define DSW_PREFACE_ACK_MIN_SIZE_MASK       BITS(0, 15)
    #define DSW_PREFACE_ACK_MIN_SIZE_OFFSET     0
#define  TMAC_DSWCR4      (WIFI_TMAC_BASE + 0x01B0)
    #define DSW_MEASURE_SIZE_MASK               BITS(16, 31)
    #define DSW_MEASURE_SIZE_OFFSET             16
    #define DSW_MV_AVG_SIZE_MASK                BITS(0, 15)
    #define DSW_MV_AVG_SIZE_OFFSET              0
#define  TMAC_DSWCR5      (WIFI_TMAC_BASE + 0x01B4)
    #define DSW_MV_MAX_SIZE_MASK                BITS(16, 31)
    #define DSW_MV_MAX_SIZE_OFFSET              16
    #define DSW_MV_MIN_SIZE_MASK                BITS(0, 15)
    #define DSW_MV_MIN_SIZE_OFFSET              0
#define  TMAC_DSWCR6      (WIFI_TMAC_BASE + 0x01B8)
    #define DSW_ACK_CNT_MASK                    BITS(0, 7)
    #define DSW_ACK_CNT_OFFSET                  0
#define  TMAC_DSWCR7      (WIFI_TMAC_BASE + 0x01BC)
    #define DSW_PREDEFINE_FAIL_CNT_MASK         BITS(24, 31)
    #define DSW_PREDEFINE_FAIL_CNT_OFFSET       24
    #define DSW_AIRCONDITION_FAIL_CNT_MASK      BITS(16, 23)
    #define DSW_AIRCONDITION_FAIL_CNT_OFFSET    16
    #define DSW_DATA_OVERFLOW_CNT_MASK          BITS(8, 15)
    #define DSW_DATA_OVERFLOW_CNT_OFFSET        8
    #define DSW_RTS_OVERFLOW_CNT_MASK           BITS(0, 7)
    #define DSW_RTS_OVERFLOW_CNT_OFFSET         0

/* RMAC */
#define  RMAC_RFCR        (WIFI_RMAC_BASE + 0x0000)
#define  RMAC_CB0R0       (WIFI_RMAC_BASE + 0x0004)
#define  RMAC_CB0R1       (WIFI_RMAC_BASE + 0x0008)
#define  RMAC_CB1R0       (WIFI_RMAC_BASE + 0x000C)
#define  RMAC_CB1R1       (WIFI_RMAC_BASE + 0x0010)
#define  RMAC_CB2R0       (WIFI_RMAC_BASE + 0x0014)
#define  RMAC_CB2R1       (WIFI_RMAC_BASE + 0x0018)
#define  RMAC_CB3R0       (WIFI_RMAC_BASE + 0x001C)
#define  RMAC_CB3R1       (WIFI_RMAC_BASE + 0x0020)
#define  RMAC_OMA0R0      (WIFI_RMAC_BASE + 0x0024)
#define  RMAC_OMA0R1      (WIFI_RMAC_BASE + 0x0028)
#define  RMAC_OMA1R0      (WIFI_RMAC_BASE + 0x002C)
#define  RMAC_OMA1R1      (WIFI_RMAC_BASE + 0x0030)
#define  RMAC_OMA2R0      (WIFI_RMAC_BASE + 0x0034)
#define  RMAC_OMA2R1      (WIFI_RMAC_BASE + 0x0038)
#define  RMAC_OMA3R0      (WIFI_RMAC_BASE + 0x003C)
#define  RMAC_OMA3R1      (WIFI_RMAC_BASE + 0x0040)
#define  RMAC_OMA4R0      (WIFI_RMAC_BASE + 0x0044)
#define  RMAC_OMA4R1      (WIFI_RMAC_BASE + 0x0048)
#define  RMAC_B01AR       (WIFI_RMAC_BASE + 0x004C)
#define  RMAC_B23AR       (WIFI_RMAC_BASE + 0x0050)
#define  RMAC_ACBEN       (WIFI_RMAC_BASE + 0x0054)
#define  RMAC_BCNF0       (WIFI_RMAC_BASE + 0x0058)
#define  RMAC_BCNF1       (WIFI_RMAC_BASE + 0x005C)
#define  RMAC_MAR0        (WIFI_RMAC_BASE + 0x0060)
#define  RMAC_MAR1        (WIFI_RMAC_BASE + 0x0064)
#define  RMAC_BSCR0       (WIFI_RMAC_BASE + 0x0068)
#define  RMAC_BSCR1       (WIFI_RMAC_BASE + 0x006C)
#define  RMAC_MBSSIDACT0  (WIFI_RMAC_BASE + 0x0070)
#define  RMAC_MBSSIDACT1  (WIFI_RMAC_BASE + 0x0074)

//#define  RMAC_RMACDR      (WIFI_RMAC_BASE + 0x0078)
#define  RMAC_DMY_31_0    (WIFI_RMAC_BASE + 0x0078)
#define  RMAC_NSUR        (WIFI_RMAC_BASE + 0x007C)
#define  RMAC_RMCR        (WIFI_RMAC_BASE + 0x0080)
#define  RMAC_MORE        (WIFI_RMAC_BASE + 0x0084)
#define  RMAC_LUNVR       (WIFI_RMAC_BASE + 0x0088)
#define  RMAC_PSMPCR      (WIFI_RMAC_BASE + 0x008C)
#define  RMAC_CHFREQ      (WIFI_RMAC_BASE + 0x0090)
#define  RMAC_MISC        (WIFI_RMAC_BASE + 0x0094)
#define  RMAC_MAXMINLEN   (WIFI_RMAC_BASE + 0x0098)
#define  RMAC_BTENCR      (WIFI_RMAC_BASE + 0x009C)
#define  RMAC_ACCR        (WIFI_RMAC_BASE + 0x00A0)
#define  RMAC_RFCR1       (WIFI_RMAC_BASE + 0x00A4)

#define  RMAC_PHYDELAY    (WIFI_RMAC_BASE + 0x00A8)
#define  RMAC_PMSRC0      (WIFI_RMAC_BASE + 0x00AC)
#define  RMAC_PMCTL0      (WIFI_RMAC_BASE + 0x00B0)
#define  RMAC_RICMR0      (WIFI_RMAC_BASE + 0x00B4)
#define  RMAC_RICMR1      (WIFI_RMAC_BASE + 0x00B8)
#define  RMAC_RICMR2      (WIFI_RMAC_BASE + 0x00BC)
#define  RMAC_RICMR3      (WIFI_RMAC_BASE + 0x00C0)
#define  RMAC_TFTACI0     (WIFI_RMAC_BASE + 0x00C4)
#define  RMAC_TFTACI1     (WIFI_RMAC_BASE + 0x00C8)
#define  RMAC_TFTACI2     (WIFI_RMAC_BASE + 0x00CC)
#define  RMAC_TFTACI3     (WIFI_RMAC_BASE + 0x00D0)
#define  RMAC_TFTACI4     (WIFI_RMAC_BASE + 0x00D4)
#define  RMAC_BCN01_HBRR  (WIFI_RMAC_BASE + 0x00D8)
#define  RMAC_BCN23_HBRR  (WIFI_RMAC_BASE + 0x00DC)
#define  RMAC_TMR_PA      (WIFI_RMAC_BASE + 0x00E0)
#define  RMAC_MULTFX1     (WIFI_RMAC_BASE + 0x00E4)
#define  RMAC_TXOPPS_PA   (WIFI_RMAC_BASE + 0x00E8)
#define  RMAC_FS0CR       (WIFI_RMAC_BASE + 0x00F0)
#define  RMAC_FS1CR       (WIFI_RMAC_BASE + 0x00F4)
#define  RMAC_FS2CR       (WIFI_RMAC_BASE + 0x00F8)
#define  RMAC_FS3CR       (WIFI_RMAC_BASE + 0x00FC)
#define  RMAC_PCICR       (WIFI_RMAC_BASE + 0x0100)
#define  RMAC_SCICR       (WIFI_RMAC_BASE + 0x0104)
#define  RMAC_S40CICR     (WIFI_RMAC_BASE + 0x0108)
#define  RMAC_MUARBR0     (WIFI_RMAC_BASE + 0x010C)
#define  RMAC_MUARBR1     (WIFI_RMAC_BASE + 0x0110)
#define  RMAC_SMESH       (WIFI_RMAC_BASE + 0x0114)
#define  RMAC_MISC2       (WIFI_RMAC_BASE + 0x0118)
#define  RMAC_TXBFEE      (WIFI_RMAC_BASE + 0x0120)
#define  RMAC_DBGFX1      (WIFI_RMAC_BASE + 0x0134)
#define  RMAC_DBGFX5      (WIFI_RMAC_BASE + 0x0144)
#define  RMAC_FSB0BR0     (WIFI_RMAC_BASE + 0x0150)
#define  RMAC_FSB0BR1     (WIFI_RMAC_BASE + 0x0154)
#define  RMAC_FSB1BR0     (WIFI_RMAC_BASE + 0x0158)
#define  RMAC_FSB1BR1     (WIFI_RMAC_BASE + 0x015c)
#define  RMAC_FSB2BR0     (WIFI_RMAC_BASE + 0x0160)
#define  RMAC_FSB2BR1     (WIFI_RMAC_BASE + 0x0164)
#define  RMAC_FSB3BR0     (WIFI_RMAC_BASE + 0x0168)
#define  RMAC_FSB3BR1     (WIFI_RMAC_BASE + 0x016c)

#define  RMAC_MBSSEXT1_0  (WIFI_RMAC_BASE + 0x0170)
#define  RMAC_MBSSEXT1_1  (WIFI_RMAC_BASE + 0x0174)
#define  RMAC_MBSSEXT2_0  (WIFI_RMAC_BASE + 0x0178)
#define  RMAC_MBSSEXT2_1  (WIFI_RMAC_BASE + 0x017C)
#define  RMAC_MBSSEXT3_0  (WIFI_RMAC_BASE + 0x0180)
#define  RMAC_MBSSEXT3_1  (WIFI_RMAC_BASE + 0x0184)
#define  RMAC_MBSSEXT4_0  (WIFI_RMAC_BASE + 0x0188)
#define  RMAC_MBSSEXT4_1  (WIFI_RMAC_BASE + 0x018C)
#define  RMAC_MBSSEXT5_0  (WIFI_RMAC_BASE + 0x0190)
#define  RMAC_MBSSEXT5_1  (WIFI_RMAC_BASE + 0x0194)
#define  RMAC_MBSSEXT6_0  (WIFI_RMAC_BASE + 0x0198)
#define  RMAC_MBSSEXT6_1  (WIFI_RMAC_BASE + 0x019C)
#define  RMAC_MBSSEXT7_0  (WIFI_RMAC_BASE + 0x01A0)
#define  RMAC_MBSSEXT7_1  (WIFI_RMAC_BASE + 0x01A4)
#define  RMAC_MBSSEXT8_0  (WIFI_RMAC_BASE + 0x01A8)
#define  RMAC_MBSSEXT8_1  (WIFI_RMAC_BASE + 0x01AC)
#define  RMAC_MBSSEXT9_0  (WIFI_RMAC_BASE + 0x01B0)
#define  RMAC_MBSSEXT9_1  (WIFI_RMAC_BASE + 0x01B4)
#define  RMAC_MBSSEXT10_0  (WIFI_RMAC_BASE + 0x01B8)
#define  RMAC_MBSSEXT10_1  (WIFI_RMAC_BASE + 0x01BC)
#define  RMAC_MBSSEXT11_0  (WIFI_RMAC_BASE + 0x01C0)
#define  RMAC_MBSSEXT11_1  (WIFI_RMAC_BASE + 0x01C4)
#define  RMAC_MBSSEXT12_0  (WIFI_RMAC_BASE + 0x01C8)
#define  RMAC_MBSSEXT12_1  (WIFI_RMAC_BASE + 0x01CC)
#define  RMAC_MBSSEXT13_0  (WIFI_RMAC_BASE + 0x01D0)
#define  RMAC_MBSSEXT13_1  (WIFI_RMAC_BASE + 0x01D4)
#define  RMAC_MBSSEXT14_0  (WIFI_RMAC_BASE + 0x01D8)
#define  RMAC_MBSSEXT14_1  (WIFI_RMAC_BASE + 0x01DC)
#define  RMAC_MBSSEXT15_0  (WIFI_RMAC_BASE + 0x01E0)
#define  RMAC_MBSSEXT15_1  (WIFI_RMAC_BASE + 0x01E4)

#define  RMAC_NAN0        (WIFI_RMAC_BASE + 0x01F4)
#define  RMAC_NAN1        (WIFI_RMAC_BASE + 0x01F8)
#define  RMAC_NAN2        (WIFI_RMAC_BASE + 0x01FC)
#define  RMAC_NAN3        (WIFI_RMAC_BASE + 0x0200)
#define  RMAC_NAN4        (WIFI_RMAC_BASE + 0x0204)
#define  RMAC_NAN5        (WIFI_RMAC_BASE + 0x0208)
#define  RMAC_NANBR0      (WIFI_RMAC_BASE + 0x020C)
#define  RMAC_NANBR1      (WIFI_RMAC_BASE + 0x0210)
#define  RMAC_NANBR2      (WIFI_RMAC_BASE + 0x0214)
#define  RMAC_NANBR3      (WIFI_RMAC_BASE + 0x0218)
#define  RMAC_NANBR4      (WIFI_RMAC_BASE + 0x021C)

#define  RMAC_PCSR        (WIFI_RMAC_BASE + 0xFFE0)
#define  RMAC_PCRDR       (WIFI_RMAC_BASE + 0xFFE4)
#define  RMAC_DBG0_FSM    (WIFI_RMAC_BASE + 0xFFE8)
#define  RMAC_DBG4        (WIFI_RMAC_BASE + 0xFFEC)
//#define  RMAC_DBG6        (WIFI_RMAC_BASE + 0xFFF0)
#define  RMAC_DBG8        (WIFI_RMAC_BASE + 0xFFF4)
#define  RMAC_FBITMAP0    (WIFI_RMAC_BASE + 0xFFF8)
#define  RMAC_FBITMAP1    (WIFI_RMAC_BASE + 0xFFFC)

#define  RMAC_TMRCS       (CONFG_TMRCSR)
#define  RMAC_TMRD        (CONFG_TMRDR)
#define  RMAC_TMRDT       (CONFG_TMRDTR)

#define  RMAC_RR0           (CONFG_RCPI0)
#define  RMAC_RR2           (CONFG_RCPI2)
#define  RMAC_RR3           (CONFG_RCPI3)
#define  RMAC_RR4           (CONFG_RCPI4)
#define  RMAC_RR6           (CONFG_RCPI6)
#define  RMAC_RR7           (CONFG_RCPI7)
#define  RMAC_RR9           (CONFG_RCPI9)
#define  RMAC_RR11          (CONFG_RCPI11)
#define  RMAC_RCPI0_CCK     (CONFG_RCPI_CCK0)
#define  RMAC_RCPI1_CCK     (CONFG_RCPI_CCK1)
#define  RMAC_RCPI2_CCK     (CONFG_RCPI_CCK2)
#define  RMAC_RCPI3_CCK     (CONFG_RCPI_CCK3)
#define  RMAC_RCPI0_OFDM    (CONFG_RCPI_OFDM0)
#define  RMAC_RCPI1_OFDM    (CONFG_RCPI_OFDM1)
#define  RMAC_RCPI2_OFDM    (CONFG_RCPI_OFDM2)
#define  RMAC_RCPI3_OFDM    (CONFG_RCPI_OFDM3)
#define  RMAC_RCPI_LQ_VALID (CONFG_RCPI_LQ_MISC)




#define  RMAC_LQR0              (CONFG_LQR0)
#define  RMAC_LQR1              (CONFG_LQR1)
#define  RMAC_DBG1_LQ_AVG       (CONFG_DBG1_LQ_AVG)
#define  RMAC_DBG2_LQ_AVG       (CONFG_DBG2_LQ_AVG)
#define  RMAC_LQR2              (CONFG_LQR2)
#define  RMAC_LQR3              (CONFG_LQR3)
#define  RMAC_LQBUF_CFG         (CONFG_LQ_BUF_CFG)
#define  RMAC_LQR4              (CONFG_LQR4)
#define  RMAC_LQR5              (CONFG_LQR5)
#define  RMAC_LQR6              (CONFG_LQR6)
#define  RMAC_LQR7              (CONFG_LQR7)
#define  RMAC_DBG3_LQ_AVG       (CONFG_DBG3_LQ_AVG)
#define  RMAC_DBG4_LQ_AVG       (CONFG_DBG4_LQ_AVG)
#define  RMAC_LQR8              (CONFG_LQR8)
#define  RMAC_LQR9              (CONFG_LQR9)
#define  RMAC_DBG6              (CONFG_DBG6_LQ_BUF)
#define  RMAC_LQR10             (CONFG_LQR10)
#define  RMAC_LQR11             (CONFG_LQR11)
#define  RMAC_LQBCR             (CONFG_LQBCR)
#define  RMAC_LQBPCR            (CONFG_LQBPCR)
#define  RMAC_LQBBUF0R          (CONFG_LQBBUF0R)
#define  RMAC_LQBBUF1R          (CONFG_LQBBUF1R)
#define  RMAC_LQBBUF2R          (CONFG_LQBBUF2R)
#define  RMAC_LQBBUF3R          (CONFG_LQBBUF3R)
#define  RMAC_LQCCK0R           (CONFG_LQCCK0R)
#define  RMAC_LQCCK1R           (CONFG_LQCCK1R)
#define  RMAC_LQCCK2R           (CONFG_LQCCK2R)
#define  RMAC_LQCCK3R           (CONFG_LQCCK3R)
#define  RMAC_LQOFDM0R          (CONFG_LQOFDM0R)
#define  RMAC_LQOFDM1R          (CONFG_LQOFDM1R)
#define  RMAC_LQOFDM2R          (CONFG_LQOFDM2R)
#define  RMAC_LQOFDM3R          (CONFG_LQOFDM3R)
#define  RMAC_LQRXCR0           (CONFG_LQRXCR0)
#define  RMAC_LQRXCR1           (CONFG_LQRXCR1)
#define  RMAC_LQRXCR2           (CONFG_LQRXCR2)
#define  RMAC_LQRXCR3           (CONFG_LQRXCR3)
#define  RMAC_LQRXBUF0          (CONFG_LQRXBUF0)
#define  RMAC_LQRXBUF1          (CONFG_LQRXBUF1)
#define  RMAC_LQRXBUF2          (CONFG_LQRXBUF2)
#define  RMAC_LQRXBUF3          (CONFG_LQRXBUF3)
#define  RMAC_LQRXHBUF0         (CONFG_LQRXHBUF0)
#define  RMAC_LQRXHBUF1         (CONFG_LQRXHBUF1)
#define  RMAC_LQRXHBUF2         (CONFG_LQRXHBUF2)
#define  RMAC_LQRXHBUF3         (CONFG_LQRXHBUF3)


#define  WTBL_ARCPICR     (WIFI_WTBL_BASE + 0xFF20)
#define  WTBL_ARCPIV0CR   (WIFI_WTBL_BASE + 0xFF24)
#define  WTBL_ARCPIV1CR   (WIFI_WTBL_BASE + 0xFF28)
#define  WTBL_ARCPIV5CR   (WIFI_WTBL_BASE + 0xFF38)

#define  RMAC_DBG3_BCN_CRC16 (WIFI_RMAC_BASE + 0x088)

#define  RMAC_MAXLEN         (WIFI_RMAC_BASE + 0x098)
#define  RMAC_DBG7           (WIFI_RMAC_BASE + 0x0C0)

/* LMAC PF CR */
#define PF_PFCR          (WIFI_PF_BASE +0x0000)
#define PF_PFRR          (WIFI_PF_BASE +0x0004)
#define PF_IPV6ECR       (WIFI_PF_BASE +0x0008)
#define PF_CF0SR         (WIFI_PF_BASE +0x0100)
/* Coalecing Filter 1 */
#define PF_CF0FPR1       (WIFI_PF_BASE +0x0104)
#define PF_CF0FPR2       (WIFI_PF_BASE +0x0108)
#define PF_CF0F1MR0      (WIFI_PF_BASE +0x0110)  /* filter 1, field1, mask */
#define PF_CF0F1MR1      (WIFI_PF_BASE +0x0114)
#define PF_CF0F1MR2      (WIFI_PF_BASE +0x0118)
#define PF_CF0F1MR3      (WIFI_PF_BASE +0x011C)
#define PF_CF0F2MR0      (WIFI_PF_BASE +0x0120)  /* filter 1, field2, mask */
#define PF_CF0F2MR1      (WIFI_PF_BASE +0x0124)
#define PF_CF0F2MR2      (WIFI_PF_BASE +0x0128)
#define PF_CF0F2MR3      (WIFI_PF_BASE +0x012C)
#define PF_CF0F3MR0      (WIFI_PF_BASE +0x0130)  /* filter 1, field3, mask */
#define PF_CF0F3MR1      (WIFI_PF_BASE +0x0134)
#define PF_CF0F3MR2      (WIFI_PF_BASE +0x0138)
#define PF_CF0F3MR3      (WIFI_PF_BASE +0x013C)
#define PF_CF0F4MR0      (WIFI_PF_BASE +0x0140)  /* filter 1, field4, mask */
#define PF_CF0F4MR1      (WIFI_PF_BASE +0x0144)
#define PF_CF0F4MR2      (WIFI_PF_BASE +0x0148)
#define PF_CF0F4MR3      (WIFI_PF_BASE +0x014C)
#define PF_CF0F5MR0      (WIFI_PF_BASE +0x0150)  /* filter 1, field5, mask */
#define PF_CF0F5MR1      (WIFI_PF_BASE +0x0154)
#define PF_CF0F5MR2      (WIFI_PF_BASE +0x0158)
#define PF_CF0F5MR3      (WIFI_PF_BASE +0x015C)
#define PF_CF0F1MP0      (WIFI_PF_BASE +0x0160)  /* filter 1, field1, pattern */
#define PF_CF0F1MP1      (WIFI_PF_BASE +0x0164)
#define PF_CF0F1MP2      (WIFI_PF_BASE +0x0168)
#define PF_CF0F1MP3      (WIFI_PF_BASE +0x016C)
#define PF_CF0F2MP0      (WIFI_PF_BASE +0x0170)  /* filter 1, field2, pattern */
#define PF_CF0F2MP1      (WIFI_PF_BASE +0x0174)
#define PF_CF0F2MP2      (WIFI_PF_BASE +0x0178)
#define PF_CF0F2MP3      (WIFI_PF_BASE +0x017C)
#define PF_CF0F3MP0      (WIFI_PF_BASE +0x0180)  /* filter 1, field3, pattern */
#define PF_CF0F3MP1      (WIFI_PF_BASE +0x0184)
#define PF_CF0F3MP2      (WIFI_PF_BASE +0x0188)
#define PF_CF0F3MP3      (WIFI_PF_BASE +0x018C)
#define PF_CF0F4MP0      (WIFI_PF_BASE +0x0190)  /* filter 1, field4, pattern */
#define PF_CF0F4MP1      (WIFI_PF_BASE +0x0194)
#define PF_CF0F4MP2      (WIFI_PF_BASE +0x0198)
#define PF_CF0F4MP3      (WIFI_PF_BASE +0x019C)
#define PF_CF0F5MP0      (WIFI_PF_BASE +0x01A0)  /* filter 1, field5, pattern */
#define PF_CF0F5MP1      (WIFI_PF_BASE +0x01A4)
#define PF_CF0F5MP2      (WIFI_PF_BASE +0x01A8)
#define PF_CF0F5MP3      (WIFI_PF_BASE +0x01AC)
/* Coalecing Filter 2 */
#define PF_CF1SR         (WIFI_PF_BASE +0x0200)
#define PF_CF1FPR1       (WIFI_PF_BASE +0x0204)
#define PF_CF1FPR2       (WIFI_PF_BASE +0x0208)
#define PF_CF1F1MR0      (WIFI_PF_BASE +0x0210)  /* filter 2, field1, mask */
#define PF_CF1F1MR1      (WIFI_PF_BASE +0x0214)
#define PF_CF1F1MR2      (WIFI_PF_BASE +0x0218)
#define PF_CF1F1MR3      (WIFI_PF_BASE +0x021C)
#define PF_CF1F2MR0      (WIFI_PF_BASE +0x0220)  /* filter 2, field2, mask */
#define PF_CF1F2MR1      (WIFI_PF_BASE +0x0224)
#define PF_CF1F2MR2      (WIFI_PF_BASE +0x0228)
#define PF_CF1F2MR3      (WIFI_PF_BASE +0x022C)
#define PF_CF1F3MR0      (WIFI_PF_BASE +0x0230)  /* filter 2, field3, mask */
#define PF_CF1F3MR1      (WIFI_PF_BASE +0x0234)
#define PF_CF1F3MR2      (WIFI_PF_BASE +0x0238)
#define PF_CF1F3MR3      (WIFI_PF_BASE +0x023C)
#define PF_CF1F4MR0      (WIFI_PF_BASE +0x0240)  /* filter 2, field4, mask */
#define PF_CF1F4MR1      (WIFI_PF_BASE +0x0244)
#define PF_CF1F4MR2      (WIFI_PF_BASE +0x0248)
#define PF_CF1F4MR3      (WIFI_PF_BASE +0x024C)
#define PF_CF1F5MR0      (WIFI_PF_BASE +0x0250)  /* filter 2, field5, mask */
#define PF_CF1F5MR1      (WIFI_PF_BASE +0x0254)
#define PF_CF1F5MR2      (WIFI_PF_BASE +0x0258)
#define PF_CF1F5MR3      (WIFI_PF_BASE +0x025C)
#define PF_CF1F1MP0      (WIFI_PF_BASE +0x0260)  /* filter 2, field1, pattern */
#define PF_CF1F1MP1      (WIFI_PF_BASE +0x0264)
#define PF_CF1F1MP2      (WIFI_PF_BASE +0x0268)
#define PF_CF1F1MP3      (WIFI_PF_BASE +0x026C)
#define PF_CF1F2MP0      (WIFI_PF_BASE +0x0270)  /* filter 2, field2, pattern */
#define PF_CF1F2MP1      (WIFI_PF_BASE +0x0274)
#define PF_CF1F2MP2      (WIFI_PF_BASE +0x0278)
#define PF_CF1F2MP3      (WIFI_PF_BASE +0x027C)
#define PF_CF1F3MP0      (WIFI_PF_BASE +0x0280)  /* filter 2, field3, pattern */
#define PF_CF1F3MP1      (WIFI_PF_BASE +0x0284)
#define PF_CF1F3MP2      (WIFI_PF_BASE +0x0288)
#define PF_CF1F3MP3      (WIFI_PF_BASE +0x028C)
#define PF_CF1F4MP0      (WIFI_PF_BASE +0x0290)  /* filter 2, field4, pattern */
#define PF_CF1F4MP1      (WIFI_PF_BASE +0x0294)
#define PF_CF1F4MP2      (WIFI_PF_BASE +0x0298)
#define PF_CF1F4MP3      (WIFI_PF_BASE +0x029C)
#define PF_CF1F5MP0      (WIFI_PF_BASE +0x02A0)  /* filter 2, field5, pattern */
#define PF_CF1F5MP1      (WIFI_PF_BASE +0x02A4)
#define PF_CF1F5MP2      (WIFI_PF_BASE +0x02A8)
#define PF_CF1F5MP3      (WIFI_PF_BASE +0x02AC)

#define  PF_CF2FPR1         (WIFI_PF_BASE + 0x0304)
#define  PF_CF2FPR2         (WIFI_PF_BASE + 0x0308)
#define  PF_CF3FPR1         (WIFI_PF_BASE + 0x0404)
#define  PF_CF3FPR2         (WIFI_PF_BASE + 0x0408)
#define  PF_CF4FPR1         (WIFI_PF_BASE + 0x0504)
#define  PF_CF4FPR2         (WIFI_PF_BASE + 0x0508)
#define  PF_CF5FPR1         (WIFI_PF_BASE + 0x0604)
#define  PF_CF5FPR2         (WIFI_PF_BASE + 0x0608)
#define  PF_CF6FPR1         (WIFI_PF_BASE + 0x0704)
#define  PF_CF6FPR2         (WIFI_PF_BASE + 0x0708)
#define  PF_CF7FPR1         (WIFI_PF_BASE + 0x0804)
#define  PF_CF7FPR2         (WIFI_PF_BASE + 0x0808)
#define  PF_CF8FPR1         (WIFI_PF_BASE + 0x0904)
#define  PF_CF8FPR2         (WIFI_PF_BASE + 0x0908)
#define  PF_CF9FPR1         (WIFI_PF_BASE + 0x0A04)
#define  PF_CF9FPR2         (WIFI_PF_BASE + 0x0A08)

/* Bitmap Pattern */
#define PF_WB00_PR       (WIFI_PF_BASE +0x0B00) /* bitmap 00 */
#define PF_WB00_R        (WIFI_PF_BASE +0x0B04)
#define PF_WB01_PR       (WIFI_PF_BASE +0x0B08) /* bitmap 01 */
#define PF_WB01_R        (WIFI_PF_BASE +0x0B0C)
#define PF_WB02_PR       (WIFI_PF_BASE +0x0B10) /* bitmap 02 */
#define PF_WB02_R        (WIFI_PF_BASE +0x0B14)
#define PF_WB03_PR       (WIFI_PF_BASE +0x0B18) /* bitmap 03 */
#define PF_WB03_R        (WIFI_PF_BASE +0x0B1C)
#define PF_WB04_PR       (WIFI_PF_BASE +0x0B20) /* bitmap 04 */
#define PF_WB04_R        (WIFI_PF_BASE +0x0B24)
#define PF_WB05_PR       (WIFI_PF_BASE +0x0B28) /* bitmap 05 */
#define PF_WB05_R        (WIFI_PF_BASE +0x0B2C)
#define PF_WB06_PR       (WIFI_PF_BASE +0x0B30) /* bitmap 06 */
#define PF_WB06_R        (WIFI_PF_BASE +0x0B34)
#define PF_WB07_PR       (WIFI_PF_BASE +0x0B38) /* bitmap 07 */
#define PF_WB07_R        (WIFI_PF_BASE +0x0B3c)
#define PF_WB08_PR       (WIFI_PF_BASE +0x0B40) /* bitmap 08 */
#define PF_WB08_R        (WIFI_PF_BASE +0x0B44)
#define PF_WB09_PR       (WIFI_PF_BASE +0x0B48) /* bitmap 09 */
#define PF_WB09_R        (WIFI_PF_BASE +0x0B4C)
#define PF_WB10_PR       (WIFI_PF_BASE +0x0B50) /* bitmap 10 */
#define PF_WB10_R        (WIFI_PF_BASE +0x0B54)
#define PF_WB11_PR       (WIFI_PF_BASE +0x0B58) /* bitmap 11 */
#define PF_WB11_R        (WIFI_PF_BASE +0x0B5C)
#define PF_WB12_PR       (WIFI_PF_BASE +0x0B60) /* bitmap 12 */
#define PF_WB12_R        (WIFI_PF_BASE +0x0B64)
#define PF_WB13_PR       (WIFI_PF_BASE +0x0B68) /* bitmap 13 */
#define PF_WB13_R        (WIFI_PF_BASE +0x0B6C)
#define PF_WB14_PR       (WIFI_PF_BASE +0x0B70) /* bitmap 14 */
#define PF_WB14_R        (WIFI_PF_BASE +0x0B74)
#define PF_WB15_PR       (WIFI_PF_BASE +0x0B78) /* bitmap 15 */
#define PF_WB15_R        (WIFI_PF_BASE +0x0B7C)
/* ARP/NS Offload */
#define PF_ANOCR0        (WIFI_PF_BASE +0x0C00)
#define PF_ANOCR1        (WIFI_PF_BASE +0x0C04)
#define PF_ANOIP1R0      (WIFI_PF_BASE +0x0C10)
#define PF_ANOIP1R1      (WIFI_PF_BASE +0x0C14)
#define PF_ANOIP1R2      (WIFI_PF_BASE +0x0C18)
#define PF_ANOIP1R3      (WIFI_PF_BASE +0x0C1C)
#define PF_ANOIP2R0      (WIFI_PF_BASE +0x0C20)
#define PF_ANOIP2R1      (WIFI_PF_BASE +0x0C24)
#define PF_ANOIP2R2      (WIFI_PF_BASE +0x0C28)
#define PF_ANOIP2R3      (WIFI_PF_BASE +0x0C2C)
#define PF_ANOIP3R0      (WIFI_PF_BASE +0x0C30)
#define PF_ANOIP3R1      (WIFI_PF_BASE +0x0C34)
#define PF_ANOIP3R2      (WIFI_PF_BASE +0x0C38)
#define PF_ANOIP3R3      (WIFI_PF_BASE +0x0C3C)
#define PF_ANORIP6R0     (WIFI_PF_BASE +0x0C40)
#define PF_ANORIP6R1     (WIFI_PF_BASE +0x0C44)
#define PF_ANORIP6R2     (WIFI_PF_BASE +0x0C48)
#define PF_ANORIP6R3     (WIFI_PF_BASE +0x0C4C)
#define PF_ANORIP4R      (WIFI_PF_BASE +0x0C50)



/* INT WAKEUP */
#define  LP_WIS0R           (WIFI_INT_BASE + 0x0000)
#define  LP_WIS1R           (WIFI_INT_BASE + 0x0004)
#define  LP_WIE0R           (WIFI_INT_BASE + 0x0008)
#define  LP_WIE1R           (WIFI_INT_BASE + 0x000C)
#define  LP_AHB_CLK_CTL     (WIFI_INT_BASE + 0x0020)
#define  LP_WIS2R           (WIFI_INT_BASE + 0x0024)
#define  LP_WIE2R           (WIFI_INT_BASE + 0x0028)
#define  LP_WIS3R           (WIFI_INT_BASE + 0x002C)
#define  LP_WIE3R           (WIFI_INT_BASE + 0x0030)
#define  LP_WIS4R           (WIFI_INT_BASE + 0x0034)
#define  LP_WIE4R           (WIFI_INT_BASE + 0x0038)

/* LP */
#define  LP_TT0SBOR1        (WIFI_LP_BASE + 0x0000)
#define  LP_TT0SBOR2        (WIFI_LP_BASE + 0x0004)
#define  LP_TT0SBOR3        (WIFI_LP_BASE + 0x0008)
#define  LP_TT0SBOR4        (WIFI_LP_BASE + 0x000C)
/*
#define  LP_MTR0            (WIFI_LP_BASE + 0x0000)
#define  LP_MTR1            (WIFI_LP_BASE + 0x0004)
#define  LP_MTCR0           (WIFI_LP_BASE + 0x0008)
#define  LP_MTCR1           (WIFI_LP_BASE + 0x000C)
*/
#define  LP_T0CR            (WIFI_LP_BASE + 0x0010)
#define  LP_T1CR            (WIFI_LP_BASE + 0x0014)
#define  LP_UTTR0           (WIFI_LP_BASE + 0x0018)
#define  LP_UTTR1           (WIFI_LP_BASE + 0x001C)
#define  LP_BEIR            (WIFI_LP_BASE + 0x0020)
#define  LP_TIMTR           (WIFI_LP_BASE + 0x0024)
//#define  LP_32KCCR          (WIFI_LP_BASE + 0x0024)
#define  LP_T0STR           (WIFI_LP_BASE + 0x0028)
#define  LP_T1STR           (WIFI_LP_BASE + 0x002C)
#define  LP_PISR            (WIFI_LP_BASE + 0x0030)
#define  LP_T0TPCR          (WIFI_LP_BASE + 0x0034)
#define  LP_T1TPCR          (WIFI_LP_BASE + 0x0038)
#define  LP_TTSR            (WIFI_LP_BASE + 0x003C)
#define  LP_TFRSR           (WIFI_LP_BASE + 0x0040)
#define  LP_TSELR           (WIFI_LP_BASE + 0x0044)
#define  LP_SPCR            (WIFI_LP_BASE + 0x0048)
//#define  LP_TR              (WIFI_LP_BASE + 0x004C)
#define  LP_BCNTR           (WIFI_LP_BASE + 0x004C)
#define  LP_TCLCR           (WIFI_LP_BASE + 0x0050)
//#define  LP_WSHPCR          (WIFI_LP_BASE + 0x0054)
//#define  LP_WTHPCR          (WIFI_LP_BASE + 0x0058)
#define  LP_TT0SBOR5        (WIFI_LP_BASE + 0x0054)
#define  LP_TT0SBOR6        (WIFI_LP_BASE + 0x0058)
#define  LP_MPTCR0          (WIFI_LP_BASE + 0x005C)
#define  LP_MPTCR1          (WIFI_LP_BASE + 0x0060)
#define  LP_TT0SBOR7        (WIFI_LP_BASE + 0x0064)
#define  LP_TT0SBOR8        (WIFI_LP_BASE + 0x0068)
#define  LP_TT0SBOR9        (WIFI_LP_BASE + 0x006C)
#define  LP_TT0SBOR10       (WIFI_LP_BASE + 0x0070)
#define  LP_TT0SBOR11       (WIFI_LP_BASE + 0x0074)
#define  LP_TT0SBOR12       (WIFI_LP_BASE + 0x0078)
#define  LP_FRCR            (WIFI_LP_BASE + 0x007C)
//#define  LP_WSHSCR          (WIFI_LP_BASE + 0x0080)    // Deprecated
//#define  LP_WTHSCR          (WIFI_LP_BASE + 0x0084)    // Deprecated
#define  LP_TT0SBOR13       (WIFI_LP_BASE + 0x0080)
#define  LP_TT0SBOR14       (WIFI_LP_BASE + 0x0084)
#define  LP_WLANCKCR        (WIFI_LP_BASE + 0x0088)
//#define  LP_SRAMSP          (WIFI_LP_BASE + 0x008C)
#define  LP_TT0SBOR15       (WIFI_LP_BASE + 0x008C)
#define  LP_TXTCR0          (WIFI_LP_BASE + 0x0090)
#define  LP_TTR0            (WIFI_LP_BASE + 0x0094)
//#define  LP_MTR2            (WIFI_LP_BASE + 0x0098)
//#define  LP_MTR3            (WIFI_LP_BASE + 0x009C)
//#define  LP_SMCTL0          (WIFI_LP_BASE + 0x00A0)
//#define  LP_SMCTL1          (WIFI_LP_BASE + 0x00A4)
//#define  LP_SMCTL2          (WIFI_LP_BASE + 0x00A8)
//#define  LP_SMCTL3          (WIFI_LP_BASE + 0x00AC)
//#define  LP_SMCTL4          (WIFI_LP_BASE + 0x00B0)
//#define  LP_SMCTL5          (WIFI_LP_BASE + 0x00B4)
//#define  LP_SMCTL6          (WIFI_LP_BASE + 0x00B8)
#define  LP_DUMMY0          (WIFI_LP_BASE + 0x0098)
#define  LP_DUMMY1          (WIFI_LP_BASE + 0x009C)

//#define  LP_MMBSR0              (WIFI_LP_BASE + 0x00C4)    // Deprecated?
//#define  LP_MMBSR1              (WIFI_LP_BASE + 0x00C8)    // Deprecated?

//#define  LP_MTR4                  (WIFI_LP_BASE + 0x00CC)
//#define  LP_MTR5                  (WIFI_LP_BASE + 0x00D0)
//#define  LP_MTR6                  (WIFI_LP_BASE + 0x00D4)
//#define  LP_MTR7                  (WIFI_LP_BASE + 0x00D8)
//#define  LP_MTCR2                (WIFI_LP_BASE + 0x00DC)
#define  LP_SBTOR1          (WIFI_LP_BASE + 0x00A0)
#define  LP_SBTOR2          (WIFI_LP_BASE + 0x00A4)
#define  LP_SBTOR3          (WIFI_LP_BASE + 0x00A8)
#define  LP_SBTOR4          (WIFI_LP_BASE + 0x00AC)
#define  LP_SBTOR5          (WIFI_LP_BASE + 0x00B0)
#define  LP_SBTOR6          (WIFI_LP_BASE + 0x00B4)
#define  LP_SBTOR7          (WIFI_LP_BASE + 0x00B8)
#define  LP_SBTOR8          (WIFI_LP_BASE + 0x00BC)
#define  LP_SBTOR9          (WIFI_LP_BASE + 0x00C0)
#define  LP_SBTOR10         (WIFI_LP_BASE + 0x00C4)
#define  LP_SBTOR11         (WIFI_LP_BASE + 0x00C8)
#define  LP_SBTOR12         (WIFI_LP_BASE + 0x00CC)
#define  LP_SBTOR13         (WIFI_LP_BASE + 0x00D0)
#define  LP_SBTOR14         (WIFI_LP_BASE + 0x00D4)
#define  LP_SBTOR15         (WIFI_LP_BASE + 0x00D8)

#define  LP_PTTISR         (WIFI_LP_BASE + 0x00DC) //NPS
#define  LP_TXTCR1          (WIFI_LP_BASE + 0x00E0)
#define  LP_TTR1            (WIFI_LP_BASE + 0x00E4)

#define  LP_TT0TPCR         (WIFI_LP_BASE + 0x00E8)
//#define LP_SMCTL7           (WIFI_LP_BASE + 0x00E8)
//#define LP_SMCTL8           (WIFI_LP_BASE + 0x00EC)
//#define LP_SMCTL9           (WIFI_LP_BASE + 0x00F0)
#define  LP_TT1TPCR         (WIFI_LP_BASE + 0x00EC)
#define  LP_TT2TPCR         (WIFI_LP_BASE + 0x00F0)
#define  LP_TT3TPCR         (WIFI_LP_BASE + 0x00F4)
#define  LP_T2CR            (WIFI_LP_BASE + 0x00F8)
#define  LP_T3CR            (WIFI_LP_BASE + 0x00FC)
#define  LP_T2STR           (WIFI_LP_BASE + 0x0100)
#define  LP_T3STR           (WIFI_LP_BASE + 0x0104)
#define  LP_T2TPCR          (WIFI_LP_BASE + 0x0108)
#define  LP_T3TPCR          (WIFI_LP_BASE + 0x010C)
#define  LP_MPTCR2          (WIFI_LP_BASE + 0x0110)
#define  LP_MPTCR3          (WIFI_LP_BASE + 0x0114)
#define  LP_TXTCR2          (WIFI_LP_BASE + 0x0118)
#define  LP_TTR2            (WIFI_LP_BASE + 0x011C)
#define  LP_TXTCR3          (WIFI_LP_BASE + 0x0120)
#define  LP_TTR3            (WIFI_LP_BASE + 0x0124)
#define  LP_MPTCR4          (WIFI_LP_BASE + 0x0128)
#define  LP_MPTCR5          (WIFI_LP_BASE + 0x012C)
#define  LP_MPTCR6          (WIFI_LP_BASE + 0x0130)
#define  LP_MPTCR7          (WIFI_LP_BASE + 0x0134)
#define  LP_TT0STR          (WIFI_LP_BASE + 0x0138)
#define  LP_TT1STR          (WIFI_LP_BASE + 0x013C)
#define  LP_TT2STR          (WIFI_LP_BASE + 0x0140)
#define  LP_TT3STR          (WIFI_LP_BASE + 0x0144)
#define  LP_FMSCR0          (WIFI_LP_BASE + 0x0148)
#define  LP_FMSCNTCR0       (WIFI_LP_BASE + 0x014C)
#define  LP_FMSCNTCR1       (WIFI_LP_BASE + 0x0150)
#define  LP_FMSIDCR0        (WIFI_LP_BASE + 0x0154)
#define  LP_FMSIDCR1        (WIFI_LP_BASE + 0x0158)
#define  LP_FMSIDCR2        (WIFI_LP_BASE + 0x015C)
#define  LP_FMSIDCR3        (WIFI_LP_BASE + 0x0160)
#define  LP_LGRXBCNCR0      (WIFI_LP_BASE + 0x0164)
#define  LP_LGRXBCNCR1      (WIFI_LP_BASE + 0x0168)
#define  LP_LGRXBCNCR2      (WIFI_LP_BASE + 0x016C)
#define  LP_WNMSLPCR        (WIFI_LP_BASE + 0x0170)
#define  LP_WLANCKCALCR     (WIFI_LP_BASE + 0x0174)
#define  LP_TIMSPCR         (WIFI_LP_BASE + 0x0178)
#define  LP_CTBCNCR         (WIFI_LP_BASE + 0x017C)
// LP_OFF Group
#define LP_MMCR0            (WIFI_LP_BASE + 0x0180)
#define LP_MMCR1            (WIFI_LP_BASE + 0x0184)
#define LP_MMCR2            (WIFI_LP_BASE + 0x0188)
#define LP_MMCR3            (WIFI_LP_BASE + 0x018C)
#define LP_MMBSR0           (WIFI_LP_BASE + 0x0190)
#define LP_MMBSR1           (WIFI_LP_BASE + 0x0194)
#define LP_QCCR0            (WIFI_LP_BASE + 0x0198)
#define LP_QCCR1            (WIFI_LP_BASE + 0x019C)
#define LP_MMCR4            (WIFI_LP_BASE + 0x01A0)
#define LP_MMCR5            (WIFI_LP_BASE + 0x01A4)
#define LP_MMCR6            (WIFI_LP_BASE + 0x01A8)
#define LP_MMCR7            (WIFI_LP_BASE + 0x01AC)
#define LP_MMBSR2           (WIFI_LP_BASE + 0x01B0)
#define LP_MMBSR3           (WIFI_LP_BASE + 0x01B4)
#define LP_TT0TPR           (WIFI_LP_BASE + 0x01B8)
#define LP_TT1TPR           (WIFI_LP_BASE + 0x01BC)
#define LP_TT2TPR           (WIFI_LP_BASE + 0x01C0)
#define LP_TT3TPR           (WIFI_LP_BASE + 0x01C4)
//LP internal CRs
#define LP_LTTR0            (WIFI_LP_BASE + 0x01C8)
#define LP_LTTR1            (WIFI_LP_BASE + 0x01CC)
#define LP_LTTR2            (WIFI_LP_BASE + 0x01D0)
#define LP_LTTR3            (WIFI_LP_BASE + 0x01D4)
#define LP_LTTR4            (WIFI_LP_BASE + 0x01D8)
#define LP_LTTR5            (WIFI_LP_BASE + 0x01DC)
#define LP_LTTR6            (WIFI_LP_BASE + 0x01D0)
#define LP_LTTR7            (WIFI_LP_BASE + 0x01E4)
#define LP_TTTR0            (WIFI_LP_BASE + 0x01D8)
#define LP_TTTR1            (WIFI_LP_BASE + 0x01EC)
#define LP_TTTR2            (WIFI_LP_BASE + 0x01F0)
#define LP_TTTR3            (WIFI_LP_BASE + 0x01F4)
#define LP_TTTR4            (WIFI_LP_BASE + 0x01F8)
#define LP_TTTR5            (WIFI_LP_BASE + 0x01FC)
#define LP_TTTR6            (WIFI_LP_BASE + 0x0200)
#define LP_TTTR7            (WIFI_LP_BASE + 0x0204)
#define LP_TGTR             (WIFI_LP_BASE + 0x0208)
#define LP_LBCNTOR          (WIFI_LP_BASE + 0x020C)
#define LP_LTIMTOR          (WIFI_LP_BASE + 0x0210)
#define LP_LBMCTOR          (WIFI_LP_BASE + 0x0214)
#define LP_LTBMCTOR         (WIFI_LP_BASE + 0x0218)
#define LP_LLGRBRR          (WIFI_LP_BASE + 0x021C)
#define LP_LLGRBLR          (WIFI_LP_BASE + 0x0220)
#define LP_LDTIMCR          (WIFI_LP_BASE + 0x0224)
#define LP_LMTBDTCR         (WIFI_LP_BASE + 0x0228)
#define LP_LFMSCR           (WIFI_LP_BASE + 0x022C)
#define LP_LWNMCR           (WIFI_LP_BASE + 0x0230)
// new for MT7637 new feature
#define LP_LFBCR0           (WIFI_LP_BASE + 0x0234)
#define LP_SBTOCR           (WIFI_LP_BASE + 0x0238)
#define LP_PBCR0            (WIFI_LP_BASE + 0x023C)
#define LP_NANCR0           (WIFI_LP_BASE + 0x0240)



/* WF_LP_AON for 6630 */
#define LPAON_LPCR             (WIFI_LPAON_BASE + 0x0000)
#define LPAON_LPCR_EN          (WIFI_LPAON_BASE + 0x0004)
#define LPAON_LPCR_DIS         (WIFI_LPAON_BASE + 0x0008)
#define LPAON_CCR0             (WIFI_LPAON_BASE + 0x000C)
#define LPAON_MTR0             (WIFI_LPAON_BASE + 0x002C)
#define LPAON_MTR1             (WIFI_LPAON_BASE + 0x0030)
#define LPAON_C32KCCR          (WIFI_LPAON_BASE + 0x0034)
#define LPAON_SMCTL0           (WIFI_LPAON_BASE + 0x0038)
#define LPAON_SMCTL1           (WIFI_LPAON_BASE + 0x003C)
#define LPAON_SMCTL2           (WIFI_LPAON_BASE + 0x0040)
#define LPAON_SMCTL3           (WIFI_LPAON_BASE + 0x0044)
#define LPAON_SMCTL4           (WIFI_LPAON_BASE + 0x0048)
#define LPAON_SMCTL5           (WIFI_LPAON_BASE + 0x004C)
#define LPAON_SMCTL6           (WIFI_LPAON_BASE + 0x0050)
#define LPAON_SMCTL7           (WIFI_LPAON_BASE + 0x0054)
#define LPAON_DUMMY0           (WIFI_LPAON_BASE + 0x0058)
/* WIFI LED for 6630 */
#define WFLED0CTRL             (WIFI_LPAON_BASE + 0x005C)
#define LED0_PR_S0             (WIFI_LPAON_BASE + 0x0060)
#define LED0_PR_S1             (WIFI_LPAON_BASE + 0x0064)

/* WFLED0CTRL */
#define WF_LED0_CTRL_KICK                 BIT(31)
#define WF_LED0_CTRL_MISC_CTRL            BIT(28)
#define WF_LED0_CTRL_MAIN_BLINK           BIT(27)
#define WF_LED0_CTRL_BLINK_MODE           BIT(26)
#define WF_LED0_CTRL_POLARITY             BIT(25)
#define WF_LED0_CTRL_REPLAY               BIT(24)
#define WF_LED0_CTRL_TX_BLINK_OFF_MASK    BITS(15,8)
#define WF_LED0_CTRL_TX_BLINK_OFF_OFFSET  8
#define WF_LED0_CTRL_TX_BLINK_ON_MASK     BITS(7,0)
#define WF_LED0_CTRL_TX_BLINK_ON_OFFSET   0

#define WF_LED0_CTRL_POLARITY_ACTIVE_HIGH 0
#define WF_LED0_CTRL_POLARITY_ACTIVE_LOW  1

/* LED0_PR_S0 */
#define WF_LED0_S0_OFF_MASK               BITS(31,24)
#define WF_LED0_S0_OFF_OFSET              24
#define WF_LED0_S0_ON_MASK                BITS(23,16)
#define WF_LED0_S0_ON_OFSET               16
#define WF_LED0_S0_LASTING_MASK           BITS(15,0)
#define WF_LED0_S0_LASTING_OFFSET         0

/* LED0_PR_S1 */
#define WF_LED0_S1_OFF_MASK               BITS(31,24)
#define WF_LED0_S1_OFF_OFSET              24
#define WF_LED0_S1_ON_MASK                BITS(23,16)
#define WF_LED0_S1_ON_OFSET               16
#define WF_LED0_S1_LASTING_MASK           BITS(15,0)
#define WF_LED0_S1_LASTING_OFFSET         0


/* chris 20140408  Add Definition for LED Emulation */
/* WIFI LED for MT7603 */
#define WF_LED_CTRL_KICK                  BIT(7)
#define WF_LED_CTRL_TX_OVER_BLINK         BIT(5)
#define WF_LED_CTRL_MAUAL_TX_BLINK        BIT(3)
#define WF_LED_CTRL_TX_BLINK_MODE         BIT(2)
#define WF_LED_CTRL_POLARITY              BIT(1)
#define WF_LED_CTRL_REPLAY                BIT(0)

#define WF_LED_CTRL_TX_BLINK_OFF_MASK     BITS(15,8)
#define WF_LED_CTRL_TX_BLINK_OFF_OFFSET   8
#define WF_LED_CTRL_TX_BLINK_ON_MASK      BITS(7,0)
#define WF_LED_CTRL_TX_BLINK_ON_OFFSET    0

#define WF_LED_PARAM_OFFSET               0x8

/*   */


/* THERMAL SENSOR */
#define THERM_THERMCR0      (WIFI_THERM_BASE + 0x0000)
#define THERM_THERMCR1      (WIFI_THERM_BASE + 0x0004)
#define THERM_THERMCR2      (WIFI_THERM_BASE + 0x0008)
#define THERM_TSTCS         (WIFI_THERM_BASE + 0x000C)
#define THERM_TSTDR         (WIFI_THERM_BASE + 0x0010)
#define THERM_DUMMY         (WIFI_THERM_BASE + 0x0014)
#define THERM_THERMCR3      (WIFI_THERM_BASE + 0x0018)
#define THERM_THERMISR      (WIFI_THERM_BASE + 0x001C)

#define MCU_CONFIG_MCCR     (MCU_CONFIG_BASE + 0x0100)

/* VOLTAGE SENSOR */
#define VOLTA_SARCR0        (TOP_SADC_BASE + 0x0320)
#define VOLTA_SARCR1        (TOP_SADC_BASE + 0x0324)
#define VOLTA_SARCR2        (TOP_SADC_BASE + 0x0328)

#define EMUL_TMTCR   CONFG_TMTCSR
#define EMUL_TMTDR   CONFG_TMTDR

/* CMDBT */
//#define  CMDBT_GPO                      (WIFI_CMDBT_BASE + 0x0004)
//#define  CMDBT_LPI                      (WIFI_CMDBT_BASE + 0x0008)
#define  CMDBT_FETCH_ADDR_WLANON_ON     (WIFI_CMDBT_ON_BASE + 0x0024)
#define  CMDBT_FETCH_ADDR_WLANON_SLEEP  (WIFI_CMDBT_ON_BASE + 0x0028)
#define  CMDBT_FETCH_ADDR_SLEEP_WLANON  (WIFI_CMDBT_ON_BASE + 0x002C)
#define  CMDBT_FETCH_ADDR_INIT_SLEEP    (WIFI_CMDBT_ON_BASE + 0x0030)
#define  CMDBT_FETCH_ADDR_NON_LPI       (WIFI_CMDBT_ON_BASE + 0x0034)
//#define  CMDBT_FETCH_ADDR_INTERRUPT     (WIFI_CMDBT_BASE + 0x006c)
//#define  CMDBT_GPI_TRG                  (WIFI_CMDBT_BASE + 0x0038)
//#define	 CMDBT_GPI_TRG_FALLING		(WIFI_CMDBT_BASE + 0x003C)
//


#define  CMDBT_TRG                      (WIFI_CMDBT_BASE + 0x0000)
#define  CMDBT_CTL                      (WIFI_CMDBT_ON_BASE + 0x0020)

#define  CMDBT_OPDA                     (WIFI_CMDBT_BASE + 0x0040)
#define  CMDBT_OPDB                     (WIFI_CMDBT_BASE + 0x0044)
#define  CMDBT_OPDC                     (WIFI_CMDBT_BASE + 0x0048)
#define  CMDBT_OPDD                     (WIFI_CMDBT_BASE + 0x004C)
#define  CMDBT_OPDE                     (WIFI_CMDBT_BASE + 0x0050)
#define  CMDBT_OPDF                     (WIFI_CMDBT_BASE + 0x0054)
#define  CMDBT_OPDG                     (WIFI_CMDBT_BASE + 0x0058)
#define  CMDBT_OPDH                     (WIFI_CMDBT_BASE + 0x005C)
#define  CMDBT_OPD_WR_MAC               (WIFI_CMDBT_BASE + 0x0060)
#define  CMDBT_OPD_WR_PHY               (WIFI_CMDBT_BASE + 0x0064)
#define  CMDBT_OPD_WR_WFON              (WIFI_CMDBT_BASE + 0x0068)

#define  CMDBT_INTA                     (WIFI_CMDBT_BASE + 0x0070)
#define  CMDBT_INTA_CLR                 (WIFI_CMDBT_BASE + 0x0074)
#define  CMDBT_STS                      (WIFI_CMDBT_BASE + 0x0080)
#define  CMDBT_RSV                      (WIFI_CMDBT_BASE + 0x0084)

#define  CMDBT_FETCH_ADDR_IRQ           (WIFI_CMDBT_ON_BASE + 0x0088)
#define  CMDBT_FETCH_ADDR_PSE_BACKUP    (WIFI_CMDBT_ON_BASE + 0x008C)
#define  CMDBT_FETCH_ADDR_PSE_RESTORE   (WIFI_CMDBT_ON_BASE + 0x0090)
#define  CMDBT_FETCH_ADDR_TOP_BACKUP    (WIFI_CMDBT_ON_BASE + 0x0094)
#define  CMDBT_FETCH_ADDR_TOP_RESTORE   (WIFI_CMDBT_ON_BASE + 0x0098)

/* this is used for ARM memory spacing (not for CMDBT internal use) */
#define  CMDBT_RAM_ENTRY_NUM            2048 /* in unit of 4 byte */

/***** DMA *****/
#define  DMA_DCR0            (WIFI_DMA_BASE + 0x0000)
//#define  DMA_DCR0           DMA_DCR
#define  DMA_DCR1           (WIFI_DMA_BASE + 0x0004)
//#define  DMA_MICR           DMA_DCR1
#define  DMA_FQCR0          (WIFI_DMA_BASE + 0x0008)
#define  DMA_FQCR1          (WIFI_DMA_BASE + 0x000C)
#define  DMA_VTR0           (WIFI_DMA_BASE + 0x0010)
#define  DMA_VTR1           (WIFI_DMA_BASE + 0x0014)
#define  DMA_VTR2           (WIFI_DMA_BASE + 0x0018)
#define  DMA_VTR3           (WIFI_DMA_BASE + 0x001C)
#define  DMA_VTR4           (WIFI_DMA_BASE + 0x0020)
#define  DMA_VTR5           (WIFI_DMA_BASE + 0x0024)
#define  DMA_VTR6           (WIFI_DMA_BASE + 0x0028)
#define  DMA_VTR7           (WIFI_DMA_BASE + 0x002C)
#define  DMA_VTR8           (WIFI_DMA_BASE + 0x0030)
#define  DMA_VTR9           (WIFI_DMA_BASE + 0x0034)
#define  DMA_VTR10          (WIFI_DMA_BASE + 0x0038)
#define  DMA_VTR11          (WIFI_DMA_BASE + 0x003C)
#define  DMA_VTR12          (WIFI_DMA_BASE + 0x0040)
#define  DMA_VTR13          (WIFI_DMA_BASE + 0x0044)
#define  DMA_VTR14          (WIFI_DMA_BASE + 0x0048)
#define  DMA_VTR15          (WIFI_DMA_BASE + 0x004C)
#define  DMA_VTR16          (WIFI_DMA_BASE + 0x0050)
#define  DMA_VTR17          (WIFI_DMA_BASE + 0x0054)

#define  DMA_ETBLR0         (WIFI_DMA_BASE + 0x0060)
#define  DMA_ETBLR1         (WIFI_DMA_BASE + 0x0064)
#define  DMA_ETBLR2         (WIFI_DMA_BASE + 0x0068)
#define  DMA_ETBLR3         (WIFI_DMA_BASE + 0x006C)
#define  DMA_RCFR0          (WIFI_DMA_BASE + 0x0070)
#define  DMA_RCFR1          (WIFI_DMA_BASE + 0x0074)
#define  DMA_RCFR2          (WIFI_DMA_BASE + 0x0078)
#define  DMA_VCFR0          (WIFI_DMA_BASE + 0x007C)
#define  DMA_TCFR0          (WIFI_DMA_BASE + 0x0080)
#define  DMA_TCFR1          (WIFI_DMA_BASE + 0x0084)
#define  DMA_TMCFR0         (WIFI_DMA_BASE + 0x0088)

#define  DMA_DBG0           (WIFI_DMA_BASE + 0x00D0)
/***** End of DMA *****/


/* Security */
#define  SEC_WPCR           (WIFI_SEC_BASE + 0x0000)
#define  SEC_SCR            (WIFI_SEC_BASE + 0x0004)
#define  SEC_SSR            (WIFI_SEC_BASE + 0x0010)

//#define  SEC_TTR0           (WIFI_SEC_BASE + 0x00B0)
//#define  SEC_TTR1           (WIFI_SEC_BASE + 0x00B4)
//#define  SEC_WPR0           (WIFI_SEC_BASE + 0x00C0)
//#define  SEC_WPR1           (WIFI_SEC_BASE + 0x00C4)
//#define  SEC_WPR2           (WIFI_SEC_BASE + 0x00C8)
//#define  SEC_WPR3           (WIFI_SEC_BASE + 0x00CC)


#define SEC_SFR1            (WIFI_SEC_BASE + 0x0018)
#define SEC_SMACR           (WIFI_SEC_BASE + 0x0070)
#define SEC_AKR0            (WIFI_SEC_BASE + 0x0080)
#define SEC_AKR1            (WIFI_SEC_BASE + 0x0084)
#define SEC_AKR2            (WIFI_SEC_BASE + 0x0088)
#define SEC_AKR3            (WIFI_SEC_BASE + 0x008C)
#define SEC_AIDR0           (WIFI_SEC_BASE + 0x0090)
#define SEC_AIDR1           (WIFI_SEC_BASE + 0x0094)
#define SEC_AIDR2           (WIFI_SEC_BASE + 0x0098)
#define SEC_AIDR3           (WIFI_SEC_BASE + 0x009c)
#define SEC_AODR0           (WIFI_SEC_BASE + 0x00a0)
#define SEC_AODR1           (WIFI_SEC_BASE + 0x00a4)
#define SEC_AODR2           (WIFI_SEC_BASE + 0x00a8)
#define SEC_AODR3           (WIFI_SEC_BASE + 0x00ac)


/* SEC SMACR */
#define SEC_SMACR_SW_MODE           BIT(0)
#define SEC_SMACR_KE_SKIP           BIT(1)
#define SEC_SMACR_AES_START         BIT(2)
#define SEC_SMACR_AES_DONE          BIT(3)
#define SEC_SMACR_SW_SECRET_KEY_SEL BITS(12, 13)
#define SEC_SMACR_SW_AES2_EN        BIT(15)

/* SEC_SCR */
#define SCR_NON_WPI_ORDER_MASK      BIT(16)


#define  APRCY_SMACR          (WIFI_SEC_BASE + 0x0070)
#define     FIELD_SW_AES2_EN                            BIT(15)
#define     FIELD_SW_AES2_EN_OFFSET                     15
#define     FIELD_SW_SECRET_KEY_SEL                     BITS(12,13)
#define     FIELD_SW_SECRET_KEY_SEL_OFFSET              12
#define     FIELD_AES_DONE                              BIT(3)
#define     FIELD_AES_DONE_OFFSET                       3
#define     FIELD_AES_START                             BIT(2)
#define     FIELD_AES_START_OFFSET                      2
#define     FIELD_KEY_SKIP                              BIT(1)
#define     FIELD_KEY_SKIP_OFFSET                       1
#define     FIELD_SW_AES_MODE                           BIT(0)
#define     FIELD_SW_AES_MODE_OFFSET                    0
#define  APRCY_AKR0           (WIFI_SEC_BASE + 0x0080)
#define  APRCY_AKR1           (WIFI_SEC_BASE + 0x0084)
#define  APRCY_AKR2           (WIFI_SEC_BASE + 0x0088)
#define  APRCY_AKR3           (WIFI_SEC_BASE + 0x008C)
#define  APRCY_AIDR0          (WIFI_SEC_BASE + 0x0090)
#define  APRCY_AIDR1          (WIFI_SEC_BASE + 0x0094)
#define  APRCY_AIDR2          (WIFI_SEC_BASE + 0x0098)
#define  APRCY_AIDR3          (WIFI_SEC_BASE + 0x009C)
#define  APRCY_AODR0          (WIFI_SEC_BASE + 0x00A0)
#define  APRCY_AODR1          (WIFI_SEC_BASE + 0x00A4)
#define  APRCY_AODR2          (WIFI_SEC_BASE + 0x00A8)
#define  APRCY_AODR3          (WIFI_SEC_BASE + 0x00AC)
#define  SEC_TTR0             (WIFI_SEC_BASE + 0x00B0)
#define  SEC_TTR1             (WIFI_SEC_BASE + 0x00B4)
#define  SEC_WPR0             (WIFI_SEC_BASE + 0x00C0)
#define  SEC_WPR1             (WIFI_SEC_BASE + 0x00C4)
#define  SEC_WPR2             (WIFI_SEC_BASE + 0x00C8)
#define  SEC_WPR3             (WIFI_SEC_BASE + 0x00CC)

/***** TOP_RGU *****/
#ifndef RGU_WCR
#define  RGU_WCR                  (TOP_RGU_BASE + 0x0000)
#endif
#define  RGU_WFPSE_MEM_INIT       (TOP_RGU_BASE + 0x0014)
#define  RGU_WFSYS_MEM_PDN_EN     (TOP_RGU_BASE + 0x0060)
#define  RGU_WFSYS_MEM_SLP_EN     (TOP_RGU_BASE + 0x0064)
#define  RGU_WFSYS_MEM_PDN        (TOP_RGU_BASE + 0x0068)
#define  RGU_WFSYS_MEM_PON        (TOP_RGU_BASE + 0x006C)
#define  RGU_WFSYS_MEM_SLP        (TOP_RGU_BASE + 0x0070)
#define  RGU_WFSYS_MEM_WKUP       (TOP_RGU_BASE + 0x0074)
#define  RGU_WFSYS_MEM_ISO        (TOP_RGU_BASE + 0x0078)
#define  RGU_WFSYS_MEM_DISISO     (TOP_RGU_BASE + 0x007C)

#define  RGU_WFPSE_MEM_PDN_EN     (TOP_RGU_BASE + 0x0080)
#define  RGU_WFPSE_MEM_SLP_EN     (TOP_RGU_BASE + 0x0084)
#define  RGU_WFPSE_MEM_PDN        (TOP_RGU_BASE + 0x0088)
#define  RGU_WFPSE_MEM_PON        (TOP_RGU_BASE + 0x008C)
#define  RGU_WFPSE_MEM_SLP        (TOP_RGU_BASE + 0x0090)
#define  RGU_WFPSE_MEM_WKUP       (TOP_RGU_BASE + 0x0094)
#define  RGU_WFPSE_MEM_ISO        (TOP_RGU_BASE + 0x0098)
#define  RGU_WFPSE_MEM_DISISO     (TOP_RGU_BASE + 0x009C)

#define  RGU_WFIDLM_MEM_PDN_EN    (TOP_RGU_BASE + 0x00A0)
#define  RGU_WFIDLM_MEM_SLP_EN    (TOP_RGU_BASE + 0x00A4)
#define  RGU_WFIDLM_MEM_PDN       (TOP_RGU_BASE + 0x00A8)
#define  RGU_WFIDLM_MEM_PON       (TOP_RGU_BASE + 0x00AC)
#define  RGU_WFIDLM_MEM_SLP       (TOP_RGU_BASE + 0x00B0)
#define  RGU_WFIDLM_MEM_WKUP      (TOP_RGU_BASE + 0x00B4)
#define  RGU_WFIDLM_MEM_ISO       (TOP_RGU_BASE + 0x00B8)
#define  RGU_WFIDLM_MEM_DISISO    (TOP_RGU_BASE + 0x00BC)

#define  RGU_WFSYS_MEM_PDN_STAT   (TOP_RGU_BASE + 0x00C0)
#define  RGU_WFSYS_MEM_SLP_STAT   (TOP_RGU_BASE + 0x00C4)
#define  RGU_WFSYS_MEM_CS_STAT    (TOP_RGU_BASE + 0x00C8)

#define  RGU_WFPSE_MEM_PDN_STAT   (TOP_RGU_BASE + 0x00D0)
#define  RGU_WFPSE_MEM_SLP_STAT   (TOP_RGU_BASE + 0x00D4)
#define  RGU_WFPSE_MEM_CS_STAT    (TOP_RGU_BASE + 0x00D8)

#define  RGU_WFIDLM_MEM_PDN_STAT  (TOP_RGU_BASE + 0x00E0)
#define  RGU_WFIDLM_MEM_SLP_STAT  (TOP_RGU_BASE + 0x00E4)
#define  RGU_WFIDLM_MEM_CS_STAT   (TOP_RGU_BASE + 0x00E8)

#define  RGU_WFSYS_DLY_CHAIN_CTL  (TOP_RGU_BASE + 0x00F0)
#define  RGU_WFPSE_DLY_CHAIN_CTL  (TOP_RGU_BASE + 0x00F4)
#define  RGU_WFIDLM_DLY_CHAIN_CTL (TOP_RGU_BASE + 0x00F8)

#define  RGU_WF_RGDIG_CTL         (TOP_RGU_BASE + 0x00FC)

#define  RGU_WFPSE_MEM_RSTB       (TOP_RGU_BASE + 0x0104)

#if 0
/* WIFI LED PIN AUX for 6630 */
#define  RGU_AGPS_SYNC_AON_SEL    (TOP_RGU_BASE + 0x3014)
#define  WF_LED_PIN_MUX_MASK             BITS(3,2)
#define  WF_LED_PIN_MUX_OFFSET           2
#endif


/***** TOP_GPIO *****/
#if (PRODUCT_VERSION == 6630)
#define  GPIO_PINMUX_AON_SEL      (TOP_GPIO_BASE + 0x001C)
#elif (PRODUCT_VERSION == 7637)
    #define  GPIO_PINMUX_AON_SEL      (TOP_GPIO_BASE + 0x008C)
#else
#define  GPIO_PINMUX_AON_SEL      (TOP_GPIO_BASE + 0x003C)
#endif

#if (PRODUCT_VERSION == 7637)
#define  GPIO_TOP_AON_DBG_FLG_SEL (TOP_GPIO_BASE + 0x0088)
#else
#define  GPIO_TOP_AON_DBG_FLG_SEL (TOP_GPIO_BASE + 0x0038)
#endif

/***** TOP_CFG_AON *****/
#define TOP_CFG_PWRCTLCR          (TOP_CFG_AON_BASE + 0x110)


/* chris 20140408  Add Definition for LED Emulation */
/* WIFI LED PIN AUX for MT7603 */
#define TOP_GPIO_PINMUX_SEL1      (TOP_GPIO_BASE + 0x0014)
//#define TOP_GPIO_0_SEL_MASK       BITS(3,0)
#define TOP_GPIO_0_SEL_MASK       BITS(10,8)
#define TOP_GPIO_0_SEL_OFFSET     8
#if 0
/* LED */
#define LED_BASE                  (0x81021500)
#define LED_CTRL                  (LED_BASE  + 0x0000)
#define LED_TX_BLINK_PARAM0       (LED_BASE  + 0x0004)
#define LED_TX_BLINK_PARAM1       (LED_BASE  + 0x0008)
#define LED_0_PARAM_S0            (LED_BASE  + 0x0010)
#define LED_0_PARAM_S1            (LED_BASE  + 0x0014)
#endif
/*     */


/***** Bit definition *****/

/***** WTBL *****/
/* WTBL 1 */
/* DW 0*/
#define WTBL_WM_BURST                   BIT(31)
#define WTBL_SW                         BIT(30)
#define WTBL_RCA2                       BIT(29)
#define WTBL_RV                         BIT(28)
#define WTBL_IKV                        BIT(27)
#define WTBL_RKV                        BIT(26)
#define WTBL_RCID                       BIT(25)
#define WTBL_KID_MASK                   BITS(23,24)
#define WTBL_KID_OFFSET                 23
#define WTBL_RCA1                       BIT(22)
#define WTBL_MUAR_IDX_MASK              BITS(16,21)
#define WTBL_MUAR_IDX_OFFSET            16

/* DW 2 */
#define WTBL_GID_63                     BIT(31)
#define WTBL_ADM                        BIT(30)
#define WTBL_VHT                        BIT(29)
#define WTBL_HT                         BIT(28)
#define WTBL_QOS                        BIT(27)
#define WTBL_MESH                       BIT(26)
#define WTBL_TXOP_PS_CAP                BIT(25)
#define WTBL_TEBF                       BIT(24)
#define WTBL_TIBF                       BIT(23)
#define WTBL_LDPC                       BIT(22)
#define WTBL_BYN_BW                     BIT(21)
#define WTBL_BAF_EN                     BIT(20)
#define WTBL_SMPS                       BIT(19)
#define WTBL_RDGBA                      BIT(18)
#define WTBL_CFACK                      BIT(17)
#define WTBL_RTS                        BIT(16)
#define WTBL_RDG                        BIT(15)
#define WTBL_POWER_SAVE_STATUS          BIT(14)

#if 0   //Posh 20131112, Old design on MT6630 E1
#define WTBL_AF_MASK                    BITS(11,13)
#define WTBL_AF_OFFSET                  11
#else
#define WTBL_VHT_LDPC                   BIT(11)
#define WTBL_VHT_TIBF                   BIT(12)
#define WTBL_VHT_TEBF                   BIT(13)
#endif

#define WTBL_DIS_RHTR                   BIT(10)
#define WTBL_FROM_DS                    BIT(9)
#define WTBL_TO_DS                      BIT(8)
#define WTBL_WPI_EVEN                   BIT(7)
#define WTBL_CIPHER_SUITE_MASK          BITS(3,6)
#define WTBL_CIPHER_SUITE_OFFSET        3

#if 0   //Posh 20131112, Old design on MT6630 E1
#define WTBL_MM_MASK                    BITS(0,2)
#endif

/* DW 3 */
#define WTBL_TBL2_FID_MASK             BITS(0,10)
#define WTBL_TBL2_EID_MASK             BITS(11,15)
#define WTBL_TBL2_EID_OFFSET           11
#define WTBL_TBL4_FID_MASK             BITS(16,26)
#define WTBL_TBL4_FID_OFFSET           16
#define WTBL_CHK_PER                   BIT(27)
#define WTBL_DONOT_UPDATE_I_PSM        BIT(28)
#define WTBL_I_PSM                     BIT(29)
#define WTBL_PSM                       BIT(30)
#define WTBL_SKIP_TX                   BIT(31)

/* DW 4 */
#define WTBL_TBL3_FID_MASK             BITS(0,10)
#define WTBL_TBL3_EID_MASK             BITS(11,16)
#define WTBL_TBL3_EID_OFFSET           11
#define WTBL_TBL4_EID_MASK             BITS(17,22)
#define WTBL_TBL4_EID_OFFSET           17
#define WTBL_PARTIAL_AID_MASK          BITS(23,31)
#define WTBL_PARTIAL_AID_OFFSET        23

/* WTBL1 OR */
#define WTBL_WTBL1_PSM                 BIT(31)
#define WTBL_WTBL1_IPSM                BIT(30)


/* WTBL 2 */

/* DW 1 */
#define WTBL_TBL2_COM_SN_MASK                               BITS(16,27)
#define WTBL_TBL2_COM_SN_OFFSET                             16

/* DW 2 */
#define WTBL_TBL2_TID0_AC0_SN_MASK                          BITS(0,11)
#define WTBL_TBL2_TID0_AC0_SN_OFFSET                        0
#define WTBL_TBL2_TID1_AC1_SN_MASK                          BITS(12,23)
#define WTBL_TBL2_TID1_AC1_SN_OFFSET                        12
#define WTBL_TBL2_TID2_AC2_SN_0_7_MASK                      BITS(24,31)
#define WTBL_TBL2_TID2_AC2_SN_0_7_OFFSET                    24

/* DW 3 */
#define WTBL_TBL2_TID2_AC2_SN_8_11_MASK                     BITS(0,3)
#define WTBL_TBL2_TID2_AC2_SN_8_11_OFFSET                   0
#define WTBL_TBL2_TID3_AC3_SN_MASK                          BITS(4,15)
#define WTBL_TBL2_TID3_AC3_SN_OFFSET                        4
#define WTBL_TBL2_TID4_SN_MASK                              BITS(16,27)
#define WTBL_TBL2_TID4_SN_OFFSET                            16
#define WTBL_TBL2_TID5_SN_0_3_MASK                          BITS(28,31)
#define WTBL_TBL2_TID5_SN_0_3_OFFSET                        28

/* DW 4 */
#define WTBL_TBL2_TID5_SN_4_11_MASK                         BITS(0,7)
#define WTBL_TBL2_TID5_SN_4_11_OFFSET                       0
#define WTBL_TBL2_TID6_SN_MASK                              BITS(8,19)
#define WTBL_TBL2_TID6_SN_OFFSET                            8
#define WTBL_TBL2_TID7_SN_MASK                              BITS(20,31)
#define WTBL_TBL2_TID7_SN_OFFSET                            20

/*DW 5*/
#define WTBL_TBL2_RATE1_TX_CNT_MASK                         BITS(0,15)
#define WTBL_TBL2_RATE1_TX_CNT_OFFSET                       0
#define WTBL_TBL2_RATE1_FAIL_CNT_MASK                       BIT(16,31)
#define WTBL_TBL2_RATE1_FAIL_CNT_OFFSET                     16


/*DW 6*/
#define WTBL_TBL2_RATEN_TX_CNT_MASK                         BITS(0,15)
#define WTBL_TBL2_RATEN_TX_CNT_OFFSET                       0
#define WTBL_TBL2_RATEN_FAIL_CNT_MASK                       BIT(16,31)
#define WTBL_TBL2_RATEN_FAIL_CNT_OFFSET                     16


/*DW 7*/
#define WTBL_TBL2_CURRENT_BW_TX_CNT_MASK                    BITS(0,15)
#define WTBL_TBL2_CURRENT_BW_TX_CNT_OFFSET                  0
#define WTBL_TBL2_CURRENT_BW_FAIL_CNT_MASK                  BIT(16,31)
#define WTBL_TBL2_CURRENT_BW_FAIL_CNT_OFFSET                16


/*DW 8*/
#define WTBL_TBL2_OTHER_BW_TX_CNT_MASK                      BITS(0,15)
#define WTBL_TBL2_OTHER_BW_TX_CNT_OFFSET                    0
#define WTBL_TBL2_OTHER_BW_FAIL_CNT_MASK                    BIT(16,31)
#define WTBL_TBL2_OTHER_BW_FAIL_CNT_OFFSET                  16


/* DW 9 */
#define WTBL_TBL2_TX_POWER_OFFSET_MASK BITS(0,4)
#define WTBL_TBL2_SPE_EN               BIT(5)
#define WTBL_TBL2_SPE_EN_OFFSET        5

#define WTBL_TBL2_ANT_PRI_MASK         BITS(6,8)
#define WTBL_TBL2_ANT_PRI_OFFSET       6
#define WTBL_TBL2_CCBW_40MHZ           BIT(9)
#define WTBL_TBL2_CCBW_80MHZ           BIT(10)
#define WTBL_TBL2_CCBW_160MHZ          BITS(9,10)
#define WTBL_TBL2_CCBW_160MHZ_OFFSET           9

#define WTBL_TBL2_CBRN_MASK            BITS(11,13)
#define WTBL_TBL2_CBRN_OFFSET          11
#define WTBL_TBL2_FCAP_20_40_MHZ       BIT(14)
#define WTBL_TBL2_FCAP_20_TO_80_MHZ    BIT(15)
#define WTBL_TBL2_FCAP_20_TO_160_MHZ   BITS(14,15)
#define WTBL_TBL2_FCAP_20_TO_160_MHZ_OFFSET   14

#define WTBL_TBL2_G2                   BIT(16)
#define WTBL_TBL2_G4                   BIT(17)
#define WTBL_TBL2_G8                   BIT(18)
#define WTBL_TBL2_G16                  BIT(19)
#define WTBL_TBL2_G2_OFFSET                   16
#define WTBL_TBL2_G4_OFFSET                   17
#define WTBL_TBL2_G8_OFFSET                   18
#define WTBL_TBL2_G16_OFFSET                  19

#define WTBL_TBL2_RATE_N_SEL_MASK      BITS(20,22)
#define WTBL_TBL2_RATE_N_SEL_OFFSET    20
#define WTBL_TBL2_MPDU_FAIL_CNT_MASK   BITS(23,25)
#define WTBL_TBL2_MPDU_FAIL_CNT_OFFSET 23
#define WTBL_TBL2_MPDU_TX_CNT_MASK     BITS(26,28)
#define WTBL_TBL2_MPDU_TX_CNT_OFFSET   26

#define WTBL_TBL2_RATE_IDX_MASK        BITS(29,31)
#define WTBL_TBL2_RATE_IDX_OFFSET      29


/* DW 10 */
#define WTBL_TBL2_RATE1_MASK           BITS(0,11)
#define WTBL_TBL2_RATE2_MASK           BITS(12,23)
#define WTBL_TBL2_RATE2_OFFSET         12
#define WTBL_TBL2_RATE3_D10_MASK       BITS(24,31)
#define WTBL_TBL2_RATE3_D10_OFFSET     24


/* DW 11 */
#define WTBL_TBL2_RATE3_D11_MASK       BITS(0,3)
#define WTBL_TBL2_RATE3_D11_OFFSET     0
#define WTBL_TBL2_RATE4_MASK           BITS(4,15)
#define WTBL_TBL2_RATE4_OFFSET         4
#define WTBL_TBL2_RATE5_MASK           BITS(16,27)
#define WTBL_TBL2_RATE5_OFFSET         16
#define WTBL_TBL2_RATE6_D11_MASK       BITS(28,31)
#define WTBL_TBL2_RATE6_D11_OFFSET     28


/* DW 12 */
#define WTBL_TBL2_RATE6_D12_MASK       BITS(0,7)
#define WTBL_TBL2_RATE6_D12_OFFSET     0
#define WTBL_TBL2_RATE7_MASK           BITS(8,19)
#define WTBL_TBL2_RATE7_OFFSET         8
#define WTBL_TBL2_RATE8_MASK           BITS(20,31)
#define WTBL_TBL2_RATE8_OFFSET         20

/*DW 13*/
#define WTBL_TBL2_RCPI0_MASK            BITS(0,7)
#define WTBL_TBL2_RCPI0_OFFSET          0
#define WTBL_TBL2_RCPI1_MASK            BITS(8,15)
#define WTBL_TBL2_RCPI1_OFFSET          8
#define WTBL_TBL2_RCPI2_MASK            BITS(16,23)
#define WTBL_TBL2_RCPI2_OFFSET          16

//Posh 20131112, Add since MT7636
#define WTBL_TBL2_MM_MASK               BITS(26,28)
#define WTBL_TBL2_MM_OFFSET             26
#define WTBL_TBL2_AF_MASK               BITS(29,31)
#define WTBL_TBL2_AF_OFFSET             29


/* DW  14*/
#define WTBL_TBL2_1STS_CHANNEL_CAPACITY_OR_NOISE_MASK       BITS(0,6)
#define WTBL_TBL2_1STS_CHANNEL_CAPACITY_OR_NOISE_OFFSET     0
#define WTBL_TBL2_2STS_CHANNEL_CAPACITY_OR_NOISE_MASK       BITS(7,13)
#define WTBL_TBL2_2STS_CHANNEL_CAPACITY_OR_NOISE_OFFSET     7
#define WTBL_TBL2_3STS_CHANNEL_CAPACITY_OR_NOISE_MASK       BITS(14,20)
#define WTBL_TBL2_3STS_CHANNEL_CAPACITY_OR_NOISE_OFFSET     14

#define WTBL_TBL2_CE_RMSD_MASK                              BITS(21,24)
#define WTBL_TBL2_CE_RMSD_OFFSET                            21
#define WTBL_TBL2_ANT_SEL_MASK                              BITS(26,31)
#define WTBL_TBL2_ANT_SEL_OFFSET                            26
#define WTBL_TBL2_CHANNEL_CAPACITY_OR_NOISE_SEL             BIT(25)


/* DW 15 */
#define WTBL_TBL2_BA_EN_MASK                BITS(24,31)
#define WTBL_TBL2_BA_EN_OFFSET              24
#define WTBL_TBL2_BA_WIN_SIZE_TID_MASK      BITS(0,2)
#define WTBL_TBL2_BA_WIN_SIZE_TID_LEN       3

//TBD
#define WTBL_G2                         BIT(16)
#define WTBL_FCAP_20_40_MHZ             BIT(15)
#define WTBL_G4                         BIT(11)
#define WTBL_BA_CHECK                   BIT(9)


/* TSR */
#define TSR_DLS_TIMEOUT_WLAN_IDX        BITS(12,15)
#define TSR_DLS_TIMEOUT_TID             BITS(8,10)
#define TSR_BA_TIMEOUT_WLAN_IDX         BITS(4,7)
#define TSR_BA_TIMEOUT_TID              BITS(0,2)


/***** TRB *****/


/***** AGG *****/
/* AALCR1 */
#define AALCR1_AC13_AGG_LIMIT            BITS(24,28)
#define AALCR1_AC13_AGG_LIMIT_OFFSET     24
#define AALCR1_AC12_AGG_LIMIT            BITS(16,20)
#define AALCR1_AC12_AGG_LIMIT_OFFSET     16
#define AALCR1_AC11_AGG_LIMIT            BITS(8,12)
#define AALCR1_AC11_AGG_LIMIT_OFFSET     8
#define AALCR1_AC10_AGG_LIMIT            BITS(0,4)
#define AALCR1_AC10_AGG_LIMIT_OFFSET     0

/* SCR */
#define SCR_SPARE_CR                   BITS(0,31)

/* SMCR */
#define SMCR_AGG_CS                    BITS(12,14)
#define SMCR_AGG_CS_OFFSET      12
#define SMCR_PP_CS                       BITS(8,10)
#define SMCR_PP_CS_OFFSET         8
#define SMCR_PS_CS                       BITS(4,6)
#define SMCR_PS_CS_OFFSET         4
#define SMCR_FLUSH_CS                 BITS(0,3)
#define SMCR_FLUSH_CS_OFFSET    0

/* ARUCR */
#define ARUCR_RATE4_UP_MPDU_LIMIT    BITS(12,15)
#define ARUCR_RATE4_UP_MPDU_LIMIT_OFFSET        12
#define ARUCR_RATE3_UP_MPDU_LIMIT    BITS(8,10)
#define ARUCR_RATE3_UP_MPDU_LIMIT_OFFSET        8
#define ARUCR_RATE2_UP_MPDU_LIMIT    BITS(4,6)
#define ARUCR_RATE2_UP_MPDU_LIMIT_OFFSET        4
#define ARUCR_RATE1_UP_MPDU_LIMIT    BITS(0,2)
#define ARUCR_RATE1_UP_MPDU_LIMIT_OFFSET        0

#define ARUCR_RATE8_UP_MPDU_LIMIT    BITS(28,30)
#define ARUCR_RATE8_UP_MPDU_LIMIT_OFFSET        28
#define ARUCR_RATE7_UP_MPDU_LIMIT    BITS(24,26)
#define ARUCR_RATE7_UP_MPDU_LIMIT_OFFSET        24
#define ARUCR_RATE6_UP_MPDU_LIMIT    BITS(20,22)
#define ARUCR_RATE6_UP_MPDU_LIMIT_OFFSET        20
#define ARUCR_RATE5_UP_MPDU_LIMIT    BITS(16,18)
#define ARUCR_RATE5_UP_MPDU_LIMIT_OFFSET        16
/* TICR */
#define TICR_TXD_INIT                  BIT(0)

/* PCR */
#define PCR_ERP_PROTECTION_EN           BIT(24)
#define PCR_BW160_PROTECTION_EN         BIT(23)
#define PCR_BW80_PROTECTION_EN          BIT(22)
#define PCR_RIFS_PROTECTION_EN          BIT(21)
#define PCR_BW40_PROTECTION_EN          BIT(20)
#define PCR_PROTECTION_MODE             BIT(19)
#define PCR_GF_PROTECTION_EN            BIT(17)
#define PCR_MM_PROTECTION_EN            BIT(16)

/* PCR1 */
#define PCR1_RTS_THRESHOLD_IN_PKT_NUM_DISABLE       BIT(31)
#define PCR1_RTS_THRESHOLD_IN_PKT_NUM               BITS(25,30)
#define PCR1_RTS_THRESHOLD_IN_PKT_NUM_OFFSET        25
#define PCR1_RTS_THRESHOLD_IN_PKT_LEN               BITS(0,19)
#define PCR1_RTS_THRESHOLD_IN_PKT_LEN_OFFSET        0

/* ASRCR */
#define ASRCR_AGG_CNT_RANGE_MASK                    BITS(0,5)
#define ASRCR_AGG_CNT_RANGE0                        BITS(0,5)
#define ASRCR_AGG_CNT_RANGE0_OFFSET                 0
#define ASRCR_AGG_CNT_RANGE1                        BITS(8,13)
#define ASRCR_AGG_CNT_RANGE1_OFFSET                 8
#define ASRCR_AGG_CNT_RANGE2                        BITS(16,21)
#define ASRCR_AGG_CNT_RANGE2_OFFSET                 16

/* BQCR */
#define BQCR_BCN_DST_QID_MASK           BITS(2,6)
#define BQCR_BCN_DST_QID_OFFSET         2
#define BQCR_BCN_DST_PID_MASK           BITS(0,1)
#define BQCR_BCN_DST_PID_OFFSET         0


/* BWCR */
#define BWCR_MCS32_TO_0_EN                BIT(15)
#define BWCR_HONOR_CFP_DURA_REMAINING  		BIT(5)
#define BWCR_RF_BW_MASK                   BITS(2,3)
#define BWCR_RF_BW_OFFSET                 2
#define BWCR_PCO_ACTIVE                   BIT(1)
#define BWCR_PCO_PHASE                    BIT(0)

/* ACR */
#define ACR_BAR_RATE                        BITS(20,31)
#define ACR_BAR_RATE_OFFSET                 20
#define ACR_CFEND_RATE                      BITS(4,15)
#define ACR_CFEND_RATE_OFFSET               4
#define ACR_AMPDU_NO_BA_AR_RULE             BIT(1)
#define ACR_AMPDU_NO_BA_AR_RULE_OFFSET      1
#define ACR_AMPDU_NO_BA_RULE                BIT(0)
#define ACR_AMPDU_NO_BA_RULE_OFFSET         0

/* DSCR1 */ //Posh 20131212, Add DMA scheduler
#define DSCR1_DISABLE_DMA_SHDL              BIT(31)
#define DSCR1_DISABLE_DMA_SHDL_OFFSET       31

/* MRCR */
#define MRCR_BAR_TX_CNT_LIMIT            BITS(12,15)
#define MRCR_BAR_TX_CNT_LIMIT_OFFSET      12
#define MRCR_RTS_RTY_CNT_LIMIT          BITS(7,11)
#define MRCR_RTS_RTY_CNT_LIMIT_OFFSET      7


/* ARDCR */
#define ARDCR_RATE8_DOWN_MPDU_LIMIT    BITS(28,30)
#define ARDCR_RATE8_DOWN_MPDU_LIMIT_OFFSET        28
#define ARDCR_RATE7_DOWN_MPDU_LIMIT    BITS(24,26)
#define ARDCR_RATE7_DOWN_MPDU_LIMIT_OFFSET        24
#define ARDCR_RATE6_DOWN_MPDU_LIMIT    BITS(20,22)
#define ARDCR_RATE6_DOWN_MPDU_LIMIT_OFFSET        20
#define ARDCR_RATE5_DOWN_MPDU_LIMIT    BITS(16,18)
#define ARDCR_RATE5_DOWN_MPDU_LIMIT_OFFSET        16
#define ARDCR_RATE4_DOWN_MPDU_LIMIT    BITS(12,15)
#define ARDCR_RATE4_DOWN_MPDU_LIMIT_OFFSET        12
#define ARDCR_RATE3_DOWN_MPDU_LIMIT    BITS(8,10)
#define ARDCR_RATE3_DOWN_MPDU_LIMIT_OFFSET        8
#define ARDCR_RATE2_DOWN_MPDU_LIMIT    BITS(4,6)
#define ARDCR_RATE2_DOWN_MPDU_LIMIT_OFFSET        4
#define ARDCR_RATE1_DOWN_MPDU_LIMIT    BITS(0,2)
#define ARDCR_RATE1_DOWN_MPDU_LIMIT_OFFSET        0

/* AALCR */
#define AALCR_AC3_AGG_LIMIT             BITS(24,29)
#define AALCR_AC3_AGG_LIMIT_OFFSET      24
#define AALCR_AC2_AGG_LIMIT             BITS(16,21)
#define AALCR_AC2_AGG_LIMIT_OFFSET      16
#define AALCR_AC1_AGG_LIMIT             BITS(8,13)
#define AALCR_AC1_AGG_LIMIT_OFFSET      8
#define AALCR_AC0_AGG_LIMIT             BITS(0,5)
#define AALCR_AC0_AGG_LIMIT_OFFSET      0

/* AWSCR */
#define AWSCR_WIN_SIZE_ENTRY_MASK        BITS(0,5)
#define AWSCR_WIN_SIZE_ENTRY_LENGTH      8

/* DWER */

/* DWER2 */
#define DWER2_FROM_DS                       BIT(18)
#define DWER2_FROM_DS_OFFSET           18
#define DWER2_TO_DS                           BIT(17)
#define DWER2_TO_DS_OFFSET               17
#define DWER2_PS_STATUS                    BIT(16)
#define DWER2_PS_STATUS_OFFSET       16
#define DWER2_PEER_ADDR                    BITS(0,15)

/*
 * AGG ARCR
 */
#define AGG_ARCR_RTS_RATE_DOWN_TH_MASK              BITS(8, 12)
#define AGG_ARCR_RTS_RATE_DOWN_TH_OFFSET            8
#define AGG_ARCR_RATE_DOWN_EXTRA_RATIO_MASK         BITS(16, 17)
#define AGG_ARCR_RATE_DOWN_EXTRA_RATIO_OFFSET       16
#define AGG_ARCR_RATE_DOWN_EXTRA_RATIO_EN           BIT(19)
#define AGG_ARCR_RATE_UP_EXTRA_TH_MASK              BITS(20, 22)
#define AGG_ARCR_RATE_UP_EXTRA_TH_OFFSET            20
#define AGG_ARCR_SPE_DIS_TH_MASK                    BITS(24, 27)
#define AGG_ARCR_SPE_DIS_TH_OFFSET                  24


/***** ARB *****/
/* TQCR0 */
#define TQCR0_AC5_START4                 BIT(30)
#define TQCR0_AC5_START3                 BIT(29)
#define TQCR0_AC5_START2                 BIT(28)
#define TQCR0_AC5_START1                 BIT(27)
#define TQCR0_AC5_START0                 BIT(26)
#define TQCR0_AC4_START4                 BIT(25)
#define TQCR0_AC4_START3                 BIT(24)
#define TQCR0_AC4_START2                 BIT(23)
#define TQCR0_AC4_START1                 BIT(22)
#define TQCR0_AC4_START0                 BIT(21)
#define TQCR0_AC3_START4                 BIT(20)
#define TQCR0_AC3_START3                 BIT(19)
#define TQCR0_AC3_START2                 BIT(18)
#define TQCR0_AC3_START1                 BIT(17)
#define TQCR0_AC3_START0                 BIT(16)

#define TQCR0_AC2_START4                 BIT(14)
#define TQCR0_AC2_START3                 BIT(13)
#define TQCR0_AC2_START2                 BIT(12)
#define TQCR0_AC2_START1                 BIT(11)
#define TQCR0_AC2_START0                 BIT(10)
#define TQCR0_AC1_START4                 BIT(9)
#define TQCR0_AC1_START3                 BIT(8)
#define TQCR0_AC1_START2                 BIT(7)
#define TQCR0_AC1_START1                 BIT(6)
#define TQCR0_AC1_START0                 BIT(5)
#define TQCR0_AC0_START4                 BIT(4)
#define TQCR0_AC0_START3                 BIT(3)
#define TQCR0_AC0_START2                 BIT(2)
#define TQCR0_AC0_START1                 BIT(1)
#define TQCR0_AC0_START0                 BIT(0)
//#define TQCR0_AC_START_OFFSET            0

#define TQCR0_AC0_TO_AC4_START0       (TQCR0_AC0_START0 | TQCR0_AC1_START0 | TQCR0_AC2_START0 | TQCR0_AC3_START0 | TQCR0_AC4_START0)
#define TQCR0_AC0_TO_AC4_START1       (TQCR0_AC0_START1 | TQCR0_AC1_START1 | TQCR0_AC2_START1 | TQCR0_AC3_START1 | TQCR0_AC4_START1)
#define TQCR0_AC0_TO_AC4_START2       (TQCR0_AC0_START2 | TQCR0_AC1_START2 | TQCR0_AC2_START2 | TQCR0_AC3_START2 | TQCR0_AC4_START2)
#define TQCR0_AC0_TO_AC4_START3       (TQCR0_AC0_START3 | TQCR0_AC1_START3 | TQCR0_AC2_START3 | TQCR0_AC3_START3 | TQCR0_AC4_START3)



/* TQCR1 */
#define TQCR1_AC6_START4                 BIT(30)
#define TQCR1_AC6_START3                 BIT(29)
#define TQCR1_AC6_START2                 BIT(28)
#define TQCR1_AC6_START1                 BIT(27)
#define TQCR1_AC6_START0                 BIT(26)
#define TQCR1_AC14_START4               BIT(25)
#define TQCR1_AC14_START3               BIT(24)
#define TQCR1_AC14_START2               BIT(23)
#define TQCR1_AC14_START1               BIT(22)
#define TQCR1_AC14_START0               BIT(21)
#define TQCR1_AC13_START4               BIT(20)
#define TQCR1_AC13_START3               BIT(19)
#define TQCR1_AC13_START2               BIT(18)
#define TQCR1_AC13_START1               BIT(17)
#define TQCR1_AC13_START0               BIT(16)
#define TQCR1_AC12_START4               BIT(14)
#define TQCR1_AC12_START3               BIT(13)
#define TQCR1_AC12_START2               BIT(12)
#define TQCR1_AC12_START1               BIT(11)
#define TQCR1_AC12_START0               BIT(10)
#define TQCR1_AC11_START4               BIT(9)
#define TQCR1_AC11_START3               BIT(8)
#define TQCR1_AC11_START2               BIT(7)
#define TQCR1_AC11_START1               BIT(6)
#define TQCR1_AC11_START0               BIT(5)
#define TQCR1_AC10_START4               BIT(4)
#define TQCR1_AC10_START3               BIT(3)
#define TQCR1_AC10_START2               BIT(2)
#define TQCR1_AC10_START1               BIT(1)
#define TQCR1_AC10_START0               BIT(0)

#define TQCR1_AC10_TO_AC14_START0   (TQCR1_AC10_START0 | TQCR1_AC11_START0 | TQCR1_AC12_START0 | TQCR1_AC13_START0 | TQCR1_AC14_START0)
#define TQCR1_AC10_TO_AC14_START1   (TQCR1_AC10_START1 | TQCR1_AC11_START1 | TQCR1_AC12_START1 | TQCR1_AC13_START1 | TQCR1_AC14_START1)
#define TQCR1_AC10_TO_AC14_START2   (TQCR1_AC10_START2 | TQCR1_AC11_START2 | TQCR1_AC12_START2 | TQCR1_AC13_START2 | TQCR1_AC14_START2)
#define TQCR1_AC10_TO_AC14_START3   (TQCR1_AC10_START3 | TQCR1_AC11_START3 | TQCR1_AC12_START3 | TQCR1_AC13_START3 | TQCR1_AC14_START3)

/* TQCR2 */
#define TQCR2_AC5_FLUSH4                 BIT(30)
#define TQCR2_AC5_FLUSH3                 BIT(29)
#define TQCR2_AC5_FLUSH2                 BIT(28)
#define TQCR2_AC5_FLUSH1                 BIT(27)
#define TQCR2_AC5_FLUSH0                 BIT(26)
#define TQCR2_AC4_FLUSH4                 BIT(25)
#define TQCR2_AC4_FLUSH3                 BIT(24)
#define TQCR2_AC4_FLUSH2                 BIT(23)
#define TQCR2_AC4_FLUSH1                 BIT(22)
#define TQCR2_AC4_FLUSH0                 BIT(21)
#define TQCR2_AC3_FLUSH4                 BIT(20)
#define TQCR2_AC3_FLUSH3                 BIT(19)
#define TQCR2_AC3_FLUSH2                 BIT(18)
#define TQCR2_AC3_FLUSH1                 BIT(17)
#define TQCR2_AC3_FLUSH0                 BIT(16)
#define TQCR2_AC2_FLUSH4                 BIT(14)
#define TQCR2_AC2_FLUSH3                 BIT(13)
#define TQCR2_AC2_FLUSH2                 BIT(12)
#define TQCR2_AC2_FLUSH1                 BIT(11)
#define TQCR2_AC2_FLUSH0                 BIT(10)
#define TQCR2_AC1_FLUSH4                 BIT(9)
#define TQCR2_AC1_FLUSH3                 BIT(8)
#define TQCR2_AC1_FLUSH2                 BIT(7)
#define TQCR2_AC1_FLUSH1                 BIT(6)
#define TQCR2_AC1_FLUSH0                 BIT(5)
#define TQCR2_AC0_FLUSH4                 BIT(4)
#define TQCR2_AC0_FLUSH3                 BIT(3)
#define TQCR2_AC0_FLUSH2                 BIT(2)
#define TQCR2_AC0_FLUSH1                 BIT(1)
#define TQCR2_AC0_FLUSH0                 BIT(0)

/* TQCR3 */
#define TQCR3_AC6_FLUSH4                 BIT(30)
#define TQCR3_AC6_FLUSH3                 BIT(29)
#define TQCR3_AC6_FLUSH2                 BIT(28)
#define TQCR3_AC6_FLUSH1                 BIT(27)
#define TQCR3_AC6_FLUSH0                 BIT(26)
#define TQCR3_AC14_FLUSH4               BIT(25)
#define TQCR3_AC14_FLUSH3               BIT(24)
#define TQCR3_AC14_FLUSH2               BIT(23)
#define TQCR3_AC14_FLUSH1               BIT(22)
#define TQCR3_AC14_FLUSH0               BIT(21)
#define TQCR3_AC13_FLUSH4               BIT(20)
#define TQCR3_AC13_FLUSH3               BIT(19)
#define TQCR3_AC13_FLUSH2               BIT(18)
#define TQCR3_AC13_FLUSH1               BIT(17)
#define TQCR3_AC13_FLUSH0               BIT(16)
#define TQCR3_AC12_FLUSH4               BIT(14)
#define TQCR3_AC12_FLUSH3               BIT(13)
#define TQCR3_AC12_FLUSH2               BIT(12)
#define TQCR3_AC12_FLUSH1               BIT(11)
#define TQCR3_AC12_FLUSH0               BIT(10)
#define TQCR3_AC11_FLUSH4               BIT(9)
#define TQCR3_AC11_FLUSH3               BIT(8)
#define TQCR3_AC11_FLUSH2               BIT(7)
#define TQCR3_AC11_FLUSH1               BIT(6)
#define TQCR3_AC11_FLUSH0               BIT(5)
#define TQCR3_AC10_FLUSH4               BIT(4)
#define TQCR3_AC10_FLUSH3               BIT(3)
#define TQCR3_AC10_FLUSH2               BIT(2)
#define TQCR3_AC10_FLUSH1               BIT(1)
#define TQCR3_AC10_FLUSH0               BIT(0)

/* TQCR4 */
#define TQCR4_AC5_STOP4                   BIT(30)
#define TQCR4_AC5_STOP3                   BIT(29)
#define TQCR4_AC5_STOP2                   BIT(28)
#define TQCR4_AC5_STOP1                   BIT(27)
#define TQCR4_AC5_STOP0                   BIT(26)
#define TQCR4_AC4_STOP4                   BIT(25)
#define TQCR4_AC4_STOP3                   BIT(24)
#define TQCR4_AC4_STOP2                   BIT(23)
#define TQCR4_AC4_STOP1                   BIT(22)
#define TQCR4_AC4_STOP0                   BIT(21)
#define TQCR4_AC3_STOP4                   BIT(20)
#define TQCR4_AC3_STOP3                   BIT(19)
#define TQCR4_AC3_STOP2                   BIT(18)
#define TQCR4_AC3_STOP1                   BIT(17)
#define TQCR4_AC3_STOP0                   BIT(16)
#define TQCR4_AC2_STOP4                   BIT(14)
#define TQCR4_AC2_STOP3                   BIT(13)
#define TQCR4_AC2_STOP2                   BIT(12)
#define TQCR4_AC2_STOP1                   BIT(11)
#define TQCR4_AC2_STOP0                   BIT(10)
#define TQCR4_AC1_STOP4                   BIT(9)
#define TQCR4_AC1_STOP3                   BIT(8)
#define TQCR4_AC1_STOP2                   BIT(7)
#define TQCR4_AC1_STOP1                   BIT(6)
#define TQCR4_AC1_STOP0                   BIT(5)
#define TQCR4_AC0_STOP4                   BIT(4)
#define TQCR4_AC0_STOP3                   BIT(3)
#define TQCR4_AC0_STOP2                   BIT(2)
#define TQCR4_AC0_STOP1                   BIT(1)
#define TQCR4_AC0_STOP0                   BIT(0)

/* TQCR5 */
#define TQCR5_AC6_STOP4                   BIT(30)
#define TQCR5_AC6_STOP3                   BIT(29)
#define TQCR5_AC6_STOP2                   BIT(28)
#define TQCR5_AC6_STOP1                   BIT(27)
#define TQCR5_AC6_STOP0                   BIT(26)
#define TQCR5_AC14_STOP4                  BIT(25)
#define TQCR5_AC14_STOP3                  BIT(24)
#define TQCR5_AC14_STOP2                  BIT(23)
#define TQCR5_AC14_STOP1                  BIT(22)
#define TQCR5_AC14_STOP0                  BIT(21)
#define TQCR5_AC13_STOP4                  BIT(20)
#define TQCR5_AC13_STOP3                  BIT(19)
#define TQCR5_AC13_STOP2                  BIT(18)
#define TQCR5_AC13_STOP1                  BIT(17)
#define TQCR5_AC13_STOP0                  BIT(16)
#define TQCR5_AC12_STOP4                  BIT(14)
#define TQCR5_AC12_STOP3                  BIT(13)
#define TQCR5_AC12_STOP2                  BIT(12)
#define TQCR5_AC12_STOP1                  BIT(11)
#define TQCR5_AC12_STOP0                  BIT(10)
#define TQCR5_AC11_STOP4                  BIT(9)
#define TQCR5_AC11_STOP3                  BIT(8)
#define TQCR5_AC11_STOP2                  BIT(7)
#define TQCR5_AC11_STOP1                  BIT(6)
#define TQCR5_AC11_STOP0                  BIT(5)
#define TQCR5_AC10_STOP4                  BIT(4)
#define TQCR5_AC10_STOP3                  BIT(3)
#define TQCR5_AC10_STOP2                  BIT(2)
#define TQCR5_AC10_STOP1                  BIT(1)
#define TQCR5_AC10_STOP0                  BIT(0)

/* BCNQCR0*/
#define BCNQCR0_BCN_START0_15        BIT(30)
#define BCNQCR0_BCN_START0_14        BIT(29)
#define BCNQCR0_BCN_START0_13        BIT(28)
#define BCNQCR0_BCN_START0_12        BIT(27)
#define BCNQCR0_BCN_START0_11        BIT(26)
#define BCNQCR0_BCN_START0_10        BIT(25)
#define BCNQCR0_BCN_START0_9          BIT(24)
#define BCNQCR0_BCN_START0_8          BIT(23)
#define BCNQCR0_BCN_START0_7          BIT(22)
#define BCNQCR0_BCN_START0_6          BIT(21)
#define BCNQCR0_BCN_START0_5          BIT(20)
#define BCNQCR0_BCN_START0_4          BIT(19)
#define BCNQCR0_BCN_START0_3          BIT(18)
#define BCNQCR0_BCN_START0_2          BIT(17)
#define BCNQCR0_BCN_START0_1          BIT(16)
#define BCNQCR0_ARB_TX_START_TOGGLE     BIT(15)
#define BCNQCR0_TMAC_SLOT_IDLE_TOGGLE   BIT(14)
#define BCNQCR0_LP_TBTT_TOGGLE          BIT(13)
#define BCNQCR0_LP_PRETBTT_TOGGLE       BIT(12)
#define BCNQCR0_LP_TTTT_TOGGLE          BIT(11)
#define BCNQCR0_LP_PRETTTT_TOGGLE       BIT(10)
#define BCNQCR0_BCN_START3              BIT(3)
#define BCNQCR0_BCN_START2              BIT(2)
#define BCNQCR0_BCN_START1              BIT(1)
#define BCNQCR0_BCN_START0_0          BIT(0)

/* BCNQCR1*/
#define BCNQCR1_BCN_FLUSH0_15        BIT(30)
#define BCNQCR1_BCN_FLUSH0_14        BIT(29)
#define BCNQCR1_BCN_FLUSH0_13        BIT(28)
#define BCNQCR1_BCN_FLUSH0_12        BIT(27)
#define BCNQCR1_BCN_FLUSH0_11        BIT(26)
#define BCNQCR1_BCN_FLUSH0_10        BIT(25)
#define BCNQCR1_BCN_FLUSH0_9          BIT(24)
#define BCNQCR1_BCN_FLUSH0_8          BIT(23)
#define BCNQCR1_BCN_FLUSH0_7          BIT(22)
#define BCNQCR1_BCN_FLUSH0_6          BIT(21)
#define BCNQCR1_BCN_FLUSH0_5          BIT(20)
#define BCNQCR1_BCN_FLUSH0_4          BIT(19)
#define BCNQCR1_BCN_FLUSH0_3          BIT(18)
#define BCNQCR1_BCN_FLUSH0_2          BIT(17)
#define BCNQCR1_BCN_FLUSH0_1          BIT(16)
#define BCNQCR1_BCN_FLUSH3              BIT(3)
#define BCNQCR1_BCN_FLUSH2              BIT(2)
#define BCNQCR1_BCN_FLUSH1              BIT(1)
#define BCNQCR1_BCN_FLUSH0_0          BIT(0)

/* BMCQCR0*/
#define BMCQCR0_BMC_START0_15        BIT(30)
#define BMCQCR0_BMC_START0_14        BIT(29)
#define BMCQCR0_BMC_START0_13        BIT(28)
#define BMCQCR0_BMC_START0_12        BIT(27)
#define BMCQCR0_BMC_START0_11        BIT(26)
#define BMCQCR0_BMC_START0_10        BIT(25)
#define BMCQCR0_BMC_START0_9          BIT(24)
#define BMCQCR0_BMC_START0_8          BIT(23)
#define BMCQCR0_BMC_START0_7          BIT(22)
#define BMCQCR0_BMC_START0_6          BIT(21)
#define BMCQCR0_BMC_START0_5          BIT(20)
#define BMCQCR0_BMC_START0_4          BIT(19)
#define BMCQCR0_BMC_START0_3          BIT(18)
#define BMCQCR0_BMC_START0_2          BIT(17)
#define BMCQCR0_BMC_START0_1          BIT(16)
#define BMCQCR0_BMC_START3              BIT(3)
#define BMCQCR0_BMC_START2              BIT(2)
#define BMCQCR0_BMC_START1              BIT(1)
#define BMCQCR0_BMC_START0_0          BIT(0)

/* BMCQCR1*/
#define BMCQCR1_BMC_FLUSH0_15        BIT(30)
#define BMCQCR1_BMC_FLUSH0_14        BIT(29)
#define BMCQCR1_BMC_FLUSH0_13        BIT(28)
#define BMCQCR1_BMC_FLUSH0_12        BIT(27)
#define BMCQCR1_BMC_FLUSH0_11        BIT(26)
#define BMCQCR1_BMC_FLUSH0_10        BIT(25)
#define BMCQCR1_BMC_FLUSH0_9          BIT(24)
#define BMCQCR1_BMC_FLUSH0_8          BIT(23)
#define BMCQCR1_BMC_FLUSH0_7          BIT(22)
#define BMCQCR1_BMC_FLUSH0_6          BIT(21)
#define BMCQCR1_BMC_FLUSH0_5          BIT(20)
#define BMCQCR1_BMC_FLUSH0_4          BIT(19)
#define BMCQCR1_BMC_FLUSH0_3          BIT(18)
#define BMCQCR1_BMC_FLUSH0_2          BIT(17)
#define BMCQCR1_BMC_FLUSH0_1          BIT(16)
#define BMCQCR1_BMC_FLUSH3              BIT(3)
#define BMCQCR1_BMC_FLUSH2              BIT(2)
#define BMCQCR1_BMC_FLUSH1              BIT(1)
#define BMCQCR1_BMC_FLUSH0_0          BIT(0)

/* BMCQCR2*/
#define BMCQCR2_BMC_CNT0_4              BITS(28,31)
#define BMCQCR2_BMC_CNT0_4_OFFSET 28
#define BMCQCR2_BMC_CNT0_3              BITS(24,27)
#define BMCQCR2_BMC_CNT0_3_OFFSET 24
#define BMCQCR2_BMC_CNT0_2              BITS(20,23)
#define BMCQCR2_BMC_CNT0_2_OFFSET 20
#define BMCQCR2_BMC_CNT0_1              BITS(16,19)
#define BMCQCR2_BMC_CNT0_1_OFFSET 16
#define BMCQCR2_BMC_CNT3                BITS(12,15)
#define BMCQCR2_BMC_CNT3_OFFSET    12
#define BMCQCR2_BMC_CNT2                BITS(8,11)
#define BMCQCR2_BMC_CNT2_OFFSET    8
#define BMCQCR2_BMC_CNT1                BITS(4,7)
#define BMCQCR2_BMC_CNT1_OFFSET    4
#define BMCQCR2_BMC_CNT0_0              BITS(0,3)
#define BMCQCR2_BMC_CNT0_0_OFFSET 0

/* BMCQCR3*/
#define BMCQCR3_BMC_CNT0_12             BITS(28,31)
#define BMCQCR3_BMC_CNT0_12_OFFSET 28
#define BMCQCR3_BMC_CNT0_11             BITS(24,27)
#define BMCQCR3_BMC_CNT0_11_OFFSET 24
#define BMCQCR3_BMC_CNT0_10             BITS(20,23)
#define BMCQCR3_BMC_CNT0_10_OFFSET 20
#define BMCQCR3_BMC_CNT0_9              BITS(16,19)
#define BMCQCR3_BMC_CNT0_9_OFFSET 16
#define BMCQCR3_BMC_CNT0_8              BITS(12,15)
#define BMCQCR3_BMC_CNT0_8_OFFSET 12
#define BMCQCR3_BMC_CNT0_7              BITS(8,11)
#define BMCQCR3_BMC_CNT0_7_OFFSET 8
#define BMCQCR3_BMC_CNT0_6              BITS(4,7)
#define BMCQCR3_BMC_CNT0_6_OFFSET 4
#define BMCQCR3_BMC_CNT5_0              BITS(0,3)
#define BMCQCR3_BMC_CNT5_0_OFFSET 0

/* BMCQCR4*/
#define BMCQCR4_BMC_CNT_CLR3          BIT(19)
#define BMCQCR4_BMC_CNT_CLR2          BIT(18)
#define BMCQCR4_BMC_CNT_CLR1          BIT(17)
#define BMCQCR4_BMC_CNT_CLR0          BIT(16)
#define BMCQCR4_BMC_CNT_CLR_OFFSET 16
#define BMCQCR4_BMC_CNT0_15             BITS(8,11)
#define BMCQCR4_BMC_CNT0_15_OFFSET 8
#define BMCQCR4_BMC_CNT0_14             BITS(4,7)
#define BMCQCR4_BMC_CNT0_14_OFFSET 4
#define BMCQCR4_BMC_CNT0_13            BIT(0)
#define BMCQCR4_BMC_CNT0_13_OFFSET 0

/* TXENTRYR*/
#define TXENTRYR_TX_ENTRY_ADDR          BITS(8,15)
#define TXENTRYR_TX_ENTRY_OFFSET        8
#define TXENTRYR_TX_ENTRY_MODE      BIT(0)

/* BTIMCR0*/
#define BTIMCR0_BTIM_EN0_15             BIT(30)
#define BTIMCR0_BTIM_EN0_14             BIT(29)
#define BTIMCR0_BTIM_EN0_13             BIT(28)
#define BTIMCR0_BTIM_EN0_12             BIT(27)
#define BTIMCR0_BTIM_EN0_11             BIT(26)
#define BTIMCR0_BTIM_EN0_10             BIT(25)
#define BTIMCR0_BTIM_EN0_9               BIT(24)
#define BTIMCR0_BTIM_EN0_8               BIT(23)
#define BTIMCR0_BTIM_EN0_7               BIT(22)
#define BTIMCR0_BTIM_EN0_6               BIT(21)
#define BTIMCR0_BTIM_EN0_5               BIT(20)
#define BTIMCR0_BTIM_EN0_4               BIT(19)
#define BTIMCR0_BTIM_EN0_3               BIT(18)
#define BTIMCR0_BTIM_EN0_2               BIT(17)
#define BTIMCR0_BTIM_EN0_1               BIT(16)
#define BTIMCR0_BTIM_EN3                   BIT(3)
#define BTIMCR0_BTIM_EN2                   BIT(2)
#define BTIMCR0_BTIM_EN1                   BIT(1)
#define BTIMCR0_BTIM_EN0_0               BIT(0)

/* BTIMCR1*/
#define BTIMCR1_BTIM_CNT0_15           BIT(30)
#define BTIMCR1_BTIM_CNT0_14           BIT(29)
#define BTIMCR1_BTIM_CNT0_13           BIT(28)
#define BTIMCR1_BTIM_CNT0_12           BIT(27)
#define BTIMCR1_BTIM_CNT0_11           BIT(26)
#define BTIMCR1_BTIM_CNT0_10           BIT(25)
#define BTIMCR1_BTIM_CNT0_9             BIT(24)
#define BTIMCR1_BTIM_CNT0_8             BIT(23)
#define BTIMCR1_BTIM_CNT0_7             BIT(22)
#define BTIMCR1_BTIM_CNT0_6             BIT(21)
#define BTIMCR1_BTIM_CNT0_5             BIT(20)
#define BTIMCR1_BTIM_CNT0_4             BIT(19)
#define BTIMCR1_BTIM_CNT0_3             BIT(18)
#define BTIMCR1_BTIM_CNT0_2             BIT(17)
#define BTIMCR1_BTIM_CNT0_1             BIT(16)
#define BTIMCR1_BTIM_CNT3                 BIT(3)
#define BTIMCR1_BTIM_CNT2                 BIT(2)
#define BTIMCR1_BTIM_CNT1                 BIT(1)
#define BTIMCR1_BTIM_CNT0_0             BIT(0)

/* GTQR0 */
#define GTQR0_GUARD_TIME_AC3       BITS(24,31)
#define GTQR0_GUARD_TIME_AC3_OFFSET     24
#define GTQR0_GUARD_TIME_AC2       BITS(16,23)
#define GTQR0_GUARD_TIME_AC2_OFFSET     16
#define GTQR0_GUARD_TIME_AC1       BITS(8,15)
#define GTQR0_GUARD_TIME_AC1_OFFSET     8
#define GTQR0_GUARD_TIME_AC0       BITS(0,7)
#define GTQR0_GUARD_TIME_AC0_OFFSET     0

/* GTQR1 */
#define GTQR1_GUARD_TIME_BCN       BITS(24,31)
#define GTQR1_GUARD_TIME_BCN_OFFSET     24
#define GTQR1_GUARD_TIME_AC6       BITS(16,23)
#define GTQR1_GUARD_TIME_AC6_OFFSET     16
#define GTQR1_GUARD_TIME_AC5       BITS(8,15)
#define GTQR1_GUARD_TIME_AC5_OFFSET     8
#define GTQR1_GUARD_TIME_AC4       BITS(0,7)
#define GTQR1_GUARD_TIME_AC4_OFFSET     0

/* GTQR2 */
#define GTQR2_GUARD_TIME_AC12       BITS(24,31)
#define GTQR2_GUARD_TIME_AC12_OFFSET     24
#define GTQR2_GUARD_TIME_AC11       BITS(16,23)
#define GTQR2_GUARD_TIME_AC11_OFFSET     16
#define GTQR2_GUARD_TIME_AC10       BITS(8,15)
#define GTQR2_GUARD_TIME_AC10_OFFSET     8
#define GTQR2_GUARD_TIME_BMC       BITS(0,7)
#define GTQR2_GUARD_TIME_BMC_OFFSET    0

/* GTQR3 */
#define GTQR3_GUARD_TIME_AC14       BITS(8,15)
#define GTQR3_GUARD_TIME_AC14_OFFSET     8
#define GTQR3_GUARD_TIME_AC13       BITS(0,7)
#define GTQR3_GUARD_TIME_AC13_OFFSET     0

/* AIFSR0 */
#define AIFSR0_AIFS_BMC               BITS(28,31)
#define AIFSR0_AIFS_BMC_OFFSET 28
#define AIFSR0_AIFS_BCN               BITS(24,27)
#define AIFSR0_AIFS_BCN_OFFSET 24
#define AIFSR0_AIFS5                     BITS(20,23)
#define AIFSR0_AIFS5_OFFSET       20
#define AIFSR0_AIFS4                     BITS(16,19)
#define AIFSR0_AIFS4_OFFSET       16
#define AIFSR0_AIFS3                     BITS(12,15)
#define AIFSR0_AIFS3_OFFSET      12
#define AIFSR0_AIFS2                     BITS(8,11)
#define AIFSR0_AIFS2_OFFSET      8
#define AIFSR0_AIFS1                     BITS(4,7)
#define AIFSR0_AIFS1_OFFSET      4
#define AIFSR0_AIFS0                     BITS(0,3)
#define AIFSR0_AIFS0_OFFSET      0

#define AIFSR_AIFS_BMC              AIFSR0_AIFS_BMC
#define AIFSR_AIFS_BMC_OFFSET         AIFSR0_AIFS_BMC_OFFSET
#define AIFSR_AIFS_BCN               AIFSR0_AIFS_BCN
#define AIFSR_AIFS_BCN_OFFSET         AIFSR0_AIFS_BCN_OFFSET
#define AIFSR_AIFS4_5                  AIFSR0_AIFS5
#define AIFSR_AIFS4_5_OFFSET         AIFSR0_AIFS5_OFFSET
#define AIFSR_AIFS3                     AIFSR0_AIFS3
#define AIFSR_AIFS3_OFFSET         AIFSR0_AIFS3_OFFSET
#define AIFSR_AIFS2                     AIFSR0_AIFS2
#define AIFSR_AIFS2_OFFSET         AIFSR0_AIFS2_OFFSET
#define AIFSR_AIFS1                     AIFSR0_AIFS1
#define AIFSR_AIFS1_OFFSET         AIFSR0_AIFS1_OFFSET
#define AIFSR_AIFS0                     AIFSR0_AIFS0
#define AIFSR_AIFS0_OFFSET         AIFSR0_AIFS0_OFFSET


/* AIFSR1 */
#define AIFSR1_AIFS14                     BITS(16,19)
#define AIFSR1_AIFS14_OFFSET       16
#define AIFSR1_AIFS13                     BITS(12,15)
#define AIFSR1_AIFS13_OFFSET      12
#define AIFSR1_AIFS12                     BITS(8,11)
#define AIFSR1_AIFS12_OFFSET      8
#define AIFSR1_AIFS11                     BITS(4,7)
#define AIFSR1_AIFS11_OFFSET      4
#define AIFSR1_AIFS10                     BITS(0,3)
#define AIFSR1_AIFS10_OFFSET      0

/* ACCWXR0 */
#define ACCWXR0_AC1CWMAX                BITS(16,31)
#define ACCWXR0_AC1CWMAX_OFFSET  16
#define ACCWXR0_AC0CWMAX                BITS(0,15)
#define ACCWXR0_AC0CWMAX_OFFSET  0

/* ACCWXR1 */
#define ACCWXR1_AC3CWMAX                BITS(16,31)
#define ACCWXR1_AC3CWMAX_OFFSET  16
#define ACCWXR1_AC2CWMAX                BITS(0,15)
#define ACCWXR1_AC2CWMAX_OFFSET  0

/* ACCWXR2 */
#define ACCWXR2_AC5CWMAX                BITS(16,31)
#define ACCWXR2_AC5CWMAX_OFFSET  16
#define ACCWXR2_AC4_5CWMAX             ACCWXR2_AC5CWMAX
#define ACCWXR2_AC4_5CWMAX_OFFSET  ACCWXR2_AC5CWMAX_OFFSET

#define ACCWXR2_AC4CWMAX                BITS(0,15)
#define ACCWXR2_AC4CWMAX_OFFSET  0

/* ACCWXR3 */
#define ACCWXR3_AC11CWMAX                BITS(16,31)
#define ACCWXR3_AC11CWMAX_OFFSET  16
#define ACCWXR3_AC10CWMAX                BITS(0,15)
#define ACCWXR3_AC10CWMAX_OFFSET  0

/* ACCWXR4 */
#define ACCWXR4_AC13CWMAX                BITS(16,31)
#define ACCWXR4_AC13CWMAX_OFFSET  16
#define ACCWXR4_AC12CWMAX                BITS(0,15)
#define ACCWXR4_AC12CWMAX_OFFSET  0

/* ACCWXR5 */
#define ACCWXR5_AC14CWMAX                BITS(0,15)
#define ACCWXR5_AC14CWMAX_OFFSET  0

/* ACCWIR0 */
#define ACCWIR0_AC3CWMIN                BITS(24,31)
#define ACCWIR0_AC3CWMIN_OFFSET  24
#define ACCWIR0_AC2CWMIN                BITS(16,23)
#define ACCWIR0_AC2CWMIN_OFFSET  16
#define ACCWIR0_AC1CWMIN                BITS(8,15)
#define ACCWIR0_AC1CWMIN_OFFSET  8
#define ACCWIR0_AC0CWMIN                BITS(0,7)
#define ACCWIR0_AC0CWMIN_OFFSET  0


/* ACCWIR1 */
#define ACCWIR1_BMC_CWMIN                 BITS(24,31)
#define ACCWIR1_BMC_CWMIN_OFFSET   24
#define ACCWIR1_BCN_CWMIN                 BITS(16,23)
#define ACCWIR1_BCN_CWMIN_OFFSET 16
#define ACCWIR1_AC5_CWMIN                 BITS(8,15)
#define ACCWIR1_AC5_CWMIN_OFFSET   8
#define ACCWIR1_AC4_5CWMIN              ACCWIR1_AC5_CWMIN
#define ACCWIR1_AC4_5CWMIN_OFFSET   ACCWIR1_AC5_CWMIN_OFFSET
#define ACCWIR1_AC4_CWMIN                 BITS(0,7)
#define ACCWIR1_AC4_CWMIN_OFFSET 0

/* ACCWIR2 */
#define ACCWIR2_AC13CWMIN                BITS(24,31)
#define ACCWIR2_AC13CWMIN_OFFSET  24
#define ACCWIR2_AC12CWMIN                BITS(16,23)
#define ACCWIR2_AC12CWMIN_OFFSET  16
#define ACCWIR2_AC11CWMIN                BITS(8,15)
#define ACCWIR2_AC11CWMIN_OFFSET  8
#define ACCWIR2_AC10CWMIN                BITS(0,7)
#define ACCWIR2_AC10CWMIN_OFFSET  0

/* ACCWIR3 */
#define ACCWIR3_CW_CTL                         BIT(31)
#define ACCWIR3_PRI_CTL                         BIT(30)
#define ACCWIR3_BACKOFF_CTL                BIT(29)
#define ACCWIR3_AC14_CWMIN                 BITS(0,7)
#define ACCWIR3_AC14_CWMIN_OFFSET 0

/* ACCCWR0 */
#define ACCCWR0_AC1_CURR_CW           BITS(16,31)
#define ACCCWR0_AC0_CURR_CW           BITS(0,15)

/* ACCCWR1 */
#define ACCCWR1_AC3_CURR_CW           BITS(16,31)
#define ACCCWR1_AC2_CURR_CW           BITS(0,15)

/* ACCCWR2 */
#define ACCCWR2_AC5_CURR_CW               BITS(16,31)
#define ACCCWR2_AC4_CURR_CW               BITS(0,15)


/* ACCCWR3 */
#define ACCCWR3_AC11_CURR_CW           BITS(16,31)
#define ACCCWR3_AC10_CURR_CW           BITS(0,15)

/* ACCCWR4 */
#define ACCCWR4_AC13_CURR_CW           BITS(16,31)
#define ACCCWR4_AC12_CURR_CW           BITS(0,15)

/* ACCCWR5 */
#define ACCCWR5_AC14_CURR_CW               BITS(0,15)

/* DRNGR */
#define DRNGR_FIXED                     BIT(16)
#define DRNGR_PRN                       BITS(0,15)
/***** End of ARB for TX *****/


/* RQCR */
#define RQCR_RXVT_EN                   BIT(8)
#define RQCR_RXVR_EN                   BIT(7)
#define RQCR_RXV_REM                   BIT(5)
#define RQCR_RXV_START                 BIT(4)

#define RQCR_RX_RESET                  BIT(2)
#define RQCR_RX_REM                    BIT(1)
#define RQCR_RX_START                  BIT(0)

/* SCR */
#define SCR_BCN_Q_OPERTION_MODE_0           BITS(0,1)
#define SCR_BCN_Q_OPERTION_MODE_0_OFFSET    0
#define SCR_BCN_Q_OPERTION_MODE_1           BITS(2,3)
#define SCR_BCN_Q_OPERTION_MODE_1_OFFSET    2
#define SCR_BCN_Q_OPERTION_MODE_2           BITS(4,5)
#define SCR_BCN_Q_OPERTION_MODE_2_OFFSET    4
#define SCR_BCN_Q_OPERTION_MODE_3           BITS(6,7)
#define SCR_BCN_Q_OPERTION_MODE_3_OFFSET    6

#define SCR_TX_DISABLE               BIT(8)
#define SCR_RX_DISABLE               BIT(9)
#define SCR_PLD_EN_BF_TH               BITS(16,19)
#define SCR_PLD_EN_NAV_TH              BITS(20,23)
#define SCR_BTIM_MTK_PPTARY            BIT(24)
#define SCR_BTIM_CONTROL               BIT(29)
#define SCR_BCN_CONTROL           BIT(30)
#define SCR_BM_CONTROL                 BIT(31)


/***** MIB Counter *****/
/* MSCR */ //Posh 20131110, Re-map due to HW change
#define MSCR_MIB_COUNTER_RD_CONTROL    BIT(31)

//#define MSCR_PED_TIME1_EN               BIT(30)
//#define MSCR_PED_TIME0_EN               BIT(29)
#define MSCR_PED_TIME_EN_MASK           BITS(28,30)
#define MSCR_PED_TIME_OFFSET            28
//#define MSCR_OFDM_CCK_MDRDY_TIME1_EN    BIT(28)
//#define MSCR_OFDM_CCK_MDRDY_TIME0_EN    BIT(27)
#define MSCR_OFDM_CCK_MDRDY_TIME_EN_MASK BITS(25,27)
#define MSCR_OFDM_CCK_MDRDY_TIME_OFFSET 25
//#define MSCR_PED_TIME_EN_MSB            BIT(26)

#define MSCR_PHY_MIB_CNT1_EN            BIT(24)
#define MSCR_PHY_MIB_CNT0_EN            BIT(23)
#define MSCR_AMPDU_TX_COUNT_EN          BIT(22)

//#define MSCR_OFDM_CCK_MDRDY_TIME_EN_MSB BIT(22)

#define MSCR_TX_AGG_RANGE_COUNT_EN      BIT(21)
#define MSCR_TX_BW_COUNT_EN             BIT(20)
#define MSCR_ARB_RWP_FAIL_COUNT_EN      BIT(19)
#define MSCR_ARB_RWP_NEED_COUNT_EN      BIT(18)
#define MSCR_BCN_TX_COUNT_EN            BIT(17)

//#define MSCR_CCA_NAV_TX_TIME1_EN        BIT(15)
//#define MSCR_CCA_NAV_TX_TIME0_EN        BIT(14)
#define MSCR_CCA_NAV_TX_TIME_EN_MASK    BITS(14,16)
#define MSCR_CCA_NAV_TX_TIME_OFFSET     14
#define MSCR_CCA_TIME_EN_MASK           BITS(11,13)
//#define MSCR_CCA_TIME1_EN               BIT(13)
//#define MSCR_CCA_TIME0_EN               BIT(12)
#define MSCR_CCA_TIME_OFFSET            11
#define MSCR_AMPDU_RX_COUNT_EN          BIT(10)
#define MSCR_RX_LEN_MIS_COUNT_EN        BIT(9)
#define MSCR_PF_DROP_COUNT_EN           BIT(8)
#define MSCR_MDRDY_EVENT_EN             BIT(7)
#define MSCR_VEC_MISMATCH_EN            BIT(6)
#define MSCR_DELIMITER_FAIL_COUNT_EN    BIT(5)

//#define MSCR_CCA_TIME_EN_MSB            BIT(5)

#define MSCR_VEC_DROP_COUNT_EN          BIT(4)
#define MSCR_CH_IDLE_COUNT_EN           BIT(3)
#define MSCR_RX_MPDU_COUNT_EN           BIT(2)
#define MSCR_RX_FIFO_FULL_COUNT_EN      BIT(1)
#define MSCR_RX_FCS_ERR_COUNT_EN        BIT(0)

#define MSCR_MIB_ALL_COUNT_EN          BITS(0,30)

/* MPBSCR */
#define MPBSCR_BSS3_FRAME_RETRY3_CNT_EN     BIT(30) //Posh 20131208, Add MIB retry 3 count
#define MPBSCR_BSS3_FRAME_RETRY2_CNT_EN     BIT(29)
#define MPBSCR_BSS3_FRAME_RETRY_CNT_EN      BIT(28)
#define MPBSCR_BSS3_ACK_FAIL_CNT_EN         BIT(27)
#define MPBSCR_BSS3_BA_MISS_CNT_EN          BIT(26)
#define MPBSCR_BSS3_RTS_RETRY_CNT_EN        BIT(25)
#define MPBSCR_BSS3_RTS_TX_CNT_EN           BIT(24)

#define MPBSCR_BSS2_FRAME_RETRY3_CNT_EN     BIT(22) //Posh 20131208, Add MIB retry 3 count
#define MPBSCR_BSS2_FRAME_RETRY2_CNT_EN     BIT(21)
#define MPBSCR_BSS2_FRAME_RETRY_CNT_EN      BIT(20)
#define MPBSCR_BSS2_ACK_FAIL_CNT_EN         BIT(19)
#define MPBSCR_BSS2_BA_MISS_CNT_EN          BIT(18)
#define MPBSCR_BSS2_RTS_RETRY_CNT_EN        BIT(17)
#define MPBSCR_BSS2_RTS_TX_CNT_EN           BIT(16)

#define MPBSCR_BSS1_FRAME_RETRY3_CNT_EN     BIT(14) //Posh 20131208, Add MIB retry 3 count
#define MPBSCR_BSS1_FRAME_RETRY2_CNT_EN     BIT(13)
#define MPBSCR_BSS1_FRAME_RETRY_CNT_EN      BIT(12)
#define MPBSCR_BSS1_ACK_FAIL_CNT_EN         BIT(11)
#define MPBSCR_BSS1_BA_MISS_CNT_EN          BIT(10)
#define MPBSCR_BSS1_RTS_RETRY_CNT_EN        BIT(9)
#define MPBSCR_BSS1_RTS_TX_CNT_EN           BIT(8)

#define MPBSCR_BSS0_FRAME_RETRY3_CNT_EN     BIT(6)  //Posh 20131208, Add MIB retry 3 count
#define MPBSCR_BSS0_FRAME_RETRY2_CNT_EN     BIT(5)
#define MPBSCR_BSS0_FRAME_RETRY_CNT_EN      BIT(4)
#define MPBSCR_BSS0_ACK_FAIL_CNT_EN         BIT(3)
#define MPBSCR_BSS0_BA_MISS_CNT_EN          BIT(2)
#define MPBSCR_BSS0_RTS_RETRY_CNT_EN        BIT(1)
#define MPBSCR_BSS0_RTS_TX_CNT_EN           BIT(0)

/*
 * Posh 20131208, Add MIB retry 3 count
 */
#define MPBSCR_ALL_FRAME_RETRY3_CNT_EN     (MPBSCR_BSS0_FRAME_RETRY3_CNT_EN |\
                                            MPBSCR_BSS1_FRAME_RETRY3_CNT_EN |\
                                            MPBSCR_BSS2_FRAME_RETRY3_CNT_EN |\
                                            MPBSCR_BSS3_FRAME_RETRY3_CNT_EN)

#define MPBSCR_ALL_FRAME_RETRY2_CNT_EN     (MPBSCR_BSS0_FRAME_RETRY2_CNT_EN |\
                                            MPBSCR_BSS1_FRAME_RETRY2_CNT_EN |\
                                            MPBSCR_BSS2_FRAME_RETRY2_CNT_EN |\
                                            MPBSCR_BSS3_FRAME_RETRY2_CNT_EN)

#define MPBSCR_ALL_FRAME_RETRY_CNT_EN      (MPBSCR_BSS0_FRAME_RETRY_CNT_EN |\
                                            MPBSCR_BSS1_FRAME_RETRY_CNT_EN |\
                                            MPBSCR_BSS2_FRAME_RETRY_CNT_EN |\
                                            MPBSCR_BSS3_FRAME_RETRY_CNT_EN)

#define MPBSCR_ALL_ACK_FAIL_CNT_EN         (MPBSCR_BSS0_ACK_FAIL_CNT_EN |\
                                            MPBSCR_BSS1_ACK_FAIL_CNT_EN |\
                                            MPBSCR_BSS2_ACK_FAIL_CNT_EN |\
                                            MPBSCR_BSS3_ACK_FAIL_CNT_EN)

#define MPBSCR_ALL_BA_MISS_CNT_EN          (MPBSCR_BSS0_BA_MISS_CNT_EN |\
                                            MPBSCR_BSS1_BA_MISS_CNT_EN |\
                                            MPBSCR_BSS2_BA_MISS_CNT_EN |\
                                            MPBSCR_BSS3_BA_MISS_CNT_EN)

#define MPBSCR_ALL_RTS_RETRY_CNT_EN        (MPBSCR_BSS0_RTS_RETRY_CNT_EN |\
                                            MPBSCR_BSS1_RTS_RETRY_CNT_EN |\
                                            MPBSCR_BSS2_RTS_RETRY_CNT_EN |\
                                            MPBSCR_BSS3_RTS_RETRY_CNT_EN)

#define MPBSCR_ALL_RTS_TX_CNT_EN           (MPBSCR_BSS0_RTS_TX_CNT_EN |\
                                            MPBSCR_BSS1_RTS_TX_CNT_EN |\
                                            MPBSCR_BSS2_RTS_TX_CNT_EN |\
                                            MPBSCR_BSS3_RTS_TX_CNT_EN)

/* MSCR1 */
#define MSCR1_RX_NON_NO_DATA_CNT_EN    BIT(0)

/* MSDR0 */
#define MSDR0_BCN_TX_COUNT             BITS(0,15)

/* MSDR4 */
#define MSDR4_RX_FCS_ERROR_COUNT       BITS(16,31)
#define MSDR4_RX_FIFO_FULL_COUNT       BITS(0,15)

/* MSDR5 */
#define MSDR5_PF_DROP_COUNT            BITS(16,23)
#define MSDR5_RX_MPDU_COUNT            BITS(0,15)

/* MSDR6 */
#define MSDR6_VEC_DROP_COUNT           BITS(16,31)
#define MSDR6_CHANNEL_IDLE_COUNT       BITS(0,15)

/* MSDR7 */
#define MSDR7_VEC_MISS_COUNT           BITS(16,31)

/* MSDR8 */
#define MSDR8_DELIMITER_FAIL_COUNT     BITS(0,15)

/* MSDR9 */
#define MSDR9_CCA_NAV_TX_TIME          BITS(0,23)

/* MSDR10 */
#define MSDR10_RX_MDRDY_COUNT          BITS(0,25)

/* MSDR11 */
#define MSDR11_RX_LEN_MISMATCH      BITS(0,15)

/* MSDR16 */
#define MSDR16_P_CCA_TIME             BITS(0,23)
/* MSDR17 */
#define MSDR17_S_CCA_TIME             BITS(0,23)
/* MSDR18 */
#define MSDR18_P_ED_TIME             BITS(0,23)
/* MSDR19 */
#define MSDR19_RX_CCK_MDRDY_TIME     BITS(0,23)
/* MSDR20 */
#define MSDR20_RX_LG_MIXED_OFDM_MDRDY_TIME     BITS(0,23)
/* MSDR21 */
#define MSDR21_RX_GREEN_OFDM_MDRDY_TIME     BITS(0,23)



/* MSDR22 */
#define MSDR22_RX_AMPDU_CNT      BITS(0,15)


/* MSDR23 */
#define MSDR23_RX_TOTAL_BYTE      BITS(0,31)

/* MSDR24 */
#define MSDR24_RX_VALID_APMDU_SF      BITS(0,23)

/* MSDR25 */
#define MSDR25_RX_VALID_TOTAL_BYTE      BITS(0,31)

/* MSDR26 */
#define MSDR26_PHY_MIB_CNT0      BITS(0,31)

/* MSDR27 */
#define MSDR27_PHY_MIB_CNT1      BITS(0,31)

/* MSDR28 */
#define MSDR28_ARB_TX_RWP_FAIL_COUNT  BITS(16,31)
#define MSDR28_ARB_TX_RWP_NEED_COUNT  BITS(0,15)

/* MSDR29 */
#define MSDR29_RX_DATA_CNT_MASK        BITS(0, 15)
#define MSDR29_RX_DATA_OFFSET          0

/* MSDR30 */
#define MSDR30_RX_PARTIAL_BCN_CNT1_MASK         BITS(16, 31)
#define MSDR30_RX_PARTIAL_BCN_CNT1_OFFSET       16
#define MSDR30_RX_PARTIAL_BCN_CNT0_MASK         BITS(0, 15)
#define MSDR30_RX_PARTIAL_BCN_CNT0_OFFSET       0

/* MSDR31 */
#define MSDR31_RX_PARTIAL_BCN_CNT3_MASK         BITS(16, 31)
#define MSDR31_RX_PARTIAL_BCN_CNT3_OFFSET       16
#define MSDR31_RX_PARTIAL_BCN_CNT2_MASK         BITS(0, 15)
#define MSDR31_RX_PARTIAL_BCN_CNT2_OFFSET       0


/* MBxSDR0 */
#define MBSDR0_RTS_RETRY_CNT      BITS(16, 31)
#define MBSDR0_RTS_TX_CNT         BITS(0, 15)

/* MBxSDR1 */
#define MBSDR1_ACK_FAIL_CNT       BITS(16, 31)
#define MBSDR1_BA_MISS_CNT        BITS(0, 15)

/* MBxSDR2 */
#define MBSDR2_FRAME_RETRY2_CNT   BITS(16, 31)
#define MBSDR2_FRAME_RETRY_CNT    BITS(0, 15)

/* MBxSDR3 */
#define MBSDR3_FRAME_RETRY_CNT    BITS(0,15)  //Posh 20131208, Add MIB retry 3 count

//Per BSS TX/RX Counter
#define MIB_TX_CNT_REQ              BIT(0)
#define MIB_TX_BYTE_CNT_REQ         BIT(1)
#define MIB_RX_CNT_REQ              BIT(4)
#define MIB_RX_BYTE_CNT_REQ         BIT(5)

#define MIB_TX_MPDU_CNT             BITS(16,31)
#define MIB_TX_MPDU_CNT_OFFSET      16
#define MIB_TX_DATA_CNT             BITS(0, 15)
#define MIB_RX_MPDU_CNT             BITS(16,31)
#define MIB_RX_MPDU_CNT_OFFSET      16
#define MIB_RX_DATA_CNT             BITS(0, 15)


/***** End of MIB Counter *****/


/***** TMAC *****/
/* TCR */
#define TCR_SMOOTHING                                   BIT(31)
#define TCR_GRANT_REVERSE_WHEN_NO_PENDING_FRM           BIT(30)
#define TCR_GRANT_REVERSE_WHEN_NO_PENDING_FRM_OFFSET    30
#define TCR_RDG_RESP_En                                 BIT(29)
#define TCR_RDG_RESP_En_OFFSET                          29
#define TCR_RDGRA_MODE                                  BIT(27)
#define TCR_TXOP_BURST_STOP                             BIT(26)
#define TCR_TBTT_TX_STOP_CONTROL                        BIT(25)
#define TCR_TXOP_TBTT_CONTROL                           BIT(24)
#define TCR_RX_RIFS_MODE                                BIT(23)
#define TCR_TX_RIFS_EN                                  BIT(22)
#define TCR_SCH_DET_DIS                                 BIT(21)
#define TCR_SCH_DET_PERIOD                              BIT(20)
#define TCR_SCHANNEL_IDLE_SEL_MASK                      BITS(18,19)
#define TCR_SCHANNEL_IDLE_SEL_MASK_OFFSET               18
#define TCR_RTS_SIGNALING_TA_EN                         BIT(14)

/* ACTXOPLR0 */
#define ACTXOPLR0_AC3_LIMIT_MASK        BITS(16,31)
#define ACTXOPLR0_AC3_LIMIT_OFFSET      16
#define ACTXOPLR0_AC2_LIMIT_MASK        BITS(0,15)
#define ACTXOPLR0_AC2_LIMIT_OFFSET      0

/* ACTXOPLR1 */
#define ACTXOPLR1_AC1_LIMIT_MASK        BITS(16,31)
#define ACTXOPLR1_AC1_LIMIT_OFFSET      16
#define ACTXOPLR1_AC0_LIMIT_MASK        BITS(0,15)
#define ACTXOPLR1_AC0_LIMIT_OFFSET      0

/* ACTXOPLR2 */
#define ACTXOPLR2_AC13_LIMIT_MASK        BITS(16,31)
#define ACTXOPLR2_AC13_LIMIT_OFFSET      16
#define ACTXOPLR2_AC12_LIMIT_MASK        BITS(0,15)
#define ACTXOPLR2_AC12_LIMIT_OFFSET      0

/* ACTXOPLR3 */
#define ACTXOPLR3_AC11_LIMIT_MASK        BITS(16,31)
#define ACTXOPLR3_AC11_LIMIT_OFFSET      16
#define ACTXOPLR3_AC10_LIMIT_MASK        BITS(0,15)
#define ACTXOPLR3_AC10_LIMIT_OFFSET      0

/* FPSET */
#define FPR_64_QAM_FRAME_POWER_DBM BITS(24, 30)
#define FPR_64_QAM_FRAME_POWER_DBM_OFFSET 24
#define FPR_16_QAM_FRAME_POWER_DBM BITS(16,22)
#define FPR_16_QAM_FRAME_POWER_DBM_OFFSET 16
#define FPR_QPSK_FRAME_POWER_DBM BITS(8, 14)
#define FPR_QPSK_FRAME_POWER_DBM_OFFSET 8
#define FPR_BPSK_FRAME_POWER_DBM BITS(0,6)
#define FPR_BPSK_FRAME_POWER_DBM_OFFSET 0

/* FP0R0 */
#define FP0R0_LG_OFDM3_FRAME_POWER_DBM BITS(24, 30)
#define FP0R0_LG_OFDM2_FRAME_POWER_DBM BITS(16,22)
#define FP0R0_LG_OFDM1_FRAME_POWER_DBM BITS(8, 14)
#define FP0R0_LG_OFDM0_FRAME_POWER_DBM BITS(0,6)

/* FP0R1 */
#define FP0R1_HT20_3_FRAME_POWER_DBM BITS(24, 30)
#define FP0R1_HT20_2_FRAME_POWER_DBM BITS(16,22)
#define FP0R1_HT20_1_FRAME_POWER_DBM BITS(8, 14)
#define FP0R1_HT20_0_FRAME_POWER_DBM BITS(0,6)

/* FP0R2 */
#define FP0R2_HT40_3_FRAME_POWER_DBM BITS(24, 30)
#define FP0R2_HT40_2_FRAME_POWER_DBM BITS(16,22)
#define FP0R2_HT40_1_FRAME_POWER_DBM BITS(8, 14)
#define FP0R2_HT40_0_FRAME_POWER_DBM BITS(0,6)

/* FP0R3 */
#define FP0R3_CCK1_FRAME_POWER0_DBM     BITS(16, 22) /* For CCK 5.5M & 11M */
#define FP0R3_CCK1_FRAME_POWER0_DBM_OFFSET 16
#define FP0R3_LG_OFDM4_FRAME_POWER0_DBM BITS(8, 14)  /* For OFDM 54M */
#define FP0R3_CCK0_FRAME_POWER0_DBM     BITS(0, 6)   /* For CCK 1M & 2M */

/* FP0R4 */
#define FP0R4_HT40_5_FRAME_POWER_DBM BITS(24, 30)   /* For MCS 7 */
#define FP0R4_HT40_4_FRAME_POWER_DBM BITS(16,22)    /* For MCS 6 */
#define FP0R4_HT20_5_FRAME_POWER_DBM BITS(8, 14)     /* For MCS 7 */
#define FP0R4_HT20_4_FRAME_POWER_DBM BITS(0,6)        /* For MCS 6 */

/* FP0R5 */
#define FP0R5_VHT20_3_FRAME_POWER_DBM       BITS(24,30)   /* For MCS 5, 6 */
#define FP0R5_VHT20_2_FRAME_POWER_DBM       BITS(16,22)   /* For 16-QAM */
#define FP0R5_VHT20_1_FRAME_POWER_DBM       BITS(8,14)    /* For QPSK */
#define FP0R5_VHT20_0_FRAME_POWER_DBM       BITS(0,6)     /* For BPSK */

/* FP0R6 */
#define FP0R6_VHT20_6_FRAME_POWER_DBM        BITS(16,22)   /* For MCS 9 */
#define FP0R6_VHT20_5_FRAME_POWER_DBM        BITS(8,14)    /* For MCS 8 */
#define FP0R6_VHT20_4_FRAME_POWER_DBM        BITS(0,6)     /* For MCS 7 */

/* FP0R7 */
#define FP0R7_VHT160_FRAME_POWER_OFFET_DBM   BITS(16,22)
#define FP0R7_VHT80_FRAME_POWER_OFFET_DBM    BITS(8,14)
#define FP0R7_VHT40_FRAME_POWER_OFFET_DBM    BITS(0,6)

#if 0   //Posh 20140101, Merge code from MT6630 E2
/* FP1R0 */
#define FP1R0_LG_OFDM3_FRAME_POWER_DBM BITS(24, 30)
#define FP1R0_LG_OFDM2_FRAME_POWER_DBM BITS(16,22)
#define FP1R0_LG_OFDM1_FRAME_POWER_DBM BITS(8, 14)
#define FP1R0_LG_OFDM0_FRAME_POWER_DBM BITS(0,6)

/* FP1R1 */
#define FP1R1_HT20_3_FRAME_POWER_DBM BITS(24, 30)
#define FP1R1_HT20_2_FRAME_POWER_DBM BITS(16,22)
#define FP1R1_HT20_1_FRAME_POWER_DBM BITS(8, 14)
#define FP1R1_HT20_0_FRAME_POWER_DBM BITS(0,6)

/* FP1R2 */
#define FP1R2_HT40_3_FRAME_POWER_DBM BITS(24, 30)
#define FP1R2_HT40_2_FRAME_POWER_DBM BITS(16,22)
#define FP1R2_HT40_1_FRAME_POWER_DBM BITS(8, 14)
#define FP1R2_HT40_0_FRAME_POWER_DBM BITS(0,6)

/* FP1R3 */
#define FP1R3_CCK1_FRAME_POWER1_DBM     BITS(16, 22) /* For CCK 5.5M & 11M */
#define FP1R3_CCK1_FRAME_POWER1_DBM_OFFSET 16
#define FP1R3_LG_OFDM4_FRAME_POWER1_DBM BITS(8, 14)  /* For OFDM 54M */
#define FP1R3_CCK0_FRAME_POWER1_DBM     BITS(0, 6)   /* For CCK 1M & 2M */

/* FP1R4 */
#define FP1R4_HT40_5_FRAME_POWER_DBM BITS(24, 30)   /* For MCS 7 */
#define FP1R4_HT40_4_FRAME_POWER_DBM BITS(16,22)    /* For MCS 6 */
#define FP1R4_HT20_5_FRAME_POWER_DBM BITS(8, 14)     /* For MCS 7 */
#define FP1R4_HT20_4_FRAME_POWER_DBM BITS(0,6)        /* For MCS 6 */

/* FP2R0 */
#define FP2R0_LG_OFDM3_FRAME_POWER_DBM BITS(24, 30)
#define FP2R0_LG_OFDM2_FRAME_POWER_DBM BITS(16,22)
#define FP2R0_LG_OFDM1_FRAME_POWER_DBM BITS(8, 14)
#define FP2R0_LG_OFDM0_FRAME_POWER_DBM BITS(0,6)

/* FP2R1 */
#define FP2R1_HT20_3_FRAME_POWER_DBM BITS(24, 30)
#define FP2R1_HT20_2_FRAME_POWER_DBM BITS(16,22)
#define FP2R1_HT20_1_FRAME_POWER_DBM BITS(8, 14)
#define FP2R1_HT20_0_FRAME_POWER_DBM BITS(0,6)

/* FP2R2 */
#define FP2R2_HT40_3_FRAME_POWER_DBM BITS(24, 30)
#define FP2R2_HT40_2_FRAME_POWER_DBM BITS(16,22)
#define FP2R2_HT40_1_FRAME_POWER_DBM BITS(8, 14)
#define FP2R2_HT40_0_FRAME_POWER_DBM BITS(0,6)

/* FP2R3 */
#define FP2R3_CCK1_FRAME_POWER2_DBM     BITS(16, 22) /* For CCK 5.5M & 11M */
#define FP2R3_CCK1_FRAME_POWER2_DBM_OFFSET 16
#define FP2R3_LG_OFDM4_FRAME_POWER2_DBM BITS(8, 14)  /* For OFDM 54M */
#define FP2R3_CCK0_FRAME_POWER2_DBM     BITS(0, 6)   /* For CCK 1M & 2M */

/* FP2R4 */
#define FP2R4_HT40_5_FRAME_POWER_DBM BITS(24, 30)   /* For MCS 7 */
#define FP2R4_HT40_4_FRAME_POWER_DBM BITS(16,22)    /* For MCS 6 */
#define FP2R4_HT20_5_FRAME_POWER_DBM BITS(8, 14)     /* For MCS 7 */
#define FP2R4_HT20_4_FRAME_POWER_DBM BITS(0,6)        /* For MCS 6 */
#endif

/* FPCR */
#define FPCR_POWER2_OFFSET_EN           BIT(31)
#define FPCR_FRAME_POWER_MAX_DBM        BITS(24, 30)
#define FPCR_FRAME_POWER_MAX_DBM_OFFSET 24
#define FPCR_POWER1_OFFSET_EN           BIT(23)
#define FPCR_FRAME_POWER_MIN_DBM        BITS(16, 22)
#define FPCR_FRAME_POWER_MIN_DBM_OFFSET 16

/* FP0R8 */
#define FP0R8_LG_OFDM160_FRAME_POWER_OFFET_DBM   BITS(16,22)
#define FP0R8_LG_OFDM80_FRAME_POWER_OFFET_DBM    BITS(8,14)
#define FP0R8_LG_OFDM40_FRAME_POWER_OFFET_DBM    BITS(0,6)


/* CDTR */
#define CDTR_CCK_CCA_TIMEOUT            BITS(16,31)
#define CDTR_CCK_CCA_TIMEOUT_OFFSET          16
#define CDTR_CCK_MDRDY_TIMEOUT          BITS(0,15)
#define CDTR_CCK_MDRDY_TIMEOUT_OFFSET        0

/* ODTR */
#define ODTR_OFDM_CCA_TIMEOUT           BITS(16,31)
#define ODTR_OFDM_CCA_TIMEOUT_OFFSET         16
#define ODTR_OFDM_MDRDY_TIMEOUT         BITS(0,15)
#define ODTR_OFDM_MDRDY_TIMEOUT_OFFSET       0

/* ATCR */
#define ATCR_AGG_TIMEOUT                    BITS(16,23)
#define ATCR_AGG_TIMEOUT_OFFSET        16
#define ATCR_RIFS_TXV_TIMEOUT           BITS(8,15)
#define ATCR_RIFS_TXV_TIMEOUT_OFFSET   8
#define ATCR_TXV_TIMEOUT                    BITS(0,7)
#define ATCR_TXV_TIMEOUT_OFFSET        0

/* TRCR */
#define TRCR_CCA_SRC_SEL                   BITS(30,31)
#define TRCR_CCA_SEC_SRC_SEL               BITS(28,29)
#define TRCR_I2T_CHK_SCH_EN                BIT(26)
#define TRCR_I2T_CHK_EN                      BIT(25)
#define TRCR_I2T_CHK                            BITS(16,24)
#define TRCR_RIFS_T2T_CHK                  BITS(9,15)
#define TRCR_TR2T_CHK                         BITS(0,8)

/* RRCR */
#define RRCR_RXREQ_DELAY                   BITS(0,8)

/* ICR */
#define ICR_SLOT_TIME                        BITS(24,30)
#define ICR_SLOT_TIME_OFFSET         24
#define ICR_SIFS_TIME                        BITS(16,22)
#define ICR_SIFS_TIME_OFFSET         16
#define ICR_RIFS_TIME                        BITS(10,14)
#define ICR_RIFS_TIME_OFFSET          10
#define ICR_EIFS_TIME                        BITS(0,8)
#define ICR_EIFS_TIME_OFFSET          0

/* PPDR */
#define PPDR_CCK_20M_PDLY                        BITS(16,19)
#define PPDR_CCK_20M_PDLY_OFFSET            16
#define PPDR_OFDM_20M_PDLY                  BITS(12,15)
#define PPDR_OFDM_20M_PDLY_OFFSET           12
#define PPDR_OFDM_40M_PDLY                  BITS(8,11)
#define PPDR_OFDM_40M_PDLY_OFFSET           8
#define PPDR_OFDM_80M_PDLY                  BITS(4,7)
#define PPDR_OFDM_80M_PDLY_OFFSET           4
#define PPDR_OFDM_160M_PDLY                 BITS(0,3)
#define PPDR_OFDM_160M_PDLY_OFFSET          0

/* BRCR */


/* PRCR */
#define PRCR_PROTECTION_RATE_SEL_AUTO       BIT(15)
#define PRCR_PROTECTION_RATE                BITS(0,8)

/* B0BRR0 */
#define B0BRR0_RESP_IBFTX_EN                BIT(17)
#define B0BRR0_RESP_SPE_EN                  BIT(16)
#define B0BRR0_LG_RATE_MAP_MASK             BITS(0,11)
#define B0BRR0_LG_RATE_MAP_OFFSET           0

/* B0BRR1 */
#define B0BRR1_HT_MCS_MAP_MASK              BITS(0,8)
#define B0BRR1_HT_MCS_MAP_OFFSET            0

/* QNCR0 */
#define QNCR0_QN1_INIT_SEQ               BITS(20,31)
#define QNCR0_QN1_SEQ_RANGE_IN_POWER2    BITS(16,19)
#define QNCR0_QN0_INIT_SEQ               BITS(4,15)
#define QNCR0_QN0_SEQ_RANGE_IN_POWER2    BITS(0,3)

/* QNCR1 */
#define QNCR1_QN1_QOS_CTRL             BITS(16,31)
#define QNCR1_QN0_QOS_CTRL             BITS(0,15)

/* QNCR2 */
#define QNCR2_QN3_RESET_SEQ            BIT(3)
#define QNCR2_QN2_RESET_SEQ            BIT(2)
#define QNCR2_QN1_RESET_SEQ            BIT(1)
#define QNCR2_QN0_RESET_SEQ            BIT(0)

/* TICMR0 */
#define TICMR0_STBC_MASK                BITS(28,29)
#define TICMR0_STBC_OFFSET              28
#define TICMR0_FR_MODE_MASK             BITS(26,27)
#define TICMR0_FR_MODE_OFFSET           26
#define TICMR0_TX_MODE_MASK             BITS(23,25)
#define TICMR0_TX_MODE_OFFSET           23
#define TICMR0_TX_RATE_MASK             BITS(16,21)
#define TICMR0_TX_RATE_OFFSET           16
#define TICMR0_TX_LENGTH_MASK           BITS(0,15)
#define TICMR0_TX_LENGTH_OFFSET         0

/* TICMR1 */
#define TICMR1_TX_PWR_DBM_MASK          BITS(0,6)
#define TICMR1_TX_PWR_DBM_OFFSET        0

/* PCTSR */
//#define PCTSR_MAC2PHY_TXD_REQ         BIT(0)
#define PCTSR_MAC2PHY_TX              BIT(1)
#define PCTSR_MAC2PHY_TX_RIFS         BIT(2)
#define PCTSR_MAC2PHY_RX              BIT(3)
#define PCTSR_PHY2MAC_TXRDY        BIT(4)
#define PCTSR_PHY2MAC_TXD_ACK   BIT(5)
#define PCTSR_PHY2MAC_TX_ERR      BIT(6)
#define PCTSR_PIN_CONN_TEST_EN        BIT(31)

/* PCTDR */

/* DBGR0 */
#define DBGR0_TX_FSM_CS                     BITS(5,8)
#define DBGR0_TX_FSM_CS_OFFSET       5
#define DBGR0_TXOP_INITIAL                 BIT(31)
/***** End of TMAC *****/


/***** RMAC *****/
/* RFCR */
#define RFCR_DROP_DIFF_BSSID_BTIM    BIT(19)
#define RFCR_DROP_NOT_UC2ME          BIT(18)
#define RFCR_DROP_NOT_MY_BSSID       BIT(17)
#define RFCR_DROP_DUPLICATE          BIT(16)
#define RFCR_DROP_RTS                BIT(15)
#define RFCR_DROP_CTS                BIT(14)
#define RFCR_DROP_CTRL_RSV           BIT(13)
#define RFCR_RM_FRAME_REPORT_EN      BIT(12)
#define RFCR_DROP_DIFF_BSSID_BCN     BIT(11)
#define RFCR_DROP_DIFF_BSSID_A2      BIT(10)
#define RFCR_DROP_DIFF_BSSID_A3      BIT(9)
#define RFCR_DROP_ADDR3_OWN_MAC      BIT(8)
#define RFCR_DROP_NOT_IN_MC_TABLE    BIT(7)
#define RFCR_DROP_BC_FRAME           BIT(6)
#define RFCR_DROP_MC_FRAME           BIT(5)
#define RFCR_DROP_PROBE_REQ          BIT(4)
#define RFCR_DROP_VERSION_NOT_0      BIT(3)
#define RFCR_RX_PROMISCUOUS_MODE     BIT(2)
#define RFCR_DROP_FCS_ERROR_FRAME    BIT(1)
#define RFCR_DROP_STBC_BCN_BC_MC     BIT(0)

#define OWN_MAC_ADDR_EN              BIT(16)
#define CURRENT_BSSID_EN             BIT(16)

/* B01AR */
#define B01AR_AID1                           BITS(16,27)
#define B01AR_AID1_OFFSET                    16
#define B01AR_AID0                           BITS(0,11)

/* B23AR */
#define B23AR_AID3                           BITS(16,27)
#define B23AR_AID3_OFFSET                    16
#define B23AR_AID2                           BITS(0,11)

/* RMAC_BCNF0 */
#define BCNF_BCNF1_EN                        BIT(24)
#define BCNF_BCNF1_EN_OFFSET                 24
#define BCNF_BCNF1_NUM                       BITS(16,23)
#define BCNF_BCNF1_NUM_OFFSET                16
#define BCNF_BCNF0_EN                        BIT(8)
#define BCNF_BCNF0_EN_OFFSET                 8
#define BCNF_BCNF0_NUM                       BITS(0,7)
#define BCNF_BCNF0_NUM_OFFSET                0

/* RMAC_BCNF1 */
#define BCNF_BCNF3_EN                        BIT(24)
#define BCNF_BCNF3_EN_OFFSET                 24
#define BCNF_BCNF3_NUM                       BITS(16,23)
#define BCNF_BCNF3_NUM_OFFSET                16
#define BCNF_BCNF2_EN                        BIT(8)
#define BCNF_BCNF2_EN_OFFSET                 8
#define BCNF_BCNF2_NUM                       BITS(0,7)
#define BCNF_BCNF2_NUM_OFFSET                0

/* RMAC_DMY_31_0 */
#define DMY_31_0_RX_EARLY_TERMINATE_NAV      BIT(29)
#define DMY_31_0_RX_EARLY_TERMINATE_BSSID    BIT(28)
#define DMY_31_0_RX_CRITICAL_BTIM            BIT(27)
#define DMY_31_0_CHECK_HTC_BY_RATE           BIT(26)
#define DMY_31_0_TMR_PACKET_DROP             BIT(5)
#define DMY_31_0_TSF_UPDATE_BY_BTIM_EN       BIT(4)
#define DMY_31_0_CHECK_A3_BSSID              BIT(3)
#define DMY_31_0_TSF_UPDATE_BY_PROBE_RSP_DIS BIT(0)

/* NSUR */
#define NSUR_NAV_UPDATE                 BIT(31)
#define NSUR_EDCA_RTS_RESP              BIT(30)
#define NSUR_NAV_UPDATE_VALUE_MASK      BITS(0,25)  /* NAV updated value */

/* RMCR */
#define RMCR_RX_EARLY_TERMINATE_MODE       BITS(30,31)
#define RMCR_RX_EARLY_TERMINATE_TIME_3_0   BITS(26,29)
#define RMCR_SMPS_BY_RTS_ONLY              BIT(25)
#define RMCR_SMPS_RX_STREAM_2              BIT(24)
#define RMCR_SMPS_RX_STREAM_1              BIT(23)
#define RMCR_SMPS_RX_STREAM_0              BIT(22)
#define RMCR_SMPS_RX_STREAM_MASK           BITS(22,24)
#define RMCR_SMPS_RX_STREAM_OFFSET         22
#define RMCR_SMPS_MODE                     BITS(20,21)
#define RMCR_SMPS_MODE_OFFSET              20
#define RMCR_HT_PCO_PHASE_SKIP_EN_BSSID3   BIT(19)
#define RMCR_BCN_CONTENT_CHK_EN_BSSID3     BIT(18)
#define RMCR_HT_PCO_PHASE_SKIP_EN_BSSID2   BIT(17)
#define RMCR_BCN_CONTENT_CHK_EN_BSSID2     BIT(16)
#define RMCR_P2P_OUI_TYPE                  BITS(8,15)
#define RMCR_P2P_OUI_TYPE_OFS       8
#define RMCR_BCN_WO_TIM_IE_SEL      BIT(7)
#define RMCR_RESP_BA_IF_RX_BAR_W_NOACK     BIT(6)
#define RMCR_HT_PCO_PHASE_SKIP_EN_BSSID1   BIT(5)
#define RMCR_BCN_CONTENT_CHK_EN_BSSID1     BIT(4)
#define RMCR_HT_PCO_PHASE_SKIP_EN_BSSID0   BIT(3)
#define RMCR_BCN_CONTENT_CHK_EN_BSSID0     BIT(2)
#define RMCR_DE_AMPDU_DIS               BIT(1)
#define RMCR_RX_NOACK_CTRL              BIT(0)

/* MORE */
#define MORE_DROP_NDP                   BIT(26)
#define MORE_MUAR_MODE_SEL_MASK         BITS(30,31)
#define MORE_MUAR_MODE_SEL_OFFSET       30


/* LUNVR */
#define LUNVR_RESET                     BIT(31)
#define LUNVR_NAV_VALUE_MASK            BITS(0,25)

/* PSMPCR */
#define PSMPCR_PSMP_EN                  BIT(31)
#define PSMPCR_DTT_TID_MASK             BITS(28,30)
#define PSMPCR_DTT_TID_OFFSET           28
#define PSMPCR_DTT_SSN_MASK             BITS(16,27)
#define PSMPCR_DTT_SSN_OFFSET           16

/* CHFREQ */
#define CHFREQ_MASK                      BITS(0,7)

/* MISC */
#define TSF_HDREND_TO_PLDEND_OFST_SW        BITS(20,23)
#define TSF_HDREND_TO_PLDEND_OFST_SW_OFFSET 20
#define TSF_HDREND_TO_PLDEND_OFST_EN        BIT(19)
#define MISC_TWO_MAC_EN                     BIT(18)
#define MISC_BA_TMP_CREATE_DIS              BIT(17)
#define MISC_PSMP_DUR_SEL               BIT(16)
#define MISC_DROP_THRESHOLD             BITS(0,15)

/* MAXLEN */
#define MAXMINLEN_MINLEN_MASK               BITS(24,31)
#define MAXMINLEN_MINLEN_OFFSET         24
#define MAXMINLEN_MAXLEN_MASK               BITS(0,19)
#define MAXMINLEN_MAXLEN_OFFSET             0


/* BTENCR */
#define BTENCR_BT_ENV                        BIT(16)
#define BTENCR_BT_ENV_THRESHOLD     BITS(8,14)
#define BTENCR_BT_ENV_THRESHOLD_OFFSET  8
#define BTENCR_BT_COL_OBS_TIME       BITS(0,6)

/* ACCR */
#define ACCA_ACCA_FUNCTION_ON       BIT(31)
#define ACCA_LT_ACCA_80_BUSY         BIT(17)
#define ACCA_LT_ACCA_40_BUSY         BIT(16)
#define ACCA_ACCA_TH_80              BITS(8,10)
#define ACCA_ACCA_TH_80_OFFSET       8
#define ACCA_ACCA_TH_40              BITS(4,7)
#define ACCA_ACCA_TH_40_OFFSET       4

#define ACCA_ACCA_NUM                     BITS(0,2)

/* RFCR1 */
#define RFCR1_DROP_NOT_IN_MC_TABLE_BSS_IDX  BIT(3)
#define RFCR1_DROP_NOT_IN_MC_TABLE_BSSID3   BIT(2)
#define RFCR1_DROP_NOT_IN_MC_TABLE_BSSID2   BIT(1)
#define RFCR1_DROP_NOT_IN_MC_TABLE_BSSID1   BIT(0)

/* SCICR */
#define SCICR_SEC_ITFR                            BIT(16)
#define SCICR_BW40_TO_20_THRESHOLD    BITS(0,6)

/* PHY_DELAY */
#define PHY_DELAY_RMAC_CR_192US         BITS(24,31)
#define PHY_DELAY_RMAC_CR_96US          BITS(16,23)
#define PHY_DELAY_RMAC_CR_9600NS        BITS(0,15)

/* PMSRC0 */
#define PMSRC_PPDU_LEN_COMP_MASK            BITS(0,15)
#define PMSRC_PPDU_LEN_COMP_OFFSET          0
//#define PMSRC_CCK_RXPREAMBLE_MASK           BIT(16)
//#define PMSRC_CCK_RXPREAMBLE_OFFSET         16
#define PMSRC_RX_MODE_MASK                  BITS(16,18)
#define PMSRC_RX_MODE_OFFSET                16
#define PMSRC_RX_AGGREGATION_MASK           BIT(19)
#define PMSRC_RX_AGGREGATION_OFFSET         19
#define PMSRC_RX_RXRATE_MASK                BITS(20,26)
#define PMSRC_RX_RXRATE_OFFSET              20
#define PMSRC_RX_FRMODE_MASK                BITS(27,28)
#define PMSRC_RX_FRMODE_OFFSET              27
#define PMSRC_STBC_MASK                     BITS(30,31)
#define PMSRC_STBC_OFFSET                   30

/* PMCTL0 */
#define PMCTL_PPDU_LEN_EQ_HIT_MASK          BITS(0,1)
#define PMCTL_PPDU_LEN_EQ_HIT_OFFSET        0
//#define PMCTL_CCK_RXPREAMBLE_HIT_MASK       BITS(2,3)
//#define PMCTL_CCK_RXPREAMBLE_HIT_OFFSET     2
#define PMCTL_MODE_HIT_MASK                 BITS(4,5)
#define PMCTL_MODE_HIT_OFFSET               4
#define PMCTL_AGG_HIT_MASK                  BITS(6,7)
#define PMCTL_AGG_HIT_OFFSET                6
#define PMCTL_RXRATE_HIT_MASK               BITS(8,9)
#define PMCTL_RXRATE_HIT_OFFSET             8
#define PMCTL_FRMODE_HIT_MASK               BITS(10,11)
#define PMCTL_FRMODE_HIT_OFFSET             10
#define PMCTL_STBC_HIT_MASK                 BITS(12,13)
#define PMCTL_STBC_HIT_OFFSET               12
#define PMCTL_FCSOK_HIT_MASK                BITS(14,15)
#define PMCTL_FCSOK_HIT_OFFSET              14
#define PMCTL_UC2MAC_HIT_MASK               BITS(16,17)
#define PMCTL_UC2MAC_HIT_OFFSET             16
#define PMCTL_PPDU_LEN_LAR_HIT_MASK         BITS(18,19)
#define PMCTL_PPDU_LEN_LAR_HIT_OFFSET       18
#define PMCTL_MPDU_LEN_EQ_HIT_MASK          BITS(20,21)
#define PMCTL_MPDU_LEN_EQ_HIT_OFFSET        20
#define PMCTL_MPDU_LEN_LAR_HIT_MASK         BITS(22,23)
#define PMCTL_MPDU_LEN_LAR_HIT_OFFSET       22
#define PMCTL_AND_OR_MASK                   BIT(24)
#define PMCTL_AND_OR_OFFSET                 24

/* TFTACI0 */
#define TFTACI0_TFT_ON                      BIT(0)
#define TFTACI0_ACI_ADC_ON                  BIT(1)
#define TFTACI0_LNA_ON                      BIT(2)
#define TFTACI0_RX_FILTER_PASS              BIT(3)
#define TFTACI0_RESET_CNT                   BIT(4)
#define TFTACI0_RESET_ALL                   BIT(5)
#define TFTACI0_TIMER_DISABLE               BIT(6)

#define TFTACI0_TFT_RESULT                  BIT(8)
//#define TFTACI0_ACI_ADC_RESULT              BIT(9)
#define TFTACI0_ACI_LNA_RESULT              BIT(10)
#define TFTACI0_LNA_RESULT_MASK             BITS(11,13)
#define TFTACI0_LNA_RESULT_OFFSET           11
#define TFTACI0_MIN_RSSI_WB_OFFSET          24
#define TFTACI0_MIN_RSSI_WB_MASK            BITS(24, 31)

/* TFTACI1 */
#define TFTACI1_TFT_THR_MASK                BITS(0,1)
//#define TFTACI1_ACI_ADC_THR_MASK            BITS(2,3)
//#define TFTACI1_ACI_ADC_THR_OFFSET          2
#define TFTACI1_ACI_LNA_THR_MASK            BITS(4,5)
#define TFTACI1_ACI_LNA_THR_OFFSET          4
#define TFTACI1_LNA_THR_MASK                BITS(6,7)
#define TFTACI1_LNA_THR_OFFSET              6
#define TFTACI1_ALL_THR_MASK                BITS(0,7)
#define TFTACI1_TOTAL_PKT_NUM_MASK          BITS(8,15)
#define TFTACI1_TOTAL_PKT_NUM_OFFSET        8
#define TFTACI1_TIMEOUT_SEL_MASK            BITS(16,17)
#define TFTACI1_TIMEOUT_SEL_OFFSET          16
#define TFTACI1_TOTAL_PKT_TIMEOUT_SEL_MASK  BITS(8,17)
#define TFTACI1_WB_IB_THR_MASK              BITS(24,28)
#define TFTACI1_WB_IB_THR_OFFSET            24


/* TFTACI2 */
#define TFTACI2_CNT_FILTER_PKT_MASK         BITS(0,7)
#define TFTACI2_TFT_CNT_MASK                BITS(8,15)
#define TFTACI2_TFT_CNT_OFFSET              8
//#define TFTACI2_ACI_ADC_CNT_MASK            BITS(16,23)
//#define TFTACI2_ACI_ADC_CNT_OFFSET          16
#define TFTACI2_ACI_LNA_CNT_MASK            BITS(24,31)
#define TFTACI2_ACI_LNA_CNT_OFFSET          24

/* TFTACI3 */
#define TFTACI3_LNA_UL_CNT_MASK             BITS(0,7)
#define TFTACI3_LNA_L_CNT_MASK                  BITS(8,15)
#define TFTACI3_LNA_L_CNT_OFFSET                8
#define TFTACI3_LNA_M_CNT_MASK              BITS(16,23)
#define TFTACI3_LNA_M_CNT_OFFSET            16
#define TFTACI3_LNA_H_CNT_MASK              BITS(24,31)
#define TFTACI3_LNA_H_CNT_OFFSET            24

/* TFTACI4 */
#define TFTACI4_LNA_ULL_CNT_MASK            BITS(0,7)


/* RMAC_MULTFX1     (WIFI_RMAC_BASE + 0x00E4) */
#define DAWN_IGNORE_NAV_TIME_UNIT           BITS(8,15)
#define DAWN_IGNORE_NAV_TIME_UNIT_OFFSET    8
#define DAWN_IGNORE_NAV_EN                  BIT(1)
#define DAWN_VIOLATE_NAV_EN                 BIT(0)


/* TXOPPS */
#define TXOPPS_PARTIAL_AID_MASK         BITS(0,8)
#define TXOPPS_RX_TIMEOUT               BIT(16)
#define TXOPPS_RX_LSIG_EN               BIT(17)
#define TXOPPS_RX_RX_EN                 BIT(18)

/* FS0CR */
#define FS0CR_FULL_SC_TA_SEL_MASK       BITS(17, 18)
#define FS0CR_FULL_SC_TA_SEL_OFFSET     17
#define FS0CR_FULL_SC_EN                BIT(16)
#define FS0CR_FULL_SC_TID_MASK          BITS(12,15)
#define FS0CR_FULL_SC_TID_OFFSET        12
#define FS0CR_FULL_SC_SSN_MASK          BITS(0,11)
#define FS0CR_FULL_SC_SSN_OFFSET        0


/* RMAC_MISE2 */
#define MISC2_MORE_BIT_SEL              BIT(28,31)
#define MISC2_MNG_PKT_SEL               BIT(24,27)
#define MISC2_DATA_PKT_SEL              BIT(20,23)
#define MISC2_PARTIAL_BCN_RCPI_MODE     BIT(19)
#define MISC2_FULL_SCB_OPT1             BIT(12)
#define MISC2_FULL_SCB_OPT0             BIT(11)

/* NAN0 */
#define NAN0_TSF_EN                     BIT(31)
#define NAN0_FILTER_EN                  BIT(30)
#define NAN0_AMBTT_33_BIT_SEL           BIT(29)
#define NAN0_AMBTT_ORDER_SEL            BIT(28)
#define NAN0_BSSID_SEL                  BITS(24,27)
#define NAN0_BSSID_SEL_OFFSET           24

#define NAN0_TSF_DROP_BCN_SEL           BIT(23)
#define NAN0_BCN_RX_ALL                 BIT(22)
#define NAN0_TSF_COMP_SW                BIT(21)
#define NAN0_ADDR2_47_32                BITS(0,15)

/* NAN2 */
#define NAN2_AMBTT_THR_MASK             BITS(0,23)
#define NAN2_HOP_CNT_THR_MASK           BITS(24,31)
#define NAN2_HOP_CNT_THR_OFFSET         24

/* PCSR */
#define PCSR_PHY2MAC_SEC40_CS           BIT(26)
#define PCSR_PHY2MAC_CCA_SEC40          BIT(25)
#define PCSR_PHY2MAC_SEC_CS             BIT(24)
#define PCSR_PHY2MAC_CCA_PLD            BIT(23)
#define PCSR_MAC2PHY_NAV_IND            BIT(22)
#define PCSR_PHY2MAC_RXD_TYPE           BITS(16,18)
//#define PCSR_PHY2MAC_FULL_ALARM         BIT(15)
#define PCSR_PHY2MAC_RX_ERR             BIT(14)
#define PCSR_PHY2MAC_RX_RX_AIR_END      BIT(13)
#define PCSR_PHY2MAC_CCA_CS             BIT(12)
#define PCSR_PHY2MAC_CCA_SEC            BIT(11)
#define PCSR_PHY2MAC_CCA_PRIM           BIT(10)
//#define PCSR_MAC2PHY_RXD_ACK            BIT(9)
#define PCSR_PHY2MAC_RXD_REQ            BIT(8)
#define PCSR_PHY2MAC_MDRDY              BIT(7)

/* DBG0_FSM */
#define DBG0_FSM_TMAC_HDRTIME           BITS(16,25)
#define DBG0_FSM_TMAC_HDRTIME_OFFSET    16

/* RMAC_MISC2 */
#define MORE_BIT_SEL_OFFSET             28
#define MNG_PKT_OFFSET                  24
#define DATA_PKT_OFFSET                 20

/* DBG6 */
#define DBG6_BT_OBS_CNT                 BITS(0,6)
#define DBG6_BT_OBS_CNT_OFFSET          0
#define DBG6_BT_COL_CNT                 BITS(7,13)
#define DBG6_BT_COL_CNT_OFFSET          7
#define DBG6_SCI_ITFR_CNT               BITS(14,20)
#define DBG6_SCI_ITFR_CNT_OFFSET        14
#define DBG6_PRI_ITFR_CNT_MASK          BITS(24,31)
#define DBG6_PRI_ITFR_CNT_OFFSET        24


/* DBG8 */
#define DBG8_ACCA_OBS_CNT               BITS(0,13)
#define DBG8_ACCA_CNT                   BITS(14,27)

/* TMRCS */
#define TMRCS_SEC_40_CCA                BIT(12)
#define TMRCS_SEC_40_CS                 BIT(11)
#define TMRCS_SEC_CS                    BIT(10)
#define TMRCS_PLD_ON                    BIT(9)
#define TMRCS_CCA_PLD                   BIT(8)
#define TMRCS_FULL_ALARM                BIT(7)
#define TMRCS_RX_AIR_END                BIT(6)
#define TMRCS_CCA_BT                    BIT(5)
#define TMRCS_CCA_CS                    TMRCS_CCA_BT
#define TMRCS_CCA_SEC                   BIT(4)
#define TMRCS_CCA_PRIM                  BIT(3)
#define TMRCS_RX_ERR                    BIT(2)
#define TMRCS_MDRDY                     BIT(1)
#define TMRCS_MAC_RX_RDY                BIT(0)

/* TMRDT */
#define TMRDT_RX_DATA_TYPE              BITS(0,2)

/* RR0 */
#define RR0_MOVING_AVERAGE0_EN                  BIT(31)
#define RR0_RCPI_SOURCE0_MASK                   BITS(28,30)
#define RR0_RCPI_SOURCE0_OFFSET                 28
#define RR0_RCPI_CCK_HIGH_THRESHOLD0_MASK       BITS(16,25)
#define RR0_RCPI_CCK_HIGH_THRESHOLD0_OFFSET     16
#define RR0_RCPI_MOVING_AVERAGE_PARM0_MASK      BITS(14,15)
#define RR0_RCPI_MOVING_AVERAGE_PARM0_OFFSET    14
#define RR0_RCPI_MODE_MASK                      BITS(11,13)
#define RR0_RCPI_MODE_OFFSET                    11
#define RR0_RCPI_CCK_LOW_THRESHOLD0_MASK        BITS(0,9)
#define RR0_RCPI_CCK_LOW_THRESHOLD0_OFFSET      0


/* RR2 */
#define RR2_MOVING_AVERAGE1_EN                  BIT(31)
#define RR2_RCPI_SOURCE1_MASK                   BITS(28,30)
#define RR2_RCPI_SOURCE1_OFFSET                 28
#define RR2_RCPI_CCK_HIGH_THRESHOLD1_MASK       BITS(16,25)
#define RR2_RCPI_CCK_HIGH_THRESHOLD1_OFFSET     16
#define RR2_RCPI_MOVING_AVERAGE_PARM1_MASK      BITS(14,15)
#define RR2_RCPI_MOVING_AVERAGE_PARM1_OFFSET    14
#define RR2_RCPI_CCK_LOW_THRESHOLD1_MASK        BITS(0,9)
#define RR2_RCPI_CCK_LOW_THRESHOLD1_OFFSET      0

/* RR3 */


#define RR3_RCPI_OFDM_HIGH_THRESHOLD0_MASK       BITS(16,25)
#define RR3_RCPI_OFDM_HIGH_THRESHOLD0_OFFSET     16
#define RR3_RCPI_OFDM_LOW_THRESHOLD0_MASK        BITS(0,9)
#define RR3_RCPI_OFDM_LOW_THRESHOLD0_OFFSET      0

/* RR4 */
#define RR4_MOVING_AVERAGE2_EN                  BIT(31)
#define RR4_RCPI_SOURCE2_MASK                   BITS(28,30)
#define RR4_RCPI_SOURCE2_OFFSET                 28
#define RR4_RCPI_CCK_HIGH_THRESHOLD2_MASK       BITS(16,25)
#define RR4_RCPI_CCK_HIGH_THRESHOLD2_OFFSET     16
#define RR4_RCPI_MOVING_AVERAGE_PARM2_MASK      BITS(14,15)
#define RR4_RCPI_MOVING_AVERAGE_PARM2_OFFSET    14
#define RR4_RCPI_CCK_LOW_THRESHOLD2_MASK        BITS(0,9)
#define RR4_RCPI_CCK_LOW_THRESHOLD2_OFFSET      0

/* RR6 */
#define RR6_MOVING_AVERAGE3_EN                  BIT(31)
#define RR6_RCPI_SOURCE3_MASK                   BITS(28,30)
#define RR6_RCPI_SOURCE3_OFFSET                 28
#define RR6_RCPI_CCK_HIGH_THRESHOLD3_MASK       BITS(16,25)
#define RR6_RCPI_CCK_HIGH_THRESHOLD3_OFFSET     16
#define RR6_RCPI_MOVING_AVERAGE_PARM3_MASK      BITS(14,15)
#define RR6_RCPI_MOVING_AVERAGE_PARM3_OFFSET    14
#define RR6_RCPI_CCK_LOW_THRESHOLD3_MASK        BITS(0,9)
#define RR6_RCPI_CCK_LOW_THRESHOLD3_OFFSET      0

/* RR7 */
#define RR7_RCPI_OFDM_HIGH_THRESHOLD2_MASK       BITS(16,25)
#define RR7_RCPI_OFDM_HIGH_THRESHOLD2_OFFSET     16
#define RR7_RCPI_OFDM_LOW_THRESHOLD2_MASK        BITS(0,9)
#define RR7_RCPI_OFDM_LOW_THRESHOLD2_OFFSET      0

/* RR9 */
#define RR9_RCPI_OFDM_HIGH_THRESHOLD1_MASK       BITS(16,25)
#define RR9_RCPI_OFDM_HIGH_THRESHOLD1_OFFSET     16
#define RR9_RCPI_OFDM_LOW_THRESHOLD1_MASK        BITS(0,9)
#define RR9_RCPI_OFDM_LOW_THRESHOLD1_OFFSET      0

/* RR11*/
#define RR11_RCPI_OFDM_HIGH_THRESHOLD3_MASK       BITS(16,25)
#define RR11_RCPI_OFDM_HIGH_THRESHOLD3_OFFSET     16
#define RR11_RCPI_OFDM_LOW_THRESHOLD3_MASK        BITS(0,9)
#define RR11_RCPI_OFDM_LOW_THRESHOLD3_OFFSET      0


/* RCPI */
#define RCPI_CCK_VALUE_MASK         BITS(0,9)
#define RCPI_CCK_VALUE_OFFSET       0
#define RCPI_OFDM_VALUE_MASK        BITS(0,9)
#define RCPI_OFDM_VALUE_OFFSET      0
#define RCPI_CCK_YN_MASK            BITS(16, 30)
#define RCPI_CCK_YN_OFFSET          16
#define RCPI_OFDM_YN_MASK           BITS(16, 30)
#define RCPI_OFDM_YN_OFFSET         16
#define RCPI_CCK_PRE_VALUE_MASK     BITS(21, 30)
#define RCPI_CCK_PRE_VALUE_OFFSET   21
#define RCPI_OFDM_PRE_VALUE_MASK    BITS(21, 30)
#define RCPI_OFDM_PRE_VALUE_OFFSET  21


/* LQR0 /LQR2 / LQR6 / LQR8*/
#define LQ_MOVING_AVERAGE_EN                  BIT(31)
#define LQ_LQ_SOURCE_MASK                     BITS(28,30)
#define LQ_LQ_SOURCE_OFFSET                   28
#define LQ_OFDM_RM_LOW_THRESHOLD_MASK         BITS(16,27)
#define LQ_OFDM_RM_LOW_THRESHOLD_OFFSET       16
#define LQ_LQ_MOVING_AVERAGE_PARM_MASK        BITS(14,15)
#define LQ_LQ_MOVING_AVERAGE_PARM_OFFSET      14
#define LQ_CRC_NSTS_EN                        BIT(12)
#define LQ_CCK_RM_LOW_THRESHOLD_MASK          BITS(0,11)

/* LQR1 / LQR3 / LQR7 / LQR9*/
#define LQ_RM_DETECTED_LOW_THRESHOLD_MASK     BITS(8,15)
#define LQ_RM_DETECTED_LOW_THRESHOLD_OFFSET   8
#define LQ_CRC_PASS_THRESHOLD_MASK            BITS(0,7)

/* DBG1_LQ_AVG/DBG2_LQ_AVG/DBG3_LQ_AVG/DBG4_LQ_AVG */
#define DBG_LQ_BCN_LOST                     BIT(31)
#define DBG_LQ_AVG_CCK_MASK                BITS(12,23)
#define DBG_LQ_AVG_CCK_OFFSET              12
#define DBG_LQ_AVG_OFDM_MASK               BITS(0,11)

/* LQR2 */
//#define LQR2_MOVING_AVERAGE_EN                  BIT(31)
//#define LQR2_LQ_SOURCE_MASK                     BITS(28,30)
//#define LQR2_LQ_SOURCE_OFFSET                   28
//#define LQR2_OFDM_RM_LOW_THRESHOLD_MASK         BITS(16,27)
//#define LQR2_OFDM_RM_LOW_THRESHOLD_OFFSET       16
//#define LQR2_LQ_MOVING_AVERAGE_PARM_MASK        BITS(14,15)
//#define LQR2_LQ_MOVING_AVERAGE_PARM_OFFSET      14
//#define LQR2_CCK_RM_LOW_THRESHOLD_MASK          BITS(0,11)

/* LQR3 */
//#define LQR3_RM_DETECTED_LOW_THRESHOLD_MASK     BITS(8,15)
//#define LQR3_RM_DETECTED_LOW_THRESHOLD_OFFSET   8
//#define LQR3_CRC_PASS_THRESHOLD_MASK            BITS(0,7)

/* DBG5 */
//#define DBG_LQ_LESS_CNT_BSSID1_MASK         BITS(24,31)
//#define DBG_LQ_LESS_CNT_BSSID1_OFFSET       24
//#define DBG_LQ_LESS_CNT_BSSID0_MASK         BITS(16,23)
//#define DBG_LQ_LESS_CNT_BSSID0_OFFSET       16
//#define DBG_BCN_PASS_CNT_BSSID1_MASK        BITS(8,15)
//#define DBG_BCN_PASS_CNT_BSSID1_OFFSET      8
//#define DBG_BCN_PASS_CNT_BSSID0_MASK        BITS(0,7)
//#define DBG_BCN_PASS_CNT_BSSID0_OFFSET      0

/* LQBUF_CFG */
#define LQBUF_LQ_BUF_SIZE_BSSID3_MASK       BITS(22,23)
#define LQBUF_LQ_BUF_SIZE_BSSID3_OFFSET     22
#define LQBUF_LQ_BUF_SIZE_BSSID2_MASK       BITS(20,21)
#define LQBUF_LQ_BUF_SIZE_BSSID2_OFFSET     20
#define LQBUF_CRC_BUF_SIZE_BSSID3_MASK      BITS(18,19)
#define LQBUF_CRC_BUF_SIZE_BSSID3_OFFSET    18
#define LQBUF_CRC_BUF_SIZE_BSSID2_MASK      BITS(16,17)
#define LQBUF_CRC_BUF_SIZE_BSSID2_OFFSET    16
#define LQBUF_LQ_BUF_SIZE_BSSID1_MASK       BITS(6,7)
#define LQBUF_LQ_BUF_SIZE_BSSID1_OFFSET     6
#define LQBUF_LQ_BUF_SIZE_BSSID0_MASK       BITS(4,5)
#define LQBUF_LQ_BUF_SIZE_BSSID0_OFFSET     4
#define LQBUF_CRC_BUF_SIZE_BSSID1_MASK      BITS(2,3)
#define LQBUF_CRC_BUF_SIZE_BSSID1_OFFSET    2
#define LQBUF_CRC_BUF_SIZE_BSSID0_MASK      BITS(0,1)
#define LQBUF_CRC_BUF_SIZE_BSSID0_OFFSET    0

/* LQR4 / LQR5 / LQR10 / LQR11*/
#define LQ_RM_DET_HIGH_THRESHOLD_MASK      BITS(24,31)
#define LQ_RM_DET_HIGH_THRESHOLD_OFFSET    24
#define LQ_CCK_RM_HIGH_THRESHOLD_MASK      BITS(12,23)
#define LQ_CCK_RM_HIGH_THRESHOLD_OFFSET    12
#define LQ_OFDM_RM_HIGH_THRESHOLD_MASK     BITS(0,11)
#define LQ_OFDM_RM_HIGH_THRESHOLD_OFFSET   0

/* LQBCR */
#define LQ_RX_BCN_CNT3_MASK                 BITS(24,31)
#define LQ_RX_BCN_CNT3_OFFSET               24
#define LQ_RX_BCN_CNT2_MASK                 BITS(16,23)
#define LQ_RX_BCN_CNT2_OFFSET               16
#define LQ_RX_BCN_CNT1_MASK                 BITS(8,15)
#define LQ_RX_BCN_CNT1_OFFSET               8
#define LQ_RX_BCN_CNT0_MASK                 BITS(0,7)
#define LQ_RX_BCN_CNT0_OFFSET               0

/* LQBPCR*/
#define LQ_RX_BCN_PASS_CNT3_MASK            BITS(24,31)
#define LQ_RX_BCN_PASS_CNT3_OFFSET          24
#define LQ_RX_BCN_PASS_CNT2_MASK            BITS(16,23)
#define LQ_RX_BCN_PASS_CNT2_OFFSET          16
#define LQ_RX_BCN_PASS_CNT1_MASK            BITS(8,15)
#define LQ_RX_BCN_PASS_CNT1_OFFSET          8
#define LQ_RX_BCN_PASS_CNT0_MASK            BITS(0,7)
#define LQ_RX_BCN_PASS_CNT0_OFFSET          0

/* LQCCK0R / LQCCK1R / LQCCK2R / LQCCK3R */
#define LQ_CCK_PRE_VALUE_MASK               BITS(21, 30)
#define LQ_CCK_PRE_VALUE_OFFSET             21
#define LQ_CCK_YN_MASK                      BITS(16, 30)
#define LQ_CCK_YN_OFFSET                    16
#define LQ_CCK_VALUE_MASK                   BITS(0,9)
#define LQ_CCK_VALUE_OFFSET                 0

/* LQOFDM0R / LQOFDM1R / LQOFDM2R / LQOFDM3 */
#define LQ_OFDM_PRE_VALUE_MASK              BITS(21, 27)
#define LQ_OFDM_PRE_VALUE_OFFSET            21
#define LQ_OFDM_YN_MASK                     BITS(16, 27)
#define LQ_OFDM_YN_OFFSET                   16
#define LQ_OFDM_VALUE_MASK                  BITS(0,6)
#define LQ_OFDM_VALUE_OFFSET                0

/* LQRXCNT0 / LQRXCNT1 / LQRXCNT2 / LQRXCNT3 */
#define LQ_RX_FAIL_HI_CNT_MASK              BITS(24,31)
#define LQ_RX_FAIL_HI_CNT_OFFSET            24
#define LQ_RX_HI_CNT_MASK                   BITS(16,23)
#define LQ_RX_HI_CNT_OFFSET                 16
#define LQ_RX_FAIL_CNT_MASK                 BITS(8,15)
#define LQ_RX_FAIL_CNT_OFFSET               8
#define LQ_RX_CNT_MASK                      BITS(0,7)
#define LQ_RX_CNT_OFFSET                    0



/* LQR5 */
//#define LQ_RM_DET_HIGH_THRESHOLD1_MASK      BITS(24,31)
//#define LQ_RM_DET_HIGH_THRESHOLD1_OFFSET    24
//#define LQ_CCK_RM_HIGH_THRESHOLD1_MASK      BITS(12,23)
//#define LQ_CCK_RM_HIGH_THRESHOLD1_OFFSET    12
//#define LQ_OFDM_RM_HIGH_THRESHOLD1_MASK     BITS(0,11)
//#define LQ_OFDM_RM_HIGH_THRESHOLD1_OFFSET   0

/* LQR6 */
//#define LQR6_MOVING_AVERAGE_EN              BIT(31)
//#define LQR6_LQ_SOURCE_MASK                 BITS(28,30)
//#define LQR6_LQ_SOURCE_OFFSET               28
//#define LQR6_OFDM_RM_THRESHOLD_MASK         BITS(16,27)
//#define LQR6_OFDM_RM_THRESHOLD_OFFSET       16
//#define LQR6_LQ_MOVING_AVERAGE_PARM_MASK    BITS(14,15)
//#define LQR6_LQ_MOVING_AVERAGE_PARM_OFFSET  14
//#define LQR6_CCK_RM_THRESHOLD_MASK          BITS(0,11)

/* LQR7 */
//#define LQR7_RM_DETECTED_THRESHOLD_MASK     BITS(8,15)
//#define LQR7_RM_DETECTED_THRESHOLD_OFFSET   8
//#define LQR7_CRC_PASS_THRESHOLD_MASK        BITS(0,7)

/* LQR8 */
//#define LQR8_MOVING_AVERAGE_EN              BIT(31)
//#define LQR8_LQ_SOURCE_MASK                 BITS(28,30)
//#define LQR8_LQ_SOURCE_OFFSET               28
//#define LQR8_OFDM_RM_THRESHOLD_MASK         BITS(16,27)
//#define LQR8_OFDM_RM_THRESHOLD_OFFSET       16
//#define LQR8_LQ_MOVING_AVERAGE_PARM_MASK    BITS(14,15)
//#define LQR8_LQ_MOVING_AVERAGE_PARM_OFFSET  14
//#define LQR8_CCK_RM_THRESHOLD_MASK          BITS(0,11)

/* LQR9 */
//#define LQR9_RM_DETECTED_THRESHOLD_MASK     BITS(8,15)
//#define LQR9_RM_DETECTED_THRESHOLD_OFFSET   8
//#define LQR9_CRC_PASS_THRESHOLD_MASK        BITS(0,7)

/* LQR10 */
//#define LQ_RM_DET_HIGH_THRESHOLD2_MASK      BITS(24,31)
//#define LQ_RM_DET_HIGH_THRESHOLD2_OFFSET    24
//#define LQ_CCK_RM_HIGH_THRESHOLD2_MASK      BITS(12,23)
//#define LQ_CCK_RM_HIGH_THRESHOLD2_OFFSET    12
//#define LQ_OFDM_RM_HIGH_THRESHOLD2_MASK     BITS(0,11)
///#define LQ_OFDM_RM_HIGH_THRESHOLD2_OFFSET   0

/* LQR11 */
//#define LQ_RM_DET_HIGH_THRESHOLD3_MASK      BITS(24,31)
//#define LQ_RM_DET_HIGH_THRESHOLD3_OFFSET    24
//#define LQ_CCK_RM_HIGH_THRESHOLD3_MASK      BITS(12,23)
//#define LQ_CCK_RM_HIGH_THRESHOLD3_OFFSET    12
//#define LQ_OFDM_RM_HIGH_THRESHOLD3_MASK     BITS(0,11)
//#define LQ_OFDM_RM_HIGH_THRESHOLD3_OFFSET   0


//TBD
/* BSCR1 */
#define BSCR1_START_RST_BA_SB           BIT(22)
#define BSCR1_RST_BA_SEL_MASK           BITS(20,21)
#define BSCR1_RST_BA_SEL_OFFSET         20
#define BSCR1_RST_BA_TID_MASK           BITS(16,19)
#define BSCR1_RST_BA_TID_OFFSET         16
#define BSCR1_BA_MAC_ADDR               BITS(0,15)

/* B0AR */
#define B0AR_AID0                           BITS(16,27)
#define B0AR_AID0_OFFSET                    16
#define B0AR_AID1                           BITS(0,15)
#define B0AR_AID1_OFFSET                    0

/* B01AR */
#define B01AR_AID1                          BITS(16,27)
#define B01AR_AID1_OFFSET                   16
#define B01AR_AID0                          BITS(0,11)
#define B01AR_AID0_OFFSET                   0

/* B23AR */
#define B23AR_AID3                          BITS(16,27)
#define B23AR_AID3_OFFSET                   16
#define B23AR_AID2                          BITS(0,11)
#define B23AR_AID2_OFFSET                    0

/* DBG4 */
#define DBG4_OFDM_PHY_DELAY_A           BITS(16,31)
#define DBG4_CCK_PHY_DELAY_B            BITS(0,15)


/***** PF *****/
/* PIR */
#define PFCR_MC_MATCHING_OPERATION          BIT(5)
#define PFCR_BC_MATCHING_OPERATION          BIT(4)
#define PFCR_MC_PATTERN_SRCH_EN             BIT(1)
#define PFCR_BC_PATTERN_SRCH_EN             BIT(0)

/* PCRN */
#define PCRN_PATTERN_BC_CHECK               BIT(27)
#define PCRN_PATTERN_MC_CHECK               BIT(26)
#define PCRN_PATTERN_BYTE_MASK              BITS(16,23)
#define PCRN_PATTERN_BYTE_MASK_OFFSET       16
#define PCRN_AND_OR_OPERAND                 BIT(15)
#define PCRN_NOT_OPERAND                    BIT(14)
#define PCRN_PATTERN_HEADER_MODE            BITS(12,13)
#define PCRN_PATTERN_HEADER_MODE_OFFSET     12
#define PCRN_PATTERN_OFFSET                 BITS(0,7)
#define PCRN_PATTERN_OFFSET_OFFSET          0


/***** LP *****/

/* TT0SBOR1 ~  TT0SBOR15  */
#define TT0SBOR1_TTTT0_SBOFFSET1        BITS(17,0)
#define TT0SBOR2_TTTT0_SBOFFSET2        BITS(17,0)
#define TT0SBOR3_TTTT0_SBOFFSET3        BITS(17,0)
#define TT0SBOR4_TTTT0_SBOFFSET4        BITS(17,0)
#define TT0SBOR5_TTTT0_SBOFFSET5        BITS(17,0)
#define TT0SBOR6_TTTT0_SBOFFSET6        BITS(17,0)
#define TT0SBOR7_TTTT0_SBOFFSET7        BITS(17,0)
#define TT0SBOR8_TTTT0_SBOFFSET8        BITS(17,0)
#define TT0SBOR9_TTTT0_SBOFFSET9        BITS(17,0)
#define TT0SBOR10_TTTT0_SBOFFSET10      BITS(17,0)
#define TT0SBOR11_TTTT0_SBOFFSET11      BITS(17,0)
#define TT0SBOR12_TTTT0_SBOFFSET12      BITS(17,0)
#define TT0SBOR13_TTTT0_SBOFFSET13      BITS(17,0)
#define TT0SBOR14_TTTT0_SBOFFSET14      BITS(17,0)
#define TT0SBOR15_TTTT0_SBOFFSET15      BITS(17,0)

/* T0CR */
#define T0CR_TSF_DRIFT_WINDOW_UPDATE        BIT(24)
#define T0CR_TSF_DRIFT_WINDOW               BITS(16,23)
#define T0CR_TSF_DRIFT_WINDOW_OFFSET        16

#define T0CR_TSF_TIMER_HW_MODE_FULL_ADHOC   (3 << 2)
#define T0CR_TSF_TIMER_HW_MODE_RX_ONLY      (2 << 2)
#define T0CR_TSF_TIMER_HW_MODE_TICK_ONLY    (1 << 2)
#define T0CR_TSF_TIMER_HW_MODE_FULL         (0 << 2)
#define T0CR_TSF_TIMER_HW_MODE              BITS(2,3)

#define T0CR_TSF_TIMER_SW_MODE_TSFTIMERVALUEREAD        3
#define T0CR_TSF_TIMER_SW_MODE_TSFTIMERVALUEADJUST      2
#define T0CR_TSF_TIMER_SW_MODE_TSFTIMERVALUECHANGE      1
#define T0CR_TSF_TIMER_SW_MODE_MASK         BITS(0,1)

/* T1CR */
#define T1CR_TSF_DRIFT_WINDOW_UPDATE        BIT(24)
#define T1CR_TSF_DRIFT_WINDOW               BITS(16,23)
#define T1CR_TSF_DRIFT_WINDOW_OFFSET        16

#define T1CR_TSF_TIMER_HW_MODE_FULL_ADHOC   (3 << 2)
#define T1CR_TSF_TIMER_HW_MODE_RX_ONLY      (2 << 2)
#define T1CR_TSF_TIMER_HW_MODE_TICK_ONLY    (1 << 2)
#define T1CR_TSF_TIMER_HW_MODE_FULL         (0 << 2)
#define T1CR_TSF_TIMER_HW_MODE              BITS(2,3)

#define T1CR_TSF_TIMER_SW_MODE_TSFTIMERVALUEREAD        3
#define T1CR_TSF_TIMER_SW_MODE_TSFTIMERVALUEADJUST      2
#define T1CR_TSF_TIMER_SW_MODE_TSFTIMERVALUECHANGE      1
#define T1CR_TSF_TIMER_SW_MODE_MASK         BITS(0,1)

/* T2CR */
#define T2CR_TSF_DRIFT_WINDOW_UPDATE        BIT(24)
#define T2CR_TSF_DRIFT_WINDOW               BITS(16,23)
#define T2CR_TSF_DRIFT_WINDOW_OFFSET        16

#define T2CR_TSF_TIMER_HW_MODE_FULL_ADHOC   (3 << 2)
#define T2CR_TSF_TIMER_HW_MODE_RX_ONLY      (2 << 2)
#define T2CR_TSF_TIMER_HW_MODE_TICK_ONLY    (1 << 2)
#define T2CR_TSF_TIMER_HW_MODE_FULL         (0 << 2)
#define T2CR_TSF_TIMER_HW_MODE              BITS(2,3)

#define T2CR_TSF_TIMER_SW_MODE_TSFTIMERVALUEREAD        3
#define T2CR_TSF_TIMER_SW_MODE_TSFTIMERVALUEADJUST      2
#define T2CR_TSF_TIMER_SW_MODE_TSFTIMERVALUECHANGE      1
#define T2CR_TSF_TIMER_SW_MODE_MASK         BITS(0,1)

/* T3CR */
#define T3CR_TSF_DRIFT_WINDOW_UPDATE        BIT(24)
#define T3CR_TSF_DRIFT_WINDOW               BITS(16,23)
#define T3CR_TSF_DRIFT_WINDOW_OFFSET        16

#define T3CR_TSF_TIMER_HW_MODE_FULL_ADHOC   (3 << 2)
#define T3CR_TSF_TIMER_HW_MODE_RX_ONLY      (2 << 2)
#define T3CR_TSF_TIMER_HW_MODE_TICK_ONLY    (1 << 2)
#define T3CR_TSF_TIMER_HW_MODE_FULL         (0 << 2)
#define T3CR_TSF_TIMER_HW_MODE              BITS(2,3)

#define T3CR_TSF_TIMER_SW_MODE_TSFTIMERVALUEREAD        3
#define T3CR_TSF_TIMER_SW_MODE_TSFTIMERVALUEADJUST      2
#define T3CR_TSF_TIMER_SW_MODE_TSFTIMERVALUECHANGE      1
#define T3CR_TSF_TIMER_SW_MODE_MASK         BITS(0,1)

/* UTTR0 */
#define UTTR0_UPDATEDTSFTIMER               BITS(0,31)

/* UTTR1 */
#define UTTR1_UPDATEDTSFTIMER               BITS(0,31)

/* BEIR */
#define BEIR_TEI_CAL_EN                     BIT(24)
#define BEIR_TIM_EARLIER_INTERVAL           BITS(23,16)
#define BEIR_TIM_EARLIER_INTERVAL_OFFSET    16
#define BEIR_BEI_CAL_EN                     BIT(8)
#define BEIR_BCN_EARLIER_INTERVAL           BITS(0,7)

#if 0 //marked for mt6630
/* LPIAR0 */ //Deprecated in MT6630
#define LPIAR0_WLAN_ON_SLEEP_START_ADDR     BITS(24,31)
#define LPIAR0_WLAN_ON_SLEEP_END_ADDR       BITS(16,23)
#define LPIAR0_INITIAL_SLEEP_START_ADDR     BITS(8,15)
#define LPIAR0_INITIAL_SLEEP_END_ADDR       BITS(0,7)

/* LPIAR1 */ //Deprecated in MT6630
#define LPIAR1_SLEEP_WLAN_ON_START_ADDR     BITS(24,31)
#define LPIAR1_SLEEP_WLAN_ON_END_ADDR       BITS(16,23)
#define LPIAR1_WLAN_ON_ON_START_ADDR        BITS(8,15)
#define LPIAR1_WLAN_ON_ON_END_ADDR          BITS(0,7)
#endif

/* TIMTR */
#define TIMTR_TIM_MAX_TIME_LIMIT            BITS(24, 31)
#define TIMTR_TIM_MAX_TIME_LIMIT_OFFSET     24
#define TIMTR_TIM_MAX_TIME_LIMIT_VALID      BIT(23)
#define TIMTR_TIM_MIN_TIME_LIMIT_VALID      BIT(22)
#define TIMTR_TIM_MIN_TIME_LIMIT            BITS(16, 21)
#define TIMTR_TIM_MIN_TIME_LIMIT_OFFSET     16
#define TIMTR_TIM_TIMEOUT_MASK              BITS(16,31)

#define TIMTR_TIM_BMC_MAX_TIME_LIMIT        BITS(8, 15)
#define TIMTR_TIM_BMC_MAX_TIME_LIMIT_OFFSET 8
#define TIMTR_TIM_BMC_MAX_TIME_LIMIT_VALID  BIT(7)
#define TIMTR_TIM_BMC_MIN_TIME_LIMIT_VALID  BIT(6)
#define TIMTR_TIM_BMC_MIN_TIME_LIMIT        BITS(0, 5)
#define TIMTR_TIM_BMC_MIN_TIME_LIMIT_OFFSET 0
#define TIMTR_TIM_BMC_TIMEOUT_MASK          BITS(0,15)


/* T0STR */
#define T0STR_UPDATE_DTIM_COUNT             BIT(31)
#define T0STR_UPDATE_NEXT_TBTT              BIT(30)
#define T0STR_TBTT_CAL_BUSY                 BIT(29)
#define T0STR_CTBCN2_WAKEUP_NEXTDTIM        BIT(28)
#define T0STR_EARILER_TBTT_OFFSET           BITS(24,27)
#define T0STR_EARILER_TBTT_OFFSET_OFFSET    24
#define T0STR_DTIM_COUNT                    BITS(16,23)
#define T0STR_DTIM_COUNT_OFFSET             16
#define T0STR_NEXT_TBTT_TIME                BITS(0,15)

/* T1STR */
#define T1STR_UPDATE_DTIM_COUNT             BIT(31)
#define T1STR_UPDATE_NEXT_TBTT              BIT(30)
#define T1STR_TBTT_CAL_BUSY                 BIT(29)
#define T1STR_CTBCN2_WAKEUP_NEXTDTIM        BIT(28)
#define T1STR_EARILER_TBTT_OFFSET           BITS(24,27)
#define T1STR_EARILER_TBTT_OFFSET_OFFSET    24
#define T1STR_DTIM_COUNT                    BITS(16,23)
#define T1STR_DTIM_COUNT_OFFSET             16
#define T1STR_NEXT_TBTT_TIME                BITS(0,15)

/* T2STR */
#define T2STR_UPDATE_DTIM_COUNT             BIT(31)
#define T2STR_UPDATE_NEXT_TBTT              BIT(30)
#define T2STR_TBTT_CAL_BUSY                 BIT(29)
#define T2STR_CTBCN2_WAKEUP_NEXTDTIM        BIT(28)
#define T2STR_EARILER_TBTT_OFFSET           BITS(24,27)
#define T2STR_EARILER_TBTT_OFFSET_OFFSET    24
#define T2STR_DTIM_COUNT                    BITS(16,23)
#define T2STR_DTIM_COUNT_OFFSET             16
#define T2STR_NEXT_TBTT_TIME                BITS(0,15)

/* T3STR */
#define T3STR_UPDATE_DTIM_COUNT             BIT(31)
#define T3STR_UPDATE_NEXT_TBTT              BIT(30)
#define T3STR_TBTT_CAL_BUSY                 BIT(29)
#define T3STR_CTBCN2_WAKEUP_NEXTDTIM        BIT(28)
#define T3STR_EARILER_TBTT_OFFSET           BITS(24,27)
#define T3STR_EARILER_TBTT_OFFSET_OFFSET    24
#define T3STR_DTIM_COUNT                    BITS(16,23)
#define T3STR_DTIM_COUNT_OFFSET             16
#define T3STR_NEXT_TBTT_TIME                BITS(0,15)

/* TT0STR */
#define TT0STR_TTTT0_CAL_BUSY               BIT(31)
#define TT0STR_UPDATE_NEXT_TTTT0            BIT(30)
#define TT0STR_EARLIER_TTTT0_OFFSET         BITS(26,29)
#define TT0STR_EARLIER_TTTT0_OFFSET_OFFSET  26
#define TT0STR_NEXT_TTTT_TIME               BITS(0,25)

/* TT1STR */
#define TT1STR_TTTT1_CAL_BUSY               BIT(31)
#define TT1STR_UPDATE_NEXT_TTTT1            BIT(30)
#define TT1STR_EARLIER_TTTT1_OFFSET         BITS(26,29)
#define TT1STR_EARLIER_TTTT1_OFFSET_OFFSET  26
#define TT1STR_NEXT_TTTT_TIME               BITS(0,25)

/* TT2STR */
#define TT2STR_TTTT2_CAL_BUSY               BIT(31)
#define TT2STR_UPDATE_NEXT_TTTT2            BIT(30)
#define TT2STR_EARLIER_TTTT2_OFFSET         BITS(26,29)
#define TT2STR_EARLIER_TTTT2_OFFSET_OFFSET  26
#define TT2STR_NEXT_TTTT_TIME               BITS(0,25)

/* TT3STR */
#define TT3STR_TTTT3_CAL_BUSY               BIT(31)
#define TT3STR_UPDATE_NEXT_TTTT3            BIT(30)
#define TT3STR_EARLIER_TTTT3_OFFSET         BITS(26,29)
#define TT3STR_EARLIER_TTTT3_OFFSET_OFFSET  26
#define TT3STR_NEXT_TTTT_TIME               BITS(0,25)

/* PISR */
//#define PISR_PRETBTT_INTERVAL               BITS(24,31)
//#define PISR_PRETBTT_INTERVAL_OFFSET        24
#define PISR_PRETBTT_INTERVAL3               BITS(24,31)
#define PISR_PRETBTT_INTERVAL3_OFFSET        24
#define PISR_PRETBTT_INTERVAL2               BITS(16,23)
#define PISR_PRETBTT_INTERVAL2_OFFSET        16
#define PISR_PRETBTT_INTERVAL1               BITS(8,15)
#define PISR_PRETBTT_INTERVAL1_OFFSET        8
#define PISR_PRETBTT_INTERVAL0               BITS(0,7)
#define PISR_PRETBTT_INTERVAL0_OFFSET        0

/* T0TPCR */
#define T0TPCR_TBTT_CAL_EN                  BIT(31)
#define T0TPCR_DTIMWAKEPERIOD               BITS(28,30)
#define T0TPCR_DTIMWAKEPERIOD_OFFSET        28
#define T0TPCR_TBTTWAKEPERIOD               BITS(24,27)
#define T0TPCR_TBTTWAKEPERIOD_OFFSET        24
#define T0TPCR_DTIMPERIOD                   BITS(16,23)
#define T0TPCR_DTIMPERIOD_OFFSET            16
#define T0TPCR_BEACONPERIOD                 BITS(0,15)
#define T0TPCR_BEACONPERIOD_OFFSET          0
#define T0TPCR_TIMERPERIOD_MASK             BITS(0,30)

/* T1TPCR */
#define T1TPCR_TBTT_CAL_EN                  BIT(31)
#define T1TPCR_DTIMWAKEPERIOD               BITS(28,30)
#define T1TPCR_DTIMWAKEPERIOD_OFFSET        28
#define T1TPCR_TBTTWAKEPERIOD               BITS(24,27)
#define T1TPCR_TBTTWAKEPERIOD_OFFSET        24
#define T1TPCR_DTIMPERIOD                   BITS(16,23)
#define T1TPCR_DTIMPERIOD_OFFSET            16
#define T1TPCR_BEACONPERIOD                 BITS(0,15)
#define T1TPCR_BEACONPERIOD_OFFSET          0
#define T1TPCR_TIMERPERIOD_MASK             BITS(0,30)

/* T2TPCR */
#define T2TPCR_TBTT_CAL_EN                  BIT(31)
#define T2TPCR_DTIMWAKEPERIOD               BITS(28,30)
#define T2TPCR_DTIMWAKEPERIOD_OFFSET        28
#define T2TPCR_TBTTWAKEPERIOD               BITS(24,27)
#define T2TPCR_TBTTWAKEPERIOD_OFFSET        24
#define T2TPCR_DTIMPERIOD                   BITS(16,23)
#define T2TPCR_DTIMPERIOD_OFFSET            16
#define T2TPCR_BEACONPERIOD                 BITS(0,15)
#define T2TPCR_BEACONPERIOD_OFFSET          0
#define T2TPCR_TIMERPERIOD_MASK             BITS(0,30)

/* T3TPCR */
#define T3TPCR_TBTT_CAL_EN                  BIT(31)
#define T3TPCR_DTIMWAKEPERIOD               BITS(28,30)
#define T3TPCR_DTIMWAKEPERIOD_OFFSET        28
#define T3TPCR_TBTTWAKEPERIOD               BITS(24,27)
#define T3TPCR_TBTTWAKEPERIOD_OFFSET        24
#define T3TPCR_DTIMPERIOD                   BITS(16,23)
#define T3TPCR_DTIMPERIOD_OFFSET            16
#define T3TPCR_BEACONPERIOD                 BITS(0,15)
#define T3TPCR_BEACONPERIOD_OFFSET          0
#define T3TPCR_TIMERPERIOD_MASK             BITS(0,30)

/* TTSR */ // Extend to t0~7 for MT6630
#define TTSR_TIMER_SEL                      BITS(29,31)
#define TTSR_TIMER_SEL_OFFSET               29
#define TTSR_TIME_VALUE_CTRL_PERIOD         BIT(28)
#define TTSR_LOCAL_TSF_TIMER_ID             BITS(24,25)
#define TTSR_LOCAL_TSF_TIMER_ID_OFFSET      24
#define TTSR_TIME_VALUE                     BITS(0,21)

/* TFRSR */
#define TFRSR_PERIODICAL_CTRL               BIT(31)
#define TFRSR_TIME_VALUE                    BITS(0,25)

/* DTIMCNT */
#define DTIMCNTHW_DTIMWAKEUP_CNT3           BITS(28,30)
#define DTIMCNT_HW_TBTTWAKEUP_CNT3          BITS(24,27)
#define DTIMCNTHW_DTIMWAKEUP_CNT2           BITS(20,22)
#define DTIMCNT_HW_TBTTWAKEUP_CNT2          BITS(16,19)
#define DTIMCNTHW_DTIMWAKEUP_CNT1           BITS(12,14)
#define DTIMCNT_HW_TBTTWAKEUP_CNT1          BITS(8,11)
#define DTIMCNTHW_DTIMWAKEUP_CNT0           BITS(4,6)
#define DTIMCNT_HW_TBTTWAKEUP_CNT0          BITS(0,3)

/* LPPTCR */ // Move to LP_AON for MT6630
#define LPPTCR_ANALOG_STABLE_TIME           BITS(24,30)
#define LPPTCR_ANALOG_STABLE_TIME_OFFSET    24
#define LPPTCR_OSC_STABLE_TIME              BITS(16,23)
#define LPPTCR_OSC_STABLE_TIME_OFFSET       16

/* SPCR */ //Extend to 0~3 BSSID for MT6630
#define SPCR_BMC_SP3_STATE                  BIT(27)
#define SPCR_BEACON_SP3_STATE               BIT(26)
#define SPCR_BMC_SP2_STATE                  BIT(25)
#define SPCR_BEACON_SP2_STATE               BIT(24)
#define SPCR_BMC_SP3_RESET                  BIT(23)
#define SPCR_BEACON_SP3_RESET               BIT(22)
#define SPCR_BMC_SP2_RESET                  BIT(21)
#define SPCR_BEACON_SP2_RESET               BIT(20)
#define SPCR_BMC_SP3_ENABLE                 BIT(19)
#define SPCR_BEACON_SP3_ENABLE              BIT(18)
#define SPCR_BMC_SP2_ENABLE                 BIT(17)
#define SPCR_BEACON_SP2_ENABLE              BIT(16)
#define SPCR_NDPA_SP_STATE                  BIT(14)
#define SPCR_NDPA_SP_RESET                  BIT(13)
#define SPCR_NDPA_SP_ENABLE                 BIT(12)
#define SPCR_BMC_SP1_STATE                  BIT(11)
#define SPCR_BEACON_SP1_STATE               BIT(10)
#define SPCR_BMC_SP0_STATE                  BIT(9)
#define SPCR_BEACON_SP0_STATE               BIT(8)
#define SPCR_BMC_SP1_RESET                  BIT(7)
#define SPCR_BEACON_SP1_RESET               BIT(6)
#define SPCR_BMC_SP0_RESET                  BIT(5)
#define SPCR_BEACON_SP0_RESET               BIT(4)
#define SPCR_BMC_SP1_ENABLE                 BIT(3)
#define SPCR_BEACON_SP1_ENABLE              BIT(2)
#define SPCR_BMC_SP0_ENABLE                 BIT(1)
#define SPCR_BEACON_SP0_ENABLE              BIT(0)

/* BCNTR */
#define BCNTR_BCN_MAX_TIME_LIMIT            BITS(24,31)
#define BCNTR_BCN_MAX_TIME_LIMIT_OFFSET     24
#define BCNTR_BCN_MAX_TIME_LIMIT_VALID      BIT(23)
#define BCNTR_BCN_MIN_TIME_LIMIT_VALID      BIT(22)
#define BCNTR_BCN_MIN_TIME_LIMIT            BITS(16,21)
#define BCNTR_BCN_MIN_TIME_LIMIT_OFFSET     16
#define BCNTR_BCN_TIMEOUT_MASK              BITS(16,31)

#define BCNTR_BMC_MAX_TIME_LIMIT            BITS(8,15)
#define BCNTR_BMC_MAX_TIME_LIMIT_OFFSET     8
#define BCNTR_BMC_MAX_TIME_LIMIT_VALID      BIT(7)
#define BCNTR_BMC_MIN_TIME_LIMIT_VALID      BIT(6)
#define BCNTR_BMC_MIN_TIME_LIMIT            BITS(0,5)
#define BCNTR_BMC_MIN_TIME_LIMIT_OFFSET     0
#define BCNTR_BMC_TIMEOUT_MASK              BITS(0,15)

/* TCLCR */
//#define TCLCR_TIMEOUT_EVENT_COUNTER_RESET_CTRL  BIT(31)
#define TCLCR_BCN_TO_EVT_CNT_RESET_CTRL            BIT(31)
#define TCLCR_MIN_TIME_CTRL_USE_CCA                BIT(30)
#define TCLCR_TIM_TO_EVT_CNT_RESET_CTRL            BIT(29)
#define TCLCR_TIM_BMC_TIMEOUT_COUNT_LIMIT          BITS(24,27)
#define TCLCR_TIM_BMC_TIMEOUT_COUNT_LIMIT_OFFSET   24
#define TCLCR_TIM_TIMEOUT_COUNT_LIMIT              BITS(16,21)
#define TCLCR_TIM_TIMEOUT_COUNT_LIMIT_OFFSET       16
#define TCLCR_BMC_TIMEOUT_COUNT_LIMIT              BITS(8,11)
#define TCLCR_BMC_TIMEOUT_COUNT_LIMIT_OFFSET       8
#define TCLCR_BCN_TIMEOUT_COUNT_LIMIT              BITS(0,5)
#define TCLCR_BCN_TIMEOUT_COUNT_LIMIT_OFFSET       0

/* MPTCR0 */
#define MPTCR0_SP_END_CHK_EN                BIT(31)
#define MPTCR0_BCN_BMC1_EN                  BIT(30)
#define MPTCR0_BCN_PARSE_TIM1_EN            BIT(29)
#define MPTCR0_BCN_BMC0_EN                  BIT(28)
#define MPTCR0_BCN_PARSE_TIM0_EN            BIT(27)
#define MPTCR0_T8TIMER_EN                   BIT(20)
#define MPTCR0_T3TIMER_EN                   BIT(19)
#define MPTCR0_T2TIMER_EN                   BIT(18)
#define MPTCR0_T1TIMER_EN                   BIT(17)
#define MPTCR0_T0TIMER_EN                   BIT(16)

#define MPTCR0_PRETBTT1INT_EN               BIT(15) //New
#define MPTCR0_BMC_TIMEOUT1_EN              BIT(14)
#define MPTCR0_BCN_TIMEOUT1_EN              BIT(13)
#define MPTCR0_PRETBTT1TIMEUP_EN            BIT(12)
#define MPTCR0_TBTT1TIMEUP_EN               BIT(11)
#define MPTCR0_PREDTIM1_TRIG_EN             BIT(10)
#define MPTCR0_PRETBTT1_TRIG_EN             BIT(9)
#define MPTCR0_TBTT1PERIODTIMER_EN          BIT(8)

#define MPTCR0_PRETBTT0INT_EN               BIT(7) //New
#define MPTCR0_BMC_TIMEOUT0_EN              BIT(6)
#define MPTCR0_BCN_TIMEOUT0_EN              BIT(5)
#define MPTCR0_PRETBTT0TIMEUP_EN            BIT(4)
#define MPTCR0_TBTT0TIMEUP_EN               BIT(3)
#define MPTCR0_PREDTIM0_TRIG_EN             BIT(2)
#define MPTCR0_PRETBTT0_TRIG_EN             BIT(1)
#define MPTCR0_TBTT0PERIODTIMER_EN          BIT(0)

/* MPTCR1 */
#define MPTCR1_SP_END_CHK_DIS               BIT(31)
#define MPTCR1_BCN_BMC1_DIS                 BIT(30)
#define MPTCR1_BCN_PARSE_TIM1_DIS           BIT(29)
#define MPTCR1_BCN_BMC0_DIS                 BIT(28)
#define MPTCR1_BCN_PARSE_TIM0_DIS           BIT(27)
#define MPTCR1_T8TIMER_DIS                  BIT(20)
#define MPTCR1_T3TIMER_DIS                  BIT(19)
#define MPTCR1_T2TIMER_DIS                  BIT(18)
#define MPTCR1_T1TIMER_DIS                  BIT(17)
#define MPTCR1_T0TIMER_DIS                  BIT(16)

#define MPTCR1_PRETBTT1INT_DIS              BIT(15) //New
#define MPTCR1_BMC_TIMEOUT1_DIS             BIT(14)
#define MPTCR1_BCN_TIMEOUT1_DIS             BIT(13)
#define MPTCR1_PRETBTT1TIMEUP_DIS           BIT(12)
#define MPTCR1_TBTT1TIMEUP_DIS              BIT(11)
#define MPTCR1_PREDTIM1_TRIG_DIS            BIT(10)
#define MPTCR1_PRETBTT1_TRIG_DIS            BIT(9)
#define MPTCR1_TBTT1PERIODTIMER_DIS         BIT(8)

#define MPTCR1_PRETBTT0INT_DIS              BIT(7) //New
#define MPTCR1_BMC_TIMEOUT0_DIS             BIT(6)
#define MPTCR1_BCN_TIMEOUT0_DIS             BIT(5)
#define MPTCR1_PRETBTT0TIMEUP_DIS           BIT(4)
#define MPTCR1_TBTT0TIMEUP_DIS              BIT(3)
#define MPTCR1_PREDTIM0_TRIG_DIS            BIT(2)
#define MPTCR1_PRETBTT0_TRIG_DIS            BIT(1)
#define MPTCR1_TBTT0PERIODTIMER_DIS         BIT(0)

/* MPTCR2 */
#define MPTCR2_BCN_BMC3_EN                  BIT(30)
#define MPTCR2_BCN_PARSE_TIM3_EN            BIT(29)
#define MPTCR2_BCN_BMC2_EN                  BIT(28)
#define MPTCR2_BCN_PARSE_TIM2_EN            BIT(27)
#define MPTCR2_T7TIMER_EN                   BIT(19)
#define MPTCR2_T6TIMER_EN                   BIT(18)
#define MPTCR2_T5TIMER_EN                   BIT(17)
#define MPTCR2_T4TIMER_EN                   BIT(16)

#define MPTCR2_PRETBTT3INT_EN               BIT(15) //New
#define MPTCR2_BMC_TIMEOUT3_EN              BIT(14)
#define MPTCR2_BCN_TIMEOUT3_EN              BIT(13)
#define MPTCR2_PRETBTT3TIMEUP_EN            BIT(12)
#define MPTCR2_TBTT3TIMEUP_EN               BIT(11)
#define MPTCR2_PREDTIM3_TRIG_EN             BIT(10)
#define MPTCR2_PRETBTT3_TRIG_EN             BIT(9)
#define MPTCR2_TBTT3PERIODTIMER_EN          BIT(8)

#define MPTCR2_PRETBTT2INT_EN               BIT(7) //New
#define MPTCR2_BMC_TIMEOUT2_EN              BIT(6)
#define MPTCR2_BCN_TIMEOUT2_EN              BIT(5)
#define MPTCR2_PRETBTT2TIMEUP_EN            BIT(4)
#define MPTCR2_TBTT2TIMEUP_EN               BIT(3)
#define MPTCR2_PREDTIM2_TRIG_EN             BIT(2)
#define MPTCR2_PRETBTT2_TRIG_EN             BIT(1)
#define MPTCR2_TBTT2PERIODTIMER_EN          BIT(0)

/* MPTCR3 */
#define MPTCR3_BCN_BMC3_DIS                 BIT(30)
#define MPTCR3_BCN_PARSE_TIM3_DIS           BIT(29)
#define MPTCR3_BCN_BMC2_DIS                 BIT(28)
#define MPTCR3_BCN_PARSE_TIM2_DIS           BIT(27)
#define MPTCR3_T7TIMER_DIS                  BIT(19)
#define MPTCR3_T6TIMER_DIS                  BIT(18)
#define MPTCR3_T5TIMER_DIS                  BIT(17)
#define MPTCR3_T4TIMER_DIS                  BIT(16)

#define MPTCR3_PRETBTT3INT_DIS              BIT(15) //New
#define MPTCR3_BMC_TIMEOUT3_DIS             BIT(14)
#define MPTCR3_BCN_TIMEOUT3_DIS             BIT(13)
#define MPTCR3_PRETBTT3TIMEUP_DIS           BIT(12)
#define MPTCR3_TBTT3TIMEUP_DIS              BIT(11)
#define MPTCR3_PREDTIM3_TRIG_DIS            BIT(10)
#define MPTCR3_PRETBTT3_TRIG_DIS            BIT(9)
#define MPTCR3_TBTT3PERIODTIMER_DIS         BIT(8)

#define MPTCR3_PRETBTT2INT_DIS              BIT(7) //New
#define MPTCR3_BMC_TIMEOUT2_DIS             BIT(6)
#define MPTCR3_BCN_TIMEOUT2_DIS             BIT(5)
#define MPTCR3_PRETBTT2TIMEUP_DIS           BIT(4)
#define MPTCR3_TBTT2TIMEUP_DIS              BIT(3)
#define MPTCR3_PREDTIM2_TRIG_DIS            BIT(2)
#define MPTCR3_PRETBTT2_TRIG_DIS            BIT(1)
#define MPTCR3_TBTT2PERIODTIMER_DIS         BIT(0)

/* MPTCR4 */ //NPS Support
#define MPTCR4_TIM_BMC1_EN                  BIT(30)
#define MPTCR4_TIM_PARSE_TIM1_EN            BIT(29)
#define MPTCR4_TIM_BMC0_EN                  BIT(28)
#define MPTCR4_TIM_PARSE_TIM0_EN            BIT(27)

#define MPTCR4_PRETTTT1INT_EN               BIT(15) //New
#define MPTCR4_TIM_BMC_TIMEOUT1_EN          BIT(14)
#define MPTCR4_TIM_TIMEOUT1_EN              BIT(13)
#define MPTCR4_PRETTTT1TIMEUP_EN            BIT(12)
#define MPTCR4_TTTT1TIMEUP_EN               BIT(11)
#define MPTCR4_PRETTTT1_TRIG_EN             BIT(9)
#define MPTCR4_TTTT1PERIODTIMER_EN          BIT(8)

#define MPTCR4_PRETTTT0INT_EN               BIT(7) //New
#define MPTCR4_TIM_BMC_TIMEOUT0_EN          BIT(6)
#define MPTCR4_TIM_TIMEOUT0_EN              BIT(5)
#define MPTCR4_PRETTTT0TIMEUP_EN            BIT(4)
#define MPTCR4_TTTT0TIMEUP_EN               BIT(3)
#define MPTCR4_PRETTTT0_TRIG_EN             BIT(1)
#define MPTCR4_TTTT0PERIODTIMER_EN          BIT(0)

/* MPTCR5 */ //NPS Support
#define MPTCR5_TIM_BMC1_DIS                 BIT(30)
#define MPTCR5_TIM_PARSE_TIM1_DIS           BIT(29)
#define MPTCR5_TIM_BMC0_DIS                 BIT(28)
#define MPTCR5_TIM_PARSE_TIM0_DIS           BIT(27)

#define MPTCR5_PRETTTT1INT_DIS              BIT(15) //New
#define MPTCR5_TIM_BMC_TIMEOUT1_DIS         BIT(14)
#define MPTCR5_TIM_TIMEOUT1_DIS             BIT(13)
#define MPTCR5_PRETTTT1TIMEUP_DIS           BIT(12)
#define MPTCR5_TTTT1TIMEUP_DIS              BIT(11)
#define MPTCR5_PRETTTT1_TRIG_DIS            BIT(9)
#define MPTCR5_TTTT1PERIODTIMER_DIS         BIT(8)

#define MPTCR5_PRETTTT0INT_DIS              BIT(7) //New
#define MPTCR5_TIM_BMC_TIMEOUT0_DIS         BIT(6)
#define MPTCR5_TIM_TIMEOUT0_DIS             BIT(5)
#define MPTCR5_PRETTTT0TIMEUP_DIS           BIT(4)
#define MPTCR5_TTTT0TIMEUP_DIS              BIT(3)
#define MPTCR5_PRETTTT0_TRIG_DIS            BIT(1)
#define MPTCR5_TTTT0PERIODTIMER_DIS         BIT(0)

/* MPTCR6 */ //NPS Support
#define MPTCR6_TIM_BMC3_EN                  BIT(30)
#define MPTCR6_TIM_PARSE_TIM3_EN            BIT(29)
#define MPTCR6_TIM_BMC2_EN                  BIT(28)
#define MPTCR6_TIM_PARSE_TIM2_EN            BIT(27)

#define MPTCR6_PRETTTT3INT_EN               BIT(15) //New
#define MPTCR6_TIM_BMC_TIMEOUT3_EN          BIT(14)
#define MPTCR6_TIM_TIMEOUT3_EN              BIT(13)
#define MPTCR6_PRETTTT3TIMEUP_EN            BIT(12)
#define MPTCR6_TTTT3TIMEUP_EN               BIT(11)
#define MPTCR6_PRETTTT3_TRIG_EN             BIT(9)
#define MPTCR6_TTTT3PERIODTIMER_EN          BIT(8)

#define MPTCR6_PRETTTT2INT_EN               BIT(7) //New
#define MPTCR6_TIM_BMC_TIMEOUT2_EN          BIT(6)
#define MPTCR6_TIM_TIMEOUT2_EN              BIT(5)
#define MPTCR6_PRETTTT2TIMEUP_EN            BIT(4)
#define MPTCR6_TTTT2TIMEUP_EN               BIT(3)
#define MPTCR6_PRETTTT2_TRIG_EN             BIT(1)
#define MPTCR6_TTTT2PERIODTIMER_EN          BIT(0)

/* MPTCR7 */ //NPS Support
#define MPTCR7_TIM_BMC3_DIS                 BIT(30)
#define MPTCR7_TIM_PARSE_TIM3_DIS           BIT(29)
#define MPTCR7_TIM_BMC2_DIS                 BIT(28)
#define MPTCR7_TIM_PARSE_TIM2_DIS           BIT(27)

#define MPTCR7_PRETTTT3INT_DIS              BIT(15) //New
#define MPTCR7_TIM_BMC_TIMEOUT3_DIS         BIT(14)
#define MPTCR7_TIM_TIMEOUT3_DIS             BIT(13)
#define MPTCR7_PRETTTT3TIMEUP_DIS           BIT(12)
#define MPTCR7_TTTT3TIMEUP_DIS              BIT(11)
#define MPTCR7_PRETTTT3_TRIG_DIS            BIT(9)
#define MPTCR7_TTTT3PERIODTIMER_DIS         BIT(8)

#define MPTCR7_PRETTTT2INT_DIS              BIT(7) //New
#define MPTCR7_TIM_BMC_TIMEOUT2_DIS         BIT(6)
#define MPTCR7_TIM_TIMEOUT2_DIS             BIT(5)
#define MPTCR7_PRETTTT2TIMEUP_DIS           BIT(4)
#define MPTCR7_TTTT2TIMEUP_DIS              BIT(3)
#define MPTCR7_PRETTTT2_TRIG_DIS            BIT(1)
#define MPTCR7_TTTT2PERIODTIMER_DIS         BIT(0)

/* AHB_CLK_CTL */ //Deprecated
#define LP_AHB_CLK_CTL_ALWAYS_ON            BIT(3)
#define LP_AHB_CLK_CTL_CTRL_BY_LP           BIT(2)
#define LP_AHB_CLK_CTL_CTRL_BY_DMA          BIT(1)
#define LP_AHB_CLK_CTL_CTRL_BY_CMDBT        BIT(0)

/* FRCR */
#define FRCR_COUNTER            BITS(0,31)

/* WLANCKCR */
#define WLANCKCR_WLAN_CLK_MODE2 BITS(24,31)
#define WLANCKCR_WLAN_CLK_MODE1 BITS(16,23)
#define WLANCKCR_WLAN_CLK_MODE0 BITS(8,15)
#define WLANCKCR_WLAN_CLK_MODE  BITS(0,1)

/* TXTCR0 */
#define TXTCR_CEASE_RWP_TRAP    BIT(9)
#define TXTCR_CEASE_BMC_TRAP    BIT(8)
#define TXTCR_CEASE_BCN_TRAP    BIT(7)
#define TXTCR_CEASE_AC6_TRAP    BIT(6)
#define TXTCR_CEASE_AC5_TRAP    BIT(5)
#define TXTCR_CEASE_TX_TRAP_MASK     BITS(3, 4)
#define TXTCR_CEASE_TX_TRAP1    BIT(4)
#define TXTCR_CEASE_TX_TRAP     BIT(3)

#define TXTCR_PERIOD_ONE_SHOT  BIT(1)
#define TXTCR_TX_TRAP_TIME_EN    BIT(0)

#define TXTCR0_CEASE_RWP_TRAP    BIT(9)
#define TXTCR0_CEASE_RWP_TRAP_OFFSET   9
#define TXTCR0_CEASE_BMC_TRAP    BIT(8)
#define TXTCR0_CEASE_BCN_TRAP    BIT(7)
#define TXTCR0_CEASE_AC6_TRAP    BIT(6)
#define TXTCR0_CEASE_AC5_TRAP    BIT(5)
#define TXTCR0_CEASE_TX_TRAP0_MASK     BITS(3, 4)
//#define TXTCR0_CEASE_TX_TRAP1    BIT(4)
#define TXTCR0_CEASE_TX_TRAP0    BITS(3,4)
//#define TXTCR0_TSF_TRAP_SEL      BIT(2)
#define TXTCR0_PERIOD_ONE_SHOT   BIT(1)
#define TXTCR0_TX_TRAP_TIME_EN   BIT(0)

/* TXTCR1 */
#define TXTCR1_CEASE_RWP_TRAP    BIT(9)
#define TXTCR1_CEASE_RWP_TRAP_OFFSET   9
#define TXTCR1_CEASE_BMC_TRAP    BIT(8)
#define TXTCR1_CEASE_BCN_TRAP    BIT(7)
#define TXTCR1_CEASE_AC6_TRAP    BIT(6)
#define TXTCR1_CEASE_AC5_TRAP    BIT(5)
#define TXTCR1_CEASE_TX_TRAP0_MASK     BITS(3, 4)
#define TXTCR1_CEASE_TX_TRAP0    BITS(3,4)
#define TXTCR1_PERIOD_ONE_SHOT   BIT(1)
#define TXTCR1_TX_TRAP_TIME_EN   BIT(0)

/* TXTCR2 */
#define TXTCR2_CEASE_RWP_TRAP    BIT(9)
#define TXTCR2_CEASE_RWP_TRAP_OFFSET   9
#define TXTCR2_CEASE_BMC_TRAP    BIT(8)
#define TXTCR2_CEASE_BCN_TRAP    BIT(7)
#define TXTCR2_CEASE_AC6_TRAP    BIT(6)
#define TXTCR2_CEASE_AC5_TRAP    BIT(5)
#define TXTCR2_CEASE_TX_TRAP0_MASK     BITS(3, 4)
#define TXTCR2_CEASE_TX_TRAP0    BITS(3,4)
#define TXTCR2_PERIOD_ONE_SHOT   BIT(1)
#define TXTCR2_TX_TRAP_TIME_EN   BIT(0)

/* TXTCR3 */
#define TXTCR3_CEASE_RWP_TRAP    BIT(9)
#define TXTCR3_CEASE_RWP_TRAP_OFFSET   9
#define TXTCR3_CEASE_BMC_TRAP    BIT(8)
#define TXTCR3_CEASE_BCN_TRAP    BIT(7)
#define TXTCR3_CEASE_AC6_TRAP    BIT(6)
#define TXTCR3_CEASE_AC5_TRAP    BIT(5)
#define TXTCR3_CEASE_TX_TRAP0_MASK     BITS(3, 4)
#define TXTCR3_CEASE_TX_TRAP0    BITS(3,4)
#define TXTCR3_PERIOD_ONE_SHOT   BIT(1)
#define TXTCR3_TX_TRAP_TIME_EN   BIT(0)

/* LP_TT0SBOR1~15 */
#define SB_PRE_TTTT0_INT_EN         BIT(31)
#define SB_TTTT0_INT_EN             BIT(30)
#define SB_TTTT0_EN                 BIT(29)
#define SB_TTTT0_OFFSET_MASK        BITS(0,17)

/* LP_SBTOR1~15 */
#define SB_PRE_TBTT0_INT_EN         BIT(31)
#define SB_TBTT0_INT_EN             BIT(30)
#define SB_TSF_EN                   BIT(29)
#define SB_TSF_OFFSET_MASK          BITS(0,19)

/* TTR0~3 */
#define TTR0_TRAP0_TIME          BITS(0,31) //TSF low 32bits
#define TTR1_TRAP1_TIME          BITS(0,31)
#define TTR2_TRAP2_TIME          BITS(0,31)
#define TTR3_TRAP3_TIME          BITS(0,31)

/* PTTISR */ //NPS Support
#define PTTISR_PRETTTT_INTERVAL3 BITS(24,31) //64us
#define PTTISR_PRETTTT_INTERVAL3_OFFSET 24
#define PTTISR_PRETTTT_INTERVAL2 BITS(16,23)
#define PTTISR_PRETTTT_INTERVAL2_OFFSET 16
#define PTTISR_PRETTTT_INTERVAL1 BITS(8,15)
#define PTTISR_PRETTTT_INTERVAL1_OFFSET 8
#define PTTISR_PRETTTT_INTERVAL0 BITS(0,7)
#define PTTISR_PRETTTT_INTERVAL0_OFFSET 0


/* TT0TPCR */ //NPS Support
#define TT0TPCR_TTTT0_CAL_EN     BIT(31)
#define TT0TPCR_TTTT0_OFFSET_OF_TBTT0 BITS(0,19)

/* TT1TPCR */ //NPS Support
#define TT1TPCR_TTTT1_CAL_EN     BIT(31)
#define TT1TPCR_TTTT1_OFFSET_OF_TBTT0 BITS(0,19) //us

/* TT2TPCR */ //NPS Support
#define TT2TPCR_TTTT2_CAL_EN     BIT(31)
#define TT2TPCR_TTTT2_OFFSET_OF_TBTT0 BITS(0,19)

/* TT3TPCR */ //NPS Support
#define TT3TPCR_TTTT3_CAL_EN     BIT(31)
#define TT3TPCR_TTTT3_OFFSET_OF_TBTT0 BITS(0,19)

/* FMSCR0 */ //NPS Support FMS
#define FMSCR0_FMSID7_EN      BIT(23)
#define FMSCR0_FMSID6_EN      BIT(22)
#define FMSCR0_FMSID5_EN      BIT(21)
#define FMSCR0_FMSID4_EN      BIT(20)
#define FMSCR0_FMSID3_EN      BIT(19)
#define FMSCR0_FMSID2_EN      BIT(18)
#define FMSCR0_FMSID1_EN      BIT(17)
#define FMSCR0_FMSID0_EN      BIT(16)
#define FMSCR0_FMSCNT3_SET    BIT(7)
#define FMSCR0_FMSCNT2_SET    BIT(6)
#define FMSCR0_FMSCNT1_SET    BIT(5)
#define FMSCR0_FMSCNT0_SET    BIT(4)
#define FMSCR0_FMSCNT3_EN     BIT(3)
#define FMSCR0_FMSCNT2_EN     BIT(2)
#define FMSCR0_FMSCNT1_EN     BIT(1)
#define FMSCR0_FMSCNT0_EN     BIT(0)

/* FMSCNTCR0 */ //NPS Support FMS
#define FMSCNTCR0_FMSCNT1_PERIOD BITS(26,30)
#define FMSCNTCR0_FMSCNT1_CNT    BITS(21,25)
#define FMSCNTCR0_FMSCNT1_CNTID  BITS(18,20)
#define FMSCNTCR0_FMSCNT1_BSSID  BITS(16,17)
#define FMSCNTCR0_FMSCNT0_PERIOD BITS(10,14)
#define FMSCNTCR0_FMSCNT0_CNT    BITS(5,9)
#define FMSCNTCR0_FMSCNT0_CNTID  BITS(2,4)
#define FMSCNTCR0_FMSCNT0_BSSID  BITS(0,1)

/* FMSCNTCR1 */ //NPS Support FMS
#define FMSCNTCR1_FMSCNT3_PERIOD BITS(26,30)
#define FMSCNTCR1_FMSCNT3_CNT    BITS(21,25)
#define FMSCNTCR1_FMSCNT3_CNTID  BITS(18,20)
#define FMSCNTCR1_FMSCNT3_BSSID  BITS(16,17)
#define FMSCNTCR1_FMSCNT2_PERIOD BITS(10,14)
#define FMSCNTCR1_FMSCNT2_CNT    BITS(5,9)
#define FMSCNTCR1_FMSCNT2_CNTID  BITS(2,4)
#define FMSCNTCR1_FMSCNT2_BSSID  BITS(0,1)

/* FMSIDCR0 */ //NPS Support FMS
#define FMSIDCR0_FMSID1_SID      BITS(24,31)
#define FMSIDCR0_FMSCNT1_CNTID   BITS(18,20)
#define FMSIDCR0_FMSCNT1_BSSID   BITS(16,17)
#define FMSIDCR0_FMSID0_SID      BITS(8,15)
#define FMSIDCR0_FMSCNT0_CNTID   BITS(2,4)
#define FMSIDCR0_FMSCNT0_BSSID   BITS(0,1)

/* FMSIDCR1 */ //NPS Support FMS
#define FMSIDCR1_FMSID3_SID      BITS(24,31)
#define FMSIDCR1_FMSCNT3_CNTID   BITS(18,20)
#define FMSIDCR1_FMSCNT3_BSSID   BITS(16,17)
#define FMSIDCR1_FMSID2_SID      BITS(8,15)
#define FMSIDCR1_FMSCNT2_CNTID   BITS(2,4)
#define FMSIDCR1_FMSCNT2_BSSID   BITS(0,1)

/* FMSIDCR2 */ //NPS Support FMS
#define FMSIDCR2_FMSID5_SID      BITS(24,31)
#define FMSIDCR2_FMSCNT5_CNTID   BITS(18,20)
#define FMSIDCR2_FMSCNT5_BSSID   BITS(16,17)
#define FMSIDCR2_FMSID4_SID      BITS(8,15)
#define FMSIDCR2_FMSCNT4_CNTID   BITS(2,4)
#define FMSIDCR2_FMSCNT4_BSSID   BITS(0,1)

/* FMSIDCR3 */ //NPS Support FMS
#define FMSIDCR3_FMSID7_SID      BITS(24,31)
#define FMSIDCR3_FMSCNT7_CNTID   BITS(18,20)
#define FMSIDCR3_FMSCNT7_BSSID   BITS(16,17)
#define FMSIDCR3_FMSID6_SID      BITS(8,15)
#define FMSIDCR3_FMSCNT6_CNTID   BITS(2,4)
#define FMSIDCR3_FMSCNT6_BSSID   BITS(0,1)

/* LGRXBCNCR0 */
#define LGRXBCNCR0_LGRX_BCN_RCV_CNT    BITS(24,31)
#define LGRXBCNCR0_LGRX_BCN_LOST_CNT   BITS(16,23)
//#define LGXBCN_RCV_COUNT_MASK        BITS(24,31)
//#define LGXBCN_LOST_COUNT_MASK       BITS(16,23)
#define LGRXBCNCR0_LGRX_BCN_HWCTRL_EN  BIT(1)
#define LGRXBCNCR0_LGRX_BCN_EN         BIT(0)
#define LGXBCN_RCV_COUNT_OFFSET        24
#define LGXBCN_LOST_COUNT_OFFSET       16

/* LGRXBCNCR1 */
#define LGRXBCNCR1_LGRX_BCN_RCPI_THR1  BITS(16,25)
#define LGRXBCNCR1_LGRX_BCN_RCPI_THR0  BITS(0,9)
//#define LGXBCN_RCPI_THR0_MASK        BITS(0,9)
//#define LGXBCN_RCPI_THR1_MASK        BITS(16,25)
#define LGXBCN_RCPI_THR0_OFFSET        0
#define LGXBCN_RCPI_THR1_OFFSET        16

/* LGRXBCNCR2 */
#define LGRXBCNCR2_LGRX_BCN_RCPI_THR3  BITS(16,25)
#define LGRXBCNCR2_LGRX_BCN_RCPI_THR2  BITS(0,9)
//#define LGXBCN_RCPI_THR2_MASK        BITS(0,9)
//#define LGXBCN_RCPI_THR3_MASK        BITS(16,25)
#define LGXBCN_RCPI_THR2_OFFSET        0
#define LGXBCN_RCPI_THR3_OFFSET        16

/* WNMSLPCR */ //NPS Support WNM Sleep
#define WNMSLPCR_                BIT(31)
#define WNMSLPCR_WNM_SLP_BSSID   BITS(16,17)
#define WNMSLPCR_WNM_SLP_PERIOD  BITS(0,15)

/* WLANCKCALCR */
#define WLANCKCALCR_WLAN_CLK_CAL_OFST2  BITS(5,8)
#define WLANCKCALCR_WLAN_CLK_CAL_OFST1  BITS(2,4)
#define WLANCKCALCR_WLAN_CLK_CAL_OFST0  BITS(0,1)

/* TIMSPCR */
#define TIMSPCR_TIM_BMC_SP3         BIT(27)
#define TIMSPCR_TIM_SP3             BIT(26)
#define TIMSPCR_TIM_BMC_SP2         BIT(25)
#define TIMSPCR_TIM_SP2             BIT(24)
#define TIMSPCR_TIM_BMC_SP3_RESET   BIT(23)
#define TIMSPCR_TIM_SP3_RESET       BIT(22)
#define TIMSPCR_TIM_BMC_SP2_RESET   BIT(21)
#define TIMSPCR_TIM_SP2_RESET       BIT(20)
#define TIMSPCR_TIM_BMC_SP3_ENABLE  BIT(19)
#define TIMSPCR_TIM_SP3_ENABLE      BIT(18)
#define TIMSPCR_TIM_BMC_SP2_ENABLE  BIT(17)
#define TIMSPCR_TIM_SP2_ENABLE      BIT(16)
#define TIMSPCR_TIM_BMC_SP1         BIT(11)
#define TIMSPCR_TIM_SP1             BIT(10)
#define TIMSPCR_TIM_BMC_SP0         BIT(9)
#define TIMSPCR_TIM_SP0             BIT(8)
#define TIMSPCR_TIM_BMC_SP1_RESET   BIT(7)
#define TIMSPCR_TIM_SP1_RESET       BIT(6)
#define TIMSPCR_TIM_BMC_SP0_RESET   BIT(5)
#define TIMSPCR_TIM_SP0_RESET       BIT(4)
#define TIMSPCR_TIM_BMC_SP1_ENABLE  BIT(3)
#define TIMSPCR_TIM_SP1_ENABLE      BIT(2)
#define TIMSPCR_TIM_BMC_SP0_ENABLE  BIT(1)
#define TIMSPCR_TIM_SP0_ENABLE      BIT(0)

/*CTBCNCR */
#define CTBCNCR_CTBCN3_WAKEUP_NEXTDTIM  BIT(3)
#define CTBCNCR_CTBCN2_WAKEUP_NEXTDTIM  BIT(2)
#define CTBCNCR_CTBCN1_WAKEUP_NEXTDTIM  BIT(1)
#define CTBCNCR_CTBCN0_WAKEUP_NEXTDTIM  BIT(0)

/* MMCR0 */
#define MMCR0_MEASUREMENT_SKIP          BIT(31)
#define MMCR0_BSS                       BIT(30)
#define MMCR0_BSS_REQUEST               BITS(26,29)
#define MMCR0_BSS_REQUEST_OFFSET        26

#define MMCR0_BYPASS_TYPE               BITS(20,21)
#define MMCR0_BYPASS_TYPE_OFFSET        20

//#define MMCR0_TSF_SEL                   BIT(17)
#define MMCR0_MEASURE_REQ0              BIT(16)
#define MMCR0_DURATION                  BITS(0,15)

/* MMCR1 */
/* Measurement 0 TSF Time. (Can select from TSF0/TSF1) */

/* MMCR2 */
#define MMCR2_MEASUREMENT_SKIP           BIT(31)
#define MMCR2_TSF_SEL                    BIT(17)
#define MMCR2_MEASURE_REQ1               BIT(16)
#define MMCR2_DURATION                   BITS(0,15)

/* MMCR3 */
/* Measurement 1 TSF Time. (Can select from TSF0/TSF1) */

/* QCCR0 */
#define QCCR0_QUIET_EN                   BIT(31)
#define QCCR0_QUIET_SRC_SEL_MASK         BITS(29,30)
#define QCCR0_QUIET_SRC_SEL_OFFSET       29
#define QCCR0_TSF_TIME                   BITS(0,15)

/* QCCR1 */
#define QCCR1_DURATION                   BITS(16,31)
#define QCCR1_ACTIVE_PERIOD              BITS(0,7)

/* LP_TT0TPR */
#define TT0TPR_TIM_PERIOD                BITS(0,23) //nBI
/* LP_TT1TPR */
#define TT1TPR_TIM_PERIOD                BITS(0,23) //nBI
/* LP_TT2TPR */
#define TT2TPR_TIM_PERIOD                BITS(0,23) //nBI
/* LP_TT3TPR */
#define TT3TPR_TIM_PERIOD                BITS(0,23) //nBI

/* LP_LDTIMCR */
#define LDTIMCR_HW_DTIM_CNT3             BITS(24,31)
#define LDTIMCR_HW_DTIM_CNT3_OFFSET      24
#define LDTIMCR_HW_DTIM_CNT2             BITS(16,23)
#define LDTIMCR_HW_DTIM_CNT2_OFFSET      16
#define LDTIMCR_HW_DTIM_CNT1             BITS(8,15)
#define LDTIMCR_HW_DTIM_CNT1_OFFSET      8
#define LDTIMCR_HW_DTIM_CNT0             BITS(0,7)
#define LDTIMCR_HW_DTIM_CNT0_OFFSET      0

/* LP_LMTBDTCR */
#define LMTBDTCR_HW_TBTT_WAKEUP_CNT4                BITS(27,30)
#define LMTBDTCR_HW_TBTT_WAKEUP_CNT4_OFFSET         27
#define LMTBDTCR_HW_DTIM_WAKEUP_CNT3                BITS(24,26)
#define LMTBDTCR_HW_DTIM_WAKEUP_CNT3_OFFSET         24
#define LMTBDTCR_HW_TBTT_WAKEUP_CNT2                BITS(19,22)
#define LMTBDTCR_HW_TBTT_WAKEUP_CNT2_OFFSET         19
#define LMTBDTCR_HW_DTIM_WAKEUP_CNT2                BITS(16,18)
#define LMTBDTCR_HW_DTIM_WAKEUP_CNT2_OFFSET         16
#define LMTBDTCR_HW_TBTT_WAKEUP_CNT1                BITS(11,14)
#define LMTBDTCR_HW_TBTT_WAKEUP_CNT1_OFFSET         11
#define LMTBDTCR_HW_DTIM_WAKEUP_CNT1                BITS(8,10)
#define LMTBDTCR_HW_DTIM_WAKEUP_CNT1_OFFSET         8
#define LMTBDTCR_HW_TBTT_WAKEUP_CNT0                BITS(3,6)
#define LMTBDTCR_HW_TBTT_WAKEUP_CNT0_OFFSET         3
#define LMTBDTCR_HW_DTIM_WAKEUP_CNT0                BITS(0,2)
#define LMTBDTCR_HW_DTIM_WAKEUP_CNT0_OFFSET         0

/* LP_LFBCR0 */
#define LFBCR0_LP_FULL_BCN3_EN           BIT(29)
#define LFBCR0_LP_FULL_BCN3_EN_OFFSET    29
#define LFBCR0_FB_WAKE_CNT3              BITS(24,28)
#define LFBCR0_FB_WAKE_CNT3_OFFSET       24
#define LFBCR0_LP_FULL_BCN2_EN           BIT(21)
#define LFBCR0_LP_FULL_BCN2_EN_OFFSET    21
#define LFBCR0_FB_WAKE_CNT2              BITS(16,20)
#define LFBCR0_FB_WAKE_CNT2_OFFSET       16
#define LFBCR0_LP_FULL_BCN1_EN           BIT(13)
#define LFBCR0_LP_FULL_BCN1_EN_OFFSET    13
#define LFBCR0_FB_WAKE_CNT1              BITS(8,12)
#define LFBCR0_FB_WAKE_CNT1_OFFSET       8
#define LFBCR0_LP_FULL_BCN0_EN           BIT(5)
#define LFBCR0_LP_FULL_BCN0_EN_OFFSET    5
#define LFBCR0_FB_WAKE_CNT0              BITS(0,4)
#define LFBCR0_FB_WAKE_CNT0_OFFSET       0

//LP_PBCR0
#define PBCR0_DTIM_UPDATE_CHK            BIT(31)
#define PBCR0_WAKE_CNT3_RST              BIT(28)
#define PBCR0_WAKE_CNT2_RST              BIT(27)
#define PBCR0_WAKE_CNT1_RST              BIT(26)
#define PBCR0_WAKE_CNT0_RST              BIT(25)
#define PBCR0_LP_PARTIAL_BCN3_EN         BIT(24)
#define PBCR0_LP_PARTIAL_BCN2_EN         BIT(23)
#define PBCR0_LP_PARTIAL_BCN1_EN         BIT(22)
#define PBCR0_LP_PARTIAL_BCN0_EN         BIT(21)
#define PBCR0_PB_WAKE_PERIOD_RXFB        BITS(16,20)
#define PBCR0_PB_WAKE_PERIOD_RXFB_OFFSET 16
#define PBCR0_PB_WAKEUP_MODE             BIT(15)
//#define PBCR0_PB_UPDATE_DTIM             BITS(8,11)
#define PBCR0_PB_UPDATE_DTIM_BSSID3         BIT(11)
#define PBCR0_PB_UPDATE_DTIM_BSSID3_OFFSET  11
#define PBCR0_PB_UPDATE_DTIM_BSSID2         BIT(10)
#define PBCR0_PB_UPDATE_DTIM_BSSID2_OFFSET  10
#define PBCR0_PB_UPDATE_DTIM_BSSID1         BIT(9)
#define PBCR0_PB_UPDATE_DTIM_BSSID1_OFFSET  9
#define PBCR0_PB_UPDATE_DTIM_BSSID0         BIT(8)
#define PBCR0_PB_UPDATE_DTIM_BSSID0_OFFSET  8


//#define PBCR0_PB_UPDATE_TSF              BITS(4,7)
#define PBCR0_PB_UPDATE_TSF_BSSID3          BIT(7)
#define PBCR0_PB_UPDATE_TSF_BSSID3_OFFSET   7
#define PBCR0_PB_UPDATE_TSF_BSSID2          BIT(6)
#define PBCR0_PB_UPDATE_TSF_BSSID2_OFFSET   6
#define PBCR0_PB_UPDATE_TSF_BSSID1          BIT(5)
#define PBCR0_PB_UPDATE_TSF_BSSID1_OFFSET   5
#define PBCR0_PB_UPDATE_TSF_BSSID0          BIT(4)
#define PBCR0_PB_UPDATE_TSF_BSSID0_OFFSET   4


#define PBCR0_PB_EN_BSSID3               BIT(3)
#define PBCR0_PB_EN_BSSID3_OFFSET        3
#define PBCR0_PB_EN_BSSID2               BIT(2)
#define PBCR0_PB_EN_BSSID2_OFFSET        2
#define PBCR0_PB_EN_BSSID1               BIT(1)
#define PBCR0_PB_EN_BSSID1_OFFSET        1
#define PBCR0_PB_EN_BSSID0               BIT(0)
#define PBCR0_PB_EN_BSSID0_OFFSET        0



//LP_NANCR0
#define NANCR0_2G_DW_END_INT_TRIG        BIT(30)
#define NANCR0_5G_DW_END_INT_TRIG        BIT(29)

#define NANCR0_NAN_DW_LEN                BITS(8,12)
#define NANCR0_NAN_DW_LEN_OFFSET         8
#define NANCR0_NAN_5G_DW                 BIT(3)
#define NANCR0_NAN_5G_DW_OFFSET          3
#define NANCR0_NAN_2P4G_DW               BIT(2)
#define NANCR0_NAN_2P4G_DW_OFFSET        2

#define NANCR0_NAN_BAND_EN_SEL           BITS(0,1)


/***** LP AON *****/

/* LPCR */
#define LPCR_MAN_WF_WLAN_ON              BIT(26)
#define LPCR_MAN_EN_WF_WLAN_ON           BIT(25)
#define LPCR_STS_WF_WLAN_ON	             BIT(24)

#define LPCR_LP_OWN_STAT	             BIT(23)
#define LPCR_OSC_STAT	                 BIT(22)
#define LPCR_WPLL_STAT	                 BIT(21)
#define LPCR_OSC_WPLL_SEL	             BIT(20)

#define LPCR_MAN_WF_DCLK_HCLK_SEL        BIT(19)
#define LPCR_MAN_WF_PLL_CKEN	         BIT(18)
#define LPCR_MAN_WF_AWAKE_2BT	         BIT(17)
#define LPCR_MAN_WF_SX_ON	             BIT(16)
#define LPCR_MAN_WF_DIGCK_EN	         BIT(15)
#define LPCR_MAN_WF_DSLEEP_REQ_B         BIT(14)

#define LPCR_MAN_EN_WF_DCLK_HCLK_SEL     BIT(12)
#define LPCR_MAN_EN_WF_PLL_CKEN          BIT(11)
#define LPCR_MAN_EN_WF_AWAKE_2BT	     BIT(10)
#define LPCR_MAN_EN_WF_SX_ON	         BIT(9)
#define LPCR_MAN_EN_WF_DIGCK_EN          BIT(8)
#define LPCR_MAN_EN_WF_DSLEEP_REQ_B	     BIT(7)

#define LPCR_STS_DCLK_HCLK_SEL	         BIT(6)
#define LPCR_STS_WPLL_RDY	             BIT(5)
#define LPCR_STS_WF_PLL_CKEN	         BIT(4)
#define LPCR_STS_WF_AWAKE_2BT	         BIT(3)
#define LPCR_STS_WF_SX_ON	             BIT(2)
#define LPCR_STS_WF_DIGCK_EN	         BIT(1)
#define LPCR_STS_OSC_RDY                 BIT(0)

/* LPCR_EN */
#define LPCR_EN_WFAON_SW_RST   	         BIT(31)
#define LPCR_EN_WEN_MAN_WF_WLAN_ON	     BIT(26)
#define LPCR_EN_WEN_MAN_EN_WF_WLAN_ON	 BIT(25)
#define LPCR_EN_WEN_SW_WF_WLAN_ON        BIT(24)

#define LPCR_EN_LP_OWN_EN	             BIT(23)

#define LPCR_EN_WEN_OSC_STAT	         BIT(22) //FPGA only
#define LPCR_EN_WEN_WPLL_STAT	         BIT(21) //FPGA only
#define LPCR_EN_WEN_OSC_WPLL_SEL	     BIT(20)

#define LPCR_EN_WEN_MAN_DCLK_HCLK_SEL    BIT(19)
#define LPCR_EN_WEN_MAN_WF_PLL_CKEN      BIT(18)
#define LPCR_EN_WEN_MAN_WF_AWAKE_2BT     BIT(17)
#define LPCR_EN_WEN_MAN_WF_SX_ON	     BIT(16)
#define LPCR_EN_WEN_MAN_WF_DIGCK_EN      BIT(15)
#define LPCR_EN_WEN_MAN_WF_DSLEEP_REQ_B  BIT(14)


#define LPCR_EN_WEN_MAN_EN_DCLK_HCLK_SEL BIT(12)
#define LPCR_EN_WEN_MAN_EN_WF_PLL_CKEN   BIT(11)
#define LPCR_EN_WEN_MAN_EN_WF_AWAKE_2BT  BIT(10)
#define LPCR_EN_WEN_MAN_EN_WF_SX_ON	     BIT(9)
#define LPCR_EN_WEN_MAN_EN_WF_DIGCK_EN	 BIT(8)
#define LPCR_EN_WEN_MAN_EN_WF_DSLEEP_REQ_B BIT(7)

#define LPCR_EN_WEN_SW_DCLK_HCLK_SEL     BIT(6)
#define LPCR_EN_WEN_SW_WF_PLL_CKEN       BIT(4)
#define LPCR_EN_WEN_SW_WF_AWAKE_2BT	     BIT(3)
#define LPCR_EN_WEN_SW_WF_SX_ON	         BIT(2)
#define LPCR_EN_WEN_SW_WF_DIGCK_EN	     BIT(1)
#define LPCR_EN_WEN_SW_WF_DSLEEP_REQ_B   BIT(0)

/* LPCR_DIS */
#define LPCR_DIS_WDIS_MAN_WF_WLAN_ON     BIT(26)
#define LPCR_DIS_WDIS_MAN_EN_WF_WLAN_ON  BIT(25)
#define LPCR_DIS_WDIS_SW_WF_WLAN_ON      BIT(24)

#define LPCR_DIS_LP_OWN_DIS              BIT(23)
#define LPCR_DIS_WDIS_OSC_STAT           BIT(22)
#define LPCR_DIS_WDIS_WPLL_STAT          BIT(21)
#define LPCR_DIS_WDIS_OSC_WPLL_SEL       BIT(20)

#define LPCR_DIS_WDIS_MAN_DCLK_HCLK_SEL	 BIT(19)
#define LPCR_DIS_WDIS_MAN_WF_PLL_CKEN	 BIT(18)
#define LPCR_DIS_WDIS_MAN_WF_AWAKE_2BT	 BIT(17)
#define LPCR_DIS_WDIS_MAN_WF_SX_ON	     BIT(16)
#define LPCR_DIS_WDIS_MAN_WF_DIGCK_EN    BIT(15)
#define LPCR_DIS_WDIS_MAN_WF_DSLEEP_REQ_B BIT(14)

#define LPCR_DIS_WDIS_MAN_EN_DCLK_HCLK_SEL BIT(12)
#define LPCR_DIS_WDIS_MAN_EN_WF_PLL_CKEN BIT(11)
#define LPCR_DIS_WDIS_MAN_EN_WF_AWAKE_2BT BIT(10)
#define LPCR_DIS_WDIS_MAN_EN_WF_SX_ON	 BIT(9)
#define LPCR_DIS_WDIS_MAN_EN_WF_DIGCK_EN BIT(8)
#define LPCR_DIS_WDIS_MAN_EN_WF_DSLEEP_REQ_B BIT(7)

#define LPCR_DIS_WDIS_SW_DCLK_HCLK_SEL   BIT(6)
#define LPCR_DIS_WDIS_SW_WF_PLL_CKEN     BIT(4)
#define LPCR_DIS_WDIS_SW_WF_AWAKE_2BT	 BIT(3)
#define LPCR_DIS_WDIS_SW_WF_SX_ON	     BIT(2)
#define LPCR_DIS_WDIS_SW_WF_DIGCK_EN	 BIT(1)
#define LPCR_DIS_WDIS_SW_WF_DSLEEP_REQ_B BIT(0)

/* MTR0 */
#define MTR0_THERM_ADMIT_TIME               BITS(16,31)
#define MTR0_THERM_ADMIT_TIME_OFFSET        16
#define MTR0_THERM_USED_TIME                BITS(0,15)
#define MTR0_THERM_USED_TIME_OFFSET         0

/* MTR1 */
#define MTR1_THERM_AVERAGE_PERIOD           BITS(8,14)
#define MTR1_THERM_AVERAGE_PERIOD_OFFSET    8
#define MTR1_THERM_MT_UNIT_64_US            BIT(1)
#define MTR1_THERM_ADMIT_TIME_EN            BIT(0)

/* 32KCCR */ //Move to LP_AON
#define _32KCCR_TUNING_10000                BITS(28,31)
#define _32KCCR_TUNING_10000_OFFSET         28
#define _32KCCR_TUNING_1000                 BITS(24,27)
#define _32KCCR_TUNING_1000_OFFSET          24
#define _32KCCR_TUNING_100                  BITS(20,23)
#define _32KCCR_TUNING_100_OFFSET           20
#define _32KCCR_TUNING_10                   BITS(16,19)
#define _32KCCR_TUNING_10_OFFSET            16
#define _32KCCR_SLOW_CNT                    BITS(0,5)
#define _32KCCR_SLOW_CNT_OFFSET             0

/* SMCTL0 */ // Move to WF_AON fro MT6630
#define SMCTL0_ANA_OP_PHASE_OFFSET          BITS(16,25)
#define SMCTL0_ANA_OP_PHASE_OFFSET_OFFSET   16
#define SMCTL0_DCLK_HCLK_SEL_EN             BIT(7)
#define SMCTL0_SRAM_PD_CTL_BY_HW            BIT(6)
#define SMCTL0_MTCMOS_PD_BY_HW              BIT(5)
#define SMCTL0_PSE_SRAM_PON_WLAN_ON_EN      BIT(4)
#define SMCTL0_BUSY_WAIT_CNT                BITS(0,3)

/* SMCTL1 */ // Move to WF_AON fro MT6630
#define SMCTL1_OSC_STABLE_TIME              BITS(16,27)
#define SMCTL1_OSC_STABLE_TIME_OFFSET       16
//#define SMCTL1_VCORE_STABLE_TIME            BITS(0,11)
//#define SMCTL1_VCORE_STABLE_TIME_OFFSET     0

/* SMCTL2 */ // Move to WF_AON fro MT6630
#define SMCTL2_PLL_STABLE_TIME              BITS(16,27)
#define SMCTL2_PLL_STABLE_TIME_OFFSET       16
#define SMCTL2_WIFI_PON_TIME                BITS(0,11)
#define SMCTL2_WIFI_PON_TIME_OFFSET         0

/* SMCTL3 */// Move to WF_AON fro MT6630
#define SMCTL3_WF_SX_USE_2P4G_EN            BIT(31)
#define SMCTL3_WF_SX_USE_2P4G_EN_OFFSET     31
#define SMCTL3_WF_DIGCK_ON_TIME             BITS(16,27)
#define SMCTL3_WF_DIGCK_ON_TIME_OFFSET      16
#define SMCTL3_RS_BY_CMDBT_TIME             BITS(0,11)
#define SMCTL3_RS_BY_CMDBT_TIME_OFFSET      0

/* SMCTL4 */ // Move to WF_AON fro MT6630
#define SMCTL4_WIFI_PWR_OFF_EVENT_CLR      BIT(1)
#define SMCTL4_WIFI_PWR_OFF_EVENT          BIT(0)

/* SMCTL5 */ // Move to WF_AON fro MT6630
#define SMCTL5_WAKEUP_DURATION             BITS(16,31)
#define SMCTL5_WAKEUP_DURATION_OFFSET      16
#define SMCTL5_PRE_WAKEUP_OFFSET           BITS(0,9)
#define SMCTL5_PRE_WAKEUP_OFFSET_OFFSET    0

/* SMCTL6 */ // Move to WF_AON fro MT6630
#define SMCTL6_REG_RESTORE_TIME            BITS(16,31)
#define SMCTL6_REG_RESTORE_TIME_OFFSET     16
#define SMCTL6_RF_TURN_ON_DURATION         BITS(0,9)
#define SMCTL6_RF_TURN_ON_DURATION_OFFSET  0

/* SMCTL7 */ // Move to WF_AON fro MT6630
#define SMCTL7_WF_SX_OFF_GUARD_TIME        BITS(16,25)
#define SMCTL7_WF_SX_OFF_GUARD_TIME_OFFSET 16
#define SMCTL7_WF_SX_ON_TIME               BITS(0,9)
#define SMCTL7_WF_SX_ON_TIME_OFFSET        0

/* LP_WLANCKCR */
#define WLAN_CLK_MODE_2                    BITS(24,31)
#define WLAN_CLK_MODE_2_OFFSET             24
#define WLAN_CLK_MODE_1                    BITS(16,23)
#define WLAN_CLK_MODE_1_OFFSET             16
#define WLAN_CLK_MODE_0                    BITS(8,15)
#define WLAN_CLK_MODE_0_OFFSET             8
#define WLAN_CLK_MODE_SEL                  BITS(0,1)
#define WLAN_CLK_MODE_SEL_OFFSET           0


/***** CONFG *****/
/* CCR */
#define CCR_SIF_SET                     BIT(26)
#define CCR_PTA_LOGIC_GC_DIS                    BIT(15)
#define CCR_OSC_STAT                    BIT(14)
#define CCR_OSC_SEL                     BIT(13)
#define CCR_LPOFF_LOGIC_GC_DIS                BIT(12)
#define CCR_DMA_AHB_LOGIC_GC_DIS        BIT(11)
#define CCR_WTBL_LOGIC_GC_DIS           BIT(10)
#define CCR_RMAC_LOGIC_GC_DIS           BIT(9)
#define CCR_AGG_LOGIC_GC_DIS            BIT(8)
#define CCR_CMDBT_LOGIC_GC_DIS          BIT(7)
#define CCR_DBUF_LOGIC_GC_DIS           BIT(6)
#define CCR_LP_LOGIC_GC_DIS             BIT(5)
#define CCR_TMAC_LOGIC_GC_DIS           BIT(4)
#define CCR_SEC_LOGIC_GC_DIS            BIT(3)
#define CCR_DMA_MAC_GC_DIS              BIT(2)
#define CCR_ARB_LOGIC_GC_DIS            BIT(1)
#define CCR_MCR_GC_DIS                  BIT(0)

/* CFG_TOP SWRST_EN */
#define SWRST_EN_MACON_LOGRST           BIT(2)
#define SWRST_EN_MACON_REGRST           BIT(1)
#define SWRST_EN_MBIST_REGRST           BIT(0)

/* CFG_OFF WO_SWRST_EN */
#define WO_SWRST_EN_MACOFFBB_LOGRST     BIT(2)
#define WO_SWRST_EN_MACOFF_REGRST       BIT(1)
#define WO_SWRST_EN_BB_REGRST           BIT(0)

/* SDCR */
#define SDCR_SP4B_DEL_SEL               BITS(4,7)
#define SDCR_SP2B_DEL_SEL               BITS(2,3)
#define SDCR_TP2B_DEL_SEL               BITS(0,1)

/* SRAM_PWR_EN */
#define SRAM_PWR_CTRL_EN            BITS(0,15)

/* SRAM_SLEEP_EN */
#define SRAM_SLEEP_CTRL_EN          BITS(0,15)

/* TCM_PWR_EN */
#define TCM_PWR_CTRL_EN             BITS(0,31)
#define ILM_PWR_CTRL_EN             BITS(0, 11)
#define ILM_PWR_EN_OFFSET                     0
#define DLM_PWR_CTRL_EN             BITS(0, 7)
#define DLM_PWR_EN_OFFSET                     12

/* TCM_SLEEP_EN */
#define TCM_SLEEP_CTRL_EN           BITS(0,31)
#define ILM_SLEEP_CTRL_EN             BITS(0, 11)
#define ILM_SLEEP_EN_OFFSET                     0
#define DLM_SLEEP_CTRL_EN             BITS(0, 7)
#define DLM_SLEEP_EN_OFFSET                     12

#define SRAM_ALL_SUBBANK                    BITS(0, 2)

/* DCR */
#define DCR_WDT_EN                      BIT(25)
#define DCR_DEBUG_EN                    BIT(24)
#define DCR_DEBUG_SEL                   BITS(16,23)
#define DCR_SLEEP_TIMEOUT_UNIT          BIT(15)
#define DCR_SLEEP_TIMEOUT_COUNT         BITS(0,14)


/* PLL_CR */
#define PLL_CR_PLL_SETTINGS                BITS(0,9)
#define PLL_CR_PLL_DIV                BITS(4,9)
#define PLL_CR_PLL_CKSEL                BITS(2,3)
#define PLL_CR_PLL_BAND                BITS(0,1)

/***** CMDBT *****/
#define CMDBT_TRG_RESET                 BIT(2)
#define CMDBT_TRG_TRIGGER               BIT(1)
#define CMDBT_TRG_TERMINATE             BIT(0)

#define CMDBT_CTL_EXT_MEM_ADDR_PREFIX   BITS(19,31) //PSE
#define CMDBT_CTL_EXT_MEM_ADDR_PREFIX_OFFSET 19
#define CMDBT_CTL_DLM_DIS               BIT(2)
#define CMDBT_CTL_AHB_DIS               BIT(1)
#define CMDBT_CTL_MEM_WR_DIS            BIT(0)

#define CMDBT_STS_CARRY                 BIT(24)
#define CMDBT_STS_FIFO_RDY              BIT(16)
#define CMDBT_STS_FIFO_FULL             BIT(9)
#define CMDBT_STS_FIFO_EMPTY            BIT(8)
#define CMDBT_STS_BUSY                  BIT(4)
#define CMDBT_STS_ST                    BITS(0,1) //Exec's state machine for debug

/***** THERMAL *****/

/* THERMCR0 */
#define THERMCR0_THERM_MEASURE_CNT_MASK         BITS(24,28)
#define THERMCR0_THERM_MEASURE_CNT_OFFSET       24
#define THERMCR0_THERM_CAL_EN           BIT(19)
#define THERMCR0_THERM_TRIGGER          BIT(18)
#define THERMCR0_THERM_BUSY             BIT(16)
#define THERMCR0_THERM_CAL_VAL_MASK     BITS(8,14)
#define THERMCR0_THERM_CAL_VAL_OFFSET   8
#define THERMCR0_THERM_RAW_VAL_MASK     BITS(0,6)

/* THERMCR1 */
#define THERMCR1_THERM_2ND_MAX_MASK     BITS(24,30)
#define THERMCR1_THERM_2ND_MAX_OFFSET   24
#define THERMCR1_THERM_AR_FACTOR_MASK   BITS(16,17)
#define THERMCR1_THERM_AR_FACTOR_OFFSET 16
#define THERMCR1_THERM_MIN_MASK         BITS(8,14)
#define THERMCR1_THERM_MIN_OFFSET       8
#define THERMCR1_THERM_MAX_MASK         BITS(0,6)
#define THERMCR1_THERM_MAX_OFFSET       0

/* THERMCR2 */
#define THERMCR2_THERM_STDBY_CNT_MASK   BITS(16,21)
#define THERMCR2_THERM_STDBY_CNT_OFFSET 16
#define THERMCR2_RG_THAD_CLK_DIV_16     BIT(11)


/* TSTCS */
#define TSTCS_TEST_MODE_EN              BIT(31)
#define TSTCS_TEST_MODE_ADCOUNT_DONE    BIT(0)

/* TSTDR */
#define TSTDR_AD_THADCOUT_NASK          BITS(0,5)

/* DUMMY */
#define THERMAL_DUMMY1_MASK             BITS(16,31)
#define THERMAL_DUMMY1_OFFSET           16
#define THERMAL_DUMMY0_MASK             BITS(0,15)

/* THERMCR3 */
#define THERMCR3_THERM_MAX_INT_EN       BIT(24)
#define THERMCR3_THERM_SCND_MAX_INT_EN  BIT(20)
#define THERMCR3_THERM_MIN_INT_EN       BIT(16)
#define THERMCR3_UPDATE_TIME_MASK       BITS(0,5)
#define THERMCR3_UPDATE_TIME_MAX        63

/* THERMISR */
#define THERMISR_THERM_MAX_INT          BIT(24)
#define THERMISR_THERM_SCND_MAX_INT     BIT(20)
#define THERMISR_THERM_MIN_INT          BIT(16)


/***** VOLTAGE *****/

/* SARCR0 */
#define SARCR0_CR_SAR_EN                BIT(10)
#define SARCR0_CR_ST                    BIT(9)

/* SARCR1 */
#define SARCR1_CR_INT_EN                BIT(31)
#define SARCR1_CR_INT_EN_OFFSET         31
#define SARCR1_CR_THH_MASK              BITS(16,27)
#define SARCR1_CR_THH_OFFSET            16
#define SARCR1_CR_INT_PERIOD_MASK       BITS(12,13)
#define SARCR1_CR_INT_PERIOD_OFFSET     12
#define SARCR1_CR_THL_MASK              BITS(0,11)

/* SARCR2 */
#define SARCR2_ADC_INTERRUPT_B          BIT(14)
#define SARCR2_ADC_INTERRUPT_B_OFFSET   14
#define SARCR2_ADC_INT_STATE            BIT(13)
#define SARCR2_ADC_INT_STATE_OFFSET     13
#define SARCR2_ADC_LATCH_MASK           BITS(0,11)


/***** DMA *****/
/* DCR0 */
#define DCR0_ETBL7_EN                   BIT(31)
#define DCR0_ETBL6_EN                   BIT(30)
#define DCR0_ETBL5_EN                   BIT(29)
#define DCR0_ETBL4_EN                   BIT(28)
#define DCR0_ETBL3_EN                   BIT(27)
#define DCR0_ETBL2_EN                   BIT(26)
#define DCR0_ETBL1_EN                   BIT(25)
#define DCR0_ETBL0_EN                   BIT(24)
#define DCR0_RX_HDR_TRANS_MODE          BIT(23)
#define DCR0_RM_VLAN                    BIT(22)
#define DCR0_INS_VLAN                   BIT(21)
#define DCR0_PCP_EQ_TID                 BIT(20)
#define DCR0_RX_HDR_TRANS_EN            BIT(19)
#define DCR0_RX_HDR_TRANS_CHK_BSSID     BIT(18)
#define DCR0_VEC_DROP_EN                BIT(17)
#define DCR0_DAMSDU_EN                  BIT(16)
#define DCR0_MAX_RX_PKT_LENGTH_MASK     BITS(2,15)
#define DCR0_MAX_RX_PKT_LENGTH_OFFSET   2

/* DCR1 */
#define DCR1_RFB2M_GROUP1_EN            BIT(11)
#define DCR1_RFB2M_GROUP2_EN            BIT(12)
#define DCR1_RFB2M_GROUP3_EN            BIT(13)
#define DCR1_RFB2M_GROUPS_EN_MASK       BITS(11,13)
#define DCR1_RFB2M_GROUPS_EN_OFFSET     11
#define DCR1_TXDSCR_PAD_MASK           BITS(8,10)
#define DCR1_TXDSCR_PAD_OFFSET         8

/* FQCR0 */
#define FQCR0_FQ_EN                     BIT(31)
#define FQCR0_FQ_STS                    BIT(30)
#define FQCR0_FQ_MODE_OMAC_IDX          BIT(29)
#define FQCR0_FQ_DEST_QID_MASK          BITS(24,28)
#define FQCR0_FQ_DEST_QID_OFFSET        24
#define FQCR0_FQ_DEST_PID_MASK          BITS(22,23)
#define FQCR0_FQ_DEST_PID_OFFSET        22
#define FQCR0_FQ_TARG_QID_MASK          BITS(16,20)
#define FQCR0_FQ_TARG_QID_OFFSET        16
#define FQCR0_FQ_TARG_OM_MASK           BITS(8,13)
#define FQCR0_FQ_TARG_OM_OFFSET         8
#define FQCR0_FQ_TARG_WIDX_MASK         BITS(0,7)
#define FQCR0_FQ_TARG_WIDX_OFFSET       0

/* FQCR1 */
#define FQCR1_FQ_FID_CNT_MASK          BITS(24,31)
#define FQCR1_FQ_FID_CNT_OFFSET        24
#define FQCR1_FQ_TAIL_FID_MASK         BITS(12,23)
#define FQCR1_FQ_TAIL_FID_OFFSET       12
#define FQCR1_FQ_HEAD_FID_MASK         BITS(0,11)
#define FQCR1_FQ_HEAD_FID_OFFSET       0

/* VTRn */
#define VTR_BSSID00_VID_MASK           BITS(0,11)
#define VTR_BSSID00_FW_ASSIGN_VID      BIT(12)
#define VTR_BSSID00_FW_PCP_MASK        BITS(13,15)
#define VTR_BSSID00_FW_PCP_OFFSET      13
#define VTR_BSSID00_MASK               BITS(0,15)
#define VTR_BSSID00_OFFSET             0

#define VTR_BSSID01_VID_MASK           BITS(16,27)
#define VTR_BSSID01_VID_OFFSET         16
#define VTR_BSSID01_FW_ASSIGN_VID      BIT(28)
#define VTR_BSSID01_FW_PCP_MASK        BITS(29,31)
#define VTR_BSSID01_FW_PCP_OFFSET      29
#define VTR_BSSID01_MASK               BITS(16,31)
#define VTR_BSSID01_OFFSET             16

/* VTR9 */
#define VTR9_BSSID0_DUP_RFB_CTRL_MASK      BITS(25,27)
#define VTR9_BSSID0_DUP_RFB_CTRL_OFFSET    25
#define VTR9_BSSID1_DUP_RFB_CTRL_MASK      BITS(22,24)
#define VTR9_BSSID1_DUP_RFB_CTRL_OFFSET    22
#define VTR9_BSSID2_DUP_RFB_CTRL_MASK      BITS(19,21)
#define VTR9_BSSID2_DUP_RFB_CTRL_OFFSET    19
#define VTR9_BSSID3_DUP_RFB_CTRL_MASK      BITS(16,18)
#define VTR9_BSSID3_DUP_RFB_CTRL_OFFSET    16

#define DIS_DUP_RFB_INVALID_WLAN_IDX       BIT(2)
#define DIS_DUP_RFB_MGMT_TYPE              BIT(1)
#define DIS_DUP_RFB_DATA_TYPE              BIT(0)


/* ETBLn */
#define ETBL_ETBL1_MASK                BITS(16,31)
#define ETBL_ETBL1_OFFSET              16
#define ETBL_ETBL0_MASK                BITS(0,15)

/* RCFR0 */
#define RCFR0_RX2M_QIDX_MASK           BITS(30,31)
#define RCFR0_RX2M_QIDX_OFFSET         30
#define RCFR0_RX2M_QIDX1               BIT(30)
#define RCFR0_RX2M_QIDX2               BIT(31)
#define RCFR0_RX2M_QIDX3               BITS(30,31)
#define RCFR0_RX2H_QIDX_MASK           BIT(29)
#define RCFR0_RX2H_QIDX_OFFSET         29
#define RCFR0_RX_DPPKT2M_MASK          BITS(24, 27)
#define RCFR0_RX_PKT2M_MASK            BITS(0,22)

#define RCFR0_BMCDP2M_MASK             BITS(26, 27)
#define RCFR0_BMCDP2M_OFFSET           26
#define RCFR0_UCDP2M_MASK              BITS(24, 25)
#define RCFR0_UCDP2M_OFFSET            24
#define RCFR0_EL2M                     BIT(22)
#define RCFR0_BP2M                     BIT(21)
#define RCFR0_MGC2M                    BIT(20)
#define RCFR0_TDLS2M                   BIT(19)
#define RCFR0_ARP_NS2M                 BIT(18)
#define RCFR0_EAPOL2M                  BIT(17)
#define RCFR0_OFLD2M_MASK              BITS(17,19)
#define RCFR0_WOL2M                    BIT(16)
#define RCFR0_CLSF2M_MASK              BITS(6,15)
#define RCFR0_FRAG2M                   BIT(5)
#define RCFR0_CTRL_BAR2M               BIT(4)
#define RCFR0_CTRL_NON_BAR2M           BIT(3)
#define RCFR0_MGT2M                    BIT(2)
#define RCFR0_DAT_NULL2M               BIT(1)
#define RCFR0_DAT_NON_NULL2M           BIT(0)

/* RCFR1 */
#define RCFR1_BSSID3_DUP_RFB_MASK      BITS(30,31)
#define RCFR1_BSSID3_DUP_RFB_DYNAMIC   BIT(31)
#define RCFR1_BSSID3_DUP_RFB_ALWAYS    BIT(30)
#define RCFR1_BSSID3_DUP_RFB_OFFSET    30
#define RCFR1_BSSID2_DUP_RFB_MASK      BITS(28,29)
#define RCFR1_BSSID2_DUP_RFB_DYNAMIC   BIT(29)
#define RCFR1_BSSID2_DUP_RFB_ALWAYS    BIT(28)
#define RCFR1_BSSID2_DUP_RFB_OFFSET    28
#define RCFR1_BSSID1_DUP_RFB_MASK      BITS(26,27)
#define RCFR1_BSSID1_DUP_RFB_DYNAMIC   BIT(27)
#define RCFR1_BSSID1_DUP_RFB_ALWAYS    BIT(26)
#define RCFR1_BSSID1_DUP_RFB_OFFSET    26
#define RCFR1_BSSID0_DUP_RFB_MASK      BITS(24,25)
#define RCFR1_BSSID0_DUP_RFB_DYNAMIC   BIT(25)
#define RCFR1_BSSID0_DUP_RFB_ALWAYS    BIT(24)
#define RCFR1_BSSID0_DUP_RFB_OFFSET    24

#define RCFR1_BSSID3_DUP_RFB_MORE_CHECK     BIT(15)
#define RCFR1_BSSID3_DUP_RFB_CTRL_MASK      BITS(12,14)
#define RCFR1_BSSID3_DUP_RFB_CTRL_OFFSET    12
#define RCFR1_BSSID2_DUP_RFB_MORE_CHECK     BIT(11)
#define RCFR1_BSSID2_DUP_RFB_CTRL_MASK      BITS(8,10)
#define RCFR1_BSSID2_DUP_RFB_CTRL_OFFSET    8
#define RCFR1_BSSID1_DUP_RFB_MORE_CHECK     BIT(7)
#define RCFR1_BSSID1_DUP_RFB_CTRL_MASK      BITS(4,6)
#define RCFR1_BSSID1_DUP_RFB_CTRL_OFFSET    4
#define RCFR1_BSSID0_DUP_RFB_MORE_CHECK     BIT(3)
#define RCFR1_BSSID0_DUP_RFB_CTRL_MASK      BITS(0,2)
#define RCFR1_BSSID0_DUP_RFB_CTRL_OFFSET    0

#define DIS_DUP_RFB_INVALID_WLAN_IDX        BIT(2)
#define DIS_DUP_RFB_MGMT_TYPE               BIT(1)
#define DIS_DUP_RFB_DATA_TYPE               BIT(0)


/* VCFR0 */
#define VCFR0_RXV_TOUT_MASK            BITS(16,27)
#define VCFR0_RXV_TOUT_OFFSET          16
#define VCFR0_RXV2M_QIDX1              BIT(14)
#define VCFR0_RXV2M_QIDX2              BIT(15)
#define VCFR0_RXV2M_QIDX3              BITS(14,15)
#define VCFR0_RXV2M_QIDX_MASK          BITS(14,15)
#define VCFR0_RXV2M_QIDX_OFFSET        14
#define VCFR0_RXV2H_QIDX1              BIT(13)
#define VCFR0_RXV2H_QIDX_MASK          BIT(13)
#define VCFR0_RXV2H_QIDX_OFFSET        13
#define VCFR0_RXV_CNT_MASK             BITS(8,12)
#define VCFR0_RXV_CNT_OFFSET           8
#define VCFR0_RXV2M                    BIT(0)

/* TCFR0 */
#define TCFR0_TXS2M_TOUT_MASK          BITS(16,27)
#define TCFR0_TXS2M_TOUT_OFFSET        16
#define TCFR0_TXS2M_QIDX1              BIT(14)
#define TCFR0_TXS2M_QIDX2              BIT(15)
#define TCFR0_TXS2M_QIDX3              BITS(14,15)
#define TCFR0_TXS2M_OIDX_MASK          BITS(14,15)
#define TCFR0_TXS2M_OIDX_OFFSET        14
#define TCFR0_TXS2M_CNT_MASK           BITS(8,12)
#define TCFR0_TXS2M_CNT_OFFSET         8
#define TCFR0_TXS2M_BITMAP_MASK        BITS(0,6)

/* TCFR1 */
#define TCFR1_TXS2H_TOUT_MASK          BITS(16,27)
#define TCFR1_TXS2H_TOUT_OFFSET        16
#define TCFR1_TXS2H_QIDX1              BIT(14)
#define TCFR1_TXS2H_QIDX_OFFSET        14
#define TCFR1_TXS2H_CNT_MASK           BITS(8,12)
#define TCFR1_TXS2H_CNT_OFFSET         8
#define TCFR1_TXS2H_BITMAP_MASK        BITS(0,6)

/* TMCFR0 */
#define TMCFR0_TMR2M_QIDX_MASK         BITS(14,15)
#define TMCFR0_TMR2M_QIDX_OFFSET       14
#define TMCFR0_TMR2H_QIDX_MASK         BIT(13)
#define TMCFR0_TMR2H_QIDX_OFFSET       13
#define TMCFR0_TMRR2M                  BIT(1)
#define TMCFR0_TMRI2M                  BIT(0)

#if 0 /* 6630 */
/* MCR */
#define MCR_HLOCK_EN                BIT(17)
#define MCR_NP_WR_EN                BIT(16)
#define MCR_PRTO_1K_EN              BIT(4)
#define MCR_VDMA_LOCK_EN        BIT(3)
#define MCR_RDMA_LOCK_EN        BIT(2)
#define MCR_TDMA_LOCK_EN        BIT(1)
#define MCR_DDMA_LOCK_EN       BIT(0)
#endif
/***** End of DMA *****/


/****** WTBL ******/
/* BIPCR */
#define BIPCR_SUBTYPE_0_MASK          BITS(28,31)
#define BIPCR_SUBTYPE_0_OFFSET        28
#define BIPCR_SUBTYPE_1_MASK          BITS(24,27)
#define BIPCR_SUBTYPE_1_OFFSET        24
#define BIPCR_SUBTYPE_2_MASK          BITS(20,23)
#define BIPCR_SUBTYPE_2_OFFSET        20
#define BIPCR_SUBTYPE_3_MASK          BITS(16,19)
#define BIPCR_SUBTYPE_3_OFFSET        16
#define BIPCR_SUBTYPE_4_MASK          BITS(12,15)
#define BIPCR_SUBTYPE_4_OFFSET        12
#define BIPCR_SUBTYPE_5_MASK          BITS(8,11)
#define BIPCR_SUBTYPE_5_OFFSET        8
#define BIPCR_BIP_EN                  BIT(0)

/* MAR1 */
#define MAR1_MAR_GROUP_MASK           BITS(30,31)
#define MAR1_MAR_GROUP_OFFSET         30
#define MAR1_MAR_HASH_MODE_BSSID1     BIT(30)
#define MAR1_MAR_HASH_MODE_BSSID2     BIT(31)
#define MAR1_MAR_HASH_MODE_BSSID3     BITS(30,31)

#define MAR1_ADDR_INDEX_MASK          BITS(24,28)
#define MAR1_ADDR_INDEX_OFFSET        24
#define MAR1_READ                     0
#define MAR1_WRITE                    BIT(17)
#define MAR1_ACCESS_START_STATUS      BIT(16)

/* SMESH */
#define SMESH_RX_CTRL_EN              BIT(20)
#define SMESH_RX_MGMT_EN              BIT(19)
#define SMESH_RX_DATA_EN              BIT(18)
#define SMESH_RX_A1_EN                BIT(17)
#define SMESH_RX_A2_EN                BIT(16)
#define SMESH_ADDR_EN_MASK            BITS(0,7)

/* WIUCR */
#define WIUCR_WLAN_IDX                BITS(0,7)
#define WIUCR_WTBL2_UPDATE            BIT(11)   //Posh 20131112, Merge code from MT6630 E2
#define WIUCR_ADM_CNT_CLEAR           BIT(12)
#define WIUCR_RATE_UPDATE             BIT(13)
#define WIUCR_TX_CNT_CLEAR            BIT(14)
#define WIUCR_RX_CNT_CLEAR            BIT(15)
#define WIUCR_IU_BUSY                 BIT(16)
#define WIUCR_RX_CNT_LOCK             BIT(31)   //Posh 20131112, Merge code from MT6630 E2

/* RMVTCR*/
#define RMVTCR_CE_RMSD_THRESHOLD        BITS(24,27)
#define RMVTCR_CE_RMSD_THRESHOLD_OFFSET 24
#define RMVTCR_RX_MV_MODE               BIT(23)
#define RMVTCR_RX_MV_MODE_OFFSET        23
#define RMVTCR_RCPI_AVG_PARAM           BITS(20,21)
#define RMVTCR_RCPI_AVG_PARAM_OFFSET    20
#define RMVTCR_NOISE_AVG_PARAM          BITS(18,19)
#define RMVTCR_NOISE_AVG_PARAM_OFFSET       18
#define RMVTCR_CC_AVG_PARAM             BITS(16,17)
#define RMVTCR_CC_AVG_PARAM_OFFSET      16
#define RMVTCR_TX_CNT_THRESHOLD_MODE    BIT(11)
#define RMVTCR_TX_CNT_THRESHOLD_MODE_OFFSET 11
#define RMVTCR_TX_CNT_THRESHOLD         BITS(8,11)
#define RMVTCR_TX_CNT_THRESHOLD_OFFSET  8
#define RMVTCR_FAIL_CNT_THRESHOLD_MODE  BIT(3)
#define RMVTCR_FAIL_CNT_THRESHOLD_MODE_OFFSET 3
#define RMVTCR_FAIL_CNT_THRESHOLD       BITS(0,2)
#define RMVTCR_FAIL_CNT_THRESHOLD_OFFSET 0

/* WTBL PCSR */
#define WTBL_PCSR_TAG_WLAN_IDX_MASK     BITS(8, 15)
#define WTBL_PCSR_TAG_WLAN_IDX_OFFSET   8
#define WTBL_PCSR_PPSU_EN               BIT(0)

/* WTBL PCR */
#define WTBL_PCR_PPDU_CNT_MASK              BITS(16, 31)
#define WTBL_PCR_PPDU_CNT_OFFSET            16
#define WTBL_PCR_PPDU_OTH_ANT_CNT_MASK      BITS(0, 15)
#define WTBL_PCR_PPDU_OTH_ANT_CNT_OFFSET    0

/* SGCR */
#define SGCR_SGI_PASS_THRESHOLD_MASK  BITS(16,31)
#define SGCR_SGI_PASS_THRESHOLD_OFFSET 16
#define SGCR_SGI_VALID_DEC_CNT_MASK   BITS(0,14)


/* ARCR */

#define ARCR_AR_TXCOUNT_THRESHOLD_MASK   BITS(16,31)
#define ARCR_AR_TXCOUNT_THRESHOLD_OFFSET 16

#define ARCR_AR_WCF BIT(2)

#define ARCR_AR_MODE BITS(0,1)
#define ARCR_DELSEL BITS(4,5)



/* ARCPICR */
#define ARCPICR_MV_EN_MASK              BIT(31)
#define ARCPICR_MV_EN_OFFSET            31
#define ARCPICR_AGING_TIMEOUT_MASK      BIT(30)
#define ARCPICR_AGING_TIMEOUT_OFFSET    30
#define ARCPICR_RCPI_LQ_SWITCH_MASK     BIT(29)
#define ARCPICR_RCPI_LQ_SWITCH_OFFSET   29
#define ARCPICR_MV_PARA_MASK            BITS(24,25)
#define ARCPICR_MV_PARA_OFFSET          24
#define ARCPICR_WLAN_IDX_MASK           BITS(16,23)
#define ARCPICR_WLAN_IDX_OFFSET         16
#define ARCPICR_AGING_LIMIT_MASK        BITS(0,15)
#define ARCPICR_AGING_LIMIT_OFFSET        0

/* ARCPIV0CR */
#define ARCPIV0CR_BW20_11M_MASK BITS(24,31)
#define ARCPIV0CR_BW20_11M_OFFSET 24
#define ARCPIV0CR_BW20_55M_MASK BITS(16,23)
#define ARCPIV0CR_BW20_55M_OFFSET 16
#define ARCPIV0CR_BW20_2M_MASK BITS(8,15)
#define ARCPIV0CR_BW20_2M_OFFSET 8
#define ARCPIV0CR_BW20_1M_MASK BITS(0,7)
#define ARCPIV0CR_BW20_1M_OFFSET 0

/* ARCPIV1CR */
#define ARCPIV1CR_BW20_24M_MASK BITS(16,23)
#define ARCPIV1CR_BW20_24M_OFFSET 16
#define ARCPIV1CR_BW20_12M_MASK BITS(8,15)
#define ARCPIV1CR_BW20_12M_OFFSET 8
#define ARCPIV1CR_BW20_6M_MASK BITS(0,7)
#define ARCPIV1CR_BW20_6M_OFFSET 0

/* ARCPIV5CR */
#define ARCPIV5CR_BW40_24M_MASK BITS(16,23)
#define ARCPIV5CR_BW40_24M_OFFSET 16
#define ARCPIV5CR_BW40_12M_MASK BITS(8,15)
#define ARCPIV5CR_BW40_12M_OFFSET 8
#define ARCPIV5CR_BW40_6M_MASK BITS(0,7)
#define ARCPIV5CR_BW40_6M_OFFSET 0


/***** Security *****/

/* WPCR */
#define TTCR_WPI_INIT_PN_MASK        BITS(8,15)
#define TTCR_WPI_INIT_PN_OFFSET      8
#define TTCR_WPI_PN_RESET            BIT(0)
//#define TTCR_TSC_RESET               BIT(0)


/* SCR */
//#define SCR_WPI_PKTNO_MODE           BIT(31)
#define SCR_CCMP_KE_SKIP_RX_PPDU_EN  BIT(17)
#define SCR_CCMP_KE_SKIP_TX_EN       BIT(16)
#define SCR_CCMP_CCX_SC_MODE         BIT(15)
#define SCR_MPF_EN                   BIT(4)
#define SCR_WPI_QOS_MASK             BIT(3)
#define SCR_WPI_FC64_MASK            BIT(2)
#define SCR_WPI_ORDER_MASK           BITS(0,1)

/* SMACR */
#define SMACR_AES_DONE               BIT(3)
#define SMACR_AES_START              BIT(2)
#define SMACR_KEY_SKIP               BIT(1)
#define SMACR_AES_MODE               BIT(0)

/***** TOP_RGU *****/
/* RGU_WCR */
#define RGU_WCR_WFIDLM_SWCTL_PWR_OK  BIT(27)
#define RGU_WCR_WFIDLM_HW_CTL_PWR_OK BIT(26)
#define RGU_WCR_WFPSE_SWCTL_PWR_OK   BIT(25)
#define RGU_WCR_WFPSE_HWCTL_PWR_OK   BIT(24)
#define RGU_WCR_WIFI_PWR_ACK         BIT(17)
#define RGU_WCR_WIFI_PWR_ACK_S       BIT(16)
#define RGU_WCR_WIFI_TEST_MEM_PD     BIT(13)
#define RGU_WCR_WIFI_TEST_PWR_ON     BIT(12)
#define RGU_WCR_WF_MCU_HCLK_CK_EN    BIT(10)
#define RGU_WCR_QUAL_WF_PWR_SIG      BIT(9)
#define RGU_WCR_WIFI_ON_RST_B        BIT(6)
#define RGU_WCR_WIFI_HWCTL           BIT(5)
#define RGU_WCR_WIFI_POWER_ON        BIT(4)
#define RGU_WCR_WIFI_SOFT_PWR_ON     BIT(3)
#define RGU_WCR_WIFI_SOFT_PWR_ON_S   BIT(2)
#define RGU_WCR_WIFI_SOFT_ISO_EN     BIT(1)
#define RGU_WCR_WIFI_SOFT_OFF_RST_B  BIT(0)

/* SRAM_PWR_EN */
#define WFSYS_MEM_PDN_EN            BITS(0, 7)
#define WFSYS_MEM_PDN_EN_OFFSET     0
#define WFSYS_MEM_SLP_EN            BITS(0, 7)
#define WFSYS_MEM_SLP_EN_OFFSET     0
#define WFPSE_MEM_PDN_EN            BITS(0, 15)
#define WFPSE_MEM_PDN_EN_OFFSET     0
#define WFPSE_MEM_SLP_EN            BITS(0, 15)
#define WFPSE_MEM_SLP_EN_OFFSET     0
#define WFIDLM_MEM_PDN_EN           BITS(0, 31)
#define WFIDLM_MEM_PDN_EN_OFFSET    0
#define WFIDLM_MEM_SLP_EN           BITS(0, 31)
#define WFIDLM_MEM_SLP_EN_OFFSET    0

/*WF_RGDIG_CTL */
#define WF1_RGDIG_REG_RST_B         BIT(3)
#define WF1_RGDIG_LOG_RST_B         BIT(2)
#define WF0_RGDIG_REG_RST_B         BIT(1)
#define WF0_RGDIG_LOG_RST_B         BIT(0)

/***** MCU_CONFIG *****/
#define MCU_CONFIG_MCCR_WIFI_STOP   BIT(5)

/***** HIF *****/
/* HWFISR */ //already defined in MT6620_mcu_reg.h

/* HWFIER */
#define HWFIER_G_HWFIER                 BIT(31)
#define HWFIER_DRV_CLR_FW_OWN_EN        BIT(9)
#define HWFIER_DRV_SET_FW_OWN_EN        BIT(8)
#define HWFIER_RXQ1_EMPTY_EN            BIT(7)
#define HWFIER_RXQ0_EMPTY_EN            BIT(6)
#define HWFIER_TXQ1_FULL_EN             BIT(5)
#define HWFIER_TXQ0_FULL_EN             BIT(4)
#define HWFIER_RX1_DONE_EN              BIT(3)
#define HWFIER_RX0_DONE_EN              BIT(2)
#define HWFIER_TX1_RDY_EN               BIT(1)
#define HWFIER_TX0_RDY_EN               BIT(0)

/* HWFSCR */
#define HWFSCR_FW_OWN_BACK_INT_SET      BIT(4)

/* HWFCR */
#define HWFCR_ALL_CS_OFLD_EN            BITS(3,7)
#define HWFCR_RX_UDP_CS_OFLD_EN         BIT(7)
#define HWFCR_RX_TCP_CS_OFLD_EN         BIT(6)
#define HWFCR_RX_IPV4_CS_OFLD_EN        BIT(5)
#define HWFCR_RX_IPV6_CS_OFLD_EN        BIT(4)
#define HWFCR_TX_CS_OFLD_EN             BIT(3)
#define HWFCR_W_FUNC_RDY                BIT(0)

/* HWFDCR */
#define HWFDCR_RXQ1_DMA_RUM             BIT(27)
#define HWFDCR_RXQ0_DMA_RUM             BIT(26)
#define HWFDCR_TXQ1_DMA_RUM             BIT(25)
#define HWFDCR_TXQ0_DMA_RUM             BIT(24)
#define HWFDCR_RXQ1_DMA_START           BIT(23)
#define HWFDCR_RXQ0_DMA_START           BIT(22)
#define HWFDCR_TXQ1_DMA_START           BIT(21)
#define HWFDCR_TXQ0_DMA_START           BIT(20)
#define HWFDCR_RXQ1_DMA_STOP            BIT(19)
#define HWFDCR_RXQ0_DMA_STOP            BIT(18)
#define HWFDCR_TXQ1_DMA_STOP            BIT(17)
#define HWFDCR_TXQ0_DMA_STOP            BIT(16)
#define HWFDCR_INC_TQ5_CNT              BIT(5)
#define HWFDCR_INC_TQ4_CNT              BIT(4)
#define HWFDCR_INC_TQ3_CNT              BIT(3)
#define HWFDCR_INC_TQ2_CNT              BIT(2)
#define HWFDCR_INC_TQ1_CNT              BIT(1)
#define HWFDCR_INC_TQ0_CNT              BIT(0)

#define HWFDCR_INC_TQ_CNT(_ucTC)        BIT(_ucTC)

#define HWFDCR_MAX_TQ_NUM                   6

#define HWFDCR_RX_QUE_RUM_OFFSET            18
#define HWFDCR_RX_QUE_START_OFFSET          17
#define HWFDCR_RX_QUE_STOP_OFFSET           16

#define HWFDCR_TX_QUE_RUM_OFFSET            22
#define HWFDCR_TX_QUE_START_OFFSET          20
#define HWFDCR_TX_QUE_STOP_OFFSET           16

#if 0
/* MAC/PHY PIN Connection */
#define PHY_MUX_BASE                     0x60200800
#define PHY_ENABLE_CR                    0x60208000
#define PHY_SWCTRL                       0x6020080C
    #define TRSW_CTRL                       BIT(5)
    #define TRSW_CTRL_OFFSET                5
    #define SW_CTRL                         BIT(4)
    #define SW_CTRL_OFFSET                  4
    #define SW_PAPE                         BIT(1)
    #define SW_PAPE_OFFSET                   1
    #define SW_TRSW                         BIT(0)
    #define SW_TRSW_OFFSET                  0
#define PPN_TX_DATA                      0x60200818
#define PPN_TX_CTRL                      0x6020081C
#endif
#define PHY_PIN_CONN_MAC2PHY_TX             BIT(31)
#define PHY_PIN_CONN_MAC2PHY_TX_RIFS        BIT(30)
#define PHY_PIN_CONN_MAC2PHY_TX_TXD_REQ     BIT(29)
#define PHY_PIN_CONN_MAC2PHY_RX             BIT(28)
#define PHY_PIN_CONN_MAC2PHY_RXD_ACK        BIT(27)
#define PHY_PIN_CONN_MAC2PHY_STSCNT_EN      BIT(26)
#define PHY_PIN_CONN_MAC2PHY_IPI_EN         BIT(25)
#define PHY_PIN_CONN_MAC2PHY_RPI_EN         BIT(24)
#define PHY_PIN_CONN_MAC2PHY_RDD_EN         BIT(23)
#define PHY_PIN_CONN_MAC2PHY_NAV_IND        BIT(22)



/***** CONFIG *****/
/* TMTCR */
#define TMTCR_TEST_MODE_EN                 BIT(31)
#define TMTCR_TX_ERR                       BIT(3)
#define TMTCR_PHY2MAC_TXRDY         BIT(2)
#define TMTCR_MAC_TX_RDY                       BIT(1)
#define TMTCR_MAC_RX_RDY                       BIT(0)


/* TMTDR */

/* TMRCSR */
#define TMRCSR_RX_AIR_END                   BIT(6)
#define TMRCSR_CCA_CS                         BIT(5)
#define TMRCSR_CCA_SEC                       BIT(4)
#define TMRCSR_CCA_PRIM                      BIT(3)
#define TMRCSR_RX_ERR                          BIT(2)
#define TMRCSR_MDRDY                           BIT(1)
#define TMRCSR_MAC_RX_RDY                 BIT(0)

/* TMRDR */

/* TMRDTR */
#define TMRDTR_RX_DATA_TYPE             BITS(0,2)

/* DBG_EN */
#define DBG_EN_DBG_CKEN         BIT(4)
#define DBG_EN_DBG_L2_B1_EN     BIT(3)
#define DBG_EN_DBG_L2_B0_EN     BIT(2)
#define DBG_EN_DBG_L1_B1_EN     BIT(1)
#define DBG_EN_DBG_L1_B0_EN     BIT(0)


/* Select by each module*/
#define DBG_FLAG_SEL_0  BITS(0,7)
#define DBG_FLAG_SEL_0_OFFSET  0
#define DBG_FLAG_SEL_1  BITS(8,15)
#define DBG_FLAG_SEL_1_OFFSET  8
#define DBG_FLAG_SEL_2  BITS(16,23)
#define DBG_FLAG_SEL_2_OFFSET  16
#define DBG_FLAG_SEL_3  BITS(24,31)
#define DBG_FLAG_SEL_3_OFFSET  24

/*  0: WF_SEC
  *  1: WF_WTBL
  *  2: WF_SIF
  *  3: WF_TRB
  *  4: WF_DMA
  *  5: WF_ARB
  *  6: WF_AGG
  *  7: WF_RMAC
  *  8: WF_TMAC
  */
#define DBG_MODULE_SEL_0  BITS(0,7)
#define DBG_MODULE_SEL_0_OFFSET  0
#define DBG_MODULE_SEL_1  BITS(8,15)
#define DBG_MODULE_SEL_1_OFFSET  8
#define DBG_MODULE_SEL_2  BITS(16,23)
#define DBG_MODULE_SEL_2_OFFSET  16
#define DBG_MODULE_SEL_3  BITS(24,31)
#define DBG_MODULE_SEL_3_OFFSET  24


/* 0: Level 1 debug signal.
  * 1: WF_LP
  * 2: WF_AHBMUX
  * 3: WF_CFG
  */
#define DBG_SOURCE_SEL_0  BITS(0,7)
#define DBG_SOURCE_SEL_0_OFFSET  0
#define DBG_SOURCE_SEL_1  BITS(8,15)
#define DBG_SOURCE_SEL_1_OFFSET  8
#define DBG_SOURCE_SEL_2  BITS(16,23)
#define DBG_SOURCE_SEL_2_OFFSET  16
#define DBG_SOURCE_SEL_3  BITS(24,31)
#define DBG_SOURCE_SEL_3_OFFSET  24

/* Level 1 the same with level 2.
  */
#define DBG_FLAG_VALUE_0  BITS(0,7)
#define DBG_FLAG_VALUE_0_OFFSET  0
#define DBG_FLAG_VALUE_1  BITS(8,15)
#define DBG_FLAG_VALUE_1_OFFSET  8
#define DBG_FLAG_VALUE_2  BITS(16,23)
#define DBG_FLAG_VALUE_2_OFFSET  16
#define DBG_FLAG_VALUE_3  BITS(24,31)
#define DBG_FLAG_VALUE_3_OFFSET  24

#define CNTR1_END_SEL               BITS(24,27)
#define CNTR1_END_SEL_OFFSET  24
#define CNTR1_START_SEL             BITS(16,19)
#define CNTR1_START_SEL_OFFSET  16
#define CNTR0_END_SEL               BITS(8,11)
#define CNTR0_END_SEL_OFFSET  8
#define CNTR0_START_SEL             BITS(0,3)
#define CNTR0_START_SEL_OFFSET  0

#define CNTR3_END_SEL               BITS(24,27)
#define CNTR3_END_SEL_OFFSET  24
#define CNTR3_START_SEL             BITS(16,29)
#define CNTR3_START_SEL_OFFSET  16
#define CNTR2_END_SEL               BITS(8,11)
#define CNTR2_END_SEL_OFFSET  8
#define CNTR2_START_SEL             BITS(0,3)
#define CNTR2_START_SEL_OFFSET  0


#define CNTR_CTRL_CNTR0_START BIT(0)
#define CNTR_CTRL_CNTR0_CLR     BIT(1)
#define CNTR_CTRL_START0_EDG_SEL BIT(2)
#define CNTR_CTRL_END0_EDG_SEL BIT(3)
#define CNTR_CTRL_DONE0             BIT(4)
#define CNTR_CTRL_CNTR1_START BIT(8)
#define CNTR_CTRL_CNTR1_CLR     BIT(9)
#define CNTR_CTRL_START1_EDG_SEL BIT(10)
#define CNTR_CTRL_END1_EDG_SEL BIT(11)
#define CNTR_CTRL_DONE1             BIT(12)
#define CNTR_CTRL_CNTR2_START BIT(16)
#define CNTR_CTRL_CNTR2_CLR     BIT(17)
#define CNTR_CTRL_START2_EDG_SEL BIT(18)
#define CNTR_CTRL_END2_EDG_SEL BIT(19)
#define CNTR_CTRL_DONE2             BIT(20)
#define CNTR_CTRL_CNTR3_START BIT(24)
#define CNTR_CTRL_CNTR3_CLR     BIT(25)
#define CNTR_CTRL_START3_EDG_SEL BIT(26)
#define CNTR_CTRL_END3_EDG_SEL BIT(27)
#define CNTR_CTRL_DONE3             BIT(28)

#define CNTR_CTRL_H_CLK_SEL         BIT(29)
#define CNTR_CTRL_LP_CLK_SEL        BIT(30)
#define CNTR_CTRL_INVERSE_CLK_SEL      BIT(31)


#define CNTR_3_NUM  BITS(24,31)
#define CNTR_3_NUM_OFFSET  24
#define CNTR_2_NUM  BITS(16,23)
#define CNTR_2_NUM_OFFSET  16
#define CNTR_1_NUM  BITS(8,15)
#define CNTR_1_NUM_OFFSET  8
#define CNTR_0_NUM  BITS(0,7)
#define CNTR_0_NUM_OFFSET  0


#define MEASURE_PIN_CONN_TEST         (WIFI_LP_BASE + 0x00BC)
#define MEASURE_PIN_CONN_TEST_EN      BIT(2)
#define MEASURE_ENABLE_1              BIT(1)
#define MEASURE_ENABLE_0              BIT(0)

#define WIFI_MAC_VER                  0x80000700

/* HIF Client CR */
//#define HIFCR_BASE      0x800C0000
#define HIFCR_TXCFFA    (HIFCR_BASE + 0x0000)
#define HIFCR_TXCAC0    (HIFCR_BASE + 0x0004)
#define HIFCR_TXCAC1    (HIFCR_BASE + 0x0008)
#define HIFCR_TXCAC2    (HIFCR_BASE + 0x000c)
#define HIFCR_TXCAC3    (HIFCR_BASE + 0x0010)
#define HIFCR_TXCAC4    (HIFCR_BASE + 0x0014)
#define HIFCR_TXCAC5    (HIFCR_BASE + 0x0018)
#define HIFCR_TXCAC6    (HIFCR_BASE + 0x001c)
#define HIFCR_TXCBMC    (HIFCR_BASE + 0x0020)
#define HIFCR_TXCBCN    (HIFCR_BASE + 0x0024)
#define HIFCR_TXCAC10   (HIFCR_BASE + 0x0028)
#define HIFCR_TXCAC11   (HIFCR_BASE + 0x002c)
#define HIFCR_TXCAC12   (HIFCR_BASE + 0x0030)
#define HIFCR_TXCAC13   (HIFCR_BASE + 0x0034)
#define HIFCR_TXCAC14   (HIFCR_BASE + 0x0038)
#define HIFCR_TXCCPU    (HIFCR_BASE + 0x003c)
#define HIFCR_DW2       (HIFCR_BASE + 0x0040)
#define HIFCR_DW3       (HIFCR_BASE + 0x0044)
#define HIFCR_DW4       (HIFCR_BASE + 0x0048)
#define HIFCR_DW5       (HIFCR_BASE + 0x004c)
#define HIFCR_DW6       (HIFCR_BASE + 0x0050)
#define HIFCR_LPIN      (HIFCR_BASE + 0x0054)
#define HIFCR_TXINT     (HIFCR_BASE + 0x0058)
#define HIFCR_TXRETM0   (HIFCR_BASE + 0x005c)
#define HIFCR_TXRETM1   (HIFCR_BASE + 0x0060)
#define HIFCR_TXRETM2   (HIFCR_BASE + 0x0064)
#define HIFCR_RXINF     (HIFCR_BASE + 0x0068)
#define HIFCR_DSR       (HIFCR_BASE + 0x006c)
#define HIFCR_RST       (HIFCR_BASE + 0x0070)
#define HIFCR_INT       (HIFCR_BASE + 0x0074)
#define HIFCR_CSO       (HIFCR_BASE + 0x0078)
#define HIFCR_COBUIN    (HIFCR_BASE + 0x007c)
#define HIFCR_COBURX0IN (HIFCR_BASE + 0x0080)
#define HIFCR_COBURX1IN (HIFCR_BASE + 0x0084)
#define HIFCR_EXHDCSO   (HIFCR_BASE + 0x0088)

/* HIFCR_LPIN */
#define HIFCR_LPIN_MASK_RX_INT              BIT(0)
#define HIFCR_LPIN_MASK_TX_INT              BIT(1)
#define HIFCR_LPIN_MASK_RX_INT_STATUS       BIT(4)
#define HIFCR_LPIN_MASK_TX_INT_STATUS       BIT(5)

/* HIFCR_TXINT */
#define HIFCR_TXINT_TXINTMEN                BIT(16)
#define HIFCR_TXINT_TXTMO                   BIT(0, 15)

/* HIFCR_RXINF */
#define HIFCR_RXINF_GROUP1                  BIT(0)
#define HIFCR_RXINF_GROUP2                  BIT(1)
#define HIFCR_RXINF_GROUP3                  BIT(2)

/* HIFCR_RST */
#define HIFCR_RST_RX_RESET_STATUS_DATA_FLOW BIT(9)
#define HIFCR_RST_RX_RESET_STATUS_LENGTH    BIT(8)
#define HIFCR_RST_RX_RESET_DATA_FLOW        BIT(5)
#define HIFCR_RST_RX_RESET_LENGTH           BIT(4)
#define HIFCR_RST_TX_RESET                  BIT(0)

/* HIFCR_INT */
#define HIFCR_INT_TX_OVERFLOW_STATUS        BIT(0)
#define HIFCR_INT_RX_COALESCING_STATUS      BIT(1)
#define HIFCR_INT_TX_COALESCING_STATUS      BIT(2)
#define HIFCR_INT_TX_OVERFLOW_EN            BIT(16)
#define HIFCR_INT_RX_COALESCING_EN          BIT(17)
#define HIFCR_INT_TX_COALESCING_EN          BIT(18)

/* HIFCR_CSO */
#define HIFCR_CSO_TX_EN                     BIT(0)
#define HIFCR_CSO_RX_IPV4_EN                BIT(16)
#define HIFCR_CSO_RX_IPV6_EN                BIT(17)
#define HIFCR_CSO_RX_TCP_EN                 BIT(18)
#define HIFCR_CSO_RX_UDP_EN                 BIT(19)

/* HIFCR_COBUIN */
#define HIFCR_COBUIN_EN                     BIT(31)

/* HIFCR_COBURX0IN */
#define HIFCR_COBURX0IN_CLS_MAX_CNT         BITS(16, 19)
#define HIFCR_COBURX0IN_CLS_MAX_CNT_OFFSET  (16)
#define HIFCR_COBURX0IN_CLS_TOUT            BITS(0, 15)

/* HIFCR_COBURX1IN */
#define HIFCR_COBURX1IN_CLS_MAX_CNT         BITS(16, 19)
#define HIFCR_COBURX1IN_CLS_MAX_CNT_OFFSET  (16)
#define HIFCR_COBURX1IN_CLS_TOUT            BITS(0, 15)

/* HIFCR_EXHDCSO */
#define HIFCR_EXHDCSO_HOP_BY_HOP_EN                 BIT(20)
#define HIFCR_EXHDCSO_DEST_OP_EN                    BIT(19)
#define HIFCR_EXHDCSO_RH_EN                         BIT(18)
#define HIFCR_EXHDCSO_AH_EN                         BIT(17)
#define HIFCR_EXHDCSO_ESP_EN                        BIT(16)
#define HIFCR_EXHDCSO_CUSTOM_NH                     BITS(8, 15)
#define HIFCR_EXHDCSO_OFFSET_CUSTOM_NH              (8)
#define HIFCR_EXHDCSO_CUSTOM_NH_DW_OFFSET           BITS(6, 7)
#define HIFCR_EXHDCSO_OFFSET_CUSTOM_NH_DW_OFFSET    (6)
#define HIFCR_EXHDCSO_CUSTOM_NH_JUMP_WEIGHT         BITS(4, 5)
#define HIFCR_EXHDCSO_OFFSET_CUSTOM_NH_JUMP_WEIGHT  (4)

#define HIFCR_EXHDCSO_CUSTOM_NH_EN          BIT(0)


/* PDA - Patch Decryption Accelerator */
#define PDA_TAR_ADDR        (WIFI_PDA_BASE + 0x0000)
#define PDA_TAR_LEN         (WIFI_PDA_BASE + 0x0004)
#define PDA_DWLD_STATE      (WIFI_PDA_BASE + 0x0008)
#define PDA_CONFG           (WIFI_PDA_BASE + 0x000c)
#define PDA_CRC_VALUE       (WIFI_PDA_BASE + 0x0010)

#define PDA_TARGET_LENGTH                   BITS(0, 17)

#define PDA_DOWNLOAD_SOFT_RESET             BIT(31)
#define PDA_DOWNLOAD_BUSY                   BIT(1)
#define PDA_DOWNLOAD_FINISH                 BIT(0)

#define PDA_CONFG_DISABLE_DECRYPT           BIT(0)
#define PDA_CONFG_HEADER_3DW                BIT(1)

/* DMA Scheduler Control */
#define DMA_SCHEDULER_REG_4                                 (HIF_DMA_SCHEDULER_base + 0x00000094)

#define HIF_DMA_SCHED_FORCE_QID             BITS(0, 3)
#define HIF_DMA_SCHED_FORCE_MODE            BIT(4)
#define HIF_DMA_SCHED_BYPASS_MODE           BIT(5)
#define HIF_DMA_SCHED_HYBRID_MODE           BIT(6)
#define HIF_DMA_SCHED_SOFT_RST              BIT(8)

/* USB Endpoint Mapping to PSE Qid */
#define USB_EP4_OUT_QID                     0
#define USB_EP5_OUT_QID                     1
#define USB_EP6_OUT_QID                     2
#define USB_EP7_OUT_QID                     3
#define USB_EP8_OUT_QID                     8
#define USB_EP9_OUT_QID                     9



/* MISC */
#define WIFI_SW_SYNC0      (TOP_RGU_base + 0x1250)
#define WIFI_SW_SYNC1      (TOP_RGU_base + 0x1254)  /* Used by Wi-Fi Co-SIM ROM version */
#define WIFI_SW_SYNC2      (TOP_RGU_base + 0x115C)  /* Used by Wi-Fi/BT calibration flow log */
#define TOP_STRAP_STATUS   (TOP_CFG_base + 0x010)


/***** TOP_CFG_AON *****/
#define PWRCTLCR_OSC_EN_RX                          BIT(18)

/*
 * Chip version control
 */
#define TOP_CONFIG_HW_VER                           (CONFIG_base + 0x0000)      //HW Version
#define TOP_CONFIG_FW_VER                           (CONFIG_base + 0x0004)      //ROM Version
#define TOP_CONFIG_HW_CODE                          (CONFIG_base + 0x0008)      //Chip ID

#define TOP_CONFIG_VER_CODE_MASK                    BITS(0,15)
#define TOP_CONFIG_HW_SW_VER_MASK                   BITS(0,7)
#define TOP_CONFIG_FACTORY_VER_MASK                 BITS(8,11)
#define TOP_CONFIG_FACTORY_VER_OFFSET               8



/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _MT6630_WIFI_REGV1_ROM_H */

