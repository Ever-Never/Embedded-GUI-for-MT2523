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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "os.h"
#include "os_util.h"
#include "type_def.h"
#include "wifi_cli.h"
#include "wifi_lwip_helper.h"
#include "cli.h"
#include "wifi_api.h"
#include "wifi_private_api.h"
#include "wifi_scan.h"
#include "get_profile_string.h"
#include "misc.h"

#if defined(MTK_SMTCN_V5_ENABLE) || defined(MTK_SMTCN_V4_ENABLE)
#include "smt_conn.h"
#endif
#include "inband_queue.h"

#define WIFI_CLI_RETURN_STRING(ret) ((ret>=0)?"Success":"Error")

cmd_t   wifi_config_set_pub_cli[] = {
    { "opmode",         "STA/AP/Dual",      wifi_config_set_opmode_ex },
    { "ssid",           "SSID",             wifi_config_set_ssid_ex },
    { "bssid",          "BSSID",            wifi_config_set_bssid_ex },
    { "sec",        "WPA/WPA2PSK Authmode,Encrypt Type",   wifi_config_set_security_mode_ex },
    { "psk",         "WPA/WPA2PSK Key",  wifi_config_set_psk_ex },
#ifndef MTK_CLI_SLIM_FOR_32
    { "pmk",            "PMK for WPA/WPA2PSK",  wifi_config_set_pmk_ex },
#endif
    { "wep",            "WEP key",          wifi_config_set_wep_key_ex },
#ifndef MTK_CLI_SLIM_FOR_32
    { "rxfilter",       "RX filter",        wifi_config_set_rx_filter_ex },
    { "mtksmart",       "MTK smart connection", wifi_config_set_mtk_smartconnection_filter_ex },
    { "rxraw",          "set RX RAW packet",    wifi_config_set_rx_raw_pkt_ex },
#endif
    { "radio",          "RADIO on/off",     wifi_config_set_radio_on_ex },
    { "ch",             "channel",          wifi_config_set_channel_ex },
    { "bw",             "bandwidth",        wifi_config_set_bandwidth_ex },
    { "bw_extended",    "bandwidth",        wifi_config_set_bandwidth_extended_ex },
    { "wirelessmode",   "wireless mode",    wifi_config_set_wireless_mode_ex },
#ifndef MTK_CLI_SLIM_FOR_32
    { "country",        "country region",   wifi_config_set_country_region_ex },
    { "country_code",   "country code",     wifi_config_set_country_code_ex },
#endif
    { "dtim",           "DTIM interval",    wifi_config_set_dtim_interval_ex },
    { "listen",         "listen interval",  wifi_config_set_listen_interval_ex },
    { "reload",         "reload the configuration", wifi_config_set_reload_ex },
    { "txraw",          "send TX RAW packet",   wifi_tx_raw_pkt_ex },
    { NULL }
};


cmd_t   wifi_config_get_pub_cli[] = {
#ifndef MTK_CLI_SLIM_FOR_32
    { "opmode",         "STA/AP/Dual",      wifi_config_get_opmode_ex },
#endif
    { "mac",            "MAC address",      wifi_config_get_mac_address_ex },
#ifndef MTK_CLI_SLIM_FOR_32
    { "ssid",           "SSID",             wifi_config_get_ssid_ex },
    { "bssid",          "BSSID",            wifi_config_get_bssid_ex},
    { "sec",        "WPA/WPA2PSK Authmode,Encrypt Type",   wifi_config_get_security_mode_ex },
    { "psk",         "WPA/WPA2PSK Key",  wifi_config_get_psk_ex },
    { "pmk",            "PMK for WPA/WPA2PSK",  wifi_config_get_pmk_ex },
    { "wep",            "WEP key",          wifi_config_get_wep_key_ex },
    { "rxfilter",       "RX filter",        wifi_config_get_rx_filter_ex },
    { "mtksmart",       "MTK smart connection", wifi_config_get_mtk_smartconnection_filter_ex },
    { "radio",          "RADIO on/off",     wifi_config_get_radio_on_ex },
    { "ch",             "channel",          wifi_config_get_channel_ex },
    { "bw",             "bandwidth",        wifi_config_get_bandwidth_ex },
    { "bw_extended",    "bandwidth",        wifi_config_get_bandwidth_extended_ex },
    { "wirelessmode",   "wireless mode",    wifi_config_get_wireless_mode_ex },
    { "country",        "country region",   wifi_config_get_country_region_ex },
    { "country_code",   "country code",     wifi_config_get_country_code_ex },
    { "dtim",           "DTIM interval",    wifi_config_get_dtim_interval_ex },
    { "listen",         "listen interval",  wifi_config_get_listen_interval_ex },
#endif
    { NULL }
};

cmd_t   wifi_connect_set_pub_cli[] = {
    { "scan",       "start/stop scan",                      wifi_connect_set_scan_ex}, /*need implement base on WiFi Scan API*/
    { "connection", "connect/disconnect to AP",             wifi_connect_set_connection_ex},
    { "deauth",     "de-authenticate specific STA",         wifi_connect_deauth_station_ex},
    { "eventcb",    "register/un-register event callback",  wifi_connect_set_event_callback_ex},
    { NULL }
};

cmd_t   wifi_connect_get_pub_cli[] = {
    { "stalist",     "get STA list",                        wifi_connect_get_station_list_ex},
    { "linkstatus",  "get link status",                     wifi_connect_get_link_status_ex},
    { "max_num_sta", "get max number of supported STA",     wifi_connect_get_max_station_number_ex},
    { "rssi",        "get rssi",                            wifi_connect_get_rssi_ex},
    { NULL }
};

#if defined(MTK_WIFI_WPS_ENABLE)
cmd_t   wifi_wps_set_pub_cli[] = {
    { "device_info",     "set wps device information",            wifi_wps_set_device_info_ex},
    { "auto_connection", "set Enrollee auto connection on/off",   wifi_wps_set_auto_connection_ex},
    { NULL }
};

cmd_t   wifi_wps_get_pub_cli[] = {
    { "device_info",     "get wps device information",            wifi_wps_get_device_info_ex},
    { "auto_connection", "get Enrollee auto connection on/off",   wifi_wps_get_auto_connection_ex},
    { "pin_code",        "get 8-digital PIN of AP/STA",           wifi_wps_get_pin_code_ex},
    { NULL }
};

cmd_t   wifi_wps_trigger_pub_cli[] = {
    { "pbc",     "trigger wps pbc",       wifi_wps_connection_by_pbc_ex},
    { "pin",     "trigger wps pin",       wifi_wps_connection_by_pin_ex},
    { NULL }
};
#endif /*MTK_WIFI_WPS_ENABLE*/

static cmd_t   wifi_config_pub_cli[] = {
    { "set",   "wifi config set",    NULL,   wifi_config_set_pub_cli },
    { "get",   "wifi config get",    NULL,   wifi_config_get_pub_cli },
    { NULL,    NULL,                 NULL,   NULL                    }
};

static cmd_t   wifi_connect_pub_cli[] = {
    { "set",   "wifi connect set",   NULL,   wifi_connect_set_pub_cli },
    { "get",   "wifi connect get",   NULL,   wifi_connect_get_pub_cli },
    { NULL,    NULL,                 NULL,   NULL                     }
};

#if defined(MTK_WIFI_WPS_ENABLE)
static cmd_t   wifi_wps_pub_cli[] = {
    { "set",       "wifi wps set",      NULL,   wifi_wps_set_pub_cli     },
    { "get",       "wifi wps get",      NULL,   wifi_wps_get_pub_cli     },
    { "trigger",   "wifi wps trigger",	NULL,	wifi_wps_trigger_pub_cli },
    { NULL,        NULL,                NULL,   NULL                     }
};
#endif /*MTK_WIFI_WPS_ENABLE*/

#ifdef MTK_MINICLI_ENABLE
#ifdef MTK_WIFI_PROFILE_ENABLE
cmd_t   wifi_profile_cli[] = {
    { "set",   "wifi profile set",   NULL,   wifi_profile_set_cli },
    { "get",   "wifi profile get",   NULL,   wifi_profile_get_cli },
    { "reset", "wifi profile reset", wifi_profile_reset_ex,   NULL },
    { NULL,    NULL,                 NULL,   NULL                 }
};
#endif //MTK_WIFI_PROFILE_ENABLE
#endif

cmd_t   wifi_pub_cli[] = {
    { "config",     "wifi config",  NULL,    wifi_config_pub_cli      },
    { "connect",    "wifi connect", NULL,    wifi_connect_pub_cli     },
#if defined(MTK_WIFI_WPS_ENABLE)
    { "wps",        "wifi wps",     NULL,    wifi_wps_pub_cli		  },
#endif /*MTK_WIFI_WPS_ENABLE*/

#ifdef MTK_WIFI_PROFILE_ENABLE
    { "profile",    "wifi profile", NULL,       wifi_profile_cli    },
#endif //MTK_WIFI_PROFILE_ENABLE

#ifdef MTK_WIFI_MIB_ENABLE
    { "mib",        "show mib",     wifi_mib_show, NULL               },
#endif //MTK_WIFI_MIB_ENABLE

    { NULL,         NULL,           NULL,    NULL                     }
};






extern wifi_scan_list_item_t *g_scan_list;
extern uint8_t g_scan_list_size;



#ifndef MTK_CLI_SLIM_FOR_32
/**
* @brief
*
* @param
*
* @return  >=0 means success, <0 means fail
*/
int wlan_raw_pkt_rx_filter_sample(uint8_t *payload, unsigned int len)
{
    wifi_data_parse_t data_info = {0};
    if(wifi_connection_parse_data_descriptor(payload, &data_info) < 0) {
        return 0;/* Not handled */
    }

    printf("payload length=%d, rssi:%d, data rate: %ld.%ld Mbps\n", data_info.packet_payload_len, data_info.rssi, data_info.data_rate/10, data_info.data_rate%10);
    return 1;/* handled */
}

