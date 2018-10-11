/*
** $Id: //Department/DaVinci/TRUNK/MT6628_FPGA_BORA_BASE_20111109/mcu/wifi/ROM/include/precomp_rom.h#1 $
*/

/*! \file   precomp_rom.h
    \brief  Collection of most compiler flags are described here.

    In this file we collect all compiler flags and detail the driver behavior if
    enable/disable such switch or adjust numeric parameters.
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
** $Log: precomp_rom.h $
**
** 05 15 2015 raju.singh
** [BORA00004254] [MT7637E2][IOT][FW]
** IOT code with STA+promiscus mode and AP mode with APCLI and Repeater feature.
 *
 * 05 09 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Integrated Branch for AP-STA.
**
** 12 15 2013 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Refine Feature Compile Option
**
** 10 29 2013 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Not Support Calibration on ROM and With WMT
**
** 06 14 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** Add utility rom API and remove unused API
**
** 06 03 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** Change the GTK oddload function defined name
**
** 05 28 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** Add compilong flag
**
** 05 28 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** Add GTK offload related function s MD5, SHA1 and unwrap to ROM.
**
** 05 13 2013 bruce.kang
** [BORA00002409] [MT6630 Wi-Fi]
** 1. Add interfaces between WMT and Wi-Fi calibration module (rlm)
** 2. Add Rom to Ram API
** 3. Add debug count in bcm module
** 4. Add TX CF-End in bss module
**
** 05 03 2013 bruce.kang
** [BORA00002409] [MT6630 Wi-Fi]
** Fix the include error for bcm_rom.c
**
** 03 27 2013 cp.wu
** [BORA00002220] [MT6630 Wi-Fi] Draft FW for HIFSYS update and ROM code logic regarding HIFSYS access
** share the same ISR for LMAC/PSE/Wi-Fi HIF interrupts
**
** 01 31 2013 pat.lu
** [BORA00002365] [WLAN] Sec. and WTBL FPGA Emulation Modification
** .
** Update the code arrangement for debug UART Port
** Add dbg_putUARTByte to ROM code export APP header file
**
** 10 23 2012 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Avoid duplicated definition or type in ROM/RAM header files
**
** 08 06 2012 yarco.yang
** [BORA00002091] [MT6630]Support 6630 complier flag
*/

#ifndef _PRECOMP_ROM_IOT_H
#define _PRECOMP_ROM_IOT_H

// ======= This is a copy from precomp_rom.h , but remove stdio/stdlib/stdarg ========


/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/
#include "config_rom.h"     /* Place at 1st for _lint define */

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*------------------------------------------------------------------------------
 * Not in .\wifi\include
 *------------------------------------------------------------------------------
 */
LINT_SAVE_AND_DISABLE
#include "cos_config.h"
#include "cos_struct.h"
#include "cos_trc.h"
#include "cos_api.h"
#include "sys_config.h"
#include "config_hw.h"
#include "config_sw.h"

#include "intrCtrl.h"       // system\init\include
#include "Eint.h"           // system\init\include
#include "uart_sw.h"        // system\init\include
#include "hif_drv.h"        // driver\include
#include "wmt_task.h"       // wmt\include
LINT_RESTORE

#include "hif_hw.h"         // driver\include


/*------------------------------------------------------------------------------
 * .\include - Common
 *------------------------------------------------------------------------------
 */
#include "gl_typedef_rom.h"
#include "gl_kal_rom.h"

#include "typedef_rom.h"


#if WIFI_BUILD_RAM == 0
//#include "debug_rom.h"      /* Not support debug message for ROM code now */
#endif

#include "dump_rom.h"

#include "link_rom.h"
#include "queue_rom.h"

#include "utility_rom.h"
/*------------------------------------------------------------------------------
 * .\include\nic\hal
 *------------------------------------------------------------------------------
 */
#include "hal_top_rom.h"
#include "hal_callback_rom.h"

/*------------------------------------------------------------------------------
 * .\include\MGMT
 *------------------------------------------------------------------------------
 */
#include "rlm_phy_rom.h"
#include "bcm_rom.h"
#if CFG_GROUP_REKEY_OFFLOAD_CRYPTO_API
#include "crypto_i.h"
#endif

/*------------------------------------------------------------------------------
 * .\include
 *------------------------------------------------------------------------------
 */
#include "wlan_lib_rom.h"

/*------------------------------------------------------------------------------
 * .\include\nic
 *------------------------------------------------------------------------------
 */
#include "nic_rom.h"


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
/* NOTE: put external reference for LINT here */
#ifdef _lint
cos_timer_type* cos_create_timer(task_indx_type dest_id);
void cos_start_timer(cos_timer_type *timer, kal_uint32 timeout);
void cos_stop_timer(cos_timer_type *timer);

#define INDX_WIFI       1       /* Define in system\config\include\cos_config.h */
#define IRQ_WIFI_CODE   (11)    /* Define in system\init\include\intrCtrl.h */
#define HIF_base        0       /* Define in system\init\include\reg_base.h */
#define MSG_ID_WIFI_RESET   1
#endif

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

#endif /* _PRECOMP_ROM_H */


