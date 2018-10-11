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

#ifndef _GDI_IMAGE_H_
#define _GDI_IMAGE_H_

#include "gdi_const.h"
#include "gdi_datatype.h"
#include "gdi.h"
#include "gdi_internal.h"

/****************************************************************************
* Define                                                              
*****************************************************************************/
#define GDI_IMAGE_CODEC_TIMEOUT  6 // seconds

/****************************************************************************
* Struct / Typedef                                                                 
*****************************************************************************/

typedef struct
{
    gdi_result_t(*draw) (int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data_ptr, uint32_t img_size);
    gdi_result_t(*get_dimension) (uint32_t flag, uint8_t *data_ptr, uint32_t img_size, uint32_t *width, uint32_t *height);
    gdi_result_t(*get_pos_info) (uint32_t flag, uint8_t *data_ptr, uint32_t img_size, bool quick_mode, int32_t *total_pos,
                               gdi_anim_pos_type_enum *pos_type);
    gdi_result_t(*is_match_file_type) (uint8_t *file_name);
} gdi_image_codecs_struct;

extern const gdi_image_codecs_struct * const gdi_image_codecs[];

extern bool gdi_memory_output;
extern uint16_t gdi_memory_output_width;
extern uint16_t gdi_memory_output_height;
extern uint32_t gdi_memory_output_buffer_address;
extern uint32_t gdi_memory_output_buffer_size;



/**
 * @brief This function draws image with specified dimension.
 * @param[in] src_type        is the source type of the image.
 * @param[in] img_type        is the type of the image.
 * @param[in] img_src         is the image data buffer.
 * @param[in] img_size        is the image size.
 * @param[in] x               is the x position to draw image.
 * @param[in] y               is the y position to draw image.
 * @param[in] w               is the image width to draw.
 * @param[in] h               is the image height to draw.
 * @return                    whtehter the operation is successful.
 */
extern gdi_result_t gdi_image_codec_draw(gdi_image_source_t src_type, const uint8_t *img_src,
                        gdi_image_type_t img_type, uint32_t img_size,
                        int32_t x, int32_t y, uint32_t w, uint32_t h);
/**
 * @brief This function draws image from resource with original image dimension.
 * @param[in] x               is the x position to draw image.
 * @param[in] y               is the y position to draw image.
 * @param[in] img_src         is the image data buffer.
 * @return                    whtehter the operation is successful.
 */
extern gdi_result_t gdi_image_draw(int32_t x, int32_t y, const uint8_t *img_src);
/**
 * @brief This function draws image from resource with specified dimension.
 * @param[in] x               is the x position to draw image.
 * @param[in] y               is the y position to draw image.
 * @param[in] w               is the image width to draw.
 * @param[in] h               is the image height to draw.
 * @param[in] img_src         is the image data buffer.
 * @return                    whtehter the operation is successful.
 */
extern gdi_result_t gdi_image_draw_resized(int32_t x, int32_t y, uint32_t w, uint32_t h, const uint8_t *img_src);
/**
 * @brief This function draws image in memory with original image dimension.
 * @param[in] x             is the x position to draw image.
 * @param[in] y             is the y position to draw image.
 * @param[in] img_type      is the image type.
 * @param[in] img_src       is the image data pointer.
 * @param[in] img_size      is the image size.
 * @return                  whtehter the operation is successful.
 */
extern gdi_result_t gdi_image_draw_mem(int32_t x, int32_t y, gdi_image_type_t img_type, const uint8_t *img_src, int32_t img_size);
/**
 * @brief This function draws image in memory with specified dimension.
 * @param[in] OFFSET_X        is the x position to draw image.
 * @param[in] OFFSET_Y        is the y position to draw image.
 * @param[in] RESIZED_WIDTH   is the image width to draw.
 * @param[in] RESIZED_HEIGHT  is the image height to draw.
 * @param[in] IMAGE_PTR       is the image data pointer.
 * @param[in] IMAGE_TYPE      is the image type.
 * @param[in] IMAGE_LENGTH    is the image size.
 * @return                    whtehter the operation is successful.
 */
#define gdi_image_draw_resized_mem(OFFSET_X,OFFSET_Y,RESIZED_WIDTH,RESIZED_HEIGHT,IMAGE_TYPE,IMAGE_PTR,IMAGE_LENGTH) \
                    gdi_image_codec_draw(                       \
                                GDI_IMAGE_SRC_FROM_MEMORY,      \
                                IMAGE_TYPE,                     \
                                IMAGE_PTR,                      \
                                IMAGE_LENGTH,                   \
                                OFFSET_X,                       \
                                OFFSET_Y,                       \
                                RESIZED_WIDTH,                  \
                                RESIZED_HEIGHT)
/**
 * @brief This function gets the image dimension of the given image data buffer.
 * @param[in] image_ptr    is the image data buffer.
 * @param[out] width       is the image width.
 * @param[out] height      is the image height.
 * @return                 whtehter the operation is successful.
 */
