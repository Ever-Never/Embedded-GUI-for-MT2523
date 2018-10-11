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

#include "hal_clock.h"
#include "hal_clock_internal.h"

#ifdef HAL_CLOCK_MODULE_ENABLED

#include "hal_nvic_internal.h"

//#include <stdio.h>
#include <assert.h>
#include "hal_log.h"
#include "hal_gpt.h"

#define HW_MISC0 ((volatile uint8_t *)0xA2010050)

#define BUS_DCM_CON_0_F_RG_BUS_DCM_DBC_NUM ((volatile uint8_t *)0xA21D0101)
#define BUS_DCM_CON_0_F_RG_BUS_DCM_DBC_EN ((volatile uint8_t *)0xA21D0102)
#define BUS_DCM_CON_0_F_RG_BUS_DCM_EN ((volatile uint8_t *)0xA21D0103)
#define BUS_DCM_CON_1 ((volatile uint32_t *)0xA21D0104)

#define CM4_DCM_CON_0_F_RG_CM_DCM_DBC_NUM ((volatile uint8_t *)0xA21D0111)
#define CM4_DCM_CON_0_F_RG_CM_DCM_DBC_EN ((volatile uint8_t *)0xA21D0112)
#define CM4_DCM_CON_1 ((volatile uint32_t *)0xA21D0114)

#define SFC_DCM_CON_0_F_RG_SFC_DCM_DBC_NUM ((volatile uint8_t *)0xA21D0141)
#define SFC_DCM_CON_0_F_RG_SFC_DCM_DBC_EN ((volatile uint8_t *)0xA21D0142)
#define SFC_DCM_CON_0_F_RG_SFC_DCM_APB_SEL ((volatile uint8_t *)0xA21D0143)
#define SFC_DCM_CON_1_F_RG_SFC_CLKOFF_EN ((volatile uint8_t *)0xA21D0144)
#define SFC_DCM_CON_1_F_RG_SFC_DCM_APB_TOG ((volatile uint8_t *)0xA21D0147)
#define CLK_FREQ_SWCH_F_RG_PLLCK_SEL ((volatile uint8_t *)0xA21D0170)

#define PDN_COND0_F_PDR_COND0 ((volatile uint32_t *)0xA21D0300)
#define PDN_SETD0_F_PDR_SETD0 ((volatile uint32_t *)0xA21D0310)
#define PDN_CLRD0_F_PDR_CLRD0 ((volatile uint32_t *)0xA21D0320)

#define CKSYS_TST_SEL_0 ((volatile uint32_t *)0xA2020220)
#define CKSYS_TST_SEL_1_F_TCKSEL ((volatile uint8_t *)0xA2020224)
#define CKSYS_TST_SEL_1_F_FCKSEL ((volatile uint8_t *)0xA2020225)
#define CKSYS_CLK_CFG_0_F_CLK_SYS_SEL ((volatile uint8_t *)0xA2020240)
#define CKSYS_CLK_CFG_0_F_CLK_SFC_SEL ((volatile uint8_t *)0xA2020241)
#define CKSYS_CLK_CFG_0_F_CLK_CONN_SEL ((volatile uint8_t *)0xA2020242)
#define CKSYS_CLK_CFG_0_F_CLK_SPIMST_SEL ((volatile uint8_t *)0xA2020243)
#define CKSYS_CLK_CFG_1_F_CLK_XTALCTL_SEL ((volatile uint8_t *)0xA2020244)
#define CKSYS_CLK_CFG_1_F_CLK_SDIOMST_SEL ((volatile uint8_t *)0xA2020245)
#define CKSYS_CLK_UPDATE_0_F_CHG_SYS ((volatile uint8_t *)0xA2020250)
#define CKSYS_CLK_UPDATE_0_F_CHG_SFC ((volatile uint8_t *)0xA2020251)
#define CKSYS_CLK_UPDATE_0_F_CHG_CONN ((volatile uint8_t *)0xA2020252)
#define CKSYS_CLK_UPDATE_0_F_CHG_SPIMST ((volatile uint8_t *)0xA2020253)
#define CKSYS_CLK_UPDATE_1_F_CHG_SDIOMST ((volatile uint8_t *)0xA2020254)
#define CKSYS_CLK_UPDATE_STATUS_0_F_CHG_SYS_OK ((volatile uint8_t *)0xA2020260)
#define CKSYS_CLK_UPDATE_STATUS_0_F_CHG_SFC_OK ((volatile uint8_t *)0xA2020261)
#define CKSYS_CLK_UPDATE_STATUS_0_F_CHG_CONN_OK ((volatile uint8_t *)0xA2020262)
#define CKSYS_CLK_UPDATE_STATUS_0_F_CHG_SPIMST_OK ((volatile uint8_t *)0xA2020263)
#define CKSYS_CLK_UPDATE_STATUS_1_F_CHG_SDIOMST_OK ((volatile uint8_t *)0xA2020264)
#define CKSYS_CLK_FORCE_ON_0_F_CLK_SYS_FORCE_ON ((volatile uint8_t *)0xA2020270)
#define CKSYS_CLK_FORCE_ON_0_F_CLK_SFC_FORCE_ON ((volatile uint8_t *)0xA2020271)
#define CKSYS_CLK_FORCE_ON_0_F_CLK_CONN_FORCE_ON ((volatile uint8_t *)0xA2020272)
#define CKSYS_CLK_FORCE_ON_0_F_CLK_SPIMST_FORCE_ON ((volatile uint8_t *)0xA2020273)
#define CKSYS_CLK_FORCE_ON_1_F_CLK_SDIOMST_FORCE_ON ((volatile uint8_t *)0xA2020274)

