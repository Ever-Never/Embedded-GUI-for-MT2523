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

#ifndef __idp_hw_common_mt6252_h__
#define __idp_hw_common_mt6252_h__

#ifndef THIS_FILE_BELONGS_TO_IMAGE_DATA_PATH_MODULE_INTERNAL
#error "Hay~! Man~! You can not include this file directly~!"
#endif

enum idp_module_enum_t
{
    IDP_MODULE_IMGDMA_RDMA0
  , IDP_MODULE_IMGDMA_RDMA1

  , IDP_MODULE_IMGDMA_WDMA0
  , IDP_MODULE_IMGDMA_WDMA1
  , IDP_MODULE_IMGDMA_WDMA2

  , IDP_MODULE_IMGDMA_ROTDMA0 /**< The control code of the mux is put in ROT_DMA0. */
  , IDP_MODULE_IMGDMA_ROTDMA1

  , IDP_MODULE_IMGDMA_OVL
  , IDP_MODULE_IMGDMA_JPEG

  , IDP_MODULE_IMGPROC_IPP

  , IDP_MODULE_RESZ_CRZ
  , IDP_MODULE_RESZ_PRZ
  , IDP_MODULE_RESZ_DRZ

  , IDP_MODULE_MMSYS_MOUT

  , IDP_MODULE_OUTER_LCD // TODO: change outer to external
  , IDP_MODULE_OUTER_ISP // TODO: change outer to external
  , IDP_MODULE_OUTER_JPEG_ENCODER // TODO: change outer to external

  , IDP_MODULE_LAST
};
typedef enum idp_module_enum_t idp_module_enum_t;

typedef enum
{
    IDP_LCD_HW_TRIGGER_MODE_DISABLE,
    IDP_LCD_HW_TRIGGER_MODE_NO_SYNC,
    IDP_LCD_HW_TRIGGER_MODE_SYNC_N_STALL,
    IDP_LCD_HW_TRIGGER_MODE_SYNC_N_REDRAW,
    IDP_LCD_HW_TRIGGER_MODE_SYNC_N_DROP
} idp_lcd_hw_trigger_mode_t;

#endif // #ifndef __idp_hw_common_mt6252_h__

