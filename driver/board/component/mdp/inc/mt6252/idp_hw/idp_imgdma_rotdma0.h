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

#ifndef __idp_imgdma_rotdma0_mt6252_h__
#define __idp_imgdma_rotdma0_mt6252_h__

#ifndef THIS_FILE_BELONGS_TO_IMAGE_DATA_PATH_MODULE_INTERNAL
#error "Hay~! Man~! You can not include this file directly~!"
#endif

#include <idp_define.h>

#if defined(DRV_IDP_6252_SERIES)

#include "idp_imgdma.h"
#include <idp_core.h>
//#include "img_common_enum.h"
#include "idp_hw_common.h"
//#include <mt6252/idp_mem.h>
//#include "mm_power_ctrl.h"
#include <mm_comm_def.h>
//#include "drvpdn.h"
#include "hal_clock.h"
#include "hal_cm4_topsm.h"

/** \defgroup imgdma_rotdma0 ROTDMA0
 * @{
 */
#define ROTDMA_BUS_CONTROL_THRESHOLD 7  //burst 32 bytes
#define ROTDMA_ERROR_CODE__NO_ERROR (0x0)

#define ROT_DMA_base (0xA0400000) /*ROT DMA                                                            */
#define ROT_DMA_IRQn 19

#define ROT_DMA_BASE ROT_DMA_base

#define IMGDMA_ROT_DMA_IRQ_FLAG_REG              (ROT_DMA_BASE + 0x0000)
#define IMGDMA_ROT_DMA_IRQ_FLAG_CLR_REG          (ROT_DMA_BASE + 0x0008)
#define IMGDMA_ROT_DMA_CFG_REG                   (ROT_DMA_BASE + 0x0018)
#define IMGDMA_ROT_DMA_STOP_REG                  (ROT_DMA_BASE + 0x0028)
#define IMGDMA_ROT_DMA_EN_REG                    (ROT_DMA_BASE + 0x0030)
#define IMGDMA_ROT_DMA_RESET_REG                 (ROT_DMA_BASE + 0x0038)
#define IMGDMA_ROT_DMA_LOCK_REG                  (ROT_DMA_BASE + 0x003C)
#define IMGDMA_ROT_DMA_QUEUE_RSTA_REG            (ROT_DMA_BASE + 0x0040)
#define IMGDMA_ROT_DMA_QUEUE_WSTA_REG            (ROT_DMA_BASE + 0x0058)
#define IMGDMA_ROT_DMA_QUEUE_DATA_REG            (ROT_DMA_BASE + 0x0070)
#define IMGDMA_ROT_DMA_EXEC_CNT_REG              (ROT_DMA_BASE + 0x0080)
#define IMGDMA_ROT_DMA_SLOW_DOWN_REG             (ROT_DMA_BASE + 0x0300)
#define IMGDMA_ROT_DMA_Y_DST_STR_ADDR_REG        (ROT_DMA_BASE + 0x0318)
#define IMGDMA_ROT_DMA_U_DST_STR_ADDR_REG        (ROT_DMA_BASE + 0x0320)
#define IMGDMA_ROT_DMA_V_DST_STR_ADDR_REG        (ROT_DMA_BASE + 0x0328)
#define IMGDMA_ROT_DMA_SRC_SIZE_REG              (ROT_DMA_BASE + 0x0330)
#define IMGDMA_ROT_DMA_DST_SIZE_REG              (ROT_DMA_BASE + 0x0348)
#define IMGDMA_ROT_DMA_CON_REG                   (ROT_DMA_BASE + 0x0368)

