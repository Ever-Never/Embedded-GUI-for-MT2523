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

/*
** $Log: low_hal_lp.c $
**
**
**
**
*/

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/
#include "mt7687.h"
#include "hal_nvic.h"
#include "hal_lp.h"
#include "mt7687_cm4_hw_memmap.h"
#include "connsys_driver.h"
#include "connsys_bus.h"
#include <stdio.h>

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

static uint8_t hal_lp_wic_wakeup = 0;

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/
#define SYSRAM_CHECKSUM 0
#if SYSRAM_CHECKSUM
#define CHECKSUM_START  0x20000000
#define CHECKSUM_END    0x2003f000
ATTR_ZIDATA_IN_TCM volatile uint32_t before_checksum;
ATTR_ZIDATA_IN_TCM volatile uint32_t after_checksum;
ATTR_ZIDATA_IN_TCM volatile uint32_t checksum_addr;
#endif  //SYSRAM_CHECKSUM

int8_t (*ptr_lp_connsys_get_own_enable_int)(void) = NULL;
int8_t (*ptr_lp_connsys_give_n9_own)(void) = NULL;
uint8_t (*ptr_connsys_get_ownership)(void) = NULL;

static void hal_lp_cm4_hclk_switch(lhal_lp_cm4_hclk_t cm4_hclk)
{
    UINT32  cr_value = 0;
    UINT8   set_value = 0;

    /*
        CM4_HCLK_SW
            XTAL (HW default)   1.set cr_cm4_hclk_sel[2:0]=0    CM4_TOPCFGAON + 0x1B0[2:0]  3'd0
            F32K                1.set cr_cm4_hclk_sel[2:0]=1    CM4_TOPCFGAON + 0x1B0[2:0]  3'd1
            PLL1_64M            1.set cr_cm4_hclk_sel[2:0]=2    CM4_TOPCFGAON + 0x1B0[2:0]  3'd2
            PLL_CK              1.set cr_cm4_hclk_sel[2:0]=4    CM4_TOPCFGAON + 0x1B0[2:0]  3'd4
    */
    switch (cm4_hclk) {
        case LHAL_LP_CM4_HCLK_XTAL:
            set_value = 0;
            break;
        case LHAL_LP_CM4_HCLK_F32K:
            set_value = 1;
            break;
        case LHAL_LP_CM4_HCLK_PLL1_64M:
            set_value = 2;
            break;
        case LHAL_LP_CM4_HCLK_PLL_CK:
            set_value = 4;
            break;
        default:
            break;
    }
    cr_value = HAL_REG_32(TOP_AON_CM4_CKGEN0) & (~CM4_CKGEN0_CM4_HCLK_SEL_MASK);
    cr_value |= (set_value << CM4_CKGEN0_CM4_HCLK_SEL_OFFSET);
    HAL_REG_32(TOP_AON_CM4_CKGEN0) = cr_value;
}


int8_t hal_lp_connsys_get_own_enable_int()
{
    if (ptr_lp_connsys_get_own_enable_int != NULL) {
        return ptr_lp_connsys_get_own_enable_int();
    }

    return 0;
}


int8_t hal_lp_connsys_give_n9_own()
{
    if (ptr_lp_connsys_give_n9_own != NULL) {
        return ptr_lp_connsys_give_n9_own();
    }

    return 0;
}


int8_t hal_lp_connsys_get_ownership()
{
    if (ptr_connsys_get_ownership != NULL) {
        return ptr_connsys_get_ownership();
    }

    return TRUE;
}

