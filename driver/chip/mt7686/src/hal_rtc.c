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

#include "hal_rtc.h"

#if defined(HAL_RTC_MODULE_ENABLED)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal_rtc_internal.h"
#include "hal_eint.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "hal_platform.h"
#include "hal_pmu.h"
#include "hal_pmu_wrap_interface.h"
#include "hal_nvic.h"


#define RTC_32K_DEBUG       0
//#define RTC_LOG_DEBUG       0
#ifdef RTC_LOG_DEBUG
#define RTC_LOG_INFO(_message,...)            log_hal_info(_message, ##__VA_ARGS__)
#define RTC_LOG_WARNING(_message,...)         log_hal_warning(_message, ##__VA_ARGS__)
#define RTC_LOG_ERROR(_message,...)           log_hal_error(_message, ##__VA_ARGS__)
#else
#define RTC_LOG_INFO(_message,...)
#define RTC_LOG_WARNING(_message,...)
#define RTC_LOG_ERROR(_message,...)
#endif



static void rtc_wait_busy(void);
static void rtc_unlock_PROT(void);
static void rtc_write_trigger(void);
static void rtc_write_trigger_wait(void);
static void rtc_reload(void);
#ifdef RTC_LOG_DEBUG
static uint32_t rtc_read_osc32con0(void);
#endif
static void rtc_write_osc32con0(uint32_t value, uint32_t operation_type);
static uint32_t rtc_read_osc32con1(void);
static void rtc_write_osc32con1(uint32_t value);
#ifdef RTC_LOG_DEBUG
static uint32_t rtc_read_osc32con2(void);
#endif
static void rtc_write_osc32con2(uint32_t value, uint32_t operation_type);
static void rtc_lpd_init(void);
static void rtc_set_power_key(void);
static void rtc_set_xosc_mode(void);
static bool rtc_is_time_valid(const hal_rtc_time_t *time);
static void rtc_eint_callback(void *user_data);
static void rtc_register_init(void);
static void rtc_init_eint(void);
static void rtc_dump_register(char *tag);
//static void rtc_check_alarm_power_on(void);
static void f32k_eosc32_calibration(void);
static void f32k_osc32_Init(void);
uint32_t f32k_measure_clock(uint32_t fixed_clock, uint32_t tested_clock, uint32_t window_setting);
bool rtc_is_back_from_retention(void);

#ifdef HAL_RTC_FEATURE_TIME_CALLBACK
static hal_rtc_time_callback_t rtc_time_callback_function;
static void *rtc_time_user_data;
#endif
static hal_rtc_alarm_callback_t rtc_alarm_callback_function;
static void *rtc_alarm_user_data;
static bool rtc_init_done = false;
static bool rtc_in_test = false;
static char rtc_spare_register_backup[HAL_RTC_BACKUP_BYTE_NUM_MAX];
static bool rtc_fxo_is_26m = true;
bool use_xosc = false;
bool retention_flag = false;


RTC_REGISTER_T *rtc_register = (RTC_REGISTER_T *)RTC_BASE;
ABIST_FQMTR_REGISTER_T *abist_fqmtr_register = (ABIST_FQMTR_REGISTER_T *)ABIST_FQMTR_BASE;

struct rtc_spare_register_information {
    uint8_t *address;
};

static struct rtc_spare_register_information rtc_spare_register_table[HAL_RTC_BACKUP_BYTE_NUM_MAX] = {
    {(uint8_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_NEW_SPAR0_UNION.RTC_NEW_SPAR0_CELLS.RTC_NEW_SPAR0_0)},
    {(uint8_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_NEW_SPAR0_UNION.RTC_NEW_SPAR0_CELLS.RTC_NEW_SPAR0_1)},
    {(uint8_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_PDN0_UNION.RTC_PDN0_CELLS.RTC_PDN0_0)},
    {(uint8_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_PDN0_UNION.RTC_PDN0_CELLS.RTC_PDN0_1)},
    {(uint8_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_PDN1_UNION.RTC_PDN1_CELLS.RTC_PDN1_0)},
    {(uint8_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_PDN1_UNION.RTC_PDN1_CELLS.RTC_PDN1_1)},
    {(uint8_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_SPAR0_UNION.RTC_SPAR0_CELLS.RTC_SPAR0_0)},
    {(uint8_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_SPAR0_UNION.RTC_SPAR0_CELLS.RTC_SPAR0_1)},
    {(uint8_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_SPAR1_UNION.RTC_SPAR1_CELLS.RTC_SPAR1_0)},
    {(uint8_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_SPAR1_UNION.RTC_SPAR1_CELLS.RTC_SPAR1_1)}
};

#define FQMTR_FCKSEL_RTC_32K            1
#define FQMTR_FCKSEL_EOSC_F32K_CK       4
#define FQMTR_FCKSEL_DCXO_F32K_CK       5
#define FQMTR_FCKSEL_XOSC_F32K_CK       6

#define FQMTR_TCKSEL_XO_CK      1

static void rtc_wait_busy(void)
{
    uint32_t count = 0;

    while (count < 0x6EEEEE) {
        if ((RTC_CBUSY_MASK & rtc_register->RTC_WRTGR_UNION.RTC_WRTGR_CELLS.RTC_STA) == 0) {
            break;
        }
        count++;
    }

    if (count >= 0x6EEEEE) {
        RTC_LOG_ERROR("rtc_wait_busy timeout, RTC_BBPU = %x!\r\n", rtc_register->RTC_BBPU);
        RTC_LOG_ERROR("rtc_wait_busy timeout, RTC_32K = %u, EOSC = %u, DCXO = %u, XOSC = %u\r\n",
                      (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_RTC_32K, FQMTR_TCKSEL_XO_CK, 99),
                      (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_EOSC_F32K_CK, FQMTR_TCKSEL_XO_CK, 99),
                      (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_DCXO_F32K_CK, FQMTR_TCKSEL_XO_CK, 99),
                      (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_XOSC_F32K_CK, FQMTR_TCKSEL_XO_CK, 99));
    }
}

static void rtc_unlock_PROT(void)
{
    rtc_register->RTC_PROT = RTC_PROTECT1;
    rtc_write_trigger_wait();

    rtc_register->RTC_PROT = RTC_PROTECT2;
    rtc_write_trigger_wait();
}

static void rtc_write_trigger(void)
{
    rtc_register->RTC_WRTGR_UNION.RTC_WRTGR_CELLS.WRTGR = RTC_WRTGR_MASK;
}

static void rtc_write_trigger_wait(void)
{
    rtc_write_trigger();
    rtc_wait_busy();
}

static void rtc_reload(void)
{
    rtc_register->RTC_BBPU = RTC_KEY_BBPU_RELOAD;
    rtc_write_trigger_wait();
}
#ifdef RTC_LOG_DEBUG
static uint32_t rtc_read_osc32con0(void)
{
    rtc_reload();
    return rtc_register->RTC_OSC32CON0_UNION.RTC_OSC32CON0;
}
#endif
static void rtc_write_osc32con0(uint32_t value, uint32_t operation_type)
{
    rtc_register->RTC_OSC32CON0_UNION.RTC_OSC32CON0 = RTC_OSC32CON0_MAGIC_KEY_1;
    rtc_wait_busy();
    rtc_register->RTC_OSC32CON0_UNION.RTC_OSC32CON0 = RTC_OSC32CON0_MAGIC_KEY_2;
    rtc_wait_busy();
    switch(operation_type) {
        case RTC_LOW_BYTE_OPERATION:
            rtc_register->RTC_OSC32CON0_UNION.RTC_OSC32CON0_CELLS.RTC_32K_SEL = value;
            break;
        case RTC_HIGH_BYTE_OPERATION:
            rtc_register->RTC_OSC32CON0_UNION.RTC_OSC32CON0_CELLS.RTC_TIMER_CG_EN = value;
            break;
        case RTC_WORD_OPERATION:
            rtc_register->RTC_OSC32CON0_UNION.RTC_OSC32CON0 = value;
            break;
        default:
            break;
    }
    rtc_wait_busy();
}

