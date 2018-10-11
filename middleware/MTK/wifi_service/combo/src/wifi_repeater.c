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


#include "wifi_event.h"
#include "wifi.h"
#include "wifi_event_id.h"
#include "type_def.h"
#include "os.h"
#include "os_util.h"
#include "wifi_inband.h"
#ifdef MTK_WIFI_ROM_ENABLE
#ifdef MTK_WIFI_REPEATER_ENABLE
#include "mlme_scan.h"
#include "wifi_scan.h"

extern int printf(const char *format, ...);

extern SCAN_ARRAY_ITEM g_scan_array[];

static SCAN_ITEM_T g_cm4_scan_arry[MAX_SCAN_ARRAY_ITEM];

SCAN_ITEM_T * wifi_get_scan_table(VOID)
{
    int i = 0;
    os_memset(g_cm4_scan_arry,0,sizeof(g_cm4_scan_arry));

    for(;i < MAX_SCAN_ARRAY_ITEM; ++i)
    {
        if(g_scan_array[i].valid)
        {
            g_cm4_scan_arry[i].auth_mode = g_scan_array[i].beacon.auth_mode;
            g_cm4_scan_arry[i].enc_type = g_scan_array[i].beacon.encrypt_mode;
            g_cm4_scan_arry[i].rssi = g_scan_array[i].beacon.rssi;
            g_cm4_scan_arry[i].ssid_len = g_scan_array[i].beacon.SsidLen;
            g_cm4_scan_arry[i].channel = g_scan_array[i].beacon.Channel;
            os_memcpy(g_cm4_scan_arry[i].ssid,g_scan_array[i].beacon.Ssid,g_scan_array[i].beacon.SsidLen);
            os_memcpy(g_cm4_scan_arry[i].bssid,g_scan_array[i].beacon.Bssid,6);
        }
    }
    return g_cm4_scan_arry;
}

int32_t wifi_scan_in_repeater_mode(uint8_t * ssid, uint8_t ssid_len)
{
     int32_t status = 0;
     int32_t len_param = 0;
     P_CMD_IOT_START_SCAN_T scan_param = NULL;


      os_memcpy(g_scan_ssid,ssid,ssid_len);
      g_scan_ssid_len = ssid_len;

     /*Pengfei.Qiu 20170103, will integrate 1~4 as a wifi_build_channel_list() in future*/
     /*Panda.Gao 20170706, use wifi_get_scan_param_and_channel_list() replace 1~4.*/
     scan_param = wifi_get_scan_param_and_channel_list(&len_param);
     if( NULL==scan_param ){
        printf("wifi_get_scan_param_and_channel_list() fail.\n");
        return -1;
     }
     
     /*malloc memory for scan_param*/

     /*copy ssid to scan_param and enable ssid*/

     os_memcpy(scan_param->SSID, ssid,ssid_len);
     scan_param->enable_ssid = (0x80 | ssid_len); // enable ssid

     /*set partial scan and APCLI full scan recongize flag to N9*/
     scan_param->partial = 1;
     (scan_param->channel_list).triplet[0].channel_prop = (scan_param->channel_list).triplet[0].channel_prop | 0x80;
     status = wifi_inband_scan_custom(scan_param, len_param);
     os_free(scan_param);
     return status;
}

#endif

void wifi_repeater_wlan_evt_handler(unsigned char *body, int len)
{
#if defined(MTK_WIFI_REPEATER_ENABLE)
    iwreq_data_t    *wrqu = (iwreq_data_t *)body;
    uint32_t cmd;
    cmd = *(uint32_t *)(body + sizeof(iwreq_data_t));

    if (wrqu->data.flags ==  RT_SCAN_DROPPED_EVENT_FLAG)
    {
        unsigned char *extra = NULL;

        if (wrqu->data.length) {
            extra = body + sizeof(iwreq_data_t) + sizeof(cmd);
         }
        wifi_event_notification(0,WIFI_EVENT_ID_IOT_SCAN_DRPOPPED,extra,wrqu->data.length);

    }
#endif
}
#endif

