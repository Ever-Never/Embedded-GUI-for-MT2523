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
#include "task_def.h"

#include "sys_init.h"
#include "system_mt7687.h"
#include "wifi_api.h"

/* Lwip includes. */
#include "lwip/netif.h"
#include "lwip/autoip.h"
#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "ethernetif.h"
#include "netif/etharp.h"
#include "portmacro.h"
#include "wifi_lwip_helper.h"
#include "mld6.h"

/* mDNS includes. */
#include "mdns.h"
#include "dns_sd.h"

#define require_noerr(x,y) if(x) {goto y;}

#define WIFI_SSID                 ("SQA_TEST_AP")
#define WIFI_PASSWORD             ("77777777")

#define SERVICE_NAME              ("MIPlayer")
#define SERVICE_TYPE              ("_music._tcp")
#define SERVICE_DOMAIN            ("local")
#define SERVICE_PORT              (12345)

#define KEY_DEVICEID              ("deviceid")
#define KEY_SEED                  ("seed")
#define KEY_OS                    ("os")

static DNSServiceRef client = NULL;
static TXTRecordRef txtRecord;
static uint8_t seed = 1;


/**
  * Create the log control block for mdns example.
  * User needs to define their own log control blocks as project needs.
  * Please refer to the log dev guide under /doc folder for more details.
  */
log_create_module(mdns_example, PRINT_LEVEL_INFO);

static void print_user_action(const char* str)
{
    LOG_I(mdns_example, "**************************************************");
    LOG_I(mdns_example, "User Action:  %s", str);
    LOG_I(mdns_example, "**************************************************");
}

/**
  * @brief     mDNS print TXT Record info
  * @return    None
  */
static void mdns_txtrecord_info()
{
    // Get bytes length of TXT Record
    uint16_t length = TXTRecordGetLength(&txtRecord);

    // Get pointer of TXT Record
    const void *ptr = TXTRecordGetBytesPtr(&txtRecord);

    // Get key-value item count of TXT Record
    uint16_t count = TXTRecordGetCount(length, ptr);

    // Print TXT Record value
    char txtStr[100] = {0};
    strncpy(txtStr, ptr, length);
    LOG_I(mdns_example, "TXTRecord length=%d ptr=%s count=%d", length, txtStr, count);

    // Determine if TXT Record contains a specified key
    int ret = TXTRecordContainsKey(length, ptr, KEY_SEED);
    LOG_I(mdns_example, "TXTRecordContainsKey seed: %s\n", ret == 1 ? "yes" : "no");

    // Get value for a given key from TXT Record
    uint8_t valueLen = 0;
    const void * value = TXTRecordGetValuePtr(length, ptr, KEY_OS, &valueLen);
    char valueStr[20] = {0};
    strncpy(valueStr, value, valueLen);
    LOG_I(mdns_example, "TXTRecordGetValuePtr [os] value: %s strlen=%d valueLen=%d\n",
            valueStr, strlen(valueStr), valueLen);
}

/**
  * @brief     mDNS print TXT Record info
  * @return    None
  */
static DNSServiceErrorType mdns_update_txtrecord()
{
    // Remove Key-Value
    DNSServiceErrorType ret = TXTRecordRemoveValue(&txtRecord, KEY_OS);
    LOG_I(mdns_example, "TXTRecordRemoveValue [os] ret=%d", ret);
    // Update Key-Value (remove -> re-set)
    ret = TXTRecordRemoveValue(&txtRecord, KEY_SEED);
    LOG_I(mdns_example, "TXTRecordRemoveValue [seed] ret=%d", ret);
    char seedString[16];
    memset(seedString, 0, 16);
    snprintf(seedString, 16, "%d", ++seed);
    ret = TXTRecordSetValue(&txtRecord, KEY_SEED, strlen(seedString), seedString);
    LOG_I(mdns_example, "TXTRecordSetValue [seed=%d] ret=%d", seed, ret);

    // Update TXT Record
    ret = DNSServiceUpdateRecord(client,                                   // DNSServiceRef
                                 NULL,                                     // DNSRecordRef
                                 0,                                        // DNSServiceFlags
                                 TXTRecordGetLength(&txtRecord),           // txt record length
                                 TXTRecordGetBytesPtr(&txtRecord),         // txt record pointer
                                 0                                         // ttl
                                 );
    LOG_I(mdns_example, "DNSServiceUpdateRecord ret=%d", ret);
    return ret;
}

/**
  * @brief     mDNS Publish Service demo
  * @return    None
  */
