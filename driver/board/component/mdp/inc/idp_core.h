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

#ifndef __idp_core_h__
#define __idp_core_h__

/// \defgroup idp
/// @{

/// \mainpage
/// \dot
/// digraph jpeg_encode_idp {
/// rankdir=LR;
/// PRZ_R2 [label="PRZ" URL="\ref prz" style=filled fillcolor="blue" fontcolor="white"];
/// IPP1_R2 [label="IPP1" URL="\ref imgproc" style=filled fillcolor="blue" fontcolor="white"];
/// Y2R0_R [label="YUV\n2\nRGB_0" URL="\ref imgproc" style=filled fillcolor="blue" fontcolor="white"];
/// IPP2_R [label="IPP2" URL="\ref imgproc" style=filled fillcolor="blue" fontcolor="white"];
/// IBW2_R [label="IBW2" URL="\ref imgdma_ibw2" style=filled fillcolor="blue" fontcolor="white"];
/// IRT1_R [label="Rotator_1" URL="\ref imgdma_irt1" style=filled fillcolor="blue" fontcolor="white"];
/// IPP1_L [label="IPP1" URL="\ref imgproc" style=filled fillcolor="blue" fontcolor="white"];
/// Y2R0_L [label="YUV\n2\nRGB_0" URL="\ref imgproc" style=filled fillcolor="blue" fontcolor="white"];
/// IPP2_L [label="IPP2" URL="\ref imgproc" style=filled fillcolor="blue" fontcolor="white"];
/// IBW2_L [label="IBW2" URL="\ref imgdma_ibw2" style=filled fillcolor="blue" fontcolor="white"];
/// IRT1_L [label="Rotator_1" URL="\ref imgdma_irt1" style=filled fillcolor="blue" fontcolor="white"];
/// VDODEC_RDMA [label="Video\nDecoder\nRDMA" URL="\ref imgdma_video_decode" style=filled fillcolor="blue" fontcolor="white"];
/// IRT0 [label="Rotator_0" URL="\ref imgdma_irt0" style=filled fillcolor="blue" fontcolor="white"];
/// MP4_DEBLK [label="Deblocking\nfilter" URL="\ref imgdma_mp4_deblk" style=filled fillcolor="blue" fontcolor="white"];
/// IBR1 [label="IBR1" URL="\ref imgdma_ibr1" style=filled fillcolor="blue" fontcolor="white"];
/// R2Y0_R [label="RGB\n2\nYUV_0" URL="\ref imgproc" style=filled fillcolor="blue" fontcolor="white"];
/// PRZ_R1 [label="PRZ" URL="\ref prz" style=filled fillcolor="blue" fontcolor="white"];
/// IPP1_R1 [label="IPP1" URL="\ref imgproc" style=filled fillcolor="blue" fontcolor="white"];
/// DRZ [label="DRZ" URL="\ref drz" style=filled fillcolor="blue" fontcolor="white"];
/// Y2R1_R [label="YUV\n2\nRGB_1" URL="\ref imgproc" style=filled fillcolor="blue" fontcolor="white"];
/// IBW1_R [label="IBW1" URL="\ref imgdma_ibw1" style=filled fillcolor="blue" fontcolor="white"];
/// IBR2 [label="IBR2" URL="\ref imgdma_ibr2" style=filled fillcolor="blue" fontcolor="white"];
/// IRT2 [label="Rotator_2" URL="\ref imgdma_irt2" style=filled fillcolor="blue" fontcolor="white"];
/// IRT3 [label="Rotator_3" URL="\ref imgdma_irt3" style=filled fillcolor="blue" fontcolor="white"];
/// JPEG_DECODER [label="JPEG\nDecoder"];
/// PRZ_BLK [label="Block-based\nConversion" URL="\ref prz" style=filled fillcolor="blue" fontcolor="white"];
/// CRZ_R2 [label="CRZ" URL="\ref crz" style=filled fillcolor="blue" fontcolor="white"];
/// JPEG_DMA [URL="\ref imgdma_jpeg" style=filled fillcolor="blue" fontcolor="white"];
/// JPEG_ENCODER [label="JPEG\nEncoder"];
/// Y2R1_L [label="YUV\n2\nRGB_1" URL="\ref imgproc" style=filled fillcolor="blue" fontcolor="white"];
/// IBW1_L [label="IBW1" URL="\ref imgdma_ibw1" style=filled fillcolor="blue" fontcolor="white"];
/// LCD;
/// ISP;
/// CRZ_R1 [label="CRZ" URL="\ref crz" style=filled fillcolor="blue" fontcolor="white"];
/// OVL [URL="\ref imgdma_ovl" style=filled fillcolor="blue" fontcolor="white"];
/// VDOENC_WDMA [label="Video\nEncoder\nWDMA" URL="\ref imgdma_video_encode" style=filled fillcolor="blue" fontcolor="white"];
/// TV_OUT_R1 [label="TV Out"];
/// TV_OUT_R2 [label="TV Out"];
/// TV_OUT_R3 [label="TV Out"];
/// PRZ_or_CRZ [label="PRZ\nor\nCRZ" URL="\ref prz" style=filled fillcolor="blue" fontcolor="white"];
/// R2Y0_L [label="RGB\n2\nYUV_0" URL="\ref imgproc" style=filled fillcolor="blue" fontcolor="white"];
/// MPEG4_CODEC [label="MPEG-4 Codec\nor\nH.264e Decoder"];
/// ICON_FB [label="Icon\nBuffer"];
/// {
///   rank=same;
///   Y2R1_L;
///   IBW1_L;
///   LCD;
///   ICON_FB;
/// }
/// {
///   rank=same;
///   PRZ_or_CRZ;
///   R2Y0_L;
/// }
/// {
///   rank=same;
///   MP4_DEBLK;
///   IPP1_L;
/// }
/// {
///   rank=same;
///   IRT0;
///   IBW2_L;
///   IRT3;
/// }
/// {
///   rank=same;
///   MPEG4_CODEC;
///   VDODEC_RDMA;
///   IRT1_L;
///   IBR2;
/// }
/// subgraph cluster_FB
/// {
///   rank=same;
///   label="External Memory";
///   BMP_FB [label="BMP or Pixel Buffer"];
///   VOP_FB [label="VOP Frame Buffer"];
///   REC_FB [label="Rec Frame Buffer"];
///   LCD_FB_1 [label="LCD Frame Buffer"];
///   LCD_FB_2 [label="LCD Frame Buffer"];
///   JPEG_FB_1 [label="JPEG File Buffer"];
///   JPEG_FB_2 [label="JPEG File Buffer"];
///   TV_FB_1 [label="TV Out Buffer"];
///   TV_FB_2 [label="TV Out Buffer"];
///   THUMB_FB [label="Thumbnail Buffer"];
/// }
/// {
///   rank=same;
///   IBR1;
///   VDOENC_WDMA;
///   TV_OUT_R1;
///   IRT1_R;
///   TV_OUT_R2;
///   JPEG_DECODER;
///   JPEG_ENCODER;
///   TV_OUT_R3;
///   IBW1_R;
/// }
/// {
///   rank=same;
///   PRZ_R1;
///   IPP1_R2;
///   CRZ_R2;
///   JPEG_DMA;
/// }
/// {
///   rank=same;
///   IPP1_R1;
///   PRZ_R2;
///   DRZ;
/// }
/// {
///   rank=same;
///   ISP;
///   CRZ_R1;
///   OVL;
/// }
/// BMP_FB -> IBR1 -> R2Y0_R -> PRZ_R1 -> IPP1_R1;
/// MPEG4_CODEC -> VOP_FB [dir=back];
/// REC_FB -> TV_OUT_R1;
/// VDODEC_RDMA -> REC_FB [dir=back];
/// LCD_FB_2 -> TV_OUT_R2;
/// IBR2 -> LCD_FB_2 [dir=back];
/// TV_FB_2 -> TV_OUT_R3;
/// IRT2 -> IBR2 [dir=back];
/// PRZ_or_CRZ -> IRT0 [dir=back];
/// MP4_DEBLK -> IRT0 [dir=back];
/// MP4_DEBLK -> IPP1_L;
/// LCD_FB_1 -> IRT1_R -> IBW2_R -> IPP2_R -> Y2R0_R -> IPP1_R2 -> PRZ_R2 [dir=back];
/// IRT1_L -> LCD_FB_1;
/// LCD -> IRT2 [dir=back];
/// IRT3 -> TV_FB_1;
/// IBW1_L -> LCD;
/// IBW2_L -> IRT1_L;
/// LCD -> IBW2_L [dir=back];
/// IPP1_L -> Y2R0_L;
/// IPP1_R1 -> OVL;
/// IPP2_L -> IBW2_L;
/// Y2R0_L -> IPP2_L;
/// THUMB_FB -> IBW1_R -> Y2R1_R -> DRZ [dir=back];
/// Y2R1_L -> IBW1_L;
/// PRZ_or_CRZ -> R2Y0_L [dir=back];
/// PRZ_R1 -> IPP1_R2;
/// Y2R1_L -> PRZ_or_CRZ [dir=back];
/// PRZ_or_CRZ -> IPP1_L;
/// CRZ_R1 -> OVL;
/// CRZ_R2 -> OVL;
/// IPP1_R2 -> CRZ_R2 [dir=back];
/// VOP_FB -> VDOENC_WDMA -> OVL [dir=back];
/// JPEG_DMA -> OVL [dir=back];
/// DRZ -> OVL [dir=back];
/// PRZ_R2 -> OVL [dir=back];
/// IRT0 -> VDODEC_RDMA [dir=back];
/// PRZ_or_CRZ -> MP4_DEBLK [dir=back];
/// JPEG_FB_1 -> JPEG_DECODER -> PRZ_BLK -> CRZ_R2;
/// JPEG_FB_2 -> JPEG_ENCODER -> JPEG_DMA [dir=back];
/// ISP -> CRZ_R1;
/// LCD -> ICON_FB [dir=back];
/// LCD -> IRT3;
/// MPEG4_CODEC -> REC_FB;
/// R2Y0_L -> IRT2 [dir=back];
/// }
/// \enddot

