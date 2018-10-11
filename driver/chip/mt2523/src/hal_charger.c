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

#include "hal_charger.h"

#ifdef HAL_CHARGER_MODULE_ENABLED

#include "hal_pmu.h"
#include "hal_pmu_auxadc.h"
#include "hal_pmu_internal.h"
#include "hal_pmu_wrap_interface.h"
#include "hal_gpt.h"
#include "hal_nvic_internal.h"
#include "cust_charging.h"
#include "cust_battery_temperature_table.h"
#include "hal_log.h"

#define HAL_CAHRGER_DEEBUG_ENABLEx
#ifdef  HAL_CAHRGER_DEEBUG_ENABLE
#define log_hal_debug(_message,...) log_hal_info(_message, ##__VA_ARGS__)
#else
#define log_hal_debug(_message,...)
#endif

#define GETARRAYNUM(array) (sizeof(array)/sizeof(array[0]))

/*************************************
*           global variable
**************************************/

static bool init_cv_table_flag = false;
static uint16_t cv_register_value;
static uint16_t cv_trim_code;


uint32_t VBAT_CV_VTH[] = {
    HAL_BATTERY_VOLT_03_5000_V, HAL_BATTERY_VOLT_03_6000_V, HAL_BATTERY_VOLT_03_7000_V, HAL_BATTERY_VOLT_03_8000_V,
    HAL_BATTERY_VOLT_03_8500_V, HAL_BATTERY_VOLT_03_9000_V, HAL_BATTERY_VOLT_04_0000_V, HAL_BATTERY_VOLT_04_0500_V,
    HAL_BATTERY_VOLT_04_1000_V, HAL_BATTERY_VOLT_04_1250_V, HAL_BATTERY_VOLT_04_1375_V, HAL_BATTERY_VOLT_04_1500_V,
    HAL_BATTERY_VOLT_04_1625_V, HAL_BATTERY_VOLT_04_1750_V, HAL_BATTERY_VOLT_04_1875_V, HAL_BATTERY_VOLT_04_2000_V,

    HAL_BATTERY_VOLT_04_2125_V, HAL_BATTERY_VOLT_04_2250_V, HAL_BATTERY_VOLT_04_2375_V, HAL_BATTERY_VOLT_04_2500_V,
    HAL_BATTERY_VOLT_04_2625_V, HAL_BATTERY_VOLT_04_2750_V, HAL_BATTERY_VOLT_04_2875_V, HAL_BATTERY_VOLT_04_3000_V,
    HAL_BATTERY_VOLT_04_3125_V, HAL_BATTERY_VOLT_04_3250_V, HAL_BATTERY_VOLT_04_3375_V, HAL_BATTERY_VOLT_04_3500_V,
    HAL_BATTERY_VOLT_04_3625_V, HAL_BATTERY_VOLT_04_3750_V, HAL_BATTERY_VOLT_04_3875_V, HAL_BATTERY_VOLT_04_4000_V,

    HAL_BATTERY_VOLT_04_4125_V, HAL_BATTERY_VOLT_04_4250_V, HAL_BATTERY_VOLT_04_4375_V, HAL_BATTERY_VOLT_04_4500_V,
    HAL_BATTERY_VOLT_04_4625_V, HAL_BATTERY_VOLT_04_4750_V, HAL_BATTERY_VOLT_04_4875_V, HAL_BATTERY_VOLT_04_5000_V,
    HAL_BATTERY_VOLT_04_6000_V, HAL_BATTERY_VOLT_INVALID, HAL_BATTERY_VOLT_INVALID, HAL_BATTERY_VOLT_INVALID,
    HAL_BATTERY_VOLT_INVALID, HAL_BATTERY_VOLT_INVALID, HAL_BATTERY_VOLT_INVALID, HAL_BATTERY_VOLT_INVALID
};

const uint32_t VBAT_CV_VTH_F[] = {
    HAL_BATTERY_VOLT_03_5000_V, HAL_BATTERY_VOLT_03_6000_V, HAL_BATTERY_VOLT_03_7000_V, HAL_BATTERY_VOLT_03_8000_V,
    HAL_BATTERY_VOLT_03_8500_V, HAL_BATTERY_VOLT_03_9000_V, HAL_BATTERY_VOLT_04_0000_V, HAL_BATTERY_VOLT_04_0500_V,
    HAL_BATTERY_VOLT_04_1000_V, HAL_BATTERY_VOLT_04_1250_V, HAL_BATTERY_VOLT_04_1375_V, HAL_BATTERY_VOLT_04_1500_V,
    HAL_BATTERY_VOLT_04_1625_V, HAL_BATTERY_VOLT_04_1750_V, HAL_BATTERY_VOLT_04_1875_V, HAL_BATTERY_VOLT_04_2000_V,

    HAL_BATTERY_VOLT_04_2125_V, HAL_BATTERY_VOLT_04_2250_V, HAL_BATTERY_VOLT_04_2375_V, HAL_BATTERY_VOLT_04_2500_V,
    HAL_BATTERY_VOLT_04_2625_V, HAL_BATTERY_VOLT_04_2750_V, HAL_BATTERY_VOLT_04_2875_V, HAL_BATTERY_VOLT_04_3000_V,
    HAL_BATTERY_VOLT_04_3125_V, HAL_BATTERY_VOLT_04_3250_V, HAL_BATTERY_VOLT_04_3375_V, HAL_BATTERY_VOLT_04_3500_V,
    HAL_BATTERY_VOLT_04_3625_V, HAL_BATTERY_VOLT_04_3750_V, HAL_BATTERY_VOLT_04_3875_V, HAL_BATTERY_VOLT_04_4000_V,

    HAL_BATTERY_VOLT_04_4125_V, HAL_BATTERY_VOLT_04_4250_V, HAL_BATTERY_VOLT_04_4375_V, HAL_BATTERY_VOLT_04_4500_V,
    HAL_BATTERY_VOLT_04_4625_V, HAL_BATTERY_VOLT_04_4750_V, HAL_BATTERY_VOLT_04_4875_V, HAL_BATTERY_VOLT_04_5000_V,
    HAL_BATTERY_VOLT_04_6000_V, HAL_BATTERY_VOLT_INVALID, HAL_BATTERY_VOLT_INVALID, HAL_BATTERY_VOLT_INVALID,
    HAL_BATTERY_VOLT_INVALID, HAL_BATTERY_VOLT_INVALID, HAL_BATTERY_VOLT_INVALID, HAL_BATTERY_VOLT_INVALID
};

