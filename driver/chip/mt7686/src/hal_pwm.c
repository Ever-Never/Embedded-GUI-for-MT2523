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

#include "hal_pwm.h"

#ifdef HAL_PWM_MODULE_ENABLED

#include "hal_pwm_internal.h"
#include "hal_clock.h"

/*peripheral dma base address array */
PWM_REGISTER_T  *pwm[PWM_NUMBER] = {PWM0, PWM1, PWM2, PWM3, PWM4, PWM5};

static bool pwm_run_status[PWM_NUMBER] = {false, false, false, false, false, false};

const hal_clock_cg_id pwm_pdn[PWM_NUMBER] = {HAL_CLOCK_CG_PWM0, HAL_CLOCK_CG_PWM1, HAL_CLOCK_CG_PWM2, HAL_CLOCK_CG_PWM3, HAL_CLOCK_CG_PWM4, HAL_CLOCK_CG_PWM5};

volatile static uint8_t pwm_init_status[PWM_NUMBER] = {0};

hal_pwm_status_t hal_pwm_init(hal_pwm_channel_t pwm_channel, hal_pwm_source_clock_t source_clock)


{
    hal_pwm_status_t busy_status;
    /*check parameters*/

    if (pwm_channel >= HAL_PWM_MAX_CHANNEL) {
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }

    if (source_clock > HAL_PWM_CLOCK_32KHZ) {
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }

    PWM_CHECK_AND_SET_BUSY(pwm_channel, busy_status);
    if (HAL_PWM_STATUS_ERROR == busy_status) {
        return HAL_PWM_STATUS_ERROR;
    }

    /* initialize driver default setting */
    pwm[pwm_channel]->PWM_CTRL = 0;
    pwm[pwm_channel]->PWM_COUNT = 0;
    pwm[pwm_channel]->PWM_THRESH = 0;

    /*set clock setting */
    pwm[pwm_channel]->PWM_CTRL &= ~PWM_CLK_SEL_MASK;
    pwm[pwm_channel]->PWM_CTRL |= source_clock << PWM_CLK_SEL_OFFSET;

    return HAL_PWM_STATUS_OK;
}

hal_pwm_status_t hal_pwm_deinit(hal_pwm_channel_t pwm_channel)

{

    /*check parameters*/

    if (pwm_channel >= HAL_PWM_MAX_CHANNEL) {
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }

    PWM_SET_IDLE(pwm_channel);

    return HAL_PWM_STATUS_OK;
}

hal_pwm_status_t hal_pwm_set_frequency(hal_pwm_channel_t pwm_channel, uint32_t frequency, uint32_t *total_count)
{

    uint32_t clock = 0;
    uint16_t clock_div = 0;
    volatile uint16_t  control = 0;
    uint16_t tmp = 0;
    uint32_t smallest_frequency = 0;

    if (pwm_channel >= HAL_PWM_MAX_CHANNEL) {
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }

    /*read current control value*/
    control = pwm[pwm_channel]->PWM_CTRL;
    clock_div = (1 << (control & PWM_CLK_DIV_MASK));
	control &= PWM_CLK_SEL_MASK;
    if (control == PWM_CLK_SEL_32K_MASK) {
        clock = PWM_CLOCK_SEL1;
    } else if (control == PWM_CLK_SEL_13M_MASK){
        clock = PWM_CLOCK_SEL2;
    }else{
        clock = PWM_CLOCK_SEL3;
	 }

    clock = clock / clock_div;
    smallest_frequency = clock / 0X1FFF;
    if (0 == frequency) {
        tmp = clock;
    } else if (frequency < smallest_frequency) {
        return HAL_PWM_STATUS_INVALID_FREQUENCY;
    } else {
        tmp = clock / frequency;
    }


    tmp--;
    if (tmp > PWM_MAX_COUNT) {
        tmp = PWM_MAX_COUNT;
    }
    pwm[pwm_channel]->PWM_COUNT = tmp;

    *total_count = tmp ;
    return HAL_PWM_STATUS_OK;

}

hal_pwm_status_t hal_pwm_set_duty_cycle(hal_pwm_channel_t pwm_channel, uint32_t duty_cycle)
{

    volatile uint16_t tmp = 0;
    uint16_t threshold = 0;

    if (pwm_channel >= HAL_PWM_MAX_CHANNEL) {
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }

    tmp = pwm[pwm_channel]->PWM_COUNT;

    if (duty_cycle > tmp)

    {
        threshold =  tmp;
    } else {
        threshold = duty_cycle;
    }

    pwm[pwm_channel]->PWM_THRESH = threshold;

    return HAL_PWM_STATUS_OK;

}


