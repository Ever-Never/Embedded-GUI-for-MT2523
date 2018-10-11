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

#include "stdio.h"
#include "FreeRTOS.h"
#include "gdi_font_engine.h"
#include "gdi.h"
#include "main_screen.h"
#include "bt_audio.h"
#include "syslog.h"
#include "bt_init.h"
#include <string.h>
#include "bt_sink_app_event.h"
#include "ble_pxp_main.h"



bt_audio_context_t g_bt_audio_cntx;

//Enable this option, show EDR & LE duo mode demo
//If you don't need it, just undefine it
#define __BT_AUDIO_DEMO_WITH_PXP__

log_create_module(bt_audio, PRINT_LEVEL_INFO);

static void font_engine_get_string_width_height(uint8_t *string, int32_t len,
    int32_t *width, int32_t *height);

bool hci_log_enabled(void)
{
    return g_bt_audio_cntx.entered;
}

uint8_t *bt_audio_convert_string_to_wstring(char *string)
{
    static uint8_t wstring[128];
    int32_t index = 0;
    if (!string) {
        return NULL;
    }

    while (*string) {
        wstring[index] = *string;
        wstring[index + 1] = 0;
        string++;
        index += 2;
    }
    return wstring;
}

void bt_audio_post_event_callback(bt_sink_srv_event_t event_id, bt_sink_srv_status_t result, void *parameters)
{
    if (NULL != parameters) {
        vPortFree(parameters);
    }
}

