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

#ifndef __EXTERNAL_DSP_APPLICATION_H__
#define __EXTERNAL_DSP_APPLICATION_H__

#include "hal_platform.h"
#include "external_dsp_driver.h"
#include "nvdm.h"


#ifdef __cplusplus
extern "C" {
#endif

#define EXT_DSP_RESET_REG   (0x0)
#define EXT_DSP_STATE_REG   (0x1)

#define EXT_DSP_MODE_DOWNLOAD_W         (0x5)
#define EXT_DSP_MODE_DOWNLOAD_R         (0x5)
#define EXT_DSP_MODE_NORMAL_W           (0x4)
#define EXT_DSP_MODE_NORMAL_R           (0x4)
#define EXT_DSP_MODE_POWER_SAVING_R     (0xC)
#define EXT_DSP_MODE_POWER_SAVING_W     (0xD)

/* Select output source of PDM_DATO0/TX0 , PDM_DATO1/TX1
 * write dram address 0x0fffbff8 = EXP_DSP_OUTPUT_SOURCE: 0xmn,
 * m: the source for PDM_DATO1/TX1
 * n: the source for PDM_DATO0/TX0
 * m/n = 1, voice (MIC0_L) before enhancement
 * m/n = 2, voice (MIC0_R) before enhancement
 * m/n = 3, voice (MIC1_L) before enhancement
 * m/n = 9, voice after enhancement
 * m/n = a, echo reference from I2S
 */

#ifdef MTK_AUTOMOTIVE_SUPPORT
#define EXP_DSP_OUTPUT_SOURCE           (0x0099)
#define EXT_DSP_PARAMETER_BASE          (0x0FFEB880)
#else
#define EXP_DSP_OUTPUT_SOURCE           (0x0019)
#define EXT_DSP_PARAMETER_BASE          (0x0FFF4560)
#endif

typedef enum {
    EXTERNAL_DSP_AUTO_BT_MODE = 1,
    EXTERNAL_DSP_AUTO_VR_MODE = 2
} external_dsp_auto_mode_t;

typedef struct {
    bool                        external_dsp_on;
    uint32_t                    output_source;
#ifdef MTK_AUTOMOTIVE_SUPPORT
    external_dsp_auto_mode_t    auto_mode;
#endif
} external_dsp_t;

void external_dsp_switch_pdm_clock(bool enable);
external_dsp_status_t external_dsp_load_firmware(void);
void external_dsp_gpio_init(void);

external_dsp_status_t external_dsp_init(void);
external_dsp_status_t external_dsp_deinit(void);
external_dsp_status_t external_dsp_activate(bool active);
bool external_dsp_is_power_on(void);

external_dsp_status_t external_dsp_enter_psm_mode(void);
external_dsp_status_t external_dsp_enter_normal_mode(void);
external_dsp_status_t external_dsp_configure_data_path(uint32_t config);
external_dsp_status_t external_dsp_firmware_loaded(void);
external_dsp_status_t external_dsp_i2s_firmware_loaded(void);

external_dsp_status_t external_dsp_read_parameter(uint32_t index, uint16_t *data);
external_dsp_status_t external_dsp_write_parameter(uint32_t index, uint16_t data);
external_dsp_status_t external_dsp_read_dram_word(uint32_t address, uint16_t *data);
external_dsp_status_t external_dsp_write_dram_word(uint32_t address, uint16_t data);

external_dsp_status_t external_dsp_switch_automotive_mode(external_dsp_auto_mode_t mode);
external_dsp_status_t external_dsp_get_automotive_mode(external_dsp_auto_mode_t *mode);

uint16_t external_dsp_read_dsp_spl_value(void);
external_dsp_status_t external_dsp_enable_spl(bool enable);

uint32_t external_dsp_read_dsp_config_path(void);
void external_dsp_set_output_source(uint32_t config);
uint32_t external_dsp_get_output_source(void);
#ifdef MTK_AUTOMOTIVE_SUPPORT
void external_dsp_set_auto_mode(external_dsp_auto_mode_t mode);
external_dsp_auto_mode_t external_dsp_get_auto_mode(void);
#endif

//LDO/MIPS related
external_dsp_status_t external_dsp_get_clock_rate(uint32_t *mips);
external_dsp_status_t external_dsp_set_clock_rate(uint8_t mips);

#ifdef __cplusplus
}
#endif

#endif /* __EXTERNAL_DSP_APPLICATION_H__ */


