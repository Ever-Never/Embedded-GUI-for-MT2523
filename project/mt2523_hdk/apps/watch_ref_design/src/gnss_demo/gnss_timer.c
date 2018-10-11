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


#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "gnss_timer.h"
#include "gnss_log.h"
#include "semphr.h"
// timer usage demo
#define GNSS_TIMER_POOL_SIZE 10
#define GNSS_TIMER_ONCE_TYPE 0
#define GNSS_TIMER_REGULAR_TYPE 1

#define GNSSTIMERLOG(fmt,args...) //do{ LOG_I(GNSS_TAG, "[GNSSTIMER]" fmt, ##args); } while(0)

#define GNSS_TIME_MUTEX_CREATE xSemaphoreCreateBinary()
#define GNSS_TIME_MUTEX_DELETE(x) vSemaphoreDelete(x)
#define GNSS_TIMER_TAKE_MUTEX do { GNSSTIMERLOG("wait mutex\n"); } while (gnss_timer_semaphore && xSemaphoreTake(gnss_timer_semaphore, 1000) != pdTRUE)
#define GNSS_TIMER_GIVE_MUTEX xSemaphoreGive(gnss_timer_semaphore)
//#define GNSS_TIMER_TAKE_MUTEX vTaskSuspendAll()
//#define GNSS_TIMER_GIVE_MUTEX xTaskResumeAll()

typedef void (*gnss_timer_callback_f)(int32_t tid);
typedef struct {
    TickType_t tick_offset;
    gnss_timer_callback_f callback;
    int32_t next_timer;
    int32_t msec;
    int32_t flag;
} gnss_timer_manage_t;

static TimerHandle_t g_gnss_timer;
static gnss_timer_manage_t gnss_timer_pool[GNSS_TIMER_POOL_SIZE];
static int32_t timer_header;
static int32_t free_header;
static int32_t excute_header;
static int32_t regular_timer_excute_list[GNSS_TIMER_POOL_SIZE];
static TickType_t systick_flag;
static int32_t is_gnss_timer_actived;
static int32_t is_init;
SemaphoreHandle_t gnss_timer_semaphore = NULL;

static void gnss_timer_callback( TimerHandle_t timer_handle );
static void move_exp_timer_to_excute_list(void);

extern void gnss_timer_expiry_notify(void);


void gnss_timer_init()
{
    int32_t i;
    GNSSTIMERLOG("gnss_timer_init, is_init:%d\n", is_init);
    if (is_init) {
        return;
    }
    gnss_timer_semaphore = GNSS_TIME_MUTEX_CREATE;
    is_init = 1;

    g_gnss_timer = xTimerCreate(    "GNSS Timer",       // Just a text name, not used by the kernel.
                    ( 100 ),   // The timer period in ticks.
                    pdFALSE,        // The timers will not auto-reload themselves when they expire.
                    ( void * ) 0,  // Assign each timer a unique id equal to its array index.
                    gnss_timer_callback // Each timer calls the same callback when it expires.
                    );
    if (g_gnss_timer == NULL) {
        // init fail...
    }

    timer_header = -1;
    excute_header = -1;
    free_header = 0;
    for (i = 0; i < GNSS_TIMER_POOL_SIZE - 1; i++) {
        gnss_timer_pool[i].next_timer = i+1;
    }
    gnss_timer_pool[i].next_timer = -1; // end of the free list
    GNSS_TIMER_GIVE_MUTEX;
}

void gnss_timer_deinit()
{
    int32_t i;
    SemaphoreHandle_t tempSemaph;
    GNSSTIMERLOG("gnss_timer_deinit, is_init:%d\n", is_init);
    if (!is_init) {
        return;
    }
    GNSS_TIMER_TAKE_MUTEX;
    is_init = 0;

    xTimerDelete(g_gnss_timer, 1000);
    is_gnss_timer_actived = 0;
    g_gnss_timer = 0;
    timer_header = -1;
    excute_header = -1;
    free_header = 0;
    for (i = 0; i < GNSS_TIMER_POOL_SIZE - 1; i++) {
        gnss_timer_pool[i].next_timer = i+1;
        gnss_timer_pool[i].callback = NULL;
    }
    gnss_timer_pool[i].next_timer = -1;
    memset(regular_timer_excute_list, 0, sizeof(regular_timer_excute_list));
    //GNSS_TIMER_GIVE_MUTEX;
    tempSemaph = gnss_timer_semaphore;
    gnss_timer_semaphore = NULL;
    GNSS_TIME_MUTEX_DELETE(tempSemaph);
}

