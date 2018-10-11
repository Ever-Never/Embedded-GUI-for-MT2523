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
#include "bt_debug.h"
#include "bt_uuid.h"

#include "bt_gap_le.h"
#include "bt_gattc.h"
#include "bt_gatts.h"
#include "bt_spp.h"
#include "bt_app_common.h"
#include "bt_callback_manager.h"
#include "bt_gattc_discovery.h"
#include "bt_gatts_service.h"
#include "bt_device_manager_le.h"

#include "ble_ias.h"
#include "ble_bas.h"
#include "ble_dogp_adp_service.h"
#include "ble_find_me_server.h"
#include "ble_bas_app.h"
#include "syslog.h"
#include "hal_trng.h"
#include "nvdm.h"

log_create_module(BT_APP, PRINT_LEVEL_INFO);

static char* local_device_name = "BT_HEADSET_DEMO_LE";
//static uint16_t g_conn_interval;
static bt_gap_le_smp_pairing_config_t pairing_config = {//MITM, Bond, OOB
    .maximum_encryption_key_size = 16,
    .io_capability = BT_GAP_LE_SMP_NO_INPUT_NO_OUTPUT,
    .auth_req = BT_GAP_LE_SMP_AUTH_REQ_BONDING,
    .oob_data_flag = BT_GAP_LE_SMP_OOB_DATA_NOT_PRESENTED,
    .initiator_key_distribution = BT_GAP_LE_SMP_KEY_DISTRIBUTE_ENCKEY | BT_GAP_LE_SMP_KEY_DISTRIBUTE_IDKEY | BT_GAP_LE_SMP_KEY_DISTRIBUTE_SIGN,
    .responder_key_distribution = BT_GAP_LE_SMP_KEY_DISTRIBUTE_ENCKEY | BT_GAP_LE_SMP_KEY_DISTRIBUTE_IDKEY | BT_GAP_LE_SMP_KEY_DISTRIBUTE_SIGN,
};

static bt_gap_le_local_key_t local_key = {
    .encryption_info.ltk = { 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc8, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf },
    .master_id.ediv = 0x1005,
    .master_id.rand = { 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7 },
    .identity_info.irk = { 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf },
    .signing_info.csrk = { 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf }
};


//bt_bd_addr_t bt_local_random_addr = {0xC5, 0x01, 0x02, 0x03, 0x04, 0xC5};
static bt_bd_addr_t bt_local_random_addr;
//SDP part.
#define BT_SPP_STANDARD_UUID    0x00,0x00,0xFF,0x01,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFF
 
 static const uint8_t bt_spp_app_service_class_id[] =
 {
     BT_SPP_SDP_ATTRIBUTE_UUID_LENGTH,
     BT_SPP_SDP_ATTRIBUTE_UUID(BT_SPP_STANDARD_UUID)
 };
 
 static const uint8_t bt_spp_app_protocol_descriptor_list[] =
 {
     BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(BT_SPP_SERVER_ID_START + 2)
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

static void bt_app_common_generate_local_address(bt_bd_addr_t addr)
{
    nvdm_status_t status;
    int8_t i;
    uint32_t random_seed;
    uint32_t size = 12;
    uint8_t buffer[18] = {0};
    uint8_t tmp_buf[3] = {0};
    bt_bd_addr_t tempaddr = {0};
    hal_trng_status_t ret = HAL_TRNG_STATUS_OK;
    
    if (memcmp(addr, tempaddr, sizeof(bt_bd_addr_t)) == 0) {
        LOG_I(BT_APP, "[BT]Try to read from NVDM! \r\n");
        status = nvdm_read_data_item("BT_APP", "address", buffer, &size);
        if (NVDM_STATUS_OK == status) {
            for (i = 0; i < 6; ++i) {
                tmp_buf[0] = buffer[2 * i];
                tmp_buf[1] = buffer[2 * i + 1];
                addr[i] = (uint8_t)strtoul((char *)tmp_buf, NULL, 16);
            }

            LOG_I(BT_APP, "[BT]Read address from NVDM [%02X:%02X:%02X:%02X:%02X:%02X]\r\n", addr[0],
                  addr[1], addr[2], addr[3], addr[4], addr[5]);
            return;
        } else {
            LOG_I(BT_APP, "[BT]Failed to Read from NVDM:%d !\r\n", status);
            ret = hal_trng_init();
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(BT_APP, "[BT]generate_random_address--error 1\r\n");
            }
            for (i = 0; i < 30; ++i) {
                ret = hal_trng_get_generated_random_number(&random_seed);
                if (HAL_TRNG_STATUS_OK != ret) {
                    LOG_I(BT_APP, "[BT]generate_random_address--error 2\r\n");
                }
            }
            /* randomly generate address */
            ret = hal_trng_get_generated_random_number(&random_seed);
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(BT_APP, "[BT]generate_random_address--error 3\r\n");
            }
            addr[0] = random_seed & 0xFF;
            addr[1] = (random_seed >> 8) & 0xFF;
            addr[2] = (random_seed >> 16) & 0xFF;
            addr[3] = (random_seed >> 24) & 0xFF;
            ret = hal_trng_get_generated_random_number(&random_seed);
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(BT_APP, "[BT]generate_random_address--error 3\r\n");
            }
            addr[4] = random_seed & 0xFF;
            addr[5] = (random_seed >> 8) & 0xCF;
            addr[5] = addr[5] | 0xC0;
            hal_trng_deinit();    
        }
    }
    /* save address to NVDM */
    for (i = 0; i < 6; ++i) {
        sprintf((char *)buffer + 2 * i, "%02X", addr[i]);
    }
    LOG_I(BT_APP, "[BT]address to write:%s len:%d \r\n", buffer, strlen((char *)buffer));
    status = nvdm_write_data_item("BT_APP", "address", NVDM_DATA_ITEM_TYPE_STRING, buffer, strlen((char *)buffer));
    if (NVDM_STATUS_OK != status) {
        LOG_I(BT_APP, "[BT]Failed to store address!\r\n");
    } else {
        LOG_I(BT_APP, "[BT]Successfully store address to NVDM [%02X:%02X:%02X:%02X:%02X:%02X]\r\n", addr[0],
              addr[1], addr[2], addr[3], addr[4], addr[5]);
    }
}


