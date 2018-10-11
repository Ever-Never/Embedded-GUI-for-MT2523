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

#ifdef MTK_BT_AT_COMMAND_ENABLE
// For Register AT command handler
#include "at_command.h"
#include "at_command_bt.h"
#include "syslog.h"
#include <FreeRTOS.h>
#include <stdlib.h>

#include "queue.h"
#include "task.h"
#include "timers.h"

//For bt head file
#include "bt_gap.h"
#include "bt_system.h"
#ifdef MTK_PORT_SERVICE_ENABLE
#include "serial_port.h"
#endif
#include "nvdm.h"
#include "hal_trng.h"

log_create_module(atci_bt, PRINT_LEVEL_INFO);
atci_bt_context_struct_t atci_bt_cntx;
atci_bt_context_struct_t *atci_bt_cntx_p = &atci_bt_cntx;

#if 1
//extern function
extern bt_status_t bt_driver_power_on(void);
extern bt_status_t bt_driver_power_off(void);
extern uint8_t bt_hci_event_parser(uint8_t *param, uint8_t paramLen, atci_bt_hci_event *hci_event);
extern void bt_hci_log(const uint8_t in, const void * data, const uint32_t data_len);
extern uint8_t bt_driver_send_hci_command(uint16_t opCode, uint8_t cmd_len, uint8_t * cmd);
extern bool bt_driver_register_atci_rx_callback(void * rx_callback);
extern void bt_driver_deinit_atci_mode(void);

//static function
static void atci_bt_error_handle(void);
static int8_t atci_bt_str2hex(uint8_t *string, uint32_t string_len, uint8_t data[]);
static int8_t atci_bt_hex2str(uint8_t data[], uint8_t data_len, uint8_t *string, uint32_t string_len);

static int8_t atci_bt_hci_string_to_cmd(uint8_t *string, uint32_t string_len, atci_bt_hci_cmd *hci_cmd);
static int8_t atci_bt_hci_event_to_string(atci_bt_hci_event *hci_event, uint8_t *string, uint32_t string_len);
static void atci_bt_task_init(void);
static void acti_bt_task_deinit(void);
static void atci_bt_cmd_task_main(void *pvParameters);
static void acti_bt_task_msg_handler(at_command_bt_message_struct_t *message);
/*---  Variant ---*/
#define ATCI_COMMAND_TASK_NAME "AT Ehanced"
#define ATCI_COMMAND_TASK_STACK_SIZE 1024 //should be fine tune
#define ATCI_COMMAND_TASK_PRIORITY 3 //should be arrange by scenario
#define ATCI_COMMAND_QUEUE_SIZE 10

TaskHandle_t at_command_task_hdlr;
QueueHandle_t at_command_task_queue;
static TimerHandle_t event_timer = NULL;

static bool acti_bt_check_event_complete(uint8_t event)
{
    bool is_complete = 0;
    LOG_I(atci_bt, "check_event_complete--event = %d\r\n", event);
    switch (event) {
        case ATCI_BT_EVENT_INQUIRY_COMPLETE:
        case ATCI_BT_EVENT_CONNECT_COMPLETE:
        case ATCI_BT_EVENT_DISCONNECT_COMPLETE:
        case ATCI_BT_EVENT_AUTH_COMPLETE:
        case ATCI_BT_EVENT_REMOTE_NAME_REQ_COMPLETE:
        case ATCI_BT_EVENT_CHNG_CONN_LINK_KEY_COMPLETE:
        case ATCI_BT_EVENT_MASTER_LINK_KEY_COMPLETE:
        case ATCI_BT_EVENT_READ_REMOTE_FEATURES_COMPLETE:
        case ATCI_BT_EVENT_READ_REMOTE_VERSION_COMPLETE:
        case ATCI_BT_EVENT_QOS_SETUP_COMPLETE:
        case ATCI_BT_EVENT_COMMAND_COMPLETE:
        case ATCI_BT_EVENT_READ_CLOCK_OFFSET_COMPLETE:
        case ATCI_BT_EVENT_FLOW_SPECIFICATION_COMPLETE:
        case ATCI_BT_EVENT_READ_REMOTE_EXT_FEAT_COMPLETE:
        case ATCI_BT_EVENT_SYNC_CONNECT_COMPLETE:
        case ATCI_BT_EVENT_SIMPLE_PAIRING_COMPLETE:
        case ATCI_BT_EVENT_ENHANCED_FLUSH_COMPLETE: {
            is_complete = 1;
            break;
        }
        default:
            break;
    }
    LOG_I(atci_bt, "check_event_complete--is_complete = %d\r\n", is_complete);
    return is_complete;
}

static void acti_bt_delete_timer()
{
    if (event_timer && (xTimerIsTimerActive(event_timer) != pdFALSE)) {
        LOG_I(atci_bt, "event complete: timer is exist\r\n");
        xTimerStop(event_timer, 0);
        xTimerDelete(event_timer, 0);
        event_timer = NULL;
    }
}


void acti_bt_event_timer_handle_func(TimerHandle_t timer_id)
{
    LOG_I(atci_bt, "timer_handle:timer out\r\n");

    acti_bt_delete_timer();
    atci_bt_error_handle();

}


static void acti_bt_start_check_complete()
{


    if (event_timer && (xTimerIsTimerActive(event_timer) != pdFALSE)) {
        xTimerStop(event_timer, 0);

    } else {
        LOG_I(atci_bt, "timer is not exist\r\n");
        event_timer = xTimerCreate( "acti_bt_timer",
                                    (100000 / portTICK_PERIOD_MS),
                                    pdFALSE,
                                    NULL,
                                    acti_bt_event_timer_handle_func);
    }
    xTimerStart(event_timer, 0);
}

