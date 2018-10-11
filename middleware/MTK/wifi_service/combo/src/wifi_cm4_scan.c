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
#include <stdlib.h>
#include <string.h>
#include "type_def.h"
#include "wifi_scan.h"
#include "wifi_api.h"
#include "wifi.h"
#include "os.h"
#include "os_util.h"
#include "syslog.h"
#ifdef MTK_MINISUPP_ENABLE
#include "driver_inband.h"
#endif
#include "wifi_inband.h"
//#include "wifi_private_api.h"

//#include "hal_sys.h"
//#include "hal_cache.h"

#ifdef MTK_WIFI_DIRECT_ENABLE
#include "wifi_inband.h"
#endif
//#include "mt_cmd_fmt.h"
//#include "connsys_util.h"
#include "wifi_os_api.h"

#include "wifi_event_id.h"

#ifdef MTK_WIFI_ROM_ENABLE
#include "mlme_scan.h"
#endif

extern BeaconProbeRspStr_t g_beacon;

extern wifi_scan_list_item_t *g_scan_list;
extern uint8_t g_scan_list_size ;
extern uint8_t g_scan_inited ; //To protect the buffer in function wifi_connection_scan_init(), if other task start scan just before reset the buffer.
extern char g_scan_preferred_ssid[] ; //Castro
extern unsigned char g_scan_bssid[] ;
extern uint8_t ZeroSsid[];

#define DEV_PASS_ID_PBC     0x0004
#define DEV_PASS_ID_PIN     0x0000

/*
#if defined(__ICCARM__)
BSS_IN_TCM static SCAN_ARRAY_ITEM g_scan_array[MAX_SCAN_ARRAY_ITEM];
#else
static SCAN_ARRAY_ITEM BSS_IN_TCM g_scan_array[MAX_SCAN_ARRAY_ITEM];
#endif
*/
#ifdef MTK_CM4_SCAN_TABLE_ENABLE
SCAN_ARRAY_ITEM g_scan_array[MAX_SCAN_ARRAY_ITEM];
static unsigned char g_scan_sequence = 0;
#endif


#ifdef MTK_CM4_SCAN_TABLE_ENABLE
int wifi_scan_list_search_by_bssid(uint8_t *bssid)
{
    int i;
    for (i = 0; i < g_scan_table_size; i++) {
        if (os_memcmp(g_scan_array[i].beacon.Bssid, bssid, WIFI_MAC_ADDRESS_LENGTH) == 0) {
            return i;
        }
    }
    return -1;
}

int guery_config_ssid(BeaconProbeRspStr_t *beacon)
{
    uint32_t index = 0;
    int minium = 0;
    for(index = 0; index < MAX_SCAN_ARRAY_ITEM; index++) {
        if(g_scan_array[index].beacon.rssi < g_scan_array[minium].beacon.rssi) {
            minium = index;
        }
    }
    if(g_scan_array[minium].beacon.rssi < beacon->rssi) {
        return minium;
    } else {
        return MAX_SCAN_ARRAY_ITEM;
    }

}

void sort_scan_array_by_rssi(void)
{
    uint8_t counter = 0;
    uint8_t record_subscript[MAX_SCAN_ARRAY_ITEM] = {0};
    uint8_t i = 0;
    uint8_t j = 0;
    SCAN_ARRAY_ITEM temp;
    for(uint8_t index = 0; index < MAX_SCAN_ARRAY_ITEM; index++) {
        if(g_scan_array[index].is_config_ssid == 1) {
            record_subscript[counter++] = index;
        }
    }
    os_memset(&temp, 0, sizeof(SCAN_ARRAY_ITEM));
    if(counter > 1){
        for(i = 0; i < counter; i++)
            for(j = i; j < counter-1; j++) {
                if(g_scan_array[record_subscript[j]].beacon.rssi < g_scan_array[record_subscript[j+1]].beacon.rssi) {
                    os_memcpy(&temp, &g_scan_array[record_subscript[j]], sizeof(SCAN_ARRAY_ITEM));
                    os_memcpy(&g_scan_array[record_subscript[j]], &g_scan_array[record_subscript[j+1]], sizeof(SCAN_ARRAY_ITEM));
                    os_memcpy(&g_scan_array[record_subscript[j+1]],&temp, sizeof(SCAN_ARRAY_ITEM));
                }
            }
    }
}

