/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek Inc. and/or its licensors,
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

#include "gatt_service.h"
const bt_uuid_t BT_SIG_UUID_SERVICE_CHANGED =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_SERVICE_CHANGED);
const bt_uuid_t BT_SIG_UUID_DEVICE_NAME =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_DEVICE_NAME);
const bt_uuid_t BT_SIG_UUID_APPEARANCE =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_APPEARANCE);
const bt_uuid_t BT_SIG_UUID_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS);
const bt_uuid_t BT_SIG_UUID_BATTERY_LEVEL =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_BATTERY_LEVEL);
const bt_uuid_t BT_SIG_UUID_SERIAL_NUMBER =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_SERIAL_NUMBER);
const bt_uuid_t BT_SIG_UUID_MANUFACTURER_NAME =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_MANUFACTURER_NAME);

//Declare every record here
//service collects all bt_gatts_service_rec_t
//IMPORTAMT: handle:0x0000 is reserved, please start your handle from 0x0001
//GAP 0x0001
char gatts_device_name[256] = {"MTKHB"};
static uint32_t bt_if_gap_dev_name_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
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
uint16_t gap_appearance = 0x1234; //GAP appearance
static uint32_t bt_if_gap_appearance_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
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
BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_gap_primary_service, BT_GATT_UUID16_GAP_SERVICE);
BT_GATTS_NEW_INCLUDED_SERVICE_128(bt_if_gap_include_manufacturer_service, 0x0500, 0x0522);
BT_GATTS_NEW_CHARC_16_WRITABLE(bt_if_gap_char4_dev_name, BT_GATT_CHARC_PROP_READ, 0x0004, BT_SIG_UUID16_DEVICE_NAME);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_gap_dev_name, BT_SIG_UUID_DEVICE_NAME,
                                  BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE, bt_if_gap_dev_name_callback);
/* For BQB test TC_GAR_SR_BV_07_C & TC_GAR_SR_BV_08_C*/
BT_GATTS_NEW_CHARC_USER_DESCRIPTION(bt_if_gap_dev_name_user_description,
                                    BT_GATTS_REC_PERM_READABLE, bt_if_gap_dev_name_callback);
/* For BQB test TC_GAR_SR_BI_01_C */
/* This test characteristic can not read and write */
BT_GATTS_NEW_CHARC_16(bt_if_gap_char4_serial_number, 0,
                      0x0007, BT_SIG_UUID16_SERIAL_NUMBER);
BT_GATTS_NEW_CHARC_VALUE_STR16(bt_if_gap_serial_number, BT_SIG_UUID_SERIAL_NUMBER,
                               0, 9, MY_VENDOR_SERIAL_NUMBER);
/* For BQB test TC_GAR_SR_BI_28_C */
//can not read and write.
BT_GATTS_NEW_CHARC_USER_DESCRIPTION_STR16(bt_if_gap_serial_number_user_description,
        0,
        8, "MediaTek");
BT_GATTS_NEW_CHARC_16_WRITABLE(bt_if_gap_char4_appearance, BT_GATT_CHARC_PROP_READ, 0x000a, BT_SIG_UUID16_APPEARANCE);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_gap_appearance, BT_SIG_UUID_APPEARANCE,
                                  BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE, bt_if_gap_appearance_callback);

BT_GATTS_NEW_CHARC_16(bt_if_gap_char4_ppcp, BT_GATT_CHARC_PROP_READ, 0x000c, BT_SIG_UUID16_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS);
BT_GATTS_NEW_CHARC_VALUE_HALFW8_WRITABLE(bt_if_gap_ppcp, BT_SIG_UUID_PERIPHERAL_PREFERRED_CONNECTION_PARAMETERS,
        BT_GATTS_REC_PERM_READABLE, 8, 0x0580, 0x0c80, 0x0010, 0x0333);
static const bt_gatts_service_rec_t *bt_if_gap_service_rec[] = {
    (const bt_gatts_service_rec_t *) &bt_if_gap_primary_service,
    (const bt_gatts_service_rec_t *) &bt_if_gap_include_manufacturer_service,
    (const bt_gatts_service_rec_t *) &bt_if_gap_char4_dev_name,
    (const bt_gatts_service_rec_t *) &bt_if_gap_dev_name,
    (const bt_gatts_service_rec_t *) &bt_if_gap_dev_name_user_description,
    (const bt_gatts_service_rec_t *) &bt_if_gap_char4_serial_number,
    (const bt_gatts_service_rec_t *) &bt_if_gap_serial_number,
    (const bt_gatts_service_rec_t *) &bt_if_gap_serial_number_user_description,
    (const bt_gatts_service_rec_t *) &bt_if_gap_char4_appearance,
    (const bt_gatts_service_rec_t *) &bt_if_gap_appearance,
    (const bt_gatts_service_rec_t *) &bt_if_gap_char4_ppcp,
    (const bt_gatts_service_rec_t *) &bt_if_gap_ppcp
};

