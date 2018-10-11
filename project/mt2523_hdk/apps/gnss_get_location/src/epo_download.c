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

/* add include file here */

#include "gnss_app.h"

//Enable this option to download EPO via bt notification service.
//If you don't need it, you can disable this option in gnss_app.h
#ifdef GNSS_SUPPORT_EPO_DOWNLOAD_BY_BT
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "hal_flash.h"

/* bluetooth module header */
#include "bt_type.h"
#include "bt_notify.h"
#include "bt_callback_manager.h"
#include "bt_common.h"

#include "gnss_log.h"

#include "epo_gprot.h"
#include "hal_rtc.h"

/*****************************************************************************
* define
*****************************************************************************/

#define MTKEPO_RECORD_SIZE 72

#define BT_NOTIFY_READ_IND   9527

#define EPO_DOWNLOAD_QUEUE_SIZE 50
/*****************************************************************************
* static function & variable
*****************************************************************************/
static void    epo_recv_data_start(bt_notify_event_data_t *p_data);
static void    epo_recv_data_pack(bt_notify_event_data_t *p_data);
static int16_t epo_recv_data_end(void);
static void    epo_init_mem_info(void);

typedef uint32_t bt_event_t;

typedef struct
{
    bt_event_t event_id;
    void *data_ptr;
}epo_queue_struct_t;

static QueueHandle_t epo_download_queue = NULL;
bt_bd_addr_t g_epo_bt_addr;



static uint8_t epo_md5[32+1];
static BaseType_t is_md5_received;
static BaseType_t is_end_pack_come;

/*****************************************************************************
* global variable & fucntion
*****************************************************************************/

/* global variable */
static epo_mem_info_t    g_epo_mem_info;


/**
* @brief       This function used to send ack to smart phone
* @param[in]   sender: Sender name
* @param[in]   receiver: Receiver name
* @param[in]   reply_code: reply code
* @return      int32_t: the read data size
*/
static void epo_reply_int_value(char* sender, char* receiver, int32_t reply_code)
{
    char data[50] = {0};
    char error[5] = {0};
    GNSSLOGD( "[EPO] epo_reply_int_value sender = %s, receiver = %s, reply_code = %d\n",
        sender, receiver, reply_code);
    if (reply_code >= 0) {
        sprintf(error, "%d", (int) reply_code);
        sprintf(data, "%s %s %d %d %s",
            sender,
            receiver,
            0,
            strlen(error),
            error);
    } else {
        reply_code = 0 - reply_code;
        sprintf(error, "%d", (int) reply_code);
        sprintf(data, "%s %s %d %d -%s",
            sender,
            receiver,
            0,
            strlen(error),
            error);
    }
}

/**
* @brief       Send EPO download request to smart phone via notification.
* @return      void
*/
static void epo_request_download()
{
    char data[80] = {0};
    char cmd[20] = {0};
    
    sprintf(cmd, "%s", "epo_download");
    
    sprintf(data, "%s %s %d %d %s",
        EPO_EXTCMD_UPDATE_DATA_SENDER,
        EPO_EXTCMD_UPDATE_DATA_RECEIVER,
        0,
        strlen(cmd),
        cmd);
    
    
    GNSSLOGD("[EPO_REQUEST]send request to download data = %s", data);
    
    bt_notify_send_data(&g_epo_bt_addr, data, strlen(data), 1);
}


/**
* @brief       Init EPO context
* @return      void
*/
static void epo_init_mem_info()
{
    GNSSLOGD( "[EPO] init mem info\n");
    g_epo_mem_info.start_address = 0x003F0000;
    g_epo_mem_info.end_address = 0x00400000;
    g_epo_mem_info.reserved_size = 992*1024;
    g_epo_mem_info.block_size = 4*1024;
    g_epo_mem_info.block_type = HAL_FLASH_BLOCK_4K;
    g_epo_mem_info.block_count = 16;//248;
    
    g_epo_mem_info.total_received = 0;
    g_epo_mem_info.write_ptr = 0;
    g_epo_mem_info.ubin_pack_count = 0;
}


/**
* @brief       Prepare for receiving data .
* @param[in]   bt_notify_event_data_t: notification data
* @return      void
*/
static void epo_recv_data_start(bt_notify_event_data_t *event_data)
{
    int32_t ret;
    GNSSLOGD("\n[EPO_PACK]start\n");
    g_epo_mem_info.write_ptr = g_epo_mem_info.start_address;
    g_epo_mem_info.ubin_pack_count = atoi((char*)event_data->data);
    is_md5_received = pdFALSE;
    is_end_pack_come = pdFALSE;
    ret = hal_flash_erase(g_epo_mem_info.write_ptr, HAL_FLASH_BLOCK_4K);
    GNSSLOGD("[EPO] erase flash = %d\n", ret);
    GNSSLOGD("[EPO]g_epo_mem_info.ubin_pack_count = %d\n", g_epo_mem_info.ubin_pack_count);
}

