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

#include "hal_dac.h"

#ifdef HAL_DAC_MODULE_ENABLED

#include "hal_clock.h"
#include "hal_log.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_pmu_internal.h"


#ifdef __cplusplus
extern "C" {
#endif

#define DAC_BUSY 1
#define DAC_IDLE 0
#define DAC_DATA_BIT_MASK      (0x3FF)
#define DAC_DATA_LENGTH_MASK   (0x80)

volatile uint8_t g_dac_status = DAC_IDLE;

hal_dac_status_t hal_dac_init(void)
{
    uint32_t irq_flag;

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    irq_flag = save_and_set_interrupt_mask();

    /* Check module status */
    if (g_dac_status == DAC_BUSY) {
        log_hal_error("\r\n [DAC] Module is busy!");

        /* Restore the previous status of interrupt */
        restore_interrupt_mask(irq_flag);

        return HAL_DAC_STATUS_ERROR_BUSY;
    } else {
        g_dac_status = DAC_BUSY;
        restore_interrupt_mask(irq_flag);
    }

    /* Enable VA28_LDO */
    pmu_ctrl_driver_power(DRIVER_DAC, 1);

    /*Enable clock: *(volatile uint32_t *)(0xA21D0000) |= 0x40; */
    if (HAL_CLOCK_STATUS_OK != hal_clock_enable(HAL_CLOCK_CG_GPDAC)) {
        log_hal_error("\r\n [DAC] Clock enable failed!");
        return HAL_DAC_STATUS_ERROR;
    }

    /* Software reset DAC, will reset all registers of GPDAC */
    DAC->SWRST |= SWRST_SWRST_MASK;
    DAC->SWRST &= ~SWRST_SWRST_MASK;

    /*Enable PDN of DAC */
    DAC->GPDAC_PDN |= GPDAC_PDN_GPDAC_PDN_MASK;

    return HAL_DAC_STATUS_OK;
}

hal_dac_status_t hal_dac_deinit(void)
{
    /* Disable PDN of DAC */
    DAC->GPDAC_PDN &= ~GPDAC_PDN_GPDAC_PDN_MASK;

    /* Power off SRAM */
    DAC->GPDAC_SRAM_PWR &= ~GPDAC_SRAM_PWR_GPDAC_SLEEPB_MASK;
    DAC->GPDAC_SRAM_PWR |= GPDAC_SRAM_PWR_GPDAC_PD_MASK;

    /* Disable clock */
    if (HAL_CLOCK_STATUS_OK != hal_clock_disable(HAL_CLOCK_CG_GPDAC)) {
        log_hal_error("\r\n [DAC] Clock disable failed!");
        return HAL_DAC_STATUS_ERROR;
    }

    /* Disable VA28_LDO */
    pmu_ctrl_driver_power(DRIVER_DAC, 0);

    /* Change status to idle */
    g_dac_status = DAC_IDLE;

    return HAL_DAC_STATUS_OK;
}

hal_dac_status_t hal_dac_start_output(void)
{
    /* Output enable: only repeat mode can be enabled! */
    DAC->OUTPUT_COMMAND |= (OUTPUT_COMMAND_REPEAT_EN_MASK | OUTPUT_COMMAND_OUTPUT_EN_MASK);

    return HAL_DAC_STATUS_OK;
}

hal_dac_status_t hal_dac_stop_output(void)
{
    /* Output disable: make sure repeat mode also disabled, or else the output can not be disabled! */
    DAC->OUTPUT_COMMAND &= ~(OUTPUT_COMMAND_REPEAT_EN_MASK | OUTPUT_COMMAND_OUTPUT_EN_MASK);

    return HAL_DAC_STATUS_OK;
}

hal_dac_status_t hal_dac_write_data(uint32_t start_address, const uint32_t *data, uint32_t length)
{
    uint32_t end_address;
    uint32_t index;

    end_address = start_address + length - 1;

    /* Parameter check */
    if (end_address & DAC_DATA_LENGTH_MASK) {
        log_hal_error("\r\n [DAC] Invalid parameter.");
        return HAL_DAC_STATUS_INVALID_PARAMETER;
    }

    /* Parameter check */
    if (data == NULL) {
        log_hal_error("\r\n [DAC] Invalid parameter.");
        return HAL_DAC_STATUS_INVALID_PARAMETER;
    }

    /* Write data to SRAM of DAC */
    for (index = 0; index < length; index++) {
        DAC->WRITE_COMMAND = ((start_address + index) | ((data[index] & DAC_DATA_BIT_MASK) << WRITE_COMMAND_SRAM_DATA_OFFSET));
    }

    return HAL_DAC_STATUS_OK;
}

hal_dac_status_t hal_dac_configure_output(hal_dac_mode_t mode, uint32_t start_address, uint32_t end_address)
{
    /* Parameter check */
    if ((start_address | end_address) & DAC_DATA_LENGTH_MASK) {
        log_hal_error("\r\n [DAC] Invalid parameter.");
        return HAL_DAC_STATUS_INVALID_PARAMETER;
    }

    /* repeat mode or one shot mode */
    if (mode == HAL_DAC_REPEAT_MODE) {
        DAC->OUTPUT_COMMAND |= OUTPUT_COMMAND_REPEAT_EN_MASK;
    } else {
        log_hal_error("\r\n [DAC] Invalid mode.");
        return HAL_DAC_STATUS_ERROR;
    }

    /* Configure the output region when output is enabled */
    DAC->OUTPUT_REGION = (start_address | (end_address << OUTPUT_REGION_OUTPUT_END_ADDR_OFFSET));

    return HAL_DAC_STATUS_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_DAC_MODULE_ENABLED */


