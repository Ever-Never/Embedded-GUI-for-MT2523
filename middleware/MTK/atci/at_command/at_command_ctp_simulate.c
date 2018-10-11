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

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#include <stdlib.h>
#include "hal_log.h"

#ifdef MTK_CTP_ENABLE
#include "ctp.h"
#include "bsp_ctp.h"


/* CTP simulate cmd: AT+ECTP=(<X1>,<Y1>):<action_index>;(<X1>,<Y1>):<action_index>, for example:
 *
 *	 AT+ECTP=(20,30):1;(100,500):1    //press point of (20,30) and (100,500)
 *	 AT+ECTP=(20,30):0                //release point of (20,30)
 *
 */

/*--- Function ---*/
atci_status_t  atci_cmd_hdlr_ctp_simulate(atci_parse_cmd_param_t *parse_cmd);

extern bool ctp_calibrate_coordinate(bsp_ctp_multiple_event_t *tp_event);
extern void touch_panel_multiple_touch_data_push(bsp_ctp_multiple_event_t *push_event);

/* AT command handler  */
atci_status_t atci_cmd_hdlr_ctp_simulate(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};
    char *param1 = NULL;
    char *param2 = NULL;
    char *param3 = NULL;
    char *param4 = NULL;
    char *param5 = NULL;
    char *param6 = NULL;
    char *param_temp = NULL;
    //char *param7 = NULL;
    bsp_ctp_multiple_event_t push_event;
    int i;
    int pointer_count = 1;
    int status_flag = 0;

    resonse.response_flag = 0;      /*  Command Execute Finish.  */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            strcpy((char *)resonse.response_buf, "AT+ECTP=(<X1>,<Y1>):<action_index>;(<X1>,<Y1>):<action_index>\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION:
            if (ctp_config_info.is_external_initialized != false) {
                strcpy((char *)resonse.response_buf, "OK\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);

                param1 = strtok(parse_cmd->string_ptr, ";");
                param2 = strtok(NULL, ";");
                if ('(' == param2[0]) {              /* parse second point. */
                    param3 = strtok(param2, ":");
                    param4 = strtok(NULL, ":");
                    push_event.points[1].event = (bsp_ctp_event_status_t)(atoi(param4));  /* pointer 2 status */
                    param5 = strtok(param3, ",");
                    param6 = strtok(NULL, ",");

                    param_temp = ++param5;
                    push_event.points[1].x = atoi(param_temp);
                    push_event.points[1].y = atoi(strtok(param6, ")"));
                    pointer_count = 2;
                }

                param3 = strtok(param1, ":");
                param4 = strtok(NULL, ":");
                push_event.points[0].event = (bsp_ctp_event_status_t)(atoi(param4));  /* pointer 1 status. */
                param5 = strtok(param3, ",");
                param6 = strtok(NULL, ",");
                param3 = strtok(param5, "(");
                param_temp = strtok(NULL, "(");
                push_event.points[0].x = atoi(param_temp);
                push_event.points[0].y = atoi(strtok(param6, ")"));

                ctp_mask_eint_interrupt();
                push_event.time_stamp = ctp_get_time_us();
                push_event.model = pointer_count;
                push_event.padding = CTP_PATTERN;
                for (i = 0; i < pointer_count; i++) { /* check if all the pointers status is released. */
                    if (push_event.points[i].event != CTP_PEN_UP) {
                        ctp_context.state = CTP_DOWN;
                        status_flag = 1;
                        break;
                    }
                }
                if (!status_flag) { /* all the points is released. */
                    if (ctp_context.state == CTP_DOWN) {
                        ctp_context.state = CTP_UP;
                    } else {
                        strcpy((char *)resonse.response_buf, "UP to UP, BYPASS\r\n");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        atci_send_response(&resonse);
                        return ATCI_STATUS_OK;
                    }
                }
                ctp_calibrate_coordinate(&push_event);
                touch_panel_multiple_touch_data_push(&push_event);
                ctp_unmask_eint_interrupt();
            } else {
                strcpy((char *)resonse.response_buf, "Error:CTP not initialized!\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
                return ATCI_STATUS_OK;
            }
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


