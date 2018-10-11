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
 
#include "bt_gattc_discovery.h"
#include "bt_callback_manager.h"
#include "syslog.h"


log_create_module(BT_GATTC, PRINT_LEVEL_INFO);

/**************************Macro and Global*************************/
#define SRV_DISC_START_HANDLE  0x0001   /**< The start handle value used during service discovery. */
#define SRV_DISC_END_HANDLE    0xFFFF   /**< The start handle value used during service discovery. */
#define BT_GATTC_DISCOVERY_MAX_USERS BT_GATTC_DISCOVERY_MAX_SRVS  /**< The maximum number of users/registrations allowed by this module. */


static bt_gattc_registered_handlers g_registered_handlers[BT_GATTC_DISCOVERY_MAX_USERS];
static bt_gattc_pending_user_events g_pending_user_evts[BT_GATTC_DISCOVERY_MAX_USERS];
static uint32_t g_pending_usr_evt_index;    /**< The index to the pending user event array, pointing to the last added pending user event. */
static uint32_t g_num_of_handlers_reg;      /**< The number of handlers registered with the GATTC Discovery module. */
static uint32_t g_num_of_discovered;        /**< The total number of service discoveries (successful or unsuccessful) made since initialization. */
static bool     g_initialized = false;      /**< This variable Indicates if the module is initialized or not. */
static bt_gattc_discovery_t g_gattc_discovery = {0};

/************************************************
*   Utilities
*************************************************/
//static bt_status_t bt_gattc_discovery_deinit(void);
static void bt_gattc_discovery_input_buffer_reset(bt_gattc_discovery_service_t *buffer);
static bt_status_t bt_gattc_discovery_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff);
static bt_status_t bt_gattc_discovery_start_discover_service(uint16_t conn_handle, ble_uuid_t *srv_uuid);
static void bt_gattc_discovery_primary_service_cnf(bt_status_t status, void *buff);
static void bt_gattc_discovery_parse_primary_service(bt_gattc_find_by_type_value_rsp_t *rsp, bt_gattc_discovery_service_t *data);
static bt_status_t bt_gattc_discovery_start_discover_characteristics(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle);
static void bt_gattc_discovery_charactiristics_cnf(bt_status_t status, void *buff);
static void bt_gattc_discovery_parse_characteristic(bt_gattc_read_by_type_rsp_t *data, bt_gattc_discovery_service_t *service);
static void bt_gattc_discovery_parse_char_command_data(bt_gattc_discovery_characteristic_t *char_info, uint8_t *attribute_data_list);
static bt_status_t bt_gattc_discovery_all_descriptor(uint16_t conn_handle, bt_gattc_discovery_service_t *service, uint8_t index);
static bt_status_t bt_gattc_discovery_descriptor_of_characteristic(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle);
static void bt_gattc_discovery_descriptors_cnf(bt_status_t status, void *buff);
static void bt_gattc_discovery_parse_descriptor(bt_gattc_find_info_rsp_t *data, bt_gattc_discovery_characteristic_t *chara);
static void bt_gattc_discovery_discover_service_complete(void);
static void bt_gattc_discovery_disconnect_error_evt_trigger(void);
static void bt_gattc_discovery_result_evt_trigger(bool is_complete, int32_t err_code);
static void bt_gattc_discovery_pending_user_evts_send(void);
static bt_gattc_discovery_status_t bt_gattc_discovery_registered_handler_set(ble_uuid_t *srv_uuid, bt_gattc_discovery_service_t *srv_info,
                                             const bt_gattc_discovery_event_handler_t evt_handler);

/************************************************
*   Utilities
*************************************************/
static void bt_gattc_discovery_input_buffer_reset(bt_gattc_discovery_service_t *buffer)
{
    LOG_I(BT_GATTC, "%s: enter\r\n", __FUNCTION__);

    buffer->start_handle = 0x00;
    buffer->end_handle = 0x00;
    buffer->char_count_found = 0;
    memset(&(buffer->service_uuid), 0x00, sizeof(ble_uuid_t));
    LOG_I(BT_GATTC, "bt_gattc_discovery_input_buffer_reset char_count is %d\r\n", buffer->characteristic_count);
    for(int i = 0; i < buffer->characteristic_count; i++) {
        (buffer->charateristics + i)->handle = 0x00;
        (buffer->charateristics + i)->value_handle = 0x00;
        (buffer->charateristics + i)->property = 0x00;
        (buffer->charateristics + i)->descr_count_found = 0;
        memset(&((buffer->charateristics + i)->char_uuid), 0x00, sizeof(ble_uuid_t));
        LOG_I(BT_GATTC, "bt_gattc_discovery_input_buffer_reset descr_count is %d\r\n", (buffer->charateristics + i)->descriptor_count);
        for(int j = 0; j < (buffer->charateristics + i)->descriptor_count; j++) {
            ((buffer->charateristics + i)->descriptor + j)->handle = 0x00;
            memset(&(((buffer->charateristics + i)->descriptor + j)->descriptor_uuid), 0x00, sizeof(ble_uuid_t));
        }
    }
}

