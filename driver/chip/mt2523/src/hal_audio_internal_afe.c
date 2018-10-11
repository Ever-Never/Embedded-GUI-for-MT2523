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

#include "hal_audio_internal_afe.h"

#if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED)
#if defined(__AFE_HP_DC_CALIBRATION__) || defined(__AFE_HS_DC_CALIBRATION__)
#include "hal_adc.h"
#endif

#include "hal_i2s_internal.h"

#define DIGITAL_AUDIO_STREAM_OUT_0DB_REGISTER_VALUE 32767
#define DIGITAL_SPEECH_STREAM_OUT_0DB_REGISTER_VALUE 4096
#define LN10 2.3025851  // ln(10)

#define SIDETONE_DB_MINIMUM 0
#define SIDETONE_DB_MAXIMUM 31


afe_t afe;

void afe_audio_switch_audio_amp(bool on);
void afe_audio_switch_voice_amp(bool on);
void afe_audio_set_digital_gain(void);
void afe_audio_set_analog_gain(void);
void afe_switch_no_voice_audio_amp(void);

extern hal_audio_stream_t audio_config;
const uint16_t  SPEECH_TX_INIT_GAIN = 0xE3D;
const uint16_t  SPEECH_RX_INIT_GAIN = 0x1400;  //fix, don't modify
const uint16_t  AUDIO_TX_INIT_GAIN  = 0x7FFF;

const uint8_t mic0_type_sel = AFE_MAIN_MIC_TYPE;
const uint8_t mic1_type_sel = AFE_EARPHONE_MIC_TYPE;
static const uint16_t SideToneTable[] = {
    32767, 29204, 26027, 23196, 20674, 18426, 16422, 14636, 13044, 11625,
    10361, 9234, 8230, 7335, 6537, 5826, 5193, 4628, 4125, 3676,
    3276, 2919, 2602, 2319, 2066, 1841, 1641, 1463, 1304, 1162,
    1035, 923, 822, 733, 653, 582, 519, 462, 412, 367,
    327, 291, 260, 231, 206, 183, 163, 145
};

#ifdef __AFE_HP_ANALOG_DEPOP_SWITCH__

extern const char BSP_HP_ANA_SW_PIN;
extern const char BSP_HP_ANA_SW_PIN_M_GPIO;

#if (AFE_HP_DEPOP_ENABLE_LEVEL == 1) //High enable
#define AFE_DEPOP_GPIO_DATA_ON  HAL_GPIO_DATA_HIGH
#define AFE_DEPOP_GPIO_DATA_OFF HAL_GPIO_DATA_LOW
#else //AFE_HP_DEPOP_ENABLE_LEVEL == 0). Low enable
#define AFE_DEPOP_GPIO_DATA_ON  HAL_GPIO_DATA_LOW
#define AFE_DEPOP_GPIO_DATA_OFF HAL_GPIO_DATA_HIGH
#endif

static void afe_hp_buffer_sw(bool on)
{
    hal_gpio_status_t   gpio_status;
    hal_pinmux_status_t status;
    hal_gpio_pin_t      gpio_pin = (hal_gpio_pin_t)BSP_HP_ANA_SW_PIN;

    gpio_status = hal_gpio_init(gpio_pin);
    if (gpio_status != HAL_GPIO_STATUS_OK) {
        return;
    }
    status = hal_pinmux_set_function(gpio_pin, BSP_HP_ANA_SW_PIN_M_GPIO);
    if (status != HAL_PINMUX_STATUS_OK) {
        return;
    }
    hal_gpio_set_direction(gpio_pin, HAL_GPIO_DIRECTION_OUTPUT);
    if (on) {
        hal_gpio_set_output(gpio_pin, AFE_DEPOP_GPIO_DATA_ON);
    } else {
        hal_gpio_set_output(gpio_pin, AFE_DEPOP_GPIO_DATA_OFF);
    }
}

#endif /*__AFE_HP_ANALOG_DEPOP_SWITCH__*/

#if defined(HAL_I2S_MODULE_ENABLED)
void afe_i2s_pinmux_switch(bool i2s_enable)
{
#ifdef __AFE_I2S_DYNAMIC_SWITCH__
    hal_gpio_status_t   gpio_status;

    log_hal_info("\r\n [%s] i2s_enable=%x", __FUNCTION__, i2s_enable);
    gpio_status = hal_gpio_init((hal_gpio_pin_t)HAL_I2S_MASTER_EDIDO_PIN);
    gpio_status |= hal_gpio_init((hal_gpio_pin_t)HAL_I2S_MASTER_EDIDI_PIN);
    gpio_status |= hal_gpio_init((hal_gpio_pin_t)HAL_I2S_MASTER_EDIWS_PIN);
    gpio_status |= hal_gpio_init((hal_gpio_pin_t)HAL_I2S_MASTER_EDICK_PIN);
    if (gpio_status != HAL_GPIO_STATUS_OK) {
        return;
    }
    if (i2s_enable) {
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_I2S_MASTER_EDIDO_PIN, HAL_I2S_MASTER_EDIDO_PIN_M_MA_EDIDO);
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_I2S_MASTER_EDIDI_PIN, HAL_I2S_MASTER_EDIDI_PIN_M_MA_EDIDI);
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_I2S_MASTER_EDIWS_PIN, HAL_I2S_MASTER_EDIWS_PIN_M_MA_EDIWS);
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_I2S_MASTER_EDICK_PIN, HAL_I2S_MASTER_EDICK_PIN_M_MA_EDICK);
        hal_gpio_set_direction((hal_gpio_pin_t)HAL_I2S_MASTER_EDIWS_PIN, HAL_GPIO_DIRECTION_OUTPUT);
        hal_gpio_set_direction((hal_gpio_pin_t)HAL_I2S_MASTER_EDICK_PIN, HAL_GPIO_DIRECTION_OUTPUT);
        hal_gpio_set_output((hal_gpio_pin_t)HAL_I2S_MASTER_EDIWS_PIN, HAL_GPIO_DATA_HIGH);
        hal_gpio_set_output((hal_gpio_pin_t)HAL_I2S_MASTER_EDICK_PIN, HAL_GPIO_DATA_HIGH);
    } else {
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_I2S_MASTER_EDIDO_PIN, HAL_I2S_MASTER_EDIDO_PIN_M_GPIO);
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_I2S_MASTER_EDIDI_PIN, HAL_I2S_MASTER_EDIDI_PIN_M_GPIO);
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_I2S_MASTER_EDIWS_PIN, HAL_I2S_MASTER_EDIWS_PIN_M_GPIO);
        hal_pinmux_set_function((hal_gpio_pin_t)HAL_I2S_MASTER_EDICK_PIN, HAL_I2S_MASTER_EDICK_PIN_M_GPIO);
        hal_gpio_set_direction((hal_gpio_pin_t)HAL_I2S_MASTER_EDIWS_PIN, HAL_GPIO_DIRECTION_OUTPUT);
        hal_gpio_set_direction((hal_gpio_pin_t)HAL_I2S_MASTER_EDICK_PIN, HAL_GPIO_DIRECTION_OUTPUT);
        hal_gpio_set_output((hal_gpio_pin_t)HAL_I2S_MASTER_EDIWS_PIN, HAL_GPIO_DATA_LOW);
        hal_gpio_set_output((hal_gpio_pin_t)HAL_I2S_MASTER_EDICK_PIN, HAL_GPIO_DATA_LOW);
    }
#endif
}
#endif

static void afe_chip_init(void)
{
#ifdef __AFE_HP_ANALOG_DEPOP_SWITCH__
    afe_hp_buffer_sw(false);
#endif
#if defined(HAL_I2S_MODULE_ENABLED)
    afe_i2s_pinmux_switch(false);
#endif
    /*Digital part Initialization*/
    *AFE_AMCU_CON1   = 0x0E00;
    *AFE_AMCU_CON5   = 0x0002;
    *AFE_VMCU_CON3   = 0x0002;
    *AFE_VMCU_CON2   = 0x083C;
    *AFE_AMCU_CON2   = 0x003C;
    *AFE_VMCU_CON1   = 0x0080;
    *AFE_EDI_CON     = 0x003C;

    /*Uplink PGA Gain : 6dB*/
    *ABBA_VBITX_CON0 |= (0x1 << 6);

    /*Analog part Initialization and power-on control sequence*/
    *ABBA_TOP_CON0 |= 0x8000; //enable then clock gating LVSH would release
#if !defined(__AFE_HS_DC_CALIBRATION__)
    //For HP path, if need audio buffer function, please set these REGs.
    *ABBA_AUDIODL_CON4  = 0x01B9;
    hal_gpt_delay_us(10);
    *ABBA_AUDIODL_CON4 |= 0x0040;
    /*should wait 2s~3s to charge cap*/
    *ABBA_AUDIODL_CON10 = 0x1406; //LDO:2.4v
#endif

    /*main mic*/
    if (mic0_type_sel == 0) { //acc
        *ABBA_VBITX_CON4 &= 0xA7FF;
    } else if (mic0_type_sel == 1) { //dcc mems
        *ABBA_VBITX_CON4 = (*ABBA_VBITX_CON4 & 0xA7FF) | 0x4000;
    } else if (mic0_type_sel == 2) { //dcc ecm
        *ABBA_VBITX_CON4 = (*ABBA_VBITX_CON4 & 0xA7FF) | 0x4800;
    }

    /*ear-mic*/
    if (mic1_type_sel == 0) { //acc
        *ABBA_VBITX_CON4 &= 0xD9FF;
    } else if (mic1_type_sel == 1) { //dcc mems
        *ABBA_VBITX_CON4 = (*ABBA_VBITX_CON4 & 0xD9FF) | 0x2000;
    } else if (mic1_type_sel == 2) { //dcc ecm
        *ABBA_VBITX_CON4 = (*ABBA_VBITX_CON4 & 0xD9FF) | 0x2400;
    }

    *DSP_SPH_UL_VOL    = SPEECH_RX_INIT_GAIN;
    *DSP_SPH_DL_VOL    = SPEECH_TX_INIT_GAIN;
    /* sidetone initialization*/
    afe.sidetone_volume = 0; //0 = mute
    *DSP_SPH_ST_VOL = 0;
    //[To Do]WB path path gain and audio gain init need to be added.

}

