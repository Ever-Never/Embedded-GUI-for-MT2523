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
#include "mt2523.h"
#include "hal_wdt_retention.h"



static uint8_t retention_flag_magic[6] = {0x2E, 0x4E, 0x6E, 0x8E, 0xCE, 0xEE};

void wdt_set_retention_flag(WDT_RETENTION_FLAG_USER_ID_T user_id, uint32_t register_number, uint32_t bit_offset, bool value)
{
    uint32_t register_value;

    if (user_id <= WDT_RETENTION_FLAG_USER_START || user_id >= WDT_RETENTION_FLAG_USER_END) {
        return;
    }
    if ((register_number >= WDT_RETENTION_FLAG_NUMBER) || (bit_offset >= WDT_RETENTION_FLAG_BITS)) {
        return;
    }

    /* read */
    register_value = WDT_RETENTION_FLAG_REGISTER->RETN_FLAG[register_number];

    /* modify */
    if (0 == value) {
        register_value &= ~(0x1 << bit_offset) ;
    } else {
        register_value |= (0x1 << bit_offset) ;
    }

    register_value &= 0x00FFFFFF;
    register_value |= (retention_flag_magic[register_number] << WDT_RETENTION_FLAG_MAGIC_BIT_OFFSET);

    /* write */
    WDT_RETENTION_FLAG_REGISTER->RETN_FLAG[register_number] = register_value;


}


void wdt_set_retention_data(WDT_RETENTION_DATA_USER_ID_T user_id, uint32_t register_number, uint32_t value)
{
    if (user_id <= WDT_RETENTION_DATA_USER_START || user_id >= WDT_RETENTION_DATA_USER_END) {
        return;
    }
    if (register_number >= WDT_RETENTION_DATA_NUMBER) {
        return;
    }
    WDT_RETENTION_DATA_REGISTER->RETN_DATA[register_number] = value;
}

int8_t wdt_read_retention_flag(uint32_t register_number, uint32_t bit_offset)
{
    if ((register_number >= WDT_RETENTION_FLAG_NUMBER) || (bit_offset >= WDT_RETENTION_FLAG_BITS)) {
        return -1;
    }

    return (WDT_RETENTION_FLAG_REGISTER->RETN_FLAG[register_number] >> bit_offset) & 0x1;
}

int8_t wdt_read_retention_data(uint32_t register_number)
{
    if (register_number >= WDT_RETENTION_DATA_NUMBER) {
        return -1;
    }

    return WDT_RETENTION_DATA_REGISTER->RETN_DATA[register_number];
}

#endif
