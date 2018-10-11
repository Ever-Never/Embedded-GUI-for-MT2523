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

#ifndef _HAL_RTC_INTERNAL_H_
#define _HAL_RTC_INTERNAL_H_

#include "hal_pmu.h"

#define RTC_KEY_BBPU                        (0x4300)

#define RTC_PWREN_OFFSET                    (0)
#define RTC_PWREN_MASK                      (0x1 << RTC_PWREN_OFFSET)
#define RTC_ALARM_PU_OFFSET                 (2)
#define RTC_ALARM_PU_MASK                   (0x1 << RTC_ALARM_PU_OFFSET)
#define RTC_RELOAD_OFFSET                   (5)
#define RTC_RELOAD_MASK                     (0x1 << RTC_RELOAD_OFFSET)
#define RTC_CBUSY_OFFSET                    (6)
#define RTC_CBUSY_MASK                      (0x1 << RTC_CBUSY_OFFSET)

#define RTC_ALSTA_OFFSET                    (0)
#define RTC_ALSTA_MASK                      (0x1 << RTC_ALSTA_OFFSET)
#define RTC_TCSTA_OFFSET                    (1)
#define RTC_TCSTA_MASK                      (0x1 << RTC_TCSTA_OFFSET)
#define RTC_LPSTA_OFFSET                    (3)
#define RTC_LPSTA_MASK                      (0x1 << RTC_LPSTA_OFFSET)

#define RTC_AL_EN_OFFSET                    (0)
#define RTC_AL_EN_MASK                      (0x1 << RTC_AL_EN_OFFSET)
#define RTC_TC_EN_OFFSET                    (1)
#define RTC_TC_EN_MASK                      (0x1 << RTC_TC_EN_OFFSET)
#define RTC_ONESHOT_OFFSET                  (2)
#define RTC_ONESHOT_MASK                    (0x1 << RTC_ONESHOT_OFFSET)
#define RTC_LP_EN_OFFSET                    (3)
#define RTC_LP_EN_MASK                      (0x1 << RTC_LP_EN_OFFSET)

#define RTC_SECCII_OFFSET                   (0)
#define RTC_SECCII_MASK                     (0x1 << RTC_SECCII_OFFSET)
#define RTC_MINCII_OFFSET                   (1)
#define RTC_MINCII_MASK                     (0x1 << RTC_MINCII_OFFSET)
#define RTC_HOUCII_OFFSET                   (2)
#define RTC_HOUCII_MASK                     (0x1 << RTC_HOUCII_OFFSET)
#define RTC_DOMCII_OFFSET                   (3)
#define RTC_DOMCII_MASK                     (0x1 << RTC_DOMCII_OFFSET)
#define RTC_DOWCII_OFFSET                   (4)
#define RTC_DOWCII_MASK                     (0x1 << RTC_DOWCII_OFFSET)
#define RTC_MTHCII_OFFSET                   (5)
#define RTC_MTHCII_MASK                     (0x1 << RTC_MTHCII_OFFSET)
#define RTC_YEACII_OFFSET                   (6)
#define RTC_YEACII_MASK                     (0x1 << RTC_YEACII_OFFSET)
#define RTC_SECCII_1_2_OFFSET               (7)
#define RTC_SECCII_1_2_MASK                 (0x1 << RTC_SECCII_1_2_OFFSET)
#define RTC_SECCII_1_4_OFFSET               (8)
#define RTC_SECCII_1_4_MASK                 (0x1 << RTC_SECCII_1_4_OFFSET)
#define RTC_SECCII_1_8_OFFSET               (9)
#define RTC_SECCII_1_8_MASK                 (0x1 << RTC_SECCII_1_8_OFFSET)

#define RTC_SEC_MSK_OFFSET                  (0)
#define RTC_SEC_MSK_MASK                    (0x1 << RTC_SEC_MSK_OFFSET)
#define RTC_MIN_MSK_OFFSET                  (1)
#define RTC_MIN_MSK_MASK                    (0x1 << RTC_MIN_MSK_OFFSET)
#define RTC_HOU_MSK_OFFSET                  (2)
#define RTC_HOU_MSK_MASK                    (0x1 << RTC_HOU_MSK_OFFSET)
#define RTC_DOM_MSK_OFFSET                  (3)
#define RTC_DOM_MSK_MASK                    (0x1 << RTC_DOM_MSK_OFFSET)
#define RTC_DOW_MSK_OFFSET                  (4)
#define RTC_DOW_MSK_MASK                    (0x1 << RTC_DOW_MSK_OFFSET)
#define RTC_MTH_MSK_OFFSET                  (5)
#define RTC_MTH_MSK_MASK                    (0x1 << RTC_MTH_MSK_OFFSET)
#define RTC_YEA_MSK_OFFSET                  (6)
#define RTC_YEA_MSK_MASK                    (0x1 << RTC_YEA_MSK_OFFSET)