uint8_t bt_hci_event_parser(uint8_t *param, uint8_t paramLen, atci_bt_hci_event *hci_event)
{
    uint8_t data[260];
    uint16_t cmdOpcode;
    hci_event->event = 0;
    hci_event->status = 0;
    hci_event->handle = 0;
    hci_event->parmslen = 0;
    memset(hci_event->parms, 0, 256);

    if (param[1] != (paramLen - 2))
    {
        return 1;
    }
    memcpy(data, param, paramLen);
    hci_event->event = data[0];
    hci_event->parmslen = data[1];
    memcpy(hci_event->parms, &data[2], hci_event->parmslen);

    switch(hci_event->event)
    {
        case ATCI_HCE_INQUIRY_COMPLETE:
        case ATCI_HCE_REMOTE_NAME_REQ_COMPLETE:
        case ATCI_HCE_COMMAND_STATUS:
        case ATCI_HCE_ROLE_CHANGE:
        case ATCI_HCE_SIMPLE_PAIRING_COMPLETE:
            hci_event->status = data[2];
            break;
        case ATCI_HCE_CONNECT_COMPLETE:
        case ATCI_HCE_DISCONNECT_COMPLETE:
        case ATCI_HCE_AUTH_COMPLETE:
        case ATCI_HCE_ENCRYPT_CHNG:
        case ATCI_HCE_CHNG_CONN_LINK_KEY_COMPLETE:
        case ATCI_HCE_MASTER_LINK_KEY_COMPLETE:
        case ATCI_HCE_READ_REMOTE_FEATURES_COMPLETE:
        case ATCI_HCE_READ_REMOTE_VERSION_COMPLETE:
        case ATCI_HCE_QOS_SETUP_COMPLETE:
        case ATCI_HCE_MODE_CHNG:
        case ATCI_HCE_READ_CLOCK_OFFSET_COMPLETE:
        case ATCI_HCE_CONN_PACKET_TYPE_CHNG:
        case ATCI_HCE_FLOW_SPECIFICATION_COMPLETE:
        case ATCI_HCE_READ_REMOTE_EXT_FEAT_COMPLETE:
        case ATCI_HCE_SYNC_CONNECT_COMPLETE:
        case ATCI_HCE_SYNC_CONN_CHANGED:
        case ATCI_HCE_SNIFF_SUBRATING_EVENT:
        case ATCI_HCE_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT:
            hci_event->status = data[2];
            hci_event->handle = *((uint16_t *)(&(data[3])));
            break;
        case ATCI_HCE_COMMAND_COMPLETE:
            hci_event->status = data[5];
            memcpy(&cmdOpcode, data + 3, 2);
            switch(cmdOpcode)
            {
                case ATCI_HCC_READ_LMP_HANDLE:
                case ATCI_HCC_ROLE_DISCOVERY:
                case ATCI_HCC_READ_LINK_POLICY:
                case ATCI_HCC_WRITE_LINK_POLICY:
                case ATCI_HCC_SNIFF_SUBRATING:
                case ATCI_HCC_FLUSH:
                case ATCI_HCC_READ_AUTO_FLUSH_TIMEOUT:
                case ATCI_HCC_WRITE_AUTO_FLUSH_TIMEOUT:
                case ATCI_HCC_READ_XMIT_POWER_LEVEL:
                case ATCI_HCC_READ_LINK_SUPERV_TIMEOUT:
                case ATCI_HCC_WRITE_LINK_SUPERV_TIMEOUT:
                case ATCI_HCC_READ_FAILED_CONTACT_COUNT:
                case ATCI_HCC_RESET_FAILED_CONTACT_COUNT:
                case ATCI_HCC_GET_LINK_QUALITY:
                case ATCI_HCC_READ_RSSI:
                case ATCI_HCC_READ_AFH_CHANNEL_MAP:
                case ATCI_HCC_READ_CLOCK:
                case ATCI_HCC_LE_READ_CHANNEL_MAP:
                case ATCI_HCC_LE_LONG_TERM_KEY_REQ_REPL:
                case ATCI_HCC_LE_LONG_TERM_KEY_REQ_NEG_REPL:
                    hci_event->handle = *((uint16_t *)(&(data[6])));
                    break;
            }
            break;
        case ATCI_HCE_FLUSH_OCCURRED:
        case ATCI_HCE_MAX_SLOTS_CHNG:
        case ATCI_HCE_QOS_VIOLATION:
        case ATCI_HCE_LINK_SUPERVISION_TIMEOUT_CHNG:
        case ATCI_HCE_ENHANCED_FLUSH_COMPLETE:
            hci_event->handle = *((uint16_t *)(&(data[2])));
            break;
        case ATCI_HCE_LE_META_EVENT:
            switch(data[2])
            {
                case ATCI_HCE_LE_CONNECT_COMPLETE:
                    hci_event->status = data[3];
                    hci_event->handle = *((uint16_t *)(&(data[4])));
                    break;
                case ATCI_HCE_LE_CONNECT_UPDATE_COMPLETE:
                    hci_event->status = data[3];
                    hci_event->handle = *((uint16_t *)(&(data[4])));
                    break;
                case ATCI_HCE_LE_READ_REMOTE_FEATURES_COMPLETE:
                    hci_event->status = data[3];
                    hci_event->handle = *((uint16_t *)(&(data[4])));
                    break;
                case ATCI_HCE_LE_LONG_TERM_KEY_REQUEST_EVENT:
                    hci_event->handle = *((uint16_t *)(&(data[3])));
                    break;
                case ATCI_HCE_LE_ENHANCED_CONNECT_COMPLETE:
                    hci_event->status = data[3];
                    hci_event->handle = *((uint16_t *)(&(data[4])));
                    break;
            }
            break;
        case ATCI_HCE_CONNECT_REQUEST:
            //No status or hci handle
            break;
        case ATCI_HCE_INQUIRY_RESULT:
            //No status or hci handle
            break;
        case ATCI_HCE_HARDWARE_ERROR:
            //no status/hci handle
            break;
        case ATCI_HCE_NUM_COMPLETED_PACKETS:
            //for acl data should not used
            break;
        case ATCI_HCE_RETURN_LINK_KEYS:
            //No status/hci handle
            break;
        case ATCI_HCE_PIN_CODE_REQ:
            //No status/hci handle
            break;
        case ATCI_HCE_LINK_KEY_REQ:
            //No status/hci handle
            break;
        case ATCI_HCE_LINK_KEY_NOTIFY:
            //No status/hci handle
            break;
        case ATCI_HCE_LOOPBACK_COMMAND:
            //No status/hci handle
            break;
        case ATCI_HCE_DATA_BUFFER_OVERFLOW:
            //for acl data should not used
            break;
        case ATCI_HCE_PAGE_SCAN_REPETITION_MODE:
            //No status/hci handle
            break;
        case ATCI_HCE_INQUIRY_RESULT_WITH_RSSI:
            //No status/hci handle
            break;
        case ATCI_HCE_INQUIRY_RESULT_WITH_EIR:
            //No status/hci handle
            break;
        case ATCI_HCE_IO_CAPABILITY_REQUEST:
            //No status/hci handle
            break;
        case ATCI_HCE_IO_CAPABILITY_RESPONSE:
            //No status/hci handle
            break;
        case ATCI_HCE_USER_CONFIRM_REQUSEST:
            //No status/hci handle
            break;
        case ATCI_HCE_USER_PASSKEY_REQUEST_EVENT:
            //No status/hci handle
            break;
        case ATCI_HCE_REMOTE_OOB_DATA_REQUEST_EVENT:
            //No status/hci handle
            break;
        case ATCI_HCE_USER_PASSKEY_NOTIFICATION:
            //No status/hci handle
            break;
        case ATCI_HCE_USER_KEYPRESS:
            //No status/hci handle
            break;
        case ATCI_HCE_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT:
            //No status/hci handle
            break;
        case ATCI_HCE_BLUETOOTH_LOGO:
            //may not used
            break;
        case ATCI_HCE_VENDOR_SPECIFIC:
            //may not used
            break;
        default:
            //Not supported event
            break;
    }

    return 0;
}


