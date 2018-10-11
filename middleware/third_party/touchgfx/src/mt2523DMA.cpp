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
#include <touchgfx/mt2523DMA.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/Color.hpp>

#include "syslog.h"

#include "hal_g2d.h"
#include "hal_gpt.h"

#define G2D_EXECUTION_TIMEOUT (32768)

extern bool disableSwapFrameBuffer;

hal_g2d_handle_t *g2dHandle = NULL;

void g2d_end(void);

bool g2d_callback(void *data)
{
    disableSwapFrameBuffer = false;
    g2d_end();
    touchgfx::HAL::getInstance()->signalDMAInterrupt();

    return true;
}

void g2d_begin(void)
{
    if (g2dHandle == NULL) {
        if (HAL_G2D_STATUS_OK != hal_g2d_get_handle(&g2dHandle, HAL_G2D_CODEC_TYPE_HW, HAL_G2D_GET_HANDLE_MODE_NON_BLOCKING)) {
            g2dHandle = NULL;
            LOG_E(tgfx, "[MT2523DMA] hal_g2d_get_handle failed.\n\r");
        } else {
            hal_g2d_register_callback(g2dHandle, g2d_callback);
        }
    }
}

void g2d_end(void)
{
    if (g2dHandle) {
        hal_g2d_release_handle(g2dHandle);
        g2dHandle = NULL;
    }
}

MT2523DMA::MT2523DMA()
    : touchgfx::DMA_Interface(dma_queue), dma_queue(queue_storage, sizeof(queue_storage) / sizeof(queue_storage[0])),
      pBlitOp(0), numLines(0), use32Bit(0)
{
}

MT2523DMA::~MT2523DMA()
{
}

void MT2523DMA::initialize()
{
    hal_g2d_status_t status = hal_g2d_init();

    LOG_I(tgfx, "[MT2523DMA::initialize] g2d init status: %d\n\r", status);
}

BlitOperations MT2523DMA::getBlitCaps()
{
    return static_cast<touchgfx::BlitOperations>(touchgfx::BLIT_OP_COPY |
                                                 touchgfx::BLIT_OP_FILL |
                                                 touchgfx::BLIT_OP_COPY_WITH_ALPHA |
                                                 touchgfx::BLIT_OP_FILL_WITH_ALPHA |
                                                 touchgfx::BLIT_OP_COPY_ARGB8888 |
                                                 touchgfx::BLIT_OP_COPY_ARGB8888_WITH_ALPHA |
                                                 touchgfx::BLIT_OP_COPY_A4 |
                                                 touchgfx::BLIT_OP_COPY_A8);
}

