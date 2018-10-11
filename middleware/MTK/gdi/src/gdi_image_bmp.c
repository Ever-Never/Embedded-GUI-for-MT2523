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
#include "wchar.h"
#include "setjmp.h"
#include "string.h"
#include "gdi_bytestream.h"
#include "gdi_const.h"
#include "gdi_datatype.h"
#include "gdi.h"
#include "gdi_image.h"
#include "gdi_primitive.h"
#include "gdi_image_bmp.h"
#include "gd_include.h"
#include "gdi_imgdec_bmp.h"
#include "gdi_resource_internal.h"

/****************************************************************************
* Define
*****************************************************************************/
#define GDI_BI_RGB  0
#define GDI_BI_RLE8  1
#define GDI_BI_RLE4  2
#define GDI_BI_BITFIELDS  3
#define GDI_BMP_BYTESTREM_BUF_SIZE 4096

/****************************************************************************
* Static Declaration
*****************************************************************************/
static uint8_t   gdi_image_bmp_palette_size;      /* size of palette */

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
uint8_t gdi_image_bmp_get_palette_size(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_image_bmp_get_palette_size)
    ret = gdi_image_bmp_palette_size;
    GDI_EXIT_CRITICAL_SECTION(gdi_image_bmp_get_palette_size)

    return ret;
}


/****************************************************************************
* Cache Bitmap
*****************************************************************************/

/*****************************************************************************
 * FUNCTION
 *  gdi_image_cache_bmp_get
 * DESCRIPTION
 *  cache a region from a layer
 * PARAMETERS
 *  x1          [IN]    left-up cornor x of destination region
 *  y1          [IN]    left-up cornor y of destination region
 *  x2          [IN]    right-down cornor x of destination region
 *  y2          [IN]    right-down cornor y of destination region
 *  bmp_ptr     [OUT]   return width, height, and pointer to cached region
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
gdi_result_t gdi_image_cache_bmp_get(int32_t x1, int32_t y1, int32_t x2, int32_t y2, gdi_image_cache_bmp_struct *bmp_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t i;
    uint32_t layer_width;
    uint32_t layer_height;
    int32_t layer_clip_x1;
    int32_t layer_clip_x2;
    int32_t layer_clip_y1;
    int32_t layer_clip_y2;
    int32_t start_x, end_x, start_y, end_y;
    uint8_t *layer_buf_ptr;
    uint8_t *bmp_data_ptr;
    int32_t byte_per_row;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_image_cache_bmp_get)

    /* get src and dest buffer ptr */
    bmp_data_ptr = bmp_ptr->buf_ptr;            /* dest */
    gdi_layer_get_buffer_ptr(&layer_buf_ptr);   /* src */

    gdi_layer_get_clip(&layer_clip_x1, &layer_clip_y1, &layer_clip_x2, &layer_clip_y2);
    gdi_layer_get_dimension(&layer_width, &layer_height);

    /* out of clip region */
    if ((x1 > layer_clip_x2) || (y1 > layer_clip_y2) || (x2 < layer_clip_x1) || (y2 < layer_clip_y1))
    {
        bmp_ptr->width = 0;
        bmp_ptr->height = 0;
        GDI_RETURN(GDI_SUCCEED);
    }

    /* Calculate valid region */
    if (x1 < layer_clip_x1)
    {
        start_x = layer_clip_x1;
    }
    else
    {
        start_x = x1;
    }

    if (x2 > layer_clip_x2)
    {
        end_x = layer_clip_x2;
    }
    else
    {
        end_x = x2;
    }

    if (y1 < layer_clip_y1)
    {
        start_y = layer_clip_y1;
    }
    else
    {
        start_y = y1;
    }

    if (y2 > layer_clip_y2)
    {
        end_y = layer_clip_y2;
    }
    else
    {
        end_y = y2;
    }

    /* fill bitmap info */
    bmp_ptr->width = end_x - start_x + 1;
    bmp_ptr->height = end_y - start_y + 1;

    byte_per_row = (bmp_ptr->width * GDI_LAYER.bits_per_pixel) >> 3;

    /* find source buffer start position of the active layer */
    layer_buf_ptr += ((start_y * layer_width + start_x) * GDI_LAYER.bits_per_pixel) >> 3;       /* 16-bit */

    /* fill bitmap buffer */
    for (i = 0; i < bmp_ptr->height; i++)
    {
        memcpy(bmp_data_ptr, layer_buf_ptr, byte_per_row);

        bmp_data_ptr += byte_per_row;
        layer_buf_ptr += (layer_width * GDI_LAYER.bits_per_pixel) >> 3; /* just to next row */
    }

    GDI_EXIT_CRITICAL_SECTION(gdi_image_cache_bmp_get)
    return GDI_SUCCEED;
}


