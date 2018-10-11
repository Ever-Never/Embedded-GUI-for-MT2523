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



#ifndef __MAIN_SCREEN_H__
#define __MAIN_SCREEN_H__
#if defined(_MSC_VER)
#include "gdi_type_adaptor.h"
#else
#include "stdint.h"
#endif

#define ScrnWidth (320)
#define ScrnHeight (320)
extern uint32_t LCD_CURR_HEIGHT, LCD_CURR_WIDTH;

extern unsigned char frame_buffer[ScrnWidth*ScrnHeight*2];

#define GRAPHICLOG(fmt, args...)       LOG_I(GRAPHIC_TAG, "[GRAPHIC]"fmt, ##args)


typedef enum {
    TOUCH_EVENT_UP,
    TOUCH_EVENT_DOWN,
    TOUCH_EVENT_ABORT,
    TOUCH_EVENT_MAX
} touch_event_enum;

typedef struct touch_pos_struct {
    int16_t x;
    int16_t y;
} touch_pos_struct_t;

typedef struct touch_event_struct {
    touch_pos_struct_t position;
    touch_event_enum type;
} touch_event_struct_t;


#define CONFIG_MESSAGE_ID_ENUM
typedef enum{
    MESSAGE_ID_PEN_EVENT,
#include "screen_config.h"
} message_id_enum;
#undef CONFIG_MESSAGE_ID_ENUM

typedef void (*show_screen_proc_f)(void);

typedef void (*event_handle_func)(message_id_enum event_id, int32_t param1, void* param2);

typedef void (*touch_event_proc_func)(touch_event_struct_t* touch_event, void* user_data);


// go back to main screen
void show_main_screen(void);

// send event to your screen. can used this api to transfer data between tasks and so on.
int32_t ui_send_event(message_id_enum event_id, int32_t param1, void* param2);

// register pen event for your application.
void demo_ui_register_touch_event_callback(touch_event_proc_func proc_func, void* user_data);

extern void common_event_handler(message_id_enum event_id, int32_t param1, void* param2);

#endif

