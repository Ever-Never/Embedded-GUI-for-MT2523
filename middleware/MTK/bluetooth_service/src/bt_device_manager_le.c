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

#include "bt_device_manager_le_config.h"
#include "bt_device_manager_le.h"
#include "bt_callback_manager.h"
#include "hal_trng.h"
#include "bt_system.h"
#include <string.h>
#include "syslog.h"
#include "nvdm.h"
#include <stdlib.h>



log_create_module(BT_DM, PRINT_LEVEL_INFO);

#define BT_ADDR_TYPE_UNKNOW (0xFF)

/**
 *  @brief Connection Structure, internal use.
 */
typedef struct {
    bt_handle_t                                       connection_handle; /**< Connection handle. */    
    bt_device_manager_le_connection_param_t          connection_params; /**< Connection parameters. */
} bt_device_manager_le_connection_struct_t;

/**
 *  @brief Bongding infomation structure, internal use.
 */
typedef struct {
    bt_addr_t bt_addr;             /**< The address of a remote device to be bonded. */
    bt_gap_le_bonding_info_t info; /**< The connection bonding information. */
} bt_device_manager_le_bonded_info_t;


const char *group_name = "BT_DM";
static uint8_t g_next_resolved = 0;
static bool g_nvram_read_flag = false;
static bool bt_dm_le_initialized = false;
static bool bt_dm_clear_flag = false;
static const bt_addr_t default_bt_addr = {
                       .type = BT_ADDR_TYPE_UNKNOW,
                       .addr = {0}
                       };

static bt_bd_addr_t bt_device_manager_le_local_public_addr = {0};
bt_device_manager_le_connection_struct_t dm_info[BT_DEVICE_MANAGER_LE_CONNECTION_MAX] = {{0}};
bt_device_manager_le_bonded_info_t bonded_info[BT_DEVICE_MANAGER_LE_BONDED_MAX] = {{{0}}};

/** default secure configuration. */
static bool g_sc_only_default = false;
static bt_gap_le_local_config_req_ind_t g_local_config_default;
static bt_gap_le_smp_pairing_config_t g_pairing_config_req_default = {
    .io_capability = BT_GAP_LE_SMP_NO_INPUT_NO_OUTPUT,
    .auth_req = BT_GAP_LE_SMP_AUTH_REQ_NO_BONDING,
    .maximum_encryption_key_size = 16,
};
static bt_gap_le_local_key_t g_local_key_req_default = {
    .encryption_info = {{0}},
    .master_id = {0},
    .identity_info = {{0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x19, 0x28, 0x55, 0x33, 0x68, 0x33, 0x56, 0xde}},
    .signing_info = {{0}}
};


static void bt_device_manager_le_reset_bonded_infos(void);
static void bt_device_manager_le_gen_public_address(void);
static void bt_device_manager_le_reset_connection_infos(void);
static void bt_device_manager_le_save_connection_params(void *buff);
static void bt_device_manager_le_delete_connection_params(void *buff);
static void bt_device_manager_le_event_callback_register(void);
static bt_status_t bt_device_manager_le_add_resolving_list(uint8_t index);
static void bt_device_manager_le_store_bonded_info_to_nvdm(uint8_t index);
static uint8_t bt_device_manager_le_get_index_by_address(bt_addr_t *address);
static void bt_device_manager_le_restore_bonded_info_from_nvdm(uint8_t index);
static void bt_device_manager_le_update_current_conn_interval(void *conn_params);
static bt_gap_le_local_config_req_ind_t *bt_device_manager_le_get_local_config(void);
static bt_status_t bt_device_manager_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind);
static bt_gap_le_bonding_info_t *bt_device_manager_le_get_bonding_info(const bt_addr_t remote_addr);
static bt_status_t bt_device_manager_le_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff);
static bt_device_manager_le_bonded_info_t* bt_device_manager_le_get_bonded_info_by_handle(bt_handle_t conn_handle);
static bt_device_manager_le_bonded_info_t* bt_device_manager_le_get_or_new_bonded_info(const bt_addr_t *peer_addr, bool new_flag);


void bt_device_manager_le_init(void)
{
    LOG_I(BT_DM, "%s: enter\r\n", __FUNCTION__);

    if (bt_dm_le_initialized) {  
        LOG_I(BT_DM, "%s: Others application had initialized BT DM LE module!\r\n", __FUNCTION__);
        return;
    } else {
        bt_dm_le_initialized = true; 
        bt_device_manager_le_reset_bonded_infos();
        bt_device_manager_le_reset_connection_infos();
        bt_device_manager_le_event_callback_register();   
        if (!g_nvram_read_flag) {
            g_nvram_read_flag = true;
            bt_device_manager_le_restore_bonded_info_from_nvdm(0);
        }
        LOG_I(BT_DM, "%s: init success!\r\n", __FUNCTION__);
    }
}

