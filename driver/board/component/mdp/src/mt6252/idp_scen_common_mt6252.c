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
//#include "drv_comm.h"

#include <idp_define.h>

#if defined(DRV_IDP_6252_SERIES)

#include <mt6252/idp_engines.h>

//#include <idp_cam_if.h>
#include <idp_camera_preview.h>

#include <mt6252/idp_scen_common_mt6252.h>

/****************************************************************************
 * external symbols
 ****************************************************************************/


/****************************************************************************
 * local typedef
 ****************************************************************************/


/****************************************************************************
 * common variables
 ****************************************************************************/
// this flag is used to indicate whether MDP triple display buffer is enabled or not
#if defined(MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
kal_bool _idp_scen_triple_display_buffer_enable = KAL_FALSE;
kal_uint32 _idp_scen_display_buffer_addr[3];
kal_uint32 _idp_scen_display_buffer_cnt = 0;
#endif

// this buffer used for delay one frame trigger LCD display
#if defined(__ATV_SUPPORT__)
kal_uint32 completed_display_buffer_count = 0;
#endif

#if 0 //defined(IDP_MM_COLOR_SUPPORT)
COLOR_HANDLE mm_color_handle;
#endif

typedef enum
{
    IDP_CAL_QUERY_PATH_SUPPORT = 0,
      /* Query format
       *           Description            Data Type
       * Input :   Path ID                IDP_CAL_QUERY_DATA_PATH_ENUM
       * Output:   Support(1) or Not(0)   kal_bool
       */
    IDP_CAL_QUERY_ROT_CAP,
      /* Query format
       *           Description               Data Type
       * Input :   Path ID                   IDP_CAL_QUERY_DATA_PATH_ENUM
       *           Color format              IDP_CAL_QUERY_POSTPROC_COLOR_FMT_CAP
       *           Desired rotation angle    img_rot_angle_enum_t
       * Output:   Suggested rotation angle  img_rot_angle_enum_t
       */
    //IDP_CAL_QUERY_DISP_COLOR_FMT_CAP
    //IDP_CAL_QUERY_POSTPROC_COLOR_FMT_CAP
    //IDP_CAL_QUERY_VIDEO_ENC_COLOR_FMT_CAP
    IDP_CAL_QUERY_FRAME_DONE_CB_FUNC
      /* Query format
       *         Description               Data Type
       * Input:  Path ID                   IDP_CAL_QUERY_DATA_PATH_ENUM
       * Output: pointer to IDP frame done callback function of specific path  void*
       */
} IDP_CAL_QUERY_TYPE_T;

/****************************************************************************
 * Callback function bodies
 ****************************************************************************/
#if defined(MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
kal_bool idp_scen_acquire_write_buffer(void)
{
    CAL_CALLBACK_ID_ENUM eCBID;

    if (KAL_TRUE == _idp_scen_triple_display_buffer_enable)
    {
        CAL_BUFFER_CB_STRUCT rFrmDoneCBArg;
        // enable acquire next buffer process only
        rFrmDoneCBArg.EnableReleaseWriteBuffer= KAL_FALSE;
        rFrmDoneCBArg.EnableAcquireWriteBuffer= KAL_TRUE;

        // current preview and video encode frame done do the same thing in cal_cb
        // maybe different if design change
        // now only preview have triple buffer.
        if (1)
        {
            eCBID = IDPCAL_CBID_CAM_PREVIEW_DISPLAY_FRM_DONE;
        }
        else
        {
            eCBID = IDPCAL_CBID_VIDEO_ENCODE_PREVIEW_DISPLAY_FRM_DONE;
        }

        pfIDP_CAL_CB(eCBID , &rFrmDoneCBArg, sizeof(CAL_BUFFER_CB_STRUCT));
        // record next buffer address for triple display buffer
        if (NULL != rFrmDoneCBArg.NextFrameBufferAddr)
        {
            if (_idp_scen_display_buffer_cnt < 3) // mdp only hold 1 or 2
            {
                _idp_scen_display_buffer_addr[_idp_scen_display_buffer_cnt] = rFrmDoneCBArg.NextFrameBufferAddr;    
                _idp_scen_display_buffer_cnt++;
            }
        }
    }
    return KAL_TRUE;
}
#endif

