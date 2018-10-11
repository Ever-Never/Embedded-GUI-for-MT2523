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

#ifdef MTK_CTP_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#include "bsp_ctp.h"
#include "syslog.h"
#include <stdlib.h>

log_create_module(atci_ctp, PRINT_LEVEL_INFO);

#define LOGE(fmt,arg...)   LOG_E(atci_ctp, "[CTP]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atci_ctp, "[CTP]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atci_ctp ,"[CTP]"fmt,##arg)

/*
 * sample code
*/

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_ctp(atci_parse_cmd_param_t *parse_cmd);

/*
+EPENURC=<op>   OK / ERROR

+EPEN: <pen position-x>,<pen position-y><pen state>

Field
<op>: integer
0   turn off pen URC information.
1   turn on pen URC information.

<pen position-x >: integer
<pen position-y >: integer
It is the pen position in x, y.

<pen state>: integer
0   pen release.
1   pen press.


Example
AT+EPENURC=1           // turn on touch panel URC information
OK

+PEN:10,10,1        // touch panel URC: pen (10,10) was pressed

+PEN:10,3,0         // touch panel URC: pen (10,3) was release

+PEN:80,55,1        // touch panel URC: pen (80,55) was pressed

+PEN:70,40,0        // touch panel URC: pen (70,40) was release

AT+EPENURC=0           // turn off touch panel URC information
OK
*/
// AT command handler


extern void ctp_get_buf_roomleft(uint16_t *pleft);

void atci_ctp_handler(void)
{
    atci_response_t resonse = {{0}};

    int  pen_datax  = 0;
    int  pen_datay  = 0;
    int  pen_state = 0;
    bsp_ctp_multiple_event_t touch_event;
    bsp_ctp_status_t         ret;
    uint32_t i;
    uint16_t left_size;
    int32_t ret_len = 0;

    char *str[8] = {"CTP_PEN_UP",
                    "CTP_PEN_DOWN",
                    "CTP_PEN_MOVE" ,
                    "CTP_PEN_LONGTAP",
                    "CTP_PEN_REPEAT",
                    "CTP_PEN_LONGTAP_HOLD",
                    "CTP_PEN_REPEAT_HOLD",
                    "CTP_PEN_ABORT"
                   };

    LOGW("[user]**enter ctp hanlder get event in at command\r\n");
    while (1) {
        ret = bsp_ctp_get_event_data(&touch_event);
        ctp_get_buf_roomleft(&left_size);

        if (ret == BSP_CTP_EVENT_EMPTY) {
            LOGW("[DATA] all event data has been get out\r\n");
            break;
        }

        if (touch_event.model > 5) {
            LOGW("[DATA] model =%d overflow\r\n", touch_event.model);
            break;
        }

        for (i = 0; i < touch_event.model; i++) {
            pen_state = touch_event.points[i].event;

            if (pen_state == CTP_PEN_ABORT) {
                bsp_ctp_flush_buffer();
                break;
            }
            pen_datax = touch_event.points[i].x;
            pen_datay = touch_event.points[i].y;
            ret_len = snprintf((char *)(&resonse.response_buf[resonse.response_len]), \
                               ATCI_UART_TX_FIFO_BUFFER_SIZE, \
                               "model:%d +PEN:%d,%d,left:%d %s\r\n", \
                               (int)i, (int)pen_datax, (int)pen_datay, (int)left_size, (char *)str[pen_state]);
            resonse.response_len += (uint16_t)ret_len;
        }

        resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&resonse);
        resonse.response_len = 0;


        if ((ret !=  BSP_CTP_EVENT_EMPTY) && (ret !=  BSP_CTP_OK)) {
            LOGW("[DATA] some error happend when get event\r\n");
        }
    }

    //vTaskDelay(xTicksToWait);

} /* CalTaskMain() */

atci_status_t atci_cmd_hdlr_ctp(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};
    char *param = NULL;
    int  param1_val = -1;
    bsp_ctp_status_t         ret = BSP_CTP_OK;
    LOGW("atci_cmd_hdlr_ctp \r\n");

    ret = ret;
    resonse.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+EPENURC=?
            strcpy((char *)resonse.response_buf, "+EPENURC=(0,1)\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+EPENURC=<op>  the handler need to parse the parameters
            LOGW("AT Executing...\r\n");
            param = strtok(parse_cmd->string_ptr, ",\n\r");
            param = strtok(parse_cmd->string_ptr, "AT+EPENURC=");
            param1_val = atoi(param);

            if (param != NULL && ((param1_val >= 0) && (param1_val <= 4))) {

                /* valid parameter, update the data and return "OK"
                                param1_val = 0 :  turn off ctp pen URC information.
                                param1_val = 1 :  turn on ctp pen URC information.
                                param1_val = 2 :  ctp sleep mode.
                                param1_val = 3 :  ctp active mode.
                                param1_val = 4 :  force upgrade firmware.
                */

                if (param1_val == 1) {
                    if (bsp_ctp_init() == BSP_CTP_OK) {
                        ret = bsp_ctp_register_callback((bsp_ctp_callback_t)atci_ctp_handler, NULL);

                        snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "CTP initilized OK...\r\n");
                        resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                        resonse.response_len = strlen((char *)resonse.response_buf);
                    } else {
                        snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "CTP has been initilized...\r\n");
                        resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        resonse.response_len = strlen((char *)resonse.response_buf);

                    }

                } else if (param1_val == 0) {
                    if (bsp_ctp_deinit() == BSP_CTP_OK) {
                        snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "CTP deinitilized OK...\r\n");
                        resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                        resonse.response_len = strlen((char *)resonse.response_buf);
                    } else {
                        snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "CTP deinitilized error...\r\n");
                        resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        resonse.response_len = strlen((char *)resonse.response_buf);
                    }
                } else if (param1_val == 2) {
                    bsp_ctp_power_on(false);
                    snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "CTP Sleep OK...\r\n");
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    resonse.response_len = strlen((char *)resonse.response_buf);

                } else if (param1_val == 3) {
                    bsp_ctp_power_on(true);
                    snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "CTP wake up OK....\r\n");
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    resonse.response_len = strlen((char *)resonse.response_buf);
                } else if (param1_val == 4) {
                    bsp_ctp_force_upgrade(true);
                    snprintf((char *)resonse.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "Set force upgrade firmware OK....\r\n");
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                    resonse.response_len = strlen((char *)resonse.response_buf);
                }

            } else {

                /*invalide parameter, return "ERROR"*/
                strcpy((char *)resonse.response_buf, "ERROR\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            /* others are invalid command format */
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}

#endif