/*--- Callback Function ---*/
static void acti_bt_task_msg_handler(at_command_bt_message_struct_t *message)
{
    uint8_t result = 0;
    atci_bt_hci_event hci_event;
    atci_response_t output = {{0}};
    uint8_t string[526] = {0};
    uint8_t *temp_str;

    LOG_I(atci_bt, "msglen :0x%x", message->param1);
    temp_str = message->param2;
    for (int i = 0 ; i < message->param1; i++) {
       LOG_I(atci_bt, "msg_handler--msg:%x", *(temp_str + i));
    }

    result = bt_hci_event_parser((uint8_t *)message->param2, message->param1, &hci_event);

    LOG_I(atci_bt, "parser result:%d", result);

    if (result == 1) {
        atci_bt_error_handle();
    } else {
        result = atci_bt_hci_event_to_string(&hci_event, string, 522);
        LOG_I(atci_bt, "event to string result:%d", result);
        if (result > 0) {
            bool is_complete;
            snprintf((char *)output.response_buf, sizeof(output.response_buf), "\r\n+EBTSHC:%s\r\n", (char *)string);
            LOG_I(atci_bt, "event to string event:%d", hci_event.event);
            is_complete = acti_bt_check_event_complete(hci_event.event);
            if (is_complete) {
                LOG_I(atci_bt, "event is complete.");
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                acti_bt_delete_timer();
            } else {
                LOG_I(atci_bt, "event is status.");
                output.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                acti_bt_start_check_complete();
            }

            output.response_len = strlen((char *)output.response_buf);
        } else {
            strcpy((char *)output.response_buf, "Response data error!\n");
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            output.response_len = strlen((char *)output.response_buf);
            acti_bt_delete_timer();
        }
        atci_send_response(&output);
    }

}

void atci_bt_hci_cmd_rx_callback(uint8_t *data, uint32_t dataLen)
{
    at_command_bt_message_struct_t msg = {0, 0, {0}};
    msg.message_id = ATCI_BT_COMMAND_HCI_EVENT;
    msg.param1 = dataLen;
    memcpy(msg.param2, data, dataLen);
    xQueueSendFromISR(at_command_task_queue, &msg, 0);

    return;
}

/*--- Command handler Function ---*/
atci_status_t atci_cmd_hdlr_bt_power(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};
    bt_status_t result = BT_STATUS_FAIL;
    uint8_t register_result;

    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: { /*AT+EBTPW=<op>*/
            LOG_I(atci_bt, "bt_power : %d", atci_bt_cntx_p->is_power);
            if (strncmp(parse_cmd->string_ptr, "AT+EBTPW=0", 10) == 0) {
                if (atci_bt_cntx_p->is_power) {
                    result = bt_driver_power_off();
                    bt_driver_deinit_atci_mode();
                    atci_bt_cntx_p->is_register_rx_callback = false;
                    acti_bt_task_deinit();
                    atci_bt_cntx_p->is_power = 0;
                } else {
                    result = BT_STATUS_SUCCESS;
                }
            } else if (strncmp(parse_cmd->string_ptr, "AT+EBTPW=1", 10) == 0) {

                if (!atci_bt_cntx_p->is_power) {
                    result = bt_driver_power_on();
                    if (result == BT_STATUS_SUCCESS) {
                        atci_bt_cntx_p->is_power = 1;
                        register_result = bt_driver_register_atci_rx_callback((void*)atci_bt_hci_cmd_rx_callback);
                        atci_bt_task_init();
                        if (register_result == 0) {
                            atci_bt_cntx_p->is_register_rx_callback = true;
                        } else {
                            atci_bt_cntx_p->is_register_rx_callback = false;
                        }
                    }
                } else {
                    result = BT_STATUS_SUCCESS;
                }
            } else {
                ; //not support
            }

            LOG_I(atci_bt, "result: 0x%x", result);
            if (result == BT_STATUS_SUCCESS) {
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            output.response_len = 0;
        }
        break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
        default: {
            strcpy((char *)output.response_buf, "Not Support\n");
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
        break;
    }

    atci_send_response(&output);
    return ATCI_STATUS_OK;
}


