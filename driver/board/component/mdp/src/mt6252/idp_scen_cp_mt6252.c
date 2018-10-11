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
/* system includes */
//#include "app_ltlcom.h" /* Task message communiction */

//#include "drv_comm.h"
#include <idp_define.h>

#if defined(DRV_IDP_6252_SERIES)

#include <idp_core.h>

#include <idp_camera_preview.h>
#include <mt6252/idp_engines.h>
//#include <idp_cam_if.h>

#if defined(IDP_HISR_SUPPORT)
#include <mt6252/idp_hisr.h>
#endif
#include <mt6252/idp_scen_common_mt6252.h>

#include "mt25x3_hdk_lcd.h"

//include wfc check busy api
//#include <wfc/inc/wfc_mm_api.h>

/* to access the define of __POSTPROC_SUPPORT__ */
//#include "pp_feature_def.h"

//include G2D api
/*
#include "g2d_enum.h"
#include "g2d_common_api.h"
#include "g2d_lt_api.h"
#include "g2d_bitblt_api.h"
#include "g2d_font_api.h"
#include "g2d_rectfill_api.h"
#include "g2d_mutex.h"
*/
//include CAL api
//#include "cal_task_msg_if.h"

IDP_COMMON_API_DECLARATION(camera_preview)

/****************************************************************************
 * local variables
 ****************************************************************************/
static struct
{
    // Note this! this does not represent HW/SW trigger...
    kal_bool                  enable_trigger_lcd; /**< KAL_TRUE to enable trigger LCD, KAL_FALSE to disable trigger LCD */

    /**  CRZ param/config_cp START  *************************************/
    kal_uint16                image_src_width;  /* image width for CRZ input */
    kal_uint16                image_src_height; /* image height for CRZ input */
    kal_uint16                preview_width;    /* image width for LCD preview */
    kal_uint16                preview_height;   /* image height for LCD preview */

    kal_bool                  enable_crz_frame_start_intr;
    /**  CRZ param/config_cp   END  *************************************/

    /**  ROT0 param/config_cp  START  ***********************************/
    kal_uint32                frame_buffer_address; /* image buffer for LCM source buffer */
    kal_uint32                frame_buffer_address1; /* second image buffer for LCM source buffer */
    kal_uint32                frame_buffer_address2; /* Third image buffer for LCM source buffer */

    img_rot_angle_enum_t      rot_angle;
    /**  ROT0 param/config_cp    END  ***********************************/

    /** FW trigger param ***********/
    kal_uint32                 preview_buffer_idx;
    /**  FW trigger param   END  *******/

    /** crop param ***********/
    //kal_bool                  enable_crop; /* KAL_TRUE or KAL_FALSE to enable or disable overlay function */
    kal_uint16                crop_width;
    kal_uint16                crop_height;

    kal_bool                  dbuf_tearing_free_enable;
    /** crop param END***********/

#if defined(__ATV_SUPPORT__)
    kal_bool                  trigger_display_delay_to_frame_start_enable;
#endif

    kal_bool                  end_config;

    kal_bool                  crop_en;
#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    kal_uint32                cp_tmp_buffer;
    tile_calc_para            tile_para;
#endif
} config_cp;


#if defined(IDP_HISR_SUPPORT)
static idp_hisr_handle_t idp_camera_preview_frame_start_hisr_handle;
static idp_hisr_handle_t idp_camera_preview_display_frame_done_hisr_handle;
#endif

#if defined(MT6252H) || defined(MT6252)
static kal_bool _idp_scen_crz_width_over_limit = KAL_FALSE;
static kal_uint32 _idp_scen_crz_over_limit_width;
#endif

#if defined(__ATV_SUPPORT__)
kal_bool idp_camera_preview_pxd;
#endif

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
static kal_bool idp_cp_width_over_limit = KAL_FALSE;
static kal_bool idp_cp_crop_setting_in_use = KAL_FALSE;
static kal_uint32 idp_cp_pass_num = 1;
static kal_uint32 idp_cp_crop_width = 0;
static kal_uint32 idp_cp_crop_height = 0;
#endif

extern int lcdwidth, lcdheight;

/****************************************************************************
 * local function bodies -- LISR
 ****************************************************************************/
static void rotdma0_config_lcd_buffer_addr(kal_uint32 buffer)
{
#if defined(MT6252H) || defined(MT6252)
    if (_idp_scen_crz_width_over_limit == KAL_TRUE)
    {
        REG_IMGDMA_ROT_DMA_Y_DST_STR_ADDR = buffer + (_idp_scen_crz_over_limit_width-320)*crz_struct.tar_height;
    }
    else
#endif
    {
        REG_IMGDMA_ROT_DMA_Y_DST_STR_ADDR = buffer;
    }
    rotdma0_struct.y_dest_start_addr = REG_IMGDMA_ROT_DMA_Y_DST_STR_ADDR;
}

static void
_idp_scen_cp__lisr_crz__frame_start(void *user_data)
{
    idp_add_traced_lisr(IDP_TRACED_API_camera_preview__________CRZ_START_LISR);

#if defined(__CAMERA_MATV_ESD_RECOVERY_SUPPORT__)
    resz_esd_check_flag = KAL_TRUE;
#endif

#if defined(IDP_HISR_SUPPORT)
    idp_hisr_activate(idp_camera_preview_frame_start_hisr_handle);
#endif

}