#define CKSYS_CLK_DIV_0_F_CLK_PLL1_D2_EN ((volatile uint8_t *)0xA2020280)
#define CKSYS_CLK_DIV_0_F_CLK_PLL1_D3_EN ((volatile uint8_t *)0xA2020281)
#define CKSYS_CLK_DIV_0_F_CLK_PLL1_D5_EN ((volatile uint8_t *)0xA2020282)
#define CKSYS_CLK_DIV_0_F_CLK_PLL1_D7_EN ((volatile uint8_t *)0xA2020283)
#define CKSYS_CLK_DIV_1_F_CLK_PLL2_D2_EN ((volatile uint8_t *)0xA2020284)
#define CKSYS_CLK_DIV_1_F_CLK_PLL2_D3_EN ((volatile uint8_t *)0xA2020285)
#define CKSYS_CLK_DIV_1_F_CLK_PLL2_D5_EN ((volatile uint8_t *)0xA2020286)
#define CKSYS_CLK_DIV_1_F_CLK_PLL2_D7_EN ((volatile uint8_t *)0xA2020287)
#define CKSYS_CLK_DIV_2_F_CLK_PLL1_DIV_EN ((volatile uint8_t *)0xA2020288)
#define CKSYS_CLK_DIV_2_F_CLK_PLL2_DIV_EN ((volatile uint8_t *)0xA2020289)
#define CKSYS_CLK_DIV_2_F_CLK_PLL1_D15_EN ((volatile uint8_t *)0xA202028A)
#define CKSYS_CLK_DIV_2_F_CLK_PLL2_D15_EN ((volatile uint8_t *)0xA202028B)
#define CKSYS_CLK_DIV_3 ((volatile uint32_t *)0xA202028C)
#define CKSYS_CLK_DIV_3_F_CR_XO_DIV_32K_EN ((volatile uint8_t*)(0xA202028C))
#define CKSYS_CLK_DIV_4 ((volatile uint32_t *)0xA2020290)
#define CKSYS_CLK_DIV_5 ((volatile uint32_t *)0xA2020294)
#define CKSYS_XTAL_FREQ ((volatile uint32_t *)0xA20202A0)
#define CKSYS_XTAL_FREQ_F_F_FXO_IS_26M ((volatile uint8_t*)(0xA20202A3))

#define CKSYS_CLK_PLL_ABIST_FQMTR_CON0 ((volatile uint16_t *)0xA2020400)
#define CKSYS_CLK_PLL_ABIST_FQMTR_CON1 ((volatile uint16_t *)0xA2020404)
#define CKSYS_CLK_PLL_ABIST_FQMTR_CON2 ((volatile uint16_t *)0xA2020408)
#define CKSYS_CLK_PLL_ABIST_FQMTR_DATA ((volatile uint32_t *)0xA202040C)

#define XO_PDN_COND0 ((volatile uint32_t *)0xA2030B00)
#define XO_PDN_SETD0 ((volatile uint32_t *)0xA2030B10)
#define XO_PDN_CLRD0 ((volatile uint32_t *)0xA2030B20)

#define XO_DCM_CON_0_F_RG_XO_DCM_DBC_NUM ((volatile uint8_t *)0xA2030C01)
#define XO_DCM_CON_0_F_RG_XO_DCM_DBC_EN ((volatile uint8_t *)0xA2030C02)
#define XO_DCM_CON_1 ((volatile uint32_t *)0xA2030C04)

#define RF_WBAC0 ((volatile uint32_t*)(0xA2040300))
#define RF_WBAC1 ((volatile uint32_t*)(0xA2040304))
#define RF_WBAC2 ((volatile uint32_t*)(0xA2040308))
#define RF_WBAC3 ((volatile uint32_t*)(0xA204030C))
#define RF_WBAC24 ((volatile uint32_t*)(0xA2040360))
#define RF_WBAC27 ((volatile uint32_t*)(0xA2040370))

#define RF_WBAC28__F_CR_WBTAC_BBPLL_SW ((volatile uint8_t*)(0xA2040374))
#define RO_RF_WBAC1 ((volatile uint32_t*)(0xA2040384))
#define RO_RF_WBAC2 ((volatile uint32_t*)(0xA2040388))

#define RG_WF_BBPLL_01 ((volatile uint32_t*)(0xA20A0700))
#define RG_WF_BBPLL_02 ((volatile uint32_t*)(0xA20A0704))

ATTR_RWDATA_IN_TCM static bool clock_initialized = false;
ATTR_ZIDATA_IN_TCM static bool fxo_is_26m;

/*************************************************************************
 * Clock mux select API definition part
 * 1. Enable individual clock divider
 * 2. Force clock on th prevent clock can't switch to target clock
 * 3. Set CSW to target clock freq. and set change bit
 * 4. After clock change to target freq. Change bit will be cleared to0 and release clock gating
 * 5. Disable forced on clock
 *************************************************************************/
