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
#include "bt_gap_le.h"
#include "bt_sink_srv_action.h"
#include "bt_sink_srv_db.h"
#include "bt_sink_srv_custom_db.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_sink_srv_common.h"
#include "bt_device_manager_config.h"
#include "bt_device_manager.h"
#include "hal_wdt.h"
#include "bt_sink_srv_spp.h"
#include "bt_sink_srv_music.h"
#ifdef __BT_AWS_SUPPORT__
#include "bt_sink_srv_aws_system.h"
#endif
#ifdef __BT_SINK_SRV_VOICE_PROMPT__
#include "bt_sink_srv_vp_pesudo_dev.h"
#endif


#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_sink_srv_cm_get_hfp_info=_default_bt_sink_srv_cm_get_hfp_info")
#pragma comment(linker, "/alternatename:_bt_sink_srv_cm_get_a2dp_sink_info=_default_bt_sink_srv_cm_get_a2dp_sink_info")
#pragma comment(linker, "/alternatename:_bt_sink_srv_cm_get_avrcp_info=_default_bt_sink_srv_cm_get_avrcp_info")
#pragma comment(linker, "/alternatename:_bt_sink_srv_cm_get_pbapc_info=_default_bt_sink_srv_cm_get_pbapc_info")
#pragma comment(linker, "/alternatename:_bt_sink_srv_cm_get_aws_info=_default_bt_sink_srv_cm_get_aws_info")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_sink_srv_cm_get_hf_info = default_bt_sink_srv_cm_get_hf_info
#pragma weak bt_sink_srv_cm_get_a2dp_sink_info = default_bt_sink_srv_cm_get_a2dp_sink_info
#pragma weak bt_sink_srv_cm_get_avrcp_info = default_bt_sink_srv_cm_get_avrcp_info
#pragma weak bt_sink_srv_cm_get_pbapc_info = default_bt_sink_srv_cm_get_pbapc_info
#pragma weak bt_sink_srv_cm_get_aws_info = default_bt_sink_srv_cm_get_aws_info
#else
#error "Unsupported Platform"
#endif

void* default_bt_sink_srv_cm_get_hf_info(uint8_t device_idx)
{
    return NULL;
}
void* default_bt_sink_srv_cm_get_a2dp_sink_info(uint8_t device_idx)
{
    return NULL;
}
void* default_bt_sink_srv_cm_get_avrcp_info(uint8_t device_idx)
{
    return NULL;
}
void* default_bt_sink_srv_cm_get_pbapc_info(uint8_t device_idx)
{
    return NULL;
}
void* default_bt_sink_srv_cm_get_aws_info(uint8_t device_idx)
{
    return NULL;
}
static bt_sink_srv_cm_remote_device_t* bt_sink_srv_cm_get_aws_connected_device(void);
static void bt_sink_srv_cm_open_visibility_feature(void);
static uint8_t bt_sink_srv_db_common_comparator(const uint8_t *record, const void *key);
static uint8_t open_visibility;
bt_sink_srv_cm_context_t g_sink_srv_cm_context;
static bt_bd_addr_t g_sink_srv_cm_last_connected;
static bt_sink_srv_cm_dev_info_t g_sink_srv_cm_dev_info[BT_SINK_SRV_CM_MAX_TRUSTED_DEV];
static bt_sink_srv_db_t g_bt_sink_srv_cnmgr_db = {
    .name_p = BT_SINK_SRV_DB_DEVICE_INFO,
    .dirty = false,
    .size = sizeof(bt_sink_srv_cm_dev_info_t),
    .count = BT_SINK_SRV_CM_MAX_TRUSTED_DEV,
    .used = 0,
    .comparator_function_p = bt_sink_srv_db_common_comparator,
    .buffer_p = (uint8_t *)g_sink_srv_cm_dev_info
};

#define BT_SINK_SRV_CM_STATE_CHANGE(device, new_state)                                    \
{                                                                                     \
    bt_sink_srv_report("[Sink][CM] State Change, previous_state:%d, new_state:%d", device->link_state, new_state); \
    device->link_state = new_state;                                    \
}

#if defined(__BT_SINK_SRV_DEBUG_INFO__ ) && defined (__BT_SINK_SRV_TRACE__)
const static char *g_gap_event_string[] = {
    "BT_GAP_LINK_STATUS_UPDATED_IND", /**< This event indicates the the connection status changed, with #bt_gap_link_status_updated_ind_t payload. */
   // "BT_GAP_LINK_KEY_REQ_IND", /**< This event asks user for link key, with #bt_gap_link_key_notification_ind_t payload, user need fill the parameters with key information if the link key is found. */
    "BT_GAP_IO_CAPABILITY_REQ_IND", /**< This event asks user for io capability response, with #bt_gap_connection_handle_t payload, user need invoke #bt_gap_reply_io_capability_request() API to accept and reply the pairing request.
                                                                                        or invoke #bt_gap_reject_io_capability_request() API to reject the pairing request. */
    "BT_GAP_USER_CONFIRM_REQ_IND", /**< This event asks user for value confirm, with uint32_t payload, user need invoke #bt_gap_reply_user_confirm_request() API to replay it according to value of event parameters. */
    "BT_GAP_USER_PASSKEY_NOTIFICATION_IND", /**< This event notify the passkey value to the user, with uint32_t payload, user need display the passkey on the screen. */
    "BT_GAP_USER_PASSKEY_NOTIFICATION_END_IND", /**< This event notify the passkey enterd to the user, with NULL payload. user need cancel the passkey showing on the screen. */
    "BT_GAP_LINK_KEY_NOTIFICATION_IND", /**< This event notify user to update link key information, with #bt_gap_link_key_notification_ind_t payload. */
    "BT_GAP_USER_REPLY_PASSKEY_IND", /**< This event asks user for passkey, with NULL payload, user need invoke #bt_gap_reply_passkey_request() API to replay it according to value of event parameters. */
    "BT_GAP_SET_SCAN_MODE_CNF", /**< This event indicates the confirmation of setting scan mode with NULL payload. */
    "BT_GAP_INQUIRY_CNF", /**< This event indicates the confirmation of inquiry with NULL payload. */
    "BT_GAP_CANCEL_INQUIRY_CNF", /**< This event indicates the confirmation of cancel inquiry with NULL payload. */
    "BT_GAP_INQUIRY_IND", /**< This event indicates the result of inquiry with #bt_gap_inquiry_ind_t payload. */
    "BT_GAP_INQUIRY_COMPLETE_IND", /**< This event indicates the completement of inquiry with NULL payload. */
    "BT_GAP_SET_ROLE_CNF", /**< This event indicates the confirmation of #bt_gap_set_role() with NULL payload. */
    "BT_GAP_ROLE_CHANGED_IND", /**< This event indicates the connection role changed with #bt_gap_role_changed_ind_t payload. */
    "BT_GAP_READ_REMOTE_NAME_CNF", /**< This event indicates the confirmation of #bt_gap_read_remote_name_role() with NULL payload. */
    "BT_GAP_READ_REMOTE_NAME_COMPLETE_IND", /**< This event indicates the result of #bt_gap_read_remote_name_role() with #bt_gap_read_remote_name_complete_ind_t payload. */
    "BT_GAP_READ_CANCEL_REMOTE_NAME_CNF", /**< This event indicates the confirmation of #bt_gap_cancel_name_request() with NULL payload. */
    "BT_GAP_ENTER_TEST_MODE_CNF", /**< This event indicates the confirmation of #bt_gap_enter_test_mode() with NULL payload. */
    "BT_GAP_WRITE_INQUIRY_TX_PWR_CNF", /**< This event indicates the confirmation of #bt_gap_write_inquiry_tx() with NULL payload. */
    "BT_GAP_READ_INQUIRY_RESPONSE_TX_CNF", /**< This event indicates the confirmation of #bt_gap_read_inquiry_response_tx() with uint8_t payload. */
    "BT_GAP_READ_RSSI_CNF", /**< This event indicates the confirmation of #bt_gap_read_rssi() with #bt_gap_read_rssi_cnf_t payload. */
    "BT_GAP_GET_ROLE_CNF", /**< This event indicates the confirmation of #bt_gap_get_role() with bt_gap_get_role_cnf_t payload. */
    "BT_GAP_SET_EIR_CNF", /**< This event indicates the confirmation of #bt_gap_set_extended_inquiry_response() with NULL payload. */
    "BT_GAP_BONDING_START_IND", /**< This event indicates the start of bonding with NULL payload. */
    "BT_GAP_BONDING_COMPLETE_IND", /**< This event indicates the result of bonding with NULL payload. */
    "BT_GAP_WRITING_PAGE_SCAN_CNF", /**< This event indicates the confirmation of #bt_gap_write_page_scan_activity() with NULL payload. */
    "BT_GAP_WRITING_INQUIRY_SCAN_CNF"  /**< This event indicates the confirmation of #bt_gap_write_inquiry_scan_activity() with NULL payload. */
};

const static char *g_system_event_string[] = {
    "BT_POWER_ON_CNF",
    "BT_POWER_OFF_CNF",
    "BT_PANIC"
};
#endif /* defined(__BT_SINK_SRV_DEBUG_INFO__ ) && defined (__BT_SINK_SRV_TRACE__) */

static void bt_sink_srv_cm_write_scan_enable_mode(bt_gap_scan_mode_t mode);
static void bt_sink_srv_cm_handle_power_on_cnf(void);
static void bt_sink_srv_cm_connect_internal(bt_sink_srv_profile_connection_action_t *action_param);
static void bt_sink_srv_cm_request_done(bt_sink_srv_status_t result, bt_sink_srv_cm_node_t *req);
static bt_sink_srv_cm_remote_device_t *bt_sink_srv_cm_find_device(bt_sink_srv_cm_find_by_type_t type, void *param);
static void bt_sink_srv_cm_link_connected(const bt_bd_addr_t *dev_addr, bt_gap_connection_handle_t handle);
static void bt_sink_srv_cm_link_disconnected(const bt_bd_addr_t *dev_addr, bt_gap_connection_handle_t handle);
static void bt_sink_srv_cm_connect(bt_sink_srv_profile_connection_action_t *conn_action);
static void bt_sink_srv_cm_disconnect(void);
static void bt_sink_srv_cm_init(void);
static void bt_sink_srv_cm_deinit(void);

bool bt_sink_srv_cm_is_valid_address(bt_bd_addr_t *address)
{
    static const bt_bd_addr_t empty_address = {0};
    return ((address != NULL) && (bt_sink_srv_memcmp(address, empty_address, 6) != 0));
}