void gnss_run_timer()
{

    GNSSTIMERLOG("gnss_run_timer timer_header:%d, is_gnss_timer_actived:%d\n", timer_header, is_gnss_timer_actived);
    if (timer_header != -1 && !is_gnss_timer_actived) {
        is_gnss_timer_actived = 1;
        systick_flag = xTaskGetTickCount();
        xTimerChangePeriod(g_gnss_timer, gnss_timer_pool[timer_header].tick_offset, 10);
    }
}

int32_t get_ajust_timer_offset()
{
    int32_t ret = 0;
    //GNSSTIMERLOG("get_ajust_timer_offset\n");
    if (is_gnss_timer_actived) {
        // need to take care of overflow??? curr tick count get form xTaskGetTickCount() may small than systick_flag!!!
        ret = (xTaskGetTickCount() - systick_flag);
    } else {
        systick_flag = xTaskGetTickCount();
    }
    GNSSTIMERLOG("get_ajust_timer_offset. offset:%d\n", ret);
    return ret;
}

void gnss_stop_run_timer()
{
    GNSSTIMERLOG("gnss_stop_run_timer\n");
    xTimerStop(g_gnss_timer, 0);
    is_gnss_timer_actived = 0;
}

void gnss_ajust_timer()
{
    TickType_t curr_tick;
    int32_t duration;
    //xTimerStop(g_gnss_timer, 0);
    GNSSTIMERLOG("gnss_ajust_timer timer_header:%d\n", timer_header);

    if (timer_header == -1) {
        xTimerStop(g_gnss_timer, 0);
        is_gnss_timer_actived = 0;
        return;// no need to start timer again.
    }
    curr_tick = xTaskGetTickCount();
    if (curr_tick >= systick_flag) {
        duration = gnss_timer_pool[timer_header].tick_offset - (curr_tick - systick_flag);
        GNSSTIMERLOG("change timer period, curr duration:%d, change period:%d\n",
            curr_tick - systick_flag,
            duration);
        //in ideal case, gnss_timer_pool[timer_header].tick_offset - (curr_tick - systick_flag) should be bigger than 0
        //but there are no ideal case, may be small than 0, should be take care?
        if (duration <= 0) {
            move_exp_timer_to_excute_list();
            gnss_timer_expiry_notify();
        } else {
            GNSSTIMERLOG("period changed\n");
            xTimerChangePeriod(g_gnss_timer, (TickType_t) duration, 10);
        }
    }
}

int32_t get_timer_item_from_free_list()
{
    int32_t ret;
    ret = free_header;
    if (free_header != -1) {
        free_header = gnss_timer_pool[free_header].next_timer;
    }

    GNSSTIMERLOG("get_timer_item_from_free_list,free_header:%d\n", free_header);
    return ret;
}

void put_timer_item_to_free_list(int32_t item_ind)
{
    GNSSTIMERLOG("put_timer_item_to_free_list:%d\n", item_ind);
    gnss_timer_pool[item_ind].next_timer = free_header;
    free_header = item_ind;
}

void insert_to_timer_list(int32_t item_ind)
{
    int32_t ind;
    int32_t pre_ind = 0;

    GNSSTIMERLOG("insert_to_timer_list\n");
    GNSSTIMERLOG("[IIB]timer_header:%d, excute_header:%d, free_header:%d\n", timer_header, excute_header, free_header);
    GNSSTIMERLOG("ITTL, tick_offset:%d\n", gnss_timer_pool[item_ind].tick_offset);
    if (timer_header == -1) { // means not timer id in list
        gnss_timer_pool[item_ind].next_timer = timer_header;
        timer_header = item_ind;

        // start timer
        gnss_run_timer();
    } else {
        gnss_timer_pool[item_ind].tick_offset += get_ajust_timer_offset();
        GNSSTIMERLOG("ITTL, ajusted tick_offset:%d\n", gnss_timer_pool[item_ind].tick_offset);
        ind = timer_header;
        while(ind != -1) {
            if (gnss_timer_pool[item_ind].tick_offset < gnss_timer_pool[ind].tick_offset) {
                break;
            }
            pre_ind = ind;
            ind = gnss_timer_pool[ind].next_timer;
        }
        if (ind == timer_header) {
            gnss_timer_pool[item_ind].next_timer = timer_header;
            timer_header = item_ind;
            gnss_ajust_timer();
        } else if (ind == -1) {
            gnss_timer_pool[pre_ind].next_timer = item_ind;
            gnss_timer_pool[item_ind].next_timer = -1;
        } else {
            gnss_timer_pool[item_ind].next_timer = gnss_timer_pool[pre_ind].next_timer;
            gnss_timer_pool[pre_ind].next_timer = item_ind;
        }
    }
    GNSSTIMERLOG("[IIE]timer_header:%d, excute_header:%d, free_header:%d\n", timer_header, excute_header, free_header);
}

