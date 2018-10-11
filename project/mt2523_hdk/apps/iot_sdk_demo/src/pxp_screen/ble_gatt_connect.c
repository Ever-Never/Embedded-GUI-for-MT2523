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

#include "ble_gatt_utils.h"
#include "ble_gatt_connect.h"
#include "stdbool.h"
#include "ble_lls.h"
#include "ble_gatt_common.h"
#include "ble_gatt_srv.h"
#include "ble_gatt_srv_internal.h"

static ble_gatt_connect_context_t g_ble_gatt_connect_ctx = {0};

static ble_gatt_connect_info_t g_ble_gatt_connect_info[BLE_GATT_MAX_LINK] = {{0}};

static ble_gatt_bonded_info_t g_ble_gatt_bounded_info[BLE_GATT_MAX_LINK];

static const bt_addr_t default_ble_addr = {
    .type = 0,
    .addr = {0}
};

static bt_gap_le_local_config_req_ind_t g_ble_gatt_le_local_config = {0};

static bt_gap_le_local_key_t ble_gatt_local_key_req = {
    .encryption_info.ltk = { 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc8, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf },
    .master_id.ediv = 0x1005,
    .master_id.rand = { 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7 },
    .identity_info.irk = { 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf },
    .signing_info.csrk = { 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf }
};

static bt_gap_le_smp_pairing_config_t ble_gatt_pairing_config_req = {//mitm, bond, oob
    .maximum_encryption_key_size = 16,
    .io_capability = BT_GAP_LE_SMP_NO_INPUT_NO_OUTPUT,
    .auth_req = BT_GAP_LE_SMP_AUTH_REQ_BONDING,
    .oob_data_flag = BT_GAP_LE_SMP_OOB_DATA_NOT_PRESENTED,
    .initiator_key_distribution = BT_GAP_LE_SMP_KEY_DISTRIBUTE_ENCKEY | BT_GAP_LE_SMP_KEY_DISTRIBUTE_IDKEY | BT_GAP_LE_SMP_KEY_DISTRIBUTE_SIGN,
    .responder_key_distribution = BT_GAP_LE_SMP_KEY_DISTRIBUTE_ENCKEY | BT_GAP_LE_SMP_KEY_DISTRIBUTE_IDKEY | BT_GAP_LE_SMP_KEY_DISTRIBUTE_SIGN,
};

static bool ble_gatt_sc_only = false;

static ble_gatt_connect_info_t *ble_gatt_connect_get_info(
    ble_gatt_connect_info_type_t type, void *para);

static void ble_gatt_connect_reset_info(ble_gatt_connect_info_t *info);

static bt_status_t ble_gatt_connect_gap_power_on_cnf(bt_status_t status,
    void *buff);

static bt_status_t ble_gatt_connect_le_set_adv_cnf(bt_status_t status,
    void *buff);

static bt_status_t ble_gatt_connect_le_adv_report_ind(bt_status_t status,
    void *buff);

static bt_status_t ble_gatt_connect_le_connect_ind(bt_status_t status,
    void *buff);

static bt_status_t bt_gatt_connect_start_advertising(char *device_name);

static bt_status_t bt_gatt_connect_stop_advertising(void);

static ble_gatt_connect_info_t *ble_gatt_connect_get_info(
    ble_gatt_connect_info_type_t type, void *para)
{
    int32_t i = 0, ret = 0;
    ble_gatt_connect_info_t *info = NULL;
    ble_gatt_connect_context_t *conn_ctx = NULL;
    bt_handle_t *hd = NULL;

    conn_ctx = ble_gatt_connect_get_context();

    ble_gatt_report("[gatt][connect]get_info(s)--type: %d, para: 0x%x\n",
        type, para);

    switch (type) {
        case BLE_GATT_CONNECT_INFO_HANDLE: {
            hd = (bt_handle_t *) para;
            for (i = 0; i < BLE_GATT_MAX_LINK; ++i) {
                if (conn_ctx->conn_info[i].handle == *(hd)) {
                    info = &(conn_ctx->conn_info[i]);
                    break;
                }
            }
            break;
        }

        case BLE_GATT_CONNECT_INFO_UNUSED: {
            for (i = 0; i < BLE_GATT_MAX_LINK; ++i) {
                if (conn_ctx->conn_info[i].flag & BLE_GATT_CONNECT_INFO_FLAG_USED) {
                    ;
                } else {
                    info = &(conn_ctx->conn_info[i]);
                    info->flag |= BLE_GATT_CONNECT_INFO_FLAG_USED;
                    break;
                }
            }

            break;
        }
    }

    ble_gatt_report("[gatt][connect]get_info(e)--info: 0x%x, ret: %d\n",
        info, ret);

    return info;
}


