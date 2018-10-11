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

#include "hal_gdma.h"

#ifdef HAL_GDMA_MODULE_ENABLED
#include <string.h>
#include "dma_sw.h"
#include "dma_hw.h"
#include "hal_log.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_driver.h"


typedef struct {
    hal_gdma_callback_t callback;
    void *user_data;
} hal_gdma_callback_context;

static hal_gdma_callback_context g_gdma_callback_context[DMA_MAX_FULL_CHANNEL];

#ifdef HAL_SLEEP_MANAGER_ENABLED
static uint8_t gdma_sleep_handler[DMA_MAX_FULL_CHANNEL] = {0};
static char *gdma_lock_sleep_name[DMA_MAX_FULL_CHANNEL] = {"GDMA0", "GDMA1"};
#endif


static inline bool hal_gdma_is_channel_vaild(hal_gdma_channel_t channel)
{
    return channel < HAL_GDMA_CHANNEL_MAX;
}

static DMA_TranSize hal_gdma_calc_size(uint32_t src_addr,
                                       uint32_t dest_addr,
                                       uint32_t length,
                                       uint32_t *count)
{
    if ((0 == (src_addr % 4))
            && (0 == (dest_addr % 4))
            && (0 == (length % 4))) {
        *count = length / 4;
        return DMA_LONG;
    } else if ((0 == (src_addr % 2))
               && (0 == (dest_addr % 2))
               && (0 == (length % 2))) {
        *count = length / 2;
        return DMA_SHORT;
    } else {
        *count = length;
        return DMA_BYTE;
    }
}

static void hal_gdma1_callback(void)
{
    if (NULL != g_gdma_callback_context[0].callback) {
        g_gdma_callback_context[0].callback(HAL_GDMA_EVENT_TRANSACTION_SUCCESS,
                                            g_gdma_callback_context[0].user_data);
    }
}

static void hal_gdma2_callback(void)
{
    if (NULL != g_gdma_callback_context[1].callback) {
        g_gdma_callback_context[1].callback(HAL_GDMA_EVENT_TRANSACTION_SUCCESS,
                                            g_gdma_callback_context[1].user_data);
    }
}