/// \page design_principle Design Principle
/// There are some design principle:
///
/// -# Each data path has identical API interface:
///   - open\n
/// Open the data path, the return value of this function is a 'key', each successive API calls of this data path need this 'key'.\n
///   - close\n
/// Release the 'key' got from 'open', so that others can use this data path.\n
///   - start_input\n
/// Start the IDP hardware component for input.\n
///   - start_output\n
/// Start the IDP hardware component for output.\n
///   - start_intermedia\n
/// Start the IDP hardware component between the input one and the output one.\n
///   - start_all\n
/// Start all IDP hardware component used by this data path.\n
///   - stop\n
/// Stop all IDP hardware component used by this data path.\n
///   - config\n
/// Configure this data path. The attribute can be hardware relative or irrelative.\n
///   - config_fast\n
/// Configure this data path. The attribute must be hardware irrelative. This function provide a faster way to configure the data path.\n
///   - configurable\n
/// return KAL_TRUE or KAL_FALSE to determine whether these configure attributes is suitable for this data path.\n
///   - query\n
/// Query some attributes of this data path, ex: error code when something wrong.\n
///   - config_and_start\n
/// Configure this data path and start all the hardware component immediately.\n
///   - is_busy\n
/// Check if this data path is busy.\n
///   - is_in_use\n
/// Check if all the hardware component used by this data path is available.\n
///

/// \page porting_guide Porting guide
/// There are some steps to write IDP relative codes for the new chips:
///
/// -# Create a new folder under drv\, and use the name of the new chip as this new folder's name.
/// -# Under drv<new chip>\, create a file named 'idp_mem.h'. This file will specify the memory comsumption of each data path defined on this new platform.
/// -# Create a new folder : drv<new chip>
/// -# Put hardware relative header files into this new folder: drv<new chip>
/// -# Create a new folder under drv\, and use the name of the new chip as this new folder's name.
/// -# Create a new folder : drv<new chip>
/// -# Put hardware relative source files into this new folder: drv<new chip>
/// -# Under drv<new chip>\, put source files for each data path.
///

//#include "kal_general_types.h"
//#include "kal_public_api.h"
#include "drv_features_mdp.h"
#include <idp_define.h>

#if defined(DRV_IDP_SUPPORT)

#if !defined(IDP_DVT_LOAD)
//#include <drv_comm.h>
#endif // #if !defined(IDP_DVT_LOAD)

#if 0
#if defined(DRV_IDP_6238_SERIES)
#include <mt6238/idp_mem.h>
#endif // #if defined(DRV_IDP_6238_SERIES)

#if defined(DRV_IDP_6253_SERIES)
#include <mt6253/idp_mem.h>
#endif // #if defined(DRV_IDP_6253_SERIES)

#if defined(DRV_IDP_MT6236_SERIES)
#include <mt6236/idp_mem.h>
#endif // #if defined(DRV_IDP_MT6236_SERIES)

#if defined(DRV_IDP_MT6276_SERIES) || defined(DRV_IDP_MT6256_SERIES) || defined(DRV_IDP_MT6276E2_SERIES)
#include <mt6276/idp_mem.h>
#endif
#endif  

#if defined(DRV_IDP_MT6276_SERIES) || defined(DRV_IDP_MT6276E2_SERIES) || defined(DRV_IDP_MT6256_E1_SERIES)
#include <mt6276/idp_owner.h>
#define IDP_OWNER_BYCHIP
#endif // #if defined(DRV_IDP_MT6276_SERIES)

#if defined(DRV_IDP_MT6256_E2_SERIES)
#include <mt6256_e2/idp_owner.h>
#define IDP_OWNER_BYCHIP
#endif // #if defined(DRV_IDP_MT6256_E2_SERIES)
#if defined(DRV_IDP_MT6255_SERIES)
#include <mt6255/idp_owner.h>
#define IDP_OWNER_BYCHIP
#endif // #if defined(DRV_IDP_MT6255_SERIES)


#include <idp_hisr.h>

#define MAX_CONCURRENT_DP (2)

typedef void (*idp_lisr_intr_cb_t)(void *);
typedef void *idp_lisr_intr_cb_param_t;

typedef kal_bool (*idp_hisr_intr_cb_t)(void *);
typedef void *idp_hisr_intr_cb_param_t;

#include <stdarg.h>


#if !defined(IDP_OWNER_BYCHIP)
struct idp_hw_owner_t
{
  kal_semid sem;
  kal_uint32 key;

  //char const *filename;
  //kal_uint32 line_no;
  //kal_taskid task;
};
typedef struct idp_hw_owner_t idp_hw_owner_t;

/**
 *  Assume this still be project independent at
 *  the time of porting MT6236 MDP driver
 */
enum idp_scenario_t
{
  IDP_SCENARIO_camera_preview,
  IDP_SCENARIO_camera_preview_with_face_detection,
  IDP_SCENARIO_camera_capture_to_jpeg,
  IDP_SCENARIO_camera_capture_to_mem,
  IDP_SCENARIO_camera_capture_to_barcode,
  IDP_SCENARIO_camera_capture_to_ybuffer,
  IDP_SCENARIO_video_decode,
  IDP_SCENARIO_video_encode,
  IDP_SCENARIO_video_call_encode,
  IDP_SCENARIO_video_call_decode,
  IDP_SCENARIO_video_editor_encode,
  IDP_SCENARIO_video_editor_decode,
  IDP_SCENARIO_rgb2yuv,
  IDP_SCENARIO_jpeg_decode,
  IDP_SCENARIO_jpeg_encode,
  IDP_SCENARIO_jpeg_resize,
  IDP_SCENARIO_image_effect_pixel,
  IDP_SCENARIO_image_resize,
  IDP_SCENARIO_webcam,
  IDP_SCENARIO_simple_display_with_rotate
};
typedef enum idp_scenario_t idp_scenario_t;

// TODO: make this one chip dependent
struct idp_owner_t
{
  kal_uint32 key;

  idp_scenario_t scenario;

#if (defined(DRV_IDP_6252_SERIES))
  kal_uint32 crz_key;
  kal_uint32 irt0_key;

  kal_taskid task;

  kal_bool have_config_imgdma_irt0;

#elif (defined(DRV_IDP_6253_SERIES))
  kal_uint32 crz_key;

  kal_taskid task;

  kal_bool have_config_resz_crz;
  kal_bool have_set_mem_resz_crz;

#else

  kal_uint32 prz_key;
  kal_uint32 crz_key;
  kal_uint32 drz_key;

  kal_uint32 ibr1_key;
  kal_uint32 ibr2_key;
  kal_uint32 ibw1_key;
  kal_uint32 ibw2_key;
  kal_uint32 irt0_key;
  kal_uint32 irt1_key;
  kal_uint32 irt2_key;
  kal_uint32 irt3_key;
  kal_uint32 ovl_key;
  kal_uint32 jpeg_key;
  kal_uint32 video_decode_key;
  kal_uint32 video_encode_key;

  kal_uint32 ipp_key;
  kal_uint32 y2r1_key;
  kal_uint32 r2y0_key;

  kal_uint32 mp4deblk_key;

  //char const *filename;
  //kal_uint32 line_no;
  kal_taskid task;

  /* ***** */
  kal_bool have_config_resz_prz;
  kal_bool have_config_resz_crz;
  kal_bool have_config_resz_drz;

  kal_bool have_config_imgdma_ibr1;
  kal_bool have_config_imgdma_ibr2;
  kal_bool have_config_imgdma_ibw1;
  kal_bool have_config_imgdma_ibw2;
  kal_bool have_config_imgdma_irt0;
  kal_bool have_config_imgdma_irt1;
  kal_bool have_config_imgdma_irt2;
  kal_bool have_config_imgdma_irt3;
  kal_bool have_config_imgdma_ovl;
  kal_bool have_config_imgdma_jpeg;
  kal_bool have_config_imgdma_video_decode;
  kal_bool have_config_imgdma_video_encode;

  kal_bool have_config_imgproc_ipp;
  kal_bool have_config_imgproc_y2r1;
  kal_bool have_config_imgproc_r2y0;

  kal_bool have_config_mp4deblk;
  /* ***** */

  /* ***** */
  kal_bool have_set_mem_resz_prz;
  kal_bool have_set_mem_resz_crz;
  kal_bool have_set_mem_resz_drz;

  kal_bool have_set_mem_imgdma_ibr1;
  kal_bool have_set_mem_imgdma_ibr2;
  kal_bool have_set_mem_imgdma_ibw1;
  kal_bool have_set_mem_imgdma_ibw2;
  kal_bool have_set_mem_imgdma_irt0;
  kal_bool have_set_mem_imgdma_irt1;
  kal_bool have_set_mem_imgdma_irt2;
  kal_bool have_set_mem_imgdma_irt3;
  kal_bool have_set_mem_imgdma_ovl;
  kal_bool have_set_mem_imgdma_jpeg;
  kal_bool have_set_mem_imgdma_video_decode;
  kal_bool have_set_mem_imgdma_video_encode;

  kal_bool have_set_mem_imgproc_ipp;
  kal_bool have_set_mem_imgproc_y2r1;
  kal_bool have_set_mem_imgproc_r2y0;