static void ble_gatt_connect_reset_info(ble_gatt_connect_info_t *info)
{
    ble_gatt_memset(info, 0x00, sizeof(ble_gatt_connect_info_t));
}


static ble_gatt_bonded_info_t *ble_gatt_get_bonded_info(const bt_addr_t *addr)
{
    ble_gatt_bonded_info_t *bonded = NULL;
    int32_t err = BLE_GATT_ERR_FAIL_1ST;
    int32_t i = 0;

    ble_gatt_assert(addr);

    for (i = 0; i < BLE_GATT_MAX_LINK; ++i) {
        if (0 == ble_gatt_memcmp(addr, &(g_ble_gatt_bounded_info[i].bt_addr), sizeof(default_ble_addr))) {
            bonded = &(g_ble_gatt_bounded_info[i]);
            err = BLE_GATT_ERR_SUCCESS_OK;
            break;
        }
    }

    /* get empty bonded */
    for (i = 0; i < BLE_GATT_MAX_LINK ; ++i) {
        if (0 == ble_gatt_memcmp(&default_ble_addr, &(g_ble_gatt_bounded_info[i].bt_addr), sizeof(default_ble_addr))) {
            g_ble_gatt_bounded_info[i].info.identity_addr.address.type = BT_ADDR_TYPE_UNKNOW;
            memcpy(&(g_ble_gatt_bounded_info[i].bt_addr), addr, sizeof(default_ble_addr));
            bonded = &(g_ble_gatt_bounded_info[i]);
            err = BLE_GATT_ERR_SUCCESS_1ST + i;
            break;
        }
    }

    ble_gatt_report("[gatt][connect]get_bonded--err: %d, bonded: 0x%x, type: %d, add: %s\n",
        err, bonded, addr->type, ble_debug_bd_addr2str(addr->addr));

    return bonded;
}


static bt_status_t ble_gatt_connect_gap_power_on_cnf(bt_status_t status, void *buff)
{
    bt_status_t ret = BT_STATUS_SUCCESS;

    bt_gatt_connect_start_advertising(BLE_GATT_DEV_NAME);

    ble_gatt_report("[gatt][connect]power_on--ret: %d\n", ret);

    return ret;
}


static bt_status_t ble_gatt_connect_le_set_adv_cnf(bt_status_t status, void *buff)
{
    bt_status_t ret = BT_STATUS_SUCCESS;


    ble_gatt_report("[gatt][connect]le_set_adv--ret: %d\n", ret);

    return ret;
}


static bt_status_t ble_gatt_connect_le_adv_report_ind(bt_status_t status, void *buff)
{
    bt_status_t ret = BT_STATUS_SUCCESS;


    ble_gatt_report("[gatt][connect]le_adv_report_ind--ret: %d\n", ret);

    return ret;
}


static bt_status_t ble_gatt_connect_le_connect_ind(bt_status_t status, void *buff)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    int32_t err = 0;
    bt_gap_le_connection_ind_t *connect_ind = NULL;
    ble_gatt_connect_info_t *info = NULL;
    ble_gatt_srv_connect_t connect_info = {0};

    connect_ind = (bt_gap_le_connection_ind_t *)buff;

    ble_gatt_report("[gatt][connect]le_connect_ind(s)--hd: %d, role: %d, type: %d, addr: %s\n",
        connect_ind->connection_handle, connect_ind->role,
        connect_ind->peer_addr.type,
        ble_debug_bd_addr2str(connect_ind->peer_addr.addr));

    if (status == BT_STATUS_SUCCESS) {
        info = ble_gatt_connect_get_info(BLE_GATT_CONNECT_INFO_UNUSED, NULL);

        if (info) {
            info->handle = connect_ind->connection_handle;
            info->role = connect_ind->role;
            ble_gatt_memcpy(&(info->peer_addr), &(connect_ind->peer_addr),
                sizeof(bt_addr_t));

            ble_lls_dev_connect(info->handle);
            bt_gatt_connect_stop_advertising();
            /* notify app */
            connect_info.handle = info->handle;
            ble_gatt_srv_notify(BLE_GATT_SRV_LE_CONNECT, (void *)&connect_info);
        }
    }

    ble_gatt_report("[gatt][connect]le_connect_ind(e)--ret: %d, err: %d\n",
        ret, err);
    return ret;
}


