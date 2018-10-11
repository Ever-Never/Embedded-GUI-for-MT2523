/* Copyright Statement:
 *
 * (C) 2017-2100  MediaTek Inc. All rights reserved.
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
* @file wfc_at_api.h
*
*  Wi-Fi API configuration/settings in WFC.
*
*/

/**@addtogroup WFC
* @{
*
* This section introduces the Wi-Fi API configuration/settings in WFC.
*
*/

#ifndef __WFC_AT_API_H__
#define __WFC_AT_API_H__

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**@defgroup WIFI_DEFINE Define
* @{
*/

/**
* @brief Represents max support number of wifi scan list.
*/
#define SCAN_AP_LIST_MAX_COUNT 10

/**
* @brief Represents pack align method of WFC wifi host api command and response.
*/
#define WFC_PACK __attribute__((__packed__))

/**
* @}
*/

/**@defgroup WIFI_ENUM Enumeration
* @{
*/

/**
* @brief Represents the wifi api type defined in wfc.
*/
enum E_WFC_API
{
    WFC_API_OPMODE,
    WFC_API_SSID,
    WFC_API_SECURITY,
    WFC_API_PASSWD,
    WFC_API_RELOAD,
    WFC_API_CONNECTION,
    WFC_API_SCAN_START,
    WFC_API_SCAN_STOP,
    WFC_API_MAC_ADDR,
    WFC_API_INIT_PARA,
};

/**
* @}
*/


/**@defgroup WIFI_TYPEDEF Typedef
* @{
*/

/**
* @brief Represents the wifi api id structure defined in wfc.
*/
typedef struct{
    uint32_t api;
    bool set;
} WFC_PACK wh_api_id_t;


/**
* @brief Represents the command/response structure of #WFC_API_OPMODE command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t opmode;
} WFC_PACK wh_set_opmode_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_opmode_resp_para_t;

typedef struct {
    wh_api_id_t api_id;
} WFC_PACK wh_get_opmode_cmd_para_t;

typedef struct {
    int32_t error_code;
    uint8_t opmode;
} WFC_PACK wh_get_opmode_resp_para_t;


/**
* @brief Represents the command/response structure of #WFC_API_SSID command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    uint8_t ssid_len;
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];
} WFC_PACK wh_set_ssid_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_ssid_resp_para_t;


typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
} WFC_PACK wh_get_ssid_cmd_para_t;

typedef struct {
    int32_t error_code;
    uint8_t ssid_len;
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];
} WFC_PACK wh_get_ssid_resp_para_t;



/**
* @brief Represents the command/response structure of #WFC_API_SECURITY command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    uint8_t auth_mode;
    uint8_t encrypt_type;

} WFC_PACK wh_set_security_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_security_resp_para_t;


typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
} WFC_PACK wh_get_security_cmd_para_t;

typedef struct {
    int32_t error_code;
    uint8_t auth_mode;
    uint8_t encrypt_type;
} WFC_PACK wh_get_security_resp_para_t;



/**
* @brief Represents the command/response structure of #WFC_API_PASSWD command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    int8_t wep_key_index;
    uint8_t password_len;
    uint8_t password[WIFI_LENGTH_PASSPHRASE];
} WFC_PACK wh_set_password_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_password_resp_para_t;


typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    int8_t wep_key_index;
} WFC_PACK wh_get_password_cmd_para_t;

typedef struct {
    int32_t error_code;
    uint8_t password_len;
    uint8_t password[WIFI_LENGTH_PASSPHRASE];
} WFC_PACK wh_get_password_resp_para_t;


/**
* @brief Represents the command/response structure of #WFC_API_RELOAD command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
} WFC_PACK wh_set_reload_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_reload_resp_para_t;


/**
* @brief Represents the command/response structure of #WFC_API_CONNECTION command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    bool disconnect;
    uint8_t client_mac[WIFI_MAC_ADDRESS_LENGTH];
} WFC_PACK wh_set_connection_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_connection_resp_para_t;


typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    uint8_t client_mac[WIFI_MAC_ADDRESS_LENGTH];
} WFC_PACK wh_get_connection_cmd_para_t;

typedef struct {
    int32_t error_code;
    uint8_t link_status;
    int8_t rssi;
    uint8_t ssid_len;
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];
} WFC_PACK wh_get_connection_resp_para_t;


/**
* @brief Represents the command/response structure of #WFC_API_SCAN_START command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    bool ssid_valid;
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];
    uint8_t ssid_length;
    bool bssid_valid;
    uint8_t bssid[WIFI_MAC_ADDRESS_LENGTH];
    uint8_t scan_mode;
    uint8_t scan_option;
    uint8_t support_number;
} WFC_PACK wh_set_scan_start_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_scan_start_resp_para_t;


/**
* @brief Represents the command/response structure of #WFC_API_SCAN_STOP command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
} WFC_PACK wh_set_scan_stop_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_scan_stop_resp_para_t;


/**
* @brief Represents the command/response structure of #WFC_API_MAC_ADDR command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    uint8_t mac_addr[WIFI_MAC_ADDRESS_LENGTH];
} WFC_PACK wh_set_mac_addr_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_mac_addr_resp_para_t;


typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
} WFC_PACK wh_get_mac_addr_cmd_para_t;

typedef struct {
    int32_t error_code;
    uint8_t mac_addr[WIFI_MAC_ADDRESS_LENGTH];
} WFC_PACK wh_get_mac_addr_resp_para_t;


/**
* @brief Represents the command/response structure of #WFC_API_INIT_PARA command defined in wfc.
*/
typedef struct {
    wh_api_id_t api_id;
    uint8_t port;
    bool config_valid;
    wifi_config_t  config;
    bool config_ext_valid;
    wifi_config_ext_t config_ext;
} WFC_PACK wh_set_init_para_cmd_para_t;

typedef struct {
    int32_t error_code;
} WFC_PACK wh_set_init_para_resp_para_t;


/**
* @brief Represents the structure of wifi scan result defined in wfc.
*/
typedef struct {
    uint32_t list_size;
    wifi_scan_list_item_t scan_list[];
} WFC_PACK wh_scan_list_t;


/**
* @brief Represents the structure of wifi scan result event defined in wfc.
*/
typedef struct {
    uint16_t evt_id;
    wh_scan_list_t scan_table;
} WFC_PACK wh_scan_reslt_evt_para_t;


/**
* @}
*/

/**@defgroup WIFI_STRUCT Structure
* @{
*/

/**
* @brief Represents the advanced wifi setting from host.
*/
#ifndef WIFI_HOST_ADV_SETTING_STRUCT
typedef struct  {
    uint8_t sta_mac[WIFI_MAC_ADDRESS_LENGTH];
    uint8_t ap_mac[WIFI_MAC_ADDRESS_LENGTH];
}wifi_host_adv_setting_t;
#define WIFI_HOST_ADV_SETTING_STRUCT
#endif

/**
* @brief Represents wfc system setting.
*/
typedef struct {
    wifi_config_t wfc_wifi_config;
    wifi_config_ext_t wfc_wifi_config_ext;
    wifi_host_adv_setting_t wfc_wifi_host_adv_config;
    uint8_t wfc_wifi_status;
    uint8_t wfc_wifi_config_valid;
    uint8_t wfc_wifi_config_ext_valid;
    uint8_t wfc_wifi_host_adv_config_valid;
} wfc_system_config_t;

/**
*@}
*/

/**
* @brief This  function sets the wifi MAC in wfc module which be set from host
*            this function must invoked before triggering "WFC_API_INIT_PARA" in Host, as the MAC need to be loaded in before wifi_init() in Device
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* @param[in] pmac_addr is the pointer of mac address
*
* @return   >=0 means success, <0 means fail.
*/
int32_t wfc_set_wifi_host_mac(uint8_t port, uint8_t *pmac_addr);


/**
* @brief  This function gets the wifi MAC in wfc module which be set from host.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* @param[out] pmac_addr is the pointer of mac address.
*
* @return  >=0 means success, <0 means fail.
*/
int32_t wfc_get_wifi_host_mac(uint8_t port, uint8_t *pmac_addr);


/**
* @brief  This function sets the received host wifi settings to wfc module
*
* @param[in] pconfig is the pointer of wifi basic config structure
*
* @param[in] pconfig_ext is the pointer of wifi extend config structure
*
* @returnl None
*/
void wfc_set_wifi_host_setting(wifi_config_t *pconfig, wifi_config_ext_t*pconfig_ext);


/**
* @brief  This function gets the wifi settings in wfc module which be set from host
*
* @param[out] pconfig is the pointer of wifi basic config structure
*
* @param[out] pconfig_valid indicates whether the wifi basic config value is valid
*
* @param[out] pconfig_ext is the pointer of wifi extend config structure
*
* @param[out] pconfig_ext_valid indicates whether the wifi extend config value is valid
*
* @return None
*/
void wfc_get_wifi_host_setting(wifi_config_t *pconfig, uint8_t *pconfig_valid, wifi_config_ext_t *pconfig_ext,uint8_t *pconfig_ext_valid);


/**
* @brief This function process the wfc api command from host.
*
* @param[in] pcmd is the pointer of wfc api command.
*
* @param[in] cmdSize is the length of wfc api command.
*
* @param[out] txptr is the pointer of response for wfc api command.
*
* @param[out] txsize is the length of response for wfc api command.
*
* @return  >=0 means success, <0 means fail.
*
*/
int32_t wfc_api_process(uint8_t *pcmd,  uint32_t cmdSize, uint8_t *txptr, uint32_t *txsize);


#ifdef __cplusplus
}
#endif

/**
*@}
*/
#endif  /* __WFC_AT_API_H__ */
