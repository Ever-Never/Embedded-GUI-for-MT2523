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
#include "bt_sink_srv_conmgr.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_common.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_sink_srv_hf.h"

#ifdef __BT_AWS_SUPPORT__
#include "bt_sink_srv_aws.h"

uint32_t bt_sink_srv_cm_aws_handle = 0;

bt_sink_srv_status_t bt_sink_srv_aws_system_send_sink_state(bt_sink_srv_state_t state)
{
    bt_sink_srv_status_t result = BT_SINK_SRV_STATUS_FAIL;
    if (bt_sink_srv_cm_aws_handle  != 0) {
        bt_sink_srv_aws_cmd_t *aws_cmd  = NULL;
        aws_cmd = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_aws_cmd_t) + 2  - 1);
        aws_cmd->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_SYSTEM_SINK_STATE;
        aws_cmd->type = BT_SINK_SRV_AWS_COMMAND_TYPE_REQ;
        aws_cmd->data_len = 2;
        aws_cmd->accept = 0;
        aws_cmd->data[0] = (uint8_t)(state & 0x00FF);
        aws_cmd->data[1] = (uint8_t)((state & 0xFF00) >> 8);
        bt_status_t ret = bt_sink_srv_aws_send_cmd(
        bt_sink_srv_cm_aws_handle, BT_SINK_SRV_AWS_MODULE_ID_SYSTEM, aws_cmd);
        if (ret == BT_STATUS_SUCCESS) {
            bt_sink_srv_report("[Sink][AWS_SYSTEM] send state: 0x%04x",state);
            result = BT_SINK_SRV_STATUS_SUCCESS;
        }
        bt_sink_srv_memory_free((void*)aws_cmd);
    }
    return result;
}

void bt_sink_srv_aws_system_receive_sink_state(uint8_t *data, uint16_t data_len)
{
    if (data_len != sizeof(bt_sink_srv_state_t)) {
        bt_sink_srv_report("[Sink][AWS_SYSTEM] wrong state len.");
        return;
    }  
    bt_sink_srv_state_t state = ((uint16_t)data[0] & 0x00FF) |(((uint16_t)data[1] & 0x00FF) << 8);
    bt_sink_srv_report("[Sink][AWS_SYSTEM] received state: 0x%x",state);
    bt_sink_srv_state_set(state);
    return;
}

bt_sink_srv_status_t bt_sink_srv_aws_system_send_power_off(void)
{
    bt_sink_srv_status_t result = BT_SINK_SRV_STATUS_FAIL;
    const bt_sink_srv_features_config_t *config_p = bt_sink_srv_get_features_config();
    if (config_p->features & BT_SINK_SRV_FEATURE_SYNC_POWER_OFF) {
        if (bt_sink_srv_cm_aws_handle  != 0) {
            bt_sink_srv_aws_cmd_t aws_cmd ;
            aws_cmd.cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_SYSTEM_POWER_OFF;
            aws_cmd.type = BT_SINK_SRV_AWS_COMMAND_TYPE_REQ;
            aws_cmd.accept = 0;
            aws_cmd.data_len = 0;
            bt_status_t ret = bt_sink_srv_aws_send_cmd(
                bt_sink_srv_cm_aws_handle, BT_SINK_SRV_AWS_MODULE_ID_SYSTEM,&aws_cmd);
            if (ret == BT_STATUS_SUCCESS) {
                result = BT_SINK_SRV_STATUS_SUCCESS;
            }
        }
    }
    return result;
}

bt_sink_srv_status_t bt_sink_srv_aws_system_receive_power_off(void)
{
    bt_sink_srv_cm_disconnect_by_type(BT_SINK_SRV_PROFILE_AWS);
    bt_sink_srv_cm_set_flags(BT_SINK_SRV_FLAG_SYSTEM_POWER_OFF);
    return BT_SINK_SRV_STATUS_SUCCESS;
}

#if 0
bt_sink_srv_status_t bt_sink_srv_aws_system_send_key(bt_sink_srv_key_information_t* key)
{
    bt_sink_srv_status_t result = BT_SINK_SRV_STATUS_FAIL;
    if (bt_sink_srv_cm_aws_handle  != 0) {
        bt_sink_srv_aws_cmd_t aws_cmd ;
        aws_cmd.cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_SYSTEM_KEY;
        aws_cmd.type = BT_SINK_SRV_AWS_COMMAND_TYPE_REQ;
        aws_cmd.accept = 0;
        aws_cmd.data_len = 2;
        aws_cmd.data[0] = key->value;
        aws_cmd.data[1] = key->action;
        bt_status_t ret = bt_sink_srv_aws_send_cmd(
        bt_sink_srv_cm_aws_handle, BT_SINK_SRV_AWS_MODULE_ID_SYSTEM,&aws_cmd);
        if (ret == BT_STATUS_SUCCESS) {
            result = BT_SINK_SRV_STATUS_SUCCESS;
        }
    }
    return result;
}

void bt_sink_srv_aws_system_receive_key(bt_sink_srv_key_value_t value, bt_sink_srv_key_action_t action)
{
    bt_sink_srv_event_param_t *event = NULL;
    event = bt_sink_srv_memory_alloc(sizeof(*event));
    event->key_info.value = value;
    event->key_info.action = action;
    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_REMOTE_KEY_RECEIVED, event);
    bt_sink_srv_memory_free(event);
    return;
}
#endif

