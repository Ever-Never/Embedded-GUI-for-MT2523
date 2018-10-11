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

/****************************************************************************
 * macros and options
 ****************************************************************************/
#define THIS_FILE_BELONGS_TO_IMAGE_DATA_PATH_MODULE_INTERNAL

/****************************************************************************
 * include directives
 ****************************************************************************/
#include "drv_comm.h"
#include <idp_define.h>

#if defined(DRV_IDP_6252_SERIES)

#include <idp_core.h>

#include <idp_camera_capture_to_jpeg.h>
#include <mt6252/idp_engines.h>
#include <idp_cam_if.h>

#if defined(IDP_HISR_SUPPORT)
#include <mt6252/idp_hisr.h>
#endif
#include <mt6252/idp_scen_common_mt6252.h>

IDP_COMMON_API_DECLARATION(camera_capture_to_jpeg)

/****************************************************************************
 * local variables
 ****************************************************************************/
static struct
{
    /* CRZ parameters */
    kal_uint16                image_src_width; /* image width that come from ISP */
    kal_uint16                image_src_height; /* image width that come from ISP */
    kal_uint16                image_capture_width; /* captured picture width */
    kal_uint16                image_capture_height; /* captured picture height */
    kal_uint32                image_capture_y_buffer;  // for MT6253/58/35
    kal_uint32                image_capture_u_buffer;  // for MT6253/58/35
    kal_uint32                image_capture_v_buffer;  // for MT6253/58/35

    /* Fullsize JPEG image parameters. */
    kal_uint8                 jpeg_yuv_mode;

    /** crop param ***********/
    kal_bool                  enable_crop; /* KAL_TRUE or KAL_FALSE to enable or disable overlay function */
    kal_uint16                crop_width;
    kal_uint16                crop_height;

    #if defined(IDP_TILE_MODE_SUPPORT)
    tile_calc_para            tile_para;
    #endif

    /** crop param END***********/
} config_ic;

#if defined(IDP_TILE_MODE_SUPPORT)
static kal_uint32 idp_scen_ic_sm_y_offset;
static kal_uint32 idp_scen_ic_sm_uv_offset;
#endif

static kal_uint32 idp_capture_to_jpeg_pass_1_tar_width = 0;
static kal_uint32 idp_capture_to_jpeg_pass_1_tar_height = 0;

enum idp_capture_to_jpeg_pass_state_enum
{
    IDP_IC_1PASS_1_STATE,
    IDP_IC_2PASS_1_STATE,
    IDP_IC_2PASS_2_STATE,
    IDP_IC_3PASS_1_STATE,
    IDP_IC_3PASS_2_STATE,
    IDP_IC_3PASS_3_STATE,
    IDP_IC_4PASS_1_STATE,
    IDP_IC_4PASS_2_STATE,
    IDP_IC_4PASS_3_STATE,
    IDP_IC_4PASS_4_STATE
};
typedef enum idp_capture_to_jpeg_pass_state_enum idp_capture_to_jpeg_pass_state_t;

static idp_capture_to_jpeg_pass_state_t idp_capture_to_jpeg_current_running_pass = IDP_IC_1PASS_1_STATE;

#if defined(IDP_HISR_SUPPORT)
static idp_hisr_handle_t idp_capture_to_jpeg_frame_done_hisr_handle;
#endif

/****************************************************************************
 * local function bodies -- LISR
 ****************************************************************************/
#if defined(__MTK_TARGET__)

static void
_idp_scen_ic_jpeg__lisr_crz__frame_start(void *user_data)
{
    idp_add_traced_lisr(IDP_TRACED_API_camera_capture_to_jpeg__________CRZ_START_LISR);
}

static void
_idp_scen_ic_jpeg__lisr_rotdma0__frame_done(void *user_data)
{
    idp_add_traced_lisr(IDP_TRACED_API_camera_capture_to_jpeg__________IRT0_LISR);

#if defined(IDP_HISR_SUPPORT)
    idp_hisr_activate(idp_capture_to_jpeg_frame_done_hisr_handle);
#endif
}

static void
_idp_scen_ic_jpeg__lisr_crz__pixel_dropped(void *user_data)
{
    idp_add_traced_lisr(IDP_TRACED_API_camera_capture_to_jpeg________________CRZ_PIXEL_DROPPED_LISR);
    ASSERT(0);
}

static void
_idp_scen_ic_jpeg__lisr_crz__lock_drop_frame(void *user_data)
{
    idp_add_traced_lisr(IDP_TRACED_API_camera_capture_to_jpeg________________CRZ_LOCK_DROP_FRAME_LISR);
    ASSERT(0);
}

#endif  // #if defined(__MTK_TARGET__)

/****************************************************************************
 * local function bodies -- HISR
 ****************************************************************************/
static kal_bool
idp_scen_ic_jpeg__2nd_pass_config_crz(void)
{
#if defined(__MTK_TARGET__)
    kal_bool result;

    crz_struct.two_pass_resize_en = KAL_FALSE;

    crz_struct.input_src1 = IDP_MODULE_IMGDMA_RDMA0;
    crz_struct.mem_in_color_fmt1 = MM_IMAGE_COLOR_FORMAT_YUV420;

    crz_struct.src_buff_y_addr = REG_IMGDMA_ROT_DMA_Y_DST_STR_ADDR;
    crz_struct.src_buff_u_addr = REG_IMGDMA_ROT_DMA_U_DST_STR_ADDR;
    crz_struct.src_buff_v_addr = REG_IMGDMA_ROT_DMA_V_DST_STR_ADDR;

    crz_struct.hw_frame_sync = KAL_FALSE;

    crz_struct.src_width = idp_capture_to_jpeg_pass_1_tar_width;      //before source clipped w if clipping enable
    crz_struct.src_height = idp_capture_to_jpeg_pass_1_tar_height;    //before source clipped h if clipping enable
    crz_struct.tar_width = config_ic.image_capture_width;
    crz_struct.tar_height = config_ic.image_capture_height;

    crz_struct.crop_en1 = KAL_FALSE;

    result =  idp_resz_crz_config_real(&crz_struct, KAL_TRUE);
    ASSERT(KAL_TRUE == result);
#endif

    return KAL_TRUE;
}


static kal_bool
idp_scen_ic_jpeg__2nd_pass_config_rotdma0(void)
{
#if defined(__MTK_TARGET__)

    kal_bool result;

    rotdma0_struct.y_dest_start_addr = config_ic.image_capture_y_buffer;
    rotdma0_struct.u_dest_start_addr = config_ic.image_capture_u_buffer;
    rotdma0_struct.v_dest_start_addr = config_ic.image_capture_v_buffer;

    rotdma0_struct.src_width = crz_struct.tar_width;
    rotdma0_struct.src_height = crz_struct.tar_height;

    rotdma0_struct.hw_frame_sync = KAL_FALSE;

    rotdma0_struct.protect_en = KAL_FALSE;   //According to spec. From Camera, enable protect

    result =  idp_imgdma_rotdma0_config_real(&rotdma0_struct, KAL_TRUE);
    ASSERT(KAL_TRUE == result);
#endif

    return KAL_TRUE;
}

kal_bool
idp_scen_ic_jpeg__2nd_pass_config_start(void)
{
#if defined(__MTK_TARGET__)
    idp_scen_ic_jpeg__2nd_pass_config_crz();

    idp_scen_ic_jpeg__2nd_pass_config_rotdma0();

    idp_imgdma_rotdma0_start_real(&rotdma0_struct);

    idp_resz_crz_start_real(&crz_struct);

    idp_capture_to_jpeg_current_running_pass = IDP_IC_2PASS_2_STATE;

#endif

    return KAL_TRUE;
}

