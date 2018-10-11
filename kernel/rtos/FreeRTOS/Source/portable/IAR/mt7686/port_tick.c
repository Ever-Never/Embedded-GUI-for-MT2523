/*
    FreeRTOS V8.2.0 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM4F port.
 *----------------------------------------------------------*/

#include "FreeRTOS.h"
#include "port_tick.h"
#include "hal_clock.h"
#include "timers.h"

#if configUSE_TICKLESS_IDLE == 2
#include "task.h"
#include "memory_attribute.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#include "hal_sleep_manager_platform.h"
#include "core_cm4.h"
#include "hal_gpt.h"
#include "hal_rtc.h"
#include "hal_log.h"
#include "hal_eint.h"
#include "connsys_driver.h"
#include "hal_wdt.h"
#include "hal_rtc_internal.h"
#define RTC_FXO_IS_26M ((volatile uint8_t*)(0xA20202A3))
#endif

//#define TICKLESS_DEEBUG_ENABLE
#ifdef  TICKLESS_DEEBUG_ENABLE
#define log_debug(_message,...) printf(_message, ##__VA_ARGS__)
#else
#define log_debug(_message,...)
#endif

#define GIVE_OWN_BIT   1
#ifndef configSYSTICK_CLOCK_HZ
#define configSYSTICK_CLOCK_HZ configCPU_CLOCK_HZ
#endif

#if configUSE_TICKLESS_IDLE != 0
/*
 * The number of SysTick increments that make up one tick period.
 */
static uint32_t ulTimerCountsForOneTick = 0;

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
static uint32_t xMaximumPossibleSuppressedTicks = 0;
#endif

#if configUSE_TICKLESS_IDLE == 2
static uint32_t TimeStampSystick, TimeStampCounter, StoppedSystickCompensation;
void tickless_log_timestamp(void);
static void get_rtc_real_clock_freq(void);
TimerHandle_t timestamp_timer = NULL;

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
extern uint32_t eint_get_status(void);
extern uint32_t wakeup_source_status;
uint32_t wakeup_eint;
#endif

#define DEEP_SLEEP_HW_WAKEUP_TIME 2
#define DEEP_SLEEP_SW_BACKUP_RESTORE_TIME 8
extern uint32_t cli_dtim_sleep_mode;

/* RTC 32.768KHz Freq*/
float RTC_Freq;

#define MaximumIdleTime 20  //ms
void AST_vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    TickType_t xModifiableIdleTime;
    volatile static unsigned int ulAST_Reload_ms;
    static uint32_t ulCompleteTickPeriods, ulCompletedCountDecrements, ulCompletedTickDecrements;
    static uint32_t nowCount, nowTick;

    __asm volatile("cpsid i");

    StoppedSystickCompensation = (SysTick->VAL);
    //Calculate total idle time to ms
    ulAST_Reload_ms = (xExpectedIdleTime - 1) * (1000 / configTICK_RATE_HZ);
    ulAST_Reload_ms = ulAST_Reload_ms - DEEP_SLEEP_SW_BACKUP_RESTORE_TIME - DEEP_SLEEP_HW_WAKEUP_TIME;

    if (eTaskConfirmSleepModeStatus() == eAbortSleep) {

        SysTick->LOAD = SysTick->VAL;

        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

        SysTick->LOAD = ulTimerCountsForOneTick - 1UL;

        __asm volatile("cpsie i");

        return;
    } else {

        xModifiableIdleTime = xExpectedIdleTime;

        if (cli_dtim_sleep_mode == 2) {
            configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
            if (xModifiableIdleTime > 0) {
                printf("enter cli deep sleep\r\n");
                *SPM_WAKEUP_SOURCE_MASK = 0xFFFD;
                hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_SLEEP);
                cli_dtim_sleep_mode = 0;

                *SPM_WAKEUP_SOURCE_MASK = 0x80;
            }
            configPOST_SLEEP_PROCESSING(xExpectedIdleTime);
        } else {
            configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
            if (xModifiableIdleTime > 0) {
                hal_sleep_manager_set_sleep_time((uint32_t)ulAST_Reload_ms);
                hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_SLEEP);
            }
            configPOST_SLEEP_PROCESSING(xExpectedIdleTime);
        }

        //calculate time(systick) to jump
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &nowCount);
        nowTick = ((uint32_t)xTaskGetTickCount() * (1000 / configTICK_RATE_HZ));
        // get counter distance from last record
        if (nowCount >= TimeStampCounter) {
            ulCompletedCountDecrements = nowCount - TimeStampCounter;
        } else {
            ulCompletedCountDecrements = nowCount + (0xFFFFFFFF - TimeStampCounter);
        }
        // get systick distance from last record
        if (nowTick >= TimeStampSystick) {
            ulCompletedTickDecrements = nowTick - TimeStampSystick;
        } else {
            ulCompletedTickDecrements = nowTick + (0xFFFFFFFF - TimeStampSystick);
        }
        // get counter distance for this sleep
        ulCompletedCountDecrements = (unsigned int)(((float)ulCompletedCountDecrements) - ((float)ulCompletedTickDecrements * RTC_Freq));
        // calculate ticks for jumpping
        ulCompleteTickPeriods = ((unsigned int)(((float)ulCompletedCountDecrements) / RTC_Freq)) / ((1000 / configTICK_RATE_HZ));

        //Limit OS Tick Compensation Value
        if (ulCompleteTickPeriods > (xExpectedIdleTime - 1)) {
            ulCompleteTickPeriods = xExpectedIdleTime - 1;
        }

        SysTick->LOAD = StoppedSystickCompensation;
        SysTick->VAL = 0UL;
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
        vTaskStepTick(ulCompleteTickPeriods);
        SysTick->LOAD = ulTimerCountsForOneTick - 1UL;
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
        wakeup_eint = eint_get_status();
