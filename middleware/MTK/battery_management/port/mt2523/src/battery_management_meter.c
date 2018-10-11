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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "timers.h"

#include "hal_rtc.h"
#include "hal_gpt.h"
#include "hal_sleep_manager.h"
#include "battery_common.h"
#include "cust_battery_meter_table.h"


#ifdef BMT_DEBUG_INFO_ENABLE
#define BMT_DBG(fmt,args...)    LOG_I(bmt ,"BM met: "fmt,##args)
#define BMT_PRINT(fmt, args...) LOG_I(bmt ,"BM met: "fmt,##args)
#else
#define BMT_DBG(fmt, args...)
#define BMT_PRINT(fmt, args...)
#endif

#define BMT_INFO(fmt, args...)  LOG_I(bmt ,"BM met: "fmt,##args)
#define BMT_WARN(fmt, args...)  LOG_W(bmt ,"BM met: "fmt,##args)
#define BMT_ERR(fmt, args...)   LOG_E(bmt ,"BM met: "fmt,##args)

/*************************************
*           global variable
**************************************/
static int32_t g_I_SENSE_offset = 0;

static int32_t gFG_15_vlot = 3700;
static int32_t gFG_voltage_init = 0;
static int32_t gFG_current_init = 0;
static bool gFG_Is_Charging_init = false;
static int32_t g_sw_vbat_temp = 0;

uint32_t sleep_total_time = NORMAL_WAKEUP_PERIOD;
static uint32_t g_suspend_timer = NORMAL_WAKEUP_PERIOD;

static int32_t gFG_plugout_status = 0;
static int32_t g_tracking_point = CUST_TRACKING_POINT;
static int32_t gFG_voltage = 0;
static int32_t gFG_hwocv = 0;
static int32_t gFG_capacity_by_c = -1;


/* SW FG */
static int32_t oam_v_ocv;
static int32_t oam_r;
static int32_t swfg_ap_suspend_time = 0;
static int32_t ap_suspend_car = 0;

static int32_t is_hwocv_update = false;

static int32_t shutdown_system_voltage = SHUTDOWN_SYSTEM_VOLTAGE;
static int32_t charge_tracking_time = CHARGE_TRACKING_TIME;
static int32_t discharge_tracking_time = DISCHARGE_TRACKING_TIME;

uint32_t wake_up_smooth_time = 0; /* sec */
bool g_suspend_timeout = false;
uint32_t ap_suspend_time;
uint32_t battery_suspend_time;
uint32_t battery_thread_time;

struct battery_meter_custom_data batt_meter_cust_data;
struct battery_meter_table_custom_data batt_meter_table_cust_data;
extern struct battery_custom_data batt_cust_data;

extern PMU_ChargerStruct BMT_status;
extern HAL_BATTERY_VOLTAGE_ENUM g_cv_voltage;
extern bool g_battery_soc_ready;


int battery_meter_init_custom_charger_data(void)
{
    /* cust_charging.h */

    /* Linear Charging Threshold */
#if defined(V_PRE2CC_THRES)
    batt_cust_data.v_pre2cc_thres = V_PRE2CC_THRES;
#endif
#if defined(V_CC2TOPOFF_THRES)
    batt_cust_data.v_cc2topoff_thres = V_CC2TOPOFF_THRES;
#endif
#if defined(RECHARGING_VOLTAGE)
    batt_cust_data.recharging_voltage = RECHARGING_VOLTAGE;
#endif
#if defined(CHARGING_FULL_CURRENT)
    batt_cust_data.charging_full_current = CHARGING_FULL_CURRENT;
#endif

    /* Charging Current Setting */
#if defined(USB_CHARGER_CURRENT)
    batt_cust_data.usb_charger_current = USB_CHARGER_CURRENT;
#endif
#if defined(AC_CHARGER_CURRENT)
    batt_cust_data.ac_charger_current = AC_CHARGER_CURRENT;
#endif
#if defined(NON_STD_AC_CHARGER_CURRENT)
    batt_cust_data.non_std_ac_charger_current = NON_STD_AC_CHARGER_CURRENT;
#endif
#if defined(CHARGING_HOST_CHARGER_CURRENT)
    batt_cust_data.charging_host_charger_current = CHARGING_HOST_CHARGER_CURRENT;
#endif

    /* High battery support */
#if defined(HIGH_BATTERY_VOLTAGE_SUPPORT)
    batt_cust_data.high_battery_voltage_support = 1;
#else
    batt_cust_data.high_battery_voltage_support = 0;
#endif

    return 0;
}


