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

#ifndef __HAL_DISP_AAL_INTERNAL_H__
#define __HAL_DISP_AAL_INTERNAL_H__

#define DISP_AAL_base 0xA0490000

typedef union {
    uint32_t	value;
    struct {
        uint32_t AAL_EN 						: 1; 	/*! Bit 0 */
        uint32_t reserved							: 31;		/*! reserved */
    } field;
} DISP_AAL_EN_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t AAL_RESET						: 1; 	/*! Bit 0 */
        uint32_t reserved							: 31;		/*! reserved */
    } field;
} DISP_AAL_RESET_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t IF_END_INT_EN						: 1; 	/*! Bit 0 */
        uint32_t OF_END_INT_EN					: 1; 	/*! Bit 1 */
        uint32_t reserved							: 30;		/*! reserved */
    } field;
} DISP_AAL_INTEN_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t IF_END_INT 					: 1; 	/*! Bit 0 */
        uint32_t OF_END_INT 					: 1; 	/*! Bit 1 */
        uint32_t reserved							: 30;		/*! reserved */
    } field;
} DISP_AAL_INTSTA_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t IF_UNFINISH						: 1; 	/*! Bit 0 */
        uint32_t OF_UNFINISH						: 1; 	/*! Bit 1 */
        uint32_t reserved1							: 2; 	/*! reserved */
        uint32_t OF_END_INT 					: 24;		/*! Bit 4..27 */
        uint32_t reserved							: 4;		/*! reserved */
    } field;
} DISP_AAL_STATUS_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t RELAY_MODE 					: 1; 	/*! Bit 0 */
        uint32_t AAL_ENGINE_EN					: 1; 	/*! Bit 1 */
        uint32_t AAL_HIST_EN						: 1; 	/*! Bit 2 */
        uint32_t AAL_HIST_LOCK					: 1; 	/*! Bit 3 */
        uint32_t AAL_CG_DISABLE 				: 1; 	/*! Bit 4 */
        uint32_t reserved1							: 23;		/*! reserved */
        uint32_t CHKSUM_EN						: 1; 	/*! Bit 28 */
        uint32_t CHKSUM_SEL 					: 2; 	/*! Bit 29..30 */
        uint32_t reserved							: 1;		/*! reserved */
    } field;
} DISP_AAL_CFG_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t INP_PIX_CNT						: 13;		/*! Bit 0..12 */
        uint32_t reserved1							: 3; 	/*! reserved */
        uint32_t INP_LINE_CNT						: 13;		/*! Bit 16..28 */
        uint32_t reserved							: 3;		/*! reserved */
    } field;
} DISP_AAL_INPUT_COUNT_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t OUTP_PIX_CNT						: 13;		/*! Bit 0..12 */
        uint32_t reserved1							: 3; 	/*! reserved */
        uint32_t OUTP_LINE_CNT					: 13;		/*! Bit 16..28 */
        uint32_t reserved							: 3;		/*! reserved */
    } field;
} DISP_AAL_OUTPUT_COUNT_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t OF_END_INT_EN					: 27;		/*! Bit 0..26 */
        uint32_t reserved							: 5;		/*! reserved */
    } field;
} DISP_AAL_CHKSUM_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t VSIZE							: 13;		/*! Bit 0..12 */
        uint32_t reserved1							: 3; 	/*! reserved */
        uint32_t HSIZE							: 13;		/*! Bit 16..28 */
        uint32_t reserved							: 3;		/*! reserved */
    } field;
} DISP_AAL_SIZE_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t AAL_ATPG_OB						: 1; 	/*! Bit 0 */
        uint32_t AAL_ATPG_CT						: 1; 	/*! Bit 1 */
        uint32_t reserved							: 30;		/*! reserved */
    } field;
} DISP_AAL_ATPG_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t reserved1							: 16;		/*! reserved */
        uint32_t MAXHIST_RGB_WEIGHT 			: 4; 	/*! Bit 16..19 */
        uint32_t reserved							: 12;		/*! reserved */
    } field;
} DISP_AAL_MAX_HIST_CONFIG_00_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t reserved							: 31;		/*! reserved */
        uint32_t CABC_EN							: 1;		/*! Bit 31 */
    } field;
} DISP_AAL_CABC_00_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CABC_EN							: 10;		/*! Bit 0..9 */
        uint32_t reserved							: 22;		/*! reserved */
    } field;
} DISP_AAL_CABC_02_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CABC_OUT_R_FORCE				: 8; 	/*! Bit 0..7 */
        uint32_t reserved2							: 1; 	/*! reserved */
        uint32_t CABC_OUT_G_FORCE				: 8; 	/*! Bit 9..16 */
        uint32_t reserved1							: 1; 	/*! reserved */
        uint32_t CABC_OUT_B_FORCE				: 8; 	/*! Bit 18..25 */
        uint32_t reserved							: 5; 	/*! reserved */
        uint32_t CABC_INK							: 1;		/*! Bit 31 */
    } field;
} DISP_AAL_CABC_04_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DISP_AAL_STATUS					: 22;		/*! Bit 0..21 */
        uint32_t reserved							: 10;		/*! reserved */
    } field;
} DISP_AAL_HIST_STATUS_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DRE_GAIN_FLT_0_FORCE				: 12;		/*! Bit 0..11 */
        uint32_t DRE_GAIN_FLT_1_FORCE				: 12;		/*! Bit 12..23 */
        uint32_t reserved							: 8;		/*! reserved */
    } field;
} DISP_AAL_DRE_FLT_FORCE_00_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DRE_GAIN_FLT_2_FORCE				: 12;		/*! Bit 0..11 */
        uint32_t DRE_GAIN_FLT_3_FORCE				: 11;		/*! Bit 12..22 */
        uint32_t reserved							: 9;		/*! reserved */
    } field;
} DISP_AAL_DRE_FLT_FORCE_01_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DRE_GAIN_FLT_4_FORCE				: 11;		/*! Bit 0..10 */
        uint32_t DRE_GAIN_FLT_5_FORCE				: 11;		/*! Bit 11..21 */
        uint32_t reserved							: 10;		/*! reserved */
    } field;
} DISP_AAL_DRE_FLT_FORCE_02_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DRE_GAIN_FLT_6_FORCE				: 11;		/*! Bit 0..10 */
        uint32_t DRE_GAIN_FLT_7_FORCE				: 10;		/*! Bit 11..20 */
        uint32_t DRE_GAIN_FLT_8_FORCE				: 10;		/*! Bit 21..30 */
        uint32_t reserved							: 1;		/*! reserved */
    } field;
} DISP_AAL_DRE_FLT_FORCE_03_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DRE_GAIN_FLT_9_FORCE				: 10;		/*! Bit 0..9 */
        uint32_t DRE_GAIN_FLT_10_FORCE			: 10;		/*! Bit 10..19 */
        uint32_t DRE_GAIN_FLT_11_FORCE			: 10;		/*! Bit 20..29 */
        uint32_t reserved							: 2;		/*! reserved */
    } field;
} DISP_AAL_DRE_FLT_FORCE_04_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DRE_GAIN_FLT_12_FORCE			: 10;		/*! Bit 0..9 */
        uint32_t DRE_GAIN_FLT_13_FORCE			: 10;		/*! Bit 10..19 */
        uint32_t DRE_GAIN_FLT_14_FORCE			: 10;		/*! Bit 20..29 */
        uint32_t reserved							: 2;		/*! reserved */
    } field;
} DISP_AAL_DRE_FLT_FORCE_05_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DRE_GAIN_FLT_15_FORCE			: 10;		/*! Bit 0..9 */
        uint32_t DRE_GAIN_FLT_16_FORCE			: 10;		/*! Bit 10..19 */
        uint32_t DRE_GAIN_FLT_17_FORCE			: 10;		/*! Bit 20..29 */
        uint32_t reserved							: 2;		/*! reserved */
    } field;
} DISP_AAL_DRE_FLT_FORCE_06_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DRE_GAIN_FLT_18_FORCE			: 9; 	/*! Bit 0..8 */
        uint32_t DRE_GAIN_FLT_19_FORCE			: 9; 	/*! Bit 9..17 */
        uint32_t DRE_GAIN_FLT_20_FORCE			: 9; 	/*! Bit 18..26 */
        uint32_t reserved							: 5;		/*! reserved */
    } field;
} DISP_AAL_DRE_FLT_FORCE_07_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DRE_GAIN_FLT_21_FORCE			: 9; 	/*! Bit 0..8 */
        uint32_t DRE_GAIN_FLT_22_FORCE			: 9; 	/*! Bit 9..17 */
        uint32_t DRE_GAIN_FLT_23_FORCE			: 9; 	/*! Bit 18..26 */
        uint32_t reserved							: 5;		/*! reserved */
    } field;
} DISP_AAL_DRE_FLT_FORCE_08_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DRE_GAIN_FLT_24_FORCE			: 9; 	/*! Bit 0..8 */
        uint32_t DRE_GAIN_FLT_25_FORCE			: 9; 	/*! Bit 9..17 */
        uint32_t DRE_GAIN_FLT_26_FORCE			: 9; 	/*! Bit 18..26 */
        uint32_t reserved							: 5;		/*! reserved */
    } field;
} DISP_AAL_DRE_FLT_FORCE_09_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DRE_GAIN_FLT_27_FORCE			: 9; 	/*! Bit 0..8 */
        uint32_t DRE_GAIN_FLT_28_FORCE			: 9; 	/*! Bit 9..17 */
        uint32_t reserved							: 14;		/*! reserved */
    } field;
} DISP_AAL_DRE_FLT_FORCE_10_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DRE_MAP_RGB_WEIGHT 			: 4; 	/*! Bit 0..3 */
        uint32_t DRE_MAP_BYPASS 				: 1; 	/*! Bit 4 */
        uint32_t reserved							: 27;		/*! reserved */
    } field;
} DISP_AAL_DRE_MAPPING_00_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DISP_AAL_CABC_GAINLMT_TBL_00		: 10;		/*! Bit 0..9 */
        uint32_t DISP_AAL_CABC_GAINLMT_TBL_01		: 10;		/*! Bit 10..19 */
        uint32_t DISP_AAL_CABC_GAINLMT_TBL_02		: 10;		/*! Bit 20..29 */
        uint32_t reserved							: 2;		/*! reserved */
    } field;
} DISP_AAL_CABC_GAINLMT_TBL_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t SPLIT_EN							: 1; 	/*! Bit 0 */
        uint32_t SPLIT_SWAP 					: 1; 	/*! Bit 1 */
        uint32_t reserved							: 30;		/*! reserved */
    } field;
} DISP_AAL_DBG_CFG_MAIN_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t WIN_X_START						: 13;		/*! Bit 0..12 */
        uint32_t reserved1							: 3; 	/*! reserved */
        uint32_t WIN_X_END						: 13;		/*! Bit 16..28 */
        uint32_t reserved							: 3;		/*! reserved */
    } field;
} DISP_AAL_WIN_X_MAIN_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t WIN_Y_START						: 13;		/*! Bit 0..12 */
        uint32_t reserved1							: 3; 	/*! reserved */
        uint32_t WIN_Y_END						: 13;		/*! Bit 16..28 */
        uint32_t reserved							: 3;		/*! reserved */
    } field;
} DISP_AAL_WIN_Y_MAIN_REGISTER_T;