#define RTC_TC_SECOND_OFFSET                (0)
#define RTC_TC_SECOND_MASK                  (0x3F << RTC_TC_SECOND_OFFSET)

#define RTC_TC_MINUTE_OFFSET                (0)
#define RTC_TC_MINUTE_MASK                  (0x3F << RTC_TC_MINUTE_OFFSET)

#define RTC_TC_HOUR_OFFSET                  (0)
#define RTC_TC_HOUR_MASK                    (0x1F << RTC_TC_HOUR_OFFSET)

#define RTC_TC_DOM_OFFSET                   (0)
#define RTC_TC_DOM_MASK                     (0x1F << RTC_TC_DOM_OFFSET)

#define RTC_TC_DOW_OFFSET                   (0)
#define RTC_TC_DOW_MASK                     (0x7 << RTC_TC_DOW_OFFSET)

#define RTC_TC_MONTH_OFFSET                 (0)
#define RTC_TC_MONTH_MASK                   (0xF << RTC_TC_MONTH_OFFSET)

#define RTC_TC_YEAR_OFFSET                  (0)
#define RTC_TC_YEAR_MASK                    (0x7F << RTC_TC_YEAR_OFFSET)

#define RTC_AL_SECOND_OFFSET                (0)
#define RTC_AL_SECOND_MASK                  (0x3F << RTC_AL_SECOND_OFFSET)
#define RTC_RTC_LPD_OPT_OFFSET              (12)
#define RTC_RTC_LPD_OPT_MASK                (0x3 << RTC_RTC_LPD_OPT_OFFSET)
#define RTC_K_EOSC32_VTCXO_ON_SEL_OFFSET    (15)
#define RTC_K_EOSC32_VTCXO_ON_SEL_MASK      (0x1 << RTC_K_EOSC32_VTCXO_ON_SEL_OFFSET)

#define RTC_LPD_OPT_XOSC_EOSC               (0x0)
#define RTC_LPD_OPT_EOSC                    (0x1)
#define RTC_LPD_OPT_XOSC                    (0x2)
#define RTC_LPD_OPT_NO                      (0x3)

#define RTC_AL_MINUTE_OFFSET                (0)
#define RTC_AL_MINUTE_MASK                  (0x3F << RTC_AL_MINUTE_OFFSET)
#define RTC_OSC32CON_RD_SEL_OFFSET          (8)
#define RTC_OSC32CON_RD_SEL_MASK            (0x3 << RTC_OSC32CON_RD_SEL_OFFSET)

#define RTC_AL_HOUR_OFFSET                  (0)
#define RTC_AL_HOUR_MASK                    (0x1F << RTC_AL_HOUR_OFFSET)
#define RTC_NEW_SPARE0_OFFSET               (8)
#define RTC_NEW_SPARE0_MASK                 (0xFF << RTC_NEW_SPARE0_OFFSET)

#define RTC_AL_DOM_OFFSET                   (0)
#define RTC_AL_DOM_MASK                     (0x1F << RTC_AL_DOM_OFFSET)
#define RTC_NEW_SPARE1_OFFSET               (8)
#define RTC_NEW_SPARE1_MASK                 (0xFF << RTC_NEW_SPARE1_OFFSET)

#define RTC_AL_DOW_OFFSET                   (0)
#define RTC_AL_DOW_MASK                     (0x7 << RTC_AL_DOW_OFFSET)
#define RTC_NEW_SPARE2_OFFSET               (8)
#define RTC_NEW_SPARE2_MASK                 (0xFF << RTC_NEW_SPARE2_OFFSET)

#define RTC_AL_MONTH_OFFSET                 (0)
#define RTC_AL_MONTH_MASK                   (0xF << RTC_AL_MONTH_OFFSET)
#define RTC_NEW_SPARE3_OFFSET               (8)
#define RTC_NEW_SPARE3_MASK                 (0xFF << RTC_NEW_SPARE3_OFFSET)

