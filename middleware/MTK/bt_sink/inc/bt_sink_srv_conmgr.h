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

#ifndef __BT_SINK_SRV_CONMGR_H__
#define __BT_SINK_SRV_CONMGR_H__

#include <stdint.h>
#include "bt_gap.h"
#include "bt_sink_srv_resource.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_ami.h"
#include "bt_device_manager.h"
#include "bt_device_manager_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __BT_AWS_SUPPORT__
#define BT_SINK_SRV_CM_MAX_PROFILE_NUMBER         5
#else
#define BT_SINK_SRV_CM_MAX_PROFILE_NUMBER         4
#endif
#define BT_SINK_SRV_CM_MAX_DEVICE_NUMBER          2
#define BT_SINK_SRV_CM_MAX_TRUSTED_DEV           BT_DEVICE_MANAGER_MAX_PAIR_NUM
#define BT_SINK_SRV_CM_REASON_CONNECTION_TIMEOUT  0x08
#define BT_SINK_SRV_CM_DEVICE_ID_UNAVAILABLE      0xFF
#define BT_SINK_SRV_CM_DISCOVERABLE_DURATION      90000
#define BT_SINK_SRV_CM_PROFILE_NOTIFY             6000
#define BT_SINK_SRV_CM_POWER_ON_RECONNECT         6000

#define BT_SINK_SRV_UUID_HF                     (0x111E)
#define BT_SINK_SRV_UUID_A2DP                   (0x110B)
#define BT_SINK_SRV_UUID_AVRCP                  (0x110C)
#define BT_SINK_SRV_UUID_PBAPC                  (0x112F)
#define BT_SINK_SRV_UUID_AWS                    (0x113F)

// database constants
#define BT_SINK_SRV_CM_DB_NAME  "BT_Headset_Demo"
#define BT_SINK_SRV_CM_DB_COD 0x240408
#define BT_SINK_SRV_CM_DB_IO    BT_GAP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT

typedef enum {
    BT_SINK_SRV_CM_PROFILE_HF       = 0,
    BT_SINK_SRV_CM_PROFILE_A2DP     = 1,
    BT_SINK_SRV_CM_PROFILE_AVRCP    = 2,
    BT_SINK_SRV_CM_PROFILE_PBAPC    = 3,
    #ifdef __BT_AWS_SUPPORT__
    BT_SINK_SRV_CM_PROFILE_AWS      = 4,
    #endif
    BT_SINK_SRV_CM_PROFILE_END
} bt_sink_srv_cm_profile_type_t;

typedef enum {
    BT_SINK_SRV_CM_DEVICE_NONE      = 0,
    BT_SINK_SRV_CM_DEVICE_AWS       = 1,
    BT_SINK_SRV_CM_DEVICE_SP        = 2
} bt_sink_srv_cm_device_type_t;

typedef enum {
    BT_SINK_SRV_FLAG_POWER_ON              = 0x01,
    BT_SINK_SRV_FLAG_INQRUIRY_SCAN         = 0x02,
} bt_sink_srv_cm_flags_t;

typedef enum {
    BT_SINK_SRV_PROFILE_STATE_DISCONNECTED,
    BT_SINK_SRV_PROFILE_STATE_CONNECTING,
    BT_SINK_SRV_PROFILE_STATE_CONNECTED,
    BT_SINK_SRV_PROFILE_STATE_AUDIO_ON
} bt_sink_srv_cm_profile_state_t;

typedef enum {
    BT_SINK_SRV_CM_STATE_DISCONNECTED,
    BT_SINK_SRV_CM_STATE_CONNECTING,
    BT_SINK_SRV_CM_STATE_CONNECTED,
    BT_SINK_SRV_CM_STATE_DISCONNECTING,
    BT_SINK_SRV_CM_STATE_END
} bt_sink_srv_cm_state_t;

typedef enum {
    BT_SINK_SRV_CM_FIND_BY_HANDLE,
    BT_SINK_SRV_CM_FIND_BY_ADDR
} bt_sink_srv_cm_find_by_type_t;

