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

#ifndef _GDI_IMGDEC_BMP_H_
#define _GDI_IMGDEC_BMP_H_

#include "gdi_imgdec.h"
#include "gdi_imgdec_internal.h"
#include "gdi_imgdec_bytestream.h"
#include "gdi_imgdec_resizer.h"

typedef enum
{
    GDI_IMGDEC_BMP_DITHER_DISABLE,
    GDI_IMGDEC_BMP_DITHER_RGB565,
    GDI_IMGDEC_BMP_DITHER_RGB8882BITS,
    GDI_IMGDEC_BMP_DITHER_END_OF_ENUM
} gdi_imgdec_bmp_dithering_enum;

typedef struct
{
    GDI_IMGDEC_BASE_STRUCT

    gdi_imgdec_resizer_struct resizer;
    
    gdi_imgdec_bytestream_struct bs;
    uint8_t *bs_buf_ptr;
    uint32_t bs_buf_size;

    gdi_color_t *palette;
    uint32_t palette_size;
    uint8_t compression;
    int32_t width;
    int32_t height;
    int32_t src_x;
    int32_t src_y;
    int32_t run_length;
    int32_t run_length_used;
    int32_t run_length_odd;
    uint8_t code;
    bool is_raw_mode;
    bool is_from_file;
    gdi_imgdec_bmp_dithering_enum dither_mode;    
} gdi_imgdec_bmp_struct;


/****************************************************************************
* Function                                                               
*****************************************************************************/
extern void gdi_imgdec_bmp_init(gdi_imgdec_bmp_struct *dec, gdi_layer_handle_t out_layer, void *bs_buf, uint32_t bs_buf_size, bool *is_aborted, uint32_t flag);
extern void gdi_imgdec_bmp_deinit(gdi_imgdec_bmp_struct *dec);
extern gdi_result_t gdi_imgdec_bmp_draw_resized(
            gdi_imgdec_bmp_struct *dec,
            int32_t offset_x,
            int32_t offset_y,
            bool is_resized,
            int32_t resized_width,
            int32_t resized_height,
            uint8_t *BMP_src,
            uint32_t size,
            uint32_t aspect_flag);
extern gdi_result_t gdi_imgdec_bmp_get_dimension(uint8_t *src, uint32_t size, uint32_t *width, uint32_t *height);
extern gdi_result_t gdi_imgdec_bmp_draw_handler(gdi_layer_handle_t out_layer, int32_t x, int32_t y, uint32_t w, uint32_t h, uint8_t *data_ptr, uint32_t img_size, bool *is_aborted, int32_t aspect_flag);

#endif /* _GDI_IMGDEC_BMP_H_ */ 