static void atci_bt_cmd_task_main(void *pvParameters)
{

    at_command_bt_message_struct_t queue_item;
    LOG_I(atci_bt, "enter task main.");

    at_command_task_queue = xQueueCreate(ATCI_COMMAND_QUEUE_SIZE, sizeof(at_command_bt_message_struct_t));
    while (1) {
        if (xQueueReceive(at_command_task_queue, &queue_item, portMAX_DELAY)) {
            LOG_I(atci_bt, "enter massge handler.");
            acti_bt_task_msg_handler(&queue_item);
        }
    }

}

static void atci_bt_task_init(void)
{
    LOG_I(atci_bt, "atci_bt_task_init.");

    xTaskCreate(atci_bt_cmd_task_main, ATCI_COMMAND_TASK_NAME, ATCI_COMMAND_TASK_STACK_SIZE, NULL, ATCI_COMMAND_TASK_PRIORITY, &at_command_task_hdlr);
}

static void acti_bt_task_deinit(void)
{
    vTaskDelete(at_command_task_hdlr);
}

atci_status_t atci_cmd_hdlr_bt_send_hci_command(atci_parse_cmd_param_t *parse_cmd)
{
    int8_t result;
    atci_bt_hci_cmd hci_cmd;
    atci_response_t output = {{0}};

    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: { //AT+EBTSHC=<op>
            LOG_I(atci_bt, "cmd_len:%d, name_len:%d, string_len:%d", strlen(parse_cmd->string_ptr), parse_cmd->name_len, parse_cmd->string_len);
            uint32_t op_length = strlen(parse_cmd->string_ptr) - parse_cmd->name_len - 1 - 2;
            result = atci_bt_hci_string_to_cmd((uint8_t *)parse_cmd->string_ptr + 10, op_length, &hci_cmd);
            LOG_I(atci_bt, "string to hci cmd result:%d", result);
            if (result > 0) {
                result = bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);
                atci_bt_cntx_p->is_sent_cmd = true;
                LOG_I(atci_bt, "result:%d", result);
            } else {
                output.response_flag = 0;
                strcpy((char *)output.response_buf, "Command Error\n");
                output.response_len = strlen((char *)output.response_buf);
                atci_send_response(&output);
            }
        }

        break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
        default: {
            strcpy((char *)output.response_buf, "Not Support\n");
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&output);
        }
        break;
    }

    return ATCI_STATUS_OK;
}

/*--- Static Function ---*/
static void atci_bt_error_handle(void)
{
    atci_response_t output;
    LOG_I(atci_bt, "exception handle.");

    output.response_len = 0;
    output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
    atci_send_response(&output);
}

static int8_t atci_bt_str2hex(uint8_t *string, uint32_t string_len, uint8_t data[])
{
    uint32_t i;
    uint8_t value = 0;
    uint8_t composed_value = 0;
    LOG_I(atci_bt, "string : %s, string_len : %d", string, string_len);

    for (i = 0; i < string_len; i++) {
        if ('0' <= string[i] && '9' >= string[i]) {
            value = string[i] - '0';
        } else if ('a' <= string[i] && 'f' >= string[i]) {
            value = string[i] - 'a' + 10;
        } else if ('A' <= string[i] && 'F' >= string[i]) {
            value = string[i] - 'A' + 10;
        } else {
            return -1;
        }

        if (i % 2 == 0) {
            composed_value += value * 16;
        } else {
            composed_value += value;
        }

        if (i % 2 == 1) {
            data[(i - 1) / 2] = composed_value;
            composed_value = 0;
        }

    }

    return 1;
}

static int8_t atci_bt_hex2str(uint8_t data[], uint8_t data_len, uint8_t *string, uint32_t string_len)
{
    uint32_t i;
    uint8_t high_8bits_string = 0;
    uint8_t low_8bits_string = 0;

    if ( (2 * data_len) > string_len ) {
        return -1;
    }

    for (i = 0; i < data_len; i++) {
        /*parse high 8 bits value*/
        high_8bits_string = data[i] / 16;
        if (high_8bits_string <= 9) {
            string[2 * i] = high_8bits_string + '0';
        } else if (10 <= high_8bits_string && high_8bits_string <= 15) {
            string[2 * i] = high_8bits_string - 10 + 'A';
        } else {
            return -2;
        }

        /*parse low 8 bits value*/
        low_8bits_string = data[i] % 16;
        if (low_8bits_string <= 9) {
            string[2 * i + 1] = low_8bits_string + '0';
        } else if (10 <= low_8bits_string && low_8bits_string <= 15) {
            string[2 * i + 1] = low_8bits_string - 10 + 'A';
        } else {
            return -3;
        }
    }

    return 1;
}

