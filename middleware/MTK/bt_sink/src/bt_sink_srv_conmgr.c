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

#include "bt_sink_srv_conmgr.h"
#include "bt_gap.h"
// for EIR parsing
#include "bt_gap_le.h"
#include "bt_system.h"
#include "bt_sink_srv_action.h"
#include "bt_sink_srv_hf.h"
#include "bt_sink_srv_avrcp.h"
#include "bt_sink_srv_a2dp.h"
#include "bt_sink_srv_pbapc.h"
#include "bt_sink_srv_spp.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_db.h"
#include "bt_sink_srv_custom_db.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_sink_srv_common.h"
#include "bt_sink_srv_ami.h"
#ifdef __BT_AWS_SUPPORT__
#include "bt_sink_srv_aws.h"
#endif
#define __QA_TEST__
#ifdef __QA_TEST__
#include "nvdm.h"
#endif

extern bt_sink_srv_profile_t g_sink_srv_hf[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
extern bt_sink_srv_profile_t g_sink_srv_pbapc[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
extern SinkProfileA2dp sink_a2dp[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
extern SinkProfileAvrcp sink_avrcp[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
#ifdef __BT_AWS_SUPPORT__
extern SinkProfileAws sink_aws[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
#endif

#ifdef MTK_BT_COMMAND_ENABLE
void default_bt_sink_srv_custom_event_callback(bt_sink_srv_event_t event_id, void *param, uint32_t param_length)
{
    bt_sink_srv_report("[Sink] event_id:%d, param:0x%x", event_id, param,param_length);
}

// Weak symbol declaration
#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_sink_srv_custom_event_callback=_default_bt_sink_srv_custom_event_callback")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_sink_srv_custom_event_callback = default_bt_sink_srv_custom_event_callback
#else
#error "Unsupported Platform"
#endif

#endif
static uint8_t bt_sink_srv_db_common_comparator(const uint8_t *record, const void *key);

#ifdef __QA_TEST__
static bt_gap_config_t g_bt_sink_srv_gap_config = {
    .inquiry_mode  = 2, /**< It indicates the inquiry result format.
                                                        0: Standerd inquiry result format (Default).
                                                        1: Inquiry result format with RSSI.
                                                        2: Inquiry result with RSSI format or Extended Inquiry Result(EIR) format. */
    .io_capability = BT_SINK_SRV_CM_DB_IO,
    .cod           = BT_SINK_SRV_CM_DB_COD, /* It defines the class of the local device. */
    .device_name   = {BT_SINK_SRV_CM_DB_NAME}, /* It defines the name of the local device with '\0' ending. */
};
#else
const static bt_gap_config_t g_bt_sink_srv_gap_config = {
    .inquiry_mode  = 2, /**< It indicates the inquiry result format.
                                                        0: Standerd inquiry result format (Default).
                                                        1: Inquiry result format with RSSI.
                                                        2: Inquiry result with RSSI format or Extended Inquiry Result(EIR) format. */
    .io_capability = BT_SINK_SRV_CM_DB_IO,
    .cod           = BT_SINK_SRV_CM_DB_COD, /* It defines the class of the local device. */
    .device_name   = {BT_SINK_SRV_CM_DB_NAME}, /* It defines the name of the local device with '\0' ending. */
};
#endif

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
    bt_sink_srv_report("[Sink][CM] State Change, previous_state:%d, new_state:%d", device->state, new_state); \
    device->state = new_state;                                    \
}

#if defined(__BT_SINK_SRV_DEBUG_INFO__ ) && defined (__BT_SINK_SRV_TRACE__)
const static char *g_gap_event_string[] = {
    "BT_GAP_LINK_STATUS_UPDATED_IND",    /**< This event indicates the the connection status changed, with #bt_gap_link_status_updated_ind_t payload. */
    "BT_GAP_LINK_KEY_REQ_IND",    /**< This event asks user for link key, with #bt_gap_link_key_notification_ind_t payload, user need fill the parameters with key information if the link key is found. */
    "BT_GAP_IO_CAPABILITY_REQ_IND",    /**< This event asks user for io capability response, with #bt_gap_connection_handle_t payload, user need invoke #bt_gap_reply_io_capability_request() API to accept and reply the pairing request.
                                                                                        or invoke #bt_gap_reject_io_capability_request() API to reject the pairing request. */
    "BT_GAP_USER_CONFIRM_REQ_IND",    /**< This event asks user for value confirm, with uint32_t payload, user need invoke #bt_gap_reply_user_confirm_request() API to replay it according to value of event parameters. */
    "BT_GAP_USER_PASSKEY_NOTIFICATION_IND",    /**< This event notify the passkey value to the user, with uint32_t payload, user need display the passkey on the screen. */
    "BT_GAP_USER_PASSKEY_NOTIFICATION_END_IND",    /**< This event notify the passkey enterd to the user, with NULL payload. user need cancel the passkey showing on the screen. */
    "BT_GAP_LINK_KEY_NOTIFICATION_IND",    /**< This event notify user to update link key information, with #bt_gap_link_key_notification_ind_t payload. */
    "BT_GAP_USER_REPLY_PASSKEY_IND",    /**< This event asks user for passkey, with NULL payload, user need invoke #bt_gap_reply_passkey_request() API to replay it according to value of event parameters. */
    "BT_GAP_SET_SCAN_MODE_CNF",    /**< This event indicates the confirmation of setting scan mode with NULL payload. */
    "BT_GAP_INQUIRY_CNF",   /**< This event indicates the confirmation of inquiry with NULL payload. */
    "BT_GAP_CANCEL_INQUIRY_CNF",    /**< This event indicates the confirmation of cancel inquiry with NULL payload. */
    "BT_GAP_INQUIRY_IND",   /**< This event indicates the result of inquiry with #bt_gap_inquiry_ind_t payload. */
    "BT_GAP_INQUIRY_COMPLETE_IND",    /**< This event indicates the completement of inquiry with NULL payload. */
    "BT_GAP_SET_ROLE_CNF",   /**< This event indicates the confirmation of #bt_gap_set_role() with NULL payload. */
    "BT_GAP_ROLE_CHANGED_IND",    /**< This event indicates the connection role changed with #bt_gap_role_changed_ind_t payload. */
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
    "BT_GAP_BONDING_COMPLETE_IND" /**< This event indicates the result of bonding with NULL payload. */
};

const static char *g_system_event_string[] = {
    "BT_POWER_ON_CNF",
    "BT_POWER_OFF_CNF"
};
#endif /* defined(__BT_SINK_SRV_DEBUG_INFO__ ) && defined (__BT_SINK_SRV_TRACE__) */

static void bt_sink_srv_cm_write_scan_enable_mode(bt_gap_scan_mode_t mode);
static void bt_sink_srv_cm_handle_inquiry_scan_switch(void);
static void bt_sink_srv_cm_handle_power_on_cnf(void);
static void bt_sink_srv_cm_connect_internal(bt_sink_srv_profile_connection_action_t *action_param);
static void bt_sink_srv_cm_request_done(bt_sink_srv_status_t result, bt_sink_srv_cm_node_t *req);

static bool bt_sink_srv_cm_is_valid_addr(bt_bd_addr_t *addr)
{
    uint32_t i;
    bool result = false;

    if (NULL != addr) {
        for (i = 0; i < 6; i++) {
            if (0 != (*addr)[i]) {
                return true;
            }
        }
    }
    return result;
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
            if (!bt_sink_srv_cm_is_valid_addr(&request->action_param.bt_addr)) {
                result = BT_SINK_SRV_STATUS_FAIL;
            } else if (BT_SINK_SRV_CM_MAX_DEVICE_NUMBER == g_sink_srv_cm_context.connected_dev) {
                result = BT_SINK_SRV_STATUS_MAX_LINK;
            } else if (BT_SINK_SRV_CM_REQ_TYPE_NORMAL != request->type &&
                       NULL != bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, &request->action_param.bt_addr)) {
                result = BT_SINK_SRV_STATUS_LINK_EXIST;
            } else if (!(g_sink_srv_cm_context.flags & BT_SINK_SRV_FLAG_POWER_ON)) {
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
            bt_sink_srv_cm_request_run();
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
                    0 == bt_sink_srv_memcmp(&cur_node->parameters->action_param.bt_addr, &request->action_param.bt_addr, sizeof(bt_bd_addr_t))) {
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
        }
    } else {
        result = BT_SINK_SRV_STATUS_REQUEST_EXIST;
    }
    return result;
}

void bt_sink_srv_cm_request_cancel(bt_sink_srv_cm_req_type_t type)
{
    bt_sink_srv_cm_node_t *cur_node = g_sink_srv_cm_context.request_list.next;
    bt_sink_srv_cm_node_t *del_node = NULL;;

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
    uint32_t i;
    bt_sink_srv_cm_remote_device_t *device = NULL;
    bt_sink_srv_cm_remote_device_t *empty = NULL;

    for (i = 0, device = NULL, empty = NULL; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        if (bt_sink_srv_cm_is_valid_addr(&g_sink_srv_cm_context.devices[i].addr)) {
            if (0 == bt_sink_srv_memcmp((void *)addr, (void *)&g_sink_srv_cm_context.devices[i].addr, sizeof(bt_bd_addr_t))) {
                device = &g_sink_srv_cm_context.devices[i];
                break;
            }
        } else {
            empty = &g_sink_srv_cm_context.devices[i];
        }
    }

    if (NULL == device) {
        device = empty;
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
        device_p->state = BT_SINK_SRV_CM_STATE_DISCONNECTED;
    }
}

bt_sink_srv_cm_remote_device_t *bt_sink_srv_cm_find_device(bt_sink_srv_cm_find_by_type_t type, void *param)
{
    bt_sink_srv_cm_remote_device_t *dev_p;
    uint32_t i;

    for (i = 0, dev_p = NULL; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        if (BT_SINK_SRV_CM_FIND_BY_HANDLE == type && g_sink_srv_cm_context.devices[i].handle == *((bt_gap_connection_handle_t *)param)) {
            dev_p = &g_sink_srv_cm_context.devices[i];
            bt_sink_srv_report("handle:0x%x", dev_p->handle);
            break;
        } else if (BT_SINK_SRV_CM_FIND_BY_ADDR == type &&
                   bt_sink_srv_memcmp(param, &g_sink_srv_cm_context.devices[i].addr, sizeof(bt_bd_addr_t)) == 0) {
            dev_p = &g_sink_srv_cm_context.devices[i];
            break;
        }
    }
    bt_sink_srv_report("dev_p:%x", dev_p);
    return dev_p;
}

void bt_sink_srv_cm_connect_internal(bt_sink_srv_profile_connection_action_t *action_param)
{
    if (bt_sink_srv_cm_is_valid_addr(&action_param->bt_addr)) {
        bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_alloc_device(&action_param->bt_addr);
        bt_sink_srv_report("[Sink][CM] device_p:0x%x, address:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", device_p,
                           action_param->bt_addr[5], action_param->bt_addr[4], action_param->bt_addr[3], 
                           action_param->bt_addr[2], action_param->bt_addr[1], action_param->bt_addr[0]);
        if (NULL != device_p && BT_SINK_SRV_CM_STATE_DISCONNECTED == device_p->state) {
            BT_SINK_SRV_CM_STATE_CHANGE(device_p, BT_SINK_SRV_CM_STATE_CONNECTING);
            bt_sink_srv_action_send(BT_SINK_SRV_ACTION_PROFILE_CONNECT, (void *)action_param);
        } else {
            bt_sink_srv_report("[Sink][CM] Fatal Error");
        }
    } else {
        bt_sink_srv_report("[Sink][CM] Connect, invalid address");
    }
}

void bt_sink_srv_cm_connect(bt_sink_srv_profile_connection_action_t *action_param)  
{
    bt_sink_srv_cm_request_t request;

    bt_sink_srv_memset((void *)&request, 0, sizeof(bt_sink_srv_cm_request_t));
    request.type = BT_SINK_SRV_CM_REQ_TYPE_NORMAL;
    request.attampts = 1;
    bt_sink_srv_memcpy((void *)&request.action_param, (void *)action_param, sizeof(*action_param));
    bt_sink_srv_cm_request_new(&request);
}

void bt_sink_srv_cm_disconnect()
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
            
            BT_SINK_SRV_CM_STATE_CHANGE(device_p , BT_SINK_SRV_CM_STATE_DISCONNECTING);
            bt_sink_srv_memcpy(action_param.bt_addr, (void *)&device_p->addr, sizeof(bt_bd_addr_t));
            bt_sink_srv_report("[Sink][CM] Disconnect, device profile_type:0x%x", device_p->profile_type);
            if (device_p->profile_type & BT_SINK_SRV_TYPE_AWS) {
                action_param.profile_connection_mask = BT_SINK_SRV_PROFILE_CONNECTION_TYPE_AWS; 
            } else {
                action_param.profile_connection_mask = BT_SINK_SRV_PROFILE_CONNECTION_TYPE_HFP |
                                                       BT_SINK_SRV_PROFILE_CONNECTION_TYPE_A2DP_SINK |
                                                       BT_SINK_SRV_PROFILE_CONNECTION_TYPE_AVRCP;
            }
            bt_sink_srv_action_send(BT_SINK_SRV_ACTION_PROFILE_DISCONNECT, (void *)&action_param);
        }
    }
}

