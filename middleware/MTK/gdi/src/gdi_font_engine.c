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
/*****************************************************************************
*  Include
*****************************************************************************/
#include "gdi_font_show_char.h"
#include "string.h"
#include "gdi_common_language.h"
#include "gdi_language_module.h"
#include "gdi_font_engine_internal.h"
#include "gdi_font_resource.h"
#include "gdi_datatype.h"
#include "gdi_internal.h"
#include "syslog.h"

log_create_module(FONT, PRINT_LEVEL_INFO);

/*****************************************************************************
*  Define
*****************************************************************************/
static const uint32_t font_tilt_table[61] = {
    0, 0, 0, 0, 0, 1, 1, 1, 1, 2,
    2, 2, 2, 2, 3, 3, 3, 3, 4, 4,
    4, 4, 5, 5, 5, 5, 5, 6, 6, 6,
    6, 7, 7, 7, 7, 8, 8, 8, 8, 8,
    9, 9, 9, 9, 10, 10, 10, 10, 11, 11,
    11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13
};

#define ENCODING_LENGTH         2

/* to check the control chars
   add 0x0A,0x0D~0x0F for displaying control characters as space
   add 0xA0 for NBSP in Vietnamese as space */
#define FONT_TEST_CONTROL_CHAR(c)      \
    (     (c=='\t'/* 0x09 */)                \
       || (c==0x08)                        \
       || (c==0x0A)                        \
       || (c==0x0D)                        \
       || (c==0x0E)                        \
       || (c==0x0F)                        \
       || (c==0xA0)                        \
       || (c== 0x2028)                    \
       || (c == 0x2029)                   \
    )

#define FONT_TEST_FORMATTING_CHARS(c)   (((c) >= 0x200C && (c)<= 0x200f) || ((c) >= 0x202a && (c)<= 0x202f))
#define FONT_TEST_DATE_CHARS(c)   ((c) >= 0x87 && (c)<= 0x94)
#define FONT_TEST_SPACE_CHAR(c) (((c) >= 0x2000 && (c)<= 0x200A) || ((c) == 0xfeff)) /* 0x200B is zero space but we treat it as null character. */
#define FONT_TEST_ZERO_WIDTH_SPACE(c) ((c) == 0x200B)
#define FONT_TEST_ZERO_WIDTH_CHAR(c) (((c)<= 0x1F) || (((c)>= 0x80) && ((c)<= 0x84)) || (((c)>= 0x95) && ((c)<= 0x9f)))

#define MAX_SHOW_STRING_TEXT_LENGTH          700

#define FONT_TEST_NEW_LINE_CHAR(c) (((c) == 0x0a || (c) == 0x0d || (c) == 0x2028 || (c) == 0x2029) ? (1) : (0))

//#define __FONT_ENGINE_SWLA__
#define FE_SWLA_START(_symbol_)
#define FE_SWLA_STOP(_symbol_)

/* transparent color (blue) */
#define TRANSPARENT_COLOR_R 0
#define TRANSPARENT_COLOR_G 0
#define TRANSPARENT_COLOR_B 0xff
#define FE_DISPLAY_FLAG_DATE_CHAR  0x01
#define FE_DISPLAY_FLAG_PROPRIETARY_SYMBOL_CHAR  0x02
/*****************************************************************************
*  Typedef
*****************************************************************************/
typedef struct {
    uint32_t index;    /* cluster index in text buffer */
    int32_t x;        /* cluster x position */
    uint8_t len;       /* Cluster length */
    uint8_t width;     /* cluster width. */
    uint8_t adv_w;     /* cluster adv_w. */
    uint8_t bidi_type;  /* is arabic char */
} mmi_fe_cluster_metrics_struct, *mmi_fe_cluster_metrics_struct_p;

typedef struct {
    int32_t x;
    int32_t y;
    uint8_t *string;
    int32_t length;
    int32_t baseline_height;
    uint32_t bordered;
    int32_t visible_x_start;
    int32_t visible_x_end;
    int32_t visible_x1_start;
    int32_t visible_x1_end;
    int32_t string_width;
    gdi_color_t border_color;
    gdi_color_t text_color;
    int32_t max_ascent;
    int32_t max_descent;
    int32_t clip_x1;
    int32_t clip_x2;
    int32_t clip_y1;
    int32_t clip_y2;
} mmi_fe_showstring_int_param_struct, *mmi_fe_showstring_int_param_struct_p;

typedef enum {
    MMI_FE_INFO_ONLY,
    MMI_FE_GET_DATA,
    MMI_FE_VALID_CHECK,
    MMI_FE_GLYPH_INDEX
} mmi_fe_font_query_type;
/*****************************************************************************
*  Global Variables
*****************************************************************************/
uint8_t gnCurrentFontAttrib = FONTATTRIB_NORMAL;
static uint32_t fe_display_flag = 0;
bool cancelWidth = 0;

typedef void (*fe_save_cluster_info_cb) (mmi_fe_cluster_metrics_struct metric, void *user_data);

static gdi_font_engine_color_t g_mmi_fe_text_color = {0, 0, 0, 100};
static gdi_font_engine_color_t g_mmi_fe_text_border_color = {0, 0, 0, 100};

static uint32_t mmi_fe_get_string_widthheight_int(
    gdi_font_engine_string_info_t *query,
    uint8_t *direction,
    fe_save_cluster_info_cb f_save_info,
    void *user_data,
    bool is_BIDI);


#define FE_LOCK
#define FE_UNLOCK

extern const gdi_font_engine_language_t mtk_gLanguageArray[];
extern font_group_struct g_fontfamily[];

extern const sCustFontData proprietaryfont_small;
const RangeData *pAMTKProprietaryFont_RangeData;

uint32_t gnCurrentFont = GDI_FONT_ENGINE_FONT_SMALL;
const sCustFontData *gpCurrentFont = NULL;
extern const uint16_t mtk_gMaxDeployedLangs;
static uint16_t gCurrLangIndex = 0;

/*****************************************************************************
*  Global Function
*****************************************************************************/
static uint32_t mmi_fe_show_string_int(mmi_fe_showstring_int_param_struct_p param);
static void GetLangIndex(uint8_t *pCount, int32_t *pnFontIndex, int32_t *pnIndex, uint32_t nCh, uint32_t *pnBase);
static int32_t SearchIndexinList(uint8_t nListIndex, uint16_t nFontCount, uint32_t nCh, uint32_t *pnBase);
static int32_t SearchInPropFont(uint32_t nCh, uint32_t *pnBase);

/*****************************************************************************
 * FUNCTION
 *  mmi_fe_test_display_as_space
 * DESCRIPTION
 *  To test if a special character should be displayed as space or not
 * PARAMETERS
 *  ch      [IN]
 * RETURNS
 *
 *****************************************************************************/
#if defined(__ICCARM__)
#pragma inline
static bool mmi_fe_test_display_as_space(uint32_t ch)
#else
__inline static bool mmi_fe_test_display_as_space(uint32_t ch)
#endif
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (FONT_TEST_CONTROL_CHAR(ch) || FONT_TEST_SPACE_CHAR(ch)) {
        return true;
    }
    return false;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_fe_test_display_as_null
 * DESCRIPTION
 *  To test if a special character should be displayed as zero width character or not
 * PARAMETERS
 *  ch      [IN]
 * RETURNS
 *
 *****************************************************************************/