/****************************************************************************
 * common function bodies
 ****************************************************************************/ 
kal_bool
idp_common_open_hook(
    idp_owner_t * const owner)
{
    kal_bool result;

    // open crz
    result = idp_resz_crz_open(&(owner->crz_key));
    if (KAL_FALSE == result)
    {
        return KAL_FALSE;
    }

    //owner->have_config_resz_crz = KAL_FALSE;

    kal_mem_set(&crz_struct, 0x0, sizeof(idp_resz_crz_struct));

    // open rotdma0
    result = idp_imgdma_rotdma0_open(&(owner->irt0_key));
    if (KAL_FALSE == result)
    {
        return KAL_FALSE;
    }

    owner->have_config_imgdma_irt0 = KAL_FALSE;

    kal_mem_set(&rotdma0_struct, 0x0, sizeof(idp_imgdma_rotdma0_struct));

    return KAL_TRUE;
}



kal_bool
idp_common_stop_hook(
    idp_owner_t * const owner)
{
    idp_resz_crz_stop(owner->crz_key, &crz_struct);
    idp_imgdma_rotdma0_stop(owner->irt0_key, &rotdma0_struct);

    return KAL_TRUE;
}



kal_bool
idp_common_start_input_hook(
    idp_owner_t * const owner)
{
    idp_resz_crz_start(owner->crz_key, &crz_struct);

    return KAL_TRUE;
}



kal_bool
idp_common_start_output_hook(
    idp_owner_t * const owner)
{
    idp_imgdma_rotdma0_start(owner->irt0_key, &rotdma0_struct);

    return KAL_TRUE;
}



kal_bool
idp_common_is_busy_hook(
    idp_owner_t const * const owner,
    kal_bool * const busy)
{

    idp_imgdma_rotdma0_is_busy(owner->irt0_key, busy, &rotdma0_struct);

    if (KAL_TRUE == (*busy))
    {
        return KAL_TRUE;
    }

    idp_resz_crz_is_busy(owner->crz_key, busy, &crz_struct);

    return KAL_TRUE;
}



kal_bool
idp_common_is_in_use_hook(void)
{
    if (KAL_TRUE == idp_resz_crz_is_in_use())
    { 
        return KAL_TRUE; 
    }

    if (KAL_TRUE == idp_imgdma_rotdma0_is_in_use())
    { 
        return KAL_TRUE; 
    }

    return KAL_FALSE;
}



