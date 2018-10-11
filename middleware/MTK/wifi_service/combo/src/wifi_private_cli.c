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
/**
    @file       wifi_private_cli.c
    @brief      WiFi - Configure API Examples, these CLI in the wifi_private_api.c
                is only for internal use, it will be removed or modified in future.

    @history    2016/07/27  Initial for 1st draft  (Pengfei Qiu)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "os.h"
#include "os_util.h"
#include "FreeRTOS.h"
#include "wifi_api.h"
#include "wifi_private_api.h"
#include "wifi_scan.h"
#include "ethernet_filter.h"
#include "inband_queue.h"
#include "get_profile_string.h"
#include "queue.h"
#include "task.h"
#include "task_def.h"
#include "wifi_private_cli.h"
#include "inband_queue_option.h"
#if defined(MTK_MINICLI_ENABLE)
#include "cli_def.h"
#include "cli.h"
#include "wifi_cli.h"
#endif

#if defined(MTK_MINISUPP_ENABLE)
#if defined(MTK_WIFI_WPS_ENABLE)
#include "driver_inband.h"
#endif
#endif

#ifdef MTK_SINGLE_SKU_SUPPORT
#include "nvdm.h"
#endif

#ifdef MTK_WIFI_ROM_ENABLE
#include "mlme_interface.h"
#endif

#ifdef MTK_ANT_DIV_ENABLE
#include "wifi_ant_div.h"
#endif

#define WIFI_CLI_RETURN_STRING(ret) ((ret>=0)?"Success":"Error")
#define WIFI_CLI_CALC_PER_BEFORE_DOT(x, y) ( (y>0) ? ( 100*(x) / (y) ) : 0 )
#define WIFI_CLI_CALC_PER_AFTER_DOT(x, y)  ( (y>0) ? ( ( 100*(x) % (y) )*100 / (y) ) : 0 )

uint8_t lwip_ip_cli(uint8_t len, char *param[]);

#ifdef MTK_CM4_SCAN_TABLE_ENABLE
/**
* @brief Example of Set WiFi Scan Table Size
* wifi config set scantabsize <size>
* @param [IN]num 1 ~ 255
*
* @return  =0 means success, >0 means fail
*/
static uint8_t wifi_config_set_scan_tab_size_ex(uint8_t len, char *param[]);

/**
* @brief Example of Get WiFi Scan Table Size
* wifi config get scantabsize
* @param [OUT]num 1 ~ 255
*
* @return  =0 means success, >0 means fail
*/
static uint8_t wifi_config_get_scan_tab_size_ex(uint8_t len, char *param[]);
#endif

