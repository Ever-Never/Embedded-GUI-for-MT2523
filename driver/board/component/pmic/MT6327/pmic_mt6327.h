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

#ifndef __PMIC_MT6237_H__
#define __PMIC_MT6237_H__

#include <stdio.h>
#include <stdlib.h>
#include "hal_platform.h"
#include "external_pmic_driver.h"

#define MT6327_DEVICE_ID 	0x60//0xC0 0xC1
#define MT6327_CHIP_ID	0x27


typedef enum {
    EXT_PMIC_MT6327_VMD,
    EXT_PMIC_MT6327_VRF18,
    EXT_PMIC_MT6327_VTCXO,
    EXT_PMIC_MT6327_VA25,
    EXT_PMIC_MT6327_VM,
    EXT_PMIC_MT6327_VSIM1,
    EXT_PMIC_MT6327_VIO18,
    EXT_PMIC_MT6327_VMC
} external_pmic_mt6327_vr_t;

typedef enum {
    EXT_PMIC_MAXIM_CHARGER_OFF_MODE = 0,
    EXT_PMIC_MAXIM_CHARGING_SUSPEND_MODE,
    EXT_PMIC_MAXIM_PRE_CHARGE_MODE,
    EXT_PMIC_MAXIM_FAST_CHARGE_CC_MODE,
    EXT_PMIC_MAXIM_FAST_CHARGE_CV_MODE,
    EXT_PMIC_MAXIM_MAINTAIN_CHARGE_MODE,
    EXT_PMIC_MAXIM_MAINTAIN_CHARGE_DONE_MODE,
    EXT_PMIC_MAXIM_CHARGER_FAULT_MODE
} external_pmic_mt6327_status_of_charger_mode_t;

void pmic_init_mt6327(void);
void pmic_charger_restore_setup(void);
unsigned char pmic_charger_get_charging_status(void);
unsigned char pmic_charger_get_charger_exist(void);
external_pmic_status_t pmic_ctrl_power_mt6327(external_pmic_mt6327_vr_t power_name, external_pmic_vr_ctl_t enable);
external_pmic_status_t pmic_vosel_config_mt6327(external_pmic_mt6327_vr_t power_name, external_pmic_voltage_t voltage);

#endif

