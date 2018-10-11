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

#include "bt_gap_le.h"
#include "bt_gap.h"
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "atci.h"
#include "syslog.h"

#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"
#include "bt_platform.h"

#include "bt_init.h"
#include "bt_sink_app_main.h"
#include "bt_sink_srv_am_task.h"
#include "bt_hfp_codec_internal.h"
#include "audio_middleware_api.h"

typedef struct _lp_tasks_list_ {
    TaskFunction_t      pvTaskCode;
    char                *pcName;
    uint16_t            usStackDepth;
    void                *pvParameters;
    UBaseType_t         uxPriority;
} lp_tasks_list_t;

extern void bsp_bt_codec_task_main(void *arg);
extern void am_task_main(void *arg);
extern void bt_sink_app_task_main(void *arg);
extern void bt_task(void * arg);

static bool g_bt_lp_switch = false;

static const lp_tasks_list_t bt_lp_list[] = {
    { bt_task,              BT_TASK_NAME,           BT_TASK_STACKSIZE/((uint32_t)sizeof( StackType_t )),     NULL,    BT_TASK_PRIORITY},
    { bsp_bt_codec_task_main,    BT_CODEC_TASK_NAME,     BT_CODEC_TASK_STACKSIZE/(( uint32_t )sizeof( StackType_t )),    NULL,   BT_CODEC_TASK_PRIO },
    { am_task_main,          AM_TASK_NAME,           AM_TASK_STACKSIZE/(( uint32_t )sizeof( StackType_t )),     NULL,   AM_TASK_PRIO },
    { bt_sink_app_task_main, BT_SINK_APP_TASK_NAME,  BT_SINK_APP_TASK_STACKSIZE/(( uint32_t )sizeof( StackType_t )),    NULL,   BT_SINK_APP_TASK_PRIO },
};

#define lp_list_count  (sizeof(bt_lp_list) / sizeof(lp_tasks_list_t))

static TaskHandle_t     lp_handles[lp_list_count];

