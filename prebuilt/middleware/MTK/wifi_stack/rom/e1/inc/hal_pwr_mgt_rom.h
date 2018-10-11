/*
** $Id: //Department/DaVinci/TRUNK/MT6628_FPGA_BORA_BASE_20111109/mcu/wifi/ROM/include/nic/hal/MT6628/hal_pwr_mgt_rom.h#1 $
*/

/*! \file   "hal_hif.h"
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
** 06 19 2015 tiger.hung
** [BORA00004558] [MT7637][E2] Checkin
** Update SMCTL  to 6/12
** fix SMCTL related functions
**
** 05 28 2015 tiger.hung
** [BORA00004558] [MT7637][E2] Checkin
** - RGU control correction
** - rom api fix
** - misc
**
** 01 19 2015 tiger.hung
** [BORA00003732] [MT7637][E1][MP][WIFI]
** sync ILM/DLM bank size definition from cos_api_t.c
**
** 01 19 2015 tiger.hung
** [BORA00003732] [MT7637][E1][MP][WIFI]
** speperate Low Power backup/restore time source
**
** 01 01 2015 ted.huang
** [BORA00003379] [MT7637]
** add API to determine whether control WIFI memory power in ROM code.
**
** 12 10 2014 tiger.hung
** [BORA00003582] [MT7637] Rom code modification
** update ILM/DLM boundary for check
**
** 11 26 2014 tiger.hung
** [BORA00003573] [MT7637] Wifi Power on Sequence  Check-n
** Modify PSE band size and WIFI sys band size according to POS doc
** http://teams.mediatek.inc/WCP/WCN/Combo/MT7637/MT7637/12_WIFISYS/WIFI_MAC/WF_LP%20Flow/WiFi_LP%20power%20on%20sequence.xlsx
**
** 11 20 2014 ted.huang
** [BORA00003379] [MT7637]
** Enable MTCMOS_WIFI_POWER_DOWN_CONTROL in FPGA.
**
** 12 11 2013 ted.huang
** [BORA00002807] [MT7636]
** move g_ucIsPseMBISTPass.
**
** 12 06 2013 ted.huang
** [BORA00002807] [MT7636]
** update MT7636 WIFI ROM code.
**
** 12 03 2013 ted.huang
** [BORA00002807] [MT7636]
** update 7636 power on sequence 1203.
**
** 12 03 2013 ted.huang
** [BORA00002807] [MT7636]
** update WIFI ROM low power API.
**
** 11 29 2013 ted.huang
** [BORA00002807] [MT7636]
** update 7636 power on/off sequence.
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
** 07 25 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add LP test case for multicast table backup/restore.
**
** 07 24 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add rom patch for "Enter LP assertion check" and fix getCurrentTsf function.
**
** 07 23 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add rom patch for "LP stable time modification for wakeup".
**
** 06 06 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** 1. add rfdig reset into wifi power on sequence.
** 2. adjust wifi timing for power on
**
** 06 04 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** remove obsolete pattern filter.
**
** 06 03 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** update tttt earlier interval hal functions.
**
** 05 24 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** update hal to control stable time.
**
** 05 24 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** update wifi power on sequence.
**
** 05 21 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** update memory power control.
**
** 05 20 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add busy status check for rx path.
**
** 05 17 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add preTbtt and preTTTT for HW timeup.
**
** 05 17 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** update sram power control.
**
** 05 08 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** change hal functions to config tttt offset.
**
** 05 07 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add programmable beacon filters.
**
** 05 02 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add sleep control.
**
** 05 02 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add hal functions for earlier detection of tim broadcast.
**
** 04 30 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add hal functions for tim broadcast .
**
** 04 26 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add pse sram power control.
**
** 04 24 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add sram base address for power control.
**
** 02 20 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** add WIS3R for LP and change Timer4 to Timer8.
**
** 02 19 2013 tsaiyuan.hsu
** [BORA00002391] [MT6630]Develop LP emulation code
** extend bssid number for LP functions.
**
** 10 01 2012 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Draft version with new BSS_INFO, NETWORK_TYPE_INDEX, pkt/mem alloc/free and so on
**
** 08 30 2012 yarco.yang
** [BORA00002091] [MT6630]Add compiler flag
** .
**
** 08 29 2012 yarco.yang
** [BORA00002091] [MT6630]Add compiler flag
** .
**
** 08 06 2012 yarco.yang
** [BORA00002091] [MT6630]Support 6630 complier flag
 *
 * 06 28 2012 george.huang
 * [BORA00001717] [MT6628][ROM patch] HAL power on sequence patch
 * update for 6582
 *
 * 11 09 2011 jeffrey.chang
 * [BORA00001425] [WIFISYS] Initial WI-FI F/W import for MT6628
 * import latest wifi f/w
 *
 * 10 21 2011 george.huang
 * [WCXRP00000905] [MT6628 Wi-Fi][FW] Code refinement for ROM/ RAM module dependency
 * resolve build error when r=1
 *
 * 10 19 2011 george.huang
 * [WCXRP00000905] [MT6628 Wi-Fi][FW] Code refinement for ROM/ RAM module dependency
 * New feature "WIFI Unify Driver" support
 *
 * 09 30 2011 george.huang
 * [WCXRP00000905] [MT6628 Wi-Fi][FW] Code refinement for ROM/ RAM module dependency
 * Add related HAL function declairation
 *
 * 09 14 2011 jeffrey.chang
 * [WCXRP00000984] [MT6628][MCUSYS] import latest BORA system service
 * import for latest system service
 *
 * 08 25 2011 george.huang
 * [WCXRP00000905] [MT6628 Wi-Fi][FW] Code refinement for ROM/ RAM module dependency
 * use ROM inclusion files in ROM sources
 *
 * 08 17 2011 jeffrey.chang
 * [WCXRP00000936] [MT6628] Initial import BORA  for FPGA 0815
 * initial import for FPGA 0815 bit file and COS
 *
 * 08 15 2011 george.huang
 * [WCXRP00000905] [MT6628 Wi-Fi][FW] Code refinement for ROM/ RAM module dependency
 * modify for HAL_SRAM_SW_ENABLE_POWER_SAVE() interface
 *
 * 08 15 2011 george.huang
 * [WCXRP00000905] [MT6628 Wi-Fi][FW] Code refinement for ROM/ RAM module dependency
 * .
 *
 * 08 15 2011 george.huang
 * [WCXRP00000905] [MT6628 Wi-Fi][FW] Code refinement for ROM/ RAM module dependency
 * Move HAL functions to ROM image.
 *
 * 08 15 2011 george.huang
 * [WCXRP00000905] [MT6628 Wi-Fi][FW] Code refinement for ROM/ RAM module dependency
 * update for HAL part
 *
 * 08 12 2011 george.huang
 * [WCXRP00000905] [MT6628 Wi-Fi][FW] Code refinement for ROM/ RAM module dependency
 * modify for SRAM control dependency in 6628
 *
 * 08 10 2011 george.huang
 * [WCXRP00000905] [MT6628 Wi-Fi][FW] Code refinement for ROM/ RAM module dependency
 * coding modification for HAL and its caller
 *
 * 07 18 2011 jeffrey.chang
 * [WCXRP00000859] [MT6628 Wi-Fi] import the MT6628 FPGA base from BORA
 * import 6628 baseline
 *
 * 07 18 2011 jeffrey.chang
 * [BORA00001425] [WIFISYS] Initial WI-FI F/W import for MT6628
 * import wi-fi build script
 *
 * 07 11 2011 george.huang
 * [WCXRP00000818] [MT6620 Wi-Fi][Driver] Remove unused code segment regarding CONFIG_IPV6
 * .
 *
 * 06 14 2011 george.huang
 * [WCXRP00000331] [MT5931][FW] Supprot register WiFi power down with backup/ restore function
 * update CMDBT instructions for MTCMOS function by DE's request
 *
 * 06 03 2011 george.huang
 * [WCXRP00000754] [MT6620 Wi-Fi][FW] FW assert on settign T0 timer
 * disable MTCMOS PD
 *
 * 05 25 2011 george.huang
 * [WCXRP00000670] [MT5931][FW] enable PS protocol
 * separate common CMDBT direct write part to function
 *
 * 05 24 2011 george.huang
 * [WCXRP00000670] [MT5931][FW] enable PS protocol
 * enable LP function (including MTCMOS, SRAM power control)
 *
 * 03 02 2011 george.huang
 * [WCXRP00000331] [MT5931][FW] Supprot register WiFi power down with backup/ restore function
 * modify backup/ restore entries
 *
 * 01 04 2011 george.huang
 * [WCXRP00000333] [MT5931][FW] support SRAM power control drivers
 * Revise for new ILM/ DLM setting
 *
 * 12 24 2010 george.huang
 * [WCXRP00000232] [MT5931 Wi-Fi][FW] Modifications for updated HW power on sequence and related design
 * 1. integrate SRAM power control
 *
 * 12 08 2010 george.huang
 * [WCXRP00000232] [MT5931 Wi-Fi][FW] Modifications for updated HW power on sequence and related design
 * HELP rlm_test to control SX and PLL by HAL macro.
 *
 * 12 07 2010 george.huang
 * [WCXRP00000232] [MT5931 Wi-Fi][FW] Modifications for updated HW power on sequence and related design
 * Update MT5931 power on sequence.
 *
 * 12 06 2010 george.huang
 * [WCXRP00000232] [MT5931 Wi-Fi][FW] Modifications for updated HW power on sequence and related design
 * Modification for updated HW power on sequence.
 *
 * 10 25 2010 cp.wu
 * [WCXRP00000083] [MT5931][Driver][FW] Add necessary logic for MT5931 first connection
 * turn off low power options for MT6620 family.
 *
 * 10 22 2010 cp.wu
 * [WCXRP00000083] [MT5931][Driver][FW] Add necessary logic for MT5931 first connection
 * resync.
 *
 * 10 18 2010 george.huang
 * [WCXRP00000090] [MT6620 Wi-Fi][FW] power down register backup/ restore procedure
 * Support register recovery function after WIFISYS power down (but PD function is default disabled).
 *
 * 10 05 2010 george.huang
 * NULL
 * update LP timing setting.
 *
 * 08 10 2010 cp.wu
 * NULL
 * add HAL to firmware part.
 *
 * 07 26 2010 george.huang
 *
 * 1. Tune max. beacon timeout from 100 to 20
 * 2. Add halPmGetLatestBeaconContentHashValue()
 * 3. Add advanced handler for beacon timeout, by extend timeout settings
 * 4. Add TIM IE composing function (buffered BMC/ UC status update)
 * 5. On connection deferred timeout, do absence procedure if pending absence request
 *
 * 07 12 2010 cp.wu
 *
 * BORA Part (ROM code) of MT6620-E1
 *
 * 04 22 2010 george.huang
 * [BORA00000678][MT6620]WiFi LP integration
 * default disable TCM/ SRAM power control
 *
 * 04 22 2010 george.huang
 * [BORA00000678][MT6620]WiFi LP integration
 * 1. modify WiFi power on sequence
 * 2. add more compile time sanity check
 *
 * 04 20 2010 george.huang
 * [BORA00000678][MT6620]WiFi LP integration
 *
 *  *  *  *  * ; *  *  *  * 1. modify WiFi variable partition and do initialization
 *  *  *  *  * ; *  *  *  *
 *  *  *  *  * ; *  *  *  * 2. add emulation startup veneer in ROM code
 *
 * 04 16 2010 george.huang
 * [BORA00000678][MT6620]WiFi LP integration
 * move LP instruction buffer from using stack to TCM
 *
 * 04 12 2010 george.huang
 * [BORA00000678][MT6620]WiFi LP integration
 * update for IGNORE_WIFI_TASK_TO_DO_WIFISYS_POWER_ON_OFF
 *
 * 03 26 2010 george.huang
 * [BORA00000678][MT6620]WiFi LP integration
 * place code to proper TCM address
 *
 * 03 25 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * .
 *
 * 03 25 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * prevent compiler warning
 *
 * 03 24 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * update LP functions
 *
 * 03 23 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * update for TCM/ SRAM power control macro
 *
 * 03 21 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * Update TCM/ SRAM power control related driver code
 *
 * 03 16 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * add LP functions
 *
 * 03 15 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * update LP functions
 *
 * 03 12 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * add LP functions
 *
 * 03 11 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * modification for WiFi power on sequence and SRAM/ TCM power control sequence
 *
 * 03 03 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * move ENUM_LOCAL_TSF_T back to hal_pwr_mgt.h by modified include sequence
 *
 * 03 02 2010 yuche.tsai
 * [BORA00000343][MT6620] Emulation For TX
 * Move ENUM_LOCAL_TSF_T from hal_pwr_mgt.h to hal.h.
 * Because there is one HAL function which also need this type definition.
 *
 * 02 25 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * Update LP related function
 *
 * 02 20 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * update LP related control
 *
 * 01 29 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * Integrate LP related functions
 *
 * 01 18 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * modify for LP function updates
 *
 * 01 13 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * .
 *
 * 01 10 2010 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * .
 *
 * 12 24 2009 george.huang
 * [BORA00000018]Integrate WIFI part into BORA for the 1st time
 * .
 *
 * Dec 8 2009 mtk01084
 * [BORA00000018] Integrate WIFI part into BORA for the 1st time
 * update PM control
 *
 * Nov 10 2009 mtk01084
 * [BORA00000018] Integrate WIFI part into BORA for the 1st time
 *
 *
 * Nov 3 2009 mtk01084
 * [BORA00000018] Integrate WIFI part into BORA for the 1st time
 * update latest HAL functions
 *
 * Oct 15 2009 mtk01084
 * [BORA00000018] Integrate WIFI part into BORA for the 1st time
 *
**
*/

