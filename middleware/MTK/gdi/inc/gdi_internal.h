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

#ifndef _GDI_INTERNAL_H_
#define _GDI_INTERNAL_H_

#define GDI_VERSION              0x06B
#undef  GDI_USING_LFS

#include "gdi_features.h"
#include "gdi_const.h"
#include "gdi_datatype.h"
#include "gdi.h"
#include <stdio.h>
//#define GDI_LOG printf
#define GDI_LOG(...)

//#define act_bit_per_pixel           bits_per_pixel
#define act_buf_ptr              buf_ptr
//#define act_pitch                width
//#define act_rows                 height

#define GDI_LAYER_EMPTY_HANDLE               (0)        /* handle 0 is empty handle, not used */

#ifdef GDI_USING_LAYER_ROTATE
#define GDI_LAYER_ROTATE_0             LCD_LAYER_ROTATE_NORMAL
#define GDI_LAYER_ROTATE_90            LCD_LAYER_ROTATE_90
#define GDI_LAYER_ROTATE_180           LCD_LAYER_ROTATE_180
#define GDI_LAYER_ROTATE_270           LCD_LAYER_ROTATE_270
#define GDI_LAYER_ROTATE_0_MIRROR      LCD_LAYER_MIRROR
#define GDI_LAYER_ROTATE_90_MIRROR     LCD_LAYER_MIRROR_ROTATE_90
#define GDI_LAYER_ROTATE_180_MIRROR    LCD_LAYER_MIRROR_ROTATE_180
#define GDI_LAYER_ROTATE_270_MIRROR    LCD_LAYER_MIRROR_ROTATE_270
#else /* GDI_USING_LAYER_ROTATE */
#define GDI_LAYER_ROTATE_0             0
#define GDI_LAYER_ROTATE_90            1
#define GDI_LAYER_ROTATE_180           2
#define GDI_LAYER_ROTATE_270           3
#define GDI_LAYER_ROTATE_0_MIRROR      4
#define GDI_LAYER_ROTATE_90_MIRROR     5
#define GDI_LAYER_ROTATE_180_MIRROR    6
#define GDI_LAYER_ROTATE_270_MIRROR    7
#endif /* GDI_USING_LAYER_ROTATE */

#define GDI_MAINLCD_BIT_PER_PIXEL                  16
/*
 * In current design, GDI_MAINLCD_BIT_PER_PIXEL is always 16.
 *
 * Assume MMI_MAIN_BASE_LAYER_BITS_PER_PIXEL == 24 bit, then
 * GDI_MAINLCD_BIT_PER_PIXEL != MMI_MAIN_BASE_LAYER_BITS_PER_PIXEL.
 * We need to convert cache buffer (16-bit) to base layer (24-bit) in some applications
 */
#if defined(GDI_USING_LAYER_COLOR_FORMAT) && (GDI_MAINLCD_BIT_PER_PIXEL != MMI_MAIN_BASE_LAYER_BITS_PER_PIXEL)
    #if MMI_MAIN_BASE_LAYER_BITS_PER_PIXEL==24
        #define  GDI_SET_BUFFER_PIXEL2(X,Y,VALUE)    gdi_act_put_pixel(X,Y,gdi_rgb565_to_rgb888(VALUE));
    #elif MMI_MAIN_BASE_LAYER_BITS_PER_PIXEL==32
        #define  GDI_SET_BUFFER_PIXEL2(X,Y,VALUE)    gdi_act_put_pixel(X,Y,gdi_rgb565_to_rgb888(VALUE)|0xff000000);
    #elif MMI_MAIN_BASE_LAYER_BITS_PER_PIXEL==16
        #define  GDI_SET_BUFFER_PIXEL2               GDI_SET_BUFFER_PIXEL
    #else
        #error "didn't support this color format!!"
    #endif
#else
#define  GDI_SET_BUFFER_PIXEL2               GDI_SET_BUFFER_PIXEL
#endif

#define  GDI_SET_BUFFER_PIXEL(X,Y,VALUE)     gdi_act_put_pixel(X,Y,VALUE)
#define  GDI_GET_BUFFER_PIXEL(X,Y,VALUE)     VALUE=gdi_act_get_pixel(X,Y)


#define GDI_COLOR_TRANSPARENT    gdi_act_color_from_rgb(0, 0, 0, 255)

typedef enum
{
    GDI_IMAGE_SOURCE_RESOURCE,
    GDI_IMAGE_SOURCE_MEMORY,
    GDI_IMAGE_SOURCE_RESOURCE_WITHOUT_HEADER
} gdi_image_source_t;

typedef enum
{
    GDI_GRADIENT_RECT_STYLE_HOR = 0,
    GDI_GRADIENT_RECT_STYLE_VER
} gdi_gradient_rect_style_t;

typedef struct
{
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
} gdi_rect_t;

typedef struct gdi_layer_struct
{
    uint8_t id;
    uint8_t flag;
    uint8_t clips_top;
    uint16_t width;
    uint16_t height;
    uint8_t cf;          /* real color format ( it will support LCD color format) */
    uint8_t vcf;         /* virtual color format (convert to standard color format) */
    uint8_t bits_per_pixel;
    uint8_t rotate_value;
    int32_t offset_x;
    int32_t offset_y;
    uint16_t blt_x;
    uint16_t blt_y;
    uint16_t blt_width;
    uint16_t blt_height; /* blt rect, the coordinate is offset from layer frame buffer */
    uint8_t *buf_ptr;
    int16_t clipx1;
    int16_t clipy1;
    int16_t clipx2;
    int16_t clipy2;
    gdi_rect_t clips[8];
    uint32_t layer_size; /* unit: bytes */
} gdi_layer_struct_t;

extern gdi_layer_struct_t GDI_LAYERS[];
#define GDI_LAYER (*gdi_act_layer)
#define GDI_LAYER_MAIN_BASE_LAYER_HANDLE     ((gdi_layer_handle_t)(&GDI_LAYERS[0]))
extern gdi_layer_struct_t *gdi_act_layer;
extern gd_put_pixel_func gdi_act_put_pixel;
extern gd_get_pixel_func gdi_act_get_pixel;
/* -------------------extern function for color------------------------------ */
extern gd_color_from_rgb_func gdi_act_color_from_rgb;             /**< Generate color from r, g, b. */
extern gd_color_to_rgb_func gdi_act_color_to_rgb;                 /**< Extract r, g, b from given color. */
typedef enum
{
    GDI_INT_MEMORY_TYPE,
    GDI_EXT_MEMORY_TYPE_NONCACHEABLE,
    GDI_EXT_MEMORY_TYPE_CACHEABLE,
    GDI_EXT_MEMORY_TYPE_FRAMEBUFFER,
    GDI_EXT_MEMORY_TYPE_COUNT
} gdi_memory_type_enum;

extern void *gdi_alloc_ext_mem_ext(int32_t size, uint8_t location);
extern void gdi_free_ext_mem_ext(void **pointer);
#define gdi_alloc_ext_mem(size)                 gdi_alloc_ext_mem_ext(size,GDI_EXT_MEMORY_TYPE_NONCACHEABLE)
#define gdi_alloc_ext_mem_cacheable(size)       gdi_alloc_ext_mem_ext(size,GDI_EXT_MEMORY_TYPE_CACHEABLE)
#define gdi_alloc_ext_mem_framebuffer(size)     gdi_alloc_ext_mem_ext(size,GDI_EXT_MEMORY_TYPE_FRAMEBUFFER)
#define gdi_free_ext_mem(pointer)               gdi_free_ext_mem_ext(pointer)

#include "gdi_primitive.h"
#include "gdi_lcd.h"
#include "gdi_layer.h"

#define GDI_RETURN_TYPE(T,X)                 do{T x=X;GDI_UNLOCK; return x;}while(0)
#define GDI_RETURN(X)                  do{gdi_result_t x=X;GDI_UNLOCK; return x;}while(0)
#define GDI_RETURN_VOID             do{GDI_UNLOCK; return ;}while(0)
#define GDI_ENTER_CRITICAL_SECTION(func)  GDI_LOCK; do{
#define GDI_EXIT_CRITICAL_SECTION(func)   }while(0); GDI_UNLOCK;

#define GDI_TRACE(...)
#define GDI_BRIEF_TRACE

///////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MUTEX API
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef GDI_USING_MUTEX
#define GDI_LOCK     gdi_fast_mutex_lock()
#define GDI_LOCK_NO_WAIT    gdi_fast_mutex_lock_no_wait()
#define GDI_UNLOCK      gdi_fast_mutex_unlock()
#define GDI_UNLOCK_N(N) while(N< gdi_mutex.cnt) {gdi_fast_mutex_unlock();}
#else /* GDI_USING_MUTEX */
#define GDI_LOCK
#define GDI_UNLOCK
#define GDI_UNLOCK_N(N)
#endif /* GDI_USING_MUTEX */

/* DOM-NOT_FOR_SDK-END */
/*****************************************************************************
 * FUNCTION
 *  gdi_fast_mutex_lock
 * DESCRIPTION
 *  lock gdi mutex
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_fast_mutex_lock(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_fast_mutex_unlock
 * DESCRIPTION
 *  unlock gdi mutex
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_fast_mutex_unlock(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_fast_mutex_lock_no_wait
 * DESCRIPTION
 *  the same with gdi_fast_mutex_lock
 *  but it won't wait the mutex if it can't get the mutex immediately
 * PARAMETERS
 *  void
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
extern gdi_result_t gdi_fast_mutex_lock_no_wait(void);
extern int32_t gdi_fast_mutex_get_count(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_rgb565_to_rgb888
 * DESCRIPTION
 *  convert RGB565 to RGB888
 * PARAMETERS
 *  p       : [IN]    RGB565 pixel
 * RETURNS
 *  uint32_t, 24-bit RGB888 pixel
 *****************************************************************************/
extern gdi_color_t gdi_rgb565_to_rgb888(gdi_color_t p);
/**
 * @brief This function draws a dotted line with specified "bit_value" on the active layer.
 * @param[in] x1              is the x position of the first end point of line.
 * @param[in] y1              is the y position of the first end point of line.
 * @param[in] x2              is the x position of the other end point of line.
 * @param[in] y2              is the y position of the other end point of line.
 * @param[in] color           is the color of the line.
 * @param[in] cycle           is the size of bit_value.
 * @param[in] bit_value       is an array represent the dotted style.
 * @return                    void.
 */
void gdi_draw_line_style(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                         gdi_color_t color, uint8_t cycle, const uint8_t *bit_value);
/**
 * @brief This function draws an anti aliasing line on the active layer.
 * @param[in] x1              is the x position of the first end point of line.
 * @param[in] y1              is the y position of the first end point of line.
 * @param[in] x2              is the x position of the other end point of line.
 * @param[in] y2              is the y position of the other end point of line.
 * @param[in] color           is the color of the line.
 * @return                    void.
 */
void gdi_draw_antialiasing_line(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t color);
/*****************************************************************************
 * FUNCTION
 *  gdi_fill_dot_rect
 * DESCRIPTION
 *  draw a dotted rectangle
 * PARAMETERS
 *  x1                : [IN]   x of left-top cornor of rectangle
 *  y1                : [IN]   y of left-top cornor of rectangle
 *  x2                : [IN]   x of right-down cornor of rectangle
 *  y2                : [IN]   y of right-down cornor of rectangle
 *  c                 [IN]the color of the rectangle
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_fill_dot_rect(int32_t x1,int32_t y1,int32_t x2,int32_t y2,gdi_color_t c);
/**
 * @brief This function draws a rectangle with frame on active layer.
 * @param[in] x1              is x of left-top corner of the rectangle.
 * @param[in] y1              is y of left-top corner of the rectangle.
 * @param[in] x2              is x of right-down corner of the rectangle.
 * @param[in] y2              is y of right-down corner of the rectangle.
 * @param[in] color           is the color of the rectangle.
 * @param[in] frame_color     is the color of the rectangle frame.
 * @param[in] frame_width     is the width of the rectangle frame border.
 * @return                    void.
 */
