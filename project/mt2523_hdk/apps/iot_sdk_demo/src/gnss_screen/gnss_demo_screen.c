/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation (MediaTek Software) are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. (MediaTek) and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek (License Agreement) and been granted explicit permission to do so within
 * the License Agreement (Permitted User).  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN AS-IS BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
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


#if defined(_MSC_VER)
#else
#include "stdio.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#endif

#include "gdi.h"
#include "gdi_font_engine.h"
#include "main_screen.h"
#include "gnss_app.h"
#include "task.h"
#include "gnss_log.h"


static struct {
    int32_t title_x;
    int32_t title_y;
    int32_t lat_x;
    int32_t lat_y;
    int32_t long_x;
    int32_t long_y;
    int32_t fix_x;
    int32_t fix_y;
    int32_t sv_used_x;
    int32_t sv_used_y;
    int32_t sv_total_x;
    int32_t sv_totol_y;
    int32_t speed_x;
    int32_t speed_y;
    int32_t back_x1;
    int32_t back_y1;
    int32_t back_x2;
    int32_t back_y2;
    gdi_color_t bg_color;
    gdi_font_engine_color_t font_color;
} gnss_screen_cntx;

static uint8_t* gnss_convert_string_to_wstring(uint8_t* string);

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
    gdi_color_t color;

    int8_t pnr_string[3];
    int16_t pnr_num;
    int16_t pnr_x;
    int16_t pnr_y;
    gdi_font_engine_color_t pnr_color;

    int8_t snr_string[3];
    int16_t snr_num;
    int16_t snr_x;
    int16_t snr_y;
    gdi_font_engine_color_t snr_color;

    int8_t need_show;
} gnss_screen_colom_rect_t;


typedef struct
{
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
    gdi_font_engine_color_t color;
    int8_t is_solid;
    int8_t string[30];
    gdi_color_t frame_color;
} gnss_screen_text_rect_t;

static struct {
    int32_t width_per_col;
    int32_t bottom_y;
    int32_t max_snr_height;
    int32_t arear_height;
    gnss_screen_text_rect_t location;
    gnss_screen_text_rect_t accuracy;
    gnss_screen_text_rect_t sv_in_use;
    gnss_screen_text_rect_t sv_total;
    gnss_screen_text_rect_t exit_button;
    gnss_screen_colom_rect_t colom_array[10];
} gnss_screen_cntx001;


#define GNSS_DEMO_SCREEN_MAX_CNR 30

typedef struct gnss_demo_screen_satellite_data_struct{ 
    int32_t satellite_number; 
    int32_t cnr; 
    int32_t in_used;
}gnss_demo_screen_satellite_data_struct_t;   

typedef struct gnss_demo_screen_satellite_list_struct{ 
    int32_t statellite_total; 
    int32_t statellite_shown; 
    gnss_demo_screen_satellite_data_struct_t gnss_demo_screen_statellite_data[GNSS_DEMO_SCREEN_MAX_CNR]; 
}gnss_demo_screen_satellite_list_struct_t; 

static gnss_demo_screen_satellite_list_struct_t gnss_demo_screen_satellite_list = {0};


static uint8_t latitude[15]="\0";
static uint8_t longitude[15]="\0";
static uint8_t fix[15] = "No Fix";
static uint8_t sv_used[15] = "0";
static uint8_t sv_total[15] = "00";
static uint8_t speed[15] = "0";
static uint8_t accuracy[10] = "\0";

static void show_gnss_screen001(void);

static TaskHandle_t gnss_task_handle;

#define RESIZE_RATE LCD_CURR_HEIGHT/240
#define COLOM_BAR_GAP 8 * RESIZE_RATE
#define BOTTOM_TEXT_HEIGHT 24 * RESIZE_RATE
#define TOP_TEXT_HEIGHT 24 * RESIZE_RATE

#define LEFT_GAP 20 * RESIZE_RATE
#define ITEM_HEIGHT 28 * RESIZE_RATE

