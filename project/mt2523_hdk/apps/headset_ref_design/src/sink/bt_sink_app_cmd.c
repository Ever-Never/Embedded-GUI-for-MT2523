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

#include "FreeRTOS.h"
#include "atci.h"
#include <string.h>
#include "bt_sink_srv.h"
#include "bt_sink_app_event.h"
#include "bt_sink_app_cmd.h"
#include "bt_sink_app_main.h"
#include "task_def.h"

static atci_status_t bt_sink_app_atci_handler(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_sink_app_atci_role_handler(atci_parse_cmd_param_t *parse_cmd);

static const char *bt_sink_app_key_list[] = {
    "NONE,",
    "FUNC,",
    "NEXT,",
    "PREV,",
    "VOLUP,",
    "VOLDOWN,"
};

static const char *bt_sink_app_action_list[] = {
    ",NONE",
    ",PRESSDOWN",
    ",PRESSUP",
    ",LONGPRESSDOWN",
    ",LONGPRESSUP",
    ",LONGLONGPRESSDOWN",
    ",LONGLONGPRESSUP",
    ",VERYLONGPRESSDOWN",
    ",VERYLONGPRESSUP",
    ",DOUBLECLICK",
    ",TRIPLECLICK"
};

static const char *bt_sink_app_atci_error_string_list[] = {
    "Success\r\n",
    "Unknown error\r\n",
    "Invalid command format\r\n",
    "Invalid key value\r\n",
    "Invalid key action\r\n",
    "No mapping action\r\n",
    "Link is not existed\r\n"
};

static atci_cmd_hdlr_item_t bt_sink_app_atci_cmd[] = {
    {
        .command_head = "AT+BTSINK",    /**< AT command string. */
        .command_hdlr = bt_sink_app_atci_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BTHCI",    /**< AT command string. */
        .command_hdlr = bt_sink_app_atci_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BTSTACK",    /**< AT command string. */
        .command_hdlr = bt_sink_app_atci_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BTROLE",    /**< AT command string. */
        .command_hdlr = bt_sink_app_atci_role_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BTSYS",    /**< AT command string. */
        .command_hdlr = bt_sink_app_atci_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    }
};

void bt_sink_app_command_post_callback(bt_sink_srv_event_t event_id, bt_sink_srv_status_t result, void *parameters)
{
    if (NULL != parameters) {
        vPortFree(parameters);
    }
}

bt_sink_app_cmd_result_t bt_sink_app_command_handler(char *command)
{
    bt_sink_app_cmd_result_t result = BT_SINK_APP_CMD_RESULT_UNKNOWN;

    if (NULL != command) {
        uint16_t i;
        bt_sink_srv_key_value_t key;
        bt_sink_srv_key_action_t action;
        uint16_t count = sizeof(bt_sink_app_key_list) / sizeof(char *);

        for (i = 1; i < count; i++) {
            if (NULL != strstr(command, bt_sink_app_key_list[i])) {
                key = (bt_sink_srv_key_value_t)i;
                break;
            }
        }

        if (count == i) {
            result = BT_SINK_APP_CMD_RESULT_INVALID_KEY;
        } else {
            count = sizeof(bt_sink_app_action_list) / sizeof(char *);

            for (i = 1; i < count; i++) {
                if (NULL != strstr(command, bt_sink_app_action_list[i])) {
                    action = (bt_sink_srv_key_action_t)i;
                    break;
                }
            }

            if (count == i) {
                result = BT_SINK_APP_CMD_RESULT_INVALID_ACT;
            } else {
                const bt_sink_srv_table_t *mapping_table = bt_sink_srv_get_mapping_table();
                result = BT_SINK_APP_CMD_RESULT_NO_MAPPING;
                if (NULL != mapping_table) {
                    bt_sink_app_ext_cmd_t *ext_cmd_p = (bt_sink_app_ext_cmd_t *)pvPortMalloc(sizeof(*ext_cmd_p));
                    ext_cmd_p->key_value = key;
                    ext_cmd_p->key_action = action;
                    bt_sink_app_event_post((bt_sink_srv_event_t)BT_SINK_EVENT_APP_EXT_COMMAND,
                                                   (void *)ext_cmd_p,
                                                   bt_sink_app_command_post_callback);
                    result = BT_SINK_APP_CMD_RESULT_SUCCESS;
                }
            }
        }
    } else {
        result = BT_SINK_APP_CMD_RESULT_INVALID_FORMAT;
    }
    return result;
}

bt_sink_app_cmd_result_t bt_sink_app_hci_command_handler(char *command)
{
    bt_sink_app_cmd_result_t result = BT_SINK_APP_CMD_RESULT_UNKNOWN;

    if (0 == strncmp(command, "ON", strlen("ON"))) {
        bt_sink_app_event_post(BT_SINK_EVENT_APP_HCI_LOG_ON, NULL, NULL);
        result = BT_SINK_APP_CMD_RESULT_SUCCESS;
    } else if (0 == strncmp(command, "OFF", strlen("OFF"))) {
        bt_sink_app_event_post(BT_SINK_EVENT_APP_HCI_LOG_OFF, NULL, NULL);
        result = BT_SINK_APP_CMD_RESULT_SUCCESS;
    }
    return result;
}

bt_sink_app_cmd_result_t bt_sink_app_sys_command_handler(char *command)
{
    bt_sink_app_cmd_result_t result = BT_SINK_APP_CMD_RESULT_UNKNOWN;

    if (0 == strncmp(command, "ON", strlen("ON"))) {
        bt_sink_app_event_post(BT_SINK_EVENT_APP_SYS_LOG_ON, NULL, NULL);
        result = BT_SINK_APP_CMD_RESULT_SUCCESS;
    } else if (0 == strncmp(command, "OFF", strlen("OFF"))) {
        bt_sink_app_event_post(BT_SINK_EVENT_APP_SYS_LOG_OFF, NULL, NULL);
        result = BT_SINK_APP_CMD_RESULT_SUCCESS;
    }
    return result;
}

static atci_status_t bt_sink_app_atci_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    bt_sink_app_cmd_result_t result;

    response.response_flag = 0; /* Command Execute Finish. */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION:
            /* rec: AT+BTSINK=<key>,<action> */
            if (0 == memcmp(parse_cmd->string_ptr + 5, "SINK", 4)) {
                result = bt_sink_app_command_handler(parse_cmd->string_ptr + 10);
                if (result == BT_SINK_APP_CMD_RESULT_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    strncpy((char *)response.response_buf,
                            bt_sink_app_atci_error_string_list[result],
                            ATCI_UART_TX_FIFO_BUFFER_SIZE - 1);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + 5, "HCI", 3)) {
                result = bt_sink_app_hci_command_handler(parse_cmd->string_ptr + 9);
                if (result == BT_SINK_APP_CMD_RESULT_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    strncpy((char *)response.response_buf,
                            bt_sink_app_atci_error_string_list[result],
                            ATCI_UART_TX_FIFO_BUFFER_SIZE - 1);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + 5, "SYS", 3)) {
                result = bt_sink_app_sys_command_handler(parse_cmd->string_ptr + 9);
                if (result == BT_SINK_APP_CMD_RESULT_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    strncpy((char *)response.response_buf,
                            bt_sink_app_atci_error_string_list[result],
                            ATCI_UART_TX_FIFO_BUFFER_SIZE - 1);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + 5, "STACK", 5)) {
                task_def_dump_stack_water_mark();
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        default :
            /* others are invalid command format */
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

static atci_status_t bt_sink_app_atci_role_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    response.response_flag = 0; /* Command Execute Finish. */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION:
            /* rec: AT+ROLE=<action>,<value> */
            if (0 == memcmp(parse_cmd->string_ptr + 10, "GET", 3)) {
                bt_sink_app_device_role_t role = bt_sink_app_get_device_role();
                if (role == BT_SINK_APP_DEVICE_MASTER) {
                    strcpy((char *)response.response_buf, "MASTER\n");
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else if (role == BT_SINK_APP_DEVICE_SLAVE) {
                    strcpy((char *)response.response_buf, "SLAVE\n");
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    strcpy((char *)response.response_buf, "Error role.\n");
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + 10, "SET", 3)) {
                if (0 == strncmp(parse_cmd->string_ptr + 14, "MASTER", strlen("MASTER"))) {
                    bt_sink_app_set_device_role(BT_SINK_APP_DEVICE_MASTER);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else if (0 == strncmp(parse_cmd->string_ptr + 14, "SLAVE", strlen("SLAVE"))) {
                    bt_sink_app_set_device_role(BT_SINK_APP_DEVICE_SLAVE);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else {
                strcpy((char *)response.response_buf, "Wrong action.\n");
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
        break;

        default :
            /* others are invalid command format */
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

void bt_sink_app_atci_indicate(bt_sink_app_indicate_type_t type, uint32_t params)
{
    atci_response_t *response = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));

    if (NULL != response) {
        memset(response, 0, sizeof(*response));

        switch (type) {
            case BT_SINK_APP_IND_TYPE_STATE: {
                bt_sink_srv_state_t state = (bt_sink_srv_state_t)params;
                bt_sink_app_report("[Sink] state:0x%x", state);
                strcpy((char *)response->response_buf, "Sink Status: ");
                if (state == BT_SINK_SRV_STATE_STREAMING) {
                    strcat((char *)response->response_buf, "STREAMING.\r\n");
                } else if (state == BT_SINK_SRV_STATE_MULTIPARTY) {
                    strcat((char *)response->response_buf, "MULTIPARTY.\r\n");
                } else if (state == BT_SINK_SRV_STATE_HELD_REMAINING) {
                    strcat((char *)response->response_buf, "HELD_REMAINING.\r\n");
                } else if (state ==BT_SINK_SRV_STATE_HELD_ACTIVE) {
                    strcat((char *)response->response_buf, "HELD_ACTIVE.\r\n");
                }else  if (state == BT_SINK_SRV_STATE_TWC_OUTGOING) {
                    strcat((char *)response->response_buf, "TWC_OUTGOING.\r\n");
                } else if (state ==BT_SINK_SRV_STATE_TWC_INCOMING) {
                    strcat((char *)response->response_buf, "TWC_INCOMING.\r\n");
                } else if (state == BT_SINK_SRV_STATE_ACTIVE) {
                    strcat((char *)response->response_buf, "ACTIVE.\r\n");
                } else if (state == BT_SINK_SRV_STATE_OUTGOING) {
                    strcat((char *)response->response_buf, "OUTGOING.\r\n");
                } else if (state == BT_SINK_SRV_STATE_INCOMING) {
                    strcat((char *)response->response_buf, "INCOMING.\r\n");
                }else  if (state == BT_SINK_SRV_STATE_CONNECTED) {
                    strcat((char *)response->response_buf, "IDLE (CONNECTED).\r\n");
                } else if (state == BT_SINK_SRV_STATE_POWER_ON) {
                    strcat((char *)response->response_buf, "BT_POWER_ON (NOT CONNECTED).\r\n");
                } else if (state == BT_SINK_SRV_STATE_LOCAL_PLAYING) {
                    strcat((char *)response->response_buf, "LOCAL_MUSIC_PLAYING.\r\n");
                } else if (state == BT_SINK_SRV_STATE_NONE) {
                    strcat((char *)response->response_buf, "BT_POWER_OFF.\r\n");
                } else {
                    strcat((char *)response->response_buf, "ERROR STATE!\r\n");
                }
            }
            break;

            case BT_SINK_APP_IND_TYPE_CALLER: {
                snprintf((char *)response->response_buf,
                         ATCI_UART_TX_FIFO_BUFFER_SIZE,
                         "Alert: Incoming call:%s\r\n",
                         (char *)params);
            }
            break;

            case BT_SINK_APP_IND_TYPE_MISSED_CALL: {
                snprintf((char *)response->response_buf,
                         ATCI_UART_TX_FIFO_BUFFER_SIZE,
                         "Alert: Missed call:%s\r\n",
                         (char *)params);
            }
            break;

            case BT_SINK_APP_IND_TYPE_LINK_LOST: {
                strcpy((char *)response->response_buf, "Alert: All Profiles disconnected.\r\n");
            }
            break;
            
            case BT_SINK_APP_IND_TYPE_VISIBILITY: {
                if (params == 1) {
                    strcpy((char *)response->response_buf, "Alert: Discoverable.\r\n");
                } else {
                    strcpy((char *)response->response_buf, "Alert: Not discoverable.\r\n");
                }
            }
            break;

            case BT_SINK_APP_IND_TYPE_PROFILE_CONN_STATE: {
                bt_sink_srv_profile_connection_state_update_t *profile_state = (bt_sink_srv_profile_connection_state_update_t*) params;
                strcpy((char *)response->response_buf, "Alert: ");
                if (profile_state->profile == BT_SINK_SRV_PROFILE_HFP) {
                    strcat((char *)response->response_buf, "HFP ");
                } else if (profile_state->profile == BT_SINK_SRV_PROFILE_A2DP_SINK) {
                    strcat((char *)response->response_buf, "A2DP ");
                } else if (profile_state->profile == BT_SINK_SRV_PROFILE_AVRCP) {
                    strcat((char *)response->response_buf, "AVRCP ");
                } 
                if (profile_state->state == BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED) {
                    strcat((char *)response->response_buf, "disconnected.\r\n");
                } else if (profile_state->state == BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED) {
                    strcat((char *)response->response_buf, "connected.\r\n");
                }
            }
            break;

            case BT_SINK_APP_INT_TYPE_NOTI_NEW: {
                strcpy((char *)response->response_buf, "Alert: New notification\r\n");
            }
            break;
            
            case BT_SINK_APP_IND_TYPE_HF_SCO_STATE: {
                if (params == 1) {
                    strcpy((char *)response->response_buf, "Alert: SCO connected.\r\n");
                } else {
                    strcpy((char *)response->response_buf, "Alert: SCO disconnected.\r\n");
                }
            }
            break;

            case BT_SINK_APP_IND_TYPE_NOTI_MISSED_CALL: {
                strcpy((char *)response->response_buf, "Alert: Missed call notification\r\n");
            }
            break;
            
            case BT_SINK_APP_INT_TYPE_NOTI_SMS: {
                strcpy((char *)response->response_buf, "Alert: New message\r\n");
            }
            break;

            default:
                break;
        }
        response->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
        response->response_len = strlen((char *)response->response_buf);

        bt_sink_app_report("[Sink] string length:%d", response->response_len);

        if (response->response_len > 0) {
            atci_send_response(response);
        }
        vPortFree(response);
    }
}

void bt_sink_app_atci_init(void)
{
    atci_register_handler(bt_sink_app_atci_cmd, sizeof(bt_sink_app_atci_cmd) / sizeof(atci_cmd_hdlr_item_t));
}

