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

#ifndef __HAL_CHARGER_H__
#define __HAL_CHARGER_H__

#include "hal_platform.h"

#ifdef HAL_CHARGER_MODULE_ENABLED

/**
* @addtogroup HAL
* @{
* @addtogroup CHARGER
* @{
* This document mainly introduce all details about the Charger APIs including terms and acronym,
* supported features, software architecture, details on how to use this driver, all enums, structures, typedefs and functions.
*
* @section HAL_Charger_Terms_Chapter Terms and acronyms
*
* |Terms                   |Details                                                                 |
* |------------------------------|------------------------------------------------------------------------|
* |\b ADC                        | ADC is an Analog-to-Digital Converter that converts a continuous physical quantity (usually voltage) to a digital number that represents the quantity's amplitude.|
* |\b OCV                        | The Open Circuit Voltage (OCV) is voltage of a device when disconnected from any circuit.|
* |\b PMU                        | The Power Management Unit (PMU) is a microcontroller that governs power functions of digital platforms.|
* |\b AUXADC                     | Auxiliary ADC|
*
* @section HAL_CHARGER_Features_Chapter Supported features
*
* The charger controller senses the charger input voltage from either a standard AC-DC adaptor or an USB charger.
* When the charger input voltage is within a pre-determined range, the charging process will be activated.
* This detector resists higher input voltages than other parts of PMU.
*   \n \n
* - \b Charger \b Detection \n
* Whenever an invalid charging source is detected (> 7.0 V), the charger detector stops the charging process immediately
* to avoid burning out the chip or the device. In addition, if the charger-in level is not high enough (< 4.3V),
* the charger will also be disabled to avoid improper charging behavior.
*   \n \n
* - \b Charging  \b Control \n
* When the charger is active, the charger controller manages the charging phase according to the battery status.
* During the charging period, the battery voltage is constantly monitored. The battery charger supports pre-charge mode
* (VBAT < 3.2V, PMU power-off state), CC mode (constant current mode or fast charging mode at the range 3.2V < VBAT < 4.2V)
* and CV mode (constant voltage mode) to optimize the charging procedure for Li-ion battery.
*
*
* @section HAL_Charger_Driver_Usage_Chapter How to use this driver
*
* - \b Using \b Charger. \n
*  The steps are shown below:
*  - Step1: Call #hal_charger_init() to initialization charger hardware setup.
*  - Step2: Call #hal_charger_set_charging_current() to set charging current.
*  - Step3: Call #hal_charger_set_constant_voltage() to set constant voltage charging mode voltage.
*  - Step4: Call #hal_charger_set_high_voltage_threshold() to set high voltage threshold, this is set as default and the configuration is optional.
*  - Step5: Call #hal_charger_enable() to enable charging
*  - Step6: Call #hal_charger_reset_watchdog_timer() to reset watchdog timer before watchdog timeout.
*  - sample code:
*    @code
*    {
        hal_charger_init();
        hal_charger_set_charging_current(HAL_CHARGE_CURRENT_450_MA);
        hal_charger_set_constant_voltage(HAL_BATTERY_VOLT_04_2000_V);
        hal_charger_set_high_voltage_threshold(HAL_BATTERY_VOLT_07_0000_V);
        hal_charger_enable(true);
        hal_charger_reset_watchdog_timer();
     }
*    @endcode
*/


