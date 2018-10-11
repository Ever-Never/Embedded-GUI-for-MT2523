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
/****************************************************************************
    Module Name:
    WiFi

    Abstract:
    WiFi processor configure / setting for STA operations

    Revision History:
    Who                     When                 What
    Michael Rong      2015/04/24       Initial
    --------            ----------      ------------------------------------------
***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type_def.h"
#include "os.h"
#include "os_util.h"
#include "nvdm.h"
#include "ethernet_filter.h"
#include "syslog.h"
#include "hal_efuse.h"
#include "misc.h"
#include "wifi_host_api.h"
#include "wfc_stub.h"
#include "host_os_utils.h"
#include "wifi_host_event.h"
#include "wifi_host_os_api.h"
#include "wfc_at_api.h"


#define RSP_BUF_SIZE 1024
uint8_t rsp_buf[RSP_BUF_SIZE];
uint32_t rsp_size;

wifi_scan_list_item_t *g_scan_list = NULL;
uint8_t g_scan_inited = 0;
uint8_t g_scan_list_size = 0;
volatile bool g_wifi_host_scanning = false;

#define WIFI_ERR_PARA_INVALID (-1)
#define WIFI_ERR_NOT_SUPPORT  (-1)

/**
* @brief This function sets the Wi-Fi operation mode and it takes effect immediately.
*
* @param[in] mode the operation mode to set.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0x01                       | #WIFI_MODE_STA_ONLY|
* \b 0x02                       | #WIFI_MODE_AP_ONLY|
* \b 0x03                       | #WIFI_MODE_REPEATER|
* \b 0x04                       | #WIFI_MODE_MONITOR|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note This function resets the Wi-Fi driver configuration to default values of the target's operation mode (including supplicant and firmware) previously set by Wi-Fi Configuration APIs.
* @note Once the Wi-Fi operation mode is set to station mode, the device will switch to idle state and will try to connect to the AP
* @note only after calling #wifi_host_config_reload_setting().
*/
int32_t wifi_host_config_set_opmode(uint8_t mode)
{
    int32_t ret;
    wh_set_opmode_cmd_para_t cmd_para;
    wh_set_opmode_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_OPMODE;
    cmd_para.api_id.set = true;
    cmd_para.opmode = mode;
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_set_opmode_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    return ret;
}


/**
* @brief This function receives the Wi-Fi operation mode of the Wi-Fi driver.
*
* @param[out]  mode indicates the operation mode.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0x01                       | #WIFI_MODE_STA_ONLY|
* \b 0x02                       | #WIFI_MODE_AP_ONLY|
* \b 0x03                       | #WIFI_MODE_REPEATER|
* \b 0x04                       | #WIFI_MODE_MONITOR|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_get_opmode(uint8_t *mode)
{
    int32_t ret;
    wh_get_opmode_cmd_para_t cmd_para;
    wh_get_opmode_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_OPMODE;
    cmd_para.api_id.set = false;
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_get_opmode_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    *mode = rsp_para->opmode;
    return ret;
}

/**
* @brief This function sets the SSID and SSID length that the Wi-Fi driver uses for a specific wireless port.
* This operation only takes effect after #wifi_host_config_reload_setting() is called.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] ssid is the SSID content. If #WIFI_PORT_STA or #WIFI_PORT_APCLI, the SSID indicates the target AP's SSID.
*                       If WIFI_PORT_AP, the SSID indicates the device's own SSID.
* @param[in] ssid_length is the length of the SSID, the maximum length is #WIFI_MAX_LENGTH_OF_SSID in bytes.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_set_ssid(uint8_t port, uint8_t *ssid, uint8_t ssid_length)
{
    int32_t ret;
    wh_set_ssid_cmd_para_t cmd_para;
    wh_set_ssid_resp_para_t *rsp_para;
    if(ssid_length > WIFI_MAX_LENGTH_OF_SSID) {
        return -1;
    }
    cmd_para.api_id.api = WFC_API_SSID;
    cmd_para.api_id.set = true;
    cmd_para.port = port;
    cmd_para.ssid_len = ssid_length;

    os_memcpy(cmd_para.ssid, ssid, ssid_length);
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_set_ssid_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    return ret;

}

/**
* @brief This function sets the authentication and encryption modes used in the Wi-Fi driver for a specific wireless port.
* This operation only takes effect after #wifi_host_config_reload_setting() is called.
*
* @param[in] port is the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] auth_mode is the authentication mode. For details about this parameter, please refer to
* #wifi_auth_mode_t.
* @param[in] encrypt_type is the encryption mode. For details about this parameter, please refer to
* #wifi_encrypt_type_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note In station mode, the device can connect to AP without setting any security mode.\n
*       If #wifi_host_config_set_security_mode() is not called, the device will change security mode dynamically according to the target AP.\n
*       If #wifi_host_config_set_security_mode() is called, the device will set a security mode to connect to the target AP.\n
*       Calling #wifi_host_config_set_security_mode() is optional in station mode.\n
* To skip the security mode setting, simply do not call #wifi_host_config_set_security_mode() before #wifi_host_config_reload_setting().
*/
int32_t wifi_host_config_set_security_mode(uint8_t port, wifi_auth_mode_t auth_mode, wifi_encrypt_type_t encrypt_type)
{
    int32_t ret;
    wh_set_security_cmd_para_t cmd_para;
    wh_set_security_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_SECURITY;
    cmd_para.api_id.set = true;
    cmd_para.port = port;
    cmd_para.auth_mode = auth_mode;
    cmd_para.encrypt_type = encrypt_type;
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_set_security_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    return ret;

}

