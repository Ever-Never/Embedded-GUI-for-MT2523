/* Copyright Statement:
 *
 * @2015 MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek Inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE.
 */

#include "bt_timer_external.h"

#ifdef __BT_TIMER_EXT_SUPPORT__
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "portmacro.h"
#include "queue.h"
#include <timers.h>
#include <string.h>
#include "syslog.h"
#include "bt_platform.h"


log_create_module(BT_TIMER_EXT, PRINT_LEVEL_INFO);

static uint32_t bt_sink_srv_events = 0;

static uint32_t bt_timer_ext_semaphore = 0;

static bt_timer_ext_t bt_timer_ext_array[BT_TIMER_EXT_INSTANCE_NUM];
static TimerHandle_t bt_sink_srv_os_timer = NULL; /**< Timer handler. */
static const bt_timer_ext_t* bt_sink_srv_time_running;

static uint32_t bt_timer_ext_get_current_tick(void);
static void bt_timer_ext_expire(TimerHandle_t timer);
static uint32_t bt_timer_ext_os_is_active(void);
static void bt_timer_ext_os_start(uint32_t tick);
static void bt_timer_ext_os_stop(void);
static const bt_timer_ext_t* bt_timer_ext_find_recent(uint32_t current);

void bt_timer_ext_init(void)
{
    LOG_I(BT_TIMER_EXT, "[sink][timer] bt_timer_ext_init()\r\n");

    if (NULL == bt_sink_srv_os_timer) {
        bt_sink_srv_os_timer = xTimerCreate( "bt timer ext", 0xffff, pdFALSE, NULL, bt_timer_ext_expire);
        bt_timer_ext_semaphore = (uint32_t)xSemaphoreCreateBinary();
    }
    xSemaphoreGive((SemaphoreHandle_t)bt_timer_ext_semaphore);
    memset(bt_timer_ext_array, 0, sizeof(bt_timer_ext_array));
}

/**
static void bt_timer_ext_deinit(void)
{
    LOG_I(BT_TIMER_EXT, "[sink][timer] bt_timer_ext_deinit()\r\n");
    if (bt_sink_srv_os_timer != NULL) {
        bt_timer_ext_os_stop();
        xTimerDelete(bt_sink_srv_os_timer, 0);
        bt_sink_srv_os_timer = NULL;
    }
    memset(bt_timer_ext_array, 0, sizeof(bt_timer_ext_array));
}
*/

bt_timer_ext_status_t bt_timer_ext_start(uint32_t timer_id, uint32_t data, uint32_t time_ms, bt_timer_ext_timeout_callback_t cb)
{
    uint32_t current;
    uint32_t length;
    int32_t diff, is_timeout;
    bt_timer_ext_t *bt_timer_ext;
    xSemaphoreTake((SemaphoreHandle_t)bt_timer_ext_semaphore, portMAX_DELAY);
    if (NULL != bt_timer_ext_find(timer_id)) {
        LOG_I(BT_TIMER_EXT, "[sink][timer] bt_timer_ext_start(%x) start fail!\r\n", timer_id);
        xSemaphoreGive((SemaphoreHandle_t)bt_timer_ext_semaphore);
        return BT_TIMER_EXT_STATUS_FAIL;
    }
    LOG_I(BT_TIMER_EXT, "[sink][timer] bt_timer_ext_start, ID(%x) duration(%d ms)\r\n", timer_id, time_ms);
    //bt_timer_ext_check_status(0);
    bt_timer_ext = bt_timer_ext_find(0);

    if (NULL == bt_timer_ext) {
        LOG_I(BT_TIMER_EXT, "[sink][timer] bt_timer_ext_start start OOM!\r\n");
        xSemaphoreGive((SemaphoreHandle_t)bt_timer_ext_semaphore);
        return BT_TIMER_EXT_STATUS_OOM;
    }
    current = bt_timer_ext_get_current_tick();
    length = time_ms/portTICK_PERIOD_MS + 1;

    bt_timer_ext->timer_id = timer_id;
    bt_timer_ext->data = data;
    bt_timer_ext->time_tick = current + length;
    bt_timer_ext->cb = cb;

    if ((bt_sink_srv_time_running != NULL) && (bt_sink_srv_time_running->timer_id != 0)) {
        is_timeout = bt_sink_srv_time_running->time_tick - current;
        diff = is_timeout - length;
        if ((is_timeout < 0) || (diff < 0)) //is_timeout <= 0, means timeout; diff < 0, means bt_timer_ext_array[i] is before bt_timer_ext.
        {
            bt_timer_ext = NULL;
        }
    }
    if (bt_timer_ext) {
        bt_timer_ext_os_stop();
        bt_timer_ext_os_start(length);
        bt_sink_srv_time_running = bt_timer_ext;
    } 
    LOG_I(BT_TIMER_EXT, "[sink][timer] bt_timer_ext_start end!\r\n");
    xSemaphoreGive((SemaphoreHandle_t)bt_timer_ext_semaphore);
    return BT_TIMER_EXT_STATUS_SUCCESS;
}

