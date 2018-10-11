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

#include "hal_gpt.h"

#ifdef HAL_GPT_MODULE_ENABLED
#include "hal_gpt_internal.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_log.h"
#include "memory_attribute.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include <assert.h>


ATTR_RWDATA_IN_TCM GPT_REGISTER_T *gp_gpt[HAL_GPT_MAX_PORT] = {GPT0, GPT1, GPT2, GPT3, GPT4, GPT5};
GPT_REGISTER_GLOABL_T   *gp_gpt_glb                                     = {GPTGLB};

ATTR_ZIDATA_IN_TCM gpt_context_t g_gpt_context[HAL_GPT_MAX_PORT];
gpt_sw_context_t        gpt_sw_context;
ATTR_ZIDATA_IN_TCM uint32_t gpt_clock_source;


ATTR_TEXT_IN_TCM uint32_t gpt_calibrate_algorithm(uint32_t mul1, uint32_t mul2, uint32_t div)
{
    uint32_t data1[2];
    uint32_t data2[2];
    uint32_t result[2];

    /* algorithm = (mul1*mul2)/div */
    data1[0] = (mul1 & 0xffff) * mul2;
    data1[1] = ((mul1 >> 16) & 0xffff) * mul2;

    data2[0]  =  data1[0] & 0xffff;
    data2[1] = ((data1[0] >> 16) & 0xffff) + data1[1];

    result[1] = data2[1] / div;
    result[0] = (((data2[1] % div) << 16) + data2[0]) / div;

    return (uint32_t)((result[1] << 16) + result[0]);
}

ATTR_TEXT_IN_TCM uint32_t gpt_current_count(GPT_REGISTER_T *gpt)
{
    return gpt->GPT_COUNT;

}

ATTR_TEXT_IN_TCM uint32_t gpt_convert_ms_to_32k_count(uint32_t ms)
{
    return ((uint32_t)(ms * 32 + (7 * ms) / 10 + (6 * ms) / 100 + (8 * ms) / 1000));
}

ATTR_TEXT_IN_TCM void gpt_get_lfosc_clock(void)
{
    ATTR_RWDATA_IN_TCM static bool flag = false;

    if (flag == false) {
        if (is_clk_use_lfosc() == true) {
            gpt_clock_source = clock_get_freq_lfosc();
            if ((gpt_clock_source == 0) || (gpt_clock_source == GPT_CLOCK_26M_SOURCE)) {
                gpt_clock_source =  GPT_CLOCK_26M_SOURCE;
                flag = false;
            } else {
                flag = true;
            }
        } else {
            gpt_clock_source = GPT_CLOCK_26M_SOURCE; /*the unit of gpt_clock_source is Khz*/
            flag = true;
        }
    }

}

ATTR_TEXT_IN_TCM void gpt_open_clock_source(void)
{
    if (hal_clock_is_enabled(HAL_CLOCK_CG_GPTIMER) != true) {
        hal_clock_enable(HAL_CLOCK_CG_GPTIMER);
    }
}

ATTR_TEXT_IN_TCM uint32_t gpt_clock_calibrate_us_to_count(uint32_t time_us)
{
    gpt_get_lfosc_clock();

    if (is_clk_use_lfosc() == true) {
        /* calibration_count = (time_us*gpt_clock_source)/26000 */
        return gpt_calibrate_algorithm(time_us, gpt_clock_source, GPT_CLOCK_26M_SOURCE);
    } else {
        return time_us;
    }

}

ATTR_TEXT_IN_TCM uint32_t gpt_clock_calibrate_count_to_us(uint32_t count)
{
    gpt_get_lfosc_clock();

    if (is_clk_use_lfosc() == true) {
        /* calibration_time_us = (count*26000)/gpt_clock_source */
        return gpt_calibrate_algorithm(count, GPT_CLOCK_26M_SOURCE, gpt_clock_source);
    } else {
        return count;
    }

}