hal_pwm_status_t hal_pwm_start(hal_pwm_channel_t pwm_channel)
{
    uint32_t saved_mask;

    hal_clock_status_t clock_status;
    if (pwm_channel >= HAL_PWM_MAX_CHANNEL) {
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }

    saved_mask = save_and_set_interrupt_mask();

    if (pwm_run_status[pwm_channel] == false) {
        pwm_run_status[pwm_channel] = true;
        clock_status = hal_clock_enable(pwm_pdn[pwm_channel]);
        if (HAL_CLOCK_STATUS_ERROR == clock_status) {
            return HAL_PWM_STATUS_ERROR;
        }
    } else {
        restore_interrupt_mask(saved_mask);
        return  HAL_PWM_STATUS_ERROR;

    }
    restore_interrupt_mask(saved_mask);


    return  HAL_PWM_STATUS_OK;
}

hal_pwm_status_t hal_pwm_stop(hal_pwm_channel_t pwm_channel)
{

    uint32_t saved_mask;
    hal_clock_status_t clock_status;

    if (pwm_channel >= HAL_PWM_MAX_CHANNEL) {
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }

    saved_mask = save_and_set_interrupt_mask();

    if (pwm_run_status[pwm_channel] == true) {
        pwm_run_status[pwm_channel] = false;

        /*disable pwm clock */
        clock_status = hal_clock_disable(pwm_pdn[pwm_channel]);
        if (HAL_CLOCK_STATUS_ERROR == clock_status) {
            return HAL_PWM_STATUS_ERROR;
        }
    } else {
        restore_interrupt_mask(saved_mask);
        return  HAL_PWM_STATUS_ERROR;

    }
    restore_interrupt_mask(saved_mask);

    return  HAL_PWM_STATUS_OK;
}

hal_pwm_status_t hal_pwm_get_frequency(hal_pwm_channel_t pwm_channel, uint32_t *frequency)
{
    uint32_t clock = 0;
    uint16_t clock_div = 0;
    volatile uint16_t  control = 0;
    volatile uint16_t tmp = 0;

    if (pwm_channel >= HAL_PWM_MAX_CHANNEL) {
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }

    /*read current control value*/
    control = pwm[pwm_channel]->PWM_CTRL;
    clock_div = (1 << (control & PWM_CLK_DIV_MASK));
	control &= PWM_CLK_SEL_MASK;
    if (control == PWM_CLK_SEL_32K_MASK) {
        clock = PWM_CLOCK_SEL1;
    } else if (control == PWM_CLK_SEL_13M_MASK){
        clock = PWM_CLOCK_SEL2;
    }else{
        clock = PWM_CLOCK_SEL3;
	 }

    tmp = pwm[pwm_channel]->PWM_COUNT;
    tmp = (tmp + 1) * clock_div;

    *frequency = clock / tmp;

    return HAL_PWM_STATUS_OK;


}

hal_pwm_status_t hal_pwm_get_duty_cycle(hal_pwm_channel_t pwm_channel, uint32_t *duty_cycle)
{

    volatile uint16_t tmp = 0;

    if (pwm_channel >= HAL_PWM_MAX_CHANNEL) {
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }

    tmp = pwm[pwm_channel]->PWM_THRESH;


    *duty_cycle = tmp ;

    return HAL_PWM_STATUS_OK;

}

hal_pwm_status_t hal_pwm_get_running_status(hal_pwm_channel_t pwm_channel, hal_pwm_running_status_t *running_status)
{

    if (pwm_channel >= HAL_PWM_MAX_CHANNEL) {
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }

    if (true == pwm_run_status[pwm_channel]) {
        /*if pwm 's clock is enable ,so which is running*/
        *running_status = HAL_PWM_BUSY;
    } else {
        /*if pwm 's clock is disabled ,so which is stopped*/
        *running_status = HAL_PWM_IDLE;
    }

    return HAL_PWM_STATUS_OK;
}

hal_pwm_status_t hal_pwm_set_advanced_config(hal_pwm_channel_t pwm_channel, hal_pwm_advanced_config_t advanced_config)

{

    if (pwm_channel >= HAL_PWM_MAX_CHANNEL) {
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }

    if (advanced_config < HAL_PWM_CLOCK_DIVISION_2 || advanced_config > HAL_PWM_CLOCK_DIVISION_8) {
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }

    pwm[pwm_channel]->PWM_CTRL |= advanced_config;

    return HAL_PWM_STATUS_OK;

}

#endif /*HAL_PWM_MODULE_ENABLED*/

