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

#ifdef HAL_DISPLAY_COLOR_MODULE_ENABLED

#include "hal_display_color_internal.h"
#include "hal_display_lcd_internal.h"
#include "hal_display_color.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"

volatile MM_COLOR_REGISTER_T *mm_color_register_ptr = (MM_COLOR_REGISTER_T *)(MM_COLOR_BASE);
extern volatile LCD_REGISTER_T	*lcd_register_ptr;

hal_display_color_status_t display_color_apply_color(uint32_t src_width, uint8_t CustomEnable, hal_display_color_parameter_t *color_para)
{
    if (0 == CustomEnable) {
        DISABLE_LCD_PQ_DITHER_EN;
        DISABLE_LCD_COLOR;

        mm_color_register_ptr->MM_COLOR_BYPASS_REGISTER.value = COLOR_BYPASS_ALL;
        mm_color_register_ptr->MM_COLOR_EN_REGISTER.field.EN = 0;
    } else {
        // Config sharpness
        if (0 == (color_para->bypass_config & COLOR_BYPASS_SHARP)) {
            mm_color_register_ptr->MM_COLOR_SHARP_CON_REGISTER.field.SHARP_GAIN = color_para->sharpness.gain;
            mm_color_register_ptr->MM_COLOR_SHARP_CON_REGISTER.field.SHARP_SOFT_RATIO = color_para->sharpness.soft_ratio;
            mm_color_register_ptr->MM_COLOR_SHARP_CON_REGISTER.field.SHARP_LIMIT = color_para->sharpness.limit;
            mm_color_register_ptr->MM_COLOR_SHARP_CON_REGISTER.field.SHARP_BOUND = color_para->sharpness.bound;
        }


        // Config contrast
        if (0 == (color_para->bypass_config & COLOR_BYPASS_CONTRAST)) {
            uint16_t offset0, offset1;

            offset0 = (uint16_t)color_para->contrast.point0 * (uint16_t)color_para->contrast.gain0;
            offset1 = offset0 + ((uint16_t)color_para->contrast.point1 - (uint16_t)color_para->contrast.point0) * (uint16_t)color_para->contrast.gain1;

            mm_color_register_ptr->MM_COLOR_CONTRAST_POINT_REGISTER.field.CONTRAST_POINT0 = color_para->contrast.point0;
            mm_color_register_ptr->MM_COLOR_CONTRAST_POINT_REGISTER.field.CONTRAST_POINT1 = color_para->contrast.point1;
            mm_color_register_ptr->MM_COLOR_CONTRAST_GAIN_REGISTER.field.CONTRAST_GAIN0 = color_para->contrast.gain0;
            mm_color_register_ptr->MM_COLOR_CONTRAST_GAIN_REGISTER.field.CONTRAST_GAIN1 = color_para->contrast.gain1;
            mm_color_register_ptr->MM_COLOR_CONTRAST_GAIN_REGISTER.field.CONTRAST_GAIN2 = color_para->contrast.gain2;
            mm_color_register_ptr->MM_COLOR_CONTRAST_OFFSET_REGISTER.value = offset1 << 16 | offset0;

        }

        // Config saturation
        if (0 == (color_para->bypass_config & COLOR_BYPASS_SAT)) {
            uint32_t squarepoint, coefb;

            squarepoint = ((uint32_t)color_para->saturation.ctrl_point * (uint32_t)color_para->saturation.ctrl_point + (1 << 3)) >> 4;
            coefb = (uint32_t)color_para->saturation.gain + (((uint32_t)color_para->saturation.coef_a * squarepoint + (1 << 10)) >> 11);

            mm_color_register_ptr->MM_COLOR_SAT_GAIN_REGISTER.field.SAT_GAIN = color_para->saturation.gain;
            mm_color_register_ptr->MM_COLOR_SAT_POINT_REGISTER.field.SAT_COEF_A = color_para->saturation.coef_a;
            mm_color_register_ptr->MM_COLOR_SAT_POINT_REGISTER.field.SAT_COEF_B = coefb;
            mm_color_register_ptr->MM_COLOR_SAT_POINT_REGISTER.field.SAT_CON_POINT = squarepoint;
        }


        color_para->bypass_config &= ~(COLOR_BYPASS_Y2R | COLOR_BYPASS_R2Y);
        ENABLE_LCD_PQ_DITHER_EN;
        ENABLE_LCD_COLOR;

        mm_color_register_ptr->MM_COLOR_SRC_SIZE_REGISTER.field.SRC_W = src_width;
        mm_color_register_ptr->MM_COLOR_BYPASS_REGISTER.value = color_para->bypass_config;
        mm_color_register_ptr->MM_COLOR_EN_REGISTER.field.EN = 1;
    }

    return HAL_DISPLAY_COLOR_STATUS_OK;
}


hal_display_color_status_t display_color_power_on(void)
{
    hal_display_color_status_t ret = HAL_DISPLAY_COLOR_STATUS_OK;

    hal_clock_status_t clock_ret;

    clock_ret = hal_clock_enable(HAL_CLOCK_CG_MM_COLOR);

    if (clock_ret != HAL_CLOCK_STATUS_OK) {
        ret = HAL_DISPLAY_COLOR_STATUS_POWER_FAILED;
    }

    return ret;
}

hal_display_color_status_t display_color_power_off(void)
{
    hal_display_color_status_t ret = HAL_DISPLAY_COLOR_STATUS_OK;

    hal_clock_status_t clock_ret;

    clock_ret = hal_clock_disable(HAL_CLOCK_CG_MM_COLOR);

    if (clock_ret != HAL_CLOCK_STATUS_OK) {
        ret = HAL_DISPLAY_COLOR_STATUS_POWER_FAILED;
    }

    return ret;
}


#endif
