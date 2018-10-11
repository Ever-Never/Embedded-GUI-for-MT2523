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


#include "bt_gatt.h"
#include "bt_gatts.h"
#include "atci.h"
#include "bt_hci.h"
#include "bt_gap_le.h"
#include "bt_init.h"
#include "ble_hogp.h"
#include "FreeRTOS.h"


#define BLE_HOGP_START_HANDLE                       (0x0720)
#define BLE_HOGP_END_HANDLE                         (0x0760)

#define HOGP_SERVICE_HID_UUID                       (0x1812)
#define HOGP_SERVICE_DEVICE_INFO_UUID               (0x180A)
#define HOGP_SERVICE_BATTERY_UUID                   (0x180F)

#define HOGP_CHAR_REPORT_MAP_UUID                   (0x2A4B)
#define HOGP_CHAR_REPORT_UUID                       (0x2A4D)
#define HOGP_CHAR_HID_INFO_UUID                     (0x2A4A)
#define HOGP_CHAR_HID_CTRL_POINT_UUID               (0x2A4C)
#define HOGP_CHAR_PNP_ID_UUID                       (0x2A50)
#define HOGP_CHAR_BATTERY_LEVEL_UUID                (0x2A19)

#define HOGP_CHAR_PROTOCOL_MODE_UUID                (0x2A4E)
#define HOGP_CHAR_BK_INPUT_REPORT_UUID              (0x2A22)
#define HOGP_CHAR_BK_OUTPUT_REPORT_UUID             (0x2A32)

#define HOGP_CHAR_PNP_ID_VALUE_HANDLE               (0x0017)
#define HOGP_CHAR_BATTERY_LEVEL_VALUE_HANDLE        (0x001A)
#define HOGP_CHAR_REPORT_MAP_VALUE_HANDLE           (0x001E)
#define HOGP_CHAR_REPORT_VALUE_HANDLE               (0x0020)
#define HOGP_CHAR_HID_INFO_VALUE_HANDLE             (0x0024)
#define HOGP_CHAR_HID_CONTROL_POINT_VALUE_HANDLE    (0x0026)
#define HOGP_CHAR_HID_PROTOCOL_MODE_VALUE_HANDLE    (0x0028)
#define HOGP_CHAR_HID_BK_INPUT_VALUE_HANDLE         (0x002A)
#define HOGP_CHAR_HID_BK_OUTPUT_VALUE_HANDLE        (0x002D)

#define BT_GATT_UUID16_REPORT_REFERENCE             (0x2908)

static const bt_uuid_t HOGP_CHAR_REPORT_MAP_UUID128 =
    BT_UUID_INIT_WITH_UUID16(HOGP_CHAR_REPORT_MAP_UUID);
static const bt_uuid_t HOGP_CHAR_REPORT_UUID128 =
    BT_UUID_INIT_WITH_UUID16(HOGP_CHAR_REPORT_UUID);
static const bt_uuid_t HOGP_CHAR_HID_INFO_UUID128 =
    BT_UUID_INIT_WITH_UUID16(HOGP_CHAR_HID_INFO_UUID);
static const bt_uuid_t HOGP_CHAR_HID_CTRL_POINT_UUID128 =
    BT_UUID_INIT_WITH_UUID16(HOGP_CHAR_HID_CTRL_POINT_UUID);
static const bt_uuid_t HOGP_CHAR_PNP_ID_UUID128 =
    BT_UUID_INIT_WITH_UUID16(HOGP_CHAR_PNP_ID_UUID);
static const bt_uuid_t HOGP_CHAR_BATTERY_LEVEL_UUID128 =
    BT_UUID_INIT_WITH_UUID16(HOGP_CHAR_BATTERY_LEVEL_UUID);
static const bt_uuid_t HOGP_CHAR_HID_PROTOCOL_MODE_UUID128 =
    BT_UUID_INIT_WITH_UUID16(HOGP_CHAR_PROTOCOL_MODE_UUID);