/**
* @brief This function sets the password used in the Wi-Fi driver for a specific wireless port.
* @param[in] port is the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] password is the password of AP.
*
* @param[in] password_length is the password length of AP.
*
* @param[in] wep_key_index is the wep key index of WEP AP. For other kinds of AP, set it to -1.
*
* @return >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_set_password(uint8_t port, uint8_t *password, uint8_t password_length, uint8_t wep_key_index)
{
    int32_t ret;
    wh_set_password_cmd_para_t cmd_para;
    wh_set_password_resp_para_t *rsp_para;
    if(password_length > WIFI_LENGTH_PASSPHRASE) {
        return -1;
    }
    cmd_para.api_id.api = WFC_API_PASSWD;
    cmd_para.api_id.set = true;
    cmd_para.port = port;
    cmd_para.wep_key_index = wep_key_index;
    cmd_para.password_len = password_length;
    os_memcpy(cmd_para.password, password, password_length);
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_set_password_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    return ret;

}

/**
* @brief This function informs the wpa_supplicant to reload the configuration and applies the
* configuration settings of the Wi-Fi Configuration APIs. This function is applied to
* the following APIs: #wifi_host_config_set_ssid(), #wifi_host_config_set_security_mode(),
* #wifi_host_config_set_wpa_psk_key(), #wifi_host_config_set_wep_key() and  #wifi_host_config_set_pmk().
* In wireless station mode, the device will use the new configuration and start to scan
* and connect to the target AP router. In wireless AP mode, device will load the new
* SSID and encryption information.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_reload_setting(void)
{
    int32_t ret;
    wh_set_reload_cmd_para_t cmd_para;
    wh_set_reload_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_RELOAD;
    cmd_para.api_id.set = true;
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_set_reload_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    return ret;
}

/**
* @brief This function disconnects specific station's connection, and takes effect immediately. This
* API is available only in the AP mode.
*
* @param[in] address is station's MAC address.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_connection_disconnect_ap(void)
{
    int32_t ret;
    wh_set_connection_cmd_para_t cmd_para;
    wh_set_connection_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_CONNECTION;
    cmd_para.api_id.set = true;
    cmd_para.port = WIFI_PORT_STA ;
    cmd_para.disconnect = true;
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_set_connection_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    return ret;
}

/**
* @brief This function gets the RSSI of the connected AP. It's only used for the STA mode and while the station is connected to the AP.
*
* @param[out]  rssi returns the RSSI of the connected AP.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*
*/
int32_t wifi_host_connection_get_rssi(int8_t *rssi)
{
    int32_t ret;
    wh_get_connection_cmd_para_t cmd_para;
    wh_get_connection_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_CONNECTION;
    cmd_para.api_id.set = false;
    cmd_para.port = WIFI_PORT_STA ;
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_get_connection_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    *rssi = rsp_para->rssi;
    return ret;

}

