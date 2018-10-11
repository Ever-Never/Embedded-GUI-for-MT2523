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

#include "nvdm.h"
#include "atci.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_conmgr.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_action.h"
#include "bt_sink_srv_resource.h"
#include "bt_sink_srv_audio_tunning.h"
#include "bt_sink_srv_ami.h"

extern bt_hfp_audio_codec_type_t g_sink_srv_hf_audio_codec;

static atci_status_t bt_sink_srv_atci_handler(atci_parse_cmd_param_t *parse_cmd);

static atci_cmd_hdlr_item_t bt_sink_srv_atci_cmd[] = {
    {
        .command_head = "AT+BTADDR",    /**< AT command string. */
        .command_hdlr = bt_sink_srv_atci_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BTSINKIT",    /* INTERNAL USE, IT TEST */
        .command_hdlr = bt_sink_srv_atci_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    }
};

static int16_t bt_sink_srv_cm_convert_bt_addr(const char *index, uint8_t *bt_addr)
{
    int16_t result = 0;
    uint32_t total_num = strlen(index), bt_count = 6, bt_bit = 1;
    const char *temp_index = index;
    bt_sink_srv_memset(bt_addr, 0, sizeof(bt_bd_addr_t));
    while (total_num) {
        if (*temp_index <= '9' && *temp_index >= '0') {
            bt_addr[bt_count - 1] += ((*temp_index - '0') * (bt_bit * 15 + 1));
            if (bt_bit == 0) {
                bt_count--;
                bt_bit = 1;
            } else {
                bt_bit--;
            }

        } else if (*temp_index <= 'F' && *temp_index >= 'A') {
            bt_addr[bt_count - 1] += ((*temp_index - 'A' + 10) * (bt_bit * 15 + 1));
            if (bt_bit == 0) {
                bt_count--;
                bt_bit = 1;
            } else {
                bt_bit--;
            }
        } else if (*temp_index <= 'f' && *temp_index >= 'a') {
            bt_addr[bt_count - 1] += ((*temp_index - 'a' + 10) * (bt_bit * 15 + 1));
            if (bt_bit == 0) {
                bt_count--;
                bt_bit = 1;
            } else {
                bt_bit--;
            }
        }
        if (!bt_count) {
            break;
        }
        total_num--;
        temp_index++;
    }

    if (bt_count) {
        bt_sink_srv_memset(bt_addr, 0, sizeof(bt_bd_addr_t));
        result = -1;
    }

    bt_sink_srv_report("local addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
                       bt_addr[5],
                       bt_addr[4],
                       bt_addr[3],
                       bt_addr[2],
                       bt_addr[1],
                       bt_addr[0]);
    return result;
}

int16_t bt_sink_srv_cm_write_local_address(bt_bd_addr_t *local_addr)
{
    nvdm_status_t result =
        nvdm_write_data_item("BT_SINK",
                             "address",
                             NVDM_DATA_ITEM_TYPE_RAW_DATA,
                             (const uint8_t *)local_addr,
                             sizeof(bt_bd_addr_t));

    bt_sink_srv_report("write address result:%d", result);
    return (int16_t)result;
}