static void bt_device_manager_le_event_callback_register(void)
{  
    LOG_I(BT_DM, "%s: enter\r\n", __FUNCTION__);
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_bonding_info, 0, (void *)bt_device_manager_le_get_bonding_info);
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_local_cofig, 0, (void *)bt_device_manager_le_get_local_config);
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_pairing_config, 0, (void *)bt_device_manager_le_get_pairing_config);   
    bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_SYSTEM | MODULE_MASK_GAP, (void *)bt_device_manager_le_event_callback);
}

static bt_status_t bt_device_manager_le_add_dev_2_resolving_list(const bt_gap_le_bonding_info_t *bonded_info)
{
    bt_status_t st = BT_STATUS_SUCCESS;
    
    LOG_I(BT_DM, "%s:\r\n", __FUNCTION__);
    if (BT_ADDR_TYPE_UNKNOW != bonded_info->identity_addr.address.type){
        bt_hci_cmd_le_add_device_to_resolving_list_t dev;
        dev.peer_identity_address = bonded_info->identity_addr.address;
        LOG_I(BT_DM, "peer identity address: %02X:%02X:%02X:%02X:%02X:%02X \r\n",
            dev.peer_identity_address.addr[5],
            dev.peer_identity_address.addr[4],
            dev.peer_identity_address.addr[3],
            dev.peer_identity_address.addr[2],
            dev.peer_identity_address.addr[1],
            dev.peer_identity_address.addr[0]
            );
        memcpy(dev.peer_irk, &(bonded_info->identity_info), sizeof(dev.peer_irk));
        memcpy(dev.local_irk, &(bonded_info->local_key.identity_info), sizeof(dev.local_irk));
        st = bt_gap_le_set_resolving_list(BT_GAP_LE_ADD_TO_RESOLVING_LIST, (void*)&dev);
        if (BT_STATUS_OUT_OF_MEMORY == st) {
            LOG_I(BT_DM, "Add device to Resolving List Failed [OOM]\r\n");
        }
    }
    return st;
}

static bt_status_t bt_device_manager_le_delete_dev_from_resolving_list(const bt_gap_le_bonding_info_t *bonded_info)
{
    bt_status_t st = BT_STATUS_SUCCESS;
    
    LOG_I(BT_DM, "%s:\r\n", __FUNCTION__);
    if (BT_ADDR_TYPE_UNKNOW != bonded_info->identity_addr.address.type){
        bt_hci_cmd_le_remove_device_from_resolving_list_t dev;
        dev.peer_identity_address = bonded_info->identity_addr.address;
        st = bt_gap_le_set_resolving_list(BT_GAP_LE_REMOVE_FROM_RESOLVING_LIST, (void*)&dev);
        if (BT_STATUS_OUT_OF_MEMORY == st) {
            LOG_I(BT_DM, "Delete device from Resolving List Failed [OOM]\r\n");
        }
    }
    return st;
}

static bt_status_t bt_device_manager_le_add_resolving_list(uint8_t index)
{
    for (int i = index; i < BT_DEVICE_MANAGER_LE_BONDED_MAX; i++) { 
        bt_device_manager_le_bonded_info_t *bond_info;
        bond_info = &(bonded_info[i]); 
        
        if (0 == memcmp(&default_bt_addr, &(bond_info->bt_addr), sizeof(bt_addr_t))) {
            return BT_STATUS_FAIL;
        } else {
            index++;
            if (BT_ADDR_TYPE_UNKNOW != bond_info->info.identity_addr.address.type) {                
                //update resolving list
                bt_device_manager_le_add_dev_2_resolving_list(&(bond_info->info)); 
                bt_gap_le_set_address_resolution_enable(1);
                g_next_resolved = index;
                return BT_STATUS_SUCCESS;
            }
        }
    }
    return BT_STATUS_FAIL;
}

