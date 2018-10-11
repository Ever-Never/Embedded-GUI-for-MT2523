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

#include "FreeRTOS.h"
#include "timers.h"
#include "battery_common.h"

#ifdef BMT_DEBUG_INFO_ENABLE
#define BMT_DBG(fmt,args...)    LOG_I(bmt ,"BM drv: "fmt,##args)
#else
#define BMT_DBG(fmt, args...)
#endif
#define BMT_INFO(fmt, args...)  LOG_I(bmt ,"BM drv: "fmt,##args)
#define BMT_WARN(fmt, args...)  LOG_W(bmt ,"BM drv: "fmt,##args)
#define BMT_ERR(fmt, args...)   LOG_E(bmt ,"BM drv: "fmt,##args)


/*************************************
*           global variable
**************************************/
struct battery_custom_data batt_cust_data;

static uint32_t charging_full_current; /* = CHARGING_FULL_CURRENT;*/    /* mA */
static uint32_t v_cc2topoff_threshold; /* = V_CC2TOPOFF_THRES; */
static HAL_CHARGE_CURRENT_ENUM g_temp_CC_value = HAL_CHARGE_CURRENT_0_MA;
HAL_BATTERY_VOLTAGE_ENUM g_cv_voltage = HAL_BATTERY_VOLT_04_2000_V;

PMU_ChargerStruct BMT_status;
static bool usb_unlimited = false;

#if defined(CONFIG_MTK_JEITA_STANDARD_SUPPORT)
static int32_t g_jeita_recharging_voltage = JEITA_RECHARGE_VOLTAGE;
static int32_t g_temp_status = TEMP_POS_10_TO_POS_45;
static bool temp_error_recovery_chr_flag = true;
#endif

#if defined(CONFIG_MTK_JEITA_STANDARD_SUPPORT)
static HAL_BATTERY_VOLTAGE_ENUM battery_driver_select_jeita_cv(void)
{
    HAL_BATTERY_VOLTAGE_ENUM cv_voltage;
    BMT_DBG("battery_driver_select_jeita_cv");

    if (g_temp_status == TEMP_ABOVE_POS_60) {
        cv_voltage = JEITA_TEMP_ABOVE_POS_60_CV_VOLTAGE;
    } else if (g_temp_status == TEMP_POS_45_TO_POS_60) {
        cv_voltage = JEITA_TEMP_POS_45_TO_POS_60_CV_VOLTAGE;
    } else if (g_temp_status == TEMP_POS_10_TO_POS_45) {
        if (batt_cust_data.high_battery_voltage_support) {
            cv_voltage = HAL_BATTERY_VOLT_04_3500_V;
        } else {
            cv_voltage = JEITA_TEMP_POS_10_TO_POS_45_CV_VOLTAGE;
        }
    } else if (g_temp_status == TEMP_POS_0_TO_POS_10) {
        cv_voltage = JEITA_TEMP_POS_0_TO_POS_10_CV_VOLTAGE;
    } else if (g_temp_status == TEMP_NEG_10_TO_POS_0) {
        cv_voltage = JEITA_TEMP_NEG_10_TO_POS_0_CV_VOLTAGE;
    } else if (g_temp_status == TEMP_BELOW_NEG_10) {
        cv_voltage = JEITA_TEMP_BELOW_NEG_10_CV_VOLTAGE;
    } else {
        cv_voltage = HAL_BATTERY_VOLT_04_2000_V;
    }

    return cv_voltage;
}