void afe_digital_set_wideband_voice(bool wideband)
{
    if (wideband) {
        *AFE_VMCU_CON1 &= ~0x0200;  // set AFE to WB
    } else {
        *AFE_VMCU_CON1 |= 0x0200;
    }
}

void afe_digital_loopback(bool enable)
{
    if (enable) {
        afe.loopback = true;
        afe_voice_digital_on();
        afe_switch_tx_signal_path(true);
        afe_switch_rx_signal_path();
        *AFE_VLB_CON |= 0x0022;
    } else {
        *AFE_VLB_CON &= (~0x62);
        afe_audio_analog_device_off();
        afe_voice_digital_off();
        afe.loopback = false;
    }
}

void afe_set_path_type(hal_audio_active_type_t type)
{
    afe.audio_path_type = type;
}

static uint16_t afe_audio_get_stream_in_out_path(void)
{
    speech_path_type_t path = audio_pcm2way_get_path_type();
    if (afe.pcmif_loopback || path == SPH_BOTH_PATH || afe.loopback || afe.audio_path_type == HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE) {
        return STREAM_IN_OUT_PATH;
    } else if (afe.audio_path_type == HAL_AUDIO_PLAYBACK_MUSIC || afe.audio_path_type == HAL_AUDIO_PLAYBACK_VOICE || path == SPH_VOICE_PCM2WAY_DL) {
        return STREAM_OUT_PATH;
    } else {
        return STREAM_IN_PATH;
    }
}

void afe_digital_audio_set_sampling_rate(uint8_t frequency)
{
    uint16_t sample_rate;
    if (!afe.audio_clock_enable) {
        switch (frequency) {
            case HAL_AUDIO_SAMPLING_RATE_8KHZ        :
                sample_rate = 0;
                break;
            case HAL_AUDIO_SAMPLING_RATE_11_025KHZ   :
                sample_rate = 1;
                break;
            case HAL_AUDIO_SAMPLING_RATE_12KHZ       :
                sample_rate = 2;
                break;
            case HAL_AUDIO_SAMPLING_RATE_16KHZ       :
                sample_rate = 4;
                break;
            case HAL_AUDIO_SAMPLING_RATE_22_05KHZ    :
                sample_rate = 5;
                break;
            case HAL_AUDIO_SAMPLING_RATE_24KHZ       :
                sample_rate = 6;
                break;
            case HAL_AUDIO_SAMPLING_RATE_32KHZ       :
                sample_rate = 8;
                break;
            case HAL_AUDIO_SAMPLING_RATE_44_1KHZ     :
                sample_rate = 9;
                break;
            case HAL_AUDIO_SAMPLING_RATE_48KHZ       :
                sample_rate = 10;
                break;
            default                                  :
                sample_rate = 0;
                break;
        }
        *AFE_AMCU_CON1 = ((*AFE_AMCU_CON1) & (~0x03C0)) | (sample_rate << 6);
    }
}

void afe_digital_switch_digital_clock(bool on, bool audio)
{
    //uint32_t save_mask save_and_set_interrupt_mask();
    if (on) {
        if ((audio && (!afe.audio_clock_enable)) || (!audio && (!afe.voice_clock_enable))) {
            *HW_MISC |= 0x0013;
            *ABBA_AUDIODL_CON10  = 0x1406;
            *ABBA_AUDIODL_CON9  |= 0x0001;
            *ABBA_AUDIODL_CON11 |= 0x0003;
            hal_gpt_delay_us(1);
            if (audio) {
                *ABBA_AUDIODL_CON7 |= 0x0003;
            } else {
                *ABBA_AUDIODL_CON7 |= 0x0002;
            }
            *ABBA_AUDIODL_CON0 |= (RG_AUDDACRPWRUP | RG_AUDDACLPWRUP);
            *ABBA_AUDIODL_CON15  = 0x11A1;
            *ABBA_AUDIODL_CON14 |= 0x0001;
            *MD2GSYS_CG_CLR2     = 0x0100;
            if (audio) {
                if (afe_audio_get_stream_in_out_path() == STREAM_IN_OUT_PATH) {
                    *AFE_MCU_CON1 |= 0x000F;/*rx +tx*/
                } else {
                    *AFE_MCU_CON1 |= 0x000C;
                }
                *AFE_AMCU_CON0  |= 0x0001;
            } else {
                if (afe_audio_get_stream_in_out_path() == STREAM_IN_OUT_PATH) {
                    *AFE_MCU_CON1 |= 0x000F;/*rx +tx*/
                } else if (afe_audio_get_stream_in_out_path() == STREAM_OUT_PATH) {
                    *AFE_MCU_CON1 |= 0x000C;/*tx only*/
                } else {
                    *AFE_MCU_CON1 |= 0x0003;/*rx only*/
                }
                *AFE_VMCU_CON0 |= 0x0001;
            }
            *AFE_MCU_CON0 = 0x0001;
        }
    } else {
        *ABBA_AUDIODL_CON0  &= (~(RG_AUDDACRPWRUP | RG_AUDDACLPWRUP));
        *ABBA_AUDIODL_CON7  &= (~0x0003);
        *ABBA_AUDIODL_CON11 &= (~0x0003);
        *AFE_MCU_CON0 &= (~0x0001);
        if (audio) {
            *AFE_AMCU_CON0  &= (~0x0001);
        } else {
            *AFE_VMCU_CON0  &= (~0x0001);
        }
        *AFE_MCU_CON1 &= (~0x000F);
        *MD2GSYS_CG_SET2     = 0x0100;
        *ABBA_AUDIODL_CON14 &= (~0x0001);
        *ABBA_AUDIODL_CON15  = 0x1021;
        *ABBA_AUDIODL_CON9  &= (~0x0001);
        *HW_MISC &= (~0x0010);
    }
    //restore_interrupt_mask(save_mask);
}

void afe_audio_digital_on(uint16_t sample_rate)
{
    //uint32_t save_mask save_and_set_interrupt_mask();
    afe_digital_audio_set_sampling_rate(sample_rate);
    afe_digital_switch_digital_clock(true, true);
    afe.audio_clock_enable = true;
    //restore_interrupt_mask(save_mask);
}

void afe_audio_digital_off(void)
{
    //uint32_t save_mask save_and_set_interrupt_mask();
    afe_digital_switch_digital_clock(false, true);
    afe.audio_clock_enable = false;
    //restore_interrupt_mask(save_mask);
}

void afe_voice_digital_on(void)
{
    *AFE_VMCU_CON3 |= 0x0020;
    afe_digital_switch_digital_clock(true, false);
    afe.voice_clock_enable = true;
}

void afe_voice_digital_off(void)
{
    afe_digital_switch_digital_clock(false, false);
    *AFE_VMCU_CON3 &= (~0x0020);
    afe.voice_clock_enable = false;
}

void afe_audio_initial(void)
{
    afe.audio_clock_enable = false;
    afe.voice_clock_enable = false;
    afe.stream_in_mute     = false;
    afe.stream_out_mute    = false;
    afe.pcmif_loopback     = false;
    afe.init               = false;
    afe.audio_tx_enable    = false;
    afe.loopback           = false;
    afe.mic_bias           = false;
    afe.sidetone_flag      = false;
    afe.stream_in_digital_gain_in_01unit_db  = 0x3E8; //10dB
    afe.stream_in_analog_gain_in_01unit_db   = 0x258; //6dB
    afe.stream_out_digital_gain_in_01unit_db = 0x0;
    afe.stream_out_analog_gain_in_01unit_db  = 0x0; //min: HS:0dB / HP:0dB
    afe.sidetone_volume   = 0;
    afe.output_device     = HAL_AUDIO_DEVICE_NONE;
    afe.input_device      = HAL_AUDIO_DEVICE_NONE;
#if defined(HAL_I2S_MODULE_ENABLED)
    afe.ext_output_device = HAL_AUDIO_EXT_DEVICE_NONE;
#endif
    afe.audio_path_type   = HAL_AUDIO_PLAYBACK_MUSIC;
    afe.headphone_on      = false;
    afe.handset_on        = false;
    afe.hp_hs_both_on     = false;
    afe.line_in_enable    = false;
    afe.ext_pa_on         = false;
    afe.edi_on            = false;
    afe_chip_init();

#if defined(__AFE_HP_DC_CALIBRATION__) || defined(__AFE_HS_DC_CALIBRATION__)
    if (afe.dc_cali_data.calibration_sta == 0) {
    #ifdef __AFE_HS_DC_CALIBRATION__
        afe_voice_buffer_dc_calibration();
    #endif /*__AFE_HS_DC_CALIBRATION__*/
    #ifdef __AFE_HP_DC_CALIBRATION__
        afe_audio_buffer_dc_calibration();
    #endif /*__AFE_HP_DC_CALIBRATION__*/
        afe.dc_cali_data.calibration_sta = 1;
    }
#endif
    afe_backup_register();
    afe.init              = true;
}