#if defined(__ICCARM__)
#pragma inline
static bool mmi_fe_test_display_as_null(uint32_t ch)
#else
__inline static bool mmi_fe_test_display_as_null(uint32_t ch)
#endif
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (ch == 0x7f ||
            FONT_TEST_ZERO_WIDTH_CHAR(ch) ||
            FONT_TEST_FORMATTING_CHARS(ch) ||
            FONT_TEST_ZERO_WIDTH_SPACE(ch) ||
            (ch == 0x0085 && (!(fe_display_flag & FE_DISPLAY_FLAG_PROPRIETARY_SYMBOL_CHAR)))) {
        return true;
    }
    return false;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_fe_test_display_as_invalid
 * DESCRIPTION
 *  To test if a special character should be displayed as invalid character (Dec: 65515/ Hex: 0xFFEB) or not
 * PARAMETERS
 *  ch      [IN]
 * RETURNS
 *
 *****************************************************************************/
#if defined(__ICCARM__)
#pragma inline
static bool mmi_fe_test_display_as_invalid(uint32_t ch)
#else
__inline static bool mmi_fe_test_display_as_invalid(uint32_t ch)
#endif
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((FONT_TEST_DATE_CHARS(ch) && (!(fe_display_flag & FE_DISPLAY_FLAG_DATE_CHAR))) ||
            (ch == 0x0086 && (!(fe_display_flag & FE_DISPLAY_FLAG_PROPRIETARY_SYMBOL_CHAR)))) {
        return true;
    }
    return false;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_font_engine_set_text_color
 * DESCRIPTION
 *  To set text color
 * PARAMETERS
 *  color       [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_font_engine_set_text_color(gdi_font_engine_color_t color)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_LOCK;
    g_mmi_fe_text_color = color;
    GDI_UNLOCK;
}


/*****************************************************************************
 * FUNCTION
 *  gdi_font_engine_get_text_color
 * DESCRIPTION
 *  To get text color
 * PARAMETERS
 *  void
 * RETURNS
 *  color
 *****************************************************************************/
gdi_font_engine_color_t gdi_font_engine_get_text_color(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_font_engine_color_t ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_LOCK;
    ret = g_mmi_fe_text_color;
    GDI_UNLOCK;
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  SetFontValues
 * DESCRIPTION
 *
 * PARAMETERS
 *  nFont           [IN]
 *  arrCount        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void SetFontValues(uint32_t nFont, uint8_t arrCount)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    const font_group_struct *font_group = &(g_fontfamily[nFont]);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    gpCurrentFont = font_group->fontData[arrCount];
}

/*****************************************************************************
 * FUNCTION
 *  gdi_font_engine_set_font
 * DESCRIPTION
 *  To set the fonts.
 *  This is used to set all different type of font with different sizes.
 * PARAMETERS
 *  font            [IN]        The font attribute
 * RETURNS
 *  uint8_t
 *****************************************************************************/
uint8_t gdi_font_engine_set_font(gdi_font_engine_font_attribute_t font)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t font_size = font.size;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_LOCK;
    FE_LOCK;

    //printf("gdi_font_engine_set_font, font_size\n", font_size);
    gnCurrentFont = font_size;
    gnCurrentFontAttrib = 0;

    if (font.bold) {
        gnCurrentFontAttrib |= FONTATTRIB_BOLD;
    }
    if (font.italic) {
        gnCurrentFontAttrib |= FONTATTRIB_ITALIC;
    }
    if (font.oblique) {
        gnCurrentFontAttrib |= FONTATTRIB_OBLIQUE;
    }
    if (font.underline) {
        gnCurrentFontAttrib |= FONTATTRIB_UNDERLINE;
    }
    if (font.strike_through) {
        gnCurrentFontAttrib |= FONTATTRIB_STRIKETHROUGH;
    }
    if (!gnCurrentFontAttrib) {
        gnCurrentFontAttrib |= FONTATTRIB_NORMAL;
    }

    SetFontValues(gnCurrentFont, 0);


    FE_UNLOCK;
    GDI_UNLOCK;
    return 0;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_font_engine_set_font_size
 * DESCRIPTION
 *  To set the fonts.
 *  This is used to set all different type of font with different sizes.
 * PARAMETERS
 *  font            [IN]        The font size
 * RETURNS
 *  uint8_t
 *****************************************************************************/
uint8_t gdi_font_engine_set_font_size(gdi_font_engine_size_t font_size)
{
    gdi_font_engine_font_attribute_t font = {0};
    return gdi_font_engine_set_font(font);
}

/*****************************************************************************
 * FUNCTION
 *  gdi_font_engine_get_font
 * DESCRIPTION
 *  To get the current font.
 *  This is used to get current font attribute.
 * PARAMETERS
 *  font            [IN/OUT]
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_font_engine_get_font(gdi_font_engine_font_attribute_t *font)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_LOCK;
    //printf("gdi_font_engine_get_font, gnCurrentFont %d, gnCurrentFontAttrib %d\n", gnCurrentFont, gnCurrentFontAttrib);
    memset(font, 0, sizeof(gdi_font_engine_font_attribute_t));
    font->size = gnCurrentFont;

    if ((gnCurrentFontAttrib & FONTATTRIB_BOLD)) {
        font->bold = true;
    }
    if ((gnCurrentFontAttrib & FONTATTRIB_ITALIC)) {
        font->italic = true;
    }
    if ((gnCurrentFontAttrib & FONTATTRIB_OBLIQUE)) {
        font->oblique = true;
    }
    if ((gnCurrentFontAttrib & FONTATTRIB_UNDERLINE)) {
        font->underline = true;
    }
    
    GDI_UNLOCK;

}

/*****************************************************************************
 * FUNCTION
 *  gdi_font_engine_get_font_size
 * DESCRIPTION
 *  To set the fonts.
 *  This is used to set all different type of font with different sizes.
 * PARAMETERS
 *  font            [IN]        The font size
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_font_engine_get_font_size(gdi_font_engine_size_t *font_size)
{
    gdi_font_engine_font_attribute_t font = {0};
    gdi_font_engine_get_font(&font);
    *font_size = font.size;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_fe_get_font_data_internal
 * DESCRIPTION
 *  To get font resource and data for a specific UCS2 character
 * PARAMETERS
 *  Ch                  [IN]
 *  type                [IN]
 *  ppCharData          [OUT]
 *  pnWidth             [OUT]
 *  advance_x           [OUT]
 *  pnHeight            [OUT]
 *  glyph_width         [OUT]
 *  glyph_height        [OUT]
 *  ascent              [?]
 *  descent             [?]
 *  valid               [?]
 *  Font(?)             [IN]
 * RETURNS
 *  uint32_t the number of byte of the font resource
 *****************************************************************************/
#if defined(__ICCARM__)
#pragma inline
static uint32_t mmi_fe_get_font_data_internal(
    uint16_t Ch,
    uint8_t type,
    uint8_t **ppCharData,
    int32_t *pnWidth,
    int32_t *advance_x,
    uint16_t *pnHeight,
    uint16_t *glyph_width,
    uint16_t *glyph_height,
    int32_t *ascent,
    int32_t *descent,
    bool *valid)
