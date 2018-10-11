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
#include "hal_log.h"
#include "hal_audio_enhancement.h"

static hal_audio_spe_struct_t speech_enh_handle;

uint8_t spe_table[SPH_MODE_UNDEFINED] = {
    /*--------earphone mode---------*/
    0 + SPE_AEC_FLAG + SPE_UL_NR_FLAG + SPE_DL_NR_FLAG + SPE_AGC_FLAG,
    /*--------loudspeaker mode---------*/
    0 + SPE_AEC_FLAG + SPE_UL_NR_FLAG + SPE_DL_NR_FLAG + SPE_AGC_FLAG,
    /*--------bt earphone mode ------------*/
#ifdef MTK_EXTERNAL_DSP_ENABLE
    0 + SPE_DL_NR_FLAG + SPE_AGC_FLAG,   /* UL enhancement is done by exteranl DSP. */
#else
    0 + SPE_AEC_FLAG + SPE_UL_NR_FLAG + SPE_DL_NR_FLAG + SPE_AGC_FLAG,
#endif
    /*--------bt speaker mode  ---------*/
    0 + SPE_AEC_FLAG + SPE_UL_NR_FLAG + SPE_DL_NR_FLAG + SPE_AGC_FLAG,
    /*--------record mode  ---------*/
    0 + SPE_AEC_FLAG + SPE_UL_NR_FLAG  + SPE_AGC_FLAG
};

#define NUMBER_AGC_IIR        40
#define NUMBER_SRC_FLT        82
#define NUMBER_SIDETONE_FLT   31


const int16_t hal_sph_agc_hp_flt[NUMBER_AGC_IIR] = {
    -7358, 15521,  7644, -15288,  7644, -7960, 16132,  8192, -16383,  8192, /* UL WB HP-IIR-137Hz (20ms task)*/
    -7831, 15995,  7579, -15158,  7579, -7347, 15512,  8192, -16384,  8192, /* UL NB HP-IIR-137Hz (20ms task)*/
    -8009, 16194,  7884, -15768,  7884, -7758, 15943,  8192, -16384,  8192, /* UL WB HP-IIR-75Hz (20ms task)*/
    -6450, 14504,  7030, -14060,  7030, -7685, 15779,  8193, -16384,  8193 /* UL NB HP-IIR-75Hz (20ms task)*/
};

// SRC coefiicient using in DSP
const int16_t hal_sph_src_flt[NUMBER_SRC_FLT] = {
    0,    96,   162,   -92,  -488,  -302,   408,   372,  -660,  -780,
    782,  1272,  -958, -2094,  1088,  3458, -1200, -6426,  1266, 20318,
    30834, 20318,  1266, -6426, -1200,  3458,  1088, -2094,  -958,  1272,
    782,  -780,  -660,   372,   408,  -302,  -488,   -92,   162,    96,
    0,  /* SRC Up-Sampling */
    0,     0,   -36,  -175,  -262,   -51,   277,   146,  -376,  -317,
    489,   595,  -597, -1034,   689,  1777,  -744, -3351,   727, 10500,
    15852, 10500,   727, -3351,  -744,  1777,   689, -1034,  -597,   595,
    489,  -317,  -376,   146,   277,   -51,  -262,  -175,   -36,     0,
    0  /* SRC Down-Sampling */
};

const int16_t hal_sph_sidetone_flt[NUMBER_SIDETONE_FLT] = {
    -4678, 11792,  407,  204,  407, -6978, 12257,  2929, -2945,  2929,
    0, 0,  0,  0,  0, 0, 0,  0, 0,  0,
    0, 0,  0,  0,  0, 0, 0,  0, 0,  0,
    0
};