const uint32_t CS_VTH[] = {
    84000,      /* CS_VTH 84000 uV*/
    75600,      /* CS_VTH 75600 uV*/
    67200,      /* CS_VTH 67200 uV*/
    61600,      /* CS_VTH 61600 uV*/
    56000,      /* CS_VTH 56000 uV*/
    50400,      /* CS_VTH 50400 uV*/
    44800,      /* CS_VTH 44800 uV*/
    39200,      /* CS_VTH 39200 uV*/
    36400,      /* CS_VTH 36400 uV*/
    30800,      /* CS_VTH 30800 uV*/
    25200,      /* CS_VTH 25200 uV*/
    16800,      /* CS_VTH 16800 uV*/
    11200,      /* CS_VTH 11200 uV*/
    7884,       /* CS_VTH 7884 uV*/
    6234,       /* CS_VTH 6234 uV*/
    3920        /* CS_VTH 3920 uV*/
};

const uint32_t VCDT_HV_VTH[] = {
    HAL_BATTERY_VOLT_04_2000_V, HAL_BATTERY_VOLT_04_2500_V, HAL_BATTERY_VOLT_04_3000_V, HAL_BATTERY_VOLT_04_3500_V,
    HAL_BATTERY_VOLT_04_4000_V, HAL_BATTERY_VOLT_04_4500_V, HAL_BATTERY_VOLT_04_5000_V, HAL_BATTERY_VOLT_04_5500_V,
    HAL_BATTERY_VOLT_04_6000_V, HAL_BATTERY_VOLT_06_0000_V, HAL_BATTERY_VOLT_06_5000_V, HAL_BATTERY_VOLT_07_0000_V,
    HAL_BATTERY_VOLT_07_5000_V, HAL_BATTERY_VOLT_08_5000_V, HAL_BATTERY_VOLT_09_5000_V, HAL_BATTERY_VOLT_10_5000_V
};

static hal_charger_status_t charging_hw_init(void)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;

    log_hal_debug("charging_hw_init\n");

    pmu_set_register_value(PMU_RG_CHRWDT_TD_ADDR, PMU_RG_CHRWDT_TD_MASK, PMU_RG_CHRWDT_TD_SHIFT, 2);       /* CHRWDT_TD, 16s */
    pmu_set_register_value(PMU_RG_CHRWDT_EN_ADDR, PMU_RG_CHRWDT_EN_MASK, PMU_RG_CHRWDT_EN_SHIFT, 1);       /* CHRWDT_EN */
    pmu_set_register_value(PMU_RG_CHRWDT_WR_ADDR, PMU_RG_CHRWDT_WR_MASK, PMU_RG_CHRWDT_WR_SHIFT, 1);       /* CHRWDT_WR */
    pmu_set_register_value(PMU_RG_VCDT_MODE_ADDR, PMU_RG_VCDT_MODE_MASK, PMU_RG_VCDT_MODE_SHIFT, 0);       /* VCDT_MODE */
    pmu_set_register_value(PMU_RG_VCDT_HV_EN_ADDR, PMU_RG_VCDT_HV_EN_MASK, PMU_RG_VCDT_HV_EN_SHIFT, 1);    /* VCDT_HV_EN */

    pmu_set_register_value(PMU_RG_USBDL_SET_ADDR, PMU_RG_USBDL_SET_MASK, PMU_RG_USBDL_SET_SHIFT, 0);       /* force leave USBDL mode */
    pmu_set_register_value(PMU_RG_USBDL_RST_ADDR, PMU_RG_USBDL_RST_MASK, PMU_RG_USBDL_RST_SHIFT, 1);       /* force leave USBDL mode */

    pmu_set_register_value(PMU_RG_BC11_BB_CTRL_ADDR, PMU_RG_BC11_BB_CTRL_MASK, PMU_RG_BC11_BB_CTRL_SHIFT, 1); /* BC11_BB_CTRL */
    pmu_set_register_value(PMU_RG_BC11_RST_ADDR, PMU_RG_BC11_RST_MASK, PMU_RG_BC11_RST_SHIFT, 1);          /* BC11_RST */
    pmu_set_register_value(PMU_RG_CSDAC_MODE_ADDR, PMU_RG_CSDAC_MODE_MASK, PMU_RG_CSDAC_MODE_SHIFT, 1);    /* CSDAC_MODE */
    pmu_set_register_value(PMU_RG_VBAT_OV_EN_ADDR, PMU_RG_VBAT_OV_EN_MASK, PMU_RG_VBAT_OV_EN_SHIFT, 1);    /* VBAT_OV_EN */

#ifdef HIGH_BATTERY_VOLTAGE_SUPPORT
    pmu_set_register_value(PMU_RG_VBAT_OV_VTH_ADDR, PMU_RG_VBAT_OV_VTH_MASK, PMU_RG_VBAT_OV_VTH_SHIFT, 3);  /* VBAT_OV_VTH, 4.4V */
#else
    pmu_set_register_value(PMU_RG_VBAT_OV_VTH_ADDR, PMU_RG_VBAT_OV_VTH_MASK, PMU_RG_VBAT_OV_VTH_SHIFT, 2);  /* VBAT_OV_VTH, 4.3V */
#endif
    pmu_set_register_value(PMU_RG_BATON_EN_ADDR, PMU_RG_BATON_EN_MASK, PMU_RG_BATON_EN_SHIFT, 1);           /* BATON_EN */

    pmu_set_register_value(PMU_BATON_TDET_EN_ADDR, PMU_BATON_TDET_EN_MASK, PMU_BATON_TDET_EN_SHIFT, 1);     /* BATON_TDET_EN */

    /*for TBAT*/
    pmu_set_register_value(PMU_RG_BATON_HT_EN_ADDR, PMU_RG_BATON_HT_EN_MASK, PMU_RG_BATON_HT_EN_SHIFT, 0);  /* BATON_HT_EN */
    pmu_set_register_value(PMU_RG_ULC_DET_EN_ADDR, PMU_RG_ULC_DET_EN_MASK, PMU_RG_ULC_DET_EN_SHIFT, 1);     /* RG_ULC_DET_EN=1 */
    pmu_set_register_value(PMU_RG_LOW_ICH_DB_ADDR, PMU_RG_LOW_ICH_DB_MASK, PMU_RG_LOW_ICH_DB_SHIFT, 4);     /* RG_LOW_ICH_DB= 4 *16ms */
    return status;
}

