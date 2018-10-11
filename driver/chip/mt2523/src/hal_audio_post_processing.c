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
#include "hal_audio_post_processing.h"
#include "hal_audio_post_processing_internal.h"
#include "hal_audio_internal_service.h"
#include "hal_audio_fw_sherif.h"
#include "hal_audio_fw_interface.h"
#include "hal_gpt.h"
#include <math.h>
#include <string.h>

#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
#if defined(MTK_AUDIO_IIR_FILTER_UNIT_TEST)
const acf_design_t acf_default_design = {
    4,   200,   /* HPF order, fc */
    1, 10000,   /* LPF order, fc */
    {   /* BPF fc, bw, gain */
        { 500,  200,  6},
        { 800,  200, -6},
        {1000,  400,  6},
        {1600,  400, -6},
        {2000,  800,  6},
        {3200,  800, -6},
        {4000, 1600,  6},
        {6400, 1600, -6}
    }
};
static acf_parameter_format_t acf_default_format;
#endif  /* defined(MTK_AUDIO_IIR_FILTER_UNIT_TEST) */

static acf_information_t acf_handle;

static int32_t is_valid_sampling_rate(uint32_t sampling_rate)
{
    int32_t result;
    switch (sampling_rate) {
        case ACF_SAMPLING_RATE_48000HZ:
        case ACF_SAMPLING_RATE_44100HZ:
        case ACF_SAMPLING_RATE_32000HZ:
        case ACF_SAMPLING_RATE_24000HZ:
        case ACF_SAMPLING_RATE_22050HZ:
        case ACF_SAMPLING_RATE_16000HZ:
        case ACF_SAMPLING_RATE_12000HZ:
        case ACF_SAMPLING_RATE_11025HZ:
        case ACF_SAMPLING_RATE_08000HZ:
            result = ACF_TRUE;
            break;
        default:
            result = ACF_FALSE;
            break;
    }
    return result;
}

static int32_t is_valid_2nd_order_iir_filter_coefficient(uint16_t *p_coef)
{
    int32_t result;
    {   /* Check if all-zero coefficients */
        uint16_t acc = 0;
        uint32_t coef_idx;
        for (coef_idx = 0; coef_idx < ACF_ONE_COEF_LEN; coef_idx++) {
            acc |= p_coef[coef_idx];
        }
        if (acc != 0) {
            result = ACF_TRUE;
        } else {
            result = ACF_FALSE;
        }
    }
    return result;
}

static int32_t is_valid_coefficient_table(void)
{
    int32_t result;
    {   /* Check if all-zero coefficients */
        uint16_t dsp_ctrl = acf_handle.dsp_ctrl;
        if (dsp_ctrl & AUDIO_IIR_ENABLE_MASK) {
            result = ACF_TRUE;
        } else {
            result = ACF_FALSE;
        }
    }
    return result;
}

static int32_t is_valild_fs(uint32_t fs)
{
    int32_t result;
    if (fs ==  8000
            || fs == 11025
            || fs == 12000
            || fs == 16000
            || fs == 22050
            || fs == 24000
            || fs == 32000
            || fs == 44100
            || fs == 48000) {
        result = ACF_TRUE;
    } else {
        result = ACF_FALSE;
    }
    return result;
}

static int32_t is_valid_hpf(uint32_t fs, uint32_t fc, uint32_t order)
{
    int32_t result;
    if (fc >= HPF_FREQ_MIN
            && fc <= HPF_FREQ_MAX
            && (order == 4 || order == 2)
            && is_valild_fs(fs)
            && (fc < fs / 2)) {
        result = ACF_TRUE;
    } else {
        result = ACF_FALSE;
    }
    return result;
}

static int32_t is_valid_lpf(uint32_t fs, uint32_t fc, uint32_t order)
{
    int32_t result;
    if (fc >= LPF_FREQ_MIN
            && fc <= LPF_FREQ_MAX
            && (order == 2 || order == 1)
            && is_valild_fs(fs)
            && (fc < fs / 2)) {
        result = ACF_TRUE;
    } else {
        result = ACF_FALSE;
    }
    return result;
}

