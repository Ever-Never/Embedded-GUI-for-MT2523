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

#include "stdint.h"
#include "stdbool.h"
#include "main_screen.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "task_def.h"
#include "syslog.h"

#define UI_TASK_QUEUE_SIZE 20
#define UI_TASK_NAME "UI_DEMO"
#define UI_TASK_STACK_SIZE 4800
#define UI_TASK_PRIORITY 3
#include <aroma.h>
#include "message_map_queue.h"

typedef struct ui_task_message_struct
{
    int message_id;
    int param1;
    void *param2;
} ui_task_message_struct_t;

static struct
{
    QueueHandle_t event_queue;
    void *user_data;
} ui_task_cntx;

log_create_module(GRAPHIC_TAG, PRINT_LEVEL_INFO);

/*****************************************************************************
 * FUNCTION
 *  ui_task_msg_handler
 * DESCRIPTION
 *  Process message in queue
 * PARAMETERS
 *  message         [in]
 * RETURNS
 *  void
 *****************************************************************************/
static void ui_task_msg_handler(ui_task_message_struct_t *message)
{
    if (!message)
    {
        return;
    }
    GRAPHICLOG("ui_task_msg_handler, message_id:%d", message->message_id);
    switch (message->message_id)
    {
    default:
        common_event_handler((message_id_enum)message->message_id, (int32_t)message->param1, (void *)message->param2);
        break;
    }
}

/*****************************************************************************
 * FUNCTION
 *  ui_task_main
 * DESCRIPTION
 *  Initialize the UI task.
 * PARAMETERS
 *  arg        [in]
 * RETURNS
 *  void
 *****************************************************************************/
void ui_task_main(void *arg)
{
    ui_task_message_struct_t queue_item;

    
    GRAPHICLOG("ui_task_main");
    //clock_test();
    sport_test() ;
    testFb() ;
    LIBAROMA_MSG msg_data;
    byte need_sync = 0;
    int msg = -1;
    memset(&msg_data, 0, sizeof(LIBAROMA_MSG));
    QueueHandle_t handle = xQueueCreate(3, sizeof(MESSAGE_T));
    message_queue_map(UI_SET_UPDATE_FREQUECY, handle);
    message_queue_map(UI_UPDATE, handle);
    message_queue_map(UI_TP_UPLOAD, handle);
    message_queue_map(UI_KEY_UPLOAD, handle);
    while (1)
    {
        if (-1 != MessageRecv(handle, &msg, &msg_data))
        {

            printf("=============== receive msg id ====================== %d", msg);
            switch (msg)
            {
            case UI_SET_UPDATE_FREQUECY:
            {
                printf("===============set ui update delay frequency ======================");
                // delay_ms = (int)msg_data;
                break;
            }
            case UI_KEY_UPLOAD:
            case UI_TP_UPLOAD:
            {
                // LIBAROMA_MSGP libaroma_msgp = (LIBAROMA_MSGP) msg_data  ;

                break;
            }
            }
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  demo_app_start
 * DESCRIPTION
 *  Create a UI task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
TaskHandle_t demo_app_start()
{
    TaskHandle_t task_handler;
    xTaskCreate((TaskFunction_t)ui_task_main, UI_TASK_NAME, UI_TASK_STACK_SIZE / ((uint32_t)sizeof(StackType_t)), NULL, 3, &task_handler);
    GRAPHICLOG("demo_app_start, task_handler:%d", task_handler);
    return task_handler;
}
