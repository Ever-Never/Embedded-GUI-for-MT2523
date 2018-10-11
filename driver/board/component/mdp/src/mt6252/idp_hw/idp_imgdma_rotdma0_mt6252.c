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

#define THIS_FILE_BELONGS_TO_IMAGE_DATA_PATH_MODULE_INTERNAL

#include <idp_define.h>

#if defined(DRV_IDP_6252_SERIES)

#include <idp_core.h>
#include <mt6252/idp_hw/idp_imgdma_rotdma0.h>
#include "mm_comm_def.h"

#ifdef MDP_C_MODEL
EXTERN
{
#endif
#include <mt6252/idp_mem.h>
#ifdef MDP_C_MODEL
}
#endif

#ifdef MDP_C_MODEL
#include "rot_dma_reg.h"
#endif

#include "hal_log.h"

static idp_hw_owner_t owner;

void *idp_imgdma_rotdma0_fend_intr_cb_param;
void (*idp_imgdma_rotdma0_fend_intr_cb)(void *);


#if defined(NO_SHRINK_ROTDMA_CODE)
void *idp_imgdma_rotdma0_q_empty_intr_cb_param;
void (*idp_imgdma_rotdma0_q_empty_intr_cb)(void *);
#endif


void
idp_imgdma_rotdma0_LISR(void)
{
    kal_uint32 reg;

#if !defined(MDP_C_MODEL)
    //IRQMask(IRQ_ROT_DMA_CODE);
#endif

    reg = REG_IMGDMA_ROT_DMA_IRQ_FLAG & 0x00FF;

    if (reg & IMGDMA_ROTDMA_IRQ_FLAG_0_BIT)
    {
        if (idp_imgdma_rotdma0_fend_intr_cb != NULL)
        {
            idp_imgdma_rotdma0_fend_intr_cb(idp_imgdma_rotdma0_fend_intr_cb_param);
        }
    }

#if defined(NO_SHRINK_ROTDMA_CODE)
    if (reg & IMGDMA_ROTDMA_IRQ_FLAG_2_BIT)
    {
        if (idp_imgdma_rotdma0_q_empty_intr_cb != NULL)
        {
            idp_imgdma_rotdma0_q_empty_intr_cb(idp_imgdma_rotdma0_q_empty_intr_cb_param);
        }
    }
#endif

    REG_IMGDMA_ROT_DMA_IRQ_FLAG_CLR = reg; //write clear

#if !defined(MDP_C_MODEL)
    //IRQUnmask(IRQ_ROT_DMA_CODE);
#endif
}


/**
 * ROTDMA warm reset function.
 *
 * @param none.
 *
 * @return.
 */
void rotdma0_warm_reset(void)
{
    kal_uint32  irq_flag;
    kal_uint32  cfg;

    // First backup control register due to control register would be clear after warm reset
    irq_flag = REG_IMGDMA_ROT_DMA_IRQ_FLAG & 0xFFFF0000;
    cfg = REG_IMGDMA_ROT_DMA_CFG;

    IMGDMA_ROTDMA_WARM_RESET();

    {
        kal_bool  rst_done;
        //kal_uint32 last_time = drv_get_current_time();
        kal_uint32 wait_count = 0;

        // Check if rotdma reset busy, if busy over 2 tick, assert... according to Andy, < 4us (200 cycle @52Mhz)
        do
        {
            rst_done = IMGDMA_ROTDMA_IS_WARM_RESET_FINISH();
	    wait_count++;
        } while ((KAL_FALSE == rst_done) &&
                 /*(2 >= drv_get_duration_tick(last_time, drv_get_current_time()))*/
                 (2000 >= wait_count));
    }

    // Restore control register
    REG_IMGDMA_ROT_DMA_IRQ_FLAG = irq_flag;
    REG_IMGDMA_ROT_DMA_CFG = cfg;
}