void bt_sink_srv_cm_notify_timeout_handler(void *params)
{
    bt_sink_srv_cm_remote_device_t *device_p = (bt_sink_srv_cm_remote_device_t *)params;

    bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_CM_PROFILE_NOTIFY);

    if (NULL != device_p) {
        bt_sink_srv_event_param_t *event;

        bt_sink_srv_report("[Sink][CM] profile notify timeout");
        bt_sink_srv_report("[Sink][CM] address:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x",
               device_p->addr[5], device_p->addr[4], device_p->addr[3], 
               device_p->addr[2], device_p->addr[1], device_p->addr[0]);
        bt_sink_srv_cm_dev_info_t *record = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_cm_dev_info_t));
        if (bt_sink_srv_db_find_record(&g_bt_sink_srv_cnmgr_db,
                                   (void *)&device_p->addr,
                                   (uint8_t *)record) == BT_SINK_SRV_STATUS_SUCCESS) {
            bt_sink_srv_report("[Sink][CM] update record, profile_type:0x%02x", device_p->profile_type);                       
            //update device info if the connected profile is aws
            if (device_p->profile_type & BT_SINK_SRV_TYPE_AWS) {
                record->dev_type = BT_SINK_SRV_CM_DEVICE_AWS;
            } else if (device_p->profile_type & BT_SINK_SRV_TYPE_HFP ||
                       device_p->profile_type & BT_SINK_SRV_TYPE_A2DP||
                       device_p->profile_type & BT_SINK_SRV_TYPE_AVRCP) {
                record->dev_type = BT_SINK_SRV_CM_DEVICE_SP;       
            }
            bt_sink_srv_db_add_record(&g_bt_sink_srv_cnmgr_db,
                                  (uint8_t *)record,
                                  (void *)&device_p->addr);                           
        }
        bt_sink_srv_memory_free(record);

        if (NULL != g_sink_srv_cm_context.request_pending &&
            0 == bt_sink_srv_memcmp(&device_p->addr, 
                     &g_sink_srv_cm_context.request_pending->parameters->action_param.bt_addr, 
                     sizeof(bt_bd_addr_t)) &&
            device_p->profile_connection_mask == g_sink_srv_cm_context.request_pending->parameters->action_param.profile_connection_mask) {
            bt_sink_srv_cm_request_done(BT_SINK_SRV_STATUS_SUCCESS, g_sink_srv_cm_context.request_pending);
        }

        event = bt_sink_srv_memory_alloc(sizeof(*event));
        bt_sink_srv_memcpy(&event->connection_info.bt_addr, &device_p->addr, sizeof(device_p->addr));
        event->connection_info.profile_type = device_p->profile_type;
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE, event);
        bt_sink_srv_memory_free(event);
    }
}

