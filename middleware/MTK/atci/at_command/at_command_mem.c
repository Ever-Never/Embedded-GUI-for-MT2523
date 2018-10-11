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
// System head file

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "at_command.h"
#include "atci_main.h"
#include "exception_handler.h"
#include "toi.h"

extern memory_region_type memory_regions[];

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_mem(atci_parse_cmd_param_t *parse_cmd);

/*

AT+MEM=?
+MEM:
AT+MEM? Query for the memory regions in the system
AT+MEM=dump,<start_address>,<length> Dump <length> bytes since <start_address>
<start_address>  := hex or decimal, 4 bytes aligned.
<length>         := hex or decimal, a multiple of 4, max = 8448(0x2100).
OK

AT+MEM?
+MEM: list of (<region_name>,<start_address>,<end_address>)s
OK

AT+MEM=dump,<start_address>,<length>
+MEM:
start_address    : word1 word2 word3 word4
start_address+16 : word5 word6 word7 word8
...

<length> bytes dumped.

OK

If <start_address>..<start_address>+<length> is invalid
with respect with the system defined memory region table,
return ERROR.

Example:
AT+MEM=dump,0x0,64
+MEM:
0x00000000: 04028000 08010001 080231f5 00000141
0x00000010: 0000018b 000001d5 0000021f 00000000
0x00000020: 00000000 00000000 00000000 0000026d
0x00000030: 0802364b 00000000 0000027f 00000291

64 bytes dumped.

OK
*/

static unsigned int get_max_dump_size(void)
{
    /* Two entries for +MEM: and OK, reserve some buffer (2).
     * About 50 bytes text to present 16 bytes data.
     */
    return ((ATCI_LOCAL_QUEUE_LENGTH - 4) * (ATCI_UART_TX_FIFO_BUFFER_SIZE * 16 / 50)) & (~0 << 8);
}

static void mem_show_usage(uint8_t *buf)
{
    int pos = 0;

    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "%s",
                    "+MEM:\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "%-40s %s\r\n",
                    "AT+MEM?",
                    "Query for the memory regions in the system");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "%-40s %s\r\n",
                    "AT+MEM=dump,<start_address>,<length>",
                    "Dump <length> bytes since <start_address>");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "    <start_address> := hex or decimal, 4 bytes aligned.\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "    <length>        := hex or decimal, a multiple of 4, max = %d(0x%x)\r\n",
                    get_max_dump_size(),
                    get_max_dump_size());
}

static void mem_show_memory_regions(uint8_t *buf)
{
    int i, pos;

    pos = 0;
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "+MEM:\r\n");

    for (i=0; ; i++) {
        if (!memory_regions[i].region_name) {
            break;
        }
        if (!memory_regions[i].is_dumped) {
            continue;
        }
        pos += snprintf((char *)(buf + pos),
                        ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                        "%-16s: %08x -- %08x\r\n",
                        memory_regions[i].region_name,
                        (unsigned int)memory_regions[i].start_address,
                        (unsigned int)memory_regions[i].end_address);
    }
}