/**
 * rotdma_check_size_limitation.
 * Check if the source/target size is not over the limitation of MT6252 ROTDMA...
 *
 * @param source_width
 * @param source_height
 * @param tar_yuv_color_fmt
 * @param rot_angle
 *
 * @return KAL_TRUE, the set of sizes is OK. KAL_FALSE, o.w.
 */
kal_bool
rotdma_check_size_limitation(
    kal_uint32 source_width,
    kal_uint32 source_height,
    MM_IMAGE_COLOR_FORMAT_ENUM    tar_yuv_color_fmt,
    img_rot_angle_enum_t          rot_angle)
{
//#if defined(MT6252H) || defined(MT6252)
    // the upper limit of size
//    if ( (0 == source_width)
//        || (0 == source_height)
//        || ((IMG_MIRROR_ROT_ANGLE_90 == rot_angle) && (320 < source_width))
//        || ((IMG_ROT_ANGLE_0 == rot_angle) && (1023 < source_width))
//        || (1023 < source_height))
//    {
//        return KAL_FALSE;
//    }
//#elif defined(MT6250) || defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)
    // the upper limit of size
    if ( (0 == source_width)
        || (0 == source_height)
        || ((IMG_MIRROR_ROT_ANGLE_90 == rot_angle) && (480 < source_width))
        || ((IMG_ROT_ANGLE_0 == rot_angle) && (2047 < source_width))
        || (1023 < source_height))
    {
        return KAL_FALSE;
    }
//#endif

    // check target size alignment, rotdma src_w = tar_w, src_h = tar_h
    if ((0 != (source_width & 0x1))   //rotdma target format only UYVY422 YUV422 YUV420 (source_width = target_width)
        || (MM_IMAGE_COLOR_FORMAT_YUV420 == tar_yuv_color_fmt) && (0 != (source_height & 0x1)))
    {
        return KAL_FALSE;
    }

    return KAL_TRUE;
}


void
idp_imgdma_rotdma0_open_real(void)
{
    hal_clock_status_t ret = HAL_CLOCK_STATUS_ERROR;

    // lock MTCMOS before calling CG on API
    cm4_topsm_lock_MM_MTCMOS();
    ret = ROTDMA_POWERON();
    if (ret != HAL_CLOCK_STATUS_OK)
	log_hal_info("error for enable rotdma CG, ret:%d", ret);
    // reset first
    rotdma0_warm_reset();
}

void
idp_imgdma_rotdma0_close_real(void)
{
    hal_clock_status_t ret = HAL_CLOCK_STATUS_ERROR;
    rotdma0_warm_reset();
    ret = ROTDMA_POWEROFF();
    if (ret != HAL_CLOCK_STATUS_OK)
	log_hal_info("error for enable rotdma CG, ret:%d", ret);
    cm4_topsm_unlock_MM_MTCMOS();
    // unlock MTCMOS after calling CG off API
}


/** \brief Programmers can use this function to config ROTDMA0
 *
 * \ingroup imgdma_rotdma0
 *
 * Before using this function, IRT1 should be turned off.
 *
 * \param rotdma0_struct IRT1 config structure
 */