typedef struct {
    DISP_AAL_EN_REGISTER_T					DISP_AAL_EN_REGISTER;					// 0x0000
    DISP_AAL_RESET_REGISTER_T				DISP_AAL_RESET_REGISTER;					// 0x0004
    DISP_AAL_INTEN_REGISTER_T				DISP_AAL_INTEN_REGISTER;					// 0x0008
    DISP_AAL_INTSTA_REGISTER_T				DISP_AAL_INTSTA_REGISTER;					// 0x000C
    DISP_AAL_STATUS_REGISTER_T				DISP_AAL_STATUS_REGISTER;				// 0x0010
    uint32_t									reseved[3];
    DISP_AAL_CFG_REGISTER_T					DISP_AAL_CFG_REGISTER;					// 0x0020
    DISP_AAL_INPUT_COUNT_REGISTER_T			DISP_AAL_INPUT_COUNT_REGISTER;			// 0x0024
    DISP_AAL_OUTPUT_COUNT_REGISTER_T		DISP_AAL_OUTPUT_COUNT_REGISTER;			// 0x0028
    DISP_AAL_CHKSUM_REGISTER_T				DISP_AAL_CHKSUM_REGISTER;				// 0x002C
    DISP_AAL_SIZE_REGISTER_T					DISP_AAL_SIZE_REGISTER;					// 0x0030
    uint32_t									reserved1[50];
    DISP_AAL_ATPG_REGISTER_T					DISP_AAL_ATPG_REGISTER;					// 0x00FC
    uint32_t									reserved2[65];
    DISP_AAL_MAX_HIST_CONFIG_00_REGISTER_T	DISP_AAL_MAX_HIST_CONFIG_00_REGISTER;	// 0x0204
    uint32_t									reserved3;
    DISP_AAL_CABC_00_REGISTER_T				DISP_AAL_CABC_00_REGISTER;				// 0x020C
    uint32_t									reserved4;
    DISP_AAL_CABC_02_REGISTER_T				DISP_AAL_CABC_02_REGISTER;				// 0x0214
    uint32_t									reserved5;
    DISP_AAL_CABC_04_REGISTER_T				DISP_AAL_CABC_04_REGISTER;				// 0x021C
    uint32_t									reserved6;
    DISP_AAL_HIST_STATUS_REGISTER_T			DISP_AAL_HIST_STATUS_REGISTER[33];		// 0x0224--0x02A4
    uint32_t									reserved7[44];
    DISP_AAL_DRE_FLT_FORCE_00_REGISTER_T		DISP_AAL_DRE_FLT_FORCE_00_REGISTER;		// 0x358
    DISP_AAL_DRE_FLT_FORCE_01_REGISTER_T		DISP_AAL_DRE_FLT_FORCE_01_REGISTER;		// 0x35C
    DISP_AAL_DRE_FLT_FORCE_02_REGISTER_T		DISP_AAL_DRE_FLT_FORCE_02_REGISTER;		// 0x360
    DISP_AAL_DRE_FLT_FORCE_03_REGISTER_T		DISP_AAL_DRE_FLT_FORCE_03_REGISTER;		// 0x364
    DISP_AAL_DRE_FLT_FORCE_04_REGISTER_T		DISP_AAL_DRE_FLT_FORCE_04_REGISTER;		// 0x368
    DISP_AAL_DRE_FLT_FORCE_05_REGISTER_T		DISP_AAL_DRE_FLT_FORCE_05_REGISTER;		// 0x36C
    DISP_AAL_DRE_FLT_FORCE_06_REGISTER_T		DISP_AAL_DRE_FLT_FORCE_06_REGISTER;		// 0x370
    DISP_AAL_DRE_FLT_FORCE_07_REGISTER_T		DISP_AAL_DRE_FLT_FORCE_07_REGISTER;		// 0x374
    DISP_AAL_DRE_FLT_FORCE_08_REGISTER_T		DISP_AAL_DRE_FLT_FORCE_08_REGISTER;		// 0x378
    DISP_AAL_DRE_FLT_FORCE_09_REGISTER_T		DISP_AAL_DRE_FLT_FORCE_09_REGISTER;		// 0x37C
    DISP_AAL_DRE_FLT_FORCE_10_REGISTER_T		DISP_AAL_DRE_FLT_FORCE_10_REGISTER;		// 0x380
    uint32_t									reserved8[11];
    DISP_AAL_DRE_MAPPING_00_REGISTER_T		DISP_AAL_DRE_MAPPING_00_REGISTER;		// 0x3B0
    uint32_t									reserved9[22];
    DISP_AAL_CABC_GAINLMT_TBL_REGISTER_T		DISP_AAL_CABC_GAINLMT_TBL_REGISTER[11];	// 0x40C--0x434
    uint32_t									reserved10[2];
    DISP_AAL_DBG_CFG_MAIN_REGISTER_T		DISP_AAL_DBG_CFG_MAIN_REGISTER;			// 0x440
    DISP_AAL_WIN_X_MAIN_REGISTER_T			DISP_AAL_WIN_X_MAIN_REGISTER;			// 0x444
    DISP_AAL_WIN_Y_MAIN_REGISTER_T			DISP_AAL_WIN_Y_MAIN_REGISTER;			// 0x448

} DISP_AAL_REGISTER_T;

#endif //__HAL_DISP_AAL_INTERNAL_H__


