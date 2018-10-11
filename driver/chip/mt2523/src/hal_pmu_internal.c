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

#include "hal_pmu.h"

#ifdef HAL_PMU_MODULE_ENABLED
#include "hal_pmu_internal.h"
#include "hal_log.h"
#include "hal_sleep_driver.h"
#include "hal_cm4_topsm.h"
#include "hal_sys_topsm.h"
#include "hal_sleep_manager_platform.h"
#include "hal_pmu_wrap_interface.h"
#include "hal_gpt.h"

#define HAL_PMU_INTERNAL_DEEBUG_ENABLE
#ifdef  HAL_PMU_INTERNAL_DEEBUG_ENABLE
#define log_hal_debug(_message,...) log_hal_info(_message, ##__VA_ARGS__)
#else
#define log_hal_debug(_message,...)
#endif

unsigned short int pmu_mtcmos_ldo_ctrl = 0x01C0;    //0:ldo hw ctrl; 1:ldo sw ctrl ; bit6~8 SW Ctrl(VCAMA,VIBR,SWMP)
unsigned short int pmu_mtcmos_ldo_en = 0;           //0:ldo disable; 1:ldo enable
unsigned short int pmu_mtcmos_vio28_pmod = 0;       //0:vio28 psi mode(S0,S1) = LP or ULP; 1:vio28 psi mode = Nromal or Lite