void hal_lp_deep_sleep_set_rgu()
{
    /* Deep Sleep RGU control for Memory */

    /* ROM_CODE        (arx) : ORIGIN = 0x0,            LENGTH = 0x00005000  20k  */
    /* TCMRAM          (arw) : ORIGIN = 0x00100000,     LENGTH = 0x00010000  64k  */
    /* XIP_LOADER_CODE (arx) : ORIGIN = 0x10000000,     LENGTH = 0x00008000  32k  */
    /* XIP_CODE        (arx) : ORIGIN = 0x1006C000,     LENGTH = 0x00040000  256k */
    /* SYSRAM          (arw) : ORIGIN = 0x20000000,     LENGTH = 0x00040000  256k */

    P_IOT_CM4_RGU_MEM_CTRL_TypeDef pIOTCM4RGUMemCtrlTypeDef = (P_IOT_CM4_RGU_MEM_CTRL_TypeDef)(CM4_TOPRGU_BASE + 0x17C);

    /* set IDLM ROM PD to HW control        CM4_TOPRGU + 0x1C0 */
    /* Power Down ROM */
    /* 1bit for 16KB memory, total 64K */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_ROM_PD_EN) = 0xF;

    /* CM4_MEM0_PDN_EN and CM4_MEM0_SLP_EN corresponding bits can NOT both be 0 */
    /*
        0x83009190  CM4_MEM0_PDN_EN
            31  16  CM4_RAMD_HWCTL_PDN  "1bit for 16KB memory       (SYSRAM)"
            15  8   CM4_RAMC_HWCTL_PDN  "1bit for 8KB memory        (TCM)"
            7   4   CM4_RAMB_HWCTL_PDN  "1bit for 8KB memory        (CACHE)"
            3   0   CM4_RAMA_HWCTL_PDN  "1bit for 1 Memory Macro    (HSP)"
     */
    /*
        0x83009194  CM4_MEM1_PDN_EN
            7   4   CM4_RAM_AUDIO_HWCTL_PDN "1bit for 1 Memory Macro    (AUDIO)"
            3   0   CM4_RAME_HWCTL_PDN      "1bit for 1 Memory Macro    (L1CACHE)"
     */

    /* Power Down SYSRAM except last bank */
    /* 1bit for 16KB memory */
    /* set IDLM RAM PD to HW control        CM4_TOPRGU + 0x190, 0x194 */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM0_PDN_EN) = 0x7FFFFFFF;
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM1_PDN_EN) = 0x00FF; /* [7:4] Audio RAM, [3:0] L1 Cache */

    /* set IDLM RAM SLEEP to HW control     CM4_TOPRGU + 0x198, 0x19C */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM0_SLP_EN) = 0x80000000;
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM1_SLP_EN) = 0x0; /* [7:4] Audio RAM, [3:0] L1 Cache */

}

void hal_lp_legacy_sleep_set_rgu()
{
    /* Deep Sleep RGU control for Memory */

    /* ROM_CODE        (arx) : ORIGIN = 0x0,            LENGTH = 0x00005000  20k  */
    /* TCMRAM          (arw) : ORIGIN = 0x00100000,     LENGTH = 0x00010000  64k  */
    /* XIP_LOADER_CODE (arx) : ORIGIN = 0x10000000,     LENGTH = 0x00008000  32k  */
    /* XIP_CODE        (arx) : ORIGIN = 0x1006C000,     LENGTH = 0x00040000  256k */
    /* SYSRAM          (arw) : ORIGIN = 0x20000000,     LENGTH = 0x00040000  256k */

    P_IOT_CM4_RGU_MEM_CTRL_TypeDef pIOTCM4RGUMemCtrlTypeDef = (P_IOT_CM4_RGU_MEM_CTRL_TypeDef)(CM4_TOPRGU_BASE + 0x17C);


    /* set IDLM ROM PD to HW control        CM4_TOPRGU + 0x1C0 */
    /* Power Down ROM */
    /* 1bit for 16KB memory, total 64K */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_ROM_PD_EN) = 0xF;


    /* CM4_MEM0_PDN_EN and CM4_MEM0_SLP_EN corresponding bits can NOT both be 0 */
    /*
        0x83009190  CM4_MEM0_PDN_EN
            31  16  CM4_RAMD_HWCTL_PDN  "1bit for 16KB memory       (SYSRAM)"
            15  8   CM4_RAMC_HWCTL_PDN  "1bit for 8KB memory        (TCM)"
            7   4   CM4_RAMB_HWCTL_PDN  "1bit for 8KB memory        (CACHE)"
            3   0   CM4_RAMA_HWCTL_PDN  "1bit for 1 Memory Macro    (HSP)"
     */
    /*
        0x83009194  CM4_MEM1_PDN_EN
            7   4   CM4_RAM_AUDIO_HWCTL_PDN "1bit for 1 Memory Macro    (AUDIO)"
            3   0   CM4_RAME_HWCTL_PDN      "1bit for 1 Memory Macro    (L1CACHE)"
     */

    /* Sleep all SYSRAM */
    /* 1bit for 16KB memory */
    /* set IDLM RAM PD to HW control        CM4_TOPRGU + 0x190, 0x194 */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM0_PDN_EN) = 0x0;
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM1_PDN_EN) = 0x0; /* [7:4] Audio RAM, [3:0] L1 Cache */

    /* set IDLM RAM SLEEP to HW control     CM4_TOPRGU + 0x198, 0x19C */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM0_SLP_EN) = 0xFFFFFFFF;
    (pIOTCM4RGUMemCtrlTypeDef->CM4_MEM1_SLP_EN) = 0x00FF; /* [7:4] Audio RAM, [3:0] L1 Cache */

}

