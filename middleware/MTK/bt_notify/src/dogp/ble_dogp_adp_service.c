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
 * This file implements dogp adaptor service main function
 *
 ****************************************************************************/
//#ifdef MTK_BLE_DOGP_ENABLE

#include <stdio.h>
#include <string.h>
#include "bt_gatts.h"
#include "bt_gattc.h"
#include "bt_gap_le.h"
#include "ble_dogp_service.h"
#include "ble_dogp_adp_service.h"
#include "bt_type.h"
#include "bt_debug.h"
#include "ble_dogp_common.h"
#include "syslog.h"

log_create_module(DOGP_ADP, PRINT_LEVEL_INFO);

/************************************************
*   Macro
*************************************************/
#define BLE_DOGP_ADP_SEND_BUFFER_SIZE           (1024 * 2)
#define BLE_DOGP_ADP_RECEIVE_BUFFER_SIZE        (1024 * 2)

/************************************************
*   Structures
*************************************************/
typedef struct {
    uint16_t                        conn_handle;
    ble_dogp_adp_common_callback_t  callback;
} ble_dogp_adp_cntx_t;

typedef struct {
    bool                            need_ready2write;
    bool                            first_write;
    uint16_t                        notify_enabled;
    uint16_t                        conn_handle;
    ble_addr_t                      peer_addr; /**< Address information of the remote device. */
    uint8_t                         send_buffer[BLE_DOGP_ADP_SEND_BUFFER_SIZE];
    uint32_t                        send_buffer_start;
    uint32_t                        send_buffer_length;
    uint8_t                         receive_buffer[BLE_DOGP_ADP_RECEIVE_BUFFER_SIZE];
    uint32_t                        receive_buffer_start;
    uint32_t                        receive_buffer_length;
} ble_dogp_adp_buffer_t;


/************************************************
*   Global variables
*************************************************/
static ble_dogp_adp_cntx_t g_dogp_adp_cntx = {0};
static ble_dogp_adp_buffer_t g_dogp_adp_buffer = {0};

static uint16_t g_read_len = 0;
static const char *notify_string = "ios indication";

/************************************************
*   static utilities
*************************************************/
static bt_status_t ble_dogp_adp_services_init(void);   
extern int32_t ble_dogp_adp_update_connection_interval(uint16_t conn_handle);
static uint32_t ble_dogp_adp_read_evt_handler(uint16_t conn_handle, void *read_buffer, uint16_t read_length, ble_dogp_service_evt_t *p_evt);
static uint32_t ble_dogp_adp_write_evt_handler(uint16_t conn_handle, ble_dogp_service_evt_t *p_evt);


/************************************************
*   Functions
*************************************************/


/**
 * @brief Function for application to write data to the send buffer.
 */
uint32_t ble_dogp_adp_write_data(uint16_t conn_handle, uint8_t *buffer, uint32_t size)
{
    uint32_t send_size = 0;
    ble_dogp_adp_buffer_t *buffer_t = &g_dogp_adp_buffer;
    //ble_dogp_service_read_characteristic_t p_dogps_notify_to_read;

    if ((conn_handle != 0) && (conn_handle == buffer_t->conn_handle)) {
        bt_status_t status;
        if (size > (BLE_DOGP_ADP_SEND_BUFFER_SIZE - buffer_t->send_buffer_length)) {
            send_size = BLE_DOGP_ADP_SEND_BUFFER_SIZE - buffer_t->send_buffer_length;
            buffer_t->need_ready2write = true;
            LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_write_data: write buffer is full\r\n");
        } else {
            send_size = size;
        }
        memcpy(&(buffer_t->send_buffer[buffer_t->send_buffer_length]), buffer, send_size);
        buffer_t->send_buffer_length += send_size;

        /**notify remote device to read data*/
        //p_dogps_notify_to_read.read__field.p_str = notify_string;
        //p_dogps_notify_to_read.read__field.length = sizeof(notify_string);
        ble_dogp_adp_update_connection_interval(conn_handle);
        status = ble_dogp_service_read_characteristic_send(conn_handle, (uint8_t *)notify_string);
        if (BT_STATUS_SUCCESS != status) {
            LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_write_data fail, status is %d \r\n", status);
        } else {
            LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_write_data success, send_size is [%d] \r\n", send_size);
        }

        return send_size;
    }
    LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_write_data: conn id error [%d]\r\n", conn_handle);
    return 0;
}

/**
 * @brief Function for application to read data from the receive buffer.
 */
