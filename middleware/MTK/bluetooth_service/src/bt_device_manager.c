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
#include "bt_callback_manager.h"
#include "bt_device_manager_config.h"
#include "bt_device_manager.h"
#include "bt_gap.h"
#include "bt_gap_le.h"
#include "bt_type.h"
#include "bt_debug.h"
#include "bt_os_layer_api.h"
#include "nvdm.h"
#include <string.h>
#include <stdio.h>
#include "syslog.h"

#define __BT_DEVICE_MANAGER_DEBUG_INFO__

void bt_sink_paird_list_changed(bt_device_manager_paired_event_t event, bt_bd_addr_ptr_t address);
void default_bt_sink_paird_list_changed(bt_device_manager_paired_event_t event, bt_bd_addr_ptr_t address);
static bt_status_t bt_device_manager_delete_paired_device_int(bt_bd_addr_ptr_t address);

/* Weak symbol declaration */
#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_sink_paird_list_changed=_default_bt_sink_paird_list_changed")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_sink_paird_list_changed = default_bt_sink_paird_list_changed
#else
#error "Unsupported Platform"
#endif


#ifdef __BT_DEVICE_MANAGER_DEBUG_INFO__

log_create_module(device_manager, PRINT_LEVEL_INFO);

#define bt_device_manager_report(_message,...) LOG_I(device_manager, (_message), ##__VA_ARGS__)

static void bt_device_manager_dump_link_key(uint8_t *linkkey);

static void bt_device_manager_dump_bt_address(uint8_t *address);
#endif

void default_bt_sink_paird_list_changed(bt_device_manager_paired_event_t event, bt_bd_addr_ptr_t address)
{
    return;
}

static void bt_device_manager_notify_paired_list_changed(bt_device_manager_paired_event_t event, bt_bd_addr_ptr_t address)
{
    bt_sink_paird_list_changed(event, address);
    return;
}

typedef struct {
    uint8_t pase_index;
    bt_bd_addr_t address;
} bt_device_manager_paired_device_map_index;

typedef struct {
    bt_gap_link_key_notification_ind_t paired_key;
    char name[32];
} bt_device_manager_paired_device_store_item_t;

static bt_device_manager_paired_device_map_index bt_device_manager_paired_device_index[BT_DEVICE_MANAGER_MAX_PAIRED_NUM];
static int32_t bt_device_manager_is_init;
bt_status_t bt_device_manager_gap_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff);
void bt_device_manager_get_link_key_handler(bt_gap_link_key_notification_ind_t* key_information);
static int32_t bt_device_manager_read_paired_item(
    bt_device_manager_paired_device_store_item_t* paired_item, uint32_t index);
static int32_t bt_device_manager_store_paired_item(
    const bt_device_manager_paired_device_store_item_t* paired_item, uint32_t index);


static int32_t bt_device_manager_read_paired_item(
    bt_device_manager_paired_device_store_item_t* paired_item, uint32_t index)
{
    nvdm_status_t status;
    char buffer[12] = {0};
    uint32_t size = sizeof(bt_device_manager_paired_device_store_item_t);
    sprintf(buffer, "paired_%02x", (unsigned int)index);
    status = nvdm_read_data_item("BT", buffer, (uint8_t*)paired_item, &size);
    if (NVDM_STATUS_OK != status) {
        bt_device_manager_paired_device_index[index].pase_index = 0;
        memset(paired_item, 0, sizeof(bt_device_manager_paired_device_store_item_t));
    }
    return (int32_t)status;
}

static int32_t bt_device_manager_store_paired_item(
    const bt_device_manager_paired_device_store_item_t* paired_item, uint32_t index)
{
    nvdm_status_t status;
    char buffer[12] = {0};
    uint32_t size = sizeof(bt_device_manager_paired_device_store_item_t);
    sprintf(buffer, "paired_%02x", (unsigned int)index);
    status = nvdm_write_data_item("BT", buffer, NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t*)paired_item, size);
    return (int32_t)status;
}

void bt_device_manager_init(void)
{
    uint32_t i;
    int32_t status;
    bt_device_manager_paired_device_store_item_t paired_item;
    if (bt_device_manager_is_init == 0) {
        bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_GAP | MODULE_MASK_SYSTEM, (void*)bt_device_manager_gap_event_handler);
        bt_callback_manager_register_callback(bt_callback_type_gap_get_link_key, MODULE_MASK_GAP, (void*)bt_device_manager_get_link_key_handler);

        //TODO: read paired list.
        for(i = 0; i < BT_DEVICE_MANAGER_MAX_PAIRED_NUM; i++) {
            status = bt_device_manager_read_paired_item(&paired_item, i);
            if ((NVDM_STATUS_OK == status) && (paired_item.paired_key.key_type != BT_GAP_LINK_KEY_TYPE_INVAILIDE)) {
                bt_device_manager_paired_device_index[i].pase_index = i + 1;
                memcpy(bt_device_manager_paired_device_index[i].address, paired_item.paired_key.address, sizeof(bt_bd_addr_t));
            }
        }
        bt_device_manager_is_init = 1;
    }
}