static const bt_gatts_service_t bt_if_gap_service = {
    .starting_handle = 0x0001,
    .ending_handle = 0x000c,
    .required_encryption_key_size = 7,
    .records = bt_if_gap_service_rec
};

void bt_gatt_service_gap_gatts_bqb()
{
    bt_if_gap_char4_dev_name.value.properties = BT_GATT_CHARC_PROP_READ | BT_GATT_CHARC_PROP_WRITE | BT_GATT_CHARC_PROP_WRITE_WITHOUT_RSP;
    bt_if_gap_char4_appearance.value.properties = BT_GATT_CHARC_PROP_READ | BT_GATT_CHARC_PROP_WRITE | BT_GATT_CHARC_PROP_WRITE_WITHOUT_RSP;
}
//GATT 0x0011
static uint16_t bt_if_gatt_notify = 0; //Client Characteristic Configuration
static uint32_t bt_if_gatt_client_config_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    //TODO:Should record for each connection handle.
    if (rw == BT_GATTS_CALLBACK_WRITE) {
        if (size != sizeof(bt_if_gatt_notify)) { //Size check
            return 0;
        }
        bt_if_gatt_notify = *(uint16_t *)data;
    } else {
        if (size != 0) {
            uint16_t *buf = (uint16_t *) data;
            *buf = bt_if_gatt_notify;
        }
    }
    return sizeof(bt_if_gatt_notify);
}
static uint16_t bt_if_gatt_scc = 0; //Server Characteristic Configuration
static uint32_t bt_if_gatt_server_config_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    //TODO:Should record for each connection handle.
    if (rw == BT_GATTS_CALLBACK_WRITE) {
        if (size != sizeof(bt_if_gatt_scc)) { //Size check
            return 0;
        }
        bt_if_gatt_scc = *(uint16_t *)data;
    } else {
        if (size != 0) {
            uint16_t *buf = (uint16_t *) data;
            *buf = bt_if_gatt_scc;
        }
    }
    return sizeof(bt_if_gatt_scc);
}
BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_gatt_primary_service, BT_GATT_UUID16_GATT_SERVICE);
BT_GATTS_NEW_CHARC_16(bt_if_gatt_char4_service_changed, BT_GATT_CHARC_PROP_READ | BT_GATT_CHARC_PROP_NOTIFY | BT_GATT_CHARC_PROP_INDICATE,
                      0x0013, BT_SIG_UUID16_SERVICE_CHANGED);
BT_GATTS_NEW_CHARC_VALUE_UINT32_WRITABLE(bt_if_gatt_service_changed, BT_SIG_UUID_SERVICE_CHANGED,
        0x2, 0x0001050F);
BT_GATTS_NEW_CLIENT_CHARC_CONFIG(bt_if_gatt_client_config,
                                 BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE,
                                 bt_if_gatt_client_config_callback);
BT_GATTS_NEW_SERVER_CHARC_CONFIG(bt_if_gatt_server_config,
                                 BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE,
                                 bt_if_gatt_server_config_callback);
static const bt_gatts_service_rec_t *bt_if_gatt_service_rec[] = {
    (const bt_gatts_service_rec_t *) &bt_if_gatt_primary_service,
    (const bt_gatts_service_rec_t *) &bt_if_gatt_char4_service_changed,
    (const bt_gatts_service_rec_t *) &bt_if_gatt_service_changed,
    (const bt_gatts_service_rec_t *) &bt_if_gatt_client_config,
    (const bt_gatts_service_rec_t *) &bt_if_gatt_server_config
};

