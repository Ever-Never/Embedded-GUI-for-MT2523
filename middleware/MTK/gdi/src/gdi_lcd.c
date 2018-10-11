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

#include "gdi_internal.h"
#include <string.h>
#include <stdio.h>
#include "gdi.h"
#include "gdi_datatype.h"
#include "gdi_primitive.h"
#include "gdi_const.h"
#include "gdi_lcd.h"
#include "gdi_features.h"
#include "gdi_layer.h"
//#include "mt25x3_hdk_lcd.h"
/****************************************************************************
* Local Variable
*****************************************************************************/
/* gdi_layer context */
#define GDI_LCD_SUM 1
gdi_lcd_cntx_struct *GDI_LCD;
gdi_lcd_cntx_struct GDI_LCD_ARRAY[GDI_LCD_SUM];
extern uint32_t GDI_LCD_WIDTH;
extern uint32_t GDI_LCD_HEIGHT;

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_init
 * DESCRIPTION
 *  Init lcd
 * PARAMETERS
 *  void
 * RETURNS
 *  gdi_result_t(?)
 *****************************************************************************/
void gdi_lcd_init(void)
{
    GDI_LOG("gdi_lcd_init\n");
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_lcd_init)

    /* init LCD */
    memset(GDI_LCD_ARRAY, 0, sizeof(GDI_LCD_ARRAY));
    GDI_LCD_ARRAY[0].cf = GDI_COLOR_FORMAT_16;
    GDI_LCD_ARRAY[0].act_handle = GDI_LCD_MAIN_LCD_HANDLE;
    GDI_LCD_ARRAY[0].act_width = GDI_LCD_WIDTH;
    GDI_LCD_ARRAY[0].act_height = GDI_LCD_HEIGHT;
    //GDI_LCD_ARRAY[0].=;
    GDI_LCD = &GDI_LCD_ARRAY[0];

    GDI_EXIT_CRITICAL_SECTION(gdi_lcd_init)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_get_dimension
 * DESCRIPTION
 *  get dimesion of current active lcd
 * PARAMETERS
 *  width       [OUT]       
 *  height      [OUT]       
 * RETURNS
 *  void(?)
 *****************************************************************************/
void gdi_lcd_get_dimension(uint32_t *width, uint32_t *height)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((GDI_LCD->rotate_value) & (0x01))
    {
        *width = GDI_LCD_HEIGHT;
        *height = GDI_LCD_WIDTH;
    }
    else
    {
        *width = GDI_LCD_WIDTH;
        *height = GDI_LCD_HEIGHT;
    }
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_get_bits_per_pixel
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
int gdi_lcd_get_bits_per_pixel(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int ret;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_lcd_get_bits_per_pixel)
    ret = gdi_bits_per_pixel(GDI_LCD->cf);
    GDI_EXIT_CRITICAL_SECTION(gdi_lcd_get_bits_per_pixel)
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_freeze
 * DESCRIPTION
 *  
 * PARAMETERS
 *  is_freeze       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_lcd_freeze(bool is_freeze)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_LCD->is_freeze = is_freeze;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_get_freeze
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void        
 * RETURNS
 *  The acrive LCD is freezed or not
 *****************************************************************************/
