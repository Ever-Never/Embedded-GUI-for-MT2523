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

#ifndef __GDI_FONT_ENGINE_H__
#define __GDI_FONT_ENGINE_H__

/**
 * @addtogroup GDI
 * @{
 * @addtogroup Font_engine Font engine
 * @{
 * This section provides APIs to get string information and draw string with specified font size and font color.
 * @{
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b SSC                        | Supplementary Service Codes. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Supplementary_service_codes">Wikipedia</a>. |
 * |\b ISO                        | International Organization for Standardization. For more information, please refer to <a href="https://en.wikipedia.org/wiki/International_Organization_for_Standardization">Wikipedia</a>. |
 * |\b BDF                        | Bitmap Distribution Format. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Glyph_Bitmap_Distribution_Format">Wikipedia</a>. |
 *
 * @section gdi_font_engine_api_usage How to use the font engine module
 * - Step 1: Generate the font data files by using the resgen tool. It's mandatory if adding or modifying BDF files.
 *    For resource generator tool usage, please change path to ".\tools\pc_tools\resgen_tool" in windows commands and type in "make ?".
 * - Step 2: Call the font engine API to show string or get string information. Here is an example to show how to show one string with medium font size and white color.
 *  - Sample code:
 *     @code
 *     gdi_font_engine_color_t text_color = {0, 255, 255, 255};// White color.
 *
 *     gdi_font_engine_size_t font_size;
 *
 *     // Please make sure that GDI initialization with the gdi_init() is completed before starting the following process;
 *
 *     // Clear screen.
 *     gdi_draw_filled_rectangle(0, 0, 128, 128, 0);
 * 
 *     // Set font size.
 *     gdi_font_engine_get_font_size(&font_size);
 *     if (font_size != GDI_FONT_ENGINE_FONT_MEDIUM) {
 *         font_size = GDI_FONT_ENGINE_FONT_MEDIUM;
 *     }
 *     gdi_font_engine_set_font_size(font_size);
 *
 *     // Set color.
 *     gdi_font_engine_set_text_color(text_color);
 *
 *     // Print String.
 *     gdi_font_engine_display_string_info_t string_info = {0};
 *     gdi_font_engine_string_info_t query = {0};
 *     query.string = L"Test!";
 *     query.count = wstrlen(L"Test!");
 *     gdi_font_engine_get_string_information(&query);
 *     string_info.x = 0;
 *     string_info.y = 0;
 *     string_info.string = L"Test!";
 *     string_info.length = wstrlen(L"Test!");
 *     string_info.baseline_height = query.baseline; // Suggest to set -1 if the string information is not inquired.
 *     gdi_font_engine_display_string(&string_info);
 *     
 *
 *     // Update screen.
 *     gdi_lcd_update_screen(0, 0, 128, 128);
 *     @endcode
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @defgroup gdi_font_engine_define Define
 * @{
 * This section defines the macros for the font engine.
 */

/**
 * @brief Define the SSC string of different languages. Please refer to the mtk_gLanguageArray in the FontRes.c generated by the resource generator tool for the SSC string of other languages.
 */
#define GDI_FONT_ENGINE_SSC_ENGLISH     "*#0044#"    /**< The SSC string of English. */
#define GDI_FONT_ENGINE_SSC_SCHINESE    "*#0086#"    /**< The SSC string of the simplified Chinese. */
#define GDI_FONT_ENGINE_SSC_TCHINESE    "*#0886#"    /**< The SSC string of the traditional Chinese. */

/**
 * @brief Define the size of the language details.
 */
#define GDI_FONT_ENGINE_LANGUAGE_NAME_LENGTH 40     /**< The size of the language name. */
#define GDI_FONT_ENGINE_SSC_SIZE 10          /**< The SSC size. */
#define GDI_FONT_ENGINE_COUNTRY_CODE_SIZE 8  /**< The country code size. */
#define GDI_FONT_ENGINE_ISO_SIZE 4           /**< The ISO size. */

/**
 * @brief Define the font size.
 */
#define  GDI_FONT_ENGINE_FONT_SMALL        0x0000   /**< Small font size. */
#define  GDI_FONT_ENGINE_FONT_MEDIUM       0x0001   /**< Medium font size. */
#define  GDI_FONT_ENGINE_FONT_LARGE        0x0002   /**< Large font size. */
typedef uint8_t gdi_font_engine_size_t; /**< Define the type of the font size. */

