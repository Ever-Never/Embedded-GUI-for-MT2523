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

#ifndef __HAL_DISPLAY_LCD_INTERNAL_H__
#define __HAL_DISPLAY_LCD_INTERNAL_H__

#include "hal_platform.h"

#ifdef HAL_DISPLAY_LCD_MODULE_ENABLED

#include "hal_display_lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    uint32_t	value;
    struct {
        uint32_t RUN					: 1; 	/*! Bit 0 */
        uint32_t WAIT_CMDQ				: 1; 	/*! Bit 1 */
        uint32_t reserved2				: 1; 	/*! reserved */
        uint32_t WAIT_HTT				: 1; 	/*! Bit 3 */
        uint32_t WAIT_SYNC				: 1; 	/*! Bit 4 */
        uint32_t BUSY					: 1; 	/*! Bit 5 */
        uint32_t GNC					: 1; 	/*! Bit 6 */
        uint32_t reserved1				: 1; 	/*! reserved */
        uint32_t MAIN_IDLE				: 1; 	/*! Bit 8 */
        uint32_t ADDCON_IDLE			: 1; 	/*! Bit 9 */
        uint32_t CACHE_IDLE 			: 1; 	/*! Bit 10 */
        uint32_t DATA_IN_ARBITER		: 1; 	/*! Bit 11 */
        uint32_t SIF_CMD_EMPTY			: 1; 	/*! Bit 12 */
        uint32_t SCHAN_ARB_IDLE 		: 1; 	/*! Bit 13 */
        uint32_t reserved				: 18;	/*! reserved */
    } field;
} LCD_STA_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CPL					: 1; 	/*! Bit 0 */
        uint32_t REG_CPL				: 1; 	/*! Bit 1 */
        uint32_t CMDQ_CPL				: 1; 	/*! Bit 2 */
        uint32_t reserved1				: 1; 	/*! reserved */
        uint32_t HTT					: 1; 	/*! Bit 4 */
        uint32_t SYNC					: 1; 	/*! Bit 5 */
        uint32_t APB_TIMEOUT			: 1; 	/*! Bit 6 */
        uint32_t reserved				: 25;	/*! reserved */
    } field;
} LCD_INTEN_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CPL					: 1; 	/*! Bit 0 */
        uint32_t REG_CPL				: 1; 	/*! Bit 1 */
        uint32_t CMDQ_CPL				: 1; 	/*! Bit 2 */
        uint32_t reserved1				: 1; 	/*! reserved */
        uint32_t HTT					: 1; 	/*! Bit 4 */
        uint32_t SYNC					: 1; 	/*! Bit 5 */
        uint32_t APB_TIMEOUT			: 1; 	/*! Bit 6 */
        uint32_t reserved				: 25;	/*! reserved */
    } field;
} LCD_INTSTA_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t INT_RESET				: 1; 	/*! Bit 0 */
        uint32_t reserved1				: 14;	/*! reserved */
        uint32_t START					: 1; 	/*! Bit 15 */
        uint32_t reserved				: 16;	/*! reserved */
    } field;
} LCD_START_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t RSTB					: 1; 	/*! Bit 0 */
        uint32_t reserved				: 31;	/*! reserved */
    } field;
} LCD_RSTB_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t SIF0_2PIN_SIZE 		: 3; 	/*! Bit 0..2 */
        uint32_t reserved4				: 1; 	/*! reserved */
        uint32_t SIF0_PIX_2PIN			: 1; 	/*! Bit 4 */
        uint32_t reserved3				: 1; 	/*! reserved */
        uint32_t SIF0_SINGLE_A0 		: 1; 	/*! Bit 6 */
        uint32_t SIF0_CS_STAY_LOW		: 1; 	/*! Bit 7 */
        uint32_t SIF1_2PIN_SIZE 		: 3; 	/*! Bit 8..10 */
        uint32_t reserved2				: 1; 	/*! reserved */
        uint32_t SIF1_PIX_2PIN			: 1; 	/*! Bit 12 */
        uint32_t reserved1				: 1; 	/*! reserved */
        uint32_t SIF1_SINGLE_A0 		: 1; 	/*! Bit 14 */
        uint32_t SIF1_CS_STAY_LOW		: 1; 	/*! Bit 15 */
        uint32_t SIF_DDR_SCK_HW 		: 8; 	/*! Bit 16..23 */
        uint32_t SIF_DDR_DIV4			: 1; 	/*! Bit 24 */
        uint32_t SIF_DDR_DIV8			: 1; 	/*! Bit 25 */
        uint32_t reserved				: 2; 	/*! reserved */
        uint32_t SCK_SDA_SKEW			: 3;		/*! Bit 28..31 */
    } field;
} LCD_SIF_PIX_CON_REGISTER_T;


typedef union {
    uint32_t	value;
    struct {
        uint32_t WR_2ND 				: 4; 	/*! Bit 0..3 */
        uint32_t WR_1ST					: 4;		/*! Bit 4..7 */
        uint32_t RD_2ND					: 4;		/*! Bit 8..11 */
        uint32_t RD_1ST					: 4;		/*! Bit 12..15 */
        uint32_t CSH					: 4;		/*! Bit 16..19 */
        uint32_t CSS					: 4;		/*! Bit 20..23 */
        uint32_t reserved				: 8;		/*! reserved */
    } field;
} LCD_SIF_TIMING_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t SIF0_SIZE				: 3; 	/*! Bit 0..2 */
        uint32_t SIF0_3WIRE 			: 1; 	/*! Bit 3 */
        uint32_t SIF0_SDI				: 1; 	/*! Bit 4 */
        uint32_t SIF0_1ST_POL			: 1; 	/*! Bit 5 */
        uint32_t SIF0_SCK_DEF			: 1; 	/*! Bit 6 */
        uint32_t SIF0_DIV2				: 1; 	/*! Bit 7 */
        uint32_t SIF1_SIZE				: 3; 	/*! Bit 8..10 */
        uint32_t SIF1_3WIRE 			: 1; 	/*! Bit 11 */
        uint32_t SIF1_SDI				: 1; 	/*! Bit 12 */
        uint32_t SIF1_1ST_POL			: 1; 	/*! Bit 13 */
        uint32_t SIF1_SCK_DEF			: 1; 	/*! Bit 14 */
        uint32_t SIF1_DIV2				: 1; 	/*! Bit 15 */
        uint32_t reserved1				: 8; 	/*! reserved */
        uint32_t SIF_HW_CS				: 1; 	/*! Bit 24 */
        uint32_t SIF_VDO_MODE			: 1; 	/*! Bit 25 */
        uint32_t SIF_CMD_LOCK			: 1; 	/*! Bit 26 */
        uint32_t SIF_VDO_AUTO			: 1; 	/*! Bit 27 */
        uint32_t SIF_SYNC_ALIGN 		: 1; 	/*! Bit 28 */
        uint32_t SIF_DDR_EN_CONFIG		: 1; 	/*! Bit 29 */
        uint32_t reserved				: 2;		/*! reserved */
    } field;
} LCD_SIF_CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CS0					: 1; 	/*! Bit 0 */
        uint32_t CS1					: 1; 	/*! Bit 1 */
        uint32_t reserved				: 30;	/*! reserved */
    } field;
} LCD_SIF_CS_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t TIMEOUT				: 12;	/*! Bit 0..11 */
        uint32_t reserved1				: 4; 	/*! reserved */
        uint32_t COUNT					: 12;	/*! Bit 16..27 */
        uint32_t reserved				: 4;		/*! reserved */
    } field;
} LCD_CALC_HTT_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t HTT					: 10;	/*! Bit 0..9 */
        uint32_t reserved1				: 6; 	/*! reserved */
        uint32_t VTT					: 12;	/*! Bit 16..27 */
        uint32_t reserved				: 4;		/*! reserved */
    } field;
} LCD_SYNC_LCM_SIZE_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t WAITLINE				: 12;	/*! Bit 0..11 */
        uint32_t reserved1				: 4; 	/*! reserved */
        uint32_t SCANLINE				: 12;	/*! Bit 16..27 */
        uint32_t reserved				: 4;		/*! reserved */
    } field;
} LCD_SYNC_CNT_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t SYNC_EN				: 1; 	/*! Bit 0 */
        uint32_t TE_EDGE_SEL			: 1; 	/*! Bit 1 */
        uint32_t SYNC_MODE				: 1; 	/*! Bit 2 */
        uint32_t TE_REPEAT				: 1; 	/*! Bit 3 */
        uint32_t reserved2				: 4; 	/*! reserved */
        uint32_t DSI_START_CTL			: 1; 	/*! Bit 8 */
        uint32_t DSI_END_CTL			: 1; 	/*! Bit 9 */
        uint32_t TE_COUNTER_EN			: 1; 	/*! Bit 10 */
        uint32_t reserved1				: 4; 	/*! reserved */
        uint32_t SW_TE					: 1; 	/*! Bit 15 */
        uint32_t reserved				: 16;	/*! reserved */
    } field;
} LCD_TECON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t MAX_BURST				: 3; 	/*! Bit 0..2 */
        uint32_t reserved1				: 1; 	/*! reserved */
        uint32_t THROTTLE_EN			: 1; 	/*! Bit 4 */
        uint32_t reserved				: 11;	/*! reserved */
        uint32_t THROTTLE_PERIOD		: 16;	/*! Bit 16..31 */
    } field;
} LCD_GMCCON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t PAL_ADDR				: 32;	/*! Bit 0..31 */
    } field;
} LCD_PAL_ADD_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t FORMAT 				: 8; 	/*! Bit 0..7 */
        uint32_t COMMAND				: 6; 	/*! Bit 8..13 */
        uint32_t reserved2				: 1; 	/*! reserved */
        uint32_t ENC					: 1; 	/*! Bit 15 */
        uint32_t reserved1				: 8; 	/*! reserved */
        uint32_t SEND_RES_MOD		: 1; 	/*! Bit 24 */
        uint32_t _24IF					: 1; 	/*! Bit 25 */
        uint32_t COLOR_EN				: 1; 	/*! Bit 26 */
        uint32_t reserved				: 1; 	/*! reserved */
        uint32_t EN3					: 1; 	/*! Bit 28 */
        uint32_t EN2					: 1; 	/*! Bit 29 */
        uint32_t EN1					: 1; 	/*! Bit 30 */
        uint32_t EN0					: 1;		/*! Bit 31 */
    } field;
} LCD_WROICON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t XOFFSET				: 12;	/*! Bit 0..11 */
        uint32_t reserved1				: 4; 	/*! reserved */
        uint32_t YOFFSET				: 12;	/*! Bit 16..27 */
        uint32_t reserved				: 4;		/*! reserved */
    } field;
} LCD_WROIOFS_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t reserved1				: 4; 	/*! reserved */
        uint32_t ADDR					: 4; 	/*! Bit 4..7 */
        uint32_t reserved				: 24;	/*! reserved */
    } field;
} LCD_WROICADD_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t reserved1				: 4; 	/*! reserved */
        uint32_t ADDR					: 4; 	/*! Bit 4..7 */
        uint32_t reserved				: 24;	/*! reserved */
    } field;
} LCD_WROIDADD_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t COLUMN 				: 12;	/*! Bit 0..11 */
        uint32_t reserved1				: 4; 	/*! reserved */
        uint32_t ROW					: 12;	/*! Bit 16..27 */
        uint32_t reserved				: 4;		/*! reserved */
    } field;
} LCD_WROISIZE_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t BLUE					: 8; 	/*! Bit 0..7 */
        uint32_t GREEN					: 8; 	/*! Bit 8..15 */
        uint32_t RED					: 8; 	/*! Bit 16..23 */
        uint32_t ALPHA					: 8; 	/*! Bit 24..31 */
    } field;
} LCD_WROI_BGCLR_REGISTER_T;


