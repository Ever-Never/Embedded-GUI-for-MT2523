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
/*--- For Register AT command handler ---*/
#include "atci.h"
// System header files
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "os.h"
#include "os_util.h"
#include "syslog.h"
#include "misc.h"
#include "wifi_api.h"
#include "wifi_private_api.h"
#include "wifi_scan.h"
#ifdef MTK_WIFI_WPS_ENABLE
#include "wifi_wps.h"
#endif
#include "type_def.h"
#include "dhcpd.h"
#include "ethernetif.h"
#include "wifi_lwip_helper.h"
#include "get_profile_string.h"
#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#endif
#include "iperf_task.h"
#include "ping.h"
#include "dhcp.h"
#if defined(MTK_SMTCN_V5_ENABLE) || defined(MTK_SMTCN_V4_ENABLE)
#include "smt_conn.h"
#endif
log_create_module(WIFI_ATCMD, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(WIFI_ATCMD, "ATCI iot_sdk project: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(WIFI_ATCMD, "ATCI iot_sdk project: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(WIFI_ATCMD, "ATCI iot_sdk project: "fmt,##arg)


atci_status_t  atci_cmd_hdlr_opmode(atci_parse_cmd_param_t *parse_cmd);
atci_status_t  atci_cmd_hdlr_mac_address(atci_parse_cmd_param_t *parse_cmd);
atci_status_t  atci_cmd_hdlr_ip_address(atci_parse_cmd_param_t *parse_cmd);
#if defined(MTK_SMTCN_V5_ENABLE) || defined(MTK_SMTCN_V4_ENABLE)
atci_status_t  atci_cmd_hdlr_smart_connection(atci_parse_cmd_param_t *parse_cmd);
#endif
atci_status_t  atci_cmd_hdlr_ch_bw(atci_parse_cmd_param_t *parse_cmd);
atci_status_t  atci_cmd_hdlr_block_ack(atci_parse_cmd_param_t *parse_cmd);
atci_status_t  atci_cmd_hdlr_wireless_mode(atci_parse_cmd_param_t *parse_cmd);
atci_status_t  atci_cmd_hdlr_wifi_tx_raw_cmd(atci_parse_cmd_param_t *parse_cmd);
atci_status_t  atci_cmd_hdlr_wifi_sta_cmd(atci_parse_cmd_param_t *parse_cmd);
atci_status_t  atci_cmd_hdlr_wifi_ap_cmd(atci_parse_cmd_param_t *parse_cmd);
#ifdef MTK_WIFI_WPS_ENABLE
atci_status_t  atci_cmd_hdlr_wifi_wps_cmd(atci_parse_cmd_param_t *parse_cmd);
#endif
atci_status_t  atci_cmd_hdlr_iperf(atci_parse_cmd_param_t *parse_cmd);
atci_status_t  atci_cmd_hdlr_ping(atci_parse_cmd_param_t *parse_cmd);
atci_status_t  atci_cmd_hdlr_ip(atci_parse_cmd_param_t *parse_cmd);
static uint8_t atci_cmd_iperf_server(char *param);
static uint8_t atci_cmd_iperf_client(char *param);
#ifdef MODEM_ENABLE
static void atci_cmd_iperf_cb(iperf_result_t *iperf_result);
#endif
static void ping_request_result(ping_result_t *result);
static void atci_cmd_show_ip(netif_type_t type);
static void atci_cmd_set_ip(netif_type_t type, char *ipaddr, char *netmask, char *gw);


/*---  Variant ---*/
atci_cmd_hdlr_item_t item_table[] = {
    {"AT+WIFI",      atci_cmd_hdlr_opmode,      0, 0},
    {"AT+WIFIMAC",   atci_cmd_hdlr_mac_address,      0, 0},
    {"AT+WIFIIP",    atci_cmd_hdlr_ip_address,    0, 0},
#if defined(MTK_SMTCN_V5_ENABLE) || defined(MTK_SMTCN_V4_ENABLE)
    {"AT+SMNT",      atci_cmd_hdlr_smart_connection,      0, 0},
#endif
    {"AT+WIFICH",    atci_cmd_hdlr_ch_bw,      0, 0},
    {"AT+WIFIBASupport",    atci_cmd_hdlr_block_ack,      0, 0},
    {"AT+WIFIWirelessMode",    atci_cmd_hdlr_wireless_mode,    0, 0},
    {"AT+WIFITxRawPkt",    atci_cmd_hdlr_wifi_tx_raw_cmd,    0, 0},
    {"AT+WIFISTA",    atci_cmd_hdlr_wifi_sta_cmd,    0, 0},
    {"AT+WIFIAP",     atci_cmd_hdlr_wifi_ap_cmd,    0, 0},
#ifdef MTK_WIFI_WPS_ENABLE
    {"AT+WIFIWPS",    atci_cmd_hdlr_wifi_wps_cmd,    0, 0},
#endif
    {"AT+IPERF",        atci_cmd_hdlr_iperf,         0, 0},
    {"AT+PING",         atci_cmd_hdlr_ping,         0, 0},
    {"AT+IP",           atci_cmd_hdlr_ip,         0, 0},
};


void wifi_atci_example_init()
{
    atci_status_t ret = ATCI_STATUS_REGISTRATION_FAILURE;

    LOGW("atci_example_init\r\n");

    ret = atci_register_handler(item_table, sizeof(item_table) / sizeof(atci_cmd_hdlr_item_t));
    if (ret == ATCI_STATUS_OK) {
        LOGW("wifi_atci_example_init register success\r\n");
    } else {
        LOGW("wifi_atci_example_init register fail\r\n");
    }

}


/*--- AT command handler ---*/

extern uint8_t g_scan_inited;

wifi_scan_list_item_t *scan_ap_list = NULL;
static int scan_ap_size = 0;
static int scan_event_handler_sample(wifi_event_t event_id, unsigned char *payload, unsigned int len)
{
    int handled = 0;
    int i = 0;

    switch (event_id) {

        case WIFI_EVENT_IOT_SCAN_COMPLETE:
            handled = 1;
            for (i = 0; i < scan_ap_size; i++) {
                printf("\n%-4s%-33s%-20s%-8s%-8s%-8s%-8s%-8s%-8s%-8s\n", "Ch", "SSID", "BSSID", "Auth", "Cipher", "RSSI", "WPS_EN", "CM", "DPID", "SR");
                printf("%-4d", scan_ap_list[i].channel);
                printf("%-33s", scan_ap_list[i].ssid);
                printf("%02x:%02x:%02x:%02x:%02x:%02x   ",
                       scan_ap_list[i].bssid[0],
                       scan_ap_list[i].bssid[1],
                       scan_ap_list[i].bssid[2],
                       scan_ap_list[i].bssid[3],
                       scan_ap_list[i].bssid[4],
                       scan_ap_list[i].bssid[5]);
                printf("%-8d", scan_ap_list[i].auth_mode);
                printf("%-8d", scan_ap_list[i].encrypt_type);
                printf("%-8d", scan_ap_list[i].rssi);
                printf("%-8d", scan_ap_list[i].is_wps_supported);
                printf("%-8d", scan_ap_list[i].wps_element.configuration_methods);
                printf("%-8d", scan_ap_list[i].wps_element.device_password_id);
                printf("%-8d", scan_ap_list[i].wps_element.selected_registrar);
                printf("\n");
            }
            printf("[MTK Event Callback Sample]: Scan Done!\n");
            os_free(scan_ap_list);
            scan_ap_list = NULL;
            scan_ap_size = 0;
            break;

        default:
            handled = 0;
            printf("[MTK Event Callback Sample]: Unknown event(%d)\n", event_id);
            break;
    }
    return handled;
}


static int wlan_raw_pkt_rx_filter_sample(uint8_t *payload, unsigned int len)
{
    wifi_data_parse_t data_info = {0};
    if(wifi_connection_parse_data_descriptor(payload, &data_info) < 0) {
        return 0;/* Not handled */
    }

    printf("payload length=%d, rssi:%d, data rate: %ld.%ld Mbps\n", data_info.packet_payload_len, data_info.rssi, data_info.data_rate/10, data_info.data_rate%10);
    return 1; /* handled */
}


atci_status_t atci_cmd_hdlr_opmode(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    uint8_t operation = 0;      //0 is get, 1 is set
    uint8_t opmode = 0;
    int32_t ret = 0;
    LOGI("atci_cmd_hdlr_opmode");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFI=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFI?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+WIFI:\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFI
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFI=xxx,<op>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=opmode,") != NULL) {
                    LOGI("AT+WIFI=opmode,<op>  (op: 0 means get; 1 means set)\n");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    LOGI("AT CMD received operation mode:%s", param);
                    operation = (uint8_t)atoi(param);
                    if (0 == operation) {
                        ret = wifi_config_get_opmode(&opmode);
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Get Opmode failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;

                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED:Get Opmode:%d\n\r", opmode);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    } else if (1 == operation) {
                        param = strtok(NULL, ",");
                        LOGI("AT CMD received target opmode:%s", param);
                        opmode = (uint8_t)atoi(param);
                        ret = wifi_set_opmode(opmode);
                        if (opmode <= 0) {
                            LOGE("Invalid opmode\n");
                            ret = 1;
                        }
                        if (1 == opmode) {
                            ret = wifi_config_reload_setting();
                        }
                        if (ret < 0) {
                            LOGE("Set opmode failed\n");
                            ret = 1;
                        }
                        if (1 == ret) {
                            sprintf((char *)resonse.response_buf, "FAILED:Set target opmode failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED:Set target opmode:%d\n\r", opmode);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                        }
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=getinfo,") != NULL) {
                    LOGI("AT+WIFI=getinfo,<sta|ap> (0 is sta; 1 is ap)");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    LOGI("AT CMD received port:%s", param);
                    uint8_t port = 0;
                    if( atoi(param)<0 ){
                        printf("Invalid port\n");
                        return ATCI_STATUS_ERROR;
                    }
                    port = (uint8_t)atoi(param);
                    uint8_t length = 0;
                    uint8_t ssid[32] = {0};
                    uint8_t auth = 0;
                    uint8_t encrypt = 0;
                    uint8_t password[64] = {0};
                    ret = wifi_config_get_ssid(port, ssid, &length);
                    if (ret < 0) {
                        LOGE("Get ssid failed\n");
                        ret = 1;
                    }
                    ret = wifi_config_get_security_mode(port, (wifi_auth_mode_t *)&auth, (wifi_encrypt_type_t *)&encrypt);
                    if (ret < 0) {
                        LOGE("Get auth mode/encrypt type failed\n");
                        ret = 1;
                    }
                    printf("The current settings is:\n");
                    printf("[ssid]: %s, [Auth_Mode]: %d,[encrypt_type]: %d\n", ssid, auth, encrypt);
                    if ((auth == 0) && (encrypt == 0)) {
                        wifi_wep_key_t *keys = NULL;
                        keys = (wifi_wep_key_t *)os_zalloc(sizeof(wifi_wep_key_t));
                        if (keys) {
                            os_memset(keys, 0, sizeof(wifi_wep_key_t));
                        } else {
                            LOGE("%s: keys alloc fail.\n");
                            return 1;
                        }
                        ret = wifi_config_get_wep_key(port, keys);
                        if (ret < 0) {
                            LOGE("Get wep keys failed\n");
                            ret = 1;
                        }
                        if (keys->wep_tx_key_index < 4) {
                            int i;
                            printf("keys[%d]=", (int)keys->wep_tx_key_index);
                            for (i = 0; i < keys->wep_key_length[keys->wep_tx_key_index]; i++) {
                                printf("%02x", keys->wep_key[keys->wep_tx_key_index][i]);
                            }
                            printf("\n");
                        } else {
                            printf("Invalid key id:[%d]\n", (int)keys->wep_tx_key_index);
                            ret = 1;
                        }
                        os_free(keys);
                    } else {
                        ret = wifi_config_get_wpa_psk_key(port, password, &length);
                        if (ret < 0) {
                            LOGE("Get password failed\n");
                            ret = 1;
                        } else {
                            LOGE("[password]:%s\n", password);
                        }
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=dtim") != NULL) {
                    LOGI("AT+WIFI=dtim,<set|get>,<dtim_interval>(0 is get, 1 is set)(WiFi DTIM Interval:1~255)");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    operation = (uint8_t)atoi(param);
                    uint8_t dtim = 0;
                    if (0 == operation) {
                        ret = wifi_config_get_dtim_interval(&dtim);
                        if (ret < 0) {
                            LOGE("Get DTIM failed\n");
                            ret = 1;
                        } else {
                            LOGI("DTIM value is:%d", dtim);
                        }
                    } else {
                        param = strtok(NULL, ",");
                        dtim = (uint8_t)atoi(param);
                        ret = wifi_config_set_dtim_interval(dtim);
                        if (ret < 0) {
                            LOGE("Set DTIM failed\n");
                            ret = 1;
                        } else {
                            LOGI("Set DTIM value is:%d", dtim);
                        }
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=listen") != NULL) {
                    LOGI("AT+WIFI=listen,<set|get>,<listen_interval>(0 is get, 1 is set)(Listen Interval:1~255)");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    operation = (uint8_t)atoi(param);
                    uint8_t listen_interval = 0;
                    if (0 == operation) {
                        ret = wifi_config_get_listen_interval(&listen_interval);
                        if (ret < 0) {
                            LOGE("Get Listen Interval failed\n");
                            ret = 1;
                        } else {
                            LOGI("Listen Interval value is:%d", listen_interval);
                        }
                    } else {
                        param = strtok(NULL, ",");
                        listen_interval = (uint8_t)atoi(param);
                        ret = wifi_config_set_dtim_interval(listen_interval);
                        if (ret < 0) {
                            LOGE("Set Listen Interval failed\n");
                            ret = 1;
                        } else {
                            LOGI("Set Listen Interval value is:%d", listen_interval);
                        }
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=country") != NULL) {
                    LOGI("AT+WIFI=country,<set|get>,<band>,<region>(0 is get, 1 is set)");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    operation = (uint8_t)atoi(param);
                    uint8_t band = 0;
                    uint8_t region = 0;
                    if (0 == operation) {
                        ret = wifi_config_get_country_region(band, &region);
                        if (ret < 0) {
                            LOGE("Get country region failed\n");
                            ret = 1;
                        } else {
                            LOGI("Get country region is:%d, band is %d", region, band);
                        }
                    } else {
                        param = strtok(NULL, ",");
                        band = (uint8_t)atoi(param);
                        param = strtok(NULL, ",");
                        region = (uint8_t)atoi(param);
                        ret = wifi_config_set_country_region(band, region);
                        if (ret < 0) {
                            LOGE("Set country region failed\n");
                            ret = 1;
                        } else {
                            LOGI("Set country region band is:%d,region is %d", band, region);
                        }
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=scantable") != NULL) {
                    LOGI("AT+WIFI=scantable,<set/get>,<size>(0 is get, 1 is set)");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    operation = (uint8_t)atoi(param);
                    uint8_t size = 0;
                    if (0 == operation) {
                        ret = wifi_config_get_scan_table_size(&size);
                        if (ret < 0) {
                            LOGE("Get scantable size failed\n");
                            ret = 1;
                        } else {
                            LOGI("Get cantable size is:%d", size);
                        }
                    } else {
                        param = strtok(NULL, ",");
                        size = (uint8_t)atoi(param);
                        ret = wifi_config_set_scan_table_size(size);
                        if (ret < 0) {
                            LOGE("Set scantable size failed\n");
                            ret = 1;
                        } else {
                            LOGI("Set scantable size is:%d", size);
                        }
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=smartfilter") != NULL) {
                    LOGI("AT+WIFI=smartfilter,<set/get>,<flag>(0 is get, 1 is set)");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    operation = (uint8_t)atoi(param);
                    uint8_t flag = 0;
                    if (0 == operation) {
                        ret = wifi_config_get_smart_connection_filter(&flag);
                        if (ret < 0) {
                            LOGE("Get mtk smart connection filter failed\n");
                            ret = 1;
                        } else {
                            LOGI("Get mtk smart connection filter is:(flag=0x%x)", flag);
                        }
                    } else {
                        param = strtok(NULL, ",");
                        flag = (uint8_t)atoi(param);
                        ret = wifi_config_set_smart_connection_filter(flag);
                        if (ret < 0) {
                            LOGE("Set mtk smart connection filter failed\n");
                            ret = 1;
                        } else {
                            LOGI("Set mtk smart connection filter is:(flag=0x%x)", flag);
                        }
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=radio") != NULL) {
                    LOGI("AT+WIFI=radio,<set/get>,<on/off>(0 is get, 1 is set)(0 means off; 1 means on)");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    operation = (uint8_t)atoi(param);
                    uint8_t flag = 0;
                    if (0 == operation) {
                        ret = wifi_config_get_radio(&flag);
                        if (ret < 0) {
                            LOGE("Get WiFi Radio flag failed\n");
                            ret = 1;
                        } else {
                            LOGI("Get WiFi Radio flag is:0x%x", flag);
                        }
                    } else {
                        param = strtok(NULL, ",");
                        flag = (uint8_t)atoi(param);
                        ret = wifi_config_set_radio(flag);
                        if (ret < 0) {
                            LOGE("Set WiFi Radio flag failed\n");
                            ret = 1;
                        } else {
                            LOGI("Set WiFi Radio flag is:0x%x", flag);
                        }
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=rxfilter") != NULL) {
                    LOGI("AT+WIFI=rxfilter,<set/get>,<flag>(0 is get, 1 is set)(flag defined in wifi_rx_filter_t)");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    operation = (uint8_t)atoi(param);
                    uint32_t flag = 0;
                    if (0 == operation) {
                        ret = wifi_config_get_rx_filter(&flag);
                        if (ret < 0) {
                            LOGE("Get rx filter failed\n");
                            ret = 1;
                        } else {
                            LOGI("Get  rx filter  is:0x%x", flag);
                        }
                    } else {
                        param = strtok(NULL, ",");
                        flag = (uint8_t)atoi(param);
                        ret = wifi_config_set_rx_filter(flag);
                        if (ret < 0) {
                            LOGE("Set rx filter failed\n");
                            ret = 1;
                        } else {
                            LOGI("Set rx filter is:0x%x", flag);
                        }
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=rxraw") != NULL) {
                    LOGI("AT+WIFI=rxraw,<enable>(<enable> 0: unregister, 1: register)");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    uint8_t enable = 0;
                    enable = (uint8_t)atoi(param);
                    if (0 == enable) {
                        ret = wifi_config_unregister_rx_handler();
                        if (ret < 0) {
                            LOGE("unregister rx handler failed\n");
                            ret = 1;
                        }
                    } else {
                        ret = wifi_config_register_rx_handler((wifi_rx_handler_t) wlan_raw_pkt_rx_filter_sample);
                        if (ret < 0) {
                            LOGE("register rx handler failed\n");
                            ret = 1;
                        }
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFI=eventcb") != NULL) {
                    LOGI("AT+WIFI=eventcb,<enable>,<event_ID>(<enable> 0: register, 1: unregister)(event_ID refer to wifi_event_t)");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    uint8_t enable = 0;
                    uint8_t event_id = 0;
                    enable = (uint8_t)atoi(param);
                    param = strtok(NULL, ",");
                    event_id = (uint8_t)atoi(param);
                    if (0 == enable) {
                        ret = wifi_connection_unregister_event_handler((wifi_event_t)event_id, (wifi_event_handler_t) scan_event_handler_sample);
                    } else {
                        ret = wifi_connection_register_event_handler((wifi_event_t)event_id, (wifi_event_handler_t) scan_event_handler_sample);
                    }
                    if (ret < 0) {
                        LOGE("set event handler failed\n");
                        ret = 1;
                    } else {
                        LOGI("set event handler success\n");
                    }
                }

                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }

            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}

atci_status_t atci_cmd_hdlr_mac_address(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    uint8_t operation = 0;      //0 is get, 1 is set
    int32_t ret = 0;
    uint8_t addr[WIFI_MAC_ADDRESS_LENGTH] = {0};
    unsigned char addr2[WIFI_MAC_ADDRESS_LENGTH] = {0};
    unsigned char mac_addr_buf[17] = {0};
    LOGI("atci_cmd_hdlr_mac_address");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFIMAC=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFIMAC?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+WIFIMAC\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFIMAC
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFIMAC=<set|get>,<sta|ap>,mac[,<value>] the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            LOGI("AT+WIFIMAC=<set|get>, <sta|ap>, mac[<value>](op: 0 means get; 1 means set)\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFIMAC=") != NULL) {
                    param = strtok((char *)parse_cmd->string_ptr, "=");
                    param = strtok(NULL, ",");
                    LOGI("AT CMD received operation mode:%s", param);
                    operation = (uint8_t)atoi(param);
                    param = strtok(NULL, ",");
                    uint8_t port = (uint8_t)atoi(param);
                    if( atoi(param)<0 ){
                        printf("Invalid port\n");
                        return ATCI_STATUS_ERROR;
                    }
                    if (0 == operation) {
                        ret = wifi_config_get_mac_address(port, addr);
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Get MacAddress failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;

                        } else {
                            for (int i = 0; i < WIFI_MAC_ADDRESS_LENGTH; i++) {
                                addr2[i] = addr[i];
                            }

                            LOGI("wifi_config_get_mac_address(port%ld): (%02x:%02x:%02x:%02x:%02x:%02x)\n", port,
                                 addr2[0], addr2[1], addr2[2],
                                 addr2[3], addr2[4], addr2[5]);
                            sprintf((char *)resonse.response_buf, "SUCCEED:Get MacAddress \n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    } else if (1 == operation) {
                        param = strtok(NULL, "m");
                        param = param + 2;
                        sscanf(param, "%s", mac_addr_buf);
                        char *group_name = (WIFI_PORT_AP == port) ? "AP" : "STA";
#ifdef MTK_NVDM_ENABLE
                        ret = nvdm_write_data_item(group_name, "MacAddr", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)mac_addr_buf, strlen((char *)mac_addr_buf));
                        if (ret != 0) {
                            sprintf((char *)resonse.response_buf, "FAILED: Write NVDM MAC Status=%ld,Port=%d,Mac=%s\n\r", ret, port, mac_addr_buf);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;
                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED: Write Port=%d, Mac=%s, Please Reboot.\n\r", port, mac_addr_buf);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                        LOGI("nvdm_write_data_item Status=%d,Port=%d,Mac=%s\n", ret, port, mac_addr_buf);
#else
                        LOGI("NVDM not enabled\n");
                        sprintf((char *)resonse.response_buf, "FAILED: NVDM not enabled\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;
#endif
                    }
                }
                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }

            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}

atci_status_t atci_cmd_hdlr_ip_address(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    uint8_t operation = 0;   //0 is get, 1 is set
    int32_t ret = 0;
    uint8_t ip_addr[16] = {0};
    int  nvdm_len = sizeof(ip_addr);
    LOGI("atci_cmd_hdlr_mac_address");
    os_memset(&resonse, 0, sizeof(atci_response_t));
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFIIP=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
        case ATCI_CMD_MODE_READ:    // rec: AT+WIFIIP?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+AT+WIFIIP\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;
        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFIIP
            LOGI("AT Active OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFIIP=<set|get>,<sta|ap>,ip[<value>] the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            LOGI("AT+WIFIIP=<set|get>,<sta|ap>,ip[<value>](op: 0 means get; 1 means set)\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFIIP=") != NULL) {
                    param = strtok((char *)parse_cmd->string_ptr, "=");
                    param = strtok(NULL, ",");
                    LOGI("AT CMD received operation mode:%s", param);
                    operation = (uint8_t)atoi(param);
                    param = strtok(NULL, ",");
                    uint8_t port = (uint8_t)atoi(param);
                    if ( atoi(param) < 0) {
                        printf("Invalid port\n");
                        return ATCI_STATUS_ERROR;
                    }
#ifdef MTK_NVDM_ENABLE
                    char *group_name = "network";
                    if (0 == operation) {
                        ret = nvdm_read_data_item(group_name, "IpAddr", (uint8_t *)ip_addr, (uint32_t *)&nvdm_len);
                        if (ret != 0) {
                            sprintf((char *)resonse.response_buf, "FAILED: Read NVDM IpAddress Status=%ld\n\r", ret);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;
                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED: Read NVDM IpAddress Status=%ld, Port=%d, IP=%s\n\r", ret, port, ip_addr);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    } else if (1 == operation) {
                        param = strtok(NULL, "i");
                        param = param + 1;
                        sscanf(param, "%s", ip_addr);
                        ret = nvdm_write_data_item(group_name, "IpAddr", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)ip_addr, strlen((char *)ip_addr));
                        if (ret != 0) {
                            sprintf((char *)resonse.response_buf, "FAILED: Write NVDM IpAddress Status=%ld\n\r", ret);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;
                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED: Write Port=%d, IP=%s, Please Reboot.\n\r", port, ip_addr);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                        LOGI("nvdm_write_data_item Status=%ld,Port=%d,IP=%s\n", ret, port, ip_addr);
                    }
#else
                    LOGI("NVDM not enabled\n");
                    sprintf((char *)resonse.response_buf, "FAILED: NVDM not enabled\n\r");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    ret = 1;
#endif

                }
                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }
            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }
            atci_send_response(&resonse);
            param = NULL;
            break;
        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}

#if defined(MTK_SMTCN_V5_ENABLE) || defined(MTK_SMTCN_V4_ENABLE)
atci_status_t atci_cmd_hdlr_smart_connection(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    uint8_t smnt_flag = 0;     //1 is start, 0 is stop
    int32_t ret = 0;
    LOGI("atci_cmd_hdlr_smart_connection");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+SMNT=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+SMNT?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+SMNT:<smnt_flag>\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+SMNT
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+SMNT=<smnt_flag>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            printf("AT+SMNT=<smnt_flag> (smnt_flag: 1 is start, 0 is stop)\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+SMNT=") != NULL) {
                    LOGI("AT CMD received smnt_flag:%s", param);
                    param = strtok((char *)parse_cmd->string_ptr, "AT+SMNT=");
                    smnt_flag = (uint8_t)atoi(param);
                    if (0 == smnt_flag) {
                        ret = mtk_smart_stop();
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Stop smart connect failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;

                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED:Stop smart connect success\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    } else if (1 == smnt_flag) {
                        ret = mtk_smart_connect();
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Start smart connect failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;

                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED:Start smart connect success\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    }
                }

                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }

            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}
#endif

atci_status_t atci_cmd_hdlr_ch_bw(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    uint8_t operation = 0;      //0 is get, 1 is set
    uint8_t port = 0;
    int32_t ret = 0;
    uint8_t ch = 0;
    uint8_t bw = 0;

    LOGI("atci_cmd_hdlr_ch_bw");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFICH=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFICH?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+WIFICH\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFICH
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFICH=<set|get>,<sta|ap>,c<channel>,b<bandwidth>
            LOGI("AT Executing...\r\n");
            LOGI("AT+WIFICH=<set|get>,<sta|ap>,c<channel>,b<bandwidth>(op: 0 means get; 1 means set)\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFICH=") != NULL) {
                    param = strtok((char *)parse_cmd->string_ptr, "=");
                    param = strtok(NULL, ",");
                    LOGI("AT CMD received operation mode:%s", param);
                    operation = (uint8_t)atoi(param);
                    param = strtok(NULL, "c");
                    port = (uint8_t)atoi(param);
                    if ( atoi(param) < 0) {
                        printf("Invalid port\n");
                        return ATCI_STATUS_ERROR;
                    }

                    if (0 == operation) {
                        ret = wifi_config_get_channel(port, &ch);
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Get Channel failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;

                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED:Get Channel:%d\n\r", ch);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }

                        ret = wifi_config_get_bandwidth(port, &bw);
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf + resonse.response_len, "FAILED:Get Bandwidth failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;

                        } else {
                            sprintf((char *)resonse.response_buf + resonse.response_len, "SUCCEED:Get Bandwidth:%d\n\r", bw);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    } else if (1 == operation) {
                        param = strtok(NULL, "b");
                        ch = atoi(param);
                        param = strtok(NULL, ",");
                        bw = atoi(param);
                        ret = wifi_config_set_channel(port, ch);
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Set Channel failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;

                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED:Set Channel:%d\n\r", ch);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }

                        ret = wifi_config_set_bandwidth(port, bw);
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf + resonse.response_len, "FAILED:Set Bandwidth failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;

                        } else {
                            sprintf((char *)resonse.response_buf + resonse.response_len, "SUCCEED:Set Bandwidth:%d\n\r", bw);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    }
                }

                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }

            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}

atci_status_t atci_cmd_hdlr_block_ack(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    int32_t ret = 0;
    uint8_t operation = 0;      //0 is get, 1 is set
    uint8_t block_ack[2] = {0};
    int  nvdm_len = sizeof(block_ack);

    LOGI("atci_cmd_hdlr_block_ack");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFIBASupport=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFIBASupport?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+WIFIBASupport\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFIBASupport
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFIBASupport=<set|get>,<sta|ap>,<on|off>
            LOGI("AT Executing...\r\n");
            LOGI("AT+WIFIBASupport=<set|get>,<sta|ap>,<on|off>(0 means off; 1 means on)\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFIBASupport=") != NULL) {
                    param = strtok((char *)parse_cmd->string_ptr, "=");
                    param = strtok(NULL, ",");
                    LOGI("AT CMD received operation mode:%s", param);
                    operation = (uint8_t)atoi(param);
                    param = strtok(NULL, ",");
                    uint8_t port = (uint8_t)atoi(param);
                    if ( atoi(param) < 0) {
                        printf("Invalid port\n");
                        return ATCI_STATUS_ERROR;
                    }
#ifdef MTK_NVDM_ENABLE
                    char *group_name = (WIFI_PORT_AP == port) ? "AP" : "STA";
                    if (0 == operation) {
                        ret = nvdm_read_data_item(group_name, "AutoBA", (uint8_t *)block_ack, (uint32_t *)&nvdm_len);
                        if (ret != 0) {
                            sprintf((char *)resonse.response_buf, "FAILED: Read NVDM Block Ack Status=%ld\n\r", ret);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;
                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED: Read NVDM Block Ack Status=%ld, Port=%d, AutoBA=%c\n\r", ret, port, block_ack[0]);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    } else if (1 == operation) {
                        param = strtok(NULL, ",");
                        ret = nvdm_write_data_item(group_name, "AutoBA", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)param, strlen(param));
                        if (ret != 0) {
                            sprintf((char *)resonse.response_buf, "FAILED: Write NVDM Block Ack Status=%ld\n\r", ret);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;
                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED: Write Block Ack Status=%ld Port=%d, AutoBA=%c, Please Reboot.\n\r", ret, port, param[0]);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
#else
                    LOGI("NVDM not enabled\n");
                    sprintf((char *)resonse.response_buf, "FAILED: NVDM not enabled\n\r");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    ret = 1;
#endif
                    }
                }

                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }

            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}

atci_status_t atci_cmd_hdlr_wireless_mode(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    uint8_t operation = 0;      //0 is get, 1 is set
    uint8_t port = 0;
    int32_t ret = 0;
    uint8_t mode = 0;

    LOGI("atci_cmd_hdlr_ch_bw");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFIWirelessMode=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFIWirelessMode?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+WIFIWirelessMode\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFIWirelessMode
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFIWirelessMode=<set|get>,<sta|ap>,<WirelessMode>
            LOGI("AT Executing...\r\n");
            LOGI("AT+WIFIWirelessMode=<set|get>,<sta|ap>,<WirelessMode>(op: 0 means get; 1 means set)\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFIWirelessMode=") != NULL) {
                    param = strtok((char *)parse_cmd->string_ptr, "=");
                    param = strtok(NULL, ",");
                    LOGI("AT CMD received operation mode:%s", param);
                    operation = (uint8_t)atoi(param);
                    param = strtok(NULL, ",");
                    port = (uint8_t)atoi(param);
                    if ( atoi(param) < 0) {
                        printf("Invalid port\n");
                        return ATCI_STATUS_ERROR;
                    }
                    param = strtok(NULL, ",");
                    mode = atoi(param);
                    if (0 == operation) {
                        ret = wifi_config_get_wireless_mode(port, &mode);
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Get WirelessMode failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;

                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED:Get WirelessMode:%d\n\r", mode);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    } else if (1 == operation) {
                        ret = wifi_config_set_wireless_mode(port, mode);
                        if (ret < 0) {
                            sprintf((char *)resonse.response_buf, "FAILED:Set WirelessMode failed\n\r");
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 1;

                        } else {
                            sprintf((char *)resonse.response_buf, "SUCCEED:Set WirelessMode:%d\n\r", mode);
                            resonse.response_len = strlen((char *)resonse.response_buf);
                            ret = 0;
                        }
                    }
                }

                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }

            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}

atci_status_t atci_cmd_hdlr_wifi_tx_raw_cmd(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    int32_t ret = 0;
    uint8_t *hex_buf = NULL;
    uint32_t len_hex_buf = 0;
    int i_hex = 0;

    LOGI("atci_cmd_hdlr_ch_bw");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFITxRawPkt=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFITxRawPkt?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+AT+WIFITxRawPkt\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFITxRawPkt
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFITxRawPkt=<raw pkt>
            LOGI("AT Executing...\r\n");
            LOGI("AT+WIFITxRawPkt=<raw pkt>\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFITxRawPkt=") != NULL) {
                    param = param + 16;
                    LOGI("payload:%s ", param);
                    len_hex_buf = strlen(param) / 2 + strlen(param) % 2;
                    if ((hex_buf = (uint8_t *)os_malloc(len_hex_buf)) == NULL) {
                        LOGE("alloc buffer failed.\n");
                        return 1;
                    }
                    os_memset(hex_buf, 0, len_hex_buf);
                    for (int i = 0; i < strlen(param); i++) {
                        char cc = param[i];
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
                            LOGE("input invalid\n");
                            if( NULL!=hex_buf ){
                                os_free(hex_buf);
                            }
                            return 1;
                        }
                    }

                    ret = wifi_connection_send_raw_packet(hex_buf, len_hex_buf);

                    if (ret < 0) {
                        sprintf((char *)resonse.response_buf, "FAILED:Send raw packet failed\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;

                    } else {
                        sprintf((char *)resonse.response_buf, "SUCCEED:Send raw packet success\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;
                    }

                }

                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }

            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    if( NULL!=hex_buf ){
        os_free(hex_buf);
    }
    return ATCI_STATUS_OK;
}

atci_status_t atci_cmd_hdlr_wifi_sta_cmd(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    char *key = NULL;
    uint8_t port = 0;    // STA port
    uint8_t link = 0;   // 0 is disconnected, 1 is connected
    uint8_t auth = 0;
    uint8_t encrypt = 0;
    wifi_wep_key_t wep_key = {{{0}}};
    int32_t ret = 0;

    LOGI("atci_cmd_hdlr_wifi_sta_cmd");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFISTA=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFISTA?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+WIFIWSTA\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFISTA
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFISTA=xxx,xxx,xxx,xxx
            LOGI("AT Executing...\r\n");
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=disconnect") != NULL) {
                    ret = wifi_connection_disconnect_ap();
                    if (ret < 0) {
                        sprintf((char *)resonse.response_buf, "FAILED:Disconnect failed\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;

                    } else {
                        sprintf((char *)resonse.response_buf, "SUCCEED:Disconnect Success\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=connectinfo") != NULL) {
                    ret = wifi_connection_get_link_status(&link);
                    if (ret < 0) {
                        sprintf((char *)resonse.response_buf, "FAILED:Get Link Status failed\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;

                    } else {
                        sprintf((char *)resonse.response_buf, "SUCCEED:Current link status in station mode:%d\n\r", link);
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;
                    }
                    sprintf((char *)parse_cmd->string_ptr, "0");
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=connect") != NULL) {
                    LOGI("AT+WIFISTA=connect,s<ssid>,a<authMode>,e<encryption>,p<key>,<wep_key_index>\n");
                    param = strtok((char *)parse_cmd->string_ptr, "s");
                    param = strtok(NULL, ",");
                    LOGI("ssid=%s", param);
                    ret = wifi_config_set_ssid(port, (uint8_t *)param, os_strlen(param));
                    if (ret < 0) {
                        printf("Set ssid failed\n");
                        ret = 1;
                    }
                    param = strtok(NULL, ",");
                    param = param + 1;
                    auth = atoi(param);
                    LOGI("auth=%d", auth);
                    param = strtok(NULL, ",");
                    param = param + 1;
                    encrypt = atoi(param);
                    LOGI("encrypt=%d", encrypt);
                    ret = wifi_config_set_security_mode(port, (wifi_auth_mode_t)auth, (wifi_encrypt_type_t)encrypt);
                    if (ret < 0) {
                        printf("Set auth mode failed\n");
                        ret = 1;
                    }
                    if ((auth == 0) && (encrypt == 0)) {
                        LOGI("WEP encrypt\n\n");
                        param = strtok(NULL, ",");
                        param = param + 1;
                        key = param;
                        LOGI("WEP key=%s", key);
                        param = strtok(NULL, ",");
                        param = param + 1;
                        wep_key.wep_tx_key_index = atoi(param);
                        LOGI("WEP key index=%d", wep_key.wep_tx_key_index);
                        if (os_strlen(key) == 10 || os_strlen(key) == 26) {
                            wep_key.wep_key_length[wep_key.wep_tx_key_index] = os_strlen(key) / 2;
                            AtoH((char *)key, (char *)&wep_key.wep_key[wep_key.wep_tx_key_index], (int)wep_key.wep_key_length[wep_key.wep_tx_key_index]);
                        } else if (os_strlen(key) == 5 || os_strlen(key) == 13) {
                            os_memcpy(wep_key.wep_key[wep_key.wep_tx_key_index], key, os_strlen(key));
                            wep_key.wep_key_length[wep_key.wep_tx_key_index] = os_strlen(key);
                        } else {
                            printf("invalid length of value.\n");
                            return 1;
                        }

                        ret = wifi_config_set_wep_key(port, &wep_key);

                    } else if ((auth != 0) && (encrypt != 1)) {
                        param = strtok(NULL, ",");
                        param = param + 1;
                        LOGI("password=%s", param);
                        ret = wifi_config_set_wpa_psk_key(port, (uint8_t *)param, os_strlen(param));
                    }
                    ret = wifi_config_reload_setting();
                    if (ret < 0) {
                        printf("Connect to network failed\n");
                        ret = 1;
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=scanmode") != NULL) {
                    LOGI("AT+WIFISTA=scanmode,<start/stop>,<active|passive>,<partial/full> (0:stop 1:start;0:full 1:partial; 0:active 1:passive)\n");
                    param = strtok((char *)parse_cmd->string_ptr, ",");
                    param = strtok(NULL, ",");
                    uint8_t start = atoi(param);
                    if (!start) {
                        ret = wifi_connection_stop_scan();
                    } else {
                        param = strtok(NULL, ",");
                        uint8_t scan_mode = atoi(param);
                        param = strtok(NULL, ",");
                        uint8_t scan_option = atoi(param);
                        wifi_connection_scan_init(scan_ap_list, 8);
                        LOGI("size = %d\n", sizeof(wifi_scan_list_item_t));
                        ret = wifi_connection_start_scan(NULL, 0, NULL, scan_mode, scan_option);
                    }
                    if (ret < 0) {
                        ret = 1;
                    }
                    sprintf((char *)parse_cmd->string_ptr, "0");
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=scan") != NULL) {
                    LOGI("AT+WIFISTA=scan,<max_ap_num> \n");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    uint32_t number = atoi(param);
                    if (1 == g_scan_inited) {
                        wifi_connection_scan_deinit();
                    }
                    if (scan_ap_list != NULL) {
                        os_free(scan_ap_list);
                        scan_ap_list = NULL;
                    }
                    scan_ap_list = os_malloc(sizeof(wifi_scan_list_item_t) * number);
                    if (scan_ap_list == NULL) {
                        LOGE("alloc buffer failed.\n");
                        ret = 1;
                    }
                    scan_ap_size = number;
                    wifi_connection_register_event_handler(1, (wifi_event_handler_t) scan_event_handler_sample);
                    wifi_connection_scan_init(scan_ap_list, number);
                    LOGI("size = %d\n", sizeof(wifi_scan_list_item_t));
                    ret = wifi_connection_start_scan(NULL, 0, NULL, 0, 0);
                    if (ret < 0) {
                        ret = 1;
                        os_free(scan_ap_list);
                        scan_ap_list = NULL;
                    }
                }
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=network") != NULL) {
                    LOGI("AT+WIFISTA=network, <dhcp|static> \n");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    char *group_name = "network";
#ifdef MTK_NVDM_ENABLE
                    ret = nvdm_write_data_item(group_name, "IpMode", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)param, strlen(param));
                    if (ret != 0) {
                        sprintf((char *)resonse.response_buf, "FAILED: Write NVDM IpMode Status=%ld\n\r", ret);
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;
                    } else {
                        sprintf((char *)resonse.response_buf, "SUCCEED: Write IpMode =%s, Please Reboot.\n\r", param);
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;
                    }
#else
                    LOGI("NVDM not enabled\n");
                    sprintf((char *)resonse.response_buf, "FAILED: NVDM not enabled\n\r");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    ret = 1;
#endif
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=psmode") != NULL) {
                    LOGI("AT+WIFISTA=psmode,<pm>\n");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    char *group_name = "STA";
#ifdef MTK_NVDM_ENABLE
                    ret = nvdm_write_data_item(group_name, "PSMode", NVDM_DATA_ITEM_TYPE_STRING, (uint8_t *)param, strlen(param));
                    if (ret != 0) {
                        sprintf((char *)resonse.response_buf, "FAILED: Write NVDM PSMode Status=%ld\n\r", ret);
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;
                    } else {
                        sprintf((char *)resonse.response_buf, "SUCCEED: Write PSMode =%c, Please Reboot.\n\r", *param);
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;
                    }
#else
                    LOGI("NVDM not enabled\n");
                    sprintf((char *)resonse.response_buf, "FAILED: NVDM not enabled\n\r");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    ret = 1;
#endif
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFISTA=rssi") != NULL) {
                    LOGI("AT+WIFISTA=rssi(Get rssi of the connected AP)");
                    int8_t rssi = 0;
                    ret = wifi_connection_get_rssi(&rssi);
                    if (ret < 0) {
                        sprintf((char *)resonse.response_buf, "FAILED:Get rssi of the connected AP failed\n\r");
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 1;

                    } else {
                        sprintf((char *)resonse.response_buf, "SUCCEED:Get rssi of the connected AP is :%d\n\r", rssi);
                        resonse.response_len = strlen((char *)resonse.response_buf);
                        ret = 0;
                    }
                    sprintf((char *)parse_cmd->string_ptr, "0");
                }

                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }

            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}


atci_status_t atci_cmd_hdlr_wifi_ap_cmd(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    char *key = NULL;
    uint8_t port = 1;    // AP port
    uint8_t auth = 0;
    uint8_t encrypt = 0;
    wifi_wep_key_t wep_key = {{{0}}};
    int32_t ret = 0;

    LOGI("atci_cmd_hdlr_wifi_sta_cmd");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFIAP=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFIAP?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+AT+WIFIAP\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFIAP
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFIAP=xxx,xxx,xxx,xxx
            LOGI("AT Executing...\r\n");
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFIAP=connect") != NULL) {
                    LOGI("AT+WIFIAP=connect,s<ssid>,a<authMode>,e<encryption>,p<key>,<wep_key_index>\n");
                    param = strtok((char *)parse_cmd->string_ptr, "s");
                    param = strtok(NULL, ",");
                    LOGI("ssid=%s", param);
                    ret = wifi_config_set_ssid(port, (uint8_t *)param, os_strlen(param));
                    if (ret < 0) {
                        LOGE("Set ssid failed\n");
                        ret = 1;
                    }
                    param = strtok(NULL, ",");
                    param = param + 1;
                    auth = atoi(param);
                    LOGI("auth=%d", auth);
                    param = strtok(NULL, ",");
                    param = param + 1;
                    encrypt = atoi(param);
                    LOGI("encrypt=%d", encrypt);
                    ret = wifi_config_set_security_mode(port, (wifi_auth_mode_t)auth, (wifi_encrypt_type_t)encrypt);
                    if (ret < 0) {
                        printf("Set auth mode failed\n");
                        ret = 1;
                    }
                    if ((auth == 0) && (encrypt == 0)) {
                        LOGI("WEP encrypt\n\n");
                        param = strtok(NULL, ",");
                        param = param + 1;
                        key = param;
                        LOGI("WEP key=%s", key);
                        param = strtok(NULL, ",");
                        param = param + 1;
                        wep_key.wep_tx_key_index = atoi(param);
                        LOGI("WEP key index=%d", wep_key.wep_tx_key_index);
                        if (os_strlen(key) == 10 || os_strlen(key) == 26) {
                            wep_key.wep_key_length[wep_key.wep_tx_key_index] = os_strlen(key) / 2;
                            AtoH((char *)key, (char *)&wep_key.wep_key[wep_key.wep_tx_key_index], (int)wep_key.wep_key_length[wep_key.wep_tx_key_index]);
                        } else if (os_strlen(key) == 5 || os_strlen(key) == 13) {
                            os_memcpy(wep_key.wep_key[wep_key.wep_tx_key_index], key, os_strlen(key));
                            wep_key.wep_key_length[wep_key.wep_tx_key_index] = os_strlen(key);
                        } else {
                            printf("invalid length of value.\n");
                            return 1;
                        }

                        ret = wifi_config_set_wep_key(port, &wep_key);

                    } else if ((auth != 0) && (encrypt != 1)) {
                        param = strtok(NULL, ",");
                        param = param + 1;
                        LOGI("password=%s", param);
                        ret = wifi_config_set_wpa_psk_key(port, (uint8_t *)param, os_strlen(param));
                    }

                    ret = wifi_config_reload_setting();
                    if (ret < 0) {
                        LOGE("Connect to network failed\n");
                        ret = 1;
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFIAP=deauth") != NULL) {
                    LOGI("AT+WIFIAP=deauth,<mac>\n");
                    unsigned char addr[WIFI_MAC_ADDRESS_LENGTH] = {0};
                    param = strtok((char *)parse_cmd->string_ptr, ",");
                    param = strtok(NULL, ",");
                    wifi_conf_get_mac_from_str((char *)addr, param);
                    ret = wifi_connection_disconnect_sta(addr);
                    if (ret < 0) {
                        ret = 1;
                    }
                    LOGI("wifi_connect_deauth_station(%02x:%02x:%02x:%02x:%02x:%02x)\n",
                         addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFIAP=query") != NULL) {
                    LOGI("AT+WIFIAP=query,<sta_list> (0:get the current conencted station list)\n");
                    param = strtok((char *)parse_cmd->string_ptr, ",");
                    param = strtok(NULL, ",");
                    if (0 == atoi(param)) {
                        wifi_sta_list_t list[WIFI_MAX_NUMBER_OF_STA];
                        uint8_t size = 0;
                        ret = wifi_connection_get_sta_list(&size, list);
                        if (ret < 0) {
                            LOGE("get stalist failed\n");
                            ret = 1;
                        } else {
                            printf("stalist size=%d\n", size);
                            for (int i = 0; i < size; i++) {
                                printf("%d\n", i);
                                printf("    last_tx_rate: MCS=%d, LDPC=%d, MODE=%d\n", (list[i].last_tx_rate.field.mcs), (list[i].last_tx_rate.field.ldpc), (list[i].last_tx_rate.field.mode));
                                printf("    last_rx_rate: MCS=%d, LDPC=%d, MODE=%d\n", (list[i].last_rx_rate.field.mcs), (list[i].last_rx_rate.field.ldpc), (list[i].last_rx_rate.field.mode));
                                printf("    rssi_sample.LastRssi0)=%d\n", (int)(list[i].rssi_sample.last_rssi));
                                printf("    rssi_sample.AvgRssi0X8=%d\n", (int)(list[i].rssi_sample.average_rssi));
                                printf("    addr=%02x:%02x:%02x:%02x:%02x:%02x\n", list[i].mac_address[0],
                                       list[i].mac_address[1],
                                       list[i].mac_address[2],
                                       list[i].mac_address[3],
                                       list[i].mac_address[4],
                                       list[i].mac_address[5]);
                                printf("    power_save_mode=%d\n", (unsigned int)(list[i].power_save_mode));
                                printf("    bandwidth=%d\n", (unsigned int)(list[i].bandwidth));
                                printf("    keep_alive=%d\n", (unsigned int)(list[i].keep_alive));
                            }
                        }
                    }
                }
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFIAP=network") != NULL) {
                    LOGI("AT+WIFIAP=network,<dhcpd|static>,i<IP>,m<IpNetmask>,g<IpGateway>,p<PRIMARY_DNS>,s<SECONDARY_DNS>,t<IP_POOL_START>,e<IP_POOL_END> (dhcpd:1;static:0)\n");
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    uint8_t dhcpd_flag = atoi(param);
                    uint8_t origin_op_mode = 0;
                    wifi_config_get_opmode(&origin_op_mode);
                    if (origin_op_mode == WIFI_MODE_AP_ONLY) {
                        dhcpd_stop();
                        if (dhcpd_flag == 1) {
                            LOGI("dhcpd start\n");
                            lwip_net_start(WIFI_MODE_AP_ONLY);

                        } else if (dhcpd_flag == 0) {
                            LOGI("use static dhcpd\n");
                            struct netif *ap_if;
                            dhcpd_settings_t dhcpd_settings = {{0}, {0}, {0}, {0}, {0}, {0}, {0}};
                            param = strtok(NULL, ",");
                            param = param + 1;
                            strcpy((char *)dhcpd_settings.dhcpd_server_address, param);
                            param = strtok(NULL, ",");
                            param = param + 1;
                            strcpy((char *)dhcpd_settings.dhcpd_netmask, param);
                            param = strtok(NULL, ",");
                            param = param + 1;
                            strcpy((char *)dhcpd_settings.dhcpd_gateway, param);
                            param = strtok(NULL, ",");
                            param = param + 1;
                            strcpy((char *)dhcpd_settings.dhcpd_primary_dns, param);
                            param = strtok(NULL, ",");
                            param = param + 1;
                            strcpy((char *)dhcpd_settings.dhcpd_secondary_dns, param);
                            param = strtok(NULL, ",");
                            param = param + 1;
                            strcpy((char *)dhcpd_settings.dhcpd_ip_pool_start, param);
                            param = strtok(NULL, "e");
                            strcpy((char *)dhcpd_settings.dhcpd_ip_pool_end, param);
                            ap_if = netif_find_by_type(NETIF_TYPE_AP);
                            netif_set_default(ap_if);
                            netif_set_link_up(ap_if);
                            dhcpd_start(&dhcpd_settings);
                        }
                    } else {
                        LOGE("This command should be used in AP mode");
                        ret = 1;
                    }
                }

                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFIAP=maxnum") != NULL) {
                    LOGI("AT+WIFIAP=maxnum(Get the max number of the supported stations in ap mode)");
                    uint8_t number = 0;
                    ret = wifi_connection_get_max_sta_number(&number);
                    if (ret < 0) {
                        LOGE("Get the max number of the supported stations FAILED");
                        ret = 1;
                    } else {
                        LOGI("Get the max number of the supported stations is %d", number);
                    }

                }

                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }

            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;
}

