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
#include "queue.h"
#include "sys_init.h"

#if MTK_WFC_WITH_LWIP_NO_WIFI_ENABLE
#include "ping.h"
#include "iperf_task.h"
#endif

#include "wifi_api.h"
//#include "wifi_os_api.h"
#include "wfc_stub.h"
#include "wfc_sdio.h"
 
#include "lwip/pbuf.h"

void AtoH(char *src, char *dest, int destlen);

int32_t wfc_at_os_cmd(uint8_t *pcmd,  uint32_t cmdSize)
{
    printf( "heap:\n");
    printf( "\ttotal: %u\n", configTOTAL_HEAP_SIZE);
    printf( "\tfree:  %u\n", xPortGetFreeHeapSize());
    printf( "\tlow:   %u\n", xPortGetMinimumEverFreeHeapSize());
    printf( "\r\n\r\n");    
    
    return 0;
}


#if MTK_WFC_WITH_LWIP_NO_WIFI_ENABLE
static uint8_t wfc_at_iperf_server(char *param)
{
    int i;
    char **g_iperf_param = NULL;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);
    char *p = NULL;
    int is_create_task = 0;
    //LOGI("wfc_at_iperf_server.\n");

    if (param == NULL) {
        return 0;
    }
    g_iperf_param = pvPortMalloc(IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE);
    if (g_iperf_param == NULL) {
        return 0;
    }

    i = 0;
    p = strtok(param, " ");
    while (p != NULL && i < 13) {
        strcpy((char *)&g_iperf_param[i * offset], p);
        i++;
        p = strtok(NULL, " ");
    }

    for (i = 0; i < 13; i++) {
        if (strncmp((char *)&g_iperf_param[i * offset], "-u", 2) == 0) {
            //printf("Iperf UDP Server: Start!\n");
            //printf("Iperf UDP Server Receive Timeout = 20 (secs)\n");
            xTaskCreate((TaskFunction_t)iperf_udp_run_server, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE / ((uint32_t)sizeof(StackType_t)), g_iperf_param, IPERF_TASK_PRIO, NULL);
            is_create_task = 1;
            break;
        }
    }

    if (strncmp((char *)&g_iperf_param[i * offset], "-u", 2) != 0) {
        printf("Iperf TCP Server: Start!");
        printf("Iperf TCP Server Receive Timeout = 20 (secs)");
        xTaskCreate((TaskFunction_t)iperf_tcp_run_server, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE / sizeof(portSTACK_TYPE), g_iperf_param, IPERF_TASK_PRIO , NULL);
        //xTaskCreate((TaskFunction_t)iperf_tcp_run_server, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE / sizeof(portSTACK_TYPE), g_iperf_param, 6 , NULL);
        is_create_task = 1;
    }

    if (is_create_task == 0) {
        vPortFree(g_iperf_param);
    }
    return 0;
}


static uint8_t wfc_at_iperf_client(char *param)
{
    int i;
    char **g_iperf_param = NULL;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);
    int is_create_task = 0;
    char *p = NULL;

    if (param == NULL) {
        return 0;
    }

    g_iperf_param = pvPortCalloc(1, IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE);
    if (g_iperf_param == NULL) {
        //printf("Warning: No enough memory to running iperf.\n");
        return 0;
    }


    i = 0;
    p = strtok(param, " ");
    while (p != NULL && i < 18) {
        strcpy((char *)&g_iperf_param[i * offset], p);
        i++;
        p = strtok(NULL, " ");
    }

    for (i = 0; i < 18; i++) {
        if (strncmp((char *)&g_iperf_param[i * offset], "-u", 2) == 0) {
            printf("iperf run udp client\n");
            xTaskCreate((TaskFunction_t)iperf_udp_run_client, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE / sizeof(portSTACK_TYPE), g_iperf_param, IPERF_TASK_PRIO , NULL);
            is_create_task = 1;
            break;
        }
    }

    if (strncmp((char *)&g_iperf_param[i * offset], "-u", 2) != 0) {
        printf("Iperf TCP Client: Start!");
#if (CFG_CONNSYS_TRX_BALANCE_EN == 1)
        xTaskCreate((TaskFunction_t)iperf_tcp_run_client, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE / sizeof(portSTACK_TYPE), g_iperf_param, IPERF_TASK_PRIO , &g_balance_ctr.tx_handle);
#else
        xTaskCreate((TaskFunction_t)iperf_tcp_run_client, IPERF_TASK_NAME, IPERF_TASK_STACKSIZE / sizeof(portSTACK_TYPE), g_iperf_param, IPERF_TASK_PRIO , NULL);
#endif
        is_create_task = 1;
    }

    if (is_create_task == 0) {
        vPortFree(g_iperf_param);
    }

    return 0;
}


int32_t wfc_at_iperf_cmd(uint8_t *pcmd,  uint32_t cmdSize)
{
    char *param = NULL;    
    int32_t ret = 0;
        
    param = strtok((char *)pcmd, "AT+RMT_IPERF=");
    if (param != NULL) {
        int len = strlen((char *)(param + 3));
        if (param[0] == '-' && param[1] == 'c') {
            char *str = (char *)pvPortMalloc(len + 1);
            str[len] = '\0';
            memcpy(str, param + 3, len);
            wfc_at_iperf_client(str);
            vPortFree(str);
        } else if (param[0] == '-' && param[1] == 's') {
            char *str = (char *)pvPortMalloc(len + 1);
            str[len] = '\0';
            memcpy(str, param + 3, len);
            wfc_at_iperf_server(str);
            vPortFree(str);
        } else 
            return 1;
    }    
    
    return ret;
}
#endif


