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

#include "hal_platform.h"

#ifdef HAL_DISPLAY_PWM_MODULE_ENABLED

#include "hal_display_pwm.h"
#include "hal_display_pwm_internal.h"
#include "hal_clock.h"

#ifdef __cplusplus
extern "C" {
#endif

hal_display_pwm_status_t  hal_display_pwm_init(hal_display_pwm_clock_t clock)
{
    hal_display_pwm_status_t ret = HAL_DISPLAY_PWM_STATUS_OK;
    hal_clock_status_t clock_ret;

    clock_ret = hal_clock_enable(HAL_CLOCK_CG_DISP_PWM);

    if (HAL_CLOCK_STATUS_OK != clock_ret) {
        return HAL_DISPLAY_PWM_STATUS_ERROR;
    }

    ret = display_pwm_init(clock);

    return ret;
}

hal_display_pwm_status_t  hal_display_pwm_deinit(void)
{
    hal_display_pwm_status_t ret = HAL_DISPLAY_PWM_STATUS_OK;
    hal_clock_status_t clock_ret;

    clock_ret = hal_clock_disable(HAL_CLOCK_CG_DISP_PWM);

    if (HAL_CLOCK_STATUS_OK != clock_ret) {
        return HAL_DISPLAY_PWM_STATUS_ERROR;
    }

    ret = display_pwm_deinit();

    return ret;
}

hal_display_pwm_status_t hal_display_pwm_set_clock(hal_display_pwm_clock_t source_clock)
{

    hal_display_pwm_status_t ret = HAL_DISPLAY_PWM_STATUS_OK;

    if (source_clock >= HAL_DISPLAY_PWM_CLOCK_NUM) {
        return HAL_DISPLAY_PWM_STATUS_INVALID_PARAMETER;
    }

    ret = display_pwm_set_clock(source_clock);

    return ret;
}

hal_display_pwm_status_t hal_display_pwm_set_duty(uint8_t percentage)
{
    hal_display_pwm_status_t ret = HAL_DISPLAY_PWM_STATUS_OK;

    ret = display_pwm_set_duty(percentage);

    return ret;
}

hal_display_pwm_status_t hal_display_pwm_set_width(uint32_t width)
{
    hal_display_pwm_status_t ret = HAL_DISPLAY_PWM_STATUS_OK;

    if (width > 1024) {
        return HAL_DISPLAY_PWM_STATUS_INVALID_PARAMETER;
    }

    ret = display_pwm_set_width(width);

    return ret;
}
#ifdef __cplusplus
}
#endif

#endif
