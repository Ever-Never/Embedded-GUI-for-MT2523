/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
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

#ifdef __CFW_CONFIG_MODE__
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "atci.h"
#include "at_command.h"
#include "syslog.h"
#include "cfw.h"

#define ATCI_CFW_ITEM_MIN_SIZE 51 /* key(4), value(n), name(32), type(2), address(8);*/
static char* atci_cmd_hdlr_cfw_param_int(char* cmd);
static uint32_t atci_cmd_hdlr_cfw_output_append_item_int(char* dest, uint32_t size, cfw_data_info_t* info);

atci_status_t atci_cmd_hdlr_cfw(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};
    cfw_data_info_t item_info;
    char* cmd_temp;
    uint32_t i;
    uint32_t total = 0;
    *(parse_cmd->string_ptr + parse_cmd->string_len - 2) = '\0'; //remove \r\n
    LOG_I(common, "[CFW]cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);
    char* cmd = parse_cmd->string_ptr + sizeof("AT+ECFG");
    cmd_temp = atci_cmd_hdlr_cfw_param_int(cmd);
    memcpy(output.response_buf, "+ECFG=", sizeof("+ECFG="));
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
        {
            output.response_len = sizeof("+ECFG=1.0");
            output.response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
            memcpy(output.response_buf, "+ECFG=1.0", output.response_len);
            atci_send_response(&output);
            break;
        }
        case ATCI_CMD_MODE_EXECUTION: //AT+EBTAT=<op>
        {
            if (strncmp(cmd, "dump", 4) == 0) {
                uint32_t max_index = cfw_get_max_item_count();
                uint32_t add_len, len = 0;
                for (i = 0; i < max_index; i++) {
                    cfw_get_data_info_by_index(&item_info, i);
                    add_len = atci_cmd_hdlr_cfw_output_append_item_int((char *)output.response_buf + len + sizeof("+ECFG=") - 1, 1000 - len, &item_info);
                    len += add_len;
                    if ((add_len == 0) && (len > 0)) {
                        output.response_len = len + sizeof("+ECFG=") - 1;
                        total += output.response_len;
                        output.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                        atci_send_response(&output);
                        len = atci_cmd_hdlr_cfw_output_append_item_int((char *)output.response_buf + sizeof("+ECFG=") - 1, 1000, &item_info);
                    }
                }
                if (len > 0) {
                    output.response_len = len + sizeof("+ECFG=") - 1;
                    total += output.response_len;
                    output.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                    atci_send_response(&output);
                    sprintf((char *)output.response_buf, "+ECFG=%d", (int)(total + sizeof("+ECFG=") - 1));
                    output.response_len = strlen((char *)output.response_buf);
                    output.response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
                    atci_send_response(&output);
                }
                break;
            }
            else if (strncmp(cmd, "read", 4) == 0) {
                if (cmd_temp) {
                    atci_cmd_hdlr_cfw_param_int(cmd_temp); /* key */
                    item_info.key = (uint16_t)strtoul(cmd_temp, NULL, 16);
                }
                if (0 != cfw_get_data_info_by_key(&item_info)) {
                    strcpy((char *)output.response_buf, "Key not find\n");
                    output.response_len = strlen((char *)output.response_buf);
                    output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    atci_send_response(&output);
                    break;
                }
                output.response_len = sizeof("+ECFG=") + atci_cmd_hdlr_cfw_output_append_item_int((char*)(output.response_buf + sizeof("+ECFG=") - 1), 1000, &item_info);
                output.response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
                atci_send_response(&output);
                break;
            }
            else if (strncmp(cmd, "write", 5) == 0) {
                char value[4] = {0};
                cmd = cmd_temp;
                unsigned int snumber;
                cmd_temp = atci_cmd_hdlr_cfw_param_int(cmd_temp);
                item_info.key = (uint16_t)strtoul(cmd, NULL, 16);
                atci_cmd_hdlr_cfw_param_int(cmd_temp);
                if (0 != cfw_get_data_info_by_key(&item_info)) {
                    strcpy((char *)output.response_buf, "Key not find\n");
                    output.response_len = strlen((char *)output.response_buf);
                    output.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    atci_send_response(&output);
                    break;
                }
                if ((item_info.type == cfw_type_array_int8_t) || (item_info.type == cfw_type_array_uint8_t)) {
                    value[0] = *(cmd_temp);
                    value[1] = *(cmd_temp + 1);
                    i = 0;
                    while (value[0] != 0) {
                        sscanf(value, "%02x", &snumber);
                        *(cmd + (i>>1)) = (char)snumber;
                        i += 2;
                        value[0] = *(cmd_temp + i);
                        value[1] = *(cmd_temp + i + 1);
                        total++;
                    }
                }
                else if (item_info.type == cfw_type_array_char) {
                    total = strlen(cmd_temp) + 1;
                    cmd = cmd_temp;
                }
                else {
                    sscanf(cmd_temp, "%d", (int*)cmd);
                    total = item_info.size;
                }
                
                LOG_I(common, "[CFW]write key %4X, size:%d", item_info.key, total);
                cfw_write_data(item_info.key, (const uint8_t*)cmd, &total);
                sprintf((char *)output.response_buf, "+ECFG=%d", 0);
                output.response_len = strlen((char *)output.response_buf);
                output.response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
                atci_send_response(&output);
                break;
            }
            else if (strncmp(cmd, "flush", 5) == 0) {
                cfw_flush();
                sprintf((char *)output.response_buf, "+ECFG=%d", 0);
                output.response_len = strlen((char *)output.response_buf);
                output.response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
                atci_send_response(&output);
                break;
            }
        }
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
static char* atci_cmd_hdlr_cfw_param_int(char* cmd)
{
    char* cmd_end;
    cmd_end = cmd;
    while (*cmd_end != ' ') {
        if ((*cmd_end == ',') || (*cmd_end == ';') || (*cmd_end == '\0'))
        {
            break;
        }
        cmd_end++;
    }
    if (*cmd_end == '\0') {
        cmd_end = NULL;
    }
    else {
        *cmd_end = '\0';
        cmd_end++;
        while ((*cmd_end == ' ') || (*cmd_end == ',') || (*cmd_end == ';')) {
            cmd_end++;
        }
        if (*cmd_end == '\0') {
            cmd_end = NULL;
        }
    }
    return cmd_end;
}

static uint32_t atci_cmd_hdlr_cfw_output_append_item_int(char* dest, uint32_t size, cfw_data_info_t* info)
{
    char* p = dest;
    uint32_t i = 0;

    if (size < (uint32_t)(ATCI_CFW_ITEM_MIN_SIZE + (info->size) * 2)) {
        return 0;
    }
    p +=sprintf(p, "%04X,", info->key);
    if ((info->type == cfw_type_array_int8_t) || (info->type == cfw_type_array_uint8_t)) {
        for (i = 0; i < info->size; i++) {
            p += sprintf(p, "%02X", *(info->address + i));
        }
    }
    else if (info->type == cfw_type_array_char) {
        p += sprintf(p, "%s", (const char*)(info->address));
    }
    else if (info->type == cfw_type_int8_t) {
        p += sprintf(p, "%d", *((int8_t*)(info->address)));
    }
    else if (info->type == cfw_type_uint8_t) {
        p += sprintf(p, "%d", *((uint8_t*)(info->address)));
    }
    else if (info->type == cfw_type_int16_t) {
        p += sprintf(p, "%d", *((int16_t*)(info->address)));
    }
    else if (info->type == cfw_type_uint16_t) {
        p += sprintf(p, "%d", *((uint16_t*)(info->address)));
    }
    else if (info->type == cfw_type_int32_t) {
        p += sprintf(p, "%d", *((int*)(info->address)));
    }
    else if (info->type == cfw_type_uint32_t) {
        p += sprintf(p, "%u", *((unsigned int*)(info->address)));
    }
    else {
        printf("type is not support\n");
    }
    p += sprintf(p, ",%s,%02X,%d,%p;", info->var_name, info->type, info->size, info->address);
    return p - dest;
}

#endif
