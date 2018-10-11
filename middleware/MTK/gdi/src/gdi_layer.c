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
#include "gdi_const.h"
#include "gdi_layer.h"
#include "gdi_primitive.h"
#include "gdi_features.h"
#include "gdi_datatype.h"
#include "gdi.h"
#include "gd_include.h"
#include "gdi_lcd.h"
#include <assert.h>
#include "hal_display_lcd.h"
#include "mt25x3_hdk_lcd.h"
#include "hal_g2d.h"

#define GDI_GRAY_BOUNDARY 210

#define GDI_TRACE(...)  
/****************************************************************************
* MACRO
*****************************************************************************/
#define LIMIT_BOUND(pos, min_value, max_value) \
   {                                         \
      if(pos < min_value)                      \
         pos = min_value;                      \
      else if(pos > max_value)                 \
         pos = max_value;                      \
   }

//warning 

#define LIMIT_BOUND_UNSIGNED(pos, max_value) \
		 {										 \
			if(pos > max_value)				 \
			   pos = max_value; 					 \
		 }
///////////////////////////////////////////
extern uint32_t GDI_LCD_WIDTH;
extern uint32_t GDI_LCD_HEIGHT;

static gdi_layer_handle_t gdi_layer_stack[GDI_LAYER_ACT_LAYER_STACK_COUNT];
static int32_t gdi_layer_stack_index;

gdi_layer_struct_t GDI_LAYERS[GDI_LAYER_TOTAL_LAYER_COUNT];
gdi_layer_struct_t *gdi_act_layer;
gd_color_from_rgb_func gdi_act_color_from_rgb;
gd_color_to_rgb_func gdi_act_color_to_rgb;
gd_put_pixel_func gdi_act_put_pixel;
gd_get_pixel_func gdi_act_get_pixel;
gd_replace_src_key_func gdi_act_replace_src_key;

gdi_lcd_layer_struct gdi_layer_info[GDI_LAYER_TOTAL_LAYER_COUNT];   /* this struct is used for output lcd */
gdi_lcd_layer_struct *gdi_act_layer_info;

#define GDI_ALPHA_BLENDING_SOURCE_LAYER_STACK_COUNT      (8)
static gdi_layer_handle_t gdi_alpha_blending_source_layer_stack[GDI_ALPHA_BLENDING_SOURCE_LAYER_STACK_COUNT];
static int32_t gdi_alpha_blending_source_layer_stack_index = 0;
static gdi_layer_handle_t g_alpha_blending_source_layer = GDI_ERROR_HANDLE;

/****************************************************************************
* Function Pre-declare
*****************************************************************************/

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_find_valid_handle
 * DESCRIPTION
 *  Init the Frame Buffer Manager
 *  
 *  Will return handle of a valid layer.
 *  If no valid layer, will retrun GDI_LAYER_ERROR_HANDLE.
 * PARAMETERS
 *  void
 * RETURNS
 *  gdi_layer_handle_t
 *****************************************************************************/
static gdi_layer_handle_t gdi_layer_find_valid_handle(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = GDI_LAYER_USER_ALLOCATE_HANDLE_START; i < GDI_LAYER_TOTAL_LAYER_COUNT; i++) {
        if (!GDI_LAYER_GET_FLAG(&GDI_LAYERS[i], GDI_LAYER_FLAG_USED)) {
            return (gdi_layer_handle_t) & GDI_LAYERS[i];
        }
    }
    return GDI_LAYER_EMPTY_HANDLE;
}