void bt_sink_srv_cm_aws_notify_handler(bt_sink_srv_cm_remote_device_t *device_p)
{
    if (NULL != device_p) {
        bt_sink_srv_event_param_t *event;
        bt_sink_srv_report("[Sink][CM] address:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x",
               device_p->addr[5], device_p->addr[4], device_p->addr[3],
               device_p->addr[2], device_p->addr[1], device_p->addr[0]);
        bt_sink_srv_cm_dev_info_t *record = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_cm_dev_info_t));
        if (bt_sink_srv_db_find_record(&g_bt_sink_srv_cnmgr_db,
                                   (void *)&device_p->addr,
                                   (uint8_t *)record) == BT_SINK_SRV_STATUS_SUCCESS) {
            bt_sink_srv_report("[Sink][CM] update record, profile_type:0x%02x", device_p->profile_type);
            //update device info if the connected profile is aws
                record->dev_type = BT_SINK_SRV_CM_DEVICE_AWS;
            bt_sink_srv_db_add_record(&g_bt_sink_srv_cnmgr_db,
                                  (uint8_t *)record,
                                  (void *)&device_p->addr);
        }
        bt_sink_srv_memory_free(record);

        if (NULL != g_sink_srv_cm_context.request_pending &&
            0 == bt_sink_srv_memcmp(&device_p->addr,
                     &g_sink_srv_cm_context.request_pending->parameters->action_param.bt_addr,
                     sizeof(bt_bd_addr_t)) &&
            device_p->profile_connection_mask == g_sink_srv_cm_context.request_pending->parameters->action_param.profile_connection_mask) {
            bt_sink_srv_cm_request_done(BT_SINK_SRV_STATUS_SUCCESS, g_sink_srv_cm_context.request_pending);
        }

        event = bt_sink_srv_memory_alloc(sizeof(*event));
        bt_sink_srv_memcpy(&event->connection_info.bt_addr, &device_p->addr, sizeof(device_p->addr));
        event->connection_info.profile_type = device_p->profile_type;
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE, event);
        bt_sink_srv_memory_free(event);
    }
}

