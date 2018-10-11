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

#ifndef _GDI_LAYER_H_
#define _GDI_LAYER_H_

#include "gdi_features.h"
#include "gdi_datatype.h"
#include "gdi_const.h"

/* this data structure is defined in lcd_if.h, but will not include when build on older platform */
/* we should add by ourself */

#ifdef GDI_4_LAYERS
#define GDI_LAYER_ENABLE_LAYER_ALL        (LCD_LAYER0_ENABLE|LCD_LAYER1_ENABLE\
                                 |LCD_LAYER2_ENABLE|LCD_LAYER3_ENABLE         \
                                 )
#else
#define GDI_LAYER_ENABLE_LAYER_ALL        (LCD_LAYER0_ENABLE|LCD_LAYER1_ENABLE)
#endif

/* user define handle */
#ifndef __MMI_SUBLCD__
#define GDI_LAYER_USER_ALLOCATE_HANDLE_START (1)
#else 
#define GDI_LAYER_USER_ALLOCATE_HANDLE_START (2)
#endif 
#define GDI_LAYER_USER_ALLOCATE_HANDLE_END   (GDI_LAYER_TOTAL_LAYER_COUNT-1)

#define GDI_LAYER_VALID_HANDLE_START            ((gdi_layer_handle_t)&GDI_LAYERS[0])
/****************************************************************************
* Struct / Typedef                                                                 
*****************************************************************************/

// TODO: merge this struct to gdi_layer_struct_t*, remove duplicate variable between this two structures.
typedef struct
{
   uint8_t   rotate_value;           /* (keep)rotate select for specified layer */
   uint8_t   opacity_value;          /* (keep)opacity value for specified layer */
   bool      opacity_enable;         /* (keep)enable/disable opacity for specified layer */
   bool      source_key_enable;      /* (keep)enable/disable source key for specified layer */
   uint32_t  source_key;             /* (keep)source key color in RGB565 format for specified layer */
   int32_t   x_offset;               /* (keep)x axis offset from main window for specified layer, should only refrence by lcd */
   int32_t   y_offset;               /* (keep)y axis offset from main widnow for specified layer, should only refrence by lcd */
   uint32_t  frame_buffer_address;   /* frame buffer start address of specified layer */
   bool      dither_enable;          /* (keep)enable/disable source key for specified layer */
   bool      color_palette_enable;   /* enable/disable color palette for specified layer */
   uint8_t   color_palette_select;   /* selection of color palette table */
   uint8_t   source_color_format;    /* color format of the specified layer */   
} gdi_lcd_layer_struct;

extern gdi_lcd_layer_struct gdi_layer_info[GDI_LAYER_TOTAL_LAYER_COUNT];    /* this struct is used for output lcd */
extern gdi_lcd_layer_struct *gdi_act_layer_info;

#define GDI_LAYER_FLAG_USED               (1<<0)
#define GDI_LAYER_FLAG_DOUBLE_LAYER       (1<<1)
#define GDI_LAYER_FLAG_USE_OUTSIDE_MEMORY (1<<2)
#define GDI_LAYER_FLAG_FROZEN             (1<<3)
#define GDI_LAYER_FLAG_LAZY_FREE          (1<<4)
#define GDI_LAYER_FLAG_HW_UPDATE          (1<<5)

#define GDI_LAYER_CLEAR_FLAG(HANDLE,FLAG)    ((gdi_layer_struct_t*)HANDLE)->flag  &= ~(uint32_t)(FLAG)
#define GDI_LAYER_SET_FLAG(HANDLE,FLAG)      ((gdi_layer_struct_t*)HANDLE)->flag  |= (FLAG)
#define GDI_LAYER_GET_FLAG(HANDLE,FLAG)      (((gdi_layer_struct_t*)HANDLE)->flag  & (FLAG))

#define GDI_LAYER_SET_CLIP(HANDLE,X1,Y1,X2,Y2)  \
   do                                           \
   {                                            \
      ((gdi_layer_struct_t*)HANDLE)->clipx1=X1;   \
      ((gdi_layer_struct_t*)HANDLE)->clipy1=Y1;   \
      ((gdi_layer_struct_t*)HANDLE)->clipx2=X2;   \
      ((gdi_layer_struct_t*)HANDLE)->clipy2=Y2;   \
   }while(0)

#define GDI_LAYER_RESET_CLIPS(HANDLE)  ((gdi_layer_struct_t*)HANDLE)->clips_top = 0

#define GDI_LAYER_SET_BLT_RECT(HANDLE,X,Y,W,H)  \
   do                                           \
   {                                            \
      ((gdi_layer_struct_t*)HANDLE)->blt_x=X;   \
      ((gdi_layer_struct_t*)HANDLE)->blt_y=Y;   \
      ((gdi_layer_struct_t*)HANDLE)->blt_width=W;   \
      ((gdi_layer_struct_t*)HANDLE)->blt_height=H;   \
   }while(0)

#define GDI_LAYER_MAX_WIDTH ((LCD_LAYER_MAX_WIDTH < FLATTEN_LAYER_MAX_WIDTH)? LCD_LAYER_MAX_WIDTH: FLATTEN_LAYER_MAX_WIDTH)
#define GDI_LAYER_MAX_HEIGHT ((LCD_LAYER_MAX_HEIGHT < FLATTEN_LAYER_MAX_HEIGHT)? LCD_LAYER_MAX_HEIGHT: FLATTEN_LAYER_MAX_HEIGHT)              
#define GDI_LAYER_MAX_OFFSETX ((LCD_LAYER_MAX_OFFSETX < FLATTEN_LAYER_MAX_OFFSETX)? LCD_LAYER_MAX_OFFSETX: FLATTEN_LAYER_MAX_OFFSETX)
#define GDI_LAYER_MAX_OFFSETY ((LCD_LAYER_MAX_OFFSETY< FLATTEN_LAYER_MAX_OFFSETY)? LCD_LAYER_MAX_OFFSETY: FLATTEN_LAYER_MAX_OFFSETY)

extern gd_replace_src_key_func gdi_act_replace_src_key;

/***************************************************************************** 
* Function 
*****************************************************************************/
/* layers related */
extern gdi_result_t gdi_layer_init(gdi_color_format_t color_format, uint8_t *buffer);

extern gdi_result_t gdi_layer_blend_2_layers(
                    gdi_layer_handle_t src_layer_handle, 
                    int32_t x1, 
                    int32_t y1, 
                    int32_t x2, 
                    int32_t y2);

extern gdi_result_t gdi_layer_flatten_ext_internal(
                    gdi_layer_handle_t handle0, 
                    gdi_layer_handle_t handle1, 
                    gdi_layer_handle_t handle2, 
                    gdi_layer_handle_t handle3,
                    int32_t x1,
                    int32_t y1,
                    int32_t x2,
                    int32_t y2,
                    uint8_t* output_buf_ptr,
                    int32_t output_buf_size,
                    uint16_t output_buf_w, 
                    uint16_t output_buf_h,
                    int32_t output_buf_offset_x,
                    int32_t output_buf_offset_y,
                    uint8_t output_buf_bits_per_pixel,
                    uint8_t output_buf_cf,
                    uint8_t output_buf_rotate_value,
                    gdi_layer_flatten_flag_bitset flag);

#endif /* _GDI_LAYER_H_ */ 