static bt_status_t bt_gattc_discovery_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    LOG_I(BT_GATTC, "%s: status(0x%04x), msg(0x%04x)", __FUNCTION__, status, msg);
    if (!g_initialized)
    {
        return BT_STATUS_SUCCESS;
    }

    switch (msg) {
         case BT_GAP_LE_DISCONNECT_IND: {
            bt_gap_le_disconnect_ind_t *dis_ind = (bt_gap_le_disconnect_ind_t *)buff;
            if (g_gattc_discovery.conn_handle == dis_ind->connection_handle) {
                if (g_gattc_discovery.discovery_in_progress) {
                    bt_gattc_discovery_disconnect_error_evt_trigger();
                    //g_num_of_discovered = 0;
                    //memset(&g_gattc_discovery, 0x00, sizeof(bt_gattc_discovery_t));
                    g_gattc_discovery.discovery_in_progress = false;
                }
            }
            break;
        }

        case BT_GATTC_DISCOVER_PRIMARY_SERVICE_BY_UUID: {
            bt_gattc_discovery_primary_service_cnf(status, buff);
            break;
        }
        case BT_GATTC_DISCOVER_CHARC: {
            bt_gattc_discovery_charactiristics_cnf(status, buff);
            break;
        }
        case BT_GATTC_DISCOVER_CHARC_DESCRIPTOR: {
            bt_gattc_discovery_descriptors_cnf(status, buff);
            break;
        }
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_gattc_discovery_init(void)
{
    LOG_I(BT_GATTC, "%s: enter\r\n", __FUNCTION__);  
    if (g_initialized) {
        LOG_I(BT_GATTC, "bt_gattc_discovery_init fail, because it has been initialized by others! \r\n");
        return BT_STATUS_FAIL;
    }

    g_num_of_handlers_reg      = 0;
    g_initialized              = true;
    g_num_of_discovered        = 0;
    g_pending_usr_evt_index    = 0;
    memset(&g_gattc_discovery, 0x00, sizeof(bt_gattc_discovery_t));
    g_gattc_discovery.discovery_in_progress = false;
    return bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_GAP | MODULE_MASK_GATT, (void *)bt_gattc_discovery_event_callback);
}

#if 0
static bt_status_t bt_gattc_discovery_deinit(void)
{
    LOG_I(BT_GATTC, "%s: enter\r\n", __FUNCTION__);  

    g_num_of_handlers_reg      = 0;
    g_initialized              = false;
    g_num_of_discovered        = 0;
    g_pending_usr_evt_index    = 0;
    memset(&g_gattc_discovery, 0x00, sizeof(bt_gattc_discovery_t));
    g_gattc_discovery.discovery_in_progress = false;
    return bt_callback_manager_deregister_callback(bt_callback_type_app_event, (void *)bt_gattc_discovery_event_callback);
}    
#endif

bt_gattc_discovery_status_t bt_gattc_discovery_service_register(ble_uuid_t *uuid, bt_gattc_discovery_service_t *srv_info, const bt_gattc_discovery_event_handler_t handler)
{
    LOG_I(BT_GATTC, "%s: enter\r\n", __FUNCTION__);  

    if ((uuid == NULL) || (srv_info == NULL) || (handler == NULL))
    {
        /**< Have pointer is NULL. */
        return BT_GATTC_DISCOVERY_STATUS_INVALID_PARAMS;
    }

    if (!g_initialized)
    {
        /**< Please call bt_gattc_discovery_init first. */
        return BT_GATTC_DISCOVERY_STATUS_INVALID_STATE;
    }

    if (g_num_of_handlers_reg == BT_GATTC_DISCOVERY_MAX_USERS)
    {
        /**< The limit on the maximum number of supported users has been reached. Return error. */
        return BT_GATTC_DISCOVERY_STATUS_OUT_OF_MEMORY;
    }
    if (g_gattc_discovery.discovery_in_progress) {
        /**< a discovery is already in progress. please register again later. */
        return BT_GATTC_DISCOVERY_STATUS_BUSY;
    }
    return bt_gattc_discovery_registered_handler_set(uuid, srv_info, handler);
}                                         

static bt_gattc_discovery_status_t bt_gattc_discovery_registered_handler_set(ble_uuid_t *srv_uuid, bt_gattc_discovery_service_t *srv_info,
                                             const bt_gattc_discovery_event_handler_t evt_handler)
{
    LOG_I(BT_GATTC, "%s: enter\r\n", __FUNCTION__);  

    if (g_num_of_handlers_reg < BT_GATTC_DISCOVERY_MAX_USERS)
    {
        memset(&(g_registered_handlers[g_num_of_handlers_reg].service_uuid), 0x00, sizeof(ble_uuid_t));
        g_registered_handlers[g_num_of_handlers_reg].service_uuid.type = srv_uuid->type;
        if (BLE_UUID_TYPE_16BIT == g_registered_handlers[g_num_of_handlers_reg].service_uuid.type) {
            g_registered_handlers[g_num_of_handlers_reg].service_uuid.uuid.uuid16 = srv_uuid->uuid.uuid16;
            LOG_I(BT_GATTC, "uuid16 = 0x%04x\r\n", g_registered_handlers[g_num_of_handlers_reg].service_uuid.uuid.uuid16);
        } else if (BLE_UUID_TYPE_32BIT == g_registered_handlers[g_num_of_handlers_reg].service_uuid.type) {
            g_registered_handlers[g_num_of_handlers_reg].service_uuid.uuid.uuid16 = srv_uuid->uuid.uuid32;
            LOG_I(BT_GATTC, "uuid32 = 0x%04x\r\n", g_registered_handlers[g_num_of_handlers_reg].service_uuid.uuid.uuid32);
        } else if (BLE_UUID_TYPE_128BIT == g_registered_handlers[g_num_of_handlers_reg].service_uuid.type){
            memcpy(g_registered_handlers[g_num_of_handlers_reg].service_uuid.uuid.uuid, srv_uuid->uuid.uuid, 16);
            for (int i = 15; i >= 0; i--) {
                printf("0x%02x \r\n", g_registered_handlers[g_num_of_handlers_reg].service_uuid.uuid.uuid[i]);
            }
            printf("\n");
        } else {
            LOG_I(BT_GATTC, "bt_gattc_discovery_registered_handler_set error uuid! \r\n");
            return BT_GATTC_DISCOVERY_STATUS_INVALID_UUID;
        }

        g_registered_handlers[g_num_of_handlers_reg].service_info = srv_info;
        g_registered_handlers[g_num_of_handlers_reg].event_handler = evt_handler;
        g_num_of_handlers_reg++;
        return BT_GATTC_DISCOVERY_STATUS_SUCCESS;
    }
    else
    {
        return BT_GATTC_DISCOVERY_STATUS_OUT_OF_MEMORY;
    }
}