PMU_STATUS battery_driver_do_jeita_state_machine(void)
{
    int32_t previous_g_temp_status;
    HAL_BATTERY_VOLTAGE_ENUM cv_voltage;

    previous_g_temp_status = g_temp_status;
    /* JEITA battery temp Standard */
    if (BMT_status.temperature >= TEMP_POS_60_THRESHOLD) {
        BMT_INFO("[BATTERY] Battery Over high Temperature(%d) !!", TEMP_POS_60_THRESHOLD);
        g_temp_status = TEMP_ABOVE_POS_60;
        return PMU_STATUS_FAIL;
    } else if (BMT_status.temperature > TEMP_POS_45_THRESHOLD) {
        if ((g_temp_status == TEMP_ABOVE_POS_60)
                && (BMT_status.temperature >= TEMP_POS_60_THRES_MINUS_X_DEGREE)) {
            BMT_INFO("[BATTERY] Battery Temperature between %d and %d,not allow charging yet!!",
                     TEMP_POS_60_THRES_MINUS_X_DEGREE,
                     TEMP_POS_60_THRESHOLD);
            return PMU_STATUS_FAIL;
        } else {
            BMT_INFO("[BATTERY] Battery Temperature between %d and %d !!",
                     TEMP_POS_45_THRESHOLD, TEMP_POS_60_THRESHOLD);
            g_temp_status = TEMP_POS_45_TO_POS_60;
            g_jeita_recharging_voltage = JEITA_TEMP_POS_45_TO_POS_60_RECHARGE_VOLTAGE;
            v_cc2topoff_threshold = JEITA_TEMP_POS_45_TO_POS_60_CC2TOPOFF_THRESHOLD;
            charging_full_current = batt_cust_data.charging_full_current;
        }
    } else if (BMT_status.temperature >= TEMP_POS_10_THRESHOLD) {
        if (((g_temp_status == TEMP_POS_45_TO_POS_60)
                && (BMT_status.temperature >= TEMP_POS_45_THRES_MINUS_X_DEGREE))
                || ((g_temp_status == TEMP_POS_0_TO_POS_10)
                    && (BMT_status.temperature <= TEMP_POS_10_THRES_PLUS_X_DEGREE))) {
            BMT_INFO("[BATTERY] Battery Temperature not recovery to normal temperature charging mode yet!!");
        } else {
            BMT_INFO("[BATTERY] Battery Normal Temperature between %d and %d !!",
                     TEMP_POS_10_THRESHOLD, TEMP_POS_45_THRESHOLD);

            g_temp_status = TEMP_POS_10_TO_POS_45;
            if (batt_cust_data.high_battery_voltage_support) {
                g_jeita_recharging_voltage = 4200;
            } else {
                g_jeita_recharging_voltage = JEITA_TEMP_POS_10_TO_POS_45_RECHARGE_VOLTAGE;
            }

            v_cc2topoff_threshold = JEITA_TEMP_POS_10_TO_POS_45_CC2TOPOFF_THRESHOLD;
            charging_full_current = batt_cust_data.charging_full_current;
        }
    } else if (BMT_status.temperature >= TEMP_POS_0_THRESHOLD) {
        if ((g_temp_status == TEMP_NEG_10_TO_POS_0 || g_temp_status == TEMP_BELOW_NEG_10)
                && (BMT_status.temperature <= TEMP_POS_0_THRES_PLUS_X_DEGREE)) {
            if (g_temp_status == TEMP_NEG_10_TO_POS_0) {
                BMT_INFO("[BATTERY] Battery Temperature between %d and %d !!",
                         TEMP_POS_0_THRES_PLUS_X_DEGREE,
                         TEMP_POS_10_THRESHOLD);
            }
            if (g_temp_status == TEMP_BELOW_NEG_10) {
                BMT_INFO("[BATTERY] Battery Temperature between %d and %d,not allow charging yet!!",
                         TEMP_POS_0_THRESHOLD,
                         TEMP_POS_0_THRES_PLUS_X_DEGREE);
                return PMU_STATUS_FAIL;
            }
        } else {
            BMT_INFO("[BATTERY] Battery Temperature between %d and %d !!",
                     TEMP_POS_0_THRESHOLD, TEMP_POS_10_THRESHOLD);
            g_temp_status = TEMP_POS_0_TO_POS_10;
            g_jeita_recharging_voltage = JEITA_TEMP_POS_0_TO_POS_10_RECHARGE_VOLTAGE;
            v_cc2topoff_threshold = JEITA_TEMP_POS_0_TO_POS_10_CC2TOPOFF_THRESHOLD;
            charging_full_current = batt_cust_data.charging_full_current;
        }
    } else if (BMT_status.temperature >= TEMP_NEG_10_THRESHOLD) {
        if ((g_temp_status == TEMP_BELOW_NEG_10)
                && (BMT_status.temperature <= TEMP_NEG_10_THRES_PLUS_X_DEGREE)) {
            BMT_INFO("[BATTERY] Battery Temperature between %d and %d,not allow charging yet!!",
                     TEMP_NEG_10_THRESHOLD, TEMP_NEG_10_THRES_PLUS_X_DEGREE);
            return PMU_STATUS_FAIL;
        } else {
            BMT_INFO("[BATTERY] Battery Temperature between %d and %d !!",
                     TEMP_NEG_10_THRESHOLD, TEMP_POS_0_THRESHOLD);
            g_temp_status = TEMP_NEG_10_TO_POS_0;
            g_jeita_recharging_voltage = JEITA_TEMP_NEG_10_TO_POS_0_RECHARGE_VOLTAGE;
            v_cc2topoff_threshold = JEITA_TEMP_NEG_10_TO_POS_0_CC2TOPOFF_THRESHOLD;
            charging_full_current = JEITA_NEG_10_TO_POS_0_FULL_CURRENT;
        }
    } else {
        BMT_INFO("[BATTERY] Battery below low Temperature(%d) !!",
                 TEMP_NEG_10_THRESHOLD);
        g_temp_status = TEMP_BELOW_NEG_10;
        return PMU_STATUS_FAIL;
    }

    /* set CV after temperature changed */
    if (g_temp_status != previous_g_temp_status) {
        cv_voltage = battery_driver_select_jeita_cv();
        hal_charger_set_constant_voltage(cv_voltage);
        g_cv_voltage = cv_voltage;
    }

    return PMU_STATUS_OK;
}


