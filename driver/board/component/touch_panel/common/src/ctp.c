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

#ifdef MTK_CTP_ENABLE

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

#include "touch_panel_custom_config.h"
#include "ctp.h"
#include "bsp_ctp.h"
#include "hal_gpt.h"
#include "hal_nvic.h"
#include "hal_log.h"
#include "hal_gpt_internal.h"
#include "hal_eint_internal.h"
#include "task_def.h"

#include "timers.h"

ctp_config_t                    ctp_config_info;
ctp_customize_function_struct   *ctp_fun_ptr;
touch_panel_data_struct_t       ctp_context;
bsp_ctp_multiple_event_t        g_ctp_event;
touch_panel_buffer_data_t       touch_panel_data_buffer;
ctp_calibration_t               ctp_calibration;
TimerHandle_t ctp_timer;


QueueHandle_t ctp_queue_handle;
TaskHandle_t  ctp_task_handle;

const TickType_t ctp_xTicksToWait = 20;

#ifdef CTP_DEBUG_PERFORMANCE
uint32_t ctp_debug_time[2];
#endif

extern xSemaphoreHandle g_ctp_i2c_dma_semaphore;

typedef enum {
    CTP_GET_PEN_DATA            = 1,
    CTP_CALL_USER_CALLBACK      = 2,
    CTP_CALL_TIMER_CALBACK      = 3
} ctp_msg_type_t;

typedef struct {
    char *src_mod;
    ctp_msg_type_t msg_id;
} ctp_msg_t;

static uint32_t tp_state_count[TP_SUPPORT_POINTS];

static int16_t  tp_accum_plh_x_diff[TP_SUPPORT_POINTS], tp_accum_plh_y_diff[TP_SUPPORT_POINTS];
static int16_t  tp_accum_prh_x_diff[TP_SUPPORT_POINTS], tp_accum_prh_y_diff[TP_SUPPORT_POINTS];
static uint32_t tp_state_start_tick[TP_SUPPORT_POINTS];

static bsp_ctp_event_status_t ctp_state_manchine_abort(int16_t x_diff, int16_t y_diff, uint16_t count);
static bsp_ctp_event_status_t ctp_state_manchine_pen_move(int16_t x_diff, int16_t y_diff, uint16_t count);
static bsp_ctp_event_status_t ctp_state_manchine_pen_down(int16_t x_diff, int16_t y_diff, uint16_t count);
static bsp_ctp_event_status_t ctp_state_manchine_pen_longtap_hold(int16_t x_diff, int16_t y_diff, uint16_t count);
static bsp_ctp_event_status_t ctp_state_manchine_pen_repeat_hold(int16_t x_diff, int16_t y_diff, uint16_t count);

#ifdef CTP_IT7258_SUPPORTED
extern void touch_panel_ite_custom_data_init(void);
ctp_customize_function_struct *ctp_ite_7258_get_custom_function(void);
#endif

#ifdef CTP_GT9137_SUPPORTED
extern void touch_panel_goodix_custom_data_init(void);
extern ctp_customize_function_struct *ctp_goodix_gt9xx_get_custom_function(void);
#endif

typedef bsp_ctp_event_status_t (*ctp_state_machine_function)(int16_t x_diff, int16_t y_diff, int16_t count);

const ctp_state_machine_function ctp_state_machine[] = {
    (ctp_state_machine_function)ctp_state_manchine_abort,            /*0 if last event is CTP_PEN_UP aseert*/
    (ctp_state_machine_function)ctp_state_manchine_pen_down,         /*1 if last event is CTP_PEN_DOWN*/
    (ctp_state_machine_function)ctp_state_manchine_pen_move,         /*2 if last event is CTP_PEN_MOVE*/
    (ctp_state_machine_function)ctp_state_manchine_pen_move,         /*3 if last evetn is CTP_PEN_LONGTAP*/
    (ctp_state_machine_function)ctp_state_manchine_pen_move,         /*4 if last evetn is CTP_PEN_REPEAT*/
    (ctp_state_machine_function)ctp_state_manchine_pen_longtap_hold, /*5 if last evetn is CTP_PEN_LONGTAP_HOLD*/
    (ctp_state_machine_function)ctp_state_manchine_pen_repeat_hold,  /*6 if last evetn is CTP_PEN_REPEAT_HOLD*/
    (ctp_state_machine_function)ctp_state_manchine_abort,            /*7 if last evetn is CTP_PEN_ABORT*/
};

static uint32_t ctp_absolute_value(int16_t data)
{
    if (data < 0) {
        return (uint32_t)(-data);
    } else {
        return (uint32_t)(data);
    }
}

static bsp_ctp_event_status_t ctp_state_manchine_abort(int16_t x_diff, int16_t y_diff, uint16_t count)
{
    ctp_debug_printf("[CTP][state]ctp state machine abort: x_diff = %d, y_diff=%d, count = %d\r\n", x_diff, y_diff, count);
    assert(0);
    return CTP_PEN_DOWN;
}

static bsp_ctp_event_status_t ctp_state_manchine_pen_move(int16_t x_diff, int16_t y_diff, uint16_t count)
{
    tp_state_count[count]      = 0;
    tp_state_start_tick[count] = ctp_get_time_us();

    if ((ctp_absolute_value(x_diff) > ctp_context.pen_move_offset) || (ctp_absolute_value(y_diff) > ctp_context.pen_move_offset)) {
        return CTP_PEN_MOVE;
    } else {
        return CTP_PEN_REPEAT_HOLD; /*it may change to PEN_REPEAT in timer check.*/
    }
}

static bsp_ctp_event_status_t ctp_state_manchine_pen_down(int16_t x_diff, int16_t y_diff, uint16_t count)
{
    tp_accum_plh_x_diff[count] = 0;
    tp_accum_plh_y_diff[count] = 0; /*reset the difference whenever pen down.*/

    tp_accum_prh_x_diff[count] = 0;
    tp_accum_prh_y_diff[count] = 0; /*otherwise it would accumulate last pen down-up diff.*/

    tp_state_count[count]      = 0;
    tp_state_start_tick[count] = ctp_get_time_us();

    /*cosmos already handle longtap, do not need to check longtap*/
    if (ctp_context.pen_longtap_offset == 0) {
        return ctp_state_machine[CTP_PEN_MOVE](x_diff, y_diff, count);
    }
    if ((ctp_absolute_value(x_diff) > ctp_context.pen_longtap_offset) || (ctp_absolute_value(y_diff) > ctp_context.pen_longtap_offset)) {
        return CTP_PEN_MOVE;
    } else {
        tp_accum_plh_x_diff[count] += x_diff;
        tp_accum_plh_y_diff[count] += y_diff;
        return CTP_PEN_LONGTAP_HOLD; /*it may change to PEN_LOGNTAP in timer check.*/
    }

}

