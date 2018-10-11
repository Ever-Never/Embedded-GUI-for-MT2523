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

#ifndef __BATTERY_PARAMETER_H__
#define __BATTERY_PARAMETER_H__

#include "cust_charging.h"
#include "cust_battery_meter.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX_CHARGING_TIME
#define MAX_CHARGING_TIME               24*60*60 /* 24hr */
#endif

#ifndef MAX_CV_CHARGING_TIME
#define MAX_CV_CHARGING_TIME            3*60*60  /* 3hr */
#endif

#ifndef MAX_TOP_OFF_CHARGING_TIME
#define MAX_TOP_OFF_CHARGING_TIME       30*60    /* 30min */
#endif

#define BAT_TASK_PERIOD                     10   /* 10sec */

#ifndef OVER_TEMPERATURE_THRESHOLD
#define OVER_TEMPERATURE_THRESHOLD 60
#endif

#define TEMP_POS_60_THRESHOLD  50
#define TEMP_POS_60_THRES_MINUS_X_DEGREE 47

#define TEMP_POS_45_THRESHOLD  45
#define TEMP_POS_45_THRES_MINUS_X_DEGREE 39

#define TEMP_POS_10_THRESHOLD  10
#define TEMP_POS_10_THRES_PLUS_X_DEGREE 16

#define TEMP_POS_0_THRESHOLD  0
#define TEMP_POS_0_THRES_PLUS_X_DEGREE 6

#define TEMP_NEG_10_THRESHOLD  0
#define TEMP_NEG_10_THRES_PLUS_X_DEGREE  0


#ifdef  BAT_SLEEP_FORCE_WAKEUP_DISABLE
#ifndef BAT_SLEEP_WAKE_UP_PERIOD
#define BAT_SLEEP_WAKE_UP_PERIOD        BAT_TASK_PERIOD
#else
#undef BAT_SLEEP_WAKE_UP_PERIOD
#define BAT_SLEEP_WAKE_UP_PERIOD        BAT_TASK_PERIOD
#endif
#else
#ifndef BAT_SLEEP_WAKE_UP_PERIOD
#define BAT_SLEEP_WAKE_UP_PERIOD    10   /* 10sec or 20sec*/
#else
#if !((BAT_SLEEP_WAKE_UP_PERIOD == 10) || (BAT_SLEEP_WAKE_UP_PERIOD == 20))
#warning "BAT_SLEEP_WAKE_UP_PERIOD define invalid, use default 10 sec"
#undef BAT_SLEEP_WAKE_UP_PERIOD
#define BAT_SLEEP_WAKE_UP_PERIOD    10   /* 10sec */
#endif
#endif
#endif

#ifndef Q_MAX_SYS_VOLTAGE
#define Q_MAX_SYS_VOLTAGE 3300
#endif

#ifndef CHARGE_TRACKING_TIME
#define CHARGE_TRACKING_TIME        60
#endif

#ifndef DISCHARGE_TRACKING_TIME
#define DISCHARGE_TRACKING_TIME        10
#endif

#ifndef RECHARGE_TOLERANCE
#define RECHARGE_TOLERANCE    10
#endif


#ifndef FG_CURRENT_INIT_VALUE
#define FG_CURRENT_INIT_VALUE 300
#endif

#ifndef FG_MIN_CHARGING_SMOOTH_TIME
#define FG_MIN_CHARGING_SMOOTH_TIME 40
#endif


#ifndef AP_SLEEP_CAR
#define AP_SLEEP_CAR 20
#endif

#ifndef APSLEEP_BATTERY_VOLTAGE_COMPENSATE
#define APSLEEP_BATTERY_VOLTAGE_COMPENSATE 150
#endif

#ifndef SUSPEND_WAKEUP_TIME
#define SUSPEND_WAKEUP_TIME 300
#endif

#ifdef __cplusplus
}
#endif

#endif /*__BATTERY_PARAMETER_H__*/