static void 
_idp_scen_cp_lcd_change_buffer(void)
{
#if defined(MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
    if (KAL_TRUE == _idp_scen_triple_display_buffer_enable)
    {
        if (1 < _idp_scen_display_buffer_cnt)
        {
            // Currently 0 for display done. Switch to write buffer 1
            rotdma0_config_lcd_buffer_addr(_idp_scen_display_buffer_addr[1]);
        }
        else
        {
            // Currently 0 for display done, but we only have 1 buffer now
            // Still set write buffer to 0
            rotdma0_config_lcd_buffer_addr(_idp_scen_display_buffer_addr[0]);
        }
    }
    else
#endif
    {
        if (0 == config_cp.preview_buffer_idx)  // Currently writing buffer 0 done.
        {
            // Switch to write to buffer 1
            rotdma0_config_lcd_buffer_addr(config_cp.frame_buffer_address1);
        }
        else if (1 == config_cp.preview_buffer_idx)// Currently writing buffer 1 done.
        {
            // Switch to write to buffer 0
            rotdma0_config_lcd_buffer_addr(config_cp.frame_buffer_address);
        }
    }
}

static void
_idp_scen_cp__lisr_rotdma0__frame_done(void *user_data)
{
    idp_add_traced_lisr(IDP_TRACED_API_camera_preview__________ROTDMA0_DONE_LISR);

#if defined(__ATV_SUPPORT__)
    idp_camera_preview_pxd = KAL_FALSE;
#endif

#if 0
    // MATV no need check busy
#if defined(__ATV_SUPPORT__)
    if (KAL_TRUE != config_cp.trigger_display_delay_to_frame_start_enable)
#endif
    {
        // No switch and trigger LCD
        if ((WFC_TRUE == wfcMMCheckBusy()) && (config_cp.dbuf_tearing_free_enable == KAL_TRUE))
        {
          #if defined(MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
            if (KAL_FALSE == _idp_scen_triple_display_buffer_enable)
          #endif
          #if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
            if (KAL_FALSE == idp_cp_width_over_limit)
          #endif
            {
                rotdma0_warm_reset();
                IMGDMA_ROTDMA_START();

                return;
            }
        }
    }
#endif

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    if (KAL_FALSE == idp_cp_width_over_limit)
#endif
    {
    rotdma0_warm_reset();

    {
        _idp_scen_cp_lcd_change_buffer();
    }

    IMGDMA_ROTDMA_START();
    }

    // update to display
    {
        hal_display_lcd_layer_input_t layer_para;

        layer_para.source_key_flag = 0;
        layer_para.alpha_flag = 0;
        layer_para.color_format = HAL_DISPLAY_LCD_LAYER_COLOR_UYVY422;
        layer_para.alpha = 0;
        layer_para.rotate = HAL_DISPLAY_LCD_LAYER_ROTATE_0;
        layer_para.row_size = rotdma0_struct.src_width;
        layer_para.column_size = rotdma0_struct.src_height;
        layer_para.window_x_offset = 0;
        layer_para.window_y_offset = 0;
        layer_para.layer_enable = HAL_DISPLAY_LCD_LAYER0;

        //printf("src_width=%d, src_height=%d\r\n", rotdma0_struct.src_width, rotdma0_struct.src_height);

        if (0 == config_cp.preview_buffer_idx)  // Currently writing buffer 0 done.
        {
            layer_para.buffer_address = config_cp.frame_buffer_address;
            config_cp.preview_buffer_idx = 1;
        }
        else
        {
            layer_para.buffer_address = config_cp.frame_buffer_address1;
            config_cp.preview_buffer_idx = 0;
        }

        layer_para.pitch = rotdma0_struct.src_width*2;

        hal_display_lcd_turn_on_mtcmos();
        bsp_lcd_config_layer(&layer_para);
        bsp_lcd_update_screen(0 , 0 , lcdwidth-1, lcdheight-1);
    }

#if defined(IDP_HISR_SUPPORT)
    if (KAL_TRUE == config_cp.enable_trigger_lcd)
    {
        idp_hisr_activate(idp_camera_preview_display_frame_done_hisr_handle);
    }
#endif
}

static void
_idp_scen_cp__lisr_crz__pixel_dropped(void *user_data)
{
    idp_add_traced_lisr(IDP_TRACED_API_camera_preview________________CRZ_PIXEL_DROPPED_LISR);

    printf("%s:%d 0xA04501F0=%x\r\n", __FUNCTION__, __LINE__, *((volatile unsigned int *)0xA04501F0));

#if defined(__ATV_SUPPORT__)
    idp_camera_preview_pxd = KAL_TRUE;
#endif
}

static void
_idp_scen_cp__lisr_crz__lock_drop_frame(void *user_data)
{
    idp_add_traced_lisr(IDP_TRACED_API_camera_preview________________CRZ_LOCK_DROP_FRAME_LISR);
}

/****************************************************************************
 * local function bodies -- HISR
 ****************************************************************************/
