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

#ifndef __HAL_GDMA_INTERNAL_H__
#define __HAL_GDMA_INTERNAL_H__


#include "hal_platform.h"

#ifdef HAL_GDMA_MODULE_ENABLED
#include "hal_nvic.h"
#include "hal_nvic_internal.h"


/** @brief general dma transfer size format */
typedef enum {
    GDMA_BYTE = 0,            /**< general dma transfer size -byte format */
    GDMA_HALF_WORD = 1,       /**<general  dma transfer size -half word format */
    GDMA_WORD = 2             /**<general  dma transfer size -word  format */
} gdma_transfer_size_format_t;

#define MIN_LENGHT_VALUE 0x0001
#define MAX_LENGTH_VALUE 0x3FFFC
#define MAX_COUNT_VALUE  0xFFFF


#define INVALID_INDEX  0xff


#define GDMA_INIT  1
#define GDMA_DEINIT  0
#define GDMA_CHECK_AND_SET_BUSY(gdma_port,busy_status)  \
do{ \
    uint32_t saved_mask; \
    saved_mask = save_and_set_interrupt_mask(); \
    if(gdma_init_status[gdma_port] == GDMA_INIT){ \
        busy_status = HAL_GDMA_STATUS_ERROR; \
    } else { \
        gdma_init_status[gdma_port] = GDMA_INIT;  \
        busy_status = HAL_GDMA_STATUS_OK; \
    } \
        restore_interrupt_mask(saved_mask); \
}while(0)

#define GDMA_SET_IDLE(gdma_port)   \
do{  \
       gdma_init_status[gdma_port] = GDMA_DEINIT;  \
}while(0)

gdma_transfer_size_format_t gdma_calculate_size(
    uint32_t length,
    uint32_t *count);

#endif /*HAL_GDMA_MODULE_ENABLED*/

#endif //__HAL_GDMA_INTERNAL_H__