typedef union {
    uint32_t	value;
    struct {
        uint32_t ALPHA					: 8; 	/*! Bit 0..7 */
        uint32_t ALPHA_EN				: 1; 	/*! Bit 8 */
        uint32_t reserved4				: 2; 	/*! reserved */
        uint32_t ROTATE 				: 3; 	/*! Bit 11..13 */
        uint32_t SRC_KEYEN				: 1; 	/*! Bit 14 */
        uint32_t SRC					: 1; 	/*! Bit 15 */
        uint32_t BYTE_SWP				: 1; 	/*! Bit 16 */
        uint32_t reserved3				: 1; 	/*! reserved */
        uint32_t DITHER_EN				: 1; 	/*! Bit 18 */
        uint32_t reserved2				: 1; 	/*! reserved */
        uint32_t CLR_FMT				: 4; 	/*! Bit 20..23 */
        uint32_t DST_KEYEN				: 1; 	/*! Bit 24 */
        uint32_t reserved1				: 1; 	/*! reserved */
        uint32_t RGB_SWP				: 1; 	/*! Bit 26 */
        uint32_t reserved				: 5;		/*! reserved */
    } field;
} LCD_LWINCON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CLRKEY				: 32;	/*! Bit 0..31 */
    } field;
} LCD_LWINKEY_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t XOFFSET				: 12;	/*! Bit 0..11 */
        uint32_t reserved1				: 4; 	/*! reserved */
        uint32_t YOFFSET				: 12;	/*! Bit 16..27 */
        uint32_t reserved				: 4;		/*! reserved */
    } field;
} LCD_LWINOFS_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t ADDR					: 32;	/*! Bit 0..31 */
    } field;
} LCD_LWINADD_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t COLUMN 				: 12;	/*! Bit 0..11 */
        uint32_t reserved1				: 4; 	/*! reserved */
        uint32_t ROW					: 12;	/*! Bit 16..27 */
        uint32_t reserved				: 4;		/*! reserved */
    } field;
} LCD_LWINSIZE_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t XOFFSET				: 12;	/*! Bit 0..11 */
        uint32_t reserved1				: 4; 	/*! reserved */
        uint32_t YOFFSET				: 12;	/*! Bit 16..27 */
        uint32_t reserved				: 4;		/*! reserved */
    } field;
} LCD_LWINMOFS_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t PITCH					: 16;	/*! Bit 0..16 */
        uint32_t reserved				: 16;	/*! reserved */
    } field;
} LCD_LWINPITCH_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DB_B					: 2; 	/*! Bit 0..1 */
        uint32_t reserved3				: 2; 	/*! reserved */
        uint32_t DB_G					: 2; 	/*! Bit 4..5 */
        uint32_t reserved2				: 2; 	/*! reserved */
        uint32_t DB_R					: 2; 	/*! Bit 8..9 */
        uint32_t reserved1				: 17;	/*! reserved */
        uint32_t PQ_DITHER_EN			: 1; 	/*! Bit 27 */
        uint32_t reserved				: 4;		/*! reserved */
    } field;
} LCD_DITHER_CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t BCLK_DCM_EN			: 1; 	/*! Bit 0 */
        uint32_t SCLK_DCM_EN			: 1; 	/*! Bit 1 */
        uint32_t reserved				: 30;	/*! reserved */
    } field;
} LCD_DCM_CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t XOFFSET				: 12;	/*! Bit 0..11 */
        uint32_t reserved1				: 4; 	/*! reserved */
        uint32_t YOFFSET				: 12;	/*! Bit 16..27 */
        uint32_t reserved				: 4;		/*! reserved */
    } field;
} LCD_DB_ADDCON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t XOFFSET				: 12;	/*! Bit 0..11 */
        uint32_t reserved1				: 4; 	/*! reserved */
        uint32_t YOFFSET				: 12;	/*! Bit 16..27 */
        uint32_t reserved				: 4;		/*! reserved */
    } field;
} LCD_DB_MAINCON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t START					: 1; 	/*! Bit 0 */
        uint32_t CLEAR					: 1; 	/*! Bit 1 */
        uint32_t reserved1				: 2; 	/*! reserved */
        uint32_t CNT_SEL				: 2; 	/*! Bit 4..5 */
        uint32_t reserved				: 26;	/*! reserved */
    } field;
} LCD_CNT_CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t LCD_CNTB				: 16;	/*! Bit 0..15 */
        uint32_t LCD_CNTA				: 16;	/*! Bit 16..31 */
    } field;
} LCD_CNT_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DBI_ULTRA_EN			: 1; 	/*! Bit 0 */
        uint32_t GMC_ULTRA_EN			: 1; 	/*! Bit 1 */
        uint32_t reserved				: 30;	/*! reserved */
    } field;
} LCD_ULTRA_CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CONSUME_PXLS			: 10;	/*! Bit 0..9 */
        uint32_t reserved				: 22;	/*! reserved */
    } field;
} LCD_CONSUME_RATE_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DBI_TH_LOW 			: 16;	/*! Bit 0..15 */
        uint32_t DBI_TH_HIGH			: 16;	/*! Bit 16..31 */
    } field;
} LCD_DBI_ULTRA_TH_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t GMC_TH_LOW 			: 16;	/*! Bit 0..15 */
        uint32_t GMC_TH_HIGH			: 16;	/*! Bit 16..31 */
    } field;
} LCD_GMC_ULTRA_TH_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t SIF0_STR_DATA_SIZE 	: 3; 	/*! Bit 0..2 */
        uint32_t reserved2				: 3; 	/*! reserved */
        uint32_t SIF0_STR_BYTE_SWITCH	: 1; 	/*! Bit 6 */
        uint32_t SIF0_STR_BYTE_MOD	: 1; 	/*! Bit 7 */
        uint32_t SIF1_STR_DATA_SIZE 	: 3; 	/*! Bit 8..10 */
        uint32_t reserved1				: 3; 	/*! reserved */
        uint32_t SIF1_STR_BYTE_SWITCH	: 1; 	/*! Bit 14 */
        uint32_t SIF1_STR_BYTE_MOD	: 1; 	/*! Bit 15 */
        uint32_t reserved				: 16;	/*! reserved */
    } field;
} LCD_SIF_STR_BYTE_CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t SIF0_WR_STR_BYTE		: 8; 	/*! Bit 0..7 */
        uint32_t SIF1_WR_STR_BYTE		: 8; 	/*! Bit 8..15 */
        uint32_t SIF0_WR_STR_BYTE2		: 8; 	/*! Bit 16..23 */
        uint32_t SIF1_WR_STR_BYTE2		: 8;		/*! Bit 23..31 */
    } field;
} LCD_SIF_WR_STR_BYTE_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t SIF0_RD_STR_BYTE		: 8; 	/*! Bit 0..7 */
        uint32_t SIF1_RD_STR_BYTE		: 8; 	/*! Bit 8..15 */
        uint32_t reserved				: 16;	/*! reserved */
    } field;
} LCD_SIF_RD_STR_BYTE_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CYCLE_PER_PIX			: 8; 	/*! Bit 0..7 */
        uint32_t reserved				: 8; 	/*! reserved */
        uint32_t VBP_PIX_NUM			: 8; 	/*! Bit 15..23 */
        uint32_t VBP_LN_NUM				: 8;		/*! Bit 24..31 */
    } field;
} LCD_SIF_VDO_SYNC_CON0_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t HFP_PIX_NUM			: 8; 	/*! Bit 0..7 */
        uint32_t HFP_LN_NUM 			: 8; 	/*! Bit 8..15 */
        uint32_t HBP_PIX_NUM			: 8; 	/*! Bit 16..23 */
        uint32_t HBP_LN_NUM				: 8;		/*! Bit 24..31 */
    } field;
} LCD_SIF_VDO_SYNC_CON1_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t SIF_FR_DURATION		: 32;	/*! Bit 0..31 */
    } field;
} LCD_FR_DURATION_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t VDO_LN_STR_HDR 		: 16;	/*! Bit 0..15 */
        uint32_t VDO_FR_STR_HDR			: 16;	/*! Bit 16..31 */
    } field;
} LCD_SIF_VDO_HEADER_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t LSDI_SEL				: 3; 	/*! Bit 0..2 */
        uint32_t reserved1				: 13;	/*! reserved */
        uint32_t LSDA_SEL				: 3; 	/*! Bit 16..18 */
        uint32_t reserved				: 13;	/*! reserved */
    } field;
} LCD_SERIAL_PAD_SEL_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t INDEX0_RGB565			: 16;	/*! Bit 0..15 */
        uint32_t INDEX1_RGB565			: 16;	/*! Bit 16..31 */
    } field;
} LCD_TABLE_INDEX_COLOR_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CMD					: 32;	/*! Bit 0..31 */
    } field;
} LCD_SCMD0_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CMD					: 32;	/*! Bit 0..31 */
    } field;
} LCD_SPE_SCMD0_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DATA					: 32;	/*! Bit 0..31 */
    } field;
} LCD_SDAT0_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DATA					: 32;	/*! Bit 0..31 */
    } field;
} LCD_SPE_SDAT0_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CMD					: 32;	/*! Bit 0..31 */
    } field;
} LCD_SCMD1_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CMD					: 32;	/*! Bit 0..31 */
    } field;
} LCD_SPE_SCMD1_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DATA					: 32;	/*! Bit 0..31 */
    } field;
} LCD_SDAT1_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DATA					: 32;	/*! Bit 0..31 */
    } field;
} LCD_SPE_SDAT1_REGISTER_T;