bt_gattc_discovery_status_t bt_gattc_discovery_start(uint16_t conn_handle)
{
    int32_t err_code;

    LOG_I(BT_GATTC, "%s: conn_handle is 0x%04x\r\n", __FUNCTION__, conn_handle);  

    if (!g_initialized)
    {
        /** Please call bt_gattc_discovery_init first. */
        return BT_GATTC_DISCOVERY_STATUS_INVALID_STATE;
    }

    if (g_num_of_handlers_reg == 0)
    {
        /** No user modules were registered. There are no services to discover. */
        return BT_GATTC_DISCOVERY_STATUS_INVALID_STATE;
    }
    LOG_I(BT_GATTC, "bt_gattc_discovery_start discovery_in_progress is %d\r\n", g_gattc_discovery.discovery_in_progress);
    if (g_gattc_discovery.discovery_in_progress)
    {
        /** Have a user modules call this api. */
        return BT_GATTC_DISCOVERY_STATUS_BUSY;
    }

    bt_gattc_discovery_service_t *p_srv_being_discovered;

    g_num_of_discovered = 0;
    g_pending_usr_evt_index   = 0;

    g_gattc_discovery.curr_srv_ind = 0;
    g_gattc_discovery.conn_handle  = conn_handle;
    g_gattc_discovery.curr_char_ind = 0;
    g_gattc_discovery.char_count = 0;
    g_gattc_discovery.descr_count = 0;


    p_srv_being_discovered = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info;

    bt_gattc_discovery_input_buffer_reset(p_srv_being_discovered);

    p_srv_being_discovered->service_uuid.type = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type;
    if (BLE_UUID_TYPE_16BIT == g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type) {
        p_srv_being_discovered->service_uuid.uuid.uuid16 = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.uuid.uuid16;
        LOG_I(BT_GATTC, "Starting discovery of service with UUID16 0x%04x for Connection handle %d\r\n", 
              p_srv_being_discovered->service_uuid.uuid.uuid16, g_gattc_discovery.conn_handle);
    } else if (BLE_UUID_TYPE_32BIT == g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type) {
        p_srv_being_discovered->service_uuid.uuid.uuid32 = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.uuid.uuid32;
        LOG_I(BT_GATTC, "Starting discovery of service with UUID32 0x%08x for Connection handle %d\r\n", 
              p_srv_being_discovered->service_uuid.uuid.uuid32, g_gattc_discovery.conn_handle);
    } else if (BLE_UUID_TYPE_128BIT == g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type){
        memcpy(p_srv_being_discovered->service_uuid.uuid.uuid, g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.uuid.uuid, 16);
        for (int i = 15; i >= 0; i--) {
            printf("0x%02x \r\n", p_srv_being_discovered->service_uuid.uuid.uuid[i]);
        }
        printf("\n");
        LOG_I(BT_GATTC, "Starting discovery of service with UUID128 for Connection handle %d\r\n", g_gattc_discovery.conn_handle);
    } 

    err_code = bt_gattc_discovery_start_discover_service(g_gattc_discovery.conn_handle, &(p_srv_being_discovered->service_uuid));
    if (err_code != BT_STATUS_SUCCESS) {
        LOG_I(BT_GATTC, "bt_gattc_discovery_start_discover_service: err_code = 0x%04x\r\n", err_code);
        return BT_GATTC_DISCOVERY_STATUS_FAIL;
    }
    g_gattc_discovery.discovery_in_progress = true;
    return BT_GATTC_DISCOVERY_STATUS_SUCCESS;
}

static bt_status_t bt_gattc_discovery_start_discover_service(uint16_t conn_handle, ble_uuid_t *srv_uuid)
{
    uint8_t buff[40];    
    bt_gattc_discover_primary_service_by_uuid_req_t req;
    
    LOG_I(BT_GATTC, "%s: enter\r\n", __FUNCTION__); 
    
    req.att_req = (bt_att_find_by_type_value_req_t *)buff;
    if (BLE_UUID_TYPE_16BIT == srv_uuid->type) {
        req.attribute_value_length = 2;
        memcpy(req.att_req->attribute_value, &(srv_uuid->uuid.uuid16), 2);
    } else if (BLE_UUID_TYPE_32BIT == srv_uuid->type) {
        req.attribute_value_length = 4;
        memcpy(req.att_req->attribute_value, &(srv_uuid->uuid.uuid32), 4);
    } else if (BLE_UUID_TYPE_128BIT == srv_uuid->type){
        req.attribute_value_length = 16;
        memcpy(req.att_req->attribute_value, &(srv_uuid->uuid.uuid), 16);
    }

    req.att_req->opcode = BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST;
    req.att_req->starting_handle = SRV_DISC_START_HANDLE;
    req.att_req->ending_handle = SRV_DISC_END_HANDLE;
    req.att_req->uuid = BT_GATT_UUID16_PRIMARY_SERVICE;

    return bt_gattc_discover_primary_service_by_uuid(conn_handle, &req);    
}