static gdi_color_format_t gdi_layer_convert_cf_to_vcf(gdi_color_format_t cf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(cf <GDI_COLOR_FORMAT_STANDARD_END)
    {
        return cf;
    }
    else
    {
        return gdi_get_color_format(gdi_bits_per_pixel(cf));  /* convert to standard color format */
    }
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_create_internal
 * DESCRIPTION
 *  Init a layer.
 *  
 *  Init layer as most basic layer, which does not
 *  support src_key, opacity, etc.
 *  This function will retrun false if it cant allocate buffer.
 * PARAMETERS
 *  handle              [IN]        
 *  offset_x            [IN]        
 *  offset_y            [IN]        
 *  width               [IN]        
 *  height              [IN]        
 *  cf                  [IN]        
 *  buf_ptr             [IN]        
 *  buf_size            [IN]        
 *  lcd_handle(?)       [IN]        
 * RETURNS
 *  void
 *  
 *****************************************************************************/
static void gdi_layer_create_internal(
                gdi_layer_handle_t handle,
                int32_t offset_x,
                int32_t offset_y,
                int32_t width,
                int32_t height,
                gdi_color_format_t cf,
                uint8_t *buf_ptr,
                uint32_t buf_size
                )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct_t *layer = (gdi_layer_struct_t*) handle;
    uint8_t id = layer->id;
    gdi_lcd_layer_struct *layer_info = &gdi_layer_info[id];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memset(layer, 0, sizeof(gdi_layer_struct_t));
    memset(layer_info, 0, sizeof(gdi_lcd_layer_struct));
    layer->id = id;

    GDI_LAYER_SET_FLAG(layer, GDI_LAYER_FLAG_USED);

    /* 
     * Layer framebuffer can only be obtained in the following five ways:
     * 
     * 1. App-based ASM: applib_mem_ap_alloc_framebuffer()
     * 2. Screen-based ASM: applib_mem_screen_alloc_framebuffer()
     * 3. ADM for framebuffer: MMI_FRM_ADM_ALLOC_FRAMEBUFFER()
     * 4. Media buffer
     * 5. ZI data in "NONCACHEDZI" section
     */
    layer->buf_ptr = buf_ptr;
    GDI_ASSERT(layer->buf_ptr);                            /* buffer available */
    GDI_ASSERT(((uint32_t)layer->buf_ptr & 0x03) == 0);              /* buffer 4-byte alignment */
    
    layer->cf = cf;
    layer->vcf = gdi_layer_convert_cf_to_vcf(layer->cf);    /* convert to standard color format */
    if ((GDI_COLOR_FORMAT_32 == layer->cf) ||
        (GDI_COLOR_FORMAT_32_PARGB== layer->cf) ||
        (GDI_COLOR_FORMAT_PARGB6666 == layer->cf))
    {
        layer_info->opacity_enable = true;
        layer_info->opacity_value = 0xFF;
    }

    layer->width = (uint16_t) width;
    layer->height = (uint16_t) height;
    layer->bits_per_pixel = (uint8_t) gdi_bits_per_pixel(cf);
    GDI_LOG("[GDI]create_internal cf:%d bpp:%d\n", layer->cf, layer->bits_per_pixel);
    layer->layer_size = (uint32_t) buf_size;

    layer->offset_x = offset_x;
    layer->offset_y = offset_y;

    /* set color format function */
    assert(cf < GDI_COLOR_FORMAT_END);

    /* set clipping */
    GDI_LAYER_RESET_CLIPS(layer);
    GDI_LAYER_SET_CLIP(layer, 0, 0, width - 1, height - 1);

    /* set blt region */
    GDI_LAYER_SET_BLT_RECT(layer, 0, 0, width, height);

    // default layer background is WHITE
    //W05.39 Because Hardware only could be set to 24 bit color not labyer color format
    //layer->background = gd_color_from_rgb_24(255, 255, 255, 255);

    /* init lcd_layer */
    if (layer->cf == GDI_COLOR_FORMAT_UYVY422)
    {
        layer_info->dither_enable = true;
    }
    layer_info->frame_buffer_address = (uint32_t) layer->buf_ptr;
    layer_info->x_offset = (offset_x + GDI_LAYER_OFFSET);
    layer_info->y_offset = (offset_y + GDI_LAYER_OFFSET);
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_init
 * DESCRIPTION
 *  Init the Frame Buffer Manager
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
gdi_result_t gdi_layer_init(gdi_color_format_t color_format, uint8_t *buffer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_init)
    int i;

    // initialize layer array
    memset(&GDI_LAYERS, 0, sizeof(GDI_LAYERS));
    memset(&gdi_layer_info, 0, sizeof(gdi_layer_info));
    for (i = 0; i < GDI_LAYER_TOTAL_LAYER_COUNT; i++)
    {
        GDI_LAYERS[i].id = (uint8_t)i;
    }

    /* init layer stack */
    gdi_layer_stack_index = 0;

    /* init mian lcd base layer */
    gdi_layer_create_internal(
        GDI_LAYER_MAIN_BASE_LAYER_HANDLE,
        0,
        0,
        GDI_LCD_WIDTH,
        GDI_LCD_HEIGHT,
        color_format,
        buffer,
        GDI_LCD_WIDTH * GDI_LCD_HEIGHT * gdi_bits_per_pixel(color_format) / 8);
    GDI_LAYER_SET_FLAG(GDI_LAYER_MAIN_BASE_LAYER_HANDLE, GDI_LAYER_FLAG_USE_OUTSIDE_MEMORY);
    gdi_layer_set_active(GDI_LAYER_MAIN_BASE_LAYER_HANDLE);
    gdi_layer_clear(0);
 	
    /* set main LCD active */
    //gdi_lcd_set_active(GDI_LCD_MAIN_LCD_HANDLE);

    GDI_EXIT_CRITICAL_SECTION(gdi_layer_init)
    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_create_using_outside_memory
 * DESCRIPTION
 *  Create a layer
 *  
 *  Will return handle of the layer.
 *  If no valid layer, will retrun GDI_LAYER_ERROR_HANDLE.
 * PARAMETERS
 *  cf                      [IN]        
 *  offset_x                [IN]        
 *  offset_y                [IN]        
 *  width                   [IN]        
 *  height                  [IN]        
 *  handle_ptr              [?]         
 *  outside_memory          [?]         
 *  outside_memory_size     [IN]        
 * RETURNS
 *  gdi_layer_handle_t
 *****************************************************************************/
gdi_result_t gdi_layer_create_cf_using_outside_memory(
            gdi_color_format_t cf,
            int32_t offset_x,
            int32_t offset_y,
            int32_t width,
            int32_t height,
            uint8_t *outside_memory,
            uint32_t outside_memory_size,
            gdi_layer_handle_t *handle_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_handle_t handle;
    gdi_result_t ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_create_using_outside_memory)

    *handle_ptr = handle = gdi_layer_find_valid_handle();
    GDI_ASSERT(handle);
    {
        uint8_t *buf;
        int32_t buffer_size;
        buffer_size = gdi_sizeof_pixels(cf, width, height);
        if (buffer_size > outside_memory_size) {
            GDI_ASSERT(0);
        }

        buf = outside_memory;
        gdi_layer_create_internal(handle, offset_x, offset_y, width, height, cf, buf, outside_memory_size
                            #ifdef GDI_DEBUG_SUPPORT
                                , (void*)creator_lr
                            #endif
                                );
        GDI_LAYER_SET_FLAG(handle, GDI_LAYER_FLAG_USE_OUTSIDE_MEMORY);
        ret = GDI_SUCCEED;
    }
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_create_using_outside_memory)
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_active
 * DESCRIPTION
 *  Set a layer as active layer.
 *  
 *  Will set some golbal variable, for faster reference.
 * PARAMETERS
 *  handle      [IN]        
 * RETURNS
 *  void
 * bool
 *  
 *****************************************************************************/
void gdi_layer_set_active(gdi_layer_handle_t handle)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_set_active)

    GDI_ASSERT(handle != GDI_NULL_HANDLE);

    gdi_act_layer = (gdi_layer_struct_t*) handle;
    gdi_act_layer_info = &(gdi_layer_info[gdi_act_layer->id]);

    GDI_ASSERT(GDI_LAYER_GET_FLAG(gdi_act_layer, GDI_LAYER_FLAG_USED));

    gdi_act_color_from_rgb = gd_color_from_rgb[gdi_act_layer->cf];
    gdi_act_color_to_rgb = gd_color_to_rgb[gdi_act_layer->cf];

    gdi_act_put_pixel = gd_put_pixel[gdi_act_layer->cf];
    gdi_act_get_pixel = gd_get_pixel[gdi_act_layer->cf];
    gdi_act_replace_src_key = gd_replace_src_key[gdi_act_layer->cf];

    GDI_EXIT_CRITICAL_SECTION(gdi_layer_set_active)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_push_and_set_active
 * DESCRIPTION
 *  push current layer into stack and set passin layer active
 * PARAMETERS
 *  handle      [IN]        
 * RETURNS
 *  gdi_result_t(?)
 *****************************************************************************/
void gdi_layer_push_and_set_active(gdi_layer_handle_t handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_LOCK;
    /* no more stack entry available */
    GDI_ASSERT(gdi_layer_stack_index < GDI_LAYER_ACT_LAYER_STACK_COUNT - 1);

    gdi_layer_stack[gdi_layer_stack_index++] = (gdi_layer_handle_t) gdi_act_layer;
    gdi_layer_set_active(handle);
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_pop_and_restore_active
 * DESCRIPTION
 *  pop the layer in stack and make it active
 * PARAMETERS
 *  void
 * RETURNS
 *  gdi_result_t(?)
 *****************************************************************************/
void gdi_layer_pop_and_restore_active(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ASSERT(gdi_layer_stack_index > 0);
    gdi_layer_set_active(gdi_layer_stack[--gdi_layer_stack_index]);
    GDI_UNLOCK;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_active
 * DESCRIPTION
 *  get currently active layer handle
 * PARAMETERS
 *  handle_ptr      [OUT]       
 * RETURNS
 *  gdi_result_t(?)
 *****************************************************************************/
void gdi_layer_get_active(gdi_layer_handle_t *handle_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_get_active)
    *handle_ptr = (gdi_layer_handle_t) gdi_act_layer;
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_get_active)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_free
 * DESCRIPTION
 *  Release layer
 *  
 *  return false if not valid handle.
 * PARAMETERS
 *  handle      [IN]        
 * RETURNS
 * bool
 *  
 *****************************************************************************/
gdi_result_t gdi_layer_free(gdi_layer_handle_t handle)
{    
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_free)
    {
        gdi_layer_struct_t *layer = (gdi_layer_struct_t*) handle;

        GDI_DEBUG_ASSERT(handle >= (gdi_layer_handle_t) & (GDI_LAYERS[GDI_LAYER_USER_ALLOCATE_HANDLE_START]));
        GDI_ASSERT(GDI_LAYER_GET_FLAG(layer, GDI_LAYER_FLAG_USED));

        layer->flag = 0;    /* clear all flag */
        
        //gdi_layer_dump_information();
    }
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_free)
    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_color_format
 * DESCRIPTION
 *  get layer color format
 * PARAMETERS
 *  cf      [OUT]    active layer color format 
 * RETURNS
 * gdi_result_t
 *****************************************************************************/
