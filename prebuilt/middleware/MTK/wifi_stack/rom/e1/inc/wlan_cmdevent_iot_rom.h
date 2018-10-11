/*
** $Id: //Department/DaVinci/TRUNK/MT6620_WiFi_Firmware/mcu/wifi/include/mgmt/wlan_cmdevent.h#66 $
*/

/*! \file   "wlan_cmdevent.h"
    \brief  This file contains the declairation of CMD processing routines.

    This file contains the declairation of CMD processing routines, EVENT handling
    functions and related data structures.
*/

/*******************************************************************************
* Copyright (c) 2007 MediaTek Inc.
*
* All rights reserved. Copying, compilation, modification, distribution
* or any other use whatsoever of this material is strictly prohibited
* except in accordance with a Software License Agreement with
* MediaTek Inc.
********************************************************************************
*/

/*******************************************************************************
* LEGAL DISCLAIMER
*
* BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND
* AGREES THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK
* SOFTWARE") RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE
* PROVIDED TO BUYER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY
* DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT
* LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE
* ANY WARRANTY WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY
* WHICH MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK
* SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY
* WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE
* FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION OR TO
* CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
* BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
* LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL
* BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT
* ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
* BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
* WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT
* OF LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING
* THEREOF AND RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN
* FRANCISCO, CA, UNDER THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE
* (ICC).
********************************************************************************
*/

/*
** $Log: wlan_cmdevent.h $
**
** 06 14 2015 raju.singh
** [BORA00004254] [MT7637E2][IOT][FW]
** WPA2PSK security verified
 *
 * 05 21 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Remove RAM header file dependancy in ROM code.
 * Created parallel header files for RAM files which were being used in ROM code.
**
** 04 29 2015 michael.rong
** [BORA00003729] 7637 ant ram code entry address modify
** [IOT] Support sdk v0.1 inband command
**
** 03 06 2015 litien.chang
** [BORA00003962] [MT7637]
** [WIFI] inband command response enhance
** 1. add generic event handle
** 2. enhance command's error handle
**
** 07 15 2014 tiger.hung
** [BORA00003241] [MT7636] sync MT7636 E1 MP source code
**
** 02 26 2014 chiahsuan.chuang
** [BORA00002977] [MT7636][WiFi]
** .add Restart download command/response flow.
**
** 02 10 2014 ted.huang
** [BORA00002807] [MT7636]
** merge new inband cmd from 7603.
**
** 02 05 2014 posh.sun
** [BORA00002806] [Wi-Fi][General] Add MT7636 Project Settings
** Add 11ac VHT Feature Option
**
** 01 11 2014 ted.huang
** [BORA00002807] [MT7636]
** Support extension inband command.
**
** 01 08 2014 ted.huang
** [BORA00002807] [MT7636]
** Merge 6630E2 RAM code.
**
** 12 29 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** add fwlog and debug msg (debug msg to host)
**
** 12 26 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** addd cmd for 80211AC & 5G to parsing NVRAM
**
** 12 12 2013 tsaiyuan.hsu
** [BORA00002911] MT6630 unified MAC RTT
** update rtt fsm.
**
** 12 03 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** update FW support 2 power set
**
** 12 01 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** add feature option
**
** 11 22 2013 puff.wen
** [BORA00002798] [MT6630][E1_DVT] Update Thermal/Voltage hal
** Move BA from FW to driver
**
** 10 01 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** the change for new ar
**
** 09 26 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** 1. Add RESOURCE_CONFIG CMD/EVENT
**
** 09 03 2013 tsaiyuan.hsu
** [BORA00002775] MT6630 unified MAC ROAMING
** add CMD_ID_SET_GTK_REKEY_DATA to sync driver.
**
** 09 03 2013 tsaiyuan.hsu
** [BORA00002775] MT6630 unified MAC ROAMING
** 1. add roaming module.
** 2. disable roaming fsm
**
** 08 26 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** 1.update Icap function by starting addr
** 2.open 5G band for test mode
**
** 08 20 2013 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** icap event
**
** 08 15 2013 cp.wu
** [BORA00002253] [MT6630 Wi-Fi][Driver][Firmware] Add NLO and timeout mechanism to SCN module
** enlarge  match_ssid_num to 16 for PNO support
**
** 08 13 2013 wh.su
** [BORA00002734] [MT6630][Wi-Fi][Firmware][ROM] Security code development
** Support the AP mode with security (TKIP, CCMP)
**
** 08 08 2013 cp.wu
** [BORA00002253] [MT6630 Wi-Fi][Driver][Firmware] Add NLO and timeout mechanism to SCN module
** change to a more compact data structure
**
** 08 08 2013 cp.wu
** [BORA00002253] [MT6630 Wi-Fi][Driver][Firmware] Add NLO and timeout mechanism to SCN module
** add capability to carry custom IE for scheduled scans
**
** 08 05 2013 terry.wu
** [BORA00002207] [MT6630 Wi-Fi] TXM & MQM Implementation
** 1. Enable AP mode BMC packet buffer function
** 2. Update STA_REC/BSS_INFO HW default basic rate by driver
**
** 08 02 2013 yarco.yang
** [BORA00002190] [MT6630]Develop emulation code
** Add Wi-Fi via STP related code (CFG_WIFISTP_SUPPORT)
**
** 07 23 2013 yuche.tsai
** [BORA00000343] [MT6620] Emulation For TX
** Update FW code for Hot-Spot feature.
**
** 07 22 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** Report the Add key done event for data port control
**
** 07 17 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** Fixed the BC key setting the wrong EID3, and adjust some security code
**
** 07 02 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** Refine BMC wlan table assign rule
**
** 06 18 2013 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Get MAC address by NIC_CAPABILITY command
**
** 06 14 2013 eddie.chen
** [BORA00002450] [WIFISYS][MT6630] New design for mt6630
** 1. Add initial rate seting. 2. Add more information in sta command 3. add VHT info
**
** 03 27 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** add the default key handle
**
** 03 22 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** Add some security add key procedure code
**
** 03 15 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** Modify some security code
**
** 03 13 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** add code.
**
** 03 12 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** Add some code for security
**
** 03 08 2013 wh.su
** [BORA00002213] [MT6630 Wi-Fi][Normal Fw] develop security module
** Modify code for mt6630
**
** 02 18 2013 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** New feature to remove all sta records by bssIndex
**
** 02 05 2013 yuche.tsai
** [BORA00000343] [MT6620] Emulation For TX
** P2P BSS info update for FW.
**
** 01 28 2013 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Sync CMD format
**
** 01 24 2013 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Add MBSS network type to sync driver
**
** 01 22 2013 cp.wu
** [BORA00002253] [MT6630 Wi-Fi][Driver][Firmware] Add NLO and timeout mechanism to SCN module
** update CMD/EVENT definition for including channel hints
**
** 01 21 2013 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Support 80/160 MHz bandwidth channel for channel privilege
**
** 01 18 2013 cp.wu
** [BORA00002253] [MT6630 Wi-Fi][Driver][Firmware] Add NLO and timeout mechanism to SCN module
** add scan machine for SCN/NLO module
**
** 01 16 2013 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Use ucBssIndex to replace eNetworkTypeIndex
**
** 11 19 2012 cp.wu
** [BORA00002253] [MT6630 Wi-Fi][Driver][Firmware] Add NLO and timeout mechanism to SCN module
** update SCAN command definition for specifying number of probe request frame..
**
** 11 06 2012 cp.wu
** [BORA00002253] [MT6630 Wi-Fi][Driver][Firmware] Add NLO and timeout mechanism to SCN module
** add interface for NLO and modified SCAN support.
**
** 10 22 2012 eason.tsai
** [BORA00002225] [MT6630 WiFi][RAM] develop
** update ram code for rlm
**
** 10 09 2012 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Update command format of basic config
**
** 10 09 2012 cm.chang
** [BORA00002184] [MT6630 Wi-Fi] Check-in draft FW
** Modify the handle about BSS active command with MAC address
*/

#ifndef _WLAN_CMDEVENT_H
#define _WLAN_CMDEVENT_H
#include "mac.h"
/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
#define PARAM_MAX_LEN_RATES_EX                  16