static void bt_audio_pen_event_handler(touch_event_struct_t *pen_event, void *user_data)
{
    bt_audio_point_t point, l_corner, r_corner;
    int32_t touch = -1;

    LOG_I(bt_audio, "[bt_audio]pen_hdr(s)--src: %d, type: %d, x: %d, y: %d, touch: %d\n",
          g_bt_audio_cntx.scr, pen_event->type, pen_event->position.x, pen_event->position.y, touch);

    if (g_bt_audio_cntx.entered) {
        if (pen_event->type == TOUCH_EVENT_UP) {
            bt_sink_app_ext_cmd_t *ext_cmd_p = (bt_sink_app_ext_cmd_t *)pvPortMalloc(sizeof(*ext_cmd_p));

            point.x = pen_event->position.x;
            point.y = pen_event->position.y;
            switch (g_bt_audio_cntx.scr) {
                case BT_AUDIO_SCR_STOP: {
                    l_corner.x = BT_AUDIO_MUSIC_X;
                    l_corner.y = BT_AUDIO_MUSIC_Y;
                    r_corner.x = BT_AUDIO_MUSIC_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle stop touch */
                        //bt_sink_event_post(BT_SINK_EVENT_AVRCP_PAUSE, NULL, NULL);
                        //bt_sink_event_send(BT_SINK_EVENT_AVRCP_PAUSE, NULL);
                        ext_cmd_p->key_value = BT_SINK_SRV_KEY_FUNC;
                        ext_cmd_p->key_action = BT_SINK_SRV_KEY_ACT_PRESS_UP;
                        touch = 1;
                        break;
                    }

                    l_corner.x = BT_AUDIO_MUSIC_X;
                    l_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H;
                    r_corner.x = BT_AUDIO_MUSIC_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 2;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle next touch */
                        //bt_sink_event_post(BT_SINK_EVENT_AVRCP_NEXT_TRACK, NULL, NULL);
                        //bt_sink_event_send(BT_SINK_EVENT_AVRCP_NEXT_TRACK, NULL);
                        ext_cmd_p->key_value = BT_SINK_SRV_KEY_NEXT;
                        ext_cmd_p->key_action = BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP;
                        touch = 2;
                        break;
                    }

                    l_corner.x = BT_AUDIO_MUSIC_X;
                    l_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 2;
                    r_corner.x = BT_AUDIO_MUSIC_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 3;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle pre touch */
                        //bt_sink_event_post(BT_SINK_EVENT_AVRCP_PRE_TRACK, NULL, NULL);
                        //bt_sink_event_send(BT_SINK_EVENT_AVRCP_PRE_TRACK, NULL);
                        ext_cmd_p->key_value = BT_SINK_SRV_KEY_PREV;
                        ext_cmd_p->key_action = BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP;
                        touch = 3;
                        break;
                    }

                    break;
                }

                case BT_AUDIO_SCR_PLAYING: {
                    l_corner.x = BT_AUDIO_MUSIC_X;
                    l_corner.y = BT_AUDIO_MUSIC_Y;
                    r_corner.x = BT_AUDIO_MUSIC_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle play touch */
                        //bt_sink_event_post(BT_SINK_EVENT_AVRCP_PLAY, NULL, NULL);
                        //bt_sink_event_send(BT_SINK_EVENT_AVRCP_PLAY, NULL);
                        ext_cmd_p->key_value = BT_SINK_SRV_KEY_FUNC;
                        ext_cmd_p->key_action = BT_SINK_SRV_KEY_ACT_PRESS_UP;
                        touch = 4;
                        break;
                    }

                    l_corner.x = BT_AUDIO_MUSIC_X;
                    l_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H;
                    r_corner.x = BT_AUDIO_MUSIC_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 2;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle next touch */
                        //bt_sink_event_post(BT_SINK_EVENT_AVRCP_NEXT_TRACK, NULL, NULL);
                        //bt_sink_event_send(BT_SINK_EVENT_AVRCP_NEXT_TRACK, NULL);
                        ext_cmd_p->key_value = BT_SINK_SRV_KEY_NEXT;
                        ext_cmd_p->key_action = BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP;
                        touch = 5;
                        break;
                    }

                    l_corner.x = BT_AUDIO_MUSIC_X;
                    l_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 2;
                    r_corner.x = BT_AUDIO_MUSIC_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 3;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle pre touch */
                        //bt_sink_event_post(BT_SINK_EVENT_AVRCP_PRE_TRACK, NULL, NULL);
                        //bt_sink_event_send(BT_SINK_EVENT_AVRCP_PRE_TRACK, NULL);
                        ext_cmd_p->key_value = BT_SINK_SRV_KEY_PREV;
                        ext_cmd_p->key_action = BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP;
                        touch = 6;
                        break;
                    }

                    break;
                }

                case BT_AUDIO_SCR_INCOMING: {
                    l_corner.x = BT_AUDIO_CALL_X;
                    l_corner.y = BT_AUDIO_CALL_Y + BT_AUDIO_ITEM_H * 2;
                    r_corner.x = BT_AUDIO_CALL_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_CALL_Y + BT_AUDIO_ITEM_H * 3;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle answer touch */
                        //bt_sink_event_post(BT_SINK_EVENT_HF_ANSWER, NULL, NULL);
                        //bt_sink_event_send(BT_SINK_EVENT_HF_ANSWER, NULL);
                        ext_cmd_p->key_value = BT_SINK_SRV_KEY_FUNC;
                        ext_cmd_p->key_action = BT_SINK_SRV_KEY_ACT_PRESS_UP;
                        touch = 7;
                        break;
                    }

                    l_corner.x = BT_AUDIO_CALL_X;
                    l_corner.y = BT_AUDIO_CALL_Y + BT_AUDIO_ITEM_H * 3;
                    r_corner.x = BT_AUDIO_CALL_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_CALL_Y + BT_AUDIO_ITEM_H * 4;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle reject touch */
                        //bt_sink_event_post(BT_SINK_EVENT_HF_REJECT, NULL, NULL);
                        //bt_sink_event_send(BT_SINK_EVENT_HF_REJECT, NULL);
                        ext_cmd_p->key_value = BT_SINK_SRV_KEY_FUNC;
                        ext_cmd_p->key_action = BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP;
                        touch = 8;
                        break;
                    }

                    break;
                }

                case BT_AUDIO_SCR_CALLING: {
                    l_corner.x = BT_AUDIO_CALL_DROP_X;
                    l_corner.y = BT_AUDIO_CALL_DROP_Y;
                    r_corner.x = BT_AUDIO_CALL_DROP_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_CALL_DROP_Y + BT_AUDIO_ITEM_H;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle hang up touch */
                        //bt_sink_event_post(BT_SINK_EVENT_HF_HANG_UP, NULL, NULL);
                        //bt_sink_event_send(BT_SINK_EVENT_HF_HANG_UP, NULL);
                        ext_cmd_p->key_value = BT_SINK_SRV_KEY_FUNC;
                        ext_cmd_p->key_action = BT_SINK_SRV_KEY_ACT_PRESS_UP;
                        touch = 9;
                        break;
                    }

                    l_corner.x = BT_AUDIO_CALL_DROP_X;
                    l_corner.y = BT_AUDIO_CALL_DROP_Y + BT_AUDIO_ITEM_H;
                    r_corner.x = BT_AUDIO_CALL_DROP_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_CALL_DROP_Y + BT_AUDIO_ITEM_H * 2;

                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        /* handle hang up touch */
                        //bt_sink_event_send(BT_SINK_EVENT_HF_SWITCH_AUDIO_PATH, NULL);
                        ext_cmd_p->key_value = BT_SINK_SRV_KEY_FUNC;
                        ext_cmd_p->key_action = BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK;
                        touch = 10;
                        break;
                    }

                    break;
                }

                case BT_AUDIO_SCR_IDLE: {
                    l_corner.x = BT_AUDIO_CONNECT_X;
                    l_corner.y = BT_AUDIO_CONNECT_Y + BT_AUDIO_ITEM_H;
                    r_corner.x = BT_AUDIO_CALL_DROP_X + BT_AUDIO_ITEM_W;
                    r_corner.y = BT_AUDIO_CALL_DROP_Y + BT_AUDIO_ITEM_H * 2;
                    if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                        bt_bd_addr_t empty_addr;
                        memset(&empty_addr, 0, sizeof(bt_bd_addr_t));
                        if (0 != memcmp(&empty_addr, &g_bt_audio_cntx.reconnect_addr, sizeof(bt_bd_addr_t))) {
                            ext_cmd_p->key_value = BT_SINK_SRV_KEY_FUNC;
                            ext_cmd_p->key_action = BT_SINK_SRV_KEY_ACT_DOUBLE_CLICK;
                            //bt_sink_event_send(BT_SINK_EVENT_CM_CONNECT, &reconnect_addr);
                            touch = 11;
                        }
                    }
                    break;
                }

                default:
                    break;
            }

            if (touch > 0) {
                bt_sink_app_event_post((bt_sink_srv_event_t)BT_SINK_EVENT_APP_EXT_COMMAND,
                                       (void *)ext_cmd_p, bt_audio_post_event_callback);
            } else {
                vPortFree(ext_cmd_p);
            }

            l_corner.x = BT_AUDIO_HOME_X;
            l_corner.y = BT_AUDIO_HOME_Y;
            r_corner.x = 240;
            r_corner.y = 240;
            if (bt_audio_is_point_range(&point, &l_corner, &r_corner)) {
                /* handle exit touch */
                g_bt_audio_cntx.entered = false;
#ifdef __BT_AUDIO_DEMO_WITH_PXP__
                ble_pxp_deinit();
#endif /* __BT_AUDIO_DEMO_WITH_PXP__ */
                bt_demo_power_off();
                show_main_screen();
            }
        }
    }
    LOG_I(bt_audio, "[bt_audio]pen_hdr(e)--type: %d, x: %d, y: %d, touch: %d\n",
          pen_event->type, pen_event->position.x, pen_event->position.y, touch);
}