gdi_result_t gdi_layer_get_color_format(gdi_color_format_t *cf)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_get_color_format)
    *cf = gdi_act_layer->cf;
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_get_color_format)
    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_color_format
 * DESCRIPTION
 *  change color format of active layer.
 *  It assert if the layer buffer size is not enough for new color format.
 *  Please noticy that it may change the opacity & color_palette_enable of layer.
 * PARAMETERS
 *  cf      [IN]    new color format 
 * RETURNS
 * gdi_result_t
 *****************************************************************************/
gdi_result_t gdi_layer_set_color_format(gdi_color_format_t cf)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t buffer_size = 0;
    gdi_lcd_layer_struct *layer_info = &gdi_layer_info[gdi_act_layer->id];
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/    
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_set_color_format)

    if (gdi_act_layer->cf == cf)    /* don't have to change cf */
    {
        GDI_RETURN(GDI_SUCCEED);
    }
    
    GDI_DEBUG_ASSERT(GDI_LAYER_GET_FLAG(gdi_act_layer,GDI_LAYER_FLAG_USED));
    GDI_DEBUG_ASSERT(cf < GDI_COLOR_FORMAT_END);

    /* buf size have to be enough for new color format */
    buffer_size = gdi_sizeof_pixels(cf, gdi_act_layer->width, gdi_act_layer->height);
    GDI_ASSERT(buffer_size <= gdi_act_layer->layer_size);
    if (buffer_size > gdi_act_layer->layer_size) {
        GDI_ASSERT(0);
    }

    /* set color format */
    gdi_act_layer->cf = cf;
    gdi_act_layer->vcf = gdi_layer_convert_cf_to_vcf(cf);    /* convert to standard color format */

    /* set layer dithering */
    if (gdi_act_layer->cf == GDI_COLOR_FORMAT_UYVY422)
    {
        layer_info->dither_enable = true;
    }
    else
    {
        layer_info->dither_enable = false;
    }
    
    /* set opacity and opacity value */
    if ((GDI_COLOR_FORMAT_32 == gdi_act_layer->cf) ||
        (GDI_COLOR_FORMAT_32_PARGB== gdi_act_layer->cf) ||
        (GDI_COLOR_FORMAT_PARGB6666 == gdi_act_layer->cf))
    {
        layer_info->opacity_enable = true;
        layer_info->opacity_value = 0xFF;
    }
    else
    {
        layer_info->opacity_enable = false;
        layer_info->opacity_value = 0;
    }

    /* set bits per pixel */
    gdi_act_layer->bits_per_pixel = (uint8_t) gdi_bits_per_pixel(cf);

    /* set active layer func pointer */
    gdi_act_color_from_rgb = gd_color_from_rgb[cf];
    gdi_act_color_to_rgb = gd_color_to_rgb[cf];

    gdi_act_put_pixel = gd_put_pixel[cf];
    gdi_act_get_pixel = gd_get_pixel[cf];
    gdi_act_replace_src_key = gd_replace_src_key[cf];

    GDI_EXIT_CRITICAL_SECTION(gdi_layer_set_color_format)
    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_position
 * DESCRIPTION
 *  Move the layer to its desired position.
 * PARAMETERS
 *  offset_x            [IN]        
 *  offset_y            [IN]        
 *  gdi_layer_handle_t(?)       [IN]        Handle
 * RETURNS
 *  void
 *****************************************************************************/
gdi_result_t gdi_layer_set_position(int32_t offset_x, int32_t offset_y)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_set_position)
    ret = gdi_layer_set_position_nb_concurrent((gdi_layer_handle_t)gdi_act_layer, offset_x, offset_y);
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_set_position)
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_blt_rect
 * DESCRIPTION
 *  set layer blt region. Display driver will merge blt rect of the layers and
 *  blt roi, so the final blt region may exceed this rect.
 * PARAMETERS
 *  x   [IN]  x coordinate, origin is layer frame buffer
 *  y   [IN]  y coordinate, origin is layer frame buffer
 *  width   [IN] blt rect width
 *  height  [IN] blt rect height
 * RETURNS
 *  void
 *****************************************************************************/
gdi_result_t gdi_layer_set_blt_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t ret = GDI_SUCCEED;
    uint16_t x2, y2, x_max_limit, y_max_limit;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_set_blt_rect)

    x2 = x + width - 1;
    y2 = y + height - 1;
    x_max_limit = gdi_act_layer->width - 1;
    y_max_limit = gdi_act_layer->height - 1;

    LIMIT_BOUND_UNSIGNED(x,  x_max_limit);
    LIMIT_BOUND_UNSIGNED(y,  y_max_limit);
    LIMIT_BOUND_UNSIGNED(x2, x_max_limit);
    LIMIT_BOUND_UNSIGNED(y2, y_max_limit);

    if (width == 0 || height == 0)
	{
    	GDI_LAYER_SET_BLT_RECT(gdi_act_layer, x, y, 0, 0);
	}
	else
	{
		GDI_LAYER_SET_BLT_RECT(gdi_act_layer, x, y, x2 - x + 1, y2 - y + 1);
	}

    GDI_EXIT_CRITICAL_SECTION(gdi_layer_set_blt_rect)
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_clip
 * DESCRIPTION
 *  Set clip region.
 *  
 *  Will set clip region, use layer's own coordinate.
 * PARAMETERS
 *  x1                  [IN]        
 *  y1                  [IN]        
 *  x2                  [IN]        
 *  y2                  [IN]        
 *  gdi_layer_handle_t(?)       [IN]        Handle
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_set_clip(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t x_max_limit;
    int32_t y_max_limit;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_set_clip)

    if (x2 < x1)
    {
        x2 = x1;
    }
    if (y2 < y1)
    {
        y2 = y1;
    }

    x_max_limit = gdi_act_layer->width - 1;
    y_max_limit = gdi_act_layer->height - 1;

    LIMIT_BOUND(x1, 0, x_max_limit);
    LIMIT_BOUND(x2, 0, x_max_limit);
    LIMIT_BOUND(y1, 0, y_max_limit);
    LIMIT_BOUND(y2, 0, y_max_limit);

    GDI_LAYER_SET_CLIP(gdi_act_layer, x1, y1, x2, y2);

    GDI_EXIT_CRITICAL_SECTION(gdi_layer_set_clip)
}

