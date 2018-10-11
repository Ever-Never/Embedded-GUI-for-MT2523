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

#include "mt25x3_hdk_backlight.h"

#include "hal_gpio.h"
#ifdef HAL_ISINK_MODULE_ENABLED
#include "hal_isink.h"
#endif
#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED
#include "mt25x3_hdk_lcd.h"
extern lcm_config_para_t lcm_setting;
#endif
#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
#include "hal_display_dsi.h"
#endif

#include "hal_gpt.h"
#include "hal_log.h"

#ifdef FREERTOS_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
extern SemaphoreHandle_t lcd_sema;
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern volatile bool isUpdate;
uint8_t prev_brightness;

bsp_backlight_status_t bsp_backlight_init(void)
{
    bsp_backlight_status_t ret = BSP_BACKLIGHT_STATUS_OK;

#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED
#ifdef MTK_EXTERNAL_PMIC
    if (BACKLIGHT_TYPE_ISINK == lcm_setting.backlight_type){
        ret = bsp_backlight_init_external_isink();}
#else
#ifdef HAL_ISINK_MODULE_ENABLED
    if (BACKLIGHT_TYPE_ISINK == lcm_setting.backlight_type){
        ret = bsp_backlight_init_isink(HAL_ISINK_CHANNEL_0, 1, HAL_ISINK_OUTPUT_CURRENT_24_MA);}
#endif
#endif

#ifdef HAL_DISPLAY_PWM_MODULE_ENABLED
    if (BACKLIGHT_TYPE_DISPLAY_PWM == lcm_setting.backlight_type){
        ret = bsp_backlight_init_display_pwm();}
#endif

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
    if (BACKLIGHT_TYPE_LCM_BRIGHTNESS == lcm_setting.backlight_type){
        ret = bsp_backlight_init_lcm_brightness();}
#endif
#endif

    return ret;
}

bsp_backlight_status_t bsp_backlight_deinit(void)
{
    bsp_backlight_status_t ret = BSP_BACKLIGHT_STATUS_OK;

#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED
#ifdef MTK_EXTERNAL_PMIC
    if (BACKLIGHT_TYPE_ISINK == lcm_setting.backlight_type){
        return bsp_backlight_deinit_external_isink();}
#else
#ifdef HAL_ISINK_MODULE_ENABLED
    if (BACKLIGHT_TYPE_ISINK == lcm_setting.backlight_type) {
        if (HAL_ISINK_STATUS_OK != hal_isink_deinit(HAL_ISINK_CHANNEL_0)){
            return BSP_BACKLIGHT_STATUS_ERROR;}
        if (HAL_ISINK_STATUS_OK != hal_isink_deinit(HAL_ISINK_CHANNEL_1)){
            return BSP_BACKLIGHT_STATUS_ERROR;}
    }
#endif
#endif

#ifdef HAL_DISPLAY_PWM_MODULE_ENABLED
    if (BACKLIGHT_TYPE_DISPLAY_PWM == lcm_setting.backlight_type) {
        if (HAL_DISPLAY_PWM_STATUS_OK != hal_display_pwm_deinit()){
            return BSP_BACKLIGHT_STATUS_ERROR;}
    }
#endif

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
    if (BACKLIGHT_TYPE_LCM_BRIGHTNESS == lcm_setting.backlight_type){
        return bsp_backlight_deinit_lcm_brightness();}
#endif
#endif

    return ret;
}

bsp_backlight_status_t bsp_backlight_enable(bool enable)
{
    if (enable) {
        return bsp_backlight_init();
    } else {
        return bsp_backlight_deinit();
    }
}

#ifdef HAL_ISINK_MODULE_ENABLED

bool isDobuleEn = false;

bsp_backlight_status_t bsp_backlight_init_isink(hal_isink_channel_t channel, bool isDouble, hal_isink_current_t current)
{
    bsp_backlight_status_t ret = BSP_BACKLIGHT_STATUS_OK;

    if (HAL_ISINK_STATUS_OK != hal_isink_init(channel)){
        return BSP_BACKLIGHT_STATUS_ERROR;}
    if (HAL_ISINK_STATUS_OK != hal_isink_set_mode(channel, HAL_ISINK_MODE_REGISTER)){
        return BSP_BACKLIGHT_STATUS_ERROR;}
    if (HAL_ISINK_STATUS_OK != hal_isink_set_clock_source(channel, HAL_ISINK_CLOCK_SOURCE_2MHZ)){
        return BSP_BACKLIGHT_STATUS_ERROR;}
    if (isDouble) {
        if (HAL_ISINK_STATUS_OK != hal_isink_set_double_current(channel, current)){
            return BSP_BACKLIGHT_STATUS_ERROR;}
    } else {
        if (HAL_ISINK_STATUS_OK != hal_isink_set_step_current(channel, current)){
            return BSP_BACKLIGHT_STATUS_ERROR;}
    }
    isDobuleEn = isDouble;

    return ret;
}

bsp_backlight_status_t bsp_backlight_set_clock_source_isink(hal_isink_channel_t channel, hal_isink_clock_source_t source_clock)
{
    bsp_backlight_status_t ret = BSP_BACKLIGHT_STATUS_OK;

    if (HAL_ISINK_STATUS_OK != hal_isink_set_clock_source(channel, source_clock)){
        return BSP_BACKLIGHT_STATUS_ERROR;}

    return ret;
}