#if defined(IDP_HISR_SUPPORT)
static void idp_camera_preview_update_lcd_buffer(void)
{
    CAL_BUFFER_CB_STRUCT rCPFrmDoneCBArg;
    CAL_CALLBACK_ID_ENUM eCBID = IDPCAL_CBID_CAM_PREVIEW_DISPLAY_FRM_DONE;

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
        // Currently writing buffer 0 done, change index to 1
        if (0 == config_cp.preview_buffer_idx)
        {
            rCPFrmDoneCBArg.ImageBuffAddr = config_cp.frame_buffer_address;
            config_cp.preview_buffer_idx = 1;
        }
        else if (1 == config_cp.preview_buffer_idx)  // Currently writing buffer 0 done, change index to 0
        {
            rCPFrmDoneCBArg.ImageBuffAddr = config_cp.frame_buffer_address1;
            config_cp.preview_buffer_idx = 0;
        }

        // enable release and acquire write buffer process for double display buffer
        rCPFrmDoneCBArg.EnableAcquireWriteBuffer= KAL_TRUE;
    }

    rCPFrmDoneCBArg.EnableReleaseWriteBuffer= KAL_TRUE;

    // Trigger delay one frame and this is first frame of frame start 
#if defined(__ATV_SUPPORT__)
    if ((KAL_TRUE == config_cp.trigger_display_delay_to_frame_start_enable) && (completed_display_buffer_count < 2))
    {
        completed_display_buffer_count++;
        return; // First 2 frame no trigger display
    }
#endif

    rCPFrmDoneCBArg.ImageBuffFormat = (MM_IMAGE_FORMAT_ENUM) MM_IMAGE_COLOR_FORMAT_PACKET_UYVY422;
    rCPFrmDoneCBArg.ImageRotateAngle = (MM_IMAGE_ROTATE_ENUM) config_cp.rot_angle;
    rCPFrmDoneCBArg.ImageBuffSize = crz_struct.tar_width * crz_struct.tar_height * 2;

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