uint32_t ble_dogp_adp_read_data(uint16_t conn_handle, uint8_t *buffer, uint32_t size)
{
    uint32_t read_size = 0;
    ble_dogp_adp_buffer_t *buffer_t = &g_dogp_adp_buffer;

    if ((conn_handle != 0) && (conn_handle == buffer_t->conn_handle)) {
        if (buffer_t->receive_buffer_length > size) {
            read_size = size;
        } else {
            read_size = buffer_t->receive_buffer_length;
        }

        if (0 == read_size) {
            LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_read_data: read buffer is null\r\n");
            return 0;
        }
        memcpy(buffer, buffer_t->receive_buffer, read_size);
        if (buffer_t->receive_buffer_length > read_size) {
            memmove(buffer_t->receive_buffer, &(buffer_t->receive_buffer[read_size]), (buffer_t->receive_buffer_length - read_size));
            buffer_t->receive_buffer_length -= read_size;
        } else {
            buffer_t->receive_buffer_length = 0;
            memset(buffer_t->receive_buffer, 0, sizeof(buffer_t->receive_buffer));
        }

        LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_read_data: read_size is [%d]\r\n", read_size);
        return read_size;
    }
    LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_read_data: conn id error [%d]\r\n", conn_handle);
    return 0;
}

/**
 * @brief Function for handling the DOGP Service events.
 *
 * @details     This function will be called for all DOGP Service read events which are passed to
 *			    the application.
 *
 * @param[in]   conn_handle                  Connection handle.
 * @param[in]   read_length                  Length of the data.
 * @param[in]   *read_buffer                 Data buffer.
 * @param[in] 	p_evt	                     Event received from the DOGP Service.*
 *
 * @return      length of the data have been read.
 */
static uint32_t ble_dogp_adp_read_evt_handler(uint16_t conn_handle, void *read_buffer, uint16_t read_length, ble_dogp_service_evt_t *p_evt)
{
    uint32_t r_size = 0;
    ble_dogp_adp_buffer_t *buffer_t = &g_dogp_adp_buffer;
    LOG_I(DOGP_ADP, "ble_dogp_adp_read_evt_handler: event type is [%d], read_length is %d\r\n", p_evt->evt_type, read_length);
    
    if ((conn_handle != 0) && (conn_handle == buffer_t->conn_handle)) {
        switch (p_evt->evt_type) {
            case BLE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_CCCD_READ:{ 
                if (0 != read_length) { //Size check
                    uint16_t *buf = (uint16_t*) read_buffer;
                    *buf = buffer_t->notify_enabled;
                } 
                LOG_I(DOGP_ADP, "read characteristic cccd value = %d, read_length = %d\r\n", buffer_t->notify_enabled, read_length);
                return sizeof(buffer_t->notify_enabled);
            }
                
            case BLE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_READ: {
                /**send data to remote device*/
                if (0 == read_length) {
                    
                    LOG_I(DOGP_ADP, "read offset = %d \r\n", p_evt->params.dogp_service_read_characteristic_read_offset);
                    if (0 == p_evt->params.dogp_service_read_characteristic_read_offset) {
                        g_read_len = buffer_t->send_buffer_length;   
                    }
                    LOG_I(DOGP_ADP, "g_read_len = %d \r\n", g_read_len);
                    return g_read_len;
                } else {
                    uint32_t read_size = 0;
                    uint32_t mtu_size;
                    uint8_t *read_rsp_value = (uint8_t*)read_buffer;

                    mtu_size = bt_gattc_get_mtu((bt_handle_t)conn_handle);
                
                    LOG_I(DOGP_ADP, "mtu = %d, read_length = %d, send_buf_length = %d\r\n", mtu_size, read_length, buffer_t->send_buffer_length);
                    if ((mtu_size - 1) < read_length) {
                        read_size = mtu_size - 1;
                    } else {
                        read_size = read_length;
                    }
                    if (0 == read_size) {
                        LOG_I(DOGP_ADP, "[BLE_DOGP] read characteristic: read_size is 0!\r\n");
                        return 0;
                    }
                    
                    ble_dogp_adp_update_connection_interval(conn_handle);
                    memcpy(read_rsp_value, buffer_t->send_buffer, read_size);

                    if (buffer_t->send_buffer_length > read_size) {
                        memmove(buffer_t->send_buffer, &(buffer_t->send_buffer[read_size]), (buffer_t->send_buffer_length - read_size));
                        buffer_t->send_buffer_length -= read_size;                  
                    } else {
                        buffer_t->send_buffer_length = 0;
                        memset(buffer_t->send_buffer, 0, sizeof(buffer_t->send_buffer));
                    }
                    LOG_I(DOGP_ADP, "[BLE_DOGP] read characteristic: read_size[%d], send_buffer_length[%d]\r\n",
                          read_size, buffer_t->send_buffer_length);
                    if (buffer_t->need_ready2write) {
                        ble_dogp_adp_ready_to_write_t ready_to_write;
                        memset(&ready_to_write, 0, sizeof(ble_dogp_adp_ready_to_write_t));

                        ready_to_write.conn_handle = conn_handle;
                        memcpy(&(ready_to_write.bdaddr), &(buffer_t->peer_addr), 6);

                        LOG_I(DOGP_ADP, "[BLE_DOGP] ble_dogp_adp_evt_handler: ready to write send\r\n");
                        g_dogp_adp_cntx.callback(BLE_DOGP_ADP_EVENT_READY_TO_WRITE_IND, (void *)&ready_to_write);
                        buffer_t->need_ready2write = false;
                    }
                    if (buffer_t->send_buffer_length > 0) {
                        ble_dogp_service_read_characteristic_send(conn_handle, (uint8_t *)notify_string);
                    }   
                    LOG_I(DOGP_ADP, "read_size = %d \r\n", read_size);
                    return read_size;
                }
            }

            default :
                break;
        }
    }
    return r_size;
}

