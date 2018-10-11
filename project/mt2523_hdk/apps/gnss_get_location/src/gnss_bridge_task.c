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

#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

#include "gnss_app.h"
#include "task_def.h"
// This option is used to enable connect powerGPS tool via UART
// This feature is only for debug usage, and will impact the power consumption.
// You can disable it when you don't need it.
#ifdef GNSS_SUPPORT_TOOL_BRIDGE

#include "gnss_bridge.h"
#include "gnss_ring_buffer.h"
#include "atci.h"
#include "atci_main.h"

#define GNSS_BRIDGE_QUEUE_SIZE 40
#define GNSS_BRIDGE_COMMAND_BUFFER_SIZE 256
#define GNSS_BRIDGE_RING_BUFFER_SIZE 10240
#define GNSS_BRIDGE_TRY_READ_TIME 80
typedef enum gnss_bridge_message_enum {
    GNSS_BRIDGE_MESSAGE_ID_CAN_READ,
        GNSS_BRIDGE_MESSAGE_ID_CAN_WRITE,
        GNSS_BRIDGE_MESSAGE_ID_WRITE_DATA,
        GNSS_BRIDGE_MESSAGE_ID_TIMEOUT
} gnss_bridge_message_enum;

typedef struct gnss_bridge_message_struct {
    int message_id;
    int param1;
    void* param2;
} gnss_bridge_message_struct_t;

static struct {
    QueueHandle_t gnss_bridge_task_queue;
} gnss_bridge_task_cntx;

static uint8_t gnss_bridge_command_buf[GNSS_BRIDGE_COMMAND_BUFFER_SIZE + 1];
static ring_buf_struct_t gnss_bridge_ring_buf;
static uint8_t ring_buf[GNSS_BRIDGE_RING_BUFFER_SIZE];

static int32_t put_len = 0;
static int32_t consume_len = 0;

/**
* @brief       This function used to put gnss data to ring buffer
* @param[in]   buf: Data buffer pointer
* @param[in]   buf_len: The data length
* @return      int32_t: the put data size
*/
uint32_t gnss_bridge_put_data(uint8_t *buf, uint32_t buf_len)
{
    if (gnss_bridge_ring_buf.buf !=0) {
        put_len += buf_len;
        put_data(&gnss_bridge_ring_buf, (int8_t*)buf, (int32_t)buf_len); 
        return buf_len;
    } else {
        return 0;
    }
}

/**
* @brief       The uart callback function
* @param[in]   gnss_bridge_callback_type_t: UART event
* @return      None
*/
static void gnss_bridge_task_callback(gnss_bridge_callback_type_t type)
{
    gnss_bridge_message_struct_t message = {0};
    BaseType_t xHigherPriorityTaskWoken;
    if (type == GNSS_BRIDGE_CALLBACK_TYPE_CAN_READ) {
        message.message_id = GNSS_BRIDGE_MESSAGE_ID_CAN_READ;
    }
    else if (type == GNSS_BRIDGE_CALLBACK_TYPE_CAN_WRITE) {
        message.message_id = GNSS_BRIDGE_MESSAGE_ID_CAN_WRITE;
    }
    xQueueSendFromISR(gnss_bridge_task_cntx.gnss_bridge_task_queue, &message, &xHigherPriorityTaskWoken);
}

