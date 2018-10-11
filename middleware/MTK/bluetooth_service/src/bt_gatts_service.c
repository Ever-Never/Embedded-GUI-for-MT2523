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
#include "bt_uuid.h"
#include "bt_system.h"
#include "bt_gattc.h"
#include "bt_gatt.h"
#include "bt_gatts.h"
#include "bt_gap_le.h"
#include "bt_hci.h"
#include "bt_gatts_service.h"
#include "bt_callback_manager.h"
#include "syslog.h"


log_create_module(BT_GATTS, PRINT_LEVEL_INFO);

#define BT_GATTS_SRV_DEVNAME_MAX_LEN             (248)     /**< The Max length of the Device Name characteristic value. */

#ifdef BT_GATTS_SERVICE_CHANGED
#define BT_GATTS_SRV_LE_CONNECTION_MAX           (8)       /**< Defineds the maxium number of LE connections. */
#define BT_GATTS_SRV_CCCD_NOTIFICATION           (0x0001)
#define BT_GATTS_SRV_CCCD_INDICATION             (0x0002)
/**
 *  @brief GATTS Service Change Characteristic Structure, Internal use.
 */
typedef struct {
    uint8_t           gatts_wait_att_rx_opcode;      /**< Use to wait handle value confirmation. */
    uint16_t          gatts_cccd_value;              /**< Client Characteristic Configuration of Service Changed Characteristic. */
    uint16_t          gatts_sccd_value;              /**< Server Characteristic Configuration of Service Changed Characteristic. */
} bt_gatts_service_change_info_t;

/**
 *  @brief GATTS Service Information Structure, Internal use.
 */
typedef struct {
    bt_handle_t                       conn_handle;       /**< Connection handle. */
    bt_role_t                         role;              /**< Role of the local device. */
    bt_gatts_service_change_info_t    gatts_info;        /**< Service change characteristic info. */
} bt_gatts_config_info_t;
#endif

/**************************Macro and Global*************************/
#ifdef BT_GATTS_SERVICE_CHANGED
#define BT_SERVICE_CHANGED_VALUE_HANDLE (0x0013)
const bt_uuid_t BT_SIG_UUID_SERVICE_CHANGED =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_SERVICE_CHANGED);
#endif
const bt_uuid_t BT_SIG_UUID_DEVICE_NAME =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_DEVICE_NAME);
const bt_uuid_t BT_SIG_UUID_APPEARANCE =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_APPEARANCE);
const bt_uuid_t BT_SIG_UUID_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS);
const bt_uuid_t BT_SIG_UUID_CENTRAL_ADDRESS_RESOLUTION =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_CENTRAL_ADDRESS_RESOLUTION);


/** default value. */
#ifdef BT_GATTS_SERVICE_CHANGED
static bool bt_gatts_initialized = false;
static bt_gatts_config_info_t config_info[BT_GATTS_SRV_LE_CONNECTION_MAX];
#endif
static uint8_t gap_car = 0;//central address resolution
static char gatts_device_name[BT_GATTS_SRV_DEVNAME_MAX_LEN] = {"MTKHB_DEVICE_LE"};
static uint16_t gap_appearance = 0x1234;
static bt_gatts_le_gap_ppcp_t gap_ppcp_default = {0x0580,0x0c80,0x0010,0x0333};

/************************************************
*   Utilities
*************************************************/
#ifdef BT_GATTS_SERVICE_CHANGED
static void bt_gatts_service_clear_conn_info(void);
static void bt_gatts_service_add_conn_info(void *buff);
static void bt_gatts_service_indication_cnf(void *buff);
static void bt_gatts_service_delete_conn_info(void *buff);
static bt_status_t bt_gatts_service_event_callback_register(void);
static bt_status_t bt_gatts_service_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff);
static uint32_t bt_if_gatt_client_config_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
static uint32_t bt_if_gatt_server_config_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
/**
 * @brief   Indicate the Service Changed attribute value.
 * @details This call will send a Handle Value Indication to one or more peers connected to inform them that the Attribute
 *          Table layout has changed. As soon as the peer has confirmed the indication, a @ref BT_GATTC_CHARC_VALUE_CONFIRMATION event will
 *          be issued.
 * @param[in] conn_handle  is the connection handle.
 * @param[in] start_handle Start of affected attribute handle range.
 * @param[in] end_handle   End of affected attribute handle range.
 * @return    #BT_STATUS_SUCCESS, the operation completed successfully.
 *            #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_gatts_service_change_notify(uint16_t connection_handle, uint16_t start_handle, uint16_t end_handle);
#endif

static uint32_t bt_if_gap_device_name_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
static uint32_t bt_if_gap_appearance_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
static uint32_t bt_if_gap_ppcp_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
static uint32_t bt_if_gap_car_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);


/********************** Declare every record here*******************/
/** gatt service collects all bt_gatts_service_rec_t. */
/** IMPORTAMT: handle:0x0000 is reserved, please start your handle from 0x0001. */