static bsp_ctp_event_status_t ctp_state_manchine_pen_longtap_hold(int16_t x_diff, int16_t y_diff, uint16_t count)
{
    /*cosmos already handle longtap, do not need to check longtap*/
    if (ctp_context.pen_longtap_offset == 0) {
        return ctp_state_machine[CTP_PEN_MOVE](x_diff, y_diff, count);
    }

    tp_state_count[count]++;
    tp_accum_plh_x_diff[count] += x_diff;
    tp_accum_plh_y_diff[count] += y_diff;

    if ((ctp_absolute_value(tp_accum_plh_x_diff[count]) > ctp_context.pen_longtap_offset) || \
            (ctp_absolute_value(tp_accum_plh_y_diff[count]) > ctp_context.pen_longtap_offset)) {
        tp_accum_plh_x_diff[count] = 0;
        tp_accum_plh_y_diff[count] = 0;
        return CTP_PEN_MOVE;
    } else {
        if (ctp_get_duration_time_us_to_ms(tp_state_start_tick[count]) >= ctp_context.pen_longtap_time) {
            tp_accum_plh_x_diff[count] = 0;
            tp_accum_plh_y_diff[count] = 0;
            return CTP_PEN_LONGTAP;
        } else {
            return CTP_PEN_LONGTAP_HOLD; //it may change to PEN_LOGNTAP in timer check.
        }
    }
}

static bsp_ctp_event_status_t ctp_state_manchine_pen_repeat_hold(int16_t x_diff, int16_t y_diff, uint16_t count)
{
    tp_state_count[count]++;
    tp_accum_prh_x_diff[count] += x_diff;
    tp_accum_prh_y_diff[count] += y_diff;

    if ((ctp_absolute_value(tp_accum_prh_x_diff[count]) > ctp_context.pen_move_offset)  || \
            (ctp_absolute_value(tp_accum_prh_y_diff[count]) > ctp_context.pen_move_offset)) {
        tp_accum_prh_x_diff[count] = tp_accum_prh_y_diff[count] = 0;
        return CTP_PEN_MOVE;
    } else {
        if (ctp_get_duration_time_us_to_ms(tp_state_start_tick[count]) >= ctp_context.pen_repeat_time) {
            tp_accum_prh_x_diff[count] = 0;
            tp_accum_prh_y_diff[count] = 0;
            return CTP_PEN_REPEAT;
        } else {
            return CTP_PEN_REPEAT_HOLD;    //it may change to PEN_REPEAT in timer check.
        }
    }
}

void ctp_get_buf_roomleft(uint16_t *pleft)
{
    if (touch_panel_data_buffer.touch_buffer_rindex <= touch_panel_data_buffer.touch_buffer_windex) {
        *pleft = TOUCH_PANEL_BUFFER_EVENT_SIZE - touch_panel_data_buffer.touch_buffer_windex + touch_panel_data_buffer.touch_buffer_rindex;
    } else {
        *pleft = touch_panel_data_buffer.touch_buffer_rindex - touch_panel_data_buffer.touch_buffer_windex;
    }
}

uint32_t ctp_get_time_us(void)
{
    uint32_t count;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &count);

    return count;
}

uint32_t ctp_get_duration_time_us_to_ms(uint32_t start_count)
{
    uint32_t current_count;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_count);
    if (current_count >= start_count) {
        return ((current_count - start_count) / 1000);
    } else {
        return ((0xffffffff - (start_count - current_count) + 1) / 1000);
    }
}

void ctp_delay_us(uint32_t time)
{
    hal_gpt_delay_us(time);

}
void ctp_delay_ms(uint32_t time)
{
#ifdef CTP_USED_TASK_DEALY
    vTaskDelay(time / portTICK_PERIOD_MS);
#else
    hal_gpt_delay_ms(time);
#endif
}

void ctp_mask_eint_interrupt(void)
{
    #ifdef CTP_DEBUG_CODE_FLOW
    #if defined(__GNUC__) 
    uint32_t xLinkRegAddr = (uint32_t)__builtin_return_address(0);
    #elif defined(__CC_ARM)
    uint32_t xLinkRegAddr = __return_address(); 
    #elif defined(__ICCARM__)
    uint32_t xLinkRegAddr = __get_LR();
    #endif 
    #endif
    
    hal_eint_mask(ctp_config_info.ctp_eint.eint_num);
    ctp_context.eint_mask = true;
    
    #ifdef CTP_DEBUG_CODE_FLOW
    ctp_debug_printf("mask interrupt,0x%x\r\n",xLinkRegAddr);
    #endif
}

void ctp_unmask_eint_interrupt(void)
{
    #ifdef CTP_DEBUG_CODE_FLOW
    #if defined(__GNUC__) 
    uint32_t xLinkRegAddr = (uint32_t)__builtin_return_address(0);
    #elif defined(__CC_ARM)
    uint32_t xLinkRegAddr = __return_address(); 
    #elif defined(__ICCARM__)
    uint32_t xLinkRegAddr = __get_LR();
    #endif 
    #endif
    
    eint_ack_interrupt(ctp_config_info.ctp_eint.eint_num);
    hal_eint_unmask(ctp_config_info.ctp_eint.eint_num);
    
    ctp_context.eint_mask = false;
    #ifdef CTP_DEBUG_CODE_FLOW
    ctp_debug_printf("unmask interrupt,0x%x\r\n",xLinkRegAddr);
    #endif
}

void ctp_mask_gpt_interrupt(void)
{

}

void ctp_unmask_gpt_interrupt(void)
{


}

