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

#ifndef __HAL_MIPI_TX_CONFIG_INTERNAL_H__
#define __HAL_MIPI_TX_CONFIG_INTERNAL_H__

typedef union {
    uint32_t	value;
    struct {
        uint32_t 	RG_PLL_EN				: 1; 	/*! Bit 0 PLL enable*/
        uint32_t 	reserved				: 1;		/*! reserved */
        uint32_t 	RG_LNT_LPTX_BIAS_EN		: 1; 	/*! Bit 2 Lane LP  bias enable */
        uint32_t	RG_LNT_HSTX_EDGE_SEL  	: 1; 	/*! Bit 3 Lane LP  bias enable */
        uint32_t 	RG_DSI_PHY_CK_PSEL  	: 1; 	/*! Bit 4 DSI PHY CLK edge sel. 1'b0 = inverse */
        uint32_t 	reserved1				: 1;		/*! reserved */
        uint32_t 	RG_LNT_HSTX_BIAS_EN  	: 1; 	/*! Bit 6 Lane HS  bias enable */
        uint32_t 	RG_CLK_SEL				: 1; 	/*! Bit 7 1'b1 = HS clk/4 1'b0 = HS clk/8*/
        uint32_t 	RG_MONCK_EN				: 1; 	/*! Bit 8 Enable monitor VCO clock for debug */
        uint32_t 	RG_MONVC_EN				: 1; 	/*! Bit 9 Enable monitor Vctrl Voltage for debug */
        uint32_t 	RG_MONREF_EN			: 1; 	/*! Bit 10 Enable PFD Clock Out for Frequency Meter */
        uint32_t 	RG_MONFB_EN				: 1; 	/*! Bit 11 Enable PFD Clock Out for Frequency Meter */
        uint32_t 	RG_PLL_VOD_EN			: 1; 	/*! Bit 12 Enable CHP OverDrive */
        uint32_t 	RG_PLL_LF				: 1; 	/*! Bit 13 */
        uint32_t 	RG_PLL_DDSFBK_EN  		: 1; 	/*! Bit 14 */
        uint32_t 	reserved2				: 1;		/*! reserved */
        uint32_t 	RG_PLL_RST_DLY    		: 2; 	/*! Bit 16..17 */
        uint32_t 	reserved3				: 13;	/*! reserved */
        uint32_t 	RG_DIG_CK_EN    		: 1; 	/*! Bit 31 Digital clk enable */
    } field;
} MIPITX_CON0_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t	reserved				: 12; 	/*! reserved */
        uint32_t 	RG_LNT_CALZ_CZ			: 4; 	/*! Bit 12..15 */
        uint32_t 	RG_LNT_CALZ_EN  		: 1;		/*! Bit 16 */
        uint32_t 	reserved1				: 15; 	/*! reserved */
    } field;
} MIPITX_CON1_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t 	RG_PLL_BP				: 1; 	/*! Bit 0 */
        uint32_t 	RG_PLL_BR				: 1;		/*! Bit 1 */
        uint32_t 	RG_PLL_PHIDIV_SEL		: 1; 	/*! Bit 2 */
        uint32_t		reserved				: 1; 	/*! reserved */
        uint32_t 	RG_PLL_PREDIV			: 2; 	/*! Bit 4..5 */
        uint32_t 	RG_PLL_POSDIV			: 2;		/*! Bit 6..7 */
        uint32_t 	RG_PLL_FBKDIV			: 8; 	/*! Bit 8..15 */
        uint32_t 	RG_PLL_DIVEN			: 3; 	/*! Bit 16..18 */
        uint32_t 	reserved1				: 5;		/*! reserved */
        uint32_t 	RG_PLL_RST_DLY			: 8; 	/*! Bit 24..31 */
    } field;
} MIPITX_CON2_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t 	RG_BYTECKD1_EN			: 1; 	/*! Bit 0 */
        uint32_t 	RG_BYTECKD2_EN  		: 1;		/*! Bit 1 */
        uint32_t 	reserved1				: 30; 	/*! reserved */
    } field;
} MIPITX_CON3_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t		reserved				: 5; 	/*! reserved */
        uint32_t 	RG_CLK_DLY_EN			: 1; 	/*! Bit 5 */
        uint32_t 	RG_CLK_DLY_SWC			: 2;		/*! Bit 6..7 */
        uint32_t 	RG_CLK_DLY_SEL			: 4; 	/*! Bit 8..11 */
        uint32_t 	RG_CLKB_DLY_SEL  		: 4; 	/*! Bit 12..15 */
        uint32_t		reserved1				: 1; 	/*! reserved */
        uint32_t 	RG_PAD_PL_EN			: 1;		/*! Bit 17 */
        uint32_t 	RG_PAD_PL_SEL			: 1;		/*! Bit 18 */
        uint32_t 	RG_BYPASS_26M_EN  		: 1; 	/*! Bit 19 */
        uint32_t 	RG_MIPI_26M_REQ_EN  	: 1;		/*! Bit 20 */
        uint32_t 	RG_MIPI_26M_CK_SEL    	: 1; 	/*! Bit 21 */
        uint32_t	reserved2				: 10; 	/*! reserved */
    } field;
} MIPITX_CON4_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t		reserved				: 2; 	/*! reserved */
        uint32_t 	RG_LNT_CALZ_CMPZOUT  	: 1; 	/*! Bit 2 */
        uint32_t		reserved1				: 7; 	/*! reserved */
        uint32_t 	RG_LNT_BGR_DOUT2 		: 1;		/*! Bit 10 */
        uint32_t 	RG_LNT_BGR_DOUT1		: 1;		/*! Bit 11 */
        uint32_t	reserved2				: 20; 	/*! reserved */
    } field;
} MIPITX_CON5_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t 	RG_LNT_BGR_EN			: 1; 	/*! Bit 0 */
        uint32_t 	RG_LNT_BGR_CHP_EN		: 1; 	/*! Bit 1 */
        uint32_t 	RG_LNT_BGR_SEL_PH		: 1; 	/*! Bit 2 */
        uint32_t		reserved				: 1; 	/*! reserved */
        uint32_t 	RG_LNT_BGR_DIV			: 2; 	/*! Bit 4..5 */
        uint32_t		RG_LNT_BGR_DOUT1_SEL  	: 2; 	/*! Bit 6..7 */
        uint32_t 	RG_LNT_BGR_DOUT2_SEL	: 2;		/*! Bit 8..9 */
        uint32_t 	RG_LNT_AIO_SEL			: 4;		/*! Bit 10..13 */
        uint32_t	reserved2				: 18; 	/*! reserved */
    } field;
} MIPITX_CON6_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t		reserved				: 8; 	/*! reserved */
        uint32_t 	RG_LNT_LPTX_CALI		: 3; 	/*! Bit 8..10 */
        uint32_t		reserved1				: 1; 	/*! reserved */
        uint32_t 	RG_LNT_LPCD_CALI		: 3; 	/*! Bit 12..14 */
        uint32_t	reserved2				: 17; 	/*! reserved */
    } field;
} MIPITX_CON7_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t 	RG_LNTC_HS_CZ			: 4; 	/*! Bit 0..3 */
        uint32_t 	RG_LNT0_HS_CZ			: 4; 	/*! Bit 4..7 */
        uint32_t 	RG_LNT1_HS_CZ			: 4; 	/*! Bit 8..11 */
        uint32_t 	RG_LNT_CIRE				: 2; 	/*! Bit 12..13 */
        uint32_t	reserved				: 18; 	/*! reserved */
    } field;
} MIPITX_CON8_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t 	RG_FORCE_EN				: 3; 	/*! Bit 0..2 */
        uint32_t 	RG_FORCE_TX_D			: 3; 	/*! Bit 3..5 */
        uint32_t 	RG_TMODE				: 1; 	/*! Bit 6 */
        uint32_t		reserved				: 1; 	/*! reserved */
        uint32_t 	RG_LNT_RESERVED			: 7; 	/*! Bit 8..14 */
        uint32_t		RG_LN_META_CTRL			: 1;		/*! Bit 15 */
        uint32_t 	RG_VODC_ADJ_IS			: 4; 	/*! Bit 16..19 */
        uint32_t 	RG_VOD0_ADJ_IS  		: 4; 	/*! Bit 20..23 */
        uint32_t 	RG_VOD1_ADJ_IS  		: 4; 	/*! Bit 24..27 */
        uint32_t 	RG_VODC_ADJ_P	  		: 1; 	/*! Bit 28 */
        uint32_t 	RG_VOD0_ADJ_P	  		: 1; 	/*! Bit 29 */
        uint32_t 	RG_VOD1_ADJ_P	  		: 1; 	/*! Bit 30 */
        uint32_t 	reserved1				: 1; 	/*! reserved */
    } field;
} MIPITX_CON9_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t	RG_RESERVED				: 16; 	/*! Bit 0..15 */
        uint32_t 	reserved				: 16; 	/*! reserved */
    } field;
} MIPI_RESERVED_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t	RG_LCDDS_PCW_NCPO		: 32; 	/*! Bit 0..31 */
    } field;
} MIPI_LCDDS_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t 	RG_LCDDS_PWDB  			: 1; 	/*! Bit 0 */
        uint32_t 	RG_LCDDS_PREDIV2  		: 1; 	/*! Bit 1 */
        uint32_t 	RG_LCDDS_PCW_NCPO_CHG  	: 1; 	/*! Bit 2 */
        uint32_t		RG_LCDDS_MONEN			: 1; 	/*! Bit 3 */
        uint32_t 	RG_LCDDS_C				: 3; 	/*! Bit 4..6 */
        uint32_t 	reserved				: 1; 	/*! reserved */
        uint32_t 	RG_LCDDS_SSC_EN  		: 1; 	/*! Bit 8 */
        uint32_t 	RG_LCDDS_SSC_PHASE_INI	: 1; 	/*! Bit 9 */
        uint32_t 	reserved1				: 6; 	/*! reserved */
        uint32_t 	RG_LCDDS_SSC_PRD		: 16; 	/*! Bit 16..31 */
    } field;
} MIPI_LCDDS1_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t	RG_LCDDS_SSC_DELTA  	: 16; 	/*! Bit 0..15 */
        uint32_t	RG_LCDDS_SSC_DELTA1    	: 16; 	/*! Bit 16..31 */
    } field;
} MIPI_LCDDS2_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t 	RG_TX_SW_CTL_EN    		: 1; 	/*! Bit 0 */
        uint32_t 	reserved				: 1; 	/*! reserved */
        uint32_t 	RG_TX_TST_CK_OUT_EN		: 1; 	/*! Bit 2 */
        uint32_t		RG_TX_GPIO_MODE_EN		: 1; 	/*! Bit 3 */
        uint32_t 	RG_TX_DBG_OUT_SEL		: 4; 	/*! Bit 4..7 */
        uint32_t 	RG_TX_DBG_OUT_EN  		: 1; 	/*! Bit 8 */
        uint32_t 	reserved1				: 7; 	/*! reserved */
        uint32_t 	RG_SW_LNTC_ZEROCLK  	: 1; 	/*! Bit 16 */
        uint32_t 	RG_SW_LNTC_HSTX_PRE_OE  : 1; 	/*! Bit 17 */
        uint32_t 	RG_SW_LNTC_LPTX_DN  	: 1; 	/*! Bit 18 */
        uint32_t 	RG_SW_LNTC_LPTX_DP  	: 1; 	/*! Bit 19 */
        uint32_t 	RG_SW_LNTC_LPTX_OE  	: 1; 	/*! Bit 20 */
        uint32_t 	RG_SW_LNTC_HSTX_OE  	: 1; 	/*! Bit 21 */
        uint32_t 	RG_TX_TST_CK_OUT_SEL  	: 2; 	/*! Bit 22..23 */
        uint32_t 	reserved2				: 8; 	/*! reserved */
    } field;
} MIPI_SWCTL_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t 	RG_SW_LNT0_HSTX_DATA	: 8; 	/*! Bit 0..7 */
        uint32_t 	RG_SW_LNT0_HSTX_DRDY	: 1; 	/*! Bit 8 */
        uint32_t		RG_SW_LNT0_HSTX_OE		: 1; 	/*! Bit 9 */
        uint32_t 	RG_SW_LNT0_HSTX_PRE_OE	: 1; 	/*! Bit 10 */
        uint32_t 	reserved				: 1; 	/*! reserved */
        uint32_t 	RG_SW_LNT0_LPTX_DN  	: 1; 	/*! Bit 12 */
        uint32_t 	RG_SW_LNT0_LPTX_DP  	: 1; 	/*! Bit 13 */
        uint32_t 	RG_SW_LNT0_LPTX_OE		: 1; 	/*! Bit 14 */
        uint32_t 	RG_SW_LNT0_LPTX_EN  	: 1; 	/*! Bit 15 */
        uint32_t 	reserved1				: 16; 	/*! reserved */
    } field;
} MIPI_SWCTL1_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t	MIPI_TX_DBG_OUT			: 32; 	/*! Bit 0..31 */
    } field;
} MIPI_DBG_OUT_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t 	DA_PLL_DDS_PWR_ON		: 1; 	/*! Bit 0 */
        uint32_t		DA_PLL_DDS_ISO_EN		: 1; 	/*! Bit 1 */
        uint32_t 	reserved				: 6; 	/*! reserved */
        uint32_t 	AD_PLL_DDS_PWR_ACK  	: 1; 	/*! Bit 8 */
        uint32_t 	reserved1				: 23; 	/*! reserved */
    } field;
} MIPI_PLL_PWR_REGISTER_T;