bsp_backlight_status_t bsp_backlight_set_step_current(hal_isink_channel_t channel, hal_isink_current_t current)
{
    bsp_backlight_status_t ret = BSP_BACKLIGHT_STATUS_OK;

    if (isDobuleEn) {
        if (BSP_BACKLIGHT_STATUS_OK != bsp_backlight_deinit()){
            return BSP_BACKLIGHT_STATUS_ERROR;}
        if (BSP_BACKLIGHT_STATUS_OK != bsp_backlight_init_isink(channel, false, current)){
            return BSP_BACKLIGHT_STATUS_ERROR;}
    } else {
        if (HAL_ISINK_STATUS_OK != hal_isink_set_step_current(channel, current)){
            return BSP_BACKLIGHT_STATUS_ERROR;}
    }

    return ret;
}

bsp_backlight_status_t bsp_backlight_set_double_current_isink(hal_isink_channel_t channel, hal_isink_current_t current)
{
    bsp_backlight_status_t ret = BSP_BACKLIGHT_STATUS_OK;

    if (!isDobuleEn) {
        if (BSP_BACKLIGHT_STATUS_OK != bsp_backlight_deinit()){
            return BSP_BACKLIGHT_STATUS_ERROR;}
        if (BSP_BACKLIGHT_STATUS_OK != bsp_backlight_init_isink(channel, true, current)){
            return BSP_BACKLIGHT_STATUS_ERROR;}
    } else {
        if (HAL_ISINK_STATUS_OK != hal_isink_set_double_current(channel, current)){
            return BSP_BACKLIGHT_STATUS_ERROR;}
    }

    return ret;
}
#endif

#ifdef HAL_DISPLAY_PWM_MODULE_ENABLED
bsp_backlight_status_t bsp_backlight_init_display_pwm(void)
{
    bsp_backlight_status_t ret = BSP_BACKLIGHT_STATUS_OK;

    if(HAL_DISPLAY_PWM_STATUS_OK != hal_display_pwm_init(HAL_DISPLAY_PWM_CLOCK_26MHZ)){
        return BSP_BACKLIGHT_STATUS_ERROR;}
    if(HAL_DISPLAY_PWM_STATUS_OK != hal_display_pwm_set_duty(80)){
        return BSP_BACKLIGHT_STATUS_ERROR;}

    return ret;
}

bsp_backlight_status_t bsp_backlight_set_clock_source_display_pwm(hal_display_pwm_clock_t source_clock)
{
    bsp_backlight_status_t ret = BSP_BACKLIGHT_STATUS_OK;

    if(HAL_DISPLAY_PWM_STATUS_OK != hal_display_pwm_set_clock(source_clock)){
        return BSP_BACKLIGHT_STATUS_ERROR;}

    return ret;
}

bsp_backlight_status_t bsp_backlight_set_duty_display_pwm(uint8_t percent)
{
    bsp_backlight_status_t ret = BSP_BACKLIGHT_STATUS_OK;

    if(HAL_DISPLAY_PWM_STATUS_OK != hal_display_pwm_set_duty(percent)){
        return BSP_BACKLIGHT_STATUS_ERROR;}

    return ret;
}

bsp_backlight_status_t bsp_backlight_set_width_display_pwm(uint32_t width)
{
    bsp_backlight_status_t ret = BSP_BACKLIGHT_STATUS_OK;

    if(HAL_DISPLAY_PWM_STATUS_OK != hal_display_pwm_set_width(width)){
        return BSP_BACKLIGHT_STATUS_ERROR;}

    return ret;
}

#endif

bsp_backlight_status_t bsp_backlight_init_lcm_brightness(void)
{
#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
    return bsp_backlight_set_step_lcm_brightness(255);
#else
    return BSP_BACKLIGHT_STATUS_OK;
#endif
}

bsp_backlight_status_t bsp_backlight_deinit_lcm_brightness(void)
{
#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
    return bsp_backlight_set_step_lcm_brightness(30);
#else
    return BSP_BACKLIGHT_STATUS_OK;
#endif
}

bsp_backlight_status_t bsp_backlight_set_step_lcm_brightness(uint8_t level)
{
    bsp_backlight_status_t ret = BSP_BACKLIGHT_STATUS_OK;

if(level == prev_brightness)
    return BSP_BACKLIGHT_STATUS_OK;

#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED
    bsp_lcd_lock();

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
    uint32_t data_array[16];

    prev_brightness = level;

    if (LCM_INTERFACE_TYPE_DSI == lcm_setting.type) {
        bsp_lcd_power_on();
        data_array[0] = 0x00023902;
        data_array[1] = 0x51 | (level << 8);
        hal_display_dsi_set_command_queue(data_array, 2, true);
        bsp_lcd_power_off();
    }
#endif

    bsp_lcd_unlock();
#endif
    return ret;
}

#ifdef MTK_EXTERNAL_PMIC
bsp_backlight_status_t bsp_backlight_init_external_isink(void)
{
    bsp_backlight_status_t ret = BSP_BACKLIGHT_STATUS_OK;

    hal_pinmux_set_function(HAL_GPIO_44, 0); // ISINK EN
    hal_gpio_set_direction(HAL_GPIO_44, HAL_GPIO_DIRECTION_OUTPUT);

    hal_gpio_set_output(HAL_GPIO_44, HAL_GPIO_DATA_LOW);
    hal_gpio_set_output(HAL_GPIO_44, HAL_GPIO_DATA_HIGH);

    return ret;
}
bsp_backlight_status_t bsp_backlight_deinit_external_isink(void)
{
    bsp_backlight_status_t ret = BSP_BACKLIGHT_STATUS_OK;

    hal_pinmux_set_function(HAL_GPIO_44, 0); // ISINK EN
    hal_gpio_set_direction(HAL_GPIO_44, HAL_GPIO_DIRECTION_OUTPUT);

    hal_gpio_set_output(HAL_GPIO_44, HAL_GPIO_DATA_LOW);

    return ret;
}
#endif

#ifdef __cplusplus
}
#endif