#ifndef _HAL_PM_ROM_H
#define _HAL_PM_ROM_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/
#define MTCMOS_WIFI_POWER_DOWN_CONTROL              1
#define TCM_POWER_CONTROL                           1
#define WIFISYS_SRAM_POWER_CONTROL                  1
#define PSE_SRAM_POWER_CONTROL                      1
#define LP_POWER_ON_OFF_SEQ_IN_HW_MODE              1

#define OPTIMIZE_BACKUP_TIME                        1


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
/* Memory allocation */
#if 0
#define ILM_BASE_START_ADDR                 0x000D0000  //exclude the 1st and 2nd TCM banks fro MCSUSYS
#define ILM_BASE_END_ADDR                   0x00108000  /* ILM 320K */
#define DLM_BASE_START_ADDR                 0x02090000
#define DLM_BASE_END_ADDR                   0x020D8000  /* DLM 288K */
#define PSESRAM_BASE_START_ADDR             PSE_MT6630_DIRECT_ACCESS_ADDR
#define PSESRAM_BASE_END_ADDR               (PSE_MT6630_DIRECT_ACCESS_ADDR + PSE_MEMORY_SIZE)
#endif

/* RGU each bank size */
#define RGU_PER_BANK_SIZE                   (32<<10)    // 32KB