static void bt_audio_show_th_str()
{
    int32_t width = 0, height = 0;

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_TITLE),
        strlen(STR_BT_AUDIO_TITLE),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_TITLE_X, BT_AUDIO_TITLE_Y,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_TITLE),
                                     strlen(STR_BT_AUDIO_TITLE), g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_HOME),
        strlen(STR_BT_AUDIO_HOME),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_HOME_X, BT_AUDIO_HOME_Y,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_HOME),
                                     strlen(STR_BT_AUDIO_HOME), g_bt_audio_cntx.font_color);
}


static void bt_audio_show_pxp_status(void)
{
#ifdef __BT_AUDIO_DEMO_WITH_PXP__
    char *string;

    switch (g_bt_audio_cntx.pxp_status) {
        case BT_SINK_EVENT_APP_EXT_PXP_CONNECT:
            string = STR_BT_AUDIO_PXP_CONNECTED;
            break;

        case BT_SINK_EVENT_APP_EXT_PXP_ALERT:
            string = STR_BT_AUDIO_PXP_ALERT;
            break;

        default:
            string = STR_BT_AUDIO_PXP_IDLE;
            break;
    }

    bt_audio_font_engine_show_string(BT_AUDIO_PXP_STATUS_X,
                                     BT_AUDIO_PXP_STATUS_Y,
                                     bt_audio_convert_string_to_wstring(string),
                                     strlen(string),
                                     g_bt_audio_cntx.font_color);
#endif /* __BT_AUDIO_DEMO_WITH_PXP__ */
}