static void battery_driver_set_jeita_charging_current(void)
{

    if (g_temp_status == TEMP_NEG_10_TO_POS_0) {
        g_temp_CC_value = HAL_CHARGE_CURRENT_200_MA;    /* for low temp */
        BMT_INFO("[BATTERY] JEITA set charging current : %d",
                 g_temp_CC_value);
    }
}
#endif

static void battery_driver_init_charging_varaibles(void)
{
    static int32_t init_flag = 0;
    if (init_flag == 0) {
        init_flag = 1;
        charging_full_current = batt_cust_data.charging_full_current;
        v_cc2topoff_threshold = batt_cust_data.v_cc2topoff_thres;

    }
}

bool battery_driver_get_usb_current_unlimited(void)
{
    if (BMT_status.charger_type == HAL_CHARGER_TYPE_STANDARD_HOST || BMT_status.charger_type == HAL_CHARGER_TYPE_CHARGING_HOST) {
        return usb_unlimited;
    } else {
        return false;
    }
}

void battery_driver_set_usb_current_unlimited(bool enable)
{
    usb_unlimited = enable;
}

void battery_driver_select_charging_current(void)
{
    if (BMT_status.charger_type == HAL_CHARGER_TYPE_STANDARD_HOST) {
        g_temp_CC_value = (HAL_CHARGE_CURRENT_ENUM)batt_cust_data.usb_charger_current;
    } else if (BMT_status.charger_type == HAL_CHARGER_TYPE_NONSTANDARD_CHARGER) {
        g_temp_CC_value = (HAL_CHARGE_CURRENT_ENUM)batt_cust_data.non_std_ac_charger_current;
    } else if (BMT_status.charger_type == HAL_CHARGER_TYPE_STANDARD_CHARGER) {
        g_temp_CC_value = (HAL_CHARGE_CURRENT_ENUM)batt_cust_data.ac_charger_current;
    } else if (BMT_status.charger_type == HAL_CHARGER_TYPE_CHARGING_HOST) {
        g_temp_CC_value = (HAL_CHARGE_CURRENT_ENUM)batt_cust_data.charging_host_charger_current;
    } else {
        g_temp_CC_value = HAL_CHARGE_CURRENT_0_MA;
    }

    BMT_INFO("[BATTERY] Default CC mode charging : %d", g_temp_CC_value);

#if defined(CONFIG_MTK_JEITA_STANDARD_SUPPORT)
    battery_driver_set_jeita_charging_current();
#endif
}


static uint32_t battery_driver_charging_full_check(void)
{
    uint32_t status = false;

    static uint8_t full_check_count = 0;

    if (BMT_status.ICharging <= charging_full_current) {
        full_check_count++;
        if (6 == full_check_count) {
            status = true;
            full_check_count = 0;
            BMT_INFO("[BATTERY] Battery full and disable charging on %d mA", (int)BMT_status.ICharging);
        }
    } else {
        full_check_count = 0;
    }

    return status;
}

static void battery_driver_charging_current_calibration(void)
{
    int32_t bat_isense_offset;

    int32_t bat_vol = battery_meter_get_battery_voltage(true);
    int32_t Vsense = battery_meter_get_voltage_sense();

    bat_isense_offset = bat_vol - Vsense;

    BMT_INFO("[BATTERY] bat_vol=%d, Vsense=%d, offset=%d", bat_vol, Vsense, bat_isense_offset);

    battery_meter_sync_current_sense_offset(bat_isense_offset);
}


