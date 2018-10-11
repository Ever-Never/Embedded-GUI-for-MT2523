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

#ifndef __BT_SINK_APP_LED_INTERNAL_H__
#define __BT_SINK_APP_LED_INTERNAL_H__

#include "bt_sink_app_led.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

/* LED light States */
typedef uint8_t bt_sink_app_led_lighting_state_t;
#define BT_SINK_APP_LED_LIGHTING_STATE_IDLE               (0x00)
#define BT_SINK_APP_LED_LIGHTING_STATE_FLASH             (0x01)
#define BT_SINK_APP_LED_LIGHTING_STATE_INTERVAL    (0x02)

typedef uint8_t bt_sink_app_led_status_t;
#define BT_SINK_APP_LED_STATUS_OFF              (0x00)
#define BT_SINK_APP_LED_STATUS_ON                (0x01)

typedef enum {
    BT_SINK_APP_LED_MODE_NONE,               
    BT_SINK_APP_LED_MODE_LONG_ON,                
    BT_SINK_APP_LED_MODE_LONG_OFF,
    BT_SINK_APP_LED_MODE_FLASH_INFINITE,
    BT_SINK_APP_LED_MODE_FLASH_NORMAL,
    BT_SINK_APP_LED_MODE_PERIOD_INFINITE
} bt_sink_app_led_mode_t;

typedef struct {
    TimerHandle_t timer_hd;
    bt_sink_app_led_state_config_table_t*curr_config;
    bt_sink_app_led_mode_t mode;
    uint8_t remain_period_count;
    uint8_t remain_flash_count;
    bt_sink_app_led_lighting_state_t state;
    bt_sink_app_led_status_t status; 
} bt_sink_app_led_cntx_t;

const bt_sink_app_led_state_config_table_t *bt_sink_app_led_get_state_cfg_table(void);

#ifdef __cplusplus
}
#endif

#endif /* __BT_SINK_APP_LED_INTERNAL_H__ */