hal_gdma_status_t hal_gdma_init(hal_gdma_channel_t channel)
{
    uint8_t channel_used = 0xff;
    if (!hal_gdma_is_channel_vaild(channel)) {
        log_hal_error("channel invalid: %d", channel);
        return HAL_GDMA_STATUS_ERROR_CHANNEL;
    }
    /*get sleep handler*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
    gdma_sleep_handler[channel] = hal_sleep_manager_set_sleep_handle(gdma_lock_sleep_name[channel]);
    if (gdma_sleep_handler[channel] == INVALID_SLEEP_HANDLE) {
        log_hal_error("there's no available handle when GDMA get sleep handle");
        return HAL_GDMA_STATUS_ERROR;
    }
#endif
    memset(&g_gdma_callback_context[channel], 0, sizeof(g_gdma_callback_context));
    channel_used = dma_set_channel_busy(channel);
    if (0xff == channel_used) {
        return HAL_GDMA_STATUS_ERROR;
    }
    return HAL_GDMA_STATUS_OK;
}

hal_gdma_status_t hal_gdma_deinit(hal_gdma_channel_t channel)
{

    if (!hal_gdma_is_channel_vaild(channel)) {
        log_hal_error("channel invalid: %d", channel);
        return HAL_GDMA_STATUS_ERROR_CHANNEL;
    }

    /*release sleep hander*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_release_sleep_handle(gdma_sleep_handler[channel]);
#endif

    dma_set_channel_idle(channel);
    memset(&g_gdma_callback_context[channel], 0, sizeof(g_gdma_callback_context));
    return HAL_GDMA_STATUS_OK;
}

hal_gdma_status_t hal_gdma_start_interrupt(hal_gdma_channel_t channel,
        uint32_t destination_address,
        uint32_t source_address,
        uint32_t data_length)
{
    DMA_INPUT input;
    uint32_t count = 0;
    uint32_t size = 0;
    DMA_SWCOPYMENU menu;
    static const uint8_t invalid_handle = 0xff;
    uint8_t handle;
    if (!hal_gdma_is_channel_vaild(channel)) {
        log_hal_error("channel invalid: %d", channel);
        return HAL_GDMA_STATUS_ERROR_CHANNEL;
    }
    handle = DMA_FullSize_CheckIdleChannel(DMA_SW, channel);

    if (invalid_handle == handle) {
        log_hal_error("DMA_FullSize_GetChannel fail.");
        return HAL_GDMA_STATUS_ERROR;
    }
    if (data_length == 0) {
        return HAL_GDMA_STATUS_INVALID_PARAMETER;
    }

#ifdef  HAL_GDMA_WRAP_FLASH_ADDRESS_TO_VIRTUAL_ADDRESS
    if ((destination_address & HAL_GDMA_WRAP_FLASH_ADDRESS_HIGH_BYTE_MASK) == HAL_GDMA_WRAP_FLASH_ADDRESS_MASK) {

        destination_address |= HAL_GDMA_WRAP_FLASH_VIRTUAL_ADDRESS_MASK;
    }

    if ((source_address & HAL_GDMA_WRAP_FLASH_ADDRESS_HIGH_BYTE_MASK) == HAL_GDMA_WRAP_FLASH_ADDRESS_MASK) {

        source_address |= HAL_GDMA_WRAP_FLASH_VIRTUAL_ADDRESS_MASK;
    }
#endif

    /*lock sleep mode*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_lock_sleep(gdma_sleep_handler[channel]);
#endif

    menu.srcaddr = source_address;
    menu.dstaddr = destination_address;

    size = hal_gdma_calc_size(source_address,
                              destination_address,
                              data_length,
                              &count);

    input.type = DMA_SWCOPY;
    input.size = (DMA_TranSize)size;
    input.count = count;
    input.menu = &menu;

    if (handle == 1) {
        input.callback = hal_gdma1_callback;
    } else {
        input.callback = hal_gdma2_callback;
    }


    DMA_FullSize_Config(handle, &input, true);
    return HAL_GDMA_STATUS_OK;
}

hal_gdma_status_t hal_gdma_start_polling(hal_gdma_channel_t channel,
        uint32_t destination_address,
        uint32_t source_address,
        uint32_t data_length)
{
    DMA_INPUT input;
    uint32_t count = 0;
    uint32_t size = 0;
    DMA_SWCOPYMENU menu;
    static const uint8_t invalid_handle = 0xff;
    uint8_t handle;
    if (!hal_gdma_is_channel_vaild(channel)) {
        log_hal_error("channel invalid: %d", channel);
        return HAL_GDMA_STATUS_ERROR_CHANNEL;
    }
    handle = DMA_FullSize_CheckIdleChannel(DMA_SW, channel);
    if (invalid_handle == handle) {
        log_hal_error("DMA_FullSize_GetChannel fail.");
        return HAL_GDMA_STATUS_ERROR;
    }
    if (data_length == 0) {
        return HAL_GDMA_STATUS_INVALID_PARAMETER;
    }

#ifdef  HAL_GDMA_WRAP_FLASH_ADDRESS_TO_VIRTUAL_ADDRESS
    if ((destination_address & HAL_GDMA_WRAP_FLASH_ADDRESS_HIGH_BYTE_MASK) == HAL_GDMA_WRAP_FLASH_ADDRESS_MASK) {

        destination_address |= HAL_GDMA_WRAP_FLASH_VIRTUAL_ADDRESS_MASK;
    }

    if ((source_address & HAL_GDMA_WRAP_FLASH_ADDRESS_HIGH_BYTE_MASK) == HAL_GDMA_WRAP_FLASH_ADDRESS_MASK) {

        source_address |= HAL_GDMA_WRAP_FLASH_VIRTUAL_ADDRESS_MASK;
    }
#endif

    /*lock sleep mode*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_lock_sleep(gdma_sleep_handler[channel]);
#endif

    menu.srcaddr = source_address;
    menu.dstaddr = destination_address;

    size = hal_gdma_calc_size(source_address,
                              destination_address,
                              data_length,
                              &count);

    input.type = DMA_SWCOPY;
    input.size = (DMA_TranSize)size;
    input.count = count;
    input.menu = &menu;
    input.callback = NULL;
    DMA_FullSize_Config(handle, &input, true);
    while (DMA_CheckRunStat(channel + 1));
    return HAL_GDMA_STATUS_OK;
}

hal_gdma_status_t hal_gdma_register_callback(hal_gdma_channel_t channel,
        hal_gdma_callback_t callback,
        void *user_data)
{
    if (!hal_gdma_is_channel_vaild(channel)) {
        log_hal_error("channel invalid: %d", channel);
        return HAL_GDMA_STATUS_ERROR_CHANNEL;
    }
    g_gdma_callback_context[channel].callback = callback;
    g_gdma_callback_context[channel].user_data = user_data;
    return HAL_GDMA_STATUS_OK;
}

hal_gdma_status_t hal_gdma_get_running_status(hal_gdma_channel_t channel, hal_gdma_running_status_t *running_status)

{


    if (!hal_gdma_is_channel_vaild(channel)) {
        log_hal_error("channel invalid: %d", channel);
        return HAL_GDMA_STATUS_ERROR_CHANNEL;
    }

    if (DMA_CheckRunStat(channel + 1)) {
        *running_status = HAL_GDMA_BUSY;
    } else {
        *running_status = HAL_GDMA_IDLE;
    }
    return HAL_GDMA_STATUS_OK;
}


hal_gdma_status_t hal_gdma_stop(hal_gdma_channel_t channel)
{
    uint8_t index = 0;

    if (!hal_gdma_is_channel_vaild(channel)) {
        log_hal_error("channel invalid: %d", channel);
        return HAL_GDMA_STATUS_ERROR_CHANNEL;
    }

    index = (uint8_t)channel + 1;

    DMA_FullSize_FreeChannel(index);

    /*unlock sleep mode*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_unlock_sleep(gdma_sleep_handler[channel]);
#endif

    return HAL_GDMA_STATUS_OK;

}

#endif