void append_excute_time_list(int32_t tid)
{
    int32_t ind;
    GNSSTIMERLOG("append_excute_time_list,tid:%d, excute header:%d\n", tid, excute_header);
    if (excute_header == -1) {
        gnss_timer_pool[tid].next_timer = excute_header;
        excute_header = tid;
    } else {
        ind = excute_header;
        while (gnss_timer_pool[ind].next_timer != -1) {
            ind = gnss_timer_pool[ind].next_timer;
        }
        gnss_timer_pool[tid].next_timer = -1;
        gnss_timer_pool[ind].next_timer = tid;
    }
}

void excute_timer()
{
    int32_t timer_item;
    int32_t ind = 0;
    gnss_timer_callback_f callback[10] = {0};
    int32_t callback_parm[10];
    int32_t regular_timer_list[10];
    GNSSTIMERLOG("excute_timer\n");
    GNSS_TIMER_TAKE_MUTEX;
    while (excute_header != -1) {
        timer_item = excute_header;
        excute_header = gnss_timer_pool[excute_header].next_timer;
        if (gnss_timer_pool[timer_item].callback) {
            callback[ind] = gnss_timer_pool[timer_item].callback;
            callback_parm[ind++] = timer_item;
        }
        put_timer_item_to_free_list(timer_item);
    }
    GNSS_TIMER_GIVE_MUTEX;

    ind = 0;
    while (callback[ind]) {
        callback[ind](callback_parm[ind]);
        ind++;
    }
    GNSS_TIMER_TAKE_MUTEX;
    memcpy(regular_timer_list, regular_timer_excute_list, sizeof(regular_timer_excute_list));
    memset(regular_timer_excute_list, 0, sizeof(regular_timer_excute_list));
    for (ind = 0; ind < GNSS_TIMER_POOL_SIZE; ind++) {
    callback[ind] = gnss_timer_pool[ind].callback;
    }
    GNSS_TIMER_GIVE_MUTEX;

    for (ind = 0; ind < GNSS_TIMER_POOL_SIZE; ind++) {
        if (callback[ind]) {
            while (regular_timer_list[ind]) {
                callback[ind](ind);
                regular_timer_list[ind]--;
            }
        } else {
            regular_timer_list[ind] = 0;
        }
    }
}

void move_exp_timer_to_excute_list()
{
    int32_t offset = gnss_timer_pool[timer_header].tick_offset;
    int32_t timer_ind;
    int32_t regular_temp_timer_header = -1;
    int32_t regular_temp_timer_tail = -1;
    GNSSTIMERLOG("move_exp_timer_to_excute_list\n");
    GNSSTIMERLOG("[MMB]timer_header:%d, excute_header:%d, free_header:%d\n", timer_header, excute_header, free_header);
    if (timer_header == -1) { // means not timer id in list
        return;
    }

    do {
        int32_t removed_timer_header = timer_header;
        timer_header = gnss_timer_pool[timer_header].next_timer;
        if (gnss_timer_pool[removed_timer_header].flag == GNSS_TIMER_ONCE_TYPE) {
            append_excute_time_list(removed_timer_header);
        } else if (gnss_timer_pool[removed_timer_header].flag == GNSS_TIMER_REGULAR_TYPE) {
            regular_timer_excute_list[removed_timer_header]++;
            // add timer item to temp list; those timer should be reinsert to wating timer list
            if (regular_temp_timer_header == -1) {
                regular_temp_timer_header = regular_temp_timer_tail = removed_timer_header;
                gnss_timer_pool[removed_timer_header].next_timer = -1;
            } else {
                gnss_timer_pool[regular_temp_timer_tail].next_timer = removed_timer_header;
                gnss_timer_pool[removed_timer_header].next_timer = -1;
                regular_temp_timer_tail = removed_timer_header;
            }
        }

    } while (timer_header != -1 && offset == gnss_timer_pool[timer_header].tick_offset);

    timer_ind = timer_header;
    while (timer_ind != -1) {
        gnss_timer_pool[timer_ind].tick_offset -= offset;
        timer_ind = gnss_timer_pool[timer_ind].next_timer;
    }

    // insert temp list timer item to waiting list
    while (regular_temp_timer_header != -1) {
        gnss_timer_pool[regular_temp_timer_header].tick_offset = gnss_timer_pool[regular_temp_timer_header].msec / portTICK_PERIOD_MS;
        insert_to_timer_list(regular_temp_timer_header);
        regular_temp_timer_header = gnss_timer_pool[regular_temp_timer_header].next_timer;
    }
    if (timer_header == -1) {
        gnss_stop_run_timer();
    }
    GNSSTIMERLOG("[MME]timer_header:%d, excute_header:%d, free_header:%d\n", timer_header, excute_header, free_header);
}

