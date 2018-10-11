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

#include "bt_pbapc.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_pbapc.h"
#include "bt_sink_srv_conmgr.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_resource.h"

//bt_sink_srv_profile_t g_sink_srv_pbapc[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
bt_sink_srv_pbapc_context_t g_sink_srv_pbapc_context[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];

#ifdef __BT_SINK_SRV_DEBUG_INFO__
const static char *g_sink_pbapc_event_string[] = {
    "BT_PBAPC_CONNECT_CNF", /**< The result of a connect attempt initiated by a local device is available. */
    "BT_PBAPC_GET_NUMBER_OF_PHOENBOOK_OBJECTS_IND", /**< The result of get missed call size information from remote device.*/
    "BT_PBAPC_GET_NUMBER_OF_PHOENBOOK_OBJECTS_CNF", /**< The result of get missed call size request is done.*/
    "BT_PBAPC_GET_PHOENBOOK_OBJECTS_IND", /**< The result of get missed call information from remote device.*/
    "BT_PBAPC_GET_PHONEBOOK_OBJECTS_CNF", /**< The result of get missed call request is done.*/
    "BT_PBAPC_GET_CALLER_NAME_IND", /**< The result of get caller name information from remote device.*/
    "BT_PBAPC_GET_CALLER_NAME_CNF", /**< The result of get caller name request is done.*/
    "BT_PBAPC_DISCONNECT_IND" /**< A disconnect attempt is initiated by a local or remote device. */
};
#endif /* __BT_SINK_SRV_DEBUG_INFO__ */

void* bt_sink_srv_cm_get_pbapc_info(uint8_t device_idx)
{
    return (void*)&g_sink_srv_pbapc_context[device_idx];
}

static bt_sink_srv_pbapc_context_t* bt_sink_srv_pbapc_get_context_by_handle(uint32_t handle)
{
    bt_sink_srv_pbapc_context_t *context = NULL;
    for (uint8_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        if (handle == g_sink_srv_pbapc_context[i].pbapc_handle) {
            context =  &g_sink_srv_pbapc_context[i];
            break;
        } 
    } 
    return context;
}

static void bt_sink_srv_pbapc_add_number(bt_bd_addr_t *address_p, bt_sink_srv_pbapc_record_t *record)
{
    bt_sink_srv_pbapc_record_t *record_p = NULL;
    bt_sink_srv_pbapc_record_t *last_p = NULL;
    bt_sink_srv_pbapc_record_t *empty_p = NULL;
    bt_sink_srv_pbapc_stored_data_t *stored_data_p =
        (bt_sink_srv_pbapc_stored_data_t *)bt_sink_srv_memory_alloc(sizeof(*stored_data_p));

    if (NULL != record && bt_sink_srv_strlen((char *)record->number) < BT_SINK_SRV_MAX_PHONE_NUMBER
            && bt_sink_srv_cm_get_profile_data(address_p, BT_SINK_SRV_PROFILE_PBAPC, (void *)stored_data_p)) {

        for (uint32_t i = 0; i < BT_SINK_SRV_PBAPC_STORED_COUNT; i++) {
            if (0 == bt_sink_srv_strnmp((const char *)(((*stored_data_p)[i]).number),
                                        (const char *)(record->number),
                                        BT_SINK_SRV_MAX_PHONE_NUMBER)) {
                record_p = &((*stored_data_p)[i]);
                break;
            } else if (0 == bt_sink_srv_strlen((char *)((*stored_data_p)[i]).number)) {
                if (NULL == empty_p) {
                    empty_p = &((*stored_data_p)[i]);
                }
            } else {
                last_p = &((*stored_data_p)[i]);
            }
        }

        if (NULL == record_p) {
            if (NULL != empty_p) {
                record_p = empty_p;
            } else  {
                record_p = last_p;
                if (BT_SINK_SRV_PBAPC_STORED_COUNT > 1) {
                    bt_sink_srv_memcpy((void *) & ((*stored_data_p)[0]),
                                       (void *) & ((*stored_data_p)[1]),
                                       (BT_SINK_SRV_PBAPC_STORED_COUNT - 1) * sizeof(bt_sink_srv_pbapc_record_t));
                }
            }
        }
        bt_sink_srv_memcpy((void *)record_p, (const void *)record, sizeof(bt_sink_srv_pbapc_record_t));
        bt_sink_srv_cm_set_profile_data(address_p, BT_SINK_SRV_PROFILE_PBAPC, (void *)stored_data_p);
    }
    bt_sink_srv_memory_free(stored_data_p);
    bt_sink_srv_report("[Sink][PBAPC] record_p:0x%x", record_p);
}

