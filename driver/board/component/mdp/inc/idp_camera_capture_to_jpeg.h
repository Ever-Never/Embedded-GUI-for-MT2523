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

#ifndef __idp_camera_capture_to_jpeg_h__
#define __idp_camera_capture_to_jpeg_h__

#include <idp_core.h>
#include "idp_define.h"
#include "kal_general_types.h"

enum
{
  IDP_CAMERA_CAPTURE_TO_JPEG_EXTMEM_START_ADDRESS = 1,
  IDP_CAMERA_CAPTURE_TO_JPEG_EXTMEM_SIZE,

  IDP_CAMERA_CAPTURE_TO_JPEG_IMAGE_SRC_WIDTH,
  IDP_CAMERA_CAPTURE_TO_JPEG_IMAGE_SRC_HEIGHT,

  //fullsize image refers to the largest size image for still image encoding. There is no switch for this image, as it could be the only image in the low cost platforms(6235/6253).
  IDP_CAMERA_CAPTURE_TO_JPEG_FULLSIZE_IMAGE_ROT_ANGLE,
  IDP_CAMERA_CAPTURE_TO_JPEG_FULLSIZE_IMAGE_FORMAT,
  IDP_CAMERA_CAPTURE_TO_JPEG_FULLSIZE_IMAGE_CAPTURE_WIDTH,
  IDP_CAMERA_CAPTURE_TO_JPEG_FULLSIZE_IMAGE_CAPTURE_HEIGHT,
  IDP_CAMERA_CAPTURE_TO_JPEG_FULLSIZE_JPEG_YUV_MODE, // JPEG gray mode
  IDP_CAMERA_CAPTURE_TO_JPEG_FULLSIZE_IMAGE_YUV_ADDRESSES,  // three arguments, if RGB565, only first one is valid

  //quickview image refers to the display size image. For low cost platforms, it may be disabled.
  IDP_CAMERA_CAPTURE_TO_JPEG_ENABLE_QUICKVIEW, // to replace IDP_CAMERA_CAPTURE_TO_JPEG_OUTPUT_RGB,
  IDP_CAMERA_CAPTURE_TO_JPEG_QUICKVIEW_ROT_ANGLE,
  IDP_CAMERA_CAPTURE_TO_JPEG_QUICKVIEW_IMAGE_COLOR_FORMAT,
  IDP_CAMERA_CAPTURE_TO_JPEG_QUICKVIEW_WIDTH,
  IDP_CAMERA_CAPTURE_TO_JPEG_QUICKVIEW_HEIGHT,
  IDP_CAMERA_CAPTURE_TO_JPEG_QUICKVIEW_FRAME_BUFFER_ADDRESS,
  IDP_CAMERA_CAPTURE_TO_JPEG_ENABLE_QUICKVIEW_PITCH, // to replace IDP_CAMERA_CAPTURE_TO_JPEG_ENABLE_PITCH
  IDP_CAMERA_CAPTURE_TO_JPEG_QUICKVIEW_PITCH_WIDTH, // to replace IDP_CAMERA_CAPTURE_TO_JPEG_BG_IMAGE_WIDTH,

  //thumbnail image is the smallest image. For low cost platforms, it may be disabled.
  IDP_CAMERA_CAPTURE_TO_JPEG_ENABLE_THUMBNAIL,
  IDP_CAMERA_CAPTURE_TO_JPEG_THUMBNAIL_ROT_ANGLE,
  IDP_CAMERA_CAPTURE_TO_JPEG_THUMBNAIL_IMAGE_COLOR_FORMAT,
  IDP_CAMERA_CAPTURE_TO_JPEG_THUMBNAIL_WIDTH,
  IDP_CAMERA_CAPTURE_TO_JPEG_THUMBNAIL_HEIGHT,
  IDP_CAMERA_CAPTURE_TO_JPEG_THUMBNAIL_BUFFER_ADDRESS,

  IDP_CAMERA_CAPTURE_TO_JPEG_ENABLE_OVERLAY,
  IDP_CAMERA_CAPTURE_TO_JPEG_SET_OVERLAY_PALETTE_SIZE_AND_ADDR,
  IDP_CAMERA_CAPTURE_TO_JPEG_OVERLAY_PALETTE_MODE,
  IDP_CAMERA_CAPTURE_TO_JPEG_OVERLAY_FRAME_SOURCE_KEY,
  IDP_CAMERA_CAPTURE_TO_JPEG_OVERLAY_FRAME_WIDTH,
  IDP_CAMERA_CAPTURE_TO_JPEG_OVERLAY_FRAME_HEIGHT,
  IDP_CAMERA_CAPTURE_TO_JPEG_OVERLAY_FRAME_BUFFER_ADDRESS,
  
  IDP_CAMERA_CAPTURE_TO_JPEG_CROP_ENABLE,
  IDP_CAMERA_CAPTURE_TO_JPEG_CROP_WIDTH,
  IDP_CAMERA_CAPTURE_TO_JPEG_CROP_HEIGHT,

  IDP_CAMERA_CAPTURE_TO_JPEG_LAST_MANDATORY_USER_CONFIG,
  // ***********************Mandatory parameter definition end****************************
  // The following region defines the optional parameters which will not be checked by idp_finish_read_user_config()