typedef struct {
    LCD_LWINCON_REGISTER_T		lcd_lwincon_register;
    LCD_LWINKEY_REGISTER_T		lcd_lwinkey_register;
    LCD_LWINOFS_REGISTER_T		lcd_lwinofs_register;
    LCD_LWINADD_REGISTER_T		lcd_lwinadd_register;
    LCD_LWINSIZE_REGISTER_T		lcd_lwinsize_register;
    uint32_t						reseved;
    LCD_LWINMOFS_REGISTER_T		lcd_lwinmofs_register;
    LCD_LWINPITCH_REGISTER_T		lcd_lwinpitch_register;
    uint32_t						reseved1[4];
} LCD_LAYER_REGISTER_T;

typedef struct {
    LCD_STA_REGISTER_T					lcd_sta_register;
    LCD_INTEN_REGISTER_T					lcd_inten_register;
    LCD_INTSTA_REGISTER_T				lcd_intsta_register;
    volatile LCD_START_REGISTER_T					lcd_start_register;
    LCD_RSTB_REGISTER_T 					lcd_rstb_register;
    uint32_t								reserved;
    LCD_SIF_PIX_CON_REGISTER_T			lcd_sif_pix_con_register;
    LCD_SIF_TIMING_REGISTER_T			lcd_sif_timing_register[2];
    uint32_t								reserved1;
    LCD_SIF_CON_REGISTER_T				lcd_sif_con_register;
    LCD_SIF_CS_REGISTER_T				lcd_sif_cs_register;
    uint32_t								reserved2[5];
    LCD_CALC_HTT_REGISTER_T 				lcd_calc_htt_register;
    LCD_SYNC_LCM_SIZE_REGISTER_T		lcd_sync_lcm_size_register;
    LCD_SYNC_CNT_REGISTER_T 				lcd_sync_cnt_register;
    LCD_TECON_REGISTER_T				lcd_tecon_register;
    LCD_GMCCON_REGISTER_T				lcd_gmccon_register;
    uint32_t								reserved3[9];
    LCD_PAL_ADD_REGISTER_T				lcd_pal_add_register;
    LCD_WROICON_REGISTER_T				lcd_wroicon_register;
    LCD_WROIOFS_REGISTER_T				lcd_wroiofs_register;
    LCD_WROICADD_REGISTER_T 			lcd_wroicadd_register;
    LCD_WROIDADD_REGISTER_T 			lcd_wroidadd_register;
    LCD_WROISIZE_REGISTER_T 				lcd_wroisize_register;
    uint32_t								reserved4[2];
    LCD_WROI_BGCLR_REGISTER_T			lcd_wroi_bgclr_register;
    uint32_t								reserved5[4];
    LCD_LAYER_REGISTER_T					lcd_layer_register[4];
    LCD_DITHER_CON_REGISTER_T			lcd_dither_con_register;
    LCD_DCM_CON_REGISTER_T				lcd_dcm_con_register;
    uint32_t								reserved6[30];
    LCD_DB_ADDCON_REGISTER_T			lcd_db_addcon_register;
    LCD_DB_MAINCON_REGISTER_T			lcd_db_maincon_register;
    uint32_t								reserved7[10];
    LCD_CNT_CON_REGISTER_T				lcd_cnt_con_register;
    LCD_CNT_REGISTER_T					lcd_cnt_register;
    uint32_t								reserved8[6];
    LCD_ULTRA_CON_REGISTER_T			lcd_ultra_con_register;
    LCD_CONSUME_RATE_REGISTER_T			lcd_consume_rate_register;
    LCD_DBI_ULTRA_TH_REGISTER_T 			lcd_dbi_ultra_th_register;
    LCD_GMC_ULTRA_TH_REGISTER_T 			lcd_gmc_ultra_th_register;
    uint32_t								reserved9[8];
    LCD_SIF_STR_BYTE_CON_REGISTER_T 		lcd_sif_str_byte_con_register;
    uint32_t								reserved10;
    LCD_SIF_WR_STR_BYTE_REGISTER_T		lcd_sif_wr_str_byte_register;
    LCD_SIF_RD_STR_BYTE_REGISTER_T		lcd_sif_rd_str_byte_register;
    uint32_t								reserved11[4];
    LCD_SIF_VDO_SYNC_CON0_REGISTER_T	lcd_sif_vdo_sync_con0_register;
    LCD_SIF_VDO_SYNC_CON1_REGISTER_T	lcd_sif_vdo_sync_con1_register;
    LCD_FR_DURATION_REGISTER_T			lcd_fr_duration_register;
    LCD_SIF_VDO_HEADER_REGISTER_T		lcd_sif_vdo_header_register;
    uint32_t								reserved12[24];
    LCD_SERIAL_PAD_SEL_REGISTER_T		lcd_serial_pad_sel_register;
    uint32_t								reserved13[63];
    LCD_TABLE_INDEX_COLOR_REGISTER_T	lcd_table_index_color_register[8];
    uint32_t								reserved14[728];
    LCD_SCMD0_REGISTER_T				lcd_scmd0_register;
    uint32_t								reserved15;
    LCD_SPE_SCMD0_REGISTER_T			lcd_spe_scmd0_register;
    uint32_t								reserved16;
    LCD_SDAT0_REGISTER_T					lcd_sdat0_register;
    uint32_t								reserved17;
    LCD_SPE_SDAT0_REGISTER_T				lcd_spe_sdat0_register;
    uint32_t								reserved18;
    LCD_SCMD1_REGISTER_T				lcd_scmd1_register;
    uint32_t								reserved19;
    LCD_SPE_SCMD1_REGISTER_T			lcd_spe_scmd1_register;
    uint32_t								reserved20;
    LCD_SDAT1_REGISTER_T					lcd_sdat1_register;
    uint32_t								reserved21;
    LCD_SPE_SDAT1_REGISTER_T				lcd_spe_sdat1_register;
} LCD_REGISTER_T;

