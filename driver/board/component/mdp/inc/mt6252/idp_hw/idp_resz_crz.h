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

#ifndef __idp_resz_crz_mt6252_h__
#define __idp_resz_crz_mt6252_h__

#ifndef THIS_FILE_BELONGS_TO_IMAGE_DATA_PATH_MODULE_INTERNAL
#error "Hay~! Man~! You can not include this file directly~!"
#endif

#include <idp_define.h>

#if defined(DRV_IDP_6252_SERIES)

#include "idp_resz.h"
#include "idp_hw_common.h"
#include "idp_core.h"
#include <mm_comm_def.h>
//#include "drvpdn.h"
//#include "mm_power_ctrl.h"
#include "hal_clock.h"
#include "hal_cm4_topsm.h"

/* ===============================================================
 *                       CRZ definition
 * =============================================================== */
/**
 * \defgroup crz Capture resizer
 * \ingroup resizer
 * @{
 */
#define CRZ_base    (0xA0410000) /*Resizer                                                            */
#define RESIZE_IRQn 16

#define CRZ_BASE CRZ_base

#define CRZ_CFG_REG             (CRZ_BASE + 0x0000)
#define CRZ_CON_REG             (CRZ_BASE + 0x0004)
#define CRZ_STA_REG             (CRZ_BASE + 0x0008)
#define CRZ_INT_REG             (CRZ_BASE + 0x000C)

#define CRZ_SRCSZ1_REG          (CRZ_BASE + 0x0010)
#define CRZ_TARSZ1_REG          (CRZ_BASE + 0x0014)
#define CRZ_HRATIO1_REG         (CRZ_BASE + 0x0018)
#define CRZ_VRATIO1_REG         (CRZ_BASE + 0x001C)
#define CRZ_HRES1_REG           (CRZ_BASE + 0x0020)
#define CRZ_VRES1_REG           (CRZ_BASE + 0x0024)
#define CRZ_LOCK_REG            (CRZ_BASE + 0x002C)
#define CRZ_ORIGSZ1_REG         (CRZ_BASE + 0x0030)
#define CRZ_CROPLR1_REG         (CRZ_BASE + 0x0034)
#define CRZ_CROPTB1_REG         (CRZ_BASE + 0x0038)

#define CRZ_FRCFG_REG           (CRZ_BASE + 0x0040)

#define CRZ_SRCSZ2_REG          (CRZ_BASE + 0x0060)
#define CRZ_TARSZ2_REG          (CRZ_BASE + 0x0064)
#define CRZ_HRATIO2_REG         (CRZ_BASE + 0x0068)
#define CRZ_VRATIO2_REG         (CRZ_BASE + 0x006C)
#define CRZ_HRES2_REG           (CRZ_BASE + 0x0070)
#define CRZ_VRES2_REG           (CRZ_BASE + 0x0074)

#define CRZ_DBGCFG_REG          (CRZ_BASE + 0x0090)
#define CRZ_ORIGSZ2_REG         (CRZ_BASE + 0x00A0)
#define CRZ_CROPLR2_REG         (CRZ_BASE + 0x00A4)
#define CRZ_CROPTB2_REG         (CRZ_BASE + 0x00A8)

#define CRZ_INFO0_REG           (CRZ_BASE + 0x00B0)
#define CRZ_INFO1_REG           (CRZ_BASE + 0x00B4)
#define CRZ_SMBASE_Y_REG        (CRZ_BASE + 0x00DC)
#define CRZ_SMBASE_U_REG        (CRZ_BASE + 0x00E0)
#define CRZ_SMBASE_V_REG        (CRZ_BASE + 0x00E4)
#define CRZ_GMCCON_REG          (CRZ_BASE + 0x00F0)
#define CRZ_CLIP_REG            (CRZ_BASE + 0x00FC)

//#if defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)
#define CRZ_TILE_CFG_REG            (CRZ_BASE + 0x0100)
#define CRZ_TILE_START_POS_X1_REG   (CRZ_BASE + 0x0104)
#define CRZ_TILE_START_POS_Y1_REG   (CRZ_BASE + 0x010C)
#define CRZ_BI_TRUNC_ERR_COMP1_REG  (CRZ_BASE + 0x0114)
#define CRZ_SZ_BI_INIT_RESID1_REG   (CRZ_BASE + 0x0118)
//#endif