static int8_t atci_bt_hci_event_to_string(atci_bt_hci_event *hci_event, uint8_t *string, uint32_t string_len)
{
    int8_t parse_result = 0;
    uint8_t data[260];
    uint32_t string_pos = 0;
    uint32_t data_pos = 0;

    LOG_I(atci_bt, "event parmslen:%d", hci_event->parmslen);

    /*init data[]*/
    data[0] = hci_event->event;
    data[1] = hci_event->status ;
    data[2] = (uint8_t)((hci_event->handle & 0xFF00) >> 8);
    data[3] = (uint8_t)(hci_event->handle & 0x00FF) ;
    data[4] = hci_event->parmslen;
    memcpy(data + 5, hci_event->parms, hci_event->parmslen);

    /*1. parser hci_event->event*/
    parse_result = atci_bt_hex2str(data + data_pos, 1, string + string_pos, string_len);
    if (parse_result <= 0 ) {
        return -1;
    }
    string_pos += 2;
    data_pos += 1;
    string[string_pos++] = ',';

    /*2. parser hci_event->status*/
    parse_result = atci_bt_hex2str(data + data_pos, 1, string + string_pos, string_len - string_pos);
    if (parse_result <= 0 ) {
        return -2;
    }
    string_pos += 2;
    data_pos += 1;
    string[string_pos++] = ',';

    /*3. parser hci_event->handle*/
    parse_result = atci_bt_hex2str(data + data_pos, 2, string + string_pos, string_len - string_pos);
    if (parse_result <= 0 ) {
        return -3;
    }
    string_pos += 4;
    data_pos += 2;
    string[string_pos++] = ',';

    /*4. parser hci_event->parmslen*/
    parse_result = atci_bt_hex2str(data + data_pos, 1, string + string_pos, string_len - string_pos);
    if (parse_result <= 0 ) {
        return -5;
    }
    string_pos += 2;
    data_pos += 1;

    /*5. parser hci_event->parms[256]*/
    if (hci_event->parmslen > 0) {
        string[string_pos++] = ',';
        parse_result = atci_bt_hex2str(data + data_pos, hci_event->parmslen, string + string_pos, string_len - string_pos);
        if (parse_result <= 0 ) {
            return -6;
        }
        string_pos += 2 * hci_event->parmslen;
        data_pos += hci_event->parmslen;
    }

    string[string_pos] = '\0';

    return 1;
}

static int8_t atci_bt_hci_string_to_cmd(uint8_t *string, uint32_t string_len, atci_bt_hci_cmd *hci_cmd)
{
    int8_t parse_result = 0;
    uint8_t data[259];
    uint32_t string_pos = 0;
    uint32_t data_pos = 0;

    LOG_I(atci_bt, "command: %s, len:%d", string, string_len);

    /*reset hci cmd*/
    hci_cmd->op_code = 0;
    hci_cmd->cmd_len = 0;
    memset(hci_cmd->cmd, 0x0, 256);

    /*parse hci cmd string*/
    /*0401, 02, 1234*/

    /*parse opcode*/
    parse_result = atci_bt_str2hex(string + string_pos, 4, data + data_pos);
    if (parse_result <= 0) {
        return -1;
    }
    string_pos += 4;
    data_pos += 2;

    /*parse 1st ','*/
    if (string[string_pos] != ',') {
        return -2;
    }
    string_pos += 1;

    /*parse cmd length*/
    parse_result = atci_bt_str2hex(string + string_pos, 2, data + data_pos);
    if (parse_result <= 0) {
        return -3;
    }

    if (data[data_pos] == 0) {
        goto exit;
    }

    string_pos += 2;
    data_pos += 1;

    /*parse 2nd ','*/
    if (string[string_pos] != ',') {
        return -4;
    }
    string_pos += 1;

    LOG_I(atci_bt, "string : %s, str_pos:%d, data_pos = %d", string + string_pos, string_pos, data_pos);
    /*parse cmd data*/
    if (data[2] > 0) {
        parse_result = atci_bt_str2hex(string + string_pos, string_len - string_pos, data + data_pos);
        if (parse_result <= 0) {
            return -5;
        }
    }

exit:
    hci_cmd->op_code = ((data[0] << 8) | data[1]);
    hci_cmd->cmd_len = data[2];
    if (hci_cmd->cmd_len > 0) {
        memcpy(hci_cmd->cmd, data + 3, hci_cmd->cmd_len);
    }

    return 1;
}
#endif /*#if 0*/

extern bool bt_enable_relay_mode(hal_uart_port_t port);
#ifdef MTK_PORT_SERVICE_ENABLE
extern bool bt_enable_relay_mode_with_port_service(serial_port_dev_t port);
#endif

