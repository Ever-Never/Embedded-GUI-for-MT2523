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

#ifndef __HAL_AUDIO_LOW_POWER_H__
#define __HAL_AUDIO_LOW_POWER_H__

#include "hal_audio.h"

#if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED)

#include "hal_audio_fw_sherif.h"
#include "hal_audio_internal_service.h"
#include "hal_dsp_topsm.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct audlp2_hdl {
    bool             audio_lowpower_enable;
    bool             clock_switch_26m;
} hal_audio_lowpower_struct_t;

//typedef volatile unsigned long  *APBADDR32;

/* #define MODEM2G_TOPSM_BASE   0x83010000 */
/* #define MDCONFIG_BASE        0x83000000*/
#define MODEM2G_TOPSM_SW_CLK_FORCE_ON_SET        ((volatile unsigned long  *)(MODEM2G_TOPSM_BASE+0x0A04))  //32bit
#define MODEM2G_TOPSM_SW_CLK_FORCE_ON_CLR        ((volatile unsigned long  *)(MODEM2G_TOPSM_BASE+0x0A08))  //32bit
#define MODEM_ALLOW_26M_CLKSW                    ((volatile unsigned long  *)(MDCONFIG_BASE+0x0508))       //32bit

void audio_lowpower_init(void);
void audio_lowpower_set_mode(bool enable);
void audio_lowpower_set_clock_mode(bool flag);
void audio_lowpower_sw_trigger_event_set(bool flag);
void audio_lowpower_set_26mclock(bool flag);
bool audio_lowpower_check_status(void);

#ifdef __cplusplus
}
#endif

#endif  /* defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED) */

#endif /* __HAL_AUDIO_LOW_POWER_H__ */
