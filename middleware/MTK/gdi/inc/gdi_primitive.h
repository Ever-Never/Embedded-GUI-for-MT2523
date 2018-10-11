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

#ifndef _GDI_PRIMITIVE_H_
#define _GDI_PRIMITIVE_H_

#include "gdi_datatype.h"
#include "gdi_const.h"
#include "gdi.h"
#include "gdi_internal.h"
#include <assert.h>

/* define blue as transprent color */

/* Assert */
#define GDI_ASSERT(exp)       assert(exp)
#define GDI_DEBUG_ASSERT(exp) assert(exp)

/* check rect within clip, if exceed limit, will return */
#define GDI_CLIP_TWO_RECT(X1, Y1, X2, Y2,CX1,CY1,CX2,CY2,NULL_ACTION)\
{                                                                    \
   if(X1 > X2) { NULL_ACTION; }                                      \
   else if(Y1 > Y2)  { NULL_ACTION; }                                \
   else if(X2 < CX1) { NULL_ACTION; }                                \
   else if(Y2 < CY1) { NULL_ACTION; }                                \
   else if(X1 > CX2) { NULL_ACTION; }                                \
   else if(Y1 > CY2) { NULL_ACTION; }                                \
   else                                                              \
   {                                                                 \
      if(X1 < CX1) { X1 = CX1; }                                     \
      if(Y1 < CY1) { Y1 = CY1; }                                     \
      if(X2 > CX2) { X2 = CX2; }                                     \
      if(Y2 > CY2) { Y2 = CY2; }                                     \
   }                                                                 \
}

#define GDI_CLIP_RECT_TEST(X1, Y1, X2, Y2,NULL_ACTION)    \
{                                                         \
   if(X1 > X2) { NULL_ACTION; }                           \
   else if(Y1 > Y2)  { NULL_ACTION; }                     \
   else if(X2 < GDI_LAYER.clipx1) { NULL_ACTION; }        \
   else if(Y2 < GDI_LAYER.clipy1) { NULL_ACTION; }        \
   else if(X1 > GDI_LAYER.clipx2) { NULL_ACTION; }        \
   else if(Y1 > GDI_LAYER.clipy2) { NULL_ACTION; }        \
   else                                                   \
   {                                                      \
      if(X1 < GDI_LAYER.clipx1) { X1 = GDI_LAYER.clipx1; }\
      if(Y1 < GDI_LAYER.clipy1) { Y1 = GDI_LAYER.clipy1; }\
      if(X2 > GDI_LAYER.clipx2) { X2 = GDI_LAYER.clipx2; }\
      if(Y2 > GDI_LAYER.clipy2) { Y2 = GDI_LAYER.clipy2; }\
   }                                                      \
}

/****************************************************************************
* function
*****************************************************************************/
extern void gdi_memset16(uint8_t *address, uint16_t pixel, int size);
extern void gdi_memset24(uint8_t *address, uint32_t pixel, int size);
extern void gdi_memset32(uint8_t *address, uint32_t pixel, int size);

extern uint32_t gdi_sizeof_pixels(gdi_color_format_t cf, uint32_t width, uint32_t height);
extern uint32_t gdi_bits_per_pixel(gdi_color_format_t cf);
extern gdi_color_format_t gdi_get_color_format(uint32_t bits);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
// GDI Software resizer
///////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct
{
    uint8_t *dest;
    int32_t bitsperpixels;
    int32_t src_height_range;   /* Source */
    int32_t dest_height_range;  /* Destination */
    int32_t offset_dx;
    int32_t offset_dy;

    int32_t want_dx1, want_dx2; /* wanted rendering area */
    int32_t want_dy1, want_dy2;

    int32_t want_sx;    /* current wanted point (want_x,want_y) */
    int32_t want_sy;

    int16_t *want_sx_table;
    int16_t *want_sx_table_end;
    int16_t *next_want_sx;
    int32_t next_want_dy;

    int32_t dir_x;
    int32_t dir_y;

    /* for non resize used */
    int32_t want_start_sx, want_start_sy;
    int32_t want_end_sx, want_end_sy;
    int32_t dest_pitch_jump;
} gdi_resizer_struct;

typedef void (*gdi_resizer_put_func) (gdi_color_t c, bool want_draw);
extern gdi_resizer_put_func gdi_resizer_put;
extern gdi_resizer_struct GDI_RESIZER;

#define IS_GDI_RESIZER_WANT_DRAW(SX,SY)   (SX == GDI_RESIZER.want_sx && SY == GDI_RESIZER.want_sy)

