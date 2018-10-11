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

#if defined(_MSC_VER)
#else
#include "stdio.h"
#endif

#include "sensor_demo.h"

#ifdef SENSOR_DEMO

#include "gdi_font_engine.h"
#include "gdi.h"
//#include "graphic_interface.h"
#include "main_screen.h"
#include "bt_gap.h"
#include "sensor_demo.h"
#include "mt25x3_hdk_lcd.h"

#define LEFT_GAP 5
#define ITEM_HEIGHT 25

#include "syslog.h"
log_create_module(sensor_screen, PRINT_LEVEL_INFO);
#define SENSOR_DEMO_LOGI(fmt,...) LOG_I(sensor_screen,  (fmt), ##__VA_ARGS__)

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
static char s_hr_bpm_str[16] = {0} ;
static int32_t s_hr_bpm_val = 0;

#ifdef FUSION_HEART_RATE_VARIABILITY_USE
static char s_hrv_sdnn_str[16] = {0} ;
static char s_hrv_lf_str[16] = {0} ;
static char s_hrv_hf_str[16] = {0} ;
static char s_hrv_lfhf_str[16] = {0} ;

static int32_t s_hrv_measure_status = 1;/* 1: init , 2: measuring , 3: measure done */
static int32_t s_hrv_sdnn_val = 15000;
static int32_t s_hrv_lf_val = 17000;
static int32_t s_hrv_hf_val = 23000;
static int32_t s_hrv_lfhf_val = 2000;
#endif

#ifdef FUSION_BLOOD_PRESSURE_USE
static char s_bp_str[32] = {0} ;
static int32_t s_bp_measure_status = 1;/* 1: init , 2: measuring , 3: measure done */
static int32_t s_bp_sbp;
static int32_t s_bp_dbp;
#endif

#define ITOA_LEN           20
#define ITOA_SLIM_UDIV_R(N, D, R) (((R)=(N)%(D)), ((N)/(D)))

static void show_sensor_screen(void);

static struct {
  int32_t sensor_title_x;
  int32_t sensor_title_y;
  int32_t hr_title_x;
  int32_t hr_title_y;
  int32_t hrv_title_x;
  int32_t hrv_title_y;
  int32_t hrv_value_x;
  int32_t hrv_value_y;
  int32_t bt_name_x;
  int32_t bt_name_y;
  int32_t bt_status_x;
  int32_t bt_status_y;
  int32_t back_x1;
  int32_t back_y1;
  int32_t back_x2;
  int32_t back_y2;
  gdi_color_t bg_color;
  gdi_color_t font_color;
  uint32_t width;
  uint32_t height;
} sensor_screen_cntx;

static uint8_t* sensor_convert_string_to_wstring(char* string)
{
    static uint8_t wstring[50];
    int32_t index = 0;
    if (!string) {
        return NULL;
    }
    while (*string) {
        wstring[index] = *((uint8_t*)string);
        wstring[index + 1] = 0;
        string++;
        index+=2;
    }
    return wstring;
}

static void sensor_pen_event_handler(touch_event_struct_t* pen_event, void* user_data)
{
    static int32_t back_hit;
    static int32_t hrv_hit;
    if (pen_event->position.x <= sensor_screen_cntx.back_x2 && pen_event->position.x >= sensor_screen_cntx.back_x1) {
        if (pen_event->position.y <= sensor_screen_cntx.back_y2 && pen_event->position.y >= sensor_screen_cntx.back_y1) {
            if (pen_event->type == TOUCH_EVENT_DOWN) {
                back_hit = 1;
            } else if (back_hit == 1 && pen_event->type == TOUCH_EVENT_UP) {
                back_hit = 0;
              #ifdef FUSION_HEART_RATE_VARIABILITY_USE
                s_hrv_measure_status = 1;
              #endif

              #ifdef FUSION_BLOOD_PRESSURE_USE
                s_bp_measure_status = 1;
              #endif

                SENSOR_DEMO_LOGI("sensor screen exit\r\n");
                SENSOR_DEMO_LOGI("disable_all_sensors\r\n");
                disable_all_sensors();
                show_main_screen();
            }
        }
    } else if(pen_event->position.x <= 100 && pen_event->position.x >= sensor_screen_cntx.hrv_title_x) {
        if (pen_event->position.y <= sensor_screen_cntx.hrv_value_y && pen_event->position.y >= sensor_screen_cntx.hrv_title_y) {
            if (pen_event->type == TOUCH_EVENT_DOWN) {
                hrv_hit = 1;
            } else if (hrv_hit == 1 && pen_event->type == TOUCH_EVENT_UP) {
                hrv_hit = 0;
              #ifdef FUSION_HEART_RATE_VARIABILITY_USE
                s_hrv_measure_status = 2;
                SENSOR_DEMO_LOGI("press HRV\r\n");
                enable_hrv();
              #endif

              #ifdef FUSION_BLOOD_PRESSURE_USE
                s_hr_bpm_val = 0; /* exclusive application of B.P. and heart rate */
                s_bp_measure_status = 2;
                SENSOR_DEMO_LOGI("press B.P.\r\n");
                enable_bp();
              #endif

                show_sensor_screen();
            }
        }
    }
    else {
        back_hit = 0;
        hrv_hit = 0;
    }
}