/* RGU DLM Memory bank */
#define DLM_ALL_BANK_SIZE                   9
#define DLM_SUBBANK_OFFSET                  10          // DLM bank memory control is following ILD bank
#define DLM_ALL_SUBBANK                     BITS(0, DLM_ALL_BANK_SIZE - 1)
#define DLM_WIFI_SUBBANK                    BITS(2, 3)

/* RGU ILM Memory bank */
#define ILM_ALL_BANK_SIZE                   10
#define ILM_ALL_SUBBANK                     BITS(0, ILM_ALL_BANK_SIZE - 1)
#define ILM_WIFI_SUBBANK                    BITS(2, 8)


/* RGU TCM memory control (The TCM control is used by MCUSYS for all chip TCM or ILM/DLM */
#define TCM_ALL_BANK_SIZE                   (ILM_ALL_BANK_SIZE + DLM_ALL_BANK_SIZE)
#define TCM_ALL_SUBBANK                     BITS(0, TCM_ALL_BANK_SIZE - 1)

/* RGU PSE SRAM Memory bank */
#define PSE_ALL_BANK_SIZE                   9
#define PSE_ALL_SUBBANK                     BITS(0, PSE_ALL_BANK_SIZE - 1)

/* RGU WIFISYS SRAM Memory bank */
#define WFSYS_ALL_BANK_SIZE                 5
#define WFSYS_ALL_SUBBANK                   BITS(0, WFSYS_ALL_BANK_SIZE - 1)


/* LP timing. These timing are from DE LP document, sync 6/12 value */
#define ANA_OP_PHASE_OFFSET                 (5)
#define OSC_STABLE_TIME                     (35*30)     //0x9F 32k ~3ms
#define PLL_STABLE_TIME                     (49*30)     //0xB6 32k ~3ms+250us
#define WIFI_PON_TIME                       (38*30)     //0xAB 32k
#define WF_DIGCK_ON_TIME                    (35*30)     //0x9F 32k
#define RS_BY_CMDBT_TIME                    (39*30)     //0x9F 32k
#define WAKEUP_DURATION                     (1860)       //0x168A
#define PRE_WAKEUP_OFFSET                   (590)        //0x282
#define REG_RESTORE_TIME                    (625)        //0xFA
#define RF_TURN_ON_DURATION                 (260)         //0x21  (SX_ON_TIME+10)
#define SX_ON_TIME                          (250)         //0x17

#define NUM_MAX_LP_INST_INIT_TO_SLEEP       30
#define NUM_MAX_LP_INST_WLANON_TO_SLEEP     10
#define NUM_MAX_LP_INST_SLEEP               40
#define NUM_MAX_LP_INST_WAKE                60
#define NUM_MAX_LP_INST_FULL_ON             10

#define NUM_MAX_LP_INST_EXTEND_INIT_TO_SLEEP           (NUM_MAX_LP_INST_INIT_TO_SLEEP+232)

#define WORKAOURND_NOT_ENOUGH_BACKUP_RESTORE_TABLE      0

#define MTCOMS_OFF_TIME                     200

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
typedef enum
{
    HAL_LP_INST_INIT_TO_SLEEP,
    HAL_LP_INST_WLAN_ON_TO_SLEEP,
    HAL_LP_INST_SLEEP_TO_WLAN_ON,
    HAL_LP_INST_WLAN_ON_TO_ACTIVE
} HAL_LP_INST_STATE_TRANSITION;

typedef enum
{
    HAL_TSF_SW_UPDATE_BY_OVERWRITE,
    HAL_TSF_SW_UPDATE_BY_ADJUST_OFFSET
} HAL_TSF_SW_UPDATE_MODE;

typedef enum
{
    HAL_TSF_HW_UPDATE_BY_TICK_AND_RECEIVED_FRAME,
    HAL_TSF_HW_UPDATE_BY_TICK_ONLY,
    HAL_TSF_HW_UPDATE_BY_RECEIVED_FRAME_ONLY,
    HAL_TSF_HW_UPDATE_BY_TICK_AND_RECEIVED_FRAME_AD_HOC
} HAL_TSF_HW_UPDATE_MODE;

typedef enum _ENUM_HW_TSF_TIMER_T {
    ENUM_TSF_TIMER_0,
    ENUM_TSF_TIMER_1,
    ENUM_TSF_TIMER_2,
    ENUM_TSF_TIMER_3,
    ENUM_TSF_TIMER_4,
    ENUM_TSF_TIMER_5,
    ENUM_TSF_TIMER_6,
    ENUM_TSF_TIMER_7,
    ENUM_TSF_TIMER_NUM
} ENUM_HW_TSF_TIMER_T, *P_ENUM_HW_TSF_TIMER_T;

typedef enum _ENUM_TSF_T {
    ENUM_LOCAL_TSF_0,
    ENUM_LOCAL_TSF_1,
    ENUM_LOCAL_TSF_2,
    ENUM_LOCAL_TSF_3,
    ENUM_LOCAL_TSF_NUM
} ENUM_LOCAL_TSF_T, *P_ENUM_LOCAL_TSF_T;

typedef enum {
    ENUM_MIN_TIME_CTRL_MDRDY,
    ENUM_MIN_TIME_CTRL_CCA,
    ENUM_MIN_TIME_NUM
} ENUM_MIN_TIME_CTRL_T;

typedef enum {
    ENUM_PF_HDR_NONE,
    ENUM_PF_HDR_IPV4_IP_CTRL,
    ENUM_PF_HDR_ARP_PATTERN_CTRL,
    ENUM_PF_HDR_GARP_IP_EQUAL_CTRL,
    ENUM_PF_HDR_NUM
} ENUM_PATTER_HEADER_T;

typedef enum {
    ENUM_PM_BSSID_0,
    ENUM_PM_BSSID_1,
    ENUM_PM_BSSID_2,
    ENUM_PM_BSSID_3,
    ENUM_PM_BSSID_NUM
} ENUM_PM_BSS_ID_T;

typedef enum {
    ENUM_BCNF_ID_0,
    ENUM_BCNF_ID_1,
    ENUM_BCNF_ID_2,
    ENUM_BCNF_ID_3,
    ENUM_BCNF_ID_NUM
} ENUM_PM_BCNF_ID_T;

typedef struct _REG_RESTORE_DIRECT_WRITE_T {
    UINT_32 u4RegAddr;
    UINT_32 u4RegValue;
} REG_RESTORE_DIRECT_WRITE_T, *P_REG_RESTORE_DIRECT_WRITE_T;

typedef VOID (*PFN_GET_DIRECT_WRITE_ENTRY)(UINT_32 u4EntryIdx, P_REG_RESTORE_DIRECT_WRITE_T prRegDirectWriteEntry);