static const bt_uuid_t HOGP_CHAR_HID_BK_INPUT_UUID128 =
    BT_UUID_INIT_WITH_UUID16(HOGP_CHAR_BK_INPUT_REPORT_UUID);


static const bt_uuid_t BT_GATT_UUID_REPORT_REFERENCE =
    BT_UUID_INIT_WITH_UUID16(BT_GATT_UUID16_REPORT_REFERENCE);

#define BT_GATTS_NEW_REPORT_REFERENCE(name, _perm, _callback)          \
    static const bt_gatts_client_characteristic_config_t name = {       \
    .rec_hdr.uuid_ptr = &BT_GATT_UUID_REPORT_REFERENCE,     \
    .rec_hdr.perm = _perm,                                               \
    .rec_hdr.value_len = 0,                                             \
    .callback = _callback                                                \
    }


static ble_hogp_cntx_t g_hogp_cntx;
static ble_hogp_cntx_t *g_hogp_p = &g_hogp_cntx;
static bt_bd_addr_t hogp_local_random_addr = {0xC5, 0x02, 0x02, 0x04, 0x04, 0xC5};

static atci_status_t ble_hogp_atci_callback(atci_parse_cmd_param_t *parse_cmd);

static atci_cmd_hdlr_item_t ble_hogp_at_cmd = {
    .command_head = "AT+HOGP",    /**< AT command string. */
    .command_hdlr = ble_hogp_atci_callback,
    .hash_value1 = 0,
    .hash_value2 = 0,
};


static uint8_t ble_hogp_char2hex(uint8_t value)
{
    if (value >= '0' && value <= '9')
    {
        return value - '0';
    }

    if (value >= 'a' && value <= 'f')
    {
        return value - 'a' + 0x0A;
    }

    if (value >= 'A' && value <= 'F')
    {
        return value - 'A' + 0x0A;
    }

    return 0x10;
}



/* Inverse copy. And at most copy len bytes of src. At mean time, convert char to hex value when copying. */
static uint32_t ble_hogp_str_ivcpy_char2hex(uint8_t *dst, const uint8_t *src, uint32_t len)
{
    uint32_t src_len, dst_len = 0;
    uint8_t lv, hv, *dst_orig = dst;
    
    if (!dst ||
        !src ||
        ((src_len = strlen((char *)src)) % 2) ||
        !len ||
        (len % 2))
    {
        return 0;
    }
    
    src_len = len > src_len ? src_len : len;

    do 
    {
        lv = ble_hogp_char2hex(*(src + src_len -1));
        hv = ble_hogp_char2hex(*(src + src_len -2));
        if (lv > 0x0F || hv > 0x0F)
        {
            break;
        }
        *dst++ = (hv << 4) | (lv & 0x0F);
        dst_len++;
    } while (src_len -= 2);
    
    if (src_len)
    {
        memset(dst_orig, 0, dst_len);
        return 0;
    }

    return dst_len;
}


/* Convert hex char string to hex value array */
static uint32_t ble_hogp_str_char2hex(uint8_t *dst, const uint8_t *src, uint32_t len)
{
    uint32_t src_len, dst_len = 0;
    uint8_t lv, hv;
    
    if (!dst ||
        !src ||
        ((src_len = strlen((char *)src)) % 2) ||
        !len ||
        (len % 2))
    {
        return 0;
    }
    
    src_len = len > src_len ? src_len : len;

    do 
    {
        hv = ble_hogp_char2hex(*(src++));
        lv = ble_hogp_char2hex(*(src++));
        if (lv > 0x0F || hv > 0x0F)
        {
            break;
        }
        *dst++ = (hv << 4) | (lv & 0x0F);
        dst_len++;
    } while (src_len -= 2);
    
    if (src_len)
    {
        *(--dst) = 0;
        return 0;
    }

    return dst_len;
}


