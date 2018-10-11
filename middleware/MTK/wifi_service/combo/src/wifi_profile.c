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
#include <stdlib.h>
#include "type_def.h"
#include "connsys_driver.h"
#include "connsys_util.h"
#include "wifi_scan.h"
#include "wifi_api.h"
#include "wifi_inband.h"
#include "os.h"
#include "os_util.h"
#include "nvdm.h"
#include "get_profile_string.h"
#include "timer.h"
#include "syslog.h"
#include "wifi_profile.h"
#include "cli.h"
#include "connsys_profile.h"
#include "misc.h"

#ifdef MTK_WIFI_ROM_ENABLE
#include "security_interface.h"
#endif

/**
 * @brief judge whether the opmode is valid
 */
bool wifi_is_opmode_valid(uint8_t mode)
{
    return (mode <= WIFI_MODE_P2P_ONLY);
}

/**
 * @brief judge whether the port is valid
 */
bool wifi_is_port_valid(uint8_t port)
{
    return (port <= WIFI_PORT_AP);
}

/**
 * @brief judge whether the band is valid
 */
bool wifi_is_band_valid(uint8_t band)
{
    return (band <= WIFI_BAND_5_G);
}

/**
 * @brief judge whether the bandwidth is valid
 */
bool wifi_is_bandwidth_valid(uint8_t bandwidth)
{
    return (bandwidth <= WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ);
}

/**
 * @brief judge whether the auth mode is valid
 */
bool wifi_is_auth_mode_valid(uint8_t auth_mode)
{
    return (auth_mode <= WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK);
}

/**
 * @brief judge whether the encrypt type is valid
 */
bool wifi_is_encrypt_type_valid(uint8_t encrypt_type)
{
#ifdef WAPI_SUPPORT
    return (encrypt_type <= WIFI_ENCRYPT_TYPE_ENCRYPT_SMS4_ENABLED);
#else
    return (encrypt_type <= WIFI_ENCRYPT_TYPE_GROUP_WEP104_ENABLED);
#endif
}

/**
 * @brief judge whether the power save mode is valid
 */
bool wifi_is_ps_mode_valid(uint8_t ps_mode)
{
    return (ps_mode <= 2);
}

#ifdef MTK_WIFI_PROFILE_ENABLE
int32_t wifi_profile_set_opmode(uint8_t mode)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_opmode_valid(mode)) {
        LOG_E(wifi, "mode is invalid: %d", mode);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", mode);

    if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "OpMode",
            NVDM_DATA_ITEM_TYPE_STRING,
            (uint8_t *)buf, os_strlen(buf))) {
        return WIFI_FAIL;
    }

    return WIFI_SUCC;
}

int32_t wifi_profile_get_opmode(uint8_t *mode)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == mode) {
        LOG_E(wifi, "mode is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_COMMON, "OpMode", (uint8_t *)buf, &len)) {
        return WIFI_FAIL;
    }

    *mode = atoi(buf);
    return WIFI_SUCC;
}


int32_t wifi_profile_set_channel(uint8_t port, uint8_t channel)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    sprintf(buf, "%d", channel);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "Channel",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else if (port == WIFI_PORT_STA) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "Channel",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    return WIFI_SUCC;
}

int32_t wifi_profile_get_channel(uint8_t port, uint8_t *channel)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == channel) {
        return WIFI_ERR_PARA_INVALID;
    }

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "Channel", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else if (port == WIFI_PORT_STA) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "Channel", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }

    *channel = atoi(buf);
    return WIFI_SUCC;
}

int32_t wifi_profile_set_bandwidth(uint8_t port, uint8_t bandwidth)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_bandwidth_valid(bandwidth)) {
        LOG_E(wifi, "bandwidth is invalid: %d", bandwidth);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", bandwidth);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "BW",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else if (port == WIFI_PORT_STA) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "BW",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    return WIFI_SUCC;
}


int32_t wifi_profile_get_bandwidth(uint8_t port, uint8_t *bandwidth)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == bandwidth) {
        LOG_E(wifi, "bandwidth is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "BW", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else if (port == WIFI_PORT_STA) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "BW", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }

    *bandwidth = atoi(buf);
    return WIFI_SUCC;
}

int32_t wifi_profile_get_mac_address(uint8_t port, uint8_t *address)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == address) {
        LOG_E(wifi, "address is null");
        return WIFI_ERR_PARA_INVALID;
    }

#ifdef MTK_WIFI_REPEATER_ENABLE
    uint8_t mode;
    if (wifi_profile_get_opmode(&mode) < 0) {
        return WIFI_FAIL;
    }
    if ((mode == WIFI_MODE_REPEATER) && (port == WIFI_PORT_AP)) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "MacAddr", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "MacAddr", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    }
#else
    /* Use STA MAC/IP as AP MAC/IP for the time being, due to N9 dual interface not ready yet */
    if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "MacAddr", (uint8_t *)buf, &len)) {
        return WIFI_FAIL;
    }
#endif

    wifi_conf_get_mac_from_str((char *)address, buf);
    return WIFI_SUCC;
}


int32_t wifi_profile_set_mac_address(uint8_t port, uint8_t *address)
{
    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (address == NULL) {
        LOG_E(wifi, "mac address is null.");
        return WIFI_ERR_PARA_INVALID;
    }

    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x",
            address[0], address[1], address[2], address[3], address[4], address[5]);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "MacAddr",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "MacAddr",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    }

    return WIFI_SUCC;
}


int32_t wifi_profile_set_ssid(uint8_t port, uint8_t *ssid , uint8_t ssid_length)
{
    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (ssid_length > WIFI_MAX_LENGTH_OF_SSID) {
        LOG_I(wifi, "incorrect length(=%d)", ssid_length);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == ssid) {
        LOG_E(wifi, "ssid is null.");
        return WIFI_ERR_PARA_INVALID;
    }

    char ssid_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char ssid_len_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    os_memcpy(ssid_buf, ssid, ssid_length);
    ssid_buf[ssid_length] = '\0';
    printf("----------------------------update ssid \n");

    /***add by Pengfei, optimize PMK calculate at boot up***/
   {
        uint8_t pmk_info[WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE+WIFI_LENGTH_PMK] = {0};
        uint8_t buf_passphrase[WIFI_LENGTH_PASSPHRASE] = {0};
        uint8_t passphrase_length = 0;
        uint8_t psk[WIFI_LENGTH_PMK];
        if (wifi_profile_get_wpa_psk_key(port, buf_passphrase, &passphrase_length) < 0) {
            return WIFI_FAIL;
        }

#ifdef MTK_MINISUPP_ENABLE
        if (__g_wpa_supplicant_api.cal_pmk) {
            if (__g_wpa_supplicant_api.cal_pmk(buf_passphrase, ssid, ssid_length, psk) < 0) {
                LOG_E(wifi, "callback cal_pmk is not ready");
                return WIFI_FAIL;
            }
        }
#endif /* MTK_MINISUPP_ENABLE */

#ifdef MTK_WIFI_ROM_ENABLE
        char pmk[40];

        CalculatePmk((char *)buf_passphrase, ssid, ssid_length,(unsigned char *) pmk);
        os_memcpy(psk,pmk,32);
#endif

        os_memcpy(pmk_info, ssid, ssid_length);
        os_memcpy(pmk_info+WIFI_MAX_LENGTH_OF_SSID, buf_passphrase, passphrase_length);
        os_memcpy(pmk_info+WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE, psk, WIFI_LENGTH_PMK);
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "PMK_INFO", NVDM_DATA_ITEM_TYPE_STRING,
                            (uint8_t *)pmk_info, WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE+WIFI_LENGTH_PMK)) {
            return WIFI_FAIL;
        }
    }

    sprintf(ssid_len_buf, "%d", ssid_length);
    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "Ssid",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_buf, os_strlen(ssid_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "SsidLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_len_buf, os_strlen(ssid_len_buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "Ssid",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_buf, os_strlen(ssid_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SsidLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)ssid_len_buf, os_strlen(ssid_len_buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}



int32_t wifi_profile_get_ssid(uint8_t port, uint8_t *ssid, uint8_t *ssid_length)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH];
    uint32_t len;

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == ssid_length) {
        LOG_E(wifi, "ssid_length is null.");
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == ssid) {
        LOG_E(wifi, "ssid is null.");
        return WIFI_ERR_PARA_INVALID;
    }

    if (port == WIFI_PORT_AP) {
        os_memset(buf, 0, WIFI_PROFILE_BUFFER_LENGTH);
        len = sizeof(buf);
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "Ssid", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
        strcpy((char *)ssid, buf);

        os_memset(buf, 0, WIFI_PROFILE_BUFFER_LENGTH);
        len = sizeof(buf);
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "SsidLen", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
        *ssid_length = atoi(buf);
        return WIFI_SUCC;
    } else {
        os_memset(buf, 0, WIFI_PROFILE_BUFFER_LENGTH);
        len = sizeof(buf);
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "Ssid", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
        strcpy((char *)ssid, buf);

        os_memset(buf, 0, WIFI_PROFILE_BUFFER_LENGTH);
        len = sizeof(buf);
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "SsidLen", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
        *ssid_length = atoi(buf);
        return WIFI_SUCC;
    }
}

