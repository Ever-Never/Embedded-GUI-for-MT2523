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
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "syslog.h"
#include "hal_pmu.h"
#include "hal_gpt.h"
#include "hal_rtc.h"
#include "hal_sleep_manager.h"
#include "hal_cm4_topsm.h"
#include "battery_common.h"
#include "battery_management.h"
#include "cust_charging.h"
#include "task_def.h"

#ifdef BMT_DEBUG_ENABLE
log_create_module(bmt, PRINT_LEVEL_INFO);
#else
log_create_module(bmt, PRINT_LEVEL_WARNING);
#endif

#ifdef BMT_DEBUG_INFO_ENABLE
#define BMT_DBG(fmt,args...)    LOG_I(bmt ,"BM core: "fmt,##args)
#define BMT_PRINT(fmt, args...) LOG_I(bmt ,"BM core: "fmt,##args)
#else
#define BMT_DBG(fmt, args...)
#define BMT_PRINT(fmt, args...)
#endif

#define BMT_INFO(fmt, args...)  LOG_I(bmt ,"BM core: "fmt,##args)
#define BMT_WARN(fmt, args...)  LOG_W(bmt ,"BM core: "fmt,##args)
#define BMT_ERR(fmt, args...)   LOG_E(bmt ,"BM core: "fmt,##args)

#define BMT_FG_DEBUG_ENABLE


/*************************************
*           global variable
**************************************/
extern PMU_ChargerStruct BMT_status;
extern uint32_t battery_thread_time;
extern bool g_suspend_timeout;
extern uint32_t sleep_total_time;
extern uint32_t wake_up_smooth_time;
extern int32_t battery_meter_suspend(void);
extern int32_t battery_meter_resume(void);
extern void usb_cable_detect(void);
#if defined(CONFIG_MTK_JEITA_STANDARD_SUPPORT)
extern PMU_STATUS battery_driver_do_jeita_state_machine(void);
#endif

void battery_management_suspend(void *data);
void battery_management_resume(void *data);

static bool g_bat_init_flag = false;
bool g_battery_soc_ready = false;

uint32_t battery_duration_time[DURATION_NUM] = {0}; /* sec */
static bool battery_meter_initilized = false;
static bool battery_suspended_flag = false;

/* Thermal related flags */
static int32_t g_battery_thermal_throttling_flag = 1;    /* 0:nothing, 1:enable batTT&chrTimer, 2:disable batTT&chrTimer, 3:enable batTT, disable chrTimer */
static int32_t battery_cmd_thermal_test_mode = 0;
static int32_t battery_cmd_thermal_test_mode_value = 0;

#if defined(MTK_TEMPERATURE_RECHARGE_SUPPORT)
static uint32_t g_batt_temp_status = TEMP_POS_NORMAL;
#endif

#define BMT_TIMER_ID0 0
static int32_t bmt_timer_id;
static TimerHandle_t xTimerofBattery = NULL;
static TaskHandle_t battery_management_task_handle = NULL;
static bool g_bmt_gpt_port_init_flag = false;
static uint32_t bmt_gpt_handle = 0;
static bool g_battery_management_init_flag = false;
static bmt_cmd_event_t bmt_timer_event;
static QueueHandle_t bmt_queue_handle;

static bmt_callback_context_t bmt_callback = {0};

static uint32_t battery_core_convert_duration_time_ms(uint32_t pre_time_ms, uint32_t this_time_ms)
{
    if (this_time_ms >= pre_time_ms) {
        return (this_time_ms - pre_time_ms);
    } else {
        /* Avoid overflow in ms 0xffffffff*1000/32768= 0x07cfffff */
        return (0x07cfffff - (pre_time_ms - this_time_ms) + 1);
    }
}

uint32_t battery_core_get_current_time_in_ms(void)
{
    uint32_t count = 0;
    uint64_t count64 = 0;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count);
    count64 = ((uint64_t)count) * 1000 / 32768;

    return (uint32_t)(count64);
}

void battery_core_update_time(uint32_t *pre_time, BATTERY_TIME_ENUM duration_type)
{
    uint32_t time;
    uint32_t duration_time;
    static uint32_t pre_saved_time[DURATION_NUM] = {0};

    time = battery_core_get_current_time_in_ms();

    duration_time = battery_core_convert_duration_time_ms(*pre_time, time);

    battery_duration_time[duration_type] = (duration_time + pre_saved_time[duration_type]) / 1000;
    pre_saved_time[duration_type] = (duration_time + pre_saved_time[duration_type]) % 1000;

    *pre_time = time;

}

uint32_t battery_core_get_duration_time(BATTERY_TIME_ENUM duration_type)
{
    return battery_duration_time[duration_type];
}

bool battery_core_get_charger_status(void)
{
    bool chr_status = false;
    hal_charger_get_charger_detect_status(&chr_status);
    return chr_status;
}