static void bt_sink_srv_cm_node_insert_head(bt_sink_srv_cm_node_t *head, bt_sink_srv_cm_node_t *node)
{
    bt_sink_srv_assert(head && node);
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

static void bt_sink_srv_cm_node_insert_tail(bt_sink_srv_cm_node_t *head, bt_sink_srv_cm_node_t *node)
{
    bt_sink_srv_assert(head && node);
    node->next = head;
    node->prev = head->prev;
    head->prev->next = node;
    head->prev = node;
}

static bt_sink_srv_cm_node_t *bt_sink_srv_cm_node_new(bt_sink_srv_cm_request_t *request)
{
    bt_sink_srv_cm_node_t *new_req;

    bt_sink_srv_assert(request);

    new_req = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_cm_node_t));
    bt_sink_srv_memset(new_req, 0, sizeof(bt_sink_srv_cm_node_t));
    new_req->parameters = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_cm_request_t));
    bt_sink_srv_memcpy(new_req->parameters, request, sizeof(bt_sink_srv_cm_request_t));

    return new_req;
}

static bt_sink_srv_cm_node_t *bt_sink_srv_cm_node_get_next(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_cm_node_t *cur_req = g_sink_srv_cm_context.request_list.next;
    bt_sink_srv_cm_node_t *result = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (cur_req != &g_sink_srv_cm_context.request_list) {
        result = cur_req;
        cur_req->next->prev = cur_req->prev;
        cur_req->prev->next = cur_req->next;
        result->next = result;
        result->prev = result;
    }
    return result;
}

static void bt_sink_srv_cm_node_delete(bt_sink_srv_cm_node_t *del_req)
{
    if (del_req) {
        if (del_req->parameters) {
            bt_sink_srv_memory_free(del_req->parameters);
        }
        bt_sink_srv_memory_free(del_req);
    }
}

static void bt_sink_srv_cm_node_remove(bt_sink_srv_cm_node_t *node)
{
    bt_sink_srv_assert(node);
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

static void bt_sink_srv_cm_request_execute(bt_sink_srv_cm_node_t *req)
{
    switch (req->parameters->type) {
        case BT_SINK_SRV_CM_REQ_TYPE_NORMAL:
        case BT_SINK_SRV_CM_REQ_TYPE_POWER_ON:
        case BT_SINK_SRV_CM_REQ_TYPE_LINK_LOST: {
            bt_sink_srv_cm_connect_internal(&req->parameters->action_param);
        }
        break;

        default:
            break;
    }
}

static void bt_sink_srv_cm_request_delay_execute(void *params)
{
    bt_sink_srv_cm_node_t *req = (bt_sink_srv_cm_node_t *)params;

    if (req) {
        bt_sink_srv_cm_request_execute(req);
    }
}

static bt_sink_srv_status_t bt_sink_srv_cm_request_permit(bt_sink_srv_cm_request_t *request)
{
    bt_sink_srv_status_t result = BT_SINK_SRV_STATUS_INVALID_PARAM;

    bt_sink_srv_assert(request);

    switch (request->type) {
        case BT_SINK_SRV_CM_REQ_TYPE_NORMAL:
        case BT_SINK_SRV_CM_REQ_TYPE_POWER_ON:
        case BT_SINK_SRV_CM_REQ_TYPE_LINK_LOST:
            if (!bt_sink_srv_cm_is_valid_address(&request->action_param.address)) {
                result = BT_SINK_SRV_STATUS_FAIL;
            } else if (BT_SINK_SRV_CM_MAX_DEVICE_NUMBER == g_sink_srv_cm_context.connected_dev_num) {
                result = BT_SINK_SRV_STATUS_MAX_LINK;
            } else if (BT_SINK_SRV_CM_REQ_TYPE_NORMAL != request->type &&
                       NULL != bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, &request->action_param.address)) {
                result = BT_SINK_SRV_STATUS_LINK_EXIST;
            } else if (!(bt_sink_srv_cm_get_flags() & BT_SINK_SRV_FLAG_POWER_ON)) {
                result = BT_SINK_SRV_STATUS_INVALID_STATUS;
            } else {
                result = BT_SINK_SRV_STATUS_SUCCESS;
            }
            break;

        default:
            break;
    }
    bt_sink_srv_report("[Sink][CM] permit result:%d", result);
    return result;
}

static bt_sink_srv_status_t bt_sink_srv_cm_request_run(void)
{
    bt_sink_srv_status_t result;

    if (g_sink_srv_cm_context.request_pending != NULL) {
        result = BT_SINK_SRV_STATUS_PENDING;
    } else {
        bt_sink_srv_cm_node_t *req = bt_sink_srv_cm_node_get_next();

        if (req && req->parameters) {
            result = bt_sink_srv_cm_request_permit(req->parameters);

            if (req->parameters->attampts > 0) {
                req->parameters->attampts--;
            }

            if (BT_SINK_SRV_STATUS_SUCCESS == result) {
                g_sink_srv_cm_context.request_pending = req;

                if (req->parameters->delay_time > 0) {
                    bt_sink_srv_report("[Sink][CM] request delay:%d, type:%d", req->parameters->delay_time, req->parameters->type);
                    bt_sink_srv_timer_start(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY,
                                            req->parameters->delay_time,
                                            bt_sink_srv_cm_request_delay_execute,
                                            (void *)req);
                } else {
                    bt_sink_srv_cm_request_execute(req);
                }
            } else {
                bt_sink_srv_cm_request_done(result, req);
            }
        } else {
            result = BT_SINK_SRV_STATUS_NO_REQUEST;
        }
    }
    return result;
}

void bt_sink_srv_cm_request_done(bt_sink_srv_status_t result, bt_sink_srv_cm_node_t *req)
{
    if (req && req->parameters) {
        bt_sink_srv_report("[Sink][CM] request done, result:%d, pending:0x%x, attampts:%d",
                           result, g_sink_srv_cm_context.request_pending, req->parameters->attampts);

        g_sink_srv_cm_context.request_pending = NULL;

        if (BT_SINK_SRV_STATUS_SUCCESS != result && req->parameters->attampts > 0) {
            req->parameters->delay_time += BT_SINK_SRV_REQUEST_DELAY_TIME_INCREASE;
            bt_sink_srv_cm_node_insert_tail(&g_sink_srv_cm_context.request_list, req);
            bt_sink_srv_cm_request_run();
        } else if (BT_SINK_SRV_STATUS_NEED_RETRY == result) {
            req->parameters->delay_time = BT_SINK_SRV_REQUEST_DELAY_TIME;
            bt_sink_srv_cm_node_insert_head(&g_sink_srv_cm_context.request_list, req);
            bt_sink_srv_cm_request_run();
        } else {
            if (req->parameters->callback) {
                req->parameters->callback(result, req->parameters);
            }
            bt_sink_srv_cm_node_delete(req);
            bt_sink_srv_status_t ret = bt_sink_srv_cm_request_run();
            if (BT_SINK_SRV_STATUS_NO_REQUEST == ret && BT_SINK_SRV_STATUS_SUCCESS != result) {
                bt_sink_srv_cm_open_visibility_feature();
            }
        }
    }
}

static void bt_sink_srv_cm_open_visibility_feature(void)
{
    if (open_visibility == 1) {
        /* check visibility feature*/
        const bt_sink_srv_feature_visibility_params_t * visibility_params = bt_sink_srv_get_visibility_params();
        open_visibility = 0;
        bt_sink_srv_report("[Sink][CM] bt_sink_srv_cm_open_visibility_feature() visiable_flag %d", visibility_params->power_on_be_visible_once);
        if (visibility_params->power_on_be_visible_once) {
            bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);
        } else {
            bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
        }
    }
}

bt_sink_srv_cm_node_t *bt_sink_srv_cm_request_find(bt_sink_srv_cm_request_t *request)
{
    bt_sink_srv_cm_node_t *find_node = NULL;

    if (request) {
        bt_sink_srv_cm_node_t *cur_node = g_sink_srv_cm_context.request_list.next;

        while (cur_node != &g_sink_srv_cm_context.request_list) {
            if (cur_node->parameters &&
                    0 == bt_sink_srv_memcmp(&cur_node->parameters->action_param.address, &request->action_param.address, sizeof(bt_bd_addr_t))) {
                find_node = cur_node;
                break;
            }
            cur_node = cur_node->next;
        }
    }

    bt_sink_srv_report("[Sink][CM] request find:0x%x", find_node);

    return find_node;
}

bt_sink_srv_status_t bt_sink_srv_cm_request_new(bt_sink_srv_cm_request_t *new_req)
{
    bt_sink_srv_status_t result = bt_sink_srv_cm_request_permit(new_req);

    if (BT_SINK_SRV_STATUS_SUCCESS == result && NULL == bt_sink_srv_cm_request_find(new_req)) {
        bt_sink_srv_cm_node_t *new_node = bt_sink_srv_cm_node_new(new_req);

        if (NULL != new_node) {
            bt_sink_srv_cm_node_insert_tail(&g_sink_srv_cm_context.request_list, new_node);
        }

        result = bt_sink_srv_cm_request_run();

        bt_sink_srv_report("[Sink][CM] request new, result:%d, pending:0x%x", result, g_sink_srv_cm_context.request_pending);

        if (BT_SINK_SRV_STATUS_PENDING == result) {
            new_node->parameters->delay_time = BT_SINK_SRV_REQUEST_DELAY_TIME;
            result = BT_SINK_SRV_STATUS_SUCCESS;
        }
    } else {
        result = BT_SINK_SRV_STATUS_REQUEST_EXIST;
    }
    return result;
}

void bt_sink_srv_cm_request_cancel(bt_sink_srv_cm_req_type_t type)
{
    bt_sink_srv_cm_node_t *cur_node = g_sink_srv_cm_context.request_list.next;
    bt_sink_srv_cm_node_t *del_node = NULL;

    while (cur_node != &g_sink_srv_cm_context.request_list) {
        if (cur_node->parameters &&
                (type == cur_node->parameters->type || type == BT_SINK_SRV_CM_REQ_TYPE_NONE)) {
            del_node = cur_node;
        }

        cur_node = cur_node->next;

        if (del_node != NULL) {
            bt_sink_srv_cm_node_remove(del_node);
            bt_sink_srv_cm_node_delete(del_node);
            del_node = NULL;
        }
    }

    if (bt_sink_srv_timer_is_exist(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY)) {
        bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY);
        if (NULL != g_sink_srv_cm_context.request_pending) {
            bt_sink_srv_cm_request_done(BT_SINK_SRV_STATUS_USER_CANCEL,
                                        g_sink_srv_cm_context.request_pending);
        }
    }
}

uint8_t bt_sink_srv_db_common_comparator(const uint8_t *record, const void *key)
{
    bt_sink_srv_cm_common_record *btdbRecord = (bt_sink_srv_cm_common_record *)record;
    return (!bt_sink_srv_memcmp(&btdbRecord->bt_addr, key, sizeof(bt_bd_addr_t)));
}

static bt_sink_srv_cm_remote_device_t *bt_sink_srv_cm_alloc_device(bt_bd_addr_t *addr)
{
    if (!bt_sink_srv_cm_is_valid_address(addr)) {
        bt_sink_srv_report("[Sink][CM] Invalid address. ");
        return NULL;
    }

    bt_sink_srv_cm_remote_device_t *device = NULL;
    for (uint8_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        if (0 == bt_sink_srv_memcmp((void *)addr, (void *)&g_sink_srv_cm_context.devices[i].addr, sizeof(bt_bd_addr_t))) {
            device = &g_sink_srv_cm_context.devices[i];
            break;
        } else if (!bt_sink_srv_cm_is_valid_address(&g_sink_srv_cm_context.devices[i].addr)) {
            device = &g_sink_srv_cm_context.devices[i];
        }
    }

    if (NULL != device) {
        bt_sink_srv_memcpy((void *)&device->addr, (void *) addr, sizeof(bt_bd_addr_t));
    }
    return device;
}

