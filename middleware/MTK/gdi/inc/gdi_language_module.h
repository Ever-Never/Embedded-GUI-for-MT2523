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

#ifndef __GDI_LANGUAGE_MODULE_H__
#define __GDI_LANGUAGE_MODULE_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "gdi_font_engine_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*****************************************************************************
* Define
*****************************************************************************/
#define MMI_LM_BASE_CHAR 0x25cc


#if defined(__MMI_LANG_RUSSIAN__) || defined(__MMI_LANG_SPANISH__) \
    || defined(__MMI_LANG_FRENCH__) || defined(__MMI_LANG_PORTUGUESE__) \
    || defined(__MMI_LANG_GERMAN__) || defined(__MMI_LANG_ITALIAN__) || defined(__MMI_LANG_TURKISH__) \
    || defined(__MMI_LANG_POLISH__) || defined(__MMI_LANG_CZECH__) || defined(__MMI_LANG_SLOVAK__) \
    || defined(__MMI_LANG_CROATIAN__) || defined(__MMI_LANG_SLOVENIAN__) || defined(__MMI_LANG_DANISH__) \
    || defined(__MMI_LANG_HUNGARIAN__) || defined(__MMI_LANG_FINNISH__) || defined(__MMI_LANG_ROMANIAN__) \
    || defined(__MMI_LANG_MOLDOVAN__) || defined(__MMI_LANG_VIETNAMESE__)|| defined (__MMI_LANG_YORUBA__)

#define  __MMI_LM_LANG_EUROPEAN_SUPPORT__

#endif /* European languages */

/*****************************************************************************
* Typedef
*****************************************************************************/
#define MMI_LM_CLUSTER_MAX  5

/* cluster relation attribute */
#define MMI_LM_CLUSTER_ATTR_NO_GAP 0
#define MMI_LM_CLUSTER_ATTR_GAP 1
#define MMI_LM_CLUSTER_ATTR_DISCONNECT 0
#define MMI_LM_CLUSTER_ATTR_CONNECT 1

#define MMI_LM_GET_CHARACTER_FROM_BUFFER(buffer) ((buffer)[0]|((buffer)[1]<<8))
#define MMI_LM_GET_PREV_CHARACTER_FROM_BUFFER(buffer, start) ((buffer > start) ? ((*(((buffer)-2)+0))|((*(((buffer)-2)+1))<<8)): 0)
#define MMI_LM_GET_NEXT_CHARACTER_FROM_BUFFER(buffer, end) (((buffer) < (end)) ? ((*(((buffer)+2)+0))|((*(((buffer)+2)+1))<<8)) : 0)

#define MMI_LM_TEST_ALPHABETIC_CHAR(c) (((((uint16_t)(c)>=(uint16_t)0x2E80) && ((uint16_t)(c)<=(uint16_t)0x9FAF))  || (c) == 0xFF1A || (c)==0xFE55 || (c)==0xFF1F|| (c) == '(' || (c) == 0xff08)?0:1)
#define MMI_LM_TEST_WORD_WRAPPING_BREAK_CHARACTER(c)  (((c) == 0x20 || (c) == 0x2D) ? 1:0)
#define MMI_LM_TEST_PREV_CONNECT(c) ((c) == ')' || (c) == 0xff09)
#define MMI_LM_TEST_BACK_CONNECT(c)  ((c) == '(' || (c) == 0xff08)

#define MMI_LM_CHARACTER_SIZE 2

typedef enum {
    MMI_LM_LANG_NONE = 0,
    MMI_LM_LANG_OTHERS,
    MMI_LM_LANG_MAX
} mmi_lm_lang_enum;


typedef struct {
    /* original data & cluster info */
    uint8_t *data_ptr;
    uint16_t display_buffer[MMI_LM_CLUSTER_MAX];

    /* display info */
    int16_t w;
    int16_t h;
    int8_t adv_x;
    int8_t adv_y;
    int8_t bx;
    int8_t by;
    int8_t rx;
    int8_t ry;


    /* relation flags */
    uint32_t is_gap : 1;   /* to identify if there is gap between current and previous cluster. */
    uint32_t is_connect : 1; /* to identify if current cluster could be put into next line by refering the previous cluster. */
    uint32_t data_len: 4;
    uint32_t cluster_len: 4;
    uint32_t lang_id: 4; /* mmi_lm_lang_enum */

} mmi_lm_cluster_info, *mmi_lm_cluster_info_p;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t baseline;
    uint8_t bordered;
    mmi_lm_cluster_info_p cluster;
    mmi_lm_cluster_info_p prev_cluster;
} mmi_lm_show_cluster_param, *mmi_lm_show_cluster_param_p;


/*****************************************************************************
* Extern Global Variable
*****************************************************************************/

/*****************************************************************************
* Extern Global Function
*****************************************************************************/
extern void mmi_fe_query_single_cluster(mmi_lm_cluster_info_p prev_cluster, mmi_lm_cluster_info_p cluster);

#define mmi_lm_cluster_init(x)  memset (x, 0, sizeof (mmi_lm_cluster_info))

extern void mmi_lm_get_current_cluster(
    uint8_t *start,
    uint8_t *end,
    mmi_lm_cluster_info_p prev2_cluster,
    mmi_lm_cluster_info_p prev_cluster,
    mmi_lm_cluster_info_p curr_cluster);
extern void mmi_lm_get_next_cluster(
    uint8_t *start,
    uint8_t *end,
    uint8_t *cursor,
    bool pos_only,
    mmi_lm_cluster_info_p next_cluster);
extern void mmi_lm_get_prev_cluster(
    uint8_t *start,
    uint8_t *end,
    uint8_t *cursor,
    bool pos_only,
    mmi_lm_cluster_info_p prev_cluster);

extern void mmi_lm_draw_cluster(mmi_lm_show_cluster_param_p param);

extern int32_t mmi_lm_cluster_translate(uint16_t *src, int32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __GDI_LANGUAGE_MODULE_H__ */