#if defined(IDP_TILE_MODE_SUPPORT)
//Up scaling use 3 pass, 1st pass => corp & 1x scaling and pitching, 2nd and 3rd pass => tile with source clip and pitching
static kal_bool
idp_scen_ic_jpeg__bi_tile_2nd_pass_config_crz(void)
{
#if defined(__MTK_TARGET__)
    kal_bool result;

    crz_struct.two_pass_resize_en = KAL_FALSE;

    crz_struct.input_src1 = IDP_MODULE_IMGDMA_RDMA0;
    crz_struct.mem_in_color_fmt1 = MM_IMAGE_COLOR_FORMAT_YUV420;

    crz_struct.hw_frame_sync = KAL_FALSE;

    crz_struct.src_width = config_ic.tile_para.tile_src_size_x[0];
    crz_struct.src_height = idp_capture_to_jpeg_pass_1_tar_height;
    crz_struct.tar_width = config_ic.image_capture_width >> 1;
    crz_struct.tar_height = config_ic.image_capture_height;

    crz_struct.crop_en1 = KAL_TRUE;
    crz_struct.crop_left1 = config_ic.tile_para.tile_bi_sa_src_start_pos[0] - config_ic.tile_para.tile_adj_src_start_pos[0];
    crz_struct.crop_right1 = config_ic.tile_para.tile_bi_sa_src_end_pos[0] - config_ic.tile_para.tile_adj_src_start_pos[0];
    crz_struct.crop_top1 = 0;
    crz_struct.crop_bottom1 = idp_capture_to_jpeg_pass_1_tar_height - 1;

    // Source clip color is MM_IMAGE_COLOR_FORMAT_YUV420, y and uv offset need re-cauculate
    crz_struct.clip_en = KAL_TRUE;
    crz_struct.org_width = config_ic.image_capture_width;

    idp_scen_ic_sm_y_offset = ((config_ic.image_capture_height - idp_capture_to_jpeg_pass_1_tar_height) * config_ic.image_capture_width) +
                              (config_ic.image_capture_width - idp_capture_to_jpeg_pass_1_tar_width);
    idp_scen_ic_sm_uv_offset = (((config_ic.image_capture_height - idp_capture_to_jpeg_pass_1_tar_height) * config_ic.image_capture_width) >> 2) + 
                               ((config_ic.image_capture_width - idp_capture_to_jpeg_pass_1_tar_width) >> 1);

    // Config source address
    crz_struct.src_buff_y_addr = config_ic.image_capture_y_buffer + idp_scen_ic_sm_y_offset;
    crz_struct.src_buff_u_addr = config_ic.image_capture_u_buffer + idp_scen_ic_sm_uv_offset;
    crz_struct.src_buff_v_addr = config_ic.image_capture_v_buffer + idp_scen_ic_sm_uv_offset;

    // Tile Setting
    crz_struct.tile_mode = KAL_TRUE;
    crz_struct.sa_en_x = KAL_FALSE;
    crz_struct.sa_en_y = KAL_FALSE;
    crz_struct.tile_start_pos_x = config_ic.tile_para.tile_start_pos_x[0];
    crz_struct.tile_start_pos_y = config_ic.tile_para.tile_start_pos_y[0];
    crz_struct.tile_trunc_err_comp_x = config_ic.tile_para.tile_trunc_err_comp_x[0];
    crz_struct.tile_trunc_err_comp_y = config_ic.tile_para.tile_trunc_err_comp_y[0];
    crz_struct.tile_resid_x = config_ic.tile_para.tile_init_resid_x[0];
    crz_struct.tile_resid_y = config_ic.tile_para.tile_init_resid_y[0];

    result =  idp_resz_crz_config_real(&crz_struct, KAL_TRUE);
    ASSERT(KAL_TRUE == result);
#endif

    return KAL_TRUE;
}


static kal_bool
idp_scen_ic_jpeg__bi_tile_2nd_pass_config_rotdma0(void)
{
#if defined(__MTK_TARGET__)
    kal_bool result;

    rotdma0_struct.y_dest_start_addr = config_ic.image_capture_y_buffer;
    rotdma0_struct.u_dest_start_addr = config_ic.image_capture_u_buffer;
    rotdma0_struct.v_dest_start_addr = config_ic.image_capture_v_buffer;

    rotdma0_struct.src_width = crz_struct.tar_width;
    rotdma0_struct.src_height = crz_struct.tar_height;

    rotdma0_struct.hw_frame_sync = KAL_FALSE;

    rotdma0_struct.protect_en = KAL_FALSE;   //According to spec. From Camera, enable protect

    rotdma0_struct.pitch_enable = KAL_TRUE;
    rotdma0_struct.pitch_bytes = config_ic.image_capture_width; //Final width

    result =  idp_imgdma_rotdma0_config_real(&rotdma0_struct, KAL_TRUE);
    ASSERT(KAL_TRUE == result);
#endif

    return KAL_TRUE;
}

kal_bool
idp_scen_ic_jpeg__bi_tile_2nd_pass_config_start(void)
{
#if defined(__MTK_TARGET__)
    idp_scen_ic_jpeg__bi_tile_2nd_pass_config_crz();

    idp_scen_ic_jpeg__bi_tile_2nd_pass_config_rotdma0();

    idp_imgdma_rotdma0_start_real(&rotdma0_struct);

    idp_resz_crz_start_real(&crz_struct);

    idp_capture_to_jpeg_current_running_pass = IDP_IC_3PASS_2_STATE;
#endif

    return KAL_TRUE;
}

//Up scaling use 3 pass, 1st pass => corp & 1x scaling and pitching, 2nd and 3rd pass => tile with source clip and pitching
static kal_bool
idp_scen_ic_jpeg__bi_tile_3rd_pass_config_crz(void)
{
#if defined(__MTK_TARGET__)
    kal_bool result;

    crz_struct.two_pass_resize_en = KAL_FALSE;

    crz_struct.input_src1 = IDP_MODULE_IMGDMA_RDMA0;
    crz_struct.mem_in_color_fmt1 = MM_IMAGE_COLOR_FORMAT_YUV420;

    crz_struct.hw_frame_sync = KAL_FALSE;

    crz_struct.src_width = config_ic.tile_para.tile_src_size_x[1];
    crz_struct.src_height = idp_capture_to_jpeg_pass_1_tar_height;
    crz_struct.tar_width = config_ic.image_capture_width >> 1;
    crz_struct.tar_height = config_ic.image_capture_height;

    crz_struct.crop_en1 = KAL_TRUE;
    crz_struct.crop_left1 = config_ic.tile_para.tile_bi_sa_src_start_pos[1] - config_ic.tile_para.tile_adj_src_start_pos[1];
    crz_struct.crop_right1 = config_ic.tile_para.tile_bi_sa_src_end_pos[1] - config_ic.tile_para.tile_adj_src_start_pos[1];
    crz_struct.crop_top1 = 0;
    crz_struct.crop_bottom1 = idp_capture_to_jpeg_pass_1_tar_height - 1;

    // Source clip color is MM_IMAGE_COLOR_FORMAT_YUV420, y and uv offset need re-cauculate
    crz_struct.clip_en = KAL_TRUE;
    crz_struct.org_width = config_ic.image_capture_width;

    idp_scen_ic_sm_y_offset = ((config_ic.image_capture_height - idp_capture_to_jpeg_pass_1_tar_height) * config_ic.image_capture_width) +
                              (config_ic.image_capture_width - idp_capture_to_jpeg_pass_1_tar_width) +
                              (idp_capture_to_jpeg_pass_1_tar_width >> 1);
    idp_scen_ic_sm_uv_offset = (((config_ic.image_capture_height - idp_capture_to_jpeg_pass_1_tar_height) * config_ic.image_capture_width) >> 2) + 
                               ((config_ic.image_capture_width - idp_capture_to_jpeg_pass_1_tar_width) >> 1) +
                               (idp_capture_to_jpeg_pass_1_tar_width >> 2);

    // Config source address
    crz_struct.src_buff_y_addr = config_ic.image_capture_y_buffer + idp_scen_ic_sm_y_offset;
    crz_struct.src_buff_u_addr = config_ic.image_capture_u_buffer + idp_scen_ic_sm_uv_offset;
    crz_struct.src_buff_v_addr = config_ic.image_capture_v_buffer + idp_scen_ic_sm_uv_offset;

    // Tile Setting
    crz_struct.tile_mode = KAL_TRUE;
    crz_struct.sa_en_x = KAL_FALSE;
    crz_struct.sa_en_y = KAL_FALSE;
    crz_struct.tile_start_pos_x = config_ic.tile_para.tile_start_pos_x[1];
    crz_struct.tile_start_pos_y = config_ic.tile_para.tile_start_pos_y[1];
    crz_struct.tile_trunc_err_comp_x = config_ic.tile_para.tile_trunc_err_comp_x[1];
    crz_struct.tile_trunc_err_comp_y = config_ic.tile_para.tile_trunc_err_comp_y[1];
    crz_struct.tile_resid_x = config_ic.tile_para.tile_init_resid_x[1];
    crz_struct.tile_resid_y = config_ic.tile_para.tile_init_resid_y[1];

    result =  idp_resz_crz_config_real(&crz_struct, KAL_TRUE);
    ASSERT(KAL_TRUE == result);
#endif

    return KAL_TRUE;
}