static void bt_sink_srv_cm_free_device(bt_sink_srv_cm_remote_device_t *device_p)
{
    if (NULL != device_p) {
        bt_sink_srv_memset((void *)&device_p->addr, 0, sizeof(bt_bd_addr_t));
        device_p->handle = 0;
        device_p->link_state = BT_SINK_SRV_CM_STATE_DISCONNECTED;
    }
}

static bt_sink_srv_cm_remote_device_t* bt_sink_srv_cm_get_aws_connected_device(void)
{
    bt_sink_srv_cm_remote_device_t* aws_dev_p = NULL;
    for (uint8_t i= 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
        if (BT_SINK_SRV_CM_STATE_CONNECTED == g_sink_srv_cm_context.devices[i].link_state) {
                if (g_sink_srv_cm_context.devices[i].profile_connection_mask == BT_SINK_SRV_PROFILE_AWS) {
                    aws_dev_p = &(g_sink_srv_cm_context.devices[i]);
                    break;
            }
        }
    }
    bt_sink_srv_report("[Sink][CM] bt_sink_srv_cm_get_aws_connected_device return %x", aws_dev_p);
    return aws_dev_p;
}

bt_sink_srv_cm_remote_device_t *bt_sink_srv_cm_find_device(bt_sink_srv_cm_find_by_type_t type, void *param)
{
    bt_sink_srv_cm_remote_device_t *dev_p = NULL;
    for (uint8_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        if (BT_SINK_SRV_CM_FIND_BY_HANDLE == type && g_sink_srv_cm_context.devices[i].handle == *((bt_gap_connection_handle_t *)param)) {
            dev_p = &g_sink_srv_cm_context.devices[i];
            bt_sink_srv_report("[Sink][CM]handle:0x%x", dev_p->handle);
            break;
        } else if (BT_SINK_SRV_CM_FIND_BY_ADDR == type &&
                   bt_sink_srv_memcmp(param, &g_sink_srv_cm_context.devices[i].addr, sizeof(bt_bd_addr_t)) == 0) {
            dev_p = &g_sink_srv_cm_context.devices[i];
            break;
        }
    }
    bt_sink_srv_report("[Sink][CM]dev_p:%x", dev_p);
    return dev_p;
}

void bt_sink_srv_cm_connect_internal(bt_sink_srv_profile_connection_action_t *action_param)
{
    bt_sink_srv_assert(action_param && "Null param");
    bt_sink_srv_report("[Sink][CM] address:%02x:%02x:%02x:%02x:%02x:%02x",
                           action_param->address[5], action_param->address[4], action_param->address[3],
                           action_param->address[2], action_param->address[1], action_param->address[0]);

    if (!bt_sink_srv_cm_is_valid_address(&action_param->address)) {
        return;
    }

    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_alloc_device(&action_param->address);
    if (device_p == NULL) {
        bt_sink_srv_report("[Sink][CM] alloc device failed.");
    } else if (BT_SINK_SRV_CM_STATE_DISCONNECTED == device_p->link_state) {
        BT_SINK_SRV_CM_STATE_CHANGE(device_p, BT_SINK_SRV_CM_STATE_CONNECTING);
        bt_sink_srv_action_send(BT_SINK_SRV_ACTION_PROFILE_CONNECT, (void *)action_param);
    } else {
        bt_sink_srv_report("[Sink][CM] device state is not disconnected:%d", device_p->link_state);
    }

    return;
}

static void bt_sink_srv_cm_connect(bt_sink_srv_profile_connection_action_t *action_param)
{
    bt_sink_srv_cm_request_t request;

    bt_sink_srv_memset((void *)&request, 0, sizeof(bt_sink_srv_cm_request_t));
    request.type = BT_SINK_SRV_CM_REQ_TYPE_NORMAL;
    request.attampts = 1;
    bt_sink_srv_memcpy((void *)&request.action_param, (void *)action_param, sizeof(*action_param));
    bt_sink_srv_cm_request_new(&request);
}

static void bt_sink_srv_cm_disconnect(void)
{
    uint32_t count = 0;
    bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];

    count = bt_sink_srv_cm_get_connected_device(0, addr_list);
    for (uint32_t i = 0; i < count; i++) {
        bt_sink_srv_cm_remote_device_t *device_p =
            bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)&addr_list[i]);

        bt_sink_srv_report("[Sink][CM] Disconnect, device_p:0x%x", device_p);

        if (NULL != device_p) {
            bt_sink_srv_profile_connection_action_t action_param = {{0}};

            BT_SINK_SRV_CM_STATE_CHANGE(device_p, BT_SINK_SRV_CM_STATE_DISCONNECTING);
            bt_sink_srv_memcpy(action_param.address, (void *)&device_p->addr, sizeof(bt_bd_addr_t));
            bt_sink_srv_report("[Sink][CM] Disconnect, device profile_type:0x%x", device_p->profile_connection_mask);
            action_param.profile_connection_mask =  device_p->profile_connection_mask;
            bt_sink_srv_action_send(BT_SINK_SRV_ACTION_PROFILE_DISCONNECT, (void *)&action_param);
        }
    }
}

static void bt_sink_srv_cm_disconnect_by_address(bt_bd_addr_t* addr)
{

        bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)addr);

        bt_sink_srv_report("[Sink][CM] Disconnect, device_p:0x%x", device_p);

        if (NULL != device_p) {
            bt_sink_srv_profile_connection_action_t action_param = {{0}};
            BT_SINK_SRV_CM_STATE_CHANGE(device_p, BT_SINK_SRV_CM_STATE_DISCONNECTING);
            bt_sink_srv_memcpy(action_param.address, (void *)&device_p->addr, sizeof(bt_bd_addr_t));
            bt_sink_srv_report("[Sink][CM] Disconnect, device profile_type:0x%x", device_p->profile_connection_mask);
            action_param.profile_connection_mask =  device_p->profile_connection_mask;
            bt_sink_srv_action_send(BT_SINK_SRV_ACTION_PROFILE_DISCONNECT, (void *)&action_param);
        }
}

void bt_sink_srv_cm_disconnect_by_type(bt_sink_srv_cm_device_type_t type)
{
        bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
        uint32_t count = bt_sink_srv_cm_get_connected_device(type, addr_list);
        for (uint32_t i = 0; i < count; i++) {
            bt_sink_srv_cm_disconnect_by_address(&addr_list[i]);
    }
    return;
}

bt_sink_srv_cm_flags_t bt_sink_srv_cm_get_flags(void)
{
    bt_sink_srv_report("[Sink][CM] get flags:0x%x", g_sink_srv_cm_context.flags);
    return g_sink_srv_cm_context.flags;
}

void bt_sink_srv_cm_set_flags(bt_sink_srv_cm_flags_t flags)
{
    g_sink_srv_cm_context.flags |= flags;
    bt_sink_srv_report("[Sink][CM] set flags:0x%x, new flags:0x%x", flags, g_sink_srv_cm_context.flags);
    return;
}

void bt_sink_srv_cm_reset_flags(bt_sink_srv_cm_flags_t flags)
{
    g_sink_srv_cm_context.flags &=~ flags;
    bt_sink_srv_report("[Sink][CM] reset flags:0x%x, new flags:0x%x", flags, g_sink_srv_cm_context.flags);
    return;
}

void bt_sink_srv_cm_notify_timeout_handler(void *params)
{
    bt_sink_srv_cm_remote_device_t *device_p = (bt_sink_srv_cm_remote_device_t *)params;

    bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_CM_PROFILE_NOTIFY);

    if (NULL != device_p) {
        bt_sink_srv_event_param_t *event;

        bt_sink_srv_report("[Sink][CM] profile notify timeout");
        bt_sink_srv_report("[Sink][CM] address:%02x:%02x:%02x:%02x:%02x:%02x",
               device_p->addr[5], device_p->addr[4], device_p->addr[3],
               device_p->addr[2], device_p->addr[1], device_p->addr[0]);
        bt_sink_srv_cm_dev_info_t *record = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_cm_dev_info_t));
        if ((bt_sink_srv_db_find_record(&g_bt_sink_srv_cnmgr_db,
                                   (void *)&device_p->addr,
                                   (uint8_t *)record) == BT_SINK_SRV_STATUS_SUCCESS) && (record->dev_type == BT_SINK_SRV_DEVICE_UNKNOWN)) {
            bt_sink_srv_report("[Sink][CM] update record, profile_type:0x%02x", device_p->profile_connection_mask);
            //update device info if the connected profile is aws
            if (device_p->profile_connection_mask & BT_SINK_SRV_CM_PROFILE_AWS) {
                record->dev_type = BT_SINK_SRV_DEVICE_AWS_HEADSET;
            } else if (device_p->profile_connection_mask & BT_SINK_SRV_CM_PROFILE_SP) {
                record->dev_type = BT_SINK_SRV_DEVICE_PHONE;
            }
            bt_sink_srv_db_add_record(&g_bt_sink_srv_cnmgr_db,
                                  (uint8_t *)record,
                                  (void *)&device_p->addr);
        }
        bt_sink_srv_memory_free(record);

        if (NULL != g_sink_srv_cm_context.request_pending &&
            0 == bt_sink_srv_memcmp(&device_p->addr,
                     &g_sink_srv_cm_context.request_pending->parameters->action_param.address,
                     sizeof(bt_bd_addr_t)) &&
            device_p->profile_connection_mask == g_sink_srv_cm_context.request_pending->parameters->action_param.profile_connection_mask) {
            bt_sink_srv_cm_request_done(BT_SINK_SRV_STATUS_SUCCESS, g_sink_srv_cm_context.request_pending);
        }

        event = bt_sink_srv_memory_alloc(sizeof(*event));
        bt_sink_srv_memcpy(&event->connection_info.bt_addr, &device_p->addr, sizeof(device_p->addr));
        event->connection_info.profile_type = device_p->profile_connection_mask;
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE, event);
        bt_sink_srv_memory_free(event);
    }
}

static void bt_sink_srv_cm_profile_connection_state_notify(bt_bd_addr_t* address, bt_sink_srv_profile_type_t profile, bt_sink_srv_profile_connection_state_t state)
{
    //bt_sink_srv_report("[Sink][CM] profile type:0x%x, connection state:0x%x", profile, state);

    bt_sink_srv_event_param_t *event = NULL;
    event = bt_sink_srv_memory_alloc(sizeof(*event));
    bt_sink_srv_memcpy(&event->profile_state.address, address, sizeof(*address));
    event->profile_state.profile = profile;
    event->profile_state.state = state;
    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_PROFILE_CONNECTION_UPDATE, event);
    bt_sink_srv_memory_free(event);
    return;
}

