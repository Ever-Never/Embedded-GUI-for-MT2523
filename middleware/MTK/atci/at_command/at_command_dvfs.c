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

#include "hal_dvfs.h"

#ifdef HAL_DVFS_MODULE_ENABLED

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "at_command.h"

#include "hal_dvfs_internal.h"

#define LOGE(fmt,arg...)   LOG_E(atcmd, "[dvfs]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atcmd, "[dvfs]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atcmd ,"[dvfs]"fmt,##arg)

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_dvfs_get(atci_parse_cmd_param_t *parse_cmd);
atci_status_t atci_cmd_hdlr_dvfs_set(atci_parse_cmd_param_t *parse_cmd);
atci_status_t atci_cmd_hdlr_dvfs_dbg(atci_parse_cmd_param_t *parse_cmd);

// AT command handler
atci_status_t atci_cmd_hdlr_dvfs_get(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response;
    char *param = NULL, tmp[32];
    int32_t op;
    unsigned int freq, list_num, idx;
    const unsigned int *cpufreq_list;

    memset(&response, 0, sizeof(response));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            /* AT+CPUFGET=? */
            strcpy((char *)response.response_buf, "+CPUFGET=0\r\nOK\r\n");
            break;
        case ATCI_CMD_MODE_READ:
            /* AT+CPUFGET? */
            freq = (unsigned int)hal_dvfs_get_cpu_frequency();
            snprintf((char *)response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "+CPUFGET:%u\r\n", freq);
            break;

        case ATCI_CMD_MODE_EXECUTION:
            /* AT+CPUFGET=<op> the handler need to parse the parameters */
            param = strtok(parse_cmd->string_ptr, ",\n\r");
            param = strtok(parse_cmd->string_ptr, "AT+CPUFGET=");
            if (param != NULL) {
                op = atoi(param);
                if (op == 0) {
                    hal_dvfs_get_cpu_frequency_list((const uint32_t **)&cpufreq_list, (uint32_t *)&list_num);
                    for (idx = 0; idx < list_num; idx++) {
                        if (idx == list_num - 1) {
                            snprintf(tmp, sizeof(tmp), "%d\r\n", cpufreq_list[idx]);
                        } else {
                            snprintf(tmp, sizeof(tmp), "%d,", cpufreq_list[idx]);
                        }
                        strcat((char *)response.response_buf, tmp);
                    }
                }
                break;
            }

            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            strcpy((char *)response.response_buf, "E: op should be 0\r\n");
            break;

        default :
            /* others are invalid command format */
            strcpy((char *)response.response_buf, "ERROR\r\n");
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);

    return ATCI_STATUS_OK;
}

atci_status_t atci_cmd_hdlr_dvfs_set(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response;
    char *param = NULL, *param1 = NULL;
    unsigned int freq, relation;
    hal_dvfs_status_t status;

    memset(&response, 0, sizeof(response));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            /* AT+CPUFSET=? */
            strcpy((char *)response.response_buf, "+CPUFSET=(<freq_in_kHz,(0,1)>)\r\nOK\r\n");
            break;

        case ATCI_CMD_MODE_EXECUTION:
            /* AT+CPUFSET=<freq,relation> the handler need to parse the parameters */
            param = strtok(parse_cmd->string_ptr, "=");
            param = strtok(NULL, ",");
            param1 = strtok(NULL, ",");
            if (!param || !param1) {
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                strcpy((char *)response.response_buf, "E: invalid parameters\r\n");
            } else {
                freq = atoi(param);
                relation = atoi(param1);
                status = hal_dvfs_target_cpu_frequency((uint32_t)freq, (hal_dvfs_freq_relation_t)relation);
                freq = (unsigned int)hal_dvfs_get_cpu_frequency();
                snprintf((char *)response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "status=%d, cur_freq=%u\r\n", status, freq);
            }
            break;

        default :
            /* others are invalid command format */
            strcpy((char *)response.response_buf, "ERROR\r\n");
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);

    return ATCI_STATUS_OK;
}

atci_status_t atci_cmd_hdlr_dvfs_dbg(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response;
    char *param = NULL;
    int32_t op;

    memset(&response, 0, sizeof(response));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            /* AT+DVFSDBG=? */
            strcpy((char *)response.response_buf, "+DVFSDBG=0\r\nOK\r\n");
            break;

        case ATCI_CMD_MODE_EXECUTION:
            /* AT+DVFSDBG=<op> the handler need to parse the parameters */
            param = strtok(parse_cmd->string_ptr, "=");
            param = strtok(NULL, ",");
            if (!param) {
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                strcpy((char *)response.response_buf, "E: invalid parameters\r\n");
            } else {
                op = atoi(param);
                if (op == 0) {
                    dvfs_debug_dump();
                    snprintf((char *)response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "debug dump\r\n");
                }
            }
            break;

        default :
            /* others are invalid command format */
            strcpy((char *)response.response_buf, "ERROR\r\n");
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);

    return ATCI_STATUS_OK;
}


#endif /* HAL_DVFS_MODULE_ENABLED */