gdi_result_t gdi_layer_test_set_clip(gdi_rect_t *rect)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t x_max_limit;
    int32_t y_max_limit;
    gdi_result_t ret = GDI_FAILED;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_test_set_clip)
    if (rect->x1 <= gdi_act_layer->clipx2 && rect->x2 >= gdi_act_layer->clipx1 && rect->x1 <= rect->x2
        && rect->y1 <= gdi_act_layer->clipy2 && rect->y2 >= gdi_act_layer->clipy1 && rect->y1 <= rect->y2)
    {
        if (rect->x1 < gdi_act_layer->clipx1) {rect->x1 = gdi_act_layer->clipx1;}
        if (rect->x2 > gdi_act_layer->clipx2) {rect->x2 = gdi_act_layer->clipx2;}
        if (rect->y1 < gdi_act_layer->clipy1) {rect->y1 = gdi_act_layer->clipy1;}
        if (rect->y2 > gdi_act_layer->clipy2) {rect->y2 = gdi_act_layer->clipy2;}

        x_max_limit = gdi_act_layer->width - 1;
        y_max_limit = gdi_act_layer->height - 1;

        LIMIT_BOUND((rect->x1), 0, x_max_limit);
        LIMIT_BOUND((rect->x2), 0, x_max_limit);
        LIMIT_BOUND((rect->y1), 0, y_max_limit);
        LIMIT_BOUND((rect->y2), 0, y_max_limit);

        GDI_LAYER_SET_CLIP(gdi_act_layer, (rect->x1), (rect->y1), (rect->x2), (rect->y2));
        ret = GDI_SUCCEED;
    }

    GDI_EXIT_CRITICAL_SECTION(gdi_layer_test_set_clip)
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_reset_clip
 * DESCRIPTION
 *  Reset clip region to layer size
 *  
 *  Will reset clip region to layer's width/height.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_reset_clip(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_reset_clip)
    GDI_LAYER_SET_CLIP(gdi_act_layer, 0, 0, gdi_act_layer->width - 1, gdi_act_layer->height - 1);
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_reset_clip)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_clip
 * DESCRIPTION
 *  Get current clip region of a layer.
 * PARAMETERS
 *  x1                  [IN]        
 *  y1                  [IN]        
 *  x2                  [IN]        
 *  y2                  [IN]        
 *  gdi_layer_handle_t(?)       [IN]        Handle
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_get_clip(int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_get_clip)
    *x1 = gdi_act_layer->clipx1;
    *y1 = gdi_act_layer->clipy1;
    *x2 = gdi_act_layer->clipx2;
    *y2 = gdi_act_layer->clipy2;
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_get_clip)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_push_clip
 * DESCRIPTION
 *  Push current clip region into stack.
 *  
 *  JF's opnion is the merge the clip region
 *  to get final clip region - need more study
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_push_clip(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_rect_t *rect;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_LOCK;
    GDI_ASSERT(gdi_act_layer->clips_top < GDI_LAYER_CLIP_STACK_COUNT);

    rect = &gdi_act_layer->clips[gdi_act_layer->clips_top];

    rect->x1 = gdi_act_layer->clipx1;
    rect->y1 = gdi_act_layer->clipy1;
    rect->x2 = gdi_act_layer->clipx2;
    rect->y2 = gdi_act_layer->clipy2;

    gdi_act_layer->clips_top++;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_push_and_set_clip
 * DESCRIPTION
  *  Push current clip region into stack and set clip region, use layer's 
  *  own coordinate.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_push_and_set_clip(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/   
    gdi_layer_push_clip();
    gdi_layer_set_clip(x1, y1, x2, y2);
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_pop_clip
 * DESCRIPTION
 *  Pop current clip region into stack.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_pop_clip(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_rect_t *rect;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ASSERT(gdi_act_layer->clips_top > 0);
    rect = &gdi_act_layer->clips[--(gdi_act_layer->clips_top)];

    gdi_act_layer->clipx1 = rect->x1;
    gdi_act_layer->clipy1 = rect->y1;
    gdi_act_layer->clipx2 = rect->x2;
    gdi_act_layer->clipy2 = rect->y2;

    GDI_UNLOCK;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_clip_preset
 * DESCRIPTION
 *  Set clip region.
 *  
 *  Will set clip region, use layer's own coordinate.
 * PARAMETERS
 *  x1                  [IN]        
 *  y1                  [IN]        
 *  x2                  [IN]        
 *  y2                  [IN]        
 *  gdi_layer_handle_t(?)       [IN]        Handle
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_set_clip_preset(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_set_clip_preset)

    if (GDI_LAYER.clipx1 < x1)
    {
        x1 = GDI_LAYER.clipx1;
    }
    if (GDI_LAYER.clipy1 < y1)
    {
        y1 = GDI_LAYER.clipy1;
    }
    if (GDI_LAYER.clipx2 > x2)
    {
        x2 = GDI_LAYER.clipx2;
    }
    if (GDI_LAYER.clipy2 > y2)
    {
        y2 = GDI_LAYER.clipy2;
    }

    if (x2 < x1)
    {
        x2 = x1;
    }
    if (y2 < y1)
    {
        y2 = y1;
    }

    /* the previous clip area is already in lcd area */

    GDI_LAYER_SET_CLIP(gdi_act_layer, x1, y1, x2, y2);
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_set_clip_preset)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_opacity
 * DESCRIPTION
 *  Set opacity value.
 *  
 *  Set opacity.
 * PARAMETERS
 *  opacity_enable      [IN]        
 *  opacity_value       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
gdi_result_t gdi_layer_set_opacity(bool opacity_enable, uint8_t opacity_value)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_set_opacity)
    ret = gdi_layer_set_opacity_nb_concurrent((gdi_layer_handle_t)gdi_act_layer, opacity_enable, opacity_value);
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_set_opacity)
    
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_opacity
 * DESCRIPTION
 *  Set opacity value.
 *  
 *  Set opacity.
 * PARAMETERS
 *  *opacity_enable      [OUT]     
 *  *opacity_value       [OUT]        
 * RETURNS
 *  void
 *****************************************************************************/
gdi_result_t gdi_layer_get_opacity(bool *opacity_enable, uint8_t *opacity_value)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_get_opacity)

    *opacity_enable = gdi_act_layer_info->opacity_enable;
    *opacity_value = gdi_act_layer_info->opacity_value;
    
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_get_opacity)
    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_source_key
 * DESCRIPTION
 *  Set opacity value.
 *  
 *  Set opacity.
 * PARAMETERS
 *  source_key_enable       [IN]        
 *  source_key_value        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
gdi_result_t gdi_layer_set_source_key(bool source_key_enable, gdi_color_t source_key_value)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_set_source_key)
    ret = gdi_layer_set_source_key_nb_concurrent((gdi_layer_handle_t)gdi_act_layer, source_key_enable, source_key_value);
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_set_source_key)
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_source_key
 * DESCRIPTION
 *  
 * PARAMETERS
 *  source_key_enable       [?]     
 *  source_key_value        [?]     
 * RETURNS
 *  
 *****************************************************************************/
gdi_result_t gdi_layer_get_source_key(bool *source_key_enable, gdi_color_t *source_key_value)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_set_source_key)
    *source_key_enable = gdi_act_layer_info->source_key_enable;
    *source_key_value = gdi_act_layer_info->source_key;
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_set_source_key)
    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_rotate
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
uint8_t gdi_layer_get_rotate(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
   uint8_t ret;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_get_rotate)
    ret = gdi_act_layer->rotate_value;
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_get_rotate)
    return ret;
}

static uint8_t get_bpp_from_cf(uint8_t layer_cf)
{
    switch (layer_cf) {
        case GDI_COLOR_FORMAT_8:
            return 1;
        case GDI_COLOR_FORMAT_16:
            return 2;
        case GDI_COLOR_FORMAT_24:
            return 3;
        case GDI_COLOR_FORMAT_32:
            return 4;
        case GDI_COLOR_FORMAT_UYVY422:
            return 4;
        case GDI_COLOR_FORMAT_PARGB6666:
            return 3;
    }
    return 2;
}

