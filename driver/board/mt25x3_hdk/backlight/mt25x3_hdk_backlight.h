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

#ifndef __MT25X3_HDK_BACKLIGHT_H__
#define __MT25X3_HDK_BACKLIGHT_H__

#ifdef HAL_ISINK_MODULE_ENABLED
#include "hal_isink.h"
#endif
#ifdef HAL_DISPLAY_PWM_MODULE_ENABLED
#include "hal_display_pwm.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BSP_BACKLIGHT_STATUS_ERROR = -1,
    BSP_BACKLIGHT_STATUS_OK = 0
} bsp_backlight_status_t;

bsp_backlight_status_t bsp_backlight_enable(bool enable);
bsp_backlight_status_t bsp_backlight_init(void);
bsp_backlight_status_t bsp_backlight_deinit(void);

#ifdef HAL_ISINK_MODULE_ENABLED
bsp_backlight_status_t bsp_backlight_init_isink(hal_isink_channel_t channel, bool isDouble, hal_isink_current_t current);
bsp_backlight_status_t bsp_backlight_set_clock_source_isink(hal_isink_channel_t channel, hal_isink_clock_source_t source_clock);
bsp_backlight_status_t bsp_backlight_set_step_current_isink(hal_isink_channel_t channel, hal_isink_current_t current);
bsp_backlight_status_t bsp_backlight_set_double_current_isink(hal_isink_channel_t channel, hal_isink_current_t current);
#endif

#ifdef HAL_DISPLAY_PWM_MODULE_ENABLED
bsp_backlight_status_t bsp_backlight_init_display_pwm(void);
bsp_backlight_status_t bsp_backlight_set_clock_source_display_pwm(hal_display_pwm_clock_t source_clock);
bsp_backlight_status_t bsp_backlight_set_duty_display_pwm(uint8_t percent);
bsp_backlight_status_t bsp_backlight_set_width_display_pwm(uint32_t width);
#endif

bsp_backlight_status_t bsp_backlight_init_lcm_brightness(void);
bsp_backlight_status_t bsp_backlight_deinit_lcm_brightness(void);
bsp_backlight_status_t bsp_backlight_set_step_lcm_brightness(uint8_t level);

#ifdef MTK_EXTERNAL_PMIC
bsp_backlight_status_t bsp_backlight_init_external_isink(void);
bsp_backlight_status_t bsp_backlight_deinit_external_isink(void);
#endif

#ifdef __cplusplus
}
#endif

#endif //__MT25X3_HDK_BACKLIGHT_H__