static void bt_audio_show_idle_screen()
{
    int32_t width = 0, height = 0;


    g_bt_audio_cntx.scr = BT_AUDIO_SCR_IDLE;
    bt_audio_gdi_draw_solid_rect(0, 0, 239, 239, g_bt_audio_cntx.bg_color);

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_CONN),
        strlen(STR_BT_AUDIO_CONN),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_CONNECT_X,
                                     BT_AUDIO_CONNECT_Y,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_CONN),
                                     strlen(STR_BT_AUDIO_CONN),
                                     g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_RECONN),
        strlen(STR_BT_AUDIO_RECONN),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_CONNECT_X,
                                     BT_AUDIO_CONNECT_Y + BT_AUDIO_ITEM_H,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_RECONN),
                                     strlen(STR_BT_AUDIO_RECONN),
                                     g_bt_audio_cntx.font_color);


    bt_audio_show_th_str();

    bt_audio_show_pxp_status();

    bt_audio_gdi_lcd_update_screen(0, 0, 239, 239);
}


static void bt_audio_show_play_screen()
{
    int32_t width = 0, height = 0;

    g_bt_audio_cntx.scr = BT_AUDIO_SCR_PLAYING;
    bt_audio_gdi_draw_solid_rect(0, 0, 239, 239, g_bt_audio_cntx.bg_color);

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_PLAY),
        strlen(STR_BT_AUDIO_PLAY),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_MUSIC_X,
                                     BT_AUDIO_MUSIC_Y,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_PLAY),
                                     strlen(STR_BT_AUDIO_PLAY),
                                     g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_NEXT),
        strlen(STR_BT_AUDIO_NEXT),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_MUSIC_X,
                                     BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_NEXT),
                                     strlen(STR_BT_AUDIO_NEXT),
                                     g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_PRE),
        strlen(STR_BT_AUDIO_PRE),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_MUSIC_X,
                                     BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 2,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_PRE),
                                     strlen(STR_BT_AUDIO_PRE),
                                     g_bt_audio_cntx.font_color);

    bt_audio_show_th_str();

    bt_audio_show_pxp_status();

    bt_audio_gdi_lcd_update_screen(0, 0, 239, 239);
}


static void bt_audio_show_stop_screen()
{
    int32_t width = 0, height = 0;

    g_bt_audio_cntx.scr = BT_AUDIO_SCR_STOP;
    bt_audio_gdi_draw_solid_rect(0, 0, 239, 239, g_bt_audio_cntx.bg_color);

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_STOP),
        strlen(STR_BT_AUDIO_STOP),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_MUSIC_X,
                                     BT_AUDIO_MUSIC_Y,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_STOP),
                                     strlen(STR_BT_AUDIO_STOP),
                                     g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_NEXT),
        strlen(STR_BT_AUDIO_NEXT),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_MUSIC_X,
                                     BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_NEXT),
                                     strlen(STR_BT_AUDIO_NEXT),
                                     g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_PRE),
        strlen(STR_BT_AUDIO_PRE),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_MUSIC_X,
                                     BT_AUDIO_MUSIC_Y + BT_AUDIO_ITEM_H * 2,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_PRE),
                                     strlen(STR_BT_AUDIO_PRE),
                                     g_bt_audio_cntx.font_color);

    bt_audio_show_th_str();

    bt_audio_show_pxp_status();

    bt_audio_gdi_lcd_update_screen(0, 0, 239, 239);
}


