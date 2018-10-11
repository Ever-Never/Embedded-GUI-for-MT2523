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

#include <assert.h>
#include "hal.h"
#include "external_ntc.h"
#include "max17048_common.h"
#include "cust_battery_temperature_table.h"

extern const char EXT_NTC_POWER_SW_PIN;
extern const char EXT_NTC_ADC_PIN;

static int32_t ntc_resistance_to_temperature(int32_t resistance)
{
    int32_t i = 0;
    int32_t res_1 = 0, res_2 = 0;
    int32_t temperature = -200, temp_1 = 0, temp_2 = 0;

    if (resistance >= batt_temperature_table[0].temperature_r) {
        temperature = -20;
    } else if (resistance <= batt_temperature_table[16].temperature_r) {
        temperature = 60;
    } else {
        res_1 = batt_temperature_table[0].temperature_r;
        temp_1 = batt_temperature_table[0].battery_temp;

        for (i = 0; i <= 16; i++) {
            if (resistance >= batt_temperature_table[i].temperature_r) {
                res_2 = batt_temperature_table[i].temperature_r;
                temp_2 = batt_temperature_table[i].battery_temp;
                break;
            } else {
                res_1 = batt_temperature_table[i].temperature_r;
                temp_1 = batt_temperature_table[i].battery_temp;
            }
        }

        temperature = (((resistance - res_2) * temp_1) + ((res_1 - resistance) * temp_2)) / (res_1 - res_2);
    }

    return temperature;
}

static int32_t ntc_voltage_to_temperature(int32_t voltage)
{
    int64_t temp_res;
    int32_t ntc_temp = -100;
    lldiv_t rel_temp;

    temp_res = (RBAT_PULL_UP_R * (int64_t) voltage);
    /*lldiv for  64bit/32bit division*/
    rel_temp = lldiv(temp_res, abs(RBAT_PULL_UP_VOLT - voltage));
    temp_res = rel_temp.quot;

    if ((int32_t)temp_res < 0) {
        MAX17048_ERR("temp_res < 0 ERROR! RBAT_PULL_UP_VOLT = %d voltage = %d mV", RBAT_PULL_UP_VOLT, voltage);
        return -20;
    }
    /* convert resistance to temperature */
    ntc_temp = (int32_t)ntc_resistance_to_temperature((int32_t)temp_res);

    return ntc_temp;
}

static void ntc_power_enable(bool enable)
{

    hal_gpio_init((hal_gpio_pin_t)EXT_NTC_POWER_SW_PIN);

    if (enable) {
        hal_gpio_set_output((hal_gpio_pin_t)EXT_NTC_POWER_SW_PIN, HAL_GPIO_DATA_HIGH);
    } else {
        hal_gpio_set_output((hal_gpio_pin_t)EXT_NTC_POWER_SW_PIN, HAL_GPIO_DATA_LOW);
    }

    hal_gpio_deinit((hal_gpio_pin_t)EXT_NTC_POWER_SW_PIN);

}

static int32_t adc_ntc_voltage(void)
{
    uint32_t adc_data;
    uint32_t voltage;
    hal_adc_channel_t ntc_adc_channel;
    /* Enable NTC power */
    ntc_power_enable(true);
    /* Delay for switch on */
    hal_gpt_delay_ms(1);
    /* Initialize ADC module */
    hal_adc_init();

    switch (EXT_NTC_ADC_PIN) {
        case HAL_GPIO_0:
            ntc_adc_channel = HAL_ADC_CHANNEL_11;
            break;
        case HAL_GPIO_1:
            ntc_adc_channel = HAL_ADC_CHANNEL_12;
            break;
        case HAL_GPIO_2:
            ntc_adc_channel = HAL_ADC_CHANNEL_13;
            break;
        case HAL_GPIO_3:
            ntc_adc_channel = HAL_ADC_CHANNEL_14;
            break;
        case HAL_GPIO_10:
            ntc_adc_channel = HAL_ADC_CHANNEL_15;
            break;
        default:
            MAX17048_ERR("NTC ADC GPIO %d PIN Error! Could not mapping ADC channel", EXT_NTC_ADC_PIN);
            assert(0);
            break;
    }

    /* Retrieve ADC data */
    hal_adc_get_data_polling(ntc_adc_channel, &adc_data);
    /* Deinit ADC module when job done */
    hal_adc_deinit();
    /* Disable NTC power */
    ntc_power_enable(false);
    /* According to the formulation described above, the corresponding voltage of the raw data "adc_data" is
    2800/(2^12-1)*adc_data, and the uint of the voltage is mV */
    voltage = (adc_data * 2800) / 4095;
    return voltage;
}

int32_t external_ntc_get_temperature(void)
{
    int32_t ntc_volt;
    int32_t ntc_temperature;

    ntc_volt = adc_ntc_voltage();
    MAX17048_INFO("ntc_volt = %d mV", ntc_volt);

    if (ntc_volt == 0) {
        MAX17048_ERR("NTC ERROR! ntc_volt = %d mV", ntc_volt);
    }

    ntc_temperature = ntc_voltage_to_temperature(ntc_volt);

    return ntc_temperature;
}

bool external_ntc_init(void)
{
    if (EXT_NTC_POWER_SW_PIN == 0xFF || EXT_NTC_ADC_PIN == 0xFF) {
        MAX17048_ERR("No define EXT_NTC_POWER_SW_PIN and EXT_NTC_ADC_PIN of ept file!!");
        assert(0);
        return false;
    }

    if ((EXT_NTC_ADC_PIN != HAL_GPIO_0) && (EXT_NTC_ADC_PIN != HAL_GPIO_1) && (EXT_NTC_ADC_PIN != HAL_GPIO_2) &&
            (EXT_NTC_ADC_PIN != HAL_GPIO_3) && (EXT_NTC_ADC_PIN != HAL_GPIO_10)) {

        MAX17048_ERR("EXT_NTC_ADC_PIN define error!!");
        assert(0);
        return false;

    }

    return true;
}

