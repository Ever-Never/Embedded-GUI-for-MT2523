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

#include "hal_audio.h"

#if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED)

#include "hal_audio_internal_service.h"
#include "hal_audio_internal_afe.h"
#include "hal_audio_internal_pcm.h"
#include "hal_i2s_internal.h"
#include "hal_audio_fw_sherif.h"
#include "hal_audio_fw_interface.h"
#include "hal_pmu_internal.h"
#include "hal_nvic_internal.h"
#include "hal_sleep_manager.h"
#include "hal_dsp_topsm.h"
#include "hal_sys_topsm.h"
#include "hal_log.h"
#include "hal_cm4_topsm.h"
#include "hal_nvic.h"

#ifdef HAL_AUDIO_LOW_POWER_ENABLED
#include "hal_audio_low_power.h"
#include "hal_dsp_topsm.h"
#endif
#include "exception_handler.h"
#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs.h"
#endif
#include "hal_audio_post_processing_internal.h"

#if defined(MTK_EXTERNAL_DSP_ENABLE)
#include "external_dsp_application.h"
#include "external_dsp_driver.h"
#endif  /* defined(MTK_EXTERNAL_DSP_ENABLE) */

#include "hal_audio_enhancement.h"

audio_isr_t audio_isr;
audio_common_t audio_common;
extern afe_t afe;
extern hal_audio_stream_t audio_config;
extern cm4_topsm_mtcmos_control mtcmos_control;
audio_pcm_route_isr_t audio_pcm_route_isr;
extern hal_audio_stream_t audio_config;

#ifdef HAL_DVFS_MODULE_ENABLED
static bool dsp_dvfs_valid(uint32_t voltage, uint32_t frequency)
{
    if (frequency < AUDIO_CPU_FREQ_L_BOUND) {
        return false;
    } else {
        return true;
    }
}

static dvfs_notification_t dsp_dvfs_desc = {
    .domain = "VCORE",
    .module = "CM_CK0",
    .addressee = "dsp_dvfs",
    .ops = {
        .valid = dsp_dvfs_valid,
    }
};

static void audio_register_dsp_dvfs(bool flag)
{
    if (flag) {
        dvfs_register_notification(&dsp_dvfs_desc);
        hal_dvfs_target_cpu_frequency(AUDIO_CPU_FREQ_L_BOUND, HAL_DVFS_FREQ_RELATION_L);
    } else {
        dvfs_deregister_notification(&dsp_dvfs_desc);
    }

}
#endif /*HAL_DVFS_MODULE_ENABLED*/

const int16_t speech_src_filter_coefficient[82] = {
    96,  162,   -92,   -488,  -302,   408,   372,  -660,  -780,    782,
    1272, -958, -2094,   1088,  3458, -1200, -6426,  1266, 20318,  30834,
    20318, 1266, -6426,  -1200,  3458,  1088, -2094,  -958,  1272,    782,
    -780, -660,   372,    408,  -302,  -488,   -92,   162,    96,      0,
    0,
    -36, -175,  -262,    -51,   277,   146,  -376,  -317,   489,    595,
    -597, -1034,   689,   1777,  -744, -3351,   727, 10500, 15852,  10500,
    727, -3351,  -744,   1777,   689, -1034,  -597,  -595,   489,   -317,
    -376,  146,   277,    -51,  -262,  -175,   -36,     0,     0,      0,
    0
};

void audio_dsp_reload_coeficient(void)
{
    audio_dsp_write_anti_alias_filter();

    // vvvv Issues be happend in ifly carkit project(HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE & HFP switch).
    //      We found dsp work abnormal. It seems some region in dsp become abnormal.
    //      Add this will fix some abormal when afe fix in 16k sampling rate (in fixWB).
    //      1. CVSD 4k~8k will have duplicate tone with 0k~4k (may cause by SRC filer)
    //      2. The volume is not the same in CVSD and MSBC (may cause by AGC filter & sidetone filter)
    //      You can think that we reload filters again.
    speech_reload_filter_coefficient();
}

void audio_dsp_write_anti_alias_filter(void)
{
    int i;
    const int16_t *ptr         = speech_src_filter_coefficient;
    volatile uint16_t *dsp_ptr = DSP_PM_ADDR(3, *DSP_SPH_FLT_COEF_ADDR_SRC);
    for (i = 82; i > 0; i--) {
        *dsp_ptr++ = *ptr++;
    }
}

const uint16_t audio_dsp_pcm_idle_delay_table[3][3] = {
    { 0x0A, 0x0A , 0x4F + (16 << 10) + (2 << 8)}, /*for pcm 8k record*/
    { 0x0A, 0x0A , 0x28 + (16 << 10) + (2 << 8)}, /*for pcm 16k record*/
    { 0x0A, 0x0A , 0x96 + (16 << 10) + (2 << 8)}, /*for pcm2way*/
};

void audio_dsp_speech_set_delay(uint8_t type)
{
    *DSP_SPH_DEL_R = audio_dsp_pcm_idle_delay_table[type][0];
    *DSP_SPH_DEL_W = audio_dsp_pcm_idle_delay_table[type][1];
    *DSP_SPH_DEL_M = audio_dsp_pcm_idle_delay_table[type][2];
}

static void audio_service_dsp_wakeup_setting(bool flag)
{
    uint32_t savedmask = save_and_set_interrupt_mask();
    uint16_t reg_value = *SHARE_D2M_WAKEUP_CTL;
    if (flag) {
        reg_value |= (0x1 << 0);
    } else {
        reg_value &= ~(0x1 << 0);
    }
    *SHARE_D2M_WAKEUP_CTL = reg_value;
    restore_interrupt_mask(savedmask);
    return;
}

static void audio_service_clean_dsp_wakeup_event(void)
{
    *SHARE_D2M_WAKEUP_STA = 0x1;
    return;
}

void audio_service_hook_isr(uint16_t isrtype, isr function, void *userdata)
{
    uint32_t func_id;
    uint32_t savedmask = save_and_set_interrupt_mask();
    for (func_id = 0; func_id < MAXISR; func_id++) {
        if (audio_isr.num[func_id] == 0) {
            audio_isr.num[func_id] = isrtype;
            audio_isr.function[func_id] = function;
            audio_isr.userdata[func_id] = userdata;
            break;
        }
    }
    restore_interrupt_mask(savedmask);
    return;
}

void audio_service_unhook_isr(uint16_t isrtype)
{
    uint32_t func_id;
    uint32_t savedmask = save_and_set_interrupt_mask();
    for (func_id = 0; func_id < MAXISR; func_id++) {
        if (audio_isr.num[func_id] == isrtype) {
            audio_isr.num[func_id] = 0;
            audio_isr.function[func_id] = NULL;
            audio_isr.userdata[func_id] = NULL;
            break;
        }
    }
    restore_interrupt_mask(savedmask);
    return;
}

static bool audio_is_from_speech(uint16_t iid, uint32_t *sph_int)
{
    uint32_t val = 0;
    uint16_t mask = 0;
    bool from_sph = true;
    switch (iid) {
        case DSP_IID_SPEECH_UL_ID:
            mask = *DSP_D2C_SPEECH_UL_INT;
            if (mask & 0x0001) {
                val += PSEUDO_SAL_DSPINT_ID_REC_PCM;
            }
            if (mask & 0x0002) {
                val += PSEUDO_SAL_DSPINT_ID_REC_EPL;
            }
            if (mask & 0x0004) {
                val += PSEUDO_SAL_DSPINT_ID_PNW_UL;
            }
            *DSP_D2C_SPEECH_UL_INT = 0;
            break;
        case DSP_IID_SPEECH_DL_ID:
            mask = *DSP_D2C_SPEECH_DL_INT;
            if (mask & 0x0001) {
                val += PSEUDO_SAL_DSPINT_ID_REC_VM;
            }
            if (mask & 0x0004) {
                val += PSEUDO_SAL_DSPINT_ID_PNW_DL;
            }
            *DSP_D2C_SPEECH_DL_INT = 0;
            break;
        default:
            from_sph = false;
            break;
    }
    *sph_int = val;
    return from_sph;
}

static void audio_service_trigger_callback(uint16_t magic_number)
{
    uint32_t func_id;
    for (func_id = 0; func_id < MAXISR; func_id++) {
        if (magic_number == audio_isr.num[func_id]) {
            void *userdata = audio_isr.userdata[func_id];
            audio_isr.function[func_id](userdata);
            break;
        }
    }
    return;
}

static void audio_service_dsp_to_mcu_lisr(uint16_t itype)
{
    if (itype != 0x0000) {
        uint32_t sph_int = 0;
        bool from_sph = audio_is_from_speech(itype, &sph_int);
        if (from_sph) {
            uint16_t bit_idx;
            for (bit_idx = 1; bit_idx < PSEUDO_SAL_DSPINT_PRIO_MAX ; bit_idx++) {
                if (sph_int & (1 << bit_idx)) {
                    audio_service_trigger_callback(DSP_INT_MAPPING_BASIC + bit_idx);
                }
            }
        } else {
            audio_service_trigger_callback(itype);
        }
    }
    return;
}

static void audio_service_dsp_memory_callback_init(void)
{
    if (audio_common.assert_source == AUDIO_DSP_ASSERT_NONE) {
        DSP_WRITE(DSP_MCU_STATUS, 0);   /* Freeze DSP */
        __disable_irq();
        audio_common.assert_source = AUDIO_DSP_ASSERT_FROM_MCU;
    }
    return;
}

const uint32_t audio_dsp_pm_page_table[AUDIO_DSP_PM_PAGE_COUNT] = {0, 3};
const uint32_t audio_dsp_dm_page_table[AUDIO_DSP_DM_PAGE_COUNT] = {5, 6, 7};

