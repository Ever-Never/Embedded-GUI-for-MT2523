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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "gdi.h"
#include "gdi_font_engine.h"
#include "custom_resource_def.h"
#include "main_screen.h"

#include "memory_attribute.h"
#include "mt25x3_hdk_lcd.h"
#include "bsp_lcd.h"
#include "mt25x3_hdk_backlight.h"

#define DEFAULT_LCD_W 320
#define DEFAULT_LCD_H 320
 
static uint32_t LCD_WIDTH = 320;
static uint32_t LCD_HEIGHT = 320;

ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char layer_buffer[DEFAULT_LCD_W*DEFAULT_LCD_H*2];


void main_screen_init(){ 
    bsp_lcd_init(0xF800);
    bsp_backlight_init();
    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_HEIGHT, &LCD_WIDTH); // Get the LCD size.
    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_WIDTH, &LCD_HEIGHT);
    gdi_init(LCD_WIDTH, LCD_HEIGHT, GDI_COLOR_FORMAT_16, layer_buffer);  // Initialize the GDI buffer.
}

void common_event_handler(message_id_enum event_id, int32_t param1, void* param2)
{

}

uint8_t* convert_string_to_wstring(uint8_t* string)
{
    static uint8_t wstring[50];
    int32_t index = 0;
    if (!string) {
        return NULL;
    }
    while (*string) {
        wstring[index] = *string;
        wstring[index + 1] = 0;
        string++;
        index+=2;
    }
    return wstring;
}

void main_screen_show(){
    gdi_font_engine_display_string_info_t param;
    
     // Set the font size and color.
    gdi_font_engine_color_t font_color = {0, 255, 255, 255};
    gdi_font_engine_set_font_size(GDI_FONT_ENGINE_FONT_MEDIUM);
    gdi_font_engine_set_text_color(font_color);

    // Set the screen background color.
    gdi_draw_filled_rectangle(0,0,LCD_WIDTH,LCD_HEIGHT,gdi_get_color_from_argb(0, 0, 0, 0));

    // Draw an image (source ID IMAGE_ID_BT_DISCONNECT_BMP) at position of (20, 140).
    gdi_image_draw_by_id(20,140,IMAGE_ID_BT_DISCONNECT_BMP);

    // Draw the string "Hello World!" at position of (100, 150).
    param.x = 100;
    param.y = 150;
    param.string = convert_string_to_wstring((uint8_t*) "Hello World!");
    param.length = strlen("Hello World!");
    param.baseline_height = -1;
    gdi_font_engine_display_string(&param);

    // Update the screen.
    gdi_lcd_update_screen(0, 0, LCD_WIDTH, LCD_HEIGHT);
}












