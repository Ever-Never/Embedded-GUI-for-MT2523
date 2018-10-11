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

#include "hal_isink.h"

#ifdef HAL_ISINK_MODULE_ENABLED

#include "hal_isink_internal.h"

static  hal_isink_clock_source_t clock_select[HAL_ISINK_MAX_CHANNEL] = {HAL_ISINK_CLOCK_SOURCE_32KHZ, HAL_ISINK_CLOCK_SOURCE_32KHZ};

static void pmu_ctrl_ivgen_mt6385(unsigned char enable,hal_isink_channel_t channel);

hal_isink_status_t  hal_isink_init(hal_isink_channel_t channel)
{
    bool status;
    /*init default mode setting ,default is register mode*/
    switch (channel) {
        case HAL_ISINK_CHANNEL_0:
            status = pmu_set_register_value(PMU_ISINK_MODE_CTRL, PMU_ISINK_CH0_MODE_MASK, PMU_ISINK_CH0_MODE_SHIFT, HAL_ISINK_MODE_REGISTER);
            pmu_ctrl_ivgen_mt6385(1,HAL_ISINK_CHANNEL_0);
            break;
        case HAL_ISINK_CHANNEL_1:
            status = pmu_set_register_value(PMU_ISINK_MODE_CTRL, PMU_ISINK_CH1_MODE_MASK, PMU_ISINK_CH1_MODE_MASK, HAL_ISINK_MODE_REGISTER);
            pmu_ctrl_ivgen_mt6385(1,HAL_ISINK_CHANNEL_1);
            break;
        default :
            return HAL_ISINK_STATUS_ERROR_CHANNEL;
    }

    if (true == status) {
        return HAL_ISINK_STATUS_OK;
    } else {
        return HAL_ISINK_STATUS_ERROR;
    }
}

hal_isink_status_t  hal_isink_deinit(hal_isink_channel_t channel)
{
    bool status;
    /*init default mode setting ,default is register mode*/
    switch (channel) {
        case HAL_ISINK_CHANNEL_0:
            status = pmu_set_register_value(PMU_ISINK_MODE_CTRL, PMU_ISINK_CH0_MODE_MASK, PMU_ISINK_CH0_MODE_SHIFT, HAL_ISINK_MODE_PWM);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_Low, PMU_ISINK_CH0_EN_MASK, PMU_ISINK_CH0_EN_SHIFT, 0);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_High, PMU_ISINK_CH0_BIAS_EN_MASK, PMU_ISINK_CH0_BIAS_EN_SHIFT, 0);
            status = pmu_set_register_value(PMU_ISINK_ANA0_H, PMU_RG_ISINK0_DOUBLE_MASK, PMU_RG_ISINK0_DOUBLE_SHIFT, 0);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_High, PMU_ISINK_CH0_BIAS_EN_MASK, PMU_ISINK_CH0_BIAS_EN_SHIFT, 0);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_Low, PMU_ISINK_CH0_EN_MASK, PMU_ISINK_CH0_EN_SHIFT, 0);
            
            pmu_ctrl_ivgen_mt6385(0,HAL_ISINK_CHANNEL_0);
            break;
        case HAL_ISINK_CHANNEL_1:
            status = pmu_set_register_value(PMU_ISINK_MODE_CTRL, PMU_ISINK_CH1_MODE_MASK, PMU_ISINK_CH1_MODE_MASK, HAL_ISINK_MODE_PWM);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_Low, PMU_ISINK_CH1_EN_MASK, PMU_ISINK_CH1_EN_SHIFT, 0);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_High, PMU_ISINK_CH1_BIAS_EN_MASK, PMU_ISINK_CH1_BIAS_EN_SHIFT, 0);
            status = pmu_set_register_value(PMU_ISINK_ANA0_H, PMU_RG_ISINK1_DOUBLE_MASK, PMU_RG_ISINK1_DOUBLE_SHIFT, 0);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_High, PMU_ISINK_CH1_BIAS_EN_MASK, PMU_ISINK_CH1_BIAS_EN_SHIFT, 0);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_Low, PMU_ISINK_CH1_EN_MASK, PMU_ISINK_CH1_EN_SHIFT, 0);
            
            pmu_ctrl_ivgen_mt6385(0,HAL_ISINK_CHANNEL_1);
            break;
        default :
            return HAL_ISINK_STATUS_ERROR_CHANNEL;
    }

    if (true == status) {
        return HAL_ISINK_STATUS_OK;
    } else {
        return HAL_ISINK_STATUS_ERROR;
    }
}