/*****************************************************************************
 * FUNCTION
 *  gdi_image_cache_bmp_draw
 * DESCRIPTION
 *  draw cached image to a layer
 * PARAMETERS
 *  offset_x        [IN]    position of layer want to draw to
 *  offset_y        [IN]    position of layer want to draw to 
 *  bmp_ptr         [IN]    the bmp want to draw
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
gdi_result_t gdi_image_cache_bmp_draw(int32_t offset_x, int32_t offset_y, gdi_image_cache_bmp_struct *bmp_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t i;
    uint32_t layer_width;
    uint32_t layer_height;
    int32_t layer_clip_x1;
    int32_t layer_clip_x2;
    int32_t layer_clip_y1;
    int32_t layer_clip_y2;
    int32_t start_x, end_x, start_y, end_y;
    uint8_t *layer_buf_ptr;
    uint8_t *bmp_data_ptr;
    int32_t x1, y1, x2, y2;
    int32_t vlaid_width;
    int32_t byte_per_row;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_image_cache_bmp_draw)

    /* get src and dest buffer ptr */
    bmp_data_ptr = bmp_ptr->buf_ptr;            /* src */
    gdi_layer_get_buffer_ptr(&layer_buf_ptr);   /* dest */

    byte_per_row = (bmp_ptr->width * GDI_LAYER.bits_per_pixel) >> 3;

    gdi_layer_get_clip(&layer_clip_x1, &layer_clip_y1, &layer_clip_x2, &layer_clip_y2);
    gdi_layer_get_dimension(&layer_width, &layer_height);

    x1 = offset_x;
    y1 = offset_y;

    x2 = x1 + bmp_ptr->width - 1;
    y2 = y1 + bmp_ptr->height - 1;

    /* out of clip region */
    if ((x1 > layer_clip_x2) || (y1 > layer_clip_y2) || (x2 < layer_clip_x1) || (y2 < layer_clip_y1))
    {
        GDI_RETURN(GDI_SUCCEED);
    }

    /* Calculate valid region */
    if (x1 < layer_clip_x1)
    {
        start_x = layer_clip_x1;
    }
    else
    {
        start_x = x1;
    }

    if (x2 > layer_clip_x2)
    {
        end_x = layer_clip_x2;
    }
    else
    {
        end_x = x2;
    }

    if (y1 < layer_clip_y1)
    {
        start_y = layer_clip_y1;
    }
    else
    {
        start_y = y1;
    }

    if (y2 > layer_clip_y2)
    {
        end_y = layer_clip_y2;
    }
    else
    {
        end_y = y2;
    }

    /* find update width */
    vlaid_width = end_x - start_x + 1;

    /* shift to bmp buffer start position */
    bmp_data_ptr += ((start_y - y1) * byte_per_row) + ((start_x - x1) * (GDI_LAYER.bits_per_pixel >> 3));

    /* shift to layer buffer start position */
    layer_buf_ptr += ((start_y * layer_width + start_x) * GDI_LAYER.bits_per_pixel) >> 3;

    /* fill bitmap buffer */
    for (i = start_y; i <= end_y; i++)
    {
        memcpy(layer_buf_ptr, bmp_data_ptr, (vlaid_width * GDI_LAYER.bits_per_pixel) >> 3);
        bmp_data_ptr += byte_per_row;
        layer_buf_ptr += (layer_width * GDI_LAYER.bits_per_pixel) >> 3;
    }

    GDI_EXIT_CRITICAL_SECTION(gdi_image_cache_bmp_draw)
    return GDI_SUCCEED;
}