ATTR_TEXT_IN_TCM void  gpt_start_free_run_timer(GPT_REGISTER_T *gpt, uint32_t clock_source, uint32_t divide)
{
    gpt_open_clock_source();

    gpt->GPT_CLK = clock_source | divide;

    /* set to free run mode, open clock source and start counter */
    gpt->GPT_CON = (uint32_t)GPT_CLOCK_UNGATE | (uint32_t)GPT_MODE_FREE_RUN | GPT_COUNT_START;
}

ATTR_TEXT_IN_TCM void gpt_delay_time(GPT_REGISTER_T *gpt, const uint32_t count)
{
    uint32_t end_count = 0, current = 0;

    current   = gpt_current_count(gpt);
    end_count = current + count + 1;

    /* loop to wait time pass */
    if (end_count > current) {
        while (gpt_current_count(gpt) < end_count);     /* current time < end_time */
    } else {                                            /* current time > end time */
        while (gpt_current_count(gpt) >= current);      /* wait time roll back */
        while (gpt_current_count(gpt) < end_count);     /* wait time reach end_time */
    }
}
void gpt_reset_default_timer(GPT_REGISTER_T *gpt)
{
    gpt->GPT_IRQ_EN  = 0;               /* disable interrupt */
    gpt->GPT_CON     = 0;               /* disable timer     */
    gpt->GPT_CON    |= GPT_COUNT_CLEAR; /* clear counter value */
    while(gpt->GPT_COUNT);
    gpt->GPT_CLK     = GPT_CLOCK_32KHZ; /* default 32Mhz, divide 1 */
    gpt->GPT_IRQ_ACK = GPT_IRQ_FLAG_ACK;/* clear interrupt status */
    gpt->GPT_COMPARE = 0xffffffff;      /* set max threshold value */

    if (gpt == GPT5) {
        gpt->GPT_COMPAREH = 0;
    }
}

uint32_t gpt_save_and_mask_interrupt(GPT_REGISTER_T *gpt)
{
    volatile uint32_t mask;

    mask = gpt->GPT_IRQ_EN;

    gpt->GPT_IRQ_EN  &= ~GPT_IRQ_ENABLE;

    return mask;
}

void gpt_restore_interrupt(GPT_REGISTER_T *gpt, uint32_t mask)
{
    gpt->GPT_IRQ_EN = mask;
}

void gpt_interrupt_handler(hal_nvic_irq_t irq_number)
{
    uint32_t i;
    volatile uint32_t mask;
    volatile uint32_t state;
    volatile uint32_t enable;


    irq_number = irq_number;

    for (i = 0; i < HAL_GPT_MAX_PORT; i++) {
        /* get gpt irq status */
        state  = gp_gpt[i]->GPT_IRQ_STA;
        enable = gp_gpt[i]->GPT_IRQ_EN;

        mask = gpt_save_and_mask_interrupt(gp_gpt[i]);

        gp_gpt[i]->GPT_IRQ_ACK = GPT_IRQ_FLAG_ACK;

        if ((state & GPT_IRQ_FLAG_STA) && (enable & GPT_IRQ_ENABLE)) {
            /* clear interrupt status */
#ifdef GPT_DEBUG_LOG
            if (i != HAL_GPT_SW_PORT) {
                //    log_hal_info("[GPT%d]GPT_IRQ_STA = 0x%x\r\n", (int)i, (int)gp_gpt[i]->GPT_IRQ_STA);
            }
#endif

            if (g_gpt_context[i].callback_context.callback != NULL) {
                g_gpt_context[i].callback_context.callback(g_gpt_context[i].callback_context.user_data);
            }

        }
        gpt_restore_interrupt(gp_gpt[i], mask);

    }

}