void bt_sink_srv_cm_profile_status_notify(bt_bd_addr_t *addr, bt_sink_srv_profile_type_t profile, bt_sink_srv_profile_connection_state_t state)
{
    bt_sink_srv_report("[Sink][CM] profile notify, profile:0x%x, state:%d", profile, state);
    if (NULL != addr) {
        bt_sink_srv_cm_remote_device_t *device_p =
            bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)addr);
            bt_sink_srv_report("[Sink][CM] profile notify, device_p:0x%x", device_p);
        if (NULL != device_p) {
            bt_sink_srv_cm_profile_connection_state_notify(addr, profile, state);
            bt_sink_srv_profile_type_t previous = device_p->profile_connection_mask;
            if (state == BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED) {
                device_p->profile_connection_mask |= profile;
                if (((device_p->flag & BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING) == 0) && ((BT_SINK_SRV_PROFILE_AWS & device_p->profile_connection_mask) == 0)) {
                    if (device_p->local_role != BT_SINK_SRV_GAP_ROLE_SLAVE) {
                        bt_sink_srv_cm_set_role(addr, BT_ROLE_SLAVE);
                    }
                    else {
                        bt_sink_srv_cm_open_visibility_feature();
                    }
                }
            } else if (state == BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED) {
                device_p->profile_connection_mask &= (~profile);
            } else {
                bt_sink_srv_report("[Sink][CM] not support now");
                return;
            }
             bt_sink_srv_report("[Sink][CM] connection mask: 0x%x, previous: 0x%x", device_p->profile_connection_mask, previous);
            if (device_p->profile_connection_mask != previous) {
                if (device_p->profile_connection_mask == BT_SINK_SRV_CM_PROFILE_SP || device_p->profile_connection_mask == BT_SINK_SRV_CM_PROFILE_AWS) {
                    bt_sink_srv_event_param_t *event;
                    /*update device info if the all profile were connected with remote.*/
                    bt_sink_srv_report("[Sink][CM] All profiles notify, 0x%x", device_p->profile_connection_mask);
                    bt_sink_srv_report("[Sink][CM] address:%02x:%02x:%02x:%02x:%02x:%02x",
                           device_p->addr[5], device_p->addr[4], device_p->addr[3],
                           device_p->addr[2], device_p->addr[1], device_p->addr[0]);
                    bt_sink_srv_cm_dev_info_t *record = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_cm_dev_info_t));
                    if ((bt_sink_srv_db_find_record(&g_bt_sink_srv_cnmgr_db,
                                               (void *)&device_p->addr,
                                               (uint8_t *)record) == BT_SINK_SRV_STATUS_SUCCESS) && (record->dev_type == BT_SINK_SRV_DEVICE_UNKNOWN)) {
                        if (device_p->profile_connection_mask == BT_SINK_SRV_CM_PROFILE_SP) {
                            record->dev_type = BT_SINK_SRV_DEVICE_PHONE;
                        } else {
                            record->dev_type = BT_SINK_SRV_DEVICE_AWS_HEADSET;
                        }
                        bt_sink_srv_db_add_record(&g_bt_sink_srv_cnmgr_db,
                                              (uint8_t *)record,
                                              (void *)&device_p->addr);
                    }
                    bt_sink_srv_memory_free(record);

                    bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_CM_PROFILE_NOTIFY);

                    event = bt_sink_srv_memory_alloc(sizeof(*event));
                    bt_sink_srv_memcpy(&event->connection_info.bt_addr, addr, sizeof(*addr));
                    event->connection_info.profile_type = device_p->profile_connection_mask;
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE, event);
                    bt_sink_srv_report("[Sink][CM] handle:0x%x", device_p->handle);
                    bt_sink_srv_memory_free(event);

                    if (NULL != g_sink_srv_cm_context.request_pending &&
                        0 == bt_sink_srv_memcmp(addr,
                                 &g_sink_srv_cm_context.request_pending->parameters->action_param.address,
                                 sizeof(bt_bd_addr_t)) &&
                        device_p->profile_connection_mask & g_sink_srv_cm_context.request_pending->parameters->action_param.profile_connection_mask) {
                        bt_sink_srv_cm_request_done(BT_SINK_SRV_STATUS_SUCCESS, g_sink_srv_cm_context.request_pending);
                    }
                } else if (BT_SINK_SRV_PROFILE_NONE == device_p->profile_connection_mask) {
                    bt_sink_srv_event_param_t *event;

                    bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_CM_PROFILE_NOTIFY);

                    event = bt_sink_srv_memory_alloc(sizeof(*event));
                    bt_sink_srv_memcpy(&event->connection_info.bt_addr, addr, sizeof(*addr));
                    event->connection_info.profile_type = device_p->profile_connection_mask;
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE, event);
                    bt_sink_srv_memory_free(event);
                } else {
                    bt_sink_srv_timer_start(BT_SINK_SRV_TIMER_CM_PROFILE_NOTIFY,
                                            BT_SINK_SRV_CM_PROFILE_NOTIFY,
                                            bt_sink_srv_cm_notify_timeout_handler,
                                            device_p);
                }
            }
            if (BT_SINK_SRV_PROFILE_NONE == device_p->profile_connection_mask && state == BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED
                && device_p->link_state != BT_SINK_SRV_CM_STATE_CONNECTED) {
                // create link fail, like page time out, GAP will not notify, because link status is not changed.
                if (NULL != g_sink_srv_cm_context.request_pending
                        && 0 == bt_sink_srv_memcmp(addr,
                                                   &g_sink_srv_cm_context.request_pending->parameters->action_param.address,
                                                   sizeof(bt_bd_addr_t))) {
                    bt_sink_srv_cm_free_device(device_p);
                    bt_sink_srv_cm_request_done(BT_SINK_SRV_STATUS_FAIL, g_sink_srv_cm_context.request_pending);
                }
                // SPP only one link now, need to enhance
                bt_sink_srv_spp_disconnect(addr);
            }
        }
    }
}

static void bt_sink_srv_cm_system_power_off(void)
{
    bt_sink_srv_report("[Sink][CM] enter power off");
    bt_sink_srv_mp3_write_nvdm();
#ifdef __BT_SINK_SRV_VOICE_PROMPT__
    bt_sink_srv_vp_send_voice_prompt_event(BT_SINK_SRV_ACTION_POWER_OFF);
#else
    pmu_enter_power_off();
#endif
}

static void bt_sink_srv_cm_link_connected(const bt_bd_addr_t *dev_addr, bt_gap_connection_handle_t handle)
{
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_alloc_device((bt_bd_addr_t *)dev_addr);

    bt_sink_srv_report("[Sink][CM] connected gap handle:0x%x", handle);

    if (NULL == device_p) {
        bt_sink_srv_report("[Sink][CM] No connection available");
        bt_gap_disconnect(handle);
    } else {
        device_p->handle = handle;
        bt_sink_srv_memcpy(&g_sink_srv_cm_last_connected, dev_addr, sizeof(bt_bd_addr_t));
        g_sink_srv_cm_context.connected_dev_num++;
        if (g_sink_srv_cm_context.connected_dev_num == 1) {
            bt_gap_write_page_scan_activity(2048, 36);
        }
        BT_SINK_SRV_CM_STATE_CHANGE(device_p, BT_SINK_SRV_CM_STATE_CONNECTED);
        if (BT_SINK_SRV_CM_MAX_DEVICE_NUMBER == g_sink_srv_cm_context.connected_dev_num) {
            bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);
        } else if (1 == g_sink_srv_cm_context.connected_dev_num) {
            bt_sink_srv_state_set(BT_SINK_SRV_STATE_CONNECTED);
        }
        bt_gap_get_role(device_p->handle);
    }
}
static void bt_sink_srv_cm_link_disconnected(const bt_bd_addr_t *dev_addr, bt_gap_connection_handle_t handle)
{
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_HANDLE, &handle);

    if (NULL == device_p) {
        bt_sink_srv_report("[Sink][CM] No connection available");
    } else {
        BT_SINK_SRV_CM_STATE_CHANGE(device_p, BT_SINK_SRV_CM_STATE_DISCONNECTED);
        bt_sink_srv_cm_free_device(device_p);
        g_sink_srv_cm_context.connected_dev_num--;
        if (g_sink_srv_cm_context.le_connected_dev_num == 0 &&
            g_sink_srv_cm_context.connected_dev_num == 0) {
            bt_gap_write_page_scan_activity(2048, 18);
        }

        if (BT_SINK_SRV_CM_MAX_DEVICE_NUMBER > g_sink_srv_cm_context.connected_dev_num) {
            bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
        }
        bt_sink_srv_report("[Sink][CM] connected num:%d", g_sink_srv_cm_context.connected_dev_num);

        if (0 == g_sink_srv_cm_context.connected_dev_num) {
            bt_sink_srv_state_set( BT_SINK_SRV_STATE_POWER_ON);
            if (bt_sink_srv_cm_get_flags() & BT_SINK_SRV_FLAG_SYSTEM_POWER_OFF) {
                bt_sink_srv_cm_reset_flags(BT_SINK_SRV_FLAG_SYSTEM_POWER_OFF);
                bt_sink_srv_cm_system_power_off();
            }
        }
    }
}

uint32_t bt_sink_srv_cm_get_connected_device(bt_sink_srv_profile_type_t profile, bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER])
{
    uint8_t count = 0;

    if (NULL == addr_list) {
        return count;
    } else {
        bt_sink_srv_memset((void *)addr_list, 0, sizeof(bt_bd_addr_t) * BT_SINK_SRV_CM_MAX_DEVICE_NUMBER);
    }

    for (uint8_t i= 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
        if (BT_SINK_SRV_CM_STATE_CONNECTED == g_sink_srv_cm_context.devices[i].link_state) {
            if (BT_SINK_SRV_PROFILE_NONE == profile) {
                bt_sink_srv_memcpy(&addr_list[count], &g_sink_srv_cm_context.devices[i].addr, sizeof(bt_bd_addr_t));
                bt_sink_srv_report("[Sink][CM] connected gap handle:0x%x", g_sink_srv_cm_context.devices[i].handle);
                count++;
            } else {
                    if (g_sink_srv_cm_context.devices[i].profile_connection_mask & profile) {
                        bt_sink_srv_memcpy(&addr_list[count], &g_sink_srv_cm_context.devices[i].addr, sizeof(bt_bd_addr_t));
                        count++;
                    }
            }
        }
    }

    bt_sink_srv_report("[Sink][CM] connected count:%d", count);
    return count;
}

