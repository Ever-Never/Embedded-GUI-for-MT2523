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
#ifndef __SCT_KEY_EVENT_H__
#define __SCT_KEY_EVENT_H__

#include "stdint.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
    SCT_KEY_RELEASE        = 0,        /**< A key is released. */    
    SCT_KEY_SHORT_PRESS    = 1,        /**< A key is released. */
    SCT_KEY_LONG_PRESS_1   = 2,        /**< A key is long pressed. */
    SCT_KEY_LONG_PRESS_2   = 3,        /**< A key is long pressed. */
    SCT_KEY_LONG_PRESS_3   = 4,        /**< A key is long pressed. */
    SCT_KEY_REPEAT         = 5,        /**< A key is repeat pressed. */
    SCT_KEY_DOUBLE_PRESS   = 6,        /**< A key is double click */
    SCT_KEY_TRIPLE_PRESS   = 7,        /**< A key is triple click. */
    SCT_KEY_PMU_LONG_PRESS = 8         /**< PMU hardware powerkey is long pressed. */
} sct_key_event_t;

typedef struct {
    uint32_t short_time;
    uint32_t longpress_time_1;
    uint32_t longpress_time_2;
    uint32_t longpress_time_3;
    uint32_t repeat_time;
    uint32_t double_interval_time;
    uint32_t triple_interval_time;
} sct_key_event_time_t;

typedef void (*sct_key_callback_t)(sct_key_event_t event, uint8_t key_data, void *user_data);


/********* varible extern *************/
extern sct_key_event_time_t    keypad_event_time;
extern uint32_t sct_key_driven_signal_time;

/******** funtion extern **************/
bool sct_key_event_init(void);
bool sct_key_register_callback(sct_key_callback_t callback, void *user_data);
bool sct_key_set_event_time(sct_key_event_time_t* event_time);
bool sct_key_set_driven_signal_time(uint32_t signal_time);
bool sct_key_set_debounce_time(uint32_t debounce);

#ifdef __cplusplus
}
#endif

#endif /* __SCT_KEY_EVENT_H__ */