#ifdef MTK_WIFI_WPS_ENABLE
atci_status_t  atci_cmd_hdlr_wifi_wps_cmd(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    uint8_t port = 0;
    int32_t ret = 0;
    LOGI("atci_cmd_hdlr_wifi_wps_cmd");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+WIFIWPS=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+WIFIWPS?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+AT+WIFIWPS\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+WIFIWPS
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+WIFIWPS=XXX
            LOGI("AT Executing...\r\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "\r\n");
            if (param != NULL) {
                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFIWPS=pincode") != NULL) {
                    LOGI("AT+WIFIWPS=pincode,<sta/ap>(0 is sta; 1 is ap)");
                    LOGI("AT CMD received command:%s", param);
                    param = strtok(param, ",");
                    param = strtok(NULL, ",");
                    port = (uint8_t)atoi(param);
                    if ( atoi(param) < 0) {
                        printf("Invalid port\n");
                        return ATCI_STATUS_ERROR;
                    } else {
                        char pin_code[9] = {0};
                        ret = wifi_wps_config_get_pin_code(port, (uint8_t *)&pin_code);
                        if (ret < 0) {
                            LOGE("Get pin code fail");
                            ret = 1;
                        } else {
                            LOGI("Get the PIN code is %s", pin_code);
                        }
                    }
                }


                if (strstr((char *)parse_cmd->string_ptr, "AT+WIFIWPS=connect") != NULL) {
                    LOGI("AT+WIFIWPS=connect,<flag>,<sta/ap>,<BSSID>,<PIN code>");
                    printf("<flag>:0: pbc; 1: pin\n");
                    printf("<sta/ap>:0: sta;  1:ap\n");
                    printf("<BSSID>:any: no special AP BSSID; xx:xx:xx:xx:xx:xx :48 bit AP BSSID address.\n");
                    printf("<PIN code>:PIN code: Support both Static PIN code and Dynamic PIN code input.\n");
                    LOGI("AT CMD received command:%s", param);
                    param = strtok((char *)parse_cmd->string_ptr, ",");
                    param = strtok(NULL, ",");
                    uint8_t flag = atoi(param);
                    param = strtok(NULL, ",");
                    port = atoi(param);
                    uint8_t bssid[WIFI_MAC_ADDRESS_LENGTH] = {0};
                    uint8_t *_bssid = bssid;
                    param = strtok(NULL, ",");
                    if (NULL == param || 0 == os_strcmp(param, "any")) {
                        _bssid = NULL;
                    } else {
                        //Mac address acceptable format 01:02:03:04:05:06 length 17
                        if (17 != strlen(param)) {
                            printf("wifi wps connection fail, invalid BSSID len");
                            ret = 1;
                        }
                        wifi_conf_get_mac_from_str((char *)_bssid, param);
                    }

                    if (0 == flag) {
                        LOGI("wps pbc process\n");
                        ret = wifi_wps_connection_by_pbc(port, _bssid);
                        if (ret < 0) {
                            printf("wifi wps connection by pbc fail\n");
                            ret = 1;
                        }
                    } else {
                        param = strtok(NULL, ",");
                        char pin[9] = {0};
                        os_memcpy(pin, param, strlen(param));
                        if (8 != (strlen(pin)) && 4 != (strlen(pin))) {
                            printf("wifi wps connection by pin fail: incorrect PIN\n");
                            ret = 1;
                        }
                        ret = wifi_wps_connection_by_pin(port, _bssid, (uint8_t *)pin);
                        if (ret < 0) {
                            printf("wifi wps connection by pin fail\n");
                            ret = 1;
                        }
                    }
                }

                if (0 == ret) {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK; // ATCI will help append "OK" at the end of resonse buffer
                } else {
                    resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR; // ATCI will help append "ERROR" at the end of resonse buffer
                }

            } else { /*else  of if(NULL == param)*/
                sprintf((char *)resonse.response_buf, "FAILED:Invalide Input\n\r");
                resonse.response_len = strlen((char *)resonse.response_buf);
                resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of resonse buffer
            }

            atci_send_response(&resonse);
            param = NULL;
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }

    return ATCI_STATUS_OK;

}
#endif

