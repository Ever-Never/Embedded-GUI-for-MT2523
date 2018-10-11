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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "mt7687.h"
#include "mt7687_cm4_hw_memmap.h"
#include "system_mt7687.h"

#include "hal.h"
#include "syslog.h"
#include "top.h"
#include "hal_lp.h"
#include "connsys_driver.h"
#include "sys_init.h"
#include "bsp_gpio_ept_config.h"

 /* Include this file for I/O interface API default implementation */
#include "io_def.h"

/**
* @brief       This function is low-level libc implementation, which is used for printf family.
* @param[in]   ch: the character that will be put into uart port.
* @return      The character as input.
*/

/* Predefined MACRO of gcc */
/*
#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    hal_uart_put_char(HAL_UART_0, ch);
    if (ch == '\n') {
        hal_uart_put_char(HAL_UART_0, '\r');
    }
    return ch;
}
*/

/**
* @brief       This function is to config system clock.
* @return      None.
*/
static void SystemClock_Config(void)
{
    /* Initialize XTAL, and system runs at default frequency */
    top_xtal_init();
}

/* Cache config table, MAX region number is 16 */
static const hal_cache_region_config_t region_cfg_tbl[] = {
    /* cacheable address, cacheable size(both MUST be 4k bytes aligned) */

    /* Set XIP flash region to cacheable. Please refer to memory layout dev guide for more detail. */
    { CM4_EXEC_IN_PLACE_BASE, 0x1000000}
};

/**
* @brief       This function is to do cache initialization.
* @param[in]   cache_size: the size of cache user wants to config.
*              #HAL_CACHE_SIZE_32KB, config cache size as 32KB.\n
*              #HAL_CACHE_SIZE_16KB, config cache size as 16KB.\n
*              #HAL_CACHE_SIZE_8KB, config cache size as 8KB.\n
* @return      #0, if OK.\n
*              #-1, if user's parameter is invalid.\n
*/
static int32_t cache_enable(hal_cache_size_t cache_size)
{
    hal_cache_region_t region, region_number;

    region_number = (hal_cache_region_t)(sizeof(region_cfg_tbl) / sizeof(region_cfg_tbl[0]));
    if (region_number > HAL_CACHE_REGION_MAX) {
        return -1;
    }

    hal_cache_init();

    /* Set the cache size to 32KB. It will ocuppy the TCM memory size */
    hal_cache_set_size(cache_size);
    for (region = HAL_CACHE_REGION_0; region < region_number; region++) {
        hal_cache_region_config(region, &region_cfg_tbl[region]);
        hal_cache_region_enable(region);
    }
    for (; region < HAL_CACHE_REGION_MAX; region++) {
        hal_cache_region_disable(region);
    }
    hal_cache_enable();
    return 0;
}

/**
* @brief       This function is to setup system hardware, such as increase system clock, uart init etc.
* @return      None.
*/
static void prvSetupHardware(void)
{
/*PMU driver init setting for lower power*/
#if defined(MTK_HAL_LOWPOWER_ENABLE)

    /* Handle low power interrupt */
    hal_lp_handle_intr();

    if (1 == hal_lp_get_wic_wakeup()) {
        /* N9 MUST be active for clock switch and pinmux config*/
        /* Wakeup N9 by connsys ownership */
        connsys_open();
        connsys_close();
    }
#endif

    /* Enable MCU clock to 192MHz */
    cmnCpuClkConfigureTo192M();

    /*Enable flash clock to 64MHz*/
    cmnSerialFlashClkConfTo64M();

    /* bsp_ept_gpio_setting_init() under driver/board/mt25x3_hdk/ept will initialize the GPIO settings
     * generated by easy pinmux tool (ept). ept_*.c and ept*.h are the ept files and will be used by
     * bsp_ept_gpio_setting_init() for GPIO pinumux setup.
     */
    bsp_ept_gpio_setting_init();

    /* initialize UART0 as logging port */
    bsp_io_def_uart_init();

    /* cache init */
    if (cache_enable(HAL_CACHE_SIZE_32KB) < 0) {
        LOG_E(common, "cache enable failed");
    }

    hal_flash_init();
}

/**
* @brief       This function is to get random seed.
* @return      None.
*/
static void _main_sys_random_init(void)
{
/*This option is to enable TRNG(Ture Random Number Generator).*/
#if defined(HAL_TRNG_MODULE_ENABLED)
    uint32_t            seed;
    hal_trng_status_t   s;

    s = hal_trng_init();

    if (s == HAL_TRNG_STATUS_OK) {
        s = hal_trng_get_generated_random_number(&seed);

    }

    if (s == HAL_TRNG_STATUS_OK) {
        srand((unsigned int)seed);
    }

    if (s != HAL_TRNG_STATUS_OK) {
        printf("trng init failed\n");
    } else {

    }
#endif /* HAL_TRNG_MODULE_ENABLED */
}

/**
* @brief       This function is to do system initialization, eg: hardware, nvdm, logging and random seed.
* @return      None.
*/
void system_init(void)
{
    time_t      t       = 12345;

    SystemClock_Config();

    prvSetupHardware();

/* Refer to LinkIt_for_RTOS_System_Log_Developers_Guide.pdf for more detail */
#ifndef MTK_DEBUG_LEVEL_NONE
    log_init(NULL, NULL, NULL);
#endif

    /* workaround for NSTP */
    ctime(&t);

    /* generate random seed */
    _main_sys_random_init();

    LOG_I(common, "FreeRTOS Running");
}