  // currently unused interfaces
  IDP_CAMERA_CAPTURE_TO_JPEG_INTMEM_START_ADDRESS,
  IDP_CAMERA_CAPTURE_TO_JPEG_INTMEM_SIZE,
  IDP_CAMERA_CAPTURE_TO_JPEG_RESIZER_U_VALUE, // should it from CAL or from custom file directly?
  IDP_CAMERA_CAPTURE_TO_JPEG_RESIZER_D_VALUE,

#if ENABLE_UT
  IDP_CAMERA_CAPTURE_TO_JPEG_IMAGE_BUFFER_ADDRESS,
#endif

  //SOP of adding new parameter for this scenario
  //1. Add a new definition here
  //2. Add a corresponding variable in the config struct
  //3. Make sure adding the matching handler for this definition in the "read_user_config_hook" function
  //   In the switch case, default case would cause "ASSERT(0)"!!!!!
  //4. Make sure SET_BIT to variable of "read_user_config" in the "read_user_config_hook" function
  //5. If no use in some platforms, please make sure adding the SET_BIT in the init_config_hook function
  IDP_CAMERA_CAPTURE_TO_JPEG_LAST_USER_CONFIG

#if 0
  //deprecated ones
  IDP_CAMERA_CAPTURE_TO_JPEG_SNAPSHOT_NUMBER,
  IDP_CAMERA_CAPTURE_TO_JPEG_ROT_ANGLE,
  IDP_CAMERA_CAPTURE_TO_JPEG_OUTPUT_RGB,
  IDP_CAMERA_CAPTURE_TO_JPEG_TRIGGER_LCD, // change to quickview trigger lcd
  IDP_CAMERA_CAPTURE_TO_JPEG_ENABLE_PITCH,
  IDP_CAMERA_CAPTURE_TO_JPEG_BG_IMAGE_WIDTH,
  IDP_CAMERA_CAPTURE_TO_JPEG_IMAGE_DATA_FORMAT,
  IDP_CAMERA_CAPTURE_TO_JPEG_IMAGE_CAPTURE_WIDTH,
  IDP_CAMERA_CAPTURE_TO_JPEG_IMAGE_CAPTURE_HEIGHT,
  IDP_CAMERA_CAPTURE_TO_JPEG_JPEG_YUV_MODE,
  IDP_CAMERA_CAPTURE_TO_JPEG_DISPLAY_WIDTH,
  IDP_CAMERA_CAPTURE_TO_JPEG_DISPLAY_HEIGHT,
  IDP_CAMERA_CAPTURE_TO_JPEG_DISPLAY_FRAME_BUFFER_ADDRESS,
  IDP_CAMERA_CAPTURE_TO_JPEG_RESIZER_V_VALUE
#endif
};

enum
{
  IDP_CAMERA_CAPTURE_TO_JPEG_QUERY_TYPE = 1,
  IDP_CAMERA_CAPTURE_TO_JPEG_QUERY_ARG_PATH,
  IDP_CAMERA_CAPTURE_TO_JPEG_QUERY_ARG_CLR_FMT,
  IDP_CAMERA_CAPTURE_TO_JPEG_QUERY_ARG_DESIRED_ANGLE,  
  IDP_CAMERA_CAPTURE_TO_JPEG_QUERY_RESULT
};

extern kal_bool idp_camera_capture_to_jpeg_open_real(
  kal_uint32 * const key,
  char const * const filename,
  kal_uint32 const lineno);

extern kal_bool idp_camera_capture_to_jpeg_close(
  kal_uint32 const key);

extern kal_bool idp_camera_capture_to_jpeg_stop(
  kal_uint32 const key);

extern kal_bool idp_camera_capture_to_jpeg_start_intermedia_pipe(
  kal_uint32 const key);

extern kal_bool idp_camera_capture_to_jpeg_start_input(
  kal_uint32 const key);

extern kal_bool idp_camera_capture_to_jpeg_start_output(
  kal_uint32 const key);

extern kal_bool idp_camera_capture_to_jpeg_start_all(
  kal_uint32 const key);

extern kal_bool idp_camera_capture_to_jpeg_config(
  kal_uint32 const key,
  ...);

extern kal_bool idp_camera_capture_to_jpeg_configurable(
  kal_uint32 const key,
  ...);

extern kal_bool idp_camera_preview_to_jpeg_config_fast(
  kal_uint32 const key,
  kal_uint32 const para_type,
  kal_uint32 const para_value);

extern kal_bool idp_camera_capture_to_jpeg_query(
  kal_uint32 const key,
  ...);

extern kal_bool idp_camera_capture_to_jpeg_config_and_start(
  kal_uint32 const key,
  ...);

extern kal_bool idp_camera_capture_to_jpeg_is_busy(
  kal_uint32 const key,
  kal_bool * const busy);

extern kal_bool idp_camera_capture_to_jpeg_is_in_use(void);

#define idp_camera_capture_to_jpeg_open(key) idp_camera_capture_to_jpeg_open_real(key, __FILE__, __LINE__)

#endif
