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

#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#ifdef MTK_LED_ENABLE

#include "bsp_led_internal.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @brief This enum define API status of LED */
typedef enum {
    BSP_LED_ERROR             = -2,         /**< This value means a led function EEROR */
    BSP_LED_INVALID_PARAMETER = -1,         /**< This value means an invalid parameter */
    BSP_LED_OK                = 0           /**< This value meeas no error happen during the function call*/
} bsp_led_status_t;


/** @brief This enum defines state of LED */
typedef enum {
    BSP_LED_OFF  = 0,               /**< define led state of off */
    BSP_LED_ON = 1                  /**< define led state of on */
} bsp_led_state_t;

/** initialzie selected LED**/
bsp_led_status_t bsp_led_init(uint32_t led_number);


/** turn on the selected LED **/
bsp_led_status_t bsp_led_on(uint32_t led_number);


/** turn off the selected LED **/
bsp_led_status_t bsp_led_off(uint32_t led_number);


/** toggle the selected LED **/
bsp_led_status_t bsp_led_toggle(uint32_t led_number);


/* Return the current state of a selected LED */
bsp_led_status_t bsp_led_get_state(uint32_t led_number, bsp_led_state_t *led_state);


#ifdef __cplusplus
}
#endif

#endif /*MTK_LED_ENABLE*/
#endif /* __BSP_LED_H__ */


