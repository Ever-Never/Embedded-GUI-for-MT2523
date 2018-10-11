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


#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#include "at_command.h"
#include "syslog.h"
#include "task_def.h"
#include "os_port_callback.h"

#define LOGE(fmt,arg...)   LOG_E(atcmd, "ATCMD: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atcmd, "ATCMD: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atcmd ,"ATCMD: "fmt,##arg)

/*--- Function ---*/
#if defined(MTK_SYSTEM_AT_COMMAND_ENABLE)
extern atci_status_t atci_cmd_hdlr_system(atci_parse_cmd_param_t *parse_cmd);
#endif /* MTK_SYSTEM_AT_COMMAND_ENABLE */
#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
extern atci_status_t atci_cmd_hdlr_utilization(atci_parse_cmd_param_t *parse_cmd);
#endif /* MTK_OS_CPU_UTILIZATION_ENABLE */


/*
AT+UTILIZATION=?
+UTILIZATION:
AT+UTILIZATION=<op> [,<param>]
<op> string type
start: begin os profiling
stop: stop os profiling
duration: enable os profiling with in a period of time
[, <param>] integer type
param: the profiling duration, the unit is system tick, only needed if <op> is duration

AT+SYSTEM=?
+SYSTEM:
AT+SYSTEM=<module>
<module> string type
task: show all freeRTOS tasks information
mem: show heap status
crash: force trigger system crash to dump system infomation
*/

