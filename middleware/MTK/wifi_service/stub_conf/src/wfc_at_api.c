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

#include "wifi_api.h"
//#include "wifi_os_api.h"
#include "wfc_stub.h"
#include "wfc_sdio.h"
 
#include "lwip/pbuf.h"
#include "wfc_at_api.h"
#include "wifi_os_api.h"
#include "os_util.h"
#include "os.h"


/*the semaphore to get wifi host setting,  then start wifi_init in device*/
extern os_semaphore_t g_sem_wfc_host_cfg;
wfc_system_config_t g_wfc_system_config = {{0}};

wifi_scan_list_item_t scan_ap_list[SCAN_AP_LIST_MAX_COUNT];
int scan_ap_size = 0;
extern uint8_t g_scan_inited;

#ifdef MTK_WIFI_STUB_CONF_ENABLE
uint8_t wfc_send_to_host(uint8_t *pbuf, uint32_t bufSize, uint8_t isEvt);
#endif

static int scan_event_handler_sample(wifi_event_t event_id, unsigned char *payload, unsigned int len)
{
    int handled = 0;
    int i = 0;
#ifdef MTK_WIFI_STUB_CONF_ENABLE
    uint32_t size = 0;
    wh_scan_reslt_evt_para_t *evt_para = NULL;
#endif
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
#ifdef MTK_WIFI_STUB_CONF_ENABLE
            size = sizeof(wh_scan_reslt_evt_para_t) + sizeof(wifi_scan_list_item_t)*scan_ap_size;
            evt_para = (wh_scan_reslt_evt_para_t *)os_malloc(size);
            evt_para->evt_id = WFC_EVT_SCAN_RESULTS;
            evt_para->scan_table.list_size = scan_ap_size;
            os_memcpy(evt_para->scan_table.scan_list, scan_ap_list, sizeof(wifi_scan_list_item_t)*scan_ap_size);
            wfc_send_to_host((uint8_t *)evt_para, size, 1);
            os_free(evt_para);
#endif
            scan_ap_size = 0;
            break;

        default:
            handled = 0;
            printf("[MTK Event Callback Sample]: Unknown event(%d)\n", event_id);
            break;
    }
    return handled;

}



