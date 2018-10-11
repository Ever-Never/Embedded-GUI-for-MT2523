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

#include <string.h>
#include "bt_system.h"
#include "bt_source_srv.h"
#include "bt_source_srv_utils.h"
#include "bt_source_srv_a2dp.h"
#include "bt_source_srv_avrcp.h"
#include "bt_callback_manager.h"
#include "bt_source_srv_cntx.h"


typedef struct {
    bool in_use;
    bt_source_srv_notify_callback callback;
} bt_source_srv_callback_node_t;

const static char *g_system_event_string[] = {
    "BT_POWER_ON_CNF",
    "BT_POWER_OFF_CNF"
};

static bt_source_srv_callback_node_t app_cb_list[BT_SOURCE_SRV_CB_MAX_NUM] = {{0}};


static void bt_source_srv_callback_init(void);
static bt_status_t bt_source_srv_system_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);
static bt_status_t bt_source_srv_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);


bt_status_t bt_source_srv_register_callback(bt_source_srv_notify_callback callback)
{
    //bt_source_srv_take_mutex();

    uint8_t i = 0;
    bt_status_t status = BT_STATUS_SUCCESS;
    bt_source_srv_report("[source][cbmgr]register_cb callback %x\r\n", callback);

    for (i = 0; i < BT_SOURCE_SRV_CB_MAX_NUM; i++) {
        if (!app_cb_list[i].in_use) {
            app_cb_list[i].callback = callback;
            app_cb_list[i].in_use = true;
            break;
        }
    }
    if (i == BT_SOURCE_SRV_CB_MAX_NUM) {
        
        bt_source_srv_report("[source][cbmgr]all are in use, please extend the value of BT_SOURCE_SRV_CB_MAX_NUM\r\n");
        status = BT_STATUS_FAIL;
    }
    
    //bt_source_srv_give_mutex();
            
    return status;
}


bt_status_t bt_source_srv_deregister_callback(bt_source_srv_notify_callback callback)
{
    //bt_source_srv_take_mutex();

    uint8_t i = 0;
    bt_status_t status = BT_STATUS_SUCCESS;

    bt_source_srv_report("[source][cbmgr]deregister_cb callback %x\r\n", callback);

    for (i = 0; i < BT_SOURCE_SRV_CB_MAX_NUM; i++) {
        if (app_cb_list[i].in_use && app_cb_list[i].callback == callback) {
            app_cb_list[i].callback = NULL;
            app_cb_list[i].in_use = false;
            break;
        }
    }
    if (i == BT_SOURCE_SRV_CB_MAX_NUM) {
        
        bt_source_srv_report("[source][cbmgr]delete fail, because of not find the callback\r\n");
        status = BT_STATUS_FAIL;
    }  
    
    //bt_source_srv_give_mutex();
    return status;

}

bt_status_t bt_source_srv_event_callback(bt_source_srv_event_t event_id, void *param)
{
    //bt_source_srv_take_mutex();

    uint8_t i = 0;
    bt_status_t ret = BT_STATUS_FAIL;

    bt_source_srv_report("[source]event-cb  -event_id: %d\n", event_id);

    for (i = 0; i < BT_SOURCE_SRV_CB_MAX_NUM; i++) {
        if (app_cb_list[i].in_use && app_cb_list[i].callback != NULL) {
            app_cb_list[i].callback(event_id, param);
            ret = BT_STATUS_SUCCESS;
        }
    }
    
    //bt_source_srv_give_mutex();
    
    return ret;
}

static bt_status_t bt_source_srv_system_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
#if defined(__BT_SOURCE_SRV_DEBUG_INFO__ ) && defined (__BT_SOURCE_SRV_TRACE__)
    if ((msg >= BT_POWER_ON_CNF) && (msg <= BT_POWER_OFF_CNF)) {
        bt_source_srv_report("[Source] SDK msg:%s\r\n", g_system_event_string[msg - BT_POWER_ON_CNF]);
    } else {
        bt_source_srv_report("[Source] SDK msg:0x%x\r\n", msg);
    }
#else
    bt_source_srv_report("[Source] SDK msg:0x%x\r\n", msg);
#endif /* defined(__BT_SOURCE_SRV_DEBUG_INFO__ ) && defined (__BT_SOURCE_SRV_TRACE__) */
    switch (msg) {
        case BT_POWER_ON_CNF: {
            bt_source_srv_cntx_init();
        }
            break;
        case BT_POWER_OFF_CNF:
            break;
        case BT_PANIC:
           break;
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

static bt_status_t bt_source_srv_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    uint32_t moduel = msg & 0xFF000000;
    
    bt_source_srv_report("[Source]Bluetooth msg:0x%x, status:0x%x", msg, status);
    switch (moduel) {
        case BT_MODULE_SYSTEM:
            result = bt_source_srv_system_callback(msg, status, buffer);
            break;

        case BT_MODULE_GAP:
            break;

        case BT_MODULE_A2DP:
            result = bt_source_srv_a2dp_common_callback(msg, status, buffer);
            break;

        case BT_MODULE_AVRCP:
            result = bt_source_srv_avrcp_common_callback(msg, status, buffer);
            break;

        default:
            bt_source_srv_report("[Source] Unknown Bluetooth msgg:0x%x, status:0x%x", msg, status);
            break;
    }
    return result;
}

static void bt_source_srv_callback_init(void)
{
    bt_callback_manager_register_callback(bt_callback_type_app_event,
                                          (uint32_t)(MODULE_MASK_GAP | MODULE_MASK_SYSTEM | 
                                                  MODULE_MASK_AVRCP | MODULE_MASK_A2DP ),
                                          (void *)bt_source_srv_common_callback);
 
    bt_callback_manager_register_callback(bt_callback_type_a2dp_get_init_params,
                                          0,
                                          (void *)bt_source_srv_a2dp_get_init_params);
}

void bt_source_srv_init(void)
{
    bt_source_srv_callback_init();
    bt_source_srv_create_mutex();
}

