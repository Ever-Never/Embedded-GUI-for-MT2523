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

#include "os.h"
#include "wifi_api.h"
#include "wifi_private_api.h"
#include "connsys_driver.h"
#include "wifi_scan.h"
#include "inband_queue.h"
#include "net_task.h"
#include "wifi_init.h"
#include "syslog.h"
#include "nvdm.h"
#include "wifi_profile.h"
#include "connsys_profile.h"
#include "get_profile_string.h"
#include "wifi_default_config.h"
#include "wifi_inband.h"
#include "wifi_channel.h"
#include "hal_efuse.h"
#ifdef MTK_ANT_DIV_ENABLE
#include "wifi_ant_div.h"
#endif

#ifdef MTK_MINISUPP_ENABLE
#include "wpa_supplicant_task.h"
#endif

#ifdef MTK_CM4_WIFI_TASK_ENABLE
#include "wifi_firmware_task.h"
#endif

#if MTK_WIFI_STUB_CONF_ENABLE
#include "wfc_at_api.h"
#endif

#if (PRODUCT_VERSION == 5932) || (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 7686)
uint32_t MACLPSState = 0, MACLPNum = 0;
#endif

int32_t wifi_get_pmk_by_ssid_psk_from_pmkinfo(uint8_t *pmk,uint8_t *ssid,uint8_t ssid_len, uint8_t *psk,uint8_t psk_len);

log_create_module(wifi, PRINT_LEVEL_ERROR);

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)
static int32_t wifi_is_mac_address_valid(uint8_t *mac_addr)
{
    uint32_t byte_sum = 0;
    for (uint32_t index = 0; index < WIFI_MAC_ADDRESS_LENGTH; index++) {
        byte_sum += mac_addr[index];
    }
    return (byte_sum != 0);
}
#endif

wifi_phy_mode_t wifi_change_wireless_mode_5g_to_2g(wifi_phy_mode_t wirelessmode)
{
    if (WIFI_PHY_11A == wirelessmode) {
        return WIFI_PHY_11B;
    } else if (WIFI_PHY_11ABG_MIXED == wirelessmode) {
        return WIFI_PHY_11BG_MIXED;
    } else if (WIFI_PHY_11ABGN_MIXED == wirelessmode) {
        return WIFI_PHY_11BGN_MIXED;
    } else if (WIFI_PHY_11AN_MIXED == wirelessmode) {
        return WIFI_PHY_11N_2_4G;
    } else if (WIFI_PHY_11AGN_MIXED == wirelessmode) {
        return WIFI_PHY_11GN_MIXED;
    } else if (WIFI_PHY_11N_5G == wirelessmode) {
        return WIFI_PHY_11N_2_4G;
    } else {
        return wirelessmode;
    }
}

/*Add the wifi_init_callback,  to avoid the modules dependency,
   some of callback will be invoked in the wifi driver which located in the src_protected path */
wifi_init_callback g_wifi_init_callback;

static void wifi_init_register_callback()
{
    g_wifi_init_callback.wifi_get_pmk_info = wifi_get_pmk_by_ssid_psk_from_pmkinfo;
#ifdef MTK_ANT_DIV_ENABLE
    g_wifi_init_callback.wifi_record_ant_status = wifi_profile_record_antenna_status;
#endif
}

/**
* @brief get mac address from efuse
*/
static int32_t wifi_get_mac_addr_from_efuse(uint8_t port, uint8_t *mac_addr)
{
#if (PRODUCT_VERSION == 5932) || (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 7686)
    if (wifi_config_get_mac_from_register(port, mac_addr) < 0) {
        return -1;
    }
#elif (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)
    uint8_t buf[16] = {0};      //efuse is 16 byte aligned
    uint16_t mac_offset = 0x00; //mac addr offset in efuse
    if (HAL_EFUSE_OK != hal_efuse_read(mac_offset, buf, sizeof(buf))) {
        return -1;
    }
    if (!wifi_is_mac_address_valid(buf+4)) {
        LOG_HEXDUMP_W(wifi, "data in efuse is invalid", buf, sizeof(buf));
        return -1;
    }
    if (WIFI_PORT_STA == port) {
        /* original efuse MAC address for STA */
        os_memcpy(mac_addr, buf+4, WIFI_MAC_ADDRESS_LENGTH);
    } else {
        /* original efuse MAC address with byte[5]+1 for AP */
        os_memcpy(mac_addr, buf+4, WIFI_MAC_ADDRESS_LENGTH);
        mac_addr[WIFI_MAC_ADDRESS_LENGTH-1] += 1;
    }
#endif
    return 0;
}


#ifdef MTK_NVDM_ENABLE
/**
* @brief get mac address from nvdm
*/
static int32_t wifi_get_mac_addr_from_nvdm(uint8_t port, uint8_t *mac_addr)
{
    uint8_t buff[PROFILE_BUF_LEN] = {0};
    uint32_t len = sizeof(buff);
    char *group_name = (WIFI_PORT_STA == port) ? "STA" : "AP";

    if (NVDM_STATUS_OK != nvdm_read_data_item(group_name, "MacAddr", buff, &len)) {
        return -1;
    }

    wifi_conf_get_mac_from_str((char *)mac_addr, (char *)buff);
    return 0;
}
#endif