void bt_sink_srv_cm_profile_status_notify(bt_bd_addr_t *addr, bt_sink_srv_profile_type_t profile, bool connected)
{
    if (NULL != addr) {
        bt_sink_srv_cm_remote_device_t *device_p =
            bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)addr);

        bt_sink_srv_report("[Sink] profile notify, device_p:0x%x, profile:0x%x, connected:%d", device_p, profile, connected);

        if (NULL != device_p) {
            bt_sink_srv_profile_type_t previous = device_p->profile_type;

            if (connected) {
                device_p->profile_type |= profile;
            } else {
                device_p->profile_type &= (~profile);
            }
            if (profile == BT_SINK_SRV_TYPE_HFP) {
                device_p->profile_connection_mask |= BT_SINK_SRV_PROFILE_CONNECTION_TYPE_HFP; 
            } else if (profile == BT_SINK_SRV_TYPE_A2DP) {
                device_p->profile_connection_mask |= BT_SINK_SRV_PROFILE_CONNECTION_TYPE_A2DP_SINK;
            } else if (profile == BT_SINK_SRV_TYPE_AWS) {
                device_p->profile_connection_mask |= BT_SINK_SRV_PROFILE_CONNECTION_TYPE_AWS;
                bt_gap_get_role(device_p->handle);
            }
            
            if (device_p->profile_type != previous) {
                if (device_p->profile_type == BT_SINK_SRV_PROFILE_ALL) {
                    bt_sink_srv_event_param_t *event;

                    //update device info if the connected profile is all
                    bt_sink_srv_report("[Sink][CM] All profile notify");
                    bt_sink_srv_report("[Sink][CM] address:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x",
                           device_p->addr[5], device_p->addr[4], device_p->addr[3], 
                           device_p->addr[2], device_p->addr[1], device_p->addr[0]);
                    bt_sink_srv_cm_dev_info_t *record = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_cm_dev_info_t));
                    if (bt_sink_srv_db_find_record(&g_bt_sink_srv_cnmgr_db,
                                               (void *)&device_p->addr,
                                               (uint8_t *)record) == BT_SINK_SRV_STATUS_SUCCESS) {
                        record->dev_type = BT_SINK_SRV_CM_DEVICE_SP;
                        bt_sink_srv_db_add_record(&g_bt_sink_srv_cnmgr_db,
                                              (uint8_t *)record,
                                              (void *)&device_p->addr);                           
                    }
                    bt_sink_srv_memory_free(record);

                    bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_CM_PROFILE_NOTIFY);

                    event = bt_sink_srv_memory_alloc(sizeof(*event));
                    bt_sink_srv_memcpy(&event->connection_info.bt_addr, addr, sizeof(*addr));
                    event->connection_info.profile_type = device_p->profile_type;
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE, event);
                    bt_sink_srv_report("[Sink] handle:0x%x", device_p->handle);
                    //sink srv a2dp will cover role switch
                    //bt_gap_get_role(device_p->handle);
                    bt_sink_srv_memory_free(event);

                    if (NULL != g_sink_srv_cm_context.request_pending &&
                        0 == bt_sink_srv_memcmp(addr, 
                                 &g_sink_srv_cm_context.request_pending->parameters->action_param.bt_addr, 
                                 sizeof(bt_bd_addr_t)) &&
                        device_p->profile_connection_mask == g_sink_srv_cm_context.request_pending->parameters->action_param.profile_connection_mask) {
                        bt_sink_srv_cm_request_done(BT_SINK_SRV_STATUS_SUCCESS, g_sink_srv_cm_context.request_pending);
                    }
                } else if (BT_SINK_SRV_TYPE_NONE == device_p->profile_type) {
                    bt_sink_srv_event_param_t *event;

                    bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_CM_PROFILE_NOTIFY);

                    event = bt_sink_srv_memory_alloc(sizeof(*event));
                    bt_sink_srv_memcpy(&event->connection_info.bt_addr, addr, sizeof(*addr));
                    event->connection_info.profile_type = device_p->profile_type;
                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE, event);
                    bt_sink_srv_memory_free(event);
                } else if (device_p->profile_type == BT_SINK_SRV_TYPE_AWS) {
                        bt_sink_srv_cm_aws_notify_handler(device_p);
                } else {
                    bt_sink_srv_timer_start(BT_SINK_SRV_TIMER_CM_PROFILE_NOTIFY,
                                            BT_SINK_SRV_CM_PROFILE_NOTIFY,
                                            bt_sink_srv_cm_notify_timeout_handler,
                                            device_p);
                }
            }
            if (BT_SINK_SRV_TYPE_NONE == device_p->profile_type && !connected) {
                // create link fail, like page time out, GAP will not notify, because link status is not changed.
                if (NULL != g_sink_srv_cm_context.request_pending
                        && 0 == bt_sink_srv_memcmp(addr,
                                                   &g_sink_srv_cm_context.request_pending->parameters->action_param.bt_addr,
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

void bt_sink_srv_cm_link_connected(const bt_bd_addr_t *dev_addr, bt_gap_connection_handle_t handle)
{
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_alloc_device((bt_bd_addr_t *)dev_addr);

    bt_sink_srv_report("[Sink][CM] CM connected gap handle:0x%x", handle);

    if (NULL == device_p) {
        bt_sink_srv_report("[Sink][CM] No connection available");
        bt_gap_disconnect(handle);
    } else {
        device_p->handle = handle;
        bt_sink_srv_memcpy(&g_sink_srv_cm_last_connected, dev_addr, sizeof(bt_bd_addr_t));
        g_sink_srv_cm_context.connected_dev++;
        if (g_sink_srv_cm_context.connected_dev == 1) {
            bt_gap_write_page_scan_activity(2048, 36);
        }
        BT_SINK_SRV_CM_STATE_CHANGE(device_p, BT_SINK_SRV_CM_STATE_CONNECTED);
        if (BT_SINK_SRV_CM_MAX_DEVICE_NUMBER == g_sink_srv_cm_context.connected_dev) {
            bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);
        } else {
#ifdef __BT_SINK_SRV_AWS_SUPPORT__
            bt_gap_get_role(device_p->handle);
#else
            bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
#endif
        }

        bt_sink_srv_state_set(BT_SINK_SRV_STATE_CONNECTED);
    }
}

void bt_sink_srv_cm_link_disconnected(const bt_bd_addr_t *dev_addr, bt_gap_connection_handle_t handle)
{
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_HANDLE, &handle);

    if (NULL == device_p) {
        bt_sink_srv_report("[Sink][CM] No connection available");
    } else {
        BT_SINK_SRV_CM_STATE_CHANGE(device_p, BT_SINK_SRV_CM_STATE_DISCONNECTED);
        bt_sink_srv_cm_free_device(device_p);
        g_sink_srv_cm_context.connected_dev--;
        if (g_sink_srv_cm_context.le_connected_dev == 0 &&
            g_sink_srv_cm_context.connected_dev == 0) {
            bt_gap_write_page_scan_activity(2048, 18);
        }

        if (BT_SINK_SRV_CM_MAX_DEVICE_NUMBER > g_sink_srv_cm_context.connected_dev) {
            bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
        }

        if (0 == g_sink_srv_cm_context.connected_dev) {
            bt_sink_srv_state_reset(BT_SINK_SRV_STATE_CONNECTED);
        }
    }
}

uint32_t bt_sink_srv_cm_get_connected_device(uint16_t uuid, bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER])
{
    uint32_t i, count;

    if (NULL != addr_list) {
        bt_sink_srv_memset((void *)addr_list, 0, sizeof(bt_bd_addr_t) * BT_SINK_SRV_CM_MAX_DEVICE_NUMBER);
    }

    for (i = 0, count = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
        if (BT_SINK_SRV_CM_STATE_CONNECTED == g_sink_srv_cm_context.devices[i].state) {
            if (0 == uuid) {
                if (NULL != addr_list) {
                    bt_sink_srv_memcpy(&addr_list[count], &g_sink_srv_cm_context.devices[i].addr, sizeof(bt_bd_addr_t));
                }
                bt_sink_srv_report("[Sink][CM] connected gap handle:0x%x", g_sink_srv_cm_context.devices[i].handle);
                count++;
            } else {
                uint32_t j;

                for (j = 0; j < BT_SINK_SRV_CM_MAX_PROFILE_NUMBER; j++) {
                    if (NULL != g_sink_srv_cm_context.devices[i].profile[j]
                            && (g_sink_srv_cm_context.devices[i].profile[j]->uuid == uuid)
                            && (g_sink_srv_cm_context.devices[i].profile[j]->state == BT_SINK_SRV_PROFILE_STATE_CONNECTED
                                || g_sink_srv_cm_context.devices[i].profile[j]->state == BT_SINK_SRV_PROFILE_STATE_AUDIO_ON)) {
                        if (NULL != addr_list) {
                            bt_sink_srv_memcpy(&addr_list[count], &g_sink_srv_cm_context.devices[i].addr, sizeof(bt_bd_addr_t));
                        }
                        count++;
                    }
                }
            }
        }
    }
    bt_sink_srv_report("[Sink][CM] connected count:%d", count);
    return count;
}