ATTR_TEXT_IN_TCM hal_clock_status_t clock_mux_sel(clock_mux_sel_id mux_id, uint32_t mux_sel)
{
    uint32_t pll1_en = 0;
    uint32_t pll2_en = 0;
    volatile uint8_t * sel = NULL; // 0xA2020240 - 0xA2020245
    volatile uint8_t * chg = NULL; // 0xA2020250 - 0xA2020254
    volatile uint8_t * chg_ok = NULL; // 0xA2020260 - 0xA2020264
    volatile uint8_t * force_on = NULL; // 0xA2020270 - 0xA2020274

    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
#ifdef CLK_DEBUG_ERR
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
#endif
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }

    if (mux_id < NR_MUXS) {
        /* TODO do something relations setting for mux_sel */
        switch (mux_id) {
            case CLK_SYS_SEL:
                if (mux_sel == 0) { /* XO_CK, 26/20 MHz */
                } else if ((mux_sel == 1) || (mux_sel == 2)) { /* BBPLL2_D5, 192 MHz, BBPLL2_D5_D2, 96 MHz */
                    *CKSYS_CLK_DIV_1_F_CLK_PLL2_D5_EN = 1;
                    pll2_en = 1;
                } else if (mux_sel == 3) { /* BBPLL1_D7, 148.57 MHz */
                    *CKSYS_CLK_DIV_0_F_CLK_PLL1_D7_EN = 1;
                    pll1_en = 1;
                }
                sel = CKSYS_CLK_CFG_0_F_CLK_SYS_SEL;
                force_on = CKSYS_CLK_FORCE_ON_0_F_CLK_SYS_FORCE_ON;
                chg = CKSYS_CLK_UPDATE_0_F_CHG_SYS;
                chg_ok = CKSYS_CLK_UPDATE_STATUS_0_F_CHG_SYS_OK;
                break;

            case CLK_SFC_SEL:
                if (mux_sel == 0) { /* XO_CK, 26/20 MHz */
                } else if (mux_sel == 1) { /* BBPLL2_D3_D4, 80 MHz*/
                    *CKSYS_CLK_DIV_1_F_CLK_PLL2_D3_EN = 1;
                    pll2_en = 1;
                } else if (mux_sel == 2) { /* BBPLL2_D15, 64 MHz*/
                    *CKSYS_CLK_DIV_2_F_CLK_PLL2_D15_EN = 1;
                    pll2_en = 1;
                } else if (mux_sel == 3) { /* BBPLL1_D7_D2, 74.29 MHz */
                    *CKSYS_CLK_DIV_0_F_CLK_PLL1_D7_EN = 1;
                    pll1_en = 1;
                }
                sel = CKSYS_CLK_CFG_0_F_CLK_SFC_SEL;
                force_on = CKSYS_CLK_FORCE_ON_0_F_CLK_SFC_FORCE_ON;
                chg = CKSYS_CLK_UPDATE_0_F_CHG_SFC;
                chg_ok = CKSYS_CLK_UPDATE_STATUS_0_F_CHG_SFC_OK;
                break;

            case CLK_CONN_SEL:
                if (mux_sel == 0) { /* XO_CK, 26/20 MHz */
                } else if (mux_sel == 1) { /* BBPLL2_D3, 320 MHz*/
                    *CKSYS_CLK_DIV_1_F_CLK_PLL2_D3_EN = 1;
                    pll2_en = 1;
                } else if (mux_sel == 2) { /* BBPLL1_D2, 520 MHz*/
                    *CKSYS_CLK_DIV_0_F_CLK_PLL1_D2_EN = 1;
                    pll1_en = 1;
                }
                sel = CKSYS_CLK_CFG_0_F_CLK_CONN_SEL;
                force_on = CKSYS_CLK_FORCE_ON_0_F_CLK_CONN_FORCE_ON;
                chg = CKSYS_CLK_UPDATE_0_F_CHG_CONN;
                chg_ok = CKSYS_CLK_UPDATE_STATUS_0_F_CHG_CONN_OK;
                break;

            case CLK_SPIMST_SEL:
                if (mux_sel == 0) { /* XO_CK, 26/20 MHz */
                } else if ((mux_sel == 1) || (mux_sel == 2)) { /* BBPLL2_D5_D4, 48 MHz, BBPLL2_D5_D2, 96 MHz*/
                    *CKSYS_CLK_DIV_1_F_CLK_PLL2_D5_EN = 1;
                    pll2_en = 1;
                }
                sel = CKSYS_CLK_CFG_0_F_CLK_SPIMST_SEL;
                force_on = CKSYS_CLK_FORCE_ON_0_F_CLK_SPIMST_FORCE_ON;
                chg = CKSYS_CLK_UPDATE_0_F_CHG_SPIMST;
                chg_ok = CKSYS_CLK_UPDATE_STATUS_0_F_CHG_SPIMST_OK;
                break;

            case CLK_SDIOMST_SEL:
                if (mux_sel == 0) { /* XO_CK, 26/20 MHz */
                } else if ((mux_sel == 1) || (mux_sel == 2)) { /* BBPLL2_D5_D8, 24 MHz, BBPLL2_D5_D4, 48 MHz*/
                    *CKSYS_CLK_DIV_1_F_CLK_PLL2_D5_EN = 1;
                    pll2_en = 1;
                }
                sel = CKSYS_CLK_CFG_1_F_CLK_SDIOMST_SEL;
                force_on = CKSYS_CLK_FORCE_ON_1_F_CLK_SDIOMST_FORCE_ON;
                chg = CKSYS_CLK_UPDATE_1_F_CHG_SDIOMST;
                chg_ok = CKSYS_CLK_UPDATE_STATUS_1_F_CHG_SDIOMST_OK;
                break;

            case CLK_XTALCTL_SEL:
                /* glitch free mux, no change bit required.*/
                if (mux_sel == 0) { /* RTC_CK, 32 kHz */
                } else if (mux_sel == 1) { /* XO_BBTOP_CK, 40/26 MHz*/
                }
                sel = CKSYS_CLK_CFG_1_F_CLK_XTALCTL_SEL;
                break;

            default:
                break;
        }

        if (pll1_en != 0) {
            *CKSYS_CLK_DIV_2_F_CLK_PLL1_DIV_EN = 1;
        }
        if (pll2_en != 0) {
            *CKSYS_CLK_DIV_2_F_CLK_PLL2_DIV_EN = 1;
        }

        if (sel) {
            *sel = mux_sel;
        }
        if (force_on) {
            *force_on = 1;
        }
        if (chg) {
            *chg = 1;
             while (*chg == 1);
        }
        if (chg_ok) {
            while (*chg_ok == 1);
        }
         if (force_on) {
            *force_on = 0;
        }

        return HAL_CLOCK_STATUS_OK;
    } else {
        /* TODO return fail id information */
#ifdef CLK_DEBUG_ERR
        log_hal_error("%s, error mux id(%d) sel(%d)\r\n", __FUNCTION__, mux_id, mux_sel);
#endif
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
    }
}

ATTR_TEXT_IN_TCM int8_t clock_mux_get_state(clock_mux_sel_id mux_id)
{
    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
#ifdef CLK_DEBUG_ERR
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
#endif
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }

    if (mux_id < NR_MUXS) {
        /* TODO do something relations setting for mux_sel */
#ifdef CLK_DEBUG
        log_hal_info("%s\n", __FUNCTION__);
#endif
        switch (mux_id) {
            case CLK_SYS_SEL:
                return *CKSYS_CLK_CFG_0_F_CLK_SYS_SEL;

            default:
                return HAL_CLOCK_STATUS_INVALID_PARAMETER;
        }
    } else {
        /* TODO return fail id information */
#ifdef CLK_DEBUG_ERR
        log_hal_error("%s, error mux id(%d)\r\n", __FUNCTION__, mux_id);
#endif
        return HAL_CLOCK_STATUS_INVALID_PARAMETER;
    }
}

struct cg_clock {
    uint32_t cnt;
};

ATTR_ZIDATA_IN_TCM static struct cg_clock clocks[NR_CLOCKS];

