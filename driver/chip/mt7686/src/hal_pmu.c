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
#include "hal_gpt.h"
#ifdef HAL_PMU_MODULE_ENABLED
//#define  HAL_PMU_DEBUG_ENABLE
#ifdef  HAL_PMU_DEBUG_ENABLE
#define debug_log(_message,...) printf(_message, ##__VA_ARGS__)
#else
#define debug_log(_message,...)
#endif

void pmu_set_cldo_voltage_mt7686(int index){
    pmu_set_register_value(PMU_LDO_CLDO_CON0,PMU_RG_CLDO_EN_MASK,PMU_RG_CLDO_EN_SHIFT,1);
    switch(index){
    case 0: //0.77V
        pmu_set_register_value(PMU_LDO_CLDO_CON2,PMU_RG_CLDO_VOSEL_NORM_MASK,PMU_RG_CLDO_VOSEL_NORM_SHIFT,0);
        break;
    case 1: //0.85V - default
        pmu_set_register_value(PMU_LDO_CLDO_CON2,PMU_RG_CLDO_VOSEL_NORM_MASK,PMU_RG_CLDO_VOSEL_NORM_SHIFT,0x4);
            break;
    case 2: //1.15V
        pmu_set_register_value(PMU_LDO_CLDO_CON2,PMU_RG_CLDO_VOSEL_NORM_MASK,PMU_RG_CLDO_VOSEL_NORM_SHIFT,0x13);
            break;
    case 3: //1.31V
        pmu_set_register_value(PMU_LDO_CLDO_CON2,PMU_RG_CLDO_VOSEL_NORM_MASK,PMU_RG_CLDO_VOSEL_NORM_SHIFT,0x1F);
            break;
    }
}

void pmu_set_retention_ldo_mt7686(int adjust_value){
    if((adjust_value>=0)&& (adjust_value<=7)){
        pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON3,PMU_RG_RETENTION_LDO_VOSEL_MASK,PMU_RG_RETENTION_LDO_VOSEL_SHIFT,adjust_value);
        hal_gpt_delay_ms(1);
    }
}

void pmu_set_mldo_voltage_mt7686(int adjust_value) {
    pmu_set_register_value(PMU_LDO_MLDO_CON0, PMU_RG_MLDO_EN_MASK, PMU_RG_MLDO_EN_SHIFT, 1);
    if ((adjust_value >= 0)&& (adjust_value <= 0xF)) {
        pmu_set_register_value(PMU_DIG_VCORE_ANA_CON7, PMU_RG_MLDO_VOCAL_MASK, PMU_RG_MLDO_VOCAL_SHIFT, adjust_value);
    }
}

void pmu_sram_power_switch_control_mt7686(pmu_control_t status) {
    if (status==PMU_CONTROL_OFF) {
        debug_log("[PSW HW control]\r\n");
        pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON1, PMU_RG_SRAM_PSW_MODE_MASK, PMU_RG_SRAM_PSW_MODE_SHIFT, 0);
    } else {
        debug_log("[PSW SW off]\r\n");
        pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON1, PMU_RG_SRAM_PSW_MODE_MASK, PMU_RG_SRAM_PSW_MODE_SHIFT, 1);
        pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON1, PMU_RG_SRAM_PSW_SW_MASK, PMU_RG_SRAM_PSW_SW_SHIFT, 0);
    }
}
/*
 * BUCK voltage : default value 1.45V 6'b001100
 * */
uint32_t pmu_get_vcore_voltage_mt7686(void)
{
    uint32_t cur_voltage;
    cur_voltage = pmu_get_register_value(PMU_DIG_VCORE_ANA_CON1,PMU_RG_BUCK_VSEL_MASK, PMU_RG_BUCK_VSEL_SHIFT);
    return cur_voltage;
}