static uint32_t ble_hogp_char_report_map_value_cb (const uint8_t rw, uint16_t handle,
    void *data, uint16_t size, uint16_t offset)
{
    uint32_t str_len;
    uint8_t report_map[512] = {0};
    uint8_t return_len = 0;
    
    BT_LOGI("HOGP", "[HOGP]ble_hogp_char_report_map_value_cb, rw = %d, handle = %d, size = %d, offset = %d\r\n",
        rw, handle, size, offset);

    str_len = ble_hogp_str_char2hex(
        report_map, 
        (const uint8_t *)"05010906A101050719E029E71500250175019508810295017508810195067508150025E70507190029E78100C0",
        512);

    if (rw == BT_GATTS_CALLBACK_READ) {
        if (size > 0) {
            
            memcpy(data, report_map + offset, size);
        }
    }

    return_len = size > 0? size:str_len;
    BT_LOGI("HOGP", "[HOGP]report_map_len = %d, return len = %d\r\n", str_len, return_len);
    return return_len ;
}


static uint32_t ble_hogp_char_report_value_cb (const uint8_t rw, uint16_t handle,
    void *data, uint16_t size, uint16_t offset)
{
    BT_LOGI("HOGP", "[HOGP]ble_hogp_char_report_value_cb\r\n");
    return 0;
}

static uint32_t ble_hogp_dscrp_report_ccc_cb (const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    BT_LOGI("HOGP", "[HOGP]ble_hogp_dscrp_report_ccc_cb, rw = %d, handle = %d, size = %d, offset = %d\r\n",
        rw, handle, size, offset);

    if (rw == BT_GATTS_CALLBACK_WRITE) {
        if (size == sizeof(uint16_t)) {
            g_hogp_p->report_ccc = *(uint16_t*)data;
        }

    } else if (rw == BT_GATTS_CALLBACK_READ) {
        if (size == sizeof(uint16_t)) {
            memcpy(data, &g_hogp_p->report_ccc, sizeof(uint16_t));
        }
    } 
    
    return sizeof(uint16_t);
}


static uint32_t ble_hogp_dscrp_report_reference_cb (const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    uint8_t report_ref[2] = {0x00, 0x01};   /*{Report ID, Report Type: Input Report}*/
    
    BT_LOGI("HOGP", "[HOGP]ble_hogp_dscrp_report_reference_cb, rw = %d, handle = %d, size = %d, offset = %d\r\n",
        rw, handle, size, offset);

    if (rw == BT_GATTS_CALLBACK_READ) {
        if (size == 2) {
            memcpy(data, report_ref, 2);
        }
    }
    
    return 2;
}


static uint32_t ble_hogp_char_hid_info_value_cb (const uint8_t rw, uint16_t handle,
    void *data, uint16_t size, uint16_t offset)
{
    uint32_t des_len;
    uint8_t hid_info[4] = {0};
    
    BT_LOGI("HOGP", "[HOGP]ble_hogp_char_hid_info_value_cb, rw = %d, handle = %d, size = %d, offset = %d\r\n",
        rw, handle, size, offset);

    des_len = ble_hogp_str_ivcpy_char2hex(hid_info, 
                                (const uint8_t *)"1101", 
                                4);/* Length of "1101" */

    if (rw == BT_GATTS_CALLBACK_READ) {
        if (size == des_len) {
            memcpy(data, hid_info, des_len);
        }
    }

    return des_len;
}


static uint32_t ble_hogp_char_hid_cntl_point_value_cb (const uint8_t rw, uint16_t handle,
    void *data, uint16_t size, uint16_t offset)
{
    BT_LOGI("HOGP", "[HOGP]ble_hogp_char_hid_cntl_point_value_cb, rw = %d, handle = %d, size = %d, offset = %d\r\n",
        rw, handle, size, offset);
    return 0;
}

static uint32_t ble_hogp_char_pnp_id_value_cb (const uint8_t rw, uint16_t handle,
    void *data, uint16_t size, uint16_t offset)
{
    uint8_t pnp_id[7] = {0x01, 0x46, 0x00, 0x4C, 0x01, 0x00, 0x01};
    
    BT_LOGI("HOGP", "[HOGP]ble_hogp_char_pnp_id_value_cb, rw = %d, handle = %d, size = %d, offset = %d\r\n", 
        rw, handle, size, offset);

    if (rw == BT_GATTS_CALLBACK_READ) {
        if (size == 7) {
            memcpy(data, pnp_id, 7);
        }
    }

    return 7;
}