static int32_t is_valid_bpf(uint32_t fs, uint32_t fc, uint32_t bw, float gain)
{
    int32_t result;
    if (fc >= BPF_FC_MIN
            && fc <= BPF_FC_MAX
            && bw >= BPF_BW_MIN
            && bw <= BPF_BW_MAX
            && gain >= BPF_GAIN_MIN
            && gain <= BPF_GAIN_MAX
            && is_valild_fs(fs)
            && (fc < fs / 2)) {
        result = ACF_TRUE;
    } else {
        result = ACF_FALSE;
    }
    return result;
}

static uint32_t convert_double_to_uint32(double in, double gain)
{
    uint32_t out;
    if (in > 0) {
        out = (uint32_t)(in * gain);
    } else {
        out = (uint32_t)(ACF_U32_VALUE + in * gain);
    }
    return out;
}

static void fill_dsp_coefficients_from_double_to_q5p27(double b[3], double a[3], uint16_t *p_coef)
{
    if (a[0] != 0) {
        uint32_t k[6];
        b[2] = b[2] / a[0];
        b[1] = b[1] / a[0];
        b[0] = b[0] / a[0];
        a[2] = a[2] / a[0];
        a[1] = a[1] / a[0];
        a[0] = 1;
        k[0] = convert_double_to_uint32(b[0], ACF_TICK1);
        k[1] = convert_double_to_uint32(b[1], ACF_TICK1);
        k[2] = convert_double_to_uint32(b[2], ACF_TICK1);
        k[3] = convert_double_to_uint32(-a[1], ACF_TICK1);
        k[4] = convert_double_to_uint32(-a[2], ACF_TICK1);
        k[5] = 1;
        {   /* Upper 16-bit / Lower 16-bit */
            int32_t idx;
            for (idx = 0; idx < 6; idx++) {
                uint16_t k_upper = (uint16_t)(k[idx] >> 16);
                uint16_t k_lower = (uint16_t)(k[idx] & 0xFFFF);
                *p_coef++ = k_upper;
                *p_coef++ = k_lower;
            }
        }
    } else {
        memset(p_coef, 0, sizeof(uint16_t) * ACF_ONE_COEF_LEN);
    }
    return;
}

static void generate_lpf(uint32_t fs, uint32_t fc, uint32_t order, uint16_t *p_coef)
{
    if (order == 2) {
        double wc, alpha, b[3], a[3];
        /*
            wc = pi * fc / fs;
            alpha = wc / tan(wc);
            b0 = wc^2;
            b1 = 2 * wc^2;
            b2 = wc^2;
            a0 = alpha^2 + wc^2 + sqrt(2) * wc * alpha;
            a1 = 2 * (wc^2 - alpha^2);
            a2 = alpha^2 + wc^2 - sqrt(2) * wc * alpha;
            B0 = [b0 / a0, b1 / a0, b2 / a0];
            A0 = [a0 / a0, a1 / a0, a2 / a0];
        */
        wc = ACF_PI * fc / fs;
        alpha = wc / tan(wc);
        b[0] = pow(wc, 2);
        b[1] = (double)2.0 * pow(wc, 2);
        b[2] = pow(wc, 2);
        a[0] = pow(alpha, 2) + pow(wc, 2) + sqrt(2) * wc * alpha;
        a[1] = 2 * (pow(wc, 2) - pow(alpha, 2));
        a[2] = pow(alpha, 2) + pow(wc, 2) - sqrt(2) * wc * alpha;
        fill_dsp_coefficients_from_double_to_q5p27(b, a, p_coef);
    } else if (order == 1) {
        double wc, alpha, b[3], a[3];
        /*
            wc = pi * fc / fs;
            alpha = wc / tan(wc);
            b0 = wc;
            b1 = wc;
            b2 = 0;
            a0 = alpha + wc;
            a1 = wc - alpha;
            a2 = 0;
            B0 = [b0 / a0, b1 / a0, b2 / a0];
            A0 = [a0 / a0, a1 / a0, a2 / a0];
        */
        wc = ACF_PI * fc / fs;
        alpha = wc / tan(wc);
        b[0] = wc;
        b[1] = wc;
        b[2] = 0;
        a[0] = alpha + wc;
        a[1] = wc - alpha;
        a[2] = 0;
        fill_dsp_coefficients_from_double_to_q5p27(b, a, p_coef);
    } else {
        memset(p_coef, 0, sizeof(uint16_t) * ACF_ONE_COEF_LEN);
    }
    return;
}

