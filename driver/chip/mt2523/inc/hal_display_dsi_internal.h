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

#ifndef __HAL_DISPLAY_DSI_INTERNAL_H__
#define __HAL_DISPLAY_DSI_INTERNAL_H__

#include "hal_platform.h"
#ifdef HAL_DISPLAY_DSI_MODULE_ENABLED

#include "hal_display_dsi.h"

typedef union {
    uint32_t	value;
    struct {
        uint32_t START							: 1; 	/*! Bit 0 */
        uint32_t reserved3						: 1; 	/*! reserved */
        uint32_t SLEEPOUT_START 				: 1; 	/*! Bit 2 */
        uint32_t reserved2						: 1; 	/*! reserved */
        uint32_t SKEWCAL_START					: 1; 	/*! Bit 4 */
        uint32_t reserved1						: 11;	/*! reserved */
        uint32_t VM_CMD_START					: 1; 	/*! Bit 16 */
        uint32_t reserved						: 15;	/*! reserved */
    } field;
} LCD_DSI_START_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t reserved2						: 1; 	/*! reserved */
        uint32_t BUFFER_UNDERRUN				: 1; 	/*! Bit 1 */
        uint32_t reserved1						: 2; 	/*! reserved */
        uint32_t ESC_ENTRY_ERR					: 1; 	/*! Bit 4 */
        uint32_t ESC_SYNC_ERR					: 1; 	/*! Bit 5 */
        uint32_t FALSE_CTRL_ERR 				: 1; 	/*! Bit 6 */
        uint32_t CONTENTION_ERR 				: 1; 	/*! Bit 7 */
        uint32_t reserved						: 24;	/*! reserved */
    } field;
} LCD_DSI_STATUS_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t LPRX_RD_RDY_INT_EN 			: 1; 	/*! Bit 0 */
        uint32_t CMD_DONE_INT_EN				: 1; 	/*! Bit 1 */
        uint32_t TE_RDY_INT_EN					: 1; 	/*! Bit 2 */
        uint32_t VM_DONE_INT_EN 				: 1; 	/*! Bit 3 */
        uint32_t FRAME_DONE_INT_EN				: 1; 	/*! Bit 4 */
        uint32_t VM_CMD_DONE_INT_EN 			: 1; 	/*! Bit 5 */
        uint32_t SLEEPOUT_DONE_INT_EN			: 1; 	/*! Bit 6 */
        uint32_t TE_TIMEOUT_INT_EN				: 1; 	/*! Bit 7 */
        uint32_t VM_VBP_STR_INT_EN				: 1; 	/*! Bit 8 */
        uint32_t VM_VACT_STR_INT_EN 			: 1; 	/*! Bit 9 */
        uint32_t VM_VFP_STR_INT_EN				: 1; 	/*! Bit 10 */
        uint32_t SKEWCAL_DONE_INT_EN			: 1; 	/*! Bit 11 */
        uint32_t reserved						: 20;	/*! reserved */
    } field;
} LCD_DSI_INTEN_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t LPRX_RD_RDY_INT_FLAG			: 1; 	/*! Bit 0 */
        uint32_t CMD_DONE_INT_FLAG				: 1; 	/*! Bit 1 */
        uint32_t TE_RDY_INT_FLAG				: 1; 	/*! Bit 2 */
        uint32_t VM_DONE_INT_FLAG				: 1; 	/*! Bit 3 */
        uint32_t FRAME_DONE_INT_FLAG			: 1; 	/*! Bit 4 */
        uint32_t VM_CMD_DONE_INT_FLAG			: 1; 	/*! Bit 5 */
        uint32_t SLEEPOUT_DONE_INT_FLAG 		: 1; 	/*! Bit 6 */
        uint32_t TE_TIMEOUT_INT_FLAG			: 1; 	/*! Bit 7 */
        uint32_t VM_VBP_STR_INT_FLAG			: 1; 	/*! Bit 8 */
        uint32_t VM_VACT_STR_INT_FLAG			: 1; 	/*! Bit 9 */
        uint32_t VM_VFP_STR_INT_FLAG			: 1; 	/*! Bit 10 */
        uint32_t SKEWCAL_DONE_INT_FLAG			: 1; 	/*! Bit 11 */
        uint32_t reserved						: 19;	/*! reserved */
        uint32_t DSI_BUSY						: 1;		/*! Bit 31 */
    } field;
} LCD_DSI_INTSTA_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DSI_RESET						: 1; 	/*! Bit 0 */
        uint32_t reserved2						: 1; 	/*! reserved */
        uint32_t DPHY_RESET 					: 1; 	/*! Bit 2 */
        uint32_t reserved1						: 1; 	/*! reserved */
        uint32_t DSI_DUAL_EN					: 1; 	/*! Bit 4 */
        uint32_t reserved						: 27;	/*! reserved */
    } field;
} LCD_DSI_COM_CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t MODE_CON						: 2; 	/*! Bit 0..1 */
        uint32_t reserved1						: 14;	/*! reserved */
        uint32_t FRAME_MODE 					: 1; 	/*! Bit 16 */
        uint32_t MIX_MODE						: 1; 	/*! Bit 17 */
        uint32_t V2C_SWITCH_ON					: 1; 	/*! Bit 18 */
        uint32_t C2V_SWITCH_ON					: 1; 	/*! Bit 19 */
        uint32_t SLEEP_MODE 					: 1; 	/*! Bit 20 */
        uint32_t reserved						: 11;	/*! reserved */
    } field;
} LCD_DSI_MODE_CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t VC_NUM 						: 2; 	/*! Bit 0..1 */
        uint32_t LANE_NUM						: 4; 	/*! Bit 2..5 */
        uint32_t HSTX_DIS_EOT					: 1; 	/*! Bit 6 */
        uint32_t HSTX_BLLP_EN					: 1; 	/*! Bit 7 */
        uint32_t TE_FREERUN 					: 1; 	/*! Bit 8 */
        uint32_t EXT_TE_EN						: 1; 	/*! Bit 9 */
        uint32_t EXT_TE_EDGE_SEL				: 1; 	/*! Bit 10 */
        uint32_t TE_AUTO_SYNC					: 1; 	/*! Bit 11 */
        uint32_t MAX_RTN_SIZE					: 4; 	/*! Bit 12..15 */
        uint32_t HSTX_CKLP_EN					: 1; 	/*! Bit 17 */
        uint32_t TYPE1_BTA_SEL					: 1; 	/*! Bit 17 */
        uint32_t TE_WITH_CMD_EN 				: 1; 	/*! Bit 18 */
        uint32_t TE_TIMEOUT_CHK_EN				: 1; 	/*! Bit 19 */
        uint32_t EXT_TE_TIME_VM 				: 4; 	/*! Bit 20..23 */
        uint32_t reserved						: 8; 	/*! reserved */
    } field;
} LCD_DSI_TXRX_CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DSI_PS_WC						: 14;	/*! Bit 0..13 */
        uint32_t reserved2						: 2; 	/*! reserved */
        uint32_t DSI_PS_SEL 					: 2; 	/*! Bit 16..17 */
        uint32_t reserved1						: 6; 	/*! reserved */
        uint32_t RGB_SWAP						: 1; 	/*! Bit 24 */
        uint32_t BYTE_SWAP						: 1; 	/*! Bit 25 */
        uint32_t reserved						: 6;		/*! reserved */
    } field;
} LCD_DSI_PSCON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t VSA_NL 						: 12;	/*! Bit 0..11 */
        uint32_t reserved						: 20;	/*! reserved */
    } field;
} LCD_DSI_VSA_NL_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t VBP_NL 						: 12;	/*! Bit 0..11 */
        uint32_t reserved						: 20;	/*! reserved */
    } field;
} LCD_DSI_VBP_NL_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t VFP_NL 						: 12;	/*! Bit 0..11 */
        uint32_t reserved						: 20;	/*! reserved */
    } field;
} LCD_DSI_VFP_NL_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t VACT_NL						: 12;	/*! Bit 0..11 */
        uint32_t reserved						: 20;	/*! reserved */
    } field;
} LCD_DSI_VACT_NL_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DSI_LFR_MODE					: 2; 	/*! Bit 0..1 */
        uint32_t DSI_LFR_TYPE					: 2; 	/*! Bit 2..3 */
        uint32_t DSI_LFR_EN 					: 1; 	/*! Bit 4 */
        uint32_t DSI_LFR_UPDATE 				: 1; 	/*! Bit 5 */
        uint32_t DSI_LFR_VSE_DIS				: 1; 	/*! Bit 6 */
        uint32_t reserved1						: 1; 	/*! reserved */
        uint32_t DSI_LFR_SKIP_NUM				: 6; 	/*! Bit 8..13 */
        uint32_t reserved						: 18;	/*! reserved */
    } field;
} LCD_DSI_LFR_CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DSI_LFR_SKIP_CNT				: 6; 	/*! Bit 0..5 */
        uint32_t reserved1						: 2; 	/*! reserved */
        uint32_t DSI_LFR_SKIP_STA				: 1; 	/*! Bit 8 */
        uint32_t reserved						: 23;	/*! reserved */
    } field;
} LCD_DSI_LFR_STA_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DSI_HSA_WC 					: 12;	/*! Bit 0..11 */
        uint32_t reserved						: 20;	/*! reserved */
    } field;
} LCD_DSI_HSA_WC_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DSI_HBP_WC 					: 12;	/*! Bit 0..11 */
        uint32_t reserved						: 20;	/*! reserved */
    } field;
} LCD_DSI_HBP_WC_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DSI_HFP_WC 					: 12;	/*! Bit 0..11 */
        uint32_t reserved						: 20;	/*! reserved */
    } field;
} LCD_DSI_HFP_WC_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DSI_BLLP_WC					: 12;	/*! Bit 0..11 */
        uint32_t reserved						: 20;	/*! reserved */
    } field;
} LCD_DSI_BLLP_WC_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CMDQ_SIZE						: 8; 	/*! Bit 0..7 */
        uint32_t reserved						: 24;	/*! reserved */
    } field;
} LCD_DSI_CMDQ_CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t reserved1						: 2; 	/*! reserved */
        uint32_t HSTX_CKLP_WC					: 14;	/*! Bit 2..15 */
        uint32_t HSTX_CKLP_WC_AUTO				: 1; 	/*! Bit 16 */
        uint32_t reserved						: 15;	/*! reserved */
    } field;
} LCD_DSI_HSTX_CKLP_WC_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DSI_HSTX_CKLP_WC_AUTO_RESULT	: 16;	/*! Bit 0..15 */
        uint32_t reserved						: 16;	/*! reserved */
    } field;
} LCD_DSI_HSTX_CKLP_WC_AUTO_RESULT_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t BYTE0							: 8; 	/*! Bit 0..7 */
        uint32_t BYTE1							: 8; 	/*! Bit 8..15 */
        uint32_t BYTE2							: 8; 	/*! Bit 16..23 */
        uint32_t BYTE3							: 8;		/*! Bit 24..31 */
    } field;
} LCD_DSI_RX_DATA03_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t BYTE4							: 8; 	/*! Bit 0..7 */
        uint32_t BYTE5							: 8; 	/*! Bit 8..15 */
        uint32_t BYTE6							: 8; 	/*! Bit 16..23 */
        uint32_t BYTE7							: 8;		/*! Bit 24..31 */
    } field;
} LCD_DSI_RX_DATA47_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t BYTE8							: 8; 	/*! Bit 0..7 */
        uint32_t BYTE9							: 8; 	/*! Bit 8..15 */
        uint32_t BYTEA							: 8; 	/*! Bit 16..23 */
        uint32_t BYTEB							: 8;		/*! Bit 24..31 */
    } field;
} LCD_DSI_RX_DATA8B_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t BYTEC							: 8; 	/*! Bit 0..7 */
        uint32_t BYTED							: 8; 	/*! Bit 8..15 */
        uint32_t BYTEE							: 8; 	/*! Bit 16..23 */
        uint32_t BYTEF							: 8;		/*! Bit 24..31 */
    } field;
} LCD_DSI_RX_DATAC_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t RACK							: 1; 	/*! Bit 0 */
        uint32_t RACK_BYPASS					: 1; 	/*! Bit 1 */
        uint32_t reserved						: 30;	/*! reserved */
    } field;
} LCD_DSI_RX_RACK_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t RX_TRIG_0						: 1; 	/*! Bit 0 */
        uint32_t RX_TRIG_1						: 1; 	/*! Bit 1 */
        uint32_t RX_TRIG_2						: 1; 	/*! Bit 2 */
        uint32_t RX_TRIG_3						: 1; 	/*! Bit 3 */
        uint32_t RX_ULPS						: 1; 	/*! Bit 4 */
        uint32_t DIRECTION						: 1; 	/*! Bit 5 */
        uint32_t RX_LPDT						: 1; 	/*! Bit 6 */
        uint32_t reserved1						: 1; 	/*! reserved */
        uint32_t RX_POINTER 					: 4; 	/*! Bit 8..11 */
        uint32_t reserved						: 20;	/*! reserved */
    } field;
}  LCD_DSI_RX_TRIG_STA_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DSI_RWMEM_CONTI				: 16;	/*! Bit 0..15 */
        uint32_t reserved						: 16;	/*! reserved */
    } field;
} LCD_DSI_MEM_CONTI_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DSI_FRM_BC 					: 20;	/*! Bit 0..20 */
        uint32_t reserved						: 11;	/*! reserved */
    } field;
} LCD_DSI_FRM_BC_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t V3D_MODE						: 2; 	/*! Bit 0..1 */
        uint32_t V3D_FMT						: 2; 	/*! Bit 2..3 */
        uint32_t V3D_VSYNC						: 1; 	/*! Bit 4 */
        uint32_t V3D_LR 						: 1; 	/*! Bit 5 */
        uint32_t reserved1						: 2; 	/*! reserved */
        uint32_t V3D_EN 						: 1; 	/*! Bit 8 */
        uint32_t reserved						: 23;	/*! reserved */
    } field;
} LCD_DSI_V3D_CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t ULPS_WAKEUP_PRD				: 16;	/*! Bit 0..15 */
        uint32_t SKEWCAL_PRD					: 16;	/*! Bit 16..31 */
    } field;
} LCD_DSI_TIME_CON0_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t TE_TIMEOUT_PRD 				: 16;	/*! Bit 0..15 */
        uint32_t PREFETCH_TIME					: 15;	/*! Bit 16..30 */
        uint32_t PREFETCH_EN					: 1;		/*! Bit 31 */
    } field;
} LCD_DSI_TIME_CON1_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t LC_HSTX_CK_PAT 				: 8; /*! Bit 0..20 */
        uint32_t reserved						: 24;	/*! reserved */
    } field;
} LCD_DSI_PHY_LCPAT_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t LC_HSTX_EN 					: 1; 	/*! Bit 0 */
        uint32_t LC_ULPM_EN 					: 1; 	/*! Bit 1 */
        uint32_t LC_WAKEUP_EN					: 1; 	/*! Bit 2 */
        uint32_t reserved						: 29;	/*! reserved */
    } field;
} LCD_DSI_PHY_LCCON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t L0_RM_TRIG_EN					: 1; 	/*! Bit 0 */
        uint32_t L0_ULPM_EN 					: 1; 	/*! Bit 1 */
        uint32_t L0_WAKEUP_EN					: 1; 	/*! Bit 2 */
        uint32_t LX_ULPM_AS_L0					: 1; 	/*! Bit 3 */
        uint32_t reserved						: 27;	/*! reserved */
    } field;
} LCD_DSI_PHY_LD0CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t HS_SYNC_CODE					: 8; 	/*! Bit 0..7 */
        uint32_t HS_SYNC_CODE2					: 8; 	/*! Bit 8..15 */
        uint32_t HS_SKEWCAL_PAT 				: 8; 	/*! Bit 16..23 */
        uint32_t HS_DB_SYNC_EN					: 1; 	/*! Bit 24 */
        uint32_t reserved						: 7;		/*! reserved */
    } field;
} LCD_DSI_PHY_SYNCON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t LPX							: 8; 	/*! Bit 0..7 */
        uint32_t DA_HS_PREP 					: 8; 	/*! Bit 8..15 */
        uint32_t DA_HS_ZERO 					: 8; 	/*! Bit 16..23 */
        uint32_t DA_HS_TRAIL					: 8;		/*! Bit 24..31 */
    } field;
} LCD_DSI_PHY_TIMCON0_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t TA_GO							: 8; 	/*! Bit 0..7 */
        uint32_t TA_SURE						: 8; 	/*! Bit 8..15 */
        uint32_t TA_GET 						: 8; 	/*! Bit 16..23 */
        uint32_t DA_HS_EXIT						: 8;		/*! Bit 24..31 */
    } field;
} LCD_DSI_PHY_TIMCON1_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CONT_DET						: 8; 	/*! Bit 0..7 */
        uint32_t DA_HS_SYNC 					: 8; 	/*! Bit 8..15 */
        uint32_t CLK_HS_ZERO					: 8; 	/*! Bit 16..23 */
        uint32_t CLK_HS_TRAIL					: 8;		/*! Bit 24..31 */
    } field;
} LCD_DSI_PHY_TIMCON2_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CLK_HS_PREP					: 8; 	/*! Bit 0..7 */
        uint32_t CLK_HS_POST					: 8; 	/*! Bit 8..15 */
        uint32_t CLK_HS_EXIT					: 8; 	/*! Bit 16..23 */
        uint32_t reseved						: 8;		/*! Bit 24..31 */
    } field;
} LCD_DSI_PHY_TIMCON3_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t VM_CMD_EN						: 1; 	/*! Bit 0 */
        uint32_t LONG_PKT						: 1; 	/*! Bit 1 */
        uint32_t TIME_SEL						: 1; 	/*! Bit 2 */
        uint32_t TS_VSA_EN						: 1; 	/*! Bit 3 */
        uint32_t TS_VBP_EN						: 1; 	/*! Bit 4 */
        uint32_t TS_VFP_EN						: 1; 	/*! Bit 5 */
        uint32_t reserved						: 2; 	/*! reserved */
        uint32_t CM_DATA_ID 					: 8; 	/*! Bit 8..15 */
        uint32_t CM_DATA_0						: 8; 	/*! Bit 16..23 */
        uint32_t CM_DATA_1						: 8;		/*! Bit 24..31 */
    } field;
} LCD_DSI_VM_CMD_CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t byte0							: 8;	/*! Bit 0..7 */
        uint32_t byte1							: 8;	/*! Bit 8..15 */
        uint32_t byte2							: 8;	/*! Bit 16..23 */
        uint32_t byte3							: 8;	/*! Bit 24..31 */
    } field;
} LCD_DSI_VM_CMD_DATA0_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t byte0							: 8;	/*! Bit 0..7 */
        uint32_t byte1							: 8;	/*! Bit 8..15 */
        uint32_t byte2							: 8;	/*! Bit 16..23 */
        uint32_t byte3							: 8;	/*! Bit 24..31 */
    } field;
} LCD_DSI_VM_CMD_DATA4_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t byte0							: 8;	/*! Bit 0..7 */
        uint32_t byte1							: 8;	/*! Bit 8..15 */
        uint32_t byte2							: 8;	/*! Bit 16..23 */
        uint32_t byte3							: 8;	/*! Bit 24..31 */
    } field;
} LCD_DSI_VM_CMD_DATA8_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t byte0							: 8;	/*! Bit 0..7 */
        uint32_t byte1							: 8;	/*! Bit 8..15 */
        uint32_t byte2							: 8;	/*! Bit 16..23 */
        uint32_t byte3							: 8;	/*! Bit 24..31 */
    } field;
} LCD_DSI_VM_CMD_DATAC_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t PKT_CHKSUM 					: 16;	/*! Bit 0..15 */
        uint32_t ACC_CHKSUM						: 16;	/*! Bit 16..31 */
    } field;
} LCD_DSI_CKSM_OUT_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DPHY_CTL_STATE_C				: 9; 	/*! Bit 0..8 */
        uint32_t reserved1						: 7; 	/*! reserved */
        uint32_t DPHY_HS_TX_STATE_C 			: 5; 	/*! Bit 16..20 */
        uint32_t reserved						: 11;	/*! reserved */
    } field;
} LCD_DSI_STATE_DBG0_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DPHY_CTL_STATE_0				: 15;	/*! Bit 0..14 */
        uint32_t reserved1						: 1; 	/*! reserved */
        uint32_t DPHY_HS_TX_STATE_0 			: 5; 	/*! Bit 16..20 */
        uint32_t reserved						: 3; 	/*! reserved */
        uint32_t DPHY_ESC_STATE_0				: 8;		/*! Bit 24..31 */
    } field;
} LCD_DSI_STATE_DBG1_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DPHY_RX_ESC_STATE				: 10;	/*! Bit 0..9 */
        uint32_t reserved2						: 6; 	/*! reserved */
        uint32_t DPHY_TA_T2R_STATE				: 5; 	/*! Bit 16..20 */
        uint32_t DPHY_TA_R2T_STATE				: 5; 	/*! Bit 24..28 */
        uint32_t reserved						: 3;		/*! reserved */
    } field;
} LCD_DSI_STATE_DBG2_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DPHY_CTL_STATE_1				: 5; 	/*! Bit 0..4 */
        uint32_t reserved3						: 3; 	/*! reserved */
        uint32_t DPHY_HS_TX_STATE_1 			: 5; 	/*! Bit 8..12 */
        uint32_t reserved2						: 3; 	/*! reserved */
        uint32_t DPHY_CTL_STATE_2				: 5; 	/*! Bit 16..20 */
        uint32_t reserved1						: 3; 	/*! reserved */
        uint32_t DPHY_HS_TX_STATE_2 			: 5; 	/*! Bit 24..28 */
        uint32_t reserved						: 3;		/*! reserved */
    } field;
} LCD_DSI_STATE_DBG3_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DPHY_CTL_STATE_3				: 5; 	/*! Bit 0..4 */
        uint32_t reserved3						: 3; 	/*! reserved */
        uint32_t DPHY_HS_TX_STATE_3 			: 5; 	/*! Bit 8..12 */
        uint32_t reserved						: 19;	/*! reserved */
    } field;
} LCD_DSI_STATE_DBG4_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t TIMER_COUNTER					: 16;	/*! Bit 0..15 */
        uint32_t TIMER_BUSY 					: 1; 	/*! Bit 16 */
        uint32_t reserved						: 11;	/*! reserved */
        uint32_t WAKEUP_STATE					: 4;		/*! Bit 28..31 */
    } field;
} LCD_DSI_STATE_DBG5_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t CMCTL_STATE					: 15;	/*! Bit 0..14 */
        uint32_t reserved1						: 1; 	/*! reserved */
        uint32_t CMDQ_STATE 					: 7; 	/*! Bit 16..22 */
        uint32_t reserved						: 9;		/*! reserved */
    } field;
} LCD_DSI_STATE_DBG6_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t VMCTL_STATE					: 11;	/*! Bit 0..10 */
        uint32_t reserved1						: 1; 	/*! reserved */
        uint32_t VFP_PERIOD 					: 1; 	/*! Bit 12 */
        uint32_t VACT_PERIOD					: 1; 	/*! Bit 131 */
        uint32_t VBP_PERIOD 					: 1; 	/*! Bit 14 */
        uint32_t VSA_PERIOD 					: 1; 	/*! Bit 15 */
        uint32_t reserved						: 16;	/*! reserved */
    } field;
} LCD_DSI_STATE_DBG7_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t WORD_COUNTER				    : 14;	/*! Bit 0..13 */
        uint32_t reserved						: 18;	/*! reserved */
    } field;
} LCD_DSI_STATE_DBG8_REGISTER_T;