static kal_bool
_idp_scen_cp__hisr_crz__frame_start(void *user_data)
{
    //kal_uint32 const savedMask = SaveAndSetIRQMask();
    kal_bool const enable = config_cp.enable_crz_frame_start_intr;
    //RestoreIRQMask(savedMask);

    // Triger display in frame start
#if defined(__ATV_SUPPORT__)
    if (KAL_TRUE == config_cp.trigger_display_delay_to_frame_start_enable)
    {
        idp_camera_preview_update_lcd_buffer();
    }
#endif

    if (KAL_TRUE == enable)
    {
        pfIDP_CAL_CB(IDPCAL_CBID_CAM_PREVIEW_CRZ_FRM_START, NULL, 0);
    }
    return KAL_TRUE;
}

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
static void idp_cp_config_and_start_next_pass()
{
    // stop current pass
    idp_imgdma_rotdma0_stop_real(&rotdma0_struct);
    idp_resz_crz_stop_real(&crz_struct);

    //config CRZ
    if (0 == idp_cp_pass_num)
    {
        crz_struct.input_src1 = IDP_MODULE_OUTER_ISP;
        crz_struct.fstart1_intr_en = KAL_TRUE;
        crz_struct.auto_restart = KAL_TRUE;
        crz_struct.clip_en = KAL_FALSE;
        // set CRZ to bypass mode because of limitation
        crz_struct.src_width = config_cp.image_src_width;
        crz_struct.src_height = config_cp.image_src_height;
        crz_struct.tar_width = config_cp.crop_width;
        crz_struct.tar_height = config_cp.crop_height;
        crz_struct.org_width = config_cp.crop_width;
        if (KAL_FALSE == idp_cp_crop_setting_in_use)
        {
            idp_cp_crop_width = config_cp.crop_width;
            idp_cp_crop_height = config_cp.crop_height;
            idp_cp_crop_setting_in_use = KAL_TRUE;
        }

        crz_struct.crop_en1 = config_cp.crop_en;
        idp_config_crz_crop(config_cp.image_src_width, config_cp.image_src_height, 
                            config_cp.crop_width, config_cp.crop_height);
        crz_struct.tile_mode = KAL_FALSE;
    }
    else
    {
        crz_struct.input_src1 = IDP_MODULE_IMGDMA_RDMA0;
        crz_struct.fstart1_intr_en = KAL_FALSE;
        crz_struct.auto_restart = KAL_FALSE;
        crz_struct.clip_en = KAL_TRUE;
        crz_struct.org_width = idp_cp_crop_width;

        if (1 == idp_cp_pass_num)
        {
            // tile pass 1 setting
            crz_struct.src_width = config_cp.tile_para.tile_src_size_x[0];
            crz_struct.src_height = idp_cp_crop_height;
            crz_struct.tar_width = config_cp.preview_width >> 1;
            crz_struct.tar_height = config_cp.preview_height;
            crz_struct.src_buff_y_addr = config_cp.cp_tmp_buffer;

            crz_struct.crop_en1 = KAL_TRUE;
            crz_struct.crop_left1 = config_cp.tile_para.tile_bi_sa_src_start_pos[0] - config_cp.tile_para.tile_adj_src_start_pos[0];
            crz_struct.crop_right1 = config_cp.tile_para.tile_bi_sa_src_end_pos[0] - config_cp.tile_para.tile_adj_src_start_pos[0];
            crz_struct.crop_top1 = 0;
            crz_struct.crop_bottom1 = idp_cp_crop_height - 1;

            // Tile Setting
            crz_struct.tile_mode = KAL_TRUE;
            crz_struct.tile_org_frame_width = idp_cp_crop_width;
            crz_struct.tile_org_tar_width = config_cp.preview_width;
            crz_struct.tile_start_pos_x = config_cp.tile_para.tile_start_pos_x[0];
            crz_struct.tile_start_pos_y = config_cp.tile_para.tile_start_pos_y[0];
            crz_struct.tile_trunc_err_comp_x = config_cp.tile_para.tile_trunc_err_comp_x[0];
            crz_struct.tile_trunc_err_comp_y = config_cp.tile_para.tile_trunc_err_comp_y[0];
            crz_struct.tile_resid_x = config_cp.tile_para.tile_init_resid_x[0];
            crz_struct.tile_resid_y = config_cp.tile_para.tile_init_resid_y[0];
        }
        else
        {
            // tile pass 2 setting
            crz_struct.src_width = config_cp.tile_para.tile_src_size_x[1];
            crz_struct.src_height = idp_cp_crop_height;
            crz_struct.tar_width = config_cp.preview_width >> 1;
            crz_struct.tar_height = config_cp.preview_height;
            crz_struct.src_buff_y_addr += ((idp_cp_crop_width - crz_struct.src_width) << 1);

            crz_struct.crop_en1 = KAL_TRUE;
            crz_struct.crop_left1 = config_cp.tile_para.tile_bi_sa_src_start_pos[1] - config_cp.tile_para.tile_adj_src_start_pos[1];
            crz_struct.crop_right1 = config_cp.tile_para.tile_bi_sa_src_end_pos[1] - config_cp.tile_para.tile_adj_src_start_pos[1];
            crz_struct.crop_top1 = 0;
            crz_struct.crop_bottom1 = idp_cp_crop_height - 1;

            // Tile Setting
            crz_struct.tile_mode = KAL_TRUE;
            crz_struct.tile_org_frame_width = idp_cp_crop_width;
            crz_struct.tile_org_tar_width = config_cp.preview_width;
            crz_struct.tile_start_pos_x = config_cp.tile_para.tile_start_pos_x[1];
            crz_struct.tile_start_pos_y = config_cp.tile_para.tile_start_pos_y[1];
            crz_struct.tile_trunc_err_comp_x = config_cp.tile_para.tile_trunc_err_comp_x[1];
            crz_struct.tile_trunc_err_comp_y = config_cp.tile_para.tile_trunc_err_comp_y[1];
            crz_struct.tile_resid_x = config_cp.tile_para.tile_init_resid_x[1];
            crz_struct.tile_resid_y = config_cp.tile_para.tile_init_resid_y[1];

            idp_cp_crop_setting_in_use = KAL_FALSE;
        }
    }
    idp_resz_crz_config_real(&crz_struct, KAL_TRUE);

    //config ROTDMA
    rotdma0_struct.src_width = crz_struct.tar_width;
    rotdma0_struct.src_height = crz_struct.tar_height;

    if (0 == idp_cp_pass_num)
    {
        rotdma0_struct.rot_angle = IMG_ROT_ANGLE_0;
        rotdma0_struct.y_dest_start_addr = config_cp.cp_tmp_buffer;
        rotdma0_struct.pitch_enable = KAL_FALSE;
    }
    else
    {
        rotdma0_struct.rot_angle = config_cp.rot_angle;
        rotdma0_struct.pitch_enable = KAL_TRUE;
        if (1 == idp_cp_pass_num)
        {
            if (0 == config_cp.preview_buffer_idx)  // Currently writing buffer 0 done.
            {
                rotdma0_struct.y_dest_start_addr = config_cp.frame_buffer_address;
            }
            else
            {
                rotdma0_struct.y_dest_start_addr = config_cp.frame_buffer_address1;
            }
        }
        else
        {
            if (IMG_ROT_ANGLE_0 == rotdma0_struct.rot_angle)
                rotdma0_struct.y_dest_start_addr += config_cp.preview_width;
            else
                rotdma0_struct.y_dest_start_addr += config_cp.preview_height * config_cp.preview_width;
        }
    }

    rotdma0_warm_reset();
    idp_imgdma_rotdma0_config_partial(&rotdma0_struct);

    //start pass
    idp_imgdma_rotdma0_start_real(&rotdma0_struct);
    idp_resz_crz_start_real(&crz_struct);
}
#endif

static kal_bool
_idp_scen_cp__hisr_rotdma0__frame_done(void *user_data)
{
#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    // do tile calculation when 1st pass is done
    if (KAL_TRUE == idp_cp_width_over_limit)
    {
        if (1 == idp_cp_pass_num)
        {
            if (idp_cp_crop_width * idp_cp_crop_height <= config_cp.preview_width * config_cp.preview_height)
            {
                idp_bi_tile_calc(idp_cp_crop_width, idp_cp_crop_height,
                                 config_cp.preview_width, config_cp.preview_height,
                                 &config_cp.tile_para);

                crz_struct.sa_en_x = KAL_FALSE;
                crz_struct.sa_en_y = KAL_FALSE;
            }
            else
            {
                idp_sa_tile_calc(idp_cp_crop_width, idp_cp_crop_height,
                                 config_cp.preview_width, config_cp.preview_height,
                                 &config_cp.tile_para);            

                crz_struct.sa_en_x = KAL_TRUE;
                crz_struct.sa_en_y = KAL_TRUE;
            }
        }
    
        idp_cp_config_and_start_next_pass();
    }
#endif

    // Triger display in frame done
#if defined(__ATV_SUPPORT__)
    if (KAL_TRUE != config_cp.trigger_display_delay_to_frame_start_enable)
#endif
#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    // change display buffer only at tile pass 2
    if ((KAL_FALSE == idp_cp_width_over_limit) ||
        (KAL_TRUE == idp_cp_width_over_limit && 
         0 == idp_cp_pass_num &&
         WFC_FALSE == wfcMMCheckBusy()))
#endif
    {
        idp_camera_preview_update_lcd_buffer();
    }

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    if (KAL_TRUE == idp_cp_width_over_limit)
    {
    if (2 == idp_cp_pass_num)
        idp_cp_pass_num = 0;
    else
        //update pass flags
        idp_cp_pass_num++;
    }
#endif

    return KAL_TRUE;
}
#endif  // #if defined(IDP_HISR_SUPPORT)