static DNSServiceErrorType mdns_publish_service()
{
    DNSServiceErrorType err;
    DNSServiceFlags flags = 0;
    const char *device = "MT7687";
    const char *os = "FreeRTOS";
    char seedString[16];

    // Create TXT Record
    TXTRecordCreate(&txtRecord, 0, NULL);

    // Add Device ID
    err = TXTRecordSetValue(&txtRecord, KEY_DEVICEID, strlen(device), device);
    require_noerr(err, exit);

    // Add Seed Number
    memset(seedString, 0, 16);
    snprintf(seedString, 16, "%d", seed);
    err = TXTRecordSetValue(&txtRecord, KEY_SEED, strlen(seedString), seedString);
    require_noerr(err, exit);

    // Add OS
    err = TXTRecordSetValue(&txtRecord, KEY_OS, strlen(os), os);
    require_noerr(err, exit);

    LOG_I(mdns_example, "Register Bonjour Service: %s type: %s domain: %s port: %d",
            SERVICE_NAME, SERVICE_TYPE, SERVICE_DOMAIN, SERVICE_PORT);

    // Register Bonjour Service
    err = DNSServiceRegister(&client,                      // DNSServiceRef
            flags,                                         // DNSServiceFlags
            kDNSServiceInterfaceIndexAny,                  // interface index
            SERVICE_NAME,                                  // service name
            SERVICE_TYPE,                                  // service type
            SERVICE_DOMAIN,                                // domain
            NULL,                                          // host
            htons(SERVICE_PORT),                           // port
            TXTRecordGetLength(&txtRecord),                // txt record length
            TXTRecordGetBytesPtr(&txtRecord),              // txt record pointer
            NULL,                                          // callback
            NULL);                                         // context
    if (err == kDNSServiceErr_NoError) {
        LOG_I(mdns_example, "Register Bonjour Service successfully!");
    } else {
        LOG_E(mdns_example, "Register Bonjour Service failed %d.", err);
    }
    require_noerr(err, exit);
    return err;

exit:
    TXTRecordDeallocate(&txtRecord);
    return err;
}

/**
  * @brief     mDNS Daemon Task entry
  * @param[in] void *not_used:Not used
  * @return    None
  */
static void mdnsd_entry(void *not_used)
{
    LOG_I(mdns_example, "mdnsd_entry start");
    mdnsd_start();
    LOG_I(mdns_example, "mdnsd_entry return");
    client = NULL;
    vTaskDelete(NULL);
}

/**
  * @brief     Create mdnsd task for mdns test
  * @param[in] void *args:Not used
  * @return    None
  */
static void user_entry(void *args)
{
    lwip_net_ready();

    // Support IPv6 Address after link_up and Got IP
    struct netif *sta_if = netif_find_by_type(NETIF_TYPE_STA);
    ip6_addr_t mld_address;
#if LWIP_IPV6
    netif_create_ip6_linklocal_address(sta_if, 1);
    sta_if->ip6_autoconfig_enabled = 1;
    ip6_addr_set_solicitednode(&mld_address, netif_ip6_addr(sta_if, 0)->addr[3]);
    mld6_joingroup(netif_ip6_addr(sta_if, 0), &mld_address);
#endif

    // xTaskHandle create mDNS daemon task
    if (pdPASS != xTaskCreate(mdnsd_entry,
                              MDNSD_TASK_NAME,
                              MDNSD_TASK_STACKSIZE / sizeof(portSTACK_TYPE),
                              NULL,
                              MDNSD_ENTRY_TASK_PRIO,
                              NULL)) {
        LOG_I(mdns_example, "Cannot create mdnsd_task");
    }
    LOG_I(mdns_example, "Begin to create mdnsd_task");

    // Wait for Auto IP (No DHCP server case), then update interface
    struct dhcp *dhcp = sta_if->dhcp;
    if (dhcp->autoip_coop_state == DHCP_AUTOIP_COOP_STATE_ON) {
        LOG_I(mdns_example, "Auto IP STATE_ON");
        do {
            LOG_I(mdns_example, "Wait until got ip address");
            vTaskDelay(2 * 1000 / portTICK_PERIOD_MS);
        } while (dhcp->state != DHCP_STATE_BOUND && sta_if->autoip->state != AUTOIP_STATE_BOUND);
        LOG_I(mdns_example, "Update Interface Start");
        mdns_update_interface();
        LOG_I(mdns_example, "Update Interface End");
    }

    // Publish mDNS Service
    vTaskDelay(10 * 1000 / portTICK_RATE_MS);
    DNSServiceErrorType err = mdns_publish_service();
    if (err == kDNSServiceErr_NoError) {
        print_user_action("Please use Bonjour Browser Application to discover/resolve the service in 10 seconds");
    }

    // Print TXT Record info
    vTaskDelay(10 * 1000 / portTICK_RATE_MS);
    mdns_txtrecord_info();

    // Update TXT Record
    err += mdns_update_txtrecord();
    if (err == kDNSServiceErr_NoError) {
        print_user_action("Please verify the updated TXT Record in 10 seconds");
    }

    // Unregister/Release Service
    vTaskDelay(10 * 1000 / portTICK_RATE_MS);
    LOG_I(mdns_example, "DNSServiceRefDeallocate start");
    TXTRecordDeallocate(&txtRecord);
    DNSServiceRefDeallocate(client);
    LOG_I(mdns_example, "DNSServiceRefDeallocate end");
    print_user_action("Please verify that the service has deregistered/disappeared in 10 seconds");

    // Stop mdnsd task until completion or timeout after 10 seconds
    vTaskDelay(10 * 1000 / portTICK_RATE_MS);
    LOG_I(mdns_example, "Stop mdnsd task");
    mdnsd_stop();

    // Verify result
    LOG_I(mdns_example, "user_entry err=%d client=%d", err, client);
    if (err == kDNSServiceErr_NoError && client == NULL) {
        LOG_I(mdns_example, "example project test success.\n");
    } else {
        LOG_I(mdns_example, "example project test fail.\n");
    }

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

    LOG_I(mdns_example, "start to create task.");
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
        LOG_E(mdns_example, "create user task fail");
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