/**
*  Get the AP full information list of the last scan operation (for mini_supplicant)
*
* @parameter None
* @return Number of AP scaned and a BEACON data structure will be returned
* @note If no wifi_conn_start_scan() has been performed, an empty data structure is returned
*/
int32_t wifi_scan_get_full_list(FULL_SCAN_LIST *scan_list)
{
    int i;
    int valid_count = 0;

    for (i = 0; i < g_scan_table_size; i++) {
        if (!g_scan_array[i].valid) {
            continue;
        }
        valid_count++;
        os_memcpy(&(scan_list->beacon), &(g_scan_array[i].beacon), sizeof(BeaconProbeRspStr_t));
        scan_list = scan_list->next;
        if (scan_list == NULL) {
            break;
        }
    }
    return valid_count;
}
SCAN_ARRAY_ITEM *wifi_scan_table_array(void)
{
    return g_scan_array;
}


void wifi_scan_reset()
{
    int i;
    LOG_I(wifi, "================= SCAN RESET =========");
    for (i = 0; i < MAX_SCAN_ARRAY_ITEM; i++) {
        g_scan_array[i].valid = 0;
        g_scan_array[i].is_config_ssid = 0;
    }
    g_scan_sequence++;
}


BeaconProbeRspStr_t *wifi_scan_get_item(int i)
{
    if (!g_scan_array[i].valid) {
        return NULL;
    }

    return &(g_scan_array[i].beacon);
}

/*
void wifi_scan_table_age_out(void *eloop_ctx, void *timeout_ctx)
{
    int idx = 0;
    struct os_time t;

    os_get_time(&t);
    t.sec -= 60;

    for (idx = 0; idx < g_scan_table_size; idx++) {
        if (g_scan_array[idx].valid && os_time_before(&g_scan_array[idx].last_update, &t)) {
            LOG_I(wifi, "age out this AP - %s", g_scan_array[idx].beacon.Ssid);
            os_memset(&(g_scan_array[idx].beacon), 0, sizeof(BeaconProbeRspStr_t));
            g_scan_array[idx].is_config_ssid = 0;
            g_scan_array[idx].valid = 0;
        }
    }
}
*/

#endif

