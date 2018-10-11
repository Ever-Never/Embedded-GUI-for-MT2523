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

#ifndef __HAL_AUDIO_FW_LIBRARY_INTERNAL_H__
#define __HAL_AUDIO_FW_LIBRARY_INTERNAL_H__

/*******************************************************************************\
| Include Files                                                                 |
\*******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal_audio_fw_sherif.h"

/*******************************************************************************\
| Value Defines                                                                 |
\*******************************************************************************/

#define PATCH_DSP_PAGE              2
#define PATCH_DSP_ADDR              0x3C00
#define PATCH_DSP_LEN               0x400

#define DSP_D2M_IO4_BIT             (1 << 4)
#define DSP_D2M_IO5_BIT             (1 << 5)
#define DSP_D2M_IO6_BIT             (1 << 6)
#define DSP_D2M_EXCEPTION_BIT       (1 << 7)

#define DSP_RESET_BEGIN_VALUE       0x0006
#define DSP_RESET_END_VALUE         0x0007
#define DSP_SLOW_IDLE_DIVIDER_VALUE 11
#define DSP_DEEP_CLK_DIVIDER_VALUE  0xC81
#define DSP_RUNNING_VALUE           0x6666

#define IDX_MCU_HW_PATCH            1
#define IDX_DSP_HW_PATCH            2
#define PATCH_MCU_NUM               2
#define PATCH_TOTAL_NUM             24

#endif  /* __HAL_AUDIO_FW_LIBRARY_INTERNAL_H__ */