kal_bool
idp_common_query_hook(
    idp_owner_t * const owner,
    va_list ap)
{
    kal_uint32 para_type;
    kal_bool finish = KAL_FALSE;
    IDP_CAL_QUERY_TYPE_T eTypeTmp = IDP_CAL_QUERY_PATH_SUPPORT;
    img_rot_angle_enum_t eDesiredAngleTmp = IMG_ROT_ANGLE_0;

    while (KAL_FALSE == finish)
    {
        para_type = va_arg(ap, kal_uint32);

        switch (para_type)
        {
            case 0:
                finish = KAL_TRUE;
                break;

            case IDP_CAMERA_PREVIEW_QUERY_TYPE:
            //case IDP_CAMERA_CAPTURE_TO_JPEG_QUERY_TYPE:
            //case IDP_VIDEO_ENCODE_QUERY_TYPE:
                eTypeTmp = (IDP_CAL_QUERY_TYPE_T) va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_PREVIEW_QUERY_ARG_PATH:
            //case IDP_CAMERA_CAPTURE_TO_JPEG_QUERY_ARG_PATH:
            //case IDP_VIDEO_ENCODE_QUERY_ARG_PATH:

            case IDP_CAMERA_PREVIEW_QUERY_ARG_CLR_FMT:
            //case IDP_CAMERA_CAPTURE_TO_JPEG_QUERY_ARG_CLR_FMT:
            //case IDP_VIDEO_ENCODE_QUERY_ARG_CLR_FMT:
                va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_PREVIEW_QUERY_ARG_DESIRED_ANGLE:
            //case IDP_CAMERA_CAPTURE_TO_JPEG_QUERY_ARG_DESIRED_ANGLE:
            //case IDP_VIDEO_ENCODE_QUERY_ARG_DESIRED_ANGLE:
                eDesiredAngleTmp = (img_rot_angle_enum_t) va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_PREVIEW_QUERY_RESULT:
            //case IDP_CAMERA_CAPTURE_TO_JPEG_QUERY_RESULT:
            //case IDP_VIDEO_ENCODE_QUERY_RESULT:
                {
                    kal_uint32 * const pu4Result = va_arg(ap, kal_uint32*);

                    if (eTypeTmp == IDP_CAL_QUERY_PATH_SUPPORT)
                    {
                        #if defined(MT6252H) || defined(MT6252)
                        //MT6252 MDP hardware only supports display path, main image path
                        (*pu4Result) = (kal_uint32) KAL_FALSE;
                        #elif defined(MT6250) || defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)
                        //MT6250 MDP need pp path
                        (*pu4Result) = (kal_uint32) KAL_TRUE;
                        #endif
                    }
                    else if (eTypeTmp == IDP_CAL_QUERY_ROT_CAP)
                    {
                        // 90, 270, MT6252 display rotator only supports rotation CC 90 with mirror
                        (*pu4Result) = ((eDesiredAngleTmp & 0x01 == 0x01) ? (kal_uint32) IMG_MIRROR_ROT_ANGLE_90 : (kal_uint32) IMG_ROT_ANGLE_0);
                    }
                  #if defined(MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
                    else if (eTypeTmp == IDP_CAL_QUERY_FRAME_DONE_CB_FUNC)
                    {
                        (*pu4Result) = (kal_uint32)idp_scen_acquire_write_buffer;
                    }
                  #endif
                }
                break;

            default:
                return KAL_FALSE;
        }
    }

    return KAL_TRUE;
}


kal_bool
idp_dummy_hook(
    idp_owner_t * const owner)
{
    return KAL_TRUE;
}



void
idp_dummy_hook2(
    idp_owner_t * const owner)
{
}


kal_bool
idp_dummy_hook3(void)
{
    return KAL_TRUE;
}


void idp_config_crz_crop(
    kal_uint16 image_src_width,
    kal_uint16 image_src_height,
    kal_uint16 crop_width,
    kal_uint16 crop_height)
{
    crz_struct.crop_left1 = (image_src_width - crop_width) >> 1;
    crz_struct.crop_right1 = crz_struct.crop_left1 + (crop_width - 1);
    crz_struct.crop_top1 = (image_src_height - crop_height) >> 1;
    crz_struct.crop_bottom1 = crz_struct.crop_top1 + (crop_height - 1);
}

#if defined(IDP_TILE_MODE_SUPPORT)
// Tile mode related function
static kal_uint32 gcd(kal_uint32 m, kal_uint32 n)
{
    kal_uint32 r;

    while (n != 0)
    {
        r = m % n;
        m = n;
        n = r;
    }

    return m;
}

