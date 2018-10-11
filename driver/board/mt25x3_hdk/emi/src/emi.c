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

#include <stdint.h>
#include "emi_hw_internal.h"
#include "emi_sw_internal.h"


static EMI_RUNTIME_Info *emi_cur_setting;

kal_int32 EMI_QueryCurSetting(void *emi_setting)
{
    emi_cur_setting = emi_setting ;
    emi_cur_setting->EMI_CONM_value = *EMI_CONM;
    emi_cur_setting->EMI_GENA_value = *EMI_GENA;
    emi_cur_setting->EMI_RDCT_value = *EMI_RDCT;
    emi_cur_setting->EMI_DLLV_value = *EMI_DLLV;

    emi_cur_setting->EMI_IDLC_value = *EMI_IDLC;
    emi_cur_setting->EMI_IDLD_value = *EMI_IDLD;
    emi_cur_setting->EMI_IDLE_value = *EMI_IDLE;

    emi_cur_setting->EMI_ODLC_value = *EMI_ODLC;
    emi_cur_setting->EMI_ODLD_value = *EMI_ODLD;
    emi_cur_setting->EMI_ODLE_value = *EMI_ODLE;
    emi_cur_setting->EMI_ODLF_value = *EMI_ODLF;

    emi_cur_setting->EMI_IOA_value = *EMI_IOA;
    emi_cur_setting->EMI_IOB_value = *EMI_IOB;

    emi_cur_setting->EMI_DSRAM_value = *EMI_DSRAM;
    emi_cur_setting->EMI_MSRAM_value = *EMI_MSRAM;

    emi_cur_setting->EMI_ARBA_value = *EMI_ARBA;
    emi_cur_setting->EMI_ARBB_value = *EMI_ARBB;
    emi_cur_setting->EMI_ARBC_value = *EMI_ARBC;

    emi_cur_setting->EMI_SLCT_value = *EMI_SLCT;
    emi_cur_setting->EMI_ABCT_value = *EMI_ABCT;

    emi_cur_setting->reg_num = 20;

    return 0;
}

