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

#include <idp_video_encode.h>
#include <idp_cam_if.h>

#include <mt6252/idp_engines.h>

#if defined(IDP_HISR_SUPPORT)
#include <mt6252/idp_hisr.h>
#endif
#include <mt6252/idp_scen_common_mt6252.h>

//include wfc check busy api
#include <wfc/inc/wfc_mm_api.h>

/****************************************************************************
 * external symbols
 ****************************************************************************/
IDP_COMMON_API_DECLARATION(video_encode)

/****************************************************************************
 * local typedef
 ****************************************************************************/
typedef kal_uint32 (*idp_scen_vr_get_fb_cb_t)(void);

/****************************************************************************
 * local variables
 ****************************************************************************/
static struct
{
    kal_uint16                display_width;
    kal_uint16                display_height;
    kal_uint16                encode_width;
    kal_uint16                encode_height;

    /* DZ params */
    kal_bool                  enable_crz_frame_start_intr;

    img_rot_angle_enum_t      rot_angle;

    kal_bool                  update_to_lcd; /***< This flag only used in VR5 */

    /** crop param ***********/
    kal_uint16                crop_width;
    kal_uint16                crop_height;
    /** crop param END***********/

    /** FW trigger *************/
    kal_uint32                lcd_buffer[3];
    kal_uint32                lcd_buffer_index;

    /** Video V2, return encode buffer *************/
    kal_uint32                current_encode_buffer;

    kal_uint32                vr_cur_frame;
    kal_uint32                vr_mdp_keep_enc_buf_num;

#if defined(__ATV_SUPPORT__)
    kal_bool                  trigger_display_delay_to_frame_start_enable;
#endif
#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    kal_uint32                vr_tmp_buffer;
    tile_calc_para            tile_para;
#endif
} config_vr;

static kal_uint32 vr_y_plane_size;

#if defined(IDP_HISR_SUPPORT)
static idp_hisr_handle_t idp_scen_vr_frame_start_hisr_handle;       // for digital zoom
static idp_hisr_handle_t idp_scen_vr_pass1_frame_done_hisr_handle;  // for display frame
#endif

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
static kal_bool idp_vr_width_over_limit = KAL_FALSE;
static kal_bool idp_vr_crop_setting_in_use = KAL_FALSE;
static kal_bool idp_vr_crop_en = KAL_FALSE;
static kal_uint32 idp_vr_pass_num = 1;
static kal_uint32 idp_vr_img_width = 0;
static kal_uint32 idp_vr_img_height = 0;
static kal_uint32 idp_vr_crop_width = 0;
static kal_uint32 idp_vr_crop_height = 0;
#endif

static void rotdma0_vr_lcd_buffer_partial_config(void)
{
    rotdma0_struct.yuv_color_fmt = MM_IMAGE_COLOR_FORMAT_PACKET_UYVY422;

    rotdma0_struct.src_width = config_vr.display_width;
    rotdma0_struct.src_height = config_vr.display_height;

#if defined(MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
    if (KAL_TRUE == _idp_scen_triple_display_buffer_enable)
    {
        if (1 < _idp_scen_display_buffer_cnt)
        {
            // Currently 0 for display done. Switch to write buffer 1
            rotdma0_struct.y_dest_start_addr = _idp_scen_display_buffer_addr[1];
        }
        else
        {
            // Currently 0 for display done, but we only have 1 buffer now
            // Still set write buffer to 0
            rotdma0_struct.y_dest_start_addr = _idp_scen_display_buffer_addr[0];
        }
    }
    else
#endif
    {
        rotdma0_struct.y_dest_start_addr = config_vr.lcd_buffer[config_vr.lcd_buffer_index];
    }

    rotdma0_struct.rot_angle = config_vr.rot_angle;

    idp_imgdma_rotdma0_config_partial(&rotdma0_struct);
}

static void rotdma0_vr_encode_buffer_partial_config(void)
{
    // MP4 encode, get encode buffer first time rotdma frame done(LCD buffer).
    // Get encode buffer after return encode buffer next time.
    if (0 == config_vr.vr_mdp_keep_enc_buf_num)
    {
        kal_uint32 u4EncodeBufAddr = 0;
        MM_ERROR_CODE_ENUM eErr = MM_ERROR_NONE;

        // get yuv address from video
        eErr = pfIDP_CAL_CB(IDPCAL_CBID_VIDEO_ENCODE_GET_ENC_BUF, &u4EncodeBufAddr, sizeof(kal_uint32));
        if ((0x0 == u4EncodeBufAddr) ||
            (eErr != MM_ERROR_NONE))
        {
            ASSERT(0);
        }

        config_vr.current_encode_buffer = u4EncodeBufAddr;
        config_vr.vr_mdp_keep_enc_buf_num ++;
    }

    rotdma0_struct.yuv_color_fmt = MM_IMAGE_COLOR_FORMAT_YUV420;

    rotdma0_struct.src_width = config_vr.encode_width;
    rotdma0_struct.src_height = config_vr.encode_height;

    rotdma0_struct.y_dest_start_addr  =  config_vr.current_encode_buffer;
    rotdma0_struct.u_dest_start_addr =  rotdma0_struct.y_dest_start_addr + vr_y_plane_size;
    rotdma0_struct.v_dest_start_addr = rotdma0_struct.u_dest_start_addr + (vr_y_plane_size >> 2);

    rotdma0_struct.rot_angle = IMG_ROT_ANGLE_0;

    idp_imgdma_rotdma0_config_partial(&rotdma0_struct);
}

static void
_idp_scen_vr__lisr_crz__frame_start(void *user_data)
{
#if defined(__MTK_TARGET__)
    idp_add_traced_lisr(IDP_TRACED_API_video_encode__________CRZ_START_LISR);

#if defined(IDP_HISR_SUPPORT)
    idp_hisr_activate(idp_scen_vr_frame_start_hisr_handle);
#endif

#endif
}

