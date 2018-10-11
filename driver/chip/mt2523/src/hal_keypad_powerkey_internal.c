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

#include "hal_keypad.h"

#ifdef HAL_KEYPAD_FEATURE_POWERKEY
#include "hal_keypad_powerkey_internal.h"
#include "hal_pmu.h"
#include <assert.h>
#include "hal_log.h"
#include "hal_gpt.h"
#include "hal_gpt_internal.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_driver.h"

const static char *powerkey_lock_sleep_name = "Powerkey";
static uint8_t powerkey_lock_sleep_handle;

#define POWERKEY_GET_SLEEP_HANDLE() \
do{ \
    powerkey_lock_sleep_handle = hal_sleep_manager_set_sleep_handle(powerkey_lock_sleep_name); \
    if(powerkey_lock_sleep_handle == INVALID_SLEEP_HANDLE ) { \
        log_hal_error("[keypad][powerkey]:get sleep handle failed\r\n"); \
        return false; \
    } \
}while(0)

#define POWERKEY_RELEASE_SLEEP_HANDLE() \
do{ \
    hal_sleep_manager_release_sleep_handle(powerkey_lock_sleep_handle); \
}while(0)

#define POWERKEY_LOCK_SLEEP() \
do{ \
    hal_sleep_manager_lock_sleep(powerkey_lock_sleep_handle); \
}while(0)

#define POWERKEY_UNLOCK_SLEEP() \
do{ \
    hal_sleep_manager_unlock_sleep(powerkey_lock_sleep_handle); \
}while(0)

#else
#define POWERKEY_GET_SLEEP_HANDLE()
#define POWERKEY_RELEASE_SLEEP_HANDLE()
#define POWERKEY_LOCK_SLEEP()
#define POWERKEY_UNLOCK_SLEEP()
#endif


powerkey_buffer_t powerkey_buffer;
powerkey_state_t  powerkey_state;

void powerkey_push_one_key_to_buffer(hal_keypad_key_state_t state, uint32_t data)
{
    uint32_t time;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &time);
    
    powerkey_buffer.data[powerkey_buffer.write_index].state      = state;
    powerkey_buffer.data[powerkey_buffer.write_index].key_data   = data;
    powerkey_buffer.data[powerkey_buffer.write_index].time_stamp = time;
    
    powerkey_buffer.write_index++;
    powerkey_buffer.write_index &=  POWERKEY_BUFFER_SIZE - 1;
}

void powerkey_pop_one_key_from_buffer(hal_keypad_powerkey_event_t *key_event)
{
    key_event->state      = powerkey_buffer.data[powerkey_buffer.read_index].state;
    key_event->key_data   = powerkey_buffer.data[powerkey_buffer.read_index].key_data;
    key_event->time_stamp = powerkey_buffer.data[powerkey_buffer.read_index].time_stamp;
    powerkey_buffer.read_index++;
    powerkey_buffer.read_index &=  POWERKEY_BUFFER_SIZE - 1;
}

uint32_t powerkey_get_buffer_left_size(void)
{
    if (powerkey_buffer.write_index >= powerkey_buffer.read_index) {

        return (POWERKEY_BUFFER_SIZE - powerkey_buffer.write_index + powerkey_buffer.read_index);
    } else {
        return (powerkey_buffer.read_index - powerkey_buffer.write_index);

    }
}

uint32_t powerkey_get_buffer_data_size(void)
{
    return (POWERKEY_BUFFER_SIZE - powerkey_get_buffer_left_size());

}
static void powerkey_call_user_callback(void)
{
    powerkey_callback_context_t *context;

    context = &powerkey_context.powerkey_callback;

    if (powerkey_context.has_initilized != true) {
        return;
    }

    if (context->callback != NULL) {
        log_hal_info("[keypad][powerkey]inform user\r\n");
        context->callback(context->user_data);
    } else {
        log_hal_info("[keypad][powerkey]callback is NULL!\r\n");
    }

}


void powerkey_process_repeat_and_longpress(uint32_t *powerkey_type)
{
    hal_gpt_status_t ret_state;

    assert(*powerkey_type == powerkey_context.registerd_data);

    if (powerkey_get_buffer_left_size() > (powerkey_buffer.press_count + 1)) {
        if (powerkey_state.current_state == HAL_KEYPAD_KEY_PRESS) {
            powerkey_state.current_state = HAL_KEYPAD_KEY_LONG_PRESS;
        } else {
            powerkey_state.current_state = HAL_KEYPAD_KEY_REPEAT;
        }

        powerkey_push_one_key_to_buffer(powerkey_state.current_state, powerkey_context.registerd_data);

        /*start timer*/
        ret_state = hal_gpt_sw_start_timer_ms(\
                                              powerkey_state.timer_handle, \
                                              powerkey_context.repeat_time, \
                                              (hal_gpt_callback_t)powerkey_process_repeat_and_longpress, \
                                              (void *)(&powerkey_context.registerd_data));
        if (ret_state != HAL_GPT_STATUS_OK) {
            log_hal_info("[keypad][powerkey]start timer error,ret = %d, handle = 0x%x\r\n", (int)ret_state, (int)powerkey_state.timer_handle);
        }

        log_hal_info("[keypad][powerkey]key state = %d, key_data = 0x%x\r\n", (int)powerkey_state.current_state, (int)powerkey_context.registerd_data);
        powerkey_call_user_callback();
    }

}


