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
 *
 * Description:
 * ------------
 * This file implements DOGP service service main function
 *
 ****************************************************************************/
//#ifdef MTK_BLE_DOGP_ENABLE


#include <stdint.h>
#include "bt_gatts.h"
#include "bt_gattc.h"
#include "ble_dogp_service.h"

//#include "dispatcher.h"
#include "bt_uuid.h"
#include "bt_type.h"

#ifndef WIN32
#include "FreeRTOS.h"
//#include "portable.h"
#endif
#include "syslog.h"


log_create_module(DOGP, PRINT_LEVEL_INFO);

/************************************************
*   Macro
*************************************************/
#define DOGP_SERVICE_UUID             (0x18A0)          /* Data Transfer Over Gatt Service UUID. */
#define DOGP_READ_CHAR_UUID           (0x2AA0)          /* Read Characteristic UUID. */
#define DOGP_WRITE_CHAR_UUID          (0x2AA1)          /* Write Characteristic UUID. */
#define DOGP_READ_CHAR_VALUE_HANDLE   (0x0022)          /* Read Characteristic value's handle. */
#define DOGP_WRITE_CHAR_VALUE_HANDLE  (0x0025)          /* Read Characteristic value's handle. */

/************************************************
*   Global
*************************************************/ 
static ble_dogp_service_t p_dogps_t;
const bt_uuid_t DOGP_READ_CHAR_UUID128 = BT_UUID_INIT_WITH_UUID16(DOGP_READ_CHAR_UUID);
const bt_uuid_t DOGP_WRITE_CHAR_UUID128 = BT_UUID_INIT_WITH_UUID16(DOGP_WRITE_CHAR_UUID);

/************************************************
*   Utilities
*************************************************/
static uint32_t ble_dogp_read_char_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
static uint32_t ble_dogp_read_char_cccd_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
static uint32_t ble_dogp_write_char_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);



BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_dogp_primary_service, DOGP_SERVICE_UUID);

BT_GATTS_NEW_CHARC_16(bt_if_dogp_read_char,
                      BT_GATT_CHARC_PROP_READ | BT_GATT_CHARC_PROP_NOTIFY, DOGP_READ_CHAR_VALUE_HANDLE, DOGP_READ_CHAR_UUID);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_dogp_read_char_value, DOGP_READ_CHAR_UUID128,
                    BT_GATTS_REC_PERM_READABLE, ble_dogp_read_char_callback);

BT_GATTS_NEW_CLIENT_CHARC_CONFIG(bt_if_dogp_read_char_cccd,
                                 BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE,
                                 ble_dogp_read_char_cccd_callback);

BT_GATTS_NEW_CHARC_16(bt_if_dogp_write_char,
                      BT_GATT_CHARC_PROP_WRITE, DOGP_WRITE_CHAR_VALUE_HANDLE, DOGP_WRITE_CHAR_UUID);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_dogp_write_char_value, DOGP_WRITE_CHAR_UUID128,
                    BT_GATTS_REC_PERM_WRITABLE, ble_dogp_write_char_callback);


static const bt_gatts_service_rec_t *bt_if_dogp_service_rec[] = {
    (const bt_gatts_service_rec_t *) &bt_if_dogp_primary_service,
    (const bt_gatts_service_rec_t *) &bt_if_dogp_read_char,
    (const bt_gatts_service_rec_t *) &bt_if_dogp_read_char_value,
    (const bt_gatts_service_rec_t *) &bt_if_dogp_read_char_cccd,
    (const bt_gatts_service_rec_t *) &bt_if_dogp_write_char,
    (const bt_gatts_service_rec_t *) &bt_if_dogp_write_char_value,
    
};

const bt_gatts_service_t bt_if_dogp_service = {
    .starting_handle = 0x0020,
    .ending_handle = 0x0025,
    .required_encryption_key_size = 0,
    .records = bt_if_dogp_service_rec
};

/**
 * @brief Function for handling Client Configuration Characteristisc Descriptor's read and write event.
 *
 * @param[in]   rw                    Flag of Read or Write event.
 * @param[in]   handle                Connection handle.
 * @param[in]   size                  Length of the data.
 * @param[in]   *data                 Data buffer.
 * @param[in]   *data                 Data buffer.
 * @param[in]  offset                 Write or Read offset.
 *
 * @return      Real wrote or read length of the data.
 */
static uint32_t ble_dogp_read_char_cccd_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_I(DOGP, "ble_dogp_read_char_cccd_callback, rw is %d, size is %d, offset is %d \r\n",rw, size, offset);

    //TODO:Should record for each connection handle.
    if (rw == BT_GATTS_CALLBACK_WRITE){
        ble_dogp_service_evt_t evt;
        memset(&evt, 0, sizeof(ble_dogp_service_evt_t));
        evt.evt_type = BLE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_CCCD_WRITE;
        evt.params.dogp_service_read_characteristic_cccd_value = *(uint16_t*)data;
        if (p_dogps_t.w_evt_handler) {
            return p_dogps_t.w_evt_handler(handle, &evt);
        }
    } else if (rw == BT_GATTS_CALLBACK_READ) {
            ble_dogp_service_evt_t evt;
            memset(&evt, 0, sizeof(ble_dogp_service_evt_t));
            evt.evt_type = BLE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_CCCD_READ;
            if (p_dogps_t.r_evt_handler) {
                return p_dogps_t.r_evt_handler(handle, data, size, &evt);
            }
    } 
    return 0;
}