static void battery_driver_turn_on_charging(void)
{
#if !defined(CONFIG_MTK_JEITA_STANDARD_SUPPORT)
    HAL_BATTERY_VOLTAGE_ENUM cv_voltage;
#endif
    bool charging_enable = true;

    BMT_DBG("[BATTERY] battery_driver_turn_on_charging()!");

    if (BMT_status.bat_charging_state == CHR_ERROR) {
        BMT_WARN("[BATTERY] Charger Error, turn OFF charging !");
        charging_enable = false;
    } else {
        /*HW initialization */
        BMT_DBG("charging_hw_init");
        hal_charger_init();
        /* Set Charging Current */
        if (battery_driver_get_usb_current_unlimited()) {
            g_temp_CC_value = (HAL_CHARGE_CURRENT_ENUM)batt_cust_data.ac_charger_current;
            BMT_DBG("USB_CURRENT_UNLIMITED, use AC_CHARGER_CURRENT");
        } else {
            BMT_DBG("[BATTERY] select_charging_current !");
            battery_driver_select_charging_current();
        }

        if (g_temp_CC_value == HAL_CHARGE_CURRENT_0_MA) {
            charging_enable = false;
            BMT_WARN("[BATTERY] charging current is set 0mA, turn off charging !");
        } else {

            hal_charger_set_charging_current(g_temp_CC_value);

            /* Set CV */
#if !defined(CONFIG_MTK_JEITA_STANDARD_SUPPORT)
            if (batt_cust_data.high_battery_voltage_support) {
                cv_voltage = HAL_BATTERY_VOLT_04_3500_V;
            } else {
                cv_voltage = HAL_BATTERY_VOLT_04_2000_V;
            }

            hal_charger_set_constant_voltage(cv_voltage);
            g_cv_voltage = cv_voltage;
#endif
        }
    }

    /* enable/disable charging */
    BMT_INFO("[BATTERY] battery_driver_turn_on_charging(), enable =%d", (int)charging_enable);
    hal_charger_enable(charging_enable);

}


PMU_STATUS battery_driver_precharger_mode_action(void)
{

    BMT_INFO("[BATTERY] Pre-CC mode charge, timer=%d on %d !!", (int)BMT_status.PRE_charging_time, (int)BMT_status.total_charging_time);
    BMT_status.PRE_charging_time += BAT_TASK_PERIOD;
    BMT_status.CC_charging_time = 0;
    BMT_status.TOPOFF_charging_time = 0;
    BMT_status.total_charging_time += BAT_TASK_PERIOD;

    battery_driver_select_charging_current();

    if (BMT_status.UI_SOC == 100) {
        BMT_status.bat_charging_state = CHR_BATFULL;
        BMT_status.bat_full = true;
    } else if (BMT_status.bat_vol > batt_cust_data.v_pre2cc_thres) {
        BMT_status.bat_charging_state = CHR_CC;
    }

    battery_driver_turn_on_charging();

    return PMU_STATUS_OK;
}

PMU_STATUS battery_driver_constant_current_mode_action(void)
{
    bool charging_enable = false;

    BMT_INFO("[BATTERY] CC mode charge, timer=%d on %d !!", (int)BMT_status.CC_charging_time, (int)BMT_status.total_charging_time);

    BMT_status.PRE_charging_time = 0;
    BMT_status.CC_charging_time += BAT_TASK_PERIOD;
    BMT_status.TOPOFF_charging_time = 0;
    BMT_status.total_charging_time += BAT_TASK_PERIOD;


    if (BMT_status.bat_vol > v_cc2topoff_threshold) {
        BMT_status.bat_charging_state = CHR_TOP_OFF;
    }

    /* Charging 9s and discharging 1s : start */
    hal_charger_enable(charging_enable);
    vTaskDelay(1000 / portTICK_RATE_MS);

    battery_driver_charging_current_calibration();

    battery_driver_turn_on_charging();

    return PMU_STATUS_OK;
}