int32_t wifi_profile_commit_setting(char *profile_section)
{
    return WIFI_SUCC;
}

int32_t wifi_profile_get_profile(uint8_t port, wifi_profile_t *profile)
{
    return WIFI_SUCC;
}

int32_t wifi_profile_set_wireless_mode(uint8_t port, wifi_phy_mode_t mode)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }

    int32_t support_5g = 0;
    support_5g = wifi_5g_support();
    if (((WIFI_PHY_11A == mode) || (WIFI_PHY_11ABG_MIXED == mode) ||(WIFI_PHY_11ABGN_MIXED == mode) || (WIFI_PHY_11AN_MIXED == mode))
         && (support_5g < 0)) {
         LOG_E(wifi, "Chip doesn't support 5G.");
         return WIFI_ERR_NOT_SUPPORT;
    }

    sprintf(buf, "%d", mode);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "WirelessMode",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WirelessMode",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}


int32_t wifi_profile_get_wireless_mode(uint8_t port, wifi_phy_mode_t *mode)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == mode) {
        LOG_E(wifi, "mode is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "WirelessMode", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "WirelessMode", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    }

    *mode = (wifi_phy_mode_t)atoi(buf);
    return WIFI_SUCC;
}

int32_t wifi_profile_set_security_mode(uint8_t port, wifi_auth_mode_t auth_mode, wifi_encrypt_type_t encrypt_type)
{
    char auth_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char encrypt_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (!wifi_is_auth_mode_valid(auth_mode)) {
        LOG_E(wifi, "auth_mode is invalid: %d", auth_mode);
        return WIFI_ERR_PARA_INVALID;
    }
    if (!wifi_is_encrypt_type_valid(encrypt_type)) {
        LOG_E(wifi, "encrypt_type is invalid: %d", encrypt_type);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(auth_buf, "%d", auth_mode);
    sprintf(encrypt_buf, "%d", encrypt_type);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "AuthMode",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)auth_buf, os_strlen(auth_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "EncrypType",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)encrypt_buf, os_strlen(encrypt_buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "AuthMode",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)auth_buf, os_strlen(auth_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "EncrypType",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)encrypt_buf, os_strlen(encrypt_buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}

int32_t wifi_profile_get_security_mode(uint8_t port, wifi_auth_mode_t *auth_mode, wifi_encrypt_type_t *encrypt_type)
{
    char auth_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char encypt_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t auth_buf_len = sizeof(auth_buf);
    uint32_t encypt_buf_len = sizeof(encypt_buf);

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == auth_mode || NULL == encrypt_type) {
        LOG_E(wifi, "null input pointer");
        return WIFI_ERR_PARA_INVALID;
    }

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "AuthMode", (uint8_t *)auth_buf, &auth_buf_len)) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "EncrypType", (uint8_t *)encypt_buf, &encypt_buf_len)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "AuthMode", (uint8_t *)auth_buf, &auth_buf_len)) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "EncrypType", (uint8_t *)encypt_buf, &encypt_buf_len)) {
            return WIFI_FAIL;
        }
    }

    *auth_mode = (wifi_auth_mode_t)atoi(auth_buf);
    *encrypt_type = (wifi_encrypt_type_t)atoi(encypt_buf);
    return WIFI_SUCC;
}


int32_t wifi_profile_set_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t passphrase_length)
{
    char pass_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char pass_len_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (passphrase == NULL) {
        LOG_E(wifi, "passphrase is null.");
        return WIFI_ERR_PARA_INVALID;
    }
    if ((passphrase_length < 8) || (passphrase_length > WIFI_LENGTH_PASSPHRASE)) {
        LOG_E(wifi, "incorrect length(=%d)", passphrase_length);
        return WIFI_ERR_PARA_INVALID;
    }
    if (passphrase_length == WIFI_LENGTH_PASSPHRASE) {
        for (uint8_t index = 0; index < WIFI_LENGTH_PASSPHRASE; index++) {
            if (!hex_isdigit(passphrase[index])) {
                LOG_E(wifi, "length(=%d) but the strings are not hex strings!", passphrase_length);
                return WIFI_ERR_PARA_INVALID;
            }
        }
    }

    sprintf(pass_len_buf, "%d", passphrase_length);
    os_memcpy(pass_buf, passphrase, passphrase_length);
    pass_buf[passphrase_length] = '\0';

    /***add by Pengfei, optimize PMK calculate at boot up***/
    if (WIFI_PORT_STA == port) {
        uint8_t pmk_info[WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE+WIFI_LENGTH_PMK] = {0};
        uint8_t ssid[WIFI_PROFILE_BUFFER_LENGTH] = {0};
        uint8_t ssid_length = 0;
        uint8_t psk[WIFI_LENGTH_PMK];
        if (wifi_profile_get_ssid(port, ssid, &ssid_length) < 0) {
            return WIFI_FAIL;
        }

#ifdef MTK_MINISUPP_ENABLE
        if (__g_wpa_supplicant_api.cal_pmk) {
            if (__g_wpa_supplicant_api.cal_pmk(passphrase, ssid, ssid_length, psk) < 0) {
                LOG_E(wifi, "passphrase length is 64.");
            }
        }
#endif /* MTK_MINISUPP_ENABLE */

#ifdef MTK_WIFI_ROM_ENABLE
        char pmk[40];
        CalculatePmk((char *)passphrase, ssid, ssid_length,(unsigned char *) pmk);
        os_memcpy(psk,pmk,32);
#endif

        os_memcpy(pmk_info, ssid, ssid_length);
        os_memcpy(pmk_info+WIFI_MAX_LENGTH_OF_SSID, passphrase, passphrase_length);
        os_memcpy(pmk_info+WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE, psk, WIFI_LENGTH_PMK);
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "PMK_INFO", NVDM_DATA_ITEM_TYPE_STRING,
                            (uint8_t *)pmk_info, WIFI_MAX_LENGTH_OF_SSID+WIFI_LENGTH_PASSPHRASE+WIFI_LENGTH_PMK)) {
            return WIFI_FAIL;
        }

    }

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "WpaPskLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_len_buf, os_strlen(pass_len_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "WpaPsk",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_buf, os_strlen(pass_buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPskLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_len_buf, os_strlen(pass_len_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPsk",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)pass_buf, os_strlen(pass_buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}


