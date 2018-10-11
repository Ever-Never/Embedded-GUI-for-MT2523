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
 
#ifdef MTK_LED_ENABLE

#include "bsp_led.h"
#include "bt_sink_app_led_internal.h"
#include "bt_sink_app_led.h"
#include "bt_sink_app_main.h"

log_create_module(LED_APP, PRINT_LEVEL_INFO);

static bt_sink_app_led_num_t bt_sink_app_led_get_led_num_by_timer(TimerHandle_t timer_hd);
static void  bt_sink_app_led_lighting_state_machine(bt_sink_app_led_cntx_t* led_cntx);

bt_sink_app_led_cntx_t bt_sink_app_led_context[BT_SINK_APP_LED_MAX_NUM] = {{0}};
bt_sink_app_led_cntx_t* g_bt_sink_app_led_cntx_p = bt_sink_app_led_context;
SemaphoreHandle_t bt_sink_app_led_mutex = NULL;

void  bt_sink_app_led_create_mutex(void)
{
    bt_sink_app_led_mutex = xSemaphoreCreateRecursiveMutex();
    bt_sink_app_led_report("sempphonre mutex: 0x%x", bt_sink_app_led_mutex);
    return;
}

void  bt_sink_app_led_take_mutex(void)
{
    SemaphoreHandle_t mutex = bt_sink_app_led_mutex;

    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return;
    }
    
    if (mutex!= NULL) {
        xSemaphoreTakeRecursive(mutex, portMAX_DELAY);
    }
}

void  bt_sink_app_led_give_mutex(void)
{
    SemaphoreHandle_t mutex = bt_sink_app_led_mutex;

    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return; 
    }
    
    if (mutex!= NULL) {
        xSemaphoreGiveRecursive(mutex);
    }
}

void  bt_sink_app_led_delete_mutex(void)
{
    vSemaphoreDelete(bt_sink_app_led_mutex);
}

static void bt_sink_app_led_on(bt_sink_app_led_num_t led_num)
{
    bt_sink_app_led_report("[LED STATUS] led_num:0x%x ON", led_num);
    bsp_led_on((uint32_t)led_num);
    return;
}

static void bt_sink_app_led_off(bt_sink_app_led_num_t led_num)
{
    bt_sink_app_led_report("[LED STATUS]led_num:0x%x OFF", led_num);
    bsp_led_off((uint32_t)led_num);
    return;
}

static void bt_sink_app_led_timer_handle(TimerHandle_t timer_hd)
{
    bt_sink_app_led_take_mutex();
    uint8_t led_num = bt_sink_app_led_get_led_num_by_timer(timer_hd);
    if (led_num != BT_SINK_APP_LED_MAX_NUM) {
        bt_sink_app_led_cntx_t* led_cntx_p = &(g_bt_sink_app_led_cntx_p[led_num]);
        #if 0
        if(led_cntx_p->curr_config->state == BT_SINK_APP_LED_STATE_PAIR_COMPLETE 
            && led_cntx_p->remain_flash_count == 0) {
            bt_sink_app_led_light(BT_SINK_APP_LED_STATE_CONNECTED_IDLE); 
        } else if(((led_cntx_p->curr_config->state == BT_SINK_APP_LED_STATE_RESET_PAIRED_LIST )
          ||(led_cntx_p->curr_config->state == BT_SINK_APP_LED_STATE_PAIRING))
            && led_cntx_p->remain_flash_count == 0) {
            bt_sink_app_led_light(BT_SINK_APP_LED_STATE_IDLE);
        } else {
            bt_sink_app_led_lighting_state_machine(led_cntx_p);
        }
        #endif
        bt_sink_app_led_lighting_state_machine(led_cntx_p);
    }
    bt_sink_app_led_give_mutex();
    return;
}

static void bt_sink_app_led_timer_restart(TimerHandle_t timer_hd, uint32_t time_ms)
{
    bt_sink_app_led_report("restart led timer,  timer id: 0x%x, delay:%d", timer_hd, time_ms);
    bt_sink_app_assert(time_ms);
    xTimerChangePeriod(timer_hd, time_ms /portTICK_PERIOD_MS, 0);
    xTimerReset(timer_hd, 0);
    return;
}

static void bt_sink_app_led_timer_stop(TimerHandle_t timer_hd)
{
    if (xTimerIsTimerActive(timer_hd)) {
        bt_sink_app_led_report("stop  timer,  timer hd: 0x%x", timer_hd);
        xTimerStop(timer_hd, 0);
    }
    return;
}