void PMIC_VR_PSI_PMOD_CONFIG(PMIC_VR vr_name, PMIC_PSI psi_mode, PMIC_LDO_PMOD pmod)
{
    unsigned char  hwid;
    //psi_mode: OVR/HP/LP/S0/S1/OFF/SLP/ACT
    //pmod    : NORMAL/LITE/LP/ULP
    pmu_set_vrc_key_mt6385(PMIC_UNLOCK_KEY);
    pmu_set_ovr_mode_mt6385(PMIC_ENTER_OVR_MODE);

    switch (vr_name) {
        case PMIC_VCLDO:
            switch (psi_mode) {
                case PMIC_PSI_OVR:
                    pmu_set_register_value(PMU_RG_OVR_VC_LDO_PMOD_ADDR   , PMU_RG_OVR_VC_LDO_PMOD_MASK    , PMU_RG_OVR_VC_LDO_PMOD_SHIFT   , pmod);
                    break;
                case PMIC_PSI_HP:
                    pmu_set_register_value(PMU_RG_HP_VC_LDO_PMOD_ADDR    , PMU_RG_HP_VC_LDO_PMOD_MASK     , PMU_RG_HP_VC_LDO_PMOD_SHIFT    , pmod);
                    break;
                case PMIC_PSI_LP:
                    pmu_set_register_value(PMU_RG_LP_VC_LDO_PMOD_ADDR    , PMU_RG_LP_VC_LDO_PMOD_MASK     , PMU_RG_LP_VC_LDO_PMOD_SHIFT    , pmod);
                    break;
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_S0_VC_LDO_PMOD_ADDR    , PMU_RG_S0_VC_LDO_PMOD_MASK     , PMU_RG_S0_VC_LDO_PMOD_SHIFT    , pmod);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_S1_VC_LDO_PMOD_ADDR    , PMU_RG_S1_VC_LDO_PMOD_MASK     , PMU_RG_S1_VC_LDO_PMOD_SHIFT    , pmod);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illegal PSI Mode!\n");
                    break;
            }
            break;

        case PMIC_VIO18:
            switch (psi_mode) {
                case PMIC_PSI_OVR:
                    pmu_set_register_value(PMU_RG_OVR_VIO18_PMOD_ADDR    , PMU_RG_OVR_VIO18_PMOD_MASK     , PMU_RG_OVR_VIO18_PMOD_SHIFT    , pmod);
                    break;
                case PMIC_PSI_HP:
                    pmu_set_register_value(PMU_RG_HP_VIO18_PMOD_ADDR     , PMU_RG_HP_VIO18_PMOD_MASK      , PMU_RG_HP_VIO18_PMOD_SHIFT     , pmod);
                    break;
                case PMIC_PSI_LP:
                    pmu_set_register_value(PMU_RG_LP_VIO18_PMOD_ADDR     , PMU_RG_LP_VIO18_PMOD_MASK      , PMU_RG_LP_VIO18_PMOD_SHIFT     , pmod);
                    break;
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_S0_VIO18_PMOD_ADDR     , PMU_RG_S0_VIO18_PMOD_MASK      , PMU_RG_S0_VIO18_PMOD_SHIFT     , pmod);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_S1_VIO18_PMOD_ADDR     , PMU_RG_S1_VIO18_PMOD_MASK      , PMU_RG_S1_VIO18_PMOD_SHIFT     , pmod);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illegal PSI Mode!\n");
                    break;
            }
            break;

        case PMIC_VSF:
            switch (psi_mode) {
                case PMIC_PSI_OVR:
                    pmu_set_register_value(PMU_RG_OVR_VSF_PMOD_ADDR      , PMU_RG_OVR_VSF_PMOD_MASK       , PMU_RG_OVR_VSF_PMOD_SHIFT      , pmod);
                    break;
                case PMIC_PSI_HP:
                    pmu_set_register_value(PMU_RG_HP_VSF_PMOD_ADDR       , PMU_RG_HP_VSF_PMOD_MASK        , PMU_RG_HP_VSF_PMOD_SHIFT       , pmod);
                    break;
                case PMIC_PSI_LP:
                    pmu_set_register_value(PMU_RG_LP_VSF_PMOD_ADDR       , PMU_RG_LP_VSF_PMOD_MASK        , PMU_RG_LP_VSF_PMOD_SHIFT       , pmod);
                    break;
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_S0_VSF_PMOD_ADDR       , PMU_RG_S0_VSF_PMOD_MASK        , PMU_RG_S0_VSF_PMOD_SHIFT       , pmod);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_S1_VSF_PMOD_ADDR       , PMU_RG_S1_VSF_PMOD_MASK        , PMU_RG_S1_VSF_PMOD_SHIFT       , pmod);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illegal PSI Mode!\n");
                    break;
            }
            break;

        case PMIC_VIO28:
            pmu_set_register_value(PMU_RG_CPS_W_KEY0_ADDR   , PMU_RG_CPS_W_KEY0_MASK , PMU_RG_CPS_W_KEY0_SHIFT    , 0x29);
            pmu_set_register_value(PMU_RG_CPS_W_KEY1_ADDR   , PMU_RG_CPS_W_KEY1_MASK , PMU_RG_CPS_W_KEY1_SHIFT    , 0x47);

            hwid = pmu_get_register_value(PMU_HWCID_L_ADDR  , PMU_HWCID_L_MASK    , PMU_HWCID_L_SHIFT);
            //PMU E1 VIO28 Address
            if (hwid == 0x10) {
                switch (psi_mode) {
                    case PMIC_PSI_OVR:
                        pmu_set_register_value(PMU_RG_OVR_VIO28_PMOD_ADDR    , PMU_RG_OVR_VIO28_PMOD_MASK     , PMU_RG_OVR_VIO28_PMOD_SHIFT    , pmod);
                        break;
                    case PMIC_PSI_HP:
                        pmu_set_register_value(PMU_RG_HP_VIO28_PMOD_E1_ADDR  , PMU_RG_HP_VIO28_PMOD_E1_MASK   , PMU_RG_HP_VIO28_PMOD_E1_SHIFT  , pmod);
                        break;
                    case PMIC_PSI_LP:
                        pmu_set_register_value(PMU_RG_LP_VIO28_PMOD_E1_ADDR  , PMU_RG_LP_VIO28_PMOD_E1_MASK   , PMU_RG_LP_VIO28_PMOD_E1_SHIFT  , pmod);
                        break;
                    case PMIC_PSI_S0:
                        pmu_set_register_value(PMU_RG_S0_VIO28_PMOD_E1_ADDR  , PMU_RG_S0_VIO28_PMOD_E1_MASK   , PMU_RG_S0_VIO28_PMOD_E1_SHIFT  , pmod);
                        if ((pmod == PMIC_LDO_PMOD_NORMAL) || (pmod == PMIC_LDO_PMOD_LITE)) {
                            pmu_mtcmos_vio28_pmod |= 0x01;
                        } else {
                            pmu_mtcmos_vio28_pmod &= 0xFFFE;
                        }
                        break;
                    case PMIC_PSI_S1:
                        pmu_set_register_value(PMU_RG_S1_VIO28_PMOD_E1_ADDR  , PMU_RG_S1_VIO28_PMOD_E1_MASK   , PMU_RG_S1_VIO28_PMOD_E1_SHIFT  , pmod);
                        if ((pmod == PMIC_LDO_PMOD_NORMAL) || (pmod == PMIC_LDO_PMOD_LITE)) {
                            pmu_mtcmos_vio28_pmod |= 0x02;
                        } else {
                            pmu_mtcmos_vio28_pmod &= ~0xFFFD;
                        }
                        break;
                    default:
                        log_hal_debug("PMU ERROR: Illegal PSI Mode\n");
                        break;
                }
            }
            //PMU E2 VIO28 Address
            if (hwid == 0x11) {
                switch (psi_mode) {
                    case PMIC_PSI_OVR:
                        pmu_set_register_value(PMU_RG_OVR_VIO28_PMOD_ADDR    , PMU_RG_OVR_VIO28_PMOD_MASK     , PMU_RG_OVR_VIO28_PMOD_SHIFT    , pmod);
                        break;
                    case PMIC_PSI_HP:
                        pmu_set_register_value(PMU_RG_HP_VIO28_PMOD_ADDR     , PMU_RG_HP_VIO28_PMOD_MASK      , PMU_RG_HP_VIO28_PMOD_SHIFT     , pmod);
                        break;
                    case PMIC_PSI_LP:
                        pmu_set_register_value(PMU_RG_LP_VIO28_PMOD_ADDR     , PMU_RG_LP_VIO28_PMOD_MASK      , PMU_RG_LP_VIO28_PMOD_SHIFT     , pmod);
                        break;
                    case PMIC_PSI_S0:
                        pmu_set_register_value(PMU_RG_S0_VIO28_PMOD_ADDR     , PMU_RG_S0_VIO28_PMOD_MASK      , PMU_RG_S0_VIO28_PMOD_SHIFT     , pmod);
                        if ((pmod == PMIC_LDO_PMOD_NORMAL) || (pmod == PMIC_LDO_PMOD_LITE)) {
                            pmu_mtcmos_vio28_pmod |= 0x01;
                        } else {
                            pmu_mtcmos_vio28_pmod &= 0xFFFE;
                        }
                        break;
                    case PMIC_PSI_S1:
                        pmu_set_register_value(PMU_RG_S1_VIO28_PMOD_ADDR     , PMU_RG_S1_VIO28_PMOD_MASK      , PMU_RG_S1_VIO28_PMOD_SHIFT     , pmod);
                        if ((pmod == PMIC_LDO_PMOD_NORMAL) || (pmod == PMIC_LDO_PMOD_LITE)) {
                            pmu_mtcmos_vio28_pmod |= 0x02;
                        } else {
                            pmu_mtcmos_vio28_pmod &= ~0xFFFD;
                        }
                        break;
                    default:
                        log_hal_debug("PMU ERROR: Illegal PSI Mode\n");
                        break;
                }
            }

            pmu_set_register_value(PMU_RG_CPS_W_KEY0_ADDR   , PMU_RG_CPS_W_KEY0_MASK , PMU_RG_CPS_W_KEY0_SHIFT    , 0x0);
            pmu_set_register_value(PMU_RG_CPS_W_KEY1_ADDR   , PMU_RG_CPS_W_KEY1_MASK , PMU_RG_CPS_W_KEY1_SHIFT    , 0x0);
            break;

        case PMIC_VDIG18:
            switch (psi_mode) {
                case PMIC_PSI_OVR:
                    pmu_set_register_value(PMU_RG_ACT_VDIG18_PMOD_ADDR   , PMU_RG_ACT_VDIG18_PMOD_MASK    , PMU_RG_ACT_VDIG18_PMOD_SHIFT   , pmod);
                    break;
                case PMIC_PSI_HP:
                    pmu_set_register_value(PMU_RG_ACT_VDIG18_PMOD_ADDR   , PMU_RG_ACT_VDIG18_PMOD_MASK    , PMU_RG_ACT_VDIG18_PMOD_SHIFT   , pmod);
                    break;
                case PMIC_PSI_LP:
                    pmu_set_register_value(PMU_RG_ACT_VDIG18_PMOD_ADDR   , PMU_RG_ACT_VDIG18_PMOD_MASK    , PMU_RG_ACT_VDIG18_PMOD_SHIFT   , pmod);
                    break;
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_SLP_VDIG18_PMOD_ADDR   , PMU_RG_SLP_VDIG18_PMOD_MASK    , PMU_RG_SLP_VDIG18_PMOD_SHIFT   , pmod);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_SLP_VDIG18_PMOD_ADDR   , PMU_RG_SLP_VDIG18_PMOD_MASK    , PMU_RG_SLP_VDIG18_PMOD_SHIFT   , pmod);
                    break;
                case PMIC_PSI_OFF:
                    pmu_set_register_value(PMU_RG_OFF_VDIG18_PMOD_ADDR   , PMU_RG_OFF_VDIG18_PMOD_MASK    , PMU_RG_OFF_VDIG18_PMOD_SHIFT   , pmod);
                    break;
                case PMIC_PSI_SLP:
                    pmu_set_register_value(PMU_RG_SLP_VDIG18_PMOD_ADDR   , PMU_RG_SLP_VDIG18_PMOD_MASK    , PMU_RG_SLP_VDIG18_PMOD_SHIFT   , pmod);
                    break;
                case PMIC_PSI_ACT:
                    pmu_set_register_value(PMU_RG_ACT_VDIG18_PMOD_ADDR   , PMU_RG_ACT_VDIG18_PMOD_MASK    , PMU_RG_ACT_VDIG18_PMOD_SHIFT   , pmod);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illegal PSI Mode\n");
                    break;
            }
            break;

        case PMIC_VA18: // pmod: can only be 0 or 1
            switch (psi_mode) {
                case PMIC_PSI_OVR:
                    pmu_set_register_value(PMU_RG_OVR_VA18_PMOD_ADDR     , PMU_RG_OVR_VA18_PMOD_MASK      , PMU_RG_OVR_VA18_PMOD_SHIFT     , pmod);
                    break;
                case PMIC_PSI_HP:
                    pmu_set_register_value(PMU_RG_HP_VA18_PMOD_ADDR      , PMU_RG_HP_VA18_PMOD_MASK       , PMU_RG_HP_VA18_PMOD_SHIFT      , pmod);
                    break;
                case PMIC_PSI_LP:
                    pmu_set_register_value(PMU_RG_LP_VA18_PMOD_ADDR      , PMU_RG_LP_VA18_PMOD_MASK       , PMU_RG_LP_VA18_PMOD_SHIFT      , pmod);
                    break;
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_S0_VA18_PMOD_ADDR      , PMU_RG_S0_VA18_PMOD_MASK       , PMU_RG_S0_VA18_PMOD_SHIFT      , pmod);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_S1_VA18_PMOD_ADDR      , PMU_RG_S1_VA18_PMOD_MASK       , PMU_RG_S1_VA18_PMOD_SHIFT      , pmod);
                    break;
                case PMIC_PSI_OFF:
                    pmu_set_register_value(PMU_RG_OFF_VA18_PMOD_ADDR     , PMU_RG_OFF_VA18_PMOD_MASK      , PMU_RG_OFF_VA18_PMOD_SHIFT     , pmod);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illegal PSI Mode!\n");
                    break;
            }
            break;

        case PMIC_VUSB:
            switch (psi_mode) {
                case PMIC_PSI_OVR:
                    pmu_set_register_value(PMU_RG_OVR_VUSB_PMOD_ADDR     , PMU_RG_OVR_VUSB_PMOD_MASK      , PMU_RG_OVR_VUSB_PMOD_SHIFT     , pmod);
                    break;
                case PMIC_PSI_HP:
                    pmu_set_register_value(PMU_RG_HP_VUSB_PMOD_ADDR      , PMU_RG_HP_VUSB_PMOD_MASK       , PMU_RG_HP_VUSB_PMOD_SHIFT      , pmod);
                    break;
                case PMIC_PSI_LP:
                    pmu_set_register_value(PMU_RG_LP_VUSB_PMOD_ADDR      , PMU_RG_LP_VUSB_PMOD_MASK       , PMU_RG_LP_VUSB_PMOD_SHIFT      , pmod);
                    break;
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_S0_VUSB_PMOD_ADDR      , PMU_RG_S0_VUSB_PMOD_MASK       , PMU_RG_S0_VUSB_PMOD_SHIFT      , pmod);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_S1_VUSB_PMOD_ADDR      , PMU_RG_S1_VUSB_PMOD_MASK       , PMU_RG_S1_VUSB_PMOD_SHIFT      , pmod);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illegal PSI Mode\n");
                    break;
            }
            break;

        case PMIC_VBT:
            switch (psi_mode) {
                case PMIC_PSI_OVR:
                    pmu_set_register_value(PMU_RG_OVR_VBT_PMOD_ADDR      , PMU_RG_OVR_VBT_PMOD_MASK       , PMU_RG_OVR_VBT_PMOD_SHIFT      , pmod);
                    break;
                case PMIC_PSI_HP:
                    pmu_set_register_value(PMU_RG_HP_VBT_PMOD_ADDR       , PMU_RG_HP_VBT_PMOD_MASK        , PMU_RG_HP_VBT_PMOD_SHIFT       , pmod);
                    break;
                case PMIC_PSI_LP:
                    pmu_set_register_value(PMU_RG_LP_VBT_PMOD_ADDR       , PMU_RG_LP_VBT_PMOD_MASK        , PMU_RG_LP_VBT_PMOD_SHIFT       , pmod);
                    break;
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_S0_VBT_PMOD_ADDR       , PMU_RG_S0_VBT_PMOD_MASK        , PMU_RG_S0_VBT_PMOD_SHIFT       , pmod);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_S1_VBT_PMOD_ADDR       , PMU_RG_S1_VBT_PMOD_MASK        , PMU_RG_S1_VBT_PMOD_SHIFT       , pmod);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illegal PSI Mode!\n");
                    break;
            }
            break;

        case PMIC_VA28:
            switch (psi_mode) {
                case PMIC_PSI_OVR:
                    pmu_set_register_value(PMU_RG_OVR_VA_PMOD_ADDR       , PMU_RG_OVR_VA_PMOD_MASK        , PMU_RG_OVR_VA_PMOD_SHIFT       , pmod);
                    break;
                case PMIC_PSI_HP:
                    pmu_set_register_value(PMU_RG_HP_VA_PMOD_ADDR        , PMU_RG_HP_VA_PMOD_MASK         , PMU_RG_HP_VA_PMOD_SHIFT        , pmod);
                    break;
                case PMIC_PSI_LP:
                    pmu_set_register_value(PMU_RG_LP_VA_PMOD_ADDR        , PMU_RG_LP_VA_PMOD_MASK         , PMU_RG_LP_VA_PMOD_SHIFT        , pmod);
                    break;
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_S0_VA_PMOD_ADDR        , PMU_RG_S0_VA_PMOD_MASK         , PMU_RG_S0_VA_PMOD_SHIFT        , pmod);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_S1_VA_PMOD_ADDR        , PMU_RG_S1_VA_PMOD_MASK         , PMU_RG_S1_VA_PMOD_SHIFT        , pmod);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illegal PSI Mode!\n");
                    break;
            }
            break;

        case PMIC_VMC:
            switch (psi_mode) {
                case PMIC_PSI_OVR:
                    pmu_set_register_value(PMU_RG_OVR_VMC_PMOD_ADDR      , PMU_RG_OVR_VMC_PMOD_MASK       , PMU_RG_OVR_VMC_PMOD_SHIFT      , pmod);
                    break;
                case PMIC_PSI_HP:
                    pmu_set_register_value(PMU_RG_HP_VMC_PMOD_ADDR       , PMU_RG_HP_VMC_PMOD_MASK        , PMU_RG_HP_VMC_PMOD_SHIFT       , pmod);
                    break;
                case PMIC_PSI_LP:
                    pmu_set_register_value(PMU_RG_LP_VMC_PMOD_ADDR       , PMU_RG_LP_VMC_PMOD_MASK        , PMU_RG_LP_VMC_PMOD_SHIFT       , pmod);
                    break;
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_S0_VMC_PMOD_ADDR       , PMU_RG_S0_VMC_PMOD_MASK        , PMU_RG_S0_VMC_PMOD_SHIFT       , pmod);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_S1_VMC_PMOD_ADDR       , PMU_RG_S1_VMC_PMOD_MASK        , PMU_RG_S1_VMC_PMOD_SHIFT       , pmod);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illegal PSI Mode!\n");
                    break;
            }
            break;

        case PMIC_VCAMA:
            pmu_set_register_value(PMU_RG_OVR_VCAMA_PMOD_ADDR     , PMU_RG_OVR_VCAMA_PMOD_MASK     , PMU_RG_OVR_VCAMA_PMOD_SHIFT    , pmod);
            break;

        case PMIC_VIBR:
            pmu_set_register_value(PMU_RG_OVR_VIBR_PMOD_ADDR      , PMU_RG_OVR_VIBR_PMOD_MASK      , PMU_RG_OVR_VIBR_PMOD_SHIFT     , pmod);
            break;

        default:
            log_hal_debug("PMU ERROR: No matching LDO!\n");
            break;
    }
    pmu_set_ovr_mode_mt6385(PMIC_LEAVE_OVR_MODE); // switch to PSI Bus
    //need to enable key protect?-immelman comment
    pmu_set_vrc_key_mt6385(PMIC_LOCK_KEY);
}


void PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VR vr_name, PMIC_PSI psi_mode, PMIC_SLP_WKUP_MODE ctl_mode, int wk_slot)
{
    // psi_mode : S0/S1
    // ctl_mode :
    //   1. HW control sleep on/off
    //   2. SW control on/off
    //   3. Sleep always on
    // wk_slot: 1~N
    int wk_seq_len = 0;
    int wk_slot_final = 14;

    // Check if wk_slot is reasonable.
    switch (psi_mode) {
        case PMIC_PSI_S0:
            wk_seq_len = pmu_get_register_value(PMU_RG_W0SEQ_LEN_ADDR , PMU_RG_W0SEQ_LEN_MASK  , PMU_RG_W0SEQ_LEN_SHIFT);
            break;
        case PMIC_PSI_S1:
            wk_seq_len = pmu_get_register_value(PMU_RG_W1SEQ_LEN_ADDR , PMU_RG_W1SEQ_LEN_MASK  , PMU_RG_W1SEQ_LEN_SHIFT);
            break;
        default:
            log_hal_debug("PMU ERROR: Illegal PSI mode!\n");
            break;
    }

    if (wk_slot > wk_seq_len) {
        log_hal_debug("PMU ERROR: wk_slot > Wake sequence length setting!\n");
    }

    switch (ctl_mode) {
        case PMIC_SLP_WKUP_HW_CTL:
            wk_slot_final = wk_slot;
            break;
        case PMIC_SLP_WKUP_SW_CTL:
            wk_slot_final = 15;
            break;
        case PMIC_SLP_WKUP_AON:
            wk_slot_final = 15;
            break;
        default:
            log_hal_debug("PMU ERROR: Undefined control mode =%d!\n", ctl_mode);
            break;
    }

    //printf("PMIC_VR_SLEEP_WKUP_CONFIG:: wk_slot=%d  wk_slot_final=%d", wk_slot, wk_slot_final);

    // Fill protection key
    pmu_set_register_value(PMU_RG_CPS_W_KEY0_ADDR   , PMU_RG_CPS_W_KEY0_MASK , PMU_RG_CPS_W_KEY0_SHIFT    , 0x29);
    pmu_set_register_value(PMU_RG_CPS_W_KEY1_ADDR   , PMU_RG_CPS_W_KEY1_MASK , PMU_RG_CPS_W_KEY1_SHIFT    , 0x47);

    switch (vr_name) {
        case PMIC_VCLDO:
            log_hal_debug("PMU ERROR: No sleep wake up setting for VCORE LDO!\n");
            break;

        case PMIC_VIO18:
            log_hal_debug("PMU ERROR: No sleep wake up setting for VIO18 LDO!\n");
            break;

        case PMIC_VSF:
            switch (psi_mode) {
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_VSF_WS0A_ADDR   , PMU_RG_VSF_WS0A_MASK   , PMU_RG_VSF_WS0A_SHIFT  , wk_slot_final);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_VSF_WS1A_ADDR   , PMU_RG_VSF_WS1A_MASK   , PMU_RG_VSF_WS1A_SHIFT  , wk_slot_final);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illeagal PSI mode!\n");
                    break;
            }
            break;

        case PMIC_VIO28:
            if (wk_slot_final == 15) {
                pmu_mtcmos_ldo_ctrl |= 0x1;
            } else {
                pmu_mtcmos_ldo_ctrl &= 0xFFFE;
            }
            switch (psi_mode) {
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_VIO28_WS0A_ADDR     , PMU_RG_VIO28_WS0A_MASK     , PMU_RG_VIO28_WS0A_SHIFT    , wk_slot_final);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_VIO28_WS1A_ADDR     , PMU_RG_VIO28_WS1A_MASK     , PMU_RG_VIO28_WS1A_SHIFT    , wk_slot_final);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illeagal PSI mode!\n");
                    break;
            }
            break;

        case PMIC_VDIG18:
            log_hal_debug("PMU ERROR: No sleep wake up setting for VDIG18 LDO!");
            break;

        case PMIC_VA18:
            if ((ctl_mode == PMIC_SLP_WKUP_AON) || (ctl_mode == PMIC_SLP_WKUP_SW_CTL)) {

                pmu_set_vrc_key_mt6385(PMIC_UNLOCK_KEY);
                pmu_set_ovr_mode_mt6385(PMIC_ENTER_OVR_MODE); // switch to PSIOVR
                switch (psi_mode) {
                    case PMIC_PSI_S0:
                        pmu_set_register_value(PMU_RG_S0_VA18_ON_ADDR   , PMU_RG_S0_VA18_ON_MASK , PMU_RG_S0_VA18_ON_SHIFT    , 1);
                        break;
                    case PMIC_PSI_S1:
                        pmu_set_register_value(PMU_RG_S1_VA18_ON_ADDR   , PMU_RG_S1_VA18_ON_MASK , PMU_RG_S1_VA18_ON_SHIFT    , 1);
                        break;
                    default:
                        log_hal_debug("PMU ERROR: Illeagal PSI mode!\n");
                        break;
                }

                pmu_set_ovr_mode_mt6385(PMIC_LEAVE_OVR_MODE); // switch to PSI Bus
                pmu_set_vrc_key_mt6385(PMIC_LOCK_KEY);
            }
            break;

        case PMIC_VUSB:
            switch (psi_mode) {
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_VUSB_WS0A_ADDR      , PMU_RG_VUSB_WS0A_MASK      , PMU_RG_VUSB_WS0A_SHIFT     , wk_slot_final);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_VUSB_WS1A_ADDR      , PMU_RG_VUSB_WS1A_MASK      , PMU_RG_VUSB_WS1A_SHIFT     , wk_slot_final);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illeagal PSI mode!\n");
                    break;
            }
            break;

        case PMIC_VBT:
            if (wk_slot_final == 15) {
                pmu_mtcmos_ldo_ctrl |= 0x2;
            } else {
                pmu_mtcmos_ldo_ctrl &= 0xFFFD;
            }
            if (wk_slot_final == 0) { // 0 is only available for IVGEN AON
                // VBT uses BG/IVGEN
                log_hal_debug("PMU ERROR: wk_slot should greater than 0!\n");
            }
            switch (psi_mode) {
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_VBT_WS0A_ADDR       , PMU_RG_VBT_WS0A_MASK       , PMU_RG_VBT_WS0A_SHIFT      , wk_slot_final);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_VBT_WS1A_ADDR       , PMU_RG_VBT_WS1A_MASK       , PMU_RG_VBT_WS1A_SHIFT      , wk_slot_final);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illeagal PSI mode!\n");
                    break;
            }
            break;

        case PMIC_VA28:
            if (wk_slot_final == 15) {
                pmu_mtcmos_ldo_ctrl |= 0x4;
            } else {
                pmu_mtcmos_ldo_ctrl &= 0xFFFB;
            }
            if (wk_slot_final == 0) { // 0 is only available for IVGEN AON
                // VA28 uses BG/IVGEN
                log_hal_debug("PMU ERROR: wk_slot should greater than 0!\n");
            }
            switch (psi_mode) {
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_VA_WS0A_ADDR        , PMU_RG_VA_WS0A_MASK        , PMU_RG_VA_WS0A_SHIFT       , wk_slot_final);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_VA_WS1A_ADDR        , PMU_RG_VA_WS1A_MASK        , PMU_RG_VA_WS1A_SHIFT       , wk_slot_final);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illeagal PSI mode!\n");
                    break;
            }
            break;

        case PMIC_VMC:
            if (wk_slot_final == 15) {
                pmu_mtcmos_ldo_ctrl |= 0x20;
            } else {
                pmu_mtcmos_ldo_ctrl &= 0xFFDF;
            }
            if (wk_slot_final == 0) { // 0 is only available for IVGEN AON
                // VMC uses BG/IVGEN
                log_hal_debug("PMU ERROR: wk_slot should greater than 0!\n");
            }
            switch (psi_mode) {
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_VMC_WS0A_ADDR   , PMU_RG_VMC_WS0A_MASK       , PMU_RG_VMC_WS0A_SHIFT      , wk_slot_final);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_VMC_WS1A_ADDR   , PMU_RG_VMC_WS1A_MASK       , PMU_RG_VMC_WS1A_SHIFT      , wk_slot_final);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illeagal PSI mode!\n");
                    break;
            }
            break;

        case PMIC_VCAMA:
            log_hal_debug("PMU ERROR: No sleep wake up setting for VCAMA LDO!\n");
            break;

        case PMIC_VIBR:
            log_hal_debug("PMU ERROR: No sleep wake up setting for VIBR! LDO\n");
            break;

        case PMIC_SWXM:
            if (wk_slot_final == 15) {
                pmu_mtcmos_ldo_ctrl |= 0x8;
            } else {
                pmu_mtcmos_ldo_ctrl &= 0xFFF7;
            }
            if (wk_slot_final == 0) { // 0 is only available for BG/IVGEN AON
                // SWXM soft start use BG/IVGEN
                log_hal_debug("PMU ERROR: wk_slot should greater than 0!\n");
            }
            switch (psi_mode) {
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_SWXM_WS0A_ADDR  , PMU_RG_SWXM_WS0A_MASK      , PMU_RG_SWXM_WS0A_SHIFT     , wk_slot_final);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_SWXM_WS1A_ADDR  , PMU_RG_SWXM_WS1A_MASK      , PMU_RG_SWXM_WS1A_SHIFT     , wk_slot_final);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illeagal PSI mode!\n");
                    break;
            }
            break;

        case PMIC_SWDP:
            if (wk_slot_final == 15) {
                pmu_mtcmos_ldo_ctrl |= 0x10;
            } else {
                pmu_mtcmos_ldo_ctrl &= 0xFFEF;
            }
            if (wk_slot_final == 0) { // 0 is only available for BG/IVGEN AON
                // SWDP soft start use BG/IVGEN
                log_hal_debug("PMU ERROR: wk_slot should greater than 0!\n");
            }
            switch (psi_mode) {
                case PMIC_PSI_S0:
                    pmu_set_register_value(PMU_RG_SWDP_WS0A_ADDR  , PMU_RG_SWDP_WS0A_MASK      , PMU_RG_SWDP_WS0A_SHIFT     , wk_slot_final);
                    break;
                case PMIC_PSI_S1:
                    pmu_set_register_value(PMU_RG_SWDP_WS1A_ADDR  , PMU_RG_SWDP_WS1A_MASK      , PMU_RG_SWDP_WS1A_SHIFT     , wk_slot_final);
                    break;
                default:
                    log_hal_debug("PMU ERROR: Illeagal PSI mode!\n");
                    break;
            }
            break;

        case PMIC_SWMP:
            log_hal_debug("PMU ERROR: No sleep wake up setting for SWMP! LDO\n");
            break;

        default:
            log_hal_debug("PMU ERROR: No matching LDO!");
            break;
    }

    // Clear protection key
    pmu_set_register_value(PMU_RG_CPS_W_KEY0_ADDR   , PMU_RG_CPS_W_KEY0_MASK , PMU_RG_CPS_W_KEY0_SHIFT    , 0x0);
    pmu_set_register_value(PMU_RG_CPS_W_KEY1_ADDR   , PMU_RG_CPS_W_KEY1_MASK , PMU_RG_CPS_W_KEY1_SHIFT    , 0x0);
}