static void audio_service_dsp_memory_callback_dump(void)
{
    {   /* Dump PM */
        uint32_t table_idx;
        for (table_idx = 0; table_idx < AUDIO_DSP_PM_PAGE_COUNT; table_idx++) {
            uint32_t page_idx = audio_dsp_pm_page_table[table_idx];
            volatile uint16_t *p_src = DSP_PM_ADDR(page_idx, 0x0000);
            uint32_t loop_idx;
            for (loop_idx = 0; loop_idx < 0x4000; loop_idx += 8) {
                uint16_t dsp_val[8];
                uint32_t val_idx;
                uint32_t base_ptr = (uint32_t)p_src;
                for (val_idx = 0; val_idx < 8; val_idx++) {
                    dsp_val[val_idx] = *p_src++;
                }
                printf("0x%08x: %04x %04x %04x %04x %04x %04x %04x %04x\n\r", (unsigned int)base_ptr, dsp_val[0], dsp_val[1], dsp_val[2], dsp_val[3], dsp_val[4], dsp_val[5], dsp_val[6], dsp_val[7]);
            }
        }
    }
    {   /* Dump DM */
        uint32_t table_idx;
        for (table_idx = 0; table_idx < AUDIO_DSP_DM_PAGE_COUNT; table_idx++) {
            uint32_t page_idx = audio_dsp_dm_page_table[table_idx];
            volatile uint16_t *p_src = DSP_DM_ADDR(page_idx, 0x0000);
            uint32_t loop_idx;
            for (loop_idx = 0; loop_idx < 0x4000; loop_idx += 8) {
                uint16_t dsp_val[8];
                uint32_t val_idx;
                uint32_t base_ptr = (uint32_t)p_src;
                for (val_idx = 0; val_idx < 8; val_idx++) {
                    dsp_val[val_idx] = *p_src++;
                }
                printf("0x%08x: %04x %04x %04x %04x %04x %04x %04x %04x\n\r", (unsigned int)base_ptr, dsp_val[0], dsp_val[1], dsp_val[2], dsp_val[3], dsp_val[4], dsp_val[5], dsp_val[6], dsp_val[7]);
            }
        }
    }
    return;
}

void audio_service_dsp_memory_dump_init(void)
{
    exception_config_type callback_config;
    callback_config.init_cb = audio_service_dsp_memory_callback_init;
    callback_config.dump_cb = audio_service_dsp_memory_callback_dump;
    exception_register_callbacks(&callback_config);
    audio_common.assert_source = AUDIO_DSP_ASSERT_NONE;
    return;
}

void audio_service_dsp_to_mcu_interrupt(void)
{
    /* mask D2C interrupt and Ack D2M interrupt */
    hal_nvic_disable_irq(DSP22CPU_IRQn);
    audio_service_clean_dsp_wakeup_event();
    {
        uint16_t d2m_st = DSP_READ(SHARE_D2MSTA);
        if (d2m_st & (1 << 4)) {    /* Task 4 */
            uint16_t d2m_id = DSP_READ(DSP_D2M_TASK4);
            audio_service_dsp_to_mcu_lisr(d2m_id);
        }
        if (d2m_st & (1 << 5)) {    /* Task 5 */
            uint16_t d2m_id = DSP_READ(DSP_D2M_TASK5);
            audio_service_dsp_to_mcu_lisr(d2m_id);
        }
        if (d2m_st & (1 << 6)) {    /* Task 6 */
            uint16_t d2m_id = DSP_READ(DSP_D2M_TASK6);
            audio_service_dsp_to_mcu_lisr(d2m_id);
        }
        if (d2m_st & (1 << 7)) {    /* Task 7 : DSP 0xDD44 crash */
            uint16_t d2m_id = DSP_READ(DSP_D2M_TASK7);
            if (d2m_id == DSP_DSP_EXCEPTION_ID) {
                DSP_WRITE(DSP_MCU_STATUS, 0);   /* Freeze DSP */
                __disable_irq();
                audio_common.assert_source = AUDIO_DSP_ASSERT_FROM_DSP;
                assert(0);
            }
        }
    }
    /* unmask D2C interrupt */
    hal_nvic_enable_irq(DSP22CPU_IRQn);
    /* At this point, the next D2M interrupt is able to trigger MCU. */
    return;
}

static uint16_t audio_convert_dsp_sample(uint16_t frequency)
{
    uint16_t dsp_frequency;
    switch (frequency) {
        case AUDIO_COMMON_SAMPLING_RATE_8KHZ:
            dsp_frequency = ASP_FS_8K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_11_025KHZ:
            dsp_frequency = ASP_FS_11K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_12KHZ:
            dsp_frequency = ASP_FS_12K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_16KHZ:
            dsp_frequency = ASP_FS_16K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_22_05KHZ:
            dsp_frequency = ASP_FS_22K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_24KHZ:
            dsp_frequency = ASP_FS_24K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_32KHZ:
            dsp_frequency = ASP_FS_32K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_44_1KHZ:
            dsp_frequency = ASP_FS_44K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_48KHZ:
            dsp_frequency = ASP_FS_48K;
            break;
        case AUDIO_COMMON_SAMPLING_RATE_96KHZ:
            dsp_frequency = ASP_FS_96K;
            break;
        default:
            dsp_frequency = 0;
    }
    return dsp_frequency;
}

uint16_t audio_transfer_sample_rate(uint16_t sample_rate)
{
    uint16_t sample_rate_temp = 8000;
    switch (sample_rate) {
        case HAL_AUDIO_SAMPLING_RATE_8KHZ:
            sample_rate_temp = 8000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_11_025KHZ:
            sample_rate_temp = 11025;
            break;
        case HAL_AUDIO_SAMPLING_RATE_12KHZ:
            sample_rate_temp = 12000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_16KHZ:
            sample_rate_temp = 16000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_22_05KHZ:
            sample_rate_temp = 22050;
            break;
        case HAL_AUDIO_SAMPLING_RATE_24KHZ:
            sample_rate_temp = 24000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_32KHZ:
            sample_rate_temp = 32000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_44_1KHZ:
            sample_rate_temp = 44100;
            break;
        case HAL_AUDIO_SAMPLING_RATE_48KHZ:
            sample_rate_temp = 48000;
            break;
        default:
            return sample_rate_temp;
    }
    return  sample_rate_temp;
}

uint8_t audio_transfer_channel_number(uint16_t channel)
{
    uint8_t channel_temp = 2;
    switch (channel) {
        case HAL_AUDIO_MONO:
            channel_temp = 1;
            break;
        case HAL_AUDIO_STEREO:
            channel_temp = 2;
            break;
        default:
            return  channel_temp;
    }
    return  channel_temp;
}

#if defined(MTK_AUDIO_IIR_FILTER_ENABLE) || defined(__HAL_AUDIO_AWS_SUPPORT__)
static const uint16_t pcm_sd_playback_frame_length_table[9] = {
    512, 512, 512, 1024, 1024, 1024, 2048, 2048, 2048
};
#endif

static void audio_service_pcm_route_isr(void *common_data)
{
    {   /* Execute PCM route functions */
        int32_t func_id;
        for (func_id = 0; func_id < MAX_PCM_ROUTE_FUNCTIONS; func_id++) {
            if (audio_pcm_route_isr.num[func_id] != 0) {
                void *user_data = audio_pcm_route_isr.userdata[func_id];
                audio_pcm_route_isr.function[func_id](user_data);
            }
        }
    }
    {   /* DSP handshake */
        *DSP_TASK4_COSIM_HANDSHAKE = 0;
    }
    return;
}

void audio_pcm_route_start(void)
{
    if (audio_pcm_route_isr.pcm_route_id == 0)
        return;
    
    if (audio_pcm_route_isr.counter == 0) {
        /* Hook PCM route ISR */
        uint16_t mask    = AUDIO_DSP_POST_PROCESSING_ENABLE_MASK | AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_MASK;
        uint16_t control = AUDIO_DSP_POST_PROCESSING_ENABLE_ON   | AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_ON;

        audio_service_hook_isr(DSP_D2M_PCM_ROUTE_INT, audio_service_pcm_route_isr, NULL);
        audio_service_set_post_process_control(mask, control);        
    }
    audio_pcm_route_isr.counter++;
}

void audio_pcm_route_stop(void)
{
    if (audio_pcm_route_isr.pcm_route_id == 0)
        return;
    
    audio_pcm_route_isr.counter--;
    
    if (audio_pcm_route_isr.counter == 0) {
        /* Unhook PCM route ISR */
        uint16_t mask    = AUDIO_DSP_POST_PROCESSING_ENABLE_MASK | AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_MASK;
        uint16_t control = AUDIO_DSP_POST_PROCESSING_ENABLE_OFF  | AUDIO_DSP_POST_PROCESSING_PCM_ROUTE_OFF;

        audio_service_set_post_process_control(mask, control);
        audio_service_unhook_isr(DSP_D2M_PCM_ROUTE_INT);
    }
}

