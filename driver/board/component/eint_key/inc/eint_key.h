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

#ifndef _EINT_KEY_H_
#define _EINT_KEY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "hal_gpio.h"
#include "hal_eint.h"



typedef struct {
    hal_gpio_pin_t    gpio_port;    /*GPIO port*/
    uint8_t           eint_mode;    /*GPIO eint mode*/
    hal_eint_number_t eint_number;  /*eint number*/
    uint8_t           key_data;     /*keydata define by user*/
} bsp_eint_key_mapping_t;

typedef struct {
    uint32_t    longpress_time;     /*longpress time setting*/
    uint32_t    repeat_time;        /*repeat time setting*/
} bsp_eint_key_config_t;

/** This enum defines keypad event status */
typedef enum {
    BSP_EINT_KEY_RELEASE        = 0,        /**< A key is released. */
    BSP_EINT_KEY_PRESS          = 1,        /**< A key is pressed. */
    BSP_EINT_KEY_LONG_PRESS     = 2,        /**< A key is long pressed. */
    BSP_EINT_KEY_REPEAT         = 3,        /**< A key is repeat pressed. */
} bsp_eint_key_event_t;

typedef void (*bsp_eint_key_callback_t)(bsp_eint_key_event_t event, uint8_t key_data, void *user_data);


bool bsp_eint_key_init(bsp_eint_key_config_t *config);
bool bsp_eint_key_register_callback(bsp_eint_key_callback_t callback, void *user_data);
bool bsp_eint_key_set_event_time(bsp_eint_key_config_t *config);
bool bsp_eint_key_set_debounce_time(uint32_t debounce_time);
bool bsp_eint_key_enable(void);


#ifdef __cplusplus
}
#endif

#endif /* _EINT_KEY_CUSTOM_H_ */