uint32_t bt_device_manager_get_paired_number(void)
{
    uint32_t i;
    uint32_t count = 0;
    for(i = 0; i < BT_DEVICE_MANAGER_MAX_PAIRED_NUM; i++) {
        if (bt_device_manager_paired_device_index[i].pase_index != 0) {
            count++;
        }
    }
    return count;
}

void bt_device_manager_get_paired_list(bt_device_manager_paired_infomation_t* info, uint32_t* read_count)
{
    int32_t i = 0;
    uint32_t buff_size = *read_count;
    bt_device_manager_paired_device_store_item_t item;
    bt_device_manager_paired_infomation_t* p = info;
    *read_count = 0;
    while ((i < BT_DEVICE_MANAGER_MAX_PAIRED_NUM) && (*read_count < buff_size)) {
        if (bt_device_manager_paired_device_index[i].pase_index != 0) {
            bt_device_manager_read_paired_item(&item, i);
            memcpy(p->address, bt_device_manager_paired_device_index[i].address, sizeof(bt_bd_addr_t));
            memcpy(p->name, item.name, 31);
            (*read_count)++;
            p++;
        }
        i++;
    }
}

bool bt_device_manager_is_paired(bt_bd_addr_ptr_t address)
{
    int32_t i;
    for(i = 0; i < BT_DEVICE_MANAGER_MAX_PAIRED_NUM - 1; i++) {
        if (memcmp(address, bt_device_manager_paired_device_index[i].address, sizeof(bt_bd_addr_t)) == 0) {
            return true;
        }
    }
    return false;
}

bt_status_t bt_device_manager_delete_paired_device(bt_bd_addr_ptr_t address)
{
    bt_status_t status;
    status = bt_device_manager_delete_paired_device_int(address);
    if (BT_STATUS_SUCCESS == status) {
        bt_device_manager_notify_paired_list_changed(BT_DEVICE_MANAGER_PAIRED_UNPAIR, address);
    }
    return status;
}

