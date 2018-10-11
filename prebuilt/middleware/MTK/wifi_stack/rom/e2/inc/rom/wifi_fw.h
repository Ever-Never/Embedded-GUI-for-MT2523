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


#include "rom/wifi_os.h"
#include "cm4_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __WIFI_FW_H__
#define __WIFI_FW_H__


/**
 * the return code of wifi_fw functions.
 */
typedef uint32_t wlan_status_t;

#define WLAN_STATUS_SUCCESS                     ((wlan_status_t) 0x00000000L)
#define WLAN_STATUS_FAILURE_ASSERT              ((wlan_status_t) 0x000000FAL)
#define WLAN_STATUS_COMMAND_NOT_SUPPORT         ((wlan_status_t) 0x000000FEL)
#define WLAN_STATUS_PENDING                     ((wlan_status_t) 0x00000103L)
#define WLAN_STATUS_NOT_ACCEPTED                ((wlan_status_t) 0x00010003L)
#define WLAN_STATUS_MEDIA_CONNECT               ((wlan_status_t) 0x4001000BL)
#define WLAN_STATUS_MEDIA_DISCONNECT            ((wlan_status_t) 0x4001000CL)
#define WLAN_STATUS_MEDIA_SPECIFIC_INDICATION   ((wlan_status_t) 0x40010012L)
#define WLAN_STATUS_SCAN_COMPLETE               ((wlan_status_t) 0x60010001L)
#define WLAN_STATUS_MSDU_OK                     ((wlan_status_t) 0x60010002L)

#define WLAN_STATUS_ROAM_OUT_FIND_BEST          ((wlan_status_t) 0x60010101L)
#define WLAN_STATUS_ROAM_DISCOVERY              ((wlan_status_t) 0x60010102L)

#define WLAN_STATUS_FAILURE                     ((wlan_status_t) 0xC0000001L)
#define WLAN_STATUS_RESOURCES                   ((wlan_status_t) 0xC000009AL)
#define WLAN_STATUS_NOT_SUPPORTED               ((wlan_status_t) 0xC00000BBL)

#define WLAN_STATUS_MULTICAST_FULL              ((wlan_status_t) 0xC0010009L)
#define WLAN_STATUS_INVALID_PACKET              ((wlan_status_t) 0xC001000FL)
#define WLAN_STATUS_ADAPTER_NOT_READY           ((wlan_status_t) 0xC0010011L)
#define WLAN_STATUS_INVALID_LENGTH              ((wlan_status_t) 0xC0010014L)
#define WLAN_STATUS_INVALID_DATA                ((wlan_status_t) 0xC0010015L)
#define WLAN_STATUS_BUFFER_TOO_SHORT            ((wlan_status_t) 0xC0010016L)

#define WLAN_STATUS_BUFFER_RETAINED             ((wlan_status_t) 0x00000001L)
#define WLAN_STATUS_PACKET_FLUSHED              ((wlan_status_t) 0x00000002L)
#define WLAN_STATUS_PACKET_AGING_TIMEOUT        ((wlan_status_t) 0x00000003L)
#define WLAN_STATUS_PACKET_MPDU_ERROR           ((wlan_status_t) 0x00000004L)
#define WLAN_STATUS_PACKET_RTS_ERROR            ((wlan_status_t) 0x00000005L)
#define WLAN_STATUS_PACKET_LIFETIME_ERROR       ((wlan_status_t) 0x00000006L)
#define WLAN_STATUS_PACKET_BIP_ERROR            ((wlan_status_t) 0x00000007L)
#define WLAN_STATUS_PACKET_DROPPED              ((wlan_status_t) 0x00000008L)


#if 0
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

    EVENT_ID_LAYER_0_EXT_MAGIC_NUM	= 0xED,	    /* magic number for Extending MT6630 original EVENT header  */
    EVENT_ID_LAYER_1_MAGIC_NUM		= 0xEE,	    /* magic number for backward compatible with MT76xx EVENT  */

    EVENT_ID_MAX_NUM
} ENUM_EVENT_ID_T, *P_ENUM_EVENT_ID_T;


