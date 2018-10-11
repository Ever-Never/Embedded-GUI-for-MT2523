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

#ifdef HAL_G2D_MODULE_ENABLED
#include <string.h>

#include "hal_cache.h"
#include "hal_clock.h"
#include "hal_cm4_topsm.h"
#include "hal_gpt.h"
#include "hal_log.h"
#include "hal_nvic.h"
#include "hal_sleep_manager.h"

#include "hal_g2d.h"
#include "hal_g2d_internal.h"

#define HAL_G2D_COLOR_FORMAT_MAX (HAL_G2D_COLOR_FORMAT_UYVY + 1)
#define HAL_G2D_FONT_BIT_MAX     (HAL_G2D_FONT_FORMAT_8_BIT + 1)
#define HAL_G2D_ROTATE_MAX       (HAL_G2D_ROTATE_ANGLE_MIRROR_270 + 1)

volatile G2D_REGISTER_T *g2d_register_ptr = (G2D_REGISTER_T *)(G2D_BASE);

static g2d_context_t g2d_context;
static g2d_dither_context_t g2d_dither_context;
static bool g2d_allocate;
#ifdef HAL_SLEEP_MANAGER_ENABLED
const char *g2d_name = "G2D";
static uint8_t g2d_sleep_handle = 0xFF;
volatile bool g2d_lock = false;
#endif

const uint8_t g2d_clrfmt[HAL_G2D_COLOR_FORMAT_MAX] = {
    G2D_CLRFMT_RGB565, G2D_CLRFMT_RGB888, G2D_CLRFMT_BGR888, G2D_CLRFMT_ARGB8565,
    G2D_CLRFMT_ARGB6666, G2D_CLRFMT_ARGB8888, G2D_CLRFMT_PARGB8565, G2D_CLRFMT_PARGB6666,
    G2D_CLRFMT_PARGB8888, G2D_CLRFMT_UYVY
};
const uint8_t g2d_clrfmt_Bpp[HAL_G2D_COLOR_FORMAT_MAX] = {
    2, 3, 3, 3,
    3, 4, 3, 3,
    4, 2
};
const uint8_t g2d_font_format[HAL_G2D_FONT_BIT_MAX] = {0, 1, 2, 3};
const uint8_t g2d_font_format_bpp[HAL_G2D_FONT_BIT_MAX] = {1, 2, 4, 8};
const uint8_t g2d_rotate[HAL_G2D_ROTATE_MAX] = {
    G2D_ROTATE_0, G2D_ROTATE_90, G2D_ROTATE_180, G2D_ROTATE_270,
    G2D_ROTATE_FLIP_0, G2D_ROTATE_FLIP_90, G2D_ROTATE_FLIP_180, G2D_ROTATE_FLIP_270
};

