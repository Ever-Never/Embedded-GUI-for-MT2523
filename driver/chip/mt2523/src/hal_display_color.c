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

#include "hal_display_color.h"
#include "hal_display_color_internal.h"
#include "hal_display_lcd_internal.h"

hal_display_color_parameter_t color_para;
static uint8_t colorCustomEnable = 0xFF;

hal_display_color_custom_table_t colorCustomTableIndex[HAL_DISPLAY_COLOR_MODE_NUM] = {
    {1, 3, 7},
    {1, 1, 1}
};

hal_display_color_sharpness_t colorCustomSharpnessTable[COLOR_CUSTOM_SHARPNESS_TABLE_NUM + 1] = {
    {0xFF, 0xFF, 0xFF, 0xFF},  // turn off effect

    {4, 8, 2, 4},
    {8, 8, 4, 8},
    {12, 8, 6, 12},
    {16, 8, 8, 16},
    {20, 8, 10, 20},
    {24, 8, 12, 24},
    {28, 8, 14, 28},
    {32, 8, 16, 32},
    {36, 8, 18, 36},
    {40, 8, 20, 40},
};

hal_display_color_contrast_t colorCustomContrastTable[COLOR_CUSTOM_CONTRAST_TABLE_NUM + 1] = {
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF},  // turn off effect

    {64, 192, 128, 128, 128},
    {64, 192, 120, 136, 120},
    {64, 192, 112, 144, 112},
    {64, 192, 104, 152, 104},
    {64, 192, 96, 160, 96},
    {64, 192, 88, 168, 88},
    {64, 192, 80, 176, 80},
    {64, 192, 72, 184, 72},
    {64, 192, 64, 192, 64},
    {64, 192, 56, 200, 56},
};

hal_display_color_saturation_t colorCustomSaturationTable[COLOR_CUSTOM_SATURATION_TABLE_NUM + 1] = {
    {0xFFFF, 0xFF, 0xFF},  // turn off effect

    {1160, 128, 120},
    {1044, 128, 128},
    {928, 128, 136},
    {812, 128, 144},
    {696, 128, 152},
    {580, 128, 160},
    {464, 128, 168},
    {348, 128, 176},
    {232, 128, 184},
    {116, 128, 192},
};

uint32_t colorGetCustomSharpnessIndex(hal_display_color_mode_t scenario)
{
    hal_display_color_custom_table_t *customTable;

    if (HAL_DISPLAY_COLOR_MODE_NUM <= scenario) {
		return 0;
    }

    customTable = &(colorCustomTableIndex[scenario]);

    return customTable->sharpness;
}

uint32_t colorGetCustomContrastIndex(hal_display_color_mode_t scenario)
{
    hal_display_color_custom_table_t *customTable;

    if (HAL_DISPLAY_COLOR_MODE_NUM <= scenario) {
		return 0;
    }

    customTable = &(colorCustomTableIndex[scenario]);

    return customTable->contrast;
}

uint32_t colorGetCustomSaturationIndex(hal_display_color_mode_t scenario)
{
    hal_display_color_custom_table_t *customTable;

    if (HAL_DISPLAY_COLOR_MODE_NUM <= scenario) {
		return 0;
    }

    customTable = &(colorCustomTableIndex[scenario]);

    return customTable->saturation;

}

hal_display_color_status_t hal_display_color_init(void)
{
    color_para.mode = HAL_DISPLAY_COLOR_MODE_DEFAULT;
    color_para.bypass_config = 0;

    hal_display_color_set_mode(color_para.mode);

    return HAL_DISPLAY_COLOR_STATUS_OK;
}


hal_display_color_status_t hal_display_color_set_color_engine(bool enable)
{
    colorCustomEnable = (bool)enable;

    return HAL_DISPLAY_COLOR_STATUS_OK;
}

hal_display_color_status_t hal_display_color_get_color_engine(bool *enable)
{
    if (NULL == enable) {
		return HAL_DISPLAY_COLOR_STATUS_INVALID_PARAMETER;
    }

    if (0xFF == colorCustomEnable) {
        // default turn on
        *enable = 1;
    } else {
        *enable = (bool)colorCustomEnable;
    }

    return HAL_DISPLAY_COLOR_STATUS_OK;
}

