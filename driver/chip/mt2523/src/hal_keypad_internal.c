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

#ifdef HAL_KEYPAD_MODULE_ENABLED
#include "hal_keypad_internal.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include <assert.h>
#include "hal_log.h"
#include "hal_gpt.h"
#include "hal_gpt_internal.h"
#include "hal_eint.h"
#include "hal_eint_internal.h"

KEYPAD_REGISTER_T *keypad = KEYPAD_REGISTER;

keypad_key_bitmap_t keypad_reg_bitmap;
keypad_key_bitmap_t keypad_saved_bitmap;

keypad_status_t keypad_state;
keypad_buffer_t keypad_buffer;

/* single key default index */
static const uint32_t single_key_index[10] = { 0,  1, 2,
                                               9, 10, 11,
                                               18, 19, 20,
                                               KEY_INDEX_END
                                             };

/* double key default index */
static const uint32_t double_key_index[19] = { 0,  1,  2,  3,  4,  5,
                                               13, 14, 15, 16, 17, 18,
                                               26, 27, 28, 29, 30, 31,
                                               KEY_INDEX_END
                                             };

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_driver.h"

const static char *keypad_lock_sleep_name = "keypad";
static uint8_t keypad_lock_sleep_handle[32];

#define KEYPAD_GET_SLEEP_HANDLE(i) \
do{ \
    keypad_lock_sleep_handle[i] = hal_sleep_manager_set_sleep_handle(keypad_lock_sleep_name); \
    if(keypad_lock_sleep_handle[i] == INVALID_SLEEP_HANDLE ) { \
        log_hal_error("[keypad]:get sleep handle failed\r\n"); \
        return; \
    } \
}while(0)

#define KEYPAD_RELEASE_SLEEP_HANDLE(i) \
do{ \
    hal_sleep_manager_release_sleep_handle(keypad_lock_sleep_handle[i]); \
}while(0)

#define KEYPAD_LOCK_SLEEP(i) \
do{ \
    hal_sleep_manager_lock_sleep(keypad_lock_sleep_handle[i]); \
}while(0)

#define KEYPAD_UNLOCK_SLEEP(i) \
do{ \
    hal_sleep_manager_unlock_sleep(keypad_lock_sleep_handle[i]); \
}while(0)

#else
#define KEYPAD_GET_SLEEP_HANDLE(i)
#define KEYPAD_RELEASE_SLEEP_HANDLE(i)
#define KEYPAD_LOCK_SLEEP(i)
#define KEYPAD_UNLOCK_SLEEP(i)
#endif


void keypad_get_register_map(keypad_key_bitmap_t *key_bitmap)
{
    uint32_t reg[2];

    reg[0] = keypad->KP_MEM1;
    reg[1] = keypad->KP_MEM2;
    key_bitmap->register_value[0] = ((uint32_t)(reg[1] << 16) + (uint32_t)reg[0]);
	#ifdef KEYPAD_DEBUG_LOG
    log_hal_info("[keypad][map]bitmap_reg = 0x%x\r\n", (unsigned int)keypad_reg_bitmap.register_value[0]);
    #endif
}

hal_keypad_mode_t keypad_get_mode(void)
{

    return (hal_keypad_mode_t)(keypad->KP_SEL & KEYPAD_FLAG_MODE);
}

hal_keypad_key_state_t keypad_get_state(void)
{
    return (hal_keypad_key_state_t)(keypad->KP_STA & KEYPAD_FLAG_STA);
}

void keypad_push_one_key_to_buffer(hal_keypad_key_state_t state, uint32_t data)
{
    uint32_t time;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &time);
    
    keypad_buffer.data[keypad_buffer.write_index].state         = state;
    keypad_buffer.data[keypad_buffer.write_index].key_data      = data;
    keypad_buffer.data[keypad_buffer.write_index].time_stamp    = time;
    
    keypad_buffer.write_index++;
    keypad_buffer.write_index &= (KEYPAD_BUFFER_SIZE - 1);
}

void keypad_pop_one_key_from_buffer(hal_keypad_event_t *key_event)
{
    key_event->state     = keypad_buffer.data[keypad_buffer.read_index].state;
    key_event->key_data  = keypad_buffer.data[keypad_buffer.read_index].key_data;
    key_event->time_stamp= keypad_buffer.data[keypad_buffer.read_index].time_stamp;
    keypad_buffer.read_index++;
    keypad_buffer.read_index &= (KEYPAD_BUFFER_SIZE - 1);
}

uint32_t keypad_get_buffer_left_size(void)
{
    if (keypad_buffer.write_index >= keypad_buffer.read_index) {

        return (KEYPAD_BUFFER_SIZE - (keypad_buffer.write_index - keypad_buffer.read_index));
    } else {
        return (keypad_buffer.read_index - keypad_buffer.write_index);

    }
}