atci_status_t atci_cmd_hdlr_bt_relay(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};
    bool result;

    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: { /*AT+EBTER=<port>*/
            uint8_t port = *((uint8_t *)parse_cmd->string_ptr + 9) - '0';
            LOG_I(atci_bt, "port:%d", port);

#ifdef MTK_PORT_SERVICE_ENABLE
            if ((serial_port_dev_t)port >= SERIAL_PORT_DEV_MAX) {
#else
            if ((hal_uart_port_t)port >= HAL_UART_MAX) {
#endif
                output.response_len = 0;
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
                break;
            }

#ifdef MTK_PORT_SERVICE_ENABLE
            if ((serial_port_dev_t)port >= SERIAL_PORT_DEV_USB_TYPE_BEGIN)
            {
                result = bt_enable_relay_mode_with_port_service((serial_port_dev_t)port);
            } else
#endif
            {
                result = bt_enable_relay_mode((hal_uart_port_t)port);
            }

            LOG_I(atci_bt, "enter relay mode result: 0x%x", result);
            if (result) {
                output.response_len = 0;
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                output.response_len = 0;
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
        }
        break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
        default: {
            strcpy((char *)output.response_buf, "Not Support\n");
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
        break;
    }

    atci_send_response(&output);
    return ATCI_STATUS_OK;
}



/*---ATA Command handler Function Begain---*/
static void atci_bt_ata_response_result(bool is_success);
static void atci_bt_ata_cmd_paser(uint8_t *string);
static void atci_bt_ata_power_on_req(void);
static void atci_bt_ata_power_on_cnf(bt_status_t status, void*buf);
static void atci_bt_ata_power_off_req(void);
static void atci_bt_ata_power_off_cnf(bt_status_t status, void*buf);
static void atci_bt_ata_inquiry_ind(bt_status_t status, void*buf);
static void atci_bt_ata_inquiry_complete_ind(bt_status_t status, void*buf);
static void atci_bt_ata_read_remote_name_complete_ind(bt_status_t status, void*buf);
static void atci_bt_ata_search_handler(void);
static void atci_bt_ata_dut_handler(void);
static void atci_bt_ata_tx_config_handler(char* string);

static void atci_bt_ata_response_result(bool is_success)
{
    atci_response_t output = {{0}};
    LOG_I(atci_bt, "response result:%d.", is_success);
    output.response_len = 0;
    if (is_success) {
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
    } else {
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
    }
    atci_send_response(&output);
}

#ifdef MTK_BUILD_SMT_LOAD
static void atci_bt_ata_generate_local_address(bt_bd_addr_t addr)
{
    nvdm_status_t status;
    int8_t i;
    uint32_t random_seed;
    uint32_t size = 12;
    uint8_t buffer[18] = {0};
    uint8_t tmp_buf[3] = {0};
    bt_bd_addr_t tempaddr = {0};
    hal_trng_status_t ret = HAL_TRNG_STATUS_OK;
    if (memcmp(addr, tempaddr, sizeof(bt_bd_addr_t)) == 0) {
        LOG_I(atci_bt, "[BT]Empty bt address after bt_gap_le_get_local_address()\n");
        LOG_I(atci_bt, "[BT]Try to read from NVDM.\n");
        status = nvdm_read_data_item("BT", "address", buffer, &size);
        if (NVDM_STATUS_OK == status) {
            LOG_I(atci_bt, "[BT]Read from NVDM:%s\n", buffer);
            for (i = 0; i < 6; ++i) {
                tmp_buf[0] = buffer[2 * i];
                tmp_buf[1] = buffer[2 * i + 1];
                addr[i] = (uint8_t)strtoul((char *)tmp_buf, NULL, 16);
            }

            LOG_I(atci_bt, "[BT]Read address from NVDM [%02X:%02X:%02X:%02X:%02X:%02X]\n", addr[0],
                  addr[1], addr[2], addr[3], addr[4], addr[5]);
            return;
        } else {
            LOG_I(atci_bt, "[BT]Failed to Read from NVDM:%d.\n", status);
            ret = hal_trng_init();
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(atci_bt, "[BT]generate_random_address--error 1");
            }
            for (i = 0; i < 30; ++i) {
                ret = hal_trng_get_generated_random_number(&random_seed);
                if (HAL_TRNG_STATUS_OK != ret) {
                    LOG_I(atci_bt, "[BT]generate_random_address--error 2");
                }
                LOG_I(atci_bt, "[BT]generate_random_address--trn: 0x%x", random_seed);
            }
            /* randomly generate address */
            ret = hal_trng_get_generated_random_number(&random_seed);
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(atci_bt, "[BT]generate_random_address--error 3");
            }
            LOG_I(atci_bt, "[BT]generate_random_address--trn: 0x%x", random_seed);
            addr[0] = random_seed & 0xFF;
            addr[1] = (random_seed >> 8) & 0xFF;
            addr[2] = (random_seed >> 16) & 0xFF;
            addr[3] = (random_seed >> 24) & 0xFF;
            ret = hal_trng_get_generated_random_number(&random_seed);
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(atci_bt, "[BT]generate_random_address--error 3");
            }
            LOG_I(atci_bt, "[BT]generate_random_address--trn: 0x%x", random_seed);
            addr[4] = random_seed & 0xFF;
            addr[5] = (random_seed >> 8) & 0xCF;
            hal_trng_deinit();
        }
    }
    /* save address to NVDM */
    for (i = 0; i < 6; ++i) {
        sprintf((char *)buffer + 2 * i, "%02X", addr[i]);
    }
    LOG_I(atci_bt, "[BT]address to write:%s len:%d\n", buffer, strlen((char *)buffer));
    status = nvdm_write_data_item("BT", "address", NVDM_DATA_ITEM_TYPE_STRING, buffer, strlen((char *)buffer));
    if (NVDM_STATUS_OK != status) {
        LOG_I(atci_bt, "[BT]Failed to store address.\n");
    } else {
        LOG_I(atci_bt, "[BT]Successfully store address to NVDM [%02X:%02X:%02X:%02X:%02X:%02X]\n", addr[0],
              addr[1], addr[2], addr[3], addr[4], addr[5]);
    }
}
#endif /*MTK_BUILD_SMT_LOAD*/

static void atci_bt_ata_power_on_req(void)
{
    LOG_I(atci_bt, "power on req, host power on: %d", atci_bt_cntx_p->is_host_power_on);
   if (!atci_bt_cntx_p->is_host_power_on ) {
#ifdef MTK_BUILD_SMT_LOAD
        bt_bd_addr_t local_public_addr = {0};
        atci_bt_ata_generate_local_address(local_public_addr);
        bt_status_t result = bt_power_on(local_public_addr, NULL);
#else /*MTK_BUILD_SMT_LOAD*/
        bt_status_t result = bt_power_on(NULL, NULL);
#endif /*MTK_BUILD_SMT_LOAD*/
        LOG_I(atci_bt, "power on req result: 0x%x", result);
        if (BT_STATUS_SUCCESS == result ) {
        /*waiting power on cnf event*/
            return;
        } else  if (BT_STATUS_FAIL == result) {
            atci_bt_cntx_p->is_host_power_on = true;
            atci_bt_ata_response_result(true);
        } else {
            atci_bt_ata_response_result(false);
        }
    } else {
       atci_bt_ata_response_result(true);
    }
}