typedef struct _HAL_PWR_MGT_CTRL_T {
    union {
        struct {
            UINT_16 u2WakeupTimeUs;
        } sHwMode;
        struct {
            UINT_16 u2GuardUs;
        } sSwMode;
    } u;

    UINT_16 u2AdditionalWakeupGuardUs; //in us
    UINT_16 u2RegisterRestoreTimeUs;    //Backup and Restore ?
    UINT_16 u2RegisterPreWakeupTimeUs;    // For Backup
    //UINT_16 u2WiFiPowerOnGuardTimeUs;  //PLL Stable Time (in 32k)
    UINT_16 u2PreTbttGuardUs;          //PTA need

    /* following is shown as ARM address decode spacing */
    UINT_32 u4InstAhbStartAddr;
    UINT_32 u4InstAhbEndAddr;
    UINT_32 u4RegisterBackupTableStartAddr;
    UINT_32 u4RegisterBackupTableEntryNum;
    UINT_32 u4NumOfBkupReg;
    PUINT_32 pu4RegBkupArray;

    // WiFi MIPS requirements
    UINT_8 ucMipsReq;

    UINT_16 u2HwPwrCtrlWiFiSysSramBank;
    UINT_16 u2SwPwrCtrlWiFiSysSramBank;
    UINT_16 u2HwPwrCtrlSramBank;
    UINT_16 u2SwPwrCtrlSramBank;

    UINT_32 u4HwPwrCtrlIlmBank;
    UINT_32 u4SwPwrCtrlIlmBank;
    UINT_32 u4HwPwrCtrlDlmBank;
    UINT_32 u4SwPwrCtrlDlmBank;
    UINT_32 u4HwPwrCtrlTcmBank;
    UINT_32 u4SwPwrCtrlTcmBank;

    UINT_32 u4PhyDbgDelayValue;

    UINT_8 ucSlowCntDuty;
    UINT_8 ucSlowCntDuty_10;
    UINT_8 ucSlowCntDuty_100;
    UINT_8 ucSlowCntDuty_1000;
    UINT_8 ucSlowCntDuty_10000;

    /* modified for ROM/ RAM dependency */
    PUINT_32 au4LowPowerInst_Init_to_sleep;
    PUINT_32 au4LowPowerInst_Wlanon_to_sleep;
    PUINT_32 au4LowPowerInst_wake;
    PUINT_32 au4LowPowerInst_full_on;

    PUINT_32 au4RegBkupRstr_CmdBT;
    UINT_32 u4NumRegBkup_CmdBT;

    UINT_32 u4NumRegDirectWriteOnWake_CmdBT;
    UINT_32 u4NumRegDirectWriteOnSleep_CmdBT;

    PUINT_32 au4RegBkupRstrAddr_SW;
    PUINT_32 au4RegBkupBuffer_SW;

    UINT_32 u4NumRegBkup_SW;
    UINT_32 u4NumRegDirectWrite_SW;

    P_REG_RESTORE_DIRECT_WRITE_T arRegDirectWrite_SW;
    P_REG_RESTORE_DIRECT_WRITE_T arRegDirectWriteOnSleep_CmdBT;
    P_REG_RESTORE_DIRECT_WRITE_T arRegDirectWriteOnWake_CmdBT;

#if OPTIMIZE_BACKUP_TIME // moved from hal_pwr_mgt.c
    PUINT_32 au4ExtraLowPowerInst_wlanon_to_sleep;
    PUINT_32 au4ExtraLowPowerInst_init_to_sleep;
    PUINT_32 au4ExtraLowPowerInst_wake;
    PUINT_32 au4ExtraLowPowerInst_full_on;

    UINT_8 ucMaxEntryNum_ExtraLowPowerInst_init_to_sleep;
    UINT_8 ucMaxEntryNum_ExtraLowPowerInst_wlanon_to_sleep;
    UINT_8 ucMaxEntryNum_ExtraLowPowerInst_wake;
#endif

    BOOLEAN fgMTCMOS_PD_EN;

    //UINT_32 u4RegTxQueueStatusBackup;

    union {
        struct {
            UINT_32 u4AddrTcmAlwaysOnBase;
            UINT_32 u4AddrTcmAlwaysOnLength;
        } rVonNeumann;
        struct {
            UINT_32 u4AddrIlmAlwaysOnBase;
            UINT_32 u4AddrIlmAlwaysOnLength;
            UINT_32 u4AddrDlmAlwaysOnBase;
            UINT_32 u4AddrDlmAlwaysOnLength;
        } rHarvard;
    } uMcuArch;

    UINT_32 u4AddrSramAlwaysOnBase;   //PSE
    UINT_32 u4AddrSramAlwaysOnLength;

} HAL_PWR_MGT_CTRL_T, *P_HAL_PWR_MGT_CTRL_T;



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
/* NOTE: ASSERTION of the _Base value should be done outside the macro
*/
#define CEILING_IN_UNIT_OF(_Value, _Base)    \
            (((UINT_32)(_Value) + (UINT_32)((_Base) - 1)) / (UINT_32)(_Base))

#define CEILING(_Value, _Base)    \
        ((((UINT_32)(_Value) + (UINT_32)((_Base) - 1)) / (UINT_32)(_Base)) * (UINT_32)(_Base))

#define FLOOR(_Value, _Base)    \
        ((((UINT_32)(_Value) ) / (UINT_32)(_Base)) * (UINT_32)(_Base))

/* DEALAY time in each of the clock procedures, with also consider the
   case of 32K slow clock time greater than the component stable time
*/
#if 0
#define LP_CLOCK_ON_DELAY_P1_US(_32kCcrSlowCnt)      \
            (g_u2OscStableTime_ROM > (UINT_16)_32kCcrSlowCnt) ? \
            (g_u2OscStableTime_ROM - (UINT_16)_32kCcrSlowCnt) : 0

#define LP_CLOCK_ON_DELAY_P2_US(_32kCcrSlowCnt)      \
            (g_u2RfSxStableTime_ROM > (UINT_16)_32kCcrSlowCnt) ? \
            (g_u2RfSxStableTime_ROM - (UINT_16)_32kCcrSlowCnt) : 0

#define LP_CLOCK_ON_DELAY_P3_US(_32kCcrSlowCnt)      \
            (g_u2RfSxStableTime_ROM + (UINT_16)_32kCcrSlowCnt)
#endif

/* unit: 64 us, Set to 256 us (> preamble).
   NOTE: This value should be set to a non-zero value for HW to transmit beacon!!
*/
#define PRE_TBTT_INTERVAL           4

#if (TARGET_PLATFORM == PLATFORM_FPGA)
#define SLOW_CLOCK_DUTY_US                  30
#define SLOW_CLOCK_DUTY_US_DEC_10           5
#define SLOW_CLOCK_DUTY_US_DEC_100          6
#define SLOW_CLOCK_DUTY_US_DEC_1000         2
#define SLOW_CLOCK_DUTY_US_DEC_10000        2

#else /*(TARGET_PLATFORM == PLATFORM_CHIP_V1)*/
#define SLOW_CLOCK_DUTY_US                  30
#define SLOW_CLOCK_DUTY_US_DEC_10           5
#define SLOW_CLOCK_DUTY_US_DEC_100          1
#define SLOW_CLOCK_DUTY_US_DEC_1000         7
#define SLOW_CLOCK_DUTY_US_DEC_10000        5

#endif

/*------------------------------------------------------------------------------
 * Macros for CMDBT instructions related functions
 *------------------------------------------------------------------------------
 */
#define CMDBT_INSTR_REG_WR_CODE          (0x21)

#if 0
#define CMDBT_INST_REG_32_WR_ADDR(_addr) \
                                    (CMDBT_INSTR_REG_WR_CODE | \
                                     (_addr   << 15) | \
                                     (_unitTu       << 14) | \
                                     (_value))
#define CMDBT_INST_REG_32_WR_DATA(_value) \
                                        (CMDBT_INSTR_REG_WR_CODE | \
                                         (_addr   << 15) | \
                                         (_unitTu       << 14) | \
                                         (_value))
#endif

/*------------------------------------------------------------------------------
 * Macros for Low Power instructions related functions
 *------------------------------------------------------------------------------
 */
#if 0 //marked for mt6630
#define LP_INSTR_CLK_OFF_CODE       ((UINT_32)0xD << 28)
#define LP_INSTR_CLK_ON_CODE        ((UINT_32)0xE << 28)
#define LP_INSTR_DELAY_CODE         ((UINT_32)0xF << 28)

#define LP_INSTR_DELAY_P2_SKIP      BIT(15)
#define LP_INSTR_DELAY_UNIT_TU      BIT(14)


