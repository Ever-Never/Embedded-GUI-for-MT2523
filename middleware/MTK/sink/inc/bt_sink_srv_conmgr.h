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

#include "bt_sink_srv_resource.h"
#include "bt_sink_srv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_SINK_SRV_CM_MAX_PROFILE_NUMBER         5
#define BT_SINK_SRV_CM_MAX_DEVICE_NUMBER          2
#define BT_SINK_SRV_CM_MAX_TRUSTED_DEV           BT_DEVICE_MANAGER_MAX_PAIR_NUM
#define BT_SINK_SRV_CM_REASON_CONNECTION_TIMEOUT  0x08
#define BT_SINK_SRV_CM_DEVICE_ID_UNAVAILABLE      0xFF
#define BT_SINK_SRV_CM_PROFILE_NOTIFY             6000
#define BT_SINK_SRV_CM_POWER_ON_RECONNECT    5000
#define BT_SINK_SRV_REQUEST_DELAY_TIME                          (3000)
#define BT_SINK_SRV_LINK_LOST_RECONNECT_DELAY (5000)
#define BT_SINK_SRV_REQUEST_DELAY_TIME_INCREASE                 (3000)  //(15000)
#define BT_SINK_SRV_CM_POWER_ON_WAITING_REMOTE_AWS_CONNECTION   (100)//15000

#define BT_SINK_SRV_FLAG_POWER_ON                              (0x01)
#define BT_SINK_SRV_FLAG_SET_SCAN_MODE                 (0x02)
#define BT_SINK_SRV_FLAG_SYSTEM_POWER_OFF         (0x04)
typedef uint8_t bt_sink_srv_cm_flags_t;     /**<cm flag type. */

/**< All the profiles were connected with SP */
#define BT_SINK_SRV_CM_PROFILE_SP      (BT_SINK_SRV_PROFILE_HFP | BT_SINK_SRV_PROFILE_A2DP_SINK  | BT_SINK_SRV_PROFILE_AVRCP) 
/**< All the profiles were connected with AWS */
#define BT_SINK_SRV_CM_PROFILE_AWS  (BT_SINK_SRV_PROFILE_AWS) 
typedef uint8_t bt_sink_srv_cm_device_type_t;     /**<type device type. */

/**< AACL link state */
typedef enum {
    BT_SINK_SRV_CM_STATE_DISCONNECTED,
    BT_SINK_SRV_CM_STATE_CONNECTING,
    BT_SINK_SRV_CM_STATE_CONNECTED,
    BT_SINK_SRV_CM_STATE_DISCONNECTING
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

#define BT_SINK_SRV_PROFILE_INDEX_HFP                          (0) /**< Profile type: Hands-free Profile(HFP). */
#define BT_SINK_SRV_PROFILE_INDEX_A2DP_SINK           (1)  /**< Profile type: Advanced Audio Distribution Profile(A2DP) as sink. */
#define BT_SINK_SRV_PROFILE_INDEX_AVRCP                    (2) /**< Profile type: Audio/Video Remote Control Profile(AVRCP). */
#define BT_SINK_SRV_PROFILE_INDEX_PBAPC                    (3) /**< Profile type: Audio/Video Remote Control Profile(PBAPC). */
#define BT_SINK_SRV_PROFILE_INDEX_AWS                         (4) /**< Profile type: Advanced Wireless Stero Profile(AWS). */
typedef uint8_t bt_sink_srv_cm_profile_index_t;     /**<The feature configuration of sink service. */

#define BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING          0x01
//#define BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_GETTING            0x02
typedef uint8_t bt_sink_srv_cm_remote_flag_t;

typedef struct {
    bt_bd_addr_t bt_addr;
} bt_sink_srv_cm_common_record;

typedef struct {
    bt_sink_srv_cm_state_t link_state; /*ACL link state*/
    bt_gap_connection_handle_t handle;
    bt_bd_addr_t addr;
    bt_sink_srv_profile_type_t profile_connection_mask;
    bt_sink_srv_gap_role_type_t local_role;
    bt_sink_srv_cm_remote_flag_t flag;
    bt_sink_srv_state_t device_state;
    void* profile_info[BT_SINK_SRV_CM_MAX_PROFILE_NUMBER];
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
    bt_sink_srv_device_type_t dev_type;
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
    bt_sink_srv_cm_flags_t flags;
    uint8_t connected_dev_num;
    uint8_t le_connected_dev_num;
    bt_sink_srv_cm_node_t *request_pending;
    bt_sink_srv_cm_node_t request_list;
    bt_gap_scan_mode_t scan_mode;
    bt_sink_srv_cm_remote_device_t devices[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
} bt_sink_srv_cm_context_t;

bt_sink_srv_status_t bt_sink_srv_cm_action_handler(bt_sink_srv_action_t action, void *param);
bool bt_sink_srv_cm_get_profile_data(bt_bd_addr_t *bt_addr, bt_sink_srv_profile_type_t profile, void *data_p);
bool bt_sink_srv_cm_set_profile_data(bt_bd_addr_t *bt_addr, bt_sink_srv_profile_type_t profile, void *data_p);
bool bt_sink_srv_cm_is_valid_address(bt_bd_addr_t *address);
uint32_t bt_sink_srv_cm_get_connected_device(bt_sink_srv_profile_type_t profile, bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER]);
bt_bd_addr_t *bt_sink_srv_cm_last_connected_device(void);
void bt_sink_srv_cm_set_role(bt_bd_addr_t *address_p, bt_role_t role);
void bt_sink_srv_cm_profile_status_notify(bt_bd_addr_t *addr, bt_sink_srv_profile_type_t profile, bt_sink_srv_profile_connection_state_t state);
bt_sink_srv_status_t bt_sink_srv_cm_read_local_address(uint8_t *local_addr);
bt_status_t bt_sink_srv_cm_gap_callback(bt_msg_type_t msg, bt_status_t status, void *param);
bt_status_t bt_sink_srv_cm_system_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);
bt_sink_srv_state_t bt_sink_srv_cm_find_device_handle_by_address(bt_bd_addr_t *address);
void* bt_sink_srv_cm_get_hf_info(uint8_t device_idx);
void* bt_sink_srv_cm_get_pbapc_info(uint8_t device_idx);
void* bt_sink_srv_cm_get_avrcp_info(uint8_t device_idx);
void* bt_sink_srv_cm_get_a2dp_sink_info(uint8_t device_idx);
void* bt_sink_srv_cm_get_aws_info(uint8_t device_idx);
void *bt_sink_srv_cm_get_profile_info(bt_bd_addr_t *address, bt_sink_srv_profile_type_t  profile);
bt_bd_addr_t *bt_sink_srv_cm_get_address_by_profile_info(void *profile_info);
bt_sink_srv_state_t bt_sink_srv_cm_set_device_state(bt_bd_addr_t *address, bt_sink_srv_state_t state);
bt_sink_srv_gap_role_type_t bt_sink_srv_cm_get_gap_role(bt_bd_addr_t *address_p);
void bt_sink_srv_cm_disconnect_by_type(bt_sink_srv_cm_device_type_t type);
bt_sink_srv_cm_flags_t bt_sink_srv_cm_get_flags(void);
void bt_sink_srv_cm_set_flags(bt_sink_srv_cm_flags_t flags);
void bt_sink_srv_cm_reset_flags(bt_sink_srv_cm_flags_t flags);
bool bt_sink_srv_cm_if_permit_aws_connection(bt_bd_addr_t *address);
#ifdef __cplusplus
}
#endif

#endif