#define GDI_NON_RESIZER_PUT_X(COLOR,WANT_DRAW)                    \
   do                                                             \
   {                                                              \
      if(WANT_DRAW)                                               \
         GDI_RESIZER_PUT_PIXEL(COLOR);                            \
      GDI_RESIZER_INCREASE_DEST;                                  \
      if(GDI_RESIZER.want_sx == GDI_RESIZER.want_end_sx)          \
      {                                                           \
         if(GDI_RESIZER.want_sy == GDI_RESIZER.want_end_sy)       \
         {                                                        \
            GDI_RESIZER.want_sx = 0x0fffffff;/* very large number */\
         }                                                        \
         else/* move to next line begin */                         \
         {                                                        \
            GDI_RESIZER.want_sx = GDI_RESIZER.want_start_sx;      \
            GDI_RESIZER.want_sy += GDI_RESIZER.dir_y;             \
            GDI_RESIZER.dest += GDI_RESIZER.dest_pitch_jump;      \
         }                                                        \
      }                                                           \
      else                                                        \
      {                                                           \
         GDI_RESIZER.want_sx += GDI_RESIZER.dir_x;                \
      }                                                           \
   }while(0)

extern int32_t gdi_resizer_init(
                int32_t src_width,
                int32_t src_height,
                int32_t src_clipx1,
                int32_t src_clipy1,
                int32_t src_clipx2,
                int32_t src_clipy2,
                int32_t dest_x1,
                int32_t dest_y1,
                int32_t dest_x2,
                int32_t dest_y2);
extern void gdi_resizer_deinit(void);
extern void gdi_resizer_set_want_sy(int32_t want_sy);


////////////////////////////////////////////////////////////////////////////////
// Image buffer operator API
////////////////////////////////////////////////////////////////////////////////
/* GDI_DIV_255(x) == int(x / 255 + 0.5) */
#define GDI_DIV_255(_x)                 ((((_x) + 128) * 257) >> 16)
/* GDI_DIV_15(x) == int(x / 15 + 0.5) */
#define GDI_DIV_15(_x)                  ((((_x) + 8) * 4369) >> 16)
/*
 * Divide by 31. The result will equal to (int)(x / 31 + 0.5)
 * Only support 0 <= x < 128 * 256
 */
#define GDI_DIV_31(_x) ((2114 * ((_x) + 16)) >> 16)
/*
 * Divide by 63. the result will equal to (int)(x / 63 + 0.5)
 * Only support 0 <= x < 256 * 256
 */
#define GDI_DIV_63(_x) ((4161 * ((_x) + 32)) >> 18)
/* GDI_BITS_5_to_8(x) ~= int(x * 255 / 31 + 0.5) */
#define GDI_BITS_5_to_8(_x)             (((_x) << 3) | ((_x) >> 2))
#define GDI_BITS_8_to_5(_x)             ((_x) >> 3)
/* GDI_BITS_6_to_8(x) ~= int(x * 255 / 63 + 0.5) */
#define GDI_BITS_6_to_8(_x)             (((_x) << 2) | ((_x) >> 4))
#define GDI_BITS_8_to_6(_x)             ((_x) >> 2)

#define GDI_BITS_5_to_6(_x)             (((_x) << 1) | ((_x) >> 4))
#define GDI_BITS_6_to_5(_x)             ((_x) >> 1)

#define GDI_MEM_GET_8(_ptr)             (*((uint8_t*)(_ptr)))
#define GDI_MEM_PUT_8(_ptr, _v)         (*((uint8_t*)(_ptr)) = _v)

#define GDI_MEM_GET_16(_ptr)            (*((uint16_t*)(_ptr)))
#define GDI_MEM_PUT_16(_ptr, _v)        (*((uint16_t*)(_ptr)) = _v)

#define GDI_MEM_GET_24(_ptr)            ((((uint8_t *)(_ptr))[0]) | (((uint8_t *)(_ptr))[1] << 8) | (((uint8_t*)(_ptr))[2] << 16))
#define GDI_MEM_PUT_24(_ptr, _v) \
    do \
    { \
        uint8_t *ptr8 = (uint8_t*)(_ptr); \
        ptr8[0] = _v; \
        ptr8[1] = _v >> 8; \
        ptr8[2] = _v >> 16; \
    } while (0)

#define GDI_MEM_GET_PTR_24(_buf_base, _width, _x, _y) \
    ((uint8_t*)(_buf_base) + ((_width) * (_y) + (_x)) * 3)

#define GDI_MEM_GET_32(_ptr)            (*((uint32_t*)(_ptr)))
#define GDI_MEM_PUT_32(_ptr, _v)        (*((uint32_t*)(_ptr)) = _v)

/*
 * Color format
 */
