/********************************************************************[libaroma]*
 * Copyright (C) 2011-2015 Ahmad Amarullah (http://amarullz.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *______________________________________________________________________________
 *
 * Filename    : hid_driver.c
 * Description : QNX NTO / hid driver - read from hidevent pipe
 *
 * + This is part of libaroma, an embedded ui toolkit.
 * + 21/01/15 - Author(s): Ahmad Amarullah
 *
 */
#ifndef __libaroma_rtos_hid_driver_c__
#define __libaroma_rtos_hid_driver_c__
#include <aroma_internal.h>
#include "stdbool.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "bsp_ctp.h"
#include "ctp.h"
#include "message_map_queue.h"

/* hiddi driver */

/* forward function */
byte rtoshid_getinput(LIBAROMA_HIDP me, LIBAROMA_HID_EVENTP dest_ev);
void rtoshid_release(LIBAROMA_HIDP me);

typedef struct
{
    QueueHandle_t event_queue;
    QueueHandle_t touch_queue;
} RTOSHIDRV_INTERNAL, *RTOSHIDRV_INTERNALP;

static RTOSHIDRV_INTERNALP hid_mi;
#ifdef MTK_CTP_ENABLE
/*****************************************************************************
 * FUNCTION
 *  demo_ui_ctp_callback_func
 * DESCRIPTION
 *  CTP callback function
 * PARAMETERS
 *  param       [in]
 * RETURNS
 *  void
 *****************************************************************************/
static void libaroma_ui_ctp_callback_func(void *param)
{
    ALOGD("----------touch event up ----------");

    BaseType_t xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    LIBAROMA_MSG message;
    xQueueSendFromISR(hid_mi->event_queue, &message, &xHigherPriorityTaskWoken);
}

#endif

void rtoshid_push_event()
{
}

void rtoshid_send_event(byte id, int x, int y)
{
}