hal_isink_status_t hal_isink_set_clock_source(hal_isink_channel_t channel, hal_isink_clock_source_t source_clock)
{

    bool status;
    if (source_clock > HAL_ISINK_CLOCK_SOURCE_2MHZ) {
        return HAL_ISINK_STATUS_ERROR_INVALID_PARAMETER;
    }

    switch (channel) {
        case HAL_ISINK_CHANNEL_0:
            status = pmu_set_register_value(PMU_CKCFG5, PMU_RG_DRV_ISINK0_CK_CKSEL_MASK, PMU_RG_DRV_ISINK0_CK_CKSEL_SHIFT, source_clock);
            break;
        case HAL_ISINK_CHANNEL_1:
            status = pmu_set_register_value(PMU_CKCFG5, PMU_RG_DRV_ISINK1_CK_CKSEL_MASK, PMU_RG_DRV_ISINK1_CK_CKSEL_MASK, source_clock);
            break;
        default :
            return HAL_ISINK_STATUS_ERROR_CHANNEL;
    }

    clock_select[channel] = source_clock;

    if (true == status) {
        return HAL_ISINK_STATUS_OK;
    } else {
        return HAL_ISINK_STATUS_ERROR;
    }

}

hal_isink_status_t hal_isink_set_mode(hal_isink_channel_t channel, hal_isink_mode_t mode)

{
    bool status;
    if (mode > HAL_ISINK_MODE_REGISTER) {
        return HAL_ISINK_STATUS_ERROR_INVALID_PARAMETER;
    }
    switch (channel) {
        case HAL_ISINK_CHANNEL_0:
            status = pmu_set_register_value(PMU_ISINK_MODE_CTRL, PMU_ISINK_CH0_MODE_MASK, PMU_ISINK_CH0_MODE_SHIFT, (uint16_t)mode);
            break;
        case HAL_ISINK_CHANNEL_1:
            status = pmu_set_register_value(PMU_ISINK_MODE_CTRL, PMU_ISINK_CH1_MODE_MASK, PMU_ISINK_CH1_MODE_SHIFT, (uint16_t)mode);
            break;
        default:
            return HAL_ISINK_STATUS_ERROR_CHANNEL;
    }
    if (HAL_ISINK_MODE_PWM == mode) {

        if (HAL_ISINK_CLOCK_SOURCE_32KHZ == clock_select[channel]) {

            switch (channel) {
                case HAL_ISINK_CHANNEL_0:
                    /* duty is (N+1)/32*/
                    status = pmu_set_register_value(PMU_ISINK0_CON2, PMU_ISINK_DIM0_DUTY_MASK, PMU_ISINK_DIM0_DUTY_SHIFT, 15);
                    /*output is 200 Hz*/
                    status = pmu_set_register_value(PMU_ISINK0_CON0, PMU_ISINK_DIM0_FSEL_L_MASK, PMU_ISINK_DIM0_FSEL_L_SHIFT, 4);
                    status = pmu_set_register_value(PMU_ISINK0_CON1, PMU_ISINK_DIM0_FSEL_H_MASK, PMU_ISINK_DIM0_FSEL_H_SHIFT, 0);
                    break;
                case HAL_ISINK_CHANNEL_1:
                    /* duty is (N+1)/32*/
                    status = pmu_set_register_value(PMU_ISINK1_CON2, PMU_ISINK_DIM1_DUTY_MASK, PMU_ISINK_DIM1_DUTY_SHIFT, 15);
                    /*output is 200 Hz*/
                    status = pmu_set_register_value(PMU_ISINK1_CON0, PMU_ISINK_DIM1_FSEL_L_MASK, PMU_ISINK_DIM1_FSEL_L_SHIFT, 4);
                    status = pmu_set_register_value(PMU_ISINK1_CON1, PMU_ISINK_DIM1_FSEL_H_MASK, PMU_ISINK_DIM1_FSEL_H_SHIFT, 0);
                    break;
                default:
                    return HAL_ISINK_STATUS_ERROR_CHANNEL;
            }
        } else if (HAL_ISINK_CLOCK_SOURCE_2MHZ == clock_select[channel]) {
            switch (channel) {
                case HAL_ISINK_CHANNEL_0:
                    /* duty is (N+1)/32*/
                    status = pmu_set_register_value(PMU_ISINK0_CON1, PMU_ISINK_DIM0_DUTY_MASK, PMU_ISINK_DIM0_DUTY_SHIFT, 15);
                    /*output is 200 Hz*/
                    status = pmu_set_register_value(PMU_ISINK0_CON0, PMU_ISINK_DIM0_FSEL_L_MASK, PMU_ISINK_DIM0_FSEL_L_SHIFT, 55);
                    status = pmu_set_register_value(PMU_ISINK0_CON1, PMU_ISINK_DIM0_FSEL_H_MASK, PMU_ISINK_DIM0_FSEL_H_SHIFT, 1);
                    break;
                case HAL_ISINK_CHANNEL_1:
                    /* duty is (N+1)/32*/
                    status = pmu_set_register_value(PMU_ISINK1_CON2, PMU_ISINK_DIM1_DUTY_MASK, PMU_ISINK_DIM1_DUTY_SHIFT, 15);
                    /*output is 200 Hz*/
                    status = pmu_set_register_value(PMU_ISINK1_CON0, PMU_ISINK_DIM1_FSEL_L_MASK, PMU_ISINK_DIM1_FSEL_L_SHIFT, 55);
                    status = pmu_set_register_value(PMU_ISINK1_CON1, PMU_ISINK_DIM1_FSEL_H_MASK, PMU_ISINK_DIM1_FSEL_H_SHIFT, 1);
                    break;

                default:
                    return HAL_ISINK_STATUS_ERROR_CHANNEL;
            }

        } else {
            return HAL_ISINK_STATUS_ERROR;
        }
    }

    if (true == status) {
        return HAL_ISINK_STATUS_OK;
    } else {
        return HAL_ISINK_STATUS_ERROR;
    }

}


