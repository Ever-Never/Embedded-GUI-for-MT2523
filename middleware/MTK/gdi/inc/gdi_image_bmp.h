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

#ifndef _GDI_IMAGE_BMP_H_
#define _GDI_IMAGE_BMP_H_

#include "gdi_datatype.h"
#include "gdi_const.h"


/****************************************************************************
* Function                                                               
*****************************************************************************/

extern gdi_result_t gdi_image_bmp_draw_handler(
                    int32_t x,
                    int32_t y,
                    int32_t w,
                    int32_t h,
                    uint8_t *data_ptr,
                    uint32_t img_size);
extern gdi_result_t gdi_image_bmp_get_dimension_handler(uint32_t flag, uint8_t *data_ptr, uint32_t img_size, uint32_t *width, uint32_t *height);

#define gdi_image_bmp_draw( offset_x,  offset_y, BMP_src,  size)  \
    gdi_image_bmp_draw_handler(0, 0, offset_x, offset_y, 0, 0, BMP_src, size)

#define gdi_image_bmp_draw_file( offset_x,  offset_y, filename) \
    gdi_image_bmp_draw_handler(GDI_IMAGE_CODEC_FLAG_IS_FILE, 0, offset_x, offset_y, 0, 0, filename,  0)

#define gdi_image_bmp_draw_resized( offset_x, offset_y, resized_width, resized_height, BMP_src, size) \
    gdi_image_bmp_draw_handler(0, 0, offset_x, offset_y, resized_width, resized_height, BMP_src,  size)

#define gdi_image_bmp_draw_resized_file( offset_x, offset_y, resized_width, resized_height, filename) \
    gdi_image_bmp_draw_handler(GDI_IMAGE_CODEC_FLAG_IS_FILE, 0, offset_x, offset_y, resized_width, resized_height, filename,  0)

#define gdi_image_bmp_get_dimension(BMP_src, width, height) \
    gdi_image_bmp_get_dimension_handler(0, BMP_src, 1, width, height)
#define gdi_image_bmp_get_dimension_file(filename, width, height) \
    gdi_image_bmp_get_dimension_handler(GDI_IMAGE_CODEC_FLAG_IS_FILE, filename, 0, width, height)


#define GDI_BMP_EXCEPTION_ESCAPE_MODE_END_OF_BITMAP     100
#endif /* _GDI_IMAGE_BMP_H_ */ 


