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

#include "verno.h"
#if defined(MTK_FW_VERSION) && ((PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) || (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 5932))
// System head file
#include "FreeRTOS.h"
#include "task.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// For Register AT command handler
#include "at_command.h"

#include "syslog.h"
#include "connsys_util.h"
#include "hal_wdt.h"

#define LOGE(fmt,arg...)   LOG_E(atcmd, "ATCMD: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atcmd, "ATCMD: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atcmd ,"ATCMD: "fmt,##arg)

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_sys(atci_parse_cmd_param_t *parse_cmd);

// AT command handler
atci_status_t atci_cmd_hdlr_sys(atci_parse_cmd_param_t *parse_cmd)
{    
    atci_response_t response = {{0}};
    char fw_ver[32];
    char patch_ver[32];
    hal_wdt_config_t wdt_config;

    memset(fw_ver, 0, 32);
    memset(patch_ver, 0, 32);

    LOGI("atci_cmd_hdlr_sys\r\n");

    response.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+SYS=?
            LOGI("AT SYS test OK.\n");
            strcpy((char *)response.response_buf, "+SYS:(VER)\r\nOK\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+SYS?
            LOGI("AT SYS Read done.\n");
            snprintf((char *)response.response_buf, sizeof(response.response_buf), "+SYS:VER\r\n");
            response.response_len = strlen((char *)response.response_buf);
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+SYS
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)response.response_buf, "+SYS\r\nOK\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+SYS=<p1>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");

            if (strstr((char *)parse_cmd->string_ptr, "AT+SYS=VER") != NULL) {
                // valid parameter, update the data and return "OK"
                connsys_util_get_n9_fw_ver(fw_ver);
                connsys_util_get_ncp_patch_ver(patch_ver);
                //snprintf((char *)response.response_buf, sizeof(response.response_buf),
                //         "+SYS=VER:CM4 Image Ver: %s\r\nN9 Image Ver: %s\r\nHW Patch Ver: %s\r\nOK\r\n",
                //         MTK_FW_VERSION, fw_ver, patch_ver);
				snprintf((char *)response.response_buf, sizeof(response.response_buf),
                         "+SYS=VER:CM4 Image Ver: %s\r\nN9 Image Ver: %s\r\nOK\r\n", MTK_FW_VERSION, fw_ver);

                response.response_len = strlen((char *)response.response_buf);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+SYS=REBOOT") != NULL) {
                // valid parameter, update the data and return "OK"
#if defined(MTK_FW_VERSION) && (PRODUCT_VERSION != 7697)
                wdt_config.mode = HAL_WDT_MODE_RESET;
#else
                // MT7697 has WDT reset issue
                wdt_config.mode = HAL_WDT_MODE_INTERRUPT;               
#endif
                wdt_config.seconds = 1;
                hal_wdt_init(&wdt_config);
                hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
                snprintf((char *)response.response_buf, sizeof(response.response_buf),
                         "+SYS=REBOOT:OK\r\n");

                response.response_len = strlen((char *)response.response_buf);
            } else {
                // invalide parameter, return "ERROR"
                response.response_len = 0;
                response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of response buffer
            };

            atci_send_response(&response);
            break;
        default :
            strcpy((char *)response.response_buf, "+SYS\r\nERROR\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

#endif //  defined(MTK_QUERY_SDK_VERSION) && defined(MTK_FW_VERSION)