static void audio_dsp_playback_on(uint16_t active_type, uint16_t sample_rate)
{
    uint16_t isr_cycle = 0;
    uint16_t del_m = 0;
    uint16_t del_w = 0x20;
    uint16_t uFrameLen = ASP_FRAMELEN_PCM;
#if defined(__HAL_AUDIO_AWS_SUPPORT__)
    if ((audio_common.aws_flag == true) && (audio_common.aws_init_flag == true)) {
        if (audio_common.aws_clock_skew_flag == true) {
            /* [AWS] hook aws pcm route isr */
            log_hal_info("[AWS] hook aws pcm route isr\r\n");
            int32_t pcm_route_id = audio_pcm_route_register_isr(audio_service_a2dp_aws_pcm_route_isr, NULL, AUDIO_PCM_ROUTE_ORDER_AWS);
            if (pcm_route_id < 0) {
              log_hal_info("[AWS] register PCM route fail: %d\r\n", (int)pcm_route_id);
            }
            audio_common.aws_pcm_route_id = pcm_route_id;
        }
        if (active_type == ASP_TYPE_PCM_HI) {
            /*set SD playback if AWS + MP3*/
            active_type = ASP_TYPE_PCM_SD_PLAYBACK;
        }
    }
#endif  /*(__HAL_AUDIO_AWS_SUPPORT__) */
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    {   /* Check & Apply IIR If Needed */
        int32_t apply_iir;
        uint32_t sr_val = (uint32_t)audio_transfer_sample_rate(sample_rate);
        audio_iir_generate_coefficient_table(sr_val);
        apply_iir = is_applying_audio_iir(sr_val);
        audio_common.apply_iir = apply_iir;
        if (apply_iir) {
            audio_write_iir_coefficients_to_dsp();
            audio_iir_start();
            if (active_type == ASP_TYPE_PCM_HI) {
                active_type = ASP_TYPE_PCM_SD_PLAYBACK;
            }
        }
    }
#endif
    audio_common.active_type = active_type;
    audio_common.is_dsp_codec = false;
    switch (active_type) {
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE) || defined(__HAL_AUDIO_AWS_SUPPORT__)
        case ASP_TYPE_PCM_SD_PLAYBACK:
            sample_rate = UPPER_BOUND(sample_rate, AUDIO_COMMON_SAMPLING_RATE_48KHZ);
            {    /* IIR filter */
                uFrameLen = pcm_sd_playback_frame_length_table[sample_rate];
            }
            audio_pcm_set_dsp_sd_task_playback(uFrameLen);
            audio_common.sd_frame_len=uFrameLen;
            del_m = uFrameLen;
            isr_cycle = del_m;
            break;
#endif
        case ASP_TYPE_PCM_HI:
            del_m = ASP_DELM_PCM_HIGFS;
            if (((*DSP_AUDIO_PP_CTRL_BASE) & 0x0020) == 0) {
                if (sample_rate <= AUDIO_COMMON_SAMPLING_RATE_12KHZ) {
                    del_m = ASP_DELM_PCM_LOWFS;
                } else if (sample_rate <= AUDIO_COMMON_SAMPLING_RATE_24KHZ) {
                    del_m = ASP_DELM_PCM_MEDFS;
                }
            }
            isr_cycle = uFrameLen;
            break;
        case ASP_TYPE_I2SBypass:
            uFrameLen = ASP_FRAMELEN_I2S;
            del_m = ASP_DELM_I2SBypass;
            isr_cycle = del_m;
            break;
        case ASP_TYPE_SBC_DEC:
            {
                uFrameLen = ASP_FRAMELEN_SBC_DEC;
                del_m = ASP_DELM_SBC_DEC;
            }
            isr_cycle = del_m;
            audio_common.is_dsp_codec = true;
            break;
        case ASP_TYPE_AAC_DEC:
            {
                uFrameLen = ASP_FRAMELEN_AAC_DEC;
                del_m = ASP_DELM_AAC_DEC;
            }
            isr_cycle = del_m;
            audio_common.is_dsp_codec = true;
            break;
        default:
            break;
    }
    audio_common.frame_sample_count = uFrameLen >> 1;   /* One pair of L / R counts as one sample. */
    log_hal_info("frame sample count = %d\r\n", (int)audio_common.frame_sample_count);
    *DSP_AUDIO_ASP_TYPE_ADDR      = active_type;
    *DSP_AUDIO_ASP_FS_ADDR        = audio_convert_dsp_sample(sample_rate);
    *DSP_AUDIO_ASP_DEL_W_ADDR     = del_w;
    *DSP_AUDIO_ASP_DEL_MARGIN_W_ADDR     = del_m;
    *DSP_AUDIO_DEC_FRAME_LENGTH = uFrameLen;
    *DSP_AUDIO_SD_ISR_CYCLE  = isr_cycle;
    *DSP_ASP_FORCE_KT_8K = 0x8000;
    *DSP_AUDIO_CLOCK_COUNTER = 0;
    *DSP_AUDIO_CUR_GAIN = 0;
    audio_common.mcu_sample_counter = 0;
    audio_common.dsp_clock_counter = 0;

    audio_pcm_route_start();

    if (afe.audio_path_type == HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE) {
        *DSP_SPH_8K_DIS_VBI_CTRL |= 0x0001;
    }
#if defined(__HAL_AUDIO_AWS_SUPPORT__)
    if (audio_common.aws_flag) {
        *DSP_ADD_POINT_IDX = 0;
        *DSP_AUD_CLK_SKEW_PIN_VAL = 0;
        *DSP_AUD_CLK_SKEW_PON_VAL = 0;
        *DSP_AUDIO_ASP_COMMON_FLAG_1 |= (1 << 6);
        /* Do VBI_RESET in other API */
    } else
#endif  /* __HAL_AUDIO_AWS_SUPPORT__ */
    {
        VBI_RESET();
    }
    return;
}

static void audio_dsp_playback_off()
{
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    if (audio_common.apply_iir) {
        audio_iir_stop();
        audio_common.apply_iir = 0;
    }
#endif

    audio_pcm_route_stop();

#if defined(MTK_AUDIO_IIR_FILTER_ENABLE) || defined(__HAL_AUDIO_AWS_SUPPORT__)
    if (audio_common.active_type == ASP_TYPE_PCM_SD_PLAYBACK) {
        audio_pcm_stop_dsp_sd_task_playback();
    }
#endif
#if defined(__HAL_AUDIO_AWS_SUPPORT__)
    if (audio_common.aws_clock_skew_flag == true) {
        int32_t pcm_route_id = audio_common.aws_pcm_route_id;
        int32_t result = audio_pcm_route_deregister_isr(pcm_route_id);
        if (result < 0) {
            log_hal_info("[AWS] deregister PCM route fail: %d\r\n", (int)result);
        }
        audio_common.aws_pcm_route_id = 0;
    }
#endif /* defined(__HAL_AUDIO_AWS_SUPPORT__) */
    if (afe.audio_path_type == HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE) {
        *DSP_SPH_8K_DIS_VBI_CTRL &= ~0x0001;
    }
    *DSP_AUDIO_ASP_TYPE_ADDR      = ASP_TYPE_VOICE;
    *DSP_AUDIO_ASP_FS_ADDR        = ASP_FS_8K;
    *DSP_AUDIO_ASP_DEL_W_ADDR     = 95;
    *DSP_AUDIO_ASP_DEL_MARGIN_W_ADDR = 95;
    *DSP_AUDIO_DEC_FRAME_LENGTH = 0;
    *DSP_AUDIO_SD_ISR_CYCLE    = *DSP_AUDIO_SD_ISR_CYCLE;
    *DSP_ASP_FORCE_KT_8K = 0x8000;
    VBI_END();
    audio_common.is_dsp_codec = false;
    return;
}

void audio_playback_on(uint16_t active_type, uint16_t sample_rate)
{
#ifdef HAL_I2S_DSP_DECODE_CONCURRENT_ENABLED
    if (((active_type == ASP_TYPE_SBC_DEC) || (active_type == ASP_TYPE_AAC_DEC)) && i2s_is_dsp_decode_concurrency()) {
        i2s_rx_dsp_decode(true);
    }
#endif
    afe_audio_digital_on(sample_rate);
    audio_dsp_playback_on(active_type, sample_rate);
    afe_switch_tx_signal_path(true);
    if (active_type != ASP_TYPE_I2SBypass) {
        afe_audio_device_volume_update();
    }
}

void audio_playback_off(void)
{
    afe_audio_analog_device_off();
    afe_switch_tx_signal_path(false);
    audio_dsp_playback_off();
    afe_audio_digital_off();
#ifdef HAL_I2S_DSP_DECODE_CONCURRENT_ENABLED
    i2s_rx_dsp_decode(false);
#endif
}

uint16_t audio_get_id(void)
{
    uint16_t i = 0;
    for (i = 0; i < MAX_AUDIO_FUNCTIONS; i++) {
        if ((audio_isr.audio_id & (1 << i)) == 0) {
            audio_isr.audio_id |= (1 << i);
            break;
        }
    }
    return i;
}

void audio_free_id(uint16_t audio_id)
{
    audio_isr.audio_id &= ~(1 << audio_id);
}

void audio_service_wake_dsp(bool flag)
{
    if (flag) {
        afe_restore_register(); /*restore AFE register*/
        hal_dsp_topsm_get_resource(RM_MODEM_DSP_3, true);
        if (mtcmos_control.MD2G_Count == 0) {
            dsp_audio_fw_reboot();
        }else{
            dsp_audio_fw_init();
        }
        audio_service_dsp_wakeup_setting(flag);
#ifdef HAL_SLEEP_MANAGER_ENABLED
        cm4_topsm_lock_MD2G_MTCMOS();
#endif
    } else {
        audio_service_dsp_wakeup_setting(flag);
        hal_dsp_topsm_get_resource(RM_MODEM_DSP_3, false);
        afe_backup_register(); /*Backup AFE register*/
        audio_service_clean_dsp_wakeup_event();
#ifdef HAL_SLEEP_MANAGER_ENABLED
        cm4_topsm_unlock_MD2G_MTCMOS();
#endif
    }
}

void audio_service_setflag(uint16_t audio_id)
{
    uint32_t savedmask;
    if (AUDIO_GET_CHIP_ID() == 0) {     /*E1 chip*/
        pmu_ctrl_driver_power(DRIVER_AUDIO, 1);
    }
    if (audio_common.running_state == 0) {
#ifdef HAL_DVFS_MODULE_ENABLED
        //audio_register_dsp_dvfs(true);
#endif
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
        if (!audio_lowpower_check_status())
#endif
        {
            audio_common.sleep_handle = hal_sleep_manager_set_sleep_handle("audio");
            hal_sleep_manager_lock_sleep(audio_common.sleep_handle);
        }
        audio_service_wake_dsp(true);
    }
    *(volatile uint16_t *)(0x82255D5E) = 0x0000;  //temp solution to clear speech debug sheriff
    savedmask = save_and_set_interrupt_mask();
    audio_common.running_state |= (1 << audio_id);
    restore_interrupt_mask(savedmask);
}