#define REG_IMGDMA_ROT_DMA_IRQ_FLAG                *((volatile unsigned int*)IMGDMA_ROT_DMA_IRQ_FLAG_REG)
#define REG_IMGDMA_ROT_DMA_IRQ_FLAG_CLR            *((volatile unsigned int*)IMGDMA_ROT_DMA_IRQ_FLAG_CLR_REG)
#define REG_IMGDMA_ROT_DMA_CFG                     *((volatile unsigned int*)IMGDMA_ROT_DMA_CFG_REG)
#define REG_IMGDMA_ROT_DMA_STOP                    *((volatile unsigned int*)IMGDMA_ROT_DMA_STOP_REG)
#define REG_IMGDMA_ROT_DMA_EN                      *((volatile unsigned int*)IMGDMA_ROT_DMA_EN_REG)
#define REG_IMGDMA_ROT_DMA_RESET                   *((volatile unsigned int*)IMGDMA_ROT_DMA_RESET_REG)
#define REG_IMGDMA_ROT_DMA_LOCK                    *((volatile unsigned int*)IMGDMA_ROT_DMA_LOCK_REG)
#define REG_IMGDMA_ROT_DMA_QUEUE_RSTA              *((volatile unsigned int*)IMGDMA_ROT_DMA_QUEUE_RSTA_REG)
#define REG_IMGDMA_ROT_DMA_QUEUE_WSTA              *((volatile unsigned int*)IMGDMA_ROT_DMA_QUEUE_WSTA_REG)
#define REG_IMGDMA_ROT_DMA_QUEUE_DATA              *((volatile unsigned int*)IMGDMA_ROT_DMA_QUEUE_DATA_REG)
#define REG_IMGDMA_ROT_DMA_EXEC_CNT                *((volatile unsigned int*)IMGDMA_ROT_DMA_EXEC_CNT_REG)
#define REG_IMGDMA_ROT_DMA_SLOW_DOWN               *((volatile unsigned int*)IMGDMA_ROT_DMA_SLOW_DOWN_REG)
#define REG_IMGDMA_ROT_DMA_Y_DST_STR_ADDR          *((volatile unsigned int*)IMGDMA_ROT_DMA_Y_DST_STR_ADDR_REG)
#define REG_IMGDMA_ROT_DMA_U_DST_STR_ADDR          *((volatile unsigned int*)IMGDMA_ROT_DMA_U_DST_STR_ADDR_REG)
#define REG_IMGDMA_ROT_DMA_V_DST_STR_ADDR          *((volatile unsigned int*)IMGDMA_ROT_DMA_V_DST_STR_ADDR_REG)
#define REG_IMGDMA_ROT_DMA_SRC_SIZE                *((volatile unsigned int*)IMGDMA_ROT_DMA_SRC_SIZE_REG)
#define REG_IMGDMA_ROT_DMA_DST_SIZE                *((volatile unsigned int*)IMGDMA_ROT_DMA_DST_SIZE_REG)
#define REG_IMGDMA_ROT_DMA_CON                     *((volatile unsigned int*)IMGDMA_ROT_DMA_CON_REG)

/* bit maps */
#define IMGDMA_ROTDMA_IRQ_FLAG_0_BIT                0x00000001
#define IMGDMA_ROTDMA_IRQ_FLAG_0_EN_BIT             0x00010000

#define IMGDMA_ROTDMA_IRQ_FLAG_2_BIT                0x00000004
#define IMGDMA_ROTDMA_IRQ_FLAG_2_EN_BIT             0x00040000

#define IMGDMA_ROTDMA_CFG_AUTO_LOOP_EN_BIT          0x00000001
#define IMGDMA_ROTDMA_CFG_DES_MODE_BIT              0x00000080
#define IMGDMA_ROTDMA_CFG_DES_QUEUE_DEPTH_MASK      0x00000100
#define IMGDMA_ROTDMA_CFG_OFF_DROP_BIT              0x00004000
#define IMGDMA_ROTDMA_CFG_DES_Q_EMPTY_DROP_EN_BIT   0x00008000
//#if defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)
#define IMGDMA_ROTDMA_CFG_YUV_PITCH_EN_BIT          0x00010000
//#endif
#define IMGDMA_ROTDMA_CFG_FRAME_SYNC_EN_BIT         0x80000000

#define IMGDMA_ROTDMA_STOP_STOP_BIT                 0x00000001

#define IMGDMA_ROTDMA_ENABLE_EN_BIT                 0x00000001