static void bt_audio_show_incoming_screen(bt_audio_caller_t *num)
{
    int32_t width = 0, height = 0;

    g_bt_audio_cntx.scr = BT_AUDIO_SCR_INCOMING;
    bt_audio_gdi_draw_solid_rect(0, 0, 239, 239, g_bt_audio_cntx.bg_color);

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_INCOMING),
        strlen(STR_BT_AUDIO_INCOMING),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_CALL_X,
                                     BT_AUDIO_CALL_Y,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_INCOMING),
                                     strlen(STR_BT_AUDIO_INCOMING),
                                     g_bt_audio_cntx.font_color);

    if (num->num_len > 0 || num->name_len > 0) {
        uint8_t *display_name;
        uint8_t length;

        if (num->name_len > 0) {
            display_name = num->name;
            length = num->name_len;
        } else {
            display_name = num->num;
            length = num->num_len;
        }

        font_engine_get_string_width_height(
            display_name,
            length,
            &width,
            &height);

        bt_audio_font_engine_show_string(BT_AUDIO_CALL_X + BT_AUDIO_CALL_GAP,
                                         BT_AUDIO_CALL_Y + BT_AUDIO_ITEM_H,
                                         bt_audio_convert_string_to_wstring((char *)display_name),
                                         length,
                                         g_bt_audio_cntx.font_color);
    }
    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_ANSWER),
        strlen(STR_BT_AUDIO_ANSWER),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_CALL_X,
                                     BT_AUDIO_CALL_Y + BT_AUDIO_ITEM_H * 2,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_ANSWER),
                                     strlen(STR_BT_AUDIO_ANSWER),
                                     g_bt_audio_cntx.font_color);


    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_REJECT),
        strlen(STR_BT_AUDIO_REJECT),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_CALL_X,
                                     BT_AUDIO_CALL_Y + BT_AUDIO_ITEM_H * 3,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_REJECT),
                                     strlen(STR_BT_AUDIO_REJECT),
                                     g_bt_audio_cntx.font_color);

    bt_audio_show_th_str();

    bt_audio_show_pxp_status();

    bt_audio_gdi_lcd_update_screen(0, 0, 239, 239);
}


static void bt_audio_show_calling_screen()
{
    int32_t width = 0, height = 0;

    g_bt_audio_cntx.scr = BT_AUDIO_SCR_CALLING;
    bt_audio_gdi_draw_solid_rect(0, 0, 239, 239, g_bt_audio_cntx.bg_color);

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_DROP),
        strlen(STR_BT_AUDIO_DROP),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_CALL_DROP_X,
                                     BT_AUDIO_CALL_DROP_Y,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_DROP),
                                     strlen(STR_BT_AUDIO_DROP),
                                     g_bt_audio_cntx.font_color);

    font_engine_get_string_width_height(
        bt_audio_convert_string_to_wstring(STR_BT_AUDIO_SWITCH_PATH),
        strlen(STR_BT_AUDIO_SWITCH_PATH),
        &width,
        &height);

    bt_audio_font_engine_show_string(BT_AUDIO_CALL_DROP_X,
                                     BT_AUDIO_CALL_DROP_Y + BT_AUDIO_ITEM_H,
                                     bt_audio_convert_string_to_wstring(STR_BT_AUDIO_SWITCH_PATH),
                                     strlen(STR_BT_AUDIO_SWITCH_PATH),
                                     g_bt_audio_cntx.font_color);


    bt_audio_show_th_str();

    bt_audio_show_pxp_status();

    bt_audio_gdi_lcd_update_screen(0, 0, 239, 239);
}


static void bt_audio_show_screen(bt_audio_screen_t scr, void *param)
{
    g_bt_audio_cntx.bg_screen = scr;
    g_bt_audio_cntx.entered = true;
    demo_ui_register_touch_event_callback(bt_audio_pen_event_handler, NULL);

    switch (scr) {
        case BT_AUDIO_SCR_IDLE:
            bt_audio_show_idle_screen();
            break;

        case BT_AUDIO_SCR_PLAYING:
            bt_audio_show_play_screen();
            break;

        case BT_AUDIO_SCR_STOP:
            bt_audio_show_stop_screen();
            break;

        case BT_AUDIO_SCR_INCOMING:
            bt_audio_show_incoming_screen((bt_audio_caller_t *)param);
            break;

        case BT_AUDIO_SCR_CALLING:
            bt_audio_show_calling_screen();
            break;

        default:
            break;
    }
}