void audio_service_clearflag(uint16_t audio_id)
{
    uint32_t savedmask;
    savedmask = save_and_set_interrupt_mask();
    audio_common.running_state &= ~(1 << audio_id);
    restore_interrupt_mask(savedmask);
    if (audio_common.running_state == 0) {
#ifdef HAL_AUDIO_LOW_POWER_ENABLED
        if (!audio_lowpower_check_status())
#endif
        {
            hal_sleep_manager_unlock_sleep(audio_common.sleep_handle);
            hal_sleep_manager_release_sleep_handle(audio_common.sleep_handle);
        }
        audio_service_wake_dsp(false);
#ifdef HAL_DVFS_MODULE_ENABLED
        audio_register_dsp_dvfs(false);
#endif
        if (AUDIO_GET_CHIP_ID() == 0) {     /*E1 chip*/
            pmu_ctrl_driver_power(DRIVER_AUDIO, 0);
        }
    }
}

void audio_clear_dsp_common_flag(void)
{
    *DSP_AUDIO_ASP_COMMON_FLAG_1 = 0;
}


/*pcm interface and slave i2s */
void audio_turn_on_pcm_interface(bool is_wideband)
{
    // Zero-pending
    *DSP_SPH_BT_CTRL = 0x0002;
    *DSP_SPH_BT_MODE = 0;

    *DSP_SPH_BT_CTRL &= ~(0x0010);   // MSB first
    *DSP_SPH_BT_CTRL |= (0x100 & 0xff00); //0x100 is UL default setting 0dB

    *DSP_SPH_8K_CTRL = (*DSP_SPH_8K_CTRL & 0x00FF) | 0x2000;
    if (is_wideband) {
        *DSP_SPH_8K_CTRL |= (1 << 10);
    } else {
        *DSP_SPH_8K_CTRL &= ~(1 << 10);
    }
}

void audio_turn_off_pcm_interface()
{
    *DSP_SPH_8K_CTRL = (*DSP_SPH_8K_CTRL & ~0x2000) | 0x8000;
}

/*idma read/write common functions */
void audio_idma_read_from_dsp(uint16_t *dst, volatile uint16_t *src, uint32_t length)
{
    uint32_t i;
    for (i = length; i > 0; i--) {
        *dst++ = *src++;
    }
}

void audio_idma_read_from_dsp_dropR(uint16_t *dst, volatile uint16_t *src, uint32_t length)
{
    uint32_t i;
    for (i = length; i > 0; i--) {
        *dst++ = *src;
        src += 2;
    }
}

void audio_idma_write_to_dsp(volatile uint16_t *dst, uint16_t *src, uint32_t length)
{
    uint32_t i;
    for (i = length; i > 0; i--) {
        *dst++ = *src++;
    }
}

void audio_idma_write_to_dsp_duplicate(volatile uint16_t *dst, uint16_t *src, uint32_t length)
{
    uint32_t i;

    for (i = length; i > 0; i -= 2) {
        *dst++ = *src;
        *dst++ = *src++;
    }
}

void audio_idma_fill_to_dsp(volatile uint16_t *dst, uint16_t value, uint32_t length)
{
    uint32_t i;

    for (i = length; i > 0; i--) {
        *dst++ = value;
    }
}
/*@brief     circular buffer(ring buffer) implemented by mirroring, which keep an extra bit to distinguish empty and full situation. */
uint32_t ring_buffer_get_data_byte_count(ring_buffer_information_t *p_info)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t data_byte_count;
    if (write_pointer >= read_pointer) {
        data_byte_count = write_pointer - read_pointer;
    } else {
        data_byte_count = (buffer_byte_count << 1) - read_pointer + write_pointer;
    }
    return data_byte_count;
}

uint32_t ring_buffer_get_space_byte_count(ring_buffer_information_t *p_info)
{
    return p_info->buffer_byte_count - ring_buffer_get_data_byte_count(p_info);
}

void ring_buffer_get_write_information(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t space_byte_count  = ring_buffer_get_space_byte_count(p_info);
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t tail_byte_count;
    if (write_pointer < buffer_byte_count) {
        *pp_buffer = buffer_pointer + write_pointer;
        tail_byte_count = buffer_byte_count - write_pointer;
    } else {
        *pp_buffer = buffer_pointer + write_pointer - buffer_byte_count;
        tail_byte_count = (buffer_byte_count << 1) - write_pointer;
    }
    *p_byte_count = MINIMUM(space_byte_count, tail_byte_count);
    return;
}

void ring_buffer_get_read_information(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t data_byte_count   = ring_buffer_get_data_byte_count(p_info);
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t tail_byte_count;
    if (read_pointer < buffer_byte_count) {
        *pp_buffer = buffer_pointer + read_pointer;
        tail_byte_count = buffer_byte_count - read_pointer;
    } else {
        *pp_buffer = buffer_pointer + read_pointer - buffer_byte_count;
        tail_byte_count = (buffer_byte_count << 1) - read_pointer;
    }
    *p_byte_count = MINIMUM(data_byte_count, tail_byte_count);
    return;
}

void ring_buffer_write_done(ring_buffer_information_t *p_info, uint32_t write_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t buffer_end        = buffer_byte_count << 1;
    uint32_t write_pointer     = p_info->write_pointer + write_byte_count;
    p_info->write_pointer = write_pointer >= buffer_end ? write_pointer - buffer_end : write_pointer;
    return;
}

void ring_buffer_read_done(ring_buffer_information_t *p_info, uint32_t read_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t buffer_end        = buffer_byte_count << 1;
    uint32_t read_pointer      = p_info->read_pointer + read_byte_count;
    p_info->read_pointer = read_pointer >= buffer_end ? read_pointer - buffer_end : read_pointer;
    return;
}

#ifdef MTK_BT_A2DP_AAC_ENABLE
/*@brief     circular buffer(ring buffer) implemented by keeping one slot open. Full buffer has at most (size - 1) slots. */
uint32_t ring_buffer_get_data_byte_count_non_mirroring(ring_buffer_information_t *p_info)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t data_byte_count;

    if (write_pointer >= read_pointer) {
        data_byte_count = write_pointer - read_pointer;
    } else {
        data_byte_count = buffer_byte_count - read_pointer + write_pointer;
    }
    return data_byte_count;
}

uint32_t ring_buffer_get_space_byte_count_non_mirroring(ring_buffer_information_t *p_info)
{
    return p_info->buffer_byte_count - ring_buffer_get_data_byte_count_non_mirroring(p_info);
}

void ring_buffer_get_write_information_non_mirroring(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;     //buffer size
    uint32_t space_byte_count  = ring_buffer_get_space_byte_count_non_mirroring(p_info) - 2;  //space two bytes(one word) empty for DSP operation
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t write_pointer     = p_info->write_pointer;
    uint32_t tail_byte_count;

    tail_byte_count = buffer_byte_count - write_pointer;
    *pp_buffer = buffer_pointer + write_pointer;
    *p_byte_count = MINIMUM(space_byte_count, tail_byte_count);
    return;
}

void ring_buffer_get_read_information_non_mirroring(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t data_byte_count   = ring_buffer_get_data_byte_count_non_mirroring(p_info);
    uint8_t *buffer_pointer    = p_info->buffer_base_pointer;
    uint32_t read_pointer      = p_info->read_pointer;
    uint32_t tail_byte_count;

    *pp_buffer = buffer_pointer + read_pointer;
    tail_byte_count = buffer_byte_count - read_pointer;
    *p_byte_count = MINIMUM(data_byte_count, tail_byte_count);
    return;
}


void ring_buffer_write_done_non_mirroring(ring_buffer_information_t *p_info, uint32_t write_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t write_pointer     = p_info->write_pointer + write_byte_count;
    p_info->write_pointer = write_pointer == buffer_byte_count ? write_pointer - buffer_byte_count : write_pointer;
    return;
}

void ring_buffer_read_done_non_mirroring(ring_buffer_information_t *p_info, uint32_t read_byte_count)
{
    uint32_t buffer_byte_count = p_info->buffer_byte_count;
    uint32_t read_pointer      = p_info->read_pointer + read_byte_count;
    p_info->read_pointer = read_pointer == buffer_byte_count ? read_pointer - buffer_byte_count : read_pointer;
    return;
}
#endif /*MTK_BT_A2DP_AAC_ENABLE*/

void audio_get_last_two_write_sample(ring_buffer_information_t *p_info, uint16_t *last_sample, uint16_t *second_last_sample)
{
    uint32_t write_byte_count = 0;
    uint32_t i = 0;
    uint8_t  temp[4] = {0};
    uint8_t *p_src;

    ring_buffer_get_write_information(p_info, &p_src, &write_byte_count);
    uint32_t count = p_src - p_info->buffer_base_pointer;
    if (count < 4) {
        for (i = 0; i < 4 ; i++) {
            uint32_t index = p_info->buffer_byte_count + i + count - 4;
            if (index >= p_info->buffer_byte_count) {
                index -= p_info->buffer_byte_count;
            }
            temp[i] = * (p_src + index);
        }
    } else {
        memcpy(temp, p_src - 4, 4);
    }
    memcpy(second_last_sample, temp, 2);
    memcpy(last_sample, temp+2, 2);

    return;

}

void audio_sd_write_sample_to_dsp(ring_buffer_information_t *p_dsp_info, uint32_t byte_count, uint16_t last_sample, uint16_t second_last_sample, bool mono_to_stereo)
{
    volatile uint16_t *p_dsp_buf = NULL;
    uint32_t write_byte_cnt = 0;
    uint32_t remain_byte_cnt = 0;
    uint32_t move_byte_cnt = 0;
    uint32_t move_word_cnt = 0;
    uint32_t i = 0;
    uint16_t l_data = 0;
    uint16_t r_data = 0;
    uint32_t loop_idx;
    uint32_t loop_cnt = 2;

    if (mono_to_stereo) {
        l_data = r_data = last_sample;

    } else {
        l_data = second_last_sample;
        r_data = last_sample;
    }
    remain_byte_cnt = byte_count & ~0x1;

    for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
        p_dsp_info->read_pointer = *DSP_PCM_SD_PLAYBACK_DM_DSP_R_PTR;
        ring_buffer_get_write_information(p_dsp_info, (uint8_t **)&p_dsp_buf, &write_byte_cnt);
        write_byte_cnt &= ~0x1;
        move_byte_cnt = MINIMUM(write_byte_cnt, remain_byte_cnt);
        move_word_cnt = move_byte_cnt >> 1;
        move_word_cnt &= ~0x1;
        if (move_word_cnt > 0) {
            for (i = move_word_cnt; i > 0; i -= 2) {
                *p_dsp_buf++ = l_data;
                *p_dsp_buf++ = r_data;
            }
            remain_byte_cnt -= move_byte_cnt;
            ring_buffer_write_done(p_dsp_info, move_byte_cnt);
            *DSP_PCM_SD_PLAYBACK_DM_MCU_W_PTR = p_dsp_info->write_pointer;
        }
        if (remain_byte_cnt == 0) {
            break;
        }
    }

    return ;
}