static void bt_gattc_discovery_primary_service_cnf(bt_status_t status, void *buff)
{
    bt_status_t sta;
    uint16_t conn_handle = 0;
    bt_gattc_discovery_service_t *p_srv_being_discovered;
    
    LOG_I(BT_GATTC, "%s: status = %d\r\n", __FUNCTION__, status);  
    p_srv_being_discovered = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info;
    if ((status != BT_STATUS_SUCCESS) && (status != BT_ATT_ERRCODE_CONTINUE)) {
        bt_gattc_error_rsp_t *p_event_data = (bt_gattc_error_rsp_t *)buff;
        if (p_event_data) {
            conn_handle = p_event_data->connection_handle;
            if (g_gattc_discovery.conn_handle != conn_handle) {
                return;
    }
    
            if (status == BT_ATT_ERRCODE_ATTRIBUTE_NOT_FOUND) {
                if (p_srv_being_discovered->start_handle < p_srv_being_discovered->end_handle) {
                    sta = bt_gattc_discovery_start_discover_characteristics(conn_handle, p_srv_being_discovered->start_handle,
                        p_srv_being_discovered->end_handle);
                    if (BT_STATUS_SUCCESS != sta) {
                        /**< Error with discovering the service. Indicate the error to the registered user application. */
                        bt_gattc_discovery_result_evt_trigger(false, sta);  
                        bt_gattc_discovery_discover_service_complete();
                    } 
                } else {
                    /**< Trigger Service Not Found event to the application. */
                    bt_gattc_discovery_result_evt_trigger(false, BT_GATTC_DISCOVERY_ERROR_SERVICE_NOT_FOUND);
                    bt_gattc_discovery_discover_service_complete();
                }
            } else {
                /**< Trigger Service Not Found event to the application. */
                bt_gattc_discovery_result_evt_trigger(false, BT_GATTC_DISCOVERY_ERROR_SERVICE_NOT_FOUND);
                bt_gattc_discovery_discover_service_complete();
            }
        }
    } else {
        bt_gattc_find_by_type_value_rsp_t *p_event_data = (bt_gattc_find_by_type_value_rsp_t *)buff;
    if (p_event_data) {
        conn_handle = p_event_data->connection_handle;
        if (g_gattc_discovery.conn_handle != conn_handle) {
            return;
        }
        
        if (status == BT_ATT_ERRCODE_CONTINUE) {
            bt_gattc_discovery_parse_primary_service(p_event_data, p_srv_being_discovered);
        } else if (status == BT_STATUS_SUCCESS) {
            if (p_event_data->att_rsp) {
                bt_gattc_discovery_parse_primary_service(p_event_data, p_srv_being_discovered);
            }
            
            if (p_srv_being_discovered->characteristic_count) {
                sta = bt_gattc_discovery_start_discover_characteristics(conn_handle, p_srv_being_discovered->start_handle,
                    p_srv_being_discovered->end_handle);
                if (BT_STATUS_SUCCESS != sta) {
                    /**< Error with discovering the service. Indicate the error to the registered user application. */
                    bt_gattc_discovery_result_evt_trigger(false, sta);  
                    bt_gattc_discovery_discover_service_complete();
                }
            }  else {
                /**< Trigger Service Not Found event to the application. */
                bt_gattc_discovery_result_evt_trigger(true, 0);
                bt_gattc_discovery_discover_service_complete();
            }
            }
        }
    }
}

static void bt_gattc_discovery_parse_primary_service(bt_gattc_find_by_type_value_rsp_t *rsp, bt_gattc_discovery_service_t *data)
{
    uint8_t i;
    uint8_t *attribute_data_list = rsp->att_rsp->handles_info_list;
    uint8_t num_of_data = (rsp->length - 1) / 4;
    LOG_I(BT_GATTC, "%s: num_of_primary is %d\r\n", __FUNCTION__, num_of_data);  

    for (i = 0 ; i < num_of_data; i++) {
        memcpy(&(data->start_handle), attribute_data_list + i * 4, 2);
        memcpy(&(data->end_handle), attribute_data_list +  i * 4 + 2, 2);
        LOG_I(BT_GATTC, "starting_handle = 0x%04x, end_handle = 0x%04x \r\n", data->start_handle, data->end_handle);
    }
}

/**@brief     Function for performing characteristic discovery.
 *
 * @param[in] g_gattc_discovery Pointer to the DB Discovery structure.
 *
 * @return    NRF_SUCCESS if the SoftDevice was successfully requested to perform the characteristic
 *            discovery. Otherwise an error code. This function returns the error code returned
 *            by the SoftDevice API @ref sd_ble_gattc_characteristics_discover.
 */
static bt_status_t bt_gattc_discovery_start_discover_characteristics(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle)
{
    bt_status_t status;
    bt_gattc_discover_charc_req_t req;
    
    LOG_I(BT_GATTC, "discover_char_start: handle range: start_handle = 0x%04x, end_handle = 0x%04x\r\n", start_handle, end_handle);
    uint16_t uuid_16 = BT_GATT_UUID16_CHARC;
    req.opcode = BT_ATT_OPCODE_READ_BY_TYPE_REQUEST;
    req.starting_handle = start_handle;
    req.ending_handle = end_handle;
    bt_uuid_load(&(req.type), (void *)&uuid_16, 2);
    status = bt_gattc_discover_charc(conn_handle, &req);
    LOG_I(BT_GATTC, "dis_char end: status = %d", status);
    return status;
}