/**
* @brief Get WiFi Interface MAC Address.
*
*/
int32_t wifi_config_get_mac_address(uint8_t port, uint8_t *address)
{
    if (NULL == address) {
        LOG_E(wifi, "address is null.");
        return WIFI_ERR_PARA_INVALID;
    }

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }

#ifdef MTK_WIFI_STUB_CONF_ENABLE
    /*in MT5932, consider to use WFC Host MAC first,  then consider the Efuse MAC*/
    if (0 == wfc_get_wifi_host_mac(port, address)){
        return 0;
    }

    LOG_W(wifi, "wfc_get_wifi_host_mac fail.");
#endif

    if (0 == wifi_get_mac_addr_from_efuse(port, address)) {
        return 0;
    }
#if (PRODUCT_VERSION == 5932)
   /*use default MAC if MT5932 get the Efuse MAC fail*/
    else {
      uint8_t tmp_mac[WIFI_MAC_ADDRESS_LENGTH]={0x00, 0x0c, 0x43, 0x76, 0x86, 0x09};
      tmp_mac[WIFI_MAC_ADDRESS_LENGTH-1] += port; /* STA/AP use different MAC address */

      os_memcpy(address, tmp_mac, WIFI_MAC_ADDRESS_LENGTH);
        LOG_E(wifi, "Get mac from efuse/CR fail! Apply temp MAC addr");
        return 0;
     }
#endif

    LOG_W(wifi, "wifi_get_mac_addr_from_efuse fail.");
#ifdef MTK_NVDM_ENABLE
    if (0 == wifi_get_mac_addr_from_nvdm(port, address)) {
        return 0;
    }
    LOG_E(wifi, "wifi_get_mac_addr_from_nvdm fail.");
#endif
    return -1;

}


uint8_t wifi_get_ps_mode(void)
{
#ifdef MTK_WIFI_PROFILE_ENABLE
    uint8_t buff[PROFILE_BUF_LEN];
    uint32_t len = sizeof(buff);
    nvdm_read_data_item("STA", "PSMode", buff, &len);
    return (uint8_t)atoi((char *)buff);
#else
    return (uint8_t)atoi(WIFI_DEFAULT_STA_POWER_SAVE_MODE);
#endif
}

static void wifi_save_sta_ext_config(wifi_sys_cfg_t *syscfg, wifi_config_ext_t *config_ext)
{
    if (NULL != config_ext) {
        if (config_ext->sta_wep_key_index_present) {
            syscfg->sta_default_key_id = config_ext->sta_wep_key_index;
        }
        if (config_ext->sta_bandwidth_present) {
            syscfg->sta_bw = config_ext->sta_bandwidth;
        }
        if (config_ext->sta_wireless_mode_present) {
            syscfg->sta_wireless_mode = config_ext->sta_wireless_mode;
        }
        if (config_ext->sta_listen_interval_present) {
            syscfg->sta_listen_interval = config_ext->sta_listen_interval;
        }
        if (config_ext->sta_power_save_mode_present) {
            syscfg->sta_ps_mode = config_ext->sta_power_save_mode;
        }

    }
}

static void wifi_save_ap_ext_config(wifi_sys_cfg_t *syscfg, wifi_config_ext_t *config_ext)
{
    if (NULL != config_ext) {
        if (config_ext->ap_wep_key_index_present) {
            syscfg->ap_default_key_id = config_ext->ap_wep_key_index;
        }
        if (config_ext->ap_hidden_ssid_enable_present) {
            syscfg->ap_hide_ssid = config_ext->ap_hidden_ssid_enable;
        }
        if (config_ext->ap_wireless_mode_present) {
            syscfg->ap_wireless_mode = config_ext->ap_wireless_mode;
        }
        if (config_ext->ap_dtim_interval_present) {
            syscfg->ap_dtim_period = config_ext->ap_dtim_interval;
        }
    }
}

static int32_t wifi_apply_sta_config(wifi_sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    if( NULL != config ){
        os_memcpy(syscfg->sta_ssid, config->sta_config.ssid, WIFI_MAX_LENGTH_OF_SSID);
        if( WIFI_MAX_LENGTH_OF_SSID < config->sta_config.ssid_length ){
            //config->sta_config.ssid_length = WIFI_MAX_LENGTH_OF_SSID;
            LOG_E(wifi, "sta_config.ssid_length is longer than %d. initial aborted!", WIFI_MAX_LENGTH_OF_SSID);
            return WIFI_FAIL;
        }
        syscfg->sta_ssid_len = config->sta_config.ssid_length;

        os_memcpy(syscfg->sta_wpa_psk, config->sta_config.password, WIFI_LENGTH_PASSPHRASE);
        if( WIFI_LENGTH_PASSPHRASE < config->sta_config.password_length ){
            //config->sta_config.password_length = WIFI_LENGTH_PASSPHRASE;
            LOG_E(wifi, "sta_config.password_length is longer than %d. initial aborted!", WIFI_LENGTH_PASSPHRASE);
            return WIFI_FAIL;
        }
        syscfg->sta_wpa_psk_len = config->sta_config.password_length;
    }
    /* save extension config */
    if( NULL != config_ext ){
        wifi_save_sta_ext_config(syscfg, config_ext);
    }
    return WIFI_SUCC;
}