#define IMGDMA_ROTDMA_RESET_HARD_RST_BIT            0x00000001
#define IMGDMA_ROTDMA_RESET_WARM_RST_BIT            0x00000002


#define IMGDMA_ROTDMA_LOCK_LOCK_BIT                 0x00000001

#define IMGDMA_ROTDMA_SLOW_DOWN_EN_BIT              0x00000001
#define IMGDMA_ROTDMA_SLOW_DOWN_COUNT_MASK          0xFFFF0000

//#if defined(MT6252H) || defined(MT6252)
//#define IMGDMA_ROTDMA_SRC_SIZE_W_MASK               0x000003FF
//#define IMGDMA_ROTDMA_SRC_SIZE_H_MASK               0x03FF0000

//#define IMGDMA_ROTDMA_DST_SIZE_MASK                 0x000007FF
//#elif defined(MT6250) || defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)
#define IMGDMA_ROTDMA_SRC_SIZE_W_MASK               0x000007FF
#define IMGDMA_ROTDMA_SRC_SIZE_H_MASK               0x07FF0000

#define IMGDMA_ROTDMA_DST_SIZE_MASK                 0x00000FFF
//#endif


#define IMGDMA_ROTDMA_CON_OUTPUT_FORMAT_MASK        0x00000007
#define IMGDMA_ROTDMA_CON_PROT_EN_BIT               0x00000040
#define IMGDMA_ROTDMA_CON_ULTRA_EN_BIT              0x00000080
#define IMGDMA_ROTDMA_CON_BUS_THERS_MASK            0x00000700
#define IMGDMA_ROTDMA_CON_V_SUBSAMP_BIT             0x01000000
#define IMGDMA_ROTDMA_CON_ROT_EN_BIT                0x08000000
#define IMGDMA_ROTDMA_CON_NOP_BIT                   0x40000000
#define IMGDMA_ROTDMA_CON_INT_EN_BIT                0x80000000


/* Macros of ROT_DMA */
/**
 * \brief Check if IRT0 is started or not.
 */
/// Is enabled
#define IMGDMA_ROTDMA_IS_ENABLED() \
    ((IMGDMA_ROTDMA_ENABLE_EN_BIT == (REG_IMGDMA_ROT_DMA_EN & IMGDMA_ROTDMA_ENABLE_EN_BIT)) ? KAL_TRUE : KAL_FALSE)

// Start ROTDMA
#define IMGDMA_ROTDMA_START() \
    do { REG_IMGDMA_ROT_DMA_EN = IMGDMA_ROTDMA_ENABLE_EN_BIT; } while(0)

// Stop ROTDMA, stop at frame end
#define IMGDMA_ROTDMA_STOP() \
    do { REG_IMGDMA_ROT_DMA_STOP = IMGDMA_ROTDMA_STOP_STOP_BIT; } while(0)

// Warm reset ROTDMA, reset at GMC transaction end
#define IMGDMA_ROTDMA_WARM_RESET() \
    do { REG_IMGDMA_ROT_DMA_RESET = IMGDMA_ROTDMA_RESET_WARM_RST_BIT; \
          } while(0)

#define IMGDMA_ROTDMA_IS_WARM_RESET_FINISH() \
    ((IMGDMA_ROTDMA_RESET_WARM_RST_BIT == (REG_IMGDMA_ROT_DMA_RESET & IMGDMA_ROTDMA_RESET_WARM_RST_BIT)) ? KAL_FALSE : KAL_TRUE)

// Hard reset ROTDMA, reset immediately, pending GMC transaction may cause state machine incorrect
// FPGA found GMC hang bug caused by rotdma hard reset break GMC protocol between GMC & rotdma
// GMC hw bug can not fix, not suggest to use hard reset, except indeed rotdma hw bug, not GMC
// Keep it for future usage, but just use warm reset now
#define IMGDMA_ROTDMA_HARD_RESET() \
    do { REG_IMGDMA_ROT_DMA_RESET = IMGDMA_ROTDMA_RESET_HARD_RST_BIT; \
           {   \
               volatile kal_uint32 loop = 100;   \
               while(loop--){ ; }   \
           } \
         REG_IMGDMA_ROT_DMA_RESET = 0x0; \
    } while(0)


