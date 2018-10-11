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

#include "bt_sink_srv.h"

#ifdef __BT_AWS_SUPPORT__
#include "bt_sink_srv_aws.h"
#include "bt_sink_srv_conmgr.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_common.h"
#include "bt_device_manager.h"
#include "bt_sink_srv_aws_system.h"

extern bt_status_t bt_sink_srv_music_aws_a2dp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);

extern bt_status_t bt_sink_srv_aws_mp3_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);

extern bt_status_t  bt_sink_srv_aws_system_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);

extern bt_status_t  bt_sink_srv_aws_hf_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);



bt_status_t default_bt_sink_srv_music_aws_a2dp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    return BT_STATUS_SUCCESS;
}

bt_status_t default_bt_sink_srv_aws_mp3_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    return BT_STATUS_SUCCESS;
}

bt_status_t default_bt_sink_srv_aws_hf_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    return BT_STATUS_SUCCESS;
}

bt_status_t default_bt_sink_srv_aws_system_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    return BT_STATUS_SUCCESS;
}



#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_sink_srv_music_aws_a2dp_common_callback=_bt_sink_srv_music_aws_a2dp_common_callback")
#pragma comment(linker, "/alternatename:_bt_sink_srv_aws_mp3_common_callback=_bt_sink_srv_aws_mp3_common_callback")
#pragma comment(linker, "/alternatename:_bt_sink_srv_aws_hf_common_callback=_default_bt_sink_srv_aws_hf_common_callback")
#pragma comment(linker, "/alternatename:_bt_sink_srv_aws_system_common_callback=_default_bt_sink_srv_aws_system_common_callback")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_sink_srv_music_aws_a2dp_common_callback = default_bt_sink_srv_music_aws_a2dp_common_callback
#pragma weak bt_sink_srv_aws_mp3_common_callback = default_bt_sink_srv_aws_mp3_common_callback
#pragma weak bt_sink_srv_aws_hf_common_callback = default_bt_sink_srv_aws_hf_common_callback
#pragma weak bt_sink_srv_aws_system_common_callback = default_bt_sink_srv_aws_system_common_callback
#else
#error "Unsupported Platform"
#endif


bt_sink_srv_aws_context_t aws_cntx[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];


static const bt_sink_srv_aws_cb_t aws_cb[] = 
{
    {
        BT_SINK_SRV_AWS_MODULE_ID_A2DP,
        bt_sink_srv_music_aws_a2dp_common_callback
    },
    {
        BT_SINK_SRV_AWS_MODULE_ID_MP3,
        bt_sink_srv_aws_mp3_common_callback
    },
    {
        BT_SINK_SRV_AWS_MODULE_ID_CALL,
        bt_sink_srv_aws_hf_common_callback
    },
    {
        BT_SINK_SRV_AWS_MODULE_ID_SYSTEM,
        bt_sink_srv_aws_system_common_callback
    }
};

void bt_sink_srv_aws_set_state(bt_sink_srv_aws_context_t *cntx_p, bt_sink_srv_aws_state_t state)
{
    bt_sink_srv_report("[sink][aws]set_state,ori:%d, curr:%d", cntx_p->state, state);

    cntx_p->state = state;
}


static void bt_sink_srv_aws_init()
{
    int i;

    for(i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
        aws_cntx[i].aws_handle = 0;
        bt_sink_srv_aws_set_state(&aws_cntx[i], BT_SINK_SRV_AWS_STATE_READY);
    }
}

static void bt_sink_srv_aws_deinit()
{
    int i = 0;
    for(i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
        aws_cntx[i].aws_handle = 0;
        bt_sink_srv_aws_set_state(&aws_cntx[i], BT_SINK_SRV_AWS_STATE_NONE);
    }
}

static bool bt_sink_srv_aws_handle_connect_ind(bt_aws_connect_ind_t *conn_ind)
{
    bool ret = false;
    bt_sink_srv_report("[sink][aws]connect_ind-aws_hd: 0x%x", conn_ind->handle);

    if (bt_sink_srv_cm_if_permit_aws_connection(conn_ind->address)) {
        if (BT_STATUS_SUCCESS == bt_aws_connect_response(conn_ind->handle, true)) {
            ret = true;
        }
    } else {
        bt_aws_connect_response(conn_ind->handle, false);
        bt_device_manager_delete_paired_device((bt_bd_addr_ptr_t)conn_ind->address);
    }
    bt_sink_srv_report("[sink][aws]connect_ind-ret: %d", ret);
    return ret;
}

