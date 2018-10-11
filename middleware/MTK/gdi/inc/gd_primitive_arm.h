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
#include "gdi_primitive.h"

#ifndef __GD_PRIMITIVE_ARM__
#define __GD_PRIMITIVE_ARM__

extern int GDI_GRAY16SET(int add_end, int add);

/* fast sw resizer */
//extern GDI_RESULT gdi_bitblt_resized_sw(gdi_handle src_layer_handle, int32_t sx1, int32_t sy1, int32_t sx2, int32_t sy2, int32_t dx1, int32_t dy1, int32_t dx2, int32_t dy2, BOOL is_high_quality);


extern gdi_color_t gdi_img_buf_convert_color(
    gdi_color_t from_color, 
    gdi_img_buf_color_format_enum to_format,
    gdi_img_buf_color_format_enum from_format);

extern void gdi_img_buf_fill_byte(
    gdi_img_buf_struct *dst_buf, 
    uint8_t dst_byte,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_fill_color_16(
    gdi_img_buf_struct *dst_buf, 
    gdi_color_t dst_color,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_fill_color_24(
    gdi_img_buf_struct *dst_buf, 
    gdi_color_t dst_color,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_fill_color_32(
    gdi_img_buf_struct *dst_buf, 
    gdi_color_t dst_color,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_blending_color_16(
    gdi_img_buf_struct *dst_buf, 
    gdi_color_t pargb8888_color,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_blending_color_24(
    gdi_img_buf_struct *dst_buf, 
    gdi_color_t pargb8888_color,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_blending_color_PARGB8888(
    gdi_img_buf_struct *dst_buf, 
    gdi_color_t pargb8888_color,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_flatten_16_16(
    gdi_img_buf_struct *dst_buf, 
    gdi_color_t dst_color_key,
    const gdi_img_buf_struct *src_buf,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_flatten_general(
    gdi_img_buf_struct *dst_buf, 
    gdi_color_t dst_color_key,
    const gdi_img_buf_struct *src_buf,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_copy_same_format(
    gdi_img_buf_struct *dst_buf, 
    const gdi_img_buf_struct *src_buf,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_copy_general(
    gdi_img_buf_struct *dst_buf, 
    const gdi_img_buf_struct *src_buf,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_blending_16_PARGB8888(
    gdi_img_buf_struct *dst_buf, 
    const gdi_img_buf_struct *src_buf,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_blending_24_PARGB8888(
    gdi_img_buf_struct *dst_buf, 
    const gdi_img_buf_struct *src_buf,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_blending_PARGB8888_PARGB8888(
    gdi_img_buf_struct *dst_buf, 
    const gdi_img_buf_struct *src_buf,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_blending_PARGB8888_to_PARGB6666(
    gdi_img_buf_struct *dst_buf, 
    const gdi_img_buf_struct *src_buf,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_convert_PARGB8888_from_ARGB8888(
    gdi_img_buf_struct *buf, 
    int32_t width,
    int32_t height);

extern void gdi_img_buf_convert_ARGB8888_from_PARGB8888(
    gdi_img_buf_struct *buf, 
    int32_t width,
    int32_t height);

extern void gdi_img_buf_blending_PARGB6666_to_PARGB6666(
    gdi_img_buf_struct *dst_buf, 
    const gdi_img_buf_struct *src_buf,
    int32_t width,
    int32_t height);

extern void gdi_img_buf_blending_PARGB6666_to_RGB565(
    gdi_img_buf_struct *dst_buf, 
    const gdi_img_buf_struct *src_buf,
    int32_t width,
    int32_t height);

#endif /* __GD_PRIMITIVE_ARM__ */