int32_t battery_meter_init_custom_meter_data(void)
{
    BMT_INFO("battery_meter_init_custom_meter_data");

    /* cust_battery_meter_table.h */

    batt_meter_table_cust_data.battery_profile_t0_size = sizeof(battery_profile_t0) / sizeof(BATTERY_PROFILE_STRUC);

    memcpy(&batt_meter_table_cust_data.battery_profile_t0,
           &battery_profile_t0,
           sizeof(battery_profile_t0));

    batt_meter_table_cust_data.battery_profile_t1_size = sizeof(battery_profile_t1) / sizeof(BATTERY_PROFILE_STRUC);

    memcpy(&batt_meter_table_cust_data.battery_profile_t1,
           &battery_profile_t1,
           sizeof(battery_profile_t1));

    batt_meter_table_cust_data.battery_profile_t2_size = sizeof(battery_profile_t2) / sizeof(BATTERY_PROFILE_STRUC);

    memcpy(&batt_meter_table_cust_data.battery_profile_t2,
           &battery_profile_t2,
           sizeof(battery_profile_t2));

    batt_meter_table_cust_data.battery_profile_t3_size = sizeof(battery_profile_t3) / sizeof(BATTERY_PROFILE_STRUC);

    memcpy(&batt_meter_table_cust_data.battery_profile_t3,
           &battery_profile_t3,
           sizeof(battery_profile_t3));

    batt_meter_table_cust_data.r_profile_t0_size = sizeof(r_profile_t0) / sizeof(R_PROFILE_STRUC);

    memcpy(&batt_meter_table_cust_data.r_profile_t0,
           &r_profile_t0,
           sizeof(r_profile_t0));

    batt_meter_table_cust_data.r_profile_t1_size = sizeof(r_profile_t1) / sizeof(R_PROFILE_STRUC);

    memcpy(&batt_meter_table_cust_data.r_profile_t1,
           &r_profile_t1,
           sizeof(r_profile_t1));

    batt_meter_table_cust_data.r_profile_t2_size = sizeof(r_profile_t2) / sizeof(R_PROFILE_STRUC);

    memcpy(&batt_meter_table_cust_data.r_profile_t2,
           &r_profile_t2,
           sizeof(r_profile_t2));
    batt_meter_table_cust_data.r_profile_t3_size = sizeof(r_profile_t3) / sizeof(R_PROFILE_STRUC);

    memcpy(&batt_meter_table_cust_data.r_profile_t3,
           &r_profile_t3,
           sizeof(r_profile_t3));

    /* cust_battery_meter.h */

#if defined(SOC_BY_HW_FG)
    batt_meter_cust_data.soc_flow = HW_FG;
#elif defined(SOC_BY_SW_FG)
    batt_meter_cust_data.soc_flow = SW_FG;
#elif defined(SOC_BY_AUXADC)
    batt_meter_cust_data.soc_flow = AUXADC;
#endif


    batt_meter_cust_data.temperature_t0 = TEMPERATURE_T0;
    batt_meter_cust_data.temperature_t1 = TEMPERATURE_T1;
    batt_meter_cust_data.temperature_t2 = TEMPERATURE_T2;
    batt_meter_cust_data.temperature_t3 = TEMPERATURE_T3;
    batt_meter_cust_data.temperature_t = TEMPERATURE_T;

    batt_meter_cust_data.fg_meter_resistance = FG_METER_RESISTANCE;

    /* Qmax for battery  */
    batt_meter_cust_data.q_max_pos_50 = Q_MAX_POS_50;
    batt_meter_cust_data.q_max_pos_25 = Q_MAX_POS_25;
    batt_meter_cust_data.q_max_pos_0 = Q_MAX_POS_0;
    batt_meter_cust_data.q_max_neg_10 = Q_MAX_NEG_10;
    batt_meter_cust_data.q_max_pos_50_h_current = Q_MAX_POS_50_H_CURRENT;
    batt_meter_cust_data.q_max_pos_25_h_current = Q_MAX_POS_25_H_CURRENT;
    batt_meter_cust_data.q_max_pos_0_h_current = Q_MAX_POS_0_H_CURRENT;
    batt_meter_cust_data.q_max_neg_10_h_current = Q_MAX_NEG_10_H_CURRENT;

#if defined(CHANGE_TRACKING_POINT)
    batt_meter_cust_data.change_tracking_point = 1;
#else
    batt_meter_cust_data.change_tracking_point = 0;
#endif
    batt_meter_cust_data.cust_tracking_point = CUST_TRACKING_POINT;
    g_tracking_point = CUST_TRACKING_POINT;
    batt_meter_cust_data.aging_tuning_value = AGING_TUNING_VALUE;
    batt_meter_cust_data.ocv_board_compesate = OCV_BOARD_COMPESATE;
    batt_meter_cust_data.car_tune_value = CAR_TUNE_VALUE;


    batt_meter_cust_data.difference_hwocv_rtc = DIFFERENCE_HWOCV_RTC;
    batt_meter_cust_data.difference_hwocv_swocv = DIFFERENCE_HWOCV_SWOCV;
    batt_meter_cust_data.difference_swocv_rtc = DIFFERENCE_SWOCV_RTC;
    batt_meter_cust_data.max_swocv = MAX_SWOCV;

    batt_meter_cust_data.shutdown_system_voltage = SHUTDOWN_SYSTEM_VOLTAGE;
    batt_meter_cust_data.recharge_tolerance = RECHARGE_TOLERANCE;

    batt_meter_cust_data.batterypseudo100 = BATTERYPSEUDO100;
    batt_meter_cust_data.batterypseudo1 = BATTERYPSEUDO1;

#if defined(Q_MAX_BY_CURRENT)
    batt_meter_cust_data.q_max_by_current = 1;
#elif defined(Q_MAX_BY_SYS)
    batt_meter_cust_data.q_max_by_current = 2;
#else
    batt_meter_cust_data.q_max_by_current = 0;
#endif
    batt_meter_cust_data.q_max_sys_voltage = Q_MAX_SYS_VOLTAGE;

#if defined(SHUTDOWN_GAUGE0)
    batt_meter_cust_data.shutdown_gauge0 = 1;
#else
    batt_meter_cust_data.shutdown_gauge0 = 0;
#endif
#if defined(SHUTDOWN_GAUGE1_XMINS)
    batt_meter_cust_data.shutdown_gauge1_xmins = 1;
#else
    batt_meter_cust_data.shutdown_gauge1_xmins = 0;
#endif
    batt_meter_cust_data.shutdown_gauge1_mins = SHUTDOWN_GAUGE1_MINS;

    batt_meter_cust_data.min_charging_smooth_time = FG_MIN_CHARGING_SMOOTH_TIME;

    batt_meter_cust_data.apsleep_battery_voltage_compensate = APSLEEP_BATTERY_VOLTAGE_COMPENSATE;

    return 0;
}