#else
__inline static uint32_t mmi_fe_get_font_data_internal(
    uint16_t Ch,
    uint8_t type,
    uint8_t **ppCharData,
    int32_t *pnWidth,
    int32_t *advance_x,
    uint16_t *pnHeight,
    uint16_t *glyph_width,
    uint16_t *glyph_height,
    int32_t *ascent,
    int32_t *descent,
    bool *valid)
#endif
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t count;
    int32_t NumChar = 0;
    int32_t index = -1;
    int32_t nFontIndex = -1;
    uint32_t nIndexInList;
    uint32_t nBase;

    const sCustFontData *TempFont = gpCurrentFont;
    //printf("mmi_fe_get_font_data_internal\n");
    GetLangIndex(&count, &nFontIndex, &index, Ch, &nBase);
    //printf("mmi_fe_get_font_data_internal, index %d\n", index);
    if (index > -1) {
        TempFont = gpCurrentFont;
    } else {
        index = SearchInPropFont(Ch, &nBase);
        if (index == -1) {
            /* ch is junk */
            Ch = 65515; /* 0x4E01;//0x000F; */
            *valid = false;
            index = SearchInPropFont(Ch, &nBase);
            if (index == -1) {
                *advance_x = 0;
                *pnWidth = 0;
                *glyph_height = 0;
                *glyph_width = 0;
                *ascent = 0;
                *descent = 0;
                //printf("mmi_fe_get_font_data_internal, junk char\n");
                return NumChar;
            }
        }
        TempFont = &proprietaryfont_small;
    }

    *pnWidth = TempFont->nWidth;
    *advance_x = TempFont->nWidth;
    *pnHeight = TempFont->nHeight;
    *ascent = TempFont->nAscent;
    *descent = TempFont->nDescent;
    nIndexInList = index + (Ch - nBase);
    //printf("mmi_fe_get_font_data_internal, nIndexInList %d\n", nIndexInList);

    if (TempFont->nEquiDistant) {

        NumChar = TempFont->nCharBytes;
        *ppCharData = (TempFont->pDataArray + (nIndexInList * NumChar));
    } else {
        *pnWidth = TempFont->pWidthArray[nIndexInList];
        *advance_x = TempFont->pWidthArray[nIndexInList];
        NumChar = (TempFont->pOffsetArray[nIndexInList + 1] - TempFont->pOffsetArray[nIndexInList]);
        *ppCharData = (TempFont->pDataArray + TempFont->pOffsetArray[nIndexInList]);
    }

    /* usually the character width = data width */
    //printf("mmi_fe_get_font_data_internal, w %d, h %d\n", *pnWidth, *pnHeight);
    *glyph_height = *pnHeight;
    *glyph_width = *pnWidth;
    if ((gnCurrentFontAttrib & FONTATTRIB_BOLD)) {
        /* when bold word, character width is one more than data width */
        *pnWidth = *pnWidth + 1;
        *advance_x = *advance_x + 1;
    }

    return NumChar;

}
/*****************************************************************************
 * FUNCTION
 *  mmi_fe_get_font_data
 * DESCRIPTION
 *  To get font resource and data for a specific UCS2 character
 * PARAMETERS
 *  type                [IN]
 *  ch                  [IN]
 *  ppCharData          [OUT]
 *  data_attr           [?]
 *  pnWidth             [OUT]
 *  advance_x           [OUT]
 *  glyph_width         [OUT]
 *  glyph_height        [OUT]
 *  bearing_x           [?]
 *  ascent              [?]
 *  descent             [?]
 *  valid               [?]
 *  pnHeight(?)         [OUT]
 *  Font(?)             [IN]
 *  Ch(?)               [IN]
 * RETURNS
 *  uint32_t the number of byte of the font resource
 *****************************************************************************/
#if defined(__ICCARM__)
#pragma inline
static uint32_t mmi_fe_get_font_data(
    uint8_t type,
    uint16_t ch,
    uint8_t **ppCharData,
    uint32_t *data_attr,
    int32_t *pnWidth,
    int32_t *advance_x,
    uint16_t *glyph_width,
    uint16_t *glyph_height,
    int32_t *bearing_x,
    int32_t *ascent,
    int32_t *descent,
    bool *valid)
#else
__inline static uint32_t mmi_fe_get_font_data(
    uint8_t type,
    uint16_t ch,
    uint8_t **ppCharData,
    uint32_t *data_attr,
    int32_t *pnWidth,
    int32_t *advance_x,
    uint16_t *glyph_width,
    uint16_t *glyph_height,
    int32_t *bearing_x,
    int32_t *ascent,
    int32_t *descent,
    bool *valid)
#endif
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint16_t dummy_pnHeight;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    FE_SWLA_START("GFD");
    //printf("mmi_fe_get_font_data\n");
    *valid = true;

    if (type != MMI_FE_GLYPH_INDEX) {
        if (mmi_fe_test_display_as_null((uint32_t) ch)) {
            *ascent = 0;
            *descent = 0;
            *pnWidth = 0;
            *advance_x = 0;
            *glyph_height = 0;
            *glyph_width = 0;
            *data_attr = 0;
            return 0;
        }
        if (mmi_fe_test_display_as_space(ch)) {
            ch = 0x20;
        }
        if (mmi_fe_test_display_as_invalid(ch)) {
            ch = 0xFFEB;
        }
    }
    *data_attr = 0; /* don't assign any data attribute */
    *bearing_x = 0;
    /* for bdf font, it is ok to save data into cache in the two case. */
    return mmi_fe_get_font_data_internal(
               ch,
               type,
               ppCharData,
               pnWidth,
               advance_x,
               &dummy_pnHeight,
               glyph_width,
               glyph_height,
               ascent,
               descent,
               valid);
}


