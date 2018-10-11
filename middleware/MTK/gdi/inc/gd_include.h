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

#ifndef __GD_INCLUDE_H__
#define __GD_INCLUDE_H__

#include "gdi_datatype.h"

#define GD_CLIP_RECT_TEST(X1, Y1, X2, Y2,CX1,CY1,CX2,CY2)\
{                                                        \
   if(X1 > X2  ) { return; }                             \
   if(Y1 > Y2  ) { return; }                             \
   if(X2 < CX1) { return; }                              \
   if(Y2 < CY1) { return; }                              \
   if(X1 > CX2) { return; }                              \
   if(Y1 > CY2) { return; }                              \
   if(X1 < CX1) { X1 = CX1; }                            \
   if(Y1 < CY1) { Y1 = CY1; }                            \
   if(X2 > CX2) { X2 = CX2; }                            \
   if(Y2 > CY2) { Y2 = CY2; }                            \
}

typedef void (*gd_draw_line_func) (uint16_t *imgp, int destWidth, int x1, int y1, int x2, int y2, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color, bool dotted);
typedef void (*gd_draw_rect_func) (uint16_t *imgp, int destWidth, int x1, int y1, int x2, int y2, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color, bool dotted);
typedef void (*gd_fill_rect_func) (uint16_t *imgp, int destWidth, int x1, int y1, int x2, int y2, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color);
typedef void (*gd_draw_round_rect_func) (uint16_t *dstPtr, int destWidth, int xo, int yo, int width, int height, int arcWidth, int arcHeight, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color, bool dotted);
typedef void (*gd_fill_round_rect_func) (uint16_t *dstPtr, int destWidth, int xo, int yo, int width, int height, int arcWidth, int arcHeight, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color);
typedef void (*gd_draw_arc_func) (uint16_t *dstPtr, int destWidth, int x, int y, int width, int height, int startAngle, int arcAngle, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color, bool dotted); 
typedef void (*gd_draw_ellipse_func) (uint16_t *dstPtr, int destWidth, int x, int y,
                                      int arcWidth, int arcHeight, int clipx1, 
                                      int clipy1, int clipx2, int clipy2, 
                                      gdi_color_t color, bool dotted); extern 
                                      void gd_draw_ellipse16( uint16_t *dstPtr, int 
                                      destWidth, int x, int y, int arcWidth, 
                                      int arcHeight, int clipx1, int clipy1, 
                                      int clipx2, int clipy2, gdi_color_t color, 
                                      bool dotted);
typedef void (*gd_fill_ellipse_func) (uint16_t *dstPtr, int destWidth,
                                      int x, int y,
                                      int arcWidth, int arcHeight,
                                      int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color);
typedef void (*gd_fill_arc_func) (uint16_t *dstPtr, int destWidth,
                                  int x, int y,
                                  int width, int height,
                                  int startAngle, int arcAngle,
                                  int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color);
typedef void (*gd_fill_triangle_func) (uint16_t *imgp, int destWidth, int x1, int y1, int x2, int y2, int x3, int y3, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color, bool dotted);

extern int gdi_atan(int sita); 
extern void gd_draw_line16( uint16_t *imgp, int destWidth, int x1, int y1, int x2, int y2, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color, bool dotted);
extern void gd_draw_rect16( uint16_t *imgp, int destWidth, int x1, int y1, int x2, int y2, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color, bool dotted);
extern void gd_fill_rect16( uint16_t *imgp, int destWidth, int x1, int y1, int x2, int y2, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color);
extern void gd_draw_round_rect16( uint16_t *dstPtr, int destWidth, int xo, int yo, int width, int height, int arcWidth, int arcHeight, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color, bool dotted);
extern void gd_fill_round_rect16( uint16_t *dstPtr, int destWidth, int xo, int yo, int width, int height, int arcWidth, int arcHeight, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color);
//extern void gd_draw_arc16( uint16_t *dstPtr, int destWidth, int x, int y, int width, int height, int startAngle, int arcAngle, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color, bool dotted);
extern void gd_fill_ellipse16(uint16_t *dstPtr, int destWidth, int x, int y, int arcWidth, int arcHeight, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color);
//extern void gd_fill_arc16(uint16_t *dstPtr, int destWidth, int x, int y, int width, int height, int startAngle, int arcAngle, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color); 
extern void gd_fill_triangle16(uint16_t *imgp, int destWidth, int x1, int y1, int x2, int y2, int x3, int y3, int clipx1, int clipy1, int clipx2, int clipy2, gdi_color_t color, bool dotted);
extern void gd_null_pointer_function(void);

