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

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "at_command.h"
#include "syslog.h"

#ifdef HAL_CHARGER_MODULE_ENABLED
#include "hal_charger.h"

#ifdef MTK_BUILD_SMT_LOAD
#include "cust_charging.h"
#endif

log_create_module(atci_charger, PRINT_LEVEL_INFO);

/* ===================================================*/
/* define */
/* ===================================================*/

/* ===================================================*/
/* function */
/* ===================================================*/

/* ===================================================*/
/* global variable */
/* ===================================================*/

/* ===================================================*/
// AT command handler
atci_status_t atci_cmd_hdlr_charger(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}};
    bool cable_in = false;
    int32_t batt_volt = 0;
    int32_t batt_temp = 0;
    int32_t char_curr = 0;
    int32_t char_volt = 0;

    LOG_I(atci_charger, "[atci_charger]start\r\n");

#ifndef MTK_BUILD_SMT_LOAD
    hal_charger_init();
    hal_charger_get_charger_detect_status(&cable_in);
    hal_charger_meter_get_charging_current(&char_curr);
#else
    hal_charger_init();
    hal_charger_get_charger_detect_status(&cable_in);
    if (cable_in) {
        /* Default pre-charging current. It depends on the battery specifications.
            The USBDL mode current level  = 16.8mV / R_SESNE */
        hal_charger_set_charging_current(16800 / CUST_R_SENSE);

        hal_charger_enable(true);
        LOG_I(atci_charger, "hal_charger_enable delay start\r\n");

        vTaskDelay(1000 / portTICK_RATE_MS);

        hal_charger_meter_get_charging_current(&char_curr);
        LOG_I(atci_charger, "hal_charger_enable delay end\r\n");
        hal_charger_enable(false);
    }
#endif

    hal_charger_meter_get_battery_voltage_sense(&batt_volt);
    hal_charger_meter_get_battery_temperature(&batt_temp);
    hal_charger_meter_get_charger_voltage(&char_volt);

    response.response_flag = 0;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            /* AT+ECHAR=? */
            strcpy((char *)response.response_buf, "+ECHAR=(0,1)\r\nOK\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_READ:
            /* AT+ECHAR? */
            snprintf((char *)response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "+ECHAR: %d\r\n", (int)cable_in);
            response.response_len = strlen((char *)response.response_buf);
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_ACTIVE:
            /* AT+ECHAR */
            snprintf((char *)response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "+ECHAR:%d,%d,%d,%d\r\n", (int)batt_volt, (int)batt_temp, (int)char_curr, (int)char_volt);
            response.response_len = strlen((char *)response.response_buf);
            response.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(&response);
            break;

        case ATCI_CMD_MODE_EXECUTION:
        /* AT+ECHAR=... */

        default :
            // others are invalid command format
            LOG_I(atci_charger, "[atci_charger] mode=%d\r\n", parse_cmd->mode);
            strcpy((char *)response.response_buf, "ERROR\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }

    return ATCI_STATUS_OK;
}
#endif /* HAL_CHARGER_MODULE_ENABLED */