atci_status_t atci_cmd_hdlr_iperf(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;

    LOGI("atci_cmd_hdlr_iperf\n");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    resonse.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+IPERF=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\n+IPERF:(0,1)\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+IPERF?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+IPERF:<ip>\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;    // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:    // rec: AT+IPERF
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+IPERF=<p1>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            LOGI("AT CMD received: %s", parse_cmd->string_ptr);

            param = strtok((char *)parse_cmd->string_ptr, "AT+IPERF=");
            if (param != NULL) {
                int len = strlen((char *)(param + 3));
                if (param[0] == '-' && param[1] == 'c') {
                    char *str = (char *)pvPortMalloc(len + 1);
                    str[len] = '\0';
                    os_memcpy(str, param + 3, len);
                    atci_cmd_iperf_client(str);
                    vPortFree(str);
                } else if (param[0] == '-' && param[1] == 's') {
                    char *str = (char *)pvPortMalloc(len + 1);
                    str[len] = '\0';
                    os_memcpy(str, param + 3, len);
                    atci_cmd_iperf_server(str);
                    vPortFree(str);
                } else {
                    strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
                    resonse.response_len = strlen((char *)resonse.response_buf);
                    atci_send_response(&resonse);
                    break;
                }
                strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
                resonse.response_len = strlen((char *)resonse.response_buf);
                atci_send_response(&resonse);
                break;
            }
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;

}