static void
_idp_scen_vr__lisr_rotdma0__frame_done(void *user_data)
{
#if defined(__MTK_TARGET__)
    idp_add_traced_lisr(IDP_TRACED_API_video_encode__________IRT0_LISR);

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    if (KAL_FALSE == idp_vr_width_over_limit)
#endif
    {
        switch (config_vr.vr_cur_frame)
        {
            case 0: //current frame is UYVY, config next encode buffer, then output to LCD
                rotdma0_warm_reset();
    
                rotdma0_vr_encode_buffer_partial_config();
                IMGDMA_ROTDMA_START();
                break;
    
            case 1: //current frame is YUV, config next lcd frame buffer, then return encode buffer
                rotdma0_warm_reset();
    
                rotdma0_vr_lcd_buffer_partial_config();
                IMGDMA_ROTDMA_START();
                break;
    
            default:
                ASSERT(0);
                break;
        }
    }

#if defined(__CAMERA_MATV_ESD_RECOVERY_SUPPORT__)
    resz_esd_check_flag = KAL_TRUE;
#endif

#if defined(IDP_HISR_SUPPORT)
    idp_hisr_activate(idp_scen_vr_pass1_frame_done_hisr_handle);
#endif
#endif //#if defined(__MTK_TARGET__)
}

static void
_idp_scen_vr__lisr_crz__pixel_dropped(void *user_data)
{
    idp_add_traced_lisr(IDP_TRACED_API_video_encode________________CRZ_PIXEL_DROPPED_LISR);

    // Current frame drop, means next frame will use new crz setting, but old rotdma setting.
    // We must change back crz setting next frame start hisr, which will drop next frame.
    config_vr.vr_cur_frame = 1 - config_vr.vr_cur_frame;
}

static void
_idp_scen_vr__lisr_crz__lock_drop_frame(void *user_data)
{
    idp_add_traced_lisr(IDP_TRACED_API_video_encode________________CRZ_LOCK_DROP_FRAME_LISR);
}

#if defined(IDP_HISR_SUPPORT)
/****************************************************************************
 * local function bodies -- HISR
 ****************************************************************************/
