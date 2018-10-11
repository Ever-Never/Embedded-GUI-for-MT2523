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
#include "hal_clock_internal.h"

#ifdef __cplusplus
extern "C" {
#endif


volatile DISP_PWM_REGISTER_T *disp_pwm_register_ptr = (DISP_PWM_REGISTER_T *)(DISP_PWM_BASE);

hal_display_pwm_status_t display_pwm_init(hal_display_pwm_clock_t clock)
{
    hal_display_pwm_status_t ret = HAL_DISPLAY_PWM_STATUS_OK;

    display_pwm_set_clock(clock);
    disp_pwm_register_ptr->DISP_PWM_CON_0_REGISTER.field.PWM_DONT_UPDATE = 0;
    disp_pwm_register_ptr->DISP_PWM_CON_0_REGISTER.field.PWM_UPDATE_SEL = 0;
    disp_pwm_register_ptr->DISP_PWM_CON_0_REGISTER.field.PWM_FRAME_SYNC = 0;
    disp_pwm_register_ptr->DISP_PWM_CON_0_REGISTER.field.PWM_AUTO_MODE = 0;
    disp_pwm_register_ptr->DISP_PWM_DEBUG_REGISTER.field.PWM_DOUBLE_BUFFER_DIS = 1;
    disp_pwm_register_ptr->DISP_PWM_EN_REGISTER.field.PWM_EN = 1;

    return ret;
}

hal_display_pwm_status_t display_pwm_deinit(void)
{
    hal_display_pwm_status_t ret = HAL_DISPLAY_PWM_STATUS_OK;

    disp_pwm_register_ptr->DISP_PWM_EN_REGISTER.field.PWM_EN = 0;

    return ret;
}

hal_display_pwm_status_t display_pwm_set_clock(hal_display_pwm_clock_t source_clock)
{
    hal_display_pwm_status_t ret = HAL_DISPLAY_PWM_STATUS_OK;

    if (source_clock == HAL_DISPLAY_PWM_CLOCK_104MHZ) {
        clock_mux_sel(DISP_PWM_MUX_SEL, 3);
    } else {
        clock_mux_sel(DISP_PWM_MUX_SEL, 1);
        clock_mux_sel(DISP_PWM_MUX_SEL, 0);
    }

    return ret;
}

hal_display_pwm_status_t display_pwm_set_duty(uint8_t percent)
{
    hal_display_pwm_status_t ret = HAL_DISPLAY_PWM_STATUS_OK;

    uint32_t value;
    value = 1024 * percent / 100;

    disp_pwm_register_ptr->DISP_PWM_CON_1_REGISTER.field.PWM_HIGH_WIDTH = value;
    disp_pwm_register_ptr->DISP_PWM_COMMIT_REGISTER.field.PWM_COMMIT = 1;
    disp_pwm_register_ptr->DISP_PWM_COMMIT_REGISTER.field.PWM_COMMIT = 0;

    return ret;
}

hal_display_pwm_status_t display_pwm_set_width(uint32_t width)
{
    hal_display_pwm_status_t ret = HAL_DISPLAY_PWM_STATUS_OK;

    disp_pwm_register_ptr->DISP_PWM_CON_1_REGISTER.field.PWM_HIGH_WIDTH = width;
    disp_pwm_register_ptr->DISP_PWM_COMMIT_REGISTER.field.PWM_COMMIT = 1;
    disp_pwm_register_ptr->DISP_PWM_COMMIT_REGISTER.field.PWM_COMMIT = 0;

    return ret;
}

#ifdef __cplusplus
}
#endif

#endif // HAL_DISPLAY_PWM_MODULE_ENABLED