/**
* @brief This function gets the current STA port's link up or link down connection status.
*
* @param[out]  link_status indicates the current STA port's link up or link down connection status.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | #WIFI_STATUS_LINK_DISCONNECTED|
* \b 1                          | #WIFI_STATUS_LINK_CONNECTED|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note #WIFI_STATUS_LINK_DISCONNECTED indicates station may be in
*       IDLE/ SCAN/ CONNECTING state.
*/
int32_t wifi_host_connection_get_link_status(uint8_t *link_status)
{
    int32_t ret;
    wh_get_connection_cmd_para_t cmd_para;
    wh_get_connection_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_CONNECTION;
    cmd_para.api_id.set = false;
    cmd_para.port = WIFI_PORT_STA;
    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);
    rsp_para = (wh_get_connection_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;
    *link_status = rsp_para->link_status;
    return ret;
}

/**
* @brief This function initializes the scan table to the driver.
*
* @param[in] ap_list is a pointer of the user buffer where the further scanned AP list will be stored.
* @param[in] max_count is the maximum number of ap_list can be stored.
*
* @return   >=0 the operation completed successfully, <0 the operation failed.
*
* @note #wifi_host_connection_scan_init() should be called before calling #wifi_host_connection_start_scan(), and it should be called only once to initialize one scan.\n
*       When the scan is done, the scanned AP list is already stored in parameter ap_list with descending order of the RSSI values.
*/

int32_t wifi_host_connection_scan_init(wifi_scan_list_item_t *ap_list, uint32_t max_count)
{
    if(g_wifi_host_scanning == true) {
        return -1;
    }

    if (NULL == ap_list) {
        return -1;
    }
    wifi_os_task_enter_critical();
    if ((1 != g_scan_inited) && (NULL == g_scan_list)) {
        g_scan_list = ap_list;
        g_scan_list_size = max_count;
        os_memset(g_scan_list, 0, g_scan_list_size * sizeof(wifi_scan_list_item_t));
        g_scan_inited = 1;
        wifi_os_task_exit_critical();
    } else {
        wifi_os_task_exit_critical();
        return -1;
    }
    return 0;

}

/**
* @brief This function deinitializes the scan table.
*
* @return   >=0 the operation completed successfully, <0 the operation failed.
*
* @note When the scan is finished, #wifi_host_connection_scan_deinit() should be called to unload the buffer from the driver. After that, the data in the parameter ap_list can be safely processed by user applications, and then another scan can be initialized by calling #wifi_host_connection_scan_init().
*/
int32_t wifi_host_connection_scan_deinit(void)
{
    if(g_wifi_host_scanning == true) {
        return -1;
    }

    if (g_scan_inited == 0) {
        return -1;
    }
    g_scan_inited = 0;
    g_scan_list = NULL;
    g_scan_list_size = 0;
    return 0;

}

/**
* @brief This function starts Wi-Fi Scanning.
*
* @param[in] ssid specifies the SSID to be included in the probe request packet for scanning the hidden AP. If the SSID is NULL, the SSID field in probe request packet will be NULL.
* @param[in] ssid_length specifies the length of the SSID.
* @param[in] bssid specifies the BSSID of the AP to be scanned. If the BSSID is specified, the unicast probe request is sent. If the BSSID is NULL, then the broadcast probe request is sent.
* @param[in] scan_mode is the scan mode that can be either a full scan or a partial scan.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | Full|
* \b 1                          | Partial, it's expected to be used in the SoftAP mode and keep stations online.|
*
* In the partial scan mode, the scanning is performed as follows.
* If the AP is currently on channel 5, it jumps to channels (1, 2),
* and then back to channel 5. After that it scans the channels (3, 4).
* The number of channels it scans each time is specified in the implementation.
* @param[in] scan_option selects scan options based on one of the following: active scan, passive scan or force active scan.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | Active scan, (Passive in regulatory channels). The channels that will be scanned are decided by the product's country and region. A probe request will be sent to each of these channels.|
* \b 1                          | Passive in all channels. Receives and processes the beacon. No probe request is sent.|
* \b 2                          | Force active (active in all channels). Forced to active scan in all channels. A probe request will be sent to each of these channels.|
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note 1. No matter what the SSID and BSSID parameters are, the device will receive all beacon and probe response packets over the air, \n
* and record the result into ap_list buffer registered by #wifi_host_connection_scan_init().
* @note 2. The station mode supports only the full scan mode.
* @note 3. The AP/repeater/p2p GO mode supports only the partial scan mode.
*/
int32_t wifi_host_connection_start_scan(uint8_t *ssid, uint8_t ssid_length, uint8_t *bssid, uint8_t scan_mode, uint8_t scan_option)
{
    if(g_wifi_host_scanning == true) {
        return -1;
    }

    if ((NULL != ssid) && (ssid_length > WIFI_MAX_LENGTH_OF_SSID)) {
        //LOG_E(wifi, "ssid_length is invalid: %d", ssid_length);
        return WIFI_ERR_PARA_INVALID;
    }
    if (scan_mode > 1) {
        //LOG_E(wifi, "scan_mode is invalid: %d", scan_mode);
        return WIFI_ERR_PARA_INVALID;
    }
    if (scan_option > 2) {
        //LOG_E(wifi, "scan_option is invalid: %d", scan_option);
        return WIFI_ERR_PARA_INVALID;
    }


    int32_t ret;
    wh_set_scan_start_cmd_para_t cmd_para;
    wh_set_scan_start_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_SCAN_START;
    cmd_para.api_id.set = true;
    if(ssid != NULL) {
        os_memcpy(cmd_para.ssid, ssid, ssid_length);
        cmd_para.bssid_valid = true;
    } else {
        cmd_para.bssid_valid = false;
    }
    if(bssid != NULL) {
        os_memcpy(cmd_para.bssid, bssid, WIFI_MAC_ADDRESS_LENGTH);
        cmd_para.bssid_valid = true;
    } else {
        cmd_para.bssid_valid = false;
    }

    cmd_para.scan_mode = scan_mode;
    cmd_para.scan_option = scan_option;

    cmd_para.support_number = g_scan_list_size;

    ret = wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);

    rsp_para = (wh_set_scan_start_resp_para_t *)rsp_buf;

    if (ret == WFC_RSP_OK && rsp_para->error_code > 0){
        g_wifi_host_scanning = true;
        ret = rsp_para->error_code;
    }

    return ret; 

    //return wifi_inband_start_scan(ssid, ssid_length, bssid, scan_mode, scan_option);

}


