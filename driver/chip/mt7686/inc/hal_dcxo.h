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

#include <stdio.h>
#include <stdlib.h>
#include "hal_platform.h"
#ifdef HAL_DCXO_MODULE_ENABLED
/*
* xtal define
*/
//#define XO_3225
#define XO_2520  /*EVB default*/
//#define XO_2016

typedef enum {
    xtal_type_2520,
    xtal_type_3225,
    xtal_type_2016,
} hal_xtal_type_t;

typedef enum {
    freq_40M=0,
    freq_26M=1,
} hal_xtal_freq_t;

#define CAPID2 0x80000000
#define CAPID1   0x800000
#define CAPID0     0x8000
void hal_dcxo_init(void);
void hal_dcxo_counter_start(void);
void hal_dcxo_counter_end(void);
bool hal_dcxo_is_32kless(void);
void hal_dcxo_capid_init(void);
void hal_dcxo_load_calibration(void);
void hal_dcxo_factory_calibration(void);
void dcxo_set_register_value(uint32_t address, short int mask, short int shift, short int value);
void dcxo_calbration_efuse(void);
void dcxo_set_xtal_lpm_status(uint8_t xosc32_enable);
uint32_t dcxo_get_register_value(uint32_t address, short int mask, short int shift);

#define SW_EFSUE_SIMULATION_VALUE 0xb2
//#define SW_EFSUE_SIMULATION Ccalbration by sw setting

#define DCXO_EFUSE_BIT_MASK                              0x7F
#define XTAL_CTL_XO_POWERON_TEMP_MASK                    0xFFFFF
#define XTAL_CTL_XO_POWERON_TEMP_SHIFT                   0

#define XTAL_CTL16_XO_POR_STEP_TIME_MASK                 0x7F
#define XTAL_CTL16_XO_POR_STEP_TIME_SHIFT                0
#define XTAL_CTL16_XO_RDY_STABLE_TIME_MASK               0xFF
#define XTAL_CTL16_XO_RDY_STABLE_TIME_SHIFT              8
#define XTAL_CTL16_XO_PREBUF_STABLE_TIME_MASK            0x7F
#define XTAL_CTL16_XO_PREBUF_STABLE_TIME_SHIFT           16
#define XTAL_CTL16_XO_DIGPREBUF_STABLE_TIME_MASK         0x7F
#define XTAL_CTL16_XO_DIGPREBUF_STABLE_TIME_SHIFT        24

#define XTAL_CTL17_XO_BIAS_START_STABLE_TIME_MASK        0x7F
#define XTAL_CTL17_XO_BIAS_START_STABLE_TIME_SHIFT       0
#define XTAL_CTL17_XO_RESET_HIGH_STABLE_TIME_MASK        0x7F
#define XTAL_CTL17_XO_RESET_HIGH_STABLE_TIME_SHIFT       8
#define XTAL_CTL17_XO_RESET_LOW_STABLE_TIME_MASK         0x7F
#define XTAL_CTL17_XO_RESET_LOW_STABLE_TIME_SHIFT        16

//#define XTAL_CTRL_BASE       0xA2060000 /*26M, RF Xtal control register*/
#define XTAL_CTL0                         ((volatile uint32_t*)0xA2060000)
#define XTAL_CTL1                         ((volatile uint32_t*)0xA2060004)
#define XTAL_CTL2                         ((volatile uint32_t*)0xA2060008)
#define XTAL_CTL3                         ((volatile uint32_t*)0xA206000C)

#define XTAL_CTL4                         ((volatile uint32_t *)(0xA2060010))
#define XTAL_CTL5                         ((volatile uint32_t *)(0xA2060014))
#define XTAL_CTL6                         ((volatile uint32_t *)(0xA2060018))
#define XTAL_CTL7                         ((volatile uint32_t *)(0xA206001C))

#define XTAL_CTL8                         ((volatile uint32_t *)(0xA2060020))
#define XTAL_CTL9                         ((volatile uint32_t *)(0xA2060024))
#define XTAL_CTL10                        ((volatile uint32_t *)(0xA2060028))
#define XTAL_CTL11                        ((volatile uint32_t *)(0xA206002C))

#define XTAL_CTL13                        ((volatile uint32_t *)(0xA2060034))
#define XTAL_CTL14                        ((volatile uint32_t *)(0xA2060038))
#define XTAL_CTL15                        ((volatile uint32_t *)(0xA206003C))

#define XTAL_CTL16                        ((volatile uint32_t *)(0xA2060040))
#define XTAL_CTL17                        ((volatile uint32_t *)(0xA2060044))
#define XTAL_CTL18                        ((volatile uint32_t *)(0xA2060048))
#define XTAL_CTL19                        ((volatile uint32_t *)(0xA206004C))
#define XTAL_CTL20                        ((volatile uint32_t *)(0xA2060050))

/* DCXO CAPID operation*/
#define BL_EFUSE_CAPID                    ((volatile uint32_t *)(0xA20A0180))
#define BL_EFUSE_MAGIC                    ((volatile uint32_t *)(0xA20A0008))
#define BL_EFUSE_MAGIC_CODE               ((volatile uint32_t *)(0xA07923B6))
#define BL_EFUSE_REFRESH                  ((volatile uint32_t *)(0xA20A0000))
#define BL_DCXO_CAP_ID                    ((volatile uint32_t *)(0xA21E0080))

#define BL_EFUSE_CAPID0_MASK              ((volatile uint32_t *)(0x000001FF))
#define BL_EFUSE_CAPID1_MASK              ((volatile uint32_t *)(0x0003FE00))
#define BL_EFUSE_CAPID2_MASK              ((volatile uint32_t *)(0x07FC0000))
#define CKSYS_XTAL_FREQ_F_F_FXO_IS_26M    ((volatile uint32_t *)(0xA20202A3))
#define SYSTEM_INFOD                      ((volatile uint32_t *)(0xA2010040))

#endif /* HAL_DCXO_MODULE_ENABLED */