#define REG_CRZ_CFG             *((volatile unsigned int *)(CRZ_CFG_REG))
#define REG_CRZ_CON             *((volatile unsigned int *)(CRZ_CON_REG))
#define REG_CRZ_STA             *((volatile unsigned int *)(CRZ_STA_REG))
#define REG_CRZ_INT             *((volatile unsigned int *)(CRZ_INT_REG))
#define REG_CRZ_SRCSZ1          *((volatile unsigned int *)(CRZ_SRCSZ1_REG))
#define REG_CRZ_TARSZ1          *((volatile unsigned int *)(CRZ_TARSZ1_REG))
#define REG_CRZ_HRATIO1         *((volatile unsigned int *)(CRZ_HRATIO1_REG))
#define REG_CRZ_VRATIO1         *((volatile unsigned int *)(CRZ_VRATIO1_REG))
#define REG_CRZ_HRES1           *((volatile unsigned int *)(CRZ_HRES1_REG))
#define REG_CRZ_VRES1           *((volatile unsigned int *)(CRZ_VRES1_REG))

#define REG_CRZ_LOCK            *((volatile unsigned int *)(CRZ_LOCK_REG))
#define REG_CRZ_ORIGSZ1         *((volatile unsigned int *)(CRZ_ORIGSZ1_REG))
#define REG_CRZ_CROPLR1         *((volatile unsigned int *)(CRZ_CROPLR1_REG))
#define REG_CRZ_CROPTB1         *((volatile unsigned int *)(CRZ_CROPTB1_REG))

#define REG_CRZ_FRCFG           *((volatile unsigned int *)(CRZ_FRCFG_REG))

#define REG_CRZ_SRCSZ2          *((volatile unsigned int *)(CRZ_SRCSZ2_REG))
#define REG_CRZ_TARSZ2          *((volatile unsigned int *)(CRZ_TARSZ2_REG))
#define REG_CRZ_HRATIO2         *((volatile unsigned int *)(CRZ_HRATIO2_REG))
#define REG_CRZ_VRATIO2         *((volatile unsigned int *)(CRZ_VRATIO2_REG))
#define REG_CRZ_HRES2           *((volatile unsigned int *)(CRZ_HRES2_REG))
#define REG_CRZ_VRES2           *((volatile unsigned int *)(CRZ_VRES2_REG))

#define REG_CRZ_DBGCFG          *((volatile unsigned int *)(CRZ_DBGCFG_REG))

#define REG_CRZ_ORIGSZ2         *((volatile unsigned int *)(CRZ_ORIGSZ2_REG))
#define REG_CRZ_CROPLR2         *((volatile unsigned int *)(CRZ_CROPLR2_REG))
#define REG_CRZ_CROPTB2         *((volatile unsigned int *)(CRZ_CROPTB2_REG))

#define REG_CRZ_INFO0           *((volatile unsigned int *)(CRZ_INFO0_REG))
#define REG_CRZ_INFO1           *((volatile unsigned int *)(CRZ_INFO1_REG))
#define REG_CRZ_SMBASE_Y        *((volatile unsigned int *)(CRZ_SMBASE_Y_REG))
#define REG_CRZ_SMBASE_U        *((volatile unsigned int *)(CRZ_SMBASE_U_REG))
#define REG_CRZ_SMBASE_V        *((volatile unsigned int *)(CRZ_SMBASE_V_REG))
#define REG_CRZ_GMCCON          *((volatile unsigned int *)(CRZ_GMCCON_REG))
#define REG_CRZ_CLIP            *((volatile unsigned int *)(CRZ_CLIP_REG))

//#if defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)
#define REG_CRZ_TILE_CFG            *((volatile unsigned int *)(CRZ_TILE_CFG_REG))
#define REG_CRZ_TILE_START_POS_X1   *((volatile unsigned int *)(CRZ_TILE_START_POS_X1_REG))
#define REG_CRZ_TILE_START_POS_Y1   *((volatile unsigned int *)(CRZ_TILE_START_POS_Y1_REG))
#define REG_CRZ_BI_TRUNC_ERR_COMP1  *((volatile unsigned int *)(CRZ_BI_TRUNC_ERR_COMP1_REG))
#define REG_CRZ_SZ_BI_INIT_RESID1   *((volatile unsigned int *)(CRZ_SZ_BI_INIT_RESID1_REG))
//#endif


