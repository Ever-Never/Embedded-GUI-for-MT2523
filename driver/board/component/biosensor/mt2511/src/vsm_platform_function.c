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

#include "vsm_platform_function.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "syslog.h"
#include "mems_bus.h"
#include "memory_attribute.h"
#include "task.h"
#include "hal_gpio.h"
#include "hal_gpt.h"

log_create_module(mt2511_platform_function, PRINT_LEVEL_INFO);

#define LOGE(fmt,arg...)   LOG_E(mt2511_platform_function, "[mt2511]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(mt2511_platform_function, "[mt2511]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(mt2511_platform_function, "[mt2511]"fmt,##arg)

static uint32_t vsm_curr_tick;
static uint32_t vsm_curr_ms;
static uint32_t vsm_tick_remainder;

void vsm_platform_gpio_get_output(int32_t gpio_pin, int32_t *gpio_data)
{
    hal_gpio_get_output((hal_gpio_pin_t)gpio_pin, (hal_gpio_data_t *)gpio_data);
}

void vsm_platform_gpio_set_output(int32_t gpio_pin, int32_t gpio_data)
{
    hal_gpio_set_output((hal_gpio_pin_t)gpio_pin, (hal_gpio_data_t)gpio_data);
}

void vsm_platform_gpio_get_direction(int32_t gpio_pin, int32_t *gpio_direction)
{
    hal_gpio_get_direction((hal_gpio_pin_t)gpio_pin, (hal_gpio_direction_t *)gpio_direction);
}

void vsm_platform_gpio_set_direction(int32_t gpio_pin, int32_t gpio_direction)
{
    hal_gpio_set_direction((hal_gpio_pin_t)gpio_pin,(hal_gpio_direction_t) gpio_direction);
}

void vsm_platform_gpio_set_pinmux(int32_t gpio_pin, uint8_t function_index)
{
    hal_pinmux_set_function((hal_gpio_pin_t)gpio_pin,function_index);
}

void vsm_platform_gpio_init(int32_t gpio_pin)
{
    hal_gpio_init((hal_gpio_pin_t)gpio_pin);
}

void vsm_platform_gpio_deinit(int32_t gpio_pin)
{
    hal_gpio_deinit((hal_gpio_pin_t)gpio_pin);
}

void ms_delay(uint32_t ms)
{
#if 0
    uint32_t end, current; 
    current = xTaskGetTickCount();
    end = xTaskGetTickCount() + ms + 1;

    do{
        current = xTaskGetTickCount();
    }while(time_before(current, end));
#else
    //hal_gpt_delay_ms(ms);
    vTaskDelay(ms);
#endif
    
}


uint32_t vsm_platform_get_ms_tick(void)
{
    uint32_t new_curr_tick;
    hal_gpt_status_t ret;

    ret = hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &new_curr_tick);
    if (ret != HAL_GPT_STATUS_OK) {
        LOGE("hal_gpt_get_free_run_count err (%d) \r\n", ret);
    }

    vsm_curr_ms += (uint32_t)((((uint64_t)(new_curr_tick - vsm_curr_tick))*1000 + vsm_tick_remainder)/32768);
    vsm_tick_remainder = (((uint64_t)(new_curr_tick - vsm_curr_tick))*1000%32768);
    vsm_curr_tick = new_curr_tick;

    return vsm_curr_ms;
}

