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

#include <stdint.h>
#include <stdio.h>
#include <string.h>


#include "FreeRTOS.h"
#include "task.h"

#include "task_def.h"
#include "syslog.h"

#define ATCI_DEMO
#define BT_SINK_DEMO
#define SLEEP_MANAGER_LOW_POWER_CONFIG

#include "bt_platform.h"
#ifdef BT_SINK_DEMO
#include "bt_sink_app_main.h"
#include "bt_sink_srv_am_task.h"
#include "bt_hfp_codec_internal.h"
#include "audio_middleware_api.h"
#endif /* BT_SINK_DEMO */

#ifdef ATCI_DEMO
#include "atci.h"

#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
#include "serial_port.h"
#endif
#ifdef MTK_AUDIO_MP3_ENABLED
#include "audio_player_main.h"
#endif

void atci_def_task(void *param)
{
    while (1) {
        atci_processing();
    }
}
#endif

/****************************************************************************
 * Types.
 ****************************************************************************/

typedef struct tasks_list_ {
    TaskFunction_t      pvTaskCode;
    char                *pcName;
    uint16_t            usStackDepth;
    void                *pvParameters;
    UBaseType_t         uxPriority;
} tasks_list_t;

/****************************************************************************
 * Forward Declarations.
 ****************************************************************************/
//extern void ui_task_main(void*arg);
extern void wf_app_task(void *arg);
extern void bsp_bt_codec_task_main(void *arg);
extern void am_task_main(void *arg);
extern void bt_sink_app_task_main(void *arg);
extern void ble_app_task(void *arg);
extern void bt_task(void * arg);

static const tasks_list_t tasks_list[] = {
    { bt_task,              "bt_task",      1024,     NULL,   4 },
#ifdef ATCI_DEMO
    //{ atci_def_task,        "ATCI",         ATCI_TASK_STACK_SIZE,     NULL,   3 },
    { atci_def_task,        ATCI_TASK_NAME,         ATCI_TASK_STACKSIZE/(( uint32_t )sizeof( StackType_t )),     NULL,   ATCI_TASK_PRIO },
#endif
//    { ui_task_main,         "UI_DEMO",      4800,     NULL,   5 },
#ifdef BT_SINK_DEMO
    { bsp_bt_codec_task_main,   "BT_codec_task", 1024,    NULL,   5 },
#ifdef MTK_AUDIO_MP3_ENABLED
    { am_task_main,         "AudioManager",  1024,     NULL,   5 },
#else
    { am_task_main,         "AudioManager",  512,     NULL,   5 },
#endif    
    { bt_sink_app_task_main,"BT_sink_task",  1024,    NULL,   1 },
#endif
#ifdef MTK_AUDIO_MP3_ENABLED
    { audio_player_task_main, AUDIO_PLAYER_TASK_NAME, AUDIO_PLAYER_TASK_STACKSIZE/sizeof(StackType_t), NULL, AUDIO_PLAYER_TASK_PRIORITY},
#endif
};

#define tasks_list_count  (sizeof(tasks_list) / sizeof(tasks_list_t))

static TaskHandle_t     task_handles[tasks_list_count];

/****************************************************************************
 * Private Functions
 ****************************************************************************/


/****************************************************************************
 * Public API
 ****************************************************************************/
extern size_t xPortGetFreeHeapSize( void );


void task_def_init(void)
{

#ifdef ATCI_DEMO
#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)

    serial_port_dev_t port;
    serial_port_setting_uart_t uart_setting;

    if(serial_port_config_read_dev_number("atci", &port) != SERIAL_PORT_STATUS_OK)
    {
#if defined(MTK_CORD_FREE_HEADSET_ENABLE) || defined(MTK_INEAR_NOODLES_HEADSET_ENABLE)
        port = SERIAL_PORT_DEV_USB_COM1; //SERIAL_PORT_DEV_USB_COM1;//SERIAL_PORT_DEV_UART_1;
#else
        port = SERIAL_PORT_DEV_UART_2; //SERIAL_PORT_DEV_USB_COM1;//SERIAL_PORT_DEV_UART_1;
#endif
        serial_port_config_write_dev_number("atci", port);
        LOG_W(common, "serial_port_config_write_dev_number setting uart1");
        uart_setting.baudrate = HAL_UART_BAUDRATE_115200;
        serial_port_config_write_dev_setting(port, (serial_port_dev_setting_t *)&uart_setting);
    }

    atci_init(port);

#else
    atci_init(HAL_UART_1);
#endif
#ifdef SLEEP_MANAGER_LOW_POWER_CONFIG
    extern void bt_atci_init(void);
    extern void sleep_atci_init(void);
    extern void local_atci_init(void);

    bt_atci_init();
    sleep_atci_init();
    local_atci_init();
#endif

    //atci_init(HAL_UART_1);
#endif

#ifdef BT_SINK_DEMO
    hal_audio_init();
#endif
}
void task_def_create(void)
{
    uint16_t            i;
    BaseType_t          x;
    const tasks_list_t  *t;

    for (i = 0; i < tasks_list_count; i++) {
        t = &tasks_list[i];

        LOG_I(common, "xCreate task %s, pri %d", t->pcName, (int)t->uxPriority);

        x = xTaskCreate(t->pvTaskCode,
                        t->pcName,
                        t->usStackDepth,
                        t->pvParameters,
                        t->uxPriority,
                        &task_handles[i]);

        if (x != pdPASS) {
            LOG_E(common, ": failed");
        } else {
            LOG_I(common, ": succeeded");
        }
    }
    LOG_I(common, "Free Heap size:%d bytes", xPortGetFreeHeapSize());
}

void task_def_delete_wo_curr_task(void)
{
    uint16_t            i;
    const tasks_list_t  *t;
    // Note:
    // This is a temp solution, making bt to siwtch off before killing bt task.
    // And need to discuss low power team.
    bt_power_off();
    vTaskDelay(1000);
    for (i = 0; i < tasks_list_count; i++) {
        t = &tasks_list[i];
        if (task_handles[i] == NULL) {
            continue;
        }
        if (task_handles[i] == xTaskGetCurrentTaskHandle()) {

             LOG_I(common, "Current task %s, pri %d", t->pcName, (int)t->uxPriority);
             continue;
        }
        
        vTaskDelete(task_handles[i]);
    }
    LOG_I(common, "Free Heap size:%d bytes", xPortGetFreeHeapSize());
}

void task_def_dump_stack_water_mark(void)
{
    uint16_t i;

    for (i = 0; i < tasks_list_count; i++) {
        if (task_handles[i] != NULL) {
            LOG_I(common, "task name:%s, water_mark:%d",
                tasks_list[i].pcName,
                uxTaskGetStackHighWaterMark(task_handles[i]));
        }
    }
}

