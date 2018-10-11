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

#include "bl_misc.h"
#include "bl_mtk_bb_reg.h"

#define BL_CONFG_BASE   (0xA2010000)
#define BL_SYSTEM_INFOD ((BL_P_UINT_32)(BL_CONFG_BASE+0x0224))//(BL_CONFG_BASE+0x0224)
#define BL_HW_MISC3     ((BL_P_UINT_32)(BL_CONFG_BASE+0x0234))//(BL_CONFG_BASE+0x0234)
#define BL_PLL_RSV_CON0 ((BL_P_UINT_32)(0xA2040360))

void bl_bonding_io_reg(void)
{
    unsigned int bond_sta;
    unsigned int hw_misc3;
    unsigned int bond_extsf;
    unsigned int bond_psram;
    unsigned int bond_gnss;
    unsigned int bond_sipsf_18v;

    unsigned int bond_pu = 0xF;
    unsigned int bond_pd = 0x0;

    bond_sta = *BL_SYSTEM_INFOD;//(unsigned int *)(BL_SYSTEM_INFOD);
    hw_misc3 = *BL_HW_MISC3;//(unsigned int *)(BL_HW_MISC3);

    bond_extsf     = (bond_sta >> 10) & 0x1;
    bond_psram     = (bond_sta >> 7) & 0x1;;
    bond_gnss      = (bond_sta >> 3) & 0x1;;
    bond_sipsf_18v = (bond_sta >> 2) & 0x1;;

    if (!bond_psram) {
        bond_pu &= 0x7;
        bond_pd |= 0x8;
    }
    if (!bond_extsf) {
        bond_pu &= 0xB;
        bond_pd |= 0x4;
    }
    if (!bond_gnss) {
        bond_pu &= 0xD;
        bond_pd |= 0x2;
    }
    if (!bond_sipsf_18v) {
        bond_pu &= 0xE;
        bond_pd |= 0x1;
    }

    (*((volatile unsigned int *)(BL_HW_MISC3)) = (unsigned int)(bond_pu << 12) |  (bond_pd << 8) | (hw_misc3 & 0x1));
}

void bl_d2d_io_pull_down(void)
{
    (*((volatile unsigned int *)(BL_PLL_RSV_CON0)) = (unsigned int)(0x11));
}
