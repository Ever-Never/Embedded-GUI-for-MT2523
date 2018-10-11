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
#include "bt_spp_main.h"
#include "bt_spp_server.h"

#define BT_SPP_STANDARD_UUID    0x00,0x00,0x11,0x01,0x00,0x00,0x10,0x00,   \
                                 0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFB
 /****************************************************************************
  *
  * ROMable data
  *SPP example server sdp record
  ****************************************************************************/
 static const uint8_t bt_spp_app_service_class_id[] =
 {
     BT_SPP_SDP_ATTRIBUTE_UUID_LENGTH,
     BT_SPP_SDP_ATTRIBUTE_UUID(BT_SPP_STANDARD_UUID)
 };
 
 static const uint8_t bt_spp_app_protocol_descriptor_list[] =
 {
     BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(BT_SPP_SERVER_ID_START)
 };
 
 static const uint8_t bt_spp_app_browse_group[] =
 {
     BT_SPP_SDP_ATTRIBUTE_PUBLIC_BROWSE_GROUP
 };
     
 static const uint8_t bt_spp_app_language[] =
 {
     BT_SPP_SDP_ATTRIBUTE_LANGUAGE
 };
 
 static const uint8_t bt_spp_app_service_name[] =
 {
     BT_SPP_SDP_ATTRIBUTE_SIZE_OF_SERVICE_NAME(9),
     'S', 'P', 'P', 'S', 'E', 'R', 'V', 'E', 'R'
 };
 
 static const bt_sdps_attribute_t bt_spp_app_sdp_attributes[] =
 {
     /* Service Class ID List attribute */
     BT_SPP_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(bt_spp_app_service_class_id),
     /* Protocol Descriptor List attribute */
     BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESC_LIST(bt_spp_app_protocol_descriptor_list),
     /* Public Browse Group Service */
     BT_SPP_SDP_ATTRIBUTE_BROWSE_GROUP_LIST(bt_spp_app_browse_group),
     /* Language Base ID List attribute */
     BT_SPP_SDP_ATTRIBUTE_LANGUAGE_BASE_LIST(bt_spp_app_language),
     /* Serial Port Profile Service Name */
     BT_SPP_SDP_ATTRIBUTE_SERVICE_NAME(bt_spp_app_service_name)
 };
 
 
 static const bt_sdps_record_t bt_spp_app_sdp_record = 
 {
     .attribute_list_length = sizeof(bt_spp_app_sdp_attributes),
     .attribute_list = bt_spp_app_sdp_attributes,
 };
 
 /*implement SDP Record weak functions bt_sdps_get_customized_record*/
 static const bt_sdps_record_t *sdps_spp_app_record[]= {
     &bt_spp_app_sdp_record
 };
 
 uint8_t bt_sdps_get_customized_record(const bt_sdps_record_t *** record_list)
 {
     *record_list = sdps_spp_app_record;
     return sizeof(sdps_spp_app_record) /sizeof(bt_sdps_record_t*);
 }

app_spp_server_cntx_t spp_server_cntx;
app_spp_server_cntx_t *spp_server_cntx_p = &spp_server_cntx;

void spp_server_connect_ind(bt_status_t status, bt_spp_connect_ind_t* conn_ind_p)
{
    LOG_I(bt_spp_example,"handle:0x%x", conn_ind_p->handle);
    spp_server_cntx.spp_handle = conn_ind_p->handle;
    bt_status_t result = bt_spp_connect_response(spp_server_cntx.spp_handle, true);
    LOG_I(bt_spp_example,"spp server response result: 0x%x", result);
}

void spp_server_connect_cnf(bt_status_t status, bt_spp_connect_cnf_t* conn_cnf_p)
{
    LOG_I(bt_spp_example,"handle:0x%x, max_packet_length: %d", conn_cnf_p->handle, conn_cnf_p->max_packet_length);
    if(status == BT_STATUS_SUCCESS) {
        spp_server_cntx_p->is_connected = true;
        spp_server_cntx_p->max_packet_length = conn_cnf_p->max_packet_length;
        
        //The string will be sent to remote device.
        memcpy(spp_server_cntx_p->tx_data_buf, (uint8_t*)SPP_SERVER_STRING, strlen(SPP_SERVER_STRING));
        spp_server_cntx_p->tx_data_length = 0;
    }  else {
        memset(spp_server_cntx_p,  0x0, sizeof(app_spp_server_cntx_t));
    }
    return;
}