kal_bool
idp_imgdma_rotdma0_config_real(
    idp_imgdma_rotdma0_struct * const p_rotdma0_struct,
    kal_bool const config_to_hardware)
{
    kal_uint32 dst_size_weight = 0;

    //warm reset
    rotdma0_warm_reset();

    if (KAL_TRUE == p_rotdma0_struct->drop_prev_engine_when_off)
    {
        IMGDMA_ROTDMA_DROP_PREV_ENGINE();
    }
    else
    {
        IMGDMA_ROTDMA0_STALL_PREV_ENGINE();
    }

    //enable int
    IMGDMA_ROTDMA_INTERRUPT_DISABLE();
    if (KAL_TRUE == p_rotdma0_struct->fend_intr_en) /* enable int or not */
    {
        IMGDMA_ROTDMA_FEND_INTERRUPT_ENABLE();
        idp_imgdma_rotdma0_fend_intr_cb = p_rotdma0_struct->fend_intr_cb;
        idp_imgdma_rotdma0_fend_intr_cb_param = p_rotdma0_struct->fend_intr_cb_param;
    }
    else
    {
        IMGDMA_ROTDMA_FEND_INTERRUPT_DISABLE();
        //idp_imgdma_rotdma0_fend_intr_cb = NULL;
        //idp_imgdma_rotdma0_fend_intr_cb_param = NULL;
    }

#if defined(NO_SHRINK_ROTDMA_CODE)
    if (KAL_TRUE == p_rotdma0_struct->q_empty_intr_en) /* enable int or not */
    {
        IMGDMA_ROTDMA_Q_EMPTY_DROP_INTERRUPT_ENABLE();
        idp_imgdma_rotdma0_q_empty_intr_cb = p_rotdma0_struct->q_empty_intr_cb;
        idp_imgdma_rotdma0_q_empty_intr_cb_param = p_rotdma0_struct->q_empty_intr_cb_param;
    }
    else
    {
        IMGDMA_ROTDMA_Q_EMPTY_DROP_INTERRUPT_DISABLE();
        //idp_imgdma_rotdma0_q_empty_intr_cb = NULL;
        //idp_imgdma_rotdma0_q_empty_intr_cb_param = NULL;
    }
#else
    IMGDMA_ROTDMA_Q_EMPTY_DROP_INTERRUPT_DISABLE();
#endif

#if defined(NO_SHRINK_ROTDMA_CODE)
    if (KAL_TRUE == p_rotdma0_struct->slow_down_en)
    {
        IMGDMA_ROTDMA_SLOW_DOWN_ENABLE();
        IMGDMA_ROTDMA_SET_SLOW_DOWN_COUNT(p_rotdma0_struct->slow_down_cnt);
    }
    else
#endif
    {
        IMGDMA_ROTDMA_SLOW_DOWN_DISABLE();
    }

    IMGDMA_ROTDMA_SET_BUS_THRESHOLD(p_rotdma0_struct->bus_control_threshold);

#if defined(NO_SHRINK_ROTDMA_CODE)
    /// register/descriptor mode
    if (KAL_TRUE == p_rotdma0_struct->descriptor_mode)
    {
        kal_uint32 des_q_depth, write_q_data_loop = 0;
        kal_uint32 *des_queue_data_addr;

        IMGDMA_ROTDMA_DESCRIPTOR_MODE();
        // queue depth
        des_q_depth = p_rotdma0_struct->queue_depth;
        IMGDMA_ROTDMA_SET_QUEUE_DEPTH(des_q_depth - 1);

        write_q_data_loop = des_q_depth * IMGDMA_ROTDMA_DES_EACH_Q_SIZE;
        des_queue_data_addr = p_rotdma0_struct->des_queue_data_addr;
        while (write_q_data_loop--)
        {
            while(0x0 == (REG_IMGDMA_ROT_DMA_QUEUE_WSTA&0x1)) {}
            REG_IMGDMA_ROT_DMA_QUEUE_DATA = *des_queue_data_addr++;
        }

    }
    else
#endif
    {
        IMGDMA_ROTDMA_REGISTER_MODE();
    }

#if defined(NO_SHRINK_ROTDMA_CODE)
    if (p_rotdma0_struct->queue_empty_drop == KAL_TRUE)
    {
        IMGDMA_ROTDMA_QUEUE_EMPTY_DROP();
    }
    else
#endif
    {
        IMGDMA_ROTDMA_QUEUE_EMPTY_STALL();
    }

    // camera frame sync mode.
    if (KAL_TRUE == p_rotdma0_struct->hw_frame_sync)
    {
        IMGDMA_ROTDMA_FRAME_SYNC_ENABLE();
    }
    else
    {
        IMGDMA_ROTDMA_FRAME_SYNC_DISABLE();
    }


    if (KAL_TRUE == p_rotdma0_struct->auto_loop)
    {
        IMGDMA_ROTDMA_AUTO_LOOP_ENABLE();
    }
    else
    {
        IMGDMA_ROTDMA_AUTO_LOOP_DISABLE();
    }

#if defined(NO_SHRINK_ROTDMA_CODE)
    // mt6252, descriptor mode must set all y,u,v_dst_str_addr, src-w, src_h, dst_w_in_byte, con
    if (KAL_TRUE == p_rotdma0_struct->descriptor_mode)
    {
        return KAL_TRUE;
    }
#endif

    /// NOTICE:
    /// below HW register parameters only need to fill in non-descriptor mode (register mode), otherwise, must fill in descriptor
    if (KAL_TRUE == p_rotdma0_struct->protect_en)
    {
        IMGDMA_ROTDMA_PROTECT_ENABLE();
    }
    else
    {
        IMGDMA_ROTDMA_PROTECT_DISABLE();
    }

    if (p_rotdma0_struct->ultra_high_en)
    {
        IMGDMA_ROTDMA_ULTRA_HIGH_ENABLE();
    }
    else
    {
        IMGDMA_ROTDMA_ULTRA_HIGH_DISABLE();
    }


    // clipping --> mt6252 has no target clipping

    // color format
    // mt6252 only support yuv format (yuyv422 or planar yuv422/yuv420)
    switch (p_rotdma0_struct->yuv_color_fmt)
    {
        case MM_IMAGE_COLOR_FORMAT_YUV422:
            IMGDMA_ROTDMA_SET_OUTPUT_YUV422();
            dst_size_weight = 1;//from spec
            break;

        case MM_IMAGE_COLOR_FORMAT_YUV420:
            IMGDMA_ROTDMA_SET_OUTPUT_YUV420();
            dst_size_weight = 1;//from spec
            break;

        case MM_IMAGE_COLOR_FORMAT_PACKET_UYVY422: /**equal to UYVY in spec  */
            IMGDMA_ROTDMA_SET_OUTPUT_YUYV422();
            dst_size_weight = 2;//from spec.
            break;

        default:
            break;
    }

    // destination start addresses
    REG_IMGDMA_ROT_DMA_Y_DST_STR_ADDR = p_rotdma0_struct->y_dest_start_addr;
    REG_IMGDMA_ROT_DMA_U_DST_STR_ADDR = p_rotdma0_struct->u_dest_start_addr;
    REG_IMGDMA_ROT_DMA_V_DST_STR_ADDR = p_rotdma0_struct->v_dest_start_addr;

    // source width & height
    REG_IMGDMA_ROT_DMA_SRC_SIZE = (p_rotdma0_struct->src_width | (p_rotdma0_struct->src_height << 16));


    switch (p_rotdma0_struct->rot_angle)
    {
        case IMG_ROT_ANGLE_0:
            IMGDMA_ROTDMA_ROT_0();
            break;

        case IMG_MIRROR_ROT_ANGLE_90:
            IMGDMA_ROTDMA_ROT_90CW_WITH_FLIP();
            break;

        default:
            break;
    }

    // destination width in bytes
    if (IMG_MIRROR_ROT_ANGLE_90 == p_rotdma0_struct->rot_angle)
    {
        REG_IMGDMA_ROT_DMA_DST_SIZE = p_rotdma0_struct->src_height * dst_size_weight;
    }
    else
    {
        REG_IMGDMA_ROT_DMA_DST_SIZE = p_rotdma0_struct->src_width * dst_size_weight;
    }

    //output pitch
    #if defined(IDP_TILE_MODE_SUPPORT)
    IMGDMA_ROTDMA_YUV_PITCH_DISABLE();
    #endif
    if (p_rotdma0_struct->pitch_enable)
    {
        REG_IMGDMA_ROT_DMA_DST_SIZE = p_rotdma0_struct->pitch_bytes;

      #if defined(IDP_TILE_MODE_SUPPORT)
        IMGDMA_ROTDMA_YUV_PITCH_ENABLE();

        if (((p_rotdma0_struct->src_width & 0x07) != 0) && 
            ((p_rotdma0_struct->yuv_color_fmt == MM_IMAGE_COLOR_FORMAT_YUV420) 
              || (p_rotdma0_struct->yuv_color_fmt == MM_IMAGE_COLOR_FORMAT_YUV422)))
        {
            ASSERT(0);
        }
      #endif
    }

    return KAL_TRUE;
}

