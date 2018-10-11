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

#include "sys_init.h"
#include "wifi_api.h"
#include "timers.h"
#include "wifi_lwip_helper.h"
#include "httpclient.h"

#define WIFI_SSID                ("SQA_TEST_AP")
#define WIFI_PASSWORD            ("77777777")

#define BUF_SIZE                    (1024 * 1)
#define HTTPS_MTK_CLOUD_POST_URL    "https://api.mediatek.com/mcs/v2/devices/D0n2yhrl/datapoints.csv"
#define HTTPS_MTK_CLOUD_HEADER      "deviceKey:FZoo0S07CpwUHcrt\r\n"
#define HTTPS_POST_TIME_TICK        (5000/portTICK_PERIOD_MS)

static httpclient_t client = {0};
static int post_count = 5; 

log_create_module(http_client_keepalive_example, PRINT_LEVEL_INFO);

/**
  * @brief      Send/Recv package by "post" method.
  * @param      None
  * @return     0, if OK.\n
  *             <0, Error code, if errors occurred.\n
  */
static HTTPCLIENT_RESULT httpclient_test_keepalive_post(void)
{
    HTTPCLIENT_RESULT ret = HTTPCLIENT_ERROR_CONN;
    char *post_url = HTTPS_MTK_CLOUD_POST_URL;
    char *header = HTTPS_MTK_CLOUD_HEADER;
    httpclient_data_t client_data = {0};
    char *buf;
    char *content_type = "text/csv";
    char post_data[32];

    buf = pvPortMalloc(BUF_SIZE);
    if (buf == NULL) {        
        LOG_I(http_client_keepalive_example, "memory malloc failed.");
        return ret;
    }

    client_data.response_buf = buf;
    client_data.response_buf_len = BUF_SIZE;        
    client_data.post_content_type = content_type;
    sprintf(post_data, "1,,temperature:%d", (10 + post_count));
    client_data.post_buf = post_data;
    client_data.post_buf_len = strlen(post_data);
    httpclient_set_custom_header(&client, header);
    
    ret = httpclient_send_request(&client, post_url, HTTPCLIENT_POST, &client_data);
    if (ret < 0)                     
        goto fail;
    
    ret = httpclient_recv_response(&client, &client_data);
    if (ret < 0)        
        goto fail;

fail:    
    vPortFree(buf); 
    return ret;
}

/**
  * @brief      Send "keepalive" package by "post" method in every HTTP_POST_TIME_TICK.
  * @param      None
  * @return     None
  */
static void httpclient_test_keepalive_timeout_handle(TimerHandle_t tmr)
{
    HTTPCLIENT_RESULT ret = HTTPCLIENT_OK;

    if (post_count--) {
        ret = httpclient_test_keepalive_post();
        if (ret >= 0)                     
            return; // continue
    }
        
    // Stop and delete timer
    xTimerStop(tmr, 0);
    xTimerDelete(tmr, portMAX_DELAY);

    // Close http connection
    httpclient_close(&client);

    // Print final log
    if (ret >= 0)    
        LOG_I(http_client_keepalive_example, "example project test success.");
    else        
        LOG_I(http_client_keepalive_example, "example project test fail, reason:%d.", ret);
}

/**
  * @brief      Http client connect to test server and test "keepalive" function.
  * @param      None
  * @return     None
  */
static void httpclient_test_keepalive(void)
{
    HTTPCLIENT_RESULT ret = HTTPCLIENT_ERROR_CONN;
    char *post_url = HTTPS_MTK_CLOUD_POST_URL;
    TimerHandle_t tmr;
         
    // Connect to server.
    ret = httpclient_connect(&client, post_url);
    if (ret < 0)                     
        goto fail;

    // Send "keepalive" package by "post" method.
    ret = httpclient_test_keepalive_post();
    if (ret < 0)                     
        goto fail;

    // Create and start timer 
    tmr = xTimerCreate( "https_post_timer",
                        HTTPS_POST_TIME_TICK,
                        pdTRUE,
                        NULL,
                        httpclient_test_keepalive_timeout_handle);           
    xTimerStart(tmr, 0);
    return;
    
fail:
    // Close http connection
    httpclient_close(&client);

    // Print fail log
    LOG_I(http_client_keepalive_example, "example project test fail, reason:%d.", ret);
}

/**
  * @brief      Create a task for http client keepalive example
  * @param[in]  void *args: Not used
  * @return     None
  */
void app_entry(void *args)
{
    lwip_net_ready();

    // Httpclient "keepalive" feature test
    httpclient_test_keepalive();

    while (1) {        
        vTaskDelay(1000 / portTICK_RATE_MS); // Release CPU
    }
}

/**
  * @brief      Main program
  * @param      None
  * @return     0
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

    LOG_I(http_client_keepalive_example, "FreeRTOS Running");

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
    xTaskCreate(app_entry, APP_TASK_NAME, APP_TASK_STACKSIZE/sizeof(portSTACK_TYPE), NULL, APP_TASK_PRIO, NULL);


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


