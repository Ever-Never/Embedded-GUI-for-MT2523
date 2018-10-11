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

#define PMU_K_EOSC_CON0_BASE    ((volatile uint16_t *)(PMU_BASE+0x40))

#define RTC_32K_DEBUG       0

/* #define RTC_XOSC_EXT_CAP */

#define RTC_EOSC            4
#define RTC_K_EOSC          5
#define RTC_MODE            0

static void rtc_wait_busy(void);
static void rtc_unlock_PROT(void);
static void rtc_write_trigger(void);
static void rtc_write_trigger_wait(void);
static void rtc_reload(void);
uint16_t rtc_read_osc32con(void);
static void rtc_write_osc32con1(uint16_t value);
static uint16_t rtc_read_osc32con1(void);
static void rtc_write_osc32con2(uint16_t value);
static uint16_t rtc_read_osc32con2(void);
static void rtc_write_osc32con(uint16_t value);
static void rtc_lpd_init(void);
static void rtc_set_power_key(void);
#ifndef USE_XOSC_MODE
static void rtc_set_scxo_mode(void);
#endif
#ifdef USE_XOSC_MODE
static void rtc_set_xosc_mode(void);
#endif
static bool rtc_is_time_valid(const hal_rtc_time_t *time);
static void rtc_eint_callback(void *user_data);
static void rtc_register_init(void);
static void rtc_init_eint(void);
static void rtc_dump_register(char *tag);
static void rtc_check_alarm_power_on(void);
static void f32k_eosc32_calibration(void);
static void f32k_osc32_Init(void);
uint32_t f32k_measure_clock(uint16_t fixed_clock, uint16_t tested_clock, uint16_t window_setting);

#ifdef HAL_RTC_FEATURE_TIME_CALLBACK
static hal_rtc_time_callback_t rtc_time_callback_function;
static void *rtc_time_user_data;
#endif
static hal_rtc_alarm_callback_t rtc_alarm_callback_function;
static void *rtc_alarm_user_data;
static bool rtc_init_done = false;
static bool rtc_in_test = false;
static char rtc_spare_register_backup[HAL_RTC_BACKUP_BYTE_NUM_MAX];
uint16_t rtc_irq_sta;

RTC_REGISTER_T *rtc_register = (RTC_REGISTER_T *)RTC_BASE;
ABIST_FQMTR_REGISTER_T *abist_fqmtr_register = (ABIST_FQMTR_REGISTER_T *)ABIST_FQMTR_BASE;
volatile uint16_t *pmu_k_eosc_con0 = (volatile uint16_t *)PMU_K_EOSC_CON0_BASE;
volatile uint16_t *hw_version_code = (volatile uint16_t *)0xA2000000;

struct rtc_spare_register_information {
    uint16_t *address;
    uint16_t mask;
    uint16_t offset;
};

static struct rtc_spare_register_information rtc_spare_register_table[HAL_RTC_BACKUP_BYTE_NUM_MAX] = {
    {(uint16_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_AL_HOU), RTC_NEW_SPARE0_MASK, RTC_NEW_SPARE0_OFFSET},
    {(uint16_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_AL_DOM), RTC_NEW_SPARE1_MASK, RTC_NEW_SPARE1_OFFSET},
    {(uint16_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_AL_DOW), RTC_NEW_SPARE2_MASK, RTC_NEW_SPARE2_OFFSET},
    {(uint16_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_AL_MTH), RTC_NEW_SPARE3_MASK, RTC_NEW_SPARE3_OFFSET},
    {(uint16_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_AL_YEA), RTC_NEW_SPARE4_MASK, RTC_NEW_SPARE4_OFFSET},
    {(uint16_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_PDN1), RTC_RTC_PDN1_1_MASK, RTC_RTC_PDN1_1_OFFSET},
    {(uint16_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_PDN1), RTC_RTC_PDN1_2_MASK, RTC_RTC_PDN1_2_OFFSET},
    {(uint16_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_PDN2), RTC_RTC_PDN2_1_MASK, RTC_RTC_PDN2_1_OFFSET},
    {(uint16_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_PDN2), RTC_RTC_PDN2_2_MASK, RTC_RTC_PDN2_2_OFFSET},
    {(uint16_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_SPAR0), RTC_RTC_SPAR0_1_MASK, RTC_RTC_SPAR0_1_OFFSET},
    {(uint16_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_SPAR0), RTC_RTC_SPAR0_2_MASK, RTC_RTC_SPAR0_2_OFFSET},
    {(uint16_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_SPAR1), RTC_RTC_SPAR1_1_MASK, RTC_RTC_SPAR1_1_OFFSET},
    {(uint16_t *) &(((RTC_REGISTER_T *)RTC_BASE)->RTC_SPAR1), RTC_RTC_SPAR1_2_MASK, RTC_RTC_SPAR1_2_OFFSET}
};

#define FQMTR_FCKSEL_RTC_32K            1
#define FQMTR_FCKSEL_EOSC_F32K_CK       4
#define FQMTR_FCKSEL_DCXO_F32K_CK       5
#define FQMTR_FCKSEL_XOSC_F32K_CK       6

//#define FQMTR_TCKSEL_CSW_GP_26M_CK      1
#define FQMTR_TCKSEL_CSW_GP_26M_CK      7 // use 26MHz xtal other than system 26MHz