static uint32_t calculate_audio_sample_counter_increment(uint16_t curr_cnt, uint16_t prev_cnt, uint32_t mcu_cnt, int32_t inc_cnt)
{
    uint32_t diff = curr_cnt >= prev_cnt ? (uint32_t)(curr_cnt - prev_cnt) : 0x10000 - (uint32_t)prev_cnt + (uint32_t)curr_cnt;
    uint32_t smpl_cnt = mcu_cnt + diff * 6 + inc_cnt;
    return smpl_cnt;
}

uint32_t audio_dsp_get_audio_sample_counter(void)
{
    uint32_t saved_mask = save_and_set_interrupt_mask();
    uint16_t curr_cnt = *DSP_AUDIO_CLOCK_COUNTER;
    uint16_t prev_cnt = audio_common.dsp_clock_counter;
    uint32_t mcu_cnt = audio_common.mcu_sample_counter;
    int32_t inc_cnt = 0;
    uint32_t smpl_cnt = calculate_audio_sample_counter_increment(curr_cnt, prev_cnt, mcu_cnt, inc_cnt);
    restore_interrupt_mask(saved_mask);
    return smpl_cnt;
}

void audio_dsp_update_audio_counter(int32_t inc_cnt)
{
    uint32_t saved_mask = save_and_set_interrupt_mask();
    uint16_t curr_cnt = *DSP_AUDIO_CLOCK_COUNTER;
    uint16_t prev_cnt = audio_common.dsp_clock_counter;
    uint32_t mcu_cnt = audio_common.mcu_sample_counter;
    uint32_t smpl_cnt = calculate_audio_sample_counter_increment(curr_cnt, prev_cnt, mcu_cnt, inc_cnt);
    audio_common.mcu_sample_counter = smpl_cnt;
    audio_common.dsp_clock_counter = curr_cnt;
    restore_interrupt_mask(saved_mask);
    return;
}

/*********************************************************|
|   Channel Assignment                                    |
|   SHERIF: AUDIO_CH_ASSIGNMENT                           |
|   bit 0:  0 => L channel assigns to L DAC               |
|           1 => R channel assigns to L DAC               |
|   bit 1:  0 => R channel assigns to R DAC               |
|           1 => L channel assigns to R DAC               |
|*********************************************************/
void audio_dsp_assign_audio_channel(hal_audio_channel_number_t channel_number)
{
    uint16_t channel_value = *DSP_AUDIO_CHANNEL_ASSIGNMENT;
    switch (channel_number) {
        case HAL_AUDIO_STEREO_BOTH_L_CHANNEL:
            channel_value &= ~0x1;  /* L channel to L DAC */
            channel_value |=  0x2;  /* L channel to R DAC */
            break;
        case HAL_AUDIO_STEREO_BOTH_R_CHANNEL:
            channel_value |=  0x1;  /* R channel to L DAC */
            channel_value &= ~0x2;  /* R channel to R DAC */
            break;
        case HAL_AUDIO_STEREO_BOTH_L_R_SWAP:
            channel_value |=  0x3;  /* R channel to L DAC */
            break;
        case HAL_AUDIO_MONO:
        case HAL_AUDIO_STEREO:
        default:
            channel_value &= ~0x3;  /* L / R channel to L / R DAC respectively */
            break;
    }
    *DSP_AUDIO_CHANNEL_ASSIGNMENT = channel_value;
    return;
}

void audio_service_set_post_process_control(uint16_t mask, uint16_t control)
{
    uint32_t saved_mask = save_and_set_interrupt_mask();
    uint16_t control_cache = *DSP_AUDIO_PP_CTRL;
    control_cache = (control_cache & ~mask) | (control & mask);
    *DSP_AUDIO_PP_CTRL = control_cache;
    restore_interrupt_mask(saved_mask);
    return;
}

uint32_t audio_service_get_frame_sample_count(void)
{
    return audio_common.frame_sample_count;
}

#ifdef MTK_BT_A2DP_AAC_ENABLE
const uint8_t AAC_ADTS_HEADER[ADTS_HEADER_LENGTH] = { 0xFF, 0xF9, 0x40, 0x20, 0x00, 0x1F, 0xFC };
const uint8_t aac_silence_data_mono[SILENCE_DATA_LENGTH] = { 0x01, 0x40, 0x20, 0x07, 0xF6, 0x00, 0x00, 0x00, 0x00 };
const uint8_t aac_silence_data_stereo[SILENCE_DATA_LENGTH] = { 0x21, 0x00, 0x49, 0x90, 0x02, 0x19, 0x00, 0x23, 0x80 };
/* aac */
void aac_get_silence_pattern(uint32_t sample_rate, uint16_t channel_number, uint8_t *silence_frame)
{
    uint8_t frequence_index;
    memcpy(silence_frame, AAC_ADTS_HEADER, ADTS_HEADER_LENGTH);

    switch (sample_rate) {
        case HAL_AUDIO_SAMPLING_RATE_8KHZ:
            frequence_index = 11;
            break;
        case HAL_AUDIO_SAMPLING_RATE_11_025KHZ:
            frequence_index = 10;
            break;
        case HAL_AUDIO_SAMPLING_RATE_12KHZ:
            frequence_index = 9;
            break;
        case HAL_AUDIO_SAMPLING_RATE_16KHZ:
            frequence_index = 8;
            break;
        case HAL_AUDIO_SAMPLING_RATE_22_05KHZ:
            frequence_index = 7;
            break;
        case HAL_AUDIO_SAMPLING_RATE_24KHZ:
            frequence_index = 6;
            break;
        case HAL_AUDIO_SAMPLING_RATE_32KHZ:
            frequence_index = 5;
            break;
        case HAL_AUDIO_SAMPLING_RATE_44_1KHZ:
            frequence_index = 4;
            break;
        case HAL_AUDIO_SAMPLING_RATE_48KHZ:
            frequence_index = 3;
            break;
        default:
            frequence_index = 0;
            break;
    }
    /* fill AAC silence data */
    if(channel_number == 1) {
        memcpy(silence_frame+ADTS_HEADER_LENGTH, aac_silence_data_mono, SILENCE_DATA_LENGTH);
    } else {
        memcpy(silence_frame+ADTS_HEADER_LENGTH, aac_silence_data_stereo, SILENCE_DATA_LENGTH);
    }
    /* update AAC ADTS header */
    silence_frame[2] |= (frequence_index << 2);
    silence_frame[3] |= ((channel_number) << 6) | ((SILENCE_TOTAL_LENGTH) >> 11);
    silence_frame[4] = ((SILENCE_TOTAL_LENGTH) >> 3) & 0xFF;
    silence_frame[5] |= ((SILENCE_TOTAL_LENGTH) & 0x07) << 5;
    memcpy(audio_common.aac_silence_pattern, silence_frame, SILENCE_TOTAL_LENGTH);

    return;
}

#endif
void audio_service_aws_set_flag(bool flag)
{
#if defined(__HAL_AUDIO_AWS_SUPPORT__)
    log_hal_info("[AWS] AWS flag: %d\r\n", flag);
    audio_common.aws_flag = flag;
#endif  /* __HAL_AUDIO_AWS_SUPPORT__ */
    return;
}

bool audio_service_aws_query_flag(void)
{
#if defined(__HAL_AUDIO_AWS_SUPPORT__)
    return audio_common.aws_flag;
#else
    return false;
#endif
}

void audio_service_aws_set_clock_skew(bool flag)
{
#if defined(__HAL_AUDIO_AWS_SUPPORT__)
    log_hal_info("[AWS] aws_clock_skew_flag: %d\r\n", flag);
    audio_common.aws_clock_skew_flag = flag;
#endif  /* __HAL_AUDIO_AWS_SUPPORT__ */
    return;
}

void audio_service_aws_set_initial_sync(void)
{
#if defined(__HAL_AUDIO_AWS_SUPPORT__)
    if (audio_common.aws_flag) {
        VBI_RESET();
    }
#endif  /* __HAL_AUDIO_AWS_SUPPORT__ */
    return;
}

#if defined(__HAL_AUDIO_AWS_SUPPORT__)
static uint16_t audio_service_get_frame_length(aws_codec_type_t codec_type)
{
    uint16_t frm_len;
    switch (codec_type) {
        case AWS_CODEC_TYPE_SBC_FORMAT:
            frm_len = ASP_FRAMELEN_SBC_DEC;
            break;
        case AWS_CODEC_TYPE_AAC_FORMAT:
            frm_len = ASP_FRAMELEN_AAC_DEC;
            break;
        case AWS_CODEC_TYPE_MP3_FORMAT:
        case AWS_CODEC_TYPE_PCM_FORMAT:
            frm_len = pcm_sd_playback_frame_length_table[AUDIO_COMMON_SAMPLING_RATE_48KHZ];
            break;
        default:
            frm_len = 0;
            break;
    }
    return frm_len;
}