int32_t battery_meter_init_custom_data(void)
{
    battery_meter_init_custom_charger_data();
    return battery_meter_init_custom_meter_data();
}

int32_t battery_meter_force_get_tbat(bool update)
{

    int32_t bat_temperature_val = 0;
    static int32_t pre_bat_temperature_val = -100;

    if (update == true || pre_bat_temperature_val == -100) {
        /* Get V_BAT_Temperature */
        hal_charger_meter_get_battery_temperature(&bat_temperature_val);

        BMT_DBG("[battery_meter_force_get_tbat] %d", (int)bat_temperature_val);

        pre_bat_temperature_val = bat_temperature_val;
    } else {
        bat_temperature_val = pre_bat_temperature_val;
    }
    return bat_temperature_val;
}

int32_t battery_meter_get_battery_voltage(bool update)
{
    int32_t val = 5;
    static int32_t pre_val = -1;

    if (update == true || pre_val == -1) {
        val = 5;
        hal_charger_meter_get_battery_voltage_sense(&val);

        pre_val = val;
    } else {
        val = pre_val;
    }
    g_sw_vbat_temp = val;

    return val;
}

#if defined(SOC_BY_SW_FG)
void battery_meter_get_fuel_gauge_init_data(void)
{
    bool charging_enable = false;

    /*stop charging for vbat measurement*/
    hal_charger_enable(charging_enable);

    hal_gpt_delay_ms(50);
    /* 1. Get Raw Data */
    gFG_voltage_init = battery_meter_get_battery_voltage(true);
    gFG_current_init = FG_CURRENT_INIT_VALUE;
    gFG_Is_Charging_init = false;

    charging_enable = true;
    hal_charger_enable(charging_enable);

    BMT_INFO("1.[battery_meter_get_fuel_gauge_init_data](gFG_voltage_init %d, gFG_current_init %d, gFG_Is_Charging_init %d)",
             (int)gFG_voltage_init, (int)gFG_current_init, (int)gFG_Is_Charging_init);
}
#endif