bool bt_sink_srv_cm_get_profile_data(bt_bd_addr_t *bt_addr, uint16_t uuid, void *data_p)
{
    bool result = false;
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, bt_addr);
    bt_sink_srv_cm_dev_info_t *record = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_cm_dev_info_t));

    if (NULL != device_p && NULL != data_p &&
            bt_sink_srv_db_find_record(&g_bt_sink_srv_cnmgr_db,
                                       (void *)&device_p->addr,
                                       (uint8_t *)record) == BT_SINK_SRV_STATUS_SUCCESS) {
        switch (uuid) {
#ifdef BT_SINK_SRV_HFP_STORAGE_SIZE
            case BT_SINK_SRV_UUID_HF:
                bt_sink_srv_memcpy(data_p, record->hfp_info, BT_SINK_SRV_HFP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_A2DP_STORAGE_SIZE
            case BT_SINK_SRV_UUID_A2DP:
                bt_sink_srv_memcpy(data_p, record->a2dp_info, BT_SINK_SRV_A2DP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_AVRCP_STORAGE_SIZE
            case BT_SINK_SRV_UUID_AVRCP:
                bt_sink_srv_memcpy(data_p, record->avrcp_info, BT_SINK_SRV_AVRCP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_PBAP_STORAGE_SIZE
            case BT_SINK_SRV_UUID_PBAPC:
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


bool bt_sink_srv_cm_set_profile_data(bt_bd_addr_t *bt_addr, uint16_t uuid, void *data_p)
{
    bool result = false;
    bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, bt_addr);
    bt_sink_srv_cm_dev_info_t *record = bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_cm_dev_info_t));

    if (NULL != device_p && NULL != data_p &&
            bt_sink_srv_db_find_record(&g_bt_sink_srv_cnmgr_db,
                                       (void *)&device_p->addr,
                                       (uint8_t *)record) == BT_SINK_SRV_STATUS_SUCCESS) {
        switch (uuid) {
#ifdef BT_SINK_SRV_HFP_STORAGE_SIZE
            case BT_SINK_SRV_UUID_HF:
                bt_sink_srv_memcpy(record->hfp_info, data_p, BT_SINK_SRV_HFP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_A2DP_STORAGE_SIZE
            case BT_SINK_SRV_UUID_A2DP:
                bt_sink_srv_memcpy(record->a2dp_info, data_p, BT_SINK_SRV_A2DP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_AVRCP_STORAGE_SIZE
            case BT_SINK_SRV_UUID_AVRCP:
                bt_sink_srv_memcpy(record->avrcp_info, data_p, BT_SINK_SRV_AVRCP_STORAGE_SIZE);
                result = true;
                break;
#endif
#ifdef BT_SINK_SRV_PBAP_STORAGE_SIZE
            case BT_SINK_SRV_UUID_PBAPC:
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

void *bt_sink_srv_cm_find_profile_by_address(bt_bd_addr_t *bt_addr, uint16_t uuid)
{
    uint8_t i;
    uint8_t dev_id = BT_SINK_SRV_CM_DEVICE_ID_UNAVAILABLE;
    bt_sink_srv_profile_t *profile = NULL;

    // search device
    for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        if (bt_sink_srv_cm_is_valid_addr(&g_sink_srv_cm_context.devices[i].addr)
                && 0 == bt_sink_srv_memcmp((uint8_t *)&g_sink_srv_cm_context.devices[i].addr, (uint8_t *)bt_addr, sizeof(bt_bd_addr_t))) {
            dev_id = i;
            break;
        }
    }
    if (dev_id >= BT_SINK_SRV_CM_MAX_DEVICE_NUMBER) {
        bt_sink_srv_report("[Sink][CM] Can not find device\n");
        return NULL;
    }

    // search profile
    for (i = 0; i < BT_SINK_SRV_CM_MAX_PROFILE_NUMBER; ++i) {
        if (g_sink_srv_cm_context.devices[dev_id].profile[i]) {
            profile = g_sink_srv_cm_context.devices[dev_id].profile[i];
            if (profile->uuid == uuid) {
                return g_sink_srv_cm_context.devices[dev_id].profile[i];
            }
        }
    }
    return NULL;
}

void *bt_sink_srv_cm_find_profile_by_hanlde(uint32_t handle)
{
    if (0 != handle) {
        uint8_t i, j;

        // search connected device
        for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
            if (bt_sink_srv_cm_is_valid_addr(&g_sink_srv_cm_context.devices[i].addr)) {
                // search profile
                for (j = 0; j < BT_SINK_SRV_CM_MAX_PROFILE_NUMBER; ++j) {
                    if (NULL != g_sink_srv_cm_context.devices[i].profile[j]
                            && handle == g_sink_srv_cm_context.devices[i].profile[j]->handle) {
                        return g_sink_srv_cm_context.devices[i].profile[j];
                    }
                }
            }
        }
    }
    return NULL;
}

bt_bd_addr_t *bt_sink_srv_cm_find_address_by_hanlde(uint32_t handle)
{
    if (0 != handle) {
        uint8_t i, j;

        // search connected device
        for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
            if (bt_sink_srv_cm_is_valid_addr(&g_sink_srv_cm_context.devices[i].addr)) {
                // search profile
                for (j = 0; j < BT_SINK_SRV_CM_MAX_PROFILE_NUMBER; ++j) {
                    if (NULL != g_sink_srv_cm_context.devices[i].profile[j]
                            && handle == g_sink_srv_cm_context.devices[i].profile[j]->handle) {
                        return &g_sink_srv_cm_context.devices[i].addr;
                    }
                }
            }
        }
    }
    return NULL;
}

static void bt_sink_srv_cm_discoverable_timeout(void *parameter)
{
    bt_sink_srv_cm_handle_inquiry_scan_switch();
}

static void bt_sink_srv_cm_power_on_callback(bt_sink_srv_status_t result, void *parameters)
{
    if (g_sink_srv_cm_context.connected_dev >= BT_SINK_SRV_CM_MAX_DEVICE_NUMBER) {
        bt_sink_srv_cm_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_POWER_ON);
    }
}

void bt_sink_srv_cm_power_on_reconnection(void)
{
    uint32_t i = g_bt_sink_srv_cnmgr_db.used;
    bt_sink_srv_cm_request_t request;

    while (i-- > 0 && g_sink_srv_cm_dev_info[i].dev_type != BT_SINK_SRV_CM_DEVICE_NONE) {
        bt_sink_srv_memset(&request, 0, sizeof(bt_sink_srv_cm_request_t));
        request.type = BT_SINK_SRV_CM_REQ_TYPE_POWER_ON;
        request.attampts = 1;
        request.delay_time = BT_SINK_SRV_CM_POWER_ON_RECONNECT;
        request.callback = bt_sink_srv_cm_power_on_callback;
        bt_sink_srv_memcpy(&request.action_param.bt_addr, &g_sink_srv_cm_dev_info[i].address, sizeof(bt_bd_addr_t));
        bt_sink_srv_report("[Sink][CM] reconnection address:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x",
                   g_sink_srv_cm_dev_info[i].address[5], g_sink_srv_cm_dev_info[i].address[4], 
                   g_sink_srv_cm_dev_info[i].address[3], g_sink_srv_cm_dev_info[i].address[2], 
                   g_sink_srv_cm_dev_info[i].address[1], g_sink_srv_cm_dev_info[i].address[0]);
        bt_sink_srv_report("[Sink][CM] reconnection dev type:%d", g_sink_srv_cm_dev_info[i].dev_type);
        if (g_sink_srv_cm_dev_info[i].dev_type == BT_SINK_SRV_CM_DEVICE_AWS) {
            request.action_param.profile_connection_mask = BT_SINK_SRV_PROFILE_CONNECTION_TYPE_AWS;
        } else if (g_sink_srv_cm_dev_info[i].dev_type == BT_SINK_SRV_CM_DEVICE_SP) {
            request.action_param.profile_connection_mask = BT_SINK_SRV_PROFILE_CONNECTION_TYPE_HFP |
                                                           BT_SINK_SRV_PROFILE_CONNECTION_TYPE_A2DP_SINK;
        }
        bt_sink_srv_cm_request_new(&request);
    }
}

void bt_sink_srv_cm_init(void)
{
    uint8_t i;

    bt_sink_srv_memset((void *)&g_sink_srv_cm_context, 0, sizeof(bt_sink_srv_cm_context_t));

    g_sink_srv_cm_context.request_list.next = &g_sink_srv_cm_context.request_list;
    g_sink_srv_cm_context.request_list.prev = &g_sink_srv_cm_context.request_list;

    // register profiles
    for (i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; ++i) {
        g_sink_srv_cm_context.devices[i].profile[BT_SINK_SRV_CM_PROFILE_HF] = &g_sink_srv_hf[i];
        g_sink_srv_cm_context.devices[i].profile[BT_SINK_SRV_CM_PROFILE_A2DP] = &sink_a2dp[i].profile;
        g_sink_srv_cm_context.devices[i].profile[BT_SINK_SRV_CM_PROFILE_AVRCP] = &sink_avrcp[i].profile;
        g_sink_srv_cm_context.devices[i].profile[BT_SINK_SRV_CM_PROFILE_PBAPC] = &g_sink_srv_pbapc[i];
        #ifdef __BT_AWS_SUPPORT__
        g_sink_srv_cm_context.devices[i].profile[BT_SINK_SRV_CM_PROFILE_AWS] = &sink_aws[i].profile;
        #endif
    }

    bt_sink_srv_music_init();
    // init profiles
    bt_sink_srv_action_send(BT_SINK_SRV_ACTION_PROFILE_INIT, NULL);

    if (BT_SINK_SRV_STATUS_SUCCESS == bt_sink_srv_db_open(&g_bt_sink_srv_cnmgr_db)) {
        g_bt_sink_srv_cnmgr_db.used = bt_device_manager_get_paired_number();
        bt_sink_srv_report("[Sink][CM] Get paired device num:%d", g_bt_sink_srv_cnmgr_db.used);
        bt_bd_addr_t tempaddr = {0};
         for (uint8_t i = 0; i < g_bt_sink_srv_cnmgr_db.count; i++) {
            if (bt_sink_srv_memcmp(&(g_sink_srv_cm_dev_info[i].address), &tempaddr, sizeof(bt_bd_addr_t)) != 0 
                  && !bt_device_manager_is_paired(g_sink_srv_cm_dev_info[i].address)) {
                    bt_sink_srv_report("[Sink][CM] delete index:%d,addr:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", i,
                    g_sink_srv_cm_dev_info[i].address[5], g_sink_srv_cm_dev_info[i].address[4], g_sink_srv_cm_dev_info[i].address[3], 
                    g_sink_srv_cm_dev_info[i].address[2], g_sink_srv_cm_dev_info[i].address[1], g_sink_srv_cm_dev_info[i].address[0]);
                bt_sink_srv_db_delete_record(&g_bt_sink_srv_cnmgr_db, (void *)g_sink_srv_cm_dev_info[i].address);
            }
         }
    } else {
        bt_sink_srv_report("[Sink][CM] Opend database failed");
    }
}

void bt_sink_srv_cm_deinit(void)
{
    bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_CM_REQUEST_DELAY);
    bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_CM_DISCOVERABLE);
    bt_sink_srv_cm_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_NONE);
    g_sink_srv_cm_context.request_pending = NULL;
    bt_sink_srv_action_send(BT_SINK_SRV_ACTION_PROFILE_DEINIT, NULL);
    bt_sink_srv_db_close(&g_bt_sink_srv_cnmgr_db);
}