#define RTC_AL_YEAR_OFFSET                  (0)
#define RTC_AL_YEAR_MASK                    (0x7F << RTC_AL_YEAR_OFFSET)
#define RTC_NEW_SPARE4_OFFSET               (8)
#define RTC_NEW_SPARE4_MASK                 (0xFF << RTC_NEW_SPARE4_OFFSET)

#define RTC_RTC_D2D_32K_CG_EN_OFFSET        (3)
#define RTC_RTC_D2D_32K_CG_EN_MASK          (0x1 << RTC_RTC_D2D_32K_CG_EN_OFFSET)
#define RTC_SW_PWKEY_RST_OFFSET             (4)
#define RTC_SW_PWKEY_RST_MASK               (0x1 << RTC_SW_PWKEY_RST_OFFSET)
#define RTC_RTC_XOSC32_ENB_OFFSET           (5)
#define RTC_RTC_XOSC32_ENB_MASK             (0x1 << RTC_RTC_XOSC32_ENB_OFFSET)
#define RTC_RTC_EMBCK_SEL_MODE_OFFSET       (6)
#define RTC_RTC_EMBCK_SEL_MODE_MASK         (0x3 << RTC_RTC_EMBCK_SEL_MODE_OFFSET)
#define RTC_RTC_EMBCK_SRC_SEL_OFFSET        (8)
#define RTC_RTC_EMBCK_SRC_SEL_MASK          (0x1 << RTC_RTC_EMBCK_SRC_SEL_OFFSET)
#define RTC_RTC_EMBCK_SEL_OPTION_OFFSET     (9)
#define RTC_RTC_EMBCK_SEL_OPTION_MASK       (0x1 << RTC_RTC_EMBCK_SEL_OPTION_OFFSET)
#define RTC_RTC_DDLO_32K_SRC_SEL_OFFSET     (10)
#define RTC_RTC_DDLO_32K_SRC_SEL_MASK       (0x1 << RTC_RTC_DDLO_32K_SRC_SEL_OFFSET)
#define RTC_RTC_AREA_CK_CGEN_OFFSET         (11)
#define RTC_RTC_AREA_CK_CGEN_MASK           (0x1 << RTC_RTC_AREA_CK_CGEN_OFFSET)
#define RTC_RTC_32K_EN_OFFSET               (12)
#define RTC_RTC_32K_EN_MASK                 (0x1 << RTC_RTC_32K_EN_OFFSET)
#define RTC_RTC_OSC32_GPSCK_EN_OFFSET       (13)
#define RTC_RTC_OSC32_GPSCK_EN_MASK         (0x1 << RTC_RTC_OSC32_GPSCK_EN_OFFSET)
#define RTC_XRTC_EN_OFFSET                  (14)
#define RTC_XRTC_EN_MASK                    (0x1 << RTC_XRTC_EN_OFFSET)
#define RTC_RTC_REG_XOSC32_ENB_OFFSET       (15)
#define RTC_RTC_REG_XOSC32_ENB_MASK         (0x1 << RTC_RTC_REG_XOSC32_ENB_OFFSET)

#define RTC_XOSCCALI_OFFSET                 (0)
#define RTC_XOSCCALI_MASK                   (0x1F << RTC_XOSCCALI_OFFSET)
#define RTC_XOSC_STP_PWD_OFFSET             (5)
#define RTC_XOSC_STP_PWD_MASK               (0x1 << RTC_XOSC_STP_PWD_OFFSET)
#define RTC_XOSC_CGM_LPEN_OFFSET            (6)
#define RTC_XOSC_CGM_LPEN_MASK              (0x1 << RTC_XOSC_CGM_LPEN_OFFSET)
#define RTC_XOSC_CORE_LPEN_OFFSET           (7)
#define RTC_XOSC_CORE_LPEN_MASK             (0x1 << RTC_XOSC_CORE_LPEN_OFFSET)
#define RTC_XOSC_CL_EN_OFFSET               (8)
#define RTC_XOSC_CL_EN_MASK                 (0x1 << RTC_XOSC_CL_EN_OFFSET)
#define RTC_XOSC_CL_SW_OFFSET               (9)
#define RTC_XOSC_CL_SW_MASK                 (0x1 << RTC_XOSC_CL_SW_OFFSET)
#define RTC_EOSC32_CHOP_EN_OFFSET           (10)
#define RTC_EOSC32_CHOP_EN_MASK             (0x1 << RTC_EOSC32_CHOP_EN_OFFSET)
#define RTC_EOSC32_VCT_EN_OFFSET            (11)
#define RTC_EOSC32_VCT_EN_MASK              (0x1 << RTC_EOSC32_VCT_EN_OFFSET)
#define RTC_EOSC32_RSV_OFFSET               (12)
#define RTC_EOSC32_RSV_MASK                 (0xF << RTC_EOSC32_RSV_OFFSET)

