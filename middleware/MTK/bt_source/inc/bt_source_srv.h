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

#ifndef __BT_SOURCE_SRV_H__
#define __BT_SOURCE_SRV_H__
#include <stdint.h>
#include <stdbool.h>
#ifndef WIN32
#include <syslog.h>
#else
#include "osapi.h"
#endif
#include "FreeRTOSConfig.h"
#include "bt_type.h"
#include "bt_avrcp.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BT_SOURCE_SRV_CB_MAX_NUM                 (5)
#define BT_SOURCE_SRV_SBC_SHARE_SIZE             (40*1024)
#define BT_SOURCE_SRV_A2DP_PAYLOAD_SIZE          (1*1024)

typedef struct {
    uint32_t handle;
    bt_status_t  result;
    bt_bd_addr_t address;
} bt_source_srv_a2dp_connect_t;

typedef struct {
    uint32_t handle;
    bt_status_t result;
} bt_source_srv_a2dp_start_cnf_t;

typedef struct {
    uint32_t handle;
} bt_source_srv_a2dp_start_ind_t;

typedef struct {
    uint32_t handle;
    bt_avrcp_operation_id_t op_id;
} bt_source_srv_avrcp_operation_ind_t;

typedef struct{
    uint32_t handle;      /**< The connection handle of the AVRCP. */
    uint8_t  volume;      /**< The absolute volume. Values is a percentage from 0% to 100%. Other values are invalid. */
}bt_source_srv_avrcp_change_volume_cnf_t;

typedef struct{
    uint32_t handle;     /**< The connection handle of the AVRCP. */   
    bt_status_t  result;
    bt_bd_addr_t address;
}bt_source_srv_avrcp_connect_t;

typedef enum {
    BT_SOURCE_SRV_EVENT_A2DP_CONNECT = 0,     /* CNF of bt_source_srv_a2dp_connect. */
    BT_SOURCE_SRV_EVENT_A2DP_DISCONNECT = 1,  /* CNF of bt_source_srv_a2dp_disconnect. */
    BT_SOURCE_SRV_EVENT_A2DP_START_CNF = 2,   /* CNF of bt_source_srv_a2dp_start. */
    BT_SOURCE_SRV_EVENT_A2DP_STOP_CNF = 3,    /* CNF of bt_source_srv_a2dp_stop. */
    BT_SOURCE_SRV_EVENT_AVRCP_CONNECT = 4,    /* CNF of bt_source_srv_avrcp_connect. */
    BT_SOURCE_SRV_EVENT_AVRCP_DISCONNECT = 5, /* CNF of bt_source_srv_avrcp_disconnect. */
    BT_SOURCE_SRV_EVENT_AVRCP_OPERATION_IND = 6,    /* like play pause pre next. */
    BT_SOURCE_SRV_EVENT_AVRCP_CHANGE_VOLUME_CNF = 7,/* CNF of @bt_source_srv_avrcp_change_volume. */
    BT_SOURCE_SRV_EVENT_AVRCP_REMOTE_VOLUME_IND = 8,/* IND of volume setting comes from remote device. */
    BT_SOURCE_SRV_EVENT_A2DP_START_IND = 9,   /* IND of A2DP Start comes from remote device. */
    BT_SOURCE_SRV_EVENT_A2DP_STOP_IND = 10,   /* IND of A2DP Stop comes from remote device. */
    /* add before this event. */
    BT_SOURCE_SRV_EVENT_ALL
} bt_source_srv_event_t;

typedef bt_source_srv_a2dp_connect_t bt_source_srv_a2dp_disconnect_t;
typedef bt_source_srv_a2dp_start_cnf_t bt_source_srv_a2dp_stop_cnf_t;
typedef bt_source_srv_a2dp_start_ind_t bt_source_srv_a2dp_stop_ind_t;
typedef bt_source_srv_avrcp_connect_t bt_source_srv_avrcp_disconnect_t;
typedef bt_source_srv_avrcp_change_volume_cnf_t bt_source_srv_avrcp_remote_volume_ind_t;

typedef void(* bt_source_srv_notify_callback)(bt_source_srv_event_t event_id, void *param);

void bt_source_srv_init(void);
bt_status_t bt_source_srv_register_callback(bt_source_srv_notify_callback callback);
bt_status_t bt_source_srv_deregister_callback(bt_source_srv_notify_callback callback);
bt_status_t bt_source_srv_event_callback(bt_source_srv_event_t event_id, void *param);
bt_status_t bt_source_srv_avrcp_change_volume(uint32_t handle, uint8_t volume);

bt_status_t bt_source_srv_a2dp_start(uint32_t handle);
bt_status_t bt_source_srv_a2dp_stop(uint32_t handle);
bt_status_t bt_source_srv_a2dp_connect(bt_bd_addr_t *address);
bt_status_t bt_source_srv_a2dp_disconnect(uint32_t handle);

bt_status_t bt_source_srv_avrcp_connect(bt_bd_addr_t *device_addr);
bt_status_t bt_source_srv_avrcp_disconnect(uint32_t handle);

#ifdef __cplusplus
}
#endif

#endif /* __BT_SOURCE_SRV_H__ */