static TimerHandle_t bt_sink_app_led_timer_create(void)
{
    TimerHandle_t time_handle = xTimerCreate(
                "BT_SINK_APP_LED_TIMER", 0xFFFF, pdFALSE, NULL, bt_sink_app_led_timer_handle);
    bt_sink_app_led_report("create timer,  timer handle: 0x%x", time_handle);
    return time_handle;
}

static void bt_sink_app_led_light_internal(bt_sink_app_led_num_t led_num , bool is_on, TimerHandle_t timer_hd, uint16_t duration)
{
    bt_sink_app_led_report("[LED STATUS] led_num:0x%x, is_on:%d, duration:%d", led_num, is_on, duration);
    if(duration != BT_SINK_APP_LED_DURATION_INFINITE) {
        bt_sink_app_led_timer_restart(timer_hd, duration);
    }
    if (is_on) {
        bt_sink_app_led_on(led_num);
    } else {
        bt_sink_app_led_off(led_num);
    }
    return;
}

static bt_sink_app_led_num_t bt_sink_app_led_get_led_num_by_timer(TimerHandle_t timer_hd)
{
    bt_sink_app_led_num_t led_num = BT_SINK_APP_LED_MAX_NUM;
    for (bt_sink_app_led_num_t i = BT_SINK_APP_LED_NUM_1; i < BT_SINK_APP_LED_MAX_NUM; i++) {
        if (g_bt_sink_app_led_cntx_p[i].timer_hd == timer_hd) {
            led_num = i ;
            break;
        }
    }
    bt_sink_app_led_report("timer_hd:0x%x, led_num:%d", timer_hd, led_num);
    return led_num;
}

static bt_sink_app_led_num_t bt_sink_app_led_get_led_num_by_cntx(bt_sink_app_led_cntx_t* led_cntx_p)
{
     bt_sink_app_led_num_t led_num = BT_SINK_APP_LED_MAX_NUM;
    if(led_cntx_p != NULL) {
        for (bt_sink_app_led_num_t i = BT_SINK_APP_LED_NUM_1; i < BT_SINK_APP_LED_MAX_NUM; i++) {
            if (&(g_bt_sink_app_led_cntx_p[i])== led_cntx_p) {
                led_num = i ;
                break;
            }
        }
    }
    bt_sink_app_led_report("led ctx:0x%x, led_num:%d", led_cntx_p, led_num);
    return led_num;
}

static bt_sink_app_led_state_config_table_t* bt_sink_app_led_get_state_config_param(bt_sink_app_led_state_t state)
{
    bt_sink_app_led_state_config_table_t* state_config_table = (bt_sink_app_led_state_config_table_t*)bt_sink_app_led_get_state_cfg_table();
    bt_sink_app_led_state_config_table_t*  config_param = NULL;
    for (bt_sink_app_led_state_t i = 0; i < BT_SINK_APP_LED_STATE_END; i++) {
        bt_sink_app_led_state_config_table_t* temp_cofig = (bt_sink_app_led_state_config_table_t*)((uint8_t*)state_config_table+i * sizeof(*state_config_table));
        //bt_sink_app_led_report("i:%d, temp_cofig->state:%d, config:0x%x", i,temp_cofig->state, temp_cofig);
        if (temp_cofig->state == state) {
            config_param = temp_cofig;
            break;
        }
    }
    bt_sink_app_led_report("state:%d, param:0x%x ", state, config_param);
    return config_param;
}

static void bt_sink_app_led_light_reset(bt_sink_app_led_cntx_t* led_cntx)
{
    bt_sink_app_led_timer_stop(led_cntx->timer_hd);
    led_cntx->mode = BT_SINK_APP_LED_MODE_NONE;
    led_cntx->status = BT_SINK_APP_LED_STATUS_OFF;
    led_cntx->remain_flash_count = 0;
    led_cntx->remain_period_count = 0;
    led_cntx->state = BT_SINK_APP_LED_LIGHTING_STATE_IDLE;
    led_cntx->curr_config = NULL;
    return ;
}