//color
#define GNSS_COLOR565(r,g,b) ((((r) >> 3) << 11) | (((g) >> 2)<<5) | ((b) >> 3))
#define GNSS_COLOM_BACKGROUND gdi_get_color_from_argb(255, 0,0,0)
#define GNSS_COLOM_COLOR_INUSED gdi_get_color_from_argb(255, 0, 255, 0)
#define GNSS_COLOM_COLOR_INVIEW gdi_get_color_from_argb(255, 127, 127, 127)
#define GNSS_COLOR_TEXT gdi_get_color_from_argb(255, 255, 255, 255)


static void text_rect_init(gnss_screen_text_rect_t* rect, 
                        int16_t x, 
                        int16_t y, 
                        int16_t width, 
                        int16_t height, 
                        int8_t is_solid, 
                        gdi_color_t color, 
                        uint8_t * string,
                        gdi_font_engine_color_t text_color)
{
    rect->frame_color = color;
    rect->color = text_color;
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    rect->is_solid = is_solid;
    strcpy((char*) rect->string, (char*) string);
}

static void colom_rect_init(gnss_screen_colom_rect_t * rect,
                    int16_t x,
                    int16_t bottom_y,
                    int16_t unit_width,
                    int16_t max_bar_height,
                    int16_t arear_height,
                    int16_t snr_num,
                    int16_t pnr_num,
                    gdi_font_engine_color_t snr_color,
                    gdi_font_engine_color_t pnr_color,
                    gdi_color_t colom_color)
{
    int32_t width, height;
    gdi_font_engine_string_info_t font_info;
    rect->need_show = 1;
    rect->color = colom_color;
    rect->width = unit_width - COLOM_BAR_GAP;
    rect->height = (max_bar_height * snr_num)/60;
    rect->x = x + COLOM_BAR_GAP/2;
    rect->y = bottom_y - BOTTOM_TEXT_HEIGHT - rect->height;

    if (pnr_num >= 100) {
        GNSSLOGD("[GNSS Demo] PNR number is wrong!!!\n");
    }
    rect->pnr_color = pnr_color;
    rect->pnr_num = pnr_num;
    rect->pnr_string[0] = (pnr_num / 10) + '0';
    rect->pnr_string[1] = (pnr_num % 10) + '0';
    rect->pnr_string[2] = 0;
    font_info.string = gnss_convert_string_to_wstring((uint8_t*) rect->pnr_string);
    font_info.count = strlen((char*) rect->pnr_string);
    gdi_font_engine_get_string_information(&font_info);
    width = font_info.width;
    height = font_info.height;
    rect->pnr_x = x + ((unit_width - width) >> 1);
    rect->pnr_y = bottom_y - BOTTOM_TEXT_HEIGHT + ((BOTTOM_TEXT_HEIGHT - height) >> 1);

    if (snr_num >= 100) {
        GNSSLOGD("[GNSS Demo] SNR number is wrong!!!\n");
    }
    rect->snr_color = snr_color;
    rect->snr_num = snr_num;
    rect->snr_string[0] = (snr_num / 10) + '0';
    rect->snr_string[1] = (snr_num % 10) + '0';
    rect->snr_string[2] = 0;
    font_info.string = gnss_convert_string_to_wstring((uint8_t*) rect->snr_string);
    font_info.count = strlen((char*) rect->snr_string);
    gdi_font_engine_get_string_information(&font_info);
    width = font_info.width;
    height = font_info.height;
    rect->snr_x = x + ((unit_width - width) >> 1);
    rect->snr_y = bottom_y - BOTTOM_TEXT_HEIGHT - height - rect->height + ((TOP_TEXT_HEIGHT - height) >> 1);
    
}

static void text_rect_draw(gnss_screen_text_rect_t* rect)
{
    gdi_font_engine_string_info_t font_info;
    gdi_font_engine_display_string_info_t param;
    gdi_draw_rectangle(rect->x, rect->y, rect->x + rect->width - 1, rect->y + rect->height - 1, rect->frame_color);
    font_info.string = gnss_convert_string_to_wstring((uint8_t*) rect->string);
    font_info.count = strlen((char*) rect->string);
    gdi_font_engine_get_string_information(&font_info);
    param.string = gnss_convert_string_to_wstring((uint8_t*) rect->string);
    param.length = strlen((char*) rect->string);
    param.x = rect->x + (rect->width - font_info.width)/2;
    param.y = rect->y + (rect->height - font_info.height)/2;
    param.baseline_height = -1;
    gdi_font_engine_display_string(&param);
}