static const bt_gatts_service_t bt_if_gatt_service = {
    .starting_handle = 0x0011,
    .ending_handle = 0x0015,
    .required_encryption_key_size = 7,
    .records = bt_if_gatt_service_rec
};
/*---------------------------------------------*/
BT_GATTS_NEW_PRIMARY_SERVICE_16(gatt_primary_service, BT_GATT_UUID16_GATT_SERVICE);
BT_GATTS_NEW_CHARC_16(gatt_char4_service_changed, BT_GATT_CHARC_PROP_READ | BT_GATT_CHARC_PROP_NOTIFY | BT_GATT_CHARC_PROP_INDICATE,
                      0x0013, BT_SIG_UUID16_SERVICE_CHANGED);
BT_GATTS_NEW_CHARC_VALUE_UINT32_WRITABLE(gatt_service_changed, BT_SIG_UUID_SERVICE_CHANGED,
        0x2, 0x0001050F);
static const bt_gatts_service_rec_t *gatt_service_rec[] = {
    (const bt_gatts_service_rec_t *) &gatt_primary_service,
    (const bt_gatts_service_rec_t *) &gatt_char4_service_changed,
    (const bt_gatts_service_rec_t *) &gatt_service_changed
};

static const bt_gatts_service_t bt_if_gatt_service_ro = {
    .starting_handle = 0x0011,
    .ending_handle = 0x0013,
    .required_encryption_key_size = 7,
    .records = gatt_service_rec
};

/*---------------------------------------------*/

//Battery State Service 0x0021
static uint8_t bas_level = 50;

static uint16_t bt_if_bas_notify = 0; //Client Characteristic Configuration
//Return Read or Write size.

//vendor 0x0500
char gatts_manufacture_name[64] = {"M"};
uint32_t length_of_gatts_manufacture_name = 1;
uint32_t buf_size_of_gatts_manufacture_name = sizeof(gatts_manufacture_name);
char gatts_manufacture_name_write_buff[64] = {"MEDIATEK_01_MEDIATEK_02_MEDIATEK_03_MEDIATEK_04_MEDIATEK_05"};
uint32_t execute_write_status;//It could be BT_ATT_ERRCODE_INVALID_ATTRIBUTE_VALUE_LENGTH, BT_ATT_ERRCODE_INVALID_OFFSET and BT_ATT_ERRCODE_APPLICATION_ERROR

char gatts_manufacture_name_1[64] = {"M"};
uint32_t length_of_gatts_manufacture_name_1 = 1;
uint32_t buf_size_of_gatts_manufacture_name_1 = sizeof(gatts_manufacture_name);
char gatts_manufacture_name_write_buff_1[64] = {"MEDIATEK_01_MEDIATEK_02_MEDIATEK_03_MEDIATEK_04_MEDIATEK_05"};
uint32_t execute_write_status_1;//It could be BT_ATT_ERRCODE_INVALID_ATTRIBUTE_VALUE_LENGTH, BT_ATT_ERRCODE_INVALID_OFFSET and BT_ATT_ERRCODE_APPLICATION_ERROR


extern const bt_gatts_service_t bt_if_hr_service;

//server collects all service
const bt_gatts_service_t *bt_if_gatt_server[] = {
    &bt_if_gap_service,//0x0001
    &bt_if_gatt_service_ro,//0x0010
    //&bt_if_bas_service,//0x0020
    //&bt_if_vendor_service,//0x500
    &bt_if_hr_service,
    NULL
};

//bt_gatt_service_change_x() are example to change service of server.
void bt_gatt_service_change_notify()
{
    uint8_t i;
    uint8_t buf[64] = {0};
    bt_gattc_charc_value_notification_indication_t *bas_noti_rsp;
    bas_noti_rsp = (bt_gattc_charc_value_notification_indication_t *) buf;
    for (i = 0; i < BT_CONNECTION_MAX ; i++) {
        if (connection_cb[i].connection_handle > 0) {
            //TODO:IT just a example. In real case  you have to record each connection's bt_if_gatt_notify.
            if (bt_if_gatt_notify > 0) {
                if (bt_if_gatt_notify == CLIENT_CHARC_CONFIGURATION_NOTIFICATION) {
                    bas_noti_rsp->att_req.opcode = BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION;
                } else {
                    if (connection_cb[i].gatts_wait_att_rx_opcode == BT_ATT_OPCODE_HANDLE_VALUE_CONFIRMATION) {
                        BT_LOGD("GATT_SERVER", "Waiting handle value confirmation.\n");
                        break;
                    }
                    connection_cb[i].gatts_wait_att_rx_opcode = BT_ATT_OPCODE_HANDLE_VALUE_CONFIRMATION;
                    bas_noti_rsp->att_req.opcode = BT_ATT_OPCODE_HANDLE_VALUE_INDICATION;
                }

                bas_noti_rsp->att_req.handle = 0x0013;
                memcpy((void *)(bas_noti_rsp->att_req.attribute_value), (void *) & (bt_if_gatt_service_changed.value.value_uint_32), sizeof(bt_if_gatt_service_changed.value.value_uint_32));

                bas_noti_rsp->attribute_value_length = 7;
                if (BT_STATUS_SUCCESS != bt_gatts_send_charc_value_notification_indication(connection_cb[i].connection_handle, bas_noti_rsp)) {
                    connection_cb[i].gatts_wait_att_rx_opcode = 0;
                }
            }
        }
    }
}
void bt_gatt_service_change_1()
{
    bt_if_gatt_server[0] = &bt_if_gap_service;
    bt_if_gatt_server[1] = &bt_if_gatt_service;
    // bt_if_gatt_server[2] = &bt_if_vendor_service;
    bt_if_gatt_server[2] = NULL;
    bt_if_gatt_service_changed.value.value_uint_32 = bt_if_gatt_service_changed.value.value_uint_32 + 1;
    bt_gatt_service_change_notify();
}