bool gdi_lcd_get_freeze(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bool ret;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ret = GDI_LCD->is_freeze;
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_set_high_priority_freeze
 * DESCRIPTION
 *  Sets the high priority freeze flag. This high priority flag can override normal lcd freeze.
 *  This API is reserved for framework usage. Do not use it.
 * PARAMETERS
 *  is_freeze       [IN]    freeze flag
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_lcd_set_high_priority_freeze(bool is_freeze)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_LCD->is_high_priority_freeze = is_freeze;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_get_high_priority_freeze
 * DESCRIPTION
 *  Gets the high priorty freeze flag.
 *  This API is reserved for framework usage. Do not use it.
 * PARAMETERS
 *  void        
 * RETURNS
 *  The active LCD is freezed or not.
 *****************************************************************************/
bool gdi_lcd_get_high_priority_freeze(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bool ret;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ret = GDI_LCD->is_high_priority_freeze;
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_set_rotate
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rotate_value        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_lcd_set_rotate(uint8_t rotate_value)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_lcd_set_rotate)
    GDI_LCD->rotate_value = rotate_value;
    GDI_EXIT_CRITICAL_SECTION(gdi_lcd_set_rotate)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_get_rotate
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
uint8_t gdi_lcd_get_rotate(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t ret;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_lcd_get_rotate)
    ret = GDI_LCD->rotate_value;
    GDI_EXIT_CRITICAL_SECTION(gdi_lcd_get_rotate)
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_get_blt_counter
 * DESCRIPTION
 *  return the blt counter.
 *  blt counter is the number that lcd update.
 *  UI engine use this to know their previous setting ever blt out or not.
 * PARAMETERS
 *  
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
gdi_result_t gdi_lcd_get_blt_counter(uint32_t *blt_counter)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* currently not to get gdi mutex for performance */
    if (blt_counter ==  NULL)
    {
        return GDI_FAILED;
    }
    
    *blt_counter = GDI_LCD->blt_counter;
    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_rotate_map_lcd_to_absolute_hw_region
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rotate_value        [IN]        
 *  x1                  [?]         
 *  y1                  [?]         
 *  x2                  [?]         
 *  y2                  [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_rotate_map_lcd_to_absolute_hw_region(uint8_t rotate_value, int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t tx1, ty1, tx2, ty2;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    switch (rotate_value)
    {
        case GDI_LCD_LAYER_ROTATE_0:
        default:
            break;
        case GDI_LCD_LAYER_ROTATE_90:
            tx1 = *y1;
            ty1 = GDI_LCD->act_height - *x2 - 1;
            tx2 = *y2;
            ty2 = GDI_LCD->act_height - *x1 - 1;
            *x1 = tx1;
            *y1 = ty1;
            *x2 = tx2;
            *y2 = ty2;
            break;
        case GDI_LCD_LAYER_ROTATE_180:
            tx1 = GDI_LCD->act_width - *x2 - 1;
            ty1 = GDI_LCD->act_height - *y2 - 1;
            tx2 = GDI_LCD->act_width - *x1 - 1;
            ty2 = GDI_LCD->act_height - *y1 - 1;
            *x1 = tx1;
            *y1 = ty1;
            *x2 = tx2;
            *y2 = ty2;
            break;
        case GDI_LCD_LAYER_ROTATE_270:
            tx1 = GDI_LCD->act_width - *y2 - 1;
            ty1 = *x1;
            tx2 = GDI_LCD->act_width - *y1 - 1;
            ty2 = *x2;
            *x1 = tx1;
            *y1 = ty1;
            *x2 = tx2;
            *y2 = ty2;
            break;
        case GDI_LCD_LAYER_ROTATE_0_MIRROR:
            tx1 = GDI_LCD->act_width - *x2 - 1;
            tx2 = GDI_LCD->act_width - *x1 - 1;
            *x1 = tx1;
            *x2 = tx2;
            break;
        case GDI_LCD_LAYER_ROTATE_90_MIRROR:
            tx1 = *y1;
            ty1 = *x1;
            tx2 = *y2;
            ty2 = *x2;
            *x1 = tx1;
            *y1 = ty1;
            *x2 = tx2;
            *y2 = ty2;
            break;
        case GDI_LCD_LAYER_ROTATE_180_MIRROR:
            ty1 = GDI_LCD->act_height - *y2 - 1;
            ty2 = GDI_LCD->act_height - *y1 - 1;
            *y1 = ty1;
            *y2 = ty2;
            break;
        case GDI_LCD_LAYER_ROTATE_270_MIRROR:
            tx1 = GDI_LCD->act_width - *y2 - 1;
            ty1 = GDI_LCD->act_height - *x2 - 1;
            tx2 = GDI_LCD->act_width - *y1 - 1;
            ty2 = GDI_LCD->act_height - *x1 - 1;
            *x1 = tx1;
            *y1 = ty1;
            *x2 = tx2;
            *y2 = ty2;
            break;
    }
}

/*****************************************************************************
 * FUNCTION
 *  gdi_rotate_map_absolute_hw_to_lcd_region
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rotate_value        [IN]        
 *  x1                  [?]         
 *  y1                  [?]         
 *  x2                  [?]         
 *  y2                  [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_rotate_map_absolute_hw_to_lcd_region(uint8_t rotate_value, int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t tx1, ty1, tx2, ty2;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (rotate_value)
    {
        case GDI_LCD_LAYER_ROTATE_0:
        default:
            break;
        case GDI_LCD_LAYER_ROTATE_90:
            tx1 = GDI_LCD->act_height - *y2 - 1;
            ty1 = *x1;
            tx2 = GDI_LCD->act_height - *y1 - 1;
            ty2 = *x2;
            *x1 = tx1;
            *y1 = ty1;
            *x2 = tx2;
            *y2 = ty2;
            break;
        case GDI_LCD_LAYER_ROTATE_180:
            tx1 = GDI_LCD->act_width - *x2 - 1;
            ty1 = GDI_LCD->act_height - *y2 - 1;
            tx2 = GDI_LCD->act_width - *x1 - 1;
            ty2 = GDI_LCD->act_height - *y1 - 1;
            *x1 = tx1;
            *y1 = ty1;
            *x2 = tx2;
            *y2 = ty2;
            break;
        case GDI_LCD_LAYER_ROTATE_270:
            tx1 = *y1;
            ty1 = GDI_LCD->act_width - *x2 - 1;
            tx2 = *y2;
            ty2 = GDI_LCD->act_width - *x1 - 1;
            *x1 = tx1;
            *y1 = ty1;
            *x2 = tx2;
            *y2 = ty2;
            break;
        case GDI_LCD_LAYER_ROTATE_0_MIRROR:
            tx1 = GDI_LCD->act_width - *x2 - 1;
            tx2 = GDI_LCD->act_width - *x1 - 1;
            *x1 = tx1;
            *x2 = tx2;
            break;
        case GDI_LCD_LAYER_ROTATE_90_MIRROR:
            break;
        case GDI_LCD_LAYER_ROTATE_180_MIRROR:
            ty1 = GDI_LCD->act_height - *y2 - 1;
            ty2 = GDI_LCD->act_height - *y1 - 1;
            *y1 = ty1;
            *y2 = ty2;
            break;
        case GDI_LCD_LAYER_ROTATE_270_MIRROR:
            tx1 = GDI_LCD->act_height - *x2 - 1;
            ty1 = GDI_LCD->act_width - *y2 - 1;
            tx2 = GDI_LCD->act_height - *x1 - 1;
            ty2 = GDI_LCD->act_width - *y1 - 1;
            *x1 = tx1;
            *y1 = ty1;
            *x2 = tx2;
            *y2 = ty2;
            break;
    }
}

/*****************************************************************************
 * FUNCTION
 *  gdi_rotate_map_absolute_hw_to_lcd
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x       [?]     
 *  y       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_rotate_map_absolute_hw_to_lcd(int32_t *x, int32_t *y)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_rotate_map_absolute_hw_to_lcd)
    int32_t tx, ty;

    switch (GDI_LCD->rotate_value)
    {
        case GDI_LCD_LAYER_ROTATE_0:
        default:
            break;
        case GDI_LCD_LAYER_ROTATE_90:
            tx = GDI_LCD->act_height - *y - 1;
            ty = *x;
            *x = tx;
            *y = ty;
            break;
        case GDI_LCD_LAYER_ROTATE_180:
            tx = GDI_LCD->act_width - *x - 1;
            ty = GDI_LCD->act_height - *y - 1;
            *x = tx;
            *y = ty;
            break;
        case GDI_LCD_LAYER_ROTATE_270:
            tx = *y;
            ty = GDI_LCD->act_width - *x - 1;
            *x = tx;
            *y = ty;
            break;
        case GDI_LCD_LAYER_ROTATE_0_MIRROR:
            tx = GDI_LCD->act_width - *x - 1;
            *x = tx;
            break;
        case GDI_LCD_LAYER_ROTATE_90_MIRROR:
            tx = *y;
            ty = *x;
            *x = tx;
            *y = ty;
            break;
        case GDI_LCD_LAYER_ROTATE_180_MIRROR:
            ty = GDI_LCD->act_height - *y - 1;
            *y = ty;
            break;
        case GDI_LCD_LAYER_ROTATE_270_MIRROR:
            tx = GDI_LCD->act_height - *y - 1;
            ty = GDI_LCD->act_width - *x - 1;
            *x = tx;
            *y = ty;
            break;
    }
    GDI_EXIT_CRITICAL_SECTION(gdi_rotate_map_absolute_hw_to_lcd)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_rotate_map_lcd_to_absolute_hw
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x       [?]     
 *  y       [?]     
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_rotate_map_lcd_to_absolute_hw(int32_t *x, int32_t *y)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_rotate_map_lcd_to_absolute_hw)
    int32_t tx, ty;

    switch (GDI_LCD->rotate_value)
    {
        case GDI_LCD_LAYER_ROTATE_0:
        default:
            break;
        case GDI_LCD_LAYER_ROTATE_90:
            tx = *y;
            ty = GDI_LCD->act_height - *x - 1;
            *x = tx;
            *y = ty;
            break;
        case GDI_LCD_LAYER_ROTATE_180:
            tx = GDI_LCD->act_width - *x - 1;
            ty = GDI_LCD->act_height - *y - 1;
            *x = tx;
            *y = ty;
            break;
        case GDI_LCD_LAYER_ROTATE_270:
            tx = GDI_LCD->act_width - *y - 1;
            ty = *x;
            *x = tx;
            *y = ty;
            break;
        case GDI_LCD_LAYER_ROTATE_0_MIRROR:
            tx = GDI_LCD->act_width - *x - 1;
            *x = tx;
            break;
        case GDI_LCD_LAYER_ROTATE_90_MIRROR:
            tx = *y;
            ty = *x;
            *x = tx;
            *y = ty;
            break;
        case GDI_LCD_LAYER_ROTATE_180_MIRROR:
            ty = GDI_LCD->act_height - *y - 1;
            *y = ty;
            break;
        case GDI_LCD_LAYER_ROTATE_270_MIRROR:
            tx = GDI_LCD->act_height - *y - 1;
            ty = GDI_LCD->act_width - *x - 1;
            *x = tx;
            *y = ty;
            break;
    }
    GDI_EXIT_CRITICAL_SECTION(gdi_rotate_map_lcd_to_absolute_hw)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_blt
 * DESCRIPTION
 *  
 * PARAMETERS
 *  handle0     [IN]        
 *  handle1     [IN]        
 *  handle2     [IN]        
 *  handle3     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_result_t gdi_lcd_blt(gdi_layer_handle_t handle0, gdi_layer_handle_t handle1, gdi_layer_handle_t handle2, gdi_layer_handle_t handle3)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t ret;
    uint32_t lcd_width, lcd_height;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_lcd_blt)

    gdi_lcd_get_dimension(&lcd_width, &lcd_height);
    GDI_LOG("gdi_lcd_blt w:%d h:%d\n", lcd_width, lcd_height);
    ret = gdi_layer_blt(handle0, handle1, handle2, handle3, 0, 0, lcd_width - 1, lcd_height - 1);
    GDI_EXIT_CRITICAL_SECTION(gdi_lcd_blt)
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_set_rotate_by_layer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rotate_by_layer     [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_lcd_set_rotate_by_layer(bool rotate_by_layer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* If is in scanline, not permit it to change rotate_by_layer value.
     * It should always be TRUE.
     */
    GDI_ENTER_CRITICAL_SECTION(gdi_lcd_set_rotate_by_layer)
    GDI_LCD->rotate_by_layer = rotate_by_layer;
    GDI_EXIT_CRITICAL_SECTION(gdi_lcd_set_rotate_by_layer)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_get_rotate_by_layer
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
bool gdi_lcd_get_rotate_by_layer(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if defined(GDI_USING_LCD_ROTATE)
    bool ret;
    GDI_ENTER_CRITICAL_SECTION(gdi_lcd_get_rotate_by_layer)
    ret = GDI_LCD->rotate_by_layer;
    GDI_EXIT_CRITICAL_SECTION(gdi_lcd_get_rotate_by_layer)
    return ret;
#else 
    return true;
#endif 
    
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_paint_ext
 * DESCRIPTION
 *  
 * PARAMETERS
 *  handle0     [IN]        
 *  handle1     [IN]        
 *  handle2     [IN]        
 *  handle3     [IN]               
 *  x1          [IN]        
 *  y1          [IN]        
 *  x2          [IN]        
 *  y2          [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_result_t gdi_lcd_paint_ext(gdi_layer_handle_t handle0, gdi_layer_handle_t handle1, gdi_layer_handle_t handle2, gdi_layer_handle_t handle3,
                               int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t result;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_lcd_paint_ext)
    result = gdi_layer_blt_ext(handle0, handle1, handle2, handle3,
                               x1, y1, x2, y2);
    GDI_EXIT_CRITICAL_SECTION(gdi_lcd_paint_ext)
    return result;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_repaint
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x1      [IN]        
 *  y1      [IN]        
 *  x2      [IN]        
 *  y2      [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_result_t gdi_lcd_repaint(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t ret;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_lcd_repaint_ext)
    ret = gdi_layer_blt_previous(x1, y1, x2, y2);
    GDI_EXIT_CRITICAL_SECTION(gdi_lcd_repaint_ext)
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_paint_all_ext
 * DESCRIPTION
 *  
 * PARAMETERS
 *  handle0     [IN]        
 *  handle1     [IN]        
 *  handle2     [IN]        
 *  handle3     [IN]       
 * RETURNS
 *  
 *****************************************************************************/
gdi_result_t gdi_lcd_paint_all_ext(gdi_layer_handle_t handle0, gdi_layer_handle_t handle1, gdi_layer_handle_t handle2, gdi_layer_handle_t handle3)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t lcd_width, lcd_height;
    gdi_result_t result;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_lcd_paint_all_ext)

    gdi_lcd_get_dimension(&lcd_width, &lcd_height);
    result = gdi_layer_blt_ext(handle0, handle1, handle2, handle3,
                               0, 0, lcd_width - 1, lcd_height - 1);
    GDI_EXIT_CRITICAL_SECTION(gdi_lcd_paint_all_ext)
    return result;
}


/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_repaint_all
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
gdi_result_t gdi_lcd_repaint_all(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t lcd_width, lcd_height;
    gdi_result_t ret;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_lcd_repaint_all_ext)
    gdi_lcd_get_dimension(&lcd_width, &lcd_height);
    ret = gdi_layer_blt_previous(0, 0, lcd_width - 1, lcd_height - 1);
    GDI_EXIT_CRITICAL_SECTION(gdi_lcd_repaint_all_ext)
    return ret;
}