static void bt_app_common_advtising_init(void)
{
    bt_hci_cmd_le_set_advertising_parameters_t adv_param = {
        .advertising_interval_min = 0x0800,
        .advertising_interval_max = 0x0800,
        .advertising_type = BT_HCI_ADV_TYPE_CONNECTABLE_UNDIRECTED,
        .own_address_type = BT_ADDR_RANDOM,
        .advertising_channel_map = 7,
        .advertising_filter_policy = 0
    };

    bt_hci_cmd_le_set_advertising_enable_t enable;
    bt_hci_cmd_le_set_advertising_data_t adv_data = {
        .advertising_data_length = 31,
    };
    bt_hci_cmd_le_set_scan_response_data_t scan_data = {
        .scan_response_data_length = 31,
        .scan_response_data = "DDBT_HEADSET_DEMO_LE",
    };

    scan_data.scan_response_data[0] = strlen(local_device_name) + 1; /* ADV length. */
    scan_data.scan_response_data[1] = 0x08;

    adv_data.advertising_data[0] = 2; 
    adv_data.advertising_data[1] = BT_GAP_LE_AD_TYPE_FLAG;
    adv_data.advertising_data[2] = BT_GAP_LE_AD_FLAG_BR_EDR_NOT_SUPPORTED | BT_GAP_LE_AD_FLAG_GENERAL_DISCOVERABLE;
    adv_data.advertising_data[3] = strlen(local_device_name) + 1;
    adv_data.advertising_data[4] = BT_GAP_LE_AD_TYPE_NAME_COMPLETE;
    memcpy(&adv_data.advertising_data[5], local_device_name, strlen(local_device_name));
    adv_data.advertising_data[5 + strlen(local_device_name)] = 0x0;

    enable.advertising_enable = BT_HCI_ENABLE;
    bt_gap_le_set_advertising(&enable, &adv_param, &adv_data, &scan_data);
    bt_gatts_set_max_mtu(512);
}