typedef struct {
    LCD_PAL_ADD_REGISTER_T				lcd_pal_add_register;
    LCD_WROICON_REGISTER_T				lcd_wroicon_register;
    LCD_WROIOFS_REGISTER_T				lcd_wroiofs_register;
    LCD_WROICADD_REGISTER_T 			lcd_wroicadd_register;
    LCD_WROIDADD_REGISTER_T 			lcd_wroidadd_register;
    LCD_WROISIZE_REGISTER_T 				lcd_wroisize_register;
    LCD_WROI_BGCLR_REGISTER_T			lcd_wroi_bgclr_register;
    LCD_LAYER_REGISTER_T					lcd_layer_register[4];
} backup_register_t;

// Define macros
// LCD STA
#define LCD_BUSY								(lcd_register_ptr->lcd_sta_register.field.BUSY)
#define LCD_IS_RUNNING						(lcd_register_ptr->lcd_sta_register.field.RUN)
#define LCD_WAIT_TE					 		(lcd_register_ptr->lcd_sta_register.field.WAIT_SYNC)

// LCD INTEN
#define ENABLE_LCD_TRANSFER_COMPLETE_INT   	lcd_register_ptr->lcd_inten_register.field.CPL = 1;
#define DISABLE_LCD_TRANSFER_COMPLETE_INT  	lcd_register_ptr->lcd_inten_register.field.CPL = 0;
#define ENABLE_LCD_REG_COMPLETE_INT 	   		lcd_register_ptr->lcd_inten_register.field.REG_CPL = 1;
#define DISABLE_LCD_REG_COMPLETE_INT	   		lcd_register_ptr->lcd_inten_register.field.REG_CPL = 0;
#define ENABLE_LCD_CMD_COMPLETE_INT 	   		lcd_register_ptr->lcd_inten_register.field.CMDQ_CPL = 1;
#define DISABLE_LCD_CMD_COMPLETE_INT	   		lcd_register_ptr->lcd_inten_register.field.CMDQ_CPL = 0;
#define ENABLE_LCD_SYNC_COMPLETE_INT	  		lcd_register_ptr->lcd_inten_register.field.SYNC = 1;
#define DISABLE_LCD_SYNC_COMPLETE_INT	   	lcd_register_ptr->lcd_inten_register.field.SYNC = 0;
#define ENABLE_LCD_HTT_COMPLETE_INT 	   		lcd_register_ptr->lcd_inten_register.field.HTT = 1;
#define DISABLE_LCD_HTT_COMPLETE_INT	   		lcd_register_ptr->lcd_inten_register.field.HTT = 0;

// LCD START
#define SET_LCD_CTRL_RESET_PIN		 		lcd_register_ptr->lcd_rstb_register.field.RSTB = 1;
#define CLEAR_LCD_CTRL_RESET_PIN	 			lcd_register_ptr->lcd_rstb_register.value = 0;
#define STOP_LCD_TRANSFER			 		lcd_register_ptr->lcd_start_register.value = 0;

#define START_LCD_TRANSFER					lcd_register_ptr->lcd_start_register.value = 0;\
									 		lcd_register_ptr->lcd_start_register.field.START = 1;

// LCD Serial interface pixel config
#define ENABLE_LCD_SERIAL0_2PIN 		  		lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_PIX_2PIN = 1;
#define DISABLE_LCD_SERIAL0_2PIN		  		lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_PIX_2PIN = 0;
#define ENABLE_LCD_SERIAL1_2PIN 		  		lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_PIX_2PIN = 1;
#define DISABLE_LCD_SERIAL1_2PIN		  		lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_PIX_2PIN = 0;
#define ENABLE_LCD_SERIAL0_SINGLE_A0	  		lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_SINGLE_A0 = 1;
#define DISABLE_LCD_SERIAL0_SINGLE_A0	  		lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_SINGLE_A0 = 0;
#define ENABLE_LCD_SERIAL1_SINGLE_A0	  		lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_SINGLE_A0 = 1;
#define DISABLE_LCD_SERIAL1_SINGLE_A0	  		lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_SINGLE_A0 = 0;
#define ENABLE_LCD_SERIAL0_CS_STAY_LOW	  	lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_CS_STAY_LOW = 1;
#define DISABLE_LCD_SERIAL0_CS_STAY_LOW  		lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_CS_STAY_LOW = 0;
#define ENABLE_LCD_SERIAL1_CS_STAY_LOW	  	lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_CS_STAY_LOW = 1;
#define DISABLE_LCD_SERIAL1_CS_STAY_LOW   		lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_CS_STAY_LOW = 0;
#define SET_LCD_SERIAL0_IF_2PIN_SIZE(n)   		lcd_register_ptr->lcd_sif_pix_con_register.field.SIF0_2PIN_SIZE = n;
#define SET_LCD_SERIAL1_IF_2PIN_SIZE(n)   		lcd_register_ptr->lcd_sif_pix_con_register.field.SIF1_2PIN_SIZE = n;
#define SET_LCD_SERIAL_IF_DDR_SCK_HW(n)   		lcd_register_ptr->lcd_sif_pix_con_register.field.SIF_DDR_SCK_HW = n;
#define ENABLE_LCD_SERIAL_DDR_DIV4			lcd_register_ptr->lcd_sif_pix_con_register.field.SIF_DDR_DIV4 = 1;
#define DISABLE_LCD_SERIAL_DDR_DIV4			lcd_register_ptr->lcd_sif_pix_con_register.field.SIF_DDR_DIV4 = 0;
#define ENABLE_LCD_SERIAL_DDR_DIV8			lcd_register_ptr->lcd_sif_pix_con_register.field.SIF_DDR_DIV8 = 1;
#define DISABLE_LCD_SERIAL_DDR_DIV8			lcd_register_ptr->lcd_sif_pix_con_register.field.SIF_DDR_DIV8 = 0;
#define SET_LCD_SERIAL_IF_SCK_SDA_SKEW(n)   	lcd_register_ptr->lcd_sif_pix_con_register.field.SCK_SDA_SKEW = n;


// LCD Serial interface time setting
#define SET_LCD_SERIAL_IF_CSS(ifnum, n) 	  		lcd_register_ptr->lcd_sif_timing_register[ifnum].field.CSS = n;
#define SET_LCD_SERIAL_IF_CSH(ifnum, n) 	 		lcd_register_ptr->lcd_sif_timing_register[ifnum].field.CSH = n;
#define SET_LCD_SERIAL_IF_RD_1ST(ifnum, n)		lcd_register_ptr->lcd_sif_timing_register[ifnum].field.RD_1ST = n;
#define SET_LCD_SERIAL_IF_RD_2ND(ifnum, n)	 	lcd_register_ptr->lcd_sif_timing_register[ifnum].field.RD_2ND = n;
#define SET_LCD_SERIAL_IF_WR_1ST(ifnum, n)	 	lcd_register_ptr->lcd_sif_timing_register[ifnum].field.WR_1ST = n;
#define SET_LCD_SERIAL_IF_WR_2ND(ifnum, n)	 	lcd_register_ptr->lcd_sif_timing_register[ifnum].field.WR_2ND = n;