/* _value is the "last" 16bit value of the data to be written,
   it will not shifted within the macro, if it is target to write "high" word.
*/

#define LP_INST_CLOCK_ON(_AS, _BS, _OS, _PS,    _A, _B, _O, _P,     _SS, _S, _CS, _C_32K,   _DS, _D_TU, _VS, _V, _delayCount) \
                            (LP_INSTR_CLK_ON_CODE | \
                                (_AS    << 27) | \
                                (_BS    << 26) | \
                                (_OS    << 25) | \
                                (_PS    << 24) | \
                                (_A     << 23) | \
                                (_B     << 22) | \
                                (_O     << 21) | \
                                (_P     << 20) | \
                                (_SS    << 19) | \
                                (_S     << 18) | \
                                (_CS    << 17) | \
                                (_C_32K << 16) | \
                                (_DS    << 15) | \
                                (_D_TU  << 14) | \
                                (_VS    << 13) | \
                                (_V     << 12) | \
                                ((_delayCount) & BITS(0, 11)))

#define LP_INST_CLOCK_OFF(_AS, _BS, _OS, _PS,   _A, _B, _O, _P,     _SS, _S, _CS, _C_32K,   _VS, _V) \
                            (LP_INSTR_CLK_OFF_CODE | \
                                (_AS    << 27) | \
                                (_BS    << 26) | \
                                (_OS    << 25) | \
                                (_PS    << 24) | \
                                (_A     << 23) | \
                                (_B     << 22) | \
                                (_O     << 21) | \
                                (_P     << 20) | \
                                (_SS    << 19) | \
                                (_S     << 18) | \
                                (_CS    << 17) | \
                                (_C_32K << 16) | \
                                (_VS    << 13) | \
                                (_V     << 12))

#define LP_INST_DELAY(_value, _phase2Skip, _unitTu) \
                                (LP_INSTR_DELAY_CODE | \
                                 (_phase2Skip   << 15) | \
                                 (_unitTu       << 14) | \
                                 (_value & BITS(0, 13)))
#endif

#define SLOW_CLOCK_COUNT_TO_SYSTEM_TIME_US(_clkCnt) \
                                ((_clkCnt)            * SLOW_CLOCK_DUTY_US + \
                                 (_clkCnt) / 10       * SLOW_CLOCK_DUTY_US_DEC_10 + \
                                 (_clkCnt) / 100      * SLOW_CLOCK_DUTY_US_DEC_100 + \
                                 (_clkCnt) / 1000     * SLOW_CLOCK_DUTY_US_DEC_1000 + \
                                 (_clkCnt) / 10000    * SLOW_CLOCK_DUTY_US_DEC_10000)
#if 0 //IOT_PROCESS_PORTING  
#if TCM_POWER_CONTROL
/* caller to call this function should be placed under the place of not SLEEP nor POWER DOWN
 * which is "WIFI_TCM_ALWAYS_ON"
 */
#define HAL_TCM_SW_ENABLE_POWER_SAVE(_prPwrMgtCtrl, _fgEn) \
    { \
        if (_fgEn) { \
            halPmSetTcmSwPowerCtrl( \
                0, \
                0, \
                0, \
                _prPwrMgtCtrl->u4SwPwrCtrlTcmBank); \
        } else { \
            halPmSetTcmSwPowerCtrl( \
                0, \
                0, \
                _prPwrMgtCtrl->u4SwPwrCtrlTcmBank, \
                0); \
        } \
    }
#else
#define HAL_TCM_SW_ENABLE_POWER_SAVE(_prPwrMgtCtrl, _fgEn)
#endif

#if WIFISYS_SRAM_POWER_CONTROL
#define HAL_WIFISYS_SRAM_SW_ENABLE_POWER_SAVE(_prPwrMgtCtrl, _fgEn) \
    { \
        if (_fgEn) { \
            halPmSetWiFiSysSramSwPowerCtrl( \
                0, \
                _prPwrMgtCtrl->u2SwPwrCtrlWiFiSysSramBank, \
                0, \
                0); \
        } else { \
            halPmSetWiFiSysSramSwPowerCtrl( \
                _prPwrMgtCtrl->u2SwPwrCtrlWiFiSysSramBank, \
                0, \
                0, \
                0); \
        } \
    }
#else
#define HAL_WIFISYS_SRAM_SW_ENABLE_POWER_SAVE(_prPwrMgtCtrl, _fgEn)
#endif

#if PSE_SRAM_POWER_CONTROL
#define HAL_SRAM_SW_ENABLE_POWER_SAVE(_prPwrMgtCtrl, _fgEn) \
    { \
        if (_fgEn) { \
            halPmSetPseSramSwPowerCtrl( \
                0, \
                _prPwrMgtCtrl->u2SwPwrCtrlSramBank, \
                0, \
                (PSE_ALL_SUBBANK & ~(_prPwrMgtCtrl->u2SwPwrCtrlSramBank))); \
        } else { \
            halPmSetPseSramSwPowerCtrl( \
                _prPwrMgtCtrl->u2SwPwrCtrlSramBank, \
                0, \
                (PSE_ALL_SUBBANK & ~(_prPwrMgtCtrl->u2SwPwrCtrlSramBank)), \
                0); \
        } \
    }
#else
#define HAL_SRAM_SW_ENABLE_POWER_SAVE(_prPwrMgtCtrl, _fgEn)
#endif

/* set 1 pattern only, for filtering IPv4 address of ARP packet */
#if 1
#define HAL_TEST_CFG_ARP_IPv4_ADDR_PTRN(_B3, _B2, _B1, _B0)
#endif

#if MTCMOS_WIFI_POWER_DOWN_CONTROL
#define HAL_ENABLE_WIFISYS_POWER_DOWN_CTRL(_fgEn) \
    { \
        halPmEnableWiFiSysPowerDownCtrl(_fgEn); \
    }
#else
#define HAL_ENABLE_WIFISYS_POWER_DOWN_CTRL(_fgEn)
#endif

#define HAL_RX_SX_PWR_CTRL(_fgEn) \
    { \
        if (_fgEn) { \
            HAL_REG_32(CONFG_LPCR_EN) = LPCR_EN_SW_EN_DIG_RF_SXON; \
        } else { \
            HAL_REG_32(CONFG_LPCR_DIS) = LPCR_DIS_SW_DIS_DIG_RF_SXON; \
        } \
    }

#define HAL_PLL_PWR_CTRL(_fgEn) \
    { \
        if (_fgEn) { \
        } else { \
        } \
    }
#endif/*IOT_PROCESS_PORTING*/
/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
#if (WIFI_UNI_DRIVER_RAM == 0) // WIFI UNIFY DRIVER support, for separation of ROM/ RAM function dependency
VOID
halPmInit (
    P_HAL_PWR_MGT_CTRL_T    prPwrMgtCtrl
    );

VOID
halPmLpPowerOnOffSeqInHwMode (
    BOOLEAN fgEnable
    );

VOID halPmPowerOnWifi (
    VOID
    );

VOID halPmPowerOffWifi (
    VOID
    );

VOID
halpmSetAdditionalWakeupGuardTime (
    UINT_16 u2AdditionalWakeupGuardUs
    );

extern VOID
(*halPmSetPreTbttInterval) (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN UINT_16  u2PreTbttIntvUs
    );

UINT_16
halPmGetPreTbttInterval (
    IN ENUM_PM_BSS_ID_T     eBssid
    );

VOID
halPmEnterLowPower (
    VOID
    );

BOOLEAN
halPmLeaveLowPower (
    VOID
    );

BOOLEAN
halPmGetLowPowerOwnership (
    VOID
    );

VOID
halPmResetBeaconServicePeriodState (
    IN ENUM_PM_BSS_ID_T     eBssid
    );

