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

#ifndef __HAL_CLOCK_PLATFORM_MT2523_H__
#define __HAL_CLOCK_PLATFORM_MT2523_H__

#include "hal_platform.h"

#ifdef HAL_CLOCK_MODULE_ENABLED

#define STR_VALUE(arg)      #arg
#define __stringify(name) STR_VALUE(name)

#define DVT_CG_TEST                 0
/* #define CLK_SUSPEND_RESUME_TEST */
#define CLK_DFS_USE_HARD_CODE 0
/* #define CLK_DFS_TEST */
/* #define CLK_TOP_CLK_DVT */
#ifdef CLK_TOP_CLK_DVT
#define CLOCK_OVER_78M true
#define CLOCK_LOWER_78M false
#define DVT_GP_26M_CK_ENABLE        1       /* GP01 */
#define DVT_BUS_ENABLE              1       /* GP01 */
#define DVT_LP_26M_CK_ENABLE        1       /* GP01 */
#define DVT_DISP_PWM_CK_ENABLE      1       /* GP02 */
#define DVT_CAM_CK_ENABLE           1       /* GP02 */
#define DVT_SFC_CK_ENABLE           1
#define DVT_CM_CK_ENABLE            1
#define DVT_DSP_CK_ENABLE           1
#define DVT_BSI_CK_ENABLE           1
#define DVT_SLCD_CK_ENABLE          1
#define DVT_USB_CK_ENABLE           1
#define DVT_MSDC0_CK_ENABLE         1
#define DVT_MSDC1_CK_ENABLE         1