PMU_STATUS battery_driver_top_off_mode_action(void)
{
    bool charging_enable = false;

    BMT_INFO("[BATTERY] Top Off mode charge, timer=%d on %d !!", (int)BMT_status.TOPOFF_charging_time, (int)BMT_status.total_charging_time);

    BMT_status.PRE_charging_time = 0;
    BMT_status.CC_charging_time = 0;
    BMT_status.TOPOFF_charging_time += BAT_TASK_PERIOD;
    BMT_status.total_charging_time += BAT_TASK_PERIOD;

    battery_driver_turn_on_charging();

    if ((BMT_status.TOPOFF_charging_time >= MAX_CV_CHARGING_TIME)
            || (battery_driver_charging_full_check() == true)
#ifndef  MAX_TOP_OFF_CHARGING_TIME_DISABLE
            || (BMT_status.TOPOFF_charging_time >= MAX_TOP_OFF_CHARGING_TIME)
#endif
       ) {
        BMT_status.bat_charging_state = CHR_BATFULL;
        BMT_status.bat_full = true;

        /*  Disable charging */
        hal_charger_enable(charging_enable);

#ifndef  MAX_TOP_OFF_CHARGING_TIME_DISABLE
        if (BMT_status.TOPOFF_charging_time >= MAX_TOP_OFF_CHARGING_TIME) {
            BMT_INFO("[BATTERY] Top Off mode charge timeout , timer=%d MAX_TOP_OFF_CHARGING_TIME = %d !!", (int)BMT_status.TOPOFF_charging_time, (int)MAX_TOP_OFF_CHARGING_TIME);
        }
#endif

    }
    return PMU_STATUS_OK;
}

PMU_STATUS battery_driver_battery_full_action(void)
{
    bool charging_enable = false;

    BMT_INFO("[BATTERY] Battery full !!");

    BMT_status.bat_full = true;
    BMT_status.total_charging_time = 0;
    BMT_status.PRE_charging_time = 0;
    BMT_status.CC_charging_time = 0;
    BMT_status.TOPOFF_charging_time = 0;
    BMT_status.POSTFULL_charging_time = 0;
    BMT_status.bat_in_recharging_state = false;


#if defined(CONFIG_MTK_JEITA_STANDARD_SUPPORT)
    if (BMT_status.bat_vol < g_jeita_recharging_voltage)
#else
    if (BMT_status.bat_vol < batt_cust_data.recharging_voltage)
#endif
    {
        BMT_INFO("[BATTERY] Battery Enter Re-charging!! , vbat=(%d)", (int)BMT_status.bat_vol);

        BMT_status.bat_in_recharging_state = true;
        BMT_status.bat_charging_state = CHR_CC;
    }

    /*  Disable charging */
    hal_charger_enable(charging_enable);

    return PMU_STATUS_OK;
}


PMU_STATUS battery_driver_battery_status_fail_action(void)
{
    bool charging_enable;

    BMT_ERR("[BATTERY] BAD Battery status... Charging Stop !!");

#if defined(CONFIG_MTK_JEITA_STANDARD_SUPPORT)
    if ((g_temp_status == TEMP_ABOVE_POS_60) || (g_temp_status == TEMP_BELOW_NEG_10)) {
        temp_error_recovery_chr_flag = false;
    }
    if ((temp_error_recovery_chr_flag == false) && (g_temp_status != TEMP_ABOVE_POS_60)
            && (g_temp_status != TEMP_BELOW_NEG_10)) {
        temp_error_recovery_chr_flag = true;
        BMT_status.bat_charging_state = CHR_PRE;
    }
#endif

    BMT_status.total_charging_time = 0;
    BMT_status.PRE_charging_time = 0;
    BMT_status.CC_charging_time = 0;
    BMT_status.TOPOFF_charging_time = 0;
    BMT_status.POSTFULL_charging_time = 0;

    /*  Disable charger */
    charging_enable = false;
    hal_charger_enable(charging_enable);

    return PMU_STATUS_OK;
}


void battery_driver_charging_algorithm(void)
{
    battery_driver_init_charging_varaibles();

    switch (BMT_status.bat_charging_state) {
        case CHR_PRE:
            battery_driver_precharger_mode_action();
            break;

        case CHR_CC:
            battery_driver_constant_current_mode_action();
            break;

        case CHR_TOP_OFF:
            battery_driver_top_off_mode_action();
            break;

        case CHR_BATFULL:
            battery_driver_battery_full_action();
            break;

        case CHR_ERROR:
            battery_driver_battery_status_fail_action();
            break;
    }

}


