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

#include "hal_pmu_auxadc.h"

#ifdef HAL_PMU_AUXADC_MODULE_ENABLED
#ifndef MTK_EXTERNAL_PMIC
#include "hal_pmu.h"
#include "hal_gpt.h"
#include "hal_log.h"

void pmu_auxadc_delay(unsigned int ms);
unsigned short int aux_get_adc_result(unsigned int ADC_OUT_L_Addr);
unsigned int pmu_auxadc_get_isense(void);

void pmu_auxadc_init(void)
{
    unsigned char AUXADC_SW_GAIN_TRIM_MSB, AUXADC_SW_OFFSET_TRIM_MSB;

    AUXADC_SW_GAIN_TRIM_MSB = pmu_get_register_value(PMU_AUXADC_SW_GAIN_TRIM_MSB_ADDR  , PMU_AUXADC_SW_GAIN_TRIM_MSB_MASK    , PMU_AUXADC_SW_GAIN_TRIM_MSB_SHIFT);
    if ((AUXADC_SW_GAIN_TRIM_MSB & 0x08) == 0x08) {
        AUXADC_SW_GAIN_TRIM_MSB |= 0x70;
    } else {
        AUXADC_SW_GAIN_TRIM_MSB &= 0x8F;
    }
    pmu_set_register_value(PMU_AUXADC_SW_GAIN_TRIM_MSB_ADDR  , PMU_AUXADC_SW_GAIN_TRIM_MSB_MASK    , PMU_AUXADC_SW_GAIN_TRIM_MSB_SHIFT , AUXADC_SW_GAIN_TRIM_MSB);

    AUXADC_SW_OFFSET_TRIM_MSB = pmu_get_register_value(PMU_AUXADC_SW_OFFSET_TRIM_MSB_ADDR  , PMU_AUXADC_SW_OFFSET_TRIM_MSB_MASK    , PMU_AUXADC_SW_OFFSET_TRIM_MSB_SHIFT);
    if ((AUXADC_SW_OFFSET_TRIM_MSB & 0x4) == 0x04) {
        AUXADC_SW_OFFSET_TRIM_MSB |= 0x78;
    } else {
        AUXADC_SW_OFFSET_TRIM_MSB &= 0x87;
    }
    pmu_set_register_value(PMU_AUXADC_SW_OFFSET_TRIM_MSB_ADDR  , PMU_AUXADC_SW_OFFSET_TRIM_MSB_MASK    , PMU_AUXADC_SW_OFFSET_TRIM_MSB_SHIFT , AUXADC_SW_OFFSET_TRIM_MSB);
    return;
}

