/* Copyright Statement:
 *
 * (C) 2017-2017  MediaTek Inc. All rights reserved.
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

/*
    Module Name:
    connsys_log.c

    Abstract:
    Provide CONNSYS module logging wrapping interface to unify the print mechanisms.

    Revision History:
    Who         When            What
    --------    ----------      ----------------------------------------------
*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "memory_attribute.h"

#include "uart_slim_version.h"

#include "connsys_log.h"


ATTR_ZIDATA_IN_TCM static uint8_t _g_connsys_util_uart_slim_disable;


int connsys_util_uart_slim_printf(const char *format, ...)
{
    int len;
    char buf[256];

    if (_g_connsys_util_uart_slim_disable)
        return 0;

    va_list ap;

    va_start(ap, format);

    len = vsprintf(buf, format, ap);

    uart_init_boot_phase();
    uart_put_char_boot_phase((uint8_t *)buf, len);
    uart_deinit_boot_phase();

    va_end(ap);

    return len;
}


int connsys_util_uart_slim_dump(
    const char      level,
    const char      *msg,
    const char      *p,
    size_t          s
    )
{
    size_t          len;
    size_t          i = 0;

    if (_g_connsys_util_uart_slim_disable)
        return 0;

    uart_init_boot_phase();

    len = strlen(msg);

    /* description */

    connsys_util_uart_slim_printf("  ");
    uart_put_char_boot_phase((uint8_t *)msg, len);
    uart_put_char_boot_phase((uint8_t *)"\n", 1);

    /* bytes */

    while (i < s) {
        if ((i % 16) == 0)
            connsys_util_uart_slim_printf("  %04x  ", i);
        connsys_util_uart_slim_printf("%02x%s",
                                        p[i] & 0xFF,
                                        i == s - 1  ? "\n" :
                                        !(~i & 15)  ? "\n" :
                                        !(~i &  7)  ? " - " : " ");
        i++;
        len++;
    }

    uart_deinit_boot_phase();

    return len;
}


void connsys_util_uart_slim_enable(
    uint8_t         enable
    )
{
    _g_connsys_util_uart_slim_disable = !enable;
}


uint8_t connsys_util_uart_slim_status(
    void
    )
{
    return !_g_connsys_util_uart_slim_disable;
}
