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

#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED

#include "hal_display_lcd_internal.h"
#include "hal_display_color.h"
#include "hal_display_color_internal.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "hal_cm4_topsm.h"
#include "hal_nvic_internal.h"
#include "hal_nvic.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#endif
#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED
#include "hal_display_dsi.h"
#include "hal_display_dsi_internal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t main_command_address;
    uint32_t main_data_address;
    uint32_t main_lcd_output_format;
} hal_display_lcd_backup_t;

hal_display_lcd_backup_t lcd_config;
hal_display_lcd_callback_t display_cb = NULL;
static uint32_t display_mtcoms_count = 0;
extern LCD_REGISTER_T *lcd_register_ptr;
#ifdef HAL_SLEEP_MANAGER_ENABLED
static uint8_t lcd_sleep_handle = 0xFF;
const char* lcd_name = "LCD";
#endif

static void display_lcd_isr(void)
{
    LCD_INTSTA_REGISTER_T int_status;

    int_status = lcd_register_ptr->lcd_intsta_register;

    if(int_status.field.CPL == 1) {
        if (display_cb != NULL) {
            display_cb(NULL);
        }
    }
}

hal_display_lcd_status_t hal_display_lcd_init(uint32_t main_command_address, uint32_t main_data_address, uint32_t main_lcd_output_format)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    // Set clock source first
    //hal_lcd_set_clock(LCD_SERIAL_CLOCK_124MHZ);

    lcd_config.main_command_address = main_command_address;
    lcd_config.main_data_address = main_data_address;
    lcd_config.main_lcd_output_format = main_lcd_output_format;

    ret = display_lcd_init(main_command_address, main_data_address, main_lcd_output_format);
    display_lcd_init_para();
    hal_display_color_init();

    hal_nvic_register_isr_handler(LCD_IRQn, (hal_nvic_isr_t)display_lcd_isr);
    hal_nvic_set_priority(LCD_IRQn, LCD_IRQ_PRIORITY);
    hal_nvic_enable_irq(LCD_IRQn);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    if (lcd_sleep_handle == 0xFF) {
        lcd_sleep_handle = hal_sleep_manager_set_sleep_handle(lcd_name);
    }
#endif

    return ret;
}

hal_display_lcd_status_t hal_display_lcd_set_interface_timing(hal_display_lcd_interface_timing_t para)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    if ((para.port_number > 1) || (para.css > 0xF) || (para.csh > 0xF) || (para.rd_low > 0xF) || (para.rd_high > 0xF) || (para.wr_low > 0xF) || (para.wr_high > 0xF)) {
        ret = HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER;
        return ret;
    }

    display_lcd_set_interface_timing(para);

    return ret;
}

hal_display_lcd_status_t hal_display_lcd_set_interface_mode(hal_display_lcd_interface_mode_t para)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    if ((para.port_number > 1) || (para.cs_stay_low_mode > 1) || (para.single_a0_mode > 1) || (para.read_from_SDI > 1) || (para.width >= HAL_DISPLAY_LCD_INTERFACE_WIDTH_NUM) || (para.hw_cs > 1)) {
        ret = HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER;
        return ret;
    }

    display_lcd_set_interface_mode(para);

    return ret;
}

hal_display_lcd_status_t hal_display_lcd_set_interface_2data_lane_mode(hal_display_lcd_interface_port_t port, bool flag, hal_display_lcd_interface_2pin_width_t two_data_width)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    if ((port > 1) || (two_data_width >= HAL_DISPLAY_LCD_INTERFACE_2PIN_WIDTH_NUM)) {
        ret = HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER;
        return ret;
    }

    display_lcd_set_interface_2data_lane_mode(port, flag, two_data_width);

    return ret;
}

hal_display_lcd_status_t hal_display_lcd_config_layer(hal_display_lcd_layer_input_t *layer_data)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    display_lcd_config_layer(layer_data);

    return ret;
}

hal_display_lcd_status_t hal_display_lcd_config_roi(hal_display_lcd_roi_output_t *roi_para)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    roi_para->main_lcd_output = lcd_config.main_lcd_output_format;

    display_lcd_config_roi(roi_para);

    return ret;
}