static bool battery_core_is_charger_detect(void)
{
    bool tmp32;
    if (!g_bat_init_flag) {
        BMT_INFO("[battery_core_is_charger_detect] battery thread not ready, will do after bettery init.");
        return false;
    }

    tmp32 = battery_core_get_charger_status();

    if (tmp32 == false) {
        return false;
    } else {
        return true;
    }
}

hal_charger_type_t battery_core_charger_type_detection(void)
{
    hal_charger_type_t CHR_Type_num = HAL_CHARGER_TYPE_UNKNOWN;

    if (BMT_status.charger_type == HAL_CHARGER_TYPE_UNKNOWN) {
        hal_charger_get_charger_type(&CHR_Type_num);
        BMT_status.charger_type = CHR_Type_num;
    }

    return (hal_charger_type_t)BMT_status.charger_type;
}

static void battery_core_check_charger_detect(void)
{
    if (battery_core_is_charger_detect() == true) {
        BMT_status.charger_exist = true;
        if (BMT_status.charger_type == HAL_CHARGER_TYPE_UNKNOWN) {

            battery_core_charger_type_detection();
            if ((BMT_status.charger_type == HAL_CHARGER_TYPE_STANDARD_HOST)
                    || (BMT_status.charger_type == HAL_CHARGER_TYPE_CHARGING_HOST)) {
#ifdef MTK_USB_DEMO_ENABLED
                usb_cable_detect();
#endif
            }
        }
        BMT_INFO("[battery_core_thread]Cable in, CHR_Type_num=%d", (int)BMT_status.charger_type);
    } else {

        BMT_status.charger_exist = false;
        BMT_status.charger_type = HAL_CHARGER_TYPE_UNKNOWN;
        BMT_status.bat_full = false;
        BMT_status.bat_in_recharging_state = false;
        BMT_status.bat_charging_state = CHR_PRE;
        BMT_status.total_charging_time = 0;
        BMT_status.PRE_charging_time = 0;
        BMT_status.CC_charging_time = 0;
        BMT_status.TOPOFF_charging_time = 0;
        BMT_status.POSTFULL_charging_time = 0;

        BMT_INFO("[battery_core_thread]Cable out");

#ifdef MTK_USB_DEMO_ENABLED
        usb_cable_detect();
#endif
    }
}

static void battery_core_average_method_init(BATTERY_AVG_ENUM type, int32_t *bufferdata, uint32_t data,
        int32_t *sum)
{
    uint32_t i;
    static bool batteryBufferFirst = true;
    static bool previous_charger_exist = false;
    static bool previous_in_recharge_state = false;
    static uint8_t index;

    /* reset charging current window while plug in/out { */
    if (type == BATTERY_AVG_CURRENT) {
        if (BMT_status.charger_exist == true) {
            if (previous_charger_exist == false) {
                batteryBufferFirst = true;
                previous_charger_exist = true;
                if (BMT_status.charger_type == HAL_CHARGER_TYPE_STANDARD_CHARGER) {
                    data = AC_CHARGER_CURRENT;
                } else if (BMT_status.charger_type == HAL_CHARGER_TYPE_CHARGING_HOST) {
                    data = CHARGING_HOST_CHARGER_CURRENT;
                } else if (BMT_status.charger_type == HAL_CHARGER_TYPE_NONSTANDARD_CHARGER) {
                    data = NON_STD_AC_CHARGER_CURRENT;    /* mA */
                } else {    /* USB */
                    data = USB_CHARGER_CURRENT;    /* mA */
                }
            } else if ((previous_in_recharge_state == false)
                       && (BMT_status.bat_in_recharging_state == true)) {
                batteryBufferFirst = true;

                if (BMT_status.charger_type == HAL_CHARGER_TYPE_STANDARD_CHARGER) {
                    data = AC_CHARGER_CURRENT;
                } else if (BMT_status.charger_type == HAL_CHARGER_TYPE_CHARGING_HOST) {
                    data = CHARGING_HOST_CHARGER_CURRENT;
                } else if (BMT_status.charger_type == HAL_CHARGER_TYPE_NONSTANDARD_CHARGER) {
                    data = NON_STD_AC_CHARGER_CURRENT;    /* mA */
                } else {    /* USB */
                    data = USB_CHARGER_CURRENT;    /* mA */
                }
            }

            previous_in_recharge_state = BMT_status.bat_in_recharging_state;
        } else {
            if (previous_charger_exist == true) {
                batteryBufferFirst = true;
                previous_charger_exist = false;
                data = 0;
            }
        }
    }
    /* reset charging current window while plug in/out } */

    BMT_DBG("batteryBufferFirst =%d, data= (%d)",
            (int) batteryBufferFirst, (int) data);

    if (batteryBufferFirst == true) {
        for (i = 0; i < BATTERY_AVERAGE_SIZE; i++) {
            bufferdata[i] = data;
        }

        *sum = data * BATTERY_AVERAGE_SIZE;
    }

    index++;
    if (index >= BATTERY_AVERAGE_DATA_NUMBER) {
        index = BATTERY_AVERAGE_DATA_NUMBER;
        batteryBufferFirst = false;
    }
}

