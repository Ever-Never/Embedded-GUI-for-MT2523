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

#include "ble_gatt_service.h"
#include "ble_gatt_common.h"

static uint32_t bt_if_gap_dev_name_callback (const uint8_t rw, uint16_t handle,
    void *data, uint16_t size, uint16_t offset);

static uint32_t bt_if_gap_appearance_callback (const uint8_t rw, uint16_t handle,
    void *data, uint16_t size, uint16_t offset);

const bt_uuid_t BT_SIG_UUID_SERVICE_CHANGED =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_SERVICE_CHANGED);
static const bt_uuid_t BT_SIG_UUID_DEVICE_NAME =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_DEVICE_NAME);
static const bt_uuid_t BT_SIG_UUID_APPEARANCE =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_APPEARANCE);
const bt_uuid_t BT_SIG_UUID_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS);

BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_gap_primary_service, BT_GATT_UUID16_GAP_SERVICE);

//BT_GATTS_NEW_INCLUDED_SERVICE_128(bt_if_gap_included_manufacturer_service, 0x0500, 0x0522);

BT_GATTS_NEW_CHARC_16(bt_if_gap_char4_dev_name, BT_GATT_CHARC_PROP_READ,
    BLE_GAP_VAL_HD_DEV_NAME, BT_SIG_UUID16_DEVICE_NAME);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_gap_dev_name, BT_SIG_UUID_DEVICE_NAME,
    BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE, bt_if_gap_dev_name_callback);

BT_GATTS_NEW_CHARC_16_WRITABLE(bt_if_gap_char4_appearance, BT_GATT_CHARC_PROP_READ,
    BLE_GAP_VAL_HD_APP, BT_SIG_UUID16_APPEARANCE);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_gap_appearance, BT_SIG_UUID_APPEARANCE,
    BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE, bt_if_gap_appearance_callback);

BT_GATTS_NEW_CHARC_16(bt_if_gap_char4_ppcp, BT_GATT_CHARC_PROP_READ,
    BLE_GAP_VAL_HD_PPCP, BT_SIG_UUID16_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS);

BT_GATTS_NEW_CHARC_VALUE_HALFW8_WRITABLE(bt_if_gap_ppcp, BT_SIG_UUID_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS,
    BT_GATTS_REC_PERM_READABLE, 8, 0x0580, 0x0c80, 0x0010, 0x0333);

static const bt_gatts_service_rec_t *bt_if_gap_service_rec[] = {
    (const bt_gatts_service_rec_t *) &bt_if_gap_primary_service,
    //(const bt_gatts_service_rec_t *) &bt_if_gap_included_manufacturer_service,
    (const bt_gatts_service_rec_t *) &bt_if_gap_char4_dev_name,
    (const bt_gatts_service_rec_t *) &bt_if_gap_dev_name,
    (const bt_gatts_service_rec_t *) &bt_if_gap_char4_appearance,
    (const bt_gatts_service_rec_t *) &bt_if_gap_appearance,
    (const bt_gatts_service_rec_t *) &bt_if_gap_char4_ppcp,
    (const bt_gatts_service_rec_t *) &bt_if_gap_ppcp
};

const bt_gatts_service_t bt_if_gap_service = {
    .starting_handle = BLE_GAP_START_HANDLE,
    .ending_handle = BLE_GAP_END_HANDLE,
    .required_encryption_key_size = BLE_GAP_REKS,
    .records = bt_if_gap_service_rec
};

BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_gatt_primary_service, BT_GATT_UUID16_GATT_SERVICE);

BT_GATTS_NEW_CHARC_16(bt_if_gatt_char4_service_changed, BT_GATT_CHARC_PROP_READ | BT_GATT_CHARC_PROP_NOTIFY | BT_GATT_CHARC_PROP_INDICATE,
                      BLE_GAP_VAL_HD_SC, BT_SIG_UUID16_SERVICE_CHANGED);

BT_GATTS_NEW_CHARC_VALUE_UINT32_WRITABLE(bt_if_gatt_service_changed,
    BT_SIG_UUID_SERVICE_CHANGED, 0x2, 0x0001050F);

static const bt_gatts_service_rec_t *bt_if_gatt_service_rec[] = {
    (const bt_gatts_service_rec_t *) &bt_if_gatt_primary_service,
    (const bt_gatts_service_rec_t *) &bt_if_gatt_char4_service_changed,
    (const bt_gatts_service_rec_t *) &bt_if_gatt_service_changed
};

const bt_gatts_service_t bt_if_gatt_service = {
    .starting_handle = BLE_GATT_START_HANDLE,
    .ending_handle = BLE_GATT_END_HANDLE,
    .required_encryption_key_size = BLE_GATT_REKS,
    .records = bt_if_gatt_service_rec
};


char gatts_device_name[256] = {"MediaTek_PXP"};
static uint32_t bt_if_gap_dev_name_callback (const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    uint32_t str_size = strlen(gatts_device_name);
    uint32_t buf_size = sizeof(gatts_device_name);
    uint32_t copy_size;

    switch (rw) {
        case BT_GATTS_CALLBACK_READ:
            copy_size = (str_size > offset) ? (str_size - offset) : 0;
            if (size == 0) {
                return str_size;
            }
            copy_size = (size > copy_size) ? copy_size : size;
            memcpy(data, gatts_device_name + offset, copy_size);
            return copy_size;
        case BT_GATTS_CALLBACK_WRITE:
            copy_size = (size > buf_size) ? buf_size : size;
            memcpy(gatts_device_name, data, copy_size);
            return copy_size;
        default:
            return BT_STATUS_SUCCESS;
    }
}


uint16_t gap_appearance = 0xbabe; //GAP appearance
static uint32_t bt_if_gap_appearance_callback (const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    if (rw == BT_GATTS_CALLBACK_WRITE) {
        if (size != sizeof(gap_appearance)) { //Size check
            return 0;
        }
        gap_appearance = *(uint16_t *)data;
    } else {
        if (size != 0) {
            uint16_t *buf = (uint16_t *) data;
            *buf = gap_appearance;
        }
    }
    return sizeof(gap_appearance);
}

