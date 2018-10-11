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

#ifndef __BATTERY_METER_H__
#define __BATTERY_METER_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "fuelgauge_interface.h"

struct battery_meter_table_custom_data {

    int32_t battery_profile_t0_size;
    BATTERY_PROFILE_STRUC battery_profile_t0[51];
    int32_t battery_profile_t1_size;
    BATTERY_PROFILE_STRUC battery_profile_t1[51];
    int32_t battery_profile_t2_size;
    BATTERY_PROFILE_STRUC battery_profile_t2[51];
    int32_t battery_profile_t3_size;
    BATTERY_PROFILE_STRUC battery_profile_t3[51];
    int32_t battery_profile_temperature_size;
    BATTERY_PROFILE_STRUC battery_profile_temperature[51];

    int32_t r_profile_t0_size;
    R_PROFILE_STRUC r_profile_t0[51];
    int32_t r_profile_t1_size;
    R_PROFILE_STRUC r_profile_t1[51];
    int32_t r_profile_t2_size;
    R_PROFILE_STRUC r_profile_t2[51];
    int32_t r_profile_t3_size;
    R_PROFILE_STRUC r_profile_t3[51];
    int32_t r_profile_temperature_size;
    R_PROFILE_STRUC r_profile_temperature[51];
};

struct battery_custom_data {
    /* cust_charging.h */

    /* Linear Charging Threshold */
    int32_t v_pre2cc_thres;
    int32_t v_cc2topoff_thres;
    int32_t recharging_voltage;
    int32_t charging_full_current;

    /* Charging Current Setting */
    int32_t usb_charger_current;
    int32_t ac_charger_current;
    int32_t non_std_ac_charger_current;
    int32_t charging_host_charger_current;

    /* High battery support */
    int32_t high_battery_voltage_support;

};


/* ============================================================ */
/* External function */
/* ============================================================ */
extern void battery_meter_init(void);
extern int32_t battery_meter_initial(void);
extern int32_t battery_meter_get_battery_nPercent_zcv(void);
extern int32_t battery_meter_get_battery_voltage(bool update);
extern int32_t battery_meter_get_charger_voltage(void);
extern int32_t battery_meter_get_battery_temperature(void);
extern int32_t battery_meter_get_charging_current(void);
extern int32_t battery_meter_get_battery_nPercent_UI_SOC(void);
extern int32_t battery_meter_sync_current_sense_offset(int32_t bat_i_sense_offset);
extern int32_t battery_meter_get_voltage_sense(void);
extern int32_t battery_meter_get_battery_zcv(void);
extern uint32_t battery_meter_get_dynamic_period(void);
#ifdef __cplusplus
}
#endif

#endif /*__BATTERY_METER_H__*/