static uint32_t battery_core_average_method(BATTERY_AVG_ENUM type, int32_t *bufferdata, uint32_t data,
        int32_t *sum, uint8_t batteryIndex)
{
    uint32_t avgdata;

    battery_core_average_method_init(type, bufferdata, data, sum);

    *sum -= bufferdata[batteryIndex];
    *sum += data;
    bufferdata[batteryIndex] = data;
    avgdata = (*sum) / BATTERY_AVERAGE_SIZE;

    BMT_DBG("bufferdata[%d]= (%d)", (int)batteryIndex, (int)bufferdata[batteryIndex]);

    return avgdata;
}


static void battery_core_get_battery_data(void)
{
    uint32_t bat_vol, charger_vol, Vsense, ZCV;
    int32_t ICharging, temperature;
    static int32_t bat_sum, icharging_sum, temperature_sum;
    static int32_t batteryVoltageBuffer[BATTERY_AVERAGE_SIZE];
    static int32_t batteryCurrentBuffer[BATTERY_AVERAGE_SIZE];
    static int32_t batteryTempBuffer[BATTERY_AVERAGE_SIZE];
    static uint8_t batteryIndex = 0;
    static int32_t previous_SOC = -1;

    bat_vol = battery_meter_get_battery_voltage(true);
    Vsense = battery_meter_get_voltage_sense();
    if (battery_core_is_charger_detect() == true) {
        ICharging = battery_meter_get_charging_current();
        charger_vol = battery_meter_get_charger_voltage();
    } else {
        ICharging = 0;
        charger_vol = 0;
    }
    temperature = battery_meter_get_battery_temperature();
    ZCV = battery_meter_get_battery_zcv();

    BMT_status.ICharging =
        battery_core_average_method(BATTERY_AVG_CURRENT, &batteryCurrentBuffer[0], ICharging, &icharging_sum,
                                    batteryIndex);

    if (previous_SOC == -1 && bat_vol <= V_0PERCENT_TRACKING) {
        previous_SOC = 0;
        if (ZCV != 0) {
            BMT_INFO("battery voltage too low, use ZCV to init average data.");
            BMT_status.bat_vol =
                battery_core_average_method(BATTERY_AVG_VOLT, &batteryVoltageBuffer[0], ZCV, &bat_sum,
                                            batteryIndex);
        } else {
            BMT_INFO("battery voltage too low, use V_0PERCENT_TRACKING + 100 to init average data.");
            BMT_status.bat_vol =
                battery_core_average_method(BATTERY_AVG_VOLT, &batteryVoltageBuffer[0], V_0PERCENT_TRACKING + 100, &bat_sum,
                                            batteryIndex);
        }
    } else {
        BMT_status.bat_vol =
            battery_core_average_method(BATTERY_AVG_VOLT, &batteryVoltageBuffer[0], bat_vol, &bat_sum,
                                        batteryIndex);
    }
    BMT_status.temperature =
        battery_core_average_method(BATTERY_AVG_TEMP, &batteryTempBuffer[0], temperature, &temperature_sum,
                                    batteryIndex);
    BMT_status.Vsense = Vsense;
    BMT_status.charger_vol = charger_vol;
    BMT_status.ZCV = ZCV;

    batteryIndex++;
    if (batteryIndex >= BATTERY_AVERAGE_SIZE) {
        batteryIndex = 0;
    }


    BMT_INFO("[kernel]AvgVbat %d,bat_vol %d, AvgI %d, I %d, VChr %d, AvgT %d, T %d, ZCV %d, UI_SOC2 %d, LEVEL %d",
             (int)BMT_status.bat_vol, (int)bat_vol, (int)BMT_status.ICharging, (int)ICharging,
             (int)BMT_status.charger_vol, (int)BMT_status.temperature, (int)temperature,
             (int)BMT_status.ZCV, (int)BMT_status.UI_SOC2, (int)BMT_status.UI_SOC2_LEVEL);
}

static void battery_core_check_thermal(void)
{
    if ((g_battery_thermal_throttling_flag == 1) || (g_battery_thermal_throttling_flag == 3)) {
        if (battery_cmd_thermal_test_mode == 1) {
            BMT_status.temperature = battery_cmd_thermal_test_mode_value;
            BMT_DBG("[Battery] In thermal_test_mode , Tbat=%d", (int) BMT_status.temperature);
        }
#if defined(CONFIG_MTK_JEITA_STANDARD_SUPPORT)
        /* ignore default rule */
#else
        if (BMT_status.temperature >= OVER_TEMPERATURE_THRESHOLD) {

            {

                BMT_ERR("[Battery] Tbat(%d)>=%d, system need power down by application.", (int)BMT_status.temperature, OVER_TEMPERATURE_THRESHOLD);

            }
        }
#endif

    }

}