const uint16_t hal_sph_default_common[NUMBER_COMMON_PARAS] = {
    6, 55997,  31000,  10752,  32769,    0,    0,    0,    0,  0,
    0,      0
};
const uint16_t hal_sph_default_nb_mode[NUMBER_MODE_PARAS] = {
    0,   253,  10756,    31, 53255,    31,   400,     0,    80,  4325,
    611,     0, 20488,     0,     0,    86,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0, 54740, 44461, 44461, 44461, 11693, 44461,  2580,  3858,
    39951, 1295, 521, 51, 2599, 20600, 22904, 36216
};
const uint16_t hal_sph_default_wb_mode[NUMBER_MODE_PARAS] = {
    0,   253, 10756,    31, 53511,    31,   400,     0,    80,  4325,
    611,     0, 16392,     0,     0,    86,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0, 54740, 44461, 44461, 44461, 11693, 44461,  2580,  3858,
    39951, 1295, 521, 51, 2599, 20600, 22904, 36216
};
const int16_t hal_sph_default_nb_fir[SPH_FLT_COEF_LEN_BKF_NB] = {
    32767,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,	    0,	   0
};
const int16_t hal_sph_default_wb_fir[SPH_FLT_COEF_LEN_BKF_WB] = {
    32767,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    0,     0,     0,     0,     0,     0,     0,     0,     0,     0
};
void speech_write_nb_fir_coeffs(const int16_t *in_coeff, const int16_t *out_coeff);
void speech_write_wb_fir_coeffs(const int16_t *in_coeff, const int16_t *out_coeff);
static uint16_t speech_get_audio_id(void)
{
    uint16_t audio_id = audio_get_id();
    audio_service_setflag(audio_id);
    return audio_id;
}
static void speech_free_audio_id(uint16_t audio_id)
{
    audio_service_clearflag(audio_id);
    audio_free_id(audio_id);
}

/*
Support AEC, NR(no DMNR), AGC, BKF, sidetone
*/
static volatile uint16_t *dsp_param_get_buf(hal_speech_enhancement_parameter_t type)
{
    volatile uint16_t *addr = 0;
    switch (type) {
        case SPH_SPE_COMMON_PARAMETER:
            addr = DSP_DM_ADDR(5, *DSP_SPH_EMP_ADDR);
            break;
        case SPH_SPE_MODE_PARAMETER:
            addr = DSP_DM_ADDR(SPH_EMP_PAGE, 0);
            break;
        case SPH_SPE_MODE_PARAMETER_WB:
            addr = DSP_DM_ADDR(SPH_EMP_PAGE, 0x30);
            break;
        default:
            return 0;
    }
    return addr;
}

volatile uint16_t *dsp_get_filter_addr(hal_speech_filter_type_t type)
{
    volatile uint16_t *addr = 0;
    switch (type) {
        case SPH_SPE_DSP_FILTER_SIDETONE:
            addr = DSP_PM_ADDR(SPH_FLT_PAGE, *DSP_SPH_FLT_COEF_ADDR_ST);
            break;
        case SPH_SPE_DSP_FILTER_BKF_NB_UL:
            addr = DSP_PM_ADDR(SPH_FLT_PAGE, *DSP_SPH_FLT_COEF_ADDR_BKF_NB_UL);
            break;
        case SPH_SPE_DSP_FILTER_BKF_NB_DL:
            addr = DSP_PM_ADDR(SPH_FLT_PAGE, *DSP_SPH_FLT_COEF_ADDR_BKF_NB_DL);
            break;
        case SPH_SPE_DSP_FILTER_SRC:
            addr = DSP_PM_ADDR(SPH_FLT_PAGE, *DSP_SPH_FLT_COEF_ADDR_SRC);
            break;
        case SPH_SPE_DSP_FILTER_AGC:
            addr = DSP_PM_ADDR(SPH_FLT_PAGE, DSP_SPH_FLT_COEF_ADDR_AGC);
            break;
        case SPH_SPE_DSP_FILTER_BKF_WB_UL:
            addr = DSP_PM_ADDR(SPH_FLT_PAGE, *DSP_SPH_FLT_COEF_ADDR_BKF_WB_UL);
            break;
        case SPH_SPE_DSP_FILTER_BKF_WB_DL:
            addr = DSP_PM_ADDR(SPH_FLT_PAGE, *DSP_SPH_FLT_COEF_ADDR_BKF_WB_DL);
            break;
        default:
            return 0;
    }
    return addr;
}

