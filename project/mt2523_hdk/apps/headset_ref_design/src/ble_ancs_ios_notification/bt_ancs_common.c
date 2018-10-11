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


#include "bt_hci.h"
#include "ble_ancs.h"
#include "bt_gattc.h"
#include "bt_debug.h"
#include "bt_gatts.h"
#include "bt_ancs_common.h"
#include "bt_callback_manager.h"
#include "syslog.h"


static ble_ancs_srv_content_t g_ancs_srv;


/**
 * @brief   This function is a static callback for the application to listen to the event. Provide a user-defined callback.
 * @param[in] msg     is the callback message type.
 * @param[in] status  is the status of the callback message.
 * @param[in] buf     is the payload of the callback message.
 * @return            The status of this operation returned from the callback.
 */
static bt_status_t ble_ancs_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff)
{

    /*Dont' remove ble_ancs_event_callback, make sure ancs lib can receive bt stack events*/
    ble_ancs_event_callback(msg, status, buff);
    
    switch (msg)
    {
        case BT_POWER_ON_CNF:
            break;
            
        case BT_GAP_LE_CONNECT_IND:
            break;

        case BT_GAP_LE_BONDING_COMPLETE_IND:
           if(status != BT_STATUS_SUCCESS) {
                LOG_I(ANCS, "[ANCS]BT_GAP_LE_BONDING_COMPLETE_IND, status = %d\n", status);
           } else {
               const bt_gap_le_bonding_complete_ind_t *ind = (bt_gap_le_bonding_complete_ind_t*)buff;
               bt_gattc_discovery_status_t result;

               /*You can start service disvoery when you need*/
               result = bt_gattc_discovery_start(ind->handle);
               LOG_I(ANCS, "[ANCS]start service discover, result = %d\n", result);
           }
           break;
            
        case BT_GATTC_READ_CHARC:
            if (status == BT_STATUS_SUCCESS) {
                bt_gattc_read_rsp_t *rsp = (bt_gattc_read_rsp_t *)buff;
                uint8_t data_source_uuid[16] = {0xfb, 0x7b, 0x7c, 0xce, 0x6a, 0xb3, 0x44, 0xbe, 0xb5, 0x4b, 0xd6, 0x24, 0xe9, 0xc6, 0xea, 0x22};
            
                if (g_ancs_srv.status == ANCS_COMM_STATUS_READ_NOTIF_SOURCE) {
                    uint8_t i;
                    
                    LOG_I(ANCS, "notificaiton source enabled = %d\n", rsp->att_rsp->attribute_value[0]);

                    /*read data source descriptor*/
                    for (i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) {
                        if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, data_source_uuid, 16)) {
                            break;
                        }
                    }

                    if (i < BLE_ANCS_MAX_CHARC_NUMBER) {
                        BT_GATTC_NEW_READ_CHARC_REQ(req, g_ancs_srv.service.charateristics[i].descriptor[0].handle);
                        bt_gattc_read_charc(rsp->connection_handle, &req);
                        g_ancs_srv.status = ANCS_COMM_STATUS_READ_DATA_SOURCE;
                    }
                } else if (g_ancs_srv.status == ANCS_COMM_STATUS_READ_DATA_SOURCE){
                    /*notify app about the finish*/
                    ble_ancs_event_t notif_evt;
                    
                    LOG_I(ANCS, "data source enabled = %d\n", rsp->att_rsp->attribute_value[0]);
                    
                    notif_evt.evt_type = BLE_ANCS_EVENT_CONNECTED;
                    notif_evt.connection_handle = rsp->connection_handle;                        
                    notif_evt.result = BT_STATUS_SUCCESS;
                    g_ancs_srv.evt_handler(&notif_evt);
                    g_ancs_srv.status = ANCS_COMM_STATUS_READY;
                } else {
                    LOG_I(ANCS, "BT_GATTC_READ_CHARC: action = %d\r\n", g_ancs_srv.status);
                }
            } else {
                LOG_I(ANCS, "BT_GATTC_READ_CHARC:status = 0x%x\n", status);
            }
            break;

        case BT_GATTC_WRITE_CHARC:
            if (status == BT_STATUS_SUCCESS) {
                bt_gattc_write_rsp_t *rsp = (bt_gattc_write_rsp_t *)buff;
                ble_ancs_event_t notif_evt;
                
                notif_evt.evt_type = BLE_ANCS_EVENT_REQUEST_COMPLETED;
                notif_evt.result = BT_STATUS_SUCCESS;
                notif_evt.connection_handle = rsp->connection_handle;
                g_ancs_srv.evt_handler(&notif_evt);
            } else {
                LOG_I(ANCS, "BT_GATTC_WRITE_CHARC: status = %x\r\n", status);
            }
            break;

        case BT_GATTC_CHARC_VALUE_NOTIFICATION:
            if (status == BT_STATUS_SUCCESS) {
                bt_gatt_handle_value_notification_t *data = (bt_gatt_handle_value_notification_t *)buff;
                ble_ancs_event_t ancs_evt;
                uint8_t i, m;
                uint8_t noti_source_uuid[16] = {0xbd, 0x1d, 0xa2, 0x99, 0xe6, 0x25, 0x58, 0x8c, 0xd9, 0x42, 0x01, 0x63, 0x0d, 0x12, 0xbf, 0x9f};
                uint8_t data_source_uuid[16] = {0xfb, 0x7b, 0x7c, 0xce, 0x6a, 0xb3, 0x44, 0xbe, 0xb5, 0x4b, 0xd6, 0x24, 0xe9, 0xc6, 0xea, 0x22};
                
                for(i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) {
                   if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, noti_source_uuid, 16)) {
                       break;
                   }
                }

                 for (m = 0; m < BLE_ANCS_MAX_CHARC_NUMBER; m++) {
                    if (!memcmp(g_ancs_srv.service.charateristics[m].char_uuid.uuid.uuid, data_source_uuid, 16)) {
                        break;
                    }

                }
                if (data->att_rsp->handle == g_ancs_srv.service.charateristics[i].value_handle) {

                    ancs_evt.result = ble_ancs_parse_notification(&ancs_evt.data.notification, data->length - 3, data->att_rsp->attribute_value);
                    ancs_evt.connection_handle = data->connection_handle;
                    ancs_evt.evt_type = BLE_ANCS_EVENT_IOS_NOTIFICATION;
                    g_ancs_srv.evt_handler(&ancs_evt);
                    
                } else if (data->att_rsp->handle == g_ancs_srv.service.charateristics[m].value_handle) {
                    /*parse attribute*/
                    bt_status_t result;
                    
                    result = ble_ancs_parse_attributes(data->connection_handle, data->length - 3, data->att_rsp->attribute_value, &ancs_evt.data.attribute);

                    /*if status == BT_STATUS_PENDING, wait for another BT_GATTC_CHARC_VALUE_NOTIFICATION */
                    
                    if (BT_STATUS_SUCCESS == result || BT_STATUS_FAIL == result) {
                        //send event to app
                        ancs_evt.result = result;
                        ancs_evt.connection_handle = data->connection_handle;
                        if (ancs_evt.data.attribute.command_id == 0) {
                            ancs_evt.evt_type = BLE_ANCS_EVENT_NOTIFICATION_ATTRIBUTE;
                        } else if (ancs_evt.data.attribute.command_id == 1) {
                            ancs_evt.evt_type = BLE_ANCS_EVENT_APP_ATTRIBUTE;
                        }
                        
                        LOG_I(ANCS, "Parse attribute done, result = %d, event = %d\r\n", result, ancs_evt.evt_type);
                        g_ancs_srv.evt_handler(&ancs_evt);
                    }    
                }
            }else {
                LOG_I(ANCS, "BT_GATTC_CHARC_VALUE_NOTIFICATION: status = %x\r\n", status);
            }
            break;

        case BT_GAP_LE_DISCONNECT_IND:
            if (status == BT_STATUS_SUCCESS) {
                bt_gap_le_disconnect_ind_t * dis_ind = (bt_gap_le_disconnect_ind_t *)buff;
                ble_ancs_event_t ancs_evt;
                
                memset(&ancs_evt, 0, sizeof(ble_ancs_event_t));
                ancs_evt.result = BT_STATUS_SUCCESS;
                ancs_evt.connection_handle = dis_ind->connection_handle;
                ancs_evt.evt_type = BLE_ANCS_EVENT_DISCONNECTED;
                g_ancs_srv.evt_handler(&ancs_evt);
            } else {
                LOG_I(ANCS, "BT_GAP_LE_DISCONNECT_IND: status = %x\r\n", status);
            }
            break;
         default:
             break;
    }

    return BT_STATUS_SUCCESS;
}


