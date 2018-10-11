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

#include "hal_accdet.h"

#ifdef HAL_ACCDET_MODULE_ENABLED

#include "hal_accdet_internal.h"
#include "hal_log.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"


#ifdef __cplusplus
extern "C" {
#endif

#define PMU_ACCDET_CON1     0xA21A0064
#define RG_ACDET_PULLOW     0x04
#ifndef ABBA_TOP_CON0
#define ABBA_TOP_CON0       0xA21C0028
#endif
#ifndef RG_ABB_LVSH_EN
#define RG_ABB_LVSH_EN      0x8000
#endif

volatile uint8_t g_accdet_status = ACCDET_IDLE;
volatile hal_accdet_callback_event_t g_accdet_last_event;
accdet_callback_t g_accdet_call_back = {
    NULL, NULL
};

#ifdef ACCDET_EINT_SUPPORT
volatile bool g_accdet_is_plug_in;
#endif

hal_accdet_status_t hal_accdet_init(void)
{
    uint32_t irq_flag;

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    irq_flag = save_and_set_interrupt_mask();

    /* Check module status */
    if (g_accdet_status == ACCDET_BUSY) {
        log_hal_error("\r\n [ACCDET] Module is busy!");

        /* Restore the previous status of interrupt */
        restore_interrupt_mask(irq_flag);

        return HAL_ACCDET_STATUS_ERROR_BUSY;
    } else {
        g_accdet_status = ACCDET_BUSY;

        /* Restore the previous status of interrupt */
        restore_interrupt_mask(irq_flag);
    }

    /* Enable VA28_LDO */
    pmu_ctrl_driver_power(DRIVER_ACCDET, 1);

    /* Set LVSH in order to disable bypass of anolog output to digital part */
    *(volatile uint32_t *)(ABBA_TOP_CON0) |= RG_ABB_LVSH_EN;

    /* Init global flags */
#ifdef ACCDET_EINT_SUPPORT
    g_accdet_is_plug_in = false;
#endif
    g_accdet_last_event = HAL_ACCDET_EVENT_PLUG_OUT;

    /* Software reset ACCDET */
    ACCDET->ACCDET_RSTB &= ~ACCDET_RSTB_RSTB_MASK;
    ACCDET->ACCDET_RSTB |= ACCDET_RSTB_RSTB_MASK;

    /* Configure PWM */
    ACCDET->ACCDET_PWM_WIDTH = HAL_ACCDET_PWM_WIDTH;
    ACCDET->ACCDET_PWM_THRESH = HAL_ACCDET_PWM_THRESHOLD;
    ACCDET->ACCDET_EN_DELAY_NUM = HAL_ACCDET_PWM_DELAY;

    /* Enables MICBIAS, voltage threshold and comparator unit of ACCDET */
    ACCDET->ACCDET_STATE_SWCTRL |= (ACCDET_STATE_SWCTRL_CMP_PWM_EN_MASK | ACCDET_STATE_SWCTRL_VTH_PWM_EN_MASK \
                                    | ACCDET_STATE_SWCTRL_MBIAS_PWM_EN_MASK);

    /* Pull low MICBIAS when MICBIAS is off */
    *((volatile uint32_t *)(PMU_ACCDET_CON1)) |= RG_ACDET_PULLOW;

    return HAL_ACCDET_STATUS_OK;
}

hal_accdet_status_t hal_accdet_deinit(void)
{
    /* Set ACCDET PWM related registers to default value */
    ACCDET->ACCDET_PWM_WIDTH = 0;
    ACCDET->ACCDET_PWM_THRESH = 0;
    ACCDET->ACCDET_EN_DELAY_NUM = 0x0101;

    /* Set ACCDET debounce related registers to default value */
    ACCDET->ACCDET_DEBOUNCE0 = 0x0010;
    ACCDET->ACCDET_DEBOUNCE1 = 0x0010;
    ACCDET->ACCDET_DEBOUNCE3 = 0x0010;

    /* Disables MICBIAS, voltage threshold and comparator unit of ACCDET */
    ACCDET->ACCDET_STATE_SWCTRL &= ~(ACCDET_STATE_SWCTRL_CMP_PWM_EN_MASK | ACCDET_STATE_SWCTRL_VTH_PWM_EN_MASK \
                                     | ACCDET_STATE_SWCTRL_MBIAS_PWM_EN_MASK);

    /* unregister irq callback */
    accdet_irq_callback_unregister();

    /* Disable VA28_LDO */
    pmu_ctrl_driver_power(DRIVER_ACCDET, 0);

    /* Change status to idle */
    g_accdet_status = ACCDET_IDLE;

    return HAL_ACCDET_STATUS_OK;
}

hal_accdet_status_t hal_accdet_enable(void)
{
#ifdef ACCDET_EINT_SUPPORT
    /* Register EINT callback */
    accdet_eint_callback_register();
#else
    ACCDET->ACCDET_CTRL |= ACCDET_CTRL_ACCDET_EN_MASK;
#endif

    return HAL_ACCDET_STATUS_OK;
}

hal_accdet_status_t hal_accdet_disable(void)
{
#ifdef ACCDET_EINT_SUPPORT
    /* unregister EINT callback */
    accdet_eint_callback_unregister();
#else
    ACCDET->ACCDET_CTRL &= ~ACCDET_CTRL_ACCDET_EN_MASK;
#endif

    return HAL_ACCDET_STATUS_OK;
}

hal_accdet_status_t hal_accdet_set_debounce_time(const hal_accdet_debounce_time_t *debounce_time)
{
    /* Parameter check */
    if (debounce_time == NULL) {
        log_hal_error("\r\n [ACCDET] Invalid parameter.");
        return HAL_ACCDET_STATUS_INVALID_PARAMETER;
    }

    /* Set debounce time for hook-key, plug-in and plug-out, unit of debounce time is ms */
    ACCDET->ACCDET_DEBOUNCE0 = debounce_time->accdet_hook_key_debounce * 33;
    ACCDET->ACCDET_DEBOUNCE1 = debounce_time->accdet_plug_in_debounce * 33;
    ACCDET->ACCDET_DEBOUNCE3 = debounce_time->accdet_plug_out_debounce * 33;

    return HAL_ACCDET_STATUS_OK;
}

hal_accdet_status_t hal_accdet_register_callback(hal_accdet_callback_t accdet_callback, void *user_data)
{
    /* Parameter check */
    if (accdet_callback == NULL) {
        log_hal_error("\r\n [ACCDET] Invalid parameter.");
        return HAL_ACCDET_STATUS_INVALID_PARAMETER;
    }

    /* Update callback function and parameter */
    g_accdet_call_back.func = accdet_callback;
    g_accdet_call_back.arg = user_data;

    /* Register irq callback */
    accdet_irq_callback_register();

    return HAL_ACCDET_STATUS_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_ACCDET_MODULE_ENABLED */

