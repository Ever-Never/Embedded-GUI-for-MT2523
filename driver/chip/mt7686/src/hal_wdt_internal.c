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

#include "hal_wdt.h"

#if defined(HAL_WDT_MODULE_ENABLED)
#include "hal_wdt_internal.h"

void wdt_set_length(uint32_t seconds)
{
    uint32_t length_register_value = 0;
    /* trasfer seconds to register value */
    length_register_value = (seconds * 10000) / WDT_1_TICK_LENGTH;
    length_register_value <<= WDT_STANDARD_16_OFFSET;
    length_register_value |= WDT_LENGTH_KEY;

    /* write the length register */
    WDT_REGISTER->WDT_LENGTH = length_register_value;

    /*restart WDT to let the new value take effect */
    WDT_REGISTER->WDT_SW_RESTART = WDT_SW_RESTART_KEY;
}

void wdt_set_mode(uint32_t value)
{
    if (HAL_WDT_MODE_INTERRUPT == value) {
        WDT_REGISTER->WDT_IE = (WDT_STANDARD_1_MASK | WDT_IE_KEY);
    } else {
        WDT_REGISTER->WDT_IE = WDT_IE_KEY;
    }
}

void wdt_set_restart(void)
{
    WDT_REGISTER->WDT_SW_RESTART = WDT_SW_RESTART_KEY;
}

void wdt_set_sw_rst(void)
{
    WDT_REGISTER->WDT_SW_RST = WDT_SW_RST_KEY;
}

void wdt_set_enable(uint32_t enable)
{
    if (0 != enable) {
        WDT_REGISTER->WDT_EN = (WDT_STANDARD_1_MASK | WDT_EN_KEY);
        WDT_REGISTER->WDT_AUTO_RESTART_EN = (WDT_STANDARD_1_MASK | WDT_AUTO_RESTART_EN_KEY);
    } else {
        WDT_REGISTER->WDT_EN = WDT_EN_KEY;
        WDT_REGISTER->WDT_AUTO_RESTART_EN = WDT_AUTO_RESTART_EN_KEY;
    }
}

uint32_t wdt_get_reset_status(void)
{
    uint32_t status_register_value = 0;

    status_register_value = WDT_REGISTER->WDT_STA;

    if (0 != (status_register_value & WDT_STA_HW_WDT_MASK)) {
        return HAL_WDT_TIMEOUT_RESET;
    } else if (0 != (status_register_value & WDT_STA_SW_WDT_MASK)) {
        return HAL_WDT_SOFTWARE_RESET;
    }

    return HAL_WDT_NONE_RESET;
}

uint32_t wdt_get_enable_status(void)
{
    return (WDT_REGISTER->WDT_EN >> WDT_STANDARD_1_OFFSET);
}

uint32_t wdt_get_mode_status(void)
{
    return (WDT_REGISTER->WDT_IE >> WDT_STANDARD_1_OFFSET);
}

void wdt_clear_irq(void)
{
    uint32_t status = 0;
    status = WDT_REGISTER->WDT_INT;
    status = status;
}

#endif