bool bt_sink_srv_cm_get_profile_data(bt_bd_addr_t *bt_addr, bt_sink_srv_profile_type_t profile, void *data_p)
{
    bool result = false;
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, bt_addr);
    bt_sink_srv_cm_dev_info_t *record = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_cm_dev_info_t));

    if (NULL != device_p && NULL != data_p &&
            bt_sink_srv_db_find_record(&g_bt_sink_srv_cnmgr_db,
                                       (void *)&device_p->addr,
                                       (uint8_t *)record) == BT_SINK_SRV_STATUS_SUCCESS) {
        switch (profile) {
#ifdef BT_SINK_SRV_HFP_STORAGE_SIZE
            case BT_SINK_SRV_PROFILE_HFP:
                bt_sink_srv_memcpy(data_p, record->hfp_info, BT_SINK_SRV_HFP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_A2DP_STORAGE_SIZE
            case BT_SINK_SRV_PROFILE_A2DP_SINK:
                bt_sink_srv_memcpy(data_p, record->a2dp_info, BT_SINK_SRV_A2DP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_AVRCP_STORAGE_SIZE
            case BT_SINK_SRV_PROFILE_AVRCP:
                bt_sink_srv_memcpy(data_p, record->avrcp_info, BT_SINK_SRV_AVRCP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_PBAP_STORAGE_SIZE
            case BT_SINK_SRV_PROFILE_PBAPC:
                bt_sink_srv_memcpy(data_p, record->pbap_info, BT_SINK_SRV_PBAP_STORAGE_SIZE);
                result = true;
                break;
#endif
            default:
                break;
        }
    }

    bt_sink_srv_memory_free(record);

    return result;
}

bool bt_sink_srv_cm_set_profile_data(bt_bd_addr_t *bt_addr, bt_sink_srv_profile_type_t profile, void *data_p)
{
    bool result = false;
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, bt_addr);
    bt_sink_srv_cm_dev_info_t *record = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_cm_dev_info_t));

    if (NULL != device_p && NULL != data_p &&
            bt_sink_srv_db_find_record(&g_bt_sink_srv_cnmgr_db,
                                       (void *)&device_p->addr,
                                       (uint8_t *)record) == BT_SINK_SRV_STATUS_SUCCESS) {
        switch (profile) {
#ifdef BT_SINK_SRV_HFP_STORAGE_SIZE
            case BT_SINK_SRV_PROFILE_HFP:
                bt_sink_srv_memcpy(record->hfp_info, data_p, BT_SINK_SRV_HFP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_A2DP_STORAGE_SIZE
            case BT_SINK_SRV_PROFILE_A2DP_SINK:
                bt_sink_srv_memcpy(record->a2dp_info, data_p, BT_SINK_SRV_A2DP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_AVRCP_STORAGE_SIZE
            case BT_SINK_SRV_PROFILE_AVRCP:
                bt_sink_srv_memcpy(record->avrcp_info, data_p, BT_SINK_SRV_AVRCP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_PBAP_STORAGE_SIZE
            case BT_SINK_SRV_PROFILE_PBAPC:
                bt_sink_srv_memcpy(record->pbap_info, data_p, BT_SINK_SRV_PBAP_STORAGE_SIZE);
                result = true;
                break;
#endif
            default:
                break;
        }
        bt_sink_srv_db_add_record(&g_bt_sink_srv_cnmgr_db,
                                  (uint8_t *)record,
                                  (void *)&device_p->addr);
    }
    bt_sink_srv_memory_free(record);
    return result;
}

void *bt_sink_srv_cm_get_profile_info(bt_bd_addr_t *address, bt_sink_srv_profile_type_t  profile)
{
    void *profile_info = NULL;
    for (uint8_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        if (bt_sink_srv_cm_is_valid_address(&g_sink_srv_cm_context.devices[i].addr)
                && 0 == bt_sink_srv_memcmp((uint8_t *)&g_sink_srv_cm_context.devices[i].addr, (uint8_t *)address, sizeof(bt_bd_addr_t))) {
            switch (profile) {
                        case BT_SINK_SRV_PROFILE_HFP:
                            profile_info = g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_HFP];
                            break;
                        case BT_SINK_SRV_PROFILE_A2DP_SINK:
                            profile_info = g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_A2DP_SINK];
                            break;
                        case BT_SINK_SRV_PROFILE_AVRCP:
                            profile_info = g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_AVRCP];
                            break;
                        case BT_SINK_SRV_PROFILE_PBAPC:
                            profile_info = g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_PBAPC];
                            break;
                        case BT_SINK_SRV_PROFILE_AWS:
                            profile_info = g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_AWS];
                            break;
                        default:
                            break;
                    }
            break;
        }
    }
    return profile_info;
}

bt_bd_addr_t *bt_sink_srv_cm_get_address_by_profile_info(void *profile_info)
{
      if (NULL != profile_info) {
        uint8_t i, j;
        // search connected device
        for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
            if (bt_sink_srv_cm_is_valid_address(&g_sink_srv_cm_context.devices[i].addr)) {
                // search profile
                for (j = 0; j < BT_SINK_SRV_CM_MAX_PROFILE_NUMBER; ++j) {
                    if (NULL != g_sink_srv_cm_context.devices[i].profile_info[j]
                            && profile_info == g_sink_srv_cm_context.devices[i].profile_info[j]) {
                        return &g_sink_srv_cm_context.devices[i].addr;
                    }
                }
            }
        }
    }
    return NULL;
}

bt_sink_srv_state_t bt_sink_srv_cm_set_device_state(bt_bd_addr_t *address, bt_sink_srv_state_t state)
{
    bt_sink_srv_state_t other_link_state = BT_SINK_SRV_STATE_NONE;
    // search connected device
    for (uint8_t i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        if (bt_sink_srv_cm_is_valid_address(&g_sink_srv_cm_context.devices[i].addr)
                && 0 == bt_sink_srv_memcmp((uint8_t *)&g_sink_srv_cm_context.devices[i].addr, (uint8_t *)address, sizeof(bt_bd_addr_t))) {
            g_sink_srv_cm_context.devices[i].device_state = state;
            if (0 == i) {
                other_link_state = g_sink_srv_cm_context.devices[1].device_state;
            } else {
                other_link_state = g_sink_srv_cm_context.devices[0].device_state;
            }
            break;
        }
    }
    return other_link_state;
}

static void bt_sink_srv_cm_discoverable_timeout(void *parameter)
{
    bt_sink_srv_report("[Sink][CM]scan mode:%d, connected dev num:%d",
         g_sink_srv_cm_context.scan_mode, g_sink_srv_cm_context.connected_dev_num);
    if (g_sink_srv_cm_context.scan_mode == BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE ||
                g_sink_srv_cm_context.scan_mode == BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY ) {
        if (g_sink_srv_cm_context.connected_dev_num < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER) {
            bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
        } else {
            bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);
        }
    }
    return;
}

static bool bt_sink_srv_cm_if_device_is_locked(bt_bd_addr_t* addr)
{
    int8_t i = BT_SINK_SRV_CM_MAX_TRUSTED_DEV-1;
    bool result = false;
    bool aws_is_existed = false;

    if (addr == NULL) {
        bt_sink_srv_report("[Sink][CM] Invalid address");
        return false;
    }

    while (i >= 0) {
        if (g_sink_srv_cm_dev_info[i].dev_type == BT_SINK_SRV_DEVICE_AWS_HEADSET) {
            aws_is_existed = true;
            if (bt_sink_srv_memcmp(addr, &g_sink_srv_cm_dev_info[i].address, sizeof(bt_bd_addr_t)) == 0) {
                result = true;
                break;
            }
        }
        i --;
    }

    if (!aws_is_existed) {
        bt_sink_srv_report("[Sink][CM] Not locked aws device");
        result = true;
    }

    bt_sink_srv_report("[Sink][CM] is AWS locked device:%d", result);
    return result;
}

bool bt_sink_srv_cm_check_aws_device_is_existed_in_paired_list(uint32_t* index)
{
    uint32_t i = 0;
    bool result = false;

    while ( i < g_bt_sink_srv_cnmgr_db.used) {
        if (g_sink_srv_cm_dev_info[i].dev_type == BT_SINK_SRV_DEVICE_AWS_HEADSET) {
            result = true;
            *index = i;
            break;
        }
        i++;
    }
    bt_sink_srv_report("[Sink][CM] is AWS device, result: %d, index:%d, ", result, i);
    return result;
}

static void bt_sink_srv_cm_power_on_callback(bt_sink_srv_status_t result, void *parameters)
{
    if (g_sink_srv_cm_context.connected_dev_num >= BT_SINK_SRV_CM_MAX_DEVICE_NUMBER) {
        bt_sink_srv_cm_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_POWER_ON);
    }
}

