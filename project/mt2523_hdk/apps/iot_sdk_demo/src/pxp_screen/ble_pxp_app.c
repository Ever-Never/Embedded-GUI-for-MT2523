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

/*****************************************************************************
 *
 * Description:
 * ------------
 * This file implements PXP app main function
 *
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include "main_screen.h"
#include "syslog.h"
#include "bt_init.h"
#include "ble_gatt_utils.h"
#include "ble_pxp_app.h"
#include "ble_pxp_main.h"
#include <math.h>
#include "mt25x3_hdk_lcd.h"
#include "bt_sink_app_event.h"
#include "gdi_font_engine.h"
#include "gdi.h"

#define PXP_LEFT_GAP 40
#define PXP_ITEM_HEIGHT 28


ble_pxp_screen_cntx g_ble_pxp_screen_cntx;

uint8_t lcd_size = 0;

static int32_t bt_demo_get_convert_coordinate(uint8_t lcd_size, int32_t pos);

static void font_engine_get_string_width_height(uint8_t *string, int32_t len,
    int32_t *width, int32_t *height);

uint8_t* ble_pxp_convert_string_to_wstring(uint8_t* string)
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


void ble_pxp_pen_event_handler(touch_event_struct_t* pen_event, void* user_data)
{
    static int32_t back_hit;
    if (pen_event->position.x <= g_ble_pxp_screen_cntx.back_x2 && pen_event->position.x >= g_ble_pxp_screen_cntx.back_x1) {
        if (pen_event->position.y <= g_ble_pxp_screen_cntx.back_y2 && pen_event->position.y >= g_ble_pxp_screen_cntx.back_y1) {
            if (pen_event->type == TOUCH_EVENT_DOWN) {
                back_hit = 1;
            } else if (back_hit == 1 && pen_event->type == TOUCH_EVENT_UP) {
                back_hit = 0;
                memset(&g_ble_pxp_screen_cntx, 0, sizeof(ble_pxp_screen_cntx));

                ble_pxp_deinit();

                bt_demo_power_off();

                show_main_screen();
            }
        }
    } else {
        back_hit = 0;
    }
}


void ble_pxp_screen_event_handler(message_id_enum event_id, int32_t param1, void* param2)
{

}


void ble_pxp_show_main_screen(void)
{
    int32_t string_width, string_height;
    uint32_t rect_x1 = 0, rect_y1 = 0, rect_x2 = 0, rect_y2 = 0;
    gdi_font_engine_color_t text_color = {0, 255, 255, 255};//white color

    demo_ui_register_touch_event_callback(ble_pxp_pen_event_handler, NULL);

    memset(&g_ble_pxp_screen_cntx, 0, sizeof(ble_pxp_screen_cntx));

    if (lcd_size == 0) {
        g_ble_pxp_screen_cntx.bg_color = 0;
        g_ble_pxp_screen_cntx.font_color = text_color;
        g_ble_pxp_screen_cntx.font = GDI_FONT_ENGINE_FONT_MEDIUM;
        g_ble_pxp_screen_cntx.title_x = 5;
        g_ble_pxp_screen_cntx.title_y = 6;
        g_ble_pxp_screen_cntx.content_x = PXP_LEFT_GAP;

        g_ble_pxp_screen_cntx.back_x2 = 239;
        g_ble_pxp_screen_cntx.back_y2 = 239;
        g_ble_pxp_screen_cntx.back_x1 = 200;
        g_ble_pxp_screen_cntx.back_y1 = g_ble_pxp_screen_cntx.back_y2 - PXP_ITEM_HEIGHT;
        //g_ble_pxp_screen_cntx.content_y = (g_ble_pxp_screen_cntx.back_y2 - g_ble_pxp_screen_cntx.title_y - PXP_ITEM_HEIGHT)/2;
        g_ble_pxp_screen_cntx.content_y = (240)/2;

        gdi_draw_filled_rectangle(0,0,239,239, g_ble_pxp_screen_cntx.bg_color);
        rect_x1 = 0;
        rect_y1 = 0;
        rect_x2 = 239;
        rect_y2 = 239;
    } else if (lcd_size == 1) {
        g_ble_pxp_screen_cntx.bg_color = 0;
        g_ble_pxp_screen_cntx.font_color = text_color;
        g_ble_pxp_screen_cntx.font = GDI_FONT_ENGINE_FONT_MEDIUM;
        g_ble_pxp_screen_cntx.title_x = bt_demo_get_convert_coordinate(lcd_size, 5);
        g_ble_pxp_screen_cntx.title_y = bt_demo_get_convert_coordinate(lcd_size, 6);
        g_ble_pxp_screen_cntx.content_x = bt_demo_get_convert_coordinate(lcd_size, PXP_LEFT_GAP);

        g_ble_pxp_screen_cntx.back_x2 = bt_demo_get_convert_coordinate(lcd_size, 239);
        g_ble_pxp_screen_cntx.back_y2 = bt_demo_get_convert_coordinate(lcd_size, 239);
        g_ble_pxp_screen_cntx.back_x1 = bt_demo_get_convert_coordinate(lcd_size, 200);
        g_ble_pxp_screen_cntx.back_y1 = g_ble_pxp_screen_cntx.back_y2 - PXP_ITEM_HEIGHT;
        g_ble_pxp_screen_cntx.content_y = 320/2;
        gdi_draw_filled_rectangle(0, 0, bt_demo_get_convert_coordinate(lcd_size, 239), bt_demo_get_convert_coordinate(lcd_size, 239), g_ble_pxp_screen_cntx.bg_color);
        rect_x1 = 0;
        rect_y1 = 0;
        rect_x2 = bt_demo_get_convert_coordinate(lcd_size, 239);
        rect_y2 = bt_demo_get_convert_coordinate(lcd_size, 239);
    } else {
        g_ble_pxp_screen_cntx.bg_color = 0;
        g_ble_pxp_screen_cntx.font_color = text_color;
        g_ble_pxp_screen_cntx.font = GDI_FONT_ENGINE_FONT_MEDIUM;
        g_ble_pxp_screen_cntx.title_x = 5;
        g_ble_pxp_screen_cntx.title_y = 6;
        g_ble_pxp_screen_cntx.content_x = PXP_LEFT_GAP;
        
        g_ble_pxp_screen_cntx.back_x2 = 239;
        g_ble_pxp_screen_cntx.back_y2 = 239;
        g_ble_pxp_screen_cntx.back_x1 = 200;
        g_ble_pxp_screen_cntx.back_y1 = g_ble_pxp_screen_cntx.back_y2 - PXP_ITEM_HEIGHT;
        //g_ble_pxp_screen_cntx.content_y = (g_ble_pxp_screen_cntx.back_y2 - g_ble_pxp_screen_cntx.title_y - PXP_ITEM_HEIGHT)/2;
        g_ble_pxp_screen_cntx.content_y = (240)/2;
        gdi_draw_filled_rectangle(0,0,239,239, g_ble_pxp_screen_cntx.bg_color);
        rect_x1 = 0;
        rect_y1 = 0;
        rect_x2 = 239;
        rect_y2 = 239;
    }

    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"BLE PXP"), 
                                strlen("BLE PXP"),
                                &string_width,
                                &string_height);
    ble_pxp_font_engine_show_string(g_ble_pxp_screen_cntx.title_x, 
                            g_ble_pxp_screen_cntx.title_y, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"BLE PXP"), 
                            strlen("BLE PXP"), 
                            g_ble_pxp_screen_cntx.font_color, GDI_FONT_ENGINE_FONT_MEDIUM);

    /**< determine what to display by "disconencted or connected or out of range". */
    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"Unconnected"), 
                                strlen("Unconnected"),
                                &string_width,
                                &string_height);
    ble_pxp_font_engine_show_string(g_ble_pxp_screen_cntx.content_x, 
                            g_ble_pxp_screen_cntx.content_y, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"Unconnected"), 
                            strlen("Unconnected"), 
                            g_ble_pxp_screen_cntx.font_color, GDI_FONT_ENGINE_FONT_MEDIUM);
 
    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"Exit"), 
                                strlen("Exit"),
                                &string_width,
                                &string_height);
    
    //g_ble_pxp_screen_cntx.back_x1 = g_ble_pxp_screen_cntx.back_x2 - string_width;
    //g_ble_pxp_screen_cntx.back_x1 = g_ble_pxp_screen_cntx.back_x2 - 15;
    ble_pxp_font_engine_show_string(g_ble_pxp_screen_cntx.back_x1, 
                            g_ble_pxp_screen_cntx.back_y1, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"Exit"), 
                            strlen("Exit"), 
                            g_ble_pxp_screen_cntx.font_color, GDI_FONT_ENGINE_FONT_MEDIUM);

    gdi_lcd_update_screen(rect_x1, rect_y1, rect_x2, rect_y2);
}


