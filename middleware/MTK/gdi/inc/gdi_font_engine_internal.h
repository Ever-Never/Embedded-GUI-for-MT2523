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

#ifndef __GDI_FONT_ENGINE_INTERNAL_H__
#define __GDI_FONT_ENGINE_INTERNAL_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "gdi_font_engine.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @brief Define the font attribute.
 */
#define  FONTATTRIB_NORMAL                      0x00000001  /**< Normal font. */
#define  FONTATTRIB_BOLD                        0x00000002  /**< Bold font. */
#define  FONTATTRIB_ITALIC                      0x00000004  /**< Italic font. */
#define  FONTATTRIB_OBLIQUE                     0x00000008  /**< Oblique font. */
#define  FONTATTRIB_UNDERLINE                   0x00000010  /**< Font with underline. */
#define  FONTATTRIB_STRIKETHROUGH               0x00000020  /**< Font with strike through effect. */

/**
 * @brief Define the attribute structure of the font.
 */
typedef struct {
    uint8_t *string;     /* string buffer */
    int32_t len;       /* len of the string */
    int32_t subarray_start : 16; /* the start index of the string */
    int32_t subarray_len : 16;   /* the lenght of the show segment */
    int32_t x;                   /* x position */
    int32_t y;                   /* y position */
    int32_t baseline;            /* baseline */
    uint8_t bordered;             /* bordered or not */
    uint8_t langid;               /* language id */
} mmi_fe_show_one_cluster_param_struct, *mmi_fe_show_one_cluster_param_struct_p;

/**
 * @brief Define the attribute structure of the font.
 */
typedef struct {
    uint8_t bold;            /**< The bold style. */
    uint8_t italic;          /**< The italic style. */
    uint8_t underline;       /**< The underline style. */
    uint8_t size;            /**< The font size. */
    uint8_t oblique;         /**< The oblique style. */
    uint8_t strike_through;  /**< The strike through style. */
} gdi_font_engine_font_attribute_t;

/**
 * @brief Define the glyph info structure.
 */
typedef struct {
    int16_t ascent;        /**< The glyph ascent. */
    int16_t descent;       /**< The glyph descent. */
    int16_t advance_x;     /**< The glyph advance width. */
    uint16_t width;        /**< The glyph width. It is considered with font attribute. */
    uint16_t height;       /**< The glyph height. It is considered with font attribute. */
} gdi_font_engine_glyph_metrics_t;


void mmi_fe_show_single_cluster(mmi_fe_show_one_cluster_param_struct_p param);
void gdi_font_engine_get_current_language_index(uint8_t *language_index);

/**
 * @brief      This function gets the current supported language list and number.
 * @param[out] language_list     is the information list of languages currently supported.
 * @param[out] language_number   is the number of languages currently supported.
 * @return     void.
 */
void gdi_font_engine_get_language_list(const gdi_font_engine_language_t **language_list, uint8_t *language_number);

/**
 * @brief     This function sets the current language by the SSC string.
 * @param[in] language_ssc     is the SSC string buffer with at least the size of GDI_FONT_ENGINE_SSC_SIZE.
 * @return    GDI_FONT_ENGINE_OK, the operation completed successfully.
 *            GDI_FONT_ENGINE_ERROR_LANGUAGE_NOT_SUPPORTED, the operation has failed.
 */
gdi_font_engine_status_t gdi_font_engine_set_current_language_by_ssc(const int8_t *language_ssc);

/**
 * @brief      This function gets the SSC of the current language.
 * @param[out] language_ssc     is SSC string buffer which at least the size of GDI_FONT_ENGINE_SSC_SIZE.
 * @return     void.
 */
void gdi_font_engine_get_current_language_ssc(int8_t *language_ssc);

/**
 * @brief     This function gets raw string data of the current language by the string ID.
 * @param[in] string_id    is the string ID added in the resource generator tool flow.
 * @return    The raw string data of the current language.
 */
uint8_t *gdi_font_engine_get_string_data(uint16_t string_id);

#ifdef __cplusplus
}
#endif


#endif /* __GDI_FONT_ENGINE_INTERNAL_H__ */

