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
#include "task.h"
#include "task_def.h"
#include "nvdm.h"
#include "bt_system.h"
#include "bt_gap.h"
#include "bt_gap_le.h"
#include "hal_trng.h"
#include "project_config.h"

bt_gap_le_local_config_req_ind_t *bt_gap_le_get_local_config(void)
{
    return NULL;
}

bt_gap_le_bonding_info_t *bt_gap_le_get_bonding_info(const bt_addr_t remote_addr)
{
    return NULL;
}

bt_status_t bt_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind)
{
    return BT_STATUS_FAIL;
}


bt_gap_config_t spp_gap_config = {
        .inquiry_mode  = 2, /**< It indicates the inquiry result format.
                                                            0: Standerd inquiry result format (Default).
                                                            1: Inquiry result format with RSSI.
                                                            2: Inquiry result with RSSI format or Extended Inquiry Result(EIR) format. */
        .io_capability = BT_GAP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT,
        .cod           = 0x240404, /* It defines the class of the local device. */
        .device_name   = {"bt_spp_example"}, /* It defines the name of the local device with '\0' ending. */
    };

const bt_gap_config_t* bt_gap_get_local_configuration(void)
{
    return &spp_gap_config;
}

BT_ALIGNMENT4(
static char timer_cb_buf[BT_TIMER_BUF_SIZE]//one timer control block is 20 bytes
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
static char bt_spp_connection_cb_buf[BT_SPP_CONNECTION_BUF_SIZE]
);


bt_bd_addr_t local_public_addr;
void bt_task(void * arg);

static void bt_preread_local_address(bt_bd_addr_t addr)
{
    nvdm_status_t status;
    int8_t i;
    uint32_t random_seed;
    uint32_t size = 12;
    uint8_t buffer[18] = {0};
    uint8_t tmp_buf[3] = {0};
    bt_bd_addr_t tempaddr = {0};
    hal_trng_status_t ret = HAL_TRNG_STATUS_OK;
    if (memcmp(addr, &tempaddr, sizeof(bt_bd_addr_t)) == 0) {
        LOG_I(bt_spp_example, "[BT]Empty bt address after bt_gap_le_get_local_address()\n");
        LOG_I(bt_spp_example, "[BT]Try to read from NVDM.");
        status = nvdm_read_data_item("BT", "address", buffer, &size);
        if (NVDM_STATUS_OK == status) {
            LOG_I(bt_spp_example, "[BT]Read from NVDM:%s", buffer);
            for (i = 0; i < 6; ++i) {
                tmp_buf[0] = buffer[2 * i];
                tmp_buf[1] = buffer[2 * i + 1];
                addr[i] = (uint8_t)strtoul((char *)tmp_buf, NULL, 16);
            }

            LOG_I(bt_spp_example, "[BT]Read address from NVDM [%02X:%02X:%02X:%02X:%02X:%02X]", addr[0],
                  addr[1], addr[2], addr[3], addr[4], addr[5]);
            return;
        } else {
            LOG_I(bt_spp_example, "[BT]Failed to Read from NVDM:%d.", status);
            ret = hal_trng_init();
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(bt_spp_example, "[BT]generate_random_address--error 1");
            }
            for (i = 0; i < 30; ++i) {
                ret = hal_trng_get_generated_random_number(&random_seed);
                if (HAL_TRNG_STATUS_OK != ret) {
                    LOG_I(bt_spp_example, "[BT]generate_random_address--error 2");
                }
                LOG_I(bt_spp_example, "[BT]generate_random_address--trn: 0x%x", random_seed);
            }
            /* randomly generate address */
            ret = hal_trng_get_generated_random_number(&random_seed);
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(bt_spp_example, "[BT]generate_random_address--error 3");
            }
            LOG_I(bt_spp_example, "[BT]generate_random_address--trn: 0x%x", random_seed);
            addr[0] = random_seed & 0xFF;
            addr[1] = (random_seed >> 8) & 0xFF;
            addr[2] = (random_seed >> 16) & 0xFF;
            addr[3] = (random_seed >> 24) & 0xFF;
            ret = hal_trng_get_generated_random_number(&random_seed);
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_I(bt_spp_example, "[BT]generate_random_address--error 3");
            }
            LOG_I(bt_spp_example, "[BT]generate_random_address--trn: 0x%x", random_seed);
            addr[4] = random_seed & 0xFF;
            addr[5] = (random_seed >> 8) & 0xCF;
            hal_trng_deinit();
        }
    }
    /* save address to NVDM */
    for (i = 0; i < 6; ++i) {
        sprintf((char *)buffer + 2 * i, "%02X", addr[i]);
    }
    LOG_I(bt_spp_example, "[BT]address to write:%s len:%d", buffer, strlen((char *)buffer));
    status = nvdm_write_data_item("BT", "address", NVDM_DATA_ITEM_TYPE_STRING, buffer, strlen((char *)buffer));
    if (NVDM_STATUS_OK != status) {
        LOG_I(bt_spp_example, "[BT]Failed to store address.");
    } else {
        LOG_I(bt_spp_example, "[BT]Successfully store address to NVDM [%02X:%02X:%02X:%02X:%02X:%02X]", addr[0],
              addr[1], addr[2], addr[3], addr[4], addr[5]);
    }
}

void bt_mm_init()
{
    bt_memory_init_packet(BT_MEMORY_TX_BUFFER, tx_buf, BT_TX_BUF_SIZE);
    bt_memory_init_packet(BT_MEMORY_RX_BUFFER, rx_buf, BT_RX_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_TIMER, timer_cb_buf, BT_TIMER_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_EDR_CONNECTION, bt_connection_cb_buf, BT_CONNECTION_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_RFCOMM, bt_rfcomm_link_cb_buf, BT_RFCOMM_LINK_BUF_SIZE);
    bt_memory_init_control_block(BT_MEMORY_CONTROL_BLOCK_SPP, bt_spp_connection_cb_buf, BT_SPP_CONNECTION_BUF_SIZE);
}

void bt_create_task(void)
{
    bt_mm_init();
    log_config_print_switch(BTHCI, DEBUG_LOG_OFF);
    log_config_print_switch(BTMM, DEBUG_LOG_OFF);
    log_config_print_switch(BTRFCOMM, DEBUG_LOG_OFF);
    log_config_print_switch(BTL2CAP, DEBUG_LOG_OFF);
    log_config_print_switch(BT, DEBUG_LOG_OFF);
    bt_preread_local_address(local_public_addr);
    LOG_I(bt_spp_example, "[BT]local_public_addr [%02X:%02X:%02X:%02X:%02X:%02X]\n", local_public_addr[5],
          local_public_addr[4], local_public_addr[3], local_public_addr[2], local_public_addr[1], local_public_addr[0]);

    if (pdPASS != xTaskCreate(bt_task, BT_TASK_NAME, BT_TASK_STACKSIZE/(sizeof(StackType_t)), (void *)local_public_addr, BT_TASK_PRIORITY, NULL)) {
        LOG_E(bt_spp_example, "cannot create bt_task.");
    }
}