static void bt_sink_app_led_lighting_state_idle(bt_sink_app_led_cntx_t* led_cntx) 
{
    //bt_sink_app_led_report("enter idle state");
    bt_sink_app_led_state_config_table_t *curr_config_p = led_cntx->curr_config;
    bt_sink_app_led_num_t led_num = bt_sink_app_led_get_led_num_by_cntx(led_cntx);
    led_cntx->state = BT_SINK_APP_LED_LIGHTING_STATE_FLASH;
    led_cntx->status = BT_SINK_APP_LED_STATUS_ON;
    bt_sink_app_led_light_internal(led_num, true, led_cntx->timer_hd, curr_config_p->flash_on_duration);
    return;
}

static void bt_sink_app_led_lighting_state_flash(bt_sink_app_led_cntx_t* led_cntx) 
{
   //bt_sink_app_led_report("enter blink state");
    bt_sink_app_led_state_config_table_t *curr_config_p = led_cntx->curr_config;
    bt_sink_app_led_num_t led_num = bt_sink_app_led_get_led_num_by_cntx(led_cntx);
    //bt_sink_app_led_report("led_cntx->mode:%d",led_cntx->mode);

    if ( led_cntx->status == BT_SINK_APP_LED_STATUS_ON) {
        if (curr_config_p->flash_off_duration > 0) {
            bt_sink_app_led_light_internal(led_num, false, led_cntx->timer_hd,  curr_config_p->flash_off_duration);
            led_cntx->status = BT_SINK_APP_LED_STATUS_OFF;
            if (led_cntx->mode != BT_SINK_APP_LED_MODE_FLASH_INFINITE) {
                led_cntx->remain_flash_count --;
            } 
        } else {
            bt_sink_app_led_report("Flash off duration time is 0,led state machine is finished.");
            bt_sink_app_led_off(led_num);
            bt_sink_app_event_post(BT_SINK_EVENT_APP_LED_NOTIFICATION, (void *)curr_config_p->state, NULL);
            bt_sink_app_led_light_reset(led_cntx);
        }
    } else { //the current status is off.
        bt_sink_app_led_report("remain count: 0x%x, period_count: 0x%x", led_cntx->remain_flash_count,  led_cntx->remain_period_count );
        if (led_cntx->mode == BT_SINK_APP_LED_MODE_FLASH_INFINITE) {
            bt_sink_app_led_light_internal(led_num, true, led_cntx->timer_hd, curr_config_p->flash_on_duration);
            led_cntx->status = BT_SINK_APP_LED_STATUS_ON;
        } else if (led_cntx->mode == BT_SINK_APP_LED_MODE_FLASH_NORMAL || 
            led_cntx->mode == BT_SINK_APP_LED_MODE_PERIOD_INFINITE) {
            if (led_cntx->remain_flash_count > 0) {
                bt_sink_app_led_light_internal(led_num, true, led_cntx->timer_hd, curr_config_p->flash_on_duration);
                led_cntx->status = BT_SINK_APP_LED_STATUS_ON;
            } else if ((led_cntx->remain_period_count > 0) && (curr_config_p->interval > 0)) {
                led_cntx->state = BT_SINK_APP_LED_LIGHTING_STATE_INTERVAL;
                bt_sink_app_led_light_internal(led_num, false, led_cntx->timer_hd, curr_config_p->interval);
            } else {
                bt_sink_app_event_post(BT_SINK_EVENT_APP_LED_NOTIFICATION, (void *)curr_config_p->state, NULL);
                bt_sink_app_led_report("led state machine is finished.");
                bt_sink_app_led_light_reset(led_cntx);
            }
        }
    } 
    return;
}

static void bt_sink_app_led_lighting_state_interval(bt_sink_app_led_cntx_t* led_cntx)
{
   // bt_sink_app_led_report("enter interval state");
    bt_sink_app_led_state_config_table_t *curr_config_p = led_cntx->curr_config;
    bt_sink_app_led_num_t led_num = bt_sink_app_led_get_led_num_by_cntx(led_cntx);
    bt_sink_app_assert(curr_config_p->flash_off_duration > 0);
    bt_sink_app_assert(led_cntx->status == BT_SINK_APP_LED_STATUS_OFF);

    if (led_cntx->mode == BT_SINK_APP_LED_MODE_PERIOD_INFINITE) {
        goto FLASH_STATE;
    } else if (led_cntx->mode == BT_SINK_APP_LED_MODE_FLASH_NORMAL) {
        led_cntx->remain_period_count --;
        if (led_cntx->remain_period_count  > 0) {
            goto FLASH_STATE;
        } else {
            bt_sink_app_led_light_reset(led_cntx);
            return;
        }
    } else {
        bt_sink_app_led_report("error led mode:%d", led_cntx->mode);
        bt_sink_app_led_light_reset(led_cntx);
        return;
    }

FLASH_STATE:
    led_cntx->state = BT_SINK_APP_LED_LIGHTING_STATE_FLASH;
    led_cntx->status = BT_SINK_APP_LED_STATUS_ON;
    led_cntx->remain_flash_count = curr_config_p->flash_count;
    bt_sink_app_led_light_internal(led_num, true, led_cntx->timer_hd, curr_config_p->flash_on_duration);
    return;
}

