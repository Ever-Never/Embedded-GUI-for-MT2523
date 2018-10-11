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


#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "gnss_timer.h"
#include "gnss_log.h"
#include "semphr.h"
// timer usage demo
#define GNSS_TIMER_POOL_SIZE 10
#define GNSS_TIMER_ONCE_TYPE 0
#define GNSS_TIMER_REGULAR_TYPE 1
#define GNSS_TIMER_DUMMY 100 //only used for timer init
#define GNSS_TIMER_INVALID -1
#define GNSSTIMERLOG(fmt,args...) //do{ LOG_I(GNSS_TAG, "[GNSSTIMER]" fmt, ##args); } while(0)

#define GNSS_TIME_MUTEX_CREATE xSemaphoreCreateBinary()
#define GNSS_TIME_MUTEX_DELETE(x) vSemaphoreDelete(x)
#define GNSS_TIMER_TAKE_MUTEX do { GNSSTIMERLOG("wait mutex\n"); } while (gnss_timer_semaphore && xSemaphoreTake(gnss_timer_semaphore, 1000) != pdTRUE)
#define GNSS_TIMER_GIVE_MUTEX xSemaphoreGive(gnss_timer_semaphore)
 
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
static SemaphoreHandle_t gnss_timer_semaphore = NULL;
static gnss_timer_expiry_func timeout_notify_callback;
static void gnss_timer_callback( TimerHandle_t timer_handle );
static void move_exp_timer_to_excute_list(void);

/**
* @brief       Init GNSS timer
* @param[in]   notify_callback: timeout notify callback
* @return      None
*/
void gnss_timer_init(gnss_timer_expiry_func notify_callback)
{
    int32_t i;
    GNSSTIMERLOG("gnss_timer_init, is_init:%d\n", is_init);
    if (is_init) {
        return;
    }
    gnss_timer_semaphore = GNSS_TIME_MUTEX_CREATE;
    timeout_notify_callback = notify_callback;
    is_init = 1;
    
    g_gnss_timer = xTimerCreate(    "GNSS Timer",       // Just a text name, not used by the kernel.
        ( GNSS_TIMER_DUMMY ),   // The timer period in ticks.
        pdFALSE,        // The timers will not auto-reload themselves when they expire.
        ( void * ) 0,  // Assign each timer a unique id equal to its array index.
        gnss_timer_callback // Each timer calls the same callback when it expires.
        );
    if (g_gnss_timer == NULL) {
        // init fail...
    }
    
    timer_header = GNSS_TIMER_INVALID;
    excute_header = GNSS_TIMER_INVALID;
    free_header = 0;
    for (i = 0; i < GNSS_TIMER_POOL_SIZE - 1; i++) {
        gnss_timer_pool[i].next_timer = i+1;
    }
    gnss_timer_pool[i].next_timer = GNSS_TIMER_INVALID; // end of the free list
    GNSS_TIMER_GIVE_MUTEX;
}

/**
* @brief       Deinit GNSS timer
* @return      None
*/
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
    
    xTimerDelete(g_gnss_timer, 0);
    is_gnss_timer_actived = 0;
    g_gnss_timer = 0;
    timer_header = GNSS_TIMER_INVALID;
    excute_header = GNSS_TIMER_INVALID;
    free_header = 0;
    for (i = 0; i < GNSS_TIMER_POOL_SIZE - 1; i++) {
        gnss_timer_pool[i].next_timer = i+1;
        gnss_timer_pool[i].callback = NULL;
    }
    gnss_timer_pool[i].next_timer = GNSS_TIMER_INVALID;
    memset(regular_timer_excute_list, 0, sizeof(regular_timer_excute_list));
    //GNSS_TIMER_GIVE_MUTEX;
    tempSemaph = gnss_timer_semaphore;
    gnss_timer_semaphore = NULL;
    GNSS_TIME_MUTEX_DELETE(tempSemaph);
}

/**
* @brief       Active timer
* @return      None
*/
static void gnss_run_timer()
{
    
    GNSSTIMERLOG("gnss_run_timer timer_header:%d, is_gnss_timer_actived:%d\n", timer_header, is_gnss_timer_actived);
    if (timer_header != GNSS_TIMER_INVALID && !is_gnss_timer_actived) {
        is_gnss_timer_actived = 1;
        systick_flag = xTaskGetTickCount();
        xTimerChangePeriod(g_gnss_timer, gnss_timer_pool[timer_header].tick_offset, 0);
    }
}

