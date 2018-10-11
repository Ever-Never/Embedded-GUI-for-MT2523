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
 * The file is used for testing BT notification..
 *
 ****************************************************************************/

//#include "bt_notify.h"
#include "stdint.h"
#include <string.h>
#include "stdio.h"

#include <FreeRTOS.h>

#include "queue.h"
#include "task.h"
#include "bt_gap.h"
#include "bt_gap_le.h"
#include "bt_system.h"
#include "bt_spp.h"
#include "bt_callback_manager.h"
#include "bt_notify.h"
#include "task_def.h"
#include "bt_notify_test.h"

#include "syslog.h"

log_create_module(NOTIFY_APP, PRINT_LEVEL_INFO);

#define APPHEADER "app_test apk 0 "  /*format:'sender  receiver datatype '*/ /*app_test apk 0 strlen(data_string) data_string*/

bt_bd_addr_t g_bt_addr;
app_queue_struct_t queue_item;

QueueHandle_t app_queue = NULL;

bt_notify_test_cntx_t g_text_cntx;

/*****************************************************************************
 * define
 *****************************************************************************/
#define APP_QUEUE_SIZE      2000

/*****************************************************************************
 * typedef
 *****************************************************************************/

#define BT_SPP_STANDARD_UUID    0x00,0x00,0xFF,0x01,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFF
 
 /****************************************************************************
  *
  * ROMable data
  *SPP sdp ut record
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
     'B', 'T', 'N', 'O', 'T', 'I', 'F', 'Y', 'R'
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
 

#define DEVICE_NAME "notify_example_device"
bt_gap_config_t bt_custom_config;


const bt_gap_config_t* bt_gap_get_local_configuration_test(void)
{
    

    LOG_I(NOTIFY_APP,"bt_get_local_configuration1\n");
    bt_custom_config.inquiry_mode = 3;
    bt_custom_config.io_capability = BT_GAP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT;
    bt_custom_config.cod = 0x240404;
    memcpy(&bt_custom_config.device_name, DEVICE_NAME, sizeof(DEVICE_NAME));

    
    LOG_I(NOTIFY_APP,"1:%08x\r\n", (unsigned int)DEVICE_NAME);
    return  &bt_custom_config;
}

bt_status_t bt_notify_app_event_callback(
    bt_msg_type_t msg, 
    bt_status_t status, 
    void *buff)

{

    LOG_I(NOTIFY_APP,"event:0x%x, %x\n", msg, status);
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
            LOG_I(NOTIFY_APP,"bt power on confirm.\n");
            bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);
            break;      

        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}

/**
 * @brief          This function is for dealing the event of the notify service in the app task.
 * @param[in]  evt_id    is the event of the notify service.
 * @param[in]  data      is the data about the event.
 * @return       void.
 */