bt_sink_srv_status_t bt_sink_srv_cm_power_on_reconnection(void)
{
    uint32_t index, i;
    uint32_t master_request_conn_num = 0;
    bt_sink_srv_cm_request_t request;
    const bt_sink_srv_features_config_t *config_p = bt_sink_srv_get_features_config();
    bt_sink_srv_status_t result = BT_SINK_SRV_STATUS_FAIL;
    if (config_p->features & BT_SINK_SRV_FEATURE_POWER_ON_RECONNECT) {
        const bt_sink_srv_feature_reconnect_params_t * params = bt_sink_srv_get_power_on_reconnect_params();
        bt_sink_srv_report("[Sink][CM] reconnection params, attempts:%d, type:0x%x", params->attampts, params->remote);
        bt_sink_srv_memset(&request, 0, sizeof(bt_sink_srv_cm_request_t));
        request.type = BT_SINK_SRV_CM_REQ_TYPE_POWER_ON;
        request.attampts = params->attampts;
        request.callback = bt_sink_srv_cm_power_on_callback;
        request.action_param.profile_connection_mask = (BT_SINK_SRV_PROFILE_HFP | BT_SINK_SRV_PROFILE_A2DP_SINK);
        if (bt_sink_srv_cm_check_aws_device_is_existed_in_paired_list(&index)) {
            request.delay_time = BT_SINK_SRV_CM_POWER_ON_WAITING_REMOTE_AWS_CONNECTION;
            if (BT_SINK_SRV_DEVICE_PHONE == params->remote) {
                //Re-connect to the last phone.
                for (index = 0; index < g_bt_sink_srv_cnmgr_db.used; index++) {
                    i = g_bt_sink_srv_cnmgr_db.used - 1 - index;
                    if (g_sink_srv_cm_dev_info[i].dev_type == BT_SINK_SRV_DEVICE_PHONE) {
                        master_request_conn_num = 1;
                        //connect it.
                        bt_sink_srv_memcpy(&request.action_param.address, &g_sink_srv_cm_dev_info[i].address, sizeof(bt_bd_addr_t));
                        bt_sink_srv_report("[Sink][CM] reconnection address:%02x:%02x:%02x:%02x:%02x:%02x",
                                        g_sink_srv_cm_dev_info[i].address[5], g_sink_srv_cm_dev_info[i].address[4],
                                        g_sink_srv_cm_dev_info[i].address[3], g_sink_srv_cm_dev_info[i].address[2],
                                        g_sink_srv_cm_dev_info[i].address[1], g_sink_srv_cm_dev_info[i].address[0]);
                        bt_sink_srv_report("[Sink][CM] reconnection dev type:%d, delay:%d",
                                        g_sink_srv_cm_dev_info[i].dev_type,
                                        request.delay_time);
                        bt_sink_srv_report("[Sink][CM] reconnection mask:%d", request.action_param.profile_connection_mask);
                        result = bt_sink_srv_cm_request_new(&request);
                        break;
                    }
                }
            }
            else if (BT_SINK_SRV_DEVICE_AWS_HEADSET == params->remote) {
                //Re-connect to aws master.
                request.delay_time = BT_SINK_SRV_CM_POWER_ON_RECONNECT;
                request.action_param.profile_connection_mask = BT_SINK_SRV_PROFILE_AWS;
                bt_sink_srv_memcpy(&request.action_param.address, &g_sink_srv_cm_dev_info[index].address, sizeof(bt_bd_addr_t));
                bt_sink_srv_report("[Sink][CM] reconnection address:%02x:%02x:%02x:%02x:%02x:%02x",
                              g_sink_srv_cm_dev_info[index].address[5], g_sink_srv_cm_dev_info[index].address[4],
                              g_sink_srv_cm_dev_info[index].address[3], g_sink_srv_cm_dev_info[index].address[2],
                              g_sink_srv_cm_dev_info[index].address[1], g_sink_srv_cm_dev_info[index].address[0]);
                bt_sink_srv_report("[Sink][CM] reconnection dev type:%d, delay:%d", g_sink_srv_cm_dev_info[index].dev_type, request.delay_time);
                bt_sink_srv_report("[Sink][CM] reconnection mask:%d", request.action_param.profile_connection_mask);
                result = bt_sink_srv_cm_request_new(&request);
            }
        }
        else {
            request.delay_time = BT_SINK_SRV_CM_POWER_ON_WAITING_REMOTE_AWS_CONNECTION;
            if (BT_SINK_SRV_DEVICE_PHONE == params->remote) {
                //Re-connect ot the last 2 phone.
                for (index = 0; index < g_bt_sink_srv_cnmgr_db.used; index++) {
                    i = g_bt_sink_srv_cnmgr_db.used - 1 - index;
                    if (g_sink_srv_cm_dev_info[i].dev_type == BT_SINK_SRV_DEVICE_PHONE) {
                        master_request_conn_num++;
                        //connect it.
                        bt_sink_srv_memcpy(&request.action_param.address, &g_sink_srv_cm_dev_info[i].address, sizeof(bt_bd_addr_t));
                        bt_sink_srv_report("[Sink][CM] reconnection address:%02x:%02x:%02x:%02x:%02x:%02x",
                                        g_sink_srv_cm_dev_info[i].address[5], g_sink_srv_cm_dev_info[i].address[4],
                                        g_sink_srv_cm_dev_info[i].address[3], g_sink_srv_cm_dev_info[i].address[2],
                                        g_sink_srv_cm_dev_info[i].address[1], g_sink_srv_cm_dev_info[i].address[0]);
                        bt_sink_srv_report("[Sink][CM] reconnection dev type:%d, delay:%d",
                                        g_sink_srv_cm_dev_info[i].dev_type,
                                        request.delay_time);
                        bt_sink_srv_report("[Sink][CM] reconnection mask:%d", request.action_param.profile_connection_mask);
                        result = bt_sink_srv_cm_request_new(&request);
                    }
                    if (master_request_conn_num == 2) {
                        break;
                    }
                }
            }
        }
    }
    if ((open_visibility == 1) && (master_request_conn_num == 0)) {
        bt_sink_srv_cm_open_visibility_feature();
    }
    return result;
}

void bt_sink_srv_cm_link_loss_reconnection(bt_bd_addr_t* address)
{
    const bt_sink_srv_features_config_t *config_p = bt_sink_srv_get_features_config();
    if (config_p->features & BT_SINK_SRV_FEATURE_RECONNECT_ON_LINK_LOSS) {
        const bt_sink_srv_feature_reconnect_params_t * config_params = bt_sink_srv_get_link_loss_reconnect_params();
        bt_sink_srv_cm_request_t link_lost;
        uint32_t i = g_bt_sink_srv_cnmgr_db.used;
        bt_sink_srv_device_type_t dev_type = BT_SINK_SRV_DEVICE_UNKNOWN;

        bt_sink_srv_memset((void *)&link_lost, 0, sizeof(bt_sink_srv_cm_request_t));
        link_lost.type = BT_SINK_SRV_CM_REQ_TYPE_LINK_LOST;
        link_lost.attampts = config_params->attampts;
        link_lost.delay_time = BT_SINK_SRV_LINK_LOST_RECONNECT_DELAY;

        /*Get the remote device type in the paired list.*/
        while (i-- > 0  && g_sink_srv_cm_dev_info[i].dev_type != BT_SINK_SRV_DEVICE_UNKNOWN) {
            if (bt_sink_srv_memcmp(address, &g_sink_srv_cm_dev_info[i].address, sizeof(bt_bd_addr_t)) == 0) {
                dev_type = g_sink_srv_cm_dev_info[i].dev_type;
                break;
            }
        }

        bt_sink_srv_report("[Sink][CM] reconnection dev type:%d", dev_type);
        if (dev_type & config_params->remote) {
            if (dev_type == BT_SINK_SRV_DEVICE_AWS_HEADSET) {
                link_lost.action_param.profile_connection_mask = BT_SINK_SRV_PROFILE_AWS;
            } else if (dev_type == BT_SINK_SRV_DEVICE_PHONE) {
                link_lost.action_param.profile_connection_mask = (BT_SINK_SRV_PROFILE_HFP | BT_SINK_SRV_PROFILE_A2DP_SINK);
            }

            if (link_lost.action_param.profile_connection_mask) {
                bt_sink_srv_report("[Sink][CM] link lost reconnection mask:%d", link_lost.action_param.profile_connection_mask);
                bt_sink_srv_memcpy(&link_lost.action_param.address, address, sizeof(bt_bd_addr_t));
                bt_sink_srv_cm_request_new(&link_lost);
            }
        }
    }
}

void bt_sink_srv_cm_init(void)
{
    uint8_t i;

    bt_sink_srv_memset((void *)&g_sink_srv_cm_context, 0, sizeof(bt_sink_srv_cm_context_t));

    g_sink_srv_cm_context.request_list.next = &g_sink_srv_cm_context.request_list;
    g_sink_srv_cm_context.request_list.prev = &g_sink_srv_cm_context.request_list;

    for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        // register profiles
        g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_HFP] = bt_sink_srv_cm_get_hf_info(i);
        g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_A2DP_SINK] = bt_sink_srv_cm_get_a2dp_sink_info(i);
        g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_AVRCP] = bt_sink_srv_cm_get_avrcp_info(i);
        g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_PBAPC] = bt_sink_srv_cm_get_pbapc_info(i);
        g_sink_srv_cm_context.devices[i].profile_info[BT_SINK_SRV_PROFILE_INDEX_AWS] = bt_sink_srv_cm_get_aws_info(i);

        g_sink_srv_cm_context.devices[i].local_role = BT_SINK_SRV_GAP_ROLE_UNKNOWN;
        g_sink_srv_cm_context.devices[i].profile_connection_mask = BT_SINK_SRV_PROFILE_NONE;
        g_sink_srv_cm_context.devices[i].device_state = BT_SINK_SRV_STATE_POWER_ON;
    }
    
    //bt_sink_srv_music_init();
    // init profiles
    bt_sink_srv_action_send(BT_SINK_SRV_ACTION_PROFILE_INIT, NULL);

    if (BT_SINK_SRV_STATUS_SUCCESS == bt_sink_srv_db_open(&g_bt_sink_srv_cnmgr_db)) {
        g_bt_sink_srv_cnmgr_db.used = bt_device_manager_get_paired_number();
        bt_sink_srv_report("[Sink][CM] Get paired device num:%d", g_bt_sink_srv_cnmgr_db.used);
        bt_bd_addr_t tempaddr = {0};
         for (uint8_t i = 0; i < g_bt_sink_srv_cnmgr_db.count; i++) {
            bt_sink_srv_report("[Sink][CM] Get paired device[%d] type:0x%x", i, g_sink_srv_cm_dev_info[i].dev_type);
            bt_sink_srv_report("[Sink][CM] Get index:%d, addr:%02x:%02x:%02x:%02x:%02x:%02x", i,
                    g_sink_srv_cm_dev_info[i].address[5], g_sink_srv_cm_dev_info[i].address[4], g_sink_srv_cm_dev_info[i].address[3],
                    g_sink_srv_cm_dev_info[i].address[2], g_sink_srv_cm_dev_info[i].address[1], g_sink_srv_cm_dev_info[i].address[0]);
            if (bt_sink_srv_memcmp(&(g_sink_srv_cm_dev_info[i].address), &tempaddr, sizeof(bt_bd_addr_t)) != 0
                  && !bt_device_manager_is_paired(g_sink_srv_cm_dev_info[i].address)) {
                    bt_sink_srv_report("[Sink][CM] delete index:%d, addr:%02x:%02x:%02x:%02x:%02x:%02x", i,
                    g_sink_srv_cm_dev_info[i].address[5], g_sink_srv_cm_dev_info[i].address[4], g_sink_srv_cm_dev_info[i].address[3],
                    g_sink_srv_cm_dev_info[i].address[2], g_sink_srv_cm_dev_info[i].address[1], g_sink_srv_cm_dev_info[i].address[0]);
                bt_sink_srv_db_delete_record(&g_bt_sink_srv_cnmgr_db, (void *)g_sink_srv_cm_dev_info[i].address);
            }
        }
    } else {
        bt_sink_srv_report("[Sink][CM] Opend database failed");
    }
}

static void bt_sink_srv_cm_deinit(void)
{
    bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY);
    bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_CM_DISCOVERABLE);
    bt_sink_srv_cm_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_NONE);
    g_sink_srv_cm_context.request_pending = NULL;
    bt_sink_srv_action_send(BT_SINK_SRV_ACTION_PROFILE_DEINIT, NULL);
    bt_sink_srv_db_close(&g_bt_sink_srv_cnmgr_db);
}

static void bt_sink_srv_cm_set_eir()
{
    bt_sink_srv_report("[Sink][CM] set  eir");
    const bt_sink_srv_eir_information_t* eir_info = bt_sink_srv_get_eir();
    const bt_gap_config_t * sink_gap_config = bt_sink_srv_gap_config();
    uint32_t name_length;
    uint8_t eir_data[56] = {0x11, 0x06};
    name_length = strlen(sink_gap_config->device_name) + 1;
    bt_sink_srv_memcpy((void *)(eir_data + 2), (void *) eir_info->uuid_128bit, 16);
    eir_data[18] = name_length + 1;
    eir_data[19] = 0x09;
    bt_sink_srv_memcpy((void *)(eir_data + 20), (void *) sink_gap_config->device_name, name_length);
    bt_gap_set_extended_inquiry_response(eir_data, 20 + name_length);
}

void bt_sink_srv_cm_inquiry(void)
{
    const bt_sink_srv_features_config_t *config_p = bt_sink_srv_get_features_config();
    bt_sink_srv_report("[Sink][CM] inquiry flag:0x%x", config_p->features);
    if (config_p->features & BT_SINK_SRV_FEATURE_INQUIRY_AND_CONNECT) {
        const bt_sink_srv_feature_inquiry_and_connect_params_t * conn_params = bt_sink_srv_get_inquiry_and_connect_aws_params();
        bt_gap_inquiry(conn_params->inquiry_duration, conn_params->max_count);
    }
}