void sensor_event_handler(message_id_enum event_id, int32_t param1, void* param2)
{
}

static void show_sensor_screen(void)
{
    gdi_font_engine_display_string_info_t sensor_string_info = {0};
  #ifdef FUSION_BLOOD_PRESSURE_USE
    int32_t tmp_bp_sbp;
    int32_t tmp_bp_dbp;
  #endif
  #ifdef FUSION_HEART_RATE_VARIABILITY_USE
    int32_t hrv_value_width;
    int32_t tmp_hrv_sdnn_val;
    int32_t tmp_hrv_lf_val;
    int32_t tmp_hrv_hf_val;
    int32_t tmp_hrv_lfhf_val;
  #endif

    if ((sensor_screen_cntx.height == 0) && (sensor_screen_cntx.width==0)) {

        bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_HEIGHT, &(sensor_screen_cntx.height));
        bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_WIDTH, &(sensor_screen_cntx.width));
        SENSOR_DEMO_LOGI("width(%d), height(%d).\r\n", sensor_screen_cntx.width, sensor_screen_cntx.height);

        sensor_screen_cntx.bg_color = 0;
        sensor_screen_cntx.font_color = 0xFFFF;
        /* first */
        sensor_screen_cntx.sensor_title_x = 5;
        sensor_screen_cntx.sensor_title_y = 6;

        /* middle */
        sensor_screen_cntx.hr_title_x = LEFT_GAP;
        sensor_screen_cntx.hr_title_y = ITEM_HEIGHT*2 + sensor_screen_cntx.sensor_title_y;
        sensor_screen_cntx.hrv_title_x = LEFT_GAP;
        sensor_screen_cntx.hrv_title_y = ITEM_HEIGHT + sensor_screen_cntx.hr_title_y;
        sensor_screen_cntx.hrv_value_x = LEFT_GAP;
        sensor_screen_cntx.hrv_value_y = ITEM_HEIGHT + sensor_screen_cntx.hrv_title_y;

        /* end */
        sensor_screen_cntx.bt_name_x = LEFT_GAP;
        sensor_screen_cntx.bt_name_y = sensor_screen_cntx.height - 6 - (ITEM_HEIGHT*3);
        sensor_screen_cntx.bt_status_x = LEFT_GAP;
        sensor_screen_cntx.bt_status_y = ITEM_HEIGHT*2 + sensor_screen_cntx.bt_name_y;
        sensor_screen_cntx.back_x1 = sensor_screen_cntx.width-80;
        sensor_screen_cntx.back_y1 = ITEM_HEIGHT*2 + sensor_screen_cntx.bt_name_y;
        sensor_screen_cntx.back_x2 = sensor_screen_cntx.width-1;
        sensor_screen_cntx.back_y2 = sensor_screen_cntx.height-1;

    }

    demo_ui_register_touch_event_callback(sensor_pen_event_handler, NULL);
    gdi_draw_filled_rectangle(0,0,sensor_screen_cntx.width-1,sensor_screen_cntx.height-1, sensor_screen_cntx.bg_color);

    gdi_font_engine_size_t font = GDI_FONT_ENGINE_FONT_MEDIUM;
    gdi_font_engine_color_t text_color = {0, 255, 255, 255};//white color

    gdi_font_engine_set_font_size(font);
    gdi_font_engine_set_text_color(text_color);

    sensor_string_info.baseline_height = -1;
    sensor_string_info.x = sensor_screen_cntx.sensor_title_x;
    sensor_string_info.y = sensor_screen_cntx.sensor_title_y;
    sensor_string_info.string = sensor_convert_string_to_wstring("Sensor Subsystem");
    sensor_string_info.length = strlen("Sensor Subsystem");
    gdi_font_engine_display_string(&sensor_string_info);

    /* HR */
    memset(s_hr_bpm_str, 0, 16);
    if (s_hr_bpm_val == 0) {
        snprintf(s_hr_bpm_str,16, "HR: ___ bpm");
    } else {
        snprintf(s_hr_bpm_str,16, "HR: %3ld bpm", s_hr_bpm_val);
    }

    sensor_string_info.x = sensor_screen_cntx.hr_title_x;
    sensor_string_info.y = sensor_screen_cntx.hr_title_y;
    sensor_string_info.string = sensor_convert_string_to_wstring(s_hr_bpm_str);
    sensor_string_info.length = strlen(s_hr_bpm_str);
    gdi_font_engine_display_string(&sensor_string_info);