bt_sink_srv_status_t bt_sink_srv_aws_system_send_sink_action(bt_sink_srv_action_t action)
{
    bt_sink_srv_status_t result = BT_SINK_SRV_STATUS_FAIL;
    bt_sink_srv_action_t send_action = action;
    if (bt_sink_srv_cm_aws_handle  != 0) {
        bt_sink_srv_aws_cmd_t *aws_cmd  = NULL;
        aws_cmd = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_aws_cmd_t) + 4  - 1);
        aws_cmd->cmd_id = BT_SINK_SRV_AWS_COMMAND_ID_SYSTEM_SINK_ACTION;
        aws_cmd->type = BT_SINK_SRV_AWS_COMMAND_TYPE_REQ;
        aws_cmd->accept = 0;
        aws_cmd->data_len = 4;
        aws_cmd->data[0] = (uint8_t)(send_action & 0x000000FF);
        aws_cmd->data[1] = (uint8_t)((send_action & 0x0000FF00) >> 8);
        aws_cmd->data[2] = (uint8_t)((send_action & 0x00FF0000) >> 16);
        aws_cmd->data[3] = (uint8_t)((send_action & 0xFF000000)>> 24);
        bt_status_t ret = bt_sink_srv_aws_send_cmd(
        bt_sink_srv_cm_aws_handle, BT_SINK_SRV_AWS_MODULE_ID_SYSTEM, aws_cmd);
        if (ret == BT_STATUS_SUCCESS) {
            result = BT_SINK_SRV_STATUS_SUCCESS;
        }
        bt_sink_srv_memory_free((void*)aws_cmd);
    }
    return result;
}

void bt_sink_srv_aws_system_receive_sink_action(uint8_t *data, uint16_t data_len)
{
    if (data_len != sizeof(bt_sink_srv_action_t*)) {
        bt_sink_srv_report("[Sink][AWS_SYSTEM] wrong data len.");
        return;
    }  
    bt_sink_srv_action_t action = ((uint32_t)data[0] & 0x000000FF) |(((uint32_t)data[1] & 0x000000FF) << 8) |
                                                       (((uint32_t)data[2] & 0x000000FF) << 16) |(((uint32_t) data[3] & 0x000000FF)<< 24);
    bt_sink_srv_event_param_t *event = NULL;
    event = bt_sink_srv_memory_alloc(sizeof(*event));
    event->remote_sink_action = action;
    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_REMOTE_SINK_ACTION_RECEIVED, event);
    bt_sink_srv_memory_free(event);
    return;
}

bt_status_t  bt_sink_srv_aws_system_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    switch (msg) {
        case BT_AWS_CONNECT_CNF: 
        {
            bt_aws_connect_cnf_t *conn_cnf = (bt_aws_connect_cnf_t *)buffer;
            if (conn_cnf->status == BT_STATUS_SUCCESS) {
                bt_sink_srv_cm_aws_handle = conn_cnf->handle;
                // sync call state to remote aws remote if a call exist.
                bt_sink_srv_state_t state = bt_sink_srv_state_query();
                if (state & BT_SINK_SRV_HF_CALL_STATE_ALL) {
                    bt_sink_srv_report("[Sink][AWS_SYSTEM] call is exist, sync call state to remote.");
                    bt_sink_srv_aws_system_send_sink_state(state);
                }
            } 
        }
        break;

        case BT_AWS_DISCONNECT_CNF:
        case BT_AWS_DISCONNECT_IND: 
        {  //contiune to disconnect SP
            if (bt_sink_srv_cm_get_flags() & BT_SINK_SRV_FLAG_SYSTEM_POWER_OFF) {
                bt_sink_srv_cm_disconnect_by_type(BT_SINK_SRV_CM_PROFILE_SP);
            }
            bt_sink_srv_cm_aws_handle = 0;
        }    
        break;
        
        case BT_AWS_COMMAND_IND: 
        {
            bt_aws_command_result_t* result = (bt_aws_command_result_t*)buffer;
            if (result->type == BT_SINK_SRV_AWS_COMMAND_TYPE_REQ ) {
                if (result->cmd_ind.cmd_id == BT_SINK_SRV_AWS_COMMAND_ID_SYSTEM_POWER_OFF) {
                    bt_sink_srv_aws_system_receive_power_off();
                } else if (result->cmd_ind.cmd_id == BT_SINK_SRV_AWS_COMMAND_ID_SYSTEM_KEY) {
                   // bt_sink_srv_aws_system_receive_key(result->cmd_ind.data[0], result->cmd_ind.data[1]);
                } else if (result->cmd_ind.cmd_id == BT_SINK_SRV_AWS_COMMAND_ID_SYSTEM_SINK_ACTION) {
                    bt_sink_srv_aws_system_receive_sink_action(result->cmd_ind.data, result->cmd_ind.data_len);
                } else if (result->cmd_ind.cmd_id == BT_SINK_SRV_AWS_COMMAND_ID_SYSTEM_SINK_STATE) {
                    bt_sink_srv_aws_system_receive_sink_state(result->cmd_ind.data, result->cmd_ind.data_len);
                }
            }
        }         
        break;

        default:
        break;
    }
    
    return BT_STATUS_SUCCESS;
}

#endif /* __BT_AWS_SUPPORT__ */