#define EVENT_PACKET_TYPE  			            (0xE000)


/* Define CMD ID from Host to firmware (v0.07) */
typedef enum _ENUM_CMD_ID_T {
    CMD_ID_TEST_CTRL            = 0x01, /* 0x01 (Set) */
    CMD_ID_BASIC_CONFIG,                /* 0x02 (Set) */
    CMD_ID_SCAN_REQ_V2,                 /* 0x03 (Set) */
    CMD_ID_NIC_POWER_CTRL,              /* 0x04 (Set) */
    CMD_ID_POWER_SAVE_MODE,             /* 0x05 (Set) */
    CMD_ID_LINK_ATTRIB,                 /* 0x06 (Set) */
    CMD_ID_ADD_REMOVE_KEY,              /* 0x07 (Set) */
    CMD_ID_DEFAULT_KEY_ID,              /* 0x08 (Set) */
    CMD_ID_INFRASTRUCTURE,              /* 0x09 (Set) */
    CMD_ID_SET_RX_FILTER,               /* 0x0a (Set) */
    CMD_ID_DOWNLOAD_BUF,                /* 0x0b (Set) */
    CMD_ID_WIFI_START,                  /* 0x0c (Set) */
    CMD_ID_CMD_BT_OVER_WIFI,            /* 0x0d (Set) */
    CMD_ID_SET_MEDIA_CHANGE_DELAY_TIME, /* 0x0e (Set) */
    CMD_ID_SET_DOMAIN_INFO,             /* 0x0f (Set) */
    CMD_ID_SET_IP_ADDRESS,              /* 0x10 (Set) */
    CMD_ID_BSS_ACTIVATE_CTRL,           /* 0x11 (Set) */
    CMD_ID_SET_BSS_INFO,                /* 0x12 (Set) */
    CMD_ID_UPDATE_STA_RECORD,           /* 0x13 (Set) */
    CMD_ID_REMOVE_STA_RECORD,           /* 0x14 (Set) */
    CMD_ID_INDICATE_PM_BSS_CREATED,     /* 0x15 (Set) */
    CMD_ID_INDICATE_PM_BSS_CONNECTED,   /* 0x16 (Set) */
    CMD_ID_INDICATE_PM_BSS_ABORT,       /* 0x17 (Set) */
    CMD_ID_UPDATE_BEACON_CONTENT,       /* 0x18 (Set) */
    CMD_ID_SET_BSS_RLM_PARAM,           /* 0x19 (Set) */
    CMD_ID_SCAN_REQ,                    /* 0x1a (Set) */
    CMD_ID_SCAN_CANCEL,                 /* 0x1b (Set) */
    CMD_ID_CH_PRIVILEGE,                /* 0x1c (Set) */
    CMD_ID_UPDATE_WMM_PARMS,            /* 0x1d (Set) */
    CMD_ID_SET_WMM_PS_TEST_PARMS,       /* 0x1e (Set) */
    CMD_ID_TX_AMPDU,                    /* 0x1f (Set) */
    CMD_ID_ADDBA_REJECT,                /* 0x20 (Set) */
    CMD_ID_SET_PS_PROFILE_ADV,          /* 0x21 (Set) */
    CMD_ID_SET_RAW_PATTERN,             /* 0x22 (Set) */
    CMD_ID_CONFIG_PATTERN_FUNC,         /* 0x23 (Set) */
    CMD_ID_SET_TX_PWR,                  /* 0x24 (Set) */
    CMD_ID_SET_PWR_PARAM,               /* 0x25 (Set) */
    CMD_ID_P2P_ABORT,                   /* 0x26 (Set) */
    CMD_ID_SET_TX_EXTEND_PWR,           /* 0x27 (Set) */

    /* SLT commands */
    CMD_ID_RANDOM_RX_RESET_EN   = 0x2C, /* 0x2C (Set ) */
    CMD_ID_RANDOM_RX_RESET_DE   = 0x2D, /* 0x2D (Set ) */
    CMD_ID_SAPP_EN              = 0x2E, /* 0x2E (Set ) */
    CMD_ID_SAPP_DE              = 0x2F, /* 0x2F (Set ) */

    CMD_ID_ROAMING_TRANSIT      = 0x30, /* 0x30 (Set) */
    CMD_ID_SET_PHY_PARAM,               /* 0x31 (Set) */
    CMD_ID_SET_NOA_PARAM,               /* 0x32 (Set) */
    CMD_ID_SET_OPPPS_PARAM,             /* 0x33 (Set) */
    CMD_ID_SET_UAPSD_PARAM,             /* 0x34 (Set) */
    CMD_ID_SET_SIGMA_STA_SLEEP,         /* 0x35 (Set) */
    CMD_ID_SET_EDGE_TXPWR_LIMIT,        /* 0x36 (Set) */
    CMD_ID_SET_DEVICE_MODE,             /* 0x37 (Set) */
    CMD_ID_SET_TXPWR_CTRL,              /* 0x38 (Set) */
    CMD_ID_SET_AUTOPWR_CTRL,            /* 0x39 (Set) */
    CMD_ID_SET_WFD_CTRL,                /* 0x3a (Set) */
    CMD_ID_SET_NLO_REQ,                 /* 0x3b (Set) */
    CMD_ID_SET_NLO_CANCEL,              /* 0x3c (Set) */
    CMD_ID_SET_GTK_REKEY_DATA,          /* 0x3d (Set) */
    CMD_ID_ROAMING_CONTROL,             /* 0x3e (Set) */
	CMD_ID_RESET_BA_SCOREBOARD	= 0x3f, /* 0x3f (Set) */
	CMD_ID_RTT_REQ = 0x40,
    CMD_ID_SET_EDGE_TXPWR_LIMIT_5G =0x41,/* 0x41 (Set) */
    CMD_ID_SET_CHANNEL_PWR_OFFSET,          /* 0x42 (Set) */
    CMD_ID_SET_80211AC_TX_PWR,          /* 0x43 (Set) */
    CMD_ID_SET_PATH_COMPASATION,        /* 0x44 (Set) */

    CMD_ID_SET_SLTINFO = 0x50,          /* 0x50 (Set) */
    CMD_ID_UART_ACK,                    /* 0x51 (Set) */
    CMD_ID_UART_NAK,                    /* 0x52 (Set) */
    CMD_ID_GET_CHIPID,

    CMD_ID_GET_NIC_CAPABILITY   = 0x80, /* 0x80 (Query) */
    CMD_ID_GET_LINK_QUALITY,            /* 0x81 (Query) */
    CMD_ID_GET_STATISTICS,              /* 0x82 (Query) */

    CMD_ID_ACCESS_REG           = 0xc0, /* 0xc0 (Set / Query) */
    CMD_ID_MAC_MCAST_ADDR,              /* 0xc1 (Set / Query) */
    CMD_ID_802_11_PMKID,                /* 0xc2 (Set / Query) */
    CMD_ID_ACCESS_EEPROM,               /* 0xc3 (Set / Query) */
    CMD_ID_SW_DBG_CTRL,                 /* 0xc4 (Set / Query) */
    CMD_ID_SEC_CHECK,                   /* 0xc5 (Set / Query) */
    CMD_ID_DUMP_MEM,                    /* 0xc6 (Query) */
    CMD_ID_RESOURCE_CONFIG,             /* 0xc7 (Set / Query) */

	CMD_ID_IOT_OWN_MAC_ADDR = 0xd0, 	/* 0xd0 (Set / Query) */
	CMD_ID_IOT_SSID,					/* 0xd1 (Set / Query) */
	CMD_ID_IOT_BSSID,					/* 0xd2 (Set / Query) */
	CMD_ID_IOT_BW,						/* 0xd3 (Set / Query) */
	CMD_ID_IOT_SET_DISCONNECT,			/* 0xd4 (Set) */
	CMD_ID_IOT_GET_LINK_STATUS, 		/* 0xd5 (Query) */
	CMD_ID_IOT_GET_PROFILE, 			/* 0xd6 (Query) */
	CMD_ID_IOT_START_SCAN,				/* 0xd7 (Set) */
	CMD_ID_IOT_STOP_SCAN,				/* 0xd8 (Set) */
	CMD_ID_IOT_GET_SCAN_LIST,			/* 0xd9 (Query) */
	CMD_ID_IOT_SET_RX_RILTER,			/* 0xda (Set) */
	CMD_ID_IOT_CHANNEL, 				/* 0xdb (Set / Query) */


    CMD_ID_SET_RDD_CH               = 0xE1,

    CMD_ID_LAYER_0_EXT_MAGIC_NUM	= 0xED,	/* magic number for Extending MT6630 original CMD header  */
    CMD_ID_LAYER_1_MAGIC_NUM		= 0xEE,	/* magic number for backward compatible with MT76xx CMD  */
    CMD_ID_INIT_CMD_WIFI_RESTART    = 0xEF, /* 0xef (reload firmware use) wayne_note 2013.09.26 */


    CMD_ID_SET_BWCS                 = 0xF1,
    CMD_ID_SET_OSC                  = 0xF2

} ENUM_CMD_ID_T, *P_ENUM_CMD_ID_T;