static kal_bool
idp_scen_ic_jpeg__bi_tile_3rd_pass_config_rotdma0(void)
{
#if defined(__MTK_TARGET__)
    kal_bool result;

    rotdma0_struct.y_dest_start_addr = config_ic.image_capture_y_buffer + (config_ic.image_capture_width >> 1);
    rotdma0_struct.u_dest_start_addr = config_ic.image_capture_u_buffer + (config_ic.image_capture_width >> 2);
    rotdma0_struct.v_dest_start_addr = config_ic.image_capture_v_buffer + (config_ic.image_capture_width >> 2);

    rotdma0_struct.src_width = crz_struct.tar_width;
    rotdma0_struct.src_height = crz_struct.tar_height;

    rotdma0_struct.hw_frame_sync = KAL_FALSE;

    rotdma0_struct.protect_en = KAL_FALSE;   //According to spec. From Camera, enable protect

    rotdma0_struct.pitch_enable = KAL_TRUE;
    rotdma0_struct.pitch_bytes = config_ic.image_capture_width; //Final width

    result =  idp_imgdma_rotdma0_config_real(&rotdma0_struct, KAL_TRUE);
    ASSERT(KAL_TRUE == result);
#endif

    return KAL_TRUE;
}

kal_bool
idp_scen_ic_jpeg__bi_tile_3rd_pass_config_start(void)
{
#if defined(__MTK_TARGET__)
    idp_scen_ic_jpeg__bi_tile_3rd_pass_config_crz();

    idp_scen_ic_jpeg__bi_tile_3rd_pass_config_rotdma0();

    idp_imgdma_rotdma0_start_real(&rotdma0_struct);

    idp_resz_crz_start_real(&crz_struct);

    idp_capture_to_jpeg_current_running_pass = IDP_IC_3PASS_3_STATE;
#endif

    return KAL_TRUE;
}

//Down scaling use 4 pass, 1st pass => corp & 1x scaling, 2nd and 3rd pass => tile and pitching, 4th pass -> source clip
static kal_bool
idp_scen_ic_jpeg__sa_tile_2nd_pass_config_crz(void)
{
#if defined(__MTK_TARGET__)
    kal_bool result;

    crz_struct.two_pass_resize_en = KAL_FALSE;

    crz_struct.input_src1 = IDP_MODULE_IMGDMA_RDMA0;
    crz_struct.mem_in_color_fmt1 = MM_IMAGE_COLOR_FORMAT_YUV420;

    crz_struct.hw_frame_sync = KAL_FALSE;

    crz_struct.src_width = config_ic.tile_para.tile_src_size_x[0];
    crz_struct.src_height = idp_capture_to_jpeg_pass_1_tar_height;
    crz_struct.tar_width = config_ic.image_capture_width >> 1;
    crz_struct.tar_height = config_ic.image_capture_height;

    crz_struct.crop_en1 = KAL_TRUE;
    crz_struct.crop_left1 = config_ic.tile_para.tile_bi_sa_src_start_pos[0] - config_ic.tile_para.tile_adj_src_start_pos[0];
    crz_struct.crop_right1 = config_ic.tile_para.tile_bi_sa_src_end_pos[0] - config_ic.tile_para.tile_adj_src_start_pos[0];
    crz_struct.crop_top1 = 0;
    crz_struct.crop_bottom1 = idp_capture_to_jpeg_pass_1_tar_height - 1;

    // Source clip color is MM_IMAGE_COLOR_FORMAT_YUV420, y and uv offset need re-cauculate
    crz_struct.clip_en = KAL_TRUE;
    crz_struct.org_width = idp_capture_to_jpeg_pass_1_tar_width;

    idp_scen_ic_sm_y_offset = config_ic.tile_para.tile_adj_src_start_pos[0];  // + offset_x
    idp_scen_ic_sm_uv_offset = (idp_scen_ic_sm_y_offset >> 1);      // + offset_x/2

    // Config source address
    crz_struct.src_buff_y_addr = config_ic.image_capture_y_buffer + idp_scen_ic_sm_y_offset;
    crz_struct.src_buff_u_addr = config_ic.image_capture_u_buffer + idp_scen_ic_sm_uv_offset;
    crz_struct.src_buff_v_addr = config_ic.image_capture_v_buffer + idp_scen_ic_sm_uv_offset;

    // Tile Setting
    crz_struct.tile_mode = KAL_TRUE;
    crz_struct.sa_en_x = KAL_TRUE;
    crz_struct.sa_en_y = KAL_TRUE;
    crz_struct.tile_start_pos_x = config_ic.tile_para.tile_start_pos_x[0];
    crz_struct.tile_start_pos_y = config_ic.tile_para.tile_start_pos_y[0];
    crz_struct.tile_trunc_err_comp_x = config_ic.tile_para.tile_trunc_err_comp_x[0];
    crz_struct.tile_trunc_err_comp_y = config_ic.tile_para.tile_trunc_err_comp_y[0];
    crz_struct.tile_resid_x = config_ic.tile_para.tile_init_resid_x[0];
    crz_struct.tile_resid_y = config_ic.tile_para.tile_init_resid_y[0];

    result =  idp_resz_crz_config_real(&crz_struct, KAL_TRUE);
    ASSERT(KAL_TRUE == result);
#endif

    return KAL_TRUE;
}

static kal_bool
idp_scen_ic_jpeg__sa_tile_2nd_pass_config_rotdma0(void)
{
#if defined(__MTK_TARGET__)
    kal_bool result;

    rotdma0_struct.y_dest_start_addr = config_ic.image_capture_y_buffer;
    rotdma0_struct.u_dest_start_addr = config_ic.image_capture_u_buffer;
    rotdma0_struct.v_dest_start_addr = config_ic.image_capture_v_buffer;

    rotdma0_struct.src_width = crz_struct.tar_width;
    rotdma0_struct.src_height = crz_struct.tar_height;

    rotdma0_struct.hw_frame_sync = KAL_FALSE;

    rotdma0_struct.protect_en = KAL_FALSE;   //According to spec. From Camera, enable protect

    rotdma0_struct.pitch_enable = KAL_TRUE;
    rotdma0_struct.pitch_bytes = idp_capture_to_jpeg_pass_1_tar_width; //Final width

    result =  idp_imgdma_rotdma0_config_real(&rotdma0_struct, KAL_TRUE);
    ASSERT(KAL_TRUE == result);
#endif

    return KAL_TRUE;
}