/****************************************************************************
 * local function bodies -- HW config functions
 ****************************************************************************/
static kal_bool
_idp_scen_cp__config_crz(
    idp_owner_t * const owner,
    kal_bool const config_to_hardware)
{
    kal_bool result;

    crz_struct.input_src1 = IDP_MODULE_OUTER_ISP;

    crz_struct.fstart1_intr_en = KAL_TRUE;
    crz_struct.fstart1_intr_cb = _idp_scen_cp__lisr_crz__frame_start;

    crz_struct.pixel_dropped_intr_en = KAL_TRUE;
    crz_struct.pixel_dropped_intr_cb = _idp_scen_cp__lisr_crz__pixel_dropped;

    crz_struct.lock_drop_frame_intr_en = KAL_TRUE;
    crz_struct.lock_drop_frame_intr_cb = _idp_scen_cp__lisr_crz__lock_drop_frame;

    crz_struct.src_width = config_cp.image_src_width;
    crz_struct.src_height = config_cp.image_src_height;

    printf("src_width=%d, src_height=%d\r\n", crz_struct.src_width, crz_struct.src_height);

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    if (KAL_TRUE == idp_cp_width_over_limit)
    {
        // set CRZ to bypass mode because of limitation
        crz_struct.tar_width = config_cp.crop_width;
        crz_struct.tar_height = config_cp.crop_height;
        crz_struct.mem_in_color_fmt1 = MM_IMAGE_COLOR_FORMAT_PACKET_UYVY422;
        crz_struct.src_buff_y_addr = config_cp.cp_tmp_buffer;
        crz_struct.org_width = config_cp.crop_width;
        if (KAL_FALSE == idp_cp_crop_setting_in_use)
        {
            idp_cp_crop_width = config_cp.crop_width;
            idp_cp_crop_height = config_cp.crop_height;            
            idp_cp_crop_setting_in_use = KAL_TRUE;
        }
    }
    else
#endif
    {
    crz_struct.tar_width = config_cp.preview_width;
    crz_struct.tar_height = config_cp.preview_height;
    }

    crz_struct.crop_en1 = config_cp.crop_en;
    crz_struct.auto_restart = KAL_TRUE;
    crz_struct.hw_frame_sync = KAL_TRUE;

    //if (KAL_TRUE == crz_struct.crop_en1) // Always TRUE, code shrink
    {
        idp_config_crz_crop(config_cp.image_src_width, config_cp.image_src_height, config_cp.crop_width, config_cp.crop_height);
    }

#if defined(MT6252H) || defined(MT6252)
    if (crz_struct.tar_width >= 320)
    {
        _idp_scen_crz_over_limit_width = crz_struct.tar_width;
        crz_struct.tar_width = 320;
        _idp_scen_crz_width_over_limit = KAL_TRUE;

        // change crop for same width/hieght rate.
        crz_struct.crop_left1 = (config_cp.image_src_width - config_cp.crop_width*320/_idp_scen_crz_over_limit_width) >> 1;
        crz_struct.crop_right1 = crz_struct.crop_left1 + config_cp.crop_width*320/_idp_scen_crz_over_limit_width - 1;
    }
#endif

    result = idp_resz_crz_config(owner->crz_key, &crz_struct, config_to_hardware);
    if (KAL_FALSE == result)
    {
        return KAL_FALSE;
    }

    return KAL_TRUE;
}