/* Define EVENT ID from firmware to Host (v0.09) */
typedef enum _ENUM_EVENT_ID_T {
    //EVENT_ID_NIC_CAPABILITY     = 0x01, /* 0x01 (Query) */
    EVENT_ID_INIT_EVENT_CMD_RESULT  = 0x01, /* 0x01 (Generic Event) */
    EVENT_ID_LINK_QUALITY,              /* 0x02 (Query / Unsolicited) */
    EVENT_ID_STATISTICS,                /* 0x03 (Query) */
    EVENT_ID_MIC_ERR_INFO,              /* 0x04 (Unsolicited) */
    EVENT_ID_ACCESS_REG,                /* 0x05 (Query - CMD_ID_ACCESS_REG) */
    EVENT_ID_ACCESS_EEPROM,             /* 0x06 (Query - CMD_ID_ACCESS_EEPROM) */
    EVENT_ID_SLEEPY_INFO,               /* 0x07 (Unsolicited) */
    EVENT_ID_BT_OVER_WIFI,              /* 0x08 (Unsolicited) */
    EVENT_ID_TEST_STATUS,               /* 0x09 (Query - CMD_ID_TEST_CTRL) */
    EVENT_ID_RX_ADDBA,                  /* 0x0a (Unsolicited) */
    EVENT_ID_RX_DELBA,                  /* 0x0b (Unsolicited) */
    EVENT_ID_ACTIVATE_STA_REC,          /* 0x0c (Response) */
    EVENT_ID_SCAN_DONE,                 /* 0x0d (Unsoiicited) */
    EVENT_ID_RX_FLUSH,                  /* 0x0e (Unsolicited) */
    EVENT_ID_TX_DONE,                   /* 0x0f (Unsolicited) */
    EVENT_ID_CH_PRIVILEGE,              /* 0x10 (Unsolicited) */
    EVENT_ID_BSS_ABSENCE_PRESENCE,      /* 0x11 (Unsolicited) */
    EVENT_ID_STA_CHANGE_PS_MODE,        /* 0x12 (Unsolicited) */
    EVENT_ID_BSS_BEACON_TIMEOUT,        /* 0x13 (Unsolicited) */
    EVENT_ID_UPDATE_NOA_PARAMS,         /* 0x14 (Unsolicited) */
    EVENT_ID_AP_OBSS_STATUS,            /* 0x15 (Unsolicited) */
    EVENT_ID_STA_UPDATE_FREE_QUOTA,     /* 0x16 (Unsolicited) */
    EVENT_ID_SW_DBG_CTRL,               /* 0x17 (Query - CMD_ID_SW_DBG_CTRL) */
    EVENT_ID_ROAMING_STATUS,            /* 0x18 (Unsolicited) */
    EVENT_ID_STA_AGING_TIMEOUT,         /* 0x19 (Unsolicited) */
    EVENT_ID_SEC_CHECK_RSP,             /* 0x1a (Query - CMD_ID_SEC_CHECK) */
    EVENT_ID_SEND_DEAUTH,               /* 0x1b (Unsolicited) */
    EVENT_ID_UPDATE_RDD_STATUS,         /* 0x1c (Unsolicited) */
    EVENT_ID_UPDATE_BWCS_STATUS,        /* 0x1d (Unsolicited) */
    EVENT_ID_UPDATE_BCM_DEBUG,          /* 0x1e (Unsolicited) */
    EVENT_ID_RX_ERR,                    /* 0x1f (Unsolicited) */
    EVENT_ID_DUMP_MEM,                  /* 0x20 (Query - CMD_ID_DUMP_MEM) */
    EVENT_ID_STA_STATISTICS,            /* 0x21 (Query ) */
    EVENT_ID_STA_STATISTICS_UPDATE,     /* 0x22 (Unsolicited) */
    EVENT_ID_NLO_DONE,                  /* 0x23 (Unsoiicited) */
    EVENT_ID_ADD_PKEY_DONE,             /* 0x24 (Unsoiicited) */
    EVENT_ID_ICAP_DONE,                 /* 0x25 (Unsoiicited) */
    EVENT_ID_RESOURCE_CONFIG = 0x26,    /* 0x26 (Query - CMD_ID_RESOURCE_CONFIG) */
    EVENT_ID_DEBUG_MSG = 0x27,          /* 0x27 (Unsoiicited) */

    EVENT_ID_UART_ACK = 0x40,           /* 0x40 (Unsolicited) */
    EVENT_ID_UART_NAK,                  /* 0x41 (Unsolicited) */
    EVENT_ID_GET_CHIPID,                /* 0x42 (Query - CMD_ID_GET_CHIPID) */
    EVENT_ID_SLT_STATUS,                /* (Query - CMD_ID_SET_SLTINFO) */

	/* 0x50~0x80 reserved for IOT ? */
	EVENT_ID_IOT_SCAN_LIST = 0x50,			   /* 0x50 (Unsoiicited) */
	EVENT_ID_IOT_CF_CREDENTIAL = 0x55,			/* Save ConfigFree Credentials and Reboot */
	EVENT_ID_IOT_WLAN_EVT = 0x60,				/* 0x60 (Unsoiicited) */
	EVENT_ID_IOT_N9_CONSOLE_LOG_EVT = 0x80,

    EVENT_ID_LAYER_0_EXT_MAGIC_NUM	= 0xED,	    /* magic number for Extending MT6630 original EVENT header  */
    EVENT_ID_LAYER_1_MAGIC_NUM		= 0xEE,	    /* magic number for backward compatible with MT76xx EVENT  */

    EVENT_ID_MAX_NUM
} ENUM_EVENT_ID_T, *P_ENUM_EVENT_ID_T;

#define DISCONNECT_REASON_CODE_RESERVED         0
#define DISCONNECT_REASON_CODE_RADIO_LOST       1
#define DISCONNECT_REASON_CODE_DEAUTHENTICATED  2
#define DISCONNECT_REASON_CODE_DISASSOCIATED    3
#define DISCONNECT_REASON_CODE_NEW_CONNECTION   4


/* Define bandwidth, band, channel and SCO configurations from host command,
 * whose CMD ID is 0x13.
 */
#define CONFIG_BW_20_40M            0
#define CONFIG_BW_20M               1   /* 20MHz only */
#define CONFIG_BW_20_40_80M         2
#define CONFIG_BW_20_40_80_160M     3
#define CONFIG_BW_20_40_80_8080M    4

/* Band definition in CMD EVENT */
#define CMD_BAND_NULL               0
#define CMD_BAND_2G4                1
#define CMD_BAND_5G                 2


/* Action field in structure CMD_CH_PRIVILEGE_T */
#define CMD_CH_ACTION_REQ           0
#define CMD_CH_ACTION_ABORT         1

/* Status field in structure EVENT_CH_PRIVILEGE_T */
#define EVENT_CH_STATUS_GRANT       0
#define EVENT_CH_STATUS_REJECT      1
#define EVENT_CH_STATUS_RECOVER     2

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
/*----------------------------------------------------------------------------*/
/* Parameters of User Configuration which match to NDIS5.1                    */
/*----------------------------------------------------------------------------*/
typedef INT_32          PARAM_RSSI;