#define IMGDMA_ROTDMA_FEND_INTERRUPT_ENABLE() \
    do { REG_IMGDMA_ROT_DMA_CON |= IMGDMA_ROTDMA_CON_INT_EN_BIT; \
            REG_IMGDMA_ROT_DMA_IRQ_FLAG |= IMGDMA_ROTDMA_IRQ_FLAG_0_EN_BIT;} while(0)

#define IMGDMA_ROTDMA_FEND_INTERRUPT_DISABLE() \
    do { REG_IMGDMA_ROT_DMA_IRQ_FLAG &= ~IMGDMA_ROTDMA_IRQ_FLAG_0_EN_BIT; } while(0)

#define IMGDMA_ROTDMA_Q_EMPTY_DROP_INTERRUPT_ENABLE() \
    do { REG_IMGDMA_ROT_DMA_IRQ_FLAG |= IMGDMA_ROTDMA_IRQ_FLAG_2_EN_BIT;} while(0)

#define IMGDMA_ROTDMA_Q_EMPTY_DROP_INTERRUPT_DISABLE() \
    do { REG_IMGDMA_ROT_DMA_IRQ_FLAG &= ~IMGDMA_ROTDMA_IRQ_FLAG_2_EN_BIT; } while(0)

#define IMGDMA_ROTDMA_INTERRUPT_ENABLE() \
    do { REG_IMGDMA_ROT_DMA_CON |= IMGDMA_ROTDMA_CON_INT_EN_BIT; } while(0)

#define IMGDMA_ROTDMA_INTERRUPT_DISABLE() \
    do { REG_IMGDMA_ROT_DMA_CON &= ~IMGDMA_ROTDMA_CON_INT_EN_BIT; } while(0)

#define IMGDMA_ROTDMA_AUTO_LOOP_ENABLE() \
    do { REG_IMGDMA_ROT_DMA_CFG |= IMGDMA_ROTDMA_CFG_AUTO_LOOP_EN_BIT; } while(0)

#define IMGDMA_ROTDMA_AUTO_LOOP_DISABLE() \
    do { REG_IMGDMA_ROT_DMA_CFG &= ~IMGDMA_ROTDMA_CFG_AUTO_LOOP_EN_BIT; } while(0)

// Descriptor mode
#define IMGDMA_ROTDMA_DESCRIPTOR_MODE() \
    do { REG_IMGDMA_ROT_DMA_CFG |= IMGDMA_ROTDMA_CFG_DES_MODE_BIT; } while(0)

// Register mode
#define IMGDMA_ROTDMA_REGISTER_MODE() \
    do { REG_IMGDMA_ROT_DMA_CFG &= ~IMGDMA_ROTDMA_CFG_DES_MODE_BIT; } while(0)

//#if defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)
// Enable YUV ptich
#define IMGDMA_ROTDMA_YUV_PITCH_ENABLE() \
    do { REG_IMGDMA_ROT_DMA_CFG |= IMGDMA_ROTDMA_CFG_YUV_PITCH_EN_BIT; } while(0)

// Disable YUV ptich
#define IMGDMA_ROTDMA_YUV_PITCH_DISABLE() \
    do { REG_IMGDMA_ROT_DMA_CFG &= ~IMGDMA_ROTDMA_CFG_YUV_PITCH_EN_BIT; } while(0)
//#endif

// Enable frame sync
#define IMGDMA_ROTDMA_FRAME_SYNC_ENABLE() \
    do { REG_IMGDMA_ROT_DMA_CFG |= IMGDMA_ROTDMA_CFG_FRAME_SYNC_EN_BIT; } while(0)

// Disable frame sync
#define IMGDMA_ROTDMA_FRAME_SYNC_DISABLE() \
    do { REG_IMGDMA_ROT_DMA_CFG &= ~IMGDMA_ROTDMA_CFG_FRAME_SYNC_EN_BIT; } while(0)