static uint32_t ble_hogp_char_battery_level_value_cb (const uint8_t rw, uint16_t handle,
    void *data, uint16_t size, uint16_t offset)
{
    uint8_t bat_level = 100;
    
    BT_LOGI("HOGP", "[HOGP]ble_hogp_char_battery_level_value_cb, rw = %d, handle = %d, size = %d, offset = %d\r\n", 
        rw, handle, size, offset);

    if (rw == BT_GATTS_CALLBACK_READ) {
        if (size == sizeof(uint8_t)) {
            memcpy(data, &bat_level, sizeof(uint8_t));
        }
    }

    return sizeof(uint8_t);
}

static uint32_t ble_hogp_dscrp_battery_level_ccc_cb (const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    BT_LOGI("HOGP", "[HOGP]ble_hogp_dscrp_battery_level_ccc_cb, rw = %d, handle = %d, size = %d, offset = %d\r\n",
        rw, handle, size, offset);

    if (rw == BT_GATTS_CALLBACK_WRITE) {
        if (size == sizeof(uint16_t)) {
            g_hogp_p->bat_level_ccc = *(uint16_t*)data;
        }
    } else if (rw == BT_GATTS_CALLBACK_READ) {
        if (size == sizeof(uint16_t)) {
           memcpy(data, &(g_hogp_p->bat_level_ccc), size);
        }
    }
    
    return sizeof(uint16_t);
}


static uint32_t ble_hogp_char_hid_protocol_mode_value_cb (const uint8_t rw, uint16_t handle,
    void *data, uint16_t size, uint16_t offset)
{
    BT_LOGI("HOGP", "[HOGP]ble_hogp_char_hid_protocol_mode_value_cb, rw = %d, handle = %d, size = %d, offset = %d\r\n",
        rw, handle, size, offset);
    return 0;
}

static uint32_t ble_hogp_char_hid_bk_input_value_cb (const uint8_t rw, uint16_t handle,
    void *data, uint16_t size, uint16_t offset)
{
    BT_LOGI("HOGP", "[HOGP]ble_hogp_char_hid_bk_input_value_cb, rw = %d, handle = %d, size = %d, offset = %d\r\n",
        rw, handle, size, offset);
    return 0;
}

static uint32_t ble_hogp_dscrp_bk_input_ccc_cb (const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    BT_LOGI("HOGP", "[HOGP]ble_hogp_dscrp_bk_input_ccc_cb, rw = %d, handle = %d, size = %d, offset = %d\r\n",
        rw, handle, size, offset);

    if (rw == BT_GATTS_CALLBACK_WRITE) {
        if (size == sizeof(uint16_t)) {
            g_hogp_p->bk_input_ccc = *(uint16_t*)data;
        }

    } else if (rw == BT_GATTS_CALLBACK_READ) {
        if (size == sizeof(uint16_t)) {
            memcpy(data, &g_hogp_p->bk_input_ccc, sizeof(uint16_t));
        }
    } 
    
    return sizeof(uint16_t);
}

static uint32_t ble_hogp_char_hid_bk_output_value_cb (const uint8_t rw, uint16_t handle,
    void *data, uint16_t size, uint16_t offset)
{
    BT_LOGI("HOGP", "[HOGP]ble_hogp_char_hid_bk_output_value_cb, rw = %d, handle = %d, size = %d, offset = %d\r\n",
        rw, handle, size, offset);
    return 0;
}


/*Device information service*/
BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_dev_info_primary_service, HOGP_SERVICE_DEVICE_INFO_UUID);