static bt_status_t bt_device_manager_le_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    LOG_I(BT_DM, "%s: status(0x%04x), msg(0x%04x)\r\n", __FUNCTION__, status, msg);

    if (BT_STATUS_SUCCESS != status)
        return BT_STATUS_SUCCESS;
    
    switch (msg)
    {
        case BT_POWER_ON_CNF:
        {
            bt_device_manager_le_add_resolving_list(g_next_resolved);
        } 
        #if 0
        for (int i = 0; i < BT_DEVICE_MANAGER_LE_BONDED_MAX; i++) {
            
            bt_device_manager_le_bonded_info_t *bond_info;
            bond_info = &(bonded_info[i]); 
            if ((NULL != bond_info) && (BT_ADDR_TYPE_UNKNOW != bond_info->info.identity_addr.address.type)) {                
                //update resolving list
                bt_device_manager_le_add_dev_2_resolving_list(&(bond_info->info)); 
                bt_gap_le_set_address_resolution_enable(1);
            }
        }
        #endif
            break;
        
        case BT_GAP_LE_SET_RESOLVING_LIST_CNF:
        {
            bt_status_t sta;
            sta = bt_device_manager_le_add_resolving_list(g_next_resolved);
            if (BT_STATUS_FAIL == sta) {
                g_next_resolved = 0;
            }
        }
            break;
        case BT_GAP_LE_CONNECT_IND:
        {
            const bt_gap_le_connection_ind_t *connect_ind = (bt_gap_le_connection_ind_t *)buff;
            bt_device_manager_le_save_connection_params(buff);
            LOG_I(BT_DM, "connection role =%s\r\n", (connect_ind->role == BT_ROLE_MASTER) ? "Master" : "Slave");
        }
            break;      
            
        case BT_GAP_LE_CONNECTION_UPDATE_IND:
            bt_device_manager_le_update_current_conn_interval(buff);
            break;

        case BT_GAP_LE_DISCONNECT_IND:
            bt_device_manager_le_delete_connection_params(buff);
            break;
      
        case BT_GAP_LE_BONDING_COMPLETE_IND:
        {  
            nvdm_status_t write_status;  
            char data_item_name[16] = {0};
            bt_gap_le_bonding_complete_ind_t *ind = (bt_gap_le_bonding_complete_ind_t*)buff;
            LOG_I(BT_DM, "Bond complete, conn_handle(0x%04x).\r\n", ind->handle);

            bt_device_manager_le_bonded_info_t *bond_info = bt_device_manager_le_get_bonded_info_by_handle(ind->handle);
            if (NULL != bond_info) {
                uint8_t i = 255;
                #if 1
                /* If we got IRK/Identity address from peer, we have to change
                 * bonding info's bd address; bt_device_manager_le_bonded_info_t. */
                if (BT_ADDR_TYPE_UNKNOW != bond_info->info.identity_addr.address.type){                
                    /* Because value of bonded_info->info.identity_addr.address_type is 0[Public Identity] or 1[Random Identity],
                     *but Identity address type were definied 2 or 3 in spec.
                     *We have to "+2" for synchronization. */
                    bond_info->bt_addr = bond_info->info.identity_addr.address;

                    //update resolving list
                    bt_device_manager_le_add_dev_2_resolving_list(&(bond_info->info));  
                    bt_gap_le_set_address_resolution_enable(1);
                }
                #endif
                i = bt_device_manager_le_get_index_by_address(&(bond_info->bt_addr));
                if (255 != i) {
                    sprintf(data_item_name, "bonded_info_%02x", (unsigned int)i);
                    write_status = nvdm_write_data_item(group_name, (const char *)data_item_name, NVDM_DATA_ITEM_TYPE_RAW_DATA, (const uint8_t *)bond_info, sizeof(bt_device_manager_le_bonded_info_t));  
                    if (write_status == NVDM_STATUS_OK) { 
                        LOG_I(BT_DM, "Write nvdm success, bonded_info[%d].\r\n", i);
                    } else {
                        LOG_I(BT_DM, "Write nvdm fail, bonded_info[%d], write nvdm status = %d.\r\n", i, write_status);
                    }
                }
            }

        }   
            break;

        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

static bt_gap_le_local_config_req_ind_t *bt_device_manager_le_get_local_config(void)
{
    LOG_I(BT_DM, "%s: enter\r\n", __FUNCTION__);
    g_local_config_default.local_key_req = &g_local_key_req_default;
    g_local_config_default.sc_only_mode_req = g_sc_only_default;
    return &g_local_config_default;
}

static bt_status_t bt_device_manager_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind)
{
    LOG_I(BT_DM, "%s: lost_bond(%d)\r\n", __FUNCTION__, ind->is_previously_bonded);
    ind->pairing_config_req = g_pairing_config_req_default;
    return BT_STATUS_SUCCESS;
}

static bt_gap_le_bonding_info_t *bt_device_manager_le_get_bonding_info(const bt_addr_t remote_addr)
{
    LOG_I(BT_DM, "%s: enter\r\n", __FUNCTION__);
    //return &(bt_device_manager_le_get_or_new_bonded_info(&remote_addr, true)->info);
    LOG_I(BT_DM, "get bonding info address: %02X:%02X:%02X:%02X:%02X:%02X \r\n",
            remote_addr.addr[5],
            remote_addr.addr[4],
            remote_addr.addr[3],
            remote_addr.addr[2],
            remote_addr.addr[1],
            remote_addr.addr[0]
            );
    bt_device_manager_le_bonded_info_t *dm_bonded_info = bt_device_manager_le_get_or_new_bonded_info(&remote_addr, true);
    if (dm_bonded_info) {
        return &(dm_bonded_info->info);
    }
    return NULL;
}