static uint32_t charging_parameter_to_value(const uint32_t *parameter, const uint32_t array_size, const uint32_t val)
{
    uint32_t i;

    for (i = 0; i < array_size; i++) {
        if (val == *(parameter + i)) {
            return i;
        }
    }

    log_hal_error("NO register value match \n");
    /*TODO: ASSERT(0); not find the value */
    return 0;
}

static uint32_t find_closest_level(const uint32_t *pList, uint32_t number, uint32_t level)
{
    uint32_t i;
    uint32_t max_value_in_last_element;

    if (pList[0] < pList[1]) {
        max_value_in_last_element = true;
    } else {
        max_value_in_last_element = false;
    }

    if (max_value_in_last_element == true) {
        for (i = (number - 1); i != 0; i--) { /* max value in the last element */
            if (pList[i] <= level) {
                return pList[i];
            }
        }
        log_hal_error("Can't find closest level\n");
        return pList[0];
    } else {
        for (i = 0; i < number; i++) {  /* max value in the first element */
            if (pList[i] <= level) {
                return pList[i];
            }
        }
        log_hal_error("Can't find closest level\n");
        return pList[number - 1];
    }
}

/* This function is used for FT CV threshold trimming and not for customer's fine tuning */
/* Do not modify */
static void calibration_cv_threshold_table(void)
{
    int32_t cv_offset;
    uint32_t index;
    uint8_t efuse_high_byte;
    uint8_t efuse_low_byte;
    uint32_t irq_flag;

    if (init_cv_table_flag == false) {

        init_cv_table_flag = true;

        irq_flag = save_and_set_interrupt_mask();
        /* This efause value is used for FT CV threshold trimming and not for customer's fine tuning */
        pmu_get_efuse(0x1A, &efuse_low_byte, &efuse_high_byte);
        restore_interrupt_mask(irq_flag);

        log_hal_info("CV trim efuse_low_byte  = 0x%X ", efuse_low_byte);

        cv_register_value = efuse_low_byte & 0x3F;

        /* Get CV trim code. Do not modify */
        if (cv_register_value <= 0x08) {
            cv_trim_code = 0x08;
        } else if (cv_register_value >= 0x18) {
            cv_trim_code = 0x18;
        } else {
            cv_trim_code = cv_register_value;
        }

        log_hal_info("cv_register_value = 0x%X cv_trim_code = 0x%X", cv_register_value, cv_trim_code);

        /* Calculate offset of CV trim code. Do not modify */
        if (cv_trim_code == 0x0f) {
            cv_offset = 0;
        } else if (cv_trim_code == 0x08) {
            cv_offset = 1000; /* 0.1 V */
        } else if (cv_trim_code > 0x0f) {
            cv_offset = -(cv_trim_code - 0x0f) * 125;   /* Step = 0.0125V */
        } else if (cv_trim_code < 0x0f) {
            cv_offset = (0x0f - cv_trim_code) * 125;   /* Step = 0.0125V */
        }

        log_hal_debug("cv_offset = %d", cv_offset);

        /* Rebuild CV table */
        for (index = 0 ; index <= 0x28 ; index++) {
            VBAT_CV_VTH[index] = VBAT_CV_VTH_F[index] + cv_offset;
        }

#ifdef HAL_CAHRGER_DEEBUG_ENABLE
        for (index = 0 ; index <= 47 ; index = index + 4) {
            if (index == 0) {
                log_hal_info("cv_trim_code table = %d ", cv_trim_code);
            }

            if ((index % 16) == 0) {
                log_hal_info("");
            }
            log_hal_info("%d %d %d %d ", VBAT_CV_VTH[index], VBAT_CV_VTH[index + 1], VBAT_CV_VTH[index + 2], VBAT_CV_VTH[index + 3]);
        }
        log_hal_info("\n");
#endif
    }
}

hal_charger_status_t hal_charger_enable(bool enable)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;

    log_hal_debug("hal_charger_enable %s\n", enable ? "true" : "false");

    if (enable == true) {
        pmu_set_register_value(PMU_RG_CSDAC_DLY_ADDR, PMU_RG_CSDAC_DLY_MASK, PMU_RG_CSDAC_DLY_SHIFT, 1);    /* CSDAC_DLY */
        pmu_set_register_value(PMU_RG_CSDAC_STP_ADDR, PMU_RG_CSDAC_STP_MASK, PMU_RG_CSDAC_STP_SHIFT, 7);    /* CSDAC_STP */
        pmu_set_register_value(PMU_RG_CSDAC_STP_INC_ADDR, PMU_RG_CSDAC_STP_INC_MASK, PMU_RG_CSDAC_STP_INC_SHIFT, 0); /* CSDAC_STP_INC */
        pmu_set_register_value(PMU_RG_CSDAC_STP_DEC_ADDR, PMU_RG_CSDAC_STP_DEC_MASK, PMU_RG_CSDAC_STP_DEC_SHIFT, 0); /* CSDAC_STP_DEC */
        pmu_set_register_value(PMU_RG_CS_EN_ADDR, PMU_RG_CS_EN_MASK, PMU_RG_CS_EN_SHIFT, 1);                /* CS_EN */
        pmu_set_register_value(PMU_RG_HWCV_EN_ADDR, PMU_RG_HWCV_EN_MASK, PMU_RG_HWCV_EN_SHIFT, 1);

        pmu_set_register_value(PMU_RG_VBAT_CV_EN_ADDR, PMU_RG_VBAT_CV_EN_MASK, PMU_RG_VBAT_CV_EN_SHIFT, 1); /* CV_EN */
        pmu_set_register_value(PMU_RG_CSDAC_EN_ADDR, PMU_RG_CSDAC_EN_MASK, PMU_RG_CSDAC_EN_SHIFT, 1);        /* CSDAC_EN */
        pmu_set_register_value(PMU_RG_PCHR_FLAG_EN_ADDR, PMU_RG_PCHR_FLAG_EN_MASK, PMU_RG_PCHR_FLAG_EN_SHIFT, 1);   /* enable debug falg output */

        pmu_set_register_value(PMU_RG_NORM_CHR_EN_ADDR, PMU_RG_NORM_CHR_EN_MASK, PMU_RG_NORM_CHR_EN_SHIFT, 1);      /* NORM_CHR_EN */

    } else {
        pmu_set_register_value(PMU_RG_CHRWDT_EN_ADDR, PMU_RG_CHRWDT_EN_MASK, PMU_RG_CHRWDT_EN_SHIFT, 0);            /* CHRWDT_EN */
        pmu_set_register_value(PMU_RG_CHRWDT_FLAG_WR_ADDR, PMU_RG_CHRWDT_FLAG_WR_MASK, PMU_RG_CHRWDT_FLAG_WR_SHIFT, 0); /* CHRWDT_FLAG */
        pmu_set_register_value(PMU_RG_CSDAC_EN_ADDR, PMU_RG_CSDAC_EN_MASK, PMU_RG_CSDAC_EN_SHIFT, 0);               /* CSDAC_EN */
        pmu_set_register_value(PMU_RG_NORM_CHR_EN_ADDR, PMU_RG_NORM_CHR_EN_MASK, PMU_RG_NORM_CHR_EN_SHIFT, 0);      /* NORM_CHR_EN */
        pmu_set_register_value(PMU_RG_HWCV_EN_ADDR, PMU_RG_HWCV_EN_MASK, PMU_RG_HWCV_EN_SHIFT, 0);                  /* RG_HWCV_EN */
    }
    return status;
}