void spp_server_disconnect_ind(bt_status_t status, bt_spp_disconnect_ind_t *disc_ind_p)
{
    LOG_I(bt_spp_example,"handle:0x%x, result:0x%x", disc_ind_p->handle,status);    
    spp_server_cntx_p->is_connected = false;
    memset(spp_server_cntx_p, 0x0, sizeof(app_spp_server_cntx_t));
    return;
}

void spp_server_disconnect_request(void)
{
    bt_status_t result = bt_spp_disconnect(spp_server_cntx_p->spp_handle);
    LOG_I(bt_spp_example,"handle:0x%x, result:0x%x", spp_server_cntx_p->spp_handle, result);
    return;
}

void spp_server_print_received_data(void)
{
        uint32_t i = 0;
        LOG_I(bt_spp_example,"print server recevied data start:%d", spp_server_cntx_p->rx_data_length);
        for (i = 0; i < spp_server_cntx_p->rx_data_length; i++) {
            LOG_I(bt_spp_example, "%c", spp_server_cntx_p->rx_data_buf[i]);
        }
        LOG_I(bt_spp_example,"print server recevied end!!!");
}

void spp_server_send_data(void)
{
    bt_status_t result = BT_STATUS_FAIL;
    uint32_t remain_data_len = strlen(SPP_SERVER_STRING)  - spp_server_cntx_p->tx_data_length;
    do {
        uint16_t will_send_len = 0;
        if (remain_data_len >= spp_server_cntx_p->max_packet_length) {
            will_send_len = spp_server_cntx_p->max_packet_length;
        } else {
           will_send_len = remain_data_len;
       }
        result = bt_spp_send(spp_server_cntx_p->spp_handle, 
           spp_server_cntx_p->tx_data_buf + strlen(SPP_SERVER_STRING) - remain_data_len,
            will_send_len);
       if(result == BT_STATUS_SUCCESS) {
           remain_data_len -= will_send_len;
       } else if (result == BT_STATUS_SPP_TX_NOT_AVAILABLE) {
            //means it send fail currently data, need send again when u recieve BT_SPP_READY_TO_SEND_IND.
            return;
       } else {
           LOG_I(bt_spp_example, "error result = 0x%x, need to check reason.", result);
            return;
        }
    } while (remain_data_len > 0);
}

void spp_server_receive_data(uint8_t *data, uint16_t len)
{
    memcpy((void *) (spp_server_cntx_p->rx_data_buf + spp_server_cntx_p->rx_data_length), data, len);
    spp_server_cntx_p->rx_data_length += len;
    return;
}

void spp_server_ready_to_send_ind(bt_spp_ready_to_send_ind_t* send_ind_p)
{
    LOG_I(bt_spp_example,"server ready to send data.");
    spp_server_send_data();
}

/**
 * @brief          This function is for spp server event handler implement.
 * @param[in]  msg      is bt message type value.
 * @param[in]  status   is the status of the message.
 * @param[in]  buff       is the parameter of the message.
 * @return       void.
 */
void spp_server_event_handler(bt_msg_type_t msg, bt_status_t status, void *param)
{
     LOG_I(bt_spp_example,"spp_server_event_handler, 0x%x", msg);
     switch (msg) {
         case BT_SPP_CONNECT_IND:/*0x34000000*/
         {
             spp_server_connect_ind(status, (bt_spp_connect_ind_t *) param);
         }
         break;
         
         case BT_SPP_CONNECT_CNF:/*0x34000001*/
         {
             spp_server_connect_cnf(status, (bt_spp_connect_cnf_t*)param);
             if(spp_server_cntx_p->is_connected) {
                 spp_server_send_data();
             }
         }
         break;
         
         case BT_SPP_DISCONNECT_IND:/*0x34000002*/
         {
             spp_server_disconnect_ind(status,(bt_spp_disconnect_ind_t*)param);
         }
         break;
         
         case BT_SPP_READY_TO_SEND_IND:/*0x34000004*/
         {            
             spp_server_ready_to_send_ind((bt_spp_ready_to_send_ind_t*)param);
         }
         break;
         
        case BT_SPP_DATA_RECEIVED_IND:/*0x34000003*/
        {
            if (spp_server_cntx_p->rx_data_length == strlen(SPP_CLIENT_STRING)) {
                spp_server_print_received_data();
                 bt_status_t result = bt_spp_disconnect(spp_server_cntx_p->spp_handle);
                 LOG_I(bt_spp_example,"disconnect result:0x%8x", result);
            }   
        }
        break;

        default:
        break;
    }
}