static void  bt_sink_app_led_lighting_state_machine(bt_sink_app_led_cntx_t* led_cntx)
{
    bt_sink_app_led_report("[IN]light_state:%d", led_cntx->state);
    switch (led_cntx->state) {
        case BT_SINK_APP_LED_LIGHTING_STATE_IDLE:
            bt_sink_app_led_lighting_state_idle(led_cntx);
            break;
        case BT_SINK_APP_LED_LIGHTING_STATE_FLASH:
            bt_sink_app_led_lighting_state_flash(led_cntx);
            break;
        case BT_SINK_APP_LED_LIGHTING_STATE_INTERVAL:
            bt_sink_app_led_lighting_state_interval(led_cntx);
            break;
        default:
            break;
    }
    bt_sink_app_led_report("[OUT] light_state:%d", led_cntx->state);
    return;
}

void bt_sink_app_led_init(void)
{
    bt_sink_app_led_num_t led_num = BT_SINK_APP_LED_NUM_1; 
    bt_sink_app_led_create_mutex();
    for (led_num= 0; led_num < BT_SINK_APP_LED_MAX_NUM; led_num++) {
        bsp_led_status_t result = bsp_led_init((uint32_t)led_num);
        bt_sink_app_led_report("init led_num=%d, result:%d", led_num, result);
        if (result == BSP_LED_OK) {
            g_bt_sink_app_led_cntx_p[led_num].timer_hd = bt_sink_app_led_timer_create();
            g_bt_sink_app_led_cntx_p[led_num].curr_config = NULL;
            g_bt_sink_app_led_cntx_p[led_num].mode = BT_SINK_APP_LED_MODE_NONE;
            g_bt_sink_app_led_cntx_p[led_num].remain_flash_count = 0;
            g_bt_sink_app_led_cntx_p[led_num].remain_period_count = 0;
            g_bt_sink_app_led_cntx_p[led_num].state = BT_SINK_APP_LED_LIGHTING_STATE_IDLE;
            g_bt_sink_app_led_cntx_p[led_num].status = BT_SINK_APP_LED_STATUS_OFF;
        } else {
            //bt_sink_app_assert(0);
        }
    }
    return ;
}