/* NDIS_MEDIA_STATE */
typedef enum _ENUM_PARAM_MEDIA_STATE_T {
    MEDIA_STATE_DISCONNECTED = 0,
    MEDIA_STATE_CONNECTED
} ENUM_PARAM_MEDIA_STATE, *P_ENUM_PARAM_MEDIA_STATE;

/* NDIS_802_11_RATES_EX */
typedef UINT_8  PARAM_RATES_EX[PARAM_MAX_LEN_RATES_EX];

/* NDIS_802_11_NETWORK_TYPE */
typedef enum _ENUM_PARAM_PHY_NET_TYPE_T {
    PHY_NET_TYPE_FH,
    PHY_NET_TYPE_DS,
    PHY_NET_TYPE_OFDM5,
    PHY_NET_TYPE_OFDM24,
    PHY_NET_TYPE_AUTOMODE,
    PHY_NET_TYPE_NUM                    /*!< Upper bound, not real case */
} ENUM_PARAM_PHY_NET_TYPE_T, *P_ENUM_PARAM_PHY_NET_TYPE_T;

/* NDIS_802_11_NETWORK_INFRASTRUCTURE */
typedef enum _ENUM_PARAM_OP_MODE_T {
    NET_TYPE_IBSS = 0,                  /*!< Try to merge/establish an AdHoc, do periodic SCAN for merging. */
    NET_TYPE_INFRA,                     /*!< Try to join an Infrastructure, do periodic SCAN for joining. */
    NET_TYPE_AUTO_SWITCH,               /*!< Try to join an Infrastructure, if fail then try to merge or
                                         establish an AdHoc, do periodic SCAN for joining or merging. */
    NET_TYPE_NUM // 3
} ENUM_PARAM_OP_MODE_T, *P_ENUM_PARAM_OP_MODE_T;

/* NDIS_802_11_AUTHENTICATION_MODE */
typedef enum _ENUM_PARAM_AUTH_MODE_T
{
    AUTH_MODE_OPEN,                     /*!< Open system */
    AUTH_MODE_SHARED,                   /*!< Shared key */
    AUTH_MODE_AUTO_SWITCH,              /*!< Either open system or shared key */
    AUTH_MODE_WPA,
    AUTH_MODE_WPA_PSK,
    AUTH_MODE_WPA_NONE,                 /*!< For Ad hoc */
    AUTH_MODE_WPA2,
    AUTH_MODE_WPA2_PSK,
    AUTH_MODE_NUM                       /*!< Upper bound, not real case */
} ENUM_PARAM_AUTH_MODE_T, *P_ENUM_PARAM_AUTH_MODE_T;

/* NDIS_802_11_ENCRYPTION_STATUS */
typedef enum _ENUM_WEP_STATUS_T
{
    ENUM_WEP_ENABLED,
    ENUM_ENCRYPTION1_ENABLED = ENUM_WEP_ENABLED,
    ENUM_WEP_DISABLED,
    ENUM_ENCRYPTION_DISABLED = ENUM_WEP_DISABLED,
    ENUM_WEP_KEY_ABSENT,
    ENUM_ENCRYPTION1_KEY_ABSENT = ENUM_WEP_KEY_ABSENT,
    ENUM_WEP_NOT_SUPPORTED,
    ENUM_ENCRYPTION_NOT_SUPPORTED = ENUM_WEP_NOT_SUPPORTED,
    ENUM_ENCRYPTION2_ENABLED,
    ENUM_ENCRYPTION2_KEY_ABSENT,
    ENUM_ENCRYPTION3_ENABLED,
    ENUM_ENCRYPTION3_KEY_ABSENT
} ENUM_PARAM_ENCRYPTION_STATUS_T, *P_ENUM_PARAM_ENCRYPTION_STATUS_T;

/*----------------------------------------------------------------------------*/
/* Parameters of User Configuration                                           */
/*----------------------------------------------------------------------------*/
typedef enum _ENUM_PARAM_CONNECTION_POLICY_T {
    CONNECT_BY_SSID_BEST_RSSI = 0,
    CONNECT_BY_SSID_GOOD_RSSI_MIN_CH_LOAD,
    CONNECT_BY_SSID_ANY, /* NOTE(Kevin): Needed by WHQL */
    CONNECT_BY_CUSTOMIZED_RULE /* NOTE(Kevin): TBD */
} ENUM_PARAM_CONNECTION_POLICY_T, *P_ENUM_PARAM_CONNECTION_POLICY_T;

typedef enum _ENUM_PARAM_PREAMBLE_TYPE_T {
    PREAMBLE_TYPE_LONG = 0,
    PREAMBLE_TYPE_SHORT,
    PREAMBLE_TYPE_AUTO                  /*!< Try preamble short first, if fail tray preamble long. */
} ENUM_PARAM_PREAMBLE_TYPE_T, *P_ENUM_PARAM_PREAMBLE_TYPE_T;

/* This is enum defined for user to select a phy config listed in combo box */
typedef enum _ENUM_PARAM_PHY_CONFIG_T {
    PHY_CONFIG_802_11ABG = 0,           /*!< Can associated with 802.11abg AP but without n capability, Scan dual band. */
    PHY_CONFIG_802_11BG,                /*!< Can associated with 802_11bg AP, Scan single band and not report 5G BSSs. */
    PHY_CONFIG_802_11G,                 /*!< Can associated with 802_11g only AP, Scan single band and not report 5G BSSs. */
    PHY_CONFIG_802_11A,                 /*!< Can associated with 802_11a only AP, Scan single band and not report 2.4G BSSs. */
    PHY_CONFIG_802_11B,                 /*!< Can associated with 802_11b only AP, Scan single band and not report 5G BSSs. */
    PHY_CONFIG_802_11ABGN,              /*!< Can associated with 802.11abgn AP, Scan dual band. */
    PHY_CONFIG_802_11BGN,               /*!< Can associated with 802_11bgn AP, Scan single band and not report 5G BSSs. */
    PHY_CONFIG_802_11AN,                /*!< Can associated with 802_11an AP, Scan single band and not report 2.4G BSSs. */
    PHY_CONFIG_802_11GN,                /*!< Can associated with 802_11gn AP, Scan single band and not report 5G BSSs. */
    PHY_CONFIG_802_11ANAC,               /*!< Can associated with 802_11an AP, Scan single band and not report 2.4G BSSs. */
    PHY_CONFIG_802_11ABGNAC,             /*!< Can associated with 802_11abgnac AP, Scan dual band */
    PHY_CONFIG_802_11BGNAC,                /*!< Can associated with 802_11bgnac AP, Scan single band and not report 5G BSSs. */
    PHY_CONFIG_NUM // 12
} ENUM_PARAM_PHY_CONFIG_T, *P_ENUM_PARAM_PHY_CONFIG_T;

/* This is enum defined for user to select an AdHoc Mode */
typedef enum _ENUM_PARAM_AD_HOC_MODE_T {
    AD_HOC_MODE_11B = 0,                /*!< Create 11b IBSS if we support 802.11abg/802.11bg. */
    AD_HOC_MODE_MIXED_11BG,             /*!< Create 11bg mixed IBSS if we support 802.11abg/802.11bg/802.11g. */
    AD_HOC_MODE_11G,                    /*!< Create 11g only IBSS if we support 802.11abg/802.11bg/802.11g. */
    AD_HOC_MODE_11A,                    /*!< Create 11a only IBSS if we support 802.11abg. */
    AD_HOC_MODE_NUM // 4
} ENUM_PARAM_AD_HOC_MODE_T, *P_ENUM_PARAM_AD_HOC_MODE_T;

