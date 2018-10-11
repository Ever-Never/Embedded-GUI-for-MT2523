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

#include "hal_dvfs_internal.h"

#ifdef HAL_DVFS_MODULE_ENABLED

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "hal_flash_sf.h"
#include "hal_gpt.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "hal_pmu_internal.h"
#include "hal_pmu_wrap_interface.h"
#include "hal_emi_internal.h"
#include "hal_nvic_internal.h"

#define DVFS_MODULE_DEF(domain, mod, ...)
#define DVFS_VCORE_MODULE_GRP \
            DVFS_MODULE_DEF(VCORE, CM_CK0,       {208000, 104000,  26000}) /* CM, TCM */ \
            DVFS_MODULE_DEF(VCORE, MSDC_CK,      { 89140,  62400,  26000}) /* MSDC */

#undef DVFS_MODULE_DEF
#define DVFS_MODULE_DEF(domain, mod, ...) DVFS_##domain##_MODULE_##mod,
typedef enum {
    DVFS_VCORE_MODULE_GRP

    DVFS_VCORE_MODULE_NUM
} dvfs_vcore_module_t;

typedef enum {
    DVFS_VCORE_MODE_HIGH_SPEED = 0,
    DVFS_VCORE_MODE_FULL_SPEED,
    DVFS_VCORE_MODE_LOW_SPEED,

    DVFS_VCORE_MODE_NUM
} dvfs_vcore_mode_t;

typedef struct {
    pmic_voltage_t cur_volt;
    const pmic_voltage_t voltage[DVFS_VCORE_MODE_NUM];
    dvfs_opp_module_t module[DVFS_VCORE_MODULE_NUM];
} dvfs_vcore_opp_t;

typedef uint32_t dvfs_vcore_freq_t[DVFS_VCORE_MODE_NUM];

#undef DVFS_MODULE_DEF
#define DVFS_MODULE_DEF(domain, mod, ...) __VA_ARGS__,
static const uint32_t dvfs_vcore_mod_frequency[DVFS_VCORE_MODULE_NUM][DVFS_VCORE_MODE_NUM] = {
    DVFS_VCORE_MODULE_GRP
};

#undef DVFS_MODULE_DEF
#define DVFS_MODULE_DEF(domain, mod, ...) \
    {.name = __stringify(mod), \
     .notification =  NULL, \
     .frequency = &dvfs_vcore_mod_frequency[DVFS_##domain##_MODULE_##mod][0]},
static dvfs_vcore_opp_t dvfs_vcore_opp = {
    .voltage = {PMIC_VOL_1P3V, PMIC_VOL_1P1V, PMIC_VOL_0P9V},
    .module = {
        DVFS_VCORE_MODULE_GRP
    }
};

static const uint32_t dvfs_vcore_voltage[DVFS_VCORE_MODE_NUM] = {
    1300000, 1100000, 900000
};

static const uint32_t dvfs_vcore_frequency[DVFS_VCORE_MODE_NUM] = {
    208000, 104000, 26000
};

static dvfs_domain_t dvfs_vcore_domain;

static void dvfs_vcore_switch_voltage(struct _dvfs_domain_t *domain, unsigned int cur_opp, unsigned int next_opp)
{
    dvfs_vcore_opp_t *vcore_opp = (dvfs_vcore_opp_t *)domain->opp.data;

    assert(next_opp < domain->opp.opp_num);
    pmu_set_vcore_voltage(PMIC_VCORE_LOCK, vcore_opp->voltage[next_opp]);

    if (vcore_opp->cur_volt != PMIC_VOL_ERROR) {
        pmu_set_vcore_voltage(PMIC_VCORE_UNLOCK, vcore_opp->cur_volt);
    }

    vcore_opp->cur_volt = vcore_opp->voltage[next_opp];
}

extern uint32_t SysTick_Set(uint32_t ticks);

ATTR_TEXT_IN_TCM void dvfs_vcore_switch_frequency(struct _dvfs_domain_t *domain, unsigned int cur_opp, unsigned int next_opp)
{
    register unsigned int c_opp = cur_opp;
    register unsigned int n_opp = next_opp;

    hal_gpt_delay_us(1000);

    dvfs_enter_privileged_level();

    emi_mask_master();
    switch (n_opp) {
        case DVFS_VCORE_MODE_HIGH_SPEED:
            cm_bus_clk_208m();
            SF_DAL_DEV_SWITCH_TO_HIGH_FQ();
            break;
        case DVFS_VCORE_MODE_FULL_SPEED:
            cm_bus_clk_104m();
            SF_DAL_DEV_SWITCH_TO_HIGH_FQ();
            break;
        case DVFS_VCORE_MODE_LOW_SPEED:
            cm_bus_clk_26m();
            SF_DAL_DEV_SWITCH_TO_LOW_FQ();
            break;
        default:
            assert(0);
    }
    EMI_DynamicClockSwitch((n_opp < c_opp) ? EMI_CLK_LOW_TO_HIGH : EMI_CLK_HIGH_TO_LOW);
    emi_unmask_master();

    dvfs_exit_privileged_level();

    hal_gpt_delay_us(1000);

    SystemCoreClockUpdate();
    SysTick_Set(hal_clock_get_mcu_clock_frequency() / 1000);
    __ISB();
    __DSB();
}

static dvfs_opp_module_t *dvfs_vcore_get_next_module(struct _dvfs_domain_t *domain, dvfs_opp_module_t *module)
{
    dvfs_vcore_opp_t *vcore_opp = (dvfs_vcore_opp_t *)domain->opp.data;

    if (!module) {
        return (dvfs_opp_module_t *) & (vcore_opp->module[0]);
    }

    if (module >= &(vcore_opp->module[domain->opp.module_num - 1])) {
        return NULL;
    }

    return (dvfs_opp_module_t *)(&module[1]);
}

dvfs_domain_t *dvfs_vcore_domain_initialize(void)
{
    uint32_t cpufreq;

    memset(&dvfs_vcore_domain, 0, sizeof(dvfs_vcore_domain));
    dvfs_vcore_domain.name = __stringify(VCORE);
    dvfs_vcore_domain.opp.opp_num = DVFS_VCORE_MODE_NUM;
    dvfs_vcore_domain.opp.module_num = DVFS_VCORE_MODULE_NUM;
    dvfs_vcore_domain.opp.ops.switch_voltage = dvfs_vcore_switch_voltage;
    dvfs_vcore_domain.opp.ops.switch_frequency = dvfs_vcore_switch_frequency;
    dvfs_vcore_domain.opp.ops.get_next_module = dvfs_vcore_get_next_module;
    dvfs_vcore_domain.opp.frequency = dvfs_vcore_frequency;
    dvfs_vcore_domain.opp.voltage = dvfs_vcore_voltage;
    dvfs_vcore_domain.opp.data = (void *)&dvfs_vcore_opp;
    dvfs_vcore_opp.cur_volt = PMIC_VOL_ERROR;

    SystemCoreClockUpdate();
    cpufreq = SystemCoreClock / 1000;
    dvfs_vcore_domain.opp.cur_opp = dvfs_query_frequency(cpufreq, dvfs_vcore_domain.opp.frequency, dvfs_vcore_domain.opp.opp_num);
    assert((int)dvfs_vcore_domain.opp.cur_opp != DVFS_MODE_NONE);

    dvfs_vcore_domain.initialized = true;

    dvfs_register_domain(&dvfs_vcore_domain);

    return &dvfs_vcore_domain;
}

#endif /* HAL_DVFS_MODULE_ENABLED */