int32_t wifi_profile_get_wpa_psk_key(uint8_t port, uint8_t *passphrase, uint8_t *passphrase_length)
{
    char pass_len_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char pass_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t pass_len_buf_size = sizeof(pass_len_buf);
    uint32_t pass_buf_size = sizeof(pass_buf);

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == passphrase || NULL == passphrase_length) {
        LOG_E(wifi, "null input pointer");
        return WIFI_ERR_PARA_INVALID;
    }

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "WpaPskLen", (uint8_t *)pass_len_buf, &pass_len_buf_size)) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "WpaPsk", (uint8_t *)pass_buf, &pass_buf_size)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPskLen", (uint8_t *)pass_len_buf, &pass_len_buf_size)) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "WpaPsk", (uint8_t *)pass_buf, &pass_buf_size)) {
            return WIFI_FAIL;
        }
    }

    if ((strlen(pass_len_buf) == 0) || (strlen(pass_buf) == 0)) {
        *passphrase_length = 0;
        passphrase[0] = '\0';
        return WIFI_SUCC;
    }

    *passphrase_length = atoi(pass_len_buf);
    if (*passphrase_length > WIFI_LENGTH_PASSPHRASE) {
        LOG_E(wifi, "passphrase_length is too big: %d", *passphrase_length);
        return WIFI_FAIL;
    }

    os_memcpy(passphrase, pass_buf, *passphrase_length);
    return WIFI_SUCC;
}


int32_t wifi_profile_set_pmk(uint8_t port, uint8_t *pmk)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == pmk) {
        LOG_E(wifi, "pmk is null");
        return WIFI_ERR_PARA_INVALID;
    }

    char *buf_ptr = buf;
    for (int i = 0; i < WIFI_LENGTH_PMK; i++) {
        sprintf(buf_ptr, "%02x", pmk[i]);
        buf_ptr += 2;
    }
    *buf_ptr = '\0';

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "PMK",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "PMK",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}


int32_t wifi_profile_get_pmk(uint8_t port, uint8_t *pmk)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == pmk) {
        LOG_E(wifi, "pmk is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "PMK", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "PMK", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    }

    if (os_strlen(buf) == 0) {
        os_memset(pmk, 0, WIFI_LENGTH_PMK);
        return WIFI_SUCC;
    } else if (os_strlen(buf) != (WIFI_LENGTH_PMK * 2)) {
        LOG_E(wifi, "pmk integrity check fail");
        return WIFI_FAIL;
    }

    char segment[3];
    for (int i = 0; i < WIFI_LENGTH_PMK; i++) {
        os_memset(segment, 0, 3);
        os_memcpy(segment, (buf + (i * 2)), 2);
        segment[2] = '\0';
        pmk[i] = (int)strtol(segment, NULL, 16);
    }
    return WIFI_SUCC;
}

int32_t wifi_profile_set_wep_key(uint8_t port, wifi_wep_key_t *wep_keys)
{
    char key_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char key_len_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    char def_key_id_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    char temp_str[4];
    char temp_str1[WIFI_MAX_WEP_KEY_LENGTH + 2];
    char *key_len_ptr = key_len_buf;
    char *key_ptr = key_buf;

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == wep_keys) {
        LOG_E(wifi, "wep_keys is null");
        return WIFI_ERR_PARA_INVALID;
    }

    for (uint8_t index = 0; index < WIFI_NUMBER_WEP_KEYS; index++) {
        if (index < WIFI_NUMBER_WEP_KEYS - 1) {
            sprintf(temp_str, "%d,", wep_keys->wep_key_length[index]);
            os_memcpy(temp_str1, wep_keys->wep_key[index], wep_keys->wep_key_length[index]);
            temp_str1[wep_keys->wep_key_length[index]] = ',';
            temp_str1[wep_keys->wep_key_length[index] + 1] = '\0';
        } else {
            sprintf(temp_str, "%d", wep_keys->wep_key_length[index]);
            os_memcpy(temp_str1, wep_keys->wep_key[index], wep_keys->wep_key_length[index]);
            temp_str1[wep_keys->wep_key_length[index]] = '\0';
        }
        strcpy(key_len_ptr, temp_str);
        key_len_ptr += strlen(temp_str);
        strcpy(key_ptr, temp_str1);
        key_ptr += strlen(temp_str1);
        os_memset(temp_str, 0, 4);
        os_memset(temp_str1, 0, WIFI_MAX_WEP_KEY_LENGTH + 2);
    }

    sprintf(def_key_id_buf, "%d", wep_keys->wep_tx_key_index);
    LOG_I(wifi, "wifi_profile_set_wep_key: SharedKey =%s, SharedKeyLen=%s, DefaultKeyId=%s", key_buf, key_len_buf, temp_str);

    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "SharedKey",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)key_buf, os_strlen(key_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "SharedKeyLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)key_len_buf, os_strlen(key_len_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "DefaultKeyId",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)def_key_id_buf, os_strlen(def_key_id_buf))) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SharedKey",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)key_buf, os_strlen(key_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "SharedKeyLen",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)key_len_buf, os_strlen(key_len_buf))) {
            return WIFI_FAIL;
        }
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "DefaultKeyId",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)def_key_id_buf, os_strlen(def_key_id_buf))) {
            return WIFI_FAIL;
        }
    }
    return WIFI_SUCC;
}

int32_t wifi_profile_get_wep_key(uint8_t port, wifi_wep_key_t *wep_keys)
{
    char key_len_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t key_len_buf_size = sizeof(key_len_buf);

    uint8_t index = 0;
    char *ptr = NULL;

    if (!wifi_is_port_valid(port)) {
        LOG_E(wifi, "port is invalid: %d", port);
        return WIFI_ERR_PARA_INVALID;
    }
    if (NULL == wep_keys) {
        LOG_E(wifi, "wep_keys is null");
        return WIFI_ERR_PARA_INVALID;
    }

    /* WEP KEY LEN */
    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "SharedKeyLen", (uint8_t *)key_len_buf, &key_len_buf_size)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "SharedKeyLen", (uint8_t *)key_len_buf, &key_len_buf_size)) {
            return WIFI_FAIL;
        }
    }

    for (index = 0, ptr = rstrtok((char *)key_len_buf, ","); (ptr); ptr = rstrtok(NULL, ",")) {
        wep_keys->wep_key_length[index] = atoi(ptr);
        index++;
        if (index >= WIFI_NUMBER_WEP_KEYS) {
            break;
        }
    }

    /* WEP KEY */
    char key_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t key_buf_len = sizeof(key_buf);
    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "SharedKey", (uint8_t *)key_buf, &key_buf_len)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "SharedKey", (uint8_t *)key_buf, &key_buf_len)) {
            return WIFI_FAIL;
        }
    }

    for (index = 0, ptr = rstrtok(key_buf, ","); (ptr); ptr = rstrtok(NULL, ",")) {
        if (wep_keys->wep_key_length[index] == 5 ||
                wep_keys->wep_key_length[index] == 13) {
            os_memcpy(&wep_keys->wep_key[index], ptr, wep_keys->wep_key_length[index]);
        } else if (wep_keys->wep_key_length[index] == 10 ||
                   wep_keys->wep_key_length[index] == 26) {
            wep_keys->wep_key_length[index] /= 2;
            AtoH(ptr, (char *)&wep_keys->wep_key[index], (int)wep_keys->wep_key_length[index]);
        } else {
            //printf("WEP Key Length(=%d) is incorrect.\n", wep_keys->wep_key_length[index]);
        }
        index++;
        if (index >= WIFI_NUMBER_WEP_KEYS) {
            break;
        }
    }

    /* Default key ID */
    char def_key_id_buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t def_key_id_buf_size = sizeof(def_key_id_buf);
    if (port == WIFI_PORT_AP) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "DefaultKeyId", (uint8_t *)def_key_id_buf, &def_key_id_buf_size)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "DefaultKeyId", (uint8_t *)def_key_id_buf, &def_key_id_buf_size)) {
            return WIFI_FAIL;
        }
    }

    wep_keys->wep_tx_key_index = (uint8_t)atoi(def_key_id_buf);
    return WIFI_SUCC;
}


