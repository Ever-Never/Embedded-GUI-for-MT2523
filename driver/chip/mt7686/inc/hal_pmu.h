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

#ifndef __HAL_PMU_H__
#define __HAL_PMU_H__
#include <stdio.h>
#include <stdlib.h>
#include "hal_platform.h"
#include "hal_pmu_wrap_interface.h"

#ifdef HAL_PMU_MODULE_ENABLED

int  pmu_count_bit_1(uint32_t n);
void pmu_init_mt7686(void);
void pmu_init_power_mode_mt7686(void);
uint32_t pmu_get_vcore_voltage_mt7686(void); /* 6'b000000: 1.3V ;  * 6'b001100: 1.45V ;  * 6'b010000: 1.5V ; * 6'b100000: 1.7V ; * 6'b111111: 2.0875V ; */
void pmu_set_vcore_voltage_mt7686(hal_pmu_stage mode,pmu_vcore_vosel_t vol);
void pmu_enter_sleep_mode_mt7686(void);
void pmu_enter_retention_mode_mt7686(pmu_retention_status_t status);
void pmu_resume_to_normal_mt7686(hal_pmu_stage_flow mode);
uint32_t pmu_get_register_value(uint32_t address, short int mask, short int shift);
void pmu_set_register_value(uint32_t address, short int mask, short int shift, short int value);
void pmu_set_cldo_voltage_mt7686(int index);
void pmu_set_mldo_voltage_mt7686(int adjust_value);
void pmu_retention_ldo_control_mt7686(pmu_control_t status);
void pmu_sram_power_switch_control_mt7686(pmu_control_t status);
void pmu_trigger_wd_cold_reset(void);
void pmu_buck_force_mode(pmu_force_mode_t mode);
void pmu_set_retention_ldo_mt7686(int adjust_value); /*3b'000: 855mV ; 3b'001:905mV; 3b'010:955mV; 3b'011:1.005V(default); 3b'100:1.056V; 3b'101:1.106V; 3b'110:1.156V; 3b'111:1.206V*/

void hal_pmu_init(void);
void hal_pmu_init_power_mode(void);
void hal_pmu_set_vcore_voltage(hal_pmu_stage mode,pmu_vcore_vosel_t vol);
void hal_pmu_enter_sleep_mode(void);
void hal_pmu_enter_retention_mode(pmu_retention_status_t status);
void hal_pmu_resume_to_normal(hal_pmu_stage_flow mode);
void hal_pmu_set_cldo_voltage(int index);
void hal_pmu_set_mldo_voltage(int adjust_value);
void hal_pmu_retention_ldo_control(pmu_control_t status);
void hal_pmu_sram_power_switch_control(pmu_control_t status);
void hal_pmu_set_retention_ldo_voltage(int adjust_value);

#define EFUSE_VALID                ((volatile uint32_t*)(0xA20A0308))

