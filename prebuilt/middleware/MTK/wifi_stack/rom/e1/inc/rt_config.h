/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	rt_config.h

	Abstract:
	Central header file to maintain all include files for all NDIS
	miniport driver routines.

	Revision History:
	Who         When          What
	--------    ----------    ----------------------------------------------
	Paul Lin    08-01-2002    created

*/
#ifndef	__RT_CONFIG_H__
#define	__RT_CONFIG_H__
#if N9_PORTING
#include "precomp_rom.h"

//#include "gl_typedef.h"
#include "gl_kal_iot_rom.h"
#endif
#if 0
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#if (defined(DATA_PATH_86) || defined(DATA_PATH_87))
#include "lwip/pbuf.h"
#endif
#endif
#include "rom/type_def.h"
#include "rom/connsys_profile.h"
#include "type_def_N9.h"
#include "hal_reg_rom.h"
#include "New_Prototype.h"
#include "typedef_iot_rom.h"
//#include "link.h"
#include "queue_iot_rom.h"
#include "hal_hw_def_rom.h"
#include "wlan_cmdevent_iot_rom.h"

//#include "wifi_rom_api_vctr.h"
#if N9_PORTING
#include "cnm_timer_iot_rom.h"
#endif
#if N9_PORTING
#include "packet_filter_iot_rom.h"
#include "sys_irq.h"
#include "bget.h"
#include	"init.h"
#endif
#include 	"sys_hal_types_ext.h"
#include    "rtmp_type.h"
#include    "rtmp_def.h"
#include 	"rt_bora.h"
//#include    "rt28xx.h"  need to use mt76x2.h ??
#ifdef UAPSD_SUPPORT
#include "uapsd.h"
#endif /* UAPSD_SUPPORT */

#ifdef RT2860
#include	"rt2860.h"
#endif // RT2860 //
#include "rt28xx.h"

#ifdef N9_HAL_PORTING_UNPATCH
#if 0
#include "wifi_hal.h"
#include "wifi_hal_reg.h"
#include "wifi_hal_pse.h"
#include "wifi_hal_pse_lib.h"
#include "wifi_hal_wtbl.h"
#endif
#if 1
#include "hal_rom.h"
#ifdef IOT_PROCESS_PORTING
#include "hal_pwr_mgt_rom.h"
#endif/*IOT_PROCESS_PORTING*/
//#include "hal_reg_rom.h"
#include "hal_pse_rom.h"
#include "hal_pse_com_def_rom.h"
#include "hal_pse_mt6630def_rom.h"
#include "hal_pse_lib_rom.h"
#include "hal_wtbl_rom.h"
#ifdef IOT_PROCESS_PORTING
#include "nic/nic.h"
#include "nic/nic_dma.h"
#include "mgmt/txm.h"
#endif/*IOT_PROCESS_PORTING*/



#endif

#endif/*N9_HAL_PORTING_UNPATCH*/

#include    "oid.h"
#include    "mlme.h"
#include    "wpa.h"
#include    "md5.h"
#include    "rtmp.h"
#include    "wpa_rom.h"
#include	"dfs.h"
#include	"ioctl.h"
#include	"chlist.h"
#include 	"rtmp_comm.h"
#ifdef RALINK_ATE
#include "rt_ate.h"
#endif // RALINK_ATE //

#undef WSC_INCLUDED

#ifdef CONFIG_AP_SUPPORT
#include "ap.h"
#include "ap_autoChSel.h"
#include "ap_cfg.h"
#include "bcn.h"

#ifdef APCLI_SUPPORT
#include "ap_apcli.h"
#ifdef MAT_SUPPORT
#include "mat.h"
#endif
#endif /* APCLI_SUPPORT */
#ifdef CONFIG_AP_SUPPORT
#ifdef APCLI_SUPPORT
#ifdef MAC_REPEATER_SUPPORT
#include "ap_repeater.h"
#endif /* MAC_REPEATER_SUPPORT */
#endif /* APCLI_SUPPORT */
#endif /* CONFIG_AP_SUPPORT */

#endif /* CONFIG_AP_SUPPORT */
//#include "ra_ctrl.h"
#include "mt_mac.h"
//#include "cmm_asic_mt.h"
#ifdef LEAP_SUPPORT
#include    "leap.h"
#endif // LEAP_SUPPORT //

#ifdef CONFIG_STA_SUPPORT
#ifdef WSC_STA_SUPPORT
#define WSC_INCLUDED
#endif // WSC_STA_SUPPORT //
#endif // CONFIG_STA_SUPPORT //



#ifdef CONFIG_STA_SUPPORT
#ifdef NATIVE_WPA_SUPPLICANT_SUPPORT
#ifndef WPA_SUPPLICANT_SUPPORT
#error "Build for being controlled by NetworkManager or wext, please set HAS_WPA_SUPPLICANT=y and HAS_NATIVE_WPA_SUPPLICANT_SUPPORT=y"
#endif // WPA_SUPPLICANT_SUPPORT //
#endif // NATIVE_WPA_SUPPLICANT_SUPPORT //
#ifdef WOW_SUPPORT
#include "wow.h"
#endif // WOW_SUPPORT //
#endif // CONFIG_STA_SUPPORT //

#include    "md4.h"
#include 	"md5.h"

#include	"iot_sha2.h"
#include	"hmac.h"
#include	"dh_key.h"
#include	"evp_enc.h"
#include "rtmp_ckipmic.h"
#ifdef WSC_INCLUDED
// WSC security code
#include	"iot_sha2.h"
#include	"hmac.h"
#include	"dh_key.h"
#include	"evp_enc.h"

#include    "wsc_rom.h"
#include    "wsc_tlv_rom.h"
#endif // WSC_INCLUDE_HEADER //

#ifdef TIMER32K_PS
#include    "timer32k_ctrl.h"
#endif /* TIMER32K_PS */

#ifdef PS_ENHANCE
#include "ps_enhance.h"
#endif /* PS_ENHANCE */

#include "action.h"
#include "mac_rom.h"

#include "iot_rom_api_private.h"

#ifdef IOT_CFG80211_SUPPORT
#include "cfg80211extr.h"
#include "cfg80211_cmm.h"
#include "rtmp_cmd.h"
#include "rt_os_net.h"
#endif /* IOT_CFG80211_SUPPORT */

#ifdef HAL_LAYER_WRAPPER
#include "Wifi_hal_wrapper_fn.h"
#endif
#include "iot_lib.h"
#ifdef UNPATCH_N9_PATCHES
#include "rom/patch_config.h"
#endif
#include "public_rom_api.h"
#endif	// __RT_CONFIG_H__