  kal_bool have_set_mem_mp4deblk;

#endif //#if (defined(DRV_IDP_6252_SERIES))

};
typedef struct idp_owner_t idp_owner_t;
#endif

struct idp_line_count_ratio_t
{
  kal_uint32 ratio[10];
  kal_uint32 ratio_size;
  kal_uint32 victim_idx;
};
typedef struct idp_line_count_ratio_t idp_line_count_ratio_t;

extern void idp_line_count_ratio_init(idp_line_count_ratio_t * const data,
                                      kal_uint32 const size);
extern void idp_line_count_ratio_minus_one(idp_line_count_ratio_t * const data);

typedef void     (*idp_init_config_hook_t)(idp_owner_t * const owner);
typedef kal_bool (*idp_open_hook_t)(idp_owner_t * const owner);
typedef kal_bool (*idp_close_hook_t)(idp_owner_t * const owner);
typedef kal_bool (*idp_stop_hook_t)(idp_owner_t * const owner);
//typedef kal_bool (*idp_stop_for_config_hook_t)(idp_owner_t * const owner);
//typedef void     (*idp_read_user_config_hook_t)(idp_owner_t * const owner, va_list ap);
//typedef void     (*idp_read_user_config_fast_hook_t)(idp_owner_t * const owner, kal_uint32 const para_type, kal_uint32 const para_value);
typedef kal_bool     (*idp_read_user_config_hook_t)(idp_owner_t * const owner, va_list ap);
typedef kal_bool     (*idp_read_user_config_fast_hook_t)(idp_owner_t * const owner, kal_uint32 const para_type, kal_uint32 const para_value);
typedef kal_bool (*idp_finish_read_user_config_hook_t)(void);
typedef kal_bool (*idp_hardware_setting_have_been_changed_hook_t)(idp_owner_t * const owner);
typedef kal_bool (*idp_config_hook_t)(idp_owner_t * const owner, kal_bool const config_to_hardware);
typedef kal_bool (*idp_query_hook_t)(idp_owner_t * const owner, va_list ap);
typedef kal_bool (*idp_start_hook_t)(idp_owner_t * const owner);
typedef void     (*idp_compute_mem_for_each_hw_hook_t)(idp_owner_t * const owner);
typedef kal_bool (*idp_is_busy_hook_t)(idp_owner_t const * const owner, kal_bool * const busy);
typedef kal_bool (*idp_is_in_use_hook_t)(void);
typedef void     (*idp_clear_setting_diff_hook_t)(idp_owner_t * const owner);
typedef kal_bool (*idp_is_setting_diff_is_ok_when_busy_hook_t)(idp_owner_t * const owner);

struct hook_collect_t
{
  idp_init_config_hook_t idp_init_config_hook;
  idp_open_hook_t idp_open_hook;
  idp_close_hook_t idp_close_hook;
  idp_stop_hook_t idp_stop_hook;
//  idp_stop_for_config_hook_t idp_stop_for_config_hook;
  idp_read_user_config_hook_t idp_read_user_config_hook;
  idp_read_user_config_fast_hook_t idp_read_user_config_fast_hook;
  idp_finish_read_user_config_hook_t idp_finish_read_user_config_hook;
  idp_hardware_setting_have_been_changed_hook_t idp_hardware_setting_have_been_changed_hook;
  idp_config_hook_t idp_config_hook;
  idp_query_hook_t idp_query_hook;
  idp_start_hook_t idp_start_intermedia_pipe_hook;
  idp_start_hook_t idp_start_input_hook;
  idp_start_hook_t idp_start_output_hook;
  idp_compute_mem_for_each_hw_hook_t idp_compute_mem_for_each_hw_hook;
  idp_is_busy_hook_t idp_is_busy_hook;
  idp_is_in_use_hook_t idp_is_in_use_hook;
  idp_clear_setting_diff_hook_t idp_clear_setting_diff_hook;
  idp_is_setting_diff_is_ok_when_busy_hook_t idp_is_setting_diff_is_ok_when_busy_hook;
};
typedef struct hook_collect_t hook_collect_t;

#define IDP_HW_COMMON_API_DECLARATION(name)           \
  extern kal_bool idp_##name##_open(                  \
      kal_uint32 * const key);                        \
                                                      \
  extern kal_bool idp_##name##_close(                 \
      kal_uint32 const key,                           \
      idp_##name##_struct const * const config);      \
                                                      \
  extern kal_bool idp_##name##_config(                \
      kal_uint32 const key,                                             \
      idp_##name##_struct * const config,                               \
      kal_bool const config_to_hardware);                               \
                                                                        \
  extern kal_bool idp_##name##_query(                 \
      idp_##name##_struct * const config, ...);       \
                                                      \
  extern kal_bool idp_##name##_get_working_mem_size(  \
      kal_uint32 const key,                           \
      idp_##name##_struct * const config,             \
      kal_uint32 * const size);                       \
                                                      \
  extern kal_bool idp_##name##_get_working_mem(       \
      kal_uint32 const key,                           \
      kal_bool use_internal_mem,                      \
      idp_##name##_struct * const config,             \
      kal_bool const do_alloc,                        \
      kal_uint32 const max_allowable_size);           \
                                                      \
  extern kal_bool idp_##name##_init(void);            \
                                                      \
  extern kal_bool idp_##name##_start(                 \
      kal_uint32 const key,                           \
      idp_##name##_struct const * const config);      \
                                                      \
  extern kal_bool idp_##name##_stop(                  \
      kal_uint32 const key,                           \
      idp_##name##_struct const * const config);      \
                                                      \
  extern kal_bool idp_##name##_is_busy(               \
      kal_uint32 const key,                           \
      kal_bool * const busy,                          \
      idp_##name##_struct const * const config);      \
                                                      \
  extern kal_bool idp_##name##_is_in_use(void);

#if 0 //!defined(__MTK_TARGET__)
#define IDP_HW_COMMON_API_DEFINITION(name)      \
  kal_bool                                      \
  idp_##name##_open(                            \
      kal_uint32 * const key,                   \
      char const * const filename,              \
      kal_uint32 const lineno)                  \
  {                                             \
    return KAL_TRUE;                            \
  }                                             \
                                                \
  kal_bool                                      \
  idp_##name##_close(                           \
      kal_uint32 const key,                     \
      idp_##name##_struct const * const config) \
  {                                             \
    return KAL_TRUE;                            \
  }                                             \
                                                \
  kal_bool                                      \
  idp_##name##_config(                          \
      kal_uint32 const key,                     \
      idp_##name##_struct * const config,       \
      kal_bool const config_to_hardware)        \
  {                                             \
    return KAL_TRUE;                            \
  }                                             \
                                                \
  kal_bool                                      \
  idp_##name##_get_working_mem_size(            \
      kal_uint32 const key,                     \
      idp_##name##_struct * const config,       \
      kal_uint32 * const size)                  \
  {                                             \
    return KAL_TRUE;                            \
  }                                             \
                                                \
  kal_bool                                      \
  idp_##name##_get_working_mem(                 \
      kal_uint32 const key,                     \
      kal_bool const use_internal_mem,          \
      idp_##name##_struct * const config,       \
      kal_bool const do_alloc,                  \
      kal_uint32 const max_allowable_size)      \
  {                                             \
    return KAL_TRUE;                            \
  }                                             \
                                                \
  kal_bool                                      \
  idp_##name##_init(void)                       \
  {                                             \
    return KAL_TRUE;                            \
  }                                             \
                                                \
  kal_bool                                      \
  idp_##name##_start(                           \
      kal_uint32 const key,                     \
      idp_##name##_struct const * const config) \
  {                                             \
    return KAL_TRUE;                            \
  }                                             \
                                                \
  kal_bool                                      \
  idp_##name##_stop(                            \
      kal_uint32 const key,                     \
      idp_##name##_struct const * const config) \
  {                                             \
    return KAL_TRUE;                            \
  }                                             \
                                                \
  kal_bool                                      \
  idp_##name##_is_busy(                         \
      kal_uint32 const key,                     \
      kal_bool * const busy,                    \
      idp_##name##_struct const * const config) \
  {                                             \
    return KAL_TRUE;                            \
  }                                             \
                                                \
  kal_bool                                      \
  idp_##name##_is_in_use(void)                  \
  {                                             \
    return KAL_TRUE;                            \
  }
#endif

#define IDP_COMMON_API_DECLARATION(name)              \
  extern kal_bool idp_##name##_open_real(             \
      kal_uint32 * const key,                         \
      char const * const filename,                    \
      kal_uint32 const lineno);                       \
                                                      \
  extern kal_bool idp_##name##_is_in_use(void);       \
                                                      \
  extern kal_bool idp_##name##_close(                 \
      kal_uint32 const key);                          \
                                                      \
  extern kal_bool idp_##name##_stop(                  \
      kal_uint32 const key);                          \
                                                      \
  extern kal_bool idp_##name##_start_intermedia_pipe( \
      kal_uint32 const key);                          \
                                                      \
  extern kal_bool idp_##name##_start_input(           \
      kal_uint32 const key);                          \
                                                      \
  extern kal_bool idp_##name##_start_output(          \
      kal_uint32 const key);                          \
                                                      \
  extern kal_bool idp_##name##_start_all(             \
      kal_uint32 const key);                          \
                                                      \
  extern kal_bool idp_##name##_config_fast(           \
      kal_uint32 const key,                           \
      kal_uint32 const para_type,                     \
      kal_uint32 const para_value);                   \
                                                      \
  extern kal_bool idp_##name##_config(                \
      kal_uint32 const key,                           \
      ...);                                           \
                                                      \
  extern kal_bool idp_##name##_configurable(          \
      kal_uint32 const key,                           \
      ...);                                           \
                                                      \
  extern kal_bool idp_##name##_query(                 \
      kal_uint32 const key,                           \
      ...);                                           \
                                                      \
  extern kal_bool idp_##name##_config_and_start(      \
      kal_uint32 const key,                           \
      ...);                                           \
                                                      \
  extern kal_bool idp_##name##_is_busy(               \
      kal_uint32 const key,                           \
      kal_bool * const busy);

