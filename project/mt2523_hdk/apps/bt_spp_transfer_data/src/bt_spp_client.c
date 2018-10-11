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
#include "bt_spp_client.h"

app_spp_client_cntx_t spp_client_cntx;
app_spp_client_cntx_t *spp_client_cntx_p = &spp_client_cntx;

//static bt_bd_addr_t remote_address = {0x29,0x92,0x65,0x46,0xF2,0xBE};
static const bt_bd_addr_t remote_address = {0x9a,0x93,0x31,0xbd,0xd6,0x0c};

void spp_client_print_received_data(void)
{
        uint32_t i = 0;
        LOG_I(bt_spp_example,"print server recevied data start:%d", spp_client_cntx_p->rx_data_length);
        for (i = 0; i < spp_client_cntx_p->rx_data_length; i++) {
            LOG_I(bt_spp_example, "%c", spp_client_cntx_p->rx_data_buf[i]);
        }
        LOG_I(bt_spp_example,"print server recevied end!!!");
}

void spp_client_send_data(void)
{    
    uint32_t remain_data_len = strlen(SPP_CLIENT_STRING)  - spp_client_cntx_p->tx_data_length;
    do {
        uint16_t will_send_len = 0;
        if (remain_data_len >= spp_client_cntx_p->max_packet_length) {
            will_send_len = spp_client_cntx_p->max_packet_length;
        } else {
           will_send_len = remain_data_len;
       }
      bt_status_t result = bt_spp_send(spp_client_cntx_p->spp_handle, 
           spp_client_cntx_p->tx_data_buf+ strlen(SPP_CLIENT_STRING) - remain_data_len,
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
    
void spp_client_ready_to_send_ind(bt_spp_ready_to_send_ind_t* send_ind_p)
{
    LOG_I(bt_spp_example,"client ready to send data.");
    spp_client_send_data();
}
void spp_client_connect_cnf(bt_status_t status, bt_spp_connect_cnf_t* conn_cnf_p)
{
    LOG_I(bt_spp_example,"handle:0x%x, max_packet_length: %d", conn_cnf_p->handle, conn_cnf_p->max_packet_length);
    if (status == BT_STATUS_SUCCESS) {
        spp_client_cntx_p->is_connected = true;
        spp_client_cntx_p->max_packet_length = conn_cnf_p->max_packet_length;
        //The string will be sent to remote device.
        memcpy(spp_client_cntx_p->tx_data_buf, (uint8_t*)SPP_CLIENT_STRING, strlen(SPP_CLIENT_STRING));
        spp_client_cntx_p->tx_data_length = 0;
    }  else {
        memset(spp_client_cntx_p,  0x0, sizeof(app_spp_client_cntx_t));
    }
    return;   
}

void spp_client_disconnect_ind(bt_status_t status, bt_spp_disconnect_ind_t* disc_ind_p)
{
    LOG_I(bt_spp_example,"handle:0x%x, result:0x%x", disc_ind_p->handle,status);    
    spp_client_cntx_p->is_connected = false;
    memset(spp_client_cntx_p,  0x0, sizeof(app_spp_client_cntx_t));
    return;
}

void spp_client_send_connect_request(void)
{
    const uint8_t spp_uuid[16] = {0x00,0x00,0x11,0x01,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x80,0x5f,0x9b,0x34,0xfb};
    bt_status_t result = bt_spp_connect(&(spp_client_cntx_p->spp_handle), &remote_address, spp_uuid);
    LOG_I(bt_spp_example,"request, result:0x%x", result);
    return;
}

void spp_client_disconnect_request(void)
{
    bt_status_t result = bt_spp_disconnect(spp_client_cntx_p->spp_handle);
    LOG_I(bt_spp_example,"handle:0x%x, result:0x%x", spp_client_cntx_p->spp_handle, result);
    return;
}

bool spp_role_is_client(uint32_t handle)
{
   bool is_client = false;
   if (spp_client_cntx_p->spp_handle ==  handle){
       is_client = true;
   } 
   LOG_I(bt_spp_example, "is_client:%d", is_client);
   return is_client;
}
void spp_client_receive_data(uint8_t *data, uint16_t len)
{
    memcpy((void *) (spp_client_cntx_p->rx_data_buf + spp_client_cntx_p->rx_data_length), data, len);
    spp_client_cntx_p->rx_data_length += len;
    return;
}

/**
 * @brief          This function is for spp client event handler implement.
 * @param[in]  msg      is bt message type value.
 * @param[in]  status   is the status of the message.
 * @param[in]  buff       is the parameter of the message.
 * @return       void.
 */
void spp_client_event_handler(bt_msg_type_t msg, bt_status_t status, void *param)
{

    LOG_I(bt_spp_example,"spp_client_event_handler, 0x%x", msg);

    switch(msg) {
        case BT_SPP_CONNECT_CNF:/*0x34000001*/
        {
            spp_client_connect_cnf(status, (bt_spp_connect_cnf_t*)param);
            if(spp_client_cntx_p->is_connected) {
                spp_client_send_data();
            }
        }
        break;
             
        case BT_SPP_DISCONNECT_IND:/*0x34000002*/
        {
            spp_client_disconnect_ind(status,(bt_spp_disconnect_ind_t*)param);
        }
        break;
             
        case BT_SPP_READY_TO_SEND_IND:/*0x34000004*/
         {            
            spp_client_ready_to_send_ind((bt_spp_ready_to_send_ind_t*)param);
        }
        break;
             
        case BT_SPP_DATA_RECEIVED_IND:/*0x34000003*/
        {
            if (spp_client_cntx_p->rx_data_length == strlen(SPP_SERVER_STRING)) {
                spp_client_print_received_data();
                bt_status_t result = bt_spp_disconnect(spp_client_cntx_p->spp_handle);
                LOG_I(bt_spp_example,"disconnect result:0x%8x", result);
            }   
        }
        break;

        default:
        break;
    }
}

