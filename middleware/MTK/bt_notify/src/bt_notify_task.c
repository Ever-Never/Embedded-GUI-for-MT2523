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

#include "bt_notify_private.h"
#include "bt_notify_task.h"
#include "bt_notify_list.h"
#include "syslog.h"


QueueHandle_t bt_notify_service_queue;
static uint32_t length = 0;
static uint32_t length1 = 0;

/**
 * @brief          This function is for service send queue.
 * @param[in]  msg      is bt message type value.
 * @param[in]  status   is the status of the message.
 * @param[in]  buff       is the parameter of the message.
 * @return       void.
 */

void bt_notify_service_send_queue(bt_msg_type_t msg, 
     bt_status_t status, void* data_ind)
{
    bt_notify_service_message_t message;

      message.buff = data_ind;     
    
      message.msg = msg;
      message.status = status;
    
     xQueueSend(bt_notify_service_queue, (void*)&message, 0);

}

/**
 * @brief          This function is for service recieves spp data indication.
 * @param[in]  msg      is bt message type value.
 * @param[in]  status   is the status of the message.
 * @param[in]  buff       is the parameter of the message.
 * @return       void.
 */

void bt_notify_service_spp_data_ind(bt_spp_data_received_ind_t * data_buff, bt_msg_type_t msg, 
     bt_status_t status)
{
    int32_t cntx_chnl = bt_notify_find_cntx_channel_by_handle(data_buff->handle);

    bt_notify_context *cntx_p = bt_notify_get_cntx(cntx_chnl);
    
    LOG_I(NOTIFY, "bt_notify_service_spp_data_ind, cntx_p->ring_buf_len:%d\n", cntx_p->ring_buf_len);

    if (cntx_p->ring_buf_len == 0){
        
        if (Length(bt_notify_get_head()) == 0) {//list is null
             //save ring buff. and send task
             
             bt_notify_service_message_t message;
             LOG_I(NOTIFY_SRV, "bt_notify_service_spp_data_ind1");
             memcpy(cntx_p->ring_buf, data_buff->packet, data_buff->packet_length);
             cntx_p->ring_buf_len = data_buff->packet_length;




             message.buff = (bt_spp_data_received_ind_t*)pvPortMalloc(sizeof(bt_spp_data_received_ind_t)+ data_buff->packet_length);     
            ((bt_spp_data_received_ind_t *)message.buff)->handle = data_buff->handle;
            ((bt_spp_data_received_ind_t *)message.buff)->packet_length = data_buff->packet_length;
            memcpy((((bt_spp_data_received_ind_t *)message.buff)+1), data_buff->packet, data_buff->packet_length);
            message.msg = msg;
            message.status = status;
            LOG_I(NOTIFY,  "data :%d\n", data_buff->packet_length);

            xQueueSend(bt_notify_service_queue, (void*)&message, 0);





        }else {
              
            bt_spp_hold_data(data_buff->packet);
            addNode(bt_notify_get_head(), data_buff->packet, data_buff->packet_length);  
        
        }

    }else {
    
        bt_spp_hold_data(data_buff->packet);
        addNode(bt_notify_get_head(), data_buff->packet, data_buff->packet_length);  
        
    }

}

/**
 * @brief          This function is for service receive spp send data indication.
 * @param[in]  msg      is bt message type value.
 * @param[in]  status   is the status of the message.
 * @param[in]  buff       is the parameter of the message.
 * @return       void.
 */

void bt_notify_service_spp_send_ind(bt_msg_type_t msg, 
     bt_status_t status, 
     void *buff)
{
    bt_notify_service_message_t message;

    message.buff = (bt_spp_ready_to_send_ind_t*)pvPortMalloc(sizeof(bt_spp_ready_to_send_ind_t));
    memcpy((void*) message.buff,(void*)buff, sizeof(bt_spp_ready_to_send_ind_t));
    message.msg = msg;
    message.status = status;


    xQueueSend(bt_notify_service_queue, (void*)&message, 0);

}

/**
 * @brief          This function is for service callback handle in service task.
 * @param[in]  msg      is bt message type value.
 * @param[in]  status   is the status of the message.
 * @param[in]  buff       is the parameter of the message.
 * @return       void.
 */

void bt_notify_service_event_handler(bt_msg_type_t msg, 
     bt_status_t status, 
     void *buff)
{
     LOG_I(NOTIFY, "spp_server_event_handler, 0x%x", msg);
     switch (msg) {
         
         case BT_SPP_READY_TO_SEND_IND:/*0x34000004*/
         {            

            bt_notify_handle_ready_to_write_ind(buff,BT_NOTIFY_DATA_SOURCE_SPP);

         }
         break;
         
         case BT_SPP_DATA_RECEIVED_IND:/*0x34000003*/
         {
             
             bt_spp_data_received_ind_t* data = (bt_spp_data_received_ind_t*)buff;            
             int32_t cntx_chnl = bt_notify_find_cntx_channel_by_handle(data->handle);
             bt_notify_context *cntx_p = bt_notify_get_cntx(cntx_chnl);
             length1 += data->packet_length;
           //  LOG_I(NOTIFY, "bt len11:%d", length1);
           
              //printf("bt len11:%d\r\n", length1);
             LOG_I(NOTIFY, "handle:%d, cntx_chnl:%d", data->handle, cntx_chnl);
             if (cntx_p->ring_buf_len != 0) {//not null
                 bt_notify_handle_ready_to_read_ind(data, BT_NOTIFY_DATA_SOURCE_SPP);


             }
               
             //release.
         }
         break;
         
     }
     if (buff){
         vPortFree(buff);
         buff = NULL;
    }
}