static void ble_ancs_gattc_callback(bt_gattc_discovery_event_t *event)
{
    
    switch (event->event_type) {
        case BT_GATTC_DISCOVERY_EVENT_COMPLETE:
            {
                uint8_t noti_source_uuid[16] = {0xbd, 0x1d, 0xa2, 0x99, 0xe6, 0x25, 0x58, 0x8c, 0xd9, 0x42, 0x01, 0x63, 0x0d, 0x12, 0xbf, 0x9f};
                uint8_t data_source_uuid[16] = {0xfb, 0x7b, 0x7c, 0xce, 0x6a, 0xb3, 0x44, 0xbe, 0xb5, 0x4b, 0xd6, 0x24, 0xe9, 0xc6, 0xea, 0x22};
                uint8_t i;
                ble_ancs_charc_t charc[BLE_ANCS_MAX_CHARC_NUMBER];
                
                LOG_I(ANCS, "DISCOVER_COMPLETE: start = %d, end = %d, char_num = %d, handle = %d, %d, %d\n", g_ancs_srv.service.start_handle, 
                    g_ancs_srv.service.end_handle, g_ancs_srv.service.char_count_found, g_ancs_srv.service.charateristics[0].handle, 
                    g_ancs_srv.service.charateristics[1].handle, g_ancs_srv.service.charateristics[2].handle);

    
                for (i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) {

                    if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, data_source_uuid, 16)) {
                        (charc + i)->uuid_type = BLE_ANCS_UUID_DATA_SOURCE;

                    } else if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, noti_source_uuid, 16)) {
                        (charc + i)->uuid_type = BLE_ANCS_UUID_NOTIFICATION_SOURCE;

                    } else {
                        (charc + i)->uuid_type = BLE_ANCS_UUID_CONTROL_POINT;

                    }
                    (charc + i)->value_handle = g_ancs_srv.service.charateristics[i].value_handle;
                    (charc + i)->desc_handle = g_ancs_srv.service.charateristics[i].descriptor[0].handle;
                }
                
                ble_ancs_init(event->conn_handle, charc);


                /*start to read notification source descriptor*/
                for(i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) {
                    LOG_I(ANCS, "read notification source, uuid_type = %d\n", g_ancs_srv.service.charateristics[i].char_uuid.type);

                    if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, noti_source_uuid, 16)) {
                        break;
                    }
                }

                if (i < BLE_ANCS_MAX_CHARC_NUMBER) {
                    BT_GATTC_NEW_READ_CHARC_REQ(req, g_ancs_srv.service.charateristics[i].descriptor[0].handle);
                    bt_gattc_read_charc(event->conn_handle, &req);
                    g_ancs_srv.status = ANCS_COMM_STATUS_READ_NOTIF_SOURCE;
                }
            }
            break;
            
        default:
            break;
   }
}


