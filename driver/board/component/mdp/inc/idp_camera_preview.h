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

#ifndef __idp_camera_preview_h__
#define __idp_camera_preview_h__

#include <idp_core.h>
#include "idp_define.h"
//#include "kal_general_types.h"

enum
{
    IDP_CAMERA_PREVIEW_EXTMEM_START_ADDRESS = 1
  , IDP_CAMERA_PREVIEW_EXTMEM_SIZE

  , IDP_CAMERA_PREVIEW_IMAGE_SRC_WIDTH
  , IDP_CAMERA_PREVIEW_IMAGE_SRC_HEIGHT
  , IDP_CAMERA_PREVIEW_PREVIEW_WIDTH        // display_width
  , IDP_CAMERA_PREVIEW_PREVIEW_HEIGHT       // display_height
  , IDP_CAMERA_PREVIEW_FRAME_BUFFER_ADDRESS
  , IDP_CAMERA_PREVIEW_FRAME_BUFFER_ADDRESS1
  , IDP_CAMERA_PREVIEW_FRAME_BUFFER_ADDRESS2
  , IDP_CAMERA_PREVIEW_DISPLAY_ROT_ANGLE    // for display path rotation

  , IDP_CAMERA_PREVIEW_PREVIEW_IMAGE_COLOR_FORMAT /**< img_color_fmt_rgb_enum_t*/
  , IDP_CAMERA_PREVIEW_ENABLE_PREVIEW_PITCH
  , IDP_CAMERA_PREVIEW_PREVIEW_PITCH_WIDTH

  , IDP_CAMERA_PREVIEW_ENABLE_OVERLAY
  , IDP_CAMERA_PREVIEW_SET_OVERLAY_PALETTE_SIZE_AND_ADDR
  , IDP_CAMERA_PREVIEW_OVERLAY_PALETTE_MODE
  , IDP_CAMERA_PREVIEW_OVERLAY_FRAME_SOURCE_KEY
  , IDP_CAMERA_PREVIEW_OVERLAY_FRAME_WIDTH
  , IDP_CAMERA_PREVIEW_OVERLAY_FRAME_HEIGHT
  , IDP_CAMERA_PREVIEW_OVERLAY_FRAME_BUFFER_ADDRESS

  , IDP_CAMERA_PREVIEW_POSTPROC_PATH_ENABLE
  , IDP_CAMERA_PREVIEW_POSTPROC_ROT_ANGLE
  , IDP_CAMERA_PREVIEW_POSTPROC_IMAGE_WIDTH
  , IDP_CAMERA_PREVIEW_POSTPROC_IMAGE_HEIGHT
  , IDP_CAMERA_PREVIEW_POSTPROC_BUFFER_ADDRESS_1
  , IDP_CAMERA_PREVIEW_POSTPROC_BUFFER_ADDRESS_2
  , IDP_CAMERA_PREVIEW_POSTPROC_BUFFER_ADDRESS_3
  , IDP_CAMERA_PREVIEW_POSTPROC_IMAGE_COLOR_FORMAT

  , IDP_CAMERA_PREVIEW_FRAME_START_HISR_CB_ENABLE
  , IDP_CAMERA_PREVIEW_PREVIEW_CROP_ENABLE
  , IDP_CAMERA_PREVIEW_PREVIEW_CROP_WIDTH
  , IDP_CAMERA_PREVIEW_PREVIEW_CROP_HEIGHT

  , IDP_CAMERA_PREVIEW_ENABLE_TRIGGER_LCD /**< KAL_TRUE to enable trigger LCD, KAL_FALSE to disable trigger LCD */

  , IDP_CAMERA_PREVIEW_LAST_MANDATORY_USER_CONFIG
  // ***********************Mandatory parameter definition end****************************
  // The following region defines the optional parameters which will not be checked by idp_finish_read_user_config()

  // currently unused interfaces
  , IDP_CAMERA_PREVIEW_INTMEM_START_ADDRESS
  , IDP_CAMERA_PREVIEW_INTMEM_SIZE
  , IDP_CAMERA_PREVIEW_RESIZER_U_VALUE
  , IDP_CAMERA_PREVIEW_RESIZER_D_VALUE

  // This param is used for MATV chip MT5931 issue workaround: 
  // Delay trigger display timing from current frame done interrupt to next CRZ frame start interrupt
  , IDP_CAMERA_PREVIEW_ENABLE_TRIGGER_DISPLAY_DELAY_TO_FRAME_START
  