/*****************************************************************************
 * FUNCTION
 *  mmi_fe_show_font_data
 * DESCRIPTION
 *  to show character at the desired x, y position by the color
 * PARAMETERS
 *  ch                  [IN]
 *  x                   [IN]
 *  y                   [IN]
 *  glyph_ptr           [?]
 *  glyph_size          [IN]
 *  glyph_attr          [IN]
 *  glyph_width         [IN]
 *  glyph_height        [IN]
 *  bordered            [IN]
 *  font_attr           [IN]
 *  text_color          [IN]
 *  border_color        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
#if defined(__ICCARM__)
#pragma inline
static void mmi_fe_show_font_data(
    uint16_t ch,
    int32_t x,
    int32_t y,
    uint8_t *glyph_ptr,
    uint32_t glyph_size,
    uint32_t glyph_attr,
    uint16_t glyph_width,
    uint16_t glyph_height,
    uint32_t bordered,
    uint8_t font_attr,
    gdi_color_t text_color,
    gdi_color_t border_color)
#else
__inline static void mmi_fe_show_font_data(
    uint16_t ch,
    int32_t x,
    int32_t y,
    uint8_t *glyph_ptr,
    uint32_t glyph_size,
    uint32_t glyph_attr,
    uint16_t glyph_width,
    uint16_t glyph_height,
    uint32_t bordered,
    uint8_t font_attr,
    gdi_color_t text_color,
    gdi_color_t border_color)
#endif
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //printf("mmi_fe_show_font_data, bordered=%d, gnCurrentFontAttrib %d\n", bordered, gnCurrentFontAttrib);
    if (bordered == 1 || bordered == 2) {
        gdi_show_char_bordered(
            x,
            y,
            text_color,
            border_color,
            glyph_ptr,
            glyph_size,
            glyph_width,
            glyph_height,
            font_attr,
            false);
    } else if ((!(gnCurrentFontAttrib & FONTATTRIB_BOLD)) || bordered == 3) {
        gdi_show_char(x, y, text_color, glyph_ptr, glyph_size, glyph_width, glyph_height, font_attr, false);
    } else {
        gdi_show_char(x + 1, y, text_color, glyph_ptr, glyph_size, glyph_width, glyph_height, font_attr, false);
        gdi_show_char(x, y, text_color, glyph_ptr, glyph_size, glyph_width, glyph_height, font_attr, false);
    }
}

/*****************************************************************************
 * FUNCTION
 *  GetLangIndex
 * DESCRIPTION
 *  To get the language index of the character.
 * PARAMETERS
 *  pCount          [OUT]       The langugae Index
 *  pnFontIndex     [OUT]
 *  pnIndex         [OUT]       The offset of the character in the array
 *  nCh             [IN]        The character to look for
 *  pnBase          [OUT]       Base of Character set
 * RETURNS
 *  void
 *****************************************************************************/
static void GetLangIndex(uint8_t *pCount, int32_t *pnFontIndex, int32_t *pnIndex, uint32_t nCh, uint32_t *pnBase)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /* first check the index in the current language. 90% of the cases strings will be from current language */
    uint16_t nFontCount = 0;
    const font_group_struct *font_group = &(g_fontfamily[gnCurrentFont]);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *pCount = 0;

    nFontCount = 0;
    {

        while (nFontCount < font_group->nTotalFonts) {
            //printf("GetLangIndex, min %x, max %x\n", font_group->fontData[nFontCount]->pRangeDetails->pRangeData[0].nMin, font_group->fontData[nFontCount]->pRangeDetails->pRangeData[font_group->fontData[nFontCount]->pRangeDetails->nNoOfRanges - 1].nMax);
            if (nCh >= font_group->fontData[nFontCount]->pRangeDetails->pRangeData[0].nMin &&
                    nCh <=
                    font_group->fontData[nFontCount]->pRangeDetails->pRangeData[font_group->fontData[nFontCount]->pRangeDetails->nNoOfRanges - 1].nMax) {
                /* pnIndex is index of range block, pnBase is index of character which is start of range block */
                *pnIndex = SearchIndexinList(*pCount, nFontCount, nCh, pnBase);
                if (*pnIndex == -1) {
                    ++nFontCount;
                } else {
                    gpCurrentFont = font_group->fontData[nFontCount];
                    *pnFontIndex = nFontCount;
                    return;
                }

            }   /* if(nCh>=pFontFamily->fontData[nFontCount].pRangeData[0].nMin && */
            else {
                ++nFontCount;
            }
        }   /* while(nFontCount < pFontFamily->nTotalFonts) */
    }

    if (*pnIndex == -1) {
    }
    return;
}


/*****************************************************************************
 * FUNCTION
 *  SearchIndexinList
 * DESCRIPTION
 *  To search index and base of the character in a language.
 *
 *  This function applies binary search assuming that the RangeList
 *  array is sorted ina scending order and no ranges overlap.
 * PARAMETERS
 *  nListIndex      [IN]
 *  nFontCount      [IN]
 *  nCh             [IN]
 *  pnBase          [OUT]       Returns the base
 * RETURNS
 *  int32_t - The offset of the character in the Range table
 *****************************************************************************/
static int32_t SearchIndexinList(uint8_t nListIndex, uint16_t nFontCount, uint32_t nCh, uint32_t *pnBase)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t nFirst = 0;
    int32_t nLast = g_fontfamily[gnCurrentFont].fontData[nFontCount]->pRangeDetails->nNoOfRanges - 1;
    const RangeData *pRangeData = g_fontfamily[gnCurrentFont].fontData[nFontCount]->pRangeDetails->pRangeData;

    uint16_t *pRange = g_fontfamily[gnCurrentFont].fontData[nFontCount]->pRange;
    int16_t nMid;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //printf("SearchIndexinList, nFontCount %d, nCh %x, nLast %d, gnCurrentFont %d\n", nFontCount, nCh, nLast, gnCurrentFont);
    while (nLast >= nFirst) {
        nMid = (nFirst + nLast) / 2;
        if ((nCh >= pRangeData[nMid].nMin) && (nCh <= pRangeData[nMid].nMax)) {
            *pnBase = pRangeData[nMid].nMin;
            return pRange[nMid];

        }
        if (nCh > pRangeData[nMid].nMin) {
            nFirst = nMid + 1;
        } else {
            nLast = nMid - 1;
        }

    }
    //printf("SearchIndexinList, can't found\n");
    return -1;
}
/*****************************************************************************
 * FUNCTION
 *  SearchInPropFont
 * DESCRIPTION
 *  To search a character in the properitory font
 *
 *  This function applies binary search assuming that the RangeList
 *  array is sorted ina scending order and no ranges overlap.
 * PARAMETERS
 *  nCh         [IN]
 *  pnBase      [OUT]       Returns the base
 * RETURNS
 *  int32_t - The offset of the character in the Range table
 *****************************************************************************/
static int32_t SearchInPropFont(uint32_t nCh, uint32_t *pnBase)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int16_t nFirst = 0;
    int16_t nLast = proprietaryfont_small.pRangeDetails->nNoOfRanges - 1;
    int16_t nMid;

    pAMTKProprietaryFont_RangeData = proprietaryfont_small.pRangeDetails->pRangeData;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //printf("SearchInPropFont, nCh %x, nLast %d\n", nCh, nLast);
    while (nLast >= nFirst) {
        nMid = (nFirst + nLast) / 2;;
        if ((nCh >= pAMTKProprietaryFont_RangeData[nMid].nMin) && (nCh <= pAMTKProprietaryFont_RangeData[nMid].nMax)) {
            *pnBase = pAMTKProprietaryFont_RangeData[nMid].nMin;
            return proprietaryfont_small.pRange[nMid];

        }
        if (nCh > pAMTKProprietaryFont_RangeData[nMid].nMin) {
            nFirst = nMid + 1;
        } else {
            nLast = nMid - 1;
        }

    }
    //printf("SearchInPropFont, can't found\n");
    return -1;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_font_engine_get_glyph_metrics
 * DESCRIPTION
 *  This is used to get the glyph metrics for a sepcified character.
 *  The font metrics is considered with font attribute.
 *  The caller to use this API should get GDI lock first for thread safe in font engine.
 * PARAMETERS
 *  glyph              [IN]        The ucs2 code for the character
 *  metrics         [OUT]       The metrics for the ch
 * RETURNS
 *  bool        [OUT] to indicate if the character is valid or not.
 *  If it is invalid character, the metrics info is the info for nil character.
 *****************************************************************************/