static bt_status_t ble_gatt_connect_le_disconnect_ind(bt_status_t status, void *buff)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    int32_t err = BLE_GATT_ERR_SUCCESS_OK;
    bt_gap_le_disconnect_ind_t *disconn_ind = NULL;
    ble_gatt_connect_info_t *info = NULL;
    ble_gatt_srv_disconnect_t discon_info = {0};


    disconn_ind = (bt_gap_le_disconnect_ind_t *)buff;
    info = ble_gatt_connect_get_info(BLE_GATT_CONNECT_INFO_HANDLE,
        (void *)&(disconn_ind->connection_handle));

    if (info) {
        ble_lls_dev_disconnect(disconn_ind->connection_handle);
        /* notify app */
        discon_info.handle = info->handle;
        ble_gatt_srv_notify(BLE_GATT_SRV_LE_DISCONNECT, (void *)&discon_info);
        ble_gatt_connect_reset_info(info);
        err = BLE_GATT_ERR_SUCCESS_1ST;
    }

    ble_gatt_report("[gatt][connect]le_disconnect_ind--ret: %d, err: %d\n",
        ret, err);

    return ret;
}


static bt_status_t ble_gatt_connect_le_set_scan_cnf(bt_status_t status, void *buff)
{
    bt_status_t ret = BT_STATUS_SUCCESS;


    ble_gatt_report("[gatt][connect]le_set_scan_cnf--ret: %d\n", ret);

    return ret;
}


static bt_status_t bt_gatt_connect_start_advertising(char *device_name)
{
    uint8_t total_length;
    bt_hci_cmd_le_set_advertising_enable_t adv_enable = {0};
    bt_hci_cmd_le_set_advertising_data_t adv_data;
    bt_hci_cmd_le_set_advertising_parameters_t adv_para = {
        .advertising_interval_min = 0x0200,
        .advertising_interval_max = 0x0200,
        .advertising_type = BT_HCI_ADV_TYPE_CONNECTABLE_UNDIRECTED,
        .advertising_channel_map = 7,
        .own_address_type = 1,
        .advertising_filter_policy = 0
    };

    ble_gatt_memset(&adv_data, 0, sizeof(adv_data));

    *adv_data.advertising_data = strlen(device_name) + 1;
    *(adv_data.advertising_data + 1) = BT_GAP_LE_AD_TYPE_NAME_COMPLETE;
    memcpy(adv_data.advertising_data + 2, device_name, strlen(device_name));
    total_length = strlen(device_name) + 2; // 1 byte for length and 1 byte for adv type

    *(adv_data.advertising_data + total_length) = 3;
    *(adv_data.advertising_data + total_length + 1) = BT_GAP_LE_AD_TYPE_16_BIT_UUID_COMPLETE;
    *((uint16_t *)(adv_data.advertising_data + total_length + 2)) = BT_GATT_UUID16_GAP_SERVICE;
    total_length += 4; // 1 type for length, 1 type for adv type, 2 type for uuid

    *(adv_data.advertising_data + total_length) = 3;
    *(adv_data.advertising_data + total_length + 1) = BT_GAP_LE_AD_TYPE_16_BIT_UUID_COMPLETE;
    *((uint16_t *)(adv_data.advertising_data + total_length + 2)) = BT_GATT_UUID16_GATT_SERVICE;
    total_length += 4; // 1 type for length, 1 type for adv type, 2 type for uuid

    *(adv_data.advertising_data + total_length) = 3;
    *(adv_data.advertising_data + total_length + 1) = BT_GAP_LE_AD_TYPE_16_BIT_UUID_COMPLETE;
    *((uint16_t *)(adv_data.advertising_data + total_length + 2)) = BT_GATT_UUID16_LINK_LOSS_SERVICE;
    total_length += 4; // 1 type for length, 1 type for adv type, 2 type for uuid

    adv_data.advertising_data_length = total_length;
    adv_enable.advertising_enable = BT_HCI_ENABLE;

    return bt_gap_le_set_advertising(&adv_enable, &adv_para, &adv_data, NULL);
}


