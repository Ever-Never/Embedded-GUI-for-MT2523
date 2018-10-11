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

#include <common/TouchGFXInit.hpp>
#include <touchgfx/hal/BoardConfiguration.hpp>

#include <touchgfx/hal/NoDMA.hpp>
#include <touchgfx/mt2523DMA.hpp>
#include <touchgfx/mt2523HAL.hpp>

#include <touchgfx/hal/OSWrappers.hpp>
#include <touchgfx/LCD16bpp.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/hal/Buttons.hpp>
#include <touchgfx/hal/GPIO.hpp>

//driver includes
#include <touchgfx/mt2523TouchController.hpp>
#include <platform/core/arm/cortex-m/CortexMMCUInstrumentation.hpp>

#include "FreeRTOS.h"
#include "task.h"

#include "mt2523.h"
#include "mt25x3_hdk_lcd.h"
#include "mt25x3_hdk_backlight.h"
#include "memory_attribute.h"

/* SDRAM Address Base for DYCS0*/
#define SDRAM_BASE              0x28000000

#define configDisplay_TASK_PRIORITY ( tskIDLE_PRIORITY + 9 )
#define configDisplay_TASK_STK_SIZE ( 500 )

//16bpp rendering mode
LCD16bpp display;
//Initialize DMA driver with native display height
//NoDMA dma;
MT2523DMA dma;
//Define touch controller with dimensions of display
mt2523TouchController tc;
//MCU instrumentation for performance monitoring
CortexMMCUInstrumentation mcuInstr;
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t framebuffer[2][320 * 320 * 2];

extern void DisplayTask(void *pvParameters);

namespace touchgfx
{

void touchgfx_init()
{
    uint32_t width, height;

    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_WIDTH, &width);
    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_HEIGHT, &height);

    //Instantiate HAL
//    HAL& hal = touchgfx_generic_init<mt2523HAL>(dma, display, tc, DISPLAY_NATIVE_WIDTH, DISPLAY_NATIVE_HEIGHT, (uint16_t*)(SDRAM_BASE + (0xBB800 * 3)), 0xFFFFFFFF);
    HAL& hal = touchgfx_generic_init<mt2523HAL>(dma, display, tc, width, height, 0, 0);
    hal.setFrameBufferStartAddress((uint16_t*)&framebuffer[0][0], 16, true, false);
    hal.setTouchSampleRate(1);
    hal.setFingerSize(1);

    // By default frame rate compensation is off.
    // Enable frame rate compensation to smooth out animations in case there is periodic slow frame rates.
    hal.setFrameRateCompensation(false);

    // This platform can handle simultaneous DMA and TFT accesses to SDRAM, so disable lock to increase performance.
    hal.lockDMAToFrontPorch(false);

    //Set MCU instrumentation and Load calculation
    mcuInstr.init();
    hal.setMCUInstrumentation(&mcuInstr);
    hal.enableMCULoadCalculation(true);
}

void hw_init()
{
    uint32_t width, height;
    hal_display_lcd_roi_output_t lcd_para;

    //Initialize board
    bsp_lcd_init(0xF800);
    bsp_backlight_init();

    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_WIDTH, &width);
    bsp_lcd_get_parameter(LCM_IOCTRL_QUERY__LCM_HEIGHT, &height);

    lcd_para.target_start_x = 0;
    lcd_para.target_start_y = 0;
    lcd_para.target_end_x = width - 1;
    lcd_para.target_end_y = height - 1;
    lcd_para.roi_offset_x = 0;
    lcd_para.roi_offset_y = 0;
    lcd_para.main_lcd_output = LCM_16BIT_24_BPP_RGB888_1;
    bsp_lcd_config_roi(&lcd_para);

    xTaskCreate(DisplayTask, (const char*)"DisplayTask", configDisplay_TASK_STK_SIZE, NULL, configDisplay_TASK_PRIORITY, NULL);
    //For performance measurement purposes
    //GPIO::init();
}

}
