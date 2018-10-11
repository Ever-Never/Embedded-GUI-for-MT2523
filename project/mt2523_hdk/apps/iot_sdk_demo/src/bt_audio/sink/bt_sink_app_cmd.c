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

static atci_status_t bt_sink_app_atci_handler(atci_parse_cmd_param_t *parse_cmd);

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
    ",DOUBLECLICK"
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

                if (NULL != mapping_table) {
                    uint32_t index = 0;
                    result = BT_SINK_APP_CMD_RESULT_NO_MAPPING;

                    while (BT_SINK_SRV_KEY_NONE != mapping_table[index].key_value) {
                        if (bt_sink_app_context.state & mapping_table[index].sink_state
                                && key == mapping_table[index].key_value
                                && action == mapping_table[index].key_action) {
                            bt_sink_app_ext_cmd_t *ext_cmd_p = (bt_sink_app_ext_cmd_t *)pvPortMalloc(sizeof(*ext_cmd_p));
                            ext_cmd_p->key_value = key;
                            ext_cmd_p->key_action = action;
                            bt_sink_app_event_post((bt_sink_srv_event_t)BT_SINK_EVENT_APP_EXT_COMMAND,
                                                   (void *)ext_cmd_p,
                                                   bt_sink_app_command_post_callback);
                            result = BT_SINK_APP_CMD_RESULT_SUCCESS;
                            break;
                        }
                        index++;
                    }
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

atci_status_t bt_sink_app_atci_handler(atci_parse_cmd_param_t *parse_cmd)
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
                            ATCI_UART_TX_FIFO_BUFFER_SIZE);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + 5, "HCI", 3)) {
                result = bt_sink_app_hci_command_handler(parse_cmd->string_ptr + 9);
                if (result == BT_SINK_APP_CMD_RESULT_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    strncpy((char *)response.response_buf,
                            bt_sink_app_atci_error_string_list[result],
                            ATCI_UART_TX_FIFO_BUFFER_SIZE);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
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


void bt_sink_app_atci_indicate(bt_sink_srv_state_t state)
{
    atci_response_t *response = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));

    bt_sink_app_report("[Sink] response:0x%x, state:0x%x", response, state);

    if (NULL != response) {
        memset(response, 0, sizeof(*response));

        strcpy((char *)response->response_buf, "Sink Status: ");

        if (state & BT_SINK_SRV_STATE_STREAMING) {
            strcat((char *)response->response_buf, "STREAMING,");
        }
        if (state & BT_SINK_SRV_STATE_MULTIPARTY) {
            strcat((char *)response->response_buf, "MULTIPARTY,");
        }
        if (state & BT_SINK_SRV_STATE_HELD_REMAINING) {
            strcat((char *)response->response_buf, "HELD_REMAINING,");
        }
        if (state & BT_SINK_SRV_STATE_HELD_ACTIVE) {
            strcat((char *)response->response_buf, "HELD_ACTIVE,");
        }
        if (state & BT_SINK_SRV_STATE_TWC_OUTGOING) {
            strcat((char *)response->response_buf, "TWC_OUTGOING,");
        }
        if (state & BT_SINK_SRV_STATE_TWC_INCOMING) {
            strcat((char *)response->response_buf, "TWC_INCOMING,");
        }
        if (state & BT_SINK_SRV_STATE_ACTIVE) {
            strcat((char *)response->response_buf, "ACTIVE,");
        }
        if (state & BT_SINK_SRV_STATE_OUTGOING) {
            strcat((char *)response->response_buf, "OUTGOING,");
        }
        if (state & BT_SINK_SRV_STATE_INCOMING) {
            strcat((char *)response->response_buf, "INCOMING,");
        }
        if (state & BT_SINK_SRV_STATE_CONNECTED) {
            strcat((char *)response->response_buf, "CONNECTED,");
        }
        if (state & BT_SINK_SRV_STATE_DISCOVERABLE) {
            strcat((char *)response->response_buf, "DISCOVERABLE,");
        }
        if (state & BT_SINK_SRV_STATE_POWER_ON) {
            strcat((char *)response->response_buf, "POWER_ON\r\n");
        }
        if (BT_SINK_SRV_STATE_NONE == state) {
            strcat((char *)response->response_buf, "POWER_OFF\r\n");
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