#if 0
static bt_status_t ble_app_common_conn_interval_update(bt_handle_t connection_handle)
{
    bt_hci_cmd_le_connection_update_t conn_params;

    conn_params.supervision_timeout = 0x0258;            /** TBC: 6000ms : 600 * 10 ms. */
    conn_params.connection_handle = connection_handle;
    
    conn_params.conn_interval_min = 0x0120;/** TBC: 380ms : 288 * 1.25 ms. */
    conn_params.conn_interval_max = 0x0130;/** TBC: 400ms : 320 * 1.25 ms. */
    conn_params.slave_latency = 0;

    return bt_gap_le_update_connection_parameter(&conn_params);
}
#endif
static bt_status_t bt_app_common_evnet_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    LOG_I(BT_APP, "%s: status(0x%04x), msg(0x%04x)", __FUNCTION__, status, msg);

    if(status != BT_STATUS_SUCCESS) {
       return BT_STATUS_SUCCESS;
    }

     switch (msg)
     {
         case BT_POWER_ON_CNF:
             bt_app_common_generate_local_address(bt_local_random_addr);
             bt_gap_le_set_random_address((bt_bd_addr_ptr_t)bt_local_random_addr);  
             break;

         case BT_GAP_LE_SET_RANDOM_ADDRESS_CNF:
             bt_app_common_advtising_init();
             break;

         case BT_GAP_LE_CONNECT_IND: {
             bt_gap_le_connection_ind_t *connection_ind = (bt_gap_le_connection_ind_t *)buff;
             //g_conn_interval = connection_ind->conn_interval;
             LOG_I(BT_APP, "Connect complete, conn_handle(0x%04x).\r\n", connection_ind->connection_handle);
             bt_gap_le_bond(connection_ind->connection_handle, &pairing_config);
         }
             break;  
             
         case BT_GAP_LE_BONDING_REPLY_REQ_IND:
             break;

         case BT_GAP_LE_BONDING_COMPLETE_IND: {
            bt_gattc_discovery_status_t status;
            bt_gap_le_bonding_complete_ind_t *ind = (bt_gap_le_bonding_complete_ind_t*)buff;
            LOG_I(BT_APP, "Bond complete, conn_handle(0x%04x).\r\n", ind->handle);
            status = bt_gattc_discovery_start(ind->handle);
            LOG_I(BT_APP, "start discovery, status is %d\r\n", status);
            break;
        }  
         case BT_GAP_LE_CONNECTION_UPDATE_CNF:
         case BT_GAP_LE_CONNECTION_UPDATE_IND:
         {
             bt_gap_le_connection_update_ind_t *ind = (bt_gap_le_connection_update_ind_t *)buff; 
             LOG_I(BT_APP, "CONNECTION UPDATE: event_id = %x, interval = %d\n", msg, ind->conn_interval);
             //g_conn_interval = ind->conn_interval;
         }
            break;
            
         case BT_GAP_LE_DISCONNECT_IND: {
            bt_app_common_advtising_init();
        }
            break;
            
         default:
             break;
     }
     return BT_STATUS_SUCCESS;
}

static bt_status_t bt_app_common_event_callback_register(void)
{
    LOG_I(BT_APP, "%s: enter\r\n", __FUNCTION__);
    bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_GAP | MODULE_MASK_SYSTEM, (void *)bt_app_common_evnet_callback);    
    bt_callback_manager_add_sdp_customized_record(&bt_spp_app_sdp_record);
    return BT_STATUS_SUCCESS;
}

static bt_status_t bt_app_common_gap_param_init(void)
{
    uint16_t appearance = 0x1234;
#if 0
    bt_gatts_le_gap_ppcp_t   gap_conn_params;

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = 0x0120;
    gap_conn_params.max_conn_interval = 0x0130;
    gap_conn_params.slave_latency     = 0x00;
    gap_conn_params.supervision_timeout  = 0x0258;
#endif
    if (0 != (memcmp(local_device_name, bt_gatts_service_get_gap_device_name(),sizeof(local_device_name)))) {
        bt_gatts_service_set_gap_device_name((const uint8_t *)local_device_name, strlen(local_device_name));
    }
    bt_gatts_service_set_gap_appearance(appearance);
    //bt_gatts_service_set_gap_ppcp(&gap_conn_params);
    bt_gatts_service_set_gap_central_address_resolution(0);

    return BT_STATUS_SUCCESS;
}

static bt_status_t bt_app_common_security_param_init(void)
{
    bool sc_only = false;

    bt_device_manager_le_gap_set_pairing_configuration(&pairing_config); 
    bt_device_manager_le_gap_set_local_configuration(&local_key, sc_only);
    return BT_STATUS_SUCCESS;
}

void bt_app_common_init(void)
{
    bt_device_manager_le_init();
    //bt_gatts_service_init();
    bt_gattc_discovery_init();
    ble_fmp_server_init();
    ble_bas_app_init();

    bt_app_common_gap_param_init();
    bt_app_common_security_param_init();
    bt_app_common_event_callback_register();
}