static int32_t wifi_apply_ap_config(wifi_sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    if( NULL != config ){
        os_memcpy(syscfg->ap_ssid, config->ap_config.ssid, WIFI_MAX_LENGTH_OF_SSID);
        if( WIFI_MAX_LENGTH_OF_SSID < config->ap_config.ssid_length ){
            //config->ap_config.ssid_length = WIFI_MAX_LENGTH_OF_SSID;
            LOG_E(wifi, "ap_config.ssid_length is longer than %d. initial aborted!", WIFI_MAX_LENGTH_OF_SSID);
            return WIFI_FAIL;
        }
        syscfg->ap_ssid_len = config->ap_config.ssid_length;

        os_memcpy(syscfg->ap_wpa_psk, config->ap_config.password, WIFI_LENGTH_PASSPHRASE);
        if( WIFI_MAX_LENGTH_OF_SSID < config->ap_config.password_length ){
            //config->ap_config.password_length = WIFI_MAX_LENGTH_OF_SSID;
            LOG_E(wifi, "ap_config.password_length is longer than %d. initial aborted!", WIFI_LENGTH_PASSPHRASE);
            return WIFI_FAIL;
        }
        syscfg->ap_wpa_psk_len = config->ap_config.password_length;

        syscfg->ap_auth_mode = config->ap_config.auth_mode;

        syscfg->ap_encryp_type = config->ap_config.encrypt_type;

        syscfg->ap_channel = config->ap_config.channel;

        syscfg->ap_bw = config->ap_config.bandwidth;

        syscfg->ap_ht_ext_ch = (WIFI_BANDWIDTH_EXT_40MHZ_UP == config->ap_config.bandwidth_ext) ? 1 : 3;
    }

    /* save extension config */
    if( NULL!= config_ext ){
        wifi_save_ap_ext_config(syscfg, config_ext);
    }
    return WIFI_SUCC;
}

static int32_t wifi_apply_repeater_config(wifi_sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    if( WIFI_SUCC != wifi_apply_sta_config(syscfg, config, config_ext)){
        return WIFI_FAIL;
    }

    if( WIFI_SUCC != wifi_apply_ap_config(syscfg, config, config_ext) ){
        return WIFI_FAIL;
    }
    return WIFI_SUCC;
}

static int32_t wifi_apply_p2p_config(wifi_sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    /* TBD */
    return WIFI_SUCC;
}

#ifdef MTK_WIFI_PROFILE_ENABLE

#if 0
#ifdef MTK_SINGLE_SKU_SUPPORT
/*need improve,  if the NVDM is disable in project,  it will build error here!*/
void wifi_get_single_sku_from_nvdm(singleSKU2G_t * single, uint8_t group_number)
{
    uint8_t buff_ext[1024];
    uint32_t len = sizeof(buff_ext);

    switch(group_number) {
        case 0:
            nvdm_read_data_item("common", "SingleSKU2G_0", buff_ext, &len);
            break;

        default:
            printf("deafult only support group 0\n");
            nvdm_read_data_item("common", "SingleSKU2G_0", buff_ext, &len);
            break;
    }
    wifi_conf_get_pwr_from_str(sizeof(singleSKU2G_t), single, (char *)buff_ext);
}
#endif/*MTK_SINGLE_SKU_SUPPORT*/
#endif

extern void wifi_conf_get_pwr_from_str(uint16_t ret_len, void * single, const char *pwr_src);

static void wifi_get_config_from_nvdm(wifi_sys_cfg_t *config)
{
    // init wifi profile
    uint8_t buff[PROFILE_BUF_LEN];
    uint32_t len = sizeof(buff);

    // common
    len = sizeof(buff);
    nvdm_read_data_item("common", "OpMode", buff, &len);
    config->opmode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "CountryRegion", buff, &len);
    config->country_region = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "CountryRegionABand", buff, &len);
    config->country_region_a_band = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "CountryCode", buff, &len);
    os_memcpy(config->country_code, buff, 4);
    wifi_country_code_region_mapping(config->country_code, &(config->country_region), &(config->country_region_a_band));
    len = sizeof(buff);
    nvdm_read_data_item("common", "RadioOff", buff, &len);
    config->radio_off = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "RTSThreshold", buff, &len);
    config->rts_threshold = (uint16_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "FragThreshold", buff, &len);
    config->frag_threshold = (uint16_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "DbgLevel", buff, &len);
    config->dbg_level = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("network", "IpAddr", buff, &len);
    wifi_conf_get_ip_from_str(config->ap_ip_addr, (char *)buff);
    wifi_conf_get_ip_from_str(config->sta_ip_addr, (char *)buff);