static void bt_gattc_discovery_charactiristics_cnf(bt_status_t status, void *buff)
{
    bt_status_t sta;
    uint16_t conn_handle;
    bt_gattc_discovery_service_t *p_srv_being_discovered;

    LOG_I(BT_GATTC, "discover_charactiristics_cnf: status = %d\r\n", status);

    p_srv_being_discovered = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info;
    if ((status != BT_STATUS_SUCCESS) && (status != BT_ATT_ERRCODE_CONTINUE)) {
        bt_gattc_error_rsp_t *p_event_data = (bt_gattc_error_rsp_t *)buff;
        if (p_event_data == NULL) {
            return;
        }
        conn_handle = p_event_data->connection_handle;
        if (g_gattc_discovery.conn_handle != conn_handle) {
            return;
    }
    
        LOG_I(BT_GATTC, "disc_char_cnf: not found char_num = %d\r\n", g_gattc_discovery.char_count);
        /**< The previous characteristic discovery resulted in no characteristics. 
        Descriptor discovery should be performed. */
        sta = bt_gattc_discovery_all_descriptor(conn_handle, p_srv_being_discovered, 0);
        if (BT_STATUS_SUCCESS != sta) {
            /**< Error with discovering the service. Indicate the error to the registered user application. */
            bt_gattc_discovery_result_evt_trigger(false, sta); 
            bt_gattc_discovery_discover_service_complete();
        }
    } else {
        bt_gattc_read_by_type_rsp_t *p_event_data = (bt_gattc_read_by_type_rsp_t *)buff;
    if (p_event_data == NULL) {
        return;
    }
    conn_handle = p_event_data->connection_handle;
    if (g_gattc_discovery.conn_handle != conn_handle) {
        return;
    }

    if (status == BT_ATT_ERRCODE_CONTINUE) {
        bt_gattc_discovery_parse_characteristic(p_event_data, p_srv_being_discovered);
    } else if (status == BT_STATUS_SUCCESS) {
        if (p_event_data->att_rsp) {
            bt_gattc_discovery_parse_characteristic(p_event_data, p_srv_being_discovered);
        }
        if (p_srv_being_discovered->characteristic_count) {
            sta = bt_gattc_discovery_all_descriptor(conn_handle, p_srv_being_discovered, 0);
            if (BT_STATUS_SUCCESS != sta) {
                /**< Error with discovering the service. Indicate the error to the registered user application. */
                bt_gattc_discovery_result_evt_trigger(false, sta);
                bt_gattc_discovery_discover_service_complete();
            }
        } else {
            /**< Trigger Service Not Found event to the application. */
            bt_gattc_discovery_result_evt_trigger(true, 0);
            bt_gattc_discovery_discover_service_complete();
        }
        }
    }
}

static void bt_gattc_discovery_parse_characteristic(bt_gattc_read_by_type_rsp_t *data, bt_gattc_discovery_service_t *service)
{    
    bt_gattc_read_by_type_rsp_t *rsp = (bt_gattc_read_by_type_rsp_t *)data;
    bt_gattc_discovery_characteristic_t *char_info;
    uint8_t *attribute_data_list = rsp->att_rsp->attribute_data_list;

    LOG_I(BT_GATTC, "%s: enter\r\n", __FUNCTION__);  

    uint8_t num_of_data = (rsp->length - 2) / rsp->att_rsp->length;
    g_gattc_discovery.char_count += num_of_data;
    
    LOG_I(BT_GATTC, "ble_gattc_parse_characteristic : char_num = %d, char_count = %d, real char_count = %d\r\n", num_of_data, 
        service->characteristic_count, g_gattc_discovery.char_count);
    if (service->characteristic_count < g_gattc_discovery.char_count) {
        num_of_data = service->characteristic_count - (g_gattc_discovery.char_count - num_of_data);
        g_gattc_discovery.char_count = service->characteristic_count;
    }
    
    if (rsp->att_rsp->length < 20) {/*16+2 + 1 +1*/
        for (int i = 0 ; i < num_of_data; i++) {
            char_info = service->charateristics + i + service->char_count_found;
            bt_gattc_discovery_parse_char_command_data(char_info, (rsp->att_rsp->attribute_data_list + i * rsp->att_rsp->length));
            char_info->char_uuid.type = BLE_UUID_TYPE_16BIT;
            memcpy(&(char_info->char_uuid.uuid.uuid16), attribute_data_list + i * rsp->att_rsp->length + 5, 2);
            LOG_I(BT_GATTC, "char_index = %d, char_uuid16 = 0x%04x\r\n", i, char_info->char_uuid.uuid.uuid16);
        }
    } else {
        for (int j = 0 ; j < num_of_data; j++) {
            char_info = service->charateristics + j + service->char_count_found;
            bt_gattc_discovery_parse_char_command_data(char_info, (rsp->att_rsp->attribute_data_list + j * rsp->att_rsp->length)); 
        char_info->char_uuid.type = BLE_UUID_TYPE_128BIT;
            memcpy(&(char_info->char_uuid.uuid.uuid), attribute_data_list + j * rsp->att_rsp->length + 5, 16); 
        LOG_I(BT_GATTC, "char_uuid1128 is\r\n");
        for (int i = 15; i >= 0; i--) {
            printf("0x%02x \r\n", char_info->char_uuid.uuid.uuid[i]);
        }
        printf("\n");
    }
}
    service->char_count_found = g_gattc_discovery.char_count;
    LOG_I(BT_GATTC, "ble_gattc_parse_characteristic : char_count_found = %d\r\n", service->char_count_found);
}

