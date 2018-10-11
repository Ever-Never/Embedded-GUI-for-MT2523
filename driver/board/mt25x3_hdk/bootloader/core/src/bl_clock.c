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

#include "hal_platform.h"
#include "bl_gpt.h"
#include "bl_clock.h"
#include "bl_common.h"

#define K_LFOSC
#define K_HFOSC
/* #define VCORE_1P2V_PLL */
#define VCORE_1P3V_PLL

/* #define BL_USE_LFOSC */
#define CLK_BRING_UP
#define BL_CLOCK_OVER_78M

/* Temporarily used in cpu frequency monitor */
uint32_t bl_clock_freq_monitor(uint16_t tcksel, uint16_t fcksel, bool over78m)
{
    uint32_t target_freq = 0, tmp = 0;
    uint32_t bak_gpio30_mode3, bak_gpio30_dir0;

    /* select fqmtr_ck by FQMTR_TCKSEL  bit[4:0], select fixed_ck by FQMTR_FCKSEL[2:0] */
    *PLL_ABIST_FQMTR_CON1 = (tcksel << 0) | (fcksel << 8);
    bl_print(LOG_DEBUG, "%s: ABIST_FQMTR_CON1=%x \n\r",
         __func__, *PLL_ABIST_FQMTR_CON1);

    /* fqmtr_rst = 0, for divider, set winset=100 */
    *PLL_ABIST_FQMTR_CON0 = 0x0000CFFF; /* rest meter */
    bl_gpt_delay_us(2);
    *PLL_ABIST_FQMTR_CON0 = 0x00008FFF;

/* FQMTR_TCKSEL = 0x4, 0x6, 0xC etc. >78MHz clock need this define */
    if (over78m == true) {
        *PLL_ABIST_FQMTR_CON2 = 0x00000001;     /* fqmtr_ck div 4 */
        *PLL_ABIST_FQMTR_CON2 = 0x00000011;     /* enable div clock */
    } else
        *PLL_ABIST_FQMTR_CON2 = 0x00000000;     /* disable div clock */

    *PLL_SYS_ABIST_MON_CON1 = 0x00000201;   /* mon sel to mon_fq_mtr_ck */

    bak_gpio30_mode3 = (*BL_GPIO_MODE3 & 0x0F000000);       /* backup setting for gpio30 in mode3 */
    bak_gpio30_dir0 = (*BL_GPIO_DIR0 & 0x40000000);         /* backup setting for gpio30 in dir0 */

    *BL_TOP_DEBUG = 0x0000000E;             /* debug_sel to mixedsys_d output */
    *BL_GPIO_MODE3_CLR = 0x0F000000;        /* clear setting for gpio30 in mode3 */
    *BL_GPIO_MODE3_SET = 0x07000000;        /* GPIO30 select to debug mode */
    *BL_GPIO_DIR0_SET = 0x40000000;         /* DIR = output mode */

    tmp = *PLL_ABIST_FQMTR_CON1;
    tmp = *PLL_ABIST_FQMTR_CON1;
    while (tmp & (0x1 << 15)) {
        tmp = *PLL_ABIST_FQMTR_CON1;
    }

    bl_gpt_delay_us(240);
    /* fqmtr_ck = fixed_ck*fqmtr_data/winset, */
    /* ex. 312*0.96=26*fqmtr_data/100, expect fqmtr_data=0x480 */
    target_freq = (26 * *PLL_ABIST_FQMTR_DATA) / 4096;
    if (over78m == true)
        target_freq *= 4;

    bl_print(LOG_DEBUG, "%s: ABIST_FQMTR_CON2=%x,CON1=%x,DATA=%x \n\r", __func__,
         *PLL_ABIST_FQMTR_CON2, *PLL_ABIST_FQMTR_CON1, *PLL_ABIST_FQMTR_DATA);

    *BL_GPIO_MODE3_CLR = 0x0F000000;        /* clear setting for gpio30 in mode3 */
    *BL_GPIO_DIR0_CLR = 0x40000000;         /* clear setting for gpio30 in dir0 */
    *BL_GPIO_MODE3_SET = bak_gpio30_mode3;  /* restore setting for gpio30 in mode3 */
    *BL_GPIO_DIR0_SET = bak_gpio30_dir0;    /* restore setting for gpio30 in dir0 */

    return target_freq;
}

