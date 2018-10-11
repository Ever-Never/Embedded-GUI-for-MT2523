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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "nvdm.h"
#include "hal_trng.h"
#include "bt_system.h"
#include "bt_type.h"
#include "bt_hfp.h"
#include "bt_a2dp.h"
#include "bt_init.h"
#include "bt_platform.h"
#include "bt_gap_le.h"
#include "bt_gap.h"
//#include "bt_sink_app_event.h"
#include "bt_callback_manager.h"
#include "bt_sink_srv.h"

BT_ALIGNMENT4(
    static char timer_cb_buf[BT_TIMER_BUF_SIZE]//one timer control block is 20 bytes
);
BT_ALIGNMENT4(
    static char le_connection_cb_buf[BT_LE_CONNECTION_BUF_SIZE]
);

BT_ALIGNMENT4(
    static char bt_connection_cb_buf[BT_CONNECTION_BUF_SIZE]
);

BT_ALIGNMENT4(
    static char tx_buf[BT_TX_BUF_SIZE]
);

BT_ALIGNMENT4(
    static char rx_buf[BT_RX_BUF_SIZE]
);

BT_ALIGNMENT4(
    static char bt_rfcomm_link_cb_buf[BT_RFCOMM_LINK_BUF_SIZE]
);
BT_ALIGNMENT4(
    static char bt_hfp_link_cb_buf[BT_HFP_LINK_BUF_SIZE]
);
BT_ALIGNMENT4(
    static char bt_avrcp_link_cb_buf[BT_AVRCP_LINK_BUF_SIZE]
);

BT_ALIGNMENT4(
    static char bt_a2dp_sep_cb_buf[BT_A2DP_SEP_BUF_SIZE]
);

BT_ALIGNMENT4(
    static char bt_a2dp_link_cb_buf[BT_A2DP_LINK_BUF_SIZE]
);

BT_ALIGNMENT4(
    static char bt_spp_connection_cb_buf[BT_SPP_CONNECTION_BUF_SIZE]
);

BT_ALIGNMENT4(
    static char bt_pbapc_connection_cb_buf[BT_PBAPC_CONNECTION_BUF_SIZE]
);


bt_bd_addr_t   bt_local_public_addr;

static bool bt_demo_hci_log_status = true;

static void bt_demo_generate_local_address(bt_bd_addr_t addr)
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
        LOG_I(common, "[BT]Empty bt address after bt_gap_le_get_local_address()\n");
        LOG_I(common, "[BT]Try to read from NVDM.\n");
        status = nvdm_read_data_item("BT", "address", buffer, &size);
        if (NVDM_STATUS_OK == status) {
            LOG_I(common, "[BT]Read from NVDM:%s\n", buffer);
            for (i = 0; i < 6; ++i) {
                tmp_buf[0] = buffer[2 * i];
                tmp_buf[1] = buffer[2 * i + 1];
                addr[i] = (uint8_t)strtoul((char *)tmp_buf, NULL, 16);
            }

            LOG_I(common, "[BT]Read address from NVDM [%02X:%02X:%02X:%02X:%02X:%02X]\n", addr[0],
                  addr[1], addr[2], addr[3], addr[4], addr[5]);
            return;
        } else {
            LOG_I(common, "[BT]Failed to Read from NVDM:%d.\n", status);
            ret = hal_trng_init();
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(common, "[BT]generate_random_address--error 1");
            }
            for (i = 0; i < 30; ++i) {
                ret = hal_trng_get_generated_random_number(&random_seed);
                if (HAL_TRNG_STATUS_OK != ret) {
                    LOG_I(common, "[BT]generate_random_address--error 2");
                }
                LOG_I(common, "[BT]generate_random_address--trn: 0x%x", random_seed);
            }
            /* randomly generate address */
            ret = hal_trng_get_generated_random_number(&random_seed);
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(common, "[BT]generate_random_address--error 3");
            }
            LOG_I(common, "[BT]generate_random_address--trn: 0x%x", random_seed);
            addr[0] = random_seed & 0xFF;
            addr[1] = (random_seed >> 8) & 0xFF;
            addr[2] = (random_seed >> 16) & 0xFF;
            addr[3] = (random_seed >> 24) & 0xFF;
            ret = hal_trng_get_generated_random_number(&random_seed);
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(common, "[BT]generate_random_address--error 3");
            }
            LOG_I(common, "[BT]generate_random_address--trn: 0x%x", random_seed);
            addr[4] = random_seed & 0xFF;
            addr[5] = (random_seed >> 8) & 0xCF;
            hal_trng_deinit();
        }
    }
    /* save address to NVDM */
    for (i = 0; i < 6; ++i) {
        sprintf((char *)buffer + 2 * i, "%02X", addr[i]);
    }
    LOG_I(common, "[BT]address to write:%s len:%d\n", buffer, strlen((char *)buffer));
    status = nvdm_write_data_item("BT", "address", NVDM_DATA_ITEM_TYPE_STRING, buffer, strlen((char *)buffer));
    if (NVDM_STATUS_OK != status) {
        LOG_I(common, "[BT]Failed to store address.\n");
    } else {
        LOG_I(common, "[BT]Successfully store address to NVDM [%02X:%02X:%02X:%02X:%02X:%02X]\n", addr[0],
              addr[1], addr[2], addr[3], addr[4], addr[5]);
    }
}

