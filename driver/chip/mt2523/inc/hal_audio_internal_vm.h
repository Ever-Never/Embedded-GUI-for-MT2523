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

#ifndef __HAL_AUDIO_INTERNAL_VM_H__
#define __HAL_AUDIO_INTERNAL_VM_H__

#include "hal_audio.h"

#if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED)


#ifdef __cplusplus
extern "C" {
#endif


#define VM_CTRL_UL         1
#define VM_CTRL_DL         2
#define VM_CTRL_SC_START   2
#define VM_CTRL_SD_START   6

#define VM_PCM_1ST_SET_RECORD_FLAG  0x1
#define VM_PCM_2ND_SET_RECORD_FLAG  0x2
#define VM_VM_RECORD_FLAG           0x4
#define VM_PCM_REFMIC_RECORD_FLAG   0x8

#define VM_STATE_RECORD       0x1
#define VM_STATE_IDLE         0x10
#define VM_STATE_RECORD_STOP  0x20 // entering vm stop

#define FIVE_PCM_SAVE_SIZE    (2+320+320+2+320+320+2+320+2)   //UL 320+320+2; DL 320+320+2; REF 320+2+2
#define VM_BUFFER_SIZE        513

#define VM_MAGIC_HEADER 0xAA88

#define VM_DBGINFO_LEN 56
#define VM_ENH_DBGINFO_LEN 69
#define VM_DRV_DBGINFO_LEN 20

#define VM_PCM_BAND_FLAG_UL_PRE   0x0010
#define VM_PCM_BAND_FLAG_UL_POS   0x0020
#define VM_PCM_BAND_FLAG_DL_PRE   0x0040
#define VM_PCM_BAND_FLAG_DL_POS   0x0080
#define VM_PCM_BAND_FLAG_UL2_POS  0x0100

typedef struct {
    uint16_t ul_pre_len;
    uint16_t ul_pos_len;
    uint16_t dl_pre_len;
    uint16_t dl_pos_len;
    uint16_t ul2_pos_len;

    volatile uint16_t *ul_pre_buf;
    volatile uint16_t *ul_pos_buf;
    volatile uint16_t *dl_pre_buf;
    volatile uint16_t *dl_pos_buf;
    volatile uint16_t *ul2_pos_buf;
} epl_frame_t;

typedef struct {
    uint16_t enc_mode;
    uint16_t dec_mode;
    uint16_t enc_hdr;
    uint16_t dec_hdr;

    volatile uint16_t *enc_hb_addr;
    volatile uint16_t *dec_hb_addr;
    volatile uint16_t *dbgInfo_addr;
    volatile uint16_t *enh_dbgInfo_addr;

} vm_frame_t;

typedef enum {
    VM_SAVE_OR_PRINT_METHOD_NONE = 0,
    VM_SAVE_OR_PRINT_METHOD_WRITE_TO_FILE = 1 << 0,
    VM_SAVE_OR_PRINT_METHOD_WRITE_USB_DEBUG_PORT = 1 << 1,
} vm_save_or_print_method_t;

void audio_vm_Start(vm_save_or_print_method_t save_or_print_method);
void audio_vm_Stop(void);


#ifdef __cplusplus
}
#endif

#endif  /* defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED) */

#endif /*__HAL_AUDIO_INTERNAL_VM_H__*/