void pmu_set_vcore_voltage_mt7686(hal_pmu_stage mode,pmu_vcore_vosel_t vol)
{
    if (vol == PMU_VCORE_1p3V) {
        pmu_set_register_value(PMU_DIG_VCORE_ANA_CON1,PMU_RG_BUCK_VSEL_MASK, PMU_RG_BUCK_VSEL_SHIFT, 0);//1.3V
    } else if (vol == PMU_VCORE_1p45v) {
        pmu_set_register_value(PMU_DIG_VCORE_ANA_CON1,PMU_RG_BUCK_VSEL_MASK, PMU_RG_BUCK_VSEL_SHIFT, 0xc); //1.45V
    } else if (vol == PMU_VCORE_1p5v) {
        pmu_set_register_value(PMU_DIG_VCORE_ANA_CON1,PMU_RG_BUCK_VSEL_MASK, PMU_RG_BUCK_VSEL_SHIFT, 0x10); //1.5V
    } else if (vol == PMU_VCORE_1p7v) {
        pmu_set_register_value(PMU_DIG_VCORE_ANA_CON1,PMU_RG_BUCK_VSEL_MASK, PMU_RG_BUCK_VSEL_SHIFT, 0x20); //1.7V
    } else if (vol == PMU_VCORE_2p08v) {
        pmu_set_register_value(PMU_DIG_VCORE_ANA_CON1,PMU_RG_BUCK_VSEL_MASK, PMU_RG_BUCK_VSEL_SHIFT, 0x3F); //2.08V
    } else {
        debug_log("[Error used]\r\n");
    }
}

/*
 * SLDOH enable status : 1:enable; 0:disable
 * */
bool pmu_get_buck_control_status() {
    uint32_t status = pmu_get_register_value(PMU_BUCK_CON3,PMU_DA_BUCK_NDIS_EN_MASK,PMU_DA_BUCK_NDIS_EN_SHIFT);
    if(status==PMU_STATUS_DISABLE) {
        return PMU_STATUS_DISABLE ;
    } else {
        return PMU_STATUS_ENABLE;
    }
}

/* mode=0 :PFM mode=1 :PWM
*buck force pfm or pwm control by sw */
void pmu_buck_force_mode(pmu_force_mode_t mode) {
    if(mode==PMU_FORCE_PFM) {
        debug_log("[BUCK in PFM]\r\n");
    } else {
        debug_log("[BUCK in PWM]\r\n");
    }
    pmu_set_register_value(PMU_BUCK_CON2,PMU_RG_BUCK_FPWM_MASK,PMU_RG_BUCK_FPWM_SHIFT,mode);
}

void pmu_retention_ldo_control_mt7686(pmu_control_t status){
    if(status == PMU_CONTROL_OFF){
        debug_log("[Retention LDO off : RTC mode]");
    }else{
        debug_log("[Retention LDO on : Retention ldo]");
    }
    pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON0,PMU_RG_RETENTION_LDO_EN_MASK, PMU_RG_RETENTION_LDO_EN_SHIFT,status);
}

void pmu_enter_sleep_mode_mt7686(void) {
    pmu_set_register_value(PMU_SLEEP_CON0,PMU_RG_SLEEP_MODE_MASK,PMU_RG_SLEEP_MODE_SHIFT,1);
    hal_gpt_delay_us(500);
    debug_log("[PMU SLEEP stage]\r\n");
}

void pmu_enter_retention_mode_mt7686(pmu_retention_status_t status) {
    if(status==PMU_RETENTION_LDO_OFF){
        debug_log("[PMU RTC stage ]\r\n");
        pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON0,PMU_RG_RETENTION_LDO_EN_MASK,PMU_RG_RETENTION_LDO_EN_SHIFT,0);
        hal_gpt_delay_us(500);
        pmu_set_register_value(PMU_RETENTION_CON0,PMU_RG_RETENTION_MODE_MASK,PMU_RG_RETENTION_MODE_SHIFT,1);
    }else{
        debug_log("[PMU RETENTION stage]\r\n");
        pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON0,PMU_RG_RETENTION_LDO_EN_MASK,PMU_RG_RETENTION_LDO_EN_SHIFT,1);
        hal_gpt_delay_us(500);
        pmu_set_register_value(PMU_RETENTION_CON0,PMU_RG_RETENTION_MODE_MASK,PMU_RG_RETENTION_MODE_SHIFT,1);
    }
}

