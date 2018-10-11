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
#include "bt_callback_manager.h"
#include "bt_callback_manager_config.h"
#if !defined(__MTK_BLE_ONLY_ENABLE__)
#include "bt_gap.h"
#include "bt_hfp.h"
#include "bt_a2dp.h"
#include "bt_aws.h"
#endif /* __MTK_BLE_ONLY_ENABLE__ */
#include "bt_gap_le.h"
#include "bt_gatts.h"
#include "bt_type.h"
#include "bt_debug.h"


typedef bt_status_t (*bt_app_event_callback_p)(bt_msg_type_t msg, bt_status_t status, void *buff);
#if !defined(__MTK_BLE_ONLY_ENABLE__)
typedef const bt_gap_config_t* (*bt_gap_get_local_configuration_p)(void);
typedef void (*bt_gap_get_link_key_p)(bt_gap_link_key_notification_ind_t* key_information);
typedef const bt_gap_pin_code_information_t* (*bt_gap_get_pin_code_p)(void);
#endif
typedef bool (*bt_gap_le_is_accept_connection_update_request_p)(bt_handle_t handle, bt_gap_le_connection_update_param_t* connection_parameter);
typedef bt_status_t (*bt_gap_le_get_pairing_config_p)(bt_gap_le_bonding_start_ind_t *ind);
typedef bt_gap_le_local_key_t * (*bt_gap_le_get_local_key_p)(void);
typedef bt_gap_le_bonding_info_t * (*bt_gap_le_get_bonding_info_p)(const bt_addr_t remote_addr);
typedef bt_gap_le_local_config_req_ind_t * (*bt_gap_le_get_local_config_p)(void);
typedef bt_status_t (*bt_gatts_get_authorization_check_result_p)(bt_gatts_authorization_check_req_t *req);
typedef bt_status_t (*bt_gatts_get_execute_write_result_p)(bt_gatts_execute_write_req_t *req);
#if !defined(__MTK_BLE_ONLY_ENABLE__)
typedef bt_status_t (*bt_hfp_get_init_params_p)(bt_hfp_init_param_t *init_params);
typedef bt_status_t (*bt_a2dp_get_init_params_p)(bt_a2dp_init_params_t *param);
#endif

typedef struct {
    bool in_use;
    uint32_t module_mask;
    bt_app_event_callback_p callback;
} bt_callback_manager_app_event_node_t;

typedef struct {
    bool in_use;
    void * callback;
} bt_callback_manager_normal_callback_node_t;

//User can config the max number of the app event callback.
static bt_callback_manager_app_event_node_t app_event_callback_list[BT_CALLBACK_MANAGER_APP_EVENT_CALLBACK_MAX] = {{0}};

//Only support one user for each type except app event callback.
static bt_callback_manager_normal_callback_node_t normal_callback_list[bt_callback_type_max - 1] = {{0}};

bt_status_t bt_callback_manager_register_callback(bt_callback_type_t type, uint32_t module_mask, void * callback)
{
    uint8_t i = 0;
    bt_status_t status = BT_STATUS_SUCCESS;
    BT_LOGI("BT_CMGR", "%s, type %d, module_mask %x, callback %x", __FUNCTION__, type, module_mask, callback);
    switch (type) {
        case bt_callback_type_app_event: {
            for (i = 0; i < BT_CALLBACK_MANAGER_APP_EVENT_CALLBACK_MAX; i++) {
                if (!app_event_callback_list[i].in_use) {
                    app_event_callback_list[i].callback = (bt_app_event_callback_p)callback;
                    app_event_callback_list[i].in_use = true;
                    app_event_callback_list[i].module_mask = module_mask;
                    break;
                }
            }
            if (i == BT_CALLBACK_MANAGER_APP_EVENT_CALLBACK_MAX) {
                BT_LOGI("BT_CMGR", "all are in use, please extend the value of BT_CALLBACK_MANAGER_APP_EVENT_CALLBACK_MAX");
                status = BT_STATUS_FAIL;
            }
            break;
        }
        default: {
            if (type < bt_callback_type_max && !normal_callback_list[type - 1].in_use) {
                normal_callback_list[type - 1].callback = callback;
                normal_callback_list[type - 1].in_use = true;
            } else {
                BT_LOGI("BT_CMGR", "wrong type or in use");
                status = BT_STATUS_FAIL;
            }
            break;
        }
            
    }
    return status;
}

bt_status_t bt_callback_manager_deregister_callback(bt_callback_type_t type, void * callback)
{
    uint8_t i = 0;
    bt_status_t status = BT_STATUS_SUCCESS;

    BT_LOGI("BT_CMGR", "%s, type %d, callback %x", __FUNCTION__, type, callback);
    switch (type) {
        case bt_callback_type_app_event: {
            for (i = 0; i < BT_CALLBACK_MANAGER_APP_EVENT_CALLBACK_MAX; i++) {
                if (app_event_callback_list[i].in_use && app_event_callback_list[i].callback == (bt_app_event_callback_p)callback) {
                    app_event_callback_list[i].callback = NULL;
                    app_event_callback_list[i].in_use = false;
                    app_event_callback_list[i].module_mask = 0;
                }
            }
            break;
        }
        default: {
            if (type < bt_callback_type_max && normal_callback_list[type - 1].in_use) {
                normal_callback_list[type - 1].callback = NULL;
                normal_callback_list[type - 1].in_use = false;
            } else {
                BT_LOGI("BT_CMGR", "wrong type or not in use");
                status = BT_STATUS_FAIL;
            }
            break;
        }
            
    }
    return status;

}

bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    uint8_t i = 0;
    bt_status_t ret = BT_STATUS_FAIL;
    uint32_t module_flag = MODULE_MASK_OFFSET(msg);

#if !defined(__MTK_BLE_ONLY_ENABLE__) 
    if (msg != BT_A2DP_STREAMING_RECEIVED_IND &&
        msg != BT_MEMORY_FREE_GARBAGE_IND &&
        msg != BT_AWS_STREAMING_PACKET_RECEIVED_IND) 
#endif
    {
        BT_LOGI("BT_CMGR", "%s, module_flag %d, msg %x", __FUNCTION__, module_flag, msg);
    }
    for (i = 0; i < BT_CALLBACK_MANAGER_APP_EVENT_CALLBACK_MAX; i++) {
        if (app_event_callback_list[i].in_use && app_event_callback_list[i].module_mask & module_flag) {
            ret = app_event_callback_list[i].callback(msg, status, buff);
        }
    }
    return ret;
}

#if !defined(__MTK_BLE_ONLY_ENABLE__) 
//must have a register
const bt_gap_config_t* bt_gap_get_local_configuration(void)
{
    bt_callback_type_t type = bt_callback_type_gap_get_local_configuration;
    bt_gap_get_local_configuration_p callback = (bt_gap_get_local_configuration_p)(normal_callback_list[type - 1].callback);

    BT_LOGI("BT_CMGR", "%s, in use %d", __FUNCTION__, normal_callback_list[type - 1].in_use);
    if (normal_callback_list[type - 1].in_use) {
        return callback();
    }
    
    assert(0);
    return NULL;
}

void bt_gap_get_link_key(bt_gap_link_key_notification_ind_t* key_information)
{
    bt_callback_type_t type = bt_callback_type_gap_get_link_key;
    bt_gap_get_link_key_p callback = (bt_gap_get_link_key_p)(normal_callback_list[type - 1].callback);

    BT_LOGI("BT_CMGR", "%s, in use %d", __FUNCTION__, normal_callback_list[type - 1].in_use);
    if (normal_callback_list[type - 1].in_use) {
        callback(key_information);
    }
}


bt_gap_pin_code_information_t bt_callback_pin_code = {
    .pin_len = 4,
    .pin_code = {0,0,0,0}
};

const bt_gap_pin_code_information_t* bt_gap_get_pin_code(void)
{
    bt_callback_type_t type = bt_callback_type_gap_get_pin_code;
    bt_gap_get_pin_code_p callback = (bt_gap_get_pin_code_p)(normal_callback_list[type - 1].callback);

    BT_LOGI("BT_CMGR", "%s, in use %d", __FUNCTION__, normal_callback_list[type - 1].in_use);
    if (normal_callback_list[type - 1].in_use) {
        return callback();
    }
    return &bt_callback_pin_code;
}
#endif /*#if !defined(__MTK_BLE_ONLY_ENABLE__) */

bool bt_gap_le_is_connection_update_request_accepted(bt_handle_t handle, bt_gap_le_connection_update_param_t* connection_parameter)
{
    bt_callback_type_t type = bt_callback_type_gap_le_is_accept_connection_update_request;
    bt_gap_le_is_accept_connection_update_request_p callback = (bt_gap_le_is_accept_connection_update_request_p)(normal_callback_list[type - 1].callback);

    BT_LOGI("BT_CMGR", "%s, in use %d", __FUNCTION__, normal_callback_list[type - 1].in_use);
    if (normal_callback_list[type - 1].in_use) {
        return callback(handle, connection_parameter);
    } 
    return false;
}

//must have a register
bt_status_t bt_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind)
{
    bt_callback_type_t type = bt_callback_type_gap_le_get_pairing_config;
    bt_gap_le_get_pairing_config_p callback = (bt_gap_le_get_pairing_config_p)(normal_callback_list[type - 1].callback);

    BT_LOGI("BT_CMGR", "%s, in use %d", __FUNCTION__, normal_callback_list[type - 1].in_use);
    if (normal_callback_list[type - 1].in_use) {
        return callback(ind);
    }
    assert(0);
    return BT_STATUS_FAIL;
}

