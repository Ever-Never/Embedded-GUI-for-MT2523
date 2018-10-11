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

#ifndef __FUELGAUGE_INTERFACE_H__
#define __FUELGAUGE_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#define FG_CURRENT_AVERAGE_SIZE     20
#define FG_VBAT_AVERAGE_SIZE        20

enum {
    HW_FG,
    SW_FG,
    AUXADC
};

typedef enum {
    BATTERY_THREAD_TIME = 0,
    CAR_TIME,
    SUSPEND_TIME,
    AP_SUSPEND_TIME,
    DURATION_NUM
} BATTERY_TIME_ENUM;

typedef struct _BATTERY_PROFILE_STRUC {
    int16_t     percentage;
    int16_t     voltage;
} BATTERY_PROFILE_STRUC, *BATTERY_PROFILE_STRUC_P;

typedef struct _R_PROFILE_STRUC {
    int16_t     resistance; /* Ohm */
    int16_t     voltage;
} R_PROFILE_STRUC, *R_PROFILE_STRUC_P;

struct battery_meter_custom_data {

    /* cust_battery_meter.h */
    int32_t soc_flow;

    int32_t temperature_t0;
    int32_t temperature_t1;
    int32_t temperature_t2;
    int32_t temperature_t3;
    int32_t temperature_t;

    int32_t fg_meter_resistance;

    /* Qmax for battery  */
    int32_t q_max_pos_50;
    int32_t q_max_pos_25;
    int32_t q_max_pos_0;
    int32_t q_max_neg_10;
    int32_t q_max_pos_50_h_current;
    int32_t q_max_pos_25_h_current;
    int32_t q_max_pos_0_h_current;
    int32_t q_max_neg_10_h_current;

    int32_t change_tracking_point;
    int32_t cust_tracking_point;
    int32_t aging_tuning_value;
    int32_t ocv_board_compesate;
    int32_t car_tune_value;


    int32_t difference_hwocv_rtc;
    int32_t difference_hwocv_swocv;
    int32_t difference_swocv_rtc;
    int32_t max_swocv;

    int32_t shutdown_system_voltage;
    int32_t recharge_tolerance;

    int32_t batterypseudo100;
    int32_t batterypseudo1;

    int32_t q_max_by_current;       /* 7. qmax varient by current loading. */
    int32_t q_max_sys_voltage;      /* 8. qmax variant by sys voltage. */

    int32_t shutdown_gauge0;
    int32_t shutdown_gauge1_xmins;
    int32_t shutdown_gauge1_mins;

    int32_t min_charging_smooth_time;

    /* SW Fuel gauge */
    int32_t apsleep_battery_voltage_compensate;

};

struct battery_meter_table_custom_data_p {

    int32_t battery_profile_t0_size;
    BATTERY_PROFILE_STRUC *cust_battery_profile_t0;
    int32_t battery_profile_t1_size;
    BATTERY_PROFILE_STRUC *cust_battery_profile_t1;
    int32_t battery_profile_t2_size;
    BATTERY_PROFILE_STRUC *cust_battery_profile_t2;
    int32_t battery_profile_t3_size;
    BATTERY_PROFILE_STRUC *cust_battery_profile_t3;
    int32_t battery_profile_temperature_size;
    BATTERY_PROFILE_STRUC *cust_battery_profile_temperature;

    int32_t r_profile_t0_size;
    R_PROFILE_STRUC *cust_r_profile_t0;
    int32_t r_profile_t1_size;
    R_PROFILE_STRUC *cust_r_profile_t1;
    int32_t r_profile_t2_size;
    R_PROFILE_STRUC *cust_r_profile_t2;
    int32_t r_profile_t3_size;
    R_PROFILE_STRUC *cust_r_profile_t3;
    int32_t r_profile_temperature_size;
    R_PROFILE_STRUC *cust_r_profile_temperature;
};

struct fgd_cmd_param_t_custom {
    int32_t version;
    struct battery_meter_custom_data fgd_custom_data;
    struct battery_meter_table_custom_data_p fgd_table_custom_data;
};

/* ============================================================ */
/* typedef */
/* ============================================================ */

typedef enum {
    FG_CMD_GET_RTC_SPARE_FG_VALUE,
    FG_CMD_IS_CHARGER_EXIST,
    FG_CMD_GET_SHUTDOWN_SYSTEM_VOLTAGE,
    FG_CMD_GET_BATTERY_INIT_VOLTAGE,
    FG_CMD_GET_HW_FG_INIT_CURRENT,
    FG_CMD_GET_BATTERY_VOLTAGE,
    FG_CMD_GET_TEMPERTURE,
    FG_CMD_GET_CUSTOM_TABLE,
    FG_CMD_GET_FG_CUSTOM_DATA,
    FG_CMD_GET_HW_OCV,
    FG_CMD_GET_BATTERY_PLUG_STATUS,
    FG_CMD_GET_DURATION_TIME,
    FG_CMD_IS_BATTERY_FULL,
    FG_CMD_GET_CV_VALUE,
    FG_CMD_GET_SUSPEND_CAR,
    FG_CMD_GET_SUSPEND_TIME,
    FG_CMD_IS_HW_OCV_UPDATE,
    FG_CMD_GET_CHARGE_TRACKING_TIME,
    FG_CMD_GET_DISCHARGE_TRACKING_TIME,
    FG_CMD_SET_SWOCV,
    FG_CMD_SET_SOC,
    FG_CMD_SET_WAKEUP_SMOOTH_TIME,
    FG_CMD_SET_BATTERY_FULL,
    FG_CMD_SET_UI_SOC,
    FG_CMD_SET_UI_SOC2,
    FG_CMD_SET_RTC,
    FG_CMD_SET_OAM_V_OCV,
    FG_CMD_SET_OAM_R,
    FG_CMD_SET_POWEROFF,
    FG_CMD_NUMBER
} FG_CTRL_CMD;

typedef uint32_t (*fuel_gauge_command_callback_t)(FG_CTRL_CMD cmd, void *parameter, void *data);
typedef void (*fuel_gauge_debug_message_callback_t)(const char *format, va_list args);

typedef struct {
    fuel_gauge_command_callback_t fgauge_operate;
    fuel_gauge_debug_message_callback_t fgauge_debug_message_operate;
} fg_data_t;

void fg_set_debug_level(int32_t debug_level);
void fgauge_register_callback(fuel_gauge_command_callback_t func);
void fgauge_register_debug_message_callback(fuel_gauge_debug_message_callback_t func);
int32_t  fgauge_get_sw_ocv(bool init);
void fgauge_initialization(void);
void sw_fg_main_flow(void);
void fg_suspend_flow(void);
void fg_resume_flow(void);
void reset_parameter_dod_charger_plug_event(void);

#ifdef __cplusplus
}
#endif

#endif /*__FUELGAUGE_INTERFACE_H__*/