static uint8_t atci_cmd_iperf_server(char *param)
{
    int i;
    char **g_iperf_param = NULL;
    int offset = IPERF_COMMAND_BUFFER_SIZE / sizeof(char *);
    char *p = NULL;
    int is_create_task = 0;
    LOGI("atci_cmd_iperf_server.\n");

    if (param == NULL) {
        return 0;
    }
    g_iperf_param = pvPortMalloc(IPERF_COMMAND_BUFFER_NUM * IPERF_COMMAND_BUFFER_SIZE);
    if (g_iperf_param == NULL) {
        return 0;
    }

#ifdef MODEM_ENABLE
    sio_set_mode(gprs_get_app_id(), SIO_DATA_TYPE_DATA);
    iperf_register_callback(atci_cmd_iperf_cb);
#endif

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
        is_create_task = 1;
    }

    if (is_create_task == 0) {
        vPortFree(g_iperf_param);
    }
    return 0;
}

static uint8_t atci_cmd_iperf_client(char *param)
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

#ifdef MODEM_ENABLE
    sio_set_mode(gprs_get_app_id(), SIO_DATA_TYPE_DATA);
    iperf_register_callback(atci_cmd_iperf_cb);
#endif

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


#ifdef MODEM_ENABLE
static void atci_cmd_iperf_cb(iperf_result_t *iperf_result)
{
    atci_response_t resonse;

    vTaskDelay(3000);

    sio_set_mode(gprs_get_app_id(), SIO_DATA_TYPE_COMMAND);

    // TODO: handle the result
    resonse.response_flag = 0; // Command Execute Finish.
    if (iperf_result)
        sprintf((char *)resonse.response_buf, "\r\n iperf finish, %s, data_size = %d, total = %s, result = %s \r\n",
                iperf_result->report_title, (int)iperf_result->data_size, iperf_result->total_len, iperf_result->result);
    else {
        strcpy((char *)resonse.response_buf, "\r\n iperf finish, no result!\r\n");
    }
    resonse.response_len = strlen((char *)resonse.response_buf);
    atci_send_response(&resonse);
}
#endif