static bool bt_sink_srv_pbapc_search_number(bt_bd_addr_t *address, bt_sink_srv_pbapc_record_t *record)
{
    bool result = false;

    if (NULL != record && bt_sink_srv_strlen((char *)record->number) < BT_SINK_SRV_MAX_PHONE_NUMBER) {
        uint32_t i;
        bt_sink_srv_pbapc_stored_data_t *stored_data_p =
            (bt_sink_srv_pbapc_stored_data_t *)bt_sink_srv_memory_alloc(sizeof(*stored_data_p));

        if (bt_sink_srv_cm_get_profile_data(address, BT_SINK_SRV_PROFILE_PBAPC, (void *)stored_data_p)) {
            for (i = 0; i < BT_SINK_SRV_PBAPC_STORED_COUNT; i++) {
                if (0 == bt_sink_srv_strnmp((const char *)(((*stored_data_p)[i]).number),
                                            (const char *)(record->number),
                                            BT_SINK_SRV_MAX_PHONE_NUMBER)) {
                    result = true;
                    bt_sink_srv_memcpy((void *)record->name,
                                       (void *)(((*stored_data_p)[i]).name),
                                       sizeof(record->name));
                    break;
                }
            }
        }
        bt_sink_srv_memory_free((void *)stored_data_p);
    }
    bt_sink_srv_report("[Sink][PBAPC] result:%d", result);
    return result;
}

bt_status_t bt_sink_srv_pbapc_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_sink_srv_pbapc_context_t *pbapc_context_p = NULL;
    bt_bd_addr_t *address_p = NULL;

#ifdef __BT_SINK_SRV_DEBUG_INFO__
    if (msg >= BT_PBAPC_CONNECT_CNF && msg <= BT_PBAPC_DISCONNECT_IND) {
        bt_sink_srv_report("[Sink][PBAPC] SDK msg:%s", g_sink_pbapc_event_string[msg - BT_PBAPC_CONNECT_CNF]);
    } else {
        bt_sink_srv_report("[Sink][PBAPC] SDK msg:0x%x", msg);
    }
#else
    bt_sink_srv_report("[Sink][PBAPC] SDK msg:0x%x", msg);
