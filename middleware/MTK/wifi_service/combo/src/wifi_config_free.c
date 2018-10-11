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
    WiFi  Scan

    Abstract:
    WiFi processor scan result handler for Supplicant

    Revision History:
    Who                     When                 What
    Michael Rong      2015/05/22       Initial
    --------            ----------      ------------------------------------------
***************************************************************************/
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
//#include "wifi_inband.h"
//#include "wifi_private_api.h"

#include "hal_sys.h"
#include "hal_cache.h"

#ifdef MTK_WIFI_DIRECT_ENABLE
#include "wifi_inband.h"
#endif
#include "wifi_os_api.h"
#include "wifi_config_free.h"


CONFIGURE_FREE_CALLBACK g_configure_free_callback = {
	.save_credential = NULL,
	.save_ready = NULL,
};
#ifdef MTK_WIFI_CONFIGURE_FREE_ENABLE

void register_configure_free_callback(void *save_credential_func, void *save_ready_func)
{
	g_configure_free_callback.save_credential = (int32_t (*)(P_IOT_CONFIG_FREE_IE cred))save_credential_func;
	g_configure_free_callback.save_ready = (int32_t (*)(uint8_t config_ready))save_ready_func;
}

//
//  ConfigureFree - parse credential for NVRAM Setting
//
int wifi_cf_credential_handler(unsigned int inf, unsigned char *body, int len)
{
    int status = 0; // 0: SUCCESS, -1: Failure
    uint8_t config_ready = 1;
    P_IOT_CONFIG_FREE_IE cred = (P_IOT_CONFIG_FREE_IE)body;

    // Sanity Check
    if (len < sizeof(IOT_CONFIG_FREE_IE)) { // no iwreq CMD(4) needed
        LOG_E(wifi, "ERROR! [ConfigFree] Invalid wlan evt received, len(%d) < %d",
              len, sizeof(IOT_CONFIG_FREE_IE));
        return -1;
    }
#ifdef MTK_MINISUPP_ENABLE
    if (inf >= MAX_INTERFACE_NUM) {
        LOG_E(wifi, "ERROR! [ConfigFree] Wlan evt received from invalid interface(=%d)(MTK_WIFI_REPEATER_ENABLE?).", inf);
        return -1;
    }
#endif /* MTK_MINISUPP_ENABLE */

    if (!g_configure_free_callback.save_credential ||
	  !g_configure_free_callback.save_ready)
    {
        LOG_E(wifi, "ERROR! [ConfigFree] callback not registered yet");
    	return -1;
    }

    // Set nvram configuration by credential
    status = g_configure_free_callback.save_credential(cred);

    // Success - 1. Set NVRAM ConfigFree_Ready, 2. Reboot
    if (status == 0) {
        // Set NVRAM ConfigFree_Ready (0: un-configured, 1: configured)
        if (g_configure_free_callback.save_ready(config_ready) != 0) {
            LOG_E(wifi, "ERROR! [ConfigFree][COMMON] wifi_profile_set_configfree failed");
            status = -1;
        }

        LOG_I(wifi, "[ConfigFree] Successfully. System Reboot!!!");
#ifdef HAL_CACHE_MODULE_ENABLED
        hal_cache_disable();
        hal_cache_deinit();
#endif
#ifdef READY
        hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
#endif
    }

    return status;
}
#else
void register_configure_free_callback(void *save_credential_func, void *save_ready_func)
{
    LOG_W(wifi,"Not support Config free callback");
}

int wifi_cf_credential_handler(unsigned int inf, unsigned char *body, int len)
{
    LOG_W(wifi,"Not support config free handler");
    return 0;
}

#endif
