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

#ifndef __WDT_RETENTION_H__
#define __WDT_RETENTION_H__

#include <stdint.h>
#include <stdbool.h>



#define __IO volatile
#define WDT_RETENTION_FLAG_NUMBER  (6)
#define WDT_RETENTION_FLAG_BITS    (12)
#define WDT_RETENTION_FLAG_MAGIC_BIT_OFFSET   (24)


#define WDT_RETENTION_DATA_NUMBER   (5)




typedef enum {
    WDT_RETENTION_FLAG_USER_START = 0xD0,
    WDT_RETENTION_FLAG_USER_BROM = 0xD1,
    WDT_RETENTION_FLAG_USER_BL = 0xD2,
    WDT_RETENTION_FLAG_FOTA_GNSS = 0xD3,
    WDT_RETENTION_FLAG_USER_WARM_BOOT = 0xD4,
    WDT_RETENTION_FLAG_USER_LOW_POWER = 0xD5,
    WDT_RETENTION_FLAG_USER_FLASH_TOOL = 0xD6,
    WDT_RETENTION_FLAG_USER_END = 0xD7,
} WDT_RETENTION_FLAG_USER_ID_T;

typedef enum {
    WDT_RETENTION_DATA_USER_START = 0xE0,
    WDT_RETENTION_DATA_USER_BROM = 0xE1,
    WDT_RETENTION_DATA_USER_BL = 0xE2,
    WDT_RETENTION_DATA_USER_FLASH_TOOL = 0xE3,
    WDT_RETENTION_DATA_USER_END = 0xE4,
} WDT_RETENTION_DATA_USER_ID_T;


typedef struct {
    __IO uint32_t RETN_FLAG[WDT_RETENTION_FLAG_NUMBER];         /* WDT MODE register */
} WDT_RETENTION_FLAG_T;

typedef struct {
    __IO uint32_t RETN_DATA[WDT_RETENTION_DATA_NUMBER];     /* WDT INTERVAL register */
} WDT_RETENTION_DATA_T;

#define WDT_RETENTION_FLAG_REGISTER  ((WDT_RETENTION_FLAG_T *)(RGU_BASE+0x0800))
#define WDT_RETENTION_DATA_REGISTER  ((WDT_RETENTION_DATA_T *)(RGU_BASE+0x0820))


/**
 * @brief
 * @param[in] register_number is the number of register. from 0 ~ 5
 * @param[in] bit_offset is the register bit offset you want to set. from 0 ~ 11
 * @param[in] value is the value you want to set. the value is 0 or 1.
 * @return
 */
void wdt_set_retention_flag(WDT_RETENTION_FLAG_USER_ID_T user_id, uint32_t register_number, uint32_t bit_offset, bool value);


/**
 * @brief
 * @param[in] register_number is the number of register. from 0 ~ 5
 * @param[in] value is the value you want to set. the value is a 32bit value.
 * @return
 */
void wdt_set_retention_data(WDT_RETENTION_DATA_USER_ID_T user_id, uint32_t register_number, uint32_t value);


/**
 * @brief
 * @param[in] register_number is the number of register. from 0 ~ 5
 * @param[in] bit_offset is the register bit offset you want to set. from 0 ~ 11
 * @return    return the bit value. the value is 0 or 1. if return -1, it means register_number is wrong.
 */
int8_t wdt_read_retention_flag(uint32_t register_number, uint32_t bit_offset);


/**
 * @brief
 * @param[in] register_number is the number of register. from 0 ~ 5
 * @return    return the bit value. the value is a 32bit value. if return -1, it means register_number is wrong.
 */
int8_t wdt_read_retention_data(uint32_t register_number);



#endif