/*************************************************************************
 * CG Clock API definition
 *************************************************************************/
ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_enable(hal_clock_cg_id clock_id)
{
    hal_clock_status_t ret = HAL_CLOCK_STATUS_OK;
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */
    volatile uint32_t * clr_addr = NULL;
    volatile uint32_t * sta_addr = NULL;
    uint32_t bit_idx = clock_id%32;

    (void)sta_addr;

#ifdef CLK_DEBUG
    /* TODO cannot print log before log_hal_info init done */
    log_hal_info("%s: clock_id=%d\r\n", __FUNCTION__, clock_id);
#endif /* ifdef CLK_DEBUG */

#ifdef CLK_BRING_UP
    ret = HAL_CLOCK_STATUS_OK;
#else
    if (clock_id<=HAL_CLOCK_CG_SDIOSLV) { /* >=HAL_CLOCK_CG_DMA is true for comparison to unsigned zero */
        sta_addr = PDN_COND0_F_PDR_COND0;
        if (clocks[clock_id].cnt == 0) {
            clr_addr = PDN_CLRD0_F_PDR_CLRD0;
            *(clr_addr) = (0x1 << bit_idx);    /* HW Register is write 1 clear */
        }
        if (clocks[clock_id].cnt < 32767) {
            clocks[clock_id].cnt++;
        }
    } else if ((clock_id>=HAL_CLOCK_CG_SW_BBPLL) && (clock_id<=HAL_CLOCK_CG_SW_AUXADC)) {
        sta_addr = XO_PDN_COND0;
        if (clocks[clock_id].cnt == 0) {
            clr_addr = XO_PDN_CLRD0;
            *(clr_addr) = (0x1 << bit_idx);    /* HW Register is write 1 clear */
        }
        if (clocks[clock_id].cnt < 32767) {
            clocks[clock_id].cnt++;
        }
    } else {
        ret = HAL_CLOCK_STATUS_INVALID_PARAMETER;
    }
#endif /* ifdef CLK_BRING_UP */

#ifdef CLK_DEBUG
    log_hal_info("ref_cnt = %d, @0x%x = %x\r\n", clocks[clock_id].cnt, sta_addr, *(sta_addr));
#endif

    restore_interrupt_mask(irq_mask);  /* restore interrupt */
    return ret;
}

ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_disable(hal_clock_cg_id clock_id)
{
    hal_clock_status_t ret = HAL_CLOCK_STATUS_OK;
    uint32_t irq_mask = save_and_set_interrupt_mask();  /* disable interrupt */
    volatile uint32_t * set_addr = NULL;
    volatile uint32_t * sta_addr = NULL;
    uint32_t bit_idx = clock_id%32;

    (void)sta_addr;

#ifdef CLK_DEBUG
    /* TODO cannot print log before log_hal_info init done */
    log_hal_info("%s: clock_id=%d\n", __FUNCTION__, clock_id);
#endif

#ifdef CLK_BRING_UP
    ret = HAL_CLOCK_STATUS_OK;
#else
    if (clock_id<=HAL_CLOCK_CG_SDIOSLV) { /* >=HAL_CLOCK_CG_DMA is true for comparison to unsigned zero */
        sta_addr = PDN_COND0_F_PDR_COND0;
        if (clocks[clock_id].cnt > 0) {
            clocks[clock_id].cnt--;
        }
        if (clocks[clock_id].cnt == 0) {
            set_addr = PDN_SETD0_F_PDR_SETD0;
            *(set_addr) = (0x1 << bit_idx);    /* HW Register is write 1 clear */
        }
    } else if ((clock_id>=HAL_CLOCK_CG_SW_BBPLL) && (clock_id<=HAL_CLOCK_CG_SW_AUXADC)) {
        sta_addr = XO_PDN_COND0;
        if (clocks[clock_id].cnt > 0) {
            clocks[clock_id].cnt--;
        }
        if (clocks[clock_id].cnt == 0) {
            set_addr = XO_PDN_SETD0;
            *(set_addr) = (0x1 << bit_idx);    /* HW Register is write 1 clear */
        }
    } else {
        ret = HAL_CLOCK_STATUS_INVALID_PARAMETER;
    }
#endif /* CLK_BRING_UP */

    /* TODO cannot use log_hal_info print log before syslog init */
#ifdef CLK_DEBUG
    log_hal_info("ref_cnt = %d, @0x%x = %x\n", clocks[clock_id].cnt, sta_addr, *(sta_addr));
#endif

    restore_interrupt_mask(irq_mask);  /* restore interrupt */
    return ret;
}

ATTR_TEXT_IN_TCM bool hal_clock_is_enabled(hal_clock_cg_id clock_id)
{
    uint32_t bit_idx = clock_id%32;
    volatile uint32_t * sta_addr = NULL;

#ifdef CLK_BRING_UP
    return true;
#endif

    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
#ifdef CLK_DEBUG_ERR
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
#endif
        return false;
    }

    if (clock_id<=HAL_CLOCK_CG_SDIOSLV) { /* >=HAL_CLOCK_CG_DMA is true for comparison to unsigned zero */
        sta_addr = PDN_COND0_F_PDR_COND0;
    } else if ((clock_id>=HAL_CLOCK_CG_SW_BBPLL) && (clock_id<=HAL_CLOCK_CG_SW_AUXADC)) {
        sta_addr = XO_PDN_COND0;
    } else {
        return false;
    }

    if (((*sta_addr) & (0x1 << bit_idx)) != 0x0) {
        /* TODO cannot use log_hal_info print log before syslog init */
#ifdef CLK_DEBUG
        log_hal_info("%s: %d: bit = %d: clock is disabled\n", __FUNCTION__, clock_id, bit_idx);
#endif
        return false;
    } else {
        /* TODO cannot use log_hal_info print log before syslog init */
#ifdef CLK_DEBUG
        log_hal_info("%s: %d: bit = %d: clock is enabled\n", __FUNCTION__, clock_id, bit_idx);
#endif
        return true;
    }
}

/*************************************************************************
 * Suspend/Resume API definition
 *************************************************************************/
ATTR_TEXT_IN_TCM hal_clock_status_t clock_suspend(void)
{
    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
#ifdef CLK_DEBUG_ERR
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
#endif
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }

    uint32_t irq_mask = save_and_set_interrupt_mask();          /* disable interrupt */
#if 0 // TODO, which CGs should be disabled here?
    clk_suspend_resume.cm_freq_saved = cm_freq_state;           /* save cm_freq_saved before suspend */
    pll_clock[SYSCLK].ops->save(&pll_clock[SYSCLK]);            /* save and force release pll request */
    pll_clock[UPLLCLK].ops->save(&pll_clock[UPLLCLK]);
    pll_clock[MPLLCLK].ops->save(&pll_clock[MPLLCLK]);
    if (is_sleep_with_26m) {
        clk_suspend_resume.sleep_with_26m = true;               /* set sleep_with_26m flag */
        cm_bus_clk_26m();                                       /* switch to 26m */
        if (cm_freq_state != CM_26M_STA) {
            EMI_DynamicClockSwitch(EMI_CLK_HIGH_TO_LOW);
            /* Set flash timing delay to 26MHz, switch SFC PLL to 26MHz DCXO or LFOSC */
            SF_DAL_DEV_SWITCH_TO_LOW_FQ();
        }
    }