static void spe_load_speech_parameter(uint16_t *common_para, uint16_t *mode_para, uint16_t *wb_mode_param)
{
    uint16_t *param;
    volatile uint16_t *dsp_addptr;
    int16_t I = 0;
    uint16_t audio_id = speech_get_audio_id();

    // AGC IIR Filter
    dsp_addptr = dsp_get_filter_addr(SPH_SPE_DSP_FILTER_AGC);
    for (I = 0; I < NUMBER_AGC_IIR; I++) {
        *dsp_addptr++ = hal_sph_agc_hp_flt[I];
    }

    // SRC Filter
    dsp_addptr = dsp_get_filter_addr(SPH_SPE_DSP_FILTER_SRC);
    for (I = 0; I < NUMBER_SRC_FLT; I++) {
        *dsp_addptr++ = hal_sph_src_flt[I];
    }

    // side tone filter
    dsp_addptr = dsp_get_filter_addr(SPH_SPE_DSP_FILTER_SIDETONE);
    for (I = 0; I < NUMBER_SIDETONE_FLT; I++) {
        *dsp_addptr++ = hal_sph_sidetone_flt[I];
    }

    if (common_para) {
        param = common_para;
        dsp_addptr = dsp_param_get_buf(SPH_SPE_COMMON_PARAMETER);
        for (I = NUMBER_COMMON_PARAS; I > 0; I--) {
            *dsp_addptr++ = *param++;
        }
    }

    if (mode_para) {
        param = mode_para;
        dsp_addptr = dsp_param_get_buf(SPH_SPE_MODE_PARAMETER);
        for (I = NUMBER_MODE_PARAS; I > 0; I--) {
            *dsp_addptr++ = *param++;
        }
    }

    if (wb_mode_param) {
        param = wb_mode_param;
        dsp_addptr = dsp_param_get_buf(SPH_SPE_MODE_PARAMETER_WB);
        for (I = NUMBER_MODE_PARAS; I > 0; I--) {
            *dsp_addptr++ = *param++;
        }
    }
    speech_free_audio_id(audio_id);
}


static void spe_clear_dll_entry(uint8_t current_state)
{
    if (!(current_state & (SPE_AEC_FLAG + SPE_UL_NR_FLAG))) {
        *DSP_EC_SUPP &= ~FLAG_DDL_UL_ENTRY;
    }
    if (!(current_state & (SPE_DL_NR_FLAG + SPE_AEC_FLAG))) {
        *DSP_SPE_DL_DLL_ENTRY &= ~FLAG_DDL_DL_ENTRY;
    }
}

static uint8_t spe_turn_on_preparation(void)
{
    uint8_t next_state, keep_on_state, on_state, off_state;
    uint8_t I = 0;

    next_state = spe_table[speech_enh_handle.speech_mode];
    for (I = 0; I < NUM_OF_SPH_FLAG; I++) {
        uint8_t temp_mask_bit = (1 << I);
        if (((speech_enh_handle.spe_app_mask & temp_mask_bit) == 0) && (next_state & temp_mask_bit)) {
            next_state -= temp_mask_bit;
        }
    }
    printf("\n\r [SPE] turn on process,next_state = %x", next_state);
    keep_on_state = next_state & speech_enh_handle.spe_state;
    on_state = next_state - keep_on_state;
    off_state = speech_enh_handle.spe_state - keep_on_state;

    assert(0 == off_state);
    assert(on_state == next_state);

    if (next_state & (SPE_AEC_FLAG + SPE_UL_NR_FLAG + SPE_DL_NR_FLAG)) {
        spe_load_speech_parameter(speech_enh_handle.sph_common_para, speech_enh_handle.sph_mode_para, speech_enh_handle.sph_mode_para_wb);
        speech_write_nb_fir_coeffs(speech_enh_handle.sph_in_fir, speech_enh_handle.sph_out_fir);
        speech_write_wb_fir_coeffs(speech_enh_handle.sph_in_fir_wb, speech_enh_handle.sph_out_fir_wb);
    }

    return next_state;

}