extern gdi_color_t gd_color_from_rgb_8(uint8_t A, uint8_t R, uint8_t G, uint8_t B);
extern gdi_color_t gd_color_from_rgb_16(uint8_t A, uint8_t R, uint8_t G, uint8_t B);
extern gdi_color_t gd_color_from_rgb_24(uint8_t A, uint8_t R, uint8_t G, uint8_t B);
extern gdi_color_t gd_color_from_rgb_32(uint8_t A, uint8_t R, uint8_t G, uint8_t B);
extern gdi_color_t gd_color_from_rgb_32PARGB(uint8_t A, uint8_t R, uint8_t G, uint8_t B);
extern gdi_color_t gd_color_from_rgb_PARGB6666(uint8_t A, uint8_t R, uint8_t G, uint8_t B);

extern void gd_color_to_rgb_8(uint8_t *R, uint8_t *A, uint8_t *G, uint8_t *B, gdi_color_t c);
extern void gd_color_to_rgb_16(uint8_t *R, uint8_t *A, uint8_t *G, uint8_t *B, gdi_color_t c);
extern void gd_color_to_rgb_24(uint8_t *R, uint8_t *A, uint8_t *G, uint8_t *B, gdi_color_t c);
extern void gd_color_to_rgb_32(uint8_t *R, uint8_t *A, uint8_t *G, uint8_t *B, gdi_color_t c);
extern void gd_color_to_rgb_32PARGB(uint8_t *R, uint8_t *A, uint8_t *G, uint8_t *B, gdi_color_t c);
extern void gd_color_to_rgb_PARGB6666(uint8_t *R, uint8_t *A, uint8_t *G, uint8_t *B, gdi_color_t c);

extern void gd_put_pixel_8(int x, int y, gdi_color_t c);
extern void gd_put_pixel_16(int x, int y, gdi_color_t c);
extern void gd_put_pixel_24(int x, int y, gdi_color_t c);
extern void gd_put_pixel_32(int x, int y, gdi_color_t c);
extern void gd_put_pixel_32PARGB(int x, int y, gdi_color_t c);
extern void gd_put_pixel_PARGB6666(int x, int y, gdi_color_t c);

extern void gd_put_pixel_to_layer_8(gdi_layer_handle_t layer_handle, int x, int y, gdi_color_t c);
extern void gd_put_pixel_to_layer_16(gdi_layer_handle_t layer_handle, int x, int y, gdi_color_t c);
extern void gd_put_pixel_to_layer_24(gdi_layer_handle_t layer_handle, int x, int y, gdi_color_t c);
extern void gd_put_pixel_to_layer_32(gdi_layer_handle_t layer_handle, int x, int y, gdi_color_t c);
extern void gd_put_pixel_to_layer_32PARGB(gdi_layer_handle_t layer_handle, int x, int y, gdi_color_t c);
extern void gd_put_pixel_to_layer_PARGB6666(gdi_layer_handle_t layer_handle, int x, int y, gdi_color_t c);