static kal_bool
_idp_scen_cp__config_rotdma0(
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
    if (KAL_TRUE == idp_cp_width_over_limit)
    {
        if (NULL == config_cp.cp_tmp_buffer)
        {
            return KAL_FALSE;
        }
        else
        {
            rotdma0_struct.y_dest_start_addr = config_cp.cp_tmp_buffer;
            if (IMG_ROT_ANGLE_0 == config_cp.rot_angle)
                rotdma0_struct.pitch_bytes = (config_cp.preview_width << 1); //Final width
            else
                rotdma0_struct.pitch_bytes = (config_cp.preview_height << 1); //Final width
        }
    }
    else
#endif
    rotdma0_struct.y_dest_start_addr = config_cp.frame_buffer_address;

    rotdma0_struct.src_width = crz_struct.tar_width;
    rotdma0_struct.src_height = crz_struct.tar_height;

    rotdma0_struct.fend_intr_en = KAL_TRUE;
    rotdma0_struct.fend_intr_cb = _idp_scen_cp__lisr_rotdma0__frame_done;

    rotdma0_struct.hw_frame_sync = KAL_TRUE;

    rotdma0_struct.protect_en = KAL_TRUE;   //According to spec. From Camera, enable protect
    rotdma0_struct.bus_control_threshold = ROTDMA_BUS_CONTROL_THRESHOLD;

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    if (KAL_TRUE == idp_cp_width_over_limit)
        rotdma0_struct.rot_angle = IMG_ROT_ANGLE_0;
    else
#endif
    rotdma0_struct.rot_angle = config_cp.rot_angle;

#if defined(MT6252H) || defined(MT6252)
    // reset start address, clear write buffer to black
    if (_idp_scen_crz_width_over_limit == KAL_TRUE)
    {
        kal_uint32 i, size;
        kal_uint32* pu4Addr1= (kal_uint32*) config_cp.frame_buffer_address;
        kal_uint32* pu4Addr2= (kal_uint32*) config_cp.frame_buffer_address1;

        rotdma0_struct.y_dest_start_addr += (_idp_scen_crz_over_limit_width-320)*crz_struct.tar_height;

        size = (_idp_scen_crz_over_limit_width*crz_struct.tar_height) >> 1;
        for (i = 0; i <size; i++)
        {
            *(pu4Addr1 + i) = 0x00800080;
            *(pu4Addr2 + i) = 0x00800080;
        }
    }
#endif

    result = idp_imgdma_rotdma0_config(owner->irt0_key, &rotdma0_struct, config_to_hardware);
    if (KAL_FALSE == result)
    {
        return KAL_FALSE;
    }

    owner->have_config_imgdma_irt0 = KAL_TRUE;

#if defined(MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
    if (KAL_TRUE == _idp_scen_triple_display_buffer_enable)
    {
        //record the 3 display buffers
        _idp_scen_display_buffer_addr[0] = config_cp.frame_buffer_address;
        _idp_scen_display_buffer_addr[1] = config_cp.frame_buffer_address1;
        _idp_scen_display_buffer_addr[2] = config_cp.frame_buffer_address2;
        _idp_scen_display_buffer_cnt = 3;
    }
#endif

    return KAL_TRUE;
}


/****************************************************************************
 * local function bodies -- IDP scenario hook function bodies
 ****************************************************************************/
static void
idp_camera_preview_init_config_hook(
    idp_owner_t * const owner)
{

#if defined(IDP_HISR_SUPPORT)
    if (idp_camera_preview_frame_start_hisr_handle == NULL)
    {
        idp_camera_preview_frame_start_hisr_handle =
            idp_hisr_register(owner->key,
                              IDP_HISR_CB_TYPE_RESZ,
                              _idp_scen_cp__hisr_crz__frame_start,
                              NULL);
    }

    if (idp_camera_preview_display_frame_done_hisr_handle == NULL)
    {
        idp_camera_preview_display_frame_done_hisr_handle =
            idp_hisr_register(owner->key,
                              IDP_HISR_CB_TYPE_IMGDMA,
                              _idp_scen_cp__hisr_rotdma0__frame_done,
                              NULL);
    }
#endif

#if defined(MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
    _idp_scen_triple_display_buffer_enable = KAL_FALSE;
#endif

#if defined(__ATV_SUPPORT__)
    completed_display_buffer_count = 0;
#endif

    kal_mem_set(&config_cp, 0, sizeof(config_cp));

#if 0 //defined(IDP_MM_COLOR_SUPPORT)
    colorWrapperOpen(&mm_color_handle, COLOR_SCENARIO_CAMERA_PREVIEW);
    colorWrapperEnable(mm_color_handle, 0);
#endif

#if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
    idp_cp_crop_setting_in_use = KAL_FALSE;
    idp_cp_pass_num = 1;
#endif

}

kal_bool
idp_camera_preview_close_hook(
    idp_owner_t * const owner)
{

    idp_resz_crz_close(owner->crz_key, &crz_struct);
    idp_imgdma_rotdma0_close(owner->irt0_key, &rotdma0_struct);

#if defined(IDP_HISR_SUPPORT)
    idp_hisr_clear_all(owner->key, IDP_HISR_CB_TYPE_RESZ);
    idp_hisr_clear_all(owner->key, IDP_HISR_CB_TYPE_IMGDMA);

    idp_camera_preview_frame_start_hisr_handle = NULL;
    idp_camera_preview_display_frame_done_hisr_handle = NULL;
#endif

#if 0 //defined(IDP_MM_COLOR_SUPPORT)
    colorWrapperDisable(mm_color_handle);
    colorWrapperClose(mm_color_handle);
#endif


    return KAL_TRUE;
}

kal_bool
idp_camera_preview_read_user_config_fast_hook(
    idp_owner_t * const owner,
    kal_uint32 const para_type,
    kal_uint32 const para_value)
{

    switch (para_type)
    {
      #if defined(IDP_HISR_SUPPORT)
        case IDP_CAMERA_PREVIEW_FRAME_START_HISR_CB_ENABLE:
            {
                // If this is enabled, CRZ LISR will activate a registered HISR to invoke camera DZ...
                //kal_uint32 const savedMask = SaveAndSetIRQMask();
                config_cp.enable_crz_frame_start_intr = (kal_bool) para_value;
                //RestoreIRQMask(savedMask);
            }
            break;
      #endif
        default:
            return KAL_FALSE;
            //break;
    }

    return KAL_TRUE;
}

