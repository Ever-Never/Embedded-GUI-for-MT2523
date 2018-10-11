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

#include "ble_gatt_common.h"
#include "ble_ias.h"
#include "ble_gatt_srv.h"
#include "ble_gatt_utils.h"
#include "ble_gatt_srv_internal.h"

static ble_ias_context_t g_ias_context;

static ble_ias_dev_info_t g_ias_dev_info[BLE_GATT_MAX_LINK];

static uint32_t bt_if_ias_alert_level_callback (const uint8_t rw, uint16_t handle,
    void *data, uint16_t size, uint16_t offset);

static ble_ias_dev_info_t *ble_ias_get_dev_info(ble_ias_dev_info_type_t type,
    void *para);

static void ble_ias_reset_dev_info(ble_ias_dev_info_t *dev_info);

static const bt_uuid_t BT_SIG_UUID_ALERT_LEVEL =
    BT_UUID_INIT_WITH_UUID16(BT_SIG_UUID16_ALERT_LEVEL);

BT_GATTS_NEW_PRIMARY_SERVICE_16(bt_if_ias_primary_service,
    BT_GATT_UUID16_IMMEDIATE_ALERT_SERVICE);

BT_GATTS_NEW_CHARC_16_WRITABLE(bt_if_ias_char4_alert_level,
    BT_GATT_CHARC_PROP_WRITE_WITHOUT_RSP, BLE_IAS_VAL_HD_AL,
    BT_SIG_UUID16_ALERT_LEVEL);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(bt_if_ias_alert_level, BT_SIG_UUID_ALERT_LEVEL,
    BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE, bt_if_ias_alert_level_callback);

static const bt_gatts_service_rec_t *bt_if_ias_service_rec[] = {
    (const bt_gatts_service_rec_t *) &bt_if_ias_primary_service,
    (const bt_gatts_service_rec_t *) &bt_if_ias_char4_alert_level,
    (const bt_gatts_service_rec_t *) &bt_if_ias_alert_level
};

const bt_gatts_service_t bt_if_ias_service = {
    .starting_handle = BLE_IAS_START_HANDLE,
    .ending_handle = BLE_IAS_END_HANDLE,
    .required_encryption_key_size = BLE_IAS_REKS,
    .records = bt_if_ias_service_rec
};


/*****************************************************************************
* FUNCTION
*  bt_if_ias_alert_level_callback
* DESCRIPTION
* Alert level callback
* PARAMETERS
*  rw            [IN]        Reading or writing
*  handle        [IN]        Connect handle
*  data          [IN]        Data
*  size          [IN]        Data size
*  offset        [IN]        Offset
* RETURNS
*  uint32_t
*****************************************************************************/
static uint32_t bt_if_ias_alert_level_callback (const uint8_t rw, uint16_t handle,
    void *data, uint16_t size, uint16_t offset)
{
    uint32_t ret = 0;
    ble_gatt_char_alter_level_t char_al;
    ble_ias_dev_info_t *dev_info = NULL;

    ble_gatt_report("[gatt][ias]al_cb(s)--rw: %d, hd: %d, size: %d, offset: %d\n",
        rw, handle, size, offset);

    dev_info = ble_ias_get_dev_info(BLE_IAS_DEV_INFO_HANDLE, (void *)&handle);

    if (rw == BT_GATTS_CALLBACK_WRITE) {
        if (size == sizeof(dev_info->alert_level)) { //Size check
            dev_info->alert_level = *(uint8_t *)data;
            ret = size;
            char_al.al = dev_info->alert_level;
            char_al.handle = dev_info->handle;
            ble_gatt_srv_notify(BLE_GATT_SRV_IAS_AL_WRITE, &char_al);
        }
    } else if (rw == BT_GATTS_CALLBACK_READ) {
        if (size == sizeof(dev_info->alert_level)) { //Size check
            *(uint8_t *)data = dev_info->alert_level;
            ret = size;
            char_al.al = dev_info->alert_level;
            char_al.handle = dev_info->handle;
            ble_gatt_srv_notify(BLE_GATT_SRV_IAS_AL_READ, &char_al);
        } else if (0 == size) {
            ret = sizeof(dev_info->alert_level); 
        }
    }

    ble_gatt_report("[gatt][ias]al_cb(e)--ret: %d\n", ret);
    return ret;
}