kal_bool
idp_scen_ic_jpeg__sa_tile_2nd_pass_config_start(void)
{
#if defined(__MTK_TARGET__)
    idp_scen_ic_jpeg__sa_tile_2nd_pass_config_crz();

    idp_scen_ic_jpeg__sa_tile_2nd_pass_config_rotdma0();

    idp_imgdma_rotdma0_start_real(&rotdma0_struct);

    idp_resz_crz_start_real(&crz_struct);

    idp_capture_to_jpeg_current_running_pass = IDP_IC_4PASS_2_STATE;
#endif

    return KAL_TRUE;
}

//Down scaling use 4 pass, 1st pass => corp & 1x scaling, 2nd and 3rd pass => tile and pitching, 4th pass -> source clip
static kal_bool
idp_scen_ic_jpeg__sa_tile_3rd_pass_config_crz(void)
{
#if defined(__MTK_TARGET__)
    kal_bool result;

    crz_struct.two_pass_resize_en = KAL_FALSE;

    crz_struct.input_src1 = IDP_MODULE_IMGDMA_RDMA0;
    crz_struct.mem_in_color_fmt1 = MM_IMAGE_COLOR_FORMAT_YUV420;

    crz_struct.hw_frame_sync = KAL_FALSE;

    crz_struct.src_width = config_ic.tile_para.tile_src_size_x[1];
    crz_struct.src_height = idp_capture_to_jpeg_pass_1_tar_height;
    crz_struct.tar_width = config_ic.image_capture_width >> 1;
    crz_struct.tar_height = config_ic.image_capture_height;

    crz_struct.crop_en1 = KAL_TRUE;
    crz_struct.crop_left1 = config_ic.tile_para.tile_bi_sa_src_start_pos[1] - config_ic.tile_para.tile_adj_src_start_pos[1];
    crz_struct.crop_right1 = config_ic.tile_para.tile_bi_sa_src_end_pos[1] - config_ic.tile_para.tile_adj_src_start_pos[1];
    crz_struct.crop_top1 = 0;
    crz_struct.crop_bottom1 = idp_capture_to_jpeg_pass_1_tar_height - 1;

    // Source clip color is MM_IMAGE_COLOR_FORMAT_YUV420, y and uv offset need re-cauculate
    crz_struct.clip_en = KAL_TRUE;
    crz_struct.org_width = idp_capture_to_jpeg_pass_1_tar_width;

    idp_scen_ic_sm_y_offset = config_ic.tile_para.tile_adj_src_start_pos[1];  // + offset_x
    idp_scen_ic_sm_uv_offset = (idp_scen_ic_sm_y_offset >> 1);      // + offset_x/2

    // Config source address
    crz_struct.src_buff_y_addr = config_ic.image_capture_y_buffer + idp_scen_ic_sm_y_offset;
    crz_struct.src_buff_u_addr = config_ic.image_capture_u_buffer + idp_scen_ic_sm_uv_offset;
    crz_struct.src_buff_v_addr = config_ic.image_capture_v_buffer + idp_scen_ic_sm_uv_offset;

    // Tile Setting
    crz_struct.tile_mode = KAL_TRUE;
    crz_struct.sa_en_x = KAL_TRUE;
    crz_struct.sa_en_y = KAL_TRUE;
    crz_struct.tile_start_pos_x = config_ic.tile_para.tile_start_pos_x[1];
    crz_struct.tile_start_pos_y = config_ic.tile_para.tile_start_pos_y[1];
    crz_struct.tile_trunc_err_comp_x = config_ic.tile_para.tile_trunc_err_comp_x[1];
    crz_struct.tile_trunc_err_comp_y = config_ic.tile_para.tile_trunc_err_comp_y[1];
    crz_struct.tile_resid_x = config_ic.tile_para.tile_init_resid_x[1];
    crz_struct.tile_resid_y = config_ic.tile_para.tile_init_resid_y[1];

    result =  idp_resz_crz_config_real(&crz_struct, KAL_TRUE);
    ASSERT(KAL_TRUE == result);
#endif

    return KAL_TRUE;
}

static kal_bool
idp_scen_ic_jpeg__sa_tile_3rd_pass_config_rotdma0(void)
{
#if defined(__MTK_TARGET__)
    kal_bool result;

    rotdma0_struct.y_dest_start_addr = config_ic.image_capture_y_buffer + (config_ic.image_capture_width >> 1);
    rotdma0_struct.u_dest_start_addr = config_ic.image_capture_u_buffer + (config_ic.image_capture_width >> 2);
    rotdma0_struct.v_dest_start_addr = config_ic.image_capture_v_buffer + (config_ic.image_capture_width >> 2);

    rotdma0_struct.src_width = crz_struct.tar_width;
    rotdma0_struct.src_height = crz_struct.tar_height;

    rotdma0_struct.hw_frame_sync = KAL_FALSE;

    rotdma0_struct.protect_en = KAL_FALSE;   //According to spec. From Camera, enable protect

    rotdma0_struct.pitch_enable = KAL_TRUE;
    rotdma0_struct.pitch_bytes = idp_capture_to_jpeg_pass_1_tar_width; //Final width

    result =  idp_imgdma_rotdma0_config_real(&rotdma0_struct, KAL_TRUE);
    ASSERT(KAL_TRUE == result);
#endif

    return KAL_TRUE;
}

kal_bool
idp_scen_ic_jpeg__sa_tile_3rd_pass_config_start(void)
{
#if defined(__MTK_TARGET__)
    idp_scen_ic_jpeg__sa_tile_3rd_pass_config_crz();

    idp_scen_ic_jpeg__sa_tile_3rd_pass_config_rotdma0();

    idp_imgdma_rotdma0_start_real(&rotdma0_struct);

    idp_resz_crz_start_real(&crz_struct);

    idp_capture_to_jpeg_current_running_pass = IDP_IC_4PASS_3_STATE;
#endif

    return KAL_TRUE;
}

//Down scaling use 4 pass, 1st pass => corp & 1x scaling, 2nd and 3rd pass => tile and pitching, 4th pass -> source clip
static kal_bool
idp_scen_ic_jpeg__sa_tile_4td_pass_config_crz(void)
{
#if defined(__MTK_TARGET__)
    kal_bool result;

    crz_struct.two_pass_resize_en = KAL_FALSE;

    crz_struct.input_src1 = IDP_MODULE_IMGDMA_RDMA0;
    crz_struct.mem_in_color_fmt1 = MM_IMAGE_COLOR_FORMAT_YUV420;

    crz_struct.hw_frame_sync = KAL_FALSE;
    crz_struct.crop_en1 = KAL_FALSE;

    // 1:1 output
    crz_struct.src_width = config_ic.image_capture_width;      //before source clipped w if clipping enable
    crz_struct.src_height = config_ic.image_capture_height;    //before source clipped h if clipping enable
    crz_struct.tar_width = config_ic.image_capture_width;
    crz_struct.tar_height = config_ic.image_capture_height;

    // Source clip color is MM_IMAGE_COLOR_FORMAT_YUV420, y and uv offset need re-cauculate
    crz_struct.clip_en = KAL_TRUE;
    crz_struct.org_width = idp_capture_to_jpeg_pass_1_tar_width;

    // Config source address
    crz_struct.src_buff_y_addr = config_ic.image_capture_y_buffer;
    crz_struct.src_buff_u_addr = config_ic.image_capture_u_buffer;
    crz_struct.src_buff_v_addr = config_ic.image_capture_v_buffer;

    // Tile Setting
    crz_struct.tile_mode = KAL_FALSE;

    result =  idp_resz_crz_config_real(&crz_struct, KAL_TRUE);
    ASSERT(KAL_TRUE == result);
#endif

    return KAL_TRUE;
}