void afe_audio_deinit(void)
{
    *ABBA_AUDIODL_CON4 &= (~RG_ADEPOP_EN);
    *ABBA_AUDIODL_CON4 &= (~RG_DEPOP_VCM_EN);
}

void afe_backup_register(void)
{
    afe.register_backup.VMCU_CON0 = *AFE_VMCU_CON0;
    afe.register_backup.VDB_CON   = *AFE_VDB_CON;
    afe.register_backup.VLB_CON   = *AFE_VLB_CON;
    afe.register_backup.AMCU_CON0 = *AFE_AMCU_CON0;
    afe.register_backup.AMCU_CON1 = *AFE_AMCU_CON1;
    afe.register_backup.EDI_CON   = *AFE_EDI_CON;
    afe.register_backup.AMCU_CON2 = *AFE_AMCU_CON2;
    afe.register_backup.DAC_TEST  = *AFE_DAC_TEST;
    afe.register_backup.VMCU_CON1 = *AFE_VMCU_CON1;
    afe.register_backup.VMCU_CON2 = *AFE_VMCU_CON2;
    afe.register_backup.MCU_CON0  = *AFE_MCU_CON0;
    afe.register_backup.MCU_CON1  = *AFE_MCU_CON1;
}

void afe_restore_register(void)
{
    if (!afe.init) {
        return;
    }
    *AFE_VMCU_CON0  = afe.register_backup.VMCU_CON0;
    *AFE_VDB_CON   = afe.register_backup.VDB_CON;
    *AFE_VLB_CON   = afe.register_backup.VLB_CON;
    *AFE_AMCU_CON0 = afe.register_backup.AMCU_CON0;
    *AFE_AMCU_CON1 = afe.register_backup.AMCU_CON1;
    *AFE_EDI_CON   = afe.register_backup.EDI_CON;
    *AFE_AMCU_CON2 = afe.register_backup.AMCU_CON2;
    *AFE_DAC_TEST  = afe.register_backup.DAC_TEST;
    *AFE_VMCU_CON1 = afe.register_backup.VMCU_CON1;
    *AFE_VMCU_CON2 = afe.register_backup.VMCU_CON2;
}

void afe_switch_mic_bias(bool on)
{
    if (on) {
        afe.mic_bias = true;
    } else {
        afe.mic_bias = false;
    }
}

void afe_mic_power_setting(void)
{
    if (afe.stream_in_mute || (!afe.mic_bias)) {
        if (afe.mic_bias) {
            *ABBA_VBITX_CON1 &= (~RG_VPWDB_ADC);
            *ABBA_VBITX_CON0 &= (~RG_VPWDB_PGA);
            *ABBA_VBITX_CON4 |= (RG_VPWDB_MBIAS);
            *ABBA_VBITX_CON0 |= (RG_VCFG_1); //avoid cross-talk
            *ABBA_VBITX_CON1 |= (RG_VREF24_EN | RG_VCM14_EN);
        } else {
            *ABBA_VBITX_CON1 &= (~RG_VPWDB_ADC);
            *ABBA_VBITX_CON1 &= (~(RG_VREF24_EN | RG_VCM14_EN));
            *ABBA_VBITX_CON0 &= (~RG_VPWDB_PGA);
            *ABBA_VBITX_CON4 &= (~RG_VPWDB_MBIAS);
            *ABBA_VBITX_CON0 |= (RG_VCFG_1);
        }
        return;
    } else if (afe.input_device != HAL_AUDIO_DEVICE_NONE) {
        *ABBA_VBITX_CON4 |= (RG_VPWDB_MBIAS);
        *ABBA_VBITX_CON1 |= (RG_VPWDB_ADC | RG_VREF24_EN | RG_VCM14_EN);
        *ABBA_VBITX_CON0 |= (RG_VPWDB_PGA);
        *ABBA_VBITX_CON0 &= ~(RG_VCFG_1);
        if ((mic0_type_sel != 0) && (mic1_type_sel != 0)) { //iff VIN0 and VIN1 are not ACC (both should be the same in general)
            *ABBA_VBITX_CON0 |= (RG_VPGA_DC_PRECHARGE);
            hal_gpt_delay_us(100);
            *ABBA_VBITX_CON0 &= ~(RG_VPGA_DC_PRECHARGE);
        }
    }
}

void afe_audio_device_volume_update(void)
{
    afe_audio_set_digital_gain();
    afe_audio_set_analog_gain();
}

void afe_audio_set_output_volume(uint32_t analog_gain_in_01unit_db, uint32_t digital_gain_in_01unit_db)
{
    afe.stream_out_analog_gain_in_01unit_db = analog_gain_in_01unit_db;
    afe.stream_out_digital_gain_in_01unit_db = digital_gain_in_01unit_db;
    afe_audio_device_volume_update();
}

void afe_audio_set_input_volume(uint32_t analog_gain_in_01unit_db, uint32_t digital_gain_in_01unit_db)
{
    afe.stream_in_analog_gain_in_01unit_db = analog_gain_in_01unit_db;
    afe.stream_in_digital_gain_in_01unit_db = digital_gain_in_01unit_db;
    afe_audio_device_volume_update();
}

void afe_truncate_out_of_range_value(int32_t *truncate_value, int32_t minimum, int32_t maximum)
{
    *truncate_value = *truncate_value < minimum ? minimum : *truncate_value;
    *truncate_value = *truncate_value > maximum ? maximum : *truncate_value;
}

uint32_t afe_dB_transfer_to_register_value(int32_t input_dB, int32_t maximum_db_value, int32_t minimum_db_value, int32_t db_step_value, uint32_t maximum_register_value, uint32_t minimum_register_value, uint32_t minimum_db_to_minimum_reg_value)
{
    int32_t input_dB_to_dB_step = 0;
    uint32_t register_value = 0;

    afe_truncate_out_of_range_value(&input_dB, minimum_db_value, maximum_db_value);

    input_dB_to_dB_step = (input_dB - minimum_db_value) / db_step_value;

    if (minimum_db_to_minimum_reg_value) {
        register_value = minimum_register_value + (uint32_t)input_dB_to_dB_step;
    } else {
        register_value = maximum_register_value - (uint32_t)input_dB_to_dB_step;
    }


    if (register_value > maximum_register_value) {
        register_value = maximum_register_value;
    }

    if ((int)register_value < 0) {
        register_value = 0;
    }

    return register_value;
}

void afe_audio_set_uplink_gain(void)
{
    uint16_t  agc_ul_gain, pga_ul_gain = 0;

    int32_t db_in_1unit = 0;
    int32_t db_max = 0;
    int32_t db_min = 0;
    int32_t db_step = 1;
    uint32_t reg_max = 0;
    uint32_t reg_min = 0;
    uint32_t minimum_db_to_minimum_reg_value = 1;

    if (afe.stream_in_mute) {
        pga_ul_gain = 0; //0dB
        agc_ul_gain = 0;
        *DSP_SPH_MUTE_CTRL |= 0x0001; //mute UL
    } else {
        db_in_1unit = ((int32_t)afe.stream_in_analog_gain_in_01unit_db) / 100;
        db_max = 18;
        db_min = 0;
        db_step = 6;
        reg_max = 3;
        reg_min = 0;
        minimum_db_to_minimum_reg_value = 1;
        afe.stream_in_analog_gain_index = afe_dB_transfer_to_register_value(db_in_1unit, db_max, db_min, db_step, reg_max, reg_min, minimum_db_to_minimum_reg_value);

        db_in_1unit = ((int32_t)afe.stream_in_digital_gain_in_01unit_db) / 100;
        db_max = 40;
        db_min = 0;
        db_step = 1;
        reg_max = 40;
        reg_min = 0;
        minimum_db_to_minimum_reg_value = 1;
        afe.stream_in_digital_gain_index = afe_dB_transfer_to_register_value(db_in_1unit, db_max, db_min, db_step, reg_max, reg_min, minimum_db_to_minimum_reg_value);

        pga_ul_gain = afe.stream_in_analog_gain_index;
        agc_ul_gain = afe.stream_in_digital_gain_index;
        *DSP_SPH_MUTE_CTRL &= ~0x0001; //unmute UL
    }
    /* Config dB directly, only positive gain*/
    *DSP_AGC_GAIN = agc_ul_gain;
    /* *ABBA_VBITX_CON0[8:6]: 0db~+18db, 6db per step*/
    *ABBA_VBITX_CON0 = ((*ABBA_VBITX_CON0 & (~0x01C0)) | (pga_ul_gain << 6));
    *DSP_SPH_UL_VOL    = SPEECH_RX_INIT_GAIN;
}