/**
* @brief       Handle the received data package .
* @param[in]   bt_notify_event_data_t: notification data
* @return      void
*/
static void epo_recv_data_pack(bt_notify_event_data_t *event_data)
{
    int32_t ret;
    int32_t curr_block;
    if (g_epo_mem_info.write_ptr >= g_epo_mem_info.start_address
        && g_epo_mem_info.write_ptr <= g_epo_mem_info.end_address) {
        if (g_epo_mem_info.write_ptr + event_data->length <= g_epo_mem_info.end_address) {
            curr_block = g_epo_mem_info.write_ptr >> 12;
            if (g_epo_mem_info.write_ptr == (curr_block << 12)) {
                ret = hal_flash_erase(g_epo_mem_info.write_ptr, HAL_FLASH_BLOCK_4K);
                GNSSLOGD("[EPO] erase flash = %d\n", ret);
            }
            if (g_epo_mem_info.write_ptr + event_data->length > ((curr_block + 1) << 12)) {
                ret = hal_flash_erase(((curr_block + 1) << 12), HAL_FLASH_BLOCK_4K);
                GNSSLOGD("[EPO] erase flash = %d\n", ret);
            }
            
            ret = hal_flash_write(g_epo_mem_info.write_ptr, event_data->data, event_data->length);
            if (HAL_FLASH_STATUS_OK == ret) {
                g_epo_mem_info.ubin_pack_count--;
                g_epo_mem_info.write_ptr = g_epo_mem_info.write_ptr + event_data->length;
                g_epo_mem_info.total_received = g_epo_mem_info.total_received + event_data->length;
                GNSSLOGD("[EPO] current received = %d\n", event_data->length);
                GNSSLOGD("[EPO] total received = %d\n", g_epo_mem_info.total_received);
                GNSSLOGD("[EPO] pack left = %d\n", g_epo_mem_info.ubin_pack_count);
                GNSSLOGD("[EPO] write_ptr = 0x%x\n", g_epo_mem_info.write_ptr);
            } else {
                epo_reply_int_value(EPO_EXTCMD_UPDATE_DATA_SENDER, EPO_EXTCMD_UPDATE_DATA_RECEIVER, EPO_WRITE_FLASH_FAIL);
            }
        } else {
            GNSSLOGD("[EPO] write_ptr = 0x%x\n", g_epo_mem_info.write_ptr);
            GNSSLOGD("[EPO] current_received = %d\n", event_data->length);
            GNSSLOGD("[EPO] end_address = 0x%x\n", g_epo_mem_info.end_address);
            epo_reply_int_value(EPO_EXTCMD_UPDATE_DATA_SENDER, EPO_EXTCMD_UPDATE_DATA_RECEIVER, EPO_UBIN_OVERSIZE);
        }
    }
}

/**
* @brief       Finish the EPO data recieving.
* @return      int16_t
*/
static int16_t epo_recv_data_end()
{
    GNSSLOGD("\n[EPO_PACK]end\n");
    if (g_epo_mem_info.write_ptr >= g_epo_mem_info.start_address
        && g_epo_mem_info.write_ptr <= g_epo_mem_info.end_address) {
        g_epo_mem_info.write_ptr = 0;
        if (g_epo_mem_info.ubin_pack_count == 0) {
            epo_reply_int_value(EPO_EXTCMD_UPDATE_DATA_SENDER, EPO_EXTCMD_UPDATE_DATA_RECEIVER, EPO_FILE_DATA_END);
            return EPO_PACK_END_CORRECT;
        } else {
            epo_reply_int_value(EPO_EXTCMD_UPDATE_DATA_SENDER, EPO_EXTCMD_UPDATE_DATA_RECEIVER, EPO_TRANSFER_DATA_FAIL);
            return EPO_PACK_END_WRONG;
        }
    } else {
        return EPO_PACK_END_WRONG;
    }
}

 
/**
* @brief       Notify EPO download is success or not.
* @return      void
*/
void epo_update_notify(epo_update_notify_type_t type, void* param)
{
    if (type == EPO_UPDATE_NOTIFY_TYPE_SUCCESS) {
        // checksum
        GNSSLOGD( "[EPO Demo] EPO download success!\n");
    } else {
        GNSSLOGD( "[EPO Demo] EPO download fail!\n");
    }
}