#ifdef MTK_ANT_DIV_ENABLE
    len = sizeof(buff);
    nvdm_read_data_item("common", "AntennaNumber", buff, &len);
    config->antenna_number = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "AntDiversityEnable", buff, &len);
    config->antenna_diversity_enable= (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "AntProbeReqCount", buff, &len);
    config->antenna_probe_req_count= (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "AntThresholdRssi", buff, &len);
    config->antenna_threshold_level= (int8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "AntDeltaRssi", buff, &len);
    config->antenna_delta_rssi= (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "AntRssiSwitch", buff, &len);
    config->antenna_rssi_switch= (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("common", "AntPreSelectedRssi", buff, &len);
    config->antenna_pre_selected_rssi= (int8_t)atoi((char *)buff);
#endif

    // STA
    len = sizeof(buff);
    nvdm_read_data_item("STA", "LocalAdminMAC", buff, &len);
    config->sta_local_admin_mac = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);

    //nvdm_read_data_item("STA", "MacAddr", buff, &len);
    //wifi_conf_get_mac_from_str((char *)config->sta_mac_addr, (char *)buff);
    if (wifi_config_get_mac_address(WIFI_PORT_STA, config->sta_mac_addr) < 0) {
        LOG_E(wifi, "Wi-Fi get MAC address fail.");
        return;
    }

    len = sizeof(buff);
    nvdm_read_data_item("STA", "SsidLen", buff, &len);
    config->sta_ssid_len = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "Ssid", buff, &len);
    os_memcpy(config->sta_ssid, buff, config->sta_ssid_len);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BssType", buff, &len);
    config->sta_bss_type = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "Channel", buff, &len);
    config->sta_channel = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BW", buff, &len);
    config->sta_bw = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WirelessMode", buff, &len);
    if (wifi_5g_support() < 0) {
        config->sta_wireless_mode = (uint8_t)wifi_change_wireless_mode_5g_to_2g((wifi_phy_mode_t)atoi((char *)buff));
    }else {
        config->sta_wireless_mode = (uint8_t)atoi((char *)buff);
    }
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BADecline", buff, &len);
    config->sta_ba_decline = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "AutoBA", buff, &len);
    config->sta_auto_ba = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_MCS", buff, &len);
    config->sta_ht_mcs = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_BAWinSize", buff, &len);
    config->sta_ht_ba_win_size = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_GI", buff, &len);
    config->sta_ht_gi = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_PROTECT", buff, &len);
    config->sta_ht_protect = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "HT_EXTCHA", buff, &len);
    config->sta_ht_ext_ch = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WmmCapable", buff, &len);
    config->sta_wmm_capable = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "ListenInterval", buff, &len);
    config->sta_listen_interval = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "AuthMode", buff, &len);
    config->sta_auth_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "EncrypType", buff, &len);
    config->sta_encryp_type = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WpaPskLen", buff, &len);
    config->sta_wpa_psk_len = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "WpaPsk", buff, &len);
    os_memcpy(config->sta_wpa_psk, buff, config->sta_wpa_psk_len);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "PMK", buff, &len);
    os_memcpy(config->sta_pmk, buff, 32); // TODO: How to save binary PMK value not ending by ' ; ' ?
    len = sizeof(buff);
    nvdm_read_data_item("STA", "PairCipher", buff, &len);
    config->sta_pair_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "GroupCipher", buff, &len);
    config->sta_group_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "DefaultKeyId", buff, &len);
    config->sta_default_key_id = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "PSMode", buff, &len);
    config->sta_ps_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "KeepAlivePeriod", buff, &len);
    config->sta_keep_alive_period = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "BeaconLostTime", buff, &len);
    config->beacon_lost_time = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("STA", "ApcliBWAutoUpBelow", buff, &len);
    config->apcli_40mhz_auto_upbelow = (uint8_t)atoi((char *)buff);

    // AP
#ifdef MTK_WIFI_REPEATER_ENABLE
    if (config->opmode == WIFI_MODE_REPEATER) {
        len = sizeof(buff);
        nvdm_read_data_item("STA", "Channel", buff, &len);
        config->ap_channel = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("STA", "BW", buff, &len);
        config->ap_bw = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("STA", "WirelessMode", buff, &len);
        config->ap_wireless_mode = (uint8_t)atoi((char *)buff);
    } else {
#endif
        /* Use STA MAC/IP as AP MAC/IP for the time being, due to N9 dual interface not ready yet */
        len = sizeof(buff);
        nvdm_read_data_item("AP", "Channel", buff, &len);
        config->ap_channel = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("AP", "BW", buff, &len);
        config->ap_bw = (uint8_t)atoi((char *)buff);
        len = sizeof(buff);
        nvdm_read_data_item("AP", "WirelessMode", buff, &len);
        if (wifi_5g_support() < 0) {
            config->ap_wireless_mode = (uint8_t)wifi_change_wireless_mode_5g_to_2g((wifi_phy_mode_t)atoi((char *)buff));
        }else {
            config->ap_wireless_mode = (uint8_t)atoi((char *)buff);
        }
#ifdef MTK_WIFI_REPEATER_ENABLE
    }