/*****************************************************************************
 * FUNCTION
 *  gdi_image_bmp_draw_handler
 * DESCRIPTION
 *  draw bmp
 * PARAMETERS
 *  flag            [IN]        flag
 *  frame_pos       [IN]        which frame to draw
 *  x               [IN]        draw position x
 *  y               [IN]        draw position y
 *  w               [IN]        draw width
 *  h               [IN]        draw height
 *  data_ptr        [IN]        image pointer
 *  img_size        [IN]        image fize
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
gdi_result_t gdi_image_bmp_draw_handler(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data_ptr, uint32_t img_size)
{
    gdi_result_t ret;
    bool aborted = false;
    
    ret = gdi_imgdec_bmp_draw_handler((gdi_layer_handle_t)gdi_act_layer, x, y, w, h, data_ptr, img_size, &aborted, 0);
    return ret;
}


/*****************************************************************************
 * FUNCTION
 *  gdi_image_bmp_get_dimension_handler
 * DESCRIPTION
 *  get dimension of the bmp image
 * PARAMETERS
 *  flag           [IN]     flag
 *  data_ptr       [IN]     image pointer
 *  img_size       [IN]     image fize
 *  width          [OUT]    image width
 *  height         [OUT]    image height
 * RETURNS
 *  gdi_result_t, return GDI_SUCCEED if succeed
 *****************************************************************************/
gdi_result_t gdi_image_bmp_get_dimension_handler(uint32_t flag, uint8_t *data_ptr, uint32_t img_size, uint32_t *width, uint32_t *height)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (flag & GDI_IMAGE_CODEC_FLAG_IS_FILE)
    {
        return gdi_imgdec_bmp_get_dimension(data_ptr,0,width,height);
    }
    else
    {
        return gdi_imgdec_bmp_get_dimension(data_ptr,img_size,width,height);
    }
}

typedef struct {
    uint16_t signature;              /* 19778 */
    uint32_t size;
    uint16_t reserved1;              /* 0 */
    uint16_t reserved2;              /* 0 */
    uint32_t image_offset;
    uint32_t size_of_bmpinfoheader;  /* 40 */
    uint32_t width;
    uint32_t height;
    uint16_t planes;                 /* 1 */
    uint16_t bpp;                    /* 1, 4, 8, 24 */
    uint32_t compression;            /* 0 = none, 1 = RLE - 8, 2 = RLE - 4 */
    uint32_t size_of_image;
    uint32_t h_resolution;
    uint32_t v_resolution;
    uint32_t number_of_colors;
    uint32_t number_of_major_colors;
} BITMAP_HEADER;

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
gdi_result_t gdi_image_bmp_encode_get_size(
                int32_t width, 
                int32_t height, 
                uint8_t  depth,
                int32_t *bmp_size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t bytePerLine;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    GDI_ENTER_CRITICAL_SECTION(gdi_image_bmp_encode_get_size);
    if (depth == 2)
    {
        bytePerLine = 2 * (width + width % 2);
    }
    else if (depth == 3)
    {
        bytePerLine = 4 * ((width * 24 + 31) / 32);
    }
    else
    {
        *bmp_size = 0;
        GDI_RETURN(GDI_FAILED);
    }

    *bmp_size = bytePerLine * height + 54;
    
    GDI_EXIT_CRITICAL_SECTION(gdi_image_bmp_encode_get_size)
    return GDI_SUCCEED;
}
