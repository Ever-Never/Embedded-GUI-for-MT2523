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
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <string.h>
#include <stdio.h>
#include "syslog.h"
#include <stdlib.h>
#include <assert.h>

#include "hal_log.h"
#include "task_def.h"
#include "hal_gpt.h"

#include "sct_key_event_internal.h"

QueueHandle_t sct_key_event_queue_handle;
TaskHandle_t  sct_key_event_task_handle;

sct_key_event_context_t sct_key_event_context;
sct_key_event_state_t   sct_key_event_state;
sct_key_event_time_t    sct_key_event_time;
    
uint32_t sct_key_driven_signal_time = SCT_KEY_EVENT_DRIVEN_SIGNAL_TIME;


static void sct_key_multiple_press_process(uint8_t *key_type);


void sct_key_send_message(sct_key_msg_type_t msg, uint8_t *key_index)
{
    sct_key_msg_t msgs;
    BaseType_t xHigherPriorityTaskWoken;

    /* We have not woken a task at the start of the ISR*/
    xHigherPriorityTaskWoken = pdFALSE;

    msgs.msg_id    = msg;
    msgs.key_index = *key_index;
    
    while (xQueueSendFromISR(sct_key_event_queue_handle, &msgs, &xHigherPriorityTaskWoken) != pdTRUE);

    /* Now the buffer is empty we can switch context if necessary.*/
    if (xHigherPriorityTaskWoken) {
        /*Actual macro used here is port specific.*/
        portYIELD_FROM_ISR(pdTRUE);
    }
}

static uint8_t sct_key_get_key_name(uint8_t index, bool is_mapping_tale)
{
    /*get registered key name*/
    if (is_mapping_tale == true) {
        #ifdef MTK_KEYPAD_ENABLE
        return keypad_custom_translate_keydata(index);
        #else
        return DEVICE_KEY_NONE;
        #endif
    }
    else {
        return sct_key_event_state.key_data;
    }

    
}

static void sct_key_allocate_timer(uint8_t index)
{
    hal_gpt_status_t ret_state;

    ret_state = hal_gpt_sw_get_timer(&sct_key_event_state.timer_handle[index]);
    
    if (ret_state != HAL_GPT_STATUS_OK) {
        log_hal_info("[key][sct]get timer handle error,ret = %d, handle = 0x%x\r\n", \
                     (unsigned int)ret_state, \
                     (unsigned int)sct_key_event_state.timer_handle[index]);
    }
}
static void sct_key_start_timer(uint8_t index, uint32_t time)
{
    hal_gpt_status_t ret_state;
        
    /*start timer*/
    ret_state = hal_gpt_sw_start_timer_ms(sct_key_event_state.timer_handle[index], \
                                          time, \
                                          (hal_gpt_callback_t)sct_key_multiple_press_process, \
                                          (void *)(&sct_key_event_state.position[index]));
    if (ret_state != HAL_GPT_STATUS_OK) {
        log_hal_info("[key][sct]start timer handle error,ret = 0x%x, sate=%d\r\n", ret_state, sct_key_event_state.sct_state[index]);
    }   

}

static void sct_key_stop_timer(uint8_t index)
{
    hal_gpt_status_t ret_state;
        
    ret_state = hal_gpt_sw_stop_timer_ms(sct_key_event_state.timer_handle[index]);
    if (ret_state != HAL_GPT_STATUS_OK) {
        log_hal_info("[key][sct]stop timer handle error,ret = %d, handle = 0x%x\r\n", \
                     (int)ret_state, \
                     (int)sct_key_event_state.timer_handle[index]);
    }   
}

static void sct_key_free_timer(uint8_t index)
{
    hal_gpt_status_t ret_state;
        
    /*free timer*/
    ret_state = hal_gpt_sw_free_timer(sct_key_event_state.timer_handle[index]);
    if (ret_state != HAL_GPT_STATUS_OK) {
        log_hal_info("[key][sct]free timer handle error,ret = %d, handle = 0x%x\r\n", \
                     (int)ret_state, \
                     (int)sct_key_event_state.timer_handle[index]);
    }
}

