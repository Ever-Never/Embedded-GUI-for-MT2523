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

#ifndef AT_COMMAND_BT_H
#define AT_COMMAND_BT_H

//System head file
//#include "FreeRTOS.h"
//#include "task.h"
#include <string.h>
#include <stdio.h>

//#ifdef MTK_BT_AT_COMMAND_ENABLE

#ifdef __cplusplus
extern "C"
{
#endif


/*---  Define ---*/
#define ATCI_BT_ATA_INQUIRY_NUM 1
#define ATCI_BT_ATA_INQUIRY_TIME 10

typedef struct {
    int message_id;
    int param1;
    uint8_t param2[256];
} at_command_bt_message_struct_t;

typedef enum {
    ATCI_BT_COMMAND_HCI_EVENT = 0,
    ATCI_BT_COMMAND_END
} acti_bt_command_message_id_enum;

typedef enum {
    ATCI_BT_EVENT_INQUIRY_COMPLETE               = 0x01,
    ATCI_BT_EVENT_CONNECT_COMPLETE               = 0x03,
    ATCI_BT_EVENT_DISCONNECT_COMPLETE            = 0x05,
    ATCI_BT_EVENT_AUTH_COMPLETE                  = 0x06,
    ATCI_BT_EVENT_REMOTE_NAME_REQ_COMPLETE       = 0x07,
    ATCI_BT_EVENT_CHNG_CONN_LINK_KEY_COMPLETE    = 0x09,
    ATCI_BT_EVENT_MASTER_LINK_KEY_COMPLETE       = 0x0A,
    ATCI_BT_EVENT_READ_REMOTE_FEATURES_COMPLETE  = 0x0B,
    ATCI_BT_EVENT_READ_REMOTE_VERSION_COMPLETE   = 0x0C,
    ATCI_BT_EVENT_QOS_SETUP_COMPLETE             = 0x0D,
    ATCI_BT_EVENT_COMMAND_COMPLETE               = 0x0E,
    ATCI_BT_EVENT_READ_CLOCK_OFFSET_COMPLETE     = 0x1C,
    ATCI_BT_EVENT_FLOW_SPECIFICATION_COMPLETE    = 0x21, /* 1.2 */
    ATCI_BT_EVENT_READ_REMOTE_EXT_FEAT_COMPLETE  = 0x23, /* 1.2 */
    ATCI_BT_EVENT_SYNC_CONNECT_COMPLETE          = 0x2C, /* 1.2 */
    ATCI_BT_EVENT_SIMPLE_PAIRING_COMPLETE        = 0x36, /* 2.1 */
    ATCI_BT_EVENT_ENHANCED_FLUSH_COMPLETE        = 0x39, /* 2.1 */
} acti_bt_event_msg_id_enum;

/*---------------------------------------------------------------------------
 * atci_hci_command_type type
 *
 *     Commands are indicated to the HCI layer when calling
 *     HCI_SendCommand. A command type from the list below is specified
 *     in the "opCode" parameter to this function. Additional
 *     command-specific information is provided in the "cmd" parameter.
 */
typedef enum
{
    ATCI_HCC_READ_LMP_HANDLE              = 0x0420, /* 1.2 */
/* Group: Link policy commands */
    ATCI_HCC_ROLE_DISCOVERY               = 0x0809,
    ATCI_HCC_READ_LINK_POLICY             = 0x080C,
    ATCI_HCC_WRITE_LINK_POLICY            = 0x080D,
    ATCI_HCC_SNIFF_SUBRATING              = 0x0811, /* 2.1 */

/* Group: Host controller and baseband commands */
    ATCI_HCC_FLUSH                        = 0x0C08,
    ATCI_HCC_READ_AUTO_FLUSH_TIMEOUT      = 0x0C27,
    ATCI_HCC_WRITE_AUTO_FLUSH_TIMEOUT     = 0x0C28,
    ATCI_HCC_READ_XMIT_POWER_LEVEL        = 0x0C2D,
    ATCI_HCC_READ_LINK_SUPERV_TIMEOUT     = 0x0C36,
    ATCI_HCC_WRITE_LINK_SUPERV_TIMEOUT    = 0x0C37,
/* Group: Status parameters */
    ATCI_HCC_READ_FAILED_CONTACT_COUNT    = 0x1401,
    ATCI_HCC_RESET_FAILED_CONTACT_COUNT   = 0x1402,
    ATCI_HCC_GET_LINK_QUALITY             = 0x1403,
    ATCI_HCC_READ_RSSI                    = 0x1405,
    ATCI_HCC_READ_AFH_CHANNEL_MAP         = 0x1406, /* 1.2 */
    ATCI_HCC_READ_CLOCK                   = 0x1407, /* 1.2 */
    ATCI_HCC_LE_READ_CHANNEL_MAP          = 0x2015,
    ATCI_HCC_LE_LONG_TERM_KEY_REQ_REPL    = 0x201A,
    ATCI_HCC_LE_LONG_TERM_KEY_REQ_NEG_REPL= 0x201B,
}atci_hci_command_type;



/*---------------------------------------------------------------------------
 * atci_hci_event_type type
 *
 *     These events are indicated by HCI to the Management Entity during
 *     a HCI_CONTROLLER_EVENT callback. The event type is found in the
 *     "hciEvent->event" field of the callback parameters structure
 *     (HciCallbackParms).
 */

typedef enum
{
    ATCI_HCE_INQUIRY_COMPLETE               = 0x01,
    ATCI_HCE_INQUIRY_RESULT                 = 0x02,
    ATCI_HCE_CONNECT_COMPLETE               = 0x03,
    ATCI_HCE_CONNECT_REQUEST                = 0x04,
    ATCI_HCE_DISCONNECT_COMPLETE            = 0x05,
    ATCI_HCE_AUTH_COMPLETE                  = 0x06,
    ATCI_HCE_REMOTE_NAME_REQ_COMPLETE       = 0x07,
    ATCI_HCE_ENCRYPT_CHNG                   = 0x08,
    ATCI_HCE_CHNG_CONN_LINK_KEY_COMPLETE    = 0x09,
    ATCI_HCE_MASTER_LINK_KEY_COMPLETE       = 0x0A,
    ATCI_HCE_READ_REMOTE_FEATURES_COMPLETE  = 0x0B,
    ATCI_HCE_READ_REMOTE_VERSION_COMPLETE   = 0x0C,
    ATCI_HCE_QOS_SETUP_COMPLETE             = 0x0D,
    ATCI_HCE_COMMAND_COMPLETE               = 0x0E,
    ATCI_HCE_COMMAND_STATUS                 = 0x0F,
    ATCI_HCE_HARDWARE_ERROR                 = 0x10,
    ATCI_HCE_FLUSH_OCCURRED                 = 0x11,
    ATCI_HCE_ROLE_CHANGE                    = 0x12,
    ATCI_HCE_NUM_COMPLETED_PACKETS          = 0x13,
    ATCI_HCE_MODE_CHNG                      = 0x14,
    ATCI_HCE_RETURN_LINK_KEYS               = 0x15,
    ATCI_HCE_PIN_CODE_REQ                   = 0x16,
    ATCI_HCE_LINK_KEY_REQ                   = 0x17,
    ATCI_HCE_LINK_KEY_NOTIFY                = 0x18,
    ATCI_HCE_LOOPBACK_COMMAND               = 0x19,
    ATCI_HCE_DATA_BUFFER_OVERFLOW           = 0x1A,
    ATCI_HCE_MAX_SLOTS_CHNG                 = 0x1B,
    ATCI_HCE_READ_CLOCK_OFFSET_COMPLETE     = 0x1C,
    ATCI_HCE_CONN_PACKET_TYPE_CHNG          = 0x1D,
    ATCI_HCE_QOS_VIOLATION                  = 0x1E,
    ATCI_HCE_PAGE_SCAN_REPETITION_MODE      = 0x20,
    ATCI_HCE_FLOW_SPECIFICATION_COMPLETE    = 0x21, /* 1.2 */
    ATCI_HCE_INQUIRY_RESULT_WITH_RSSI       = 0x22, /* 1.2 */
    ATCI_HCE_READ_REMOTE_EXT_FEAT_COMPLETE  = 0x23, /* 1.2 */
    ATCI_HCE_SYNC_CONNECT_COMPLETE          = 0x2C, /* 1.2 */
    ATCI_HCE_SYNC_CONN_CHANGED              = 0x2D, /* 1.2 */
    ATCI_HCE_SNIFF_SUBRATING_EVENT          = 0x2E, /* Posh_assert */
    ATCI_HCE_INQUIRY_RESULT_WITH_EIR        = 0x2F, /* 2.1 */
    ATCI_HCE_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT   = 0x30, /* Posh_assert */
    ATCI_HCE_IO_CAPABILITY_REQUEST          = 0x31, /* 2.1 */
    ATCI_HCE_IO_CAPABILITY_RESPONSE         = 0x32, /* 2.1 */
    ATCI_HCE_USER_CONFIRM_REQUSEST          = 0x33, /* 2.1 */
    ATCI_HCE_USER_PASSKEY_REQUEST_EVENT     = 0x34, /* Posh_assert */
    ATCI_HCE_REMOTE_OOB_DATA_REQUEST_EVENT  = 0x35, /* Posh_assert */
    ATCI_HCE_SIMPLE_PAIRING_COMPLETE        = 0x36, /* 2.1 */
    ATCI_HCE_LINK_SUPERVISION_TIMEOUT_CHNG  = 0x38, /* 2.1 */
    ATCI_HCE_ENHANCED_FLUSH_COMPLETE        = 0x39, /* 2.1 */
    ATCI_HCE_USER_PASSKEY_NOTIFICATION      = 0x3B, /* 2.1 */
    ATCI_HCE_USER_KEYPRESS                  = 0x3C, /* 2.1 */
    ATCI_HCE_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT   = 0x3D,
    ATCI_HCE_LE_META_EVENT                  = 0x3E, /* 4.0 */
    ATCI_HCE_BLUETOOTH_LOGO                 = 0xFE,
    ATCI_HCE_VENDOR_SPECIFIC                = 0xFF,
}atci_hci_event_type;

typedef enum
{
    ATCI_HCE_LE_CONNECT_COMPLETE = 0x01,
    ATCI_HCE_LE_ADVERTISE_REPORT,                    /* 0x02 */
    ATCI_HCE_LE_CONNECT_UPDATE_COMPLETE,             /* 0x03 */
    ATCI_HCE_LE_READ_REMOTE_FEATURES_COMPLETE,       /* 0x04 */
    ATCI_HCE_LE_LONG_TERM_KEY_REQUEST_EVENT,         /* 0x05 */
    ATCI_HCE_LE_ENHANCED_CONNECT_COMPLETE = 0x0A
}atci_hci_le_sub_event_type;

/*Context Define */
typedef struct {
    bool ata;
    bool is_host_power_on;
    bool is_inquiry_completed;
    uint8_t searched_num;
    bool is_register_rx_callback;
    bool is_power;
    bool is_sent_cmd;
} atci_bt_context_struct_t;

typedef struct {
    uint16_t op_code;
    uint8_t cmd_len;
    uint8_t cmd[256];
} atci_bt_hci_cmd;


typedef struct {
    uint8_t event;
    uint8_t status;
    uint16_t handle;
    uint8_t parmslen;
    uint8_t parms[256];
} atci_bt_hci_event;

#ifdef __cplusplus
}
#endif


//#endif /*MTK_BT_AT_COMMAND_ENABLE*/

#endif
