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

//#include "IntrCtrl.h"

#include <mt6252/idp_hw/idp_imgdma_rotdma0.h>

/**
 * \brief Initialize the image DMA subsystem.
 *
 * \ingroup imgdma
 *
 * This functino will setup the interrupt for the image DMA
 * to level sensitive, and unmask all the interrupts for the
 * image DMA.
 */

#if defined(IDP_FULL_ISR_SUPPORT)
static
#endif
void idp_imgdma_LISR(void);
#if defined(__MTK_TARGET__) && defined(__DCM_WITH_COMPRESSION_MAUI_INIT__)
#pragma push
#pragma arm section code="DYNAMIC_COMP_MAUIINIT_SECTION"
#endif

void
idp_imgdma_init(void)
{
#ifndef MDP_C_MODEL

#if defined(IDP_FULL_ISR_SUPPORT)
    //NVIC_Register(ROT_DMA_IRQn, idp_imgdma_LISR);
    hal_nvic_register_isr_handler(ROT_DMA_IRQn, idp_imgdma_LISR);
    NVIC_SetPriority(ROT_DMA_IRQn, 5);
    NVIC_EnableIRQ(ROT_DMA_IRQn);
#endif

    //IRQSensitivity(IRQ_ROT_DMA_CODE, LEVEL_SENSITIVE);
    //IRQUnmask(IRQ_ROT_DMA_CODE);

#endif // #ifndef MDP_C_MODEL

    // Initial the owner structure.
    idp_imgdma_rotdma0_init();
}
#if defined(__MTK_TARGET__) && defined(__DCM_WITH_COMPRESSION_MAUI_INIT__)
#pragma arm section code
#pragma pop
#endif

#if defined(IDP_FULL_ISR_SUPPORT)
static
#endif
void
idp_imgdma_LISR(void)
{
    idp_imgdma_rotdma0_LISR();
}


kal_uint32
idp_get_imgdma_status_reg_value(void)
{
    return 0;
}

#endif // #if defined(DRV_IDP_6252_SERIES)

