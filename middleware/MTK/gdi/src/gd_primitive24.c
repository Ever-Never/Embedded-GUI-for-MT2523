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
#include "gdi_datatype.h"
#include "gdi.h"

/*****************************************************************************
 * FUNCTION
 *  gd_color_from_rgb_24
 * DESCRIPTION
 *  
 * PARAMETERS
 *  A       [IN]        
 *  R       [IN]        
 *  G       [IN]        
 *  B       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_color_t gd_color_from_rgb_24(uint8_t A, uint8_t R, uint8_t G, uint8_t B)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return (gdi_color_t) ((R << 16) | (G << 8) | B);
}


/*****************************************************************************
 * FUNCTION
 *  gd_color_to_rgb_24
 * DESCRIPTION
 *  
 * PARAMETERS
 *  A       [?]         
 *  R       [?]         
 *  G       [?]         
 *  B       [?]         
 *  c       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_color_to_rgb_24(uint8_t *A, uint8_t *R, uint8_t *G, uint8_t *B, gdi_color_t c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *A = 0xFF;
    *R = ((uint8_t) (c >> 16));
    *G = ((uint8_t) (c >> 8));
    *B = ((uint8_t) (c));
}

#if !defined(GDI_SLIM_COLOR_FORMAT)
/*****************************************************************************
 * FUNCTION
 *  gd_get_pixel_24
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x       [IN]        
 *  y       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_color_t gd_get_pixel_24(int x, int y)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t *ptr = (uint32_t*) (gdi_act_layer->buf_ptr + ((y * gdi_act_layer->width + x) * 3));

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (((uint32_t) ptr) & 0x3)
    {
        case 0:
            return ((*ptr) & 0x00ffffff);
        case 1:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            return ((*ptr) >> 8);
        case 2:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            return ((*ptr) >> 16) | ((*(ptr + 1) & 0x0ff) << 16);
        default:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            return ((*ptr) >> 24) | ((*(ptr + 1) & 0x0ffff) << 8);
    }
}


/*****************************************************************************
 * FUNCTION
 *  gd_put_pixel_24
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x       [IN]        
 *  y       [IN]        
 *  c       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_put_pixel_24(int x, int y, gdi_color_t c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t *ptr = (uint32_t*) (gdi_act_layer->buf_ptr + ((y * gdi_act_layer->width + x) * 3));

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (((uint32_t) ptr) & 0x3)
    {
        case 0:
            *ptr = (*ptr & 0xff000000) | (c & 0x00ffffff);
            break;
        case 1:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            *ptr = (*ptr & 0x000000ff) | (c << 8);
            break;
        case 2:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            *ptr = (*ptr & 0x0000ffff) | (c << 16);
            ptr++;
            *ptr = (*ptr & 0xffffff00) | ((c & 0x00ff0000) >> 16);
            break;
        default:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            *ptr = (*ptr & 0x00ffffff) | (c << 24);
            ptr++;
            *ptr = (*ptr & 0xffff0000) | ((c & 0x00ffff00) >> 8);
    }
}


/*****************************************************************************
 * FUNCTION
 *  gd_put_pixel_to_layer_24
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x       [IN]        
 *  y       [IN]        
 *  c       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_put_pixel_to_layer_24(gdi_layer_handle_t layer_handle, int x, int y, gdi_color_t c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct_t *layer = (gdi_layer_struct_t*)layer_handle;
    uint32_t *ptr = (uint32_t*) (layer->buf_ptr + ((y * layer->width + x) * 3));

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (((uint32_t) ptr) & 0x3)
    {
        case 0:
            *ptr = (*ptr & 0xff000000) | (c & 0x00ffffff);
            break;
        case 1:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            *ptr = (*ptr & 0x000000ff) | (c << 8);
            break;
        case 2:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            *ptr = (*ptr & 0x0000ffff) | (c << 16);
            ptr++;
            *ptr = (*ptr & 0xffffff00) | ((c & 0x00ff0000) >> 16);
            break;
        default:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            *ptr = (*ptr & 0x00ffffff) | (c << 24);
            ptr++;
            *ptr = (*ptr & 0xffff0000) | ((c & 0x00ffff00) >> 8);
    }
}

/*****************************************************************************
 * FUNCTION
 *  gd_get_buf_pixel_24
 * DESCRIPTION
 *  
 * PARAMETERS
 *  buf_ptr         [?]         
 *  buf_width       [IN]        
 *  x               [IN]        
 *  y               [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_color_t gd_get_buf_pixel_24(uint8_t *buf_ptr, uint32_t buf_width, int x, int y)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t *ptr = (uint32_t*) (buf_ptr + ((y * buf_width + x) * 3));

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (((uint32_t) ptr) & 0x3)
    {
        case 0:
            return ((*ptr) & 0x00ffffff);
        case 1:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            return ((*ptr) >> 8);
        case 2:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            return ((*ptr) >> 16) | ((*(ptr + 1) & 0x0ff) << 16);
        default:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            return ((*ptr) >> 24) | ((*(ptr + 1) & 0x0ffff) << 8);
    }
}


/*****************************************************************************
 * FUNCTION
 *  gd_put_buf_pixel_24
 * DESCRIPTION
 *  
 * PARAMETERS
 *  buf_ptr         [?]         
 *  buf_width       [IN]        
 *  x               [IN]        
 *  y               [IN]        
 *  c               [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_put_buf_pixel_24(uint8_t *buf_ptr, uint32_t buf_width, int x, int y, gdi_color_t c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t *ptr = (uint32_t*) (buf_ptr + ((y * buf_width + x) * 3));

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (((uint32_t) ptr) & 0x3)
    {
        case 0:
            *ptr = (*ptr & 0xff000000) | (c & 0x00ffffff);
            break;
        case 1:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            *ptr = (*ptr & 0x000000ff) | (c << 8);
            break;
        case 2:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            *ptr = (*ptr & 0x0000ffff) | (c << 16);
            ptr++;
            *ptr = (*ptr & 0xffffff00) | ((c & 0x00ff0000) >> 16);
            break;
        default:
            ptr = (uint32_t*) (((uint32_t) ptr) & (~0x3));
            *ptr = (*ptr & 0x00ffffff) | (c << 24);
            ptr++;
            *ptr = (*ptr & 0xffff0000) | ((c & 0x00ffff00) >> 8);
    }
}

#define  REPLACE_U24_0(PTR,SRCKEY)                   \
   {                                                 \
      gdi_color_t c = (*PTR)&0x00ffffff;               \
      if (SRCKEY == c)                               \
      {                                              \
         *PTR = (*PTR&0xff000000)|((c^1)&0x00ffffff);\
      }                                              \
   }while(0)                                         \

#define  REPLACE_U24_1(PTR,SRCKEY)                     \
   {                                                   \
      uint32_t *temp_ptr = (uint32_t*)(((uint32_t)PTR)&(~0x3));       \
      gdi_color_t c = (*temp_ptr)>>8;                    \
      if (SRCKEY == c)                                 \
      {                                                \
         *temp_ptr = (*temp_ptr&0x000000ff)|((c^1)<<8);\
      }                                                \
   }while(0)                                           \

#define  REPLACE_U24_2(PTR,SRCKEY)                                     \
   {                                                                   \
      uint32_t *temp_ptr = (uint32_t*)(((uint32_t)PTR)&(~0x3));                       \
      gdi_color_t c = ((*temp_ptr)>>16) | ((*(temp_ptr+1)&0x0ff  ) <<16);\
      if (SRCKEY == c)                                                 \
      {                                                                \
         *temp_ptr = (*temp_ptr&0x0000ffff)|((c^1)<<16); temp_ptr++;   \
         *temp_ptr = (*temp_ptr&0xffffff00)|(((c^1)&0x00ff0000)>>16);  \
      }                                                                \
   }while(0)                                                           \

#define  REPLACE_U24_3(PTR,SRCKEY)                                     \
   {                                                                   \
      uint32_t *temp_ptr = (uint32_t*)(((uint32_t)PTR)&(~0x3));                       \
      gdi_color_t c = ((*temp_ptr)>>24) | ((*(temp_ptr+1)&0x0ffff) << 8);\
      if (SRCKEY == c)                                                 \
      {                                                                \
         *temp_ptr = (*temp_ptr&0x00ffffff)|((c^1)<<24); temp_ptr++;   \
         *temp_ptr = (*temp_ptr&0xffff0000)|(((c^1)&0x00ffff00)>>8);   \
      }                                                                \
   }while(0)                                                           \


/*****************************************************************************
 * FUNCTION
 *  gd_replace_src_key_24
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest_ptr            [?]         
 *  dest_pitch          [IN]        
 *  dest_offset_x       [IN]        
 *  dest_offset_y       [IN]        
 *  clipx1              [IN]        
 *  clipy1              [IN]        
 *  clipx2              [IN]        
 *  clipy2              [IN]        
 *  src_key             [IN]        
 *  width               [IN]        
 *  height              [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_replace_src_key_24(
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
        int32_t height)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t *dest_line_start_ptr;

    int32_t jump_size;
    int32_t start_x, start_y, end;
    int32_t x_draw_count, y_draw_count;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    /* if not totally out of clip region */
    if (!((dest_offset_x > clipx2) ||
        (dest_offset_y > clipy2) || (dest_offset_x + width < clipx1) || (dest_offset_y + height < clipy1)))
    {
        /* find x_draw_count and y_draw_count */
        start_x = (dest_offset_x < clipx1) ? clipx1 : dest_offset_x;
        end = (dest_offset_x + (int32_t) width - 1 > clipx2) ? clipx2 : dest_offset_x + (int32_t) width - 1;
        x_draw_count = end - start_x + 1;

        start_y = (dest_offset_y < clipy1) ? clipy1 : dest_offset_y;
        end = (dest_offset_y + (int32_t) height - 1 > clipy2) ? clipy2 : dest_offset_y + (int32_t) height - 1;
        y_draw_count = end - start_y + 1;

        /* adjust dest ptr to the start of drawing point */
        /* find start ptr for actual drawing */
        jump_size = ((start_y * dest_pitch + start_x) * 24) >> 3;
        dest_line_start_ptr = dest_ptr + jump_size;
          
        {
            uint8_t *dest;
            int32_t x, y;
            int32_t x_draw_count_afer_shift;

            for (y = 0; y < y_draw_count; y++)
            {
                dest = (dest_line_start_ptr + ((dest_pitch * y * 24) >> 3));
                x = 0;

                switch (((uint32_t) dest) & 0x3)
                {
                    case 3:
                        REPLACE_U24_3(dest, src_key);
                        dest += 3;
                        if (++x >= x_draw_count)
                        {
                            break;
                        }
                    case 2:
                        REPLACE_U24_2(dest, src_key);
                        dest += 3;
                        if (++x >= x_draw_count)
                        {
                            break;
                        }
                    case 1:
                        REPLACE_U24_1(dest, src_key);
                        dest += 3;
                        if (++x >= x_draw_count)
                        {
                            break;
                        }
                        break;
                }
                x_draw_count_afer_shift = x_draw_count - x;
                if (x_draw_count_afer_shift > 3)
                {
                    for (x = 0; x < (x_draw_count_afer_shift & (~0x3)); x += 4)
                    {
                        REPLACE_U24_0(dest, src_key);
                        dest += 3;
                        REPLACE_U24_3(dest, src_key);
                        dest += 3;
                        REPLACE_U24_2(dest, src_key);
                        dest += 3;
                        REPLACE_U24_1(dest, src_key);
                        dest += 3;
                    }
                }
                switch (((uint32_t) x_draw_count_afer_shift) & 0x3)
                {
                    case 3:
                        REPLACE_U24_2(dest + 6, src_key);
                    case 2:
                        REPLACE_U24_3(dest + 3, src_key);
                    case 1:
                        REPLACE_U24_0(dest, src_key);
                        break;
                }
            }
        }
    }
}