#define CRZ_CFG_SRC1_MASK                   0x00000003
#define CRZ_CFG_SRC2_MASK                   0x000000C0
#define CRZ_CFG_PCON_BIT                    0x00000010
#define CRZ_CFG_2PASS_BIT                   0x00000020
#define CRZ_CFG_VSYNC_RST_0_BIT             0x00000040 // new frames comes auto reset
#define CRZ_CFG_VSYNC_RST_1_BIT             0x00000080 // output too slow auto reset
#define CRZ_CFG_VSYNC_RST_2_BIT             0x00000100 // drop pixel auto reset
//#if defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)
#define CRZ_CFG_MODE1_MASK                  0x00010000
//#endif


#define CRZ_CFG_SRC1_FORMAT_CAM             0x00000000
#define CRZ_CFG_SRC1_FORMAT_UYVY            0x00000001
#define CRZ_CFG_SRC1_FORMAT_YUV420          0x00000002
#define CRZ_CFG_SRC1_FORMAT_YUV422          0x00000003

#define CRZ_CFG_SRC2_FORMAT_CAM             0x00000000
#define CRZ_CFG_SRC2_FORMAT_UYVY            0x00000004
#define CRZ_CFG_SRC2_FORMAT_YUV420          0x00000008
#define CRZ_CFG_SRC2_FORMAT_YUV422          0x0000000C


#define CRZ_CON_ENA_BIT                     0x00000001
#define CRZ_CON_RST_BIT                     0x00010000

// STA bits are all RO
#define CRZ_STA_OUT_BUSY_BIT                0x00000001
#define CRZ_STA_MEMIN_BUSY_BIT              0x00000002
#define CRZ_STA_IN_BUSY_BIT                 0x00000004
#define CRZ_STA_RUN2_STATUS_BIT             0x00000008
#define CRZ_STA_ERR0_BIT                    0x00000100  // pixel over run
#define CRZ_STA_ERR1_BIT                    0x00000200  // input pixel is not enough
#define CRZ_STA_ERR2_BIT                    0x00000400  // output not complete when new frame comes

#define CRZ_STA_ERR3_BIT                    0x00001000  // drop due to lock when vsync comes
#define CRZ_STA_ERR4_BIT                    0x00002000  //
#define CRZ_STA_ERR5_BIT                    0x00004000  // input pixel not enough when crop enabled

#define CRZ_CROP_ORIGSZ_WIDTH_MASK          0x000003FF
#define CRZ_CROP_ORIGSZ_HEIGHT_MASK         0x03FF0000

#define CRZ_CROP_ENABLE_BIT                 0x80000000

#define CRZ_CROP_R_MASK                     0x000003FF
#define CRZ_CROP_L_MASK                     0x03FF0000

#define CRZ_CROP_B_MASK                     0x000003FF
#define CRZ_CROP_T_MASK                     0x03FF0000

#define CRZ_LOCK_LOCK_BIT                   0x00000001

#define CRZ_FROCE_V_RESIZE_ENABLE_BIT       0x00000200
#define CRZ_FROCE_H_RESIZE_ENABLE_BIT       0x00000400
#define CRZ_DOUBLE_BUFFERD_REGISTER_DISABLE_BIT     0x00000800

#define CRZ_INT_FEND_BIT                    0x00000001  //RC
#define CRZ_INT_FSTART1_BIT                 0x00000002  //RC
#define CRZ_INT_FSTART2_BIT                 0x00000004  //RC
#define CRZ_INT_PIXEL_DROP_BIT              0x00000008  //RC
#define CRZ_INT_MEM_IN_DONE_BIT             0x00000010  //RC
#define CRZ_INT_LOCK_DROP_FRAME_BIT         0x00000020  //RC


#define CRZ_FRCFG_FEND_INT_EN_BIT           0x00000400
#define CRZ_FRCFG_FSTART1_INT_EN_BIT        0x00000800
#define CRZ_FRCFG_FSTART2_INT_EN_BIT        0x00001000
#define CRZ_FRCFG_PXLDROP_INT_EN_BIT        0x00002000
#define CRZ_FRCFG_MEM_IN_DONE_INT_EN_BIT    0x00004000
#define CRZ_FRCFG_LOCK_DROP_FRAME_INT_EN_BIT      0x00008000

