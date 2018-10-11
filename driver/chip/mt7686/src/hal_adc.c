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
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
volatile static uint32_t s_macro_con2;
volatile static uint32_t s_ana_en_con;

void adc_backup_all_register(void);
void adc_restore_all_register(void);
#endif

#define ADC_BUSY 1
#define ADC_IDLE 0

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

#ifdef HAL_SLEEP_MANAGER_ENABLED
    sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_AUXADC, (sleep_management_suspend_callback_t)adc_backup_all_register, NULL);
    sleep_management_register_resume_callback(SLEEP_BACKUP_RESTORE_AUXADC, (sleep_management_resume_callback_t)adc_restore_all_register, NULL);
#endif

    /* Enable VA28_LDO */
    ADC->MACRO_CON2 |= MACRO_CON2_RG_AUXADC_LDO_EN_MASK;

    /* Enable AUXADC */
    ADC->ANA_EN_CON = ANA_EN_CON_AUXADC_EN_MASK;

    /*Enable clock: *(volatile uint32_t *)(0xA2030b20) |= 0x200000; */
    if (HAL_CLOCK_STATUS_OK != hal_clock_enable(HAL_CLOCK_CG_SW_AUXADC)) {
        log_hal_error("\r\n [ADC] Clock enable failed!");
        return HAL_ADC_STATUS_ERROR;
    }

    /* Software reset ADC */
    ADC->AUXADC_CON3_UNION.AUXADC_CON3_CELLS.SOFT_RST = AUXADC_CON3_SOFT_RST_MASK;
    ADC->AUXADC_CON3_UNION.AUXADC_CON3_CELLS.SOFT_RST = 0;

    return HAL_ADC_STATUS_OK;
}

hal_adc_status_t hal_adc_deinit(void)
{
    /* Disable clock: *(volatile uint32_t *)(0xA2030b10) |= 0x200000; */
    if (HAL_CLOCK_STATUS_OK != hal_clock_disable(HAL_CLOCK_CG_SW_AUXADC)) {
        log_hal_error("\r\n [ADC] Clock disable failed!");
        return HAL_ADC_STATUS_ERROR;
    }

    /* Disable AUXADC */
    ADC->ANA_EN_CON = 0;

    /* Disable VA28_LDO */
    ADC->MACRO_CON2 = 0;

    /* Change status to idle */
    g_adc_status = ADC_IDLE;

    return HAL_ADC_STATUS_OK;
}

hal_adc_status_t hal_adc_get_data_polling(hal_adc_channel_t channel, uint32_t *data)
{

    uint32_t times = 0;
    uint32_t temp_data = 0;;

    /* Channel is invalid */
    if (channel >= HAL_ADC_CHANNEL_MAX) {
        log_hal_error("\r\n [ADC] Invalid channel: %d.", channel);
        return HAL_ADC_STATUS_ERROR_CHANNEL;
    }

    /* Parameter check */
    if (data == NULL) {
        log_hal_error("\r\n [ADC] Invalid parameter.");
        return HAL_ADC_STATUS_INVALID_PARAMETER;
    }
    for (times = 0; times < 8; times++) {
        /* Disable the corresponding region */
        ADC->AUXADC_CON1 = 0;
        ADC->AUXADC_CON1 = (1 << (uint16_t)channel);

        /* Wait until the module status is idle */
        while (ADC->AUXADC_CON3_UNION.AUXADC_CON3_CELLS.ADC_STAT & AUXADC_CON3_ADC_STA_MASK);

        /* Retrieve data for corresponding channel */
        switch (channel) {
            case HAL_ADC_CHANNEL_0:
                temp_data += ADC->AUXADC_DATA0;
                break;
            case HAL_ADC_CHANNEL_1:
                temp_data += ADC->AUXADC_DATA1;
                break;
            case HAL_ADC_CHANNEL_2:
                temp_data += ADC->AUXADC_DATA2;
                break;
            case HAL_ADC_CHANNEL_3:
                temp_data += ADC->AUXADC_DATA3;
                break;
            default:
                /* Should not run here */
                break;
        }
    }
    *data = (temp_data >> 3);

    return HAL_ADC_STATUS_OK;
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
void adc_backup_all_register(void)
{
    s_macro_con2 = ADC->MACRO_CON2;
    s_ana_en_con = ADC->ANA_EN_CON;

}

void adc_restore_all_register(void)
{
    ADC->MACRO_CON2 = s_macro_con2;
    ADC->ANA_EN_CON = s_ana_en_con;

}
#endif

#ifdef __cplusplus
}
#endif

#endif /* HAL_ADC_MODULE_ENABLED */