/** GAP Service, attribute handle from 0x0001. */ 
bt_status_t bt_gatts_service_set_gap_device_name(const uint8_t *device_name, uint16_t length)
{
    if ((!device_name) || (length > BT_GATTS_SRV_DEVNAME_MAX_LEN)) {
        return BT_STATUS_FAIL;
    } else {
        memset(&gatts_device_name, 0, BT_GATTS_SRV_DEVNAME_MAX_LEN);
            memcpy(&gatts_device_name, device_name, length);
        }
    LOG_I(BT_GATTS, "%s \r\n", gatts_device_name);
    return BT_STATUS_SUCCESS;
}

const uint8_t *bt_gatts_service_get_gap_device_name(void)
{
    return (const uint8_t *)gatts_device_name;
}

static uint32_t bt_if_gap_device_name_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    uint32_t str_size = strlen(gatts_device_name);
    uint32_t copy_size = 0;
    if (BT_GATTS_CALLBACK_READ == rw) {
        copy_size = (str_size> offset)?(str_size-offset):0;
        if (size==0) {
            return str_size;
        }
        copy_size = (size > copy_size)? copy_size:size;
        memcpy(data, gatts_device_name+offset, copy_size);
    } else if (BT_GATTS_CALLBACK_WRITE == rw) {
        copy_size = (size > BT_GATTS_SRV_DEVNAME_MAX_LEN)? BT_GATTS_SRV_DEVNAME_MAX_LEN:size;
        memcpy(gatts_device_name, data, copy_size);
    }
    return copy_size;
}

void bt_gatts_service_set_gap_appearance(uint16_t appearance)
{
    gap_appearance = appearance;
    
    LOG_I(BT_GATTS, "bt_gatts_service_set_gap_appearance, appearance(0x%04x) \r\n", gap_appearance);
}

static uint32_t bt_if_gap_appearance_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_I(BT_GATTS, "bt_if_gap_appearance_callback, opcode:%d, size:%d \r\n", rw, size);

    if (rw == BT_GATTS_CALLBACK_WRITE){
        if (size != sizeof(gap_appearance)){ //Size check
            return 0;
        }
        gap_appearance = *(uint16_t*)data;
    }else {
        if (size!=0){
            uint16_t *buf = (uint16_t*) data;
            *buf = gap_appearance;
        }
    }
    return sizeof(gap_appearance);
}

