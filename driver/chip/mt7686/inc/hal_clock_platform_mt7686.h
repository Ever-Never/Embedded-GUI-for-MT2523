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

#ifndef __HAL_CLOCK_PLATFORM_MT7686_H__
#define __HAL_CLOCK_PLATFORM_MT7686_H__

#include "hal_platform.h"

#ifdef HAL_CLOCK_MODULE_ENABLED

#define STR_VALUE(arg)      #arg
#define __stringify(name) STR_VALUE(name)


/*************************************************************************
 * Define clock force on register and bits structure
 *************************************************************************/
enum clock_force_on_id {
    CLK_SYS_FORCE_ON,
    CLK_SFC_FORCE_ON,
    CLK_CONN_FORCE_ON,
    CLK_SPIMST_FORCE_ON,
    CLK_SDIOMST_FORCE_ON,
    NR_CLKFORCE_ON,
};

/*************************************************************************
 * Define clock change bit register and bits structure
 *************************************************************************/
typedef enum {
    CHG_SYS,
    CHG_SFC,
    CHG_CONN,
    CHG_SPIMST,
    NR_CLKCHANGE_BIT,
} clock_change_bit_id;

/*************************************************************************
 * Define clock mux register and bits structure
 *************************************************************************/
typedef enum {
    CLK_SYS_SEL,
    CLK_SFC_SEL,
    CLK_CONN_SEL,
    CLK_SPIMST_SEL,
    CLK_XTALCTL_SEL,
    CLK_SDIOMST_SEL,
    NR_MUXS,
    CLK_MUX_SEL_NONE,
} clock_mux_sel_id;

/*************************************************************************
* Define clock gating register and bits structure
* Note: MUST modify clk_cg_mask in hal_clock.c if hal_clock_cg_id is changed
*************************************************************************/
enum {
    NR_CLOCKS           = HAL_CLOCK_CG_END
};

enum {
    GRP_PDN_COND0       = 0,
    GRP_XO_PDN_COND0    = 1,
    NR_GRPS             = 2,
};

#endif /* HAL_CLOCK_MODULE_ENABLED */
#endif /* __HAL_CLOCK_PLATFORM_MT7686_H__ */