#endif /* MTK_WIFI_REPEATER_ENABLE */
    len = sizeof(buff);
    nvdm_read_data_item("AP", "LocalAdminMAC", buff, &len);
    config->ap_local_admin_mac = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);

    //nvdm_read_data_item("AP", "MacAddr", buff, &len);
    //wifi_conf_get_mac_from_str((char *)config->ap_mac_addr, (char *)buff);
    if (wifi_config_get_mac_address(WIFI_PORT_AP, config->ap_mac_addr) < 0) {
        LOG_E(wifi, "Wi-Fi get MAC address fail.");
        return;
    }

    len = sizeof(buff);
    nvdm_read_data_item("AP", "SsidLen", buff, &len);
    config->ap_ssid_len = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "Ssid", buff, &len);
    os_memcpy(config->ap_ssid, buff, config->ap_ssid_len);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "AutoBA", buff, &len);
    config->ap_auto_ba = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_MCS", buff, &len);
    config->ap_ht_mcs = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_BAWinSize", buff, &len);
    config->ap_ht_ba_win_size = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_GI", buff, &len);
    config->ap_ht_gi = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_PROTECT", buff, &len);
    config->ap_ht_protect = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HT_EXTCHA", buff, &len);
    config->ap_ht_ext_ch = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "WmmCapable", buff, &len);
    config->ap_wmm_capable = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "DtimPeriod", buff, &len);
    config->ap_dtim_period = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "HideSSID", buff, &len);
    config->ap_hide_ssid = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "AutoChannelSelect", buff, &len);
    config->ap_auto_channel_select = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "AuthMode", buff, &len);
    config->ap_auth_mode = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "EncrypType", buff, &len);
    config->ap_encryp_type = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "WpaPskLen", buff, &len);
    config->ap_wpa_psk_len = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "WpaPsk", buff, &len);
    os_memcpy(config->ap_wpa_psk, buff, config->ap_wpa_psk_len);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "PMK", buff, &len);
    os_memcpy(config->ap_pmk, buff, 32); // TODO: How to save binary PMK value not ending by ' ; ' ?
    len = sizeof(buff);
    nvdm_read_data_item("AP", "PairCipher", buff, &len);
    config->ap_pair_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "GroupCipher", buff, &len);
    config->ap_group_cipher = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "DefaultKeyId", buff, &len);
    config->ap_default_key_id = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("AP", "BcnDisEn", buff, &len);
    config->ap_beacon_disable = (uint8_t)atoi((char *)buff);

    // scan channel table and regulatory table
    len = sizeof(buff);
    nvdm_read_data_item("common", "BGChannelTable", buff, &len);
    config->bg_band_entry_num = wifi_conf_get_ch_table_from_str(config->bg_band_triple, 10, (char *)buff, os_strlen((char *)buff));

    len = sizeof(buff);
    nvdm_read_data_item("common", "AChannelTable", buff, &len);
    config->a_band_entry_num = wifi_conf_get_ch_table_from_str(config->a_band_triple, 10, (char *)buff, os_strlen((char *)buff));

    config->forwarding_zero_copy = 1;

#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE
    /* These are for MBSS support, but not exist trunk (it's customer feature), however,
            we have to add them here due to N9 FW has them (only one version of N9 FW)
         */
    // TODO: How to solve it in the future...Michael
    config->mbss_enable = 0;
    os_memset(config->mbss_ssid1, 0x0, sizeof(config->mbss_ssid1));;
    config->mbss_ssid_len1 = 0;
    os_memset(config->mbss_ssid2, 0x0, sizeof(config->mbss_ssid2));;
    config->mbss_ssid_len2 = 0;

    len = sizeof(buff);
    nvdm_read_data_item("config_free", "ConfigFree_Ready", buff, &len);
    config->config_free_ready = (uint8_t)atoi((char *)buff);
    len = sizeof(buff);
    nvdm_read_data_item("config_free", "ConfigFree_Enable", buff, &len);
    config->config_free_enable = (uint8_t)atoi((char *)buff);
#endif /* MTK_WIFI_CONFIGURE_FREE_ENABLE */
    len = sizeof(buff);
    nvdm_read_data_item("common", "StaFastLink", buff, &len);
    config->sta_fast_link = (uint8_t)atoi((char *)buff);

#ifdef MTK_WIFI_PRIVILEGE_ENABLE
    len = sizeof(buff);
    nvdm_read_data_item("common", "WiFiPrivilegeEnable", buff, &len);
    config->wifi_privilege_enable = (uint8_t)atoi((char *)buff);