static void bt_sink_srv_cm_handle_power_on_cnf(void)
{
    const bt_sink_srv_features_config_t *config_p = bt_sink_srv_get_features_config();
#ifdef __BT_SINK_SRV_VOICE_PROMPT__
    bt_sink_srv_vp_dev_init();
#endif
    bt_sink_srv_cm_init();
    bt_sink_srv_state_set(BT_SINK_SRV_STATE_POWER_ON);
    bt_sink_srv_cm_set_flags(BT_SINK_SRV_FLAG_POWER_ON);
    bt_sink_srv_cm_set_eir();
    if (config_p->features & BT_SINK_SRV_FEATURE_VISIBILITY) {
        open_visibility = 1;
    }
    /* check power on reconnect feature*/
    if (config_p->features & BT_SINK_SRV_FEATURE_POWER_ON_RECONNECT) {
        bt_sink_srv_cm_power_on_reconnection();
    }
    else if (open_visibility == 1) {
        bt_sink_srv_cm_open_visibility_feature();
    }
}

static void bt_sink_srv_cm_handle_power_off_cnf(void)
{
    bt_sink_srv_cm_deinit();
    bt_sink_srv_cm_reset_flags(BT_SINK_SRV_FLAG_POWER_ON);
    bt_sink_srv_state_set(BT_SINK_SRV_STATE_NONE);
}

void bt_sink_srv_cm_write_scan_enable_mode(bt_gap_scan_mode_t mode)
{
    bt_sink_srv_report("[Sink][CM] scan mode, old: %x, new:%x", g_sink_srv_cm_context.scan_mode, mode);

    if (bt_sink_srv_cm_get_flags() & BT_SINK_SRV_FLAG_SET_SCAN_MODE) {
        return;
    }

    if (g_sink_srv_cm_context.scan_mode != mode) {
            g_sink_srv_cm_context.scan_mode = mode;
            bt_sink_srv_cm_set_flags(BT_SINK_SRV_FLAG_SET_SCAN_MODE);
    }

    if (bt_sink_srv_cm_get_flags() & BT_SINK_SRV_FLAG_SET_SCAN_MODE) {
        const bt_sink_srv_features_config_t *config_p = bt_sink_srv_get_features_config();
        if (config_p->features & BT_SINK_SRV_FEATURE_VISIBILITY) {
            if (g_sink_srv_cm_context.scan_mode == BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE ||
                g_sink_srv_cm_context.scan_mode == BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY ) {
                const bt_sink_srv_feature_visibility_params_t * visibility_params = bt_sink_srv_get_visibility_params();
                bt_sink_srv_timer_start(BT_SINK_SRV_TIMER_CM_DISCOVERABLE, visibility_params->visibility_duration,
                                    bt_sink_srv_cm_discoverable_timeout, NULL);
            } else {
                if (bt_sink_srv_timer_is_exist(BT_SINK_SRV_TIMER_CM_DISCOVERABLE)) {
                    bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_CM_DISCOVERABLE);
                }
            }
        }
        bt_gap_set_scan_mode(mode);
        bt_sink_srv_report("[Sink][CM] scan enable:%x", mode);
    }
}

static void bt_sink_srv_cm_handle_link_disconnected(bt_status_t status, bt_gap_link_status_updated_ind_t *param)
{
    bt_sink_srv_report("[Sink][CM] detach single link, reason: 0x%x, handle = 0x%x", status, param->handle);
    bt_sink_srv_cm_link_disconnected(param->address, param->handle);

    if (NULL != g_sink_srv_cm_context.request_pending
            && 0 == bt_sink_srv_memcmp(param->address, &g_sink_srv_cm_context.request_pending->parameters->action_param.address, sizeof(bt_bd_addr_t))) {
        bt_sink_srv_cm_request_done(BT_SINK_SRV_STATUS_FAIL, g_sink_srv_cm_context.request_pending);
    }

    if (BT_SINK_SRV_CM_REASON_CONNECTION_TIMEOUT == status) {
        bt_sink_srv_cm_link_loss_reconnection((bt_bd_addr_t* )param->address);
    }
}

static void bt_sink_srv_cm_handle_link_connected(bt_status_t status, const bt_gap_link_status_updated_ind_t *param)
{
    if (status != BT_STATUS_SUCCESS) {
        bt_sink_srv_cm_remote_device_t *device_p
            = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)param->address);

        if (NULL != device_p) {
            BT_SINK_SRV_CM_STATE_CHANGE(device_p, BT_SINK_SRV_CM_STATE_DISCONNECTED);
        }
        bt_sink_srv_cm_free_device(device_p);
        bt_sink_srv_report("[Sink][CM] Create link failed, %x", status);
        bt_sink_srv_cm_request_done(BT_SINK_SRV_STATUS_FAIL, g_sink_srv_cm_context.request_pending);
    } else {
        bt_sink_srv_cm_link_connected(param->address, param->handle);

        // To avoid controller issue: if one link is paging, another link role switch will fail,
        // request done when all profiles are connected.
        //if (0 == bt_sink_srv_memcmp(param->address, &g_sink_srv_cm_context.request_pending->parameters->action_param.bt_addr, sizeof(bt_bd_addr_t))) {
        //    bt_sink_srv_cm_request_done(BT_SINK_SRV_STATUS_SUCCESS, g_sink_srv_cm_context.request_pending);
        //}
        /*If the link is connected  due to link lost reconnect, and the reconnection is still running, we need finish this process. */
        if (NULL != g_sink_srv_cm_context.request_pending
                && 0 == bt_sink_srv_memcmp(param->address, &g_sink_srv_cm_context.request_pending->parameters->action_param.address, sizeof(bt_bd_addr_t))
                && g_sink_srv_cm_context.request_pending->parameters->type == BT_SINK_SRV_CM_REQ_TYPE_LINK_LOST) {
            bt_sink_srv_cm_request_done(BT_SINK_SRV_STATUS_SUCCESS, g_sink_srv_cm_context.request_pending);
        }

    }
}

bt_bd_addr_t *bt_sink_srv_cm_last_connected_device(void)
{
    return &g_sink_srv_cm_last_connected;
}

void bt_sink_srv_cm_set_role(bt_bd_addr_t *address_p, bt_role_t role)
{
    if (NULL != address_p) {
        bt_sink_srv_cm_remote_device_t *remote_device_p =
            bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)address_p);

        if (NULL != remote_device_p && remote_device_p->local_role != role) {
            bt_sink_srv_report("[Sink][CM] Set role, %x", role);
            if (BT_STATUS_SUCCESS == bt_gap_set_role(remote_device_p->handle, role)) {
                remote_device_p->flag |= BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING;
            }
        }
    }
}

bt_sink_srv_gap_role_type_t bt_sink_srv_cm_get_gap_role(bt_bd_addr_t *address_p)
{
    bt_sink_srv_gap_role_type_t local_role = BT_SINK_SRV_GAP_ROLE_UNKNOWN;
    if (NULL != address_p) {
        bt_sink_srv_cm_remote_device_t *remote_device_p =
            bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)address_p);
        if (NULL != remote_device_p) {
            local_role =  remote_device_p->local_role;
        }
    }
    bt_sink_srv_report("[Sink][CM] get gap role:0x%x", local_role);
    return local_role;
}

static void bt_sink_srv_cm_handle_link_updata_ind(bt_status_t status, bt_gap_link_status_updated_ind_t *param)
{
    bt_sink_srv_report("[Sink][CM] link staus:0x%x", param->link_status);

    if (BT_GAP_LINK_STATUS_CONNECTED_0 == param->link_status) {
        bt_sink_srv_cm_handle_link_connected(status, param);
    } else if (BT_GAP_LINK_STATUS_DISCONNECTED == param->link_status) {
        bt_sink_srv_cm_handle_link_disconnected(status, param);
    } else if (BT_GAP_LINK_STATUS_CONNECTED_3 == param->link_status) {
    }
}

bt_sink_srv_status_t bt_sink_srv_cm_action_handler(bt_sink_srv_action_t action, void *param)
{
    bt_sink_srv_report("[Sink][CM] bt_sink_srv_cm_action_handler, action:%x", action);

    switch (action) {
        case BT_SINK_SRV_ACTION_INQUIRY:
            bt_sink_srv_cm_inquiry();
            break;

        case BT_SINK_SRV_ACTION_CONNECT: {
            bt_sink_srv_profile_connection_action_t connect_action = {{0}};
            bt_sink_srv_cm_remote_device_t *device_p = NULL;
            if (NULL != param) {
                bt_sink_srv_memcpy((void *)&connect_action.address, param, sizeof(bt_bd_addr_t));
            } else {
                bt_sink_srv_memcpy((void *)&connect_action.address, (const void*)&g_sink_srv_cm_last_connected, sizeof(bt_bd_addr_t));
            }
            device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)connect_action.address);
            if (device_p != NULL) {
                bt_sink_srv_report("[Sink][CM] profile connection mask:0x%x", device_p->profile_connection_mask);
                connect_action.profile_connection_mask = device_p->profile_connection_mask;
            } else {
                connect_action.profile_connection_mask = (BT_SINK_SRV_PROFILE_HFP | BT_SINK_SRV_PROFILE_A2DP_SINK);
            }
            bt_sink_srv_cm_connect(&connect_action);
        }
        break;

        case BT_SINK_SRV_ACTION_DISCONNECT:
            bt_sink_srv_cm_disconnect();
            break;

        case BT_SINK_SRV_ACTION_DISCOVERABLE:
        {
            const bt_sink_srv_features_config_t *config_p = bt_sink_srv_get_features_config();
            if ((config_p->features & BT_SINK_SRV_FEATURE_VISIBILITY) &&
                  (g_sink_srv_cm_context.connected_dev_num >= BT_SINK_SRV_CM_MAX_DEVICE_NUMBER)) {
                bt_sink_srv_report("[Sink][CM] Max connected num: %d, can't be discoverable.", g_sink_srv_cm_context.connected_dev_num);
                break;
            }
            bt_sink_srv_cm_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_POWER_ON);
            bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);
        }
            break;

        case BT_SINK_SRV_ACTION_RESET_TRUSTED_LIST:
        {
            bt_sink_srv_db_delete_all_records(&g_bt_sink_srv_cnmgr_db);
            bt_device_manager_unpair_all();
        }
        break;

        case BT_SINK_SRV_ACTION_POWER_OFF: {
            uint32_t count = 0;
            bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
            count = bt_sink_srv_cm_get_connected_device(0, addr_list);
            bt_sink_srv_report("[Sink][CM] power off system, connected_dev_num:%d", count);
            if (0 == count) {
                bt_sink_srv_cm_system_power_off();
            #ifdef __BT_AWS_SUPPORT__
            } else if (bt_sink_srv_aws_system_send_power_off() == BT_SINK_SRV_STATUS_SUCCESS) {
                bt_sink_srv_cm_set_flags(BT_SINK_SRV_FLAG_SYSTEM_POWER_OFF);
            #endif /*__BT_AWS_SUPPORT__*/
            } else {
                bt_sink_srv_cm_set_flags( BT_SINK_SRV_FLAG_SYSTEM_POWER_OFF);
                bt_sink_srv_cm_disconnect();
            }
            break;
        }
        default:
            break;
    }

    return BT_SINK_SRV_STATUS_SUCCESS;
}