/**
 *  @brief Function for handling the DOGP Service events.
 *
 *  @details     This function will be called for all DOGP Service write events which are passed to
 *			    the application.
 *
 *  @param[in]	conn_handle	 connection handle.
 *  @param[in] 	p_evt	     Event received from the DOGP Service.
 *
 *  @return      length of the data have been wrote.
 */
static uint32_t ble_dogp_adp_write_evt_handler(uint16_t conn_handle, ble_dogp_service_evt_t *p_evt)
{
    uint32_t w_size = 0;
    ble_dogp_adp_buffer_t *buffer_t = &g_dogp_adp_buffer;
    LOG_I(DOGP_ADP, "write evt type= %d\r\n", p_evt->evt_type);

    if ((conn_handle != 0) && (conn_handle == buffer_t->conn_handle)) {
        switch (p_evt->evt_type) {
            case BLE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_CCCD_WRITE: {
                buffer_t->notify_enabled = p_evt->params.dogp_service_read_characteristic_cccd_value;      
                LOG_I(DOGP_ADP, "write characteristic cccd value = %d\r\n", buffer_t->notify_enabled);
                return sizeof(buffer_t->notify_enabled);
            }
                
            case BLE_DOGP_SERVICE_WRITE_CHARACTERISTIC_EVT_WRITE: {
                /**remote write & notify app ready to read*/
                LOG_I(DOGP_ADP, "write length= %d\r\n", p_evt->params.dogp_service_write_characteristic.write_field.length);
                if (p_evt->params.dogp_service_write_characteristic.write_field.length >
                        (BLE_DOGP_ADP_RECEIVE_BUFFER_SIZE - buffer_t->receive_buffer_length)) {
                    LOG_I(DOGP_ADP, "write characteristic: buffer full error\r\n");
                    return 0; /**means fail, buffer full*/
                }

                ble_dogp_adp_update_connection_interval(conn_handle);
                memcpy(&(buffer_t->receive_buffer[buffer_t->receive_buffer_length]),
                       p_evt->params.dogp_service_write_characteristic.write_field.p_str,
                       p_evt->params.dogp_service_write_characteristic.write_field.length);
                buffer_t->receive_buffer_length += p_evt->params.dogp_service_write_characteristic.write_field.length;

                if (g_dogp_adp_cntx.callback) {
                    ble_dogp_adp_ready_to_read_t ready_to_read;
                    memset(&ready_to_read, 0, sizeof(ble_dogp_adp_ready_to_read_t));

                    ready_to_read.conn_handle= conn_handle;
                    memcpy(&(ready_to_read.bdaddr), &(buffer_t->peer_addr), 6);

                    if (false == buffer_t->first_write) {
                        ble_dogp_adp_connect_t connect_param;
                        memset(&connect_param, 0, sizeof(ble_dogp_adp_connect_t));

                        buffer_t->first_write = true;
                        connect_param.conn_handle = conn_handle;
                        connect_param.conn_result = 0;
                        memcpy(&connect_param.bdaddr, &buffer_t->peer_addr, 6);
                        LOG_I(DOGP_ADP, "write characteristic: connect and first write\r\n");
                        g_dogp_adp_cntx.callback(BLE_DOGP_ADP_EVENT_CONNECT_IND, (void *)&connect_param);
                    }

                    LOG_I(DOGP_ADP, "write characteristic: write size = %d \r\n", p_evt->params.dogp_service_write_characteristic.write_field.length);
                    g_dogp_adp_cntx.callback(BLE_DOGP_ADP_EVENT_READY_TO_READ_IND, (void *)&ready_to_read);

                    return p_evt->params.dogp_service_write_characteristic.write_field.length;
                }
                return 0;
            }

            default :
                break;
        }
    }
    return w_size;
}