#ifdef FUSION_BLOOD_PRESSURE_USE
    /* B.P.*/
    sensor_string_info.x = sensor_screen_cntx.hrv_title_x;
    sensor_string_info.y = sensor_screen_cntx.hrv_title_y;
    sensor_string_info.string = sensor_convert_string_to_wstring("B.P.:");
    sensor_string_info.length = strlen("B.P.:");
    gdi_font_engine_display_string(&sensor_string_info);

    /* B.P. value */
    if(s_bp_measure_status==3){

        /* measure done */

        /* sbp/dbp mmHg */
        memset(s_bp_str, 0, 32);
        tmp_bp_sbp = s_bp_sbp;
        tmp_bp_dbp = s_bp_dbp;
        snprintf(s_bp_str, 32, "%ld/%ld mmHg", (int32_t)(tmp_bp_sbp), (int32_t)(tmp_bp_dbp));

        sensor_string_info.x = sensor_screen_cntx.hrv_value_x;
        sensor_string_info.y = sensor_screen_cntx.hrv_value_y;
        sensor_string_info.string = sensor_convert_string_to_wstring(s_bp_str);
        sensor_string_info.length = strlen(s_bp_str);
        gdi_font_engine_display_string(&sensor_string_info);
    }else if(s_bp_measure_status==2){
        /* measuring */
        sensor_string_info.x = sensor_screen_cntx.hrv_value_x;
        sensor_string_info.y = sensor_screen_cntx.hrv_value_y;
        sensor_string_info.string = sensor_convert_string_to_wstring("Measuring");
        sensor_string_info.length = strlen("Measuring");
        gdi_font_engine_display_string(&sensor_string_info);
    }else{
        /* not measure */
        sensor_string_info.x = sensor_screen_cntx.hrv_value_x;
        sensor_string_info.y = sensor_screen_cntx.hrv_value_y;
        sensor_string_info.string = sensor_convert_string_to_wstring("Press B.P. to measure");
        sensor_string_info.length = strlen("Press B.P. to measure");
        gdi_font_engine_display_string(&sensor_string_info);
    }
#endif /* FUSION_BLOOD_PRESSURE_USE */