void ble_ancs_start(ble_ancs_event_handler_t evt_handler)
{
    uint8_t i;
    
    ble_uuid_t ancs_srv_uuid = {
        .type = BLE_UUID_TYPE_128BIT,
        .uuid.uuid = {0xd0, 0x00, 0x2d, 0x12, 0x1e, 0x4b, 0x0f, 0xa4, 0x99, 0x4e, 0xce, 0xb5, 0x31, 0xf4, 0x05, 0x79}
    };

    memset(&g_ancs_srv, 0, sizeof(ble_ancs_srv_content_t));

    g_ancs_srv.evt_handler = evt_handler;

    bt_callback_manager_register_callback(bt_callback_type_app_event,
                                              (uint32_t)(MODULE_MASK_GAP | MODULE_MASK_GATT | MODULE_MASK_SYSTEM),
                                              (void *)ble_ancs_event_handler);

    for (i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) { 
        g_ancs_srv.charc[i].descriptor_count = 1;
        g_ancs_srv.charc[i].descriptor = &g_ancs_srv.descrp[i];
    }

    g_ancs_srv.service.characteristic_count = BLE_ANCS_MAX_CHARC_NUMBER;
    g_ancs_srv.service.charateristics = g_ancs_srv.charc;

    bt_gattc_discovery_service_register(&ancs_srv_uuid, &g_ancs_srv.service, ble_ancs_gattc_callback);
}


void ble_ancs_end(void)
{

    bt_callback_manager_deregister_callback(bt_callback_type_app_event, (void *)ble_ancs_event_handler);

    memset(&g_ancs_srv, 0, sizeof(ble_ancs_srv_content_t));
}