static hal_display_lcd_layer_source_color_format_t convert_gdi_cf_to_lcd(gdi_color_format_t cf)
{
    hal_display_lcd_layer_source_color_format_t real_cf = HAL_DISPLAY_LCD_LAYER_COLOR_RGB565;
    if (GDI_COLOR_FORMAT_16 == cf) {
        real_cf = HAL_DISPLAY_LCD_LAYER_COLOR_RGB565;
    } else if (GDI_COLOR_FORMAT_24 == cf) {
        real_cf = HAL_DISPLAY_LCD_LAYER_COLOR_RGB888;
    } else if (GDI_COLOR_FORMAT_32 == cf) {
        real_cf = HAL_DISPLAY_LCD_LAYER_COLOR_ARGB8888;
    } else if (GDI_COLOR_FORMAT_8 == cf) {
        real_cf = HAL_DISPLAY_LCD_LAYER_COLOR_8BPP_INDEX;
    } else if (GDI_COLOR_FORMAT_1 == cf) {
        real_cf = HAL_DISPLAY_LCD_LAYER_COLOR_1BIT_INDEX;
    } else {
        real_cf = HAL_DISPLAY_LCD_LAYER_COLOR_RGB565;
    }
    return real_cf;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_blt_ext_internal
 * DESCRIPTION
 *  Blt means to update layers to LCD.
 *  The last four parameters are the region which to blt to LCD.
 *  handle0 is the most bottom layer, and handle5 is most top layer.
 *  NOTICE: Should not take gdi mutex or modify any global variable in this API.
 * PARAMETERS
 *  handle0     [IN]        
 *  handle1     [IN]        
 *  handle2     [IN]        
 *  handle3     [IN]        
 *  handle4     [IN]        
 *  handle5     [IN]        
 *  x1          [IN]        
 *  y1          [IN]        
 *  x2          [IN]        
 *  y2          [IN]        
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
static gdi_result_t gdi_layer_blt_ext_internal(
                    gdi_layer_handle_t handle0,
                    gdi_layer_handle_t handle1,
                    gdi_layer_handle_t handle2,
                    gdi_layer_handle_t handle3,
                    int32_t x1,
                    int32_t y1,
                    int32_t x2,
                    int32_t y2,
                    gdi_layer_blt_flag_bit_set flag
                    )
{
    hal_display_lcd_roi_output_t lcd_para;
    hal_display_lcd_layer_input_t lcm_para;
    gdi_layer_struct_t *layer_struct;

GDI_LOG("[GDI_LAYER]Enter gdi_layer_blt_ext_internal x1:%d y1:%d x2:%d y2:%d\n", x1, y1, x2, y2);

	if (y2 < y1 || x2 < x1 || x1 >= GDI_LCD->act_width || x2 < 0 || y1 >= GDI_LCD->act_height || y2 < 0)
	{
		return GDI_LAYER_ERR_INVALID_BLT_REGION;
	}
  //Fixed: outside memory layer will not be cleared.    
  //hal_lcd_initialize(MAIN_LCD_CMD_ADDR, MAIN_LCD_DATA_ADDR, MAIN_LCD_OUTPUT_FORMAT);

	if (x1 < 0)
	{
		x1 = 0;
	}
	if (y1 < 0)
	{
		y1 = 0;
	}
	if (x2 > GDI_LCD->act_width - 1)
	{
		x2 = GDI_LCD->act_width - 1;
	}
	if (y2 > GDI_LCD->act_height - 1)
	{
		y2 = GDI_LCD->act_height - 1;
	}

    lcd_para.target_start_x = x1;
    lcd_para.target_start_y = y1;
    lcd_para.target_end_x = x2;
    lcd_para.target_end_y = y2;
    lcd_para.roi_offset_x = x1;
    lcd_para.roi_offset_y = y1;
    //lcd_para.update_layer = HAL_DISPLAY_LCD_LAYER0;
    lcd_para.main_lcd_output = LCM_16BIT_16_BPP_RGB565_1;
GDI_LOG("[GDI_LAYER]bsp_lcd_config_roi (%d, %d, %d, %d)\n", x1, y1, x2, y2);
    bsp_lcd_config_roi(&lcd_para);

  if (handle0 != GDI_NULL_HANDLE) {
    layer_struct = (gdi_layer_struct_t *)handle0;
    lcm_para.layer_enable = HAL_DISPLAY_LCD_LAYER0;
    lcm_para.source_key_flag = 0;
    lcm_para.alpha_flag = 0;
    lcm_para.color_format = convert_gdi_cf_to_lcd(layer_struct->cf);
    lcm_para.alpha = 0;
    lcm_para.rotate = HAL_DISPLAY_LCD_LAYER_ROTATE_0;
    lcm_para.buffer_address = (uint32_t)layer_struct->buf_ptr;
    lcm_para.row_size = layer_struct->height;
    lcm_para.column_size = layer_struct->width;
    lcm_para.window_x_offset = layer_struct->offset_x;
    lcm_para.window_y_offset = layer_struct->offset_y;
    lcm_para.pitch = layer_struct->width * get_bpp_from_cf(layer_struct->cf);

GDI_LOG("[GDI_LAYER]bsp_lcd_config_layer, buf:%x, row:%d, col:%d, xoff:%d, yoff:%d, pitch:%d\n", lcm_para.buffer_address,
        GDI_LCD->act_height, GDI_LCD->act_width, layer_struct->offset_x, layer_struct->offset_y, lcm_para.pitch);
		bsp_lcd_config_layer(&lcm_para);
	}

	if (handle1 != GDI_NULL_HANDLE) {
		layer_struct = (gdi_layer_struct_t *)handle1;

		lcm_para.source_key_flag = gdi_layer_info[layer_struct->id].source_key_enable;
        lcm_para.color_key = gdi_layer_info[layer_struct->id].source_key;
		lcm_para.alpha_flag = 0;
        lcm_para.alpha = 0;
		lcm_para.color_format = convert_gdi_cf_to_lcd(layer_struct->cf);;
		lcm_para.rotate = HAL_DISPLAY_LCD_LAYER_ROTATE_0;
		lcm_para.row_size = layer_struct->height;
		lcm_para.column_size = layer_struct->width;
		lcm_para.window_x_offset = layer_struct->offset_x;
		lcm_para.window_y_offset = layer_struct->offset_y;
		lcm_para.buffer_address = (uint32_t)layer_struct->buf_ptr;
		lcm_para.pitch = layer_struct->width * get_bpp_from_cf(layer_struct->cf);
		lcm_para.layer_enable = HAL_DISPLAY_LCD_LAYER1;

		bsp_lcd_config_layer(&lcm_para);
	}

	if (handle2 != GDI_NULL_HANDLE) {
		layer_struct = (gdi_layer_struct_t *)handle2;

		lcm_para.source_key_flag = gdi_layer_info[layer_struct->id].source_key_enable;
        lcm_para.color_key = gdi_layer_info[layer_struct->id].source_key;
		lcm_para.alpha_flag = 0;
        lcm_para.alpha = 0;
		lcm_para.color_format = convert_gdi_cf_to_lcd(layer_struct->cf);;
		lcm_para.rotate = HAL_DISPLAY_LCD_LAYER_ROTATE_0;
		lcm_para.row_size = layer_struct->height;
		lcm_para.column_size = layer_struct->width;
		lcm_para.window_x_offset = layer_struct->offset_x;
		lcm_para.window_y_offset = layer_struct->offset_y;
		lcm_para.buffer_address = (uint32_t)layer_struct->buf_ptr;
		lcm_para.pitch = layer_struct->width * get_bpp_from_cf(layer_struct->cf);
		lcm_para.layer_enable = HAL_DISPLAY_LCD_LAYER2;

		bsp_lcd_config_layer(&lcm_para);
	}

	if (handle3 != GDI_NULL_HANDLE) {
		layer_struct = (gdi_layer_struct_t *)handle3;

		lcm_para.source_key_flag = gdi_layer_info[layer_struct->id].source_key_enable;
        lcm_para.color_key = gdi_layer_info[layer_struct->id].source_key;
		lcm_para.alpha_flag = 0;
        lcm_para.alpha = 0;
		lcm_para.color_format = convert_gdi_cf_to_lcd(layer_struct->cf);;
		lcm_para.rotate = HAL_DISPLAY_LCD_LAYER_ROTATE_0;
		lcm_para.row_size = layer_struct->height;
		lcm_para.column_size = layer_struct->width;
		lcm_para.window_x_offset = layer_struct->offset_x;
		lcm_para.window_y_offset = layer_struct->offset_y;
		lcm_para.buffer_address = (uint32_t)layer_struct->buf_ptr;
		lcm_para.pitch = layer_struct->width * get_bpp_from_cf(layer_struct->cf);
		lcm_para.layer_enable = HAL_DISPLAY_LCD_LAYER3;

		bsp_lcd_config_layer(&lcm_para);
	}
GDI_LOG("[GDI_LAYER]Begin to bsp_lcd_update_screen [%d, %d, %d, %d]\n", lcd_para.target_start_x , lcd_para.target_start_y ,lcd_para.target_end_x, lcd_para.target_end_y);
	bsp_lcd_update_screen(lcd_para.target_start_x , lcd_para.target_start_y ,lcd_para.target_end_x, lcd_para.target_end_y);
	return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_blt_ext
 * DESCRIPTION
 *  Blt layers.
 *
 *  The last four parameters are the region which to blt to LCD.
 * PARAMETERS
 *  handle0     [IN]        
 *  handle1     [IN]        
 *  handle2     [IN]        
 *  handle3     [IN]        
 *  handle4     [IN]        
 *  handle5     [IN]        
 *  x1          [IN]        
 *  y1          [IN]        
 *  x2          [IN]        
 *  y2          [IN]        
 * RETURNS
 * bool
 *  
 *****************************************************************************/
gdi_result_t gdi_layer_blt_ext(
            gdi_layer_handle_t handle0, 
            gdi_layer_handle_t handle1, 
            gdi_layer_handle_t handle2, 
            gdi_layer_handle_t handle3,
            int32_t x1, 
            int32_t y1, 
            int32_t x2, 
            int32_t y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t ret;
    gdi_layer_blt_flag_bit_set flag;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/   
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_blt)

    flag = 0;

    ret = gdi_layer_blt_ext_internal(
            handle0, 
            handle1, 
            handle2, 
            handle3,
            x1, 
            y1, 
            x2, 
            y2,
            flag
            );

    GDI_EXIT_CRITICAL_SECTION(gdi_layer_blt)

    return ret;
}

gdi_result_t gdi_layer_blt(gdi_layer_handle_t handle0, gdi_layer_handle_t handle1, 
                           gdi_layer_handle_t handle2, gdi_layer_handle_t handle3,
                           int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    return gdi_layer_blt_ext(handle0, handle1, handle2, handle3, x1, y1, x2, y2);
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_blt_layer_ext
 * DESCRIPTION
 *  
 * PARAMETERS
 *  handle0     [?]     
 *  handle1     [?]     
 *  handle2     [?]     
 *  handle3     [?]     
 *  handle4     [?]     
 *  handle5     [?]     
 * RETURNS
 *  
 *****************************************************************************/
gdi_result_t gdi_layer_get_blt_layer_ext(
            gdi_layer_handle_t *handle0, 
            gdi_layer_handle_t *handle1, 
            gdi_layer_handle_t *handle2,
            gdi_layer_handle_t *handle3
            )
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_get_blt_layer)
    
    if(handle0) *handle0 = GDI_LCD->blt_handle[0];
    if(handle1) *handle1 = GDI_LCD->blt_handle[1];
    if(handle2) *handle2 = GDI_LCD->blt_handle[2];
    if(handle3) *handle3 = GDI_LCD->blt_handle[3];

    GDI_EXIT_CRITICAL_SECTION(gdi_layer_get_blt_layer)
    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_blt_layer_number
 * DESCRIPTION
 *  get how many layers that hw can blt or flatten at one time.
 * PARAMETERS
 *  void
 * RETURNS
 *  uint8_t
 *****************************************************************************/
uint8_t gdi_layer_get_blt_layer_number(void)
{
    return 4;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_blt_base_layer
 * DESCRIPTION
 *  Blt base layer
 * PARAMETERS
 *  x1      [IN]        
 *  y1      [IN]        
 *  x2      [IN]        
 *  y2      [IN]        
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
gdi_result_t gdi_layer_blt_base_layer(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t ret;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_blt_base_layer)
    ret = gdi_layer_blt(GDI_LAYER_MAIN_BASE_LAYER_HANDLE, 0, 0, 0, x1, y1, x2, y2);
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_blt_base_layer)
    return ret;
}