static void spe_turn_on_process(uint8_t on_state)
{
    *DSP_UL_COMFORT_NOISE_THRESHOLD = 32;
    *DSP_UL_COMFORT_NOISE_SHIFT = (uint16_t) - 12;
    *DSP_SPH_ENH_DYNAMIC_SWITCH = 0x3C;

    if (on_state & SPE_AGC_FLAG) {  // Set AGC
        *DSP_AGC_CTRL = 0xB82A;
    }


    if (on_state & (SPE_AEC_FLAG + SPE_UL_NR_FLAG)) {  // Enable UL enhancement
        *DSP_EC_SUPP |= FLAG_DDL_UL_ENTRY;
    }

    if (on_state & (SPE_DL_NR_FLAG + SPE_AEC_FLAG)) { // Enable DL enhancement
        *DSP_SPE_DL_DLL_ENTRY |= FLAG_DDL_DL_ENTRY;
    }

    if (on_state & SPE_UL_NR_FLAG) {  // Enable UL NR
        _spe_set_state_(*DSP_UL_NR_CTRL, SPE_READY_STATE, 0x0);
    }

    if (on_state & SPE_DL_NR_FLAG) {  // Enable DL NR
        _spe_set_state_(*DSP_DL_NR_CTRL, SPE_READY_STATE, 0x0);
    }

    if (on_state & SPE_AEC_FLAG) {  // Enable AEC
        _spe_set_state_(*DSP_AEC_CTRL, SPE_READY_STATE, 0x0);
    }

    if (on_state & SPE_UL_NR_FLAG) {  // Wait UL NR
        _spe_wait_state_change_(*DSP_UL_NR_CTRL, SPE_WORKING_STATE, 0xFFFF);
    }

    if (on_state & SPE_DL_NR_FLAG) {  // Wait DL NR
        _spe_wait_state_change_(*DSP_DL_NR_CTRL, SPE_WORKING_STATE, 0xFFFF);
    }

    if (on_state & SPE_AEC_FLAG) {  // Wait AEC
        _spe_wait_state_change_(*DSP_AEC_CTRL, SPE_WORKING_STATE, 0xFFFF);
    }

}

static void spe_turn_off_process(uint8_t off_state)
{
    if (off_state & SPE_UL_NR_FLAG) {  // Disable UL NR
        _spe_set_state_(*DSP_UL_NR_CTRL, SPE_STOP_STATE, 0x0);
    }

    if (off_state & SPE_DL_NR_FLAG) {  // Disable DL NR
        _spe_set_state_(*DSP_DL_NR_CTRL, SPE_STOP_STATE, 0x0);
    }

    if (off_state & SPE_AEC_FLAG) {   // Disable AEC
        _spe_set_state_(*DSP_AEC_CTRL, SPE_STOP_STATE, 0x0);
    }

    if (off_state & SPE_UL_NR_FLAG) {  // Wait UL NR
        _spe_wait_state_change_(*DSP_UL_NR_CTRL, SPE_IDLE_STATE, 0xFFFF);
    }

    if (off_state & SPE_DL_NR_FLAG) { // Wait DL NR
        _spe_wait_state_change_(*DSP_DL_NR_CTRL, SPE_IDLE_STATE, 0xFFFF);
    }

    if (off_state & SPE_AEC_FLAG) {   // Wait AEC
        _spe_wait_state_change_(*DSP_AEC_CTRL, SPE_IDLE_STATE, 0xFFFF);
    }

    if (off_state & SPE_AGC_FLAG) {   // Clear AGC
        *DSP_AGC_CTRL = 0;
    }
}

static void spe_on(void)
{
    uint8_t next_state;
    next_state = spe_turn_on_preparation();
    spe_turn_on_process(next_state);

    speech_enh_handle.spe_state = next_state;
    spe_clear_dll_entry(speech_enh_handle.spe_state);
}
static void spe_off(void)
{
    if (speech_enh_handle.spe_state) {
        spe_turn_off_process(speech_enh_handle.spe_state);
        speech_enh_handle.spe_state = 0;
        spe_clear_dll_entry(0);
    }

}