/**
* @brief       Bridge task message process function
* @param[in]   gnss_bridge_message_struct_t: Message pointer
* @return      None
*/
static void gnss_bridge_msg_handler(gnss_bridge_message_struct_t *message)
{
    int32_t ret;
    if (!message) {
        return;
    }
    switch (message->message_id) {
        case GNSS_BRIDGE_MESSAGE_ID_CAN_READ:
            do {
                ret = gnss_bridge_read_command(gnss_bridge_command_buf, GNSS_BRIDGE_COMMAND_BUFFER_SIZE);
                if (ret > 0) {
                    gnss_app_forward_cmd((int8_t*)gnss_bridge_command_buf, ret);
                }
            } while (ret >= GNSS_BRIDGE_COMMAND_BUFFER_SIZE);
            break;
        case GNSS_BRIDGE_MESSAGE_ID_CAN_WRITE:
        case GNSS_BRIDGE_MESSAGE_ID_WRITE_DATA:
        case GNSS_BRIDGE_MESSAGE_ID_TIMEOUT:
            {
                static uint8_t temp_buf[GNSS_BRIDGE_COMMAND_BUFFER_SIZE + 1];
                static uint32_t data_len;
                static uint32_t send_len;
                do {
                    if (data_len == 0) {
                        data_len = consume_data(&gnss_bridge_ring_buf, (int8_t*)temp_buf, GNSS_BRIDGE_COMMAND_BUFFER_SIZE);
                        consume_len += data_len;
                        temp_buf[data_len] = 0;
                        //GNSSLOGD("comsume data [%s]\r\n", temp_buf);
                    }
                    if (data_len > 0) {
                        send_len += gnss_bridge_send_debug_data(temp_buf + send_len, data_len - send_len);
                    } else {
                        break;
                    }
                    if (data_len == send_len) {
                        data_len = send_len = 0;
                    } else {
                        GNSSLOGD("wait can write message,left size:%d\r\n", data_len - send_len);
                        break;
                    }
                } while (1);
            }
            break;
        default:
            break;
    }
}

/**
* @brief       Bridge task main function
* @return      None
*/
static void gnss_bridge_task_main()
{
    gnss_bridge_message_struct_t queue_item;
#if defined(MTK_PORT_SERVICE_ENABLE)
    //gnss_bridge_port_init(SERIAL_PORT_DEV_UART_1, gnss_bridge_task_callback);
#else
    gnss_uart_bridge_init(GNSS_BRIDGE_UART_PORT, gnss_bridge_task_callback);
#endif
    GNSSLOGD("gnss_bridge_task_main\r\n");
    while (1) {
        if (xQueueReceive(gnss_bridge_task_cntx.gnss_bridge_task_queue, &queue_item, portMAX_DELAY)) {
            gnss_bridge_msg_handler(&queue_item);
        }
    }
}

/**
* @brief       Time out callback function
* @param[in]   timer_handle: Timer handle
* @return      None
*/
static void gnss_bridge_timer_callback( TimerHandle_t timer_handle )
{
    gnss_bridge_message_struct_t message;
    message.message_id = GNSS_BRIDGE_MESSAGE_ID_TIMEOUT;
    xQueueSend(gnss_bridge_task_cntx.gnss_bridge_task_queue, &message, 0);
}

/**
* @brief       This function used to create bridge task
* @return      None
*/
TaskHandle_t gnss_bridge_task_init()
{
    TaskHandle_t task_handler;
    TimerHandle_t time_handle;
    GNSSLOGD("gnss_bridge_task_init\r\n");
    ring_buf_init(&gnss_bridge_ring_buf, (int8_t*)ring_buf, GNSS_BRIDGE_RING_BUFFER_SIZE);
    gnss_bridge_task_cntx.gnss_bridge_task_queue = xQueueCreate( GNSS_BRIDGE_QUEUE_SIZE, sizeof( gnss_bridge_message_struct_t ) );
    time_handle = xTimerCreate("BRDGT",       // Just a text name, not used by the kernel.
        ( GNSS_BRIDGE_TRY_READ_TIME ),   // The timer period in ticks.
        pdTRUE,        // The timer will auto-reload themselves when they expire.
        ( void * ) 0,  // Assign each timer a unique id equal to its array index.
        gnss_bridge_timer_callback // Each timer calls the same callback when it expires.
        );
    xTimerStart(time_handle, 0);
    task_handler = (TaskHandle_t) xTaskCreate( (TaskFunction_t) gnss_bridge_task_main, 
        GNSS_BRIDGE_TASK_NAME, 
        GNSS_BRIDGE_TASK_STACK_SIZE/(( uint32_t )sizeof( StackType_t )), 
        NULL, 
        GNSS_BRIDGE_TASK_PRIO, 
        &task_handler );
    GNSSLOGD("create task handler:%dr\n", task_handler);
    return task_handler;
}