/**
 * @}
 */

/**
 * @defgroup gdi_font_engine_struct Struct
 * @{
 * This section defines the structures for the font engine.
 */

/**
 * @brief Define the return status of the font engine API.
 */
typedef enum {
    GDI_FONT_ENGINE_OK = 0,                             /**< Successful. */
    GDI_FONT_ENGINE_ERROR_LANGUAGE_NOT_SUPPORTED = -1,  /**< Unsupported language. */
} gdi_font_engine_status_t;

/**
 * @brief Define the color of the font engine.
 */
typedef struct {
    uint8_t alpha;    /**< The alpha value. */
    uint8_t red;      /**< The red value. */
    uint8_t green;    /**< The green value. */
    uint8_t blue;     /**< The blue value. */
} gdi_font_engine_color_t;

/**
 * @brief Define the string information structure to query.
 */
typedef struct {
    uint8_t *string;                    /**< A pointer to the Unicode string. It must be set when passing this structure to #gdi_font_engine_get_string_information. */
    int32_t count;                      /**< The character counter to query. It must be set when passing this structure to #gdi_font_engine_get_string_information. */
    int32_t width;                      /**< The string width. */
    int32_t height;                     /**< The string height. */
    int32_t baseline;                   /**< An imaginary line that is used to guide glyphs when rendering text. */
    int32_t max_ascent;                 /**< The distance from the baseline to the highest or upper coordinate used to place an outline point. */
    int32_t max_descent;                /**< The distance from the baseline to the lowest coordinate used to place an outline point. */
    int32_t advance_width;              /**< The horizontal distance to increment (for left-to-right writing) or decrement (for right-to-left writing) the pen position after a glyph has been rendered when processing the text. */
    int32_t advance_height;             /**< The vertical distance to decrement the pen position after a glyph has been renderedd. */
} gdi_font_engine_string_info_t;

/**
 * @brief Define the string information structure to display.
 */
typedef struct {
    int32_t x;                /**< Top left x coordinate. */
    int32_t y;                /**< Top left y coordinate. */
    uint8_t *string;          /**< The pointer to a Unicode string. */
    int32_t length;           /**< The string length. */
    int32_t baseline_height; /**< The base line of the string. -1, the font engine will query the string information. */
} gdi_font_engine_display_string_info_t;

/**
 * @brief Define the structure of the language details.
 */
typedef struct {
    uint8_t name[GDI_FONT_ENGINE_LANGUAGE_NAME_LENGTH];              /**< The language name. */
    uint8_t ssc[GDI_FONT_ENGINE_SSC_SIZE];                    /**< The SSC code of the language. */
    uint8_t country_code[GDI_FONT_ENGINE_COUNTRY_CODE_SIZE];  /**< The country code of the language. */
    uint8_t iso_code[GDI_FONT_ENGINE_ISO_SIZE];               /**< The ISO code of the language. */
} gdi_font_engine_language_t;
/**
* @}
*/

/**
 * @brief     This function sets the current font size.
 * @param[in] font_size     is the font size to set.
 * @return    void.
 */
uint8_t gdi_font_engine_set_font_size(gdi_font_engine_size_t font_size);

/**
 * @brief      This function gets the current font size.
 * @param[out] font_size     is the size of the current font.
 * @return     void.
 */
void gdi_font_engine_get_font_size(gdi_font_engine_size_t *font_size);

/**
 * @brief     This function sets the text color.
 * @param[in] color     is the text color.
 * @return    void.
 */
void gdi_font_engine_set_text_color(gdi_font_engine_color_t color);

/**
 * @brief     This function gets the text color.
 * @return    The text color.
 */
gdi_font_engine_color_t gdi_font_engine_get_text_color(void);

/**
 * @brief     This function displays the specific string.
 * @param[in] parameter     is the user-defined data to draw the string.
 * @return    GDI_FONT_ENGINE_OK, the operation completed successfully.
 */
uint32_t gdi_font_engine_display_string(gdi_font_engine_display_string_info_t *parameter);


/**
 * @brief     This function gets the detailed information of the specified string.
 * @param[in,out] query     is the user-defined query information and query the result provided by the font engine.
 * @return    Indicates the number of characters to get the information.
 */
uint32_t gdi_font_engine_get_string_information(gdi_font_engine_string_info_t *query);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 * @}
 */

#endif /* __GDI_FONT_ENGINE_H__ */