bool gdi_font_engine_get_glyph_metrics(gdi_font_engine_glyph_metrics_t *metrics, uint16_t glyph)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t *pCharData;
    uint32_t data_attr;
    int32_t pnWidth;
    int32_t advance_x;
    uint16_t glyph_width;
    uint16_t glyph_height;
    int32_t bearing_x;
    bool valid;
    int32_t ascent, descent;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_LOCK;
    FE_LOCK;

    mmi_fe_get_font_data(
        false,
        glyph,
        &pCharData,
        &data_attr,
        &pnWidth,
        &advance_x,
        &glyph_width,
        &glyph_height,
        &bearing_x,
        &ascent,
        &descent,
        &valid);
    metrics->advance_x = (int16_t) advance_x;
    metrics->width = glyph_width;
    metrics->height = glyph_height;
    metrics->descent = (int16_t) descent;
    metrics->ascent = (int16_t) ascent;
    /* enlarge the font width for italic or qblique if it is bitmap font. */

    if ((gnCurrentFontAttrib & (FONTATTRIB_ITALIC | FONTATTRIB_OBLIQUE)) && data_attr == 0) {
        metrics->width += (font_tilt_table[metrics->height]);
    }
    if ((gnCurrentFontAttrib & (FONTATTRIB_BOLD)) && data_attr == 0) {
        metrics->width += 1;
    }


    /* add one pixel height for the underline */
    if (gnCurrentFontAttrib & FONTATTRIB_UNDERLINE) {
        metrics->descent++;
    }
    FE_UNLOCK;
    GDI_UNLOCK;
    return valid;

}


/*****************************************************************************
 * FUNCTION
 *  mmi_fe_init_param_int
 * DESCRIPTION
 *  To init gdi_font_engine_display_string_info_t
 * PARAMETERS
 *  param       [IN]        The point of gdi_font_engine_display_string_info_t need to init
 *  string        [IN]        The string need to init
 *  nLen          [IN]       The len of string need init
 * RETURNS
 * void
 *****************************************************************************/
void mmi_fe_init_param_int(mmi_fe_showstring_int_param_struct_p param, int32_t x, int32_t y, uint8_t *string, int32_t nLen)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t clip_x1, clip_x2, clip_y1, clip_y2;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memset(param, 0, sizeof(mmi_fe_showstring_int_param_struct));

    gdi_layer_get_clip(&clip_x1, &clip_y1, &clip_x2, &clip_y2);

    param->x = x;
    param->y = y;
    param->string = string;
    param->baseline_height = - 1;
    //param.bordered = false;
    param->length = nLen;
    param->visible_x_start = -1;
    param->visible_x_end = -1;
    param->clip_x1 = clip_x1;
    param->clip_x2 = clip_x2;
    param->clip_y1 = clip_y1;
    param->clip_y2 = clip_y2;
}


/*****************************************************************************
 * FUNCTION
 *  app_ucs2_strlen
 * DESCRIPTION
 *  The function is used for query UCS2 string length in character not in bytes.
  * PARAMETERS
 *  arrOut    [IN]  UCS2 string
 * RETURNS
 *  Return UCS2 string length in character not in bytes
 *****************************************************************************/
int32_t mmi_fe_ucs2_strlen(const int8_t *arrOut)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    int32_t nCount = 0;
    int32_t nLength = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Check for NULL character only at the odd no. of bytes
       assuming forst byte start from zero */
    if (arrOut) {
        while (arrOut[nCount] != 0 || arrOut[nCount + 1] != 0) {
            ++nLength;
            nCount += 2;
        }
    }
    return nLength; /* One is added to count 0th byte */
}

/*****************************************************************************
 * FUNCTION
 *  mmi_fe_init_param
 * DESCRIPTION
 *  To init gdi_font_engine_display_string_info_t
 * PARAMETERS
 *  param       [IN]        The point of gdi_font_engine_display_string_info_t need to init
 *  string        [IN]        The string need to init
 * RETURNS
 * void
 *****************************************************************************/
void mmi_fe_init_param(mmi_fe_showstring_int_param_struct_p param, int32_t x, int32_t y, uint8_t *string)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t nLen = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    nLen = mmi_fe_ucs2_strlen((const int8_t *)string);
    mmi_fe_init_param_int(param, x, y, string, nLen);

}


/*****************************************************************************
 * FUNCTION
 *  mmi_fe_init_color
 * DESCRIPTION
 *  To init gdi color
 * PARAMETERS
 *  border_color       [IN]        The point of border color need to init
 *  text_color           [IN]        The point of text color need to init
 * RETURNS
 * void
 *****************************************************************************/
void mmi_fe_init_color(gdi_color_t *border_color, gdi_color_t *text_color)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/


    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //printf("mmi_fe_init_color\n");
    if (border_color != NULL) {

        *border_color = gdi_act_color_from_rgb(
                            0xff,
                            g_mmi_fe_text_border_color.red,
                            g_mmi_fe_text_border_color.green,
                            g_mmi_fe_text_border_color.blue);
    }

    *text_color = gdi_act_color_from_rgb(0xff, g_mmi_fe_text_color.red, g_mmi_fe_text_color.green, g_mmi_fe_text_color.blue);

}


/*****************************************************************************
 * FUNCTION
 *  gdi_font_engine_display_string
 * DESCRIPTION
 *  To display string in the screen
 * PARAMETERS
 *  parameter       [IN]        The input parameter for string display
 * RETURNS
 *  UNIT32  how many character is diaplyed.
 *****************************************************************************/
uint32_t gdi_font_engine_display_string(gdi_font_engine_display_string_info_t *parameter)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    mmi_fe_showstring_int_param_struct param_in;
    uint32_t ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parameter->length <= 0 || parameter->string == NULL) {
        return 0;
    }

    mmi_fe_init_param_int(&param_in, parameter->x, parameter->y, parameter->string, parameter->length);

    param_in.baseline_height = parameter->baseline_height;
    param_in.bordered = 0;
    //printf("gdi_font_engine_display_string, %s, len %d, x %d, y %d\n", param_in.string, param_in.length, param_in.x, param_in.y);
    ret = mmi_fe_show_string_int(&param_in);
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  mmi_fe_show_single_cluster_other
 * DESCRIPTION
 *  To display one cluster for common language
 * PARAMETERS
 *  param               [IN]
 *  text_color          [IN]
 *  border_color        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void mmi_fe_show_single_cluster_other(
    mmi_fe_show_one_cluster_param_struct_p param,
    gdi_color_t text_color,
    gdi_color_t border_color)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint16_t curch = 0;
    int32_t i;
    uint8_t *CharData;
    uint32_t CharDataAttr;
    uint32_t NumChar;
    int32_t nWidth = 0;
    int32_t bearing_x;
    int32_t advance_x;
    uint16_t glyph_width = 0;
    uint16_t glyph_height = 0;
    int32_t ascent = 0;
    int32_t descent = 0;
    bool bordered = (bool)param->bordered;
    bool valid;
    int32_t curX = param->x;
    int32_t y_offset;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //printf("mmi_fe_show_single_cluster_other\n");
    for (i = 0; i < param->len; i++) {
        curch = param->string[(i << 1)] | (param->string[(i << 1) + 1] << 8);

        NumChar = mmi_fe_get_font_data(
                      MMI_FE_GET_DATA,
                      curch,
                      &CharData,
                      &CharDataAttr,
                      &nWidth,
                      &advance_x,
                      &glyph_width,
                      &glyph_height,
                      &bearing_x,
                      &ascent,
                      &descent,
                      &valid);
        //printf("mmi_fe_show_single_cluster_other, NumChar %d\n", NumChar);
        y_offset = param->y + (param->baseline - ascent);
        if (i == 0 && bearing_x < 0) {
            bearing_x = 0;
        }
        if (NumChar)
            //printf("mmi_fe_show_single_cluster_other, before show font data\n");
            mmi_fe_show_font_data(
                curch,
                curX + bearing_x,
                y_offset,
                CharData,
                NumChar,
                CharDataAttr,
                glyph_width,
                glyph_height,
                bordered,
                gnCurrentFontAttrib,
                text_color,
                border_color);
            //printf("mmi_fe_show_single_cluster_other, after show font data\n");
        curX += advance_x;

    }
}