hal_isink_status_t hal_isink_set_step_current(hal_isink_channel_t channel, hal_isink_current_t current)
{
    bool status;
    switch (channel) {
        case HAL_ISINK_CHANNEL_0:
            status = pmu_set_register_value(PMU_ISINK0_CON2, PMU_ISINK_CH0_STEP_MASK, PMU_ISINK_CH0_STEP_SHIFT, (uint16_t)current);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_Low, PMU_RG_ISINK0_CHOP_EN_MASK, PMU_RG_ISINK0_CHOP_EN_SHIFT, 1);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_High, PMU_ISINK_CH0_BIAS_EN_MASK, PMU_ISINK_CH0_BIAS_EN_SHIFT, 1);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_Low, PMU_ISINK_CH0_EN_MASK, PMU_ISINK_CH0_EN_SHIFT, 1);
            break;
        case HAL_ISINK_CHANNEL_1:
            status = pmu_set_register_value(PMU_ISINK1_CON2, PMU_ISINK_CH1_STEP_MASK, PMU_ISINK_CH1_STEP_SHIFT, (uint16_t)current);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_Low, PMU_RG_ISINK1_CHOP_EN_MASK, PMU_RG_ISINK1_CHOP_EN_MASK, 1);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_High, PMU_ISINK_CH1_BIAS_EN_MASK, PMU_ISINK_CH1_BIAS_EN_SHIFT, 1);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_Low, PMU_ISINK_CH1_EN_MASK, PMU_ISINK_CH1_EN_SHIFT, 1);
            break;
        default:
            return HAL_ISINK_STATUS_ERROR_CHANNEL;
    }

    if (true == status) {
        return HAL_ISINK_STATUS_OK;
    } else {
        return HAL_ISINK_STATUS_ERROR;
    }
}