//#if defined(MT6252H) || defined(MT6252)
//#define CRZ_FRCFG_WMSZ1_MASK                0x001F0000
//#define CRZ_FRCFG_WMSZ2_MASK                0x1F000000
//#elif defined(MT6250) || defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)
#define CRZ_FRCFG_WMSZ1_MASK                0x003F0000
#define CRZ_FRCFG_WMSZ2_MASK                0x3F000000
//#endif


#define CRZ_DBGCFG_PVR1_BIT                 0x00000200
#define CRZ_DBGCFG_PHR1_BIT                 0x00000400
#define CRZ_DBGCFG_NODB_BIT                 0x00000800


#define CRZ_GMCCON_RD_MIN_REQ_EN_BIT        0x00000001
#define CRZ_GMCCON_RD_MAX_BURST_LENG_BIT    0x00000010
#define CRZ_GMCCON_RD_MIN_REQ_INTERVAL_MASK 0xFFF00000


#define CRZ_CLIP_ORG_WD_MASK                0x000007FF
#define CRZ_CLIP_EN_BIT                     0x80000000

#define CRZ_LOCK_EN_BIT                     0x00000001

#define CRZ_H_RATIO_SHIFT_BITS  20
#define CRZ_V_RATIO_SHIFT_BITS  20

//#if defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)
#define CRZ_TILE_CFG_SA_EN_X1_BIT           0x00000001
#define CRZ_TILE_CFG_SA_EN_Y1_BIT           0x00000002

#define CRZ_BI_TRUNC_ERR_COMP1_X_MASK       0x00000FFF
#define CRZ_BI_TRUNC_ERR_COMP1_Y_MASK       0x0FFF0000

#define CRZ_BI_INIT_RESID1_X_MASK           0x00001FFF
#define CRZ_BI_INIT_RESID1_Y_MASK           0x1FFF0000
//#endif

#define CRZ_INTERRUPT_FSTART1_ENABLE() \
    do { REG_CRZ_FRCFG |= CRZ_FRCFG_FSTART1_INT_EN_BIT; } while(0)

#define CRZ_INTERRUPT_FSTART1_DISABLE() \
    do { REG_CRZ_FRCFG &= ~CRZ_FRCFG_FSTART1_INT_EN_BIT; } while(0)

#define CRZ_INTERRUPT_FSTART2_ENABLE() \
    do { REG_CRZ_FRCFG |= CRZ_FRCFG_FSTART2_INT_EN_BIT; } while(0)

#define CRZ_INTERRUPT_FSTART2_DISABLE() \
    do { REG_CRZ_FRCFG &= ~CRZ_FRCFG_FSTART2_INT_EN_BIT; } while(0)

#define CRZ_INTERRUPT_FEND_ENABLE() \
    do { REG_CRZ_FRCFG |= CRZ_FRCFG_FEND_INT_EN_BIT; } while(0)

#define CRZ_INTERRUPT_FEND_DISABLE() \
    do { REG_CRZ_FRCFG &= ~CRZ_FRCFG_FEND_INT_EN_BIT; } while(0)

#define CRZ_INTERRUPT_PXLDROP_ENABLE() \
    do { REG_CRZ_FRCFG |= CRZ_FRCFG_PXLDROP_INT_EN_BIT; } while(0)

#define CRZ_INTERRUPT_PXLDROP_DISABLE() \
    do { REG_CRZ_FRCFG &= ~CRZ_FRCFG_PXLDROP_INT_EN_BIT; } while(0)

#define CRZ_INTERRUPT_MEM_IN_DONE_ENABLE() \
    do { REG_CRZ_FRCFG |= CRZ_FRCFG_MEM_IN_DONE_INT_EN_BIT; } while(0)

#define CRZ_INTERRUPT_MEM_IN_DONE_DISABLE() \
    do { REG_CRZ_FRCFG &= ~CRZ_FRCFG_MEM_IN_DONE_INT_EN_BIT; } while(0)


#define CRZ_INTERRUPT_LOCK_DROP_FRAME_ENABLE() \
    do { REG_CRZ_FRCFG |= CRZ_FRCFG_LOCK_DROP_FRAME_INT_EN_BIT; } while(0)

#define CRZ_INTERRUPT_LOCK_DROP_FRAME_DISABLE() \
    do { REG_CRZ_FRCFG &= ~CRZ_FRCFG_LOCK_DROP_FRAME_INT_EN_BIT; } while(0)