#endif
    restore_interrupt_mask(irq_mask);                           /* restore interrupt */
    return HAL_CLOCK_STATUS_OK;
}

ATTR_TEXT_IN_TCM hal_clock_status_t clock_resume(void)
{
    if (!clock_initialized) {
        /* TODO cannot use log_hal_info print log before syslog init */
#ifdef CLK_DEBUG_ERR
        log_hal_error("%s: clock is uninitialized\n", __FUNCTION__);
#endif
        return HAL_CLOCK_STATUS_UNINITIALIZED;
    }

    uint32_t irq_mask = save_and_set_interrupt_mask();          /* disable interrupt */
#if 0 // TODO, which CGs should be enabled?
    if (clk_suspend_resume.sleep_with_26m) {
        if (clk_suspend_resume.cm_freq_saved == CM_208M_STA) {  /* check cm_freq_saved when resume */
            cm_bus_clk_208m();
            EMI_DynamicClockSwitch(EMI_CLK_LOW_TO_HIGH);
            SF_DAL_DEV_SWITCH_TO_HIGH_FQ();
        } else if (clk_suspend_resume.cm_freq_saved == CM_104M_STA) {
            cm_bus_clk_104m();
            EMI_DynamicClockSwitch(EMI_CLK_LOW_TO_HIGH);
            SF_DAL_DEV_SWITCH_TO_HIGH_FQ();
        } else {
            cm_bus_clk_26m();   /* CM_26M_STA */
        }
        clk_suspend_resume.sleep_with_26m = false;              /* clear sleep_with_26m flag */
    }

    pll_clock[SYSCLK].ops->restore(&pll_clock[SYSCLK]);         /* restore the pll request */
    pll_clock[UPLLCLK].ops->restore(&pll_clock[UPLLCLK]);
    pll_clock[MPLLCLK].ops->restore(&pll_clock[MPLLCLK]);
    hal_gpt_delay_us(100);
#endif
    restore_interrupt_mask(irq_mask);                           /* restore interrupt */
    return HAL_CLOCK_STATUS_OK;

}

/*
 * Funtion: Query frequency meter
 * tcksel: TESTED clock selection. 1: f_fxo_ck, 19: hf_fsys_ck.
 * fcksel: FIXED clock selection. 0: f_fxo_ck, 1: f_frtc_ck, 6: XOSC32K_CK.
 * return frequency unit: KHz
 */
ATTR_TEXT_IN_TCM uint32_t hal_clock_get_freq_meter(uint8_t tcksel, uint8_t fcksel)
{
    uint32_t target_freq = 0, tmp = 0;
    uint64_t target_freq_64 = 0;

    /* select fqmtr_ck by FQMTR_TCKSEL  bit[4:0], select fixed_ck by FQMTR_FCKSEL[2:0] */
    *CKSYS_TST_SEL_1_F_TCKSEL = tcksel;
    *CKSYS_TST_SEL_1_F_FCKSEL = fcksel;
#ifdef CLK_DEBUG
    log_hal_info("%s: CKSYS_TST_SEL_1=0x%x/0x%x \n\r", __func__, (*CKSYS_TST_SEL_1_F_FCKSEL), (*CKSYS_TST_SEL_1_F_TCKSEL));
#endif
    /* fqmtr_rst = 0, for divider, set winset. */
    *CKSYS_CLK_PLL_ABIST_FQMTR_CON0 = 0x4000; /* reset meter */
    *CKSYS_CLK_PLL_ABIST_FQMTR_CON0 = 0xFFF; /* set window */
    *CKSYS_CLK_PLL_ABIST_FQMTR_CON0 = (0x8000|0xFFF);

    hal_gpt_delay_us(5);

    tmp = *CKSYS_CLK_PLL_ABIST_FQMTR_CON1;
    while (tmp != 0) {
        tmp = *CKSYS_CLK_PLL_ABIST_FQMTR_CON1;
    }

    /* fqmtr_ck = fixed_ck*fqmtr_data/winset, */
    target_freq = (*CKSYS_CLK_PLL_ABIST_FQMTR_DATA);
    target_freq_64 = (uint64_t)target_freq;
    target_freq_64 = 26 * 10000 * target_freq_64 / 4096 / 10;
    target_freq = (uint32_t)target_freq_64;

#ifdef CLK_DEBUG
    log_hal_info("[PLL] %s: ABIST_FQMTR_CON2=0x%x, DATA=0x%x \n\r", __func__,
                 (*CKSYS_CLK_PLL_ABIST_FQMTR_CON2), (*CKSYS_CLK_PLL_ABIST_FQMTR_DATA));
#endif

    return target_freq;
}

/*
 * Funtion: Query frequency meter measurement cycles
 * tcksel: TESTED clock selection. 1: f_fxo_ck, 19: hf_fsys_ck.
 * fcksel: FIXED clock selection. 0: f_fxo_ck, 1: f_frtc_ck, 6: XOSC32K_CK.
 * winset: measurement window setting (number of fixed clock cycles)
 * return frequency unit: KHz
 */
ATTR_TEXT_IN_TCM uint32_t hal_clock_get_freq_meter_cycle(uint8_t tcksel, uint8_t fcksel, uint16_t winset)
{
    uint32_t cycles = 0, tmp = 0;

    /* select fqmtr_ck by FQMTR_TCKSEL  bit[4:0], select fixed_ck by FQMTR_FCKSEL[2:0] */
    *CKSYS_TST_SEL_1_F_TCKSEL = tcksel;
    *CKSYS_TST_SEL_1_F_FCKSEL = fcksel;
#ifdef CLK_DEBUG
    log_hal_info("%s: CKSYS_TST_SEL_1=0x%x/0x%x \n\r", __func__, (*CKSYS_TST_SEL_1_F_FCKSEL), (*CKSYS_TST_SEL_1_F_TCKSEL));
#endif
    /* fqmtr_rst = 0, for divider, set winset. */
    *CKSYS_CLK_PLL_ABIST_FQMTR_CON0 = 0x4000; /* reset meter */
    *CKSYS_CLK_PLL_ABIST_FQMTR_CON0 = winset; /* set window */
    *CKSYS_CLK_PLL_ABIST_FQMTR_CON0 = (0x8000|winset);

    hal_gpt_delay_us(5);

    tmp = *CKSYS_CLK_PLL_ABIST_FQMTR_CON1;
    while (tmp != 0) {
        tmp = *CKSYS_CLK_PLL_ABIST_FQMTR_CON1;
    }

    /* fqmtr_ck = fixed_ck*fqmtr_data/winset, */
    cycles = (*CKSYS_CLK_PLL_ABIST_FQMTR_DATA);

#ifdef CLK_DEBUG
    log_hal_info("[PLL] %s: ABIST_FQMTR_CON2=0x%x,DATA=0x%x \n\r", __func__,
                 (*CKSYS_CLK_PLL_ABIST_FQMTR_CON2), (*CKSYS_CLK_PLL_ABIST_FQMTR_DATA));
#endif

    return cycles;
}