static void crz_vr_frame_config(kal_uint32 frame, idp_resz_crz_struct *resize_struct)
{
    kal_uint32 result;

    switch (frame)
    {
        case 0:
            //LCD output
            crz_struct.tar_width = config_vr.display_width;
            crz_struct.tar_height = config_vr.display_height;
            break;

        case 1:
            crz_struct.tar_width = config_vr.encode_width;
            crz_struct.tar_height = config_vr.encode_height;
            break;

        default:
            ASSERT(0);
            break;
    }

    result =  idp_resz_crz_config_real(&crz_struct, KAL_TRUE);
    ASSERT(KAL_TRUE == result);
}

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
static void idp_vr_config_and_start_next_pass()
{
    // stop current pass
    idp_imgdma_rotdma0_stop_real(&rotdma0_struct);
    idp_resz_crz_stop_real(&crz_struct);

    //config CRZ
    if (0 == idp_vr_pass_num)
    {
        crz_struct.input_src1 = IDP_MODULE_OUTER_ISP;
        crz_struct.fstart1_intr_en = KAL_TRUE;
        crz_struct.auto_restart = KAL_TRUE;
        crz_struct.clip_en = KAL_FALSE;
        // set CRZ to bypass mode because of limitation
        crz_struct.src_width = idp_vr_img_width;
        crz_struct.src_height = idp_vr_img_height;
        crz_struct.tar_width = config_vr.crop_width;
        crz_struct.tar_height = config_vr.crop_height;
        crz_struct.org_width = config_vr.crop_width;
        if (KAL_FALSE == idp_vr_crop_setting_in_use)
        {
            idp_vr_crop_width = config_vr.crop_width;
            idp_vr_crop_height = config_vr.crop_height;
            idp_vr_crop_setting_in_use = KAL_TRUE;
        }

        crz_struct.crop_en1 = idp_vr_crop_en;
        idp_config_crz_crop(idp_vr_img_width, idp_vr_img_height, 
                            config_vr.crop_width, config_vr.crop_height);
        crz_struct.tile_mode = KAL_FALSE;
    }
    else
    {
        crz_struct.input_src1 = IDP_MODULE_IMGDMA_RDMA0;
        crz_struct.fstart1_intr_en = KAL_FALSE;
        crz_struct.auto_restart = KAL_FALSE;
        crz_struct.clip_en = KAL_TRUE;
        crz_struct.org_width = idp_vr_crop_width;

        if (1 == idp_vr_pass_num)
        {
            // tile pass 1 setting
            crz_struct.src_width = config_vr.tile_para.tile_src_size_x[0];
            crz_struct.src_height = idp_vr_crop_height;
            crz_struct.tar_width = config_vr.display_width >> 1;
            crz_struct.tar_height = config_vr.display_height;
            crz_struct.src_buff_y_addr = config_vr.vr_tmp_buffer;

            crz_struct.crop_en1 = KAL_TRUE;
            crz_struct.crop_left1 = config_vr.tile_para.tile_bi_sa_src_start_pos[0] - config_vr.tile_para.tile_adj_src_start_pos[0];
            crz_struct.crop_right1 = config_vr.tile_para.tile_bi_sa_src_end_pos[0] - config_vr.tile_para.tile_adj_src_start_pos[0];
            crz_struct.crop_top1 = 0;
            crz_struct.crop_bottom1 = idp_vr_crop_height - 1;

            // Tile Setting
            crz_struct.tile_mode = KAL_TRUE;
            crz_struct.tile_org_frame_width = idp_vr_crop_width;
            crz_struct.tile_org_tar_width = config_vr.display_width;
            crz_struct.tile_start_pos_x = config_vr.tile_para.tile_start_pos_x[0];
            crz_struct.tile_start_pos_y = config_vr.tile_para.tile_start_pos_y[0];
            crz_struct.tile_trunc_err_comp_x = config_vr.tile_para.tile_trunc_err_comp_x[0];
            crz_struct.tile_trunc_err_comp_y = config_vr.tile_para.tile_trunc_err_comp_y[0];
            crz_struct.tile_resid_x = config_vr.tile_para.tile_init_resid_x[0];
            crz_struct.tile_resid_y = config_vr.tile_para.tile_init_resid_y[0];
        }
        else if (2 == idp_vr_pass_num)
        {
            // tile pass 2 setting
            crz_struct.src_width = config_vr.tile_para.tile_src_size_x[1];
            crz_struct.src_height = idp_vr_crop_height;
            crz_struct.tar_width = config_vr.display_width >> 1;
            crz_struct.tar_height = config_vr.display_height;
            crz_struct.src_buff_y_addr += ((idp_vr_crop_width - crz_struct.src_width) << 1);

            crz_struct.crop_en1 = KAL_TRUE;
            crz_struct.crop_left1 = config_vr.tile_para.tile_bi_sa_src_start_pos[1] - config_vr.tile_para.tile_adj_src_start_pos[1];
            crz_struct.crop_right1 = config_vr.tile_para.tile_bi_sa_src_end_pos[1] - config_vr.tile_para.tile_adj_src_start_pos[1];
            crz_struct.crop_top1 = 0;
            crz_struct.crop_bottom1 = idp_vr_crop_height - 1;

            // Tile Setting
            crz_struct.tile_mode = KAL_TRUE;
            crz_struct.tile_org_frame_width = idp_vr_crop_width;
            crz_struct.tile_org_tar_width = config_vr.display_width;
            crz_struct.tile_start_pos_x = config_vr.tile_para.tile_start_pos_x[1];
            crz_struct.tile_start_pos_y = config_vr.tile_para.tile_start_pos_y[1];
            crz_struct.tile_trunc_err_comp_x = config_vr.tile_para.tile_trunc_err_comp_x[1];
            crz_struct.tile_trunc_err_comp_y = config_vr.tile_para.tile_trunc_err_comp_y[1];
            crz_struct.tile_resid_x = config_vr.tile_para.tile_init_resid_x[1];
            crz_struct.tile_resid_y = config_vr.tile_para.tile_init_resid_y[1];
         }
        else
        {
            // video encode setting
            crz_struct.src_width = idp_vr_crop_width;
            crz_struct.src_height = idp_vr_crop_height;
            crz_struct.tar_width = config_vr.encode_width;
            crz_struct.tar_height = config_vr.encode_height;
            crz_struct.src_buff_y_addr = config_vr.vr_tmp_buffer;

            crz_struct.crop_en1 = KAL_FALSE;

            // Tile Setting
            crz_struct.tile_mode = KAL_FALSE;
            
            // release crop settings lock
            idp_vr_crop_setting_in_use = KAL_FALSE;
        }
    }
    idp_resz_crz_config_real(&crz_struct, KAL_TRUE);

    //config ROTDMA
    rotdma0_struct.src_width = crz_struct.tar_width;
    rotdma0_struct.src_height = crz_struct.tar_height;

    if (0 == idp_vr_pass_num)
    {
        rotdma0_struct.rot_angle = IMG_ROT_ANGLE_0;
        rotdma0_struct.y_dest_start_addr = config_vr.vr_tmp_buffer;
        rotdma0_struct.pitch_enable = KAL_FALSE;
        rotdma0_struct.yuv_color_fmt = MM_IMAGE_COLOR_FORMAT_PACKET_UYVY422;
    }
    else
    {
        rotdma0_struct.rot_angle = config_vr.rot_angle;
        rotdma0_struct.pitch_enable = KAL_TRUE;
        if (1 == idp_vr_pass_num)
        {
            rotdma0_struct.y_dest_start_addr = config_vr.lcd_buffer[config_vr.lcd_buffer_index];
        }
        else if (2 == idp_vr_pass_num)
        {
            if (IMG_ROT_ANGLE_0 == rotdma0_struct.rot_angle)
                rotdma0_struct.y_dest_start_addr += config_vr.display_width;
            else
                rotdma0_struct.y_dest_start_addr += config_vr.display_height * config_vr.display_width;
        }
        else
        {
            rotdma0_struct.pitch_enable = KAL_FALSE;
            rotdma0_vr_encode_buffer_partial_config();
        }
    }

    rotdma0_warm_reset();
    idp_imgdma_rotdma0_config_partial(&rotdma0_struct);

    //start pass
    idp_imgdma_rotdma0_start_real(&rotdma0_struct);
    idp_resz_crz_start_real(&crz_struct);
}
#endif