void PMIC_VR_CONTROL(PMIC_VR vr_name, PMIC_VR_CTL enable)
{
    int poll_stb_b = 1;

    if (enable) { // polling STB status
        poll_stb_b = 0;
    }

    // Fill protection key
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR     , PMU_RG_VRC_CFG_KEY0_MASK    , PMU_RG_VRC_CFG_KEY0_SHIFT  , 0xFC);
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR     , PMU_RG_VRC_CFG_KEY1_MASK    , PMU_RG_VRC_CFG_KEY1_SHIFT  , 0xDA);

    switch (vr_name) {
        case PMIC_VCLDO:
            log_hal_debug("PMU ERROR: VCORE LDO cannot be turned off!\n");
            break;

        case PMIC_VIO18:
            log_hal_debug("PMU ERROR: VIO18 LDO cannot be turned off!\n");
            break;

        case PMIC_VSF:
            pmu_set_register_value(PMU_RG_VSF_EN_ADDR   , PMU_RG_VSF_EN_MASK     , PMU_RG_VSF_EN_SHIFT    , enable);
            while (poll_stb_b == 0) {
                poll_stb_b = pmu_get_register_value(PMU_RGS_VSF_STBSTS_ADDR     , PMU_RGS_VSF_STBSTS_MASK    , PMU_RGS_VSF_STBSTS_SHIFT);
            }
            break;

        case PMIC_VIO28:
            if (enable == 1) {
                pmu_mtcmos_ldo_en |= 0x1;
            } else {
                pmu_mtcmos_ldo_en &= 0xFFFE;
            }
            pmu_set_register_value(PMU_RG_VIO28_EN_ADDR , PMU_RG_VIO28_EN_MASK   , PMU_RG_VIO28_EN_SHIFT  , enable);
            while (poll_stb_b == 0) {
                poll_stb_b = pmu_get_register_value(PMU_RGS_VIO28_STBSTS_ADDR   , PMU_RGS_VIO28_STBSTS_MASK  , PMU_RGS_VIO28_STBSTS_SHIFT);
            }
            break;

        case PMIC_VDIG18:
            log_hal_debug("PMU ERROR: VIO18 LDO cannot be turned off!\n");
            break;

        case PMIC_VA18:
            pmu_set_register_value(PMU_RG_VA18_EN_ADDR  , PMU_RG_VA18_EN_MASK    , PMU_RG_VA18_EN_SHIFT   , enable);
            break;

        case PMIC_VUSB:
            pmu_set_register_value(PMU_RG_VUSB_EN_ADDR  , PMU_RG_VUSB_EN_MASK    , PMU_RG_VUSB_EN_SHIFT   , enable);
            while (poll_stb_b == 0) {
                poll_stb_b = pmu_get_register_value(PMU_RGS_VUSB_STBSTS_ADDR    , PMU_RGS_VUSB_STBSTS_MASK   , PMU_RGS_VUSB_STBSTS_SHIFT);
            }
            break;

        case PMIC_VBT:
            if (enable == 1) {
                pmu_mtcmos_ldo_en |= 0x2;
            } else {
                pmu_mtcmos_ldo_en &= 0xFFFD;
            }
            pmu_set_register_value(PMU_RG_VBT_EN_ADDR   , PMU_RG_VBT_EN_MASK     , PMU_RG_VBT_EN_SHIFT    , enable);
            while (poll_stb_b == 0) {
                poll_stb_b = pmu_get_register_value(PMU_RGS_VBT_STBSTS_ADDR     , PMU_RGS_VBT_STBSTS_MASK    , PMU_RGS_VBT_STBSTS_SHIFT);
            }
            break;

        case PMIC_VA28:
            if (enable == 1) {
                pmu_mtcmos_ldo_en |= 0x4;
            } else {
                pmu_mtcmos_ldo_en &= 0xFFFB;
            }
            pmu_set_register_value(PMU_RG_VA_EN_ADDR    , PMU_RG_VA_EN_MASK      , PMU_RG_VA_EN_SHIFT     , enable);
            while (poll_stb_b == 0) {
                poll_stb_b = pmu_get_register_value(PMU_RGS_VA_STBSTS_ADDR      , PMU_RGS_VA_STBSTS_MASK     , PMU_RGS_VA_STBSTS_SHIFT);
            }
            break;

        case PMIC_VMC:
            if (enable == 1) {
                pmu_mtcmos_ldo_en |= 0x20;
            } else {
                pmu_mtcmos_ldo_en &= 0xFFDF;
            }
            pmu_set_register_value(PMU_RG_VMC_EN_ADDR   , PMU_RG_VMC_EN_MASK     , PMU_RG_VMC_EN_SHIFT    , enable);
            while (poll_stb_b == 0) {
                poll_stb_b = pmu_get_register_value(PMU_RGS_VMC_STBSTS_ADDR     , PMU_RGS_VMC_STBSTS_MASK    , PMU_RGS_VMC_STBSTS_SHIFT);
            }
            break;

        case PMIC_VCAMA:
            if (enable == 1) {
                pmu_mtcmos_ldo_en |= 0x40;
            } else {
                pmu_mtcmos_ldo_en &= 0xFFBF;
            }
            pmu_set_register_value(PMU_RG_VCAMA_EN_ADDR , PMU_RG_VCAMA_EN_MASK   , PMU_RG_VCAMA_EN_SHIFT  , enable);
            while (poll_stb_b == 0) {
                poll_stb_b = pmu_get_register_value(PMU_RGS_VCAMA_STBSTS_ADDR   , PMU_RGS_VCAMA_STBSTS_MASK  , PMU_RGS_VCAMA_STBSTS_SHIFT);
            }
            break;

        case PMIC_VIBR:
            if (enable == 1) {
                pmu_mtcmos_ldo_en |= 0x80;
            } else {
                pmu_mtcmos_ldo_en &= 0xFF7F;
            }
            pmu_set_register_value(PMU_RG_VIBR_EN_ADDR  , PMU_RG_VIBR_EN_MASK    , PMU_RG_VIBR_EN_SHIFT   , enable);
            while (poll_stb_b == 0) {
                poll_stb_b = pmu_get_register_value(PMU_RGS_VIBR_STBSTS_ADDR    , PMU_RGS_VIBR_STBSTS_MASK   , PMU_RGS_VIBR_STBSTS_SHIFT);
            }
            break;

        case PMIC_SWXM: // No STB flag available
            if (enable == 1) {
                pmu_mtcmos_ldo_en |= 0x8;
            } else {
                pmu_mtcmos_ldo_en &= 0xFFF7;
            }
            pmu_set_register_value(PMU_RG_SWXM_EN_ADDR , PMU_RG_SWXM_EN_MASK   , PMU_RG_SWXM_EN_SHIFT  , enable);
            break;

        case PMIC_SWDP: // No STB flag available
            if (enable == 1) {
                pmu_mtcmos_ldo_en |= 0x10;
            } else {
                pmu_mtcmos_ldo_en &= 0xFFEF;
            }
            pmu_set_register_value(PMU_RG_SWDP_EN_ADDR  , PMU_RG_SWDP_EN_MASK    , PMU_RG_SWDP_EN_SHIFT   , enable);
            break;

        case PMIC_SWMP: // No STB flag available
            if (enable == 1) {
                pmu_mtcmos_ldo_en |= 0x100;
            } else {
                pmu_mtcmos_ldo_en &= 0xFEFF;
            }
            pmu_set_register_value(PMU_RG_SWMP_EN_ADDR  , PMU_RG_SWMP_EN_MASK    , PMU_RG_SWMP_EN_SHIFT   , enable);
            break;

        default:
            log_hal_debug("PMU ERROR: No matching VR!\n");
            break;
    }

    // Clear protection key
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR     , PMU_RG_VRC_CFG_KEY0_MASK    , PMU_RG_VRC_CFG_KEY0_SHIFT  , 0x0);
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR     , PMU_RG_VRC_CFG_KEY1_MASK    , PMU_RG_VRC_CFG_KEY1_SHIFT  , 0x0);
}

void PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI psi_mode, PMIC_VCORE_VOSEL vcore_vosel)
{
    // psi_mode : OVR/HP/LP/S1
    int vcbuck_voval[7] = {2, 4, 6, 8, 10, 12, 14};
    int vc_ldo_vosel[7] = {0, 2, 4, 6, 8, 10, 12};

    pmu_set_vrc_key_mt6385(PMIC_UNLOCK_KEY);
    pmu_set_ovr_mode_mt6385(PMIC_ENTER_OVR_MODE); // switch to PSIOVR
    hal_gpt_delay_us(350);
    switch (psi_mode) {
        case PMIC_PSI_OVR:
            pmu_set_register_value(PMU_RG_OVR_VC_BUCK_VOVAL_ADDR    , PMU_RG_OVR_VC_BUCK_VOVAL_MASK  , PMU_RG_OVR_VC_BUCK_VOVAL_SHIFT , vcbuck_voval[vcore_vosel]);
            pmu_set_register_value(PMU_RG_OVR_VC_LDO_VOSEL_ADDR     , PMU_RG_OVR_VC_LDO_VOSEL_MASK   , PMU_RG_OVR_VC_LDO_VOSEL_SHIFT  , vc_ldo_vosel[vcore_vosel]);
            break;

        case PMIC_PSI_HP:
            pmu_set_register_value(PMU_RG_HP_VC_BUCK_VOVAL_ADDR     , PMU_RG_HP_VC_BUCK_VOVAL_MASK   , PMU_RG_HP_VC_BUCK_VOVAL_SHIFT  , vcbuck_voval[vcore_vosel]);
            pmu_set_register_value(PMU_RG_HP_VC_LDO_VOSEL_ADDR      , PMU_RG_HP_VC_LDO_VOSEL_MASK    , PMU_RG_HP_VC_LDO_VOSEL_SHIFT   , vc_ldo_vosel[vcore_vosel]);
            break;

        case PMIC_PSI_LP:
            pmu_set_register_value(PMU_RG_LP_VC_BUCK_VOVAL_ADDR     , PMU_RG_LP_VC_BUCK_VOVAL_MASK   , PMU_RG_LP_VC_BUCK_VOVAL_SHIFT  , vcbuck_voval[vcore_vosel]);
            pmu_set_register_value(PMU_RG_LP_VC_LDO_VOSEL_ADDR      , PMU_RG_LP_VC_LDO_VOSEL_MASK    , PMU_RG_LP_VC_LDO_VOSEL_SHIFT   , vc_ldo_vosel[vcore_vosel]);
            break;

        case PMIC_PSI_S1:
            pmu_set_register_value(PMU_RG_S1_VC_LDO_VOSEL_ADDR      , PMU_RG_S1_VC_LDO_VOSEL_MASK    , PMU_RG_S1_VC_LDO_VOSEL_SHIFT   , vc_ldo_vosel[vcore_vosel]);
            break;

        default:
            log_hal_debug("PMU ERROR: Illegal PSI mode for setting VCORE voltage!\n");
            break;
    }
    pmu_set_ovr_mode_mt6385(PMIC_LEAVE_OVR_MODE);
    hal_gpt_delay_us(250);
    pmu_set_vrc_key_mt6385(PMIC_LOCK_KEY);
}