#define VOLTAGE_FULL_RANGE     	1800
unsigned int pmu_auxadc_get_channel_value(pmu_adc_channel_t dwChannel, int deCount, int trimd)
{
    unsigned char Channel = (unsigned char)dwChannel;
    unsigned short int  adc_result;
    int temperature;

    if (Channel >= PMU_AUX_MAX) {
        return (0xFFFFFFFF);    //Channel error
    }

    switch (Channel) {
        case PMU_AUX_BATSNS:    //CH0
            pmu_set_register_value(PMU_AUXADC_RQST_CH0_ADDR  , PMU_AUXADC_RQST_CH0_MASK    , PMU_AUXADC_RQST_CH0_SHIFT   , 1);
            pmu_auxadc_delay(1);
            adc_result = aux_get_adc_result(PMU_AUXADC_ADC_OUT_CH0_L_ADDR);
            break;

        case PMU_AUX_ISENSE:    //CH1
            adc_result = pmu_auxadc_get_isense();
            break;

        case PMU_AUX_VCDT:      //CH2
            pmu_set_register_value(PMU_AUXADC_RQST_CH2_ADDR  , PMU_AUXADC_RQST_CH2_MASK    , PMU_AUXADC_RQST_CH2_SHIFT   , 1);
            pmu_auxadc_delay(1);
            adc_result = aux_get_adc_result(PMU_AUXADC_ADC_OUT_CH2_L_ADDR);
            break;

        case PMU_AUX_BATON:     //CH3
            pmu_set_register_value(PMU_AUXADC_RQST_CH3_ADDR  , PMU_AUXADC_RQST_CH3_MASK    , PMU_AUXADC_RQST_CH3_SHIFT   , 1);
            pmu_auxadc_delay(1);
            adc_result = aux_get_adc_result(PMU_AUXADC_ADC_OUT_CH3_L_ADDR);
            break;

        case PMU_AUX_CH4:     //CH4
            pmu_set_register_value(PMU_AUXADC_RQST_CH4_ADDR  , PMU_AUXADC_RQST_CH4_MASK    , PMU_AUXADC_RQST_CH4_SHIFT   , 1);
            pmu_auxadc_delay(1);
            adc_result = aux_get_adc_result(PMU_AUXADC_ADC_OUT_CH4_L_ADDR);
            break;

        case PMU_AUX_ACCDET:  //CH5
            pmu_set_register_value(PMU_AUXADC_RQST_CH5_ADDR  , PMU_AUXADC_RQST_CH5_MASK    , PMU_AUXADC_RQST_CH5_SHIFT   , 1);
            pmu_auxadc_delay(1);
            adc_result = aux_get_adc_result(PMU_AUXADC_ADC_OUT_CH5_L_ADDR);
            break;

        case PMU_AUX_CH6:     //CH6
            pmu_set_register_value(PMU_AUXADC_RQST_CH6_ADDR  , PMU_AUXADC_RQST_CH6_MASK    , PMU_AUXADC_RQST_CH6_SHIFT   , 1);
            pmu_auxadc_delay(1);
            adc_result = aux_get_adc_result(PMU_AUXADC_ADC_OUT_CH6_L_ADDR);
            break;

        case PMU_AUX_TSX:     //CH7
            pmu_set_register_value(PMU_AUXADC_RQST_CH7_ADDR  , PMU_AUXADC_RQST_CH7_MASK    , PMU_AUXADC_RQST_CH7_SHIFT   , 1);
            pmu_auxadc_delay(1);
            adc_result = aux_get_adc_result(PMU_AUXADC_ADC_OUT_CH7_L_ADDR);
            break;

        case PMU_AUX_CH8:     //CH8
            pmu_set_register_value(PMU_AUXADC_RQST_CH8_ADDR  , PMU_AUXADC_RQST_CH8_MASK    , PMU_AUXADC_RQST_CH8_SHIFT   , 1);
            pmu_auxadc_delay(1);
            adc_result = aux_get_adc_result(PMU_AUXADC_ADC_OUT_CH8_L_ADDR);
            break;

        case PMU_AUX_CH9:     //CH9
            pmu_set_register_value(PMU_AUXADC_RQST_CH9_ADDR  , PMU_AUXADC_RQST_CH9_MASK    , PMU_AUXADC_RQST_CH9_SHIFT   , 1);
            pmu_auxadc_delay(1);
            adc_result = aux_get_adc_result(PMU_AUXADC_ADC_OUT_CH9_L_ADDR);
            break;

        case PMU_AUX_CH10:    //CH10
            pmu_set_register_value(PMU_AUXADC_RQST_CH10_ADDR  , PMU_AUXADC_RQST_CH10_MASK    , PMU_AUXADC_RQST_CH10_SHIFT   , 1);
            pmu_auxadc_delay(1);
            adc_result = aux_get_adc_result(PMU_AUXADC_ADC_OUT_CH10_L_ADDR);
            break;

        case PMU_AUX_CH11:    //CH11
            pmu_set_register_value(PMU_AUXADC_RQST_CH11_ADDR  , PMU_AUXADC_RQST_CH11_MASK    , PMU_AUXADC_RQST_CH11_SHIFT   , 1);
            pmu_auxadc_delay(1);
            adc_result = aux_get_adc_result(PMU_AUXADC_ADC_OUT_CH11_L_ADDR);
            break;

        case PMU_AUX_CH12:    //CH12
            pmu_set_register_value(PMU_AUXADC_RQST_CH12_ADDR  , PMU_AUXADC_RQST_CH12_MASK    , PMU_AUXADC_RQST_CH12_SHIFT   , 1);
            pmu_auxadc_delay(1);
            adc_result = aux_get_adc_result(PMU_AUXADC_ADC_OUT_CH12_15_L_ADDR);
            break;

        case PMU_AUX_WAKEUP_PCHR:    //Read WAKEUP_PCHR Only
            adc_result = aux_get_adc_result(PMU_AUXADC_ADC_OUT_WAKEUP_PCHR_L_ADDR);
            break;
    }

    //calculation result
    switch (dwChannel) {
        case PMU_AUX_BATSNS:    //CH0
        case PMU_AUX_ISENSE:    //CH1
            temperature = pmu_get_auxadc_temperature();
            if (pmu_get_thermal_status(0, 0) == 0) {
                adc_result = (adc_result * 3 * VOLTAGE_FULL_RANGE) / 32768;
            } else {
                float adc_vol, calibrate;
                adc_vol = (((float)adc_result * 1800 * 3) / 32768);
                calibrate = (0.37237f * (float)temperature) - 6.f;
                adc_result = (unsigned int)(adc_vol + calibrate);
            }
            break;

        case PMU_AUX_TSX:       //CH7
            adc_result = (adc_result * 1 * VOLTAGE_FULL_RANGE) / 32768;
            break;

        case PMU_AUX_BATON:
            adc_result = (adc_result * 2 * VOLTAGE_FULL_RANGE) / 4096;
            break;

        case PMU_AUX_WAKEUP_PCHR:
            adc_result = (adc_result * 3 * VOLTAGE_FULL_RANGE) / 32768;
            break;

        default:
            adc_result = (adc_result * 1 * VOLTAGE_FULL_RANGE) / 4096;
            break;
    }

    return (adc_result);
}