void bt_mm_init()
{
    bt_memory_init_packet(BT_MEMORY_TX_BUFFER, tx_buf, BT_TX_BUF_SIZE);
    bt_memory_init_packet(BT_MEMORY_RX_BUFFER, rx_buf, BT_RX_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_TIMER, timer_cb_buf, BT_TIMER_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_LE_CONNECTION, le_connection_cb_buf, BT_LE_CONNECTION_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_EDR_CONNECTION, bt_connection_cb_buf, BT_CONNECTION_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_RFCOMM, bt_rfcomm_link_cb_buf, BT_RFCOMM_LINK_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_HFP, bt_hfp_link_cb_buf, BT_HFP_LINK_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_AVRCP, bt_avrcp_link_cb_buf, BT_AVRCP_LINK_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_A2DP_SEP, bt_a2dp_sep_cb_buf, BT_A2DP_SEP_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_A2DP, bt_a2dp_link_cb_buf, BT_A2DP_LINK_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_SPP, bt_spp_connection_cb_buf, BT_SPP_CONNECTION_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_PBAPC, bt_pbapc_connection_cb_buf, BT_PBAPC_CONNECTION_BUF_SIZE);
    
}

bt_gap_le_local_key_t bt_sink_local_key_req = {
    .encryption_info = {{0}},
    .master_id = {0},
    .identity_info = {{0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x19, 0x28, 0x55, 0x33, 0x68, 0x33, 0x56, 0xde}},
    .signing_info = {{0}}
};

static bt_gap_le_local_config_req_ind_t bt_sink_le_local_config;

bt_gap_le_local_config_req_ind_t *bt_sink_app_get_le_local_configuration(void)
{
    bt_sink_le_local_config.local_key_req = &bt_sink_local_key_req;
    bt_sink_le_local_config.sc_only_mode_req = false;
    return &bt_sink_le_local_config;
}

static bt_gap_le_bonding_info_t bt_sink_le_bonding_info;

bt_gap_le_bonding_info_t *bt_sink_app_gap_le_get_bonding_info(const bt_addr_t remote_addr)
{
    return &bt_sink_le_bonding_info;
}

bt_status_t bt_sink_app_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind)
{
    return BT_STATUS_SUCCESS;
}

static void bt_demo_sink_callback_init(void)
{
    bt_callback_manager_register_callback(bt_callback_type_app_event,
                                          (uint32_t)(MODULE_MASK_GAP | MODULE_MASK_SYSTEM | MODULE_MASK_HFP |
                                                  MODULE_MASK_AVRCP | MODULE_MASK_A2DP | MODULE_MASK_PBAPC |
                                                  MODULE_MASK_SPP),
                                          (void *)bt_sink_srv_common_callback);
    bt_callback_manager_register_callback(bt_callback_type_gap_get_local_configuration,
                                          0,
                                          (void *)bt_sink_srv_gap_config);
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_local_cofig,
                                          0,
                                          (void *)bt_sink_app_get_le_local_configuration);
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_pairing_config,
                                          0,
                                          (void *)bt_sink_app_gap_le_get_pairing_config);
    bt_callback_manager_register_callback(bt_callback_type_gap_le_get_bonding_info,
                                          0,
                                          (void *)bt_sink_app_gap_le_get_bonding_info);
    bt_callback_manager_register_callback(bt_callback_type_hfp_get_init_params,
                                          0,
                                          (void *)bt_sink_srv_hf_get_init_params);
    bt_callback_manager_register_callback(bt_callback_type_a2dp_get_init_params,
                                          0,
                                          (void *)bt_sink_srv_a2dp_get_init_params);
}

void bt_demo_init(void)
{
    bt_mm_init();
    bt_demo_generate_local_address(bt_local_public_addr);
    bt_demo_sink_callback_init();
}

bt_status_t bt_demo_power_on(void)
{
    return bt_power_on((bt_bd_addr_ptr_t)&bt_local_public_addr, NULL);
}

bt_status_t bt_demo_power_off(void)
{
    return bt_power_off();
}

void bt_demo_hci_log_switch(bool on_off)
{
    bt_demo_hci_log_status = on_off;
}

void bt_demo_syslog_switch(bool on_off)
{
    log_switch_t syslog_switch;

    if (on_off) {
        syslog_switch = DEBUG_LOG_ON;
    } else {
        syslog_switch = DEBUG_LOG_OFF;
    }
    log_config_print_switch(BTHCI, syslog_switch);
    log_config_print_switch(BTL2CAP, syslog_switch);
    log_config_print_switch(BTRFCOMM, syslog_switch);
    log_config_print_switch(BTSPP, syslog_switch);
    log_config_print_switch(BT, syslog_switch);
}

bool bt_hci_log_enabled(void)
{
    return bt_demo_hci_log_status;
}