// LCD Serial interface config
#define ENABLE_LCD_SERIAL0_SDI			  	lcd_register_ptr->lcd_sif_con_register.field.SIF0_SDI = 1;
#define DISABLE_LCD_SERIAL0_SDI 		  		lcd_register_ptr->lcd_sif_con_register.field.SIF0_SDI = 0;
#define ENABLE_LCD_SERIAL1_SDI			  	lcd_register_ptr->lcd_sif_con_register.field.SIF1_SDI = 1;
#define DISABLE_LCD_SERIAL1_SDI 		  		lcd_register_ptr->lcd_sif_con_register.field.SIF1_SDI = 0;
#define ENABLE_LCD_SERIAL0_3WIRE		  		lcd_register_ptr->lcd_sif_con_register.field.SIF0_3WIRE = 1;
#define DISABLE_LCD_SERIAL0_3WIRE		 		lcd_register_ptr->lcd_sif_con_register.field.SIF0_3WIRE = 0;
#define ENABLE_LCD_SERIAL1_3WIRE		 		lcd_register_ptr->lcd_sif_con_register.field.SIF1_3WIRE = 1;
#define DISABLE_LCD_SERIAL1_3WIRE		  		lcd_register_ptr->lcd_sif_con_register.field.SIF1_3WIRE = 0;
#define SET_LCD_SERIAL0_IF_SIZE(n)   			lcd_register_ptr->lcd_sif_con_register.field.SIF0_SIZE = n;
#define SET_LCD_SERIAL1_IF_SIZE(n)   			lcd_register_ptr->lcd_sif_con_register.field.SIF1_SIZE = n;
#define SET_LCD_SERIAL0_CLOCK_PHASE_HIGH		lcd_register_ptr->lcd_sif_con_register.field.SIF0_1ST_POL = 1;
#define SET_LCD_SERIAL0_CLOCK_PHASE_LOW		lcd_register_ptr->lcd_sif_con_register.field.SIF0_1ST_POL = 0;
#define SET_LCD_SERIAL1_CLOCK_PHASE_HIGH		lcd_register_ptr->lcd_sif_con_register.field.SIF1_1ST_POL = 1;
#define SET_LCD_SERIAL1_CLOCK_PHASE_LOW		lcd_register_ptr->lcd_sif_con_register.field.SIF1_1ST_POL = 0;
#define SET_LCD_SERIAL0_CLOCK_DEF_HIGH		lcd_register_ptr->lcd_sif_con_register.field.SIF0_SCK_DEF = 1;
#define SET_LCD_SERIAL0_CLOCK_DEF_LOW		lcd_register_ptr->lcd_sif_con_register.field.SIF0_SCK_DEF = 0;
#define SET_LCD_SERIAL1_CLOCK_DEF_HIGH		lcd_register_ptr->lcd_sif_con_register.field.SIF1_SCK_DEF = 1;
#define SET_LCD_SERIAL1_CLOCK_DEF_LOW		lcd_register_ptr->lcd_sif_con_register.field.SIF1_SCK_DEF = 0;
#define ENABLE_LCD_SERIAL0_DIV2				lcd_register_ptr->lcd_sif_con_register.field.SIF0_DIV2 = 1;
#define DISABLE_LCD_SERIAL0_DIV2				lcd_register_ptr->lcd_sif_con_register.field.SIF0_DIV2 = 0;
#define ENABLE_LCD_SERIAL1_DIV2				lcd_register_ptr->lcd_sif_con_register.field.SIF1_DIV2 = 1;
#define DISABLE_LCD_SERIAL1_DIV2				lcd_register_ptr->lcd_sif_con_register.field.SIF1_DIV2 = 0;
#define ENABLE_LCD_SERIAL_IF_HW_CS			lcd_register_ptr->lcd_sif_con_register.field.SIF_HW_CS = 1;
#define DISABLE_LCD_SERIAL_IF_HW_CS 	   		lcd_register_ptr->lcd_sif_con_register.field.SIF_HW_CS = 0;
#define ENABLE_LCD_SERIAL_IF_VDO_MODE		lcd_register_ptr->lcd_sif_con_register.field.SIF_VDO_MODE = 1;
#define DISABLE_LCD_SERIAL_IF_VDO_MODE		lcd_register_ptr->lcd_sif_con_register.field.SIF_VDO_MODE = 0;
#define ENABLE_LCD_SERIAL_IF_CMD_LOCK		lcd_register_ptr->lcd_sif_con_register.field.SIF_CMD_LOCK = 1;
#define DISABLE_LCD_SERIAL_IF_CMD_LOCK		lcd_register_ptr->lcd_sif_con_register.field.SIF_CMD_LOCK= 0;
#define ENABLE_LCD_SERIAL_IF_VDO_AUTO		lcd_register_ptr->lcd_sif_con_register.field.SIF_VDO_AUTO = 1;
#define DISABLE_LCD_SERIAL_IF_VDO_AUTO		lcd_register_ptr->lcd_sif_con_register.field.SIF_VDO_AUTO = 0;
#define ENABLE_LCD_SERIAL_IF_SYNC_ALIGN		lcd_register_ptr->lcd_sif_con_register.field.SIF_SYNC_ALIGN = 1;
#define DISABLE_LCD_SERIAL_IF_SYNC_ALIGN		lcd_register_ptr->lcd_sif_con_register.field.SIF_SYNC_ALIGN = 0;
#define ENABLE_LCD_SERIAL_IF_DDR_MODE		lcd_register_ptr->lcd_sif_con_register.field.SIF_DDR_EN_CONFIG = 1;
#define DISABLE_LCD_SERIAL_IF_DDR_MODE		lcd_register_ptr->lcd_sif_con_register.field.SIF_DDR_EN_CONFIG = 0;

// LCD Serial interface CS
#define ENABLE_LCD_SERIAL0_CS					lcd_register_ptr->lcd_sif_cs_register.field.CS0 = 0;
#define DISABLE_LCD_SERIAL0_CS				lcd_register_ptr->lcd_sif_cs_register.field.CS0 = 1;
#define ENABLE_LCD_SERIAL1_CS					lcd_register_ptr->lcd_sif_cs_register.field.CS1 = 0;
#define DISABLE_LCD_SERIAL1_CS				lcd_register_ptr->lcd_sif_cs_register.field.CS1 = 1;

// LCD calc HTT
#define GET_TE_CALC_HTT_REG_VAL(n)	  		(n) = lcd_register_ptr->lcd_calc_htt_register.value
#define SET_TE_CALC_HTT_REG_VAL(n)	 		lcd_register_ptr->lcd_calc_htt_register.value= (n)

// LCD Sync LCM size		// ASK PETER
#define SET_TE_DELAYTIME_MODE_HSYNC_WIDTH(n)	lcd_register_ptr->lcd_sync_lcm_size_register.field.HTT = (n);
#define SET_TE_DELAYTIME_MODE_VSYNC_WIDTH(n)	lcd_register_ptr->lcd_sync_lcm_size_register.field.VTT = (n);

// LCD Sync counter
#define SET_TE_SYNC_COUNTER_WAITLINE(n)  		lcd_register_ptr->lcd_sync_cnt_register.field.WAITLINE = (n);
#define GET_TE_SYNC_COUNTER_WAITLINE(n)  		(n) = lcd_register_ptr->lcd_sync_cnt_register.field.WAITLINE;
#define GET_TE_SYNC_COUNTER_SCANLINE(n)  		(n) = lcd_register_ptr->lcd_sync_cnt_register.field.SCANLINE;

// LCD TE
#define ENABLE_LCD_TE_DETECT					lcd_register_ptr->lcd_tecon_register.field.SYNC_EN = 1;
#define DISABLE_LCD_TE_DETECT 	 			lcd_register_ptr->lcd_tecon_register.field.SYNC_EN = 0;
#define SET_TE_RISING_DETECT		 			lcd_register_ptr->lcd_tecon_register.field.TE_EDGE_SEL = 0;
#define SET_TE_FALLING_DETECT 		 			lcd_register_ptr->lcd_tecon_register.field.TE_EDGE_SEL = 1;
#define SET_TE_DELAYTIME_SYNC_MODE 			lcd_register_ptr->lcd_tecon_register.field.SYNC_MODE = 0;
#define SET_TE_SCANLINE_MODE					lcd_register_ptr->lcd_tecon_register.field.SYNC_MODE = 1;
#define ENABLE_LCD_TE_REPEAT_MODE			lcd_register_ptr->lcd_tecon_register.field.TE_REPEAT = 1;
#define DISABLE_LCD_TE_REPEAT_MODE			lcd_register_ptr->lcd_tecon_register.field.TE_REPEAT = 0;
#define SET_DSI_START_CTL_VSYNC				lcd_register_ptr->lcd_tecon_register.field.DSI_START_CTL = 0;
#define SET_DSI_START_CTL_TE					lcd_register_ptr->lcd_tecon_register.field.DSI_START_CTL = 1;
#define SET_DSI_END_CTL_VDE					lcd_register_ptr->lcd_tecon_register.field.DSI_END_CTL = 0;
#define SET_DSI_END_CTL_FRM_DONE				lcd_register_ptr->lcd_tecon_register.field.DSI_END_CTL = 1;
#define ENABLE_LCD_SW_TE						lcd_register_ptr->lcd_tecon_register.field.SW_TE = 1;
#define DISABLE_LCD_SW_TE					lcd_register_ptr->lcd_tecon_register.field.SW_TE = 0;

