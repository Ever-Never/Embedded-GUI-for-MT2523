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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal.h"

#include "syslog.h"
#include "sys_init.h"

/* application includes */
#include "bt_init.h"
#include "task_def.h"
#include "hal_sleep_manager_platform.h"
/* Create the log control block as user wishes. Here we use 'template' as module name.
 * User needs to define their own log control blocks as project needs.
 * Please refer to the log dev guide under /doc folder for more details.
 */
log_create_module(template, PRINT_LEVEL_INFO);

/**
* @brief       Main function
* @param[in]   None.
* @return      None.
*/
int count = 0;
uint32_t example_sleep_time = 5000;
#define WITHOS
/*
 * enter_power_saving_task behaviour is : (Activity -> Deep-sleep->Activity -> Sleep)*2 -> Power Off
 * hal_sleep_manager_set_sleep_time : Set sleep(suspend) time length.If no other wake up event occurs, the system will sleep for a specific time.Unit is ms
 * hal_sleep_manager_enter_sleep_mode : This function sets the system to sleep mode ,Ref:hal_sleep_mode_t
 * */
static void enter_power_saving_task(void *pvParameters)
{

    while (1) {
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        /* Enter a critical section but don't use the taskENTER_CRITICAL()
         method as that will mask interrupts that should exit sleep mode.
         Ref:http://www.freertos.org/FreeRTOS_Support_Forum_Archive/November_2013/freertos_Implementing_tickless_idle_on_Cortex_M4._72d095e3j.html */
        __asm volatile("cpsid i");
        LOG_I(template, "Example:Enter Deep Sleep.\n");
        hal_sleep_manager_set_sleep_time(example_sleep_time);
        hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_DEEP_SLEEP);
        LOG_I(template, "Example:Enter Sleep.\n");
        hal_sleep_manager_set_sleep_time(example_sleep_time);
        hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_SLEEP);
        /* Re-enable interrupts - see comments above the cpsid instruction() above.
         * Ref:http://www.freertos.org/FreeRTOS_Support_Forum_Archive/November_2013/freertos_Implementing_tickless_idle_on_Cortex_M4._72d095e3j.html */
        __asm volatile("cpsie i");
        LOG_I(template, "Power off.\n");
        hal_sleep_manager_enter_power_off_mode();
    }
}
int main(void)
{
    /* Do system initialization, eg: hardware, nvdm. */
    system_init();

    /* system log initialization.
     * This is the simplest way to initialize system log, that just inputs three NULLs
     * as input arguments. User can use advanced feature of system log along with NVDM.
     * For more details, please refer to the log dev guide under /doc folder or projects
     * under project/mtxxxx_hdk/apps/.
     */
    log_init(NULL, NULL, NULL);

    LOG_I(template, "start to create task.\n");

    /* user may create own tasks here.
     * EX:
     *    xTaskCreate(UserTask, USER_TASK_NAME, USER_TASK_STACKSIZE/sizeof(StackType_t), NULL, USER_TASK_PRIO, NULL);
     *
     *    UserTask is user's task entry function, which may be defined in another C file to do application job.
     *    USER_TASK_NAME, USER_TASK_STACKSIZE and USER_TASK_PRIO should be defined in task_def.h. User needs to
     *    refer to example in task_def.h, then makes own task MACROs defined.
     */
#ifdef WITHOS
    LOG_I(template, "This example run with OS \n");
    xTaskCreate(enter_power_saving_task, SLEEP_MANAGER_EXAMPLE_TASK_NAME,
                SLEEP_MANAGER_EXAMPLE_TASK_STACKSIZE / sizeof(StackType_t), NULL,
                SLEEP_MANAGER_EXAMPLE_TASK_PRIO, NULL);


    /* Call this function to indicate the system initialize done. */
    SysInitStatus_Set();

    /* Start the scheduler. */
    vTaskStartScheduler();
#else //non-os
    LOG_I(template, "This example run with non-OS \n");
    __asm volatile("cpsid i");
    LOG_I(template, "Example:Enter Deep Sleep.\n");
    hal_sleep_manager_set_sleep_time(example_sleep_time);
    hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_DEEP_SLEEP);
    LOG_I(template, "Example:Enter Sleep.\n");
    hal_sleep_manager_set_sleep_time(example_sleep_time);
    hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_SLEEP);
    /* Re-enable interrupts - see comments above the cpsid instruction() above.
     * Ref:http://www.freertos.org/FreeRTOS_Support_Forum_Archive/November_2013/freertos_Implementing_tickless_idle_on_Cortex_M4._72d095e3j.html */
    __asm volatile("cpsie i");
    LOG_I(template, "Power off.\n");
    hal_sleep_manager_enter_power_off_mode();
#endif
    /* If all is well, the scheduler will now be running, and the following line
     will never be reached.  If the following line does execute, then there was
     insufficient FreeRTOS heap memory available for the idle and/or timer tasks
     to be created.  See the memory management section on the FreeRTOS web site
     for more details. */
    for (;;);
}