void bt_notify_test_callback_hdlr(uint32_t evt_id, void *data)
{

    bt_notify_test_cntx_t *text_cntx = &g_text_cntx;

    LOG_I(NOTIFY_APP, "[App test]callback evt_id = %d!\r\n", evt_id);
    switch (evt_id) {
        case BT_NOTIFY_EVENT_CONNECTION: {
            bt_bd_addr_t *p_data = (bt_bd_addr_t *)data;
            memcpy(&g_bt_addr, p_data, 6);
            /*connected with the remote device*/
        }
        break;
        case BT_NOTIFY_EVENT_DISCONNECTION: {
            
          //  bt_bd_addr_t *p_data = (bt_bd_addr_t *)data;
            memset(&g_bt_addr, 0, 6);
            /*disconnected with the remote device*/
        }
        break;

        case BT_NOTIFY_EVENT_SEND_IND: {
            /*send  new/the rest data flow start*/
            
            int32_t send_size;
            send_size = bt_notify_send_data(&g_bt_addr, (const char*)(text_cntx->write_buffer + text_cntx->write_len), (uint16_t)strlen((const char*)(text_cntx->write_buffer + text_cntx->write_len)), false);
            text_cntx->write_len += send_size;
            if (text_cntx->write_len >= text_cntx->write_total_len) {
                text_cntx->is_write_done = true;
            } else {
                text_cntx->is_write_done = false;
            }

            LOG_I(NOTIFY_APP, "[App test]1 = %d,2 = %d,3 = %d\r\n", text_cntx->write_total_len, text_cntx->write_len, send_size);

        }
        break;
        case BT_NOTIFY_EVENT_DATA_RECEIVED: {
            bt_notify_event_data_t *p_data = (bt_notify_event_data_t *)data;
            char len[5] = {0};
            /*receive data*/
            LOG_I(NOTIFY_APP, "[App test] data = %s, len = %d\r\n", p_data->data, p_data->length);
            strcat((char *)text_cntx->read_buffer, (const char*)p_data->data );
            text_cntx->read_total_len += (strlen((const char*)p_data->data));
            if (text_cntx->read_total_len < (p_data->length )) {
                LOG_I(NOTIFY_APP, "[App test]no-read-done!\r\n");
            } else {

                if (text_cntx->is_write_done == true) {
                    memcpy(text_cntx->write_buffer, APPHEADER, strlen(APPHEADER) + 1);
                    sprintf(len, "%d ", (int)text_cntx->read_total_len);
                    strcat((char *)text_cntx->write_buffer, len);
                    strcat((char *)text_cntx->write_buffer, (const char*)text_cntx->read_buffer);

                    LOG_I(NOTIFY_APP, "[App test]write_buffer = %s\n", text_cntx->write_buffer);
                    text_cntx->write_len  = bt_notify_send_data(&g_bt_addr, (const char*)text_cntx->write_buffer,(uint16_t)strlen(text_cntx->write_buffer), true);

                    LOG_I(NOTIFY_APP, "[App test]%d\r\n", text_cntx->write_len);
                    text_cntx->write_total_len = text_cntx->read_total_len;
                    memset(text_cntx->read_buffer, 0, APP_RECEIVE_BUF_SIZE);
                    text_cntx->read_total_len = 0;

                }

            }

        }
        break;
        case BT_NOTIFY_EVENT_NOTIFICATION: {
            /*receive a notification*/
            uint32_t i = 0, j = 0;
            bt_notify_notification_t *noti = (bt_notify_notification_t *)data;

            LOG_I(NOTIFY_APP, "********Noti_S,UCS2******************\r\n");
            LOG_I(NOTIFY_APP, "notification id: %d\r\n", noti->msg_id);
            LOG_I(NOTIFY_APP, "action: %d\r\n", noti->action);
            if (noti->action != BT_NOTIFY_ACTION_TYPE_DELETE) {
                LOG_I(NOTIFY_APP, "sender: %s\r\n", noti->sender);
                LOG_I(NOTIFY_APP, "timestamp: %d\r\n", noti->timestamp);
                LOG_I(NOTIFY_APP, "app id: %d\r\n", noti->app_id);
                LOG_I(NOTIFY_APP, "page num: %d\r\n", noti->page_num);
                {
                    bt_notify_page_content_list_t *page_content;

                    for (i = noti->page_num; i > 0; i--) {
                        page_content = noti->page_content;
                        for (j = 0; j < i - 1; j++) {
                            page_content = page_content->next;
                        }

                        LOG_I(NOTIFY_APP, "content: %s\r\n", page_content->content);
                        LOG_I(NOTIFY_APP, "title: %s\r\n", page_content->title);

                    }
                }

                LOG_I(NOTIFY_APP, "group id: %s\r\n", noti->group_id);
                LOG_I(NOTIFY_APP, "action number: %d\r\n", noti->action_number);
                for (i = 0; i < noti->action_number; i++) {
                    LOG_I(NOTIFY_APP, "action name: %s\r\n", noti->action_content[i].action_name);
                    LOG_I(NOTIFY_APP, "action id: %s\r\n", noti->action_content[i].action_id);
                }

            }

            LOG_I(NOTIFY_APP, "********Noti_E******************\r\n");
        }
        break;
        case BT_NOTIFY_EVENT_MISSED_CALL: {

            /*receive a missed call*/
            bt_notify_missed_call_t *call = (bt_notify_missed_call_t *)data;

            LOG_I(NOTIFY_APP, "********MissedCall_S******************\r\n");
            LOG_I(NOTIFY_APP, "sender: %s\r\n", call->sender);
            LOG_I(NOTIFY_APP, "send number: %s\r\n", call->sender_number);
            LOG_I(NOTIFY_APP, "missed call count: %d\r\n", call->missed_call_count);
            LOG_I(NOTIFY_APP, "notification id: %d\r\n", call->msg_id);
            LOG_I(NOTIFY_APP, "timestamp: %d\r\n", call->timestamp);
            LOG_I(NOTIFY_APP, "********MissedCall_E******************\r\n");

        }
        break;
        case BT_NOTIFY_EVENT_SMS: {
            /*receive a SMS*/
            bt_notify_sms_t *sms = (bt_notify_sms_t *)data;

            LOG_I(NOTIFY_APP, "********SMS_S UCS2******************\r\n");
            LOG_I(NOTIFY_APP, "sender: %s\r\n", sms->sender);
            LOG_I(NOTIFY_APP, "send number: %s\r\n", sms->sender_number);
            LOG_I(NOTIFY_APP, "page content: %s\r\n", sms->page_content->content);
            LOG_I(NOTIFY_APP, "page title: %s\r\n", sms->page_content->title);
            LOG_I(NOTIFY_APP, "notification id: %d\r\n", sms->msg_id);
            LOG_I(NOTIFY_APP, "timestamp: %d\r\n", sms->timestamp);
            LOG_I(NOTIFY_APP, "********SMS_E******************\r\n");

        }
        break;
        default:
            break;
    }
    if (data) {
        vPortFree(data);
        data = NULL;
    }
}

/**
 * @brief          This function is for recieve the event of the notify service.
 * @param[in]  data      is the data about the event.
 * @return       void.
 */

