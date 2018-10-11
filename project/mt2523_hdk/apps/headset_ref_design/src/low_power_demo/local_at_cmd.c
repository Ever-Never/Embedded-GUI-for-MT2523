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

#include "bt_sink_app_main.h"
#include "bt_sink_srv_am_task.h"
#include "bt_hfp_codec_internal.h"
#include "audio_middleware_api.h"
#include "audio_player_main.h"

#ifndef ATCI_DEMO
#define ATCI_DEMO
#endif
#ifndef BT_SINK_DEMO
#define BT_SINK_DEMO
#endif

typedef struct _lp_tasks_list_ {
    TaskFunction_t      pvTaskCode;
    char                *pcName;
    uint16_t            usStackDepth;
    void                *pvParameters;
    UBaseType_t         uxPriority;
} lp_tasks_list_t;

//extern void bsp_bt_codec_task_main(void *arg);
extern void am_task_main(void *arg);
//extern void bt_sink_app_task_main(void *arg);
//extern void bt_task(void * arg);

static bool g_local_lp_switch = false;

static const lp_tasks_list_t local_lp_list[] = {
    //{ bt_task,              BT_TASK_NAME,           BT_TASK_STACKSIZE/((uint32_t)sizeof( StackType_t )),     NULL,    BT_TASK_PRIORITY},
#ifdef ATCI_DEMO
    //{ atci_def_task,        ATCI_TASK_NAME,         ATCI_TASK_STACKSIZE/(( uint32_t )sizeof( StackType_t )),     NULL,   ATCI_TASK_PRIO },
#endif

    //{ bsp_bt_codec_task_main,    BT_CODEC_TASK_NAME,     BT_CODEC_TASK_STACKSIZE/(( uint32_t )sizeof( StackType_t )),    NULL,   BT_CODEC_TASK_PRIO },
    { am_task_main,          AM_TASK_NAME,           AM_TASK_STACKSIZE/(( uint32_t )sizeof( StackType_t )),     NULL,   AM_TASK_PRIO },
    { audio_player_task_main, AUDIO_PLAYER_TASK_NAME, AUDIO_PLAYER_TASK_STACKSIZE/sizeof(StackType_t), NULL, AUDIO_PLAYER_TASK_PRIORITY},
    //{ bt_sink_app_task_main, BT_SINK_APP_TASK_NAME,  BT_SINK_APP_TASK_STACKSIZE/(( uint32_t )sizeof( StackType_t )),    NULL,   BT_SINK_APP_TASK_PRIO },
};

#define lp_list_count  (sizeof(local_lp_list) / sizeof(lp_tasks_list_t))

static TaskHandle_t     lp_handles[lp_list_count];

void local_lp_task_create(void)
{
    uint16_t            i;
    BaseType_t          x;
    const lp_tasks_list_t  *t;

    // deinit mp3 context before create a new one.
    bt_sink_srv_mp3_deinit();

    for (i = 0; i < lp_list_count; i++) {
        t = &local_lp_list[i];

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


extern void bt_sink_srv_mp3_set_lp(void);
static bool local_atci_parse_cmd(char *string)
{
    bool result = true;

    if (strstr(string, "AT+LOCAL=LP") != 0) {
        /* Create tasks for local music low power. */
        LOG_I(common, "enter low power");
        if (g_local_lp_switch == false) {
            g_local_lp_switch = true;
            local_lp_task_create();
            bt_sink_srv_mp3_set_lp();
            //bt_demo_power_on();
        }
        return result;
    } else if (strstr(string, "AT+LOCAL=PLAY") != 0) {
        audio_player_play();
    } else if (strstr(string, "AT+LOCAL=PAUSE_WAV") != 0) {
        audio_player_pause_wav();
    } else if (strstr(string, "AT+LOCAL=RESUME_WAV") != 0) {
        audio_player_resume_wav();
    } else if (strstr(string, "AT+LOCAL=VOLUME") != 0) {
    }

    /* Reture directly if not enter local low power, avoiding exception. */
    if (g_local_lp_switch == false) {
        LOG_W(common, "[LOCAL_LP] Please enter low power mode firstly");
        return result;
    }

    return result;
}

static atci_status_t local_atci_reg_callback(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};

    LOG_I(common,"[LOCAL_ATCI] mode:%d, CMD:%s", parse_cmd->mode, parse_cmd->string_ptr);

    if (parse_cmd->mode == ATCI_CMD_MODE_EXECUTION) {
        bool result;
        result = local_atci_parse_cmd(parse_cmd->string_ptr);
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

static atci_cmd_hdlr_item_t local_at_cmd[] = {
{"AT+LOCAL=LP", local_atci_reg_callback, 0, 0},
{"AT+LOCAL=PLAY", local_atci_reg_callback, 0, 0},
{"AT+LOCAL=PAUSE_WAV", local_atci_reg_callback, 0, 0},
{"AT+LOCAL=RESUME_WAV", local_atci_reg_callback, 0, 0},
{"AT+LOCAL=VOLUME", local_atci_reg_callback, 0, 0},
};

#define LOCAL_ATCI_COMMAND_COUNT (sizeof(local_at_cmd)/sizeof(atci_cmd_hdlr_item_t))


void local_atci_init(void)
{
    atci_status_t ret = atci_register_handler(&local_at_cmd[0], LOCAL_ATCI_COMMAND_COUNT);
    if (ret == ATCI_STATUS_OK) {
        LOG_I(common, "local_atci register success");
    } else {
        LOG_W(common, "local_atci register fail");
    }
}