/**
* @brief       This function used to deinit bridge task
* @return      None
*/
void gnss_bridge_task_deinit(TaskHandle_t taskHandler)
{
    GNSSLOGD("gnss_bridge_task_deinit handle:%d\r\n", taskHandler);
#if defined(MTK_PORT_SERVICE_ENABLE)
    gnss_serial_port_deinit();
#else
    gnss_uart_bridge_deinit();
#endif
    vTaskDelete(taskHandler);
}

#if defined(MTK_PORT_SERVICE_ENABLE)
void gnss_bridge_port_reinit(serial_port_dev_t port)
{
    gnss_serial_port_deinit();
    gnss_bridge_task_init();
    gnss_bridge_port_init(port, gnss_bridge_task_callback);
}
#endif


atci_status_t gnss_agps_at_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};

    GNSSLOGD("gnss_agps_at_handler:%s\n", parse_cmd->string_ptr);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_ACTIVE:
             if (strstr((char *)parse_cmd->string_ptr, "POWERGPS")) {
#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE) && defined(GNSS_SUPPORT_TOOL_BRIDGE)
                serial_port_dev_t port;
                serial_port_setting_uart_t uart_setting;
            
                if (serial_port_config_read_dev_number("atci", &port) != SERIAL_PORT_STATUS_OK)
                {
                    port = SERIAL_PORT_DEV_USB_COM1;//SERIAL_PORT_DEV_UART_1;
                    serial_port_config_write_dev_number("atci", port);
                    LOG_W(common, "serial_port_config_write_dev_number setting uart1");
                    uart_setting.baudrate = HAL_UART_BAUDRATE_115200;
                    serial_port_config_write_dev_setting(port, (serial_port_dev_setting_t *)&uart_setting);
                }
                atci_deinit((hal_uart_port_t) port);
                gnss_bridge_port_reinit(port);
                return ATCI_STATUS_OK;
#endif
            }
            strcpy((char*) output.response_buf, "OK");
            output.response_len = strlen((char*) output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
            atci_send_response(&output);
            break;
        default :
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&output);
            break;
    }
    return ATCI_STATUS_OK;
}

atci_status_t gnss_agps_pmtk_cmd_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};
    char* pmtk_command;
    GNSSLOGD("gnss_agps_at_handler:%s\n", parse_cmd->string_ptr);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION:
            if (strstr((char *)parse_cmd->string_ptr, "SENDCMD")) {
                pmtk_command = strstr((char *)parse_cmd->string_ptr, "=") + 1;
                gnss_app_forward_cmd((int8_t*) pmtk_command, strlen(pmtk_command));
            }
            strcpy((char*) output.response_buf, "OK");
            output.response_len = strlen((char*) output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
            atci_send_response(&output);
            break;
        default :
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&output);
            break;
    }
    return ATCI_STATUS_OK;
}


void gnss_bridge_atcmd_init()
{
    int32_t ret;
    static atci_cmd_hdlr_item_t gnss_app_atcmd[] = {
        {"AT+POWERGPS",     gnss_agps_at_handler,   0, 0},
        {"AT+SENDCMD",     gnss_agps_pmtk_cmd_handler,   0, 0},
    };
    ret = atci_register_handler(gnss_app_atcmd, sizeof(gnss_app_atcmd) / sizeof(atci_cmd_hdlr_item_t));
    if (ret == ATCI_STATUS_OK) {
        GNSSLOGD("at_example_init register success\r\n");
    } else {
        GNSSLOGE("at_example_init register fail\r\n");
    }
}


#endif


