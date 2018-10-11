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

// For Register AT command handler
//#include "atci.h"
// System head file

#ifdef MTK_GNSS_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "at_command.h"
#include "gnss_api.h"
#include "gnss_log.h"
#include "hal_eint.h"
#include "task_def.h"
#define GNSS_SLEEP_HANDLER

#ifdef GNSS_SLEEP_HANDLER
#include "hal_sleep_manager.h"
#include "timers.h"
#include "gnss_driver.h"
#endif

int32_t gnss_state = 0;
uint8_t pmtk_command[100]; 

//GNSS Data 
#ifndef GNSS_DEMO_TASK_NAME
    #define GNSS_DEMO_TASK_NAME "gnss_t"
#endif
#ifndef GNSS_DEMO_TASK_STACK_SIZE
    #define GNSS_DEMO_TASK_STACK_SIZE 4000
#endif
#ifndef GNSS_DEMO_TASK_PRIO
    #define GNSS_DEMO_TASK_PRIO TASK_PRIORITY_HIGH
#endif
#define GNSS_AT_COMMAND_TASK_NAME "gnss_at_t"
#define GNSS_AT_COMMAND_TASK_STACK_SIZE 2500 //should be fine tune
#define GNSS_AT_COMMAND_TASK_PRIORITY 3 //should be arrange by scenario
#define GNSS_AT_COMMAND_QUEUE_SIZE 10
#define GNSS_AT_COMMAND_RECIEVE_TEMP_BUF 256

TaskHandle_t gnss_at_command_task_handler;
QueueHandle_t gnss_at_command_task_queue;

typedef enum gnss_at_command_message_id {
    GNSS_AT_COMMAND_ENUM_POWER_ON_REQ,
    GNSS_AT_COMMAND_ENUM_POWER_OFF_REQ,
    GNSS_AT_COMMAND_ENUM_READY_TO_READ,
    GNSS_AT_COMMAND_ENUM_SEND_COMMAND,
    GNSS_AT_COMMAND_ENUM_HOST_WAKEUP
} gnss_at_command_message_id_enum;

typedef struct gnss_at_command_message_struct {
    int message_id;
    int param1;
    void* param2;
} gnss_at_command_message_struct_t;

#ifdef GNSS_SLEEP_HANDLER
static TimerHandle_t sleep_timer = NULL;
uint8_t sleep_handler;
#endif

// AT command handler

static void gnss_at_command_driver_callback_func(gnss_notification_type_t type, void *param)
{
    gnss_at_command_message_struct_t gnss_message;
    BaseType_t xHigherPriorityTaskWoken;
    switch (type) {
        case GNSS_NOTIFICATION_TYPE_READ:
            gnss_message.message_id = GNSS_AT_COMMAND_ENUM_READY_TO_READ;
            break;
#ifdef GNSS_SLEEP_HANDLER
        case GNSS_NOTIFICATION_TYPE_HOST_WAKEUP:
            gnss_message.message_id = GNSS_AT_COMMAND_ENUM_HOST_WAKEUP;
            break;
#endif                
        default:
            return;

    }
    xQueueSendFromISR(gnss_at_command_task_queue, &gnss_message, &xHigherPriorityTaskWoken);
}

#ifdef GNSS_SLEEP_HANDLER
void gnss_at_command_sleep_timer_handle_func(TimerHandle_t timer_id)
{
    GNSSLOGD("gnss_sleep_timer_handle_func\r\n");
    hal_sleep_manager_unlock_sleep(sleep_handler);
    hal_eint_unmask(gnss_eint);
}

void gnss_at_command_sleep_restart_timer()
{
    GNSSLOGD("gnss_sleep_restart_timer\r\n");
    if (xTimerIsTimerActive(sleep_timer) != pdFALSE) {
        xTimerStop(sleep_timer, 0);
        xTimerStart(sleep_timer, 0);
    }
}

void gnss_at_command_wakeup_handle()
{
    GNSSLOGD("gnss_wakeup_handle\r\n");
    hal_sleep_manager_lock_sleep(sleep_handler);
    gnss_send_command((int8_t *) "$PMTK850*3F\r\n", strlen("$PMTK850*3F\r\n"));
    if (xTimerIsTimerActive(sleep_timer) == pdFALSE) {
        xTimerStart(sleep_timer, 0);
    }
}
#endif

void gnss_at_command_task_msg_handler(gnss_at_command_message_struct_t *message)
{

    if (!message) {
        return;
    }
    GNSSLOGD("gnss_task_msg_handler, message id:%d\n", message->message_id);
    switch (message->message_id) {
        case GNSS_AT_COMMAND_ENUM_POWER_ON_REQ:
            gnss_power_on();
            break;
        case GNSS_AT_COMMAND_ENUM_POWER_OFF_REQ:
            gnss_power_off();
            break;
        case GNSS_AT_COMMAND_ENUM_READY_TO_READ:
        {
#ifdef GNSS_SLEEP_HANDLER
            gnss_at_command_sleep_restart_timer();
#endif
            while (1) {
                static int8_t sentence_buf[GNSS_AT_COMMAND_RECIEVE_TEMP_BUF] = {0};
                int32_t sentence_length;
                atci_response_t output = {{0}};

                sentence_length = gnss_read_sentence(sentence_buf, GNSS_AT_COMMAND_RECIEVE_TEMP_BUF);
                if (sentence_length > 0) {
                    memcpy(output.response_buf, sentence_buf, sentence_length);
                    output.response_buf[sentence_length] = 0;
                    output.response_len = strlen((char*) output.response_buf);
                    output.response_flag = 0 | ATCI_RESPONSE_FLAG_URC_FORMAT;
                    atci_send_response(&output);
                } else {
                    return;
                }
            }
        }
        case GNSS_AT_COMMAND_ENUM_SEND_COMMAND:
            gnss_send_command((int8_t*)message->param2, (int32_t)message->param1);
            break;
            
#ifdef GNSS_SLEEP_HANDLER
        case GNSS_AT_COMMAND_ENUM_HOST_WAKEUP:
            gnss_at_command_wakeup_handle();
            break;
#endif
        default:
            break;
    }
}