// Enable ultra high
#define IMGDMA_ROTDMA_ULTRA_HIGH_ENABLE() \
    do { REG_IMGDMA_ROT_DMA_CON |= IMGDMA_ROTDMA_CON_ULTRA_EN_BIT; } while(0)

// Disable ultra high
#define IMGDMA_ROTDMA_ULTRA_HIGH_DISABLE() \
    do { REG_IMGDMA_ROT_DMA_CON &= ~IMGDMA_ROTDMA_CON_ULTRA_EN_BIT; } while(0)

#define IMGDMA_ROTDMA_SET_BUS_THRESHOLD(thres) \
    do { REG_IMGDMA_ROT_DMA_CON &= ~IMGDMA_ROTDMA_CON_BUS_THERS_MASK; \
            REG_IMGDMA_ROT_DMA_CON |= ((thres << 8) & IMGDMA_ROTDMA_CON_BUS_THERS_MASK);} while(0)


#define IMGDMA_ROTDMA_SET_SLOW_DOWN_COUNT(count) \
    do { REG_IMGDMA_ROT_DMA_SLOW_DOWN &= ~IMGDMA_ROTDMA_SLOW_DOWN_COUNT_MASK; \
            REG_IMGDMA_ROT_DMA_SLOW_DOWN |= ((count << 16) & IMGDMA_ROTDMA_SLOW_DOWN_COUNT_MASK);} while(0)

// Enable slow down
#define IMGDMA_ROTDMA_SLOW_DOWN_ENABLE() \
    do { REG_IMGDMA_ROT_DMA_SLOW_DOWN |= IMGDMA_ROTDMA_SLOW_DOWN_EN_BIT; } while(0)

// Disable slow donw
#define IMGDMA_ROTDMA_SLOW_DOWN_DISABLE() \
    do { REG_IMGDMA_ROT_DMA_SLOW_DOWN &= ~IMGDMA_ROTDMA_SLOW_DOWN_EN_BIT; } while(0)



#define IMGDMA_ROTDMA_SET_QUEUE_DEPTH(depth) \
    do { REG_IMGDMA_ROT_DMA_CFG = \
        ((REG_IMGDMA_ROT_DMA_CFG & ~IMGDMA_ROTDMA_CFG_DES_QUEUE_DEPTH_MASK) | \
         (IMGDMA_ROTDMA_CFG_DES_QUEUE_DEPTH_MASK & ((depth) << 8))); } while(0)

#define IMGDMA_ROTDMA_QUEUE_EMPTY_DROP() \
    do { REG_IMGDMA_ROT_DMA_CFG |= IMGDMA_ROTDMA_CFG_DES_Q_EMPTY_DROP_EN_BIT; } while(0)

#define IMGDMA_ROTDMA_QUEUE_EMPTY_STALL() \
    do { REG_IMGDMA_ROT_DMA_CFG &= ~IMGDMA_ROTDMA_CFG_DES_Q_EMPTY_DROP_EN_BIT; } while(0)


#define IMGDMA_ROTDMA_DES_EACH_Q_SIZE     6 // 6*4 = 24 bytes


#define IMGDMA_ROTDMA_CON_OUTPUT_FORMAT_YUYV422     0x00000004
#define IMGDMA_ROTDMA_CON_OUTPUT_FORMAT_PLANAR_YUV  0x00000007

#define IMGDMA_ROTDMA_SET_OUTPUT_FORMAT(format) \
    do { REG_IMGDMA_ROT_DMA_CON = \
        ((REG_IMGDMA_ROT_DMA_CON & (~IMGDMA_ROTDMA_CON_OUTPUT_FORMAT_MASK)) | (format)); } while(0)