typedef struct {
    MIPITX_CON0_REGISTER_T			mipitx_con0_register;		// 0x00
    MIPITX_CON1_REGISTER_T			mipitx_con1_register;		// 0x04
    MIPITX_CON2_REGISTER_T			mipitx_con2_register;		// 0x08
    MIPITX_CON3_REGISTER_T			mipitx_con3_register;		// 0x0C
    MIPITX_CON4_REGISTER_T			mipitx_con4_register;		// 0x10
    MIPITX_CON5_REGISTER_T			mipitx_con5_register;		// 0x14
    uint32_t						reserved[10];
    MIPITX_CON6_REGISTER_T			mipitx_con6_register;		// 0x40
    MIPITX_CON7_REGISTER_T			mipitx_con7_register;		// 0x44
    uint32_t						reserved1[2];
    MIPITX_CON8_REGISTER_T			mipitx_con8_register;		// 0x50
    MIPITX_CON9_REGISTER_T			mipitx_con9_register;		// 0x54
    uint32_t						reserved2[8];
    MIPI_RESERVED_REGISTER_T		mipi_reserved_register;		// 0x78
    uint32_t						reserved3[2];
    MIPI_LCDDS_REGISTER_T			mipi_lcdds_register;		// 0x84
    MIPI_LCDDS1_REGISTER_T			mipi_lcdds1_register;		// 0x88
    MIPI_LCDDS2_REGISTER_T			mipi_lcdds2_register;		// 0x8C
    MIPI_SWCTL_REGISTER_T			mipi_swctl_register;		// 0x90
    MIPI_SWCTL1_REGISTER_T			mipi_swctl1_register;		// 0x94
    MIPI_DBG_OUT_REGISTER_T 		mipi_dbg_out_register;		// 0x98
    MIPI_PLL_PWR_REGISTER_T 		mipi_pll_pwr_register;		// 0x9C
} MIPITX_CONFIG_REGISTER_T;

#endif //__HAL_MIPI_TX_CONFIG_INTERNAL_H__