bt_status_t bt_gatts_service_set_gap_ppcp(bt_gatts_le_gap_ppcp_t *ppcp_param)
{
    if (NULL != ppcp_param) {
        gap_ppcp_default.min_conn_interval = ppcp_param->min_conn_interval;
        gap_ppcp_default.max_conn_interval = ppcp_param->max_conn_interval;
        gap_ppcp_default.slave_latency = ppcp_param->slave_latency;
        gap_ppcp_default.supervision_timeout = ppcp_param->supervision_timeout;


        LOG_I(BT_GATTS, "bt_gatts_service_set_gap_ppcp, min_interval(%d),max_interval(%d),slave_latency(%d), timeout(%d) \r\n", 
            gap_ppcp_default.min_conn_interval, gap_ppcp_default.max_conn_interval, 
            gap_ppcp_default.slave_latency, gap_ppcp_default.supervision_timeout);
        
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

bt_status_t bt_gatts_service_set_gap_central_address_resolution(uint8_t flag)
{
    if ((0 != flag) && (1 != flag)) {
        LOG_I(BT_GATTS, "set gap central address resolution fail, invalid input value!  \r\n");
        return BT_STATUS_FAIL;
    }
    gap_car = flag;   
    LOG_I(BT_GATTS, "set gap central address resolution successfully, car_flag(%d) \r\n", flag);
    return BT_STATUS_SUCCESS;
}

static uint32_t bt_if_gap_ppcp_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_I(BT_GATTS, "bt_if_gap_ppcp_callback, opcode:%d, size:%d \r\n", rw, size);

    switch (rw) {
        case BT_GATTS_CALLBACK_READ: {
            if (size!=0) {
                bt_gatts_le_gap_ppcp_t *buf = (bt_gatts_le_gap_ppcp_t*) data;
                buf->min_conn_interval = gap_ppcp_default.min_conn_interval;
                buf->max_conn_interval = gap_ppcp_default.max_conn_interval;
                buf->slave_latency = gap_ppcp_default.slave_latency;
                buf->supervision_timeout = gap_ppcp_default.supervision_timeout;
            }
            return sizeof(bt_gatts_le_gap_ppcp_t);
        }
        
        default:
            return 0;
    }
}

static uint32_t bt_if_gap_car_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_I(BT_GATTS, "bt_if_gap_car_callback, opcode:%d, size:%d \r\n", rw, size);

    switch (rw) {
        case BT_GATTS_CALLBACK_READ: {
            if (size!=0){
                uint8_t *buf = (uint8_t*) data;
                *buf = gap_car;
            }
            return sizeof(gap_car);
        }
        default:
            return 0;
    }
}

BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_gap_primary_service, BT_GATT_UUID16_GAP_SERVICE);
BT_GATTS_NEW_CHARC_16_WRITABLE(bt_if_gap_char4_dev_name, BT_GATT_CHARC_PROP_READ, 0x0003, BT_SIG_UUID16_DEVICE_NAME);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_gap_dev_name, BT_SIG_UUID_DEVICE_NAME,
                BT_GATTS_REC_PERM_READABLE|BT_GATTS_REC_PERM_WRITABLE, bt_if_gap_device_name_callback);
BT_GATTS_NEW_CHARC_16_WRITABLE(bt_if_gap_char4_appearance, BT_GATT_CHARC_PROP_READ, 0x0005, BT_SIG_UUID16_APPEARANCE);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_gap_appearance, BT_SIG_UUID_APPEARANCE,
                BT_GATTS_REC_PERM_READABLE|BT_GATTS_REC_PERM_WRITABLE, bt_if_gap_appearance_callback);
BT_GATTS_NEW_CHARC_16(bt_if_gap_char4_ppcp, BT_GATT_CHARC_PROP_READ, 0x0007, BT_SIG_UUID16_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_gap_ppcp, BT_SIG_UUID_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS,
                BT_GATTS_REC_PERM_READABLE, bt_if_gap_ppcp_callback);
BT_GATTS_NEW_CHARC_16(bt_if_gap_char4_central_address_resolution, BT_GATT_CHARC_PROP_READ, 0x0009, BT_SIG_UUID16_CENTRAL_ADDRESS_RESOLUTION);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_central_address_resolution, BT_SIG_UUID_CENTRAL_ADDRESS_RESOLUTION, BT_GATTS_REC_PERM_READABLE, bt_if_gap_car_callback);

static const bt_gatts_service_rec_t *bt_if_gap_service_rec[] = {
    (const bt_gatts_service_rec_t*) &bt_if_gap_primary_service,
    (const bt_gatts_service_rec_t*) &bt_if_gap_char4_dev_name,
    (const bt_gatts_service_rec_t*) &bt_if_gap_dev_name,
    (const bt_gatts_service_rec_t*) &bt_if_gap_char4_appearance,
    (const bt_gatts_service_rec_t*) &bt_if_gap_appearance,
    (const bt_gatts_service_rec_t*) &bt_if_gap_char4_ppcp,
    (const bt_gatts_service_rec_t*) &bt_if_gap_ppcp,
    (const bt_gatts_service_rec_t*) &bt_if_gap_char4_central_address_resolution,
    (const bt_gatts_service_rec_t*) &bt_if_central_address_resolution
    };

const bt_gatts_service_t bt_if_gap_service = {
    .starting_handle = 0x0001,
    .ending_handle = 0x0009,
    .required_encryption_key_size = 7,
    .records = bt_if_gap_service_rec
    };