#define AUXADC_TIMEROUT_MS 15
unsigned short int aux_get_adc_result(unsigned int ADC_OUT_L_Addr)
{
    unsigned int short count = 0, adc_result;
    unsigned char adc_l;
    do {
        adc_result = pmu_get_register_value(ADC_OUT_L_Addr + 1  , 0xFF    , 0);
        if ((adc_result & 0x80) == 0x80) {
            adc_l = pmu_get_register_value(ADC_OUT_L_Addr  , 0xFF    , 0);
            adc_result = ((adc_result << 8) & 0x7F00) | adc_l;
            return (adc_result);
        }
        pmu_auxadc_delay(1);
    } while (count++ <= AUXADC_TIMEROUT_MS);

    //printf timeout error msg
    return (0);
}

void pmu_auxadc_delay(unsigned int ms)
{
    hal_gpt_delay_ms(ms);
}

unsigned int pmu_auxadc_get_isense(void)
{
    static signed int trim_gain_ch7, trim_offset_ch7;
    static char init = 0;
    signed int adc_result;
    unsigned int short ch1_raw;
    unsigned char adc_l;

    if (init == 0) {
        signed short int sint_tmp, msb, lsb;
        init = 1;

        lsb = pmu_get_register_value(PMU_EFUSE_GAIN_CH7_TRIM_LSB_ADDR     , PMU_EFUSE_GAIN_CH7_TRIM_LSB_MASK       , PMU_EFUSE_GAIN_CH7_TRIM_LSB_SHIFT);
        msb = pmu_get_register_value(PMU_EFUSE_GAIN_CH7_TRIM_MSB_ADDR     , PMU_EFUSE_GAIN_CH7_TRIM_MSB_MASK       , PMU_EFUSE_GAIN_CH7_TRIM_MSB_SHIFT);
        sint_tmp = ((msb << 8) & 0xFF00) | lsb;
        if ((sint_tmp & 0x0800) == 0x0800) {
            sint_tmp |= 0xF000;
        }
        trim_gain_ch7 = (signed int)sint_tmp;

        lsb = pmu_get_register_value(PMU_EFUSE_OFFSET_CH7_TRIM_LSB_ADDR   , PMU_EFUSE_OFFSET_CH7_TRIM_LSB_MASK     , PMU_EFUSE_OFFSET_CH7_TRIM_LSB_SHIFT);
        msb = pmu_get_register_value(PMU_EFUSE_OFFSET_CH7_TRIM_MSB_ADDR   , PMU_EFUSE_OFFSET_CH7_TRIM_MSB_MASK     , PMU_EFUSE_OFFSET_CH7_TRIM_MSB_SHIFT);
        sint_tmp = ((msb << 8) & 0xFF00) | lsb;
        if ((sint_tmp & 0x0400) == 0x0400) {
            sint_tmp |= 0xF800;
        }
        trim_offset_ch7 = (signed int)sint_tmp;
    }

    pmu_set_register_value(PMU_AUXADC_RQST_CH1_ADDR  , PMU_AUXADC_RQST_CH1_MASK    , PMU_AUXADC_RQST_CH1_SHIFT   , 1);
    pmu_auxadc_delay(1);
    adc_result = aux_get_adc_result(PMU_AUXADC_ADC_OUT_CH1_L_ADDR);

    ch1_raw = pmu_get_register_value(PMU_AUXADC_ADC_OUT_RAW_H_ADDR  , 0xFF    , 0);
    adc_l = pmu_get_register_value(PMU_AUXADC_ADC_OUT_RAW_L_ADDR  , 0xFF    , 0);
    ch1_raw = ((ch1_raw << 8) & 0x7F00) | adc_l;

    adc_result = (signed int)(((float)ch1_raw) * (1 + (((float)trim_gain_ch7) / 32768)));
    adc_result = adc_result + trim_offset_ch7;
    return ((unsigned int)adc_result);
}