typedef union {
    uint32_t	value;
    struct {
        uint32_t LINE_COUNTER					: 22;	/*! Bit 0..21 */
        uint32_t reserved						: 10;	/*! reserved */
    } field;
} LCD_DSI_STATE_DBG9_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t DEBUG_OUT_SEL					: 5; 	/*! Bit 0..4 */
        uint32_t reserved1						: 3; 	/*! reserved */
        uint32_t CHKSUM_REC_EN					: 1; 	/*! Bit 8 */
        uint32_t reserved						: 23;	/*! reserved */
    } field;
} LCD_DSI_DEBUG_SEL_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t BIST_PATTERN					: 32;	/*! Bit 0..31 */
    } field;
} LCD_DSI_BIST_PATTERN_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t BIST_MODE						: 1; 	/*! Bit 0 */
        uint32_t BIST_ENABLE					: 1; 	/*! Bit 1 */
        uint32_t BIST_FIX_PATTERN				: 1; 	/*! Bit 2 */
        uint32_t BIST_SPECIFIED_PATTERN 		: 1; 	/*! Bit 3 */
        uint32_t BIST_HS_FREE					: 1; 	/*! Bit 4 */
        uint32_t reserved3						: 1; 	/*! reserved */
        uint32_t SELF_PAT_MODE					: 1; 	/*! Bit 6 */
        uint32_t reserved2						: 1; 	/*! reserved */
        uint32_t BIST_LANE_NUM					: 4; 	/*! Bit 8..11 */
        uint32_t reserved1						: 4; 	/*! reserved */
        uint32_t BIST_TIMIING					: 8; 	/*! Bit 16..23 */
        uint32_t reserved						: 8;		/*! reserved */
    } field;
} LCD_DSI_BIST_CON_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t WORD							: 32;	/*! Bit 0..31 */
    } field;
} LCD_DSI_VM_CMD_DATA10_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t WORD							: 32;	/*! Bit 0..31 */
    } field;
} LCD_DSI_VM_CMD_DATA14_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t WORD							: 32;	/*! Bit 0..31 */
    } field;
} LCD_DSI_VM_CMD_DATA18_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t WORD							: 32;	/*! Bit 0..31 */
    } field;
} LCD_DSI_VM_CMD_DATA1C_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t TYPE							: 1; 	/*! Bit 0..1 */
        uint32_t BTA							: 1; 	/*! Bit 2 */
        uint32_t HS 							: 1; 	/*! Bit 3 */
        uint32_t CL 							: 1; 	/*! Bit 4 */
        uint32_t TE 							: 1; 	/*! Bit 5 */
        uint32_t RESV							: 2; 	/*! Bit 6..7 */
        uint32_t DATA_ID						: 8; 	/*! Bit 8..15 */
        uint32_t DATA_0 						: 8; 	/*! Bit 16..23 */
        uint32_t DATA_1							: 8;		/*! Bit 24..31 */
    } field;
} LCD_DSI_CMDQ_REGISTER_T;

