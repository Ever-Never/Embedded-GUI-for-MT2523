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

#ifndef __HAL_CLOCK_INTERNAL_H__
#define __HAL_CLOCK_INTERNAL_H__

#include "hal_platform.h"

#ifdef HAL_CLOCK_MODULE_ENABLED

#include "hal_clock_platform_mt2523.h"
#include "memory_attribute.h"

#ifdef MTK_CLK_USE_MPLL
/* #define CLK_USE_MPLL */
#endif

#ifdef MTK_CLK_USE_LFOSC
#define CLK_USE_LFOSC
#endif

/* #define CLK_BRING_UP */
/* #define CLK_CG_DEFAULT_ON */
/* #define CLK_DEBUG */
/* #define CLK_DEBUG_ERR */
/* #define CLK_DFS_TEST */
/* #define CLK_CALI_LFOSC_TEST */

#define CLK_BUS_FREQ_62M      ((uint32_t) 62400000)
#define CLK_BUS_FREQ_52M      ((uint32_t) 52000000)
#define CLK_BUS_FREQ_26M      ((uint32_t) 26000000)

/*****************************************************************************
* Enum
*****************************************************************************/
/** @defgroup hal_clock_enum Enum
  * @{
  */
/** @brief This enum defines return type of clock source. */
typedef enum {
    HAL_CLOCK_SRC_DCXO        = 0x1,            /**< DCXO (26M) */
    HAL_CLOCK_SRC_LFOSC       = 0x2,            /**< LFOSC (low frequency) */
    HAL_CLOCK_SRC_HFOSC       = 0x4,            /**< HFOSC (high frequency) */
} hal_clock_src_type_t;

/*****************************************************************************
* Structure
*****************************************************************************/
/** @brief Internal oscillator (DCXO, LFOSC and HFOSC) configuration structure definition */
typedef struct {
    uint32_t osc_type;      /**< The oscillators to be configured */
    uint32_t osc_state;     /**< The current state of oscillators */
    uint8_t lfosc_cali;     /**< Frequency calibration for LFOSC */
    uint8_t lfosc_ft;       /**< Fine tune for LFOSC */
    uint8_t hfosc_cali;     /**< Frequency calibration for HFOSC */
    uint8_t hfosc_ft;       /**< Fine tune for HFOSC */
} clock_osc_type_t;

/** @brief System, AHB and APB bus clock configuration structure definition */
typedef struct {
    uint32_t clock_type;          /**< The system clock to be configured. */
    uint32_t sys_clock_src;       /**< The clock source (SYSCLKS) used as system clock. */
    uint32_t ahb_clock_divider;   /**< The AHB clock (HCLK) divider. This clock is derived from the system clock (SYSCLK). */
    uint32_t apb1_clock_divider;  /**< The APB1 clock (PCLK1) divider. This clock is derived from the AHB clock (HCLK). */
    uint32_t apb2_clock_divider;  /**< The APB2 clock (PCLK2) divider. This clock is derived from the AHB clock (HCLK). */
} sys_clock_type_t;

ATTR_RWDATA_IN_TCM extern clock_cm_freq_state_id cm_freq_state;

/*************************************************************************
 * Define customized function prototype
 *************************************************************************/
ATTR_TEXT_IN_TCM int clk_init(void);
ATTR_TEXT_IN_TCM void cg_init(void);
ATTR_TEXT_IN_TCM void clock_dump_info(void);
ATTR_TEXT_IN_TCM void clock_dump_log(void);
ATTR_TEXT_IN_TCM bool is_clk_use_lfosc(void);
#ifdef CLK_CALI_LFOSC_TEST
ATTR_TEXT_IN_TCM uint32_t clock_lfosc_cali_up(void);
ATTR_TEXT_IN_TCM uint32_t clock_lfosc_cali_down(void);
#endif

/* CG */
ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_force_disable(hal_clock_cg_id clock_id); /* Debug only */

/* MUX */
ATTR_TEXT_IN_TCM hal_clock_status_t clock_mux_sel(clock_mux_sel_id mux_id, uint32_t mux_sel);
ATTR_TEXT_IN_TCM int8_t clock_mux_get_state(clock_mux_sel_id mux_id);

/* Change bit */
ATTR_TEXT_IN_TCM hal_clock_status_t clock_change_bit_set(clock_change_bit_id change_bit_id);
ATTR_TEXT_IN_TCM hal_clock_status_t clock_change_bit_verified_all(void);

/* Divider */
ATTR_TEXT_IN_TCM hal_clock_status_t clock_div_enable(clock_div_id div_id);
ATTR_TEXT_IN_TCM hal_clock_status_t clock_div_disable(clock_div_id div_id);

/* PLL */
ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_enable_mpll(void);
ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_disable_mpll(void);
ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_enable_upll(void);
ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_disable_upll(void);

/* Suspend/Resume */
ATTR_TEXT_IN_TCM hal_clock_status_t clock_suspend(bool is_sleep_with_26m);
ATTR_TEXT_IN_TCM hal_clock_status_t clock_resume(void);

/* Frequency meter used */
#if 0
ATTR_TEXT_IN_TCM static uint32_t clock_get_freq_monitor(uint16_t tcksel, uint16_t fcksel, bool over78m);
#endif
ATTR_TEXT_IN_TCM uint32_t freq_monitor(uint16_t target, bool over78m);
uint32_t clock_get_freq_lfosc(void);
uint32_t clock_get_freq_peri_bus(void);

/* LFOSC cali and ft setting offset for UART testing */
ATTR_TEXT_IN_TCM void lfosc_cali_ft_set_offset(int offset);

/* DVT DVFS used */
ATTR_TEXT_IN_TCM void cm_bus_clk_208m(void);
ATTR_TEXT_IN_TCM void cm_bus_clk_104m(void);
ATTR_TEXT_IN_TCM void cm_bus_clk_26m(void);
ATTR_TEXT_IN_TCM void peri_clk_26m(void);

ATTR_TEXT_IN_TCM void cm_bus_clk_208m_mpll_mpll(void);       /* 1.3V, 208M, CM: MPLL, BUS: MPLL */
ATTR_TEXT_IN_TCM void cm_bus_clk_104m_mpll_mpll(void);       /* 1.1V, 104M, CM: MPLL, BUS: MPLL */
ATTR_TEXT_IN_TCM void cm_bus_clk_208m_mpll_hfosc(void);      /* 1.3V, 208M, CM: MPLL, BUS: HFOSC */
ATTR_TEXT_IN_TCM void cm_bus_clk_104m_hfosc_hfosc(void);     /* 1.1V, 104M, CM: HFOSC, BUS: HFOSC */

ATTR_TEXT_IN_TCM void cm_bus_clk_26m_lfosc_lfosc(void);
ATTR_TEXT_IN_TCM void cm_bus_clk_26m_dcxo_dcxo(void);
ATTR_TEXT_IN_TCM void peri_26m_dcxo(void);
ATTR_TEXT_IN_TCM void peri_26m_lfosc(void);

#ifdef CLK_TOP_CLK_DVT
ATTR_TEXT_IN_TCM void clock_test_idle(void);
ATTR_TEXT_IN_TCM void clock_test_efuse_104m(void);
#endif

#endif /* HAL_CLOCK_MODULE_ENABLED */
#endif /* __HAL_CLOCK_INTERNAL_H__ */
