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

#ifndef __BT_SINK_APP_KEYPAD_H__
#define __BT_SINK_APP_KEYPAD_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "hal_keypad.h"
#include "sct_key_event.h"
#include "bt_sink_srv.h"
#include "bt_sink_app_event.h"

typedef struct {
    uint8_t value;
    bt_sink_srv_key_value_t sink_key;
} bt_sink_app_keypad_mapping_t;

typedef struct {
    uint8_t  value;              // FUNC, NEXT or PREV key
    sct_key_event_t event;       // Press, release, double click or triple event
} bt_sink_app_keypad_key_t;

typedef void (*bt_sink_app_keypad_post_result_t)(bt_sink_srv_event_t event_id, bt_sink_srv_status_t result, void *parameters);

void bt_sink_app_keypad_init(void);

void bt_sink_app_keypad_callback(sct_key_event_t event, uint8_t key_data, void *user_data);

void bt_sink_app_keypad_powerkey_callback(void *user_data);

void bt_sink_app_keypad_post_callback(bt_sink_srv_event_t event_id, bt_sink_srv_status_t result, void *parameters);

void bt_sink_app_keypad_post_event(bt_sink_srv_event_t key_event, bt_sink_app_keypad_key_t *key, bt_sink_app_keypad_post_result_t callback);

bt_sink_srv_status_t bt_sink_app_keypad_event_handler(bt_sink_srv_event_t event_id, void *parameters);

#endif /* __BT_SINK_APP_KEYPAD_H__ */

