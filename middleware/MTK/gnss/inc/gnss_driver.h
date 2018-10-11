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

#ifndef GNSS_UART_H
#define GNSS_UART_H

#if defined(_MSC_VER)
#include "maui_adapter.h"
#else
#include <stdint.h>
#endif
#include "hal_platform.h"
#include "hal_gpio.h"

typedef enum gnss_uart_callback_type_s {
    GNSS_UART_CALLBACK_TYPE_CAN_READ,
    GNSS_UART_CALLBACK_TYPE_CAN_WRITE,
    GNSS_UART_CALLBACK_TYPE_WAKEUP
} gnss_uart_callback_type_t;

typedef void (*gnss_driver_uart_callback)(gnss_uart_callback_type_t type);

extern uint8_t gnss_driver_init(gnss_driver_uart_callback callback_function);

extern int32_t gnss_driver_uart_read(uint8_t port, int8_t *buffer, int32_t length);

extern int32_t gnss_driver_uart_write(uint8_t port, int8_t *buffer, int32_t length);

extern void gnss_driver_uart_deinit(uint8_t port);

extern void gnss_driver_power_on(void);

extern void gnss_driver_power_off(void);

#ifdef __ICCARM__
__weak void gnss_driver_init_coclock(void);
__weak void gnss_driver_deinit_coclock(void);
#else
__attribute__((weak)) void gnss_driver_init_coclock(void);
__attribute__((weak)) void gnss_driver_deinit_coclock(void);
#endif

extern hal_gpio_pin_t gnss_hrst;
extern hal_gpio_pin_t gnss_ldo_en;
extern hal_gpio_clock_t gnss_32k_clock;
extern hal_gpio_pin_t gnss_clock_pin;
extern hal_eint_number_t gnss_eint;
extern hal_uart_port_t gnss_uart;
extern void cust_gnss_gpio_config(void);

#endif /*GNSS_UART_H*/