static void rtc_wait_busy(void)
{
    uint32_t count = 0;

    while (count < 0x6EEEEE) {
        if ((RTC_CBUSY_MASK & rtc_register->RTC_BBPU) == 0) {
            break;
        }
        count++;
    }

    if (count >= 0x6EEEEE) {
        log_hal_error("rtc_wait_busy timeout, RTC_BBPU = %x!", rtc_register->RTC_BBPU);
        log_hal_error("rtc_wait_busy timeout, RTC_32K = %u, EOSC = %u, DCXO = %u, XOSC = %u",
                      (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_RTC_32K, FQMTR_TCKSEL_CSW_GP_26M_CK, 99),
                      (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_EOSC_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 99),
                      (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_DCXO_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 99),
                      (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_XOSC_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 99));
        pmu_set_register_value(PMU_RG_D2D_F32K_CK_PDN_ADDR, PMU_RG_D2D_F32K_CK_PDN_MASK, PMU_RG_D2D_F32K_CK_PDN_SHIFT, 0);
        rtc_dump_register("reopen PMU_32K");
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
    rtc_register->RTC_WRTGR = rtc_register->RTC_WRTGR | RTC_WRTGR_MASK;
}

static void rtc_write_trigger_wait(void)
{
    rtc_write_trigger();;
    rtc_wait_busy();
}

static void rtc_reload(void)
{
    uint16_t value;

    value = rtc_register->RTC_BBPU;
    /* don't clear RTC_ALARM_PU. */
    value &= ~RTC_ALARM_PU_MASK;
    value |= (RTC_KEY_BBPU | (0x1 << RTC_RELOAD_OFFSET));
    rtc_register->RTC_BBPU = value;
    rtc_write_trigger_wait();
}

uint16_t rtc_read_osc32con(void)
{
    uint16_t value;

    value = rtc_register->RTC_AL_MIN;
    value = value & ~RTC_OSC32CON_RD_SEL_MASK;
    value = value | (0x0 << RTC_OSC32CON_RD_SEL_OFFSET);
    rtc_register->RTC_AL_MIN = value;
    rtc_write_trigger_wait();
    rtc_reload();

    return rtc_register->RTC_OSC32CON;
}

static void rtc_write_osc32con(uint16_t value)
{
    rtc_register->RTC_OSC32CON = RTC_OSC32CON_MAGIC_KEY_1;
    rtc_wait_busy();
    rtc_register->RTC_OSC32CON = RTC_OSC32CON_MAGIC_KEY_2;
    rtc_wait_busy();
    rtc_register->RTC_OSC32CON = value;
    rtc_wait_busy();
}

static uint16_t rtc_read_osc32con1(void)
{
    uint16_t value;

    value = rtc_register->RTC_AL_MIN;
    value = value & ~RTC_OSC32CON_RD_SEL_MASK;
    value = value | (0x1 << RTC_OSC32CON_RD_SEL_OFFSET);
    rtc_register->RTC_AL_MIN = value;
    rtc_write_trigger_wait();
    rtc_reload();

    return rtc_register->RTC_OSC32CON;
}

static void rtc_write_osc32con1(uint16_t value)
{
    rtc_register->RTC_OSC32CON = RTC_OSC32CON1_MAGIC_KEY_1;
    rtc_wait_busy();
    rtc_register->RTC_OSC32CON = RTC_OSC32CON1_MAGIC_KEY_2;
    rtc_wait_busy();
    rtc_register->RTC_OSC32CON = value;
    rtc_wait_busy();
}

static uint16_t rtc_read_osc32con2(void)
{
    uint16_t value;

    value = rtc_register->RTC_AL_MIN;
    value = value & ~RTC_OSC32CON_RD_SEL_MASK;
    value = value | (0x2 << RTC_OSC32CON_RD_SEL_OFFSET);
    rtc_register->RTC_AL_MIN = value;
    rtc_write_trigger_wait();
    rtc_reload();

    return rtc_register->RTC_OSC32CON;
}

static void rtc_write_osc32con2(uint16_t value)
{
    rtc_register->RTC_OSC32CON = RTC_OSC32CON2_MAGIC_KEY_1;
    rtc_wait_busy();
    rtc_register->RTC_OSC32CON = RTC_OSC32CON2_MAGIC_KEY_2;
    rtc_wait_busy();
    rtc_register->RTC_OSC32CON = value;
    rtc_wait_busy();
}

static void rtc_lpd_init(void)
{
    uint16_t value;
    uint16_t value_sec;

    value = rtc_register->RTC_CON;
    value = value | RTC_EOSC32_LPEN_MASK | RTC_XOSC32_LPEN_MASK;
    value = value & ~RTC_LPRST_MASK;
    rtc_register->RTC_CON = value;
    rtc_write_trigger_wait();

    value = rtc_register->RTC_CON;
    value = value | RTC_LPRST_MASK;
    rtc_register->RTC_CON = value;
    rtc_write_trigger_wait();

    /* designer suggests delay at least 1 ms */
    hal_gpt_delay_us(10000);

    value = rtc_register->RTC_CON;
    value = value | RTC_EOSC32_LPEN_MASK | RTC_XOSC32_LPEN_MASK;
    value = value & ~RTC_LPRST_MASK;
    rtc_register->RTC_CON = value;
    rtc_write_trigger_wait();

    if ((rtc_register->RTC_CON & RTC_LPSTA_RAW_MASK) != 0) {
        value = rtc_register->RTC_CON;
        value_sec = rtc_register->RTC_AL_SEC;
        log_hal_error("rtc_lpd_init fail : RTC_CON = %x, RTC_AL_SEC = %x!",
                      value, value_sec);
        rtc_dump_register("rtc_lpd_init fail");
        log_hal_error("RTC_32K = %u, EOSC = %u, DCXO = %u, XOSC = %u",
                      (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_RTC_32K, FQMTR_TCKSEL_CSW_GP_26M_CK, 99),
                      (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_EOSC_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 99),
                      (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_DCXO_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 99),
                      (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_XOSC_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 99));
    }
}

static void rtc_set_power_key(void)
{
    uint16_t value;
    rtc_dump_register("Before Set Power Key");
    /* Set powerkey1 and powerkey2 */
    rtc_register->RTC_POWERKEY1 = RTC_POWERKEY1_KEY;
    rtc_register->RTC_POWERKEY2 = RTC_POWERKEY2_KEY;
    rtc_write_trigger_wait();

    /* Inicialize LPD */
    rtc_lpd_init();

    /* Set powerkey1_new and powerkey2_new */
    rtc_register->RTC_POWERKEY1 = RTC_POWERKEY1_KEY;
    rtc_register->RTC_POWERKEY2 = RTC_POWERKEY2_KEY;
    rtc_write_trigger_wait();

    /* POWER_DETECTED will be set after all powerkey match the specific value. */
    rtc_reload();
    if ((rtc_register->RTC_DIFF & RTC_POWER_DETECTED_MASK) == 0) {
        log_hal_error("rtc_set_power_key fail : rtc_diff = %x!", rtc_register->RTC_DIFF);
    }

    rtc_dump_register("After Set Power Key");

    /* Change for RTC_MODE setting start from SDK v4.0.0 to remove 15s waiting time */
    /* Must clear CII_EN to disable tick alarm feature */
    rtc_register->RTC_CII_EN = 0;
    rtc_write_trigger_wait();

    /* set RTC_MODE bit */
    value = rtc_register->RTC_CON;
    value |= RTC_RTC_MODE_MASK;
    rtc_register->RTC_CON = value;
    rtc_write_trigger_wait();
}

#ifndef USE_XOSC_MODE
static void rtc_set_scxo_mode(void)
{
    uint16_t value;

    /* RG_XOSC_CALI[4:0] = [NORMAL]5'b0001111 */
    /* RG_XOSC_STP_PWD = [NORMAL]1'b1 */
    /* RG_XOSC_CGM_LPEN = [NORMAL]1'b1 */
    value = rtc_read_osc32con1();
    value &= ~(RTC_XOSCCALI_MASK | RTC_XOSC_STP_PWD_MASK | RTC_XOSC_CGM_LPEN_MASK);
    value |= (0x7 << RTC_XOSCCALI_OFFSET) | (0x1 << RTC_XOSC_STP_PWD_OFFSET) | (0x1 << RTC_XOSC_CGM_LPEN_OFFSET);
    rtc_write_osc32con1(value);
    /* RG_SCXO_LDO_EN = [NORMAL]1'b1 */
    /* RG_SCXO_LVSH_ENB = [NORMAL]1'b0 */
    /* RG_SCXO_DRV[1:0] = [NORMAL]2'b11 */
    /* RG_SCXO_IB_SET[1:0] = [NORMAL]2'b11 */
    value = rtc_read_osc32con2();
    value &= ~(RTC_SCXO_LDO_EN_MASK | RTC_SCXO_LVSH_ENB_MASK | RTC_SCXO_DRV_MASK | RTC_SCXO_IB_SET_MASK);
    value |= (0x1 << RTC_SCXO_LDO_EN_OFFSET) | (0x0 << RTC_SCXO_LVSH_ENB_OFFSET) |
             (0x3 << RTC_SCXO_DRV_OFFSET) | (0x3 << RTC_SCXO_IB_SET_OFFSET);
    rtc_write_osc32con2(value);
    /* delay 1ms */
    hal_gpt_delay_us(1000);
    /* RG_SCXO_RST = [NORMAL]1'b0 */
    value = rtc_read_osc32con2();
    value &= ~(RTC_SCXO_RST_MASK);
    value |= (0x0 << RTC_SCXO_RST_OFFSET);
    rtc_write_osc32con2(value);
    /* delay 1ms */
    hal_gpt_delay_us(1000);
    /* RG_SCXO_EN = [NORMAL]1'b1 */
    value = rtc_read_osc32con2();
    value &= ~(RTC_SCXO_EN_MASK);
    value |= (0x1 << RTC_SCXO_EN_OFFSET);
    rtc_write_osc32con2(value);
    do {
        /* delay 1ms */
        hal_gpt_delay_us(1000);
        /* RG_SCXO_CK_ENB = [NORMAL]1'b1 */
        value = rtc_read_osc32con2();
    } while ((value & RTC_SCXO_EN_MASK) == 0x0);
    value &= ~(RTC_SCXO_CK_ENB_MASK);
    value |= (0x1 << RTC_SCXO_CK_ENB_OFFSET);
    rtc_write_osc32con2(value);
    /* RG_SCXO_CHP_EN   [NORMAL]1'b1 */
    hal_gpt_delay_us(1000);
    value = rtc_read_osc32con2();
    hal_gpt_delay_us(1000);
    value = rtc_read_osc32con2();
    value &= ~(RTC_SCXO_CHP_EN_MASK);
    value |= (0x1 << RTC_SCXO_CHP_EN_OFFSET);
    rtc_write_osc32con2(value);
}
#endif

#ifdef USE_XOSC_MODE
static void rtc_set_xosc_mode(void)
{
    uint16_t value;

    /* RTC_OSC32CON1[5](RG_XOSC_STP_PWD) = 1 */
    /* RTC_OSC32CON1[6](RG_XOSC_CGM_LPEN) = 0 */
    value = rtc_read_osc32con1();
    value &= ~(RTC_XOSCCALI_MASK | RTC_XOSC_STP_PWD_MASK | RTC_XOSC_CGM_LPEN_MASK);
    value |= (0x1 << RTC_XOSC_STP_PWD_OFFSET);
    rtc_write_osc32con1(value);
    /* RTC_OSC32CON2[4](RG_SCXO_SW_EN) = 1 */
    /* RTC_OSC32CON2[11](RG_SCXO_CK_ENB) = 1 */
    value = rtc_read_osc32con2();
    value |= (0x1 << RTC_SCXO_SW_EN_OFFSET) | (0x1 << RTC_SCXO_CK_ENB_OFFSET);
    rtc_write_osc32con2(value);
}
#endif

static bool rtc_is_time_valid(const hal_rtc_time_t *time)
{
    bool result = true;

    if (time->rtc_year > 127) {
        log_hal_error("Invalid year : %d", time->rtc_year);
        result = false;
    }

    if ((time->rtc_mon == 0) || (time->rtc_mon > 12)) {
        log_hal_error("Invalid month : %d", time->rtc_mon);
        result = false;
    }

    if (time->rtc_week > 6) {
        log_hal_info("Invalid day of week : %d", time->rtc_week);
    }

    if ((time->rtc_day == 0) || (time->rtc_day > 31)) {
        log_hal_error("Invalid day of month : %d", time->rtc_day);
        result = false;
    }

    if (time->rtc_hour > 23) {
        log_hal_error("Invalid hour : %d", time->rtc_hour);
        result = false;
    }

    if (time->rtc_min > 59) {
        log_hal_error("Invalid minute : %d", time->rtc_min);
        result = false;
    }

    if (time->rtc_sec > 59) {
        log_hal_error("Invalid second : %d", time->rtc_sec);
        result = false;
    }

    return result;
}

static void rtc_eint_callback(void *user_data)
{
    uint16_t value;

    hal_eint_mask(HAL_EINT_RTC);

    /* read clear interrupt status */
    value = rtc_register->RTC_IRQ_STA;
    /* clear alarm power on */
    rtc_register->RTC_BBPU = RTC_KEY_BBPU | (0x1 << RTC_ALARM_PU_OFFSET);
    rtc_write_trigger_wait();

    if ((value & RTC_ALSTA_MASK) != 0) {
        /* disable alarm interrupt */
        rtc_register->RTC_IRQ_EN = rtc_register->RTC_IRQ_EN & (~RTC_AL_EN_MASK);
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
    /* Clear ALARM_PU */
    rtc_register->RTC_BBPU = RTC_KEY_BBPU | (0x1 << RTC_ALARM_PU_OFFSET);
    /* Read clear */
    rtc_irq_sta = rtc_register->RTC_IRQ_STA;
    rtc_register->RTC_IRQ_EN = 0x0;
    rtc_register->RTC_AL_MASK = 0x10;
    rtc_register->RTC_TC_SEC = 0x0;
    rtc_register->RTC_TC_MIN = 0x0;
    rtc_register->RTC_TC_HOU = 0x0;
    rtc_register->RTC_TC_DOM = 0x1;
    rtc_register->RTC_TC_DOW = 0x1;
    rtc_register->RTC_TC_MTH = 0x1;
    rtc_register->RTC_TC_YEA = 0x0;

    rtc_register->RTC_AL_SEC = 0x0;
    rtc_register->RTC_AL_MIN = 0x0;
    rtc_register->RTC_AL_HOU = 0x0;
    rtc_register->RTC_AL_DOM = 0x1;
    rtc_register->RTC_AL_DOW = 0x1;
    rtc_register->RTC_AL_MTH = 0x1;
    rtc_register->RTC_AL_YEA = 0x0;

    rtc_register->RTC_PDN1 = 0x0;
    rtc_register->RTC_PDN2 = 0x0;
    rtc_register->RTC_SPAR0 = 0x0;
    rtc_register->RTC_SPAR1 = 0x0;
    rtc_register->RTC_DIFF = 0x0;
    rtc_register->RTC_CALI = 0x0;
    rtc_write_trigger_wait();
    rtc_register->RTC_CALI = 0x4000;
    rtc_register->RTC_CON = 0x1400;

    rtc_write_trigger_wait();
}

static void rtc_init_eint(void)
{
    hal_eint_config_t eint_config;
    hal_eint_status_t result;

    eint_config.trigger_mode = HAL_EINT_LEVEL_LOW;
    eint_config.debounce_time = 0;
    result = hal_eint_init(HAL_EINT_RTC, &eint_config);
    if (result != HAL_EINT_STATUS_OK) {
        log_hal_error("hal_eint_init fail: %d", result);
        return;
    }

    result = hal_eint_register_callback(HAL_EINT_RTC, rtc_eint_callback, NULL);
    if (result != HAL_EINT_STATUS_OK) {
        log_hal_error("hal_eint_register_callback fail: %d", result);
        return;
    }

    result = hal_eint_unmask(HAL_EINT_RTC);
    if (result != HAL_EINT_STATUS_OK) {
        log_hal_error("hal_eint_unmask fail: %d", result);
        return;
    }
}

static void rtc_dump_register(char *tag)
{
    uint16_t value_key1, value_key2, value_diff, value_con, value_bbpu, value_sec;
    rtc_reload();
    value_key1 = rtc_register->RTC_POWERKEY1;
    value_key2 = rtc_register->RTC_POWERKEY2;
    value_diff = rtc_register->RTC_DIFF;
    value_con = rtc_register->RTC_CON;
    value_bbpu = rtc_register->RTC_BBPU;
    value_sec = rtc_register->RTC_AL_SEC;
    log_hal_warning("rtc_dump_register[%s], RTC_POWERKEY1 = %x, RTC_POWERKEY2 = %x, RTC_DIFF = %x, RTC_CON = %x",
                    tag, value_key1, value_key2, value_diff, value_con);
    log_hal_warning("RTC_BBPU = %x, RTC_AL_SEC = %x, RTC_CON32CON = %x, RTC_CON32CON1 = %x, RTC_CON32CON2 = %x, %d, %d",
                    value_bbpu, value_sec, rtc_read_osc32con(), rtc_read_osc32con1(), rtc_read_osc32con2(),
                    pmu_get_register_value(PMU_RTC_XOSC32_ENB_ADDR, PMU_RTC_XOSC32_ENB_MASK, PMU_RTC_XOSC32_ENB_SHIFT),
                    pmu_get_register_value(PMU_RTC_K_EOSC32_EN_ADDR, PMU_RTC_K_EOSC32_EN_MASK, PMU_RTC_K_EOSC32_EN_SHIFT));
}

static void rtc_check_alarm_power_on(void)
{
    hal_rtc_time_t time;

    if ((rtc_register->RTC_BBPU & RTC_ALARM_PU_MASK) != 0) {
        hal_rtc_get_time(&time);
        log_hal_warning("time : %d/%d/%d %d:%d:%d", time.rtc_year, time.rtc_mon, time.rtc_day,
                        time.rtc_hour, time.rtc_min, time.rtc_sec);
        hal_rtc_get_alarm(&time);
        log_hal_warning("alarm : %d/%d/%d %d:%d:%d", time.rtc_year, time.rtc_mon, time.rtc_day,
                        time.rtc_hour, time.rtc_min, time.rtc_sec);
        log_hal_warning("Alarm power on, %x", rtc_register->RTC_BBPU);
        /* clear alarm power on */
        rtc_register->RTC_BBPU = RTC_KEY_BBPU | (0x1 << RTC_ALARM_PU_OFFSET);
        rtc_write_trigger_wait();
    }
}

static void rtc_enable_2sec_reboot(void)
{
    uint16_t value;

    value = pmu_get_register_value(PMU_RTC_SPAR_2SEC_STAT_STA_ADDR,
                                   PMU_RTC_SPAR_2SEC_STAT_STA_MASK, PMU_RTC_SPAR_2SEC_STAT_STA_SHIFT);
    if (value != 0) {
        log_hal_warning("2sec reboot happened");
        pmu_set_register_value(PMU_RTC_SPAR_2SEC_STAT_CLEAR_ADDR,
                               PMU_RTC_SPAR_2SEC_STAT_CLEAR_MASK, PMU_RTC_SPAR_2SEC_STAT_CLEAR_SHIFT, 1);
        pmu_set_register_value(PMU_RTC_SPAR_2SEC_STAT_CLEAR_ADDR,
                               PMU_RTC_SPAR_2SEC_STAT_CLEAR_MASK, PMU_RTC_SPAR_2SEC_STAT_CLEAR_SHIFT, 0);
    }

    pmu_set_register_value(PMU_RTC_SPAR_2SEC_MODE_ADDR,
                           PMU_RTC_SPAR_2SEC_MODE_MASK, PMU_RTC_SPAR_2SEC_MODE_SHIFT, 2);
    pmu_set_register_value(PMU_RTC_SPAR_2SEC_EN_ADDR,
                           PMU_RTC_SPAR_2SEC_EN_MASK, PMU_RTC_SPAR_2SEC_EN_SHIFT, 1);
    value = rtc_read_osc32con();
    value &= ~(RTC_RTC_D2D_32K_CG_EN_MASK);
    value |= (0x1 << RTC_RTC_D2D_32K_CG_EN_OFFSET);
    rtc_write_osc32con(value);
}

static void f32k_eosc32_calibration(void)
{
    uint16_t value;

    uint16_t low_xosccali = 0x00;
    uint16_t high_xosccali = 0x1f;
    uint16_t medium_xosccali;

    uint32_t low_frequency = 0;
    uint32_t high_frequency = 0;
    uint32_t medium_frequency;

    value = rtc_read_osc32con1();
    value &= ~RTC_XOSCCALI_MASK;
    value |= (low_xosccali << RTC_XOSCCALI_OFFSET);
    rtc_write_osc32con1(value);
    high_frequency = f32k_measure_clock(FQMTR_FCKSEL_EOSC_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 99);
    if (high_frequency <= 32768) {
        log_hal_info("high_frequency <= 32768, frequency = %u, xosccali = %d", (unsigned int)high_frequency, (unsigned int)low_xosccali);
        return;
    }

    value = rtc_read_osc32con1();
    value &= ~RTC_XOSCCALI_MASK;
    value |= (high_xosccali << RTC_XOSCCALI_OFFSET);
    rtc_write_osc32con1(value);
    low_frequency = f32k_measure_clock(FQMTR_FCKSEL_EOSC_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 99);
    if (low_frequency >= 32768) {
        log_hal_info("low_frequency >= 32768, frequency = %u, xosccali = %d", (unsigned int)low_frequency, (unsigned int)high_xosccali);
        return;
    }

    while ((high_xosccali - low_xosccali) > 1) {
        medium_xosccali = (low_xosccali + high_xosccali) / 2;
        value = rtc_read_osc32con1();
        value &= ~RTC_XOSCCALI_MASK;
        value |= (medium_xosccali << RTC_XOSCCALI_OFFSET);
        rtc_write_osc32con1(value);
        medium_frequency = f32k_measure_clock(FQMTR_FCKSEL_EOSC_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 99);
        if (medium_frequency > 32768) {
            low_xosccali = medium_xosccali;
            high_frequency = medium_frequency;
        } else if (medium_frequency < 32768) {
            high_xosccali = medium_xosccali;
            low_frequency = medium_frequency;
        } else {
            log_hal_info("xosccali = %d", medium_xosccali);
            return;
        }
    }

    if ((32768 - low_frequency) < (high_frequency - 32768)) {
        value = rtc_read_osc32con1();
        value &= ~RTC_XOSCCALI_MASK;
        value |= (high_xosccali << RTC_XOSCCALI_OFFSET);
        rtc_write_osc32con1(value);
        log_hal_info("frequency = %u, xosccali = %d", (unsigned int)low_frequency, (unsigned int)high_xosccali);
    } else {
        value = rtc_read_osc32con1();
        value &= ~RTC_XOSCCALI_MASK;
        value |= (low_xosccali << RTC_XOSCCALI_OFFSET);
        rtc_write_osc32con1(value);
        log_hal_info("frequency = %u, xosccali = %d", (unsigned int)high_frequency, (unsigned int)low_xosccali);
    }
}

#if RTC_32K_DEBUG
static hal_rtc_status_t rtc_wait_second_changed()
{
    hal_rtc_time_t time;
    uint16_t second;

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
    uint16_t value;
    uint32_t frequency;
    bool use_xosc = false;
    int16_t cali;
    uint16_t value_diff, value_con;

    if ((rtc_read_osc32con() & RTC_RTC_XOSC32_ENB_MASK) == 0) {
        log_hal_info("Use 32k crystal, %x", rtc_read_osc32con());
        use_xosc = true;
    } else {
        log_hal_info("No 32k crystal, %x", rtc_read_osc32con());
        /* VA should always on during sleep mode. */
        pmu_set_register_value(PMU_RTC_K_EOSC32_EN_ADDR, PMU_RTC_K_EOSC32_EN_MASK, PMU_RTC_K_EOSC32_EN_SHIFT, 0);
    }

    if ((rtc_read_osc32con() & RTC_RTC_32K_EN_MASK) == 0) {
        /* PORSTB happened; */
        log_hal_warning("PORSTB occurred.");
        /* D-Die : RTC_REG_XOSC32_ENB, XRTC_EN, RTC_32K_EN, RTC_AREA_CK_CGEN and RTC_EMBCK_SEL_MODE have been reset. */
        /* A-Die : RTC_XOSC32_ENB and RTC_K_EOSC32_EN have been reset. */

        if (((rtc_register->RTC_DIFF & RTC_POWER_DETECTED_MASK) == 0) ||
            ((rtc_register->RTC_CON & RTC_LPSTA_RAW_MASK) != 0)) {
            /* Initial OSC32CON before set power key. */
            value = rtc_read_osc32con1();
            value = 0x180;
            rtc_write_osc32con1(value);
            value = rtc_read_osc32con2();
            value = 0x022C;
            rtc_write_osc32con2(value);
        }

        /* Set A die XOSC32_ENB register and RTC_REG_XOSC32_ENB with pin config if PORSTB happened. */
        if (use_xosc) {
            /* Use 32k crystal */

            /* I2C write A die(RTC_MISC_CON0, 0x00D2, 0); */
            /* VA can off during sleep/off mode. */
            pmu_set_register_value(PMU_RTC_XOSC32_ENB_ADDR, PMU_RTC_XOSC32_ENB_MASK, PMU_RTC_XOSC32_ENB_SHIFT, 0);
            /* RTC_OSC32CON[15] = RTC_OSC32CON[5] = 0 */
            /* RTC_OSC32CON[12] = 1 */
            value = rtc_read_osc32con();
            value &= ~(RTC_RTC_REG_XOSC32_ENB_MASK | RTC_RTC_32K_EN_MASK);
            value |= (0x0 << RTC_RTC_REG_XOSC32_ENB_OFFSET) | (0x1 << RTC_RTC_32K_EN_OFFSET);
            rtc_write_osc32con(value);
        } else {
            /* No 32k crystal */

            /* I2C write A die(RTC_MISC_CON0, 0x00D2, 1); */
            /* VA should on during sleep mode. */
            pmu_set_register_value(PMU_RTC_XOSC32_ENB_ADDR, PMU_RTC_XOSC32_ENB_MASK, PMU_RTC_XOSC32_ENB_SHIFT, 1);
            /* RTC_OSC32CON[15] = RTC_OSC32CON[5] = 1 */
            /* RTC_OSC32CON[12] = 1 */
            /* RTC_OSC32CON[8] = 0 */
            /* RTC_OSC32CON[7:6] = 2'b01 */
            value = rtc_read_osc32con();
            value &= ~(RTC_RTC_REG_XOSC32_ENB_MASK | RTC_RTC_32K_EN_MASK | RTC_RTC_EMBCK_SRC_SEL_MASK | RTC_RTC_EMBCK_SEL_MODE_MASK);
            value |= (0x1 << RTC_RTC_REG_XOSC32_ENB_OFFSET) | (0x1 << RTC_RTC_32K_EN_OFFSET) |
                     (0x0 << RTC_RTC_EMBCK_SRC_SEL_OFFSET) | (0x1 << RTC_RTC_EMBCK_SEL_MODE_OFFSET);
            rtc_write_osc32con(value);
        }

        if (((rtc_register->RTC_DIFF & RTC_POWER_DETECTED_MASK) == 0) ||
            ((rtc_register->RTC_CON & RTC_LPSTA_RAW_MASK) != 0)) {
            /* LPD */
            value_diff = rtc_register->RTC_DIFF;
            value_con = rtc_register->RTC_CON;
            log_hal_warning("LPD occurred, rtc_diff: %x, rtc_con: %x, rtc_al_sec = %x",
                            value_diff, value_con, rtc_register->RTC_AL_SEC);

            rtc_register_init();

            if (use_xosc) {
                /* Use 32k crystal */

                /* Don't check EOSC LPD when using 32k crystal for saving power. */
                value = rtc_register->RTC_AL_SEC;
                value &= ~RTC_RTC_LPD_OPT_MASK;
                value |= (0x2 << RTC_RTC_LPD_OPT_OFFSET);
                rtc_register->RTC_AL_SEC = value;
                rtc_write_trigger_wait();

                /* Set power key */
                rtc_set_power_key();

                /* Disable EOSC LPD when using 32k crystal for saving power. */
                value = rtc_register->RTC_CON;
                value &= ~RTC_EOSC32_LPEN_MASK;
                rtc_register->RTC_CON = value;
                rtc_write_trigger_wait();

                /* Disable PMU 32K CG */
                /* pmu_set_register_value(PMU_RG_D2D_F32K_CK_PDN_ADDR, PMU_RG_D2D_F32K_CK_PDN_MASK, PMU_RG_D2D_F32K_CK_PDN_SHIFT, 1); */
#ifndef USE_XOSC_MODE
                /* Set SCXO mode */
                log_hal_info("RTC_SCXO");
                rtc_set_scxo_mode();
                /* wait for scxo stable */
                hal_gpt_delay_us(50000);
#else
#ifndef RTC_XOSC_EXT_CAP
                log_hal_info("RTC_XOSC_INT_CAP");
                /* workaround, disable LPD due to we need to switch to 32k-less mode. */
                value = rtc_register->RTC_AL_SEC;
                value &= ~RTC_RTC_LPD_OPT_MASK;
                value |= (0x3 << RTC_RTC_LPD_OPT_OFFSET);
                rtc_register->RTC_AL_SEC = value;
                rtc_write_trigger_wait();
                rtc_set_xosc_mode();
                /* Set to internal cap. */
                value = rtc_read_osc32con1();
                value &= ~(RTC_XOSC_CL_EN_MASK);
                value |= (0x1 << RTC_XOSC_CL_EN_OFFSET);
                rtc_write_osc32con1(value);
#else
                log_hal_info("RTC_XOSC_EXT_CAP");
                /* workaround, disable LPD due to we need to switch to 32k-less mode. */
                value = rtc_register->RTC_AL_SEC;
                value &= ~RTC_RTC_LPD_OPT_MASK;
                value |= (0x3 << RTC_RTC_LPD_OPT_OFFSET);
                rtc_register->RTC_AL_SEC = value;
                rtc_write_trigger_wait();
                rtc_set_xosc_mode();
                /* Set to external cap. */
                value = rtc_read_osc32con1();
                value &= ~(RTC_XOSC_CL_EN_MASK);
                value |= (0x0 << RTC_XOSC_CL_EN_OFFSET);
                rtc_write_osc32con1(value);
#endif
#endif
                /* Measure SCXO 32K */
                frequency = f32k_measure_clock(FQMTR_FCKSEL_XOSC_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 99);
                /* Set RTC_CALI */
                value = rtc_register->RTC_CALI;
                value &= ~RTC_RTC_CALI_MASK;

                cali = (32768 - frequency) << 3;
                if (*hw_version_code == 0) {
                    if (cali > (int16_t)0x3F) {
                        cali = 0x3F;
                    } else if (cali < (int16_t)0xFFC0) {
                        cali = 0xFFC0;
                    }
                } else {
                    if (cali > (int16_t)0x1FFF) {
                        cali = 0x1FFF;
                    } else if (cali < (int16_t)0xE000) {
                        cali = 0xE000;
                    }
                }
                value |= ((cali << RTC_RTC_CALI_OFFSET) & RTC_RTC_CALI_MASK);
                rtc_register->RTC_CALI = value;
                rtc_write_trigger_wait();
                log_hal_info("xosc frequency = %u, RTC_CALI = %x", (unsigned int)frequency, rtc_register->RTC_CALI);
            } else {
                /* No 32k crystal */

                /* Set A die RTC_XOSC32_ENB */
                /* Select DCXO 32K, switch to RTC 32K and set REG_XOSC32_ENB */
                /* Set power key */
#if 1
                /* workaround, disable EOSC LPD when 32k-less. */
                value = rtc_register->RTC_AL_SEC;
                value &= ~RTC_RTC_LPD_OPT_MASK;
                value |= (0x2 << RTC_RTC_LPD_OPT_OFFSET);
                rtc_register->RTC_AL_SEC = value;
                rtc_write_trigger_wait();
#endif
                rtc_set_power_key();

                /* Designer's suggestion */
                value = rtc_read_osc32con1();
                value &= ~(RTC_XOSC_STP_PWD_MASK);
                value |= (0x1 << RTC_XOSC_STP_PWD_OFFSET);
                rtc_write_osc32con1(value);
                /* Need to toggle LPD_RST again between set RTC_XOSC_STP_PWD and enable LPD */
                rtc_lpd_init();
                /* Enalbe XOSC and EOSC LPD. */
                value = rtc_register->RTC_AL_SEC;
                value &= ~RTC_RTC_LPD_OPT_MASK;
                value |= (0x0 << RTC_RTC_LPD_OPT_OFFSET);
                rtc_register->RTC_AL_SEC = value;
                rtc_write_trigger_wait();

                /* Disable PMU 32K CG */
                /* I2C write A die(PMIC_CKCFG6, 0x0017, PMIC_CKCFG6|0x2) */
                pmu_set_register_value(PMU_RG_D2D_F32K_CK_PDN_ADDR, PMU_RG_D2D_F32K_CK_PDN_MASK, PMU_RG_D2D_F32K_CK_PDN_SHIFT, 1);
            }
        }
    }

    if (!use_xosc) {
        /* rtc_dump_register("Before eosc32 calibration."); */
        f32k_eosc32_calibration();
        /* rtc_dump_register("After eosc32 calibration."); */


        /* RTC_OSC32CON[8] = 0 */
        /* RTC_OSC32CON[7:6] = 2'b01 */
        value = rtc_read_osc32con();
        value &= ~(RTC_RTC_EMBCK_SRC_SEL_MASK | RTC_RTC_EMBCK_SEL_MODE_MASK);
        value |= (0x0 << RTC_RTC_EMBCK_SRC_SEL_OFFSET) | (0x1 << RTC_RTC_EMBCK_SEL_MODE_OFFSET);
        rtc_write_osc32con(value);

        /* workaround, embedded clock switch back to dcxo decided by powerkey_match only. */
        value = rtc_read_osc32con();
        value &= ~(RTC_RTC_EMBCK_SEL_OPTION_MASK);
        value |= (0x1 << RTC_RTC_EMBCK_SEL_OPTION_OFFSET);
        rtc_write_osc32con(value);
#if (RTC_MODE == RTC_EOSC)
        log_hal_error("RTC_EOSC");
        /* switch to EMB_SW_EOSC32 mode. */
        value = rtc_read_osc32con();
        value &= ~(RTC_RTC_EMBCK_SEL_MODE_MASK);
        value |= (0x3 << RTC_RTC_EMBCK_SEL_MODE_OFFSET);
        rtc_write_osc32con(value);
#elif (RTC_MODE == RTC_K_EOSC)
        log_hal_error("RTC_K_EOSC");
        /* switch to EMB_K_EOSC32 mode, f32k_ck src = eosc32_ck. */
        value = rtc_read_osc32con();
        value &= ~(RTC_RTC_EMBCK_SRC_SEL_MASK);
        value |= (0x1 << RTC_RTC_EMBCK_SRC_SEL_OFFSET);
        rtc_write_osc32con(value);

        value = rtc_register->RTC_DIFF;
        value |= (0x1 << RTC_CALI_RD_SEL_OFFSET);
        rtc_register->RTC_DIFF = value;

        *pmu_k_eosc_con0 = 0xE000;
        log_hal_error("pmu_k_eosc_con0 = %x", *pmu_k_eosc_con0);
#endif
    }

    rtc_dump_register("f32k_osc32_Init done");

#if RTC_32K_DEBUG
    while (1) {
        hal_rtc_time_t time;
        hal_rtc_get_time(&time);
        log_hal_warning("time : %d/%d/%d %d:%d:%d", time.rtc_year, time.rtc_mon, time.rtc_day,
                        time.rtc_hour, time.rtc_min, time.rtc_sec);
#endif
        log_hal_warning("RTC_32K = %u, EOSC = %u, DCXO = %u, XOSC = %u",
                        (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_RTC_32K, FQMTR_TCKSEL_CSW_GP_26M_CK, 99),
                        (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_EOSC_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 99),
                        (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_DCXO_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 99),
                        (unsigned int)f32k_measure_clock(FQMTR_FCKSEL_XOSC_F32K_CK, FQMTR_TCKSEL_CSW_GP_26M_CK, 99));
#if RTC_32K_DEBUG
        rtc_dump_register("debug dump");
        rtc_wait_second_changed();
    }
#endif
}

uint32_t f32k_measure_clock(uint16_t fixed_clock, uint16_t tested_clock, uint16_t window_setting)
{
    uint32_t fqmtr_data;
    uint32_t frequency;
    uint16_t fqmtr_data_msb;

    /* 1) PLL_ABIST_FQMTR_CON0 = 0xCXXX */
    abist_fqmtr_register->ABIST_FQMTR_CON0 |= 0xC000;
    // FQMTR's busy didn't set immediately, need 2~3T fixed_ck(32k) latency
    // Now the lowest EOSC is 18k, equal to almost 0.2ms. Now delay 1ms for safe.
    hal_gpt_delay_us(1000);
    while ((abist_fqmtr_register->ABIST_FQMTR_CON1 & 0x8000) != 0);
    /* 2) PLL_ABIST_FQMTR_CON1 = 0x0 */
    abist_fqmtr_register->ABIST_FQMTR_CON1 = 0;
    /* 3) PLL_ABIST_FQMTR_CON1 = 0x0601 */
    abist_fqmtr_register->ABIST_FQMTR_CON1 = (fixed_clock << 8) | tested_clock;
    abist_fqmtr_register->ABIST_FQMTR_CON2 = 0;
    /* 4) PLL_ABIST_FQMTR_CON0 = 0x8009 */
    abist_fqmtr_register->ABIST_FQMTR_CON0 = 0x8000 | window_setting;
    hal_gpt_delay_us(1000);
    /* 5) Wait PLL_ABIST_FQMTR_CON1 & 0x8000 == 0x8000 */
    while ((abist_fqmtr_register->ABIST_FQMTR_CON1 & 0x8000) != 0);
    /* 6) Read PLL_ABIST_FQMTR_DATA */
    fqmtr_data_msb = abist_fqmtr_register->ABIST_FQMTR_DATA_MSB;
    fqmtr_data = ((uint32_t)fqmtr_data_msb << 16) | abist_fqmtr_register->ABIST_FQMTR_DATA;
    /* 7) Freq = 26000000*10/PLL_ABIST_FQMTR_DATA */
    frequency = 26000000 * (window_setting + 1) / fqmtr_data;
#if 0
    if ((frequency >= 100000) || (frequency < 10000) || (abist_fqmtr_register->ABIST_FQMTR_CON1 & 0x8000) != 0)
        log_hal_info("fqmtr_con0 = %x, fqmtr_con1 = %x, fqmtr_con2 = %x, fqmtr_data = %u, frequency = %u",
                     abist_fqmtr_register->ABIST_FQMTR_CON0, abist_fqmtr_register->ABIST_FQMTR_CON1, abist_fqmtr_register->ABIST_FQMTR_CON2,
                     (unsigned int)fqmtr_data, frequency);
#endif

    return frequency;
}

uint32_t f32k_measure_count(uint16_t fixed_clock, uint16_t tested_clock, uint16_t window_setting)
{
    uint32_t fqmtr_data;
    uint16_t fqmtr_data_msb;

    /* 1) PLL_ABIST_FQMTR_CON0 = 0xCXXX */
    abist_fqmtr_register->ABIST_FQMTR_CON0 |= 0xC000;
    // FQMTR's busy didn't set immediately, need 2~3T fixed_ck(32k) latency
    // Now the lowest EOSC is 18k, equal to almost 0.2ms. Now delay 1ms for safe.
    hal_gpt_delay_us(1000);
    while ((abist_fqmtr_register->ABIST_FQMTR_CON1 & 0x8000) != 0);
    /* 2) PLL_ABIST_FQMTR_CON1 = 0x0 */
    abist_fqmtr_register->ABIST_FQMTR_CON1 = 0;
    /* 3) PLL_ABIST_FQMTR_CON1 = 0x0601 */
    abist_fqmtr_register->ABIST_FQMTR_CON1 = (fixed_clock << 8) | tested_clock;
    abist_fqmtr_register->ABIST_FQMTR_CON2 = 0;
    /* 4) PLL_ABIST_FQMTR_CON0 = 0x8009 */
    abist_fqmtr_register->ABIST_FQMTR_CON0 = 0x8000 | window_setting;
    hal_gpt_delay_us(1000);
    /* 5) Wait PLL_ABIST_FQMTR_CON1 & 0x8000 == 0x8000 */
    while ((abist_fqmtr_register->ABIST_FQMTR_CON1 & 0x8000) != 0);
    /* 6) Read PLL_ABIST_FQMTR_DATA */
    fqmtr_data_msb = abist_fqmtr_register->ABIST_FQMTR_DATA_MSB;
    fqmtr_data = ((uint32_t)fqmtr_data_msb << 16) | abist_fqmtr_register->ABIST_FQMTR_DATA;

    return fqmtr_data;
}

hal_rtc_status_t hal_rtc_init(void)
{
    if (true == rtc_init_done) {
        return HAL_RTC_STATUS_OK;
    }

    rtc_wait_busy();

    rtc_unlock_PROT();

    rtc_dump_register("Just After Power On");

    f32k_osc32_Init();

    rtc_check_alarm_power_on();

    rtc_enable_2sec_reboot();

    rtc_init_eint();

    rtc_init_done = true;

    rtc_dump_register("hal_rtc_init done");

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_deinit(void)
{
    bool use_xosc = false;
    uint16_t value;

    if ((rtc_read_osc32con() & RTC_RTC_XOSC32_ENB_MASK) == 0) {
        log_hal_info("Use 32k crystal, %x", rtc_read_osc32con());
        use_xosc = true;
    } else {
        log_hal_info("No 32k crystal, %x", rtc_read_osc32con());
    }

    if (false == use_xosc) {
        value = rtc_register->RTC_DIFF;
        value |= (0x1 << RTC_CALI_RD_SEL_OFFSET);
        rtc_register->RTC_DIFF = value;

        *pmu_k_eosc_con0 = 0xE000;

        /* switch to EMB_K_EOSC32 mode, f32k_ck src = eosc32_ck. */
        value = rtc_read_osc32con();
        value &= ~(RTC_RTC_EMBCK_SRC_SEL_MASK);
        value |= (0x1 << RTC_RTC_EMBCK_SRC_SEL_OFFSET);
        rtc_write_osc32con(value);

        /* VA can on/off during off mode. */
        pmu_set_register_value(PMU_RTC_K_EOSC32_EN_ADDR, PMU_RTC_K_EOSC32_EN_MASK, PMU_RTC_K_EOSC32_EN_SHIFT, 1);
    }

    rtc_dump_register("hal_rtc_deinit done");

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_set_time(const hal_rtc_time_t *time)
{
    uint16_t value;

    if (!rtc_is_time_valid(time)) {
        return HAL_RTC_STATUS_INVALID_PARAM;
    }

    value = rtc_register->RTC_TC_YEA;
    value &= ~RTC_TC_YEAR_MASK;
    value |= (time->rtc_year << RTC_TC_YEAR_OFFSET);
    rtc_register->RTC_TC_YEA = value;

    value = rtc_register->RTC_TC_MTH;
    value &= ~RTC_TC_MONTH_MASK;
    value |= (time->rtc_mon << RTC_TC_MONTH_OFFSET);
    rtc_register->RTC_TC_MTH = value;

    value = rtc_register->RTC_TC_DOW;
    value &= ~RTC_TC_DOW_MASK;
    value |= (((time->rtc_week > 6) ? 7 : (time->rtc_week + 1)) << RTC_TC_DOW_OFFSET);
    rtc_register->RTC_TC_DOW = value;

    value = rtc_register->RTC_TC_DOM;
    value &= ~RTC_TC_DOM_MASK;
    value |= (time->rtc_day << RTC_TC_DOM_OFFSET);
    rtc_register->RTC_TC_DOM = value;

    value = rtc_register->RTC_TC_HOU;
    value &= ~RTC_TC_HOUR_MASK;
    value |= (time->rtc_hour << RTC_TC_HOUR_OFFSET);
    rtc_register->RTC_TC_HOU = value;

    value = rtc_register->RTC_TC_MIN;
    value &= ~RTC_TC_MINUTE_MASK;
    value |= (time->rtc_min << RTC_TC_MINUTE_OFFSET);
    rtc_register->RTC_TC_MIN = value;

    value = rtc_register->RTC_TC_SEC;
    value &= ~RTC_TC_SECOND_MASK;
    value |= (time->rtc_sec << RTC_TC_SECOND_OFFSET);
    rtc_register->RTC_TC_SEC = value;

    rtc_write_trigger_wait();

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_get_time(hal_rtc_time_t *time)
{
    uint16_t value_sec, value;
    /* re-read time information if second value changed */
    do {
        value_sec = (rtc_register->RTC_TC_SEC & RTC_TC_SECOND_MASK) >> RTC_TC_SECOND_OFFSET;
        time->rtc_year = (rtc_register->RTC_TC_YEA & RTC_TC_YEAR_MASK) >> RTC_TC_YEAR_OFFSET;
        time->rtc_mon = (rtc_register->RTC_TC_MTH & RTC_TC_MONTH_MASK) >> RTC_TC_MONTH_OFFSET;
        time->rtc_week = ((rtc_register->RTC_TC_DOW & RTC_TC_DOW_MASK) >> RTC_TC_DOW_OFFSET) - 1;
        time->rtc_day = (rtc_register->RTC_TC_DOM & RTC_TC_DOM_MASK) >> RTC_TC_DOM_OFFSET;
        time->rtc_hour = (rtc_register->RTC_TC_HOU & RTC_TC_HOUR_MASK) >> RTC_TC_HOUR_OFFSET;
        time->rtc_min = (rtc_register->RTC_TC_MIN & RTC_TC_MINUTE_MASK) >> RTC_TC_MINUTE_OFFSET;
        value = (rtc_register->RTC_TC_SEC & RTC_TC_SECOND_MASK) >> RTC_TC_SECOND_OFFSET;
    } while ((value_sec & RTC_TC_SECOND_MASK) != (value & RTC_TC_SECOND_MASK));
    time->rtc_sec = value;

    return HAL_RTC_STATUS_OK;
}

#ifdef HAL_RTC_FEATURE_TIME_CALLBACK
hal_rtc_status_t hal_rtc_set_time_notification_period(hal_rtc_time_notification_period_t period)
{
    uint16_t value;
    uint16_t enable;
    uint16_t cii_setting;

    switch (period) {
        case HAL_RTC_TIME_NOTIFICATION_NONE:
            enable = 0;
            cii_setting = 0;
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND:
            enable = 1;
            cii_setting = (0x1 << RTC_SECCII_OFFSET);
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_MINUTE:
            enable = 1;
            cii_setting = (0x1 << RTC_MINCII_OFFSET);
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_HOUR:
            enable = 1;
            cii_setting = (0x1 << RTC_HOUCII_OFFSET);
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_DAY:
            enable = 1;
            cii_setting = (0x1 << RTC_DOMCII_OFFSET);
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_MONTH:
            enable = 1;
            cii_setting = (0x1 << RTC_MTHCII_OFFSET);
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_YEAR:
            enable = 1;
            cii_setting = (0x1 << RTC_YEACII_OFFSET);
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND_1_2:
            enable = 1;
            cii_setting = (0x1 << RTC_SECCII_1_2_OFFSET);
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND_1_4:
            enable = 1;
            cii_setting = (0x1 << RTC_SECCII_1_4_OFFSET);
            break;
        case HAL_RTC_TIME_NOTIFICATION_EVERY_SECOND_1_8:
            enable = 1;
            cii_setting = (0x1 << RTC_SECCII_1_8_OFFSET);
            break;
        default:
            return HAL_RTC_STATUS_INVALID_PARAM;
    }

    value = rtc_register->RTC_IRQ_EN;
    value &= ~RTC_TC_EN_MASK;
    value |= (enable << RTC_TC_EN_OFFSET);
    rtc_register->RTC_IRQ_EN = value;

    rtc_register->RTC_CII_EN = cii_setting;

    rtc_write_trigger_wait();

    return HAL_RTC_STATUS_OK;
}
#endif

hal_rtc_status_t hal_rtc_set_alarm(const hal_rtc_time_t *time)
{
    uint16_t value;

    if (!rtc_is_time_valid(time)) {
        return HAL_RTC_STATUS_INVALID_PARAM;
    }

    value = rtc_register->RTC_AL_YEA;
    value &= ~RTC_AL_YEAR_MASK;
    value |= (time->rtc_year << RTC_AL_YEAR_OFFSET);
    rtc_register->RTC_AL_YEA = value;

    value = rtc_register->RTC_AL_MTH;
    value &= ~RTC_AL_MONTH_MASK;
    value |= (time->rtc_mon << RTC_AL_MONTH_OFFSET);
    rtc_register->RTC_AL_MTH = value;

    value = rtc_register->RTC_AL_DOM;
    value &= ~RTC_AL_DOM_MASK;
    value |= (time->rtc_day << RTC_AL_DOM_OFFSET);
    rtc_register->RTC_AL_DOM = value;

    value = rtc_register->RTC_AL_DOW;
    value &= ~RTC_AL_DOW_MASK;
    value |= (((time->rtc_week > 6) ? 7 : (time->rtc_week + 1)) << RTC_AL_DOW_OFFSET);
    rtc_register->RTC_AL_DOW = value;

    value = rtc_register->RTC_AL_HOU;
    value &= ~RTC_AL_HOUR_MASK;
    value |= (time->rtc_hour << RTC_AL_HOUR_OFFSET);
    rtc_register->RTC_AL_HOU = value;

    value = rtc_register->RTC_AL_MIN;
    value &= ~RTC_AL_MINUTE_MASK;
    value |= (time->rtc_min << RTC_AL_MINUTE_OFFSET);
    rtc_register->RTC_AL_MIN = value;

    value = rtc_register->RTC_AL_SEC;
    value &= ~RTC_AL_SECOND_MASK;
    value |= (time->rtc_sec << RTC_AL_SECOND_OFFSET);
    rtc_register->RTC_AL_SEC = value;

    rtc_write_trigger_wait();

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_get_alarm(hal_rtc_time_t *time)
{
    time->rtc_year = (rtc_register->RTC_AL_YEA & RTC_AL_YEAR_MASK) >> RTC_AL_YEAR_OFFSET;
    time->rtc_mon = (rtc_register->RTC_AL_MTH & RTC_AL_MONTH_MASK) >> RTC_AL_MONTH_OFFSET;
    time->rtc_week = ((rtc_register->RTC_AL_DOW & RTC_AL_DOW_MASK) >> RTC_AL_DOW_OFFSET) - 1;
    time->rtc_day = (rtc_register->RTC_AL_DOM & RTC_AL_DOM_MASK) >> RTC_AL_DOM_OFFSET;
    time->rtc_hour = (rtc_register->RTC_AL_HOU & RTC_AL_HOUR_MASK) >> RTC_AL_HOUR_OFFSET;
    time->rtc_min = (rtc_register->RTC_AL_MIN & RTC_AL_MINUTE_MASK) >> RTC_AL_MINUTE_OFFSET;
    time->rtc_sec = (rtc_register->RTC_AL_SEC & RTC_AL_SECOND_MASK) >> RTC_AL_SECOND_OFFSET;

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_enable_alarm(void)
{
    uint16_t value;

    /* Enable alarm interrupt */
    value = rtc_register->RTC_IRQ_EN;
    value &= ~RTC_AL_EN_MASK;
    value |= (0x1 << RTC_AL_EN_OFFSET);
    rtc_register->RTC_IRQ_EN = value;

    /* Enable alarm power on */
    value = rtc_register->RTC_BBPU;
    value &= ~RTC_PWREN_MASK;
    value |= RTC_KEY_BBPU | (0x1 << RTC_PWREN_OFFSET);
    rtc_register->RTC_BBPU = value;

    rtc_write_trigger_wait();

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_disable_alarm(void)
{
    uint16_t value;

    value = rtc_register->RTC_IRQ_EN;
    value &= ~RTC_AL_EN_MASK;
    value |= (0x0 << RTC_AL_EN_OFFSET);
    rtc_register->RTC_IRQ_EN = value;

    value = rtc_register->RTC_BBPU;
    value &= ~RTC_PWREN_MASK;
    value |= RTC_KEY_BBPU | (0x0 << RTC_PWREN_OFFSET);
    rtc_register->RTC_BBPU = value;

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
    *frequency = f32k_measure_clock(FQMTR_FCKSEL_RTC_32K, FQMTR_TCKSEL_CSW_GP_26M_CK, 99);
    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t rtc_set_data(uint16_t offset, const char *buf, uint16_t len, bool access_hw)
{
    uint32_t i = 0;
    uint16_t value;

    if ((offset >= (sizeof(rtc_spare_register_table) / sizeof(rtc_spare_register_table[0]))) ||
        (offset + len > (sizeof(rtc_spare_register_table) / sizeof(rtc_spare_register_table[0]))) || (buf == NULL)) {
        log_hal_error("Invalid parameter, offset = %d, len = %d, buf = %p", offset, len, buf);
        return HAL_RTC_STATUS_INVALID_PARAM;
    }

    for (i = 0; i < len; i++) {
        if (access_hw) {
            value = *(rtc_spare_register_table[i + offset].address);
            value &= ~rtc_spare_register_table[i + offset].mask;
            value |= ((*(buf + i) << rtc_spare_register_table[i + offset].offset) & rtc_spare_register_table[i + offset].mask);
            *(rtc_spare_register_table[i + offset].address) = value;

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
        log_hal_warning("%s: in rtc test mode.", __func__);
    }

    rtc_set_data(offset, buf, len, !rtc_in_test);

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t rtc_get_data(uint16_t offset, char *buf, uint16_t len, bool access_hw)
{
    uint32_t i = 0;

    if ((offset >= (sizeof(rtc_spare_register_table) / sizeof(rtc_spare_register_table[0]))) ||
        (offset + len > (sizeof(rtc_spare_register_table) / sizeof(rtc_spare_register_table[0]))) || (buf == NULL)) {
        log_hal_error("Invalid parameter, offset = %d, len = %d, buf = %p", offset, len, buf);
        return HAL_RTC_STATUS_INVALID_PARAM;
    }

    for (i = 0; i < len; i++) {
        if (access_hw) {
            *(buf + i) = (*(rtc_spare_register_table[i + offset].address) & rtc_spare_register_table[i + offset].mask) >>
                         rtc_spare_register_table[i + offset].offset;
        } else {
            *(buf + i) = rtc_spare_register_backup[offset + i];
        }
    }

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t hal_rtc_get_data(uint16_t offset, char *buf, uint16_t len)
{
    if (rtc_in_test) {
        log_hal_warning("%s: in rtc test mode.", __func__);
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
        log_hal_warning("%s: in rtc test mode.", __func__);
    }

    memset(buf, 0, sizeof(buf));

    rtc_set_data(offset, buf, len, !rtc_in_test);

    return HAL_RTC_STATUS_OK;
}

void rtc_set_register(uint16_t address, uint16_t value)
{
    if (address > (uint32_t) & (((RTC_REGISTER_T *)0)->RTC_CON)) {
        log_hal_error("Invalid address");
    }

    *(uint16_t *)((uint8_t *)rtc_register + address) = value;
}

uint16_t rtc_get_register(uint16_t address)
{
    if (address > (uint32_t) & (((RTC_REGISTER_T *)0)->RTC_CON)) {
        log_hal_error("Invalid address");
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
    log_hal_warning("test_rtc_alarm_callback");

    alarm_power_on_time = (hal_rtc_time_t *)parameter;

    log_hal_warning("target alarm time: 20%d,%d,%d (%d) %d:%d:%d", alarm_power_on_time->rtc_year,
                    alarm_power_on_time->rtc_mon, alarm_power_on_time->rtc_day, alarm_power_on_time->rtc_week,
                    alarm_power_on_time->rtc_hour, alarm_power_on_time->rtc_min, alarm_power_on_time->rtc_sec);
    hal_rtc_set_alarm_callback(NULL, NULL);

    hal_rtc_set_alarm(alarm_power_on_time);
    hal_rtc_enable_alarm();
    pmu_set_register_value(PMU_RG_PWRHOLD_ADDR          , PMU_RG_PWRHOLD_MASK            , PMU_RG_PWRHOLD_SHIFT           , 0);
}

hal_rtc_status_t rtc_alarm_power_on_test(hal_rtc_time_t *time)
{
    static hal_rtc_time_t alarm_power_on_time;
    hal_rtc_time_t rtc_get_time;

    memcpy(&alarm_power_on_time, time, sizeof(hal_rtc_time_t));

    log_hal_error("target alarm time: 20%d,%d,%d (%d) %d:%d:%d", time->rtc_year,
                  time->rtc_mon, time->rtc_day, time->rtc_week, time->rtc_hour, time->rtc_min, time->rtc_sec);
    hal_rtc_get_time(&rtc_get_time);
    log_hal_error("get alarm time: 20%d,%d,%d (%d) %d:%d:%d", rtc_get_time.rtc_year,
                  rtc_get_time.rtc_mon, rtc_get_time.rtc_day, rtc_get_time.rtc_week - 1,
                  rtc_get_time.rtc_hour, rtc_get_time.rtc_min, rtc_get_time.rtc_sec);
    rtc_forward_time(&rtc_get_time, 10);
    log_hal_error("set alarm time: 20%d,%d,%d (%d) %d:%d:%d", rtc_get_time.rtc_year,
                  rtc_get_time.rtc_mon, rtc_get_time.rtc_day, rtc_get_time.rtc_week - 1,
                  rtc_get_time.rtc_hour, rtc_get_time.rtc_min, rtc_get_time.rtc_sec);
    hal_rtc_set_alarm(&rtc_get_time);
    hal_rtc_set_alarm_callback(test_rtc_alarm_callback, &alarm_power_on_time);
    hal_rtc_enable_alarm();

    return HAL_RTC_STATUS_OK;
}

hal_rtc_status_t rtc_enter_test(bool enter)
{
    log_hal_info("%s: %d", __func__, enter);

    if (enter) {
        rtc_get_data(0, rtc_spare_register_backup, HAL_RTC_BACKUP_BYTE_NUM_MAX, true);
    } else {
        rtc_set_data(0, rtc_spare_register_backup, HAL_RTC_BACKUP_BYTE_NUM_MAX, true);
    }
    rtc_in_test = enter;

    return HAL_RTC_STATUS_OK;
}

#endif

