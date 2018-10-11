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
#include "bt_gap.h"
#include "bt_gap_le.h"
#include "bt_system.h"
#include "bt_debug.h"
#include "syslog.h"
#include "nvdm.h"
#include "bt_init.h"

#include "bt_spp.h"

LOG_CONTROL_BLOCK_DECLARE(fota_dl_m);

extern bt_bd_addr_t local_ble_random_addr;

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
 

 bt_sdps_record_t* fota_get_spp_sdp_record()
 {
     return (bt_sdps_record_t* )&bt_spp_app_sdp_record;
 }


#define LINK_KEY_NUM 5
#define FOTA_LINK_KEY_NVDM_GROUP     "fota_link_key"
#define FOTA_LINK_KEY_NVDM_NAME      "edrs"

static bt_gap_link_key_notification_ind_t edr_keys[LINK_KEY_NUM];

static void _fota_save_link_key(void)
{
    nvdm_status_t ret = nvdm_write_data_item(
                            FOTA_LINK_KEY_NVDM_GROUP,
                            FOTA_LINK_KEY_NVDM_NAME, 
                            NVDM_DATA_ITEM_TYPE_RAW_DATA,
                            (uint8_t*)&edr_keys[0],
                            sizeof(edr_keys));

    if (NVDM_STATUS_OK != ret) {
        LOG_E(fota_dl_m, "[FOTA] failed to save link key to nvdm, err: %d\r\n", ret);
    } else {
        LOG_I(fota_dl_m, "[FOTA] succeed to save link key to nvdm.\r\n");
    }
}

void fota_load_link_key(void)
{
    uint32_t len = sizeof(edr_keys);
    LOG_I(fota_dl_m, "[FOTA] nvdm data len: %u\r\n", len);
    nvdm_status_t ret = nvdm_read_data_item (
                            FOTA_LINK_KEY_NVDM_GROUP,
                            FOTA_LINK_KEY_NVDM_NAME, 
                            (uint8_t*)edr_keys,
                            &len);

    if (NVDM_STATUS_OK != ret) {
        LOG_E(fota_dl_m, "[FOTA] failed to load link key, err: %d\r\n", ret);
    } else if (len != sizeof(edr_keys)) {
        LOG_E(fota_dl_m, "[FOTA] link key loading is not complete.\r\n");
    } else {
        LOG_I(fota_dl_m, "[FOTA] succeed to load link key.\r\n");
    }
}

    
static bt_gap_link_key_notification_ind_t* _bt_find_link_key_edr(bt_bd_addr_t *address)
{
    uint32_t i;
    for (i = 0; i < LINK_KEY_NUM; i++) {
        if (edr_keys[i].key_type == BT_GAP_LINK_KEY_TYPE_INVAILIDE) {
            memset(&(edr_keys[i].address), 0, sizeof(bt_bd_addr_t));
        }
        if (memcmp(address, &(edr_keys[i].address), sizeof(bt_bd_addr_t)) == 0) {
            return edr_keys + i;
        }
    }
    return NULL;
}

static bt_gap_link_key_notification_ind_t* _bt_alloc_link_key_edr(bt_bd_addr_t *address)
{
    uint32_t i;
    bt_bd_addr_t empty = {0};
    bt_gap_link_key_notification_ind_t* key;
    key = _bt_find_link_key_edr(address);
    if (key == NULL) {
        key = _bt_find_link_key_edr(&empty);        
    }
    if (key == NULL) {
        for (i = 0; i < LINK_KEY_NUM - 1; i++) {
            memcpy(&(edr_keys[i]), &(edr_keys[i + 1]), sizeof(bt_gap_link_key_notification_ind_t));
        }
        key = edr_keys + (LINK_KEY_NUM - 1);
        key->key_type = BT_GAP_LINK_KEY_TYPE_INVAILIDE;
    }
    return key;
}

void fota_bt_gap_get_link_key(bt_gap_link_key_notification_ind_t* key_information)
{
    bt_gap_link_key_notification_ind_t* key;
    LOG_I(fota_dl_m, "[FOTA] fota_bt_gap_get_link_key enter.\r\n");
    key = _bt_find_link_key_edr((bt_bd_addr_t*)&(key_information->address));
    if (key) {
        memcpy(key_information, key, sizeof(bt_gap_link_key_notification_ind_t));
    }
    else 
    {
        key_information->key_type = BT_GAP_LINK_KEY_TYPE_INVAILIDE;
    }
    return;
}


/**
 * @brief   This function is a static callback for the application to listen to the event. Provide a user-defined callback.
 * @param[in] msg     is the callback message type.
 * @param[in] status  is the status of the callback message.
 * @param[in] buf     is the payload of the callback message.
 * @return            The status of this operation returned from the callback.
 */
bt_status_t fota_bt_app_event_callback(
    bt_msg_type_t msg, 
    bt_status_t status, 
    void *buff)
{
    bt_gap_link_key_notification_ind_t * key_info = (bt_gap_link_key_notification_ind_t *) buff;
    bt_gap_link_key_notification_ind_t * key;
    LOG_I(fota_dl_m, "[FOTA] event:0x%x, %x\n", msg, status);
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
        
        case BT_GAP_LINK_KEY_NOTIFICATION_IND:
            LOG_I(fota_dl_m, "[FOTA] link key notification.\r\n");
            if (key_info->key_type == BT_GAP_LINK_KEY_TYPE_INVAILIDE) {
                key = _bt_find_link_key_edr((bt_bd_addr_t*)&(key_info->address));
                if(key != NULL) {
                    memset(key, 0, sizeof(bt_gap_link_key_notification_ind_t));
                    key->key_type = BT_GAP_LINK_KEY_TYPE_INVAILIDE;
                    }
            }
            else {
                  key = _bt_alloc_link_key_edr((bt_bd_addr_t*)&(key_info->address));
                  memcpy(key, key_info, sizeof(bt_gap_link_key_notification_ind_t));
            }
            _fota_save_link_key();
            break;
        case BT_POWER_ON_CNF:  /*24000001*/
            LOG_I(fota_dl_m, "[FOTA] bt power on confirm.\n");
            bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE);
            bt_gap_le_set_random_address(local_ble_random_addr);
            break;      

        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}


#define DEVICE_NAME "fota_download"
bt_gap_config_t bt_custom_config;

const bt_gap_config_t* fota_bt_gap_get_local_configuration(void)
{
    

    LOG_I(fota_dl_m, "bt_get_local_configuration1\n");
    bt_custom_config.inquiry_mode = 3;
    bt_custom_config.io_capability = BT_GAP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT;
    bt_custom_config.cod = 0x240404;
    memcpy(&bt_custom_config.device_name, DEVICE_NAME, sizeof(DEVICE_NAME));

    
    LOG_I(fota_dl_m,"1:%08x\r\n", (unsigned int)DEVICE_NAME);
    return  &bt_custom_config;
}