#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup hal_charger_enum Enum
  * @{
  */


/** @brief charger status*/
typedef enum {
    HAL_CHARGER_STATUS_INVALID_PARAMETER = -2,  /**< CHARGER invalid parameter */
    HAL_CHARGER_STATUS_ERROR = -1,              /**< CHARGER function ERROR */
    HAL_CHARGER_STATUS_OK = 0                   /**< CHARGER function OK */
} hal_charger_status_t;

/** @brief charger type */
typedef enum {
    HAL_CHARGER_TYPE_UNKNOWN = 0,               /**< UNKNOWN */
    HAL_CHARGER_TYPE_STANDARD_HOST,             /**< USB */
    HAL_CHARGER_TYPE_NONSTANDARD_CHARGER,       /**< Non-standard AC charger */
    HAL_CHARGER_TYPE_STANDARD_CHARGER,          /**< Standard AC charger */
    HAL_CHARGER_TYPE_CHARGING_HOST              /**< Charging host */
} hal_charger_type_t;

/** @brief Voltage List */
typedef enum {
    HAL_BATTERY_VOLT_03_5000_V = 35000,         /**< define voltage as  3500 mV */
    HAL_BATTERY_VOLT_03_6000_V = 36000,         /**< define voltage as  3600 mV */
    HAL_BATTERY_VOLT_03_7000_V = 37000,         /**< define voltage as  3700 mV */
    HAL_BATTERY_VOLT_03_7750_V = 37750,         /**< define voltage as  3775 mV */
    HAL_BATTERY_VOLT_03_8000_V = 38000,         /**< define voltage as  3800 mV */
    HAL_BATTERY_VOLT_03_8500_V = 38500,         /**< define voltage as  3850 mV */
    HAL_BATTERY_VOLT_03_9000_V = 39000,         /**< define voltage as  3900 mV */
    HAL_BATTERY_VOLT_04_0000_V = 40000,         /**< define voltage as  4000 mV */
    HAL_BATTERY_VOLT_04_0500_V = 40500,         /**< define voltage as  4050 mV */
    HAL_BATTERY_VOLT_04_1000_V = 41000,         /**< define voltage as  4100 mV */
    HAL_BATTERY_VOLT_04_1250_V = 41250,         /**< define voltage as  4125 mV */
    HAL_BATTERY_VOLT_04_1375_V = 41375,         /**< define voltage as  4137.5 mV */
    HAL_BATTERY_VOLT_04_1500_V = 41500,         /**< define voltage as  4150 mV */
    HAL_BATTERY_VOLT_04_1625_V = 41625,         /**< define voltage as  4162.5 mV */
    HAL_BATTERY_VOLT_04_1750_V = 41750,         /**< define voltage as  4175 mV */
    HAL_BATTERY_VOLT_04_1875_V = 41875,         /**< define voltage as  4187.5 mV */
    HAL_BATTERY_VOLT_04_2000_V = 42000,         /**< define voltage as  4200 mV */
    HAL_BATTERY_VOLT_04_2125_V = 42125,         /**< define voltage as  4212 mV */
    HAL_BATTERY_VOLT_04_2250_V = 42250,         /**< define voltage as  4225 mV */
    HAL_BATTERY_VOLT_04_2375_V = 42375,         /**< define voltage as  4237.5 mV */
    HAL_BATTERY_VOLT_04_2500_V = 42500,         /**< define voltage as  4250 mV */
    HAL_BATTERY_VOLT_04_2625_V = 42625,         /**< define voltage as  4262.5 mV */
    HAL_BATTERY_VOLT_04_2750_V = 42750,         /**< define voltage as  4275 mV */
    HAL_BATTERY_VOLT_04_2875_V = 42875,         /**< define voltage as  4287.5 mV */
    HAL_BATTERY_VOLT_04_3000_V = 43000,         /**< define voltage as  4300 mV */
    HAL_BATTERY_VOLT_04_3125_V = 43125,         /**< define voltage as  4312.5 mV */
    HAL_BATTERY_VOLT_04_3250_V = 43250,         /**< define voltage as  4325 mV */
    HAL_BATTERY_VOLT_04_3375_V = 43375,         /**< define voltage as  4337.5 mV */
    HAL_BATTERY_VOLT_04_3500_V = 43500,         /**< define voltage as  4350 mV */
    HAL_BATTERY_VOLT_04_3625_V = 43625,         /**< define voltage as  4362.5 mV */
    HAL_BATTERY_VOLT_04_3750_V = 43750,         /**< define voltage as  4375 mV */
    HAL_BATTERY_VOLT_04_3875_V = 43875,         /**< define voltage as  4387.5 mV */
    HAL_BATTERY_VOLT_04_4000_V = 44000,         /**< define voltage as  4400 mV */
    HAL_BATTERY_VOLT_04_4125_V = 44125,         /**< define voltage as  4412.5 mV */
    HAL_BATTERY_VOLT_04_4375_V = 44375,         /**< define voltage as  4437.5 mV */
    HAL_BATTERY_VOLT_04_4250_V = 44250,         /**< define voltage as  4425 mV */
    HAL_BATTERY_VOLT_04_4500_V = 44500,         /**< define voltage as  4450 mV */
    HAL_BATTERY_VOLT_04_4625_V = 44625,         /**< define voltage as  4462.5 mV */
    HAL_BATTERY_VOLT_04_4750_V = 44750,         /**< define voltage as  4475 mV */
    HAL_BATTERY_VOLT_04_4875_V = 44875,         /**< define voltage as  4487.5 mV */
    HAL_BATTERY_VOLT_04_5000_V = 45000,         /**< define voltage as  4500 mV */
    HAL_BATTERY_VOLT_04_5500_V = 45500,         /**< define voltage as  4550 mV */
    HAL_BATTERY_VOLT_04_6000_V = 46000,         /**< define voltage as  4600 mV */
    HAL_BATTERY_VOLT_06_0000_V = 60000,         /**< define voltage as  6000 mV */
    HAL_BATTERY_VOLT_06_5000_V = 65000,         /**< define voltage as  6500 mV */
    HAL_BATTERY_VOLT_07_0000_V = 70000,         /**< define voltage as  7000 mV */
    HAL_BATTERY_VOLT_07_5000_V = 75000,         /**< define voltage as  7500 mV */
    HAL_BATTERY_VOLT_08_5000_V = 85000,         /**< define voltage as  8500 mV */
    HAL_BATTERY_VOLT_09_5000_V = 95000,         /**< define voltage as  9500 mV */
    HAL_BATTERY_VOLT_10_5000_V = 105000,        /**< define voltage as 10500 mV */
    HAL_BATTERY_VOLT_MAX,
    HAL_BATTERY_VOLT_INVALID
} HAL_BATTERY_VOLTAGE_ENUM;

/** @brief Charger current List */
typedef enum {
    HAL_CHARGE_CURRENT_0_MA    = 0    , /**< define current as  0       mA */
    HAL_CHARGE_CURRENT_5_MA    = 5    , /**< define current as  5       mA */
    HAL_CHARGE_CURRENT_10_MA   = 10   , /**< define current as  10      mA */
    HAL_CHARGE_CURRENT_15_MA   = 15   , /**< define current as  15      mA */
    HAL_CHARGE_CURRENT_20_MA   = 20   , /**< define current as  20      mA */
    HAL_CHARGE_CURRENT_25_MA   = 25   , /**< define current as  25      mA */
    HAL_CHARGE_CURRENT_30_MA   = 30   , /**< define current as  30      mA */
    HAL_CHARGE_CURRENT_35_MA   = 35   , /**< define current as  35      mA */
    HAL_CHARGE_CURRENT_40_MA   = 40   , /**< define current as  40      mA */
    HAL_CHARGE_CURRENT_45_MA   = 45   , /**< define current as  45      mA */
    HAL_CHARGE_CURRENT_50_MA   = 50   , /**< define current as  50      mA */
    HAL_CHARGE_CURRENT_55_MA   = 55   , /**< define current as  55      mA */
    HAL_CHARGE_CURRENT_60_MA   = 60   , /**< define current as  60      mA */
    HAL_CHARGE_CURRENT_65_MA   = 65   , /**< define current as  65      mA */
    HAL_CHARGE_CURRENT_70_MA   = 70   , /**< define current as  70      mA */
    HAL_CHARGE_CURRENT_75_MA   = 75   , /**< define current as  75      mA */
    HAL_CHARGE_CURRENT_80_MA   = 80   , /**< define current as  80      mA */
    HAL_CHARGE_CURRENT_85_MA   = 85   , /**< define current as  85      mA */
    HAL_CHARGE_CURRENT_90_MA   = 90   , /**< define current as  90      mA */
    HAL_CHARGE_CURRENT_95_MA   = 95   , /**< define current as  95      mA */
    HAL_CHARGE_CURRENT_100_MA  = 100  , /**< define current as  100     mA */
    HAL_CHARGE_CURRENT_105_MA  = 105  , /**< define current as  105     mA */
    HAL_CHARGE_CURRENT_110_MA  = 110  , /**< define current as  110     mA */
    HAL_CHARGE_CURRENT_115_MA  = 115  , /**< define current as  115     mA */
    HAL_CHARGE_CURRENT_120_MA  = 120  , /**< define current as  120     mA */
    HAL_CHARGE_CURRENT_125_MA  = 125  , /**< define current as  125     mA */
    HAL_CHARGE_CURRENT_130_MA  = 130  , /**< define current as  130     mA */
    HAL_CHARGE_CURRENT_135_MA  = 135  , /**< define current as  135     mA */
    HAL_CHARGE_CURRENT_140_MA  = 140  , /**< define current as  140     mA */
    HAL_CHARGE_CURRENT_145_MA  = 145  , /**< define current as  145     mA */
    HAL_CHARGE_CURRENT_150_MA  = 150  , /**< define current as  150     mA */
    HAL_CHARGE_CURRENT_160_MA  = 160  , /**< define current as  160     mA */
    HAL_CHARGE_CURRENT_170_MA  = 170  , /**< define current as  170     mA */
    HAL_CHARGE_CURRENT_180_MA  = 180  , /**< define current as  180     mA */
    HAL_CHARGE_CURRENT_190_MA  = 190  , /**< define current as  190     mA */
    HAL_CHARGE_CURRENT_200_MA  = 200  , /**< define current as  200     mA */
    HAL_CHARGE_CURRENT_210_MA  = 210  , /**< define current as  210     mA */
    HAL_CHARGE_CURRENT_220_MA  = 220  , /**< define current as  220     mA */
    HAL_CHARGE_CURRENT_230_MA  = 230  , /**< define current as  230     mA */
    HAL_CHARGE_CURRENT_240_MA  = 240  , /**< define current as  240     mA */
    HAL_CHARGE_CURRENT_250_MA  = 250  , /**< define current as  250     mA */
    HAL_CHARGE_CURRENT_260_MA  = 260  , /**< define current as  260     mA */
    HAL_CHARGE_CURRENT_270_MA  = 270  , /**< define current as  270     mA */
    HAL_CHARGE_CURRENT_280_MA  = 280  , /**< define current as  280     mA */
    HAL_CHARGE_CURRENT_290_MA  = 290  , /**< define current as  290     mA */
    HAL_CHARGE_CURRENT_300_MA  = 300  , /**< define current as  300     mA */
    HAL_CHARGE_CURRENT_320_MA  = 320  , /**< define current as  320     mA */
    HAL_CHARGE_CURRENT_340_MA  = 340  , /**< define current as  340     mA */
    HAL_CHARGE_CURRENT_360_MA  = 360  , /**< define current as  360     mA */
    HAL_CHARGE_CURRENT_380_MA  = 380  , /**< define current as  380     mA */
    HAL_CHARGE_CURRENT_400_MA  = 400  , /**< define current as  400     mA */
    HAL_CHARGE_CURRENT_420_MA  = 420  , /**< define current as  420     mA */
    HAL_CHARGE_CURRENT_440_MA  = 440  , /**< define current as  440     mA */
    HAL_CHARGE_CURRENT_460_MA  = 460  , /**< define current as  460     mA */
    HAL_CHARGE_CURRENT_480_MA  = 480  , /**< define current as  480     mA */
    HAL_CHARGE_CURRENT_500_MA  = 500  , /**< define current as  500     mA */
    HAL_CHARGE_CURRENT_550_MA  = 550  , /**< define current as  550     mA */
    HAL_CHARGE_CURRENT_600_MA  = 600  , /**< define current as  600     mA */
    HAL_CHARGE_CURRENT_650_MA  = 650  , /**< define current as  650     mA */
    HAL_CHARGE_CURRENT_700_MA  = 700  , /**< define current as  700     mA */
    HAL_CHARGE_CURRENT_750_MA  = 750  , /**< define current as  750     mA */
    HAL_CHARGE_CURRENT_800_MA  = 800  , /**< define current as  800     mA */
    HAL_CHARGE_CURRENT_850_MA  = 850  , /**< define current as  850     mA */
    HAL_CHARGE_CURRENT_900_MA  = 900  , /**< define current as  900     mA */
    HAL_CHARGE_CURRENT_950_MA  = 950  , /**< define current as  950     mA */
    HAL_CHARGE_CURRENT_1000_MA = 1000 , /**< define current as  1000    mA */
    HAL_CHARGE_CURRENT_1100_MA = 1100 , /**< define current as  1100    mA */
    HAL_CHARGE_CURRENT_1200_MA = 1200 , /**< define current as  1200    mA */
    HAL_CHARGE_CURRENT_1300_MA = 1300 , /**< define current as  1300    mA */
    HAL_CHARGE_CURRENT_1400_MA = 1400 , /**< define current as  1400    mA */
    HAL_CHARGE_CURRENT_1500_MA = 1500 , /**< define current as  1500    mA */
    HAL_CHARGE_CURRENT_1600_MA = 1600 , /**< define current as  1600    mA */
    HAL_CHARGE_CURRENT_MAX
} HAL_CHARGE_CURRENT_ENUM;

/**
  * @}
  */


/*************************************
*         Interface proto
**************************************/

/**
 * @brief     This function is used to initializing charger hardware with basic function.
 * @return
 *                #HAL_CHARGER_STATUS_OK if OK. \n
 *                #HAL_CHARGER_STATUS_ERROR if charger hardware initializing fail. \n
 * @par       Example
 * Sample code please refer to @ref HAL_Charger_Driver_Usage_Chapter
 */
hal_charger_status_t hal_charger_init(void);

/**
 * @brief     This function is used to initializing charger meter hardware with basic function.
 * @return
 *                #HAL_CHARGER_STATUS_OK if OK. \n
 *                #HAL_CHARGER_STATUS_ERROR if charger meter hardware initializing fail. \n
 */
hal_charger_status_t hal_charger_meter_init(void);

/**
 * @brief     This function is used to set charger enable/disable.
 * @param[in] enable is a flag to set the charging on/off
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if Charger enable/disable fail. \n
 * @par       Example
 * Sample code please refer to @ref HAL_Charger_Driver_Usage_Chapter
 */
hal_charger_status_t hal_charger_enable(bool enable);

/**
 * @brief     This function is used to set voltage on constant voltage mode.
 * @param[in] voltage is constant voltage mode voltage. The unit is 0.1 millivolt (0.1 mV).
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if set constant voltage mode voltage fail. \n
 *                 #HAL_CHARGER_STATUS_INVALID_PARAMETER if constant voltage parameter is invalid. \n
 * @par       Example
 * Sample code please refer to @ref HAL_Charger_Driver_Usage_Chapter
 */
hal_charger_status_t hal_charger_set_constant_voltage(uint32_t voltage);


/**
 * @brief     This function is used to set charging current.
 * @param[in] current is charging current. The unit is milliamps (mA).
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if set charging current fail. \n
 * @par       Example
 * @code      hal_charger_set_charging_current(HAL_CHARGE_CURRENT_300_MA);
 * @endcode
 */
hal_charger_status_t hal_charger_set_charging_current(uint32_t current);

/**
 * @brief     This function is used to reset PMU watchdog timer.
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if rest PMU watchdog timer fail. \n
 * @note    If watchdog timeout, the charging will be force disable.
 *          The watchdog timer default is 2 second.
 */
hal_charger_status_t hal_charger_reset_watchdog_timer(void);

/**
 * @brief     This function is used to set charger input high voltage threshold voltage.
 * @param[in] voltage is charger input high voltage threshold voltage. The unit is 0.1 millivolt (0.1 mV).
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if set charger input high voltage threshold voltage fail. \n
 * @par       Example
 * @code      hal_charger_set_high_voltage_threshold(HAL_BATTERY_VOLT_07_5000_V);
 * @endcode
 */
hal_charger_status_t hal_charger_set_high_voltage_threshold(uint32_t voltage);

/**
 * @brief      This function is used to get over-voltage status on charger input.
 * @param[out] high_voltage_status.TRUE: charger input voltage > high voltage threshold voltage;FALSE: charger input voltage < high voltage threshold voltage.
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if get charger input high voltage detect status fail. \n
 */
hal_charger_status_t hal_charger_get_high_voltage_status(bool *high_voltage_status);

/**
 * @brief      This function is used to get battery status.
 * @param[out] battery_status is used to indicate the battery exist status.
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if get get battery status fail. \n
 */
hal_charger_status_t hal_charger_get_battery_status(bool *battery_status);

/**
 * @brief      This function is used to get charger detect status.
 * @param[out] charger_status is used to indicate the charger exist status.
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if get charger detect status fail. \n
 */
hal_charger_status_t hal_charger_get_charger_detect_status(bool *charger_status);

/**
 * @brief      This function is used to get charger type.
 * @param[out] type is the charger detect type, such as HAL_CHARGER_TYPE_STANDARD_HOST,HAL_CHARGER_TYPE_STANDARD_CHARGER
 * , please refer to #hal_charger_type_t.
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if get charger detect type fail. \n
 */
hal_charger_status_t hal_charger_get_charger_type(hal_charger_type_t *type);

/**
 * @brief      This function is used to get AUXADC battery voltage.The unit is millivolt.
 * @param[out] battery_voltage is get AUXADC battery voltage.
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if get AUXADC battery voltage fail. \n
 */
hal_charger_status_t hal_charger_meter_get_battery_voltage_sense(int32_t *battery_voltage);

/**
 * @brief      This function is used to get AUXADC voltage current sense value.The unit is millivolt.
 * @param[out] sense_voltage is get AUXADC voltage current sense value.
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if get AUXADC voltage current sense value fail. \n
 */
hal_charger_status_t hal_charger_meter_get_voltage_current_sense(int32_t *sense_voltage);

/**
 * @brief      This function is used to get AUXADC battery temperature.The unit is celsius degrees.
 * @param[out] battery_temperature is used to get AUXADC battery temperature.
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if get AUXADC battery temperature fail. \n
 */
hal_charger_status_t hal_charger_meter_get_battery_temperature(int32_t *battery_temperature);

/**
 * @brief      This function is used to get AUXADC charger voltage sense value.The unit is millivolt.
 * @param[out] charger_voltage is used to get AUXADC charger voltage.
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if get AUXADC charger voltage fail. \n
 */
hal_charger_status_t hal_charger_meter_get_charger_voltage(int32_t *charger_voltage);

/**
 * @brief      This function is used to get hardware open circuit voltage (OCV).The unit is millivolt.
 * @param[out] open_circuit_voltage is used to get hardware open circuit voltage.
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if get hardware open circuit voltage fail. \n
 * @note    Open circuit voltage is measure after the battery is disconnect from any load or power source.
 */
hal_charger_status_t hal_charger_meter_get_hardware_open_circuit_voltage(int32_t *open_circuit_voltage);

/**
 * @brief      This function is used to get battery charging current.
 * @param[out] charging_current is used to get battery charging current. The unit is milliamps (mA).
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if get battery charging current fail. \n
 */
hal_charger_status_t hal_charger_meter_get_charging_current(int32_t *charging_current);

/**
 * @brief      This function is used to get battery plug out status.
 * @param[out] battery_status is battery plug out status; 0:battery have not been plug out ,1:battery ever plug out.
 * @return         #HAL_CHARGER_STATUS_OK if OK. \n
 *                 #HAL_CHARGER_STATUS_ERROR if get battery plug out status fail. \n
 */
hal_charger_status_t hal_charger_meter_get_battery_plug_out_status(int32_t *battery_status);


#ifdef __cplusplus
}
#endif
/**
* @}
* @}
*/
#endif /*HAL_CHARGER_MODULE_ENABLED*/
#endif /*__HAL_CHARGER_H__*/