#define RTC_SCXO_EN_OFFSET                  (0)
#define RTC_SCXO_EN_MASK                    (0x1 << RTC_SCXO_EN_OFFSET)
#define RTC_SCXO_LDO_EN_OFFSET              (1)
#define RTC_SCXO_LDO_EN_MASK                (0x1 << RTC_SCXO_LDO_EN_OFFSET)
#define RTC_SCXO_LVSH_ENB_OFFSET            (2)
#define RTC_SCXO_LVSH_ENB_MASK              (0x1 << RTC_SCXO_LVSH_ENB_OFFSET)
#define RTC_SCXO_RST_OFFSET                 (3)
#define RTC_SCXO_RST_MASK                   (0x1 << RTC_SCXO_RST_OFFSET)
#define RTC_SCXO_SW_EN_OFFSET               (4)
#define RTC_SCXO_SW_EN_MASK                 (0x1 << RTC_SCXO_SW_EN_OFFSET)
#define RTC_SCXO_DRV_OFFSET                 (5)
#define RTC_SCXO_DRV_MASK                   (0x3 << RTC_SCXO_DRV_OFFSET)
#define RTC_SCXO_ED_SEL_OFFSET              (7)
#define RTC_SCXO_ED_SEL_MASK                (0x1 << RTC_SCXO_ED_SEL_OFFSET)
#define RTC_SCXO_CHP_EN_OFFSET              (8)
#define RTC_SCXO_CHP_EN_MASK                (0x1 << RTC_SCXO_CHP_EN_OFFSET)
#define RTC_SCXO_IB_SET_OFFSET              (9)
#define RTC_SCXO_IB_SET_MASK                (0x3 << RTC_SCXO_IB_SET_OFFSET)
#define RTC_SCXO_CK_ENB_OFFSET              (11)
#define RTC_SCXO_CK_ENB_MASK                (0x1 << RTC_SCXO_CK_ENB_OFFSET)
#define RTC_XOSC_RSV_OFFSET                 (12)
#define RTC_XOSC_RSV_MASK                   (0xF << RTC_XOSC_RSV_OFFSET)

#define RTC_RTC_POWERKEY1_OFFSET            (0)
#define RTC_RTC_POWERKEY1_MASK              (0xFFFF << RTC_RTC_POWERKEY1_OFFSET)

#define RTC_RTC_POWERKEY2_OFFSET            (0)
#define RTC_RTC_POWERKEY2_MASK              (0xFFFF << RTC_RTC_POWERKEY2_OFFSET)

#define RTC_RTC_PDN1_OFFSET                 (0)
#define RTC_RTC_PDN1_MASK                   (0xFFFF << RTC_RTC_PDN1_OFFSET)
#define RTC_RTC_PDN1_1_OFFSET               (0)
#define RTC_RTC_PDN1_1_MASK                 (0xFF << RTC_RTC_PDN1_1_OFFSET)
#define RTC_RTC_PDN1_2_OFFSET               (8)
#define RTC_RTC_PDN1_2_MASK                 (0xFF << RTC_RTC_PDN1_2_OFFSET)

#define RTC_RTC_PDN2_OFFSET                 (0)
#define RTC_RTC_PDN2_MASK                   (0xFFFF << RTC_RTC_PDN2_OFFSET)
#define RTC_RTC_PDN2_1_OFFSET               (0)
#define RTC_RTC_PDN2_1_MASK                 (0xFF << RTC_RTC_PDN2_1_OFFSET)
#define RTC_RTC_PDN2_2_OFFSET               (8)
#define RTC_RTC_PDN2_2_MASK                 (0xFF << RTC_RTC_PDN2_2_OFFSET)

#define RTC_RTC_SPAR0_OFFSET                (0)
#define RTC_RTC_SPAR0_MASK                  (0xFFFF << RTC_RTC_SPAR0_OFFSET)
#define RTC_RTC_SPAR0_1_OFFSET              (0)
#define RTC_RTC_SPAR0_1_MASK                (0xFF << RTC_RTC_SPAR0_1_OFFSET)
#define RTC_RTC_SPAR0_2_OFFSET              (8)
#define RTC_RTC_SPAR0_2_MASK                (0xFF << RTC_RTC_SPAR0_2_OFFSET)