static void bt_gattc_discovery_parse_char_command_data(bt_gattc_discovery_characteristic_t *char_info, uint8_t *attribute_data_list)
{
    LOG_I(BT_GATTC, "%s: enter\r\n", __FUNCTION__);  

    if (attribute_data_list) {
        memcpy(&(char_info->handle), attribute_data_list, 2);
        memcpy(&(char_info->property), attribute_data_list + 2, 1);
        memcpy(&(char_info->value_handle), attribute_data_list + 3, 2);
        LOG_I(BT_GATTC, "char_handle = 0x%04x, value_handle = 0x%04x, property = %d\r\n", char_info->handle, 
            char_info->value_handle, char_info->property);
    }

}

static bt_status_t bt_gattc_discovery_all_descriptor(uint16_t conn_handle, bt_gattc_discovery_service_t *service, uint8_t index)
{
    bt_gattc_discovery_characteristic_t *cur_char, *next_char;
    bt_status_t status = 0;
    uint8_t char_num;
    
    LOG_I(BT_GATTC, "%s: enter\r\n", __FUNCTION__);
    
    char_num = g_gattc_discovery.char_count;
    g_gattc_discovery.curr_char_ind = index;
    g_gattc_discovery.descr_count = 0;
    if (index <= (char_num - 1)) {
        uint16_t start_handle;
        uint16_t end_handle;
        cur_char = service->charateristics + index;
        start_handle = cur_char->value_handle + 1;

        LOG_I(BT_GATTC, "[Gattc]ble_gattc_find_all_descriptor char_index = %d, char_num = %d\r\n", index, char_num);

        if (0 == cur_char->descriptor_count) {
            status = bt_gattc_discovery_all_descriptor(conn_handle, service, index + 1);
            return status;
        }
        /* This one might be the last characteristic in service */
        if (index == char_num - 1) {
            end_handle = service->end_handle;

        } else {
            next_char = service->charateristics + index + 1;
            end_handle = next_char->handle - 1;
        }

        LOG_I(BT_GATTC, "[Gattc]ble_gattc_find_all_descriptor start_handle = 0x%04x, end_handle =0x%04x\r\n", start_handle, end_handle);
        if (start_handle <= end_handle) {
            status = bt_gattc_discovery_descriptor_of_characteristic(conn_handle, start_handle, end_handle);
        } else {
            status = bt_gattc_discovery_all_descriptor(conn_handle, service, index + 1);
        }
    } else {/*all descriptor is complete done */

        LOG_I(BT_GATTC, "[Gattc]ble_gattc_find_all_descriptor_of_char done, to discover next service!\r\n");
        bt_gattc_discovery_result_evt_trigger(true, 0);
        bt_gattc_discovery_discover_service_complete();
    }
    return status;
}

static bt_status_t bt_gattc_discovery_descriptor_of_characteristic(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle)
{
    bt_status_t status;
    bt_gattc_discover_charc_descriptor_req_t req;
    LOG_I(BT_GATTC, "discovery_descriptor_start: start_handle = 0x%04x, end_handle = 0x%04x\r\n", start_handle, end_handle);
    
    req.opcode = BT_ATT_OPCODE_FIND_INFORMATION_REQUEST;
    req.starting_handle = start_handle;
    req.ending_handle = end_handle;

    status = bt_gattc_discover_charc_descriptor(conn_handle, &req);
    LOG_I(BT_GATTC, "dis_descr end: status = %d\r\n", status);
    return status;
}

static void bt_gattc_discovery_descriptors_cnf(bt_status_t status, void *buff)
{
    uint16_t conn_handle;
    bt_gattc_discovery_characteristic_t *chara = NULL;
    bt_gattc_discovery_service_t *service;
    LOG_I(BT_GATTC, "%s: enter, status = %d\r\n", __FUNCTION__, status);  
 
    service = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info;
    if (service == NULL) {
        return;
    }
    if (g_gattc_discovery.curr_char_ind < g_gattc_discovery.char_count) {
        chara = service->charateristics + g_gattc_discovery.curr_char_ind;
    }
    g_gattc_discovery.curr_char_ind++;
    
    if ((status != BT_STATUS_SUCCESS) && (status != BT_ATT_ERRCODE_CONTINUE)) {
        bt_gattc_error_rsp_t *p_event_data = (bt_gattc_error_rsp_t *)buff;
    if (p_event_data == NULL) {
        return;
    }
    conn_handle = p_event_data->connection_handle;
    if (g_gattc_discovery.conn_handle != conn_handle) {
        return;
    }

        LOG_I(BT_GATTC, "bt_gattc_discovery_descriptors_cnf: fail:index = %d\r\n", g_gattc_discovery.curr_char_ind);
        bt_gattc_discovery_all_descriptor(conn_handle, service, g_gattc_discovery.curr_char_ind);;
    } else {
        bt_gattc_find_info_rsp_t *p_event_data = (bt_gattc_find_info_rsp_t *)buff;
        if (p_event_data == NULL) {
        return;
    }
        conn_handle = p_event_data->connection_handle;
        if (g_gattc_discovery.conn_handle != conn_handle) {
            return;
    }

    if (status == BT_ATT_ERRCODE_CONTINUE) {
        bt_gattc_discovery_parse_descriptor(p_event_data, chara);
    } else if (status == BT_STATUS_SUCCESS) {
        if (p_event_data->att_rsp) {
            bt_gattc_discovery_parse_descriptor(p_event_data, chara);
        }
        LOG_I(BT_GATTC, "bt_gattc_discovery_descriptors_cnf: succsee:index = %d\r\n", g_gattc_discovery.curr_char_ind);
        bt_gattc_discovery_all_descriptor(conn_handle, service, g_gattc_discovery.curr_char_ind);
        }
    }
}