bool hal_clock_fxo_is_26m(void)
{
    return fxo_is_26m;
}

ATTR_TEXT_IN_TCM bool EFUSE_Desense(void)
{
    return false;
}

/* Enable PLL and switch clock to PLL
   Note, the data and code MUST be placed at TCM!*/
ATTR_TEXT_IN_TCM void hal_clock_set_pll_dcm_init(void)
{
    uint32_t read_data;
    uint32_t rg_wf_bbpll_01_value, rg_wf_bbpll_02_value;

    if ((*XO_PDN_COND0 & 0x1) == 0) {
        // BBPLL enabled
        return;
    }

    /* 1. Sequence to enable PLL*/

    /* check EFUSE valid bit, if valid, fill PLL process related parameter*/
    if (EFUSE_Desense()) {
        *RF_WBAC0 = 0x10000; //0xA2040300, bit[16]=1, PLL1: 1040MHz, PLL2: 960MHz
    } else {
        *RF_WBAC0 = 0x0; //0xA2040300, bit[16]=0, PLL1: 832MHz, PLL2: 960MHz
    }


    if ((*RG_WF_BBPLL_01 & 0x1) == 0x1) {
        read_data = *RO_RF_WBAC1;
        rg_wf_bbpll_01_value = *RG_WF_BBPLL_01;
        *RF_WBAC1 =(read_data & ~0xFF00E0) | ((rg_wf_bbpll_01_value & 0xE) <<4) | ((rg_wf_bbpll_01_value & 0xF0) <<12) | ((rg_wf_bbpll_01_value & 0xF00) <<12);

        read_data = *RO_RF_WBAC2;
        *RF_WBAC2 =(read_data & ~0xFF00E0) | ((rg_wf_bbpll_01_value & 0x7000) >>7) | ((rg_wf_bbpll_01_value & 0x78000) <<1) | ((rg_wf_bbpll_01_value & 0x780000) <<1);

        if (*CKSYS_XTAL_FREQ_F_F_FXO_IS_26M ==0) {
            *RF_WBAC3 = 0xC00001FA;
        } else {
            *RF_WBAC3 = 0x000000FA;
        }
        *RF_WBAC28__F_CR_WBTAC_BBPLL_SW=0x1;
    }

    if (((*RG_WF_BBPLL_01) & 0x800000) == 0x800000) {
        read_data = *RF_WBAC27;
        rg_wf_bbpll_01_value = *RG_WF_BBPLL_01;
        rg_wf_bbpll_02_value = *RG_WF_BBPLL_02;
        *RF_WBAC27 =(read_data & ~0x7FF8FF) | ((rg_wf_bbpll_01_value & 0xF000000) >>20) | ((rg_wf_bbpll_01_value & 0xF0000000) >>28) |
            ((rg_wf_bbpll_02_value & 0x7) <<17) | ((rg_wf_bbpll_02_value) <<17) | ((rg_wf_bbpll_02_value) <<5) | ((rg_wf_bbpll_02_value & 0xE00) <<5);

        read_data = *RF_WBAC24;
        *RF_WBAC24 =(read_data & ~0x60) | ((rg_wf_bbpll_02_value & 0x3000) >>7);
    }

    *XO_PDN_CLRD0 = 0x1; //0xA2030B20, bit[0] set to 1 to enable bbpll controller clock
    // wait 20us for bbpll controller init
    hal_gpt_delay_us(20);
    if (EFUSE_Desense()) {
        *RF_WBAC0 = 0x3010003; //0xA2040300, bit [25:24] & [1:0] set to 1 to enable conn/spm pll request for PLL1 & 2, PLL1: 1040MHz, PLL2: 960MHz
    } else {
        if (*CKSYS_XTAL_FREQ_F_F_FXO_IS_26M ==0) {
            *RF_WBAC0 = 0x1000001; //0xA2040300, bit [24] & [0] set to 1 to enable conn/spm pll request for PLL2, PLL1: off, PLL2: 960MHz
        } else {
            *RF_WBAC0 = 0x3000003; //0xA2040300, bit [25:24] & [1:0] set to 1 to enable conn/spm pll request for PLL1 & 2, PLL1: 832MHz, PLL2: 960MHz
        }
    }
    // wait 130us for PLL stable
    hal_gpt_delay_us(130);

    /* 2. Enable clock divider */
    *CKSYS_CLK_DIV_1_F_CLK_PLL2_D5_EN = 0x1;   //0xA2020286, to enable digital frequency divider
    *CKSYS_CLK_DIV_1_F_CLK_PLL2_D3_EN = 0x1;   //0xA2020285, to enable digital frequency divider
    *CKSYS_CLK_DIV_2_F_CLK_PLL2_DIV_EN = 0x1;   //0xA2020289, to enable digital frequency divider

    /* 3. Set clock mux. force_on is 1 after reset.*/
    *CKSYS_CLK_CFG_0_F_CLK_SYS_SEL = 0x1; //0xA2020240, SYS clock @ 192MHz, BBPLL2_D5.

    *CKSYS_CLK_UPDATE_0_F_CHG_SYS = 0x1; //0xA2020250, SYS clock switch
    while (*CKSYS_CLK_UPDATE_0_F_CHG_SYS ==1);
    while (*CKSYS_CLK_UPDATE_STATUS_0_F_CHG_SYS_OK ==1);
    *CKSYS_CLK_FORCE_ON_0_F_CLK_SYS_FORCE_ON = 0;

    *CKSYS_CLK_CFG_0_F_CLK_SFC_SEL = 0x1; //0xA2020241, SFC clock @ 80MHz, BBPLL2_D3_D4.

    *CKSYS_CLK_UPDATE_0_F_CHG_SFC = 0x1; //0xA2020251, SFC clock switch
    while (*CKSYS_CLK_UPDATE_0_F_CHG_SFC ==1);
    while (*CKSYS_CLK_UPDATE_STATUS_0_F_CHG_SFC_OK ==1);
    *CKSYS_CLK_FORCE_ON_0_F_CLK_SFC_FORCE_ON = 0;

    *CKSYS_CLK_CFG_0_F_CLK_CONN_SEL = 0x1; //0xA2020242, CONN clock @ 320MHz, BBPLL2_D3.

    *CKSYS_CLK_UPDATE_0_F_CHG_CONN = 0x1; //0xA2020252, CONN clock switch
    while (*CKSYS_CLK_UPDATE_0_F_CHG_CONN ==1);
    while (*CKSYS_CLK_UPDATE_STATUS_0_F_CHG_CONN_OK ==1);
    *CKSYS_CLK_FORCE_ON_0_F_CLK_CONN_FORCE_ON = 0;

    *CKSYS_CLK_CFG_0_F_CLK_SPIMST_SEL = 0x2; //0xA2020243, SPIMST clock @ 96MHz, BBPLL2_D5_D2.

    *CKSYS_CLK_UPDATE_0_F_CHG_SPIMST = 0x1; //0xA2020253, SPIMST clock switch
    while (*CKSYS_CLK_UPDATE_0_F_CHG_SPIMST ==1);
    while (*CKSYS_CLK_UPDATE_STATUS_0_F_CHG_SPIMST_OK ==1);
    *CKSYS_CLK_FORCE_ON_0_F_CLK_SPIMST_FORCE_ON = 0;

    *CKSYS_CLK_CFG_1_F_CLK_XTALCTL_SEL = 0x1; //0xA2020244, XTALCTL clock @ 40/26MHz, XO_BBTOP_CK.
    /* glitch free mux, no change bit required.*/

    *CKSYS_CLK_CFG_1_F_CLK_SDIOMST_SEL = 0x2; //0xA2020245, SDIOMST clock @ 48MHz, BBPLL2_D5_D4

    *CKSYS_CLK_UPDATE_1_F_CHG_SDIOMST = 0x1; //0xA2020254, SDIOMST clock switch
    while (*CKSYS_CLK_UPDATE_1_F_CHG_SDIOMST ==1);
    while (*CKSYS_CLK_UPDATE_STATUS_1_F_CHG_SDIOMST_OK ==1);
    *CKSYS_CLK_FORCE_ON_1_F_CLK_SDIOMST_FORCE_ON = 0;

    /* Wait 1 us to change from 26M to PLL clock*/
    hal_gpt_delay_us(1);

    *SFC_DCM_CON_0_F_RG_SFC_DCM_DBC_NUM = 0xFF;
    *SFC_DCM_CON_0_F_RG_SFC_DCM_DBC_EN = 0x1;
    *SFC_DCM_CON_1_F_RG_SFC_CLKOFF_EN = 0x1;
    *SFC_DCM_CON_0_F_RG_SFC_DCM_APB_SEL = 0x6;
    *SFC_DCM_CON_1_F_RG_SFC_DCM_APB_TOG = 0x1;

    *BUS_DCM_CON_1 = 0x101;
    *BUS_DCM_CON_0_F_RG_BUS_DCM_EN = 0x3;

    *CM4_DCM_CON_1 = 0x100;
    *XO_DCM_CON_1 = 0x101;

    *CLK_FREQ_SWCH_F_RG_PLLCK_SEL = 0x1;
    hal_gpt_delay_us(1);

    // Sequence to disable XO_32K (default on)
    //*CKSYS_CLK_DIV_3_F_CR_XO_DIV_32K_EN = 0x0;

    // Unmask top for WIFI on to bulk PWM mode
    read_data = *HW_MISC0;
    *HW_MISC0 = (read_data|0x4);
}   /* void clock_set_pll_dcm_init(void) */