static uint32_t rtc_read_osc32con1(void)
{
    rtc_reload();
    return rtc_register->RTC_OSC32CON1;
}

static void rtc_write_osc32con1(uint32_t value)
{
    rtc_register->RTC_OSC32CON1 = RTC_OSC32CON1_MAGIC_KEY_1;
    rtc_wait_busy();
    rtc_register->RTC_OSC32CON1 = RTC_OSC32CON1_MAGIC_KEY_2;
    rtc_wait_busy();
    rtc_register->RTC_OSC32CON1 = value;
    rtc_wait_busy();
}
#ifdef RTC_LOG_DEBUG
static uint32_t rtc_read_osc32con2(void)
{
    rtc_reload();
    return rtc_register->RTC_OSC32CON2_UNION.RTC_OSC32CON2;
}
#endif
static void rtc_write_osc32con2(uint32_t value, uint32_t operation_type)
{
    rtc_register->RTC_OSC32CON2_UNION.RTC_OSC32CON2 = RTC_OSC32CON2_MAGIC_KEY_1;
    rtc_wait_busy();
    rtc_register->RTC_OSC32CON2_UNION.RTC_OSC32CON2 = RTC_OSC32CON2_MAGIC_KEY_2;
    rtc_wait_busy();
    switch(operation_type) {
        case RTC_LOW_BYTE_OPERATION:
            rtc_register->RTC_OSC32CON2_UNION.RTC_OSC32CON2_CELLS.OSC32_PW_DB = value;
            break;
        case RTC_HIGH_BYTE_OPERATION:
            rtc_register->RTC_OSC32CON2_UNION.RTC_OSC32CON2_CELLS.SRAM_ISO_EN = value;
            break;
        case RTC_WORD_OPERATION:
            rtc_register->RTC_OSC32CON2_UNION.RTC_OSC32CON2 = value;
            break;
        default:
            break;
    }
    rtc_wait_busy();
}

static uint32_t rtc_read_sramcon(void)
{
    rtc_reload();
    return rtc_register->RTC_SRAM_CON_UNION.RTC_SRAM_CON;
}

static void rtc_write_sramcon(uint32_t value, uint32_t operation_type)
{
    rtc_register->RTC_SRAM_CON_UNION.RTC_SRAM_CON = RTC_SRAMCON_MAGIC_KEY_1;
    rtc_wait_busy();
    rtc_register->RTC_SRAM_CON_UNION.RTC_SRAM_CON = RTC_SRAMCON_MAGIC_KEY_2;
    rtc_wait_busy();
    switch(operation_type) {
        case RTC_LOW_BYTE_OPERATION:
            rtc_register->RTC_SRAM_CON_UNION.RTC_SRAM_CON_CELLS.SRAM_PD = value;
            break;
        case RTC_HIGH_BYTE_OPERATION:
            rtc_register->RTC_SRAM_CON_UNION.RTC_SRAM_CON_CELLS.SRAM_SLEEPB = value;
            break;
        case RTC_WORD_OPERATION:
            rtc_register->RTC_SRAM_CON_UNION.RTC_SRAM_CON = value;
            break;
        default:
            break;
    }
    rtc_wait_busy();
}
static void rtc_lpd_init(void)
{
    rtc_register->RTC_LPD_CON_UNION.RTC_LPD_CON_CELLS.LPD_CON = RTC_EOSC32_LPEN_MASK;
    rtc_write_trigger_wait();

    rtc_register->RTC_LPD_CON_UNION.RTC_LPD_CON_CELLS.LPD_CON = RTC_EOSC32_LPEN_MASK | RTC_LPRST_MASK;
    rtc_write_trigger_wait();

    /* designer suggests delay at least 1 ms */
    hal_gpt_delay_us(1000);

    rtc_register->RTC_LPD_CON_UNION.RTC_LPD_CON_CELLS.LPD_CON = RTC_EOSC32_LPEN_MASK;
    rtc_write_trigger_wait();

    if ((rtc_register->RTC_LPD_CON_UNION.RTC_LPD_CON & RTC_LPSTA_RAW_MASK) != 0) {
        rtc_dump_register("rtc_lpd_init fail\r\n");
    }
}
static void rtc_set_power_key(void)
{
    rtc_dump_register("Before Set Power Key");
    /* Set powerkey0 and powerkey1 */
    rtc_register->RTC_POWERKEY0 = RTC_POWERKEY0_KEY;
    rtc_register->RTC_POWERKEY1 = RTC_POWERKEY1_KEY;
    rtc_write_trigger_wait();

    /* Inicialize LPD */
    rtc_lpd_init();

    /* Set powerkey0 and powerkey1 */
    rtc_register->RTC_POWERKEY0 = RTC_POWERKEY0_KEY;
    rtc_register->RTC_POWERKEY1 = RTC_POWERKEY1_KEY;
    rtc_write_trigger_wait();

    rtc_reload();
    if ((rtc_register->RTC_WRTGR_UNION.RTC_WRTGR_CELLS.RTC_STA & RTC_POWER_DETECTED_MASK) == 0) {
        RTC_LOG_ERROR("rtc_set_power_key fail : rtc_wrtgr = %x!\r\n", rtc_register->RTC_WRTGR_UNION.RTC_WRTGR);
    }

    rtc_dump_register("After Set Power Key");

    /* check fxo_clk */
    rtc_fxo_is_26m = (*RTC_FXO_IS_26M == 1) ? true : false;
}

static void rtc_set_xosc_mode(void)
{
    uint32_t value;
    // XOSC init setting: OSC32CON1[6:5] = 0x0 -> 0x3
    value = rtc_read_osc32con1();
    value |= (RTC_AMP_GSEL_MASK | RTC_AMPCTL_EN_MASK);
    rtc_write_osc32con1(value);
}

static bool rtc_is_time_valid(const hal_rtc_time_t *time)
{
    bool result = true;

    if (time->rtc_year > 127) {
        RTC_LOG_ERROR("Invalid year : %d\r\n", time->rtc_year);
        result = false;
    }

    if ((time->rtc_mon == 0) || (time->rtc_mon > 12)) {
        RTC_LOG_ERROR("Invalid month : %d\r\n", time->rtc_mon);
        result = false;
    }
    //MT7686 day-of-week range is 1~7;
    if ((time->rtc_week - 1) > 6) {
        RTC_LOG_ERROR("Invalid day of week : %d\r\n", time->rtc_week);
    }

    if ((time->rtc_day == 0) || (time->rtc_day > 31)) {
        RTC_LOG_ERROR("Invalid day of month : %d\r\n", time->rtc_day);
        result = false;
    }

    if (time->rtc_hour > 23) {
        RTC_LOG_ERROR("Invalid hour : %d\r\n", time->rtc_hour);
        result = false;
    }

    if (time->rtc_min > 59) {
        RTC_LOG_ERROR("Invalid minute : %d\r\n", time->rtc_min);
        result = false;
    }

    if (time->rtc_sec > 59) {
        RTC_LOG_ERROR("Invalid second : %d\r\n", time->rtc_sec);
        result = false;
    }

    return result;
}