static void bt_gattc_discovery_parse_descriptor(bt_gattc_find_info_rsp_t *data, bt_gattc_discovery_characteristic_t *chara)
{
    uint8_t format = 0;
    uint8_t attribute_length = 0;
    uint8_t i;

    bt_gattc_find_info_rsp_t rsp = *data;
    bt_gattc_discovery_descriptor_t *descr_info;

    LOG_I(BT_GATTC, "%s: enter\r\n", __FUNCTION__);  

    if (rsp.att_rsp) {
        format = rsp.att_rsp->format;
    }
    if (format == 0x02) {
        attribute_length = 18;
    } else {
        attribute_length = 4;
    }
    uint8_t num_of_attribute = (rsp.length - 2) / attribute_length;
    g_gattc_discovery.descr_count += num_of_attribute;
    
    LOG_I(BT_GATTC, "descr_count = %d, num is = %d, real num is %d\r\n", chara->descriptor_count, num_of_attribute, g_gattc_discovery.descr_count);
    if (chara->descriptor_count < g_gattc_discovery.descr_count) {
        num_of_attribute = chara->descriptor_count - (g_gattc_discovery.descr_count - num_of_attribute);
        g_gattc_discovery.descr_count = chara->descriptor_count;
    }
    
    for (i = 0; i < num_of_attribute; i++) {
        descr_info = chara->descriptor + i + chara->descr_count_found;
        if (format == 0x02) {
            /* uuid 128 */
            memcpy(&(descr_info->handle), rsp.att_rsp->info_data + i * attribute_length, 2);
            descr_info->descriptor_uuid.type = BLE_UUID_TYPE_128BIT;
            memcpy(&(descr_info->descriptor_uuid.uuid.uuid), rsp.att_rsp->info_data + i * attribute_length + 2, 16);
        } else {
            /* uuid 16 */
            memcpy(&(descr_info->handle), rsp.att_rsp->info_data + i * attribute_length, 2);
            descr_info->descriptor_uuid.type = BLE_UUID_TYPE_16BIT;
            memcpy(&(descr_info->descriptor_uuid.uuid.uuid16), rsp.att_rsp->info_data + i * attribute_length + 2, 2);
            LOG_I(BT_GATTC, "descr_uuid16 = 0x%04x\r\n", descr_info->descriptor_uuid.uuid.uuid16);
        }
        LOG_I(BT_GATTC, "attribute handle = 0x%04x\r\n", descr_info->handle);
    }
    chara->descr_count_found = g_gattc_discovery.descr_count;
    LOG_I(BT_GATTC, "descr_count_found %d\r\n", chara->descr_count_found);
}

/**@brief     Function for handling service discovery completion.
 *
 * @details   This function will be used to determine if there are more services to be discovered,
 *            and if so, initiate the discovery of the next service.
 *
 * @param[in] g_gattc_discovery Pointer to the DB Discovery Structure.
 */
static void bt_gattc_discovery_discover_service_complete(void)
{
    uint32_t err_code;

    g_num_of_discovered++;
    
    LOG_I(BT_GATTC, "%s: g_num_of_discovered = %d\r\n", __FUNCTION__, g_num_of_discovered);  
    /**< Check if more services need to be discovered. */
    if (g_num_of_discovered < g_num_of_handlers_reg)
    {
       bt_gattc_discovery_service_t *p_srv_being_discovered;
        /**< Reset the current service information since a new service discovery is about to start. */
        g_gattc_discovery.curr_char_ind = 0;
        g_gattc_discovery.char_count = 0;
        g_gattc_discovery.descr_count = 0;

        /**< Initiate discovery of the next service. */
        g_gattc_discovery.curr_srv_ind++;
        p_srv_being_discovered = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info;
        
        LOG_I(BT_GATTC, "bt_gattc_discovery_discover_service_complete char_count = %d\r\n", p_srv_being_discovered->characteristic_count);
        bt_gattc_discovery_input_buffer_reset(p_srv_being_discovered);
        p_srv_being_discovered->service_uuid.type = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type;
        if (BLE_UUID_TYPE_16BIT == g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type) {
            p_srv_being_discovered->service_uuid.uuid.uuid16 = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.uuid.uuid16;
            LOG_I(BT_GATTC, "Starting discovery of service with UUID16 0x%04x for Connection handle %d\r\n", 
                p_srv_being_discovered->service_uuid.uuid.uuid16, g_gattc_discovery.conn_handle);
        } else if (BLE_UUID_TYPE_32BIT == g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type) {
            p_srv_being_discovered->service_uuid.uuid.uuid32 = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.uuid.uuid32;
            LOG_I(BT_GATTC, "Starting discovery of service with UUID32 0x%08x for Connection handle %d\r\n", 
                p_srv_being_discovered->service_uuid.uuid.uuid32, g_gattc_discovery.conn_handle);
        } else if (BLE_UUID_TYPE_128BIT == g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type){
            memcpy(p_srv_being_discovered->service_uuid.uuid.uuid, g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.uuid.uuid, 16);
            LOG_I(BT_GATTC, "Starting discovery of service with UUID128 for Connection handle %d\r\n", g_gattc_discovery.conn_handle);
            for (int i = 15; i >= 0; i--) {
                printf("0x%02x \r\n", p_srv_being_discovered->service_uuid.uuid.uuid[i]);
            }
            printf("\n");
        } 
        
        err_code = bt_gattc_discovery_start_discover_service(g_gattc_discovery.conn_handle, &(p_srv_being_discovered->service_uuid));
        if (err_code != BT_STATUS_SUCCESS)
        {
            bt_gattc_discovery_result_evt_trigger(false, err_code);
            bt_gattc_discovery_discover_service_complete();
        }                
    }
    else
    {
        /**< No more service discovery is needed. */  
        g_gattc_discovery.discovery_in_progress = false;
        LOG_I(BT_GATTC, "all services discovery complete!, discovery_in_progress is %d\r\n", g_gattc_discovery.discovery_in_progress);
    }
}