#ifdef BT_GATTS_SERVICE_CHANGED
bt_status_t bt_gatts_service_change_notify(uint16_t connection_handle, uint16_t start_handle, uint16_t end_handle)
{
    uint8_t i;
    uint8_t buf[64]={0};
    uint8_t att_value[8]={0};
    bt_status_t status = BT_STATUS_FAIL;
    bt_gattc_charc_value_notification_indication_t *noti_rsp;
    noti_rsp = (bt_gattc_charc_value_notification_indication_t*) buf;
    
    if (!bt_gatts_initialized)
        LOG_I(BT_GATTS,"bt_gatts_service_change_notify fail, please init bt_gatts_service first! \r\n");
        return status;
        
    for (i=0; i<BT_GATTS_SRV_LE_CONNECTION_MAX ; i++) {
        if ((connection_handle > 0) && (config_info[i].conn_handle == connection_handle)) {
            if (config_info[i].gatts_info.gatts_cccd_value > 0) {
                if (config_info[i].gatts_info.gatts_cccd_value == BT_GATTS_SRV_CCCD_NOTIFICATION) {
                    LOG_I(BT_GATTS,"Service changed not support notification! \r\n");
                    return BT_STATUS_FAIL;
                } else if (config_info[i].gatts_info.gatts_cccd_value == BT_ATT_OPCODE_HANDLE_VALUE_INDICATION) {
                    if (config_info[i].gatts_info.gatts_wait_att_rx_opcode == BT_ATT_OPCODE_HANDLE_VALUE_CONFIRMATION) {
                        LOG_I(BT_GATTS,"Waiting handle value confirmation! \r\n");
                        break;
                    }
                    config_info[i].gatts_info.gatts_wait_att_rx_opcode = BT_ATT_OPCODE_HANDLE_VALUE_CONFIRMATION;
                    noti_rsp->att_req.opcode = BT_ATT_OPCODE_HANDLE_VALUE_INDICATION;
                } else {
                    LOG_I(BT_GATTS,"Service changed CCCD wrong value! \r\n");
                    return BT_STATUS_FAIL;
                }
                    
                noti_rsp->att_req.handle = BT_SERVICE_CHANGED_VALUE_HANDLE;
                att_value[0] = (uint8_t) ((start_handle & 0x00FF) >> 0);
                att_value[1] = (uint8_t) ((start_handle & 0xFF00) >> 8);
                att_value[2] = (uint8_t) ((end_handle & 0x00FF) >> 0);
                att_value[3] = (uint8_t) ((end_handle & 0xFF00) >> 8);
                memcpy((void*)(noti_rsp->att_req.attribute_value), (void*)att_value, sizeof(att_value));

                noti_rsp->attribute_value_length = 3 + sizeof(att_value);
                status = bt_gatts_send_charc_value_notification_indication(config_info[i].conn_handle, noti_rsp);
                if (BT_STATUS_SUCCESS != status) {
                    config_info[i].gatts_info.gatts_wait_att_rx_opcode = 0;
                }
                return status;
            }
            return status;
        }
    }
    return status;
}

static void bt_gatts_service_indication_cnf(void *buff)
{
    uint8_t i;
    LOG_I(BT_GATTS, "%s: enter\r\n", __FUNCTION__);
    
    BT_ASSERT(buff != NULL);
    bt_handle_t *handle = (bt_handle_t*) buff;
    for (i =0; i< BT_GATTS_SRV_LE_CONNECTION_MAX; i++) {
        if (config_info[i].conn_handle == *handle) {
            config_info[i].gatts_info.gatts_wait_att_rx_opcode = 0;
        }
    }
}

static void bt_gatts_service_add_conn_info(void *buff)
{
    uint8_t i;
    LOG_I(BT_GATTS, "%s: enter\r\n", __FUNCTION__);

    bt_gap_le_connection_ind_t *connection_ind = (bt_gap_le_connection_ind_t *)buff;
    for (i =0; i< BT_GATTS_SRV_LE_CONNECTION_MAX; i++) {
        if (0 == config_info[i].conn_handle) {
            config_info[i].conn_handle = connection_ind->connection_handle;
            config_info[i].role = connection_ind->role;
            memset(&(config_info[i].gatts_info),0x00, sizeof(bt_gatts_service_change_info_t));
            break;
        }
    }
    if (i == BT_GATTS_SRV_LE_CONNECTION_MAX) {
        LOG_I(BT_GATTS, "Reach maximum connection, no empty buffer!\r\n");
    }
}


