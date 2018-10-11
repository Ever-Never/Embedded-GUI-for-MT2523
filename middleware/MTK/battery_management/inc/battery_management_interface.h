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

#ifndef __BATTERY_MANAGEMENT_INTERFACE_H__
#define __BATTERY_MANAGEMENT_INTERFACE_H__

#include "syslog.h"
#include "battery_management.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BMT_DEBUG_ENABLE
#define BMT_DBG(fmt,args...)    LOG_I(bmt ,fmt,##args)
#else
#define BMT_DBG(fmt, args...)
#endif

#define BMT_INFO(fmt, args...)  LOG_I(bmt ,fmt,##args)
#define BMT_WARN(fmt, args...)  LOG_W(bmt ,fmt,##args)
#define BMT_ERR(fmt, args...)   LOG_E(bmt ,fmt,##args)

typedef struct {
    battery_management_status_t (*battery_management_init)(void);
    int32_t (*battery_management_get_battery_property)(battery_property_t property);
    battery_management_status_t (*battery_management_register_callback)(battery_management_callback_t callback);
    battery_management_status_t (*battery_management_deinit)(void);
} bmt_function_t;

typedef struct {
    int32_t capacity;
    int32_t charging_current;
    int32_t charger_exist;
    int32_t charger_type;
    int32_t temperature;
    int32_t voltage;
    int32_t capacity_level;
} bmt_battery_t;

typedef struct {
    bool initialized;
    bool callback_init;
    battery_management_callback_t callback_func;
} bmt_context_t;

#ifdef __cplusplus
}
#endif

#endif /*__BATTERY_MANAGEMENT_INTERFACE_H__*/