uint32_t  battery_meter_get_dynamic_period(void)
{

    uint32_t vbat_val = 0;

    vbat_val = g_sw_vbat_temp;

    if (wake_up_smooth_time == 0) {
        /* change wake up period when system suspend. */
        if (vbat_val > VBAT_NORMAL_WAKEUP) { /* 3.6v */
            g_suspend_timer = NORMAL_WAKEUP_PERIOD;    /* 45 min */
        } else if (vbat_val > VBAT_LOW_POWER_WAKEUP) { /* 3.5v */
            g_suspend_timer = LOW_POWER_WAKEUP_PERIOD;    /* 5 min */
        } else {
            g_suspend_timer = CLOSE_POWEROFF_WAKEUP_PERIOD;    /* 0.5 min */
        }
    } else {
        g_suspend_timer = wake_up_smooth_time;
    }

    BMT_DBG("vbat_val=%d, g_suspend_timer=%d wake_up_smooth_time=%d", vbat_val, g_suspend_timer, wake_up_smooth_time);

    return g_suspend_timer;
}

int32_t battery_meter_get_charging_current(void)
{
    int32_t ADC_BAT_SENSE_tmp[20] =
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int32_t ADC_BAT_SENSE_sum = 0;
    int32_t ADC_BAT_SENSE = 0;
    int32_t ADC_I_SENSE_tmp[20] =
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int32_t ADC_I_SENSE_sum = 0;
    int32_t ADC_I_SENSE = 0;
    int32_t repeat = 20;
    int32_t i = 0;
    int32_t j = 0;
    int32_t temp = 0;
    int32_t ICharging = 0;
    int32_t val = 1;

    for (i = 0; i < repeat; i++) {
        val = 1;
        hal_charger_meter_get_battery_voltage_sense(&val);
        ADC_BAT_SENSE_tmp[i] = val;

        val = 1;
        hal_charger_meter_get_voltage_current_sense(&val);
        ADC_I_SENSE_tmp[i] = val;

        ADC_BAT_SENSE_sum += ADC_BAT_SENSE_tmp[i];
        ADC_I_SENSE_sum += ADC_I_SENSE_tmp[i];
    }

    /* sorting    BAT_SENSE */
    for (i = 0; i < repeat; i++) {
        for (j = i; j < repeat; j++) {
            if (ADC_BAT_SENSE_tmp[j] < ADC_BAT_SENSE_tmp[i]) {
                temp = ADC_BAT_SENSE_tmp[j];
                ADC_BAT_SENSE_tmp[j] = ADC_BAT_SENSE_tmp[i];
                ADC_BAT_SENSE_tmp[i] = temp;
            }
        }
    }
#ifdef BMT_DEBUG_INFO_ENABLE
    BMT_DBG("[g_Get_I_Charging:BAT_SENSE]");
    for (i = 0; i < repeat; i++) {
        BMT_DBG("%d,", (int)ADC_BAT_SENSE_tmp[i]);
    }
    BMT_DBG("");
#endif
    /* sorting    I_SENSE */
    for (i = 0; i < repeat; i++) {
        for (j = i; j < repeat; j++) {
            if (ADC_I_SENSE_tmp[j] < ADC_I_SENSE_tmp[i]) {
                temp = ADC_I_SENSE_tmp[j];
                ADC_I_SENSE_tmp[j] = ADC_I_SENSE_tmp[i];
                ADC_I_SENSE_tmp[i] = temp;
            }
        }
    }
#ifdef BMT_DEBUG_INFO_ENABLE
    BMT_DBG("[g_Get_I_Charging:I_SENSE]\r\n");
    for (i = 0; i < repeat; i++) {
        BMT_DBG("%d,", (int)ADC_I_SENSE_tmp[i]);
    }
    BMT_DBG("");
#endif
    ADC_BAT_SENSE_sum -= ADC_BAT_SENSE_tmp[0];
    ADC_BAT_SENSE_sum -= ADC_BAT_SENSE_tmp[1];
    ADC_BAT_SENSE_sum -= ADC_BAT_SENSE_tmp[18];
    ADC_BAT_SENSE_sum -= ADC_BAT_SENSE_tmp[19];
    ADC_BAT_SENSE = ADC_BAT_SENSE_sum / (repeat - 4);


    ADC_I_SENSE_sum -= ADC_I_SENSE_tmp[0];
    ADC_I_SENSE_sum -= ADC_I_SENSE_tmp[1];
    ADC_I_SENSE_sum -= ADC_I_SENSE_tmp[18];
    ADC_I_SENSE_sum -= ADC_I_SENSE_tmp[19];
    ADC_I_SENSE = ADC_I_SENSE_sum / (repeat - 4);

    BMT_INFO("[g_Get_I_Charging] ADC_I_SENSE=%d ADC_BAT_SENSE=%d offset = %d ", (int)ADC_I_SENSE, (int)ADC_BAT_SENSE, (int)g_I_SENSE_offset);

    if (ADC_I_SENSE > ADC_BAT_SENSE) {
        ICharging = (ADC_I_SENSE - ADC_BAT_SENSE + g_I_SENSE_offset) * 1000 / CUST_R_SENSE;
    } else {
        ICharging = 0;
    }

    return ICharging;
}

