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

#include "gdi_const.h"
#include "gdi_datatype.h"
#include "gdi_internal.h"
#include "gdi.h"
#include "gdi_font_show_char.h"

const uint32_t gdi_tilt_table[61] = {0, 0, 0, 0, 0, 1, 1, 1, 1, 2,
    2, 2, 2, 2, 3, 3, 3, 3, 4, 4,
    4, 4, 5, 5, 5, 5, 5, 6, 6, 6,
    6, 7, 7, 7, 7, 8, 8, 8, 8, 8,
    9, 9, 9, 9, 10, 10, 10, 10, 11, 11,
    11, 11, 11, 12, 12, 12, 12, 13, 13, 13, 13
};
#define MAX_TILT  13

static int32_t text_clip_x1;    /* Left edge of text clip rectangle */
static int32_t text_clip_y1;    /* Top edge of text clip rectangle  */
static int32_t text_clip_x2;    /* Right edge of text clip rectangle   */
static int32_t text_clip_y2;    /* Bottom edge of text clip rectangle  */

#define GDI_CHECK_CHAR_CLIP(x,y,char_width,char_height,NULL_ACTION)                 \
   {                                                                                \
      if((x -1 > text_clip_x2   )    || (x+char_width  +MAX_TILT +1 < text_clip_x1) \
      ||(y -1 > text_clip_y2) || (y+char_height +1 < text_clip_y1))                 \
         NULL_ACTION;                                                               \
   }                                                                                \


static gd_put_pixel_func put_pixel;

#define put_pixel_with_clip(x,y,c)                    \
   {                                                  \
      if((x >= text_clip_x1) && (x <= text_clip_x2)   \
      &&(y >= text_clip_y1) && (y <= text_clip_y2))   \
         put_pixel(x,y,c);                            \
   }

#define __GDI_SHOW_CHAR__  gdi_show_char_no_clip
#define __GDI_SET_PIXEL__  put_pixel
#include "gdi_font_show_char_internal.h"
#undef __GDI_SHOW_CHAR__
#undef __GDI_SET_PIXEL__

#define __GDI_SHOW_CHAR__  gdi_show_char_with_clip
#define __GDI_SET_PIXEL__  put_pixel_with_clip
#include "gdi_font_show_char_internal.h"
#undef __GDI_SHOW_CHAR__
#undef __GDI_SET_PIXEL__


/* 
 * ABM font engine 
 */
 /*
 * bitstream reader
 */