static void generate_hpf(uint32_t fs, uint32_t fc, uint32_t order, uint16_t *p_coef)
{
    if (order == 4) {
        double wc, alpha, b[3], a[3];
        /*
            wc = pi * fc / fs;
            alpha = wc / tan(wc);
            b0 = 1;
            b1 = -2;
            b2 = 1;
            a00 = 1 + wc^2 + 2 * wc * cos(    pi / 8) / alpha;
            a10 = 2 * (wc^2 - 1);
            a20 = 1 + wc^2 - 2 * wc * cos(    pi / 8) / alpha;
            a01 = 1 + wc^2 + 2 * wc * cos(3 * pi / 8) / alpha;
            a11 = 2 * (wc^2 - 1);
            a21 = 1 + wc^2 - 2 * wc * cos(3 * pi / 8) / alpha;
            B0 = [ b0 / a00,  b1 / a00,  b2 / a00];
            A0 = [a00 / a00, a10 / a00, a20 / a00];
            B1 = [ b0 / a01,  b1 / a01,  b2 / a01];
            A1 = [a01 / a01, a11 / a01, a21 / a01];
        */
        wc = ACF_PI * fc / fs;
        alpha = wc / tan(wc);
        b[0] = 1;
        b[1] = -2;
        b[2] = 1;
        a[0] = (double)1.0 + wc * wc + COS_1PI_8TH_X2 * wc / alpha;
        a[1] = (double)2.0 * (wc * wc - 1);
        a[2] = (double)1.0 + wc * wc - COS_1PI_8TH_X2 * wc / alpha;
        fill_dsp_coefficients_from_double_to_q5p27(b, a, p_coef);
        p_coef += ACF_ONE_COEF_LEN;
        b[0] = 1;
        b[1] = -2;
        b[2] = 1;
        a[0] = (double)1.0 + wc * wc + COS_3PI_8TH_X2 * wc / alpha;
        a[1] = (double)2.0 * (wc * wc - 1);
        a[2] = (double)1.0 + wc * wc - COS_3PI_8TH_X2 * wc / alpha;
        fill_dsp_coefficients_from_double_to_q5p27(b, a, p_coef);
    } else if (order == 2) {
        double wc, alpha, b[3], a[3];
        /*
            wc = pi * fc / fs;
            alpha = wc / tan(wc);
            b0 = alpha^2;
            b1 = -2 * alpha^2;
            b2 = alpha^2;
            a0 = alpha^2 + wc^2 + sqrt(2) * wc * alpha;
            a1 = 2 * (wc ^ 2 - alpha ^ 2);
            a2 = alpha^2 + wc^2 - sqrt(2) * wc * alpha;
            B = [b0 / a0, b1 / a0, b2 / a0];
            A = [a0 / a0, a1 / a0, a2 / a0];
        */
        wc = ACF_PI * fc / fs;
        alpha = wc / tan(wc);
        b[0] = pow(alpha, 2);
        b[1] = (double) - 2.0 * pow(alpha, 2);
        b[2] = pow(alpha, 2);
        a[0] = pow(alpha, 2) + pow(wc, 2) + sqrt(2) * wc * alpha;
        a[1] = 2 * (pow(wc, 2) - pow(alpha, 2));
        a[2] = pow(alpha, 2) + pow(wc, 2) - sqrt(2) * wc * alpha;
        fill_dsp_coefficients_from_double_to_q5p27(b, a, p_coef);
        p_coef += ACF_ONE_COEF_LEN;
        memset(p_coef, 0, sizeof(uint16_t) * ACF_ONE_COEF_LEN);
    } else {
        memset(p_coef, 0, sizeof(uint16_t) * ACF_HPF_COEF_LEN);
    }
    return;
}

