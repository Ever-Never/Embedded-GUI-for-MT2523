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
#include "hal_pmu.h"
typedef enum {
    PMU_STATUS_INVALID_PARAMETER  = -1,     /**< pmu error invalid parameter */
    PMU_STATUS_ERROR              = 0,     /**< pmu undefined error */
    PMU_STATUS_SUCCESS            = 1       /**< pmu function ok */
} pmu_status_t;

typedef enum {
    PMU_STATUS_DISABLE            = 0,     /**< pmu undefined error */
    PMU_STATUS_ENABLE          = 1       /**< pmu function ok */
} pmu_control_status_t;

typedef enum{
    PMU_NORMAL=0,
    PMU_SLEEP,
    PMU_RETENTION,
    PMU_POWER_OFF
}hal_pmu_stage;

typedef enum{
    SLEEP_NORMAL=0,
    RETENTION_NORMAL
}hal_pmu_stage_flow;

typedef enum {
  PMU_FORCE_PFM       = 0,
  PMU_FORCE_PWM       = 1,
} pmu_force_mode_t;

typedef enum {
  PMU_VCORE_1p3V         = 0,
  PMU_VCORE_1p45v,
  PMU_VCORE_1p5v,
  PMU_VCORE_1p7v,
  PMU_VCORE_2p08v
} pmu_vcore_vosel_t;

typedef enum {
  PMU_CONTROL_OFF         = 0,
  PMU_CONTROL_ON,
} pmu_control_t;

typedef enum {
  PMU_RETENTION_LDO_OFF         = 0,
  PMU_RETENTION_LDO_ON,
} pmu_retention_status_t;

typedef struct{
    void (*init)(void);
    void (*init_power_mode)(void);
    void (*set_vcore_voltage)(hal_pmu_stage mode,pmu_vcore_vosel_t vol);
    void (*enter_sleep_mode)(void);
    void (*enter_retention_mode)(pmu_retention_status_t status);
    void (*resume_to_normal)(hal_pmu_stage_flow mode);
    void (*set_cldo_voltage)(int index);
    void (*set_mldo_voltage)(int adjust_value);
    void (*control_retention_lod)(pmu_control_t status);
    void (*sram_power_switch_control)(pmu_control_t status);
    void (*set_retention_ldo)(int adjust_value);
}hal_pmu_wrap_struct;
#endif /* HAL_PMU_MODULE_ENABLED */
#endif