bool _hal_g2d_check_available(void)
{
    uint32_t saved_mask;
    bool available = false;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (!g2d_allocate) {
        g2d_allocate = true;
        available = true;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    return available;
}

void _hal_g2d_set_available(void)
{
    g2d_allocate = false;
}

bool _hal_g2d_check_offset(int32_t x)
{
    return ((x < -2048) || (x > 2047)) ? false : true;
}

bool _hal_g2d_check_area(hal_g2d_area_t *area)
{
    if (_hal_g2d_check_offset(area->x) == false) {
        return false;
    }

    if (_hal_g2d_check_offset(area->y) == false) {
        return false;
    }

    if ((area->width == 0) || (area->width > 2048)) {
        return false;
    }

    if ((area->height == 0) || (area->height > 2048)) {
        return false;
    }

    return true;
}

bool _hal_g2d_check_color(hal_g2d_color_format_t color_format, uint32_t color)
{
    uint32_t mask = 0xFFFFFFFF, invalid_value = 0;

    if (g2d_clrfmt_Bpp[color_format] == 2) {
        mask = 0xFFFF0000;
    }
    if (g2d_clrfmt_Bpp[color_format] == 3) {
        mask = 0xFF000000;
    }

    if (g2d_clrfmt_Bpp[color_format] != 4) {
        invalid_value = color & mask;
    }

    if (invalid_value) {
        return false;
    }

    return true;
}

void _hal_g2d_warm_reset(void)
{
    g2d_register_ptr->G2D_RESET = G2D_RESET_WRST;
    while (g2d_register_ptr->G2D_STATUS)
        ;
    g2d_register_ptr->G2D_RESET = G2D_RESET_HRST;
    g2d_register_ptr->G2D_RESET = 0;
}

void _hal_g2d_hard_reset(void)
{
    g2d_register_ptr->G2D_RESET = 0;
    g2d_register_ptr->G2D_RESET = G2D_RESET_HRST;
    g2d_register_ptr->G2D_RESET = 0;
}

void _hal_g2d_power_on(void)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    if (!g2d_lock) {
        hal_sleep_manager_lock_sleep(g2d_sleep_handle);
        g2d_lock = true;
    }
#endif

    if (!hal_clock_is_enabled(HAL_CLOCK_CG_G2D)) {
        cm4_topsm_lock_MM_MTCMOS();
        hal_clock_enable(HAL_CLOCK_CG_G2D);
    }
}

void _hal_g2d_power_off(void)
{
    if (hal_clock_is_enabled(HAL_CLOCK_CG_G2D)) {
        hal_clock_disable(HAL_CLOCK_CG_G2D);
        cm4_topsm_unlock_MM_MTCMOS();
    }

#ifdef HAL_SLEEP_MANAGER_ENABLED
    if (g2d_lock) {
        hal_sleep_manager_unlock_sleep(g2d_sleep_handle);
        g2d_lock = false;
    }
#endif
}

bool _hal_g2d_is_busy(void)
{
    if (g2d_register_ptr->G2D_STATUS & (G2D_STATUS_BUSY_BIT | G2D_STATUS_TBUSY_BIT)) {
        return true;
    }

    return false;
}

bool _hal_g2d_wait_frame_done(void)
{
    uint32_t gpt_start_count, gpt_current_count, gpt_duration_count;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
    while (_hal_g2d_is_busy()) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
        hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
        if ((gpt_duration_count > G2D_POLLING_TIMEOUT) && _hal_g2d_is_busy()) {
            log_hal_info("_hal_g2d_wait_frame_done timeout\n");
            return false;
        }
    }

    return true;
}

void _hal_g2d_coordinate_correction(hal_g2d_rotate_angle_t rotate, hal_g2d_area_t *area)
{
    switch (rotate) {
        case HAL_G2D_ROTATE_ANGLE_90:
            area->y += area->width - 1;
            break;
        case HAL_G2D_ROTATE_ANGLE_180:
            area->x += area->width - 1;
            area->y += area->height - 1;
            break;
        case HAL_G2D_ROTATE_ANGLE_270:
            area->x += area->height - 1;
            break;
        case HAL_G2D_ROTATE_ANGLE_MIRROR_0:
            area->x += area->width - 1;
            break;
        case HAL_G2D_ROTATE_ANGLE_MIRROR_180:
            area->y += area->height - 1;
            break;
        case HAL_G2D_ROTATE_ANGLE_MIRROR_270:
            area->x += area->height - 1;
            area->y += area->width - 1;
            break;
        default:
            break;
    }
}

void _hal_g2d_isr(hal_nvic_irq_t irq_number)
{
    if (g2d_register_ptr->G2D_IRQ & G2D_IRQ_FLAG0) {
        g2d_register_ptr->G2D_IRQ &= ~G2D_IRQ_FLAG0;
    }

    if (g2d_context.callback) {
        g2d_context.callback(NULL);
    }

    if (hal_g2d_get_status((hal_g2d_handle_t *)&g2d_context) == HAL_G2D_STATUS_OK) {
#ifdef HAL_SLEEP_MANAGER_ENABLED
        if (g2d_lock) {
            hal_sleep_manager_unlock_sleep(g2d_sleep_handle);
            g2d_lock = false;
        }
#endif
    }
}