/**
* @brief
* @parameter
* @return =0 means success, >0 means fail
*/
#ifndef MTK_CLI_SLIM_FOR_32
uint8_t wifi_config_get_rssi_threshold_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t enable = 0;
    int8_t rssi = 0;

    if((ret = wifi_inband_get_rssi_threshold(&enable, &rssi)) >= 0){
        printf("rssi threthold: [enabled: %d], [value: %d]\n", enable, rssi);
    } else {
        status = 1;
    }
    printf("wifi_inband_get_rssi_threshold: ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

/**
* @brief
* @parameter
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_rssi_threshold_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t enable = 0;
    int8_t rssi = 0;

    if( len < 2 ){
        printf("Usage:  wifi config set rssi_threshold <enabled> <rssi value> \n");
        return 1;
    }

    enable = (uint8_t)atoi(param[0]);
    rssi = (int8_t)atoi(param[1]);

    ret = wifi_inband_set_rssi_threshold(enable, rssi);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_inband_set_rssi_threshold: ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}
#endif
int32_t wifi_frame_type_event_handler(wifi_event_t event,
                                                    uint8_t *payload,
                                                    uint32_t length)
{
    uint8_t type = 0;
    if(payload == NULL) {
        LOG_E(wifi, "payload is empty!");
        return -1;
    }
    LOG_HEXDUMP_E(wifi, "event id: %d", payload, length, event);
    type = payload[0]>>4;
    switch (type) {
        case 0: printf("frame type is assoc req\r\n");break;
        case 1: printf("frame type is assoc resp\r\n");break;
        case 2: printf("frame type is reassoc req\r\n");break;
        case 3: printf("frame type is reassoc resp\r\n");break;
        case 4: printf("frame type is probe req\r\n");break;
        case 5: printf("frame type is probe resp\r\n");break;
        case 8: printf("frame type is beacon\r\n");break;
        case 9: printf("frame type is atim\r\n");break;
        case 10: printf("frame type is disassoc\r\n");break;
        case 11: printf("frame type is auth\r\n");break;
        case 12: printf("frame type is deauth\r\n");break;
        default:
            printf("wrong management frame\r\n");
            break;
    }
    return 1;
}

int32_t wifi_frame_type_event_advanced_handler(wifi_event_t event,
                                                    uint8_t *payload,
                                                    uint32_t length)
{
    uint8_t type = 0;
    wifi_data_parse_t data;
    if(payload == NULL) {
        LOG_E(wifi, "payload is empty!");
        return -1;
    }
    wifi_connection_parse_data_descriptor(payload, &data);
    printf("data RSSI: %d, data rate: %d\n",(int)data.rssi,(int)data.data_rate);
    type = (data.mac_header[0])>>4;
    switch (type) {
        case 0: printf("frame type is assoc req\r\n");break;
        case 1: printf("frame type is assoc resp\r\n");break;
        case 2: printf("frame type is reassoc req\r\n");break;
        case 3: printf("frame type is reassoc resp\r\n");break;
        case 4: printf("frame type is probe req\r\n");break;
        case 5: printf("frame type is probe resp\r\n");break;
        case 8: printf("frame type is beacon\r\n");break;
        case 9: printf("frame type is atim\r\n");break;
        case 10: printf("frame type is disassoc\r\n");break;
        case 11: printf("frame type is auth\r\n");break;
        case 12: printf("frame type is deauth\r\n");break;
        default:
            printf("wrong management frame\r\n");
            break;
    }
    return 1;
}

/**
* @brief Example of set frame filter for packets format wanted to be received
* wifi config set frame_filter <enable> <frame_type> <vender_ie.element_id> <vender_ie.OUI[0]> <vender_ie.OUI[1]> <vender_ie.OUI[2]>
* wifi config set frame_filter 1 8 221 0 15 172
* @parameter
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_frame_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t enable = 0;
    uint16_t frame_type = 0;
    extra_vender_ie_t vender_ie = {0};

    if( len < 2 ){
        printf("Usage:  wifi config set frame_filter <enable> <frame_type> [<vender_ie.element_id> <vender_ie.OUI[0]> <vender_ie.OUI[1]> <vender_ie.OUI[2]>] \n");
        return 1;
    }

    wifi_connection_register_event_handler(WIFI_EVENT_IOT_REPORT_FILTERED_FRAME, wifi_frame_type_event_handler);
    enable = (uint8_t)atoi(param[0]);
    frame_type = (uint16_t)atoi(param[1]);
    if (2 == len) {
        ret = wifi_config_set_frame_filter(enable, frame_type, NULL);
    }
    else {
        vender_ie.element_id = (uint8_t)atoi(param[2]);
        vender_ie.OUI[0] = (uint8_t)atoi(param[3]);
        vender_ie.OUI[1] = (uint8_t)atoi(param[4]);
        vender_ie.OUI[2] = (uint8_t)atoi(param[5]);
        ret = wifi_config_set_frame_filter(enable, frame_type, &vender_ie);
    }
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_frame_filter: ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

/**
* @brief Example of set frame filter for packets format wanted to be received
* wifi config set frame_filter_advanced <enable> <frame_type> <vender_ie.element_id> <vender_ie.OUI[0]> <vender_ie.OUI[1]> <vender_ie.OUI[2]>
* wifi config set frame_filter_advanced 1 8 221 0 15 172
* @parameter
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_frame_filter_advanced_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t enable = 0;
    uint16_t frame_type = 0;
    extra_vender_ie_t vender_ie = {0};

    if( len < 2 ){
        printf("Usage:  wifi config set frame_filter_advanced <enable> <frame_type> [<vender_ie.element_id> <vender_ie.OUI[0]> <vender_ie.OUI[1]> <vender_ie.OUI[2]>] \n");
        return 1;
    }

    wifi_connection_register_event_handler(WIFI_EVENT_IOT_REPORT_FILTERED_FRAME, wifi_frame_type_event_advanced_handler);
    enable = (uint8_t)atoi(param[0]);
    frame_type = (uint16_t)atoi(param[1]);
    if (2 == len) {
        ret = wifi_config_set_frame_filter_advanced(enable, frame_type, NULL);
    }
    else {
        vender_ie.element_id = (uint8_t)atoi(param[2]);
        vender_ie.OUI[0] = (uint8_t)atoi(param[3]);
        vender_ie.OUI[1] = (uint8_t)atoi(param[4]);
        vender_ie.OUI[2] = (uint8_t)atoi(param[5]);
        ret = wifi_config_set_frame_filter_advanced(enable, frame_type, &vender_ie);
    }
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_frame_filter_advanced: ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}


#define EXT_SEC_CMD_SET_AUTH_MODE    0x00
#define EXT_SEC_CMD_SET_ENCR_TYPE    0x01
#define EXT_SEC_CMD_SET_WPAPSK       0x02
#define EXT_SEC_CMD_SET_WEP_KEY      0x03
#define EXT_SEC_CMD_MAX              0x04

typedef struct _EXT_SECURITY_CMD_T {
    UINT16     u2ExtCmdID;
    UINT16     u2DataLen;
    UINT      aucDataBuffer[1];
} EXT_SECURITY_CMD_T, *P_EXT_SECURITY_CMD_T;

/**
* @brief Set N9 Debug Level
* wifi config set n9dbg <dbg_level>
* @param [IN]dbg_level
*   0: None
*   1: ERROR
*   2. WARNING
*   3. TRACE
*   4. INFO
*   5. LAUD
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_set_n9_dbg_level(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t dbg_level = 0;

    if( len < 1 ){
        printf("Usage:  wifi config set n9dbg <dbg_level> \n");
        return 1;
    }

    dbg_level = atoi(param[0]);
    ret = wifi_inband_set_debug_level(dbg_level);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_n9_dbg_level, level = %d, ret:%s, Code=%ld\n", dbg_level, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

#ifndef MTK_CLI_SLIM_FOR_32

/**
* @brief Get N9 Debug Level
* wifi config get n9dbg
* @param [OUT]dbg_level
*   0: None
*   1: ERROR
*   2. WARNING
*   3. TRACE
*   4. INFO
*   5. LAUD
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_get_n9_dbg_level(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t dbg_level = 0;

    ret = wifi_inband_get_debug_level(&dbg_level);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_n9_dbg_level, level = %d, ret:%s, Code=%ld\n",
           dbg_level, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

#endif
extern wifi_scan_list_item_t *g_scan_list;
extern uint8_t g_scan_list_size;


static wifi_scan_list_item_t g_ap_list[8] = {{0}};
void parse_channel_list_from_string(const char *channel_list, ch_list_t *channel_item, uint8_t band)
{
    char string_data[4] = {0};
    uint32_t index = 0;
    uint32_t save_index = 0;
    uint32_t count = 0;
    uint32_t channel_section_count = 0;

    while ('\0' != channel_list[index]) {
        if ('|' == channel_list[index]) {
            channel_item->triplet[channel_section_count + channel_item->num_bg_band_triplet].channel_prop = (char)atoi((char *)string_data);
            channel_section_count++;
            os_memset(string_data, 0, sizeof(string_data));
            save_index = 0;
            count = 0;
        } else if (',' == channel_list[index]) {
            switch(count) {
            case 0: {
                channel_item->triplet[channel_section_count + channel_item->num_bg_band_triplet].first_channel = (char)atoi((char *)string_data);
                break;
            }
            case 1: {
                channel_item->triplet[channel_section_count + channel_item->num_bg_band_triplet].num_of_ch = (char)atoi((char *)string_data);
                break;
            }
            default:
                return;
            }
            os_memset(string_data, 0, sizeof(string_data));
            save_index = 0;
            count++;
        } else {
            string_data[save_index] = channel_list[index];
            save_index++;
        }
        index++;
    }
    channel_item->triplet[channel_section_count].channel_prop = (char)atoi((char *)string_data);
    if (WIFI_BAND_2_4_G == band) {
        channel_item->num_bg_band_triplet = channel_section_count + 1;
    } else if (WIFI_BAND_5_G == band) {
        channel_item->num_a_band_triplet = channel_section_count + 1;
    }
}


/**
* @brief Example of Start/Stop WiFi Scanning
* wifi connect set scan <start/stop> <mode> <operation> [ssid] [bssid]
* @param [IN]ssid SSID, "NULL" means not specified
* @param [IN]bssid BSSID, "NULL" means not specified
* @param [IN]flag
* @param 0 Active Scan
* @param 1 Passive Scan
*
* @return  =0 means success, >0 means fail
*
* @note When SSID/BSSID specified (not NULL)
* @note 1. ProbeReq carries SSID (for Hidden AP)
* @note 2. Scan Tab always keeps the APs of this specified SSID/BSSID
*/
uint8_t wifi_connect_scan_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    ch_list_t *scan_channel_list = NULL;
    uint8_t start = 0;
    int32_t len_param = 0;
    P_CMD_IOT_START_SCAN_T scan_param = NULL;

    if( len < 1 ){
        printf("Usage:  wifi connect set scan <start/stop> <mode> <operation> [ssid] [bssid] \n");
        return 1;
    }

    start = atoi(param[0]);
    scan_channel_list=(ch_list_t *)os_malloc(sizeof(ch_list_t)+6*sizeof(ch_desc_t));
    os_memset(scan_channel_list, 0, sizeof(ch_list_t)+6*sizeof(ch_desc_t));

    if (!start) {
        ret = wifi_connection_stop_scan();
    } else {
        uint8_t scan_mode = atoi(param[1]);
        uint8_t scan_option = atoi(param[2]);

        char *chanel_string_2g = NULL;
        char *chanel_string_5g = NULL;
        char *ssid = NULL;
        uint8_t ssid_len = 0;
        uint8_t bssid_val[WIFI_MAX_NUMBER_OF_STA] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
        uint8_t *bssid = (uint8_t *)bssid_val;

        if (len >= 4) {
            chanel_string_2g = param[3];
            if (len == 5) {
                chanel_string_5g = param[4];
                ssid = NULL;
                ssid_len = 0;
                bssid = (uint8_t *)(NULL);
            } else if (len == 6) {
                chanel_string_5g = param[4];
                ssid = param[5];
                ssid_len = os_strlen(ssid);
                bssid = (uint8_t *)(NULL);
            } else if (len >6) {
                chanel_string_5g = param[4];
                ssid = param[5];
                ssid_len = os_strlen(ssid);
                wifi_conf_get_mac_from_str((char *)bssid_val, param[6]);
            }
        } else {
            chanel_string_2g = NULL;
            chanel_string_5g = NULL;
            ssid = NULL;
            ssid_len = 0;
            bssid = (uint8_t *)(NULL);
        }

        if (chanel_string_2g != NULL && strlen(chanel_string_2g) > 4) {
            parse_channel_list_from_string(chanel_string_2g, scan_channel_list, WIFI_BAND_2_4_G);
        }

        if (chanel_string_5g != NULL && strlen(chanel_string_5g) > 4) {
            parse_channel_list_from_string(chanel_string_5g, scan_channel_list, WIFI_BAND_5_G);
        }
        //hex_dump("scan_channel_list",scan_channel_list, sizeof(CH_LIST_T)+6*sizeof(CH_DESC_T));

        len_param = (scan_channel_list->num_a_band_triplet + scan_channel_list->num_bg_band_triplet) * sizeof(ch_desc_t) + sizeof(CMD_IOT_START_SCAN_T);
        scan_param = (P_CMD_IOT_START_SCAN_T)os_malloc(len_param);
        os_memset((unsigned char *)(scan_param), 0x0, len_param);

        /******************reset scan list*******************************/
        wifi_connection_scan_init(g_ap_list, 8);
        //printf("size = %d", sizeof(wifi_scan_list_item_t));
        /******************reset scan list*******************************/


        if (g_scanning && g_scan_by_supplicant) {
            wifi_connection_stop_scan();
            g_scanning = 0;
        }
        g_scan_by_supplicant = 0;
        g_scan_by_app = 1;


        if (ssid && (ssid_len > 0)) {
            os_memcpy(scan_param->SSID, ssid, ssid_len);
            scan_param->enable_ssid = (0x80 | ssid_len);
        }

        if (bssid) {
            os_memcpy(scan_param->BSSID, bssid, WIFI_MAC_ADDRESS_LENGTH);
            scan_param->enable_bssid = 1;
        }

        if (scan_mode > 0) {
            scan_param->partial = 1;
            scan_param->partial_bcn_cnt = 1;
        }
        scan_param->scan_mode = scan_option;
        os_memcpy(&scan_param->channel_list, scan_channel_list, (scan_channel_list->num_a_band_triplet + scan_channel_list->num_bg_band_triplet)* sizeof(ch_desc_t)+2);

        ret = inband_queue_exec(WIFI_COMMAND_ID_IOT_START_SCAN, (unsigned char *)(scan_param), len_param, TRUE, FALSE);
        os_free(scan_param);
        if (ret == 0) {
            g_scanning = 1;
        }

    }

    if (ret < 0) {
        status = 1;
    }
    printf("wifi_connect_scan_ex(), ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    if( NULL!=scan_channel_list ){
        os_free(scan_channel_list);
    }

    return status;
}

