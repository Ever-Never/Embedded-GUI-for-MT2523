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

#ifndef __DRV_FEATURES_MDP_H__
#define __DRV_FEATURES_MDP_H__

//#include "drv_features_chip_select.h"
//#include "lcd_sw_inc.h"
//#include "sensor_capability.h"
#include "drv_features_6261.h"

/**********************************
 * Chip-independent IDP driver definition section
 **********************************/
#undef DRV_RGB_RESIZE_SUPPORT
#undef DRV_BLOCK_EFFECT_SUPPORT
#undef DRV_PIXEL_EFFECT_SUPPORT

/**
 *  \def MDP_SUPPORT_FAST_POWER_ON_OFF
 *  If this is defined, it means video renderer will power on/off mdp engines
 *  when mdp frame start/end to save power
 */
#undef MDP_SUPPORT_FAST_POWER_ON_OFF

/**
 *  \def MDP_SUPPORT_VP_EMI_BW_SWITCH
 *  If this is defined, it means video renderer will switch EMI BW 
 *  when renderer start/stop to release EMI BW to other modules.
 */
#undef MDP_SUPPORT_VP_EMI_BW_SWITCH

#if defined(DRV_IDP_6252_SERIES)
#define DRV_IDP_SUPPORT
//#define MDP_FW_TRIGGER_LCD    //DDV1, no use now

#undef DRV_RGB_RESIZE_SUPPORT
#undef DRV_BLOCK_EFFECT_SUPPORT
#undef DRV_PIXEL_EFFECT_SUPPORT

#undef MDP_SUPPORT_RGB565_FMT_INTERNAL
#define USE_UYVY422_BETWEEN_MDP_LCD_INTERNAL  //MDP -> UYVY422 buffer -> LCD
#undef MDP_SUPPORT_IMAGE_EFFECT_INTERNAL
#undef MDP_SUPPORT_HW_OVERLAP_INTERNAL
#define MDP_SUPPORT_FAST_POWER_ON_OFF

/* USE 2 pass to break ROTDMA 90 degree source width cannot > 320(MT6252) or 480(MT6250) */
#if defined(MT6252H) || defined(MT6252)
  #if (LCD_HEIGHT > 320) || (LCD_WIDTH > 320)
    #define IDP_VIDEO_DECODE_2PASS_ROTATE_TRICK
  #endif
#elif defined(MT6250) || defined(MT6260) || defined(MT6261) || defined(MT2523)
  #if (LCD_HEIGHT > 480) || (LCD_WIDTH > 480)
    #define IDP_VIDEO_DECODE_2PASS_ROTATE_TRICK
  #endif
#endif

#if defined(MT6260) || defined(MT6261) || defined(MT2523)
  #if defined(__IMAGE_SENSOR_2M__) || defined(__IMAGE_SENSOR_1M__) || (LCD_HEIGHT > 480) || (LCD_WIDTH > 480)
    #define IDP_TILE_MODE_SUPPORT
    #if (LCD_HEIGHT > 480) || (LCD_WIDTH > 480)
      #define IDP_CAM_PREVIEW_TILEMODE_2PASS_SUPPORT
    #endif
  #endif
  #if defined(__DRV_COLOR_MT6260_SERIES__)
    #define IDP_MM_COLOR_SUPPORT
  #endif
#endif

#endif // #if defined(DRV_IDP_6252_SERIES)

#endif   /*End __DRV_FEATURES_MDP_H__*/
