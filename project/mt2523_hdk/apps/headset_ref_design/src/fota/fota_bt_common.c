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

#include "bt_sink_srv.h"
#include "hal_flash.h"
#include "bl_fota.h"
#include "memory_map.h"

LOG_CONTROL_BLOCK_DECLARE(fota_dl_m);

#include "bt_spp.h"

static bt_gap_le_smp_pairing_config_t pairing_config = {//mitm, bond, oob
    .auth_req = BT_GAP_LE_SMP_AUTH_REQ_BONDING,        
    .maximum_encryption_key_size = 16,
};


static bt_gap_le_local_key_t local_key = {
    .encryption_info = {{0}},
    .master_id = {0},
    .identity_info = {{0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x19, 0x28, 0x55, 0x33, 0x68, 0x33, 0x56, 0xde}},
    .signing_info = {{0}}
};

static bt_gap_le_bonding_info_t  bonding_info;
static bt_gap_le_local_config_req_ind_t local_config;

/**
 * @brief   This function is a static callback for the application to listen to the event. Provide a user-defined callback.
 * @param[in] msg     is the callback message type.
 * @param[in] status  is the status of the callback message.
 * @param[in] buf     is the payload of the callback message.
 * @return            The status of this operation returned from the callback.
 */

extern bt_sink_srv_status_t bt_sink_srv_action_send(bt_sink_srv_action_t action, void *parameters);
#define UPDATE_INFO_SIZE    (sizeof(bl_fota_update_info_t))
bt_status_t fota_bt_app_event_callback(
    bt_msg_type_t msg, 
    bt_status_t status, 
    void *buff)

{
    LOG_I(fota_dl_m, "[FOTA] event:0x%x, %x\n", msg, status);
    switch (msg) {
        case BT_POWER_ON_CNF:  /*24000001*/
            LOG_I(fota_dl_m, "[FOTA] bt power on confirm.\n");
            uint8_t update_info[UPDATE_INFO_SIZE];
            hal_flash_status_t fret;
            bool updated = false;
            if ((fret = hal_flash_read(FOTA_RESERVED_BASE + FOTA_RESERVED_LENGTH - BL_FOTA_UPDATE_INFO_RESERVE_SIZE - BL_BASE,
                       update_info, sizeof(bl_fota_update_info_t))) != HAL_FLASH_STATUS_OK) {
                LOG_E(fota_dl_m, "fail to read flash, fret: %d\r\n", fret);
                break;
            }
            
            for (int i = 0; i < UPDATE_INFO_SIZE; i++) {
                if (update_info[i] != 0xFF) {
                    updated = true;
                    break;
                }
            }

            /* if bootup after fota updating, turn on BT discoverable */
            if (updated) {
                LOG_I(fota_dl_m, "FOTA turn on BT discoverable\r\n");
                bt_sink_srv_action_send(BT_SINK_SRV_ACTION_DISCOVERABLE, NULL);
            }
            break;      

        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}

/**
 * @brief     This API invoked by the SDK process should be implemented by the application. The application should return the Local Configuration field.
 * @return    The loacl configuration pointer, please set the local key and secure connection mode flag. The pointer should not be NULL and it must be a global variable.
 */
bt_gap_le_local_config_req_ind_t *fota_bt_gap_le_get_local_config(void)
{
    local_config.local_key_req = &local_key;
    local_config.sc_only_mode_req = false;
 
    return &local_config;
}

/**
 * @brief   This API invoked by the SDK process should be implemented by the application. The application should return the Bonding Information field.
 * @param[in] remote_addr The address of the remote device to be bonded.
 * @return                The Bonding Information pointer, please set a pointer to the connection bonding information. The pointer should not be NULL and it must be a global variable.
 */
bt_gap_le_bonding_info_t *fota_bt_gap_le_get_bonding_info(const bt_addr_t remote_addr)
{
    return &bonding_info;
}

/**
 * @brief   This API invoked by the SDK process should be implemented by the application. The application should return the Pairing Configuration field.
 * @param[in] ind         Bonding start indication structure. Application should set the pairing_config_req variable to a global variable.
 * @return    #BT_STATUS_SUCCESS, the application set the pairing configuration successfully.
 *            #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t fota_bt_gap_le_get_pairing_config(bt_gap_le_bonding_start_ind_t *ind)
{
    ind->pairing_config_req = pairing_config;

    return BT_STATUS_SUCCESS;
}



#define DEVICE_NAME "fota_download"
bt_gap_config_t fota_bt_custom_config;

const bt_gap_config_t* fota_bt_gap_get_local_configuration(void)
{
    

    LOG_I(fota_dl_m, "bt_get_local_configuration1\n");
    fota_bt_custom_config.inquiry_mode = 3;
    fota_bt_custom_config.io_capability = BT_GAP_IO_CAPABILITY_NO_INPUT_NO_OUTPUT;
    fota_bt_custom_config.cod = 0x240404;
    memcpy(&fota_bt_custom_config.device_name, DEVICE_NAME, sizeof(DEVICE_NAME));

    
    LOG_I(fota_dl_m,"1:%08x\r\n", (unsigned int)DEVICE_NAME);
    return  &fota_bt_custom_config;
}