extern gdi_color_t gd_get_pixel_8(int x, int y);
extern gdi_color_t gd_get_pixel_16(int x, int y);
extern gdi_color_t gd_get_pixel_24(int x, int y);
extern gdi_color_t gd_get_pixel_32(int x, int y);
extern gdi_color_t gd_get_pixel_32PARGB(int x, int y);
extern gdi_color_t gd_get_pixel_PARGB6666(int x, int y);

extern void gd_put_buf_pixel_8(uint8_t *buf_ptr, uint32_t buf_width, int x, int y, gdi_color_t c);
extern void gd_put_buf_pixel_16(uint8_t *buf_ptr, uint32_t buf_width, int x, int y, gdi_color_t c);
extern void gd_put_buf_pixel_24(uint8_t *buf_ptr, uint32_t buf_width, int x, int y, gdi_color_t c);
extern void gd_put_buf_pixel_32(uint8_t *buf_ptr, uint32_t buf_width, int x, int y, gdi_color_t c);
extern void gd_put_buf_pixel_32PARGB(uint8_t *buf_ptr, uint32_t buf_width, int x, int y, gdi_color_t c);
extern void gd_put_buf_pixel_PARGB6666(uint8_t *buf_ptr, uint32_t buf_width, int x, int y, gdi_color_t c);

extern gdi_color_t gd_get_buf_pixel_8(uint8_t *buf_ptr, uint32_t buf_width, int x, int y);
extern gdi_color_t gd_get_buf_pixel_16(uint8_t *buf_ptr, uint32_t buf_width, int x, int y);
extern gdi_color_t gd_get_buf_pixel_24(uint8_t *buf_ptr, uint32_t buf_width, int x, int y);
extern gdi_color_t gd_get_buf_pixel_32(uint8_t *buf_ptr, uint32_t buf_width, int x, int y);
extern gdi_color_t gd_get_buf_pixel_32PARGB(uint8_t *buf_ptr, uint32_t buf_width, int x, int y);
extern gdi_color_t gd_get_buf_pixel_PARGB6666(uint8_t *buf_ptr, uint32_t buf_width, int x, int y);

extern void gd_replace_src_key_8(
                uint8_t *dest_ptr,
                int32_t dest_pitch,
                int32_t dest_offset_x,
                int32_t dest_offset_y,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                int32_t width,
                int32_t height);
extern void gd_replace_src_key_16(
                uint8_t *dest_ptr,
                int32_t dest_pitch,
                int32_t dest_offset_x,
                int32_t dest_offset_y,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                int32_t width,
                int32_t height);
extern void gd_replace_src_key_24(
                uint8_t *dest_ptr,
                int32_t dest_pitch,
                int32_t dest_offset_x,
                int32_t dest_offset_y,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                int32_t width,
                int32_t height);
extern void gd_replace_src_key_32(
                uint8_t *dest_ptr,
                int32_t dest_pitch,
                int32_t dest_offset_x,
                int32_t dest_offset_y,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                int32_t width,
                int32_t height);
extern void gd_replace_src_key_32PARGB(
                uint8_t *dest_ptr,
                int32_t dest_pitch,
                int32_t dest_offset_x,
                int32_t dest_offset_y,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                int32_t width,
                int32_t height);
extern void gd_replace_src_key_PARGB6666(
                uint8_t *dest_ptr,
                int32_t dest_pitch,
                int32_t dest_offset_x,
                int32_t dest_offset_y,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                int32_t width,
                int32_t height);

extern void gd_resize_bitblt_8(
                uint8_t *dest,
                int32_t dw,
                int32_t dh,
                int32_t dx1,
                int32_t dy1,
                int32_t dx2,
                int32_t dy2,
                uint8_t *src,
                int32_t sw,
                int32_t sh,
                int32_t sx1,
                int32_t sy1,
                int32_t sx2,
                int32_t sy2,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                gdi_color_t layer_key);