/** \brief Programmers can use this function to partial config ROTDMA0
 *
 * \ingroup imgdma_rotdma0
 *
 * Before using this function, IRT1 need not be turned off.
 *
 * \param rotdma0_struct IRT1 config structure
 */
kal_bool
idp_imgdma_rotdma0_config_partial(
    idp_imgdma_rotdma0_struct * const p_rotdma_struct)
{
    kal_uint32 dst_size_weight = 0;

    switch (p_rotdma_struct->yuv_color_fmt)
    {
        case MM_IMAGE_COLOR_FORMAT_YUV420:
            IMGDMA_ROTDMA_SET_OUTPUT_YUV420();
            dst_size_weight = 1;//from spec
            break;

        case MM_IMAGE_COLOR_FORMAT_PACKET_UYVY422: /**equal to UYVY in spec  */
            IMGDMA_ROTDMA_SET_OUTPUT_YUYV422();
            dst_size_weight = 2;//from spec.
            break;

        default:
            break;
    }

    // destination start addresses
    if ((p_rotdma_struct->y_dest_start_addr & 0x03) != 0)
    {
        ASSERT(0);
    }
    REG_IMGDMA_ROT_DMA_Y_DST_STR_ADDR = p_rotdma_struct->y_dest_start_addr;
    if (p_rotdma_struct->yuv_color_fmt != MM_IMAGE_COLOR_FORMAT_PACKET_UYVY422)
    {
        if (((p_rotdma_struct->u_dest_start_addr & 0x03) != 0) ||
            ((p_rotdma_struct->v_dest_start_addr & 0x03) != 0))
        {
            ASSERT(0);
        }
        REG_IMGDMA_ROT_DMA_U_DST_STR_ADDR = p_rotdma_struct->u_dest_start_addr;
        REG_IMGDMA_ROT_DMA_V_DST_STR_ADDR = p_rotdma_struct->v_dest_start_addr;
    }

    // source width & height
    REG_IMGDMA_ROT_DMA_SRC_SIZE = (p_rotdma_struct->src_width | (p_rotdma_struct->src_height << 16));

    switch (p_rotdma_struct->rot_angle)
    {
        case IMG_ROT_ANGLE_0:
            IMGDMA_ROTDMA_ROT_0();
            break;

        case IMG_MIRROR_ROT_ANGLE_90:
            IMGDMA_ROTDMA_ROT_90CW_WITH_FLIP();
            break;

        default:
            ASSERT(0);
            break;
    }

    // destination width in bytes
    if (IMG_MIRROR_ROT_ANGLE_90 == p_rotdma_struct->rot_angle)
    {
        REG_IMGDMA_ROT_DMA_DST_SIZE = p_rotdma_struct->src_height * dst_size_weight;
    }
    else
    {
        REG_IMGDMA_ROT_DMA_DST_SIZE = p_rotdma_struct->src_width * dst_size_weight;
    }

    return KAL_TRUE;
}