bool bt_audio_is_point_range(bt_audio_point_t *point, bt_audio_point_t *l_corner, bt_audio_point_t *r_corner)
{
    bool ret = false;
    double tmp = 0.0;
    double factor = 1.0;
    double skip = 0.5;
    int32_t x1 = 0, y1 = 0, x2 = 0, y2 = 0;

    LOG_I(bt_audio, "[bt_audio]point_range(enter), x1: %d, y1: %d, x2: %d, y2: %d\n",
          l_corner->x, l_corner->y, r_corner->x, r_corner->y);

    if (g_bt_audio_cntx.lcd_size == 0) {
    } else if (g_bt_audio_cntx.lcd_size == 1) {
        factor = 320.0 / 240;
        tmp = (double)floor(l_corner->x * factor + skip);
        x1 = (int32_t) tmp;

        tmp = floor(l_corner->y * factor + skip);
        y1 = (int32_t) tmp;

        tmp = floor(r_corner->x * factor + skip);
        x2 = (int32_t) tmp;

        tmp = floor(r_corner->y * factor + skip);
        y2 = (int32_t) tmp;

        l_corner->x = x1;
        l_corner->y = y1;
        r_corner->x = x2;
        r_corner->y = y2;
    }

    if (point->x >= l_corner->x && point->x <= r_corner->x &&
            point->y >= l_corner->y && point->y <= r_corner->y) {
        ret = true;
    }

    LOG_I(bt_audio, "[bt_audio]point_range(end), x1: %d, y1: %d, x2: %d, y2: %d\n",
          l_corner->x, l_corner->y, r_corner->x, r_corner->y);

    return ret;
}



void bt_audio_event_handler(message_id_enum event_id, int32_t param1, void *param2)
{
    LOG_I(bt_audio, "[bt_audio]event_handler--bt: %d, eid: %d, p1: %d, p2: 0x%x\n",
          MESSAGE_ID_BT_AUDIO, event_id, param1, param2);

    if (event_id == MESSAGE_ID_BT_AUDIO) {
        bt_audio_show_screen((bt_audio_screen_t)param1, param2);
    }
}

#ifdef __BT_AUDIO_DEMO_WITH_PXP__
bt_sink_srv_status_t bt_audio_pxp_event_hdr(bt_sink_srv_event_t event_id, void *parameters)
{
    g_bt_audio_cntx.pxp_status = event_id;
    // Redraw to update
    ui_send_event(MESSAGE_ID_BT_AUDIO, g_bt_audio_cntx.bg_screen, NULL);
    return BT_SINK_SRV_STATUS_SUCCESS;
}
#endif /* __BT_AUDIO_DEMO_WITH_PXP__ */

