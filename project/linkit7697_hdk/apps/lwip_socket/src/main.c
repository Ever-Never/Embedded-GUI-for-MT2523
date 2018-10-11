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
#include "sys_init.h"
#include "wifi_api.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "ethernetif.h"
#include "lwip/sockets.h"
#include "netif/etharp.h"
#include "portmacro.h"
#include "wifi_lwip_helper.h"
#include "task_def.h"

#define WIFI_SSID                ("SQA_TEST_AP")
#define WIFI_PASSWORD            ("77777777")

#define SOCK_TCP_SRV_PORT        6500
#define SOCK_UDP_SRV_PORT        6600
#define TRX_PACKET_COUNT         3

/**
  * Create the log control block for lwip socket example.
  * User needs to define their own log control blocks as project needs.
  * Please refer to the log dev guide under /doc folder for more details.
  */
log_create_module(lwip_socket_example, PRINT_LEVEL_INFO);

/**
  * @brief  Tcp client create socket, connect tcp server and send/receive data.
  * @param  None
  * @return int: test result of tcp_client
  */
static int tcp_client_test(void)
{
    int s;
    int ret;
    struct sockaddr_in addr;
    int count = 0;
    int rcv_len, rlen;
    char rcv_buf[32] = {0};
    struct netif *sta_if = netif_find_by_type(NETIF_TYPE_STA);

    vTaskDelay(5000);
    char send_data[] = "Hello Server!";

    LOG_I(lwip_socket_example, "tcp_client_test starts");

    memset(&addr, 0, sizeof(addr));
    addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = lwip_htons(SOCK_TCP_SRV_PORT);
    inet_addr_from_ipaddr(&addr.sin_addr, netif_ip4_addr(sta_if));

    /* Create the socket */
    s = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        LOG_I(lwip_socket_example, "TCP client create failed");
        goto idle;
    }

    /* Connect */
    ret = lwip_connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        lwip_close(s);
        LOG_I(lwip_socket_example, "TCP client connect failed");
        goto idle;
    }

    while (count < TRX_PACKET_COUNT) {
        /* Write something */
        ret = lwip_write(s, send_data, sizeof(send_data));
        LOG_I(lwip_socket_example, "TCP client write:ret = %d", ret);

        LOG_I(lwip_socket_example, "TCP client waiting for data...");
        rcv_len = 0;
        while (rcv_len < sizeof(send_data)) {  //sonar client
            rlen = lwip_recv(s, &rcv_buf[rcv_len], sizeof(rcv_buf) - 1 - rcv_len, 0);
            rcv_len += rlen;
        }
        LOG_I(lwip_socket_example, "TCP client received data:%s", rcv_buf);

        count++;
        vTaskDelay(2000);
    }

    /* close */
    ret = lwip_close(s);
    LOG_I(lwip_socket_example, "TCP client s close:ret = %d", ret);
    return ret;
idle:
    LOG_I(lwip_socket_example, "TCP client test completed");
    return -1;
}

/**
  * @brief  Tcp server create socket, wait for client connection and receive/send data
  * @param  None
  * @return None
  */