/**
* @brief       Get the time offset between systick_flag and current time
* @return      int32_t: Time offset
*/
static int32_t get_ajust_timer_offset()
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

/**
* @brief       stop timer
* @return      None
*/
static void gnss_stop_run_timer()
{
    GNSSTIMERLOG("gnss_stop_run_timer\n");
    xTimerStop(g_gnss_timer, 0);
    is_gnss_timer_actived = 0;
}

/**
* @brief       Ajust timer when timer header is change
* @return      None
*/
static void gnss_ajust_timer()
{
    TickType_t curr_tick;
    int32_t duration;
    //xTimerStop(g_gnss_timer, 0);
    GNSSTIMERLOG("gnss_ajust_timer timer_header:%d\n", timer_header);
    
    if (timer_header == GNSS_TIMER_INVALID) {
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
            timeout_notify_callback();
        } else {
            GNSSTIMERLOG("period changed\n");
            xTimerChangePeriod(g_gnss_timer, (TickType_t) duration, 0);
        }
    }
}

/**
* @brief       Alloc a timer slot from free list
* @return      None
*/
static int32_t get_timer_item_from_free_list()
{
    int32_t ret;
    ret = free_header;
    if (free_header != GNSS_TIMER_INVALID) {
        free_header = gnss_timer_pool[free_header].next_timer;
    }
    
    GNSSTIMERLOG("get_timer_item_from_free_list,free_header:%d\n", free_header);
    return ret;
}

/**
* @brief       Free a timer slot to free list
* @return      None
*/
static void put_timer_item_to_free_list(int32_t item_ind)
{
    GNSSTIMERLOG("put_timer_item_to_free_list:%d\n", item_ind);
    gnss_timer_pool[item_ind].next_timer = free_header;
    free_header = item_ind;
}

/**
* @brief       Insert a timer slot to waiting list
* @return      None
*/
static void insert_to_timer_list(int32_t item_ind)
{
    int32_t ind;
    int32_t pre_ind = GNSS_TIMER_INVALID;
    
    GNSSTIMERLOG("insert_to_timer_list\n");
    GNSSTIMERLOG("[IIB]timer_header:%d, excute_header:%d, free_header:%d\n", timer_header, excute_header, free_header);
    GNSSTIMERLOG("ITTL, tick_offset:%d\n", gnss_timer_pool[item_ind].tick_offset);
    if (timer_header == GNSS_TIMER_INVALID) { // means not timer id in list
        gnss_timer_pool[item_ind].next_timer = timer_header;
        timer_header = item_ind;
        
        // start timer
        gnss_run_timer();
    } else {
        gnss_timer_pool[item_ind].tick_offset += get_ajust_timer_offset();
        GNSSTIMERLOG("ITTL, ajusted tick_offset:%d\n", gnss_timer_pool[item_ind].tick_offset);
        ind = timer_header;
        while(ind != GNSS_TIMER_INVALID) {
            GNSSTIMERLOG("time[%d] offset:%d\n", ind, gnss_timer_pool[ind].tick_offset);
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
        } else if (ind == GNSS_TIMER_INVALID) {
            gnss_timer_pool[pre_ind].next_timer = item_ind;
            gnss_timer_pool[item_ind].next_timer = GNSS_TIMER_INVALID;
        } else {
            gnss_timer_pool[item_ind].next_timer = gnss_timer_pool[pre_ind].next_timer;
            gnss_timer_pool[pre_ind].next_timer = item_ind;
        }
    }
    GNSSTIMERLOG("[IIE]timer_header:%d, excute_header:%d, free_header:%d\n", timer_header, excute_header, free_header);
}

/**
* @brief       Insert a timer slot to excute list
* @return      None
*/
static void append_excute_time_list(int32_t tid)
{
    int32_t ind;
    GNSSTIMERLOG("append_excute_time_list,tid:%d, excute header:%d\n", tid, excute_header);
    if (excute_header == GNSS_TIMER_INVALID) {
        gnss_timer_pool[tid].next_timer = excute_header;
        excute_header = tid;
    } else {
        ind = excute_header;
        while (gnss_timer_pool[ind].next_timer != GNSS_TIMER_INVALID) {
            ind = gnss_timer_pool[ind].next_timer;
        }
        gnss_timer_pool[tid].next_timer = GNSS_TIMER_INVALID;
        gnss_timer_pool[ind].next_timer = tid;
    }
}