#define IMGDMA_ROTDMA_SET_OUTPUT_YUV422() \
    do { IMGDMA_ROTDMA_SET_OUTPUT_FORMAT(IMGDMA_ROTDMA_CON_OUTPUT_FORMAT_PLANAR_YUV); \
        (REG_IMGDMA_ROT_DMA_CON &= ~IMGDMA_ROTDMA_CON_V_SUBSAMP_BIT); } while(0)

#define IMGDMA_ROTDMA_SET_OUTPUT_YUV420() \
    do { IMGDMA_ROTDMA_SET_OUTPUT_FORMAT(IMGDMA_ROTDMA_CON_OUTPUT_FORMAT_PLANAR_YUV); \
        (REG_IMGDMA_ROT_DMA_CON |= IMGDMA_ROTDMA_CON_V_SUBSAMP_BIT); } while(0)

#define IMGDMA_ROTDMA_SET_OUTPUT_YUYV422() \
    do { IMGDMA_ROTDMA_SET_OUTPUT_FORMAT(IMGDMA_ROTDMA_CON_OUTPUT_FORMAT_YUYV422); \
          } while(0)


//mt6252 only support 90 degree with flip rotation
#define IMGDMA_ROTDMA_ROT_0() \
    do { REG_IMGDMA_ROT_DMA_CON &= ~IMGDMA_ROTDMA_CON_ROT_EN_BIT; } while(0)

#define IMGDMA_ROTDMA_ROT_90CW_WITH_FLIP() \
    do { REG_IMGDMA_ROT_DMA_CON |= IMGDMA_ROTDMA_CON_ROT_EN_BIT; } while(0)

#define IMGDMA_ROTDMA_DROP_PREV_ENGINE()   \
    do {REG_IMGDMA_ROT_DMA_CFG |= IMGDMA_ROTDMA_CFG_OFF_DROP_BIT; } while(0)

#define IMGDMA_ROTDMA0_STALL_PREV_ENGINE() \
    do {REG_IMGDMA_ROT_DMA_CFG &= ~IMGDMA_ROTDMA_CFG_OFF_DROP_BIT; } while(0)


#define IMGDMA_ROTDMA_PROTECT_ENABLE()   \
    do {REG_IMGDMA_ROT_DMA_CON |= IMGDMA_ROTDMA_CON_PROT_EN_BIT; } while(0)

#define IMGDMA_ROTDMA_PROTECT_DISABLE() \
    do {REG_IMGDMA_ROT_DMA_CON &= ~IMGDMA_ROTDMA_CON_PROT_EN_BIT; } while(0)

///power control
#define ROTDMA_POWERON()  hal_clock_enable(HAL_CLOCK_CG_ROTDMA) //mm_enable_power(MMPWRMGR_ROTDMA);
#define ROTDMA_POWEROFF() hal_clock_disable(HAL_CLOCK_CG_ROTDMA) //mm_disable_power(MMPWRMGR_ROTDMA);


/**
 *  \def  NO_SHRINK_ROTDMA_CODE
 *  If this is defined, ROTDAM can use full capability and some debug code.
 */
//#define NO_SHRINK_ROTDMA_CODE


/**
 * \brief rotdma structure
 *
 * Programmers can use this structure to set up rotdma
 */