atci_status_t atci_cmd_hdlr_ping(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    int count = 3;
    int pktsz = 64;
    char ip[16] = {0};
    char *p = NULL;

    LOGI("atci_cmd_hdlr_ping\n");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    resonse.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+PING=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+PING?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+PING:<ip>\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+PING
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+PING=<p1>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "AT+PING=");
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
            LOGI("AT CMD received IP:%s, count:%d, ptksz:%d", ip, count, pktsz);
            ping_request(count, ip, PING_IP_ADDR_V4, pktsz, ping_request_result);

            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}

static void ping_request_result(ping_result_t *result)
{
    atci_response_t resonse = {{0}};
    int len = 0;

    resonse.response_flag = 0; // Command Execute Finish.
    resonse.response_len  = 0;
    sprintf((char *)resonse.response_buf, "Packets: Sent = %d, Received =%d, Lost = %d (%d%% loss)\n\r", (int)result->total_num, (int)result->recv_num, (int)result->lost_num, (int)((result->lost_num * 100) / result->total_num));
    len = strlen((char *)resonse.response_buf);
    sprintf((char *)(resonse.response_buf + len), "Packets: min = %d, max =%d, avg = %d\n", (int)result->min_time, (int)result->max_time, (int)result->avg_time);
    resonse.response_len = strlen((char *)resonse.response_buf);
    atci_send_response(&resonse);
}