static void sct_key_multiple_press_process(uint8_t *key_type)
{
    uint8_t index;

    index = *key_type;
    
    sct_key_send_message(SCT_CALL_CALLBACK, key_type);
    sct_key_free_timer(index);
}


static bool sct_key_event_mapping(sct_key_msg_type_t msg_type, sct_key_mapping_event_t *key_event)
{
    #ifdef MTK_KEYPAD_ENABLE
    hal_keypad_event_t      hal_key_event;
    #ifdef HAL_KEYPAD_FEATURE_POWERKEY 
    hal_keypad_powerkey_event_t hal_powerkey_event;
    #endif /*HAL_KEYPAD_FEATURE_POWERKEY*/
    #endif /*MTK_KEYPAD_ENABLE*/

    #ifdef MTK_KEYPAD_ENABLE
    if (msg_type == SCT_GET_KEY_DATA_KEYPAD) {
        if (hal_keypad_get_key(&hal_key_event) == HAL_KEYPAD_STATUS_ERROR) {
            return false;
        }
        key_event->state        = (sct_key_driven_t)hal_key_event.state;
        key_event->key_data     = hal_key_event.key_data;
        key_event->time_stamp   = hal_key_event.time_stamp;

        return true;
    }

    #ifdef HAL_KEYPAD_FEATURE_POWERKEY
    if (msg_type == SCT_GET_KEY_DATA_POWERKEY) {
        if (hal_keypad_powerkey_get_key(&hal_powerkey_event) == HAL_KEYPAD_STATUS_ERROR){
            return false;
        }
        key_event->state        = (sct_key_driven_t)hal_powerkey_event.state;
        key_event->key_data     = hal_powerkey_event.key_data;
        key_event->time_stamp   = hal_powerkey_event.time_stamp;

        return true;
    }
    #endif /*HAL_KEYPAD_FEATURE_POWERKEY*/
    #endif /*MTK_KEYPAD_ENABLE*/

    #ifdef MTK_EINT_KEY_ENABLE
    if (msg_type == SCT_GET_KEY_DATA_EINT) {
        if (sct_eint_key_event.is_update == false){
            return false;
        }
        key_event->state        = sct_eint_key_event.state;
        key_event->key_data     = sct_eint_key_event.key_data ;
        key_event->time_stamp   = sct_eint_key_event.time_stamp;

        sct_eint_key_event.is_update = false;

        return true;
    }   

    #endif

    
    return false;
    
}