void _hal_g2d_start(void)
{
    int32_t i;
    g2d_layer_t *g2d_layer;

    g2d_register_ptr->G2D_IRQ = 0;

    _hal_g2d_hard_reset();

    if (g2d_context.speed) {
        g2d_register_ptr->G2D_SLOW_DOWN |= G2D_SLOW_DOWN_EN | ((100 - g2d_context.speed) & G2D_SLOW_DOWN_SLOW_CNT_MASK);
    }

    g2d_register_ptr->G2D_ROI_CON =
        g2d_context.source[0].enable << G2D_ROI_CON_EN0_OFFSET |
        g2d_context.source[1].enable << G2D_ROI_CON_EN1_OFFSET |
        g2d_context.source[2].enable << G2D_ROI_CON_EN2_OFFSET |
        g2d_context.source[3].enable << G2D_ROI_CON_EN3_OFFSET |
        g2d_context.enable_color_replace << G2D_ROI_CON_CLR_REP_EN_OFFSET |
        g2d_context.enable_clipping << G2D_ROI_CON_CLP_EN_OFFSET |
        g2d_context.destination.alpha << G2D_ROI_CON_OUT_ALPHA_OFFSET |
        g2d_context.destination.enable_alpha << G2D_ROI_CON_OUT_ALP_EN_OFFSET |
        g2d_clrfmt[g2d_context.destination.color_format];

    g2d_register_ptr->G2D_W2M_ADDR = (uint32_t)g2d_context.destination.address;
    g2d_register_ptr->G2D_W2M_PITCH = g2d_context.destination.pitch;
    g2d_register_ptr->G2D_ROI_OFS = (g2d_context.destination.area.x << G2D_X_OFFSET) | (g2d_context.destination.area.y & G2D_OFFSET_MASK);
    g2d_register_ptr->G2D_ROI_SIZE = (g2d_context.destination.area.width << G2D_WIDTH_OFFSET) | g2d_context.destination.area.height;
    g2d_register_ptr->G2D_ROI_BGCLR = g2d_context.destination.color;

    if (g2d_context.enable_clipping) {
        g2d_register_ptr->G2D_CLP_MIN = (g2d_context.clipping_min.x << G2D_X_OFFSET) | (g2d_context.clipping_min.y & G2D_OFFSET_MASK);
        g2d_register_ptr->G2D_CLP_MAX = (g2d_context.clipping_max.x << G2D_X_OFFSET) | (g2d_context.clipping_max.y & G2D_OFFSET_MASK);
    }

    if (g2d_context.enable_color_replace) {
        g2d_register_ptr->G2D_AVO_CLR = g2d_context.avoid_color;
        g2d_register_ptr->G2D_REP_CLR = g2d_context.replaced_color;
    }

    g2d_register_ptr->G2D_W2M_MOFS = g2d_context.destination_offset.x << G2D_X_OFFSET | (g2d_context.destination_offset.y & G2D_OFFSET_MASK);

    if (g2d_dither_context.mode != HAL_G2D_DITHERING_MODE_DISABLE) {
        g2d_register_ptr->G2D_DI_CON = (g2d_dither_context.red_bit << G2D_DI_CON_DI_R_OFFSET) | (g2d_dither_context.green_bit << G2D_DI_CON_DI_G_OFFSET) | (g2d_dither_context.blue_bit << G2D_DI_CON_DI_B_OFFSET) | g2d_dither_context.mode;
    }

    for (i = 0; i < G2D_MAX_LAYER; i++) {
        g2d_layer = &g2d_context.source[i];
        if (g2d_layer->enable) {
            g2d_register_ptr->G2D_L[i].CON =
                (g2d_layer->function == HAL_G2D_OVERLAY_LAYER_FUNCTION_FONT ? 1 : 0) << G2D_LX_CON_FONT_EN_OFFSET |
                (g2d_font_format[g2d_layer->font_format] & 0x00000003) << G2D_LX_CON_IDX_OFFSET |
                g2d_layer->enable_color_key << G2D_LX_CON_SKEY_EN_OFFSET |
                (g2d_layer->function == HAL_G2D_OVERLAY_LAYER_FUNCTION_RECTFILL ? 1 : 0) << G2D_LX_CON_RECT_EN_OFFSET |
                (g2d_rotate[g2d_layer->rotate] & 0x00000007) << G2D_LX_CON_ROT_OFFSET |
                g2d_layer->alpha << G2D_LX_CON_ALPHA_OFFSET |
                g2d_layer->enable_alpha << G2D_LX_CON_ALP_EN_OFFSET |
                g2d_clrfmt[g2d_layer->color_format];

            g2d_register_ptr->G2D_L[i].ADDR = (uint32_t)g2d_layer->address;
            g2d_register_ptr->G2D_L[i].PITCH = g2d_layer->pitch;
            g2d_register_ptr->G2D_L[i].OFS = (g2d_layer->area.x << G2D_X_OFFSET) | (g2d_layer->area.y & G2D_OFFSET_MASK);
            g2d_register_ptr->G2D_L[i].SIZE = (g2d_layer->area.width << G2D_WIDTH_OFFSET) | g2d_layer->area.height;
            g2d_register_ptr->G2D_L[i].SRCKEY = g2d_layer->color;
        }
    }

    g2d_register_ptr->G2D_IRQ = G2D_IRQ_FLAG0_IRQ_EN;
    g2d_register_ptr->G2D_START = G2D_START_START;
}