int32_t wfc_api_process(uint8_t *pcmd,  uint32_t cmdSize, uint8_t *txptr, uint32_t *txsize)
{
    int32_t ret = 0;
    wh_api_id_t *api_id = (wh_api_id_t *)pcmd;
    
    LOG_I(wfc, "<WFC>wfc_api_process(%u)(set=%d)",api_id->api,api_id->set);
    
    switch(api_id->api)
    {                 
        case WFC_API_OPMODE:
        {
            if(api_id->set == true) {
                wh_set_opmode_cmd_para_t  *set_opmode_cmd = (wh_set_opmode_cmd_para_t *)pcmd;
                wh_set_opmode_resp_para_t set_opmode_resp;
                set_opmode_resp.error_code = wifi_config_set_opmode(set_opmode_cmd->opmode);
                ret = set_opmode_resp.error_code;
                os_memcpy(txptr, &set_opmode_resp, sizeof(set_opmode_resp));
                *txsize = sizeof(set_opmode_resp);
            } else {
                wh_get_opmode_resp_para_t get_opmode_resp;
                get_opmode_resp.error_code = wifi_config_get_opmode(&get_opmode_resp.opmode);
                ret = get_opmode_resp.error_code;
                os_memcpy(txptr, &get_opmode_resp, sizeof(get_opmode_resp));
                *txsize = sizeof(get_opmode_resp);
            }
        
            break;
        }
        case WFC_API_SSID:  
        {
            if(api_id->set == true) {
                wh_set_ssid_cmd_para_t  *set_ssid_cmd = (wh_set_ssid_cmd_para_t *)pcmd;
                wh_set_ssid_resp_para_t set_ssid_resp;
                set_ssid_resp.error_code = wifi_config_set_ssid(set_ssid_cmd->port, set_ssid_cmd->ssid, set_ssid_cmd->ssid_len);
                ret = set_ssid_resp.error_code;
                os_memcpy(txptr, &set_ssid_resp, sizeof(set_ssid_resp));
                *txsize = sizeof(set_ssid_resp);
            } else {
                wh_get_ssid_cmd_para_t  *get_ssid_cmd = (wh_get_ssid_cmd_para_t *)pcmd;
                wh_get_ssid_resp_para_t get_ssid_resp;
                get_ssid_resp.error_code = wifi_config_get_ssid(get_ssid_cmd->port, get_ssid_resp.ssid, &get_ssid_resp.ssid_len);
                ret = get_ssid_resp.error_code;
                os_memcpy(txptr, &get_ssid_resp, sizeof(get_ssid_resp));
                *txsize = sizeof(get_ssid_resp);
            }
            break;
        }
        case WFC_API_SECURITY:  
        {
            if(api_id->set == true) {
                wh_set_security_cmd_para_t  *set_security_cmd = (wh_set_security_cmd_para_t *)pcmd;
                wh_set_security_resp_para_t set_security_resp;
                set_security_resp.error_code = wifi_config_set_security_mode(set_security_cmd->port, set_security_cmd->auth_mode, set_security_cmd->encrypt_type);
                ret = set_security_resp.error_code;
                os_memcpy(txptr, &set_security_resp, sizeof(set_security_resp));
                *txsize = sizeof(set_security_resp);
            } else {
                wh_get_security_cmd_para_t  *get_security_cmd = (wh_get_security_cmd_para_t *)pcmd;
                wh_get_security_resp_para_t get_security_resp;
                get_security_resp.error_code = wifi_config_get_security_mode(get_security_cmd->port, &get_security_resp.auth_mode, &get_security_resp.encrypt_type);
                ret = get_security_resp.error_code;
                os_memcpy(txptr, &get_security_resp, sizeof(get_security_resp));
                *txsize = sizeof(get_security_resp);
            }
            break;
        }
        case WFC_API_PASSWD:
        {
            if(api_id->set == true) {
                wh_set_password_cmd_para_t  *set_password_cmd = (wh_set_password_cmd_para_t *)pcmd;
                wh_set_password_resp_para_t set_password_resp;
                if(set_password_cmd->wep_key_index >= 0 && set_password_cmd->wep_key_index < WIFI_NUMBER_WEP_KEYS) {
                    wifi_wep_key_t wep_keys= {{{0}}};
                    if(set_password_cmd->password_len > WIFI_MAX_WEP_KEY_LENGTH) {
                        return -1;
                    }
                    wep_keys.wep_tx_key_index = set_password_cmd->wep_key_index;
                    wep_keys.wep_key_length[set_password_cmd->wep_key_index] = set_password_cmd->password_len;
                    os_memcpy(wep_keys.wep_key[set_password_cmd->wep_key_index], set_password_cmd->password, set_password_cmd->password_len);
                    set_password_resp.error_code = wifi_config_set_wep_key(set_password_cmd->port, &wep_keys);    
                } else {
                    set_password_resp.error_code = wifi_config_set_wpa_psk_key(set_password_cmd->port, set_password_cmd->password, set_password_cmd->password_len);
                }
                ret = set_password_resp.error_code;
                os_memcpy(txptr, &set_password_resp, sizeof(set_password_resp));
                *txsize = sizeof(set_password_resp);
            } else {
                wh_get_password_cmd_para_t  *get_password_cmd = (wh_get_password_cmd_para_t *)pcmd;
                wh_get_password_resp_para_t get_password_resp;
                if(get_password_cmd->wep_key_index >= 0 && get_password_cmd->wep_key_index < WIFI_NUMBER_WEP_KEYS) {
                    wifi_wep_key_t wep_keys;
                    get_password_resp.error_code = wifi_config_get_wep_key(get_password_cmd->port, &wep_keys);
                    get_password_resp.password_len = wep_keys.wep_key_length[get_password_cmd->wep_key_index];
                    if(get_password_resp.password_len > WIFI_MAX_WEP_KEY_LENGTH) {
                        return -1;
                    }
                    os_memcpy(get_password_resp.password, wep_keys.wep_key[get_password_cmd->wep_key_index], wep_keys.wep_key_length[get_password_cmd->wep_key_index]);
                } else {
                    get_password_resp.error_code = wifi_config_get_wpa_psk_key(get_password_cmd->port, get_password_resp.password, &get_password_resp.password_len);
                }
                ret = get_password_resp.error_code;
                os_memcpy(txptr, &get_password_resp, sizeof(get_password_resp));
                *txsize = sizeof(get_password_resp);
            }
            break;
        }
        case WFC_API_RELOAD:
        {
            if(api_id->set == true) {
                wh_set_reload_resp_para_t set_reload_resp;
                set_reload_resp.error_code = wifi_config_reload_setting();
                ret = set_reload_resp.error_code;
                os_memcpy(txptr, &set_reload_resp, sizeof(set_reload_resp));
                *txsize = sizeof(set_reload_resp);
            }
            break;
        }
        case WFC_API_CONNECTION:    
        {
            if(api_id->set == true) {
                wh_set_connection_cmd_para_t  *set_connection_cmd = (wh_set_connection_cmd_para_t *)pcmd;
                wh_set_connection_resp_para_t set_connection_resp;
                if(set_connection_cmd->disconnect == true) {
                    if(set_connection_cmd->port == WIFI_PORT_STA) {
                        set_connection_resp.error_code = wifi_connection_disconnect_ap();
                    } else {
                        set_connection_resp.error_code = wifi_connection_disconnect_sta(set_connection_cmd->client_mac);

                    }
                } else {
                    set_connection_resp.error_code = wifi_config_reload_setting();
                }
                ret = set_connection_resp.error_code;
                os_memcpy(txptr, &set_connection_resp, sizeof(set_connection_resp));
                *txsize = sizeof(set_connection_resp);
            } else {
                wh_get_connection_cmd_para_t  *get_connection_cmd = (wh_get_connection_cmd_para_t *)pcmd;
                wh_get_connection_resp_para_t get_connection_resp;
                os_memset(&get_connection_resp, 0 ,sizeof(wh_get_connection_resp_para_t));
                if(get_connection_cmd->port == WIFI_PORT_STA) {
                    get_connection_resp.error_code = wifi_connection_get_rssi(&get_connection_resp.rssi);
                    get_connection_resp.error_code = wifi_connection_get_link_status(&get_connection_resp.link_status);
                }
                else {
                    return -1;
                }
                ret = get_connection_resp.error_code;
                os_memcpy(txptr, &get_connection_resp, sizeof(get_connection_resp));
                *txsize = sizeof(get_connection_resp);
            }
            break;
        }
        case WFC_API_SCAN_START:    
        {
            if(api_id->set == true) {
                wh_set_scan_start_cmd_para_t  *set_scan_start_cmd = (wh_set_scan_start_cmd_para_t *)pcmd;
                wh_set_scan_start_resp_para_t set_scan_start_resp;
                uint8_t *ssid;
                uint8_t ssid_length;
                uint8_t *bssid;
                if(set_scan_start_cmd->ssid_valid == false) {
                    ssid = NULL;
                    ssid_length = 0;
                } else {
                    ssid = set_scan_start_cmd->ssid;
                    ssid_length = set_scan_start_cmd->ssid_length;
                }
                if(set_scan_start_cmd->bssid_valid == false) {
                    bssid = NULL;
                } else {
                    bssid = set_scan_start_cmd->bssid;
                }
                uint32_t number = set_scan_start_cmd->support_number;
                if(number > SCAN_AP_LIST_MAX_COUNT) {
                    number = SCAN_AP_LIST_MAX_COUNT;
                }
                if (1 == g_scan_inited) {
                    wifi_connection_scan_deinit();
                    wifi_connection_unregister_event_handler(1, (wifi_event_handler_t) scan_event_handler_sample);
                }
                scan_ap_size = number;
                wifi_connection_register_event_handler(1, (wifi_event_handler_t) scan_event_handler_sample);
                wifi_connection_scan_init(scan_ap_list, number);

                set_scan_start_resp.error_code = wifi_connection_start_scan(ssid, ssid_length, bssid, set_scan_start_cmd->scan_mode, set_scan_start_cmd->scan_option);

                ret = set_scan_start_resp.error_code;
                os_memcpy(txptr, &set_scan_start_resp, sizeof(set_scan_start_resp));
                *txsize = sizeof(set_scan_start_resp);
            } else {

                return -1;
            }
            break;
        }
        case WFC_API_SCAN_STOP:     
        {
            if(api_id->set == true) {
                wh_set_scan_stop_resp_para_t set_scan_stop_resp;
                set_scan_stop_resp.error_code = wifi_connection_stop_scan();
                ret = set_scan_stop_resp.error_code;
                os_memcpy(txptr, &set_scan_stop_resp, sizeof(set_scan_stop_resp));
                *txsize = sizeof(set_scan_stop_resp);
            } else {

                return -1;
            }
            break;
        }
        case WFC_API_MAC_ADDR:
        {
            if(api_id->set == true) {
                wh_set_mac_addr_cmd_para_t  *set_mac_addr_cmd = (wh_set_mac_addr_cmd_para_t *)pcmd;
                wh_set_mac_addr_resp_para_t set_mac_addr_resp;
                set_mac_addr_resp.error_code = wfc_set_wifi_host_mac(set_mac_addr_cmd->port, set_mac_addr_cmd->mac_addr);
                ret = set_mac_addr_resp.error_code;
                os_memcpy(txptr, &set_mac_addr_resp, sizeof(set_mac_addr_resp));
                *txsize = sizeof(set_mac_addr_resp);
            } else {
                wh_get_mac_addr_cmd_para_t  *get_mac_addr_cmd = (wh_get_mac_addr_cmd_para_t *)pcmd;
                wh_get_mac_addr_resp_para_t get_mac_addr_resp;
                get_mac_addr_resp.error_code = wifi_config_get_mac_address(get_mac_addr_cmd->port, get_mac_addr_resp.mac_addr);
                ret = get_mac_addr_resp.error_code;
                os_memcpy(txptr, &get_mac_addr_resp, sizeof(get_mac_addr_resp));
                *txsize = sizeof(get_mac_addr_resp);
            }
            break;
        }
        case WFC_API_INIT_PARA:     
        {
            if(api_id->set == true) {
                wh_set_init_para_cmd_para_t *set_init_para_cmd = (wh_set_init_para_cmd_para_t *)pcmd;
                wh_set_init_para_resp_para_t set_init_para_resp;
                wifi_config_t  *pconfig;
                wifi_config_ext_t *pconfig_ext;
                if(set_init_para_cmd->config_valid == false) {
                    pconfig = NULL;
                } else {
                    pconfig = &set_init_para_cmd->config;
                }
                if(set_init_para_cmd->config_ext_valid == false) {
                    pconfig_ext = NULL;
                } else {
                    pconfig_ext = &set_init_para_cmd->config_ext;
                }

                wfc_set_wifi_host_setting(pconfig, pconfig_ext);

                set_init_para_resp.error_code = 0;
                os_memcpy(txptr, &set_init_para_resp, sizeof(set_init_para_resp));
                *txsize = sizeof(set_init_para_resp);
            } else {

                return -1;
            }
            break;
        }

        default:
            break;
    }

    return ret;

}