void* bt_sink_srv_cm_get_aws_info(uint8_t device_idx)
{
    return &(aws_cntx[device_idx]);
}


bt_sink_srv_aws_context_t * bt_sink_srv_aws_get_cntx_by_handle(uint32_t handle)
{
    int i = 0;
    for(i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
        if (aws_cntx[i].aws_handle == handle) {
            return &aws_cntx[i];
        }
    }
    return NULL;
}


bt_bd_addr_t *bt_sink_srv_aws_get_address_by_handle(uint32_t handle)
{
    bt_bd_addr_t * bd_addr = NULL;
    int i = 0;
    for(i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
        if (aws_cntx[i].aws_handle == handle) {
            bd_addr = bt_sink_srv_cm_get_address_by_profile_info(&aws_cntx[i]);

            return bd_addr;
        }
    }

    return NULL;
}

bt_status_t bt_sink_srv_aws_send_cmd(
                                uint32_t handle,
                                bt_aws_module_id_t module_id,
                                bt_sink_srv_aws_cmd_t *aws_cmd)

{
    uint16_t total_len;

    total_len = sizeof(bt_sink_srv_aws_cmd_t) + aws_cmd->data_len - 1;
    return bt_aws_send_command(handle, module_id, (const uint8_t *)aws_cmd, total_len);

}


void bt_sink_srv_aws_notify_all(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    uint32_t i = 0;
    for(i = 0; i < sizeof(aws_cb) /sizeof(bt_sink_srv_aws_cb_t); i++)
    {
        if (aws_cb[i].aws_callback != NULL) {
            aws_cb[i].aws_callback(msg, status, buffer);
        }
    }    
}


bt_status_t bt_sink_srv_aws_cmd_handle(bt_aws_command_ind_t *cmd_ind, bt_aws_command_result_t *cmd_result)
{
    uint16_t command_id;
    bt_sink_srv_cmd_type_t cmd_type;
    uint16_t curr_pos = 0;

    //cmd_id: 2;
    // type: 1;
    // accept: 1;
    // data_len: 2;
    // data:data_len;
    
    memcpy(&command_id, &cmd_ind->data[curr_pos], 2);
    curr_pos += 2;

    cmd_result->handle = cmd_ind->handle;
    
    bt_sink_srv_report("[sink][aws] cmd_handle1, command_id:%d", command_id);
    if (command_id > 0x00FF) {
        cmd_result->type = BT_SINK_SRV_AWS_COMMAND_TYPE_CUSTOMIZE;
        bt_sink_srv_memcpy(&(cmd_result->cmd), cmd_ind, sizeof(bt_aws_command_ind_t));
        return BT_STATUS_SUCCESS;
    }

    cmd_type = cmd_ind->data[curr_pos];
    curr_pos += 1; 

    if (cmd_type == BT_SINK_SRV_AWS_COMMAND_TYPE_REQ) {
        cmd_result->type = BT_SINK_SRV_AWS_COMMAND_TYPE_REQ;
        cmd_result->cmd_ind.cmd_id = command_id;
        curr_pos += 1;//accept
        memcpy(&(cmd_result->cmd_ind.data_len), &cmd_ind->data[curr_pos], 2);
        curr_pos += 2; //2:data_len, 
        cmd_result->cmd_ind.data = &cmd_ind->data[curr_pos];
        return BT_STATUS_SUCCESS;
    }

    if (cmd_type == BT_SINK_SRV_AWS_COMMAND_TYPE_RSP) {
        cmd_result->type = BT_SINK_SRV_AWS_COMMAND_TYPE_RSP;
        cmd_result->cmd_rsp.cmd_id = command_id;
        cmd_result->cmd_rsp.accept = cmd_ind->data[curr_pos];
        curr_pos += 1;
        memcpy(&(cmd_result->cmd_rsp.data_len), &cmd_ind->data[curr_pos], 2);
        curr_pos += 2; //2:data_len, 
        cmd_result->cmd_rsp.data = &cmd_ind->data[curr_pos];
        return BT_STATUS_SUCCESS;
    }

    return BT_STATUS_FAIL;
}


