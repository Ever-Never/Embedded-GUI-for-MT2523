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

#ifndef __hal_pmu_internal_H__
#define __hal_pmu_internal_H__

#include "hal_pmu_wrap_interface.h"
#include "hal_mt2523_pmu_platform.h"
#include "hal_platform.h"

#ifdef HAL_PMU_MODULE_ENABLED

typedef enum {
    PMIC_VCLDO,
    PMIC_VIO18,
    PMIC_VSF,
    PMIC_VIO28,
    PMIC_VDIG18,
    PMIC_VA18,
    PMIC_VUSB,
    PMIC_VBT,
    PMIC_VA28,
    PMIC_VMC,
    PMIC_VCAMA,
    PMIC_VIBR,
    PMIC_SWXM,
    PMIC_SWDP,
    PMIC_SWMP
} PMIC_VR;

typedef enum {
    PMIC_PSI_OVR,
    PMIC_PSI_HP,
    PMIC_PSI_LP,
    PMIC_PSI_S0,
    PMIC_PSI_S1,
    PMIC_PSI_OFF,
    PMIC_PSI_SLP,
    PMIC_PSI_ACT
} PMIC_PSI;

typedef enum {
    PMIC_LDO_PMOD_NORMAL  = 0,
    PMIC_LDO_PMOD_LITE    = 1,
    PMIC_LDO_PMOD_LP      = 2,
    PMIC_LDO_PMOD_ULP     = 3
} PMIC_LDO_PMOD;

typedef enum {
    PMIC_SLP_WKUP_HW_CTL  = 0,
    PMIC_SLP_WKUP_SW_CTL  = 1,
    PMIC_SLP_WKUP_AON     = 2
} PMIC_SLP_WKUP_MODE;

typedef enum {
    PMIC_VR_CTL_DISABLE   = 0,
    PMIC_VR_CTL_ENABLE    = 1
} PMIC_VR_CTL;

typedef enum {
    PMIC_VCORE_0P7V	= 0,
    PMIC_VCORE_0P8V	= 1,
    PMIC_VCORE_0P9V	= 2,
    PMIC_VCORE_1P0V	= 3,
    PMIC_VCORE_1P1V	= 4,
    PMIC_VCORE_1P2V	= 5,
    PMIC_VCORE_1P3V	= 6,
    PMIC_VCORE_ERROR  = 255
} PMIC_VCORE_VOSEL;

typedef enum {
    PMIC_LDO_1P3V = 0,
    PMIC_LDO_1P5V = 1,
    PMIC_LDO_1P8V = 2,
    PMIC_LDO_2P0V = 3,
    PMIC_LDO_2P5V = 4,
    PMIC_LDO_2P8V = 5,
    PMIC_LDO_3P0V = 6,
    PMIC_LDO_3P3V = 7
} PMIC_LDO_VOSEL;

typedef enum {
    PMIC_LEAVE_OVR_MODE = 0,
    PMIC_ENTER_OVR_MODE = 1
} PMIC_SET_OVR_MODE;

typedef enum {
    PMIC_UNLOCK_KEY = 0,
    PMIC_LOCK_KEY = 1
} PMIC_SET_KEY;

void PMIC_VR_PSI_PMOD_CONFIG(PMIC_VR vr_name, PMIC_PSI psi_mode, PMIC_LDO_PMOD pmod);
void PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VR vr_name, PMIC_PSI psi_mode, PMIC_SLP_WKUP_MODE ctl_mode, int wk_slot);
void PMIC_VR_CONTROL(PMIC_VR vr_name, PMIC_VR_CTL enable);
void PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI psi_mode, PMIC_VCORE_VOSEL vcore_vosel);
void PMIC_VR_VOSEL_INIT_CONFIG(PMIC_VR vr_name, PMIC_LDO_VOSEL ldo_vosel);

void pmu_control_mtcmos_mt6385(void);
void pmu_ctrl_va28_ldo_mt6385(unsigned char enable);
void pmu_ctrl_vbt_ldo_mt6385(unsigned char enable);
unsigned char pmu_set_vcore_voltage_mt6385(unsigned char lock, unsigned char vcore_vosel);
void pmu_set_mtcmos_mt6385(unsigned char setting);
unsigned char pmu_get_vcore_voltage_mt6385(void);
void pmu_set_vcore_s1_buck_mt6385(void);
unsigned char pmu_get_usb_input_status_mt6385(void);
void pmu_enter_power_off_mt6385(void);
void pmu_reset_mt6385(void);
void pmu_set_ovr_mode_mt6385(PMIC_SET_OVR_MODE mode);
void pmu_set_vrc_key_mt6385(PMIC_SET_KEY mode);

#endif /* HAL_PMU_MODULE_ENABLED */
#endif