static void sct_key_process_key(sct_key_mapping_event_t *key_event, bool is_mapping_tale)
{
    uint8_t  index;
    uint8_t  key_name;
    uint32_t time_ms;
    uint32_t duration_count;
    uint32_t interval_time = 0;
    uint32_t event_time[3];
    
    static bool is_event_send[3];
    static uint32_t repeat_count;

    /*check powerkey or keypad index number*/
    if (is_mapping_tale == false) {
        #ifdef MTK_EINT_KEY_ENABLE
        if (key_event->key_data == BSP_EINT_KEY_DATA0) {
            index = SCT_KEY_KEYPAD_NUMBER + 1;
        }
        else if(key_event->key_data == BSP_EINT_KEY_DATA1){
            index = SCT_KEY_KEYPAD_NUMBER + 2;
        }
        else if(key_event->key_data == BSP_EINT_KEY_DATA2) {
            index = SCT_KEY_KEYPAD_NUMBER + 3;
        }
        else {
            index = SCT_KEY_KEYPAD_NUMBER;
        }
        #else
            index = SCT_KEY_KEYPAD_NUMBER;  
        #endif
        
        sct_key_event_state.key_data = key_event->key_data;
    }
    else {
        index = key_event->key_data;
    }
    
    sct_key_event_state.position[index]    = index;
    sct_key_event_state.is_mapping_tale    = is_mapping_tale;


    /*first press processing*/
    if (key_event->state == SCT_KEY_DRIVEN_PRESS) {
        sct_key_event_state.time_stamp[index] = key_event->time_stamp;
        sct_key_event_state.key_state[index] = SCT_KEY_DRIVEN_PRESS;
        
        is_event_send[0] = false;
        is_event_send[1] = false;
        is_event_send[2] = false;
        repeat_count     = 0;

        log_hal_info("[key][sct]sct_state_1 = %d \r\n", sct_key_event_state.sct_state[index]);
        
        if (sct_key_event_state.sct_state[index] == SCT_KEY_RELEASE) {
            /*The first press, allocate timer for short/double/triple press check*/
            sct_key_allocate_timer(index);
        }
        else {
            /*release key should stop timer*/
            sct_key_stop_timer(index);
        }
        
        return;
    }

    /*get duration time and switch to milliseconds*/
    hal_gpt_get_duration_count(sct_key_event_state.time_stamp[index],key_event->time_stamp, &duration_count);
    time_ms = (((duration_count*1000)+16384)/32768);

    sct_key_event_state.press_time = time_ms;
    log_hal_info("[key][sct]press time %d \r\n", sct_key_event_state.press_time);
    
    /*get key symbol name*/
    key_name = sct_key_get_key_name(index,is_mapping_tale);
    
    
    /*check release and short press*/
    if (key_event->state == SCT_KEY_DRIVEN_RELEASE) {
        sct_key_event_state.time_stamp[index] = 0;
        sct_key_event_state.key_state[index]  = SCT_KEY_DRIVEN_RELEASE;
        
        if (time_ms <= sct_key_event_time.short_time) {

            /*multiple press state machine*/
            if (sct_key_event_state.sct_state[index] == SCT_KEY_RELEASE) {
                
                /*short press processing*/
                sct_key_event_state.sct_state[index] = SCT_KEY_SHORT_PRESS;
                interval_time = sct_key_event_time.double_interval_time;
            }
            else if (sct_key_event_state.sct_state[index] == SCT_KEY_SHORT_PRESS){
                
                /*double press processing*/
                sct_key_event_state.sct_state[index] = SCT_KEY_DOUBLE_PRESS;
                interval_time = sct_key_event_time.triple_interval_time;
            }
            else if(sct_key_event_state.sct_state[index] == SCT_KEY_DOUBLE_PRESS ){
                
                /*triple press processing*/
                sct_key_free_timer(index);
                sct_key_event_state.sct_state[index] = SCT_KEY_TRIPLE_PRESS;
                sct_key_event_context.callback(sct_key_event_state.sct_state[index], key_name, sct_key_event_context.user_data);
                sct_key_event_state.sct_state[index] = SCT_KEY_RELEASE;
                return;
            }
            else {
                assert(0);
            }

            sct_key_start_timer(index, interval_time);

        }
        else {
            sct_key_free_timer(index);
        }

        sct_key_event_context.callback(SCT_KEY_RELEASE, key_name, sct_key_event_context.user_data);
        
        return;
    }

    //log_hal_info("<-----------------\r\ncheck longpress and repeat: %d \r\n", time_ms);


    /*check repeat event*/
    if ((time_ms >= sct_key_event_time.repeat_time) && ((time_ms/sct_key_event_time.repeat_time) != repeat_count)) {
        sct_key_event_context.callback(SCT_KEY_REPEAT, key_name, sct_key_event_context.user_data);
        repeat_count = time_ms/sct_key_event_time.repeat_time;
    }

    
    /*long press upper limit*/
    event_time[0] = sct_key_event_time.longpress_time_2;
    event_time[1] = sct_key_event_time.longpress_time_3;

    

    /*check longpress event*/
    if ((time_ms >= sct_key_event_time.longpress_time_1) && (time_ms < event_time[0]) && (is_event_send[0] == false)) {
        sct_key_event_context.callback(SCT_KEY_LONG_PRESS_1 ,key_name, sct_key_event_context.user_data);
        sct_key_event_state.sct_state[index] = SCT_KEY_RELEASE;
        is_event_send[0] = true;
    }
    else if ((time_ms >= sct_key_event_time.longpress_time_2) && (time_ms < event_time[1])&& (is_event_send[1] == false)) {
        sct_key_event_context.callback(SCT_KEY_LONG_PRESS_2, key_name, sct_key_event_context.user_data);
        sct_key_event_state.sct_state[index] = SCT_KEY_RELEASE;
        is_event_send[1] = true;
    }
    else if ((time_ms >= sct_key_event_time.longpress_time_3) && (is_event_send[2] == false)) {
        sct_key_event_context.callback(SCT_KEY_LONG_PRESS_3, key_name, sct_key_event_context.user_data);
        sct_key_event_state.sct_state[index] = SCT_KEY_RELEASE;
        is_event_send[2] = true;
    }

    /*we can add more longpress event as above*/
    
}