/*PnP ID characteristic*/
BT_GATTS_NEW_CHARC_16(bt_if_char_pnp_id,
                      BT_GATT_CHARC_PROP_READ, HOGP_CHAR_PNP_ID_VALUE_HANDLE, HOGP_CHAR_PNP_ID_UUID);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_char_pnp_id_value, HOGP_CHAR_PNP_ID_UUID128,
    BT_GATTS_REC_PERM_READABLE, ble_hogp_char_pnp_id_value_cb);

static const bt_gatts_service_rec_t *bt_if_device_info_service_rec[] = { 
    (const bt_gatts_service_rec_t *) &bt_if_dev_info_primary_service,

    (const bt_gatts_service_rec_t *) &bt_if_char_pnp_id,
    (const bt_gatts_service_rec_t *) &bt_if_char_pnp_id_value
};

const bt_gatts_service_t bt_if_hogp_device_info_service = {
    .starting_handle = 0x0015,
    .ending_handle = 0x0017,
    .required_encryption_key_size = 0,
    .records = bt_if_device_info_service_rec
};


/*Battery service*/
BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_battery_primary_service, HOGP_SERVICE_BATTERY_UUID);

/*Battery level characteristic*/
BT_GATTS_NEW_CHARC_16(bt_if_char_battery_level,
                      BT_GATT_CHARC_PROP_READ | BT_GATT_CHARC_PROP_NOTIFY, HOGP_CHAR_BATTERY_LEVEL_VALUE_HANDLE, HOGP_CHAR_BATTERY_LEVEL_UUID);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_char_battery_level_value, HOGP_CHAR_BATTERY_LEVEL_UUID128,
    BT_GATTS_REC_PERM_READABLE, ble_hogp_char_battery_level_value_cb);

BT_GATTS_NEW_CLIENT_CHARC_CONFIG(bt_if_char_battery_level_config,
                                 BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE,
                                 ble_hogp_dscrp_battery_level_ccc_cb);

static const bt_gatts_service_rec_t *bt_if_battery_service_rec[] = { 
    (const bt_gatts_service_rec_t *) &bt_if_battery_primary_service,

    (const bt_gatts_service_rec_t *) &bt_if_char_battery_level,
    (const bt_gatts_service_rec_t *) &bt_if_char_battery_level_value,
    (const bt_gatts_service_rec_t *) &bt_if_char_battery_level_config
};

const bt_gatts_service_t bt_if_hogp_battery_service = {
    .starting_handle = 0x0018,
    .ending_handle = 0x001B,
    .required_encryption_key_size = 0,
    .records = bt_if_battery_service_rec
};


/*****************HID service*****************/
BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_hid_primary_service, HOGP_SERVICE_HID_UUID);

/*report map characteristic*/
BT_GATTS_NEW_CHARC_16(bt_if_char_report_map,
                      BT_GATT_CHARC_PROP_READ, HOGP_CHAR_REPORT_MAP_VALUE_HANDLE, HOGP_CHAR_REPORT_MAP_UUID);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_char_report_map_value, HOGP_CHAR_REPORT_MAP_UUID128,
    BT_GATTS_REC_PERM_READABLE_ENCRYPTION, ble_hogp_char_report_map_value_cb);

/*report characteristic*/
BT_GATTS_NEW_CHARC_16(bt_if_char_report,
                      BT_GATT_CHARC_PROP_READ | BT_GATT_CHARC_PROP_NOTIFY, HOGP_CHAR_REPORT_VALUE_HANDLE, HOGP_CHAR_REPORT_UUID);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_char_report_value, HOGP_CHAR_REPORT_UUID128,
    BT_GATTS_REC_PERM_READABLE, ble_hogp_char_report_value_cb);

BT_GATTS_NEW_CLIENT_CHARC_CONFIG(bt_if_char_report_config,
                                 BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE,
                                 ble_hogp_dscrp_report_ccc_cb);
BT_GATTS_NEW_REPORT_REFERENCE(bt_if_char_report_reference,
                                 BT_GATTS_REC_PERM_READABLE,
                                 ble_hogp_dscrp_report_reference_cb);