#define BIT_INIT(mem_ptr)               \
    {                                   \
        uint32_t BR_bits_buf;                \
        uint16_t *BR_bits_mem_ptr;           \
        uint32_t BR_bits_left;               \
                                        \
        BR_bits_mem_ptr = (uint16_t*)(mem_ptr);              \
        BR_bits_buf = 0;                \
        BR_bits_left = 0;                               \
                                                        \
        /* take care of unalignment */                  \
        if ((uint32_t)BR_bits_mem_ptr & 1)                   \
        {                                               \
            uint8_t *BR_mem_8 = (uint8_t*)BR_bits_mem_ptr;        \
                                                        \
            BR_bits_buf = *BR_mem_8++;                  \
            BR_bits_mem_ptr = (uint16_t*)BR_mem_8;           \
            BR_bits_left = 8;                           \
        }
  
#define BIT_END() }

#define BIT_READ(cb, data_mask, bit_data)  BR_bit_reader((cb), (data_mask), BR_bits_buf, BR_bits_left, BR_bits_mem_ptr, bit_data)
#define BIT_SEEK(s)                        BR_bit_seek((s), BR_bits_buf, BR_bits_left, BR_bits_mem_ptr)

/* bitstream read, output bit_data */
#define BR_bit_reader(cb, data_bit_mask, bits_buf, bits_left, bits_mem_ptr, bit_data) \
    do                                                                              \
    {                                                                               \
        /* bits in the buffer are not enough, append the buffer with 16 bits  */    \
        if (bits_left < cb)                                                         \
        {                                                                           \
            bits_buf += (*bits_mem_ptr++) << bits_left;                             \
            bits_left += 16;                                                        \
        }                                                                           \
                                                                                    \
        /* consume bits */                                                          \
        bit_data = bits_buf & (data_bit_mask);                                      \
        bits_buf >>= cb;                                                            \
        bits_left -= cb;                                                            \
    } while (0)


/* bitstream seek */
#define BR_bit_seek(seek_bits, bits_buf, bits_left, bits_mem_ptr)  \
    do                                                              \
    {                                                               \
        if (bits_left > seek_bits)                                  \
        {                                                           \
            bits_left -= seek_bits;                                 \
            bits_buf >>= seek_bits;                                 \
        }                                                           \
        else                                                        \
        {                                                           \
            uint32_t sb = seek_bits;                                     \
                                                                    \
            sb -= bits_left;                                        \
            bits_mem_ptr += sb / 16;                                \
            bits_buf = *bits_mem_ptr++;                             \
            sb &= 0xF;                                              \
            /* consume skipped bits */                              \
            bits_buf >>= sb;                                        \
            bits_left = 16 - sb;                                    \
        }                                                           \
    } while (0)
#define max(a,b)        (((a) > (b)) ? (a) : (b))
#define min(a,b)        (((a) < (b)) ? (a) : (b))

#define PUT_PIXEL_16(bpp, mem, p)    *((uint16_t*)(mem)) = (uint16_t)p;
#define PUT_PIXEL_dst_bpp(bpp, mem, p)          \
    do                                  \
    {                                   \
        if (bpp == 24){                 \
            /*uint8_t *mem = (mem);*/       \
            ((uint8_t*)mem)[0] = p;                 \
            ((uint8_t*)mem)[1] = p >> 8;            \
            ((uint8_t*)mem)[2] = p >> 16;           \
        }else if (bpp == 8){            \
            *((uint8_t*)(mem)) = (uint8_t)p;      \
        }else if (bpp == 16){           \
            *((uint16_t*)(mem)) = (uint16_t)p;    \
        }else{                          \
            *((uint32_t*)(mem)) = (uint32_t)p;    \
        }                               \
    } while (0)    
              
#define PUT_PIXEL(bpp, mem, p)  PUT_PIXEL_##bpp(bpp, mem,p);

#define OUTPUT_BORDER(border_buf, output_border_start, output_border_end, dst_bpp, not_draw_left_border)   \
    do                                                              \
    {                                                               \
        uint32_t tmp_line_buf2;                                          \
        int32_t i;                                                      \
        tmp_line_buf2 = border_buf >> not_draw_left_border;         \
        for (i = output_border_start; i >= output_border_end; i--)  \
        {                                                           \
            if (tmp_line_buf2 & 1){                                 \
                PUT_PIXEL(dst_bpp, out_mem_ptr, border_color);      \
            }                                                       \
            out_mem_ptr += dst_byte_per_pixel;                      \
            tmp_line_buf2 = tmp_line_buf2 >> 1;                     \
        }                                                           \
    }while(0)


static void gdi_font_show_border_of_char(
                int32_t x,
                int32_t y,
                gdi_color_t border_color,
                uint8_t *font_data,
                uint32_t font_data_size,
                uint16_t char_width,
                uint16_t char_height,
                uint8_t font_attr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    /* 16-byte ABM header for font information */
    uint32_t font_buf;                                   //buffer to store data in font data
    int32_t clip_width, clip_height;                    //char width and height after clipping
    uint32_t abm_start_bit_offset;                       //bit offset of first pixel to draw in font data
    uint32_t dst_line_jump_bytes, abm_line_jump_bits;    //offset to jump to first pixel to draw in the next line
    uint32_t bpp, dst_bpp, dst_byte_per_pixel;           //bits per pixel in source, destination
    uint8_t  *out_mem_ptr = 0;                             //points to current output memory
    uint8_t  not_draw_up_border, not_draw_down_border, not_draw_left_border, not_draw_right_border;  //decide to draw the border or not after cliping
   
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    //pre process, the section is a simplify version of gdi_image_abm_pre_process()
    {
        uint32_t     abm_start_pixel_offset;
        uint32_t     img_w, img_h;
        int32_t     x1, x2, y1, y2;
        uint32_t     line_jump_pixel_num;
        int32_t     layer_width;
        int32_t     clip_rect_x1, clip_rect_y1, clip_rect_x2, clip_rect_y2;
        int32_t     border_clip_rect_x1, border_clip_rect_y1, border_clip_rect_x2, border_clip_rect_y2;
        gdi_layer_handle_t dst_layer;
        gdi_layer_struct_t *dst_layer_struct;
        
        img_w = (uint32_t)char_width;
        img_h = (uint32_t)char_height;    
        x1 = x;
        y1 = y;
        x2 = x1 + img_w - 1;
        y2 = y1 + img_h - 1;
        gdi_layer_get_active(&dst_layer);
        dst_layer_struct = (gdi_layer_struct_t*)dst_layer;
        /* check if the draw rect is out of the clip */
        if ((x1 > dst_layer_struct->clipx2) || (y1 > dst_layer_struct->clipy2) || (x2 < dst_layer_struct->clipx1) || (y2 < dst_layer_struct->clipy1))
        {
            return ;
        }

        /* calculate the overlapped rect */
        clip_rect_y1 = max(y1, dst_layer_struct->clipy1);
        clip_rect_y2 = min(y2, dst_layer_struct->clipy2);
        clip_rect_x1 = max(x1, dst_layer_struct->clipx1);
        clip_rect_x2 = min(x2, dst_layer_struct->clipx2);

        /* calculate the draw rect of border */
        border_clip_rect_y1 = max(y1-1, dst_layer_struct->clipy1);
        border_clip_rect_y2 = min(y2+1, dst_layer_struct->clipy2);
        border_clip_rect_x1 = max(x1-1, dst_layer_struct->clipx1);
        border_clip_rect_x2 = min(x2+1, dst_layer_struct->clipx2);

        //decide to draw border or not
        not_draw_up_border = (clip_rect_y1 == border_clip_rect_y1)? 1:0;
        not_draw_down_border = (clip_rect_y2 == border_clip_rect_y2)? 1:0;
        not_draw_left_border = (clip_rect_x1 == border_clip_rect_x1)? 1:0;
        not_draw_right_border = (clip_rect_x2 == border_clip_rect_x2)? 1:0;
        
        bpp = 1;    
        abm_start_pixel_offset = clip_rect_x1 - x1 + (clip_rect_y1 - y1) * img_w;
        abm_start_bit_offset = abm_start_pixel_offset * bpp;

        layer_width = GDI_LAYER.width;
        clip_width = clip_rect_x2 - clip_rect_x1 + 1;
        clip_height = clip_rect_y2 - clip_rect_y1 + 1;
        line_jump_pixel_num = layer_width - clip_width;

        dst_bpp = gdi_layer_get_bit_per_pixel();
        dst_byte_per_pixel = dst_bpp / 8;
        dst_line_jump_bytes = line_jump_pixel_num * dst_byte_per_pixel;

        // bits to skipped for a line jump of the source ABM 
        abm_line_jump_bits = (img_w - clip_width) * bpp;

        // set memory pointers to the start pixel 
        out_mem_ptr = (uint8_t*)GDI_LAYER.buf_ptr;
        out_mem_ptr += (clip_rect_y1 * layer_width + clip_rect_x1) * dst_byte_per_pixel;
    }
    {
        BIT_INIT(font_data);
        BIT_SEEK(abm_start_bit_offset);

        {
            int32_t y = clip_height;
            uint32_t border_buf0, border_buf1, border_buf2, readpixelnum;
            int32_t output_border_start, output_border_end;

            border_buf0 = 0;    //border_buf0,1,2 is bit mask used to store the border
            border_buf1 = 0;
            border_buf2 = 0;
            
            readpixelnum = 16;  // the max number that the Bit reader can read
            
            if ((dst_bpp == 16) && (clip_width < readpixelnum))
            {
                // if dsp_bpp is 16 and clip_width<readpixelnum, we use the faster algorithm
                uint32_t mask, border_buf_len;
                readpixelnum = clip_width;
                border_buf_len = readpixelnum + 2;
                mask = (1 << readpixelnum) - 1;

                output_border_start = (border_buf_len - 1) - not_draw_left_border;
                output_border_end = not_draw_right_border;
                while (y > 0)                                                                  
                {
                    uint32_t tmp_line_buf;                       
                    uint8_t *ori_output_mem;
                    
                    border_buf2 = 0;
                    // read the pixel from bitstream 
                    BIT_READ(readpixelnum, mask, font_buf); 

                    //find the border, store in border_buf0,1,2
                    border_buf0 |= (font_buf);              
                    border_buf1 |= (font_buf);              
                    border_buf2 |= (font_buf);              
                    tmp_line_buf = font_buf << 1;           
                    border_buf0 |= tmp_line_buf;            
                    border_buf2 |= tmp_line_buf;            
                    tmp_line_buf = font_buf << 2;           
                    border_buf0 |= tmp_line_buf;            
                    border_buf1 |= tmp_line_buf;            
                    border_buf2 |= tmp_line_buf;  

                    //out put the border to dest layer
                    ori_output_mem = out_mem_ptr;   /* store current output mem position */ 
                    do
                    {
                        if (y == clip_height && not_draw_up_border) //the first line and not to draw up border
                        {
                            out_mem_ptr = ori_output_mem + readpixelnum * dst_byte_per_pixel;
                            break;
                        }
                        
                        /* move pos to previous pixel of previous line */ 
                        out_mem_ptr = ori_output_mem - ((GDI_LAYER.width + ((not_draw_left_border - 1) * (-1))) * dst_byte_per_pixel);
                        /* read border_buf to draw border */
                        OUTPUT_BORDER(border_buf0, output_border_start, output_border_end, 16, not_draw_left_border);
                                                                                     
                        if (y == 1) // if it's the final line
                        {                                                               
                            out_mem_ptr = ori_output_mem - ((not_draw_left_border - 1) * (-1)) * dst_byte_per_pixel; 
                            OUTPUT_BORDER(border_buf1, output_border_start, output_border_end, 16, not_draw_left_border);

                            if (not_draw_down_border)   // not draw the down border
                            {
                                out_mem_ptr = ori_output_mem + readpixelnum * dst_byte_per_pixel;     
                                break;
                            }
                            out_mem_ptr = ori_output_mem + ((GDI_LAYER.width - ((not_draw_left_border - 1) * (-1))) * dst_byte_per_pixel);    
                            OUTPUT_BORDER(border_buf2, output_border_start, output_border_end, 16, not_draw_left_border);
                        }                                          
                        out_mem_ptr = ori_output_mem + readpixelnum * dst_byte_per_pixel;               
                    }while(0);
                    
                    BIT_SEEK(abm_line_jump_bits);   // jump to the start pixel in the next line of the ABM image 
                    out_mem_ptr += dst_line_jump_bytes; // jump to the start pixel in the next line of the target layer 
                    y--;
                    
                    tmp_line_buf = border_buf0;     //shift the three border_buf
                    border_buf0 = border_buf1;
                    border_buf1 = border_buf2;
                    border_buf2 = tmp_line_buf;
                }
            }
            else
            {   //dst_bpp != 16 || clip_width > readpixelnum, we use the generic version
                while (y > 0)                                                                  
                {
                    //output a line 
                    for (x = (clip_width - 1); x >= 0; x -= readpixelnum)
                    {   
                        uint32_t mask, border_buf_len;
                        uint32_t tmp_line_buf;                       
                        uint8_t *ori_output_mem;     
                        uint8_t tmp_not_draw_left_border,tmp_not_draw_right_border;

                        tmp_not_draw_left_border = (x == (clip_width - 1))? not_draw_left_border : 0;
                        tmp_not_draw_right_border = (x <= 16)? not_draw_right_border : 0;
                        
                        border_buf0 = 0;
                        border_buf1 = 0;
                        border_buf2 = 0;
                        // read the pixel from bitstream 
                        readpixelnum = 16;
                        if (readpixelnum > x+1)
                        {
                            readpixelnum = x+1;
                        }
                        border_buf_len = readpixelnum + 2;
                        mask = (1 << readpixelnum) - 1;
                                
                        BIT_READ(readpixelnum, mask, font_buf); 
                                                                
                        border_buf0 |= (font_buf);              
                        border_buf1 |= (font_buf);              
                        border_buf2 |= (font_buf);              
                        tmp_line_buf = font_buf << 1;           
                        border_buf0 |= tmp_line_buf;            
                        border_buf2 |= tmp_line_buf;            
                        tmp_line_buf = font_buf << 2;           
                        border_buf0 |= tmp_line_buf;            
                        border_buf1 |= tmp_line_buf;            
                        border_buf2 |= tmp_line_buf;            

                        output_border_start = (border_buf_len - 1) - tmp_not_draw_left_border;
                        output_border_end = tmp_not_draw_right_border;

                        ori_output_mem = out_mem_ptr;   /* store current output mem position */ 

                        if (!(y == clip_height && not_draw_up_border))
                        {
                            /* move pos to previous pixel of previous line */
                            out_mem_ptr = ori_output_mem - ((GDI_LAYER.width + ((tmp_not_draw_left_border - 1) * (-1))) * dst_byte_per_pixel);     
                            /* read border_buf to draw border */    
                            OUTPUT_BORDER(border_buf0, output_border_start, output_border_end, dst_bpp, tmp_not_draw_left_border);
                        }
                        out_mem_ptr = ori_output_mem - ((tmp_not_draw_left_border - 1) * (-1)) * dst_byte_per_pixel; 
                        OUTPUT_BORDER(border_buf1, output_border_start, output_border_end, dst_bpp, tmp_not_draw_left_border);
                         
                        if (!(y == 1 && not_draw_down_border)){
                            out_mem_ptr = ori_output_mem + ((GDI_LAYER.width - ((tmp_not_draw_left_border - 1) * (-1))) * dst_byte_per_pixel);    
                            OUTPUT_BORDER(border_buf2, output_border_start, output_border_end, dst_bpp, tmp_not_draw_left_border);
                        }                                         
                        out_mem_ptr = ori_output_mem + readpixelnum * dst_byte_per_pixel;

                    }
                    BIT_SEEK(abm_line_jump_bits);   // jump to the start pixel in the next line of the ABM image 
                    out_mem_ptr += dst_line_jump_bytes; // jump to the start pixel in the next line of the target layer 
                    y--;
                }
            }
        }
        /* close bitstream reader */
        BIT_END();
    }    
}
/*****************************************************************************
 * FUNCTION
 *  gdi_show_char
 * DESCRIPTION
 *  Low-level font drawing
 * PARAMETERS
 *  x                   [IN]        X
 *  y                   [IN]        Y
 *  color               [IN]        Font color
 *  font_data           [IN]        Raw font data
 *  font_data_size      [IN]        Font data size in bytes
 *  char_width          [IN]        Character width
 *  char_height         [IN]        Character height
 *  font_attr           [IN]        Font attributes
 * RETURNS
 *  void
 *****************************************************************************/
gdi_result_t gdi_show_char(
            int32_t x,
            int32_t y,
            gdi_color_t color,
            uint8_t *font_data,
            uint32_t font_data_size,
            uint16_t char_width,
            uint16_t char_height,
            uint8_t font_attr,
            bool is_gray_bitmap)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t ret = GDI_SUCCEED;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Skip unnecessary action when the character is out of clip */
    GDI_CHECK_CHAR_CLIP(x, y, char_width, char_height, return GDI_SUCCEED);

    /* software font engine */
    if ((x - 1 < text_clip_x1) || (x + char_width + MAX_TILT + 1 > text_clip_x2) ||
             (y - 1 < text_clip_y1) || (y + char_height + 1 > text_clip_y2))
    {
        gdi_show_char_with_clip(x, y, color, font_data, font_data_size, char_width, char_height, font_attr);
    }
    else
    {
        gdi_show_char_no_clip(x, y, color, font_data, font_data_size, char_width, char_height, font_attr);
    }
    return ret;
}