void sct_key_task_main(void *pvParameters)
{
    sct_key_msg_t           msgs;
    sct_key_mapping_event_t key_event;

    
    static uint32_t         stack_max = 0;
    uint32_t                stack_cur = 0;

    uint8_t                 index;
    uint8_t                 key_name;
    sct_key_event_t         event;
    
    while (1) {
        if (xQueueReceive(sct_key_event_queue_handle, &msgs, portMAX_DELAY)) {
            switch (msgs.msg_id) {
                case SCT_GET_KEY_DATA_KEYPAD: {
                    while (1) {
                        if(sct_key_event_mapping(msgs.msg_id, &key_event) == false) {
                            break;
                        }
                        sct_key_process_key(&key_event, true);
                    }
                }
                break;

                case SCT_GET_KEY_DATA_POWERKEY: {
                    while (1) {
                        if(sct_key_event_mapping(msgs.msg_id, &key_event) == false) {
                            break;
                        }
                        sct_key_process_key(&key_event, false);
                    }
                }
                break;

                case SCT_GET_KEY_DATA_EINT: {
                    if(sct_key_event_mapping(msgs.msg_id, &key_event) == false) {
                        break;
                    }
                    sct_key_process_key(&key_event, false);
                }
                break;

                case SCT_CALL_CALLBACK: {
                    index    = msgs.key_index;
                    key_name = sct_key_get_key_name(index, sct_key_event_state.is_mapping_tale);

                    if (sct_key_event_state.sct_state[index] == SCT_KEY_SHORT_PRESS) {
                        event = SCT_KEY_SHORT_PRESS; 
                    }
                    else if (sct_key_event_state.sct_state[index] == SCT_KEY_DOUBLE_PRESS){
                        event = SCT_KEY_DOUBLE_PRESS;    
                    }
                    else {
                        log_hal_info("[key][sct]wrong event in timer callback]\r\n");
                        assert(0);
                    }
                    sct_key_event_state.sct_state[index] = SCT_KEY_RELEASE;
                    sct_key_event_context.callback(event, key_name, sct_key_event_context.user_data);
                }
                break;
                
                
                default: {

                } break;
            }
            
            stack_cur = (SCT_KEY_TASK_STACKSIZE / sizeof(StackType_t) - uxTaskGetStackHighWaterMark(sct_key_event_task_handle)) * sizeof(portSTACK_TYPE);
            if (stack_cur > stack_max) {
                stack_max = stack_cur;
                log_hal_info("key sct task max-usage:%d \r\n", stack_max);
            }
        }
    }

}


void sct_key_task_init(void)
{
     /*creat task*/
    sct_key_event_queue_handle   = xQueueCreate(SCT_KEY_QUEUE_LENGTH, sizeof(sct_key_msg_t));
    
    xTaskCreate( sct_key_task_main, 
                 SCT_KEY_TASK_NAME, 
                 SCT_KEY_TASK_STACKSIZE / sizeof(StackType_t), 
                 NULL, 
                 SCT_KEY_TASK_PRIO, 
                 &sct_key_event_task_handle);
}