#else
    config->wifi_privilege_enable = 0;
#endif
    len = sizeof(buff);
    nvdm_read_data_item("STA", "StaKeepAlivePacket", buff, &len);
    config->sta_keep_alive_packet = (uint8_t)atoi((char *)buff);

#ifdef MTK_SINGLE_SKU_SUPPORT
    len = sizeof(buff);
    nvdm_read_data_item("common", "SupportSingleSKU", buff, &len);
    config->support_single_sku= (uint8_t)atoi((char *)buff);
#if 0
    len = sizeof(buff);
    nvdm_read_data_item("common", "SingleSKU_index", buff, &len);

    if((uint8_t)atoi((char *)buff) == 0) {
        wifi_get_single_sku_from_nvdm(&config->single_sku_2g, 0);
    } else {
        LOG_E(wifi, "only support one group default");
    }
#else
    len = sizeof(buff);
    nvdm_read_data_item("common", "SingleSKU2G_0", buff, &len);
    wifi_conf_get_pwr_from_str(sizeof(singleSKU2G_t), &(config->single_sku_2g), (char *)buff);
#endif

    len = sizeof(buff);
    nvdm_read_data_item("common", "SingleSKU5G_L0", buff, &len);
    wifi_conf_get_pwr_from_str(WIFI_5G_LOW_CHANNEL_SKU_LEN, &(config->single_sku_5g), (char *)buff);

    len = sizeof(buff);
    nvdm_read_data_item("common", "SingleSKU5G_M0", buff, &len);
    wifi_conf_get_pwr_from_str(WIFI_5G_MIDDLE_CHANNEL_SKU_LEN, (char *)&(config->single_sku_5g) + WIFI_5G_LOW_CHANNEL_SKU_LEN, (char *)buff);

    len = sizeof(buff);
    nvdm_read_data_item("common", "SingleSKU5G_H0", buff, &len);
    wifi_conf_get_pwr_from_str(WIFI_5G_HIGH_CHANNEL_SKU_LEN, (char *)&(config->single_sku_5g) + (WIFI_5G_LOW_CHANNEL_SKU_LEN + WIFI_5G_MIDDLE_CHANNEL_SKU_LEN), (char *)buff);

#endif

}
#endif
static int32_t wifi_apply_user_config(wifi_sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    int32_t ret = WIFI_SUCC;

    if(NULL!=config){
        syscfg->opmode = config->opmode;
    }
    if ( config_ext!= NULL && config_ext->country_code_present) {
        os_memcpy(syscfg->country_code, config_ext->country_code, 4);
        if( WIFI_SUCC != wifi_country_code_region_mapping(syscfg->country_code, &(syscfg->country_region), &(syscfg->country_region_a_band)) ){
            LOG_E(wifi, "wifi_country_code_region_mapping fail. initial aborted!");
            return WIFI_FAIL;
        }
    }
    if (WIFI_MODE_STA_ONLY == syscfg->opmode) {
        ret = wifi_apply_sta_config(syscfg, config, config_ext);
    } else if (WIFI_MODE_AP_ONLY == syscfg->opmode) {
        ret = wifi_apply_ap_config(syscfg, config, config_ext);
    } else if (WIFI_MODE_REPEATER == syscfg->opmode) {
        ret = wifi_apply_repeater_config(syscfg, config, config_ext);
    } else if (WIFI_MODE_P2P_ONLY == syscfg->opmode) {
        ret = wifi_apply_p2p_config(syscfg, config, config_ext);
    } else {
        /* no configuration is required for Monitor Mode */
    }
    return ret;
}

/**
* @brief build the whole configurations
*/
static int32_t wifi_build_whole_config(wifi_sys_cfg_t *syscfg, wifi_config_t *config, wifi_config_ext_t *config_ext)
{
#ifdef MTK_WIFI_PROFILE_ENABLE
    wifi_get_config_from_nvdm(syscfg);
#else
    if (0 != wifi_get_default_config(syscfg)) {
        LOG_E(wifi, "wifi_get_default_config fail. initial aborted!");
        return -1;
    }
#endif
    /* both config and config_ext are NULL, it will not apply user config, */
    if( (NULL!=config) || (NULL!=config_ext) ){
        if( WIFI_SUCC != wifi_apply_user_config(syscfg, config, config_ext) ){
            LOG_E(wifi, "wifi_apply_user_config fail. initial aborted!");
            return -1;
        }
    }
    return 0;
}