void gdi_draw_round_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                         gdi_color_t color, gdi_color_t frame_color, uint16_t frame_width);
/*****************************************************************************
 * FUNCTION
 *  gdi_effect_alpha_blending_rect
 * DESCRIPTION
 *  do alpha blending between the src_layer and transparent_color, then output to active layer
 * PARAMETERS
 *  src_layer_handle          : [IN]   layer handle of alpha blending source layer
 *  x1                        : [IN]   x of left-top cornor of blending region in source layer
 *  y1                        : [IN]   y of left-top cornor of blending region in source layer
 *  x2                        : [IN]   x of right-down cornor of blending region in source layer
 *  y2                        : [IN]   y of right-down cornor of blending region in source layer
 *  transparent_color_r       : [IN]   red of transparent_color
 *  transparent_color_g       : [IN]   green of transparent_color
 *  transparent_color_b       : [IN]   blue of transparent_color
 * RETURNS
 *  void
 *****************************************************************************/
 extern void gdi_effect_alpha_blending_rect(gdi_layer_handle_t src_layer_handle, int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                uint8_t transparent_color_a, uint8_t transparent_color_r, uint8_t transparent_color_g, uint8_t transparent_color_b);
/**
 * @brief This function creates a layer using buffer in parameter.
 * @param[in] color_format    is the color format of the layer.
 * @param[in] offset_x        is the x position of the layer.
 * @param[in] offset_y        is the y position of the layer.
 * @param[in] width           is the width of the layer.
 * @param[in] height          is the height of the layer.
 * @param[in] buffer          is the buffer used to create layer.
 * @param[in] buffer_size     is the size of the buffer.
 * @param[out] handle         is the handle of the created layer.
 * @return                    whether the operation is successful.
 */
gdi_result_t gdi_layer_create_using_outside_memory(
    gdi_color_format_t color_format,
    int32_t offset_x,
    int32_t offset_y,
    int32_t width,
    int32_t height,
    uint8_t *buffer,
    int32_t buffer_size,
    gdi_layer_handle_t *handle);
/**
 * @brief This function frees the resource of a layer.
 * @param[in] handle          is the layer to free.
 * @return                    void.
 */
gdi_result_t gdi_layer_free(gdi_layer_handle_t handle);
/**
 * @brief This function gets handle of base layer.
 * @param[out] handle     is the base layer handle.
 * @return                whether the operation is successful.
 */
gdi_result_t gdi_layer_get_base_handle(gdi_layer_handle_t *handle);
/**
 * @brief This function restores base layer as active layer.
 * @return                    void.
 */
void gdi_layer_restore_base_active(void);
/**
 * @brief This function pushes current layer into stack and set it as active layer.
 * @param[in] handle          is the layer to be set as active layer.
 * @return                    void.
 */
void gdi_layer_push_and_set_active(gdi_layer_handle_t handle);
/**
 * @brief This function pops the layer in stack and make it active.
 * @return                    void.
 */
void gdi_layer_pop_and_restore_active(void);
/**
 * @brief This function moves the active layer to the desired position.
 * @param[in] offset_x        is the x position to move the active layer.
 * @param[in] offset_y        is the x position to move the active layer.
 * @return                    whether the operation is successful.
 */
gdi_result_t gdi_layer_set_position(int32_t offset_x, int32_t offset_y);
/**
 * @brief This function sets opacity value and enable opacity of the active layer.
 * @param[in] opacity_enable     is whether to enable the opacity of the active layer.
 * @param[in] opacity_value      is the opacity value of active layer.
 * @return                       whether the operation is successful.
 */
gdi_result_t gdi_layer_set_opacity(bool opacity_enable, uint8_t opacity_value);
/**
 * @brief This function gets opacity value and enable opacity of the active layer.
 * @param[out] opacity_enable     is whether the opacity of the active layer is enabled.
 * @param[out] opacity_value      is the opacity value of active layer.
 * @return                        whether the operation is successful.
 */
gdi_result_t gdi_layer_get_opacity(bool *opacity_enable, uint8_t *opacity_value);
/**
 * @brief This function gets currently active layer's size.
 * @param[out] width       is the width of active layer.
 * @param[out] height      is the height of active layer.
 * @return                 whether the operation is successful.
 */
gdi_result_t gdi_layer_get_dimension(uint32_t *width, uint32_t *height);
/**
 * @brief This function gets currently active layer's position.
 * @param[out] offset_x      is the position x of active layer.
 * @param[out] offset_y      is position y of active layer.
 * @return                   whether the operation is successful.
 */
gdi_result_t gdi_layer_get_position(int32_t *offset_x, int32_t *offset_y);
/**
 * @brief This function paints active layer with given color.
 * @param[in] color       is the color to paint the active layer.
 * @return                whether the operation is successful.
 */
gdi_result_t gdi_layer_clear(gdi_color_t color);
/**
 * @brief This function sets rotate value of active layer.
 * @param[in] rotate_value       is the rotate value.
 * @return                       void.
 */
void gdi_layer_set_rotate(uint8_t rotate_value);
/**
 * @brief This function gets dimension of current active LCD.
 * @param[out] width       is the LCD width.
 * @param[out] height      is the LCD height.
 * @return                 void.
 */
void gdi_lcd_get_dimension(uint32_t *width, uint32_t *height);
/**
 * @brief This function BLT layers to LCD in the order that handle0 is the most bottom layer. The last four parameters are the region which to BLT to LCD.
 * @param[in] handle0         is layer handle 0, the most bottom layer.
 * @param[in] handle1         is layer handle 1.
 * @param[in] handle2         is layer handle 2.
 * @param[in] handle3         is layer handle 3.
 * @param[in] x1              is x of left-top corner of BLT region.
 * @param[in] y1              is y of left-top corner of BLT region.
 * @param[in] x2              is x of right-down corner of BLT region.
 * @param[in] y2              is y of right-down corner of BLT region.
 * @return                    whether the operation is successful.
 */
gdi_result_t gdi_layer_blt(gdi_layer_handle_t handle0, gdi_layer_handle_t handle1,
                           gdi_layer_handle_t handle2, gdi_layer_handle_t handle3,
                           int32_t x1, int32_t y1, int32_t x2, int32_t y2);
/**
 * @brief This function BLT layers to LCD in the order that handle0 is the most bottom layer. The BLT region is the full LCD size.
 * @param[in] handle0         is layer handle 0, the most bottom layer.
 * @param[in] handle1         is layer handle 1.
 * @param[in] handle2         is layer handle 2.
 * @param[in] handle3         is layer handle 3.
 * @return                    whether the operation is successful.
 */
gdi_result_t gdi_lcd_blt(gdi_layer_handle_t handle0, gdi_layer_handle_t handle1,
                         gdi_layer_handle_t handle2, gdi_layer_handle_t handle3);
/**
 * @brief This function BLT the previous BLT layers to LCD. The four parameters are the region to BLT to LCD.
 * @param[in] x1              is x of left-top corner of BLT region.
 * @param[in] y1              is y of left-top corner of BLT region.
 * @param[in] x2              is x of right-down corner of BLT region.
 * @param[in] y2              is y of right-down corner of BLT region.
 * @return                    whether the operation is successful.
 */