void MT2523DMA::setupDataCopy(const touchgfx::BlitOp& blitOp)
{
    uint8_t dstBitDepth = HAL::lcd().bitDepth();
    uint8_t alpha;
    uint32_t color;
    uint32_t srcBufferSize;
    uint32_t dstBufferSize = (dstBitDepth == 16) ? (blitOp.nSteps * blitOp.nLoops * 2) : (blitOp.nSteps * blitOp.nLoops * 3);
    hal_g2d_overlay_layer_function_t function = HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER;
    hal_g2d_color_format_t srcColorFormat;
    hal_g2d_color_format_t dstColorFormat = (dstBitDepth == 16) ? HAL_G2D_COLOR_FORMAT_RGB565 : HAL_G2D_COLOR_FORMAT_RGB888;
    hal_g2d_font_format_t srcFontFormat;
    hal_g2d_status_t g2dStatus;

    //LOG_I(tgfx, "[MT2523DMA::setupDataCopy] op:%x\n\r", (unsigned int)blitOp.operation);
    switch (blitOp.operation) {
    case touchgfx::BLIT_OP_COPY:
    case touchgfx::BLIT_OP_COPY_WITH_ALPHA:
        srcColorFormat = HAL_G2D_COLOR_FORMAT_RGB565;
        srcBufferSize = blitOp.nSteps * blitOp.nLoops * 2;
        break;
    case touchgfx::BLIT_OP_COPY_ARGB8888:
    case touchgfx::BLIT_OP_COPY_ARGB8888_WITH_ALPHA:
        srcColorFormat = HAL_G2D_COLOR_FORMAT_ARGB8888;
        srcBufferSize = blitOp.nSteps * blitOp.nLoops * 4;
        break;
    case touchgfx::BLIT_OP_COPY_A4:
        function = HAL_G2D_OVERLAY_LAYER_FUNCTION_FONT;
        srcFontFormat = HAL_G2D_FONT_FORMAT_4_BIT;
        srcBufferSize = blitOp.nSteps * blitOp.nLoops / 2;
        break;
    case touchgfx::BLIT_OP_COPY_A8:
        function = HAL_G2D_OVERLAY_LAYER_FUNCTION_FONT;
        srcFontFormat = HAL_G2D_FONT_FORMAT_8_BIT;
        srcBufferSize = blitOp.nSteps * blitOp.nLoops;
        break;
    default:
        LOG_E(tgfx, "[MT2523DMA::setupDataCopy] Invalid op:%x\n\r", (unsigned int)blitOp.operation);
        touchgfx::HAL::getInstance()->signalDMAInterrupt();
        return;
    }

    g2d_begin();

    if (g2dHandle == NULL) {
        LOG_E(tgfx, "[MT2523DMA::setupDataCopy] Invalid handle. Address = 0x%08X.\n\r", (unsigned int)blitOp.pSrc);
        g2d_end();
        touchgfx::HAL::getInstance()->signalDMAInterrupt();
        return;
    }

    hal_g2d_set_rgb_buffer(g2dHandle, (uint8_t *)blitOp.pDst, dstBufferSize, blitOp.dstLoopStride, blitOp.nLoops, dstColorFormat);
    hal_g2d_overlay_set_roi_window(g2dHandle, 0, 0, blitOp.dstLoopStride, blitOp.nLoops);
    hal_g2d_set_clipping_window(g2dHandle, 0, 0, blitOp.nSteps, blitOp.nLoops);

    if (function == HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER) {
        hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER1, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
        hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER1, (uint8_t*)blitOp.pSrc, srcBufferSize, blitOp.srcLoopStride, blitOp.nLoops, srcColorFormat);
        hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER1, 0, 0, blitOp.srcLoopStride, blitOp.nLoops);
        alpha = (blitOp.operation & (touchgfx::BLIT_OP_COPY_WITH_ALPHA | touchgfx::BLIT_OP_COPY_ARGB8888_WITH_ALPHA)) ? blitOp.alpha : 0xFF;
        hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER1, alpha);
        hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER1, true);
    }
    if (function == HAL_G2D_OVERLAY_LAYER_FUNCTION_FONT) {
        disableSwapFrameBuffer = true;
        color = (blitOp.alpha << 24) | (touchgfx::Color::getRedColor(blitOp.color) << 16) | (touchgfx::Color::getGreenColor(blitOp.color) << 8) | (touchgfx::Color::getBlueColor(blitOp.color));
        hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER1, HAL_G2D_OVERLAY_LAYER_FUNCTION_FONT);
        hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER1, (uint8_t*)blitOp.pSrc, srcBufferSize, blitOp.srcLoopStride, blitOp.nLoops, HAL_G2D_COLOR_FORMAT_ARGB8888);
        hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER1, 0, 0, blitOp.srcLoopStride, blitOp.nLoops);
        hal_g2d_overlay_set_layer_font_format(g2dHandle, HAL_G2D_OVERLAY_LAYER1, srcFontFormat);
        hal_g2d_overlay_set_layer_font_color(g2dHandle, HAL_G2D_OVERLAY_LAYER1, color);
        hal_g2d_overlay_set_layer_alpha(g2dHandle, HAL_G2D_OVERLAY_LAYER1, 0xFF);
        hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER1, true);
    }

    if (blitOp.operation & (touchgfx::BLIT_OP_COPY_WITH_ALPHA | touchgfx::BLIT_OP_COPY_ARGB8888 | touchgfx::BLIT_OP_COPY_ARGB8888_WITH_ALPHA | touchgfx::BLIT_OP_COPY_A4 | touchgfx::BLIT_OP_COPY_A8)) {
        hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
        hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, (uint8_t*)blitOp.pDst, dstBufferSize, blitOp.dstLoopStride, blitOp.nLoops, dstColorFormat);
        hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 0, 0, blitOp.dstLoopStride, blitOp.nLoops);
        hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, true);
    } else {
        hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, false);
    }

    g2dStatus = hal_g2d_overlay_start(g2dHandle);
    if (g2dStatus != HAL_G2D_STATUS_OK) {
        LOG_E(tgfx, "[MT2523DMA::setupDataCopy] hal_g2d_overlay_start failed.\n\r");
        g2d_end();
        touchgfx::HAL::getInstance()->signalDMAInterrupt();
    }
}