/**
* @brief       Run the timeout callback in excute list timer
* @return      None
*/
void excute_timer()
{
    int32_t timer_item;
    int32_t ind = 0;
    gnss_timer_callback_f callback[GNSS_TIMER_POOL_SIZE] = {0};
    int32_t callback_parm[GNSS_TIMER_POOL_SIZE];
    int32_t regular_timer_list[GNSS_TIMER_POOL_SIZE];
    GNSSTIMERLOG("excute_timer\n");
    GNSS_TIMER_TAKE_MUTEX;
    while (excute_header != GNSS_TIMER_INVALID) {
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

/**
* @brief       Move timeout timer from waiting list to excute list
* @return      None
*/
static void move_exp_timer_to_excute_list()
{
    int32_t offset = gnss_timer_pool[timer_header].tick_offset;
    int32_t timer_ind;
    int32_t regular_temp_timer_header = GNSS_TIMER_INVALID;
    int32_t regular_temp_timer_tail = GNSS_TIMER_INVALID;
    GNSSTIMERLOG("move_exp_timer_to_excute_list\n");
    GNSSTIMERLOG("[MMB]timer_header:%d, excute_header:%d, free_header:%d\n", timer_header, excute_header, free_header);
    if (timer_header == GNSS_TIMER_INVALID) { // means not timer id in list
        return;
    }
    
    do {
        int32_t removed_timer_header = timer_header;
        GNSSTIMERLOG("[MMD]header[%d] offset[%d] next timer[%d]\n", 
            removed_timer_header,
            gnss_timer_pool[removed_timer_header].tick_offset,
            gnss_timer_pool[removed_timer_header].next_timer);
        timer_header = gnss_timer_pool[timer_header].next_timer;
        if (gnss_timer_pool[removed_timer_header].flag == GNSS_TIMER_ONCE_TYPE) {
            append_excute_time_list(removed_timer_header);
        } else if (gnss_timer_pool[removed_timer_header].flag == GNSS_TIMER_REGULAR_TYPE) {
            regular_timer_excute_list[removed_timer_header]++;
            // add timer item to temp list; those timer should be reinsert to wating timer list
            if (regular_temp_timer_header == GNSS_TIMER_INVALID) {
                regular_temp_timer_header = regular_temp_timer_tail = removed_timer_header;
                gnss_timer_pool[removed_timer_header].next_timer = GNSS_TIMER_INVALID;
            } else {
                gnss_timer_pool[regular_temp_timer_tail].next_timer = removed_timer_header;
                gnss_timer_pool[removed_timer_header].next_timer = GNSS_TIMER_INVALID;
                regular_temp_timer_tail = removed_timer_header;
            }
        }
        
    } while (timer_header != GNSS_TIMER_INVALID && offset == gnss_timer_pool[timer_header].tick_offset);
    GNSSTIMERLOG("[MMM]timer_header:%d, excute_header:%d, free_header:%d\n", timer_header, excute_header, free_header);
    
    timer_ind = timer_header;
    while (timer_ind != GNSS_TIMER_INVALID) {
        gnss_timer_pool[timer_ind].tick_offset -= offset;
        timer_ind = gnss_timer_pool[timer_ind].next_timer;
    }
    
    // insert temp list timer item to waiting list
    while (regular_temp_timer_header != GNSS_TIMER_INVALID) {
        int32_t temp = regular_temp_timer_header;
        regular_temp_timer_header = gnss_timer_pool[regular_temp_timer_header].next_timer;
        gnss_timer_pool[temp].tick_offset = gnss_timer_pool[temp].msec / portTICK_PERIOD_MS;
        GNSSTIMERLOG("[MMD]regular[%d] offset[%d] next timer[%d]\n", 
            temp,
            gnss_timer_pool[temp].tick_offset,
            gnss_timer_pool[temp].next_timer);
        insert_to_timer_list(temp);
    }
    if (timer_header == GNSS_TIMER_INVALID) {
        gnss_stop_run_timer();
    }
    GNSSTIMERLOG("[MME]timer_header:%d, excute_header:%d, free_header:%d\n", timer_header, excute_header, free_header);
}

/**
* @brief       Delete timeout timer
* @return      None
*/
static void delete_timer_item(int32_t tid)
{
    int32_t ind, pre_ind = GNSS_TIMER_INVALID;
    
    GNSSTIMERLOG("delete_timer_item:%d\n", tid);
    GNSSTIMERLOG("[DDB]timer_header:%d, excute_header:%d, free_header:%d\n", timer_header, excute_header, free_header);
    regular_timer_excute_list[tid] = 0;
    ind = excute_header;
    while (ind != GNSS_TIMER_INVALID) {
        if (ind == tid) {
            break;
        }
        pre_ind = ind;
        ind = gnss_timer_pool[ind].next_timer;
    }
    if (ind != GNSS_TIMER_INVALID) {
        if (ind == excute_header) {
            excute_header = gnss_timer_pool[excute_header].next_timer;
        } else {
            gnss_timer_pool[pre_ind].next_timer = gnss_timer_pool[ind].next_timer;
        }
        put_timer_item_to_free_list(ind);
    } else {
        ind = timer_header;
        while (ind != GNSS_TIMER_INVALID) {
            if (ind == tid) {
                break;
            }
            pre_ind = ind;
            ind = gnss_timer_pool[ind].next_timer;
        }
        if (ind != GNSS_TIMER_INVALID) {
            if (ind == timer_header) {
                timer_header = gnss_timer_pool[timer_header].next_timer;
                if (timer_header == GNSS_TIMER_INVALID
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

/**
* @brief       Create and start timer
* @param[in]   msec: Timeout duration
* @param[in]   call_back: callback function will be trigger when timeout
* @param[in]   mode: Indicate the timer is repeat one or not
* @return      int32_t: timer id
*/
static int32_t gnss_start_timer_internal(int32_t msec, gnss_timer_callback_f call_back, int32_t mode)
{
    int32_t tid;
    GNSSTIMERLOG("gnss_start_timer_internal,0:%d\n", msec);
    
    tid = get_timer_item_from_free_list();
    
    GNSSTIMERLOG("gnss_start_timer_internal,tid:%d\n", tid);
    if (tid == GNSS_TIMER_INVALID) {
        return tid;
    }
    gnss_timer_pool[tid].msec = msec;
    gnss_timer_pool[tid].tick_offset = msec/portTICK_PERIOD_MS;
    gnss_timer_pool[tid].callback = call_back;
    gnss_timer_pool[tid].flag = mode;
    if (gnss_timer_pool[tid].tick_offset == 0) {
        append_excute_time_list(tid);
        timeout_notify_callback();
    } else {
        insert_to_timer_list(tid);
    }
    return tid;
}

/**
* @brief       Start a no repeat timer
* @param[in]   msec: Timeout duration
* @param[in]   call_back: callback function will be trigger when timeout
* @return      int32_t: timer id
*/
int32_t gnss_start_timer(int32_t msec, gnss_timer_callback_f call_back)
{
    int32_t ret;
    GNSSTIMERLOG("gnss_start_timer:%d\n", msec);
    GNSS_TIMER_TAKE_MUTEX;
    ret = gnss_start_timer_internal(msec, call_back, GNSS_TIMER_ONCE_TYPE);
    GNSS_TIMER_GIVE_MUTEX;
    return ret;
}

/**
* @brief       Start a repeat timer
* @param[in]   msec: Timeout duration
* @param[in]   call_back: callback function will be trigger when timeout
* @return      int32_t: timer id
*/
int32_t gnss_start_repeat_timer(int32_t msec, gnss_timer_callback_f call_back)
{
    int32_t ret;
    GNSSTIMERLOG("gnss_start_repeat_timer:%d\n", msec);
    
    GNSS_TIMER_TAKE_MUTEX;
    ret = gnss_start_timer_internal(msec, call_back, GNSS_TIMER_REGULAR_TYPE);
    GNSS_TIMER_GIVE_MUTEX;
    return ret;
}

/**
* @brief       Stop a repeat timer
* @param[in]   timer_id: The Timer id
* @return      None
*/
void gnss_stop_timer(int32_t timer_id)
{
    GNSSTIMERLOG("gnss_stop_timer:%d\n", timer_id);
    
    GNSS_TIMER_TAKE_MUTEX;
    delete_timer_item(timer_id);
    GNSS_TIMER_GIVE_MUTEX;
}

/**
* @brief       Timeout callback which will be notify by timer task.
* @param[in]   timer_handle: The timer handle in timer task
* @return      None
*/
static void gnss_timer_callback( TimerHandle_t timer_handle )
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
    timeout_notify_callback();
    GNSS_TIMER_GIVE_MUTEX;
}