extern const gd_put_pixel_func gd_put_pixel[];

gdi_result_t gdi_show_char_bordered(
            int32_t x,
            int32_t y,
            gdi_color_t text_color,
            gdi_color_t border_color,
            uint8_t *font_data,
            uint32_t font_data_size,
            uint16_t char_width,
            uint16_t char_height,
            uint8_t font_attr,
            bool is_gray_bitmap)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    gdi_result_t ret = GDI_SUCCEED;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Skip unnecessary action when the character is out of clip */
    GDI_CHECK_CHAR_CLIP(x, y, char_width, char_height,  return GDI_SUCCEED);
    /* Optimized software version for bordered font drawing */
    if (((font_attr == 0) || (font_attr & FONTATTRIB_NORMAL)) && (gdi_act_layer->bits_per_pixel >= 16)
	&&  ( text_clip_x1 <=x && x + char_width <=text_clip_x2 
		&&text_clip_y1 <=y && y + char_height<=text_clip_y2))
    {
        gdi_font_show_border_of_char(x, y, border_color, font_data, font_data_size, char_width, char_height, font_attr);
        gdi_show_char(x, y, text_color, font_data, font_data_size, char_width, char_height, font_attr, false);
    }
    else /* Original version (slower) */
    {
        gdi_show_char(x - 1, y - 1, border_color, font_data, font_data_size, char_width, char_height, font_attr, false);
        gdi_show_char(x - 1, y + 1, border_color, font_data, font_data_size, char_width, char_height, font_attr, false);
        gdi_show_char(x + 1, y - 1, border_color, font_data, font_data_size, char_width, char_height, font_attr, false);
        gdi_show_char(x + 1, y + 1, border_color, font_data, font_data_size, char_width, char_height, font_attr, false);
        gdi_show_char(x + 0, y - 1, border_color, font_data, font_data_size, char_width, char_height, font_attr, false);
        gdi_show_char(x + 0, y + 1, border_color, font_data, font_data_size, char_width, char_height, font_attr, false);
        gdi_show_char(x - 1, y + 0, border_color, font_data, font_data_size, char_width, char_height, font_attr, false);
        gdi_show_char(x + 1, y + 0, border_color, font_data, font_data_size, char_width, char_height, font_attr, false);

        gdi_show_char(x, y, text_color, font_data, font_data_size, char_width, char_height, font_attr, false);
    }
    return ret;
}
void gdi_font_begin(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    put_pixel = gd_put_pixel[GDI_LAYER.cf];

    text_clip_x1 = GDI_LAYER.clipx1;
    text_clip_y1 = GDI_LAYER.clipy1;
    text_clip_x2 = GDI_LAYER.clipx2;
    text_clip_y2 = GDI_LAYER.clipy2;
}

void gdi_font_end(void)
{

}
