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
#include "hal_pmu_wrap_interface.h"
#ifdef HAL_PMU_MODULE_ENABLED

static hal_pmu_wrap_struct hal_pmu_struct_t = {
        pmu_init_mt7686,
        pmu_init_power_mode_mt7686,
        pmu_set_vcore_voltage_mt7686,
        pmu_enter_sleep_mode_mt7686,
        pmu_enter_retention_mode_mt7686,
        pmu_resume_to_normal_mt7686,
        pmu_set_cldo_voltage_mt7686,
        pmu_set_mldo_voltage_mt7686,
        pmu_retention_ldo_control_mt7686,
        pmu_sram_power_switch_control_mt7686,
        pmu_set_retention_ldo_mt7686
};
void hal_pmu_init(){
    hal_pmu_struct_t.init();
}

void hal_pmu_init_power_mode(){
    hal_pmu_struct_t.init_power_mode();
}

void hal_pmu_set_vcore_voltage(hal_pmu_stage mode,pmu_vcore_vosel_t vol){
    hal_pmu_struct_t.set_vcore_voltage(mode,vol);
}

void hal_pmu_enter_sleep_mode(){
    hal_pmu_struct_t.enter_sleep_mode();
}

void hal_pmu_enter_retention_mode(pmu_retention_status_t status){
    hal_pmu_struct_t.enter_retention_mode(status);
}

void hal_pmu_resume_to_normal(hal_pmu_stage_flow mode){
    hal_pmu_struct_t.resume_to_normal(mode);
}

void hal_pmu_set_cldo_voltage(int index){
    hal_pmu_struct_t.set_cldo_voltage(index);
}
void hal_pmu_set_mldo_voltage(int adjust_value){
    hal_pmu_struct_t.set_mldo_voltage(adjust_value);
}

void hal_pmu_retention_ldo_control(pmu_control_t status){
    hal_pmu_struct_t.control_retention_lod(status);
}

void hal_pmu_sram_power_switch_control(pmu_control_t status){
    hal_pmu_struct_t.sram_power_switch_control(status);
}

void hal_pmu_set_retention_ldo_voltage(int adjust_value){
    hal_pmu_struct_t.set_retention_ldo(adjust_value);
}
#endif /* HAL_PMU_MODULE_ENABLED */