#define PIXEL_TYPE                  uint32_t
#define PIXEL_BYTES                 3
#define GET_PIXEL(C,PTR)                                  \
         do                                               \
         {                                                \
            uint32_t *SS = (uint32_t*)PTR;                          \
            switch(((uint32_t)SS)&0x3)                         \
            {                                             \
            case 0:                                       \
               C = ((*SS)&0x00ffffff);                    \
               break;                                     \
            case 1:                                       \
               SS= (uint32_t*)(((uint32_t)SS)&(~0x3));              \
               C =((*SS)>>8);                             \
               break;                                     \
            case 2:                                       \
               SS= (uint32_t*)(((uint32_t)SS)&(~0x3));              \
               C = ((*SS)>>16) | ((*(SS+1)&0x0ff  ) <<16);\
               break;                                     \
            default:                                      \
               SS= (uint32_t*)(((uint32_t)SS)&(~0x3));              \
               C = ((*SS)>>24) | ((*(SS+1)&0x0ffff) << 8);\
               break;                                     \
            }                                             \
         }while(0)

#define PUT_PIXEL(C,PTR)                                   \
         do                                                \
         {                                                 \
            uint32_t *DD = (uint32_t*)PTR;                           \
            switch(((uint32_t)DD)&0x3)                          \
            {                                              \
            case 0:                                        \
               *DD = (*DD&0xff000000)|(C&0x00ffffff);      \
               break;                                      \
            case 1:                                        \
               DD = (uint32_t*)(((uint32_t)DD)&(~0x3));              \
               *DD = (*DD&0x000000ff)|(C<<8);              \
               break;                                      \
            case 2:                                        \
               DD = (uint32_t*)(((uint32_t)DD)&(~0x3));              \
               *DD = (*DD&0x0000ffff)|(C<<16); DD++;       \
               *DD = (*DD&0xffffff00)|((C&0x00ff0000)>>16);\
               break;                                      \
            default:                                       \
               DD = (uint32_t*)(((uint32_t)DD)&(~0x3));              \
               *DD = (*DD&0x00ffffff)|(C<<24); DD++;       \
               *DD = (*DD&0xffff0000)|((C&0x00ffff00)>>8); \
               break;                                      \
            }                                              \
         }while(0)

#define JUMP_PIXEL(PTR,N)           PTR = (((uint8_t*)PTR)+(N)* PIXEL_BYTES)
#define GD_RESIZE_BITBLT            gd_resize_bitblt_24
#define GD_BITBLT                   gd_bitblt_24
#define GD_MEMSET                   gd_memset_24
#define GD_FILL_DOT_RECTANGLE       gd_fill_dot_rect_24
#define GD_IMAGE_BITS_DRAW          gd_image_bits_draw_24
#include "gd_primitive_internal.h"

#endif