void speech_write_nb_fir_coeffs(const int16_t *in_coeff, const int16_t *out_coeff)
{
    volatile uint16_t *dsp_addptr;
    uint8_t I = 0;
    dsp_addptr = dsp_get_filter_addr(SPH_SPE_DSP_FILTER_BKF_NB_UL);
    for (I = 0; I < SPH_FLT_COEF_LEN_BKF_NB; I++) {
        *dsp_addptr ++ = *in_coeff++;
    }
    dsp_addptr = dsp_get_filter_addr(SPH_SPE_DSP_FILTER_BKF_NB_DL);
    for (I = 0; I < SPH_FLT_COEF_LEN_BKF_NB; I++) {
        *dsp_addptr ++ = *out_coeff++;
    }
    *DSP_COMPEN_BLOCK_FLT_PAR_PTR |= (VOICE_INPUT_FILTER_FLAG + VOICE_OUTPUT_FILTER_FLAG);
    *DSP_AUDIO_PAR |= VOICE_16K_SWITCH_FLAG;
}

void speech_write_wb_fir_coeffs(const int16_t *in_coeff, const int16_t *out_coeff)
{
    volatile uint16_t *dsp_addptr;
    uint8_t I = 0;
    dsp_addptr = dsp_get_filter_addr(SPH_SPE_DSP_FILTER_BKF_WB_UL);
    for (I = 0; I < SPH_FLT_COEF_LEN_BKF_WB; I++) {
        *dsp_addptr ++ = *in_coeff++;
    }
    dsp_addptr = dsp_get_filter_addr(SPH_SPE_DSP_FILTER_BKF_WB_DL);
    for (I = 0; I < SPH_FLT_COEF_LEN_BKF_WB; I++) {
        *dsp_addptr ++ = *out_coeff++;
    }
}

/*
static void speech_write_st_coeffs(const int16_t *coeff)
{
    volatile uint16_t *dsp_addptr = dsp_get_filter_addr(SPH_SPE_DSP_FILTER_SIDETONE);
    uint8_t I = 0;
    for (I = 0; I < SPH_FLT_COEF_LEN_SIDETONE; I++) {
        *dsp_addptr++ = *coeff++;
    }
}
*/



void speech_set_application_mask(uint16_t spe_flags, bool enable)
{
    if (enable) {
        speech_enh_handle.spe_app_mask |= (spe_flags);
    } else {
        speech_enh_handle.spe_app_mask &= ~(spe_flags);
    }
}

// ============================================================================
// Public APIs
// ============================================================================

void speech_set_sidetone_volume(uint32_t sidetone_volume)
{
    afe_audio_set_sidetone_volume(sidetone_volume);
}

void speech_set_mode(hal_speech_device_mode_t mode)
{
    uint16_t audio_id = speech_get_audio_id();

    if (speech_enh_handle.spe_flag) {
        spe_off();
    }

    speech_enh_handle.speech_mode = mode;

    if (speech_enh_handle.spe_flag) {
        spe_on();
    }

    speech_free_audio_id(audio_id);
}




void speech_set_enhancement(bool enable)
{
    if (enable) {
        if (speech_enh_handle.spe_flag) {
            log_hal_error("%s() is retry with enable", __func__);
            return;
        }
        speech_enh_handle.spe_audio_id = speech_get_audio_id();
        dsp_audio_fw_dynamic_download(DDID_SPH_ENH);

        spe_on();
        speech_enh_handle.spe_flag = true;
    } else {
        if (false == speech_enh_handle.spe_flag) {
            log_hal_error("%s() is retry without enable", __func__);
            return;
        }

        spe_off();
        speech_enh_handle.spe_flag = false;

        speech_free_audio_id(speech_enh_handle.spe_audio_id);
        speech_enh_handle.spe_audio_id = 0;
    }


    if (speech_enh_handle.spe_app_mask & SPH_ENH_MASK_SIDETONE) {
        if (speech_enh_handle.speech_mode == SPH_MODE_LOUDSPEAKER) {
            afe_set_sidetone(false);
        } else {
            afe_set_sidetone(true);
        }
    }


}

void speech_update_common(const uint16_t *common)
{
    uint8_t i = 0;

    if (NULL != common) {
        for (i = 0; i < NUMBER_COMMON_PARAS; i++) {
            speech_enh_handle.sph_common_para[i] = *common++;
        }
    }
}

void speech_update_nb_param(const uint16_t *param)
{
    uint8_t i = 0;

    if (NULL != param) {
        for (i = 0; i < NUMBER_MODE_PARAS; i++) {
            speech_enh_handle.sph_mode_para[i] = *param++;
        }
    }
}

