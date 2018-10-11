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

/*****************************************************************************
 *
 * Description:
 * ------------
 * The header file of the libbtnotify.a.
 *
 ****************************************************************************/



#ifndef __BT_NOTIFY_PRIVATE_H__
#define __BT_NOTIFY_PRIVATE_H__
#include "stdbool.h"
#include <stdint.h>
#include "bt_notify.h"
#include "bt_notify_list.h"

/*The header file is for libnotify.a, please do not change it.*/

/* register callback return code enum */
#define BT_NOTIFY_MAX_HEADER_LEN 60
#define BT_NOTIFY_MAX_SUPPORTED_REGISTER_COUNT 10
#define BT_NOTIFY_RECEIVE_BUF_SIZE 1024 // max length will not larger than 1024


typedef enum {
    BT_NOTIFY_STATUS_ENUM_IDLE = 0,         /* no available data in recv_buf*/
    BT_NOTIFY_STATUS_ENUM_READING_CMD,      /* reading command */
    BT_NOTIFY_STATUS_ENUM_READING_DATA,     /* reading data*/
    BT_NOTIFY_STATUS_ENUM_PARSING,          /* parsing data*/
    BT_NOTIFY_STATUS_ENUM_READING_EXT_DATA,   /* reading data*/
} bt_notify_status_t;
typedef enum {
    BT_NOTIFY_CMD_TYPE_ENUM_INVALID = -1,
    BT_NOTIFY_CMD_TYPE_ENUM_DATA,
    BT_NOTIFY_CMD_TYPE_ENUM_SYNC,
    BT_NOTIFY_CMD_TYPE_ENUM_ACKY,
    BT_NOTIFY_CMD_TYPE_ENUM_ACKN,
    BT_NOTIFY_CMD_TYPE_ENUM_VERN,
    BT_NOTIFY_CMD_TYPE_ENUM_CMD1,  /* just reserved */
} bt_notify_cmd_type_t;


typedef struct {
    char cmd[5];
    char sender_id[BT_NOTIFY_MAX_SENDER_RECEIVER_ID_LENGTH];
    bt_notify_callback_t callback;
    bool exist;
} bt_notify_register_callback_t;

typedef struct {
    bool is_connect;
    bool is_sync;
    bool is_vern_matched;
    bool connect_fail;
    bool is_ver_send;
    bool is_wait_timer_started;
    bool is_receiving_timer_started;

    uint32_t apk_vern;
    bt_notify_status_t status;
    bt_notify_cmd_type_t cmd_type;
    bool buffer_to_read;

    uint16_t conn_id;
    uint32_t handle;
    
    uint8_t sdp_id;

    
    uint16_t max_packet_length;
    bt_bd_addr_t bt_addr;
    uint32_t source;

    uint32_t xml_buffer_size;
    uint32_t xml_file_wrritten_size;
    uint32_t ext_recv_buf_size;

    uint8_t recv_buf[BT_NOTIFY_RECEIVE_BUF_SIZE];
    int32_t recv_buf_len;

    int32_t cmd_buf_len;
    uint8_t cmd_buf_plain[BT_NOTIFY_MAX_HEADER_LEN];

    uint8_t send_buf[BT_NOTIFY_RECEIVE_BUF_SIZE];

    char cmd[5];
    bt_notify_register_callback_t register_callback[BT_NOTIFY_MAX_SUPPORTED_REGISTER_COUNT];
    bt_notify_remote_system_type_t remote_system;


    uint8_t ring_buf[BT_NOTIFY_RECEIVE_BUF_SIZE];
    int32_t ring_buf_len;
} bt_notify_context;

bool bt_notify_check_channel_valid(int32_t cntx_chnl);
bt_notify_context * bt_notify_get_cntx(uint8_t cntx_chnl);
/**
 * This function is for getting device addr by channel index.
 * cntx_chnl         is the channel index.
 * return            is the BT address of remote device          .
 * note
 *
 */

bt_bd_addr_t *bt_notify_get_bt_device_addr(uint8_t cntx_chnl);
/**
 * This function is for getting app callback.
 * cntx_chnl         is the channel index.
 * return            is the callback structure.          .
 * note
 *
 */

bt_notify_callback_data_t *bt_notify_get_callback_struct(uint8_t cntx_chnl);
/**
 * This function is for reset the ststus of receiving the data.
 * cntx_chnl         is the channel index.
 * return            is the result of the status.            .
 * note
 *
 */

bt_notify_result_t bt_notify_reset_parse_status(uint8_t cntx_chnl);

/**
 * This function is for callback app.
 * cntx_chnl         is the channel index.
 * return                        .
 * note
 *
 */

void bt_notify_callback_app(uint8_t cntx_chnl);
int32_t bt_notify_find_cntx_channel_by_handle(uint32_t handle);
void bt_notify_handle_ready_to_write_ind(void *para, bt_notify_data_source_t source);
void bt_notify_handle_ready_to_read_ind(void *para, bt_notify_data_source_t source);
void bt_notify_spp_connect_ind(bt_spp_connect_ind_t *parameter);
void bt_notify_handle_connect_cnf(bt_status_t status, void* para, bt_notify_data_source_t source);
void bt_notify_handle_disconnect_ind(bt_status_t status,void *prameter, bt_notify_data_source_t source);
bool bt_notify_check_handle_valid(uint32_t handle);
Lnode * bt_notify_get_head(void);
uint8_t bt_notify_get_sdp_id(void);


#endif/*__BT_NOTIFY_PRIVATE_H__*/