int32_t wfc_at_wifi_cmd(uint8_t *pcmd,  uint32_t cmdSize)
{
    char *param = NULL;
    char *key = NULL;    
    uint8_t port = 0;    // STA port
    uint8_t auth = 0;
    uint8_t encrypt = 0;
    wifi_wep_key_t wep_key = {{{0}}};    
    int32_t ret = 0;
                
    //printf( "<AT>: Cmd(%s), Size(%lu)\r\n",pcmd, cmdSize);
 
    if (strstr((char *)pcmd, "AT+WIFISTA=connect") != NULL) {
        //LOGI("AT+WIFISTA=connect,s<ssid>,a<authMode>,e<encryption>,p<key>,<wep_key_index>\n");
        param = strtok((char *)pcmd, "s");
        param = strtok(NULL, ",");
        //LOGI("ssid=%s", param);
        ret = wifi_config_set_ssid(port, (uint8_t *)param, strlen(param));
        if (ret < 0) {
            printf("Set ssid failed\n");
            ret = 1;
        }
        param = strtok(NULL, ",");
        param = param + 1;
        auth = atoi(param);
        //LOGI("auth=%d", auth);
        param = strtok(NULL, ",");
        param = param + 1;
        encrypt = atoi(param);
        //LOGI("encrypt=%d", encrypt);
        ret = wifi_config_set_security_mode(port, (wifi_auth_mode_t)auth, (wifi_encrypt_type_t)encrypt);
        if (ret < 0) {
            printf("Set auth mode failed\n");
            ret = 1;
        }
        if ((auth == 0) && (encrypt == 0)) {
            //LOGI("WEP encrypt\n\n");
            param = strtok(NULL, ",");
            param = param + 1;
            key = param;
            //LOGI("WEP key=%s", key);
            param = strtok(NULL, ",");
            param = param + 1;
            wep_key.wep_tx_key_index = atoi(param);
            //LOGI("WEP key index=%d", wep_key.wep_tx_key_index);
            if (strlen(key) == 10 || strlen(key) == 26) {
                wep_key.wep_key_length[wep_key.wep_tx_key_index] = strlen(key) / 2;
                AtoH((char *)key, (char *)&wep_key.wep_key[wep_key.wep_tx_key_index], (int)wep_key.wep_key_length[wep_key.wep_tx_key_index]);
            } else if (strlen(key) == 5 || strlen(key) == 13) {
                memcpy(wep_key.wep_key[wep_key.wep_tx_key_index], key, strlen(key));
                wep_key.wep_key_length[wep_key.wep_tx_key_index] = strlen(key);
            } else {
                printf("invalid length of value.\n");
                return 1;
            }

            ret = wifi_config_set_wep_key(port, &wep_key);

        } else if ((auth != 0) && (encrypt != 1)) {
            param = strtok(NULL, ",");
            param = param + 1;
            //LOGI("password=%s", param);
            ret = wifi_config_set_wpa_psk_key(port, (uint8_t *)param, strlen(param));
        }
        ret = wifi_config_reload_setting();
        if (ret < 0) {
            printf("Connect to network failed\n");
            ret = 1;
        }
    }          
    
    return ret;         
}

#if MTK_WFC_WITH_LWIP_NO_WIFI_ENABLE
static void ping_request_result(ping_result_t *result)
{
    printf("Packets: Sent = %d, Received =%d, Lost = %d (%d%% loss)\n\r", (int)result->total_num, (int)result->recv_num, (int)result->lost_num, (int)((result->lost_num * 100) / result->total_num));
    printf("Packets: min = %d, max =%d, avg = %d\n", (int)result->min_time, (int)result->max_time, (int)result->avg_time);

}

int32_t wfc_at_ping_cmd(uint8_t *pcmd,  uint32_t cmdSize)
{
    char *param = NULL;
    int count = 3;
    int pktsz = 64;
    char ip[16] = {0};
    char *p = NULL;
        
    param = strtok((char *)pcmd, "AT+RMT_PING=");
    if (param != NULL) {
        p = strtok(param, ",");
        strcpy(ip, p);
        p = strtok(NULL, ",");
        if (p != NULL) {
            count = atoi(p);
            p = strtok(NULL, ",");
            if (p != NULL) {
                pktsz = atoi(p);
            }
        }
    }
    //LOGI("AT CMD received IP:%s, count:%d, ptksz:%d", ip, count, pktsz);
    ping_request(count, ip, PING_IP_ADDR_V4, pktsz, ping_request_result);
    
    return 0;
}
#endif

int32_t wfc_at_process(uint8_t *pcmd,  uint32_t cmdSize)
{
    int32_t ret = 0;
                
    printf( "<AT>: Cmd(%s), Size(%lu)\r\n",pcmd, cmdSize);
 
    if (strstr((char *)pcmd, "AT+WIFISTA") != NULL) 
    {
        ret = wfc_at_wifi_cmd(pcmd, cmdSize);
    }

    if (strstr((char *)pcmd, "AT+RMTOS") != NULL) {
        ret = wfc_at_os_cmd(pcmd, cmdSize);
    }

#if MTK_WFC_WITH_LWIP_NO_WIFI_ENABLE
    if (strstr((char *)pcmd, "AT+RMT_PING") != NULL) {
        ret = wfc_at_ping_cmd(pcmd, cmdSize);
    }

    if (strstr((char *)pcmd, "AT+RMT_IPERF") != NULL) {
        ret = wfc_at_iperf_cmd(pcmd, cmdSize);
    }
#endif

    return ret;         
}