static void wifi_build_config_from_sys_cfg(wifi_config_t *config, wifi_sys_cfg_t *syscfg )
{
    config->opmode = syscfg->opmode;

    /*sta config*/
    os_memcpy(config->sta_config.ssid, syscfg->sta_ssid, WIFI_MAX_LENGTH_OF_SSID);
    config->sta_config.ssid_length = syscfg->sta_ssid_len;
    os_memcpy(config->sta_config.password, syscfg->sta_wpa_psk, WIFI_LENGTH_PASSPHRASE);
    config->sta_config.password_length = syscfg->sta_wpa_psk_len;

    /*ap config*/
    os_memcpy(config->ap_config.ssid, syscfg->ap_ssid, WIFI_MAX_LENGTH_OF_SSID);
    config->ap_config.ssid_length = syscfg->ap_ssid_len;
    os_memcpy(config->ap_config.password, syscfg->ap_wpa_psk, WIFI_LENGTH_PASSPHRASE);
    config->ap_config.password_length = syscfg->ap_wpa_psk_len;
    config->ap_config.auth_mode = (wifi_auth_mode_t)syscfg->ap_auth_mode;
    config->ap_config.encrypt_type = (wifi_encrypt_type_t)syscfg->ap_encryp_type;
    config->ap_config.channel = syscfg->ap_channel;
    config->ap_config.bandwidth = syscfg->ap_bw;
    config->ap_config.bandwidth_ext = (syscfg->ap_ht_ext_ch == 1) ? WIFI_BANDWIDTH_EXT_40MHZ_UP : WIFI_BANDWIDTH_EXT_40MHZ_BELOW;
}

static void wifi_build_config_ext_from_sys_cfg(wifi_config_ext_t *config_ext, wifi_sys_cfg_t *syscfg)
{
    os_memcpy(config_ext->country_code, syscfg->country_code, 4);
    config_ext->country_code_present = 1;
    wifi_country_code_region_mapping(syscfg->country_code, &(syscfg->country_region), &(syscfg->country_region_a_band));

    /*sta config_ext*/
    config_ext->sta_wep_key_index = syscfg->sta_default_key_id;
    config_ext->sta_wep_key_index_present = 1;
    config_ext->sta_bandwidth = syscfg->sta_bw;
    config_ext->sta_bandwidth_present = 1;
    config_ext->sta_wireless_mode = (wifi_phy_mode_t)syscfg->sta_wireless_mode;
    config_ext->sta_wireless_mode_present = 1;
    config_ext->sta_listen_interval = syscfg->sta_listen_interval;
    config_ext->sta_listen_interval_present = 1;
    config_ext->sta_power_save_mode = (wifi_power_saving_mode_t)syscfg->sta_ps_mode;
    config_ext->sta_power_save_mode_present = 1;

    /*ap config_ext*/
    config_ext->ap_wep_key_index = syscfg->ap_default_key_id;
    config_ext->ap_wep_key_index_present = 1;
    config_ext->ap_hidden_ssid_enable = syscfg->ap_hide_ssid;
    config_ext->ap_hidden_ssid_enable_present = 1;
    config_ext->ap_wireless_mode = (wifi_phy_mode_t)syscfg->ap_wireless_mode;
    config_ext->ap_wireless_mode_present = 1;
    config_ext->ap_dtim_interval = syscfg->ap_dtim_period;
    config_ext->ap_dtim_interval_present = 1;
}

void wifi_sys_cfg_init(wifi_sys_cfg_t *config)
{
     return;
}


void wifi_init(wifi_config_t *config, wifi_config_ext_t *config_ext)
{
    wifi_sys_cfg_t syscfg = {0};
    wifi_config_t running_config = {0};
    wifi_config_ext_t running_config_ext = {0};
    wifi_config_t *pconfig = NULL;
    wifi_config_ext_t *pconfig_ext = NULL;

    os_memset(&syscfg, 0, sizeof(syscfg));

    pconfig = config;
    pconfig_ext = config_ext;
    if (0 != wifi_build_whole_config(&syscfg, pconfig, pconfig_ext)) {
        LOG_E(wifi, "wifi_build_whole_config fail. initial aborted!");
        return;
    }
    /*If user have setting params, use user setting, else use default.*/
    /*config is NULL, all config using default syscfg configurations. */
    /*config is not NULL, user not setting params items use default configurations.*/
    if (NULL == pconfig) {
        LOG_I(wifi, "config is null. Will use default config configuration");
    }else{
        os_memcpy(&running_config, config, sizeof(wifi_config_t));
    }
    wifi_build_config_from_sys_cfg(&running_config, &syscfg);
    pconfig = &running_config;

    if (NULL == pconfig_ext) {
        LOG_I(wifi, "config_ext is null. Will use default config_ext configuration");
    }else{
        os_memcpy(&running_config_ext, config_ext, sizeof(wifi_config_ext_t));
    }
    wifi_build_config_ext_from_sys_cfg(&running_config_ext, &syscfg);
    pconfig_ext = &running_config_ext;

#ifdef MTK_ANT_DIV_ENABLE
    /* After HW/patch download, to overwrite the patch configuration */
    //tx_switch_for_antenna,rx_switch_for_antenna is controled in NVDM and include in MTK_WIFI_PROFILE_ENABLE
    if(syscfg.antenna_diversity_enable != 0) {
        ant_diversity_init(syscfg.antenna_number);
        //printf("tx_switch_for_antenna=%d,rx_switch_for_antenna=%d",syscfg.tx_switch_for_antenna,syscfg.rx_switch_for_antenna);
    }
    //printf_ant_diversity(10);
#endif

    wifi_channel_list_init(&syscfg);
#if defined(MTK_WIFI_ROM_ENABLE) && !defined(MTK_WIFI_SLIM_ENABLE)
    connsys_set_wifi_profile(&syscfg);
    // connsys_init(); /* moved to  system_init() for early N9 FW download */
#else
    connsys_init(&syscfg);
#endif
    wifi_init_register_callback();

    wifi_scan_init(pconfig);
    inband_queue_init();
    NetTaskInit();

   /*Fix if SSID length = 0, switch to repeater mode from station mode, N9 will assert*/
    if(pconfig->ap_config.ssid_length == 0) {
        pconfig->ap_config.ssid_length = syscfg.ap_ssid_len;
        os_memcpy(pconfig->ap_config.ssid, syscfg.ap_ssid, WIFI_MAX_LENGTH_OF_SSID);
    } else if(pconfig->sta_config.ssid_length == 0) {
        pconfig->sta_config.ssid_length = syscfg.sta_ssid_len;
        os_memcpy(pconfig->sta_config.ssid, syscfg.sta_ssid, WIFI_MAX_LENGTH_OF_SSID);
    }
#ifdef MTK_MINISUPP_ENABLE
    wpa_supplicant_task_init(pconfig, pconfig_ext);
#endif
#ifdef MTK_CM4_WIFI_TASK_ENABLE
    wifi_firmware_init(pconfig, pconfig_ext,&syscfg);
    if (wifi_firmware_task_start(pconfig) != 0) {
        LOG_E(wifi, "failed to start firmware Wi-Fi task");
    }
#endif
}