static void audio_service_aws_src_process (int16_t *p_in, uint32_t in_byte_cnt, int16_t *p_ou, uint32_t ou_byte_cnt)
{   /* Input / output channel count are fixed to stereo */
    /* Part 1: Write first samples to output samples */
    uint32_t in_smpl_cnt = (in_byte_cnt - 4) / sizeof(int16_t) / 2; // minus 4 bytes: first sample(L/R) is excluded
    uint32_t ou_smpl_cnt = (ou_byte_cnt - 4) / sizeof(int16_t) / 2; // minus 4 bytes: first sample(L/R) is excluded
    *p_ou++ = *p_in++; // assign first frame buffer L sample to SRC buffer
    *p_ou++ = *p_in++; // assign first frame buffer R sample to SRC buffer
    if (in_smpl_cnt > 0 && ou_smpl_cnt > 0) {
        {   /* Part 2: Write normal output samples */
            uint32_t loop_idx;
            for (loop_idx = 1; loop_idx < ou_smpl_cnt; loop_idx++) {
                uint32_t phase = (loop_idx * in_smpl_cnt << AWS_SRC_PHASE_BITS) / ou_smpl_cnt;
                uint32_t idx_lo = ((phase >> AWS_SRC_PHASE_BITS) - 1) << 1;
                int32_t val_lo_L = (int32_t)p_in[idx_lo + 0];
                int32_t val_lo_R = (int32_t)p_in[idx_lo + 1];
                int32_t val_hi_L = (int32_t)p_in[idx_lo + 2];
                int32_t val_hi_R = (int32_t)p_in[idx_lo + 3];
                phase &= AWS_SRC_PHASE_MASK;
                *p_ou++ = (int16_t)AWS_SRC_INTERPOLATION(val_lo_L, val_hi_L, (int32_t)phase);
                *p_ou++ = (int16_t)AWS_SRC_INTERPOLATION(val_lo_R, val_hi_R, (int32_t)phase);
            }
        }
        {   /* Part 3: Write last output samples & update previous samples */
            int16_t val_idx = (in_smpl_cnt - 1) << 1;
            int16_t val_L = p_in[val_idx + 0];
            int16_t val_R = p_in[val_idx + 1];
            *p_ou++ = val_L;
            *p_ou++ = val_R;
        }
    }
    return;
}

static int32_t audio_service_a2dp_aws_sbc_get_media_payload_sample_count (uint8_t *buffer, uint32_t *p_smpl_cnt, uint32_t *p_byte_cnt)
{
    int32_t result = -1;
    uint32_t smpl_cnt = 0;
    uint32_t available_byte_cnt = *p_byte_cnt;
    uint32_t consumed_byte_cnt = 0;
    if (available_byte_cnt > 0) { /* Skip 1 byte packet header */
        buffer++;
        consumed_byte_cnt++;
        available_byte_cnt--;
    }
    while (available_byte_cnt >= 5) {
        uint8_t header[5];
        memcpy(header, buffer, 5);
        result = 0;
        if (header[0] != 0x9C) {    /* Invalid sync byte */
            result = -2;
            break;
        } else {
            uint32_t setting = (uint32_t)header[1];
            uint32_t bitpool = (uint32_t)header[2];
            uint32_t consumed_len = 0;
            if (bitpool == 0 && setting == 0x33 && header[3] == 0x77) { /* Silence frame, 5 bytes */
                if (header[4] > 128) {  /* Invalid silence sample count */
                    result = -3;
                    break;
                } else {
                    consumed_len = 5;
                    smpl_cnt += header[4];
                }
            } else if (bitpool >= 2 && bitpool <= 250) {
                uint32_t equation_type;
                uint32_t nrof_channels;
                uint32_t join;
                uint32_t nrof_subbands;
                uint32_t nrof_blocks;
                {   /* Channel mode : equation_type, nrof_channels, join */
                    uint32_t channel_mode = (setting >> 2) & 0x03;
                    if (channel_mode == 0x00) { /* MONO */
                        equation_type = 0;
                        nrof_channels = 1;
                        join = 0;
                    } else if (channel_mode == 0x01) { /* DUAL_CHANNEL */
                        equation_type = 0;
                        nrof_channels = 2;
                        join = 0;
                    } else if (channel_mode == 0x02) { /* STEREO */
                        equation_type = 1;
                        nrof_channels = 2;
                        join = 0;
                    } else { /* JOINT_STEREO */
                        equation_type = 1;
                        nrof_channels = 2;
                        join = 1;
                    }
                }
                nrof_subbands = (setting & 0x01) == 0 ? 4 : 8;
                {   /* Blocks : nrof_blocks */
                    uint32_t blocks = (setting >> 4) & 0x03;
                    if (blocks == 0) {
                        nrof_blocks = 4;
                    } else if (blocks == 1) {
                        nrof_blocks = 8;
                    } else if (blocks == 2) {
                        nrof_blocks = 12;
                    } else {
                        nrof_blocks = 16;
                    }
                }
                {   /* Calculate frame length & sample count */
                    uint32_t tmp;
                    if (equation_type == 0) {
                        /* channel_mode == MONO || channel_mode == DUAL_CHANNEL */
                        /* frame_length = 4 + (4 * nrof_subbands * nrof_channels) / 8 + ceiling(nrof_blocks * nrof_channels * bitpool / 8) */
                        tmp = nrof_blocks * nrof_channels * bitpool;
                    } else {
                        /* channel_mode == STEREO || channel_mode == JOINT_STEREO */
                        /* frame_length = 4 + (4 * nrof_subbands * nrof_channels) / 8 + ceiling((join * nrof_subbands + nrof_blocks * bitpool) / 8) */
                        tmp = join * nrof_subbands + nrof_blocks * bitpool;
                    }
                    consumed_len = tmp >> 3;
                    if ((consumed_len << 3) < tmp) {
                        consumed_len++;
                    }
                    consumed_len += 4 + (nrof_subbands * nrof_channels >> 1);
                    if (consumed_len <= available_byte_cnt) {   /* Check if input buffer size is enough or not */
                        smpl_cnt += nrof_blocks * nrof_subbands;
                    }
                }
            } else {    /* Invalid bitpool value */
                result = -4;
                break;
            }
            consumed_byte_cnt += consumed_len;
            available_byte_cnt -= consumed_len;
        }
    }
    *p_smpl_cnt = smpl_cnt;
    *p_byte_cnt = consumed_byte_cnt;
    return result;
}

void audio_service_a2dp_aws_pcm_route_isr(void *data)
{
    { /*skip aws pcm route 1. if clock skew turns off temporary 2. prompt hasn't call stop to deregister aws pcm route*/
        if ((audio_common.aws_skip_clk_skew_flag == true) || (audio_common.aws_flag == false)) {
            return;
        }
    }
    uint16_t word_cnt = *DSP_PCM_ROUTE_DATA_LENGTH & 0x7FFF;
    uint16_t dsp_addr = *DSP_PCM_ROUTE_DATA_ADDRESS;
    uint16_t page_num = *DSP_PCM_ROUTE_DATA_PAGENUM;
    uint16_t *p_frm_buf = audio_common.pcm_route_frame_buffer;
    uint16_t *p_src_buf = audio_common.pcm_route_src_buffer;
    volatile uint16_t *p_dsp_buf = DSP_DM_ADDR(page_num, dsp_addr);
    ring_buffer_information_t *p_ring = &audio_common.pcm_route_ring_buffer;
    uint32_t byte_cnt = (uint32_t)word_cnt * sizeof(uint16_t);
    int32_t comp_cnt = 0;
    audio_common.aws_pcm_route_counter++;
#if defined(AWS_DEBUG_CODE)
    /* log_hal_info("[AWS] PCM route ISR\r\n"); */
#endif  /* defined(AWS_DEBUG_CODE) */
    {   /* DSP buffer --> Frame buffer */
        audio_idma_read_from_dsp(p_frm_buf, p_dsp_buf, word_cnt);
    }
    {   /* Frame buffer --> Ring buffer */
        uint8_t *p_src_ring;
        uint32_t need_cnt;
        uint32_t in_byte_cnt = byte_cnt;
        uint32_t ou_byte_cnt;
        {   /* Update sample count */
            int32_t frm_smpl_cnt = (int32_t)audio_service_get_frame_sample_count();
            int32_t adjust_smpl_cnt = 0;
            aws_clock_skew_status_t status = audio_common.clock_skew_status;
            if (status == AWS_CLOCK_SKEW_STATUS_BUSY) {    /* Adjust sample count */
                log_hal_info("[AWS] clock skew working, remained sample count: %d\r\n", audio_common.remained_sample_count);
                int32_t remained_smpl_cnt = audio_common.remained_sample_count;
                adjust_smpl_cnt = BOUNDED(remained_smpl_cnt, AWS_ADD_POINT_BUF_SIZE, -AWS_ADD_POINT_BUF_SIZE); /* Speed control */
                remained_smpl_cnt -= adjust_smpl_cnt;
                frm_smpl_cnt += adjust_smpl_cnt;
                audio_common.remained_sample_count  = remained_smpl_cnt;
                if (remained_smpl_cnt == 0) {
                    status = AWS_CLOCK_SKEW_STATUS_IDLE;
                }
            }
            audio_dsp_update_audio_counter(-adjust_smpl_cnt);
            comp_cnt = adjust_smpl_cnt * 2 * sizeof(int16_t);
            ou_byte_cnt = frm_smpl_cnt * 2 * sizeof(int16_t);
            audio_common.clock_skew_status = status;
            audio_common.accumulated_sample_count += (uint32_t)frm_smpl_cnt;
        }
        if (in_byte_cnt != ou_byte_cnt) {   /* Frame buffer --> SRC buffer */
            p_src_ring = (uint8_t *)p_src_buf;
            need_cnt = ou_byte_cnt;
            audio_service_aws_src_process ((int16_t *)p_frm_buf, in_byte_cnt, (int16_t *)p_src_buf, ou_byte_cnt);
        } else {
            p_src_ring = (uint8_t *)p_frm_buf;
            need_cnt = in_byte_cnt;
        }
        {   /* Frame / SRC buffer --> Ring buffer */
            uint32_t spce_cnt = ring_buffer_get_space_byte_count(p_ring);
            if (spce_cnt >= need_cnt) {
                int32_t loop_idx;
                uint8_t *p_src = p_src_ring;
                uint32_t src_cnt = need_cnt;
                for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                    uint8_t *p_dst = NULL;
                    uint32_t dst_cnt = 0;
                    uint32_t write_cnt;
                    ring_buffer_get_write_information(p_ring, &p_dst, &dst_cnt);
                    write_cnt = MINIMUM(dst_cnt, src_cnt);
                    memcpy(p_dst, p_src, write_cnt);
                    p_src += write_cnt;
                    src_cnt -= write_cnt;
                    ring_buffer_write_done(p_ring, write_cnt);
                }
            } else {
                log_hal_info("[AWS] RB OF, space byte %d < need byte %d\r\n", (int)spce_cnt, (int)need_cnt);
            }
        }
    }
    {   /* Ring buffer --> Frame buffer */
        uint32_t need_cnt = byte_cnt;
        uint32_t threshold_cnt = (uint32_t)((int32_t)need_cnt + comp_cnt);
        uint32_t data_cnt = ring_buffer_get_data_byte_count(p_ring);
        if (data_cnt >= threshold_cnt) {
            int32_t loop_idx;
            uint8_t *p_dst = (uint8_t *)p_frm_buf;
            uint32_t dst_cnt = comp_cnt < 0 ? threshold_cnt : need_cnt;
            memset(p_frm_buf, 0, need_cnt);
            for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                uint8_t *p_src = NULL;
                uint32_t src_cnt = 0;
                uint32_t read_cnt;
                ring_buffer_get_read_information(p_ring, &p_src, &src_cnt);
                read_cnt = MINIMUM(dst_cnt, src_cnt);
                memcpy(p_dst, p_src, read_cnt);
                p_dst += read_cnt;
                dst_cnt -= read_cnt;
                ring_buffer_read_done(p_ring, read_cnt);
            }
            if (comp_cnt != 0) { /* update SHERIF */
                bool pin_buf_flag = (bool)(*DSP_AUD_CLK_PIN_PON_IDX);  /* 0: clock on pin, router on pon, 1: clock on pon, router on pin */
                if (comp_cnt > 0) { /* add point buffer */
                    volatile uint16_t *p_dsp = pin_buf_flag ? DSP_ADD_POINT_PIN : DSP_ADD_POINT_PON;  /* need to judge pin or pon "add point buffer" */
                    uint32_t dst_cnt = comp_cnt;
                    for (loop_idx = 0; loop_idx < 2; loop_idx++) {
                        uint8_t *p_src = NULL;
                        uint32_t src_cnt = 0;
                        uint32_t read_cnt;
                        ring_buffer_get_read_information(p_ring, &p_src, &src_cnt);
                        read_cnt = MINIMUM(dst_cnt, src_cnt);
                        audio_idma_write_to_dsp(p_dsp, (uint16_t *)p_src, read_cnt >> 1);
                        p_dsp += read_cnt >> 1;
                        dst_cnt -= read_cnt;
                        ring_buffer_read_done(p_ring, read_cnt);
                    }
                }
                if (pin_buf_flag) { /* save comp_cnt to dsp*/
                    *DSP_AUD_CLK_SKEW_PIN_VAL = (int16_t)(comp_cnt >> 2);  /* 1 for stereo, 1 for sizeof(int16_t) */
                } else {
                    *DSP_AUD_CLK_SKEW_PON_VAL = (int16_t)(comp_cnt >> 2);  /* 1 for stereo, 1 for sizeof(int16_t) */
                }
            }
        } else {
            log_hal_info("[AWS] RB UF, data byte %d < need byte %d\r\n", (int)data_cnt, (int)need_cnt);
            memset(p_frm_buf, 0, need_cnt);
        }
    }
    {   /* Frame buffer --> DSP buffer */
        audio_idma_write_to_dsp(p_dsp_buf, p_frm_buf, word_cnt);
    }
    {   /* Callback functions */
            audio_common.aws_callback(CODEC_AWS_CHECK_CLOCK_SKEW, audio_common.aws_user_data);
            audio_common.aws_callback(CODEC_AWS_CHECK_UNDERFLOW, audio_common.aws_user_data);
    }

    return;
}