static void generate_bpf(uint32_t fs, uint32_t fc, uint32_t bw, float gain, uint16_t *p_coef)
{
    double g, alpha, beta, H, b[3], a[3], g_for_alpha;
    g = pow(10, gain / 20);
    g_for_alpha = LOWER_BOUND(g, 1);
    alpha = (tan(ACF_PI * bw / fs) - g_for_alpha) / (tan(ACF_PI * bw / fs) + g_for_alpha);
    beta = -cos(2 * ACF_PI * fc / fs);
    H = g - 1;
    b[0] = 1 + (1 + alpha) * H / 2;
    b[1] = beta * (1 - alpha);
    b[2] = -alpha - (1 + alpha) * H / 2;
    a[0] = 1;
    a[1] = b[1];
    a[2] = -alpha;
    fill_dsp_coefficients_from_double_to_q5p27(b, a, p_coef);
    return;
}

static int32_t audio_check_iir_filter(uint32_t sampling_rate)
{
    int32_t result;
    if (acf_handle.update_flag == ACF_FALSE) {
        result = ACF_NO_UPDATE_FLAG;
    } else if (is_valid_sampling_rate(sampling_rate) == ACF_FALSE) {
        result = ACF_INVALID_SAMPLING_RATE;
    } else if (is_valid_coefficient_table() == ACF_FALSE) {
        result = ACF_INVALID_COEFFICIENT_TABLE;
    } else {
        result = ACF_EVERYTHING_IS_VALID;
    }
    return result;
}

void audio_iir_generate_coefficient_table(uint32_t fs)
{
    {   /* Generate IIR filter coefficients */
        uint16_t *p_coef = acf_handle.coef;
        uint32_t update_flag = acf_handle.update_flag;
        int32_t is_valid_fs = is_valid_sampling_rate(fs);
        if (update_flag == ACF_FALSE || is_valid_fs == ACF_FALSE) {
            memset(p_coef, 0, sizeof(uint16_t) * ACF_ALL_COEF_LEN);
        } else {
            acf_design_t *p_design = &acf_handle.design;
            {   /* Generate HPF coefficients */
                uint32_t fc    = p_design->hpf_fc;
                uint32_t order = p_design->hpf_order;
                if (is_valid_hpf(fs, fc, order)) {
                    generate_hpf(fs, fc, order, p_coef);
                } else {
                    memset(p_coef, 0, sizeof(uint16_t) * ACF_HPF_COEF_LEN);
                }
                p_coef += ACF_HPF_COEF_LEN;
            }
            {   /* Generate LPF coefficients */
                uint32_t fc    = p_design->lpf_fc;
                uint32_t order = p_design->lpf_order;
                if (is_valid_lpf(fs, fc, order)) {
                    generate_lpf(fs, fc, order, p_coef);
                } else {
                    memset(p_coef, 0, sizeof(uint16_t) * ACF_ONE_COEF_LEN);
                }
                p_coef += ACF_ONE_COEF_LEN;
            }
            {   /* Generate BPF coefficients */
                acf_bpf_design_t *p_bpf_design = p_design->bpf;
                uint32_t bpf_idx;
                for (bpf_idx = 0; bpf_idx < BPF_NUM; bpf_idx++) {
                    uint32_t fc   = p_bpf_design->fc;
                    uint32_t bw   = p_bpf_design->bw;
                    float    gain = p_bpf_design->gain;
                    if (is_valid_bpf(fs, fc, bw, gain)) {
                        generate_bpf(fs, fc, bw, gain, p_coef);
                    } else {
                        memset(p_coef, 0, sizeof(uint16_t) * ACF_ONE_COEF_LEN);
                    }
                    p_bpf_design++;
                    p_coef += ACF_ONE_COEF_LEN;
                }
            }
        }
    }
    {   /* Check all-zero coefficients */
        int32_t iir_idx;
        uint16_t *p_coef  = acf_handle.coef;
        uint16_t dsp_ctrl = acf_handle.dsp_ctrl;
        for (iir_idx = 0; iir_idx < IIR_NUM; iir_idx++) {
            int32_t is_valid_flt = is_valid_2nd_order_iir_filter_coefficient(p_coef);
            if (is_valid_flt) {
                dsp_ctrl |=   0x1 << iir_idx;
            } else {
                dsp_ctrl &= ~(0x1 << iir_idx);
            }
            p_coef += ACF_ONE_COEF_LEN;
        }
        acf_handle.dsp_ctrl = dsp_ctrl;
    }
    return;
}