typedef struct {
    volatile LCD_DSI_START_REGISTER_T           LCD_DSI_START_REGISTER;         // 0x00
    volatile LCD_DSI_STATUS_REGISTER_T          LCD_DSI_STATUS_REGISTER;        // 0x04
    volatile LCD_DSI_INTEN_REGISTER_T           LCD_DSI_INTEN_REGISTER;         // 0x08
    volatile LCD_DSI_INTSTA_REGISTER_T          LCD_DSI_INTSTA_REGISTER;        // 0x0C
    volatile LCD_DSI_COM_CON_REGISTER_T         LCD_DSI_COM_CON_REGISTER;       // 0x10
    volatile LCD_DSI_MODE_CON_REGISTER_T        LCD_DSI_MODE_CON_REGISTER;      // 0x14
    volatile LCD_DSI_TXRX_CON_REGISTER_T        LCD_DSI_TXRX_CON_REGISTER;      // 0x18
    volatile LCD_DSI_PSCON_REGISTER_T           LCD_DSI_PSCON_REGISTER;         // 0x1C
    volatile LCD_DSI_VSA_NL_REGISTER_T          LCD_DSI_VSA_NL_REGISTER;        // 0x20
    volatile LCD_DSI_VBP_NL_REGISTER_T          LCD_DSI_VBP_NL_REGISTER;        // 0x24
    volatile LCD_DSI_VFP_NL_REGISTER_T          LCD_DSI_VFP_NL_REGISTER;        // 0x28
    volatile LCD_DSI_VACT_NL_REGISTER_T         LCD_DSI_VACT_NL_REGISTER;       // 0x2C
    volatile LCD_DSI_LFR_CON_REGISTER_T         LCD_DSI_LFR_CON_REGISTER;       // 0x30
    volatile LCD_DSI_LFR_STA_REGISTER_T         LCD_DSI_LFR_STA_REGISTER;       // 0x34
    uint32_t                                    reserved[6];
    volatile LCD_DSI_HSA_WC_REGISTER_T          LCD_DSI_HSA_WC_REGISTER;        // 0x50
    volatile LCD_DSI_HBP_WC_REGISTER_T          LCD_DSI_HBP_WC_REGISTER;        // 0x54
    volatile LCD_DSI_HFP_WC_REGISTER_T          LCD_DSI_HFP_WC_REGISTER;        // 0x58
    volatile LCD_DSI_BLLP_WC_REGISTER_T         LCD_DSI_BLLP_WC_REGISTER;       // 0x5C
    volatile LCD_DSI_CMDQ_CON_REGISTER_T        LCD_DSI_CMDQ_CON_REGISTER;      // 0x60
    volatile LCD_DSI_HSTX_CKLP_WC_REGISTER_T    LCD_DSI_HSTX_CKLP_WC_REGISTER;  // 0x64
    volatile LCD_DSI_HSTX_CKLP_WC_AUTO_RESULT_REGISTER_T	LCD_DSI_HSTX_CKLP_WC_AUTO_RESULT_REGISTER;      // 0x68
    uint32_t                                    reserved1[2];
    volatile LCD_DSI_RX_DATA03_REGISTER_T       LCD_DSI_RX_DATA03_REGISTER;     // 0x74
    volatile LCD_DSI_RX_DATA47_REGISTER_T       LCD_DSI_RX_DATA47_REGISTER;     // 0x78
    volatile LCD_DSI_RX_DATA8B_REGISTER_T       LCD_DSI_RX_DATA8B_REGISTER;     // 0x7C
    volatile LCD_DSI_RX_DATAC_REGISTER_T        LCD_DSI_RX_DATAC_REGISTER;      // 0x80
    volatile LCD_DSI_RX_RACK_REGISTER_T         LCD_DSI_RX_RACK_REGISTER;       // 0x84
    volatile LCD_DSI_RX_TRIG_STA_REGISTER_T     LCD_DSI_RX_TRIG_STA_REGISTER;   // 0x88
    uint32_t                                    reserved2;
    volatile LCD_DSI_MEM_CONTI_REGISTER_T       LCD_DSI_MEM_CONTI_REGISTER;     // 0x90
    volatile LCD_DSI_FRM_BC_REGISTER_T          LCD_DSI_FRM_BC_REGISTER;        // 0x94
    volatile LCD_DSI_V3D_CON_REGISTER_T         LCD_DSI_V3D_CON_REGISTER;       // 0x98
    uint32_t                                    reserved3;
    volatile LCD_DSI_TIME_CON0_REGISTER_T       LCD_DSI_TIME_CON0_REGISTER;     // 0xA0
    volatile LCD_DSI_TIME_CON1_REGISTER_T       LCD_DSI_TIME_CON1_REGISTER;     // 0xA4
    uint32_t                                    reserved4[22];
    volatile LCD_DSI_PHY_LCPAT_REGISTER_T       LCD_DSI_PHY_LCPAT_REGISTER;     // 0x100
    volatile LCD_DSI_PHY_LCCON_REGISTER_T       LCD_DSI_PHY_LCCON_REGISTER;     // 0x104
    volatile LCD_DSI_PHY_LD0CON_REGISTER_T      LCD_DSI_PHY_LD0CON_REGISTER;    // 0x108
    volatile LCD_DSI_PHY_SYNCON_REGISTER_T      LCD_DSI_PHY_SYNCON_REGISTER;    // 0x10C
    volatile LCD_DSI_PHY_TIMCON0_REGISTER_T     LCD_DSI_PHY_TIMCON0_REGISTER;   // 0x110
    volatile LCD_DSI_PHY_TIMCON1_REGISTER_T     LCD_DSI_PHY_TIMCON1_REGISTER;   // 0x114
    volatile LCD_DSI_PHY_TIMCON2_REGISTER_T     LCD_DSI_PHY_TIMCON2_REGISTER;   // 0x118
    volatile LCD_DSI_PHY_TIMCON3_REGISTER_T     LCD_DSI_PHY_TIMCON3_REGISTER;   // 0x11C
    uint32_t                                    reserved5[4];
    volatile LCD_DSI_VM_CMD_CON_REGISTER_T      LCD_DSI_VM_CMD_CON_REGISTER;    // 0x130
    volatile LCD_DSI_VM_CMD_DATA0_REGISTER_T    LCD_DSI_VM_CMD_DATA0_REGISTER;  // 0x134
    volatile LCD_DSI_VM_CMD_DATA4_REGISTER_T    LCD_DSI_VM_CMD_DATA4_REGISTER;  // 0x138
    volatile LCD_DSI_VM_CMD_DATA8_REGISTER_T    LCD_DSI_VM_CMD_DATA8_REGISTER;  // 0x13C
    volatile LCD_DSI_VM_CMD_DATAC_REGISTER_T    LCD_DSI_VM_CMD_DATAC_REGISTER;  // 0x140
    volatile LCD_DSI_CKSM_OUT_REGISTER_T        LCD_DSI_CKSM_OUT_REGISTER;      // 0x144
    volatile LCD_DSI_STATE_DBG0_REGISTER_T      LCD_DSI_STATE_DBG0_REGISTER;    // 0x148
    volatile LCD_DSI_STATE_DBG1_REGISTER_T      LCD_DSI_STATE_DBG1_REGISTER;    // 0x14C
    volatile LCD_DSI_STATE_DBG2_REGISTER_T      LCD_DSI_STATE_DBG2_REGISTER;    // 0x150
    volatile LCD_DSI_STATE_DBG3_REGISTER_T      LCD_DSI_STATE_DBG3_REGISTER;    // 0x154
    volatile LCD_DSI_STATE_DBG4_REGISTER_T      LCD_DSI_STATE_DBG4_REGISTER;    // 0x158
    volatile LCD_DSI_STATE_DBG5_REGISTER_T      LCD_DSI_STATE_DBG5_REGISTER;    // 0x15C
    volatile LCD_DSI_STATE_DBG6_REGISTER_T      LCD_DSI_STATE_DBG6_REGISTER;    // 0x160
    volatile LCD_DSI_STATE_DBG7_REGISTER_T      LCD_DSI_STATE_DBG7_REGISTER;    // 0x164
    volatile LCD_DSI_STATE_DBG8_REGISTER_T      LCD_DSI_STATE_DBG8_REGISTER;    // 0x168
    volatile LCD_DSI_STATE_DBG9_REGISTER_T      LCD_DSI_STATE_DBG9_REGISTER;    // 0x16C
    volatile LCD_DSI_DEBUG_SEL_REGISTER_T       LCD_DSI_DEBUG_SEL_REGISTER;     // 0x170
    uint32_t                                    reserved6;
    volatile LCD_DSI_BIST_PATTERN_REGISTER_T    LCD_DSI_BIST_PATTERN_REGISTER;  // 0x178
    volatile LCD_DSI_BIST_CON_REGISTER_T        LCD_DSI_BIST_CON_REGISTER;      // 0x17C
    volatile LCD_DSI_VM_CMD_DATA10_REGISTER_T   LCD_DSI_VM_CMD_DATA10_REGISTER; // 0x180
    volatile LCD_DSI_VM_CMD_DATA14_REGISTER_T   LCD_DSI_VM_CMD_DATA14_REGISTER; // 0x184
    volatile LCD_DSI_VM_CMD_DATA18_REGISTER_T   LCD_DSI_VM_CMD_DATA18_REGISTER; // 0x188
    volatile LCD_DSI_VM_CMD_DATA1C_REGISTER_T   LCD_DSI_VM_CMD_DATA1C_REGISTER; // 0x18C
    uint32_t                                    reserved7[28];
    volatile LCD_DSI_CMDQ_REGISTER_T            LCD_DSI_CMDQ_REGISTER[128];     // 0x200
} LCD_DSI_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t config                         : 8;    /*! Bit 0..7 */
        uint32_t dataID                         : 8;    /*! Bit 8..15 */
        uint32_t data0                          : 8;    /*! Bit 16..23 */
        uint32_t data1                          : 8;    /*! Bit 24..31 */
    } field;
} LCD_DSI_CMD_REGISTER_T;