int32_t audio_service_aws_get_buffer_size(aws_codec_type_t codec_type)
{
    int32_t aws_total_buf_size = 0;
    /*Get frame length depend on codec type*/
    uint16_t max_frm_len = audio_service_get_frame_length(codec_type);
    if (max_frm_len == 0) {
        log_hal_info("[AWS] Fail: Codec type error.\r\n");
        return HAL_AUDIO_AWS_ERROR;
    }

    /* Sum up to total aws buffer size */
    aws_total_buf_size += max_frm_len * sizeof(uint16_t); /* pcm_route_frame_size */
    aws_total_buf_size += max_frm_len * sizeof(uint16_t) + AWS_CLOCK_SKEW_MAX_SAMPLE_COUNT * 2 * sizeof(uint16_t); /* pcm_route_src_size */
    aws_total_buf_size += AWS_CLOCK_SKEW_RING_BUFFER_SAMPLE_COUNT * 2 * sizeof(uint16_t); /* pcm_route_ring_buffer */

    return aws_total_buf_size;
}

int32_t audio_service_aws_init(uint8_t *buffer, aws_codec_type_t codec_type, aws_callback_t callback, void *user_data)
{
    /* Error handling to make sure buffer allocate is successful.*/
    if (buffer != NULL) {
        log_hal_info("[AWS] audio_service_aws_init \r\n");
    } else {
        log_hal_info("[AWS] Fail: AWS buffer is NULL.\r\n");
        return HAL_AUDIO_AWS_ERROR;
    }
    /* Caculate each buffer size and store them */
    {
        uint16_t max_frm_len = audio_service_get_frame_length(codec_type);
        uint32_t frm_buf_size = max_frm_len * sizeof(uint16_t);
        uint32_t src_buf_size = frm_buf_size + AWS_CLOCK_SKEW_MAX_SAMPLE_COUNT * 2 * sizeof(uint16_t);
        uint32_t ring_buf_size = AWS_CLOCK_SKEW_RING_BUFFER_SAMPLE_COUNT * 2 * sizeof(uint16_t);
        ring_buffer_information_t *p_ring = &audio_common.pcm_route_ring_buffer;
        if (max_frm_len == 0) {
            log_hal_info("[AWS] Fail: Codec type error.\r\n");
            return HAL_AUDIO_AWS_ERROR;
        }
        audio_common.pcm_route_frame_size = frm_buf_size;
        audio_common.pcm_route_src_size = src_buf_size;
        audio_common.pcm_route_ring_buffer.buffer_byte_count = ring_buf_size;

        /* AWS parameter initialization */
        audio_common.aws_flag = false;
        audio_common.aws_init_flag = true;
        audio_common.clock_skew_status = AWS_CLOCK_SKEW_STATUS_IDLE;
        audio_common.accumulated_sample_count = 0;
        audio_common.aws_pcm_route_counter = 0;
        audio_common.aws_skip_clk_skew_flag = false;

        /* Assign clock skew ring buffer, src buffer, and frame buffer*/
        p_ring->buffer_base_pointer = buffer;
        buffer += ring_buf_size;
        audio_common.pcm_route_src_buffer = (uint16_t *)buffer;
        buffer += src_buf_size;
        audio_common.pcm_route_frame_buffer = (uint16_t *)buffer;

        /* Reset ring buffer */
        p_ring->write_pointer = 0;
        p_ring->read_pointer = 0;
        memset(p_ring->buffer_base_pointer, 0, ring_buf_size);

        /* Hook callback function */
        audio_common.aws_callback = callback;
        audio_common.aws_user_data = user_data;

        return HAL_AUDIO_AWS_NORMAL;
    }
}

int32_t audio_service_aws_deinit(void)
{
    /* Set PCM route */
    log_hal_info("[AWS] AWS deinit\r\n");
    audio_common.aws_flag = false;
    audio_common.aws_init_flag = false;

    return 0;
}

uint32_t audio_service_aws_get_accumulated_sample_count(void)
{
    return audio_common.accumulated_sample_count;
}

int32_t audio_service_aws_get_silence_frame_information(aws_codec_type_t codec_type, uint32_t *sample_count, uint32_t *byte_count)
{
    if (codec_type == AWS_CODEC_TYPE_SBC_FORMAT) {
        *sample_count = audio_common.sbc_frame_sample;
        *byte_count = (SBC_SILENCE_FIX_PART_LENGTH + SBC_SILENCE_MODIFY_LENGTH);
    } else if (codec_type == AWS_CODEC_TYPE_AAC_FORMAT) {
#ifdef MTK_BT_A2DP_AAC_ENABLE
        *sample_count = 1024;
        *byte_count = SILENCE_TOTAL_LENGTH;
#else   /* MTK_BT_A2DP_AAC_ENABLE */
        return HAL_AUDIO_AWS_NOT_SUPPORT;
#endif /* MTK_BT_A2DP_AAC_ENABLE*/
    } else if (codec_type == AWS_CODEC_TYPE_PCM_FORMAT) {
        *sample_count = pcm_sd_playback_frame_length_table[0] >> 1;
        *byte_count = *sample_count * sizeof(uint16_t);
    } else if (codec_type == AWS_CODEC_TYPE_MP3_FORMAT) {
        *sample_count = 0;
        *byte_count = 0;
        return HAL_AUDIO_AWS_ERROR;
    } else {
        *sample_count = 0;
        *byte_count = 0;
        return HAL_AUDIO_AWS_ERROR;
    }
    return 0;
}

static const uint8_t sbc_silence_pattern[SBC_SILENCE_FIX_PART_LENGTH] = {0x9C, 0x33, 0x00, 0x77}; //last byte is depend on sub_band * block_len