atci_status_t atci_cmd_hdlr_ip(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse;
    char *param = NULL;
    uint8_t operation = 0;   //0 is get, 1 is set
    uint8_t port = 0;
    netif_type_t type = NETIF_TYPE_STA;
    char ip[16] = {0};
    char netmask[16] = {0};
    char gw[16] = {0};
    char *p = NULL;

    LOGI("atci_cmd_hdlr_ping\n");
    os_memset(&resonse, 0, sizeof(atci_response_t));

    resonse.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+IP=?
            LOGI("AT TEST OK.\n");
            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_READ:    // rec: AT+IP?
            LOGI("AT Read done.\n");
            sprintf((char *)resonse.response_buf, "\r\n+IP:<get|set>,<sta|ap>,ip[<value>](op: 0 means get; 1 means set)\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            resonse.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append OK at the end of resonse buffer
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_ACTIVE:  // rec: AT+IP
            LOGI("AT Active OK.\n");
            // assume the active mode is invalid and we will return "ERROR"
            strcpy((char *)resonse.response_buf, "\r\nERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+IP=<p1>  the handler need to parse the parameters
            LOGI("AT Executing...\r\n");
            LOGI("AT CMD received:%s ", parse_cmd->string_ptr);
            param = strtok((char *)parse_cmd->string_ptr, "AT+IP=");
            p = strtok(param, ",");
            if (p != NULL) {
                operation = (uint8_t)atoi(p);
                p = strtok(NULL, ",");
                if (p != NULL) {
                    port = (uint8_t)atoi(p);
                }
            }
            if (port == 0) {
                type = NETIF_TYPE_STA;
            } else if (port == 1) {
                type = NETIF_TYPE_AP;
            } else {
                LOGI("invalid port");
            }

            if (operation == 0) {
                atci_cmd_show_ip(type);
            } else {
                p = strtok(NULL, ",");
                strcpy(ip, p);
                p = strtok(NULL, ",");
                strcpy(netmask, p);
                p = strtok(NULL, ",");
                strcpy(gw, p);
                atci_cmd_set_ip(type, ip, netmask, gw);
            }

            strcpy((char *)resonse.response_buf, "\r\nOK\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;

        default :
            strcpy((char *)resonse.response_buf, "ERROR\r\n");
            resonse.response_len = strlen((char *)resonse.response_buf);
            atci_send_response(&resonse);
            break;
    }
    return ATCI_STATUS_OK;
}

static void atci_cmd_show_ip(netif_type_t type)
{
    atci_response_t resonse = {{0}};
    char ip[16] = {0};
    char netmask[16] = {0};
    char gw[16] = {0};
    resonse.response_flag = 0; // Command Execute Finish.
    resonse.response_len  = 0;

    LOGI("show_ip:port=%d", type);
    struct netif *iface = netif_find_by_type(type);
    if (!iface) {
        sprintf((char *)resonse.response_buf, "Can't find interface by type. %d", type);
        resonse.response_len = strlen((char *)resonse.response_buf);
        atci_send_response(&resonse);
        return;
    }

#if LWIP_DHCP
    if (dhcp_supplied_address(iface)) {
        struct dhcp *d = iface->dhcp;
        strcpy(ip, ip4addr_ntoa(&d->offered_ip_addr));
        strcpy(netmask, ip4addr_ntoa(&d->offered_sn_mask));
        strcpy(gw, ip4addr_ntoa(&d->offered_gw_addr));
        sprintf((char *)resonse.response_buf, "mode:dhcp\r\nip:%s, netmask:%s, gateway:%s\n\r", ip, netmask, gw);
        resonse.response_len = strlen((char *)resonse.response_buf);
    } else
#endif
    {
        strcpy(ip, ip4addr_ntoa(&iface->ip_addr));
        strcpy(netmask, ip4addr_ntoa(&iface->netmask));
        strcpy(gw, ip4addr_ntoa(&iface->gw));
        sprintf((char *)resonse.response_buf, "mode:static\r\nip:%s, netmask:%s, gateway:%s\n\r", ip, netmask, gw);
        resonse.response_len = strlen((char *)resonse.response_buf);
    }
    atci_send_response(&resonse);
}

static void atci_cmd_set_ip(netif_type_t type, char *ipaddr, char *netmask, char *gw)
{
    atci_response_t resonse = {{0}};
    resonse.response_flag = 0; // Command Execute Finish.
    resonse.response_len  = 0;
    ip4_addr_t   ip;
    ip4_addr_t   nm;
    ip4_addr_t   gateway;

    LOGI("set_ip: port=%d", type);
    struct netif *iface = netif_find_by_type(type);
    if (!iface) {
        sprintf((char *)resonse.response_buf, "Can't find interface by type. %d", type);
        resonse.response_len = strlen((char *)resonse.response_buf);
        atci_send_response(&resonse);
        return;
    }
    LOGI("set_ip:ipaddr=%s", ipaddr);
    LOGI("set_ip:netmask=%s", netmask);
    LOGI("set_ip:gw=%s", gw);
    if ((ip4addr_aton(ipaddr, &ip) == 0) ||
            (ip4addr_aton(netmask, &nm) == 0) ||
            (ip4addr_aton(gw, &gateway) == 0)) {
        sprintf((char *)resonse.response_buf, "input error, can't set address\r\n");
        resonse.response_len = strlen((char *)resonse.response_buf);
        atci_send_response(&resonse);
        return;
    }
    netif_set_addr(iface, &ip, &nm, &gateway);
    sprintf((char *)resonse.response_buf, "Set address success\r\n");
    resonse.response_len = strlen((char *)resonse.response_buf);
    atci_send_response(&resonse);
}