#define RTC_RTC_SPAR1_OFFSET                (0)
#define RTC_RTC_SPAR1_MASK                  (0xFFFF << RTC_RTC_SPAR1_OFFSET)
#define RTC_RTC_SPAR1_1_OFFSET              (0)
#define RTC_RTC_SPAR1_1_MASK                (0xFF << RTC_RTC_SPAR1_1_OFFSET)
#define RTC_RTC_SPAR1_2_OFFSET              (8)
#define RTC_RTC_SPAR1_2_MASK                (0xFF << RTC_RTC_SPAR1_2_OFFSET)

#define RTC_RTC_PROT_OFFSET                 (0)
#define RTC_RTC_PROT_MASK                   (0xFFFF << RTC_RTC_PROT_OFFSET)

#define RTC_RTC_DIFF_OFFSET                 (0)
#define RTC_RTC_DIFF_MASK                   (0xFFF << RTC_RTC_DIFF_OFFSET)
#define RTC_POWER_DETECTED_OFFSET           (12)
#define RTC_POWER_DETECTED_MASK             (0x1 << RTC_POWER_DETECTED_OFFSET)
#define RTC_K_EOSC32_RSV_OFFSET             (14)
#define RTC_K_EOSC32_RSV_MASK               (0x1 << RTC_K_EOSC32_RSV_OFFSET)
#define RTC_CALI_RD_SEL_OFFSET              (15)
#define RTC_CALI_RD_SEL_MASK                (0x1 << RTC_CALI_RD_SEL_OFFSET)

#define RTC_RTC_CALI_OFFSET                 (0)
#define RTC_RTC_CALI_MASK                   (0x3FFF << RTC_RTC_CALI_OFFSET)
#define RTC_CALI_WR_SEL_OFFSET              (14)
#define RTC_CALI_WR_SEL_MASK                (0x1 << RTC_CALI_WR_SEL_OFFSET)
#define RTC_K_EOSC32_OVERFLOW_OFFSET        (15)
#define RTC_K_EOSC32_OVERFLOW_MASK          (0x1 << RTC_K_EOSC32_OVERFLOW_OFFSET)

#define RTC_WRTGR_OFFSET                    (0)
#define RTC_WRTGR_MASK                      (0x1 << RTC_WRTGR_OFFSET)

#define RTC_VBAT_LPSTA_RAW_OFFSET           (0)
#define RTC_VBAT_LPSTA_RAW_MASK             (0x1 << RTC_VBAT_LPSTA_RAW_OFFSET)
#define RTC_EOSC32_LPEN_OFFSET              (1)
#define RTC_EOSC32_LPEN_MASK                (0x1 << RTC_EOSC32_LPEN_OFFSET)
#define RTC_XOSC32_LPEN_OFFSET              (2)
#define RTC_XOSC32_LPEN_MASK                (0x1 << RTC_XOSC32_LPEN_OFFSET)
#define RTC_LPRST_OFFSET                    (3)
#define RTC_LPRST_MASK                      (0x1 << RTC_LPRST_OFFSET)
#define RTC_CDBO_OFFSET                     (4)
#define RTC_CDBO_MASK                       (0x1 << RTC_CDBO_OFFSET)
#define RTC_F32KOB_OFFSET                   (5)
#define RTC_F32KOB_MASK                     (0x1 << RTC_F32KOB_OFFSET)
#define RTC_GPO_OFFSET                      (6)
#define RTC_GPO_MASK                        (0x1 << RTC_GPO_OFFSET)
#define RTC_GOE_OFFSET                      (7)
#define RTC_GOE_MASK                        (0x1 << RTC_GOE_OFFSET)
#define RTC_RTC_MODE_OFFSET                 (8)
#define RTC_RTC_MODE_MASK                   (0x1 << RTC_RTC_MODE_OFFSET)
#define RTC_XTAL_TYPE_OFFSET                (9)
#define RTC_XTAL_TYPE_MASK                  (0x1 << RTC_XTAL_TYPE_OFFSET)
#define RTC_GPEN_OFFSET                     (10)
#define RTC_GPEN_MASK                       (0x1 << RTC_GPEN_OFFSET)
#define RTC_GPU_OFFSET                      (11)
#define RTC_GPU_MASK                        (0x1 << RTC_GPU_OFFSET)
#define RTC_XRTC_INT_G_OFFSET               (12)
#define RTC_XRTC_INT_G_MASK                 (0x1 << RTC_XRTC_INT_G_OFFSET)
#define RTC_XRTC_INT_OE_OFFSET              (13)
#define RTC_XRTC_INT_OE_MASK                (0x1 << RTC_XRTC_INT_OE_OFFSET)
#define RTC_GPI_OFFSET                      (14)
#define RTC_GPI_MASK                        (0x1 << RTC_GPI_OFFSET)
#define RTC_LPSTA_RAW_OFFSET                (15)
#define RTC_LPSTA_RAW_MASK                  (0x1 << RTC_LPSTA_RAW_OFFSET)