kal_bool
idp_imgdma_rotdma0_start_real(
    idp_imgdma_rotdma0_struct const * const p_rotdma0_struct)
{
    IMGDMA_ROTDMA_START();

    return KAL_TRUE;
}

kal_bool
idp_imgdma_rotdma0_stop_real(
    idp_imgdma_rotdma0_struct const * const p_rotdma0_struct)
{
    rotdma0_warm_reset();

    return KAL_TRUE;
}

kal_bool
idp_imgdma_rotdma0_is_busy_real(
    kal_bool * const p_busy,
    idp_imgdma_rotdma0_struct const * const p_rotdma0_struct)
{
    (*p_busy) = IMGDMA_ROTDMA_IS_ENABLED();

    return KAL_TRUE;
}




kal_bool
idp_imgdma_rotdma0_open(
    kal_uint32 * const key)
{
    kal_bool result;

    if (owner.key==0)
    {
        result = idp_hw_open(&owner);
    }
    else
    {
        return KAL_FALSE;
    }

    if (KAL_TRUE == result)
    {
        idp_imgdma_rotdma0_open_real();
        (*key) = owner.key;
    }
    else
    {
        (*key) = 0;
    }

    return result;
}

kal_bool
idp_imgdma_rotdma0_close(
    kal_uint32 const key,
    idp_imgdma_rotdma0_struct const * const config)
{
    kal_bool result;

    result = idp_imgdma_rotdma0_stop_real(config);

    idp_imgdma_rotdma0_close_real();

    result = idp_hw_close(&owner, key);

    return result;
}