static void powerkey_process_key(powerkey_pmu_status_t pmu_state, uint32_t trigger_source, powerkey_context_t *countext)
{
    hal_keypad_key_state_t key_state[2];
    uint32_t pmu_key_flag[2];
    uint32_t status;
    hal_gpt_status_t ret_state;

    if (powerkey_context.is_pmu_init != true) {
        return;
    }

    status = (uint32_t)trigger_source;


    if ((pmu_state == POWERKEY_PMU_LONG_POWERKEY)) {
        key_state[0]    = HAL_KEYPAD_KEY_PMU_LONG_PRESS;
        key_state[1]    = HAL_KEYPAD_KEY_RELEASE;
        pmu_key_flag[0] = POWERKEY_STATUS_LONG_PRESS;
        pmu_key_flag[1] = POWERKEY_STATUS_RELEASE;
    } else {
        key_state[0]    = HAL_KEYPAD_KEY_PRESS;
        key_state[1]    = HAL_KEYPAD_KEY_RELEASE;
        pmu_key_flag[0] = POWERKEY_STATUS_PRESS;
        pmu_key_flag[1] = POWERKEY_STATUS_RELEASE;
    }

    /*press*/
    if ((status & pmu_key_flag[0]) != 0) {
        POWERKEY_LOCK_SLEEP();
        if (powerkey_get_buffer_left_size() > (powerkey_buffer.press_count + 1)) {

            powerkey_state.current_state = key_state[0];

            /* if press ,start timer to check normal longpress and repeat*/
            if (key_state[0] == HAL_KEYPAD_KEY_PRESS) {
                //log_hal_info("[keypad][powerkey]allocate sw gpt\r\n");
                /*allocate timer*/
                ret_state = hal_gpt_sw_get_timer(&powerkey_state.timer_handle);
                if (ret_state != HAL_GPT_STATUS_OK) {
                    log_hal_info("[keypad][powerkey]get timer handle error,ret = %d, handle = 0x%x\r\n", \
                                 (unsigned int)ret_state, \
                                 (unsigned int)powerkey_state.timer_handle);
                }

                /*start timer*/
                ret_state = hal_gpt_sw_start_timer_ms(\
                                                      powerkey_state.timer_handle, \
                                                      powerkey_context.longpress_time, \
                                                      (hal_gpt_callback_t)powerkey_process_repeat_and_longpress, \
                                                      (void *)(&powerkey_context.registerd_data));
                if (ret_state != HAL_GPT_STATUS_OK) {
                    log_hal_info("[keypad][powerkey]start timer error,ret = %d, handle = 0x%x\r\n", (int)ret_state, (int)powerkey_state.timer_handle);
                }
            }

            powerkey_buffer.press_count++;
            powerkey_context.release_count = 0;
            powerkey_push_one_key_to_buffer(powerkey_state.current_state, powerkey_context.registerd_data);
            log_hal_info("[keypad]powerkey pmu_state=%d PRESS\r\n", pmu_state);
            powerkey_call_user_callback();

        }
    }

    /*release*/
    if ((status & pmu_key_flag[1]) != 0) {
        if (powerkey_get_buffer_left_size() > 0) {
            if (powerkey_buffer.press_count == 0) {
                log_hal_info("[keypad][powerkey] no press event\r\n");
                return;
            }
            powerkey_context.release_count++;
            /*after pmu press and longpress, also trigger interrupt, sometimes trigger twice, only send once release*/
            if (powerkey_context.release_count >= 2) {
                return;
            }

            powerkey_state.current_state = key_state[1];
            /*release key should stop timer*/
            ret_state = hal_gpt_sw_stop_timer_ms(powerkey_state.timer_handle);
            if (ret_state != HAL_GPT_STATUS_OK) {
                log_hal_info("[keypad][state]stop timer handle error,ret = %d, handle = 0x%x\r\n", \
                             (int)ret_state, \
                             (int)powerkey_state.timer_handle);
            }

            /*free timer*/
            ret_state = hal_gpt_sw_free_timer(powerkey_state.timer_handle);
            if (ret_state != HAL_GPT_STATUS_OK) {
                log_hal_info("[keypad][state]free timer handle error,ret = %d, handle = 0x%x\r\n", \
                             (int)ret_state, \
                             (int)powerkey_state.timer_handle);
            }

            powerkey_buffer.press_count--;
            powerkey_push_one_key_to_buffer(powerkey_state.current_state , powerkey_context.registerd_data);
            log_hal_info("[keypad]powerkey pmu_state=%d RELEASE\r\n", pmu_state);
            powerkey_call_user_callback();
            POWERKEY_UNLOCK_SLEEP();
        }
    }

}