static kal_bool
idp_scen_ic_jpeg__sa_tile_4td_pass_config_rotdma0(void)
{
#if defined(__MTK_TARGET__)
    kal_bool result;

    rotdma0_struct.y_dest_start_addr = config_ic.image_capture_y_buffer;
    rotdma0_struct.u_dest_start_addr = config_ic.image_capture_u_buffer;
    rotdma0_struct.v_dest_start_addr = config_ic.image_capture_v_buffer;

    rotdma0_struct.src_width = crz_struct.tar_width;
    rotdma0_struct.src_height = crz_struct.tar_height;

    rotdma0_struct.hw_frame_sync = KAL_FALSE;

    rotdma0_struct.protect_en = KAL_FALSE;   //According to spec. From Camera, enable protect

    rotdma0_struct.pitch_enable = KAL_FALSE;

    result =  idp_imgdma_rotdma0_config_real(&rotdma0_struct, KAL_TRUE);
    ASSERT(KAL_TRUE == result);
#endif

    return KAL_TRUE;
}

kal_bool
idp_scen_ic_jpeg__sa_tile_4th_pass_config_start(void)
{
#if defined(__MTK_TARGET__)
    idp_scen_ic_jpeg__sa_tile_4td_pass_config_crz();

    idp_scen_ic_jpeg__sa_tile_4td_pass_config_rotdma0();

    idp_imgdma_rotdma0_start_real(&rotdma0_struct);

    idp_resz_crz_start_real(&crz_struct);

    idp_capture_to_jpeg_current_running_pass = IDP_IC_4PASS_4_STATE;
#endif

    return KAL_TRUE;
}
#endif

#if defined(IDP_HISR_SUPPORT)
static void
_idp_scen_ic_jpeg_image_process_callback(void* param)
{
    CAL_BUFFER_CB_STRUCT rCPFrmDoneCBArg;
    kal_uint32 y_plane_size = config_ic.image_capture_width * config_ic.image_capture_height;

    rCPFrmDoneCBArg.ImageBuffFormat = (MM_IMAGE_FORMAT_ENUM) rotdma0_struct.yuv_color_fmt;
    rCPFrmDoneCBArg.ImageRotateAngle = (MM_IMAGE_ROTATE_ENUM) rotdma0_struct.rot_angle;
    rCPFrmDoneCBArg.ImageBuffYAddr = config_ic.image_capture_y_buffer;
    rCPFrmDoneCBArg.ImageBuffYSize = y_plane_size;
    rCPFrmDoneCBArg.ImageBuffUAddr = config_ic.image_capture_u_buffer;
    rCPFrmDoneCBArg.ImageBuffUSize = y_plane_size >> 2;
    rCPFrmDoneCBArg.ImageBuffVAddr = config_ic.image_capture_v_buffer;
    rCPFrmDoneCBArg.ImageBuffVSize = y_plane_size >> 2;

    pfIDP_CAL_CB(IDPCAL_CBID_CAM_CAPTURE_MAIN_IMAGE_DONE, &rCPFrmDoneCBArg, sizeof(IDPCAL_CB_ARG_FRAME_DONE));
}

static void
_idp_scen_ic_jpeg_pp()
{
    Camera_Capture_Crop_Buffer_Struct rIC2CropBufFrmDoneCBArg;
    rIC2CropBufFrmDoneCBArg.ErrorStatus = MM_ERROR_NONE;

    // config captured buffer settings: image buffer format/address/width/height/callback function
    rIC2CropBufFrmDoneCBArg.ImageBuffFormat = (MM_IMAGE_FORMAT_ENUM) rotdma0_struct.yuv_color_fmt;
    rIC2CropBufFrmDoneCBArg.ImageBuffYAddr = config_ic.image_capture_y_buffer;
    rIC2CropBufFrmDoneCBArg.ImageBuffUAddr = config_ic.image_capture_u_buffer;
    rIC2CropBufFrmDoneCBArg.ImageBuffVAddr = config_ic.image_capture_v_buffer;
    rIC2CropBufFrmDoneCBArg.ImageBuffWidth = config_ic.image_capture_width;
    rIC2CropBufFrmDoneCBArg.ImageBuffHeight = config_ic.image_capture_height;
    rIC2CropBufFrmDoneCBArg.CropBufferCbfn = (pCropBufferCallback_t)_idp_scen_ic_jpeg_image_process_callback;

    // callback to CAL for image processing
    pfIDP_CAL_CB(IDPCAL_CBID_CAM_CAPTURE_CROP_SIZE_IMAGE_DONE, &rIC2CropBufFrmDoneCBArg, sizeof(Camera_Capture_Crop_Buffer_Struct));
}

static kal_bool
_idp_scen_ic_jpeg__hisr_rotdma0__frame_done(void *user_data)
{
    // [camera_capture_2_passes]
    if (IDP_IC_2PASS_1_STATE == idp_capture_to_jpeg_current_running_pass)
    {
        idp_scen_ic_jpeg__2nd_pass_config_start();
    }
#if defined(IDP_TILE_MODE_SUPPORT)
    else if (IDP_IC_3PASS_1_STATE == idp_capture_to_jpeg_current_running_pass)
    {
        idp_scen_ic_jpeg__bi_tile_2nd_pass_config_start();
    }
    else if (IDP_IC_3PASS_2_STATE == idp_capture_to_jpeg_current_running_pass)
    {
        idp_scen_ic_jpeg__bi_tile_3rd_pass_config_start();
    }
    else if (IDP_IC_4PASS_1_STATE == idp_capture_to_jpeg_current_running_pass)
    {
        idp_scen_ic_jpeg__sa_tile_2nd_pass_config_start();
    }
    else if (IDP_IC_4PASS_2_STATE == idp_capture_to_jpeg_current_running_pass)
    {
        idp_scen_ic_jpeg__sa_tile_3rd_pass_config_start();
    }
    else if (IDP_IC_4PASS_3_STATE == idp_capture_to_jpeg_current_running_pass)
    {
        idp_scen_ic_jpeg__sa_tile_4th_pass_config_start();
    }
#endif
    else if ((IDP_IC_1PASS_1_STATE == idp_capture_to_jpeg_current_running_pass) ||
             (IDP_IC_2PASS_2_STATE == idp_capture_to_jpeg_current_running_pass) ||
             (IDP_IC_3PASS_3_STATE == idp_capture_to_jpeg_current_running_pass) ||  // MT6260
             (IDP_IC_4PASS_4_STATE == idp_capture_to_jpeg_current_running_pass))    // MT6260
    {
        _idp_scen_ic_jpeg_pp();
    }
    else
    {
        ASSERT(0);
    }

    return KAL_TRUE;
}

#endif  // #if defined(IDP_HISR_SUPPORT)

/****************************************************************************
 * local function bodies -- HW config functions
 ****************************************************************************/