//#define PMU_BASE             0xA2070000 /*PMU configure*/
#define PMU_SLEEP_CON0              (PMU_BASE + 0x0000)
#define PMU_SLEEP_CON1              (PMU_BASE + 0x0004)
#define PMU_RETENTION_CON0          (PMU_BASE + 0x0008)
#define PMU_TOP_TEST_CON0           (PMU_BASE + 0x000C)
#define PMU_TOP_STATUS_CON0         (PMU_BASE + 0x0010)
#define PMU_TOP_CON0                (PMU_BASE + 0x0014)
#define PMU_INT_CON0                (PMU_BASE + 0x0018)
#define PMU_INT_MASK_CON0           (PMU_BASE + 0x001C)
#define PMU_INT_CLR_CON0            (PMU_BASE + 0x0020)
#define PMU_INT_STATUS_CON0         (PMU_BASE + 0x0024)
#define PMU_INT_RAW_STATUS_CON0     (PMU_BASE + 0x0028)
#define PMU_INT_MISC_CON            (PMU_BASE + 0x002C)
#define PMU_INT_DEB_CON0            (PMU_BASE + 0x0030)
#define PMU_INT_DEB_CON1            (PMU_BASE + 0x0034)
#define PMU_INT_DEB_CON2            (PMU_BASE + 0x0038)
#define PMU_INT_DEB_CON3            (PMU_BASE + 0x003C)
#define PMU_INT_DEB_CON4            (PMU_BASE + 0x0040)
#define PMU_INT_DEB_CON5            (PMU_BASE + 0x0044)
#define PMU_LDO_SLDOH_CON0          (PMU_BASE + 0x0080)
#define PMU_LDO_SLDOH_CON1          (PMU_BASE + 0x0084)
#define PMU_LDO_CLDO_CON0           (PMU_BASE + 0x0088)
#define PMU_LDO_CLDO_CON1           (PMU_BASE + 0x008C)
#define PMU_LDO_CLDO_CON2           (PMU_BASE + 0x0090)
#define PMU_LDO_CLDO_CON3           (PMU_BASE + 0x0094)
#define PMU_LDO_CLDO_CON4           (PMU_BASE + 0x0098)
#define PMU_LDO_MLDO_CON0           (PMU_BASE + 0x00A0)
#define PMU_LDO_MLDO_CON1           (PMU_BASE + 0x00A4)
#define PMU_LDO_RETENTIONLDO_CON0   (PMU_BASE + 0x00A8)
#define PMU_LDO_RETENTIONLDO_CON1   (PMU_BASE + 0x00AC)
#define PMU_LDO_RETENTIONLDO_CON2   (PMU_BASE + 0x00B0)
#define PMU_LDO_RETENTIONLDO_CON3   (PMU_BASE + 0x00B4)
#define PMU_LDO_RETENTIONLDO_CON4   (PMU_BASE + 0x00B8)
#define PMU_LDO_RETENTIONLDO_CON5   (PMU_BASE + 0x00BC)
#define PMU_LDO_RETENTIONLDO_CON6   (PMU_BASE + 0x00C0)
#define PMU_LDO_RETENTIONLDO_CON7   (PMU_BASE + 0x00C4)
#define PMU_BUCK_CON0               (PMU_BASE + 0x00D0)
#define PMU_BUCK_CON1               (PMU_BASE + 0x00D4)
#define PMU_BUCK_CON2               (PMU_BASE + 0x00D8)
#define PMU_BUCK_CON3               (PMU_BASE + 0x00DC)
#define PMU_BUCK_STATUS_CON0        (PMU_BASE + 0x00E0)
#define PMU_DIG_VCORE_ANA_CON0  (PMU_BASE + 0x0100)
#define PMU_DIG_VCORE_ANA_CON1  (PMU_BASE + 0x0104)
#define PMU_DIG_VCORE_ANA_CON2  (PMU_BASE + 0x0108)
#define PMU_DIG_VCORE_ANA_CON3  (PMU_BASE + 0x010C)
#define PMU_DIG_VCORE_ANA_CON4  (PMU_BASE + 0x0110)
#define PMU_DIG_VCORE_ANA_CON5  (PMU_BASE + 0x0114)
#define PMU_DIG_VCORE_ANA_CON6  (PMU_BASE + 0x0118)
#define PMU_DIG_VCORE_ANA_CON7  (PMU_BASE + 0x011C)
#define PMU_DIG_VCORE_ANA_CON8  (PMU_BASE + 0x0120)
#define PMU_DIG_VCORE_ANA_CON9  (PMU_BASE + 0x0124)
#define PMU_DIG_VCORE_ANA_CON10 (PMU_BASE + 0x0128)
#define PMU_DIG_VCORE_ANA_CON11 (PMU_BASE + 0x012C)
#define PMU_DIG_VCORE_ANA_CON12 (PMU_BASE + 0x0130)
// mask is HEX;  shift is Integer
#define PMU_RG_SLEEP_MODE_MASK                    0x1
#define PMU_RG_SLEEP_MODE_SHIFT                   0
#define PMU_RG_SLEEP_STATUS_CLR_MASK              0x1
#define PMU_RG_SLEEP_STATUS_CLR_SHIFT             1
#define PMU_SLEEP_CTRL_BUCK_STATE_MASK            0x7
#define PMU_SLEEP_CTRL_BUCK_STATE_SHIFT           4
#define PMU_SLEEP_CTRL_SLDOH_STATE_MASK           0x7
#define PMU_SLEEP_CTRL_SLDOH_STATE_SHIFT          7
#define PMU_SLEEP_CTRL_FLOW_STATE_MASK            0x3
#define PMU_SLEEP_CTRL_FLOW_STATE_SHIFT           10
#define PMU_SLEEP_STATUS_GATED_MASK               0x1
#define PMU_SLEEP_STATUS_GATED_SHIFT              12
#define PMU_SLEEP_STATUS_MASK                     0x1
#define PMU_SLEEP_STATUS_SHIFT                    13
#define PMU_AD_PMU_SLEEP_MASK                     0x1
#define PMU_AD_PMU_SLEEP_SHIFT                    14
#define PMU_DA_PMU_SLEEP_MASK                     0x1
#define PMU_DA_PMU_SLEEP_SHIFT                    15
#define PMU_RG_SLEEP_POWER_SEL_MASK               0x1
#define PMU_RG_SLEEP_POWER_SEL_SHIFT              0
#define PMU_SLEEP_POWER_SEL_SYNC_MASK             0x1
#define PMU_SLEEP_POWER_SEL_SYNC_SHIFT            15
#define PMU_RG_RETENTION_MODE_MASK                0x1
#define PMU_RG_RETENTION_MODE_SHIFT               0
#define PMU_RG_TOP_MON_GRP_SEL_MASK               0x1F
#define PMU_RG_TOP_MON_GRP_SEL_SHIFT              4
#define PMU_RG_TOP_TEST_OUT_MASK                  0x3F
#define PMU_RG_TOP_TEST_OUT_SHIFT                 10
#define PMU_AD_PMU_V3_STATUS_MASK                 0x1
#define PMU_AD_PMU_V3_STATUS_SHIFT                0
#define PMU_AD_PMU_V2_STATUS_MASK                 0x1
#define PMU_AD_PMU_V2_STATUS_SHIFT                1
#define PMU_AD_PMU_V1_STATUS_MASK                 0x1
#define PMU_AD_PMU_V1_STATUS_SHIFT                2
#define PMU_AD_BGCLK_MASK                         0x1
#define PMU_AD_BGCLK_SHIFT                        3
#define PMU_AD_SLDOH_EN_STATUS_MASK               0x1
#define PMU_AD_SLDOH_EN_STATUS_SHIFT              4
#define PMU_AD_ELDO_EN_SATSU_MASK                 0x1
#define PMU_AD_ELDO_EN_SATSU_SHIFT                5
#define PMU_AD_BG_RDY_STATUS_MASK                 0x1
#define PMU_AD_BG_RDY_STATUS_SHIFT                6
#define PMU_AD_BUCK_EN_STATUS_MASK                0x1
#define PMU_AD_BUCK_EN_STATUS_SHIFT               7
#define PMU_AD_UVLO_STATUS_MASK                   0x1
#define PMU_AD_UVLO_STATUS_SHIFT                  8
#define PMU_AD_OT_STATUS_MASK                     0x1
#define PMU_AD_OT_STATUS_SHIFT                    9
#define PMU_AD_PMU_RSTB_STATUS_MASK               0x1
#define PMU_AD_PMU_RSTB_STATUS_SHIFT              10
#define PMU_AD_BUCK_PR_BYPASS_STATUS_MASK         0x1
#define PMU_AD_BUCK_PR_BYPASS_STATUS_SHIFT        11
#define PMU_AD_CLK2M_BUCK_MASK                    0x1
#define PMU_AD_CLK2M_BUCK_SHIFT                   12
#define PMU_AD_TM_SIG_BUCK_MASK                   0x1
#define PMU_AD_TM_SIG_BUCK_SHIFT                  13
#define PMU_AD_BUCK_V2P5NA_BP_STATUS_MASK         0x1
#define PMU_AD_BUCK_V2P5NA_BP_STATUS_SHIFT        14
#define PMU_RG_TOP_RSV_1_MASK                     0xFF
#define PMU_RG_TOP_RSV_1_SHIFT                    0
#define PMU_RG_TOP_RSV_0_MASK                     0xFF
#define PMU_RG_TOP_RSV_0_SHIFT                    8
#define PMU_RG_INT_EN_OT_MASK                     0x1
#define PMU_RG_INT_EN_OT_SHIFT                    0
#define PMU_RG_INT_EN_MLDO_PG_MASK                0x1
#define PMU_RG_INT_EN_MLDO_PG_SHIFT               1
#define PMU_RG_INT_EN_MLDO_OC_MASK                0x1
#define PMU_RG_INT_EN_MLDO_OC_SHIFT               2
#define PMU_RG_INT_EN_CLDO_PWR_GOOD_MASK          0x1
#define PMU_RG_INT_EN_CLDO_PWR_GOOD_SHIFT         3
#define PMU_RG_INT_EN_CLDO_OC_MASK                0x1
#define PMU_RG_INT_EN_CLDO_OC_SHIFT               4
#define PMU_RG_INT_EN_SLDOH_OC_MASK               0x1
#define PMU_RG_INT_EN_SLDOH_OC_SHIFT              5
#define PMU_RG_INT_MASK_OT_MASK                   0x1
#define PMU_RG_INT_MASK_OT_SHIFT                  0
#define PMU_RG_INT_MASK_MLDO_PG_MASK              0x1
#define PMU_RG_INT_MASK_MLDO_PG_SHIFT             1
#define PMU_RG_INT_MASK_MLDO_OC_MASK              0x1
#define PMU_RG_INT_MASK_MLDO_OC_SHIFT             2
#define PMU_RG_INT_MASK_CLDO_PWR_GOOD_MASK        0x1
#define PMU_RG_INT_MASK_CLDO_PWR_GOOD_SHIFT       3
#define PMU_RG_INT_MASK_CLDO_OC_MASK              0x1
#define PMU_RG_INT_MASK_CLDO_OC_SHIFT             4
#define PMU_RG_INT_MASK_SLDOH_OC_MASK             0x1
#define PMU_RG_INT_MASK_SLDOH_OC_SHIFT            5
#define PMU_RG_INT_STATUS_OT_CLR_MASK             0x1
#define PMU_RG_INT_STATUS_OT_CLR_SHIFT            0
#define PMU_RG_INT_STATUS_MLDO_PG_CLR_MASK        0x1
#define PMU_RG_INT_STATUS_MLDO_PG_CLR_SHIFT       1
#define PMU_RG_INT_STATUS_MLDO_OC_CLR_MASK        0x1
#define PMU_RG_INT_STATUS_MLDO_OC_CLR_SHIFT       2
#define PMU_RG_INT_STATUS_CLDO_PWR_GOOD_CLR_MASK  0x1
#define PMU_RG_INT_STATUS_CLDO_PWR_GOOD_CLR_SHIFT 3
#define PMU_RG_INT_STATUS_CLDO_OC_CLR_MASK        0x1
#define PMU_RG_INT_STATUS_CLDO_OC_CLR_SHIFT       4
#define PMU_RG_INT_STATUS_SLDOH_OC_CLR_MASK       0x1
#define PMU_RG_INT_STATUS_SLDOH_OC_CLR_SHIFT      5
#define PMU_RG_INT_STATUS_OT_MASK                 0x1
#define PMU_RG_INT_STATUS_OT_SHIFT                0
#define PMU_RG_INT_STATUS_MLDO_PG_MASK            0x1
#define PMU_RG_INT_STATUS_MLDO_PG_SHIFT           1
#define PMU_RG_INT_STATUS_MLDO_OC_MASK            0x1
#define PMU_RG_INT_STATUS_MLDO_OC_SHIFT           2
#define PMU_RG_INT_STATUS_CLDO_PWR_GOOD_MASK      0x1
#define PMU_RG_INT_STATUS_CLDO_PWR_GOOD_SHIFT     3
#define PMU_RG_INT_STATUS_CLDO_OC_MASK            0x1
#define PMU_RG_INT_STATUS_CLDO_OC_SHIFT           4
#define PMU_RG_INT_STATUS_SLDOH_OC_MASK           0x1
#define PMU_RG_INT_STATUS_SLDOH_OC_SHIFT          5
#define PMU_RG_INT_RAW_STATUS_OT_MASK             0x1
#define PMU_RG_INT_RAW_STATUS_OT_SHIFT            0
#define PMU_RG_INT_RAW_STATUS_MLDO_PG_MASK        0x1
#define PMU_RG_INT_RAW_STATUS_MLDO_PG_SHIFT       1
#define PMU_RG_INT_RAW_STATUS_MLDO_OC_MASK        0x1
#define PMU_RG_INT_RAW_STATUS_MLDO_OC_SHIFT       2
#define PMU_RG_INT_RAW_STATUS_CLDO_PWR_GOOD_MASK  0x1
#define PMU_RG_INT_RAW_STATUS_CLDO_PWR_GOOD_SHIFT 3
#define PMU_RG_INT_RAW_STATUS_CLDO_OC_MASK        0x1
#define PMU_RG_INT_RAW_STATUS_CLDO_OC_SHIFT       4
#define PMU_RG_INT_RAW_STATUS_SLDOH_OC_MASK       0x1
#define PMU_RG_INT_RAW_STATUS_SLDOH_OC_SHIFT      5
#define PMU_RG_PMU_DIG_INT_POLARITY_MASK          0x1
#define PMU_RG_PMU_DIG_INT_POLARITY_SHIFT         0
#define PMU_RG_SLDOH_OC_DEG_EN_MASK               0x1
#define PMU_RG_SLDOH_OC_DEG_EN_SHIFT              0
#define PMU_RG_SLDOH_OC_DEG_WND_MASK              0x1
#define PMU_RG_SLDOH_OC_DEG_WND_SHIFT             1
#define PMU_RG_SLDOH_OC_DEG_THD_MASK              0x3
#define PMU_RG_SLDOH_OC_DEG_THD_SHIFT             2
#define PMU_RG_DEG_IN_SLDOH_OC_MASK               0x1
#define PMU_RG_DEG_IN_SLDOH_OC_SHIFT              14
#define PMU_RG_EVENT_SLDOH_OC_MASK                0x1
#define PMU_RG_EVENT_SLDOH_OC_SHIFT               15
#define PMU_RG_CLDO_OC_DEG_EN_MASK                0x1
#define PMU_RG_CLDO_OC_DEG_EN_SHIFT               0
#define PMU_RG_CLDO_OC_DEG_WND_MASK               0x1
#define PMU_RG_CLDO_OC_DEG_WND_SHIFT              1
#define PMU_RG_CLDO_OC_DEG_THD_MASK               0x3
#define PMU_RG_CLDO_OC_DEG_THD_SHIFT              2
#define PMU_RG_DEG_IN_CLDO_OC_MASK                0x1
#define PMU_RG_DEG_IN_CLDO_OC_SHIFT               14
#define PMU_RG_EVENT_CLDO_OC_MASK                 0x1
#define PMU_RG_EVENT_CLDO_OC_SHIFT                15
#define PMU_RG_CLDO_PWR_GOOD_DEG_EN_MASK          0x1
#define PMU_RG_CLDO_PWR_GOOD_DEG_EN_SHIFT         0
#define PMU_RG_CLDO_PWR_GOOD_DEG_WND_MASK         0x1
#define PMU_RG_CLDO_PWR_GOOD_DEG_WND_SHIFT        1
#define PMU_RG_CLDO_PWR_GOOD_DEG_THD_MASK         0x3
#define PMU_RG_CLDO_PWR_GOOD_DEG_THD_SHIFT        2
#define PMU_RG_DEG_IN_CLDO_PWR_GOOD_MASK          0x1
#define PMU_RG_DEG_IN_CLDO_PWR_GOOD_SHIFT         14
#define PMU_RG_EVENT_CLDO_PWR_GOOD_MASK           0x1
#define PMU_RG_EVENT_CLDO_PWR_GOOD_SHIFT          15
#define PMU_RG_MLDO_OC_DEG_EN_MASK                0x1
#define PMU_RG_MLDO_OC_DEG_EN_SHIFT               0
#define PMU_RG_MLDO_OC_DEG_WND_MASK               0x1
#define PMU_RG_MLDO_OC_DEG_WND_SHIFT              1
#define PMU_RG_MLDO_OC_DEG_THD_MASK               0x3
#define PMU_RG_MLDO_OC_DEG_THD_SHIFT              2
#define PMU_RG_DEG_IN_MLDO_OC_MASK                0x1
#define PMU_RG_DEG_IN_MLDO_OC_SHIFT               14
#define PMU_RG_EVENT_MLDO_OC_MASK                 0x1
#define PMU_RG_EVENT_MLDO_OC_SHIFT                15
#define PMU_RG_MLDO_PG_DEG_EN_MASK                0x1
#define PMU_RG_MLDO_PG_DEG_EN_SHIFT               0
#define PMU_RG_MLDO_PG_DEG_WND_MASK               0x1
#define PMU_RG_MLDO_PG_DEG_WND_SHIFT              1
#define PMU_RG_MLDO_PG_DEG_THD_MASK               0x3
#define PMU_RG_MLDO_PG_DEG_THD_SHIFT              2
#define PMU_RG_DEG_IN_MLDO_PG_MASK                0x1
#define PMU_RG_DEG_IN_MLDO_PG_SHIFT               14
#define PMU_RG_EVENT_MLDO_PG_MASK                 0x1
#define PMU_RG_EVENT_MLDO_PG_SHIFT                15
#define PMU_RG_OT_DEG_EN_MASK                     0x1
#define PMU_RG_OT_DEG_EN_SHIFT                    0
#define PMU_RG_OT_DEG_WND_MASK                    0x1
#define PMU_RG_OT_DEG_WND_SHIFT                   1
#define PMU_RG_OT_DEG_THD_MASK                    0x3
#define PMU_RG_OT_DEG_THD_SHIFT                   2
#define PMU_RG_DEG_IN_OT_MASK                     0x1
#define PMU_RG_DEG_IN_OT_SHIFT                    14
#define PMU_RG_EVENT_OT_MASK                      0x1
#define PMU_RG_EVENT_OT_SHIFT                     15
#define PMU_RG_SLDOH_EN_MASK                      0x1
#define PMU_RG_SLDOH_EN_SHIFT                     0
#define PMU_SLDOH_SLEEP_TRIG_OFF_MASK             0x1
#define PMU_SLDOH_SLEEP_TRIG_OFF_SHIFT            12
#define PMU_SLDOH_SLEEP_TRIG_ON_MASK              0x1
#define PMU_SLDOH_SLEEP_TRIG_ON_SHIFT             13
#define PMU_RG_SLDOH_EN_FINAL_MASK                0x1
#define PMU_RG_SLDOH_EN_FINAL_SHIFT               14
#define PMU_DA_SLDOH_EN_MASK                      0x1
#define PMU_DA_SLDOH_EN_SHIFT                     15
#define PMU_AD_SLDOH_OC_EN_STATUS_MASK            0x1
#define PMU_AD_SLDOH_OC_EN_STATUS_SHIFT           0
#define PMU_AD_SLDOH_OC_STATUS_MASK               0x1
#define PMU_AD_SLDOH_OC_STATUS_SHIFT              1
#define PMU_RG_CLDO_EN_MASK                       0x1
#define PMU_RG_CLDO_EN_SHIFT                      0
#define PMU_DA_CLDO_EN_MASK                       0x1
#define PMU_DA_CLDO_EN_SHIFT                      15
#define PMU_AD_CLDO_OC_EN_STATUS_MASK             0x1
#define PMU_AD_CLDO_OC_EN_STATUS_SHIFT            0
#define PMU_AD_CLDO_OC_STATUS_MASK                0x1
#define PMU_AD_CLDO_OC_STATUS_SHIFT               1
#define PMU_AD_CLDO_PWR_GOOD_MASK                 0x1
#define PMU_AD_CLDO_PWR_GOOD_SHIFT                2
#define PMU_RG_CLDO_VOSEL_NORM_MASK               0x1F
#define PMU_RG_CLDO_VOSEL_NORM_SHIFT              0
#define PMU_RG_CLDO_VOSEL_SLEEP_MASK              0x1F
#define PMU_RG_CLDO_VOSEL_SLEEP_SHIFT             5
#define PMU_DA_CLDO_VOSEL_MASK                    0x1F
#define PMU_DA_CLDO_VOSEL_SHIFT                   11
#define PMU_RG_CLDO_VOTRIM_NORM_MASK              0xF
#define PMU_RG_CLDO_VOTRIM_NORM_SHIFT             0
#define PMU_RG_CLDO_VOTRIM_SLEEP_MASK             0xF
#define PMU_RG_CLDO_VOTRIM_SLEEP_SHIFT            4
#define PMU_DA_CLDO_VOTRIM_MASK                   0xF
#define PMU_DA_CLDO_VOTRIM_SHIFT                  12
#define PMU_RG_CLDO_NDISC_WIND_MASK               0x7
#define PMU_RG_CLDO_NDISC_WIND_SHIFT              0
#define PMU_DA_CLDO_DVS_DISC_MASK                 0x1
#define PMU_DA_CLDO_DVS_DISC_SHIFT                14
#define PMU_DA_CLDO_LP_MASK                       0x1
#define PMU_DA_CLDO_LP_SHIFT                      15
#define PMU_RG_MLDO_EN_MASK                       0x1
#define PMU_RG_MLDO_EN_SHIFT                      0
#define PMU_DA_MLDO_EN_MASK                       0x1
#define PMU_DA_MLDO_EN_SHIFT                      15
#define PMU_AD_MLDO_OCEN_STATUS_MASK              0x1
#define PMU_AD_MLDO_OCEN_STATUS_SHIFT             0
#define PMU_AD_MLDO_OC_STATUS_MASK                0x1
#define PMU_AD_MLDO_OC_STATUS_SHIFT               1
#define PMU_AD_MLDO_PG_STATUS_MASK                0x1
#define PMU_AD_MLDO_PG_STATUS_SHIFT               2
#define PMU_RG_RETENTION_LDO_EN_MASK              0x1
#define PMU_RG_RETENTION_LDO_EN_SHIFT             0
#define PMU_DA_RETENTION_LDO_EN_MASK              0x1
#define PMU_DA_RETENTION_LDO_EN_SHIFT             15
#define PMU_RG_SRAM_PSW_MODE_MASK                 0x1
#define PMU_RG_SRAM_PSW_MODE_SHIFT                1
#define PMU_RG_SRAM_PSW_SW_MASK                   0x1
#define PMU_RG_SRAM_PSW_SW_SHIFT                  2
#define PMU_DA_SRAM_PSW_EN_MASK                   0x1
#define PMU_DA_SRAM_PSW_EN_SHIFT                  15
#define PMU_RG_TD_IBH_SEL_MASK                    0x3
#define PMU_RG_TD_IBH_SEL_SHIFT                   0
#define PMU_DA_RETENTION_LDO_IBH_EN_MASK          0x1
#define PMU_DA_RETENTION_LDO_IBH_EN_SHIFT         15
#define PMU_RG_RETENTION_LDO_RSV_MASK             0x3
#define PMU_RG_RETENTION_LDO_RSV_SHIFT            0
#define PMU_RG_RETENTION_LDO_VOSEL_MASK           0x7
#define PMU_RG_RETENTION_LDO_VOSEL_SHIFT          2
#define PMU_RG_NVTREF_VOTRIM_MASK                 0xF
#define PMU_RG_NVTREF_VOTRIM_SHIFT                5
#define PMU_RG_NVTREF_TCTRIM_MASK                 0x7
#define PMU_RG_NVTREF_TCTRIM_SHIFT                9
#define PMU_RG_NVTREF_IBSEL_MASK                  0x7
#define PMU_RG_NVTREF_IBSEL_SHIFT                 12
#define PMU_RG_RETENTION_LDO_NDISC_EN_MASK        0x1
#define PMU_RG_RETENTION_LDO_NDISC_EN_SHIFT       0
#define PMU_RG_RETENTION_LDO_IQDB_MASK            0x1
#define PMU_RG_RETENTION_LDO_IQDB_SHIFT           1
#define PMU_DA_RETENTION_LDO_RSV_MASK             0x3
#define PMU_DA_RETENTION_LDO_RSV_SHIFT            0
#define PMU_DA_RETENTION_LDO_VOSEL_MASK           0x7
#define PMU_DA_RETENTION_LDO_VOSEL_SHIFT          2
#define PMU_DA_NVTREF_VOTRIM_MASK                 0xF
#define PMU_DA_NVTREF_VOTRIM_SHIFT                5
#define PMU_DA_NVTREF_TCTRIM_MASK                 0x7
#define PMU_DA_NVTREF_TCTRIM_SHIFT                9
#define PMU_DA_NVTREF_IBSEL_MASK                  0x7
#define PMU_DA_NVTREF_IBSEL_SHIFT                 12
#define PMU_DA_RETENTION_LDO_NDISC_EN_MASK        0x1
#define PMU_DA_RETENTION_LDO_NDISC_EN_SHIFT       0
#define PMU_DA_RETENTION_LDO_IQDB_MASK            0x1
#define PMU_DA_RETENTION_LDO_IQDB_SHIFT           1
#define PMU_RG_AO_REG_INITIAL_MASK                0x1
#define PMU_RG_AO_REG_INITIAL_SHIFT               0
#define PMU_RG_SRAM_PSW_INITIAL_MASK              0x1
#define PMU_RG_SRAM_PSW_INITIAL_SHIFT             1
#define PMU_RG_RETENTION_LDO_EN_INITIAL_MASK      0x1
#define PMU_RG_RETENTION_LDO_EN_INITIAL_SHIFT     2
#define PMU_RG_BUCK_EN_MASK                       0x1
#define PMU_RG_BUCK_EN_SHIFT                      0
#define PMU_BUCK_PS_EN_MASK                       0x1
#define PMU_BUCK_PS_EN_SHIFT                      12
#define PMU_BUCK_V2P5NALDO_STB_MASK               0x1
#define PMU_BUCK_V2P5NALDO_STB_SHIFT              13
#define PMU_BUCK_V2P5NA_EN_MASK                   0x1
#define PMU_BUCK_V2P5NA_EN_SHIFT                  14
#define PMU_BUCK_EN_MASK                          0x1
#define PMU_BUCK_EN_SHIFT                         15
#define PMU_RG_BUCK_FPFM_CTRL_MASK                0x1
#define PMU_RG_BUCK_FPFM_CTRL_SHIFT               0
#define PMU_RG_BUCK_PFMVL_SLEEP_MASK              0x3
#define PMU_RG_BUCK_PFMVL_SLEEP_SHIFT             1
#define PMU_RG_BUCK_PFMVL_NORM_MASK               0x3
#define PMU_RG_BUCK_PFMVL_NORM_SHIFT              3
#define PMU_RG_BUCK_PFMVH_SLEEP_MASK              0x3
#define PMU_RG_BUCK_PFMVH_SLEEP_SHIFT             5
#define PMU_RG_BUCK_PFMVH_NORM_MASK               0x3
#define PMU_RG_BUCK_PFMVH_NORM_SHIFT              7
#define PMU_DA_BUCK_PFMVL_MASK                    0x3
#define PMU_DA_BUCK_PFMVL_SHIFT                   12
#define PMU_DA_BUCK_PFMVH_MASK                    0x3
#define PMU_DA_BUCK_PFMVH_SHIFT                   14
#define PMU_RG_BUCK_FPWM_MASK                     0x1
#define PMU_RG_BUCK_FPWM_SHIFT                    0
#define PMU_DA_BUCK_FPWM_PRE_MASK                 0x1
#define PMU_DA_BUCK_FPWM_PRE_SHIFT                13
#define PMU_AD_FORCE_BUCK_PWM_MASK                0x1
#define PMU_AD_FORCE_BUCK_PWM_SHIFT               14
#define PMU_DA_BUCK_FPWM_MASK                     0x1
#define PMU_DA_BUCK_FPWM_SHIFT                    15
#define PMU_RG_BUCK_NDIS_EN_MASK                  0x1
#define PMU_RG_BUCK_NDIS_EN_SHIFT                 0
#define PMU_DA_BUCK_NDIS_EN_MASK                  0x1
#define PMU_DA_BUCK_NDIS_EN_SHIFT                 15
#define PMU_BUCK_SLEEP_TRIG_MASK                  0x1
#define PMU_BUCK_SLEEP_TRIG_SHIFT                 14
#define PMU_RG_BUCK_EN_FINAL_MASK                 0x1
#define PMU_RG_BUCK_EN_FINAL_SHIFT                15
#define PMU_RG_BGR_VBGTRIM_MASK                   0xF
#define PMU_RG_BGR_VBGTRIM_SHIFT                  0
#define PMU_RG_BGR_TCTRIM_MASK                    0xF
#define PMU_RG_BGR_TCTRIM_SHIFT                   4
#define PMU_RG_BGOP_IBSEL_MASK                    0x1
#define PMU_RG_BGOP_IBSEL_SHIFT                   8
#define PMU_RG_IBIAS_TRIM_MASK                    0xF
#define PMU_RG_IBIAS_TRIM_SHIFT                   9
#define PMU_RG_THRCMP_TEST_EN_MASK                0x1
#define PMU_RG_THRCMP_TEST_EN_SHIFT               13
#define PMU_RG_STUP_V2I_NOSLEEP_MASK              0x1
#define PMU_RG_STUP_V2I_NOSLEEP_SHIFT             14
#define PMU_RG_BUCK_RZSEL_MASK                    0x7
#define PMU_RG_BUCK_RZSEL_SHIFT                   0
#define PMU_RG_BUCK_CZADO_MASK                    0x1
#define PMU_RG_BUCK_CZADO_SHIFT                   3
#define PMU_RG_BUCK_VSEL_MASK                     0x3F
#define PMU_RG_BUCK_VSEL_SHIFT                    4
#define PMU_RG_BUCK_MASEL_MASK                    0x3
#define PMU_RG_BUCK_MASEL_SHIFT                   10
#define PMU_RG_BUCK_ZX_OF_MASK                    0x3
#define PMU_RG_BUCK_ZX_OF_SHIFT                   12
#define PMU_RG_BUCK_CSL_MASK                      0x3
#define PMU_RG_BUCK_CSL_SHIFT                     14
#define PMU_RG_BUCK_CKTRIM_MASK                   0x7
#define PMU_RG_BUCK_CKTRIM_SHIFT                  0
#define PMU_RG_BUCK_CSR_MASK                      0x3
#define PMU_RG_BUCK_CSR_SHIFT                     3
#define PMU_RG_BUCK_PR_BYPASS_EN_MASK             0x1
#define PMU_RG_BUCK_PR_BYPASS_EN_SHIFT            5
#define PMU_RG_BUCK_RSV_MASK                      0xFF
#define PMU_RG_BUCK_RSV_SHIFT                     6
#define PMU_RG_BUCK_ICS_OFFSET_MASK               0x3
#define PMU_RG_BUCK_ICS_OFFSET_SHIFT              14
#define PMU_RG_BUCK_CSM_MASK                      0x3
#define PMU_RG_BUCK_CSM_SHIFT                     0
#define PMU_RG_BUCK_SLEW_MASK                     0x3
#define PMU_RG_BUCK_SLEW_SHIFT                    2
#define PMU_RG_BUCK_ZXPDN_MASK                    0x1
#define PMU_RG_BUCK_ZXPDN_SHIFT                   4
#define PMU_RG_BUCK_DCTEST_EN_MASK                0x1
#define PMU_RG_BUCK_DCTEST_EN_SHIFT               5
#define PMU_RG_PMU_TOP_RSV0_MASK                  0x1FF
#define PMU_RG_PMU_TOP_RSV0_SHIFT                 6
#define PMU_RG_SLDOH_VOSEL_MASK                   0x3F
#define PMU_RG_SLDOH_VOSEL_SHIFT                  0
#define PMU_RG_SLDOH_HP_EN_MASK                   0x1
#define PMU_RG_SLDOH_HP_EN_SHIFT                  6
#define PMU_RG_SLDOH_BIA_EN_MASK                  0x1
#define PMU_RG_SLDOH_BIA_EN_SHIFT                 7
#define PMU_RG_SLDOH_RSV1_MASK                    0x3
#define PMU_RG_SLDOH_RSV1_SHIFT                   8
#define PMU_RG_SLDOH_OC_AO_MASK                   0x1
#define PMU_RG_SLDOH_OC_AO_SHIFT                  10
#define PMU_RG_SLDOH_OC_SEL_MASK                  0x3
#define PMU_RG_SLDOH_OC_SEL_SHIFT                 11
#define PMU_RG_SLDOH_RSV2_MASK                    0x3
#define PMU_RG_SLDOH_RSV2_SHIFT                   13
#define PMU_RG_CLDO_NDISC_EN_MASK                 0x1
#define PMU_RG_CLDO_NDISC_EN_SHIFT                0
#define PMU_RG_CLDO_PG_EN_MASK                    0x1
#define PMU_RG_CLDO_PG_EN_SHIFT                   1
#define PMU_RG_CLDO_OC_AO_MASK                    0x1
#define PMU_RG_CLDO_OC_AO_SHIFT                   2
#define PMU_RG_CLDO_OC_SEL_MASK                   0x3
#define PMU_RG_CLDO_OC_SEL_SHIFT                  3
#define PMU_RG_CLDO_RSV1_MASK                     0x3
#define PMU_RG_CLDO_RSV1_SHIFT                    5
#define PMU_RG_CLDO_RSV2_MASK                     0x3
#define PMU_RG_CLDO_RSV2_SHIFT                    7
#define PMU_RG_PMU_TOP_RSV1_MASK                  0x7F
#define PMU_RG_PMU_TOP_RSV1_SHIFT                 9
#define PMU_RG_ELDO_EN_MASK                       0x1
#define PMU_RG_ELDO_EN_SHIFT                      0
#define PMU_RG_ELDO_LOAD_TRAN_IMPROVE_MASK        0x3
#define PMU_RG_ELDO_LOAD_TRAN_IMPROVE_SHIFT       1
#define PMU_RG_ELDO_NDIS_EN_MASK                  0x1
#define PMU_RG_ELDO_NDIS_EN_SHIFT                 3
#define PMU_RG_ELDO_TBST_EN_MASK                  0x1
#define PMU_RG_ELDO_TBST_EN_SHIFT                 4
#define PMU_RG_ELDO_VSEL_MASK                     0x7
#define PMU_RG_ELDO_VSEL_SHIFT                    5
#define PMU_RG_ELDO_DCTEST_EN_MASK                0x1
#define PMU_RG_ELDO_DCTEST_EN_SHIFT               8
#define PMU_RG_PMU_LOCAL_DCTEST_SEL_MASK          0xF
#define PMU_RG_PMU_LOCAL_DCTEST_SEL_SHIFT         9
#define PMU_RG_ELDO_PSWN_RON_INCREASE_MASK        0x3
#define PMU_RG_ELDO_PSWN_RON_INCREASE_SHIFT       13
#define PMU_RG_ELDO_RSV_MASK                      0x3
#define PMU_RG_ELDO_RSV_SHIFT                     0
#define PMU_RG_MLDO_NDIS_EN_MASK                  0x1
#define PMU_RG_MLDO_NDIS_EN_SHIFT                 2
#define PMU_RG_MLDO_LEAKCOMP_EN_MASK              0x1
#define PMU_RG_MLDO_LEAKCOMP_EN_SHIFT             3
#define PMU_RG_MLDO_LEAKCOMP_BP_MASK              0x1
#define PMU_RG_MLDO_LEAKCOMP_BP_SHIFT             4
#define PMU_RG_MLDO_HP_EN_MASK                    0x1
#define PMU_RG_MLDO_HP_EN_SHIFT                   5
#define PMU_RG_MLDO_OC_SEL_MASK                   0x7
#define PMU_RG_MLDO_OC_SEL_SHIFT                  6
#define PMU_RG_MLDO_OC_EN_MASK                    0x1
#define PMU_RG_MLDO_OC_EN_SHIFT                   9
#define PMU_RG_MLDO_PG_EN_MASK                    0x1
#define PMU_RG_MLDO_PG_EN_SHIFT                   10
#define PMU_RG_MLDO_VOCAL_MASK                    0xF
#define PMU_RG_MLDO_VOCAL_SHIFT                   11
#define PMU_RG_MLDO_RSV1_MASK                     0x3
#define PMU_RG_MLDO_RSV1_SHIFT                    0
#define PMU_RG_MLDO_RSV2_MASK                     0x3
#define PMU_RG_MLDO_RSV2_SHIFT                    2
#define PMU_RG_TM_EN_MASK                         0x1
#define PMU_RG_TM_EN_SHIFT                        4
#define PMU_RG_TM_VOSEL_MASK                      0x7
#define PMU_RG_TM_VOSEL_SHIFT                     5
#define PMU_RG_RETENTION_LDO_IBH_TM_EN_MASK       0x1
#define PMU_RG_RETENTION_LDO_IBH_TM_EN_SHIFT      8
#define PMU_RG_PMU_TOP_DCTEST_EN_MASK             0x1
#define PMU_RG_PMU_TOP_DCTEST_EN_SHIFT            9
#define PMU_RG_PMU_TOP_DCTEST_SEL_MASK            0x7
#define PMU_RG_PMU_TOP_DCTEST_SEL_SHIFT           10
#define PMU_RG_PMU_TOP_DCTEST2_EN_MASK            0x1
#define PMU_RG_PMU_TOP_DCTEST2_EN_SHIFT           13
#define PMU_RG_STUP_IBHIGH_MASK                   0x1
#define PMU_RG_STUP_IBHIGH_SHIFT                  14
#define PMU_RG_VBG_TM_EN_MASK                     0x1
#define PMU_RG_VBG_TM_EN_SHIFT                    15
#define PMU_RG_BGPRE_BYPASS_ENB_MASK              0x1
#define PMU_RG_BGPRE_BYPASS_ENB_SHIFT             0
#define PMU_RG_STUP_TM_EN_MASK                    0x1
#define PMU_RG_STUP_TM_EN_SHIFT                   1
#define PMU_RG_PMU_GPO_MASK                       0x1
#define PMU_RG_PMU_GPO_SHIFT                      2
#define PMU_RG_PMU_GPO_I_MASK                     0x1
#define PMU_RG_PMU_GPO_I_SHIFT                    3
#define PMU_RG_PMU_GPO_E4_MASK                    0x1
#define PMU_RG_PMU_GPO_E4_SHIFT                   4
#define PMU_RG_PMU_GPO_E8_MASK                    0x1
#define PMU_RG_PMU_GPO_E8_SHIFT                   5
#define PMU_RG_PMU_GPO_SR_MASK                    0x1
#define PMU_RG_PMU_GPO_SR_SHIFT                   6
#define PMU_RG_TM_RTLDO_CLK_OFF_MASK              0x1
#define PMU_RG_TM_RTLDO_CLK_OFF_SHIFT             7
#define PMU_RG_PMU_TOP_RSV2_MASK                  0xFF
#define PMU_RG_PMU_TOP_RSV2_SHIFT                 8
#define PMU_RG_STRUP_MON_GRP_SEL_MASK             0xF
#define PMU_RG_STRUP_MON_GRP_SEL_SHIFT            0
#define PMU_RG_STRUP_RSV_MASK                     0xFF
#define PMU_RG_STRUP_RSV_SHIFT                    4
#define PMU_RG_COLD_RST_EN_MASK                   0x1
#define PMU_RG_COLD_RST_EN_SHIFT                  12
#define PMU_RGS_AO_RETENTION_LDO_EN_MASK          0x1
#define PMU_RGS_AO_RETENTION_LDO_EN_SHIFT         0
#define PMU_RGS_AO_SRAM_PWR_SW_MASK               0x1
#define PMU_RGS_AO_SRAM_PWR_SW_SHIFT              1
#define PMU_RGS_NVTREF_IBSEL_MASK                 0x7
#define PMU_RGS_NVTREF_IBSEL_SHIFT                2
#define PMU_RGS_NVTREF_TCTRIM_MASK                0x7
#define PMU_RGS_NVTREF_TCTRIM_SHIFT               5
#define PMU_RGS_NVTREF_VOTRIM_MASK                0xF
#define PMU_RGS_NVTREF_VOTRIM_SHIFT               8
#define PMU_RGS_RETENTION_LDO_VOSEL_MASK          0x7
#define PMU_RGS_RETENTION_LDO_VOSEL_SHIFT         12
#define PMU_RGS_RETENTION_LDO_IQDB_MASK           0x1
#define PMU_RGS_RETENTION_LDO_IQDB_SHIFT          15
#define PMU_RGS_RETENTION_LDO_NDISC_EN_MASK       0x1
#define PMU_RGS_RETENTION_LDO_NDISC_EN_SHIFT      0
#define PMU_RGS_RETENTION_LDO_RSV_MASK            0x3
#define PMU_RGS_RETENTION_LDO_RSV_SHIFT           1

#endif /* HAL_PMU_MODULE_ENABLED */
#endif /* __HAL_PMU_H__ */