int32_t battery_meter_get_battery_temperature(void)
{
    return battery_meter_force_get_tbat(true);
}


int32_t battery_meter_get_charger_voltage(void)
{
    int32_t val = 0;
    val = 5;
    hal_charger_meter_get_charger_voltage(&val);
    return val;
}

int32_t battery_meter_get_battery_capacity_level(int32_t percentage)
{
    int32_t capacity_level = 0;

    if (percentage >= BATTERY_CAPACITY_LEVEL_5) {
        capacity_level = 5;
    } else if (percentage >= BATTERY_CAPACITY_LEVEL_4) {
        capacity_level = 4;
    } else if (percentage >= BATTERY_CAPACITY_LEVEL_3) {
        capacity_level = 3;
    } else if (percentage >= BATTERY_CAPACITY_LEVEL_2) {
        capacity_level = 2;
    } else if (percentage >= BATTERY_CAPACITY_LEVEL_1) {
        capacity_level = 1;
    } else {
        capacity_level = 0;
    }

    return capacity_level;

}

uint32_t battery_meter_fuel_gauge_command_callback(FG_CTRL_CMD cmd, void *parameter, void *data)
{
    if (cmd < FG_CMD_NUMBER) {
        switch (cmd) {
            case FG_CMD_GET_RTC_SPARE_FG_VALUE: {

                hal_rtc_status_t ret;
                char rtc_value = 0;
                ret = hal_rtc_get_data(0, &rtc_value, 1);
                if (HAL_RTC_STATUS_OK == ret) {
                    *((uint32_t *)data) = rtc_value;
                }
            }
            break;
            case FG_CMD_IS_CHARGER_EXIST:
                *((bool *)data) = battery_core_get_charger_status();
                break;
            case FG_CMD_GET_SHUTDOWN_SYSTEM_VOLTAGE:
                *((int32_t *)data) = shutdown_system_voltage;
                break;
            case FG_CMD_GET_BATTERY_INIT_VOLTAGE:
                *((int32_t *)data) = gFG_voltage_init;
                break;
            case FG_CMD_GET_HW_FG_INIT_CURRENT:
                *((int32_t *)data) = gFG_current_init;
                break;
            case FG_CMD_GET_TEMPERTURE: {
                int32_t temperture = 0;
                temperture = battery_meter_force_get_tbat(*((bool *)parameter));
                *((int32_t *)data) = temperture;
            }
            break;
            case FG_CMD_GET_CUSTOM_TABLE: {

                ((struct battery_meter_table_custom_data_p *)data)->battery_profile_t0_size = batt_meter_table_cust_data.battery_profile_t0_size;
                ((struct battery_meter_table_custom_data_p *)data)->battery_profile_t1_size = batt_meter_table_cust_data.battery_profile_t1_size;
                ((struct battery_meter_table_custom_data_p *)data)->battery_profile_t2_size = batt_meter_table_cust_data.battery_profile_t2_size;
                ((struct battery_meter_table_custom_data_p *)data)->battery_profile_t3_size = batt_meter_table_cust_data.battery_profile_t3_size;
                ((struct battery_meter_table_custom_data_p *)data)->battery_profile_temperature_size = batt_meter_table_cust_data.battery_profile_temperature_size;

                ((struct battery_meter_table_custom_data_p *)data)->r_profile_t0_size = batt_meter_table_cust_data.r_profile_t0_size;
                ((struct battery_meter_table_custom_data_p *)data)->r_profile_t1_size = batt_meter_table_cust_data.r_profile_t1_size;
                ((struct battery_meter_table_custom_data_p *)data)->r_profile_t2_size = batt_meter_table_cust_data.r_profile_t2_size;
                ((struct battery_meter_table_custom_data_p *)data)->r_profile_t3_size = batt_meter_table_cust_data.r_profile_t3_size;
                ((struct battery_meter_table_custom_data_p *)data)->r_profile_temperature_size = batt_meter_table_cust_data.r_profile_temperature_size;

                ((struct battery_meter_table_custom_data_p *)data)->cust_battery_profile_t0 = &batt_meter_table_cust_data.battery_profile_t0[0];
                ((struct battery_meter_table_custom_data_p *)data)->cust_battery_profile_t1 = &batt_meter_table_cust_data.battery_profile_t1[0];
                ((struct battery_meter_table_custom_data_p *)data)->cust_battery_profile_t2 = &batt_meter_table_cust_data.battery_profile_t2[0];
                ((struct battery_meter_table_custom_data_p *)data)->cust_battery_profile_t3 = &batt_meter_table_cust_data.battery_profile_t3[0];
                ((struct battery_meter_table_custom_data_p *)data)->cust_battery_profile_temperature = &batt_meter_table_cust_data.battery_profile_temperature[0];

                ((struct battery_meter_table_custom_data_p *)data)->cust_r_profile_t0 = &batt_meter_table_cust_data.r_profile_t0[0];
                ((struct battery_meter_table_custom_data_p *)data)->cust_r_profile_t1 = &batt_meter_table_cust_data.r_profile_t1[0];
                ((struct battery_meter_table_custom_data_p *)data)->cust_r_profile_t2 = &batt_meter_table_cust_data.r_profile_t2[0];
                ((struct battery_meter_table_custom_data_p *)data)->cust_r_profile_t3 = &batt_meter_table_cust_data.r_profile_t3[0];
                ((struct battery_meter_table_custom_data_p *)data)->cust_r_profile_temperature = &batt_meter_table_cust_data.r_profile_temperature[0];
#ifdef BMT_DEBUG_INFO_ENABLE
                BMT_DBG("cust_r_profile_t0  size = %d\n", (int)((struct battery_meter_table_custom_data_p *)data)->r_profile_t0_size);
                BMT_DBG("cust_r_profile_t1  size = %d\n", (int)((struct battery_meter_table_custom_data_p *)data)->r_profile_t0_size);
                BMT_DBG("cust_r_profile_t2  size = %d\n", (int)((struct battery_meter_table_custom_data_p *)data)->r_profile_t0_size);
                BMT_DBG("cust_r_profile_t3  size = %d\n", (int)((struct battery_meter_table_custom_data_p *)data)->r_profile_t0_size);
                BMT_DBG("cust_r_profile_temperature  size = %d\n", (int)((struct battery_meter_table_custom_data_p *)data)->r_profile_temperature_size);

                int32_t i;
                for (i = 0 ; i < ((struct battery_meter_table_custom_data_p *)data)->r_profile_t0_size; i++) {
                    BMT_DBG("<DOD,Voltage> at %d = <%d,%d>\n", i, (int)(((struct battery_meter_table_custom_data_p *)data)->cust_r_profile_t0 + i)->resistance , (int)(((struct battery_meter_table_custom_data_p *)data)->cust_r_profile_t0 + i)->voltage);
                }
#endif

            }
            break;
            case FG_CMD_GET_FG_CUSTOM_DATA:
                memcpy(data, &batt_meter_cust_data, sizeof(batt_meter_cust_data));
                break;
            case FG_CMD_GET_HW_OCV: {
                int32_t voltage = 0;
                hal_charger_meter_get_hardware_open_circuit_voltage(&voltage);
                *((int32_t *)data) = voltage;
                gFG_hwocv = voltage;
                (void)gFG_hwocv;
            }
            break;
            case FG_CMD_GET_BATTERY_PLUG_STATUS: {
                hal_charger_meter_get_battery_plug_out_status((int32_t *)data);
                gFG_plugout_status = *((int32_t *)data);
                (void)gFG_plugout_status;
            }
            break;
            case FG_CMD_IS_BATTERY_FULL: {
                *((bool *)data) = BMT_status.bat_full;
            }
            break;
            case FG_CMD_GET_CV_VALUE: {
                *((uint32_t *)data) = g_cv_voltage;
            }
            break;
            case FG_CMD_GET_SUSPEND_CAR: {
                int32_t  car = ap_suspend_car / 3600;
                *((int32_t *)data) = car;
                ap_suspend_car = ap_suspend_car % 3600;
            }
            break;
            case FG_CMD_GET_SUSPEND_TIME: {
                *((int32_t *)data) = swfg_ap_suspend_time;
                swfg_ap_suspend_time = 0;
            }
            break;
            case FG_CMD_GET_DURATION_TIME: {
                int32_t duration_time = 0;
                BATTERY_TIME_ENUM duration_type;
                duration_type = *((BATTERY_TIME_ENUM *)parameter);
                duration_time = battery_core_get_duration_time(duration_type);
                *((int32_t *)data) = duration_time;
            }
            break;
            case FG_CMD_GET_BATTERY_VOLTAGE: {
                bool update;
                int32_t voltage = 0;
                update = *((bool *)parameter);
                if (update == true) {
                    voltage = battery_meter_get_battery_voltage(true);
                } else {
                    voltage = BMT_status.bat_vol;
                }
                *((int32_t *)data) = voltage;
            }
            break;
            case FG_CMD_IS_HW_OCV_UPDATE: {
                *((int32_t *)data) = is_hwocv_update;
                is_hwocv_update = false;
            }
            break;
            case FG_CMD_GET_CHARGE_TRACKING_TIME: {
                *((int32_t *)data) = charge_tracking_time;
            }
            break;
            case FG_CMD_GET_DISCHARGE_TRACKING_TIME: {
                *((int32_t *)data) = discharge_tracking_time;
            }
            break;
            case FG_CMD_SET_SWOCV:
                gFG_voltage = *((int32_t *)data);
                break;
            case FG_CMD_SET_SOC: {
                gFG_capacity_by_c = *((int32_t *)data);
                BMT_status.SOC = gFG_capacity_by_c;
            }
            break;
            case FG_CMD_SET_WAKEUP_SMOOTH_TIME: {
                wake_up_smooth_time = *((uint32_t *)data);
            }
            break;
            case FG_CMD_SET_BATTERY_FULL: {
                BMT_status.bat_full = *((bool *)data);
            }
            break;
            case FG_CMD_SET_UI_SOC: {
                BMT_status.UI_SOC = *((int32_t *)data);
            }
            break;
            case FG_CMD_SET_UI_SOC2: {
                BMT_status.UI_SOC2 = *((int32_t *)data);
                BMT_status.UI_SOC2_LEVEL = battery_meter_get_battery_capacity_level(BMT_status.UI_SOC2);
                if (!g_battery_soc_ready) {
                    g_battery_soc_ready = true;
                }
            }
            break;
            case FG_CMD_SET_RTC: {
                hal_rtc_status_t ret;
                char rtc_value = (char) * ((int32_t *)data);
                ret = hal_rtc_set_data(0, &rtc_value, 1);
                if (HAL_RTC_STATUS_OK != ret) {
                    BMT_WARN("Set RTC data error");
                }
            }
            break;
            case FG_CMD_SET_OAM_V_OCV: {
                oam_v_ocv = *((int32_t *)data);
                (void)oam_v_ocv;
            }
            break;
            case FG_CMD_SET_OAM_R: {
                oam_r = *((int32_t *)data);
                (void)oam_r;
            }
            break;
            case FG_CMD_SET_POWEROFF: {
                BMT_WARN("FG_CMD_SET_POWEROFF power request!!");
                BMT_WARN("Gauge 0% low battery, system need power off by application");

            }
            break;

            default:

                break;
        }
        return 1;
    }
    return 0;
}