typedef enum
{
    GDI_IMG_BUF_COLOR_FORMAT_UNKNOWN    = 0x00,
    GDI_IMG_BUF_COLOR_FORMAT_RGB565     = 0x02,
    GDI_IMG_BUF_COLOR_FORMAT_RGB888     = 0x03,
    GDI_IMG_BUF_COLOR_FORMAT_ARGB8888   = 0x04,
    GDI_IMG_BUF_COLOR_FORMAT_PARGB8888  = 0x14,
    GDI_IMG_BUF_COLOR_FORMAT_PARGB6666  = 0x13
} gdi_img_buf_color_format_enum;

#define GDI_IMG_BUF_COLOR_FORMAT_PIXEL_BYTES(_format) (((int32_t)(_format)) & 0x0f)

/*
 * RGB565 color format
 */
#define GDI_RGB565(_r, _g, _b)          (((_r) << 11) | ((_g) << 5) | (_b))
#define GDI_R_OF_RGB565(_c)             (((_c) << 16) >> 27)
#define GDI_G_OF_RGB565(_c)             (((_c) << 21) >> 26)
#define GDI_B_OF_RGB565(_c)             (((_c) << 27) >> 27)

#define GDI_RGB565_FROM_RGB888(_c) \
    (GDI_RGB565( \
        GDI_BITS_8_to_5(GDI_R_OF_RGB888(_c)), \
        GDI_BITS_8_to_6(GDI_G_OF_RGB888(_c)), \
        GDI_BITS_8_to_5(GDI_B_OF_RGB888(_c))))

#define GDI_RGB565_FROM_ARGB8888(_c) \
    (GDI_RGB565( \
        GDI_BITS_8_to_5(GDI_R_OF_ARGB8888(_c)), \
        GDI_BITS_8_to_6(GDI_G_OF_ARGB8888(_c)), \
        GDI_BITS_8_to_5(GDI_B_OF_ARGB8888(_c))))

/*
 * RGB888 color format
 */
#define GDI_RGB888(_r, _g, _b)          (((_r) << 16) | ((_g) << 8) | (_b))
#define GDI_R_OF_RGB888(_c)             (((_c) <<  8) >> 24)
#define GDI_G_OF_RGB888(_c)             (((_c) << 16) >> 24)
#define GDI_B_OF_RGB888(_c)             (((_c) << 24) >> 24)

#define GDI_RGB888_FROM_RGB565(_c) \
    (GDI_RGB888( \
        GDI_BITS_5_to_8(GDI_R_OF_RGB565(_c)), \
        GDI_BITS_6_to_8(GDI_G_OF_RGB565(_c)), \
        GDI_BITS_5_to_8(GDI_B_OF_RGB565(_c))))

#define GDI_RGB888_FROM_ARGB8888(_c)      ((_c) & 0x00ffffff)

/*
 * ARGB8888 color format
 */
#define GDI_ARGB8888(_a, _r, _g, _b)    (((_a) << 24) | ((_r) << 16) | ((_g) << 8) | (_b))
#define GDI_A_OF_ARGB8888(_c)           ((_c) >> 24)
#define GDI_R_OF_ARGB8888(_c)           (((_c) <<  8) >> 24)
#define GDI_G_OF_ARGB8888(_c)           (((_c) << 16) >> 24)
#define GDI_B_OF_ARGB8888(_c)           (((_c) << 24) >> 24)

#define GDI_ARGB8888_FROM_RGB565(_c) \
    (GDI_ARGB8888( \
        (uint32_t)0xff, \
        GDI_BITS_5_to_8(GDI_R_OF_RGB565(_c)), \
        GDI_BITS_6_to_8(GDI_G_OF_RGB565(_c)), \
        GDI_BITS_5_to_8(GDI_B_OF_RGB565(_c))))
        
#define GDI_ARGB8888_FROM_RGB888(_c)      (((uint32_t)0xff << 24) | (_c))

/*
 * PARGB8888 color format
 */
#define GDI_PARGB8888(_a, _pr, _pg, _pb) (((_a) << 24) | ((_pr) << 16) | ((_pg) << 8) | (_pb))
#define GDI_A_OF_PARGB8888(_c)          ((_c) >> 24)
#define GDI_PR_OF_PARGB8888(_c)         (((_c) <<  8) >> 24)
#define GDI_PG_OF_PARGB8888(_c)         (((_c) << 16) >> 24)
#define GDI_PB_OF_PARGB8888(_c)         (((_c) << 24) >> 24)

#define GDI_PARGB8888_FROM_RGB565(_c) \
    (GDI_PARGB8888( \
        (uint32_t)0xff, \
        GDI_BITS_5_to_8(GDI_R_OF_RGB565(_c)), \
        GDI_BITS_6_to_8(GDI_G_OF_RGB565(_c)), \
        GDI_BITS_5_to_8(GDI_B_OF_RGB565(_c))))

#define GDI_PARGB8888_FROM_RGB888(_c)     (((uint32_t)0xff << 24) | (_c))