static void *_libaroma_msgqueue_tp_thread(void *cookie)
{

    ALOGI("=====================>receiver touch event start<================");
    LIBAROMA_MSG queue_item;

    while (1)
    {

        if (xQueueReceive(hid_mi->event_queue, &queue_item, portMAX_DELAY))
        {
            ALOGI("=====================>receiver touch event<================");
            bsp_ctp_multiple_event_t touch_event;
            bsp_ctp_status_t ret;
            bool update_flag = false;
            while (1)
            {
                LIBAROMA_HID_EVENT event;
                memset(&event, 0, sizeof(LIBAROMA_HID_EVENT));
                event.type = LIBAROMA_HID_EV_TYPE_TOUCH;
                ret = bsp_ctp_get_event_data(&touch_event);
                if (ret == BSP_CTP_OK)
                {

                    /*if have get up or down, do not update point, waiting to all point to be get out*/
                    // if (update_flag == false)
                    // {
                    event.x = (int32_t)touch_event.points[0].x;
                    event.y = (int32_t)touch_event.points[0].y;
                    // }

                    /*if there is up or abort, release event*/
                    if (touch_event.points[0].event == CTP_PEN_DOWN)
                    {
                        /*if there is down, press event*/
                        ALOGD("==================>LIBAROMA_HID_EV_STATE_DOWN<==================");
                        event.state = LIBAROMA_HID_EV_STATE_DOWN;
                        update_flag = true;
                        ALOGD("==================>LIBAROMA_HID_EV_STATE_DOWN  post start <==================");
                        libaroma_msg_post_hid(LIBAROMA_MSG_TOUCH, event.state, event.key, event.x, event.y);
                        ALOGD("==================>LIBAROMA_HID_EV_STATE_DOWN  post end <==================");
                        // xQueueSend(ui_task_cntx.touch_queue, &event, portMAX_DELAY);
                        //break;
                    }
                    else if ((touch_event.points[0].event == CTP_PEN_UP))
                    {
                        ALOGD("==================>LIBAROMA_HID_EV_STATE_UP<==================");
                        event.state = LIBAROMA_HID_EV_STATE_UP;
                        update_flag = true;
                        ALOGD("==================>LIBAROMA_HID_EV_STATE_UP  post start <==================");
                        libaroma_msg_post_hid(LIBAROMA_MSG_TOUCH, event.state, event.key, event.x, event.y);
                        ALOGD("==================>LIBAROMA_HID_EV_STATE_UP  post end <==================");
                        // xQueueSend(ui_task_cntx.touch_queue, &event, portMAX_DELAY);
                        //break;
                    }
                    else if (touch_event.points[0].event == CTP_PEN_ABORT)
                    {
                        bsp_ctp_flush_buffer();
                        //break;
                    }
                    else if ((touch_event.points[0].event == CTP_PEN_MOVE))
                    {
                        ALOGD("==================>LIBAROMA_HID_EV_STATE_MOVE<==================")
                        event.state = LIBAROMA_HID_EV_STATE_MOVE;
                        update_flag = true;
                        ALOGD("==================>LIBAROMA_HID_EV_STATE_MOVE  post start <==================")
                        libaroma_msg_post_hid(LIBAROMA_MSG_TOUCH, event.state, event.key, event.x, event.y);
                        ALOGD("==================>LIBAROMA_HID_EV_STATE_MOVE  post end <==================")
                        // xQueueSend(ui_task_cntx.touch_queue, &event,portMAX_DELAY);
                        //break
                    }
                    else
                    {
                        /*others, while loop to get point until the all points are get out. */
                    }
                }
                else
                { /* no point in buffer, keep last point data and unchange state*/
                    break;
                }
            }
        }
    }

    return NULL;
}
static LIBAROMA_THREAD tp_thread;
/* init */
byte rtoshid_init(LIBAROMA_HIDP me)
{

    RTOSHIDRV_INTERNALP mi = (RTOSHIDRV_INTERNALP)calloc(sizeof(RTOSHIDRV_INTERNAL), 1);
    hid_mi = mi;
    me->internal = (voidp)mi;
    mi->touch_queue = xQueueCreate(20, sizeof(LIBAROMA_HID_EVENT));
#ifdef MTK_CTP_ENABLE
    bsp_ctp_status_t ret;
    mi->event_queue = xQueueCreate(1, sizeof(LIBAROMA_MSG));

    ret = bsp_ctp_init();
    ALOGI("ctp init, ret:%d", ret);
    /*set to 20ms polling hardware*/

    bsp_ctp_gpt_sample_period_t config;
    config.high_sample_period = 10;
    config.low_sample_period = 10;
    config.is_used_hardware_polling = true;
    bsp_ctp_set_sample_period(&config);
    ret = bsp_ctp_register_callback(libaroma_ui_ctp_callback_func, NULL);
    ALOGI("ctp register callback, ret:%d", ret);
    libaroma_thread_create_rtos(
        &tp_thread, _libaroma_msgqueue_tp_thread, (voidp)mi, "tp_thread");
#endif
    me->release = &rtoshid_release;
    me->getinput = &rtoshid_getinput;

    ALOGI("rtos_hid_init: successed");
    return 1;
}

/* driver init */
byte libaroma_hid_driver_init(LIBAROMA_HIDP me)
{
    return rtoshid_init(me);
}

/* release */
void rtoshid_release(LIBAROMA_HIDP me)
{
    if (me == NULL)
    {
        return;
    }

    bsp_ctp_deinit();
    free(me->internal);
}

/* get input */
byte rtoshid_getinput(LIBAROMA_HIDP me, LIBAROMA_HID_EVENTP dest_ev)
{
    // ALOGI("=====================>rtoshid_getinput<================");
    LIBAROMA_HID_EVENT queue_item;
    byte ret = LIBAROMA_HID_EV_RET_TOUCH;
    // if (xQueueReceive(ui_task_cntx.touch_queue, &queue_item, portMAX_DELAY))
    // {
    //     // ALOGI("=====================>receiver touch event<===================");
    //     memcpy(dest_ev, &queue_item, sizeof(LIBAROMA_HID_EVENT));
    //     return ret;
    // }
    return LIBAROMA_HID_EV_RET_EXIT;
}

#endif /* __libaroma_rtos_hid_driver_c__ */