/*
 * Generic RAM event status enum for inband command
 *
 * @note EVENT_CMD_STATUS_CMD_ABORT is from iot_patch_lib.c.
 *       The rests are from wlan_cmdevent_iot_rom.h.
 */
typedef enum _ENUM_EVENT_CMD_RESULT_STATUS {
    EVENT_CMD_STATUS_SUCCESS         = 0x00,
    EVENT_CMD_STATUS_CMD_ABORT       = 0xF1,
    EVENT_CMD_STATUS_FAILURE_ASSERT  = 0xFA,
    EVENT_CMD_STATUS_CMD_NOT_SUPPORT = 0xFE,
} ENUM_EVENT_CMD_RESULT_STATUS, *P_ENUM_EVENT_CMD_RESULT_STATUS;

#endif
typedef enum
{
    MSG_ID_WIFI_IST                 = 0x0000,
    MSG_ID_TIMER_EXPIRY             = 0x0001,
    MSG_ID_WIFI_RESET               = 0x0002,
    MSG_ID_INBAND                   = 0x0003,
    MSG_ID_WIFI_RX_DATA				= 0x0004,
    MSG_ID_WIFI_TX_DATA				= 0x0005,
    MSG_ID_WIFI_EXT				= 0x0006,
} wifi_fw_func_id_t;


typedef struct _wifi_fw_msg_t
{
    wifi_fw_func_id_t   id;
    uint32_t            data;
} wifi_fw_msg_t;


// Moved from wifi_fw_int.h (which is deprecated in MT5932 ROM)
typedef struct _wifi_fw_t
{
    wifi_os_api_t   *os_api;
#if 1 //defined(NEW_QUEUE_DESIGN) && defined(DATA_PATH_87)	
    int32_t         queue_id_cmd;
	int32_t         queue_id_mgmt;
	int32_t         queue_id_data;
#else
	int32_t         queue_id;	
#endif
} wifi_fw_t;


#define INIT_EVENT_PACKET_TYPE (0xE000)

struct _WIFI_CMD_T;
/* message handler function pointer type */
typedef wlan_status_t (*wifi_fw_func_t)(wifi_fw_t *fw, struct _WIFI_CMD_T *cmd);

/* message handler mapping table type (maps id to hanlder) */
typedef struct _wifi_fw_func_map_t
{
    wifi_fw_func_id_t   id;
    wifi_fw_func_t      func;
} wifi_fw_func_map_t;

extern wifi_fw_func_map_t g_wifi_fw_func_map[];

/*Already defined so commneted here*/
/**
 * the function initializes wifi firmware to ready status.
 *
 * @param os_api OS dependent API implemented by caller, see file: wifi_os.h
 *               for the descriptions of needed API.
 * @param msg_sz the size of each message of message queue.
 * @param q_sz   the number of memebers of message queue.
 * @return       the returned non-typed pointer points to a control block
 *               prepared by wifi_fw_init.
 */
void *wifi_fw_init(wifi_os_api_t *os_api, uint32_t msg_sz, uint8_t q_sz);


/**
 * note that the function returns every time it has processed one message.
 * hence, the sequence of this function should not include init/de-init.
 */
void wifi_fw_process(void *fw_control_block);


/**
 * simulate an interrupt request
 */
void wifi_fw_sim_ist(void *fw_control_block);
void wifi_fw_sim_set_opmode(void *fw_control_block);
void wifi_fw_sim_reset_opmode(void *fw_control_block);

#if defined(DATA_PATH_86) || defined(DATA_PATH_87)
void enqueue_tx_data_msg_to_wifi_task(void *fw_control_block,void *data_buf);
#endif

#ifdef DATA_PATH_87
void enqueue_rx_data_msg_to_wifi_task(void *fw_control_block,void *data_buf);
#endif /*DATA_PATH_87*/

#endif

#ifdef __cplusplus
}
#endif
