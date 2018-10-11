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

#ifndef __GDI_COMMON_LANGUAGE_H__
#define __GDI_COMMON_LANGUAGE_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>


#include "gdi_common_language.h"
#include "gdi_language_module.h"
#include "gdi_font_engine_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/*****************************************************************************
* Define
*****************************************************************************/

/*****************************************************************************
* Typedef
*****************************************************************************/






/*****************************************************************************
* Extern Global Variable
*****************************************************************************/

/*****************************************************************************
* Extern Global Function
*****************************************************************************/
extern void mmi_lm_common_get_current_cluster(
    uint8_t *start,
    uint8_t *end,
    mmi_lm_cluster_info_p prev2_cluster,
    mmi_lm_cluster_info_p prev_cluster,
    mmi_lm_cluster_info_p curr_cluster,
    bool pos_only);
extern void mmi_lm_common_get_next_cluster(
    uint8_t *start,
    uint8_t *end,
    uint8_t *cursor,
    bool pos_only,
    mmi_lm_cluster_info_p next_cluster);
extern void mmi_lm_common_get_prev_cluster(
    uint8_t *start,
    uint8_t *end,
    uint8_t *cursor,
    bool pos_only,
    mmi_lm_cluster_info_p prev_cluster);

extern void mmi_lm_common_draw_cluster(mmi_lm_show_cluster_param_p param);
extern int32_t mmi_lm_common_cluster_translate(uint16_t *src, uint16_t *dst, int32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __GDI_COMMON_LANGUAGE_H__ */

