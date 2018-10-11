/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
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

#ifndef _COMBO_SFI_DEFS_H
#define _COMBO_SFI_DEFS_H

#include "hal_flash_sfi_release.h"
//-----------------------------------------------------------------------------
// Combo MEM HW Settings
//-----------------------------------------------------------------------------
#define COMBO_SFI_VER   1



#define _SFI_SIP_SerialFlash

#if defined(_SFI_SIP_SerialFlash)
#define SFI_COMBO_COUNT    5
#elif defined(__COMBO_MEMORY_SUPPORT__)
#if !defined(__DUAL_SERIAL_FLASH_SUPPORT__)
#define SFI_COMBO_COUNT    COMBO_MEM_ENTRY_COUNT
#else
#define SFI_COMBO_COUNT    DUAL_COMBO_MEM_ENTRY_COUNT
#endif
#else
#define SFI_COMBO_COUNT    1
#endif

typedef struct {
#if defined(__SFI_CLK_166MHZ__)
    uint32_t      HWConf_166M[10];    // to be defined by SFI/EMI owner
    uint8_t       DevInit_166M[32];   // to be defined by SFI/EMI owner  (reserved for serial flash)
#endif //defined(__SFI_CLK_166MHZ__)
#if defined(__SFI_CLK_130MHZ__)
    uint32_t      HWConf_130M[10];    // to be defined by SFI/EMI owner
    uint8_t       DevInit_130M[32];   // to be defined by SFI/EMI owner  (reserved for serial flash)
#endif //defined(__SFI_CLK_130MHZ__)
#if defined(__SFI_CLK_104MHZ__)
    uint32_t      HWConf_104M[10];    // to be defined by SFI/EMI owner
    uint8_t       DevInit_104M[32];   // to be defined by SFI/EMI owner  (reserved for serial flash)
#endif //defined(__SFI_CLK_104MHZ__)
#if defined(__SFI_CLK_78MHZ__)
    uint32_t      HWConf_78M[10];     // to be defined by SFI/EMI owner
    uint8_t       DevInit_78M[32];    // to be defined by SFI/EMI owner  (reserved for serial flash)
#endif //defined(__SFI_CLK_78MHZ__)
    uint32_t      HWConf_26M[8];      // to be defined by SFI/EMI owner

} CMEMEntrySFI;

typedef struct {
#if defined(__SFI_CLK_166MHZ__)
    uint32_t      HWConf_166M[5];     // to be defined by SFI/EMI owner
#endif //defined(__SFI_CLK_166MHZ__)
#if defined(__SFI_CLK_130MHZ__)
    uint32_t      HWConf_130M[5];     // to be defined by SFI/EMI owner
#endif //defined(__SFI_CLK_130MHZ__)
#if defined(__SFI_CLK_104MHZ__)
    uint32_t      HWConf_104M[5];     // to be defined by SFI/EMI owner
#endif //defined(__SFI_CLK_104MHZ__)
#if defined(__SFI_CLK_78MHZ__)
    uint32_t      HWConf_78M[5];      // to be defined by SFI/EMI owner
#endif //defined(__SFI_CLK_78MHZ__)
    uint32_t      HWConf_26M[3];      // to be defined by SFI/EMI owner
} CMEMEntrySFI_DCM;


typedef struct {
    uint32_t      HWConf[16];         // to be defined by EMI owner
} CMEMEntryEMI;

typedef struct {
    char               m_identifier[16];   // MTK_COMBO_ID_INFO
    unsigned int       m_ver;
    unsigned int       Count;
    CMEMEntrySFI       List[SFI_COMBO_COUNT];  // to be defined by SFI/EMI owner
} CMEMEntrySFIList;

typedef struct {
    CMEMEntrySFI_DCM      List[SFI_COMBO_COUNT];  // to be defined by SFI/EMI owner
} CMEMEntrySFIList_dcm;

#endif