static void bt_gatts_service_delete_conn_info(void *buff)
{
    uint8_t i;
    bt_hci_evt_disconnect_complete_t *disconnect_complete;

    LOG_I(BT_GATTS, "%s: enter\r\n", __FUNCTION__);
    disconnect_complete = (bt_hci_evt_disconnect_complete_t*) buff;
    for (i = 0; i< BT_GATTS_SRV_LE_CONNECTION_MAX ; i++) {
        if (disconnect_complete->connection_handle == config_info[i].conn_handle) {
            memset(&(config_info[i]),0x00, sizeof(bt_gatts_config_info_t));
            break;
        }
    }
    if (i == BT_GATTS_SRV_LE_CONNECTION_MAX) {
        LOG_I(BT_GATTS, "Don't know connection info for deleting!\r\n");
    }
}


static void bt_gatts_service_clear_conn_info(void)
{
    uint8_t i;
    LOG_I(BT_GATTS, "%s: enter\r\n", __FUNCTION__);

    for (i = 0; i< BT_GATTS_SRV_LE_CONNECTION_MAX ; i++) {   
        memset(&(config_info[i]),0x00, sizeof(bt_gatts_config_info_t));
    }
}

/**
 * @brief   This function is a static callback to listen the stack event. Provide a user-defined callback.
 * @param[in] msg     is the callback message type.
 * @param[in] status  is the status of the callback message.
 * @param[in] buf     is the payload of the callback message.
 * @return            The status of this operation returned from the callback.
 */
static bt_status_t bt_gatts_service_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    LOG_I(BT_GATTS, "%s: status(0x%04x), msg(0x%04x)", __FUNCTION__, status, msg);

    if(status != BT_STATUS_SUCCESS) {
        return BT_STATUS_SUCCESS;
    }

    switch (msg)
    {
        case BT_GAP_LE_CONNECT_IND:
            bt_gatts_service_add_conn_info(buff);
            break;      

        case BT_GAP_LE_DISCONNECT_IND:
            bt_gatts_service_delete_conn_info(buff);
            break;

        case BT_GATTC_CHARC_VALUE_CONFIRMATION:
            bt_gatts_service_indication_cnf(buff);
            break;
            
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

static bt_status_t bt_gatts_service_event_callback_register(void)
{  
    LOG_I(BT_GATTS, "%s: enter\r\n", __FUNCTION__);  
    return bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_GAP | MODULE_MASK_GATT, (void *)bt_gatts_service_event_callback);   
}

void bt_gatts_service_init(void)
{
    LOG_I(BT_GATTS, "%s: enter\r\n", __FUNCTION__);  
    if (bt_gatts_initialized) {
        LOG_I(BT_DM, "bt_gatts_service_init fail, because it was initialized by others! \r\n");
        return;
    } else {
        bt_gatts_initialized = true; 
        bt_gatts_service_clear_conn_info();
        if (BT_STATUS_SUCCESS != bt_gatts_service_event_callback_register()) {
            LOG_I(BT_GATTS, "bt_gatts_service_init register callback fail! \r\n");  
        }
    }
}

/** Client Characteristic Configuration. */
static uint32_t bt_if_gatt_client_config_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    uint8_t i;
    LOG_I(BT_GATTS, "bt_if_gatt_client_config_callback, opcode:%d, size:%d, BT_GATTS_SRV_LE_CONNECTION_MAX is %d \r\n", rw, size, BT_GATTS_SRV_LE_CONNECTION_MAX);

    if (!bt_gatts_initialized)
        LOG_I(BT_GATTS,"bt_if_gatt_client_config_callback fail, please init bt_gatts_service first! \r\n");
        return 0;
        
    for (i=0; i<BT_GATTS_SRV_LE_CONNECTION_MAX; i++) {
        LOG_I(BT_GATTS, "bt_if_gatt_client_config_callback, handle:0x%04x, conn_handle:0x%04x \r\n", handle, config_info[i].conn_handle);     
        if ((handle > 0) && (handle == config_info[i].conn_handle)) {
            /** record for each connection. */                
            if (rw == BT_GATTS_CALLBACK_WRITE) {
                if (size != sizeof(uint16_t)) { //Size check
                    LOG_I(BT_GATTS, "size:%d \r\n", sizeof(uint16_t));
                    return 0;
                }
                config_info[i].gatts_info.gatts_cccd_value = *(uint16_t*)data;
                
                LOG_I(BT_GATTS, "bt_if_gatt_client_config_callback, cccd:0x%04x\r\n", config_info[i].gatts_info.gatts_cccd_value);
            } else if (rw == BT_GATTS_CALLBACK_READ) {
                if (size != 0) {
                    uint16_t *buf = (uint16_t*) data;
                    *buf = config_info[i].gatts_info.gatts_cccd_value;
                }
            }
            return sizeof(uint16_t);
        }
    }
    return 0;
}