static void
idp_video_encode_update_lcd_buffer(void)
{
#if defined(__MTK_TARGET__)
    kal_uint32 update_addr;
    CAL_BUFFER_CB_STRUCT rCPFrmDoneCBArg;
    CAL_CALLBACK_ID_ENUM eCBID = IDPCAL_CBID_VIDEO_ENCODE_PREVIEW_DISPLAY_FRM_DONE;

    // if LCD is busy, no switch and trigger LCD
    if (WFC_TRUE == wfcMMCheckBusy())
    {
        return;
    }

#if defined(MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
    if (KAL_TRUE == _idp_scen_triple_display_buffer_enable)
    {
        // get buffer for display, buffer 0 always for display
        rCPFrmDoneCBArg.ImageBuffAddr = _idp_scen_display_buffer_addr[0];

        // enable release write buffer and disable acquire next buffer process for triple display buffer
        rCPFrmDoneCBArg.EnableAcquireWriteBuffer= KAL_FALSE;
    }
    else
#endif
    {
        update_addr = config_vr.lcd_buffer[config_vr.lcd_buffer_index++];
        config_vr.lcd_buffer_index = (config_vr.lcd_buffer_index) & 0x01;

        // enable release and acquire write buffer process for double display buffer
        rCPFrmDoneCBArg.EnableAcquireWriteBuffer= KAL_TRUE;
    }
    rCPFrmDoneCBArg.EnableReleaseWriteBuffer= KAL_TRUE;

    // Triger delay one frame and this is first frame of frame start
#if defined(__ATV_SUPPORT__)
    if ((KAL_TRUE == config_vr.trigger_display_delay_to_frame_start_enable) && (completed_display_buffer_count < 2))
    {
        completed_display_buffer_count++;
        return; // First 2 frame no trigger display
    }
#endif

    // update LCM
    if (KAL_TRUE == config_vr.update_to_lcd)
    {
        //LCD_recode_preview(0, update_addr);
        rCPFrmDoneCBArg.ImageBuffFormat = (MM_IMAGE_FORMAT_ENUM) MM_IMAGE_COLOR_FORMAT_PACKET_UYVY422;
        rCPFrmDoneCBArg.ImageRotateAngle = (MM_IMAGE_ROTATE_ENUM) config_vr.rot_angle;
        rCPFrmDoneCBArg.ImageBuffAddr = update_addr;

#if defined(MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
        if (KAL_TRUE == _idp_scen_triple_display_buffer_enable)
        {
            // release current display buffer if MDP holds more than 1 display buffer and 
            // release function is success
            // if we only hold 1 frame, no trigger display and no return buffer
            if (1 < _idp_scen_display_buffer_cnt)
            {
                if (pfIDP_CAL_CB(eCBID, &rCPFrmDoneCBArg, sizeof(CAL_BUFFER_CB_STRUCT)) == MM_ERROR_NONE)
                {
                    // return current display buffer 0
                    // and set next frame done buffer 0 is next frame current display buffer
                    _idp_scen_display_buffer_addr[0] = _idp_scen_display_buffer_addr[1];
                    _idp_scen_display_buffer_addr[1] = _idp_scen_display_buffer_addr[2];
                    _idp_scen_display_buffer_cnt--;
                }
            }
        }
        else
#endif
        {
            // release and get new buffer
            pfIDP_CAL_CB(eCBID, &rCPFrmDoneCBArg, sizeof(CAL_BUFFER_CB_STRUCT));
        }
    }
#endif // #if defined(__MTK_TARGET__)
}

static kal_bool
_idp_scen_vr__hisr_rotdma0__frame_done(void *user_data)
{
#if defined(__MTK_TARGET__)
    kal_uint32 vr_current_frame;
    kal_uint32 savedMask;

    savedMask = SaveAndSetIRQMask();
    vr_current_frame = config_vr.vr_cur_frame;
    RestoreIRQMask(savedMask);

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    // display buffer is ready at pass 2 end, encode buffer is ready at pass 3 end
    if (KAL_FALSE == idp_vr_width_over_limit ||
        ((KAL_TRUE == idp_vr_width_over_limit) && 
         (0 == idp_vr_pass_num || 3 == idp_vr_pass_num)))
#endif
    {
    if (1 == vr_current_frame)  // means encode buffer done
    {
        CAL_BUFFER_CB_STRUCT rVRFrmDoneCBArg;

        rVRFrmDoneCBArg.ImageBuffAddr = (kal_uint32) config_vr.current_encode_buffer;

        pfIDP_CAL_CB(IDPCAL_CBID_VIDEO_ENCODE_RET_ENC_BUF, &rVRFrmDoneCBArg, sizeof(CAL_BUFFER_CB_STRUCT));
        if (1 != config_vr.vr_mdp_keep_enc_buf_num)
        {
            ASSERT(0);
        }
        config_vr.vr_mdp_keep_enc_buf_num--;  //vr_mdp_keep_enc_buf_num should 0 after return buffer at this moment

        // Return buffer and get new buffer form CAL for next time encode
        if (0 == config_vr.vr_mdp_keep_enc_buf_num)
        {
            kal_uint32 u4EncodeBufAddr = 0;
            MM_ERROR_CODE_ENUM eErr = MM_ERROR_NONE;

            // get yuv address from video
            eErr = pfIDP_CAL_CB(IDPCAL_CBID_VIDEO_ENCODE_GET_ENC_BUF, &u4EncodeBufAddr, sizeof(kal_uint32));
            if ((0x0 == u4EncodeBufAddr) ||
                (eErr != MM_ERROR_NONE))
            {
                ASSERT(0);
            }

            config_vr.current_encode_buffer = u4EncodeBufAddr;
            config_vr.vr_mdp_keep_enc_buf_num ++;
        }
    }
    else if (0 == vr_current_frame)  // means lcd frame done
    {
        // Triger display in frame done
    #if defined(__ATV_SUPPORT__)
        if (KAL_TRUE != config_vr.trigger_display_delay_to_frame_start_enable)
    #endif
        {
            idp_video_encode_update_lcd_buffer();
        }
    }
    else
    {
        ASSERT(0);
    }

    config_vr.vr_cur_frame = 1 - config_vr.vr_cur_frame;
    }
#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    if (KAL_TRUE == idp_vr_width_over_limit)
    {
        // do tile calculation when 1st pass is done
        if (1 == idp_vr_pass_num)
        {
            if (idp_vr_crop_width * idp_vr_crop_height <= config_vr.display_width * config_vr.display_height)
            {
                idp_bi_tile_calc(idp_vr_crop_width, idp_vr_crop_height,
                                 config_vr.display_width, config_vr.display_height,
                                 &config_vr.tile_para);      

                crz_struct.sa_en_x = KAL_FALSE;
                crz_struct.sa_en_y = KAL_FALSE;
            }
            else
            {
                idp_sa_tile_calc(idp_vr_crop_width, idp_vr_crop_height,
                                 config_vr.display_width, config_vr.display_height,
                                 &config_vr.tile_para);

                crz_struct.sa_en_x = KAL_TRUE;
                crz_struct.sa_en_y = KAL_TRUE;
            }
        }

        idp_vr_config_and_start_next_pass();
    
        if (3 == idp_vr_pass_num)
            idp_vr_pass_num = 0;
        else
            idp_vr_pass_num++;
    }
#endif
#endif
    return KAL_TRUE;
}