#if defined(MTK_TEMPERATURE_RECHARGE_SUPPORT)
static PMU_STATUS battery_core_do_temp_state_machine(void)
{
    if (BMT_status.temperature == ERR_CHARGE_TEMPERATURE) {
        return PMU_STATUS_FAIL;
    }
#ifdef BAT_LOW_TEMP_PROTECT_ENABLE
    if (BMT_status.temperature < MIN_CHARGE_TEMPERATURE) {
        BMT_ERR("[BATTERY] Battery Under Temperature or NTC fail !!");
        g_batt_temp_status = TEMP_POS_LOW;
        return PMU_STATUS_FAIL;
    } else if (g_batt_temp_status == TEMP_POS_LOW) {
        if (BMT_status.temperature >= MIN_CHARGE_TEMPERATURE_PLUS_X_DEGREE) {
            BMT_WARN("[BATTERY] Battery Temperature raise from %d to %d(%d), allow charging!!",
                     (int)MIN_CHARGE_TEMPERATURE, (int)BMT_status.temperature,
                     (int)MIN_CHARGE_TEMPERATURE_PLUS_X_DEGREE);
            g_batt_temp_status = TEMP_POS_NORMAL;
            BMT_status.bat_charging_state = CHR_PRE;
            return PMU_STATUS_OK;
        } else {
            return PMU_STATUS_FAIL;
        }
    } else
#endif
        if (BMT_status.temperature >= MAX_CHARGE_TEMPERATURE) {
            BMT_ERR("[BATTERY] Battery Over Temperature !!");
            g_batt_temp_status = TEMP_POS_HIGH;
            return PMU_STATUS_FAIL;
        } else if (g_batt_temp_status == TEMP_POS_HIGH) {
            if (BMT_status.temperature < MAX_CHARGE_TEMPERATURE_MINUS_X_DEGREE) {
                BMT_WARN("[BATTERY] Battery Temperature down from %d to %d(%d), allow charging!!",
                         (int)MAX_CHARGE_TEMPERATURE, (int)BMT_status.temperature,
                         (int)MAX_CHARGE_TEMPERATURE_MINUS_X_DEGREE);
                g_batt_temp_status = TEMP_POS_NORMAL;
                BMT_status.bat_charging_state = CHR_PRE;
                return PMU_STATUS_OK;
            } else {
                return PMU_STATUS_FAIL;
            }
        } else {
            g_batt_temp_status = TEMP_POS_NORMAL;
        }
    return PMU_STATUS_OK;
}
#endif

static PMU_STATUS battery_core_check_battery_temperature(void)
{
    PMU_STATUS status = PMU_STATUS_OK;

#if defined(CONFIG_MTK_JEITA_STANDARD_SUPPORT)

    BMT_INFO("[BATTERY] support JEITA, temperature=%d", (int)BMT_status.temperature);
    if (battery_driver_do_jeita_state_machine() == PMU_STATUS_FAIL) {
        BMT_ERR("[BATTERY] JEITA : fail");
        status = PMU_STATUS_FAIL;
    }
#else

#if defined(MTK_TEMPERATURE_RECHARGE_SUPPORT)
    if (battery_core_do_temp_state_machine() == PMU_STATUS_FAIL) {
        BMT_ERR("[BATTERY] Batt temp check : fail");
        status = PMU_STATUS_FAIL;
    }
#else
#ifdef BAT_LOW_TEMP_PROTECT_ENABLE
    if ((BMT_status.temperature < MIN_CHARGE_TEMPERATURE)
            || (BMT_status.temperature == ERR_CHARGE_TEMPERATURE)) {
        BMT_ERR("[BATTERY] Battery Under Temperature or NTC fail A!!");
        status = PMU_STATUS_FAIL;
    }
#endif
    if (BMT_status.temperature >= MAX_CHARGE_TEMPERATURE) {
        BMT_ERR("[BATTERY] Battery Over Temperature !!");
        status = PMU_STATUS_FAIL;
    }
#endif

#endif
    return status;
}

static PMU_STATUS battery_core_check_charger_voltage(void)
{
    PMU_STATUS status = PMU_STATUS_OK;

    if (BMT_status.charger_exist == true) {
#if (V_CHARGER_ENABLE == 1)
        if (BMT_status.charger_vol <= V_CHARGER_MIN) {
            BMT_ERR("[BATTERY]Charger under voltage!!");
            BMT_status.bat_charging_state = CHR_ERROR;
            status = PMU_STATUS_FAIL;
        }
#endif
        if (BMT_status.charger_vol >= V_CHARGER_MAX) {
            BMT_ERR("[BATTERY]Charger over voltage !!");
            BMT_status.bat_charging_state = CHR_ERROR;
            status = PMU_STATUS_FAIL;
        }
    }

    return status;
}