extern log_control_block_t log_control_block_bmt;
void battery_meter_fuel_gauge_log(const char *format, va_list args)
{
#ifndef MTK_DEBUG_LEVEL_NONE
    vprint_module_log(&log_control_block_bmt, __FUNCTION__, __LINE__, PRINT_LEVEL_INFO, format, args);
#endif
}

int32_t battery_meter_initial(void)
{

    static bool meter_initilized = false;

    if (meter_initilized == false) {
#if defined(SOC_BY_SW_FG)
        battery_meter_get_fuel_gauge_init_data();
#endif
        meter_initilized = true;
    }

    fgauge_register_debug_message_callback(battery_meter_fuel_gauge_log);
    fgauge_register_callback(battery_meter_fuel_gauge_command_callback);
    fgauge_initialization();

    return 0;
}

int32_t battery_meter_sync_current_sense_offset(int32_t bat_i_sense_offset)
{
    g_I_SENSE_offset = bat_i_sense_offset;
    return 0;
}

int32_t battery_meter_get_battery_zcv(void)
{
    return gFG_voltage;
}

int32_t battery_meter_get_battery_nPercent_zcv(void)
{
    return gFG_15_vlot;    /* 15% zcv,  15% can be customized by 100-g_tracking_point */
}

int32_t battery_meter_get_battery_nPercent_UI_SOC(void)
{
    return g_tracking_point;    /* tracking point */
}