void ble_pxp_show_connected_screen()
{
    int32_t string_width, string_height;
    uint32_t rect_x1 = 0, rect_y1 = 0, rect_x2 = 0, rect_y2 = 0;
    gdi_font_engine_color_t text_color = {0, 255, 255, 255};//white color

    demo_ui_register_touch_event_callback(ble_pxp_pen_event_handler, NULL);

    memset(&g_ble_pxp_screen_cntx, 0, sizeof(ble_pxp_screen_cntx));
    if (lcd_size == 0) {
        g_ble_pxp_screen_cntx.bg_color = 0;
        g_ble_pxp_screen_cntx.font_color = text_color;
        g_ble_pxp_screen_cntx.font = GDI_FONT_ENGINE_FONT_MEDIUM;
        g_ble_pxp_screen_cntx.title_x = 5;
        g_ble_pxp_screen_cntx.title_y = 6;
        g_ble_pxp_screen_cntx.content_x = PXP_LEFT_GAP;

        g_ble_pxp_screen_cntx.back_x2 = 239;
        g_ble_pxp_screen_cntx.back_y2 = 239;
        g_ble_pxp_screen_cntx.back_y1 = g_ble_pxp_screen_cntx.back_y2 - PXP_ITEM_HEIGHT;
        g_ble_pxp_screen_cntx.back_x1 = 200;
        //g_ble_pxp_screen_cntx.content_y = (g_ble_pxp_screen_cntx.back_y2 - g_ble_pxp_screen_cntx.title_y - PXP_ITEM_HEIGHT)/2;
        g_ble_pxp_screen_cntx.content_y = (240)/2;

        gdi_draw_filled_rectangle(0,0,239,239, g_ble_pxp_screen_cntx.bg_color);
        rect_x1 = 0;
        rect_y1 = 0;
        rect_x2 = 239;
        rect_y2 = 239;
    } else if (lcd_size == 1) {
        g_ble_pxp_screen_cntx.bg_color = 0;
        g_ble_pxp_screen_cntx.font_color = text_color;
        g_ble_pxp_screen_cntx.font = GDI_FONT_ENGINE_FONT_MEDIUM;
        g_ble_pxp_screen_cntx.title_x = bt_demo_get_convert_coordinate(lcd_size, 5);
        g_ble_pxp_screen_cntx.title_y = bt_demo_get_convert_coordinate(lcd_size, 6);
        g_ble_pxp_screen_cntx.content_x = bt_demo_get_convert_coordinate(lcd_size, PXP_LEFT_GAP);
        
        g_ble_pxp_screen_cntx.back_x2 = bt_demo_get_convert_coordinate(lcd_size, 239);
        g_ble_pxp_screen_cntx.back_y2 = bt_demo_get_convert_coordinate(lcd_size, 239);
        g_ble_pxp_screen_cntx.back_x1 = bt_demo_get_convert_coordinate(lcd_size, 200);
        g_ble_pxp_screen_cntx.back_y1 = g_ble_pxp_screen_cntx.back_y2 - PXP_ITEM_HEIGHT;
        g_ble_pxp_screen_cntx.content_y = 320/2;

        gdi_draw_filled_rectangle(0, 0, bt_demo_get_convert_coordinate(lcd_size, 239), bt_demo_get_convert_coordinate(lcd_size, 239), g_ble_pxp_screen_cntx.bg_color);
        rect_x1 = 0;
        rect_y1 = 0;
        rect_x2 = bt_demo_get_convert_coordinate(lcd_size, 239);
        rect_y2 = bt_demo_get_convert_coordinate(lcd_size, 239);
    } else {
        g_ble_pxp_screen_cntx.bg_color = 0;
        g_ble_pxp_screen_cntx.font_color = text_color;
        g_ble_pxp_screen_cntx.font = GDI_FONT_ENGINE_FONT_MEDIUM;
        g_ble_pxp_screen_cntx.title_x = 5;
        g_ble_pxp_screen_cntx.title_y = 6;
        g_ble_pxp_screen_cntx.content_x = PXP_LEFT_GAP;
        
        g_ble_pxp_screen_cntx.back_x2 = 239;
        g_ble_pxp_screen_cntx.back_y2 = 239;
        g_ble_pxp_screen_cntx.back_y1 = g_ble_pxp_screen_cntx.back_y2 - PXP_ITEM_HEIGHT;
        g_ble_pxp_screen_cntx.back_x1 = 200;
        //g_ble_pxp_screen_cntx.content_y = (g_ble_pxp_screen_cntx.back_y2 - g_ble_pxp_screen_cntx.title_y - PXP_ITEM_HEIGHT)/2;
        g_ble_pxp_screen_cntx.content_y = (240)/2;

        gdi_draw_filled_rectangle(0,0,239,239, g_ble_pxp_screen_cntx.bg_color);
        rect_x1 = 0;
        rect_y1 = 0;
        rect_x2 = 239;
        rect_y2 = 239;
    }

    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"BLE PXP"), 
                                strlen("BLE PXP"),
                                &string_width,
                                &string_height);
    ble_pxp_font_engine_show_string(g_ble_pxp_screen_cntx.title_x, 
                            g_ble_pxp_screen_cntx.title_y, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"BLE PXP"), 
                            strlen("BLE PXP"), 
                            g_ble_pxp_screen_cntx.font_color, GDI_FONT_ENGINE_FONT_MEDIUM);

    /**determine what to display by "disconencted or connected or out of range"*/
    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"Connected"), 
                                strlen("Connected"),
                                &string_width,
                                &string_height);
    ble_pxp_font_engine_show_string(g_ble_pxp_screen_cntx.content_x, 
                            g_ble_pxp_screen_cntx.content_y, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"Connected"), 
                            strlen("Connected"), 
                            g_ble_pxp_screen_cntx.font_color, GDI_FONT_ENGINE_FONT_MEDIUM);
 

    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"Exit"), 
                                strlen("Exit"),
                                &string_width,
                                &string_height);
    
    //g_ble_pxp_screen_cntx.back_x1 = g_ble_pxp_screen_cntx.back_x2 - string_width;
    //g_ble_pxp_screen_cntx.back_x1 = g_ble_pxp_screen_cntx.back_x2 - 15;
    ble_pxp_font_engine_show_string(g_ble_pxp_screen_cntx.back_x1, 
                            g_ble_pxp_screen_cntx.back_y1, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"Exit"), 
                            strlen("Exit"), 
                            g_ble_pxp_screen_cntx.font_color, GDI_FONT_ENGINE_FONT_MEDIUM);

    gdi_lcd_update_screen(rect_x1, rect_y1, rect_x2, rect_y2);
}