static bt_status_t bt_device_manager_delete_paired_device_int(bt_bd_addr_ptr_t address)
{
    uint32_t i, j = 0xFF;
    bt_device_manager_paired_device_store_item_t item;

    for(i = 0; i < BT_DEVICE_MANAGER_MAX_PAIRED_NUM - 1; i++) {
        if (memcmp(address, bt_device_manager_paired_device_index[i].address, sizeof(bt_bd_addr_t)) == 0) {
            j = i;
            break;
        }
    }
    if (j != 0xFF) {
        for(i = 0; i < BT_DEVICE_MANAGER_MAX_PAIRED_NUM - 1; i++) {
            if (bt_device_manager_paired_device_index[i].pase_index > bt_device_manager_paired_device_index[j].pase_index) {
                bt_device_manager_paired_device_index[i].pase_index--;
            }
        }
        memset(&item, 0, sizeof(bt_device_manager_paired_device_store_item_t));
        item.paired_key.key_type = BT_GAP_LINK_KEY_TYPE_INVAILIDE;
        bt_device_manager_store_paired_item(&item, j);
        memset(bt_device_manager_paired_device_index + j, 0, sizeof(bt_device_manager_paired_device_map_index));
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

bt_status_t bt_device_manager_gap_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    //bt_status_t st;
    switch (msg) {
        case BT_GAP_IO_CAPABILITY_REQ_IND:
        {
            bt_gap_reply_io_capability_request(BT_GAP_OOB_DATA_PRESENTED_NONE, BT_GAP_SECURITY_AUTH_REQUEST_GENERAL_BONDING_AUTO_ACCEPTED);
            break;
        }
        case BT_GAP_LINK_KEY_NOTIFICATION_IND:
        {
            // This event is received before BT_GAP_BONDING_COMPLETE_IND and bonding success or the old link key is phased out.
            bt_gap_link_key_notification_ind_t * key_info = (bt_gap_link_key_notification_ind_t *) buff;
            bt_device_manager_paired_device_store_item_t item;
            bt_device_manager_paired_device_map_index delete_item =
            {
                .pase_index = 0,
            };
            uint32_t i, j = 0xFF;
            memset(&item, 0, sizeof(bt_device_manager_paired_device_store_item_t));
            bt_device_manager_delete_paired_device_int(key_info->address);
            if (key_info->key_type != BT_GAP_LINK_KEY_TYPE_INVAILIDE) {
                 // Save the new link key, and key_type to link key database.
                for(i = 0; i < BT_DEVICE_MANAGER_MAX_PAIRED_NUM; i++) {
                    if (bt_device_manager_paired_device_index[i].pase_index > 0) {
                        bt_device_manager_paired_device_index[i].pase_index++;
                        if (bt_device_manager_paired_device_index[i].pase_index == BT_DEVICE_MANAGER_MAX_PAIRED_NUM) {
                            j = i;
                            memcpy(&delete_item, bt_device_manager_paired_device_index + i, sizeof(bt_device_manager_paired_device_map_index));
                        }
                    }
                    else if (j == 0xFF) {
                        j = i;
                    }
                }
                bt_device_manager_paired_device_index[j].pase_index = 1;
                memcpy(bt_device_manager_paired_device_index[j].address, key_info->address, sizeof(bt_bd_addr_t));
                memcpy(&(item.paired_key), key_info, sizeof(bt_gap_link_key_notification_ind_t));
                bt_device_manager_store_paired_item(&item, j);
                bt_gap_read_remote_name((const bt_bd_addr_t*)(&(bt_device_manager_paired_device_index[j].address)));
                if (delete_item.pase_index != 0) {
                    bt_device_manager_notify_paired_list_changed(BT_DEVICE_MANAGER_PAIRED_LIST_FULL, delete_item.address);
                }
                bt_device_manager_notify_paired_list_changed(BT_DEVICE_MANAGER_PAIRED_ADD, key_info->address);
            }
            else {
                bt_device_manager_notify_paired_list_changed(BT_DEVICE_MANAGER_PAIRED_KEY_MISS, key_info->address);
            }    
            #ifdef __BT_DEVICE_MANAGER_DEBUG_INFO__
            bt_device_manager_dump_bt_address((uint8_t *)key_info->address);
            bt_device_manager_dump_link_key((uint8_t *)key_info->key);
            #endif
            break;
        }
        case BT_GAP_READ_REMOTE_NAME_COMPLETE_IND:
        {
            bt_gap_read_remote_name_complete_ind_t* p = (bt_gap_read_remote_name_complete_ind_t*)buff;
            bt_device_manager_paired_device_store_item_t paired_item;
            int32_t i;
            for(i = 0; i < BT_DEVICE_MANAGER_MAX_PAIRED_NUM - 1; i++) {
                if (memcmp(p->address, bt_device_manager_paired_device_index[i].address, sizeof(bt_bd_addr_t)) == 0) {
                    bt_device_manager_read_paired_item(&paired_item, i);
                    strncpy(paired_item.name, p->name, 31);
                    bt_device_manager_store_paired_item(&paired_item, i);
                    break;
                }
            }
            break;
        }
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

void bt_device_manager_get_link_key_handler(bt_gap_link_key_notification_ind_t* key_information)
{
    bt_device_manager_paired_device_store_item_t paired_item;
    int32_t i;
    for(i = 0; i < BT_DEVICE_MANAGER_MAX_PAIRED_NUM - 1; i++) {
        if (memcmp(key_information->address, bt_device_manager_paired_device_index[i].address, sizeof(bt_bd_addr_t)) == 0) {
            bt_device_manager_read_paired_item(&paired_item, i);
            memcpy(key_information, &(paired_item.paired_key), sizeof(bt_gap_link_key_notification_ind_t));
            #ifdef __BT_DEVICE_MANAGER_DEBUG_INFO__
            bt_device_manager_dump_bt_address((uint8_t *)key_information->address);
            bt_device_manager_dump_link_key((uint8_t *)key_information->key);
            #endif
            break;
        }
    }
}

bt_status_t bt_device_manager_unpair_all(void)
{
    int32_t i;
    bt_device_manager_paired_device_store_item_t item;
    memset(bt_device_manager_paired_device_index, 0, sizeof(bt_device_manager_paired_device_map_index) * BT_DEVICE_MANAGER_MAX_PAIRED_NUM);
    memset(&item, 0, sizeof(bt_device_manager_paired_device_store_item_t));
    item.paired_key.key_type = BT_GAP_LINK_KEY_TYPE_INVAILIDE;

    for(i = 0; i < BT_DEVICE_MANAGER_MAX_PAIRED_NUM - 1; i++) {
        bt_device_manager_store_paired_item(&item, i);
    }
    bt_device_manager_notify_paired_list_changed(BT_DEVICE_MANAGER_PAIRED_UNPAIR_ALL, NULL);
    return BT_STATUS_SUCCESS;
}

#ifdef __BT_DEVICE_MANAGER_DEBUG_INFO__
static void bt_device_manager_dump_link_key(uint8_t *linkkey)
{
    bt_device_manager_report("[BT_DM] link key:%02x,%02x,%02x,%02x,  %02x,%02x,%02x,%02x,  %02x,%02x,%02x,%02x,  %02x,%02x,%02x,%02x",
                       linkkey[0], linkkey[1], linkkey[2], linkkey[3], linkkey[4], linkkey[5], linkkey[6], linkkey[7],
                       linkkey[8], linkkey[9], linkkey[10], linkkey[11], linkkey[12], linkkey[13], linkkey[14], linkkey[15]);
}

static void bt_device_manager_dump_bt_address(uint8_t *address)
{
    bt_device_manager_report("[BT_DM] address:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x",
         address[5], address[4], address[3], address[2], address[1], address[0]);
}
#endif