#define CRZ_ENABLE_2PASS() \
    do { REG_CRZ_CFG |= CRZ_CFG_2PASS_BIT; } while(0)

#define CRZ_DISABLE_2PASS() \
    do { REG_CRZ_CFG &= ~CRZ_CFG_2PASS_BIT; } while(0)


#define CRZ_ENABLE_HW_FRAME_SYNC() \
    do { REG_CRZ_CFG |= CRZ_CFG_VSYNC_RST_2_BIT; \
           REG_CRZ_CFG &= ~ CRZ_CFG_VSYNC_RST_1_BIT; \
           REG_CRZ_CFG &= ~ CRZ_CFG_VSYNC_RST_0_BIT;} while (0)

#define CRZ_DISABLE_HW_FRAME_SYNC() \
    do { REG_CRZ_CFG &= ~CRZ_CFG_VSYNC_RST_2_BIT; \
           REG_CRZ_CFG &= ~ CRZ_CFG_VSYNC_RST_1_BIT; \
           REG_CRZ_CFG &= ~ CRZ_CFG_VSYNC_RST_0_BIT;} while (0)

#define CRZ_SET_CONT_RUN_MODE() \
    do { REG_CRZ_CFG |= CRZ_CFG_PCON_BIT; } while(0)

#define CRZ_SET_SINGLE_RUN_MODE() \
    do { REG_CRZ_CFG &= ~CRZ_CFG_PCON_BIT; } while(0)

//#if defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)
#define CRZ_ENABLE_TILE_MODE() \
    do { REG_CRZ_CFG |= CRZ_CFG_MODE1_MASK; } while(0)

#define CRZ_DISABLE_TILE_MODE() \
    do { REG_CRZ_CFG &= ~CRZ_CFG_MODE1_MASK; } while(0)
//#endif

#define CRZ_RESET() \
    do { REG_CRZ_CON = CRZ_CON_RST_BIT;} while(0)

#define CRZ_IS_RESET_FINISH() \
    ((CRZ_CON_RST_BIT == (REG_CRZ_CON & CRZ_CON_RST_BIT)) ? KAL_FALSE : KAL_TRUE)

#define CRZ_IS_STARTED() \
    ((CRZ_CON_ENA_BIT == (REG_CRZ_CON & CRZ_CON_ENA_BIT)) ? KAL_TRUE : KAL_FALSE)

#define CRZ_START() \
    do { REG_CRZ_CON = CRZ_CON_ENA_BIT;} while(0)

#define CRZ_STOP() \
    do { REG_CRZ_CON &= ~CRZ_CON_ENA_BIT; } while(0)

#define CRZ_SET_SRC_SIZE(width, height) \
    do { REG_CRZ_SRCSZ1 = ((width) | ((height) << 16)); } while(0)

#define CRZ_SET_TARGET_SIZE(width, height) \
    do { REG_CRZ_TARSZ1 = ((width) | ((height) << 16)); } while(0)


#define CRZ_SET_SRC_SIZE2(width, height) \
    do { REG_CRZ_SRCSZ2 = ((width) | ((height) << 16)); } while(0)

#define CRZ_SET_TARGET_SIZE2(width, height) \
    do { REG_CRZ_TARSZ2 = ((width) | ((height) << 16)); } while(0)


#define CRZ_SET_CROP_ORIG1_SIZE(width, height) \
    do { REG_CRZ_ORIGSZ1 = ((width) | ((height) << 16)); } while(0)

#define CRZ_SET_CROP_ORIG2_SIZE(width, height) \
    do { REG_CRZ_ORIGSZ2 = ((width) | ((height) << 16)); } while(0)


#define CRZ_SET_CROP_RANGE1(left, right, top, bottom) \
    do { REG_CRZ_CROPLR1 = ((right) | ((left) << 16)); \
             REG_CRZ_CROPTB1 = ((bottom) | ((top) << 16)); } while(0)

#define CRZ_SET_CROP_RANGE2(left, right, top, bottom) \
    do { REG_CRZ_CROPLR2 = ((right) | ((left) << 16)); \
             REG_CRZ_CROPTB2 = ((bottom) | ((top) << 16)); } while(0)


#define CRZ_SET_CROP1_ENABLE() \
    do { REG_CRZ_CROPLR1 |= CRZ_CROP_ENABLE_BIT; } while(0)