int pmu_get_auxadc_temperature(void)
{
    static char init = 0;
    static int O_VTS;
    static float O_SLOPE, DEGC_CALI;
    float ftmp;
    int result;

    if (init == 0) {
        init = 1;
        unsigned char data[4];
        unsigned int tmp;

        pmu_get_efuse(0x5E, &data[0], &data[1]);
        pmu_get_efuse(0x60, &data[2], &data[3]);

        pmu_get_thermal_status(1, data[0]);

        tmp = (data[0] >> 1) & 0x3F;
        DEGC_CALI = (float)tmp;
        if ((DEGC_CALI < 38) || (DEGC_CALI > 60)) {
            DEGC_CALI = 53;
        }
        DEGC_CALI /= 2;

        O_VTS = data[2];
        O_VTS = ((O_VTS << 8) & 0x1F00) | data[1];

        tmp = (data[3] >> 1) & 0x3F;
        O_SLOPE = (float)tmp;
        if ((data[3] & 0x01) == 1) {
            O_SLOPE *= -1;
        }

        if ((data[3] & 0x80) == 0) {
            O_SLOPE = 0;
        }
        O_SLOPE = (0.001921f + (O_SLOPE / 100000.f)) * -1.f;
    }

    pmu_set_register_value(PMU_AUXADC_RQST_CH4_ADDR  , PMU_AUXADC_RQST_CH4_MASK    , PMU_AUXADC_RQST_CH4_SHIFT   , 1);
    pmu_auxadc_delay(1);
    result = (int)aux_get_adc_result(PMU_AUXADC_ADC_OUT_CH4_L_ADDR);

    ftmp = (float)(result - O_VTS);
    ftmp = ((ftmp / 4096.f) * 1.8f);
    result = (int)(DEGC_CALI + ((ftmp) / O_SLOPE));

    return (result);
}

void pmu_get_efuse(unsigned char addr, unsigned char *lowbyte, unsigned char *highbyte)
{
    pmu_set_register_value(0x14 , 0xFF , 0 , 3);
    pmu_set_register_value(0xC9 , 0xFF , 0 , 1);
    pmu_set_register_value(0xCC , 0xFF , 0 , 1);
    pmu_set_register_value(0xBF , 0xFF , 0 , addr);
    pmu_auxadc_delay(1);
    pmu_set_register_value(0xC9 , 0xFF , 0 , 0);

    pmu_auxadc_delay(1);
    *lowbyte  = pmu_get_register_value(0xCD , 0xFF , 0);
    *highbyte = pmu_get_register_value(0xCE , 0xFF , 0);
}

unsigned char pmu_get_thermal_status(unsigned char mode, unsigned char data)
{
    static unsigned char ADC_CALI_EN;
    if (mode != 0) {
        if ((data & 0x7E) != 0) {
            ADC_CALI_EN = 1;
        } else {
            ADC_CALI_EN = 0;
        }
    }

    return (ADC_CALI_EN);
}
#endif /* MTK_EXTERNAL_PMIC */
#endif /* HAL_PMU_AUXADC_MODULE_ENABLED */