hal_display_color_status_t hal_display_color_set_sharpness(hal_display_color_sharpness_t *sharpness)
{
    uint32_t colorBypass;

    if (NULL == sharpness) {
		return HAL_DISPLAY_COLOR_STATUS_INVALID_PARAMETER;
    }

    colorBypass = color_para.bypass_config & (~COLOR_BYPASS_SHARP);
    if ((0xFF == sharpness->gain)
            && (0xFF == sharpness->soft_ratio)
            && (0xFF == sharpness->limit)
            && (0xFF == sharpness->bound)
       ) {
        colorBypass |= COLOR_BYPASS_SHARP;
    }
    color_para.bypass_config = colorBypass;

    // Shoud change to memcpy
    color_para.sharpness.gain = sharpness->gain;
    color_para.sharpness.soft_ratio = sharpness->soft_ratio;
    color_para.sharpness.limit = sharpness->limit;
    color_para.sharpness.bound = sharpness->bound;

    return HAL_DISPLAY_COLOR_STATUS_OK;

}

hal_display_color_status_t hal_display_color_set_contrast(hal_display_color_contrast_t *contrast)
{
    uint32_t colorBypass;

    if (NULL == contrast) {
		return HAL_DISPLAY_COLOR_STATUS_INVALID_PARAMETER;
    }

    colorBypass = color_para.bypass_config & (~COLOR_BYPASS_CONTRAST);
    if ((0xFF == contrast->point0)
            && (0xFF == contrast->point1)
            && (0xFF == contrast->gain0)
            && (0xFF == contrast->gain1)
            && (0xFF == contrast->gain2)
       ) {
        colorBypass |= COLOR_BYPASS_CONTRAST;
    }
    color_para.bypass_config = colorBypass;

    // Shoud change to memcpy
    color_para.contrast.point0 = contrast->point0;
    color_para.contrast.point1 = contrast->point1;
    color_para.contrast.gain0 = contrast->gain0;
    color_para.contrast.gain1 = contrast->gain1;
    color_para.contrast.gain2 = contrast->gain2;

    return HAL_DISPLAY_COLOR_STATUS_OK;
}

hal_display_color_status_t hal_display_color_set_saturation(hal_display_color_saturation_t *saturation)
{
    uint32_t colorBypass;

    if (NULL == saturation) {
	   return HAL_DISPLAY_COLOR_STATUS_INVALID_PARAMETER;
    }

    colorBypass = color_para.bypass_config & (~COLOR_BYPASS_SAT);
    if ((0xFFFF == saturation->ctrl_point)
            && (0xFF == saturation->coef_a)
            && (0xFF == saturation->gain)
       ) {
        colorBypass |= COLOR_BYPASS_SAT;
    }
    color_para.bypass_config = colorBypass;

    // Shoud change to memcpy
    color_para.saturation.ctrl_point = saturation->ctrl_point;
    color_para.saturation.coef_a = saturation->coef_a;
    color_para.saturation.gain = saturation->gain;

    return HAL_DISPLAY_COLOR_STATUS_OK;
}

hal_display_color_status_t hal_display_color_set_bypass(bool bypass)
{
    color_para.bypass_config |= bypass;

    return HAL_DISPLAY_COLOR_STATUS_OK;
}

hal_display_color_status_t hal_display_color_set_mode(hal_display_color_mode_t mode)
{
    hal_display_color_sharpness_t *sharpnessTable;
    hal_display_color_contrast_t *contrastTable;
    hal_display_color_saturation_t *saturationTable;

    color_para.mode = mode;

    sharpnessTable = &(colorCustomSharpnessTable[colorGetCustomSharpnessIndex(color_para.mode)]);
    contrastTable = &(colorCustomContrastTable[colorGetCustomContrastIndex(color_para.mode)]);
    saturationTable = &(colorCustomSaturationTable[colorGetCustomSaturationIndex(color_para.mode)]);

    hal_display_color_set_sharpness(sharpnessTable);
    hal_display_color_set_contrast(contrastTable);
    hal_display_color_set_saturation(saturationTable);

    return HAL_DISPLAY_COLOR_STATUS_OK;
}


hal_display_color_status_t hal_display_color_apply_color(uint32_t src_width)
{
    display_color_power_on();

    display_color_apply_color(src_width, colorCustomEnable, &color_para);

    display_color_power_off();

    return HAL_DISPLAY_COLOR_STATUS_OK;
}

#endif