static kal_bool
_idp_scen_vr__hisr_crz__frame_start(void *param)
{
    kal_uint32 const savedMask = SaveAndSetIRQMask();
    kal_bool const enable = config_vr.enable_crz_frame_start_intr;
    RestoreIRQMask(savedMask);

    if (KAL_TRUE == enable) // digital zoom callback
    {
        pfIDP_CAL_CB(IDPCAL_CBID_VIDEO_ENCODE_CRZ_FRM_START, NULL, 0);
    }

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    if (KAL_FALSE == idp_vr_width_over_limit)
#endif
    {
    switch (config_vr.vr_cur_frame)
    {
        case 0: // current frame is UYVY422 LCD, config next YUV420 frame setting ENCODE
            crz_vr_frame_config(1, &crz_struct);
            break;

        case 1: // current frame is YUV420 ENCODE, config next UYVY422 frame setting LCD
            crz_vr_frame_config(0, &crz_struct);

            // Triger display in frame start
        #if defined(__ATV_SUPPORT__)
            if (KAL_TRUE == config_vr.trigger_display_delay_to_frame_start_enable)
            {
                idp_video_encode_update_lcd_buffer();
            }
        #endif
            break;

        default:
            ASSERT(0);
            break;
    }
    }
#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    else
    {
    #if defined(__ATV_SUPPORT__)
        if (KAL_TRUE == config_vr.trigger_display_delay_to_frame_start_enable)
        {
            idp_video_encode_update_lcd_buffer();
        }
    #endif
    }
#endif

    return KAL_TRUE;
}
#endif  // #if defined(IDP_HISR_SUPPORT)

/****************************************************************************
 * local function bodies -- HW config functions
 ****************************************************************************/
#if defined(__MTK_TARGET__)
static kal_bool
_idp_scen_vr__config_crz(
    idp_owner_t * const owner,
    kal_bool const config_to_hardware)
{
    kal_bool result;

    crz_struct.input_src1 = IDP_MODULE_OUTER_ISP;

    crz_struct.fstart1_intr_en = KAL_TRUE;
    crz_struct.fstart1_intr_cb = _idp_scen_vr__lisr_crz__frame_start;

    crz_struct.pixel_dropped_intr_en = KAL_TRUE;
    crz_struct.pixel_dropped_intr_cb = _idp_scen_vr__lisr_crz__pixel_dropped;

    crz_struct.lock_drop_frame_intr_en = KAL_TRUE;
    crz_struct.lock_drop_frame_intr_cb = _idp_scen_vr__lisr_crz__lock_drop_frame;

    crz_struct.auto_restart = KAL_TRUE;
    crz_struct.hw_frame_sync = KAL_TRUE;

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    crz_struct.src_width = idp_vr_img_width;
    crz_struct.src_height = idp_vr_img_height;
    crz_struct.crop_en1 = idp_vr_crop_en;

    if (KAL_TRUE == idp_vr_width_over_limit)
    {
        // set CRZ to bypass mode because of limitation
        crz_struct.tar_width = config_vr.crop_width;
        crz_struct.tar_height = config_vr.crop_height;
        crz_struct.mem_in_color_fmt1 = MM_IMAGE_COLOR_FORMAT_PACKET_UYVY422;
        crz_struct.src_buff_y_addr = config_vr.vr_tmp_buffer;
        crz_struct.org_width = config_vr.crop_width;
        if (KAL_FALSE == idp_vr_crop_setting_in_use)
        {
            idp_vr_crop_width = config_vr.crop_width;
            idp_vr_crop_height = config_vr.crop_height;            
            idp_vr_crop_setting_in_use = KAL_TRUE;
        }
    }
    else
#endif
    {
    crz_struct.tar_width = config_vr.display_width;
    crz_struct.tar_height = config_vr.display_height;
    }

    //if (KAL_TRUE == crz_struct.crop_en1) // Always TRUE, code shrink
    {
        idp_config_crz_crop(crz_struct.src_width, crz_struct.src_height, config_vr.crop_width, config_vr.crop_height);
    }

    result = idp_resz_crz_config(owner->crz_key, &crz_struct, config_to_hardware);
    if (KAL_FALSE == result)
    {
        return KAL_FALSE;
    }

    return KAL_TRUE;
}

