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

#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED

#include "hal_clock.h"
#include "hal_display_lcd.h"
#include "hal_display_dsi_internal.h"
#include "hal_mipi_tx_config_internal.h"
#include "hal_nvic_internal.h"
#include "hal_nvic.h"
#ifdef __cplusplus
extern "C" {
#endif

extern hal_display_lcd_callback_t display_cb;
extern volatile LCD_DSI_REGISTER_T *lcd_dsi_register_ptr;

static void display_dsi_isr(void)
{

    if (lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.field.FRAME_DONE_INT_FLAG == 1) {
        lcd_dsi_register_ptr->LCD_DSI_INTSTA_REGISTER.field.FRAME_DONE_INT_FLAG = 0;
        lcd_dsi_register_ptr->LCD_DSI_INTEN_REGISTER.value = 0;
        hal_display_dsi_set_transfer_mode(HAL_DISPLAY_DSI_TRANSFER_MODE_LP);
        if (display_cb != NULL) {
            display_cb(NULL);
        }
    }
}

hal_display_dsi_status_t hal_display_dsi_init(bool ssc_enable)
{
    hal_display_dsi_status_t ret = HAL_DISPLAY_DSI_STATUS_OK;

    ret = hal_display_dsi_power_on();
    ret = display_dsi_init(ssc_enable);

    hal_nvic_register_isr_handler(DSI_IRQn, (hal_nvic_isr_t)display_dsi_isr);
    hal_nvic_set_priority(DSI_IRQn, DSI_IRQ_PRIORITY);
    hal_nvic_enable_irq(DSI_IRQn);

    return ret;
}

hal_display_dsi_status_t hal_display_dsi_deinit(void)
{
    hal_display_dsi_status_t ret = HAL_DISPLAY_DSI_STATUS_OK;

    hal_nvic_disable_irq(DSI_IRQn);
    display_dsi_deinit();
    hal_display_dsi_power_off();

    return ret;
}

hal_display_dsi_status_t hal_display_dsi_power_on(void)
{
    hal_display_dsi_status_t ret = HAL_DISPLAY_DSI_STATUS_OK;
    hal_clock_status_t clock_ret;

    clock_ret = hal_clock_enable(HAL_CLOCK_CG_DSI0);
    if (clock_ret != HAL_CLOCK_STATUS_OK) {
        ret = HAL_DISPLAY_DSI_STATUS_POWER_FAILED;
    }

    return ret;
}

hal_display_dsi_status_t hal_display_dsi_power_off(void)
{
    hal_display_dsi_status_t ret = HAL_DISPLAY_DSI_STATUS_OK;
    hal_clock_status_t clock_ret;

    clock_ret = hal_clock_disable(HAL_CLOCK_CG_DSI0);

    if (clock_ret != HAL_CLOCK_STATUS_OK) {
        ret = HAL_DISPLAY_DSI_STATUS_POWER_FAILED;
    }

    return ret;
}

hal_display_dsi_status_t hal_display_dsi_enable_te(void)
{
    hal_display_dsi_status_t ret = HAL_DISPLAY_DSI_STATUS_OK;

    return ret;
}

hal_display_dsi_status_t hal_display_dsi_set_command_queue(uint32_t *pData, uint8_t size, bool force_update)
{
    hal_display_dsi_status_t ret = HAL_DISPLAY_DSI_STATUS_OK;

    display_dsi_set_command_queue(pData, size, force_update);

    return ret;
}

hal_display_dsi_status_t hal_display_dsi_read_register(uint8_t cmd, uint8_t *buffer, uint8_t size)
{
    hal_display_dsi_status_t ret = HAL_DISPLAY_DSI_STATUS_OK;

    ret = display_dsi_read_register(cmd, buffer, size);

    return ret;
}

hal_display_dsi_status_t hal_display_dsi_enter_ulps(void)
{
    hal_display_dsi_status_t ret = HAL_DISPLAY_DSI_STATUS_OK;

    display_dsi_enter_ulps();

    return ret;
}

hal_display_dsi_status_t hal_display_dsi_exit_ulps(void)
{
    hal_display_dsi_status_t ret = HAL_DISPLAY_DSI_STATUS_OK;

    display_dsi_exit_ulps();

    return ret;
}

hal_display_dsi_status_t hal_display_dsi_set_dphy_timing(hal_display_dsi_dphy_timing_struct_t *timing)
{
    hal_display_dsi_status_t ret = HAL_DISPLAY_DSI_STATUS_OK;

    display_dsi_set_dphy_timing(timing);

    return ret;
}

hal_display_dsi_status_t hal_display_dsi_set_clock(uint32_t pll_clock, bool ssc_enable)
{
    hal_display_dsi_status_t ret = HAL_DISPLAY_DSI_STATUS_OK;

    display_dsi_set_clock(pll_clock, ssc_enable);

    return ret;
}

hal_display_dsi_status_t hal_display_dsi_set_transfer_mode(hal_display_dsi_transfer_mode_t mode)
{
    hal_display_dsi_status_t ret = HAL_DISPLAY_DSI_STATUS_OK;

    if (mode >= HAL_DISPLAY_DSI_TRANSFER_MODE_NUM) {
        return HAL_DISPLAY_DSI_STATUS_INVALID_PARAMETER;
    }

    ret = display_dsi_set_transfer_mode(mode);

    return ret;
}

void hal_dsiplay_dsi_start_bta_transfer(uint32_t cmd)
{
    hal_display_dsi_start_bta_transfer(cmd);
}

void hal_display_dsi_start_bta_transfer(uint32_t cmd)
{
    display_dsi_start_bta_transfer(cmd);
}

void hal_display_dsi_restore_callback(void)
{
    hal_display_dsi_init(false);
    display_dsi_restore_dsi_setting();
}

#ifdef __cplusplus
}
#endif

#endif // HAL_DISPLAY_DSI_MODULE_ENABLED