/* Standard Sequence for LFOSC Calibration, Enabling PLL and Switching Clock to PLL */
void clock_set_pll_dcm_init(void) /* This function is in TCM code section */
{
#if defined (K_LFOSC) || defined (K_HFOSC)
    volatile uint32_t tmp = 0, tmp_cali = 0, tmp_ft = 0;
    /* LFOSC calibration before set 28MHz, multiple 10000 for accuration */
    uint32_t while_count = 0, k_init = 0, k_before = 280000, k_after = 0;
    /* LFOSC upper_bound and lower_bound 24.96MHz and 24.75Mhz, multiple 10000 for accuration */
    uint32_t upper_bound = 249600, lower_bound = 247500;
    /* LFOSC desired_fqdata caculated by Target(26 * 0.96) / FIX_CK(26) / FQMTR_CLKDIV(1) * WINSET(4096) */
    uint32_t magic_before = 197017, magic_after = 197017, desired_fqdata = 3932;
    uint32_t start = 0, end = 0, sum = 0;
#endif
    uint32_t test_freq = 0;

    *BL_CLK_SOURCE_SEL = 0x00FFFFFF;   /* TODO: under check if it's ok? */
#if 0
    /* change back to DCXO 26MHz for system reset */
    *BL_CLK_CONDB = 0x3800000;
    *BL_CLK_CONDD = 0x10000FFF;         /* CONFG_BASE + 0x010C, bit 0~11 set to 1 to force clock on before clock change */
    *BL_ACFG_CLK_UPDATE = 0x0E7F;       /* ANA CFGSYS_BASE + 0x0150, set chg_clk */
    /* wait for 2us to change source from 26M to PLL clock */
    bl_gpt_delay_us(2);
#endif

    /* To release de-sense issue before enable LFOSC */
    *PLL_LFOSC_CON1 = 0xF; /* MIXED_BASE (0XA2040000)+ 0x0904, bit [7:4] set to 0 */

    /* enable LFOSC */
    *PLL_LFOSC_CON0 = 0x2801;   /* MIXED_BASE + 0x0900, bit 0 set to 1 */

    /* wait 5us for LFOSC settle (can be covered by UPLL) */
    /* bl_gpt_delay_us(2); */
    /* bl_gpt_delay_ns(500); */

    /* To release de-sense issue before enable HFOSC */
    *PLL_HFOSC_CON1 = 0xF; /* MIXED_BASE (0XA2040000)+ 0x0924, bit [7:4] set to 0 */

    /* enable HFOSC */
    *PLL_HFOSC_CON0 = 0x2801;   /* MIXED_BASE + 0x0920, bit 0 set to 1 */

    /* wait 5us for HFOSC settle (can be covered by UPLL) */
    /* bl_gpt_delay_us(5); */

    /* Sequence to enable CLKSQ/IV-Gen */
    *PLL_CLKSQ_CON0 = 0x1;                      /* MIXED_BASE + 0x0020, bit 0 set to 1'b1 to enable CLKSQ/IV-Gen of PLLGP */

//    bl_print(LOG_DEBUG, "[PLL] %s, PDN_COND2=0x%x \n\r", __func__, *BL_PDN_COND2);

    /* wait 3us for CLKSQ/IV-Gen stable */
    bl_gpt_delay_us(3);

    /* enable UPLL */
    *PLL_UPLL_CON1 = 0x4000;                    /* MIXED_BASE + 0x0140, bit [15:14] set to 2'b01 to output UPLL 312MHz */

    /* enable and reset UPLL */
    *PLL_UPLL_CON0 = 0x1801;                    /* MIXED_BASE + 0x0140, bit 0 set to 1 to enable UPLL */

    /* wait 20us for UPLL stable */
//    bl_print(LOG_DEBUG, "[PLL] %s: wait UPLL stable \n\r",__func__);
    bl_gpt_delay_us(20);

#ifdef K_LFOSC
    test_freq = bl_clock_freq_monitor(11, 0, CLK_LOWER_78M);
    bl_print(LOG_DEBUG, "LFOSC freq before K = %d\n\r", test_freq);

    while(1) {
        bl_gpt_get_free_run_count(BL_GPT_CLOCK_SOURCE_1M, &start);
        *PLL_ABIST_FQMTR_CON2 = 0x0000; /* disable fqmtr_ck div */
        *PLL_ABIST_FQMTR_CON1 = 0x000B;
        *PLL_ABIST_FQMTR_CON0 = 0xCFFF;
        *PLL_ABIST_FQMTR_CON0 = 0x8FFF;
        tmp = *PLL_ABIST_FQMTR_CON1;
        tmp = *PLL_ABIST_FQMTR_CON1;
        while (tmp & (0x1 << 15)) {
            tmp = *PLL_ABIST_FQMTR_CON1;
        }
        tmp = *PLL_ABIST_FQMTR_DATA;

        /* Freq = FQMTR_DATA(tmp) * FIX_CK(26) * FQMTR_CLKDIV(1) * For_Accurate_Count(10000) / WINSET(4096) */
        k_after = tmp * 26 * 10000 / 4096;

        if (while_count == 0)
            k_init = k_after;

        if (((k_after >= lower_bound) && (k_after <= upper_bound)) || (while_count >= 25)) {
            bl_gpt_get_free_run_count(BL_GPT_CLOCK_SOURCE_1M, &end);
            sum = sum + end - start;
            bl_print(LOG_DEBUG, "[LFOSC] Before K = %d, After K = %d\n\r", k_init, k_after);
            if (while_count >= 25)
                bl_print(LOG_DEBUG, "[LFOSC] calibration failed\n\r");
            //bl_print(LOG_DEBUG, "[%d] calibration cost %d us, tmp = %d, magic_before = %d,
            //magic_after = %d\n\r", while_count, end - start, tmp, magic_before, magic_after);
            break;
        } else if ((k_before > upper_bound) && (k_after < lower_bound))
            magic_after = ((magic_before + magic_after) / 2) - tmp;
        else if ((k_before < lower_bound) && (k_after > upper_bound))
            magic_after = ((magic_before + magic_after) / 2) + tmp;
        else
            magic_after = magic_before * desired_fqdata / tmp;


        if ((magic_after - (10 * tmp))  > 0) {
            tmp_cali = (magic_after - 10 * tmp) / tmp;
            tmp_ft = ((magic_after - 10 * tmp) - tmp_cali * tmp) / ( tmp * 6 / 100);
        } else {
            tmp_cali = (10 * tmp - magic_after) / tmp;
            tmp_ft = ((10 * tmp - magic_after) - tmp_cali * tmp) / (tmp * 6 / 100);
        }

        *PLL_LFOSC_CON0 = (*PLL_LFOSC_CON0 & 0xC0FF) | (tmp_cali << 8);
        *PLL_LFOSC_CON0 = (*PLL_LFOSC_CON0 & 0xFF0F) | (tmp_ft << 4);

        bl_gpt_delay_us(6);
        bl_gpt_get_free_run_count(BL_GPT_CLOCK_SOURCE_1M, &end);
        sum = sum + end - start;
        //bl_print(LOG_DEBUG, "[%d] calibration cost %d us, cali = %d, ft = %d, k_before = %d, k_after = %d, magic_before = %d,
        //magic_after = %d, tmp = %d\n\r", while_count, end - start, tmp_cali, tmp_ft, k_before, k_after, magic_before, magic_after, tmp);
        magic_before = magic_after;
        k_before = k_after;
        while_count++;
    }
    test_freq = bl_clock_freq_monitor(4, 3, CLK_OVER_78M);
    bl_print(LOG_DEBUG, "UPLL freq measured by LFOSC = %d\n\r", test_freq);
    test_freq = bl_clock_freq_monitor(11, 0, CLK_LOWER_78M);
    bl_print(LOG_DEBUG, "LFOSC freq after K= %d, K %d times, total %d us\n\r", test_freq, while_count + 1, sum);
#endif /* ifdef K_LFOSC */

#ifdef K_HFOSC
    test_freq = bl_clock_freq_monitor(12, 0, CLK_OVER_78M);
    bl_print(LOG_DEBUG, "HFOSC freq before K = %d\n\r", test_freq);

    /* HFOSC calibration before set 340MHz, multiple 1000 for accuration */
    while_count = 0, k_init = 0, k_before = 340000, k_after = 0;
    /* HFOSC upper_bound and lower_bound 293.28MHz and 280.8Mhz, multiple 1000 for accuration */
    upper_bound = 293280, lower_bound = 280800;
    /* HFOSC desired_fqdata caculated by Target(312 * 0.92) / FIX_CK(26) / FQMTR_CLKDIV(4) * WINSET(4096) */
    magic_before = 565248, magic_after = 565248, desired_fqdata = 11304;
    start = 0, end = 0, sum = 0;

    while (1) {
        bl_gpt_get_free_run_count(BL_GPT_CLOCK_SOURCE_1M, &start);
        /* HFOSC Calibration */
        /* select HFOSC as fqmtr_ck and enable FQMTR */
        *PLL_ABIST_FQMTR_CON2 = 0x0001; /* MIXED_BASE (0XA2040000)+ 0x0408, bit [1:0], set to 1 to divide 4 */
        *PLL_ABIST_FQMTR_CON2 = 0x0011; /* MIXED_BASE (0XA2040000)+ 0x0408, bit [4], set to 1 to enable divider */
        *PLL_ABIST_FQMTR_CON1 = 0x000C;     /* MIXED_BASE + 0x0404, bit [4:0], set to 0 to 12, AD_HFOSC_CK */
        *PLL_ABIST_FQMTR_CON0 = 0xCFFF;     /* MIXED_BASE + 0x0400, Reset FQMTR, Winset = 400 */
        *PLL_ABIST_FQMTR_CON0 = 0x8FFF;     /* MIXED_BASE + 0x0400, Enable FQMTR, Winset = 400 */

        /* read *PLL_ABIST_FQMTR_CON1 */
        /* *PLL_ABIST_FQMTR_CON1 [15] will change as 0->1->0 . a dummy read here so that we don't poll 1st 0 and exit, polling until *PLL_ABIST_FQMTR_CON1 [15] ==0 */
        tmp = *PLL_ABIST_FQMTR_CON1;
        tmp = *PLL_ABIST_FQMTR_CON1;
        while (tmp & (0x1 << 15)) {
            tmp = *PLL_ABIST_FQMTR_CON1;
        }
        tmp = *PLL_ABIST_FQMTR_DATA;    /* MIXED_BASE + 0x040C, read fqmtr_data */
        bl_gpt_delay_us(6);

        /* Freq = FQMTR_DATA(tmp) * FIX_CK(26) * FQMTR_CLKDIV(4) * For_Accurate_Count(1000) / WINSET(4096) */
        k_after = tmp * 26 * 1000 * 4 / 4096;

        if (while_count == 0)
            k_init = k_after;

        if (((k_after >= lower_bound) && (k_after <= upper_bound)) || (while_count >= 25)) {
            bl_gpt_get_free_run_count(BL_GPT_CLOCK_SOURCE_1M, &end);
            sum = sum + end - start;
            bl_print(LOG_DEBUG, "[HFOSC] Before K = %d, After K = %d\n\r", k_init, k_after);
            if (while_count >= 25)
                bl_print(LOG_DEBUG, "[HFOSC] calibration failed\n\r");
            //bl_print(LOG_DEBUG, "[%d] calibration cost %d us, tmp = %d, magic_before = %d,
            //magic_after = %d\n\r", while_count, end - start, tmp, magic_before, magic_after);
            break;
        } else if ((k_before > upper_bound) && (k_after < lower_bound))
            magic_after = ((magic_before + magic_after) / 2) - tmp;
        else if ((k_before < lower_bound) && (k_after > upper_bound))
            magic_after = ((magic_before + magic_after) / 2) + tmp;
        else
            magic_after = magic_before / 100 * desired_fqdata / tmp * 100; /* Divide and Multiple 100 to avoid overflow */

        /* Derive RG_HFOSC_CALI & RG_HFOSC_FT, AD_HFOSC_CK = tmp / 100 * 26M */
        /* Target Freq: 312M * 0.96 = AD_HFOSC_CK * (1 + (RG_HFOSC_CALI - 6'd40) * 0.02 + RG_HFOSC_FT * 0.0012) */
        if (magic_after - 10 * tmp > 0) {
            tmp_cali = (magic_after - 10 * tmp) / tmp;
            tmp_ft = ((magic_after - 10 * tmp) - tmp_cali * tmp) / (tmp * 6 / 100);
        } else {
            tmp_cali = (10 * tmp - magic_after) / tmp;
            tmp_ft = ((10 * tmp - magic_after) - tmp_cali * tmp) / (tmp * 6 / 100);
        }

        /* MIXED_BASE + 0x0920, bit [13:8] = RG_HFOSC_CALI */
        *PLL_HFOSC_CON0 = (*PLL_HFOSC_CON0 & 0xC0FF) | (tmp_cali << 8);

        /* MIXED_BASE + 0x0920, bit [7:4] = RG_HFOSC_FT */
        *PLL_HFOSC_CON0 = (*PLL_HFOSC_CON0 & 0xFF0F) | (tmp_ft << 4);

        bl_gpt_delay_us(6);
        bl_gpt_get_free_run_count(BL_GPT_CLOCK_SOURCE_1M, &end);
        sum = sum + end - start;
        //bl_print(LOG_DEBUG, "[%d] calibration cost %d us, cali = %d, ft = %d, k_before = %d, k_after = %d, magic_before = %d,
        //magic_after = %d, tmp = %d\n\r", while_count, end - start, tmp_cali, tmp_ft, k_before, k_after, magic_before, magic_after, tmp);
        magic_before = magic_after;
        k_before = k_after;
        while_count++;
    }
    test_freq = bl_clock_freq_monitor(12, 0, CLK_OVER_78M);
    bl_print(LOG_DEBUG, "HFOSC freq after K= %d, K %d times, total %d us\n\r", test_freq, while_count + 1, sum);
#endif /* ifdef K_HFOSC */

    /* Set TOPSM related clock source selection */
#ifdef VCORE_1P3V
    *BL_CLK_SOURCE_SEL = 0x003BB888;   /* CONFG_BASE + 0x0180 */
#elif defined(VCORE_1P2V_PLL) || defined(VCORE_1P3V_PLL)
    *BL_CLK_SOURCE_SEL = 0x00FFFFFF;   /* CONFG_BASE + 0x0180 */
#elif defined VCORE_0P9V
#ifdef BL_USE_LFOSC
    *BL_CLK_SOURCE_SEL = 0x00444444;   /* CONFG_BASE + 0x0180 */
#else
    *BL_CLK_SOURCE_SEL = 0x00111111;   /* CONFG_BASE + 0x0180 */
#endif  /* #ifdef BL_USE_LFOSC */
#else /* VCORE_1P1V */
    *BL_CLK_SOURCE_SEL = 0x00899888;   /* CONFG_BASE + 0x0180 */
#ifdef BL_USE_LFOSC
    *BL_CLK_SOURCE_SEL |= 0x00044000;  /* CONFG_BASE + 0x0180 */
#endif  /* #ifdef BL_USE_LFOSC */
#endif

    /* enable HW mode TOPSM control and clock CG of PLL control */
    *PLL_PLL_CON2 = 0x0000;         /* MIXED_BASE + 0x0048, to enable PLL TOPSM control and clock CG of controller */
    *PLL_PLL_CON3 = 0x0000;         /* MIXED_BASE + 0x004C, to enable DCXO 26M TOPSM control and clock CG of controller */

    /* enable delay control */
    *PLL_PLLTD_CON0= 0x0000;        /* MIXED_BASE + 0x0700, bit 0 set to 0 to enable delay control */

    /* wait for 1us for TOPSM and delay (HW) control signal stable */
    bl_gpt_delay_us(1);

    /* select MPLL frequency */
#if 0 /* prevent to stop MPLL here since it will hung if system reset */
    *PLL_MPLL_CON0 = 0x1800;        /* MIXED_BASE + 0x0100, set MPLL = 624M */
#endif
    *PLL_MPLL_CON1 = 0x0000;        /* MIXED_BASE + 0x0104, set post divider = /1 */

    /* enable PLL_PGDET */
    *PLL_PLL_CON4 = 0x1204;         /* MIXED_BASE + 0x0050, bit 2 set to 1 to enable PGDET of MDDS */

    /* enable MDDS */
    *PLL_MDDS_CON0 = 0x0001;        /* MIXED_BASE + 0x0640, bit 0 set to 1 to enable MDDS */

    /* enable and reset MPLL */
    *PLL_MPLL_CON0 = 0x1801;        /* MIXED_BASE + 0x0100, bit 0 set to 1 to enable MPLL and generate reset of MPLL */

    /* wait 50us for PLL and DDS settle */
    bl_gpt_delay_us(50);

/* Sequence for DCM setting is as below: */
#ifndef CLOCK_26M
#ifndef DCM_DIS
#ifdef DCM_DBC

    *BL_CM4_DCM_CON |= 0x220000;        /* CM_MEMS_DCM_BASE +0x0150, dbc 2T */
    *BL_MEMS_DCM_CON |= 0x220000;       /* CM_MEMS_DCM_BASE +0x0154, dbc 2T */
    *BL_CLK_CONDF |= 0x220000;          /* CONFG_BASE + 0x0114, PD_BUS DCM, dbc 2T */
    *BL_CLK_CONDG |= 0x220000;          /* CONFG_BASE + 0x0118, AO_BUS DCM, dbc 2T */

#endif  /* ifdef DCM_DBC */

    *BL_CM4_DCM_CON |= 0x01C00202;      /* CM_MEMS_DCM_BASE +0x0150 */
    *BL_MEMS_DCM_CON |= 0x01C00603;     /* CM_MEMS_DCM_BASE +0x0154 */

#ifdef DCM_FREE_SEL /* need to set before enter VCORE_0P9V */

    /* CM_FREE_DCM_CON = 0x8; */        /* CM_MEMS_DCM_BASE + 0x0158, CM FREE DCM */
    *BL_CLK_CONDH = 0x8;                /* CONFG_BASE + 0x11C, BUS FREE DCM */

#endif  /* ifdef DCM_FREE_SEL */

    *BL_CLK_CONDF = (*BL_CLK_CONDF & ~0xFC00) | 0x00000603;   /* CONFG_BASE + 0x0114, PD_BUS DCM, idle clock 62.4M / 32 */
    *BL_CLK_CONDG = (*BL_CLK_CONDG & ~0xFC00) | 0x00000603;   /* CONFG_BASE + 0x0118, AO_BUS DCM, idle clock 62.4M / 32 */

#endif  /* ifndef DCM_DIS */

    /* enable clock slow down when idle */
    *BL_ACFG_FREQ_SWCH = 0x0001;        /* ANA CFGSYS_BASE + 0x0154, set rg_pllck_sel = 1'b1 */

/* Sequence to enable PLL/HFOSC divider is as below: */
#ifdef VCORE_1P3V
    *BL_CLK_CONDA = 0x40B26;            /* CONFG_BASE + 0x0100 */
#elif defined(VCORE_1P2V_PLL) || defined(VCORE_1P3V_PLL)
    *BL_CLK_CONDA = 0xFFFFFFFF;         /* CONFG_BASE + 0x0100 */
#else /* VCORE_1P1V */
    *BL_CLK_CONDA = 0xB2D;              /* CONFG_BASE + 0x0100 */
#endif /* ifdef VCORE_1P3V */

    *BL_CLK_CONDB = 0x83800000;         /* CONFG_BASE + 0x0104, bit 31 set to 1 to enable digital frequency divider */

    /* wait for 1us to enable divider */
    bl_gpt_delay_us(1);

    /* Sequence to switch to PLL/HFOSC clocks as below: */
    *BL_CLK_CONDD = 0x10000FFF;         /* CONFG_BASE + 0x010C, bit 0~11 set to 1 to force clock on before clock change */

#ifdef VCORE_1P3V
    *BL_CLK_CONDB = 0xE389402D;         /*CONFG_BASE + 0x0104 */
    /*  DSP_MUX_SEL, 6: HFOSC_DIV2_CK (156MHz)
        DISP_PWM_MUX_SEL, 0: DCXO 26M
        USB_MUX_SEL, 3: UPLL_F62M_CK
        CAM_MUX_SEL, 2: UPLL_48M_CK
        GP_F26M_GFMUX_SEL, 0: DCXO 26M
        LP_F26M_GFMUX_SEL, 0: DCXO 26M
        BSI_MUX_SEL, 4: HFOSC_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SLCD_MUX_SEL, 5: HFOSC_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SFC_MUX_SEL, 0: DCXO 26M
        CM_MUX_SEL, 5: MPLL_F208M_CK
        BUS_MUX_SEL, 5: HFOSC_DIV5_CK (62.4MHz) */
#elif defined  VCORE_1P3V_PLL
    *BL_CLK_CONDB = 0xC384802B;         /* CONFG_BASE + 0x0104 */
    /*  DSP_MUX_SEL, 4: MPLL_DIV2P5_CK (none 50-50 duty 124.8MHz)
        DISP_PWM_MUX_SEL, 0: CLKSQ_F26M_CK
        USB_MUX_SEL, 3: UPLL_F62M_CK
        CAM_MUX_SEL, 2: UPLL_48M_CK
        GP_F26M_GFMUX_SEL, 0: DCXO
        LP_F26M_GFMUX_SEL, 0: DCXO
        BSI_MUX_SEL, 2: MPLL_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SLCD_MUX_SEL, 2: MPLL_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SFC_MUX_SEL, 0: CLKSQ_F26M_CK
        CM_MUX_SEL, 5: MPLL_F208M_CK (208MHz)
        BUS_MUX_SEL, 3: MPLL_DIV5_CK (62.4MHz) */
#elif defined  VCORE_1P2V_PLL
    *BL_CLK_CONDB = 0xC384801B;         /* CONFG_BASE + 0x0104 */
    /*  DSP_MUX_SEL, 4: MPLL_DIV2P5_CK (none 50-50 duty 124.8MHz)
        DISP_PWM_MUX_SEL, 0: CLKSQ_F26M_CK
        USB_MUX_SEL, 3: UPLL_F62M_CK
        CAM_MUX_SEL, 2: UPLL_48M_CK
        GP_F26M_GFMUX_SEL, 0: DCXO
        LP_F26M_GFMUX_SEL, 0: DCXO
        BSI_MUX_SEL, 2: MPLL_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SLCD_MUX_SEL, 2: MPLL_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SFC_MUX_SEL, 0: CLKSQ_F26M_CK
        CM_MUX_SEL, 3: MPLL_DIV3_CK (104MHz)
        BUS_MUX_SEL, 3: MPLL_DIV5_CK (62.4MHz) */
#elif defined  VCORE_0P9V
#ifdef BL_USE_LFOSC
    *BL_CLK_CONDB = 0x17B24489;         /* CONFG_BASE (0XA2010000)+ 0x0104 */
    /*  DSP_MUX_SEL, 1: LFOSC
        DISP_PWM_MUX_SEL, 1: LFOSC
        USB_MUX_SEL, 3: UPLL_F62M_CK
        CAM_MUX_SEL, 2: UPLL_48M_CK
        GP_F26M_GFMUX_SEL, 1: LFOSC
        LP_F26M_GFMUX_SEL, 1: LFOSC
        BSI_MUX_SEL, 1: LFOSC
        SLCD_MUX_SEL, 1: LFOSC
        SFC_MUX_SEL, 1: LFOSC
        CM_MUX_SEL, 1: LFOSC
        BUS_MUX_SEL, 1: LFOSC
        */
#else
    *BL_CLK_CONDB = 0x03800000;         /* CONFG_BASE (0XA2010000)+ 0x0104 */
    /*  DSP_MUX_SEL, 1: DCXO
        DISP_PWM_MUX_SEL, 1: DCXO
        USB_MUX_SEL, 3: UPLL_F62M_CK
        CAM_MUX_SEL, 2: UPLL_48M_CK
        GP_F26M_GFMUX_SEL, 1: DCXO
        LP_F26M_GFMUX_SEL, 1: DCXO
        BSI_MUX_SEL, 1: DCXO
        SLCD_MUX_SEL, 1: DCXO
        SFC_MUX_SEL, 1: DCXO
        CM_MUX_SEL, 1: DCXO
        BUS_MUX_SEL, 1: DCXO */
#endif  /* #ifdef BL_USE_LFOSC */
#else /* VCORE_1P1V*/
    *BL_CLK_CONDB = 0xF3894025;         /* CONFG_BASE + 0x0104 */
    /*  DSP_MUX_SEL, 7: HFOSC_DIV2P5_CK (none 50-50 duty 124.8MHz)
        DISP_PWM_MUX_SEL, 0: DCXO
        USB_MUX_SEL, 3: UPLL_F62M_CK
        CAM_MUX_SEL, 2: UPLL_48M_CK
        GP_F26M_GFMUX_SEL, 0: DCXO
        LP_F26M_GFMUX_SEL, 0: DCXO
        BSI_MUX_SEL, 4: HFOSC_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SLCD_MUX_SEL, 5: HFOSC_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SFC_MUX_SEL, 0: DCXO
        CM_MUX_SEL, 4: HFOSC_DIV3_CK (104MHz)
        BUS_MUX_SEL, 5: HFOSC_DIV5_CK (62.4MHz) */
#endif
#ifdef BL_USE_LFOSC
    /* *BL_PDN_SETD1 |= 0x2000; */
    *BL_CLK_CONDB |= 0x04300000;        /* CONFG_BASE (0XA2010000)+ 0x0104 */
        /* [27:26]: DISP_PWM_MUX_SEL, 01: LFOSC */
        /* [21]: GP_F26M_GFMUX_SEL, 1: LFOSC */
        /* [20]: LP_F26M_GFMUX_SEL, 1: LFOSC */
    bl_gpt_delay_us(2);
    /* *BL_PDN_CLRD1 |= 0x2000; */
#endif /* #ifdef BL_USE_LFOSC */

    *BL_ACFG_CLK_UPDATE = 0x0E7F;       /* ANA CFGSYS_BASE + 0x0150, set chg_clk */

    /* wait for 2us to change source from 26M to PLL clock */
    bl_gpt_delay_us(2);

    *BL_CLK_CONDD = 0x10000000;         /* CONFG_BASE + 0x010C, bit 0~11 set to 0 to release force clock on after clock change */
    *BL_SLEEP_COND = 0xC;               /* CONFG_BASE + 0x0200, bit [3:2] set 2'b11 to turn on clock switch clock by request */
    *BL_PDN_SETD0 = 0x300000;           /* CONFG_BASE + 0x0310, bit [21:20] set 2'b11 to turn on clock switch clock by request */

#endif /* ifdef CLOCK_26M */

}   /* void clock_set_pll_dcm_init(void) */