void audio_write_iir_coefficients_to_dsp(void)
{
    uint16_t *p_src = acf_handle.coef;
    volatile uint16_t *p_dst = DSP_PM_ADDR(AUDIO_IIR_PM_TABLE_PAGE, AUDIO_IIR_PM_TABLE_ADDR);
    int32_t coef_idx;
    for (coef_idx = 0; coef_idx < ACF_ALL_COEF_LEN; coef_idx++) {
        *p_dst++ = *p_src++;
    }
    return;
}

void audio_post_processing_init(void)
{
    memset(&acf_handle, 0, sizeof(acf_information_t));
#if defined(MTK_AUDIO_IIR_FILTER_UNIT_TEST)
    {   /* Default design parameter setting */
        acf_design_t *p_design = &acf_default_design;
        acf_parameter_format_t *p_format = &acf_default_format;
        memset(p_format, 0, sizeof(acf_parameter_format_t));
        p_format->acf_par_hpf1_id_len = p_design->hpf_order << 12;
        p_format->acf_par_hpf1_fc     = p_design->hpf_fc << 16;
        p_format->acf_par_lpf1_id_len = p_design->lpf_order << 12;
        p_format->acf_par_lpf1_fc     = p_design->lpf_fc << 16;
        {   /* Set BPF format parameters */
            uint32_t bpf_idx;
            for (bpf_idx = 0; bpf_idx < BPF_NUM; bpf_idx++) {
                acf_bpf_design_t *p_bpf_design = &p_design->bpf[bpf_idx];
                acf_bpf_format_t *p_bpf_format = &p_format->acf_par_bpf[bpf_idx];
                uint32_t fc_bw = (p_bpf_design->fc << 16) | (p_bpf_design->bw);
                p_bpf_format->acf_par_bpf_fc_bw = fc_bw;
                memcpy(&p_bpf_format->acf_par_bpf_gain, &p_bpf_design->gain, sizeof(float));
            }
        }
        audio_update_iir_design((uint32_t *)p_format);
    }
#endif  /* defined(MTK_AUDIO_IIR_FILTER_UNIT_TEST) */
    return;
}

int32_t is_applying_audio_iir(uint32_t sr_val)
{
    /* Check whether to apply IIR filter or not */
    int32_t apply_iir;
    int32_t result = audio_check_iir_filter(sr_val);
    if (result == ACF_EVERYTHING_IS_VALID) {
        apply_iir = ACF_TRUE;
    } else {
        apply_iir = ACF_FALSE;
    }
    return apply_iir;
}

void audio_iir_start(void)
{
    uint16_t dsp_ctrl = acf_handle.dsp_ctrl;
    dsp_ctrl &= AUDIO_IIR_STATE_CLR;
    dsp_ctrl |= AUDIO_IIR_STATE_START;
    acf_handle.dsp_ctrl = dsp_ctrl;
    *DSP_AUD_IIR_CTRL = dsp_ctrl;
    *DSP_AUDIO_PP_CTRL |= 0x1;
    return;
}