static int mem_dump_memory(atci_response_t *p_atci_rsp, unsigned int start_address, unsigned int length)
{
    int i, pos;
    unsigned int mem_dump_size;
    unsigned int *current, *end;
    uint8_t *buf;
    atci_response_t *p_urc;

    buf = p_atci_rsp->response_buf;
    pos = 0;
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "+MEM:\r\n");

    if (start_address % 4 != 0) {
        pos += snprintf((char *)(buf + pos),
                        ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                        "<start_address>=%08x is not 4 bytes aligned\r\n",
                        start_address);
        return -1;
    }

    mem_dump_size = length;

    /* sanity check on start_address */
    for (i=0; ; i++) {
        if (!memory_regions[i].region_name) {
            pos += snprintf((char *)(buf + pos),
                            ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                            "Invalid <start_address>=%08x\r\n",
                            start_address);
            return -2;
        }
        if (!memory_regions[i].is_dumped) {
            continue;
        }
        if (start_address >= (unsigned int)memory_regions[i].start_address &&
            start_address <  (unsigned int)memory_regions[i].end_address) {
            if (start_address + length > (unsigned int)memory_regions[i].end_address) {
                mem_dump_size = (unsigned int)memory_regions[i].end_address - start_address;
            }
            break;
        }
    }

    if (mem_dump_size > get_max_dump_size()) {
        mem_dump_size = get_max_dump_size();
    }

    mem_dump_size -= mem_dump_size % 4;

    if (mem_dump_size == 0) {
        pos += snprintf((char *)(buf + pos),
                        ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                        "Dump length = 0\r\n");
        return -3;
    }

    /* +MEM: */
    p_urc = p_atci_rsp;
    p_urc->response_len  = strlen((char *)buf);
    p_urc->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
    atci_send_response(p_urc);

    pos = 0;
    current = (unsigned int *)start_address;
    end     = (unsigned int *)(start_address + mem_dump_size);
    for (; current+3 < end; current += 4 ) {
        pos += snprintf((char *)(buf + pos),
                        ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                        "0x%08x: %08x %08x %08x %08x\r\n",
                        (unsigned int)current,
                        *(current + 0),
                        *(current + 1),
                        *(current + 2),
                        *(current + 3));

        if (pos + 64 > ATCI_UART_TX_FIFO_BUFFER_SIZE) {
            p_urc->response_len  = strlen((char *)buf);
            p_urc->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
            atci_send_response(p_urc);
            pos = 0;
        }
    }

    /* print the last words if any */
    if (current < end) {
        pos += snprintf((char *)(buf + pos),
                        ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                        "0x%08x:",
                        (unsigned int)current);
        for (; current < end; current++) {
            pos += snprintf((char *)(buf + pos),
                            ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                            " %08x",
                            *current);
        }
        pos += snprintf((char *)(buf + pos),
                        ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                        "\r\n");
    }
    p_urc->response_len  = strlen((char *)buf);
    p_urc->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
    atci_send_response(p_urc);

    snprintf((char *)buf,
             ATCI_UART_TX_FIFO_BUFFER_SIZE,
             "\r\n%d bytes dumped\r\n\r\n",
             mem_dump_size);

    return 0;
}

static int parse_mem_execute_command(char *cmd, char **pp1, unsigned int *pp2, unsigned int *pp3)
{
    uint8_t type;
    uint32_t value;
    char *pch, *p2, *p3;

    pch = cmd;
    *pp1 = pch;

    pch = strchr(pch, ',');

    if (pch != NULL) {
        *pch = 0;
        pch++;
        p2 = pch;
    } else {
        return -1;
    }

    pch = strchr(pch, ',');

    if (pch != NULL) {
        *pch = 0;
        pch++;
        p3 = pch;
    } else {
        return -1;
    }

    while (*pch && (*pch != '\n') && (*pch != '\r')) {
        pch++;
    }

    *pch = 0;

    value = toi(p2, &type);
    if ( !(type == TOI_HEX || type == TOI_DEC ) ) {
        return -2;
    }
    *pp2 = value;

    value = toi(p3, &type);
    if ( !(type == TOI_HEX || type == TOI_DEC ) ) {
        return -3;
    }
    *pp3 = value;

    return 0;
}

/* AT command handler  */
atci_status_t atci_cmd_hdlr_mem(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    char *p1;
    unsigned int p2, p3;
    int ret;

    response.response_flag = 0; /* Command Execute Finish. */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING: /* rec: AT+SYSLOG=? */
            mem_show_usage(response.response_buf);
            response.response_len = strlen((char *)response.response_buf);
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_READ:    /* rec: AT+SYSLOG? */
            mem_show_memory_regions(response.response_buf);
            response.response_len = strlen((char *)response.response_buf);
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+MEM=dump,<start_address>,<length> */
            p2 = 0;
            p3 = 0;
            ret = parse_mem_execute_command(parse_cmd->string_ptr + 7, &p1, &p2, &p3);
            if (ret == 0) {
                if (strcmp(p1, "dump") == 0 && mem_dump_memory(&response, p2, p3) == 0) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else {
                if (ret == -1) {
                    strcpy((char *)response.response_buf, "command syntax error\r\n");
                } else {
                    snprintf((char *)response.response_buf,
                             ATCI_UART_TX_FIFO_BUFFER_SIZE,
                             "<%s> error\r\n",
                             (ret == -2) ? "start_address" : "length");
                }
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
