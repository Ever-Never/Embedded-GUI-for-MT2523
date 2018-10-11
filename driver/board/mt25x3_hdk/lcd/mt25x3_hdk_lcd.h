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

#ifndef __MT25X3_HDK_LCD_H__
#define __MT25X3_HDK_LCD_H__

#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED
#include "bsp_lcd.h"

#ifdef __cplusplus
    extern "C" {
#endif

typedef enum {
    BSP_LCD_STATUS_ERROR = -1,
    BSP_LCD_STATUS_OK = 0
} bsp_lcd_status_t;

typedef void (*bsp_lcd_callback_t)(void *data);

bsp_lcd_status_t bsp_lcd_init(uint16_t bgcolor);
bsp_lcd_status_t bsp_lcd_set_layer_to_default(void);
bsp_lcd_status_t bsp_lcd_display_on(void);
bsp_lcd_status_t bsp_lcd_display_off(void);
bsp_lcd_status_t bsp_lcd_enter_idle(void);
bsp_lcd_status_t bsp_lcd_exit_idle(void);
bsp_lcd_status_t bsp_lcd_update_screen(uint32_t start_x,  uint32_t start_y, uint32_t end_x, uint32_t end_y);
bsp_lcd_status_t bsp_lcd_clear_screen(uint16_t color);
bsp_lcd_status_t bsp_lcd_clear_screen_bw(void);
bsp_lcd_status_t bsp_lcd_get_parameter(LCM_IOCTRL_ID_ENUM ID, void* parameters);
bsp_lcd_status_t bsp_lcd_config_roi(hal_display_lcd_roi_output_t *roi_para);
bsp_lcd_status_t bsp_lcd_config_layer(hal_display_lcd_layer_input_t *layer_data);
bsp_lcd_status_t bsp_lcd_set_index_color_table(uint32_t* index_table);
bsp_lcd_status_t bsp_lcd_register_callback(bsp_lcd_callback_t bsp_lcd_callback);
bsp_lcd_status_t bsp_lcd_power_on(void);
bsp_lcd_status_t bsp_lcd_power_off(void);
bsp_lcd_status_t bsp_lcd_lock(void);
bsp_lcd_status_t bsp_lcd_unlock(void);

#ifdef __cplusplus
}
#endif
#endif

#endif //__MT25X3_HDK_LCD_H__