extern gdi_result_t gdi_image_get_dimension(const uint8_t *image_ptr, uint32_t *width, uint32_t *height);

/**
 * @brief This function gets the image dimension of the given image data.
 * @param[in] img_type     is the image type.
 * @param[in] img_ptr      is the image data buffer.
 * @param[in] img_size     is the image size.
 * @param[out] width       is the image width.
 * @param[out] height      is the image height.
 * @return                 whtehter the operation is successful.
 */
extern gdi_result_t gdi_image_get_dimension_mem(gdi_image_type_t img_type, const uint8_t *img_ptr, uint32_t img_size, uint32_t *width, uint32_t *height);
/**
 * @brief This function gets image type by image data buffer.
 * @param[in] ptr     is the image data buffer.
 * @return            the image type.
 */
extern gdi_image_type_t gdi_image_get_type_from_mem(const uint8_t *ptr);
/**
 * @brief This function gets image size by image data buffer.
 * @param[in] ptr     is the image data buffer.
 * @return            the size of the image data.
 */
extern uint32_t gdi_image_get_size(const uint8_t *ptr);
/**
 * @brief This function gets image size by image ID.
 * @param[in] ID      is the image resource id.
 * @return            the size of the image data.
 */
#define gdi_image_get_size_from_id(ID) gdi_image_get_buf_len(gdi_resource_get_image_data(ID))
/**
 * @brief This function gets image type by image ID.
 * @param[in] ID      is the image resource id.
 * @return            the image type.
 */
extern gdi_result_t gdi_image_get_info_from_res_data(
    const uint8_t *res_data,
    gdi_image_type_t *image_type,
    uint32_t *width,
    uint32_t *height,
    bool *with_alpha);
/****************************************************************************
* Function Declar                                                              
*****************************************************************************/
/* init function */
extern void gdi_image_parse_resource_internal(const uint8_t *res_src, uint8_t **data_ptr, uint8_t *img_type, uint32_t *img_size, uint32_t *frame_number, uint32_t *width, uint32_t *height);
extern gdi_result_t gdi_image_parse_info(
                    gdi_image_source_t src_type,
                    const uint8_t *img_src,
                    uint8_t **data_ptr,
                    uint8_t *img_type,
                    uint32_t *img_size,
                    uint32_t *frame_number,
                    uint32_t *width,
                    uint32_t *height);

extern gdi_result_t gdi_image_codec_get_dimension(
                    gdi_image_source_t src_type,
                    const uint8_t *img_src,
                    uint8_t img_type,
                    uint32_t img_size,
                    uint32_t *width,
                    uint32_t *height,
                    uint32_t flag);

/* 
 * Image Helper functions 
 */
extern gdi_result_t gdi_image_create_temp_layer(
    gdi_layer_handle_t *temp_layer_handle,
    gdi_color_format_t cf,
    int32_t x,
    int32_t y,
    int32_t width,
    int32_t height);
    
extern void gdi_image_free_temp_layer(gdi_layer_handle_t temp_layer_handle);

#define gdi_image_resize_and_blending(layer_handle, dst_x, dst_y, resized_width, resized_height) \
    gdi_image_resize_and_blending_ex(layer_handle, dst_x, dst_y, resized_width, resized_height, false);

extern gdi_result_t gdi_image_resize_and_blending_ex(
    gdi_layer_handle_t layer_handle, 
    int32_t dst_x, 
    int32_t dst_y, 
    int32_t resized_width, 
    int32_t resized_height,
    bool always_blending);

extern gdi_result_t gdi_image_bitlit_PARGB8888(gdi_layer_handle_t img_layer);

extern gdi_result_t gdi_image_blending_32_to_16(
    uint8_t *img_ptr,
    uint32_t img_pitch_bytes,
    uint8_t *active_ptr,
    uint32_t active_pitch_bytes,
    uint8_t *src_ptr,
    uint32_t src_pitch_bytes,
    int32_t width,
    int32_t height);

extern gdi_result_t gdi_image_blending_32_to_24(
    uint8_t *img_ptr,
    uint32_t img_pitch_bytes,
    uint8_t *active_ptr,
    uint32_t active_pitch_bytes,
    uint8_t *src_ptr,
    uint32_t src_pitch_bytes,
    int32_t width,
    int32_t height);

extern gdi_result_t gdi_image_blending_32_to_32(
    uint8_t *img_ptr,
    uint32_t img_pitch_bytes,
    uint8_t *active_ptr,
    uint32_t active_pitch_bytes,
    uint8_t *src_ptr,
    uint32_t src_pitch_bytes,
    int32_t width,
    int32_t height);

extern gdi_result_t gdi_image_blending_PARGB8888_to_PARGB6666(
    uint8_t *img_ptr,
    uint32_t img_pitch_bytes,
    uint8_t *active_ptr,
    uint32_t active_pitch_bytes,
    uint8_t *src_ptr,
    uint32_t src_pitch_bytes,
    int32_t width,
    int32_t height);

#endif /* _GDI_IMAGE_H_ */ 