void
idp_bi_tile_calc(
    kal_uint32 src_w, kal_uint32 src_h,
    kal_uint32 tar_w, kal_uint32 tar_h,
    tile_calc_para *tile_result)
{
    kal_uint32 in_ratio, in_resid;
    kal_uint32 round_step;
    kal_uint32 curr_tile_first_idx, curr_tile_last_idx;
    kal_uint32 prev_last_round_idx, prev_tile_last_idx;
    kal_uint32 tar_tile_size;
    kal_uint32 curr_last_round_idx;

    // Compute the ratio and resid
    in_ratio = (src_w << 20) / tar_w;
    in_resid = src_w % tar_w;

    // Set tile is target witdh / 2
    tar_tile_size = tar_w / 2;

    // Compute the rounding step, every rounding step will have src pixel = tar pixel
    // Ex. 10 -> 15, GCD = 5, 15/3 = 5, every 3 pixel(tar) meat 2 pixel(src)
    // 0, 2/3, 4/3, 6/3, 8/3, 10/3, 12/3, 14/3, 16/3, 18/3, 20/3, 22/3, 24/3, 26/3, 28/3
    // [tar/GCD(src,tar)]
    round_step = tar_w / gcd(src_w, tar_w);

    /*********************************************************************
     * Tile #0 Setting.
     *********************************************************************/
    // Compute the last output rounding index of the current tile
    curr_tile_first_idx = 0;
    curr_tile_last_idx = tar_tile_size - 1;

    // A0 = {O0/[tar/GCD(src,tar)]} * [tar/GCD(src,tar)]
    curr_last_round_idx = (curr_tile_last_idx/round_step) * round_step;

    // Left source index = 0
    tile_result->tile_bi_sa_src_start_pos[0] = 0;

    if (src_w != tar_w)
    {
        // Right source index = {[(O0-A0) * Ratio] >> 20} + [(src*A0)/tar] + 1
        tile_result->tile_bi_sa_src_end_pos[0] = (kal_uint32)((((curr_tile_last_idx-curr_last_round_idx) * in_ratio) >> 20) +
                                                           ((src_w*curr_last_round_idx)/tar_w) +
                                                           1);
    }
    else
    {
        // Right source index = {[(O0-A0) * Ratio] >> 20} + [(src*A0)/tar]
        tile_result->tile_bi_sa_src_end_pos[0] = (kal_uint32)((((curr_tile_last_idx-curr_last_round_idx) * in_ratio) >> 20) +
                                                           ((src_w*curr_last_round_idx)/tar_w));
    }

    if (tile_result->tile_bi_sa_src_end_pos[0] >= src_w)
    {
        tile_result->tile_bi_sa_src_end_pos[0] = src_w - 1;
    }

    tile_result->tile_adj_src_start_pos[0] = tile_result->tile_bi_sa_src_start_pos[0] & 0xFFFFFFFE;
    tile_result->tile_adj_src_end_pos[0] = tile_result->tile_bi_sa_src_end_pos[0] | 0x00000001;
    tile_result->tile_src_size_x[0] = tile_result->tile_adj_src_end_pos[0] - tile_result->tile_adj_src_start_pos[0] + 1;

    tile_result->tile_start_pos_x[0] = 0;
    tile_result->tile_init_resid_x[0] = 0;
    tile_result->tile_trunc_err_comp_x[0] = tar_w;

    tile_result->tile_start_pos_y[0] = 0;
    tile_result->tile_trunc_err_comp_y[0] = tar_h;
    tile_result->tile_init_resid_y[0] = 0;

    /*********************************************************************
     * Tile #1 Setting.
     *********************************************************************/
    prev_last_round_idx = curr_last_round_idx;
    prev_tile_last_idx = curr_tile_last_idx;

    curr_tile_first_idx += tar_tile_size;
    curr_tile_last_idx += tar_tile_size;

    // {A1 = {O1/[tar/GCD(src,tar)]} * [tar/GCD(src,tar)]
    curr_last_round_idx = (curr_tile_last_idx/round_step) * round_step;

    // Compute tile parameters
    if (curr_tile_first_idx == (prev_last_round_idx + round_step)) // Of1 == A0 + [tar/GCD(src,tar)]
    {
        // Left source index = (src*Of1) / tar
        tile_result->tile_bi_sa_src_start_pos[1] = (src_w*curr_tile_first_idx) / tar_w;
        if (src_w != tar_w)
        {
            // Right source index = {[(O1-A1) * Ratio] >> 20} + [(src¡ÑA1)/tar] + 1
            tile_result->tile_bi_sa_src_end_pos[1] = (kal_uint32)((((curr_tile_last_idx-curr_last_round_idx) * in_ratio) >> 20) +
                                                               ((src_w*curr_last_round_idx)/tar_w) +
                                                               1);
        }
        else
        {
            // Right source index = {[(O1-A1) * Ratio] >> 20} + [(src¡ÑA1)/tar]
            tile_result->tile_bi_sa_src_end_pos[1] = (kal_uint32)((((curr_tile_last_idx-curr_last_round_idx) * in_ratio) >> 20) +
                                                               ((src_w*curr_last_round_idx)/tar_w));
        }
        tile_result->tile_start_pos_x[1] = 0;
        tile_result->tile_init_resid_x[1] = 0;
    }
    else
    {
        // Left source index = {[(O0+1-A0) * Ratio] >> 20} + [(src * A0 )/tar]
        tile_result->tile_bi_sa_src_start_pos[1] = (kal_uint32)((((prev_tile_last_idx+1-prev_last_round_idx) * in_ratio) >> 20) +
                                                             ((src_w*prev_last_round_idx)/tar_w));

        // Left source index = {[(O1-A1) * Ratio] >> 20} + [(src * A1 )/tar] + 1
        tile_result->tile_bi_sa_src_end_pos[1] = (kal_uint32)((((curr_tile_last_idx-curr_last_round_idx) * in_ratio) >> 20) +
                                                           ((src_w*curr_last_round_idx)/tar_w) +
                                                           1);
        // Start position = [(O0+1-A0) * Ratio] & 0x000FFFFF
        tile_result->tile_start_pos_x[1] = ((prev_tile_last_idx+1-prev_last_round_idx) * in_ratio) & 0x000FFFFF;
        // Initial residual = [(O0+1-A0) * Residual] % tar
        tile_result->tile_init_resid_x[1] = ((prev_tile_last_idx+1-prev_last_round_idx) * in_resid) % tar_w;
    }

    if (tile_result->tile_bi_sa_src_end_pos[1] >= src_w)
    {
        tile_result->tile_bi_sa_src_end_pos[1] = src_w - 1;
    }

    tile_result->tile_adj_src_start_pos[1] = tile_result->tile_bi_sa_src_start_pos[1] & 0xFFFFFFFE;
    tile_result->tile_adj_src_end_pos[1] = tile_result->tile_bi_sa_src_end_pos[1] | 0x00000001;
    tile_result->tile_src_size_x[1] = tile_result->tile_adj_src_end_pos[1] - tile_result->tile_adj_src_start_pos[1] + 1;

    tile_result->tile_trunc_err_comp_x[1] = tar_w;

    tile_result->tile_start_pos_y[1] = 0;
    tile_result->tile_trunc_err_comp_y[1] = tar_h;
    tile_result->tile_init_resid_y[1] = 0;
}