#endif
        __asm volatile("cpsie i");

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
#ifdef  SLEEP_MANAGEMENT_DEBUG_SLEEP_WAKEUP_LOG_ENABLE
        sleep_management_dump_wakeup_source(wakeup_source_status, wakeup_eint);
#endif
#endif
        log_debug("\r\nEIT=%u\r\n"  , (unsigned int)xExpectedIdleTime);
        log_debug("CSD=%d\r\n"      , (unsigned int)ulCompleteTickPeriods);
        log_debug("RL=%u\r\n"       , (unsigned int)ulAST_Reload_ms);
    }
}


void tickless_handler(uint32_t xExpectedIdleTime)
{
    static long unsigned int ulReloadValue;
    static long unsigned int xModifiableIdleTime;
    static long unsigned int before_sleep_time, after_sleep_time, sleep_time = 0;
    static int timestamp_diff = 0;
    static uint32_t ulCompleteTickPeriods, ulCompleteTickPeriods_timestamp, ulCompletedCountDecrements, ulCompletedTickDecrements;
    static uint32_t nowCount, nowTick, skip_time = 0;

    /* Enter a critical section but don't use the taskENTER_CRITICAL()
    method as that will mask interrupts that should exit sleep mode. */
    __asm volatile("cpsid i");

    /* Stop the SysTick momentarily.  */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&before_sleep_time);

    if ((xExpectedIdleTime > (MaximumIdleTime / (1000 / configTICK_RATE_HZ))) && (hal_sleep_manager_is_sleep_locked() == 0)) {
        AST_vPortSuppressTicksAndSleep(xExpectedIdleTime);
        timestamp_diff = 0;
        return;
    }
    
    /* Make sure the SysTick reload value does not overflow the counter. */
    if (xExpectedIdleTime > (xMaximumPossibleSuppressedTicks)) {
        xExpectedIdleTime = (xMaximumPossibleSuppressedTicks);
    }   
  
    /*disable wdt*/
#ifdef MTK_SYSTEM_HANG_CHECK_ENABLE
    hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
#endif
    /* If a context switch is pending or a task is waiting for the scheduler
    to be unsuspended then abandon the low power entry. */
    if (eTaskConfirmSleepModeStatus() == eAbortSleep) {
        /* Restart from whatever is left in the count register to complete
        this tick period. */
        SysTick->LOAD = SysTick->VAL;

        /* Restart SysTick. */
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

        /* Reset the reload register to the value required for normal tick
        periods. */
        SysTick->LOAD = ulTimerCountsForOneTick - 1UL;

        /* enable wdt*/
#ifdef MTK_SYSTEM_HANG_CHECK_ENABLE
        hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
#endif
        /* Re-enable interrupts - see comments above the cpsid instruction()
        above. */
        __asm volatile("cpsie i");
    } else {
        if ((timestamp_diff < 0) && (xExpectedIdleTime > 2)) {
            ulReloadValue = ((xExpectedIdleTime - 2UL) * (1000 / configTICK_RATE_HZ));
            skip_time = 1;
        } else {
            ulReloadValue = ((xExpectedIdleTime - 1UL) * (1000 / configTICK_RATE_HZ));
        }
        hal_sleep_manager_set_sleep_time((uint32_t)ulReloadValue);

        xModifiableIdleTime = xExpectedIdleTime;
        configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
        if (xModifiableIdleTime > 0) {
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_IDLE);
        }
        configPOST_SLEEP_PROCESSING(xExpectedIdleTime);

        //calculate time(systick) to jump
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &nowCount);
        nowTick = ((uint32_t)xTaskGetTickCount() * (1000 / configTICK_RATE_HZ));

        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&after_sleep_time);

        if (after_sleep_time >= before_sleep_time) {
            sleep_time = after_sleep_time - before_sleep_time;
        } else {
            sleep_time = after_sleep_time + (0xFFFFFFFF - before_sleep_time);
        }

        ulCompleteTickPeriods = (sleep_time / 1000) / (1000 / configTICK_RATE_HZ);

        // get counter distance from last record
        if (nowCount >= TimeStampCounter) {
            ulCompletedCountDecrements = nowCount - TimeStampCounter;
        } else {
            ulCompletedCountDecrements = nowCount + (0xFFFFFFFF - TimeStampCounter);
        }

        // get systick distance from last record
        if (nowTick >= TimeStampSystick) {
            ulCompletedTickDecrements = nowTick - TimeStampSystick;
        } else {
            ulCompletedTickDecrements = nowTick + (0xFFFFFFFF - TimeStampSystick);
        }

        // get counter distance for this sleep
        ulCompletedCountDecrements = (unsigned int)(((float)ulCompletedCountDecrements) - ((float)ulCompletedTickDecrements * RTC_Freq));
        // calculate ticks for jumpping
        ulCompleteTickPeriods_timestamp = ((unsigned int)(((float)ulCompletedCountDecrements) / RTC_Freq)) / ((1000 / configTICK_RATE_HZ));

        if (skip_time > 0) {
            skip_time = 0;
            ulCompleteTickPeriods++;
        }

        if (ulCompleteTickPeriods_timestamp > ulCompleteTickPeriods) {
            ulCompleteTickPeriods++;
        } else {
            if ((ulCompleteTickPeriods_timestamp < ulCompleteTickPeriods) && (ulCompleteTickPeriods > 2)) {
                ulCompleteTickPeriods--;
            }
        }

        //Limit OS Tick Compensation Value
        if (ulCompleteTickPeriods >= (xExpectedIdleTime)) {
            ulCompleteTickPeriods = xExpectedIdleTime;
        }
        timestamp_diff = ulCompleteTickPeriods - ulCompleteTickPeriods_timestamp;

        SysTick->LOAD = ulTimerCountsForOneTick - 1UL;
        SysTick->VAL = 0UL;
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
        vTaskStepTick(ulCompleteTickPeriods);
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

        /* enable wdt*/