void MT2523DMA::setupDataFill(const touchgfx::BlitOp& blitOp)
{
    uint8_t dstBitDepth = HAL::lcd().bitDepth();
    uint8_t alpha = (blitOp.operation & touchgfx::BLIT_OP_FILL_WITH_ALPHA) ? blitOp.alpha : 0xFF;
    uint32_t color = (alpha << 24) | (touchgfx::Color::getRedColor(blitOp.color) << 16) | (touchgfx::Color::getGreenColor(blitOp.color) << 8) | (touchgfx::Color::getBlueColor(blitOp.color));
    uint32_t dstBufferSize = (dstBitDepth == 16) ? (blitOp.nSteps * blitOp.nLoops * 2) : (blitOp.nSteps * blitOp.nLoops * 3);
    hal_g2d_color_format_t dstColorFormat = (dstBitDepth == 16) ? HAL_G2D_COLOR_FORMAT_RGB565 : HAL_G2D_COLOR_FORMAT_RGB888;
    hal_g2d_status_t g2dStatus;

    //LOG_I(tgfx, "[MT2523DMA::setupDataFill] op:%x\n\r", (unsigned int)blitOp.operation);

    g2d_begin();

    if (g2dHandle == NULL) {
        LOG_E(tgfx, "[MT2523DMA::setupDataFill] Invalid handle. Address = 0x%08X.\n\r", (unsigned int)blitOp.pSrc);
        touchgfx::HAL::getInstance()->signalDMAInterrupt();
        return;
    }

    hal_g2d_set_rgb_buffer(g2dHandle, (uint8_t *)blitOp.pDst, dstBufferSize, blitOp.dstLoopStride, blitOp.nLoops, dstColorFormat);
    hal_g2d_overlay_set_roi_window(g2dHandle, 0, 0, blitOp.dstLoopStride, blitOp.nLoops);
    hal_g2d_set_clipping_window(g2dHandle, 0, 0, blitOp.nSteps, blitOp.nLoops);

    hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER1, HAL_G2D_OVERLAY_LAYER_FUNCTION_RECTFILL);
    hal_g2d_overlay_set_layer_rectfill_color(g2dHandle, HAL_G2D_OVERLAY_LAYER1, color);
    hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER1, 0, 0, blitOp.dstLoopStride, blitOp.nLoops);
    hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER1, true);

    if (blitOp.operation & touchgfx::BLIT_OP_FILL_WITH_ALPHA) {
        hal_g2d_overlay_set_layer_function(g2dHandle, HAL_G2D_OVERLAY_LAYER0, HAL_G2D_OVERLAY_LAYER_FUNCTION_BUFFER);
        hal_g2d_overlay_set_layer_rgb_buffer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, (uint8_t*)blitOp.pDst, dstBufferSize, blitOp.dstLoopStride, blitOp.nLoops, dstColorFormat);
        hal_g2d_overlay_set_layer_window(g2dHandle, HAL_G2D_OVERLAY_LAYER0, 0, 0, blitOp.dstLoopStride, blitOp.nLoops);
        hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, true);
    } else {
        hal_g2d_overlay_enable_layer(g2dHandle, HAL_G2D_OVERLAY_LAYER0, false);
    }

    g2dStatus = hal_g2d_overlay_start(g2dHandle);
    if (g2dStatus != HAL_G2D_STATUS_OK) {
        LOG_E(tgfx, "[MT2523DMA::setupDataCopy] hal_g2d_overlay_start failed.\n\r");
        g2d_end();
        touchgfx::HAL::getInstance()->signalDMAInterrupt();
    }
}

void MT2523DMA::signalDMAInterrupt()
{
    executeCompleted();
}