uint32_t keypad_get_buffer_data_size(void)
{
    return (KEYPAD_BUFFER_SIZE - keypad_get_buffer_left_size());
}


void keypad_get_key_index(void)
{
    if (keypad_get_mode() == HAL_KEYPAD_MODE_SINGLE_KEY) {
        keypad_state.p_key_index = (uint32_t *)(&single_key_index[0]);
    } else if (keypad_get_mode() == HAL_KEYPAD_MODE_DOUBLE_KEY) {
        keypad_state.p_key_index = (uint32_t *)(&double_key_index[0]);
    }
}

static void keypad_call_user_callback(void)
{
    keypad_callback_context_t *context;

    context = &keypad_context.keypad_callback;

    if (keypad_context.has_initilized != true) {
        return;
    }

    context->callback(context->user_data);
}

void keypad_process_repeat_and_longpress(uint32_t *keypad_type)
{
    uint32_t temp_saved, temp_reg, temp_index;
    uint32_t key_position;

    hal_gpt_status_t ret_state;

    /* get current register bit map */
    keypad_get_register_map(&keypad_reg_bitmap);

    key_position = *keypad_type;

    temp_saved = (keypad_saved_bitmap.register_value[0] >> key_position)  & 0x01;
    temp_reg   = (keypad_reg_bitmap.register_value[0] >> key_position)   & 0x01;
    temp_index = (uint32_t)(temp_saved << 4) + (uint32_t)temp_reg;

    if (temp_index == KEYPAD_TYPE_OLD_PRESS) {
        if (keypad_get_buffer_left_size() > (keypad_buffer.press_count + 1)) {
            if (keypad_state.current_state[key_position] != HAL_KEYPAD_KEY_PRESS) {
                keypad_state.current_state[key_position] = HAL_KEYPAD_KEY_REPEAT;
            } else {
                keypad_state.current_state[key_position] = HAL_KEYPAD_KEY_LONG_PRESS;

            }

            keypad_state.position[key_position] = key_position;
            keypad_push_one_key_to_buffer(keypad_state.current_state[key_position], keypad_state.position[key_position]);

            /*start timer*/
            ret_state = hal_gpt_sw_start_timer_ms(\
                                                  keypad_state.timer_handle[key_position], \
                                                  keypad_context.repeat_time, \
                                                  (hal_gpt_callback_t)keypad_process_repeat_and_longpress, \
                                                  (void *)(&keypad_state.position[key_position]));
            if (ret_state != HAL_GPT_STATUS_OK) {
                log_hal_info("[keypad][state]get timer handle error,ret = 0x%x\r\n", ret_state);
            }

            log_hal_info("[keypad][state]key state = %d, key_position = 0x%x\r\n", (int)keypad_state.current_state[key_position], (int)key_position);
            keypad_call_user_callback();
        }

    }

}