typedef enum {
    BT_SINK_SRV_CM_REQ_TYPE_NONE,
    BT_SINK_SRV_CM_REQ_TYPE_NORMAL,
    BT_SINK_SRV_CM_REQ_TYPE_POWER_ON,
    BT_SINK_SRV_CM_REQ_TYPE_LINK_LOST,
    BT_SINK_SRV_CM_REQ_TYPE_TOTAL
} bt_sink_srv_cm_req_type_t;

#ifdef MTK_BT_COMMAND_ENABLE
#define BT_SINK_SRV_EVENT_PROFILE_CONNECTION_UPDATE (BT_SINK_SRV_EVENT_USER)
#define BT_SINK_SRV_EVENT_HFP_SCO_STATE  (BT_SINK_SRV_EVENT_USER + 1)

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_PROFILE_CONNECTON_UPDATE) which indicates sink service profile state is changed.
 */
typedef struct {
    bt_bd_addr_t address;                                 /**<  the remote device address */
    bt_sink_srv_cm_profile_type_t profile;                        /**< Profile type*/
    bt_sink_srv_cm_profile_state_t state;   /**<current sink profile connection state */
} bt_sink_srv_cm_profile_connection_update_t;

extern void bt_sink_srv_cm_profile_state_change_notify(
    bt_bd_addr_t* address, 
    bt_sink_srv_cm_profile_type_t profile, 
    bt_sink_srv_cm_state_t state);
#endif /*MTK_BT_COMMAND_ENABLE*/

typedef struct {
    bt_bd_addr_t bt_addr;
} bt_sink_srv_cm_common_record;

typedef struct {
    void *data;
    uint32_t handle;
    uint16_t uuid;
    uint8_t state;
} bt_sink_srv_profile_t;

typedef struct {
    uint8_t state;
    bt_gap_connection_handle_t handle;
    bt_bd_addr_t addr;
    bt_sink_srv_profile_type_t profile_type;
    uint8_t                    profile_connection_mask;
    bt_sink_srv_profile_t *profile[BT_SINK_SRV_CM_MAX_PROFILE_NUMBER];
} bt_sink_srv_cm_remote_device_t;

typedef void (*bt_sink_srv_cm_req_callback)(bt_sink_srv_status_t result, void *parameters);

typedef struct {
    bt_sink_srv_cm_req_type_t type;
    bt_sink_srv_cm_req_callback callback;
    uint32_t delay_time;
    bt_sink_srv_profile_connection_action_t action_param;
    uint8_t attampts;
} bt_sink_srv_cm_request_t;

typedef struct _bt_sink_srv_cm_node_t {
    bt_sink_srv_cm_request_t *parameters;
    struct _bt_sink_srv_cm_node_t *prev;
    struct _bt_sink_srv_cm_node_t *next;
} bt_sink_srv_cm_node_t;

typedef struct {
    uint8_t speaker_volume;
} bt_sink_srv_hf_stored_data_t;

#define BT_SINK_SRV_REQUEST_DELAY_TIME (3000)
#define BT_SINK_SRV_LINK_LOST_RECONNECT_DELAY (5000)
#define BT_SINK_SRV_REQUEST_DELAY_TIME_INCREASE (15000)
#define BT_SINK_SRV_PBAPC_STORED_COUNT  (3)

typedef struct {
    uint8_t  number[BT_SINK_SRV_MAX_PHONE_NUMBER + 1];
    uint8_t  name[BT_SINK_SRV_MAX_NAME + 1];
} bt_sink_srv_pbapc_record_t;

typedef bt_sink_srv_pbapc_record_t bt_sink_srv_pbapc_stored_data_t[BT_SINK_SRV_PBAPC_STORED_COUNT];