static int32_t wfc_is_mac_address_valid(uint8_t *mac_addr)
{
    uint32_t byte_sum = 0;
    for (uint32_t index = 0; index < WIFI_MAC_ADDRESS_LENGTH; index++) {
        byte_sum += mac_addr[index];
    }
    return (byte_sum != 0);
}


/**
  * @brief  wfc_get_wifi_host_mac  function to get the wifi MAC in wfc module which be set from host
  *            this function must invoked before triggering "WFC_API_INIT_PARA" in Host, as the MAC need to be loaded in before wifi_init() in Device
  * @param  None
  * @retval None
  */
int32_t wfc_set_wifi_host_mac(uint8_t port, uint8_t *pmac_addr)
{
    if (port == WIFI_PORT_STA){
        os_memcpy(g_wfc_system_config.wfc_wifi_host_adv_config.sta_mac, pmac_addr, WIFI_MAC_ADDRESS_LENGTH);
    }else if(port == WIFI_PORT_AP){
        os_memcpy(g_wfc_system_config.wfc_wifi_host_adv_config.ap_mac, pmac_addr, WIFI_MAC_ADDRESS_LENGTH);
    }
   
    //LOG_I(wfc, "wfc_set_wifi_host_mac (port:%d) MAC: 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,\n\r",
    //    port,pmac_addr[0],pmac_addr[1],pmac_addr[2],pmac_addr[3],pmac_addr[4],pmac_addr[5]);

    return 0;
}