void bt_sink_app_led_light(bt_sink_app_led_state_t state)
{
    bt_sink_app_led_take_mutex();
    bt_sink_app_led_report("[LED STATUS][new action] state:%d", state);

    /*1. get state cfg param.*/
    bt_sink_app_led_state_config_table_t* state_config_p = bt_sink_app_led_get_state_config_param(state);
    if (state_config_p == NULL) {
        bt_sink_app_led_report("Error,The led state is not configurated!");
        bt_sink_app_led_give_mutex();
        return;
    }

    /*2. check this state is enable or not.*/
    if ( !state_config_p->enable ||(state_config_p->state == BT_SINK_APP_LED_STATE_NONE) ) {
        bt_sink_app_led_report("Error,The led state is not enable!");
        bt_sink_app_led_give_mutex();
        return;
    }

    for (bt_sink_app_led_num_t led_num = BT_SINK_APP_LED_NUM_1; led_num < BT_SINK_APP_LED_MAX_NUM; led_num++) {
        if (g_bt_sink_app_led_cntx_p[led_num].curr_config->state == BT_SINK_APP_LED_STATE_POWER_OFF) {
            bt_sink_app_led_report("Powering off!!!");
            bt_sink_app_led_give_mutex();
            return;
        }
    }

    for (bt_sink_app_led_num_t led_num = BT_SINK_APP_LED_NUM_1; led_num < BT_SINK_APP_LED_MAX_NUM; led_num++) {
            bt_sink_app_led_off(led_num);
            bt_sink_app_led_light_reset(&(g_bt_sink_app_led_cntx_p[led_num]));
    }

    /*3. get led num.*/
    bt_sink_app_led_mask_t led_mask = state_config_p->led_mask;
    for (bt_sink_app_led_num_t led_num = BT_SINK_APP_LED_NUM_1; led_num < BT_SINK_APP_LED_MAX_NUM; led_num++) {
        if(led_mask & (0x01<< led_num)) {
            bt_sink_app_led_cntx_t* led_cntx_p = &(g_bt_sink_app_led_cntx_p[led_num]);
            #if 0
            if(led_cntx_p->status == BT_SINK_APP_LED_STATUS_ON) {
                bt_sink_app_led_off(led_num);
            }
            #endif
            
            bt_sink_app_led_light_reset(led_cntx_p);
            led_cntx_p->curr_config = state_config_p;
            /*4. save the led context.*/
            if (state_config_p->flash_on_duration == BT_SINK_APP_LED_DURATION_INFINITE) {
                led_cntx_p->mode = BT_SINK_APP_LED_MODE_LONG_ON;
                bt_sink_app_led_on(led_num);
            } else if (state_config_p->flash_on_duration == 0) {
                led_cntx_p->mode = BT_SINK_APP_LED_MODE_LONG_OFF;
                bt_sink_app_led_off(led_num);
            } else if (state_config_p->flash_count == BT_SINK_APP_LED_COUNT_INFINITE) {
                led_cntx_p->mode = BT_SINK_APP_LED_MODE_FLASH_INFINITE;
                bt_sink_app_led_lighting_state_machine(led_cntx_p);
            }  else if (state_config_p->period_count == BT_SINK_APP_LED_COUNT_INFINITE) {
                led_cntx_p->mode = BT_SINK_APP_LED_MODE_PERIOD_INFINITE;
                led_cntx_p->remain_flash_count = state_config_p->flash_count;
                bt_sink_app_led_lighting_state_machine(led_cntx_p);
            } else {
                led_cntx_p->mode = BT_SINK_APP_LED_MODE_FLASH_NORMAL;
                led_cntx_p->remain_flash_count = state_config_p->flash_count;
                led_cntx_p->remain_period_count = state_config_p->period_count;
                /*5. enter the led state machine.*/
                bt_sink_app_led_lighting_state_machine(led_cntx_p);
            }
        }
    }
    bt_sink_app_led_give_mutex();
    return;
}

static bt_sink_app_led_num_t bt_sink_app_led_get_led_num(bt_sink_app_led_state_t state)
{
    bt_sink_app_led_num_t led_num = BT_SINK_APP_LED_MAX_NUM;
    bt_sink_app_led_state_config_table_t* config_table = bt_sink_app_led_get_state_config_param(state);
    if (config_table != NULL) {
        for (bt_sink_app_led_num_t i = BT_SINK_APP_LED_NUM_1; i < BT_SINK_APP_LED_MAX_NUM; i++) {
            if(config_table->led_mask & (0x01<< i)) {
                led_num = i;
                break;
            }
        }
    }
    bt_sink_app_led_report("state:%d, led num:%d", state, led_num);
    return led_num;
}

bool bt_sink_app_led_check_if_in_the_state(bt_sink_app_led_state_t state)
{
    bt_sink_app_led_take_mutex();
    bool result = false;
    bt_sink_app_led_num_t led_num = bt_sink_app_led_get_led_num(state);
    if (BT_SINK_APP_LED_NUM_1 <= led_num && BT_SINK_APP_LED_MAX_NUM > led_num) {
        if (state == g_bt_sink_app_led_cntx_p[led_num].curr_config->state) {
            result = true;
        }
    }
    bt_sink_app_led_report("state:%d, result:%d", state, result);
    bt_sink_app_led_give_mutex();
    return result;
}