void afe_set_sidetone(bool enable)
{
    afe.sidetone_flag = enable;
    if (afe.sidetone_flag == true) { //enable sidetone
        *DSP_SPH_SFE_CTRL = ((*DSP_SPH_SFE_CTRL & (~0x01)) | 0x08); //bit3=1 ,bit0 = 0;
    } else { //disable sidetone
        *DSP_SPH_SFE_CTRL = ((*DSP_SPH_SFE_CTRL & (~0x08)));  //bit3=0
    }
}
void afe_audio_set_sidetone_volume(uint32_t sidetone_volume)
{
    int32_t temp_int32_stream_out_in_01unit_db = 0;
    int32_t temp_int32_sidetone_volume = 0;
    int32_t temp_int32_stream_in_digital_gain_in_01unit_db = 0;
    uint8_t temp_uint8_sidetone_volume = 0;

    speech_path_type_t path = audio_pcm2way_get_path_type();
    afe.sidetone_volume = sidetone_volume; // MMI_Sidetone_Volume
    if (path == SPH_BOTH_PATH) {
        temp_int32_stream_out_in_01unit_db = (int32_t)afe.stream_out_analog_gain_in_01unit_db;
        temp_int32_stream_in_digital_gain_in_01unit_db = (int32_t)afe.stream_in_digital_gain_in_01unit_db;
        temp_int32_sidetone_volume = (int32_t)sidetone_volume;
        afe_truncate_out_of_range_value(&temp_int32_sidetone_volume, SIDETONE_DB_MINIMUM, SIDETONE_DB_MAXIMUM);
        temp_uint8_sidetone_volume = (uint8_t)temp_int32_sidetone_volume;

        afe_audio_update_sidetone_volume((int)(temp_int32_stream_out_in_01unit_db / 100), temp_uint8_sidetone_volume, (int)(temp_int32_stream_in_digital_gain_in_01unit_db / 100));
    } else {
        *DSP_SPH_ST_VOL = 0;
    }
}

void afe_audio_update_sidetone_volume(int DL_PGA_Gain, int Sidetone_Volume, uint8_t SW_AGC_Ul_Gain)
{
    int vol = 0;
    uint16_t DSP_ST_GAIN = 0;
    int UL_PGA_GAIN_OFFSET = 2;//default UL_PGA_GAIN_OFFSET=2

    log_hal_info("pdateSidetone DL_PGA_Gain = %d MMI_Sidetone_Volume = %d SW_AGC_Ul_Gain = %d", DL_PGA_Gain, Sidetone_Volume, SW_AGC_Ul_Gain);


    vol = Sidetone_Volume + SW_AGC_Ul_Gain; //1dB/step
    vol = DL_PGA_Gain - vol + 55 - UL_PGA_GAIN_OFFSET;
    log_hal_info("vol = %d", vol);
    if (vol < 0) {
        vol = 0;
    }
    if (vol > 47) {
        vol = 47;
    }
    DSP_ST_GAIN = SideToneTable[vol]; //output 1dB/step
    if (Sidetone_Volume == 0) {
        DSP_ST_GAIN = 0 ;
    }
    log_hal_info("DSP_ST_GAIN = %d", DSP_ST_GAIN);
    *DSP_SPH_ST_VOL = DSP_ST_GAIN;
    //return DSP_ST_GAIN;
}

void afe_audio_set_analog_gain(void)
{
    uint16_t amp_gain = 0;
    speech_path_type_t path = audio_pcm2way_get_path_type();
    int32_t temp_int32_stream_out_in_01unit_db = 0;
    int32_t temp_int32_sidetone_volume = 0;
    int32_t temp_int32_stream_in_digital_gain_in_01unit_db = 0;
    uint8_t temp_uint8_sidetone_volume = 0;

    int32_t db_in_1unit = 0;
    int32_t db_max = 0;
    int32_t db_min = 0;
    int32_t db_step = 1;
    uint32_t reg_max = 0;
    uint32_t reg_min = 0;
    uint32_t minimum_db_to_minimum_reg_value = 1;

    if (afe.stream_out_mute) {
        *ABBA_AUDIODL_CON16 |= (RG_AMUTER | RG_AMUTEL); // mute R and L at digital
    } else {
        if (afe.output_device == HAL_AUDIO_DEVICE_HANDSET || afe.output_device == HAL_AUDIO_DEVICE_HANDS_FREE_MONO || afe.output_device == HAL_AUDIO_DEVICE_HANDS_FREE_STEREO) {
            db_in_1unit = ((int32_t)afe.stream_out_analog_gain_in_01unit_db) / 100;
            db_max = 8;
            db_min = -22;
            db_step = 1;
            reg_max = 30;
            reg_min = 0;
            minimum_db_to_minimum_reg_value = 0;
            afe.stream_out_analog_gain_index = afe_dB_transfer_to_register_value(db_in_1unit, db_max, db_min, db_step, reg_max, reg_min, minimum_db_to_minimum_reg_value);

            amp_gain = afe.stream_out_analog_gain_index;
            *ABBA_AUDIODL_CON13 = (*ABBA_AUDIODL_CON13 & 0x83FF) | (uint16_t)(amp_gain << 10);
        } else {
            db_in_1unit = ((int32_t)afe.stream_out_analog_gain_in_01unit_db) / 100;
            db_max = 11;
            db_min = -3;
            db_step = 1;
            reg_max = 14;
            reg_min = 0;
            minimum_db_to_minimum_reg_value = 0;
            afe.stream_out_analog_gain_index = afe_dB_transfer_to_register_value(db_in_1unit, db_max, db_min, db_step, reg_max, reg_min, minimum_db_to_minimum_reg_value);
            amp_gain = afe.stream_out_analog_gain_index;
            *ABBA_AUDIODL_CON13 = (*ABBA_AUDIODL_CON13 & 0xFC21) | (uint16_t)(amp_gain << 1) | (uint16_t)(amp_gain << 6);
#ifdef MTK_AUTOMOTIVE_SUPPORT
            //temp mark for IME project
            *ABBA_AUDIODL_CON13 = 0x7FBD;
#endif
        }
        *ABBA_AUDIODL_CON16 &= (~(RG_AMUTER | RG_AMUTEL));
    }
    afe_audio_set_uplink_gain();
    if (path == SPH_BOTH_PATH) {
        temp_int32_stream_out_in_01unit_db = (int32_t)afe.stream_out_analog_gain_in_01unit_db;
        temp_int32_stream_in_digital_gain_in_01unit_db = (int32_t)afe.stream_in_digital_gain_in_01unit_db;
        temp_int32_sidetone_volume = (int32_t)afe.sidetone_volume;
        afe_truncate_out_of_range_value(&temp_int32_sidetone_volume, SIDETONE_DB_MINIMUM, SIDETONE_DB_MAXIMUM);
        temp_uint8_sidetone_volume = (uint8_t)temp_int32_sidetone_volume;

        afe_audio_update_sidetone_volume((int)(temp_int32_stream_out_in_01unit_db / 100), temp_uint8_sidetone_volume, (int)(temp_int32_stream_in_digital_gain_in_01unit_db / 100));
    } else {
        *DSP_SPH_ST_VOL = 0;
    }
}

// (float x)^(int n)
float afe_fast_pow_function(float x, int n)
{
    float resut = 1.0;
    float temp = x;
    unsigned m = (n >= 0) ? n : -n;
    while (m) {
        if (m & 1) {
            resut *= temp;
        }
        temp *= temp;
        m >>= 1;
    }
    return (n >= 0) ? resut : (float)1.0 / resut;
}

// e^x
float afe_exp_power_function(const float x, const float minimum_acceptable_error)
{
    float ans1 ;
    float ans2 = 1.0;
    float fact = 1, xn = x, cnt = 2.0;
    do {
        ans1 = ans2;
        ans2 = ans1 + xn / fact;
        fact *= cnt;
        xn = xn * x;
        cnt = cnt + (float)1.0;
    } while ((ans1 > ans2 + minimum_acceptable_error) || (ans2 > ans1 + minimum_acceptable_error));
    return ans2;
}

void afe_seperate_int_and_decimal(float input, float *int_part, float *decimal_part)
{
    int b = 0;
    float c = 0;
    b = (int)input;
    c = input - (float)b;

    *int_part = (float)b;
    *decimal_part = c;
}

// e^x fast function
float afe_exp_fast_function(const float exponment, const float minimum_acceptable_error)
{
    const float Euler = 2.718281828459045;
    //float rst=1.0;
    float p1 = 0, p2 = 0;
    afe_seperate_int_and_decimal(exponment, &p1, &p2);

    if (exponment > (float)709.0) {
        p1 = 1.0;
        p2 = 0.0;

        return 0xFFFFFFFF;  // too big not to calculate
    } else if (exponment < (float)(-709.0)) {
        return 0.0;
    } else {
        return afe_exp_power_function(p2, minimum_acceptable_error) * afe_fast_pow_function(Euler, (int)p1);
    }
}

uint32_t afe_calculate_digital_stream_out_gain_index(uint32_t digital_gain_in_01unit_db, uint32_t digital_stream_out_0db_register_value)
{
    uint32_t digital_gain_index = 0;
    int32_t temp_int32_digital_gain = (int32_t)digital_gain_in_01unit_db;
    int32_t temp_int32_digital_gain_register_value = 0;
    float temp_float_digital_gain_register_value = 0;
    float temp_float_digital_gain_in_unit_db = (float)temp_int32_digital_gain / 100;
    float temp_digital_stream_out_0db_register_value = (float)digital_stream_out_0db_register_value;
    float exp_exponment = temp_float_digital_gain_in_unit_db / (float)20 * (float)LN10;
    temp_float_digital_gain_register_value = temp_digital_stream_out_0db_register_value * afe_exp_fast_function(exp_exponment, 1 / temp_digital_stream_out_0db_register_value);
    temp_int32_digital_gain_register_value = (int32_t)temp_float_digital_gain_register_value;
    digital_gain_index = (uint32_t)temp_int32_digital_gain_register_value;

    return digital_gain_index;
}