void powerkey_long_press_handler(uint8_t trigger_source, powerkey_context_t *countext)
{
    powerkey_process_key(POWERKEY_PMU_LONG_POWERKEY, (uint32_t)trigger_source, countext);

}

void powerkey_press_handler(uint8_t trigger_source, powerkey_context_t *countext)
{
    powerkey_process_key(POWERKEY_PMU_POWRKEY, (uint32_t)trigger_source, countext);
}

bool powerkey_pmu_reisgter_callback(void)
{
    pmu_status_t ret;
    log_hal_info("[keypad]powerkey before pmu register callback\r\n");
    /*register longpress callback*/
    ret = pmu_register_callback(PMU_INT_PKEYLP, PMU_EDGE_FALLING_AND_RISING, (pmu_callback_t)powerkey_long_press_handler, (void *)(&powerkey_context));
    if (ret != PMU_STATUS_SUCCESS) {
        log_hal_info("[keypad]powerkey register powerkey_long_press_handler fail\r\n");
        return false;
    }


    /*register press callback*/
    ret = pmu_register_callback(PMU_INT_PWRKEY, PMU_EDGE_FALLING_AND_RISING, (pmu_callback_t)powerkey_press_handler, (void *)(&powerkey_context));
    if (ret != PMU_STATUS_SUCCESS) {
        log_hal_info("[keypad]powerkey register powerkey_press_handler fail\r\n");
        return false;
    }
    log_hal_info("[keypad]powerkey after pmu register callback\r\n");

    return true;
}


bool powerkey_pmu_init(void)
{
    bool ret;

    POWERKEY_GET_SLEEP_HANDLE();

    pmu_init();
    //pmu_set_register_value(PMU_RG_PWRKEY_TRIG_ADDR ,PMU_RG_PWRKEY_TRIG_MASK  ,PMU_RG_PWRKEY_TRIG_SHIFT,POWERKEY_PMU_RG_SET_1);

    pmu_set_register_value(PMU_INTSTS1, POWERKEY_PMU_STS_MASK, PMU_INTS_PKEYLP_R_SHIFT, POWERKEY_PMU_STS_MASK);
    pmu_set_register_value(PMU_INTSTS2, POWERKEY_PMU_STS_MASK, PMU_INTS_PWRKEY_R_SHIFT, POWERKEY_PMU_STS_MASK);

    log_hal_info("[keypad]powerkey after pmu init\r\n");
    /*hold on power*/
    ret = pmu_set_register_value(PMU_RG_PWRHOLD_ADDR, PMU_RG_PWRHOLD_MASK, PMU_RG_PWRHOLD_SHIFT, POWERKEY_PMU_RG_SET_1);
    if (ret != true) {
        log_hal_info("[keypad]powerkey set PMU_RG_PWRHOLD_MASK hold on power fail\r\n");
        return false;
    }

    /*setting pmu longpress feature*/
    ret = pmu_set_register_value(PMU_RG_PKEY_LPEN_ADDR, POWERKEY_PMU_RG_MASK, POWERKEY_PMU_RG_PKEY_CFG, POWERKEY_PMU_RG_SET_0);
    if (ret != true) {
        log_hal_info("[keypad]powerkey set POWERKEY_PMU_RG_PKEY_CFG fail\r\n");
        return false;
    }

    if (powerkey_pmu_reisgter_callback() != true) {
        log_hal_info("[keypad]powerkey register PMU callback fail\r\n");
        return false;
    }

    return true;
}


bool powerkey_get_pressed_state(void)
{
    static uint8_t last_state;
    uint32_t current_state;
    
    current_state = pmu_get_register_value(PMU_PWRKEY_VAL_ADDR,PMU_PWRKEY_VAL_MASK,PMU_PWRKEY_VAL_SHIFT); 

    if (last_state != current_state) {
        log_hal_info("[keypad]powerke PMU_PWRKEY_VAL=0x%x\r\n",current_state);
    }

    last_state = current_state;

    if (current_state != 0) {
        return false;
    }
    else {
        return true;
    }
}


#endif //HAL_KEYPAD_FEATURE_POWERKEY