/*****************************************************************************
 * FUNCTION
 *  mmi_fe_show_string_int_core
 * DESCRIPTION
 *  To display a string in the screen
 * PARAMETERS
 *  param       [IN]
 * RETURNS
 *  UNIT32
 *****************************************************************************/
static uint32_t mmi_fe_show_string_int_core(mmi_fe_showstring_int_param_struct_p param)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t len = param->length;
    int32_t curX = 0;

    uint8_t *string = param->string;
    int32_t x_offset = 0;
    gdi_color_t border_color = 0, text_color;
    uint8_t tempFontAttrib = gnCurrentFontAttrib;
    bool drawTwice = false;
    int32_t max_descent = 0, temp_descent;
    bool border;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    FE_SWLA_START("FES");
    //printf("mmi_fe_show_string_int_core, %s, len %d\n", string, len);

    GDI_LOCK;
    FE_LOCK;

    if (g_mmi_fe_text_border_color.red == TRANSPARENT_COLOR_R &&
            g_mmi_fe_text_border_color.green == TRANSPARENT_COLOR_G && g_mmi_fe_text_border_color.blue == TRANSPARENT_COLOR_B) {
        if (param->bordered) {
            param->bordered = 0;
            //font_attr_change = true;
        }
    }
    border = (bool)param->bordered;
    mmi_fe_init_color(&border_color, &text_color);

    gnCurrentFontAttrib &= (~(FONTATTRIB_UNDERLINE | FONTATTRIB_STRIKETHROUGH));

    /* initialize */
    param->text_color = text_color;
    param->border_color = border_color;

    if (len < 0) {
        len = mmi_fe_ucs2_strlen((const int8_t *)string);
    }

    {
        if (len > MAX_SHOW_STRING_TEXT_LENGTH - ENCODING_LENGTH) {
            len = MAX_SHOW_STRING_TEXT_LENGTH - ENCODING_LENGTH;
        }

        curX = param->x;
        {
            gdi_font_engine_string_info_t query;
            memset(&query, 0, sizeof(gdi_font_engine_string_info_t));

            query.string = string;
            query.count = len;

            gdi_font_engine_get_string_information(&query);

            if (param->baseline_height == - 1) {
                param->baseline_height = query.baseline;
            }
        }

        /* the data passed from editor has been processed by BIDI and shaping. Hence, go to process. */
        if (cancelWidth) {
            goto MMI_FE_SHOW_STRING;
        }

MMI_FE_SHOW_STRING:
        gdi_font_begin();


FE_SHOW_STRING_START:
        //printf("mmi_fe_show_string_int_core FE_SHOW_STRING_START\n");
        /* change param->x to real start x */
        param->x = curX;
        {
            mmi_lm_cluster_info cluster, prev_cluster, prev2_cluster;
            uint8_t *str_end;
            uint8_t *str_start;
            bool show = true /*, show_prev = false*/;
            mmi_lm_show_cluster_param show_param;
            int32_t x_real_offset = 0;
            //uint16_t curr_ucs_index;

            str_start = string;
            str_end = string + len * 2;
            mmi_lm_cluster_init(&prev_cluster);
            mmi_lm_cluster_init(&prev2_cluster);
            mmi_lm_cluster_init(&cluster);

            show_param.cluster = &cluster;
            {
                x_offset = curX;
                show_param.baseline = param->baseline_height;
            }
            show_param.y = param->y;
            show_param.bordered = param->bordered;
            show_param.prev_cluster = &prev_cluster;
            if (!(param->visible_x1_start == -1 && param->visible_x1_end == -1)) {

                x_real_offset = curX;
            }

            do {
                //printf("mmi_fe_show_string_int_core while loop\n");

                /* variable initialize */
                show = true;

                {

                    memcpy(&prev2_cluster, &prev_cluster, sizeof(mmi_lm_cluster_info));
                    memcpy(&prev_cluster, &cluster, sizeof(mmi_lm_cluster_info));
                    mmi_lm_cluster_init(&cluster);

                    cluster.data_ptr = str_start;
                    mmi_lm_get_current_cluster(cluster.data_ptr, str_end, &prev2_cluster, &prev_cluster, &cluster);
                }

                temp_descent = cluster.h - cluster.by;
                if (temp_descent > max_descent) {
                    max_descent = temp_descent;
                }

                if (
                    (cluster.lang_id != MMI_LM_LANG_OTHERS)) {
                    drawTwice = true;
                }

                if (param->visible_x1_start == -1 && param->visible_x1_end == -1) {
                    if ((x_offset > param->clip_x2 + 1)) {
                        break;
                    }
                }
                x_offset += cluster.rx;
                x_real_offset += cluster.rx;

                /* the end of the cluster is in the front of the clip x. */
                if (
                    (x_offset + cluster.w < param->clip_x1)) {
                    show = false;
                }

                if (!(param->visible_x1_start == -1 && param->visible_x1_end == -1)) {
                    show_param.x = x_real_offset;
                } else {
                    show_param.x = x_offset;
                }

                if (show) {
                    mmi_lm_draw_cluster(&show_param);
                    x_real_offset += cluster.adv_x;
                }
                str_start += cluster.data_len * 2;

                if (str_start == str_end) { //for underline style, if last cluster, then add the width of cluster
                    x_offset += cluster.w;
                } else {
                    x_offset += cluster.adv_x;
                }

                //show_prev = show;
            } while (str_start < str_end);
        }
    }

    if (param->bordered) {
        if (drawTwice) {
            /* draw the data inside again */

            gdi_font_end();
            gdi_font_begin();
            param->bordered = 0;

            goto FE_SHOW_STRING_START;
        }
    }
    param->bordered = border;
    gnCurrentFontAttrib = tempFontAttrib;

    if (gnCurrentFontAttrib & FONTATTRIB_UNDERLINE) {
        int32_t height_offset;

        if (param->max_descent) {
            height_offset = param->y + (param->baseline_height + param->max_descent);
        } else {
            height_offset = param->y + (param->baseline_height + max_descent);
        }

        gdi_draw_line(param->x, height_offset, x_offset - 1, height_offset, text_color);
    }
    
    if (gnCurrentFontAttrib & FONTATTRIB_STRIKETHROUGH) {
        int32_t height_offset = param->y + ((param->baseline_height + max_descent) >> 1);

        gdi_draw_line(param->x, height_offset, x_offset - 1, height_offset, text_color);
    }

    gdi_font_end();

    FE_UNLOCK;
    GDI_UNLOCK;

    FE_SWLA_STOP("FES");
    return x_offset;
}