gdi_result_t gdi_lcd_update_screen(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    return gdi_layer_blt_base_layer(x1, y1, x2, y2);
}
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_blt_previous
 * DESCRIPTION
 *  Blt previous blt layers.
 * PARAMETERS
 *  x1      [IN]        
 *  y1      [IN]        
 *  x2      [IN]        
 *  y2      [IN]        
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
gdi_result_t gdi_layer_blt_previous(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t result;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_blt_previous)

    if (GDI_LCD->blt_handle[0] || GDI_LCD->blt_handle[1] || GDI_LCD->blt_handle[2] || GDI_LCD->blt_handle[3]) {
        result = gdi_layer_blt_ext(
                    GDI_LCD->blt_handle[0],
                    GDI_LCD->blt_handle[1], 
                    GDI_LCD->blt_handle[2], 
                    GDI_LCD->blt_handle[3],
                    x1,
                    y1, 
                    x2, 
                    y2);
    } else {
        result = gdi_layer_blt_ext(GDI_LCD->act_layer_handle, 0, 0, 0, x1, y1, x2, y2);
    }

    GDI_EXIT_CRITICAL_SECTION(gdi_layer_blt_previous)
    return result;
}


/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_base_handle
 * DESCRIPTION
 *  Get handle of base layer.
 * PARAMETERS
 *  handle_ptr      [?]     
 * RETURNS
 *  gdi_layer_handle_t
 *****************************************************************************/
gdi_result_t gdi_layer_get_base_handle(gdi_layer_handle_t *handle_ptr)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_get_base_handle)
    *handle_ptr = GDI_LAYER_MAIN_BASE_LAYER_HANDLE;
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_get_base_handle)
    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_restore_base_active
 * DESCRIPTION
 *  Restore base layer as active layer
 * PARAMETERS
 *  void
 * RETURNS
 *  gdi_result_t(?)
 *****************************************************************************/
