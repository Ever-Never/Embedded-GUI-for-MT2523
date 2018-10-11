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

#include "hal_adc.h"

#ifdef HAL_ADC_MODULE_ENABLED

#include "hal_clock.h"
#include "hal_log.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_pmu_internal.h"


#ifdef __cplusplus
extern "C" {
#endif

#define ADC_BUSY 1
#define ADC_IDLE 0
#ifndef ABBA_TOP_CON0
#define ABBA_TOP_CON0       0xA21C0028
#endif
#ifndef RG_ABB_LVSH_EN
#define RG_ABB_LVSH_EN      0x8000
#endif

volatile uint8_t g_adc_status = ADC_IDLE;

hal_adc_status_t hal_adc_init(void)
{
    uint32_t irq_flag;

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    irq_flag = save_and_set_interrupt_mask();

    /* Check module status */
    if (g_adc_status == ADC_BUSY) {
        log_hal_error("\r\n [ADC] Module is busy!");

        /* Restore the previous status of interrupt */
        restore_interrupt_mask(irq_flag);

        return HAL_ADC_STATUS_ERROR_BUSY;
    } else {
        /* Change status to busy */
        g_adc_status = ADC_BUSY;

        /* Restore the previous status of interrupt */
        restore_interrupt_mask(irq_flag);
    }

    /* Enable VA28_LDO */
    pmu_ctrl_driver_power(DRIVER_ADC, 1);

    /* Set LVSH in order to disable bypass of anolog output to digital part */
    *(volatile uint32_t *)(ABBA_TOP_CON0) |= RG_ABB_LVSH_EN;

    /*Enable clock: *(volatile uint32_t *)(0xA21D0000) |= 0x04; */
    if (HAL_CLOCK_STATUS_OK != hal_clock_enable(HAL_CLOCK_CG_AUXADC)) {
        log_hal_error("\r\n [ADC] Clock enable failed!");
        return HAL_ADC_STATUS_ERROR;
    }

    /* Set ADC related registers to default value */
    ADC->AUXADC_CON1 = 0;
    ADC->AUXADC_CON3 = 0x10;

    /* Software reset ADC */
    ADC->AUXADC_CON3 |= AUXADC_CON3_SOFT_RST_MASK;
    ADC->AUXADC_CON3 &= ~AUXADC_CON3_SOFT_RST_MASK;

    return HAL_ADC_STATUS_OK;
}

hal_adc_status_t hal_adc_deinit(void)
{
    /* Set ADC related registers to default value */
    ADC->AUXADC_CON1 = 0;
    ADC->AUXADC_CON3 = 0x10;

    /* Disable clock */
    if (HAL_CLOCK_STATUS_OK != hal_clock_disable(HAL_CLOCK_CG_AUXADC)) {
        log_hal_error("\r\n [ADC] Clock disable failed!");
        return HAL_ADC_STATUS_ERROR;
    }

    /* Set LVSH in order to bypass anolog output to digital part */
    //*(volatile uint32_t *)(ABBA_TOP_CON0) &= ~RG_ABB_LVSH_EN;

    /* Disable VA28_LDO */
    pmu_ctrl_driver_power(DRIVER_ADC, 0);

    /* Change status to idle */
    g_adc_status = ADC_IDLE;

    return HAL_ADC_STATUS_OK;
}

hal_adc_status_t hal_adc_get_data_polling(hal_adc_channel_t channel, uint32_t *data)
{
    /* Channel is invalid */
    if (channel < HAL_ADC_CHANNEL_11 || channel >= HAL_ADC_CHANNEL_MAX) {
        if ((channel != HAL_ADC_CHANNEL_7) && (channel != HAL_ADC_CHANNEL_8)) {
            log_hal_error("\r\n [ADC] Invalid channel: %d.", channel);
            return HAL_ADC_STATUS_ERROR_CHANNEL;
        }
    }

    /* Parameter check */
    if (data == NULL) {
        log_hal_error("\r\n [ADC] Invalid parameter.");
        return HAL_ADC_STATUS_INVALID_PARAMETER;
    }

    /* Disable the corresponding region */
    ADC->AUXADC_CON1 &= ~(1 << (uint16_t)channel);
    ADC->AUXADC_CON1 |= (1 << (uint16_t)channel);

    /* Wait until the module status is idle */
    while (ADC->AUXADC_CON3 & AUXADC_CON3_AUXADC_STA_MASK);

    /* Retrieve data for corresponding channel */
    switch (channel) {
        case HAL_ADC_CHANNEL_7:
            *data = ADC->AUXADC_DAT7;
            break;
        case HAL_ADC_CHANNEL_8:
            *data = ADC->AUXADC_DAT8;
            break;
        case HAL_ADC_CHANNEL_11:
            *data = ADC->AUXADC_DAT11;
            break;
        case HAL_ADC_CHANNEL_12:
            *data = ADC->AUXADC_DAT12;
            break;
        case HAL_ADC_CHANNEL_13:
            *data = ADC->AUXADC_DAT13;
            break;
        case HAL_ADC_CHANNEL_14:
            *data = ADC->AUXADC_DAT14;
            break;
        case HAL_ADC_CHANNEL_15:
            *data = ADC->AUXADC_DAT15;
            break;
        default:
            /* Should not run here */
            break;
    }

    return HAL_ADC_STATUS_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_ADC_MODULE_ENABLED */

