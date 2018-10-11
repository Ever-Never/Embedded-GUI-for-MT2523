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
#include "os.h"
#include "sys_init.h"
#include "wifi_api.h"
#include "wifi_lwip_helper.h"
#include "task_def.h"

#include "sntp.h"
#include "syslog.h"
#include "hal_rtc.h"


#define WIFI_SSID                ("SQA_TEST_AP")
#define WIFI_PASSWORD            ("77777777")

/**
  * Create the log control block for lwip socket example.
  * User needs to define their own log control blocks as project needs.
  * Please refer to the log dev guide under /doc folder for more details.
  */
log_create_module(sntp_client_main, PRINT_LEVEL_INFO);


/**
* @brief         SNTP task process function.
* @param[in]     Not using
* @return        None
*/
static void verify_proc(void *args)
{
    hal_rtc_time_t r_time;
    hal_rtc_status_t ret = HAL_RTC_STATUS_OK;

    for (int i = 0 ; i < 90; i++) {
        LOG_I(sntp_client_main, "Waiting for SNTP success [%d]", i);
        ret = hal_rtc_get_time(&r_time);
        if (ret == HAL_RTC_STATUS_OK && (r_time.rtc_year != 0 || r_time.rtc_mon != 1 || r_time.rtc_day != 1)) {
            LOG_I(sntp_client_main, "SNTP success [%d]", i);
            LOG_I(sntp_client_main, "cur_time[%d,%d,%d,%d]", r_time.rtc_year, r_time.rtc_mon, r_time.rtc_day, r_time.rtc_week);
            LOG_I(sntp_client_main, "[%d]cur_time[%d:%d:%d]", ret, r_time.rtc_hour, r_time.rtc_min, r_time.rtc_sec);
            sntp_stop();
            break;
        }
        vTaskDelay(1000 / portTICK_RATE_MS);
    }

    LOG_I(sntp_client_main, "test_proc TaskDelete");
    LOG_I(sntp_client_main, "example project test success.");
    vTaskDelete(NULL);
}


/**
* @brief        SNTP client example entry function
* @return       None
*/
static void sntp_client(void)
{
    /** Set this to 1 to allow config of SNTP server(s) by DNS name */
#if (!SNTP_SERVER_DNS)
    struct ip4_addr test_addr;
#endif
    hal_rtc_time_t r_time = {6,6,6,1,1,6,0};
    hal_rtc_status_t ret = HAL_RTC_STATUS_OK;

    //Set RTC to a incorrect time.
    ret = hal_rtc_set_time(&r_time);
    LOG_I(sntp_client_main, "[%d]cur_time[%d:%d:%d]", ret, r_time.rtc_hour, r_time.rtc_min, r_time.rtc_sec);

    //Create a task to check SNTP status.
    portBASE_TYPE type = xTaskCreate(verify_proc, APP_SNTP_TASK_NAME, APP_SNTP_TASK_STACKSIZE / sizeof(portSTACK_TYPE), NULL, APP_SNTP_TASK_PRIO, NULL);
    LOG_I(sntp_client_main, "xTaskCreate test_proc -- %d", type);

    //SNTP example start.
    LOG_I(sntp_client_main, "Begin to init SNTP");
    
    /** Set this to 1 to allow config of SNTP server(s) by DNS name */
#if SNTP_SERVER_DNS
    sntp_setservername(0, "1.cn.pool.ntp.org");
    sntp_setservername(1, "1.hk.pool.ntp.org");
#else
    IP4_ADDR(&test_addr, 213, 161, 194, 93);
    sntp_setserver(0, (const ip_addr_t *)(&test_addr));
    IP4_ADDR(&test_addr, 129, 6, 15, 29);
    sntp_setserver(1, (const ip_addr_t *)(&test_addr));
#endif
    sntp_init();

    LOG_I(sntp_client_main, "SNTP init done");
}


/**
  * @brief     Create a task for SNTP client example
  * @param[in] void *args:Not used
  * @retval    None
  */
static void user_entry(void *args)
{
    lwip_net_ready();

    sntp_client();

    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}



/**
  * @brief  Main program
  * @param  None
  * @retval int
  */
int main(void)
{    
    /* Do system initialization, eg: hardware, nvdm, logging and random seed. */
    system_init();
    
    /* system log initialization.
        * This is the simplest way to initialize system log, that just inputs three NULLs
        * as input arguments. User can use advanved feature of system log along with NVDM.
        * For more details, please refer to the log dev guide under /doc folder or projects
        * under project/mtxxxx_hdk/apps/.
        */
    log_init(NULL, NULL, NULL);
    
    LOG_I(sntp_client_main, "FreeRTOS Running");

    /* User initial the parameters for wifi initial process,  system will determin which wifi operation mode
        * will be started , and adopt which settings for the specific mode while wifi initial process is running
        */
    wifi_config_t config = {0};
    config.opmode = WIFI_MODE_STA_ONLY;
    strcpy((char *)config.sta_config.ssid, WIFI_SSID);
    strcpy((char *)config.sta_config.password, WIFI_PASSWORD);
    config.sta_config.ssid_length = strlen(WIFI_SSID);
    config.sta_config.password_length = strlen(WIFI_PASSWORD);

    /* Initialize wifi stack and register wifi init complete event handler,
        * notes:  the wifi initial process will be implemented and finished while system task scheduler is running.
        */
    wifi_init(&config, NULL);

    /* Tcpip stack and net interface initialization,  dhcp client, dhcp server process initialization. */
    lwip_network_init(config.opmode);
    lwip_net_start(config.opmode);

    /* Create a user task for demo when and how to use wifi config API  to change WiFI settings,
        * Most WiFi APIs must be called in task scheduler, the system will work wrong if called in main(),
        * For which API must be called in task, please refer to wifi_api.h or WiFi API reference.
        * xTaskCreate(user_wifi_app_entry,
        *       UNIFY_USR_DEMO_TASK_NAME,
        *       UNIFY_USR_DEMO_TASK_STACKSIZE / 4,
        *       NULL, UNIFY_USR_DEMO_TASK_PRIO, NULL);
        */
    if (pdPASS != xTaskCreate(user_entry,
                              APP_TASK_NAME,
                              APP_TASK_STACKSIZE / sizeof(portSTACK_TYPE),
                              NULL,
                              APP_TASK_PRIO,
                              NULL)) {
        LOG_E(sntp_client_main, "create user task fail");
        return -1;
    }

    /* Call this function to indicate the system initialize done. */
    SysInitStatus_Set();

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
        * will never be reached.  If the following line does execute, then there was
        * insufficient FreeRTOS heap memory available for the idle and/or timer tasks
        * to be created.  See the memory management section on the FreeRTOS web site
        * for more details.
        */
    for ( ;; );
}