/*****************************************************************************
* FUNCTION
*  ble_ias_get_dev_info
* DESCRIPTION
* Get connected device's detail infromation
* PARAMETERS
*  type        [IN]        Device type
*  para        [IN]        Pointer
* RETURNS
*  ble_ias_dev_info_t
*****************************************************************************/
static ble_ias_dev_info_t *ble_ias_get_dev_info(
    ble_ias_dev_info_type_t type, void *para)
{
    int32_t i = 0, ret = 0;
    ble_ias_dev_info_t *info = NULL;
    ble_ias_context_t *ias_ctx = NULL;
    bt_handle_t *hd = NULL;

    ias_ctx = ble_ias_get_context();

    ble_gatt_report("[gatt][ias]get_dev_info(s)--type: %d, para: 0x%x\n",
        type, para);

    switch (type) {

        case BLE_IAS_DEV_INFO_HANDLE: {
            hd = (bt_handle_t *) para;
            for (i = 0; i < BLE_GATT_MAX_LINK; ++i) {
                if ((ias_ctx->dev_info[i].flag & BLE_IAS_DEV_FLAG_USED) &&
                    ias_ctx->dev_info[i].handle == *(hd)) {
                    info = &(ias_ctx->dev_info[i]);
                    break;
                }
            }
            break;
        }

        case BLE_IAS_DEV_INFO_UNUSED: {
            for (i = 0; i < BLE_GATT_MAX_LINK; ++i) {
                if (!(ias_ctx->dev_info[i].flag & BLE_IAS_DEV_FLAG_USED)) {
                    info = &(ias_ctx->dev_info[i]);
                    ias_ctx->dev_info[i].flag |= BLE_IAS_DEV_FLAG_USED;
                    break;
                }
            }
            break;
        }
        default:
            break;
    }

    ble_gatt_report("[gatt][ias]get_dev_info(e)--info: 0x%x, ret: %d\n",
        info, ret);

    return info;
}


/*****************************************************************************
* FUNCTION
*  ble_ias_reset_dev_info
* DESCRIPTION
* Reset connection information pointer
* PARAMETERS
*  dev_info        [IN]        Connect_info pointer
* RETURNS
*  void
*****************************************************************************/
static void ble_ias_reset_dev_info(ble_ias_dev_info_t *dev_info)
{
    ble_gatt_memset(dev_info, 0x00, sizeof(ble_ias_dev_info_t));
}


/*****************************************************************************
* FUNCTION
*  ble_ias_init
* DESCRIPTION
* IAS init
* PARAMETERS
*  void
* RETURNS
*  void
*****************************************************************************/
void ble_ias_init(void)
{
    ble_gatt_memset(&g_ias_context, 0x00, sizeof(ble_ias_context_t));
    ble_gatt_memset(&g_ias_dev_info[0], 0x00,
        sizeof(ble_ias_dev_info_t) * BLE_GATT_MAX_LINK);

    g_ias_context.dev_info = g_ias_dev_info;

    ble_gatt_report("[gatt][ias]init\n");
}


/*****************************************************************************
* FUNCTION
*  ble_ias_dev_connect
* DESCRIPTION
* Notify IAS connection
* PARAMETERS
*  handle        [IN] Connection handle
* RETURNS
*  void
*****************************************************************************/
void ble_ias_dev_connect(bt_handle_t handle)
{
    ble_ias_dev_info_t *dev_info = NULL;

    dev_info = ble_ias_get_dev_info(BLE_IAS_DEV_INFO_UNUSED, NULL);
    dev_info->handle = handle;
}


/*****************************************************************************
* FUNCTION
*  ble_ias_dev_disconnect
* DESCRIPTION
* Notify IAS disconnection
* PARAMETERS
*  handle        [IN] Connection handle
* RETURNS
*  void
*****************************************************************************/
void ble_ias_dev_disconnect(bt_handle_t handle)
{
    ble_ias_dev_info_t *dev_info = NULL;

    dev_info = ble_ias_get_dev_info(BLE_IAS_DEV_INFO_HANDLE, (void *)&handle);
    ble_ias_reset_dev_info(dev_info);
}


/*****************************************************************************
* FUNCTION
*  ble_ias_get_context
* DESCRIPTION
* Get IAS context
* PARAMETERS
*  void
* RETURNS
*  ble_ias_context_t
*****************************************************************************/
ble_ias_context_t *ble_ias_get_context(void)
{
    return &g_ias_context;
}