void _hal_g2d_stop(bool force)
{
    if (!hal_clock_is_enabled(HAL_CLOCK_CG_G2D)) {
        return;
    }

    if (force) {
        if (!_hal_g2d_wait_frame_done()) {
            _hal_g2d_hard_reset();
        }
    } else {
        if (_hal_g2d_is_busy()) {
            return;
        }
    }

    g2d_register_ptr->G2D_IRQ = 0;
    g2d_register_ptr->G2D_START = 0;
    _hal_g2d_power_off();
}

void _hal_g2d_dump_register(void)
{
    int i;

    for (i = 0; i < 0x158; i += 4) {
        log_hal_info("G2D: %03X = %08X\r\n", i, *((volatile unsigned int *)(G2D_BASE + i)));
    }
}

hal_g2d_status_t hal_g2d_init(void)
{
    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_get_handle(hal_g2d_handle_t **handle_ptr, hal_g2d_codec_type_t codec_type, hal_g2d_get_handle_mode_t handle_mode)
{
    *handle_ptr = (void *)&g2d_context;

    if (_hal_g2d_check_available() == false) {
        return HAL_G2D_STATUS_BUSY;
    }

    memset(&g2d_context, 0, sizeof(g2d_context_t));
    memset(&g2d_dither_context, 0, sizeof(g2d_dither_context_t));

    if (handle_mode == HAL_G2D_GET_HANDLE_MODE_BLOCKING) {
        g2d_context.is_blocking = true;
    }

    hal_nvic_register_isr_handler(G2D_IRQn, _hal_g2d_isr);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    if (g2d_sleep_handle == 0xFF) {
        g2d_sleep_handle = hal_sleep_manager_set_sleep_handle(g2d_name);
    }
#endif

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_release_handle(hal_g2d_handle_t *handle)
{
    _hal_g2d_stop(true);

    _hal_g2d_set_available();

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_register_callback(hal_g2d_handle_t *handle, hal_g2d_callback_t callback)
{
    if (callback == NULL) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    g2d_context.callback = callback;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_get_status(hal_g2d_handle_t *handle)
{
    if (!hal_clock_is_enabled(HAL_CLOCK_CG_G2D)) {
        return HAL_G2D_STATUS_OK;
    }

    if (g2d_register_ptr->G2D_STATUS & (G2D_STATUS_BUSY_BIT | G2D_STATUS_TBUSY_BIT)) {
        return HAL_G2D_STATUS_BUSY;
    }

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_start(hal_g2d_handle_t *handle)
{
    hal_g2d_status_t status = HAL_G2D_STATUS_OK;

    if (g2d_context.is_blocking) {
        _hal_g2d_power_on();
        if (_hal_g2d_is_busy()) {
            return HAL_G2D_STATUS_BUSY;
        }
        _hal_g2d_start();
        if (!_hal_g2d_wait_frame_done()) {
            status = HAL_G2D_STATUS_HW_ERROR;
        }
        _hal_g2d_power_off();
    } else {
        _hal_g2d_power_on();
        if (_hal_g2d_is_busy()) {
            return HAL_G2D_STATUS_BUSY;
        }
        _hal_g2d_start();
    }

    return status;
}

hal_g2d_status_t hal_g2d_query_rgb_buffer_range(hal_g2d_handle_t *handle, uint32_t *min_width, uint32_t *max_width, uint32_t *min_height, uint32_t *max_height)
{
    *min_width = 1;
    *max_width = 2048;
    *min_height = 1;
    *max_height = 2048;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_query_support(hal_g2d_codec_type_t codecType, hal_g2d_supported_function_t function)
{
    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_set_alpha(hal_g2d_handle_t *handle, uint32_t alpha_value)
{
    if (alpha_value > 255) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    g2d_context.destination.alpha = alpha_value;
    g2d_context.destination.enable_alpha = true;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_set_rgb_buffer(hal_g2d_handle_t *handle, uint8_t *buffer, uint32_t size, uint32_t width, uint32_t height, hal_g2d_color_format_t color_format)
{
    return hal_g2d_set_rgb_buffer(handle, buffer, size, width, height, color_format);
}

hal_g2d_status_t hal_g2d_set_rgb_buffer(hal_g2d_handle_t *handle, uint8_t *buffer, uint32_t size, uint32_t width, uint32_t height, hal_g2d_color_format_t color_format)
{
    if (color_format == HAL_G2D_COLOR_FORMAT_UYVY) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if (buffer == NULL) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if (true == hal_cache_is_cacheable((uint32_t)buffer)) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if ((uint32_t)buffer & BL_BASE) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if ((width == 0) || (width > 2048)) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if ((height == 0) || (height > 2048)) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    g2d_context.destination.color_format = color_format;
    g2d_context.destination.address = buffer;
    g2d_context.destination.area.width = width;
    g2d_context.destination.area.height = height;
    g2d_context.destination.pitch = width * g2d_clrfmt_Bpp[color_format];

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_set_roi_window(hal_g2d_handle_t *handle, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    hal_g2d_area_t area;

    area.x = x;
    area.y = y;
    area.width = width;
    area.height = height;
    if (_hal_g2d_check_area(&area) == false) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    g2d_context.destination.area.x = area.x;
    g2d_context.destination.area.y = area.y;
    g2d_context.destination.area.width = area.width;
    g2d_context.destination.area.height = area.height;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_set_clipping_window(hal_g2d_handle_t *handle, int32_t x, int32_t y, int32_t width, int32_t height)
{
    if ((width == 0) || (height == 0)) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if (_hal_g2d_check_offset(x) == false) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if (_hal_g2d_check_offset(y) == false) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if (_hal_g2d_check_offset(x + width - 1) == false) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if (_hal_g2d_check_offset(y + height - 1) == false) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    g2d_context.clipping_min.x = x;
    g2d_context.clipping_min.y = y;
    g2d_context.clipping_max.x = x + width - 1;
    g2d_context.clipping_max.y = y + height - 1;

    g2d_context.enable_clipping = true;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_set_window(hal_g2d_handle_t *handle, int32_t x, int32_t y)
{
    if (_hal_g2d_check_offset(x) == false) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if (_hal_g2d_check_offset(y) == false) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    g2d_context.destination_offset.x = x;
    g2d_context.destination_offset.y = y;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_replace_color(hal_g2d_handle_t *handle, uint8_t alpha0, uint8_t red0, uint8_t green0, uint8_t blue0, uint8_t alpha1, uint8_t red1, uint8_t green1, uint8_t blue1)
{
    uint32_t avoid_color = 0;
    uint32_t replaced_color = 0;

    switch (g2d_context.destination.color_format) {
        case HAL_G2D_COLOR_FORMAT_RGB565:
            avoid_color = G2D_RGB888_TO_RGB565(red0, green0, blue0);
            replaced_color = G2D_RGB888_TO_RGB565(red1, green1, blue1);
            break;
        case HAL_G2D_COLOR_FORMAT_RGB888:
            avoid_color = G2D_RGB888_PACKED(red0, green0, blue0);
            replaced_color = G2D_RGB888_PACKED(red1, green1, blue1);
            break;
        case HAL_G2D_COLOR_FORMAT_BGR888:
            avoid_color = G2D_BGR888_PACKED(red0, green0, blue0);
            replaced_color = G2D_BGR888_PACKED(red1, green1, blue1);
            break;
        case HAL_G2D_COLOR_FORMAT_ARGB8565:
        case HAL_G2D_COLOR_FORMAT_PARGB8565:
            avoid_color = G2D_ARGB8888_TO_ARGB8565(alpha0, red0, green0, blue0);
            replaced_color = G2D_ARGB8888_TO_ARGB8565(alpha1, red1, green1, blue1);
            break;
        case HAL_G2D_COLOR_FORMAT_ARGB6666:
        case HAL_G2D_COLOR_FORMAT_PARGB6666:
            avoid_color = G2D_ARGB8888_TO_ARGB6666(alpha0, red0, green0, blue0);
            replaced_color = G2D_ARGB8888_TO_ARGB6666(alpha1, red1, green1, blue1);
            break;
        case HAL_G2D_COLOR_FORMAT_ARGB8888:
        case HAL_G2D_COLOR_FORMAT_PARGB8888:
            avoid_color = G2D_ARGB8888_PACKED(alpha0, red0, green0, blue0);
            replaced_color = G2D_ARGB8888_PACKED(alpha1, red1, green1, blue1);
            break;
        default:
            return HAL_G2D_STATUS_NOT_SUPPORTEDED_COLOR_FORMAT;
    }

    g2d_context.avoid_color = avoid_color;
    g2d_context.replaced_color = replaced_color;

    g2d_context.enable_color_replace = true;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_set_background_color(hal_g2d_handle_t *handle, uint32_t color)
{
    g2d_context.destination.color = color;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_set_dithering(hal_g2d_handle_t *handle, hal_g2d_dithering_mode_t mode, uint8_t red_bit, uint8_t green_bit, uint8_t blue_bit)
{
    if (mode != HAL_G2D_DITHERING_MODE_DISABLE) {
        if ((red_bit > 3) || (green_bit > 3) || (blue_bit > 3)) {
            return HAL_G2D_STATUS_INVALID_PARAMETER;
        }

        if ((red_bit == 0) && (green_bit == 0) && (blue_bit == 0)) {
            mode = HAL_G2D_DITHERING_MODE_DISABLE;
        }
    }

    g2d_dither_context.mode = mode;
    g2d_dither_context.red_bit = red_bit;
    g2d_dither_context.green_bit = green_bit;
    g2d_dither_context.blue_bit = blue_bit;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_enable_layer(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer, bool enable)
{
    g2d_context.source[layer].enable = enable;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_set_layer_function(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer, hal_g2d_overlay_layer_function_t function)
{
    memset(&g2d_context.source[layer], 0, sizeof(g2d_layer_t));
    g2d_context.source[layer].function = function;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_set_layer_alpha(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer, uint32_t alpha_value)
{
    if (alpha_value > 255) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    g2d_context.source[layer].alpha = alpha_value;
    g2d_context.source[layer].enable_alpha = true;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_set_layer_rgb_buffer(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer, uint8_t *buffer, uint32_t size, uint32_t width, uint32_t height, hal_g2d_color_format_t color_format)
{
    if (g2d_context.source[layer].function == HAL_G2D_OVERLAY_LAYER_FUNCTION_RECTFILL) {
        return HAL_G2D_STATUS_NOT_SUPPORTED;
    }

    if (buffer == NULL) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if (true == hal_cache_is_cacheable((uint32_t)buffer)) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if ((uint32_t)buffer & BL_BASE) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if ((width == 0) || (width > 2048)) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if ((height == 0) || (height > 2048)) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    if (g2d_context.source[layer].function == HAL_G2D_OVERLAY_LAYER_FUNCTION_FONT) {
        if (color_format != HAL_G2D_COLOR_FORMAT_ARGB8888) {
            return HAL_G2D_STATUS_INVALID_PARAMETER;
        }
    }

    g2d_context.source[layer].color_format = color_format;
    g2d_context.source[layer].address = buffer;
    g2d_context.source[layer].area.width = width;
    g2d_context.source[layer].area.height = height;
    if (g2d_context.source[layer].function != HAL_G2D_OVERLAY_LAYER_FUNCTION_FONT) {
        g2d_context.source[layer].pitch = width * g2d_clrfmt_Bpp[color_format];
    }

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_set_layer_window(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    hal_g2d_area_t area;

    area.x = x;
    area.y = y;
    area.width = width;
    area.height = height;
    if (_hal_g2d_check_area(&area) == false) {
        return HAL_G2D_STATUS_INVALID_PARAMETER;
    }

    g2d_context.source[layer].area.x = area.x;
    g2d_context.source[layer].area.y = area.y;
    g2d_context.source[layer].area.width = area.width;
    g2d_context.source[layer].area.height = area.height;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_set_layer_source_key(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer, uint32_t color_key)
{
    if (g2d_context.source[layer].function != HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER) {
        return HAL_G2D_STATUS_NOT_SUPPORTED;
    }

    g2d_context.source[layer].color = color_key;
    g2d_context.source[layer].enable_color_key = true;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_set_layer_rotation(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer, hal_g2d_rotate_angle_t rotation)
{
    g2d_context.source[layer].rotate = rotation;

    _hal_g2d_coordinate_correction(g2d_context.source[layer].rotate, &g2d_context.source[layer].area);

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_set_layer_rectfill_color(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer, uint32_t color)
{
    if (g2d_context.source[layer].function != HAL_G2D_OVERLAY_LAYER_FUNCTION_RECTFILL) {
        return HAL_G2D_STATUS_NOT_SUPPORTED;
    }

    g2d_context.source[layer].color_format = HAL_G2D_COLOR_FORMAT_ARGB8888;
    g2d_context.source[layer].color = color;

    g2d_context.source[layer].alpha = 0xFF;
    g2d_context.source[layer].enable_alpha = true;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_set_layer_font_format(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer, hal_g2d_font_format_t format)
{
    if (g2d_context.source[layer].function != HAL_G2D_OVERLAY_LAYER_FUNCTION_FONT) {
        return HAL_G2D_STATUS_NOT_SUPPORTED;
    }

    g2d_context.source[layer].font_format = format;
    g2d_context.source[layer].pitch = g2d_context.source[layer].area.width;

    return HAL_G2D_STATUS_OK;
}

hal_g2d_status_t hal_g2d_overlay_set_layer_font_color(hal_g2d_handle_t *handle, hal_g2d_overlay_layer_t layer, uint32_t color)
{
    if (g2d_context.source[layer].function != HAL_G2D_OVERLAY_LAYER_FUNCTION_FONT) {
        return HAL_G2D_STATUS_NOT_SUPPORTED;
    }

    g2d_context.source[layer].color = color;

    return HAL_G2D_STATUS_OK;
}

#endif