static bt_status_t bt_gatt_connect_stop_advertising(void)
{
    bt_hci_cmd_le_set_advertising_enable_t adv_enable = {
        .advertising_enable = BT_HCI_DISABLE
    };
    return bt_gap_le_set_advertising(&adv_enable, NULL, NULL, NULL);
}


bt_gap_le_local_config_req_ind_t *ble_gatt_connect_gap_le_get_local_config(void)
{
    g_ble_gatt_le_local_config.local_key_req = &ble_gatt_local_key_req;
    g_ble_gatt_le_local_config.sc_only_mode_req = ble_gatt_sc_only;

    return &g_ble_gatt_le_local_config;
}


bt_gap_le_bonding_info_t *ble_gatt_connect_gap_le_get_bonding_info(const bt_addr_t remote_addr)
{
    int32_t err = BLE_GATT_ERR_FAIL_1ST;
    ble_gatt_bonded_info_t *bonded_info = NULL;
    bt_gap_le_bonding_info_t *bonding_info = NULL;

    bonded_info = ble_gatt_get_bonded_info(&(remote_addr));
    if (bonded_info) {
        bonding_info = &(bonded_info->info);
        err = BLE_GATT_ERR_SUCCESS_1ST;
    }

    ble_gatt_report("[gatt][connect]le_bonding_info_req_ind--bonding: 0x%x, err: %d\n",
        bonding_info, err);

    return bonding_info;

}


bt_status_t bt_gatt_connect_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind)
{
    bt_status_t ret = BT_STATUS_SUCCESS;

    ind->pairing_config_req = ble_gatt_pairing_config_req;

    return ret;
}


bt_status_t ble_gap_event_handle(bt_msg_type_t msg, bt_status_t status,
    void *buff)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    int32_t err_1st = BLE_GATT_ERR_FAIL_1ST;

    ble_gatt_report("[gatt][connect]ent_hdr(s)--msg: 0x%x, status: %d, buff: 0x%x",
        msg, status, buff);

    switch (msg) {
         case BT_POWER_ON_CNF:
            ret = ble_gatt_connect_gap_power_on_cnf(status, buff);
            break;
        case BT_GAP_LE_SET_ADVERTISING_CNF:
            ret = ble_gatt_connect_le_set_adv_cnf(status, buff);
            break;
        case BT_GAP_LE_ADVERTISING_REPORT_IND:
            ret = ble_gatt_connect_le_adv_report_ind(status, buff);
            break;
        case BT_GAP_LE_CONNECT_IND:
            ret = ble_gatt_connect_le_connect_ind(status, buff);
            break;
        case BT_GAP_LE_DISCONNECT_IND:
            ret = ble_gatt_connect_le_disconnect_ind(status, buff);
            break;
        case BT_GAP_LE_SET_SCAN_CNF:
            ret = ble_gatt_connect_le_set_scan_cnf(status, buff);
            break;
        default:
            break;
    }
 
    ble_gatt_report("[gatt][connect]ent_hdr(e)--ret: %d, err_1st: %d\n",
        ret, err_1st);
    return ret;
}


void ble_gatt_connect_init(void)
{
    ble_gatt_memset(&g_ble_gatt_connect_ctx, 0x00,
        sizeof(ble_gatt_connect_context_t));
    ble_gatt_memset(&g_ble_gatt_connect_info[0], 0x00,
        sizeof(ble_gatt_connect_info_t) * BLE_GATT_MAX_LINK);
    ble_gatt_memset(&g_ble_gatt_bounded_info[0], 0x00,
        sizeof(ble_gatt_bonded_info_t) * BLE_GATT_MAX_LINK);

    g_ble_gatt_connect_ctx.flag |= BLE_GATT_CONNECT_FLAG_INIT;
    g_ble_gatt_connect_ctx.conn_info = g_ble_gatt_connect_info;

    ble_gatt_report("[gatt][connect]init");
}


ble_gatt_connect_context_t *ble_gatt_connect_get_context(void)
{
    return &g_ble_gatt_connect_ctx;
}