void gdi_layer_restore_base_active(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_restore_base_active)
    gdi_layer_set_active(GDI_LAYER_MAIN_BASE_LAYER_HANDLE);
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_restore_base_active)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_clear
 * DESCRIPTION
 *  draw currently active layer's color
 * PARAMETERS
 *  bg_color        [IN]        
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
gdi_result_t gdi_layer_clear(gdi_color_t bg_color)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_clear)

GDI_LOG("[GDI]Enter clear_layer:%d %d\n", GDI_LAYER.bits_per_pixel, bg_color);
    /* UYVY422 is not supported currently, it's workaround */
    if (gdi_act_layer->cf == GDI_COLOR_FORMAT_UYVY422)
    {
        gdi_memset16((uint8_t*) gdi_act_layer->buf_ptr, 0x0080, gdi_act_layer->layer_size);
        GDI_RETURN(GDI_SUCCEED);
    }

    if (GDI_LAYER.bits_per_pixel == 16)
    {
        gdi_memset16((uint8_t*) gdi_act_layer->buf_ptr, (uint16_t) bg_color, gdi_act_layer->layer_size);
        //memset((uint8_t*) gdi_act_layer->buf_ptr, (uint16_t) bg_color, gdi_act_layer->layer_size);
    }
#ifdef GDI_PRIMITIVE_24_BIT_SUPPORT
    else if (GDI_LAYER.bits_per_pixel == 24)
    {
        gdi_memset24((uint8_t*) gdi_act_layer->buf_ptr, (uint32_t) bg_color, gdi_act_layer->width * gdi_act_layer->height * 3);
    }
#endif
    else if (GDI_LAYER.bits_per_pixel == 32)
    {
        gdi_memset32((uint8_t*) gdi_act_layer->buf_ptr, (uint32_t) bg_color, gdi_act_layer->layer_size);
    }
    else
    {
        gdi_layer_push_clip();
        gdi_layer_reset_clip();
        gdi_draw_filled_rectangle(0, 0, gdi_act_layer->width - 1, gdi_act_layer->height - 1, bg_color);
        gdi_layer_pop_clip();
    }
GDI_LOG("[GDI]Exit clear_layer\n");
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_clear)
    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_buffer_ptr
 * DESCRIPTION
 *  get currently active layer's frame buffer pointer.
 * PARAMETERS
 *  buf_ptr     [OUT]       
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
gdi_result_t gdi_layer_get_buffer_ptr(uint8_t **buf_ptr)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_get_buffer_ptr)
    *buf_ptr = gdi_act_layer->buf_ptr;
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_get_buffer_ptr)
    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_dimension
 * DESCRIPTION
 *  get currently active layer's size.
 * PARAMETERS
 *  width       [OUT]       
 *  height      [OUT]       
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
gdi_result_t gdi_layer_get_dimension(uint32_t *width, uint32_t *height)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_get_dimension)
    *width = gdi_act_layer->width;
    *height = gdi_act_layer->height;
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_get_dimension)
    return GDI_SUCCEED;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_position
 * DESCRIPTION
 *  get currently active layer's position.
 * PARAMETERS
 *  offset_x        [OUT]       
 *  offset_y        [OUT]       
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
gdi_result_t gdi_layer_get_position(int32_t *offset_x, int32_t *offset_y)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t ret;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_get_position)
    ret = gdi_layer_get_position_nb_concurrent((gdi_layer_handle_t)gdi_act_layer, offset_x, offset_y);
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_get_position)
    return ret;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_bit_per_pixel
 * DESCRIPTION
 *  get the active layer pixel size (in bits)
 * PARAMETERS
 *  void
 * RETURNS
 *****************************************************************************/
int gdi_layer_get_bit_per_pixel(void)
{
    return GDI_LAYER.bits_per_pixel;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_resize
 * DESCRIPTION
 *  
 * PARAMETERS
 *  width       [IN]        
 *  height      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_resize(int32_t width, int32_t height)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_resize)
    //gdi_layer_resize_nb_concurrent((gdi_layer_handle_t)gdi_act_layer, width, height);
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_resize)
}
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_rotate
 * DESCRIPTION
 *  
 * PARAMETERS
 *  rotate_value        [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_set_rotate(uint8_t rotate_value)
{
#if defined(GDI_USING_LAYER_ROTATE)
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_set_rotate)
    gdi_layer_set_rotate_nb_concurrent((gdi_layer_handle_t)gdi_act_layer, rotate_value);
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_set_rotate)
#endif /* defined(GDI_USING_LAYER_ROTATE) */ 
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_frame_buffer_lock_count
 * DESCRIPTION
 *  get the frame buffer blt_lock count
 * PARAMETERS
 *  frame_buffer_lock_count [OUT] 
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_get_frame_buffer_lock_count(int32_t *frame_buffer_lock_count)
{
    GDI_ENTER_CRITICAL_SECTION(gdi_layer_get_frame_buffer_lock_count)
    *frame_buffer_lock_count = GDI_LCD->blt_lock;
    GDI_EXIT_CRITICAL_SECTION(gdi_layer_get_frame_buffer_lock_count)
}

/*****************************************************************************
 * FUNCTION
 *  gdi_set_alpha_blending_source_layer
 * DESCRIPTION
 *  set alpha source blending source layer
 * PARAMETERS
 *  src_layer_handle       [IN] gdi handle of the alpha source layer
 * RETURNS
 *  gdi_layer_handle_t  old alpha source layer handle
 *****************************************************************************/
gdi_layer_handle_t gdi_set_alpha_blending_source_layer(gdi_layer_handle_t src_layer_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_handle_t  old_src_layer_handle;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_LOCK;
    
    old_src_layer_handle = g_alpha_blending_source_layer;
    g_alpha_blending_source_layer = src_layer_handle;  
    
    GDI_UNLOCK;

    return old_src_layer_handle;
}


/*****************************************************************************
 * FUNCTION
 *  gdi_get_alpha_blending_source_layer
 * DESCRIPTION
 *  get alpha blending source layer
 * PARAMETERS
 *  src_layer_handle       [OUT] gdi handle of the alpha source layer
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_get_alpha_blending_source_layer(gdi_layer_handle_t *src_layer_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_LOCK;
    
    *src_layer_handle = g_alpha_blending_source_layer;
    
    GDI_UNLOCK;
}

/*****************************************************************************
 * FUNCTION
 *  gdi_push_and_set_alpha_blending_source_layer
 * DESCRIPTION
 *  push current alpha source layer into stack and set passin layer as alpha source layer
 * PARAMETERS
 *  handle      [IN]        
 * RETURNS
 *  gdi_result_t(?)
 *****************************************************************************/
void gdi_push_and_set_alpha_blending_source_layer(gdi_layer_handle_t handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_LOCK;

    /* no more stack entry available */
    GDI_ASSERT(gdi_alpha_blending_source_layer_stack_index <= GDI_ALPHA_BLENDING_SOURCE_LAYER_STACK_COUNT - 1);

    gdi_alpha_blending_source_layer_stack[gdi_alpha_blending_source_layer_stack_index++] = g_alpha_blending_source_layer;
    g_alpha_blending_source_layer = handle;
}


/*****************************************************************************
 * FUNCTION
 *  gdi_pop_and_restore_alpha_blending_source_layer
 * DESCRIPTION
 *  pop the layer in stack and make it as alpha source layer
 * PARAMETERS
 *  void
 * RETURNS
 *  gdi_result_t(?)
 *****************************************************************************/