void ble_pxp_show_range_alert_screen()
{
    int32_t string_width, string_height;
    uint32_t rect_x1 = 0, rect_y1 = 0, rect_x2 = 0, rect_y2 = 0;
    gdi_font_engine_color_t text_color = {0, 255, 255, 255};//white color

    demo_ui_register_touch_event_callback(ble_pxp_pen_event_handler, NULL);

    memset(&g_ble_pxp_screen_cntx, 0, sizeof(ble_pxp_screen_cntx));
    if (lcd_size == 0) {
        g_ble_pxp_screen_cntx.bg_color = 0;
        g_ble_pxp_screen_cntx.font_color = text_color;
        g_ble_pxp_screen_cntx.font = GDI_FONT_ENGINE_FONT_MEDIUM;
        g_ble_pxp_screen_cntx.title_x = 5;
        g_ble_pxp_screen_cntx.title_y = 6;
        g_ble_pxp_screen_cntx.content_x = PXP_LEFT_GAP;

        g_ble_pxp_screen_cntx.back_x2 = 239;
        g_ble_pxp_screen_cntx.back_y2 = 239;
        g_ble_pxp_screen_cntx.back_y1 = g_ble_pxp_screen_cntx.back_y2 - 
        PXP_ITEM_HEIGHT;
        g_ble_pxp_screen_cntx.back_x1 = 200;
        //g_ble_pxp_screen_cntx.content_y = (g_ble_pxp_screen_cntx.back_y2 - g_ble_pxp_screen_cntx.title_y - PXP_ITEM_HEIGHT)/2;
        g_ble_pxp_screen_cntx.content_y = (240)/2;

        gdi_draw_filled_rectangle(0,0,239,239, g_ble_pxp_screen_cntx.bg_color);
        rect_x1 = 0;
        rect_y1 = 0;
        rect_x2 = 239;
        rect_y2 = 239;
    } else if (lcd_size == 1) {
        g_ble_pxp_screen_cntx.bg_color = 0;
        g_ble_pxp_screen_cntx.font_color = text_color;
        g_ble_pxp_screen_cntx.font = GDI_FONT_ENGINE_FONT_MEDIUM;
        g_ble_pxp_screen_cntx.title_x = bt_demo_get_convert_coordinate(lcd_size, 5);
        g_ble_pxp_screen_cntx.title_y = bt_demo_get_convert_coordinate(lcd_size, 6);
        g_ble_pxp_screen_cntx.content_x = bt_demo_get_convert_coordinate(lcd_size, PXP_LEFT_GAP);

        g_ble_pxp_screen_cntx.back_x2 = bt_demo_get_convert_coordinate(lcd_size, 239);
        g_ble_pxp_screen_cntx.back_y2 = bt_demo_get_convert_coordinate(lcd_size, 239);
        g_ble_pxp_screen_cntx.back_x1 = bt_demo_get_convert_coordinate(lcd_size, 200);
        g_ble_pxp_screen_cntx.back_y1 = g_ble_pxp_screen_cntx.back_y2 - PXP_ITEM_HEIGHT;
        g_ble_pxp_screen_cntx.content_y = 320/2;

        gdi_draw_filled_rectangle(0, 0, bt_demo_get_convert_coordinate(lcd_size, 239), bt_demo_get_convert_coordinate(lcd_size, 239), g_ble_pxp_screen_cntx.bg_color);
        rect_x1 = 0;
        rect_y1 = 0;
        rect_x2 = bt_demo_get_convert_coordinate(lcd_size, 239);
        rect_y2 = bt_demo_get_convert_coordinate(lcd_size, 239);
    } else {
        g_ble_pxp_screen_cntx.bg_color = 0;
        g_ble_pxp_screen_cntx.font_color = text_color;
        g_ble_pxp_screen_cntx.font = GDI_FONT_ENGINE_FONT_MEDIUM;
        g_ble_pxp_screen_cntx.title_x = 5;
        g_ble_pxp_screen_cntx.title_y = 6;
        g_ble_pxp_screen_cntx.content_x = PXP_LEFT_GAP;

        g_ble_pxp_screen_cntx.back_x2 = 239;
        g_ble_pxp_screen_cntx.back_y2 = 239;
        g_ble_pxp_screen_cntx.back_x1 = 200;
        g_ble_pxp_screen_cntx.back_y1 = g_ble_pxp_screen_cntx.back_y2 - PXP_ITEM_HEIGHT;
        //g_ble_pxp_screen_cntx.content_y = (g_ble_pxp_screen_cntx.back_y2 - g_ble_pxp_screen_cntx.title_y - PXP_ITEM_HEIGHT)/2;
        g_ble_pxp_screen_cntx.content_y = (240)/2;

        gdi_draw_filled_rectangle(0,0,239,239, g_ble_pxp_screen_cntx.bg_color);
        rect_x1 = 0;
        rect_y1 = 0;
        rect_x2 = 239;
        rect_y2 = 239;
    }

    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"BLE PXP"), 
                                strlen("BLE PXP"),
                                &string_width,
                                &string_height);
    ble_pxp_font_engine_show_string(g_ble_pxp_screen_cntx.title_x, 
                            g_ble_pxp_screen_cntx.title_y, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"BLE PXP"), 
                            strlen("BLE PXP"), 
                            g_ble_pxp_screen_cntx.font_color, GDI_FONT_ENGINE_FONT_MEDIUM);

    /**determine what to display by "disconencted or connected or out of range"*/
    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"Out of range"), 
                                strlen("Out of range"),
                                &string_width,
                                &string_height);
    ble_pxp_font_engine_show_string(g_ble_pxp_screen_cntx.content_x, 
                            g_ble_pxp_screen_cntx.content_y, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"Out of range"), 
                            strlen("Out of range"), 
                            g_ble_pxp_screen_cntx.font_color, GDI_FONT_ENGINE_FONT_MEDIUM);
 

    font_engine_get_string_width_height(
                                ble_pxp_convert_string_to_wstring((uint8_t *)"Exit"), 
                                strlen("Exit"),
                                &string_width,
                                &string_height);
    
    //g_ble_pxp_screen_cntx.back_x1 = g_ble_pxp_screen_cntx.back_x2 - string_width;
    //g_ble_pxp_screen_cntx.back_x1 = g_ble_pxp_screen_cntx.back_x2 - 15;
    ble_pxp_font_engine_show_string(g_ble_pxp_screen_cntx.back_x1, 
                            g_ble_pxp_screen_cntx.back_y1, 
                            ble_pxp_convert_string_to_wstring((uint8_t *)"Exit"), 
                            strlen("Exit"), 
                            g_ble_pxp_screen_cntx.font_color, GDI_FONT_ENGINE_FONT_MEDIUM);

    gdi_lcd_update_screen(rect_x1, rect_y1, rect_x2, rect_y2);
}