typedef struct
{
    // main parameters
    img_rot_angle_enum_t          rot_angle; /**< rotate direction */

    MM_IMAGE_COLOR_FORMAT_ENUM      yuv_color_fmt; /**< output yuv format */
    kal_uint32                    y_dest_start_addr;
    kal_uint32                    u_dest_start_addr;
    kal_uint32                    v_dest_start_addr;

    kal_uint16                    src_width; /**< width */
    kal_uint16                    src_height; /**< height */

    kal_bool                      pitch_enable; /**< pitch, only for UYVY */
    kal_uint16                    pitch_bytes; /**< horizontal pitch WARNING! the pitch is applied to three buffers */

    // interrupt
    kal_bool                      fend_intr_en; /**< enable int or not */
    idp_lisr_intr_cb_t            fend_intr_cb;
    idp_lisr_intr_cb_param_t      fend_intr_cb_param;

#if defined(NO_SHRINK_ROTDMA_CODE)
    kal_bool                      q_empty_intr_en; /**< enable int or not */
    idp_lisr_intr_cb_t            q_empty_intr_cb;
    idp_lisr_intr_cb_param_t      q_empty_intr_cb_param;

    //descriptor mode
    kal_bool                      descriptor_mode;
    kal_uint8                     queue_depth; /**[1,2] */
    kal_bool                      queue_empty_drop; /** otherwise: stall previous engine */
    kal_uint32                    *des_queue_data_addr; //new: 6573 for future WinMo driver
#endif

    kal_bool                      auto_loop; /**< auto loop mode */

    kal_bool                      hw_frame_sync;/**camera app, first beginning signal is from cam */

    kal_bool                      drop_prev_engine_when_off;

    // performance related
    // isp_datapath only use for protect_en. directly use protect_en instead
    //kal_bool                      isp_datapath; /**< isp datapath enable : use to detemeter the bit of protect signal*/

#if defined(NO_SHRINK_ROTDMA_CODE)
    kal_bool                      slow_down_en; /** slow the access EMI rate */
    kal_uint16                    slow_down_cnt; /** cnt: larger is slower */
#endif

    kal_bool                      protect_en; /**< bus bandwidth protection for ROT_DMA enable, from camera => enable, others => disable */
    kal_bool                      ultra_high_en;
    kal_uint16                    bus_control_threshold; /** max emi burst type: HW default is 7 (burst 8-8) */

} idp_imgdma_rotdma0_struct;



extern void idp_imgdma_rotdma0_LISR(void);

extern kal_bool idp_imgdma_rotdma0_config_real(
    idp_imgdma_rotdma0_struct * const rotdma0_struct,
    kal_bool const config_to_hardware);

extern kal_bool
idp_imgdma_rotdma0_config_partial(
    idp_imgdma_rotdma0_struct * const p_rotdma_struct);

extern kal_bool
idp_imgdma_rotdma0_start_real(
    idp_imgdma_rotdma0_struct const * const p_rotdma0_struct);

extern kal_bool
idp_imgdma_rotdma0_stop_real(
    idp_imgdma_rotdma0_struct const * const p_rotdma0_struct);

extern void
idp_imgdma_rotdma0_open_real(void);

extern kal_bool
idp_imgdma_rotdma0_is_busy_real(
    kal_bool * const p_busy,
    idp_imgdma_rotdma0_struct const * const p_rotdma0_struct);

extern void
idp_imgdma_rotdma0_close_real(void);

extern kal_bool
rotdma_check_size_limitation(
    kal_uint32 source_width,
    kal_uint32 source_height,
    MM_IMAGE_COLOR_FORMAT_ENUM    tar_yuv_color_fmt,
    img_rot_angle_enum_t          rot_angle);

extern void rotdma0_warm_reset(void);

extern kal_bool
idp_imgdma_rotdma0_init(void);

extern kal_bool
idp_imgdma_rotdma0_open(
    kal_uint32 * const key);

extern kal_bool
idp_imgdma_rotdma0_config(
    kal_uint32 const key,
    idp_imgdma_rotdma0_struct * const config,
    kal_bool const config_to_hardware);

extern kal_bool
idp_imgdma_rotdma0_is_in_use(void);

extern kal_bool
idp_imgdma_rotdma0_stop(
    kal_uint32 const key,
    idp_imgdma_rotdma0_struct const * const config);


extern kal_bool
idp_imgdma_rotdma0_close(
    kal_uint32 const key,
    idp_imgdma_rotdma0_struct const * const config);

extern kal_bool
idp_imgdma_rotdma0_start(
    kal_uint32 const key,
    idp_imgdma_rotdma0_struct const * const config);

extern kal_bool
idp_imgdma_rotdma0_is_busy(
    kal_uint32 const key,
    kal_bool * const busy,
    idp_imgdma_rotdma0_struct const * const config);

#endif // #if defined(DRV_IDP_6252_SERIES)
#endif // #ifndef __idp_imgdma_rotdma0_mt6252_h__