#if defined(MTK_SYSTEM_AT_COMMAND_ENABLE) || defined(MTK_OS_CPU_UTILIZATION_ENABLE)
static void construct_profiling_info(const char *prefix, atci_response_t *presponse, char *task_list_buf)
{
    char *substr = NULL, *saveptr = NULL;
    int32_t pos = 0;

    LOGI("##task_list_buf: \r\n%s\r\n", task_list_buf);
    substr = strtok_r(task_list_buf, "\n", &saveptr);
    pos = snprintf((char *)(presponse->response_buf), (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE - pos, "%s", prefix);
    pos += snprintf((char *)(presponse->response_buf + pos), (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE - pos, "%s\n", substr);
    while (substr) {
        /*LOGI("##substr: \r\n%s##\r\n",substr); */
        substr = strtok_r(NULL, "\n", &saveptr);
        if ((pos + strlen(substr)) <= ATCI_UART_TX_FIFO_BUFFER_SIZE) {
            pos += snprintf((char *)(presponse->response_buf + pos),
                            ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                            "%s\n",
                            substr);
            /*LOGI("##buf: \r\n%s##\r\n",(char *)(presponse->response_buf)); */
        } else {
            /*LOGI("##buf: \r\n%s##\r\n",(char *)(presponse->response_buf)); */
            presponse->response_len = strlen((char *)(presponse->response_buf));
            presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(presponse);
            pos = 0;
            pos += snprintf((char *)(presponse->response_buf + pos),
                            ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                            "%s",
                            prefix);
            pos += snprintf((char *)(presponse->response_buf + pos),
                            ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                            "%s\n",
                            substr);
        }
    }
    if (strlen((char *)(presponse->response_buf)) != 0) {
        /*LOGI("##buf: \r\n%s##\r\n",(char *)(presponse->response_buf)); */
        presponse->response_len = strlen((char *)(presponse->response_buf));
        presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(presponse);
    }
}
#endif /* #if defined(MTK_SYSTEM_AT_COMMAND_ENABLE) || defined(MTK_OS_CPU_UTILIZATION_ENABLE) */

#if defined(MTK_SYSTEM_AT_COMMAND_ENABLE)
static void system_show_usage(uint8_t *buf)
{
    int32_t pos = 0;

    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "%s",
                    "+SYSTEM:\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "(AT+SYSTEM=<module>\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "(<module>=task|mem|crash)\r\n");
}

#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) )
static void system_show_task_info(atci_response_t *presponse)
{
    char *task_list_buf;
    int32_t buf_size = uxTaskGetNumberOfTasks() * (configMAX_TASK_NAME_LEN + 18);

    if ((task_list_buf = pvPortMalloc(buf_size)) == NULL) {
        LOGE("memory malloced failed.\n");
        strncpy((char *)(presponse->response_buf),
                "+SYSTEM:\r\nheap memory is not enough to do task information profiling\r\n",
                (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
        presponse->response_len = strlen((char *)(presponse->response_buf));
        presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
        atci_send_response(presponse);
    }

    strncpy((char *)(presponse->response_buf),
            "+SYSTEM:\r\nparameter meaning:\r\n1: pcTaskName\r\n2: cStatus(Ready/Blocked/Suspended/Deleted)\r\n3: uxCurrentPriority\r\n4: usStackHighWaterMark\r\n5: xTaskNumber\r\n",
            (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
    presponse->response_len = strlen((char *)(presponse->response_buf));
    presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
    atci_send_response(presponse);

    vTaskList(task_list_buf);
    construct_profiling_info("+SYSTEM:\r\n", presponse, task_list_buf);

    vPortFree(task_list_buf);
}
#endif

static void system_query_mem(uint8_t *buf)
{
    int32_t pos = 0;
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "%s",
                    "+SYSTEM:\r\nheap information: \r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "\ttotal: %d\r\n",
                    configTOTAL_HEAP_SIZE);
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "\tfree: %d\r\n",
                    xPortGetFreeHeapSize());
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "\tmini: %d\r\n",
                    xPortGetMinimumEverFreeHeapSize());
}

/* AT command handler  */
atci_status_t atci_cmd_hdlr_system(atci_parse_cmd_param_t *parse_cmd)
{
    char *param = NULL, *saveptr = NULL;
    atci_response_t *presponse = pvPortMalloc(sizeof(atci_response_t));
    if (presponse == NULL) {
        LOGE("memory malloced failed.\r\n");
        strncpy((char *)(presponse->response_buf),
                "+SYSTEM:\r\nheap memory is not enough to do task information profiling\r\n",
                (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
        presponse->response_len = strlen((char *)(presponse->response_buf));
        presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
        return ATCI_STATUS_OK;
    }
    memset(presponse, 0, sizeof(atci_response_t));
    presponse->response_flag = 0; /* Command Execute Finish. */
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_READ: /* rec: AT+SYSTEM? */
        case ATCI_CMD_MODE_TESTING: /* rec: AT+SYSTEM=? */
            system_show_usage(presponse->response_buf);
            presponse->response_len = strlen((char *)(presponse->response_buf));
            presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(presponse);
            break;
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+SYSTEM=<module> */
            param = strtok_r(parse_cmd->string_ptr, "\n\r", &saveptr);
            param = strtok_r(param, "AT+SYSTEM=", &saveptr);
            LOGI("##module=%s##\r\n", param);

            if (strcmp(param, "task") == 0) {
#if ( ( configUSE_TRACE_FACILITY == 1 ) && ( configUSE_STATS_FORMATTING_FUNCTIONS == 1 ) )
                system_show_task_info(presponse);
#else
                strncpy((char *)(presponse->response_buf),
                        "+SYSTEM:\r\nplease define configUSE_TRACE_FACILITY = 1 and configUSE_STATS_FORMATTING_FUNCTIONS = 1 in project freeRTOSConfig.h \r\n",
                        (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                presponse->response_len = strlen((char *)(presponse->response_buf));
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                atci_send_response(presponse);
#endif
            } else if (strcmp(param, "mem") == 0) {
                system_query_mem(presponse->response_buf);
                presponse->response_len = strlen((char *)(presponse->response_buf));
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(presponse);
            } else if (strcmp(param, "crash") == 0) {
                strncpy((char *)(presponse->response_buf),
                        "+SYSTEM:\r\n system will crash...\r\n",
                        (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                presponse->response_len = strlen((char *)(presponse->response_buf));
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(presponse);
                configASSERT(0);
            } else {
                /* command syntax error */
                strncpy((char *)(presponse->response_buf),
                        "+SYSTEM:\r\ncommand syntax error\r\n",
                        (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                presponse->response_len = strlen((char *)(presponse->response_buf));
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                atci_send_response(presponse);
            }
            break;
        default :
            /* others are invalid command format */
            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            presponse->response_len = strlen((char *)(presponse->response_buf));
            atci_send_response(presponse);
            break;
    }

    vPortFree(presponse);
    return ATCI_STATUS_OK;
}
#endif /* MTK_SYSTEM_AT_COMMAND_ENABLE */

#if defined(MTK_OS_CPU_UTILIZATION_ENABLE)
static void utilization_show_usage(uint8_t *buf)
{
    int32_t pos = 0;

    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "%s",
                    "+UTILIZATION:\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "(AT+UTILIZATION=<op> [,<param>])\r\n");
    pos += snprintf((char *)(buf + pos),
                    ATCI_UART_TX_FIFO_BUFFER_SIZE - pos,
                    "(<op>=[start|stop|duration]\r\n<param> means the profiling duration, the unit is system tick, only needed when <op>=duration)\r\n");
}

static TaskHandle_t xUtilTaskHandle = NULL;
static uint32_t _cpu_meausre_time;
void _cpu_utilization_task(void *arg)
{
    char *task_list_buf = NULL;
    atci_response_t *presponse = NULL;
    int32_t buf_size = uxTaskGetNumberOfTasks() * (configMAX_TASK_NAME_LEN + 20);

#if 0
    UBaseType_t stackmark = MTK_OS_CPU_UTILIZATION_STACKSIZE - uxTaskGetStackHighWaterMark(NULL) * 4;
    LOGI("stack usage: %d \r\n", stackmark);
#endif
    if ((presponse = pvPortMalloc(sizeof(atci_response_t))) != NULL) {
        if ((task_list_buf = pvPortMalloc(buf_size)) == NULL) {
            LOGE("memory malloced failed, and do cpu utilization \r\n");
            strncpy((char *)(presponse->response_buf),
                    "+UTILIZATION:\r\nheap memory is not enough to do cpu utilization\r\n",
                    ATCI_UART_TX_FIFO_BUFFER_SIZE);
            presponse->response_len = strlen((char *)(presponse->response_buf));
            presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(presponse);
        } else {
            vConfigureTimerForRunTimeStats();
            vTaskClearTaskRunTimeCounter();

            if (_cpu_meausre_time > 0) {
                vTaskDelay(_cpu_meausre_time); /* duration mode */
            } else {
                vTaskSuspend(NULL); /* start/stop mode */
            }

            /* after resume */
            vTaskGetRunTimeStats(task_list_buf);

            strncpy((char *)(presponse->response_buf),
                    "+UTILIZATION:\r\nparameter meaning:\r\n1: pcTaskName\r\n2: count(unit is 32k of gpt)\r\n3: ratio\r\n",
                    (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
            presponse->response_len = strlen((char *)(presponse->response_buf));
            presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(presponse);

            construct_profiling_info("+UTILIZATION:\r\n", presponse, task_list_buf);
            vPortFree(task_list_buf);
            vPortFree(presponse);
#if 0
            stackmark = MTK_OS_CPU_UTILIZATION_STACKSIZE - uxTaskGetStackHighWaterMark(NULL) * 4;
            LOGI("stack usage: %d \r\n", stackmark);
#endif

            vTaskDelete(xTaskGetCurrentTaskHandle());
        }
    } else {
        LOGE("memory malloced failed, and do cpu utilization \r\n");
        vTaskDelete(xTaskGetCurrentTaskHandle());
    }
    while (1);
}

static BaseType_t utilization_cpu_benchmark(uint32_t duration)
{
    BaseType_t ret = pdPASS;
    _cpu_meausre_time = duration;
    ret = xTaskCreate(_cpu_utilization_task,
                      MTK_OS_CPU_UTILIZATION_TASK_NAME,
                      MTK_OS_CPU_UTILIZATION_STACKSIZE / sizeof(StackType_t),
                      NULL,
                      MTK_OS_CPU_UTILIZATION_PRIO,
                      &xUtilTaskHandle);
    return ret;
}

atci_status_t atci_cmd_hdlr_utilization(atci_parse_cmd_param_t *parse_cmd)
{
    char *op = NULL, *param = NULL, *saveptr = NULL;
    BaseType_t ret;
    uint32_t duration = 0;
    static uint32_t startflag = 0;
    atci_response_t *presponse = pvPortMalloc(sizeof(atci_response_t));
    if (presponse == NULL) {
        LOGE("memory malloced failed.\r\n");
        strncpy((char *)(presponse->response_buf),
                "+UTILIZATION:\r\nheap memory is not enough to do cpu utilization\r\n",
                (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
        presponse->response_len = strlen((char *)(presponse->response_buf));
        presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
        atci_send_response(presponse);
        return ATCI_STATUS_OK;
    }
    memset(presponse, 0, sizeof(atci_response_t));
    presponse->response_flag = 0; /* Command Execute Finish. */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_READ: /* rec: AT+UTILIZATION?  */
        case ATCI_CMD_MODE_TESTING: /* rec: AT+UTILIZATION=? */
            utilization_show_usage(presponse->response_buf);
            presponse->response_len = strlen((char *)(presponse->response_buf));
            presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
            atci_send_response(presponse);
            break;
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+UTILIZATION=<op> [,<param>] */
            op = strtok_r(parse_cmd->string_ptr, "\n\r", &saveptr);
            op = strtok_r(op, "AT+UTILIZATION=", &saveptr);
            op = strtok_r(op, ",", &saveptr);
            param = strtok_r(NULL, ",", &saveptr);
            LOGI("##op=%s,param=%s##\r\n", op, param);
            if (strcmp(op, "start") == 0) {
                /* start/stop mode */
                if (!startflag) {
                    ret = utilization_cpu_benchmark(0);
                    if (ret == pdPASS) {
                        startflag = 1;
                        strncpy((char *)(presponse->response_buf),
                                "+UTILIZATION:\r\ncpu utilization profing begin, please use AT+UTILIZATION=stop to end profiling...\r\n",
                                (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                        presponse->response_len = strlen((char *)(presponse->response_buf));
                        presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                        atci_send_response(presponse);
                    } else {
                        LOGE("memory malloced failed, and cannot create profiling task\r\n");
                        strncpy((char *)(presponse->response_buf),
                                "+UTILIZATION:\r\nheap memory is not enough to do cpu utilization\r\n",
                                (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                        presponse->response_len = strlen((char *)(presponse->response_buf));
                        presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        atci_send_response(presponse);
                    }
                } else {
                    strncpy((char *)(presponse->response_buf),
                            "+UTILIZATION:\r\nutilization profiling already started, please use AT+UTILIZATION=stop to stop profiling\r\n",
                            (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                    presponse->response_len = strlen((char *)(presponse->response_buf));
                    presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    atci_send_response(presponse);
                }
            } else if (strcmp(op, "stop") == 0) {
                if (!startflag) {
                    strncpy((char *)(presponse->response_buf),
                            "+UTILIZATION:\r\nutilization profiling didn't start, please use AT+UTILIZATION=start to start firstly\r\n",
                            (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                    presponse->response_len = strlen((char *)(presponse->response_buf));
                    presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    atci_send_response(presponse);
                } else {
                    vTaskResume(xUtilTaskHandle);
                    startflag = 0;
                }
            } else if (strcmp(op, "duration") == 0) {
                /* duration mode */
                duration = atoi(param);
                /* param validity check */
                if (duration <= 0) {
                    strncpy((char *)(presponse->response_buf),
                            "+UTILIZATION:\r\nparam must be positive integer if <op>=duration, which means profiling duration\r\n",
                            (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                    presponse->response_len = strlen((char *)(presponse->response_buf));
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    atci_send_response(presponse);
                } else if (startflag) {
                    /* profiling already begin by start mode, duration mode is invalid until profiling stop by AT+UTILIZATION=stop */
                    strncpy((char *)(presponse->response_buf),
                            "+UTILIZATION:\r\nprofiling already begin by AT+UTILIZATION=start, duration mode is invalid until profiling stoped by AT+UTILIZATION=stop\r\n",
                            (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                    presponse->response_len = strlen((char *)(presponse->response_buf));
                    presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    atci_send_response(presponse);
                } else {
                    /* start profiling */
                    ret = utilization_cpu_benchmark(duration); /* duration mode */
                    if (ret == pdPASS) {
                        snprintf((char *)(presponse->response_buf),
                                 ATCI_UART_TX_FIFO_BUFFER_SIZE,
                                 "+UTILIZATION:\r\n profiling begin, duration is %d tick, the tick's unit is 1/configTICK_RATE_HZ\r\n",
                                 (int)duration);
                        presponse->response_len = strlen((char *)(presponse->response_buf));
                        presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;
                        atci_send_response(presponse);
                    } else {
                        LOGE("memory malloced failed, and cannot create profiling task\r\n");
                        strncpy((char *)(presponse->response_buf),
                                "+UTILIZATION:\r\nheap memory is not enough to do cpu utilization\r\n",
                                (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                        presponse->response_len = strlen((char *)(presponse->response_buf));
                        presponse->response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        atci_send_response(presponse);
                    }
                }
            } else {
                /* command syntax error */
                strncpy((char *)(presponse->response_buf),
                        "+UTILIZATION:\r\n command syntax error\r\n",
                        (int32_t)ATCI_UART_TX_FIFO_BUFFER_SIZE);
                presponse->response_len = strlen((char *)(presponse->response_buf));
                presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                atci_send_response(presponse);
            }
            break;
        default :
            /* others are invalid command format */
            presponse->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            presponse->response_len = strlen((char *)(presponse->response_buf));
            atci_send_response(presponse);
            break;
    }

    vPortFree(presponse);
    return ATCI_STATUS_OK;
}
#endif /* MTK_OS_CPU_UTILIZATION_ENABLE */