static void rtc_eint_callback(void *user_data)
{
    uint32_t value;

    hal_eint_mask(HAL_EINT_RTC);

    /* read clear interrupt status */
    value = rtc_register->RTC_IRQ_STA;
    /* clear alarm power on */
    rtc_register->RTC_BBPU = RTC_KEY_BBPU_1 | RTC_RTC_PU_MASK;
    rtc_write_trigger_wait();

    if ((value & RTC_ALSTA_MASK) != 0) {
        /* disable alarm interrupt */
        rtc_register->RTC_AL_MASK_UNION.RTC_AL_MASK_CELLS.AL_EN = (~RTC_AL_EN_MASK);
        rtc_write_trigger_wait();
        if (rtc_alarm_callback_function != NULL) {
            rtc_alarm_callback_function(rtc_alarm_user_data);
        }
    }

#ifdef HAL_RTC_FEATURE_TIME_CALLBACK
    if ((value & RTC_TCSTA_MASK) != 0) {
        if (rtc_time_callback_function != NULL) {
            rtc_time_callback_function(rtc_time_user_data);
        }
    }
#endif

    hal_eint_unmask(HAL_EINT_RTC);
}

static void rtc_register_init(void)
{
    uint32_t rtc_irq_sta;
    /* Clear ALARM_PU */
    rtc_register->RTC_BBPU = RTC_KEY_BBPU | RTC_RTC_PU_MASK;
    /* Read clear */
    rtc_irq_sta = rtc_register->RTC_IRQ_STA;
    rtc_irq_sta = rtc_irq_sta;
    rtc_register->RTC_IRQ_EN_UNION.RTC_IRQ_EN = 0x0;
    rtc_register->RTC_CII_EN = 0x0;
    rtc_register->RTC_AL_MASK_UNION.RTC_AL_MASK = 0x0;
    rtc_register->RTC_TC0_UNION.RTC_TC0 = 0x0;
    rtc_register->RTC_TC1_UNION.RTC_TC1 = 0x100;
    rtc_register->RTC_TC2_UNION.RTC_TC2 = 0x101;
    rtc_register->RTC_TC3 = 0x0;
    rtc_register->RTC_AL0_UNION.RTC_AL0 = 0x0;
    rtc_register->RTC_AL1_UNION.RTC_AL1 = 0x100;
    rtc_register->RTC_AL2_UNION.RTC_AL2 = 0x101;
    rtc_register->RTC_AL3 = 0x0;
    rtc_register->RTC_LPD_CON_UNION.RTC_LPD_CON = 0x101;
    rtc_register->RTC_NEW_SPAR0_UNION.RTC_NEW_SPAR0 = 0x0;
    rtc_register->RTC_EINT = 0x7;
    rtc_write_trigger_wait();
    rtc_write_osc32con0(0x101, RTC_WORD_OPERATION);
    rtc_write_osc32con1(0xf07);
    /* mask below two line for excute sleep_management_exit_retention before hal_rtc_init */
    rtc_write_osc32con2(0x03, RTC_LOW_BYTE_OPERATION);
    //rtc_write_sramcon(0xFFFF, RTC_WORD_OPERATION);
    rtc_register->RTC_PDN0_UNION.RTC_PDN0 = 0x0;
    rtc_register->RTC_PDN1_UNION.RTC_PDN1 = 0x0;
    rtc_register->RTC_SPAR0_UNION.RTC_SPAR0 = 0x0;
    rtc_register->RTC_SPAR1_UNION.RTC_SPAR1 = 0x0;
    rtc_register->RTC_DIFF = 0x0;
    rtc_register->RTC_CALI = 0x0;
    rtc_register->RTC_GPIO_CON_UNION.RTC_GPIO_CON = 0x8A;

    rtc_write_trigger_wait();
}

static void rtc_init_eint(void)
{
    hal_eint_config_t eint_config;
    hal_eint_status_t result;

    // reset to default value is ok
    /* RTC EINT Pinmux setting */
    rtc_register->RTC_GPIO_CON_UNION.RTC_GPIO_CON = 0x8A;
    rtc_write_trigger_wait();
    rtc_register->RTC_EINT = 0x7;
    rtc_write_trigger_wait();

    eint_config.trigger_mode = HAL_EINT_LEVEL_LOW;
    eint_config.debounce_time = 0;
    result = hal_eint_init(HAL_EINT_RTC, &eint_config);
    if (result != HAL_EINT_STATUS_OK) {
        RTC_LOG_ERROR("hal_eint_init fail: %d\r\n", result);
        return;
    }

    result = hal_eint_register_callback(HAL_EINT_RTC, rtc_eint_callback, NULL);
    if (result != HAL_EINT_STATUS_OK) {
        RTC_LOG_ERROR("hal_eint_register_callback fail: %d\r\n", result);
        return;
    }

    result = hal_eint_unmask(HAL_EINT_RTC);
    if (result != HAL_EINT_STATUS_OK) {
        RTC_LOG_ERROR("hal_eint_unmask fail: %d\r\n", result);
        return;
    }
}

static void rtc_dump_register(char *tag)
{
#ifdef RTC_LOG_DEBUG
    uint32_t value_key1, value_key2, value_diff, value_osc32con0, value_osc32con1, value_osc32con2;
    uint32_t value_bbpu, value_lpd, value_wrtgr, value_sramcon, value_cali;
    rtc_reload();
    value_key1 = rtc_register->RTC_POWERKEY0;
    value_key2 = rtc_register->RTC_POWERKEY1;
    value_diff = rtc_register->RTC_DIFF;
    value_osc32con0 = rtc_read_osc32con0();
    value_osc32con1 = rtc_read_osc32con1();
    value_osc32con2 = rtc_read_osc32con2();
    value_sramcon = rtc_read_sramcon();
    value_bbpu = rtc_register->RTC_BBPU;
    value_lpd = rtc_register->RTC_LPD_CON_UNION.RTC_LPD_CON;
    value_wrtgr = rtc_register->RTC_WRTGR_UNION.RTC_WRTGR;
    value_cali = rtc_register->RTC_CALI;
    RTC_LOG_WARNING("rtc_dump_register[%s], RTC_POWERKEY1 = %x, RTC_POWERKEY2 = %x, RTC_DIFF = %x, RTC_OSC32CON0 = %x\r\n",
                    tag, value_key1, value_key2, value_diff, value_osc32con0);
    RTC_LOG_WARNING("RTC_BBPU = %x, RTC_LPD_CON = %x, RTC_CON32CON1 = %x, RTC_CON32CON2 = %x, RTC_WRTGR = %x, SRAMCON = %x, CALI = %x\r\n",
                    value_bbpu, value_lpd, value_osc32con1, value_osc32con2, value_wrtgr, value_sramcon, value_cali);
#endif
}
#if 0
static void rtc_check_alarm_power_on(void)
{
    hal_rtc_time_t time;

    if ((rtc_register->RTC_BBPU & RTC_RTC_PU_MASK) != 0) {
        hal_rtc_get_time(&time);
        RTC_LOG_WARNING("time : %d/%d/%d %d:%d:%d\r\n", time.rtc_year, time.rtc_mon, time.rtc_day,
                        time.rtc_hour, time.rtc_min, time.rtc_sec);
        hal_rtc_get_alarm(&time);
        RTC_LOG_WARNING("alarm : %d/%d/%d %d:%d:%d\r\n", time.rtc_year, time.rtc_mon, time.rtc_day,
                        time.rtc_hour, time.rtc_min, time.rtc_sec);
        RTC_LOG_WARNING("Alarm power on, %x\r\n", rtc_register->RTC_BBPU);
    }
}
#endif
static void f32k_eosc32_calibration(void)
{
    uint32_t value;

    uint32_t low_xosccali = 0x00;
    uint32_t high_xosccali = 0x1f;
    uint32_t medium_xosccali;

    uint32_t low_frequency = 0;
    uint32_t high_frequency = 0;
    uint32_t medium_frequency;

    value = rtc_read_osc32con1();
    value &= ~RTC_XOSCCALI_MASK;
    value |= (low_xosccali << RTC_XOSCCALI_OFFSET);
    rtc_write_osc32con1(value);
    high_frequency = f32k_measure_clock(FQMTR_FCKSEL_EOSC_F32K_CK, FQMTR_TCKSEL_XO_CK, 99);
    if (high_frequency <= 32768) {
        RTC_LOG_INFO("high_frequency <= 32768, frequency = %u, xosccali = %d\r\n", (unsigned int)high_frequency, (unsigned int)low_xosccali);
        return;
    }

    value = rtc_read_osc32con1();
    value &= ~RTC_XOSCCALI_MASK;
    value |= (high_xosccali << RTC_XOSCCALI_OFFSET);
    rtc_write_osc32con1(value);
    low_frequency = f32k_measure_clock(FQMTR_FCKSEL_EOSC_F32K_CK, FQMTR_TCKSEL_XO_CK, 99);
    if (low_frequency >= 32768) {
        RTC_LOG_INFO("low_frequency >= 32768, frequency = %u, xosccali = %d\r\n", (unsigned int)low_frequency, (unsigned int)high_xosccali);
        return;
    }

    while ((high_xosccali - low_xosccali) > 1) {
        medium_xosccali = (low_xosccali + high_xosccali) / 2;
        value = rtc_read_osc32con1();
        value &= ~RTC_XOSCCALI_MASK;
        value |= (medium_xosccali << RTC_XOSCCALI_OFFSET);
        rtc_write_osc32con1(value);
        medium_frequency = f32k_measure_clock(FQMTR_FCKSEL_EOSC_F32K_CK, FQMTR_TCKSEL_XO_CK, 99);
        if (medium_frequency > 32768) {
            low_xosccali = medium_xosccali;
            high_frequency = medium_frequency;
        } else if (medium_frequency < 32768) {
            high_xosccali = medium_xosccali;
            low_frequency = medium_frequency;
        } else {
            RTC_LOG_INFO("xosccali = %d\r\n", medium_xosccali);
            return;
        }
    }

    if ((32768 - low_frequency) < (high_frequency - 32768)) {
        value = rtc_read_osc32con1();
        value &= ~RTC_XOSCCALI_MASK;
        value |= (high_xosccali << RTC_XOSCCALI_OFFSET);
        rtc_write_osc32con1(value);
        RTC_LOG_INFO("frequency = %u, xosccali = %d\r\n", (unsigned int)low_frequency, (unsigned int)high_xosccali);
    } else {
        value = rtc_read_osc32con1();
        value &= ~RTC_XOSCCALI_MASK;
        value |= (low_xosccali << RTC_XOSCCALI_OFFSET);
        rtc_write_osc32con1(value);
        RTC_LOG_INFO("frequency = %u, xosccali = %d\r\n", (unsigned int)high_frequency, (unsigned int)low_xosccali);
    }
}