int32_t wifi_profile_get_country_region(uint8_t band, uint8_t *region)
{
    if (!wifi_is_band_valid(band)) {
        LOG_E(wifi, "band is invalid: %d", band);
        return WIFI_ERR_PARA_INVALID;
    }

    int32_t support_5g = 0;

    support_5g = wifi_5g_support();
    if ((band == WIFI_BAND_5_G) && (support_5g < 0)) {
        LOG_E(wifi, "Chip doesn't support 5G.");
        return WIFI_ERR_NOT_SUPPORT;
    }

    if (NULL == region) {
        LOG_E(wifi, "region is null");
        return WIFI_ERR_PARA_INVALID;
    }

    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (band == WIFI_BAND_2_4_G) {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_COMMON, "CountryRegion", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    } else {
        if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_COMMON, "CountryRegionABand", (uint8_t *)buf, &len)) {
            return WIFI_FAIL;
        }
    }

    *region = atoi(buf);
    return WIFI_SUCC;
}

int32_t wifi_profile_set_country_region(uint8_t band, uint8_t region)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    int32_t support_5g = 0;

    if (!wifi_is_band_valid(band)) {
        LOG_E(wifi, "band is invalid: %d", band);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", region);

    support_5g = wifi_5g_support();

    if (band == WIFI_BAND_2_4_G) {
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "CountryRegion",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
        if (wifi_get_channel_list(WIFI_BAND_2_4_G, region, buf) >= 0) {
            if (NVDM_STATUS_OK != nvdm_write_data_item("common", "BGChannelTable",
                    NVDM_DATA_ITEM_TYPE_STRING,
                    (uint8_t *)buf, os_strlen(buf))) {
                return WIFI_FAIL;
            }
        }
    } else {
        if (support_5g < 0) {
            LOG_E(wifi, "Chip doesn't support 5G.");
            return WIFI_ERR_NOT_SUPPORT;
		}
        if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "CountryRegionABand",
                NVDM_DATA_ITEM_TYPE_STRING,
                (uint8_t *)buf, os_strlen(buf))) {
            return WIFI_FAIL;
        }
        if (wifi_get_channel_list(WIFI_BAND_5_G, region, buf) >= 0) {
            if (NVDM_STATUS_OK != nvdm_write_data_item("common", "AChannelTable",
                    NVDM_DATA_ITEM_TYPE_STRING,
                    (uint8_t *)buf, os_strlen(buf))) {
                return WIFI_FAIL;
            }
        }
    }
    return WIFI_SUCC;
}


int32_t wifi_profile_get_dtim_interval(uint8_t *interval)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == interval) {
        LOG_E(wifi, "interval is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_AP, "DtimPeriod", (uint8_t *)buf, &len)) {
        return WIFI_FAIL;
    }

    *interval = atoi(buf);
    return WIFI_SUCC;
}


int32_t wifi_profile_set_dtim_interval(uint8_t interval)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (0 == interval) {
        LOG_E(wifi, "interval is invalid: %d", interval);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", interval);
    if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_AP, "DtimPeriod",
            NVDM_DATA_ITEM_TYPE_STRING,
            (const uint8_t *)buf, os_strlen(buf))) {
        return WIFI_FAIL;
    }

    return WIFI_SUCC;
}


int32_t wifi_profile_get_listen_interval(uint8_t *interval)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == interval) {
        LOG_E(wifi, "interval is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "ListenInterval", (uint8_t *)buf, &len)) {
        return WIFI_FAIL;
    }

    *interval = atoi(buf);
    return WIFI_SUCC;
}


int32_t wifi_profile_set_listen_interval(uint8_t interval)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (0 == interval) {
        LOG_E(wifi, "interval is invalid: %d", interval);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", interval);

    if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "ListenInterval",
            NVDM_DATA_ITEM_TYPE_STRING,
            (const uint8_t *)buf, os_strlen(buf))) {
        return WIFI_FAIL;
    }

    return WIFI_SUCC;
}

int32_t wifi_profile_get_power_save_mode(uint8_t *power_save_mode)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == power_save_mode) {
        LOG_E(wifi, "power_save_mode is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (NVDM_STATUS_OK != nvdm_read_data_item(WIFI_PROFILE_BUFFER_STA, "PSMode", (uint8_t *)buf, &len)) {
        return WIFI_FAIL;
    }

    *power_save_mode = atoi(buf);
    return WIFI_SUCC;
}

int32_t wifi_profile_set_power_save_mode(uint8_t power_save_mode)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (!wifi_is_ps_mode_valid(power_save_mode)) {
        LOG_E(wifi, "power_save_mode is invalid: %d", power_save_mode);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", power_save_mode);
    if (nvdm_write_data_item(WIFI_PROFILE_BUFFER_STA, "PSMode",
                             NVDM_DATA_ITEM_TYPE_STRING,
                             (const uint8_t *)buf, os_strlen(buf))) {
        return WIFI_FAIL;
    }

    return WIFI_SUCC;
}

int32_t wifi_profile_get_configfree(uint8_t *config_ready)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    uint32_t len = sizeof(buf);

    if (NULL == config_ready) {
        LOG_E(wifi, "config_ready is null");
        return WIFI_ERR_PARA_INVALID;
    }

    if (NVDM_STATUS_OK != nvdm_read_data_item("config_free", "ConfigFree_Ready", (uint8_t *)buf, &len)) {
        return WIFI_FAIL;
    }

    *config_ready = atoi(buf);
    return WIFI_SUCC;
}

int32_t wifi_profile_set_configfree(uint8_t config_ready)
{
    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    if (0 != config_ready && 1 != config_ready) {
        LOG_E(wifi, "config_ready is invalid: %d", config_ready);
        return WIFI_ERR_PARA_INVALID;
    }

    sprintf(buf, "%d", WIFI_MODE_REPEATER);
    LOG_I(wifi, "wifi_profile_set_opmode: opmode=%s", buf);

    if (NVDM_STATUS_OK != nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "OpMode",
            NVDM_DATA_ITEM_TYPE_STRING,
            (uint8_t *)buf, os_strlen(buf))) {
        return WIFI_FAIL;
    }

    os_memset(buf, 0x0, WIFI_PROFILE_BUFFER_LENGTH);

    sprintf(buf, "%d", config_ready);
    LOG_I(wifi, "ConfigFree ready: %s", buf);

    if (NVDM_STATUS_OK != nvdm_write_data_item("config_free", "ConfigFree_Ready",
            NVDM_DATA_ITEM_TYPE_STRING,
            (uint8_t *)buf, os_strlen(buf))) {
        return WIFI_FAIL;
    }

    return WIFI_SUCC;
}
#endif //MTK_WIFI_PROFILE_ENABLE

#ifdef MTK_WIFI_PROFILE_ENABLE
static uint8_t wifi_profile_set_opmode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t mode = atoi(param[0]);

    status = wifi_profile_set_opmode(mode);
    status = wifi_profile_commit_setting(WIFI_PROFILE_BUFFER_COMMON);

    printf("save opmode=%d done.\n", mode);

    return status;
}

/**
 * @brief Store channel to the profile in the Flash memory.
 * wifi profile set ch <port> <ch>
 * @param [IN]port
 * @param 0 STA / AP Client
 * @param 1 AP
 * @param [IN]channel    1~14 are supported for 2.4G only product.
 *
 * @return  >=0 means success, <0 means fail
 */