VOID
halPmResetBmcServicePeriodState (
    IN ENUM_PM_BSS_ID_T     eBssid
    );

VOID
halPmEnableBeaconServicePeriod (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN BOOLEAN fgEnable
    );

VOID
halPmEnableBmcServicePeriod (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN BOOLEAN fgEnable
    );

BOOLEAN
halPmGetBeaconServicePeriodState (
    IN ENUM_PM_BSS_ID_T     eBssid
    );

BOOLEAN
halPmGetBmcServicePeriodState (
    IN ENUM_PM_BSS_ID_T     eBssid
    );

VOID
halPmEnableBmcServicePeriodTimeout (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN BOOLEAN              fgEnable
    );

VOID
halPmEnableBeaconServicePeriodTimeout (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN BOOLEAN              fgEnable
    );

VOID
halPmEnableServicePeriodEndCheck (
    IN BOOLEAN              fgEnable
    );

VOID
halPmSetBmcTimeout (
    IN BOOLEAN      fgMinTimeoutValid,
    IN UINT_8       ucMinTimeoutValueTu,
    IN BOOLEAN      fgMaxTimeoutValid,
    IN UINT_16      u2MaxTimeoutValueTu,
    IN UINT_8       ucTimeoutCount
    );

VOID
halPmSetBeaconTimeout (
    IN BOOLEAN      fgMinTimeoutValid,
    IN UINT_8       ucMinTimeoutValueTu,
    IN BOOLEAN      fgMaxTimeoutValid,
    IN UINT_16      u2MaxTimeoutValueTu,
    IN UINT_8       ucTimeoutCount
    );

VOID
halPmConfigTimeoutExtension (
    IN ENUM_MIN_TIME_CTRL_T eMinTimeCtrl
    );

VOID
halPmResetTimeoutCount (
    VOID
    );

extern VOID
(*halPmConfigTbttBasedTimer) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN UINT_16              u2BeaconPeriod,
    IN UINT_8               ucDtimPeriod,
    IN UINT_8               ucTbttWakePeriod,
    IN UINT_8               ucDtimWakePeriod,
    IN BOOLEAN              fgEnableTrigger
    );

VOID
halPmSetDtimCount (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    UINT_8 ucDtimCount
    );

UINT_8
halPmGetDtimCount (
    IN ENUM_LOCAL_TSF_T     eTsfId
    );

extern UINT_16
(*halPmGetNextTbtt) (
    IN ENUM_LOCAL_TSF_T     eTsfId
    );

extern UINT_16
(*halPmEnableCalculateNextTbtt) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );

extern VOID
(*halPmEnableTbttTimer) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable,
    IN BOOLEAN              fgReCalNextTbttTime
    );

extern VOID
(*halPmEnableTbttTimeupEvent) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );

extern VOID
(*halPmEnablePreTbttTimeupEvent) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );

VOID
halPmEnablePreTbttTimeupHw (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );

VOID
halPmConfigTbttEarlierOffset (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN UINT_8              ucTbttEarlierOffset
    );

BOOL
halPmConfigT0_T7 (
    IN ENUM_HW_TSF_TIMER_T  eTimerId,
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN UINT_32              u4NextStartTime,
    IN UINT_32              u4TimerPeriod
    );

BOOL
halPmEnableT0_T7 (
    IN ENUM_HW_TSF_TIMER_T  eTimerId,
    IN BOOLEAN              fgEnable
    );

BOOL
halPmConfigT8 (
    IN BOOLEAN              fgPeriodicalMode,
    IN UINT_32              u4TimerPeriod
    );

BOOL
halPmEnableT8 (
    IN BOOLEAN              fgEnable
    );

VOID
halPmSetTsfDriftWindow (
    IN ENUM_LOCAL_TSF_T    eTsfId,
    IN UINT_8    ucDriftWindow
    );

extern VOID
(*halPmSetTsfHardwareUpdateMode) (
    IN ENUM_LOCAL_TSF_T    eTsfId,
    IN HAL_TSF_HW_UPDATE_MODE eMode
    );

VOID
halPmSetTsfSoftwareUpdateMode (
    IN ENUM_LOCAL_TSF_T    eTsfId,
    IN HAL_TSF_SW_UPDATE_MODE eMode,
    IN UINT_32  au4Value[]
    );

VOID
halPmGetCurrentTsf (
    IN ENUM_LOCAL_TSF_T    eTsfId,
    IN OUT UINT_32  au4Value[]
    );

VOID
halPmResetTSFTimer (
    IN ENUM_LOCAL_TSF_T    eTsfId
    );

UINT_16
halPmGetBeaconEarlyValue (
    VOID
    );

VOID
halPmEnableBeaconEarlyValueDetection (
    IN BOOLEAN              fgEnable
    );

VOID
halPmEnableParseBeaconTimFunc (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN BOOLEAN              fgEnable
    );

VOID
halPmEnableParseAndHandleBmcFunc (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN BOOLEAN              fgEnable
    );

VOID
halPmEnableBeaconContentCheck (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN BOOLEAN              fgEnable
    );

VOID
halPmBeaconContentCheck_SetP2pIeOuiType (
    IN UINT_8   ucOuiType
    );

VOID
halPmEnableBcnChkForSkippingHtPcoPhase (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN BOOLEAN              fgEnable
    );

VOID
halPmSetBssidAID (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN UINT_16              u2Aid
    );

extern VOID
(*halPmDelayUs) (
    UINT_32 u4Delay
    );

VOID
halPmCfgTimerWakeupGuardTimeInSwMode (
    UINT_16 u2OscStableTimeUs,
    UINT_16 u2AnalogStableTimeUs,
    UINT_16 u2AdditionalWakeupGuardUs
    );


#if (PRODUCT_VERSION == 6630 || PRODUCT_VERSION == 7636 || PRODUCT_VERSION == 7637 || PRODUCT_VERSION == 7687)
VOID
halPmSetTcmPowerCtrl (
    UINT_32 u4BanksForceToPwrOn,
    UINT_32 u4BanksAbleToPwrOff,
    UINT_32 u4BanksForceToWakeup,
    UINT_32 u4BanksAbleToSleep,
    UINT_32 u4BanksHwPwrOffCapable,
    UINT_32 u4BanksHwSleepCapable
    );

VOID
halPmSetTcmHwPowerCtrl (
    UINT_32 u4BanksHwPwrOffCapable,
    UINT_32 u4BanksHwSleepCapable
    );

VOID
halPmSetTcmSwPowerCtrl (
    UINT_32 u4BanksForceToPwrOn,
    UINT_32 u4BanksAbleToPwrOff,
    UINT_32 u4BanksForceToWakeup,
    UINT_32 u4BanksAbleToSleep
    );

VOID
halPmGetTcmPowerState (
    PUINT_32 pu4PowerDownState,
    PUINT_32 pu4SleepState
    );

VOID
halPmSetPseSramPowerCtrl (
    UINT_16 u2BanksForceToPwrOn,
    UINT_16 u2BanksAbleToPwrOff,
    UINT_16 u2BanksForceToWakeup,
    UINT_16 u2BanksAbleToSleep,
    UINT_16 u2BanksHwPwrCtrlCapable,
    UINT_16 u2BanksHwSleepCtrlCapable
    );

VOID
halPmSetPseSramHwPowerCtrl (
    UINT_16 u2BanksHwPwrCtrlCapable,
    UINT_16 u2BanksHwSleepCtrlCapable
    );

VOID
halPmSetPseSramSwPowerCtrl (
    UINT_16 u2BanksForceToPwrOn,
    UINT_16 u2BanksAbleToPwrOff,
    UINT_16 u2BanksForceToWakeup,
    UINT_16 u2BanksAbleToSleep
    );