#ifdef __BT_SINK_SRV_AWS_SUPPORT__
void bt_sink_srv_cm_set_eir()
{
    bt_sink_srv_report("[Sink][CM] set eir");

    uint8_t eir_data[18] = {0x11,0x06,0x1D,0x23,0xBB,0x1B,0x00,0x00,0x10,0x00,0x30,0x00,0x50,0x80,0x5F,0x9B,0x34,0xFA};
    bt_gap_set_extended_inquiry_response(eir_data, 18);
}

void bt_sink_srv_cm_inquiry(void)
{
    bt_gap_inquiry(10, 20);
}
#endif

static void bt_sink_srv_cm_handle_power_on_cnf(void)
{
    const bt_sink_srv_features_config_t *config_p = bt_sink_srv_get_features_config();

    bt_sink_srv_cm_init();

    g_sink_srv_cm_context.flags |= BT_SINK_SRV_FLAG_POWER_ON;

    if (config_p->features & BT_SINK_SRV_FEATURE_DISCOVERABLE_ONCE) {
        bt_sink_srv_cm_handle_inquiry_scan_switch();
    } else {
        bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
    }

    if (config_p->features & BT_SINK_SRV_FEATURE_POWER_ON_RECONNECT) {
        bt_sink_srv_cm_power_on_reconnection();
    }

#ifdef __BT_SINK_SRV_AWS_SUPPORT__
    // Only main earbud can inquiry and connect AWS
    if (config_p->features & BT_SINK_SRV_FEATURE_POWER_ON_INQUIRY) {
        bt_sink_srv_cm_inquiry();
    }   
    bt_sink_srv_cm_set_eir();
#endif
    bt_sink_srv_state_set(BT_SINK_SRV_STATE_POWER_ON);
}

static void bt_sink_srv_cm_handle_power_off_cnf(void)
{
    bt_sink_srv_cm_deinit();

    g_sink_srv_cm_context.flags &= (~BT_SINK_SRV_FLAG_POWER_ON);
    bt_sink_srv_state_reset(BT_SINK_SRV_STATE_POWER_ON);
}

void bt_sink_srv_cm_write_scan_enable_mode(bt_gap_scan_mode_t mode)
{
#ifndef MTK_BT_COMMAND_ENABLE

    if (BT_GAP_SCAN_MODE_NOT_ACCESSIBLE == mode) {
        g_sink_srv_cm_context.flags &= ~BT_SINK_SRV_FLAG_INQRUIRY_SCAN;
    } else if (BT_GAP_SCAN_MODE_CONNECTABLE_ONLY == mode) {
        g_sink_srv_cm_context.flags &= ~BT_SINK_SRV_FLAG_INQRUIRY_SCAN;
    } else if (BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE == mode) {
        g_sink_srv_cm_context.flags |= BT_SINK_SRV_FLAG_INQRUIRY_SCAN;
    } else {
        g_sink_srv_cm_context.flags |= BT_SINK_SRV_FLAG_INQRUIRY_SCAN;
    }

    if (g_sink_srv_cm_context.flags & BT_SINK_SRV_FLAG_INQRUIRY_SCAN) {
        bt_sink_srv_timer_start(BT_SINK_SRV_TIMER_CM_DISCOVERABLE, BT_SINK_SRV_CM_DISCOVERABLE_DURATION,
                                bt_sink_srv_cm_discoverable_timeout, NULL);
    } else {
        if (bt_sink_srv_timer_is_exist(BT_SINK_SRV_TIMER_CM_DISCOVERABLE)) {
            bt_sink_srv_timer_stop(BT_SINK_SRV_TIMER_CM_DISCOVERABLE);
        }
    }

    bt_gap_set_scan_mode(mode);

    bt_sink_srv_report("[Sink][CM] Scan enable:%x, status:0x%x", mode);
#else

    if (BT_GAP_SCAN_MODE_NOT_ACCESSIBLE == mode) {
        g_sink_srv_cm_context.flags &= ~BT_SINK_SRV_FLAG_INQRUIRY_SCAN;
    } else if (BT_GAP_SCAN_MODE_CONNECTABLE_ONLY == mode) {
        g_sink_srv_cm_context.flags &= ~BT_SINK_SRV_FLAG_INQRUIRY_SCAN;
    } else if (BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE == mode) {
        g_sink_srv_cm_context.flags |= BT_SINK_SRV_FLAG_INQRUIRY_SCAN;
    } else {
        g_sink_srv_cm_context.flags |= BT_SINK_SRV_FLAG_INQRUIRY_SCAN;
    }

    bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);

    bt_sink_srv_report("[Sink][CM] origin:%x, status:0x%x", mode,BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);
#endif /*MTK_BT_COMMAND_ENABLE*/
}

static void bt_sink_srv_cm_handle_inquiry_scan_switch(void)
{
    if (g_sink_srv_cm_context.flags & BT_SINK_SRV_FLAG_INQRUIRY_SCAN) {
        if (BT_SINK_SRV_CM_MAX_DEVICE_NUMBER > g_sink_srv_cm_context.connected_dev) {
            bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
        } else {
            bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);
        }
    } else {
        if (BT_SINK_SRV_CM_MAX_DEVICE_NUMBER > g_sink_srv_cm_context.connected_dev) {
            bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);
        }
    }
}