void gpt_nvic_register(void)
{
    hal_nvic_disable_irq(GPTimer_IRQn);
    hal_nvic_register_isr_handler(GPTimer_IRQn, gpt_interrupt_handler);
    hal_nvic_enable_irq(GPTimer_IRQn);
}
uint32_t gpt_sw_get_current_time_ms(GPT_REGISTER_T *gpt)
{
    volatile uint32_t time;
    uint32_t count,time_s,time_ms;

    count = gpt_current_count(gpt);
    time_s = count / 32768;
    time_ms = ((count % 32768) * 1000 + 16384) / 32768;
    time = time_s * 1000 + time_ms;

    /*clear count*/
    gp_gpt[HAL_GPT_SW_PORT]->GPT_CON |=  GPT_COUNT_CLEAR;
    while(gp_gpt[HAL_GPT_SW_PORT]->GPT_COUNT);
    return time;
}

uint32_t gpt_sw_get_free_timer(void)
{
    uint32_t i;

    for (i = 0; i < HAL_GPT_SW_NUMBER; i++) {
        if (gpt_sw_context.timer[i].is_used != true) {
            return i;
        }
    }

    return HAL_GPT_SW_NUMBER;
}

void gpt_sw_start_timer(void)
{
    uint32_t minimum_time, current_timer;

    if (gpt_sw_context.is_start_from_isr == true) {
        return;
    }

    gpt_sw_get_minimum_left_time_ms(&minimum_time, &current_timer);

    /*restore this absolute time*/
    gpt_sw_context.last_absolute_time = gpt_sw_context.absolute_time;

    gpt_sw_context.is_sw_gpt = true;
    hal_gpt_start_timer_ms(HAL_GPT_SW_PORT, minimum_time, HAL_GPT_TIMER_TYPE_ONE_SHOT);
    gpt_sw_context.is_sw_gpt = false;
}

uint32_t gpt_sw_absolute_value(uint32_t timer_number)
{
    uint32_t time_out_delta;
    uint32_t current_delta;
    uint32_t ret_value;

    time_out_delta = gpt_sw_delta(gpt_sw_context.timer[timer_number].time_out_ms, gpt_sw_context.last_absolute_time);
    current_delta  = gpt_sw_delta(gpt_sw_context.absolute_time, gpt_sw_context.last_absolute_time);

    if (time_out_delta > current_delta) {
        /*timer has not been expired*/
        ret_value = gpt_sw_delta(gpt_sw_context.timer[timer_number].time_out_ms, gpt_sw_context.absolute_time);
    } else {
        /*timer has been expired*/
        ret_value =  0;
    }

    return ret_value;
}
void gpt_sw_get_minimum_left_time_ms(uint32_t *minimum_time, uint32_t *number)
{
    uint32_t i;
    uint32_t minimum = 0xffffffff;
    uint32_t data;

    for (i = 0; i < HAL_GPT_SW_NUMBER; i++) {
        if (gpt_sw_context.timer[i].is_running == true) {
            data = gpt_sw_absolute_value(i);

            if (data < minimum) {
                minimum = data;
                *number = i;
            }
        }
    }

    *minimum_time = minimum;
}


void gpt_sw_handler(void *parameter)
{
    uint32_t i;
    volatile uint32_t time_ms;
    volatile uint32_t data;


    if (gpt_sw_context.used_timer_count != 0) {

        gpt_sw_context.is_sw_gpt = true;
        hal_gpt_stop_timer(HAL_GPT_SW_PORT);
        gpt_sw_context.is_sw_gpt = false;

        time_ms = gpt_sw_get_current_time_ms(gp_gpt[HAL_GPT_SW_PORT]);

        gpt_sw_context.absolute_time += time_ms;
        gpt_sw_context.is_start_from_isr = true;

        for (i = 0; i < HAL_GPT_SW_NUMBER; i++) {
            if (gpt_sw_context.timer[i].is_running == true) {

                data = gpt_sw_absolute_value(i);

                if (data == 0) {
                    gpt_sw_context.timer[i].is_running = false;
                    gpt_sw_context.running_timer_count--;

                    gpt_sw_context.timer[i].callback_context.callback(gpt_sw_context.timer[i].callback_context.user_data);
                }

            }
        }

        gpt_sw_context.is_start_from_isr = false;

        if (gpt_sw_context.running_timer_count != 0) {
            gpt_sw_start_timer();
        }
    }

}

#endif /* HAL_GPT_MODULE_ENABLED */