bt_status_t bt_device_manager_le_gap_set_local_configuration(bt_gap_le_local_key_t *local_key, bool sc_only_mode)
{ 
    LOG_I(BT_DM, "%s: enter\r\n", __FUNCTION__);
    
    if (!bt_dm_le_initialized) {
        LOG_I(BT_DM, "Set Local config fail, please init BT DM LE first!\r\n");
        return BT_STATUS_FAIL;
    }
    memset(&g_local_key_req_default, 0x00, sizeof(bt_gap_le_local_key_t));
    memcpy(&g_local_key_req_default, local_key, sizeof(bt_gap_le_local_key_t));
    g_sc_only_default = sc_only_mode;
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_device_manager_le_gap_set_pairing_configuration(bt_gap_le_smp_pairing_config_t *pairing_config)
{   
    LOG_I(BT_DM, "%s: enter\r\n", __FUNCTION__);
    
    if (!bt_dm_le_initialized) {
        LOG_I(BT_DM, "Set Pairing config fail, please init BT DM LE first!\r\n");
        return BT_STATUS_FAIL;
    }
    memset(&g_pairing_config_req_default, 0x00, sizeof(bt_gap_le_smp_pairing_config_t));
    g_pairing_config_req_default.io_capability = pairing_config->io_capability;
    g_pairing_config_req_default.oob_data_flag = pairing_config->oob_data_flag;
    g_pairing_config_req_default.auth_req = pairing_config->auth_req;
    g_pairing_config_req_default.maximum_encryption_key_size = pairing_config->maximum_encryption_key_size;
    g_pairing_config_req_default.initiator_key_distribution = pairing_config->initiator_key_distribution;
    g_pairing_config_req_default.responder_key_distribution = pairing_config->responder_key_distribution;
    return BT_STATUS_SUCCESS;
}

static void bt_device_manager_le_store_bonded_info_to_nvdm(uint8_t index)
{  
    uint8_t i;
    nvdm_status_t write_status;
    bt_device_manager_le_bonded_info_t *bond_info;
    char data_item_name[16] = {0};
    LOG_I(BT_DM, "%s: enter\r\n", __FUNCTION__);

    for (i = index; i < BT_DEVICE_MANAGER_LE_BONDED_MAX; i++) {
        /**< if it is the end unit.*/
        if (0 == memcmp(&default_bt_addr, &(bonded_info[i].bt_addr), sizeof(default_bt_addr))) {
            sprintf(data_item_name, "bonded_info_%02x", (unsigned int)i);
                bond_info = &(bonded_info[i]);
            write_status = nvdm_write_data_item(group_name, (const char *)data_item_name, NVDM_DATA_ITEM_TYPE_RAW_DATA, (const uint8_t *)bond_info, sizeof(bt_device_manager_le_bonded_info_t));  
            if (write_status == NVDM_STATUS_OK) { 
                LOG_I(BT_DM, "Write nvdm success, bonded_info[%d].\r\n", i);
            } else {
                LOG_I(BT_DM, "Write nvdm fail, bonded_info[%d], write nvdm status = %d.\r\n", i, write_status);
            }
            return ;
        }
        if (0 != memcmp(&default_bt_addr, &(bonded_info[i].bt_addr), sizeof(default_bt_addr))) {
            sprintf(data_item_name, "bonded_info_%02x", (unsigned int)i);
            bond_info = &(bonded_info[i]);
            write_status = nvdm_write_data_item(group_name, (const char *)data_item_name, NVDM_DATA_ITEM_TYPE_RAW_DATA, (const uint8_t *)bond_info, sizeof(bt_device_manager_le_bonded_info_t));  
            if (write_status == NVDM_STATUS_OK) { 
                LOG_I(BT_DM, "Write nvdm success, bonded_info[%d].\r\n", i);
            } else {
                LOG_I(BT_DM, "Write nvdm fail, bonded_info[%d], write nvdm status = %d.\r\n", i, write_status);
            }
        }
    }
}

static void bt_device_manager_le_restore_bonded_info_from_nvdm(uint8_t index)
{   
    uint8_t i;
    nvdm_status_t read_status;
    bt_device_manager_le_bonded_info_t *bond_info;
    char data_item_name[16] = {0};
    LOG_I(BT_DM, "%s: enter\r\n", __FUNCTION__);

    for (i = index; i < BT_DEVICE_MANAGER_LE_BONDED_MAX; i++) {
        uint32_t size = sizeof(bt_device_manager_le_bonded_info_t);
        sprintf(data_item_name, "bonded_info_%02x", (unsigned int)i);
        bond_info = &(bonded_info[i]);
        read_status = nvdm_read_data_item(group_name, (const char *)data_item_name, (uint8_t *)bond_info, &size);  
        if (read_status == NVDM_STATUS_OK) {  
            LOG_I(BT_DM, "Read nvdm success, bonded_info[%d].\r\n", i);
            if (0 == memcmp(&default_bt_addr, &(bonded_info[i].bt_addr), sizeof(default_bt_addr))) {
                LOG_I(BT_DM, "Read all nvdm success, all num is %d.\r\n", i);
                break;
            }
        } else {
            LOG_I(BT_DM, "Read nvdm fail, bonded_info[%d], read nvdm status = %d.\r\n", i, read_status);
            break;
        }
    }
}

static bt_device_manager_le_bonded_info_t* bt_device_manager_le_get_or_new_bonded_info(const bt_addr_t *peer_addr, bool new_flag)
{
    uint8_t i;
    LOG_I(BT_DM, "%s: unbond flag is (%d)\r\n", __FUNCTION__, new_flag);
    
    if (!g_nvram_read_flag) {
        g_nvram_read_flag = true;
        bt_device_manager_le_restore_bonded_info_from_nvdm(0);
    }
    /** Check whether bonded? */
    for (i = 0; i< BT_DEVICE_MANAGER_LE_BONDED_MAX ; i++) {
        if (0 == memcmp(peer_addr, &(bonded_info[i].bt_addr), sizeof(bt_addr_t))) {
            LOG_I(BT_DM, "Have Bonded, return bonded_info[%d].\r\n", i);
            return &(bonded_info[i]);
        }
    }
    /** unbonded, so need new a buffer. */
    if (new_flag) {
        for (i = 0; i< BT_DEVICE_MANAGER_LE_BONDED_MAX ; i++) {
            if (0 == memcmp(&default_bt_addr, &(bonded_info[i].bt_addr), sizeof(bt_addr_t))) {
                bonded_info[i].info.identity_addr.address.type = BT_ADDR_TYPE_UNKNOW;
                memcpy(&(bonded_info[i].bt_addr), peer_addr, sizeof(bt_addr_t));
                LOG_I(BT_DM, "Un-Bonded, return bonded_info[%d].\r\n", i);
                return &(bonded_info[i]);
            }
        }
        /**have no empty buffer, so delete the oldest one, and return the last one buffer. */
        if (i == BT_DEVICE_MANAGER_LE_BONDED_MAX) {
            uint8_t j;
            LOG_I(BT_DM, "No empty buffer, Need to delete the oldest one!\r\n");
            bt_device_manager_le_delete_dev_from_resolving_list((&(bonded_info[0].info)));
            memset(&(bonded_info[0]), 0x00, sizeof(bt_device_manager_le_bonded_info_t));
            bonded_info[0].bt_addr.type = BT_ADDR_TYPE_UNKNOW;
            bonded_info[0].info.identity_addr.address.type = BT_ADDR_TYPE_UNKNOW;
            for (j = 0; j< (BT_DEVICE_MANAGER_LE_BONDED_MAX - 1); j++) {
                bt_device_manager_le_delete_dev_from_resolving_list((&(bonded_info[j].info)));
                memcpy(&(bonded_info[j]), &(bonded_info[j+1]), sizeof(bt_device_manager_le_bonded_info_t));
                memset(&(bonded_info[j+1]), 0x00, sizeof(bt_device_manager_le_bonded_info_t));
                bonded_info[j+1].bt_addr.type = BT_ADDR_TYPE_UNKNOW;
                bonded_info[j+1].info.identity_addr.address.type = BT_ADDR_TYPE_UNKNOW;    
            }
            bt_device_manager_le_store_bonded_info_to_nvdm(0);
            memcpy(&(bonded_info[BT_DEVICE_MANAGER_LE_BONDED_MAX - 1].bt_addr), peer_addr, sizeof(bt_addr_t));
            LOG_I(BT_DM, "No empty buffer, return bonded_info[%d].\r\n", BT_DEVICE_MANAGER_LE_BONDED_MAX - 1);
            return &(bonded_info[BT_DEVICE_MANAGER_LE_BONDED_MAX - 1]);
        }

    }
    return NULL;
}

static bt_device_manager_le_bonded_info_t* bt_device_manager_le_get_bonded_info_by_handle(bt_handle_t conn_handle)
{
    bt_status_t status;
    LOG_I(BT_DM, "%s: conn_handle(0x%04x)\r\n", __FUNCTION__, conn_handle);

    bt_gap_le_connection_information_t con;
    memset(&(con), 0x00, sizeof(bt_gap_le_connection_information_t));
    status = bt_gap_le_get_connection_information(conn_handle, &con);
    if (BT_STATUS_SUCCESS == status) {
        return bt_device_manager_le_get_or_new_bonded_info(&(con.peer_addr), false);
    }
    return NULL;
}

static uint8_t bt_device_manager_le_get_index_by_address(bt_addr_t *address)
{
    uint8_t i;
    LOG_I(BT_DM, "%s:enter\r\n", __FUNCTION__);

    if (0 == memcmp(&default_bt_addr, address, sizeof(default_bt_addr))) {
        LOG_I(BT_DM, "empty address for find!\r\n");
        return 255;
    }
    for (i = 0; i< BT_DEVICE_MANAGER_LE_BONDED_MAX ; i++) {
        if (0 == memcmp(address, &(bonded_info[i].bt_addr), sizeof(bt_addr_t))) {
            return i;
        }
    } 
    if (i == BT_DEVICE_MANAGER_LE_BONDED_MAX) {
        LOG_I(BT_DM, "bt_device_manager_le_get_index_by_address, not find !\r\n");
        return 255;
    }
    return 255;
}

void bt_device_manager_le_remove_bonded_device(bt_addr_t *peer_addr)
{
    uint8_t i, j;
    LOG_I(BT_DM, "%s: enter\r\n", __FUNCTION__);
    
    if (!bt_dm_le_initialized) {
        LOG_I(BT_DM, "Remove bonded device fail, please init BT DM LE first!\r\n");
        return;
    }
    for (i = 0; i < BT_DEVICE_MANAGER_LE_BONDED_MAX; i++) {
        if (0 == memcmp(peer_addr, &(bonded_info[i].bt_addr), sizeof(bt_addr_t))) {
            bt_device_manager_le_delete_dev_from_resolving_list((&(bonded_info[i].info)));
            memset(&(bonded_info[i]), 0x00, sizeof(bt_device_manager_le_bonded_info_t));
            bonded_info[i].bt_addr.type = BT_ADDR_TYPE_UNKNOW;
            bonded_info[i].info.identity_addr.address.type = BT_ADDR_TYPE_UNKNOW;
            LOG_I(BT_DM, "Remove bonded info for index  %d\r\n", i);
            for (j = i; j< (BT_DEVICE_MANAGER_LE_BONDED_MAX - 1); j++) {
                bt_device_manager_le_delete_dev_from_resolving_list((&(bonded_info[j].info)));
                memcpy(&(bonded_info[j]), &(bonded_info[j+1]), sizeof(bt_device_manager_le_bonded_info_t));
                memset(&(bonded_info[j+1]), 0x00, sizeof(bt_device_manager_le_bonded_info_t));
                bonded_info[j+1].bt_addr.type = BT_ADDR_TYPE_UNKNOW;
                bonded_info[j+1].info.identity_addr.address.type = BT_ADDR_TYPE_UNKNOW;    
            }
            bt_device_manager_le_store_bonded_info_to_nvdm(i);
            break;
        }
    }
    
    if (i == BT_DEVICE_MANAGER_LE_BONDED_MAX) {
        LOG_I(BT_DM, "Remove bonded info fail, because can not find it!\r\n");
    }
}

static void bt_device_manager_le_reset_bonded_infos(void)
{
    uint8_t i;
    LOG_I(BT_DM, "%s: enter\r\n", __FUNCTION__);

    g_nvram_read_flag = false;
    for (i = 0; i< BT_DEVICE_MANAGER_LE_BONDED_MAX ; i++) {
        if (bt_dm_clear_flag) {
            bt_device_manager_le_delete_dev_from_resolving_list((&(bonded_info[i].info)));
        }
        memset(&(bonded_info[i]), 0x00, sizeof(bt_device_manager_le_bonded_info_t));
        bonded_info[i].bt_addr.type = BT_ADDR_TYPE_UNKNOW;
        bonded_info[i].info.identity_addr.address.type = BT_ADDR_TYPE_UNKNOW;
    }
    bt_dm_clear_flag = false;
}

void bt_device_manager_le_clear_all_bonded_info(void)
{
    LOG_I(BT_DM, "%s: enter\r\n", __FUNCTION__);
    if (!bt_dm_le_initialized) {
        LOG_I(BT_DM, "Clear bonded info fail, please init BT DM LE first!\r\n");
        return;
    }
    bt_dm_clear_flag = true;
    bt_device_manager_le_reset_bonded_infos();
    bt_device_manager_le_store_bonded_info_to_nvdm(0);
}

bool bt_device_manager_le_is_bonded(bt_addr_t *address)
{
    uint8_t i;
        
    if (!bt_dm_le_initialized) {
        LOG_I(BT_DM, "Device bonded status check fail, please init BT DM LE first!\r\n");
        return false;
    }
    for(i = 0; i < BT_DEVICE_MANAGER_LE_BONDED_MAX; i++) {
        if (0 == memcmp(address, &(bonded_info[i].bt_addr), sizeof(bt_addr_t))) {
            LOG_I(BT_DM, "%s: device have bonded\r\n", __FUNCTION__);
            return true;
        }
    }
    return false;
}

uint8_t bt_device_manager_le_get_bonded_number(void)
{
    uint8_t i;
    uint8_t count = 0;
    
    if (!bt_dm_le_initialized) {
        LOG_I(BT_DM, "Get bonded device number fail, please init BT DM LE first!\r\n");
        return 0;
    }
    for(i = 0; i < BT_DEVICE_MANAGER_LE_BONDED_MAX; i++) {
        if (0 == memcmp(&default_bt_addr, &(bonded_info[i].bt_addr), sizeof(bt_addr_t))) {
            count++;
        }
    }
    LOG_I(BT_DM, "%s:bonded number is %d\r\n", __FUNCTION__, count);
    return count;
}

void bt_device_manager_le_get_bonded_list(bt_bd_addr_t *list, uint32_t *count)
{
    int32_t i = 0;
    uint32_t buff_size = *count;
    bt_bd_addr_t *p = list;
    *count = 0;
    
    LOG_I(BT_DM, "%s:want_read_count is %d\r\n", __FUNCTION__, count);
    if (!bt_dm_le_initialized) {
        LOG_I(BT_DM, "Get bonded list fail, please init BT DM LE first!\r\n");
        return;
    }
    while ((i < BT_DEVICE_MANAGER_LE_BONDED_MAX) && (i < buff_size)) {
        if (0 != memcmp(&default_bt_addr, &(bonded_info[i].bt_addr), sizeof(default_bt_addr))) {
            memcpy(p, bonded_info[i].bt_addr.addr, sizeof(bt_bd_addr_t));
            //p->is_connected = false;
            //if (bt_device_manager_le_get_connection_by_address(p->bt_addr) != NULL) {
                //p->is_connected = true;
            //}
            (*count)++;
            p++;
        }
        i++;
    }
    
    LOG_I(BT_DM, "%s:success, real_read_count is %d\r\n", __FUNCTION__, count);
}

static void bt_device_manager_le_save_connection_params(void *buff)
{
    uint8_t i;
    LOG_I(BT_DM, "%s:enter\r\n", __FUNCTION__);

    bt_gap_le_connection_ind_t *conn_ind = (bt_gap_le_connection_ind_t *)buff;
    for (i =0; i< BT_DEVICE_MANAGER_LE_CONNECTION_MAX; i++) {
        if (0 == dm_info[i].connection_handle) {
            dm_info[i].connection_handle = conn_ind->connection_handle;
            dm_info[i].connection_params.conn_interval = conn_ind->conn_interval;
            dm_info[i].connection_params.slave_latency = conn_ind->conn_latency;            
            dm_info[i].connection_params.supervision_timeout = conn_ind->supervision_timeout;
            break;
        }
    }
    if (i == BT_DEVICE_MANAGER_LE_CONNECTION_MAX) {
        LOG_I(BT_DM, "Reach maximum connection, no empty buffer to save conn_info!\r\n");
    }
}

static void bt_device_manager_le_delete_connection_params(void *buff)
{
    uint8_t i;
    bt_hci_evt_disconnect_complete_t *disc_ind;
    LOG_I(BT_DM, "%s:enter\r\n", __FUNCTION__);

    disc_ind = (bt_hci_evt_disconnect_complete_t*) buff;
    for (i = 0; i< BT_DEVICE_MANAGER_LE_CONNECTION_MAX ; i++) {
        if (disc_ind->connection_handle == dm_info[i].connection_handle) {
            memset(&(dm_info[i]),0x00, sizeof(bt_device_manager_le_connection_struct_t));
            break;
        }
    }
    if (i == BT_DEVICE_MANAGER_LE_CONNECTION_MAX) {
        LOG_I(BT_DM, "Don't know connection info for deleting!\r\n");
    }
}

static void bt_device_manager_le_reset_connection_infos(void)
{
    uint8_t i;
    LOG_I(BT_DM, "%s: enter\r\n", __FUNCTION__);

    for (i = 0; i< BT_DEVICE_MANAGER_LE_CONNECTION_MAX ; i++) {   
        memset(&(dm_info[i]),0x00, sizeof(bt_device_manager_le_connection_struct_t));
    }
}

static void bt_device_manager_le_update_current_conn_interval(void *conn_params)
{
    uint8_t i;
    bt_gap_le_connection_update_ind_t *ind = (bt_gap_le_connection_update_ind_t *)conn_params;
    LOG_I(BT_DM, "%s: conn_handle(0x%04x)\r\n", __FUNCTION__, ind->conn_handle);

    for (i =0; i < BT_DEVICE_MANAGER_LE_CONNECTION_MAX; i++) {
        if (dm_info[i].connection_handle == ind->conn_handle) {
            dm_info[i].connection_params.conn_interval = ind->conn_interval;
            dm_info[i].connection_params.slave_latency = ind->conn_latency;            
            dm_info[i].connection_params.supervision_timeout = ind->supervision_timeout;
            break;
        }
    }
    if (i == BT_DEVICE_MANAGER_LE_CONNECTION_MAX) {
        LOG_I(BT_DM, "Reach maximum connection, update conn params fail!\r\n");
    }
}

bt_device_manager_le_connection_param_t *bt_device_manager_le_get_current_connection_param(bt_handle_t connection_handle)
{
    uint8_t i;
    LOG_I(BT_DM, "%s: conn_handle(0x%04x)\r\n", __FUNCTION__, connection_handle);

    if (!bt_dm_le_initialized) {
         LOG_I(BT_DM, "Get Conn interval fail, please init BT DM LE first! \r\n");
         return NULL;
    }
    for (i = 0; i< BT_DEVICE_MANAGER_LE_CONNECTION_MAX; i++) {
        if (connection_handle == dm_info[i].connection_handle) {
            return &(dm_info[i].connection_params);
        }
    }
    return NULL;
}

bt_bd_addr_ptr_t bt_device_manager_le_get_public_address(void)
{
    LOG_I(BT_DM, "%s: enter\r\n", __FUNCTION__);
    bt_device_manager_le_gen_public_address();
    return (bt_bd_addr_ptr_t)(&bt_device_manager_le_local_public_addr);
}


static void bt_device_manager_le_gen_public_address(void)
{
    int8_t i;
    uint32_t random_seed;
    uint32_t size = 12;
    uint8_t buffer[18] = {0};
    uint8_t tmp_buf[3] = {0};
    bt_bd_addr_t tempaddr = {0};
    nvdm_status_t status;
    hal_trng_status_t ret = HAL_TRNG_STATUS_OK;

    LOG_I(BT_DM, "%s: enter \r\n", __FUNCTION__);
    if (memcmp(bt_device_manager_le_local_public_addr, &tempaddr, sizeof(bt_bd_addr_t)) == 0) {
        LOG_I(BT_DM, "%s: empty public address!\r\n", __FUNCTION__);
        LOG_I(BT_DM, "[BT]Try to read public address from NVDM! \r\n");
        status = nvdm_read_data_item("BT", "address", buffer, &size);
        if (NVDM_STATUS_OK == status) {
            LOG_I(BT_DM, "[BT]Read from NVDM:%s \r\n", buffer);
            for (i = 0; i < 6; ++i) {
                tmp_buf[0] = buffer[2 * i];
                tmp_buf[1] = buffer[2 * i + 1];
                bt_device_manager_le_local_public_addr[i] = (uint8_t)strtol((const char*)tmp_buf, NULL, 16);
            }
            
            LOG_I(BT_DM, "[BT]Read address from NVDM [%02X:%02X:%02X:%02X:%02X:%02X]\r\n", 
                bt_device_manager_le_local_public_addr[0], 
                bt_device_manager_le_local_public_addr[1], 
                bt_device_manager_le_local_public_addr[2], 
                bt_device_manager_le_local_public_addr[3], 
                bt_device_manager_le_local_public_addr[4], 
                bt_device_manager_le_local_public_addr[5]);
            return;
        } else {
            LOG_I(BT_DM, "[BT]Failed to Read from NVDM, err is %d.\r\n", status);
            ret = hal_trng_init();
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(BT_DM, "[BT]generate_public_address--error 1!\r\n");
            }
            for (i = 0; i < 30; ++i) {
                ret = hal_trng_get_generated_random_number(&random_seed);
                if (HAL_TRNG_STATUS_OK != ret) {
                    LOG_I(BT_DM, "[BT]generate_public_address--error 2!\r\n");
                }
            }
            /* randomly generate address */
            ret = hal_trng_get_generated_random_number(&random_seed);
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(BT_DM, "[BT]generate_public_address--error 3!\r\n");
            }
            bt_device_manager_le_local_public_addr[0] = random_seed & 0xFF;
            bt_device_manager_le_local_public_addr[1] = (random_seed >> 8) & 0xFF;
            bt_device_manager_le_local_public_addr[2] = (random_seed >> 16) & 0xFF;
            bt_device_manager_le_local_public_addr[3] = (random_seed >> 24) & 0xFF;
            ret = hal_trng_get_generated_random_number(&random_seed);
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(BT_DM, "[BT]generate_public_address--error 3!\r\n");
            }
            bt_device_manager_le_local_public_addr[4] = random_seed & 0xFF;
            bt_device_manager_le_local_public_addr[5] = (random_seed >> 8) & 0xCF;
            hal_trng_deinit();
        }
    }

    /* save address to NVDM */
    for (i = 0; i < 6; ++i) {
        sprintf((char *)buffer + 2 * i, "%02X \r\n", bt_device_manager_le_local_public_addr[i]);
    }
    //LOG_I(BT_DM, "[BT]address to write:%s len:%d\r\n", buffer, strlen(buffer));
    status = nvdm_write_data_item("BT", "address", NVDM_DATA_ITEM_TYPE_STRING, buffer, strlen((const char*)buffer));
    if (NVDM_STATUS_OK != status) {
        LOG_I(BT_DM, "[BT]Failed to store address!\r\n");
    } else {
        LOG_I(BT_DM, "[BT]Successfully store address to NVDM [%02X:%02X:%02X:%02X:%02X:%02X]\r\n", 
            bt_device_manager_le_local_public_addr[0], 
            bt_device_manager_le_local_public_addr[1], 
            bt_device_manager_le_local_public_addr[2], 
            bt_device_manager_le_local_public_addr[3], 
            bt_device_manager_le_local_public_addr[4], 
            bt_device_manager_le_local_public_addr[5]);
    }
}