extern void gd_resize_bitblt_16(
                uint8_t *dest,
                int32_t dw,
                int32_t dh,
                int32_t dx1,
                int32_t dy1,
                int32_t dx2,
                int32_t dy2,
                uint8_t *src,
                int32_t sw,
                int32_t sh,
                int32_t sx1,
                int32_t sy1,
                int32_t sx2,
                int32_t sy2,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                gdi_color_t layer_key);
extern void gd_resize_bitblt_24(
                uint8_t *dest,
                int32_t dw,
                int32_t dh,
                int32_t dx1,
                int32_t dy1,
                int32_t dx2,
                int32_t dy2,
                uint8_t *src,
                int32_t sw,
                int32_t sh,
                int32_t sx1,
                int32_t sy1,
                int32_t sx2,
                int32_t sy2,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                gdi_color_t layer_key);
extern void gd_resize_bitblt_32(
                uint8_t *dest,
                int32_t dw,
                int32_t dh,
                int32_t dx1,
                int32_t dy1,
                int32_t dx2,
                int32_t dy2,
                uint8_t *src,
                int32_t sw,
                int32_t sh,
                int32_t sx1,
                int32_t sy1,
                int32_t sx2,
                int32_t sy2,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                gdi_color_t layer_key);
extern void gd_resize_bitblt_32PARGB(
                uint8_t *dest,
                int32_t dw,
                int32_t dh,
                int32_t dx1,
                int32_t dy1,
                int32_t dx2,
                int32_t dy2,
                uint8_t *src,
                int32_t sw,
                int32_t sh,
                int32_t sx1,
                int32_t sy1,
                int32_t sx2,
                int32_t sy2,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                gdi_color_t layer_key);
extern void gd_resize_bitblt_PARGB6666(
                uint8_t *dest,
                int32_t dw,
                int32_t dh,
                int32_t dx1,
                int32_t dy1,
                int32_t dx2,
                int32_t dy2,
                uint8_t *src,
                int32_t sw,
                int32_t sh,
                int32_t sx1,
                int32_t sy1,
                int32_t sx2,
                int32_t sy2,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                gdi_color_t layer_key);

extern void gd_bitblt_8(
                uint8_t *dest,
                int32_t dw,
                int32_t dh,
                int32_t dx1,
                int32_t dy1,
                int32_t dx2,
                int32_t dy2,
                uint8_t *src,
                int32_t sw,
                int32_t sh,
                int32_t sx1,
                int32_t sy1,
                int32_t sx2,
                int32_t sy2,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                gdi_color_t layer_key);
extern void gd_bitblt_16(
                uint8_t *dest,
                int32_t dw,
                int32_t dh,
                int32_t dx1,
                int32_t dy1,
                int32_t dx2,
                int32_t dy2,
                uint8_t *src,
                int32_t sw,
                int32_t sh,
                int32_t sx1,
                int32_t sy1,
                int32_t sx2,
                int32_t sy2,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                gdi_color_t layer_key);
extern void gd_bitblt_24(
                uint8_t *dest,
                int32_t dw,
                int32_t dh,
                int32_t dx1,
                int32_t dy1,
                int32_t dx2,
                int32_t dy2,
                uint8_t *src,
                int32_t sw,
                int32_t sh,
                int32_t sx1,
                int32_t sy1,
                int32_t sx2,
                int32_t sy2,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                gdi_color_t layer_key);
extern void gd_bitblt_32(
                uint8_t *dest,
                int32_t dw,
                int32_t dh,
                int32_t dx1,
                int32_t dy1,
                int32_t dx2,
                int32_t dy2,
                uint8_t *src,
                int32_t sw,
                int32_t sh,
                int32_t sx1,
                int32_t sy1,
                int32_t sx2,
                int32_t sy2,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                gdi_color_t layer_key);
extern void gd_bitblt_32PARGB(
                uint8_t *dest,
                int32_t dw,
                int32_t dh,
                int32_t dx1,
                int32_t dy1,
                int32_t dx2,
                int32_t dy2,
                uint8_t *src,
                int32_t sw,
                int32_t sh,
                int32_t sx1,
                int32_t sy1,
                int32_t sx2,
                int32_t sy2,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                gdi_color_t layer_key);