/**
  * @brief  wfc_get_wifi_host_mac  function to get the wifi MAC in wfc module which be set from host
  * @param  None
  * @retval None
  */
int32_t wfc_get_wifi_host_mac(uint8_t port, uint8_t *pmac_addr)
{
    if (port == WIFI_PORT_STA){
        if(wfc_is_mac_address_valid(g_wfc_system_config.wfc_wifi_host_adv_config.sta_mac)){
            os_memcpy(pmac_addr, g_wfc_system_config.wfc_wifi_host_adv_config.sta_mac, WIFI_MAC_ADDRESS_LENGTH);
            return 0;
        }
    }else if(port == WIFI_PORT_AP){
        if(wfc_is_mac_address_valid(g_wfc_system_config.wfc_wifi_host_adv_config.ap_mac)){
            os_memcpy(pmac_addr, g_wfc_system_config.wfc_wifi_host_adv_config.ap_mac, WIFI_MAC_ADDRESS_LENGTH);
            return 0;
        }
    }
    return -1;
}


/**
  * @brief  wifi_wfc_set_host_setting  function to set the received host wifi settings to wfc module
  * @param  None
  * @retval None
  */
void wfc_set_wifi_host_setting(wifi_config_t *pconfig, wifi_config_ext_t*pconfig_ext)
{   
    if (pconfig == NULL) {
        g_wfc_system_config.wfc_wifi_config_valid = FALSE;
    } else {
        os_memcpy(&g_wfc_system_config.wfc_wifi_config, pconfig, sizeof(wifi_config_t) );
        g_wfc_system_config.wfc_wifi_config_valid = TRUE;
    }

    if (pconfig_ext == NULL) {
        g_wfc_system_config.wfc_wifi_config_ext_valid = FALSE;
    } else {
        os_memcpy(&g_wfc_system_config.wfc_wifi_config_ext, pconfig_ext, sizeof(wifi_config_ext_t));
        g_wfc_system_config.wfc_wifi_config_ext_valid = TRUE;
    }

    wifi_os_semphr_give(g_sem_wfc_host_cfg);
}