void bt_gatt_service_change_2()
{
    bt_if_gatt_server[0] = &bt_if_gap_service;
    bt_if_gatt_server[1] = &bt_if_gatt_service_ro;
    //bt_if_gatt_server[2] = &bt_if_bas_service;
    //bt_if_gatt_server[3] = &bt_if_vendor_service;
    bt_if_gatt_server[2] = NULL;
    bt_if_gatt_service_changed.value.value_uint_32 = bt_if_gatt_service_changed.value.value_uint_32 + 1;
    bt_gatt_service_change_notify();
}

//When GATTS get req from remote client, GATTS will call bt_get_gatt_server() to get application's gatt service DB.
//You have to return the DB(bt_gatts_service_t pointer) to gatts stack.
const bt_gatts_service_t ** bt_get_gatt_server()
{
    return bt_if_gatt_server;
}

void bt_gatt_service_regular_handle()
{
    uint8_t i;
    uint8_t buf[64] = {0};
    bt_gattc_charc_value_notification_indication_t *bas_noti_rsp;
    bas_noti_rsp = (bt_gattc_charc_value_notification_indication_t *) buf;
    for (i = 0; i < BT_CONNECTION_MAX ; i++) {
        if (connection_cb[i].connection_handle > 0) {
            //TODO:IT just a example. In real case  you have to record each connection's bt_if_bas_notify.
            if (bt_if_bas_notify > 0) {
                //Test code...
                if (bas_level < 100) {
                    bas_level++;
                }
                /*
                //Force test Indication
                if (connection_cb[i].gatts_wait_att_rx_opcode == BT_ATT_OPCODE_HANDLE_VALUE_CONFIRMATION) {
                    BT_LOGD("GATT_SERVER","Waiting handle value confirmation.\n");
                    break;
                }
                connection_cb[i].gatts_wait_att_rx_opcode = BT_ATT_OPCODE_HANDLE_VALUE_CONFIRMATION;
                bas_noti_rsp->att_req.opcode = BT_ATT_OPCODE_HANDLE_VALUE_INDICATION;
                */

                if (bt_if_bas_notify == CLIENT_CHARC_CONFIGURATION_NOTIFICATION) {
                    bas_noti_rsp->att_req.opcode = BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION;
                } else {
                    if (connection_cb[i].gatts_wait_att_rx_opcode == BT_ATT_OPCODE_HANDLE_VALUE_CONFIRMATION) {
                        BT_LOGD("GATT_SERVER", "Waiting handle value confirmation.\n");
                        break;
                    }
                    connection_cb[i].gatts_wait_att_rx_opcode = BT_ATT_OPCODE_HANDLE_VALUE_CONFIRMATION;
                    bas_noti_rsp->att_req.opcode = BT_ATT_OPCODE_HANDLE_VALUE_INDICATION;
                }

                bas_noti_rsp->att_req.handle = 0x0023;
                bas_noti_rsp->att_req.attribute_value[0] = bas_level;

                bas_noti_rsp->attribute_value_length = 4;
                if (BT_STATUS_SUCCESS != bt_gatts_send_charc_value_notification_indication(connection_cb[i].connection_handle, bas_noti_rsp)) {
                    connection_cb[i].gatts_wait_att_rx_opcode = 0;
                }
            }
        }
    }
}

