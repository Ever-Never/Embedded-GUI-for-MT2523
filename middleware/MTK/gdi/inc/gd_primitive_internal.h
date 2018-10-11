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

#ifndef GD_PRIMITIVE_INTERNAL_H
#define GD_PRIMITIVE_INTERNAL_H

#include <string.h>
#include "gdi_datatype.h"
#include "gdi_const.h"
#include "gdi_primitive.h"

/*****************************************************************************
 * FUNCTION
 *  GD_RESIZE_BITBLT
 * DESCRIPTION
 *****************************************************************************/
void GD_RESIZE_BITBLT(
        uint8_t *dest, int32_t dw, int32_t dh, int32_t dx1, int32_t dy1, int32_t dx2, int32_t dy2,
        uint8_t *src,  int32_t sw, int32_t sh, int32_t sx1, int32_t sy1, int32_t sx2, int32_t sy2,
        int32_t clipx1,int32_t clipy1,int32_t clipx2,int32_t clipy2,
        gdi_color_t src_key, gdi_color_t layer_key)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t *d;
    int32_t rx1, ry1, rx2, ry2; /* real destination */
    int32_t rox, roy;           /* (dx1,dy1) is (0,0) , (rox,roy) is the distance of (rx1,ry1) to (0,0) */
    int32_t rw, rh;
    uint32_t xratio;
    uint32_t yratio;

    /* calculate (xratio,yratio) */
    {
        int32_t dw1;
        int32_t dh1;

        /*----------------------------------------------------------------*/
        /* Code Body                                                      */
        /*----------------------------------------------------------------*/
        dw1 = dx2 - dx1 + 1;
        dh1 = dy2 - dy1 + 1;
        if (dw1 <= 0 || dh1 <= 0)
        {
            return; /* nothing to do */
        }

        xratio = ((sx2 - sx1 + 1) << 16) / dw1;
        yratio = ((sy2 - sy1 + 1) << 16) / dh1;
    }

    rx1 = dx1;
    ry1 = dy1;
    rx2 = dx2;
    ry2 = dy2;
    GDI_CLIP_TWO_RECT(rx1, ry1, rx2, ry2, clipx1, clipy1, clipx2, clipy2, return);

    rw = rx2 - rx1 + 1;
    rh = ry2 - ry1 + 1;

    rox = rx1 - dx1;
    roy = ry1 - dy1;

    d = (uint8_t*) (dest + (rx1 + ry1 * dw) * PIXEL_BYTES);

    /* start to resample */
    {
        int32_t dx, dy, sx, sy;

        for (dy = 0; dy < rh; dy++)
        {
            sy = (((dy + roy) * yratio) >> 16) + sy1;
            if(sy<0) sy=0; else if(sy>=sh) sy=sh-1;
			
            for (dx = 0; dx < rw; dx++, JUMP_PIXEL(d, 1))
            {
                PIXEL_TYPE c;

                sx = (((dx + rox) * xratio) >> 16) + sx1;
				if(sx<0) sx=0; else if(sx>=sw) sx=sw-1;
                GET_PIXEL(c, (src + (sy * sw + sx) * PIXEL_BYTES));
                if (c != src_key)
                {
                    if (c == layer_key)
                    {
                        c ^= 1;
                    }
                    PUT_PIXEL(c, d);
                }

            }
            JUMP_PIXEL(d, (dw - rw));
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  GD_BITBLT
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest            [?]         
 *  dw              [IN]        
 *  dh              [IN]        
 *  dx1             [IN]        
 *  dy1             [IN]        
 *  dx2             [IN]        
 *  dy2             [IN]        
 *  src             [?]         
 *  sw              [IN]        
 *  sh              [IN]        
 *  sx1             [IN]        
 *  sy1             [IN]        
 *  sx2             [IN]        
 *  sy2             [IN]        
 *  clipx1          [IN]        
 *  clipy1          [IN]        
 *  clipx2          [IN]        
 *  clipy2          [IN]        
 *  src_key         [IN]        
 *  layer_key       [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void GD_BITBLT(
        uint8_t *dest,int32_t dw, int32_t dh, int32_t dx1, int32_t dy1, int32_t dx2, int32_t dy2,
        uint8_t *src,int32_t sw,int32_t sh,int32_t sx1,int32_t sy1, int32_t sx2, int32_t sy2,
        int32_t clipx1, int32_t clipy1, int32_t clipx2, int32_t clipy2,
        gdi_color_t src_key, gdi_color_t layer_key)
{
    int32_t w, h;

    /* process clipping */
    {
        int32_t old_dx, old_dy;
        int32_t offset_x, offset_y;

        /*----------------------------------------------------------------*/
        /* Code Body                                                      */
        /*----------------------------------------------------------------*/
        old_dx = dx1;
        old_dy = dy1;

        GDI_CLIP_TWO_RECT(dx1, dy1, dx2, dy2, clipx1, clipy1, clipx2, clipy2, return);
        offset_x = dx1 - old_dx;
        offset_y = dy1 - old_dy;

        sx1 += offset_x;
        sy1 += offset_y;
    }

    /* calculate the copy area */
    {
        int32_t a, b;

        a = dx2 - dx1 + 1;
        b = sx2 - sx1 + 1;
        w = GDI_MIN(a, b);

        a = dy2 - dy1 + 1;
        b = sy2 - sy1 + 1;
        h = GDI_MIN(a, b);
        if (w <= 0 || h <= 0)
        {
            return;
        }
    }

    /* start to copy data */
    {
        uint8_t *s;
        uint8_t *d;
        int32_t x, y;

        s = (uint8_t*) (src + (sx1 + sy1 * sw) * PIXEL_BYTES);
        d = (uint8_t*) (dest + (dx1 + dy1 * dw) * PIXEL_BYTES);

        for (y = h - 1; y >= 0; y--)
        {
            if ((src_key == 0x01ffffff) && (src_key == layer_key))
            {
                memcpy(d, s, w * PIXEL_BYTES);
                JUMP_PIXEL(s, sw);
                JUMP_PIXEL(d, dw);
            }
            else
            {
                for (x = w - 1; x >= 0; x--)
                {
                    PIXEL_TYPE c;

                    GET_PIXEL(c, s);
                    if (c != src_key)
                    {
                        if (c == layer_key)
                        {
                            c ^= 1;
                        }
                        PUT_PIXEL(c, d);
                    }
                    JUMP_PIXEL(d, 1);
                    JUMP_PIXEL(s, 1);
                }
                JUMP_PIXEL(s, (sw - w));
                JUMP_PIXEL(d, (dw - w));
            }
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  GD_MEMSET
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest        [?]         
 *  value       [IN]        
 *  len         [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
void GD_MEMSET(uint8_t *dest, uint32_t value, uint32_t len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    PIXEL_TYPE c = (PIXEL_TYPE) value;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (len--)
    {
        PUT_PIXEL(c, dest);
        JUMP_PIXEL(dest, 1);
    }
}

void GD_FILL_DOT_RECTANGLE(
        uint8_t *dest,
        int32_t dw,
        int32_t dh,
        int32_t dx1,
        int32_t dy1,
        int32_t dx2,
        int32_t dy2,
        int32_t clipx1,
        int32_t clipy1,
        int32_t clipx2,
        int32_t clipy2,
        gdi_color_t c)
{
        uint8_t *d;
        int32_t x, y;
        int32_t w, h;
        
        /* process clipping */
        GDI_CLIP_TWO_RECT(dx1, dy1, dx2, dy2, clipx1, clipy1, clipx2, clipy2, return);
        w = dx2-dx1+1;
        h = dy2-dy1+1;

        d = (uint8_t*) (dest + (dx1 + dy1 * dw) * PIXEL_BYTES);
        for (y = h - 1; y >= 0; y--)
        {
            if((y&1))
            {
                x = w-1;
            }
            else
            {
                x = w-2;
                JUMP_PIXEL(d,1);
            }
            for (; x >= 0; x-=2)
            {
                PUT_PIXEL(c, d);
                JUMP_PIXEL(d, 2);
            }
            JUMP_PIXEL(d, (dw - w+x+1));
        }
}
void GD_IMAGE_BITS_DRAW(
                            uint8_t *dest,int32_t dw, int32_t dh, int32_t dx1, int32_t dy1, int32_t dx2, int32_t dy2,
                            uint8_t *src, int32_t sw,int32_t sh,int32_t sx1,int32_t sy1, int32_t sx2, int32_t sy2,
                            int32_t src_size,int32_t bits_per_pixel,gdi_color_t palette[],
                            int32_t clipx1, int32_t clipy1, int32_t clipx2, int32_t clipy2)

{
    int32_t w, h;
    int32_t drop_cnt;

    if(src==NULL || src_size<=0) return;
    
    /* process clipping */
    {
        int32_t old_dx, old_dy;
        int32_t offset_x, offset_y;

        /*----------------------------------------------------------------*/
        /* Code Body                                                      */
        /*----------------------------------------------------------------*/
        old_dx = dx1;
        old_dy = dy1;

        GDI_CLIP_TWO_RECT(dx1, dy1, dx2, dy2, clipx1, clipy1, clipx2, clipy2, return);
        offset_x = dx1 - old_dx;
        offset_y = dy1 - old_dy;

        drop_cnt = offset_y * sw + offset_x;
        sx1 += offset_x;
        sy1 += offset_y;
    }

    /* calculate the copy area */
    {
        int32_t a, b;

        a = dx2 - dx1 + 1;
        b = sx2 - sx1 + 1;
        w = GDI_MIN(a, b);

        a = dy2 - dy1 + 1;
        b = sy2 - sy1 + 1;
        h = GDI_MIN(a, b);
        if (w <= 0 || h <= 0)
            return;
    }

    /* start to copy data */
    {
        uint8_t *s,*s_end;
        uint8_t *d,*d_end;
        uint32_t data;
        int32_t data_bits;
        uint32_t index;
        uint32_t bit_mask = (1<<bits_per_pixel)-1;
           
        // only support bits_per_pixel <=24 otherwise this algorithm need to be rewrite
        GDI_DEBUG_ASSERT(bits_per_pixel<=24); 

        s = (uint8_t*) src;
        s_end = ((uint8_t*) src) + src_size;
        d = (uint8_t*) (dest + (dx1 + dy1 * dw) * PIXEL_BYTES);
        d_end  = (uint8_t*) (dest + (dx2 + dy2 * dw) * PIXEL_BYTES);
        data = data_bits = 0;

        
        while(1)
        {
            if(data_bits<bits_per_pixel) 
			{
				data = (data<<8) | *s;s++;data_bits+=8;// read 1 byte data
				if(s >= s_end && data_bits<bits_per_pixel) break; // still not enough
				continue;
			}
            
            // fetch one pixel data
            data_bits -= bits_per_pixel;
            index = (data >> data_bits) & bit_mask;

            // put pixel
            drop_cnt--;
            if(drop_cnt<0)
            {
                PUT_PIXEL(palette[index], d);
                JUMP_PIXEL(d, 1);
                if(drop_cnt<= -w)  // move to next line
                {
                    drop_cnt = sw-w;
                    JUMP_PIXEL(d, (dw-w));
                    if(d>d_end) break;
                }
            }
            
        }
    }
}

#endif /* GD_PRIMITIVE_INTERNAL_H */ 