void PMIC_VR_VOSEL_INIT_CONFIG(PMIC_VR vr_name, PMIC_LDO_VOSEL ldo_vosel)
{
    int viber_vosel[8] = {0, 1, 2, 3, 4, 5, 6, 7};

    // Fill protection key
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR, PMU_RG_VRC_CFG_KEY0_MASK, PMU_RG_VRC_CFG_KEY0_SHIFT, 0xFC);
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR, PMU_RG_VRC_CFG_KEY1_MASK, PMU_RG_VRC_CFG_KEY1_SHIFT, 0xDA);

    switch (vr_name) {
        case PMIC_VCAMA:
            switch (ldo_vosel) {
                case PMIC_LDO_1P5V:
                    pmu_set_register_value(PMU_RG_VCAMA_VOSEL_ADDR  , PMU_RG_VCAMA_VOSEL_MASK    , PMU_RG_VCAMA_VOSEL_SHIFT   , 0);
                    break;

                case PMIC_LDO_1P8V:
                    pmu_set_register_value(PMU_RG_VCAMA_VOSEL_ADDR  , PMU_RG_VCAMA_VOSEL_MASK    , PMU_RG_VCAMA_VOSEL_SHIFT   , 1);
                    break;

                case PMIC_LDO_2P5V:
                    pmu_set_register_value(PMU_RG_VCAMA_VOSEL_ADDR  , PMU_RG_VCAMA_VOSEL_MASK    , PMU_RG_VCAMA_VOSEL_SHIFT   , 2);
                    break;

                case PMIC_LDO_2P8V:
                    pmu_set_register_value(PMU_RG_VCAMA_VOSEL_ADDR  , PMU_RG_VCAMA_VOSEL_MASK    , PMU_RG_VCAMA_VOSEL_SHIFT   , 3);
                    break;

                default:
                    log_hal_debug("PMU ERROR: No support voltage!\n");
                    break;
            }
            break;

        case PMIC_VIBR:
            pmu_set_register_value(PMU_RG_VIBR_VOSEL_ADDR   , PMU_RG_VIBR_VOSEL_MASK , PMU_RG_VIBR_VOSEL_SHIFT    , viber_vosel[ldo_vosel]);
            break;

        default:
            log_hal_debug("PMU ERROR: No matching VR!\n");
            break;
    }

    // Clear protection key
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR, PMU_RG_VRC_CFG_KEY0_MASK, PMU_RG_VRC_CFG_KEY0_SHIFT, 0x0);
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR, PMU_RG_VRC_CFG_KEY1_MASK, PMU_RG_VRC_CFG_KEY1_SHIFT, 0x0);
}