#ifdef MTK_SYSTEM_HANG_CHECK_ENABLE
        hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
#endif
        /* Re-enable interrupts - see comments above the cpsid instruction() above. */
        __asm volatile("cpsie i");

        log_debug("\r\nST_CPT=%u,%u\r\n"   , (unsigned int)ulCompleteTickPeriods, (unsigned int)xExpectedIdleTime);
    }
}

static void tickless_log_timestamp_callback(TimerHandle_t expiredTimer)
{
    static unsigned char change = 0;
    
    if(change == 0){
        change = 1;
        xTimerChangePeriod( timestamp_timer, 1000 * 60 * 60 * 12 / portTICK_PERIOD_MS, 0 ); //12hours
    }
    
    tickless_log_timestamp();
}

void tickless_log_timestamp()
{    
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &TimeStampCounter);
    TimeStampSystick = (uint32_t)xTaskGetTickCount() * (1000 / configTICK_RATE_HZ);
    get_rtc_real_clock_freq(); 
}

static void get_rtc_real_clock_freq(void)
{
    #define windows_cnt 1999
    
    uint32_t rtc_count,now=0,last=0;    
   
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &last);	    
    rtc_count = f32k_measure_count(1, 1, windows_cnt); 
    if(*RTC_FXO_IS_26M == 1) {
        /* use 26MHz xtal */
        RTC_Freq = (float)rtc_count;    
        RTC_Freq = (26000000.f/RTC_Freq)*(windows_cnt + 1.f);
        RTC_Freq /= 1000;                       
    } else {
        /* use 40MHz xtal */
        RTC_Freq = (float)rtc_count;    
        RTC_Freq = (20000000.f/RTC_Freq)*(windows_cnt + 1.f);
        RTC_Freq /= 1000;          
    }
        
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &now);	
    
    #if 0
    printf("***************************************************\r\n");
    printf("f32k_measure_count:%d,%d,%d\r\n",(unsigned int)(RTC_Freq*100000),rtc_count,now-last);
    printf("***************************************************\r\n");      
    #endif    
}
#endif

void vPortSetupTimerInterrupt(void)
{
#if configUSE_TICKLESS_IDLE == 2
    static int init_timestamp = 0;
    if (init_timestamp == 0) {
        init_timestamp = 1;

        tickless_log_timestamp();
        timestamp_timer = xTimerCreate("timestamp_timer",
                                       1000 * 3 / portTICK_PERIOD_MS, 
                                       true,
                                       NULL,
                                       tickless_log_timestamp_callback);        
                                       
        if (timestamp_timer == NULL) {
            printf("timestamp_timer create fail\n");
        } else {
            if (xTimerStart(timestamp_timer, 0) != pdPASS) {
                printf("timestamp_timer xTimerStart fail\n");
            }
        }
    }
#endif /* configUSE_TICKLESS_IDLE */

#if configUSE_TICKLESS_IDLE != 0
    /* Calculate the constants required to configure the tick interrupt. */
    {
        ulTimerCountsForOneTick = (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ);
        xMaximumPossibleSuppressedTicks = SysTick_LOAD_RELOAD_Msk / ulTimerCountsForOneTick;
    }
#endif /* configUSE_TICKLESS_IDLE  != 0*/

    /* Configure SysTick to interrupt at the requested rate. */
    SysTick->LOAD = (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ) - 1UL;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}
