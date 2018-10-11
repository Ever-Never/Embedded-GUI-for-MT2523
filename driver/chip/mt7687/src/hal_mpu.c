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

#include "hal_mpu.h"

#ifdef HAL_MPU_MODULE_ENABLED

#include "hal_mpu_internal.h"
#include "hal_log.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "assert.h"


#ifdef __cplusplus
extern "C" {
#endif


#define MPU_BUSY 1
#define MPU_IDLE 0

volatile uint8_t g_mpu_status = MPU_IDLE;

hal_mpu_status_t hal_mpu_init(const hal_mpu_config_t *mpu_config)
{
    hal_mpu_region_t region;
    uint32_t irq_flag;

    /* Parameter check */
    if (mpu_config == NULL) {
        return HAL_MPU_STATUS_INVALID_PARAMETER;
    }

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    irq_flag = save_and_set_interrupt_mask();

    /* Check module status */
    if (g_mpu_status == MPU_BUSY) {
        /* Restore the previous status of interrupt */
        restore_interrupt_mask(irq_flag);

        return HAL_MPU_STATUS_ERROR_BUSY;
    } else {
        /* Change status to busy */
        g_mpu_status = MPU_BUSY;

        /* Restore the previous status of interrupt */
        restore_interrupt_mask(irq_flag);
    }

    /* Set CTRL register to default value */
    MPU->CTRL = 0;

    /* Update PRIVDEFENA and HFNMIENA bit of CTRL register */
    if (mpu_config->privdefena == TRUE) {
        MPU->CTRL |= MPU_CTRL_PRIVDEFENA_Msk;
    }
    if (mpu_config->hfnmiena == TRUE) {
        MPU->CTRL |= MPU_CTRL_HFNMIENA_Msk;
    }

    /* Update the global variable */
    g_mpu_ctrl.w = MPU->CTRL;
    g_mpu_region_en = 0;

    /* Init global variable for each region */
    for (region = HAL_MPU_REGION_0; region < HAL_MPU_REGION_MAX; region++) {
        /* Initialize address to 0 for each region*/
        g_mpu_entry[region].mpu_rbar.b.ADDR = 0;

        /* Initialize region field to corresponding region */
        g_mpu_entry[region].mpu_rbar.b.REGION = region;

        /* Initialize valid field to 1, indicating region field is valide when write this global variable to MPU_RBAR*/
        g_mpu_entry[region].mpu_rbar.b.VALID = 1;

        /* Initialize rasr to 0 for each region*/
        g_mpu_entry[region].mpu_rasr.w = 0;
    }

    return HAL_MPU_STATUS_OK;
}

hal_mpu_status_t hal_mpu_deinit(void)
{
    hal_mpu_region_t region;

    /* Set CTRL register to default value */
    MPU->CTRL = 0;

    /* Update the global variable */
    g_mpu_ctrl.w = 0;
    g_mpu_region_en = 0;

    /* Reset MPU setting as well as global variables to default value */
    for (region = HAL_MPU_REGION_0; region < HAL_MPU_REGION_MAX; region++) {
        MPU->RBAR = (region << MPU_RBAR_REGION_Pos) | MPU_RBAR_VALID_Msk;
        MPU->RASR = 0;

        /* Update the global variable */
        g_mpu_entry[region].mpu_rbar.w = 0;
        g_mpu_entry[region].mpu_rasr.w = 0;
    }

    /* Change status to idle */
    g_mpu_status = MPU_IDLE;

    return HAL_MPU_STATUS_OK;
}

hal_mpu_status_t hal_mpu_enable(void)
{
    /* Enable MPU */
    MPU->CTRL |= MPU_CTRL_ENABLE_Msk;

    /* Update the global variable */
    g_mpu_ctrl.w = MPU->CTRL;

    return HAL_MPU_STATUS_OK;
}

hal_mpu_status_t hal_mpu_disable(void)
{
    /* Disable MPU */
    MPU->CTRL &= ~MPU_CTRL_ENABLE_Msk;

    /* Update the global variable */
    g_mpu_ctrl.w = MPU->CTRL;

    return HAL_MPU_STATUS_OK;
}

hal_mpu_status_t hal_mpu_region_enable(hal_mpu_region_t region)
{
    /* Region is invalid */
    if (region >= HAL_MPU_REGION_MAX) {
        return HAL_MPU_STATUS_ERROR_REGION;
    }

    /* Enable corresponding region */
    MPU->RNR = (region << MPU_RNR_REGION_Pos);
    MPU->RASR |= MPU_RASR_ENABLE_Msk;

    /* Update the global variable */
    g_mpu_entry[region].mpu_rasr.w = MPU->RASR;
    g_mpu_region_en |= (1 << region);

    return HAL_MPU_STATUS_OK;
}

hal_mpu_status_t hal_mpu_region_disable(hal_mpu_region_t region)
{
    /* Region is invalid */
    if (region >= HAL_MPU_REGION_MAX) {
        return HAL_MPU_STATUS_ERROR_REGION;
    }

    /* Disable corresponding region */
    MPU->RNR = (region << MPU_RNR_REGION_Pos);
    MPU->RASR &= ~MPU_RASR_ENABLE_Msk;

    /* Update the global variable */
    g_mpu_entry[region].mpu_rasr.w = MPU->RASR;
    g_mpu_region_en &= ~(1 << region);

    return HAL_MPU_STATUS_OK;
}

hal_mpu_status_t hal_mpu_region_configure(hal_mpu_region_t region, const hal_mpu_region_config_t *region_config)
{
    /* Region is invalid */
    if (region >= HAL_MPU_REGION_MAX) {
        return HAL_MPU_STATUS_ERROR_REGION;
    }

    /* Parameter check */
    if (region_config == NULL) {
        return HAL_MPU_STATUS_INVALID_PARAMETER;
    }

    /* The MPU region size is invalid */
    if ((region_config->mpu_region_size <= HAL_MPU_REGION_SIZE_MIN) || (region_config->mpu_region_size >= HAL_MPU_REGION_SIZE_MAX)) {
        assert(0);
        return HAL_MPU_STATUS_ERROR_REGION_SIZE;
    }

    /* The MPU region address must be size aligned */
    if (region_config->mpu_region_address != (region_config->mpu_region_address & (0xFFFFFFFFUL << (region_config->mpu_region_size + 1)))) {
        assert(0);
        return HAL_MPU_STATUS_ERROR_REGION_ADDRESS;
    }

    /* Write the region setting to corresponding register */
    MPU->RBAR = (region_config->mpu_region_address & MPU_RBAR_ADDR_Msk) | (region << MPU_RBAR_REGION_Pos) | MPU_RBAR_VALID_Msk;
    MPU->RASR = ((region_config->mpu_region_size << MPU_RASR_SIZE_Pos) | (region_config->mpu_region_access_permission << MPU_RASR_AP_Pos) | (region_config->mpu_subregion_mask << MPU_RASR_SRD_Pos));

    /* Set the XN(execution never) bit of RASR if mpu_xn is true */
    if (region_config->mpu_xn == TRUE) {
        MPU->RASR |= MPU_RASR_XN_Msk;
    }

    /* Update the global variable */
    g_mpu_entry[region].mpu_rbar.b.ADDR = region_config->mpu_region_address >> 5;
    g_mpu_entry[region].mpu_rasr.w = MPU->RASR;

    return HAL_MPU_STATUS_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_MPU_MODULE_ENABLED */