// LCD GMC comtrol
#define LCD_ENABLE_GMCCON_THROTTLE()			lcd_register_ptr->lcd_gmccon_register.field.THROTTLE_EN = 1;
#define LCD_DISABLE_GMCCON_THROTTLE()		lcd_register_ptr->lcd_gmccon_register.field.THROTTLE_EN = 0;
#define LCD_SET_GMCCON_BURST_SIZE(n)			lcd_register_ptr->lcd_gmccon_register.field.MAX_BURST = (n)
#define LCD_SET_GMCCON_THROTTLE_PERIOD(n)	lcd_register_ptr->lcd_gmccon_register.field.THROTTLE_PERIOD = (n)
#define LCD_GET_GMCCON_BURST_SIZE(n)			(n) = lcd_register_ptr->lcd_gmccon_register.field.MAX_BURST;

// LCD PAL ADDR
#define SET_LCD_PAL_ADDR(n)					lcd_register_ptr->lcd_pal_add_register.value = (n)

// LCD ROI control
#define SET_LCD_SEND_RES_MODE(n)				lcd_register_ptr->lcd_wroicon_register.field.SEND_RES_MOD = (n)
#define SET_LCD_ROI_CTRL_NUMBER_OF_CMD(n)	lcd_register_ptr->lcd_wroicon_register.field.COMMAND = (n)
#define ENABLE_LCD_LAYER0					lcd_register_ptr->lcd_wroicon_register.field.EN0 = 1;
#define DISABLE_LCD_LAYER0					lcd_register_ptr->lcd_wroicon_register.field.EN0 = 0;
#define ENABLE_LCD_LAYER1					lcd_register_ptr->lcd_wroicon_register.field.EN1 = 1;
#define DISABLE_LCD_LAYER1					lcd_register_ptr->lcd_wroicon_register.field.EN1 = 0;
#define ENABLE_LCD_LAYER2					lcd_register_ptr->lcd_wroicon_register.field.EN2 = 1;
#define DISABLE_LCD_LAYER2					lcd_register_ptr->lcd_wroicon_register.field.EN2 = 0;
#define ENABLE_LCD_LAYER3					lcd_register_ptr->lcd_wroicon_register.field.EN3 = 1;
#define DISABLE_LCD_LAYER3					lcd_register_ptr->lcd_wroicon_register.field.EN3 = 0;
#define SET_LCD_COLOR_EN(n)					lcd_register_ptr->lcd_wroicon_register.field.COLOR_EN = (n)
#define GET_LCD_COLOR_EN(n)					(n) = lcd_register_ptr->lcd_wroicon_register.field.COLOR_EN
#define ENABLE_LCD_COLOR						lcd_register_ptr->lcd_wroicon_register.field.COLOR_EN = 1;
#define DISABLE_LCD_COLOR					lcd_register_ptr->lcd_wroicon_register.field.COLOR_EN = 0;
#define ENABLE_LCD_24IF						lcd_register_ptr->lcd_wroicon_register.field._24IF = 1;
#define DISABLE_LCD_24IF						lcd_register_ptr->lcd_wroicon_register.field._24IF = 0;
#define ENABLE_LCD_ROI_CTRL_CMD_FIRST			lcd_register_ptr->lcd_wroicon_register.field.ENC = 1;
#define DISABLE_LCD_ROI_CTRL_CMD_FIRST		lcd_register_ptr->lcd_wroicon_register.field.ENC = 0;
#define SET_LCD_ROI_CTRL_OUTPUT_FORMAT(n)		lcd_register_ptr->lcd_wroicon_register.field.FORMAT = (n)
#define GET_LCD_ROI_CTRL_OUTPUT_FORMAT(n)		(n) = lcd_register_ptr->lcd_wroicon_register.field.FORMAT
#define DISABLE_ALL_LCD_LAYER_WINDOW			lcd_register_ptr->lcd_wroicon_register.value &= 0x0FFFFFFF;

#define SET_LCD_ROI_WINDOW_OFFSET(x, y)		lcd_register_ptr->lcd_wroiofs_register.value =  (( y << 16) | x);
#define SET_LCD_ROI_COMMAND_ADDR(n)			lcd_register_ptr->lcd_wroicadd_register.value =  (n);
#define SET_LCD_ROI_DATA_ADDR(n)				lcd_register_ptr->lcd_wroidadd_register.value =  (n);
#define SET_LCD_ROI_WINDOW_SIZE(column,row)	lcd_register_ptr->lcd_wroisize_register.value = ((row << 16) | column);
#define SET_LCD_ROI_BG_COLOR(color)			lcd_register_ptr->lcd_wroi_bgclr_register.value = (color);
#define GET_LCD_ROI_WIDTH(n)					(n) = lcd_register_ptr->lcd_wroisize_register.field.COLUMN
#define GET_LCD_ROI_HEIGHT(n)					(n) = lcd_register_ptr->lcd_wroisize_register.field.ROW


// LCD_LAYER
#define LCD_TOTAL_LAYER						4
#define LCD_MAX_OPACITY						0xFF

// config
#define DISABLE_LAYER_RGB_SWAP(index)					lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.RGB_SWP = 0;
#define ENABLE_LAYER_RGB_SWAP(index)					lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.RGB_SWP = 1;
#define DISABLE_LCD_LAYER_DST_KEY_EN(index)	 		lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.DST_KEYEN = 0;
#define ENABLE_LCD_LAYER_DST_KEY_EN(index)	 		lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.DST_KEYEN = 1;
#define SET_LCD_LAYER_DST_KEY_EN(index, n) 				lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.DST_KEYEN = n;
#define GET_LCD_LAYER_DST_KEY_EN(index, n) 				(n) = lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.DST_KEYEN;
#define SET_LCD_LAYER_COLOR_FORMAT(index, n)			lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.CLR_FMT = n;
#define GET_LCD_LAYER_COLOR_FORMAT(index, n)			(n) = lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.CLR_FMT;
#define SET_LCD_LAYER_DITHER_EN(index, n) 				lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.DITHER_EN = n;
#define GET_LCD_LAYER_DITHER_EN(index, n) 				(n) = lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.DITHER_EN;
#define DISABLE_LCD_LAYER_DITHER_EN(index)				lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.DITHER_EN = 0;
#define ENABLE_LCD_LAYER_DITHER_EN(index)				lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.DITHER_EN = 1;
#define DISABLE_LAYER_READ_DATA_SWAP(index)			lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.BYTE_SWP = 0;
#define ENABLE_LAYER_READ_DATA_SWAP(index)			lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.BYTE_SWP = 1;
#define DISABLE_LAYER_ADDR_AUTO_INC(index) 			lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.SRC = 0;
#define ENABLE_LAYER_ADDR_AUTO_INC(index) 			lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.SRC = 1;
#define DISABLE_LCD_LAYER_SOURCE_KEY(index)			lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.SRC_KEYEN = 0;
#define ENABLE_LCD_LAYER_SOURCE_KEY(index)			lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.SRC_KEYEN = 1;
#define SET_LCD_LAYER_SOURCE_KEY_EN(index, n) 			lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.SRC_KEYEN = n;
#define GET_LCD_LAYER_SOURCE_KEY_EN(index, n) 			(n) = lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.SRC_KEYEN;
#define SET_LCD_LAYER_ROTATE(index, n) 					lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.ROTATE = n;
#define GET_LCD_LAYER_ROTATE(index, n) 					(n) = lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.ROTATE;
#define DISABLE_LCD_LAYER_ALPHA(index)				lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.ALPHA_EN = 0;
#define ENABLE_LCD_LAYER_ALPHA(index)					lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.ALPHA_EN = 1;
#define SET_LCD_LAYER_ALPHA_EN(index, n) 				lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.ALPHA_EN = n;
#define GET_LCD_LAYER_ALPHA_EN(index, n) 				(n) = lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.ALPHA_EN;
#define SET_LCD_LAYER_ALPHA(index, n) 					lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.ALPHA = n;
#define GET_LCD_LAYER_ALPHA(index, n) 					(n) = lcd_register_ptr->lcd_layer_register[index].lcd_lwincon_register.field.ALPHA;