bt_sink_srv_status_t bt_audio_sink_event_hdr(bt_sink_srv_event_t event_id, void *parameters)
{
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)parameters;

    LOG_I(bt_audio, "[bt_audio] event_id:0x%x", event_id);

    switch (event_id) {
        case BT_SINK_SRV_EVENT_STATE_CHANGE:
            if (event->state_change.now & BT_SINK_SRV_STATE_STREAMING) {
                ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_STOP, NULL);
            } else if (event->state_change.now & BT_SINK_SRV_STATE_INCOMING) {
                ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_INCOMING, (void *)(&g_bt_audio_cntx.caller));
            } else if (event->state_change.now & BT_SINK_SRV_STATE_ACTIVE) {
                ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_CALLING, NULL);
            } else if ((event->state_change.now & BT_SINK_SRV_STATE_CONNECTED)
                       && (g_bt_audio_cntx.profile_type == (bt_sink_srv_profile_type_t)
                           (BT_SINK_SRV_TYPE_HFP | BT_SINK_SRV_TYPE_A2DP | BT_SINK_SRV_TYPE_AVRCP))) {
                g_bt_audio_cntx.caller.num_len = 0;
                g_bt_audio_cntx.caller.name_len = 0;
                ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_PLAYING, NULL);
            } else if (BT_SINK_SRV_STATE_NONE == event->state_change.previous &&
                    event->state_change.now & BT_SINK_SRV_STATE_POWER_ON) {
                ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_IDLE, NULL);
            } else {
                ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_IDLE, NULL);
            }
            break;

        case BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE:
            if (event->connection_info.profile_type == (bt_sink_srv_profile_type_t)
                    (BT_SINK_SRV_TYPE_HFP | BT_SINK_SRV_TYPE_A2DP | BT_SINK_SRV_TYPE_AVRCP)) {
                memcpy(&g_bt_audio_cntx.reconnect_addr, &event->connection_info.bt_addr, sizeof(bt_bd_addr_t));
                if (BT_AUDIO_SCR_IDLE == g_bt_audio_cntx.bg_screen) { // if smart phone is in-call when connection
                    ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_PLAYING, NULL);
                }
            } else if (event->connection_info.profile_type == BT_SINK_SRV_TYPE_NONE) {
                ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_IDLE, NULL);
            }
            g_bt_audio_cntx.profile_type = event->connection_info.profile_type;
            break;

        case BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION:
            if (0 == g_bt_audio_cntx.caller.num_len
                    || (0 == g_bt_audio_cntx.caller.name_len && 0 != strlen((char *)event->caller_info.name))) {
                g_bt_audio_cntx.caller.num_len = event->caller_info.num_size;
                memcpy(g_bt_audio_cntx.caller.num, event->caller_info.number, BT_AUDIO_MAX_NUM_LEN);
                g_bt_audio_cntx.caller.name_len = strlen((char *)event->caller_info.name);
                memcpy(g_bt_audio_cntx.caller.name, event->caller_info.name, BT_AUDIO_MAX_NUM_LEN);
                ui_send_event(MESSAGE_ID_BT_AUDIO, BT_AUDIO_SCR_INCOMING, (void *)(&g_bt_audio_cntx.caller));
            }
            break;

        default:
            break;
    }
    return BT_SINK_SRV_STATUS_SUCCESS;
}

static void bt_audio_init()
{
    gdi_font_engine_color_t text_color = {0, 255, 255, 255};//white color

#ifdef __BT_AUDIO_DEMO_WITH_PXP__
    bt_sink_app_event_register_callback(BT_SINK_EVENT_APP_EXT_PXP_CONNECT, bt_audio_pxp_event_hdr);
    bt_sink_app_event_register_callback(BT_SINK_EVENT_APP_EXT_PXP_DISCONNECT, bt_audio_pxp_event_hdr);
    bt_sink_app_event_register_callback(BT_SINK_EVENT_APP_EXT_PXP_ALERT, bt_audio_pxp_event_hdr);
#endif /* __BT_AUDIO_DEMO_WITH_PXP__ */

    //reconnect address should not be reset because reconnect feature.
    bt_bd_addr_t previous_addr = {0};
    if (0 != memcmp(&previous_addr, &g_bt_audio_cntx.reconnect_addr, sizeof(bt_bd_addr_t))) {
        memcpy(&previous_addr, &g_bt_audio_cntx.reconnect_addr, sizeof(bt_bd_addr_t));
    }
    memset(&g_bt_audio_cntx, 0, sizeof(g_bt_audio_cntx));
    memcpy(&g_bt_audio_cntx.reconnect_addr,&previous_addr, sizeof(bt_bd_addr_t));

    g_bt_audio_cntx.bg_color = 0;
    g_bt_audio_cntx.font_color = text_color;
    g_bt_audio_cntx.font = GDI_FONT_ENGINE_FONT_MEDIUM;
    //g_bt_audio_cntx.entered = true;
    bt_sink_app_event_register_callback(BT_SINK_SRV_EVENT_ALL, bt_audio_sink_event_hdr);
    //demo_ui_register_touch_event_callback(bt_audio_pen_event_handler, NULL);
}

void show_bt_audio_screen(void)
{
    uint32_t height = 0, width = 0;

    bt_audio_init();
#ifdef __BT_AUDIO_DEMO_WITH_PXP__
    ble_pxp_init();
#endif /* __BT_AUDIO_DEMO_WITH_PXP__ */
    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_HEIGHT, (void *)&height);

    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_WIDTH, (void *)&width);

    LOG_I(bt_audio, "[bt_audio]enter-h: %d, w: %d\n", height, width);

    if (height == 320 && width == 320) {
        g_bt_audio_cntx.lcd_size = 1;
    } else {
        /* default 240 * 240 */
        g_bt_audio_cntx.lcd_size = 0;
    }
    
    g_bt_audio_cntx.bg_screen = BT_AUDIO_SCR_IDLE;
    bt_audio_show_idle_screen(); 