kal_bool
idp_imgdma_rotdma0_config(
    kal_uint32 const key,
    idp_imgdma_rotdma0_struct * const config,
    kal_bool const config_to_hardware)
{
    return idp_imgdma_rotdma0_config_real(config, config_to_hardware);
}

kal_bool
idp_imgdma_rotdma0_query(
    idp_imgdma_rotdma0_struct * const config, ...)
{
    return KAL_TRUE;
}

kal_bool
idp_imgdma_rotdma0_get_working_mem_size(
    kal_uint32 const key,
    idp_imgdma_rotdma0_struct * const config,
    kal_uint32 * const size)
{
    return KAL_TRUE;
}

kal_bool
idp_imgdma_rotdma0_get_working_mem(
    kal_uint32 const key,
    kal_bool const use_internal_mem,
    idp_imgdma_rotdma0_struct * const config,
    kal_bool const do_alloc,
    kal_uint32 const max_allowable_size)
{
    return KAL_TRUE;
}
#if defined(__MTK_TARGET__) && defined(__DCM_WITH_COMPRESSION_MAUI_INIT__)
#pragma push
#pragma arm section code="DYNAMIC_COMP_MAUIINIT_SECTION"
#endif

kal_bool
idp_imgdma_rotdma0_init(void)
{
    kal_mem_set(&owner, 0, sizeof(owner));
    owner.sem = kal_create_sem("IDP", 1);

    return KAL_TRUE;
}
#if defined(__MTK_TARGET__) && defined(__DCM_WITH_COMPRESSION_MAUI_INIT__)
#pragma arm section code
#pragma pop
#endif

kal_bool
idp_imgdma_rotdma0_start(
    kal_uint32 const key,
    idp_imgdma_rotdma0_struct const * const config)
{
    return idp_imgdma_rotdma0_start_real(config);
}

kal_bool
idp_imgdma_rotdma0_stop(
    kal_uint32 const key,
    idp_imgdma_rotdma0_struct const * const config)
{
    return idp_imgdma_rotdma0_stop_real(config);
}

kal_bool
idp_imgdma_rotdma0_is_busy(
    kal_uint32 const key,
    kal_bool * const busy,
    idp_imgdma_rotdma0_struct const * const config)
{
    return idp_imgdma_rotdma0_is_busy_real(busy, config);
}

kal_bool
idp_imgdma_rotdma0_is_in_use(void)
{
    return ((KAL_TRUE == idp_hw_can_be_used(&owner, 0)) ? KAL_FALSE : KAL_TRUE);
}


#endif // #if defined(DRV_IDP_6252_SERIES)