enum IdpCallByWhichLevel
{
  IDP_CALL_BY_HISR,
  IDP_CALL_BY_TASK,
  IDP_CALL_BY_LISR
};
typedef enum IdpCallByWhichLevel IdpCallByWhichLevel;

enum IdpCheckReentrantPrevAPI
{
  IDP_CHECK_REENTRANT_PREV_OPEN,
  IDP_CHECK_REENTRANT_PREV_IS_IN_USE,
  IDP_CHECK_REENTRANT_PREV_CLOSE,
  IDP_CHECK_REENTRANT_PREV_STOP,
  IDP_CHECK_REENTRANT_PREV_START_INTER,
  IDP_CHECK_REENTRANT_PREV_START_INPUT,
  IDP_CHECK_REENTRANT_PREV_START_OUTPUT,
  IDP_CHECK_REENTRANT_PREV_START_ALL,
  IDP_CHECK_REENTRANT_PREV_CONFIG,
  IDP_CHECK_REENTRANT_PREV_CONFIGURABLE,
  IDP_CHECK_REENTRANT_PREV_CONFIG_FAST,
  IDP_CHECK_REENTRANT_PREV_QUERY,
  IDP_CHECK_REENTRANT_PREV_CONFIG_AND_START
};
typedef enum IdpCheckReentrantPrevAPI IdpCheckReentrantPrevAPI;

struct IdpCheckReentrant
{
  kal_taskid idp_check_reentrant_prev_who;
  IdpCallByWhichLevel idp_check_reentrant_prev_call_by_which_level;
  IdpCheckReentrantPrevAPI idp_check_reentrant_prev_api;
  kal_bool idp_check_reentrant_flag;
};
typedef struct IdpCheckReentrant IdpCheckReentrant;

enum IdpTracedAPI
{
  IDP_TRACED_API_camera_preview_OPEN,
  IDP_TRACED_API_camera_preview_IS_IN_USE,
  IDP_TRACED_API_camera_preview_CLOSE,
  IDP_TRACED_API_camera_preview_STOP,
  IDP_TRACED_API_camera_preview_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_camera_preview_START_INPUT,
  IDP_TRACED_API_camera_preview_START_OUTPUT,
  IDP_TRACED_API_camera_preview_START_ALL,
  IDP_TRACED_API_camera_preview_IS_BUSY,
  IDP_TRACED_API_camera_preview_CONFIG,
  IDP_TRACED_API_camera_preview_CONFIGURABLE,
  IDP_TRACED_API_camera_preview_CONFIG_FAST,
  IDP_TRACED_API_camera_preview_QUERY,
  IDP_TRACED_API_camera_preview_CONFIG_AND_START,

  IDP_TRACED_API_camera_preview__________CRZ_START_LISR,
  IDP_TRACED_API_camera_preview__________CRZ_DONE_LISR,
  IDP_TRACED_API_camera_preview__________RDMA_LISR,
  IDP_TRACED_API_camera_preview__________IBW1_LISR,
  IDP_TRACED_API_camera_preview__________IBW2_LISR,
  IDP_TRACED_API_camera_preview__________IRT0_LISR,
  IDP_TRACED_API_camera_preview__________IRT1_LISR,
  IDP_TRACED_API_camera_preview__________IRT2_LISR,
  IDP_TRACED_API_camera_preview__________OVL_LISR,
  IDP_TRACED_API_camera_preview__________TIME_OUT_ASSERT,
  IDP_TRACED_API_camera_preview__________PRZ_H_LISR,
  IDP_TRACED_API_camera_preview__________PRZ_V_LISR,
  IDP_TRACED_API_camera_preview________________CRZ_PIXEL_DROPPED_LISR,
  IDP_TRACED_API_camera_preview__________ROTDMA0_DONE_LISR,
  IDP_TRACED_API_camera_preview________________CRZ_LOCK_DROP_FRAME_LISR,
  IDP_TRACED_API_camera_preview__________JPEG_LISR,
  IDP_TRACED_API_camera_preview__________LCD_CB,

  IDP_TRACED_API_camera_preview_with_face_detection_OPEN,
  IDP_TRACED_API_camera_preview_with_face_detection_IS_IN_USE,
  IDP_TRACED_API_camera_preview_with_face_detection_CLOSE,
  IDP_TRACED_API_camera_preview_with_face_detection_STOP,
  IDP_TRACED_API_camera_preview_with_face_detection_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_camera_preview_with_face_detection_START_INPUT,
  IDP_TRACED_API_camera_preview_with_face_detection_START_OUTPUT,
  IDP_TRACED_API_camera_preview_with_face_detection_START_ALL,
  IDP_TRACED_API_camera_preview_with_face_detection_IS_BUSY,
  IDP_TRACED_API_camera_preview_with_face_detection_CONFIG,
  IDP_TRACED_API_camera_preview_with_face_detection_CONFIGURABLE,
  IDP_TRACED_API_camera_preview_with_face_detection_CONFIG_FAST,
  IDP_TRACED_API_camera_preview_with_face_detection_QUERY,
  IDP_TRACED_API_camera_preview_with_face_detection_CONFIG_AND_START,

  IDP_TRACED_API_camera_preview_with_face_detection__________IBW1_DONE_LISR,
  IDP_TRACED_API_camera_preview_with_face_detection__________CRZ_START_LISR,
  IDP_TRACED_API_camera_preview_with_face_detection__________CRZ_DONE_LISR,
  IDP_TRACED_API_camera_preview_with_face_detection__________OVL_LISR,
  IDP_TRACED_API_camera_preview_with_face_detection__________IBW2_LISR,
  IDP_TRACED_API_camera_preview_with_face_detection__________PRZ_H_LISR,
  IDP_TRACED_API_camera_preview_with_face_detection__________PRZ_V_LISR,
  IDP_TRACED_API_camera_preview_with_face_detection__________IRT1_LISR,
  IDP_TRACED_API_camera_preview_with_face_detection__________IRT2_LISR,

  IDP_TRACED_API_camera_capture_to_jpeg_OPEN,
  IDP_TRACED_API_camera_capture_to_jpeg_IS_IN_USE,
  IDP_TRACED_API_camera_capture_to_jpeg_CLOSE,
  IDP_TRACED_API_camera_capture_to_jpeg_STOP,
  IDP_TRACED_API_camera_capture_to_jpeg_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_camera_capture_to_jpeg_START_INPUT,
  IDP_TRACED_API_camera_capture_to_jpeg_START_OUTPUT,
  IDP_TRACED_API_camera_capture_to_jpeg_START_ALL,
  IDP_TRACED_API_camera_capture_to_jpeg_IS_BUSY,
  IDP_TRACED_API_camera_capture_to_jpeg_CONFIG,
  IDP_TRACED_API_camera_capture_to_jpeg_CONFIGURABLE,
  IDP_TRACED_API_camera_capture_to_jpeg_CONFIG_FAST,
  IDP_TRACED_API_camera_capture_to_jpeg_QUERY,
  IDP_TRACED_API_camera_capture_to_jpeg_CONFIG_AND_START,

  IDP_TRACED_API_camera_capture_to_jpeg__________CRZ_DONE_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg__________CRZ_START_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg__________PRZ_H_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg__________PRZ_V_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg__________IBW2_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg__________IRT0_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg__________IRT1_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg__________IBR0_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg__________IBR2_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg__________IRT2_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg__________IBW1_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg__________JPEG_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg__________DRZ_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg__________OVL_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg________________CRZ_PIXEL_DROPPED_LISR,
  IDP_TRACED_API_camera_capture_to_jpeg________________CRZ_LOCK_DROP_FRAME_LISR,

  IDP_TRACED_API_camera_capture_to_mem_OPEN,
  IDP_TRACED_API_camera_capture_to_mem_IS_IN_USE,
  IDP_TRACED_API_camera_capture_to_mem_CLOSE,
  IDP_TRACED_API_camera_capture_to_mem_STOP,
  IDP_TRACED_API_camera_capture_to_mem_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_camera_capture_to_mem_START_INPUT,
  IDP_TRACED_API_camera_capture_to_mem_START_OUTPUT,
  IDP_TRACED_API_camera_capture_to_mem_START_ALL,
  IDP_TRACED_API_camera_capture_to_mem_IS_BUSY,
  IDP_TRACED_API_camera_capture_to_mem_CONFIG,
  IDP_TRACED_API_camera_capture_to_mem_CONFIGURABLE,
  IDP_TRACED_API_camera_capture_to_mem_CONFIG_FAST,
  IDP_TRACED_API_camera_capture_to_mem_QUERY,
  IDP_TRACED_API_camera_capture_to_mem_CONFIG_AND_START,

  IDP_TRACED_API_camera_capture_to_mem__________OVL_LISR,
  IDP_TRACED_API_camera_capture_to_mem__________IBW2_LISR,
  IDP_TRACED_API_camera_capture_to_mem__________IRT0_LISR,
  IDP_TRACED_API_camera_capture_to_mem__________IRT1_LISR,
  IDP_TRACED_API_camera_capture_to_mem__________CRZ_DONE_LISR,
  IDP_TRACED_API_camera_capture_to_mem__________CRZ_START_LISR,
  IDP_TRACED_API_camera_capture_to_mem__________PRZ_H_LISR,
  IDP_TRACED_API_camera_capture_to_mem__________PRZ_V_LISR,

