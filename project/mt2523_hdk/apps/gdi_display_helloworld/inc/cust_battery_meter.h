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

#ifndef __CUST_BATTERY_METER_H__
#define __CUST_BATTERY_METER_H__


#ifdef __cplusplus
extern "C" {
#endif

#define SOC_BY_SW_FG

#define TEMPERATURE_T0             110
#define TEMPERATURE_T1             0
#define TEMPERATURE_T2             25
#define TEMPERATURE_T3             50
#define TEMPERATURE_T              255  /* This should be fixed, never change the value */

#define FG_METER_RESISTANCE     0

/* Qmax for battery  */
#define Q_MAX_POS_50    488
#define Q_MAX_POS_25    479
#define Q_MAX_POS_0     356
#define Q_MAX_NEG_10    293

#define Q_MAX_POS_50_H_CURRENT    483
#define Q_MAX_POS_25_H_CURRENT    466
#define Q_MAX_POS_0_H_CURRENT     131
#define Q_MAX_NEG_10_H_CURRENT    113


/* battery meter parameter */
#define CHANGE_TRACKING_POINT
#define CUST_TRACKING_POINT     0
#define AGING_TUNING_VALUE      100

#define OCV_BOARD_COMPESATE     0       /* mV */
#define CAR_TUNE_VALUE          100     /* 1.00 */

#define DIFFERENCE_HWOCV_RTC         30
#define DIFFERENCE_HWOCV_SWOCV       0
#define DIFFERENCE_SWOCV_RTC         10
#define MAX_SWOCV                    3

#define BATTERYPSEUDO100             95
#define BATTERYPSEUDO1               4

/*#define Q_MAX_BY_CURRENT*/            /* 7. Qmax varient by current loading. */
/*#define Q_MAX_BY_SYS*/                /* 8. Qmax varient by system drop voltage. */

#define SHUTDOWN_GAUGE0
/*#define SHUTDOWN_GAUGE1_XMINS*/
#define SHUTDOWN_GAUGE1_MINS         60

#define SHUTDOWN_SYSTEM_VOLTAGE      3400
#define CHARGE_TRACKING_TIME         60
#define DISCHARGE_TRACKING_TIME      10

#define RECHARGE_TOLERANCE           10

/* Dynamic change wake up period of battery thread when suspend*/
#define VBAT_NORMAL_WAKEUP           3600   /* 3.6V */
#define VBAT_LOW_POWER_WAKEUP        3500   /* 3.5v */
#define NORMAL_WAKEUP_PERIOD         2700   /* 45 * 60 = 45 min */
#define LOW_POWER_WAKEUP_PERIOD      300    /* 5 * 60 = 5 min */
#define CLOSE_POWEROFF_WAKEUP_PERIOD 30     /* 30 s */



#ifdef __cplusplus
}
#endif

#endif /*__CUST_BATTERY_METER_H__*/