void keypad_process_key(volatile uint32_t *keypad_type)
{
    uint32_t i;
    uint32_t *p_index;
    uint32_t temp_saved, temp_reg, temp_index;
    uint32_t key_position;

    hal_gpt_status_t ret_state;

    keypad_get_key_index();

    /* get current register bit map */
    keypad_get_register_map(&keypad_reg_bitmap);

    p_index = keypad_state.p_key_index;

    for (i = 0; ; i++, p_index++) {
        if (*p_index == KEY_INDEX_END) {
            break;
        }

        if ((*keypad_type != PROCESS_FROM_KEYPAD_IRQ)) {
            break;;
        }

        key_position = *p_index;    /* key position */
        temp_saved = (keypad_saved_bitmap.register_value[0] >> key_position)  & 0x01;
        temp_reg   = (keypad_reg_bitmap.register_value[0] >> key_position)   & 0x01;
        temp_index = (uint32_t)(temp_saved << 4) + (uint32_t)temp_reg;

        switch (temp_index) {
            /* old press */
            case KEYPAD_TYPE_OLD_PRESS: {
                continue;
            }

            /* new release */
            case KEYPAD_TYPE_NEW_RELEASE: {
                if (keypad_get_buffer_left_size() > 0) {
                    keypad_buffer.press_count--;

                    keypad_state.current_state[key_position] = HAL_KEYPAD_KEY_RELEASE;

                    keypad_push_one_key_to_buffer(keypad_state.current_state[key_position], key_position);


                    /*release key should stop timer*/
                    ret_state = hal_gpt_sw_stop_timer_ms(keypad_state.timer_handle[key_position]);
                    if (ret_state != HAL_GPT_STATUS_OK) {
                        log_hal_info("[keypad][state]stop timer handle error,ret = %d, handle = 0x%x\r\n", \
                                     (int)ret_state, \
                                     (int)keypad_state.timer_handle[key_position]);
                    }

                    /*free timer*/
                    ret_state = hal_gpt_sw_free_timer(keypad_state.timer_handle[key_position]);
                    if (ret_state != HAL_GPT_STATUS_OK) {
                        log_hal_info("[keypad][state]free timer handle error,ret = %d, handle = 0x%x\r\n", \
                                     (int)ret_state, \
                                     (int)keypad_state.timer_handle[key_position]);
                    }

                    log_hal_info("[keypad][state]key state = %d, key_position = 0x%x\r\n", (int)keypad_state.current_state[key_position], (int)key_position);
                    keypad_call_user_callback();
                    KEYPAD_UNLOCK_SLEEP(key_position);
                    KEYPAD_RELEASE_SLEEP_HANDLE(key_position);
                } else {
                    assert(0);
                }
            }
            break;

            /* new press */
            case KEYPAD_TYPE_NEW_PRESS: {
                if (keypad_get_state() == HAL_KEYPAD_KEY_PRESS) {
                    if (keypad_get_buffer_left_size() > (keypad_buffer.press_count + 1)) {
                        keypad_buffer.press_count++;

                        keypad_state.current_state[key_position] = HAL_KEYPAD_KEY_PRESS;

                        keypad_state.position[key_position] = key_position;

                        keypad_push_one_key_to_buffer(keypad_state.current_state[key_position], keypad_state.position[key_position]);

                        KEYPAD_GET_SLEEP_HANDLE(key_position);
                        KEYPAD_LOCK_SLEEP(key_position);

                        /*allocate timer*/
                        ret_state = hal_gpt_sw_get_timer(&keypad_state.timer_handle[key_position]);
                        if (ret_state != HAL_GPT_STATUS_OK) {
                            log_hal_info("[keypad][state]get timer handle error,ret = %d, handle = 0x%x\r\n", \
                                         (unsigned int)ret_state, \
                                         (unsigned int)keypad_state.timer_handle[key_position]);
                        }

                        /*start timer*/
                        ret_state = hal_gpt_sw_start_timer_ms(\
                                                              keypad_state.timer_handle[key_position], \
                                                              keypad_context.longpress_time, \
                                                              (hal_gpt_callback_t)keypad_process_repeat_and_longpress, \
                                                              (void *)(&keypad_state.position[key_position]));
                        if (ret_state != HAL_GPT_STATUS_OK) {
                            log_hal_info("[keypad][state]start timer handle error,ret = 0x%x\r\n", ret_state);
                        }

                        log_hal_info("[keypad][state]key state = %d, key_position = 0x%x\r\n", (unsigned int)keypad_state.current_state[key_position], (unsigned int)key_position);
                        keypad_call_user_callback();
                    } else {
                        /* buffer size is not enough, so drop this bit and set to 1, 1 respect not pressed. */
                        keypad_reg_bitmap.register_value[0] |= (1 << i);
                    }
                }
            }
            break;

            /* old release */
            case KEYPAD_TYPE_OLD_RELEASE: {
                continue;
            }

            default: {
            } break;

        }
    }

    /* store current bit map */
    keypad_saved_bitmap.register_value[0] = keypad_reg_bitmap.register_value[0];

    if (keypad_get_state() == HAL_KEYPAD_KEY_PRESS) {
        keypad_context.is_running = true;
    } else {
        keypad_context.is_running = false;
    }


}

void keypad_interrupt_handler(hal_nvic_irq_t irq_number)
{
    volatile uint32_t mask;
    volatile uint32_t key_type = PROCESS_FROM_KEYPAD_IRQ;

    mask = save_and_set_interrupt_mask();

    keypad_process_key(&key_type);

    restore_interrupt_mask(mask);

}

void keypad_nvic_register(void)
{
    hal_nvic_status_t ret;

    hal_nvic_disable_irq(KP_IRQn);
    ret = hal_nvic_register_isr_handler(KP_IRQn, keypad_interrupt_handler);
    if (ret != HAL_NVIC_STATUS_OK) {
        log_hal_info("[keypad]keypad_nvic_register fail\r\n");
        assert(0);
    }
    hal_nvic_enable_irq(KP_IRQn);
}


void keypad_eint_wakeup_init(void)
{
    hal_eint_config_t config;

    config.debounce_time = 0;
    config.trigger_mode  = HAL_EINT_LEVEL_LOW;
    hal_eint_init(HAL_EINT_KEYPAD, &config);
    eint_unmask_wakeup_source(HAL_EINT_KEYPAD);
}

#endif /*HAL_KEYPAD_MODULE_ENABLED*/