void touch_panel_send_message(ctp_msg_type_t msg)
{
    #ifdef CTP_DEBUG_HANG_DUMP
    #if defined(__GNUC__) 
    uint32_t xLinkRegAddr = (uint32_t)__builtin_return_address(0);
    #elif defined(__CC_ARM)
    uint32_t xLinkRegAddr = __return_address(); 
    #elif defined(__ICCARM__)
    uint32_t xLinkRegAddr = __get_LR();
    #endif 
    #endif
    
    ctp_msg_t   msgs;
    BaseType_t  xHigherPriorityTaskWoken;
    uint32_t    queue_waiting;

    /* We have not woken a task at the start of the ISR*/
    xHigherPriorityTaskWoken = pdFALSE;

    msgs.msg_id = msg;

    queue_waiting = uxQueueMessagesWaitingFromISR(ctp_queue_handle);
    
    #ifdef CTP_DEBUG_HANG_DUMP
    ctp_debug_printf("queue_waiting =%d, 0x%x\r\n",queue_waiting,xLinkRegAddr);
    #endif

    if (xQueueSendFromISR(ctp_queue_handle, &msgs, &xHigherPriorityTaskWoken) != pdTRUE) {
        
        #ifdef CTP_DEBUG_HANG_DUMP
        ctp_debug_printf("[CTP] message queue full, queue_waiting=%d\r\n",queue_waiting);
        //assert(0);
        #else
        ctp_debug_printf("[CTP] message queue full, queue_waiting=%d\r\n",queue_waiting);
        #endif
    }

    /* Now the buffer is empty we can switch context if necessary.*/
    if (xHigherPriorityTaskWoken) {
        /*Actual macro used here is port specific.*/
        portYIELD_FROM_ISR(pdTRUE);
    }
}

void ctp_user_callback_handler(void)
{
    if ((ctp_context.send_touch_ilm == true) || (ctp_context.is_buff_full == true)) {

        /*send message to call user's callback*/
        ctp_debug_printf("[CTP][MSG]Inform user to get data, send_touch_ilm=%d, is_buff_full=%d\r\n", \
                     ctp_context.send_touch_ilm, ctp_context.is_buff_full);

        ctp_context.send_touch_ilm = false;

        touch_panel_send_message(CTP_CALL_USER_CALLBACK);
    }
}


bool ctp_calibrate_coordinate(bsp_ctp_multiple_event_t *tp_event)
{
    uint32_t i = 0;

    for (i = 0; i < tp_event->model; i++) { // check each point of last event in buffer.
        //ctp_debug_printf("[CTP][cali]before calibration,x=%d, y =%d, model:%d, i:%d\r\n",tp_event->points[i].x, tp_event->points[i].y, tp_event->model,i);
        tp_event->points[i].x = (uint32_t)(ctp_calibration.x_scope * tp_event->points[i].x + ctp_calibration.x_offset);
        tp_event->points[i].y = (uint32_t)(ctp_calibration.y_scope * tp_event->points[i].y + ctp_calibration.y_offset);

        if (tp_event->points[i].x < ctp_context.x_coord_start) {
            tp_event->points[i].x = ctp_context.x_coord_start;
        } else if (tp_event->points[i].x > ctp_context.x_coord_end) {
            tp_event->points[i].x = ctp_context.x_coord_end;
        }

        if (tp_event->points[i].y < ctp_context.y_coord_start) {
            tp_event->points[i].y = ctp_context.y_coord_start;
        } else if (tp_event->points[i].y > ctp_context.y_coord_end) {
            tp_event->points[i].y = ctp_context.y_coord_end;
        }
        //ctp_debug_printf("[CTP][cali]after calibration,x = %d, y = %d, model:%d, i:%d\r\n",tp_event->points[i].x, tp_event->points[i].y, tp_event->model,i);
    }


    return true;
}

void touch_panel_multiple_touch_flush_buffer(void)
{
    
    uint32_t mask;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    ctp_context.is_buff_full     = false;
    ctp_context.first_hold_valid = 0;

    //memset(&touch_panel_data_buffer, 0, sizeof(touch_panel_data_buffer));
    touch_panel_data_buffer.touch_buffer_rindex = 0;
    touch_panel_data_buffer.touch_buffer_windex = 0;
    touch_panel_data_buffer.touch_buffer_last_rindex = 0;
    touch_panel_data_buffer.touch_buffer_last_windex = 0;

    ctp_context.skip_unrelease_state = true;

    hal_nvic_restore_interrupt_mask(mask);

    ctp_debug_printf("flush buffer\r\n");

}