#ifndef MTK_CLI_SLIM_FOR_32
/**
* @brief Example of Get WiFi TX Power
* wifi config get tx_power
* @get power: 64~190
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_tx_power_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t tx_power = 0;

    ret = wifi_config_get_tx_power(&tx_power);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_tx_power_ex tx_power:%d, ret:%s, Code=%ld\n", tx_power, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}
#endif
/**
* @brief Example of Set WiFi TX Power
* wifi config set tx_power <tx power>
* @param [IN]interval 64 ~ 190
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_tx_power_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t tx_power = 0;

    if( len < 1 ){
        printf("Usage:  wifi config set tx_power <tx power> \n");
        return 1;
    }

    tx_power = atoi(param[0]);
    ret = wifi_config_set_tx_power(tx_power);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_tx_power_ex tx_power:%d, ret:%s, Code=%ld\n", tx_power, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

static xQueueHandle n9log_queue = NULL;
static xTaskHandle n9log_task = NULL;

log_create_module(n9log, PRINT_LEVEL_INFO);

int wifi_n9log_handler(unsigned char *body, int len)
{
    if (len >= 1024) {
        LOG_HEXDUMP_E(n9log, "n9log packet length is longer than 1024", body, len);
        return 1;
    }
    xQueueSend(n9log_queue, body, 0);
    return 0;
}
void n9log_task_entry(void *args)
{
    uint8_t n9log_buffer[MAX_N9LOG_SIZE] = {0};
    for (;;) {
        xQueueReceive(n9log_queue, n9log_buffer, portMAX_DELAY);
        LOG_I(n9log, "%s", n9log_buffer);
    }
}
void n9log_queue_init(void)
{
    if (n9log_queue == NULL) {
        n9log_queue = xQueueCreate(3, MAX_N9LOG_SIZE+1);
        if (NULL == n9log_queue) {
            LOG_E(n9log, "create n9log queue failed.");
        }
    } else {
        LOG_I(n9log, "n9log_queue is not null, please deinit first.");
    }
    if (n9log_task == NULL ) {
        if (pdPASS != xTaskCreate(n9log_task_entry,
                                  N9LOG_TASK_NAME,
                                  N9LOG_TASK_STACKSIZE,
                                  NULL,
                                  N9LOG_TASK_PRIO,
                                  &n9log_task)) {
            LOG_I(n9log, "create n9log task failed.");
       }
    } else {
        LOG_I(n9log, "n9log_task is still running.");
    }
    wifi_config_register_n9log_handler((N9LOG_HANDLER)wifi_n9log_handler);
}
void n9log_queue_deinit(void)
{
    if (n9log_task != NULL) {
        vTaskDelete(n9log_task);
        n9log_task = NULL;
    }
    if (n9log_queue != NULL) {
        vQueueDelete(n9log_queue);
        n9log_queue = NULL;
    }
}
uint8_t n9log_state_set_cli(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    if (len < 2) {
        printf("Usage: \n");
        printf("n9log set off  -> Disable n9 logs\n");
        printf("n9log set n9   -> output n9 logs to uart\n");
        printf("n9log set host -> output n9 logs to host\n");
        return 1;
    }
    if (!os_strcmp(param[0], "set")){
        if (!os_strcmp(param[1], "off")){
            if ((ret = wifi_inband_set_n9_consol_log_state(0)) < 0) {
                status = 1;
            } else {
                 n9log_queue_deinit();
            }
        } else if (!os_strcmp(param[1], "n9")){
            if ((ret = wifi_inband_set_n9_consol_log_state(1)) < 0) {
                status = 1;
            } else {
                n9log_queue_deinit();
            }
        } else if (!os_strcmp(param[1], "host")){
            if ((ret = wifi_inband_set_n9_consol_log_state(2)) < 0) {
                  status = 1;
            } else {
                n9log_queue_init();
            }
        } else {
            printf("Not Supported Cmd\n");
            return 1;
        }
    } else {
        printf("Not Supported Cmd\n");
        return 1;
    }
    printf("n9log_state_set_cli, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

static char* wifi_get_phy_mode_name(uint8_t phy_mode, char* phy_name)
{
    if( NULL==phy_name ){
        printf("phy name is null\n");
        return NULL;
    }
    switch(phy_mode) {
    case 0:
        strcpy(phy_name, "Legacy CCK");
        break;
    case 1:
        strcpy(phy_name, "Legacy OFDM");
        break;
    case 2:
        strcpy(phy_name, "HT Mixed mode");
        break;
    case 3:
        strcpy(phy_name, "HT Green field mode");
        break;
    default:
        strcpy(phy_name, "unknown");
        break;
    }
    return phy_name;

}

static wifi_scan_list_item_t g_scan_list_temp[16] = {{0}};
/**
* @brief Example of privale event handler sample
*
* @return =0 means success, >0 means fail
*/
static int mtk_private_event_handler_sample(wifi_event_t event_id, unsigned char *payload, unsigned int len)
{
    int handled = 0;
    int i = 0;

    if( WIFI_EVENT_IOT_SCAN_COMPLETE==event_id )
    {
        handled = 1;
        for (i = 0; i < g_scan_list_size; i++) {
            printf("\n%-4s%-33s%-20s%-8s%-8s%-8s%-8s%-8s%-8s%-8s%-8s\n", "Ch", "SSID", "BSSID", "Auth", "Cipher", "RSSI", "WPS_EN", "CM", "DPID", "SR", "DTIM_PERIOD");
            printf("%-4d", g_scan_list[i].channel);
            printf("%-33s", g_scan_list[i].ssid);
            printf("%02x:%02x:%02x:%02x:%02x:%02x   ",
                   g_scan_list[i].bssid[0],
                   g_scan_list[i].bssid[1],
                   g_scan_list[i].bssid[2],
                   g_scan_list[i].bssid[3],
                   g_scan_list[i].bssid[4],
                   g_scan_list[i].bssid[5]);
            printf("%-8d", g_scan_list[i].auth_mode);
            printf("%-8d", g_scan_list[i].encrypt_type);
            printf("%-8d", g_scan_list[i].rssi);
            printf("%-8d", g_scan_list[i].is_wps_supported);
            printf("%-8d", g_scan_list[i].wps_element.configuration_methods);
            printf("%-8d", g_scan_list[i].wps_element.device_password_id);
            printf("%-8d", g_scan_list[i].wps_element.selected_registrar);
//            printf("%-8d", g_scan_list[i].dtim_period);

            printf("\n");
        }
        printf("[MTK Event Callback Sample]: Scan Done!\n");
        wifi_connection_unregister_event_handler(WIFI_EVENT_IOT_SCAN_COMPLETE, (wifi_event_handler_t) mtk_private_event_handler_sample);
        wifi_connection_scan_deinit();
        //printf("[MTK Event Callback Sample]: unregister and deinit!\n");
    }

    return handled;
}