static uint8_t wifi_profile_set_channel_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t ch = atoi(param[1]);
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    if (ch < 1) {
        printf("Invalid channel number\n");
        return 1;
    }

    status = wifi_profile_set_channel((uint8_t)port, ch);

    status = wifi_profile_commit_setting(section);

    printf("[%s] save ch =%d done.\n", section, ch);

    return status;
}

/**
 * @brief Store bandwidth to the profile in the Flash memory.
 * wifi profile set bw <port> <bw>
 * @param [IN]port
 * @param 0 STA / AP Client
 * @param 1 AP
 * @param [IN]bandwidth Bandwidth IOT_CMD_CBW_20MHZ, IOT_CMD_CBW_40MHZ,
 *                  IOT_CMD_CBW_2040MHZ are supported.
 * @return  >=0 means success, <0 means fail
 */
static uint8_t wifi_profile_set_bandwidth_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t bw = atoi(param[1]);
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_set_bandwidth((uint8_t)port, bw);
    status = wifi_profile_commit_setting(section);

    printf("[%s] save bw =%d done.\n", section, bw);

    return status;
}

/**
* @brief Store SSID to the profile in the Flash memory.
* wifi profile set ssid <port> <ssid>
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [IN]ssid SSID
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_ssid_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    char *ssid = param[1];
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_set_ssid((uint8_t)port, (uint8_t *)ssid, strlen(ssid));
    status = wifi_profile_commit_setting(section);

    printf("[%s] save ssid = %s done.\n", section, ssid);

    return status;
}
#if 0
/**
* @brief Store WiFi BSSID to the profile in the Flash memory.
* wifi profile set bssid <port> <bssid>
* ex: wifi profile set bssid 0 00:0c:43:76:87:02
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [IN]bssid BSSID
*
* @return  >=0 means success, <0 means fail
*/

static uint8_t wifi_profile_set_bssid_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t port = atoi(param[0]);
    uint8_t bssid[6];

    __wifi_conf_get_mac_from_str((char *)bssid, param[1]);

    status = wifi_profile_set_bssid(port, bssid);

    printf("save bssid = %02x:%02x:%02x:%02x:%02x:%02x done.\n",
           bssid[0],
           bssid[1],
           bssid[2],
           bssid[3],
           bssid[4],
           bssid[5]);

    return status;
}
#endif

/**
* @brief Store WiFi Wireless Mode to the profile in the Flash memory.
* wifi profile set wirelessmode <port> <mode>
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [IN]mode
* @param 1 legacy 11B only
* @param 2 legacy 11A only
* @param 3 legacy 11A/B/G mixed
* @param 4 legacy 11G only
* @param 5 11ABGN mixed
* @param 6 11N only in 2.4G
* @param 7 11GN mixed
* @param 8 11AN mixed
* @param 9 11BGN mixed
* @param 10 11AGN mixed
*
* @return  >=0 means success, <0 means fail
*/

static uint8_t wifi_profile_set_wireless_mode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    wifi_phy_mode_t mode = (wifi_phy_mode_t)atoi(param[1]);
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_set_wireless_mode((uint8_t)port, mode);
    status = wifi_profile_commit_setting(section);

    printf("[%s] save wireless mode =%d done.\n", section, mode);

    return status;
}
static uint8_t wifi_profile_set_country_region_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t band = atoi(param[0]);
    uint8_t region = atoi(param[1]);

    status = wifi_profile_set_country_region(band, region);
    status = wifi_profile_commit_setting(WIFI_PROFILE_BUFFER_COMMON);

    printf("save country band(%d) region =%d done.\n", band, region);

    return status;
}
static uint8_t wifi_profile_set_dtim_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dtim = atoi(param[0]);

    status = wifi_profile_set_dtim_interval(dtim);
    status = wifi_profile_commit_setting(WIFI_PROFILE_BUFFER_AP);

    printf("save dtim interval =%d done.\n", dtim);

    return status;
}
static uint8_t wifi_profile_set_listen_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t interval = atoi(param[0]);

    status = wifi_profile_set_listen_interval(interval);
    status = wifi_profile_commit_setting(WIFI_PROFILE_BUFFER_STA);

    printf("save listen interval =%d done.\n", interval);

    return status;
}
static uint8_t wifi_profile_set_mac_address_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t mac[6] = {0};

#ifdef MTK_WIFI_REPEATER_ENABLE
    uint8_t mode;
    char *section = WIFI_PROFILE_BUFFER_STA;

    status = wifi_profile_get_opmode(&mode);
    if (mode == WIFI_MODE_REPEATER) {
        strcpy(section,(port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP));
    } else {
        strcpy(section, WIFI_PROFILE_BUFFER_STA);
    }
    //char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);
#else
    /* Use STA MAC/IP as AP MAC/IP for the time being, due to N9 dual interface not ready yet */
    char *section = WIFI_PROFILE_BUFFER_STA;
#endif /* MTK_WIFI_REPEATER_ENABLE */

    if (port < 0) {
        return 1;
    }

    wifi_conf_get_mac_from_str((char *)mac, param[1]);

    status = wifi_profile_set_mac_address((uint8_t)port, mac);
    status = wifi_profile_commit_setting(section);

    printf("[%s] save mac address = %02x:%02x:%02x:%02x:%02x:%02x done.\n",
           section,
           mac[0],
           mac[1],
           mac[2],
           mac[3],
           mac[4],
           mac[5]);
    return status;
}

static uint8_t wifi_profile_set_security_mode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    wifi_auth_mode_t auth = (wifi_auth_mode_t)atoi(param[1]);
    wifi_encrypt_type_t encryp = (wifi_encrypt_type_t)atoi(param[2]);
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_set_security_mode((uint8_t)port, auth, encryp);
    status = wifi_profile_commit_setting(section);

    printf("[%s] save auth mode=%d, encrypt type=%d done.\n", section, auth, encryp);

    return status;
}

static uint8_t wifi_profile_set_psk_ex(uint8_t len, char *param[])
{
    int8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    char *password = param[1];
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_set_wpa_psk_key((uint8_t)port, (uint8_t *)password, strlen(password));
    if (status < 0) {
        printf("[%s] ERROR! Fail to save password =%s len = %d.\n", section, password, strlen(password));
        return 1;
    }
    status = wifi_profile_commit_setting(section);

    printf("[%s] save password =%s len = %d done.\n", section, password, strlen(password));

    return 0;
}