static void touch_panel_push_one_event(bsp_ctp_multiple_event_t *push_event)
{
    uint16_t index;
    uint16_t i;

    index = touch_panel_data_buffer.touch_buffer_windex;

    touch_panel_data_buffer.touch_buffer_last_windex = index; //save last windex



    touch_panel_data_buffer.event[index].model      = push_event->model;
    touch_panel_data_buffer.event[index].padding    = push_event->padding;
    touch_panel_data_buffer.event[index].time_stamp = push_event->time_stamp;

    for (i = 0; i < touch_panel_data_buffer.event[index].model; i++) {
        touch_panel_data_buffer.event[index].points[i].event = push_event->points[i].event;
        touch_panel_data_buffer.event[index].points[i].x = push_event->points[i].x;
        touch_panel_data_buffer.event[index].points[i].y = push_event->points[i].y;
    }

    touch_panel_data_buffer.touch_buffer_windex++;
    if (touch_panel_data_buffer.touch_buffer_windex >= TOUCH_PANEL_BUFFER_EVENT_SIZE) {
        touch_panel_data_buffer.touch_buffer_windex = 0;
    }
}

 void touch_panel_multiple_touch_data_push(bsp_ctp_multiple_event_t *push_event)
{
    uint32_t roomleft = 0;
    uint32_t i;
    uint16_t left_size = 0;
    uint32_t mask;

    ctp_mask_gpt_interrupt();

    if (ctp_context.is_buff_full == true) {
        ctp_debug_printf("[CTP][push point]data buffer is full\r\n");
        ctp_user_callback_handler();
        return;
    }

    ctp_get_buf_roomleft((uint16_t *)(&left_size));

    roomleft = (uint32_t)left_size;

#ifdef CTP_DEBUG_LOG_PUSH_POP_DATA
    for (i = 0; i < push_event->model; i++) {
        ctp_debug_printf("[CTP][push point]i=%d, event=%d, x=%d, y=%d, left=%d\r\n", \
                     i, \
                     push_event->points[i].event,   \
                     push_event->points[i].x,       \
                     push_event->points[i].y,       \
                     roomleft);
    }
#endif



    /*check if buffer is full*/
    if (roomleft <= 1) {
        ctp_context.is_buff_full = true;
        ctp_debug_printf("[CTP][push point]data buffer is not enough\r\n");
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    touch_panel_push_one_event(push_event);
    hal_nvic_restore_interrupt_mask(mask);
    
    ctp_user_callback_handler();

    ctp_unmask_gpt_interrupt();

}

void touch_panel_multipel_touch_retrieve_event(bsp_ctp_multiple_event_t *tp_event, uint16_t *index, bool increase_index)
{
    uint16_t current_index, i;
    uint32_t mask;

    current_index = *index;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    tp_event->model      = touch_panel_data_buffer.event[current_index].model;
    tp_event->padding    = touch_panel_data_buffer.event[current_index].padding;
    tp_event->time_stamp = touch_panel_data_buffer.event[current_index].time_stamp;


    for (i = 0; i < touch_panel_data_buffer.event[current_index].model; i++) {
        tp_event->points[i].event = touch_panel_data_buffer.event[current_index].points[i].event;
        tp_event->points[i].x     = touch_panel_data_buffer.event[current_index].points[i].x;
        tp_event->points[i].y     = touch_panel_data_buffer.event[current_index].points[i].y;
    }
    
    if (increase_index == true) {
        (*index)++;
        if ((*index) >= TOUCH_PANEL_BUFFER_EVENT_SIZE) {
            *index = 0;
        }
    }

    hal_nvic_restore_interrupt_mask(mask);


    if (tp_event->model > TP_SUPPORT_POINTS) {
        ctp_debug_printf("[CTP][retrieve]tp_event->model=%d [Error]\r\n", tp_event->model);
#ifdef CTP_DEBUG_LOG_RETRIEVE_DATA
        for (i = 0; i < tp_event->model; i++) {
            ctp_debug_printf("[CTP][retrieve]event=%d, x=%d, y=%d, i=%d\r\n",
                         tp_event->points[i].event, \
                         tp_event->points[i].x,     \
                         tp_event->points[i].y,     \
                         i);
        }
        assert(0);
#endif
    }


}

bool touch_panel_capacitive_peek_event(bsp_ctp_multiple_event_t *touch_data)
{
    bool     result;
    uint8_t  i;
    uint16_t roomleft = 0;


    assert(touch_data);

    ctp_mask_gpt_interrupt();
    
    if (touch_panel_data_buffer.touch_buffer_rindex != touch_panel_data_buffer.touch_buffer_windex) {
        touch_panel_multipel_touch_retrieve_event(touch_data, &touch_panel_data_buffer.touch_buffer_rindex, false);

        ctp_get_buf_roomleft(&roomleft);

        for (i = 0; i < touch_data->model; i++) {
            ctp_debug_printf("[CTP][peek point]event=%d, x=%d, y=%d, left=%d, i=%d\r\n", \
                         touch_data->points[i].event,   \
                         touch_data->points[i].x,       \
                         touch_data->points[i].y,       \
                         roomleft, \
                         i);
        }
        result = true;
    } else {
        ctp_context.send_touch_ilm = true;
        result = false;
    }

    ctp_unmask_gpt_interrupt();

    return result;
}

bool touch_panel_capacitive_get_event(bsp_ctp_multiple_event_t *touch_data)
{
    bool        result;
    uint16_t    i;
    uint16_t    roomleft;
    
    if (touch_data == NULL) {
        ctp_debug_printf("[CTP][get_event]parameter pointer pased by user is [NULL]\r\n");
        assert(0);
    }

    ctp_mask_gpt_interrupt();

    if (ctp_context.is_buff_full == true) {
        touch_panel_multipel_touch_retrieve_event(touch_data, &touch_panel_data_buffer.touch_buffer_rindex, false);
        for (i = 0; i < touch_data->model; i++) {
            touch_data->points[i].event = CTP_PEN_ABORT;
        }
        ctp_unmask_gpt_interrupt();
        return true;
    }


    if (touch_panel_data_buffer.touch_buffer_rindex != touch_panel_data_buffer.touch_buffer_windex) {
        touch_panel_multipel_touch_retrieve_event(touch_data, &touch_panel_data_buffer.touch_buffer_rindex, true);

#ifdef CTP_DEBUG_LOG_PUSH_POP_DATA
        ctp_get_buf_roomleft(&roomleft);

        for (i = 0; i < touch_data->model; i++) {
            ctp_debug_printf("[CTP][pop point]event=%d, x=%d, y=%d, left=%d, i=%d\r\n",
                         touch_data->points[i].event,   \
                         touch_data->points[i].x,       \
                         touch_data->points[i].y,       \
                         roomleft, \
                         i);
        }
#endif

        result = true;

    } else {
        ctp_context.send_touch_ilm = true;
        result = false;
    }

    ctp_unmask_gpt_interrupt();

    return result;
}


ctp_pen_state_enum_t touch_panel_capacitive_get_data(bsp_ctp_multiple_event_t *tp_event)
{
    bool     result = false;
    uint32_t uscnti1, uscnti2;

    uscnti1 = ctp_get_time_us();
    result = ctp_fun_ptr->ctp_get_data(tp_event);

    tp_event->time_stamp = ctp_get_time_us();

    uscnti2 = ctp_get_time_us();

    if (uscnti2 > uscnti1) {

    } else {

    }

    return result ? CTP_DOWN : CTP_UP;
}

bsp_ctp_event_status_t touch_panel_multiple_touch_state_machine(int16_t x_diff, int16_t y_diff, bsp_ctp_event_status_t pre_event, int16_t count)
{

#ifdef CTP_DEBUG_LOG
    ctp_debug_printf("[CTP][machine]x_diff=%d, y_diff=%d, pre_event=%d, count=%d\r\n", x_diff, y_diff, pre_event, count);
#endif

    if (pre_event > CTP_PEN_ABORT) {
        ctp_debug_printf("[CTP][machine]x_diff=%d, y_diff=%d, pre_event=%d, count=%d\r\n", x_diff, y_diff, pre_event, count);
        assert(0);
    }

    return ctp_state_machine[pre_event](x_diff, y_diff, count);
}

void touch_panel_multiple_touch_event_handler(bsp_ctp_multiple_event_t *tp_event, bsp_ctp_multiple_event_t *last_tp_event)
{
    /* This API may increase model, it depends on the pen up counts.*/
    uint32_t    i = 0, event;
    int16_t     x_diff, y_diff;
    uint32_t    event_need_handle;

    event_need_handle = (last_tp_event->model > tp_event->model) ? last_tp_event->model : tp_event->model;
    /*the last model may include up event.*/
    /*the current model do not include up event*/

#ifdef CTP_DEBUG_LOG
    ctp_debug_printf("[CTP][Event] event handler last_tp_event->model=%d, tp_event->model=%d\r\n", last_tp_event->model, tp_event->model);
#endif

    /*check all points which needed to be handle*/
    for (i = 0; i < event_need_handle; i++) {
        /*not included in this time, must be up.*/
        if (i >= tp_event->model) {
            if ((last_tp_event->points[i].event == CTP_PEN_UP)) {
#ifdef CTP_DEBUG_LOG
                ctp_debug_printf("[CTP][Event]CTP_PEN_UP again\r\n");
#endif

                event_need_handle = i;
                break;
            } else {
                event = CTP_PEN_UP;
                tp_event->points[i].x = last_tp_event->points[i].x;
                tp_event->points[i].y = last_tp_event->points[i].y;

#ifdef CTP_DEBUG_LOG
                ctp_debug_printf("[CTP][Event]CTP_PEN_UP\r\n");
#endif
            }
        } else if ((i >= last_tp_event->model) || (last_tp_event->points[i].event == CTP_PEN_UP)) { /*new down*/
            event = CTP_PEN_DOWN;

#ifdef CTP_DEBUG_LOG
            ctp_debug_printf("[CTP][Event]CTP_PEN_UP\r\n");
#endif

        } else { /* still down*/
            x_diff = tp_event->points[i].x - last_tp_event->points[i].x;
            y_diff = tp_event->points[i].y - last_tp_event->points[i].y;
            event = touch_panel_multiple_touch_state_machine(x_diff, y_diff, last_tp_event->points[i].event, i);

#ifdef CTP_DEBUG_LOG
            ctp_debug_printf("[CTP][Event]CTP_STATE_MACHINE_RETURN %d, i=%d\r\n", event, i);
#endif
        }
        tp_event->points[i].event = (bsp_ctp_event_status_t)event;
    }

    tp_event->model = (uint8_t)event_need_handle;
}


void touch_panel_capacitive_up_handler(void)
{
    uint32_t                    i = 0;
    bsp_ctp_multiple_event_t tp_event;

    /*check if the down point is reasonable*/
    if (ctp_context.wait_next_down == true) {
        return;
    }

    if (ctp_context.skip_unrelease_state == false) {
        //touch_panel_multiple_touch_stop_timer();
        touch_panel_multipel_touch_retrieve_event(&tp_event, &touch_panel_data_buffer.touch_buffer_last_windex, false); //not increase index

        if (tp_event.points[0].event == CTP_PEN_UP) {
            //drv_trace0(TRACE_GROUP_4, CTP_UP_HDR_CTP_PEN_UP_AGAIN);
            tp_event.model = 0;
            return;
        }


        for (i = 0; i < tp_event.model; i++) { //set all exist event as UP
            if ((tp_event.points[i].event == CTP_PEN_UP)) {
                tp_event.model = i; //all other points already up in last event
                break;
            } else { /*non-handwriting*/
                tp_event.points[i].event = CTP_PEN_UP;
            }
        }
        //tp_event.time_stamp = L1I_GetTimeStamp();
        tp_event.time_stamp = ctp_get_time_us();
        touch_panel_multiple_touch_data_push(&tp_event);
    }
    else {
    
        /*TP.skip_unrelease_state==true*/
        ctp_context.skip_unrelease_state = false;
    }
}

bool touch_panel_multiple_touch_check_event_valid(bsp_ctp_multiple_event_t *tp_event)
{
    uint32_t i;

    for (i = 0; i < tp_event->model; i++) {
        if ((tp_event->points[i].event < CTP_PEN_LONGTAP_HOLD)) {
            ctp_context.first_hold_valid = 0;
            return true;
        }
    }
    ctp_context.first_hold_valid++;
    return false;
}

static void  touch_panel_multiple_touch_event_callback(void)
{
    bool                        valid_event;
    bsp_ctp_multiple_event_t    tp_event;
    bsp_ctp_multiple_event_t    last_tp_event;

#ifdef CTP_DEBUG_PERFORMANCE
    uint32_t time[5];
    time[0] = ctp_get_time_us();
#endif
    //ctp_debug_printf("[CTP][timer] enter timer callback\r\n");
    //ctp_debug_printf("[CTP][1]eint mask status = %x\r\n", *(volatile uint32_t*)0xa2030320);
    //ctp_debug_printf("[CTP][1]eint ack  status = %x\r\n", *(volatile uint32_t*)0xa2030300);
    if (ctp_context.stop_timer == false) {
        #ifdef CTP_DEBUG_CODE_FLOW
        ctp_debug_printf("flow:6\r\n");
        #endif
        
        memcpy((uint8_t *)(&tp_event), (uint8_t *)(&g_ctp_event), sizeof(bsp_ctp_multiple_event_t));

        touch_panel_multipel_touch_retrieve_event(&last_tp_event, &touch_panel_data_buffer.touch_buffer_last_windex, false);

        touch_panel_multiple_touch_event_handler(&tp_event, &last_tp_event); //feed PEN_MOVE or PEN_STATICAL

        valid_event = touch_panel_multiple_touch_check_event_valid(&tp_event);

        tp_event.time_stamp = ctp_get_time_us();

        if (ctp_context.first_hold_valid <= 1) {
            #ifdef CTP_DEBUG_CODE_FLOW
            ctp_debug_printf("flow:7\r\n");
            #endif
            touch_panel_multiple_touch_data_push(&tp_event);
        }
        
        if (valid_event == true) {
            touch_panel_set_timer(true, CTP_TIMER_SAMPLE_RATE_HIGH);
        } else {
            if (tp_event.model != 0) { //we need to know the event is up or just invalid event.
                touch_panel_set_timer(true, CTP_TIMER_SAMPLE_RATE_LOW);
            }
        }
#ifdef CTP_DEBUG_PERFORMANCE
        time[1] = ctp_get_time_us();
        ctp_debug_printf("[CTP][time flow]timer callback execute time = %d us\r\n", time[1] - time[0]);

        //ctp_debug_printf("[CTP][2]eint mask status = %x\r\n", *(volatile uint32_t*)0xa2030320);
        //ctp_debug_printf("[CTP][2]eint ack  status = %x\r\n", *(volatile uint32_t*)0xa2030300);

#endif
    }
}

void touch_panel_stop_timer(void)
{
    ctp_context.stop_timer = true;
    xTimerStop(ctp_timer, ctp_xTicksToWait);

}

void touch_panel_start_timer(void)
{
    ctp_context.stop_timer = false;
    xTimerStart(ctp_timer, ctp_xTicksToWait);
}

void touch_panel_set_timer(bool is_from_isr, ctp_timer_sample_rate_t rate)
{   
    uint32_t time_out_ms;
    BaseType_t xHigherPriorityTaskWoken;

    if (ctp_context.is_sample_rate_configured == false){
       return;
    }
    
    if (rate == CTP_TIMER_SAMPLE_RATE_HIGH) {
        if (ctp_context.timer_sample_rate.high_sample_period != 0) {
            time_out_ms = ctp_context.timer_sample_rate.high_sample_period;
        } else {
            time_out_ms = 20;       /*default set to 20 ms*/
        }
    } else if (rate == CTP_TIMER_SAMPLE_RATE_LOW) {
        if (ctp_context.timer_sample_rate.low_sample_period != 0) {
            time_out_ms = ctp_context.timer_sample_rate.low_sample_period;
        } else {
            time_out_ms = 20;       /*default set to 20 ms*/
        }

    } else {
        time_out_ms = 20;          /*default set to 20 ms*/
    }

    if (is_from_isr == true) {
        if( xTimerChangePeriodFromISR(ctp_timer, time_out_ms/portTICK_PERIOD_MS , &xHigherPriorityTaskWoken ) != pdPASS )
        {
            ctp_debug_printf("[CTP]Change timer time in isr fail:%d\r\n",time_out_ms);
        }
    }
    else {
        if( xTimerChangePeriod(ctp_timer, time_out_ms/portTICK_PERIOD_MS ,  ctp_xTicksToWait) != pdPASS )
        {
            ctp_debug_printf("[CTP]Change timer time in task fail:%d\r\n",time_out_ms);
        }

    }
}

uint32_t touch_panel_get_remain_queue_size(void)
{
    
    return (uint32_t)uxQueueMessagesWaitingFromISR(ctp_queue_handle);
}

void touch_panel_timer_callback(TimerHandle_t xTimer)
{
#ifdef CTP_DEBUG_PERFORMANCE
    static uint32_t time[2];

    time[1] = ctp_get_time_us();

    ctp_debug_printf("[CTP][time flow]timer callback interval time = %d us\r\n", time[1] - time[0]);
    time[0] = time[1];
#endif

    #ifdef CTP_DEBUG_CODE_FLOW
    ctp_debug_printf("stop timer=%d\r\n",ctp_context.stop_timer);
    #endif
    
    if (ctp_context.stop_timer == false) {
       xTimerStart(xTimer, ctp_xTicksToWait);
    }
    else {
       ctp_unmask_eint_interrupt();
       return;
    }

    /*send message to get point poit data and process if there new finger*/
    touch_panel_send_message(CTP_GET_PEN_DATA);

    /*send message to process the longtap and repeat event*/
    touch_panel_send_message(CTP_CALL_TIMER_CALBACK);

}


void touch_panel_capacitive_down_handler(void)
{
    uint32_t i = 0;

    bsp_ctp_multiple_event_t tp_event;

    memcpy((uint8_t *)(&tp_event), (uint8_t *)(&g_ctp_event), sizeof(bsp_ctp_multiple_event_t));

    /*CTP_g_event may change during HISR prempt, if model become 0, ignore the touch*/
    if (tp_event.model == 0) {
        ctp_debug_printf("[CTP][Down]tp_event.model = %d, ignore\r\n", tp_event.model);
        return;
    }

    ctp_context.skip_unrelease_state = false;
    ctp_context.first_hold_valid = 0;

    for (i = 0; i < tp_event.model; i++) {
        {
            //touch_panel_capacitive_feed_event(CTP_PEN_DOWN, &data, i);
            tp_event.points[i].event = CTP_PEN_DOWN;
            //CTP_para.Report_interval = TP.low_sample_period*9; //GPT tick: 10ms per tick
        }
    }

    touch_panel_multiple_touch_data_push(&tp_event);
    //touch_panel_multiple_touch_set_timer(CTP_TIMER_SAMPLE_RATE_HIGH);
}



static double ctp_tunning_caculate_scope(int32_t p1, int32_t p2, int32_t p3, int32_t p4)
{
    return ((double)(p2 - p1) / (double)(p4 - p3));
}

static double ctp_tunning_caculate_offset(int32_t p1, int32_t p2, double scope)
{
    return ((double)p1 - scope * ((double)(p2)));

}
void touch_panel_tunning(touch_panel_data_struct_t *tp_data, ctp_calibration_t *calibration)
{
    calibration->x_scope = ctp_tunning_caculate_scope(tp_data->x_coord_end, \
                           tp_data->x_coord_start, \
                           tp_data->verndor_x_coord_end, \
                           tp_data->verndor_x_coord_start);

    calibration->x_offset = ctp_tunning_caculate_offset(tp_data->x_coord_start, \
                            tp_data->verndor_x_coord_start, \
                            calibration->x_scope);

    calibration->y_scope = ctp_tunning_caculate_scope(tp_data->y_coord_end, \
                           tp_data->y_coord_start, \
                           tp_data->verndor_y_coord_end, \
                           tp_data->verndor_y_coord_start);

    calibration->y_offset = ctp_tunning_caculate_offset(tp_data->y_coord_start, \
                            tp_data->verndor_y_coord_start, \
                            calibration->y_scope);

}


void touch_custom_parameter_init(void)
{

    touch_panel_tunning(&ctp_context, &ctp_calibration);

    if (ctp_context.pen_longtap_time == 0) {
        ctp_context.pen_longtap_time    = 800;  /*set longtap time default 800ms*/
    }

    if (ctp_context.pen_repeat_time == 0) {
        ctp_context.pen_repeat_time     = 500;  /*set repeat  time default 500ms*/
    }
}

void touch_panel_reset_data(void)
{
    memset(&ctp_context,             0, sizeof(touch_panel_data_struct_t));
    memset(&g_ctp_event,             0, sizeof(bsp_ctp_multiple_event_t));
    memset(&ctp_config_info,         0, sizeof(ctp_config_t));
    memset(&touch_panel_data_buffer, 0, sizeof(touch_panel_buffer_data_t));
    memset(&ctp_calibration,         0, sizeof(ctp_calibration_t));
}
bool touch_custom_init(void)
{
    ctp_delay_ms(3);

    /*ite7258 init*/
#ifdef CTP_IT7258_SUPPORTED
    touch_panel_reset_data();
    touch_panel_ite_custom_data_init();
    touch_custom_parameter_init();
    ctp_mask_eint_interrupt();
    ctp_fun_ptr = ctp_ite_7258_get_custom_function();
    if (ctp_fun_ptr->ctp_init() == true) {;
        return true;
    }
#endif

    /*goodix gt9137*/
#ifdef CTP_GT9137_SUPPORTED
    touch_panel_reset_data();
    touch_panel_goodix_custom_data_init();
    touch_custom_parameter_init();
    ctp_mask_eint_interrupt();
    ctp_fun_ptr = ctp_goodix_gt9xx_get_custom_function();
    if (ctp_fun_ptr->ctp_init() == true) {
        return true;
    }
#endif

    return false;
}

void touch_panel_eint_handler(hal_eint_number_t eint_number)
{
    #ifdef CTP_DEBUG_CODE_FLOW
    static uint8_t eint_count = 0;
    #endif

    #ifdef CTP_DEBUG_PERFORMANCE
    static uint32_t time[2];
    time[1] = ctp_get_time_us();
    ctp_debug_time[0] = time[1];
    ctp_debug_printf("[CTP][time flow]eint last interval time = %d us\r\n", time[1] - time[0]);
    time[0] = time[1];
    #endif
    
    BaseType_t xHigherPriorityTaskWoken;

    //touch_panel_multiple_touch_set_timer(CTP_TIMER_SAMPLE_RATE_HIGH);

    xHigherPriorityTaskWoken = pdFALSE;

    if (ctp_context.stop_timer == true) {
        touch_panel_set_timer(true, CTP_TIMER_SAMPLE_RATE_HIGH);
        if (pdFAIL == xTimerStartFromISR(ctp_timer, &xHigherPriorityTaskWoken)) {
            printf("ctp timer start from eint fail\r\n");
            assert(0);
        } 
        ctp_context.stop_timer = false;
    }

    #ifdef CTP_DEBUG_CODE_FLOW
    ctp_debug_printf("eint_count=%d\r\n",eint_count);
    #endif
    
    /*for build warning*/
    eint_number = eint_number;
    touch_panel_send_message(CTP_GET_PEN_DATA);

}

void ctp_process_point(void)
{
    ctp_pen_state_enum_t    state            = CTP_UP;
    bool                    is_get_new_event = false;
    bsp_ctp_multiple_event_t current_tp_event, last_tp_event, tp_event;

#ifdef CTP_DEBUG_PERFORMANCE
    uint32_t time[5];
    time[0] = ctp_get_time_us();
#endif

    /*wait to get i2c*/
    while (ctp_hw_i2c_init(&ctp_config_info.ctp_i2c) == false) {
        /* every 10ms to get i2c, ctp must get i2c avoid lost point*/
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    
    /* if data is invalid, skip.*/
    if (ctp_fun_ptr->ctp_data_valid() == true) {
        
        /* get current points*/
        state  = touch_panel_capacitive_get_data(&current_tp_event);

        /* get last event from buffer*/
        touch_panel_multipel_touch_retrieve_event(&last_tp_event, &touch_panel_data_buffer.touch_buffer_last_windex, false);

        if ((last_tp_event.model <= g_ctp_event.model) || (g_ctp_event.model >= current_tp_event.model) || (g_ctp_event.model == 0)) {

            memcpy((uint8_t *)(&g_ctp_event), (uint8_t *)(&current_tp_event), sizeof(bsp_ctp_multiple_event_t));

            is_get_new_event = true;

            if ((g_ctp_event.model != last_tp_event.model) && (last_tp_event.model != 0) && (current_tp_event.model != 0) && (ctp_context.state == CTP_DOWN)) {
                /* this flow only process multiple touch scenery, every another point down or up will enter this flow */
                /* if point number changes, need to handle it immediately. */
                /* Do not handle first pen down and last pen up. up and down handler will handle that.*/
                ctp_calibrate_coordinate(&g_ctp_event);
                memcpy((uint8_t *)(&tp_event), (uint8_t *)(&g_ctp_event), sizeof(bsp_ctp_multiple_event_t));
                touch_panel_multiple_touch_event_handler(&tp_event, &last_tp_event); //feed PEN_MOVE or PEN_STATICAL
                touch_panel_multiple_touch_data_push(&tp_event);

                /*if new down or up, need to clear this variable to ensure timer statemachine work smoothly*/
                ctp_context.first_hold_valid = 0;

                ctp_hw_i2c_deinit();

#ifdef CTP_DEBUG_PERFORMANCE
                time[1] = ctp_get_time_us();
                ctp_debug_time[1] = time[1];
                ctp_debug_printf("[CTP][time flow]a new point down or up flow execute time = %d us\r\n", time[1] - time[0]);
                ctp_debug_printf("[CTP][time flow]from eint to pen down or up exexute time = %d us\r\n", ctp_debug_time[1] - ctp_debug_time[0]);
#endif
                #ifdef CTP_DEBUG_CODE_FLOW
                ctp_debug_printf("@1\r\n");
                #endif

                return;
            }
        }

        #ifdef CTP_DEBUG_CODE_FLOW
        ctp_debug_printf("@5\r\n");
        #endif
        
        if (state == CTP_DOWN) {    /*low*/
            if (is_get_new_event) { /* no need to change coordinate again, if not get new event */
                ctp_calibrate_coordinate(&g_ctp_event);
            }

            if (ctp_context.state == CTP_UP) {
                ctp_context.state = state;
                ctp_debug_printf("[CTP][Down] enter down handler\r\n");
                touch_panel_capacitive_down_handler();
            } else { /* not trigger TP task if down->down */
                #ifdef CTP_DEBUG_CODE_FLOW
                ctp_debug_printf("@4\r\n");
                #endif
            }


        } else { /*high*/
            if (ctp_context.state == CTP_DOWN) {
                //touch_panel_multiple_touch_stop_timer(); 
                ctp_context.state = state;
                ctp_debug_printf("[CTP]enter up handler\r\n");
                touch_panel_capacitive_up_handler();
            } else {
                #ifdef CTP_DEBUG_CODE_FLOW
                ctp_debug_printf("@2\r\n");
                #endif
                ctp_debug_printf("[CTP]up_to_up %d\r\n", ctp_context.state);
            }

            ctp_context.stop_timer = true;
            
        }
        

    }
    else {
        if (ctp_context.state == CTP_UP) {
            ctp_context.stop_timer = true;
            #ifdef CTP_DEBUG_CODE_FLOW
            ctp_debug_printf("@9\r\n");
            #endif
        }
        #ifdef CTP_DEBUG_CODE_FLOW
        ctp_debug_printf("@3\r\n");
        #endif
    }

    ctp_hw_i2c_deinit();

    #ifdef CTP_DEBUG_PERFORMANCE
    time[1] = ctp_get_time_us();
    ctp_debug_time[1] = time[1];
    ctp_debug_printf("[CTP][time flow]normal execute time = %d us\r\n", time[1] - time[0]);
    ctp_debug_printf("[CTP][time flow]from eint to normal execute time = %d us\r\n", ctp_debug_time[1] - ctp_debug_time[0]);
    #endif
    
    #ifdef CTP_DEBUG_CODE_FLOW
    ctp_debug_printf("@10\r\n");
    #endif

    //ctp_debug_printf("[CTP][3]eint mask status = %x\r\n", *(volatile uint32_t*)0xa2030320);
    //ctp_debug_printf("[CTP][3]eint ack  status = %x\r\n", *(volatile uint32_t*)0xa2030300);

}

void ctp_task_main(void *pvParameters)
{
    ctp_msg_t msgs;
    static uint32_t stack_max = 0;
    uint32_t stack_cur = 0;
    
    while (1) {
        if (xQueueReceive(ctp_queue_handle, &msgs, portMAX_DELAY)) {
            switch (msgs.msg_id) {
                case CTP_GET_PEN_DATA: {                      
                    //ctp_debug_printf("[CTP]start processing points\r\n");
                    ctp_process_point();
                    //ctp_debug_printf("[CTP]end processing points\r\n");
                }
                break;

                case CTP_CALL_USER_CALLBACK: {
                    ctp_context.user_callback.callback(ctp_context.user_callback.pdata);
                }
                break;

                case CTP_CALL_TIMER_CALBACK: {
                    touch_panel_multiple_touch_event_callback();
                }

                default: {

                } break;

            }

            //vTaskDelay(xTicksToWait);
            stack_cur = (BSP_CTP_TASK_STACKSIZE / sizeof(StackType_t) - uxTaskGetStackHighWaterMark(ctp_task_handle)) * sizeof(portSTACK_TYPE);
            if (stack_cur > stack_max) {
                stack_max = stack_cur;
                ctp_debug_printf("CTP task max-usage:%d \r\n", stack_max);
            }


            //ctp_debug_printf("[CTP][4]eint mask status = %x\r\n", *(volatile uint32_t*)0xa2030320);
            //ctp_debug_printf("[CTP][4]eint ack  status = %x\r\n", *(volatile uint32_t*)0xa2030300);
        }
    }

} /* CalTaskMain() */

void ctp_task_init(void)
{
    ctp_queue_handle = xQueueCreate(BSP_CTP_QUEUE_LENGTH, sizeof(ctp_msg_t));
    xTaskCreate(ctp_task_main, BSP_CTP_TASK_NAME, BSP_CTP_TASK_STACKSIZE / sizeof(StackType_t), NULL, BSP_CTP_TASK_PRIO, &ctp_task_handle);
    g_ctp_i2c_dma_semaphore = xSemaphoreCreateBinary();
}

bool touch_panel_init(void)
{
    if (touch_custom_init() == false) {
        return false;
    }

    ctp_context.is_buff_full    = false;
    ctp_context.wait_next_down  = false;
    ctp_context.send_touch_ilm  = true;
    ctp_context.stop_timer      = true;
    
    ctp_timer = xTimerCreate(NULL, \
                             20/portTICK_PERIOD_MS , \
                             pdFALSE, NULL, \
                             touch_panel_timer_callback);

    /* set eint trigger mode and debounce time */

    hal_eint_init(ctp_config_info.ctp_eint.eint_num, &ctp_config_info.ctp_eint.init_config);
    hal_eint_register_callback(ctp_config_info.ctp_eint.eint_num, (hal_eint_callback_t)touch_panel_eint_handler, NULL);

    ctp_task_init();

    return true;
}

void touch_panel_enable(void)
{
    hal_eint_unmask(ctp_config_info.ctp_eint.eint_num);
    hal_nvic_enable_irq(EINT_IRQn);
}

void touch_panel_disable(void)
{
    hal_eint_mask(ctp_config_info.ctp_eint.eint_num);

}


void touch_panel_capacitive_power_on(bool on)
{
    ctp_fun_ptr->ctp_power_on(on);
}

bool touch_panel_capacitive_power_state(ctp_device_mode_enum_t mode)
{
    return ctp_fun_ptr->ctp_set_device_mode(mode);
}

uint32_t touch_panel_capacitive_command(uint32_t cmd, void *p1, void *p2)
{
    return ctp_fun_ptr->ctp_command((ctp_control_command_enum_t)cmd, p1, p2);
}

bool touch_panel_capacitive_set_device(ctp_device_mode_enum_t mode)
{
    return ctp_fun_ptr->ctp_set_device_mode(mode);
}

#endif /*MTK_CTP_ENABLE*/