void audio_iir_stop(void)
{
    {   /* Start --> Running */
        uint16_t dsp_ctrl = acf_handle.dsp_ctrl;
        if ((dsp_ctrl & AUDIO_IIR_STATE_MASK) == AUDIO_IIR_STATE_START) {
            int32_t loop_cnt = 0;
            while (loop_cnt < 80) {
                dsp_ctrl = *DSP_AUD_IIR_CTRL;
                if ((dsp_ctrl & AUDIO_IIR_STATE_MASK) == AUDIO_IIR_STATE_RUNNING) {
                    break;
                }
                loop_cnt++;
                hal_gpt_delay_ms(9);
            }
            if (loop_cnt >= 80) {
                dsp_ctrl &= AUDIO_IIR_STATE_CLR;
                dsp_ctrl |= AUDIO_IIR_STATE_STOP;
                acf_handle.dsp_ctrl = dsp_ctrl;
                *DSP_AUD_IIR_CTRL = dsp_ctrl;
            }
        }
    }
    {   /* Running --> Stop */
        uint16_t dsp_ctrl = acf_handle.dsp_ctrl;
        if ((dsp_ctrl & AUDIO_IIR_STATE_MASK) == AUDIO_IIR_STATE_RUNNING) {
            dsp_ctrl &= AUDIO_IIR_STATE_CLR;
            dsp_ctrl |= AUDIO_IIR_STATE_STOP;
            acf_handle.dsp_ctrl = dsp_ctrl;
            *DSP_AUD_IIR_CTRL = dsp_ctrl;
        }
    }
    {   /* Stop --> Idle */
        uint16_t dsp_ctrl = acf_handle.dsp_ctrl;
        if ((dsp_ctrl & AUDIO_IIR_STATE_MASK) == AUDIO_IIR_STATE_STOP) {
            int32_t loop_cnt = 0;
            while (loop_cnt < 80) {
                dsp_ctrl = *DSP_AUD_IIR_CTRL;
                if ((dsp_ctrl & AUDIO_IIR_STATE_MASK) == AUDIO_IIR_STATE_IDLE) {
                    break;
                }
                loop_cnt++;
                hal_gpt_delay_ms(9);
            }
            {   /* Clear to idle */
                dsp_ctrl &= AUDIO_IIR_STATE_CLR;
                dsp_ctrl |= AUDIO_IIR_STATE_IDLE;
                acf_handle.dsp_ctrl = dsp_ctrl;
                *DSP_AUD_IIR_CTRL = dsp_ctrl;
            }
        }
    }
    return;
}

void audio_iir_set_position (bool is_before_pcm_route)
{
    uint16_t dsp_ctrl = acf_handle.dsp_ctrl;
    dsp_ctrl &= AUDIO_IIR_POSITION_CLR;
    dsp_ctrl |= (is_before_pcm_route ? AUDIO_IIR_POSITION_BEFORE_PCM_ROUTER : AUDIO_IIR_POSITION_AFTER_PCM_ROUTER);
    acf_handle.dsp_ctrl = dsp_ctrl;
    return;
}
#endif /* defined(MTK_AUDIO_IIR_FILTER_ENABLE) */

int32_t audio_update_iir_design(const uint32_t *parameter)
{
#if defined(MTK_AUDIO_IIR_FILTER_ENABLE)
    acf_parameter_format_t *p_format = &acf_handle.format;
    memcpy(p_format, parameter, sizeof(acf_parameter_format_t));
    {   /* Extract audio IIR filter design parameters from raw parameters */
        acf_design_t *p_design = &acf_handle.design;
        p_design->hpf_order = (p_format->acf_par_hpf1_id_len >> 12) & 0xF;
        p_design->hpf_fc    = p_format->acf_par_hpf1_fc >> 16;
        p_design->lpf_order = (p_format->acf_par_lpf1_id_len >> 12) & 0xF;
        p_design->lpf_fc    = p_format->acf_par_lpf1_fc >> 16;
        {   /* Get BPF design parameters */
            uint32_t bpf_idx;
            for (bpf_idx = 0; bpf_idx < BPF_NUM; bpf_idx++) {
                acf_bpf_format_t *p_bpf_format = &p_format->acf_par_bpf[bpf_idx];
                acf_bpf_design_t *p_bpf_design = &p_design->bpf[bpf_idx];
                uint32_t fc_bw = p_bpf_format->acf_par_bpf_fc_bw;
                p_bpf_design->fc = fc_bw >> 16;
                p_bpf_design->bw = fc_bw & 0xFFFF;
                memcpy(&p_bpf_design->gain, &p_bpf_format->acf_par_bpf_gain, sizeof(float));
            }
        }
    }
    acf_handle.update_flag = ACF_TRUE;
    return 0;
#else  /* defined(MTK_AUDIO_IIR_FILTER_ENABLE) */
    return -1;
#endif /* defined(MTK_AUDIO_IIR_FILTER_ENABLE) */
}

#endif /* defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED) */
