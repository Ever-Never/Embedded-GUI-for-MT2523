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


#ifndef __MLME_INTERNAL_H_
#define __MLME_INTERNAL_H_

#define __CM4_TYPE_H__   // if stdint.h included, please bypass CM4_TYPE.h
#include "rom/type_def.h"
#include "wifi_inband.h"
#include "wifi_api.h"
#include "wifi_os_api.h"

#include "FreeRTOS.h"
#include"syslog.h"
#include "semphr.h"
#include "hal_gpt.h"

#include "rt_config.h"
#include "patch_config_ram.h"

#include "wifi_event_id.h"


#define DBG_MLME_REBOOT  0

#define MLME_STATUS_NONE  0
#define MLME_SSID_DIRTY  0x01
#define MLME_PASSPHRASE_DIRTY 0x02
#define MLME_AUMODE_DIRTY  0x04
#define MLME_ENC_DIRTY  0x08
#define MLME_OPMODE_DIRTY 0x10
#define MLME_PMK_DIRTY 0x20

#define MLME_MAX_SSID_LEN 32
#define MLME_MAX_PASSPHRASE_LEN 64

#undef PREFIX
#define PREFIX "-------------------"


#define LINKDOWN_FLAG_NONE 0 
#define IGNORE_LINKDOWN_FLAG 0
#define LINKDOWN_REASON_BOOT_UP 1
#define LINKDOWN_REASON_OPMODE_SWITCH 2
#define LINKDOWN_REASON_RADIO_ON_OFF 3
#define LINKDOWN_REASON_DISCONNECT_AP 4
#define LINKDOWN_REASON_RELOAD_UNDER_REPEATER 5
#define LINKDOWN_REASON_WPS_SECURITY_NOT_SUPPORT  6

#define RECONNECT_REASON_ASSOC_TIMEOUT  1
typedef enum
{
    SEC_AUTO_ENABLE,
    SEC_AUTO_DISABLE
}sec_ctrl_e;

typedef struct _comm_config
{
    int status;
    uint8_t ssid[MLME_MAX_SSID_LEN + 1];
    uint8_t ssid_length;
    uint8_t passphrase[MLME_MAX_PASSPHRASE_LEN];
    uint8_t passphrase_length;

    /*These values are stored after calculating the pmk*/
    uint8_t cached_ssid[MLME_MAX_SSID_LEN];
    uint8_t cached_ssid_length;
    uint8_t cached_passphrase[MLME_MAX_PASSPHRASE_LEN];
    uint8_t cached_passphrase_length;

    uint8_t cached_pmk[32];
    
    bool   b_pmk_cal;
    uint8_t auth_mode;
    uint8_t encrypt_type;
    wep_key_cm4_t wep_key;
}comm_config_t;

typedef struct
{
    uint8_t  bssid[6];
    char     auth_mode;
    char     enc_type;
    char     rssi;
    uint8_t  channel;
}bss_entry_t;


typedef struct
{
    bool b_valid;
    uint8_t bssid[6];
    os_tmr_t recover_timer;
}black_list_node_t;

#define FW_MAX_BLACK_LIST_SIZE  5
#define FW_BLACK_LIST_RECOVER_TIMER 15

#define FW_MAC_CIPPHER_ERROR_COUNT 5

typedef struct _mlme_cache_setting_t
{
    comm_config_t     sta_config;
    comm_config_t     ap_config;
    uint8_t           op_mode;
    uint8_t           scan_round;
    int               status;
    PRTMP_ADAPTER     pAd;
    sys_cfg_t*        fw_iotSysCfg;
    wifi_config_t     *user_config;
    wifi_config_ext_t *user_config_ex;
    bss_entry_t       target_bss;
    sec_ctrl_e        sec_ctrl;
#ifdef MTK_WIFI_WPS_ENABLE
    bool              b_wsc_auto_connect;
    bool              b_wait_deassoc_after_wsc_done;
    os_tmr_t          wsc_done_timer;
    bool              b_start_wsc_after_linkdown;
    bool              try_other_profile_if_fail;
    int               connect_count;
#endif
    bool              b_connection_allowed;
    bool              b_rx_handler_registerd;
    uint32_t          ignore_linkdown;
    bool              b_update_after_scan;
    bool              b_connect;
    uint8_t           fw_lock_idx;
#ifdef MTK_WIFI_REPEATER_ENABLE
    bool              b_delay_scan;
    os_tmr_t          delay_scan_timer;
#endif
    bool              sta_first_time_connect;
    bool              ap_first_time_linkup;
    bool              is_rxfilter_cached;
    bool              b_flush_auto_sec;
    uint32_t          cached_rxfilter;
#if (MT7682_WSAP00024415 == 1)
    os_tmr_t          rxfilter_recover_timer;
#endif
    os_tmr_t          rsn_timer;
    os_tmr_t          assoc_timer_out;
#if (DBG_MLME_REBOOT == 1)
    os_tmr_t          reboot_timer;
#endif

#ifdef MTK_WIFI_REPEATER_ENABLE
    bool              b_connect_after_ap_start;
#endif

    uint32_t         reconnect_reason;
    uint8_t          cippher_error_count;
    bool             b_process_cippher_error;
    black_list_node_t fw_black_list[FW_MAX_BLACK_LIST_SIZE] ;

}mlme_cache_setting_t;

#define MAX_BSSID_BUFFER_SIZE 5

typedef struct
{
    int totol_count;
    bss_entry_t  matched_bss_list[MAX_BSSID_BUFFER_SIZE];
}bss_list_t;

typedef void (*context_switch_handle)(void *data);

void fw_context_switch_sync(void * data,context_switch_handle pf_handle);

extern int process_global_event(void *priv, unsigned char evt_id, unsigned char *body, int len);
void __fw_lock_sleep(mlme_cache_setting_t * p_cache_setting);
void __fw_unlock_sleep(mlme_cache_setting_t * p_cache_setting);
void __seek_and_connect(mlme_cache_setting_t * p_cache_setting);
void __sta_setting_cb(void *data);
void __ap_setting_cb(void *data);

void fw_send_event(uint16_t event_id, uint8_t data_len,uint8_t * data);


#if (MT7682_WSAP00024415 == 1)
extern VOID mlmeCalcChannelQuality(BOOL enable);
#endif

#endif