extern void gd_bitblt_PARGB6666(
                uint8_t *dest,
                int32_t dw,
                int32_t dh,
                int32_t dx1,
                int32_t dy1,
                int32_t dx2,
                int32_t dy2,
                uint8_t *src,
                int32_t sw,
                int32_t sh,
                int32_t sx1,
                int32_t sy1,
                int32_t sx2,
                int32_t sy2,
                int32_t clipx1,
                int32_t clipy1,
                int32_t clipx2,
                int32_t clipy2,
                gdi_color_t src_key,
                gdi_color_t layer_key);

extern void gd_fill_dot_rect_8(uint8_t *dest,int32_t dw,int32_t dh,int32_t dx1,int32_t dy1,int32_t dx2,int32_t dy2,int32_t clipx1,int32_t clipy1,int32_t clipx2,int32_t clipy2,gdi_color_t c);
extern void gd_fill_dot_rect_16(uint8_t *dest,int32_t dw,int32_t dh,int32_t dx1,int32_t dy1,int32_t dx2,int32_t dy2,int32_t clipx1,int32_t clipy1,int32_t clipx2,int32_t clipy2,gdi_color_t c);
extern void gd_fill_dot_rect_24(uint8_t *dest,int32_t dw,int32_t dh,int32_t dx1,int32_t dy1,int32_t dx2,int32_t dy2,int32_t clipx1,int32_t clipy1,int32_t clipx2,int32_t clipy2,gdi_color_t c);
extern void gd_fill_dot_rect_32(uint8_t *dest,int32_t dw,int32_t dh,int32_t dx1,int32_t dy1,int32_t dx2,int32_t dy2,int32_t clipx1,int32_t clipy1,int32_t clipx2,int32_t clipy2,gdi_color_t c);
extern void gd_fill_dot_rect_32PARGB(uint8_t *dest,int32_t dw,int32_t dh,int32_t dx1,int32_t dy1,int32_t dx2,int32_t dy2,int32_t clipx1,int32_t clipy1,int32_t clipx2,int32_t clipy2,gdi_color_t c);
extern void gd_fill_dot_rect_PARGB6666(uint8_t *dest,int32_t dw,int32_t dh,int32_t dx1,int32_t dy1,int32_t dx2,int32_t dy2,int32_t clipx1,int32_t clipy1,int32_t clipx2,int32_t clipy2,gdi_color_t c);

extern void gd_memset_8(uint8_t *dest, uint32_t value, uint32_t len);
extern void gd_memset_16(uint8_t *dest, uint32_t value, uint32_t len);
extern void gd_memset_24(uint8_t *dest, uint32_t value, uint32_t len);
extern void gd_memset_32(uint8_t *dest, uint32_t value, uint32_t len);
extern void gd_memset_32PARGB(uint8_t *dest, uint32_t value, uint32_t len);
extern void gd_memset_PARGB6666(uint8_t *dest, uint32_t value, uint32_t len);