  IDP_TRACED_API_camera_capture_to_barcode_OPEN,
  IDP_TRACED_API_camera_capture_to_barcode_IS_IN_USE,
  IDP_TRACED_API_camera_capture_to_barcode_CLOSE,
  IDP_TRACED_API_camera_capture_to_barcode_STOP,
  IDP_TRACED_API_camera_capture_to_barcode_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_camera_capture_to_barcode_START_INPUT,
  IDP_TRACED_API_camera_capture_to_barcode_START_OUTPUT,
  IDP_TRACED_API_camera_capture_to_barcode_START_ALL,
  IDP_TRACED_API_camera_capture_to_barcode_IS_BUSY,
  IDP_TRACED_API_camera_capture_to_barcode_CONFIG,
  IDP_TRACED_API_camera_capture_to_barcode_CONFIGURABLE,
  IDP_TRACED_API_camera_capture_to_barcode_CONFIG_FAST,
  IDP_TRACED_API_camera_capture_to_barcode_QUERY,
  IDP_TRACED_API_camera_capture_to_barcode_CONFIG_AND_START,

  IDP_TRACED_API_camera_capture_to_barcode__________CRZ_DONE_LISR,
  IDP_TRACED_API_camera_capture_to_barcode__________CRZ_START_LISR,
  IDP_TRACED_API_camera_capture_to_barcode__________OVL_LISR,
  IDP_TRACED_API_camera_capture_to_barcode__________IBW2_LISR,
  IDP_TRACED_API_camera_capture_to_barcode__________IRT1_LISR,
  IDP_TRACED_API_camera_capture_to_barcode__________DRZ_LISR,
  IDP_TRACED_API_camera_capture_to_barcode__________PRZ_H_LISR,
  IDP_TRACED_API_camera_capture_to_barcode__________PRZ_V_LISR,

  IDP_TRACED_API_camera_capture_to_ybuffer_OPEN,
  IDP_TRACED_API_camera_capture_to_ybuffer_IS_IN_USE,
  IDP_TRACED_API_camera_capture_to_ybuffer_CLOSE,
  IDP_TRACED_API_camera_capture_to_ybuffer_STOP,
  IDP_TRACED_API_camera_capture_to_ybuffer_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_camera_capture_to_ybuffer_START_INPUT,
  IDP_TRACED_API_camera_capture_to_ybuffer_START_OUTPUT,
  IDP_TRACED_API_camera_capture_to_ybuffer_START_ALL,
  IDP_TRACED_API_camera_capture_to_ybuffer_IS_BUSY,
  IDP_TRACED_API_camera_capture_to_ybuffer_CONFIG,
  IDP_TRACED_API_camera_capture_to_ybuffer_CONFIGURABLE,
  IDP_TRACED_API_camera_capture_to_ybuffer_CONFIG_FAST,
  IDP_TRACED_API_camera_capture_to_ybuffer_QUERY,
  IDP_TRACED_API_camera_capture_to_ybuffer_CONFIG_AND_START,

  IDP_TRACED_API_camera_capture_to_ybuffer__________PRZ_H_LISR,
  IDP_TRACED_API_camera_capture_to_ybuffer__________PRZ_V_LISR,
  IDP_TRACED_API_camera_capture_to_ybuffer__________IBW2_LISR,
  IDP_TRACED_API_camera_capture_to_ybuffer__________IRT0_LISR,
  IDP_TRACED_API_camera_capture_to_ybuffer__________OVL_LISR,
  IDP_TRACED_API_camera_capture_to_ybuffer__________CRZ_DONE_LISR,
  IDP_TRACED_API_camera_capture_to_ybuffer__________CRZ_START_LISR,
  IDP_TRACED_API_camera_capture_to_ybuffer__________IRT1_LISR,
  IDP_TRACED_API_camera_capture_to_ybuffer__________IBR2_LISR,
  IDP_TRACED_API_camera_capture_to_ybuffer__________IRT2_LISR,
  IDP_TRACED_API_camera_capture_to_ybuffer__________VDOENC_R_DONE_LISR,
  IDP_TRACED_API_camera_capture_to_ybuffer__________VDOENC_W_DONE_LISR,

  IDP_TRACED_API_jpeg_decode_OPEN,
  IDP_TRACED_API_jpeg_decode_IS_IN_USE,
  IDP_TRACED_API_jpeg_decode_CLOSE,
  IDP_TRACED_API_jpeg_decode_STOP,
  IDP_TRACED_API_jpeg_decode_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_jpeg_decode_START_INPUT,
  IDP_TRACED_API_jpeg_decode_START_OUTPUT,
  IDP_TRACED_API_jpeg_decode_START_ALL,
  IDP_TRACED_API_jpeg_decode_IS_BUSY,
  IDP_TRACED_API_jpeg_decode_CONFIG,
  IDP_TRACED_API_jpeg_decode_CONFIGURABLE,
  IDP_TRACED_API_jpeg_decode_CONFIG_FAST,
  IDP_TRACED_API_jpeg_decode_QUERY,
  IDP_TRACED_API_jpeg_decode_CONFIG_AND_START,

  IDP_TRACED_API_jpeg_decode__________IBW2_LISR,
  IDP_TRACED_API_jpeg_decode__________CRZ_DONE_LISR,
  IDP_TRACED_API_jpeg_decode__________CRZ_START_LISR,
  IDP_TRACED_API_jpeg_decode__________PRZ_H_LISR,
  IDP_TRACED_API_jpeg_decode__________PRZ_V_LISR,
  IDP_TRACED_API_jpeg_decode__________IRT1_LISR,
  IDP_TRACED_API_jpeg_decode__________OVL_LISR,
  IDP_TRACED_API_jpeg_decode__________BRZ_DONE_LISR,

  IDP_TRACED_API_jpeg_encode_OPEN,
  IDP_TRACED_API_jpeg_encode_IS_IN_USE,
  IDP_TRACED_API_jpeg_encode_CLOSE,
  IDP_TRACED_API_jpeg_encode_STOP,
  IDP_TRACED_API_jpeg_encode_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_jpeg_encode_START_INPUT,
  IDP_TRACED_API_jpeg_encode_START_OUTPUT,
  IDP_TRACED_API_jpeg_encode_START_ALL,
  IDP_TRACED_API_jpeg_encode_IS_BUSY,
  IDP_TRACED_API_jpeg_encode_CONFIG,
  IDP_TRACED_API_jpeg_encode_CONFIGURABLE,
  IDP_TRACED_API_jpeg_encode_CONFIG_FAST,
  IDP_TRACED_API_jpeg_encode_QUERY,
  IDP_TRACED_API_jpeg_encode_CONFIG_AND_START,

  IDP_TRACED_API_jpeg_encode__________OVL_LISR,
  IDP_TRACED_API_jpeg_encode__________IBR0_LISR,
  IDP_TRACED_API_jpeg_encode__________IBR1_LISR,
  IDP_TRACED_API_jpeg_encode__________IBW1_LISR,
  IDP_TRACED_API_jpeg_encode__________JPEG_LISR,
  IDP_TRACED_API_jpeg_encode__________DRZ_LISR,
  IDP_TRACED_API_jpeg_encode__________PRZ_H_LISR,
  IDP_TRACED_API_jpeg_encode__________PRZ_V_LISR,
  IDP_TRACED_API_jpeg_encode__________IBW2_LISR,
  IDP_TRACED_API_jpeg_encode__________CRZ_START_LISR,
  IDP_TRACED_API_jpeg_encode__________CRZ_DONE_LISR,
  IDP_TRACED_API_jpeg_encode__________IRT1_LISR,

  IDP_TRACED_API_jpeg_resize_OPEN,
  IDP_TRACED_API_jpeg_resize_IS_IN_USE,
  IDP_TRACED_API_jpeg_resize_CLOSE,
  IDP_TRACED_API_jpeg_resize_STOP,
  IDP_TRACED_API_jpeg_resize_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_jpeg_resize_START_INPUT,
  IDP_TRACED_API_jpeg_resize_START_OUTPUT,
  IDP_TRACED_API_jpeg_resize_START_ALL,
  IDP_TRACED_API_jpeg_resize_IS_BUSY,
  IDP_TRACED_API_jpeg_resize_CONFIG,
  IDP_TRACED_API_jpeg_resize_CONFIGURABLE,
  IDP_TRACED_API_jpeg_resize_CONFIG_FAST,
  IDP_TRACED_API_jpeg_resize_QUERY,
  IDP_TRACED_API_jpeg_resize_CONFIG_AND_START,

  IDP_TRACED_API_jpeg_resize__________OVL_LISR,
  IDP_TRACED_API_jpeg_resize__________IBW1_LISR,
  IDP_TRACED_API_jpeg_resize__________JPEG_LISR,
  IDP_TRACED_API_jpeg_resize__________CRZ_DONE_LISR,
  IDP_TRACED_API_jpeg_resize__________CRZ_START_LISR,
  IDP_TRACED_API_jpeg_resize__________DRZ_LISR,
  IDP_TRACED_API_jpeg_resize__________PRZ_H_LISR,
  IDP_TRACED_API_jpeg_resize__________PRZ_V_LISR,

  IDP_TRACED_API_image_resize_OPEN,
  IDP_TRACED_API_image_resize_IS_IN_USE,
  IDP_TRACED_API_image_resize_CLOSE,
  IDP_TRACED_API_image_resize_STOP,
  IDP_TRACED_API_image_resize_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_image_resize_START_INPUT,
  IDP_TRACED_API_image_resize_START_OUTPUT,
  IDP_TRACED_API_image_resize_START_ALL,
  IDP_TRACED_API_image_resize_IS_BUSY,
  IDP_TRACED_API_image_resize_CONFIG,
  IDP_TRACED_API_image_resize_CONFIGURABLE,
  IDP_TRACED_API_image_resize_CONFIG_FAST,
  IDP_TRACED_API_image_resize_QUERY,
  IDP_TRACED_API_image_resize_CONFIG_AND_START,