#if defined(__MTK_TARGET__)
static kal_bool
_idp_scen_ic_jpeg__config_crz(
    idp_owner_t * const owner,
    kal_bool const config_to_hardware)
{
    kal_bool result;
    kal_uint32 src_size, tar_size;
    kal_uint32 src_w, src_h;

    #if defined(IDP_TILE_MODE_SUPPORT)
    // Tile mode BI first pass output pitch width have 8x limitation
    if ((config_ic.image_capture_width > CRZ_LIMIT_WT) && (config_ic.image_capture_width > config_ic.crop_width))
    {
        config_ic.crop_width &= 0xFFF8;
    }
  #endif

    crz_struct.input_src1 = IDP_MODULE_OUTER_ISP;

    crz_struct.fstart1_intr_en = KAL_TRUE;
    crz_struct.fstart1_intr_cb = _idp_scen_ic_jpeg__lisr_crz__frame_start;

    crz_struct.pixel_dropped_intr_en = KAL_TRUE;
    crz_struct.pixel_dropped_intr_cb = _idp_scen_ic_jpeg__lisr_crz__pixel_dropped;

    crz_struct.lock_drop_frame_intr_en = KAL_TRUE;
    crz_struct.lock_drop_frame_intr_cb = _idp_scen_ic_jpeg__lisr_crz__lock_drop_frame;

    crz_struct.hw_frame_sync = KAL_TRUE;

    crz_struct.src_width = config_ic.image_src_width;      //before source clipped w if clipping enable
    crz_struct.src_height = config_ic.image_src_height;   //before source clipped h if clipping enable

    /******* decide target w & h ***************************************/
    tar_size = config_ic.image_capture_width*config_ic.image_capture_height;
    //if (KAL_TRUE == config_ic.enable_crop) // Always TRUE, code shrink
    {
        src_w = config_ic.crop_width;
        src_h = config_ic.crop_height;
    }
    //else  //disable crop
    //{
    //    src_w = config_ic.image_src_width;
    //    src_h = config_ic.image_src_height;
    //}
    src_size = src_w * src_h; //use original width & height

  #if defined(IDP_TILE_MODE_SUPPORT)
    crz_struct.tile_org_frame_width = config_ic.crop_width;
    crz_struct.tile_org_tar_width = config_ic.image_capture_width;
  #endif

    //Down scaling use 1 pass
    //Because 1st pass output use same buffer as 2nd pass output, should use size as criterion
    if (src_size >= tar_size)
    {
        if ((config_ic.image_capture_width <= CRZ_LIMIT_WT) ||
            ((config_ic.image_capture_width == config_ic.crop_width) && (config_ic.image_capture_height == config_ic.crop_height)))
        {
            idp_capture_to_jpeg_current_running_pass = IDP_IC_1PASS_1_STATE;

            crz_struct.tar_width = config_ic.image_capture_width;
            crz_struct.tar_height = config_ic.image_capture_height;
        }
        else
        {
          #if defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502)
            #if defined(IDP_TILE_MODE_SUPPORT)
            // 60 Down scaling use 4 pass, 1st pass => corp & 1x scaling, 2nd and 3rd pass => tile and pitching, 4th pass -> source clip
            idp_capture_to_jpeg_current_running_pass = IDP_IC_4PASS_1_STATE; //Tile mode, source accumulation
            idp_sa_tile_calc(src_w, src_h, config_ic.image_capture_width, config_ic.image_capture_height, &config_ic.tile_para);
            #else
            ASSERT(0); // VGA sensor should not capture 1M(1280) and 2M(1600)
            #endif
          #else
            // 52 and 50 Resizer target width msut <= 320 or 800 when source is ISP and do resizing
            idp_capture_to_jpeg_current_running_pass = IDP_IC_2PASS_1_STATE;
          #endif
            crz_struct.tar_width = src_w;   ////1x => tar_w = src_crop_w
            crz_struct.tar_height = src_h;  ////1x => tar_h = src_crop_h
        }
    }
    else
    {
      #if defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502)
        //Up scaling use 2 pass, 1st pass => corp & 1x scaling, 2nd pass => Up scaling
        if (config_ic.image_capture_width <= CRZ_LIMIT_WT)
        {
            idp_capture_to_jpeg_current_running_pass = IDP_IC_2PASS_1_STATE;
        }
        else
        //Up scaling use 3 pass, 1st pass => corp & 1x scaling and pitching, 2nd and 3rd pass => tile with source clip and pitching
        #if defined(IDP_TILE_MODE_SUPPORT)
        {
            idp_capture_to_jpeg_current_running_pass = IDP_IC_3PASS_1_STATE; //Tile mode, billinear interpolation

            idp_bi_tile_calc(src_w, src_h, config_ic.image_capture_width, config_ic.image_capture_height, &config_ic.tile_para);
        }
        #else
        {
            ASSERT(0); // VGA sensor should not capture 1M(1280) and 2M(1600)
        }
        #endif
      #else
        //Up scaling use 2 pass, 1st pass => corp & 1x scaling, 2nd pass => Up scaling
        idp_capture_to_jpeg_current_running_pass = IDP_IC_2PASS_1_STATE;
      #endif

        crz_struct.tar_width = src_w;   ////1x => tar_w = src_crop_w
        crz_struct.tar_height = src_h;  ////1x => tar_h = src_crop_h
    }
    idp_capture_to_jpeg_pass_1_tar_width = crz_struct.tar_width;
    idp_capture_to_jpeg_pass_1_tar_height = crz_struct.tar_height;

    crz_struct.crop_en1 = config_ic.enable_crop; //pass 1
    //if (KAL_TRUE == crz_struct.crop_en1) // Always TRUE, code shrink
    {
        idp_config_crz_crop(config_ic.image_src_width, config_ic.image_src_height, config_ic.crop_width, config_ic.crop_height);
    }

    result = idp_resz_crz_config(owner->crz_key, &crz_struct, config_to_hardware);
    ASSERT(KAL_TRUE == result);

    return KAL_TRUE;
}