static void tcp_server_test(void)
{
    int s;
    int c;
    int ret;
    int rlen;
    struct sockaddr_in addr;
    char srv_buf[32] = {0};
    LOG_I(lwip_socket_example, "tcp_server_test starts");

    memset(&addr, 0, sizeof(addr));
    addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = lwip_htons(SOCK_TCP_SRV_PORT);
    addr.sin_addr.s_addr = lwip_htonl(IPADDR_ANY);

    /* Create the socket */
    s = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        LOG_I(lwip_socket_example, "TCP server create failed");
        goto done;
    }

    ret = lwip_bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        LOG_I(lwip_socket_example, "TCP server bind failed");
        goto clean;
    }

    ret = lwip_listen(s, 0);
    if (ret < 0) {
        LOG_I(lwip_socket_example, "TCP server listen failed");
        goto clean;
    }

    do {
        socklen_t sockaddr_len = sizeof(addr);
        c = lwip_accept(s, (struct sockaddr *)&addr, &sockaddr_len);
        if (c < 0) {
            LOG_I(lwip_socket_example, "TCP server accept error");
            break;   //connection request.
        }

        LOG_I(lwip_socket_example, "TCP server waiting for data...");
        while ((rlen = lwip_read(c, srv_buf, sizeof(srv_buf) - 1)) != 0) {
            if (rlen < 0) {
                LOG_I(lwip_socket_example, "read error");
                break;
            }
            srv_buf[rlen] = 0; //for the next statement - printf string.
            LOG_I(lwip_socket_example, "TCP server received data:%s", srv_buf);

            lwip_write(c, srv_buf, rlen);      // sonar server
        }

        lwip_close(c);
    }while(0);

clean:
    lwip_close(s);
    LOG_I(lwip_socket_example, "TCP server s close:ret = %d", ret);
done:
    LOG_I(lwip_socket_example, "TCP server test completed");
}

/**
  * @brief  Udp client create socket and send/receive data
  * @param  None
  * @return  int: test result of udp_client
  */
static int udp_client_test(void)
{
    int s;
    int ret;
    int rlen;
    struct sockaddr_in addr;
    int count = 0;
    char rcv_buf[32] = {0};
    char send_data[] = "Hello Server!";
    LOG_I(lwip_socket_example, "udp_client_test starts");
    struct netif *sta_if = netif_find_by_type(NETIF_TYPE_STA);

    memset(&addr, 0, sizeof(addr));
    addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = lwip_htons(SOCK_UDP_SRV_PORT);
    inet_addr_from_ipaddr(&addr.sin_addr, netif_ip4_addr(sta_if));

    /* Create the socket */
    s = lwip_socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        LOG_I(lwip_socket_example, "UDP client create failed");
        goto idle;
    }

    /* Connect */
    ret = lwip_connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        lwip_close(s);
        LOG_I(lwip_socket_example, "UDP client connect failed");
        goto idle;
    }

    while (count < TRX_PACKET_COUNT) {
        /* Write something */
        ret = lwip_write(s, send_data, sizeof(send_data));
        LOG_I(lwip_socket_example, "UDP client write:ret = %d", ret);

        LOG_I(lwip_socket_example, "UDP client waiting for server data...");
        rlen = lwip_read(s, rcv_buf, sizeof(rcv_buf) - 1);
        rcv_buf[rlen] = 0;
        LOG_I(lwip_socket_example, "UDP client received data:%s", rcv_buf);
        count++;
        vTaskDelay(2000);
    }

    /* Close */
    ret = lwip_close(s);
    LOG_I(lwip_socket_example, "UDP client s close:ret = %d", ret);
    return ret;
idle:
    LOG_I(lwip_socket_example, "UDP client test completed");
    return -1;
}

/**
  * @brief  Udp server create socket and receive/send data
  * @param  None
  * @return None
  */
static void udp_server_test(void)
{
    int s;
    int ret;
    struct sockaddr_in addr, clnt_addr;
    char rcv_buf[32] = {0};
    int count = 0;
    LOG_I(lwip_socket_example, "udp_server_test starts");

    memset(&addr, 0, sizeof(addr));
    addr.sin_len = sizeof(addr);
    addr.sin_family = AF_INET;
    addr.sin_port = lwip_htons(SOCK_UDP_SRV_PORT);
    addr.sin_addr.s_addr = lwip_htonl(IPADDR_ANY);

    /* Create the socket */
    s = lwip_socket(AF_INET, SOCK_DGRAM, 0);
    if (s < 0) {
        LOG_I(lwip_socket_example, "UDP server create failed");
        goto idle;
    }

    ret = lwip_bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        lwip_close(s);
        LOG_I(lwip_socket_example, "UDP server bind failed");
        goto idle;
    }

    while (count < TRX_PACKET_COUNT) {
        socklen_t clnt_len = sizeof(clnt_addr);
        ret = lwip_recvfrom(s, rcv_buf, sizeof(rcv_buf), 0, (struct sockaddr *)&clnt_addr, &clnt_len);
        if (ret <= 0) {
            lwip_close(s);
            LOG_I(lwip_socket_example, "UDP server recv failed");
            goto idle;
        }
        LOG_I(lwip_socket_example, "UDP server received data:%s", rcv_buf);

        lwip_sendto(s, rcv_buf, strlen(rcv_buf), 0, (struct sockaddr *)&clnt_addr, clnt_len);

        count++;
    }

    ret = lwip_close(s);
    LOG_I(lwip_socket_example, "UDP server s close:ret = %d", ret);