extern void gd_image_bits_draw_8(uint8_t *dest,int32_t dw, int32_t dh, int32_t dx1, int32_t dy1, int32_t dx2, int32_t dy2, uint8_t *src, int32_t sw,int32_t sh,int32_t sx1,int32_t sy1, int32_t sx2, int32_t sy2, int32_t src_size,int32_t bits_per_pixel,gdi_color_t palette[],int32_t clipx1, int32_t clipy1, int32_t clipx2, int32_t clipy2);
extern void gd_image_bits_draw_16(uint8_t *dest,int32_t dw, int32_t dh, int32_t dx1, int32_t dy1, int32_t dx2, int32_t dy2, uint8_t *src, int32_t sw,int32_t sh,int32_t sx1,int32_t sy1, int32_t sx2, int32_t sy2, int32_t src_size,int32_t bits_per_pixel,gdi_color_t palette[],int32_t clipx1, int32_t clipy1, int32_t clipx2, int32_t clipy2);
extern void gd_image_bits_draw_24(uint8_t *dest,int32_t dw, int32_t dh, int32_t dx1, int32_t dy1, int32_t dx2, int32_t dy2, uint8_t *src, int32_t sw,int32_t sh,int32_t sx1,int32_t sy1, int32_t sx2, int32_t sy2, int32_t src_size,int32_t bits_per_pixel,gdi_color_t palette[],int32_t clipx1, int32_t clipy1, int32_t clipx2, int32_t clipy2);
extern void gd_image_bits_draw_32(uint8_t *dest,int32_t dw, int32_t dh, int32_t dx1, int32_t dy1, int32_t dx2, int32_t dy2, uint8_t *src, int32_t sw,int32_t sh,int32_t sx1,int32_t sy1, int32_t sx2, int32_t sy2, int32_t src_size,int32_t bits_per_pixel,gdi_color_t palette[],int32_t clipx1, int32_t clipy1, int32_t clipx2, int32_t clipy2);
extern void gd_image_bits_draw_32PARGB(uint8_t *dest,int32_t dw, int32_t dh, int32_t dx1, int32_t dy1, int32_t dx2, int32_t dy2, uint8_t *src, int32_t sw,int32_t sh,int32_t sx1,int32_t sy1, int32_t sx2, int32_t sy2, int32_t src_size,int32_t bits_per_pixel,gdi_color_t palette[],int32_t clipx1, int32_t clipy1, int32_t clipx2, int32_t clipy2);
extern void gd_image_bits_draw_PARGB6666(uint8_t *dest,int32_t dw, int32_t dh, int32_t dx1, int32_t dy1, int32_t dx2, int32_t dy2, uint8_t *src, int32_t sw,int32_t sh,int32_t sx1,int32_t sy1, int32_t sx2, int32_t sy2, int32_t src_size,int32_t bits_per_pixel,gdi_color_t palette[],int32_t clipx1, int32_t clipy1, int32_t clipx2, int32_t clipy2);

extern const unsigned short gd_sqrt[1025];
extern const unsigned short gd_sin_table[91];
extern const unsigned short gd_tan_table[91];

extern const gd_draw_line_func gd_draw_line[];
extern const gd_draw_rect_func gd_draw_rect[];
extern const gd_fill_rect_func gd_fill_rect[];
extern const gd_draw_round_rect_func gd_draw_round_rect[];
extern const gd_fill_round_rect_func gd_fill_round_rect[];
extern const gd_draw_arc_func gd_draw_arc[];
extern const gd_draw_ellipse_func gd_draw_ellipse[];
extern const gd_fill_ellipse_func gd_fill_ellipse[];
extern const gd_fill_arc_func gd_fill_arc[];
extern const gd_fill_triangle_func gd_fill_triangle[];

extern const gd_color_from_rgb_func gd_color_from_rgb[];
extern const gd_color_to_rgb_func gd_color_to_rgb[];
extern const gd_get_pixel_func gd_get_pixel[];
extern const gd_put_pixel_func gd_put_pixel[];
extern const gd_put_pixel_to_layer_func gd_put_pixel_to_layer[];
extern const gd_get_buf_pixel_func gd_get_buf_pixel[];
extern const gd_put_buf_pixel_func gd_put_buf_pixel[];
extern const gd_replace_src_key_func gd_replace_src_key[];
extern const gd_bitblt_func gd_resize_bitblt[];
extern const gd_bitblt_func gd_bitblt[];
extern const gd_memset_func gd_memset[];
extern const gd_fill_dot_rect_func gd_fill_dot_rect[];
extern const gd_image_bits_draw_func gd_image_bits_draw[];


#endif