/**@brief     Function for triggering a Discovery Complete or indicating error event to the
 *            application.
 *
 * @details   This function will fetch the event handler based on the UUID of the service being
 *            discovered. (The event handler is registered by the application beforehand).
 *            The error code is added to the pending events together with the event handler.
 *            If no event handler was found, then this function will do nothing.
 *
 * @param[in] is_complete   Variable to indicate if the service was found at the peer.
 * @param[in] err_code       Error code that should be provided to the application.
 *
 */
static void bt_gattc_discovery_result_evt_trigger(bool is_complete, int32_t err_code)
{
    bt_gattc_discovery_event_handler_t p_evt_handler;
    bt_gattc_discovery_service_t       *p_srv_being_discovered;

    p_srv_being_discovered = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info;
    p_evt_handler = g_registered_handlers[g_gattc_discovery.curr_srv_ind].event_handler;
    LOG_I(BT_GATTC, "%s: is_complete= %d, error_code = %d\r\n", __FUNCTION__, is_complete, err_code);  

    if (p_evt_handler != NULL)
    {
        if (g_pending_usr_evt_index < BT_GATTC_DISCOVERY_MAX_USERS)
        {
            /**< Insert an event into the pending event list. */
            g_pending_user_evts[g_pending_usr_evt_index].event.conn_handle = g_gattc_discovery.conn_handle; 
            g_pending_user_evts[g_pending_usr_evt_index].event_handler = p_evt_handler;
            if (is_complete)
            {
                g_pending_user_evts[g_pending_usr_evt_index].event.event_type = BT_GATTC_DISCOVERY_EVENT_COMPLETE;
                g_pending_user_evts[g_pending_usr_evt_index].event.params.discovered_db = p_srv_being_discovered;
            } else {
                g_pending_user_evts[g_pending_usr_evt_index].event.event_type = BT_GATTC_DISCOVERY_EVENT_FAIL;
                g_pending_user_evts[g_pending_usr_evt_index].event.params.error_code = err_code;
            }
            g_pending_usr_evt_index++;

            if (g_pending_usr_evt_index == g_num_of_handlers_reg)
            {
                /**< All registered modules have pending events. Send all pending events to the user modules. */
                bt_gattc_discovery_pending_user_evts_send();
            }
        }
        else
        {
            /**< Too many events pending. Do nothing. (Ideally this should not happen.) */
        }
    }
}


static void bt_gattc_discovery_disconnect_error_evt_trigger(void)
{
    int8_t i = 0;
    bt_gattc_discovery_event_handler_t p_evt_handler;
    bt_gattc_discovery_service_t       *p_srv_being_discovered;

    LOG_I(BT_GATTC, "%s: enter\r\n", __FUNCTION__);  

    if (g_num_of_discovered <= g_num_of_handlers_reg) {
        for (i = g_num_of_discovered; i < g_num_of_handlers_reg; i++) {
        p_evt_handler = g_registered_handlers[i].event_handler;
        p_srv_being_discovered = g_registered_handlers[i].service_info;
        bt_gattc_discovery_input_buffer_reset(p_srv_being_discovered);

        /**< Insert an event into the pending event list. */
        g_pending_user_evts[i].event.conn_handle = g_gattc_discovery.conn_handle; 
        g_pending_user_evts[i].event_handler = p_evt_handler;
        g_pending_user_evts[i].event.event_type = BT_GATTC_DISCOVERY_EVENT_FAIL;
        g_pending_user_evts[i].event.params.error_code = BT_GATTC_DISCOVERY_ERROR_UNEXPECT_DISCONNECT; 
    }
        if (i == g_num_of_handlers_reg) {
        /**< All registered modules have pending events. Send all pending events to the user modules. */
        bt_gattc_discovery_pending_user_evts_send();
    }
}
}

/**@brief Function for sending all pending discovery events to the corresponding user modules.
 */
static void bt_gattc_discovery_pending_user_evts_send(void)
{
    uint32_t i;
    LOG_I(BT_GATTC, "%s: enter\r\n", __FUNCTION__);  

    for (i = 0; i < g_num_of_handlers_reg; i++)
    {
        /**< Pass the event to the corresponding event handler. */
        g_pending_user_evts[i].event_handler(&(g_pending_user_evts[i].event));
        memset(&(g_pending_user_evts[i]), 0x00, sizeof(bt_gattc_pending_user_events));
    }
    if (i == g_num_of_handlers_reg) {
        
        LOG_I(BT_GATTC, "%s: notify all registered users success\r\n", __FUNCTION__);  
        g_pending_usr_evt_index = 0;
    }
}


