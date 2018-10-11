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

#include "hal_audio_fw_slt.h"
#include "hal_audio_fw_slt_internal.h"

uint32_t dsp_slt_step     = 0;
uint16_t dsp_slt_checksum = 0;

int32_t dsp_audio_fw_slt_entry(void)
{
    int32_t result = DSP_AUDIO_FW_SLT_PASS;
    {   /* Check DSP HW patch */
        dsp_slt_step = 1;
        dsp_audio_fw_slt_process();                         /* Test DSP HW patch entries */
        dsp_slt_step = 2;
        while (*DSP_SLT_READY != SLT_READY_MAGIC_WORD) ;    /* Busy wait loop            */
        dsp_slt_step = 3;
        dsp_slt_checksum = *DSP_SLT_CHECKSUM;
        if (dsp_slt_checksum != SLT_CHECKSUM_ANSWER) {
            result = DSP_AUDIO_FW_SLT_FAIL;
        }
        dsp_slt_step = 4;
        dsp_audio_fw_lib_reboot();                          /* Recover DSP HW patch      */
        dsp_slt_step = 5;
    }
    {   /* PM RAM Write */
        uint16_t test_val = 0;
        volatile uint16_t *p_dst;
        uint32_t loop_idx;
        p_dst = DSP_PM_3_RAM_ADDRESS;
        dsp_slt_step = 6;
        for (loop_idx = 0; loop_idx < DSP_PM_3_RAM_LENGTH; loop_idx++) {
            *p_dst++ = test_val++;
        }
        dsp_slt_step = 7;
        p_dst = DSP_PM_F_RAM_ADDRESS;
        for (loop_idx = 0; loop_idx < DSP_PM_F_RAM_LENGTH; loop_idx++) {
            *p_dst++ = test_val++;
        }
        dsp_slt_step = 8;
    }
    {   /* PM RAM Read & Check */
        uint32_t error_count = 0;
        uint16_t test_val = 0;
        volatile uint16_t *p_src;
        uint32_t loop_idx;
        p_src = DSP_PM_3_RAM_ADDRESS;
        dsp_slt_step = 9;
        for (loop_idx = 0; loop_idx < DSP_PM_3_RAM_LENGTH; loop_idx++) {
            if (*p_src++ != test_val++) {
                error_count++;
            }
        }
        dsp_slt_step = 10;
        p_src = DSP_PM_F_RAM_ADDRESS;
        for (loop_idx = 0; loop_idx < DSP_PM_F_RAM_LENGTH; loop_idx++) {
            if (*p_src++ != test_val++) {
                error_count++;
            }
        }
        dsp_slt_step = 11;
        if (error_count != 0) {
            result = DSP_AUDIO_FW_SLT_FAIL;
        }
    }
    {   /* PM RAM Clear */
        volatile uint16_t *p_dst;
        uint32_t loop_idx;
        p_dst = DSP_PM_3_RAM_ADDRESS;
        dsp_slt_step = 12;
        for (loop_idx = 0; loop_idx < DSP_PM_3_RAM_LENGTH; loop_idx++) {
            *p_dst++ = 0;
        }
        dsp_slt_step = 13;
        p_dst = DSP_PM_F_RAM_ADDRESS;
        for (loop_idx = 0; loop_idx < DSP_PM_F_RAM_LENGTH; loop_idx++) {
            *p_dst++ = 0;
        }
        dsp_slt_step = 14;
    }
    return result;
}