hal_isink_status_t hal_isink_set_double_current(hal_isink_channel_t channel, hal_isink_current_t current)
{

    bool status;
    switch (channel) {
        case HAL_ISINK_CHANNEL_0:
            status = pmu_set_register_value(PMU_ISINK0_CON2, PMU_ISINK_CH0_STEP_MASK, PMU_ISINK_CH0_STEP_SHIFT, (uint16_t)current);
            status = pmu_set_register_value(PMU_ISINK_ANA0_H, PMU_RG_ISINK0_DOUBLE_MASK, PMU_RG_ISINK0_DOUBLE_SHIFT, 1);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_High, PMU_ISINK_CH0_BIAS_EN_MASK, PMU_ISINK_CH0_BIAS_EN_SHIFT, 1);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_Low, PMU_ISINK_CH0_EN_MASK, PMU_ISINK_CH0_EN_SHIFT, 1);
            break;
        case HAL_ISINK_CHANNEL_1:
            status = pmu_set_register_value(PMU_ISINK1_CON2, PMU_ISINK_CH1_STEP_MASK, PMU_ISINK_CH1_STEP_SHIFT, (uint16_t)current);
            status = pmu_set_register_value(PMU_ISINK_ANA0_H, PMU_RG_ISINK1_DOUBLE_MASK, PMU_RG_ISINK1_DOUBLE_SHIFT, 1);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_High, PMU_ISINK_CH1_BIAS_EN_MASK, PMU_ISINK_CH1_BIAS_EN_SHIFT, 1);
            status = pmu_set_register_value(PMU_ISINK_EN_CTRL_Low, PMU_ISINK_CH1_EN_MASK, PMU_ISINK_CH1_EN_SHIFT, 1);
            break;
        default:
            return HAL_ISINK_STATUS_ERROR_CHANNEL;
    }

    if (true == status) {
        return HAL_ISINK_STATUS_OK;
    } else {
        return HAL_ISINK_STATUS_ERROR;
    }

}

hal_isink_status_t hal_isink_enable_breath_mode(hal_isink_channel_t channel, hal_isink_breath_mode_t breath_mode)
{

    bool status;

    switch (channel) {
        case HAL_ISINK_CHANNEL_0:
            pmu_set_register_value(PMU_ISINK0_CON4, PMU_ISINK_BREATH0_TR1_SEL_MASK, PMU_ISINK_BREATH0_TR1_SEL_SHIFT, breath_mode.darker_to_lighter_time1);
            pmu_set_register_value(PMU_ISINK0_CON4, PMU_ISINK_BREATH0_TR2_SEL_MASK, PMU_ISINK_BREATH0_TR2_SEL_SHIFT, breath_mode.darker_to_lighter_time2);
            pmu_set_register_value(PMU_ISINK0_CON5, PMU_ISINK_BREATH0_TF1_SEL_MASK, PMU_ISINK_BREATH0_TF1_SEL_SHIFT, breath_mode.lighter_to_darker_time1);
            pmu_set_register_value(PMU_ISINK0_CON5, PMU_ISINK_BREATH0_TF2_SEL_MASK, PMU_ISINK_BREATH0_TF2_SEL_SHIFT, breath_mode.darker_to_lighter_time2);
            pmu_set_register_value(PMU_ISINK0_CON6, PMU_ISINK_BREATH0_TON_SEL_MASK, PMU_ISINK_BREATH0_TON_SEL_SHIFT, breath_mode.lightest_time);
            status = pmu_set_register_value(PMU_ISINK0_CON6, PMU_ISINK_BREATH0_TOFF_SEL_MASK, PMU_ISINK_BREATH0_TOFF_SEL_SHIFT, breath_mode.darkest_time);
            break;
        case HAL_ISINK_CHANNEL_1:
            pmu_set_register_value(PMU_ISINK1_CON4, PMU_ISINK_BREATH1_TR1_SEL_MASK, PMU_ISINK_BREATH1_TR1_SEL_SHIFT, breath_mode.darker_to_lighter_time1);
            pmu_set_register_value(PMU_ISINK1_CON4, PMU_ISINK_BREATH1_TR2_SEL_MASK, PMU_ISINK_BREATH1_TR2_SEL_SHIFT, breath_mode.darker_to_lighter_time2);
            pmu_set_register_value(PMU_ISINK1_CON5, PMU_ISINK_BREATH1_TF1_SEL_MASK, PMU_ISINK_BREATH1_TF1_SEL_SHIFT, breath_mode.lighter_to_darker_time1);
            pmu_set_register_value(PMU_ISINK1_CON5, PMU_ISINK_BREATH1_TF2_SEL_MASK, PMU_ISINK_BREATH1_TF2_SEL_SHIFT, breath_mode.darker_to_lighter_time2);
            pmu_set_register_value(PMU_ISINK1_CON6, PMU_ISINK_BREATH1_TON_SEL_MASK, PMU_ISINK_BREATH1_TON_SEL_SHIFT, breath_mode.lightest_time);
            status = pmu_set_register_value(PMU_ISINK1_CON6, PMU_ISINK_BREATH1_TOFF_SEL_MASK, PMU_ISINK_BREATH1_TOFF_SEL_SHIFT, breath_mode.darkest_time);
            break;
        default :
            return HAL_ISINK_STATUS_ERROR_CHANNEL;
    }

    if (true == status) {
        return HAL_ISINK_STATUS_OK;
    } else {
        return HAL_ISINK_STATUS_ERROR;
    }



}