gdi_result_t gdi_lcd_repaint(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
/**
 * @brief This function BLT the previous BLT layers to LCD. The BLT region is the full LCD.
 * @return                    whether the operation is successful.
 */
gdi_result_t gdi_lcd_repaint_all(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_create_cf_double
 * DESCRIPTION
 *  Create a double buffer layer
 *  Will return handle of the layer.
 * PARAMETERS
 *  cf               : [IN]   layer color format
 *  offset_x         : [IN]   layer position x
 *  offset_y         : [IN]   layer position y
 *  width            : [IN]   layer width
 *  height           : [IN]   layer height
 *  handle_ptr       : [OUT]  return the created layer handle
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_create_cf_double(
                    gdi_color_format_t cf,
                    int32_t offset_x,
                    int32_t offset_y,
                    int32_t width,
                    int32_t height,
                    gdi_layer_handle_t *handle_ptr);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_create_cf_double_using_outside_memory
 * DESCRIPTION
 *  Create a double buffer layer using buffer in parameter.
 *  Will return handle of the layer.
 * PARAMETERS
 *  cf                      : [IN]   layer color format
 *  offset_x                : [IN]   layer position x
 *  offset_y                : [IN]   layer position y
 *  width                   : [IN]   layer width
 *  height                  : [IN]   layer height
 *  handle_ptr              : [OUT]  return the created layer handle
 *  outside_memory1         : [IN]   the buffer1 used to create layer
 *  outside_memory_size_1   : [IN]   size of outside_memory1
 *  outside_memory2         : [IN]   the buffer2 used to create layer
 *  outside_memory_size_2   : [IN]   size of outside_memory2
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_create_cf_double_using_outside_memory(
                    gdi_color_format_t cf,
                    int32_t offset_x,
                    int32_t offset_y,
                    int32_t width,
                    int32_t height,
                    gdi_layer_handle_t *handle_ptr,
                    uint8_t *outside_memory1,
                    int32_t outside_memory_size_1,
                    uint8_t *outside_memory2,
                    int32_t outside_memory_size_2);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_lazy_free
 * DESCRIPTION
 *  Release layer
 *  Lazy free means we won't really realease this layer until we don't have enougn layer buffer.
 *  return FALSE if not valid handle.
 * PARAMETERS
 *  handle      : [IN]
 * RETURNS
 * bool
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_lazy_free(gdi_layer_handle_t handle,void (*lazy_free_callback)(gdi_layer_handle_t handle));
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_is_double
 * DESCRIPTION
 *  return TRUE if the layer is a double buffer layer
 * PARAMETERS
 *  layer       : [IN]        the layer handle
 * RETURNS
 *  bool, return TRUE if the layer is a double buffer layer
 *****************************************************************************/
extern bool gdi_layer_is_double(gdi_layer_handle_t layer);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_frame_buffer_lock_count
 * DESCRIPTION
 *  get the frame buffer blt_lock count
 * PARAMETERS
 *  frame_buffer_lock_count : [OUT]  frame buffer lock count
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_get_frame_buffer_lock_count(int32_t *frame_buffer_lock_count);
extern gdi_result_t gdi_layer_test_set_clip(gdi_rect_t *rect);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_clip_preset
 * DESCRIPTION
 *  Set clip region.
 *  The new clip region will be the intersection of original clip and clip in parameters.
 * PARAMETERS
 *  x1                  : [IN]        x position of left-up corner of cilp region
 *  y1                  : [IN]        y position of left-up corner of cilp region
 *  x2                  : [IN]        x position of right-down corner of cilp region
 *  y2                  : [IN]        y position of right-down corner of cilp region
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_set_clip_preset(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_blt_base_layer
 * DESCRIPTION
 *  Blt base layer to LCD
 * PARAMETERS
 *  x1                : [IN]   x of left-top cornor of blt region
 *  y1                : [IN]   y of left-top cornor of blt region
 *  x2                : [IN]   x of right-down cornor of blt region
 *  y2                : [IN]   y of right-down cornor of blt region
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
extern gdi_result_t gdi_layer_blt_base_layer(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_blt_layer_previous
 * DESCRIPTION
 *  config the settings of previous blt layers to LCD driver.
 * PARAMETERS
 *
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
extern gdi_result_t gdi_layer_set_blt_layer_previous(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_blt_layer_ext
 * DESCRIPTION
 *  Set blt layers to GDI and config driver's setting
 * PARAMETERS
 *  handle0     : [IN]        layer handle 0, the most bottom layer
 *  handle1     : [IN]        layer handle 1
 *  handle2     : [IN]        layer handle 2
 *  handle3     : [IN]        layer handle 3
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_set_blt_layer_ext(gdi_layer_handle_t handle0, gdi_layer_handle_t handle1, gdi_layer_handle_t handle2, gdi_layer_handle_t handle3);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_blt_layer_ext
 * DESCRIPTION
 *  get the blt layer
 * PARAMETERS
 *  handle0     : [OUT]        layer handle 0, the most bottom layer
 *  handle1     : [OUT]        layer handle 1
 *  handle2     : [OUT]        layer handle 2
 *  handle3     : [OUT]        layer handle 3
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_get_blt_layer_ext(gdi_layer_handle_t *handle0, gdi_layer_handle_t *handle1, gdi_layer_handle_t *handle2, gdi_layer_handle_t *handle3);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_blt_layer_number
 * DESCRIPTION
 *  get how many layers that hw can blt or flatten at one time.
 * PARAMETERS
 *  void
 * RETURNS
 *  int32_t
 *****************************************************************************/
extern uint8_t gdi_layer_get_blt_layer_number(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_blt_layer_flag
 * DESCRIPTION
 *  MDI needs to set blt layer flag when they set hw update layer.
 *  So they have to get blt layer flag from gdi.
 *  When "blt with double buffer" is enable, the real blt layer is in gdi_blt_with_double_buf_blt_handle[].
 * PARAMETERS
 *  blt_layer_flag  [OUT]  return the blt_layer_flag
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
extern gdi_result_t gdi_layer_get_blt_layer_flag(uint32_t *blt_layer_flag);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_lock_frame_buffer
 * DESCRIPTION
 *  Lock blt. Avoid others to blt this layer.
 *  And merge the blt region, so gdi will blt all region after unlock frame buffer.
 *  remeber to call gdi_layer_unlock_frame_buffer after this function.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_lock_frame_buffer(void);

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_unlock_frame_buffer
 * DESCRIPTION
 *  Unlock layer. Let others able to blt this layer.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_unlock_frame_buffer(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_flatten_previous_to_base
 * DESCRIPTION
 *  flatten previous blt layers to base layer
 * PARAMETERS
 *  void
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_flatten_previous_to_base(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_flatten_is_ready
 * DESCRIPTION
 *  Check if flatten is available or not.
 *  When multimedia is running, using LCD to flatten is not allowed.
 *  But we can use GOVL to do it, so it always return true on MT6236.
 * PARAMETERS
 * RETURNS
 *  bool
 *****************************************************************************/
extern bool gdi_layer_flatten_is_ready(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_flatten_to_base_ext
 * DESCRIPTION
 *  flatten layers to base layer
 * PARAMETERS
 *  handle0     : [IN]        layer handle 0, the most bottom layer
 *  handle1     : [IN]        layer handle 1
 *  handle2     : [IN]        layer handle 2
 *  handle3     : [IN]        layer handle 3
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_flatten_to_base_ext(gdi_layer_handle_t src_handle0, gdi_layer_handle_t src_handle1, gdi_layer_handle_t src_handle2, gdi_layer_handle_t src_handle3);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_flatten_ext
 * DESCRIPTION
 *  flatten the specified layers to active layer.
 * PARAMETERS
 *  handle0     : [IN]        layer handle 0, the most bottom layer
 *  handle1     : [IN]        layer handle 1
 *  handle2     : [IN]        layer handle 2
 *  handle3     : [IN]        layer handle 3
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_flatten_ext(gdi_layer_handle_t handle0, gdi_layer_handle_t handle1, gdi_layer_handle_t handle2, gdi_layer_handle_t handle3);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_flatten_with_clipping_ext
 * DESCRIPTION
 *  flatten layers to active layer in active layer's clipping region
 * PARAMETERS
 *  handle0     : [IN]        layer handle 0, the most bottom layer
 *  handle1     : [IN]        layer handle 1
 *  handle2     : [IN]        layer handle 2
 *  handle3     : [IN]        layer handle 3
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_flatten_with_clipping_ext(gdi_layer_handle_t handle0, gdi_layer_handle_t handle1, gdi_layer_handle_t handle2, gdi_layer_handle_t handle3);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_bit_per_pixel
 * DESCRIPTION
 *  get the active layer pixel size (in bits)
 * PARAMETERS
 *  void
 * RETURNS
 *  int, bit_per_pixel of layer
 *****************************************************************************/
extern int gdi_layer_get_bit_per_pixel(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_enable_non_block_blt
 * DESCRIPTION
 *  enable non blocking blt call.
 *  After calling this API, blt will become non-blocking blt when all layers in draw section.
 *  DEPRECATED: Please use gdi_layer_blt_with_flag instead
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_enable_non_block_blt(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_disable_non_block_blt
 * DESCRIPTION
 *  disable non blocking blt call
 *  DEPRECATED: Please use gdi_layer_blt_with_flag instead
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_disable_non_block_blt(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_enable_non_block_blt
 * DESCRIPTION
 *  get gdi_is_non_block_blt
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern bool gdi_layer_get_enable_non_block_blt(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_background
 * DESCRIPTION
 *  set the layer background color
 * PARAMETERS
 *  color       : [IN]        layer background color
 * RETURNS
 *  bool, return TRUE if succeed
 *****************************************************************************/
extern bool gdi_layer_set_background(gdi_color_t color);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_background
 * DESCRIPTION
 *  get the layer background color
 * PARAMETERS
 *  void
 * RETURNS
 *  gdi_color_t, background color
 *****************************************************************************/
extern gdi_color_t gdi_layer_get_background(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_create_cf
 * DESCRIPTION
 *  creat a layer
 * PARAMETERS
 *  cf               : [IN]   layer color format
 *  offset_x         : [IN]   layer position x
 *  offset_y         : [IN]   layer position y
 *  width            : [IN]   layer width
 *  height           : [IN]   layer height
 *  handle_ptr       : [OUT]  return the created layer handle
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_create_cf(gdi_color_format_t cf, int32_t offset_x, int32_t offset_y, int32_t width, int32_t height, gdi_layer_handle_t *handle_ptr);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_create
 * DESCRIPTION
 *  creat a layer
 * PARAMETERS
 *  OFFSET_X         : [IN]   layer position x
 *  OFFSET_Y         : [IN]   layer position y
 *  WIDTH            : [IN]   layer width
 *  HEIGHT           : [IN]   layer height
 *  HANDLE_PTR       : [OUT]  return the created layer handle
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_create(
            int32_t offset_x,
            int32_t offset_y,
            int32_t width,
            int32_t height,
            gdi_layer_handle_t *handle_ptr);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_create_double
 * DESCRIPTION
 *  Create a double buffer layer
 *  Will return handle of the layer.
 *  If no valid layer, will retrun GDI_LAYER_ERROR_HANDLE.
 * PARAMETERS
 *  OFFSET_X         : [IN]   layer position x
 *  OFFSET_Y         : [IN]   layer position y
 *  WIDTH            : [IN]   layer width
 *  HEIGHT           : [IN]   layer height
 *  HANDLE_PTR       : [OUT]  return the created layer handle
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_layer_create_double(OFFSET_X, OFFSET_Y,WIDTH,HEIGHT,HANDLE_PTR) \
      gdi_layer_create_cf_double(GDI_LCD->cf,OFFSET_X,OFFSET_Y,WIDTH,HEIGHT,HANDLE_PTR)
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_create_double_using_outside_memory
 * DESCRIPTION
 *  Create a double buffer layer using buffer in parameter.
 *  Will return handle of the layer.
 * PARAMETERS
 *  X                : [IN]   layer position x
 *  Y                : [IN]   layer position y
 *  WIDTH            : [IN]   layer width
 *  HEIGHT           : [IN]   layer height
 *  HANDLE_PTR       : [OUT]  return the created layer handle
 *  OUTMEM_PTR       : [IN]   the buffer used to create layer
 *  OUTMEM_SIZE      : [IN]   size of outside_memory, should be (w*h*cf*2) for double buffer
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_layer_create_double_using_outside_memory(X,Y, WIDTH,HEIGHT,HANDLE_PTR,OUTMEM_PTR,OUTMEM_SIZE) \
         gdi_layer_create_cf_double_using_outside_memory                                                  \
            (GDI_LCD->cf,X,Y,WIDTH,HEIGHT,HANDLE_PTR,                                                     \
             OUTMEM_PTR,(OUTMEM_SIZE)>>1,OUTMEM_PTR+(OUTMEM_SIZE>>1),(OUTMEM_SIZE)>>1)

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_flatten_to_base
 * DESCRIPTION
 *  flatten layers to base layer
 * PARAMETERS
 *  handle0     : [IN]        layer handle 0, the most bottom layer
 *  handle1     : [IN]        layer handle 1
 *  handle2     : [IN]        layer handle 2
 *  handle3     : [IN]        layer handle 3
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_flatten_to_base(
            gdi_layer_handle_t src_handle0,
            gdi_layer_handle_t src_handle1,
            gdi_layer_handle_t src_handle2,
            gdi_layer_handle_t src_handle3);

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_flatten
 * DESCRIPTION
 *  flatten the specified layers to active layer.
 * PARAMETERS
 *  handle0     : [IN]        layer handle 0, the most bottom layer
 *  handle1     : [IN]        layer handle 1
 *  handle2     : [IN]        layer handle 2
 *  handle3     : [IN]        layer handle 3
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_flatten(
                    gdi_layer_handle_t handle0,
                    gdi_layer_handle_t handle1,
                    gdi_layer_handle_t handle2,
                    gdi_layer_handle_t handle3);
#define gdi_layer_set_blt_layer_nb_concurrent(H1,H2,H3,H4)      gdi_layer_set_blt_layer_nb_concurrent_ext(H1,H2,H3,H4)
#define gdi_layer_blt_nb_concurrent(H1,H2,H3,H4,X1,Y1,X2,Y2)    gdi_layer_blt_nb_concurrent_ext(H1,H2,H3,H4,X1,Y1,X2,Y2)

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_flatten_with_clipping
 * DESCRIPTION
 *  flatten layers to active layer in active layer's clipping region
 * PARAMETERS
 *  handle0     : [IN]        layer handle 0, the most bottom layer
 *  handle1     : [IN]        layer handle 1
 *  handle2     : [IN]        layer handle 2
 *  handle3     : [IN]        layer handle 3
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_layer_flatten_with_clipping(H1,H2,H3,H4)  gdi_layer_flatten_with_clipping_ext(H1,H2,H3,H4)
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_blt_previous_nb_concurrent
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
extern gdi_result_t gdi_layer_blt_previous_nb_concurrent(int32_t x1, int32_t y1, int32_t x2, int32_t y2);

extern gdi_result_t gdi_layer_set_position_nb_concurrent(gdi_layer_handle_t layer_handle, int32_t offset_x, int32_t offset_y);
extern gdi_result_t gdi_layer_set_rotate_nb_concurrent(gdi_layer_handle_t layer_handle, uint8_t rotate_value);
extern gdi_result_t gdi_layer_set_source_key_nb_concurrent(
            gdi_layer_handle_t layer_handle,
            bool source_key_enable,
            gdi_color_t source_key_value);
extern gdi_result_t gdi_layer_set_opacity_nb_concurrent(gdi_layer_handle_t layer_handle, bool opacity_enable, uint8_t opacity_value);
extern gdi_result_t gdi_layer_set_background_nb_concurrent(gdi_layer_handle_t layer_handle, gdi_color_t color);
extern gdi_result_t gdi_layer_resize_nb_concurrent(gdi_layer_handle_t layer_handle, int32_t width, int32_t height);
extern gdi_result_t gdi_layer_clear_nb_concurrent(gdi_layer_handle_t layer_handle, gdi_color_t bg_color);
extern gdi_result_t gdi_layer_get_dimension_nb_concurrent(gdi_layer_handle_t layer_handle, int32_t *width, int32_t *height);
extern void gdi_draw_solid_rect_nb_concurrent(gdi_layer_handle_t layer_handle, int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_color_t rect_color);
extern gdi_result_t gdi_layer_get_position_nb_concurrent(gdi_layer_handle_t layer_handle, int32_t *offset_x, int32_t *offset_y);
extern uint8_t gdi_layer_get_rotate_nb_concurrent(gdi_layer_handle_t layer_handle);
/*****************************************************************************
 * FUNCTION
 *  gdi_rotate_map_layer_to_lcd_region
 * DESCRIPTION
 *  map a region from layer to lcd coordinate
 *  input layer coordinate, output lcd coordinate.
 * PARAMETERS
 *  rotate_value       : [IN]     rotate value of lcd
 *  x1                 : [OUT]    the x position of left-up corner of the region
 *  y1                 : [OUT]    the y position of left-up corner of the region
 *  x2                 : [OUT]    the x position of right-down corner of the region
 *  y2                 : [OUT]    the y position of right-down corner of the region
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_rotate_map_layer_to_lcd_region(uint8_t rotate_value, int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2);
/*****************************************************************************
 * FUNCTION
 *  gdi_rotate_map_layer_to_lcd
 * DESCRIPTION
 *  map a point from layer to lcd coordinate
 *  input layer coordinate, output lcd coordinate.
 * PARAMETERS
 *  rotate_value        : [IN]        rotate value of lcd
 *  x                   : [OUT]       the x position of the point
 *  y                   : [OUT]       the y position of the point
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_rotate_map_layer_to_lcd(uint8_t rotate_value, int32_t *x, int32_t *y);
/*****************************************************************************
 * FUNCTION
 *  gdi_rotate_map_lcd_to_layer
 * DESCRIPTION
 *  map a region from lcd to layer coordinate
 *  input lcd coordinate, output layer coordinate.
 * PARAMETERS
 *  rotate_value        : [IN]        rotate value of layer
 *  x                   : [OUT]       the x position of the point
 *  y                   : [OUT]       the y position of the point
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_rotate_map_lcd_to_layer(uint8_t rotate_value, int32_t *x, int32_t *y);
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_set_active
 * DESCRIPTION
 *  set a lcd active
 * PARAMETERS
 *  lcd_handle      : [IN]        lcd handle to set active
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_lcd_set_active(gdi_layer_handle_t lcd_handle);
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_get_active
 * DESCRIPTION
 *  get current active lcd's handle
 * PARAMETERS
 *  lcd_handle_ptr      : [OUT]       return active lcd handle
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_lcd_get_active(gdi_layer_handle_t *lcd_handle_ptr);
#define GDI_LCD_MAIN_LCD_HANDLE  (1)

extern gdi_lcd_cntx_struct *GDI_LCD;
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_freeze
 * DESCRIPTION
 *  set active lcd freezed or not, if freezed, the lcd won't be blt
 * PARAMETERS
 *  is_freeze       : [IN]        if TRUE, the lcd won't be blt
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_lcd_freeze(bool is_freeze);
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_get_freeze
 * DESCRIPTION
 *  get the state if lcd is freezed or not
 * PARAMETERS
 *  void
 * RETURNS
 *  bool, The acrive LCD is freezed or not
 *****************************************************************************/
extern bool gdi_lcd_get_freeze(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_rotate_map_lcd_to_absolute_hw_region
 * DESCRIPTION
 *  map a region from lcd to absolute hw coordinate
 *  input lcd coordinate, output lcd coordinate.
 * PARAMETERS
 *  rotate_value       : [IN]     rotate value of lcd
 *  x1                 : [OUT]    the x position of left-up corner of the region
 *  y1                 : [OUT]    the y position of left-up corner of the region
 *  x2                 : [OUT]    the x position of right-down corner of the region
 *  y2                 : [OUT]    the y position of right-down corner of the region
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_rotate_map_lcd_to_absolute_hw_region(uint8_t rotate_value, int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2);
/*****************************************************************************
 * FUNCTION
 *  gdi_rotate_map_absolute_hw_to_lcd_region
 * DESCRIPTION
 *  map a region from absolute hw to lcd coordinate
 *  input absolute hw coordinate, output lcd coordinate.
 * PARAMETERS
 *  rotate_value       : [IN]     rotate value of lcd
 *  x1                 : [OUT]    the x position of left-up corner of the region
 *  y1                 : [OUT]    the y position of left-up corner of the region
 *  x2                 : [OUT]    the x position of right-down corner of the region
 *  y2                 : [OUT]    the y position of right-down corner of the region
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_rotate_map_absolute_hw_to_lcd_region(uint8_t rotate_value, int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2);

/* DOM-NOT_FOR_SDK-END */

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_get_rotate
 * DESCRIPTION
 *  get the rotate value of active lcd
 * PARAMETERS
 *  void
 * RETURNS
 *  uint8_t, lcd rotate value, ex.GDI_LCD_LAYER_ROTATE_0,90,180...
 *****************************************************************************/
extern uint8_t gdi_lcd_get_rotate(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_set_rotate
 * DESCRIPTION
 *  set rotate value of active lcd
 * PARAMETERS
 *  rotate_value    : [IN]    rotate value, ex.GDI_LCD_LAYER_ROTATE_0,90,180...
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_lcd_set_rotate(uint8_t rotate_value);

/* DOM-NOT_FOR_SDK-BEGIN */

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_set_rotate
 * DESCRIPTION
 *  set rotate value of active lcd
 * PARAMETERS
 *  rotate_value    : [IN]    rotate value, ex.GDI_LCD_LAYER_ROTATE_0,90,180...
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_lcd_set_rotate(uint8_t rotate_value);
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_get_rotate
 * DESCRIPTION
 *  get the rotate value of active lcd
 * PARAMETERS
 *  void
 * RETURNS
 *  uint8_t, lcd rotate value, ex.GDI_LCD_LAYER_ROTATE_0,90,180...
 *****************************************************************************/
extern uint8_t gdi_lcd_get_rotate(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_set_rotate_by_layer
 * DESCRIPTION
 *  Implement lcd rotate by layer rotate.
 *  LCD rotate need hw support, so use layer rotate to implement it.
 * PARAMETERS
 *  rotate_by_layer     : [IN]        TRUE to enable rotate by layer
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_lcd_set_rotate_by_layer(bool rotate_by_layer);
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_set_rotate_by_layer
 * DESCRIPTION
 *  Implement lcd rotate by layer rotate.
 *  LCD rotate need hw support, so use layer rotate to implement it.
 * PARAMETERS
 *  rotate_by_layer     : [IN]        TRUE to enable rotate by layer
 * RETURNS
 *  void
 *****************************************************************************/
extern bool gdi_lcd_get_rotate_by_layer(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_rotate_map_absolute_hw_to_lcd
 * DESCRIPTION
 *  map a point from absolute hw to lcd coordinate
 *  input absolute hw coordinate, output lcd coordinate.
 * PARAMETERS
 *  x1                 : [OUT]    the x position of the point
 *  y1                 : [OUT]    the y position of the point
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_rotate_map_absolute_hw_to_lcd(int32_t *x, int32_t *y);
/*****************************************************************************
 * FUNCTION
 *  gdi_rotate_map_lcd_to_absolute_hw
 * DESCRIPTION
 *  map a region from lcd to absolute hw coordinate.
 *  Input the lcd coordinate, output the absolute hw coordinate.
 * PARAMETERS
 *  x1                 : [OUT]    the x position of the point
 *  y1                 : [OUT]    the y position of the point
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_rotate_map_lcd_to_absolute_hw(int32_t *x, int32_t *y);

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_set_te_ctrl
 * DESCRIPTION
 *  lcd API wrapper for application to set te ctrl.(tearing control)
 * PARAMETERS
 *  te_ctrl [IN] turn on or turn off lcd te ctrl.
 * RETURNS
 *  bool, return the previous setting of te ctrl
 *****************************************************************************/
extern bool gdi_lcd_set_te_ctrl(bool te_ctrl);
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_set_te_ctrl
 * DESCRIPTION
 *  lcd API wrapper for application to set te ctrl.(tearing control) for MATV
 * PARAMETERS
 *  te_ctrl [IN] turn on or turn off lcd te ctrl.
 * RETURNS
 *  bool, return the previous setting of te ctrl
 *****************************************************************************/
extern bool gdi_lcd_set_te_ctrl_with_commit(bool te_ctrl);
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_get_te_ctrl
 * DESCRIPTION
 *  lcd API wrapper for application to get te ctrl.(tearing control)
 * PARAMETERS
 *
 * RETURNS
 *  bool, return the current setting of te ctrl
 *****************************************************************************/
extern bool gdi_lcd_get_te_ctrl(void);

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_get_te_ctrl_handle
 * DESCRIPTION
 *  lcd API wrapper for application to set te ctrl.
 *  call this API before calling gdi_lcd_te_ctrl_push.
 * PARAMETERS
 *  void
 * RETURNS
 *  uint8_t te_ctrl_handle
 *****************************************************************************/
extern uint8_t gdi_lcd_get_te_ctrl_handle(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_te_ctrl_push
 * DESCRIPTION
 *  lcd API wrapper for application to set te ctrl.
 *  Set new setting and push previous setting to stack.
 * PARAMETERS
 *  on_Noff [IN] turn on or turn off lcd te ctrl.
 *  handle  [IN] handle get from gdi_lcd_get_te_ctrl_handle();
 * RETURNS
 *  bool
 *****************************************************************************/
extern bool gdi_lcd_te_ctrl_push(bool on_Noff, uint8_t handle);
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_te_ctrl_push
 * DESCRIPTION
 *  lcd API wrapper for application to set te ctrl.
 *  Pop and restore to previous lcd te ctrl setting.
 * PARAMETERS
 *  handle  [IN] handle get from gdi_lcd_get_te_ctrl_handle();
 * RETURNS
 *  bool
 *****************************************************************************/
extern bool gdi_lcd_te_ctrl_pop(uint8_t handle);

typedef struct
{
    int32_t width;
    int32_t height;
    uint8_t *buf_ptr;
} gdi_image_cache_bmp_struct;

/*****************************************************************************
 * FUNCTION
 *  gdi_image_cache_bmp_get
 * DESCRIPTION
 *  cache a region from a layer
 * PARAMETERS
 *  x1          : [IN]    left-up cornor x of destination region
 *  y1          : [IN]    left-up cornor y of destination region
 *  x2          : [IN]    right-down cornor x of destination region
 *  y2          : [IN]    right-down cornor y of destination region
 *  bmp_ptr     : [OUT]   return width, height, and pointer to cached region
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_image_cache_bmp_get(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_image_cache_bmp_struct *bmp_ptr);
/*****************************************************************************
 * FUNCTION
 *  gdi_image_cache_bmp_draw
 * DESCRIPTION
 *  draw cached image to a layer
 * PARAMETERS
 *  offset_x        : [IN]    position of layer want to draw to
 *  offset_y        : [IN]    position of layer want to draw to
 *  bmp_ptr         : [IN]    the bmp want to draw
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_image_cache_bmp_draw(int32_t offset_x, int32_t offset_y, gdi_image_cache_bmp_struct *bmp_ptr);

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_resized_frames
 * DESCRIPTION
 *  draw one frame in animation from resource with specified dimension
 * PARAMETERS
 *  OFFSET_X       : [IN]        x position to draw image
 *  OFFSET_Y       : [IN]        y position to draw image
 *  RESIZED_WIDTH  : [IN]        image width to draw
 *  RESIZED_HEIGHT : [IN]        image height to draw
 *  IMAGE_PTR      : [IN]        image pointer
 *  FRAME_NO       : [IN]        this is frame index to draw
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_image_draw_resized_frames(OFFSET_X,OFFSET_Y,RESIZED_WIDTH,RESIZED_HEIGHT,IMAGE_PTR,FRAME_NO) \
                    gdi_image_codec_draw(                       \
                                GDI_IMAGE_SRC_FROM_RESOURCE,    \
                                IMAGE_PTR,                      \
                                GDI_IMAGE_TYPE_INVALID,         \
                                0,                              \
                                OFFSET_X,                       \
                                OFFSET_Y,                       \
                                RESIZED_WIDTH,                  \
                                RESIZED_HEIGHT,                 \
                                0,                              \
                                (uint32_t) FRAME_NO)

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_file_frames
 * DESCRIPTION
 *  draw one frame in animation from file
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  IMAGE_NAME        : [IN]        image file name
 *  FRAME_NO          : [IN]        this is frame index to draw
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_image_draw_file_frames(OFFSET_X,OFFSET_Y,IMAGE_NAME,FRAME_NO) \
                    gdi_image_codec_draw(                       \
                                GDI_IMAGE_SRC_FROM_FILE,        \
                                (uint8_t*) IMAGE_NAME,               \
                                GDI_IMAGE_TYPE_INVALID,         \
                                0,                              \
                                OFFSET_X,                       \
                                OFFSET_Y,                       \
                                0,                              \
                                0,                              \
                                0,                              \
                                (uint32_t)FRAME_NO)

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_resized_file_frames
 * DESCRIPTION
 *  draw one frame in animation from file with specified dimension
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  RESIZED_WIDTH     : [IN]        image width to draw
 *  RESIZED_HEIGHT    : [IN]        image height to draw
 *  IMAGE_NAME        : [IN]        image file name
 *  FRAME_NO          : [IN]        this is frame index to draw
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_image_draw_resized_file_frames(OFFSET_X,OFFSET_Y,RESIZED_WIDTH,RESIZED_HEIGHT,IMAGE_NAME,FRAME_NO) \
                    gdi_image_codec_draw(                       \
                                GDI_IMAGE_SRC_FROM_FILE,        \
                                (uint8_t*) IMAGE_NAME,               \
                                GDI_IMAGE_TYPE_INVALID,         \
                                0,                              \
                                OFFSET_X,                       \
                                OFFSET_Y,                       \
                                RESIZED_WIDTH,                  \
                                RESIZED_HEIGHT,                 \
                                0,                              \
                                (uint32_t)FRAME_NO)

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_id_with_transparent_color
 * DESCRIPTION
 *  draw image from resource, the transparent color will be drawn if image type is gif.
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  IMAGE_ID          : [IN]        image resource id
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_image_draw_id_with_transparent_color(OFFSET_X,OFFSET_Y,IMAGE_ID) \
                    gdi_image_codec_draw(                       \
                                GDI_IMAGE_SRC_FROM_RESOURCE,    \
                                (uint8_t*) GetImage(IMAGE_ID),       \
                                GDI_IMAGE_TYPE_INVALID,         \
                                0,                              \
                                OFFSET_X,                       \
                                OFFSET_Y,                       \
                                0,                              \
                                0,                              \
                                GDI_IMAGE_CODEC_FLAG_DISABLE_SRC_KEY,\
                                0)
/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_id_with_transparent_color
 * DESCRIPTION
 *  draw image from resource, the transparent color will be drawn if image type is gif.
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  IMAGE_ID          : [IN]        image resource id
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_image_draw_resized_id_with_transparent_color(OFFSET_X,OFFSET_Y,RESIZED_WIDTH,RESIZED_HEIGHT,IMAGE_ID) \
                    gdi_image_codec_draw(                       \
                                GDI_IMAGE_SRC_FROM_RESOURCE,    \
                                (uint8_t*) GetImage(IMAGE_ID),       \
                                GDI_IMAGE_TYPE_INVALID,         \
                                0,                              \
                                OFFSET_X,                       \
                                OFFSET_Y,                       \
                                RESIZED_WIDTH,                              \
                                RESIZED_HEIGHT,                              \
                                GDI_IMAGE_CODEC_FLAG_DISABLE_SRC_KEY,\
                                0)

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_blend2layers
 * DESCRIPTION
 *  draw image from resource with original image dimension
 *  When drawing png and abm image,
 *  it use src layer color as alpha blending color only when the active layer color is transparent color.
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  IMAGE_PTR         : [IN]        image pointer
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_image_draw_blend2layers(
                    int32_t x,
                    int32_t y,
                    uint8_t *img_src);
/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_resized_blend2layers
 * DESCRIPTION
 *  draw image from resource with specified image dimension
 *  When drawing png and abm image,
 *  it use src layer color as alpha blending color only when the active layer color is transparent color.
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  RESIZED_WIDTH     : [IN]        image width to draw
 *  RESIZED_HEIGHT    : [IN]        image height to draw
 *  IMAGE_PTR         : [IN]        image pointer
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_image_draw_resized_blend2layers(
                    int32_t x,
                    int32_t y,
                    int32_t w,
                    int32_t h,
                    uint8_t *img_src);

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_resized_frames_blend2layers
 * DESCRIPTION
 *  draw one frame in animation from resource with specified dimension
 *  When drawing png and abm image,
 *  it use src layer color as alpha blending color only when the active layer color is transparent color.
 * PARAMETERS
 *  OFFSET_X       : [IN]        x position to draw image
 *  OFFSET_Y       : [IN]        y position to draw image
 *  RESIZED_WIDTH  : [IN]        image width to draw
 *  RESIZED_HEIGHT : [IN]        image height to draw
 *  IMAGE_PTR      : [IN]        image pointer
 *  FRAME_NO       : [IN]        this is frame index to draw
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_image_draw_resized_frames_blend2layers(OFFSET_X,OFFSET_Y,RESIZED_WIDTH,RESIZED_HEIGHT,IMAGE_PTR,FRAME_NO) \
                    gdi_image_codec_draw(                       \
                                GDI_IMAGE_SRC_FROM_RESOURCE,    \
                                IMAGE_PTR,                      \
                                GDI_IMAGE_TYPE_INVALID,         \
                                0,                              \
                                OFFSET_X,                       \
                                OFFSET_Y,                       \
                                RESIZED_WIDTH,                  \
                                RESIZED_HEIGHT,                 \
                                GDI_IMAGE_CODEC_FLAG_BLEND_2_LAYERS,                              \
                                (uint32_t) FRAME_NO)

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_id_blend2layers
 * DESCRIPTION
 *  draw image from resource with original image dimension
 *  When drawing png and abm image,
 *  it use src layer color as alpha blending color only when the active layer color is transparent color.
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  IMAGE_ID          : [IN]        image resource id
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_image_draw_id_blend2layers(OFFSET_X,OFFSET_Y,IMAGE_ID)           gdi_image_draw_blend2layers(OFFSET_X,OFFSET_Y,(uint8_t*) GetImage(IMAGE_ID))

/* DOM-NOT_FOR_SDK-BEGIN */

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_resized_id_blend2layers
 * DESCRIPTION
 *  draw image from resource with specified image dimension
 *  When drawing png and abm image,
 *  it use src layer color as alpha blending color only when the active layer color is transparent color.
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  RESIZED_WIDTH     : [IN]        image width to draw
 *  RESIZED_HEIGHT    : [IN]        image height to draw
 *  IMAGE_ID          : [IN]        image resource id
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_image_draw_resized_id_blend2layers(OFFSET_X,OFFSET_Y,RESIZED_WIDTH,RESIZED_HEIGHT,IMAGE_ID) \
                    gdi_image_draw_resized_blend2layers(OFFSET_X,OFFSET_Y,RESIZED_WIDTH,RESIZED_HEIGHT,(uint8_t*) GetImage(IMAGE_ID))

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_mem_blend2layers
 * DESCRIPTION
 *  draw image in memory with original image dimension
 *  When drawing png and abm image,
 *  it use src layer color as alpha blending color only when the active layer color is transparent color.
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  IMAGE_PTR         : [IN]        image pointer
 *  IMAGE_TYPE        : [IN]        image type, ex.GDI_IMAGE_TYPE_BMP...(defined in gdi_const.h)
 *  IMAGE_LENGTH      : [IN]        image size of IMAGE_PTR
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_image_draw_mem_blend2layers(
                    int32_t x,
                    int32_t y,
                    uint8_t *img_src,
                    uint8_t img_type,
                    int32_t img_size);
/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_resized_mem_blend2layers
 * DESCRIPTION
 *  draw image in memory with specified dimension
 *  When drawing png and abm image,
 *  it use src layer color as alpha blending color only when the active layer color is transparent color.
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  RESIZED_WIDTH     : [IN]        image width to draw
 *  RESIZED_HEIGHT    : [IN]        image height to draw
 *  IMAGE_PTR         : [IN]        image pointer
 *  IMAGE_TYPE        : [IN]        image type, ex.GDI_IMAGE_TYPE_BMP...(defined in gdi_const.h)
 *  IMAGE_LENGTH      : [IN]        image size of IMAGE_PTR
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_image_draw_resized_mem_blend2layers(OFFSET_X,OFFSET_Y,RESIZED_WIDTH,RESIZED_HEIGHT,IMAGE_PTR,IMAGE_TYPE,IMAGE_LENGTH) \
                    gdi_image_codec_draw(                       \
                                GDI_IMAGE_SRC_FROM_MEMORY,      \
                                IMAGE_PTR,                      \
                                IMAGE_TYPE,                     \
                                IMAGE_LENGTH,                   \
                                OFFSET_X,                       \
                                OFFSET_Y,                       \
                                RESIZED_WIDTH,                  \
                                RESIZED_HEIGHT,                 \
                                GDI_IMAGE_CODEC_FLAG_BLEND_2_LAYERS,                              \
                                0)

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_file_blend2layers
 * DESCRIPTION
 *  draw image from file with original image dimension
 *  When drawing png and abm image,
 *  it use src layer color as alpha blending color only when the active layer color is transparent color.
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  IMAGE_NAME        : [IN]        image file name
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_image_draw_file_blend2layers(OFFSET_X,OFFSET_Y,IMAGE_NAME) \
                    gdi_image_codec_draw(                       \
                                GDI_IMAGE_SRC_FROM_FILE,        \
                                (uint8_t*) IMAGE_NAME,               \
                                GDI_IMAGE_TYPE_INVALID,         \
                                0,                              \
                                OFFSET_X,                       \
                                OFFSET_Y,                       \
                                0,                              \
                                0,                              \
                                GDI_IMAGE_CODEC_FLAG_BLEND_2_LAYERS,                              \
                                0)

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_resized_file_blend2layers
 * DESCRIPTION
 *  draw image from file with specified dimension
 *  When drawing png and abm image,
 *  it use src layer color as alpha blending color only when the active layer color is transparent color.
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  RESIZED_WIDTH     : [IN]        image width to draw
 *  RESIZED_HEIGHT    : [IN]        image height to draw
 *  IMAGE_NAME        : [IN]        image file name
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_image_draw_resized_file_blend2layers(OFFSET_X,OFFSET_Y,RESIZED_WIDTH,RESIZED_HEIGHT,IMAGE_NAME) \
                    gdi_image_codec_draw(                       \
                                GDI_IMAGE_SRC_FROM_FILE,        \
                                (uint8_t*) IMAGE_NAME,               \
                                GDI_IMAGE_TYPE_INVALID,         \
                                0,                              \
                                OFFSET_X,                       \
                                OFFSET_Y,                       \
                                RESIZED_WIDTH,                  \
                                RESIZED_HEIGHT,                 \
                                GDI_IMAGE_CODEC_FLAG_BLEND_2_LAYERS,                              \
                                0)

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_mem_frames
 * DESCRIPTION
 *  draw one frame in animation in memory
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  IMAGE_PTR         : [IN]        image pointer
 *  IMAGE_TYPE        : [IN]        image type, ex.GDI_IMAGE_TYPE_BMP...(defined in gdi_const.h)
 *  IMAGE_LENGTH      : [IN]        image size of IMAGE_PTR
 *  FRAME_NO          : [IN]        this is frame index to draw
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_image_draw_mem_frames(OFFSET_X,OFFSET_Y,IMAGE_PTR,IMAGE_TYPE,IMAGE_LENGTH,FRAME_NO) \
                    gdi_image_codec_draw(                       \
                                GDI_IMAGE_SRC_FROM_MEMORY,      \
                                IMAGE_PTR,                      \
                                IMAGE_TYPE,                     \
                                IMAGE_LENGTH,                   \
                                OFFSET_X,                       \
                                OFFSET_Y,                       \
                                0,                              \
                                0,                              \
                                0,                              \
                                (uint32_t)FRAME_NO)

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_resized_mem_frames
 * DESCRIPTION
 *  draw one frame in animation in memory with specified dimension
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  RESIZED_WIDTH     : [IN]        image width to draw
 *  RESIZED_HEIGHT    : [IN]        image height to draw
 *  IMAGE_PTR         : [IN]        image pointer
 *  IMAGE_TYPE        : [IN]        image type, ex.GDI_IMAGE_TYPE_BMP...(defined in gdi_const.h)
 *  IMAGE_LENGTH      : [IN]        image size of IMAGE_PTR
 *  FRAME_NO          : [IN]        this is frame index to draw
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_image_draw_resized_mem_frames(OFFSET_X,OFFSET_Y,RESIZED_WIDTH,RESIZED_HEIGHT,IMAGE_PTR,IMAGE_TYPE,IMAGE_LENGTH,FRAME_NO) \
                    gdi_image_codec_draw(                       \
                                GDI_IMAGE_SRC_FROM_MEMORY,      \
                                IMAGE_PTR,                      \
                                IMAGE_TYPE,                     \
                                IMAGE_LENGTH,                   \
                                OFFSET_X,                       \
                                OFFSET_Y,                       \
                                RESIZED_WIDTH,                  \
                                RESIZED_HEIGHT,                 \
                                0,                              \
                                (uint32_t)FRAME_NO)

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_file
 * DESCRIPTION
 *  draw image from file with original image dimension
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  IMAGE_NAME        : [IN]        image file name
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_image_draw_file(
                    int32_t x,
                    int32_t y,
                    const char *img_name);
/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_resized_file
 * DESCRIPTION
 *  draw image from file with specified dimension
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  RESIZED_WIDTH     : [IN]        image width to draw
 *  RESIZED_HEIGHT    : [IN]        image height to draw
 *  IMAGE_NAME        : [IN]        image file name
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_image_draw_resized_file(
                    int32_t x,
                    int32_t y,
                    int32_t w,
                    int32_t h,
                    const char *img_name);
/*****************************************************************************
 * FUNCTION
 *  gdi_image_get_dimension_file
 * DESCRIPTION
 *  get image dimension from a file
 * PARAMETERS
 *  image_name      : [IN]        image file name
 *  width           : [OUT]       return image width
 *  height          : [OUT]       return image height
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_image_get_dimension_file(const char *image_name, int32_t *width, int32_t *height);
/*****************************************************************************
 * FUNCTION
 *  gdi_image_get_type_from_file
 * DESCRIPTION
 *  get image type from file
 * PARAMETERS
 *  file_name       : [IN]    image file name
 * RETURNS
 *  uint16_t, image type, ex. GDI_IMAGE_TYPE_BMP...(defined in gdi_const.h)
 *****************************************************************************/
extern uint16_t gdi_image_get_type_from_file(const char *file_name);
/*****************************************************************************
 * FUNCTION
 *  gdi_resize_bitblt
 * DESCRIPTION
 *  software layer resizer.
 *  resize src_layer and output to active layer
 * PARAMETERS
 *  src_layer_handle       : [IN]     layer to resize
 *  sx1                    : [IN]     x of left-top cornor of resize region
 *  sy1                    : [IN]     y of left-top cornor of resize region
 *  sx2                    : [IN]     x of right-down cornor of resize region
 *  sy2                    : [IN]     y of right-down cornor of resize region
 *  dx1                    : [IN]     x of left-top cornor of output region
 *  dy1                    : [IN]     y of left-top cornor of output region
 *  dx2                    : [IN]     x of right-down cornor of output region
 *  dy2                    : [IN]     y of right-down cornor of output region
 * RETURNS
 *  void
 *****************************************************************************/
extern gdi_result_t gdi_resize_bitblt(gdi_layer_handle_t src_layer, int32_t sx1,int32_t sy1,int32_t sx2,int32_t sy2,int32_t dx1,int32_t dy1,int32_t dx2,int32_t dy2);

/* DOM-NOT_FOR_SDK-BEGIN */

/*****************************************************************************
 * FUNCTION
 *  gdi_image_util_check_decoder_cap
 * DESCRIPTION
 *  check gdi decoder's capability
 * PARAMETERS
 *  image_type          : [IN]        image type
 *  image_width         : [IN]        not used
 *  image_height        : [IN]        not used
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_image_util_check_decoder_cap(uint16_t image_type, int32_t image_width, int32_t image_height);
/*****************************************************************************
 * FUNCTION
 *  gdi_image_bmp_encode_get_size
 * DESCRIPTION
 *  get the buffer size needed when bmp encode
 * PARAMETERS
 *  width           [IN]    encode width
 *  height          [IN]    encode height
 *  depth           [IN]    bits per pixel
 *  bmp_size        [OUT]   the buffer size needed when bmp encode
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
extern gdi_result_t gdi_image_bmp_encode_get_size(int32_t width, int32_t height, uint8_t  depth, int32_t *bmp_size);

/* DOM-NOT_FOR_SDK-END */


/* DOM-NOT_FOR_SDK-BEGIN */

/*****************************************************************************
 * FUNCTION
 *  gdi_image_set_update_area
 * DESCRIPTION
 *  set update area.
 *  This update area is used in gdi_image_progress_callback to progressive blt image.
 * PARAMETERS
 *  x1                 : [OUT]    the x position of left-up corner of the region
 *  y1                 : [OUT]    the y position of left-up corner of the region
 *  x2                 : [OUT]    the x position of right-down corner of the region
 *  y2                 : [OUT]    the y position of right-down corner of the region
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_image_set_update_area(int32_t x1,int32_t y1,int32_t x2,int32_t y2);
/*****************************************************************************
 * FUNCTION
 *  gdi_image_get_update_area
 * DESCRIPTION
 *  get the update area
 *  This update area is used in gdi_image_progress_callback to progressive blt image.
 * PARAMETERS
 *  x1                 : [OUT]    the x position of left-up corner of the region
 *  y1                 : [OUT]    the y position of left-up corner of the region
 *  x2                 : [OUT]    the x position of right-down corner of the region
 *  y2                 : [OUT]    the y position of right-down corner of the region
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_image_get_update_area(int32_t *x1,int32_t *y1,int32_t *x2,int32_t *y2);

/*****************************************************************************
 * FUNCTION
 *  gdi_image_bmp_get_palette_size
 * DESCRIPTION
 *  get the palette size of 8-bits bmp
 * PARAMETERS
 *  void
 * RETURNS
 *  uint8_t  palette size of 8-bits bmp
 *****************************************************************************/
extern uint8_t gdi_image_bmp_get_palette_size(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_image_abm_set_source_layer
 * DESCRIPTION
 *  set alpha source blending source layer
 * PARAMETERS
 *  src_layer_handle       : [IN] gdi handle of the alpha source layer
 * RETURNS
 *  gdi_layer_handle_t,  old alpha source layer handle
 *****************************************************************************/
#define gdi_image_abm_set_source_layer  gdi_set_alpha_blending_source_layer
/*****************************************************************************
 * FUNCTION
 *  gdi_image_abm_get_source_layer
 * DESCRIPTION
 *  get alpha blending source layer
 * PARAMETERS
 *  src_layer_handle       : [OUT] gdi handle of the alpha source layer
 * RETURNS
 *  void
 *****************************************************************************/
#define gdi_image_abm_get_source_layer  gdi_get_alpha_blending_source_layer
#define GDI_RGB_TO_BUFFER_FORMAT(R,G,B)      DRV_RGB_TO_HW(R,G,B)
#define GDI_BUFFER_FORMAT_TO_RGB_R(VALUE)    DRV_HW_TO_RGB_R(VALUE)
#define GDI_BUFFER_FORMAT_TO_RGB_G(VALUE)    DRV_HW_TO_RGB_G(VALUE)
#define GDI_BUFFER_FORMAT_TO_RGB_B(VALUE)    DRV_HW_TO_RGB_B(VALUE)
#define GDI_MAINLCD_GET_BUFFER_PIXEL(X,Y,VALUE)    DRV_MAINLCD_GET_BUFFER_PIXEL(X,Y,VALUE)
#define GDI_MAINLCD_SET_BUFFER_PIXEL(X,Y,VALUE)    DRV_MAINLCD_SET_BUFFER_PIXEL(X,Y,VALUE)

#define gdi_image_draw_ext                              gdi_image_draw_mem

/* DOM-NOT_FOR_SDK-END */

/*****************************************************************************
 * FUNCTION
 *  gdi_image_draw_resized_mem
 * DESCRIPTION
 *  draw image in memory
 * PARAMETERS
 *  OFFSET_X          : [IN]        x position to draw image
 *  OFFSET_Y          : [IN]        y position to draw image
 *  RESIZED_WIDTH     : [IN]        image width to draw
 *  RESIZED_HEIGHT    : [IN]        image height to draw
 *  IMAGE_PTR         : [IN]        image pointer
 *  IMAGE_TYPE        : [IN]        image type, ex.GDI_IMAGE_TYPE_BMP...(defined in gdi_const.h)
 *  IMAGE_LENGTH      : [IN]        image size of IMAGE_PTR
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_image_draw_resized_ext                      gdi_image_draw_resized_mem

/*****************************************************************************
 * FUNCTION
 *  gdi_bitblt_resized
 * DESCRIPTION
 *  resize the content in src_layer_handle to active layer
 * PARAMETERS
 *  src_layer_handle    [IN]    source layer
 *  sx1                 [IN]    region in source layer to resize
 *  sy1                 [IN]    region in source layer to resize
 *  sx2                 [IN]    region in source layer to resize
 *  sy2                 [IN]    region in source layer to resize
 *  dx1                 [IN]    region in active layer to resize to
 *  dy1                 [IN]    region in active layer to resize to
 *  dx2                 [IN]    region in active layer to resize to
 *  dy2                 [IN]    region in active layer to resize to
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
extern gdi_result_t gdi_bitblt_resized(
                    gdi_layer_handle_t src_layer_handle,
                    int32_t sx1,
                    int32_t sy1,
                    int32_t sx2,
                    int32_t sy2,
                    int32_t dx1,
                    int32_t dy1,
                    int32_t dx2,
                    int32_t dy2);


/*****************************************************************************
 * FUNCTION
 *  gdi_bitblt_resized_with_resizer
 * DESCRIPTION
 *  resize the content in src_layer_handle to active layer by a specified resizer.
 *  If the specified resizer failed, GDI may try other resizer to resize until success.
 * PARAMETERS
 *  src_layer_handle    [IN]    source layer
 *  sx1                 [IN]    region in source layer to resize
 *  sy1                 [IN]    region in source layer to resize
 *  sx2                 [IN]    region in source layer to resize
 *  sy2                 [IN]    region in source layer to resize
 *  dx1                 [IN]    region in active layer to resize to
 *  dy1                 [IN]    region in active layer to resize to
 *  dx2                 [IN]    region in active layer to resize to
 *  dy2                 [IN]    region in active layer to resize to
 *  resizer             [IN]    resize by this resizer
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
extern gdi_result_t gdi_bitblt_resized_with_resizer(
                    gdi_layer_handle_t src_layer_handle,
                    int32_t sx1,
                    int32_t sy1,
                    int32_t sx2,
                    int32_t sy2,
                    int32_t dx1,
                    int32_t dy1,
                    int32_t dx2,
                    int32_t dy2,
                    gdi_resizer_enum resizer);


/*****************************************************************************
 * FUNCTION
 *  gdi_bitblt_resized_with_resizer_ext
 * DESCRIPTION
 *  resize the content in src_layer_handle to active layer by a specified resizer "expected_resizer".
 *  If try_until_success is true and the specified resizer failed, GDI may try other resizer to resize until success,
 *  and remember the resizer actually used in used_resizer.
 * PARAMETERS
 *  src_layer_handle        [IN]    source layer
 *  sx1                     [IN]    region in source layer to resize
 *  sy1                     [IN]    region in source layer to resize
 *  sx2                     [IN]    region in source layer to resize
 *  sy2                     [IN]    region in source layer to resize
 *  dx1                     [IN]    region in active layer to resize to
 *  dy1                     [IN]    region in active layer to resize to
 *  dx2                     [IN]    region in active layer to resize to
 *  dy2                     [IN]    region in active layer to resize to
 *  expected_resizer        [IN]    resize by this resizer
 *  expected_resizer_result [OUT]   the result of expected_resizer.
 *  try_until_success       [IN]    if true, GDI would try other resizer to resize until resize success when expected_resizer failed.
 *  used_resizer            [OUT]   if try_until_success is true and expected_resizer failed, this is the resizer GDI actually use.
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
extern gdi_result_t gdi_bitblt_resized_with_resizer_ext(
                    gdi_layer_handle_t src_layer_handle,
                    int32_t sx1,
                    int32_t sy1,
                    int32_t sx2,
                    int32_t sy2,
                    int32_t dx1,
                    int32_t dy1,
                    int32_t dx2,
                    int32_t dy2,
                    gdi_resizer_enum expected_resizer,
                    gdi_result_t *expected_resizer_result,
                    bool try_until_success,
                    gdi_resizer_enum *used_resizer);

#include "gdi_imgdec.h"
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_layer_element
 * DESCRIPTION
 *  return the WFCDevice and WFCContext and WFCElement.
 *  WFCElement and WFCContext handle would be different between MAIN_LCD or SUBLCD.
 *  Remember to set correct active LCD before using this API.
 *
 *  Multimedia APP may need to get layer element, and pass the handle to Multimedia hw.
 *  Becuase Multimedia hw, ex, camera or video, may need to modify the layer element attribute while hw updating.
 * PARAMETERS
 *  blt_layer_index     [IN]    the index of blt layer. usually 0~5
 *  layer_element       [OUT]   return the WFCElement of the specified layer index
 *  blt_ctx             [OUT]   return the blt contex handle. It's needed when calling wfcCommit
 *  blt_dev             [OUT]   return the blt device handle. It's needed when calling wfcSetElementAttribi
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
extern gdi_result_t gdi_layer_get_layer_element(uint32_t blt_layer_index, uint32_t *layer_element, uint32_t *blt_ctx, uint32_t *blt_dev);

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_actual_rotate_value
 * DESCRIPTION
 *  return the actual rotate value.
 *  GDI user may use "layer rotate" or "lcd rotate+rotate by layer" to rotate a layer.
 *  ("layer rotate" set by gdi_layer_set_rotate)
 *  ("lcd rotate" set by gdi_lcd_set_rotate, "rotate by layer" set by gdi_lcd_get_rotate_by_layer)
 *  If it simply use "layer rotate", we return the layer rotate value.
 *  But if it use "lcd rotate+rotate by layer", we'll return the final rotate value that gdi set to lcd driver.
 *  In DDV2, while "video seek", the hw layer will not blt to lcd, so it's not set to DDv2 element.
 *  But the video driver still have to know the layer actual rotate value to rotate the video content for MDI.
 *  So GDI provide this API to calculate the layer actual rotate value.
 * PARAMETERS
 *  layer_handle                    [IN]    layer handle
 *  layer_actual_rotate_value       [OUT]   return the actual layer rotate value
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
extern gdi_result_t gdi_layer_get_actual_rotate_value(gdi_layer_handle_t layer_handle, uint8_t *layer_actual_rotate_value);

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_hw_update_layer
 * DESCRIPTION
 *  set the layer as a hw update layer.
 *  hw update layer means the layer content and settings are set by multimedia hw, ex. Camera or video.
 *  GDI won't set the layer config of hw layer to lcd, except layer offset.
 * PARAMETERS
 *  layer_handle  [IN]  the layer to be set as hw update layer
 *  is_hw_layer   [OUT] the layer is hw layer or not
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
extern gdi_result_t gdi_layer_set_hw_update_layer(gdi_layer_handle_t layer_handle, bool is_hw_layer);

/*****************************************************************************
 * FUNCTION
 *  gdi_layer_is_hw_update_layer
 * DESCRIPTION
 *  query if the layer is hw update layer.
 * PARAMETERS
 *  layer_handle  [IN]  the layer to query
 *  is_hw_layer   [OUT] the layer is hw layer or not
 * RETURNS
 *  gdi_result_t
 *****************************************************************************/
extern gdi_result_t gdi_layer_is_hw_update_layer(gdi_layer_handle_t layer_handle, bool *is_hw_layer);

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
extern gdi_result_t gdi_lcd_get_blt_counter(uint32_t *blt_counter);

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
extern gdi_result_t gdi_layer_set_blt_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height);


/*****************************************************************************
 * FUNCTION
 *  gdi_waiting_blt_finish
 * DESCRIPTION
 *  Wait last blt finish.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_waiting_blt_finish(void);


/*****************************************************************************
 * FUNCTION
 *  gdi_layer_blt_nb_concurrent_no_blt_lock
 * DESCRIPTION
 *  Blt layers without check blt lock.
 *
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
extern gdi_result_t gdi_layer_blt_nb_concurrent_no_blt_lock(
                   gdi_layer_handle_t handle0,
                   gdi_layer_handle_t handle1,
                   gdi_layer_handle_t handle2,
                   gdi_layer_handle_t handle3,
                   gdi_layer_handle_t handle4,
                   gdi_layer_handle_t handle5,
                   int32_t x1,
                   int32_t y1,
                   int32_t x2,
                   int32_t y2,
                   void (*blt_finish_callback)(void *param),
                   void *param);

extern gdi_result_t gdi_layer_flatten_nb_ext(
                    gdi_layer_handle_t handle0,
                    gdi_layer_handle_t handle1,
                    gdi_layer_handle_t handle2,
                    gdi_layer_handle_t handle3
                    );

extern gdi_result_t gdi_layer_flatten_nb_with_clip_ext(
                    gdi_layer_handle_t handle0,
                    gdi_layer_handle_t handle1,
                    gdi_layer_handle_t handle2,
                    gdi_layer_handle_t handle3
                    );

extern gdi_result_t gdi_layer_flatten_with_flag_ext(
                    gdi_layer_handle_t handle0,
                    gdi_layer_handle_t handle1,
                    gdi_layer_handle_t handle2,
                    gdi_layer_handle_t handle3
                    ,gdi_layer_flatten_flag_bitset flag
                    );

extern void gdi_layer_flatten_nb_wait_for_previous_finish(void);

extern bool gdi_layer_flatten_get_waiting_hint(void);

/* Bit set of gdi_layer_blt_flag_enum */
typedef uint32_t gdi_layer_blt_flag_bit_set;

/* DOM-NOT_FOR_SDK-BEGIN */
/*****************************************************************************
 * FUNCTION
 *  gdi_util_adjust_aspect
 * DESCRIPTION
 *  Adjust the aspect by flag
 * PARAMETERS
 *  aspect_flag    : [IN]        Is the flag of aspect mode.
 *  box_x          : [IN]        Is the box x position to fit image
 *  box_y          : [IN]        Is the box y position to fit image
 *  box_width      : [IN]        Is the box width to fit image
 *  box_height     : [IN]        Is the box height to fit image
 *  image_width    : [IN]        Is the image width
 *  image_height   : [IN]        Is the image height
 *  output_x       : [OUT]       Is the output x
 *  output_y       : [OUT]       Is the output y
 *  output_w       : [OUT]       Is the output width
 *  output_h       : [OUT]       Is the output height
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_util_adjust_aspect(
             int32_t aspect_flag,
             int32_t box_x,
             int32_t box_y,
             int32_t box_width,
             int32_t box_height,
             int32_t image_width,
             int32_t image_height,
             int32_t *output_x,
             int32_t *output_y,
             int32_t *output_w,
             int32_t *output_h);


/*****************************************************************************
 * FUNCTION
 *  gdi_set_alpha_blending_source_layer
 * DESCRIPTION
 *  set alpha source blending source layer
 * PARAMETERS
 *  src_layer_handle       : [IN] gdi handle of the alpha source layer
 * RETURNS
 *  gdi_layer_handle_t  old alpha source layer handle
 *****************************************************************************/
extern gdi_layer_handle_t gdi_set_alpha_blending_source_layer(gdi_layer_handle_t src_layer_handle);
/*****************************************************************************
 * FUNCTION
 *  gdi_get_alpha_blending_source_layer
 * DESCRIPTION
 *  get alpha blending source layer
 * PARAMETERS
 *  src_layer_handle       : [OUT] gdi handle of the alpha source layer
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_get_alpha_blending_source_layer(gdi_layer_handle_t *src_layer_handle);
/*****************************************************************************
 * FUNCTION
 *  gdi_push_and_set_alpha_blending_source_layer
 * DESCRIPTION
 *  push current alpha source layer into stack and set passin layer as alpha source layer
 * PARAMETERS
 *  handle      : [IN]        new alpha blending source layer
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_push_and_set_alpha_blending_source_layer(gdi_layer_handle_t handle);
/*****************************************************************************
 * FUNCTION
 *  gdi_pop_and_restore_alpha_blending_source_layer
 * DESCRIPTION
 *  pop the layer in stack and make it as alpha source layer
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_pop_and_restore_alpha_blending_source_layer(void);
/**
 * @brief This function draws a rectangle with frame on active layer.
 * @param[in] x1              is x of left-top cornor of the rectangle.
 * @param[in] y1              is y of left-top cornor of the rectangle.
 * @param[in] x2              is x of right-down cornor of the rectangle.
 * @param[in] y2              is y of right-down cornor of the rectangle.
 * @param[in] rect_color      is the color of the solid rectangle.
 * @param[in] frame_color     is the color of the rectangle frame.
 * @param[in] frame_width     is the width of the rectangle frame border.
 * @return                    void.
 */
void gdi_draw_frame_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                gdi_color_t rect_color, gdi_color_t frame_color, uint16_t frame_width);
/*****************************************************************************
 * FUNCTION
 *  gdi_draw_button_rect
 * DESCRIPTION
 *  draw a rect with button style
 * PARAMETERS
 *  x1                  : [IN]   x of left-top cornor of rectangle
 *  y1                  : [IN]   y of left-top cornor of rectangle
 *  x2                  : [IN]   x of right-down cornor of rectangle
 *  y2                  : [IN]   y of right-down cornor of rectangle
 *  rect_color          : [IN]   the color of rectangle
 *  frame_color         : [IN]   the color of rectangle frame
 *  frame_width         : [IN]   the width of rectangle frame border
 *  button_height       : [IN]   button height
 *  invert              : [IN]   draw a normal button, or a pushed button
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_draw_button_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                gdi_color_t rect_color, gdi_color_t frame_color,
                uint16_t frame_width, uint16_t button_height, bool invert);
/*****************************************************************************
 * FUNCTION
 *  gdi_draw_shadow_rect
 * DESCRIPTION
 *  draw a rect with shadow style
 * PARAMETERS
 *  x1                  : [IN]   x of left-top cornor of rectangle
 *  y1                  : [IN]   y of left-top cornor of rectangle
 *  x2                  : [IN]   x of right-down cornor of rectangle
 *  y2                  : [IN]   y of right-down cornor of rectangle
 *  rect_color          : [IN]   the color of rectangle
 *  frame_color         : [IN]   the color of rectangle frame
 *  frame_width         : [IN]   the width of rectangle frame
 *  shadow_color        : [IN]   shadow color
 *  shadow_height       : [IN]   shadow height
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_draw_shadow_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                gdi_color_t rect_color, gdi_color_t frame_color, uint16_t frame_width,
                gdi_color_t shadow_color, uint16_t shadow_height);
/*****************************************************************************
 * FUNCTION
 *  gdi_draw_gradient_rect
 * DESCRIPTION
 *  draw a rect with gradient filling
 * PARAMETERS
 *  x1                  : [IN]   x of left-top cornor of rectangle
 *  y1                  : [IN]   y of left-top cornor of rectangle
 *  x2                  : [IN]   x of right-down cornor of rectangle
 *  y2                  : [IN]   y of right-down cornor of rectangle
 *  rect_color_start    : [IN]   the gradient start color
 *  rect_color_end      : [IN]   the gradient end color
 *  frame_color         : [IN]   the color of rectangle frame
 *  frame_width         : [IN]   the width of rectangle frame
 *  style               : [IN]   the gradient style
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_draw_gradient_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2,
                gdi_color_t rect_color_start, gdi_color_t rect_color_end, gdi_color_t frame_color,
                uint16_t frame_width, gdi_gradient_rect_style_t style);
/**
 * @brief This function draws a greyscale rectangle on current active layer.
 *  x1                : [IN]   x of left-top cornor of rectangle
 *  y1                : [IN]   y of left-top cornor of rectangle
 *  x2                : [IN]   x of right-down cornor of rectangle
 *  y2                : [IN]   y of right-down cornor of rectangle
 *  white_value       : [IN]   white component value
 *  black_value       : [IN]   black component value
 * @return                    void.
 */
void gdi_draw_greyscale_rect(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t white_value, int32_t black_value);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_create_cf_using_outside_memory
 * DESCRIPTION
 *  Create a layer using buffer in parameter.
 *  Will return handle of the layer.
 * PARAMETERS
 *  cf                      : [IN]   layer color format
 *  offset_x                : [IN]   layer position x
 *  offset_y                : [IN]   layer position y
 *  width                   : [IN]   layer width
 *  height                  : [IN]   layer height
 *  handle                  : [OUT]  return the created layer handle
 *  data                    : [IN]   the buffer used to create layer
 *  size                    : [IN]   size of outside_memory
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_create_cf_using_outside_memory(gdi_color_format_t cf,
                        int32_t offset_x, int32_t offset_y, int32_t width, int32_t height,
                        uint8_t *data, uint32_t size, gdi_layer_handle_t *handle);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_active
 * DESCRIPTION
 *  get currently active layer handle
 * PARAMETERS
 *  handle      : [OUT]       return active layer handle
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern void gdi_layer_get_active(gdi_layer_handle_t *handle);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_active
 * DESCRIPTION
 *  Set a layer as active layer
 * PARAMETERS
 *  handle       : [IN]   layer handle to be set as active layer
 * RETURNS
 *  void
 *****************************************************************************/
void gdi_layer_set_active(gdi_layer_handle_t handle);
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
extern gdi_result_t gdi_layer_get_color_format(gdi_color_format_t *cf);
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
extern gdi_result_t gdi_layer_set_color_format(gdi_color_format_t cf);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_source_key
 * DESCRIPTION
 *  Set transparent color and enable transparent color
 * PARAMETERS
 *  source_key_enable       : [IN]        true if want to enable transparent color
 *  source_key_value        : [IN]        source key (transparent color)
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_set_source_key(bool source_key_enable, gdi_color_t source_key_value);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_source_key
 * DESCRIPTION
 *  Get transparent color and enable transparent color
 * PARAMETERS
 *  source_key_enable       : [OUT]   get true if transparent color is enable
 *  source_key_value        : [OUT]   source key (transparent color)
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_get_source_key(bool *source_key_enable, gdi_color_t *source_key_value);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_clip
 * DESCRIPTION
 *  Get current clip region of a layer.
 * PARAMETERS
 *  x1                : [OUT]   x of left-top cornor of clip region
 *  y1                : [OUT]   y of left-top cornor of clip region
 *  x2                : [OUT]   x of right-down cornor of clip region
 *  y2                : [OUT]   y of right-down cornor of clip region
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_get_clip(int32_t *x1, int32_t *y1, int32_t *x2, int32_t *y2);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_clip
 * DESCRIPTION
 *  Set clip region.
 *  Will set clip region, use layer's own coordinate.
 * PARAMETERS
 *  x1                : [IN]   x of left-top cornor of clip region
 *  y1                : [IN]   y of left-top cornor of clip region
 *  x2                : [IN]   x of right-down cornor of clip region
 *  y2                : [IN]   y of right-down cornor of clip region
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_set_clip(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_reset_clip
 * DESCRIPTION
 *  Reset clip region to layer size
 *  Will reset clip region to layer's width/height.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_reset_clip(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_push_clip
 * DESCRIPTION
 *  Push current clip region into clip stack.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_push_clip(void);
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
extern void gdi_layer_push_and_set_clip(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_pop_clip
 * DESCRIPTION
 *  Pop current clip region from stack.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_pop_clip(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_blt_ext
 * DESCRIPTION
 *  Blt layers to LCD in the order that handle0 is the most bottom layer.
 *  The last four parameters are the region which to blt to LCD.
 * PARAMETERS
 *  handle0     : [IN]        layer handle 0, the most bottom layer
 *  handle1     : [IN]        layer handle 1
 *  handle2     : [IN]        layer handle 2
 *  handle3     : [IN]        layer handle 3
 *  x1          : [IN]        x of left-top cornor of blt region
 *  y1          : [IN]        y of left-top cornor of blt region
 *  x2          : [IN]        x of right-down cornor of blt region
 *  y2          : [IN]        y of right-down cornor of blt region
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_blt_ext(gdi_layer_handle_t handle0, gdi_layer_handle_t handle1,
                                    gdi_layer_handle_t handle2, gdi_layer_handle_t handle3,
                                    int32_t x1, int32_t y1, int32_t x2, int32_t y2);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_buffer_ptr
 * DESCRIPTION
 *  get currently active layer's frame buffer pointer.
 * PARAMETERS
 *  buf_ptr     : [OUT]       return the buffer pointer of layer
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_get_buffer_ptr(uint8_t **buf_ptr);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_blt_previous
 * DESCRIPTION
 *  Blt previous blt layers.
 * PARAMETERS
 *  x1  : [IN]   x of left-top cornor of blt region
 *  y1  : [IN]   y of left-top cornor of blt region
 *  x2  : [IN]   x of right-down cornor of blt region
 *  y2  : [IN]   y of right-down cornor of blt region
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_blt_previous(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_resize
 * DESCRIPTION
 *  set the dimension of layer.
 *  (w*h*bit_per_pixel) can't larger than the buffer size of layer.
 * PARAMETERS
 *  width       : [IN]        width of layer (in pixel)
 *  height      : [IN]        height of layer (in pixel)
 * RETURNS
 *  void
 *****************************************************************************/
extern void gdi_layer_resize(int32_t width, int32_t height);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_set_blt_layer
 * DESCRIPTION
 *  Set blt layers to GDI and config driver's setting
 * PARAMETERS
 *  handle0     : [IN]        layer handle 0, the most bottom layer
 *  handle1     : [IN]        layer handle 1
 *  handle2     : [IN]        layer handle 2
 *  handle3     : [IN]        layer handle 3
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_layer_set_blt_layer(gdi_layer_handle_t handle0, gdi_layer_handle_t handle1,
                                            gdi_layer_handle_t handle2, gdi_layer_handle_t handle3);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_rotate
 * DESCRIPTION
 *  get layer rotate value
 * PARAMETERS
 *  void
 * RETURNS
 *  uint8_t, the rotate value defined in gdi_const.h, should be GDI_LCD_LAYER_ROTATE_0,90,180...
 *****************************************************************************/
extern uint8_t gdi_layer_get_rotate(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_get_bits_per_pixel
 * DESCRIPTION
 *  get the bits per pixel value of active lcd
 * PARAMETERS
 *  void
 * RETURNS
 *  int, bits per pixel of active lcd
 *****************************************************************************/
extern int gdi_lcd_get_bits_per_pixel(void);
/*****************************************************************************
 * FUNCTION
 *  gdi_layer_get_blt_layer
 * DESCRIPTION
 *  get the blt layer
 * PARAMETERS
 *  handle0     : [OUT]        layer handle 0, the most bottom layer
 *  handle1     : [OUT]        layer handle 1
 *  handle2     : [OUT]        layer handle 2
 *  handle3     : [OUT]        layer handle 3
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_layer_get_blt_layer(H1,H2,H3,H4)       gdi_layer_get_blt_layer_ext(H1,H2,H3,H4)
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_paint_ext
 * DESCRIPTION
 *  Blt layers to LCD in the order that handle0 is the most bottom layer.
 *  The last four parameters are the region which to blt to LCD.
 * PARAMETERS
 *  handle0     : [IN]        layer handle 0, the most bottom layer
 *  handle1     : [IN]        layer handle 1
 *  handle2     : [IN]        layer handle 2
 *  handle3     : [IN]        layer handle 3
 *  x1          : [IN]        x of left-top cornor of blt region
 *  y1          : [IN]        y of left-top cornor of blt region
 *  x2          : [IN]        x of right-down cornor of blt region
 *  y2          : [IN]        y of right-down cornor of blt region
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_lcd_paint_ext(gdi_layer_handle_t handle0, gdi_layer_handle_t handle1,
                                    gdi_layer_handle_t handle2, gdi_layer_handle_t handle3,
                                    int32_t x1, int32_t y1, int32_t x2, int32_t y2);
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_paint_all_ext
 * DESCRIPTION
 *  Blt layers to LCD in the order that handle0 is the most bottom layer.
 *  The blt region is the full screen.
 * PARAMETERS
 *  handle0     : [IN]        layer handle 0, the most bottom layer
 *  handle1     : [IN]        layer handle 1
 *  handle2     : [IN]        layer handle 2
 *  handle3     : [IN]        layer handle 3
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
extern gdi_result_t gdi_lcd_paint_all_ext(gdi_layer_handle_t handle0, gdi_layer_handle_t handle1,
                                        gdi_layer_handle_t handle2, gdi_layer_handle_t handle3);

/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_paint
 * DESCRIPTION
 *  Blt layers to LCD in the order that handle0 is the most bottom layer.
 *  The last four parameters are the region which to blt to LCD.
 * PARAMETERS
 *  handle0     : [IN]        layer handle 0, the most bottom layer
 *  handle1     : [IN]        layer handle 1
 *  handle2     : [IN]        layer handle 2
 *  handle3     : [IN]        layer handle 3
 *  x1          : [IN]        x of left-top cornor of blt region
 *  y1          : [IN]        y of left-top cornor of blt region
 *  x2          : [IN]        x of right-down cornor of blt region
 *  y2          : [IN]        y of right-down cornor of blt region
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_lcd_paint(H1,H2,H3,H4,X1,Y1,X2,Y2)        gdi_lcd_paint_ext(H1,H2,H3,H4,X1,Y1,X2,Y2)
/*****************************************************************************
 * FUNCTION
 *  gdi_lcd_paint_all
 * DESCRIPTION
 *  Blt layers to LCD in the order that handle0 is the most bottom layer.
 *  The blt region is the full screen.
 * PARAMETERS
 *  handle0     : [IN]        layer handle 0, the most bottom layer
 *  handle1     : [IN]        layer handle 1
 *  handle2     : [IN]        layer handle 2
 *  handle3     : [IN]        layer handle 3
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
#define gdi_lcd_paint_all(H1,H2,H3,H4)       gdi_lcd_paint_all_ext(H1,H2,H3,H4)
#endif /* _GDI_INTERNAL_H_ */


