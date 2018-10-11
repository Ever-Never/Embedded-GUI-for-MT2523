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
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"
#include "syslog.h"

#include "sys_init.h"
#include "wifi_api.h"
#include "wifi_lwip_helper.h"

#include "httpd.h"
#include "httpclient.h"

static xQueueHandle mbox;

#define WIFI_SSID                ("SQA_TEST_AP")
#define WIFI_PASSWORD            ("77777777")

#define BUF_SIZE        (1024 * 4)
#define HTTP_GET_URL    "http://127.0.0.1/"

log_create_module(httpd_example, PRINT_LEVEL_INFO);

/**
  * @brief      http server feedback handle
  * @param      None
  * @return     0, if http server run.\n
  *             1, if other status occurred.\n
  */
static int httpd_test_fb_handle(void)
{
    httpd_fb fb;

    LOG_I(httpd_example, "httpd_test_fb_handle()");

    for (;;) {
        if (xQueueReceive(mbox, (void *)&fb, portMAX_DELAY) == pdPASS) {
            LOG_I(httpd_example, "httpd status = %d", fb.status);

            switch (fb.status) {
                case HTTPD_STATUS_RUN:
                    return 0;  // HTTP server is running.
                case HTTPD_STATUS_STOPPING:
                case HTTPD_STATUS_STOP:
                    break; // HTTP server is stopping or has stopped.
                case HTTPD_STATUS_UNINIT:
                    return 1;  // Uninitialized.
            }
        }
    }
}

/**
  * @brief      Polling http server status
  * @param      None
  * @return     0, if http server stopped.\n
  *             1, if other status occurred.\n
  */
static int httpd_test_poll_status(void)
{
    HTTPD_STATUS status = HTTPD_STATUS_UNINIT;

    LOG_I(httpd_example, "httpd_test_poll_status()");

    for (;;) {
        vTaskDelay(1000); // Delay by 1000ms.

        status = httpd_get_status();

        switch (status) {
            case HTTPD_STATUS_STOP:
                return 0;  // HTTP server has stopped.
            case HTTPD_STATUS_STOPPING:
            case HTTPD_STATUS_RUN:
                break; // HTTP server is running.
            case HTTPD_STATUS_UNINIT:
                return 1;  // Uninitialized.
        }
    }
}

/**
  * @brief      Access webpage by HTTP request
  * @param      None
  * @return     0, if success.\n
  *             other, if fail.\n
  */
static int httpd_test_access_webpage(void)
{
    char *url = HTTP_GET_URL;
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf;
    HTTPCLIENT_RESULT result;

    LOG_I(httpd_example, "httpd_test_access_webpage()");

    buf = pvPortMalloc(BUF_SIZE);
    if (buf == NULL) {        
        LOG_I(httpd_example, "memory malloc fail.");
        return 1;
    }
    
    client_data.response_buf = buf;
    client_data.response_buf_len = BUF_SIZE;
    client.auth_user = "admin";
    client.auth_password = "admin";

    // httpclient "get"
    result = httpclient_get(&client, url, &client_data);
    LOG_I(httpd_example, "data received: %s", client_data.response_buf);
    
    vPortFree(buf);

    if(result >= 0)
        return 0;
    else {        
        LOG_I(httpd_example, "example project test fail, reason:%d.", result);
        return 1;
    }
}

/**
  * @brief      Http server init, start and stop flow.
  * @param      None
  * @return     None
  */
static void httpd_test(void)
{
    /*
     *  The webpages and settings can be customized.
     *
     *  The webpages on the server is stored in middleware/third_party/httpd/webhtml folder.
     *  The web server settings are defined in middleware/third_party/httpd/inc/config.h.
     */
    HTTPD_RESULT result;
    httpd_para parameter;

    LOG_I(httpd_example, "httpd_test()");

    // Firstly, initialize the HTTP server.
    result = httpd_init();

    switch (result) {
        case HTTPD_RESULT_SUCCESS:   /**<  Initialization was successful. */
            break;
        case HTTPD_RESULT_WAITING:   /**<  Needs to wait for the feedback. */
        case HTTPD_RESULT_UNINIT:   /**<  HTTP server is uninitialized. */
        case HTTPD_RESULT_QUEUE_FULL: /**<  The queue is full. */
            LOG_I(httpd_example, "init failed.");
            return ;
    }

    /*
     * Secondly, start the HTTP server.
     * After starting the HTTP server, the status can asynchronously queried, 
     * either through the message to listen to the status or through the polling.
     */
    mbox = xQueueCreate(4, sizeof(httpd_fb));
    if (mbox == NULL) {
        LOG_I(httpd_example, "xQueueCreate failed.");
        return;
    }

    memset(&parameter, 0, sizeof(httpd_para));
    parameter.fb_queue = mbox;
    result = httpd_start(&parameter);

    switch (result) {
        case HTTPD_RESULT_SUCCESS: // Successful.
            break;
        case HTTPD_RESULT_UNINIT: // Uninitialized.
        case HTTPD_RESULT_QUEUE_FULL: // Mbox queue is full.
            LOG_I(httpd_example, "httpd start failed.");
            return;
        case HTTPD_RESULT_WAITING: {// Need to wait for the feedback of the status notification or to poll the status.
            if (httpd_test_fb_handle()) {
                LOG_I(httpd_example, "httpd start failed.");
                return;
            }
            break;
        }
    }

    // Right now, HTTP server has started. The user can browse the webpage.
    if(httpd_test_access_webpage())
        return;
    
    // Finally, the following flow can be used to stop HTTP server.
    result = httpd_stop();

    switch (result) {
        case HTTPD_RESULT_SUCCESS: // Successful.
            break;
        case HTTPD_RESULT_UNINIT: // Uninitialized.
        case HTTPD_RESULT_QUEUE_FULL: // Mbox queue is full.
            LOG_I(httpd_example, "httpd stop failed.");
            return;
        case HTTPD_RESULT_WAITING: {// Need to wait for the feedback of the status notification or to poll the status.
            if (httpd_test_poll_status()) {
                LOG_I(httpd_example, "httpd stop failed.");
                return;
            }
            break;
        }

    }

    // Print success log
    LOG_I(httpd_example, "example project test success.");
    return;
}

/**
  * @brief      Create a task for httpd example
  * @param[in]  void *args: Not used
  * @return     None
  */
static void app_entry(void *args)
{
    lwip_net_ready();

    // Httpd feature test
    httpd_test();

    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS); // release CPU
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

    LOG_I(httpd_example, "FreeRTOS Running");

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