/* This is enum defined for user to select an AP Mode */
typedef enum _ENUM_PARAM_AP_MODE_T {
    AP_MODE_11B = 0,                /*!< Create 11b BSS if we support 802.11abg/802.11bg. */
    AP_MODE_MIXED_11BG,             /*!< Create 11bg mixed BSS if we support 802.11abg/802.11bg/802.11g. */
    AP_MODE_11G,                    /*!< Create 11g only BSS if we support 802.11abg/802.11bg/802.11g. */
    AP_MODE_11G_P2P,                /*!< Create 11g only BSS for P2P if we support 802.11abg/802.11bg/802.11g. */
    AP_MODE_11A,                    /*!< Create 11a only BSS if we support 802.11abg. */
    AP_MODE_NUM // 4
} ENUM_PARAM_AP_MODE_T, *P_ENUM_PARAM_AP_MODE_T;


/*----------------------------------------------------------------------------*/
/* CMD Packets                                                                */
/*----------------------------------------------------------------------------*/
typedef struct _CMD_BUILD_CONNECTION {
    UINT_8      ucInfraMode;
    UINT_8      ucAuthMode;
    UINT_8      ucEncryptStatus;
    UINT_8      ucSsidLen;
    UINT_8      aucSsid[32];
    UINT_8      aucBssid[6];

    /* The following parameters are for Ad-hoc network */
    UINT_16     u2BeaconPeriod;
    UINT_16     u2ATIMWindow;
    UINT_8      ucJoinOnly;
    UINT_8      ucReserved;
    UINT_32     u4FreqInKHz;

} CMD_BUILD_CONNECTION, *P_CMD_BUILD_CONNECTION;

#if 0
typedef struct _CMD_802_11_KEY {
    UINT_8      ucAddRemove;
    UINT_8      ucTxKey;   /* 1 : Tx key */
    UINT_8      ucKeyType; /* 0 : group Key, 1 : Pairwise key */
    UINT_8      ucIsAuthenticator; /* 1 : authenticator */
    UINT_8      aucPeerAddr[6];
    UINT_8      ucBssIndex; /* the BSS index */
    UINT_8      ucAlgorithmId;
    UINT_8      ucKeyId;
    UINT_8      ucKeyLen;
    UINT_8      ucWlanIndex;
    UINT_8      ucReverved;
    UINT_8      aucKeyMaterial[32];
    UINT_8      aucKeyRsc[16];
} CMD_802_11_KEY, *P_CMD_802_11_KEY;
#endif

typedef struct _CMD_DEFAULT_KEY_ID {
    UINT_8      ucBssIdx;
    UINT_8      ucKeyId;
    UINT_8      ucUnicast;
    UINT_8      ucMulticast;
} CMD_DEFAULT_KEY_ID, *P_CMD_DEFAULT_KEY_ID;


/* CMD for PMKID and related structure */
typedef UINT_8   PARAM_PMKID_VALUE[16];

typedef struct _PARAM_BSSID_INFO_T
{
    UINT_8              aucBssid[MAC_ADDR_LEN];    /* MAC address */
    PARAM_PMKID_VALUE   arPMKID;
} PARAM_BSSID_INFO_T, *P_PARAM_BSSID_INFO_T;

typedef struct _IPV4_NETWORK_ADDRESS_T {
  UINT_8 aucIpAddr[4];
} IPV4_NETWORK_ADDRESS_T, *P_IPV4_NETWORK_ADDRESS_T;

typedef struct _CMD_SET_NETWORK_ADDRESS_LIST_T {
  UINT_32 u4AddressCount;
  IPV4_NETWORK_ADDRESS_T arNetAddress[1];
} CMD_SET_NETWORK_ADDRESS_LIST_T, *P_CMD_SET_NETWORK_ADDRESS_LIST_T;

typedef struct _CMD_BSS_ACTIVATE_CTRL {
    UINT_8      ucBssIndex;
    UINT_8      ucActive;
    UINT_8      ucNetworkType;
    UINT_8      ucOwnMacAddrIndex;
    UINT_8      aucBssMacAddr[6];
    UINT_8      ucBMCWlanIndex;
    UINT_8      ucReserved;
} CMD_BSS_ACTIVATE_CTRL, *P_CMD_BSS_ACTIVATE_CTRL;

typedef struct _CMD_SET_BSS_RLM_PARAM_T {
    UINT_8      ucBssIndex;
    UINT_8      ucRfBand;
    UINT_8      ucPrimaryChannel;
    UINT_8      ucRfSco;
    UINT_8      ucErpProtectMode;
    UINT_8      ucHtProtectMode;
    UINT_8      ucGfOperationMode;
    UINT_8      ucTxRifsMode;
    UINT_16     u2HtOpInfo3;
    UINT_16     u2HtOpInfo2;
    UINT_8      ucHtOpInfo1;
    UINT_8      ucUseShortPreamble;
    UINT_8      ucUseShortSlotTime;
    UINT_8      ucVhtChannelWidth;          /* Posh 20140205, To support 80/160MHz bandwidth */
    UINT_8      ucVhtChannelFrequencyS1;    /* Posh 20140205, To support 80/160MHz bandwidth */
    UINT_8      ucVhtChannelFrequencyS2;    /* Posh 20140205, To support 80/160MHz bandwidth */
    UINT_16     u2VhtBasicMcsSet;           /* Posh 20140205, To support 80/160MHz bandwidth */
} CMD_SET_BSS_RLM_PARAM_T, *P_CMD_SET_BSS_RLM_PARAM_T;

typedef struct _CMD_SET_BSS_INFO {
    UINT_8  ucBssIndex;
    UINT_8  ucConnectionState;
    UINT_8  ucCurrentOPMode;
    UINT_8  ucSSIDLen;
    UINT_8  aucSSID[32];
    UINT_8  aucBSSID[6];
    UINT_8  ucIsQBSS;
    UINT_8  ucReserved1;
    UINT_16 u2OperationalRateSet;
    UINT_16 u2BSSBasicRateSet;
    UINT_8  ucStaRecIdxOfAP;
    UINT_16 u2HwDefaultFixedRateCode;
    UINT_8  ucNonHTBasicPhyType; /* For Slot Time and CWmin */
    UINT_8  ucAuthMode;
    UINT_8  ucEncStatus;
    UINT_8  ucPhyTypeSet;
    UINT_8  ucWapiMode;
    UINT_8  ucIsApMode;
    UINT_8  ucBMCWlanIndex;
    UINT_8  ucHiddenSsidMode;
    UINT_8  aucRsv[1];
    UINT_32 u4PrivateData;
    CMD_SET_BSS_RLM_PARAM_T rBssRlmParam;
} CMD_SET_BSS_INFO, *P_CMD_SET_BSS_INFO;

typedef struct _CMD_UPDATE_STA_RECORD_T {
    UINT_8  ucStaIndex;
    UINT_8  ucStaType;
    UINT_8  aucMacAddr[MAC_ADDR_LEN]; /* This field should assign at create and keep consistency for update usage */

    UINT_16 u2AssocId;
    UINT_16 u2ListenInterval;
    UINT_8  ucBssIndex;               /* This field should assign at create and keep consistency for update usage */
    UINT_8  ucDesiredPhyTypeSet;
    UINT_16 u2DesiredNonHTRateSet;

    UINT_16 u2BSSBasicRateSet;
    UINT_8  ucIsQoS;
    UINT_8  ucIsUapsdSupported;
    UINT_8  ucStaState;
    UINT_8  ucMcsSet;
    UINT_8  ucSupMcs32;
    UINT_8  aucReserved1[1];

    UINT_8  aucRxMcsBitmask[10];
    UINT_16 u2RxHighestSupportedRate;
    UINT_32 u4TxRateInfo;

    UINT_16 u2HtCapInfo;
    UINT_16 u2HtExtendedCap;
    UINT_32 u4TxBeamformingCap;

    UINT_8  ucAmpduParam;
    UINT_8  ucAselCap;
    UINT_8  ucRCPI;
    UINT_8  ucNeedResp;
    UINT_8  ucUapsdAc;                /* b0~3: Trigger enabled, b4~7: Delivery enabled */
    UINT_8  ucUapsdSp;                /* 0: all, 1: max 2, 2: max 4, 3: max 6 */
    UINT_8  ucWlanIndex;              /* This field should assign at create and keep consistency for update usage */
    UINT_8  ucBMCWlanIndex;           /* This field should assign at create and keep consistency for update usage */

    UINT_32 u4VhtCapInfo;
    UINT_16 u2VhtRxMcsMap;
    UINT_16 u2VhtRxHighestSupportedDataRate;
    UINT_16 u2VhtTxMcsMap;
    UINT_16 u2VhtTxHighestSupportedDataRate;
    UINT_8  ucRtsPolicy;              /* 0: auto 1: Dynamic 2: Static 3: Normal 7: WoRts */
    UINT_8  aucReserved2[1];

    UINT_8  ucTrafficDataType;
    UINT_8  ucTxGfMode;               /* ENUM_FEATURE_OPTION_T */
    UINT_8  ucTxSgiMode;              /* ENUM_FEATURE_OPTION_T */
    UINT_8  ucTxStbcMode;             /* ENUM_FEATURE_OPTION_T unused */
    UINT_16 u2HwDefaultFixedRateCode;
    UINT_8  aucReserved3[2];
    UINT_32 u4FixedPhyRate;           /* 0: desable BIT(31)==1,BITS(0,15) rate code */
    UINT_16 u2MaxLinkSpeed;           /* 0: unlimit ohter: unit is 0.5 Mbps */
    UINT_16 u2MinLinkSpeed;

    UINT_32  u4Flags;
    UINT_8  aucReserved4[32];

} CMD_UPDATE_STA_RECORD_T, *P_CMD_UPDATE_STA_RECORD_T;