static void text_rect_draw_text_only(gnss_screen_text_rect_t* rect)
{
    gdi_font_engine_string_info_t font_info;
    gdi_font_engine_display_string_info_t param;
    font_info.string = gnss_convert_string_to_wstring((uint8_t*) rect->string);
    font_info.count = strlen((char*) rect->string);
    gdi_font_engine_get_string_information(&font_info);
    param.string = gnss_convert_string_to_wstring((uint8_t*) rect->string);
    param.length = strlen((char*) rect->string);
    param.x = rect->x + (rect->width - font_info.width)/2;
    param.y = rect->y + (rect->height - font_info.height)/2;
    param.baseline_height = -1;
    gdi_font_engine_display_string(&param);
}

static void colom_rect_draw(gnss_screen_colom_rect_t * rect)
{
    gdi_font_engine_display_string_info_t param;
    gdi_draw_filled_rectangle(rect->x, rect->y, rect->x + rect->width - 1, rect->y + rect->height - 1, rect->color);
    param.string = gnss_convert_string_to_wstring((uint8_t *)rect->pnr_string);
    param.length = strlen((char*) rect->pnr_string);
    param.x = rect->pnr_x;
    param.y = rect->pnr_y;
    param.baseline_height = -1;
    gdi_font_engine_display_string(&param);
    param.string = gnss_convert_string_to_wstring((uint8_t *)rect->snr_string);
    param.length = strlen((char*) rect->snr_string);
    param.x = rect->snr_x;
    param.y = rect->snr_y;
    param.baseline_height = -1;
    gdi_font_engine_display_string(&param);
}

static void gnss_screen_001_init()
{
    int32_t i;
    gdi_color_t temp_color;
    gdi_font_engine_color_t text_color;
    int8_t temp_string[30];
    int32_t max_statellite = 10;
    memset(&gnss_screen_cntx001, 0, sizeof(gnss_screen_cntx001));
    gnss_screen_cntx001.max_snr_height = 60 * RESIZE_RATE;
    gnss_screen_cntx001.arear_height = gnss_screen_cntx001.max_snr_height + BOTTOM_TEXT_HEIGHT + TOP_TEXT_HEIGHT;
    gnss_screen_cntx001.bottom_y = 210 * RESIZE_RATE;
    gnss_screen_cntx001.width_per_col = 240/10 * RESIZE_RATE;
    text_color.alpha = 255;
    text_color.blue = 255;
    text_color.green = 255;
    text_color.red = 255;
    
    if ((latitude[0] == 0 && longitude[0] == 0)) {
        sprintf((char*) &temp_string, "%s", fix);
    } else {
        sprintf((char*) &temp_string, "%s, %s", longitude, latitude);
    }
    text_rect_init(&gnss_screen_cntx001.location, 5 * RESIZE_RATE, 10 * RESIZE_RATE, 230 * RESIZE_RATE, 25 * RESIZE_RATE, 0, GNSS_COLOR_TEXT, (uint8_t*) temp_string, text_color);
    if ((latitude[0] == 0 && longitude[0] == 0)) {
        temp_string[0] = 0;
    } else {
        sprintf((char*) &temp_string, "accuracy:%s m", accuracy);
    }
    text_rect_init(&gnss_screen_cntx001.accuracy, 5 * RESIZE_RATE, 35 * RESIZE_RATE, 230 * RESIZE_RATE, 25 * RESIZE_RATE, 0, GNSS_COLOR_TEXT, (uint8_t*) temp_string, text_color);
    sprintf((char*) &temp_string, "in view:%s", sv_total);
    text_rect_init(&gnss_screen_cntx001.sv_total, 10 * RESIZE_RATE, 75 * RESIZE_RATE, 100 * RESIZE_RATE, 25 * RESIZE_RATE, 0, GNSS_COLOR_TEXT, (uint8_t*) temp_string, text_color);
    sprintf((char*) &temp_string, "in used:%s", sv_used);
    text_rect_init(&gnss_screen_cntx001.sv_in_use, 130 * RESIZE_RATE, 75 * RESIZE_RATE, 100 * RESIZE_RATE, 25 * RESIZE_RATE, 0, GNSS_COLOR_TEXT, (uint8_t*) temp_string, text_color);
    text_rect_init(&gnss_screen_cntx001.exit_button, 160 * RESIZE_RATE, 215 * RESIZE_RATE, 100 * RESIZE_RATE, 25 * RESIZE_RATE, 0, GNSS_COLOR_TEXT, (uint8_t*) "Exit", text_color);

    if (gnss_demo_screen_satellite_list.statellite_shown < max_statellite) {
        max_statellite = gnss_demo_screen_satellite_list.statellite_shown;
    }
    for (i = 0; i < max_statellite; i ++) {
        if ( gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[i].in_used == 1) {
            temp_color = GNSS_COLOM_COLOR_INUSED;
        } else {
            temp_color = GNSS_COLOM_COLOR_INVIEW;
        }
        colom_rect_init(&gnss_screen_cntx001.colom_array[i], 
                        i * gnss_screen_cntx001.width_per_col, 
                        gnss_screen_cntx001.bottom_y,
                        gnss_screen_cntx001.width_per_col,
                        60,
                        gnss_screen_cntx001.arear_height,
                        gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[i].cnr,
                        gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[i].satellite_number,
                        text_color,
                        text_color,
                        temp_color);
    }
}
uint8_t* gnss_convert_string_to_wstring(uint8_t* string)
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

