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

#ifndef __BSP_CTP_H__
#define __BSP_CTP_H__

#include "stdint.h"

#ifdef MTK_CTP_ENABLE

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*bsp_ctp_callback_t)(void *user_data);

typedef enum {
    CTP_PEN_UP = 0,                         /*release on touch*/
    CTP_PEN_DOWN ,                          /*press on touch*/
    CTP_PEN_MOVE,                           /*move onf touch*/
    CTP_PEN_LONGTAP,                        /*longtap on touch*/
    CTP_PEN_REPEAT,                         /*repeat on touch*/
    CTP_PEN_LONGTAP_HOLD,                   /*longtap hold*/
    CTP_PEN_REPEAT_HOLD,                    /*repeat hold */
    CTP_PEN_ABORT                           /*when buff is full, the data is not reliable*/
} bsp_ctp_event_status_t;

typedef struct {
    bsp_ctp_event_status_t event;           /* touch panle event*/
    uint16_t x;                             /* coordinate x*/
    uint16_t y;                             /* coordinate y*/
} bsp_ctp_single_event_t;

typedef struct {
    uint32_t         time_stamp;            /* indicate when this event to be get*/
    uint8_t          model;                 /* multiple point ,single,dual,triple,four,five,all gesture */
    uint8_t          padding;               /* data pattern check, default 0xaa*/
    bsp_ctp_single_event_t points[5];       /* points */
} bsp_ctp_multiple_event_t;

/** @brief This enum define ctp status */
typedef enum {
    BSP_CTP_ERROR             = -2,         /**< This value means a ctp function EEROR */
    BSP_CTP_INVALID_PARAMETER = -1,         /**< This value means an invalid parameter */
    BSP_CTP_OK                = 0,          /**< This value meeas no error happen during the function call*/
    BSP_CTP_EVENT_EMPTY       = 1
} bsp_ctp_status_t;

typedef struct {
    uint32_t pen_longtap_time;              /**< This value means a ctp longtap time milliseconds */
    uint32_t pen_repeat_time;               /**< This value means a ctp repeat time milliseconds */
} bsp_ctp_event_time_t;

typedef struct {
    uint32_t high_sample_period;            /**< This value means set high speed sample rate, unit is milliseconds */
    uint32_t low_sample_period;             /**< This value means set low speed sample rate, unit is milliseconds */
    bool     is_used_hardware_polling;      /**< If this value is true, when the ctp interrupt trigger, driver will mask eint and polling the ctp hardware by setting time
                                                 If this value is false, driver will get data from ctp hardware by eint trigger, and the settig time means polling to check the data*/
} bsp_ctp_gpt_sample_period_t;

/** initialzie CTP module**/
bsp_ctp_status_t bsp_ctp_init(void);

/** deinitialize CTP mdule **/
bsp_ctp_status_t bsp_ctp_deinit(void);

/** get touch event value**/
bsp_ctp_status_t bsp_ctp_get_event_data(bsp_ctp_multiple_event_t *touch_event);

/** register user callback function ***/
bsp_ctp_status_t bsp_ctp_register_callback(bsp_ctp_callback_t ctp_callback, void *user_data);

/** reset buffer ***/
bsp_ctp_status_t bsp_ctp_flush_buffer(void);

/** set repeat and longtap time ***/
bsp_ctp_status_t bsp_ctp_set_event_time(bsp_ctp_event_time_t *event_time);

/** set high and low sample period ***/
bsp_ctp_status_t bsp_ctp_set_sample_period(bsp_ctp_gpt_sample_period_t *sample_period);

/** set CTP sleep or power on ***/
bsp_ctp_status_t bsp_ctp_power_on(bool on);

/** Set to true force upgrade, Set to false only different upgrade */
bsp_ctp_status_t bsp_ctp_force_upgrade(bool is_force_upgrade);


#ifdef __cplusplus
}
#endif

#endif /*MTK_CTP_ENABLE*/
#endif /* __BSP_CTP_H__ */