/*****************************************************************************
 * FUNCTION
 *  mmi_fe_show_string_int
 * DESCRIPTION
 *
 * PARAMETERS
 *  param       [IN]
 * RETURNS
 *
 *****************************************************************************/
static uint32_t mmi_fe_show_string_int(mmi_fe_showstring_int_param_struct_p param)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    mmi_fe_show_string_int_core(param);

    return 0;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_font_engine_get_string_information
 * DESCRIPTION
 *  To get string width height
 * PARAMETERS
 *  query       [IN/OUT]        The query parameter to indicate the input and output
 * RETURNS
 *  uint32_t   how many characters is counted.
 *****************************************************************************/
uint32_t gdi_font_engine_get_string_information(gdi_font_engine_string_info_t *query)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t default_direction;
    int32_t return_value;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return_value = mmi_fe_get_string_widthheight_int(query, &default_direction, NULL, NULL, true);
    return return_value;

}

/*****************************************************************************
 * FUNCTION
 *  mmi_fe_get_string_widthheight_int
 * DESCRIPTION
 *  To get string width height with callback to save all cluster information
 * PARAMETERS
 *  query           [IN/OUT]        The query parameter to indicate the input and output
 *  direction       [OUT]           string direction
 *  f_save_info     [IN]            Save cluster information function
 *  user_data       [IN]            user data
 *  is_BIDI         [IN]            need BIDI or not
 * RETURNS
 *  uint32_t   how many characters is counted.
 *****************************************************************************/
uint32_t mmi_fe_get_string_widthheight_int(
    gdi_font_engine_string_info_t *query,
    uint8_t *direction,
    fe_save_cluster_info_cb f_save_info,
    void *user_data,
    bool is_BIDI)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t numofChar = 0;
    uint32_t shapingNumOfChar = 0;
    int32_t ascent = 0, descent = 0;
    uint8_t *string = query->string;
    int32_t nLen;
    gdi_font_engine_string_info_t string_part_param;

    uint8_t default_direction = 1;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* initialization */
    //printf("mmi_fe_get_string_widthheight_int, %s, %d\n", string, query->count);
    query->height = 0;
    query->width = 0;
    query->baseline = 0;
    nLen = query->count;

    /* error check */
    if (string == NULL || nLen <= 0) {
        return numofChar;
    }
    FE_SWLA_START("FEQ");
    GDI_LOCK;
    FE_LOCK;


    if (nLen > MAX_SHOW_STRING_TEXT_LENGTH - ENCODING_LENGTH) {
        nLen = MAX_SHOW_STRING_TEXT_LENGTH - ENCODING_LENGTH;
    }

    memcpy(&string_part_param, query, sizeof(gdi_font_engine_string_info_t));

    {
        mmi_lm_cluster_info cluster, prev_cluster, prev2_cluster;
        uint8_t *str_end;
        uint8_t *str_start;
        int32_t width = 0, adv_w = 0;
        int32_t chDescent = 0;

        str_start = string;
        str_end = string + nLen * 2;
        mmi_lm_cluster_init(&prev_cluster);
        mmi_lm_cluster_init(&prev2_cluster);
        mmi_lm_cluster_init(&cluster);
        do {
            //printf("mmi_fe_get_string_widthheight_int while loop\n");
            str_start += cluster.data_len * 2;

            /* variable initialize */
            memcpy(&prev2_cluster, &prev_cluster, sizeof(mmi_lm_cluster_info));
            memcpy(&prev_cluster, &cluster, sizeof(mmi_lm_cluster_info));
            mmi_lm_cluster_init(&cluster);

            cluster.data_ptr = str_start;
            mmi_lm_get_current_cluster(cluster.data_ptr, str_end, &prev2_cluster, &prev_cluster, &cluster);

            {
                uint16_t curCh;
                curCh = ((cluster.data_ptr[0]) | (cluster.data_ptr[1] << 8));
                if ((FONT_TEST_NEW_LINE_CHAR(curCh)) && ((numofChar + cluster.data_len) >= nLen)) {
                    goto GET_STRING_WIDTHHEIGH;
                }
            }
            if ((int32_t) cluster.by > ascent) {
                ascent = (int32_t) cluster.by;
            }

            chDescent = (int32_t) (cluster.h - cluster.by);
            if (chDescent > descent) {
                descent = chDescent;
            }

            /* Get cluster info */
            if (f_save_info) {
                //printf("mmi_fe_get_string_widthheight_int, f_save_info\n");
                mmi_fe_cluster_metrics_struct metric;

                memset(&metric, 0, sizeof(mmi_fe_cluster_metrics_struct));
                metric.index = numofChar;
                metric.len = (uint8_t) (cluster.data_len);

                /* Convert the visiual index to logical index for BIDI and Shapping */

                metric.x = adv_w;
                metric.width = (uint8_t) (cluster.w);
                metric.adv_w = (uint8_t) (cluster.adv_x);

                f_save_info(metric, user_data);

                shapingNumOfChar += metric.len;
            } else {
                shapingNumOfChar += cluster.data_len;
            }
            width = adv_w + cluster.w + cluster.rx;
            adv_w += cluster.adv_x + cluster.rx;

            numofChar += cluster.data_len;

        } while (numofChar < nLen);
GET_STRING_WIDTHHEIGH:
        if (!width && adv_w) {
            width = adv_w;
        }
        query->advance_width = adv_w;
        query->advance_height = ascent + descent;
        {
            query->width = width;
        }
        query->baseline = ascent;
        query->max_ascent = ascent;
        query->max_descent = descent;
        query->height = ascent + descent;
    }
    /* because the underline is drawn in the bottom of the string. */
    if ((gnCurrentFontAttrib & FONTATTRIB_UNDERLINE)) {
        query->height++;
    }

    FE_UNLOCK;
    GDI_UNLOCK;

    if (direction) {
        *direction = default_direction;
    }
    //printf("mmi_fe_get_string_widthheight_int, end\n");
    FE_SWLA_STOP("FEQ");
    return numofChar;
}


/*****************************************************************************
 * FUNCTION
 *  mmi_fe_query_single_cluster
 * DESCRIPTION
 *  To query one cluster.
 * PARAMETERS
 *  prev_cluster        [IN]
 *  cluster             [IN]
 *  param(?)            [IN]        The parameter for one cluster
 * RETURNS
 *  void
 *****************************************************************************/