hal_isink_status_t hal_isink_get_running_status(hal_isink_channel_t channel, hal_isink_running_status_t *running_status)
{
    uint32_t value;
    switch (channel) {
        case HAL_ISINK_CHANNEL_0:
            value = pmu_get_register_value(PMU_ISINK_ANA0_l, PMU_ISINK_STATUS0_MASK, PMU_ISINK_STATUS0_SHIFT);
            break;
        case HAL_ISINK_CHANNEL_1:
            value = pmu_get_register_value(PMU_ISINK_ANA0_l, PMU_ISINK_STATUS1_MASK, PMU_ISINK_STATUS1_SHIFT);
            break;
        default :
            return HAL_ISINK_STATUS_ERROR_CHANNEL;
    }

    if (value == 1) {
        *running_status = HAL_ISINK_BUSY;
    } else {
        *running_status = HAL_ISINK_IDLE;
    }

    return HAL_ISINK_STATUS_OK;

}


static void pmu_ctrl_ivgen_mt6385(unsigned char enable,hal_isink_channel_t channel)
{
    pmu_set_vrc_key_mt6385(PMIC_UNLOCK_KEY);
    pmu_set_ovr_mode_mt6385(PMIC_ENTER_OVR_MODE);
    
    //Check ISINK Status,if user use ISINK need turn on IVGEN.
    if (enable != 0) {
        //RG_S0_IVGEN_OFF_EN = 0,S0,S1 Enable IVGEN
        pmu_set_register_value(0x42  , 0x01    , 0x02   , 0);
        pmu_set_register_value(0x42  , 0x01    , 0x03   , 0);
        //force on isink0,1 drv
        if(channel == HAL_ISINK_CHANNEL_0) {
            pmu_set_register_value(PMU_RG_DRV_ISINK0_FORCE_ADDR  , PMU_RG_DRV_ISINK0_FORCE_MASK    , PMU_RG_DRV_ISINK0_FORCE_SHIFT   , 3);
        }
        else{
            pmu_set_register_value(PMU_RG_DRV_ISINK1_FORCE_ADDR  , PMU_RG_DRV_ISINK1_FORCE_MASK    , PMU_RG_DRV_ISINK1_FORCE_SHIFT   , 3);
        }       
    } else {
        //RG_S0_IVGEN_OFF_EN = 1,S0,S1 Disable IVGEN
        pmu_set_register_value(0x42  , 0x01    , 0x02   , 1);
        pmu_set_register_value(0x42  , 0x01    , 0x03   , 1);
        //force off isink0,1 drv
        if(channel == HAL_ISINK_CHANNEL_0) {
            pmu_set_register_value(PMU_RG_DRV_ISINK0_FORCE_ADDR  , PMU_RG_DRV_ISINK0_FORCE_MASK    , PMU_RG_DRV_ISINK0_FORCE_SHIFT   , 0);
        }
        else{
            pmu_set_register_value(PMU_RG_DRV_ISINK1_FORCE_ADDR  , PMU_RG_DRV_ISINK1_FORCE_MASK    , PMU_RG_DRV_ISINK1_FORCE_SHIFT   , 0);
        }
    }    
    
    pmu_set_ovr_mode_mt6385(PMIC_LEAVE_OVR_MODE);
    pmu_set_vrc_key_mt6385(PMIC_LOCK_KEY);    
}
#endif /*HAL_ISINK_MODULE_ENABLED*/