void gdi_pop_and_restore_alpha_blending_source_layer(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    GDI_ASSERT(gdi_alpha_blending_source_layer_stack_index > 0);
    g_alpha_blending_source_layer = gdi_alpha_blending_source_layer_stack[--gdi_alpha_blending_source_layer_stack_index];
    GDI_UNLOCK;
}

/*
 *  NB Concurrent API
 */
gdi_result_t gdi_layer_set_position_nb_concurrent(gdi_layer_handle_t layer_handle, int32_t offset_x, int32_t offset_y)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct_t *layer = (gdi_layer_struct_t*)layer_handle;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    layer->offset_x = offset_x;
    layer->offset_y = offset_y;

    gdi_layer_info[layer->id].x_offset = (offset_x + GDI_LAYER_OFFSET);
    gdi_layer_info[layer->id].y_offset = (offset_y + GDI_LAYER_OFFSET);

    return GDI_SUCCEED;
}


gdi_result_t gdi_layer_set_rotate_nb_concurrent(gdi_layer_handle_t layer_handle, uint8_t rotate_value)
{
#if defined(GDI_USING_LAYER_ROTATE)
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct_t *layer = (gdi_layer_struct_t*)layer_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    layer->rotate_value = rotate_value;
    return GDI_SUCCEED;
#else
    return GDI_FAILED;
#endif /* defined(GDI_USING_LAYER_ROTATE) */ 
}


gdi_result_t gdi_layer_set_source_key_nb_concurrent(
            gdi_layer_handle_t layer_handle, 
            bool source_key_enable, 
            gdi_color_t source_key_value)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct_t *layer = (gdi_layer_struct_t*)layer_handle;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    #if defined(GDI_USE_RGB565_TO_SHOW_BW)
    gdi_layer_info[layer->id].source_key_enable = false;
    gdi_layer_info[layer->id].source_key = 0;
    #else
    gdi_layer_info[layer->id].source_key_enable = (bool)source_key_enable;
    gdi_layer_info[layer->id].source_key = source_key_value;
    #endif
    return GDI_SUCCEED;
}

gdi_result_t gdi_layer_set_opacity_nb_concurrent(gdi_layer_handle_t layer_handle, bool opacity_enable, uint8_t opacity_value)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct_t *layer = (gdi_layer_struct_t*)layer_handle;
    gdi_lcd_layer_struct *layer_info;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    layer_info = &gdi_layer_info[layer->id];
#if !defined(GDI_LCD_MEMORY_OUTPUT_PARGB8888_SUPPORT)
    if (GDI_COLOR_FORMAT_32 == layer->cf)
    {
        layer_info->opacity_enable = true;
    #ifdef LCD_MAX_OPACITY
        layer_info->opacity_value = opacity_value * LCD_MAX_OPACITY / 255;  /* shift 3, hw only support 5 bit */
    #else 
        layer_info->opacity_value = opacity_value;
    #endif
        return GDI_SUCCEED;
    }

    if (opacity_enable && opacity_value == 255)
    {
        opacity_enable = false;
    }
#endif

    layer_info->opacity_enable = (bool)opacity_enable;
#ifdef LCD_MAX_OPACITY
    layer_info->opacity_value = opacity_value * LCD_MAX_OPACITY / 255;  /* shift 3, hw only support 5 bit */
#else 
    layer_info->opacity_value = opacity_value;
#endif 

    return GDI_SUCCEED;
}

gdi_result_t gdi_layer_clear_nb_concurrent(gdi_layer_handle_t layer_handle, gdi_color_t bg_color)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct_t *layer = (gdi_layer_struct_t*)layer_handle;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    if (layer->bits_per_pixel == 16)
    {
        if (layer->cf == GDI_COLOR_FORMAT_UYVY422)
        {
            bg_color = 0x0080; /* Only support clear to black */
        }
    
        gdi_memset16((uint8_t*)layer->buf_ptr, (uint16_t) bg_color, layer->layer_size);
    }
#ifdef GDI_PRIMITIVE_24_BIT_SUPPORT
    else if (layer->bits_per_pixel == 24)
    {
        gdi_memset24((uint8_t*)layer->buf_ptr, (uint32_t) bg_color, layer->layer_size);
    }
#endif
    else if (layer->bits_per_pixel == 32)
    {
        gdi_memset32((uint8_t*)layer->buf_ptr, (uint32_t) bg_color, layer->layer_size);
    }
    else
    {
        int32_t	r_index;
    	for(r_index = 0 ; r_index <= (layer->height - 1); r_index++)
    	{
    		int32_t	h_index;
            
    		for(h_index = 0; h_index <= (layer->width - 1); h_index++)
    		{
    			gd_put_pixel_to_layer[layer->cf](layer_handle, h_index, r_index, bg_color);
    		}
    	}
    }
    return GDI_SUCCEED;
}

gdi_result_t gdi_layer_get_dimension_nb_concurrent(gdi_layer_handle_t layer_handle, int32_t *width, int32_t *height)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct_t *layer = (gdi_layer_struct_t*)layer_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *width = layer->width;
    *height = layer->height;
    return GDI_SUCCEED;
}

void gdi_draw_solid_rect_nb_concurrent(gdi_layer_handle_t layer_handle, int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t rect_color)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct_t *layer = (gdi_layer_struct_t*)layer_handle;
    int32_t	r_index;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if((x1 > x2) || 
       (y1 > y2) || 
       (x2 < layer->clipx1) || 
       (y2 < layer->clipy1) || 
       (x1 > layer->clipx2) || 
       (y1 > layer->clipy2)) 
    { 
        return; 
    }
    else
    {
        if(x1 < layer->clipx1)
        {
            x1 = layer->clipx1;
        }
        if(y1 < layer->clipy1)
        {
            y1 = layer->clipy1;
        }
        if(x2 > layer->clipx2)
        {
            x2 = layer->clipx2;
        }
        if(y2 > layer->clipy2)
        {
            y2 = layer->clipy2;
        }
    }
    
	for(r_index = y1 ; r_index <= y2; r_index++)
	{
		int32_t	h_index;
        
		for(h_index = x1; h_index <= x2; h_index++)
		{
			gd_put_pixel_to_layer[layer->cf](layer_handle, h_index, r_index, rect_color);
		}
	}
}

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_rotate_nb_concurrent
 * DESCRIPTION
 *  
 * PARAMETERS
 *  void
 * RETURNS
 *  
 *****************************************************************************/
uint8_t gdi_layer_get_rotate_nb_concurrent(gdi_layer_handle_t layer_handle)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct_t *layer = (gdi_layer_struct_t*)layer_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return layer->rotate_value;
}


/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_position
 * DESCRIPTION
 *  get currently active layer's position.
 * PARAMETERS
 *  offset_x        [OUT]       
 *  offset_y        [OUT]       
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
gdi_result_t gdi_layer_get_position_nb_concurrent(gdi_layer_handle_t layer_handle, int32_t *offset_x, int32_t *offset_y)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct_t *layer = (gdi_layer_struct_t*)layer_handle;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *offset_x = (int32_t) layer->offset_x;
    *offset_y = (int32_t) layer->offset_y;
    return GDI_SUCCEED;
}