hal_charger_status_t hal_charger_set_constant_voltage(uint32_t voltage)
{
    bool ret_status;
    uint16_t register_value;
    uint32_t array_size;
    uint32_t set_cv_vth;
    bool charger_status = false;

    log_hal_debug("hal_charger_set_constant_voltage\n");

    /* Calibration the CV threshold mapping table */
    /* This function is used for FT CV threshold trimming and not for customer's fine tuning */
    calibration_cv_threshold_table();

    /* Check CV range,The CV voltage must meet all of CV trim code mapping table limit.*/
    /* Do not modify */
    if (voltage < HAL_BATTERY_VOLT_03_6000_V || voltage > HAL_BATTERY_VOLT_04_4875_V) {
        log_hal_error("Set constant voltage fail (Over CV range) \r\n");
        return HAL_CHARGER_STATUS_INVALID_PARAMETER;
    }

    array_size = GETARRAYNUM(VBAT_CV_VTH);
    set_cv_vth = find_closest_level(VBAT_CV_VTH, array_size, voltage);
    register_value = charging_parameter_to_value(VBAT_CV_VTH, array_size , set_cv_vth);

    hal_charger_get_charger_detect_status(&charger_status);
    if (charger_status == true) {
        log_hal_info("constant_voltage = %d cs_vth %d reg[%X] cv_register_value = 0x%X  cv_trim_code = 0x%X\r\n", voltage, set_cv_vth, register_value, cv_register_value, cv_trim_code);
        /* This register is used for FT CV threshold trimming and not for customer's fine tuning */
        /* Do not directly control RG_VBAT_NORM_CV_VTH register */
        ret_status = pmu_set_register_value(PMU_RG_VBAT_NORM_CV_VTH_ADDR, PMU_RG_VBAT_NORM_CV_VTH_MASK, PMU_RG_VBAT_NORM_CV_VTH_SHIFT, register_value);
    } else {
        ret_status = false;
        log_hal_error("Set constant voltage fail (No charger plug-in) \r\n");
    }

    if (ret_status == true) {
        return HAL_CHARGER_STATUS_OK;
    } else {
        return HAL_CHARGER_STATUS_ERROR;
    }
}

hal_charger_status_t hal_charger_set_charging_current(uint32_t current)
{
    bool ret_status;
    uint32_t set_cs_vth;
    uint32_t array_size;
    uint32_t register_value;

    log_hal_debug("hal_charger_set_charging_current\n");
    if (current * CUST_R_SENSE  < CS_VTH[GETARRAYNUM(CS_VTH) - 1]) {
        return HAL_CHARGER_STATUS_INVALID_PARAMETER;
    }

    array_size = GETARRAYNUM(CS_VTH);
    set_cs_vth = find_closest_level(CS_VTH, array_size, current * CUST_R_SENSE);
    register_value = charging_parameter_to_value(CS_VTH, array_size , set_cs_vth);
    log_hal_info("charging_current = %d cs_vth %d reg[%X]\r\n", current, set_cs_vth, register_value);
    ret_status =  pmu_set_register_value(PMU_RG_NORM_CS_VTH_ADDR, PMU_RG_NORM_CS_VTH_MASK, PMU_RG_NORM_CS_VTH_SHIFT, register_value);

    if (ret_status == true) {
        return HAL_CHARGER_STATUS_OK;
    } else {
        return HAL_CHARGER_STATUS_ERROR;
    }
}

hal_charger_status_t hal_charger_reset_watchdog_timer(void)
{
    bool ret_status = true;
    log_hal_debug("hal_charger_reset_watchdog_timer\n");

    ret_status = pmu_set_register_value(PMU_RG_CHRWDT_TD_ADDR, PMU_RG_CHRWDT_TD_MASK, PMU_RG_CHRWDT_TD_SHIFT, 2);                /* CHRWDT_TD, 16s */
    ret_status &= pmu_set_register_value(PMU_RG_CHRWDT_EN_ADDR, PMU_RG_CHRWDT_EN_MASK, PMU_RG_CHRWDT_EN_SHIFT, 1);                /* CHRWDT_EN */
    ret_status &= pmu_set_register_value(PMU_RG_CHRWDT_WR_ADDR, PMU_RG_CHRWDT_WR_MASK, PMU_RG_CHRWDT_WR_SHIFT, 1);                /* CHRWDT_WR */
    ret_status &= pmu_set_register_value(PMU_RG_CHRWDT_FLAG_WR_ADDR, PMU_RG_CHRWDT_FLAG_WR_MASK, PMU_RG_CHRWDT_FLAG_WR_SHIFT, 1); /* CHRWDT_FLAG */
    if (ret_status == true) {
        return HAL_CHARGER_STATUS_OK;
    } else {
        return HAL_CHARGER_STATUS_ERROR;
    }
}