#ifdef __BT_AUDIO_DEMO_WITH_PXP__
    bt_demo_power_on(BT_MODE_DUAL);
#else
    bt_demo_power_on(BT_MODE_AUDIO);
#endif /* __BT_AUDIO_DEMO_WITH_PXP__ */
}


void bt_audio_font_engine_show_string(int32_t x, int32_t y, uint8_t *string,
                                      int32_t len, gdi_font_engine_color_t font_color)
{
    double tmp = 0.0;
    double factor = 1.0;
    double skip = 0.5;
    gdi_font_engine_display_string_info_t string_info = {0};

    LOG_I(bt_audio, "[bt_audio]show_string(enter), x: %d, y: %d\n", x, y);

    if (g_bt_audio_cntx.lcd_size == 0) {
        ;
    } else if (g_bt_audio_cntx.lcd_size == 1) {
        factor = 320.0 / 240;
        tmp = floor(x * factor + skip);
        x = (int32_t) tmp;

        tmp = floor(y * factor + skip);
        y = (int32_t) tmp;
    }
    LOG_I(bt_audio, "[bt_audio]show_string(end), x: %d, y: %d\n", x, y);
    //font_engine_show_string(x, y, string, len, font_color);

    gdi_font_engine_set_font_size(GDI_FONT_ENGINE_FONT_MEDIUM);
    gdi_font_engine_set_text_color(font_color);

    string_info.x = x;
    string_info.y = y;
    string_info.baseline_height = -1;
    string_info.string = string;
    string_info.length = len;
    gdi_font_engine_display_string(&string_info);
}


void bt_audio_gdi_draw_solid_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                                  gdi_color_t rect_color)
{
    double tmp = 0.0;
    double factor = 1.0;
    double skip = 0.5;

    LOG_I(bt_audio, "[bt_audio]draw_solid_rect(enter), x1: %d, y1: %d, x2: %d, y2: %d\n",
          x1, y1, x2, y2);

    if (g_bt_audio_cntx.lcd_size == 0) {
        ;
    } else if (g_bt_audio_cntx.lcd_size == 1) {
        factor = 320.0 / 240;
        tmp = floor(x1 * factor + skip);
        x1 = (int32_t) tmp;

        tmp = floor(y1 * factor + skip);
        y1 = (int32_t) tmp;

        tmp = floor(x2 * factor + skip);
        x2 = (int32_t) tmp;

        tmp = floor(y2 * factor + skip);
        y2 = (int32_t) tmp;
    }

    LOG_I(bt_audio, "[bt_audio]draw_solid_rect(end), x1: %d, y1: %d, x2: %d, y2: %d\n",
          x1, y1, x2, y2);

    gdi_draw_filled_rectangle(x1, y1, x2, y2, rect_color);
}


gdi_result_t bt_audio_gdi_lcd_update_screen(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    double tmp = 0.0;
    double factor = 1.0;
    double skip = 0.5;

    LOG_I(bt_audio, "[bt_audio]update_screen(enter), x1: %d, y1: %d, x2: %d, y2: %d\n",
          x1, y1, x2, y2);

    if (g_bt_audio_cntx.lcd_size == 0) {
        ;
    } else if (g_bt_audio_cntx.lcd_size == 1) {
        factor = 320.0 / 240;
        tmp = floor(x1 * factor + skip);
        x1 = (int32_t) tmp;

        tmp = floor(y1 * factor + skip);
        y1 = (int32_t) tmp;

        tmp = floor(x2 * factor + skip);
        x2 = (int32_t) tmp;

        tmp = floor(y2 * factor + skip);
        y2 = (int32_t) tmp;
    }

    LOG_I(bt_audio, "[bt_audio]update_screen(end), x1: %d, y1: %d, x2: %d, y2: %d\n",
          x1, y1, x2, y2);

    return gdi_lcd_update_screen(x1, y1, x2, y2);
}


static void font_engine_get_string_width_height(uint8_t *string, int32_t len,
    int32_t *width, int32_t *height)
{
    *width = 1;
    *height = 1;
}

