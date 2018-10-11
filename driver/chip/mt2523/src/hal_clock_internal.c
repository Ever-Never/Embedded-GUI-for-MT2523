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

#include <stdio.h>
#include <assert.h>

#include "memory_attribute.h"

#ifdef MTK_SYSTEM_CLOCK_208M
ATTR_RWDATA_IN_TCM clock_cm_freq_state_id cm_freq_state = CM_208M_STA;
#elif defined MTK_SYSTEM_CLOCK_104M
ATTR_RWDATA_IN_TCM clock_cm_freq_state_id cm_freq_state = CM_104M_STA;
#elif defined MTK_SYSTEM_CLOCK_26M
ATTR_RWDATA_IN_TCM clock_cm_freq_state_id cm_freq_state = CM_26M_STA;
#else
ATTR_RWDATA_IN_TCM clock_cm_freq_state_id cm_freq_state = CM_208M_STA;
#endif


ATTR_TEXT_IN_TCM bool is_clk_use_lfosc(void)
{
#ifdef CLK_USE_LFOSC
    return true;
#else
    return false;
#endif
}

ATTR_TEXT_IN_TCM bool is_clk_use_mpll(void)
{
#ifdef CLK_USE_MPLL
    return true;
#else
    return false;
#endif
}

ATTR_TEXT_IN_TCM void cm_bus_clk_208m()
{
    if (is_clk_use_mpll()) {
        cm_bus_clk_208m_mpll_mpll();
    } else {
        cm_bus_clk_208m_mpll_hfosc();
    }
}

ATTR_TEXT_IN_TCM void cm_bus_clk_104m()
{
    if (is_clk_use_mpll()) {
        cm_bus_clk_104m_mpll_mpll();
    } else {
        cm_bus_clk_104m_hfosc_hfosc();
    }
}

ATTR_TEXT_IN_TCM void cm_bus_clk_26m(void)
{
    if (is_clk_use_lfosc()) {
        cm_bus_clk_26m_lfosc_lfosc();
    } else {
        cm_bus_clk_26m_dcxo_dcxo();
    }
}

ATTR_TEXT_IN_TCM void peri_clk_26m(void)
{
    if (is_clk_use_lfosc()) {
        peri_26m_lfosc();
    } else {
        peri_26m_dcxo();
    }
}

#endif