int32_t audio_service_aws_fill_silence_frame(uint8_t *buffer, uint32_t *byte_count, aws_codec_type_t codec_type, uint32_t *target_frm_cnt)
{
    if (codec_type == AWS_CODEC_TYPE_SBC_FORMAT) {
        uint32_t available_frm_cnt = (*byte_count - SBC_SILENCE_FRAME_NUM_CNT) / (SBC_SILENCE_FIX_PART_LENGTH + SBC_SILENCE_MODIFY_LENGTH); // -1 is for frames number of payload
        available_frm_cnt = available_frm_cnt > SBC_MAX_FRAME_PER_PAYLOAD ? SBC_MAX_FRAME_PER_PAYLOAD : available_frm_cnt; //Max frame in a payload
        if (available_frm_cnt < 1) {
            return HAL_AUDIO_AWS_ERROR;
        } else {
            if (available_frm_cnt < *target_frm_cnt) { //available_frm_cnt not enough but greater than one
                *target_frm_cnt = available_frm_cnt;
            }
            uint32_t frm_idx;
            uint8_t *p_src = (uint8_t *)sbc_silence_pattern;
            uint8_t *p_frm_smpl = &audio_common.sbc_frame_sample;
            uint8_t *p_dst = buffer;
            uint8_t sil_frm_cnt = (uint8_t)*target_frm_cnt;
            *p_dst++ = sil_frm_cnt;
            for (frm_idx = 0; frm_idx < *target_frm_cnt; frm_idx++) {
                memcpy(p_dst, p_src, SBC_SILENCE_FIX_PART_LENGTH);
                p_dst += SBC_SILENCE_FIX_PART_LENGTH;
                memcpy(p_dst, p_frm_smpl, SBC_SILENCE_MODIFY_LENGTH);
                p_dst += SBC_SILENCE_MODIFY_LENGTH;
            }
            *byte_count -= ((*target_frm_cnt * (SBC_SILENCE_FIX_PART_LENGTH + SBC_SILENCE_MODIFY_LENGTH)) + SBC_SILENCE_FRAME_NUM_CNT);
        }
    } else if (codec_type == AWS_CODEC_TYPE_AAC_FORMAT) {
#ifdef MTK_BT_A2DP_AAC_ENABLE
        uint32_t available_frm_cnt = *byte_count / SILENCE_TOTAL_LENGTH;
        if (available_frm_cnt < 1) {
            return HAL_AUDIO_AWS_ERROR;
        } else {
            if (available_frm_cnt < *target_frm_cnt) { //available_frm_cnt not enough but greater than one
                *target_frm_cnt = available_frm_cnt;
            }
            uint32_t frm_idx;
            uint8_t *p_src = audio_common.aac_silence_pattern;
            uint8_t *p_dst = buffer;
            for (frm_idx = 0; frm_idx < *target_frm_cnt; frm_idx++) {
                memset(p_dst, 0, SILENCE_TOTAL_LENGTH);
                memcpy(p_dst, p_src, SILENCE_TOTAL_LENGTH);
                p_dst += SILENCE_TOTAL_LENGTH;
            }
            *byte_count -= (*target_frm_cnt * SILENCE_TOTAL_LENGTH);
        }
#else   /* MTK_BT_A2DP_AAC_ENABLE */
        return HAL_AUDIO_AWS_ERROR;
#endif  /* MTK_BT_A2DP_AAC_ENABLE */
    } else if (codec_type == AWS_CODEC_TYPE_MP3_FORMAT) {
        return HAL_AUDIO_AWS_ERROR;
    } else if (codec_type == AWS_CODEC_TYPE_PCM_FORMAT) {
        uint32_t frm_sample_count = audio_service_get_frame_sample_count(); /* One pair of L / R counts as one sample. */
        uint32_t available_frm_cnt = *byte_count / (frm_sample_count * sizeof(uint16_t));
        if (available_frm_cnt < 1) {
            return HAL_AUDIO_AWS_ERROR;
        } else {
            if (available_frm_cnt < *target_frm_cnt) { //available_frm_cnt not enough but greater than one
                *target_frm_cnt = available_frm_cnt;
            }
            uint32_t frm_idx;
            uint8_t *p_dst = buffer;
            for (frm_idx = 0; frm_idx < *target_frm_cnt; frm_idx++) {
                memset(p_dst, 0, (frm_sample_count * sizeof(uint16_t)));
                p_dst += (frm_sample_count * sizeof(uint16_t));
            }
            *byte_count -= (*target_frm_cnt * frm_sample_count * sizeof(uint16_t));
        }
    } else {
        return HAL_AUDIO_AWS_ERROR;
    }
    return 0;
}

int32_t audio_service_aws_parse_bitstream_information(uint8_t *buffer, uint32_t buf_byte_count, aws_codec_type_t codec_type, uint32_t *sample_count, uint32_t *pcm_byte_count)
{
    if (codec_type == AWS_CODEC_TYPE_SBC_FORMAT) {
        uint32_t decode_sample_count = 0;
        int32_t parsed = audio_service_a2dp_aws_sbc_get_media_payload_sample_count(buffer, &decode_sample_count, &buf_byte_count);
        if (parsed >= 0) {
            *sample_count = decode_sample_count;
            *pcm_byte_count = buf_byte_count;
        } else {
            *sample_count = 0;
            *pcm_byte_count = 0;
            return HAL_AUDIO_AWS_ERROR;
        }
    } else if (codec_type == AWS_CODEC_TYPE_AAC_FORMAT) {
#ifdef MTK_BT_A2DP_AAC_ENABLE
        *sample_count = 1024;
        *pcm_byte_count = 1024 << 2;
#else /*MTK_BT_A2DP_AAC_ENABLE*/
        *sample_count = 0;
        *pcm_byte_count = 0;
            return HAL_AUDIO_AWS_ERROR;
#endif /*MTK_BT_A2DP_AAC_ENABLE*/
    } else if (codec_type == AWS_CODEC_TYPE_MP3_FORMAT) {
        return HAL_AUDIO_AWS_ERROR;
    } else if (codec_type == AWS_CODEC_TYPE_PCM_FORMAT) {
        *sample_count = buf_byte_count >> 2;// one sample has (two byte * two channel)
        *pcm_byte_count = buf_byte_count;
    } else {
        *sample_count = 0;
        *pcm_byte_count = 0;
        return HAL_AUDIO_AWS_ERROR;
    }
    return 0;
}

aws_clock_skew_status_t audio_service_aws_get_clock_skew_status(void)
{
    return audio_common.clock_skew_status;
}

int32_t audio_service_aws_set_clock_skew_compensation_value(int32_t sample_count)
{
    if (audio_common.clock_skew_status == AWS_CLOCK_SKEW_STATUS_IDLE) {
        if (sample_count != 0) {
            audio_common.remained_sample_count = sample_count;
            audio_common.clock_skew_status = AWS_CLOCK_SKEW_STATUS_BUSY;
            /* Compensate clock skew */
        }
    } else {
        return HAL_AUDIO_AWS_ERROR;
    }
    return 0;
}

void audio_service_aws_skip_clock_skew(bool skip_flag) {
    audio_common.aws_skip_clk_skew_flag = skip_flag;
    log_hal_info("[AWS] aws_skip_clk_skew_flag :%d\r\n", audio_common.aws_skip_clk_skew_flag);
    return;
}
#endif  /* defined(__HAL_AUDIO_AWS_SUPPORT__) */

void sbc_set_silence_pattern_frame_sample(uint8_t block_len, uint8_t sub_bands)
{
    uint8_t block_len_val, sub_bands_val;
    switch (block_len) {
        case 0x1:
            block_len_val = 16;
            break;
        case 0x2:
            block_len_val = 12;
            break;
        case 0x4:
            block_len_val = 8;
            break;
        case 0x8:
            block_len_val = 4;
            break;
        default:
            log_hal_info("[AWS] SBC block length error :%d\r\n", block_len);
            block_len_val = 16;
            break;
    }
    switch (sub_bands) {
        case 0x1:
            sub_bands_val = 8;
            break;
        case 0x2:
            sub_bands_val = 4;
            break;
        default:
            log_hal_info("[AWS] SBC sub-band number error :%d\r\n", sub_bands);
            sub_bands_val = 8;
            break;
    }
    audio_common.sbc_frame_sample = block_len_val * sub_bands_val;
    return;
}

/*
    return:
        1 << n --> PCM route ID, where 0 <= n < MAX_PCM_ROUTE_FUNCTIONS
        -1     --> No available PCM route ID
        -2     --> PCM route registration full
        others --> Error
*/
int32_t audio_pcm_route_register_isr(isr function, void *user_data, uint32_t order)
{
    if (order >= MAX_PCM_ROUTE_FUNCTIONS) {
        return -1;   /* No available PCM route ID */
    }
    if ( audio_pcm_route_isr.pcm_route_id & (1<<order) ) {
        return -3;   /* PCM route double registration */
    }
    
    if (audio_pcm_route_isr.num[order] == 0) {
        audio_pcm_route_isr.pcm_route_id |= (1<<order);
        
        audio_pcm_route_isr.num[order] = (1<<order);
        audio_pcm_route_isr.function[order] = function;
        audio_pcm_route_isr.userdata[order] = user_data;
        
        return (1<<order);
    } else {
        return -3;  /* PCM route double registration */
    }
}

/*
    return:
        0      --> Normal
        -1     --> Invalid PCM route ID
        -2     --> Cannot find matching PCM route registration
        others --> Error
*/
int32_t audio_pcm_route_deregister_isr(int32_t pcm_route_id)
{
    int32_t result;
    if (pcm_route_id != 0) {
        int32_t func_id;
        for (func_id = 0; func_id < MAX_PCM_ROUTE_FUNCTIONS; func_id++) {
            if (audio_pcm_route_isr.num[func_id] == pcm_route_id) {
                audio_pcm_route_isr.num[func_id] = 0;
                audio_pcm_route_isr.function[func_id] = NULL;
                audio_pcm_route_isr.userdata[func_id] = NULL;
                break;
            }
        }
        if (func_id < MAX_PCM_ROUTE_FUNCTIONS) {
            audio_pcm_route_isr.pcm_route_id &= ~pcm_route_id;
            result = 0;     /* Normal */
        } else {
            result = -2;    /* Cannot find matching PCM route registration */
        }
    } else {
        result = -1;        /* Invalid PCM route ID */
    }
    return result;
}

bool audio_is_dsp_codec(void)
{
    return audio_common.is_dsp_codec;
}

void audio_get_current_stream_config(hal_audio_stream_t **config)
{
    *config = &audio_config;
}

#endif /* defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED) */
