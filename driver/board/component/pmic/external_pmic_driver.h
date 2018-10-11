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

#ifndef __EXTERNAL_PMIC_DRIVER_H__
#define __EXTERNAL_PMIC_DRIVER_H__

#include <stdio.h>
#include <stdlib.h>
#include "hal_platform.h"

typedef enum {
    EXT_PMIC_STATUS_INVALID_PARAMETER  = -2, 		/**< PMIC error invalid parameter */
    EXT_PMIC_STATUS_ERROR              = -1,     	/**< PMIC undefined error */
    EXT_PMIC_STATUS_SUCCESS            = 0       	/**< PMIC function ok */
} external_pmic_status_t;

typedef enum {
    EXT_PMIC_VR_DISABLE   = 0,
    EXT_PMIC_VR_ENABLE    = 1
} external_pmic_vr_ctl_t;

typedef enum {
    EXT_PMIC_VCORE_UNLOCK = 0,
    EXT_PMIC_VCORE_LOCK = 1
} external_pmic_vcore_lock_ctrl_t;

typedef enum {
    EXT_PMIC_VOL_0P7V = 0,
    EXT_PMIC_VOL_0P8V = 1,
    EXT_PMIC_VOL_0P9V = 2,
    EXT_PMIC_VOL_1P0V = 3,
    EXT_PMIC_VOL_1P1V = 4,
    EXT_PMIC_VOL_1P2V = 5,
    EXT_PMIC_VOL_1P3V = 6,
    EXT_PMIC_VOL_1P4V = 7,
    EXT_PMIC_VOL_1P5V = 8,
    EXT_PMIC_VOL_1P6V = 9,
    EXT_PMIC_VOL_1P7V = 10,
    EXT_PMIC_VOL_1P8V = 11,
    EXT_PMIC_VOL_1P9V = 12,
    EXT_PMIC_VOL_2P0V = 13,
    EXT_PMIC_VOL_2P1V = 14,
    EXT_PMIC_VOL_2P2V = 15,
    EXT_PMIC_VOL_2P3V = 16,
    EXT_PMIC_VOL_2P4V = 17,
    EXT_PMIC_VOL_2P5V = 18,
    EXT_PMIC_VOL_2P6V = 19,
    EXT_PMIC_VOL_2P7V = 20,
    EXT_PMIC_VOL_2P8V = 21,
    EXT_PMIC_VOL_2P9V = 22,
    EXT_PMIC_VOL_3P0V = 23,
    EXT_PMIC_VOL_3P1V = 24,
    EXT_PMIC_VOL_3P2V = 25,
    EXT_PMIC_VOL_3P3V = 26,
    EXT_PMIC_VOL_3P4V = 27,
    EXT_PMIC_VOL_ERROR = 255
} external_pmic_voltage_t;

void external_pmic_i2c_init(void);
bool external_pmic_set_register_value(unsigned char i2c_address, unsigned char address, unsigned char mask, unsigned char shift, unsigned char value);
unsigned char external_pmic_get_register_value(unsigned char i2c_address, unsigned char address, unsigned char mask, unsigned char shift);

#endif