/*[JCB]extern for test*/
extern void hex_dump(char *str, unsigned char *pSrcBufVA, unsigned int SrcBufLen);

/**
  * @brief  wifi_wfc_get_host_setting  function to get the wifi settings in wfc module which be set from host
  * @param  None
  * @retval None
  */
void wfc_get_wifi_host_setting(wifi_config_t *pconfig, uint8_t *pconfig_valid, wifi_config_ext_t *pconfig_ext,uint8_t *pconfig_ext_valid)
{
    LOG_I(common, "wfc_get_wifi_host_setting");

    /*1: need a semaphone to wait wfc get host setting done*/
    if( g_sem_wfc_host_cfg != NULL ) {
        if (wifi_os_semphr_take(g_sem_wfc_host_cfg, NULL) == 0){
            /*2: copy the config ,config_ext setting from wfc module*/
        #if 0
            pconfig->opmode = g_wfc_system_config.wfc_wifi_config.opmode;
            pconfig->sta_config.ssid_length = g_wfc_system_config.wfc_wifi_config.sta_config.ssid_length;
            memcpy(pconfig->sta_config.ssid,
                g_wfc_system_config.wfc_wifi_config.sta_config.ssid, 
                WIFI_MAC_ADDRESS_LENGTH);

            pconfig->sta_config.password_length = g_wfc_system_config.wfc_wifi_config.sta_config.password_length;
            memcpy(pconfig->sta_config.password, 
                g_wfc_system_config.wfc_wifi_config.sta_config.password, 
                pconfig->sta_config.password_length);

            pconfig->sta_config.bssid_present = g_wfc_system_config.wfc_wifi_config.sta_config.bssid_present;
            memcpy(pconfig->sta_config.bssid, 
                g_wfc_system_config.wfc_wifi_config.sta_config.bssid, 
                WIFI_MAC_ADDRESS_LENGTH);
            
            pconfig->ap_config.auth_mode    = g_wfc_system_config.wfc_wifi_config.ap_config.auth_mode;
            pconfig->ap_config.bandwidth    = g_wfc_system_config.wfc_wifi_config.ap_config.bandwidth;
            pconfig->ap_config.bandwidth_ext= g_wfc_system_config.wfc_wifi_config.ap_config.bandwidth_ext;
            pconfig->ap_config.channel      = g_wfc_system_config.wfc_wifi_config.ap_config.channel;
            pconfig->ap_config.encrypt_type = g_wfc_system_config.wfc_wifi_config.ap_config.encrypt_type;
            pconfig->ap_config.ssid_length  = g_wfc_system_config.wfc_wifi_config.ap_config.ssid_length;
            memcpy(pconfig->ap_config.ssid,
                g_wfc_system_config.wfc_wifi_config.ap_config.ssid,
                WIFI_MAC_ADDRESS_LENGTH);
            
            pconfig->ap_config.password_length= g_wfc_system_config.wfc_wifi_config.ap_config.password_length;
            memcpy(pconfig->ap_config.password,
                g_wfc_system_config.wfc_wifi_config.ap_config.password, 
                pconfig->ap_config.password_length);
        #else
            if(g_wfc_system_config.wfc_wifi_config_valid == TRUE){
                os_memcpy(pconfig,     &g_wfc_system_config.wfc_wifi_config,     sizeof(wifi_config_t));
                *pconfig_valid = TRUE;
            } else {
                pconfig = NULL;
                *pconfig_valid = FALSE;
            }

            if(g_wfc_system_config.wfc_wifi_config_ext_valid == TRUE){
                os_memcpy(pconfig_ext, &g_wfc_system_config.wfc_wifi_config_ext, sizeof(wifi_config_ext_t));
                *pconfig_ext_valid = TRUE;
            } else {
                pconfig_ext = NULL;
                *pconfig_ext_valid = FALSE;
            }
        #endif

            printf("pconfig->opmode=%d\r\n",                    pconfig->opmode);
            printf("pconfig->sta_config.ssid_length=%d\r\n",    pconfig->sta_config.ssid_length);
            printf("pconfig->sta_config.password_length=%d\r\n",pconfig->sta_config.password_length);
            printf("pconfig->sta_config.bssid_present=%d\r\n",  pconfig->sta_config.bssid_present);
            
            printf("pconfig->sta_config.ssid=%s\r\n",           pconfig->sta_config.ssid);
            hex_dump("pconfig->sta_config.password: ",          pconfig->sta_config.password, pconfig->sta_config.password_length);
            hex_dump("pconfig->sta_config.bssid: ",             pconfig->sta_config.bssid, 6);

            wifi_os_semphr_give(g_sem_wfc_host_cfg);
        } else {
            goto Exit;
        }
    } else {
        goto Exit;
    }

    return;
    
Exit:
    LOG_I(common, "wfc_get_wifi_host_setting fail");
    pconfig = NULL;
    pconfig_ext = NULL;
    *pconfig_valid = FALSE;
    *pconfig_ext_valid = FALSE;
}


void wfc_set_wifi_status(uint8_t wifi_event_id)
{
    g_wfc_system_config.wfc_wifi_status = wifi_event_id;
}