typedef struct _CMD_REMOVE_STA_RECORD_T {
    UINT_8  ucActionType;
    UINT_8  ucStaIndex;
    UINT_8  ucBssIndex;
    UINT_8  aucReserved[1];
} CMD_REMOVE_STA_RECORD_T, *P_CMD_REMOVE_STA_RECORD_T;

typedef struct _CMD_INDICATE_PM_BSS_CREATED_T {
    UINT_8  ucBssIndex;
    UINT_8  ucDtimPeriod;
    UINT_16 u2BeaconInterval;
    UINT_16 u2AtimWindow;
    UINT_8  aucReserved[2];
} CMD_INDICATE_PM_BSS_CREATED, *P_CMD_INDICATE_PM_BSS_CREATED;

typedef struct _CMD_INDICATE_PM_BSS_CONNECTED_T {
    UINT_8  ucBssIndex;
    UINT_8  ucDtimPeriod;
    UINT_16 u2AssocId;
    UINT_16 u2BeaconInterval;
    UINT_16 u2AtimWindow;
    UINT_8  fgIsUapsdConnection;
    UINT_8  ucBmpDeliveryAC;
    UINT_8  ucBmpTriggerAC;
    UINT_8  aucReserved[1];
} CMD_INDICATE_PM_BSS_CONNECTED, *P_CMD_INDICATE_PM_BSS_CONNECTED;

typedef struct _CMD_INDICATE_PM_BSS_ABORT {
    UINT_8  ucBssIndex;
    UINT_8  aucReserved[3];
} CMD_INDICATE_PM_BSS_ABORT, *P_CMD_INDICATE_PM_BSS_ABORT;

typedef struct _CMD_BEACON_TEMPLATE_UPDATE {
    UINT_8    ucUpdateMethod; // 0: update randomly, 1: update all, 2: delete all (1 and 2 will update directly without search)
    UINT_8    ucBssIndex;
    UINT_8    aucReserved[2];
    UINT_16   u2Capability;
    UINT_16   u2IELen;
    UINT_8    aucIE[MAX_IE_LENGTH];
} CMD_BEACON_TEMPLATE_UPDATE, *P_CMD_BEACON_TEMPLATE_UPDATE;

typedef struct _CMD_SET_WMM_PS_TEST_STRUC_T {
    UINT_8  ucBssIndex;
    UINT_8  bmfgApsdEnAc;           /* b0~3: trigger-en AC0~3. b4~7: delivery-en AC0~3 */
    UINT_8  ucIsEnterPsAtOnce;      /* enter PS immediately without 5 second guard after connected */
    UINT_8  ucIsDisableUcTrigger;   /* not to trigger UC on beacon TIM is matched (under U-APSD) */
} CMD_SET_WMM_PS_TEST_STRUC_T, *P_CMD_SET_WMM_PS_TEST_STRUC_T;

typedef struct _CMD_CUSTOM_NOA_PARAM_STRUC_T {
    UINT_32  u4NoaDurationMs;
    UINT_32  u4NoaIntervalMs;
    UINT_32  u4NoaCount;
} CMD_CUSTOM_NOA_PARAM_STRUC_T, *P_CMD_CUSTOM_NOA_PARAM_STRUC_T;

typedef struct _CMD_CUSTOM_OPPPS_PARAM_STRUC_T {
    UINT_32  u4CTwindowMs;
} CMD_CUSTOM_OPPPS_PARAM_STRUC_T, *P_CMD_CUSTOM_OPPPS_PARAM_STRUC_T;

typedef struct _CMD_CUSTOM_UAPSD_PARAM_STRUC_T {
    UINT_8  fgEnAPSD;
    UINT_8  fgEnAPSD_AcBe;
    UINT_8  fgEnAPSD_AcBk;
    UINT_8  fgEnAPSD_AcVo;
    UINT_8  fgEnAPSD_AcVi;
    UINT_8  ucMaxSpLen;
    UINT_8  aucResv[2];
} CMD_CUSTOM_UAPSD_PARAM_STRUC_T, *P_CMD_CUSTOM_UAPSD_PARAM_STRUC_T;


/* Definition for CHANNEL_INFO.ucBand:
 * 0:       Reserved
 * 1:       BAND_2G4
 * 2:       BAND_5G
 * Others:  Reserved
 */
typedef struct _CHANNEL_INFO_T {
    UINT_8  ucBand;
    UINT_8  ucChannelNum;
} CHANNEL_INFO_T, *P_CHANNEL_INFO_T;

typedef struct _CMD_SCAN_REQ_T {
    UINT_8          ucSeqNum;
    UINT_8          ucBssIndex;
    UINT_8          ucScanType;
    UINT_8          ucSSIDType;
    UINT_8          ucSSIDLength;
    UINT_8          ucNumProbeReq;
    UINT_16         u2ChannelMinDwellTime;
    UINT_16         u2ChannelDwellTime;
    UINT_16         u2TimeoutValue;
    UINT_8          aucSSID[32];
    UINT_8          ucChannelType;
    UINT_8          ucChannelListNum;
    UINT_8          aucReserved[2];
    CHANNEL_INFO_T  arChannelList[32];
    UINT_16         u2IELen;
    UINT_8          aucIE[MAX_IE_LENGTH];
} CMD_SCAN_REQ, *P_CMD_SCAN_REQ;

typedef struct _PARAM_SSID_T {
    UINT_32  u4SsidLen; /*!< SSID length in bytes. Zero length is broadcast(any) SSID */
    UINT_8   aucSsid[ELEM_MAX_LEN_SSID];
} PARAM_SSID_T, *P_PARAM_SSID_T;

typedef struct _CMD_SCAN_REQ_V2_T {
    UINT_8          ucSeqNum;
    UINT_8          ucBssIndex;
    UINT_8          ucScanType;
    UINT_8          ucSSIDType;
    UINT_8          ucSSIDNum;
    UINT_8          ucNumProbeReq;
    UINT_8          aucReserved[2];
    PARAM_SSID_T    arSSID[4];
    UINT_16         u2ProbeDelayTime;
    UINT_16         u2ChannelDwellTime;
    UINT_16         u2TimeoutValue;
    UINT_8          ucChannelType;
    UINT_8          ucChannelListNum;
    CHANNEL_INFO_T  arChannelList[32];
    UINT_16         u2IELen;
    UINT_8          aucIE[MAX_IE_LENGTH];
} CMD_SCAN_REQ_V2, *P_CMD_SCAN_REQ_V2;

typedef struct _CMD_SCAN_CANCEL_T {
    UINT_8          ucSeqNum;
    UINT_8          ucIsExtChannel;     /* For P2P channel extention. */
    UINT_8          aucReserved[2];
} CMD_SCAN_CANCEL, *P_CMD_SCAN_CANCEL;