static void bt_sink_srv_cm_handle_link_disconnected(bt_status_t status, bt_gap_link_status_updated_ind_t *param)
{
    const bt_sink_srv_features_config_t *config_p = bt_sink_srv_get_features_config();

    bt_sink_srv_report("[Sink][CM] detach single link, reason: 0x%x, handle = 0x%x", status, param->handle);
    bt_sink_srv_cm_link_disconnected(param->address, param->handle);

    if (NULL != g_sink_srv_cm_context.request_pending
            && 0 == bt_sink_srv_memcmp(param->address, &g_sink_srv_cm_context.request_pending->parameters->action_param.bt_addr, sizeof(bt_bd_addr_t))) {
        bt_sink_srv_cm_request_done(BT_SINK_SRV_STATUS_FAIL, g_sink_srv_cm_context.request_pending);
    }

    if (config_p->features & BT_SINK_SRV_FEATURE_DISABLE_LINK_LOST_RECONNECT) {
        return;
    } 

    if (BT_SINK_SRV_CM_REASON_CONNECTION_TIMEOUT == status) {
#ifndef MTK_BT_COMMAND_ENABLE
        bt_sink_srv_cm_request_t link_lost;
#ifdef __BT_SINK_SRV_AWS_SUPPORT__
        bt_sink_srv_cm_device_type_t cm_dev_type = BT_SINK_SRV_CM_DEVICE_NONE;
        uint32_t i = g_bt_sink_srv_cnmgr_db.used;
#endif

        bt_sink_srv_memset((void *)&link_lost, 0, sizeof(bt_sink_srv_cm_request_t));
        link_lost.type = BT_SINK_SRV_CM_REQ_TYPE_LINK_LOST;
        link_lost.attampts = 3;
        link_lost.delay_time = BT_SINK_SRV_LINK_LOST_RECONNECT_DELAY;
        bt_sink_srv_memcpy((void *)&link_lost.action_param.bt_addr, (const void *)param->address, sizeof(bt_bd_addr_t));
#ifdef __BT_SINK_SRV_AWS_SUPPORT__

        while (i-- > 0  && g_sink_srv_cm_dev_info[i].dev_type != BT_SINK_SRV_CM_DEVICE_NONE) {
            if (bt_sink_srv_memcmp(param->address, &g_sink_srv_cm_dev_info[i].address, sizeof(bt_bd_addr_t)) == 0) {
                cm_dev_type = g_sink_srv_cm_dev_info[i].dev_type;
                break;
            }
        }
        if (cm_dev_type == BT_SINK_SRV_CM_DEVICE_AWS) {
            link_lost.action_param.profile_connection_mask = BT_SINK_SRV_PROFILE_CONNECTION_TYPE_AWS;
        } else if (cm_dev_type == BT_SINK_SRV_CM_DEVICE_SP) {
#endif
            link_lost.action_param.profile_connection_mask = BT_SINK_SRV_PROFILE_CONNECTION_TYPE_HFP |
                                                             BT_SINK_SRV_PROFILE_CONNECTION_TYPE_A2DP_SINK;
#ifdef __BT_SINK_SRV_AWS_SUPPORT__
        }
#endif
        bt_sink_srv_cm_request_new(&link_lost);
#else /*MTK_BT_COMMAND_ENABLE*/
        bt_sink_srv_cm_profile_state_change_notify((bt_bd_addr_t*)param->address, BT_SINK_SRV_CM_PROFILE_END,  BT_SINK_SRV_CM_STATE_DISCONNECTED);
#endif /*MTK_BT_COMMAND_ENABLE*/
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
                && 0 == bt_sink_srv_memcmp(param->address, &g_sink_srv_cm_context.request_pending->parameters->action_param.bt_addr, sizeof(bt_bd_addr_t))
                && g_sink_srv_cm_context.request_pending->parameters->type == BT_SINK_SRV_CM_REQ_TYPE_LINK_LOST) {
            bt_sink_srv_cm_request_done(BT_SINK_SRV_STATUS_SUCCESS, g_sink_srv_cm_context.request_pending);
        }

    }
}

#ifdef MTK_BT_COMMAND_ENABLE
void bt_sink_srv_cm_profile_state_change_notify(bt_bd_addr_t* address, bt_sink_srv_cm_profile_type_t profile, bt_sink_srv_cm_state_t state)
{
    bt_sink_srv_report("[Sink][CM] profile:%d, state:%d", profile, state);

    bt_sink_srv_cm_profile_connection_update_t * params =  bt_sink_srv_memory_alloc(sizeof(*params));
    bt_sink_srv_memcpy((void *)params->address, (const void *)address, sizeof(bt_bd_addr_t));
    params->profile= profile;
    params->state= state;
    bt_sink_srv_custom_event_callback(BT_SINK_SRV_EVENT_PROFILE_CONNECTION_UPDATE, (void*)params, sizeof(*params));
    bt_sink_srv_memory_free(params);
}
#endif /*MTK_BT_COMMAND_ENABLE*/

bt_bd_addr_t *bt_sink_srv_cm_last_connected_device(void)
{
    return &g_sink_srv_cm_last_connected;
}

void bt_sink_srv_cm_set_role(bt_bd_addr_t *address_p, bt_role_t role)
{
    if (NULL != address_p) {
        bt_sink_srv_cm_remote_device_t *remote_device_p =
            bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)address_p);

        if (NULL != remote_device_p) {
            bt_gap_set_role(remote_device_p->handle, role);
        }
    }
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
#ifdef __BT_SINK_SRV_AWS_SUPPORT__        
        case BT_SINK_SRV_ACTION_INQUIRY:
            bt_sink_srv_cm_inquiry();
            break;
#endif

        case BT_SINK_SRV_ACTION_CONNECT: {
            bt_sink_srv_profile_connection_action_t connect_action = {{0}};
            bt_sink_srv_cm_remote_device_t *device_p = NULL;
            if (NULL != param) {
                bt_sink_srv_memcpy((void *)&connect_action.bt_addr, param, sizeof(bt_bd_addr_t));
            } else {
                bt_sink_srv_memcpy((void *)&connect_action.bt_addr, (const void*)&g_sink_srv_cm_last_connected, sizeof(bt_bd_addr_t));
            }
            device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)connect_action.bt_addr);
            if (device_p != NULL && device_p->profile_type & BT_SINK_SRV_TYPE_AWS) {
                connect_action.profile_connection_mask = BT_SINK_SRV_PROFILE_CONNECTION_TYPE_AWS;
            } else {
                connect_action.profile_connection_mask = BT_SINK_SRV_PROFILE_CONNECTION_TYPE_HFP |
                                                         BT_SINK_SRV_PROFILE_CONNECTION_TYPE_A2DP_SINK;
            }
            bt_sink_srv_cm_connect(&connect_action);
        }
        break;

        case BT_SINK_SRV_ACTION_DISCONNECT:
            bt_sink_srv_cm_disconnect();
            break;

        case BT_SINK_SRV_ACTION_DISCOVERABLE:
            bt_sink_srv_cm_request_cancel(BT_SINK_SRV_CM_REQ_TYPE_POWER_ON);
            bt_sink_srv_cm_handle_inquiry_scan_switch();
            break;

        case BT_SINK_SRV_ACTION_RESET_TRUSTED_LIST:
        {
            bt_sink_srv_db_delete_all_records(&g_bt_sink_srv_cnmgr_db);
            bt_device_manager_unpair_all();
        }
            break;

        default:
            break;
    }

    return BT_SINK_SRV_STATUS_SUCCESS;
}

#include "hal_wdt.h"
static void bt_sink_srv_cm_handle_bt_panic(void)
{
    hal_wdt_software_reset();
    return;
}