  IDP_TRACED_API_image_resize__________IBW2_LISR,
  IDP_TRACED_API_image_resize__________IBR0_LISR,
  IDP_TRACED_API_image_resize__________IBR1_LISR,
  IDP_TRACED_API_image_resize__________IRT1_LISR,
  IDP_TRACED_API_image_resize__________CRZ_DONE_LISR,
  IDP_TRACED_API_image_resize__________CRZ_START_LISR,
  IDP_TRACED_API_image_resize__________OVL_LISR,
  IDP_TRACED_API_image_resize__________ROTDMA0_DONE_LISR,

  IDP_TRACED_API_rgb2yuv_OPEN,
  IDP_TRACED_API_rgb2yuv_IS_IN_USE,
  IDP_TRACED_API_rgb2yuv_CLOSE,
  IDP_TRACED_API_rgb2yuv_STOP,
  IDP_TRACED_API_rgb2yuv_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_rgb2yuv_START_INPUT,
  IDP_TRACED_API_rgb2yuv_START_OUTPUT,
  IDP_TRACED_API_rgb2yuv_START_ALL,
  IDP_TRACED_API_rgb2yuv_IS_BUSY,
  IDP_TRACED_API_rgb2yuv_CONFIG,
  IDP_TRACED_API_rgb2yuv_CONFIGURABLE,
  IDP_TRACED_API_rgb2yuv_CONFIG_FAST,
  IDP_TRACED_API_rgb2yuv_QUERY,
  IDP_TRACED_API_rgb2yuv_CONFIG_AND_START,

  IDP_TRACED_API_rgb2yuv__________OVL_LISR,
  IDP_TRACED_API_rgb2yuv__________IBR1_LISR,
  IDP_TRACED_API_rgb2yuv__________IRT1_LISR,
  IDP_TRACED_API_rgb2yuv__________PRZ_H_LISR,
  IDP_TRACED_API_rgb2yuv__________PRZ_V_LISR,
  IDP_TRACED_API_rgb2yuv__________CRZ_FRAME_START_LISR,
  IDP_TRACED_API_rgb2yuv__________CRZ_FRAME_DONE_LISR,

  IDP_TRACED_API_video_editor_decode_OPEN,
  IDP_TRACED_API_video_editor_decode_IS_IN_USE,
  IDP_TRACED_API_video_editor_decode_CLOSE,
  IDP_TRACED_API_video_editor_decode_STOP,
  IDP_TRACED_API_video_editor_decode_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_video_editor_decode_START_INPUT,
  IDP_TRACED_API_video_editor_decode_START_OUTPUT,
  IDP_TRACED_API_video_editor_decode_START_ALL,
  IDP_TRACED_API_video_editor_decode_IS_BUSY,
  IDP_TRACED_API_video_editor_decode_CONFIG,
  IDP_TRACED_API_video_editor_decode_CONFIGURABLE,
  IDP_TRACED_API_video_editor_decode_CONFIG_FAST,
  IDP_TRACED_API_video_editor_decode_QUERY,
  IDP_TRACED_API_video_editor_decode_CONFIG_AND_START,

  IDP_TRACED_API_video_editor_decode__________IRT1_LISR,
  IDP_TRACED_API_video_editor_decode__________IBW2_LISR,
  IDP_TRACED_API_video_editor_decode__________IRT0_LISR,
  IDP_TRACED_API_video_editor_decode__________VIDEO_DECODE_LISR,
  IDP_TRACED_API_video_editor_decode__________PRZ_H_LISR,
  IDP_TRACED_API_video_editor_decode__________PRZ_V_LISR,

  IDP_TRACED_API_video_editor_encode_OPEN,
  IDP_TRACED_API_video_editor_encode_IS_IN_USE,
  IDP_TRACED_API_video_editor_encode_CLOSE,
  IDP_TRACED_API_video_editor_encode_STOP,
  IDP_TRACED_API_video_editor_encode_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_video_editor_encode_START_INPUT,
  IDP_TRACED_API_video_editor_encode_START_OUTPUT,
  IDP_TRACED_API_video_editor_encode_START_ALL,
  IDP_TRACED_API_video_editor_encode_IS_BUSY,
  IDP_TRACED_API_video_editor_encode_CONFIG,
  IDP_TRACED_API_video_editor_encode_CONFIGURABLE,
  IDP_TRACED_API_video_editor_encode_CONFIG_FAST,
  IDP_TRACED_API_video_editor_encode_QUERY,
  IDP_TRACED_API_video_editor_encode_CONFIG_AND_START,

  IDP_TRACED_API_video_editor_encode__________IBR1_LISR,
  IDP_TRACED_API_video_editor_encode__________VIDEO_ENCODE_R_DONE_LISR,
  IDP_TRACED_API_video_editor_encode__________VIDEO_ENCODE_W_DONE_LISR,
  IDP_TRACED_API_video_editor_encode__________OVL_LISR,
  IDP_TRACED_API_video_editor_encode__________IRT1_LISR,
  IDP_TRACED_API_video_editor_encode__________CRZ_DONE_LISR,
  IDP_TRACED_API_video_editor_encode__________CRZ_START_LISR,

  IDP_TRACED_API_webcam_OPEN,
  IDP_TRACED_API_webcam_IS_IN_USE,
  IDP_TRACED_API_webcam_CLOSE,
  IDP_TRACED_API_webcam_STOP,
  IDP_TRACED_API_webcam_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_webcam_START_INPUT,
  IDP_TRACED_API_webcam_START_OUTPUT,
  IDP_TRACED_API_webcam_START_ALL,
  IDP_TRACED_API_webcam_IS_BUSY,
  IDP_TRACED_API_webcam_CONFIG,
  IDP_TRACED_API_webcam_CONFIGURABLE,
  IDP_TRACED_API_webcam_CONFIG_FAST,
  IDP_TRACED_API_webcam_QUERY,
  IDP_TRACED_API_webcam_CONFIG_AND_START,

  IDP_TRACED_API_webcam__________OVL_LISR,
  IDP_TRACED_API_webcam__________JPEG_LISR,
  IDP_TRACED_API_webcam__________CRZ_DONE_LISR,
  IDP_TRACED_API_webcam__________CRZ_START_LISR,

  IDP_TRACED_API_simple_display_with_rotate_OPEN,
  IDP_TRACED_API_simple_display_with_rotate_IS_IN_USE,
  IDP_TRACED_API_simple_display_with_rotate_CLOSE,
  IDP_TRACED_API_simple_display_with_rotate_STOP,
  IDP_TRACED_API_simple_display_with_rotate_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_simple_display_with_rotate_START_INPUT,
  IDP_TRACED_API_simple_display_with_rotate_START_OUTPUT,
  IDP_TRACED_API_simple_display_with_rotate_START_ALL,
  IDP_TRACED_API_simple_display_with_rotate_IS_BUSY,
  IDP_TRACED_API_simple_display_with_rotate_CONFIG,
  IDP_TRACED_API_simple_display_with_rotate_CONFIGURABLE,
  IDP_TRACED_API_simple_display_with_rotate_CONFIG_FAST,
  IDP_TRACED_API_simple_display_with_rotate_QUERY,
  IDP_TRACED_API_simple_display_with_rotate_CONFIG_AND_START,

  IDP_TRACED_API_simple_display_with_rotate__________IBR2_LISR,
  IDP_TRACED_API_simple_display_with_rotate__________IRT2_LISR,

  IDP_TRACED_API_image_effect_pixel_OPEN,
  IDP_TRACED_API_image_effect_pixel_IS_IN_USE,
  IDP_TRACED_API_image_effect_pixel_CLOSE,
  IDP_TRACED_API_image_effect_pixel_STOP,
  IDP_TRACED_API_image_effect_pixel_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_image_effect_pixel_START_INPUT,
  IDP_TRACED_API_image_effect_pixel_START_OUTPUT,
  IDP_TRACED_API_image_effect_pixel_START_ALL,
  IDP_TRACED_API_image_effect_pixel_IS_BUSY,
  IDP_TRACED_API_image_effect_pixel_CONFIG,
  IDP_TRACED_API_image_effect_pixel_CONFIGURABLE,
  IDP_TRACED_API_image_effect_pixel_CONFIG_FAST,
  IDP_TRACED_API_image_effect_pixel_QUERY,
  IDP_TRACED_API_image_effect_pixel_CONFIG_AND_START,

  IDP_TRACED_API_image_effect_pixel__________IBW0_LISR,
  IDP_TRACED_API_image_effect_pixel__________IBW2_LISR,
  IDP_TRACED_API_image_effect_pixel__________IBW1_LISR,
  IDP_TRACED_API_image_effect_pixel__________IRT0_LISR,
  IDP_TRACED_API_image_effect_pixel__________IRT1_LISR,
  IDP_TRACED_API_image_effect_pixel__________IBR0_LISR,
  IDP_TRACED_API_image_effect_pixel__________IBR1_LISR,
  IDP_TRACED_API_image_effect_pixel__________PRZ_H_LISR,
  IDP_TRACED_API_image_effect_pixel__________PRZ_V_LISR,
  IDP_TRACED_API_image_effect_pixel__________CRZ_DONE_LISR,
  IDP_TRACED_API_image_effect_pixel__________CRZ_START_LISR,
  IDP_TRACED_API_image_effect_pixel__________OVL_LISR,


