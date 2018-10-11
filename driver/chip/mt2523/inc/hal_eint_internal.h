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

#ifndef __HAL_EINT_INTERNAL_H__
#define __HAL_EINT_INTERNAL_H__

#ifdef HAL_EINT_MODULE_ENABLED
#include "hal_nvic.h"

typedef struct {
    void (*eint_callback)(void *user_data);
    void *user_data;
} eint_function_t;

typedef enum {
    EINT_MASK = 1,
    EINT_UNMASK = 2,
    EINT_INACTIVE = 3,
    EINT_ACTIVE = 4,
} eint_status_t;

typedef enum {
    EINT_NOT_INIT = 0,
    EINT_INIT = 1,
    EINT_DEINIT = 2,
} eint_driver_status_t;

typedef enum {
    EINT_DOMAIN_DISABLE = 0,
    EINT_DOMAIN_ENABLE  = 1
} eint_domain_status_t;

#define EINT_CON_PRESCALER_32KHZ   (0x00000000)
#define EINT_CON_PRESCALER_16KHZ   (0x00000001)
#define EINT_CON_PRESCALER_8KHZ    (0x00000002)
#define EINT_CON_PRESCALER_4KHZ    (0x00000003)
#define EINT_CON_PRESCALER_2KHZ    (0x00000004)
#define EINT_CON_PRESCALER_1KHZ    (0x00000005)
#define EINT_CON_PRESCALER_512HZ   (0x00000006)
#define EINT_CON_PRESCALER_256HZ   (0x00000007)
#define EINT_CON_DEBOUNCE_MASK     (0xFFFF7FFF)

#define EINT_WAKE_EVENT_EN         (0xFFFFFFFF)
#define EINT_WAKE_EVENT_DIS        (0xFFFFFFFF)

void eint_ack_interrupt(uint32_t eint_number);
hal_eint_status_t eint_mask_wakeup_source(hal_eint_number_t eint_number);
hal_eint_status_t eint_unmask_wakeup_source(hal_eint_number_t eint_number);

#endif /* __HAL_EINT_INTERNAL_H__ */

#endif /* HAL_EINT_MODULE_ENABLED */