static kal_bool
_idp_scen_vr__config_rotdma0(
  idp_owner_t * const owner,
  kal_bool const config_to_hardware)
{
    kal_bool result;

    if (KAL_TRUE == owner->have_config_imgdma_irt0)
    {
        return KAL_TRUE;
    }

    rotdma0_struct.yuv_color_fmt = MM_IMAGE_COLOR_FORMAT_PACKET_UYVY422;
#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    if (KAL_TRUE == idp_vr_width_over_limit)
    {
        if (NULL == config_vr.vr_tmp_buffer)
        {
            return KAL_FALSE;
        }
        else
        {
            rotdma0_struct.y_dest_start_addr = config_vr.vr_tmp_buffer;
            if (IMG_ROT_ANGLE_0 == config_vr.rot_angle)
                rotdma0_struct.pitch_bytes = (config_vr.display_width << 1); //Final width
            else
                rotdma0_struct.pitch_bytes = (config_vr.display_height << 1); //Final width
        }
    }
    else
#endif
    rotdma0_struct.y_dest_start_addr = config_vr.lcd_buffer[0];

    rotdma0_struct.src_width = crz_struct.tar_width;
    rotdma0_struct.src_height = crz_struct.tar_height;

    rotdma0_struct.fend_intr_en = KAL_TRUE;
    rotdma0_struct.fend_intr_cb = _idp_scen_vr__lisr_rotdma0__frame_done;

    rotdma0_struct.hw_frame_sync = KAL_TRUE;

    rotdma0_struct.protect_en = KAL_TRUE;   //According to spec. From Camera, enable protect
    rotdma0_struct.bus_control_threshold = ROTDMA_BUS_CONTROL_THRESHOLD;

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    if (KAL_TRUE == idp_vr_width_over_limit)
        rotdma0_struct.rot_angle = IMG_ROT_ANGLE_0;
    else
#endif
    rotdma0_struct.rot_angle = config_vr.rot_angle;

    vr_y_plane_size = (config_vr.encode_width) *(config_vr.encode_height);

    result = idp_imgdma_rotdma0_config(owner->irt0_key, &rotdma0_struct, config_to_hardware);
    ASSERT(KAL_TRUE == result);

    owner->have_config_imgdma_irt0 = KAL_TRUE;

#if defined(MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
    if (KAL_TRUE == _idp_scen_triple_display_buffer_enable)
    {
        //record the 3 display buffers
        _idp_scen_display_buffer_addr[0] = config_vr.lcd_buffer[0];
        _idp_scen_display_buffer_addr[1] = config_vr.lcd_buffer[1];
        _idp_scen_display_buffer_addr[2] = config_vr.lcd_buffer[2];
        _idp_scen_display_buffer_cnt = 3;
    }
#endif

    return KAL_TRUE;
}

#endif // #if defined(__MTK_TARGET__)

/****************************************************************************
 * local function bodies -- IDP scenario hook function bodies
 ****************************************************************************/
static void
idp_video_encode_init_config_hook(
    idp_owner_t * const owner)
{
#if defined(__MTK_TARGET__)

#if defined(IDP_HISR_SUPPORT)
    idp_scen_vr_pass1_frame_done_hisr_handle =
        idp_hisr_register(owner->key,
                          IDP_HISR_CB_TYPE_IMGDMA,
                          _idp_scen_vr__hisr_rotdma0__frame_done,
                          NULL);

    idp_scen_vr_frame_start_hisr_handle =
        idp_hisr_register(owner->key,
                          IDP_HISR_CB_TYPE_RESZ,
                          _idp_scen_vr__hisr_crz__frame_start,
                          NULL);
#endif

#if defined(MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
    _idp_scen_triple_display_buffer_enable = KAL_FALSE;
#endif

#if defined(__ATV_SUPPORT__)
    completed_display_buffer_count = 0;
#endif

#if defined(IDP_MM_COLOR_SUPPORT)
    colorWrapperOpen(&mm_color_handle, COLOR_SCENARIO_VIDEO_ENCODE);
    colorWrapperEnable(mm_color_handle, 0);
#endif

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    idp_vr_crop_setting_in_use = KAL_FALSE;
    idp_vr_pass_num = 1;
#endif

    kal_mem_set(&config_vr, 0, sizeof(config_vr));

#endif
}


kal_bool
idp_video_encode_close_hook(
    idp_owner_t * const owner)
{
#if defined(__MTK_TARGET__)
    idp_resz_crz_close(owner->crz_key, &crz_struct);
    idp_imgdma_rotdma0_close(owner->irt0_key, &rotdma0_struct);

    // release HISR
#if defined(IDP_HISR_SUPPORT)
    idp_hisr_clear_all(owner->key, IDP_HISR_CB_TYPE_RESZ);
    idp_hisr_clear_all(owner->key, IDP_HISR_CB_TYPE_IMGDMA);

    idp_scen_vr_frame_start_hisr_handle = NULL;
    idp_scen_vr_pass1_frame_done_hisr_handle = NULL;
#endif  // #if defined(IDP_HISR_SUPPORT)

    // Return all got buffers to Video Encoder...
    {
        if (0 != config_vr.vr_mdp_keep_enc_buf_num)
        {
            CAL_BUFFER_CB_STRUCT rVRFrmDoneCBArg;

            rVRFrmDoneCBArg.ImageBuffAddr = (kal_uint32) config_vr.current_encode_buffer;

            pfIDP_CAL_CB(IDPCAL_CBID_VIDEO_ENCODE_RET_ENC_BUF, &rVRFrmDoneCBArg, sizeof(CAL_BUFFER_CB_STRUCT));
        }
    }

#endif

#if defined(IDP_MM_COLOR_SUPPORT)
    colorWrapperDisable(mm_color_handle);
    colorWrapperClose(mm_color_handle);
#endif

    return KAL_TRUE;
}

kal_bool
idp_video_encode_read_user_config_fast_hook(
    idp_owner_t * const owner,
    kal_uint32 const para_type,
    kal_uint32 const para_value)
{
#if defined(__MTK_TARGET__)

    switch (para_type)
    {
        case IDP_VIDEO_ENCODE_UPDATE_TO_LCD:
            // [MAUI_01756493]
            {
                kal_uint32 const savedMask = SaveAndSetIRQMask();
                config_vr.update_to_lcd = (kal_bool) para_value;
                RestoreIRQMask(savedMask);
            }
            break;

        case IDP_VIDEO_ENCODE_FRAME_START_HISR_CB_ENABLE:
            {
                kal_uint32 const savedMask = SaveAndSetIRQMask();
                config_vr.enable_crz_frame_start_intr = (kal_bool) para_value;
                RestoreIRQMask(savedMask);
            }
            break;

        default:
            return KAL_FALSE;
            //break;
    }
#endif

    return KAL_TRUE;
}