bt_status_t bt_sink_srv_aws_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    int32_t ret = 0;

    switch (msg) {
        case BT_AWS_CONNECT_IND: {
            bt_aws_connect_ind_t *conn_ind = (bt_aws_connect_ind_t *)buffer;
            if (bt_sink_srv_aws_handle_connect_ind(conn_ind)){
                bt_sink_srv_aws_context_t *aws_cntx_p = (bt_sink_srv_aws_context_t *)bt_sink_srv_cm_get_profile_info(conn_ind->address, BT_SINK_SRV_PROFILE_AWS);
                aws_cntx_p->aws_handle = conn_ind->handle;
                bt_sink_srv_cm_profile_status_notify(conn_ind->address, BT_SINK_SRV_PROFILE_AWS, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTING);
            }
            break;
        }
        
        case BT_AWS_CONNECT_CNF: {
            bt_aws_connect_cnf_t *conn_cnf = (bt_aws_connect_cnf_t *)buffer;
            bt_sink_srv_aws_context_t *aws_cntx_p = bt_sink_srv_aws_get_cntx_by_handle(conn_cnf->handle);
            bt_bd_addr_t * addr = bt_sink_srv_cm_get_address_by_profile_info(aws_cntx_p);
            
            if (conn_cnf->status == BT_STATUS_SUCCESS) {
                bt_sink_srv_aws_set_state(aws_cntx_p, BT_SINK_SRV_AWS_STATE_CONNECTED);
                bt_sink_srv_cm_profile_status_notify(addr, BT_SINK_SRV_PROFILE_AWS, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED);
                bt_sink_srv_aws_notify_all(msg, status, buffer);
            } else {
                bt_sink_srv_aws_set_state(aws_cntx_p, BT_SINK_SRV_AWS_STATE_READY);
                bt_sink_srv_cm_profile_status_notify(addr, BT_SINK_SRV_PROFILE_AWS, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED);
            }
            break;
        }
        case BT_AWS_DISCONNECT_CNF: {
            bt_aws_disconnect_cnf_t *dis_cnf = (bt_aws_disconnect_cnf_t *)buffer;
            bt_sink_srv_aws_context_t *aws_cntx_p = bt_sink_srv_aws_get_cntx_by_handle(dis_cnf->handle);
            bt_bd_addr_t * addr = bt_sink_srv_cm_get_address_by_profile_info(aws_cntx_p);
            bt_sink_srv_cm_profile_status_notify(addr, BT_SINK_SRV_PROFILE_AWS, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED);
            bt_sink_srv_aws_set_state(aws_cntx_p, BT_SINK_SRV_AWS_STATE_READY);
            bt_sink_srv_aws_notify_all(msg, status, buffer);
            break;
        }
        case BT_AWS_DISCONNECT_IND: {
            bt_aws_disconnect_ind_t *dis_conn = (bt_aws_disconnect_ind_t *)buffer;
            bt_sink_srv_aws_context_t *aws_cntx_p = bt_sink_srv_aws_get_cntx_by_handle(dis_conn->handle);
            bt_bd_addr_t * addr = bt_sink_srv_cm_get_address_by_profile_info(aws_cntx_p);
            bt_sink_srv_cm_profile_status_notify(addr, BT_SINK_SRV_PROFILE_AWS, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED);
            bt_sink_srv_aws_set_state(aws_cntx_p, BT_SINK_SRV_AWS_STATE_READY);
            bt_sink_srv_aws_notify_all(msg, status, buffer);
            break;
        }

        case BT_AWS_COMMAND_IND: {
            bt_aws_command_ind_t *command_ind = (bt_aws_command_ind_t *)buffer;
            bt_aws_command_result_t cmd_result;
            bt_status_t result;

            bt_sink_srv_report("[sink][aws] command_ind1, module_id:%d", command_ind->module_id);
            result = bt_sink_srv_aws_cmd_handle(command_ind, &cmd_result);
            bt_sink_srv_report("[sink][aws] command_ind2, result:%d, type:%d", result, cmd_result.type);
            aws_cb[command_ind->module_id].aws_callback(msg, status, &cmd_result);
            break;
        }

        case BT_AWS_NOTIFY_PLAY_IND: {
            bt_aws_notify_play_ind_t *play_ind = (bt_aws_notify_play_ind_t *)buffer;
            aws_cb[play_ind->module_id].aws_callback(msg, status, buffer);
            break;
        }
        
        case BT_AWS_STREAMING_PACKET_RECEIVED_IND: {
            bt_aws_streaming_received_ind_t *str_ind = (bt_aws_streaming_received_ind_t *)buffer;

            if (str_ind->packet.type == BT_AWS_STREAMING_TYPE_A2DP) {
                aws_cb[BT_SINK_SRV_AWS_MODULE_ID_A2DP].aws_callback(msg, status, buffer);
            }
            else {
                aws_cb[str_ind->module_id].aws_callback(msg, status, buffer);
            }
            break;
        } 
        case BT_AWS_OFFSET_UPDATE_IND: {
            bt_aws_offset_update_ind_t *str_ind = (bt_aws_offset_update_ind_t *)buffer;
            bt_sink_srv_music_device_t *aws_dev = NULL;
    
            aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS, (void *)AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP);
            if (aws_dev != NULL) {               
                aws_dev->offset.offset = str_ind->offset.offset;
                aws_dev->offset.offset_intra = str_ind->offset.offset_intra;
            }

            bt_sink_srv_aws_notify_all(msg, status, buffer);
            break;
        }
        default:
            break;
    }

    return ret;
}

