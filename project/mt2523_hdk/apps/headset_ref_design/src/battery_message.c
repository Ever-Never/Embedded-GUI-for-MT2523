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

#ifdef MTK_SMART_BATTERY_ENABLE
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "battery_message.h"

/* battery management includes */
#include "battery_management.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static battery_message_handle_context_t battery_context = {{{0}}};
/* Private functions ---------------------------------------------------------*/

/**
* @brief Get the free queue number.
* @return queue number.
*/
static uint32_t battery_message_get_free_queue_number(void)
{
    uint32_t i;

    for (i = 0; i < APP_TASK_MSG_QUEUE_NUM; i++) {
        if (battery_context.msg_queue[i].is_used != true) {
            return i;
        }
    }

    return APP_TASK_MSG_QUEUE_NUM;
}

/**
* @brief      This function is use to send battery message.
* @param[in]  handle is an unsigned integer for identify.
* @param[in]  event is an envent ID.
* @param[in]  data is a pointer to the data to be used by this function..
* @return     None.
*/
static void battery_message_send(uint32_t handle, battery_management_event_t event, const void *data)
{
    battery_message_context_t message;
    uint32_t msg_queue_num = 0;

    message.event = event;
    message.data  = data;

    if ((handle & BATTERY_MSG_SW_MAGIC) != BATTERY_MSG_SW_MAGIC) {
        LOG_E(bmt_demo, "battery_message_send fail!! handle issue, event = 0x%X\n\r", event);
        return;
    }

    msg_queue_num = handle & BATTERY_MSG_SW_HANDLE_MASK;

    if (battery_context.msg_queue[msg_queue_num].is_used == false || battery_context.msg_queue[msg_queue_num].queue == NULL) {
        LOG_E(bmt_demo, "battery_message_send fail!! queue issue, event = 0x%X\n\r", event);
        return;
    }

    if (xQueueSend(battery_context.msg_queue[msg_queue_num].queue, &message, 0 / portTICK_RATE_MS) != pdPASS) {
        LOG_E(bmt_demo, "battery_message_send fail!! event = 0x%X\n\r", event);
    }
}

/**
* @brief      This function is use to receive battery message.
* @param[in]  handle is an unsigned integer for identify.
* @param[out] message is receive message.
* @return     true, if the operation is successful.\n
*             false, if cannot receive message.
*/
bool battery_message_receive(uint32_t handle, battery_message_context_t *message)
{
    battery_message_context_t temp_message;
    bool result = false;
    uint32_t msg_queue_num = 0;

    if ((handle & BATTERY_MSG_SW_MAGIC) != BATTERY_MSG_SW_MAGIC) {
        assert(0);
        return false;
    }

    msg_queue_num = handle & BATTERY_MSG_SW_HANDLE_MASK;


    if (battery_context.msg_queue[msg_queue_num].queue) {
        if (xQueueReceive(battery_context.msg_queue[msg_queue_num].queue, &temp_message, portMAX_DELAY)) {
            *message = temp_message;
            result = true;
        }
    }
    return result;
}

/**
* @brief This a callback function to receive the battery management event.
* @param[in] event is notification sent by battery management.
* @param[in] data is a pointer to the data to be used by this callback function.
* @return    None.
*/
static void battery_message_callback_handler(battery_management_event_t event, const void *data)
{
    uint32_t i = 0;
    uint32_t handle = 0;

    for (i = 0; i < APP_TASK_MSG_QUEUE_NUM ; i ++) {
        if (battery_context.msg_queue[i].is_used == true) {
            LOG_I(bmt_demo, "callback handle = %d, event = 0x%X", (int)i, (int) event);
            switch (event) {
                case BATTERY_MANAGEMENT_EVENT_BATTERY_UPDATE: {
                    handle = i | BATTERY_MSG_SW_MAGIC;
                    battery_message_send(handle, event, data);
                }
                break;
                default: {

                }
                break;
            }
        }
    }
}

/**
* @brief      This function allocates battery message handle.
* @param[out] handle is an unsigned integer for identify.
* @return     true, if the operation is successful.\n
*             false, if cannot be allocated.
*/
bool battery_message_allocate(uint32_t *handle)
{
    battery_management_status_t status;
    uint32_t msg_queue_num = 0;

    if (battery_context.used_msg_queue_count >= APP_TASK_MSG_QUEUE_NUM) {
        return false;
    }

    msg_queue_num = battery_message_get_free_queue_number();

    *handle = msg_queue_num | BATTERY_MSG_SW_MAGIC;

    battery_context.msg_queue[msg_queue_num].queue = xQueueCreate(APP_TASK_QUEUE_SIZE , sizeof(battery_message_context_t));
    battery_context.msg_queue[msg_queue_num].is_used = true;
    battery_context.used_msg_queue_count++;

    if (battery_context.register_callback_flag != true) {
        battery_context.register_callback_flag = true;
        /* Register battery management callback function */
        status = battery_management_register_callback(battery_message_callback_handler);
        if (status != BATTERY_MANAGEMENT_STATUS_OK) {
            LOG_E(bmt_demo, "battery_management_register_callback fail!! status = %d\n\r", (int)status);
            battery_context.register_callback_flag = false;
            return false;
        }

    }
    return true;
}
#endif //MTK_SMART_BATTERY_ENABLE