/**
* @brief Example of Store PMK for the specified STA/AP port into the Flash memory
* wifi profile set pmk <port> <PMK>
* @param [IN]port
*       0 STA / AP Client
*       1 AP
* @param  [IN] PMK (in hex)
*       00, 05, 30, ......(size 32)
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
static uint8_t wifi_profile_set_pmk_ex(uint8_t len, char *param[])
{
    int8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);
    uint8_t hex[32] = {0};

    if (port < 0) {
        return 1;
    }

    os_memset(hex, 0, sizeof(hex));
    convert_string_to_hex_array(param[1], hex);
    status = wifi_profile_set_pmk((uint8_t)port, hex);
    if (status < 0) {
        printf("[%s] ERROR! Fail to save pmk.\n", section);
        return 1;
    }
    status = wifi_profile_commit_setting(section);

    printf("[%s] wifi_profile_set_pmk_ex: status:%d\n",
           section, status);

    return 0;

}

/**
* @brief Example of Get profile stored in NVRAM
*
* @param [IN]port
* @param 0 AP
* @param 1 AP Client
* @param 2 STA
* @param [OUT]profile WiFi Profile
*
* @return  >=0 means success, <0 means fail
*/
int32_t wifi_config_init(wifi_sys_cfg_t *wifi_config);
uint8_t wifi_profile_get_profile_ex(uint8_t len, char *param[])
{
    wifi_sys_cfg_t wifi_profile_struct = {0};
    wifi_sys_cfg_t *wifi_profile = &wifi_profile_struct;
    uint8_t status = wifi_config_init(&wifi_profile_struct);

    // COMMON
    printf("[COMMON]\n");
    printf("\tOpMode = %d\n", wifi_profile->opmode);
    printf("\tCountryRegion = %d\n", wifi_profile->country_region);
    printf("\tCountryRegionABand = %d\n", wifi_profile->country_region_a_band);
    printf("\tCountryCode = %s\n", wifi_profile->country_code);
    printf("\tRadioOff = %d\n", wifi_profile->radio_off);
    printf("\tRTSThreshold = %d\n", wifi_profile->rts_threshold);
    printf("\tFragThreshold = %d\n", wifi_profile->frag_threshold);
    printf("\tDbgLevel = %d\n", wifi_profile->dbg_level);
    printf("\tForwardingZeroCopy = %d\n", wifi_profile->forwarding_zero_copy);
    // STA
    printf("[STA]\n");
    printf("\tLocalAdminMAC = %d\n", wifi_profile->sta_local_admin_mac);
    printf("\tIpAddr = %d.%d.%d.%d\n",
           wifi_profile->sta_ip_addr[0],
           wifi_profile->sta_ip_addr[1],
           wifi_profile->sta_ip_addr[2],
           wifi_profile->sta_ip_addr[3]);
    printf("\tMacAddr = %02x:%02x:%02x:%02x:%02x:%02x\n",
           wifi_profile->sta_mac_addr[0],
           wifi_profile->sta_mac_addr[1],
           wifi_profile->sta_mac_addr[2],
           wifi_profile->sta_mac_addr[3],
           wifi_profile->sta_mac_addr[4],
           wifi_profile->sta_mac_addr[5]);
    printf("\tSsidLen = %d\n", wifi_profile->sta_ssid_len);
    printf("\tSsid = %s\n", wifi_profile->sta_ssid);
    printf("\tBssType = %d\n", wifi_profile->sta_bss_type);
    printf("\tChannel = %d\n", wifi_profile->sta_channel);
    printf("\tBW = %d\n", wifi_profile->sta_bw);
    printf("\tWirelessMode = %d\n", wifi_profile->sta_wireless_mode);
    printf("\tBADecline = %d\n", wifi_profile->sta_ba_decline);
    printf("\tAutoBA = %d\n", wifi_profile->sta_auto_ba);
    printf("\tHT_MCS = %d\n", wifi_profile->sta_ht_mcs);
    printf("\tHT_BAWinSize = %d\n", wifi_profile->sta_ht_ba_win_size);
    printf("\tHT_GI = %d\n", wifi_profile->sta_ht_gi);
    printf("\tHT_PROTECT = %d\n", wifi_profile->sta_ht_protect);
    printf("\tHT_EXTCHA = %d\n", wifi_profile->sta_ht_ext_ch);
    printf("\tWmmCapable = %d\n", wifi_profile->sta_wmm_capable);
    printf("\tListenInterval = %d\n", wifi_profile->sta_listen_interval);
    printf("\tAuthMode = %d\n", wifi_profile->sta_auth_mode);
    printf("\tEncrypType = %d\n", wifi_profile->sta_encryp_type);
    printf("\tWpaPskLen = %d\n", wifi_profile->sta_wpa_psk_len);
    printf("\tPairCipher = %d\n", wifi_profile->sta_pair_cipher);
    printf("\tGroupCipher = %d\n", wifi_profile->sta_group_cipher);
    printf("\tDefaultKeyId = %d\n", wifi_profile->sta_default_key_id);
    printf("\tPSMode = %d\n", wifi_profile->sta_ps_mode);
    printf("\tKeepAlivePeriod = %d\n", wifi_profile->sta_keep_alive_period);

    hex_dump("WpaPsk", wifi_profile->sta_wpa_psk, sizeof(wifi_profile->sta_wpa_psk));
    hex_dump("PMK", wifi_profile->sta_pmk, sizeof(wifi_profile->sta_pmk));

    // AP
    printf("[AP]\n");
    printf("\tLocalAdminMAC = %d\n", wifi_profile->ap_local_admin_mac);
    printf("\tIpAddr = %d.%d.%d.%d\n",
           wifi_profile->ap_ip_addr[0],
           wifi_profile->ap_ip_addr[1],
           wifi_profile->ap_ip_addr[2],
           wifi_profile->ap_ip_addr[3]);
    printf("\tMacAddr = %02x:%02x:%02x:%02x:%02x:%02x\n",
           wifi_profile->ap_mac_addr[0],
           wifi_profile->ap_mac_addr[1],
           wifi_profile->ap_mac_addr[2],
           wifi_profile->ap_mac_addr[3],
           wifi_profile->ap_mac_addr[4],
           wifi_profile->ap_mac_addr[5]);
    printf("\tSsidLen = %d\n", wifi_profile->ap_ssid_len);
    printf("\tSsid = %s\n", wifi_profile->ap_ssid);
    printf("\tChannel = %d\n", wifi_profile->ap_channel);
    printf("\tBW = %d\n", wifi_profile->ap_bw);
    printf("\tWirelessMode = %d\n", wifi_profile->ap_wireless_mode);
    printf("\tAutoBA = %d\n", wifi_profile->ap_auto_ba);
    printf("\tHT_MCS = %d\n", wifi_profile->ap_ht_mcs);
    printf("\tHT_BAWinSize = %d\n", wifi_profile->ap_ht_ba_win_size);
    printf("\tHT_GI = %d\n", wifi_profile->ap_ht_gi);
    printf("\tHT_PROTECT = %d\n", wifi_profile->ap_ht_protect);
    printf("\tHT_EXTCHA = %d\n", wifi_profile->ap_ht_ext_ch);
    printf("\tWmmCapable = %d\n", wifi_profile->ap_wmm_capable);
    printf("\tDtimPeriod = %d\n", wifi_profile->ap_dtim_period);
    printf("\tHideSSID = %d\n", wifi_profile->ap_hide_ssid);
    printf("\tAutoChannelSelect = %d\n", wifi_profile->ap_auto_channel_select);
    printf("\tAuthMode = %d\n", wifi_profile->ap_auth_mode);
    printf("\tEncrypType = %d\n", wifi_profile->ap_encryp_type);
    printf("\tWpaPskLen = %d\n", wifi_profile->ap_wpa_psk_len);
    printf("\tPairCipher = %d\n", wifi_profile->ap_pair_cipher);
    printf("\tGroupCipher = %d\n", wifi_profile->ap_group_cipher);
    printf("\tDefaultKeyId = %d\n", wifi_profile->ap_default_key_id);

    hex_dump("WpaPsk", wifi_profile->ap_wpa_psk, sizeof(wifi_profile->ap_wpa_psk));
    hex_dump("PMK", wifi_profile->ap_pmk, sizeof(wifi_profile->ap_pmk));

#if 0 // not ready
    // scan channel table and regulatory table
    printf("[COMMON-misc.]\n");

    printf("\tBGChannelTable:\n");
    for (i = 0; i < 10; i++)
        printf("\t\t{%d,%d,%d}\n",
               wifi_profile->bg_band_triple[i].first_channel,
               wifi_profile->bg_band_triple[i].num_of_ch,
               wifi_profile->bg_band_triple[i].channel_prop);

    printf("\tAChannelTable:\n");
    for (i = 0; i < 10; i++)
        printf("\t\t{%d,%d,%d}\n",
               wifi_profile->a_band_triple[i].first_channel,
               wifi_profile->a_band_triple[i].num_of_ch,
               wifi_profile->a_band_triple[i].channel_prop);
#endif
    return status;
}