kal_bool
idp_camera_preview_read_user_config_hook(
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

            case IDP_CAMERA_PREVIEW_IMAGE_SRC_WIDTH:
                config_cp.image_src_width = va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_PREVIEW_IMAGE_SRC_HEIGHT:
                config_cp.image_src_height = va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_PREVIEW_PREVIEW_WIDTH:
                config_cp.preview_width = va_arg(ap, kal_uint32);
            #if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
                if (480 < config_cp.preview_width)
                {
                    idp_cp_width_over_limit = KAL_TRUE;
                }
                else
                {
                    idp_cp_width_over_limit = KAL_FALSE;
                }
            #endif
                break;

            case IDP_CAMERA_PREVIEW_PREVIEW_HEIGHT:
                config_cp.preview_height = va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_PREVIEW_FRAME_BUFFER_ADDRESS:
                config_cp.frame_buffer_address = va_arg(ap, kal_uint32);
                //config_cp.frame_buffer_address &= ~0x10000000;
                break;

            case IDP_CAMERA_PREVIEW_FRAME_BUFFER_ADDRESS1:
                config_cp.frame_buffer_address1 = va_arg(ap, kal_uint32);
                //config_cp.frame_buffer_address1 &= ~0x10000000;
                break;

        #if defined(MDP_SUPPORT_CP_TRIPLE_DISPLAY_BUFFER)
            case IDP_CAMERA_PREVIEW_FRAME_BUFFER_ADDRESS2:
                config_cp.frame_buffer_address2 = va_arg(ap, kal_uint32);

                if (NULL != config_cp.frame_buffer_address2)
                {
                    _idp_scen_triple_display_buffer_enable = KAL_TRUE;
                }
                else
                {
                    _idp_scen_triple_display_buffer_enable = KAL_FALSE;
                }
                break;
        #endif

            case IDP_CAMERA_PREVIEW_DISPLAY_ROT_ANGLE:
                config_cp.rot_angle = (img_rot_angle_enum_t) va_arg(ap, kal_uint32);

                if (!((IMG_ROT_ANGLE_0 == config_cp.rot_angle) ||
                    (IMG_MIRROR_ROT_ANGLE_90 == config_cp.rot_angle)))
                {
                    return KAL_FALSE;
                }
                break;

            case IDP_CAMERA_PREVIEW_ENABLE_TRIGGER_LCD:
                config_cp.enable_trigger_lcd = (kal_bool) va_arg(ap, kal_uint32);
                break;

        #if defined(IDP_HISR_SUPPORT)
            case IDP_CAMERA_PREVIEW_FRAME_START_HISR_CB_ENABLE:
                {
                    //kal_uint32 const savedMask = SaveAndSetIRQMask();
                    config_cp.enable_crz_frame_start_intr = (kal_bool) va_arg(ap, kal_uint32);
                    //RestoreIRQMask(savedMask);
                }
                break;
        #endif

            case IDP_CAMERA_PREVIEW_PREVIEW_CROP_ENABLE:
                config_cp.crop_en = (kal_bool) va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_PREVIEW_PREVIEW_CROP_WIDTH:
                config_cp.crop_width = va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_PREVIEW_PREVIEW_CROP_HEIGHT:
                config_cp.crop_height = va_arg(ap, kal_uint32);
                break;

            case IDP_CAMERA_PREVIEW_ENABLE_DOUBLE_BUFFER_TEARING_FREE:
                config_cp.dbuf_tearing_free_enable = (kal_bool)va_arg(ap, kal_uint32);  
                break;

        #if defined(__ATV_SUPPORT__)
            case IDP_CAMERA_PREVIEW_ENABLE_TRIGGER_DISPLAY_DELAY_TO_FRAME_START:
                config_cp.trigger_display_delay_to_frame_start_enable = (kal_bool) va_arg(ap, kal_uint32);
              #if defined(MT6252H) || defined(MT6252)
                // MT6252 with 5193 MATV will drop frame when LCD and MDP work at the same time.
                // So, default we not delay, let frame done trigger LCD display
                config_cp.trigger_display_delay_to_frame_start_enable = KAL_FALSE;
              #endif
                break;
        #endif

            case IDP_CAMERA_PREVIEW_ENG_CONFIG:
                config_cp.end_config = (kal_bool) va_arg(ap, kal_uint32);
                break;

        #if defined(IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT)
            case IDP_CAMERA_PREVIEW_TEMP_BUFFER:
                config_cp.cp_tmp_buffer = (kal_uint32) va_arg(ap, kal_uint32);
                break;
        #endif
        
            default:
                va_arg(ap, kal_uint32);
                break;
        }
    }

    return KAL_TRUE;
}


kal_bool
idp_camera_preview_config_hook(
    idp_owner_t * const owner,
    kal_bool const config_to_hardware)
{
    kal_bool result = KAL_TRUE;

    if (config_cp.end_config == KAL_FALSE)
    {
        return KAL_TRUE;
    }

    result = _idp_scen_cp__config_crz(owner, config_to_hardware);
    if (KAL_FALSE == result)
    {
        return KAL_FALSE;
    }

    result = _idp_scen_cp__config_rotdma0(owner, config_to_hardware);
    if (KAL_FALSE == result)
    {
        return KAL_FALSE;
    }

    return result;
}