static void gnss_pen_event_handler_001(touch_event_struct_t* pen_event, void* user_data)
{
    static int32_t back_hit;
    if (pen_event->position.x <= gnss_screen_cntx001.exit_button.x + gnss_screen_cntx001.exit_button.width && pen_event->position.x >= gnss_screen_cntx001.exit_button.x) {
        if (pen_event->position.y <= gnss_screen_cntx001.exit_button.y + gnss_screen_cntx001.exit_button.height && pen_event->position.y >= gnss_screen_cntx001.exit_button.y) {
            if (pen_event->type == TOUCH_EVENT_DOWN) {
                back_hit = 1;
            } else if (back_hit == 1 && pen_event->type == TOUCH_EVENT_UP) {
                back_hit = 0;
                gnss_demo_app_stop();
                gnss_demo_app_destroy(gnss_task_handle);
                show_main_screen();
            }
        }
    } else {
        back_hit = 0;
    }
}


static int32_t gnss_update_data(gnss_sentence_info_t *input)
{
    uint8_t *gpgga = input->GPGGA;
    uint8_t *gpgsv = input->GPGSV;
    uint8_t *gprmc = input->GPRMC;
    uint8_t *gpgsa = input->GPGSA;
    uint8_t *gpacc = input->GPACC;
    int32_t i = 0;
    int32_t j = 0;
    int32_t k = 0;
    int32_t flag = 0;
    gnss_demo_screen_satellite_data_struct_t temp_cnr[32] = {{0}};//GP:32,BD:14,GL:24
    int32_t temp_inused[12] = {0};
    int32_t temp_inused_num = 0;

    strcpy((char*) sv_used, "00");
    strcpy((char*) sv_total, "00");
    strcpy((char*) accuracy, "");
    GNSSLOGD("[GNSS Demo] gnss_update_data\n");
    memset(&gnss_demo_screen_satellite_list, 0, sizeof(gnss_demo_screen_satellite_list_struct_t));
    if (gpgsv[0] != 0){
        gnss_demo_screen_satellite_list.statellite_total = (gpgsv[11] - '0') * 10 + (gpgsv[12] - '0');
        
        j = 0;
        k = 0;
        for (i = 0; i < 400; i++) {
            if (k >= gnss_demo_screen_satellite_list.statellite_total){
                break;
            }
            if (gpgsv[i] == '$'){
                j = 0;
            }
            if (gpgsv[i] == ',' || gpgsv[i] == '*'){
                if (j > 3 && (j - 4) % 4 == 0){
                    if (gpgsv[i - 3] != ','){
                        gnss_demo_screen_satellite_list.statellite_total--;
                        temp_cnr[k].satellite_number = -1;
                    }
                    else{
                        temp_cnr[k].satellite_number = (gpgsv[i - 2] - '0') * 10 + (gpgsv[i - 1] - '0');
                    }
                }
                if (j > 6 && (j - 7) % 4 == 0){
                    if (temp_cnr[k].satellite_number != -1){
                        if (gpgsv[i - 1] == ','){
                            temp_cnr[k].cnr = 0;
                        }
                        else{
                            temp_cnr[k].cnr = (gpgsv[i - 2] - '0') * 10 + (gpgsv[i - 1] - '0');
                        }
                        k++;
                    }
                }
                j++;
            }
        }
    }

    sv_total[0] = gnss_demo_screen_satellite_list.statellite_total / 10 + '0';
    sv_total[1] = gnss_demo_screen_satellite_list.statellite_total % 10 + '0';

    if (gnss_demo_screen_satellite_list.statellite_total >  0){
        j = 0;
        for (i  = 0; i < 100; i++){
            if (gpgsa[i] == ','){
                if (j >2 && j <= 14){
                    if (gpgsa[i - 1] != ','){
                        if (gpgsa[i - 3] == ','){
                            temp_inused[temp_inused_num] = (gpgsa[i - 2] - '0') * 10 + (gpgsa[i - 1] - '0');
                            temp_inused_num++;
                        }
                    }
                    else{
                        break;
                    }
                }
                j++;
            }
        }
        
        sv_used[0] = temp_inused_num / 10 + '0';
        sv_used[1] = temp_inused_num % 10 + '0';
        GNSSLOGD("[GNSS Demo] sv_used:%s\n", sv_used);

        for (i = 0; i < gnss_demo_screen_satellite_list.statellite_total - 1; i++){
            for (j = i + 1; j < gnss_demo_screen_satellite_list.statellite_total; j++){
                if (temp_cnr[j].cnr > temp_cnr[i].cnr){
                    int32_t cnr = temp_cnr[i].cnr;
                    int32_t number = temp_cnr[i].satellite_number;
                    temp_cnr[i].cnr = temp_cnr[j].cnr;
                    temp_cnr[j].cnr = cnr;
                    temp_cnr[i].satellite_number = temp_cnr[j].satellite_number;
                    temp_cnr[j].satellite_number = number;
                }
            }
        }

        gnss_demo_screen_satellite_list.statellite_shown = GNSS_DEMO_SCREEN_MAX_CNR > gnss_demo_screen_satellite_list.statellite_total ? gnss_demo_screen_satellite_list.statellite_total : GNSS_DEMO_SCREEN_MAX_CNR;
        memcpy(gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data, temp_cnr, sizeof(gnss_demo_screen_satellite_data_struct_t) * gnss_demo_screen_satellite_list.statellite_shown);
        for (i = 0; i < gnss_demo_screen_satellite_list.statellite_shown; i++){
            for (j = i + 1; j < gnss_demo_screen_satellite_list.statellite_shown; j++){
                if (gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[j].satellite_number < gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[i].satellite_number){
                    int32_t cnr = gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[i].cnr;
                    int32_t number = gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[i].satellite_number;
                    gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[i].cnr = gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[j].cnr;
                    gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[j].cnr = cnr;
                    gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[i].satellite_number = gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[j].satellite_number;
                    gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[j].satellite_number = number;
                }
            }
        }
        for (i = 0; i < gnss_demo_screen_satellite_list.statellite_shown; i++){
            for (j = 0; j < temp_inused_num; j++){
                if (temp_inused[j] == gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[i].satellite_number){
                    gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[i].in_used = 1;
                }
            }
            
            GNSSLOGD("statellite_list,index:%d,number:%d,cnr:%d,inused:%d\n", i, gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[i].satellite_number, gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[i].cnr, gnss_demo_screen_satellite_list.gnss_demo_screen_statellite_data[i].in_used);
        }
    }

    latitude[0] = 0;
    longitude[0] = 0;
    j = 0;
    for (i = 0; i < 100; i++) {
        if (gpgga[i] == ',') {
            if (gpgga[i + 1] == 'N') {
                int32_t len = i - j - 1 + 2;
                latitude[0] = 'N';
                latitude[1] = ':';
                latitude[len] = 0;
                while (len > 2) {
                    latitude[len - 1] = gpgga[j + len - 2];
                    len--;
                }
            }
            if (gpgga[i + 1] == 'E') {
                int32_t len = i - j - 1 + 2;
               
                if (gpgga[i + 3] == '0') {
                    GNSSLOGD("[GNSS Demo] Get location, invalid data!\n");
                    longitude[0] = 0;
                    latitude[0] = 0;
                    strcpy((char*) fix, "No Fix");
                    strcpy((char*) speed, "0");
                    return 1;
                }
                longitude[0] = 'E';
                longitude[1] = ':';
                longitude[len] = 0;
                while (len > 2) {
                    longitude[len - 1] = gpgga[j + len - 2];
                    len--;
                }
                break;
            }
            if (gpgga[i + 1] == 'S') {
                int32_t len = i - j - 1 + 2;
                latitude[0] = 'S';
                latitude[1] = ':';
                latitude[len] = 0;
                while (len > 2) {
                    latitude[len - 1] = gpgga[j + len - 2];
                    len--;
                }
            }
            if (gpgga[i + 1] == 'W') {
                int32_t len = i - j - 1 + 2;

                if (gpgga[i + 3] == '0') {
                    GNSSLOGD("[GNSS Demo] Get location, invalid data!\n");
                    longitude[0] = 0;
                    latitude[0] = 0;
                    strcpy((char*) fix, "No Fix");
                    strcpy((char*) speed, "0");
                    return 1;
                }
                longitude[0] = 'W';
                longitude[1] = ':';
                longitude[len] = 0;
                while (len > 2) {
                    longitude[len - 1] = gpgga[j + len - 2];
                    len--;
                }
                break;
            }
            j = i;            
        }
    }

    j = 0;
    for (i = 0; i < 100; i++) {
        if (gprmc[i] == ',') {
            if (gprmc[i + 1] == 'E' || gprmc[i + 1] == 'W') {
                int32_t len = 0;
                while (gprmc[i + 3 + len] != ',') {
                    speed[len] = gprmc[i + 3 + len];
                    len++;
                }
                speed[len] = 0;
                break;
            }
            j = i;            
        }
    }

    j = 0;
    for (i = 0; i < 100; i++) {
        if (gpacc[i] == '*') {
            accuracy[j] = '\0';
            break;
        } else if (flag) {
            accuracy[j++] = gpacc[i];
        } else if (gpacc[i] == ',') {
            flag = 1;
        }
    }

    if (latitude[0] == 0 && longitude[0] == 0) {
        strcpy((char*) fix, "No Fix");
        strcpy((char*) speed, "0");
        return 1;
    }

    {
        float longitude_second_temp = 0;
        float longitude_mininute_temp = 0;
        float longitude_temp;
        longitude_second_temp = (longitude[strlen((char*) longitude) - 7] - '0') * 10 + (longitude[strlen((char*) longitude) - 6] - '0') 
            + (float)(longitude[strlen((char*) longitude) - 4] - '0') / 10 + (float)(longitude[strlen((char*) longitude) - 3] - '0') / 100
            + (float)(longitude[strlen((char*) longitude) - 2] - '0') / 1000 + (float)(longitude[strlen((char*) longitude) - 1] - '0') / 10000;
        j = 1;
        for (i = strlen((char*) longitude) - 8; i > 1 ; i--){
            longitude_mininute_temp = longitude_mininute_temp + (longitude[i] - '0') * j;
            j = j * 10;
        }
        longitude_temp = (longitude_mininute_temp + longitude_second_temp / 60) * 10000;
        flag = 0;
        for (i = 9; i > 1; i--){
            longitude[i] = (int32_t)longitude_temp % 10 + '0';
            if (flag == 0 && longitude[i] == '0'){
                longitude[i] = 0;
            }
            else{
                flag = 1;
            }
            if (i == 5){
                longitude[i] = '.';
            }
            else{
                longitude_temp = (int32_t)longitude_temp / 10;
            }
        }
        longitude[10] = 0;
    }
    {
        float latitude_second_temp = 0;
        float latitude_mininute_temp = 0;
        float latitude_temp;
        latitude_second_temp = (latitude[strlen((char*) latitude) - 7] - '0') * 10 + (latitude[strlen((char*) latitude) - 6] - '0') 
            + (float)(latitude[strlen((char*) latitude) - 4] - '0') / 10 + (float)(latitude[strlen((char*) latitude) - 3] - '0') / 100
            + (float)(latitude[strlen((char*) latitude) - 2] - '0') / 1000 + (float)(latitude[strlen((char*) latitude) - 1] - '0') / 10000;
        j = 1;
        for (i = strlen((char*) latitude) - 8; i > 1 ; i--){
            latitude_mininute_temp = latitude_mininute_temp + (latitude[i] - '0') * j;
            j = j * 10;
        }
        latitude_temp = (latitude_mininute_temp + latitude_second_temp / 60) * 10000;
        flag = 0;
        for (i = 9; i > 1; i--){
            latitude[i] = (int32_t)latitude_temp % 10 + '0';
            if (flag == 0 && latitude[i] == '0'){
                latitude[i] = 0;
            }
            else{
                flag = 1;
            }
            if (i == 5){
                latitude[i] = '.';
            }
            else{
                latitude_temp = (int32_t)latitude_temp / 10;
            }
        }
        latitude[10] = 0;
    }
	
    strcpy((char*) fix, "3D Fixed");

    return 0;

}