#endif

/**
* @brief port sanity check
* @parameter [IN] port
*
* @return  >=0 means port, <0 means fail
*/
int32_t port_sanity_check(char *port_str)
{
    int32_t port = (int32_t)atoi(port_str);

    if (port > WIFI_PORT_AP) {
        printf("Invalid port argument: %ld\n", port);
        return -1;
    }
    return port;
}

/**
* @brief Example of Get MAC address for STA/AP wireless port
*
*  wifi config get mac <port> --> get port0 (STA) MAC address
* @parameter
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_get_mac_address_ex(uint8_t len, char *param[])
{
    int i;
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t addr[WIFI_MAC_ADDRESS_LENGTH] = {0};
    unsigned char addr2[WIFI_MAC_ADDRESS_LENGTH] = {0};
    int32_t port = 0;

    if( len < 1 ){
        printf("Usage: wifi config get mac <port> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    ret = wifi_config_get_mac_address(port, addr);
    if (ret < 0) {
        status = 1;
    }

    for (i = 0; i < WIFI_MAC_ADDRESS_LENGTH; i++) {
        addr2[i] = addr[i];
    }

    printf("wifi_config_get_mac_address(port%ld): (%02x:%02x:%02x:%02x:%02x:%02x), ret:%s, Code=%ld\n",
           port,
           addr2[0], addr2[1], addr2[2],
           addr2[3], addr2[4], addr2[5],
           WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}




/**
* @brief Example of Configure bandwidth for STA/AP  wireless port.
*
* wifi config set bw <port>  <0:HT20/1:HT40>
* @parameter
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_bandwidth_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    int32_t port = 0;
    char bw = 0;

    if( len < 2 ){
        printf("Usage: wifi config set bw <port>  <0:HT20/1:HT40> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    bw = atoi(param[1]);
    if (port < 0) {
        return 1;
    }

    ret = wifi_config_set_bandwidth(port, bw);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_bandwidth(port%ld): %d, ret:%s, Code=%ld\n",
           port,  bw,  WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

/**
* @brief Example of Configure bandwidth for STA/AP  wireless port.
*
* wifi config set bw_extended <port>  <0:HT20/1:HT40> <below_above_ch>
* @parameter
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_bandwidth_extended_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    int32_t port = 0;
    char bw = 0;
    char below_above_ch = 0;

    if( len < 3 ){
        printf("Usage: wifi config set bw_extended <port>  <0:HT20/1:HT40> <below_above_ch> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    bw = atoi(param[1]);
    below_above_ch = atoi(param[2]);
    if (port < 0) {
        return 1;
    }

    ret = wifi_config_set_bandwidth_extended(port, bw, below_above_ch);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_bandwidth_extended(port%ld): %d, %d, ret:%s, Code=%ld\n",
           port,  bw, below_above_ch,  WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}


#ifndef MTK_CLI_SLIM_FOR_32
/**
* @brief Example of get bandwidth for STA/AP wireless port.
*  wifi config get bw <port>
* @parameter
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_bandwidth_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t bw = 0;
    int32_t port = 0;

    if( len < 1 ){
        printf("Usage: wifi config get bw <port> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    ret = wifi_config_get_bandwidth(port, &bw);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_bandwidth(port%ld): %d, ret:%s, Code=%ld\n",
           port, bw, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

#endif
/**
* @brief Example of Configure channel for STA/AP wireless port. STA will keep idle and stay in channel specified
* wifi config set ch <port> <ch>
* @parameter
*     [IN] channel  1~14 are supported for 2.4G only product
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_channel_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    int32_t port = 0;
    char ch = 0;

    if( len < 2 ){
        printf("Usage: wifi config set ch <port> <ch> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    ch = atoi(param[1]);

    if (port < 0) {
        return 1;
    }

    if (ch < 1) {
        printf("Invalid channel number\n");
        return 1;
    }

    ret = wifi_config_set_channel(port, ch);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_channel(port%ld): %d, ret:%s, Code=%ld\n",
           port,  ch,  WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

#ifndef MTK_CLI_SLIM_FOR_32
/**
* @brief Example of get bandwidth for STA/AP wireless port.
*  wifi config get bw_extended <port>
* @parameter
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_bandwidth_extended_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t bw = 0;
    uint8_t below_above_ch = 0;
    int32_t port = 0;

    if( len < 1 ){
        printf("Usage: wifi config get bw_extended <port> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    ret = wifi_config_get_bandwidth_extended(port, &bw, &below_above_ch);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_bandwidth_extended(port%ld): %d, %d, ret:%s, Code=%ld\n",
           port,  bw, below_above_ch, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

#endif
/**
* @brief Example of get the current channel for STA/AP wireless port.
* wifi config get channel <port>
* @parameter
*     [OUT] channel I1~14 are supported for 2.4G only product
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_channel_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    int32_t port = 0;
    uint8_t ch = 0;

    if( len < 1 ){
        printf("Usage: wifi config get channel <port> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    ret = wifi_config_get_channel(port, &ch);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_channel(port%ld): %d, ret:%s, Code=%ld\n",
           port, ch, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}


#ifndef MTK_CLI_SLIM_FOR_32
/**
* @brief Example of configure rx filter for packets wanted to be received
* wifi config set rxfilter <flag>
* @parameter
*    [IN]flag defined in  wifi_rx_filter_t
* @return =0 means success, >0 means fail
* @note Default value will be WIFI_DEFAULT_IOT_RX_FILTER
*/
uint8_t wifi_config_set_rx_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint32_t flag = 0;

    if( len < 1 ){
        printf("Usage: wifi config set rxfilter <flag> \n");
        return 1;
    }

    flag = strtol(param[0], NULL, 0);
    ret = wifi_config_set_rx_filter(flag);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_rxfilter: 0x%x, ret:%s, Code=%ld\n",
           (unsigned int) flag, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

/**
* @brief Example of get rx filter for packets format wanted to be received
* wifi config get rxfilter
* @parameter
*    [OUT]flag defined in  wifi_rx_filter_t
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_rx_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint32_t flag = 0;

    ret = wifi_config_get_rx_filter(&flag);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_rxfilter: 0x%x, ret:%s, Code=%ld\n",
           (unsigned int) flag, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}


/**
* @brief Example of Set WiFi Raw Packet Receiver
* wifi config set rxraw <enable>
* @param [IN]enable 0: unregister, 1: register
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_set_rx_raw_pkt_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t enable = 0;

    if( len < 1 ){
        printf("Usage: wifi config set rxraw <enable> \n");
        return 1;
    }

    enable = atoi(param[0]);
    if (enable == 0) {
        ret = wifi_config_unregister_rx_handler();
    } else {
        ret = wifi_config_register_rx_handler((wifi_rx_handler_t) wlan_raw_pkt_rx_filter_sample);
    }
    if (ret < 0) {
        status = 1;
    }

    printf("wifi_config_set_rx_raw_pkt_ex, enable = %d, ret:%s, Code=%ld\n",
           enable, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}
#endif

/**
* @brief send txraw packets
* wifi config set txraw <raw_packets>
* @param  [IN] packets The 802.11 packets content that includes 802.11 MAC header + Payload, it does not include the FCS.
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_tx_raw_pkt_ex(uint8_t len, char *param[])
{
    int8_t status = 0;
    int32_t ret = 0;
    uint8_t *hex_buf = NULL;
    uint32_t len_hex_buf = 0;
    uint32_t len_buf = 0;
    int i_hex = 0;
    //txd_param_t txd;

    if( len < 1 ){
        printf("Usage: wifi config set txraw <raw_packets> \n");
        return 1;
    }

    len_hex_buf = os_strlen(param[0]) / 2 + os_strlen(param[0]) % 2;
    len_buf = os_strlen(param[0]);

    if ((hex_buf = (uint8_t *)os_malloc(len_hex_buf)) == NULL) {
        printf("alloc buffer failed.\n");
        return 1;
    }
    os_memset(hex_buf, 0, len_hex_buf);

    for (int i = 0; i < len_buf; i++) {
        char cc = param[0][i];
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

    if ((ret = wifi_connection_send_raw_packet(hex_buf, len_hex_buf)) < 0) {
        status = 1;
    }

    printf("tx raw packet, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    os_free(hex_buf);
    return status;
}

/**
* @brief Example of Set the authentication mode and encryption mode
   for the specified STA/AP port
* wifi config set sec <port> <auth> <encrypt>
* @param [IN]port
* @param [IN]authmode
*           1 WPAPSK
*           2 WPA2PSK
*           3 WPA1PSKWPA2PSK
* @param  [IN] encryption method index:
*              1 AES
*              2 TKIP
*              3 TKIPAES
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_security_mode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t auth = 0;
    uint8_t encrypt = 0;
    int32_t port = 0;

    if( len < 3 ){
        printf("Usage: wifi config set sec <port> <auth> <encrypt> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }
    auth = atoi(param[1]);
    encrypt = atoi(param[2]);
    ret = wifi_config_set_security_mode(port, (wifi_auth_mode_t)auth, (wifi_encrypt_type_t)encrypt);

    if (ret < 0) {
        status = 1;
    }

    printf("wifi_config_set_security_mode_ex: port:%ld, auth mode:%d, encrypt type:%d, ret:%s, Code=%ld\n",
           port, auth, encrypt, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}


/**
* @brief Example of set the psk for the specified STA/AP port
* wifi config set psk <port> <password>
@param [IN]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_psk_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    int32_t port = 0;

    if( len < 2 ){
        printf("Usage: wifi config set psk <port> <password> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }
    ret = wifi_config_set_wpa_psk_key(port, (uint8_t *)param[1], os_strlen(param[1]));
    if (ret < 0) {
        status = 1;
    }

    printf("wifi_config_set_psk_ex: port:%ld, psk(%s), ret:%s, Code=%ld\n",
           port, param[1], WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

#if 0 //Add new cli to set authentication mode and psk separately.
/**
* @brief Example of Set the authentication mode for the specified STA/AP port
* wifi config set psk <port> <auth> <encrypt> <password>
* @param [IN]authmode
*           1 WPAPSK
*           2 WPA2PSK
*           3 WPA1PSKWPA2PSK
* @param  [IN] encryption method index:
*              1 AES
*              2 TKIP
*              3 TKIPAES
* @param [IN]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
* @return =0 means success, >0 means fail
* @note Default to OPEN
*/
uint8_t wifi_config_set_psk_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t length;
    uint8_t auth = 0;
    uint8_t encrypt = 0;
    int32_t port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    if (len == 2) {
        length = os_strlen(param[1]);
        ret = wifi_config_set_wpa_psk_key(port, (uint8_t *)param[1], length);
        if (ret < 0) {
            status = 1;
        }
    } else if (len == 4) {
        auth = atoi(param[1]);
        encrypt = atoi(param[2]);
        length = os_strlen(param[3]);
        ret = wifi_config_set_security_mode(port, (wifi_auth_mode_t)auth, (wifi_encrypt_type_t)encrypt);
        if (ret < 0) {
            status = 1;
        } else {
            ret = wifi_config_set_wpa_psk_key(port, (uint8_t *)param[3], length);
            if (ret < 0) {
                status = 1;
            }
        }
    } else {
        printf("Invalid command format.\n");
        return 1;
    }

    if (len == 2) {
        printf("wifi_config_set_psk_ex: port:%ld, psk(%s), ret:%s, Code=%ld\n",
               port, param[1], WIFI_CLI_RETURN_STRING(ret), ret);
    } else {
        printf("wifi_config_set_psk_ex: port:%ld, psk(%d, %d, %s), ret:%s, Code=%ld\n",
               port, auth, encrypt, param[3], WIFI_CLI_RETURN_STRING(ret), ret);
    }

    return status;
}
#endif


