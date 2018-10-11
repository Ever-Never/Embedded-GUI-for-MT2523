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

#include "hal_eint.h"
#include "hal_log.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <string.h>
#include <stdio.h>
#include "syslog.h"
#include <stdlib.h>
#include "timers.h"
#include <assert.h>
#include "eint_key_custom.h"
#include "eint_key.h"
#ifdef EINT_KEY_HAL_MASK
#include "hal_eint_internal.h"
#endif

const char __attribute__((weak)) BSP_EINTKEY0_PIN = 0xff;
const char __attribute__((weak)) BSP_EINTKEY1_PIN = 0xff;
const char __attribute__((weak)) BSP_EINTKEY2_PIN = 0xff;

const char __attribute__((weak)) BSP_EINTKEY0_PIN_M_EINT = 0xff;
const char __attribute__((weak)) BSP_EINTKEY1_PIN_M_EINT = 0xff;
const char __attribute__((weak)) BSP_EINTKEY2_PIN_M_EINT = 0xff;

const unsigned char __attribute__((weak)) BSP_EINTKEY0_EINT = 0xff;
const unsigned char __attribute__((weak)) BSP_EINTKEY1_EINT = 0xff;
const unsigned char __attribute__((weak)) BSP_EINTKEY2_EINT = 0xff;


typedef struct {
    uint32_t                 longpress_time;
    uint32_t                 repeat_time;
    bsp_eint_key_event_t     key_sate[BSP_EINT_KEY_NUMBER];
    bool                     has_initiliazed;
    TimerHandle_t            rtos_timer[BSP_EINT_KEY_NUMBER];
    bsp_eint_key_callback_t  callback;
    void                    *user_data;
    bsp_eint_key_event_t    *event;
    uint8_t                  index[BSP_EINT_KEY_NUMBER];
    bool                     is_init_ready;
} bsp_eint_key_context_t;


bsp_eint_key_context_t eint_key_context;

void bsp_eint_key_timer_callback(TimerHandle_t xTimer)
{
    uint32_t   i;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    for(i=0; i<BSP_EINT_KEY_NUMBER; i++) {
        if (xTimer == eint_key_context.rtos_timer[i]) {
            break;
        }
    }

    if (eint_key_context.key_sate[i] == BSP_EINT_KEY_PRESS) {
        eint_key_context.key_sate[i] =  BSP_EINT_KEY_LONG_PRESS;
    }
    else {
        eint_key_context.key_sate[i] =  BSP_EINT_KEY_REPEAT;
    }
    
    if( xTimerChangePeriodFromISR( eint_key_context.rtos_timer[i], eint_key_context.repeat_time/portTICK_PERIOD_MS , &xHigherPriorityTaskWoken ) != pdPASS )
    {
        log_hal_info("[eint_key]repeat_time xTimerChangePeriodFromISR fail, index = %d\r\n",i);
    }

    eint_key_context.callback(eint_key_context.key_sate[i], eint_key_mapping[i].key_data, &eint_key_context.user_data);

}
 
void bsp_eint_key_callback(uint8_t *key_number)
{
    uint32_t index;
    BaseType_t xHigherPriorityTaskWoken;
    
    index = *key_number;

    //log_hal_info("[eint_key]enter callback:%d\r\n", index);

     xHigherPriorityTaskWoken = pdFALSE;
     
    if (eint_key_context.is_init_ready != true) {
        #ifdef EINT_KEY_HAL_MASK
        hal_eint_unmask(eint_key_mapping[index].eint_number);   
        #endif
        eint_key_context.key_sate[index] = BSP_EINT_KEY_RELEASE;
        if( xTimerStopFromISR( eint_key_context.rtos_timer[index], &xHigherPriorityTaskWoken ) != pdPASS )
        {
            log_hal_info("[eint_key]xTimerStopFromISR fail, index = %d is_init_ready\r\n",index);
        }
        
        return;
    } 
     

    if (eint_key_context.key_sate[index] == BSP_EINT_KEY_RELEASE) {
        eint_key_context.key_sate[index] = BSP_EINT_KEY_PRESS;

        if( xTimerChangePeriodFromISR( eint_key_context.rtos_timer[index], eint_key_context.longpress_time/portTICK_PERIOD_MS, &xHigherPriorityTaskWoken ) != pdPASS )
        {
            log_hal_info("[eint_key]longpress_time xTimerChangePeriodFromISR fail, index = %d\r\n",index);
        }
        
        if( xTimerStartFromISR( eint_key_context.rtos_timer[index], &xHigherPriorityTaskWoken ) != pdPASS )
        {
            log_hal_info("[eint_key]xTimerStartFromISR fail, index = %d\r\n",index);
        }
    }
    else {
        eint_key_context.key_sate[index] = BSP_EINT_KEY_RELEASE;
        
        if( xTimerStopFromISR( eint_key_context.rtos_timer[index], &xHigherPriorityTaskWoken ) != pdPASS )
        {
            log_hal_info("[eint_key]xTimerStopFromISR fail, index = %d\r\n",index);
        }
    } 

    eint_key_context.callback(eint_key_context.key_sate[index], eint_key_mapping[index].key_data, eint_key_context.user_data);
    
    if (xHigherPriorityTaskWoken != pdFALSE) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR(pdTRUE);
    }
    
    #ifdef EINT_KEY_HAL_MASK
    hal_eint_unmask(eint_key_mapping[index].eint_number);   
    #endif

}