void speech_update_wb_param(const uint16_t *param)
{
    uint8_t i = 0;

    if (NULL != param) {
        for (i = 0; i < NUMBER_MODE_PARAS; i++) {
            speech_enh_handle.sph_mode_para_wb[i] = *param++;
        }
    }
}

void speech_update_nb_fir(const int16_t *in_coeff, const int16_t *out_coeff)
{
    uint8_t i = 0;

    if (NULL != in_coeff) {
        for (i = 0; i < SPH_FLT_COEF_LEN_BKF_NB; i++) {
            speech_enh_handle.sph_in_fir[i] = *in_coeff++;
        }
    }

    if (NULL != out_coeff) {
        for (i = 0; i < SPH_FLT_COEF_LEN_BKF_NB; i++) {
            speech_enh_handle.sph_out_fir[i] = *out_coeff++;
        }
    }
}

void speech_update_wb_fir(const int16_t *in_coeff, const int16_t *out_coeff)
{
    uint8_t i = 0;

    if (NULL != in_coeff) {
        for (i = 0; i < SPH_FLT_COEF_LEN_BKF_WB; i++) {
            speech_enh_handle.sph_in_fir_wb[i] = *in_coeff++;
        }
    }

    if (NULL != out_coeff) {
        for (i = 0; i < SPH_FLT_COEF_LEN_BKF_WB; i++) {
            speech_enh_handle.sph_out_fir_wb[i] = *out_coeff++;
        }
    }
}

void speech_reload_filter_coefficient(void)
{
    spe_load_speech_parameter(0,0,0);
}
void speech_init(void)
{
    speech_enh_handle.spe_app_mask = 0xFF;
    // load default par.
    speech_update_common(hal_sph_default_common);
    speech_update_nb_param(hal_sph_default_nb_mode);
    speech_update_wb_param(hal_sph_default_wb_mode);
    speech_update_nb_fir((int16_t *)hal_sph_default_nb_fir, (int16_t *)hal_sph_default_nb_fir);
    speech_update_wb_fir(hal_sph_default_wb_fir, hal_sph_default_wb_fir);
    spe_load_speech_parameter(speech_enh_handle.sph_common_para, speech_enh_handle.sph_mode_para, speech_enh_handle.sph_mode_para_wb);
}

#if (defined MTK_EXTERNAL_DSP_ENABLE) && (defined MTK_NDVC_ENABLE)
// dBSPL: <58, ~60, ~62, ~64, ~66, ~68, ~70, ~72, ~74, ~76, ~78, ~80, ~82, ~84, ~86, >88
// INDEX: 0,     1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14, 15
uint16_t spe_ndvc_uplink_noise_index_map(uint16_t db)
{
    if (db <= 58) {
        return 0;
    } else if (db >= 88) {
        return 15;
    }
    return (db-58)>>1 ;
}

void spe_ndvc_noise_setting(uint16_t index)
{
   *DSP_SPH_SWNDVC_POWER_INDEX = index;
}

#endif //(defined MTK_EXTERNAL_DSP_ENABLE) && (defined MTK_NDVC_ENABLE)

void spe_dump_common_param(void)
{
    int i;
    volatile uint16_t *dsp_addptr = DSP_DM_ADDR(5, *DSP_SPH_EMP_ADDR);
    for (i = 0; i < 12; i++) {
        log_hal_info("\r\n common_para[%d]=%d \r\n",i,*dsp_addptr++);
    }
}

void spe_dump_mode_param(void)
{
    int i;
    volatile uint16_t *dsp_addptr = DSP_DM_ADDR(SPH_EMP_PAGE, 0);
    for (i = 0; i < 48; i++) {
        log_hal_info("\r\n nb_mode_para[%d]=%d \r\n",i,*dsp_addptr++);
    }
    dsp_addptr = DSP_DM_ADDR(SPH_EMP_PAGE, 0x30);
    for (i = 0; i < 48; i++) {
        log_hal_info("\r\n wb_mode_para[%d]=%d \r\n",i,*dsp_addptr++);
    }
}

#endif /* defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED) */