#define PMIC_RTC_SPAR_2SEC_EN_OFFSET        (0)
#define PMIC_RTC_SPAR_2SEC_EN_MASK          (0x1 << PMIC_RTC_SPAR_2SEC_EN_OFFSET)
#define PMIC_RTC_SPAR_2SEC_MODE_OFFSET      (1)
#define PMIC_RTC_SPAR_2SEC_MODE_MASK        (0x3 << PMIC_RTC_SPAR_2SEC_MODE_OFFSET)
#define PMIC_SPAR_2SEC_STAT_CLEAR_OFFSET    (3)
#define PMIC_SPAR_2SEC_STAT_CLEAR_MASK      (0x1 << PMIC_SPAR_2SEC_STAT_CLEAR_OFFSET)
#define PMIC_SPAR_2SEC_STAT_STA_OFFSET      (4)
#define PMIC_SPAR_2SEC_STAT_STA_MASK        (0x1 << PMIC_SPAR_2SEC_STAT_STA_OFFSET)

#define PMIC_RTC_XOSC32_ENB_OFFSET          (0)
#define PMIC_RTC_XOSC32_ENB_MASK            (0x1 << PMIC_RTC_XOSC32_ENB_OFFSET)
#define PMIC_RTC_K_EOSC32_EN_OFFSET         (1)
#define PMIC_RTC_K_EOSC32_EN_MASK           (0x1 << PMIC_RTC_K_EOSC32_EN_OFFSET)

#define PMIC_RG_D2D_F32K_CK_PDN_OFFSET      (1)
#define PMIC_RG_D2D_F32K_CK_PDN_MASK        (0x1 << PMIC_RG_D2D_F32K_CK_PDN_OFFSET)

#define RTC_POWERKEY1_KEY                   0xa357
#define RTC_POWERKEY2_KEY                   0x67d2

#define RTC_PROTECT1                        0x586a
#define RTC_PROTECT2                        0x9136

#define RTC_OSC32CON_MAGIC_KEY_1            0x1a57
#define RTC_OSC32CON_MAGIC_KEY_2            0x2b68

#define RTC_OSC32CON1_MAGIC_KEY_1           0x1a85
#define RTC_OSC32CON1_MAGIC_KEY_2           0xe7cf

#define RTC_OSC32CON2_MAGIC_KEY_1           0x1653
#define RTC_OSC32CON2_MAGIC_KEY_2           0x8918

#define ABIST_FQMTR_BASE                    (MIXED_BASE + 0x400)

/* frequency meter start */
typedef struct {
    __IO uint16_t ABIST_FQMTR_CON0;     /* Frequency Meter Control Register 0 */
    __IO uint16_t RESERVED1;
    __IO uint16_t ABIST_FQMTR_CON1;     /* Frequency Meter Control Register 1 */
    __IO uint16_t RESERVED2;
    __IO uint16_t ABIST_FQMTR_CON2;     /* Frequency Meter Control Register 2 */
    __IO uint16_t RESERVED3;
    __IO uint16_t ABIST_FQMTR_DATA;     /* Frequency Meter Data */
    __IO uint16_t RESERVED4;
    __IO uint16_t ABIST_FQMTR_DATA_MSB; /* Frequency Meter Data */
} ABIST_FQMTR_REGISTER_T;
/* frequency meter end */

/* For internal user request, return frequency meter count with specific test. */
uint32_t f32k_measure_count(uint16_t fixed_clock, uint16_t tested_clock, uint16_t window_setting);

#endif /* #ifndef _HAL_RTC_INTERNAL_H_ */