/**
* @brief       Relay BT notification's message to app task.
* @param[in]   data: notification data
* @return      void
*/
static void epo_btnotify_msg_relay(void* data)
{
    bt_notify_callback_data_t *p_data = (bt_notify_callback_data_t *)data;
    epo_queue_struct_t queue_item;
    queue_item.event_id = (bt_event_t) p_data->evt_id;
    queue_item.data_ptr = NULL;
    
    GNSSLOGD("[EPO_TEST] epo_btnotify_msg_relay, evt = %d\r\n",(int) p_data->evt_id);
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
            queue_item.data_ptr = (bt_notify_callback_data_t *)pvPortMalloc(sizeof(bt_notify_callback_data_t));
            memcpy(queue_item.data_ptr, data, sizeof(bt_notify_callback_data_t));
            break;
                                   }
        case BT_NOTIFY_EVENT_NOTIFICATION: {
            queue_item.data_ptr = (bt_notify_notification_t *)pvPortMalloc(sizeof(bt_notify_notification_t));
            memcpy(queue_item.data_ptr, data, sizeof(bt_notify_notification_t));
            break;
                                           }
        case BT_NOTIFY_EVENT_MISSED_CALL: {
            queue_item.data_ptr = (bt_notify_sms_t *)pvPortMalloc(sizeof(bt_notify_sms_t));
            memcpy(queue_item.data_ptr, data, sizeof(bt_notify_sms_t));
            break;
                                          }
        case BT_NOTIFY_EVENT_SMS: {
            queue_item.data_ptr = (bt_notify_missed_call_t *)pvPortMalloc(sizeof(bt_notify_missed_call_t));
            memcpy(queue_item.data_ptr, data, sizeof(bt_notify_missed_call_t));
            break;
                                  }
        default: {
            break;
                 }
    }
    
    
    if (xQueueSend(epo_download_queue, (void*)&queue_item, 0) != pdPASS) {
        GNSSLOGD("EPO queue full\n");
    }
}

/**
* @brief       Process BT notification's message.
* @param[in]   evt_id: BT notification event id
* @param[in]   data: notification data
* @return      void
*/
static void epo_btnotify_msg_hdlr(uint32_t evt_id, void* data)
{
    static int32_t is_connected;
    bt_notify_callback_data_t *p_data = (bt_notify_callback_data_t *)data;
    GNSSLOGD("epo_btnotify_msg_hdlr:%d", p_data->evt_id);
    switch (evt_id) {
        case BT_NOTIFY_EVENT_CONNECTION: {
            memcpy(g_epo_bt_addr, data, 6);
            /*connected with the remote device*/
            if (!is_connected) {
                epo_request_download();
                is_connected = 1;
            }
                                         }
            break;
        case BT_NOTIFY_EVENT_DISCONNECTION: {
            memset(g_epo_bt_addr, 0, 6);
            is_connected = 0;
            /*disconnected with the remote device*/
                                            }
            break;
        case BT_NOTIFY_EVENT_SEND_IND: {
            /*send  new/the rest data flow start*/
            
                                       }
            break;
        case BT_NOTIFY_EVENT_DATA_RECEIVED: {
            /*receive data*/
            if (strcmp(p_data->event_data.sender_id, EPO_EXTCMD_UPDATE_DATA_SENDER) == 0) {
                if (p_data->event_data.error_code == EPO_FILE_DATA_BEGIN) {
                    GNSSLOGD( "\n[EPO_CB]pack transfer: 0\n");
                    epo_recv_data_start(&(p_data->event_data));
                    return;
                } else if (p_data->event_data.error_code == EPO_FILE_DATA_PACK) {
                    GNSSLOGD( "\n[EPO_CB]pack transfer: 1\n");
                    epo_recv_data_pack(&(p_data->event_data));
                } else if (p_data->event_data.error_code == EPO_FILE_DATA_END) {
                    GNSSLOGD( "\n[EPO_CB]pack transfer: 2\n");
                    GNSSLOGD("\n[EPO_TRACE_ERROR_4]\n");
                    is_end_pack_come = pdTRUE;
                    if (epo_recv_data_end() == EPO_PACK_END_WRONG) {
                        epo_update_notify(EPO_UPDATE_NOTIFY_TYPE_FAIL, epo_md5);
                    } else {
                        if (is_md5_received == pdTRUE) {
                            GNSSLOGD("\n[EPO_MSG_HDLR] md5 received\n");
                            GNSSLOGD("\n[EPO_TRACE_ERROR_8]\n");
                            epo_update_notify(EPO_UPDATE_NOTIFY_TYPE_SUCCESS, epo_md5);
                        }
                    }
                } else {
                    GNSSLOGD("\n[EPO_CB]error_sender: %s or error_receiver: %s\n", p_data->event_data.sender_id, p_data->event_data.receiver_id);
                }
            } else if (strcmp(p_data->event_data.sender_id, EPO_EXTCMD_UPDATE_MD5_SENDER) == 0) {
                if (p_data->evt_id == BT_NOTIFY_EVENT_DATA_RECEIVED) {
                    if (!strcmp(p_data->event_data.sender_id, EPO_EXTCMD_UPDATE_MD5_SENDER) &&
                        !strcmp(p_data->event_data.receiver_id, EPO_EXTCMD_UPDATE_MD5_SENDER)) {
                        GNSSLOGD("\n[MD5_CB]md5 data transfer: %s\n", p_data->event_data.data);
                        is_md5_received = pdTRUE;
                        memcpy(epo_md5, p_data->event_data.data, p_data->event_data.length + 1);
                        if (is_end_pack_come == pdTRUE) {
                            if (g_epo_mem_info.ubin_pack_count == 0) {
                                epo_update_notify(EPO_UPDATE_NOTIFY_TYPE_SUCCESS, epo_md5);
                            } else {
                                epo_update_notify(EPO_UPDATE_NOTIFY_TYPE_FAIL, epo_md5);
                            }
                        } else {
                            GNSSLOGD("\n[MD5_CB]end pack not come yet\n");
                        }
                    } else {
                        GNSSLOGD("\n[MD5_CB]error_sender: %s or error_receiver: %s\n", p_data->event_data.sender_id, p_data->event_data.receiver_id);
                    }
                }
            }
                                   }
            break;
        case BT_NOTIFY_EVENT_NOTIFICATION: 
        case BT_NOTIFY_EVENT_MISSED_CALL: 
        case BT_NOTIFY_EVENT_SMS: 
            break;
        default:
            break;
    }
    if (data) {
        vPortFree(data);
    }
}