#define CRZ_SET_CROP1_DISABLE() \
    do { REG_CRZ_CROPLR1 &= ~CRZ_CROP_ENABLE_BIT; } while(0)


#define CRZ_SET_CROP2_ENABLE() \
    do { REG_CRZ_CROPLR2 |= CRZ_CROP_ENABLE_BIT; } while(0)


#define CRZ_SET_CROP2_DISABLE() \
    do { REG_CRZ_CROPLR2 &= ~CRZ_CROP_ENABLE_BIT; } while(0)


#define CRZ_DOUBLE_BUFFERD_REGISTER_ENABLE() \
    do { REG_CRZ_DBGCFG &= ~CRZ_DOUBLE_BUFFERD_REGISTER_DISABLE_BIT; } while(0)

#define CRZ_DOUBLE_BUFFERD_REGISTER_DISABLE() \
    do { REG_CRZ_DBGCFG |= CRZ_DOUBLE_BUFFERD_REGISTER_DISABLE_BIT; } while(0)


#define CRZ_FORCE_V_RESIZE_ENABLE() \
    do { REG_CRZ_DBGCFG |= CRZ_FROCE_V_RESIZE_ENABLE_BIT; } while(0)

#define CRZ_FORCE_V_RESIZE_DISABLE() \
    do { REG_CRZ_DBGCFG &= ~CRZ_FROCE_V_RESIZE_ENABLE_BIT; } while(0)


#define CRZ_FORCE_H_RESIZE_ENABLE() \
    do { REG_CRZ_DBGCFG |= CRZ_FROCE_H_RESIZE_ENABLE_BIT; } while(0)

#define CRZ_FORCE_H_RESIZE_DISABLE() \
    do { REG_CRZ_DBGCFG &= ~CRZ_FROCE_H_RESIZE_ENABLE_BIT; } while(0)


#define CRZ_SET_WMSZ1(n) \
    do { REG_CRZ_FRCFG = ((REG_CRZ_FRCFG & (~CRZ_FRCFG_WMSZ1_MASK)) | (n << 16)); } while(0)


#define CRZ_SET_WMSZ2(n) \
    do { REG_CRZ_FRCFG = ((REG_CRZ_FRCFG & (~CRZ_FRCFG_WMSZ2_MASK)) | (n << 24)); } while(0)


#define CRZ_SET_INPUT_SRC1_CAM() \
    do { REG_CRZ_CFG &=~CRZ_CFG_SRC1_MASK; \
       REG_CRZ_CFG |=CRZ_CFG_SRC1_FORMAT_CAM;} while (0)

#define CRZ_SET_INPUT_SRC1_MEMORY_IN_UYVY422() \
    do { REG_CRZ_CFG &=~CRZ_CFG_SRC1_MASK; \
       REG_CRZ_CFG |=CRZ_CFG_SRC1_FORMAT_UYVY;} while (0)

#define CRZ_SET_INPUT_SRC1_MEMORY_IN_YUV422() \
    do { REG_CRZ_CFG &=~CRZ_CFG_SRC1_MASK; \
       REG_CRZ_CFG |=CRZ_CFG_SRC1_FORMAT_YUV422;} while (0)

#define CRZ_SET_INPUT_SRC1_MEMORY_IN_YUV420() \
    do { REG_CRZ_CFG &=~CRZ_CFG_SRC1_MASK; \
       REG_CRZ_CFG |=CRZ_CFG_SRC1_FORMAT_YUV420;} while (0)


#define CRZ_SET_INPUT_SRC2_CAM() \
    do { REG_CRZ_CFG &=~CRZ_CFG_SRC2_MASK; \
       REG_CRZ_CFG |=CRZ_CFG_SRC2_FORMAT_CAM;} while (0)

#define CRZ_SET_INPUT_SRC2_MEMORY_IN_UYVY422() \
    do { REG_CRZ_CFG &=~CRZ_CFG_SRC2_MASK; \
       REG_CRZ_CFG |=CRZ_CFG_SRC2_FORMAT_UYVY;} while (0)

#define CRZ_SET_INPUT_SRC2_MEMORY_IN_YUV422() \
    do { REG_CRZ_CFG &=~CRZ_CFG_SRC2_MASK; \
       REG_CRZ_CFG |=CRZ_CFG_SRC2_FORMAT_YUV422;} while (0)