/** Server Characteristic Configuration. */
static uint32_t bt_if_gatt_server_config_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    uint8_t i;
    LOG_I(BT_GATTS, "bt_if_gatt_server_config_callback, opcode:%d, size:%d \r\n", rw, size);

    if (!bt_gatts_initialized)
        LOG_I(BT_GATTS,"bt_if_gatt_server_config_callback fail, please init bt_gatts_service first! \r\n");
        return 0;
        
    for (i=0; i<BT_GATTS_SRV_LE_CONNECTION_MAX ; i++) {
        if ((handle > 0) && (handle == config_info[i].conn_handle)) {
            /** record for each connection. */
            if (rw == BT_GATTS_CALLBACK_WRITE){
                if (size != sizeof(config_info[i].gatts_info.gatts_sccd_value)){ //Size check
                    return 0;
                }
                config_info[i].gatts_info.gatts_sccd_value = *(uint16_t*)data;
            }else if (rw == BT_GATTS_CALLBACK_READ) {
                if (size!=0) {
                    uint16_t *buf = (uint16_t*) data;
                    *buf = config_info[i].gatts_info.gatts_sccd_value;
                }
            }
            return sizeof(config_info[i].gatts_info.gatts_sccd_value);
        }
    }
    return 0;
}

BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_gatt_primary_service, BT_GATT_UUID16_GATT_SERVICE);
BT_GATTS_NEW_CHARC_16(bt_if_gatt_char4_service_changed, BT_GATT_CHARC_PROP_INDICATE,
                BT_SERVICE_CHANGED_VALUE_HANDLE, BT_SIG_UUID16_SERVICE_CHANGED);
BT_GATTS_NEW_CHARC_VALUE_UINT32_WRITABLE(bt_if_gatt_service_changed, BT_SIG_UUID_SERVICE_CHANGED,
                             0x2, 0x0001050F);
BT_GATTS_NEW_CLIENT_CHARC_CONFIG(bt_if_gatt_client_config,
        BT_GATTS_REC_PERM_READABLE|BT_GATTS_REC_PERM_WRITABLE,
        bt_if_gatt_client_config_callback);
BT_GATTS_NEW_SERVER_CHARC_CONFIG(bt_if_gatt_server_config,
        BT_GATTS_REC_PERM_READABLE|BT_GATTS_REC_PERM_WRITABLE,
        bt_if_gatt_server_config_callback);

static const bt_gatts_service_rec_t *bt_if_gatt_service_rec[] = {
    (const bt_gatts_service_rec_t*) &bt_if_gatt_primary_service,
    (const bt_gatts_service_rec_t*) &bt_if_gatt_char4_service_changed,
    (const bt_gatts_service_rec_t*) &bt_if_gatt_service_changed,
    (const bt_gatts_service_rec_t*) &bt_if_gatt_client_config,
    (const bt_gatts_service_rec_t*) &bt_if_gatt_server_config   
    };

const bt_gatts_service_t bt_if_gatt_service = {
    .starting_handle = 0x0011,
    .ending_handle = 0x0015,     
    .required_encryption_key_size = 7,
    .records = bt_if_gatt_service_rec
    };

#else
/*---------------------------------------------*/
BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_gatt_primary_service, BT_GATT_UUID16_GATT_SERVICE);
static const bt_gatts_service_rec_t *bt_if_gatt_service_rec[] = {
    (const bt_gatts_service_rec_t*) &bt_if_gatt_primary_service
    };

const bt_gatts_service_t bt_if_gatt_service = {
    .starting_handle = 0x0011,
    .ending_handle = 0x0011,
    .required_encryption_key_size = 7,
    .records = bt_if_gatt_service_rec
    };
/*---------------------------------------------*/
#endif