typedef union {
    uint32_t	value;
    struct {
        uint32_t byte0                          : 8;    /*! Bit 0..7 */
        uint32_t byte1                          : 8;    /*! Bit 8..15 */
        uint32_t byte2                          : 8;    /*! Bit 16..23 */
        uint32_t byte3                          : 8;    /*! Bit 24..31 */
    } field;
} LCD_DSI_RX_DATA_REGISTER_T;

hal_display_dsi_status_t display_dsi_init(bool ssc_enable);
hal_display_dsi_status_t display_dsi_deinit(void);
hal_display_dsi_status_t display_dsi_enable_te(void);
hal_display_dsi_status_t display_dsi_set_command_queue(uint32_t *pData, uint8_t size, bool force_update);
hal_display_dsi_status_t display_dsi_set_command_queue_ex(uint32_t cmd, uint32_t size, uint32_t *para_list, bool force_update);
hal_display_dsi_status_t display_dsi_read_register(uint8_t cmd, uint8_t *buffer, uint8_t size);
hal_display_dsi_status_t display_dsi_enter_ulps(void);
hal_display_dsi_status_t display_dsi_exit_ulps(void);
hal_display_dsi_status_t display_dsi_set_dphy_timing(hal_display_dsi_dphy_timing_struct_t *timing);
hal_display_dsi_status_t display_dsi_restore_dsi_setting(void);
hal_display_dsi_status_t display_dsi_set_clock(uint32_t pll_clock, bool ssc_enable);
hal_display_dsi_status_t display_dsi_set_transfer_mode(hal_display_dsi_transfer_mode_t mode);
void display_dsi_start_bta_transfer(uint32_t cmd);
void display_dsi_apply_setting(void);


#endif // HAL_DISPLAY_DSI_MODULE_ENABLED

#endif //__HAL_DISPLAY_DSI_INTERNAL_H__
