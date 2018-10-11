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

/* Includes ------------------------------------------------------------------*/
#include "display_demo.h"
#include "memory_attribute.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal_cache.h"
#include "hal_mpu.h"
#include "hal_clock.h"
#include "hal_log.h"
#include "bsp_lcd.h"
#include "mt25x3_hdk_lcd.h"
#include "mt25x3_hdk_backlight.h"

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t logo_320X320[320*320*2];
extern uint8_t linkit_320X320[320*320*2];
extern uint8_t logo_240X240[320*320*2];
extern uint8_t linkit_240X240[320*320*2];

#define LCDCHECK_DELAY						( ( portTickType ) 1000 / portTICK_RATE_MS )
 
void display_demo_task(void *param)
{
	portTickType xLastExecutionTime, xDelayTime;
    hal_display_lcd_roi_output_t roi_para;
    hal_display_lcd_layer_input_t layer_para;
    uint32_t width, height;
    lcm_config_para_t lcm_setting;

    LOG_I(hal, "enter display_demo_task\n\r");

	xLastExecutionTime = xTaskGetTickCount();
	xDelayTime = LCDCHECK_DELAY*5;

    /* Initialize LCD hardware and LCM driver */

    bsp_lcd_init(0xF800); /* Show a red screen*/

    /* Turn on maximum backlight in different interface */

    bsp_backlight_init();
    
    /* Configure output roi setting */

    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_WIDTH, &width);
    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_HEIGHT, &height);
    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_SETTING_PARAM, &lcm_setting);
    
    roi_para.target_start_x = 0;
    roi_para.target_start_y = 0;
    roi_para.target_end_x = width-1;
    roi_para.target_end_y = height-1;
    roi_para.roi_offset_x = 0;
    roi_para.roi_offset_y = 0;
    roi_para.main_lcd_output = lcm_setting.main_lcd_output;

    /* Apply roi setting */

    bsp_lcd_config_roi(&roi_para);

    /* Configure input layer setting */

    layer_para.source_key_flag = 0;
    layer_para.alpha_flag = 0;
    layer_para.color_format = HAL_DISPLAY_LCD_LAYER_COLOR_RGB565;
    layer_para.alpha = 0;
    layer_para.rotate = HAL_DISPLAY_LCD_LAYER_ROTATE_0;
    layer_para.row_size = height;
    layer_para.column_size = width;
    layer_para.window_x_offset = 0;
    layer_para.window_y_offset = 0;
    layer_para.layer_enable = HAL_DISPLAY_LCD_LAYER0;
    layer_para.pitch = width*2;

	while(1)
    {
        /* Assign frame buffer */

        if(width == 240){
            layer_para.buffer_address = (uint32_t)&logo_240X240;}
        else{
            layer_para.buffer_address = (uint32_t)&logo_320X320;}

        /* Apply layer setting */

        bsp_lcd_config_layer(&layer_para);

        /* Update screen */

        bsp_lcd_update_screen(roi_para.target_start_x, roi_para.target_start_y, roi_para.target_end_x, roi_para.target_end_y);

		vTaskDelayUntil( &xLastExecutionTime, xDelayTime );
        
        /* Assign frame buffer */

        if(width == 240){
            layer_para.buffer_address = (uint32_t)&linkit_240X240;}
        else{
            layer_para.buffer_address = (uint32_t)&linkit_320X320;}

        /* Apply layer setting */

        bsp_lcd_config_layer(&layer_para);

        /* Update screen */

        bsp_lcd_update_screen(roi_para.target_start_x, roi_para.target_start_y, roi_para.target_end_x, roi_para.target_end_y);

		vTaskDelayUntil( &xLastExecutionTime, xDelayTime );

    }
}

#ifdef __cplusplus
}
#endif