// other
#define SET_LCD_LAYER_SOURCE_KEY(index, color)			lcd_register_ptr->lcd_layer_register[index].lcd_lwinkey_register.value = color;
#define SET_LCD_LAYER_WINDOW_OFFSET(index, x, y)		lcd_register_ptr->lcd_layer_register[index].lcd_lwinofs_register.value = ((y << 16) | x);
#define SET_LCD_LAYER_WINDOW_START_ADDR(index, addr)	lcd_register_ptr->lcd_layer_register[index].lcd_lwinadd_register.value = addr;
#define SET_LCD_LAYER_WINDOW_SIZE(index, width, height)	lcd_register_ptr->lcd_layer_register[index].lcd_lwinsize_register.value  = ((height << 16) | width);
#define SET_LCD_LAYER_WINDOW_WMEM_OFFSET(index, x, y)	lcd_register_ptr->lcd_layer_register[index].lcd_lwinmofs_register.value = ((y << 16) | x);
#define SET_LCD_LAYER_WINDOW_WMEM_PITCH(index, width)	lcd_register_ptr->lcd_layer_register[index].lcd_lwinpitch_register.value = width;

// LCD DITHER CON
#define DISABLE_LCD_PQ_DITHER_EN						lcd_register_ptr->lcd_dither_con_register.field.PQ_DITHER_EN = 0;		// Dither before OVL
#define ENABLE_LCD_PQ_DITHER_EN						lcd_register_ptr->lcd_dither_con_register.field.PQ_DITHER_EN = 1;		// Dither after OVL
#define SET_LCD_DITHER_BIT(r, g, b)						lcd_register_ptr->lcd_dither_con_register.field.DB_R = r;\
		lcd_register_ptr->lcd_dither_con_register.field.DB_G = g;\
		lcd_register_ptr->lcd_dither_con_register.field.DB_B = b;
// LCD DCM
#define DISABLE_LCD_BCLK_DCM							lcd_register_ptr->lcd_dcm_con_register.field.BCLK_DCM_EN = 0;
#define ENABLE_LCD_BCLK_DCM							lcd_register_ptr->lcd_dcm_con_register.field.BCLK_DCM_EN = 1;
#define DISABLE_LCD_SCLK_DCM							lcd_register_ptr->lcd_dcm_con_register.field.SCLK_DCM_EN = 0;
#define ENABLE_LCD_SCLK_DCM							lcd_register_ptr->lcd_dcm_con_register.field.SCLK_DCM_EN = 1;


// LCD DB ADDC
#define GET_LCD_DB_ADDCON_X(x)						(x) = lcd_register_ptr->lcd_db_addcon_register.field.XOFFSET;
#define GET_LCD_DB_ADDCON_Y(y)						(y) = lcd_register_ptr->lcd_db_addcon_register.field.YOFFSET;

// LCD DB MAINC
#define GET_LCD_DB_MAINCON_X(x)						(x) = lcd_register_ptr->lcd_db_maincon_register.field.XOFFSET;
#define GET_LCD_DB_MAINCON_Y(y)						(y) = lcd_register_ptr->lcd_db_maincon_register.field.YOFFSET;

// LCD CNT CON
#define SET_LCD_CNT_CLEAR(n)							lcd_register_ptr->lcd_cnt_con_register.field.CLEAR = n;
#define START_LCD_CNT									SET_LCD_CNT_CLEAR(1);\
		SET_LCD_CNT_CLEAR(0);\
		lcd_register_ptr->lcd_cnt_con_register.field.START = 1;
#define SET_LCD_CNT_SEL(n)							lcd_register_ptr->lcd_cnt_con_register.field.CNT_SEL = n;
#define GET_LCD_CNTA(x)								(x) = lcd_register_ptr->lcd_cnt_register.field.LCD_CNTA;
#define GET_LCD_CNTB(x)								(x) = lcd_register_ptr->lcd_cnt_register.field.LCD_CNTB;

// LCD ULTRA con
#define DISABLE_DBI_ULTRA_HIGH						lcd_register_ptr->lcd_ultra_con_register.field.DBI_ULTRA_EN = 0;
#define ENABLE_DBI_ULTRA_HIGH						lcd_register_ptr->lcd_ultra_con_register.field.DBI_ULTRA_EN = 1;
#define DISABLE_GMC_ULTRA_HIGH						lcd_register_ptr->lcd_ultra_con_register.field.GMC_ULTRA_EN = 0;
#define ENABLE_GMC_ULTRA_HIGH						lcd_register_ptr->lcd_ultra_con_register.field.GMC_ULTRA_EN = 1;
#define SET_LCD_CONSUME_RATE(n) 						lcd_register_ptr->lcd_consume_rate_register = n;
#define SET_LCD_DBI_ULTRA_HIGH_THRESHOLD(low, high)	lcd_register_ptr->lcd_dbi_ultra_th_register.value = ((high & 0xFF) << 16) | (low & 0xFF)
#define SET_LCD_GMC_ULTRA_HIGH_THRESHOLD(low, high) 	lcd_register_ptr->lcd_gmc_ultra_th_register.value = ((high & 0xFF) << 16) | (low & 0xFF)

// LCD SIF start byte
#define DISABLE_SIF0_START_BYTE						lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF0_STR_BYTE_MOD = 0;
#define ENABLE_SIF0_START_BYTE						lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF0_STR_BYTE_MOD = 1;
#define DISABLE_SIF0_START_BYE_SWITCH					lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF0_STR_BYTE_SWITCH = 0;
#define ENABLE_SIF0_START_BYE_SWITCH					lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF0_STR_BYTE_SWITCH = 1;
#define SET_SIF0_START_BYTE_SIZE						lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF0_STR_DATA_SIZE = n;
#define GET_SIF0_START_BYTE_SIZE						(n) = lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF0_STR_DATA_SIZE;
#define DISABLE_SIF1_START_BYTE						lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF1_STR_BYTE_MOD = 0;
#define ENABLE_SIF1_START_BYTE						lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF1_STR_BYTE_MOD = 1;
#define DISABLE_SIF1_START_BYE_SWITCH					lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF1_STR_BYTE_SWITCH = 0;
#define ENABLE_SIF1_START_BYE_SWITCH					lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF1_STR_BYTE_SWITCH = 1;
#define SET_SIF1_START_BYTE_SIZE						lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF1_STR_DATA_SIZE = n;
#define GET_SIF1_START_BYTE_SIZE						(n) = lcd_register_ptr->lcd_sif_str_byte_con_register.field.SIF1_STR_DATA_SIZE;
#define SET_SIF0_WR_START_BYTE(n)						lcd_register_ptr->lcd_sif_wr_str_byte_register.field.SIF0_WR_STR_BYTE = n;
#define GET_SIF0_WR_START_BYTE(n)						(n) = lcd_register_ptr->lcd_sif_wr_str_byte_register.field.SIF0_WR_STR_BYTE;
#define SET_SIF0_WR_START_BYTE2(n)					lcd_register_ptr->lcd_sif_wr_str_byte_register.field.SIF0_WR_STR_BYTE2 = n;
#define GET_SIF0_WR_START_BYTE2(n)					(n) = lcd_register_ptr->lcd_sif_wr_str_byte_register.field.SIF0_WR_STR_BYTE2;
#define SET_SIF1_WR_START_BYTE(n)						lcd_register_ptr->lcd_sif_wr_str_byte_register.field.SIF1_WR_STR_BYTE = n;
#define GET_SIF1_WR_START_BYTE(n)						(n) = lcd_register_ptr->lcd_sif_wr_str_byte_register.field.SIF1_WR_STR_BYTE;
#define SET_SIF1_WR_START_BYTE2(n)					lcd_register_ptr->lcd_sif_wr_str_byte_register.field.SIF1_WR_STR_BYTE2 = n;
#define GET_SIF1_WR_START_BYTE2(n)					(n) = lcd_register_ptr->lcd_sif_wr_str_byte_register.field.SIF1_WR_STR_BYTE2;
#define SET_SIF0_RD_START_BYTE(n)						lcd_register_ptr->lcd_sif_rd_str_byte_register.field.SIF0_RD_STR_BYTE = n;
#define GET_SIF0_RD_START_BYTE(n)						(n) = lcd_register_ptr->lcd_sif_rd_str_byte_register.field.SIF0_RD_STR_BYTE;
#define SET_SIF1_RD_START_BYTE(n)						lcd_register_ptr->lcd_sif_rd_str_byte_register.field.SIF1_RD_STR_BYTE = n;
#define GET_SIF1_RD_START_BYTE(n)						(n) = lcd_register_ptr->lcd_sif_rd_str_byte_register.field.SIF1_RD_STR_BYTE;