#define GDI_PARGB8888_FROM_ARGB8888(_c) \
    (GDI_PARGB8888( \
        GDI_A_OF_ARGB8888(_c), \
        GDI_DIV_255(GDI_A_OF_ARGB8888(_c) * GDI_R_OF_ARGB8888(_c)), \
        GDI_DIV_255(GDI_A_OF_ARGB8888(_c) * GDI_G_OF_ARGB8888(_c)), \
        GDI_DIV_255(GDI_A_OF_ARGB8888(_c) * GDI_B_OF_ARGB8888(_c))))

#define GDI_PARGB8888_FROM_PARGB6666(_c) \
    (GDI_PARGB8888( \
        GDI_BITS_6_to_8(GDI_A_OF_PARGB6666(_c)), \
        GDI_BITS_6_to_8(GDI_PR_OF_PARGB6666(_c)), \
        GDI_BITS_6_to_8(GDI_PG_OF_PARGB6666(_c)), \
        GDI_BITS_6_to_8(GDI_PB_OF_PARGB6666(_c)) ))

/*
 * PARGB6666 color format
 */
#define GDI_PARGB6666(_a, _pr, _pg, _pb) (((_a) << 18) | ((_pr) << 12) | ((_pg) << 6) | (_pb))
#define GDI_A_OF_PARGB6666(_c)          ((_c) >> 18)
#define GDI_PR_OF_PARGB6666(_c)         (((_c) << 14) >> 26)
#define GDI_PG_OF_PARGB6666(_c)         (((_c) << 20) >> 26)
#define GDI_PB_OF_PARGB6666(_c)         (((_c) << 26) >> 26)

#define GDI_PARGB6666_FROM_RGB565(_c) \
    (GDI_PARGB6666( \
        (uint32_t)0x3f, \
        GDI_BITS_5_to_6(GDI_R_OF_RGB565(_c)), \
        GDI_G_OF_RGB565(_c), \
        GDI_BITS_5_to_6(GDI_B_OF_RGB565(_c))))

#define GDI_PARGB6666_FROM_RGB888(_c) \
    (GDI_PARGB6666( \
        (uint32_t)0x3f, \
        GDI_BITS_8_to_6(GDI_R_OF_RGB888(_c)), \
        GDI_BITS_8_to_6(GDI_G_OF_RGB888(_c)), \
        GDI_BITS_8_to_6(GDI_B_OF_RGB888(_c))))

#define GDI_PARGB6666_FROM_ARGB8888(_c) \
    (GDI_PARGB6666( \
        GDI_BITS_8_to_6(GDI_A_OF_PARGB8888(_c)), \
        GDI_BITS_8_to_6(GDI_DIV_255(GDI_A_OF_ARGB8888(_c) * GDI_R_OF_ARGB8888(_c))), \
        GDI_BITS_8_to_6(GDI_DIV_255(GDI_A_OF_ARGB8888(_c) * GDI_G_OF_ARGB8888(_c))), \
        GDI_BITS_8_to_6(GDI_DIV_255(GDI_A_OF_ARGB8888(_c) * GDI_B_OF_ARGB8888(_c))) ))

#define GDI_PARGB6666_FROM_PARGB8888(_c) \
    (GDI_PARGB6666( \
        GDI_BITS_8_to_6(GDI_A_OF_PARGB8888(_c)), \
        GDI_BITS_8_to_6(GDI_PR_OF_PARGB8888(_c)), \
        GDI_BITS_8_to_6(GDI_PG_OF_PARGB8888(_c)), \
        GDI_BITS_8_to_6(GDI_PB_OF_PARGB8888(_c)) ))

#define GDI_PARGB6666_FROM_ARGB8888_EXT(_a, _r, _g, _b) \
    (GDI_PARGB6666( \
        GDI_BITS_8_to_6(_a), \
        GDI_BITS_8_to_6(GDI_DIV_255((_a) * (_r))), \
        GDI_BITS_8_to_6(GDI_DIV_255((_a) * (_g))), \
        GDI_BITS_8_to_6(GDI_DIV_255((_a) * (_b))) ))

#define GDI_RGB565_FROM_PARGB6666(_c) \
    (GDI_RGB565( \
        GDI_BITS_6_to_5(GDI_PR_OF_PARGB6666(_c)), \
        GDI_PG_OF_PARGB6666(_c), \
        GDI_BITS_6_to_5(GDI_PB_OF_PARGB6666(_c))))

/*
 * GDI image buffer structure
 */
typedef struct 
{
    uint8_t *ptr;    
    gdi_img_buf_color_format_enum color_format;
    int32_t pitch_bytes;
} gdi_img_buf_struct;

#endif /* _GDI_PRIMITIVE_H_ */ 