int wifi_scan_list_handler(unsigned int inf, unsigned char *body, int len)
{
    BeaconProbeRspStr_t *pBeacon;
    wifi_scan_list_item_t temp_to_insert;
    os_memset(&temp_to_insert, 0, sizeof(wifi_scan_list_item_t));
    wifi_event_handler_t handler = NULL;

#ifdef MTK_CM4_SCAN_TABLE_ENABLE
    int i, empty_slot = -1;
    uint8_t record_config_ssid = 0;
    int record_subscript = -1;
#endif

    if (g_scan_by_app) {
        handler = wifi_rx_event_handler[WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE];
        if (handler != NULL) {
            handler(WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE, body, len);
        }
    }

    pBeacon = &g_beacon;
    os_memset(pBeacon, 0, sizeof(BeaconProbeRspStr_t));
    if (!wifi_beacon_probe_rsp_parser(body, len, pBeacon)) {
        LOG_E(wifi, "beacon parse error!");
        return -1;
    }

    /***********************add for scan list common API*******************************/
    if (g_scan_list != NULL && g_scan_inited == 1) {
        BssCipherParse(pBeacon->CapabilityInfo,
                       &pBeacon->auth_mode,
                       &pBeacon->encrypt_mode,
                       pBeacon->LengthVIE,
                       pBeacon->VarIE);
        os_memcpy(&temp_to_insert.ssid[0], &pBeacon->Ssid[0], pBeacon->SsidLen);
        os_memcpy(&temp_to_insert.bssid[0], &pBeacon->Bssid[0], WIFI_MAC_ADDRESS_LENGTH);
        temp_to_insert.bss_type = pBeacon->BssType;
        temp_to_insert.beacon_interval = pBeacon->BeaconPeriod;
        temp_to_insert.auth_mode = (wifi_auth_mode_t)pBeacon->auth_mode;
        temp_to_insert.capability_info = pBeacon->CapabilityInfo;
        temp_to_insert.channel = pBeacon->Channel;
        if (pBeacon->HtCapabilityLen > 0) {
            if (pBeacon->AddHtInfoLen > 0) {
                temp_to_insert.central_channel = get_cent_ch_by_htinfo(&pBeacon->AddHtInfo,
                                                 &pBeacon->HtCapability);
            }
        } else {
            temp_to_insert.central_channel = temp_to_insert.channel;
        }
        temp_to_insert.encrypt_type = (wifi_encrypt_type_t)pBeacon->encrypt_mode;
        temp_to_insert.rssi = pBeacon->rssi;
        temp_to_insert.ssid_length = pBeacon->SsidLen;
        temp_to_insert.is_hidden = pBeacon->hidden_ssid;
        temp_to_insert.is_valid = 1;
        temp_to_insert.is_wps_supported = pBeacon->wps;
        if(1 == temp_to_insert.is_wps_supported) {
            temp_to_insert.wps_element.configuration_methods = pBeacon->wps_element.configuration_methods;
            temp_to_insert.wps_element.device_password_id = pBeacon->wps_element.device_password_id;
            temp_to_insert.wps_element.selected_registrar = pBeacon->wps_element.selected_registrar;
        }

        insert_list_by_rssi(temp_to_insert);
    }
    /***********************add for scan list common API*******************************/


#ifdef MTK_CM4_SCAN_TABLE_ENABLE

    empty_slot = wifi_scan_list_search_by_bssid(pBeacon->Bssid);

    if (empty_slot == -1) {
        //Castro+
        if (os_strlen(g_scan_preferred_ssid) > 0 && (os_strlen(g_scan_preferred_ssid) == pBeacon->SsidLen) && (os_memcmp(g_scan_preferred_ssid, (char *)pBeacon->Ssid, pBeacon->SsidLen) == 0)
                && (os_memcmp(&g_scan_array[0].beacon.Ssid[0], g_scan_preferred_ssid, g_scan_array[0].beacon.SsidLen) != 0)) {
            empty_slot = 0;
            LOG_I(wifi, "Castro:preferred SSID - %s vs %s", pBeacon->Ssid, g_scan_preferred_ssid);
        } else {
            //Castro
            for (i = 0; i < g_scan_table_size; i++) {
                if (g_scan_array[i].valid == 1) {
                    /* Choose a empty_slot in the valid and oldest slot */
#if 0
                    /* Duplicated beacon found */
                    if (os_memcmp(g_scan_array[i].beacon.Bssid, pBeacon->Bssid, WIFI_MAC_ADDRESS_LENGTH) == 0) {
                        return 0;
                    }
#endif
                    if (g_scan_array[i].is_config_ssid) {
                        record_config_ssid++;
                        continue;
                    }

                    if ((g_scan_ssid_len != 0) && (g_scan_ssid_len == pBeacon->SsidLen) && \
                            (os_memcmp(g_scan_ssid, (char *)pBeacon->Ssid, g_scan_ssid_len) == 0)) {
                        if((os_memcmp(&g_scan_bssid[0], &ZeroMAC, WIFI_MAC_ADDRESS_LENGTH) == 0) || \
                            (os_memcmp(&g_scan_bssid[0], &pBeacon->Bssid[0], WIFI_MAC_ADDRESS_LENGTH) == 0)) {
                            /* Insert candidate AP to scan table anyway. */
                            wifi_scan_add_scan_bssid(pBeacon->Bssid);
                            empty_slot = i;
                            break;
                        }
                    }

                    if (os_memcmp(&target_bssid[0], &ZeroMAC, WIFI_MAC_ADDRESS_LENGTH) &&
                            (os_memcmp(&target_bssid[0], &pBeacon->Bssid[0], WIFI_MAC_ADDRESS_LENGTH) == 0)) {
                        /* Insert candidate AP to scan table anyway. */
                        empty_slot = i;
                        break;
                    }

                    if (g_scan_pbc_ap
                        && (1 == pBeacon->wps_element.selected_registrar)
                        && (DEV_PASS_ID_PBC == pBeacon->wps_element.device_password_id)) {
                        /* Insert  triggered WPS PBC AP to scan table for WPS PBC process. */
                        empty_slot = i;
                        break;
                    }
                    if ((1 == pBeacon->wps_element.selected_registrar)
                         && (DEV_PASS_ID_PIN == pBeacon->wps_element.device_password_id)) {
                        /* Insert triggered WPS PIN AP to scan table for WPS PIN process. */
                        empty_slot = i;
                        break;
                    }
#ifdef MTK_WIFI_DIRECT_ENABLE
                    //To Do :add opmode == 5 condition
                    char *__ssid = pBeacon->Ssid;
                    if (os_strstr(__ssid, "DIRECT-") == __ssid) {
                        /* Insert Candidate P2P to scan table for P2P process. */
                        LOG_I(wifi, "Wifi Direct candidate will be insert to the scan_list !!!!!");
                        empty_slot = i;
                        break;
                    }
#endif /*MTK_WIFI_DIRECT_ENABLE*/
#if 0
                    /* pick up min scan sequence of valid item */
                    if (min_scan_seq > g_scan_array[i].scan_seq_number) {
                        min_scan_seq = g_scan_array[i].scan_seq_number;
                        empty_slot = i;
                    }
#endif
                } else {
                    if ((g_scan_ssid_len != 0) && (g_scan_ssid_len == pBeacon->SsidLen) && \
                            (os_memcmp(g_scan_ssid, (char *)pBeacon->Ssid, g_scan_ssid_len) == 0)) {
                        wifi_scan_add_scan_bssid(pBeacon->Bssid);
                    }
                    empty_slot = i;
                    break;
                }
            }
        }
    }

    if (empty_slot < 0) {
        return -1;
    }

    if ((g_scan_ssid_len != 0) && (g_scan_ssid_len == pBeacon->SsidLen) && \
        (os_memcmp(g_scan_ssid, (char *)pBeacon->Ssid, g_scan_ssid_len) == 0) &&\
        record_config_ssid == MAX_SCAN_ARRAY_ITEM) {
            record_subscript = guery_config_ssid(pBeacon);
            empty_slot = (record_subscript == MAX_SCAN_ARRAY_ITEM? empty_slot: record_subscript);
    }
    /* Tp avoid Hidden SSID form beacon to overwirite correct SSID from probe response.
    Pengfei add: below design has a protential risk, need confirm and improve in future when get probe response first, then get beacon from the same Hidden AP.
    the g_scan_array[] will keeps "SSID" and VarIE of probe response frame, and keeps other information with Beacon frame.
    if the beacon has different VarIE with probe response, the scan table information is not consistent*/
    if (//(g_scan_array[empty_slot].beacon.hidden_ssid) &&
            (os_memcmp(&pBeacon->Ssid[0], ZeroSsid, pBeacon->SsidLen) == 0) &&
            (os_memcmp(&g_scan_array[empty_slot].beacon.Ssid[0], ZeroSsid, g_scan_array[empty_slot].beacon.SsidLen) != 0)) {
        os_memset(&pBeacon->Ssid[0], 0, WIFI_MAX_LENGTH_OF_SSID);
        os_memmove(&pBeacon->Ssid[0], &g_scan_array[empty_slot].beacon.Ssid[0], g_scan_array[empty_slot].beacon.SsidLen);
        os_memmove(&pBeacon->VarIE[0], &g_scan_array[empty_slot].beacon.VarIE[0], MAX_VIE_LEN);
        pBeacon->SsidLen = g_scan_array[empty_slot].beacon.SsidLen;
    }
    //os_get_time((struct os_time *)&g_scan_array[empty_slot].last_update);


    os_memcpy(&(g_scan_array[empty_slot].beacon), (uint8_t *)pBeacon, sizeof(BeaconProbeRspStr_t));
    g_scan_array[empty_slot].valid = 1;
    if (pBeacon->HtCapabilityLen > 0) {
        os_memcpy(&g_scan_array[empty_slot].beacon.HtCapability, &pBeacon->HtCapability, pBeacon->HtCapabilityLen);
        g_scan_array[empty_slot].beacon.HtCapabilityLen = pBeacon->HtCapabilityLen;
        if (pBeacon->AddHtInfoLen > 0) {
            os_memcpy(&g_scan_array[empty_slot].beacon.AddHtInfo, &pBeacon->AddHtInfo, pBeacon->AddHtInfoLen);
            g_scan_array[empty_slot].beacon.AddHtInfoLen = pBeacon->AddHtInfoLen;
            g_scan_array[empty_slot].beacon.CentralChannel = get_cent_ch_by_htinfo(&pBeacon->AddHtInfo,
                    &pBeacon->HtCapability);
        }
    } else {
        g_scan_array[empty_slot].beacon.CentralChannel = g_scan_array[empty_slot].beacon.Channel;
    }
    BssCipherParse(g_scan_array[empty_slot].beacon.CapabilityInfo,
                   &g_scan_array[empty_slot].beacon.auth_mode,
                   &g_scan_array[empty_slot].beacon.encrypt_mode,
                   g_scan_array[empty_slot].beacon.LengthVIE,
                   g_scan_array[empty_slot].beacon.VarIE);
    g_scan_array[empty_slot].scan_seq_number = g_scan_sequence;
    if ((g_scan_ssid_len != 0) && (g_scan_ssid_len == pBeacon->SsidLen) && \
            (os_memcmp(g_scan_ssid, (char *)pBeacon->Ssid, g_scan_ssid_len) == 0)) {
        g_scan_array[empty_slot].is_config_ssid = 1;
    }
#endif
    return 0;
}