bt_sink_srv_status_t bt_sink_srv_cm_read_local_address(uint8_t *local_addr)
{
    bt_sink_srv_status_t status = BT_SINK_SRV_STATUS_FAIL;
    uint32_t size = sizeof(bt_bd_addr_t);

    if (NULL != local_addr) {
        nvdm_status_t nvdm_status;
        nvdm_status = nvdm_read_data_item("BT_SINK", "address", (uint8_t *)local_addr, &size);

        bt_sink_srv_report("read address result:%d", nvdm_status);

        if (NVDM_STATUS_OK == nvdm_status) {
            status = BT_SINK_SRV_STATUS_SUCCESS;
        }

        bt_sink_srv_report("local addr: %02X:%02X:%02X:%02X:%02X:%02X\n",
                           local_addr[5],
                           local_addr[4],
                           local_addr[3],
                           local_addr[2],
                           local_addr[1],
                           local_addr[0]);
    }
    return status;
}
extern void bt_gap_dump(void);
extern void bt_driver_trigger_controller_codedump(void);
#define CMD_PARAM(s) s, bt_sink_srv_strlen(s)
static int16_t bt_sink_srv_cmd_entry(const char *string)
{
    bt_sink_srv_report("[Sink][INJECT] string:%s", string);

    /* ACTION PART */
    if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("INQ"))) {
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_INQUIRY, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("DSC"))) { //DISCOVERABLE
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_DISCOVERABLE, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("CLD"))) { //CONNECT LAST DEVICE
        bt_bd_addr_t *address_p = bt_sink_srv_cm_last_connected_device();
        if (NULL != address_p) {
            bt_sink_srv_action_send(BT_SINK_SRV_ACTION_CONNECT, (void *)address_p);
        } else {
            bt_sink_srv_report("[Sink][CM] No trusted devices");
        }
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("DIS"))) { // DISCONNECT
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_DISCONNECT, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("ANS"))) { //ANSWER
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_ANSWER, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("REJ"))) { //REJECT
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_REJECT, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("HAG"))) { //HANG UP
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_HANG_UP, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("DLN"))) { //DIAL LAST NUMBER
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_DIAL_LAST, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("RAH"))) { // RELEASE ALL HELD
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_3WAY_RELEASE_ALL_HELD, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("RAA"))) { // RELEASE AND ACCEPT
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_3WAY_RELEASE_ACTIVE_ACCEPT_OTHER, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("HAA"))) { // HOLD AND ACCEPT
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("SAP"))) { // SWITCH AUDIO PATH
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("SAD"))) { // SWITCH AUDIO DEVICE
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_SWITCH_AUDIO_DEVICE, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("RSI"))) { // RELEASE SPECIAL INDEX
        uint8_t index = (uint8_t)((*(string + 3)) - '0');
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_3WAY_RELEASE_SPECIAL, &index);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("HSI"))) { // HOLD SPECIAL INDEX
        uint8_t index = (uint8_t)((*(string + 3)) - '0');
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_3WAY_RELEASE_SPECIAL, &index);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("AHTC"))) { // ADD HELD TO CONVERSATION
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_3WAY_ADD_HELD_CALL_TO_CONVERSATION, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("ECT"))) { // EXPLICIT CALL TRANSFER
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_3WAY_EXPLICIT_CALL_TRANSFER, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("VRA"))) { // VOICE RECOGNITION ACTIVATE
        bool active = (bool)((*(string + 3)) - '0');
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE, &active);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("VUP"))) { // VOLUME UP
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_VOLUME_UP, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("VDN"))) { // VOLUME DOWN
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_VOLUME_DOWN, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("VMIN"))) { // VOLUME MIN
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_VOLUME_MIN, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("VMAX"))) { // VOLUME MAX
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_VOLUME_MAX, NULL);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("CLCC"))) {
        bt_bd_addr_t *address_p = bt_sink_srv_cm_last_connected_device();

        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_QUERY_CALL_LIST, address_p);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("DTMF"))) {
        bt_bd_addr_t *address_p = bt_sink_srv_cm_last_connected_device();
        bt_sink_srv_send_dtmf_t request;

        bt_sink_srv_memset(&request, 0, sizeof(request));
        bt_sink_srv_memcpy(&request.bt_addr, address_p, sizeof(bt_bd_addr_t));
        request.code = *(string + 4);
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_DTMF, &request);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("RESET"))) {
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_RESET_TRUSTED_LIST, NULL);
        /* KEY ACTION PART */
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("KEYDOWN"))) { // KEY PRESS DOWN
        bt_sink_srv_key_action(BT_SINK_SRV_KEY_FUNC, BT_SINK_SRV_KEY_ACT_PRESS_DOWN);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("KEYUP"))) { // KEY PRESS UP
        bt_sink_srv_key_action(BT_SINK_SRV_KEY_FUNC, BT_SINK_SRV_KEY_ACT_PRESS_UP);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("KEYLDOWN"))) { // KEY LONG PRESS DOWN
        bt_sink_srv_key_action(BT_SINK_SRV_KEY_FUNC, BT_SINK_SRV_KEY_ACT_LONG_PRESS_DOWN);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("KEYLUP"))) { // KEY LONG PRESS UP
        bt_sink_srv_key_action(BT_SINK_SRV_KEY_FUNC, BT_SINK_SRV_KEY_ACT_LONG_PRESS_UP);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("DUMP"))) {
        bt_sink_srv_report("DUMP");
        bt_gap_dump();
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("KEYNEXT"))) {
        bt_sink_srv_key_action(BT_SINK_SRV_KEY_NEXT, BT_SINK_SRV_KEY_ACT_PRESS_UP);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("KEYPREV"))) {
        bt_sink_srv_key_action(BT_SINK_SRV_KEY_PREV, BT_SINK_SRV_KEY_ACT_PRESS_UP);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("ASSERT"))) {
        bt_sink_srv_assert(0);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("LOGON-MED"))) {
        extern uint32_t log_switch;
        log_switch |= 0x0001;
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("LOGOFF-MED"))) {
        extern uint32_t log_switch;
        log_switch &= ~(0x0001);
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("SKEW"))) {
        #ifdef MTK_BT_AWS_ENABLE
        extern void bt_sink_srv_skew_test(const char *value);
        bt_sink_srv_skew_test(string + 5);
        #endif
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("CVSD"))) {
        g_sink_srv_hf_audio_codec = (bt_hfp_audio_codec_type_t)BT_HFP_CODEC_TYPE_CVSD;
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("MSBC"))) {
        g_sink_srv_hf_audio_codec = (bt_hfp_audio_codec_type_t)BT_HFP_CODEC_TYPE_MSBC;
#ifdef __MUSIC_MODE_SWITCH_SUPPORT__
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("MODESWITCH"))) {
        bt_sink_srv_ami_switch_music_mode();
#endif
    } else if (0 == bt_sink_srv_memcmp(string, CMD_PARAM("FW-DUMP"))) {
        bt_driver_trigger_controller_codedump();
    }

    return 0;
}