/*HID information characteristic*/
BT_GATTS_NEW_CHARC_16(bt_if_char_hid_info,
                      BT_GATT_CHARC_PROP_READ, HOGP_CHAR_HID_INFO_VALUE_HANDLE, HOGP_CHAR_HID_INFO_UUID);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_char_hid_info_value, HOGP_CHAR_HID_INFO_UUID128,
    BT_GATTS_REC_PERM_READABLE, ble_hogp_char_hid_info_value_cb);

/*HID control point characteristic*/
BT_GATTS_NEW_CHARC_16(bt_if_char_hid_control_point,
                      BT_GATT_CHARC_PROP_WRITE_WITHOUT_RSP, HOGP_CHAR_HID_CONTROL_POINT_VALUE_HANDLE, HOGP_CHAR_HID_CTRL_POINT_UUID);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_char_hid_control_point_value, HOGP_CHAR_HID_CTRL_POINT_UUID128,
    BT_GATTS_REC_PERM_WRITABLE, ble_hogp_char_hid_cntl_point_value_cb);


/*HID protocol mode characteristic*/
BT_GATTS_NEW_CHARC_16(bt_if_char_hid_protocol_mode,
                      BT_GATT_CHARC_PROP_READ | BT_GATT_CHARC_PROP_WRITE_WITHOUT_RSP, HOGP_CHAR_HID_PROTOCOL_MODE_VALUE_HANDLE, HOGP_CHAR_PROTOCOL_MODE_UUID);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_char_hid_protocol_mode_value, HOGP_CHAR_HID_PROTOCOL_MODE_UUID128,
    BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE, ble_hogp_char_hid_protocol_mode_value_cb);

/*HID boot keyboard input report characteristic*/
BT_GATTS_NEW_CHARC_16(bt_if_char_hid_bk_input,
                      BT_GATT_CHARC_PROP_READ | BT_GATT_CHARC_PROP_NOTIFY, HOGP_CHAR_HID_BK_INPUT_VALUE_HANDLE, HOGP_CHAR_BK_INPUT_REPORT_UUID);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_char_hid_bk_input_value, HOGP_CHAR_HID_BK_INPUT_UUID128,
    BT_GATTS_REC_PERM_READABLE, ble_hogp_char_hid_bk_input_value_cb);

BT_GATTS_NEW_CLIENT_CHARC_CONFIG(bt_if_char_hid_bk_input_config,
                                 BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE,
                                 ble_hogp_dscrp_bk_input_ccc_cb);


/*HID boot keyboard output report characteristic*/
BT_GATTS_NEW_CHARC_16(bt_if_char_hid_bk_output,
                      BT_GATT_CHARC_PROP_READ | BT_GATT_CHARC_PROP_WRITE | BT_GATT_CHARC_PROP_WRITE_WITHOUT_RSP, HOGP_CHAR_HID_BK_OUTPUT_VALUE_HANDLE, HOGP_CHAR_BK_OUTPUT_REPORT_UUID);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_char_hid_bk_output_value, HOGP_CHAR_HID_BK_INPUT_UUID128,
    BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE, ble_hogp_char_hid_bk_output_value_cb);


static const bt_gatts_service_rec_t *bt_if_hid_service_rec[] = {   
    (const bt_gatts_service_rec_t *) &bt_if_hid_primary_service,
        
    (const bt_gatts_service_rec_t *) &bt_if_char_report_map,
    (const bt_gatts_service_rec_t *) &bt_if_char_report_map_value,
    
    (const bt_gatts_service_rec_t *) &bt_if_char_report,
    (const bt_gatts_service_rec_t *) &bt_if_char_report_value,
    (const bt_gatts_service_rec_t *) &bt_if_char_report_config,
    (const bt_gatts_service_rec_t *) &bt_if_char_report_reference,
    
    (const bt_gatts_service_rec_t *) &bt_if_char_hid_info,
    (const bt_gatts_service_rec_t *) &bt_if_char_hid_info_value,

    (const bt_gatts_service_rec_t *) &bt_if_char_hid_control_point,
    (const bt_gatts_service_rec_t *) &bt_if_char_hid_control_point_value,
    
    (const bt_gatts_service_rec_t *) &bt_if_char_hid_protocol_mode,
    (const bt_gatts_service_rec_t *) &bt_if_char_hid_protocol_mode_value,

    (const bt_gatts_service_rec_t *) &bt_if_char_hid_bk_input,
    (const bt_gatts_service_rec_t *) &bt_if_char_hid_bk_input_value,
    (const bt_gatts_service_rec_t *) &bt_if_char_hid_bk_input_config,

    (const bt_gatts_service_rec_t *) &bt_if_char_hid_bk_output,
    (const bt_gatts_service_rec_t *) &bt_if_char_hid_bk_output_value
};