/**
 *  @brief Function for handling the Application's BLE Stack events.
 *
 *  @param[in] msg    Stack event type.
 *  @param[in] *buff  Stack event parameters.
 */
bt_status_t ble_dogp_adp_on_ble_evt(bt_msg_type_t msg, void *buff)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    ble_dogp_adp_buffer_t *buffer_t = &g_dogp_adp_buffer;
   
    LOG_I(DOGP_ADP, "ble_dogp_adp_on_ble_evt msg = 0x%04x \r\n", msg);
    switch (msg) {                     
        case BT_GAP_LE_CONNECT_IND: { 
            bt_gap_le_connection_ind_t *connection_ind = (bt_gap_le_connection_ind_t *)buff;
            uint16_t conn_handle = connection_ind->connection_handle;
            /**< only support single link. */
            if (0 == buffer_t->conn_handle) {
                buffer_t->conn_handle = conn_handle;
                memcpy(buffer_t->peer_addr, connection_ind->peer_addr.addr, sizeof(connection_ind->peer_addr.addr));
                LOG_I(DOGP_ADP, "connection handle=0x%04x", conn_handle);
            }
        }
        break;

        case BT_GAP_LE_DISCONNECT_IND: {
            bt_hci_evt_disconnect_complete_t *connection_ind = (bt_hci_evt_disconnect_complete_t *)buff;
            uint16_t conn_handle = connection_ind->connection_handle;
            if ((conn_handle != 0) && (conn_handle == buffer_t->conn_handle)) {
                if (g_dogp_adp_cntx.callback) {
                    ble_dogp_adp_disconnect_t disconnect_param;
                    memset(&disconnect_param, 0, sizeof(ble_dogp_adp_disconnect_t));

                    disconnect_param.conn_handle = conn_handle;
                    disconnect_param.disconn_result = 0;
                    g_dogp_adp_cntx.callback(BLE_DOGP_ADP_EVENT_DISCONNECT_IND, (void *)&disconnect_param);

                    LOG_I(DOGP_ADP, "disconnec handle=0x%04x", conn_handle);
                }
                memset(buffer_t, 0, sizeof(ble_dogp_adp_buffer_t));
                memset(buffer_t->peer_addr, 0, sizeof(buffer_t->peer_addr));
            }
        }
        break;

        default:
            break;
    }
    return status;
}

/**
 * @brief Function for initializing services that will be used by the application.
 */
static bt_status_t ble_dogp_adp_services_init(void)
{
    ble_dogp_service_init_t dogps_init_t;
    LOG_I(DOGP_ADP, "ble_dogp_adp_services_init");

    /** Initialize DOGP Service.*/
    memset(&dogps_init_t, 0, sizeof(ble_dogp_service_init_t));
    dogps_init_t.r_evt_handler = ble_dogp_adp_read_evt_handler;
    dogps_init_t.w_evt_handler = ble_dogp_adp_write_evt_handler;
    return ble_dogp_service_init(&dogps_init_t);
}


/**
 * @brief Function for application main entry.
 */
int32_t ble_dogp_adp_init(ble_dogp_adp_common_callback_t app_callback)
{ 
    if (app_callback) {
        if (NULL == g_dogp_adp_cntx.callback) {
            /**Initialize.*/
            memset(&g_dogp_adp_cntx, 0, sizeof(ble_dogp_adp_cntx_t));
            memset(&g_dogp_adp_buffer, 0, sizeof(ble_dogp_adp_buffer_t));
            g_dogp_adp_cntx.callback = app_callback;
            
            LOG_I(DOGP_ADP, "init app_callback=0x%04x", app_callback);
            ble_dogp_adp_services_init();/*add to main file*/
            return ble_dogp_event_callback_init();
        }
    }
    return -1;/**means failed*/
}

int32_t ble_dogp_adp_deinit(ble_dogp_adp_common_callback_t app_callback)
{   
    if (app_callback) {
        if (app_callback == g_dogp_adp_cntx.callback) {
            /**Initialize.*/
            memset(&g_dogp_adp_cntx, 0, sizeof(ble_dogp_adp_cntx_t));
            memset(&g_dogp_adp_buffer, 0, sizeof(ble_dogp_adp_buffer_t));
            
            LOG_I(DOGP_ADP, "deinit app_callback=0x%04x", app_callback);
            return ble_dogp_service_deinit();
        }
    }
    return -1;/**means failed*/
}



//#endif /*MTK_BLE_DOGP_ENABLE*/