static void gnss_demo_app_location_handle(gnss_location_handle_type_t type, void* param)
{
    if (type == GNSS_LOCATION_HANDLE_TYPE_ERROR) {
        GNSSLOGD("[GNSS Demo] location handle error! type: %d\n", (int)param);
    } else {
        gnss_location_struct_t *location = (gnss_location_struct_t *)param;
        GNSSLOGD("[GNSS Demo] App Get Location, latitude:%s, longitude:%s, accuracy:%d\n", location->latitude, location->longitude, (int)location->accuracy);
        gnss_update_data(&location->nmea_sentence);		
        ui_send_event(MESSAGE_ID_GNSS_NMEA, 0, 0);
    }
}

static void gnss_demo_main(){
    int32_t periodic = 1;
    
    latitude[0] = 0; 
    longitude[0] = 0;
    strcpy((char*) fix, "No Fix");
    strcpy((char*) sv_total, "00");
    strcpy((char*) sv_used, "0");
    strcpy((char*) speed, "0");
    memset(&gnss_demo_screen_satellite_list, 0, sizeof(gnss_demo_screen_satellite_list_struct_t));

    gnss_task_handle = gnss_demo_app_create();
    gnss_demo_app_config(periodic, gnss_demo_app_location_handle);
    gnss_demo_app_start();
}