void afe_audio_set_digital_gain(void)
{
    uint32_t sample_rate = audio_transfer_sample_rate(audio_config.stream_out.stream_sampling_rate);
    speech_path_type_t path = audio_pcm2way_get_path_type();
    if (path == SPH_BOTH_PATH) {
        afe.audio_path_type =  HAL_AUDIO_PLAYBACK_VOICE;
    }
    switch (afe.audio_path_type) {
        case HAL_AUDIO_PLAYBACK_VOICE: {
            if (afe.stream_out_mute == true) {
                *DSP_SPH_DL_VOL = 0;
            } else {
                //[To Do] Need to check range with Kenny
                afe.stream_out_digital_gain_index = afe_calculate_digital_stream_out_gain_index(afe.stream_out_digital_gain_in_01unit_db, (uint32_t)DIGITAL_SPEECH_STREAM_OUT_0DB_REGISTER_VALUE);
                *DSP_SPH_DL_VOL = afe.stream_out_digital_gain_index;
            }
        }
        break;
        case HAL_AUDIO_PLAYBACK_MUSIC:
        case HAL_AUDIO_PLAYBACK_MUSIC_RECORD_VOICE:
        {
            uint16_t dsp_gain;
            if (afe.stream_out_mute == true) {
                dsp_gain = 0;
            } else {
                afe.stream_out_digital_gain_index = afe_calculate_digital_stream_out_gain_index(afe.stream_out_digital_gain_in_01unit_db, (uint32_t)DIGITAL_AUDIO_STREAM_OUT_0DB_REGISTER_VALUE);
                dsp_gain = afe.stream_out_digital_gain_index;
            }
            *DSP_AUDIO_GAIN_STEP = (32767 * 1000 * 6) / (sample_rate * 20) + 1;
            *DSP_AUDIO_ASP_WAV_OUT_GAIN = dsp_gain;
        }
        break;
        default:
            break;
    }
    if (afe.input_device == HAL_AUDIO_DEVICE_MAIN_MIC || afe.input_device == HAL_AUDIO_DEVICE_HEADSET_MIC || afe.input_device == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC || afe.input_device == HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC) {
        afe_audio_set_uplink_gain();
    }
}

void afe_set_stream_out_mute(bool mute)
{
    afe.stream_out_mute = mute;
    afe_audio_device_volume_update();
}

void afe_set_stream_in_mute(bool mute)
{
    afe.stream_in_mute = mute;
    afe_audio_set_uplink_gain();
}

void afe_set_hardware_mute(bool mute)
{
    if (mute) {
        *AFE_AMCU_CON1 |= 0x3C;
        hal_gpt_delay_ms(70);
    } else {
        *AFE_AMCU_CON1 &= ~0x0C;
    }
}

#ifdef HAL_I2S_MODULE_ENABLED
bool afe_check_if_need_to_turn_on_i2s(void)
{
    return (afe.ext_output_device != HAL_AUDIO_EXT_DEVICE_NONE || i2s_is_legecy_in_used() || (*DSP_SPH_8K_CTRL & (1 << 5)));
}
#endif
void afe_switch_tx_signal_path(bool on)
{
    if (on) {
#ifdef HAL_I2S_MODULE_ENABLED
        /* output via external component */
        if (afe_check_if_need_to_turn_on_i2s()) {
            afe_set_and_swtich_master_i2s(true);
            if (afe.ext_output_device & HAL_AUDIO_EXT_DEVICE_I2S_OUT) {
                afe_audio_data_1x_to_edi_output(true);
            }
            if (afe.ext_output_device & HAL_AUDIO_EXT_DEVICE_I2S_IN) {
                afe_audio_data_from_edi_input(true);    //to disable stream out to audio filter
            }
        } else {
#ifdef HAL_I2S_DSP_DECODE_CONCURRENT_ENABLED
            if (i2s_is_dsp_decode_concurrency() == false)
#endif
            {
                afe_set_and_swtich_master_i2s(false);
                afe_audio_data_1x_to_edi_output(false);
                afe_audio_data_from_edi_input(false);
            }
        }
#endif /* HAL_I2S_MODULE_ENABLED */
        if (afe.output_device == HAL_AUDIO_DEVICE_NONE) {
            afe_switch_no_voice_audio_amp();
            afe_audio_device_volume_update();
            return;
        }
        if (afe.output_device == HAL_AUDIO_DEVICE_HANDS_FREE_STEREO) {
            *AFE_AMCU_CON1 |= 0x4000;   //mono mode: (left+right)/2
        } else {
            *AFE_AMCU_CON1 &= (~0x4000);
        }

        if (!afe.audio_tx_enable) {
            if (afe.output_device == HAL_AUDIO_DEVICE_HEADSET || afe.output_device == HAL_AUDIO_DEVICE_HEADSET_MONO || afe.output_device == HAL_AUDIO_DEVICE_LINE_IN) {
                afe_audio_switch_audio_amp(true);
                afe.audio_tx_enable = true;
            } else if (afe.output_device == HAL_AUDIO_DEVICE_HANDSET || afe.output_device == HAL_AUDIO_DEVICE_HANDS_FREE_MONO || afe.output_device == HAL_AUDIO_DEVICE_HANDS_FREE_STEREO) {
                afe_audio_switch_voice_amp(true);
                afe.audio_tx_enable = true;
            }
        } else { //hp and hs switch during tx enable
            bool hp_on ;
            if (afe.output_device == HAL_AUDIO_DEVICE_HEADSET || afe.output_device == HAL_AUDIO_DEVICE_HEADSET_MONO) {
                hp_on = true;
            } else {
                hp_on = false;
            }
            if (afe.handset_on) {
                if (hp_on) {
                    afe_audio_switch_voice_amp(false);
                    if (afe.ext_pa_on) {
                        afe_switch_external_amp(false);
                    }
                    afe_audio_switch_audio_amp(true);
                }
            } else if (afe.headphone_on) {
                if (!hp_on) {
                    afe_audio_switch_audio_amp(false);
                    afe_audio_switch_voice_amp(true);
                }
#ifdef __LINE_IN_SUPPORT__
                else if (hp_on && (afe.input_device == HAL_AUDIO_DEVICE_LINE_IN) && (!afe.line_in_enable)) {
                    afe_audio_switch_audio_amp(false);  //turn off HP first
                    afe_audio_switch_audio_amp(true);   //then HP line-in on
                }
#endif
            } else if (!afe.handset_on && !afe.headphone_on) {
                if (hp_on) {
                    afe_audio_switch_audio_amp(true);
                } else {
                    afe_audio_switch_voice_amp(true);
                }
            }
        }
        if (!afe.ext_pa_on && (afe.output_device == HAL_AUDIO_DEVICE_HANDS_FREE_MONO || afe.output_device == HAL_AUDIO_DEVICE_HANDS_FREE_STEREO)) {
            afe_switch_external_amp(true);
        }
        afe_set_hardware_mute(false);
#ifdef HAL_I2S_MODULE_ENABLED
    } else {
        /* disable extended output path of master I2S */
        afe_set_and_swtich_master_i2s(false);
        afe_audio_data_1x_to_edi_output(false);
        afe_audio_data_from_edi_input(false);
#endif
    }
}

void afe_audio_set_digital_mic(bool enable)
{
    if (enable) {
        if (afe.input_device == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) {
            *DSP_SPH_DUMIC_CTRL |= 0x0001;
        }
        *AFE_VMCU_CON1      |= 0x1000;     /*digital afe turn on dual mic*/
        *AFE_VMCU_CON5      |= 0x7c11;     /*enable 3.25M sample rate*/
        if (AUDIO_GET_CHIP_ID() == 0) {    /*E1 chip*/
            *ABBA_AUDIODL_CON4  |= 0x0001;
        } else {
            *ABBA_TOP_CON0    |= 0x0200; /*E2 chip*/
        }
        *ABBA_VBITX_CON0    |= 0x0400;
        /*active or power down MICBIAS*/
        if (!afe.mic_bias) {
            *ABBA_VBITX_CON4 &= ~(RG_VPWDB_MBIAS);
        } else {
            *ABBA_VBITX_CON4 |= (RG_VPWDB_MBIAS);
        }
        *ABBA_VBITX_CON7 |= (RG_AUDDIGMICEN);
    } else {
        if (afe.input_device == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) {
            *DSP_SPH_DUMIC_CTRL &= ~0x0001;
        }
        *AFE_VMCU_CON1      &= ~0x1000;     /*digital afe turn off dual mic*/
        *AFE_VMCU_CON5      &= ~0x7c11;     /*disable 3.225M sample rate*/
        if (AUDIO_GET_CHIP_ID() == 0) {     /*E1 chip*/
            *ABBA_AUDIODL_CON4  &= ~0x0001;
        } else {
            *ABBA_TOP_CON0    &= ~0x0200; /*E2 chip*/
        }
        *ABBA_VBITX_CON0    &= ~0x0400;
        *ABBA_VBITX_CON4    &= ~(RG_VPWDB_MBIAS);
        *ABBA_VBITX_CON7    &= ~(RG_AUDDIGMICEN);
    }
}

void afe_switch_rx_signal_path(void)
{
    if (afe.input_device == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC || afe.input_device == HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC) { /*Digital mic*/
        afe_audio_set_digital_mic(true);
    } else if (afe.input_device == HAL_AUDIO_DEVICE_MAIN_MIC) {
        if (mic0_type_sel == 0) {
            *ABBA_VBITX_CON0 &= 0xFFEF;
        } else {
            *ABBA_VBITX_CON0 |= 0x0010;
        }
        *ABBA_VBITX_CON0 = (*ABBA_VBITX_CON0 & MIC_SETTING_MASK) | MIC_SETTING_NORMAL;  /* Switch input source to normal microphone */
    } else if (afe.input_device == HAL_AUDIO_DEVICE_HEADSET_MIC) {
        if (mic1_type_sel == 0) {
            *ABBA_VBITX_CON0 &= 0xFFEF;
        } else {
            *ABBA_VBITX_CON0 |= 0x0010;
        }
        *ABBA_VBITX_CON0  = (*ABBA_VBITX_CON0 & MIC_SETTING_MASK) | MIC_SETTING_HEADSET; /* Switch input source to headset microphone */
    } else {
        /*HAL_AUDIO_DEVICE_NONE: no input device*/
    }
}