typedef struct _CMD_CH_PRIVILEGE_T {
    UINT_8          ucBssIndex;
    UINT_8          ucTokenID;
    UINT_8          ucAction;
    UINT_8          ucPrimaryChannel;
    UINT_8          ucRfSco;
    UINT_8          ucRfBand;
    UINT_8          ucRfChannelWidth;   /* To support 80/160MHz bandwidth */
    UINT_8          ucRfCenterFreqSeg1; /* To support 80/160MHz bandwidth */
    UINT_8          ucRfCenterFreqSeg2; /* To support 80/160MHz bandwidth */
    UINT_8          ucReqType;
    UINT_8          aucReserved[2];
    UINT_32         u4MaxInterval;      /* In unit of ms */
} CMD_CH_PRIVILEGE_T, *P_CMD_CH_PRIVILEGE_T;

typedef struct _CMD_P2P_SEC_CHECK_T {
    UINT_32 u4KeyId;
    UINT_8  aucBuffer[32];
} CMD_P2P_TRANSPORT_T, *P_CMD_P2P_TRANSPORT_T;

typedef enum _ENUM_NLO_CIPHER_ALGORITHM {
    NLO_CIPHER_ALGO_NONE            = 0x00,
    NLO_CIPHER_ALGO_WEP40           = 0x01,
    NLO_CIPHER_ALGO_TKIP            = 0x02,
    NLO_CIPHER_ALGO_CCMP            = 0x04,
    NLO_CIPHER_ALGO_WEP104          = 0x05,
    NLO_CIPHER_ALGO_WPA_USE_GROUP   = 0x100,
    NLO_CIPHER_ALGO_RSN_USE_GROUP   = 0x100,
    NLO_CIPHER_ALGO_WEP             = 0x101,
} ENUM_NLO_CIPHER_ALGORITHM, *P_ENUM_NLO_CIPHER_ALGORITHM;

typedef enum _ENUM_NLO_AUTH_ALGORITHM {
    NLO_AUTH_ALGO_80211_OPEN        = 1,
    NLO_AUTH_ALGO_80211_SHARED_KEY  = 2,
    NLO_AUTH_ALGO_WPA               = 3,
    NLO_AUTH_ALGO_WPA_PSK           = 4,
    NLO_AUTH_ALGO_WPA_NONE          = 5,
    NLO_AUTH_ALGO_RSNA              = 6,
    NLO_AUTH_ALGO_RSNA_PSK          = 7,
} ENUM_NLO_AUTH_ALGORITHM, *P_ENUM_NLO_AUTH_ALGORITHM;

typedef struct _NLO_NETWORK {
    UINT_8      ucNumChannelHint[4];
    UINT_8      ucSSIDLength;
    UINT_8      ucCipherAlgo;   /* refer to ENUM_NLO_CIPHER_ALGORITHM */
    UINT_16     u2AuthAlgo;     /* refer to ENUM_NLO_AUTH_ALGORITHM */
    UINT_8      aucSSID[32];
} NLO_NETWORK, *P_NLO_NETWORK;

typedef struct _CMD_NLO_REQ_T {
    UINT_8      ucSeqNum;
    UINT_8      ucBssIndex;
    UINT_8      fgStopAfterIndication;
    UINT_8      ucFastScanIteration;
    UINT_16     u2FastScanPeriod;
    UINT_16     u2SlowScanPeriod;
    UINT_8      ucEntryNum;
    UINT_8      ucReserved;
    UINT_16     u2IELen;
    NLO_NETWORK arNetworkList[16];
    UINT_8      aucIE[0];
} CMD_NLO_REQ, *P_CMD_NLO_REQ;

typedef struct _CMD_NLO_CANCEL_T {
    UINT_8  ucSeqNum;
    UINT_8  aucReserved[3];
} CMD_NLO_CANCEL, *P_CMD_NLO_CANCEL;

#if CFG_M0VE_BA_TO_DRIVER

typedef struct _CMD_RESET_BA_SCOREBOARD_T {
    UINT_8  ucflag;
    UINT_8  ucTID;
    UINT_8  aucMacAddr[PARAM_MAC_ADDR_LEN];
} CMD_RESET_BA_SCOREBOARD_T, *P_CMD_RESET_BA_SCOREBOARD_T;

#endif

typedef struct _CMD_RTT_REQ_T {
    UINT_8          ucSeqNum;
    UINT_8          ucBssIndex;
    UINT_8          ucRttType;
    UINT_8          ucRttRole;
    BOOLEAN         fgRttTrigger;
    UINT_8          aucReserved[3];
} CMD_RTT_REQ, *P_CMD_RTT_REQ;

//chiahsuan:  ReStart Download Firmware Request/Response Start. (duplicated one. it is copy from ROM)
typedef struct _INIT_WIFI_CMD_T {
    UINT_8      ucCID;
    UINT_8      ucPktTypeID;    /* Must be 0xA0 (CMD Packet) */
    UINT_8      ucReserved;
    UINT_8      ucSeqNum;
    UINT_32     u4Reserved;     /* add one DW to compatible with normal TXD format. */
    UINT_8      aucBuffer[0];
} INIT_WIFI_CMD_T, *P_INIT_WIFI_CMD_T;

typedef struct _INIT_HIF_TX_HEADER_T {
    UINT_16     u2TxByteCount;
    UINT_16     u2PQ_ID;        /* Must be 0x8000 (Port1, Queue 0) */
    INIT_WIFI_CMD_T rInitWifiCmd;
} INIT_HIF_TX_HEADER_T, *P_INIT_HIF_TX_HEADER_T;

/*----------------------------------------------------------------------------*/
/* EVENT Packets                                                              */
/*----------------------------------------------------------------------------*/
typedef struct _EVENT_CONNECTION_STATUS {
    UINT_8      ucMediaStatus;
    UINT_8      ucReasonOfDisconnect;

    UINT_8      ucInfraMode;
    UINT_8      ucSsidLen;
    UINT_8      aucSsid[32];
    UINT_8      aucBssid[6];
    UINT_8      ucAuthenMode;
    UINT_8      ucEncryptStatus;
    UINT_16     u2BeaconPeriod;
    UINT_16     u2AID;
    UINT_16     u2ATIMWindow;
    UINT_8      ucBssIndex;
    UINT_8      aucReserved[1];
    UINT_32     u4FreqInKHz;
} EVENT_CONNECTION_STATUS, *P_EVENT_CONNECTION_STATUS;

typedef struct _EVENT_SCAN_RESULT_T {
    INT_32                      i4RSSI;
    UINT_32                     u4LinkQuality;
    UINT_32                     u4DSConfig; /* Center frequency */
    UINT_32                     u4DomainInfo; /* Require CM opinion */
    UINT_32                     u4Reserved;
    UINT_8                      ucBssIndex;
    UINT_8                      ucOpMode;
    UINT_8                      aucBssid[MAC_ADDR_LEN];
    UINT_8                      aucRatesEx[PARAM_MAX_LEN_RATES_EX];
    WLAN_BEACON_FRAME_BODY_T    rBeaconFrameBody;
} EVENT_SCAN_RESULT_T, *P_EVENT_SCAN_RESULT_T;

/* event of tkip mic error */
typedef struct _EVENT_MIC_ERR_INFO
{
    UINT_32     u4Flags;
    UINT_8      aucStaAddr[6];
} EVENT_MIC_ERR_INFO, *P_EVENT_MIC_ERR_INFO;

/* event of add key done for port control */
typedef struct _EVENT_ADD_KEY_DONE_INFO
{
    UINT_8      ucBSSIndex;
    UINT_8      ucReserved;
    UINT_8      aucStaAddr[6];
} EVENT_ADD_KEY_DONE_INFO, *P_EVENT_ADD_KEY_DONE_INFO;

typedef struct _EVENT_LINK_QUALITY {
    INT_8       cRssi; /* AIS Network. */
    INT_8       cLinkQuality;
    UINT_16     u2LinkSpeed;            /* TX rate1 */
    UINT_8      ucMediumBusyPercentage; /* Read clear */
    UINT_8      ucIsLQ0Rdy;                 /* Link Quality BSS0 Ready. */
    INT_8       cRssiP2P; /* P2P Network. */
    INT_8       cLinkQualityP2P;
    UINT_16     u2LinkSpeedP2P;            /* TX rate1 */
    UINT_8      ucMediumBusyPercentageP2P; /* Read clear */
    UINT_8      ucIsLQ1Rdy;                 /* Link Quality BSS1 Ready. */
} EVENT_LINK_QUALITY, *P_EVENT_LINK_QUALITY;