const bt_gatts_service_t bt_if_hogp_hid_service = {
    .starting_handle = 0x001C,
    .ending_handle = 0X002D,
    .required_encryption_key_size = 0,
    .records = bt_if_hid_service_rec
};


void ble_hogp_init(void)
{
    atci_status_t ret;

    BT_LOGI("HOGP", "[HOGP]ble_hogp_init\r\n");

    ret = atci_register_handler(&ble_hogp_at_cmd, sizeof(ble_hogp_at_cmd) / sizeof(atci_cmd_hdlr_item_t));

    if (ret != ATCI_STATUS_OK) {
        BT_LOGI("HOGP", "[HOGP]at_cmd_init register fail\r\n");
    }
}


static void ble_hogp_start_adv(void)
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
        .advertising_data_length = 13,
        .advertising_data = "DDDDDDDDDHOGP",
    };

    adv_data.advertising_data[0] = 2;
    adv_data.advertising_data[1] = BT_GAP_LE_AD_TYPE_FLAG;
    adv_data.advertising_data[2] = BT_GAP_LE_AD_FLAG_BR_EDR_NOT_SUPPORTED | BT_GAP_LE_AD_FLAG_GENERAL_DISCOVERABLE;
    adv_data.advertising_data[3] = 3;
    adv_data.advertising_data[4] = BT_GAP_LE_AD_TYPE_16_BIT_UUID_COMPLETE;
    adv_data.advertising_data[5] = HOGP_SERVICE_HID_UUID & 0x00FF;
    adv_data.advertising_data[6] = (HOGP_SERVICE_HID_UUID & 0xFF00)>>8;
    adv_data.advertising_data[7] = 5;
    adv_data.advertising_data[8] = BT_GAP_LE_AD_TYPE_NAME_COMPLETE;
    enable.advertising_enable = BT_HCI_ENABLE;

    bt_gap_le_set_advertising(&enable, &adv_param, &adv_data, NULL);
    
    bt_gatts_set_max_mtu(158);
}


