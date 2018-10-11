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

#if defined(MTK_MINICLI_ENABLE)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#include "os_cli.h"

#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
#include "task_def.h"
#endif

/****************************************************************************
 *
 * Constants.
 *
 ****************************************************************************/


#define SZ_OF_TASK_LIST_BUF     (256)


/****************************************************************************
 *
 * Types.
 *
 ****************************************************************************/


/****************************************************************************
 *
 * Static variables.
 *
 ****************************************************************************/


#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
static uint32_t _cpu_meausre_time;
#endif


/****************************************************************************
 *
 * Local functions.
 *
 ****************************************************************************/


#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) )
static uint8_t _os_cli_show_task_info(uint8_t len, char *param[])
{
    printf("Show task info:\n");
    printf("parameter meaning:\n");
    printf("1: pcTaskName\n");
    printf("2: cStatus\n");
    printf("3: uxCurrentPriority\n");
    printf("4: usStackHighWaterMark\n");
    printf("5: xTaskNumber\n\n");

    char *task_list_buf;
    int32_t buf_size = uxTaskGetNumberOfTasks() * (configMAX_TASK_NAME_LEN + 18);

    if ((task_list_buf = pvPortMalloc(buf_size)) == NULL) {
        printf("memory malloced failed.\n");
        return 1;
    }

    vTaskList(task_list_buf);
    strlen(task_list_buf);
    printf("%s\n", task_list_buf);
    printf("buf len: %u\n", strlen(task_list_buf));
    vPortFree(task_list_buf);

    return 0;
}
#endif


#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
static void _cpu_utilization_task(void *arg)
{
    char *task_list_buf;
    int32_t buf_size = uxTaskGetNumberOfTasks() * (configMAX_TASK_NAME_LEN + 20);
    if ((task_list_buf = pvPortMalloc(buf_size)) == NULL) {
        printf("memory malloced failed.\n");
        return;
    }

    vConfigureTimerForRunTimeStats();
    vTaskClearTaskRunTimeCounter();
    vTaskDelay(_cpu_meausre_time);
    vTaskGetRunTimeStats(task_list_buf);
    printf("%s\n", task_list_buf);

    vPortFree(task_list_buf);
    vTaskDelete(xTaskGetCurrentTaskHandle());
    while (1);
}
#endif

#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
static uint8_t _os_cli_cpu_utilization(uint8_t len, char *param[])
{
    BaseType_t ret;

    if (len == 1) {
        TaskHandle_t xHandle = NULL;
        _cpu_meausre_time = atoi(param[0]);
        ret = xTaskCreate(_cpu_utilization_task, MTK_OS_CPU_UTILIZATION_TASK_NAME, MTK_OS_CPU_UTILIZATION_STACKSIZE / sizeof(StackType_t), NULL, MTK_OS_CPU_UTILIZATION_PRIO, &xHandle);
        if (ret != pdPASS) {
            configASSERT(0);
        }
    } else {
        printf("os cpu <duration>\n");
    }

    return 0;
}
#endif


static uint8_t _os_cli_mem(uint8_t len, char *param[])
{
    printf("heap:\n");
    printf("\ttotal: %u\n", configTOTAL_HEAP_SIZE);
    printf("\tfree:  %u\n", xPortGetFreeHeapSize());
    printf("\tlow:   %u\n", xPortGetMinimumEverFreeHeapSize());
    return 0;
}

static uint8_t _os_cli_crash(uint8_t len, char *param[])
{
    configASSERT(0);
    return 0;
}

/****************************************************************************
 *
 * API variable.
 *
 ****************************************************************************/


/****************************************************************************
 *
 * API functions.
 *
 ****************************************************************************/

cmd_t os_cli[] = {
#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
    { "cpu",   "show cpu utilization", _os_cli_cpu_utilization },
#endif
#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) )
    { "task",  "show FreeRtos task",   _os_cli_show_task_info  },
#endif
    { "mem",   "show heap status",     _os_cli_mem             },
    { "crash", "force system crash",   _os_cli_crash         },
    { NULL }
};

#endif /* #if defined(MTK_MINICLI_ENABLE) */

