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

#include "bl_common.h"
#include "bl_fota.h"
#include "bl_uart_sw.h"
#include "emi_init.h"
#include "bl_clock.h"
#include "hal_flash.h"
#include "hal_pmu.h"
#include "hal_mt2523_pmu_platform.h"
#include "hal_i2c_pmic.h"
#include "core_cm4.h"
#include "bl_misc.h"
#include "bl_dcxo_cal.h"
#include "hal_pmu_internal.h"
#include "bl_clock.h"
#include "bl_usb.h"
#include "bl_pmic.h"
#include "hal_wdt.h"

void bl_hardware_init()
{
    /* UART init */
    bl_uart_init(BL_SYSTEM_CLK_FREQ);

    /* Enable FPU. Set CP10 and CP11 Full Access.  bl_print_internal in keil uses FPU.*/
    SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));
    bl_print(LOG_DEBUG,"set CP10 and CP11 Full Access\r\n");

    /* print log */
    bl_set_debug_level(LOG_DEBUG);
    bl_print(LOG_DEBUG, "bl_uart_init\n\r");

    /* dcxo capid init , must before PLL enable */
    bl_dcxo_capid_init();

#ifdef BL_RUN_DCXO_CAL
    /* run dcxo factory calibration , must before PLL enable */
    bl_dcxo_factory_calibration();
#endif

    /* dcxo load calibration , must before PLL enable */
    bl_dcxo_load_calibration();

#if 1
    /* i2c clock enable */
    bl_print(LOG_DEBUG, "i2c clock enable \n\r");
    *BL_PDN_CLRD1 = 0x0040;

	bl_print(LOG_DEBUG, "bl_pmic_init\n\r");
	bl_pmic_init();
#endif

#ifndef __FPGA_TARGET__
    /* EMI and PLL init */
    bl_print(LOG_DEBUG, "custom_setEMI \n\r");
    custom_setEMI();

    /* PLL init */
    bl_print(LOG_DEBUG, "clk_set_pll_dcm_init \n\r");
    clock_set_pll_dcm_init();
    bl_print(LOG_DEBUG, "cm4 freq=%d\n\r", bl_clock_freq_monitor(19, 0, CLK_OVER_78M));
    bl_print(LOG_DEBUG, "upll freq=%d\n\r", bl_clock_freq_monitor(4, 0, CLK_OVER_78M));
    bl_print(LOG_DEBUG, "bus freq=%d\n\r", bl_clock_freq_monitor(8, 0, CLK_LOWER_78M));

    bl_print(LOG_DEBUG, "custom_setAdvEMI \n\r");
    custom_setAdvEMI();
#endif

    /* SFI init */
    bl_print(LOG_DEBUG, "custom_setSFIExt \n\r");
    custom_setSFIExt();

#if defined(BL_FOTA_ENABLE) || defined(__SERIAL_FLASH_STT_EN__)
    bl_print(LOG_DEBUG, "NOR_init \n\r");
    hal_flash_init();
#endif

    bl_print(LOG_DEBUG, "hal_flash_init \n\r");
    hal_flash_init();
    /* SF STT and Disturbance Test*/
#ifdef __SERIAL_FLASH_STT_EN__
    extern void stt_main(void);
    stt_main();
#endif

    /* enable 30 seconds watchdog timeout. */
    hal_wdt_config_t wdt_config;
    wdt_config.mode = HAL_WDT_MODE_RESET;
    wdt_config.seconds = 30;
    hal_wdt_status_t wdt_ret;

    wdt_ret = hal_wdt_init(&wdt_config);
    if (wdt_ret < 0) {
        bl_print(LOG_ERROR, "hal_wdt_init (%d) \n\r", wdt_ret);
    }
    hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);

#if 1
    bl_print(LOG_DEBUG, "gpdac_sram_power_down\n\r");
    bl_gpdac_sram_power_down();
    bl_print(LOG_DEBUG, "config bonding io register\n\r");
    bl_bonding_io_reg();
    bl_print(LOG_DEBUG, "D2D IO pull down\n\r");
    bl_d2d_io_pull_down();
#endif
}

void bl_set_usb_sleep()
{
#if 1
    pmic_i2c_config_t config;
#endif

#if 1
    /* i2c clock enable */
    bl_print(LOG_DEBUG, "i2c clock enable \n\r");
    *BL_PDN_CLRD1 = 0x0040;

    /* pmic i2c init */
    bl_print(LOG_DEBUG, "i2c init \n\r");
    config.transfer_frequency = 100;
    pmic_i2c_init(PMIC_I2C_MASTER, &config);

    /*usb low power mode*/
    bl_usb_low_power_setting();

    /* i2c clock disable */
    bl_print(LOG_DEBUG, "i2c clock disable \n\r");
    *BL_PDN_SETD1 = 0x0040;
#endif

}


void bl_start_user_code()
{
    uint32_t targetAddr = bl_custom_cm4_start_address();
    bl_print(LOG_DEBUG, "Jump to addr %x\n\r", targetAddr);
    JumpCmd(targetAddr);
}

int main()
{

    bl_hardware_init();

#ifdef BL_FOTA_ENABLE
    bl_fota_process();
#endif

    /* no uart log after bl_set_usb_sleep() */
    bl_set_usb_sleep();

    bl_start_user_code();

    return 0;
}