void gnss_event_handler(message_id_enum event_id, int32_t param1, void* param2)
{
    show_gnss_screen001();
}

static void show_gnss_screen001(void)
{
    int32_t i;
    int32_t max_statellite = 10;
    
    gnss_screen_001_init();
    gdi_draw_filled_rectangle(0,0,240 * RESIZE_RATE - 1,240 * RESIZE_RATE -1, gnss_screen_cntx.bg_color);
    text_rect_draw_text_only(&gnss_screen_cntx001.location);
    text_rect_draw_text_only(&gnss_screen_cntx001.accuracy);
    text_rect_draw_text_only(&gnss_screen_cntx001.sv_in_use);
    text_rect_draw_text_only(&gnss_screen_cntx001.sv_total);
    text_rect_draw_text_only(&gnss_screen_cntx001.exit_button);
    if (gnss_demo_screen_satellite_list.statellite_shown < max_statellite) {
        max_statellite = gnss_demo_screen_satellite_list.statellite_shown;
    }
    for (i = 0; i < max_statellite; i++)
    {
        colom_rect_draw(&gnss_screen_cntx001.colom_array[i]);
    }
    gdi_lcd_update_screen(0, 0, LCD_CURR_WIDTH - 1, LCD_CURR_HEIGHT - 1);;
}

void show_gnss_screen(void)
{
    gdi_font_engine_color_t color;
    demo_ui_register_touch_event_callback(gnss_pen_event_handler_001, NULL);
    gnss_demo_main();
    color.alpha = 255;
    color.blue = 255;
    color.green = 255;
    color.red = 255;
    gdi_font_engine_set_text_color(color);
    show_gnss_screen001();
    if (0)
    {
        text_rect_draw(NULL);
    }
}

