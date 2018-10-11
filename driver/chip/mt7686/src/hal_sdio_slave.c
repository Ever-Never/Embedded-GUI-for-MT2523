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
#include "hal.h"
#include "hal_sdio_slave.h"
#include "hal_sdio_slave_internal.h"

#ifdef HAL_SDIO_SLAVE_MODULE_ENABLED
#include "hal_log.h"
#include "assert.h"
#include "hal_platform.h"
#include <string.h>

extern sdio_slave_private_t sdio_private;

hal_sdio_slave_status_t hal_sdio_slave_register_callback(hal_sdio_slave_port_t sdio_slave_port, hal_sdio_slave_callback_t sdio_slave_callback, void *user_data)
{
    sdio_private.sdio_property.sdio_slave_callback = sdio_slave_callback;
    sdio_private.sdio_property.sdio_slave_callback_user_data = user_data;

    return HAL_SDIO_SLAVE_STATUS_OK;
}


hal_sdio_slave_status_t hal_sdio_slave_init(hal_sdio_slave_port_t sdio_slave_port)
{
    sdio_slave_init();

    return HAL_SDIO_SLAVE_STATUS_OK;
}



hal_sdio_slave_status_t hal_sdio_slave_read_mailbox(hal_sdio_slave_port_t sdio_slave_port, uint32_t mailbox_number, uint32_t *mailbox_value)
{
    while (true == sdio_slave_check_fw_own());

    sdio_slave_read_mailbox(mailbox_number, mailbox_value);

    return HAL_SDIO_SLAVE_STATUS_OK;
}


hal_sdio_slave_status_t hal_sdio_slave_write_mailbox(hal_sdio_slave_port_t sdio_slave_port, uint32_t mailbox_number, uint32_t mailbox_value)
{
    while (true == sdio_slave_check_fw_own());

    sdio_slave_write_mailbox(mailbox_number, mailbox_value);

    return HAL_SDIO_SLAVE_STATUS_OK;
}


hal_sdio_slave_status_t hal_sdio_slave_trigger_d2h_interrupt(hal_sdio_slave_port_t sdio_slave_port, uint32_t interrupt_number)
{
    while (true == sdio_slave_check_fw_own());

    sdio_slave_set_device_to_host_interrupt(interrupt_number);

    return HAL_SDIO_SLAVE_STATUS_OK;
}



hal_sdio_slave_status_t hal_sdio_slave_send_dma(hal_sdio_slave_port_t sdio_slave_port, hal_sdio_slave_rx_queue_id_t queue_id, const uint32_t *buffer, uint32_t size)
{
    sdio_slave_status_t status;

    while (true == sdio_slave_check_fw_own());

    status = sdio_slave_send((sdio_slave_rx_queue_id_t)queue_id, (uint32_t)buffer, size);
    if (SDIO_SLAVE_STATUS_OK != status) {
        return HAL_SDIO_SLAVE_STATUS_ERROR;
    }

    return HAL_SDIO_SLAVE_STATUS_OK;
}

hal_sdio_slave_status_t hal_sdio_slave_receive_dma(hal_sdio_slave_port_t sdio_slave_port, hal_sdio_slave_tx_queue_id_t queue_id, uint32_t *buffer, uint32_t size)
{
    sdio_slave_status_t status;

    while (true == sdio_slave_check_fw_own());

    status = sdio_slave_receive((sdio_slave_tx_queue_id_t)queue_id, (uint32_t)buffer, size);
    if (SDIO_SLAVE_STATUS_OK != status) {
        return HAL_SDIO_SLAVE_STATUS_ERROR;
    }

    return HAL_SDIO_SLAVE_STATUS_OK;
}

#endif