int32_t battery_meter_get_voltage_sense(void)
{
    int32_t val = 0;

    val = 1;
    hal_charger_meter_get_voltage_current_sense(&val);
    return val;
}

void battery_meter_init(void)
{
    hal_charger_meter_init();
    battery_meter_init_custom_data();
}

int32_t battery_meter_suspend(void)
{

    ap_suspend_time = battery_core_get_current_time_in_ms();
    battery_suspend_time = battery_core_get_current_time_in_ms();

    if ((sleep_total_time < g_suspend_timer) && sleep_total_time != 0) {
        if (wake_up_smooth_time == 0) {
            BMT_PRINT("[wake_up_smooth_time==0] return 0");
            return 0;
        } else if (sleep_total_time < wake_up_smooth_time) {
            BMT_PRINT("[(sleep_total_time < wake_up_smooth_time] return 0\n\r");
            return 0;
        }
    }

    sleep_total_time = 0;

    BMT_PRINT("[battery_meter_suspend]");
    return 0;
}

int32_t battery_meter_resume(void)
{
    int32_t duration_time = 0;

    battery_core_update_time(&battery_suspend_time, SUSPEND_TIME);
    duration_time = battery_core_get_duration_time(SUSPEND_TIME);

    sleep_total_time += duration_time;

    BMT_PRINT("[battery_meter_resume] sleep time = %d, duration_time = %d, wake_up_smooth_time %d, g_suspend_timer = %d", (int) sleep_total_time, (int) duration_time, (int) wake_up_smooth_time, (int) g_suspend_timer);


    if (battery_core_get_charger_status() == false) {
        uint32_t time;
#ifdef BMT_DEBUG_INFO_ENABLE
        int32_t voltage = 0;
#endif
        /*Reserved for gauge tuning*/
        /*int32_t oam_i,*/
        int32_t oam_car_tmp;
        battery_core_update_time(&ap_suspend_time, AP_SUSPEND_TIME);
        time = battery_core_get_duration_time(AP_SUSPEND_TIME);
#ifdef BMT_DEBUG_INFO_ENABLE
        hal_charger_meter_get_hardware_open_circuit_voltage(&voltage);
#endif
        /*Reserved for gauge tuning*/
        /*oam_i=(((voltage - oam_v_ocv ) * 1000) * 10) / oam_r;*/
        /*oam_car_tmp=(oam_i * time);*/     /* 0.1mAh */

        oam_car_tmp = -time * AP_SLEEP_CAR;/*0.1mA AP_SLEEP_CAR;*/

        is_hwocv_update = true;
#ifdef BMT_DEBUG_INFO_ENABLE
        BMT_PRINT("[battery_meter_resume](2)time:%d bat:%d ocar:%d card:%d\n", (int)time, (int)voltage, (int)ap_suspend_car / 3600, (int)oam_car_tmp / 3600);
#else
        BMT_PRINT("[battery_meter_resume](2)time:%d ocar:%d card:%d\n", (int)time, (int)ap_suspend_car / 3600, (int)oam_car_tmp / 3600);
#endif
        swfg_ap_suspend_time = swfg_ap_suspend_time + time;
        ap_suspend_car = ap_suspend_car + oam_car_tmp;

        if (abs(ap_suspend_car / 3600) >= 100) {
            g_suspend_timeout = true;
            BMT_PRINT("[battery_meter_resume](ap_suspend_car / 3600) >= 100");
            return 0;
        }

        if (sleep_total_time >= wake_up_smooth_time && wake_up_smooth_time != 0) {
            wake_up_smooth_time = 0;
            g_suspend_timeout = true;
            BMT_PRINT("[battery_meter_resume] sleep_total_time >= wake_up_smooth_time");
            return 0;
        }

        if (swfg_ap_suspend_time >= SUSPEND_WAKEUP_TIME) {
            g_suspend_timeout = true;
            BMT_PRINT("[battery_meter_resume] swfg_ap_suspend_time >= %d", SUSPEND_WAKEUP_TIME);
            return 0;
        }

        BMT_PRINT("[battery_meter_resume] resume_start_timer only");
        return 0;
    }


    BMT_PRINT("******** battery_meter_resume!! ******** suspend_time %d smooth_time %d g_suspend_timer %d", (int) sleep_total_time, (int) wake_up_smooth_time, (int) g_suspend_timer);
    g_suspend_timeout = true;
    BMT_PRINT("[battery_meter_resume]");
    return 0;
}

