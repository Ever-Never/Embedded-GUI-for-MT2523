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

#ifndef __GDI_IMGDEC_RESIZER_H__
#define __GDI_IMGDEC_RESIZER_H__

/***************************************************************************** 
 * Include
 *****************************************************************************/
#include "gdi_datatype.h"

/***************************************************************************** 
 * Define
 *****************************************************************************/

/***************************************************************************** 
 * Typedef
 *****************************************************************************/
struct _gdi_imgdec_resizer_struct
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

    gdi_layer_handle_t out_layer;
    void (*put_func)(struct _gdi_imgdec_resizer_struct *resizer, gdi_color_t c, bool want_draw);
};

typedef struct _gdi_imgdec_resizer_struct gdi_imgdec_resizer_struct;

typedef void (*gdi_imgdec_resizer_put_func)(gdi_imgdec_resizer_struct *resizer, gdi_color_t c, bool want_draw);

#define IS_GDI_IMGDEC_RESIZER_WANT_DRAW(resizer, SX,SY)   ((SX == (resizer)->want_sx) && (SY == (resizer)->want_sy))

#define GDI_IMGDEC_NON_RESIZER_PUT_X(resizer, COLOR,WANT_DRAW)    \
   do                                                             \
   {                                                              \
      if(WANT_DRAW)                                               \
         GDI_IMGDEC_RESIZER_PUT_PIXEL(COLOR);                     \
      GDI_IMGDEC_RESIZER_INCREASE_DEST;                           \
      if(resizer->want_sx == resizer->want_end_sx)          \
      {                                                           \
         if(resizer->want_sy == resizer->want_end_sy)       \
         {                                                        \
            resizer->want_sx = 0x0fffffff;/* very large number */\
         }                                                        \
         else/* move to next line begin */                         \
         {                                                        \
            resizer->want_sx = resizer->want_start_sx;      \
            resizer->want_sy += resizer->dir_y;             \
            resizer->dest += resizer->dest_pitch_jump;      \
         }                                                        \
      }                                                           \
      else                                                        \
      {                                                           \
         resizer->want_sx += resizer->dir_x;                \
      }                                                           \
   }while(0)

extern int32_t gdi_imgdec_resizer_init(
                gdi_imgdec_resizer_struct *resizer,
                gdi_layer_handle_t out_layer,
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

extern void gdi_imgdec_resizer_deinit(gdi_imgdec_resizer_struct *resizer);

#define gdi_imgdec_resizer_put(resizer, c, want_draw) (resizer)->put_func(resizer, c, want_draw)

/***************************************************************************** 
 * Global Function
 *****************************************************************************/

#endif /* __GDI_IMGDEC_RESIZER_H__ */