void afe_switch_no_voice_audio_amp(void)
{
    afe_set_hardware_mute(true);
#ifdef __AFE_HP_ANALOG_DEPOP_SWITCH__
    if (afe.init) {
        afe_hp_buffer_sw(false);
        hal_gpt_delay_ms(5);
    }
#endif
    afe_volume_ramp_down();
    if (afe.audio_tx_enable) {
        if (afe.headphone_on) {
            afe_audio_switch_audio_amp(false);
        }
        if (afe.handset_on) {
            afe_audio_switch_voice_amp(false);
        }
        if ((!afe.audio_clock_enable) || (!afe.voice_clock_enable)) {
            *ABBA_AUDIODL_CON0  &= (~(RG_AUDDACRPWRUP | RG_AUDDACLPWRUP));
            *ABBA_AUDIODL_CON7  &= (~0x0003);
            *ABBA_AUDIODL_CON11 &= (~0x0003);
        }
        *AFE_AMCU_CON1 &= (~0x4000);
        afe.audio_tx_enable = false;
    }
    *ABBA_AUDIODL_CON16 |= (RG_AMUTER | RG_AMUTEL); // mute R and L at digital
    if (afe.ext_pa_on && (afe.output_device == HAL_AUDIO_DEVICE_NONE || afe.output_device == HAL_AUDIO_DEVICE_HANDS_FREE_MONO || afe.output_device == HAL_AUDIO_DEVICE_HANDS_FREE_STEREO)) {
        afe_switch_external_amp(false);
    }
    afe_set_hardware_mute(false);
}

bool afe_is_audio_enable(void)
{
    return (afe.audio_clock_enable || afe.voice_clock_enable);
}

void afe_audio_analog_device_off(void)
{
    afe_switch_no_voice_audio_amp();
    if (afe.input_device == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC || afe.input_device == HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC) {
        afe_audio_set_digital_mic(false);
    } else {
        afe_mic_power_setting();
    }
}

void afe_audio_switch_voice_amp(bool on)
{
    if (on) {
        uint16_t restore;
#ifdef __AFE_HS_DC_CALIBRATION__
        //enable compensation
        {
            *AFE_AMCU_CON2 |= 0x8000;
            *AFE_VMCU_CON2 |= 0x8000;
            *AFE_AMCU_CON6 = afe.dc_cali_data.hs_dc_compensate_value;
            *AFE_AMCU_CON7 = afe.dc_cali_data.hs_dc_compensate_value;
            *AFE_VMCU_CON4 = afe.dc_cali_data.hs_dc_compensate_value;
        }
#endif /*__AFE_HS_DC_CALIBRATION__*/
        *ABBA_AUDIODL_CON16 &= (~(RG_AMUTER | RG_AMUTEL)); // clear again to avoid mute
        *ABBA_AUDIODL_CON11 |= 0x0003;
        *ABBA_AUDIODL_CON7  |= 0x0003;
        *ABBA_AUDIODL_CON0 |= (RG_AUDDACRPWRUP | RG_AUDDACLPWRUP);
        *ABBA_AUDIODL_CON1 &= (~0x6000);
        *ABBA_AUDIODL_CON1 |= 0x6000;  // enable HS zcd
        restore =  *ABBA_AUDIODL_CON13;
        *ABBA_AUDIODL_CON13 = (restore & ~0x7C00) | 0x2000; // set to 0dB and enable zcd
        *ABBA_AUDIODL_CON2 |= 0x0001;
        *ABBA_AUDIODL_CON1 |= 0x0004;
        *ABBA_AUDIODL_CON12 |= (audzcdenable);
        *ABBA_AUDIODL_CON1 &= (~0x1FF0);
        *ABBA_AUDIODL_CON1 |= 0x0800;
        *ABBA_AUDIODL_CON0 |= RG_AUDHSPWRUP; //HS buffer power on
        hal_gpt_delay_us(25);
        *ABBA_AUDIODL_CON2 &= (~0x0001);
        *ABBA_AUDIODL_CON1 &= (~0x0004);
        *ABBA_AUDIODL_CON13 = restore;
        if (afe.hp_hs_both_on) { /* force to turn on earphone under spk*/
            *ABBA_AUDIODL_CON2 |= 0x0001;
            *ABBA_AUDIODL_CON0 |= 0x000D;
            hal_gpt_delay_us(1);
            *ABBA_AUDIODL_CON2 &= (~0x0001);
            *ABBA_AUDIODL_CON1 |= 0x0090;
        }
        afe.handset_on = true;
    } else {
        uint16_t origin_value = *ABBA_AUDIODL_CON13;
        *ABBA_AUDIODL_CON12 &= (~audzcdenable);
        *ABBA_AUDIODL_CON13  = (MINIMUM_VGAIN_SETTING << 10) | (origin_value & 0x83FF);
        *ABBA_AUDIODL_CON0  &= (~RG_AUDHSPWRUP);
        *ABBA_AUDIODL_CON1  &= (~0x1C00);
        /*force to turn off earphone under spk*/
        if (afe.hp_hs_both_on) {
            *ABBA_AUDIODL_CON0 &= (~0x000D);
            *ABBA_AUDIODL_CON1 &= (~0x0090);
        }
#ifdef __AFE_HS_DC_CALIBRATION__
        //disable compensation
        *AFE_AMCU_CON2 &= ~0x8000;
        *AFE_VMCU_CON2 &= ~0x8000;
#endif /*__AFE_HS_DC_CALIBRATION__*/
        afe.handset_on = false;
    }
}

void afe_audio_switch_audio_amp(bool on)
{
    if (on) {
        *ABBA_AUDIODL_CON16 &= (~(RG_AMUTER | RG_AMUTEL)); // clear again to avoid mute
#ifdef __LINE_IN_SUPPORT__
        if ((!afe.line_in_enable) && (afe.input_device == HAL_AUDIO_DEVICE_LINE_IN)) {
            *ABBA_AUDIODL_CON11 |= 0x0003;
            *ABBA_AUDIODL_CON0 |= RG_AUDHSPWRUP; //HS buffer power on [must]
            hal_gpt_delay_us(10);
            *ABBA_AUDIODL_CON5 |= 0x0001;
            hal_gpt_delay_us(10);
            *ABBA_AUDIODL_CON0 |= 0x0001;
            hal_gpt_delay_us(10);
            *ABBA_AUDIODL_CON2 |= 0x0001;
            *ABBA_AUDIODL_CON0  = 0x001D;
            *ABBA_AUDIODL_CON2 &= (~0x0001);
            hal_gpt_delay_us(5);
            *ABBA_AUDIODL_CON5 &= (~0x0001);
            *ABBA_AUDIODL_CON0  = 0x000D;
            *ABBA_AUDIODL_CON1 |= 0x4240;
            *ABBA_AUDIODL_CON4  = 0x0139;
            *ABBA_AUDIODL_CON10 |= 0x0010;
            afe.line_in_enable = true;
        } else
#endif
        {
            uint16_t restore, tmp;
#ifdef __AFE_HP_ANALOG_DEPOP_SWITCH__
            if (afe.init) {
                afe_hp_buffer_sw(false);
            }
#endif
            *ABBA_AUDIODL_CON11 |= 0x0003;
            *ABBA_AUDIODL_CON7  |= 0x0003;
            *ABBA_AUDIODL_CON0  |= (RG_AUDDACRPWRUP | RG_AUDDACLPWRUP);
            *ABBA_AUDIODL_CON1  &= (~0x6000);
            *ABBA_AUDIODL_CON1  |= 0x4000;  // enable HP zcd
            restore = *ABBA_AUDIODL_CON13;
            *ABBA_AUDIODL_CON13 = (restore & ~0x03FF) | 0x02F7;  // set to 0dB and enable zcd
            *ABBA_AUDIODL_CON12 |= (audzcdenable);
#ifdef __AFE_HP_DC_CALIBRATION__
            //enable compensation
            {
                *AFE_AMCU_CON2 |= 0x8000;
                *AFE_VMCU_CON2 |= 0x8000;
                *AFE_AMCU_CON6 = afe.dc_cali_data.hp_dc_compensate_value_r;
                *AFE_AMCU_CON7 = afe.dc_cali_data.hp_dc_compensate_value_l;
                *AFE_VMCU_CON4 = afe.dc_cali_data.hp_dc_compensate_value_l;
            }
#endif /*__AFE_HP_DC_CALIBRATION__*/
            *ABBA_AUDIODL_CON1 |= 0x0004;
            *ABBA_AUDIODL_CON0 |= RG_AUDHSPWRUP; //HS buffer power on [must]
            hal_gpt_delay_us(10);
            *ABBA_AUDIODL_CON5 |= 0x0001;
            hal_gpt_delay_us(10);
            *ABBA_AUDIODL_CON0 |= 0x0001;
            hal_gpt_delay_us(10);
            *ABBA_AUDIODL_CON2 |= 0x0001;
            *ABBA_AUDIODL_CON0 |= 0x000C;
            *ABBA_AUDIODL_CON6 |= 0x0004;
            hal_gpt_delay_us(5);
            *ABBA_AUDIODL_CON2 &= (~0x0001);
            hal_gpt_delay_us(5);
            tmp = *ABBA_AUDIODL_CON1 & (~0x1FF0); // to avoid set path pop
            *ABBA_AUDIODL_CON1 = tmp | 0x0120;
            *ABBA_AUDIODL_CON5 &= (~0x0001);
            *ABBA_AUDIODL_CON0 &= (~RG_AUDHSPWRUP);
            *ABBA_AUDIODL_CON1 &= (~0x0004);
            *ABBA_AUDIODL_CON4 &= (~(RG_ADEPOP_EN | RG_DEPOP_VCM_EN));
            *ABBA_AUDIODL_CON13 = restore;
#ifdef __AFE_HP_ANALOG_DEPOP_SWITCH__
            if (afe.init) {
                hal_gpt_delay_ms(60);
                afe_hp_buffer_sw(true);
            }
#endif
        }
        afe.headphone_on = true;
    } else {
#ifdef __LINE_IN_SUPPORT__
        if (afe.line_in_enable) {
            *ABBA_AUDIODL_CON0  &= (~0x000D);
            hal_gpt_delay_us(10);
            *ABBA_AUDIODL_CON1  &= (~0x03F0);
            *ABBA_AUDIODL_CON4  |= (RG_DEPOP_VCM_EN);
            hal_gpt_delay_us(10);
            *ABBA_AUDIODL_CON4  |= (RG_ADEPOP_EN);
            hal_gpt_delay_us(10);
            *ABBA_AUDIODL_CON10 &= (~0x0010);
            afe.line_in_enable =  false;
        } else
#endif
        {
            uint16_t origin_value = *ABBA_AUDIODL_CON13;
            *ABBA_AUDIODL_CON4  |= (RG_DEPOP_VCM_EN);
            hal_gpt_delay_us(10);
            *ABBA_AUDIODL_CON4  |= (RG_ADEPOP_EN);
            hal_gpt_delay_us(10);
            *ABBA_AUDIODL_CON12 &= (~audzcdenable);
            *ABBA_AUDIODL_CON13  = (MINIMUM_AGAIN_SETTING << 1) | (MINIMUM_AGAIN_SETTING << 6) | (origin_value & 0xFC21);
            *ABBA_AUDIODL_CON0  &= (~(RG_AUDIBIASPWRUP | RG_AUDHPRPWRUP | RG_AUDHPLPWRUP));
            *ABBA_AUDIODL_CON1  &= (~0x03F0);
            *ABBA_AUDIODL_CON6 &= (~0x0004);
#ifdef __AFE_HP_DC_CALIBRATION__
            //disable compensation
            *AFE_AMCU_CON2 &= ~0x8000;
            *AFE_VMCU_CON2 &= ~0x8000;
#endif /*__AFE_HP_DC_CALIBRATION__*/
        }
        afe.headphone_on = false;
    }
}