#define CRZ_SET_INPUT_SRC2_MEMORY_IN_YUV420() \
    do { REG_CRZ_CFG &=~CRZ_CFG_SRC2_MASK; \
       REG_CRZ_CFG |=CRZ_CFG_SRC2_FORMAT_YUV420;} while (0)


#define CRZ_SET_YUV_SRC_ADDRESS(Yaddr, Uaddr, Vaddr) \
    do { REG_CRZ_SMBASE_Y = Yaddr; \
         REG_CRZ_SMBASE_U = Uaddr; \
         REG_CRZ_SMBASE_V = Vaddr;} while(0)


#define CRZ_MEMORY_IN_CLIP_ENABLE() \
    do { REG_CRZ_CLIP |= CRZ_CLIP_EN_BIT; } while(0)

#define CRZ_MEMORY_IN_CLIP_DISABLE() \
    do { REG_CRZ_CLIP &= ~CRZ_CLIP_EN_BIT; } while(0)

#define CRZ_SET_MEMORY_IN_BG_WIDTH(width) \
    do { REG_CRZ_CLIP &= ~CRZ_CLIP_ORG_WD_MASK; \
         REG_CRZ_CLIP |= width & CRZ_CLIP_ORG_WD_MASK;} while(0)


#define CRZ_LOCK_ENABLE() \
    do { REG_CRZ_LOCK |= CRZ_LOCK_EN_BIT; } while(0)

#define CRZ_LOCK_DISABLE() \
    do { REG_CRZ_LOCK &= ~CRZ_LOCK_EN_BIT; } while(0)


//#if defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)
#define CRZ_TILE_SA_X1_ENABLE() \
    do { REG_CRZ_TILE_CFG |= CRZ_TILE_CFG_SA_EN_X1_BIT; } while(0)

#define CRZ_TILE_SA_X1_DISABLE() \
    do { REG_CRZ_TILE_CFG &= ~CRZ_TILE_CFG_SA_EN_X1_BIT; } while(0)

#define CRZ_TILE_SA_Y1_ENABLE() \
    do { REG_CRZ_TILE_CFG |= CRZ_TILE_CFG_SA_EN_Y1_BIT; } while(0)

#define CRZ_TILE_SA_Y1_DISABLE() \
    do { REG_CRZ_TILE_CFG &= ~CRZ_TILE_CFG_SA_EN_Y1_BIT; } while(0)

#define CRZ_SET_BI_TRUNC_ERR_COMP1(compx, compy) \
    do { REG_CRZ_BI_TRUNC_ERR_COMP1 = ((compx) | (compy << 16)); } while(0)


#define CRZ_SET_BI_INIT_RESID1(residx, residy) \
    do { REG_CRZ_SZ_BI_INIT_RESID1 = ((residx) | (residy << 16)); } while(0)
//#endif


///power control
// New PDN API for projects later than MT6253 and MT6236...
#define CRZ_POWERON()   hal_clock_enable(HAL_CLOCK_CG_RESEIZER) //mm_enable_power(MMPWRMGR_RESZ);
#define CRZ_POWEROFF()  hal_clock_disable(HAL_CLOCK_CG_RESEIZER) //mm_disable_power(MMPWRMGR_RESZ);

//#if defined(MT6252H) || defined(MT6252)
//#define CRZ_LIMIT_WT    320
//#elif defined(MT6250) || defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)
#define CRZ_LIMIT_WT    800
//#endif


enum idp_resz_crz_error_code_t
{
    IDP_RESZ_CRZ_NO_ERROR,
    IDP_RESZ_CRZ_ERROR_CODE_SRC_WIDTH_TOO_LARGE,
    IDP_RESZ_CRZ_ERROR_CODE_SRC_WIDTH_TOO_SMALL,
    IDP_RESZ_CRZ_ERROR_CODE_SRC_HEIGHT_TOO_LARGE,
    IDP_RESZ_CRZ_ERROR_CODE_SRC_HEIGHT_TOO_SMALL,
    IDP_RESZ_CRZ_ERROR_CODE_TAR_WIDTH_TOO_LARGE,
    IDP_RESZ_CRZ_ERROR_CODE_TAR_WIDTH_TOO_SMALL,
    IDP_RESZ_CRZ_ERROR_CODE_TAR_HEIGHT_TOO_LARGE,
    IDP_RESZ_CRZ_ERROR_CODE_TAR_HEIGHT_TOO_SMALL,
    IDP_RESZ_CRZ_ERROR_CODE_HW_LIMITATION
};
typedef enum idp_resz_crz_error_code_t idp_resz_crz_error_code_t;