bt_sink_srv_status_t bt_sink_srv_aws_action_handler(bt_sink_srv_action_t action, void *param)
{
    bt_sink_srv_status_t result = BT_SINK_SRV_STATUS_SUCCESS;
    bt_sink_srv_profile_connection_action_t *conn_info = NULL;
    bt_bd_addr_t *dev_addr = NULL;

    bt_sink_srv_report("[sink][aws] bt_sink_srv_aws_action_handler, action:%x", action);
    switch(action) {
        case BT_SINK_SRV_ACTION_PROFILE_INIT: {
            bt_sink_srv_aws_init();
            break;
        }
        case BT_SINK_SRV_ACTION_PROFILE_DEINIT: {
            bt_sink_srv_aws_deinit();
            break;
        }
        case BT_SINK_SRV_ACTION_PROFILE_CONNECT: {
            conn_info = (bt_sink_srv_profile_connection_action_t *)param;
            dev_addr = &(conn_info->address);
            bt_sink_srv_aws_context_t *aws_cntx_p = (bt_sink_srv_aws_context_t *)bt_sink_srv_cm_get_profile_info(dev_addr, BT_SINK_SRV_PROFILE_AWS);
            bt_sink_srv_report("[sink][aws] profile_conn,cntx:%x, state:%x", aws_cntx_p, aws_cntx_p->state);
            if ((conn_info->profile_connection_mask & BT_SINK_SRV_PROFILE_AWS) &&
                aws_cntx_p->state == BT_SINK_SRV_AWS_STATE_READY) {
                if (bt_sink_srv_cm_if_permit_aws_connection(dev_addr)) {
                    result = (bt_sink_srv_status_t)bt_aws_connect(&(aws_cntx_p->aws_handle), dev_addr);
                    if (result == BT_STATUS_SUCCESS) {
                        bt_sink_srv_cm_profile_status_notify(dev_addr, BT_SINK_SRV_PROFILE_AWS, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTING);
                    }
                }
            }
            break;
        }
        case BT_SINK_SRV_ACTION_PROFILE_DISCONNECT: {
            conn_info = (bt_sink_srv_profile_connection_action_t *)param;
            dev_addr = &(conn_info->address);
            bt_sink_srv_aws_context_t *aws_cntx_p = (bt_sink_srv_aws_context_t *)bt_sink_srv_cm_get_profile_info(dev_addr, BT_SINK_SRV_PROFILE_AWS);
            if ((conn_info->profile_connection_mask & BT_SINK_SRV_PROFILE_AWS) &&
                aws_cntx_p->state == BT_SINK_SRV_AWS_STATE_CONNECTED) {
                if (BT_STATUS_SUCCESS == bt_aws_disconnect(aws_cntx_p->aws_handle)) {
                    bt_sink_srv_cm_profile_status_notify(&(conn_info->address), BT_SINK_SRV_PROFILE_AWS, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTING);
                }
            }
            break;
        }
        case BT_SINK_SRV_ACTION_AWS_TRANSFER_SINK_ACTION: {
            bt_sink_srv_aws_system_send_sink_action( *(bt_sink_srv_action_t*)param);
            break;
        }
    }
    return result;
}

#endif /* __BT_AWS_SUPPORT__ */