/**
* @brief This function stops the Wi-Fi Scanning triggered by #wifi_host_connection_start_scan(). If the device in station mode cannot connect to an AP, it keeps scanning till it connects to the target AP.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_connection_stop_scan(void)
{

    int32_t ret;
    wh_set_scan_stop_cmd_para_t cmd_para;
    wh_set_scan_stop_resp_para_t *rsp_para;
    cmd_para.api_id.api = WFC_API_SCAN_STOP;
    cmd_para.api_id.set = true;

    wfcm_to_slave(WFC_CMD_API, (uint8_t *)&cmd_para, sizeof(cmd_para), (uint8_t *)&rsp_buf, &rsp_size, 0);

    g_wifi_host_scanning = false;
    rsp_para = (wh_set_scan_stop_resp_para_t *)rsp_buf;
    ret = rsp_para->error_code;

    return ret;
    //return wifi_inband_stop_scan();

}

/**
* @brief This function registers the Wi-Fi event handler. Each event can register multiple event handlers with maximum number of 16.
* In the AP/STA mode, the scan stops automatically when the scan cycle is finished
* and SCAN COMPLETE event is triggered;\n
* In the AP mode, once the device is disconnected from the station,  a DISCONNECT event with station's
* MAC address is triggered;\n
* In the AP mode, once the station connects to the device, CONNECT event with station's
* MAC address is triggered;\n
* In the STA mode, once the device disconnects from the AP, DISCONNECT event with BSSID is triggered;\n
* In the STA mode, once the device connect to the AP, CONNECT event with BSSID is triggered.\n
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t.
*
* @param[in] handler is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note  It is not recommended to call any other Wi-Fi APIs or sleep in any form in the event handler.
*/
int32_t wifi_host_connection_register_event_handler(wifi_event_t event, wifi_event_handler_t handler)
{
    return wifi_api_set_event_handler(1, event, handler);
}

/**
* @brief This function unregisters Wi-Fi event handler. The function #wifi_host_connection_register_event_handler() registers an event and matches it with the corresponding event handler. For the event behavior, please refer to #wifi_host_connection_register_event_handler().
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t for more details.
*
* @param[in] handler is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_connection_unregister_event_handler(wifi_event_t event, wifi_event_handler_t handler)
{
    return wifi_api_set_event_handler(0, event, handler);
}

/**
* @brief This function starts wifi module. \n
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_set_wifi_start(void)
{
    int32_t ret = 0;
    return ret;
}


/**
* @brief This function stops wifi module. \n
*
** @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_set_wifi_stop(void)
{
    int32_t ret = 0;
    return ret;
}