void afe_volume_ramp_down(void)
{
    int32_t I;
    int16_t i2Gain;
    uint16_t OrgValue;

    i2Gain = (int16_t)(*ABBA_AUDIODL_CON13 & ~0xFFE1); //R gain = L gain ; extract one
    i2Gain = i2Gain >> 1; //LSB ignore
    OrgValue = *ABBA_AUDIODL_CON13 & 0xFC21;             //get original AUDIODL_CON13 setting

    for (I = i2Gain; I <= MINIMUM_AGAIN_SETTING; I++) {
        *ABBA_AUDIODL_CON13 = (I << 1) | (I << 6) | (OrgValue & 0xFC21); //R mod_gain | L mod_gain | original
        hal_gpt_delay_us(500);
    }

    i2Gain = (int16_t)((*ABBA_AUDIODL_CON13 & (~0x83FF)) >> 10);
    OrgValue = *ABBA_AUDIODL_CON13 & 0x83FF;

    if ((!afe.loopback) && (!afe.pcmif_loopback)) {
        for (I = i2Gain; I <= MINIMUM_VGAIN_SETTING; I++) { // ramp down to 0dB
            *ABBA_AUDIODL_CON13 = (I << 10) | (OrgValue & 0x83FF);
            hal_gpt_delay_us(500);
        }
    }
}

#ifdef HAL_I2S_MODULE_ENABLED
void afe_audio_set_ext_output_device(hal_audio_ext_device_t device)
{
    afe.ext_output_device = device;
}

hal_audio_ext_device_t afe_audio_get_ext_output_device(void)
{
    return afe.ext_output_device;
}
#endif

void afe_audio_set_output_device(uint16_t device)
{
    afe.output_device = (hal_audio_device_t)device;
}

void afe_audio_set_input_device(uint16_t device)
{
    afe.input_device = (hal_audio_device_t)device;
}

uint16_t afe_audio_get_output_device(void)
{
    return afe.output_device;
}

uint16_t afe_audio_get_input_device(void)
{
    return afe.input_device;
}

/*PCM interface functions*/
void afe_digital_switch_loopback_pcmif(bool on)
{
    if (on) {
        *AFE_VDB_CON |= 0x1000;
    } else {
        *AFE_VDB_CON &= (~0x1000);
    }
    afe.pcmif_loopback = on;
}

void afe_digital_switch_internal_loopback_pcmif(bool on)
{
    if (on) {
        *AFE_VDB_CON |= 0x0400;
    } else {
        *AFE_VDB_CON &= (~0x0400);
    }
}

void afe_digital_switch_pcm_clock(bool pcm_on, bool frame_sync, uint16_t pcm_clk_mode) //need to turn on VMC power first
{
    if (pcm_on) {
        *AFE_VDB_CON = (*AFE_VDB_CON & ~0xC01F) | 0x0010;   // clear bits, Turn VBT on
        *AFE_VDB_CON = (*AFE_VDB_CON & (~0xC000)) | (pcm_clk_mode << 14); // set pcm clk
        *AFE_VDB_CON = (*AFE_VDB_CON & (~0x0008)) | ((uint8_t)frame_sync << 3); // set long/short sync
        if (frame_sync) {
            *AFE_VDB_CON |= 0x0007; //fix VBTSLEN = 7 now.
        }
    } else {
        *AFE_VDB_CON &= (~0xC01F);
    }
}

#if defined(HAL_I2S_MODULE_ENABLED)
/* I2S functions */
//dir: 0:in, 1:out
void afe_master_i2s_edi_on()
{
    //[1]: EDI format 0:EIAJ/1: I2S, [0]: Enable EDI. [15]: output [8]:input
    *AFE_EDI_CON |= 0x8103;
}

void afe_master_i2s_edi_off()
{
    *AFE_EDI_CON &= ~0x8103;
    *AFE_VAM_SET &= ~0x07;
    *AFE_VAM_SET |= 0x05;
}
void afe_set_slave_i2s_config(hal_i2s_sample_rate_t sample_rate)
{
    *AFE_SLV_I2S_CON |= 0x3;
    //set sample rate, AFE_SLV_I2S_CON [15:12]: only 8KHz and 16KHz are useful. 0000: 8-KHz, 0100: 16-KHz
    switch (sample_rate) {
        case 0:     //HAL_I2S_SAMPLE_RATE_8K
            *AFE_SLV_I2S_CON &= ~(0xF << 12);
            break;
        case 3:     //HAL_I2S_SAMPLE_RATE_16K
            *AFE_SLV_I2S_CON &= ~(0xF << 12);
            *AFE_SLV_I2S_CON |= (0x4 << 12);
            break;
        default:
            break;
    }
    *AFE_FOC_TX_CON0 |= 0x0004;    //enable PTR_TRACK_EN
    *AFE_FOC_RX_CON0 |= 0x0004;    //enable PTR_TRACK_EN
}

void afe_reset_slave_i2s_config()
{
    *AFE_FOC_TX_CON0 &= ~0x0004;    //disable PTR_TRACK_EN
    *AFE_FOC_RX_CON0 &= ~0x0004;    //disable PTR_TRACK_EN

    *AFE_SLV_I2S_CON &= ~0x3;
}

void afe_slave_i2s_enable(void)
{
    *AFE_SLV_I2S_CON |= 0xC0000000;
    *AFE_FOC_TX_CON0 |= 0x0001;
    *AFE_FOC_RX_CON0 |= 0x0001;
}

void afe_slave_i2s_disable()
{
    *AFE_FOC_TX_CON0 &= ~0x0001;
    *AFE_FOC_RX_CON0 &= ~0x0001;
    *AFE_SLV_I2S_CON &= ~0xC0000000;
}

bool afe_set_and_swtich_master_i2s(bool enable)
{
    if (enable && !afe.edi_on) {
        afe_i2s_pinmux_switch(true);
        afe_master_i2s_edi_on();
        afe.edi_on = true;
    } else if (!enable) {
        afe_master_i2s_edi_off();
        afe_i2s_pinmux_switch(false);
        afe.edi_on = false;
    }
    return afe.edi_on;
}

void afe_audio_data_from_edi_input(bool enable)
{
    if (enable) {
        *AFE_AMCU_CON2 |= (1 << 6);
    } else {
        *AFE_AMCU_CON2 &= ~(1 << 6);
    }
}

void afe_audio_data_1x_to_edi_output(bool enable)
{
    *AFE_EDI_CON &= ~(1 << 10);
    if (enable) {
        *AFE_AMCU_CON1 |= (1 << 12);       //data from dsp FIFO without passing ASP
    } else {
        *AFE_AMCU_CON1 &= ~(1 << 12);
    }
}