void
idp_sa_tile_calc(
    kal_uint32 src_w, kal_uint32 src_h,
    kal_uint32 tar_w, kal_uint32 tar_h,
    tile_calc_para *tile_result)
{
    kal_uint32 in_ratio, in_ratio_y;
    kal_uint32 curr_tile_last_idx;
    kal_uint32 prev_tile_remain_weight;
    kal_uint32 tar_tile_size;

    // Compute the ratio
    in_ratio = ((kal_uint32)(tar_w-1)<<20)/(src_w-1);
    in_ratio_y = ((kal_uint32)(tar_h-1)<<20)/(src_h-1);

    // Set tile is target witdh / 2
    tar_tile_size = tar_w / 2;


    /*********************************************************************
     * Tile #0 Setting.
     *********************************************************************/
    curr_tile_last_idx = tar_tile_size - 1;

    // Compute tile parameters
    tile_result->tile_bi_sa_src_start_pos[0] = 0;

    // {{[2*(O0+1)-1]<<20} + 3*Ratio-1}/(2*Ratio) - 1;
    tile_result->tile_bi_sa_src_end_pos[0] = (kal_uint32)(((((((curr_tile_last_idx+1)<<1)-1)<<20)+(in_ratio*3)-1)/(in_ratio<<1)) - 1);
    // [(1<<20)+Ratio] / 2
    tile_result->tile_sa_init_weight[0] = (kal_uint32)(((1<<20) + in_ratio) >> 1);

    tile_result->tile_adj_src_start_pos[0] = tile_result->tile_bi_sa_src_start_pos[0] & 0xFFFFFFFE;
    tile_result->tile_adj_src_end_pos[0] = tile_result->tile_bi_sa_src_end_pos[0] | 0x00000001;
    tile_result->tile_src_size_x[0] = tile_result->tile_adj_src_end_pos[0] - tile_result->tile_adj_src_start_pos[0] + 1;

    tile_result->tile_start_pos_x[0] = tile_result->tile_sa_init_weight[0];
    tile_result->tile_start_pos_y[0] = (kal_uint32)(((1<<20) + in_ratio_y) >> 1);

    tile_result->tile_trunc_err_comp_x[0] = 0;
    tile_result->tile_trunc_err_comp_y[0] = 0;

    tile_result->tile_init_resid_x[0] = 0;
    tile_result->tile_init_resid_y[0] = 0;
    /*********************************************************************
     * Tile #1 Setting.
     *********************************************************************/
    curr_tile_last_idx += tar_tile_size;

    // Compute the remaining weight of the previous tile
    // B1 = IW0 + [Ratio * (R0-L0)] - [(O0-Of0+1) << 20]
    prev_tile_remain_weight = tile_result->tile_sa_init_weight[0] +
                              (in_ratio * (tile_result->tile_bi_sa_src_end_pos[0]-tile_result->tile_bi_sa_src_start_pos[0])) -
                              (tar_tile_size << 20);

    if (prev_tile_remain_weight == 0)
    {
        tile_result->tile_bi_sa_src_start_pos[1] = tile_result->tile_bi_sa_src_end_pos[0] + 1;
        tile_result->tile_bi_sa_src_end_pos[1] = src_w - 1;

        tile_result->tile_sa_init_weight[1] = in_ratio;
    }
    else
    {
        tile_result->tile_bi_sa_src_start_pos[1] = tile_result->tile_bi_sa_src_end_pos[0];
        tile_result->tile_bi_sa_src_end_pos[1] = src_w - 1;

        tile_result->tile_sa_init_weight[1] = (kal_uint32) prev_tile_remain_weight;
    }

    tile_result->tile_adj_src_start_pos[1] = tile_result->tile_bi_sa_src_start_pos[1] & 0xFFFFFFFE;
    tile_result->tile_adj_src_end_pos[1] = tile_result->tile_bi_sa_src_end_pos[1] | 0x00000001;
    tile_result->tile_src_size_x[1] = tile_result->tile_adj_src_end_pos[1] - tile_result->tile_adj_src_start_pos[1] + 1;

    tile_result->tile_start_pos_x[1] = tile_result->tile_sa_init_weight[1];
    tile_result->tile_start_pos_y[1] = (kal_uint32)(((1<<20) + in_ratio_y) >> 1);

    tile_result->tile_trunc_err_comp_x[1] = 0;
    tile_result->tile_trunc_err_comp_y[1] = 0;

    tile_result->tile_init_resid_x[1] = 0;
    tile_result->tile_init_resid_y[1] = 0;
}
#endif

#endif  // #if defined(DRV_IDP_6252_SERIES)