#ifdef FUSION_HEART_RATE_VARIABILITY_USE
    gdi_font_engine_string_info_t query;

    /* HRV*/
    sensor_string_info.x = sensor_screen_cntx.hrv_title_x;
    sensor_string_info.y = sensor_screen_cntx.hrv_title_y;
    sensor_string_info.string = sensor_convert_string_to_wstring("HRV:");
    sensor_string_info.length = strlen("HRV:");
    gdi_font_engine_display_string(&sensor_string_info);
    query.string = sensor_string_info.string;
    query.count = sensor_string_info.length;
    gdi_font_engine_get_string_information(&query);
    string_width = query.width;

    /* HRV value */
    if(s_hrv_measure_status==3){

        /* measure done */
        /* SDNN in HRV title start*/
        hrv_value_width = sensor_screen_cntx.hrv_title_x+string_width;
        sensor_string_info.x = hrv_value_width;
        sensor_string_info.y = sensor_screen_cntx.hrv_title_y;
        sensor_string_info.string = sensor_convert_string_to_wstring("SDNN=");
        sensor_string_info.length = strlen("SDNN=");
        gdi_font_engine_display_string(&sensor_string_info);
        query.string = sensor_string_info.string;
        query.count = sensor_string_info.length;
        gdi_font_engine_get_string_information(&query);
        string_width = query.width;

        /* SDNN val */
        memset(s_hrv_sdnn_str, 0, 16);
        tmp_hrv_sdnn_val = s_hrv_sdnn_val/100;
        snprintf(s_hrv_sdnn_str,16, "%ld.%ld", (int32_t)(tmp_hrv_sdnn_val/10),(int32_t)(tmp_hrv_sdnn_val%10));

        hrv_value_width = hrv_value_width + string_width;
        sensor_string_info.x = hrv_value_width;
        sensor_string_info.y = sensor_screen_cntx.hrv_title_y;
        sensor_string_info.string = sensor_convert_string_to_wstring(s_hrv_sdnn_str);
        sensor_string_info.length = strlen(s_hrv_sdnn_str);
        gdi_font_engine_display_string(&sensor_string_info);
        query.string = sensor_string_info.string;
        query.count = sensor_string_info.length;
        gdi_font_engine_get_string_information(&query);
        string_width = query.width;
        /* SDNN in HRV title end*/

        /* LF */
        hrv_value_width = sensor_screen_cntx.hrv_value_x;
        sensor_string_info.x = hrv_value_width;
        sensor_string_info.y = sensor_screen_cntx.hrv_value_y;
        sensor_string_info.string = sensor_convert_string_to_wstring("L=");
        sensor_string_info.length = strlen("L=");
        gdi_font_engine_display_string(&sensor_string_info);
        query.string = sensor_string_info.string;
        query.count = sensor_string_info.length;
        gdi_font_engine_get_string_information(&query);
        string_width = query.width;

        /* LF val */
        memset(s_hrv_lf_str, 0, 16);
        tmp_hrv_lf_val = s_hrv_lf_val/100;
        snprintf(s_hrv_lf_str,16, "%ld.%ld", (int32_t)(tmp_hrv_lf_val/10),(int32_t)(tmp_hrv_lf_val%10));

        hrv_value_width = hrv_value_width + string_width;
        sensor_string_info.x = hrv_value_width;
        sensor_string_info.y = sensor_screen_cntx.hrv_value_y;
        sensor_string_info.string = sensor_convert_string_to_wstring(s_hrv_lf_str);
        sensor_string_info.length = strlen(s_hrv_lf_str);
        gdi_font_engine_display_string(&sensor_string_info);
        query.string = sensor_string_info.string;
        query.count = sensor_string_info.length;
        gdi_font_engine_get_string_information(&query);
        string_width = query.width;

        /* HF */
        hrv_value_width = hrv_value_width + string_width;
        sensor_string_info.x = hrv_value_width;
        sensor_string_info.y = sensor_screen_cntx.hrv_value_y;
        sensor_string_info.string = sensor_convert_string_to_wstring(" H=");
        sensor_string_info.length = strlen(" H=");
        gdi_font_engine_display_string(&sensor_string_info);
        query.string = sensor_string_info.string;
        query.count = sensor_string_info.length;
        gdi_font_engine_get_string_information(&query);
        string_width = query.width;

        /* HF val */
        memset(s_hrv_hf_str, 0, 16);
        tmp_hrv_hf_val = s_hrv_hf_val/100;
        snprintf(s_hrv_hf_str,16, "%ld.%ld", (int32_t)(tmp_hrv_hf_val/10),(int32_t)(tmp_hrv_hf_val%10));

        hrv_value_width = hrv_value_width + string_width;
        sensor_string_info.x = hrv_value_width;
        sensor_string_info.y = sensor_screen_cntx.hrv_value_y;
        sensor_string_info.string = sensor_convert_string_to_wstring(s_hrv_hf_str);
        sensor_string_info.length = strlen(s_hrv_hf_str);
        gdi_font_engine_display_string(&sensor_string_info);
        query.string = sensor_string_info.string;
        query.count = sensor_string_info.length;
        gdi_font_engine_get_string_information(&query);
        string_width = query.width;

        /* LFHF */
        hrv_value_width = hrv_value_width + string_width;
        sensor_string_info.x = hrv_value_width;
        sensor_string_info.y = sensor_screen_cntx.hrv_value_y;
        sensor_string_info.string = sensor_convert_string_to_wstring(" LH=");
        sensor_string_info.length = strlen(" LH=");
        gdi_font_engine_display_string(&sensor_string_info);
        query.string = sensor_string_info.string;
        query.count = sensor_string_info.length;
        gdi_font_engine_get_string_information(&query);
        string_width = query.width;

        /* LFHF val */
        memset(s_hrv_lfhf_str, 0, 16);
        tmp_hrv_lfhf_val = s_hrv_lfhf_val/100;
        snprintf(s_hrv_lfhf_str,16, "%ld.%ld", (int32_t)(tmp_hrv_lfhf_val/10),(int32_t)(tmp_hrv_lfhf_val%10));

        hrv_value_width = hrv_value_width + string_width;
        sensor_string_info.x = hrv_value_width;
        sensor_string_info.y = sensor_screen_cntx.hrv_value_y;
        sensor_string_info.string = sensor_convert_string_to_wstring(s_hrv_lfhf_str);
        sensor_string_info.length = strlen(s_hrv_lfhf_str);
        gdi_font_engine_display_string(&sensor_string_info);
    }else if(s_hrv_measure_status==2){
        /* measuring */
        sensor_string_info.x = sensor_screen_cntx.hrv_value_x;
        sensor_string_info.y = sensor_screen_cntx.hrv_value_y;
        sensor_string_info.string = sensor_convert_string_to_wstring("Measuring");
        sensor_string_info.length = strlen("Measuring");
        gdi_font_engine_display_string(&sensor_string_info);
    }else{
        /* not measure */
        sensor_string_info.x = sensor_screen_cntx.hrv_value_x;
        sensor_string_info.y = sensor_screen_cntx.hrv_value_y;
        sensor_string_info.string = sensor_convert_string_to_wstring("Press HRV to measure");
        sensor_string_info.length = strlen("Press HRV to measure");
        gdi_font_engine_display_string(&sensor_string_info);
    }