void pmu_control_mtcmos_mt6385(void)
{
    static unsigned short int local_flag = 0, mtcmos_flag = 0;

    mtcmos_flag = (pmu_mtcmos_ldo_ctrl & pmu_mtcmos_ldo_en) | pmu_mtcmos_vio28_pmod;

    //log_hal_debug("\n\rPMU pmu_mtcmos_ldo_ctrl : %d\n\r",pmu_mtcmos_ldo_ctrl);
    //log_hal_debug("\n\rPMU pmu_mtcmos_ldo_en : %d\n\r",pmu_mtcmos_ldo_en);

    if (local_flag != mtcmos_flag) {
        if (mtcmos_flag != 0) {
            //log_hal_debug("\n\rPMU MTCMOS will be Power ON in S0,S1 Mode\n\r");
            pmu_set_mtcmos_mt6385(3);//MTCMOS Keep power on when enter S0,S1
        } else {
            //log_hal_debug("\n\rPMU MTCMOS will be Power OFF in S0,S1 Mode\n\r");
            pmu_set_mtcmos_mt6385(0);//MTCMOS Keep power off when enter S0,S1
        }
        local_flag = mtcmos_flag;
    }
}

void pmu_ctrl_va28_ldo_mt6385(unsigned char enable)
{
    static unsigned int resoure_count = 0;
    static unsigned char version = 0;

    //if A Die is E2 Version,Can't use VA28 Control
    if (version == 0x11) {
        return;
    }

    if (version == 0) {
        version = pmu_get_register_value(PMU_HWCID_L_ADDR  , PMU_HWCID_L_MASK    , PMU_HWCID_L_SHIFT);
    }

    if (enable == 1) {
        if (resoure_count == 0) {
            PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VA28, PMIC_PSI_S0, PMIC_SLP_WKUP_SW_CTL, 0);
            PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VA28, PMIC_PSI_S1, PMIC_SLP_WKUP_SW_CTL, 0);
        }
        resoure_count++;
    } else {
        if (resoure_count <= 1) {
            PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VA28, PMIC_PSI_S0, PMIC_SLP_WKUP_HW_CTL, 0);
            PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VA28, PMIC_PSI_S1, PMIC_SLP_WKUP_HW_CTL, 0);
            resoure_count = 0;
        } else {
            resoure_count--;
        }
    }
}

void pmu_ctrl_vbt_ldo_mt6385(unsigned char enable)
{
    if (enable == 1) {
        PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VBT, PMIC_PSI_S0, PMIC_SLP_WKUP_HW_CTL, 2);
        PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VBT, PMIC_PSI_S1, PMIC_SLP_WKUP_HW_CTL, 2);
        PMIC_VR_CONTROL(PMIC_VBT, PMIC_VR_CTL_ENABLE);
    } else {
        PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VBT, PMIC_PSI_S0, PMIC_SLP_WKUP_SW_CTL, 2);
        PMIC_VR_SLEEP_WKUP_CONFIG(PMIC_VBT, PMIC_PSI_S1, PMIC_SLP_WKUP_SW_CTL, 2);
        PMIC_VR_CONTROL(PMIC_VBT, PMIC_VR_CTL_DISABLE);
    }
}

void pmu_set_mtcmos_mt6385(unsigned char setting)
{
    //unlock key
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR , PMU_RG_VRC_CFG_KEY0_MASK   , PMU_RG_VRC_CFG_KEY0_SHIFT  , 0xFC);
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR , PMU_RG_VRC_CFG_KEY1_MASK   , PMU_RG_VRC_CFG_KEY1_SHIFT  , 0xDA);

    pmu_set_register_value(PMU_PPCMTCTL1    , 0xFF   , 0 , setting); //MTCMOS Keep power setting when enter S0,S1

    //lock key
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR , PMU_RG_VRC_CFG_KEY0_MASK   , PMU_RG_VRC_CFG_KEY0_SHIFT  , 0);
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR , PMU_RG_VRC_CFG_KEY1_MASK   , PMU_RG_VRC_CFG_KEY1_SHIFT  , 0);
}

PMIC_VCORE_VOSEL pmu_get_vcore_setting_index(unsigned char buck_vaval)
{
    unsigned char vcbuck_voval[7] = {2, 4, 6, 8, 10, 12, 14};
    unsigned int vosel;

    for (vosel = 0; vosel < 7; vosel++) {
        if (buck_vaval == vcbuck_voval[vosel]) {
            return ((PMIC_VCORE_VOSEL)vosel);
        }
    }
    return (PMIC_VCORE_ERROR);
}

