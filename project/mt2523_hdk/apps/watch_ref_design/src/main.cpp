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



#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/BoardConfiguration.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>
#include "sensor_demo.h"
#include <gui/database/heart_rate_db.hpp>
#include <gui/common/CommonService.hpp>


using namespace touchgfx;

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* device.h includes */
#include "mt2523.h"

/* hal includes */
#include "hal.h"
#include "nvdm.h"
#include "sys_init.h"
#include "utils.h"

#include "syslog.h"
#include "task_def.h"
//#include "gnss_app.h"

#include "mt25x3_hdk_lcd.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED
const char *touchgfx_name = "TouchGFX";
static uint8_t touchgfx_sleep_handle = 0xFF;
volatile bool touchgfx_lock = false;
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Create the log control block for freertos module.
 * The initialization of the log is in the sys_init.c.
 * Please refer to the log dev guide under /doc folder for more details.
 */
//log_create_module(freertos, PRINT_LEVEL_INFO);

#define mainCHECK_DELAY ( ( portTickType ) 1000 / portTICK_RATE_MS )


/**
 * Define the FreeRTOS task priorities and stack sizes
 */
#define configGUI_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 4 )

#define configGUI_TASK_STK_SIZE                 ( 1500 )

#define CANVAS_BUFFER_SIZE (5500)

static void GUITask(void* params)
{
    touchgfx::HAL::getInstance()->taskEntry();
}

void set_power_mode(PowerMode mode, uint32_t data)
{
    extern uint32_t awakeCount;
    static PowerMode previousMode = PowerMode_Normal;
    static uint32_t previousData = 20;

    switch (mode) {
        case PowerMode_Normal:
#ifdef HAL_SLEEP_MANAGER_ENABLED
            if (!touchgfx_lock) {
                hal_sleep_manager_lock_sleep(touchgfx_sleep_handle);
                touchgfx_lock = true;
            }
#endif
            if (data == 0)
                data = 20;
            awakeCount = 0;
            if (previousMode == PowerMode_Off)
                bsp_lcd_display_on();
            if (previousMode == PowerMode_Standby)
                bsp_lcd_exit_idle();
            break;
        case PowerMode_Standby:
            if (data == 0)
                data = 60000;
            if(data >= 60000)
                awakeCount = 1;
            if (previousMode != PowerMode_Standby)
                bsp_lcd_enter_idle();
#ifdef HAL_SLEEP_MANAGER_ENABLED
            if (touchgfx_lock) {
                hal_sleep_manager_unlock_sleep(touchgfx_sleep_handle);
                touchgfx_lock = false;
            }
#endif
            break;
        case PowerMode_Off:
            data = 0;
            awakeCount = 0;
            if (previousMode != PowerMode_Off)
                bsp_lcd_display_off();
#ifdef HAL_SLEEP_MANAGER_ENABLED
            if (touchgfx_lock) {
                hal_sleep_manager_unlock_sleep(touchgfx_sleep_handle);
                touchgfx_lock = false;
            }
#endif
            break;
        default:
            break;
    }

    if (mode != previousMode) {
        LOG_I(tgfx, "set_power_mode mode = %d\n\r", mode);
        previousMode = mode;
    }

    if (data != previousData) {
        LOG_I(tgfx, "set_power_mode data = %ld\n\r", data);

        if (hal_gpt_stop_timer(HAL_GPT_2) != HAL_GPT_STATUS_OK) {
            LOG_E(tgfx, "ERROR : HAL_GPT_2 stop timer failed\n\r");
        }

        if (data > 0) {
            if (hal_gpt_start_timer_ms(HAL_GPT_2, data, HAL_GPT_TIMER_TYPE_REPEAT) != HAL_GPT_STATUS_OK) {
                LOG_E(tgfx, "ERROR : HAL_GPT_2 start timer failed\n\r");
            }
        }

        previousData = data;
    }
}

/* sensor data update */
void update_hr_data(int32_t bpm)
{
    static int32_t hr_value = 0;

    if(bpm != hr_value)
    {
        hr_value = bpm;
        SENSOR_DEMO_LOGI("report sensor_data val to UI, value = (%ld)\r\n", hr_value);
        HeartRateCache::getHeartRateCacheInstance()->addHeartRateData(hr_value);
    }

    return;
}
void update_hrv_data(int32_t sddn,int32_t lf,int32_t hf,int32_t lfhf)
{
    return;
}
void update_bp_data(int32_t sbp, int32_t dbp)
{
    return;
}

#include "sensor_alg_interface.h"

void sensor_init()
{
    sensor_manager_init();

    enable_all_sensors(); // Heart rate is enabled
}

int main(void)
{
    static uint8_t canvasBuffer[CANVAS_BUFFER_SIZE];

    system_init();
    nvdm_init();

#ifdef __GNUC__
    section_init_func();
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
    touchgfx_sleep_handle = hal_sleep_manager_set_sleep_handle(touchgfx_name);
    if (!touchgfx_lock) {
        hal_sleep_manager_lock_sleep(touchgfx_sleep_handle);
        touchgfx_lock = true;
    }
#endif

    hw_init();
    LOG_I(tgfx, "hw_init finish\n\r");
    touchgfx_init();

#ifdef SENSOR_DEMO
    sensor_init();
#endif

    CommonService::createTask();
    CommonService::registerPowerkey();
    CommonService::registerBacklight();

    CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);

    xTaskCreate(GUITask, (const char*)"GUITask",
                configGUI_TASK_STK_SIZE,
                NULL,
                configGUI_TASK_PRIORITY,
                NULL);

    /* Start the scheduler. */
    vTaskStartScheduler();
    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for( ;; );
}