void lp_task_create(void)
{
    uint16_t            i;
    BaseType_t          x;
    const lp_tasks_list_t  *t;

    for (i = 0; i < lp_list_count; i++) {
        t = &bt_lp_list[i];

        LOG_I(common, "xCreate task %s, pri %d", t->pcName, (int)t->uxPriority);

        x = xTaskCreate(t->pvTaskCode,
                        t->pcName,
                        t->usStackDepth,
                        t->pvParameters,
                        t->uxPriority,
                        &lp_handles[i]);

        if (x != pdPASS) {
            LOG_E(common, ": failed");
        } else {
            LOG_I(common, ": succeeded");
        }
    }
    LOG_I(common, "Free Heap size:%d bytes", xPortGetFreeHeapSize());
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
extern atci_status_t atci_cmd_hdlr_sleep_manager(atci_parse_cmd_param_t *parse_cmd);
#endif

static void bt_common_convert_str_hex(const char *str, char *output, uint8_t len) 
{
    int32_t i = 0;
    char tempbuf[2];
    while(len) {
        memcpy(tempbuf, (str + (i*2)), 2);
        output[i] = strtol(tempbuf, NULL, 16);
        len = len -2 ;
        i++;
    }
}

static bool bt_atci_set_advertise(char *string)
{
    uint32_t adv_type;
    uint32_t min_interval;
    uint32_t max_interval;
    uint32_t own_addr_type;
    uint32_t is_enable;
    uint8_t buff[32]={0};
    bt_hci_cmd_le_set_advertising_data_t adv_data = {0};
    char string_buff[31*2];
    uint8_t len;
    if (sscanf(string, "%u%*c%u%*c%u%*c%u%*c%u,%62s", (unsigned int *)&is_enable, (unsigned int *)&min_interval, (unsigned int *)&max_interval, (unsigned int *)&adv_type, (unsigned int *)&own_addr_type, (char *)string_buff) > 0) {
        LOG_I(common, "enable: %d, adv type %d, min_interval %d, max_interval  %d, onw_addr_type %d", is_enable, adv_type, min_interval, max_interval, own_addr_type);
        bt_hci_cmd_le_set_advertising_enable_t adv_enable;
        bt_hci_cmd_le_set_advertising_parameters_t adv_para = {
            .advertising_interval_min = min_interval,
            .advertising_interval_max = max_interval,
            .advertising_type = adv_type,
            .own_address_type         = own_addr_type,
            .advertising_channel_map = 7,
            .advertising_filter_policy = 0
        };

        adv_enable.advertising_enable = is_enable;

        len = strlen(string_buff);
        LOG_I(common, "adv data %s, len %d", string_buff, len);
        bt_common_convert_str_hex(string_buff, (char *)buff, len);
        adv_data.advertising_data_length = len/2;
        memcpy(adv_data.advertising_data, buff, len/2);
        bt_gap_le_set_advertising(&adv_enable, &adv_para, &adv_data, NULL);

        return true;
    } else {
        return false;
    }
}
static bool bt_atci_parse_cmd(char *string)
{
    uint16_t addr_5,addr_4,addr_3,addr_2,addr_1,addr_0;
    bool result = true;

    if (strstr(string, "AT+EBTTEST_DEMO=") != 0) {
        /* Create tasks for BT low power, and power on BT. */
        LOG_I(common, "enter low power");
        if (g_bt_lp_switch == false) {
            g_bt_lp_switch = true;
            lp_task_create();
            bt_demo_power_on(BT_MODE_AUDIO);
        }
        return result;
    }

    /* Reture directly if not enter BT low power, avoiding exception. */
    if (g_bt_lp_switch == false) {
        LOG_W(common, "[BT_LP] Please enter low power mode firstly");
        return result;
    }

    if (strstr(string, "AT+EBLESA_DEMO=1") != 0) {
        /* start advertising */
        bt_hci_cmd_le_set_advertising_enable_t adv_enable;
        bt_hci_cmd_le_set_advertising_parameters_t adv_para = {
            .advertising_interval_min = 0x0800,
            .advertising_interval_max = 0x0800,
            .advertising_type = BT_HCI_ADV_TYPE_CONNECTABLE_UNDIRECTED,
            .advertising_channel_map = 7,
            .advertising_filter_policy = 0
        };

        adv_enable.advertising_enable = BT_HCI_ENABLE;
        bt_gap_le_set_advertising(&adv_enable, &adv_para, NULL, NULL);
    } else if (strstr(string, "AT+EBLESA_DEMO=0") != 0) {
        /* stop advertising*/
        bt_hci_cmd_le_set_advertising_enable_t enable;
        enable.advertising_enable = BT_HCI_DISABLE;
        bt_gap_le_set_advertising(&enable, NULL, NULL, NULL);
    } else if (strstr(string, "AT+EBLESEA_DEMO=") != 0) {        // ble adv 
         /*AT+EBLESAP 11 */
        result = bt_atci_set_advertise(string+strlen("AT+EBLESEA_DEMO="));

    } else if (strstr(string, "AT+EBLESAD_DEMO=") != 0) {
    /* avoid memory address's end is 1 */
        //uint32_t addr_5,addr_4,addr_3,addr_2,addr_1,addr_0;
        bt_bd_addr_t addr;
        if (sscanf(string+strlen("AT+EBLESAD_DEMO="), "%2x%*c%2x%*c%2x%*c%2x%*c%2x%*c%2x", (unsigned int *)&addr_5,
               (unsigned int *)&addr_4,(unsigned int *)&addr_3, (unsigned int *)&addr_2, (unsigned int *)&addr_1, (unsigned int *)&addr_0) > 0) {
            addr[5] = addr_5;
            addr[4] = addr_4;
            addr[3] = addr_3;
            addr[2] = addr_2;
            addr[1] = addr_1;
            addr[0] = addr_0;
            LOG_I(common, "addr:%x-%x-%x-%x-%x-%x",addr[5],  addr[4], addr[3],addr[2],addr[1],addr[0]);

            bt_gap_le_set_random_address((bt_bd_addr_ptr_t)&addr);

        } else {
            result = false;
        }
    } else if (strstr(string, "AT+EBTES_DEMO=") != 0) {  // scanable
        uint16_t mode = atoi(string + strlen("AT+EBTES_DEMO="));
        LOG_I(common, "mode %d", mode);
        bt_gap_set_scan_mode((bt_gap_scan_mode_t)mode);
    } else if (strstr(string, "AT+EBTSI_DEMO=") != 0) {  // sniff mode
        bt_gap_default_sniff_params_t info = {0};
        if (sscanf(string+strlen("AT+EBTSI_DEMO="), "%u,%u,%u,%u", (unsigned int *)&info.max_sniff_interval, 
                (unsigned int *)&info.min_sniff_interval, (unsigned int *)&info.sniff_attempt, (unsigned int *)&info.sniff_timeout)) {
            LOG_I(common, "max_inter %d, min_inter %d, attempt %d, to %d", info.max_sniff_interval,info.min_sniff_interval, info.sniff_attempt, info.sniff_timeout);
            bt_gap_set_default_sniff_parameters(&info);
        } else {
            result = false;
        }
    } else if (strstr(string, "AT+EBLECON_DEMO=") != 0) {    // ble connect
        bt_hci_cmd_le_create_connection_t conn_param;
        uint16_t addr_type;

        conn_param.le_scan_interval = 0x10;
        conn_param.le_scan_window = 0x10;
        conn_param.initiator_filter_policy = BT_HCI_CONN_FILTER_ASSIGNED_ADDRESS;
        conn_param.own_address_type = BT_ADDR_PUBLIC;
        conn_param.minimum_ce_length = 0x0000;
        conn_param.maximum_ce_length = 0x0190;

        if (sscanf(string+strlen("AT+EBLECON_DEMO="), "%2x:%2x:%2x:%2x:%2x:%2x,%u,%u,%u,%u,%u", (unsigned int *)&addr_5, (unsigned int *)&addr_4,
                (unsigned int *)&addr_3, (unsigned int *)&addr_2, (unsigned int *)&addr_1, (unsigned int *)&addr_0, (unsigned int *)&addr_type,
                (unsigned int *)&conn_param.conn_interval_min, 
                (unsigned int *)&conn_param.conn_interval_max, 
                (unsigned int *)&conn_param.conn_latency, 
                (unsigned int *)&conn_param.supervision_timeout) > 0) {

            conn_param.peer_address.type = addr_type;
            conn_param.peer_address.addr[5] = addr_5;
            conn_param.peer_address.addr[4] = addr_4;
            conn_param.peer_address.addr[3] = addr_3;
            conn_param.peer_address.addr[2] = addr_2;
            conn_param.peer_address.addr[1] = addr_1;
            conn_param.peer_address.addr[0] = addr_0;

            LOG_I(common, "addr:%x-%x-%x-%x-%x-%x,type:%d,min_interval:%d, max_interval:%d, latency: %d, conn_timeout: %d"
                        ,addr_5,  addr_4, addr_3,addr_2,addr_1,addr_0, addr_type, conn_param.conn_interval_min,
                        conn_param.conn_interval_max, conn_param.conn_latency,  conn_param.supervision_timeout);

            bt_gap_le_connect(&conn_param);
        } else {
            result = false;
        }
    }

    return result;
}

static atci_status_t bt_atci_reg_callback(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};

    LOG_I(common,"[BT_ATCI] mode:%d, CMD:%s", parse_cmd->mode, parse_cmd->string_ptr);

    if (parse_cmd->mode == ATCI_CMD_MODE_EXECUTION) {
        bool result;
        result = bt_atci_parse_cmd(parse_cmd->string_ptr);
        if (result) {
            strcpy((char *)output.response_buf, "Parse OK\n");
        } else {
            strcpy((char *)output.response_buf, "Parse failed\n");
        }
        output.response_len = strlen((char *)output.response_buf);
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&output);

    } else {

        strcpy((char *)output.response_buf, "Not Support\n");
        output.response_len = strlen((char *)output.response_buf);
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        atci_send_response(&output);
    }
    return ATCI_STATUS_OK; 

}

static atci_cmd_hdlr_item_t bt_at_cmd[] = {
{"AT+EBLESA_DEMO", bt_atci_reg_callback, 0, 0},
{"AT+EBLESEA_DEMO", bt_atci_reg_callback, 0, 0},
{"AT+EBLESAD_DEMO", bt_atci_reg_callback, 0, 0},
{"AT+EBLECON_DEMO", bt_atci_reg_callback, 0, 0},
{"AT+EBTES_DEMO", bt_atci_reg_callback, 0, 0},
{"AT+EBTSI_DEMO", bt_atci_reg_callback, 0, 0},
{"AT+EBTTEST_DEMO", bt_atci_reg_callback, 0, 0}
};

#define BT_ATCI_COMMAND_COUNT (sizeof(bt_at_cmd)/sizeof(atci_cmd_hdlr_item_t))


void bt_atci_init(void)
{
    atci_status_t ret = atci_register_handler(&bt_at_cmd[0], BT_ATCI_COMMAND_COUNT);
    if (ret == ATCI_STATUS_OK) {
        LOG_I(common, "bt_atci register success");
    } else {
        LOG_W(common, "bt_atci register fail");
    }
}

