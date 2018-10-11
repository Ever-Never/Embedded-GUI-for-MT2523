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

/* Includes ------------------------------------------------------------------*/
// For Register AT command handler
#include "atci.h"
// System head file
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "syslog.h"

#define LOGE(fmt,arg...)   LOG_E(atcmd, "ATCI project: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atcmd, "ATCI project: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atcmd, "ATCI project: "fmt,##arg)


/*--- Function ---*/
static atci_status_t atci_cmd_hdlr_testcmd(atci_parse_cmd_param_t *parse_cmd);


/**
 * @brief The AT command register table.
 */

atci_cmd_hdlr_item_t item_table[] = {
    {"AT+TESTCMD",         atci_cmd_hdlr_testcmd,         0, 0}
    };


/**
 * @brief This function is a function to register the AT command to ATCI module.
 * @param    None.
 * @return    None.
 */
void atci_example_init()
{
    atci_status_t ret = ATCI_STATUS_REGISTRATION_FAILURE;

    LOGW("atci_example_init\r\n");
        
    // --------------------------------------------------------- //
    // ------- Test Scenario: register AT handler in CM4 ------- //
    // --------------------------------------------------------- //
    ret = atci_register_handler(item_table, sizeof(item_table) / sizeof(atci_cmd_hdlr_item_t));
    if (ret == ATCI_STATUS_OK) {
		LOGW("at_example_init register success\r\n");
	} else {
		LOGW("at_example_init register fail\r\n");
	}

}

/**
 * @brief This function is a AT command handler for  processing the data sending by ATCI module.
 * @param[in] parse_cmd is the AT command data and related inforation, such as cmd mode.
 * @return    AT command handler status.
 */
static atci_status_t atci_cmd_hdlr_testcmd(atci_parse_cmd_param_t *parse_cmd)
{
    static int test_param1 = 0;
    atci_response_t response = {{0}};
    char *param = NULL;
    int  param1_val = -1;

    LOGW("atci_cmd_hdlr_testcmd\n");
    
    response.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+TESTCMD=?
            LOGW("AT Test OK.\n");
            strcpy((char *)response.response_buf, "+TESTCMD:(0,1)\r\nOK\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+TESTCMD?
            LOGW("AT Read done.\n");
            sprintf((char *)response.response_buf,"+TESTCMD:%d\r\n", test_param1);
            response.response_len = strlen((char *)response.response_buf);
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&response);
            break;
            
        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+TESTCMD
            LOGW("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)response.response_buf, "+TESTCMD:HELLO.OK\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
            
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+TESTCMD=<p1>  the handler need to parse the parameters
            LOGW("AT Executing...\r\n");
            //parsing the parameter
            param = strtok(parse_cmd->string_ptr, ",\n\r");
	        param = strtok(parse_cmd->string_ptr, "AT+TESTCMD=");
            param1_val = atoi(param);
		
	    if (param != NULL && (param1_val == 0 || param1_val == 1)){
                
                // valid parameter, update the data and return "OK"
                response.response_len = 0;
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer 
            } else {
                // invalide parameter, return "ERROR"
                response.response_len = 0;
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
            };
            atci_send_response(&response);
			param = NULL;
            break;
            
        default :
            strcpy((char *)response.response_buf, "ERROR\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