#ifndef MTK_CLI_SLIM_FOR_32
/**
* @brief Example of Set PMK for the specified STA/AP port
* wifi config set pmk <port> <PMK>
* @param [IN]port
*       0 STA / AP Client
*       1 AP
* @param  [IN] PMK (in hex)
*       00, 05, 30, ......(size 32)
* @return =0 means success, >0 means fail
* @note Default to OPEN
*/
uint8_t wifi_config_set_pmk_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t hex[32];
    int32_t port = 0;

    if (len < 2) {
        printf("Usage: wifi config set pmk <port> <PMK>\n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    os_memset(hex, 0, sizeof(hex));
    convert_string_to_hex_array(param[1], hex);
    ret = wifi_config_set_pmk(port, hex);
    if (ret < 0) {
        status = 1;
    }

    printf("wifi_config_set_pmk_ex: port:%ld, ret:%s, Code=%ld\n",
           port, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;

}

/**
* @brief Example of get the authentication mode for the specified STA/AP port
* wifi config get sec <port>
* @param [OUT]authmode method index:
*           1 WPAPSK
*           2 WPA2PSK
*           3 WPA1PSKWPA2PSK
* @param [OUT] encryption method index:
*              1 AES
*              2 TKIP
*              3 TKIPAES
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_security_mode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t auth = 0;
    uint8_t encrypt = 0;
    int32_t port = 0;

    if (len < 1) {
        printf("Usage:wifi config get sec <port>\n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    ret = wifi_config_get_security_mode(port, (wifi_auth_mode_t *)&auth, (wifi_encrypt_type_t *)&encrypt);
    if (ret < 0) {
        status = 1;
    }

    printf("wifi_config_get_security_mode_ex: port:%ld, auth mode:%d, encrypt type:%d, ret:%s, Code=%ld\n",
           port, auth, encrypt, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}


/**
* @brief Example of get the psk for the specified STA/AP port
* wifi config get psk <port> <password>
@param [IN]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_psk_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t password[64] = {0};
    uint8_t length = 0;
    int32_t port = 0;

    if (len < 1) {
        printf("Usage:wifi config get psk <port>\n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    ret = wifi_config_get_wpa_psk_key(port, password, &length);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_psk_ex: port:%ld, psk(%s), ret:%s, Code=%ld\n",
           port, password, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}


#if 0 //Add new cli to get authentication mode and psk separately.
/**
* @brief Example of Set the authentication mode for the specified STA/AP port
* wifi config set psk <port>
* @param [OUT]authmode
*           1 WPAPSK
*           2 WPA2PSK
*           3 WPA1PSKWPA2PSK
* @param  [OUT] encryption method index:
*              1 AES
*              2 TKIP
*                  3 TKIPAES
* @param [OUT]passphrase 8 ~ 63 bytes ASCII or 64 bytes Hex
* @param [OUT]passphrase_len 8 ~ 64
* @return =0 means success, >0 means fail
* @note Default to OPEN
*/
uint8_t wifi_config_get_psk_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t auth = 0;
    uint8_t encrypt = 0;
    uint8_t length = 0;
    uint8_t password[64] = {0};
    int32_t port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    ret = wifi_config_get_security_mode(port, (wifi_auth_mode_t *)&auth, (wifi_encrypt_type_t *)&encrypt);
    if (ret < 0) {
        status = 1;
    } else {
        ret = wifi_config_get_wpa_psk_key(port, password, &length);
        if (ret < 0) {
            status = 1;
        }
    }

    printf("wifi_config_get_psk_ex: port:%ld, psk(%d, %d, %s), ret:%s, Code=%ld\n",
           port, auth, encrypt, password, WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}
#endif

/**
* @brief Example of Get PMK for the specified STA/AP port
* wifi config get pmk <port>
* @param [IN]port
*       0 STA / AP Client
*       1 AP
* @return =0 means success, >0 means fail
* @note Default to OPEN
*/
uint8_t wifi_config_get_pmk_ex(uint8_t len, char *param[])
{
    int i;
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t hex[32];
    int32_t port = 0;

    if( len < 1 ){
        printf("Usage: wifi config get pmk <port> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    os_memset(hex, 0, sizeof(hex));
    ret = wifi_config_get_pmk(port, hex);
    if (ret < 0) {
        status = 1;
    }

    printf("wifi_config_get_pmk_ex: port:%ld, ret:%s, Code=%ld, key dump:\n",
           port, WIFI_CLI_RETURN_STRING(ret), ret);

    for (i = 0; i < sizeof(hex); i++) {
        if (i % 16 == 0) {
            printf("\n\t");
        }
        printf("%02x ", (unsigned int)hex[i]);
    }
    printf("\n");

    return status;
}

/**
* @brief Example of get WiFi WEP Keys
* wifi config get wep <port>
* @param [OUT]wifi_wep_key_t
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_get_wep_key_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    wifi_wep_key_t *keys = NULL;
    int32_t port = 0;

    if( len < 1 ){
        printf("Usage: wifi config get wep <port> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    keys = (wifi_wep_key_t *)os_zalloc(sizeof(wifi_wep_key_t));
    if (keys) {
        os_memset(keys, 0, sizeof(wifi_wep_key_t));
    } else {
        printf("%s: keys alloc fail.\n", __FUNCTION__);
        return 1;
    }
    ret = wifi_config_get_wep_key(port, keys);
    if (ret < 0) {
        status = 1;
    }

    printf("wifi_config_get_wep_ex: port:%ld, ret:%s, Code=%ld\n",
           port, WIFI_CLI_RETURN_STRING(ret), ret);

    if (keys->wep_tx_key_index < 4) {
        int i;
        printf("keys[%d]=", (int)keys->wep_tx_key_index);
        for (i = 0; i < keys->wep_key_length[keys->wep_tx_key_index]; i++) {
            printf("%02x", keys->wep_key[keys->wep_tx_key_index][i]);
        }
        printf("\n");
    } else {
        printf("Invalid key id:[%d]\n", (int)keys->wep_tx_key_index);
        status = 1;
    }

    os_free(keys);
    return status;
}
#endif

/**
* @brief Example of Set WiFi WEP Keys
* wifi config set wep <port> <key_id> <key_string>
* @param [IN]wifi_wep_key_t
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_wep_key_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    wifi_wep_key_t wep_key = {{{0}}};
    int32_t port = 0;
    char *section = NULL;

    if( len < 3 ){
        printf("Usage:  wifi config set wep <port> <key_id> <key_string> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }
    section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    wep_key.wep_tx_key_index = atoi(param[1]);
    if (os_strlen(param[2]) == 10 || os_strlen(param[2]) == 26) {
        wep_key.wep_key_length[wep_key.wep_tx_key_index] = os_strlen(param[2]) / 2;
        AtoH((char *)param[2], (char *)&wep_key.wep_key[wep_key.wep_tx_key_index], (int)wep_key.wep_key_length[wep_key.wep_tx_key_index]);
    } else if (os_strlen(param[2]) == 5 || os_strlen(param[2]) == 13) {
        os_memcpy(wep_key.wep_key[wep_key.wep_tx_key_index], param[2], os_strlen(param[2]));
        wep_key.wep_key_length[wep_key.wep_tx_key_index] = os_strlen(param[2]);
    } else {
        printf("invalid length of value.\n");
        return 1;
    }


    ret = wifi_config_set_wep_key((uint8_t)port, &wep_key);
    if (ret < 0) {
        status = 1;
    }

    printf("[%s] save wep key =(%s, %s, %s, %s) key id=%d, len=(%d, %d, %d, %d) done, ret:%s, Code=%ld\n",
           section,
           wep_key.wep_key[0],
           wep_key.wep_key[1],
           wep_key.wep_key[2],
           wep_key.wep_key[3],
           wep_key.wep_tx_key_index,
           wep_key.wep_key_length[0],
           wep_key.wep_key_length[1],
           wep_key.wep_key_length[2],
           wep_key.wep_key_length[3],
           WIFI_CLI_RETURN_STRING(ret), ret);

    return status;
}

/**
* @brief Example of Get WiFi Operation Mode.
* wifi config get opmode
* @param [OUT]mode
* @      1 WIFI_MODE_STA_ONLY
* @      2 WIFI_MODE_AP_ONLY
* @      3 WIFI_MODE_REPEATER
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_get_opmode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t mode = 0;

    ret = wifi_config_get_opmode(&mode);
    if (ret < 0) {
        status = 1;
    }

    printf("wifi_config_get_opmode(%d), ret:%s, Code=%ld\n", (unsigned int)mode, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

/**
* @brief Example of Set WiFi Operation Mode.
* wifi config set opmode <mode>
* @param [IN]mode
* @     1 WIFI_MODE_STA_ONLY
* @     2 WIFI_MODE_AP_ONLY
* @     3 WIFI_MODE_REPEATER
*
* @return  =0 means success, >0 means fail
*
* @note Set WiFi Operation Mode will RESET all the configuration set by previous WIFI-CONFIG APIs
*/
uint8_t wifi_config_set_opmode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t target_mode = 0;

    if( len < 1 ){
        printf("Usage: wifi config set opmode <mode> \n");
        return 1;
    }

    target_mode = (uint8_t)atoi(param[0]);
    status =  wifi_set_opmode(target_mode);
    if (status == 0) {
        printf("wifi_config_set_opmode(%d) Success, status=%d\n", (unsigned int)target_mode, status);
    } else {
        printf("wifi_config_set_opmode(%d) Error, status=%d\n", (unsigned int)target_mode, status);
    }
    return status;
}


#ifndef MTK_CLI_SLIM_FOR_32

/**
* @brief Example of Get WiFi SSID.
* wifi config get ssid <port>
* @param [IN]port
*       0 STA
*       1 AP
* @param [OUT]ssid SSID
* @param [OUT]ssid_len Length of SSID
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_get_ssid_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t length = 0;
    uint8_t ssid[32] = {0};
    int32_t port = 0;

    if( len < 1 ){
        printf("Usage: wifi config get ssid <port> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    ret = wifi_config_get_ssid(port, ssid, &length);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_ssid(port:%ld), [%s], ret:%s, Code=%ld\n", port, ssid, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}
#endif
/**
* @brief Example of Set WiFi SSID.
* wifi config set ssid <port> <ssid>
* @param [IN]port
*       0 STA
*       1 AP
* @param [IN]ssid SSID
* @param [IN]ssid_len Length of SSID
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_set_ssid_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    int32_t port = 0;

    if( len < 2 ){
        printf("Usage: wifi config set ssid <port> <ssid> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    ret = wifi_config_set_ssid(port, (uint8_t *)param[1], os_strlen(param[1]));
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_ssid(port:%ld), [%s], ret:%s, Code=%ld\n", port, param[1], WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}


#ifndef MTK_CLI_SLIM_FOR_32
/**
* @brief Example of Get WiFi BSSID.
* wifi config get bssid
* @param [OUT]bssid BSSID
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_get_bssid_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t bssid[6] = {0};
    ret = wifi_config_get_bssid((uint8_t *)bssid);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_bssid(), %02x:%02x:%02x:%02x:%02x:%02x, ret:%s, Code=%ld\n",
        bssid[0],bssid[1],bssid[2],bssid[3],bssid[4],bssid[5],WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}
#endif

/**
* @brief Example of Set WiFi BSSID.
* wifi config set bssid <bssid>
* @param [IN]bssid BSSID
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_set_bssid_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t bssid[6] = {0};

    if( len < 1 ){
        printf("Usage: wifi config set bssid <bssid> \n");
        return 1;
    }

    wifi_conf_get_mac_from_str((char *)bssid, (char *)param[0]);
    ret = wifi_config_set_bssid((uint8_t *)bssid);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_bssid(), [%s], ret:%s, Code=%ld\n",param[0], WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

/**
* @brief Example of Reload configuration
* wifi config set reload
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_set_reload_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    ret = wifi_config_reload_setting();
    if (ret < 0) {
        status = 1;
    }
    printf("WiFi reload configuration, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}


#ifndef MTK_CLI_SLIM_FOR_32
/**
* @brief Example of Get MTK Smart Connection Filter
* wifi config get mtksmart
* @param [IN]flag
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_get_mtk_smartconnection_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t flag = 0;

    ret = wifi_config_get_smart_connection_filter(&flag);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_mtk_smartconnection_filter(flag=0x%x), ret:%s, Code=%ld\n",
           (unsigned int)flag, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

/**
* @brief Example of Set MTK Smart Connection Filter
* wifi config set mtksmart <flag>
* @param [IN]flag
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_set_mtk_smartconnection_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t flag = 0;

    if( len < 1 ){
        printf("Usage: wifi config set mtksmart <flag> \n");
        return 1;
    }

    flag = atoi(param[0]);
    ret = wifi_config_set_smart_connection_filter(flag);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_mtk_smartconnection_filter(flag=0x%x), ret:%s, Code=%ld\n",
           (unsigned int)flag, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}
#endif


/**
* @brief Example of WiFi Radio ON/OFF
* wifi config set radio <onoff>
* @param [IN]onoff
*       0 OFF
*       1 ON
* @return  =0 means success, >0 means fail
* @note in MODE_Dual, both WiFi interface radio will be turn on/off at the same time
*/
uint8_t wifi_config_set_radio_on_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t flag = 0;

    if( len < 1 ){
        printf("Usage: wifi config set radio <onoff> \n");
        return 1;
    }

    flag = atoi(param[0]);
    ret = wifi_config_set_radio(flag);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_radio(onoff=%d), ret:%s, Code=%ld\n", flag, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

#ifndef MTK_CLI_SLIM_FOR_32

/**
* @brief Example of get WiFi Radio ON/OFF
* wifi config get radio
* @param [OUT]onoff
*       0 OFF
*       1 ON
* @return  =0 means success, >0 means fail
* @note in MODE_Dual, both WiFi interface radio will be turn on/off at the same time
*/
uint8_t wifi_config_get_radio_on_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t flag = 0;

    ret = wifi_config_get_radio(&flag);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_radio(onoff=%d), ret:%s, Code=%ld\n", (int)flag, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

/**
* @brief Example of Get WiFi Wireless Mode
* wifi config get wirelessmode <port>
* @mode  @sa Refer to #wifi_phy_mode_t
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_get_wireless_mode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t mode = 0;
    int32_t port = 0;

    if( len < 1 ){
        printf("Usage: wifi config get wirelessmode <port> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    ret = wifi_config_get_wireless_mode(port, (wifi_phy_mode_t *)&mode);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_wireless_mode(mode=%d), ret:%s, Code=%ld\n", mode, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}
#endif


/**
* @brief Example of Set WiFi Wireless Mode
* wifi config set wirelessmode <port> <mode>
* @param [IN]mode, @sa Refer to #wifi_phy_mode_t
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_set_wireless_mode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t mode;
    int32_t port = 0;

    if( len < 2 ){
        printf("Usage: wifi config set wirelessmode <port> <mode> \n");
        return 1;
    }

    port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }
    mode = atoi(param[1]);

    ret = wifi_config_set_wireless_mode(port, (wifi_phy_mode_t)mode);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_wireless_mode(mode=%d), ret:%s, Code=%ld\n", mode, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

#ifndef MTK_CLI_SLIM_FOR_32

/**
* @brief Example of Get WiFi Country Region
* wifi config get country <band>
* @param [IN]band
* @param 0 2.4G
* @param 1 5G
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_get_country_region_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t band = 0;
    uint8_t region = 0;

    if( len < 1 ){
        printf("Usage: wifi config get country <band> \n");
        return 1;
    }

    band = atoi(param[0]);
    ret = wifi_config_get_country_region(band, &region);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_country_region(band:%d), region:%d, ret:%s, Code=%ld\n",
           band, region, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}


/**
* @brief Example of Set WiFi Country Region
* wifi config set country <band> <region>
* @param [IN]band
* @param 0 2.4G
* @param 1 5G
* @param [IN]region, @sa #wifi_config_set_country_region  
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_set_country_region_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t band = 0;
    uint8_t region = 0;

    if( len < 2 ){
        printf("Usage: wifi config set country <band> <region> \n");
        return 1;
    }

    band = atoi(param[0]);
    region = atoi(param[1]);
    ret = wifi_config_set_country_region(band, region);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_country_region(band:%d, region:%d), ret:%s, Code=%ld\n",
           band, region, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

/**
* @brief Example of Set Country Code
* wifi config set country_code <code>
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_set_country_code_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    wifi_country_code_t *country_code = NULL;

    if( len < 1 ){
        printf("Usage: wifi config set country_code <code> \n");
        return 1;
    }

    printf("Set country code: %s.\n", param[0]);
    if(strlen(param[0])> 2) {
        printf("Invalid country code length: %d.\n", strlen(param[0]));
        return 1;
    }

    if(os_memcmp(param[0],"CN", 2) == 0) {
        country_code = (wifi_country_code_t *)os_malloc(sizeof(wifi_country_code_t) + 2*sizeof(wifi_channel_table_t));
        os_memset(country_code, 0, sizeof(wifi_country_code_t) + 2*sizeof(wifi_channel_table_t));
        os_memcpy(country_code, param[0], 2);
        country_code->num_of_channel_table = 2;
        country_code->channel_table[0].first_channel = 1;
        country_code->channel_table[0].num_of_channel = 13;
        country_code->channel_table[0].dfs_req = 0;
        country_code->channel_table[0].max_tx_power = 20;
        country_code->channel_table[1].first_channel = 149;
        country_code->channel_table[1].num_of_channel = 4;
        country_code->channel_table[1].dfs_req = 0;
        country_code->channel_table[1].max_tx_power = 27;
    }else if(os_memcmp(param[0],"US", 2) == 0) {
        country_code = (wifi_country_code_t *)os_malloc(sizeof(wifi_country_code_t) + 5*sizeof(wifi_channel_table_t));
        os_memset(country_code, 0, sizeof(wifi_country_code_t) + 5*sizeof(wifi_channel_table_t));
        os_memcpy(country_code, param[0], 2);
        country_code->num_of_channel_table = 5;
        country_code->channel_table[0].first_channel = 1;
        country_code->channel_table[0].num_of_channel = 11;
        country_code->channel_table[0].dfs_req = 0;
        country_code->channel_table[0].max_tx_power = 30;
        country_code->channel_table[1].first_channel = 36;
        country_code->channel_table[1].num_of_channel = 4;
        country_code->channel_table[1].dfs_req = 0;
        country_code->channel_table[1].max_tx_power = 17;
        country_code->channel_table[2].first_channel = 52;
        country_code->channel_table[2].num_of_channel = 4;
        country_code->channel_table[2].dfs_req = 1;
        country_code->channel_table[2].max_tx_power = 24;
        country_code->channel_table[3].first_channel = 100;
        country_code->channel_table[3].num_of_channel = 11;
        country_code->channel_table[3].dfs_req = 1;
        country_code->channel_table[3].max_tx_power = 30;
        country_code->channel_table[4].first_channel = 149;
        country_code->channel_table[4].num_of_channel = 5;
        country_code->channel_table[4].dfs_req = 0;
        country_code->channel_table[4].max_tx_power = 30;
    }else if(os_memcmp(param[0],"UK", 2) == 0) {
        country_code = (wifi_country_code_t *)os_malloc(sizeof(wifi_country_code_t) + 4*sizeof(wifi_channel_table_t));
        os_memset(country_code, 0, sizeof(wifi_country_code_t) + 4*sizeof(wifi_channel_table_t));
        os_memcpy(country_code, param[0], 2);
        country_code->num_of_channel_table = 4;
        country_code->channel_table[0].first_channel = 1;
        country_code->channel_table[0].num_of_channel = 13;
        country_code->channel_table[0].dfs_req = 0;
        country_code->channel_table[0].max_tx_power = 20;
        country_code->channel_table[1].first_channel = 36;
        country_code->channel_table[1].num_of_channel = 4;
        country_code->channel_table[1].dfs_req = 0;
        country_code->channel_table[1].max_tx_power = 23;
        country_code->channel_table[2].first_channel = 52;
        country_code->channel_table[2].num_of_channel = 4;
        country_code->channel_table[2].dfs_req = 1;
        country_code->channel_table[2].max_tx_power = 23;
        country_code->channel_table[3].first_channel = 100;
        country_code->channel_table[3].num_of_channel = 11;
        country_code->channel_table[3].dfs_req = 1;
        country_code->channel_table[3].max_tx_power = 30;
    } else {
        printf("Only support country code: CN, US, UK for reference design.\n");
        return 1;
    }
    ret = wifi_config_set_country_code(country_code);
    os_free(country_code);
    if (ret < 0) {
        status = 1;
    }

    printf("wifi_config_set_country_code_ex, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

/**
* @brief Example of Get Country Code
* wifi config get country_code
*
* @return =0 means success, >0 means fail
*/
uint8_t wifi_config_get_country_code_ex(uint8_t len, char *param[])
{
    uint8_t i,status = 0;
    int32_t ret = 0;
    wifi_country_code_t *country_code = NULL;
    country_code = (wifi_country_code_t *)os_malloc(sizeof(wifi_country_code_t) + 8*sizeof(wifi_channel_table_t));
    ret = wifi_config_get_country_code(country_code);
    if (ret < 0) {
        status = 1;
    }
    printf("country code :%s\n", (char *)country_code->country_code);
    printf("number of channel table: %d\n", country_code->num_of_channel_table);
    printf("First CH\t Number of CH\t MAX TX power\n");
    for(i=0;i<country_code->num_of_channel_table;i++) {
        printf("%d\t\t %d\t\t %d\n", country_code->channel_table[i].first_channel,country_code->channel_table[i].num_of_channel, country_code->channel_table[i].max_tx_power);
    }
    printf("wifi_config_get_country_code_ex, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    os_free(country_code);
    return status;
}

/**
* @brief Example of Get WiFi DTIM Interval
* wifi config get dtim
* @interval: 1~255
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_get_dtim_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t dtim = 0;

    ret = wifi_config_get_dtim_interval(&dtim);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_dtim_interval dtim:%d, ret:%s, Code=%ld\n", dtim, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

#endif
/**
* @brief Example of Set WiFi DTIM Interval
* wifi config set dtim <dtim interval>
* @param [IN]interval 1 ~ 255
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_set_dtim_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t dtim = 0;

    if( len < 1 ){
        printf("Usage: wifi config set dtim <dtim interval> \n");
        return 1;
    }

    dtim = atoi(param[0]);
    ret = wifi_config_set_dtim_interval(dtim);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_dtim_interval dtim:%d, ret:%s, Code=%ld\n", dtim, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}


#ifndef MTK_CLI_SLIM_FOR_32
/**
* @brief Example of Get WiFi Listen Interval
* wifi config get listen
* @interval: 1~255
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_get_listen_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t listen = 0;

    ret = wifi_config_get_listen_interval(&listen);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_get_listen_interval listen:%d, ret:%s, Code=%ld\n", listen, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}
#endif

/**
* @brief Example of Set WiFi Listen Interval
* wifi config set listen <listen interval>
* @param [IN]interval 1 ~ 255
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_config_set_listen_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t listen = 0;

    if( len < 1 ){
        printf("Usage: wifi config set listen <listen interval> \n");
        return 1;
    }

    listen = atoi(param[0]);
    ret = wifi_config_set_listen_interval(listen);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_config_set_listen_interval listen:%d, ret:%s, Code=%ld\n", listen, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

#ifndef MTK_CLI_SLIM_FOR_32

/**
* @brief Example of Get WiFi Associated Station List
* wifi connect get stalist
* @param [OUT]station_list
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_connect_get_station_list_ex(uint8_t len, char *param[])
{
    int i;
    uint8_t status = 0;
    int32_t ret = 0;
    wifi_sta_list_t list[WIFI_MAX_NUMBER_OF_STA];
    uint8_t size = 0;
    ret = wifi_connection_get_sta_list(&size, list);
    if (ret < 0) {
        status = 1;
    }

    printf("stalist size=%d\n", size);
    for (i = 0; i < size; i++) {
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
    printf("wifi_connection_get_sta_list, ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

/**
* @brief Example of Get the max number of the supported stations in ap mode or Repeater Mode.
* wifi connect get max_num_sta
* @param [OUT] number The max number of supported stations will be returned
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_connect_get_max_station_number_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t number = 0;
    ret = wifi_connection_get_max_sta_number(&number);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_connect_get_max_station_number_ex: max sta number=%d, ret:%s, Code=%ld\n",
           number, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}


/**
* @brief Example of Get rssi of the connected AP
* wifi connect get rssi
* @parameter None
* @return =0 means success, >0 means fail
*/
uint8_t wifi_connect_get_rssi_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    int8_t rssi = 0;

    ret = wifi_connection_get_rssi(&rssi); // fixme
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_connect_get_rssi_ex(rssi=%d), ret:%s, Code=%ld\n", rssi, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}
#endif

/**
* @brief Example of deauth some WiFi connection
* wifi connect set deauth <MAC>
* @param [IN]addr STA MAC Address
*
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_connect_deauth_station_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    unsigned char addr[WIFI_MAC_ADDRESS_LENGTH] = {0};

    if( len < 1 ){
        printf("Usage: wifi connect set deauth <MAC> \n");
        return 1;
    }

    wifi_conf_get_mac_from_str((char *)addr, param[0]);
    ret = wifi_connection_disconnect_sta(addr);
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_connect_deauth_station(%02x:%02x:%02x:%02x:%02x:%02x), ret:%s, Code=%ld\n",
           addr[0], addr[1], addr[2], addr[3], addr[4], addr[5],
           WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}
#ifndef MTK_CLI_SLIM_FOR_32

/**
* @brief Example of set connect / link up to specifed AP
* wifi connect set connection <linkup/down>
* @parameter
*     [IN] link  0: link down, 1: link up
* @return =0 means success, >0 means fail
* @note Be sure to configure security setting before connect to AP
*/
uint8_t wifi_connect_set_connection_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t link = 0;

    if( len < 1 ){
        printf("Usage: wifi connect set connection <linkup/down> \n");
        return 1;
    }

    link = atoi(param[0]);
    if (link == 0) { // link down
        ret = wifi_connection_disconnect_ap();
    } else {
        ret = wifi_config_reload_setting();
    }
    if (ret < 0) {
        status = 1;
    }

    printf("wifi_connect_set_connection_ex(link=%d), ret:%s, Code=%ld\n", link, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}


/**
* @brief Example of Get the current STA port link up / link down status of the connection
* wifi connect get linkstatus
* @link status:
* WIFI_STATUS_LINK_DISCONNECTED(0)
* WIFI_STATUS_LINK_CONNECTED(1)
* @parameter None
* @return =0 means success, >0 means fail
* @note WIFI_STATUS_LINK_DISCONNECTED indicates STA may in IDLE/ SCAN/ CONNECTING state
*/
uint8_t wifi_connect_get_link_status_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t link = 0;

    ret = wifi_connection_get_link_status(&link); // fixme
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_connect_get_link_status(link=%d), ret:%s, Code=%ld\n", link, WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

int mtk_event_handler_sample(wifi_event_t event_id, unsigned char *payload, unsigned int len)
{
    int handled = 0;
    int i = 0;

    switch (event_id) {
        case WIFI_EVENT_IOT_CONNECTED:
            handled = 1;
            if ((len == WIFI_MAC_ADDRESS_LENGTH) && (payload)) {
                printf("[MTK Event Callback Sample]: LinkUp! CONNECTED MAC = %02x:%02x:%02x:%02x:%02x:%02x\n",
                       payload[0], payload[1], payload[2], payload[3], payload[4], payload[5]);
            } else {
                printf("[MTK Event Callback Sample]: LinkUp!\n");
            }
            break;
        case WIFI_EVENT_IOT_SCAN_COMPLETE:
            handled = 1;
            for (i = 0; i < g_scan_list_size; i++) {
                printf("\n%-4s%-33s%-20s%-8s%-8s%-8s%-8s%-8s%-8s%-8s\n", "Ch", "SSID", "BSSID", "Auth", "Cipher", "RSSI", "WPS_EN", "CM", "DPID", "SR");
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
                printf("\n");
            }
            printf("[MTK Event Callback Sample]: Scan Done!\n");
            break;
        case WIFI_EVENT_IOT_DISCONNECTED:
            handled = 1;
            if ((len == WIFI_MAC_ADDRESS_LENGTH) && (payload)) {
                printf("[MTK Event Callback Sample]: Disconnect! DISCONNECTED MAC = %02x:%02x:%02x:%02x:%02x:%02x\n",
                       payload[0], payload[1], payload[2], payload[3], payload[4], payload[5]);
            } else {
                printf("[MTK Event Callback Sample]: Disconnect!\n");
            }
            break;
        case WIFI_EVENT_IOT_PORT_SECURE:
            handled = 1;
            if ((len == WIFI_MAC_ADDRESS_LENGTH) && (payload)) {
                printf("[MTK Event Callback Sample]: Port Secure! CONNECTED MAC = %02x:%02x:%02x:%02x:%02x:%02x\n",
                       payload[0], payload[1], payload[2], payload[3], payload[4], payload[5]);
            } else {
                printf("[MTK Event Callback Sample]: Port Secure!\n");
            }
            break;
        case WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE:
            handled = 1;
            //hex_dump("Beacon/ProbeResponse", payload, len);
            if (len != 0) {
                wifi_scan_list_item_t ap_data;
                os_memset(&ap_data, 0, sizeof(wifi_scan_list_item_t));
                if (wifi_connection_parse_beacon(payload, len, &ap_data) >= 0) {
                    printf("\n%-4s%-33s%-20s%-8s%-8s%-8s%-8s%-8s%-8s%-8s\n", "Ch", "SSID", "BSSID", "Auth", "Cipher", "RSSI", "WPS_EN", "CM", "DPID", "SR");
                    printf("%-4d", ap_data.channel);
                    printf("%-33s", ap_data.ssid);
                    printf("%02x:%02x:%02x:%02x:%02x:%02x   ",
                           ap_data.bssid[0],
                           ap_data.bssid[1],
                           ap_data.bssid[2],
                           ap_data.bssid[3],
                           ap_data.bssid[4],
                           ap_data.bssid[5]);
                    printf("%-8d", ap_data.auth_mode);
                    printf("%-8d", ap_data.encrypt_type);
                    printf("%-8d", ap_data.rssi);
                    printf("%-8d", ap_data.is_wps_supported);
                    /*If received Beacon frame, the configuration_methods is 0, because the configuration_methods is not exist in Beacon, it can't be prased. it exist in Probe Response Frame.*/
                    /*If received Probe Response frame, the configuration_methods value is  meaningful.*/
                    printf("%-8d", ap_data.wps_element.configuration_methods);
                    printf("%-8d", ap_data.wps_element.device_password_id);
                    printf("%-8d", ap_data.wps_element.selected_registrar);
                    printf("\n");
                }

            }
            break;

#if defined(MTK_WIFI_WPS_ENABLE)
        case WIFI_EVENT_IOT_WPS_COMPLETE:
            handled = 1;
            if (len != 0) {
                printf("[MTK Event Callback Sample]:WPS Done! Credential length = %d\n", len);
                wifi_wps_credential_info_t *customer_cred = (wifi_wps_credential_info_t *)payload;
                //uint8_t passphrase[WIFI_LENGTH_PASSPHRASE] = {0};
                switch (customer_cred->encrypt_type) {
                    case WIFI_ENCRYPT_TYPE_TKIP_ENABLED:
                    case WIFI_ENCRYPT_TYPE_AES_ENABLED:
                        //os_memcpy(passphrase, customer_cred->key, sizeof(customer_cred->key));
                        printf("WPS Credential: ssid=%s, auth_mode=%d, encrypt_type=%d, passphrase=%s.\n",
                               customer_cred->ssid, customer_cred->auth_mode,
                               customer_cred->encrypt_type, customer_cred->key);

                        //Do Wi-Fi connection demo
                        wifi_config_set_ssid(WIFI_PORT_STA, customer_cred->ssid, customer_cred->ssid_len);
                        wifi_config_set_security_mode(WIFI_PORT_STA, customer_cred->auth_mode, customer_cred->encrypt_type);
                        wifi_config_set_wpa_psk_key(WIFI_PORT_STA, customer_cred->key, customer_cred->key_length);
                        wifi_config_reload_setting();
                        break;
                    default :
                        printf("WPS Encrypt type others=%d\n", customer_cred->encrypt_type);
                        break;
                }
            }
            break;
#endif
        case WIFI_EVENT_IOT_CONNECTION_FAILED:
            handled = 1;
            if ((len != 0) && (payload)) {
                printf("[MTK Event Callback Sample]: CONNECTION_FAILED! \n");
                printf("Port=%d, Reason code=%d \n", payload[0], (payload[2]<<8)+payload[1]);
            } else {
                printf("[MTK Event Callback Sample]: CONNECTION_FAILED!\n");
            }
            break;
        default:
            handled = 0;
            printf("[MTK Event Callback Sample]: Unknown event(%d)\n", event_id);
            break;
    }
    return handled;
}


/**
* @brief Example of Register WiFi Event Notifier
* wifi connect set eventcb <enable> <event ID>
* @param [IN]evt
* @param evt Event ID, More Event ID @sa #wifi_event_t
* @param [IN]enable 0: register, 1: unregister
* @return  =0 means success, >0 means fail
*/
uint8_t wifi_connect_set_event_callback_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t enable = 0;
    uint8_t event_id = 0;

    if( len < 2 ){
        printf("Usage: wifi connect set eventcb <enable> <event ID> \n");
        return 1;
    }

    enable = atoi(param[0]);
    event_id = atoi(param[1]);
    if (enable == 0) {
        ret = wifi_connection_unregister_event_handler((wifi_event_t)event_id, (wifi_event_handler_t) mtk_event_handler_sample);
    } else {
        ret = wifi_connection_register_event_handler((wifi_event_t)event_id, (wifi_event_handler_t) mtk_event_handler_sample);
    }
    if (ret < 0) {
        status = 1;
    }

    printf("wifi_connect_set_event_callback(), ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}

#endif
/**
* @brief start scan with options
* wifi connect set scan <start> <scan mode> <scan_option> [ssid] [bssid]
* wifi connect set scan <stop>
* @parameter [IN] start/stop  0: stop, 1: start
* @parameter [IN] scan mode  0: full, 1: partial
* @parameter [IN] scan_option  0: active, 1: passive in all channel, 2:force active
* @parameter [IN] ssid   "NULL" means not specified
* @parameter [IN] bssid   "NULL" means not specified
*
* @return =0 means success, >0 means fail
*/
wifi_scan_list_item_t g_ap_list[8] = {{0}};
uint8_t wifi_connect_set_scan_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t ret = 0;
    uint8_t start = 0;

    if( len < 1 ){
        printf("Usage: wifi connect set scan <start> <scan mode> <scan_option> [ssid] [bssid] \n"
               "wifi connect set scan <stop> \n");
        return 1;
    }

    start = atoi(param[0]);
    if (!start) {
        ret = wifi_connection_stop_scan();
    } else {
        if( len < 3 ){
            printf("Usage: wifi connect set scan <start> <scan mode> <scan_option> [ssid] [bssid] \n");
            return 1;
        }
        uint8_t scan_mode = atoi(param[1]);
        uint8_t scan_option = atoi(param[2]);

        char *ssid;
        uint8_t ssid_len = 0;
        uint8_t bssid_val[WIFI_MAX_NUMBER_OF_STA] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
        uint8_t *bssid = (uint8_t *)bssid_val;

        if (len >= 4) {
            ssid = param[3];
            ssid_len = os_strlen(ssid);
            if (len == 5) {
                wifi_conf_get_mac_from_str((char *)bssid_val, param[4]);

            } else {
                bssid = (uint8_t *)(NULL);
            }
        } else {
            ssid = NULL;
            ssid_len = 0;
            bssid = (uint8_t *)(NULL);
        }
        /******************reset scan list*******************************/
        wifi_connection_scan_init(g_ap_list, 8);
        printf("size = %d\n", sizeof(wifi_scan_list_item_t));
        ret = wifi_connection_start_scan((uint8_t *)ssid, ssid_len, (uint8_t *)bssid, scan_mode, scan_option);
    }
    if (ret < 0) {
        status = 1;
    }
    printf("wifi_connect_set_scan_ex(), ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
    return status;
}


/**
* @brief
* @parameter
* @return =0 means success, >0 means fail
*/
#if defined(MTK_SMTCN_V5_ENABLE) || defined(MTK_SMTCN_V4_ENABLE)
uint8_t smart_config_test_cli(uint8_t len, char *param[])
{
    if (len < 1) {
        printf("Usage: \n");
        printf("smart connect <bm/m/b> -> smart connect use both mc and bc, mc, bc \n");
        printf("smart stop             -> smart connect stop \n");
        printf("smart debug <0/1>      -> smart connect debug 0/1 \n");
        return 1;
    }

    if(!os_strcmp(param[0], "connect"))
    {
        uint8_t key[16];
        uint8_t key_len = 0;

        if(len > 1)
        {
            #if defined(MTK_SMTCN_V4_ENABLE) && defined(MTK_SMTCN_V5_ENABLE)
            if(os_strchr(param[1],'b') && strchr(param[1],'m'))
            {
                smt_ops_set_protocol(USE_BOTH);
                printf("start both mc and bc smart connection\n");
            }
            else if(os_strchr(param[1],'m'))
            {
                printf("start mc smart connection\n");
                smt_ops_set_protocol(USE_MC_PROTOCOL);
            }
            else if(os_strchr(param[1],'b'))
            {
                printf("start bc smart connection\n");
                smt_ops_set_protocol(USE_BC_PROTOCOL);
            }
            #endif

            if(os_strlen(param[len -1 ]) == 16)
            {
                os_memcpy(key,param[len -1 ],16);
                key_len = 16;
            }
        }
        mtk_smart_set_key(key,key_len);
        mtk_smart_connect();
    }
    else if(!os_strcmp(param[0], "stop"))
    {
        mtk_smart_stop();
    }
    else if (!os_strcmp(param[0], "debug"))
    {
        if(!os_strcmp(param[1], "1"))
        {
            smtcn_enable_debug(TRUE);
            printf("set debug on\n");
        } else if (!os_strcmp(param[1], "0"))
        {
            smtcn_enable_debug(FALSE);
            printf("set debug off\n");
        } else
        {
            printf("Not supported cmd\n");
        }
    }
    else
    {
        printf("Not supported cmd\n");
    }

    return 0;
}
#endif


#if defined(MTK_WIFI_WPS_ENABLE)
    /**
     * @brief set wifi wps device info
     * @parameter
     * @return =0 means success, >0 means fail
     */
    uint8_t wifi_wps_set_device_info_ex(uint8_t len, char *param[]) {
        uint8_t status = 0;
        int32_t ret = 0;
        wifi_wps_device_info_t wps_device_info = {{0}};

        if( len < 1 ){
            printf("Usage: wifi wps set device_info <Device_Name> <Manufacturer> <Model_Name> <Model_number> <Serial_number> \n");
            return 1;
        }

        if(NULL != param[0]) {
            os_memcpy(&(wps_device_info.device_name), param[0], strlen(param[0]));
        } else {
            printf("[Warning]:device_name input NULL.\n");
        }

        if(NULL != param[1]) {
            os_memcpy(&(wps_device_info.manufacturer), param[1], strlen(param[1]));
        } else {
            printf("[Warning]:manufacturer input NULL.\n");
        }

        if(NULL != param[2]) {
            os_memcpy(&(wps_device_info.model_name), param[2], strlen(param[2]));
        } else {
            printf("[Warning]:model_name input NULL.\n");
        }

        if(NULL != param[3]) {
            os_memcpy(&(wps_device_info.model_number), param[3], strlen(param[3]));
        } else {
            printf("[Warning]:model_number input NULL.\n");
        }

        if(NULL != param[4]) {
            os_memcpy(&(wps_device_info.serial_number), param[4], strlen(param[4]));
        } else {
            printf("[Warning]:serial_number input NULL.\n");
        }

        printf("wifi_wps_set_device_info_ex: wps_device_info input: %s, %s, %s, %s, %s.\n",
               wps_device_info.device_name,
               wps_device_info.manufacturer,
               wps_device_info.model_name,
               wps_device_info.model_number,
               wps_device_info.serial_number);

        ret = wifi_wps_config_set_device_info(&wps_device_info);
        if (ret < 0) {
            status = 1;
        }

        printf("wifi_wps_set_device_info_ex(), ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
        return status;
    }

    /**
    * @brief set wifi wps auto connection
    * @parameter
    * @return =0 means success, >0 means fail
    */
    uint8_t wifi_wps_set_auto_connection_ex(uint8_t len, char *param[]) {
        uint8_t status = 0;
        int32_t ret = 0;

        if( len < 1 ){
            printf("Usage: wifi wps set auto connection <onoff> \n");
            return 1;
        }

        if (0 == os_strcmp(param[0], "on")) {
            printf("enable auto connection\n");
            ret = wifi_wps_config_set_auto_connection(1);
            if (ret < 0) {
                status = 1;
            }
        } else if (0 == os_strcmp(param[0], "off")) {
            printf("disable auto connection\n");
            ret = wifi_wps_config_set_auto_connection(0);
            if (ret < 0) {
                status = 1;
            }
        } else {
            printf("Unknown cmd\n");
            return 1;
        }

        printf("wifi_wps_set_auto_connection_ex(), ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
        return status;
    }


    /**
    * @brief get wifi wps device info
    * @parameter
    * @return =0 means success, >0 means fail
    */
    uint8_t wifi_wps_get_device_info_ex(uint8_t len, char *param[]) {
        uint8_t status = 0;
        int32_t ret = 0;
        wifi_wps_device_info_t wps_device_info = {{0}};

        ret = wifi_wps_config_get_device_info(&wps_device_info);
        if (ret < 0) {
            status = 1;
        }

        printf("wifi_wps_get_device_info_ex:device_name=%s, manufacturer=%s, model_name=%s, model_number=%s, serial_number=%s.\n",
               wps_device_info.device_name,
               wps_device_info.manufacturer,
               wps_device_info.model_name,
               wps_device_info.model_number,
               wps_device_info.serial_number);

        printf("wifi_wps_get_device_info_ex(), ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
        return status;
    }


    /**
    * @brief get wifi wps auto connection
    * @parameter
    * @return =0 means success, >0 means fail
    */
    uint8_t wifi_wps_get_auto_connection_ex(uint8_t len, char *param[]) {
        uint8_t status = 0;
        int32_t ret = 0;
        bool auto_connection = 0;

        ret = wifi_wps_config_get_auto_connection(&auto_connection);
        if (ret < 0) {
            status = 1;
        }

        printf("wifi_wps_get_auto_connection_ex(onoff=%d), ret:%s, Code=%ld\n",
               auto_connection, WIFI_CLI_RETURN_STRING(ret), ret);

        return status;
    }


    /**
    * @brief get wifi wps pin code
    * @parameter
    * @return =0 means success, >0 means fail
    */
    uint8_t wifi_wps_get_pin_code_ex(uint8_t len, char *param[]) {
        uint8_t status = 0;
        int32_t ret = 0;
        char pin_code[9] = {0};
        int32_t port = 0;

        if( len < 1 ){
            printf("Usage:  wifi wps get pin <port> \n");
            return 1;
        }

        port = port_sanity_check(param[0]);
        ret = wifi_wps_config_get_pin_code(port, (uint8_t *)&pin_code);
        if (ret < 0) {
            status = 1;
        }

        printf("wifi_wps_get_pin_code_ex (PIN:%s), ret:%s, Code=%ld\n", pin_code, WIFI_CLI_RETURN_STRING(ret), ret);

        return status;
    }


    /**
    * @brief set wifi wps connection by PBC
    * @parameter
    * @return =0 means success, >0 means fail
    */
    uint8_t wifi_wps_connection_by_pbc_ex(uint8_t len, char *param[]) {
        uint8_t status = 0;
        int32_t ret = 0;
        uint8_t bssid[WIFI_MAC_ADDRESS_LENGTH] = {0};
        uint8_t *_bssid = bssid;
        int32_t port = 0;

        if( len < 2 ){
            printf("Usage:  wifi wps trigger pbc <port> <BSSID> \n");
            return 1;
        }

        port = port_sanity_check(param[0]);
        if (NULL == param[1] || 0 == os_strcmp(param[1], "any")) {
            _bssid = NULL;
        } else {
            //Mac address acceptable format 01:02:03:04:05:06 length 17
            if (17 != strlen(param[1])) {
                printf("wifi_wps_connection_by_pbc_ex fail, invalid BSSID len");
                return 1;
            }
            wifi_conf_get_mac_from_str((char *)_bssid, param[1]);
        }

        ret = wifi_wps_connection_by_pbc(port, _bssid);
        if (ret < 0) {
            status = 1;
        }

        printf("wifi_wps_connection_by_pbc_ex(), ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
        return status;
    }


    /**
    * @brief  set wifi wps connection by PIN
    * @parameter
    * @return =0 means success, >0 means fail
    */
    uint8_t wifi_wps_connection_by_pin_ex(uint8_t len, char *param[]) {
        uint8_t status = 0;
        int32_t ret = 0;
        uint8_t bssid[WIFI_MAC_ADDRESS_LENGTH] = {0};
        uint8_t *_bssid = bssid;
        char pin[9] = {0};
        int32_t port = 0;

        if( len < 3 ){
            printf("Usage:  wifi wps trigger pin <port> <BSSID> <PIN_code> \n");
            return 1;
        }

        port = port_sanity_check(param[0]);
        if (NULL == param[1] || 0 == os_strcmp(param[1], "any")) {
            _bssid = NULL;
        } else {
            //Mac address acceptable format 01:02:03:04:05:06 length 17
            if (17 != strlen(param[1])) {
                printf("wifi_wps_connection_by_pbc_ex fail, invalid BSSID");
                return 1;
            }
            wifi_conf_get_mac_from_str((char *)_bssid, param[1]);
        }

        os_memcpy(pin, param[2], strlen(param[2]));
        if (8 != (strlen(pin)) && 4 != (strlen(pin))) {
            printf("wifi_wps_connection_by_pin_ex fail: incorrect PIN");
            return 1;
        }

        ret = wifi_wps_connection_by_pin(port, _bssid, (uint8_t *)pin);
        if (ret < 0) {
            status = 1;
        }

        printf("wifi_wps_connection_by_pin_ex(), ret:%s, Code=%ld\n", WIFI_CLI_RETURN_STRING(ret), ret);
        return status;
    }

#endif /*MTK_WIFI_WPS_ENABLE*/


#ifdef MTK_WIFI_MIB_ENABLE
/**
 * @brief Show Wi-Fi MIB counters.
 * <br><b>
 * wifi mib
 * </b></br>
 * @return 0 means success, else means fail
 */
uint8_t wifi_mib_show(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t i;

#define BUS_SHIFT               (0x60800000UL)
#define WIFI_MIB_BASE           (0x60330000UL + BUS_SHIFT)

/***** MIB Counter *****/
#define ARB_MSDR0               (WIFI_MIB_BASE + 0x0010)
#define ARB_MSDR4               (WIFI_MIB_BASE + 0x0018)
#define ARB_MSDR5               (WIFI_MIB_BASE + 0x001C)
#define ARB_MSDR6               (WIFI_MIB_BASE + 0x0020)
#define ARB_MSDR7               (WIFI_MIB_BASE + 0x0024)
#define ARB_MSDR8               (WIFI_MIB_BASE + 0x0028)
#define ARB_MSDR9               (WIFI_MIB_BASE + 0x002C)
#define ARB_MSDR10              (WIFI_MIB_BASE + 0x0030)
#define ARB_MSDR11              (WIFI_MIB_BASE + 0x0034)
#define ARB_MSDR12              (WIFI_MIB_BASE + 0x0038)
#define ARB_MSDR13              (WIFI_MIB_BASE + 0x003C)
#define ARB_MSDR14              (WIFI_MIB_BASE + 0x0040)
#define ARB_MSDR15              (WIFI_MIB_BASE + 0x0044)
#define ARB_MSDR16              (WIFI_MIB_BASE + 0x0048)
#define ARB_MSDR17              (WIFI_MIB_BASE + 0x004C)
#define ARB_MSDR18              (WIFI_MIB_BASE + 0x0050)
#define ARB_MSDR19              (WIFI_MIB_BASE + 0x0054)
#define ARB_MSDR20              (WIFI_MIB_BASE + 0x0058)
#define ARB_MSDR21              (WIFI_MIB_BASE + 0x005C)
#define ARB_MSDR22              (WIFI_MIB_BASE + 0x0060)
#define ARB_MSDR23              (WIFI_MIB_BASE + 0x0064)
#define ARB_MSDR24              (WIFI_MIB_BASE + 0x0068)
#define ARB_MSDR25              (WIFI_MIB_BASE + 0x006C)
#define ARB_MSDR26              (WIFI_MIB_BASE + 0x0070)
#define ARB_MSDR27              (WIFI_MIB_BASE + 0x0074)
#define ARB_MSDR28              (WIFI_MIB_BASE + 0x0078)
#define ARB_MSDR29              (WIFI_MIB_BASE + 0x007C)
#define ARB_MSDR30              (WIFI_MIB_BASE + 0x0080)
#define ARB_MSDR31              (WIFI_MIB_BASE + 0x0084)

#define ARB_MB0SDR0             (WIFI_MIB_BASE + 0x0100)
#define ARB_MB0SDR1             (WIFI_MIB_BASE + 0x0104)
#define ARB_MB0SDR2             (WIFI_MIB_BASE + 0x0108)
#define ARB_MB0SDR3             (WIFI_MIB_BASE + 0x010C)
#define ARB_MB1SDR0             (WIFI_MIB_BASE + 0x0110)
#define ARB_MB1SDR1             (WIFI_MIB_BASE + 0x0114)
#define ARB_MB1SDR2             (WIFI_MIB_BASE + 0x0118)
#define ARB_MB1SDR3             (WIFI_MIB_BASE + 0x011C)
#define ARB_MB2SDR0             (WIFI_MIB_BASE + 0x0120)
#define ARB_MB2SDR1             (WIFI_MIB_BASE + 0x0124)
#define ARB_MB2SDR2             (WIFI_MIB_BASE + 0x0128)
#define ARB_MB2SDR3             (WIFI_MIB_BASE + 0x012C)
#define ARB_MB3SDR0             (WIFI_MIB_BASE + 0x0130)
#define ARB_MB3SDR1             (WIFI_MIB_BASE + 0x0134)
#define ARB_MB3SDR2             (WIFI_MIB_BASE + 0x0138)
#define ARB_MB3SDR3             (WIFI_MIB_BASE + 0x013C)

static const struct mib_counter_s {
    uint32_t    addr;
    uint8_t     bit_hi;
    uint8_t     bit_lo;
    char        *str;
} counters[] = {
    { ARB_MSDR0,    15,  0, "beacontxcount"                 },
    { ARB_MSDR4,    31, 16, "rx_fcs_error_count"            },
    { ARB_MSDR4,    15,  0, "rx_fifo_full_count"            },
    { ARB_MSDR5,    23, 16, "pf_drop_count"                 },
    { ARB_MSDR5,    15,  0, "rx_mpdu_count"                 },
    { ARB_MSDR6,    31, 16, "vec_drop_count"                },
    { ARB_MSDR6,    15,  0, "channel_idle_count"            },
    { ARB_MSDR7,    31, 16, "channel_idle_count"            },
    { ARB_MSDR8,    15,  0, "delimiter_fail_count"          },
    { ARB_MSDR9,    23,  0, "cca_nav_tx_time"               },
    { ARB_MSDR10,   25,  0, "rx_mdrdy_count"                },
    { ARB_MSDR11,   15,  0, "rx_len_mismatch"               },
    { ARB_MSDR16,   23,  0, "p_cca_time"                    },
    { ARB_MSDR17,   23,  0, "s_cca_time"                    },
    { ARB_MSDR18,   23,  0, "p_ed_time"                     },
    { ARB_MSDR19,   23,  0, "cck_mdrdy_time"                },
    { ARB_MSDR20,   23,  0, "ofdm_lg_mixed_vht_mdrdy_time"  },
    { ARB_MSDR21,   23,  0, "ofdm_green_mdrdy_time"         },
    { ARB_MSDR22,   16,  0, "rx_ampdu_count"                },
    { ARB_MSDR23,   31,  0, "rx_totbyte_coun"               },
    { ARB_MSDR24,   23,  0, "rx_validsf_count"              },
    { ARB_MSDR25,   31,  0, "rx_validbyte_count"            },
    { ARB_MSDR26,   31,  0, "phy_mib_count0"                },
    { ARB_MSDR27,   31,  0, "phy_mib_count1"                },

    { ARB_MB0SDR0,  31, 16, "bss0 rtsretrycount"            },
    { ARB_MB0SDR0,  15,  0, "bss0 rtstxcount"               },
    { ARB_MB0SDR1,  31, 16, "bss0 ackfailcount"             },
    { ARB_MB0SDR1,  15,  0, "bss0 bamisscount"              },
    { ARB_MB0SDR2,  31, 16, "bss0 frameretry2count"         },
    { ARB_MB0SDR2,  15,  0, "bss0 frameretrycount"          },
    { ARB_MB0SDR3,  15,  0, "bss0 frameretry3count"         },

    { ARB_MB1SDR0,  31, 16, "bss1 rtsretrycount"            },
    { ARB_MB1SDR0,  15,  0, "bss1 rtstxcount"               },
    { ARB_MB1SDR1,  31, 16, "bss1 ackfailcount"             },
    { ARB_MB1SDR1,  15,  0, "bss1 bamisscount"              },
    { ARB_MB1SDR2,  31, 16, "bss1 frameretry2count"         },
    { ARB_MB1SDR2,  15,  0, "bss1 frameretrycount"          },
    { ARB_MB1SDR3,  15,  0, "bss1 frameretry3count"         },

    { ARB_MB1SDR0,  31, 16, "bss1 rtsretrycount"            },
    { ARB_MB1SDR0,  15,  0, "bss1 rtstxcount"               },
    { ARB_MB1SDR1,  31, 16, "bss1 ackfailcount"             },
    { ARB_MB1SDR1,  15,  0, "bss1 bamisscount"              },
    { ARB_MB1SDR2,  31, 16, "bss1 frameretry2count"         },
    { ARB_MB1SDR2,  15,  0, "bss1 frameretrycount"          },
    { ARB_MB1SDR3,  15,  0, "bss1 frameretry3count"         },

    { ARB_MB2SDR0,  31, 16, "bss2 rtsretrycount"            },
    { ARB_MB2SDR0,  15,  0, "bss2 rtstxcount"               },
    { ARB_MB2SDR1,  31, 16, "bss2 ackfailcount"             },
    { ARB_MB2SDR1,  15,  0, "bss2 bamisscount"              },
    { ARB_MB2SDR2,  31, 16, "bss2 frameretry2count"         },
    { ARB_MB2SDR2,  15,  0, "bss2 frameretrycount"          },
    { ARB_MB2SDR3,  15,  0, "bss2 frameretry3count"         },

    { ARB_MB3SDR0,  31, 16, "bss3 rtsretrycount"            },
    { ARB_MB3SDR0,  15,  0, "bss3 rtstxcount"               },
    { ARB_MB3SDR1,  31, 16, "bss3 ackfailcount"             },
    { ARB_MB3SDR1,  15,  0, "bss3 bamisscount"              },
    { ARB_MB3SDR2,  31, 16, "bss3 frameretry2count"         },
    { ARB_MB3SDR2,  15,  0, "bss3 frameretrycount"          },
    { ARB_MB3SDR3,  15,  0, "bss3 frameretry3count"         },
};

    for (i = 0; i < sizeof(counters) / sizeof(struct mib_counter_s); i++) {
        volatile uint32_t  *reg = (uint32_t *)counters[i].addr;
        uint32_t            val = *reg;

        val   = val & BITS(counters[i].bit_lo, counters[i].bit_hi);
        val >>= counters[i].bit_lo;

        printf("%30s    %u\n", (unsigned int)counters[i].addr,
                               counters[i].str,
                               (unsigned int)val);
    }

    return status;
}
#endif //MTK_WIFI_MIB_ENABLE