idle:
    LOG_I(lwip_socket_example, "UDP server test completed");
}

/**
  * @brief     Tcp server test entry
  * @param[in] void *not_used:Not used
  * @return    None
  */
static void tcp_server_thread(void *not_used)
{
    tcp_server_test();

    //Keep the task alive
    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS); // release CPU
    }
}

/**
  * @brief     Udp server test entry
  * @param[in] void *not_used:Not used
  * @return    None
  */
static void udp_server_thread(void *not_used)
{
    udp_server_test();

    //Keep the task alive
    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS); // release CPU
    }
}

/**
  * @brief     Tcp/Udp client test entry
  * @param[in] void *not_used:Not used
  * @return    None
  */
static void client_test_thread(void *not_used)
{
    int ret;
    ret = tcp_client_test();
    ret += udp_client_test();
    if (ret == 0) {
        LOG_I(lwip_socket_example, "example project test success.\n");
    }
    //Keep the task alive
    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS); // release CPU
    }
}

/**
  * @brief     Create a task for tcp/udp test
  * @param[in] void *args:Not used
  * @return    None
  */
static void user_entry(void *args)
{
    lwip_net_ready();
    LOG_I(lwip_socket_example, "Begin to create socket_sample_task");
    //xTaskHandle xHandle;
    if (pdPASS != xTaskCreate(tcp_server_thread,
                              SOCKET_TCPS_EXAMPLE_TASK_NAME,
                              SOCKET_EXAMPLE_TASK_STACKSIZE / sizeof(portSTACK_TYPE),
                              NULL,
                              SOCKET_EXAMPLE_TASK_PRIO,
                              NULL)) {
        LOG_I(lwip_socket_example, "Cannot create tcp_server_thread");
    }

    if (pdPASS != xTaskCreate(udp_server_thread,
                              SOCKET_UDPS_EXAMPLE_TASK_NAME,
                              SOCKET_EXAMPLE_TASK_STACKSIZE / sizeof(portSTACK_TYPE),
                              NULL,
                              SOCKET_EXAMPLE_TASK_PRIO,
                              NULL)) {
        LOG_I(lwip_socket_example, "Cannot create udp_server_thread");
    }

    if (pdPASS != xTaskCreate(client_test_thread,
                              SOCKET_CLINET_EXAMPLE_TASK_NAME,
                              SOCKET_EXAMPLE_TASK_STACKSIZE / sizeof(portSTACK_TYPE),
                              NULL,
                              SOCKET_EXAMPLE_TASK_PRIO,
                              NULL)) {
        LOG_I(lwip_socket_example, "Cannot create socket_sample_task");
    }

    LOG_I(lwip_socket_example, "Finish to create socket_sample_task");
    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS); // release CPU
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

    LOG_I(lwip_socket_example, "start to create task.");

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
                              USER_ENTRY_TASK_NAME,
                              USER_ENTRY_TASK_STACKSIZE / sizeof(portSTACK_TYPE),
                              NULL,
                              USER_ENTRY_TASK_PRIO,
                              NULL)) {
        LOG_E(lwip_socket_example, "create user task fail");
        return -1;
    }

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