void ble_pxp_show_screen(ble_pxp_show_type_t type)
{
    if (BT_MODE_PXP == bt_demo_get_mode()) {
        switch (type) {
            case BLE_PXP_SHOW_DISCONNECT:
                ble_pxp_show_main_screen();
                break;

            case BLE_PXP_SHOW_CONNECT:
                ble_pxp_show_connected_screen();
                break;

            case BLE_PXP_SHOW_ALERT:
                ble_pxp_show_range_alert_screen();
                break;

            default:
                break;

        }
    } else {
        bt_sink_srv_event_t sink_event[] = {
            BT_SINK_EVENT_APP_EXT_PXP_DISCONNECT,
            BT_SINK_EVENT_APP_EXT_PXP_CONNECT,
            BT_SINK_EVENT_APP_EXT_PXP_ALERT
        };
        bt_sink_app_event_post(sink_event[type], NULL, NULL);
    }
}


void ble_pxp_show_unconnected_screen(void)
{
    uint32_t height = 0, width = 0;

    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_HEIGHT, (void *)&height);

    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_WIDTH, (void *)&width);

    ble_gatt_report("[pxp][app]enter-h: %d, w: %d\n", height, width);

    if (height == 320 && width == 320) {
        lcd_size = 1;
    } else {
        /* default 240 * 240 */
        lcd_size = 0;
    }

    ble_pxp_show_main_screen();

    ble_pxp_init();

    bt_demo_power_on(BT_MODE_PXP); 
}


/* base size 240 * 240 */
static int32_t bt_demo_get_convert_coordinate(uint8_t lcd_size, int32_t pos)
{
    double tmp = 0.0;
    double factor = 1.0;
    double skip = 0.5;
    int32_t ret = 0;

    if (lcd_size == 1) {
        factor = 320.0 / 240;
        tmp = floor(factor * pos + skip);
        ret = (int32_t) tmp;
    } else if (lcd_size == 1) {
        ret = pos;
    }

    return ret;
}


void ble_pxp_font_engine_show_string(int32_t x, int32_t y, uint8_t *string,
    int32_t len, gdi_font_engine_color_t font_color,
    gdi_font_engine_size_t font_size)
{

    gdi_font_engine_display_string_info_t string_info = {0};

    gdi_font_engine_set_font_size(font_size);
    gdi_font_engine_set_text_color(font_color);

    string_info.x = x;
    string_info.y = y;
    string_info.baseline_height = -1;
    string_info.string = string;
    string_info.length = len;
    gdi_font_engine_display_string(&string_info);
}


static void font_engine_get_string_width_height(uint8_t *string, int32_t len,
    int32_t *width, int32_t *height)
{
    *width = 1;
    *height = 1;
}