kal_bool
idp_video_encode_read_user_config_hook(
    idp_owner_t * const owner,
    va_list ap)
{
#if defined(__MTK_TARGET__)
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

            case IDP_VIDEO_ENCODE_IMAGE_SRC_WIDTH:
            #if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
                idp_vr_img_width = va_arg(ap, kal_uint32);
            #else
                crz_struct.src_width = va_arg(ap, kal_uint32);
            #endif
                break;

            case IDP_VIDEO_ENCODE_IMAGE_SRC_HEIGHT:
            #if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
                idp_vr_img_height = va_arg(ap, kal_uint32);
            #else
                crz_struct.src_height = va_arg(ap, kal_uint32);
            #endif
                break;

            case IDP_VIDEO_ENCODE_DISPLAY_ROT_ANGLE:
                config_vr.rot_angle = (img_rot_angle_enum_t) va_arg(ap, kal_uint32);

                if (!((IMG_ROT_ANGLE_0 == config_vr.rot_angle) ||
                             (IMG_MIRROR_ROT_ANGLE_90 == config_vr.rot_angle)))
                {
                    return KAL_FALSE;
                }
                break;

            case IDP_VIDEO_ENCODE_FRAME_BUFFER_ADDRESS1:
                config_vr.lcd_buffer[0] = va_arg(ap, kal_uint32);
                config_vr.lcd_buffer[1] = config_vr.lcd_buffer[0];
                break;

            case IDP_VIDEO_ENCODE_FRAME_BUFFER_ADDRESS2:
                config_vr.lcd_buffer[1] = va_arg(ap, kal_uint32);
                break;

        #if defined(MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
            case IDP_VIDEO_ENCODE_FRAME_BUFFER_ADDRESS3:
                config_vr.lcd_buffer[2] = va_arg(ap, kal_uint32);

                if (NULL != config_vr.lcd_buffer[2])
                {
                    _idp_scen_triple_display_buffer_enable = KAL_TRUE;
                }
                else
                {
                    _idp_scen_triple_display_buffer_enable = KAL_FALSE;
                }
                break;
        #endif

            case IDP_VIDEO_ENCODE_DISPLAY_WIDTH:
                config_vr.display_width = va_arg(ap, kal_uint32);
            #if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
                if (480 < config_vr.display_width)
                {
                    idp_vr_width_over_limit = KAL_TRUE;
                }
            #endif
                break;

            case IDP_VIDEO_ENCODE_DISPLAY_HEIGHT:
                config_vr.display_height = va_arg(ap, kal_uint32);
                break;

            case IDP_VIDEO_ENCODE_ENCODE_WIDTH:
                config_vr.encode_width = va_arg(ap, kal_uint32);
                break;

            case IDP_VIDEO_ENCODE_ENCODE_HEIGHT:
                config_vr.encode_height = va_arg(ap, kal_uint32);
                break;

            case IDP_VIDEO_ENCODE_UPDATE_TO_LCD:
                config_vr.update_to_lcd = (kal_bool) va_arg(ap, kal_uint32);
                break;

            case IDP_VIDEO_ENCODE_FRAME_START_HISR_CB_ENABLE:
                {
                    kal_uint32 const savedMask = SaveAndSetIRQMask();
                    config_vr.enable_crz_frame_start_intr = (kal_bool) va_arg(ap, kal_uint32);
                    RestoreIRQMask(savedMask);
                }
                break;

            case IDP_VIDEO_ENCODE_PREVIEW_CROP_ENABLE:
            #if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
                idp_vr_crop_en = (kal_bool) va_arg(ap, kal_uint32);
            #else
                crz_struct.crop_en1 = (kal_bool) va_arg(ap, kal_uint32);
            #endif
                break;

            case IDP_VIDEO_ENCODE_PREVIEW_CROP_WIDTH:
                config_vr.crop_width = va_arg(ap, kal_uint32);
                break;

            case IDP_VIDEO_ENCODE_PREVIEW_CROP_HEIGHT:
                config_vr.crop_height = va_arg(ap, kal_uint32);
                break;

            case IDP_VIDEO_ENCODE_SENSOR_FPS_VIDEO_FPS:
                #if 0
                {
                    kal_uint32 sensor_fps, video_fps;
                    sensor_fps = va_arg(ap, kal_uint32);
                    video_fps = va_arg(ap, kal_uint32);

                    if ((2*video_fps) != sensor_fps)
                    {
                        ASSERT(0);
                        return KAL_FALSE;
                    }
                }
                #else
                va_arg(ap, kal_uint32);
                va_arg(ap, kal_uint32);
                #endif
                break;

        #if defined(__ATV_SUPPORT__)
            case IDP_VIDEO_ENCODE_ENABLE_TRIGGER_DISPLAY_DELAY_TO_FRAME_START:
                config_vr.trigger_display_delay_to_frame_start_enable = (kal_bool) va_arg(ap, kal_uint32);
              #if defined(MT6252H) || defined(MT6252)
                // MT6252 with 5193 MATV will drop frame when LCD and MDP work at the same time.
                // So, default we not delay, let frame done trigger LCD display
                config_vr.trigger_display_delay_to_frame_start_enable = KAL_FALSE;
              #endif
                break;
        #endif

        #if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
            case IDP_VIDEO_ENCODE_TEMP_BUFFER:
                config_vr.vr_tmp_buffer = (kal_uint32) va_arg(ap, kal_uint32);
                break;
        #endif

            default:
                va_arg(ap, kal_uint32);
                break;
        }
    } // while (KAL_FALSE == finish)
#endif  // #if defined(__MTK_TARGET__)

    return KAL_TRUE;
}


