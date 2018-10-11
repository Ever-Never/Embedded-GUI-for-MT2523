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

#include <touchgfx/hal/OSWrappers.hpp>
#include <touchgfx/mt2523HAL.hpp>
#include <touchgfx/lcd/LCD.hpp>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "syslog.h"

#include "hal_gpt.h"
#include "mt25x3_hdk_lcd.h"

volatile bool refreshRequested = false;
volatile bool disableSwapFrameBuffer = false;
volatile bool interruptdisabled = false;
QueueHandle_t xDisplayQueue;
QueueHandle_t xDisplayDoneQueue;

uint32_t awakeCount = 0;
uint32_t awakeCounter = 0;
uint32_t current_addr;
hal_display_lcd_layer_input_t layer_para;

void VsyncHandler(void *user_data);

using namespace touchgfx;

uint16_t* mt2523HAL::getTFTFrameBuffer() const
{
    return (uint16_t*)current_addr;
}

void mt2523HAL::setTFTFrameBuffer(uint16_t* adr)
{
    current_addr = (uint32_t)adr;
}

void mt2523HAL::configureInterrupts()
{
}

/* Enable LCD line interrupt, when entering video (active) area */
void mt2523HAL::enableLCDControllerInterrupt()
{
}

void mt2523HAL::disableInterrupts()
{
    interruptdisabled = true;
    NVIC_DisableIRQ(G2D_IRQn);
}

void mt2523HAL::enableInterrupts()
{
    interruptdisabled = false;
    NVIC_EnableIRQ(G2D_IRQn);
}

bool mt2523HAL ::beginFrame()
{
    bool ret = HAL::beginFrame();
    if (true == ret) {
        refreshRequested = false;
    }

    return ret;
}

void mt2523HAL::endFrame()
{
    HAL::endFrame();
    // base class will have set this var if something was drawn in this frame
    if (frameBufferUpdatedThisFrame) {
        refreshRequested = true;
        VsyncHandler(NULL);
    }
}

void VsyncHandler(void *user_data)
{
    BaseType_t status;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t message;

    if (interruptdisabled)
        return;

    HAL::getInstance()->vSync();
    OSWrappers::signalVSync();
    // Swap frame buffers immediately instead of waiting for the task to be scheduled in.
    // Note: task will also swap when it wakes up, but that operation is guarded and will not have
    // any effect if already swapped.

    if (awakeCount) {
        awakeCounter = awakeCount;
    }

    if (refreshRequested || awakeCount) {
        status = xQueueSendFromISR(xDisplayQueue, &message, &xHigherPriorityTaskWoken);
        if (status != pdTRUE)
            LOG_E(tgfx, "[VsyncHandler] xQueueSendFromISR xDisplayQueue failed %ld.\n\r", status);
    }
}

void lcd_callback(void *data)
{
    uint32_t displayStatus = 0;
    BaseType_t status;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    status = xQueueSendFromISR(xDisplayDoneQueue, &displayStatus, &xHigherPriorityTaskWoken);
    if (status != pdTRUE)
        LOG_E(tgfx, "[lcd_callback] xQueueSendFromISR xDisplayDoneQueue failed %ld.\n\r", status);
}

void DisplayTask(void *pvParameters)
{
    BaseType_t status;
    uint32_t height, width;
    uint32_t message;
    uint32_t displayStatus;

    xDisplayQueue = xQueueCreate(10 , sizeof(uint32_t));
    xDisplayDoneQueue = xQueueCreate(2 , sizeof(uint32_t));

    if (hal_gpt_init(HAL_GPT_2) != HAL_GPT_STATUS_OK) {
        LOG_E(tgfx, "ERROR : HAL_GPT_2 Init failed\n\r");
    }

    if (hal_gpt_register_callback(HAL_GPT_2, (hal_gpt_callback_t)VsyncHandler, NULL) != HAL_GPT_STATUS_OK) {
        LOG_E(tgfx, "ERROR : HAL_GPT_2 register callback failed\n\r");
    }

    if (hal_gpt_start_timer_ms(HAL_GPT_2, 20, HAL_GPT_TIMER_TYPE_REPEAT) != HAL_GPT_STATUS_OK) {
        LOG_E(tgfx, "ERROR : HAL_GPT_2 start timer failed\n\r");
    }

    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_WIDTH, &width);
    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_HEIGHT, &height);
    bsp_lcd_register_callback(lcd_callback);

    layer_para.source_key_flag = 0;
    layer_para.alpha_flag = 0;
    layer_para.color_format = HAL_DISPLAY_LCD_LAYER_COLOR_RGB565;
    layer_para.alpha = 0;
    layer_para.rotate = HAL_DISPLAY_LCD_LAYER_ROTATE_0;
    layer_para.row_size = height;
    layer_para.column_size = width;
    layer_para.window_x_offset = 0;
    layer_para.window_y_offset = 0;
    layer_para.layer_enable = HAL_DISPLAY_LCD_LAYER0;
    layer_para.pitch = width * 2;

    while (1) {
        if (refreshRequested) {
            refreshRequested = false;
            if (!disableSwapFrameBuffer)
                HAL::getInstance()->swapFrameBuffers();
            layer_para.buffer_address = current_addr;
            bsp_lcd_config_layer(&layer_para);
            bsp_lcd_update_screen(0, 0, width - 1, height - 1);
            status = xQueueReceive(xDisplayDoneQueue, &displayStatus, 1000 / portTICK_RATE_MS);
            if (status != pdTRUE)
                LOG_E(tgfx, "[DisplayTask] xQueueReceive xDisplayDoneQueue failed %ld.\n\r", status);
        }
        if (awakeCounter > 0) {
            if (!interruptdisabled) {
                refreshRequested = true;
                HAL::getInstance()->vSync();
                OSWrappers::signalVSync();
                awakeCounter--;
            }
            vTaskDelay(20 / portTICK_RATE_MS);
        } else {
            status = xQueueReceive(xDisplayQueue, &message, portMAX_DELAY);
            if (status != pdTRUE)
                LOG_E(tgfx, "[DisplayTask] xQueueReceive xDisplayQueue failed %ld.\n\r", status);
        }
    }
}