/**
 * @brief          This function is for service callback handle from dispatcher module.
 * @param[in]  msg      is bt message type value.
 * @param[in]  status   is the status of the message.
 * @param[in]  buff       is the parameter of the message.
 * @return       void.
 */

bt_status_t bt_notify_service_event_callback(bt_msg_type_t msg, 
     bt_status_t status, 
     void *buff)
{
    LOG_I(NOTIFY,  "dispatcher callback :0x%x", msg);

    switch (msg) {
         case BT_SPP_CONNECT_IND:/*0x34000000*/
         {
             bt_spp_connect_ind_t *para = (bt_spp_connect_ind_t*)buff;
             LOG_I(NOTIFY, "handle:%d", para->handle);

             if (bt_notify_get_sdp_id() == para->local_server_id){//SDP ID is match.
                bt_notify_spp_connect_ind(para);

             } else {
                  LOG_I(NOTIFY, "error sdp id!!!");

             }
         }
         break;
         
         case BT_SPP_CONNECT_CNF:/*0x34000001*/
         {
             bt_notify_handle_connect_cnf(status, buff, BT_NOTIFY_DATA_SOURCE_SPP);
              
         }
         break;
         
         case BT_SPP_DISCONNECT_IND:/*0x34000002*/
         {

             bt_notify_handle_disconnect_ind(status, buff,BT_NOTIFY_DATA_SOURCE_SPP);
            //callback to app.

         }
         break;
         
        case BT_SPP_DATA_RECEIVED_IND:/*0x34000003*/
            {
                bt_spp_data_received_ind_t * data_buff = (bt_spp_data_received_ind_t*)buff;
                
                LOG_I(NOTIFY, "handle:%d", data_buff->handle);
                length += data_buff->packet_length;
               // LOG_I(NOTIFY, "bt len:%d", length);
                //printf("bt len:%d\r\n", length);
                if (bt_notify_check_handle_valid(data_buff->handle) == false) {
                                
                     LOG_I(NOTIFY, "spp data ind handle is not sync!!!");
                  } else {

                  bt_notify_service_spp_data_ind(data_buff, msg, status);
                }
            }
            break;  
        case BT_SPP_READY_TO_SEND_IND:/*0x34000004*/
            {  
             
                bt_notify_service_spp_send_ind(msg, status, buff);

            }
            break;  
        default:
            break;
    }
    return BT_STATUS_SUCCESS;

}

/**
 * @brief          This function is for service task implement.
 * @param[in]  arg      .
 * @return       void.
 */
 
void bt_notify_service_task(void *arg)
{

    LOG_I(NOTIFY, "enter bt_notify_service_task.\n");
    bt_notify_service_message_t message;

    bt_notify_service_queue = xQueueCreate(NOTIFY_SRV_QUEUE_LENGTH, sizeof(bt_notify_service_message_t));
    if( bt_notify_service_queue == NULL ) {
        LOG_I(NOTIFY, "create service queue failed!");
        return;
    }
    
    while(1) {
        if(pdPASS == xQueueReceive(bt_notify_service_queue, (void *)&message, portMAX_DELAY)) {          
            LOG_I(NOTIFY_SRV, "service recieve message:%x,%x",message.msg, message.buff);
            bt_notify_service_event_handler(message.msg, message.status, (void*) (message.buff));
            memset((void*) &message, 0, sizeof(bt_notify_service_message_t));
        }
    }
}

/**
 * @brief          This function is for service task init implement.
 * @param[in]  void.
 * @return       void.
 */

void bt_notify_service_task_init(void)
{
    LOG_I(NOTIFY_SRV, "[service]create task!\r\n");
    if (pdPASS != xTaskCreate(bt_notify_service_task, NOTIFY_SRV_TASK_NAME, NOTIFY_SRV_TASK_STACKSIZE/sizeof(StackType_t), NULL, NOTIFY_SRV_TASK_PRIORITY, NULL)) {
        LOG_I(NOTIFY_SRV,  "cannot create bt_notify_service_task.");
    }

}


void bt_notify_dogp_callback_func(ble_dogp_adp_event_t event, void *callback_param)
{
    LOG_I(NOTIFY_SRV,  "dogp_callback :%d", event);

    switch (event) {

        case BLE_DOGP_ADP_EVENT_CONNECT_IND: 
            {                
                bt_notify_handle_connect_cnf(BT_STATUS_SUCCESS, callback_param, BT_NOTIFY_DATA_SOURCE_DOGP);
                break;
            }
        case BLE_DOGP_ADP_EVENT_DISCONNECT_IND: 
            {
                bt_notify_handle_disconnect_ind(BT_STATUS_SUCCESS, callback_param,BT_NOTIFY_DATA_SOURCE_DOGP);
                break;
            }
        case BLE_DOGP_ADP_EVENT_READY_TO_READ_IND: 
            {
                bt_notify_handle_ready_to_read_ind(callback_param, BT_NOTIFY_DATA_SOURCE_DOGP);
                break;
            }
        case BLE_DOGP_ADP_EVENT_READY_TO_WRITE_IND: 
            {
                bt_notify_handle_ready_to_write_ind(callback_param,BT_NOTIFY_DATA_SOURCE_DOGP);
                break;
            }
        default:
            break;
    }

}


