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
 *  gd_color_from_rgb_8
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
gdi_color_t gd_color_from_rgb_8(uint8_t A, uint8_t R, uint8_t G, uint8_t B)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return (gdi_color_t) ((A << 24) | (R << 16) | (G << 8) | A);
}


/*****************************************************************************
 * FUNCTION
 *  gd_color_to_rgb_8
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
void gd_color_to_rgb_8(uint8_t *A, uint8_t *R, uint8_t *G, uint8_t *B, gdi_color_t c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *A = (uint8_t) (c >> 24);
    *R = (uint8_t) (c >> 16);
    *G = (uint8_t) (c >> 8);
    *A = (uint8_t) (c);
}


/*****************************************************************************
 * FUNCTION
 *  gd_get_pixel_8
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x       [IN]        
 *  y       [IN]        
 * RETURNS
 *  
 *****************************************************************************/
gdi_color_t gd_get_pixel_8(int x, int y)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return (gdi_color_t) * (uint8_t*) (gdi_act_layer->buf_ptr + ((y * gdi_act_layer->width + x)));
}


/*****************************************************************************
 * FUNCTION
 *  gd_put_pixel_8
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x       [IN]        
 *  y       [IN]        
 *  c       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_put_pixel_8(int x, int y, gdi_color_t c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *(uint8_t*) (gdi_act_layer->buf_ptr + ((y * gdi_act_layer->width + x))) = (uint8_t) c;
}


/*****************************************************************************
 * FUNCTION
 *  gd_put_pixel_to_layer_8
 * DESCRIPTION
 *  
 * PARAMETERS
 *  x       [IN]        
 *  y       [IN]        
 *  c       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void gd_put_pixel_to_layer_8(gdi_layer_handle_t layer_handle, int x, int y, gdi_color_t c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_layer_struct_t *layer = (gdi_layer_struct_t*)layer_handle;
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *(uint8_t*) (layer->buf_ptr + ((y * layer->width + x))) = (uint8_t) c;
}

/*****************************************************************************
 * FUNCTION
 *  gd_get_buf_pixel_8
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
gdi_color_t gd_get_buf_pixel_8(uint8_t *buf_ptr, uint32_t buf_width, int x, int y)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return (gdi_color_t) * (uint8_t*) (buf_ptr + ((y * buf_width + x)));
}


/*****************************************************************************
 * FUNCTION
 *  gd_put_buf_pixel_8
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
void gd_put_buf_pixel_8(uint8_t *buf_ptr, uint32_t buf_width, int x, int y, gdi_color_t c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    *(uint8_t*) (buf_ptr + ((y * buf_width + x))) = (uint8_t) c;
}


/*****************************************************************************
 * FUNCTION
 *  gd_replace_src_key_8
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
void gd_replace_src_key_8(
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

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    uint8_t *dest_line_start_ptr;

    int32_t jump_size;
    int32_t start_x, start_y, end;
    int32_t x_draw_count, y_draw_count;

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
        jump_size = ((start_y * dest_pitch + start_x) * 8) >> 3;
        dest_line_start_ptr = dest_ptr + jump_size;
        
        {
            uint8_t *dest;
            int32_t x, y;

            for (y = 0; y < y_draw_count; y++)
            {
                dest = (uint8_t*) (dest_line_start_ptr + ((dest_pitch * y * 8) >> 3));

                for (x = 0; x < x_draw_count; x++)
                {
                    uint8_t c = *dest;

                    if (c == src_key)
                    {
                        c ^= 1;
                        *dest = c;
                    }
                    dest++;
                }
            }
        }
    }
}

#define PIXEL_TYPE                  uint8_t
#define PIXEL_BYTES                 1
#define GET_PIXEL(C,PTR)            C=*((PIXEL_TYPE*)(PTR))
#define PUT_PIXEL(C,PTR)            *(PIXEL_TYPE*)(PTR) = (PIXEL_TYPE) C
#define JUMP_PIXEL(PTR,N)           PTR = (((uint8_t*)PTR)+(N)* PIXEL_BYTES)

#define GD_RESIZE_BITBLT            gd_resize_bitblt_8
#define GD_BITBLT                   gd_bitblt_8
#define GD_MEMSET                   gd_memset_8
#define GD_FILL_DOT_RECTANGLE       gd_fill_dot_rect_8
#define GD_IMAGE_BITS_DRAW          gd_image_bits_draw_8
#include "gd_primitive_internal.h"