hal_display_lcd_status_t hal_display_lcd_config_start_byte(hal_display_lcd_interface_start_byte_mode_t *start_byte_para)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    display_lcd_config_start_byte(start_byte_para);

    return ret;
}

hal_display_lcd_status_t hal_display_lcd_set_index_color_table(uint32_t *index_table)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    display_lcd_set_index_color_table(index_table);

    return ret;
}

hal_display_lcd_status_t hal_display_lcd_init_te(uint32_t frame_rate, uint32_t back_porch, uint32_t front_porch, uint32_t width, uint32_t height, uint32_t main_lcd_output)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    display_lcd_init_te(frame_rate, back_porch, front_porch, width, height, main_lcd_output);

    return ret;
}

hal_display_lcd_status_t hal_display_lcd_calculate_te(uint32_t lcm_width, uint32_t lcm_height)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    display_lcd_calculate_te(lcm_width, lcm_height);

    return ret;
}

hal_display_lcd_status_t hal_display_lcd_toggle_reset(uint32_t low_time, uint32_t high_time)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    display_lcd_toggle_reset(low_time, high_time);

    return ret;
}

hal_display_lcd_status_t hal_display_lcd_start_dma(bool wait_te)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    display_lcd_start_dma(wait_te);

    return ret;
}

uint32_t hal_display_lcd_get_layer_address(hal_display_lcd_layer_t layer)
{
    return display_lcd_get_layer_address(layer);
}

void hal_display_lcd_restore_callback(void)
{
    display_lcd_init(lcd_config.main_command_address, lcd_config.main_data_address, lcd_config.main_lcd_output_format);
    display_lcd_restore_if_settings();
    display_lcd_restore_layer_settings();
}

hal_display_lcd_status_t hal_display_lcd_turn_on_mtcmos(void)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    if (display_mtcoms_count == 0) {
#ifdef HAL_SLEEP_MANAGER_ENABLED
        hal_sleep_manager_lock_sleep(lcd_sleep_handle);
#endif
        cm4_topsm_lock_MM_MTCMOS();
        display_lcd_power_on();
        display_color_power_on();
        hal_display_lcd_restore_callback();
    }

    display_mtcoms_count++;

    return ret;
}

hal_display_lcd_status_t hal_display_lcd_turn_off_mtcmos(void)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    display_mtcoms_count--;

    if (display_mtcoms_count == 0) {
        display_lcd_backup_register();
        display_lcd_power_off();
        display_color_power_off();
        cm4_topsm_unlock_MM_MTCMOS();
#ifdef HAL_SLEEP_MANAGER_ENABLED
        hal_sleep_manager_unlock_sleep(lcd_sleep_handle);
#endif
    }

    return ret;
}

hal_display_lcd_status_t hal_display_lcd_set_clock(hal_display_lcd_interface_clock_t clock)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    switch (clock) {
        case HAL_DISPLAY_LCD_INTERFACE_CLOCK_124MHZ:
            clock_mux_sel(SLCD_MUX_SEL, 5);
            break;
        case HAL_DISPLAY_LCD_INTERFACE_CLOCK_104MHZ:
            clock_mux_sel(SLCD_MUX_SEL, 6);
            break;
        case HAL_DISPLAY_LCD_INTERFACE_CLOCK_26MHZ:
            clock_mux_sel(SLCD_MUX_SEL, 1);
            break;
        default:
            ret = HAL_DISPLAY_LCD_STATUS_INVALID_PARAMETER;
            break;
    }

    return ret;
}

hal_display_lcd_status_t hal_display_lcd_apply_setting()
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    display_lcd_apply_setting();
    return ret;
}


hal_display_lcd_status_t hal_display_lcd_set_layer_to_default(void)
{
    hal_display_lcd_status_t ret = HAL_DISPLAY_LCD_STATUS_OK;

    display_lcd_set_layer_to_default();
    return ret;
}

hal_display_lcd_callback_t hal_display_lcd_register_callback(hal_display_lcd_callback_t lcd_callback)
{
    display_cb = lcd_callback;

    return NULL;
}

#ifdef __cplusplus
}
#endif

#endif
