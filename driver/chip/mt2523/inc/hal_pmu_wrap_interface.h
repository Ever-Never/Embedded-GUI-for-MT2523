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

#ifndef __HAL_PMU_WRAP_INTERFACE_H__
#define __HAL_PMU_WRAP_INTERFACE_H__

#include "hal_platform.h"
#ifdef HAL_PMU_MODULE_ENABLED

typedef struct {
    void (*init)(void);
    bool (*set_reg_value)(unsigned short int address, unsigned short int mask, unsigned short int shift, unsigned short int value);
    unsigned char (*get_reg_value)(unsigned short int address, unsigned short int mask, unsigned short int shift);
    void (*ctrl_mtcmos)(void);
    unsigned char (*get_vcore_vol)(void);
    unsigned char (*set_vcore_vol)(unsigned char lock, unsigned char vcore);
    void (*set_vcore_s1_buck)(void);
    void (*ctrl_driver_power)(unsigned char driver_name, unsigned char enable);
    void (*audio_lock_voltage)(void);
    void (*audio_unlock_voltage)(void);
    unsigned char (*get_usb_input_status)(void);
    void (*enter_power_off)(void);
    void (*reset)(void);
} pmu_wrap_api_t;

typedef enum {
    PMIC_VOL_0P7V = 0,
    PMIC_VOL_0P8V = 1,
    PMIC_VOL_0P9V = 2,
    PMIC_VOL_1P0V = 3,
    PMIC_VOL_1P1V = 4,
    PMIC_VOL_1P2V = 5,
    PMIC_VOL_1P3V = 6,
    PMIC_VOL_1P4V = 7,
    PMIC_VOL_1P5V = 8,
    PMIC_VOL_1P6V = 9,
    PMIC_VOL_1P7V = 10,
    PMIC_VOL_1P8V = 11,
    PMIC_VOL_1P9V = 12,
    PMIC_VOL_2P0V = 13,
    PMIC_VOL_2P1V = 14,
    PMIC_VOL_2P2V = 15,
    PMIC_VOL_2P3V = 16,
    PMIC_VOL_2P4V = 17,
    PMIC_VOL_2P5V = 18,
    PMIC_VOL_2P6V = 19,
    PMIC_VOL_2P7V = 20,
    PMIC_VOL_2P8V = 21,
    PMIC_VOL_2P9V = 22,
    PMIC_VOL_3P0V = 23,
    PMIC_VOL_3P1V = 24,
    PMIC_VOL_3P2V = 25,
    PMIC_VOL_3P3V = 26,
    PMIC_VOL_ERROR = 255
} pmic_voltage_t;

typedef enum {
    DRIVER_DVFS,
    DRIVER_BT,
    DRIVER_AUDIO,
    DRIVER_USB,
    DRIVER_CAMERA,
    DRIVER_MSDC,
    DRIVER_VIBR,
    DRIVER_ACCDET,
    DRIVER_ADC,
    DRIVER_DAC
} platform_device_t;

typedef enum {
    PMIC_CTL_DISABLE   = 0,
    PMIC_CTL_ENABLE    = 1
} pmic_power_ctrl_t;

typedef enum {
    PMIC_VCORE_UNLOCK = 0,
    PMIC_VCORE_LOCK = 1
} pmic_vcore_lock_ctrl_t;

void pmu_init(void);
bool pmu_set_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift, unsigned short int value);
unsigned char pmu_get_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift);
void pmu_control_mtcmos(void);
unsigned char pmu_get_vcore_voltage(void);
unsigned char pmu_set_vcore_voltage(unsigned char lock, unsigned char vcore);
void pmu_set_vcore_s1_buck(void);
void pmu_ctrl_driver_power(unsigned char driver_name, unsigned char enable);
void pmu_audio_lock_voltage(void);
void pmu_audio_unlock_voltage(void);
unsigned char pmu_get_usb_input_status(void);
void pmu_enter_power_off(void);
void pmu_reset(void);
unsigned int pmu_get_type(void);

/*null function don't use*/
bool null_pmu_set_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift, unsigned short int value);
unsigned char null_pmu_get_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift);
void null_function(void);

#endif /* HAL_PMU_MODULE_ENABLED */
#endif