/**
* @brief Example of Get PMK for the specified STA/AP port from NVRAM
* wifi profile get pmk <port>
* @param [IN]port
*       0 STA / AP Client
*       1 AP
* @return >=0 means success, <0 means fail
* @note Default to OPEN
*/
static uint8_t wifi_profile_get_pmk_ex(uint8_t len, char *param[])
{
    int i;
    uint8_t status = 0;
    uint8_t hex[32] = {0};
    int32_t port = port_sanity_check(param[0]);
    if (port < 0) {
        return 1;
    }

    os_memset(hex, 0, sizeof(hex));
    status = wifi_profile_get_pmk(port, hex);
    printf("wifi_profile_get_pmk_ex: port:%ld, status:%d, key dump:\n",
           port, status);
    for (i = 0; i < sizeof(hex); i++) {
        if (i % 16 == 0) {
            printf("\n\t");
        }
        printf("%02x ", (unsigned int)hex[i]);
    }

    return status;

}



/**
* @brief Store WiFi WEP Keys to the profile in NVRAM.
* wifi profile set wep <port> <key id> <key>
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [IN]wifi_wep_key_t
*
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_wep_key_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t index = 0;
    wifi_wep_key_t wep_key;
    char *ptr = NULL;
    int32_t port = port_sanity_check(param[0]);
    char *keys = param[2];
    char *section = (port == WIFI_PORT_STA ? WIFI_PROFILE_BUFFER_STA : WIFI_PROFILE_BUFFER_AP);

    if (port < 0) {
        return 1;
    }

    os_memset(&wep_key, 0, sizeof(wep_key));
    wep_key.wep_tx_key_index = atoi(param[1]);

    index = 0;
    for (ptr = rstrtok((char *)keys, ","); (ptr); ptr = rstrtok(NULL, ",")) {
        printf("[%d] key=%s\n", index, ptr);
        if (strlen(ptr) == 1 || strlen(ptr) == 5 || strlen(ptr) == 13 || strlen(ptr) == 10 || strlen(ptr) == 26) {
            os_memcpy(wep_key.wep_key[index], ptr, strlen(ptr));
            wep_key.wep_key_length[index] = strlen(ptr);
        } else {
            printf("invalid length of value.\n");
        }
        index++;
        if (index >= WIFI_NUMBER_WEP_KEYS) {
            break;
        }
    }


    status = wifi_profile_set_wep_key((uint8_t)port, &wep_key);
    status = wifi_profile_commit_setting(section);

    printf("[%s] save wep key =(%s, %s, %s, %s) key id=%d, len=(%d, %d, %d, %d) done.\n",
           section,
           wep_key.wep_key[0],
           wep_key.wep_key[1],
           wep_key.wep_key[2],
           wep_key.wep_key[3],
           wep_key.wep_tx_key_index,
           wep_key.wep_key_length[0],
           wep_key.wep_key_length[1],
           wep_key.wep_key_length[2],
           wep_key.wep_key_length[3]);

    return status;
}
#if 0
/**
 * @brief Configure packet format wanted to be received
 * wifi profile set rxfilter <flag>
 * @param flag [IN] flag defined in iot_rx_filter_t.
 * @return >=0 means success, <0 means fail
 * @note Default value will be WIFI_DEFAULT_IOT_RX_FILTER
 */
static uint8_t wifi_profile_set_rx_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint32_t flag = atoi(param[0]);

    status = wifi_profile_set_rx_filter(flag);

    printf("save RX filter = 0x%x done.\n", (unsigned int)flag);

    return status;
}


static uint8_t wifi_profile_set_smart_connection_filter_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t flag = atoi(param[0]);

    status = wifi_profile_set_smart_connection_filter(flag);

    printf("save MTK RX filter =0x%x done.\n", flag);

    return status;
}
#endif
static uint8_t wifi_profile_get_opmode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t mode;

    status = wifi_profile_get_opmode(&mode);

    printf("fetch opmode=%d done.\n", mode);

    return status;
}

/**
 * @brief Get channel from the profile in NVRAM.
 * wifi profile get ch <port>
 * @param [IN]port
 * @param 0 STA / AP Client
 * @param 1 AP
 * @param [OUT]channel    1~14 are supported for 2.4G only product.
 *
 * @return  >=0 means success, <0 means fail
 */

static uint8_t wifi_profile_get_channel_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t ch;

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_get_channel((uint8_t)port, &ch);

    printf("fetch ch =%d done.\n", ch);

    return status;
}

/**
 * @brief  Get bandwidth from the profile in NVRAM.
 * wifi profile get bw <port>
 * @param [IN]port
 * @param 0 STA / AP Client
 * @param 1 AP
 * @param [OUT]bandwidth The wirelss bandwidth.
 *                       IOT_CMD_CBW_20MHZ,
 *                       IOT_CMD_CBW_40MHZ, and
 *                       IOT_CMD_CBW_2040MHZ are supported.
 *
 * @return  >=0 means success, <0 means fail
 *
 * @note    Default value is HT_20
 */

static uint8_t wifi_profile_get_bandwidth_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t bw;

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_get_bandwidth((uint8_t)port, &bw);

    printf("fetch bw =%d done.\n", bw);

    return status;
}

/**
* @brief Get SSID from the profile in NVRAM.
* wifi profile get ssid <port>
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [OUT]ssid SSID
* @param [OUT]ssid_length Length of SSID
*
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_ssid_ex(uint8_t length, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t ssid[32] = {0};
    uint8_t len;

    if (port < 0) {
        return 1;
    }

    os_memset(ssid, 0x0, sizeof(ssid));
    status = wifi_profile_get_ssid((uint8_t)port, ssid, &len);

    printf("fetch ssid = %s, len=%d done.\n", ssid, len);

    return status;
}
#if 0
/**
* @brief Store WiFi BSSID to the profile in NVRAM.
* wifi profile get bssid <port>
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [IN]bssid BSSID
*
* @return  >=0 means success, <0 means fail
*/

static uint8_t wifi_profile_get_bssid_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t port = atoi(param[0]);
    uint8_t bssid[6];

    status = wifi_profile_set_bssid(port, bssid);

    printf("fetch bssid = %02x:%02x:%02x:%02x:%02x:%02x done.\n",
           bssid[0],
           bssid[1],
           bssid[2],
           bssid[3],
           bssid[4],
           bssid[5]);

    return status;
}
#endif

/**
* @brief Get WiFi Wireless Mode from the profile in NVRAM.
* wifi profile get wirelessmode <port>
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [OUT]mode
* @param 1 legacy 11B only
* @param 2 legacy 11A only
* @param 3 legacy 11A/B/G mixed
* @param 4 legacy 11G only
* @param 5 11ABGN mixed
* @param 6 11N only in 2.4G
* @param 7 11GN mixed
* @param 8 11AN mixed
* @param 9 11BGN mixed
* @param 10 11AGN mixed
*/

static uint8_t wifi_profile_get_wireless_mode_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t mode;

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_get_wireless_mode((uint8_t)port, (wifi_phy_mode_t *)&mode);

    printf("fetch wireless mode =%d done.\n", mode);

    return status;
}


static uint8_t wifi_profile_get_country_region_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t band = atoi(param[0]);
    uint8_t region;

    status = wifi_profile_get_country_region(band, &region);

    printf("fetch band(%d) region =%d done.\n", band, region);

    return status;
}