#if RTC_32K_DEBUG
static hal_rtc_status_t rtc_wait_second_changed()
{
    hal_rtc_time_t time;
    uint32_t second;

    hal_rtc_get_time(&time);
    second = time.rtc_sec;

    do {
        hal_rtc_get_time(&time);
    } while (second == time.rtc_sec);

    return HAL_RTC_STATUS_OK;
}
#endif

static void f32k_osc32_Init(void)
{
    uint32_t frequency, value;
    int16_t cali;
    uint32_t value_wrtgr, value_lpd_con;

    /* SYSTEM_INFOD: 0xa2010040
       SLOW_SRC_B = SYSTEM_INFOD & 0x00000020 */
    if ((*RTC_SLOW_SRC_B & 0x20) == 0) {
        log_hal_info("Use 32k crystal\r\n");
        use_xosc = true;
    } else {
        log_hal_info("No 32k crystal\r\n");
    }

    if (((rtc_register->RTC_WRTGR_UNION.RTC_WRTGR_CELLS.RTC_STA & RTC_POWER_DETECTED_MASK) == 0) ||
        ((rtc_register->RTC_LPD_CON_UNION.RTC_LPD_CON_CELLS.LPD_CON & RTC_LPSTA_RAW_MASK) != 0)) {
        /* LPD */
        value_wrtgr = rtc_register->RTC_WRTGR_UNION.RTC_WRTGR;
        value_lpd_con = rtc_register->RTC_LPD_CON_UNION.RTC_LPD_CON;
        log_hal_warning("LPD occurred, rtc_wrtgr: %x, rtc_lpd_con: %x\r\n",
                        value_wrtgr, value_lpd_con);

        if(retention_flag == false) {
            rtc_register_init();
        }
        /* LPD related */
        /* workaround, disable LPD firstly */
        rtc_register->RTC_LPD_CON_UNION.RTC_LPD_CON = 0x101;
        rtc_write_trigger_wait();

        rtc_set_power_key();

        /* set EOSC32_STP_PWD */
        value = rtc_read_osc32con1();
        value |= 0x80;
        rtc_write_osc32con1(value);

        /* LPD init again */
        rtc_lpd_init();

        /* enable LPD */
        rtc_register->RTC_LPD_CON_UNION.RTC_LPD_CON_CELLS.RTC_LPD_OPT = 0x0;
        rtc_write_trigger_wait();

        if (use_xosc) {
            /* Use 32k crystal */
            rtc_set_xosc_mode();
            /* Measure 32K */
            frequency = f32k_measure_clock(FQMTR_FCKSEL_XOSC_F32K_CK, FQMTR_TCKSEL_XO_CK, 99);
            /* Set RTC_CALI */
            cali = (32768 - frequency) << 3;
            if (cali > (int16_t)0x1FFF) {
                cali = 0x1FFF;
            } else if (cali < (int16_t)0xE000) {
                cali = 0xE000;
            }
            /* normal RTC_CALI */
            rtc_register->RTC_CALI = cali;
            rtc_write_trigger_wait();
            RTC_LOG_INFO("xosc frequency = %u, RTC_CALI = %x\r\n", (unsigned int)frequency, rtc_register->RTC_CALI);

            /* select 32k clock source for PMU and Top as XOSC*/
            rtc_write_osc32con0(0x6, RTC_LOW_BYTE_OPERATION);
            /* wait 200us for hw switch time */
            hal_gpt_delay_us(200);
            /* turn off EOSC and disable XO_DIV_32k */
            rtc_write_osc32con2(RTC_XOSC_PWDB_MASK, RTC_LOW_BYTE_OPERATION);
            *RTC_XO_DIV_32K_EN = 0;
        } else {
            /* select 32k clock source for PMU and Top: PMU is EOSC, Top is XO_DIV32 for normal mode, EOSC for retention mode */
            /* Enable XO_DIV_32k, switch to XO_DIV_32k */
            *RTC_XO_DIV_32K_EN = 1;
            rtc_write_osc32con0(0x0, RTC_LOW_BYTE_OPERATION);

            /* normal RTC_CALI */
            if (*RTC_FXO_IS_26M) {
                /* 26M/764=32745, write 23*8=184 in rtc_cali */
                rtc_register->RTC_CALI = 184;
            } else {
                /* 40M/1221=32760, write 8*8=64 in rtc_cali */
                rtc_register->RTC_CALI = 64;
            }
            rtc_write_trigger_wait();
            /* wait 200us for hw switch time */
            hal_gpt_delay_us(200);
            /* turn off XOSC*/
            rtc_write_osc32con2(RTC_EOSC_PWDB_MASK, RTC_LOW_BYTE_OPERATION);
        }
    }

}