// LCD VDO mode
#define SET_VDO_MODE_VBP_LN_NUM(n)					lcd_register_ptr->lcd_sif_vdo_sync_con0_register.field.VBP_LN_NUM = n;
#define GET_VDO_MODE_VBP_LN_NUM(n)					(n) = lcd_register_ptr->lcd_sif_vdo_sync_con0_register.field.VBP_LN_NUM;
#define SET_VDO_MODE_VBP_PIX_NUM(n)					lcd_register_ptr->lcd_sif_vdo_sync_con0_register.field.VBP_PIX_NUM = n;
#define GET_VDO_MODE_VBP_PIX_NUM(n)					(n) = lcd_register_ptr->lcd_sif_vdo_sync_con0_register.field.VBP_PIX_NUM;
#define SET_VDO_MODE_CYCLE_PER_PIX(n)				lcd_register_ptr->lcd_sif_vdo_sync_con0_register.field.CYCLE_PER_PIX = n;
#define GET_VDO_MODE_CYCLE_PER_PIX(n)				(n) = lcd_register_ptr->lcd_sif_vdo_sync_con0_register.field.CYCLE_PER_PIX;
#define SET_VDO_MODE_HBP_LN_NUM(n)					lcd_register_ptr->lcd_sif_vdo_sync_con1_register.field.HBP_LN_NUM = n;
#define GET_VDO_MODE_HBP_LN_NUM(n)					(n) = lcd_register_ptr->lcd_sif_vdo_sync_con1_register.field.HBP_LN_NUM;
#define SET_VDO_MODE_HBP_PIX_NUM(n)					lcd_register_ptr->lcd_sif_vdo_sync_con1_register.field.HBP_PIX_NUM = n;
#define GET_VDO_MODE_HBP_PIX_NUM(n)					(n) = lcd_register_ptr->lcd_sif_vdo_sync_con1_register.field.HBP_PIX_NUM;
#define SET_VDO_MODE_HFP_LN_NUM(n)					lcd_register_ptr->lcd_sif_vdo_sync_con1_register.field.HFP_LN_NUM = n;
#define GET_VDO_MODE_HFP_LN_NUM(n)					(n) = lcd_register_ptr->lcd_sif_vdo_sync_con1_register.field.HFP_LN_NUM;
#define SET_VDO_MODE_HFP_PIX_NUM(n)					lcd_register_ptr->lcd_sif_vdo_sync_con1_register.field.HFP_PIX_NUM = n;
#define GET_VDO_MODE_HFP_PIX_NUM(n)					(n) = lcd_register_ptr->lcd_sif_vdo_sync_con1_register.field.HFP_PIX_NUM;
#define SET_VDO_MODE_FR_DUR(n)						lcd_register_ptr->lcd_fr_duration_register.value = n;
#define GET_VDO_MODE_FR_DUR(n)						(n) = lcd_register_ptr->lcd_fr_duration_register.value;
#define SET_VDO_MODE_FR_STR_HDR(n)					lcd_register_ptr->lcd_sif_vdo_header_register.field.VDO_FR_STR_HDR = n;
#define GET_VDO_MODE_FR_STR_HDR(n)					(n) = lcd_register_ptr->lcd_sif_vdo_header_register.field.VDO_FR_STR_HDR;
#define SET_VDO_MODE_LN_STR_HDR(n)					lcd_register_ptr->lcd_sif_vdo_header_register.field.VDO_LN_STR_HDR = n;
#define GET_VDO_MODE_LN_STR_HDR(n)					(n) = lcd_register_ptr->lcd_sif_vdo_header_register.field.VDO_LN_STR_HDR;

// LCD SIF PAD SEL
#define SET_LCD_SIF_LSDI_SEL(n)						lcd_register_ptr->lcd_serial_pad_sel_register.field.LSDI_SEL = n;
#define GET_LCD_SIF_LSDI_SEL(n)						(n) = lcd_register_ptr->lcd_serial_pad_sel_register.field.LSDI_SEL;
#define SET_LCD_SIF_LSDA_SEL(n)						lcd_register_ptr->lcd_serial_pad_sel_register.field.LSDA_SEL = n;
#define GET_LCD_SIF_LSDA_SEL(n)						(n) = lcd_register_ptr->lcd_serial_pad_sel_register.field.LSDA_SEL;

// For LCD interface
#define LCD_SERIAL0_A0_LOW_ADDR						(LCD_BASE + 0xF80)
#define LCD_SERIAL0_A0_HIGH_ADDR						(LCD_BASE + 0xF90)
#define LCD_SERIAL1_A0_LOW_ADDR						(LCD_BASE + 0xFA0)
#define LCD_SERIAL1_A0_HIGH_ADDR						(LCD_BASE + 0xFB0)


typedef enum {
    DISPLAY_LCD_PRINT_IF_TIME_PARA = 0,
    DISPLAY_LCD_PRINT_TE_TIME_PARA,
} display_lcd_debug_info_e;

typedef enum {
    HAL_DISPLAY_LCD_GMCCON_BURST_4_BYTE = 0,
    HAL_DISPLAY_LCD_GMCCON_BURST_16_BYTE = 2,
    HAL_DISPLAY_LCD_GMCCON_BURST_32_BYTE = 3,
    HAL_DISPLAY_LCD_GMCCON_BURST_64_BYTE = 4
} hal_display_lcd_gmccon_burst_e;

typedef struct {
    hal_display_lcd_interface_port_t port_number;
    uint32_t three_wire_mode;
    uint32_t two_data_lane_mode;
    uint32_t single_a0_mode;
    uint32_t cs_stay_low_mode;
    uint32_t start_byte_mode;
    uint32_t each_trans_length;
    uint32_t two_data_lane_each_trans_length;
    uint32_t str_byte_each_trans_length;
    float		ClkWidth;
    float lcdPixelTransitionTime;
    float lcmScanTimePerLine;
    float TransferCntPerPixel;
    uint32_t transactionCycle;
} hal_display_lcd_te_timing_para_t;

hal_display_lcd_status_t display_lcd_init(uint32_t main_command_address, uint32_t main_data_addressess, uint32_t main_lcd_output_format);
hal_display_lcd_status_t display_lcd_set_interface_timing(hal_display_lcd_interface_timing_t para);
hal_display_lcd_status_t display_lcd_set_interface_mode(hal_display_lcd_interface_mode_t para);
hal_display_lcd_status_t display_lcd_set_interface_2data_lane_mode(hal_display_lcd_interface_port_t port, bool flag, hal_display_lcd_interface_2pin_width_t two_data_width);
hal_display_lcd_status_t display_lcd_restore_if_settings(void);
hal_display_lcd_status_t display_lcd_config_dither(void);
uint32_t display_lcd_GetFormatBpp(hal_display_lcd_layer_source_color_format_t layer_format);
hal_display_lcd_status_t display_lcd_config_layer(hal_display_lcd_layer_input_t *layer_data);
hal_display_lcd_status_t display_lcd_config_roi(hal_display_lcd_roi_output_t *roi_para);
hal_display_lcd_status_t display_lcd_config_start_byte(hal_display_lcd_interface_start_byte_mode_t *start_byte_para);
hal_display_lcd_status_t display_lcd_set_index_color_table(uint32_t *index_table);
hal_display_lcd_status_t display_lcd_init_te(uint32_t frame_rate, uint32_t back_porch, uint32_t front_porch, uint32_t width, uint32_t height, uint32_t main_lcd_output);
hal_display_lcd_status_t display_lcd_calculate_te(uint32_t lcm_width, uint32_t lcm_height);
uint32_t display_lcd_get_roi_width(void);
hal_display_lcd_status_t display_lcd_dbgInfoPrint(display_lcd_debug_info_e dbg_index);
hal_display_lcd_status_t display_lcd_toggle_reset(uint32_t low_time, uint32_t high_time);
hal_display_lcd_status_t display_lcd_start_dma(bool wait_te);
uint32_t display_lcd_get_layer_address(hal_display_lcd_layer_t layer);
hal_display_lcd_status_t display_lcd_init_para(void);
hal_display_lcd_status_t display_lcd_backup_register(void);
hal_display_lcd_status_t display_lcd_restore_layer_settings(void);
hal_display_lcd_status_t display_lcd_power_on(void);
hal_display_lcd_status_t display_lcd_power_off(void);
hal_display_lcd_status_t display_lcd_apply_setting(void);
hal_display_lcd_status_t display_lcd_set_layer_to_default(void);

#ifdef __cplusplus
}
#endif

#endif

#endif //__HAL_DISPLAY_LCD_INTERNAL_H__