void pmu_resume_to_normal_mt7686(hal_pmu_stage_flow mode) {
    if(mode==SLEEP_NORMAL) {
        /*sleep to normal*/
        pmu_set_register_value(PMU_SLEEP_CON0,PMU_RG_SLEEP_MODE_MASK,PMU_RG_SLEEP_MODE_SHIFT,0);
        hal_gpt_delay_us(1000);
    } else if(mode==RETENTION_NORMAL) {
        /*Retention to normal*/
        pmu_init_mt7686();
        pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON7,PMU_RG_SRAM_PSW_INITIAL_MASK,PMU_RG_SRAM_PSW_INITIAL_SHIFT,1);
        hal_gpt_delay_us(200);
        pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON7,PMU_RG_RETENTION_LDO_EN_INITIAL_MASK,PMU_RG_RETENTION_LDO_EN_INITIAL_SHIFT,1);
        hal_gpt_delay_us(200);
        pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON7,PMU_RG_AO_REG_INITIAL_MASK,PMU_RG_AO_REG_INITIAL_SHIFT,1);
        hal_gpt_delay_us(200);
        pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON7,PMU_RG_AO_REG_INITIAL_MASK,PMU_RG_AO_REG_INITIAL_SHIFT,0);
        pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON7,PMU_RG_RETENTION_LDO_EN_INITIAL_MASK,PMU_RG_RETENTION_LDO_EN_INITIAL_SHIFT,0);
        pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON7,PMU_RG_SRAM_PSW_INITIAL_MASK,PMU_RG_SRAM_PSW_INITIAL_SHIFT,0);
    } else {
        debug_log("[PMU error stage]\r\n");
    }
}

void pmu_efuse_calibration () {
    uint32_t efuse_check;
    uint32_t bit_value;
    uint32_t efuse_vaild;
    efuse_check=pmu_count_bit_1(*EFUSE_VALID);
    efuse_vaild=*((volatile uint32_t *)EFUSE_VALID);
    if (efuse_check) {
        debug_log("[PMU EFUSE Start]\r\n"); /*3:0*/
            if (efuse_vaild & (1 << 0)) {
                debug_log("RG_BGR_VBGTRIM =1\r\n");
                bit_value = pmu_get_register_value(0xA20A0300, PMU_RG_BGR_VBGTRIM_MASK, PMU_RG_BGR_VBGTRIM_SHIFT);
                pmu_set_register_value(PMU_DIG_VCORE_ANA_CON0, PMU_RG_BGR_VBGTRIM_MASK, PMU_RG_BGR_VBGTRIM_SHIFT, bit_value);
            }
            if (efuse_vaild & (1 << 1)) { /*7:4*/
                debug_log("RG_BGR_TCTRIM =1\r\n");
                bit_value = pmu_get_register_value(0xA20A0300, PMU_RG_BGR_TCTRIM_MASK, PMU_RG_BGR_TCTRIM_SHIFT);
                pmu_set_register_value(PMU_DIG_VCORE_ANA_CON0, PMU_RG_BGR_TCTRIM_MASK, PMU_RG_BGR_TCTRIM_SHIFT, bit_value);
            }
            if (efuse_vaild & (1 << 2)) { /*11:8*/
                debug_log("RG_IBIAS_TRIM =1\r\n");
                bit_value = pmu_get_register_value(0xA20A0300, PMU_RG_IBIAS_TRIM_MASK, 8);
                pmu_set_register_value(PMU_DIG_VCORE_ANA_CON0, PMU_RG_IBIAS_TRIM_MASK, 8, bit_value);
            }
            if (efuse_vaild & (1 << 3)) { /*17:12*/
                debug_log("RG_BUCK_VSEL =1\r\n");
                bit_value = pmu_get_register_value(0xA20A0300, PMU_RG_BUCK_VSEL_MASK, 12);
                pmu_set_register_value(PMU_DIG_VCORE_ANA_CON1, PMU_RG_BUCK_VSEL_MASK, PMU_RG_BUCK_VSEL_SHIFT, bit_value);
            }
            if (efuse_vaild & (1 << 4)) { /*23:18*/
                debug_log("RG_SLDOH_VOSEL =1\r\n");
                bit_value = pmu_get_register_value(0xA20A0300, PMU_RG_SLDOH_VOSEL_MASK, 18);
                pmu_set_register_value(PMU_DIG_VCORE_ANA_CON4, PMU_RG_SLDOH_VOSEL_MASK, PMU_RG_SLDOH_VOSEL_SHIFT, bit_value);
            }
            if (efuse_vaild & (1 << 5)) { /*27:24*/
                debug_log("RG_MLDO_VOCAL =1\r\n");
                bit_value = pmu_get_register_value(0xA20A0300, PMU_RG_MLDO_VOCAL_MASK, 24);
                pmu_set_register_value(PMU_DIG_VCORE_ANA_CON7, PMU_RG_MLDO_VOCAL_MASK, PMU_RG_MLDO_VOCAL_SHIFT, bit_value);
            }
            if (efuse_vaild & (1 << 6)) { /*3:0*/
                debug_log("CLDO_VOTRIM_NORM =1\r\n");
                bit_value = pmu_get_register_value(0xA20A0304, PMU_RG_CLDO_VOTRIM_NORM_MASK, PMU_RG_CLDO_VOTRIM_NORM_SHIFT);
                pmu_set_register_value(PMU_LDO_CLDO_CON3, PMU_RG_CLDO_VOTRIM_NORM_MASK, PMU_RG_CLDO_VOTRIM_NORM_SHIFT, bit_value);
            }
            if (efuse_vaild & (1 << 7)) { /*7:4*/
                debug_log("RG_CLDO_VOTRIM_SLEEP =1\r\n");
                bit_value = pmu_get_register_value(0xA20A0304, PMU_RG_CLDO_VOTRIM_SLEEP_MASK, 4);
                pmu_set_register_value(PMU_LDO_CLDO_CON3, PMU_RG_CLDO_VOTRIM_SLEEP_MASK, PMU_RG_CLDO_VOTRIM_SLEEP_SHIFT, bit_value);
            }
            if (efuse_vaild & (1 << 8)) { /*11:8*/
                debug_log("RG_NVTREF_VOTRIM =1\r\n");
                bit_value = pmu_get_register_value(0xA20A0304, PMU_RG_NVTREF_VOTRIM_MASK, 8);
                pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON3, PMU_RG_NVTREF_VOTRIM_MASK, PMU_RG_NVTREF_VOTRIM_SHIFT, bit_value);
            }
            if (efuse_vaild & (1 << 9)) { /*14:12*/
                debug_log("RG_RETENTION_LDO_VOSEL =1\r\n");
                bit_value = pmu_get_register_value(0xA20A0304, PMU_RG_RETENTION_LDO_VOSEL_MASK, 12);
                pmu_set_register_value(PMU_LDO_RETENTIONLDO_CON3, PMU_RG_RETENTION_LDO_VOSEL_MASK, PMU_RG_RETENTION_LDO_VOSEL_SHIFT, bit_value);
            }
            debug_log("[0xA20A0300:%lx]\r\n",*((volatile uint32_t *)(0xA20A0300)));
            debug_log("[0xA20A0304:%lx]\r\n",*((volatile uint32_t *)(0xA20A0304)));
        }
}