#endif /* FUSION_HEART_RATE_VARIABILITY_USE */

    sensor_string_info.x = sensor_screen_cntx.back_x1;
    sensor_string_info.y = sensor_screen_cntx.back_y1;
    sensor_string_info.string = sensor_convert_string_to_wstring("Exit");
    sensor_string_info.length = strlen("Exit");
    gdi_font_engine_display_string(&sensor_string_info);

    gdi_lcd_update_screen(0,0,sensor_screen_cntx.width-1,sensor_screen_cntx.height-1);
}

void show_sensor_ready_to_connect_screen(void)
{
    SENSOR_DEMO_LOGI("enable_all_sensors\r\n");
    enable_all_sensors();

    SENSOR_DEMO_LOGI("show_sensor_ready_to_connect_screen\r\n");
    show_sensor_screen();
}

void update_hr_data(int32_t bpm)
{
    if (s_hr_bpm_val != bpm) {
        s_hr_bpm_val = bpm;
        show_sensor_screen();
    }
}

void update_hrv_data(int32_t sdnn,int32_t lf,int32_t hf,int32_t lfhf)
{
#ifdef FUSION_HEART_RATE_VARIABILITY_USE
    s_hrv_sdnn_val = sdnn;
    s_hrv_lf_val = lf;
    s_hrv_hf_val = hf;
    s_hrv_lfhf_val = lfhf;
    s_hrv_measure_status = 3;
    SENSOR_DEMO_LOGI("update_hrv_data\r\n");
    show_sensor_screen();
#endif
}

void update_bp_data(int32_t sbp, int32_t dbp)
{
#ifdef FUSION_BLOOD_PRESSURE_USE
    s_bp_sbp = sbp;
    s_bp_dbp = dbp;
    s_bp_measure_status = 3;
    SENSOR_DEMO_LOGI("update_bp_data\r\n");
    show_sensor_screen();
#endif
}

#endif // #ifdef SENSOR_DEMO