void bt_sink_app_update_led_by_sink_state(void)
{
    bt_sink_srv_state_t sink_state = bt_sink_srv_state_query();
    bt_sink_app_led_state_t led_state = BT_SINK_APP_LED_STATE_NONE;
    switch (sink_state) {
        case BT_SINK_SRV_STATE_POWER_ON: 
        {
            led_state = BT_SINK_APP_LED_STATE_IDLE;
        }
        break;
        
        case BT_SINK_SRV_STATE_CONNECTED: 
        {
            led_state = BT_SINK_APP_LED_STATE_CONNECTED_IDLE;
        }
        break;

        case BT_SINK_SRV_STATE_STREAMING: 
        {
            led_state = BT_SINK_APP_LED_STATE_BT_MUSIC_PLAYING;
        }
        break;

        case BT_SINK_SRV_STATE_LOCAL_PLAYING: 
        {
            led_state = BT_SINK_APP_LED_STATE_LOCAL_MUSIC_PLAYING;
        }
        break;

        case BT_SINK_SRV_STATE_ACTIVE: 
        {
            led_state = BT_SINK_APP_LED_STATE_ACTIVE_CALL;
        }
        break;

        default:
            bt_sink_app_led_report("not support state:%d", sink_state);
        break;
    }
    
    bt_sink_app_led_report("sink state:%d, led state: %d", sink_state, led_state);    
    if (led_state != BT_SINK_APP_LED_STATE_NONE) {
        bt_sink_app_led_light(led_state);
    }
    return;
}

bt_sink_srv_status_t bt_sink_app_update_led_by_sink_event(bt_sink_srv_event_t event_id, void *parameters)
{
    //bt_sink_app_led_report("event:0x%x", event_id);
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)parameters;
    switch (event_id) {
        case BT_SINK_SRV_EVENT_STATE_CHANGE: 
        {
            bt_sink_srv_state_t curr_state = event->state_change.now;
            bt_sink_app_led_report("state change, previous:0x%x, now:0x%x", event->state_change.previous, curr_state);
            if (curr_state == BT_SINK_SRV_STATE_STREAMING) {
                bt_sink_app_led_light(BT_SINK_APP_LED_STATE_BT_MUSIC_PLAYING);
            } else if (curr_state == BT_SINK_SRV_STATE_LOCAL_PLAYING) {
                bt_sink_app_led_light(BT_SINK_APP_LED_STATE_LOCAL_MUSIC_PLAYING);
            } else if (curr_state == BT_SINK_SRV_STATE_MULTIPARTY) {

            } else if (curr_state == BT_SINK_SRV_STATE_HELD_REMAINING) {
            
            } else if (curr_state ==BT_SINK_SRV_STATE_HELD_ACTIVE) {
               
            }else  if (curr_state == BT_SINK_SRV_STATE_TWC_OUTGOING) {
            
            } else if (curr_state ==BT_SINK_SRV_STATE_TWC_INCOMING) {
            
            } else if (curr_state == BT_SINK_SRV_STATE_ACTIVE) {
                bt_sink_app_led_light(BT_SINK_APP_LED_STATE_ACTIVE_CALL);
            } else if (curr_state == BT_SINK_SRV_STATE_OUTGOING) {
               bt_sink_app_led_light(BT_SINK_APP_LED_STATE_OUTGOING_CALL);
            } else if (curr_state == BT_SINK_SRV_STATE_INCOMING) {
                bt_sink_app_led_light(BT_SINK_APP_LED_STATE_INCOMING_CALL);
            }else  if (curr_state == BT_SINK_SRV_STATE_CONNECTED) {
                if(event->state_change.previous == BT_SINK_SRV_STATE_POWER_ON) {
                    //bt_sink_app_led_light(BT_SINK_APP_LED_STATE_PAIR_COMPLETE);
                } else {
                    bt_sink_app_led_light(BT_SINK_APP_LED_STATE_CONNECTED_IDLE);
                }
            } else if (curr_state == BT_SINK_SRV_STATE_POWER_ON) {
                if (event->state_change.previous == BT_SINK_SRV_STATE_LOCAL_PLAYING) {
                    bt_sink_app_led_light(BT_SINK_APP_LED_STATE_IDLE);
                } else if (event->state_change.previous != BT_SINK_SRV_STATE_NONE) {
                    //bt_sink_app_led_light(BT_SINK_APP_LED_STATE_DISCONNECTED);
                } else {
                    bt_sink_app_led_light(BT_SINK_APP_LED_STATE_IDLE);
                }
            } else if (curr_state == BT_SINK_SRV_STATE_LOCAL_PLAYING) {
                bt_sink_app_led_light(BT_SINK_APP_LED_STATE_LOCAL_MUSIC_PLAYING);
            } else if (curr_state == BT_SINK_SRV_STATE_NONE) {
            
            } else {

            }
        }
        break;
        
        case BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE:
        {
            bt_sink_app_report("[Sink] connection information, profile:0x%x", event->connection_info.profile_type);
            #if 0
            if (BT_SINK_SRV_PROFILE_NONE == event->connection_info.profile_type) {
                bt_sink_app_led_light(BT_SINK_APP_LED_STATE_DISCONNECTED);
            } else {
                bt_sink_app_led_light(BT_SINK_APP_LED_STATE_PAIR_COMPLETE);
            }
            #endif
            if (bt_sink_app_context.notify_connection_state) {
                if (bt_sink_app_get_connected_device(&(event->connection_info.bt_addr)) ){
                    bt_sink_app_led_light(BT_SINK_APP_LED_STATE_PAIR_COMPLETE);
                } else {
                    bt_sink_app_led_light(BT_SINK_APP_LED_STATE_DISCONNECTED);
                }
            }
        }
        break;
        
        case BT_SINK_SRV_EVENT_VISIBILITY_STATE_UPDATE:
        {
            bt_sink_app_led_report("Visibility state, %d", event->visibility_state.visibility_on);
            if (event->visibility_state.visibility_on) {
                bt_sink_app_led_light(BT_SINK_APP_LED_STATE_PAIRING);
            }
        }
        break;
        
        case BT_SINK_EVENT_APP_LED_NOTIFICATION:
        {
            uint32_t led_state = (uint32_t)parameters;
            bt_sink_app_led_report("LED state notification, 0x%x", led_state);
            if(led_state == BT_SINK_APP_LED_STATE_PAIR_COMPLETE ||
                led_state == BT_SINK_APP_LED_STATE_RESET_PAIRED_LIST || 
                led_state == BT_SINK_APP_LED_STATE_PAIRING ||
                led_state == BT_SINK_APP_LED_STATE_DISCONNECTED
            ) {
                bt_sink_app_update_led_by_sink_state();
            } else {
            
            }
        }
        break;
        
        default:
            break;
    }
    return BT_SINK_SRV_STATUS_SUCCESS;
}