hal_charger_status_t hal_charger_set_high_voltage_threshold(uint32_t voltage)
{
    bool ret_status = true;
    uint32_t set_hv_voltage;
    uint32_t array_size;
    uint16_t register_value;

    log_hal_debug("hal_charger_set_high_voltage_threshold\n");

    if (voltage < VCDT_HV_VTH[0]) {
        return HAL_CHARGER_STATUS_INVALID_PARAMETER;
    }
    array_size = GETARRAYNUM(VCDT_HV_VTH);
    set_hv_voltage = find_closest_level(VCDT_HV_VTH, array_size, voltage);
    register_value = charging_parameter_to_value(VCDT_HV_VTH, array_size , set_hv_voltage);
    ret_status = pmu_set_register_value(PMU_RG_VCDT_HV_VTH_ADDR, PMU_RG_VCDT_HV_VTH_MASK, PMU_RG_VCDT_HV_VTH_SHIFT, register_value);

    if (ret_status == true) {
        return HAL_CHARGER_STATUS_OK;
    } else {
        return HAL_CHARGER_STATUS_ERROR;
    }
}

hal_charger_status_t hal_charger_get_high_voltage_status(bool *high_voltage_status)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;
    uint16_t register_value;
    log_hal_debug("hal_charger_get_high_voltage_status\n");

    register_value = pmu_get_register_value(PMU_RGS_VCDT_HV_DET_ADDR, PMU_RGS_VCDT_HV_DET_MASK, PMU_RGS_VCDT_HV_DET_SHIFT);

    if (register_value == 1) {
        *high_voltage_status = true;
    } else {
        *high_voltage_status = false;
    }
    return status;
}

hal_charger_status_t hal_charger_get_battery_status(bool *battery_status)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;
    uint32_t val = 0;
    uint32_t ret = 0;
    log_hal_debug("hal_charger_get_battery_status\n");

    val = pmu_get_register_value(PMU_BATON_TDET_EN_ADDR, PMU_BATON_TDET_EN_MASK, PMU_BATON_TDET_EN_SHIFT);
    log_hal_debug("[charging_get_battery_status] BATON_TDET_EN = %d\n", val);
    if (val) {
        pmu_set_register_value(PMU_BATON_TDET_EN_ADDR, PMU_BATON_TDET_EN_MASK, PMU_BATON_TDET_EN_SHIFT, 1);
        pmu_set_register_value(PMU_RG_BATON_EN_ADDR, PMU_RG_BATON_EN_MASK, PMU_RG_BATON_EN_SHIFT, 1);
        ret = pmu_get_register_value(PMU_RGS_BATON_UNDET_ADDR, PMU_RGS_BATON_UNDET_MASK, PMU_RGS_BATON_UNDET_SHIFT);
        if (ret == 0) {
            *battery_status = true;
        } else {
            *battery_status = false;
        }
    } else {
        *battery_status = true;
    }

    return status;
}

hal_charger_status_t hal_charger_get_charger_detect_status(bool *charger_status)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;
    uint16_t register_value;

    log_hal_debug("hal_charger_get_charger_detect_status\n");

    register_value = pmu_get_register_value(PMU_RGS_CHRDET_ADDR, PMU_RGS_CHRDET_MASK, PMU_RGS_CHRDET_SHIFT);

    if (register_value == 1) {
        *charger_status = true;
    } else {
        *charger_status = false;
    }

    return status;
}

static void hw_bc11_init(void)
{
    log_hal_debug("hw_bc11_init\n");

    pmu_ctrl_driver_power(DRIVER_USB, PMIC_CTL_ENABLE); /* Turn on 3.3V phy power */

    hal_gpt_delay_ms(200);

    /* RG_bc11_BIAS_EN=1 */
    pmu_set_register_value(PMU_RG_BC11_BIAS_EN_ADDR, PMU_RG_BC11_BIAS_EN_MASK, PMU_RG_BC11_BIAS_EN_SHIFT, 1);
    /* RG_bc11_VSRC_EN[1:0]=00 */
    pmu_set_register_value(PMU_RG_BC11_VSRC_EN_ADDR, PMU_RG_BC11_VSRC_EN_MASK, PMU_RG_BC11_VSRC_EN_SHIFT, 0);
    /* RG_bc11_VREF_VTH = [1:0]=00 */
    pmu_set_register_value(PMU_RG_BC11_VREF_VTH_ADDR, PMU_RG_BC11_VREF_VTH_MASK, PMU_RG_BC11_VREF_VTH_SHIFT, 0);
    /* RG_bc11_CMP_EN[1.0] = 00 */
    pmu_set_register_value(PMU_RG_BC11_CMP_EN_ADDR, PMU_RG_BC11_CMP_EN_MASK, PMU_RG_BC11_CMP_EN_SHIFT, 0);
    /* RG_bc11_IPU_EN[1.0] = 00 */
    pmu_set_register_value(PMU_RG_BC11_IPU_EN_ADDR, PMU_RG_BC11_IPU_EN_MASK, PMU_RG_BC11_IPU_EN_SHIFT, 0);
    /* RG_bc11_IPD_EN[1.0] = 00 */
    pmu_set_register_value(PMU_RG_BC11_IPD_EN_ADDR, PMU_RG_BC11_IPD_EN_MASK, PMU_RG_BC11_IPD_EN_SHIFT, 0);
    /* bc11_RST=1 */
    pmu_set_register_value(PMU_RG_BC11_RST_ADDR, PMU_RG_BC11_RST_MASK, PMU_RG_BC11_RST_SHIFT, 1);
    /* bc11_BB_CTRL=1 */
    pmu_set_register_value(PMU_RG_BC11_BB_CTRL_ADDR, PMU_RG_BC11_BB_CTRL_MASK, PMU_RG_BC11_BB_CTRL_SHIFT, 1);

    hal_gpt_delay_ms(50);
}