bool bsp_eint_key_init(bsp_eint_key_config_t *config)
{
    uint8_t             i;
    uint8_t             count = 0;
    hal_eint_config_t   eint_config;
    
    if (config == NULL ) {
        log_hal_info("[eint_key]config pioter is null\r\n");
        return false;
    }

    if (eint_key_context.has_initiliazed == true) {
        log_hal_info("[eint_key]bsp eint key has initiliazed\r\n");
        return false;
    }       
    
    memset(&eint_key_context,  0,  sizeof(bsp_eint_key_context_t));

    eint_key_custom_init();
        
    if (config->longpress_time == 0) {
        eint_key_context.longpress_time = 2000; /*default 2s*/
    }
    else {
        eint_key_context.longpress_time = config->longpress_time;
    }

    if (config->repeat_time == 0) {
        eint_key_context.repeat_time = 1000;/*default 1s */
    }
    else {
        eint_key_context.repeat_time = config->repeat_time;
    }
    
    for(i = 0; i<BSP_EINT_KEY_NUMBER; i++){
        #ifdef EINT_KEY_USED_EPT_CONFIGURATION
        if(eint_key_mapping[i].gpio_port == 0xff) {
            continue;       
        }
        #endif
        count++;
        eint_key_context.index[i] = i;

        #ifdef EINT_KEY_HAL_MASK
        hal_eint_mask(eint_key_mapping[i].eint_number); 
        #endif
        
        #ifndef EINT_KEY_USED_EPT_CONFIGURATION
        hal_gpio_init(eint_key_mapping[i].gpio_port);
        hal_pinmux_set_function(eint_key_mapping[i].gpio_port, eint_key_mapping[i].eint_mode);
        hal_gpio_set_direction(eint_key_mapping[i].gpio_port, HAL_GPIO_DIRECTION_INPUT);
        /*The board has a pull-up resistor, if no external pull-up resistor ,please set to pull up*/
        hal_gpio_disable_pull(eint_key_mapping[i].gpio_port); 
        #endif
        
        eint_config.debounce_time = 10;
        eint_config.trigger_mode  = HAL_EINT_EDGE_FALLING_AND_RISING;
        hal_eint_init(eint_key_mapping[i].eint_number, &eint_config);
        
        hal_eint_register_callback(eint_key_mapping[i].eint_number, (hal_eint_callback_t)bsp_eint_key_callback, (void*)(&eint_key_context.index[i]));
        
        eint_key_context.rtos_timer[i] = xTimerCreate(NULL, \
                                                  eint_key_context.longpress_time/portTICK_PERIOD_MS , \
                                                  pdFALSE, NULL, \
                                                  bsp_eint_key_timer_callback);
        
        log_hal_info("[eint_key][key%d]GPIO    = %d", i,eint_key_mapping[i].gpio_port);
        log_hal_info("[eint_key][key%d]eint    = %d", i,eint_key_mapping[i].eint_number);
        log_hal_info("[eint_key][key%d]key     = %d", i,eint_key_mapping[i].key_data);
        #if 0
        log_hal_info("[eint_key]eint mask status = %x\r\n", *(volatile uint32_t*)0xa2030320);
        log_hal_info("[eint_key]eint ack  status = %x\r\n", *(volatile uint32_t*)0xa2030300);
        #endif

    }

    #if 0
    log_hal_info("[eint_key]key valid num  = %d", count);
    log_hal_info("[eint_key]longpress time = %d ms", eint_key_context.longpress_time);
    log_hal_info("[eint_key]repeat    time = %d ms", eint_key_context.repeat_time);
    #endif

    return true;
    
}

bool bsp_eint_key_register_callback(bsp_eint_key_callback_t callback, void *user_data)
{
    if (callback == NULL) {
        log_hal_info("[eint_key]callback pioter is null\r\n");
        return false;
    }

    eint_key_context.callback  = callback;
    eint_key_context.user_data = user_data;

    return true;
}

bool bsp_eint_key_enable(void)
{
    uint32_t i;

    for(i = 0; i<BSP_EINT_KEY_NUMBER; i++){
#ifdef EINT_KEY_USED_EPT_CONFIGURATION
        if(eint_key_mapping[i].gpio_port == 0xff) {
            continue;       
        }
#endif

#ifdef EINT_KEY_HAL_MASK
        eint_ack_interrupt(eint_key_mapping[i].eint_number);
        hal_eint_unmask(eint_key_mapping[i].eint_number);   
#endif
    }
    
    log_hal_info("[eint_key]eint mask status = %x\r\n", *(volatile uint32_t*)0xa2030320);
    log_hal_info("[eint_key]eint ack  status = %x\r\n", *(volatile uint32_t*)0xa2030300);
    
    eint_key_context.is_init_ready =true;

    return true;

}

bool bsp_eint_key_set_event_time(bsp_eint_key_config_t *config) 
{
    if (config == NULL) {
        return false;
    }
    
    eint_key_context.longpress_time = config->longpress_time;
    eint_key_context.repeat_time    = config->repeat_time;
        
    return true;
}

bool bsp_eint_key_set_debounce_time(uint32_t debounce_time)
{
    uint32_t i;
    
     for(i = 0; i<BSP_EINT_KEY_NUMBER; i++){
        #ifdef EINT_KEY_USED_EPT_CONFIGURATION
        if(eint_key_mapping[i].gpio_port == 0xff) {
            continue;       
        }
        #endif
        hal_eint_set_debounce_time(eint_key_mapping[i].eint_number, debounce_time);
     }

     return true;

}