static void ble_hogp_send_indication(uint8_t key)
{
    uint8_t report_value[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t buf[64]={0};
    
    bt_gattc_charc_value_notification_indication_t *req = 
                        (bt_gattc_charc_value_notification_indication_t*) buf;
    
    BT_LOGI("HOGP", "[HOGP]ble_hogp_send_indication: report_ccc = 0x%x\r\n", g_hogp_p->report_ccc);

    report_value[2] = key;
    req->attribute_value_length = 3 + 8;
    req->att_req.opcode = BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION;
    req->att_req.handle = HOGP_CHAR_REPORT_VALUE_HANDLE;
    memcpy(req->att_req.attribute_value, report_value, 8);
    bt_gatts_send_charc_value_notification_indication(g_hogp_p->conn_handle, req);
    
}

static int16_t ble_hogp_at_cmd_process(char *string)
{

   BT_LOGI("HOGP", "[HOGP]cmd = %s\r\n", string);

    if (0 == memcmp(string, "START", strlen("START"))) {
        bt_demo_power_on(BT_MODE_HOGP);
        
    } else if (0 == memcmp(string, "STOP", strlen("STOP"))) {
        bt_demo_power_off();
        
    } else if (0 == memcmp(string, "ENTER", strlen("ENTER"))) {
        ble_hogp_send_indication(0x28);
        ble_hogp_send_indication(0x00); 
    } else if (0 == memcmp(string, "LEFT", strlen("LEFT"))) {
        ble_hogp_send_indication(0x50);
        ble_hogp_send_indication(0x00);

    } else if (0 == memcmp(string, "RIGHT", strlen("RIGHT"))) {
        ble_hogp_send_indication(0x4F);
        ble_hogp_send_indication(0x00);

    } else if (0 == memcmp(string, "UP", strlen("UP"))) {
        ble_hogp_send_indication(0x52);
        ble_hogp_send_indication(0x00);

    } else if (0 == memcmp(string, "DOWN", strlen("DOWN"))) {
        ble_hogp_send_indication(0x51);
        ble_hogp_send_indication(0x00);

    } else if (0 == memcmp(string, "A", strlen("A"))) {
        ble_hogp_send_indication(0x04);
        ble_hogp_send_indication(0x00);

    } else {
        uint8_t temp;

        temp = atoi(string);
        ble_hogp_send_indication(temp);
        ble_hogp_send_indication(0x00);
    }

    return 0;
}


static atci_status_t ble_hogp_atci_callback(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    int16_t result;

    BT_LOGI("HOGP", "[HOGP]ble_hogp_atci_callback\r\n");

    response.response_flag = 0; /* Command Execute Finish. */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+HOGP=<command> */
            result = ble_hogp_at_cmd_process(parse_cmd->string_ptr + 8);
            if (result == 0) {
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                snprintf((char *)response.response_buf,
                         ATCI_UART_TX_FIFO_BUFFER_SIZE,
                         "command error:%d\r\n",
                         result);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        default :
            /* others are invalid command format */
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}


bt_status_t ble_hogp_event_callback(bt_msg_type_t msg, bt_status_t status,
    void *buff)
{
    BT_LOGI("HOGP", "[HOGP]ble_hogp_event_callback ,msg = 0x%x\r\n", msg);

    switch (msg) {
        case BT_POWER_ON_CNF:
             bt_gap_le_set_random_address((bt_bd_addr_ptr_t)hogp_local_random_addr);  
             break;

         case BT_GAP_LE_SET_RANDOM_ADDRESS_CNF:
            ble_hogp_start_adv();
            break;
            
        case BT_GAP_LE_CONNECT_IND:
        {
            bt_gap_le_connection_ind_t *connection_ind = (bt_gap_le_connection_ind_t *)buff;
            bt_hci_cmd_le_connection_update_t conn_params;

            g_hogp_p->conn_handle = connection_ind->connection_handle;

            conn_params.supervision_timeout = 0x0258;            /** TBC: 6000ms : 600 * 10 ms. */
            conn_params.connection_handle = connection_ind->connection_handle;
            conn_params.conn_interval_min = 0x0020;/** TBC: 40ms : 32 * 1.25 ms. */
            conn_params.conn_interval_max = 0x0020;/** TBC: 40ms : 32 * 1.25 ms. */
            conn_params.conn_latency = 0;
            bt_gap_le_update_connection_parameter(&conn_params);
        }
            break;

        case BT_GAP_LE_BONDING_REPLY_REQ_IND:
            {
                bt_gap_le_bonding_reply_req_ind_t *bond_ind = (bt_gap_le_bonding_reply_req_ind_t *)buff;

                BT_LOGI("HOGP", "[HOGP]BT_GAP_LE_BONDING_REPLY_REQ_IND ,passkey = %06u\r\n", bond_ind->passkey_display);
            }
            break;

        case BT_GAP_LE_BONDING_COMPLETE_IND:
            BT_LOGI("HOGP", "[HOGP]BT_GAP_LE_BONDING_COMPLETE_IND \r\n");
            break;

        case BT_GAP_LE_DISCONNECT_CNF:
            BT_LOGI("HOGP", "[HOGP]BT_GAP_LE_DISCONNECT_CNF \r\n");
            break;
            
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}