/**
* @brief       EPO download context init.
* @return      void
*/
static void epo_download_init()
{
    bt_notify_result_t reg_ret_1;
    bt_notify_result_t reg_ret_2;
    bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_GAP | MODULE_MASK_SYSTEM, (void*) gnss_bt_app_event_callback);
    bt_callback_manager_register_callback(bt_callback_type_gap_get_local_configuration, 0, (void*) gnss_bt_gap_get_local_configuration);
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_bonding_info, 0, (void*) gnss_bt_gap_le_get_bonding_info);    
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_pairing_config, 0, (void*) gnss_bt_gap_le_get_pairing_config);
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_local_cofig, 0, (void*) gnss_bt_gap_le_get_local_config);
    
    epo_init_mem_info();
    gnss_load_link_key();
    bt_notify_init(BT_SPP_SERVER_ID_START);
    
    GNSSLOGD("[EPO_INIT] begin register extend command callback hdlr\n");
    reg_ret_1 = bt_notify_register_callback(NULL, EPO_EXTCMD_UPDATE_DATA_SENDER, epo_btnotify_msg_relay);
    reg_ret_2 = bt_notify_register_callback(NULL, EPO_EXTCMD_UPDATE_MD5_SENDER, epo_btnotify_msg_relay);
    GNSSLOGD("[EPO_INIT] register resul_1: %d, result_2 = %d\n", reg_ret_1, reg_ret_2);
}


/**
* @brief             EPO download main task, receive event from bt task, store fireware to flash
* @return    void
*/
void epo_download_task(void *arg)
{
    epo_queue_struct_t queue_data_item;
    epo_download_init();
    
    epo_download_queue = xQueueCreate(EPO_DOWNLOAD_QUEUE_SIZE, sizeof(epo_queue_struct_t));
    if( epo_download_queue == NULL) {
        GNSSLOGD("[epo download]create queue failed!\r\n");
    }
    
    //main loop
    while(1) {
        if(xQueueReceive(epo_download_queue, (void *)&queue_data_item, portMAX_DELAY)) {
            epo_btnotify_msg_hdlr(queue_data_item.event_id, queue_data_item.data_ptr);
        }
    }
}

#endif /*GNSS_SUPPORT_EPO_DOWNLOAD_BY_BT*/