#endif /* __BT_SINK_SRV_DEBUG_INFO__ */

    switch (msg) {
        case BT_PBAPC_CONNECT_CNF: {
            bt_pbapc_connect_cnf_t *message = (bt_pbapc_connect_cnf_t *)buffer;

            pbapc_context_p = bt_sink_srv_pbapc_get_context_by_handle(message->handle);

            if (NULL != pbapc_context_p) {
                if (BT_STATUS_SUCCESS == status) {
                    pbapc_context_p->state = BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED;
                } else {
                    pbapc_context_p->state = BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED;
                }
            }
        }
        break;

        case BT_PBAPC_DISCONNECT_IND: {
            bt_pbapc_disconnect_ind_t *message = (bt_pbapc_disconnect_ind_t *)buffer;

            pbapc_context_p = bt_sink_srv_pbapc_get_context_by_handle(message->handle);
            if (NULL != pbapc_context_p) {
                pbapc_context_p->state = BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED;
                pbapc_context_p->pbapc_handle = 0;
                if (NULL != pbapc_context_p && NULL != pbapc_context_p->pending) {
                    if (NULL != pbapc_context_p->callback) {
                        address_p = bt_sink_srv_cm_get_address_by_profile_info((void*)pbapc_context_p);
                        pbapc_context_p->callback(address_p, pbapc_context_p->pending, pbapc_context_p->user_data);
                        pbapc_context_p->callback = NULL;
                    }

                    bt_sink_srv_memory_free(pbapc_context_p->pending);
                    pbapc_context_p->pending = NULL;
                }
            }
        }
        break;

        case BT_PBAPC_GET_CALLER_NAME_IND: {
            bt_pbapc_get_caller_name_ind_t *message = (bt_pbapc_get_caller_name_ind_t *)buffer;
            pbapc_context_p = bt_sink_srv_pbapc_get_context_by_handle(message->handle);

            if (NULL != pbapc_context_p && NULL != pbapc_context_p->pending) {
                    bt_sink_srv_memcpy((void *)pbapc_context_p->pending->name, (const void *)message->caller->name, BT_SINK_SRV_MAX_NAME);
                    address_p = bt_sink_srv_cm_get_address_by_profile_info((void*)pbapc_context_p);
                    if(address_p) {
                        bt_sink_srv_pbapc_add_number(address_p, pbapc_context_p->pending);
                    }
                    if (NULL != pbapc_context_p->callback) {
                        pbapc_context_p->callback(address_p, pbapc_context_p->pending, pbapc_context_p->user_data);
                        pbapc_context_p->callback = NULL;
                    }
                    bt_sink_srv_memory_free(pbapc_context_p->pending);
                    pbapc_context_p->pending = NULL;
                    bt_sink_srv_report("[Sink][PBAPC] length:%d, name:%s", message->caller->length, message->caller->name);
            }
        }
        break;

        case BT_PBAPC_GET_CALLER_NAME_CNF: {
            bt_pbapc_get_caller_name_cnf_t *message = (bt_pbapc_get_caller_name_cnf_t *)buffer;
            pbapc_context_p = bt_sink_srv_pbapc_get_context_by_handle(message->handle);

            if (NULL != pbapc_context_p  && NULL != pbapc_context_p->pending) {
                    if (NULL != pbapc_context_p->callback) {
                        address_p = bt_sink_srv_cm_get_address_by_profile_info((void*)pbapc_context_p);
                        if(address_p) {
                            pbapc_context_p->callback(address_p, pbapc_context_p->pending, pbapc_context_p->user_data);
                        }
                        pbapc_context_p->callback = NULL;
                    }
                bt_sink_srv_memory_free(pbapc_context_p->pending);
                pbapc_context_p->pending = NULL;
            }
        }
        break;

        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

bt_sink_srv_status_t bt_sink_srv_pbapc_action_handler(bt_sink_srv_action_t action, void *parameters)
{

    bt_sink_srv_report("[Sink][PBAPC] bt_sink_srv_pbapc_action_handler, action:0x%x", action);

    switch (action) {
        case BT_SINK_SRV_ACTION_PROFILE_INIT: {
            bt_sink_srv_memset(g_sink_srv_pbapc_context, 0, sizeof(g_sink_srv_pbapc_context));         
        }
        break;

        case BT_SINK_SRV_ACTION_PROFILE_DISCONNECT: {
            bt_sink_srv_profile_connection_action_t *action_param = (bt_sink_srv_profile_connection_action_t *)parameters;
            if (action_param->profile_connection_mask & BT_SINK_SRV_CM_PROFILE_SP) {
                bt_sink_srv_pbapc_context_t *bt_sink_srv_pbapc_p = (bt_sink_srv_pbapc_context_t *)bt_sink_srv_cm_get_profile_info(&action_param->address, BT_SINK_SRV_PROFILE_PBAPC);
                if (NULL != bt_sink_srv_pbapc_p && bt_sink_srv_pbapc_p->pbapc_handle != 0) {
                    bt_sink_srv_report("[Sink][PBAPC] call pbapc profile disconnect");
                    bt_pbapc_disconnect(bt_sink_srv_pbapc_p->pbapc_handle);
                }
            }
        }
        break;

        default:
            break;
    }
    return BT_SINK_SRV_STATUS_SUCCESS;
}

bt_status_t bt_sink_srv_pbapc_get_name_from_local(bt_bd_addr_t *address, bt_sink_srv_pbapc_record_t *record)
{
    bt_status_t result = BT_STATUS_FAIL;

    if (NULL != record && NULL != address && 0 != bt_sink_srv_strlen((char *)record->number)) {
        if (bt_sink_srv_pbapc_search_number(address, record)) {
            result = BT_STATUS_SUCCESS;
        }
    }
    return result;
}

bt_status_t bt_sink_srv_pbapc_get_name_from_remote(bt_bd_addr_t *address,
        uint8_t *number,
        void *user_data,
        bt_sink_srv_pbapc_get_name_callback_t callback)
{
    bt_status_t result = BT_STATUS_FAIL;
    bt_sink_srv_pbapc_context_t *bt_sink_srv_pbapc_p = (bt_sink_srv_pbapc_context_t *)bt_sink_srv_cm_get_profile_info(address, BT_SINK_SRV_PROFILE_PBAPC);

    if (NULL != bt_sink_srv_pbapc_p &&
            BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED == bt_sink_srv_pbapc_p->state) {
        if (NULL == bt_sink_srv_pbapc_p->pending) {
            bt_sink_srv_pbapc_p->pending = (bt_sink_srv_pbapc_record_t *)bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_pbapc_record_t));
            if (NULL != bt_sink_srv_pbapc_p->pending) {
                bt_sink_srv_memcpy((void *)bt_sink_srv_pbapc_p->pending->number,
                                   (void *)number,
                                   BT_SINK_SRV_MAX_PHONE_NUMBER);
                bt_sink_srv_pbapc_p->user_data = user_data;
                bt_sink_srv_pbapc_p->callback = callback;
            }
            result = bt_pbapc_get_caller_name_by_number(bt_sink_srv_pbapc_p->pbapc_handle, number);
        }
    }
    bt_sink_srv_report("[Sink][PBAPC] get name status:0x%x", result);
    return result;
}

bt_status_t bt_sink_srv_pbapc_connect(bt_bd_addr_t *address)
{
    bt_status_t status = BT_STATUS_SUCCESS;

    bt_sink_srv_pbapc_context_t *bt_sink_srv_pbapc_p = (bt_sink_srv_pbapc_context_t *)bt_sink_srv_cm_get_profile_info(address, BT_SINK_SRV_PROFILE_PBAPC);

    if (NULL != bt_sink_srv_pbapc_p && bt_sink_srv_pbapc_p->pbapc_handle == 0) {
         bt_status_t status = bt_pbapc_connect(&bt_sink_srv_pbapc_p->pbapc_handle, (const bt_bd_addr_t *)address);
        if ( BT_STATUS_SUCCESS == status) {
            bt_sink_srv_pbapc_p->state = BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTING;
        }
        bt_sink_srv_report("[Sink][PBAPC] connect status:0x%x", status);
    }
    return status;
}