typedef struct _EVENT_ACTIVATE_STA_REC_T {
    UINT_8      aucMacAddr[6];
    UINT_8      ucStaRecIdx;
    UINT_8      ucBssIndex;
} EVENT_ACTIVATE_STA_REC_T, *P_EVENT_ACTIVATE_STA_REC_T;

typedef struct _EVENT_DEACTIVATE_STA_REC_T {
    UINT_8      ucStaRecIdx;
    UINT_8      aucReserved[3];
} EVENT_DEACTIVATE_STA_REC_T, *P_EVENT_DEACTIVATE_STA_REC_T;

typedef struct _EVENT_STATISTICS_T {
    LARGE_INTEGER   rTransmittedFragmentCount;
    LARGE_INTEGER   rMulticastTransmittedFrameCount;
    LARGE_INTEGER   rFailedCount;
    LARGE_INTEGER   rRetryCount;
    LARGE_INTEGER   rMultipleRetryCount;
    LARGE_INTEGER   rRTSSuccessCount;
    LARGE_INTEGER   rRTSFailureCount;
    LARGE_INTEGER   rACKFailureCount;
    LARGE_INTEGER   rFrameDuplicateCount;
    LARGE_INTEGER   rReceivedFragmentCount;
    LARGE_INTEGER   rMulticastReceivedFrameCount;
    LARGE_INTEGER   rFCSErrorCount;
} EVENT_STATISTICS_T, *P_EVENT_STATISTICS_T;

typedef struct _EVENT_SCAN_DONE_T {
    UINT_8          ucSeqNum;
    UINT_8          ucSparseChannelValid;
    CHANNEL_INFO_T  rSparseChannel;
} EVENT_SCAN_DONE, *P_EVENT_SCAN_DONE;

typedef struct _EVENT_CH_PRIVILEGE_T {
    UINT_8          ucBssIndex;
    UINT_8          ucTokenID;
    UINT_8          ucStatus;
    UINT_8          ucPrimaryChannel;
    UINT_8          ucRfSco;
    UINT_8          ucRfBand;
    UINT_8          ucRfChannelWidth;   /* To support 80/160MHz bandwidth */
    UINT_8          ucRfCenterFreqSeg1; /* To support 80/160MHz bandwidth */
    UINT_8          ucRfCenterFreqSeg2; /* To support 80/160MHz bandwidth */
    UINT_8          ucReqType;
    UINT_8          aucReserved[2];
    UINT_32         u4GrantInterval;    /* In unit of ms */
} EVENT_CH_PRIVILEGE_T, *P_EVENT_CH_PRIVILEGE_T;

typedef struct _EVENT_BSS_BEACON_TIMEOUT_T {
    UINT_8      ucBssIndex;
    UINT_8      aucReserved[3];
} EVENT_BSS_BEACON_TIMEOUT_T, *P_EVENT_BSS_BEACON_TIMEOUT_T;

typedef struct _EVENT_STA_AGING_TIMEOUT_T {
    UINT_8      ucStaRecIdx;
    UINT_8      aucReserved[3];
} EVENT_STA_AGING_TIMEOUT_T, *P_EVENT_STA_AGING_TIMEOUT_T;

typedef struct _EVENT_NOA_TIMING_T {
    UINT_8      ucIsInUse;              /* Indicate if this entry is in use or not */
    UINT_8      ucCount;                /* Count */
    UINT_8      aucReserved[2];

    UINT_32     u4Duration;             /* Duration */
    UINT_32     u4Interval;             /* Interval */
    UINT_32     u4StartTime;            /* Start Time */
} EVENT_NOA_TIMING_T, *P_EVENT_NOA_TIMING_T;

typedef struct _EVENT_UPDATE_NOA_PARAMS_T {
    UINT_8      ucBssIndex;
    UINT_8      aucReserved[2];
    UINT_8      ucEnableOppPS;
    UINT_16     u2CTWindow;

    UINT_8              ucNoAIndex;
    UINT_8              ucNoATimingCount; /* Number of NoA Timing */
    EVENT_NOA_TIMING_T  arEventNoaTiming[8/*P2P_MAXIMUM_NOA_COUNT*/];
} EVENT_UPDATE_NOA_PARAMS_T, *P_EVENT_UPDATE_NOA_PARAMS_T;

typedef struct _EVENT_AP_OBSS_STATUS_T {
    UINT_8      ucBssIndex;
    UINT_8      ucObssErpProtectMode;
    UINT_8      ucObssHtProtectMode;
    UINT_8      ucObssGfOperationMode;
    UINT_8      ucObssRifsOperationMode;
    UINT_8      ucObssBeaconForcedTo20M;
    UINT_8      aucReserved[2];
} EVENT_AP_OBSS_STATUS_T, *P_EVENT_AP_OBSS_STATUS_T;

typedef struct _EVENT_RDD_STATUS_T {
    UINT_8      ucRddStatus;
    UINT_8      aucReserved[3];
} EVENT_RDD_STATUS_T, *P_EVENT_RDD_STATUS_T;

typedef struct _EVENT_ICAP_STATUS_T {
    UINT_8      ucRddStatus;
    UINT_8      aucReserved[3];
    UINT_32     u4StartAddress;
    UINT_32     u4IcapSieze;
} EVENT_ICAP_STATUS_T, *P_EVENT_ICAP_STATUS_T;

typedef struct _EVENT_NLO_DONE_T {
    UINT_8      ucSeqNum;
    UINT_8      ucStatus;   /* 0: Found / other: reserved */
    UINT_8      aucReserved[2];
} EVENT_NLO_DONE_T, *P_EVENT_NLO_DONE_T;

//chiahsuan: ReStart Download Firmware Request/Response Start (duplicated one. it is copy from ROM)
typedef struct _INIT_WIFI_EVENT_T {
    UINT_16     u2RxByteCount;
    UINT_16     u2PacketType;   /* Must be filled with 0xE000 (EVENT Packet) */
    UINT_8      ucEID;
    UINT_8      ucSeqNum;
    UINT_8      aucReserved[2];

    UINT_8      aucBuffer[0];
} INIT_WIFI_EVENT_T, *P_INIT_WIFI_EVENT_T;


#define WIFI_RESTART_DOWNLOAD_STATUS_NO_ERROR            0
#if 0
#ifndef WLAN_STATUS_COMMAND_NOT_SUPPORT
#define WLAN_STATUS_COMMAND_NOT_SUPPORT       ((WLAN_STATUS) 0x000000FEL)
#endif

#ifndef WLAN_STATUS_FAILURE_ASSERT
#define WLAN_STATUS_FAILURE_ASSERT            ((WLAN_STATUS) 0x000000FAL)
#endif
#endif
/* Generic RAM event status enum for inband command */
typedef enum _ENUM_EVENT_CMD_RESULT_STATUS {
    EVENT_CMD_STATUS_SUCCESS = 0x00,
    EVENT_CMD_STATUS_CMD_ABORT = 0xF1,	
    EVENT_CMD_STATUS_FAILURE_ASSERT = 0xFA,
    EVENT_CMD_STATUS_CMD_NOT_SUPPORT = 0xFE,
} ENUM_EVENT_CMD_RESULT_STATUS, *P_ENUM_EVENT_CMD_RESULT_STATUS;


typedef struct _INIT_EVENT_CMD_RESULT {
    UINT_8      ucStatus;           /* 0: success, 0xFE command not support, others: failure */
    UINT_8      ucCID;
    UINT_8      aucReserved[2];
} INIT_EVENT_CMD_RESULT, *P_INIT_EVENT_CMD_RESULT, INIT_EVENT_PENDING_ERROR, *P_INIT_EVENT_PENDING_ERROR;

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
#endif /* _WLAN_CMDEVENT_H */