bt_status_t bt_sink_srv_cm_system_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
#if defined(__BT_SINK_SRV_DEBUG_INFO__ ) && defined (__BT_SINK_SRV_TRACE__)
    if (msg >= BT_POWER_ON_CNF && msg <= BT_POWER_OFF_CNF) {
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

bt_status_t bt_sink_srv_cm_gap_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    bt_sink_srv_report("[Sink][CM] bt_sink_srv_cm_gap_callback, msg:0x%x", msg);

#if defined( __BT_SINK_SRV_DEBUG_INFO__) && defined (__BT_SINK_SRV_TRACE__)
    if (msg >= BT_GAP_LINK_STATUS_UPDATED_IND && msg <= BT_GAP_GET_ROLE_CNF) {
        bt_sink_srv_report("[Sink][CM] SDK msg:%s", g_gap_event_string[msg - BT_GAP_LINK_STATUS_UPDATED_IND]);
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
            bt_sink_srv_report("[Sink][CM] scan result:0x%x", status);
            if (g_sink_srv_cm_context.flags & BT_SINK_SRV_FLAG_INQRUIRY_SCAN) {
                bt_sink_srv_state_set(BT_SINK_SRV_STATE_DISCOVERABLE);
            } else {
                bt_sink_srv_state_reset(BT_SINK_SRV_STATE_DISCOVERABLE);
            }
        }
        break;

#ifdef __BT_SINK_SRV_AWS_SUPPORT__        
        case BT_GAP_GET_ROLE_CNF: {
            bt_gap_get_role_cnf_t *get_role = (bt_gap_get_role_cnf_t *)buffer;
            bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_HANDLE, &get_role->handle);
            // event callback to notify app the gap role
            bt_sink_srv_event_param_t *event;
            event = bt_sink_srv_memory_alloc(sizeof(*event));
            event->role_info.handle = get_role->handle;
            event->role_info.role = get_role->local_role;
            bt_sink_srv_memcpy(&event->role_info.bt_addr, (void *)&device_p->addr, sizeof(bt_bd_addr_t));
            bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_ROLE_IND, (void *)event);
            bt_sink_srv_memory_free(event);

            if (BT_SINK_SRV_CM_MAX_DEVICE_NUMBER == g_sink_srv_cm_context.connected_dev) {
                bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);
            } else {
                if (BT_ROLE_MASTER == get_role->local_role) {
                    bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);
                } else if (BT_ROLE_SLAVE == get_role->local_role) {
                    bt_sink_srv_cm_write_scan_enable_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
                }
            }
        }
        break;

        case BT_GAP_INQUIRY_IND: {
            uint8_t count, eir_data_len, eir_data_type, eir_data_idx;
            bt_gap_inquiry_ind_t *result_ind = (bt_gap_inquiry_ind_t *)buffer;
            count = 0;
            uint8_t buff[16] = {0};
            uint8_t aws_uuid[] = {0x1D,0x23,0xBB,0x1B,0x00,0x00,0x10,0x00,0x30,0x00,0x50,0x80,0x5F,0x9B,0x34,0xFA};
            bt_sink_srv_profile_connection_action_t action_param = {{0}}; 
            bt_sink_srv_report("[Sink][CM] enter gap_qury_ind");
            if (result_ind->eir != NULL) {
                while (count < 240) { // 240 is the maximum length of EIR
                    eir_data_len = result_ind->eir[count];
                    if (eir_data_len >= 0x1F) {
                        break;
                    }
                    eir_data_type = result_ind->eir[count+1];
                    count+=2;
                    if (eir_data_type == BT_GAP_LE_AD_TYPE_128_BIT_UUID_PART) {
                        for (eir_data_idx=0; eir_data_idx<(eir_data_len-1); eir_data_idx++, count++) {
                            buff[eir_data_idx] = result_ind->eir[count];
                        }
                        bt_sink_srv_report("[Sink][CM] eir:%x", buff);
                        if (bt_sink_srv_memcmp( (const void *)buff, (const void *)aws_uuid, strlen((char *)aws_uuid)) == 0) {
                            bt_gap_cancel_inquiry();
                            bt_sink_srv_memcpy(action_param.bt_addr, result_ind->address, sizeof(bt_bd_addr_t));
                            action_param.profile_connection_mask = BT_SINK_SRV_PROFILE_CONNECTION_TYPE_AWS; 
                            bt_sink_srv_cm_connect(&action_param);
                        }
                    }
                }
            }
        }
        break;
#endif

        case BT_GAP_LE_CONNECT_IND:
            if (status == BT_STATUS_SUCCESS) {
                g_sink_srv_cm_context.le_connected_dev++;
                if (g_sink_srv_cm_context.le_connected_dev == 1) {
                    bt_gap_write_page_scan_activity(2048, 36);
                }
            }
            break;

        case BT_GAP_LE_DISCONNECT_IND:
            g_sink_srv_cm_context.le_connected_dev--;
            if (g_sink_srv_cm_context.le_connected_dev == 0 &&
                g_sink_srv_cm_context.connected_dev == 0) {
                bt_gap_write_page_scan_activity(2048, 18);
            }
            break;
            
        default:
            bt_sink_srv_report("[Sink][CM] Unexcepted msg:%x", msg);
            break;
    }
    return result;
}

const bt_gap_config_t *bt_sink_srv_gap_config(void)
{
#ifdef __QA_TEST__
    uint32_t size = 12;
    uint8_t buffer[18] = {0};
    uint8_t name[30] = "BT_Head_";
    nvdm_status_t status;

    //Change BT local name to BT_Head_xxx for QA test (xxx is BT addr)
    status = nvdm_read_data_item("BT", "address", buffer, &size);
    if (NVDM_STATUS_OK == status) {
        bt_sink_srv_memcpy(name + 8, buffer, (uint32_t)strlen((char *)buffer));
        bt_sink_srv_memcpy(g_bt_sink_srv_gap_config.device_name, name, strlen((char *)name));
        bt_sink_srv_report("[Sink][CM] device name:%s", g_bt_sink_srv_gap_config.device_name);    
    }
#endif

    return &g_bt_sink_srv_gap_config;
}

uint8_t bt_sink_srv_get_connected_device_number(void)
{
    return g_sink_srv_cm_context.connected_dev;
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
            if (bt_sink_srv_db_find_record(&g_bt_sink_srv_cnmgr_db,(void *)address,
                (uint8_t *)record) != BT_SINK_SRV_STATUS_SUCCESS) {
                bt_sink_srv_memset((void *)record, 0, sizeof(bt_sink_srv_cm_dev_info_t));
                bt_sink_srv_memcpy((void *)&record->address, (void *)address, sizeof(bt_bd_addr_t));
                bt_sink_srv_db_add_record(&g_bt_sink_srv_cnmgr_db, (uint8_t *)record, (void *)address);
            }
            bt_sink_srv_memory_free(record);
        }
        break;
                
        case BT_DEVICE_MANAGER_PAIRED_UNPAIR_ALL:
        case BT_DEVICE_MANAGER_PAIRED_UNPAIR:
        default:
            bt_sink_srv_report("[Sink][CM] un handle event: 0x%x", event);
        break;   
    }
    return;
}

/*It become dummmy funciton after SDK 4.2 release becuase it is unused when BT device manager module is enable.*/
void bt_sink_srv_cm_reply_link_key(bt_gap_link_key_notification_ind_t *key_information)
{
    return;
}


