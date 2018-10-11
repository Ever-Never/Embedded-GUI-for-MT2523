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

#include "ble_ancs_app.h"
#include <string.h>
#include "queue.h"
#include "task.h"
#include "task_def.h"
#include "bt_debug.h"
#include "bt_ancs_common.h"

 

ble_ancs_app_cntx_t ancs_app_cntx;
ble_ancs_app_cntx_t *p_ancs_app = &ancs_app_cntx;

QueueHandle_t ancs_queue = NULL;

/*****************************************************************************
* FUNCTION
*  ble_ancs_remove_from_notification_list
* DESCRIPTION
* Remove the notification from the link list.
* PARAMETERS
*  notif            [IN]        The notification node to be removed
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_remove_from_notification_list(ble_ancs_notification_node_t *notif)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ble_ancs_notification_node_t *p_notif;
    ble_ancs_notification_node_t *p_pre = NULL;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    p_notif = &p_ancs_app->notif_list;

    while (p_notif && p_notif != notif) {
        p_pre = p_notif;
        p_notif = p_notif->next;
    }

    if (p_pre && p_notif) {
        p_pre->next = p_notif->next;
        vPortFree(p_notif);
        p_ancs_app->notif_num--;
    }
    return;
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_add_to_notification_list
* DESCRIPTION
* Add one notification to the link list.
* PARAMETERS
*  conn_id        [IN]        Connection id
*  notif            [IN]        The notification node to be added to list
* RETURNS
*  ancs_notification_node_t*
*****************************************************************************/
ble_ancs_notification_node_t *ble_ancs_add_to_notification_list(bt_handle_t connection_handle, ble_ancs_event_notification_t *notif)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ble_ancs_notification_node_t *p_notif;
    ble_ancs_notification_node_t *new_node;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    p_notif = &p_ancs_app->notif_list;

    /*look for the tail of the link list*/
    while (p_notif->next != NULL) {
        p_notif = p_notif->next;
    }

    new_node = (ble_ancs_notification_node_t *)pvPortMalloc(sizeof(ble_ancs_notification_node_t));
    if (new_node) {
        new_node->notif.connection_handle = connection_handle;
        memcpy(new_node->notif.notif_uid, notif->notification_uid, 4);
        p_notif->next = new_node;
        new_node->next = NULL;
        p_ancs_app->notif_num++;
    } else {
        LOG_I(ANCS, "[ANCSapp]Allocate buffer fail!\r\n");
    }

    return new_node;
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_get_notification_information
* DESCRIPTION
* Get the notificaiton attributes.
* PARAMETERS
*
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_get_notification_information(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(ANCS, "[ANCSapp]notif_num = %d, curr_notif = 0x%x\r\n", p_ancs_app->notif_num, p_ancs_app->curr_notif);

    if (p_ancs_app->notif_num > 0 && p_ancs_app->curr_notif == NULL) {
        /*There is no notification in parsing*/
        ble_ancs_attribute_list_t attr_list[5];
        uint8_t attr_num;

        attr_num = 5;

        p_ancs_app->curr_notif = p_ancs_app->notif_list.next;

        /*If the buffer is not enough, the data will be truncated*/
        attr_list[0].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_APP_IDENTIFIER;
        attr_list[0].atrribute_len = BLE_ANCS_APP_ID_LEN;
        attr_list[0].data = p_ancs_app->appid;

        attr_list[1].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_TITLE;
        attr_list[1].atrribute_len = BLE_ANCS_TITLE_LEN;
        attr_list[1].data = p_ancs_app->title;

        attr_list[2].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_MESSAGE;
        attr_list[2].atrribute_len = BLE_ANCS_MESSAGE_LEN;
        attr_list[2].data = p_ancs_app->message;

        /*WARNING: You can get positive/negative action label only when connected with IOS8.0 or later*/
        attr_list[3].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_POSITIVE_ACTION_LABEL;
        attr_list[3].atrribute_len = BLE_ANCS_LAB_LEN;
        attr_list[3].data = p_ancs_app->posi_lab;

        attr_list[4].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_NEGATIVE_ACTION_LABEL;
        attr_list[4].atrribute_len = BLE_ANCS_LAB_LEN;
        attr_list[4].data = p_ancs_app->nega_lab;

        ble_ancs_get_notification_attributes(p_ancs_app->curr_notif->notif.connection_handle, p_ancs_app->curr_notif->notif.notif_uid,
                                             attr_list, attr_num);

        p_ancs_app->status = ANCS_APP_STATUS_GET_NOTIF_ATTRIBUTE;
    }
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_get_app_information
* DESCRIPTION
* Get the app attributes.
* PARAMETERS
*  attr        [IN]        Pointer to notificaiton attribute
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_get_app_information(ble_ancs_event_attribute_t *attr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ble_ancs_attribute_list_t attr_list[1];
    uint8_t attr_num;
    uint16_t appid_len = 0;
    uint8_t i;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /*Get app attribute*/
    for (i = 0; i < attr->attr_num; i++) {
        if (attr->attr_list[i].attribute_id == BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_APP_IDENTIFIER) {
            appid_len = attr->attr_list[i].atrribute_len;
            break;
        }
    }
    if (appid_len > 0 && appid_len < BLE_ANCS_APP_ID_LEN) {
        /*[warning]if appid_len > ANCS_APP_ID_LEN, appid is truncated, you can't get the right app attribute*/
        attr_num = 1;
        attr_list[0].attribute_id = BLE_ANCS_NOTIFICATION_ATTRIBUTE_ID_APP_IDENTIFIER;
        attr_list[0].atrribute_len = BLE_ANCS_APP_NAME_LEN;
        attr_list[0].data = p_ancs_app->app_name;

        ble_ancs_get_app_attributes(p_ancs_app->curr_notif->notif.connection_handle, attr->attr_list[i].data,
                                    appid_len, attr_list, attr_num);

        p_ancs_app->status = ANCS_APP_STATUS_GET_APP_ATTRIBUTE;
    }
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_show_attribute
* DESCRIPTION
* Show the detail information.
* PARAMETERS
*  attr        [IN]        Pointer to notificaiton attribute or app attribute
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_show_attribute(ble_ancs_event_attribute_t *attr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t i;
    char *notif_attr_name[BLE_ANCS_MAX_NOTIFICATION_ATTR_NUMBER] = {
        "App Identifier",
        "Title",
        "Subtitle",
        "Message",
        "Message Size",
        "Date",
        "Positive Action Label",
        "Negative Action Label"
    };
    char *app_attr_name[1] = {
        "Display Name"
    };
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if (attr->command_id == 0) {
        /*Show notification attribute*/
        for (i = 0; i < attr->attr_num; i++) {
            if (attr->attr_list[i].atrribute_len > 0) {
                LOG_I(ANCS, "*****Notification attribute*****%s : %s\r\n", notif_attr_name[attr->attr_list[i].attribute_id], attr->attr_list[i].data);
            }
        }
    } else if (attr->command_id == 1) {
        /*Show app attribute*/
        for (i = 0; i < attr->attr_num; i++) {
            if (attr->attr_list[i].atrribute_len > 0) {
                LOG_I(ANCS, "*****App attribute*****%s : %s\r\n", app_attr_name[attr->attr_list[i].attribute_id], attr->attr_list[i].data);
            }
        }
    }
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_show_notification
* DESCRIPTION
* Show notifications ,it is received first.
* PARAMETERS
*  attr        [IN]        Pointer to notificaiton source
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_show_notification(ble_ancs_event_notification_t *notif)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *eventid[BLE_ANCS_MAX_EVENT_ID_NUMBER] = {
        "Added",
        "Modified",
        "Removed"
    };
    char *catid[BLE_ANCS_MAX_CATEGORY_ID_NUMBER] = {
        "Other",
        "Incoming Call",
        "Missed Call",
        "Voice Mail",
        "Social",
        "Schedule",
        "Email",
        "News",
        "Health And Fitness",
        "Business And Finance",
        "Location",
        "Entertainment"
    };
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(ANCS, "*****NOTIFICATION*****Event_id = %s, Event_flags = 0x%x, Cat_id = %s, Cat_count = %d, Cat_uuid = %d%d%d%d\r\n",
        eventid[notif->event_id], notif->event_flags, catid[notif->category_id], notif->category_count,
        notif->notification_uid[0], notif->notification_uid[1], notif->notification_uid[2], notif->notification_uid[3]);
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_deal_with_new_notification
* DESCRIPTION
* Deal with the newly received notificaitons
* PARAMETERS
*  connection_handle        [IN]        Connection handle
*  notif            [IN]        Pointer to notificaiton source
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_deal_with_new_notification(bt_handle_t connection_handle, ble_ancs_event_notification_t *notif)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    ble_ancs_notification_node_t *p_notif;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    LOG_I(ANCS, "[ANCSapp]event_id = %d, notif_num = %d\r\n", notif->event_id, p_ancs_app->notif_num);

    if (notif->event_id == BLE_ANCS_EVENT_ID_NOTIFICATION_REMOVED) {
        p_notif = p_ancs_app->notif_list.next;
        while (p_notif && (p_notif->notif.connection_handle != connection_handle ||
                           memcmp(p_notif->notif.notif_uid, notif->notification_uid, 4))) {
            p_notif = p_notif->next;
        }
        if (p_notif) {
            if (p_notif == p_ancs_app->new_notif) {
                p_ancs_app->new_notif = NULL;
                LOG_I(ANCS, "[ANCSapp]new notification is removed, can't perform action on it!\r\n");
            }
            ble_ancs_remove_from_notification_list(p_notif);
        }
    } else if (notif->event_id == BLE_ANCS_EVENT_ID_NOTIFICATION_ADDED ||
               notif->event_id == BLE_ANCS_EVENT_ID_NOTIFICATION_MODIFIED) {
        if (p_ancs_app->notif_num == BLE_ANCS_MAX_NOTIF_NUM) {
            p_notif = p_ancs_app->notif_list.next;

            if (p_notif && p_notif == p_ancs_app->curr_notif) {
                p_notif = p_notif->next;
            }

            if (p_notif) {
                ble_ancs_remove_from_notification_list(p_notif);
            }
        }

        p_ancs_app->new_notif = ble_ancs_add_to_notification_list(connection_handle, notif);

        ble_ancs_get_notification_information();
    }
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_timeout_callback
* DESCRIPTION
* Set low power when timeout
* PARAMETERS
*  xTimer        [IN]        Timer id of the low power timer
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_timeout_callback(TimerHandle_t xTimer)
{
    uint16_t conn_interval;

    conn_interval = ble_ancs_get_conn_interval();
    LOG_I(ANCS, "[ANCSapp]Timeout: conn_interval = %d, notif_num = %d\r\n", conn_interval, p_ancs_app->notif_num);

    if (conn_interval > 16 && conn_interval < 32) {
        
        if (p_ancs_app->notif_num == 0) {
            
            /*set connection parameter to be low power*/
            LOG_I(ANCS, "[ANCSapp]high speed -> low power\r\n");
            ble_ancs_update_connection_interval(p_ancs_app->connection_handle, BLE_ANCS_CONN_LOW_POWER);
        } else {
        
            if (xTimerReset(p_ancs_app->ancs_timer, 0) != pdPASS) {
                LOG_I(ANCS, "[ANCSapp]reset timer fail!\r\n");
            }
        }
    }
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_app_event_callback
* DESCRIPTION
* Send events from ancs service to ancs task
* PARAMETERS
*  ancs_evt            [IN]        Pointer to event
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_app_event_callback(ble_ancs_event_t *ancs_evt)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    /*Adjust the queue size as you need. If it is too small ,notificaitons may lost at the time of connected*/
    xQueueSend(ancs_queue, (void *)ancs_evt, 0);  
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_message_handler
* DESCRIPTION
* Deal with the events from ancs service
* PARAMETERS
*  ancs_evt            [IN]        Pointer to event
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_message_handler(ble_ancs_event_t *ancs_evt)
{
    LOG_I(ANCS, "[ANCSapp]event_id = %d\r\n", ancs_evt->evt_type);

    switch (ancs_evt->evt_type) {
        case BLE_ANCS_EVENT_CONNECTED: 
        {
            if (ancs_evt->result == BT_STATUS_SUCCESS) {            
                p_ancs_app->connection_handle = ancs_evt->connection_handle;
                ble_ancs_enable_data_source(ancs_evt->connection_handle);
                p_ancs_app->status = ANCS_APP_STATUS_ENABLE_DATA_SOURCE;
                
            } else {
                LOG_I(ANCS, "[ANCSapp]Discover service failed!\r\n");
            }
        }
            break;
            
        case BLE_ANCS_EVENT_REQUEST_COMPLETED: 
        {
            if (ancs_evt->result == BT_STATUS_SUCCESS) {
                if (p_ancs_app->status == ANCS_APP_STATUS_ENABLE_DATA_SOURCE) {
                    /* if notification source is not enabled, enable it */

                    ble_ancs_enable_notification_source(ancs_evt->connection_handle);
                    p_ancs_app->status = ANCS_APP_STATUS_ENABLE_NOTIF_SOURCE;
                    
                } else if (p_ancs_app->status == ANCS_APP_STATUS_ENABLE_NOTIF_SOURCE) {
                    p_ancs_app->status = ANCS_APP_STATUS_NONE;
                    
                    /*start low power timer*/
                    if (xTimerStart(p_ancs_app->ancs_timer, 0) != pdPASS ) {
                        LOG_I(ANCS, "[ANCSapp]Start timer fail!\r\n");
                    } else {
                        LOG_I(ANCS, "[ANCSapp]ANCS timer started!\r\n");
                    }
                } else if (p_ancs_app->status == ANCS_APP_STATUS_PERFORM_ACTION) {
                    LOG_I(ANCS, "[ANCSapp]Perform action success!\r\n");
                    p_ancs_app->status = ANCS_APP_STATUS_NONE;
                } else {
                    p_ancs_app->status = ANCS_APP_STATUS_NONE;
                }
            } else {
                LOG_I(ANCS, "[ANCSapp]Enable data source/notification source failed! result = %x\r\n", ancs_evt->result);
                p_ancs_app->status = ANCS_APP_STATUS_NONE;
            }
        }
            break;
            
        case BLE_ANCS_EVENT_IOS_NOTIFICATION: 
        {
            uint16_t conn_interval;

            ble_ancs_show_notification(&ancs_evt->data.notification);

            /*If you need ,get notification attribute*/
            ble_ancs_deal_with_new_notification(ancs_evt->connection_handle, &ancs_evt->data.notification);

            /*low power*/
            conn_interval = ble_ancs_get_conn_interval();
            
            if (conn_interval > 300) {
                /*low power status*/
                ble_ancs_update_connection_interval(ancs_evt->connection_handle, BLE_ANCS_CONN_HIGH_SPEED);
                LOG_I(ANCS, "[ANCSapp]conn_interval = %d, low power -> high speed\r\n", conn_interval);
                
            } 
            
            if (xTimerReset(p_ancs_app->ancs_timer, 0) != pdPASS) {
                LOG_I(ANCS, "[ANCSapp]reset timer fail!\r\n");
            } else {
                LOG_I(ANCS, "[ANCSapp]reset timer!\r\n");
            }
        }
            break;
        
        case BLE_ANCS_EVENT_NOTIFICATION_ATTRIBUTE: 
        {
            if (ancs_evt->result == BT_STATUS_SUCCESS) {
                if (ancs_evt->data.attribute.command_id == 0) {
                    ble_ancs_show_attribute(&ancs_evt->data.attribute);

                    /*If you need ,get app attribute*/
                    ble_ancs_get_app_information(&ancs_evt->data.attribute);
                }
            } else {
                LOG_I(ANCS, "[ANCSapp]Get Notification Attribute failed!\r\n");
            }
        }
            break;
            
        case BLE_ANCS_EVENT_APP_ATTRIBUTE: 
        {
            if (ancs_evt->result == BT_STATUS_SUCCESS) {
                ble_ancs_show_attribute(&ancs_evt->data.attribute);
                ble_ancs_remove_from_notification_list(p_ancs_app->curr_notif);
                p_ancs_app->curr_notif = NULL;
                ble_ancs_get_notification_information();
            } else {
                LOG_I(ANCS, "[ANCSapp]Get App Attribute failed!\r\n");
            }
        }
            break;

        case BLE_ANCS_EVENT_DISCONNECTED: 
            LOG_I(ANCS, "[ANCSapp]ANCS disconnected!\r\n");
            break;
        default:
            break;
    }

}


/*****************************************************************************
* FUNCTION
*  ble_ancs_perform_action
* DESCRIPTION
*  Perform action to the lastest notification
* PARAMETERS
*  cmd            [IN]        cmd from UART
* RETURNS
*  void
*****************************************************************************/
void ble_ancs_perform_action(uint8_t *cmd)
{
    LOG_I(ANCS, "[ANCS]ble_ancs_perform_action, new_notif = %x\r\n", p_ancs_app->new_notif);
    if (p_ancs_app->new_notif && p_ancs_app->status == ANCS_APP_STATUS_NONE) {
        if (cmd[0] == 'a' && cmd[1] == 'n' && cmd[2] == 'c' && cmd[3] == 's' && cmd[4] == 'p' ) {
            ble_ancs_perform_notification_action(p_ancs_app->new_notif->notif.connection_handle, p_ancs_app->new_notif->notif.notif_uid, BLE_ANCS_ACTION_ID_POSITIVE);
            p_ancs_app->new_notif = NULL;
        }
        if (cmd[0] == 'a' && cmd[1] == 'n' && cmd[2] == 'c' && cmd[3] == 's' && cmd[4] == 'n') {
            ble_ancs_perform_notification_action(p_ancs_app->new_notif->notif.connection_handle, p_ancs_app->new_notif->notif.notif_uid, BLE_ANCS_ACTION_ID_NEGATIVE);
            p_ancs_app->new_notif = NULL;
        }
        p_ancs_app->status = ANCS_APP_STATUS_PERFORM_ACTION;
    }
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_app_init
* DESCRIPTION
* Initialize the context p_ancs_app and ancs service
* PARAMETERS
*
* RETURNS
*  ObStatus
*****************************************************************************/
void ble_ancs_app_init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memset(p_ancs_app, 0, sizeof(ble_ancs_app_cntx_t));

    ble_ancs_start(ble_ancs_app_event_callback);


    p_ancs_app->ancs_timer = xTimerCreate("ANCS_TIMER",
                                          BLE_ANCS_TIMER_PERIOD / portTICK_PERIOD_MS, pdFALSE,
                                          ( void *)0,
                                          ble_ancs_timeout_callback);

    if (!p_ancs_app->ancs_timer) {
        LOG_I(ANCS, "[ANCSapp]Create timer fail! 0x%x\r\n", p_ancs_app->ancs_timer);
    }
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_task_main
* DESCRIPTION
* Create ancs task queue, and receive message from the queue
* PARAMETERS
*
* RETURNS
*  ObStatus
*****************************************************************************/
void ble_ancs_task_main(void *arg)
{
    ble_ancs_event_t   recv_msg;

    ble_ancs_app_init();

    ancs_queue = xQueueCreate(ANCS_QUEUE_LENGTH, sizeof(ble_ancs_event_t));
    if ( ancs_queue == NULL ) {
        LOG_I(ANCS, "[ANCSapp]Create queue fail!\r\n");
        return;
    }

    while (1) {
        if (xQueueReceive(ancs_queue, (void *)&recv_msg, portMAX_DELAY)) {
            ble_ancs_message_handler(&recv_msg);
        }
    }
}


/*****************************************************************************
* FUNCTION
*  ble_ancs_task_init
* DESCRIPTION
* Create ancs task
* PARAMETERS
*
* RETURNS
*  ObStatus
*****************************************************************************/
void ble_ancs_task_init(void)
{
    TaskHandle_t xCreatedTask;

    LOG_I(ANCS, "[ANCSapp]create task!\r\n");
    xTaskCreate(ble_ancs_task_main, ANCS_TASK_NAME, ANCS_TASK_STACKSIZE/(sizeof(StackType_t)), NULL, ANCS_TASK_PRIORITY, &xCreatedTask);
}