uint32_t f32k_measure_clock(uint32_t fixed_clock, uint32_t tested_clock, uint32_t window_setting)
{
    uint32_t fqmtr_data;
    uint32_t frequency;

    /* 1) PLL_ABIST_FQMTR_CON0 = 0xCXXX */
    abist_fqmtr_register->ABIST_FQMTR_CON0 |= 0xC000;
    hal_gpt_delay_us(1);
    while ((abist_fqmtr_register->ABIST_FQMTR_CON1 & 0x8000) != 0);
    /* 2) CKSYS_TST_SEL_1_BASE = 0x0 */
    *CKSYS_TST_SEL_1_BASE = 0;
    /* 3) CKSYS_TST_SEL_1_BASE = 0x0601 */
    *CKSYS_TST_SEL_1_BASE = (fixed_clock << 8) | tested_clock;
    abist_fqmtr_register->ABIST_FQMTR_CON2 = 0;
    /* 4) PLL_ABIST_FQMTR_CON0 = 0x8009 */
    abist_fqmtr_register->ABIST_FQMTR_CON0 = 0x8000 | window_setting;
    hal_gpt_delay_us(200);
    /* 5) Wait PLL_ABIST_FQMTR_CON1 & 0x8000 == 0x8000 */
    while ((abist_fqmtr_register->ABIST_FQMTR_CON1 & 0x8000) != 0);
    /* 6) Read PLL_ABIST_FQMTR_DATA */
    fqmtr_data = abist_fqmtr_register->ABIST_FQMTR_DATA;
    //RTC_LOG_WARNING("fqmtr_data = %x\r\n", fqmtr_data);
    /* 7) Freq = fxo_clk*10/PLL_ABIST_FQMTR_DATA */
    if (rtc_fxo_is_26m) {
        frequency = 26000000 * (window_setting + 1) / fqmtr_data;
    } else {
        frequency = 20000000 * (window_setting + 1) / fqmtr_data;
    }
    return frequency;
}

uint32_t f32k_measure_count(uint16_t fixed_clock, uint16_t tested_clock, uint16_t window_setting)
{
    uint32_t fqmtr_data;

    /* 1) PLL_ABIST_FQMTR_CON0 = 0xCXXX */
    abist_fqmtr_register->ABIST_FQMTR_CON0 |= 0xC000;
    ///TODO: check about how much this delay time is needed
    hal_gpt_delay_us(1000);
    while ((abist_fqmtr_register->ABIST_FQMTR_CON1 & 0x8000) != 0);
    /* 2) CKSYS_TST_SEL_1_BASE = 0x0 */
    *CKSYS_TST_SEL_1_BASE = 0;
    /* 3) CKSYS_TST_SEL_1_BASE = 0x0601 */
    *CKSYS_TST_SEL_1_BASE = (fixed_clock << 8) | tested_clock;
    abist_fqmtr_register->ABIST_FQMTR_CON2 = 0;
    /* 4) PLL_ABIST_FQMTR_CON0 = 0x8009 */
    abist_fqmtr_register->ABIST_FQMTR_CON0 = 0x8000 | window_setting;
    hal_gpt_delay_us(1000);
    /* 5) Wait PLL_ABIST_FQMTR_CON1 & 0x8000 == 0x8000 */
    while ((abist_fqmtr_register->ABIST_FQMTR_CON1 & 0x8000) != 0);
    /* 6) Read PLL_ABIST_FQMTR_DATA */
    fqmtr_data = abist_fqmtr_register->ABIST_FQMTR_DATA;

    return fqmtr_data;
}

bool rtc_is_back_from_retention()
{
    uint32_t retention_flag;
    rtc_reload();
    retention_flag = rtc_register->RTC_BBPU;
    RTC_LOG_INFO("retention_flag = %x\r\n", (retention_flag & RTC_RETENTION_MODE_MASK) >> RTC_RETENTION_MODE_OFFSET);
    return (((retention_flag & RTC_RETENTION_MODE_MASK) == RTC_RETENTION_MODE_MASK) ? true : false);
}