  // This param is used for Lomo effect and Color Accent feature:
  // Move the trigger display procedure from MDP to CAL
  , IDP_CAMERA_PREVIEW_ENABLE_DOUBLE_BUFFER_TEARING_FREE
  // This param is used for Mav capture feature
  , IDP_CAMERA_PREVIEW_ENABLE_MAV_CAPTURE
  // This param is used for ZSD capture feature:
  // Inform and bring captrue related paramters to start ZSD capture process for MDP
  , IDP_CAMERA_PREVIEW_ZSD_CAPTURE_START

#if ENABLE_UT
  , IDP_CAMERA_PREVIEW_IMAGE_BUFFER_ADDRESS
#endif  // #if ENABLE_UT

  , IDP_CAMERA_PREVIEW_TEMP_BUFFER

  , IDP_CAMERA_PREVIEW_ENG_CONFIG  // Only for MT6250 no check bit and use end config

  //SOP of adding new parameter for this scenario
  //1. Add a new definition here
  //2. Add a corresponding variable in the config struct
  //3. Make sure adding the matching handler for this definition in the "read_user_config_hook" function
  //   In the switch case, default case would cause "ASSERT(0)"!!!!!
  //4. Make sure SET_BIT to variable of "read_user_config" in the "read_user_config_hook" function
  //5. If no use in some platforms, please make sure adding the SET_BIT in the init_config_hook function
  , IDP_CAMERA_PREVIEW_LAST_USER_CONFIG

#if 0
  //deprecated ones
  , IDP_CAMERA_PREVIEW_ENABLE_LAST_STAGE_DONE_LISR
  , IDP_CAMERA_PREVIEW_FLIP
  , IDP_CAMERA_PREVIEW_ADV_LCD_RD_PTR
  , IDP_CAMERA_PREVIEW_FRAME_START_HISR_CB_AND_PARAM
  , IDP_CAMERA_PREVIEW_LAST_STAGE_DONE_LISR_CB_AND_PARAM
  , IDP_CAMERA_PREVIEW_LAST_STAGE_DONE_HISR_CB_AND_PARAM
  , IDP_CAMERA_PREVIEW_POSTPROC_IMAGE_DONE_HISR_CB_AND_PARAM
  , IDP_CAMERA_PREVIEW_AUTO_RESTART
  , IDP_CAMERA_PREVIEW_ENABLE_PITCH
  , IDP_CAMERA_PREVIEW_BG_IMAGE_WIDTH
  , IDP_CAMERA_PREVIEW_ROT_ANGLE
  , IDP_CAMERA_PREVIEW_IMAGE_DATA_FORMAT /**< img_color_fmt_rgb_enum_t or
                                              IMG_COLOR_FMT_PACKET_YUYV422 */
  , IDP_CAMERA_PREVIEW_RESIZER_V_VALUE
#endif
};

enum
{
  IDP_CAMERA_PREVIEW_QUERY_TYPE = 1,
  IDP_CAMERA_PREVIEW_QUERY_ARG_PATH,
  IDP_CAMERA_PREVIEW_QUERY_ARG_CLR_FMT,
  IDP_CAMERA_PREVIEW_QUERY_ARG_DESIRED_ANGLE,  
  IDP_CAMERA_PREVIEW_QUERY_RESULT
};

enum
{
  IDP_CAMERA_PREVIEW_QUERY_CURRENT_BUFFER_ADDR_FOR_FACE_DETECTION = 1
};
    
extern kal_bool idp_camera_preview_open_real(
  kal_uint32 * const key,
  char const * const filename,
  kal_uint32 const lineno);

extern kal_bool idp_camera_preview_close(
  kal_uint32 const key);

extern kal_bool idp_camera_preview_stop(
  kal_uint32 const key);

extern kal_bool idp_camera_preview_start_intermedia_pipe(
  kal_uint32 const key);

extern kal_bool idp_camera_preview_start_input(
  kal_uint32 const key);

extern kal_bool idp_camera_preview_start_output(
  kal_uint32 const key);

extern kal_bool idp_camera_preview_start_all(
  kal_uint32 const key);

extern kal_bool idp_camera_preview_config(
  kal_uint32 const key,
  ...);

extern kal_bool idp_camera_preview_configurable(
  kal_uint32 const key,
  ...);

extern kal_bool idp_camera_preview_config_fast(
    kal_uint32 const key,
    kal_uint32 const para_type,
    kal_uint32 const para_value);

extern kal_bool idp_camera_preview_query(
  kal_uint32 const key,
  ...);

extern kal_bool idp_camera_preview_config_and_start(
  kal_uint32 const key,
  ...);

extern kal_bool idp_camera_preview_is_busy(
  kal_uint32 const key,
  kal_bool * const busy);

extern kal_bool idp_camera_preview_is_in_use(void);

#define idp_camera_preview_open(key) idp_camera_preview_open_real(key, __FILE__, __LINE__)

#endif