int32_t wifi_config_get_scan_table_size(uint8_t *size)
{
#ifdef MTK_CM4_SCAN_TABLE_ENABLE
    if (NULL == size) {
        return -1;
    }
    *size = g_scan_table_size;
    return 0;
#else
    return 0;
#endif
}

int32_t wifi_config_set_scan_table_size(uint8_t size)
{
#ifdef MTK_CM4_SCAN_TABLE_ENABLE
    int32_t status = 0;

    if ((size != 0)
            && (size <= MAX_SCAN_TABLE_SIZE)) {
        if (g_scan_table_size != size) {
            wifi_connection_stop_scan();
#ifdef MTK_MINISUPP_ENABLE
            if (__g_wpa_supplicant_api.wpa_supplicant_entry_stop_scan) {
                __g_wpa_supplicant_api.wpa_supplicant_entry_stop_scan();
            }
#endif /* MTK_MINISUPP_ENABLE */
            g_scan_table_size = size;
#ifdef MTK_MINISUPP_ENABLE
            if ((status >= 0) && __g_wpa_supplicant_api.wpa_supplicant_entry_start_scan) {
                __g_wpa_supplicant_api.wpa_supplicant_entry_start_scan();
            }
#endif /* MTK_MINISUPP_ENABLE */
        } else {
            LOG_E(wifi, "%s: same size(=%d). Do nothing here.", __FUNCTION__, size);
        }
    } else  {
        LOG_E(wifi, "%s: incorrect size(=%d), maximum size is %d", __FUNCTION__, size, MAX_SCAN_TABLE_SIZE);
        status = -1;
    }

    return status;

#else
    return -1;
#endif

}