atci_status_t bt_sink_srv_atci_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    bt_bd_addr_t local_address;
    int16_t result;

    response.response_flag = 0; /* Command Execute Finish. */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+BTADDR=<BT address> */
            if (0 == bt_sink_srv_memcmp(parse_cmd->string_ptr + 5, "ADDR", 4)) {
                result = bt_sink_srv_cm_convert_bt_addr(parse_cmd->string_ptr + 10, (uint8_t *)&local_address);
                if (result == 0) {
                    result = bt_sink_srv_cm_write_local_address(&local_address);
                    if (result == 0) {
                        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    } else {
                        snprintf((char *)response.response_buf,
                                 ATCI_UART_TX_FIFO_BUFFER_SIZE,
                                 "write error:%d\r\n",
                                 result);
                        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    }
                } else {
                    strcpy((char *)response.response_buf, "invalid address\r\n");
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else if (0 == bt_sink_srv_memcmp(parse_cmd->string_ptr + 5, "SINKIT", 6)) {
                result = bt_sink_srv_cmd_entry(parse_cmd->string_ptr + 12);
                if (result == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    snprintf((char *)response.response_buf,
                             ATCI_UART_TX_FIFO_BUFFER_SIZE,
                             "command error:%d\r\n",
                             result);
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

void bt_sink_srv_atci_init(void)
{
    atci_status_t ret;

    ret = atci_register_handler(bt_sink_srv_atci_cmd, sizeof(bt_sink_srv_atci_cmd) / sizeof(atci_cmd_hdlr_item_t));

    if (ret != ATCI_STATUS_OK) {
        bt_sink_srv_report("at_cmd_init register fail\r\n");
    }

#ifdef __BT_SINK_AUDIO_TUNING__
    bt_sink_hf_audio_tuning_atci_init();
#endif /* __BT_SINK_AUDIO_TUNING__ */

#ifdef __BT_SINK_SRV_AUDIO_TUNING__
    bt_sink_srv_audio_tunning_init();
#endif
}