#define DVT_GP_26M_CK(SEL, NAME, MODE)      clock_mux_sel(GP_F26M_GFMUX_SEL, SEL);  \
                                            log_hal_info("GP_F26M_GFMUX_SEL freq(%s) = %d\n\r", #NAME, freq_monitor(FREQ_CSW_GP_26M_CK, MODE))
#define DVT_BUS_CK(SEL, NAME, MODE)         clock_mux_sel(BUS_MUX_SEL, SEL);        \
                                            log_hal_info("BUS_MUX_SEL freq(%s) = %d\n\r", #NAME, freq_monitor(FREQ_CSW_BUS_CK, MODE))
#define DVT_LP_26M_CK(SEL, NAME, MODE)      clock_mux_sel(LP_F26M_GFMUX_SEL, SEL);  \
                                            log_hal_info("LP_F26M_GFMUX_SEL freq(%s) = %d\n\r", #NAME, freq_monitor(FREQ_CSW_LP_26M_CK, MODE))
#define DVT_DISP_PWM_CK(SEL, NAME, MODE)    clock_mux_sel(DISP_PWM_MUX_SEL, SEL);   \
                                            log_hal_info("DISP_PWM_MUX_SEL freq(%s) = %d\n\r", #NAME, freq_monitor(FREQ_CSW_DISP_PWM_CK, MODE))
#define DVT_CAM_CK(SEL, NAME, MODE)         clock_mux_sel(CAM_MUX_SEL, SEL);        \
                                            log_hal_info("CAM_MUX_SEL freq(%s) = %d\n\r", #NAME, freq_monitor(FREQ_CSW_CAM_CK, MODE))
#define DVT_SFC_CK(SEL, NAME, MODE)         clock_mux_sel(SFC_MUX_SEL, SEL);        \
                                            log_hal_info("SFC_MUX_SEL freq(%s) = %d\n\r", #NAME, freq_monitor(FREQ_CSW_SFC_CK, MODE))
#define DVT_CM_CK(SEL, NAME, MODE)          clock_mux_sel(CM_MUX_SEL, SEL);         \
                                            log_hal_info("CM_MUX_SEL freq(%s) = %d\n\r", #NAME, freq_monitor(FREQ_CSW_CM_CK, MODE))
#define DVT_DSP_CK(SEL, NAME, MODE)         clock_mux_sel(DSP_MUX_SEL, SEL);        \
                                            log_hal_info("DSP_MUX_SEL freq(%s) = %d\n\r", #NAME, freq_monitor(FREQ_CSW_DSP_CK, MODE))
#define DVT_BSI_CK(SEL, NAME, MODE)         clock_mux_sel(BSI_MUX_SEL, SEL);        \
                                            log_hal_info("BSI_MUX_SEL freq(%s) = %d\n\r", #NAME, freq_monitor(FREQ_CSW_BSI_CK, MODE))
#define DVT_SLCD_CK(SEL, NAME, MODE)        clock_mux_sel(SLCD_MUX_SEL, SEL);       \
                                            log_hal_info("SLCD_MUX_SEL freq(%s) = %d\n\r", #NAME, freq_monitor(FREQ_CSW_SLCD_CK, MODE))
#define DVT_RG_SLCD_CK(SEL, NAME, MODE)     clock_mux_sel(RG_SLCD_CK_SEL, SEL);     \
                                            log_hal_info("RG_SLCD_CK_SEL freq(%s) = %d\n\r", #NAME, freq_monitor(FREQ_CSW_SLCD_CK, MODE))
#define DVT_USB_CK(SEL, NAME, MODE)         clock_mux_sel(USB_MUX_SEL, SEL);        \
                                            log_hal_info("USB_MUX_SEL freq(%s) = %d\n\r", #NAME, freq_monitor(FREQ_CSW_USB_CK, MODE))
#define DVT_MSDC0_CK(SEL, NAME, MODE)       clock_mux_sel(MSDC0_MUX_SEL, SEL);   \
                                            log_hal_info("MSDC0_MUX_SEL freq(%s) = %d\n\r", #NAME, freq_monitor(FREQ_CSW_MSDC0_CK, MODE))
#define DVT_MSDC1_CK(SEL, NAME, MODE)       clock_mux_sel(MSDC1_MUX_SEL, SEL);   \
                                            log_hal_info("MSDC1_MUX_SEL freq(%s) = %d\n\r", #NAME, freq_monitor(FREQ_CSW_MSDC1_CK, MODE))
#endif

/*************************************************************************
 * Define cm4 clock status
 *************************************************************************/
typedef enum {
    CM_208M_STA,
    CM_104M_STA,
    CM_26M_STA,
} clock_cm_freq_state_id;

/*************************************************************************
 * Define pll clock register and bits structure
 *************************************************************************/
typedef enum {
    SYSCLK,
    UPLLCLK,
    MPLLCLK,
    NR_PLL_CLOCKS,
} hal_clock_pll_id;

/*************************************************************************
 * Define clock source register and bits structure
 *************************************************************************/
typedef enum {
    CLK_SRC_DCXO    = 0x1,
    CLK_SRC_PLL     = 0x2,
    CLK_SRC_LFOSC   = 0x4,
    CLK_SRC_HFOSC   = 0x8,
} clock_src_id;

typedef enum {
    /* ARMCLK_SOURCE_SEL, */
    CM4CLK_SOURCE_SEL,
    AO_BUSCLK_SOURCE_SEL,
    PDN_BUSCLK_SOURCE_SEL,
    SFCCLK_SOURCE_SEL,
    BSICLK_SOURCE_SEL,
    DSPCLK_SOURCE_SEL,
    NR_CLKSRC_SEL,
    NONE_CLK_SOURCE_SEL,        /* Additional setting for none mapping */
} clock_src_sel_id;

/*************************************************************************
 * Define clock divider register and bits structure
 *************************************************************************/
typedef enum {
    HFOSC_DIV_GRP,
    UPLL_DIV_GRP,
    MPLL_DIV_GRP,
    NR_DIV_GRP,
} clock_div_grp_id;

typedef enum {
    HFOSC_DIV1P5_CK,
    HFOSC_DIV2_CK,
    HFOSC_DIV4_CK,
    HFOSC_DIV2P5_CK,
    HFOSC_DIV3_CK,
    HFOSC_DIV6_CK,
    HFOSC_DIV3P5_CK,
    HFOSC_DIV5_CK,
    UPLL_F312M_CK,
    UPLL_F124M_CK,
    UPLL_F104M_CK,
    UPLL_F62M_CK,
    UPLL_48M_CK,
    MPLL_F312M_CK,
    MPLL_F250M_CK,
    MPLL_F208M_CK,
    MPLL_F125M_CK,
    MPLL_DIV1P5_CK,
    MPLL_DIV2_CK,
    MPLL_DIV4_CK,
    MPLL_DIV2P5_CK,
    MPLL_DIV3_CK,
    MPLL_DIV6_CK,
    MPLL_DIV3P5_CK,
    MPLL_DIV5_CK,
    MPLL_F138M_CK,
    MPLL_F178M_CK,
    NR_DIV,         /* NR_DIV = 27 */
    CLKSQ_F26M_CK,  /* Additional option for div map mux */
    LFOSC_F26M_CK,  /* Additional option for div map mux */
    CLK_DIV_NONE,   /* Additional option for none mapping */
    NR_HFOSC_DIV        = 8,
    NR_UPLL_DIV         = 5,
    NR_MPLL_DIV         = 14,
} clock_div_id;

/*************************************************************************
 * Define clock force on register and bits structure
 *************************************************************************/
enum clock_force_on_id {
    BUSCSW_FORCE_ON,
    CMCSW_FORCE_ON,
    /* ARMCSW_FORCE_ON, */
    SFCCSW_FORCE_ON,
    SLCDCSW_FORCE_ON,
    BSICSW_FORCE_ON,
    DSPCSW_FORCE_ON,
    CAMCSW_FORCE_ON,
    USBCSW_FORCE_ON,
    DISPPWMCSW_FORCE_ON,
    MSDC0CSW_FORCE_ON,
    MSDC1CSW_FORCE_ON,
    NR_CLKFORCE_ON,
};

/*************************************************************************
 * Define clock change bit register and bits structure
 *************************************************************************/
typedef enum {
    CHG_BUS,
    CHG_CM,
    /* CHG_ARM, */
    CHG_SFC,
    CHG_SLCD,
    CHG_BSI,
    CHG_DSP,
    CHG_CAM,
    CHG_USB,
    CHG_DISP_PWM,
    CHG_MSDC0,
    CHG_MSDC1,
    CHG_LP_CLKSQ,
    CHG_LP_LFOSC,
    NR_CLKCHANGE_BIT,
} clock_change_bit_id;

/*************************************************************************
 * Define clock mux register and bits structure
 *************************************************************************/
typedef enum {
    BUS_MUX_SEL,
    CM_MUX_SEL,
    SFC_MUX_SEL,
    SLCD_MUX_SEL,
    BSI_MUX_SEL,
    DSP_MUX_SEL,
    CAM_MUX_SEL,
    USB_MUX_SEL,
    DISP_PWM_MUX_SEL,
    MSDC0_MUX_SEL,
    MSDC1_MUX_SEL,
    LP_CLKSQ_MUX_SEL,
    LP_LFOSC_MUX_SEL,
    RG_SLCD_CK_SEL,
    GP_F26M_GFMUX_SEL,
    LP_F26M_GFMUX_SEL,
    NR_MUXS,
    CLK_MUX_SEL_NONE,
} clock_mux_sel_id;


/*************************************************************************
* Define clock gating register and bits structure
* Note: MUST modify clk_cg_mask in hal_clock.c if hal_clock_cg_id is changed
*************************************************************************/
enum {
    PDN_COND0_FROM      = HAL_CLOCK_CG_BT_26M,
    PDN_COND0_TO        = HAL_CLOCK_CG_BSI,
    NR_PDN_COND0        = 7,

    PDN_COND1_FROM      = HAL_CLOCK_CG_SEJ,
    PDN_COND1_TO        = HAL_CLOCK_CG_SPI3,
    NR_PDN_COND1        = 17,

    PDN_COND2_FROM      = HAL_CLOCK_CG_PWM0,
    PDN_COND2_TO        = HAL_CLOCK_CG_PWM5,
    NR_PDN_COND2        = 18,

    CM_PDN_COND0_FROM   = HAL_CLOCK_CG_LCD,
    CM_PDN_COND0_TO     = HAL_CLOCK_CG_LCD_APB,
    NR_CM_PDN_COND0     = 10,

    ACFG_PDN_RG_FROM    = HAL_CLOCK_CG_AUXADC,
    ACFG_PDN_RG_TO      = HAL_CLOCK_CG_I2C0,
    NR_ACFG_PDN_RG      = 5,

    NR_CLOCKS           = 139,  /* The last clock_id HAL_CLOCK_CG_I2C0 + 1 */
};
/***********************************************************************/

enum {
    GRP_PDN_COND0       = 0,
    GRP_PDN_COND1       = 1,
    GRP_PDN_COND2       = 2,
    GRP_CM_PDN_COND0    = 3,
    GRP_ACFG_PDN_RG     = 4,
    NR_GRPS             = 5,
};

#ifdef CLK_TOP_CLK_DVT
enum {
    FREQ_IDLE,
    FREQ_CSW_GP_26M_CK,
    FREQ_CSW_32K_CK,
    FREQ_UPLL_48M_CK,
    FREQ_UPLL_624M_CK0,
    FREQ_PLL_AD_TEST_CK,
    FREQ_MPLL_520M_CK0,
    FREQ_CLKSQ_26M,
    FREQ_CSW_BUS_CK,
    FREQ_UPLL_F104M_CK,
    FREQ_MIPI_DSI_TST_CK,
    FREQ_AD_LFOSC_CK,
    FREQ_AD_HFOSC_CK,
    FREQ_CSW_LP_26M_CK,
    FREQ_CSW_DISP_PWM_CK,
    FREQ_CSW_CAM_CK,
    FREQ_CSW_MSDC1_CK,
    FREQ_CSW_SFC_CK,
    FREQ_CSW_ARM_CK,
    FREQ_CSW_CM_CK,
    FREQ_CSW_DSP_CK,
    FREQ_CSW_BSI_CK,
    FREQ_CSW_SLCD_CK,
    FREQ_CSW_MSDC0_CK,
    FREQ_CSW_USB_CK,
    FREQ_MUX_CLKSQ_26M_CK0,
};
#endif /* #ifdef CLK_TOP_CLK_DVT */

#endif /* HAL_CLOCK_MODULE_ENABLED */
#endif /* __HAL_CLOCK_PLATFORM_MT2523_H__ */
