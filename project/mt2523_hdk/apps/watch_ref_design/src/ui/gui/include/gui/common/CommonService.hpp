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

#ifndef COMMON_SERVICE_HPP
#define COMMON_SERVICE_HPP

#include <touchgfx/hal/HAL.hpp>
#include <gui/common/FrontendApplication.hpp>

#ifndef SIMULATOR
#include "FreeRTOS.h"
#include "hal_keypad.h"
#include "keypad_custom.h"
#include "nvdm.h"
#include "timers.h"
#include "mt25x3_hdk_backlight.h"
#include "syslog.h"
#include "task.h"
#include "queue.h"
#include "sensor_demo.h"

#include "bsp_ctp.h"
#include "BoardConfiguration.hpp"
#include "main.h"
#endif

#define TOUCHGFX_NVDM_GROUP "touchGFX"
#define HRSTATUS_NVDM_KEY   "hrStatus"
#define STTYPE_NVDM_KEY     "screenTimeoutType"

class CommonService
{
public:
    static bool hrStatus;

    enum TimeoutType {
        TIMEOUT_10S = 0,
        TIMEOUT_30S,
        TIMEOUT_ALWAYS,
        MAX_TIMEOUT_TYPE
    };

    static TimeoutType screenTimeoutType;

#ifndef SIMULATOR
    static bool backlight;

    static TimerHandle_t timer;

    static QueueHandle_t queue;
#endif

    static bool getHeartRateStatus()
    {
        return hrStatus;
    }

    static void setHeartRateStatus(bool status)
    {
        if (hrStatus == status) {
            return;
        }
        hrStatus = status;
#ifndef SIMULATOR
        nvdm_status_t result = nvdm_write_data_item(TOUCHGFX_NVDM_GROUP, HRSTATUS_NVDM_KEY, NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&hrStatus, 1);
        LOG_I(tgfx, "result = %d, hrStatus = %d\r\n", result, hrStatus);
        if (false == status) {
            disable_all_sensors();
        }
#endif
    }

    static TimeoutType getScreenTimeoutType()
    {
        return screenTimeoutType;
    }

    static void setScreenTimeoutType(TimeoutType type)
    {
        if (screenTimeoutType == type) {
            return;
        }
        screenTimeoutType = type;
#ifndef SIMULATOR
        nvdm_status_t result = nvdm_write_data_item(TOUCHGFX_NVDM_GROUP, STTYPE_NVDM_KEY, NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&screenTimeoutType, 1);
        LOG_I(tgfx, "result = %d, screenTimeoutType = %d\r\n", result, screenTimeoutType);
#endif
    }

#ifndef SIMULATOR
    static void gotoPowerkeyCallback(void *user_data)
    {
        hal_keypad_powerkey_event_t powekey_event;
        hal_keypad_status_t status = hal_keypad_powerkey_get_key(&powekey_event);
        if (status == HAL_KEYPAD_STATUS_OK && powekey_event.state == HAL_KEYPAD_KEY_RELEASE) {
            MsgStruct msg;
            BaseType_t xHigherPriorityTaskWoken;

            // We have not woken a task at the start of the ISR.
            xHigherPriorityTaskWoken = pdFALSE;

            msg.id = MSG_TYPE_POWER_KEY;
            while (xQueueSendFromISR(queue, &msg, &xHigherPriorityTaskWoken) != pdTRUE);

            // Now the buffer is empty we can switch context if necessary.
            if (xHigherPriorityTaskWoken) {
                // Actual macro used here is port specific.
                portYIELD_FROM_ISR(pdTRUE);
            }
        }
    }
#endif

    static void registerPowerkey()
    {
#ifndef SIMULATOR
        bool ret = keypad_custom_powerkey_init();
        if (ret == false) {
            configASSERT(0);
            return;
        }

        hal_keypad_status_t status = hal_keypad_powerkey_register_callback(gotoPowerkeyCallback, NULL);
        if (status != HAL_KEYPAD_STATUS_OK) {
            configASSERT(0);
            return;
        }
#endif
    }

#ifndef SIMULATOR
    static void gotoBacklightCallback(TimerHandle_t xTimer)
    {
        xTimerStop(timer, 0);
        bsp_ctp_power_on(false);
        bsp_backlight_enable(false);
        backlight = true;
        setPowerMode(1000);
    }
#endif