void delete_timer_item(int32_t tid)
{
    int32_t ind, pre_ind = 0;

    GNSSTIMERLOG("delete_timer_item:%d\n", tid);
    GNSSTIMERLOG("[DDB]timer_header:%d, excute_header:%d, free_header:%d\n", timer_header, excute_header, free_header);
    regular_timer_excute_list[tid] = 0;
    ind = excute_header;
    while (ind != -1) {
        if (ind == tid) {
            break;
        }
        pre_ind = ind;
        ind = gnss_timer_pool[ind].next_timer;
    }
    if (ind != -1) {
        if (ind == excute_header) {
            excute_header = gnss_timer_pool[excute_header].next_timer;
        } else {
            gnss_timer_pool[pre_ind].next_timer = gnss_timer_pool[ind].next_timer;
        }
        put_timer_item_to_free_list(ind);
    } else {
        ind = timer_header;
        while (ind != -1) {
            if (ind == tid) {
                break;
            }
            pre_ind = ind;
            ind = gnss_timer_pool[ind].next_timer;
        }
        if (ind != -1) {
            if (ind == timer_header) {
                timer_header = gnss_timer_pool[timer_header].next_timer;
                if (timer_header == -1
                    || gnss_timer_pool[ind].tick_offset < gnss_timer_pool[timer_header].tick_offset) {
                    gnss_ajust_timer();
                }
            } else {
                gnss_timer_pool[pre_ind].next_timer = gnss_timer_pool[ind].next_timer;
            }
            put_timer_item_to_free_list(ind);
        }
    }
    GNSSTIMERLOG("[DDE]timer_header:%d, excute_header:%d, free_header:%d\n", timer_header, excute_header, free_header);
}

int32_t gnss_start_timer_internal(int32_t msec, gnss_timer_callback_f call_back, int32_t mode)
{
    int32_t tid;
    GNSSTIMERLOG("gnss_start_timer_internal,0:%d\n", msec);

    tid = get_timer_item_from_free_list();

    GNSSTIMERLOG("gnss_start_timer_internal,tid:%d\n", tid);
    if (tid == -1) {
        return tid;
    }
    gnss_timer_pool[tid].msec = msec;
    gnss_timer_pool[tid].tick_offset = msec/portTICK_PERIOD_MS;
    gnss_timer_pool[tid].callback = call_back;
    gnss_timer_pool[tid].flag = mode;
    if (gnss_timer_pool[tid].tick_offset == 0) {
        append_excute_time_list(tid);
        gnss_timer_expiry_notify();
    } else {
        insert_to_timer_list(tid);
    }
    return tid;
}

int32_t gnss_start_timer(int32_t msec, gnss_timer_callback_f call_back)
{
    int32_t ret;
    GNSSTIMERLOG("gnss_start_timer:%d\n", msec);
    GNSS_TIMER_TAKE_MUTEX;
    ret = gnss_start_timer_internal(msec, call_back, GNSS_TIMER_ONCE_TYPE);
    GNSS_TIMER_GIVE_MUTEX;
    return ret;
}

int32_t gnss_start_repeat_timer(int32_t msec, gnss_timer_callback_f call_back)
{
    int32_t ret;
    GNSSTIMERLOG("gnss_start_repeat_timer:%d\n", msec);

    GNSS_TIMER_TAKE_MUTEX;
    ret = gnss_start_timer_internal(msec, call_back, GNSS_TIMER_REGULAR_TYPE);
    GNSS_TIMER_GIVE_MUTEX;
    return ret;
}

void gnss_stop_timer(int32_t timer_id)
{
    GNSSTIMERLOG("gnss_stop_timer:%d\n", timer_id);

    GNSS_TIMER_TAKE_MUTEX;
    delete_timer_item(timer_id);
    GNSS_TIMER_GIVE_MUTEX;
}

void gnss_timer_callback( TimerHandle_t timer_handle )
{
    GNSSTIMERLOG("GNSS timer callback:%d\n", (int)xTaskGetTickCount());

    if (!is_init) {
        return;
    }
    GNSS_TIMER_TAKE_MUTEX;
    if (gnss_timer_semaphore == NULL) {
        return;
    }
    is_gnss_timer_actived = 0;
    move_exp_timer_to_excute_list();
    gnss_run_timer();
    gnss_timer_expiry_notify();
    GNSS_TIMER_GIVE_MUTEX;
}