#define BT_SINK_SRV_HFP_STORAGE_SIZE sizeof(bt_sink_srv_hf_stored_data_t)
#define BT_SINK_SRV_A2DP_STORAGE_SIZE 1
#define BT_SINK_SRV_PBAP_STORAGE_SIZE sizeof(bt_sink_srv_pbapc_stored_data_t)

typedef struct {
    bt_bd_addr_t        address;            /**< The remote device address. */
    bt_sink_srv_cm_device_type_t dev_type;
#ifdef BT_SINK_SRV_HFP_STORAGE_SIZE
    uint8_t hfp_info[BT_SINK_SRV_HFP_STORAGE_SIZE];
#endif
#ifdef BT_SINK_SRV_A2DP_STORAGE_SIZE
    uint8_t a2dp_info[BT_SINK_SRV_A2DP_STORAGE_SIZE];
#endif
#ifdef BT_SINK_SRV_AVRCP_STORAGE_SIZE
    uint8_t avrcp_info[BT_SINK_SRV_AVRCP_STORAGE_SIZE];
#endif
#ifdef BT_SINK_SRV_PBAP_STORAGE_SIZE
    uint8_t pbap_info[BT_SINK_SRV_PBAP_STORAGE_SIZE];
#endif
} bt_sink_srv_cm_dev_info_t;

typedef struct {
    uint8_t flags;
    uint8_t connected_dev;
    uint8_t le_connected_dev;
    
    bt_sink_srv_cm_node_t *request_pending;
    bt_sink_srv_cm_node_t request_list;

    bt_sink_srv_cm_remote_device_t devices[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
} bt_sink_srv_cm_context_t;

void bt_sink_srv_cm_connect(bt_sink_srv_profile_connection_action_t *conn_action);

void bt_sink_srv_cm_disconnect(void);

void bt_sink_srv_cm_link_connected(const bt_bd_addr_t *dev_addr, bt_gap_connection_handle_t handle);

void bt_sink_srv_cm_link_disconnected(const bt_bd_addr_t *dev_addr, bt_gap_connection_handle_t handle);

bool bt_sink_srv_cm_get_profile_data(bt_bd_addr_t *bt_addr, uint16_t uuid, void *data_p);

bool bt_sink_srv_cm_set_profile_data(bt_bd_addr_t *bt_addr, uint16_t uuid, void *data_p);

uint32_t bt_sink_srv_cm_get_connected_device(uint16_t uuid, bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER]);

void *bt_sink_srv_cm_find_profile_by_address(bt_bd_addr_t *bt_addr, uint16_t uuid);

void *bt_sink_srv_cm_find_profile_by_hanlde(uint32_t handle);

bt_bd_addr_t *bt_sink_srv_cm_find_address_by_hanlde(uint32_t handle);

bt_sink_srv_cm_remote_device_t *bt_sink_srv_cm_find_device(bt_sink_srv_cm_find_by_type_t type, void *param);

bt_bd_addr_t *bt_sink_srv_cm_last_connected_device(void);

void bt_sink_srv_cm_set_role(bt_bd_addr_t *address_p, bt_role_t role);

void bt_sink_srv_cm_init(void);

void bt_sink_srv_cm_deinit(void);

bt_sink_srv_status_t bt_sink_srv_cm_action_handler(bt_sink_srv_action_t action, void *param);

void bt_sink_srv_cm_profile_status_notify(bt_bd_addr_t *addr, bt_sink_srv_profile_type_t profile, bool connected);

bt_sink_srv_status_t bt_sink_srv_cm_read_local_address(uint8_t *local_addr);

void bt_sink_srv_atci_init(void);

bt_status_t bt_sink_srv_cm_gap_callback(bt_msg_type_t msg, bt_status_t status, void *param);

bt_status_t bt_sink_srv_cm_system_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);

#ifdef MTK_BT_COMMAND_ENABLE
void bt_sink_srv_custom_event_callback(bt_sink_srv_event_t sink_event_id, void *param, uint32_t param_length);
#endif

#ifdef __cplusplus
}
#endif

#endif