void bt_notify_callback_func(void *data)
{
    LOG_I(NOTIFY_APP, "\r\n[App test]callback task\r\n");
    
    bt_notify_callback_data_t *p_data = (bt_notify_callback_data_t *)data;
    queue_item.event_id = p_data->evt_id;

    switch (p_data->evt_id) {
        case BT_NOTIFY_EVENT_CONNECTION: {
            queue_item.data_ptr = (bt_bd_addr_t *)pvPortMalloc(sizeof(bt_bd_addr_t));
            memcpy(queue_item.data_ptr, p_data->bt_addr, sizeof(bt_bd_addr_t));
            break;
        }
        case BT_NOTIFY_EVENT_DISCONNECTION: {
            queue_item.data_ptr = (bt_bd_addr_t *)pvPortMalloc(sizeof(bt_bd_addr_t));
            memcpy(queue_item.data_ptr, p_data->bt_addr, sizeof(bt_bd_addr_t));
            break;
        }
        case BT_NOTIFY_EVENT_SEND_IND: {
            queue_item.data_ptr = (bt_bd_addr_t *)pvPortMalloc(sizeof(bt_bd_addr_t));
            memcpy(queue_item.data_ptr, p_data->bt_addr, sizeof(bt_bd_addr_t));
            break;
        }
        case BT_NOTIFY_EVENT_DATA_RECEIVED: {
            queue_item.data_ptr = (bt_notify_event_data_t *)pvPortMalloc(sizeof(bt_notify_event_data_t));
            memcpy(queue_item.data_ptr, &p_data->event_data, sizeof(bt_notify_event_data_t));
            break;
        }
        case BT_NOTIFY_EVENT_NOTIFICATION: {
            queue_item.data_ptr = (bt_notify_notification_t *)pvPortMalloc(sizeof(bt_notify_notification_t));
            memcpy(queue_item.data_ptr, &p_data->notification, sizeof(bt_notify_notification_t));
            break;
        }
        case BT_NOTIFY_EVENT_MISSED_CALL: {
            queue_item.data_ptr = (bt_notify_missed_call_t *)pvPortMalloc(sizeof(bt_notify_missed_call_t));
            memcpy(queue_item.data_ptr, &p_data->missed_call, sizeof(bt_notify_missed_call_t));
            break;
        }
        case BT_NOTIFY_EVENT_SMS: {
            queue_item.data_ptr = (bt_notify_sms_t *)pvPortMalloc(sizeof(bt_notify_sms_t));
            memcpy(queue_item.data_ptr, &p_data->sms, sizeof(bt_notify_sms_t));

            break;
        }
        default: {
            break;
        }
    }  
    if (xQueueSend(app_queue, (void *)&queue_item, 0) != pdPASS) {
        LOG_I(NOTIFY_APP, "\n[App test]QUEUE_FULL\n");
    }

}

/**
 * @brief          This function is for app task implement.
 * @param[in]  arg.
 * @return       void.
 */

void bt_notify_test_task(void *arg)
{
    app_queue_struct_t queue_data_item;
    LOG_I(NOTIFY_APP, "[App test]App test task begin\r\n");

    app_queue = xQueueCreate(APP_QUEUE_SIZE, sizeof(app_queue_struct_t));
    if ( app_queue == NULL ) {
        LOG_I(NOTIFY_APP, "[App test]create queue failed!\r\n");
        return;
    }

    /*main loop*/
    while (1) {
        if (xQueueReceive(app_queue, (void *)&queue_data_item, portMAX_DELAY)) {
            /*  LOG_I(NOTIFY_APP, "[App test]handle one message id = %d\r\n", queue_data_item.event_id);*/
            bt_notify_test_callback_hdlr(queue_data_item.event_id, queue_data_item.data_ptr);
        }
    }

}

/**
 * @brief          This function is for app init implement.
 * @param[in]  void.
 * @return       void.
 */

void bt_notify_test_init(void)
{
    memset(&g_text_cntx, 0, sizeof(bt_notify_test_cntx_t));
    g_text_cntx.is_write_done = true;
    bt_notify_init(BT_SPP_SERVER_ID_START);
  
    if (BT_NOTIFY_RESULT_REGISTER_OK !=  bt_notify_register_callback(NULL, "app_test", bt_notify_callback_func)) {
        return;
    }
    bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_GAP | MODULE_MASK_SYSTEM, (void*)bt_notify_app_event_callback);
    bt_callback_manager_register_callback(bt_callback_type_gap_get_local_configuration, 0, (void*)bt_gap_get_local_configuration_test);
    bt_callback_manager_add_sdp_customized_record(&bt_spp_app_sdp_record);


}

/**
 * @brief          This function is for app task init implement.
 * @param[in]  void.
 * @return       void.
 */

void bt_notify_test_task_init(void)
{
    LOG_I(NOTIFY_APP, "[App test]create task!\r\n");    
    if (pdPASS != xTaskCreate(bt_notify_test_task, NOTIFY_APP_TASK_NAME, NOTIFY_APP_TASK_STACKSIZE/sizeof(StackType_t), NULL, NOTIFY_APP_TASK_PRIORITY, NULL)) {
        LOG_I(NOTIFY_APP,  "[App test]cannot create bt_notify_service_task.");
    }
    bt_notify_test_init();
}


