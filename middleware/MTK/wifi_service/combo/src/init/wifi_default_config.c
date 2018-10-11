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
#include <string.h>
#include "os.h"
#include "wifi_api.h"
#include "wifi_default_config.h"
#include "syslog.h"
#include "get_profile_string.h"
#include "wifi_init.h"

int32_t wifi_get_default_config(wifi_sys_cfg_t *syscfg)
{
    if (NULL == syscfg) {
        LOG_E(wifi, "syscfg is null");
        return -1;
    }

    syscfg->opmode =                  (uint8_t)atoi(WIFI_DEFAULT_OPMODE);
    syscfg->country_region =          (uint8_t)atoi(WIFI_DEFAULT_COUNTRY_REGION);
    syscfg->country_region_a_band =   (uint8_t)atoi(WIFI_DEFAULT_COUNTRY_REGION_A_BAND);
    os_strncpy((char *)syscfg->country_code, WIFI_DEFAULT_COUNTRY_CODE, sizeof(syscfg->country_code));
    wifi_country_code_region_mapping(syscfg->country_code, &(syscfg->country_region), &(syscfg->country_region_a_band));
    syscfg->radio_off =               (uint8_t)atoi(WIFI_DEFAULT_RADIO_ONOFF);
    syscfg->dbg_level =               (uint8_t)atoi(WIFI_DEFAULT_N9_DEBUG_LEVEL);
    syscfg->rts_threshold =           (uint16_t)atoi(WIFI_DEFAULT_RTS_THRESHOLD);
    syscfg->frag_threshold =          (uint16_t)atoi(WIFI_DEFAULT_FRAGMENT_THRESHOLD);

    syscfg->sta_local_admin_mac =     (uint8_t)atoi(WIFI_DEFAULT_STA_LOCAL_ADMIN_MAC);
    //wifi_conf_get_ip_from_str(syscfg->sta_ip_addr, "192.168.1.1");
    if (0 > wifi_config_get_mac_address(WIFI_PORT_STA, syscfg->sta_mac_addr)) {
        LOG_W(wifi, "wifi_config_get_mac_address fail.");
        return -1;
    }
    os_strncpy((char *)syscfg->sta_ssid, WIFI_DEFAULT_STA_SSID, sizeof(syscfg->sta_ssid));
    syscfg->sta_ssid_len =            (uint8_t)atoi(WIFI_DEFAULT_STA_SSID_LEN);
    syscfg->sta_bss_type =            (uint8_t)atoi(WIFI_DEFAULT_STA_BSS_TYPE);
    syscfg->sta_channel =             (uint8_t)atoi(WIFI_DEFAULT_STA_CHANNEL);
    syscfg->sta_bw =                  (uint8_t)atoi(WIFI_DEFAULT_STA_BANDWIDTH);
    syscfg->sta_wireless_mode =       (uint8_t)atoi(WIFI_DEFAULT_STA_WIRELESS_MODE);
    syscfg->sta_ba_decline =          (uint8_t)atoi(WIFI_DEFAULT_STA_BA_DECLINE);
    syscfg->sta_auto_ba =             (uint8_t)atoi(WIFI_DEFAULT_STA_AUTO_BA);
    syscfg->sta_ht_mcs =              (uint8_t)atoi(WIFI_DEFAULT_STA_HT_MCS);
    syscfg->sta_ht_ba_win_size =      (uint8_t)atoi(WIFI_DEFAULT_STA_HT_BA_WINDOW_SIZE);
    syscfg->sta_ht_gi =               (uint8_t)atoi(WIFI_DEFAULT_STA_HT_GI);
    syscfg->sta_ht_protect =          (uint8_t)atoi(WIFI_DEFAULT_STA_HT_PROTECT);
    syscfg->sta_ht_ext_ch =           (uint8_t)atoi(WIFI_DEFAULT_STA_HT_EXT_CHANNEL);
    syscfg->sta_wmm_capable =         (uint8_t)atoi(WIFI_DEFAULT_STA_WMM_CAPABLE);
    syscfg->sta_listen_interval =     (uint8_t)atoi(WIFI_DEFAULT_STA_LISTEN_INTERVAL);
    syscfg->sta_auth_mode =           (uint8_t)atoi(WIFI_DEFAULT_STA_AUTH_MODE);
    syscfg->sta_encryp_type =         (uint8_t)atoi(WIFI_DEFAULT_STA_ENCRYPT_TYPE);
    os_strncpy((char *)syscfg->sta_wpa_psk, WIFI_DEFAULT_STA_WPA_PSK, sizeof(syscfg->sta_wpa_psk));
    syscfg->sta_wpa_psk_len =         (uint8_t)atoi(WIFI_DEFAULT_STA_WPA_PSK_LEN);
    //os_strncpy((char *)syscfg->sta_pmk, "", sizeof(syscfg->sta_pmk));
    syscfg->sta_pair_cipher =         (uint8_t)atoi(WIFI_DEFAULT_STA_PAIR_CIPHER);;
    syscfg->sta_group_cipher =        (uint8_t)atoi(WIFI_DEFAULT_STA_GROUP_CIPHER);;
    syscfg->sta_default_key_id =      (uint8_t)atoi(WIFI_DEFAULT_STA_DEFAULT_KEY_ID);;
    syscfg->sta_ps_mode = wifi_get_ps_mode();
    syscfg->sta_keep_alive_period =   (uint8_t)atoi(WIFI_DEFAULT_STA_KEEP_ALIVE_PERIOD);

    syscfg->ap_local_admin_mac =      (uint8_t)atoi(WIFI_DEFAULT_AP_LOCAL_ADMIN_MAC);
    //wifi_conf_get_ip_from_str(syscfg->ap_ip_addr, "192.168.1.1");
    if (0 > wifi_config_get_mac_address(WIFI_PORT_AP, syscfg->ap_mac_addr)) {
        LOG_W(wifi, "wifi_config_get_mac_address fail.");
        return -1;
    }
    os_strncpy((char *)syscfg->ap_ssid, WIFI_DEFAULT_AP_SSID, sizeof(syscfg->ap_ssid));
    syscfg->ap_ssid_len =             (uint8_t)atoi(WIFI_DEFAULT_AP_SSID_LEN);
    syscfg->ap_channel =              (uint8_t)atoi(WIFI_DEFAULT_AP_CHANNEL);
    syscfg->ap_bw =                   (uint8_t)atoi(WIFI_DEFAULT_AP_BANDWIDTH);
    syscfg->ap_wireless_mode =        (uint8_t)atoi(WIFI_DEFAULT_AP_WIRELESS_MODE);
    syscfg->ap_auto_ba =              (uint8_t)atoi(WIFI_DEFAULT_AP_AUTO_BA);
    syscfg->ap_ht_mcs =               (uint8_t)atoi(WIFI_DEFAULT_AP_HT_MCS);
    syscfg->ap_ht_ba_win_size =       (uint8_t)atoi(WIFI_DEFAULT_AP_HT_BA_WINDOW_SIZE);
    syscfg->ap_ht_gi =                (uint8_t)atoi(WIFI_DEFAULT_AP_HT_GI);
    syscfg->ap_ht_protect =           (uint8_t)atoi(WIFI_DEFAULT_AP_HT_PROTECT);
    syscfg->ap_ht_ext_ch =            (uint8_t)atoi(WIFI_DEFAULT_AP_HT_EXT_CHANNEL);
    syscfg->ap_wmm_capable =          (uint8_t)atoi(WIFI_DEFAULT_AP_WMM_CAPABLE);
    syscfg->ap_dtim_period =          (uint8_t)atoi(WIFI_DEFAULT_AP_DTIM_PERIOD);
    syscfg->ap_hide_ssid =            (uint8_t)atoi(WIFI_DEFAULT_AP_HIDDEN_SSID);
    syscfg->ap_auto_channel_select =  (uint8_t)atoi(WIFI_DEFAULT_AP_AUTO_CHANNEL_SELECT);
    syscfg->ap_auth_mode =            (uint8_t)atoi(WIFI_DEFAULT_AP_AUTH_MODE);
    syscfg->ap_encryp_type =          (uint8_t)atoi(WIFI_DEFAULT_AP_ENCRYPT_TYPE);
    os_strncpy((char *)syscfg->ap_wpa_psk, WIFI_DEFAULT_AP_WPA_PSK, sizeof(syscfg->ap_wpa_psk));
    syscfg->ap_wpa_psk_len =          (uint8_t)atoi(WIFI_DEFAULT_AP_WPA_PSK_LEN);
    //os_strncpy((char *)syscfg->ap_pmk, "", sizeof(syscfg->ap_pmk));
    syscfg->ap_pair_cipher =          (uint8_t)atoi(WIFI_DEFAULT_AP_PAIR_CIPHER);
    syscfg->ap_group_cipher =         (uint8_t)atoi(WIFI_DEFAULT_AP_GROUP_CIPHER);
    syscfg->ap_default_key_id =       (uint8_t)atoi(WIFI_DEFAULT_AP_DEFAULT_KEY_ID);
    syscfg->ap_beacon_disable =       (uint8_t)atoi(WIFI_DEFAULT_AP_BEACON_DISABLE);
    syscfg->forwarding_zero_copy = 1;

    syscfg->bg_band_entry_num = wifi_conf_get_ch_table_from_str(syscfg->bg_band_triple,
                                                                10, WIFI_DEFAULT_BG_CHANNEL_TABLE,
                                                                os_strlen(WIFI_DEFAULT_BG_CHANNEL_TABLE));

    syscfg->a_band_entry_num = wifi_conf_get_ch_table_from_str(syscfg->a_band_triple,
                                                               10, WIFI_DEFAULT_A_CHANNEL_TABLE,
                                                               os_strlen(WIFI_DEFAULT_A_CHANNEL_TABLE));
    syscfg->mbss_enable =             (uint8_t)atoi(WIFI_DEFAULT_MBSS_ENABLE);
    os_strncpy((char *)syscfg->mbss_ssid1, WIFI_DEFAULT_MBSS_SSID1, sizeof(syscfg->mbss_ssid1));
    syscfg->mbss_ssid_len1 = os_strlen(WIFI_DEFAULT_MBSS_SSID1);
    os_strncpy((char *)syscfg->mbss_ssid2, WIFI_DEFAULT_MBSS_SSID2, sizeof(syscfg->mbss_ssid2));
    syscfg->mbss_ssid_len2 = os_strlen(WIFI_DEFAULT_MBSS_SSID2);

#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
    syscfg->config_free_ready =       (uint8_t)atoi(WIFI_DEFAULT_CONFIG_FREE_READY);
    syscfg->config_free_enable =      (uint8_t)atoi(WIFI_DEFAULT_CONFIG_FREE_ENABLE);
#endif
    syscfg->beacon_lost_time =        (uint8_t)atoi(WIFI_DEFAULT_STA_BEACON_LOST_TIME);
    syscfg->apcli_40mhz_auto_upbelow =(uint8_t)atoi(WIFI_DEFAULT_APCLI_BW_AUTO_UP_BELOW);
    syscfg->wifi_privilege_enable =   (uint8_t)atoi(WIFI_DEFAULT_WIFI_PRIVILEGE_ENABLE);
    syscfg->sta_keep_alive_packet =   (uint8_t)atoi(WIFI_DEFAULT_STA_KEEP_ALIVE_PACKET);

#ifdef MTK_SINGLE_SKU_SUPPORT
    syscfg->support_single_sku  =     (uint8_t)atoi(WIFI_DEFAULT_SUPPORT_SINGLE_SKU);
    wifi_conf_get_pwr_from_str(sizeof(singleSKU2G_t), &syscfg->single_sku_2g, WIFI_DEFAULT_SKU_2G_GROUP0);
    wifi_conf_get_pwr_from_str(sizeof(singleSKU5G_t), &syscfg->single_sku_5g, WIFI_DEFAULT_SKU_5G_GROUP0);
#endif

#ifdef MTK_ANT_DIV_ENABLE
    syscfg->antenna_number = (uint8_t)atoi(WIFI_DEFAULT_ANT_NUMBER);
    syscfg->antenna_diversity_enable = (uint8_t)atoi(WIFI_DEFAULT_ANT_DIVERSITY_ENABLE);
    syscfg->antenna_probe_req_count = (uint8_t)atoi(WIFI_DEFAULT_ANT_PROB_REQ_COUNT);
    syscfg->antenna_threshold_level = (uint8_t)atoi(WIFI_DEFAULT_ANT_THRESHOLD_LEVEL);
    syscfg->antenna_delta_rssi = (uint8_t)atoi(WIFI_DEFAULT_ANT_DELTA_RSSI);
    syscfg->antenna_rssi_switch = (uint8_t)atoi(WIFI_DEFAULT_ANT_RSSI_SWITCH);
    syscfg->antenna_pre_selected_rssi = (uint8_t)atoi(WIFI_DEFAULT_ANT_PRE_SELECTED_RSSI);
#endif

    return 0;
}

