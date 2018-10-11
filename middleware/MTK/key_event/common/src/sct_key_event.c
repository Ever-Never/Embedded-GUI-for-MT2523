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


#include <string.h>
#include "hal_log.h"
#include "sct_key_event_internal.h"

bool sct_key_event_init(void)
{
    if(sct_key_event_state.has_initilized == true) {
        log_hal_info("[key][sct]sct_key_event_init has been initilized\r\n"); 
        return false;
    }

    memset(&sct_key_event_state, 0, sizeof(sct_key_event_state_t));

    #ifdef __CFW_CONFIG_SUPPORT__
    sct_key_driven_signal_time = CFW_CFG_ITEM_VALUE(cfw_sct_key_driven_time);
    #endif
        
    #ifdef MTK_KEYPAD_ENABLE
    sct_keypad_init();
    #endif

    #ifdef MTK_EINT_KEY_ENABLE
    sct_eint_key_init();
    #endif

    #ifdef __CFW_CONFIG_SUPPORT__
    sct_key_event_time.short_time            = CFW_CFG_ITEM_VALUE(cfw_sct_key_short_time);
    sct_key_event_time.longpress_time_1      = CFW_CFG_ITEM_VALUE(cfw_sct_key_longpress_time_1);
    sct_key_event_time.longpress_time_2      = CFW_CFG_ITEM_VALUE(cfw_sct_key_longpress_time_2);
    sct_key_event_time.longpress_time_3      = CFW_CFG_ITEM_VALUE(cfw_sct_key_longpress_time_3);
    sct_key_event_time.repeat_time           = CFW_CFG_ITEM_VALUE(cfw_sct_key_repeat_time);
    sct_key_event_time.double_interval_time  = CFW_CFG_ITEM_VALUE(cfw_sct_key_double_time);
    sct_key_event_time.triple_interval_time  = CFW_CFG_ITEM_VALUE(cfw_sct_key_triple_time);
    #else 
    sct_key_event_time.short_time            = SCT_KEY_DEFAULT_SHORT_TIME;
    sct_key_event_time.longpress_time_1      = SCT_KEY_DEFAULT_LONGPRESS_TIME_1;
    sct_key_event_time.longpress_time_2      = SCT_KEY_DEFAULT_LONGPRESS_TIME_2;
    sct_key_event_time.longpress_time_3      = SCT_KEY_DEFAULT_LONGPRESS_TIME_3;
    sct_key_event_time.repeat_time           = SCT_KEY_DEFAULT_REPEAT_TIME;
    sct_key_event_time.double_interval_time  = SCT_KEY_DEFAULT_DOUBLE_INTERVAL_TIME;
    sct_key_event_time.triple_interval_time  = SCT_KEY_DEFAULT_TRIPLE_INTERVAL_TIME;
    #endif

    sct_key_task_init();
    
    return true;
    
}

bool sct_key_register_callback(sct_key_callback_t callback, void *user_data)
{
    if (callback == NULL) {
        return false;
    }

    sct_key_event_context.callback  = callback;
    sct_key_event_context.user_data = user_data;

    return true;
}

bool sct_key_set_event_time(sct_key_event_time_t* event_time)
{

    if (event_time == NULL) {
        log_hal_info("[key][sct]event_time is null\r\n");    
        return false;       
    }
    
    sct_key_event_time.short_time            = event_time->short_time;
    sct_key_event_time.longpress_time_1      = event_time->longpress_time_1;
    sct_key_event_time.longpress_time_2      = event_time->longpress_time_2;
    sct_key_event_time.longpress_time_3      = event_time->longpress_time_3;
    sct_key_event_time.repeat_time           = event_time->short_time;
    sct_key_event_time.double_interval_time  = event_time->double_interval_time;
    sct_key_event_time.triple_interval_time  = event_time->triple_interval_time;
    
    return true;
}

bool sct_key_set_driven_signal_time(uint32_t signal_time)
{
    #ifdef MTK_KEYPAD_ENABLE
    keypad_custom_event_time_t hal_event_time;
    #endif

    #ifdef MTK_EINT_KEY_ENABLE
    bsp_eint_key_config_t      bsp_event_time;
    #endif

    /*setting time must over than debounce time*/
    if (signal_time <= SCT_KEY_DEFAULT_DEBOUNCE_TIME) {
        log_hal_info("[key][sct]setting signal time %d < %d \r\n",signal_time, SCT_KEY_DEFAULT_DEBOUNCE_TIME);   
        return false;
    }

    sct_key_driven_signal_time    = signal_time;

    #ifdef MTK_KEYPAD_ENABLE    
    hal_event_time.longpress_time = signal_time;
    hal_event_time.repet_time     = signal_time;
    keypad_custom_set_event_time(&hal_event_time);
    #endif

    #ifdef MTK_EINT_KEY_ENABLE
    bsp_event_time.longpress_time = signal_time;
    bsp_event_time.repeat_time    = signal_time;
    bsp_eint_key_set_event_time(&bsp_event_time);
    #endif
    
    return true;
}


bool sct_key_set_debounce_time(uint32_t debounce)
{
    #ifdef MTK_KEYPAD_ENABLE
    if (hal_keypad_set_debounce(&debounce) != HAL_KEYPAD_STATUS_OK) {
        return false;
    }
    #endif

    #ifdef MTK_EINT_KEY_ENABLE
    bsp_eint_key_set_debounce_time(debounce);
    #endif

    return true;
}