static uint32_t hw_bc11_DCD(void)
{
    uint32_t wChargerAvail = 0;

    log_hal_debug("hw_bc11_DCD\n");

    /* RG_bc11_IPU_EN[1.0] = 10 */
    pmu_set_register_value(PMU_RG_BC11_IPU_EN_ADDR, PMU_RG_BC11_IPU_EN_MASK, PMU_RG_BC11_IPU_EN_SHIFT, 0x2);
    /* RG_bc11_IPD_EN[1.0] = 01 */
    pmu_set_register_value(PMU_RG_BC11_IPD_EN_ADDR, PMU_RG_BC11_IPD_EN_MASK, PMU_RG_BC11_IPD_EN_SHIFT, 0x1);
    /* RG_bc11_VREF_VTH = [1:0]=01 */
    pmu_set_register_value(PMU_RG_BC11_VREF_VTH_ADDR, PMU_RG_BC11_VREF_VTH_MASK, PMU_RG_BC11_VREF_VTH_SHIFT, 0x1);
    /* RG_bc11_CMP_EN[1.0] = 10 */
    pmu_set_register_value(PMU_RG_BC11_CMP_EN_ADDR, PMU_RG_BC11_CMP_EN_MASK, PMU_RG_BC11_CMP_EN_SHIFT, 0x2);

    hal_gpt_delay_ms(80);

    wChargerAvail = pmu_get_register_value(PMU_RGS_BC11_CMP_OUT_ADDR, PMU_RGS_BC11_CMP_OUT_MASK, PMU_RGS_BC11_CMP_OUT_SHIFT);

    /* RG_bc11_IPU_EN[1.0] = 00 */
    pmu_set_register_value(PMU_RG_BC11_IPU_EN_ADDR, PMU_RG_BC11_IPU_EN_MASK, PMU_RG_BC11_IPU_EN_SHIFT, 0x0);
    /* RG_bc11_IPD_EN[1.0] = 00 */
    pmu_set_register_value(PMU_RG_BC11_IPD_EN_ADDR, PMU_RG_BC11_IPD_EN_MASK, PMU_RG_BC11_IPD_EN_SHIFT, 0x0);
    /* RG_bc11_CMP_EN[1.0] = 00 */
    pmu_set_register_value(PMU_RG_BC11_CMP_EN_ADDR, PMU_RG_BC11_CMP_EN_MASK, PMU_RG_BC11_CMP_EN_SHIFT, 0x0);
    /* RG_bc11_VREF_VTH = [1:0]=00 */
    pmu_set_register_value(PMU_RG_BC11_VREF_VTH_ADDR, PMU_RG_BC11_VREF_VTH_MASK, PMU_RG_BC11_VREF_VTH_SHIFT, 0x0);

    return wChargerAvail;
}

static uint32_t hw_bc11_stepA2(void)
{
    uint32_t wChargerAvail = 0;

    log_hal_debug("hw_bc11_stepA2\n");

    /* RG_bc11_VSRC_EN[1.0] = 10 */
    pmu_set_register_value(PMU_RG_BC11_VSRC_EN_ADDR, PMU_RG_BC11_VSRC_EN_MASK, PMU_RG_BC11_VSRC_EN_SHIFT, 0x2);
    /* RG_bc11_IPD_EN[1:0] = 01 */
    pmu_set_register_value(PMU_RG_BC11_IPD_EN_ADDR, PMU_RG_BC11_IPD_EN_MASK, PMU_RG_BC11_IPD_EN_SHIFT, 0x1);
    /* RG_bc11_VREF_VTH = [1:0]=00 */
    pmu_set_register_value(PMU_RG_BC11_VREF_VTH_ADDR, PMU_RG_BC11_VREF_VTH_MASK, PMU_RG_BC11_VREF_VTH_SHIFT, 0x0);
    /* RG_bc11_CMP_EN[1.0] = 01 */
    pmu_set_register_value(PMU_RG_BC11_CMP_EN_ADDR, PMU_RG_BC11_CMP_EN_MASK, PMU_RG_BC11_CMP_EN_SHIFT, 0x1);

    hal_gpt_delay_ms(80);

    hal_gpt_delay_ms(80);

    wChargerAvail = pmu_get_register_value(PMU_RGS_BC11_CMP_OUT_ADDR, PMU_RGS_BC11_CMP_OUT_MASK, PMU_RGS_BC11_CMP_OUT_SHIFT);

    /* RG_bc11_VSRC_EN[1:0]=00 */
    pmu_set_register_value(PMU_RG_BC11_VSRC_EN_ADDR, PMU_RG_BC11_VSRC_EN_MASK, PMU_RG_BC11_VSRC_EN_SHIFT, 0x0);
    /* RG_bc11_IPD_EN[1.0] = 00 */
    pmu_set_register_value(PMU_RG_BC11_IPD_EN_ADDR, PMU_RG_BC11_IPD_EN_MASK, PMU_RG_BC11_IPD_EN_SHIFT, 0x0);
    /* RG_bc11_CMP_EN[1.0] = 00 */
    pmu_set_register_value(PMU_RG_BC11_CMP_EN_ADDR, PMU_RG_BC11_CMP_EN_MASK, PMU_RG_BC11_CMP_EN_SHIFT, 0x0);

    return  wChargerAvail;
}

static uint32_t hw_bc11_stepB2(void)
{
    uint32_t wChargerAvail = 0;

    log_hal_debug("hw_bc11_stepB2\n");

    /* RG_bc11_IPU_EN[1:0]=10 */
    pmu_set_register_value(PMU_RG_BC11_IPU_EN_ADDR, PMU_RG_BC11_IPU_EN_MASK, PMU_RG_BC11_IPU_EN_SHIFT, 0x2);
    /* RG_bc11_VREF_VTH = [1:0]=01 */
    pmu_set_register_value(PMU_RG_BC11_VREF_VTH_ADDR, PMU_RG_BC11_VREF_VTH_MASK, PMU_RG_BC11_VREF_VTH_SHIFT, 0x1);
    /* RG_bc11_CMP_EN[1.0] = 01 */
    pmu_set_register_value(PMU_RG_BC11_CMP_EN_ADDR, PMU_RG_BC11_CMP_EN_MASK, PMU_RG_BC11_CMP_EN_SHIFT, 0x1);

    hal_gpt_delay_ms(80);

    wChargerAvail = pmu_get_register_value(PMU_RGS_BC11_CMP_OUT_ADDR, PMU_RGS_BC11_CMP_OUT_MASK, PMU_RGS_BC11_CMP_OUT_SHIFT);

    if (!wChargerAvail) {
        /* RG_bc11_VSRC_EN[1.0] = 10 */
        pmu_set_register_value(PMU_RG_BC11_VSRC_EN_ADDR, PMU_RG_BC11_VSRC_EN_MASK, PMU_RG_BC11_VSRC_EN_SHIFT, 0x2);
    }
    /* RG_bc11_IPU_EN[1.0] = 00 */
    pmu_set_register_value(PMU_RG_BC11_IPU_EN_ADDR, PMU_RG_BC11_IPU_EN_MASK, PMU_RG_BC11_IPU_EN_SHIFT, 0x0);
    /* RG_bc11_CMP_EN[1.0] = 00 */
    pmu_set_register_value(PMU_RG_BC11_CMP_EN_ADDR, PMU_RG_BC11_CMP_EN_MASK, PMU_RG_BC11_CMP_EN_SHIFT, 0x0);
    /* RG_bc11_VREF_VTH = [1:0]=00 */
    pmu_set_register_value(PMU_RG_BC11_VREF_VTH_ADDR, PMU_RG_BC11_VREF_VTH_MASK, PMU_RG_BC11_VREF_VTH_SHIFT, 0x0);

    return  wChargerAvail;
}