/**
 * @brief Function for handling DOGP Read Characteristisc's read and write event.
 *
 * @param[in]   rw                    Flag of Read or Write event.
 * @param[in]   handle                Connection handle.
 * @param[in]   size                  Length of the data.
 * @param[in]   *data                 Data buffer.
 * @param[in]   offset                Write or Read offset.
 *
 * @return      Real wrote or read length of the data.
 */
static uint32_t ble_dogp_read_char_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_I(DOGP, "ble_dogp_read_char_callback:rw is %d,size is %d, offset is %d\r\n", rw, size, offset);

    //TODO:Should record for each connection handle.
    if (rw == BT_GATTS_CALLBACK_READ) {
        ble_dogp_service_evt_t evt;
        memset(&evt, 0, sizeof(ble_dogp_service_evt_t));
        evt.evt_type = BLE_DOGP_SERVICE_READ_CHARACTERISTIC_EVT_READ;
        evt.params.dogp_service_read_characteristic_read_offset = offset; /**default is 0*/
        if (p_dogps_t.r_evt_handler) {
            return p_dogps_t.r_evt_handler(handle, data, size, &evt);
        }
    }
    return 0;
}

/**
 * @brief Function for handling DOGP Write Characteristisc's read and write event.
 *
 * @param[in]   rw                    Flag of Read or Write event.
 * @param[in]   handle                Connection handle.
 * @param[in]   size                  Length of the data.
 * @param[in]   *data                 Data buffer.
 * @param[in]   offset                Write or Read offset.
 * 
 * @return      Real wrote or read length of the data.
 */
static uint32_t ble_dogp_write_char_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_I(DOGP, "ble_dogp_write_char_callback:rw is %d, size is %d, offset is %d \r\n",rw, size, offset);

    //TODO:Should record for each connection handle.
    if (rw == BT_GATTS_CALLBACK_WRITE){
        ble_dogp_service_evt_t evt;
        memset(&evt, 0, sizeof(ble_dogp_service_evt_t));
        evt.evt_type = BLE_DOGP_SERVICE_WRITE_CHARACTERISTIC_EVT_WRITE;
        evt.params.dogp_service_write_characteristic_write_offset = offset;
        evt.params.dogp_service_write_characteristic.write_field.p_str = (uint8_t *)data;
        evt.params.dogp_service_write_characteristic.write_field.length = size;
        LOG_I(DOGP, "write_field.length:%d \r\n", evt.params.dogp_service_write_characteristic.write_field.length);
        //ble_dogp_service_write_characteristic_decode(&evt.params.dogp_service_write_characteristic, (uint8_t *)data, size);
        if (p_dogps_t.w_evt_handler) {
            return p_dogps_t.w_evt_handler(handle, &evt);
        }
    } 
    return 0;
}

/**
 * @brief Function for sending the DOGP service read characteristic value.
 *
 * @param[in]   conn_handle                           connection handle.
 * @param[in]   p_dogp_service_read_characteristic    DOGP service Characteristic structure.
 *
 * @return      ble_status_t                          0 means success.
 */
bt_status_t ble_dogp_service_read_characteristic_send(uint16_t conn_handle, uint8_t *buff)
{
    bt_status_t status = BT_STATUS_FAIL;
    uint8_t buf[32]={0};
    
    bt_gattc_charc_value_notification_indication_t *notify = (bt_gattc_charc_value_notification_indication_t*) buf;
    
    if (NULL != notify) {
        notify->att_req.opcode = BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION;
        notify->att_req.handle = DOGP_READ_CHAR_VALUE_HANDLE;
        memcpy((void *)(notify->att_req.attribute_value), buff, 14); 
        notify->attribute_value_length = 3 + 14;
        LOG_I(DOGP, "ble_dogp_service_read_characteristic_send! \r\n");

        status = bt_gatts_send_charc_value_notification_indication(conn_handle, notify);
    }
    return status;
}


bt_status_t ble_dogp_service_init(const ble_dogp_service_init_t *p_dogp_service_init)
{
    LOG_I(DOGP, "ble_dogp_service_init \r\n");

    /**Initialize service structure*/
    p_dogps_t.r_evt_handler = p_dogp_service_init->r_evt_handler;    
    p_dogps_t.w_evt_handler = p_dogp_service_init->w_evt_handler;

    return BT_STATUS_SUCCESS;
}

bt_status_t ble_dogp_service_deinit(void)
{
    LOG_I(DOGP, "ble_dogp_service_deinit \r\n");

    /**Initialize service structure*/
    p_dogps_t.r_evt_handler = NULL;
    p_dogps_t.w_evt_handler = NULL;

    return BT_STATUS_SUCCESS;
}


//#endif /*MTK_BLE_DOGP_ENABLE*/
