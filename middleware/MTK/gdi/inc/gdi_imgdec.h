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

#ifndef __GDI_IMGDEC_H__
#define __GDI_IMGDEC_H__

/***************************************************************************** 
 * Include
 *****************************************************************************/
#include "gdi_const.h"
#include "gdi_datatype.h"
#include "gdi.h"

/***************************************************************************** 
 * Define
 *****************************************************************************/
/*
 * IMGDEC APIs
 */
typedef void (*gdi_imgdec_nb_done_callback_funcptr)(gdi_result_t result, gdi_layer_handle_t handle);

#define gdi_imgdec_draw_resized_frames_internal(output_layer, x, y, w, h, img_ptr, img_type, img_size, src_type, frame_index) \
            gdi_imgdec_draw_internal( \
                output_layer, \
                GDI_NULL_HANDLE, \
                src_type, \
                img_ptr, \
                img_type, \
                img_size, \
                x, \
                y, \
                w, \
                h, \
                frame_index, \
                NULL, \
                0, \
                0)

#define gdi_imgdec_draw(output_layer, x, y, img_ptr) gdi_imgdec_draw_resized_frames_internal(output_layer, x, y, 0, 0, img_ptr, 0, 0, GDI_IMAGE_SRC_FROM_RESOURCE, 0)
#define gdi_imgdec_draw_resized(output_layer, x, y, w, h, img_ptr) gdi_imgdec_draw_resized_frames_internal(output_layer, x, y, w, h, img_ptr, 0, 0, GDI_IMAGE_SRC_FROM_RESOURCE, 0)
#define gdi_imgdec_draw_mem(output_layer, x, y, img_ptr, img_type, img_size) gdi_imgdec_draw_resized_frames_internal(output_layer, x, y, 0, 0, img_ptr, img_type, img_size, GDI_IMAGE_SRC_FROM_MEMORY, 0)
#define gdi_imgdec_draw_resized_mem(output_layer, x, y, w, h, img_ptr, img_type, img_size) gdi_imgdec_draw_resized_frames_internal(output_layer, x, y, w, h, img_ptr, img_type, img_size, GDI_IMAGE_SRC_FROM_MEMORY, 0)
#define gdi_imgdec_draw_file(output_layer, x, y, filename_ptr) gdi_imgdec_draw_resized_frames_internal(output_layer, x, y, 0, 0, filename_ptr, 0, 0, GDI_IMAGE_SRC_FROM_FILE, 0)
#define gdi_imgdec_draw_resized_file(output_layer, x, y, w, h, filename_ptr) gdi_imgdec_draw_resized_frames_internal(output_layer, x, y, w, h, filename_ptr, 0, 0, GDI_IMAGE_SRC_FROM_FILE, 0)

#define gdi_imgdec_draw_frames(output_layer, x, y, img_ptr, frame_index) gdi_imgdec_draw_resized_frames_internal(output_layer, x, y, 0, 0, img_ptr, 0, 0, GDI_IMAGE_SRC_FROM_RESOURCE, frame_index)
#define gdi_imgdec_draw_resized_frames(output_layer, x, y, w, h, img_ptr, frame_index) gdi_imgdec_draw_resized_frames_internal(output_layer, x, y, w, h, img_ptr, 0, 0, GDI_IMAGE_SRC_FROM_RESOURCE, frame_index)
#define gdi_imgdec_draw_mem_frames(output_layer, x, y, img_ptr, img_type, img_size, frame_index) gdi_imgdec_draw_resized_frames_internal(output_layer, x, y, 0, 0, img_ptr, img_type, img_size, GDI_IMAGE_SRC_FROM_MEMORY, frame_index)
#define gdi_imgdec_draw_resized_mem_frames(output_layer, x, y, w, h, img_ptr, img_type, img_size, frame_index) gdi_imgdec_draw_resized_frames_internal(output_layer, x, y, w, h, img_ptr, img_type, img_size, GDI_IMAGE_SRC_FROM_MEMORY, frame_index)
#define gdi_imgdec_draw_file_frames(output_layer, x, y, filename_ptr, frame_index) gdi_imgdec_draw_resized_frames_internal(output_layer, x, y, 0, 0, filename_ptr, 0, 0, GDI_IMAGE_SRC_FROM_FILE, frame_index)
#define gdi_imgdec_draw_resized_file_frames(output_layer, x, y, w, h, filename_ptr, frame_index) gdi_imgdec_draw_resized_frames_internal(output_layer, x, y, w, h, filename_ptr, 0, 0, GDI_IMAGE_SRC_FROM_FILE, frame_index)

/***************************************************************************** 
 * Typedef
 *****************************************************************************/

/***************************************************************************** 
 * Global Function
 *****************************************************************************/
extern bool g_imgdec_timer_timeout_flag;
extern gdi_result_t gdi_imgdec_draw_internal(
            gdi_layer_handle_t output_layer,
            gdi_layer_handle_t source_layer,
            gdi_image_source_t src_type,
            uint8_t *img_src,
            uint8_t img_type,
            int32_t img_size,
            int32_t x,
            int32_t y,
            uint32_t resize_width,
            uint32_t resize_height,
            uint32_t frame_index,
            bool *is_aborted,
            uint32_t flag,
            uint32_t aspect_flag);

#endif /* __GDI_IMGDEC_H__ */