    static void registerBacklight()
    {
#ifndef SIMULATOR
        timer = xTimerCreate("backlight", 0xffff, pdFALSE, NULL, gotoBacklightCallback);

        switch (screenTimeoutType) {
            case TIMEOUT_10S:
                turnOnBacklight(TIMEOUT_10S);
                break;
            case TIMEOUT_30S:
                turnOnBacklight(TIMEOUT_30S);
                break;
            case TIMEOUT_ALWAYS:
            default:
                break;
        }
#endif
    }

    static void turnOnBacklight()
    {
#ifndef SIMULATOR
        turnOnBacklight(TIMEOUT_ALWAYS);

        switch (screenTimeoutType) {
            case TIMEOUT_10S:
                turnOnBacklight(TIMEOUT_10S);
                break;
            case TIMEOUT_30S:
                turnOnBacklight(TIMEOUT_30S);
                break;
            case TIMEOUT_ALWAYS:
            default:
                break;
        }
#endif
    }

    static void turnOnBacklight(TimeoutType type)
    {
#ifndef SIMULATOR
        switch (type) {
            case TIMEOUT_10S:
                xTimerStop(timer, 0);
                xTimerChangePeriod(timer, 10000 / portTICK_PERIOD_MS, 0);
                xTimerReset(timer, 0);
                break;
            case TIMEOUT_30S:
                xTimerStop(timer, 0);
                xTimerChangePeriod(timer, 30000 / portTICK_PERIOD_MS, 0);
                xTimerReset(timer, 0);
                break;
            case TIMEOUT_ALWAYS:
                xTimerStop(timer, 0);
                if (backlight == true) {
                    bsp_ctp_power_on(true);
                    bsp_backlight_enable(true);
                    backlight = false;
                    setNormalMode();
                }
                break;
            default:
                break;
        }
#endif
    }

    static bool isBacklight()
    {
#ifndef SIMULATOR
        return backlight;
#else
        return false;
#endif
    }

    static void setPowerMode(uint32_t time)
    {
#ifndef SIMULATOR
        set_power_mode(PowerMode_Standby, time);
#endif
    }

    static void setNormalMode()
    {
#ifndef SIMULATOR
        set_power_mode(PowerMode_Normal, 0);
#endif
    }

#ifndef SIMULATOR
    typedef enum {
        MSG_TYPE_POWER_KEY = 0,
        MAX_MSG_TYPE
    } MsgType;

    typedef struct {
        MsgType id;
        void *msg;
    } MsgStruct;

    static void gotoTaskProc(void *arg)
    {
        MsgStruct msg;

        while (1) {
            if (pdTRUE == xQueueReceive(queue, &msg, portMAX_DELAY)) {
                switch (msg.id) {
                    case MSG_TYPE_POWER_KEY:
                        static_cast<FrontendApplication *>(Application::getInstance())->handleKeyEvent(0);
                        break;

                    default:
                        break;
                }
            }
        }
    }

    static void readNVDM()
    {
        uint32_t size = 1;
        nvdm_status_t result = nvdm_read_data_item(TOUCHGFX_NVDM_GROUP, HRSTATUS_NVDM_KEY, (uint8_t *)&hrStatus, &size);
        LOG_I(tgfx, "result = %d, hrStatus = %d, size = %d\r\n", result, hrStatus, size);
        size = 1;
        result = nvdm_read_data_item(TOUCHGFX_NVDM_GROUP, STTYPE_NVDM_KEY, (uint8_t *)&screenTimeoutType, &size);
        LOG_I(tgfx, "result = %d, screenTimeoutType = %d, size = %d\r\n", result, screenTimeoutType, size);
        size = 1;
    }

    static void createTask()
    {
        LOG_I(tgfx, "createTask\r\n");
        queue = xQueueCreate(10, sizeof(MsgStruct));
        xTaskCreate(gotoTaskProc, "CommonService", 2048 / sizeof(portSTACK_TYPE), NULL, 4, NULL);
    }
#endif
};

#endif /* COMMON_SERVICE_HPP */