void pmu_trigger_wd_cold_reset() {
    pmu_set_register_value(PMU_TOP_TEST_CON0,PMU_RG_TOP_MON_GRP_SEL_MASK,PMU_RG_TOP_MON_GRP_SEL_SHIFT,0);
    pmu_set_register_value(PMU_DIG_VCORE_ANA_CON10,PMU_RG_STRUP_MON_GRP_SEL_MASK,PMU_RG_STRUP_MON_GRP_SEL_SHIFT,0x7);
    hal_gpt_delay_ms(100);
    pmu_set_register_value(PMU_DIG_VCORE_ANA_CON10,PMU_RG_COLD_RST_EN_MASK,PMU_RG_COLD_RST_EN_SHIFT,1);
    *((volatile uint32_t *)(0xA2090010)) =0x156789AB;
}

int pmu_count_bit_1(uint32_t n)
{
    int c = 0;
    for (int i=0; i<32; ++i)
        if (n & (1 << i))
            c++;
    return c;
}

void pmu_set_register_value(uint32_t address, short int mask, short int shift, short int value) {
    uint32_t mask_buffer,target_value;
    mask_buffer = (~(mask << shift));    
    target_value = *((volatile uint32_t *)(address));
    target_value &= mask_buffer;
    target_value |= (value << shift);
    *((volatile uint32_t *)(address)) = target_value;
}

uint32_t pmu_get_register_value(uint32_t address, short int mask, short int shift) {
    uint32_t change_value, mask_buffer;
    mask_buffer = (mask << shift);
    change_value = *((volatile uint32_t *)(address));
    change_value &=mask_buffer;
    change_value = (change_value>> shift);
    return change_value;
}

void pmu_init_power_mode_mt7686() {
    /*Mist setting */
}

void pmu_init_mt7686(void)
{
    debug_log("pmu init\r\n");
    pmu_efuse_calibration();
    pmu_set_register_value(PMU_SLEEP_CON1,PMU_RG_SLEEP_POWER_SEL_MASK,PMU_RG_SLEEP_POWER_SEL_SHIFT,1);/*SLDOH*/
    pmu_set_register_value(PMU_BUCK_CON1,PMU_RG_BUCK_FPFM_CTRL_MASK,PMU_RG_BUCK_FPFM_CTRL_SHIFT,1);/*SLDOH*/
    pmu_init_power_mode_mt7686();
}

#endif /* HAL_PMU_MODULE_ENABLED */