static void hw_bc11_done(void)
{
    log_hal_debug("hw_bc11_done\n");

    /* RG_bc11_VSRC_EN[1:0]=00 */
    pmu_set_register_value(PMU_RG_BC11_VSRC_EN_ADDR, PMU_RG_BC11_VSRC_EN_MASK, PMU_RG_BC11_VSRC_EN_SHIFT, 0x0);
    /* RG_bc11_VREF_VTH = [1:0]=0 */
    pmu_set_register_value(PMU_RG_BC11_VREF_VTH_ADDR, PMU_RG_BC11_VREF_VTH_MASK, PMU_RG_BC11_VREF_VTH_SHIFT, 0x0);
    /* RG_bc11_CMP_EN[1.0] = 00 */
    pmu_set_register_value(PMU_RG_BC11_CMP_EN_ADDR, PMU_RG_BC11_CMP_EN_MASK, PMU_RG_BC11_CMP_EN_SHIFT, 0x0);
    /* RG_bc11_IPU_EN[1.0] = 00 */
    pmu_set_register_value(PMU_RG_BC11_IPU_EN_ADDR, PMU_RG_BC11_IPU_EN_MASK, PMU_RG_BC11_IPU_EN_SHIFT, 0x0);
    /* RG_bc11_IPD_EN[1.0] = 00 */
    pmu_set_register_value(PMU_RG_BC11_IPD_EN_ADDR, PMU_RG_BC11_IPD_EN_MASK, PMU_RG_BC11_IPD_EN_SHIFT, 0x0);
    /* RG_bc11_BIAS_EN=0 */
    pmu_set_register_value(PMU_RG_BC11_BIAS_EN_ADDR, PMU_RG_BC11_BIAS_EN_MASK, PMU_RG_BC11_BIAS_EN_SHIFT, 0x0);

    pmu_ctrl_driver_power(DRIVER_USB, PMIC_CTL_DISABLE); /* Turn off 3.3V phy power */

}

static hal_charger_type_t hw_charging_get_charger_type(void)
{
    hal_charger_type_t charger_type = HAL_CHARGER_TYPE_UNKNOWN;

    log_hal_debug("hw_charging_get_charger_type\n");

    /********* Step initial  ***************/
    hw_bc11_init();

    /********* Step DCD ***************/
    if (1 == hw_bc11_DCD()) {
        /********* Step A1 ***************/
        log_hal_debug("step A1 : Non STANDARD CHARGER!\n");
        charger_type = HAL_CHARGER_TYPE_NONSTANDARD_CHARGER;
    } else {
        /********* Step A2 ***************/
        if (1 == hw_bc11_stepA2()) {
            /********* Step B2 ***************/
            if (1 == hw_bc11_stepB2()) {
                log_hal_debug("step B2 : STANDARD CHARGER!\n");
                charger_type = HAL_CHARGER_TYPE_STANDARD_CHARGER;
            } else {
                log_hal_debug("step B2 : Charging Host!\n");
                charger_type = HAL_CHARGER_TYPE_CHARGING_HOST;
            }
        } else {
            charger_type = HAL_CHARGER_TYPE_STANDARD_HOST;
            log_hal_debug("step A2 : Standard USB Host!\n");
        }
    }

    /********* Finally setting *******************************/
    hw_bc11_done();

    return charger_type;

}

hal_charger_status_t hal_charger_get_charger_type(hal_charger_type_t *type)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;
    hal_charger_type_t charger_type = HAL_CHARGER_TYPE_UNKNOWN;

    log_hal_debug("hal_charger_get_charger_type\n");

    charger_type = hw_charging_get_charger_type();
    log_hal_debug("charging_get_charger_type = %d\r\n", charger_type);

    *type = charger_type;

    return status;
}

hal_charger_status_t hal_charger_init(void)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;

    log_hal_debug("hal_charger_init\n");

    /* Calibration the CV threshold mapping table */
    /* This function is used for FT CV threshold trimming and not for customer's fine tuning */
    calibration_cv_threshold_table();

    status = charging_hw_init();

    return status;
}

hal_charger_status_t hal_charger_meter_init(void)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;

    log_hal_debug("hal_charger_meter_init\n");

#if !(defined(ZCV_ENABLE) && (ZCV_ENABLE == 0))
    static bool hw_zcv_init = false;
    if (hw_zcv_init == false) {
        /* unlock key */
        pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR , PMU_RG_VRC_CFG_KEY0_MASK   , PMU_RG_VRC_CFG_KEY0_SHIFT  , 0xFC);
        pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR , PMU_RG_VRC_CFG_KEY1_MASK   , PMU_RG_VRC_CFG_KEY1_SHIFT  , 0xDA);

        pmu_set_register_value(PMU_RG_WK_ZCV_EN_ADDR, PMU_RG_WK_ZCV_EN_MASK, PMU_RG_WK_ZCV_EN_SHIFT, 1);
        /* lock key */
        pmu_set_register_value(PMU_RG_VRC_CFG_KEY0_ADDR , PMU_RG_VRC_CFG_KEY0_MASK   , PMU_RG_VRC_CFG_KEY0_SHIFT  , 0x0);
        pmu_set_register_value(PMU_RG_VRC_CFG_KEY1_ADDR , PMU_RG_VRC_CFG_KEY1_MASK   , PMU_RG_VRC_CFG_KEY1_SHIFT  , 0x0);

        hw_zcv_init = true;
    }
#endif

    pmu_set_register_value(PMU_BATON_TDET_EN_ADDR, PMU_BATON_TDET_EN_MASK, PMU_BATON_TDET_EN_SHIFT, 1);     /* BATON_TDET_EN */

    return status;
}

hal_charger_status_t hal_charger_meter_get_battery_voltage_sense(int32_t *battery_voltage)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;

    log_hal_debug("hal_charger_meter_get_battery_voltage_sense\n");

    *battery_voltage = pmu_auxadc_get_channel_value(PMU_AUX_BATSNS, *battery_voltage, 1);

    return status;

}

