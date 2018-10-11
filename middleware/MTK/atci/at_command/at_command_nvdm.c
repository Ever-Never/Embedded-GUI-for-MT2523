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
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "syslog.h"
#include "at_command.h"
#include "nvdm.h"

#if  defined ( __GNUC__ )
#ifndef __weak
#define __weak   __attribute__((weak))
#endif /* __weak */
#endif /* __GNUC__ */

#define AT_COMMAND_NVDM_HELP \
"AT+ENVDM - Read, write or reset value of data item according to NVDM.\r\n\
usage:\r\n\
   Read value of data item: AT+ENVDM=0,<group>,<data_item>\r\n\
   Write value of data item: AT+ENVDM=1,<group>,<data_item>,<length>,<data>\r\n\
   Reset value of data item: AT+ENVDM=2[,<group>]\r\n"

static uint32_t hexstring_to_bytearray(char *s, uint8_t *bits)
{
    uint32_t i, n = 0;

    for (i = 0; s[i]; i += 2) {
        if (s[i] >= 'A' && s[i] <= 'F') {
            bits[n] = s[i] - 'A' + 10;
        } else if (s[i] >= 'a' && s[i] <= 'f') {
            bits[n] = s[i] - 'a' + 10;
        } else {
            bits[n] = s[i] - '0';
        }
        if (s[i + 1] >= 'A' && s[i + 1] <= 'F') {
            bits[n] = (bits[n] << 4) | (s[i + 1] - 'A' + 10);
        } else if (s[i + 1] >= 'a' && s[i + 1] <= 'f') {
            bits[n] = (bits[n] << 4) | (s[i + 1] - 'a' + 10);
        } else {
            bits[n] = (bits[n] << 4) | (s[i + 1] - '0');
        }
        ++n;
    }

    return n;
}

static uint32_t hexstring_to_integer(char *s, uint32_t bits)
{
    uint32_t i, j, tmp, sum;

    sum = 0;
    for (i = 0; i < bits; i++) {
        if ((s[i] < '0') || (s[i] > '9')) {
            return 0;
        }
        tmp = s[i] - '0';
        for (j = 0; j < bits - i - 1; j++) {
            tmp *= 10;
        }
        sum += tmp;
    }

    return sum;
}

/* user should define this function in it's project normally. */
__weak void user_data_item_reset_to_default(char *group_name)
{
    group_name = group_name;
}

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_nvdm(atci_parse_cmd_param_t *parse_cmd);

/* AT command handler */
atci_status_t atci_cmd_hdlr_nvdm(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};
    char *group_name = NULL;
    char *data_item_name = NULL;
    char *parameter;
    char *saveptr = NULL;
    uint32_t length;
    uint8_t buffer[256];
    nvdm_status_t status;

    LOG_I(common, "atci_cmd_hdlr_nvdm \r\n");

    resonse.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            strcpy((char *)(resonse.response_buf), AT_COMMAND_NVDM_HELP);
            resonse.response_len = strlen((char *)(resonse.response_buf));
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+ENVDM=<op>  the handler need to parse the parameters
            if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDM=0") != NULL) {
                /* read data item from NVDM */
                group_name = strtok_r(parse_cmd->string_ptr, ",", &saveptr);
                group_name = strtok_r(NULL, ",", &saveptr);
                if (group_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                data_item_name = strtok_r(NULL, ",", &saveptr);
                if (data_item_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                parameter = strtok_r(NULL, ",", &saveptr);
                if (parameter != NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                length = strlen(data_item_name);
                /* remove \0d\0a */
                data_item_name[length - 2] = 0;
                data_item_name[length - 1] = 0;
                LOG_I(common, "group_name = %s", group_name);
                LOG_I(common, "data_item_name = %s", data_item_name);
                length = 1024;
                status = nvdm_read_data_item(group_name,
                                             data_item_name,
                                             resonse.response_buf,
                                             &length);
                if (status != NVDM_STATUS_OK) {
                    LOG_I(common, "nvdm_read_data_item status = %d", status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "nvdm_read_data_item length = %d", length);
                /* append \0d\0a */
                resonse.response_buf[length] = '\r';
                resonse.response_buf[length + 1] = '\n';
                resonse.response_len = length + 2;
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDM=1") != NULL) {
                /* write data item into NVDM */
                group_name = strtok_r(parse_cmd->string_ptr, ",", &saveptr);
                /* get group name */
                group_name = strtok_r(NULL, ",", &saveptr);
                if (group_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "group_name = %s", group_name);
                /* get data item name */
                data_item_name = strtok_r(NULL, ",", &saveptr);
                if (data_item_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "data_item_name = %s", data_item_name);
                /* get length of data item */
                parameter = strtok_r(NULL, ",", &saveptr);
                if (parameter == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                length = hexstring_to_integer(parameter, strlen(parameter));
                LOG_I(common, "length = %d", length);
                /* get content of data item */
                parameter = strtok_r(NULL, ",", &saveptr);
                if (parameter == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                /* remove \0d\0a */
                resonse.response_len = strlen(parameter);
                parameter[resonse.response_len - 2] = 0;
                parameter[resonse.response_len - 1] = 0;
                resonse.response_len = hexstring_to_bytearray(parameter, buffer);
                LOG_I(common, "parse length = %d", resonse.response_len);
                if (length != resonse.response_len) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                {
                    uint32_t i;

                    LOG_I(common, "data = ");
                    for (i = 0; i < length; i++) {
                        LOG_I(common, "0x%x", buffer[i]);
                    }
                }
                status = nvdm_write_data_item(group_name,
                                              data_item_name,
                                              NVDM_DATA_ITEM_TYPE_STRING,
                                              buffer,
                                              resonse.response_len);
                if (status != NVDM_STATUS_OK) {
                    LOG_I(common, "nvdm_write_data_item status = %d", status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDM=2") != NULL) {
                /* reset data items to default value by group or all */
                group_name = strtok_r(parse_cmd->string_ptr, ",", &saveptr);
                /* get group name */
                group_name = strtok_r(NULL, ",", &saveptr);
                if (group_name != NULL) {
                    LOG_I(common, "group_name = %s", group_name);
                }
                user_data_item_reset_to_default(group_name);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                /* invalid AT command */
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            break;
        default :
            // others are invalid command format
            strcpy((char *)(resonse.response_buf), "ERROR\r\n");
            resonse.response_len = strlen((char *)(resonse.response_buf));
            break;
    }

    atci_send_response(&resonse);

    return ATCI_STATUS_OK;
}