/**
* @brief Example of Get Scan List
* wifi config get scan_list
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_scan_list_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t scan_mode = 0;
    uint8_t op_mode = 0;
    uint8_t scan_option = 0;

    /*Register event handler*/
    ret = wifi_connection_register_event_handler(WIFI_EVENT_IOT_SCAN_COMPLETE, (wifi_event_handler_t) mtk_private_event_handler_sample);
    if (ret < 0) {
        status = 1;
    }
    /*Start scan depend on opmode*/
    ret = wifi_connection_stop_scan();
    if (ret < 0) {
        status = 1;
    }
    /******************reset scan list*******************************/
    wifi_connection_scan_init(g_scan_list_temp, 16);
    ret = wifi_config_get_opmode(&op_mode);
    if (ret < 0) {
        status = 1;
    }
    if( WIFI_MODE_STA_ONLY== op_mode){
        scan_mode = 0;    /*0:full scan*/
        scan_option = 0;  /*0: active(passive in regulatory channel)*/
    }else if( WIFI_MODE_AP_ONLY== op_mode ){
        scan_mode = 1;    /*1:partial scan*/
        scan_option = 0;  /*0: active(passive in regulatory channel)*/
    }
    ret = wifi_connection_start_scan(NULL, 0, NULL, scan_mode, scan_option);
    if (ret < 0) {
        status = 1;
    }

    printf("wifi_config_get_scan_list_ex, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

/**
* @brief Example of Get Connection Status
* wifi config get conn_status
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_connection_status_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t op_mode = 0;
    uint8_t length = 0;
    uint8_t ssid[32] = {0};
    uint8_t bssid[6] = {0};
    uint8_t dtim = 0;
    uint8_t ch = 0;
    uint8_t wireless_mode = 0;
    uint8_t auth = 0;
    uint8_t encrypt = 0;
    int8_t rssi = 0;
    char* temp[]={"st2", "ap1"};

    ret = wifi_config_get_opmode(&op_mode);
    if (ret < 0) {
        status = 1;
    }
    //if( (WIFI_MODE_STA_ONLY!=op_mode) || (WIFI_MODE_REPEATER!=op_mode) ){
    //    printf("Only support in station/repeater mode. The running opmode is %d", op_mode);
    //    return status = 1;
    //}

    ret = wifi_config_get_ssid(WIFI_PORT_STA, ssid, &length);
    if (ret < 0) {
        status = 1;
    }
    ret = wifi_config_get_bssid((uint8_t *)bssid);
    if (ret < 0) {
        status = 1;
    }
    ret = wifi_config_get_dtim_interval(&dtim);
    if (ret < 0) {
        status = 1;
    }
    ret = wifi_config_get_channel(WIFI_PORT_STA, &ch);
    if (ret < 0) {
        status = 1;
    }
    ret = wifi_config_get_wireless_mode(WIFI_PORT_STA, (wifi_phy_mode_t *)&wireless_mode);
    if (ret < 0) {
        status = 1;
    }
    ret = wifi_config_get_security_mode(WIFI_PORT_STA, (wifi_auth_mode_t *)&auth, (wifi_encrypt_type_t *)&encrypt);
    if (ret < 0) {
        status = 1;
    }
    ret = wifi_connection_get_rssi(&rssi); // fixme
    if (ret < 0) {
        status = 1;
    }

    printf("Get connection status: status:\n");
    printf("    OPMODE        = %d \n", op_mode);
    printf("    SECURITY      = (auth=%d encrypt=%d)\n", auth, encrypt);
    printf("    WIRELESS MODE = %d \n", wireless_mode);
    printf("    SSID          = %s \n", ssid);
    printf("    BSSID         = %02x:%02x:%02x:%02x:%02x:%02x \n", bssid[0],bssid[1],bssid[2],bssid[3],bssid[4],bssid[5]);
//    printf("    PHY RATE      = %d \n", mode);
    printf("    CHANNEL       = %d \n", ch);
    printf("    DTIM          = %d \n", dtim);
    printf("    RSSI          = %d \n", rssi);
    printf("    IP ADDRESS INFO: \n");
    //lwip_ip_cli(0, NULL);
    lwip_ip_cli(1, temp);

    printf("wifi_config_get_connection_status_ex(), ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

/**
* @brief Example of Get Wlan statistic
* wifi config get wlanstat
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_statistic_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint32_t tx_fail_count;
    uint32_t rx_with_CRC;
    uint32_t AMPDU_tx_fail;
    char phy_name[32];
    uint8_t op_mode;
    uint8_t link = 0;
    wifi_statistic_t statistic = {0};

    ret = wifi_config_get_opmode(&op_mode);
    if( WIFI_MODE_STA_ONLY!=op_mode ){
        printf("Get wlan statistics only support in STA mode \n");
        return status = 1;
    }

    ret = wifi_config_get_statistic(&statistic);
    if (ret < 0) {
        status = 1;
    }
    tx_fail_count = statistic.Tx_Fail_Count;
    rx_with_CRC = statistic.Rx_with_CRC;
    AMPDU_tx_fail = statistic.AMPDU_Tx_Fail;

    printf("Get wlan statistics: \n");
    printf("  CurrentTemperature    = %u \n", (unsigned int)statistic.Current_Temperature);
    printf("  Tx success count      = %u \n", (unsigned int)statistic.Tx_Success_Count);
    printf("  Tx fail count         = %u, PER = %u.%u %% \n", (unsigned int)tx_fail_count,
        (unsigned int)WIFI_CLI_CALC_PER_BEFORE_DOT(tx_fail_count, tx_fail_count + statistic.Tx_Success_Count),
        (unsigned int)WIFI_CLI_CALC_PER_AFTER_DOT(tx_fail_count, tx_fail_count + statistic.Tx_Success_Count) );
    printf("  Tx retry count        = %u \n", (unsigned int)statistic.Tx_Retry_Count);
    printf("  Rx success count      = %u \n", (unsigned int)statistic.Rx_Success_Count);
    printf("  Rx_with CRC           = %u, PER = %u.%u %% \n", (unsigned int)rx_with_CRC,
        (unsigned int)WIFI_CLI_CALC_PER_BEFORE_DOT(rx_with_CRC, rx_with_CRC + statistic.Rx_Success_Count),
        (unsigned int)WIFI_CLI_CALC_PER_AFTER_DOT(rx_with_CRC, rx_with_CRC + statistic.Rx_Success_Count) );
    printf("  Rx drop due to out of resource  = %u \n", (unsigned int)statistic.Rx_Drop_due_to_out_of_resources);
    printf("  MIC error count       = %d \n", (unsigned int)statistic.MIC_Error_Count);
    printf("  Cipher error count    = %d \n", (unsigned int)statistic.Cipher_Error_Count);
    printf("  Rssi                  = %d \n", (unsigned int)statistic.Rssi);
    printf("  Tx AGG range_1 (1)    = %u \n", (unsigned int)statistic.Tx_AGG_Range_1);
    printf("  Tx AGG range_2 (2~5)  = %u \n", (unsigned int)statistic.Tx_AGG_Range_2);
    printf("  Tx AGG range_3 (6~15) = %u \n", (unsigned int)statistic.Tx_AGG_Range_3);
    printf("  Tx AGG range_4 (>15)  = %u \n", (unsigned int)statistic.Tx_AGG_Range_4);
    printf("  AMPDU Tx success      = %u \n", (unsigned int)statistic.AMPDU_Tx_Success);
    printf("  AMPDU Tx fail count   = %u, PER = %u.%u %% \n", (unsigned int)AMPDU_tx_fail,
        (unsigned int)WIFI_CLI_CALC_PER_BEFORE_DOT(AMPDU_tx_fail, AMPDU_tx_fail + statistic.AMPDU_Tx_Success),
        (unsigned int)WIFI_CLI_CALC_PER_AFTER_DOT(AMPDU_tx_fail, AMPDU_tx_fail + statistic.AMPDU_Tx_Success ) );
    printf("  SNR                   = %ld \n",  statistic.SNR);
    //printf("WpaSupplicantUP       = %d \n", 1);

    /* Rate*/
    printf("  Current bandwidth     = %s \n", (statistic.BBPCurrentBW) ? "BW 40":"BW 20" );
    printf("  Max phy rate          = %u.%u Mbps \n", (unsigned int)statistic.MAX_PHY_Rate/10, (unsigned int)statistic.MAX_PHY_Rate%10 );
    ret = wifi_connection_get_link_status(&link);
    if (ret < 0) {
        status = 1;
    }
//    if( 1==link )
    {
        printf("  Real tx phy rate      = %u.%u Mbps \n", (unsigned int)statistic.REAL_TX_PHY_Rate/10, (unsigned int)statistic.REAL_TX_PHY_Rate%10 );
        printf("  Real tx phy mode      = %s \n", wifi_get_phy_mode_name(statistic.REAL_TX_PHY_Mode, phy_name) );
        printf("  Real tx short GI      = %s \n", (statistic.REAL_TX_ShortGI) ? "Short GI (400)" : "Normal GI (800)" );
        printf("  Real tx MCS           = %u \n", (unsigned int)statistic.REAL_TX_MCS );
        printf("  Real rx phy rate      = %u.%u Mbps \n", (unsigned int)statistic.REAL_RX_PHY_Rate/10, (unsigned int)statistic.REAL_RX_PHY_Rate%10 );
        printf("  Real rx phy mode      = %s \n", wifi_get_phy_mode_name(statistic.REAL_RX_PHY_Mode, phy_name) );
        printf("  Real rx short GI      = %s \n", (statistic.REAL_RX_ShortGI) ? "Short GI (400)" : "Normal GI (800)" );
        printf("  Real rx MCS           = %u \n", (unsigned int)statistic.REAL_RX_MCS );
    }

    printf("\n wifi_config_get_statistic_ex, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;

}

uint8_t wifi_config_set_ip_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    wifi_ip_filter_t ip_addr = {{0}};

    if( len < 1 ){
        printf("Usage:  wifi config set ip_filter <ip_address> \n");
        return 1;
    }

    wifi_conf_get_ip_from_str((unsigned char *)&ip_addr,param[0]);
    ret = wifi_config_set_ip_filter(ip_addr);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_ip_filter_ex, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

/**
* @brief Example of Set WiFi ip_mac_port_filter ON/OFF
* wifi config set ip_mac_port_filter <onoff>
* @param [IN]onoff  0: OFF, 1: ON
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_set_ip_mac_port_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t flag = 0;

    if( len < 1 ){
        printf("Usage:  wifi config set ip_mac_port_filter <onoff> \n");
        return 1;
    }

    flag = atoi(param[0]);
    ret = wifi_config_set_ip_mac_port_filter(flag);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_ip_mac_port_filter_ex(onoff=%d), ret:%s, Code=%ld\n", flag, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

/**
* @brief Example of get WiFi ip_mac_port_filter ON/OFF
* wifi config get ip_mac_port_filter
* @param [OUT]onoff 0: OFF, 1: ON
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_ip_mac_port_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t flag = 0;

    ret = wifi_config_get_ip_mac_port_filter(&flag);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_ip_mac_port_filter_ex(onoff=%d), ret:%s, Code=%ld\n", (int)flag, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

int32_t port_invalid_check(char *port_str)
{
    int32_t port = (int32_t)atoi(port_str);

    if ( (port > 65535)||(port < 0) ) {
        printf("Invalid port argument: %ld\n", port);
        return -1;
    }
    return port;
}

/** @brief This enumeration defines the ip_match_entry_filter bitmap. Each bit indicates a specific param is enable.
*/
typedef enum {
    WIFI_IP_MATCH_FILTER_SRC_IPADDR=0,       /**< bit 0   SRC_IPADDR. */
    WIFI_IP_MATCH_FILTER_SRC_MACADDR,      /**< bit 1   SRC_MACADDR. */
    WIFI_IP_MATCH_FILTER_SRC_PORT,         /**< bit 2   SRC_PORT. */
    WIFI_IP_MATCH_FILTER_DST_IPADDR,       /**< bit 3   DST_IPADDR. */
    WIFI_IP_MATCH_FILTER_DST_MACADDR,      /**< bit 4   DST_MACADDR. */
    WIFI_IP_MATCH_FILTER_DST_PORT,         /**< bit 5   DST_PORT. */
    WIFI_IP_MATCH_FILTER_MAX_NUM,          /**< MAX_NUM. */
} wifi_ip_match_entry_filter_t;

/**
* @brief Example of Set WiFi ip_mac_port_entry. When you want to use set specific parameter, you should set the
*  valid bitmap fristly. It can be use fully and partial.
*  wifi config set ip_mac_port <action> <vaild_bitmap> [src_ip] [src_mac] [src_port] [dst_ip] [dst_mac] [dst_port]
* @param [IN]action   0 delete, 1 create,
* @param [IN]vaild bitmap   For more details, please refer to #wifi_ip_match_entry_filter_t.
* @param [IN]src_ip
* @param [IN]src_mac
* @param [IN]src_port  0~65535,usually is between 1024 and 5000
* @param [IN]dst_ip
* @param [IN]dst_mac
* @param [IN]dst_port  0~65535,usually is between 1024 and 5000
* @return  >=0 means success, <0 means fail
* @note entry please refer to #ip_match_entry_t
*/
uint8_t wifi_config_set_ip_mac_port_entry_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t action = 0;
    ip_match_entry_t entry;
    uint32_t flag = 0x00;
    int8_t i = 0;
    /*exclude <action> <valid bitmap>*/
    int8_t param_limit = 0;
    int8_t valid_bitmap_cnt = 0;
    int32_t src_port;
    int32_t dst_port;

    if( len < 2 ){
        printf("Usage:wifi config set ip_mac_port <action> <vaild_bitmap> [src_ip] [src_mac] [src_port] [dst_ip] [dst_mac] [dst_port]\n");
        return 1;
    }

    /*Print cli input*/
    for(i=0; i<len; i++){
        printf("wifi_config_set_ip_mac_port_entry_ex, i=%d,len=%d pram=%s \n", i, len, param[i]);
    }
    /* Action*/
    action = (uint8_t)atoi(param[0]);
    /* Valid_bitmap*/
    flag = strtoul(param[1], NULL, 0);
    if( ULONG_MAX ==flag ){
        printf("Valid_bitmap set incorrect \n");
        return 1;
    }
    entry.valid_bitmap = flag;

    /* Set input depend on flag*/
    param_limit = len-2;
    if( flag & BIT(WIFI_IP_MATCH_FILTER_SRC_IPADDR) ){
        /* WIFI_IP_MATCH_FILTER_SRC_IPADDR is enable, then get the input param, clear flag bit*/
        flag &= ~ BIT(WIFI_IP_MATCH_FILTER_SRC_IPADDR);
        if( ++valid_bitmap_cnt <= param_limit ){
            wifi_conf_get_ip_from_str((unsigned char *)&entry.src_ipAddr,(const char *)param[valid_bitmap_cnt+1]);
        }else{
            printf("Valid_bitmap set incorrect. flag=%x \n",(unsigned int)flag);
        }
    }
    if( flag & BIT(WIFI_IP_MATCH_FILTER_SRC_MACADDR) ){
        flag &= ~ BIT(WIFI_IP_MATCH_FILTER_SRC_MACADDR);
        if( ++valid_bitmap_cnt <= param_limit ){
            wifi_conf_get_mac_from_str((char *)&entry.src_macAddr, (const char *)param[valid_bitmap_cnt+1]);
        }else{
            printf("Valid_bitmap set incorrect. flag=%x \n",(unsigned int)flag);
        }
    }
    if( flag & BIT(WIFI_IP_MATCH_FILTER_SRC_PORT) ){
        flag &= ~ BIT(WIFI_IP_MATCH_FILTER_SRC_PORT);
        if( ++valid_bitmap_cnt <= param_limit ){
            src_port = port_invalid_check(param[valid_bitmap_cnt+1]);
            if ( src_port < 0 ) {
                return 1;
            }
            entry.src_port = (uint16_t)src_port;
        }else{
            printf("Valid_bitmap set incorrect. flag=%x \n",(unsigned int)flag);
        }
    }
    if( flag & BIT(WIFI_IP_MATCH_FILTER_DST_IPADDR) ){
        flag &= ~ BIT(WIFI_IP_MATCH_FILTER_DST_IPADDR);
        if( ++valid_bitmap_cnt <= param_limit ){
            wifi_conf_get_ip_from_str((unsigned char *)&entry.dst_ipAddr,(const char *)param[valid_bitmap_cnt+1]);
        }else{
            printf("Valid_bitmap set incorrect. flag=%x \n",(unsigned int)flag);
        }
    }
    if( flag & BIT(WIFI_IP_MATCH_FILTER_DST_MACADDR) ){
        flag &= ~ BIT(WIFI_IP_MATCH_FILTER_DST_MACADDR);
        if( ++valid_bitmap_cnt <= param_limit ){
            wifi_conf_get_mac_from_str((char *)&entry.dst_macAddr, (const char *)param[valid_bitmap_cnt+1]);
        }else{
            printf("Valid_bitmap set incorrect. flag=%x \n",(unsigned int)flag);
        }
    }
    if( flag & BIT(WIFI_IP_MATCH_FILTER_DST_PORT) ){
        flag &= ~ BIT(WIFI_IP_MATCH_FILTER_DST_PORT);
        if( ++valid_bitmap_cnt <= param_limit ){
            dst_port = port_invalid_check(param[valid_bitmap_cnt+1]);
            if ( dst_port < 0 ) {
                return 1;
            }
            entry.dst_port = (uint16_t)dst_port;
        }else{
            printf("Valid_bitmap set incorrect. flag=%x \n",(unsigned int)flag);
        }
    }

    ret = wifi_config_set_ip_mac_port_list_entry(action, &entry);
    if( ret<0 ){
        status = 1;
    }
    printf("wifi_config_set_ip_mac_port_entry_ex(action=%d), ret:%s, Code=%ld\n", action, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

/**
* @brief Example of Get WiFi ip_mac_port_entry List
* wifi config get ip_mac_port
* @param [OUT]entry_list
* @return  >=0 means success, <0 means fail
*/
uint8_t wifi_config_get_ip_mac_port_entry_ex(uint8_t len, char *param[])
{
    int i;
    uint8_t status = 0;
    int32_t ret = 0;
    ip_match_entry_t list[WIFI_MAX_NUMBER_OF_IP_MAC_PORT_ENTRY];
    uint8_t size = 0;
    ret = wifi_config_get_ip_mac_port_list_entry(&size, list);
    if (ret < 0) {
        status = 1;
    }

    printf("entry list size=%d\n", size);
    for (i = 0; i < size; i++) {
        printf("%d\n", i);
        printf("valid_bitmap=%d\n", list[i].valid_bitmap);
        printf("    src_ipAddr=%d.%d.%d.%d, src_macAddr=%02x:%02x:%02x:%02x:%02x:%02x, src_port=%d\n",
            list[i].src_ipAddr[0], list[i].src_ipAddr[1], list[i].src_ipAddr[2], list[i].src_ipAddr[3],
            list[i].src_macAddr[0], list[i].src_macAddr[1], list[i].src_macAddr[2],
            list[i].src_macAddr[3], list[i].src_macAddr[4], list[i].src_macAddr[5], list[i].src_port);
        printf("    dsc_ipAddr=%d.%d.%d.%d, dst_mac=%02x:%02x:%02x:%02x:%02x:%02x, dst_port=%d\n",
            list[i].dst_ipAddr[0], list[i].dst_ipAddr[1], list[i].dst_ipAddr[2], list[i].dst_ipAddr[3],
            list[i].dst_macAddr[0], list[i].dst_macAddr[1], list[i].dst_macAddr[2],
            list[i].dst_macAddr[3], list[i].dst_macAddr[4], list[i].dst_macAddr[5], list[i].dst_port);
    }
    printf("wifi_config_get_ip_mac_port_entry_ex, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

uint8_t wifi_tx_raw_pkt_advanced_ex(uint8_t len, char *param[])
{
    int8_t status = 0;
    int32_t ret = 0;
    uint8_t *hex_buf = NULL;
    uint32_t len_hex_buf = 0;
    uint32_t len_buf = 0;
    int i_hex = 0;
    txd_param_setting_t txd;

    if( 6!=len ){
        printf("Usage:wifi config set txraw_adv <tx_mode> <mcs> <rate_fixed> <ack_required> <retry_count> <raw_packets>\n");
        return 1;
    }

    len_hex_buf = os_strlen(param[5]) / 2 + os_strlen(param[5]) % 2;
    len_buf = os_strlen(param[5]);
    if ((hex_buf = (uint8_t *)os_malloc(len_hex_buf)) == NULL) {
        printf("alloc buffer failed.\n");
        return 1;
    }
    os_memset(hex_buf, 0, len_hex_buf);

    /*Set parameters*/
    txd.tx_mode = (uint8_t)atoi(param[0]);
    txd.mcs = (uint8_t)atoi(param[1]);
    txd.rate_fixed = (uint8_t)atoi(param[2]);
    txd.ack_required = (uint8_t)atoi(param[3]);
    txd.retry_count = (uint8_t)atoi(param[4]);
    printf("wifi_tx_raw_pkt_advanced_ex input, %d %d %d %d %d\n",
        txd.tx_mode, txd.mcs, txd.rate_fixed, txd.ack_required, txd.retry_count);

    for (int i = 0; i < len_buf; i++) {
        char cc = param[5][i];
        if ((i % 2 == 0) && i) {
            i_hex++;
        }

        if (cc >= '0' && cc <= '9') {
            hex_buf[i_hex] = (hex_buf[i_hex] << 4) + cc - '0';
        } else if (cc >= 'a' && cc <= 'f') {
            hex_buf[i_hex] = (hex_buf[i_hex] << 4) + cc - 'a' + 10;
        } else if (cc >= 'A' && cc <= 'F') {
            hex_buf[i_hex] = (hex_buf[i_hex] << 4) + cc - 'A' + 10;
        } else {
            printf("input invalid\n");
            os_free(hex_buf);
            return 1;
        }
    }

    if ((ret = wifi_connection_send_raw_packet_advanced(hex_buf, len_hex_buf, &txd)) < 0) {
        status = 1;
    }

    printf("tx advanced raw packet, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    os_free(hex_buf);
    return status;
}

uint8_t wifi_config_set_ps_mode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t ps_mode = 0;

    if( len < 1 ){
        printf("Usage: wifi config set ps_mode <ps_mode> \n");
        return 1;
    }

    ps_mode = atoi(param[0]);
    ret = wifi_config_set_power_save_mode(ps_mode);

    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_ps_mode_ex ps_power:%d, ret:%s, Code=%ld\n", ps_mode, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

uint8_t wifi_config_get_ps_mode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t ps_power = 0;

    ret = wifi_config_get_power_save_mode(&ps_power);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_inband_get_ps_mode ps_power:%d, ret:%s, Code=%ld\n", ps_power, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

uint8_t wifi_config_calculate_pmk_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t pmk_hex[32];
    uint8_t i = 0;

    if( len < 2 ){
        printf("Usage: wifi config get calc_pmk <ssid> <passphrase> \n");
        return 1;
    }

    os_memset(pmk_hex, 0, sizeof(pmk_hex));
    ret = wifi_config_calculate_pmk( (uint8_t *)param[0], os_strlen(param[0]), (uint8_t *)param[1], os_strlen(param[1]), pmk_hex);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_calculate_pmk_ex: ret:%s, Code=%ld, key dump:\n", WIFI_CLI_RETURN_STRING(ret), ret);
    for (i = 0; i < sizeof(pmk_hex); i++) {
        if (i % 16 == 0) {
            printf("\n\t");
        }
        printf("%02x ", (unsigned int)pmk_hex[i]);
    }
    printf("\n");
    return status;
}

#ifdef MTK_MINICLI_ENABLE
/**
* @brief
* @param
*
* @return  =0 means success, >0 means fail
*/
uint8_t _cli_wow(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;

    if (len < 1) {
        printf("Usage: \n");
        printf("wow enable <enable ex.0/1>\n");
        printf("wow magic <enable ex.0/1> <bss_index ex.1/3...>\n");
        printf("wow bitmap <16 bytes bitmap mask> <128 bytes bitmap pattern>\n");
        printf("wow arp <ip_addr ex.192> <ip_addr ex.168> <ip_addr ex.0> <ip_addr ex.1>\n");
        printf("wow rekey <64 bytes ptk>\n");
        return 1;
    }

    if (!os_strcmp((char *)param[0], "enable")) {
        wifi_wow_enable_t wow_param;
        wow_param.enable = atoi(param[1]);

        ret = wifi_inband_wow_enable(&wow_param);
    } else if (!os_strcmp((char *)param[0], "magic")) {
        wifi_wow_magic_packet_t wow_param;
        wow_param.enable = atoi(param[1]);
        wow_param.bss_index = strtol(param[2], NULL, 16);

        ret = wifi_inband_wow_set_magic_packet(&wow_param);
    } else if (!os_strcmp((char *)param[0], "bitmap")) {
        wifi_wow_bitmap_pattern_t wow_param;
        os_memcpy(wow_param.bitmap_mask, param[1], sizeof(wow_param.bitmap_mask));
        os_memcpy(wow_param.bitmap_pattern, param[2], sizeof(wow_param.bitmap_pattern));

        ret = wifi_inband_wow_set_bitmap_pattern(&wow_param);
    } else if (!os_strcmp((char *)param[0], "arp")) {
        wifi_wow_arp_offload_t wow_param;
        wow_param.ip_address[0] = atoi(param[1]);
        wow_param.ip_address[1] = atoi(param[2]);
        wow_param.ip_address[2] = atoi(param[3]);
        wow_param.ip_address[3] = atoi(param[4]);

        ret = wifi_inband_wow_set_arp_offload(&wow_param);
    } else if (!os_strcmp((char *)param[0], "rekey")) {
        wifi_wow_rekey_offload_t wow_param;
        os_memcpy(wow_param.ptk, param[1], sizeof(wow_param.ptk));

        ret = wifi_inband_wow_set_rekey_offload(&wow_param);
    } else {
        printf("Not support cmd\n");
        return 1;
    }

    if (ret < 0) {
        status = 1;
    }
    printf("_cli_wow, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

/**
* @brief Example of Get inband queue spaces
* wifi msq get iq
* @parameter None
* @return =0 means success, >0 means fail
*/
uint8_t wifi_mq_get_iq_ex(uint8_t len, char *param[])
{
    uint32_t iq_spaces = 0;
    iq_spaces = (uint32_t)inband_get_mq_spaces();

    (iq_spaces == 0) ? printf("inband queue full\n") : printf("inband queue spaces: %ld\n", iq_spaces);
    return 0;
}

#ifdef MTK_CM4_SCAN_TABLE_ENABLE
/**
* @brief Example of Set WiFi Scan Table Size
* wifi config set scantabsize <size>
* @param [IN]num 1 ~ 255
*
* @return  =0 means success, >0 means fail
*/
static uint8_t wifi_config_set_scan_tab_size_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t size = 0;

    if( len < 1 ){
        printf("Usage: wifi config set scantabsize <size> \n");
        return 1;
    }

    size = atoi(param[0]);
    ret = wifi_config_set_scan_table_size(size);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_scan_tab_size, size:%d, ret:%s, Code=%ld\n", size, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

#ifndef MTK_CLI_SLIM_FOR_32
/**
* @brief Example of Get WiFi Scan Table Size
* wifi config get scantabsize
* @param [OUT]num 1 ~ 255
*
* @return  =0 means success, >0 means fail
*/
static uint8_t wifi_config_get_scan_tab_size_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t size = 0;

    ret = wifi_config_get_scan_table_size(&size);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_scan_tab_size size:%d, ret:%s, Code=%ld\n", size, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}
#endif
#endif
#if defined(MTK_WIFI_WPS_ENABLE)
/**
* @brief Example of Registrar or unregistrar WPS credential handler
* wifi config set wps_cred_handler <register/unregister>
* @param "registrar" or "register"
*
* @return  =0 means success, >0 means fail
*/
static uint8_t wifi_config_set_wps_crdential_handler(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int ret = 0;

    if( len < 1 ){
        printf("Usage: wifi config set wps_cred_handler <register/unregister> \n");
        return 1;
    }
#ifdef MTK_MINISUPP_ENABLE
    ret = mtk_supplicant_ctrl_wps_set_credential_handler((char *)param[0]);

#else
    ret = fw_mlme_wps_set_wps_crdential_handler((char *)param[0]);
#endif
    if (ret < 0) {
        status = 1;
    }

    printf("wifi_config_set_wps_crdential_handler, ret:%s, Code=%d\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}


/**
* @brief Example internal function: STA act as WPS Registrar
* wifi config set wps_sta_reg <cmd>
* @param
*
* @return  =0 means success, >0 means fail
*/
static uint8_t wifi_config_set_wps_sta_reg(uint8_t len, char *param[])
{
    uint8_t status = 0;

    /* 82,86 do not support set sta as registral , so ignore this function*/

#ifdef MTK_MINISUPP_ENABLE
    int ret = 0;

    if( len < 1 ){
        printf("Usage: wifi config set wps_sta_reg <cmd> \n");
        return 1;
    }

    if (__g_wpa_supplicant_api.do_wps_reg_pin) {
        ret = __g_wpa_supplicant_api.do_wps_reg_pin((char *)param[0]);
        if (ret < 0) {
            status = 1;
        }
    } else {
        printf("NOT support wps_reg.\n");
        return 1;
    }
    printf("wifi_config_set_wps_sta_reg, ret:%s, Code=%d\n", WIFI_CLI_RETURN_STRING(ret), ret);
#endif
    return status;
}
#endif/*MTK_WIFI_WPS_ENABLE*/

#endif

#ifndef MTK_CLI_SLIM_FOR_32
#ifdef MTK_SINGLE_SKU_SUPPORT
static uint8_t wifi_config_set_single_sku_table_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t band = atoi(param[0]);
    uint32_t nvdm_buff_len = 1024;
    uint8_t  * nvdm_buff = os_malloc(nvdm_buff_len);
    singleSKU2G_t  * sku_table_2g = (singleSKU2G_t  *)os_malloc(sizeof(singleSKU2G_t));
    singleSKU5G_t  * sku_table_5g = (singleSKU5G_t  *)os_malloc(sizeof(singleSKU5G_t));

    if(nvdm_buff == NULL || sku_table_2g == NULL || sku_table_5g == NULL) {
        ret = -1;
        printf("malloc fail\n");
        goto cli_end;
    }

    if ( band == WIFI_BAND_2_4_G ) {

        nvdm_read_data_item("common", "SingleSKU2G_0", nvdm_buff, &nvdm_buff_len);
        wifi_conf_get_pwr_from_str(sizeof(singleSKU2G_t), sku_table_2g, (char *)nvdm_buff);
#if 0
        printf("[DEBUG]2G single SKU table len %d :\n", sizeof(singleSKU2G_t));
        for(uint16_t i=0; i < sizeof(singleSKU2G_t); i++) {
            printf("%d,", *((char *)sku_table_2g + i));
        }
        printf("\n\n");
#endif
        ret = wifi_config_set_single_sku_2G(sku_table_2g);

    } else if (band == WIFI_BAND_5_G){

        nvdm_read_data_item("common", "SingleSKU5G_L0", nvdm_buff, &nvdm_buff_len);
        wifi_conf_get_pwr_from_str(WIFI_5G_LOW_CHANNEL_SKU_LEN, (char *)sku_table_5g, (char *)nvdm_buff);

        nvdm_read_data_item("common", "SingleSKU5G_M0", nvdm_buff, &nvdm_buff_len);
        wifi_conf_get_pwr_from_str(WIFI_5G_MIDDLE_CHANNEL_SKU_LEN, (char *)sku_table_5g + WIFI_5G_LOW_CHANNEL_SKU_LEN, (char *)nvdm_buff);

        nvdm_read_data_item("common", "SingleSKU5G_H0", nvdm_buff, &nvdm_buff_len);
        wifi_conf_get_pwr_from_str(WIFI_5G_HIGH_CHANNEL_SKU_LEN, (char *)sku_table_5g + (WIFI_5G_LOW_CHANNEL_SKU_LEN + WIFI_5G_MIDDLE_CHANNEL_SKU_LEN), (char *)nvdm_buff);
#if 0
        printf("[DEBUG]5G single SKU table len %d :\n", sizeof(singleSKU5G_t));
        for(uint16_t i = 0; i < sizeof(singleSKU5G_t); i++) {
            printf("%d,", *((char *)sku_table_5g + i));
        }
        printf("\n\n");
#endif
      ret = wifi_config_set_single_sku_5G(sku_table_5g);
    }
cli_end:

    if (ret < 0) {
        status = 1;
    }
    if (nvdm_buff != NULL) {
        os_free(nvdm_buff);
    }
    if (sku_table_2g != NULL) {
        os_free(sku_table_2g);
    }
    if (sku_table_5g != NULL) {
        os_free(sku_table_5g);
    }

    printf("wifi_config_set_single_sku_table_ex, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

static uint8_t wifi_config_get_single_sku_table_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = -1;
    uint8_t band = atoi(param[0]);
    singleSKU2G_t  * sku_table_2g = (singleSKU2G_t  *)os_malloc(sizeof(singleSKU2G_t));
    singleSKU5G_t  * sku_table_5g = (singleSKU5G_t  *)os_malloc(sizeof(singleSKU5G_t));

    if(sku_table_2g == NULL || sku_table_5g == NULL) {
        ret = -1;
        printf("malloc fail\n");
        goto cli_end;
    }

    if ( band == WIFI_BAND_2_4_G ) {
        ret = wifi_config_get_single_sku_2G(sku_table_2g);

    } else if (band == WIFI_BAND_5_G){
        ret = wifi_config_get_single_sku_5G(sku_table_5g);
    }
cli_end:

    if (ret < 0) {
        status = 1;
    } else {
        printf("print single SKU table\nA0:[CCK1M/2M] A1:[CCK5.5M/11M]\nB0:[OFDM 6M/9M] B1:[OFDM 12M/18M] B2:[OFDM 24M/36M] B3:[OFDM 48M] B4:[OFDM 54M]\n\
C0:[HT20 MCS0] C1:[HT20 MCS1/MCS2] C2:[HT20 MCS3/MCS4] C3:[HT20 MCS5] C4:[HT20 MCS6] C5:[HT20 MCS7]\n\
D0:[HT40 MCS0] D1:[HT40 MCS1/MCS2] D2:[HT40 MCS3/MCS4] D3:[HT40 MCS5] D4:[HT40 MCS6] D5:[HT40 MCS7]\n\n");
        printf("      A0 A1 B0 B1 B2 B3 B4 C0 C1 C2 C3 C4 C5 D0 D1 D2 D3 D4 D5\n");
        if(band == WIFI_BAND_2_4_G) {
            for(uint8_t i = 0; i < 14; i++) {
                printf("CH%2d:", i+1);
                for(uint8_t j = 0; j < 19; j++){
                    printf("%3d", *((char *)sku_table_2g + (i*19+j)));
                }
                //UART will miss some char, need delay between each line.
                vTaskDelay(8);
                printf("\n");
            }
        } else {
            uint8_t ch_table_5g[42] = {36,38,40,42,44,46,48,52,54,56,58,60,62,64,100,102,104,106,108,110,112,116,118,120,122,124,126,128,132,134,136,140,149,151,153,155,157,159,161,165,169,173};
            for(uint8_t i = 0; i < 42; i++) {
                printf("CH%3d:     ",ch_table_5g[i]);//skip CCK module
                for(uint8_t j = 0; j < 17; j++){
                    printf("%3d", *((char *)sku_table_5g + (i*17+j)));
                }
                //UART will miss some char, need delay between each line.
                vTaskDelay(8);
                printf("\n");
            }
        }
    }

    if (sku_table_2g != NULL) {
        os_free(sku_table_2g);
    }
    if (sku_table_5g != NULL) {
        os_free(sku_table_5g);
    }

    printf("wifi_config_set_single_sku_table_ex, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}
#endif
#endif

#ifdef MTK_ANT_DIV_ENABLE
uint8_t wifi_config_get_antenna_status_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    wifi_antenna_status_t ant_status = {0};
    ret = wifi_config_get_antenna_status(&ant_status);
    if (ret < 0) {
        status = 1;
    }

    printf("antenna_number:%d, ant0_rssi:%d; ant1_rssi:%d, pre_selected_rssi:%d\n",
        ant_status.antenna_number,ant_status.ant0_rssi,ant_status.ant1_rssi, ant_status.pre_selected_rssi);
    printf("wifi_config_get_antenna_status_ex, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

uint8_t wifi_config_set_antenna_monitor_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;

    ret = wifi_config_set_antenna_monitor();

    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_antenna_monitor_ex, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

uint8_t wifi_config_set_antenna_number_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t ant_number = (uint8_t)atoi(param[0]);

    if(ant_number > 1) {
        ret = -1;
        goto cli_end;
    }

    ret = wifi_config_set_antenna_number(ant_number);

cli_end:
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_antenna_monitor_ex, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

#endif


#if defined(MTK_MINICLI_ENABLE)
cmd_t   wifi_config_set_private_cli[] = {

#ifdef MTK_CM4_SCAN_TABLE_ENABLE
    { "scantabsize",    "scan table size",      wifi_config_set_scan_tab_size_ex },
#endif
    { "n9dbg",          "set N9 debug level",   wifi_config_set_n9_dbg_level },
#ifndef MTK_CLI_SLIM_FOR_32
    { "rssi_thresh",    "rssi threshold",       wifi_config_set_rssi_threshold_ex },
#endif
    { "frame_filter",   "frame_filter",         wifi_config_set_frame_filter_ex },
    { "frame_filter_advanced",   "frame_filter",         wifi_config_set_frame_filter_advanced_ex },
    { "tx_power",       "N9 tx power",          wifi_config_set_tx_power_ex },
    { "ip_filter",      "IP filter",            wifi_config_set_ip_filter_ex },
#if defined(MTK_WIFI_WPS_ENABLE)
    { "wps_cred_handler",  "wps cred_handler",  wifi_config_set_wps_crdential_handler },
    { "wps_sta_reg",       "wps sta is reg",    wifi_config_set_wps_sta_reg },
#endif
    { "ip_mac_port_filter","ip mac port filter on_off", wifi_config_set_ip_mac_port_filter_ex },
    { "ip_mac_port",      "ip mac port entry",  wifi_config_set_ip_mac_port_entry_ex },
    { "txraw_adv",   "send TX RAW packet with advanced",   wifi_tx_raw_pkt_advanced_ex },
    { "ps_mode",     "PS mode",                 wifi_config_set_ps_mode_ex },
#ifndef MTK_CLI_SLIM_FOR_32
#ifdef MTK_SINGLE_SKU_SUPPORT
    { "single_sku",   "single_sku table",       wifi_config_set_single_sku_table_ex },
#endif
#ifdef MTK_ANT_DIV_ENABLE
    { "ant_monitor",     "N9 ant monitor and sw", wifi_config_set_antenna_monitor_ex },
    { "ant_number",      "antenna number",         wifi_config_set_antenna_number_ex },
#endif

#endif
    { NULL }
};

cmd_t   wifi_config_get_private_cli[] = {
#ifndef MTK_CLI_SLIM_FOR_32
#ifdef MTK_CM4_SCAN_TABLE_ENABLE
    { "scantabsize",    "scan table size",      wifi_config_get_scan_tab_size_ex },
#endif
    { "n9dbg",          "get N9 debug level",   wifi_config_get_n9_dbg_level },
    { "rssi_thresh",    "rssi threshold",       wifi_config_get_rssi_threshold_ex },
    { "tx_power",       "N9 tx power",          wifi_config_get_tx_power_ex },
    { "wlanstat",       "get wlan statistic",   wifi_config_get_statistic_ex},
    { "scan_list",      "get scan list",        wifi_config_get_scan_list_ex},
    { "conn_status",    "get connection status", wifi_config_get_connection_status_ex},
#endif
    { "ip_mac_port_filter", "ip mac port filter on_off", wifi_config_get_ip_mac_port_filter_ex },
    { "ip_mac_port",      "ip mac port entry",  wifi_config_get_ip_mac_port_entry_ex },
    { "ps_mode",        "PS mode",              wifi_config_get_ps_mode_ex },
    { "calc_pmk",       "calculate pmk value",  wifi_config_calculate_pmk_ex },
#ifndef MTK_CLI_SLIM_FOR_32
#ifdef MTK_SINGLE_SKU_SUPPORT
    { "single_sku",   "single_sku table",       wifi_config_get_single_sku_table_ex },
#endif
#ifdef MTK_ANT_DIV_ENABLE
    { "ant_status",   "antenna status",       wifi_config_get_antenna_status_ex },
#endif

#endif
    { NULL }
};

cmd_t   wifi_mq_get_cli[] = {
    { "iq",     "get iq",  wifi_mq_get_iq_ex},
    { NULL }
};

static cmd_t   wifi_config_private_cli[] = {
    { "set",   "wifi config set",    NULL,   wifi_config_set_private_cli },
    { "get",   "wifi config get",    NULL,   wifi_config_get_private_cli },
    { NULL,    NULL,                 NULL,   NULL                }
};

static cmd_t   wifi_mq_cli[] = {
    { "get",   "wifi mq get",   NULL,   wifi_mq_get_cli },
    { NULL,    NULL,            NULL,   NULL }
};

cmd_t   wifi_private_cli[] = {
    { "config",     "wifi config",  NULL,      wifi_config_private_cli },
    { "wow",        "wow features", _cli_wow,  NULL },
    { "mq",         "wifi mq",      NULL,       wifi_mq_cli },
    { NULL,         NULL,           NULL,       NULL },
};
#endif