void clock_dump_log(void)
{
#if 0
    /* fix IAR build warning: undefined behavior, the order of volatile accesses is undefined in this statement. */
    uint32_t reg_tmp0, reg_tmp1, reg_tmp2, reg_tmp3, reg_tmp4, reg_tmp5;

    reg_tmp0 = *PDN_COND0_F_PDR_COND0;
    reg_tmp1 = *XO_PDN_COND0;

    /* clock_dump_cg */
    log_hal_info("%s: PDN_COND0=0x%x, XO_PDN_COND0=0x%x\r\n",
                 __FUNCTION__, reg_tmp0, reg_tmp1);

    reg_tmp0 = *CKSYS_CLK_CFG_0_F_CLK_SYS_SEL;
    reg_tmp1 = *CKSYS_CLK_CFG_0_F_CLK_SFC_SEL;
    reg_tmp2 = *CKSYS_CLK_CFG_0_F_CLK_CONN_SEL;
    reg_tmp3 = *CKSYS_CLK_CFG_0_F_CLK_SPIMST_SEL;
    reg_tmp4 = *CKSYS_CLK_CFG_1_F_CLK_XTALCTL_SEL;
    reg_tmp5 = *CKSYS_CLK_CFG_1_F_CLK_SDIOMST_SEL;

    /* clock_dump_mux */
    log_hal_info("%s: (mux)CLK_SYS_SEL=0x%x, (mux)CLK_SFC_SEL=0x%x, (mux)CLK_CONN_SEL=0x%x, (mux)CLK_SPIMST_SEL=0x%x, (mux)CLK_XTALCTL_SEL=0x%x, (mux)CKSYS_CLK_CFG_1_F_CLK_SDIOMST_SEL=0x%x\r\n",
                 __FUNCTION__, reg_tmp0, reg_tmp1, reg_tmp2, reg_tmp3, reg_tmp4, reg_tmp5);

    /* clock_dump_divider */
    reg_tmp0 = *CKSYS_CLK_DIV_0_F_CLK_PLL1_D2_EN;
    reg_tmp1 = *CKSYS_CLK_DIV_0_F_CLK_PLL1_D3_EN;
    reg_tmp2 = *CKSYS_CLK_DIV_0_F_CLK_PLL1_D5_EN;
    reg_tmp3 = *CKSYS_CLK_DIV_0_F_CLK_PLL1_D7_EN;
    reg_tmp4 = *CKSYS_CLK_DIV_2_F_CLK_PLL1_D15_EN;
    reg_tmp5 = *CKSYS_CLK_DIV_2_F_CLK_PLL1_DIV_EN;
    log_hal_info("%s: PLL1 DIV EN(%d), D2(%d), D3(%d), D5(%d), D7(%d), D15(%d)\r\n", __FUNCTION__, reg_tmp5, reg_tmp0, reg_tmp1, reg_tmp2, reg_tmp3, reg_tmp4);

    reg_tmp0 = *CKSYS_CLK_DIV_1_F_CLK_PLL2_D2_EN;
    reg_tmp1 = *CKSYS_CLK_DIV_1_F_CLK_PLL2_D3_EN;
    reg_tmp2 = *CKSYS_CLK_DIV_1_F_CLK_PLL2_D5_EN;
    reg_tmp3 = *CKSYS_CLK_DIV_1_F_CLK_PLL2_D7_EN;
    reg_tmp4 = *CKSYS_CLK_DIV_2_F_CLK_PLL2_D15_EN;
    reg_tmp5 = *CKSYS_CLK_DIV_2_F_CLK_PLL2_DIV_EN;
    log_hal_info("%s: PLL2 DIV EN(%d), D2(%d), D3(%d), D5(%d), D7(%d), D15(%d)\r\n", __FUNCTION__, reg_tmp5, reg_tmp0, reg_tmp1, reg_tmp2, reg_tmp3, reg_tmp4);

    reg_tmp0 = *CKSYS_CLK_DIV_3;
    reg_tmp1 = *CKSYS_CLK_DIV_4;
    reg_tmp2 = *CKSYS_CLK_DIV_5;
    reg_tmp3 = *CKSYS_XTAL_FREQ;
    log_hal_info("%s: CLK_DIV_3=0x%x, CLK_DIV_4=0x%x, CLK_DIV_5=0x%x, CLK_XTAL_FREQ=0x%x\r\n",
                 __FUNCTION__, reg_tmp0, reg_tmp1, reg_tmp2, reg_tmp3);
#else
    return;
#endif
}