kal_bool
idp_video_encode_config_hook(
    idp_owner_t * const owner,
    kal_bool const config_to_hardware)
{
#if defined(__MTK_TARGET__)
    if (KAL_FALSE == _idp_scen_vr__config_crz(owner, config_to_hardware))
    {
        return KAL_FALSE;
    }

    if (KAL_FALSE == _idp_scen_vr__config_rotdma0(owner, config_to_hardware))
    {
        return KAL_FALSE;
    }
#endif

  return KAL_TRUE;
}

static hook_collect_t hook_collect =
{
    idp_video_encode_init_config_hook,
    idp_common_open_hook,//idp_video_encode_open_hook,
    idp_video_encode_close_hook,
    idp_common_stop_hook,//idp_video_encode_stop_hook,
    idp_video_encode_read_user_config_hook,
    idp_video_encode_read_user_config_fast_hook,
    idp_dummy_hook3,//idp_video_encode_finish_read_user_config_hook,
    idp_dummy_hook,//idp_video_encode_hardware_setting_have_been_changed_hook,
    idp_video_encode_config_hook,
    idp_common_query_hook,//idp_video_encode_query_hook,
    idp_dummy_hook,//idp_video_encode_start_intermedia_pipe_hook,
    idp_common_start_input_hook,//idp_video_encode_start_input_hook,
    idp_common_start_output_hook,//idp_video_encode_start_output_hook,
    idp_dummy_hook2,//idp_video_encode_compute_mem_for_each_hw_hook,
    idp_common_is_busy_hook,//idp_video_encode_is_busy_hook,
    idp_common_is_in_use_hook,//idp_video_encode_is_in_use_hook,
    idp_dummy_hook2,//idp_video_encode_clear_setting_diff_hook,
    idp_dummy_hook//idp_video_encode_is_setting_diff_is_ok_when_busy_hook
};





static IdpCheckReentrant idp_video_encode_check_reentrant = {KAL_NILTASK_ID, IDP_CALL_BY_HISR, IDP_CHECK_REENTRANT_PREV_OPEN, KAL_FALSE};

kal_bool
idp_video_encode_open_real(
    kal_uint32 * const key,
    char const * const filename,
    kal_uint32 const lineno)
{
    return idp_scenario_open(key,
                             &idp_video_encode_check_reentrant,
                             IDP_TRACED_API_video_encode_OPEN,
                             IDP_SCENARIO_video_encode,
                             &hook_collect);
}

kal_bool
idp_video_encode_is_in_use(void)
{

    return idp_scenario_is_in_use(&idp_video_encode_check_reentrant,
                                  IDP_TRACED_API_video_encode_IS_IN_USE,
                                  &hook_collect);
}

kal_bool
idp_video_encode_close(
    kal_uint32 const key)
{

    return idp_scenario_close(key,
                              &idp_video_encode_check_reentrant,
                              IDP_TRACED_API_video_encode_CLOSE,
                              &hook_collect);
}

kal_bool
idp_video_encode_stop(
    kal_uint32 const key)
{

    return idp_scenario_stop(key,
                             &idp_video_encode_check_reentrant,
                             IDP_TRACED_API_video_encode_STOP,
                             &hook_collect);
}

kal_bool
idp_video_encode_start_intermedia_pipe(
    kal_uint32 const key)
{
    return idp_scenario_start_intermedia_pipe(key,
                                              &idp_video_encode_check_reentrant,
                                              IDP_TRACED_API_video_encode_START_INTERMEDIA_PIPE,
                                              &hook_collect);
}

kal_bool
idp_video_encode_start_input(
    kal_uint32 const key)
{
    return idp_scenario_start_input(key,
                                    &idp_video_encode_check_reentrant,
                                    IDP_TRACED_API_video_encode_START_INPUT,
                                    &hook_collect);
}

kal_bool
idp_video_encode_start_output(
    kal_uint32 const key)
{
    return idp_scenario_start_output(key,
                                     &idp_video_encode_check_reentrant,
                                     IDP_TRACED_API_video_encode_START_OUTPUT,
                                     &hook_collect);
}

kal_bool
idp_video_encode_start_all(
    kal_uint32 const key)
{
    return idp_scenario_start_all(key,
                                  &idp_video_encode_check_reentrant,
                                  IDP_TRACED_API_video_encode_START_ALL,
                                  &hook_collect);
}

kal_bool
idp_video_encode_is_busy(
    kal_uint32 const key,
    kal_bool * const busy)
{
    return idp_scenario_is_busy(key,
                                busy,
                                IDP_TRACED_API_video_encode_IS_BUSY,
                                &hook_collect);
}

kal_bool
idp_video_encode_config_fast(
    kal_uint32 const key,
    kal_uint32 const para_type,
    kal_uint32 const para_value)
{
    return idp_scenario_config_fast(key,
                                    para_type,
                                    para_value,
                                    IDP_TRACED_API_video_encode_CONFIG_FAST,
                                    &hook_collect);
}

kal_bool
idp_video_encode_config(
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
                                 &idp_video_encode_check_reentrant,
                                 IDP_TRACED_API_video_encode_CONFIG,
                                 &hook_collect,
                                 ap);
    va_end(ap);

    return result;

}

kal_bool
idp_video_encode_configurable(
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
                                       &idp_video_encode_check_reentrant,
                                       IDP_TRACED_API_video_encode_CONFIGURABLE,
                                       &hook_collect,
                                       ap);
    va_end(ap);

    return result;
}


kal_bool
idp_video_encode_query(
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
                                &idp_video_encode_check_reentrant,
                                IDP_TRACED_API_video_encode_QUERY,
                                &hook_collect,
                                ap);
    va_end(ap);

    return result;
}


kal_bool idp_video_encode_config_and_start(
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
                                           &idp_video_encode_check_reentrant,
                                           IDP_TRACED_API_video_encode_CONFIG_AND_START,
                                           &hook_collect,
                                           ap);
    va_end(ap);

    return result;
}


#endif  // #if defined(DRV_IDP_6252_SERIES)