static bool wifi_security_valid = false;
bool wifi_get_security_valid(void)
{
    return wifi_security_valid;
}

void wifi_set_security_valid(bool value)
{
    wifi_security_valid = value;
    return;
}

/***************** Just for internal use **********************/
#include "hal_sleep_manager.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED
#define WIFI_MAX_SLEEP_HANDLE  32   // This define value should equal to MAX_SLEEP_HANDLE in hal_sleep_driver.h
uint8_t locks[WIFI_MAX_SLEEP_HANDLE];
#endif
uint8_t wifi_set_sleep_handle(const char *handle_name)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return hal_sleep_manager_set_sleep_handle(handle_name);
#else
    return 0xff;
#endif
}

int32_t wifi_lock_sleep(uint8_t handle_index)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    locks[handle_index] = 1;
    return hal_sleep_manager_lock_sleep(handle_index);
#else
    return -1;
#endif
}

int32_t wifi_unlock_sleep(uint8_t handle_index)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    locks[handle_index] = 0;
    return hal_sleep_manager_unlock_sleep(handle_index);
#else
    return -1;
#endif
}

int32_t wifi_unlock_sleep_all(void)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    int i = 0;
    for (i = 0; i < WIFI_MAX_SLEEP_HANDLE; i++) {
        if (locks[i] == 1) {
            hal_sleep_manager_unlock_sleep(i);
        }
    }
    return 0;
#else
    return -1;
#endif
}

int32_t wifi_sleep_manager_get_lock_status(void)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return hal_sleep_manager_get_lock_status();
#else
    return -1;
#endif
}

int32_t wifi_release_sleep_handle(uint8_t handle)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    return hal_sleep_manager_release_sleep_handle(handle);
#else
    return -1;
#endif
}

int32_t wifi_get_pmk_by_ssid_psk_from_pmkinfo(uint8_t *pmk,uint8_t *ssid,uint8_t ssid_len, uint8_t *psk,uint8_t psk_len)
{
#ifdef MTK_NVDM_ENABLE
//if ((0 != ssid->auth_mode) && ((0 != ssid->encr_type) || (1 != ssid->encr_type))) {
    uint8_t pmk_info[WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE+32] = {0};
    uint8_t pmk_info_zero[WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE+32] = {0};
    uint32_t pmk_info_len = sizeof(pmk_info);
    nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "PMK_INFO",
                       (uint8_t *)pmk_info, &pmk_info_len);
    //hex_dump("pmk_info", pmk_info, 128);
    if ((0 == os_memcmp(pmk_info, ssid, ssid_len)) &&
        (0 == os_memcmp(pmk_info+WIFI_MAX_LENGTH_OF_SSID, psk, psk_len)) &&
        (0 == os_memcmp(pmk_info+ssid_len, pmk_info_zero, WIFI_MAX_LENGTH_OF_SSID - ssid_len)) &&
        (0 == os_memcmp(pmk_info+WIFI_MAX_LENGTH_OF_SSID+psk_len, pmk_info_zero, WIFI_LENGTH_PASSPHRASE - psk_len))) {
            os_memcpy(pmk, pmk_info+WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE, 32);
            return 0;
    } else
#endif
    {
        return -1;
    }
}

/***************** Just for internal use **********************/