  IDP_TRACED_API_video_decode_OPEN,
  IDP_TRACED_API_video_decode_IS_IN_USE,
  IDP_TRACED_API_video_decode_CLOSE,
  IDP_TRACED_API_video_decode_STOP,
  IDP_TRACED_API_video_decode_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_video_decode_START_INPUT,
  IDP_TRACED_API_video_decode_START_OUTPUT,
  IDP_TRACED_API_video_decode_START_ALL,
  IDP_TRACED_API_video_decode_IS_BUSY,
  IDP_TRACED_API_video_decode_CONFIG,
  IDP_TRACED_API_video_decode_CONFIGURABLE,
  IDP_TRACED_API_video_decode_CONFIG_FAST,
  IDP_TRACED_API_video_decode_QUERY,
  IDP_TRACED_API_video_decode_CONFIG_AND_START,

  IDP_TRACED_API_video_decode__________IRT1_DONE_LISR,
  IDP_TRACED_API_video_decode__________IBR0_LISR,
  IDP_TRACED_API_video_decode__________IBR2_LISR,
  IDP_TRACED_API_video_decode__________IRT2_LISR,
  IDP_TRACED_API_video_decode__________IBW1_LISR,
  IDP_TRACED_API_video_decode__________IBW2_LISR,
  IDP_TRACED_API_video_decode__________VIDEO_DECODE_LISR,
  IDP_TRACED_API_video_decode__________IRT0_LISR,
  IDP_TRACED_API_video_decode__________PRZ_H_LISR,
  IDP_TRACED_API_video_decode__________PRZ_V_LISR,
  IDP_TRACED_API_video_decode__________CRZ_DONE_LISR,
  IDP_TRACED_API_video_decode__________CRZ_START_LISR,
  IDP_TRACED_API_video_decode__________OVL_LISR,
  IDP_TRACED_API_video_decode__________IRT1_LISR,
  IDP_TRACED_API_video_decode__________VP7_IBW1_BUSY_LCD_NOT,
  IDP_TRACED_API_video_decode__________VP7_IBW1_BUSY_LCD_BUSY,
  IDP_TRACED_API_video_decode__________TIME_OUT_ASSERT,
  IDP_TRACED_API_video_decode__________CRZ_DONE_HISR,
  IDP_TRACED_API_video_decode__________CRZ_PIXEL_DROPPED_LISR,
  IDP_TRACED_API_video_decode__________IRT0_DONE_HISR,
  
  IDP_TRACED_API_video_encode_OPEN,
  IDP_TRACED_API_video_encode_IS_IN_USE,
  IDP_TRACED_API_video_encode_CLOSE,
  IDP_TRACED_API_video_encode_STOP,
  IDP_TRACED_API_video_encode_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_video_encode_START_INPUT,
  IDP_TRACED_API_video_encode_START_OUTPUT,
  IDP_TRACED_API_video_encode_START_ALL,
  IDP_TRACED_API_video_encode_IS_BUSY,
  IDP_TRACED_API_video_encode_CONFIG,
  IDP_TRACED_API_video_encode_CONFIGURABLE,
  IDP_TRACED_API_video_encode_CONFIG_FAST,
  IDP_TRACED_API_video_encode_QUERY,
  IDP_TRACED_API_video_encode_CONFIG_AND_START,

  IDP_TRACED_API_cal_video_encode_close_lcd_1,
  IDP_TRACED_API_cal_video_encode_close_lcd_2,

  IDP_TRACED_API_video_encode__________VDOENC_R_DONE_LISR,
  IDP_TRACED_API_video_encode__________VDOENC_W_DONE_LISR,
  IDP_TRACED_API_video_encode__________VDOENC_W_DONE_FOR_ROTATE_LISR,
  IDP_TRACED_API_video_encode__________IBR0_LISR,
  IDP_TRACED_API_video_encode__________IBR2_LISR,
  IDP_TRACED_API_video_encode__________IRT0_LISR,
  IDP_TRACED_API_video_encode__________IRT0_HISR,
  IDP_TRACED_API_video_encode__________IRT2_LISR,
  IDP_TRACED_API_video_encode__________CRZ_DONE_LISR,
  IDP_TRACED_API_video_encode__________CRZ_START_LISR,
  IDP_TRACED_API_video_encode__________IBW2_LISR,
  IDP_TRACED_API_video_encode__________IBW2_HISR,
  IDP_TRACED_API_video_encode__________IBW2_HISR_GET,
  IDP_TRACED_API_video_encode__________IBW2_HISR_RTRN,
  IDP_TRACED_API_video_encode__________IRT1_LISR,
  IDP_TRACED_API_video_encode__________IRT1_HISR,
  IDP_TRACED_API_video_encode__________PRZ_H_LISR,
  IDP_TRACED_API_video_encode__________PRZ_V_LISR,
  IDP_TRACED_API_video_encode__________IBW1_LISR,
  IDP_TRACED_API_video_encode__________IBW1_HISR,
  IDP_TRACED_API_video_encode__________IBW1_HISR_GET,
  IDP_TRACED_API_video_encode__________IBW1_HISR_RTRN,
  IDP_TRACED_API_video_encode__________TIME_OUT_ASSERT,
  IDP_TRACED_API_video_encode__________OVL_LISR,
  IDP_TRACED_API_video_encode__________CRZ_PASS1_DONE_LISR,
  IDP_TRACED_API_video_encode__________CRZ_PASS2_DONE_LISR,
  IDP_TRACED_API_video_encode________________CRZ_PIXEL_DROPPED_LISR,
  IDP_TRACED_API_video_encode________________CRZ_LOCK_DROP_FRAME_LISR,
  IDP_TRACED_API_video_encode__________ROTDMA_HISR_GET,
  IDP_TRACED_API_video_encode__________ROTDMA_HISR_RETURN,
  IDP_TRACED_API_video_encode__________ROTDMA_HISR_DROP,

#if defined(DRV_IDP_MT6256_E2_SERIES)
  IDP_TRACED_API_mjpeg_encode_OPEN,
  IDP_TRACED_API_mjpeg_encode_IS_IN_USE,
  IDP_TRACED_API_mjpeg_encode_CLOSE,
  IDP_TRACED_API_mjpeg_encode_STOP,
  IDP_TRACED_API_mjpeg_encode_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_mjpeg_encode_START_INPUT,
  IDP_TRACED_API_mjpeg_encode_START_OUTPUT,
  IDP_TRACED_API_mjpeg_encode_START_ALL,
  IDP_TRACED_API_mjpeg_encode_IS_BUSY,
  IDP_TRACED_API_mjpeg_encode_CONFIG,
  IDP_TRACED_API_mjpeg_encode_CONFIGURABLE,
  IDP_TRACED_API_mjpeg_encode_CONFIG_FAST,
  IDP_TRACED_API_mjpeg_encode_QUERY,
  IDP_TRACED_API_mjpeg_encode_CONFIG_AND_START,

  IDP_TRACED_API_mjpeg_encode__________CRZ_START_LISR,
  IDP_TRACED_API_mjpeg_encode__________CRZ_DONE_LISR,
  IDP_TRACED_API_mjpeg_encode__________TIME_OUT_ASSERT,
  IDP_TRACED_API_mjpeg_encode__________OVL_LISR,
  IDP_TRACED_API_mjpeg_encode__________IRT1_LISR,
  IDP_TRACED_API_mjpeg_encode__________JPEG_LISR,
#endif

  IDP_TRACED_API_video_call_encode_OPEN,
  IDP_TRACED_API_video_call_encode_IS_IN_USE,
  IDP_TRACED_API_video_call_encode_CLOSE,
  IDP_TRACED_API_video_call_encode_STOP,
  IDP_TRACED_API_video_call_encode_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_video_call_encode_START_INPUT,
  IDP_TRACED_API_video_call_encode_START_OUTPUT,
  IDP_TRACED_API_video_call_encode_START_ALL,
  IDP_TRACED_API_video_call_encode_IS_BUSY,
  IDP_TRACED_API_video_call_encode_CONFIG,
  IDP_TRACED_API_video_call_encode_CONFIGURABLE,
  IDP_TRACED_API_video_call_encode_CONFIG_FAST,
  IDP_TRACED_API_video_call_encode_QUERY,
  IDP_TRACED_API_video_call_encode_CONFIG_AND_START,

  IDP_TRACED_API_video_call_encode__________IBW1_DONE_LISR,
  IDP_TRACED_API_video_call_encode__________OVL_LISR,
  IDP_TRACED_API_video_call_encode__________VIDEO_ENCODE_W_DONE_LISR,
  IDP_TRACED_API_video_call_encode__________VIDEO_ENCODE_R_DONE_LISR,
  IDP_TRACED_API_video_call_encode__________DRZ_LISR,
  IDP_TRACED_API_video_call_encode__________CRZ_DONE_LISR,
  IDP_TRACED_API_video_call_encode__________CRZ_START_LISR,
  IDP_TRACED_API_video_call_encode__________IBR2_DONE_LISR,
  IDP_TRACED_API_video_call_encode__________IRT0_DONE_LISR,
  IDP_TRACED_API_video_call_encode__________IRT2_DONE_LISR,
  IDP_TRACED_API_video_call_encode__________TIME_OUT_ASSERT,
  IDP_TRACED_API_video_call_encode__________IBW1_MDP_LCD_BUSY,
  IDP_TRACED_API_video_call_encode__________WDMA0_HISR_GET,
  IDP_TRACED_API_video_call_encode__________WDMA0_HISR_RTRN,
  IDP_TRACED_API_video_call_encode__________WDMA0_HISR_DROP,
  IDP_TRACED_API_video_call_encode__________ROT1_DONE_LISR,
  IDP_TRACED_API_video_call_encode__________ROTDMA_HISR_GET,
  IDP_TRACED_API_video_call_encode__________ROTDMA_HISR_RETURN,
  IDP_TRACED_API_video_call_encode__________ROTDMA_HISR_DROP,