static kal_bool
_idp_scen_ic_jpeg__config_rotdma0(
    idp_owner_t * const owner,
    kal_bool const config_to_hardware)
{
    kal_bool result;
    kal_uint32 temp_tar_size, final_tar_size;
    kal_uint32 shift_size = 0;

    if (KAL_TRUE == owner->have_config_imgdma_irt0)
    {
        return KAL_TRUE;
    }

    rotdma0_struct.yuv_color_fmt = (MM_IMAGE_COLOR_FORMAT_ENUM) config_ic.jpeg_yuv_mode;

    final_tar_size = config_ic.image_capture_width * config_ic.image_capture_height;
    temp_tar_size = crz_struct.tar_width * crz_struct.tar_height;

    //Down scaling use 1 pass
    if (IDP_IC_1PASS_1_STATE == idp_capture_to_jpeg_current_running_pass)
    {
        rotdma0_struct.y_dest_start_addr = config_ic.image_capture_y_buffer;
        rotdma0_struct.u_dest_start_addr = config_ic.image_capture_u_buffer;
        rotdma0_struct.v_dest_start_addr = config_ic.image_capture_v_buffer;
    }
    else if (IDP_IC_2PASS_1_STATE == idp_capture_to_jpeg_current_running_pass)
    {
        //Down scaling use 2 pass, 1st pass => corp & 1x scaling,, 2nd pass => Down scaling, shift_size = 0
        //Up scaling use 2 pass, 1st pass => corp & 1x scaling, 2nd pass => Up scaling
        if (final_tar_size > temp_tar_size)
        {
            shift_size = (final_tar_size - temp_tar_size); // Must be 4x, due to YUV420, w & h = 2x => w x h = 4x
        }

        rotdma0_struct.y_dest_start_addr = config_ic.image_capture_y_buffer + shift_size; //// Must be 4x

        if (shift_size <= 0x0F)
        {
            rotdma0_struct.u_dest_start_addr = config_ic.image_capture_u_buffer; //// Must be 4x
            rotdma0_struct.v_dest_start_addr = config_ic.image_capture_v_buffer; //// Must be 4x
        }
        else
        {
            rotdma0_struct.u_dest_start_addr = config_ic.image_capture_u_buffer + ((shift_size>>4)<<2); //// Must be 4x
            rotdma0_struct.v_dest_start_addr = config_ic.image_capture_v_buffer + ((shift_size>>4)<<2); //// Must be 4x
        }
    }
#if defined(IDP_TILE_MODE_SUPPORT)
    else if (IDP_IC_3PASS_1_STATE == idp_capture_to_jpeg_current_running_pass)
    {
        //Up scaling use 3 pass, 1st pass => corp & 1x scaling and pitching, 2nd and 3rd pass => tile with source clip and pitching
        //Tile mode, billinear interpolation
        /*                                                        src                               tar
               ________________________________________                           ________________________________________ 
              |                   |                    |                         |                   |                    |
              |                   |                    |                         |                   |                    |
              |                   |                    |                         |                   |                    |
              |                   |                    |                         |                   |                    |
              |                   |                    |                         |                   |                    |
              |                   |                    |                         |                   |                    |
              |                   |                    |                         |                   |                    |
              |                   |                    |                         |                   |                    |
              |                   | (ptich addr)       |                         |                   |                    |
              |                   |     _______________|     _______________     |                   |                    |
              |                   |    |       |       |    |       |       |    |                   |                    |
              |                   |    |       |       |    |       |       |    |                   |                    |
              |                   |    |       |       |    |       |       |    |                   |                    |
              |___________________|____|_______|_______|    |_______|_______|    |___________________|____________________|

        */
        rotdma0_struct.pitch_enable = KAL_TRUE;
        rotdma0_struct.pitch_bytes = config_ic.image_capture_width; //Final width

        idp_scen_ic_sm_y_offset = (config_ic.image_capture_height - idp_capture_to_jpeg_pass_1_tar_height) * config_ic.image_capture_width + 
                                  (config_ic.image_capture_width - idp_capture_to_jpeg_pass_1_tar_width);

        rotdma0_struct.y_dest_start_addr = config_ic.image_capture_y_buffer + idp_scen_ic_sm_y_offset; //// Must be 4x

        idp_scen_ic_sm_uv_offset = (((config_ic.image_capture_height - idp_capture_to_jpeg_pass_1_tar_height) * config_ic.image_capture_width) >> 2) + 
                                   ((config_ic.image_capture_width - idp_capture_to_jpeg_pass_1_tar_width) >> 1);
        
        rotdma0_struct.u_dest_start_addr = config_ic.image_capture_u_buffer + idp_scen_ic_sm_uv_offset; //// Must be 4x
        rotdma0_struct.v_dest_start_addr = config_ic.image_capture_v_buffer + idp_scen_ic_sm_uv_offset; //// Must be 4x
    }
    else if (IDP_IC_4PASS_1_STATE == idp_capture_to_jpeg_current_running_pass)
    {
        //Down scaling use 4 pass, 1st pass => corp & 1x scaling, 2nd and 3rd pass => tile and pitching, 4th pass -> source clip
        //Tile mode, source accumulation
        /*                                                                     src                                tar
               ________________________________________      ________________________________________       _______________
              |       |       |   |                    |    |                   |                    |     |       |       |
              |       |       |   |                    |    |                   |                    |     |       |       |
              |       |       |   |                    |    |                   |                    |     |       |       |
              |_______|_______|   |                    |    |                   |                    |     |_______|_______|
              |                   |                    |    |                   |                    |
              |                   |                    |    |                   |                    |
              |                   |                    |    |                   |                    |
              |                   |                    |    |                   |                    |
              |                   |                    |    |                   |                    |
              |                   |                    |    |                   |                    |
              |                   |                    |    |                   |                    |
              |                   |                    |    |                   |                    |
              |                   |                    |    |                   |                    |
              |___________________|____________________|    |___________________|____________________|

        */
        rotdma0_struct.y_dest_start_addr = config_ic.image_capture_y_buffer;
        rotdma0_struct.u_dest_start_addr = config_ic.image_capture_u_buffer;
        rotdma0_struct.v_dest_start_addr = config_ic.image_capture_v_buffer;
    }
#endif
    else
    {
         ASSERT(0);
    }

    rotdma0_struct.src_width = crz_struct.tar_width;
    rotdma0_struct.src_height = crz_struct.tar_height;

    rotdma0_struct.fend_intr_en = KAL_TRUE;
    rotdma0_struct.fend_intr_cb = _idp_scen_ic_jpeg__lisr_rotdma0__frame_done;

    rotdma0_struct.hw_frame_sync = KAL_TRUE;

    rotdma0_struct.protect_en = KAL_TRUE;   //According to spec. From Camera, enable protect
    rotdma0_struct.bus_control_threshold = ROTDMA_BUS_CONTROL_THRESHOLD;

    rotdma0_struct.rot_angle = IMG_ROT_ANGLE_0;

    result = idp_imgdma_rotdma0_config(owner->irt0_key, &rotdma0_struct, config_to_hardware);
    ASSERT(KAL_TRUE == result);

    owner->have_config_imgdma_irt0 = KAL_TRUE;

    return KAL_TRUE;
}

#endif  // #if defined(__MTK_TARGET__)

/****************************************************************************
 * local function bodies -- IDP scenario hook function bodies
 ****************************************************************************/
static void
idp_camera_capture_to_jpeg_init_config_hook(
    idp_owner_t * const owner)
{
#if defined(__MTK_TARGET__)
#if defined(IDP_HISR_SUPPORT)
    idp_capture_to_jpeg_frame_done_hisr_handle =
        idp_hisr_register(owner->key,
                          IDP_HISR_CB_TYPE_IMGDMA,
                          _idp_scen_ic_jpeg__hisr_rotdma0__frame_done,
                          NULL);
#endif

    kal_mem_set(&config_ic, 0, sizeof(config_ic));
#endif
}

kal_bool
idp_camera_capture_to_jpeg_close_hook(
    idp_owner_t * const owner)
{
#if defined(__MTK_TARGET__)

    idp_resz_crz_close(owner->crz_key, &crz_struct);
    idp_imgdma_rotdma0_close(owner->irt0_key, &rotdma0_struct);

#if defined(IDP_HISR_SUPPORT)
    idp_hisr_clear_all(owner->key, IDP_HISR_CB_TYPE_IMGDMA);

    idp_capture_to_jpeg_frame_done_hisr_handle = NULL;
#endif

#endif
    return KAL_TRUE;
}

kal_bool
idp_camera_capture_to_jpeg_read_user_config_fast_hook(
    idp_owner_t * const owner,
    kal_uint32 const para_type,
    kal_uint32 const para_value)
{
    return KAL_TRUE;
}

kal_bool
idp_camera_capture_to_jpeg_read_user_config_hook(
    idp_owner_t * const owner,
    va_list ap)
{
    kal_uint32 para_type;
    kal_bool finish = KAL_FALSE;

    while (KAL_FALSE == finish)
    {
        para_type = va_arg(ap, kal_uint32);

        switch (para_type)
        {
            case 0:
                finish = KAL_TRUE;
                break;

            case IDP_CAMERA_CAPTURE_TO_JPEG_IMAGE_SRC_WIDTH:
                config_ic.image_src_width = va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_CAPTURE_TO_JPEG_IMAGE_SRC_HEIGHT:
                config_ic.image_src_height = va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_CAPTURE_TO_JPEG_FULLSIZE_IMAGE_CAPTURE_WIDTH:
                config_ic.image_capture_width = va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_CAPTURE_TO_JPEG_FULLSIZE_IMAGE_CAPTURE_HEIGHT:
                config_ic.image_capture_height = va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_CAPTURE_TO_JPEG_FULLSIZE_IMAGE_YUV_ADDRESSES:
                config_ic.image_capture_y_buffer = va_arg(ap, kal_uint32); // Y buffer address, Do nothing in MT6236
                config_ic.image_capture_u_buffer = va_arg(ap, kal_uint32); // U buffer address, Do nothing in MT6236
                config_ic.image_capture_v_buffer = va_arg(ap, kal_uint32); // V buffer address, Do nothing in MT6236
                break;

            case IDP_CAMERA_CAPTURE_TO_JPEG_FULLSIZE_JPEG_YUV_MODE:
                config_ic.jpeg_yuv_mode = (MM_IMAGE_COLOR_FORMAT_ENUM) va_arg(ap, kal_uint32);
                if (MM_IMAGE_FORMAT_YUV420 != (MM_IMAGE_FORMAT_ENUM) config_ic.jpeg_yuv_mode)
                {
                    return KAL_FALSE;
                }
                break;

            case IDP_CAMERA_CAPTURE_TO_JPEG_CROP_ENABLE:
                config_ic.enable_crop = (kal_bool) va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_CAPTURE_TO_JPEG_CROP_WIDTH:
                config_ic.crop_width = va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_CAPTURE_TO_JPEG_CROP_HEIGHT:
                config_ic.crop_height = va_arg(ap, kal_uint32);
                break;

            default:
                va_arg(ap, kal_uint32);
                break;
        }
    }

    return KAL_TRUE;
}