//must have a register
bt_gap_le_bonding_info_t *bt_gap_le_get_bonding_info(const bt_addr_t remote_addr)
{
    bt_callback_type_t type = bt_callback_type_gap_le_get_bonding_info;
    bt_gap_le_get_bonding_info_p callback = (bt_gap_le_get_bonding_info_p)(normal_callback_list[type - 1].callback);

    BT_LOGI("BT_CMGR", "%s, in use %d", __FUNCTION__, normal_callback_list[type - 1].in_use);
    if (normal_callback_list[type - 1].in_use) {
        return callback(remote_addr);
    }
    assert(0);
    return NULL;
}

bt_gap_le_local_key_t *bt_gap_le_get_local_key(void)
{
    bt_callback_type_t type = bt_callback_type_gap_le_get_local_key;
    bt_gap_le_get_local_key_p callback = (bt_gap_le_get_local_key_p)(normal_callback_list[type - 1].callback);

    BT_LOGI("BT_CMGR", "%s, in use %d", __FUNCTION__, normal_callback_list[type - 1].in_use);
    if (normal_callback_list[type - 1].in_use) {
        return callback();
    }
    return NULL;
}

//must have a register
bt_gap_le_local_config_req_ind_t *bt_gap_le_get_local_config(void)
{
    bt_callback_type_t type = bt_callback_type_gap_le_get_local_cofig;
    bt_gap_le_get_local_config_p callback = (bt_gap_le_get_local_config_p)(normal_callback_list[type - 1].callback);

    BT_LOGI("BT_CMGR", "%s, in use %d", __FUNCTION__, normal_callback_list[type - 1].in_use);
    if (normal_callback_list[type - 1].in_use) {
        return callback();
    }
    assert(0);
    return NULL;
}

bt_status_t bt_gatts_get_authorization_check_result(bt_gatts_authorization_check_req_t *req)
{
    bt_callback_type_t type = bt_callback_type_gatts_get_authorization_check_result;
    bt_gatts_get_authorization_check_result_p callback = (bt_gatts_get_authorization_check_result_p)(normal_callback_list[type - 1].callback);

    BT_LOGI("BT_CMGR", "%s, in use %d", __FUNCTION__, normal_callback_list[type - 1].in_use);
    if (normal_callback_list[type - 1].in_use) {
        return callback(req);
    }
    return BT_STATUS_FAIL;
}

bt_status_t bt_gatts_get_execute_write_result(bt_gatts_execute_write_req_t *req)
{
    bt_callback_type_t type = bt_callback_type_gatts_get_execute_write_result;
    bt_gatts_get_execute_write_result_p callback = (bt_gatts_get_execute_write_result_p)(normal_callback_list[type - 1].callback);

    BT_LOGI("BT_CMGR", "%s, in use %d", __FUNCTION__, normal_callback_list[type - 1].in_use);
    if (normal_callback_list[type - 1].in_use) {
        return callback(req);
    }
    return BT_STATUS_FAIL;
}

#if !defined(__MTK_BLE_ONLY_ENABLE__) 
bt_status_t bt_hfp_get_init_params(bt_hfp_init_param_t *init_params)
{
    bt_callback_type_t type = bt_callback_type_hfp_get_init_params;
    bt_hfp_get_init_params_p callback = (bt_hfp_get_init_params_p)(normal_callback_list[type - 1].callback);

    BT_LOGI("BT_CMGR", "%s, in use %d", __FUNCTION__, normal_callback_list[type - 1].in_use);
    if (normal_callback_list[type - 1].in_use) {
        return callback(init_params);
    }
    return BT_STATUS_FAIL;
}

bt_status_t bt_a2dp_get_init_params(bt_a2dp_init_params_t *param)
{
    bt_callback_type_t type = bt_callback_type_a2dp_get_init_params;
    bt_a2dp_get_init_params_p callback = (bt_a2dp_get_init_params_p)(normal_callback_list[type - 1].callback);

    BT_LOGI("BT_CMGR", "%s, in use %d", __FUNCTION__, normal_callback_list[type - 1].in_use);
    if (normal_callback_list[type - 1].in_use) {
        return callback(param);
    }
    return BT_STATUS_FAIL;
}


static const bt_sdps_record_t *sdps_customized_record[BT_CALLBACK_MANAGER_USER_RECORD_MAX];
static uint8_t customized_record_number = 0;

bt_status_t bt_callback_manager_add_sdp_customized_record(const bt_sdps_record_t * record)
{
    BT_LOGI("BT_CMGR", "%s, current record %d", __FUNCTION__, customized_record_number);
    if (customized_record_number < BT_CALLBACK_MANAGER_USER_RECORD_MAX) {
        uint8_t i;
        for (i = 0; i < customized_record_number; i++) {
            if (sdps_customized_record[i] == record) {
                return BT_STATUS_FAIL;
            }
        }
        sdps_customized_record[customized_record_number] = record;
        customized_record_number++;
        return BT_STATUS_SUCCESS;
    } else {
        return BT_STATUS_FAIL;
    }
  
}

uint8_t bt_sdps_get_customized_record(const bt_sdps_record_t *** record_list)
{
    *record_list = sdps_customized_record;
    return customized_record_number;
}

#endif /*#if !defined(__MTK_BLE_ONLY_ENABLE__) */