static hook_collect_t hook_collect =
{
    idp_camera_preview_init_config_hook,
    idp_common_open_hook,//idp_camera_preview_open_hook,
    idp_camera_preview_close_hook,
    idp_common_stop_hook,//idp_camera_preview_stop_hook,
    idp_camera_preview_read_user_config_hook,
    idp_camera_preview_read_user_config_fast_hook,
    idp_dummy_hook3,//idp_camera_preview_finish_read_user_config_hook,
    idp_dummy_hook,//idp_camera_preview_hardware_setting_have_been_changed_hook,
    idp_camera_preview_config_hook,
    idp_common_query_hook,//idp_camera_preview_query_hook,
    idp_dummy_hook,//idp_camera_preview_start_intermedia_pipe_hook,
    idp_common_start_input_hook,//idp_camera_preview_start_input_hook,
    idp_common_start_output_hook,//idp_camera_preview_start_output_hook,
    idp_dummy_hook2,//idp_camera_preview_compute_mem_for_each_hw_hook,
    idp_common_is_busy_hook,//idp_camera_preview_is_busy_hook,
    idp_common_is_in_use_hook,//idp_camera_preview_is_in_use_hook,
    idp_dummy_hook2,//idp_camera_preview_clear_setting_diff_hook,
    idp_dummy_hook//idp_camera_preview_is_setting_diff_is_ok_when_busy_hook
};





static IdpCheckReentrant idp_camera_preview_check_reentrant = {KAL_NILTASK_ID, IDP_CALL_BY_HISR, IDP_CHECK_REENTRANT_PREV_OPEN, KAL_FALSE};

kal_bool
idp_camera_preview_open_real(
    kal_uint32 * const key,
    char const * const filename,
    kal_uint32 const lineno)
{
    return idp_scenario_open(key,
                             &idp_camera_preview_check_reentrant,
                             IDP_TRACED_API_camera_preview_OPEN,
                             IDP_SCENARIO_camera_preview,
                             &hook_collect);
}

kal_bool
idp_camera_preview_is_in_use(void)
{
    return idp_scenario_is_in_use(&idp_camera_preview_check_reentrant,
                                  IDP_TRACED_API_camera_preview_IS_IN_USE,
                                  &hook_collect);
}

kal_bool
idp_camera_preview_close(
    kal_uint32 const key)
{
    return idp_scenario_close(key,
                              &idp_camera_preview_check_reentrant,
                              IDP_TRACED_API_camera_preview_CLOSE,
                              &hook_collect);
}

kal_bool
idp_camera_preview_stop(
    kal_uint32 const key)
{
    return idp_scenario_stop(key,
                             &idp_camera_preview_check_reentrant,
                             IDP_TRACED_API_camera_preview_STOP,
                             &hook_collect);
}

kal_bool
idp_camera_preview_start_intermedia_pipe(
    kal_uint32 const key)
{
    return idp_scenario_start_intermedia_pipe(key,
                                              &idp_camera_preview_check_reentrant,
                                              IDP_TRACED_API_camera_preview_START_INTERMEDIA_PIPE,
                                              &hook_collect);
}

kal_bool
idp_camera_preview_start_input(
    kal_uint32 const key)
{
    return idp_scenario_start_input(key,
                                    &idp_camera_preview_check_reentrant,
                                    IDP_TRACED_API_camera_preview_START_INPUT,
                                    &hook_collect);
}

kal_bool
idp_camera_preview_start_output(
    kal_uint32 const key)
{
    return idp_scenario_start_output(key,
                                     &idp_camera_preview_check_reentrant,
                                     IDP_TRACED_API_camera_preview_START_OUTPUT,
                                     &hook_collect);
}

kal_bool
idp_camera_preview_start_all(
    kal_uint32 const key)
{
    return idp_scenario_start_all(key,
                                  &idp_camera_preview_check_reentrant,
                                  IDP_TRACED_API_camera_preview_START_ALL,
                                  &hook_collect);
}

kal_bool
idp_camera_preview_is_busy(
    kal_uint32 const key,
    kal_bool * const busy)
{
    return idp_scenario_is_busy(key,
                                busy,
                                IDP_TRACED_API_camera_preview_IS_BUSY,
                                &hook_collect);
}

kal_bool
idp_camera_preview_config_fast(
    kal_uint32 const key,
    kal_uint32 const para_type,
    kal_uint32 const para_value)
{
    return idp_scenario_config_fast(key,
                                    para_type,
                                    para_value,
                                    IDP_TRACED_API_camera_preview_CONFIG_FAST,
                                    &hook_collect);
}

kal_bool
idp_camera_preview_config(
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
                                 &idp_camera_preview_check_reentrant,
                                 IDP_TRACED_API_camera_preview_CONFIG,
                                 &hook_collect,
                                 ap);
    va_end(ap);
    return result;

}

kal_bool
idp_camera_preview_configurable(
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
                                       &idp_camera_preview_check_reentrant,
                                       IDP_TRACED_API_camera_preview_CONFIGURABLE,
                                       &hook_collect,
                                       ap);
    va_end(ap);
    return result;
}


kal_bool
idp_camera_preview_query(
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
                                &idp_camera_preview_check_reentrant,
                                IDP_TRACED_API_camera_preview_QUERY,
                                &hook_collect,
                                ap);
    va_end(ap);
    return result;
}


kal_bool idp_camera_preview_config_and_start(
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
                                           &idp_camera_preview_check_reentrant,
                                           IDP_TRACED_API_camera_preview_CONFIG_AND_START,
                                           &hook_collect,
                                           ap);
    va_end(ap);
    return result;
}


#endif  // #if defined(DRV_IDP_6252_SERIES)