void afe_master_i2s_internal_loopback(bool enable)
{
    if (enable) {
        *AFE_EDI_CON |= (1 << 9);
    } else {
        *AFE_EDI_CON &= ~(1 << 9);
    }
}
#endif /*HAL_I2S_MODULE_ENABLED*/

#if defined(__AFE_HP_DC_CALIBRATION__) || defined(__AFE_HS_DC_CALIBRATION__)
static int32_t afe_sum_auxadc_value(uint16_t *p_buf, int32_t count)
{
    int32_t total_sum = 0;
    if (count > 0) {
        int32_t i;
        for (i = 0; i < count; i++) {
            total_sum += (int32_t)(*p_buf++);
        }
    }
    return total_sum;
}

#endif /*defined(__AFE_HP_DC_CALIBRATION__) || defined(__AFE_HS_DC_CALIBRATION__)*/

#ifdef __AFE_HS_DC_CALIBRATION__

//------------------- Current only HS DC CALI use this. Can be reuse with HP DC CALI.
/*Get nvdm data. Be called at nvdm init*/
void afe_nvdm_set_data(afe_dc_calibration_t nvdm_dc_calibration)
{
    afe.dc_cali_data.calibration_sta = nvdm_dc_calibration.calibration_sta;
    afe.dc_cali_data.hs_dc_compensate_value = nvdm_dc_calibration.hs_dc_compensate_value;
}

/*Be called after hal_audio_init to save cali data to nvdm*/
afe_dc_calibration_t afe_get_dc_calibration_data(void)
{
    return afe.dc_cali_data;
}
//---------------------------


static void afe_get_auxadc_data_raw(uint16_t channel, uint16_t *data)
{
    //enable immdiate mode of channel 7&channel8
    *AUXADC_CON3  |= 0x8010;
    *AUXADC_CON1  |= ((1 << 7) | (1 << 8));
    for (int i = 0; i < DC_CALI_BUF_LEN; i++) {
        //wait until idle
        while (*AUXADC_CON3 & AUXADC_CON3_AUXADC_STA_MASK);
        if (channel == 7) {
            *data++ = *AUXADC_DAT7;
        } else {
            *data++ = *AUXADC_DAT8;
        }
    }
}

void afe_voice_buffer_dc_calibration(void)
{
    uint16_t adc_data[DC_CALI_BUF_LEN];
    int32_t temp_value;
    int32_t iteration;
    int32_t channel_index;
    int32_t audadc_channel;
    int32_t  dc_comp_val_temp = 0;
    uint16_t dc_comp_val = 0;
    int32_t sum_auxadc_value[2] = {0};

    afe_digital_switch_digital_clock(true, true);
    /* Generate silence*/
    *AFE_AMCU_CON1 |= 0x003c;   /*Mute*/
    *AFE_DAC_TEST  |= 0x6000;   /*Generate audio sine tone*/
    *AFE_AMCU_CON1 &= ~0x4000;  /*Disable mono mode*/
    //auxadc
    hal_adc_init();

    /*HS on*/
    *ABBA_AUDIODL_CON13 = (*ABBA_AUDIODL_CON13 & ~0x07FF) | 0x24FF;
    afe_audio_switch_voice_amp(true);
    for (iteration = 0; iteration < HP_CALI_TIMES; iteration++) {
        for (channel_index = 0; channel_index < HP_CALI_CH; channel_index++) {
            if (channel_index == 0) {
                audadc_channel = 7; //L
            } else {
                audadc_channel = 8; //R
            }
            afe_get_auxadc_data_raw(audadc_channel, adc_data);
            temp_value = afe_sum_auxadc_value(adc_data, DC_CALI_BUF_LEN);
            if (iteration == 0) { // skip the first 120 auxadc read value
                temp_value = 0;
            }
            sum_auxadc_value[channel_index] += temp_value;
        }
    }
    for (channel_index = 0; channel_index < HP_CALI_CH; channel_index++) {
        int32_t  dc_offset_sum = sum_auxadc_value[0] - sum_auxadc_value[1]; // L - R
        dc_comp_val_temp = (dc_offset_sum * 0x0AF0 / 0x1000) * 2500 / ((HP_CALI_TIMES - 1) * DC_CALI_BUF_LEN); //1 code = 40uV, sample: 120*9
        dc_comp_val  = dc_comp_val_temp / 100; //avoid rounding error too much
        afe.dc_cali_data.hs_dc_compensate_value = dc_comp_val;
    }
    hal_adc_deinit();
    afe_audio_switch_voice_amp(false);
    // Turn off DAC tone
    *AFE_DAC_TEST  &= ~0x6000;
    *AFE_AMCU_CON1 &= ~0x000c;
    // Turn off digital AFE
    afe_digital_switch_digital_clock(false, true);

}

#endif /*#ifdef __AFE_HS_DC_CALIBRATION__*/

#ifdef __AFE_HP_DC_CALIBRATION__

// For SDM gain 60 / 64, audio buffer gain 0dB,
// compensation value 65536 --> DC offset 2000mV
// DC comp value =  DC offset mV * 65536 / 2000
//               = (DC offset mV << 12) / 125
// DC comp value = (DC offset value * 7 << 4) / 5

static uint16_t afe_convert_headphone_dc_offset_value_to_headphone_dc_compensation_value(int32_t dc_offset_sum, int32_t count)
{
    int32_t dc_comp_val;
    int16_t final_val;
    if (dc_offset_sum >= 0) {
        dc_comp_val = (int32_t)(((double)dc_offset_sum * (double)32767) / (double)count / (double)1000);
    } else {
        dc_comp_val = (int32_t)(((double)dc_offset_sum * (double)32768) / (double)count / (double)1000);
    }
    dc_comp_val = BOUNDED(dc_comp_val, 32767, -32768);
    final_val = (int16_t)dc_comp_val;
    return (uint16_t)final_val;
}

static void afe_get_auxadc_data(uint16_t channel, uint16_t *data)
{
    //enable immdiate mode of channel 7&channel8
    *AUXADC_CON3  |= 0x8010;
    *AUXADC_CON1  |= ((1 << 7) | (1 << 8));
    for (int i = 0; i < DC_CALI_BUF_LEN; i++) {
        //wait until idle
        while (*AUXADC_CON3 & AUXADC_CON3_AUXADC_STA_MASK);
        if (channel == 7) {
            *data++ = *AUXADC_DAT7 * 0x0AF0 / 0x1000;
        } else {
            *data++ = *AUXADC_DAT8 * 0x0AF0 / 0x1000;
        }
    }
}

void afe_audio_buffer_dc_calibration(void)
{
    uint16_t adc_data[DC_CALI_BUF_LEN];
    int32_t temp_value;
    int32_t switch_index;
    int32_t iteration;
    int32_t channel_index;
    int32_t audadc_channel;
    int32_t sum_auxadc_value[HP_CALI_SWITCH][HP_CALI_CH] = {{0}, {0}};
    afe_digital_switch_digital_clock(true, true);
    /* Generate silence*/
    *AFE_AMCU_CON1 |= 0x003c;   /*Mute*/
    *AFE_DAC_TEST  |= 0x6000;   /*Generate audio sine tone*/
    *AFE_AMCU_CON1 &= ~0x4000;  /*Disable mono mode*/

    //auxadc
    hal_adc_init();
    for (switch_index = 0; switch_index < HP_CALI_SWITCH; switch_index++) {
        if (switch_index == 0) {
            /*HP off if already off, do nothing*/
            uint32_t i = 0;
            while (i < HP_CHARGE_DELAY) {
                hal_gpt_delay_us(1000);
                i++;
            }
        } else {
            /*HP on*/
            *ABBA_AUDIODL_CON13 = (*ABBA_AUDIODL_CON13 & ~0x03FF) | 0x02F7;  // Default 0dB => [4:0] = 10111'b for L, [9:5] = 10111'b for R
            afe_audio_switch_audio_amp(true);
        }

        for (iteration = 0; iteration < HP_CALI_TIMES; iteration++) {
            for (channel_index = 0; channel_index < HP_CALI_CH; channel_index++) {
                if (channel_index == 0) {
                    audadc_channel = 7; //L
                } else {
                    audadc_channel = 8; //R
                }
                afe_get_auxadc_data(audadc_channel, adc_data);
                temp_value = afe_sum_auxadc_value(adc_data, DC_CALI_BUF_LEN);
                sum_auxadc_value[switch_index][channel_index] += temp_value;
            }
            hal_gpt_delay_us(1000);
        }
    }
    for (channel_index = 0; channel_index < HP_CALI_CH; channel_index++) {
        int32_t  dc_offset_sum = sum_auxadc_value[1][channel_index] - sum_auxadc_value[0][channel_index]; // on - off
        uint16_t dc_comp_val   = afe_convert_headphone_dc_offset_value_to_headphone_dc_compensation_value(dc_offset_sum, HP_CALI_TIMES * DC_CALI_BUF_LEN);
        if (channel_index == 0) {
            afe.dc_cali_data.hp_dc_compensate_value_l = dc_comp_val;
        } else {
            afe.dc_cali_data.hp_dc_compensate_value_r = dc_comp_val;
        }
    }
    hal_adc_deinit();
    afe_audio_switch_audio_amp(false);
    // Turn off DAC tone
    *AFE_DAC_TEST  &= ~0x6000;
    *AFE_AMCU_CON1 &= ~0x000c;
    // Turn off digital AFE
    afe_digital_switch_digital_clock(false, true);
}

#endif /*__AFE_HP_DC_CALIBRATION__*/

#endif  /* defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED) */