VOID
halPmGetPseSramPowerState (
    PUINT_32 pu4PowerDownState,
    PUINT_32 pu4SleepState
    );

#elif (PRODUCT_VERSION == 5931 || PRODUCT_VERSION == 6628 || PRODUCT_VERSION == 6620)
// ILM part
VOID
halPmSetIlmPowerCtrl (
    UINT_32 u4BanksForceToPwrOn,
    UINT_32 u4BanksAbleToPwrOff,
    UINT_32 u4BanksForceToWakeup,
    UINT_32 u4BanksAbleToSleep,
    UINT_32 u4BanksHwPwrOffCapable,
    UINT_32 u4BanksHwSleepCapable
    );

VOID
halPmSetIlmHwPowerCtrl (
    UINT_32 u4BanksHwPwrOffCapable,
    UINT_32 u4BanksHwSleepCapable
    );

VOID
halPmSetIlmSwPowerCtrl (
    UINT_32 u4BanksForceToPwrOn,
    UINT_32 u4BanksAbleToPwrOff,
    UINT_32 u4BanksForceToWakeup,
    UINT_32 u4BanksAbleToSleep
    );

VOID
halPmGetIlmPowerState (
    PUINT_32 pu4PowerDownState,
    PUINT_32 pu4SleepState
    );

// DLM part
VOID
halPmSetDlmPowerCtrl (
    UINT_32 u4BanksForceToPwrOn,
    UINT_32 u4BanksAbleToPwrOff,
    UINT_32 u4BanksForceToWakeup,
    UINT_32 u4BanksAbleToSleep,
    UINT_32 u4BanksHwPwrOffCapable,
    UINT_32 u4BanksHwSleepCapable
    );

VOID
halPmSetDlmHwPowerCtrl (
    UINT_32 u4BanksHwPwrOffCapable,
    UINT_32 u4BanksHwSleepCapable
    );

VOID
halPmSetDlmSwPowerCtrl (
    UINT_32 u4BanksForceToPwrOn,
    UINT_32 u4BanksAbleToPwrOff,
    UINT_32 u4BanksForceToWakeup,
    UINT_32 u4BanksAbleToSleep
    );

VOID
halPmGetDlmPowerState (
    PUINT_32 pu4PowerDownState,
    PUINT_32 pu4SleepState
    );
#endif

VOID
halPmGetWiFiSysSramPowerState (
    PUINT_16 pu2PowerDownState,
    PUINT_16 pu2SleepState
    );

VOID
halPmSetWiFiSysSramPowerCtrl (
    UINT_16 u2BanksForceToPwrOn,
    UINT_16 u2BanksAbleToPwrOff,
    UINT_16 u2BanksForceToWakeup,
    UINT_16 u2BanksAbleToSleep,
    UINT_16 u2BanksHwPwrCtrlCapable,
    UINT_16 u2BanksHwSleepCtrlCapable
    );

VOID
halPmSetWiFiSysSramHwPowerCtrl (
    UINT_16 u2BanksHwPwrCtrlCapable,
    UINT_16 u2BanksHwSleepCtrlCapable
    );

VOID
halPmSetWiFiSysSramSwPowerCtrl (
    UINT_16 u2BanksForceToPwrOn,
    UINT_16 u2BanksAbleToPwrOff,
    UINT_16 u2BanksForceToWakeup,
    UINT_16 u2BanksAbleToSleep
    );

VOID
halPmPreConfigRAMPwrMgtCtrl (
    UINT_32 u4PreConfigPwrCtrlIlmBank,
    UINT_32 u4PreConfigPwrCtrlDlmBank,
    UINT_16 u2PreConfigPwrCtrlSramBank
    );

VOID
halPmEnableWiFiSysPowerDownCtrl (
    BOOLEAN fgEnable
    );

#if (PRODUCT_VERSION == 6620)
VOID
halPmCfgTimerWakeupGuardTimeInHwMode (
    UINT_16 u2RegisterRestoreTimeUs,
    UINT_16 u2AdditionalWakeupGuardUs
    );
#elif (PRODUCT_VERSION == 5931 || PRODUCT_VERSION == 6628 || PRODUCT_VERSION == 6630 || PRODUCT_VERSION == 7636 || PRODUCT_VERSION == 7637 || PRODUCT_VERSION == 7687)
VOID
halPmCfgTimerWakeupGuardTimeInHwMode (
    UINT_16 u2RegisterRestoreTimeUs,
    UINT_16 u2AdditionalWakeupGuardUs
    );
//#else ==> avoid build fail for rom_patch.c to include ROM header file
//#error "undefined chip version for configuring HW power on sequence parameters"
#endif

UINT_16
halPmGetLatestBeaconContentHashValue (
    IN ENUM_PM_BSS_ID_T     eBssid
    );

#if IGNORE_WIFI_TASK_TO_DO_WIFISYS_POWER_ON_OFF
VOID
halPmPowerOnWiFiSubSys_ext (
    VOID
    );

VOID
halPmPowerOffWiFiSubSys_ext (
    VOID
    );
#endif

VOID
halPmRegBackup_SW (
    VOID
    );

VOID
halPmRegRestore_SW (
    VOID
    );

VOID
halPmRegGetDirectWriteEntryOnWake_CmdBT(
    UINT_32 u4EntryIdx,
    P_REG_RESTORE_DIRECT_WRITE_T prRegDirectWriteEntry
    );

VOID
halPmRegGetDirectWriteEntryOnSleep_CmdBT(
    UINT_32 u4EntryIdx,
    P_REG_RESTORE_DIRECT_WRITE_T prRegDirectWriteEntry
    );

VOID
halPmEnableMTCMOS_PowerDownFunc (
    BOOLEAN fgEnable
    );

VOID
halPmPowerOnWiFiMemory (
    VOID
    );

VOID
halPmPowerOnWiFiSubSys (
    VOID
    );

VOID
halPmPowerOFFWiFiMemory (
    VOID
    );

VOID
halPmPowerOffWiFiSubSys (
    VOID
    );

VOID
halPmProgLowPwrInstForRegisterBackupRestore (
    IN UINT_32      u4NumOfRegisters,
    IN UINT_32      au4RegisterArray[]
    );

VOID
halPmSetSlowClockCalirationValue (
    UINT_8 uc32K_CNT,
    UINT_8 uc32K_CNT_1_DEC,
    UINT_8 uc32K_CNT_2_DEC,
    UINT_8 uc32K_CNT_3_DEC,
    UINT_8 uc32K_CNT_4_DEC
    );

UINT_32
halpmUtilGetPwrSaveCapableIlmBank (
    UINT_32 u4AlwaysOnStartAddr,
    UINT_32 u4AlwaysOnLen
    );

UINT_32
halpmUtilGetPwrSaveCapableDlmBank (
    UINT_32 u4AlwaysOnStartAddr,
    UINT_32 u4AlwaysOnLen
    );

UINT_32
halpmUtilGetPwrSaveCapablePseSramBank (
    UINT_32 u4AlwaysOnStartAddr,
    UINT_32 u4AlwaysOnLen
    );

VOID
halPmProgLowPwrInst (
    IN UINT_8       uc32kSlowCount,
    IN UINT_8       ucNumExtraInit_to_SleepEntry,
    IN UINT_8       ucNumExtraWlanon_to_SleepEntry,
    IN UINT_8       ucNumExtraWakeEntry,
    IN UINT_8       ucNumExtraOnEntry,
    IN UINT_32      au4ExtraLowPowerInst_Init_to_sleep[],
    IN UINT_32      au4ExtraLowPowerInst_Wlanon_to_sleep[],
    IN UINT_32      au4ExtraLowPowerInst_wake[],
    IN UINT_32      au4ExtraLowPowerInst_full_on[],
    IN UINT_32      u4InstStartOffset
    );