#define PMU_PSI_MST_STATE   ((volatile uint32_t*)(0xA20C0000+0x34))  /*PSI BUS STATE*/
unsigned char pmu_set_vcore_voltage_mt6385(unsigned char lock, unsigned char vcore_vosel)
{
    static unsigned char Vcore_Resource_Ctrl[7], origin_voltage, old_vcore, old_vcore_lp, init = 0;
    int vol_index;

    if (init == 0) {
        old_vcore_lp = pmu_get_register_value(PMU_RG_LP_VC_BUCK_VOVAL_ADDR , PMU_RG_LP_VC_BUCK_VOVAL_MASK   , PMU_RG_LP_VC_BUCK_VOVAL_SHIFT);
        old_vcore_lp = pmu_get_vcore_setting_index(old_vcore_lp);
        old_vcore    = pmu_get_register_value(PMU_RG_HP_VC_BUCK_VOVAL_ADDR , PMU_RG_HP_VC_BUCK_VOVAL_MASK   , PMU_RG_HP_VC_BUCK_VOVAL_SHIFT);
        old_vcore    = pmu_get_vcore_setting_index(old_vcore);

        if (*PMU_PSI_MST_STATE == 0) {
            //*PSI_MST_STATE:LP = 0
            origin_voltage = old_vcore_lp;
        } else {
            //*PSI_MST_STATE:HP = 1
            origin_voltage = old_vcore;
        }
        old_vcore = origin_voltage;
        init = 1;
    }
    /* parameter check */
    if ((vcore_vosel > PMIC_VCORE_1P3V) || (vcore_vosel < PMIC_VCORE_0P9V)) {
        return (PMIC_VCORE_ERROR);
    }

    if (lock == PMIC_VCORE_LOCK) {
        Vcore_Resource_Ctrl[vcore_vosel]++;
    } else {
        if (Vcore_Resource_Ctrl[vcore_vosel] != 0) {
            Vcore_Resource_Ctrl[vcore_vosel]--;
        }
    }
    //Find Highest Vcore Voltage
    for (vol_index = 6; vol_index >= 0; vol_index--) {
        if (Vcore_Resource_Ctrl[vol_index] != 0) {
            break;
        }
    }
    if (vol_index < 0) {
        //Cna't find any Vcore Ctrl request
        vol_index = origin_voltage;
    }

    if (vol_index != old_vcore) {
        old_vcore = vol_index;

        if (vol_index == PMIC_VCORE_1P3V) {
            if (*PMU_PSI_MST_STATE == 0) {
                PSI_LP_RUN_AT_HP(1);
            }
        } else {
            if (vol_index != old_vcore_lp) {
                old_vcore_lp = vol_index;
                PMIC_VCORE_VOSEL_CONFIG(PMIC_PSI_LP, (PMIC_VCORE_VOSEL)vol_index);
            }
            if (*PMU_PSI_MST_STATE == 1) {
                PSI_HP_RUN_AT_LP(1);
            }
        }
    }
    return (vol_index);
}

unsigned char pmu_get_vcore_voltage_mt6385(void)
{
    //only use HP,LP Mode
    unsigned char psi_status, vcore;

    psi_status = pmu_get_register_value(PMU_RGS_PSI_STS_ADDR , PMU_RGS_PSI_STS_MASK   , PMU_RGS_PSI_STS_SHIFT);

    if (psi_status == 0x3) {
        //LP Mode
        vcore = pmu_get_register_value(PMU_RG_LP_VC_BUCK_VOVAL_ADDR , PMU_RG_LP_VC_BUCK_VOVAL_MASK   , PMU_RG_LP_VC_BUCK_VOVAL_SHIFT);
        vcore = pmu_get_vcore_setting_index(vcore);
    } else {
        //HP Mode
        vcore = pmu_get_register_value(PMU_RG_HP_VC_BUCK_VOVAL_ADDR , PMU_RG_HP_VC_BUCK_VOVAL_MASK   , PMU_RG_HP_VC_BUCK_VOVAL_SHIFT);
        vcore = pmu_get_vcore_setting_index(vcore);
    }
    return ((PMIC_VCORE_VOSEL)vcore);
}

void pmu_set_vcore_s1_buck_mt6385(void)
{
    static PMIC_VCORE_VOSEL S1_setting = PMIC_VCORE_ERROR, current_setting;
    int vcbuck_voval[7] = {2, 4, 6, 8, 10, 12, 14};

    current_setting = (PMIC_VCORE_VOSEL)pmu_get_vcore_voltage();

    if (current_setting != S1_setting) {
        S1_setting = current_setting;
        pmu_set_vrc_key_mt6385(PMIC_UNLOCK_KEY);
        pmu_set_ovr_mode_mt6385(PMIC_ENTER_OVR_MODE);
        pmu_set_register_value(PMU_RG_S1_VC_BUCK_VOVAL_ADDR     , PMU_RG_S1_VC_BUCK_VOVAL_MASK   , PMU_RG_S1_VC_BUCK_VOVAL_SHIFT  , vcbuck_voval[S1_setting]);
        pmu_set_ovr_mode_mt6385(PMIC_LEAVE_OVR_MODE);
        pmu_set_vrc_key_mt6385(PMIC_LOCK_KEY);
    }
}

unsigned char pmu_get_usb_input_status_mt6385(void)
{
    char ret;
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR , PMU_RG_VRC_CFG_KEY0_MASK , PMU_RG_VRC_CFG_KEY0_SHIFT, 0xFC);
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR , PMU_RG_VRC_CFG_KEY1_MASK , PMU_RG_VRC_CFG_KEY1_SHIFT, 0xDA);

    ret = pmu_get_register_value(PMU_RGS_CHRDET_ADDR, PMU_RGS_CHRDET_MASK, PMU_RGS_CHRDET_SHIFT);

    // Clear protection key
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR, PMU_RG_VRC_CFG_KEY0_MASK, PMU_RG_VRC_CFG_KEY0_SHIFT  , 0x0);
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR, PMU_RG_VRC_CFG_KEY1_MASK, PMU_RG_VRC_CFG_KEY1_SHIFT  , 0x0);
    return (ret);
}

void pmu_enter_power_off_mt6385(void)
{
    pmu_set_register_value(PMU_RG_PWRHOLD_ADDR, PMU_RG_PWRHOLD_MASK, PMU_RG_PWRHOLD_SHIFT, 0);
}

void pmu_reset_mt6385(void)
{
    //unlock key
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR , PMU_RG_VRC_CFG_KEY0_MASK   , PMU_RG_VRC_CFG_KEY0_SHIFT  , 0xFC);
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR , PMU_RG_VRC_CFG_KEY1_MASK   , PMU_RG_VRC_CFG_KEY1_SHIFT  , 0xDA);

    pmu_set_register_value(PMU_RG_CRST_ADDR    , PMU_RG_CRST_MASK   , PMU_RG_CRST_SHIFT , 1);

    //lock key
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR , PMU_RG_VRC_CFG_KEY0_MASK   , PMU_RG_VRC_CFG_KEY0_SHIFT  , 0);
    pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR , PMU_RG_VRC_CFG_KEY1_MASK   , PMU_RG_VRC_CFG_KEY1_SHIFT  , 0);
}

void pmu_set_ovr_mode_mt6385(PMIC_SET_OVR_MODE mode)
{
    if(mode == PMIC_LEAVE_OVR_MODE) {
        hal_gpt_delay_us(500);
        pmu_set_register_value(PMU_RG_PSIOVR_ADDR           , PMU_RG_PSIOVR_MASK          , PMU_RG_PSIOVR_SHIFT        , 0x0); // switch to PSI Bus
    }else {
        pmu_set_register_value(PMU_RG_PSIOVR_ADDR           , PMU_RG_PSIOVR_MASK         , PMU_RG_PSIOVR_SHIFT        , 0x1); // switch to PSIOVR        
    }
}

void pmu_set_vrc_key_mt6385(PMIC_SET_KEY mode)
{
    if(mode == PMIC_LOCK_KEY) {
        pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR     , PMU_RG_VRC_CFG_KEY0_MASK    , PMU_RG_VRC_CFG_KEY0_SHIFT  , 0x0);
        pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR     , PMU_RG_VRC_CFG_KEY1_MASK    , PMU_RG_VRC_CFG_KEY1_SHIFT  , 0x0);         
    }else {
        pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR     , PMU_RG_VRC_CFG_KEY0_MASK   , PMU_RG_VRC_CFG_KEY0_SHIFT  , 0xFC);
        pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR     , PMU_RG_VRC_CFG_KEY1_MASK   , PMU_RG_VRC_CFG_KEY1_SHIFT  , 0xDA);       
    }
}

#endif /* HAL_PMU_MODULE_ENABLED */