bt_timer_ext_status_t bt_timer_ext_stop(uint32_t timer_id)
{
    uint32_t current = bt_timer_ext_get_current_tick();
    xSemaphoreTake((SemaphoreHandle_t)bt_timer_ext_semaphore, portMAX_DELAY);
    bt_timer_ext_t* temp = bt_timer_ext_find(timer_id);
    if (temp == NULL) {
        LOG_I(BT_TIMER_EXT, "[sink][timer] bt_timer_ext_stop(%x) stop fail!\r\n", timer_id);
        xSemaphoreGive((SemaphoreHandle_t)bt_timer_ext_semaphore);
        return BT_TIMER_EXT_STATUS_FAIL;
    }
    //bt_timer_ext_check_status(0);
    LOG_I(BT_TIMER_EXT, "[sink][timer] bt_timer_ext_stop(%x)", timer_id);
    temp->timer_id = 0;
    if (temp == bt_sink_srv_time_running) {
        bt_timer_ext_os_stop();
        bt_sink_srv_time_running = bt_timer_ext_find_recent(current);
        if (bt_sink_srv_time_running != NULL) {
            bt_timer_ext_os_start(bt_sink_srv_time_running->time_tick - current);
        }
    }
    xSemaphoreGive((SemaphoreHandle_t)bt_timer_ext_semaphore);
    return BT_TIMER_EXT_STATUS_SUCCESS;
}

void bt_timer_ext_check_status(void)
{
    uint32_t current = 0;
    int32_t diff = 0;
    uint32_t timer_id = 0;
    uint32_t data = 0;
    uint32_t i;
    bt_timer_ext_timeout_callback_t timer_cb_array[BT_TIMER_EXT_INSTANCE_NUM] = {0};
    xSemaphoreTake((SemaphoreHandle_t)bt_timer_ext_semaphore, portMAX_DELAY);
    current = bt_timer_ext_get_current_tick();
    for (i = 0; i < BT_TIMER_EXT_INSTANCE_NUM; i++) {
        if (bt_timer_ext_array[i].timer_id != 0) {
            diff = (int32_t)(bt_timer_ext_array[i].time_tick - current);
            if (diff <= 0) {
                timer_id = bt_timer_ext_array[i].timer_id;
                data = bt_timer_ext_array[i].data;
                bt_timer_ext_array[i].timer_id = 0;
                LOG_I(BT_TIMER_EXT, "[sink][timer] BTsink timerid(%x), expired!\r\n", timer_id);
                timer_cb_array[i] = bt_timer_ext_array[i].cb;
                //bt_timer_ext_array[i].cb(timer_id, data);
            }
        }
    }
    if ((bt_sink_srv_time_running) && (bt_sink_srv_time_running->timer_id == 0)) {
        bt_sink_srv_time_running = NULL;
        current = bt_timer_ext_get_current_tick();
        bt_sink_srv_time_running = bt_timer_ext_find_recent(current);
        if (bt_sink_srv_time_running) {
            bt_timer_ext_os_start(bt_sink_srv_time_running->time_tick - current);
        }
    }
    xSemaphoreGive((SemaphoreHandle_t)bt_timer_ext_semaphore);

    for (i = 0; i < BT_TIMER_EXT_INSTANCE_NUM; i++) {
        if (timer_cb_array[i] != NULL) {
            timer_cb_array[i](timer_id, data);
        }
    }
}

void bt_timer_ext_handle_interrupt(void)
{
    uint32_t current_events;
    taskDISABLE_INTERRUPTS();
    current_events = bt_sink_srv_events;
    bt_sink_srv_events = 0;
    taskENABLE_INTERRUPTS();

    if (current_events & BT_TIMER_EXT_EXPIRED) {
        bt_timer_ext_check_status();
    }
    return;
}

bt_timer_ext_t* bt_timer_ext_find(uint32_t timer_id)
{
    bt_timer_ext_t* timer = NULL;
    uint32_t i;
    for (i = 0; i < BT_TIMER_EXT_INSTANCE_NUM; i++) {
        if (bt_timer_ext_array[i].timer_id == timer_id)
        {
            timer = (bt_timer_ext_t*)(bt_timer_ext_array + i);
            break;
        }
    }
    return timer;
}

static const bt_timer_ext_t* bt_timer_ext_find_recent(uint32_t current)
{
    const bt_timer_ext_t* timer = NULL;
    int32_t diff, diff2;
    uint32_t i;
    for (i = 0; i < BT_TIMER_EXT_INSTANCE_NUM; i++) {
        if (bt_timer_ext_array[i].timer_id != 0)
        {
            diff = (int32_t)(bt_timer_ext_array[i].time_tick - current);
            if (diff <= 0) {
                continue;
            }
            else if (timer == NULL) {
                timer = bt_timer_ext_array + i;
            }
            else {
                diff2 = (int32_t)(timer->time_tick - current);
                if (diff < diff2)
                {
                    timer = bt_timer_ext_array + i;
                }

            }
        }
    }
    return timer;
}

static uint32_t bt_timer_ext_get_current_tick(void)
{
    return xTaskGetTickCount();
}

static void bt_timer_ext_expire(TimerHandle_t timer)
{
    taskDISABLE_INTERRUPTS();
    bt_sink_srv_events |= BT_TIMER_EXT_EXPIRED;
    taskENABLE_INTERRUPTS();
    bt_trigger_interrupt(1);
}
    
static uint32_t bt_timer_ext_os_is_active(void)
{
    if ((bt_sink_srv_os_timer != NULL) && (xTimerIsTimerActive(bt_sink_srv_os_timer) != pdFALSE)) {
        return 1;
    }
    else {
        return 0;
    }
}


static void bt_timer_ext_os_start(uint32_t tick)
{
    if (bt_sink_srv_os_timer == NULL) {
        return;
    }
    if (bt_timer_ext_os_is_active() == 1) {
        bt_timer_ext_os_stop();
    }
    xTimerChangePeriod(bt_sink_srv_os_timer, tick, portMAX_DELAY);
    xTimerReset(bt_sink_srv_os_timer, portMAX_DELAY);
}

static void bt_timer_ext_os_stop(void)
{
    if ((bt_sink_srv_os_timer != NULL) && (bt_timer_ext_os_is_active() == 1)) {
        xTimerStop(bt_sink_srv_os_timer, portMAX_DELAY);
    }
}
#endif