hal_charger_status_t hal_charger_meter_get_voltage_current_sense(int32_t *sense_voltage)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;

    log_hal_debug("hal_charger_meter_get_voltage_current_sense\n");

    *sense_voltage = pmu_auxadc_get_channel_value(PMU_AUX_ISENSE, *sense_voltage, 1);

    return status;
}

static int32_t BattThermistorConverTemp(int32_t Res)
{
    int32_t i = 0;
    int32_t RES1 = 0, RES2 = 0;
    int32_t TBatt_Value = -200, TMP1 = 0, TMP2 = 0;

    if (Res >= Batt_Temperature_Table[0].TemperatureR) {
        TBatt_Value = -20;
    } else if (Res <= Batt_Temperature_Table[16].TemperatureR) {
        TBatt_Value = 60;
    } else {
        RES1 = Batt_Temperature_Table[0].TemperatureR;
        TMP1 = Batt_Temperature_Table[0].BatteryTemp;

        for (i = 0; i <= 16; i++) {
            if (Res >= Batt_Temperature_Table[i].TemperatureR) {
                RES2 = Batt_Temperature_Table[i].TemperatureR;
                TMP2 = Batt_Temperature_Table[i].BatteryTemp;
                break;
            } else {
                RES1 = Batt_Temperature_Table[i].TemperatureR;
                TMP1 = Batt_Temperature_Table[i].BatteryTemp;
            }
        }

        TBatt_Value = (((Res - RES2) * TMP1) + ((RES1 - Res) * TMP2)) / (RES1 - RES2);
    }

    return TBatt_Value;
}

static int32_t BattVoltToTemp(int32_t dwVolt)
{
    int64_t TRes;
    int32_t sBaTTMP = -100;
    lldiv_t rel_temp;

    TRes = (RBAT_PULL_UP_R * (int64_t) dwVolt);
    /*lldiv for  64bit/32bit division*/
    rel_temp = lldiv(TRes, abs(RBAT_PULL_UP_VOLT - dwVolt));
    TRes = rel_temp.quot;

    /* convert resistance to temperature */
    sBaTTMP = (int32_t)BattThermistorConverTemp((int32_t)TRes);

    return sBaTTMP;
}


hal_charger_status_t hal_charger_meter_get_battery_temperature(int32_t *battery_temperature)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;
    int32_t battery_temperature_volt = 0;
    log_hal_debug("hal_charger_meter_get_battery_temperature\n");

    battery_temperature_volt = pmu_auxadc_get_channel_value(PMU_AUX_BATON, battery_temperature_volt, 1);

    if (battery_temperature_volt != 0) {

        *battery_temperature = BattVoltToTemp(battery_temperature_volt);
    } else {
        status =  HAL_CHARGER_STATUS_ERROR;
        log_hal_error("BATON(NTC) voltage error!!battery_temperature_volt = %d", battery_temperature_volt);
    }

    return status;
}

hal_charger_status_t hal_charger_meter_get_charger_voltage(int32_t *charger_voltage)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;
    int32_t val;

    log_hal_debug("hal_charger_meter_get_charger_voltage\n");

    val = pmu_auxadc_get_channel_value(PMU_AUX_VCDT, *charger_voltage, 1);
    val = (((R_CHARGER_1 + R_CHARGER_2) * 100 * val) / R_CHARGER_2) / 100;

    *charger_voltage = val;

    return status;
}

static int32_t get_hw_ocv(void)
{

    int32_t adc_result = 0;
#if !(defined(ZCV_ENABLE) && (ZCV_ENABLE == 0))
    adc_result = pmu_auxadc_get_channel_value(PMU_AUX_WAKEUP_PCHR, adc_result, 1);
#else
    /* If not support ZCV ,use the battery voltage */
    adc_result = pmu_auxadc_get_channel_value(PMU_AUX_BATSNS, adc_result, 1);
#endif
    return adc_result;
}

hal_charger_status_t hal_charger_meter_get_hardware_open_circuit_voltage(int32_t *open_circuit_voltage)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;

    log_hal_debug("hal_charger_meter_get_hardware_open_circuit_voltage\n");

    *open_circuit_voltage = get_hw_ocv();

    return status;
}

hal_charger_status_t hal_charger_meter_get_charging_current(int32_t *charging_current)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;
    int32_t battery_voltage = 0;
    int32_t sense_voltage = 0;
    bool    charger_detect_status = false;
    int32_t ret = 0;

    log_hal_debug("hal_charger_meter_get_charging_current\n");

    ret = hal_charger_get_charger_detect_status(&charger_detect_status);

    if (ret != HAL_CHARGER_STATUS_OK) {
        status = HAL_CHARGER_STATUS_ERROR;
        return status;
    }

    if (charger_detect_status == false) {
        *charging_current = 0;
        return status;
    }

    ret = hal_charger_meter_get_battery_voltage_sense(&battery_voltage);
    if (ret != HAL_CHARGER_STATUS_OK) {
        status = HAL_CHARGER_STATUS_ERROR;
        return status;
    }

    ret = hal_charger_meter_get_voltage_current_sense(&sense_voltage);
    if (ret != HAL_CHARGER_STATUS_OK) {
        status = HAL_CHARGER_STATUS_ERROR;
        return status;
    }

    if (sense_voltage > battery_voltage) {
        *charging_current = (sense_voltage - battery_voltage) * 1000 / CUST_R_SENSE;
    } else {
        *charging_current = 0;
    }

    return status;
}

static int32_t detect_battery_plug_out_status(void)
{
    int32_t plug_out;

    plug_out = pmu_get_register_value(PMU_RG_POR_FLAG_ADDR, PMU_RG_POR_FLAG_MASK, PMU_RG_POR_FLAG_SHIFT);
    pmu_set_register_value(PMU_RG_POR_FLAG_ADDR, PMU_RG_POR_FLAG_MASK, PMU_RG_POR_FLAG_SHIFT, 1);

    log_hal_debug("plug_out =%d\n", plug_out);

    if (plug_out == 0) {
        return 1;
    } else {
        return 0;
    }
}

static int32_t get_battery_plug_out_status(void)
{
    return detect_battery_plug_out_status();
}

hal_charger_status_t hal_charger_meter_get_battery_plug_out_status(int32_t *battery_status)
{
    hal_charger_status_t status = HAL_CHARGER_STATUS_OK;
    log_hal_debug("hal_charger_meter_get_battery_plug_out_status\n");
    *battery_status = get_battery_plug_out_status();

    return status;
}

#endif    /*HAL_CHARGER_MODULE_ENABLED*/