void bt_sink_app_update_led_by_battery(int32_t charger_exist, int32_t capacity)
{
    bt_sink_app_led_report("charger_exist:0x%x,capacity:0x%x", charger_exist, capacity);
    if (charger_exist) { //in charging
        if (capacity == 100) { //battery full
            if (!bt_sink_app_led_check_if_in_the_state(BT_SINK_APP_LED_STATE_BATTERY_FULL)
                 && (bt_sink_app_led_check_if_in_the_state(BT_SINK_APP_LED_STATE_CONNECTED_IDLE)
                 || bt_sink_app_led_check_if_in_the_state(BT_SINK_APP_LED_STATE_IDLE)
                 ||bt_sink_app_led_check_if_in_the_state(BT_SINK_APP_LED_STATE_CHARGING))
             ) {
                bt_sink_app_led_light(BT_SINK_APP_LED_STATE_BATTERY_FULL);
            }
        } else {
            if (!bt_sink_app_led_check_if_in_the_state(BT_SINK_APP_LED_STATE_CHARGING) 
                 && (bt_sink_app_led_check_if_in_the_state(BT_SINK_APP_LED_STATE_CONNECTED_IDLE)
                 || bt_sink_app_led_check_if_in_the_state(BT_SINK_APP_LED_STATE_IDLE)
                 || bt_sink_app_led_check_if_in_the_state(BT_SINK_APP_LED_STATE_BATTERY_FULL))
            ) {
                bt_sink_app_led_light(BT_SINK_APP_LED_STATE_CHARGING);
            }
        }  
    } else { //out charing
        if (capacity<= BT_SINK_APP_LOW_BATTERY_ALARM) {
           if (!bt_sink_app_led_check_if_in_the_state(BT_SINK_APP_LED_STATE_LOW_BATTERY)) {
               bt_sink_app_led_light(BT_SINK_APP_LED_STATE_LOW_BATTERY);
           }
        } else  {
            if (bt_sink_app_led_check_if_in_the_state(BT_SINK_APP_LED_STATE_CHARGING) 
                ||bt_sink_app_led_check_if_in_the_state(BT_SINK_APP_LED_STATE_BATTERY_FULL)
            ) {
                bt_sink_app_update_led_by_sink_state();
            } 
        }
    }
    return;
}
#endif /*MTK_LED_ENABLE*/