void clock_dump_info(void)
{
    clock_dump_log();
}

ATTR_TEXT_IN_TCM int clk_init(void)
{
    if (clock_initialized) {
        return 0;
    }

    fxo_is_26m = false;
    if (*CKSYS_XTAL_FREQ_F_F_FXO_IS_26M == 1) {
        fxo_is_26m = true;
    }

    clock_initialized = true;

    return 0;
}

ATTR_TEXT_IN_TCM hal_clock_status_t hal_clock_init(void)
{
    int32_t ret;

    ret = clk_init();

    hal_clock_disable(HAL_CLOCK_CG_SW_TRNG);
    hal_clock_disable(HAL_CLOCK_CG_CRYPTO);
    hal_clock_disable(HAL_CLOCK_CG_SDIOSLV);
    hal_clock_disable(HAL_CLOCK_CG_UART0);
    hal_clock_disable(HAL_CLOCK_CG_SW_SPM);
    hal_clock_disable(HAL_CLOCK_CG_SW_SEJ);
    hal_clock_disable(HAL_CLOCK_CG_SW_AUXADC);

    return (hal_clock_status_t)(ret);
}

//#define CLK_SWITCH_TEST
#ifdef CLK_SWITCH_TEST
void clock_switch_test(void)
{
    uint32_t freq = 0;
    uint32_t i = 0;

    for (i=1; i<32; i++) {
        if( ((i>=6)&&(i<=7)) || ((i>=11)&&(i<=12)) || ((i>=14)&&(i<=16) || ((i>=28)&&(i<=30)))) {
            continue;
        }
        freq = hal_clock_get_freq_meter(i, 0);
        log_hal_info("Freqency meter %d: %d\r\n", i, freq);
    }
}
#endif

//#define CLK_DCM_TEST
#define GPIO_CLKO_CTRL_A_F_CLKO_MODE0 ((volatile uint8_t*)(0xA2010000))
#define TOP_DEBUG ((volatile uint32_t*)(0xA2010060))

#ifdef CLK_DCM_TEST
void clock_dcm_test(void)
{
    uint32_t reg_value = 0;

    // GPIO2 aux. 7 for O:CLKO0
    hal_gpio_init(HAL_GPIO_2);
    hal_pinmux_set_function(HAL_GPIO_2, 7);

    // GPIO4 aux. 7 for DEBUGMON1:cm_clk_idle
    hal_gpio_init(HAL_GPIO_4);
    hal_pinmux_set_function(HAL_GPIO_4, 7);

    // GPIO9 aux. 7 for DEBUGMON6:emi_idle
    hal_gpio_init(HAL_GPIO_9);
    hal_pinmux_set_function(HAL_GPIO_9, 7);

    // GPIO8 aux. 7 for DEBUGMON5:xo_clk_idle
    hal_gpio_init(HAL_GPIO_8);
    hal_pinmux_set_function(HAL_GPIO_8, 7);

    // GPIO6 aux. 7 for DEBUGMON3:bus_clk_idle
    hal_gpio_init(HAL_GPIO_6);
    hal_pinmux_set_function(HAL_GPIO_6, 7);

    // GPIO12 aux. 7 for DEBUGMON9:sfc_idle
    hal_gpio_init(HAL_GPIO_12);
    hal_pinmux_set_function(HAL_GPIO_12, 7);

    // enable debug monitor
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 15; // 4'd15: hf_fcm_mclk_ck div4
    #if 0
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 14; // 4'd14: hf_fsys_emi2x_phy_hclk_ck div4
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 13; // 4'd13: f_fxo_mclk_ck div2
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 12; // 4'd12: f_fxo_hclk_ck div2
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 11; // 4'd11: hf_fbus_mclk_ck div2
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 10; // 4'd10: hf_fbus_hclk_ck div2
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 9; // 4'd9:  hf_fbus_emi1x_phy_hclk_ck div2
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 8; // 4'd8:  hf_fsfc_phy_hclk_ck div2
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 7; // 4'd7:  AD_XO_BBTOP_CK
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 6; // 4'd6:  AD_XPLL_CK
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 5; // 4'd5:  f_f40m_ck
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 4; // 4'd4:  f_fxo_2m_ck
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 3; // 4'd3:  f_f48m_ck
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 2; // 4'd2:  f_f24m_ck
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 1; // 4'd1:  f_fxo_ck
    *GPIO_CLKO_CTRL_A_F_CLKO_MODE0 = 0; // 4'd0:  f_frtc_clkout_ck
    #endif
    // enable debug monitor for idle signal
    *TOP_DEBUG = 0xC;


}
#endif

#endif /* HAL_CLOCK_MODULE_ENABLED */