extern VOID
(*halPmEnablePreTbttTrigger) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );

VOID
halPmConfigTcmSramStableTime (
    UINT_8 ucPwrOnStableTime,
    UINT_8 ucWakeupStableTime
    );

VOID
halPmSetNextTbtt (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    UINT_16 u2NextTbttTime
    );

extern VOID
(*halPmEnablePreDtimTrigger) (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );

VOID
halPmProgLowPwrInstForRegisterDirectWrite (
    HAL_LP_INST_STATE_TRANSITION eLpInstState,
    PUINT_8 pucNumExtraInstEntry
    );

extern UINT_32
(*halPmGetWifiSysUpTime) (
    VOID
    );

#if 1 // make wlan_lib.h macro to function, which is to wrap used variable inside function, without export to RAM (simply for eliminate symbol dependency)
VOID
halPmWlanClaimMcuUsage (
    P_HAL_PWR_MGT_CTRL_T    prPwrMgtCtrl,
    BOOLEAN fgUseWifisysPll,
    BOOLEAN fgEn
    );

VOID
halPmEnWifiMemPowerCtrl (
    BOOLEAN fgEnTcmPowerCtrl,
    BOOLEAN fgEnWifiSysPowerCtrl
    );

VOID
halPmWlanSetMcuSleepMode (
    BOOLEAN fgEn
    );
#endif

/* SMCTL */
VOID
halPmEnableWiFiSysSramHwPowerCtrl (
    IN BOOLEAN fgEnable
    );

VOID
halPmSetPseSramPowerOnAsWlanOn (
    IN BOOLEAN fgWlanOnTrigger
    );

VOID
halPmEnableWFRestorebyHclk (
    IN BOOLEAN fgEnable
    );

VOID
halPmSetBusyWaitBeforeSleep (
    IN UINT_32      u4BusyWaitCycleCnt
    );

VOID
halPmSetStableTime(
    IN UINT_32      u4OscStableTime,
    IN UINT_32      u4WiFiPONTime,
    IN UINT_32      u4PllStableTime,
    IN UINT_32      u4DigCKOnTime
    );

VOID
halPmSetCMDBTRestoreTime(
    IN UINT_32      u4RsByCMDBTTime,
    IN UINT_32      u4RegRestoreTime
    );

VOID
halPmSetSX2P4GUsage(
    IN BOOLEAN fgEnable
    );

VOID
halPmSetPreWakeupDurationTime(
    IN UINT_32      u4WakeupDuration,
    IN UINT_32      u4PreWakeupOffset
    );

VOID
halPmSetRfTurnOnAndSXOnTime(
    IN UINT_32      u4RfTurnOnDuration,
    IN UINT_32      u4SXOnTime
    );

VOID
halPmSetSXOffGuardTime(
    IN UINT_32      u4SXOffGuardTime
    );

VOID
halPmSetSleepCtrlAnaOpPhase (
    IN UINT_32      u4PhaseNumber
    );

BOOLEAN
halPmGetWiFiPowerOffEvent(
    VOID
    );

VOID
halPmClearWiFiPowerOffEvent(
    VOID
    );


BOOLEAN
halPmGetWiFiRxPathBusy (
    VOID
    );

/* New Features */
VOID
halPmSetBeaconFilter (
    IN UINT_16  u2FilterId,
    IN UINT_16  u2FilterEid,
    IN BOOLEAN  fgEnable
    );

/* NPS: TIM Broadcast */
VOID
halPmSetPreTtttInterval (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN UINT_16  u2PreTtttIntvUs
    );

UINT_16
halPmGetPreTtttInterval (
    IN ENUM_PM_BSS_ID_T     eBssid
    );

VOID
halPmResetTimServicePeriodState (
    IN ENUM_PM_BSS_ID_T     eBssid
    );
VOID
halPmResetTimBmcServicePeriodState (
    IN ENUM_PM_BSS_ID_T     eBssid
    );

VOID
halPmEnableTimServicePeriod (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN BOOLEAN              fgEnable
    );

VOID
halPmEnableTimBmcServicePeriod (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN BOOLEAN              fgEnable
    );

BOOLEAN
halPmGetTimServicePeriodState (
    IN ENUM_PM_BSS_ID_T     eBssid
    );

BOOLEAN
halPmGetTimBmcServicePeriodState (
    IN ENUM_PM_BSS_ID_T     eBssid
    );

VOID
halPmEnableTimBmcServicePeriodTimeout (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN BOOLEAN              fgEnable
    );

VOID
halPmEnableTimServicePeriodTimeout (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN BOOLEAN              fgEnable
    );

VOID
halPmSetTimBmcTimeout (
    IN BOOLEAN      fgMinTimeoutValid,
    IN UINT_8       ucMinTimeoutValueTu,  // unit of TU, valid: 0 (no timeout) to 63
    IN BOOLEAN      fgMaxTimeoutValid,
    IN UINT_16      u2MaxTimeoutValueTu,  // unit of TU, valid: 0 (no timeout) to 255 * 4
    IN UINT_8       ucTimeoutCount      // valid: 0 (no timeout) to 15
    );

VOID
halPmSetTimTimeout (
    IN BOOLEAN      fgMinTimeoutValid,
    IN UINT_8       ucMinTimeoutValueTu,  // unit of TU, valid: 0 (no timeout) to 63
    IN BOOLEAN      fgMaxTimeoutValid,
    IN UINT_16      u2MaxTimeoutValueTu,  // unit of TU, valid: 0 (no timeout) to 255 * 4
    IN UINT_8       ucTimeoutCount      // valid: 0 (no timeout) to 63
    );

VOID
halPmResetTimTimeoutCount (
    VOID
    );

VOID
halPmEnablePreTtttTrigger (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );

VOID
halPmConfigTtttBasedTimer (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN UINT_32              u4TimOffset, // 2's complement, -131072~131071
    IN UINT_16              u2TimPeriod, // TIM Period = TTTT Interval x Beacon interval
    IN BOOLEAN              fgEnableTrigger
    );

UINT_32
halPmGetNextTttt (
    IN ENUM_LOCAL_TSF_T     eTsfId
    );

UINT_32
halPmEnableCalculateNextTttt (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );

VOID
halPmSetNextTttt (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    UINT_32 u4NextTbttTime
    );

VOID
halPmConfigCriticalBcnWakeupNextDtim (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgConfigDtim
    );

VOID
halPmEnableTtttTimer (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable,
    IN BOOLEAN              fgReCalNextTbttTime
    );

VOID
halPmEnableTtttTimeupEvent (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );

VOID
halPmEnablePreTtttTimeupEvent (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );

VOID
halPmEnablePreTtttTimeupHw (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN BOOLEAN              fgEnable
    );

VOID
halPmConfigTtttEarlierOffset (
    IN ENUM_LOCAL_TSF_T     eTsfId,
    IN UINT_8               ucTtttEarlierOffset
    );

VOID
halPmEnableParseTimBroadcastTimFunc (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN BOOLEAN              fgEnable
    );

VOID
halPmEnableParseAndHandleTimBroadcastBmcFunc (
    IN ENUM_PM_BSS_ID_T     eBssid,
    IN BOOLEAN              fgEnable
    );

#endif //#if (WIFI_UNI_DRIVER_RAM == 0) // WIFI UNIFY DRIVER support, for separation of ROM/ RAM function dependency

#endif /* _HAL_PM_ROM_H */