static void bt_sink_srv_cm_handle_bt_panic(void)
{
    hal_wdt_software_reset();
    return;
}

bt_status_t bt_sink_srv_cm_system_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
#if defined(__BT_SINK_SRV_DEBUG_INFO__ ) && defined (__BT_SINK_SRV_TRACE__)
    if (msg >= BT_POWER_ON_CNF && msg <= BT_PANIC) {
        bt_sink_srv_report("[Sink][CM] SDK msg:%s", g_system_event_string[msg - BT_POWER_ON_CNF]);
    } else {
        bt_sink_srv_report("[Sink][CM] SDK msg:0x%x", msg);
    }
#else
    bt_sink_srv_report("[Sink][CM] SDK msg:0x%x", msg);
#endif /* defined(__BT_SINK_SRV_DEBUG_INFO__ ) && defined (__BT_SINK_SRV_TRACE__) */
    switch (msg) {
        case BT_POWER_ON_CNF: {
            bt_sink_srv_cm_handle_power_on_cnf();
        }
            break;
        case BT_POWER_OFF_CNF:
            bt_sink_srv_cm_handle_power_off_cnf();
            break;
       case BT_PANIC:
           bt_sink_srv_cm_handle_bt_panic();
           break;
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

bool bt_sink_srv_cm_if_permit_aws_connection(bt_bd_addr_t *address)
{
    bool result = false;
    const bt_sink_srv_features_config_t *config_p = bt_sink_srv_get_features_config();
    if (config_p->features & BT_SINK_SRV_FEATURE_LOCK_AWS_DEVICE) {
        if (bt_sink_srv_cm_if_device_is_locked(address)) {
            result = true;
        }
    } else {
        result = true;
    }
     bt_sink_srv_report("[Sink][CM] permit result:%d", result);
    return result;
}

static bool bt_sink_srv_cm_check_if_right_aws_device(const uint8_t* eir, const uint8_t* rssi)
{
    bool result = false;
    const bt_sink_srv_features_config_t *config_p = bt_sink_srv_get_features_config();
    if (config_p->features & BT_SINK_SRV_FEATURE_INQUIRY_AND_CONNECT) {
        const bt_sink_srv_eir_information_t* aws_pattern = bt_sink_srv_get_eir();
        uint8_t eir_data_len = eir[0];
        uint8_t eir_data_type = eir[1];
        if (eir_data_len == 17 && eir_data_type == BT_GAP_LE_AD_TYPE_128_BIT_UUID_PART) {
            if (bt_sink_srv_memcmp( (const void *)(eir + 2), (const void *)(aws_pattern->uuid_128bit), 16) == 0) {
                if (*rssi >= aws_pattern->rssi) {
                    result = true;
                }
            }
        }
    }

    bt_sink_srv_report("[Sink][CM] check result:%d, rssi:0x%x", result, *rssi);
    return result;
}

bt_status_t bt_sink_srv_cm_gap_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    bt_sink_srv_report("[Sink][CM] bt_sink_srv_cm_gap_callback, msg:0x%x", msg);

#if defined( __BT_SINK_SRV_DEBUG_INFO__) && defined (__BT_SINK_SRV_TRACE__)
    if (msg == BT_GAP_LINK_STATUS_UPDATED_IND ) {
        bt_sink_srv_report("[Sink][CM] SDK msg:%s", g_gap_event_string[0]);
    } else if (msg >= BT_GAP_IO_CAPABILITY_REQ_IND && msg <= BT_GAP_WRITING_INQUIRY_SCAN_CNF) {
        bt_sink_srv_report("[Sink][CM] SDK msg:%s", g_gap_event_string[msg - BT_GAP_IO_CAPABILITY_REQ_IND + 1]);
    } else {
        bt_sink_srv_report("[Sink][CM] SDK msg:0x%x", msg);
    }
#else
    bt_sink_srv_report("[Sink][CM] SDK msg:0x%x", msg);
#endif /* __BT_SINK_SRV_DEBUG_INFO__ */

    switch (msg) {
        case BT_GAP_LINK_STATUS_UPDATED_IND:
            bt_sink_srv_cm_handle_link_updata_ind(status, (bt_gap_link_status_updated_ind_t *)buffer);
            break;
        case BT_GAP_SET_SCAN_MODE_CNF: {
            bt_sink_srv_cm_reset_flags(BT_SINK_SRV_FLAG_SET_SCAN_MODE);
            bt_sink_srv_event_param_t event;
            if (g_sink_srv_cm_context.scan_mode == BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE ||
                g_sink_srv_cm_context.scan_mode == BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY) {
                event.visibility_state.visibility_on = true;
            } else {
                event.visibility_state.visibility_on = false;
            }
            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_VISIBILITY_STATE_UPDATE, (void *)(&event));
        }
        break;

        case BT_GAP_GET_ROLE_CNF: {
            bt_gap_get_role_cnf_t *get_role = (bt_gap_get_role_cnf_t *)buffer;
            bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_HANDLE, &(get_role->handle));
            if (device_p) {
                device_p->local_role = get_role->local_role;
                bt_sink_srv_report("[Sink][CM] get role cnf: %d", device_p->local_role);
            }
        }
        break;
        case BT_GAP_SET_ROLE_CNF: {

        }
        break;
        case BT_GAP_ROLE_CHANGED_IND: {
            bt_gap_role_changed_ind_t *role_change = (bt_gap_role_changed_ind_t *)buffer;
            bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_HANDLE, &(role_change->handle));
            if (device_p) {
                device_p->local_role = role_change->local_role;
                device_p->flag &= ~BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING;
                bt_sink_srv_cm_open_visibility_feature();
                bt_sink_srv_report("[Sink][CM] role changed ind: %d", device_p->local_role);
                bt_sink_srv_event_param_t *event;
                event = bt_sink_srv_memory_alloc(sizeof(*event));
                event->role_info.role = role_change->local_role;
                bt_sink_srv_memcpy(&event->role_info.address, (void *)&device_p->addr, sizeof(bt_bd_addr_t));
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_ROLE_CHANGED_IND, (void *)event);
                bt_sink_srv_memory_free(event);
            }
        }
        break;
        case BT_GAP_SNIFF_MODE_CHANGE_IND:
        {
            bt_gap_sniff_mode_changed_ind_t* ind;
            ind = (bt_gap_sniff_mode_changed_ind_t*)buffer;
            bt_sink_srv_cm_remote_device_t *aws_device_p = bt_sink_srv_cm_get_aws_connected_device();
            bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_HANDLE, &(ind->handle));
            if (device_p && aws_device_p && (aws_device_p != device_p)) {
                bt_gap_exit_sniff_mode(aws_device_p->handle);
            }
            break;
        }
        case BT_GAP_INQUIRY_IND: {
            bt_gap_inquiry_ind_t *result_ind = (bt_gap_inquiry_ind_t *)buffer;
            if (bt_sink_srv_cm_check_if_right_aws_device(result_ind->eir, result_ind->rssi)
                && bt_sink_srv_cm_if_permit_aws_connection((bt_bd_addr_t *)result_ind->address)) {
                bt_gap_cancel_inquiry();
                bt_sink_srv_profile_connection_action_t action_param = {{0}, };
                bt_sink_srv_memcpy(&action_param.address, result_ind->address, sizeof(bt_bd_addr_t));
                action_param.profile_connection_mask = BT_SINK_SRV_PROFILE_AWS;
                bt_sink_srv_cm_connect(&action_param);
            }
        }
        break;

        case BT_GAP_LE_CONNECT_IND:
            if (status == BT_STATUS_SUCCESS) {
                g_sink_srv_cm_context.le_connected_dev_num++;
                if (g_sink_srv_cm_context.le_connected_dev_num == 1) {
                    bt_gap_write_page_scan_activity(2048, 36);
                }
            }
            break;

        case BT_GAP_LE_DISCONNECT_IND:
            g_sink_srv_cm_context.le_connected_dev_num--;
            if (g_sink_srv_cm_context.le_connected_dev_num == 0 &&
                g_sink_srv_cm_context.connected_dev_num == 0) {
                bt_gap_write_page_scan_activity(2048, 18);
            }
            break;

        default:
            bt_sink_srv_report("[Sink][CM] Unexcepted msg:%x", msg);
            break;
    }
    return result;
}

uint8_t bt_sink_srv_get_connected_device_number(void)
{
    return g_sink_srv_cm_context.connected_dev_num;
}

void bt_sink_paird_list_changed(bt_device_manager_paired_event_t event, bt_bd_addr_ptr_t address)
{
    bt_sink_srv_report("[Sink][CM] bt_sink_paird_list_changed() event:0x%x", event);

    switch (event) {
        case BT_DEVICE_MANAGER_PAIRED_KEY_MISS:
        {
            bt_sink_srv_assert(address);
            bt_sink_srv_db_delete_record(&g_bt_sink_srv_cnmgr_db, (void *)address);
        }
        break;

        case BT_DEVICE_MANAGER_PAIRED_LIST_FULL:
        {
            bt_sink_srv_assert(address);
            bt_sink_srv_db_delete_record(&g_bt_sink_srv_cnmgr_db, (void *)address);
        }
        break;

        case BT_DEVICE_MANAGER_PAIRED_ADD:
        {
            bt_sink_srv_assert(address);
            bt_sink_srv_cm_dev_info_t *record = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_cm_dev_info_t));
            if (bt_sink_srv_db_find_record(&g_bt_sink_srv_cnmgr_db, (void *)address,
                (uint8_t *)record) != BT_SINK_SRV_STATUS_SUCCESS) {
                bt_sink_srv_memset((void *)record, 0, sizeof(bt_sink_srv_cm_dev_info_t));
                bt_sink_srv_memcpy((void *)&record->address, (void *)address, sizeof(bt_bd_addr_t));
                bt_sink_srv_db_add_record(&g_bt_sink_srv_cnmgr_db, (uint8_t *)record, (void *)address);
            }
            bt_sink_srv_memory_free(record);
        }
        break;

        case BT_DEVICE_MANAGER_PAIRED_UNPAIR:
        {
            bt_sink_srv_assert(address);
            bt_sink_srv_db_delete_record(&g_bt_sink_srv_cnmgr_db, (void *)address);
        }
        break;

        case BT_DEVICE_MANAGER_PAIRED_UNPAIR_ALL:
        default:
            bt_sink_srv_report("[Sink][CM] unhandle event: 0x%x", event);
        break;
    }
    return;
}

/*It become dummmy funciton after SDK 4.2 release becuase it is unused when BT device manager module is enable.*/
void bt_sink_srv_cm_reply_link_key(bt_gap_link_key_notification_ind_t *key_information)
{
    return;
}