void clock_set_pll_init4flash_tool(void) /* This function is in TCM code section */
{
    /* enable HW mode TOPSM control and clock CG of PLL control */
    *PLL_PLL_CON2 = 0x0000;         /* MIXED_BASE + 0x0048, to enable PLL TOPSM control and clock CG of controller */
    *PLL_PLL_CON3 = 0x0000;         /* MIXED_BASE + 0x004C, to enable DCXO 26M TOPSM control and clock CG of controller */

    /* enable delay control */
    *PLL_PLLTD_CON0= 0x0000;        /* MIXED_BASE + 0x0700, bit 0 set to 0 to enable delay control */

    /* wait for 1us for TOPSM and delay (HW) control signal stable */
    bl_gpt_delay_us(1);

    /* Sequence to enable CLKSQ/IV-Gen */
    *PLL_CLKSQ_CON0 = 0x1;                      /* MIXED_BASE + 0x0020, bit 0 set to 1'b1 to enable CLKSQ/IV-Gen of PLLGP */

    /* enable and reset UPLL */
    *PLL_UPLL_CON0 = 0x1801;                    /* MIXED_BASE + 0x0140, bit 0 set to 1 to enable UPLL */

    /* Set TOPSM related clock source selection */
#ifdef VCORE_1P3V
    *BL_CLK_SOURCE_SEL = 0x003FF888;   /* CONFG_BASE + 0x0180 */
#elif defined(VCORE_1P2V_PLL) || defined(VCORE_1P3V_PLL)
    *BL_CLK_SOURCE_SEL = 0x00FFFFFF;   /* CONFG_BASE + 0x0180 */
#elif defined VCORE_0P9V
    *BL_CLK_SOURCE_SEL = 0x00444444;   /* CONFG_BASE + 0x0180 */
#else /* VCORE_1P1V */
    *BL_CLK_SOURCE_SEL = 0x008CC888;   /* CONFG_BASE + 0x0180 */
#endif

    /* enable PLL_PGDET */
    *PLL_PLL_CON4 = 0x1204;         /* MIXED_BASE + 0x0050, bit 2 set to 1 to enable PGDET of MDDS */

    /* enable MDDS */
    *PLL_MDDS_CON0 = 0x0001;        /* MIXED_BASE + 0x0640, bit 0 set to 1 to enable MDDS */

    /* enable and reset MPLL */
    *PLL_MPLL_CON0 = 0x1801;        /* MIXED_BASE + 0x0100, bit 0 set to 1 to enable MPLL and generate reset of MPLL */

    /* wait 50us for PLL and DDS settle */
    bl_gpt_delay_us(50);

    /* enable clock slow down when idle */
    *BL_ACFG_FREQ_SWCH = 0x0001;        /* ANA CFGSYS_BASE + 0x0154, set rg_pllck_sel = 1'b1 */

/* Sequence to enable PLL/HFOSC divider is as below: */
#ifdef VCORE_1P3V
    *BL_CLK_CONDA = 0x40B26;         /* CONFG_BASE + 0x0100 */
#elif defined(VCORE_1P2V_PLL) || defined(VCORE_1P3V_PLL)
    *BL_CLK_CONDA = 0xFFFFFFFF;         /* CONFG_BASE + 0x0100 */
#else /* VCORE_1P1V */
    *BL_CLK_CONDA = 0xB2D;         /* CONFG_BASE + 0x0100 */
#endif /* ifdef VCORE_1P3V */

    *BL_CLK_CONDB = 0x83800000;         /* CONFG_BASE + 0x0104, bit 31 set to 1 to enable digital frequency divider */

    /* wait for 1us to enable divider */
    bl_gpt_delay_us(1);

    /* Sequence to switch to PLL/HFOSC clocks as below: */
    *BL_CLK_CONDD = 0x10000FFF;         /* CONFG_BASE + 0x010C, bit 0~11 set to 1 to force clock on before clock change */

#ifdef VCORE_1P3V
    *BL_CLK_CONDB = 0xE7B9442D;         /*CONFG_BASE + 0x0104 */
    /*  DSP_MUX_SEL, 6: HFOSC_DIV2_CK (156MHz)
        DISP_PWM_MUX_SEL, 1: LFOSC_F26M_CK
        USB_MUX_SEL, 3: UPLL_F62M_CK
        CAM_MUX_SEL, 2: UPLL_48M_CK
        GP_F26M_GFMUX_SEL, 1: LFOSC
        LP_F26M_GFMUX_SEL, 1: LFOSC
        BSI_MUX_SEL, 4: HFOSC_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SLCD_MUX_SEL, 5: HFOSC_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SFC_MUX_SEL, 1: LFOSC_F26M_CK
        CM_MUX_SEL, 5: MPLL_F208M_CK
        BUS_MUX_SEL, 5: HFOSC_DIV5_CK (62.4MHz) */
#elif defined  VCORE_1P3V_PLL
    *BL_CLK_CONDB = 0xC384802B;         /* CONFG_BASE + 0x0104 */
    /*  DSP_MUX_SEL, 4: MPLL_DIV2P5_CK (none 50-50 duty 124.8MHz)
        DISP_PWM_MUX_SEL, 0: CLKSQ_F26M_CK
        USB_MUX_SEL, 3: UPLL_F62M_CK
        CAM_MUX_SEL, 2: UPLL_48M_CK
        GP_F26M_GFMUX_SEL, 0: DCXO
        LP_F26M_GFMUX_SEL, 0: DCXO
        BSI_MUX_SEL, 2: MPLL_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SLCD_MUX_SEL, 2: MPLL_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SFC_MUX_SEL, 0: CLKSQ_F26M_CK
        CM_MUX_SEL, 5: MPLL_F208M_CK (208MHz)
        BUS_MUX_SEL, 3: MPLL_DIV5_CK (62.4MHz) */
#elif defined  VCORE_1P2V_PLL
    *BL_CLK_CONDB = 0xC384801B;         /* CONFG_BASE + 0x0104 */
    /*  DSP_MUX_SEL, 4: MPLL_DIV2P5_CK (none 50-50 duty 124.8MHz)
        DISP_PWM_MUX_SEL, 0: CLKSQ_F26M_CK
        USB_MUX_SEL, 3: UPLL_F62M_CK
        CAM_MUX_SEL, 2: UPLL_48M_CK
        GP_F26M_GFMUX_SEL, 0: DCXO
        LP_F26M_GFMUX_SEL, 0: DCXO
        BSI_MUX_SEL, 2: MPLL_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SLCD_MUX_SEL, 2: MPLL_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SFC_MUX_SEL, 0: CLKSQ_F26M_CK
        CM_MUX_SEL, 3: MPLL_DIV3_CK (104MHz)
        BUS_MUX_SEL, 3: MPLL_DIV5_CK (62.4MHz) */
#elif defined  VCORE_0P9V
    *BL_CLK_CONDB = 0x97B24489;         /* CONFG_BASE + 0x0104 */
    /*  DSP_MUX_SEL, 1: LFOSC
        DISP_PWM_MUX_SEL, 1: LFOSC
        USB_MUX_SEL, 3: UPLL_F62M_CK
        CAM_MUX_SEL, 2: UPLL_48M_CK
        GP_F26M_GFMUX_SEL, 1: LFOSC
        LP_F26M_GFMUX_SEL, 1: LFOSC
        BSI_MUX_SEL, 1: LFOSC
        SLCD_MUX_SEL, 1: LFOSC
        SFC_MUX_SEL, 1: LFOSC
        CM_MUX_SEL, 1: LFOSC
        BUS_MUX_SEL, 1: LFOSC */
#else /* VCORE_1P1V*/
    *BL_CLK_CONDB = 0xF7B94425;         /* CONFG_BASE + 0x0104 */
    /*  DSP_MUX_SEL, 7: HFOSC_DIV2P5_CK (none 50-50 duty 124.8MHz)
        DISP_PWM_MUX_SEL, 1: LFOSC_F26M_CK
        USB_MUX_SEL, 3: UPLL_F62M_CK
        CAM_MUX_SEL, 2: UPLL_48M_CK
        GP_F26M_GFMUX_SEL, 1: LFOSC
        LP_F26M_GFMUX_SEL, 1: LFOSC
        BSI_MUX_SEL, 4: HFOSC_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SLCD_MUX_SEL, 5: HFOSC_DIV2P5_CK (none 50-50 duty 124.8MHz)
        SFC_MUX_SEL, 1: LFOSC_F26M_CK
        CM_MUX_SEL, 4: HFOSC_DIV3_CK (104MHz)
        BUS_MUX_SEL, 5: HFOSC_DIV5_CK (62.4MHz) */
#endif  /* ifdef VCORE_1P3V */

    *BL_ACFG_CLK_UPDATE = 0x0E7F;       /* ANA CFGSYS_BASE + 0x0150, set chg_clk */

    /* wait for 2us to change source from 26M to PLL clock */
    bl_gpt_delay_us(2);

    *BL_CLK_CONDD = 0x10000000;         /* CONFG_BASE + 0x010C, bit 0~11 set to 0 to release force clock on after clock change */

}   /* void clock_set_pll_dcm_init(void) */
