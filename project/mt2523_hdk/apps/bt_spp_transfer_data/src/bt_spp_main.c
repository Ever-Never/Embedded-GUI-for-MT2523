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
#include "bt_gap.h"
#include "bt_system.h"
#include "bt_spp.h"

extern bool spp_role_is_client(uint32_t handle);
extern void spp_client_receive_data(uint8_t *data, uint16_t len);
extern void spp_server_receive_data(uint8_t *data, uint16_t len);
extern void spp_client_send_connect_request(void);
extern void spp_client_event_handler(bt_msg_type_t msg, bt_status_t status, void *param);
extern void spp_server_event_handler(bt_msg_type_t msg, bt_status_t status, void *param);

log_create_module(bt_spp_example, PRINT_LEVEL_INFO);

/*for spp server*/
QueueHandle_t spp_server_queue;
/*for spp client*/
QueueHandle_t spp_client_queue;

bt_status_t bt_app_gap_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    //LOG_I(bt_spp_example,"event:0x%x, %x\n", msg, status);
        switch (msg) {
            case BT_GAP_SET_SCAN_MODE_CNF:/*109*/
                break;

            case BT_GAP_IO_CAPABILITY_REQ_IND:/*103*/
                bt_gap_reply_io_capability_request(BT_GAP_OOB_DATA_PRESENTED_NONE,
                    BT_GAP_SECURITY_AUTH_REQUEST_GENERAL_BONDING_AUTO_ACCEPTED);
                break;
    
            case BT_GAP_USER_CONFIRM_REQ_IND:/*104*/
                bt_gap_reply_user_confirm_request(true);
                break;
                     
            case BT_POWER_ON_CNF:  /*24000001*/
                LOG_I(bt_spp_example,"bt power on confirm.");
                bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);
                //spp client start
                spp_client_send_connect_request();
                break;      
    
            default:
                break;
        }
        return BT_STATUS_SUCCESS;
}

bt_status_t bt_app_spp_event_callback(bt_msg_type_t msg, bt_status_t status, void *param)
{
    LOG_I(bt_spp_example, "msg:0x%x, status:0x%x", msg, status);
    spp_message_t message;
    uint32_t handle = BT_SPP_INVALID_HANDLE;
    
    switch (msg) {
        case BT_SPP_CONNECT_IND:/*0x34000000*/
        {             
            bt_spp_connect_ind_t * conn_ind = (bt_spp_connect_ind_t*)param;
            memcpy((void *) message.param, param, sizeof(bt_spp_connect_ind_t));
            handle = conn_ind->handle;
        }
        break;
        
        case BT_SPP_CONNECT_CNF:/*0x34000001*/
        {
            bt_spp_connect_cnf_t * conn_cnf = (bt_spp_connect_cnf_t*)param;
            memcpy((void *) message.param, param, sizeof(bt_spp_connect_cnf_t));
            handle = conn_cnf->handle;
        }
        break;
        
        case BT_SPP_DISCONNECT_IND:/*0x34000002*/
        {          
            bt_spp_disconnect_ind_t * dis_ind = (bt_spp_disconnect_ind_t*)param;
            memcpy((void *) message.param, param, sizeof(bt_spp_disconnect_ind_t));
            handle = dis_ind->handle;
        }
        break;  
            
        case BT_SPP_DATA_RECEIVED_IND:/*0x34000003*/
        {
            bt_spp_data_received_ind_t * data_ind = (bt_spp_data_received_ind_t*)param;
            memcpy((void *) message.param, param, sizeof(bt_spp_data_received_ind_t));
            handle = data_ind->handle;
            //Copy out the app packet becuase the packet buffer will be release after this BT callstack is finished.
             if (spp_role_is_client(handle)){ 
                spp_client_receive_data(data_ind->packet, data_ind->packet_length);
             } else {
                spp_server_receive_data(data_ind->packet, data_ind->packet_length);
             }     
        }
        break;  
        
        case BT_SPP_READY_TO_SEND_IND:/*0x34000004*/
        {  
            bt_spp_ready_to_send_ind_t * send_ind = (bt_spp_ready_to_send_ind_t*)param;
            memcpy((void *) message.param, param, sizeof(bt_spp_ready_to_send_ind_t));
            handle = send_ind->handle;
        }
        break;   
        
        default:
        break;
    }

    message.msg = msg;
    message.status = status;
    if (spp_role_is_client(handle)){
        xQueueSend(spp_client_queue, (void*)&message, 0);
    } else {
        xQueueSend(spp_server_queue, (void*)&message, 0);
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    if (msg >= BT_MODULE_SPP && msg < BT_MODULE_AVRCP) {
        return bt_app_spp_event_callback(msg,status,buff);
    } else {
        bt_app_gap_event_callback(msg, status, buff);
    }
    return BT_STATUS_SUCCESS;
}

/**
 * @brief          This function is for spp client task implement.
 * @param[in]  arg      .
 * @return       void.
 */
void spp_client_task(void *arg)
{
    spp_message_t message;

    spp_client_queue = xQueueCreate(10, sizeof(spp_message_t));
    if( spp_client_queue == NULL ) {
        LOG_I(bt_spp_example,"create client queue failed!");
        return;
    }
    
    while(1) {
        if(xQueueReceive(spp_client_queue, (void *)&message, 0)) {
            LOG_I(bt_spp_example,"client message:%x,%x",message.msg, message.param);
            spp_client_event_handler(message.msg, message.status, (void*) (message.param));
            memset((void*) &message, 0, sizeof(spp_message_t));
        }
    }
}

/**
 * @brief          This function is for spp server task implement.
 * @param[in]  arg      .
 * @return       void.
 */
void spp_server_task(void *arg)
{
    spp_message_t message;
    spp_server_queue = xQueueCreate(3, sizeof(spp_message_t));
    if( spp_server_queue == NULL ) {
        LOG_I(bt_spp_example,"create server queue failed!");
        return;
    }
    
    while(1) {
        if(xQueueReceive(spp_server_queue, (void *)&message, 0)) {
            LOG_I(bt_spp_example,"server message:%x,%x",message.msg, message.param);
            spp_server_event_handler(message.msg, message.status, (void*) (message.param));
            memset((void*) &message, 0, sizeof(spp_message_t));
        }
    }
}

/**
 * @brief          This function is for spp example project task main function.
 * @return       void.
 */
void bt_spp_main(void)
{
    TaskHandle_t xCreatedClientTask;
    TaskHandle_t xCreatedServerTask;
    LOG_I(bt_spp_example,"create spp app task!");
    xTaskCreate(spp_client_task, SPP_CLIENT_TASK_NAME, SPP_CLIENT_TASK_STACK_SIZE /((uint32_t)sizeof(StackType_t)), NULL, SPP_CLIENT_TASK_PRIO, &xCreatedClientTask);
    xTaskCreate(spp_server_task, SPP_SERVER_TASK_NAME, SPP_SERVER_TASK_STACK_SIZE /((uint32_t)sizeof(StackType_t)), NULL, SPP_SERVER_TASK_PRIO, &xCreatedServerTask);
}