static PMU_STATUS battery_core_check_charging_time(void)
{
    PMU_STATUS status = PMU_STATUS_OK;

    if ((g_battery_thermal_throttling_flag == 2) || (g_battery_thermal_throttling_flag == 3)) {
        BMT_DBG("[TestMode] Disable Safty Timer. bat_tt_enable=%d, bat_thr_test_mode=%d, bat_thr_test_value=%d",
                (int) g_battery_thermal_throttling_flag,
                (int) battery_cmd_thermal_test_mode,
                (int) battery_cmd_thermal_test_mode_value);

    } else {
        /* Charging OT */
        if (BMT_status.total_charging_time >= MAX_CHARGING_TIME) {
            BMT_ERR("[BATTERY] Charging Over Time.");

            status = PMU_STATUS_FAIL;
        }
    }

    return status;
}

static void battery_core_check_battery_status(void)
{
    if (battery_core_check_battery_temperature() != PMU_STATUS_OK) {
        BMT_status.bat_charging_state = CHR_ERROR;
        return;
    }

    if (battery_core_check_charger_voltage() != PMU_STATUS_OK) {
        BMT_status.bat_charging_state = CHR_ERROR;
        return;
    }
    if (battery_core_check_charging_time() != PMU_STATUS_OK) {
        BMT_status.bat_charging_state = CHR_ERROR;
        return;
    }
}

void battery_core_check_battery_exist(void)
{
#if defined(CONFIG_DIS_CHECK_BATTERY)
    BMT_INFO("[BATTERY] Disable check battery exist.");
#else
    uint32_t baton_count = 0;
    bool charging_enable = false;
    uint32_t battery_status = 0;
    uint32_t i;

    for (i = 0; i < 3; i++) {
        hal_charger_get_battery_status((bool *)&battery_status);
        if (battery_status) {
            baton_count += 0;
        } else {
            baton_count += 1;
        }
    }

    if (baton_count >= 3) {
        hal_charger_enable(charging_enable);

    }
#endif
}

void battery_management_notification(battery_management_event_t event)
{
    if ((bmt_callback.callback_init == true) && (bmt_callback.func != NULL)) {
        switch (event) {
            case BATTERY_MANAGEMENT_EVENT_BATTERY_UPDATE: {

                bmt_callback.func(event, NULL);
            }
            break;
            default: {
            }
            break;
        }
    }
}

void battery_core_thread(void)
{
    if (battery_meter_initilized == false) {
        battery_core_get_battery_data();
        battery_meter_initial();
        BMT_status.nPercent_ZCV = battery_meter_get_battery_nPercent_zcv();
        battery_meter_initilized = true;
    }

    battery_core_update_time(&battery_thread_time, BATTERY_THREAD_TIME);

    hal_charger_reset_watchdog_timer();

    sw_fg_main_flow();

    battery_core_check_charger_detect();
    battery_core_get_battery_data();
    if (BMT_status.charger_exist == true) {
        battery_core_check_battery_exist();
    }

    battery_core_check_thermal();

    if (BMT_status.charger_exist == true) {
        battery_core_check_battery_status();
        battery_driver_charging_algorithm();
    }

    battery_management_notification(BATTERY_MANAGEMENT_EVENT_BATTERY_UPDATE);

}

static TickType_t battery_core_get_wakeup_ticks(void)
{
    TickType_t xQueueTicksToWait;
#ifdef  BAT_SLEEP_FORCE_WAKEUP_DISABLE
    uint32_t wakeup_time = battery_meter_get_dynamic_period();

    if (wakeup_time == 0) {
        xQueueTicksToWait = portMAX_DELAY;
    } else if (wakeup_time <= BAT_TASK_PERIOD + 1) {
        wakeup_time = BAT_TASK_PERIOD + 1;
        xQueueTicksToWait = wakeup_time * 1000 / portTICK_PERIOD_MS;
    } else {
        xQueueTicksToWait = wakeup_time * 1000 / portTICK_PERIOD_MS;
    }
    BMT_INFO("wakeup_time = %d ", wakeup_time);

#else
    xQueueTicksToWait = portMAX_DELAY;
#endif
    return xQueueTicksToWait;
}