static void atci_bt_ata_power_on_cnf(bt_status_t status, void*buf)
{
    LOG_I(atci_bt, "power on cnf result: 0x%x", status);
    if (status == BT_STATUS_SUCCESS) {
        atci_bt_cntx_p->is_host_power_on = true;
        atci_bt_ata_response_result(true);
    } else {
        atci_bt_ata_response_result(false);
    }
}

static void atci_bt_ata_power_off_req(void)
{
    LOG_I(atci_bt, "power off req, host power on: %d", atci_bt_cntx_p->is_host_power_on);
    if (!atci_bt_cntx_p->is_host_power_on) {
        atci_bt_ata_response_result(true);
    } else {
        bt_status_t result = bt_power_off();
          LOG_I(atci_bt, "power off req result: 0x%x", result);
        if (BT_STATUS_SUCCESS == result) {
             /*waiting power off cnf event*/
             return;
         } else if (BT_STATUS_FAIL == result) {
            atci_bt_cntx_p->is_host_power_on = false;
            atci_bt_ata_response_result(true);
         } else {
             atci_bt_ata_response_result(false);
             memset((void *)atci_bt_cntx_p, 0x0, sizeof(atci_bt_context_struct_t));
         }
    }
}

static void atci_bt_ata_power_off_cnf(bt_status_t status, void*buf)
{
    LOG_I(atci_bt, "power off cnf result: 0x%x", status);
    atci_bt_cntx_p->is_host_power_on = false;
    memset((void *)atci_bt_cntx_p, 0x0, sizeof(atci_bt_context_struct_t));
    if (status == BT_STATUS_SUCCESS) {
        atci_bt_ata_response_result(true);
    } else {
        atci_bt_ata_response_result(false);
    }
}

static void atci_bt_ata_inquiry_ind(bt_status_t status, void*buf)
{
    bt_gap_inquiry_ind_t* device = (bt_gap_inquiry_ind_t*) buf;
    LOG_I(atci_bt, "discovery result ind: 0x%x", status);
    if (status) {
        atci_bt_cntx_p->searched_num++;
        bt_gap_read_remote_name(device->address);
    } else {
        atci_bt_ata_response_result(false);
    }
}

static void atci_bt_ata_inquiry_complete_ind(bt_status_t status, void*buf)
{
    LOG_I(atci_bt, "discovery complete result: 0x%x, searched num: %d", status, atci_bt_cntx_p->searched_num);

    atci_bt_cntx_p->is_inquiry_completed = true;
    if (atci_bt_cntx_p->searched_num == 0) {
        atci_bt_ata_response_result(true);
    }
}

static void atci_bt_ata_read_remote_name_complete_ind(bt_status_t status, void*buf)
{
    uint8_t temp_str[50] = {0};
    bt_bd_addr_t remote_address ={0};
    bt_gap_read_remote_name_complete_ind_t *remote_name_ind = (bt_gap_read_remote_name_complete_ind_t *)buf;
    if ((status != BT_STATUS_SUCCESS) ||(buf == NULL) ) {
        atci_bt_ata_response_result(false);
        return;
    }
    memcpy((void *)&remote_address, (void *) (remote_name_ind->address), 6);
    atci_bt_cntx_p->searched_num-- ;
    LOG_I(atci_bt, "read remote name complete ind: searched num: %d", atci_bt_cntx_p->searched_num);
    /*send URC to atci*/
    atci_response_t output = {{0}};
    snprintf((char *)temp_str, sizeof(temp_str), "%s, 0x%.2X%.2X%.2X%.2X%.2X%.2X", remote_name_ind->name,
        remote_address[5], remote_address[4], remote_address[3],
        remote_address[2], remote_address[1], remote_address[0]);
    snprintf ((char *)output.response_buf, sizeof(output.response_buf), "+EBTAT:%s\r\n", (char *)temp_str);
    LOG_I(atci_bt, "response string:%s", output.response_buf);
    output.response_len = strlen((char *)output.response_buf);
    output.response_flag = 0 | ATCI_RESPONSE_FLAG_URC_FORMAT;
    atci_send_response(&output);

    /*send final response to atci*/
    if (atci_bt_cntx_p->is_inquiry_completed && (atci_bt_cntx_p->searched_num == 0)) {
        output.response_len = 0;
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&output);
    }
}

extern bool bt_driver_enter_dut_mode(void);
static void atci_bt_ata_dut_handler(void)
{
    bool result;
    result = bt_driver_enter_dut_mode();
    LOG_I(atci_bt, "enter dut result: 0x%x", result);
    atci_bt_ata_response_result(result);
}

static void atci_bt_ata_tx_config_handler(char* string)
{
    bt_status_t status = BT_STATUS_FAIL;
    uint8_t bt_init_tx, bt_max_tx, le_init_tx;
    bt_init_tx = (uint8_t)strtoul(string, NULL, 10);
    le_init_tx = (uint8_t)strtoul(string + 4, NULL, 10);
    bt_max_tx = (uint8_t)strtoul(string + 2, NULL, 10);
    
    if ((bt_init_tx | bt_max_tx | le_init_tx) <= 7) {
        bt_config_tx_power_t tx_cfg = {.bt_init_tx_power_level = bt_init_tx, .bt_max_tx_power_level = bt_max_tx, .le_init_tx_power_level = le_init_tx};
        status = bt_config_tx_power_level(&tx_cfg);
    }
    LOG_I(atci_bt, "atci_bt_ata_tx_config_handler(%d, %d, %d) status 0x%x", bt_init_tx, bt_max_tx, le_init_tx, status);
    if (BT_STATUS_SUCCESS == status) {
        atci_bt_ata_response_result(true);
    } else {
        atci_bt_ata_response_result(false);
    }
}