void hal_lp_deep_sleep()
{
    P_IOT_CM4_RGU_MEM_CTRL_TypeDef pIOTCM4RGUMemCtrlTypeDef = (P_IOT_CM4_RGU_MEM_CTRL_TypeDef)(CM4_TOPRGU_BASE + 0x17C);

    /* enable RGU top_hwctl    CM4_TOPRGU + 0x180[31]  1'b1    1: deep sleep, MTCMOS power off */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_POS_S_EN) |= (BIT(31));

    hal_lp_cm4_hclk_switch(LHAL_LP_CM4_HCLK_XTAL);
    hal_lp_deep_sleep_set_rgu();

    /* set HW_CONTROL   CM4_TOPCFGAON + 0x1B8[31:30]    2'b11 */
    HAL_REG_32(CM4_TOPCFGAON_BASE + 0x1B8) |= (BITS(30, 31));

    /* Can not in XIP, due to flash need AHB which should be controlled by HW instead of SW force */
    /* set AHB_STOP=1, MCUSYS_STOP=1    CM4_CONFG + 0x104[2:0]  3'b111 */
    /* HAL_REG_32(CM4_CONFIG_BASE + 0x104) |= (BITS(0, 2)); */

    /* enable CM4 System Control Register SleepDeep bit    0xE000ED10[2]   1'b1 */
    HAL_REG_32(MCU_CFG_NVIC_BASE + 0xD10) |= BIT(2);

#if SYSRAM_CHECKSUM
    __SYSRAM_CHECKSUM_BEFORE_SLEEP();
#endif  //SYSRAM_CHECKSUM

    /* set CM4 code "__wfi();" */
    /* Could be called outside to increase flexibility */

    __asm volatile("dsb");

    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    __asm volatile("wfi");

    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    __asm volatile("isb");

#if SYSRAM_CHECKSUM
    __SYSRAM_CHECKSUM_AFTER_SLEEP();
#endif  //SYSRAM_CHECKSUM

    HAL_REG_32(MCU_CFG_NVIC_BASE + 0xD10) &= ~(BIT(2));
    HAL_REG_32(TOP_CFG_CM4_PWR_CTL_CR)    &= ~(BIT(30));

#if SYSRAM_CHECKSUM
    __SYSRAM_CHECKSUM_RESULT();
#endif  //SYSRAM_CHECKSUM
}


void hal_lp_legacy_sleep()
{
    P_IOT_CM4_RGU_MEM_CTRL_TypeDef pIOTCM4RGUMemCtrlTypeDef = (P_IOT_CM4_RGU_MEM_CTRL_TypeDef)(CM4_TOPRGU_BASE + 0x17C);

    /* disable RGU top_hwctl   CM4_TOPRGU + 0x180[31]  1'b0    0: legacy sleep, keep MTCMOS power on */
    (pIOTCM4RGUMemCtrlTypeDef->CM4_POS_S_EN) &= (~BIT(31));

    hal_lp_cm4_hclk_switch(LHAL_LP_CM4_HCLK_XTAL);
    hal_lp_legacy_sleep_set_rgu();

    /* set HW_CONTROL   CM4_TOPCFGAON + 0x1B8[31:30]    2'b11 */
    HAL_REG_32(CM4_TOPCFGAON_BASE + 0x1B8) |= (BITS(30, 31));

    /* Can not in XIP, due to flash need AHB which should be controlled by HW instead of SW force */
    /* set AHB_STOP=1, MCUSYS_STOP=1    CM4_CONFG + 0x104[2:0]  3'b111 */
    /* HAL_REG_32(CM4_CONFIG_BASE + 0x104) |= (BITS(0, 2)); */

    /* enable CM4 System Control Register SleepDeep bit    0xE000ED10[2]   1'b1 */
    HAL_REG_32(MCU_CFG_NVIC_BASE + 0xD10) |= BIT(2);

    /* set CM4 code "__wfi();" */
    /* Could be called outside to increase flexibility */
    __asm volatile("dsb");

    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    __asm volatile("wfi");

    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");
    __asm volatile("nop");

    __asm volatile("isb");

    HAL_REG_32(MCU_CFG_NVIC_BASE + 0xD10) &= ~(BIT(2));
    HAL_REG_32(TOP_CFG_CM4_PWR_CTL_CR)    &= ~(BIT(30));
}

static void hal_lp_wic_isr(hal_nvic_irq_t irq_number)
{
    /* clear WIC IRQ status */
    HAL_REG_32(CM4_WIC_SW_CLR_ADDR) = 0x1;

    hal_lp_wic_wakeup = 1;
}

void hal_lp_handle_intr()
{
    hal_nvic_register_isr_handler((hal_nvic_irq_t)CM4_WIC_INT_WAKE_UP_IRQ, hal_lp_wic_isr);
    hal_nvic_set_priority((hal_nvic_irq_t)CM4_WIC_INT_WAKE_UP_IRQ, CM4_WIC_INT_WAKE_UP_PRI);
    hal_nvic_enable_irq((hal_nvic_irq_t)CM4_WIC_INT_WAKE_UP_IRQ);
}

uint8_t hal_lp_get_wic_wakeup()
{
    return hal_lp_wic_wakeup;
}

uint32_t hal_lp_get_wic_status()
{
    return (HAL_REG_32(CM4_WIC_PEND_STA0_ADDR) & BIT(31));
}