void battery_core_wake_up_battery(void)
{
    BaseType_t xHigherPriorityTaskWoken;

    BMT_INFO("[BATTERY] battery_core_wake_up_battery.");

    sleep_total_time = 0;

    bmt_timer_event.event = BMT_EVENT_WAKE_UP_BATTERY;

    xQueueOverwriteFromISR(bmt_queue_handle, &bmt_timer_event, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

}

static void battery_core_pmu_charger_detect_callback(uint8_t trigger_source, void *parameter)
{
    BMT_INFO("CHRDET CB success = %d", trigger_source);

    battery_core_wake_up_battery();
}

void vbattery_timeout_callback(TimerHandle_t pxTimer)
{
    bmt_timer_event.event = BMT_EVENT_TIMEOUT;
    xQueueSend(bmt_queue_handle, &bmt_timer_event, 0);
}

void battery_management_gpt_callback(void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken;
    hal_gpt_status_t ret_state;

    BMT_INFO("[battery_management_gpt_callback] gpt");

    ret_state = hal_gpt_sw_start_timer_ms(\
                                          bmt_gpt_handle, \
                                          BAT_SLEEP_WAKE_UP_PERIOD * 1000, \
                                          (hal_gpt_callback_t)battery_management_gpt_callback, \
                                          NULL);

    if (ret_state != HAL_GPT_STATUS_OK) {
        BMT_ERR("start timer handle error,ret = 0x%x\r\n", ret_state);
    }


    bmt_timer_event.event = BMT_EVENT_TIMEOUT;

    xQueueSendFromISR(bmt_queue_handle, &bmt_timer_event, &xHigherPriorityTaskWoken);

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

}

void battery_management_task(void *pvParameters)
{
    const TickType_t xTicksToWait = 20000 / portTICK_PERIOD_MS;
    TickType_t xQueueTicksToWait = portMAX_DELAY;

    bmt_cmd_event_t event_item;
    uint8_t sleep_handler;
    uint32_t xLastExecutionTime;

    BMT_DBG("batery_manager_task_init");

    sleep_handler = hal_sleep_manager_set_sleep_handle("BatteryManagement");

    battery_thread_time = battery_core_get_current_time_in_ms();

    g_bat_init_flag = true;

    while (1) {
        xQueueTicksToWait = battery_core_get_wakeup_ticks();
        if (xQueueReceive(bmt_queue_handle, &event_item, xQueueTicksToWait)) {

            hal_sleep_manager_lock_sleep(sleep_handler);

            xLastExecutionTime = xTaskGetTickCount();
            BMT_INFO("[QueueReceive] event_item.event = %d", event_item.event);
            if (event_item.event == BMT_EVENT_WAKE_UP_BATTERY) {
                if (battery_core_is_charger_detect() == true) {
                    BMT_INFO("[do_chrdet_int_task] charger exist!");
                    BMT_status.charger_exist = true;
                    /*Stop gpt timer */
                    hal_gpt_sw_stop_timer_ms(bmt_gpt_handle);
                    /*Enable 10s timer */
                    xTimerChangePeriod(xTimerofBattery, BAT_TASK_PERIOD * 1000 / portTICK_PERIOD_MS, xTicksToWait);
                    xTimerReset(xTimerofBattery, 1000 / portTICK_PERIOD_MS);

                    xTimerStart(xTimerofBattery, xTicksToWait);
                    BMT_INFO("Start xTimerofBattery when charger in");

                } else {
                    BMT_INFO("[do_chrdet_int_task] charger NOT exist!");
                    BMT_status.charger_exist = false;
                    /*Start gpt timer*/
                    hal_gpt_sw_start_timer_ms(\
                                              bmt_gpt_handle, \
                                              BAT_SLEEP_WAKE_UP_PERIOD * 1000, \
                                              (hal_gpt_callback_t)battery_management_gpt_callback, \
                                              NULL);
                    /*Stop 10s timer*/
                    if (xTimerIsTimerActive(xTimerofBattery) != pdFALSE) {
                        xTimerStop(xTimerofBattery, 1000 / portTICK_PERIOD_MS);
                        BMT_INFO("Stop xTimerofBattery when charger out");
                    }

                }

                reset_parameter_dod_charger_plug_event();

                /* Place charger detection and battery update here is used to speed up charging icon display. */
                battery_core_check_charger_detect();
                if (BMT_status.UI_SOC2 == 100 && BMT_status.charger_exist == true) {
                    BMT_status.bat_charging_state = CHR_BATFULL;
                    BMT_status.bat_full = true;
                }

                if (g_battery_soc_ready == false) {
                    if (BMT_status.nPercent_ZCV == 0) {
                        battery_meter_initial();
                    }
                }

            }

            BMT_DBG("Queue Receive");
            battery_core_thread();
            xLastExecutionTime = (xTaskGetTickCount() - xLastExecutionTime);
            BMT_DBG("Process Time = %ld", (xLastExecutionTime) / portTICK_PERIOD_MS);
            hal_sleep_manager_unlock_sleep(sleep_handler);
        } else {
            BMT_INFO("xQueueTicksToWait time out");
            sleep_total_time = 0;
            wake_up_smooth_time = 0;
            bmt_timer_event.event = BMT_EVENT_TIMEOUT;
            xQueueSend(bmt_queue_handle, &bmt_timer_event, 0 / portTICK_PERIOD_MS);
        }
    }
}

battery_management_status_t  battery_management_register_callback(battery_management_callback_t callback)
{
    if (g_battery_management_init_flag == false) {
        return BATTERY_MANAGEMENT_STATUS_ERROR;
    }

    bmt_callback.func = callback;
    bmt_callback.callback_init = true;

    return BATTERY_MANAGEMENT_STATUS_OK;
}

battery_management_status_t battery_management_init(void)
{
    hal_gpt_status_t ret_state;

    BMT_DBG("battery_management_init");

    if (g_battery_management_init_flag == true) {
        return BATTERY_MANAGEMENT_STATUS_ERROR;
    }

#ifdef BMT_FG_DEBUG_ENABLE
    fg_set_debug_level(1);
#else
    fg_set_debug_level(0);
#endif
    hal_rtc_init();
    hal_charger_init();

    /* Default pre-charging current. It depends on the battery specifications.
       The USBDL mode current level  = 16.8mV / R_SESNE */
    hal_charger_set_charging_current(16800 / CUST_R_SENSE);
    hal_charger_enable(true);

    battery_meter_init();

    memset(&bmt_callback, 0, sizeof(bmt_callback_context_t));

    /* Initialization BMT Struct */
    BMT_status.bat_exist = true;    /* device must have battery */
    BMT_status.charger_exist = false;   /* for default, no charger */
    BMT_status.bat_vol = 0;
    BMT_status.ICharging = 0;
    BMT_status.temperature = 0;
    BMT_status.charger_vol = 0;
    BMT_status.total_charging_time = 0;
    BMT_status.PRE_charging_time = 0;
    BMT_status.CC_charging_time = 0;
    BMT_status.TOPOFF_charging_time = 0;
    BMT_status.POSTFULL_charging_time = 0;
    BMT_status.SOC = 0;
    BMT_status.UI_SOC = -100;
    BMT_status.UI_SOC2 = -100;
    BMT_status.UI_SOC2_LEVEL = -1;

    BMT_status.bat_charging_state = CHR_PRE;
    BMT_status.bat_in_recharging_state = false;
    BMT_status.bat_full = false;
    BMT_status.nPercent_ZCV = 0;
    BMT_status.charger_type = HAL_CHARGER_TYPE_UNKNOWN;

    if (battery_core_get_charger_status()) {
        battery_core_charger_type_detection();
        BMT_status.charger_exist = true;
    }

    BMT_DBG("battery management timer setup");

    if (g_bmt_gpt_port_init_flag == false) {

        ret_state = hal_gpt_sw_get_timer(&bmt_gpt_handle);
        if (ret_state != HAL_GPT_STATUS_OK) {
            BMT_ERR("get timer handle error,ret = %d, handle = 0x%x\r\n", \
                    (unsigned int)ret_state, \
                    (unsigned int)bmt_gpt_handle);

            return BATTERY_MANAGEMENT_STATUS_ERROR;
        }

        BMT_INFO("GPT get timer handle ,ret = %d, handle = 0x%x\r\n", \
                 (unsigned int)ret_state, \
                 (unsigned int)bmt_gpt_handle);

        g_bmt_gpt_port_init_flag = true;
    }

    bmt_timer_id = BMT_TIMER_ID0;
    xTimerofBattery = xTimerCreate("TimerofBattery",       /* Just a text name, not used by the kernel. */
                                   (BAT_TASK_PERIOD * 1000 / portTICK_PERIOD_MS),    /* The timer period in ticks. */
                                   pdTRUE,        /* The timers will auto-reload themselves when they expire. */
                                   (void *)bmt_timer_id,   /* Assign each timer a unique id equal to its array index. */
                                   vbattery_timeout_callback /* Each timer calls the same callback when it expires. */
                                  );
    if (xTimerofBattery == NULL) {
        BMT_ERR("xTimerofBattery create fail");
        return BATTERY_MANAGEMENT_STATUS_ERROR;
    }

    /* Queue creation */
    bmt_queue_handle = xQueueCreate(BMT_QUEUE_LENGTH, sizeof(bmt_cmd_event_t));

#ifdef BAT_SLEEP_FORCE_WAKEUP_DISABLE
    /* Suspend and Resume CBs */
    hal_cm4_topsm_register_suspend_cb((cm4_topsm_cb)battery_management_suspend, NULL);
    hal_cm4_topsm_register_resume_cb((cm4_topsm_cb)battery_management_resume, NULL);
#endif

    if (battery_meter_initilized == false) {
        battery_core_get_battery_data();
        battery_meter_initial();
        BMT_status.nPercent_ZCV = battery_meter_get_battery_nPercent_zcv();
        battery_meter_initilized = true;
    }

    if (g_bmt_gpt_port_init_flag == true) {
        ret_state = hal_gpt_sw_start_timer_ms(\
                                              bmt_gpt_handle, \
                                              BAT_SLEEP_WAKE_UP_PERIOD * 1000, \
                                              (hal_gpt_callback_t)battery_management_gpt_callback, \
                                              NULL);
        if (ret_state != HAL_GPT_STATUS_OK) {
            BMT_ERR("start timer handle error,ret = 0x%x\r\n", ret_state);
        }

    }

    bmt_timer_event.event = BMT_EVENT_INIT;
    xQueueSend(bmt_queue_handle, &bmt_timer_event, 0 / portTICK_PERIOD_MS);

    pmu_register_callback(PMU_INT_CHRDET, PMU_EDGE_FALLING_AND_RISING, battery_core_pmu_charger_detect_callback, NULL);

    xTaskCreate(battery_management_task, BMT_TASK_NAME, BMT_TASK_STACKSIZE / sizeof(StackType_t), NULL, BMT_TASK_PRIO, &battery_management_task_handle);

    g_battery_management_init_flag = true;

    return BATTERY_MANAGEMENT_STATUS_OK;
}

battery_management_status_t battery_management_deinit(void)
{
    BMT_DBG("battery_management_deinit");

    if (g_battery_management_init_flag == false) {
        return BATTERY_MANAGEMENT_STATUS_ERROR;
    }

    if (g_bmt_gpt_port_init_flag == true) {
        hal_gpt_sw_stop_timer_ms(bmt_gpt_handle);
        hal_gpt_sw_free_timer(bmt_gpt_handle);
        g_bmt_gpt_port_init_flag = false;
    } else {
        return BATTERY_MANAGEMENT_STATUS_ERROR;
    }

    memset(&bmt_callback, 0, sizeof(bmt_callback_context_t));

    pmu_disable_interrupt(PMU_INT_CHRDET, PMU_EDGE_FALLING_AND_RISING);

    if (xTimerofBattery != NULL) {
        xTimerDelete(xTimerofBattery, (3000 / portTICK_PERIOD_MS));
    }

    if (battery_management_task_handle != NULL) {
        vTaskDelete(battery_management_task_handle);
    }
    g_bat_init_flag = false;
    hal_charger_enable(false);

    BMT_status.bat_exist = true;
    BMT_status.charger_exist = false;
    BMT_status.bat_vol = 0;
    BMT_status.ICharging = 0;
    BMT_status.temperature = 0;
    BMT_status.charger_vol = 0;
    BMT_status.total_charging_time = 0;
    BMT_status.PRE_charging_time = 0;
    BMT_status.CC_charging_time = 0;
    BMT_status.TOPOFF_charging_time = 0;
    BMT_status.POSTFULL_charging_time = 0;
    BMT_status.SOC = 0;
    BMT_status.UI_SOC = -100;
    BMT_status.UI_SOC2 = -100;
    BMT_status.UI_SOC2_LEVEL = -1;

    BMT_status.bat_charging_state = CHR_PRE;
    BMT_status.bat_in_recharging_state = false;
    BMT_status.bat_full = false;
    BMT_status.nPercent_ZCV = 0;

    g_battery_management_init_flag = false;

    return BATTERY_MANAGEMENT_STATUS_OK;
}

int32_t battery_management_get_battery_property(battery_property_t property)
{
    int32_t property_value;

    switch (property) {
        case BATTERY_PROPERTY_CAPACITY:
            property_value = BMT_status.UI_SOC2;
            break;
        case BATTERY_PROPERTY_CHARGING_CURRENT: {
            if (BMT_status.ICharging >= 0) {
                property_value = BMT_status.ICharging;
            } else {
                property_value = 0;
            }
        }
        break;
        case BATTERY_PROPERTY_CHARGER_EXIST:
            if (BMT_status.charger_exist) {
                property_value = 1;
            } else {
                property_value = 0;
            }
            break;
        case BATTERY_PROPERTY_CHARGER_TYPE:
            property_value = (int32_t)BMT_status.charger_type;
            break;
        case BATTERY_PROPERTY_TEMPERATURE:
            property_value = BMT_status.temperature;
            break;
        case BATTERY_PROPERTY_VOLTAGE:
            property_value = BMT_status.bat_vol;
            break;
        case BATTERY_PROPERTY_CAPACITY_LEVEL:
            property_value = BMT_status.UI_SOC2_LEVEL;
            break;
        default:
            property_value = 0;
            break;
    }
    return property_value;
}

void battery_management_suspend(void *data)
{
    BMT_PRINT("battery_management_suspend");
    if (!battery_core_get_charger_status()) {
        hal_gpt_sw_stop_timer_ms(bmt_gpt_handle);
        battery_meter_suspend();
        battery_suspended_flag = true;
    }

}

void battery_management_resume(void *data)
{

    BMT_PRINT("[battery_management_resume]");

    if (!battery_core_get_charger_status() && battery_suspended_flag == true) {

        battery_meter_resume();

        if (g_suspend_timeout) {
            g_suspend_timeout = false;
            sleep_total_time = 0;
            BMT_PRINT("battery_core_thread");
            if (g_bat_init_flag) {
                bmt_timer_event.event = BMT_EVENT_RESUME;
                xQueueSend(bmt_queue_handle, &bmt_timer_event, 0 / portTICK_PERIOD_MS);
            }
        }
        hal_gpt_sw_start_timer_ms(\
                                  bmt_gpt_handle, \
                                  BAT_SLEEP_WAKE_UP_PERIOD * 1000, \
                                  (hal_gpt_callback_t)battery_management_gpt_callback, \
                                  NULL);

    }
    battery_suspended_flag = false;
}