static void gnss_at_command_task_main(void* param)
{
    gnss_at_command_message_struct_t queue_item;
    GNSSLOGD("gnss_at_command_task_main\n");	

    gnss_at_command_task_queue = xQueueCreate( GNSS_AT_COMMAND_QUEUE_SIZE, sizeof( gnss_at_command_message_struct_t ) );
    gnss_power_on();

    while (1) {
        if (xQueueReceive(gnss_at_command_task_queue, &queue_item, portMAX_DELAY)) {
            gnss_at_command_task_msg_handler(&queue_item);
        }
    }
}

void gnss_at_command_app_open(){

    static bool gnss_is_power_on = false;
    
    GNSSLOGD("gnss_at_command_app_open\n");	

    if (!gnss_is_power_on){
        gnss_init(gnss_at_command_driver_callback_func);
#ifdef GNSS_SLEEP_HANDLER
        sleep_handler = hal_sleep_manager_set_sleep_handle("ATGSH");
        sleep_timer = xTimerCreate( "gnss_sleep_timer",
                        (50/portTICK_PERIOD_MS), 
                        pdFALSE, 
                        NULL, 
                        gnss_at_command_sleep_timer_handle_func);
        xTimerStop(sleep_timer, 0);
        GNSSLOGD("gnss get sleep handle: %d, %d\r\n", sleep_handler, sleep_timer);
#endif
        gnss_is_power_on = true;
    }
    
    xTaskCreate((TaskFunction_t) gnss_at_command_task_main, 
                GNSS_DEMO_TASK_NAME, 
                GNSS_DEMO_TASK_STACK_SIZE/(( uint32_t )sizeof( StackType_t )), 
                NULL, 
                GNSS_DEMO_TASK_PRIO, 
                &gnss_at_command_task_handler);
}

void gnss_at_command_app_close(){
    GNSSLOGD("gnss_at_command_app_close\n");	
    gnss_power_off();
    vTaskDelete(gnss_at_command_task_handler);
}

void gnss_at_command_app_send_command(int8_t* buf, int32_t buf_len){

    gnss_at_command_message_struct_t message;
    message.message_id = GNSS_AT_COMMAND_ENUM_SEND_COMMAND;
    message.param1 = buf_len;
    message.param2 = buf;
    xQueueSend(gnss_at_command_task_queue, &message, 0);
}

extern int32_t gnss_output_data;
atci_status_t gnss_power_control_at_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};

    GNSSLOGD("gnss_power_control_at_handler:%s\n", parse_cmd->string_ptr);

    if (strstr("AT+EGPSC", (char *)parse_cmd->string_ptr))
    {
        GNSSLOGW("gnss_power_control_at_handler:string error!\n");
        return ATCI_STATUS_OK;
    }
    
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
            GNSSLOGD("gnss_power_control_at_handler,mode:%d\n", parse_cmd->mode);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(&output);
            break;
    
        case ATCI_CMD_MODE_EXECUTION:
            if (parse_cmd->string_ptr[9] == '1' && gnss_state == 0){
                // open GNSS
                gnss_at_command_app_open();

                gnss_state = 1;
                strcpy((char*) output.response_buf, "OK");
                output.response_len = strlen((char*) output.response_buf);
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
                atci_send_response(&output);
            }
            else if  (parse_cmd->string_ptr[9] == '0' && gnss_state == 1){
                // close GNSS
                gnss_at_command_app_close();
                gnss_state = 0;
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&output);
            }
            else if (parse_cmd->string_ptr[9] == '2') {
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                gnss_output_data = 0;
                atci_send_response(&output);
            } else if (parse_cmd->string_ptr[9] == '3') {
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                gnss_output_data = 1;
                atci_send_response(&output);
            } else {
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
                atci_send_response(&output);
            }
            break;

        default :
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&output);
            break;
    }
    return ATCI_STATUS_OK;
}

atci_status_t gnss_send_command_at_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};
    int32_t pmtk_command_len = 0;

    GNSSLOGD("gnss_send_command_at_handler:%s\n", parse_cmd->string_ptr);

    if (strstr("AT+EGPSS", (char *)parse_cmd->string_ptr))
    {
        return ATCI_STATUS_OK;
    }

    pmtk_command_len = strlen(parse_cmd->string_ptr) - strlen("AT+EGPSS=") - 2;

    memcpy(pmtk_command, parse_cmd->string_ptr + strlen("AT+EGPSS="), pmtk_command_len);
    pmtk_command[pmtk_command_len] = '\r';
    pmtk_command[pmtk_command_len + 1] = '\n';
    pmtk_command[pmtk_command_len + 2] = 0;
    pmtk_command_len += 3;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(&output);
            break;
    
        case ATCI_CMD_MODE_EXECUTION:
            if (gnss_state == 1){
                // Send PMTK command
                gnss_at_command_app_send_command((int8_t*) pmtk_command,pmtk_command_len);
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(&output);
            }
            else{
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
                atci_send_response(&output);
            }
            break;

        default :
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&output);
            break;
    }
    return ATCI_STATUS_OK;
}

#endif