  IDP_TRACED_API_video_call_decode_OPEN,
  IDP_TRACED_API_video_call_decode_IS_IN_USE,
  IDP_TRACED_API_video_call_decode_CLOSE,
  IDP_TRACED_API_video_call_decode_STOP,
  IDP_TRACED_API_video_call_decode_START_INTERMEDIA_PIPE,
  IDP_TRACED_API_video_call_decode_START_INPUT,
  IDP_TRACED_API_video_call_decode_START_OUTPUT,
  IDP_TRACED_API_video_call_decode_START_ALL,
  IDP_TRACED_API_video_call_decode_IS_BUSY,
  IDP_TRACED_API_video_call_decode_CONFIG,
  IDP_TRACED_API_video_call_decode_CONFIGURABLE,
  IDP_TRACED_API_video_call_decode_CONFIG_FAST,
  IDP_TRACED_API_video_call_decode_QUERY,
  IDP_TRACED_API_video_call_decode_CONFIG_AND_START,

  IDP_TRACED_API_video_call_decode__________PRZ_H_LISR,
  IDP_TRACED_API_video_call_decode__________PRZ_V_LISR,
  IDP_TRACED_API_video_call_decode__________PRZ_START_LISR,
  IDP_TRACED_API_video_call_decode__________PRZ_DONE_LISR,
  IDP_TRACED_API_video_call_decode__________IBW2_LISR,
  IDP_TRACED_API_video_call_decode__________IRT0_LISR,
  IDP_TRACED_API_video_call_decode__________IRT1_LISR,
  IDP_TRACED_API_video_call_decode__________VIDEO_DECODE_LISR,
  IDP_TRACED_API_video_call_decode__________ALL_DONE_LISR
};
typedef enum IdpTracedAPI IdpTracedAPI;

struct IdpTraced
{
  IdpTracedAPI m_api;
  kal_taskid m_who;
  IdpCallByWhichLevel m_call_by_which_level;
  kal_bool m_done;

  kal_uint32 m_start_time_drv;
  kal_uint32 m_start_time_l1;
  //kal_uint32 m_start_imgdma_status_reg;

  kal_uint32 m_done_time_drv;
  kal_uint32 m_param;
};
typedef struct IdpTraced IdpTraced;

extern IdpTraced g_idp_traced[];
extern kal_int32 g_idp_traced_curr_idx;

extern kal_int32 idp_add_traced_begin(IdpTracedAPI const api);
extern void idp_add_traced_end(IdpTracedAPI const api, kal_int32 const idx);
extern kal_int32 idp_add_traced_lisr(IdpTracedAPI const api);
extern kal_int32 idp_add_traced_custom(IdpTracedAPI const api, kal_uint32 param);

  //extern kal_taskid kal_get_current_thread_ID(void);                   

#define SET_BIT(location, para_type)                              \
  do                                                              \
  {                                                               \
    _idp_set_bit(location,sizeof(location),para_type);            \
  } while (0);

#define CLEAR_BIT(location, para_type)                            \
  do                                                              \
  {                                                               \
    _idp_clear_bit(location,sizeof(location),para_type);          \
  } while (0);

typedef void (*PFN_TRIGGER_VIDEO_CALL_DECODE_PATH)(void*);

extern void _idp_set_bit(kal_uint32* location,kal_uint32 len ,kal_uint32 para_type);                             

extern void _idp_clear_bit(kal_uint32* location,kal_uint32 len ,kal_uint32 para_type);

extern kal_bool IS_BIT_SET(kal_uint32 const * location, kal_uint32 const para_type);

extern void idp_common_check(IdpCheckReentrant* idp_check_reentrant);


extern void idp_init(void);

extern kal_bool idp_hw_open(
    idp_hw_owner_t * const owner);

extern kal_bool idp_hw_close(
    idp_hw_owner_t * const owner,
    kal_uint32 const key);

extern kal_bool idp_hw_can_be_used(
    idp_hw_owner_t * const owner,
    kal_uint32 const key);

extern kal_bool idp_find_empty_and_register(
    kal_uint32 * const key,
    idp_owner_t ** const owner);

extern kal_bool idp_find(
    kal_uint32 const key,
    idp_owner_t ** const owner);

extern kal_bool idp_close(
    kal_uint32 const key,
    idp_close_hook_t const hook);

extern kal_bool idp_stop(
    kal_uint32 const key,
    idp_stop_hook_t const hook);

extern kal_bool idp_open(
    kal_uint32 * const key,
    idp_scenario_t const scenario,
    idp_open_hook_t const open_hook,
    idp_close_hook_t const close_hook,
    idp_init_config_hook_t const init_config_hook);

extern kal_bool
idp_scenario_open(
   kal_uint32 * const key,
   IdpCheckReentrant* idp_check_reentrant,
   IdpTracedAPI const api,
   idp_scenario_t const scenario,
   hook_collect_t* hook_collect);

extern kal_bool
idp_scenario_is_in_use(
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect);

extern kal_bool
idp_scenario_close(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect);

extern kal_bool
idp_scenario_stop(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect);

extern kal_bool
idp_scenario_start_intermedia_pipe(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect);

extern kal_bool
idp_scenario_start_input(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect);

extern kal_bool
idp_scenario_start_output(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect);

extern kal_bool
idp_scenario_start_all(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect);

extern kal_bool
idp_scenario_is_busy(
  kal_uint32 const key,
  kal_bool * const busy,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect);

extern kal_bool
idp_scenario_config_fast(
  kal_uint32 const key,
  kal_uint32 const para_type,
  kal_uint32 const para_value,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect);

extern kal_bool
idp_scenario_config(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect,
  va_list ap);

extern kal_bool
idp_scenario_configurable(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect,
  va_list ap);

extern kal_bool
idp_scenario_query(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect,
  va_list ap);

extern kal_bool
idp_scenario_config_and_start(
  kal_uint32 const key,
  IdpCheckReentrant* idp_check_reentrant,
  IdpTracedAPI const api,
  hook_collect_t* hook_collect,
  va_list ap);

extern kal_bool idp_is_in_use(
    idp_is_in_use_hook_t const hook);

extern kal_bool idp_config_fast(
    kal_uint32 const key,
    idp_read_user_config_fast_hook_t const read_user_config_fast_hook,
    kal_uint32 const para_type,
    kal_uint32 const para_value);

extern kal_bool idp_config(
    kal_uint32 const key,
    idp_read_user_config_hook_t const read_user_config_hook,
    idp_finish_read_user_config_hook_t const finish_read_user_config_hook,
    idp_hardware_setting_have_been_changed_hook_t const hardware_setting_have_been_changed_hook,
    idp_config_hook_t const config_hook,
    idp_clear_setting_diff_hook_t const clear_setting_diff_hook,
    idp_is_setting_diff_is_ok_when_busy_hook_t const is_setting_diff_is_ok_when_busy_hook,
    idp_is_busy_hook_t const is_busy_hook,
    idp_stop_hook_t const stop_hook,
//    idp_stop_for_config_hook_t const stop_for_config_hook,
    va_list ap);

extern kal_bool idp_configurable(
    kal_uint32 const key,
    idp_read_user_config_hook_t const read_user_config_hook,
    idp_finish_read_user_config_hook_t const finish_read_user_config_hook,
    idp_config_hook_t const config_hook,
    va_list ap);

extern kal_bool idp_query(
    kal_uint32 const key,
    idp_query_hook_t const query_hook,
    va_list ap);

extern kal_bool idp_start(
    kal_uint32 const key,
    idp_finish_read_user_config_hook_t const finish_read_user_config_hook,
    idp_start_hook_t const start_hook);

extern kal_bool idp_is_busy(
    kal_uint32 const key,
    kal_bool * const busy,
    idp_is_busy_hook_t const is_busy_hook);

extern void idp_calc_config_attr_bit_pos(
    kal_uint32 const attr_idx,
    kal_uint32 * const word_pos,
    kal_uint32 * const bit_pos);

extern kal_bool idp_finish_read_user_config(
    kal_uint32 const last_attr_idx,
    kal_uint32 const * const read_user_config);

extern kal_bool idp_cal_lcd_is_busy(void);

#if defined(THIS_FILE_BELONGS_TO_IMAGE_DATA_PATH_MODULE_INTERNAL)
#if defined(DRV_IDP_MT6236_SERIES) || defined(DRV_IDP_6238_SERIES) || defined(DRV_IDP_6235_SERIES) || defined(DRV_IDP_MT6276E2_SERIES) || defined(DRV_IDP_MT6256_E2_SERIES)  || defined(DRV_IDP_MT6255_SERIES)
#if defined(__MTK_TARGET__)
#include "custom_idp.h" // add for customization for CRZ USEL/DSEL
#include "drv_trc.h"

extern void idp_internal_crz_usel_dsel(
    kal_uint32 scenario,
    kal_uint32 source_w,
    kal_uint32 source_h,
    kal_uint32 target_w,
    kal_uint32 target_h,
    kal_uint32* usel,
    kal_uint32* dsel);

#endif  // #if defined(__MTK_TARGET__)
#endif  // #if defined(DRV_IDP_MT6236_SERIES) || defined(DRV_IDP_6238_SERIES) 
#endif  // #if defined(THIS_FILE_BELONGS_TO_IMAGE_DATA_PATH_MODULE_INTERNAL)

#endif //#if defined(DRV_IDP_SUPPORT)
#endif

/// @}