static void atci_bt_ata_search_handler(void)
{
    bt_status_t result = bt_gap_inquiry(ATCI_BT_ATA_INQUIRY_TIME, ATCI_BT_ATA_INQUIRY_NUM);

    LOG_I(atci_bt, "inquiry result: 0x%x", result);
    if ((BT_STATUS_SUCCESS == result) ||(BT_STATUS_FAIL == result)) {
        /*waiting inquiry cnf event*/
    } else {
        atci_bt_ata_response_result(false);
    }
}

static void atci_bt_ata_cmd_paser(uint8_t *string)
{
    atci_bt_cntx_p->ata = true;
    LOG_I(atci_bt, "enter parser, command: %s", string);
    if (strncmp((char *)string, "bt_power_on", 11) == 0) {
        atci_bt_ata_power_on_req();
    } else if (strncmp((char *)string, "bt_power_off", 12) == 0) {
        atci_bt_cntx_p->is_host_power_on = true;
        atci_bt_ata_power_off_req();
    } else if (strncmp((char *)string, "bt_search", 9) == 0) {
        atci_bt_ata_search_handler();
    } else if (strncmp((char *)string, "dut", 3) == 0) {
        atci_bt_ata_dut_handler();
    } else if (strncmp((char *)string, "tx_config ", sizeof("tx_config")) == 0) {
        atci_bt_ata_tx_config_handler((char *)string + sizeof("tx_config"));
    }  else {
        atci_bt_ata_response_result(false);
    }
}

atci_status_t atci_cmd_hdlr_bt_ata(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};

    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: /*AT+EBTAT=<op>*/
            atci_bt_ata_cmd_paser((uint8_t *)parse_cmd->string_ptr + 9);
            break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
        default: {
            strcpy((char *)output.response_buf, "Not Support\n");
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&output);
        }
        break;
    }

    return ATCI_STATUS_OK;
}

bt_status_t bt_gap_ata_callback(bt_msg_type_t msg, bt_status_t status, void*buf)
{

    LOG_I(atci_bt, "receive msg: 0x%x, status:0x%x, ata:%d", msg, status, atci_bt_cntx_p->ata);
    if (!atci_bt_cntx_p->ata) {
        return BT_STATUS_UNSUPPORTED;
    }

    switch (msg) {
        case BT_POWER_ON_CNF:
        {
            atci_bt_ata_power_on_cnf(status, buf);
        }
        break;

        case BT_POWER_OFF_CNF:
        {
            atci_bt_ata_power_off_cnf(status, buf);
        }
        break;

        case BT_GAP_INQUIRY_CNF:
        {
            if (status != BT_STATUS_SUCCESS) {
                atci_bt_ata_response_result(false);
            }
        }
        break;

        case BT_GAP_CANCEL_INQUIRY_CNF:
        {
            if (status != BT_STATUS_SUCCESS) {
                atci_bt_ata_response_result(false);
            }
        }
        break;

        case BT_GAP_INQUIRY_IND:
        {
            atci_bt_ata_inquiry_ind(status, buf);
        }
        break;

        case BT_GAP_INQUIRY_COMPLETE_IND:
        {
            atci_bt_ata_inquiry_complete_ind(status, buf);
        }
        break;

        case BT_GAP_READ_REMOTE_NAME_CNF:
        {
            if (status != BT_STATUS_SUCCESS) {
                atci_bt_ata_response_result(false);
            }
        }
        break;

        case BT_GAP_READ_REMOTE_NAME_COMPLETE_IND:
        {
            atci_bt_ata_read_remote_name_complete_ind(status, buf);
        }
        break;
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

/*---ATA Command handler Function End---*/

#endif  /*MTK_BT_AT_COMMAND_ENABLE*/

#ifdef __BT_DEBUG__
#include "at_command.h"
#include "at_command_bt.h"
#include "syslog.h"

log_create_module(atci_bt_driver_dump, PRINT_LEVEL_INFO);

extern void bt_driver_dump_log_switch(bool flag);

#if _MSC_VER >= 1500
    #pragma comment(linker, "/alternatename:_bt_gattc_rx_proc_error_rsp=_bt_driver_dump_log_switch_default")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION)
    #pragma weak bt_driver_dump_log_switch = bt_driver_dump_log_switch_default
#else
    #error "Unsupported Platform"
#endif

void bt_driver_dump_log_switch_default(bool flag)
{
    LOG_I(atci_bt_driver_dump, "Not support with no Bluetooth module");
}

atci_status_t atci_cmd_hdlr_bt_enable_driver_dump_log(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};

    LOG_I(atci_bt_driver_dump, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: { /*AT+EBTEDL=<op>*/
            uint8_t is_enable = *((uint8_t *)parse_cmd->string_ptr + 10) - '0';
            LOG_I(atci_bt_driver_dump, "enable:%d", is_enable);

            bt_driver_dump_log_switch((bool)is_enable);
            output.response_len = 0;
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
        }
        break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
        default: {
            strcpy((char *)output.response_buf, "Not Support\n");
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
        break;
    }

    atci_send_response(&output);
    return ATCI_STATUS_OK;
}
#endif //__BT_DEBUG__