static uint8_t wifi_profile_get_dtim_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dtim;

    status = wifi_profile_get_dtim_interval(&dtim);

    printf("fetch dtim = %d done.\n", dtim);

    return status;
}
static uint8_t wifi_profile_get_listen_interval_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t interval;

    status = wifi_profile_get_listen_interval(&interval);

    printf("fetch listen interval = %d done.\n", interval);

    return status;
}
static uint8_t wifi_profile_get_mac_address_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t mac[6] = {0};

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_get_mac_address((uint8_t)port, mac);

    printf("fetch mac address = %02x:%02x:%02x:%02x:%02x:%02x done.\n",
           mac[0],
           mac[1],
           mac[2],
           mac[3],
           mac[4],
           mac[5]);
    return status;

}
static uint8_t wifi_profile_get_security_mode_ex(uint8_t length, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t auth;
    uint8_t encryp;

    if (port < 0) {
        return 1;
    }

    status = wifi_profile_get_security_mode((uint8_t)port, (wifi_auth_mode_t *)&auth, (wifi_encrypt_type_t *)&encryp);

    printf("fetch auth mode=%d, encrypt type=%d done.\n", auth, encryp);

    return status;
}

static uint8_t wifi_profile_get_psk_ex(uint8_t length, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    uint8_t password[65] = {0};
    uint8_t len;

    if (port < 0) {
        return 1;
    }

    os_memset(password, 0, sizeof(password));
    status = wifi_profile_get_wpa_psk_key((uint8_t)port, password, &len);

    printf("fetch password =%s len=%d done.\n", password, len);

    return status;
}

/**
* @brief Get WiFi WEP Keys from the profile in NVRAM.
* wifi profile get wep <port>
* @param [IN]port
* @param 0 STA / AP Client
* @param 1 AP
* @param [OUT]wifi_wep_key_t
*
* @return >=0 means success, <0 means fail
*/

static uint8_t wifi_profile_get_wep_key_ex(uint8_t len, char *param[])
{
    uint8_t status = 0;
    int32_t port = port_sanity_check(param[0]);
    wifi_wep_key_t wep_key;
    int index, i;

    if (port < 0) {
        return 1;
    }

    os_memset(&wep_key, 0, sizeof(wep_key));
    status = wifi_profile_get_wep_key((uint8_t)port, &wep_key);

    wep_key.wep_key[0][wep_key.wep_key_length[0]] = '\0';
    wep_key.wep_key[1][wep_key.wep_key_length[1]] = '\0';
    wep_key.wep_key[2][wep_key.wep_key_length[2]] = '\0';
    wep_key.wep_key[3][wep_key.wep_key_length[3]] = '\0';

    printf("fetch wep key id =%d, len = (%d, %d, %d, %d) done.\n",
           wep_key.wep_tx_key_index,
           wep_key.wep_key_length[0],
           wep_key.wep_key_length[1],
           wep_key.wep_key_length[2],
           wep_key.wep_key_length[3]);
    for (index = 0; index < WIFI_NUMBER_WEP_KEYS; index++) {
        printf("[%d]: ", index);
        for (i = 0; i < wep_key.wep_key_length[index]; i++) {
            printf("%02x", wep_key.wep_key[index][i]);
        }
        printf("\n");
    }
    return status;
}

/**
* @brief Save N9 Debug Level into NVRAM
* wifi profile set n9dbg <dbg_level>
* @param [IN]dbg_level
*   0: None
*   1: ERROR
*   2. WARNING
*   3. TRACE
*   4. INFO
*   5. LAUD
*
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_set_n9_dbg_level(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dbg_level = atoi(param[0]);

    char buf[WIFI_PROFILE_BUFFER_LENGTH] = {0};

    sprintf(buf, "%d", dbg_level);
    if (nvdm_write_data_item(WIFI_PROFILE_BUFFER_COMMON, "DbgLevel",NVDM_DATA_ITEM_TYPE_STRING, (const uint8_t *)buf, os_strlen(buf)) != NVDM_STATUS_OK) {
        printf("write NVDM DbgLevel fail\n");
        status = 1;
    }


    printf("wifi_profile_set_n9_dbg_level, level = %d, status:%d\n", dbg_level, status);

    return status;
}


/**
* @brief Get N9 Debug Level from NVRAM
* wifi profile get n9dbg
* @param [OUT]dbg_level
*   0: None
*   1: ERROR
*   2. WARNING
*   3. TRACE
*   4. INFO
*   5. LAUD
*
* @return  >=0 means success, <0 means fail
*/
static uint8_t wifi_profile_get_n9_dbg_level(uint8_t len, char *param[])
{
    uint8_t status = 0;
    uint8_t dbg_level = 0;

    // init wifi profile
    char buff[WIFI_PROFILE_BUFFER_LENGTH] = {0};
    int buff_sz = sizeof(buff);
    // common
    nvdm_read_data_item("common", "DbgLevel", (uint8_t *)buff, (uint32_t *)&buff_sz);
    dbg_level = atoi(buff);

    printf("wifi_profile_get_n9_dbg_level, level = %d, status:%d\n", dbg_level, status);

    return status;
}

void user_data_item_reset_to_default(char *group_name);
uint8_t wifi_profile_reset_ex(uint8_t len, char *param[])
{
    user_data_item_reset_to_default(NULL);
    return 0;
}

cmd_t   wifi_profile_set_cli[] = {
    { "opmode",         "STA/AP/Dual",      wifi_profile_set_opmode_ex},
    { "ch",             "channel",          wifi_profile_set_channel_ex},
    { "bw",             "bandwidth",        wifi_profile_set_bandwidth_ex},
    { "ssid",           "SSID",             wifi_profile_set_ssid_ex      },
//  { "bssid",          "BSSID",            wifi_profile_set_bssid_ex     },
    { "wirelessmode",   "wireless mode",    wifi_profile_set_wireless_mode_ex},
    { "country",        "country region",   wifi_profile_set_country_region_ex},
    { "dtim",           "DTIM interval",    wifi_profile_set_dtim_interval_ex},
    { "listen",         "listen interval",  wifi_profile_set_listen_interval_ex},
    { "mac",            "MAC address",      wifi_profile_set_mac_address_ex},
    { "sec",        "WPA/WPA2PSK Authmode, Encrypt Type",   wifi_profile_set_security_mode_ex   },
    { "psk",         "WPA/WPA2PSK Key",  wifi_profile_set_psk_ex   },
    { "pmk",            "PMK for WPA/WPA2PSK",  wifi_profile_set_pmk_ex   },
    { "wep",            "WEP key",          wifi_profile_set_wep_key_ex },
    { "n9dbg",          "set N9 debug level",   wifi_profile_set_n9_dbg_level},
    { NULL }
};

cmd_t   wifi_profile_get_cli[] = {
    { "opmode",         "STA/AP/Dual",      wifi_profile_get_opmode_ex},
    { "ch",             "channel",          wifi_profile_get_channel_ex},
    { "bw",             "bandwidth",        wifi_profile_get_bandwidth_ex},
    { "ssid",           "SSID",             wifi_profile_get_ssid_ex      },
//  { "bssid",          "BSSID",            wifi_profile_get_bssid_ex     },
    { "wirelessmode",   "wireless mode",    wifi_profile_get_wireless_mode_ex},
    { "country",        "country region",   wifi_profile_get_country_region_ex},
    { "dtim",           "DTIM interval",    wifi_profile_get_dtim_interval_ex},
    { "listen",         "listen interval",  wifi_profile_get_listen_interval_ex},
    { "mac",            "MAC address",      wifi_profile_get_mac_address_ex},
    { "sec",        "WPA/WPA2PSK Authmode, Encrypt Type",   wifi_profile_get_security_mode_ex   },
    { "psk",         "WPA/WPA2PSK Key",  wifi_profile_get_psk_ex  },
    { "pmk",            "PMK for WPA/WPA2PSK",  wifi_profile_get_pmk_ex   },
    { "wep",            "WEP key",          wifi_profile_get_wep_key_ex },
    { "profile",            "get profile txt",      wifi_profile_get_profile_ex},
    { "n9dbg",          "get N9 debug level",   wifi_profile_get_n9_dbg_level},
    { NULL }
};

#endif //MTK_WIFI_PROFILE_ENABLE