enum
{
    IDP_RESZ_CRZ_QUERY_ERROR_CODE = 1
};

typedef struct
{
    kal_bool                  two_pass_resize_en;

    idp_module_enum_t         input_src1;
    MM_IMAGE_COLOR_FORMAT_ENUM      mem_in_color_fmt1;

    //although two pass support, but only one memory in address
    kal_uint32 src_buff_y_addr;
    kal_uint32 src_buff_u_addr;
    kal_uint32 src_buff_v_addr;

    kal_bool                  fend_intr_en;
    idp_lisr_intr_cb_t        fend_intr_cb;
    idp_lisr_intr_cb_param_t  fend_intr_cb_param;

    kal_bool                  fstart1_intr_en;
    idp_lisr_intr_cb_t        fstart1_intr_cb;
    idp_lisr_intr_cb_param_t  fstart1_intr_cb_param;

    kal_bool                  pixel_dropped_intr_en;
    idp_lisr_intr_cb_t        pixel_dropped_intr_cb;
    idp_lisr_intr_cb_param_t  pixel_dropped_intr_cb_param;

    kal_bool                  lock_drop_frame_intr_en;
    idp_lisr_intr_cb_t        lock_drop_frame_intr_cb;
    idp_lisr_intr_cb_param_t  lock_drop_frame_intr_cb_param;

    kal_bool                  auto_restart;
    kal_bool                  hw_frame_sync;

    kal_uint16                src_height;         //original source height, before crop
    kal_uint16                src_width;           //original source width, before crop
    kal_uint16                tar_height;
    kal_uint16                tar_width;

    kal_bool                  clip_en;//only memory input valid
    kal_uint16                org_width;// unit is pixels

    kal_bool                  crop_en1; //pass 1
    kal_uint16                crop_left1;
    kal_uint16                crop_right1;
    kal_uint16                crop_top1;
    kal_uint16                crop_bottom1;

//#if defined(MT6260) || defined(MT6261) || defined(MT2501) || defined(MT2502) || defined(MT2523)
    kal_bool                  tile_mode;
    kal_bool                  sa_en_x;
    kal_bool                  sa_en_y;
    kal_uint32                tile_org_frame_width;
    kal_uint32                tile_org_tar_width;
    
    kal_uint32                tile_start_pos_x;
    kal_uint32                tile_start_pos_y;
    kal_uint16                tile_trunc_err_comp_x;
    kal_uint16                tile_trunc_err_comp_y;
    kal_uint16                tile_resid_x;
    kal_uint16                tile_resid_y;
//#endif

} idp_resz_crz_struct;

IDP_HW_COMMON_API_DECLARATION(resz_crz)

extern kal_bool
idp_resz_crz_config_real(
    idp_resz_crz_struct * const p_crz_struct,
    kal_bool const config_to_hardware);

extern kal_bool
idp_resz_crz_get_working_mem_real(
    kal_bool const use_internal_mem,
    idp_resz_crz_struct * const p_crz_struct,
    kal_bool const do_alloc,
    kal_uint32 const max_allowable_size);

extern kal_bool
idp_resz_crz_start_real(
    idp_resz_crz_struct const * const p_crz_struct);

extern kal_bool
idp_resz_crz_stop_real(
    idp_resz_crz_struct const * const p_crz_struct);

extern void
idp_resz_crz_close_real(void);

extern void
idp_resz_crz_open_real(void);

extern void
idp_resz_crz_forced_reset(void);

extern kal_bool
idp_resz_crz_is_busy_real(
    kal_bool * const p_busy,
    idp_resz_crz_struct const * const p_crz_struct);

extern kal_bool
crz_check_size_limitation(
    kal_uint32 source_width,
    kal_uint32 source_height,
    kal_uint32 target_width,
    kal_uint32 target_height,
    idp_module_enum_t  input_src,
    MM_IMAGE_COLOR_FORMAT_ENUM  src_yuv_color_fmt);

#endif // #if defined(DRV_IDP_6252_SERIES)

#endif // #ifndef __idp_resz_crz_mt6252_h__