kal_bool
idp_camera_capture_to_jpeg_config_hook(
    idp_owner_t * const owner,
    kal_bool const config_to_hardware)
{
#if defined(__MTK_TARGET__)

    _idp_scen_ic_jpeg__config_crz(owner, config_to_hardware);

    _idp_scen_ic_jpeg__config_rotdma0(owner, config_to_hardware);

#endif

    return KAL_TRUE;
}

static hook_collect_t hook_collect =
{
    idp_camera_capture_to_jpeg_init_config_hook,
    idp_common_open_hook,//idp_camera_capture_to_jpeg_open_hook,
    idp_camera_capture_to_jpeg_close_hook,
    idp_common_stop_hook,//idp_camera_capture_to_jpeg_stop_hook,
    idp_camera_capture_to_jpeg_read_user_config_hook,
    idp_camera_capture_to_jpeg_read_user_config_fast_hook,
    idp_dummy_hook3,//idp_camera_capture_to_jpeg_finish_read_user_config_hook,
    idp_dummy_hook,//idp_camera_capture_to_jpeg_hardware_setting_have_been_changed_hook,
    idp_camera_capture_to_jpeg_config_hook,
    idp_common_query_hook,//idp_camera_capture_to_jpeg_query_hook,
    idp_dummy_hook,//idp_camera_capture_to_jpeg_start_intermedia_pipe_hook,
    idp_common_start_input_hook,//idp_camera_capture_to_jpeg_start_input_hook,
    idp_common_start_output_hook,//idp_camera_capture_to_jpeg_start_output_hook,
    idp_dummy_hook2,//idp_camera_capture_to_jpeg_compute_mem_for_each_hw_hook,
    idp_common_is_busy_hook,//idp_camera_capture_to_jpeg_is_busy_hook,
    idp_common_is_in_use_hook,//idp_camera_capture_to_jpeg_is_in_use_hook,
    idp_dummy_hook2,//idp_camera_capture_to_jpeg_clear_setting_diff_hook,
    idp_dummy_hook//idp_camera_capture_to_jpeg_is_setting_diff_is_ok_when_busy_hook
};




static IdpCheckReentrant idp_camera_capture_to_jpeg_check_reentrant = {KAL_NILTASK_ID, IDP_CALL_BY_HISR, IDP_CHECK_REENTRANT_PREV_OPEN, KAL_FALSE};

kal_bool
idp_camera_capture_to_jpeg_open_real(
    kal_uint32 * const key,
    char const * const filename,
    kal_uint32 const lineno)
{

    return idp_scenario_open(key,
                             &idp_camera_capture_to_jpeg_check_reentrant,
                             IDP_TRACED_API_camera_capture_to_jpeg_OPEN,
                             IDP_SCENARIO_camera_capture_to_jpeg,
                             &hook_collect);
}

kal_bool
idp_camera_capture_to_jpeg_is_in_use(void)
{
    return idp_scenario_is_in_use(&idp_camera_capture_to_jpeg_check_reentrant,
                                  IDP_TRACED_API_camera_capture_to_jpeg_IS_IN_USE,
                                  &hook_collect);
}

kal_bool
idp_camera_capture_to_jpeg_close(
    kal_uint32 const key)
{
    return idp_scenario_close(key,
                              &idp_camera_capture_to_jpeg_check_reentrant,
                              IDP_TRACED_API_camera_capture_to_jpeg_CLOSE,
                              &hook_collect);
}

kal_bool
idp_camera_capture_to_jpeg_stop(
    kal_uint32 const key)
{
    return idp_scenario_stop(key,
                             &idp_camera_capture_to_jpeg_check_reentrant,
                             IDP_TRACED_API_camera_capture_to_jpeg_STOP,
                             &hook_collect);
}

kal_bool
idp_camera_capture_to_jpeg_start_intermedia_pipe(
    kal_uint32 const key)
{
    return idp_scenario_start_intermedia_pipe(key,
                                              &idp_camera_capture_to_jpeg_check_reentrant,
                                              IDP_TRACED_API_camera_capture_to_jpeg_START_INTERMEDIA_PIPE,
                                              &hook_collect);
}

kal_bool
idp_camera_capture_to_jpeg_start_input(
    kal_uint32 const key)
{
    return idp_scenario_start_input(key,
                                    &idp_camera_capture_to_jpeg_check_reentrant,
                                    IDP_TRACED_API_camera_capture_to_jpeg_START_INPUT,
                                    &hook_collect);
}

kal_bool
idp_camera_capture_to_jpeg_start_output(
    kal_uint32 const key)
{
    return idp_scenario_start_output(key,
                                     &idp_camera_capture_to_jpeg_check_reentrant,
                                     IDP_TRACED_API_camera_capture_to_jpeg_START_OUTPUT,
                                     &hook_collect);
}

kal_bool
idp_camera_capture_to_jpeg_start_all(
    kal_uint32 const key)
{
    return idp_scenario_start_all(key,
                                  &idp_camera_capture_to_jpeg_check_reentrant,
                                  IDP_TRACED_API_camera_capture_to_jpeg_START_ALL,
                                  &hook_collect);
}

kal_bool
idp_camera_capture_to_jpeg_is_busy(
    kal_uint32 const key,
    kal_bool * const busy)
{
    return idp_scenario_is_busy(key,
                                busy,
                                IDP_TRACED_API_camera_capture_to_jpeg_IS_BUSY,
                                &hook_collect);
}

kal_bool
idp_camera_capture_to_jpeg_config_fast(
    kal_uint32 const key,
    kal_uint32 const para_type,
    kal_uint32 const para_value)
{
    return idp_scenario_config_fast(key,
                                    para_type,
                                    para_value,
                                    IDP_TRACED_API_camera_capture_to_jpeg_CONFIG_FAST,
                                    &hook_collect);
}

kal_bool
idp_camera_capture_to_jpeg_config(
    kal_uint32 const key,
    ...)
{
    va_list ap;
    kal_bool result;
    if (0 == key)
    {
        return KAL_FALSE;
    }

    va_start(ap, key);

    result = idp_scenario_config(key,
                                 &idp_camera_capture_to_jpeg_check_reentrant,
                                 IDP_TRACED_API_camera_capture_to_jpeg_CONFIG,
                                 &hook_collect,
                                 ap);
    va_end(ap);

    return result;

}

kal_bool
idp_camera_capture_to_jpeg_configurable(
    kal_uint32 const key,
    ...)
{
    va_list ap;
    kal_bool result;

    if (0 == key)
    {
        return KAL_FALSE;
    }

    va_start(ap, key);

    result = idp_scenario_configurable(key,
                                       &idp_camera_capture_to_jpeg_check_reentrant,
                                       IDP_TRACED_API_camera_capture_to_jpeg_CONFIGURABLE,
                                       &hook_collect,
                                       ap);
    va_end(ap);

    return result;
}


kal_bool
idp_camera_capture_to_jpeg_query(
    kal_uint32 const key,
    ...)
{
    va_list ap;
    kal_bool result;

    if (0 == key)
    {
        return KAL_FALSE;
    }

    va_start(ap, key);

    result = idp_scenario_query(key,
                                &idp_camera_capture_to_jpeg_check_reentrant,
                                IDP_TRACED_API_camera_capture_to_jpeg_QUERY,
                                &hook_collect,
                                ap);
    va_end(ap);

    return result;
}


kal_bool idp_camera_capture_to_jpeg_config_and_start(
    kal_uint32 const key,
    ...)
{
    va_list ap;
    kal_bool result;


    if (0 == key)
    {
        return KAL_FALSE;
    }

    va_start(ap, key);

    result = idp_scenario_config_and_start(key,
                                           &idp_camera_capture_to_jpeg_check_reentrant,
                                           IDP_TRACED_API_camera_capture_to_jpeg_CONFIG_AND_START,
                                           &hook_collect,
                                           ap);
    va_end(ap);

    return result;
}



#endif  // #if defined(DRV_IDP_6252_SERIES)