void mmi_fe_query_single_cluster(mmi_lm_cluster_info_p prev_cluster, mmi_lm_cluster_info_p cluster)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    FE_SWLA_START("QCL");
    GDI_LOCK;
    FE_LOCK;
    //printf("mmi_fe_query_single_cluster\n");

    switch (cluster->lang_id) {
        default: {
            /*----------------------------------------------------------------*/
            /* Local Variables                                                */
            /*----------------------------------------------------------------*/
            int32_t i = 0;
            uint16_t ch;
            int16_t temp_ascent = 0;
            int16_t temp_descent = 0;
            gdi_font_engine_glyph_metrics_t matrix;
            int16_t temp_height = 0;
            int16_t x = 0;

            /*----------------------------------------------------------------*/
            /* Code Body                                                      */
            /*----------------------------------------------------------------*/
            memset(&matrix, 0, sizeof(matrix));
            for (; i < cluster->cluster_len; i++) {
                ch = cluster->display_buffer[i];
                if (i != 0) {
                    cluster->w += matrix.advance_x;
                }
                gdi_font_engine_get_glyph_metrics(&matrix, ch);
                cluster->adv_x += matrix.advance_x;
                if (matrix.ascent > temp_ascent) {
                    temp_ascent = matrix.ascent;
                }
                if (matrix.descent > temp_descent) {
                    temp_descent = matrix.descent;
                }
                if (temp_height < matrix.height) {
                    temp_height = matrix.height;
                }
                x += matrix.advance_x;
            }
            if (cluster->cluster_len == 1 && cluster->adv_x == 0) {
                cluster->adv_x += matrix.width;
            }

            cluster->w += matrix.width;
            cluster->h = temp_ascent + temp_descent;
            if (cluster->h < temp_height) {
                cluster->h = temp_height;
            }
            cluster->by = temp_ascent;
        }

        break;
    }

    FE_UNLOCK;
    GDI_UNLOCK;
    FE_SWLA_STOP("QCL");
}


/*****************************************************************************
 * FUNCTION
 *  mmi_fe_show_single_cluster_core
 * DESCRIPTION
 *  To display one cluster. The content should be in its display form.
 * PARAMETERS
 *  param       [IN]        The parameter for one cluster
 * RETURNS
 *  void
 *****************************************************************************/
void mmi_fe_show_single_cluster_core(mmi_fe_show_one_cluster_param_struct_p param)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_color_t border_color = 0, text_color;
    mmi_fe_showstring_int_param_struct show_param;
    uint8_t tempFontAttrib = gnCurrentFontAttrib;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    FE_SWLA_START("SCL");
    GDI_LOCK;
    FE_LOCK;
    //printf("mmi_fe_show_single_cluster_core\n");
    memset(&show_param, 0, sizeof(mmi_fe_showstring_int_param_struct));
    mmi_fe_init_color(&border_color, &text_color);
    gnCurrentFontAttrib &= (~(FONTATTRIB_UNDERLINE | FONTATTRIB_STRIKETHROUGH));


    gdi_font_begin();

    if (param->bordered &&
            g_mmi_fe_text_border_color.red == TRANSPARENT_COLOR_R &&
            g_mmi_fe_text_border_color.green == TRANSPARENT_COLOR_G && g_mmi_fe_text_border_color.blue == TRANSPARENT_COLOR_B) {
        param->bordered = 0;
    }


    switch (param->langid) {
        default: {
            mmi_fe_show_single_cluster_other(param, text_color, border_color);
        }
        break;
    }

    gnCurrentFontAttrib = tempFontAttrib;

    gdi_font_end();

    FE_UNLOCK;
    GDI_UNLOCK;
    FE_SWLA_STOP("SCL");
}


/*****************************************************************************
 * FUNCTION
 *  mmi_fe_show_single_cluster
 * DESCRIPTION
 *
 * PARAMETERS
 *  param       [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void mmi_fe_show_single_cluster(mmi_fe_show_one_cluster_param_struct_p param)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    mmi_fe_show_single_cluster_core(param);
}


/*****************************************************************************
* FUNCTION
*  mmi_lang_support
* DESCRIPTION
*  To get the current lcc code.
* PARAMETERS
*  ssc            [in]
* RETURNS
*
*****************************************************************************/
int8_t mmi_lang_support(const int8_t *ssc)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (ssc) {
        for (i = 0; i < mtk_gMaxDeployedLangs; i++) {
            if (!strcmp((char *)mtk_gLanguageArray[i].ssc, (char *)ssc)) {
                return i;
            }
        }
    }
    return -1;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_font_engine_get_language_list
 * DESCRIPTION
 *  Get the current language list.
 * PARAMETERS
 *  [OUT] language_list: the pointer to the array mtk_gLanguageArray
 *  [OUT] language_number: the number of supported languages in this project.
 * RETURNS
 *  void.
 *****************************************************************************/
void gdi_font_engine_get_language_list(const gdi_font_engine_language_t **language_list, uint8_t *language_number)
{
    *language_list = mtk_gLanguageArray;
    *language_number = mtk_gMaxDeployedLangs;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_font_engine_set_current_language_by_index
 * DESCRIPTION
 *  Set the current language.
 * PARAMETERS
 *  [IN] language_index: index of the array mtk_gLanguageArray
 * RETURNS
 *  gdi_font_engine_status_t.
 *****************************************************************************/
static gdi_font_engine_status_t gdi_font_engine_set_current_language_by_index(int8_t language_index)
{
    if (language_index < -1 || language_index > mtk_gMaxDeployedLangs) {
        return GDI_FONT_ENGINE_ERROR_LANGUAGE_NOT_SUPPORTED;
    }
    gCurrLangIndex = (uint16_t)language_index;
    return GDI_FONT_ENGINE_OK;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_font_engine_get_current_language_index
 * DESCRIPTION
 *  Get the current language.
 * PARAMETERS
 *  [OUT] language_index: index of the array mtk_gLanguageArray
 * RETURNS
 *  void.
 *****************************************************************************/
void gdi_font_engine_get_current_language_index(uint8_t *language_index)
{
    *language_index = gCurrLangIndex;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_font_engine_set_current_language_by_ssc
 * DESCRIPTION
 *  Set the current language.
 * PARAMETERS
 *  [IN] language_ssc: ssc string
 * RETURNS
 *  gdi_font_engine_status_t.
 *****************************************************************************/
gdi_font_engine_status_t gdi_font_engine_set_current_language_by_ssc(const int8_t *language_ssc)
{
    return gdi_font_engine_set_current_language_by_index(mmi_lang_support(language_ssc));
}

/*****************************************************************************
 * FUNCTION
 *  gdi_font_set_current_language
 * DESCRIPTION
 *  Set the current language.
 * PARAMETERS
 *  [OUT] language_ssc: ssc string buffer which must at least have the size of GDI_FONT_ENGINE_SSC_SIZE.
 * RETURNS
 *  void.
 *****************************************************************************/
void gdi_font_engine_get_current_language_ssc(int8_t *language_ssc)
{
    strncpy((char *)language_ssc, (char *)mtk_gLanguageArray[gCurrLangIndex].ssc, GDI_FONT_ENGINE_SSC_SIZE);
}
extern uint8_t *gdi_resource_get_string_data(uint16_t string_id);
/**
 * @brief     This function is used to get string raw data of current language by the string id.
 * @param[in] string_id    is the string id which add in the resource generator tool flow.
 * @return    Return the string raw data of current language.
 */
uint8_t *gdi_font_engine_get_string_data(uint16_t string_id)
{
    return gdi_resource_get_string_data(string_id);
}

