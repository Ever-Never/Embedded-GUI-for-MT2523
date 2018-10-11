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
#include "hal_log.h"
#include "hal_gpt.h"





void wdt_set_length(uint32_t seconds)
{
    uint32_t length_register_value = 0;
    /* trasfer seconds to register value */
    length_register_value = (seconds * 10000) / WDT_1_TICK_LENGTH;
    length_register_value <<= WDT_LENGTH_KEY_OFFSET;
    length_register_value |= WDT_LENGTH_KEY;

    /* write the length register */
    WDT_REGISTER->WDT_LENGTH = length_register_value;

    /*restart WDT to let the new value take effect */
    WDT_REGISTER->WDT_RESTART = WDT_RESTART_KEY;
}

void wdt_set_mode_register_by_bit(uint32_t bit_offset, uint32_t value)
{
    uint32_t mode_register_value = 0;
    /* read */
    mode_register_value = WDT_REGISTER->WDT_MODE;
    /* modify */
    if (0 == value) {
        mode_register_value &= ~(0x1 << bit_offset) ;
    } else {
        mode_register_value |= (0x1 << bit_offset) ;
    }
    /*write*/
    mode_register_value &= 0x0000FFFF;
    mode_register_value |= (WDT_MODE_KEY << WDT_MODE_KEY_OFFSET);
    WDT_REGISTER->WDT_MODE = mode_register_value;

    /* delay 1 32K tick for hardware requirement */
    hal_gpt_delay_us(35);


}

uint32_t wdt_get_reset_status(void)
{
    uint32_t status_register_value = 0;

    status_register_value = WDT_REGISTER->WDT_STA;

    if (0 != (status_register_value & WDT_STATUS_TIME_OUT_MASK)) {
        return HAL_WDT_TIMEOUT_RESET;
    } else if (0 != (status_register_value & WDT_STATUS_SWRST_MASK)) {
        return HAL_WDT_SOFTWARE_RESET;
    }

    return HAL_WDT_NONE_RESET;
}


#endif
