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

#include "hal_gpio.h"
#include <stdlib.h>

#ifdef MTK_BUILD_SMT_LOAD
#ifdef HAL_GPIO_MODULE_ENABLED
/*
 * sample code
*/


/*--- Function ---*/
atci_status_t atci_cmd_hdlr_led(atci_parse_cmd_param_t *parse_cmd);

/*
AT+ELED=<op>	            |   "OK"
AT+ELED=?                   |   "+ELED=(0,1)","OK"


*/
/* AT command handler  */
atci_status_t atci_cmd_hdlr_led(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};
    char *param = NULL;
    char param_val;

    printf("atci_cmd_hdlr_led \r\n");

    resonse.response_flag = 0; /*    Command Execute Finish.  */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    /* rec: AT+ELED=?   */
            strcpy((char*)resonse.response_buf, "+ELED=(\"1: R LED turn off\",\"2: G LED turn off\",\"3: B LED turn off\",\"4: R LED turn on\",\"5: G LED turn on\",\"2: B LED turn on\")\r\nOK\r\n");
            resonse.response_len = strlen((char*)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+ELED=<op>  the handler need to parse the parameters  */
            param = strtok(parse_cmd->string_ptr, ",\n\r");
            param = strtok(parse_cmd->string_ptr, "AT+ELED=");
            param_val = atoi(param);


            if ((1 != param_val) && (2 != param_val) && (3 != param_val) && (4 != param_val) && (5 != param_val) && (6 != param_val)) {
                strcpy((char*)resonse.response_buf, "ERROR\r\n");
                resonse.response_len = strlen((char*)resonse.response_buf);
                atci_send_response(&resonse);
                break;
            }

            strcpy((char*)resonse.response_buf, "OK\r\n");
            resonse.response_len = strlen((char*)resonse.response_buf);
            atci_send_response(&resonse);

            /*   set work mode as GPIO    */
            hal_pinmux_set_function(HAL_GPIO_4, HAL_GPIO_4_GPIO4);
            hal_pinmux_set_function(HAL_GPIO_5, HAL_GPIO_5_GPIO5);
            hal_pinmux_set_function(HAL_GPIO_6, HAL_GPIO_6_GPIO6);
            /*   set dierection to be output  */
            hal_gpio_set_direction(HAL_GPIO_4, HAL_GPIO_DIRECTION_OUTPUT);
            hal_gpio_set_direction(HAL_GPIO_5, HAL_GPIO_DIRECTION_OUTPUT);
            hal_gpio_set_direction(HAL_GPIO_6, HAL_GPIO_DIRECTION_OUTPUT);

            if (1 == param_val) {
                hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_LOW);
            } else if(2 == param_val) {
                hal_gpio_set_output(HAL_GPIO_5, HAL_GPIO_DATA_LOW);
            } else if(3 == param_val) {
                hal_gpio_set_output(HAL_GPIO_6, HAL_GPIO_DATA_LOW);
            } else if(4 == param_val) {
                hal_gpio_set_output(HAL_GPIO_4, HAL_GPIO_DATA_HIGH);
            } else if(5 == param_val) {
                hal_gpio_set_output(HAL_GPIO_5, HAL_GPIO_DATA_HIGH);
            } else if(6 == param_val) {
                hal_gpio_set_output(HAL_GPIO_6, HAL_GPIO_DATA_HIGH);
            };
            break;

        default :
            /* others are invalid command format */
            strcpy((char*)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char*)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}

#endif
#endif