hal_rtc_status_t hal_rtc_init(void)
{
    if (true == rtc_init_done) {
        return HAL_RTC_STATUS_OK;
    }

    rtc_wait_busy();

    //retention_flag = rtc_is_back_from_retention();
    rtc_dump_register("Just After Power On");
#ifdef HAL_RTC_FEATURE_RETENTION_SRAM
    hal_rtc_exit_retention_mode();
#endif
    rtc_unlock_PROT();

    f32k_osc32_Init();

    //rtc_check_alarm_power_on();

    rtc_init_eint();

    rtc_init_done = true;
    /* user should call this API explictly before using RTC SRAM */
    //hal_rtc_retention_sram_config(1, HAL_RTC_SRAM_PD_TO_NORMAL_MODE);

    rtc_dump_register("hal_rtc_init done");
    RTC_LOG_INFO("init_done, RTC_32K = %u, EOSC = %u, DCXO = %u, XOSC = %u\r\n",
                 (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_RTC_32K, FQMTR_TCKSEL_XO_CK, 99),
                 (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_EOSC_F32K_CK, FQMTR_TCKSEL_XO_CK, 99),
                 (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_DCXO_F32K_CK, FQMTR_TCKSEL_XO_CK, 99),
                 (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_XOSC_F32K_CK, FQMTR_TCKSEL_XO_CK, 99));
    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_deinit(void)
{
    rtc_dump_register("hal_rtc_deinit done");

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_set_time(const hal_rtc_time_t *time)
{
    if (!rtc_is_time_valid(time)) {
        return HAL_RTC_STATUS_INVALID_PARAM;
    }
    rtc_register->RTC_TC0_UNION.RTC_TC0 = (time->rtc_min << RTC_TC_MINUTE_OFFSET) | (time->rtc_sec);
    rtc_register->RTC_TC1_UNION.RTC_TC1 = (time->rtc_day << RTC_TC_DOM_OFFSET) | (time->rtc_hour);
    // day-of-week range is 1~7, header file is 0~6
    rtc_register->RTC_TC2_UNION.RTC_TC2 = (time->rtc_mon << RTC_TC_MONTH_OFFSET) | (time->rtc_week + 1);
    rtc_register->RTC_TC3 = time->rtc_year;
    // note: The free run register line TC0~TC3 will have issue when you just configure one byte data,
    // in this case, read-modify-write operation is needed, this will be fixed in MT2625
    rtc_write_trigger_wait();

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_get_time(hal_rtc_time_t *time)
{
    uint16_t value_tc2, value_tc1, value_tc0, value;
    /* re-read time information if second value changed */
    do {
        value_tc0 = rtc_register->RTC_TC0_UNION.RTC_TC0;
        time->rtc_year = rtc_register->RTC_TC3;
        value_tc2 = rtc_register->RTC_TC2_UNION.RTC_TC2;
        value_tc1 = rtc_register->RTC_TC1_UNION.RTC_TC1;
        value = rtc_register->RTC_TC0_UNION.RTC_TC0;
    } while ((value_tc0 & RTC_TC_SECOND_MASK) != (value & RTC_TC_SECOND_MASK));
    time->rtc_week = (value_tc2 & RTC_TC_DOW_MASK) - 1;
    time->rtc_mon = (value_tc2 & RTC_TC_MONTH_MASK) >> RTC_TC_MONTH_OFFSET;
    time->rtc_hour = value_tc1 & RTC_TC_HOUR_MASK;
    time->rtc_day = (value_tc1 & RTC_TC_DOM_MASK) >> RTC_TC_DOM_OFFSET;
    time->rtc_min = (value_tc0 & RTC_TC_MINUTE_MASK) >> RTC_TC_MINUTE_OFFSET;
    time->rtc_sec = value_tc0 & RTC_TC_SECOND_MASK;

    return HAL_RTC_STATUS_OK;
}

#ifdef HAL_RTC_FEATURE_TIME_CALLBACK
hal_rtc_status_t hal_rtc_set_time_notification_period(hal_rtc_time_notification_period_t period)
{
    uint32_t enable;
    uint32_t cii_setting;

    switch (period) {
        case HAL_RTC_TIME_NOTIFICATION_NONE:
            enable = 0;
            cii_setting = 0;
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND:
            enable = 1;
            cii_setting = 3;
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_MINUTE:
            enable = 1;
            cii_setting = 4;
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_HOUR:
            enable = 1;
            cii_setting = 5;
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_DAY:
            enable = 1;
            cii_setting = 6;
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_MONTH:
            enable = 1;
            cii_setting = 8;
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_YEAR:
            enable = 1;
            cii_setting = 9;
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND_1_2:
            enable = 1;
            cii_setting = 2;
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND_1_4:
            enable = 1;
            cii_setting = 1;
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND_1_8:
            enable = 1;
            cii_setting = 0;
            break;
        default:
            return HAL_RTC_STATUS_INVALID_PARAM;
    }

    rtc_register->RTC_CII_EN = (enable << RTC_TC_EN_OFFSET) | cii_setting;
    /* set TICK_PWREN here */
    rtc_register->RTC_BBPU = RTC_KEY_BBPU_2 | (enable << RTC_TICK_PWREN_OFFSET);

    rtc_write_trigger_wait();

    return HAL_RTC_STATUS_OK;
}
#endif

hal_rtc_status_t hal_rtc_set_alarm(const hal_rtc_time_t *time)
{
    if (!rtc_is_time_valid(time)) {
        return HAL_RTC_STATUS_INVALID_PARAM;
    }

    rtc_register->RTC_AL0_UNION.RTC_AL0 = (time->rtc_min << RTC_AL_MINUTE_OFFSET) | time->rtc_sec;
    rtc_register->RTC_AL1_UNION.RTC_AL1 = (time->rtc_day << RTC_AL_DOM_OFFSET) | time->rtc_hour;
    rtc_register->RTC_AL2_UNION.RTC_AL2 = (time->rtc_mon << RTC_AL_MONTH_OFFSET) | (time->rtc_week + 1);
    rtc_register->RTC_AL3 = time->rtc_year;

    rtc_write_trigger_wait();

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_get_alarm(hal_rtc_time_t *time)
{
    time->rtc_year = rtc_register->RTC_AL3;
    time->rtc_mon = rtc_register->RTC_AL2_UNION.RTC_AL2_CELLS.AL_MONTH;
    time->rtc_week = rtc_register->RTC_AL2_UNION.RTC_AL2_CELLS.AL_DOW - 1;
    time->rtc_day = rtc_register->RTC_AL1_UNION.RTC_AL1_CELLS.AL_DOM;
    time->rtc_hour = rtc_register->RTC_AL1_UNION.RTC_AL1_CELLS.AL_HOUR;
    time->rtc_min = rtc_register->RTC_AL0_UNION.RTC_AL0_CELLS.AL_MINUTE;
    time->rtc_sec = rtc_register->RTC_AL0_UNION.RTC_AL0_CELLS.AL_SECOND;

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_enable_alarm(void)
{
    /* Enable alarm interrupt */
    rtc_register->RTC_AL_MASK_UNION.RTC_AL_MASK_CELLS.AL_EN = RTC_AL_EN_MASK;

    /* Enable alarm power on */
    rtc_register->RTC_BBPU = RTC_KEY_BBPU_0 | RTC_ALARM_PWREN_MASK;

    rtc_write_trigger_wait();

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_disable_alarm(void)
{
    rtc_register->RTC_AL_MASK_UNION.RTC_AL_MASK_CELLS.AL_EN = (0x0 << RTC_AL_EN_OFFSET);

    rtc_register->RTC_BBPU = RTC_KEY_BBPU_0;

    rtc_write_trigger_wait();

    return HAL_RTC_STATUS_OK;
}

#ifdef HAL_RTC_FEATURE_TIME_CALLBACK
hal_rtc_status_t hal_rtc_set_time_callback(hal_rtc_time_callback_t callback_function, void *user_data)
{
    rtc_time_callback_function = callback_function;
    rtc_time_user_data = user_data;

    return HAL_RTC_STATUS_OK;
}
#endif

hal_rtc_status_t hal_rtc_set_alarm_callback(const hal_rtc_alarm_callback_t callback_function, void *user_data)
{
    rtc_alarm_callback_function = callback_function;
    rtc_alarm_user_data = user_data;

    return HAL_RTC_STATUS_OK;
}

#ifdef HAL_RTC_FEATURE_CALIBRATION
hal_rtc_status_t hal_rtc_set_one_shot_calibration(int16_t ticks)
{
    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_get_one_shot_calibration(int16_t *ticks)
{
    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_set_repeat_calibration(int16_t ticks_per_8_seconds)
{
    uint32_t value = 0;

    if (ticks_per_8_seconds > (int16_t)0x1FFF) {
        ticks_per_8_seconds = 0x1FFF;
    } else if (ticks_per_8_seconds < (int16_t)0xE000) {
        ticks_per_8_seconds = 0xE000;
    }
    value = ((ticks_per_8_seconds << RTC_RTC_CALI_OFFSET) & RTC_RTC_CALI_MASK);
    rtc_register->RTC_CALI = value;

    rtc_write_trigger_wait();
    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_get_repeat_calibration(int16_t *ticks_per_8_seconds)
{
    rtc_reload();

    *ticks_per_8_seconds = (rtc_register->RTC_CALI & RTC_RTC_CALI_MASK) >> RTC_RTC_CALI_OFFSET;

    *ticks_per_8_seconds &= 0x3FFF;
    if (*ticks_per_8_seconds >= 0x2000) {
        *ticks_per_8_seconds -= 0x4000;
    }
    return HAL_RTC_STATUS_OK;
}
#endif

hal_rtc_status_t hal_rtc_get_f32k_frequency(uint32_t *frequency)
{
    *frequency = f32k_measure_clock(FQMTR_FCKSEL_RTC_32K, FQMTR_TCKSEL_XO_CK, 99);
    return HAL_RTC_STATUS_OK;
}


hal_rtc_status_t rtc_set_data(uint16_t offset, const char *buf, uint16_t len, bool access_hw)
{
    uint16_t i = 0;

    if ((offset >= (sizeof(rtc_spare_register_table) / sizeof(rtc_spare_register_table[0]))) ||
        (offset + len > (sizeof(rtc_spare_register_table) / sizeof(rtc_spare_register_table[0]))) || (buf == NULL)) {
        RTC_LOG_ERROR("Invalid parameter, offset = %d, len = %d, buf = %p", offset, len, buf);
        return HAL_RTC_STATUS_INVALID_PARAM;
    }

    for (i = 0; i < len; i++) {
        if (access_hw) {
            *(rtc_spare_register_table[i + offset].address) = *(buf + i);
            rtc_write_trigger_wait();
        } else {
            rtc_spare_register_backup[offset + i] = *(buf + i);
        }
    }

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_set_data(uint16_t offset, const char *buf, uint16_t len)
{
    if (rtc_in_test) {
        RTC_LOG_WARNING("%s: in rtc test mode.", __func__);
    }

    rtc_set_data(offset, buf, len, !rtc_in_test);

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t rtc_get_data(uint16_t offset, char *buf, uint16_t len, bool access_hw)
{
    uint16_t i = 0;

    if ((offset >= (sizeof(rtc_spare_register_table) / sizeof(rtc_spare_register_table[0]))) ||
        (offset + len > (sizeof(rtc_spare_register_table) / sizeof(rtc_spare_register_table[0]))) || (buf == NULL)) {
        RTC_LOG_ERROR("Invalid parameter, offset = %d, len = %d, buf = %p", offset, len, buf);
        return HAL_RTC_STATUS_INVALID_PARAM;
    }

    for (i = 0; i < len; i++) {
        if (access_hw) {
            *(buf + i) = *(rtc_spare_register_table[i + offset].address);
        } else {
            *(buf + i) = rtc_spare_register_backup[offset + i];
        }
    }

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_get_data(uint16_t offset, char *buf, uint16_t len)
{
    if (rtc_in_test) {
        RTC_LOG_WARNING("%s: in rtc test mode.", __func__);
    }

    rtc_get_data(offset, buf, len, !rtc_in_test);

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t rtc_clear_data(uint16_t offset, uint16_t len)
{
    char buf[HAL_RTC_BACKUP_BYTE_NUM_MAX];

    memset(buf, 0, sizeof(buf));

    rtc_set_data(offset, buf, len, true);

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_clear_data(uint16_t offset, uint16_t len)
{
    char buf[HAL_RTC_BACKUP_BYTE_NUM_MAX];

    if (rtc_in_test) {
        RTC_LOG_WARNING("%s: in rtc test mode.", __func__);
    }

    memset(buf, 0, sizeof(buf));

    rtc_set_data(offset, buf, len, !rtc_in_test);

    return HAL_RTC_STATUS_OK;
}

void rtc_set_register(uint16_t address, uint16_t value)
{
    if (address > (uint32_t) & (((RTC_REGISTER_T *)0)->RTC_GPIO_CON_UNION.RTC_GPIO_CON)) {
        RTC_LOG_ERROR("Invalid address");
    }

    *(uint16_t *)((uint8_t *)rtc_register + address) = value;
}

uint16_t rtc_get_register(uint16_t address)
{
    if (address > (uint32_t) & (((RTC_REGISTER_T *)0)->RTC_GPIO_CON_UNION.RTC_GPIO_CON)) {
        RTC_LOG_ERROR("Invalid address");
    }

    return *(uint16_t *)((uint8_t *)rtc_register + address);
}

static void rtc_forward_time(hal_rtc_time_t *time, int second)
{
    int minute = 0;
    int hour = 0;
    int day = 0;
    int remender = 0;
    int max_day;
    const int days_in_month[13] = {
        0,  /* Null */
        31, /* Jan */
        28, /* Feb */
        31, /* Mar */
        30, /* Apr */
        31, /* May */
        30, /* Jun */
        31, /* Jul */
        31, /* Aug */
        30, /* Sep */
        31, /* Oct */
        30, /* Nov */
        31  /* Dec */
    };

    second += time->rtc_sec;
    minute = time->rtc_min;
    hour = time->rtc_hour;

    if (second > 59) {
        /* min */
        minute += second / 60;
        second %= 60;
    }
    time->rtc_sec = second;
    if (minute > 59) {
        /* hour */
        hour += minute / 60;
        minute %= 60;
    }
    time->rtc_min = minute;
    if (hour > 23) {
        /* day of week */
        day = hour / 24;
        hour %= 24;
        /* day of month */
        time->rtc_day += day;
        max_day = days_in_month[time->rtc_mon];
        if (time->rtc_mon == 2) {
            remender = time->rtc_year % 4;
            if (remender == 0) {
                max_day++;
            }
        }
        if (time->rtc_day > max_day) {
            time->rtc_day -= max_day;

            /* month of year */
            time->rtc_mon++;
            if (time->rtc_mon > 12) {
                time->rtc_mon = 1;
                time->rtc_year++;
            }
        }
    }
    time->rtc_hour = hour;
}


static void test_rtc_alarm_callback(void *parameter)
{
    hal_rtc_time_t *alarm_power_on_time;
    RTC_LOG_INFO("test_rtc_alarm_callback");

    alarm_power_on_time = (hal_rtc_time_t *)parameter;

    RTC_LOG_INFO("target alarm time: 20%d,%d,%d (%d) %d:%d:%d", alarm_power_on_time->rtc_year,
                 alarm_power_on_time->rtc_mon, alarm_power_on_time->rtc_day, alarm_power_on_time->rtc_week,
                 alarm_power_on_time->rtc_hour, alarm_power_on_time->rtc_min, alarm_power_on_time->rtc_sec);
    hal_rtc_set_alarm_callback(NULL, NULL);

    hal_rtc_set_alarm(alarm_power_on_time);
    hal_rtc_enable_alarm();
}

hal_rtc_status_t rtc_alarm_power_on_test(hal_rtc_time_t *time)
{
    static hal_rtc_time_t alarm_power_on_time;
    hal_rtc_time_t rtc_get_time;

    memcpy(&alarm_power_on_time, time, sizeof(hal_rtc_time_t));

    RTC_LOG_INFO("target alarm time: 20%d,%d,%d (%d) %d:%d:%d", time->rtc_year,
                 time->rtc_mon, time->rtc_day, time->rtc_week, time->rtc_hour, time->rtc_min, time->rtc_sec);
    hal_rtc_get_time(&rtc_get_time);
    RTC_LOG_INFO("get alarm time: 20%d,%d,%d (%d) %d:%d:%d", rtc_get_time.rtc_year,
                 rtc_get_time.rtc_mon, rtc_get_time.rtc_day, rtc_get_time.rtc_week - 1,
                 rtc_get_time.rtc_hour, rtc_get_time.rtc_min, rtc_get_time.rtc_sec);
    rtc_forward_time(&rtc_get_time, 10);
    RTC_LOG_INFO("set alarm time: 20%d,%d,%d (%d) %d:%d:%d", rtc_get_time.rtc_year,
                 rtc_get_time.rtc_mon, rtc_get_time.rtc_day, rtc_get_time.rtc_week - 1,
                 rtc_get_time.rtc_hour, rtc_get_time.rtc_min, rtc_get_time.rtc_sec);
    hal_rtc_set_alarm(&rtc_get_time);
    hal_rtc_set_alarm_callback(test_rtc_alarm_callback, &alarm_power_on_time);
    hal_rtc_enable_alarm();

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t rtc_enter_test(bool enter)
{
    RTC_LOG_INFO("%s: %d", __func__, enter);

    if (enter) {
        rtc_get_data(0, rtc_spare_register_backup, HAL_RTC_BACKUP_BYTE_NUM_MAX, true);
    } else {
        rtc_set_data(0, rtc_spare_register_backup, HAL_RTC_BACKUP_BYTE_NUM_MAX, true);
    }
    rtc_in_test = enter;

    return HAL_RTC_STATUS_OK;
}

#ifdef HAL_RTC_FEATURE_RETENTION_SRAM
hal_rtc_status_t hal_rtc_retention_sram_config(uint32_t mask, hal_rtc_sram_mode_t mode)
{
    uint32_t value_low_byte, value_high_byte, value_sramcon;
    uint8_t i;
    if ((mode > HAL_RTC_SRAM_PD_MODE) || (mask >= (1 << HAL_RTC_RETENTION_SRAM_NUMBER_MAX))) {
        return HAL_RTC_STATUS_INVALID_PARAM;
    }
    if (mask == 0) {
        /* User don't care about the SRAM configuration, just return. */
        return HAL_RTC_STATUS_OK;
    }

    value_sramcon = rtc_read_sramcon();
    value_low_byte = value_sramcon & 0xff;
    value_high_byte = (value_sramcon & 0xff00) >> 8;
    switch (mode) {
        case HAL_RTC_SRAM_NORMAL_MODE:
            /* switch retention SRAM power to core power, power on retention SRAM one by one */
            /* From power down mode to normal mode ; when exit from sleep mode, all should turn on */
            for (i = 0; i < HAL_RTC_RETENTION_SRAM_NUMBER_MAX; i++) {
                if (mask & (1 << i)) {
                    if (value_sramcon&(1<<i)) {
                        /* Power down to Normal: PD change from 1 to 0 */
                        value_low_byte &= (0xfe >> (8-i) | 0xfe << i);
                        rtc_write_sramcon(value_low_byte, RTC_LOW_BYTE_OPERATION);
                    } else if (!(value_sramcon&(0x100<<i))) {
                        /* Sleep to Normal: SLEEPB change from 0 to 1 */
                        value_high_byte |= 0x1 << i;
                        rtc_write_sramcon(value_high_byte, RTC_HIGH_BYTE_OPERATION);
                    }
                }
            }
            hal_gpt_delay_us(1);
            /* setting RTC_ISO_EN = 0 */
            rtc_write_osc32con2(~RTC_SRAM_ISO_EN_MASK, RTC_HIGH_BYTE_OPERATION);
            break;
        case HAL_RTC_SRAM_SLEEP_MODE:
            /* setting RTC_ISO_EN = 1 */
            rtc_write_osc32con2(RTC_SRAM_ISO_EN_MASK, RTC_HIGH_BYTE_OPERATION);
            for (i = 0; i < HAL_RTC_RETENTION_SRAM_NUMBER_MAX; i++) {
                if (mask & (1 << i)) {
                    if (value_sramcon&(1<<i)) {
                        /* Power down to Normal: PD change from 1 to 0 */
                        value_low_byte &= (0xfe >> (8-i) | 0xfe << i);
                        rtc_write_sramcon(value_low_byte, RTC_LOW_BYTE_OPERATION);
                    } else if (!(value_sramcon&(0x100<<i))) {
                        /* Already in Sleep */
                        continue;
                    }

                    /* Normal to Sleep: SLEEPB change from 1 to 0 */
                    value_high_byte &= (0xfe >> (8-i) | 0xfe << i);
                    rtc_write_sramcon(value_high_byte, RTC_HIGH_BYTE_OPERATION);
                }
            }
            break;
        case HAL_RTC_SRAM_PD_MODE:
            /* setting RTC_ISO_EN = 1 */
            rtc_write_osc32con2(RTC_SRAM_ISO_EN_MASK, RTC_HIGH_BYTE_OPERATION);
            for (i = 0; i < HAL_RTC_RETENTION_SRAM_NUMBER_MAX; i++) {
                if (mask & (1 << i)) {
                    if (value_sramcon&(1<<i)) {
                        /* Already in Power down */
                        continue;
                    } else if (!(value_sramcon&(0x100<<i))) {
                        /* Sleep to Normal: SLEEPB change from 0 to 1 */
                        value_high_byte |= 0x1 << i;
                        rtc_write_sramcon(value_high_byte, RTC_HIGH_BYTE_OPERATION);
                    }
                    value_low_byte |= 0x1 << i;
                    rtc_write_sramcon(value_low_byte, RTC_LOW_BYTE_OPERATION);
                }
            }
            break;
        default:
            break;
    }
    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_enter_retention_mode(void)
{
    uint32_t frequency;
    uint32_t irq_status;
    int16_t cali;
    /* Prepare the power of RETSRAM in retention mode */
    hal_rtc_retention_sram_config(HAL_RTC_RETENTION_SRAM_NUMBER_MASK, HAL_RTC_SRAM_SLEEP_MODE);

    /* Set the RETSRAM in sleep mode */
    hal_pmu_enter_retention_mode(PMU_RETENTION_LDO_ON);

    if (use_xosc == false) {
        /*1.switch to eosc from xo_div32
              2.calibration
              3.measurement
              4.enter retention mode */
        /* select 32k clock source for PMU and Top: PMU is EOSC,
           Top is XO_DIV32 for normal mode, EOSC for retention mode */
        rtc_write_osc32con0(0x1, RTC_LOW_BYTE_OPERATION);
        /* wait 200us for hw switch time */
        hal_gpt_delay_us(200);
        f32k_eosc32_calibration();
        /* Measure 32K */
        frequency = f32k_measure_clock(FQMTR_FCKSEL_EOSC_F32K_CK, FQMTR_TCKSEL_XO_CK, 99);
        /* Set K_EOSC32 RTC_CALI in one second */
        cali = (32768 - frequency);
        if (cali > (int16_t)0x1FFF) {
            cali = 0x1FFF;
        } else if (cali < (int16_t)0xE000) {
            cali = 0xE000;
        }
        /* K_EOSC32 RTC_CALI */
        rtc_register->RTC_CALI = cali | RTC_CALI_RW_SEL_MASK;
        rtc_write_trigger_wait();
        RTC_LOG_INFO("rtc_32k frequency = %u, RTC_CALI = %x\r\n", (unsigned int)frequency, rtc_register->RTC_CALI);
    }
    hal_nvic_save_and_set_interrupt_mask(&irq_status);
    /* RTC_ALARM_PWREN_MASK and RTC_TICK_PWREN_MASK should be set in another place to avoid timer always wakeup issue */
    if ((rtc_register->RTC_BBPU & RTC_RTC_PU_MASK) != RTC_RTC_PU_MASK) {
        rtc_register->RTC_BBPU = RTC_KEY_BBPU_1 | RTC_RTC_PU_MASK;
        rtc_write_trigger_wait();
        rtc_register->RTC_BBPU = RTC_KEY_BBPU_3 | RTC_EINT_PWREN_MASK;
        rtc_write_trigger_wait();
        rtc_register->RTC_BBPU = RTC_KEY_BBPU_4 | RTC_RETENTION_MODE_MASK;
        rtc_write_trigger_wait();
        /* Our HW confirmed 500us is ok, here for sure safe use 1ms first. */
        hal_gpt_delay_ms(1);
    }
    /* If SW can run to this line, corner case happened, enter retention mode failed this time.
       User need try to enter retention mode again. */
    rtc_dump_register("Corner case");
    /* clear BBPU[1] */
    rtc_register->RTC_BBPU = RTC_KEY_BBPU_1 | RTC_RTC_PU_MASK;
    rtc_write_trigger_wait();
    /* clear BBPU[4] */
    rtc_register->RTC_BBPU = RTC_KEY_BBPU_4;
    rtc_write_trigger_wait();
    hal_pmu_resume_to_normal(RETENTION_NORMAL);
    hal_rtc_retention_sram_config(HAL_RTC_RETENTION_SRAM_NUMBER_MASK, HAL_RTC_SRAM_NORMAL_MODE);

    hal_nvic_restore_interrupt_mask(irq_status);

    return HAL_RTC_STATUS_ERROR;
}

void hal_rtc_exit_retention_mode(void)
{
    retention_flag = rtc_is_back_from_retention();
    if (retention_flag == true) {
        /* unlock protect for write register */
        rtc_unlock_PROT();
        /* clear BBPU[4] and BBPU[1] */
        rtc_register->RTC_BBPU = RTC_KEY_BBPU_4;
        rtc_write_trigger_wait();
        rtc_register->RTC_BBPU = RTC_KEY_BBPU_1 | RTC_RTC_PU_MASK;
        rtc_write_trigger_wait();

        /*1.switch back to xo_div32 from eosc
         */
        if ((*RTC_SLOW_SRC_B & 0x20) != 0) {
            /* 32k-less mode, need switch Top 32k source from EOSC to XO_DIV32 */
            rtc_write_osc32con0(0x0, RTC_LOW_BYTE_OPERATION);
            /* wait 200us for hw switch time */
            hal_gpt_delay_us(200);
            if (*RTC_FXO_IS_26M) {
                /* 26M/764=32745, write 23*8=184 in rtc_cali */
                rtc_register->RTC_CALI = 184;
            } else {
                /* 40M/1221=32760, write 8*8=64 in rtc_cali */
                rtc_register->RTC_CALI = 64;
            }
            rtc_write_trigger_wait();
        }
        rtc_dump_register("hal_rtc_exit_retention_mode");
        //leave retention mode
        log_hal_info("leave retention mode\r\n");
        hal_pmu_resume_to_normal(RETENTION_NORMAL);
    } else {
        //normal bootup
        log_hal_info("normal bootup\r\n");
    }

    hal_rtc_retention_sram_config(HAL_RTC_RETENTION_SRAM_NUMBER_MASK, HAL_RTC_SRAM_NORMAL_MODE);
}
#endif

#endif

