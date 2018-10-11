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

#ifndef DEMOS_VSM_DRIVER_H_
#define DEMOS_VSM_DRIVER_H_
/**
 * @addtogroup API
 * @{
 * 
 * @addtogroup VSM
 * @{
 * This section introduces the VSM driver APIs including terms and acronyms, supported features, software architecture, 
 * details on how to use this driver, the VSM function groups, enums, structures and functions.
 *
 * @section Terms_Chapter Terms and acronyms
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b VSM                        | Vital signs (or vitals) are used to measure the body's basic functions. For more information, please check <a href="https://en.wikipedia.org/wiki/Vital_signs"> Vital signs in wiki </a>.|
 * |\b MT2511                     | MT2511 is a 2-in-1 bio-sensing AFE (analog front-end) to facilitate bio-signal acquisition. The AFE comprises of low noise voltage and current sensing channels and is capable of sensing EKG (Electrocardiography) and PPG (Photoplethysmography) simultaneously.|
 * |\b EKG                        | Electrocardiography (ECG or EKG) is the process of recording the electrical activity of the heart over a period of time using electrodes placed on a patient's body.|
 * |\b EEG                        | Electroencephalography (EEG) is an electrophysiological monitoring method to record electrical activity of the brain.|
 * |\b EMG                        | Electromyography (EMG) is an electrodiagnostic medicine technique for evaluating and recording the electrical activity produced by skeletal muscles.|
 * |\b GSR                        | Galvanic skin response (GSR) is an electrophysiological monitoring method to record conductance of the skin.|
 * |\b PPG                        | Photoplethysmogram (PPG) is an optical volumetric measurement of an organ. By illuminating skin with LED, the PPG is measured from the light transmitted or reflected to photodiode.|
 * |\b BISI                       | Beat interval (BI) and signal integrity (SI) detection. The BI is computed based on subtracting the peak position of the successive PPG beats. The SI is an indicator to identify the PPG signal integrity.|
 * |\b TIA                        | Trans-impedance amplifier (TIA) converts the input photodiode current into an appropriate voltage.|
 * |\b AMBDAC                     | Ambient cancellation digital-to-analog converter (AMBDAC) is a kind of Ambient Cancellation Scheme, the receiver provides digital samples corresponding to ambient duration. The host processor (external to the AFE) can use these ambient values to estimate the amount of ambient light leakage.|
 * |\b PGA                        | Programmable gain amplifier (PGA) is an electronic amplifier (typically an operational amplifier) whose gain can be controlled by external digital or analog signals.|
 *
 * @section Features_Chapter Supported features
 *
 * - \b EKG \b Channel. \n
 *   MT2511 integrates a high-resolution DC-coupled front end. It can endure wide input offset, large interference and different electrode impedance. Besides right leg drive mode, it supports a two-electrode mode for reducing electrode cost. Note that there is larger interference during the two-electrode mode. \n
 *   \n
 * - \b PPG \b Channel. \n
 *   MT2511 consists of LED drivers, a high-end DC-coupled current-mode front end and a flexible timing control for the PPG channel. The flexible timing control enables the users to control the device timing for different applications and to power down the device for power saving operations. The PPG front-end can sense up to four channels (two for signal and two for ambient) by time multiplexing. \n
 *
 * @section Architecture_Chapter Software architecture of the VSM
 *
 * The diagram below shows the software architecture of the VSM. \n
 * - Call #vsm_driver_init() to initialize the hardware. After the VSM is initialized, software needs to call
 *  #vsm_driver_set_signal() to set the biosignals to be sensed by the VSM. Call #vsm_driver_read_sram() to get sensed results.
 *  This architecture is extensible, it can be modified to support other VSM.
 * @image html biosensor_architecture_image_name.png
 *
 * @section Driver_Usage_Chapter How to use this driver
 *
 * - Use VSM to read bio data. \n
 *  - step1: Call #vsm_driver_init() to intialize the VSM hardware.
 *  - step2: Call #vsm_driver_set_signal() to set the VSM sensing signal.
 *  - step3: Call #vsm_driver_read_sram() to read the VSM data from the SRAM.
 *  - step4: Call #vsm_driver_deinit() to de-initialize VSM hardware, if it is no longer in use.
 *  - Sample code:
 *    @code
 *       uint32_t data_buf[VSM_SRAM_LEN];
 *       int32_t len;
 *       ret = vsm_driver_init();//Initialize the VSM hardware to power on the VSM.
 *       if (ret == VSM_STATUS_OK) {
 *           ret = vsm_driver_set_signal(VSM_SIGNAL_EKG); //Set the EKG as VSM sensing signal.
 *           if (VSM_STATUS_OK != ret) {
 *                //error handle
 *           } else {
 *               vsm_driver_read_sram(VSM_SRAM_EKG, data_buf, &len); //Places the SRAM data to the data_buf with len length.
 *               //clear this signal to save power when not using it.
 *               vsm_driver_clear_signal(VSM_SIGNAL_EKG);
 *           }
 *       }
 *
 *       //De-initialize the VSM hardware.
 *       vsm_driver_deinit();
 *    @endcode
 *
 * @section Function_Group_Chapter VSM function groups
 *
 * The APIs are grouped by functionality, for an easier use.
 * - The VSM hardware initialize/de-initialize functions. \n The #vsm_driver_init() function initializes and powers on the VSM.
 *   Call the #vsm_driver_init() API before selecting the sensing signals. The #vsm_driver_deinit() disables the VSM hardware
 *   The functions are:
 *  - #vsm_driver_init()
 *  - #vsm_driver_deinit()
 *
 * - Selecting the signals.  \n The #vsm_driver_set_signal() API function selects the sensing signal of the VSM.
 *   The function is:
 *  - #vsm_driver_set_signal()
 *
 * - VSM read and write register functions.  \n The #vsm_driver_read_register() function reads the register value from assigned address.
 *   The #vsm_driver_write_register() function writes the value to the register from assigned address. The functions are:
 *  - #vsm_driver_read_register()
 *  - #vsm_driver_write_register()
 *
 * - Read SRAM.  \n The #vsm_driver_read_sram() API can get the latest  SRAM data by SRAM type.
 *   Call #vsm_driver_set_signal() to generate biosensing data to the SRAM with the status set to enable, then call #vsm_driver_read_sram() to get the SRAM data. The function is:
 *  - #vsm_driver_read_sram()
 *
 * - Other settings.  \n Apply the following functions to adjust various settings..
 *   The functions are:
 *  - #vsm_driver_set_led_current()
 *  - #vsm_driver_set_tia_gain()
 *  - #vsm_driver_set_ambdac_current()
 *  - #vsm_driver_set_pga_gain()
 *
 */

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup vsm_driver_define Define
  * @{
  */

/** @brief  This macro defines the SRAM length used in #vsm_driver_read_sram() function.
  */
#define VSM_SRAM_LEN    (384)

/**
  * @}
  */

/** @defgroup vsm_driver_enum Enum
  * @{
  */

/** @brief  This enum defines the API return type.  */
typedef enum {
    VSM_STATUS_ERROR         = -3,       /**<  The function call failed. */
    VSM_STATUS_INVALID_PARAMETER 	= -2,       /**<  Invalid parameter was given. */
    VSM_STATUS_UNINITIALIZED 		= -1,       /**<  The VSM is not initialized. */
    VSM_STATUS_OK 					= 0         /**<  This function call was successful. */
} vsm_status_t;


/** @brief This enum defines the VSM signal. */
typedef enum
{
    // Voltage signal
	VSM_SIGNAL_EKG        = 0x00000001L,    /**<  EKG signal. */
	VSM_SIGNAL_EEG        = 0x00000002L,    /**<  EEG signal. */
	VSM_SIGNAL_EMG        = 0x00000004L,    /**<  EMG signal. */
	VSM_SIGNAL_GSR        = 0x00000008L,    /**<  GSR signal. */

	// Current signal
	VSM_SIGNAL_PPG1       = 0x00000010L,    /**<  PPG1 signal. */
	VSM_SIGNAL_PPG2       = 0x00000020L,    /**<  PPG2 signal. */
	VSM_SIGNAL_BISI       = 0x00000040L,    /**<  BISI signal. */
	VSM_SIGNAL_PPG1_512HZ = 0x00000080L,    /**<  PPG1 512 Hz signal for blood pressure. */
} vsm_signal_t;

/** @brief This enum defines the sram types. */
typedef enum
{
    VSM_SRAM_EKG     = 0,    /**<  SRAM EKG type. */
	VSM_SRAM_PPG1    = 1,    /**<  SRAM PPG1 type. */
	VSM_SRAM_PPG2    = 2,    /**<  SRAM PPG2 type. */
	VSM_SRAM_BISI    = 3,    /**<  SRAM BISI type. */
	VSM_SRAM_NUMBER
} vsm_sram_type_t;

/** @brief This enum defines the tia gain. */
typedef enum
{
	VSM_TIA_GAIN_10_K =   5,    /**<  TIA GAIN 10 kΩ. */
	VSM_TIA_GAIN_25_K =   4,    /**<  TIA GAIN 25 kΩ. */
	VSM_TIA_GAIN_50_K =   3,    /**<  TIA GAIN 50 kΩ. */
	VSM_TIA_GAIN_100_K =  2,    /**<  TIA GAIN 100 kΩ. */
	VSM_TIA_GAIN_250_K =  1,    /**<  TIA GAIN 250 kΩ. */
	VSM_TIA_GAIN_500_K =  0,    /**<  TIA GAIN 500 kΩ. */
	VSM_TIA_GAIN_1000_K = 6     /**<  TIA GAIN 1000 kΩ. */
} vsm_tia_gain_t;

/** @brief This enum defines the full-scale range of LED current. */
typedef enum
{
	VSM_LED_CUR_RANGE_9_DOT_5_MA    =   0,  /**<  Full-scale range of LED current 9.5   mA */
	VSM_LED_CUR_RANGE_22_DOT_9_MA   =   1,  /**<  Full-scale range of LED current 22.9  mA */
	VSM_LED_CUR_RANGE_36_DOT_3_MA   =   2,  /**<  Full-scale range of LED current 36.3  mA */
	VSM_LED_CUR_RANGE_49_DOT_7_MA   =   3,  /**<  Full-scale range of LED current 49.7  mA */
	VSM_LED_CUR_RANGE_63_DOT_1_MA   =   4,  /**<  Full-scale range of LED current 63.1  mA */
	VSM_LED_CUR_RANGE_76_DOT_5_MA   =   5,  /**<  Full-scale range of LED current 76.5  mA */
	VSM_LED_CUR_RANGE_89_DOT_9_MA   =   6,  /**<  Full-scale range of LED current 89.9  mA */
	VSM_LED_CUR_RANGE_103_DOT_3_MA  =   7,  /**<  Full-scale range of LED current 103.3 mA */
} vsm_led_current_range_t;

/** @brief This enum defines the PGA gain. */
typedef enum
{
	VSM_PGA_GAIN_1      =   0,   /**<  PGA GAIN 1   V/V. */
	VSM_PGA_GAIN_1_DOT_5=   1,   /**<  PGA GAIN 1.5 V/V. */
	VSM_PGA_GAIN_2      =   2,   /**<  PGA GAIN 2   V/V. */
	VSM_PGA_GAIN_3      =   3,   /**<  PGA GAIN 3   V/V. */
	VSM_PGA_GAIN_4      =   4,   /**<  PGA GAIN 4   V/V. */
	VSM_PGA_GAIN_6      =   6,   /**<  PGA GAIN 5   V/V. */
} vsm_pga_gain_t;


/** @brief This enum defines the AMBDAC type. */
typedef enum
{
	VSM_AMBDAC_1    =   1,      /**<  AMBDAC1. */
	VSM_AMBDAC_2    =   2,      /**<  AMBDAC2. */
} vsm_ambdac_type_t;

/** @brief This enum defines the LED type. */
typedef enum
{
	VSM_LED_1    =   1,      /**<  LED 1. */
	VSM_LED_2    =   2,      /**<  LED 2. */
} vsm_led_type_t;

/** @brief This enum defines the AMBDAC current. */
typedef enum
{
    VSM_AMBDAC_CURR_00_MA    =   0x0,      /**<  AMBDAC CURRENT 00 mA. */
    VSM_AMBDAC_CURR_01_MA    =   0x1,      /**<  AMBDAC CURRENT 01 mA. */
    VSM_AMBDAC_CURR_02_MA    =   0x2,      /**<  AMBDAC CURRENT 02 mA. */
    VSM_AMBDAC_CURR_03_MA    =   0x3,      /**<  AMBDAC CURRENT 03 mA. */
    VSM_AMBDAC_CURR_04_MA    =   0x4,      /**<  AMBDAC CURRENT 04 mA. */
    VSM_AMBDAC_CURR_05_MA    =   0x5,      /**<  AMBDAC CURRENT 05 mA. */
    VSM_AMBDAC_CURR_06_MA    =   0x6,      /**<  AMBDAC CURRENT 06 mA. */
} vsm_ambdac_current_t;

/** @brief This enum defines the EKG sample rate. */
typedef enum
{
	VSM_EKG_FPS_256_HZ = 0,    /**<  EKG 256Hz. */
	VSM_EKG_FPS_512_HZ = 1     /**<  EKG 512Hz. */
} vsm_ekg_fps_t;

/** @brief This enum defines the PPG duty. */
typedef enum
{
	VSM_PPG_DUTY_1_DOT_5_PERCENT    = 0,    /**<  PPG DUTY 1.5%.    */
	VSM_PPG_DUTY_4_DOT_7_PERCENT    = 1,    /**<  PPG DUTY 4.7%.    */
	VSM_PPG_DUTY_6_DOT_25_PERCENT   = 2,    /**<  PPG DUTY 6.25%.   */
	VSM_PPG_DUTY_18_DOT_75_PERCENT  = 3,    /**<  PPG DUTY 18.75%.  */
} vsm_ppg_duty_t;

/**
  * @}
  */

/** @defgroup hal_wdt_struct Struct
  * @{
  */

/** @brief This structure defines the bus data structure. For more infomation, please refer to #vsm_driver_read_register(). */
typedef struct
{
   uint8_t addr;           /**< Device address.*/
   uint8_t reg;            /**< Device register value.*/
   uint8_t *data_buf;      /**< A pointer to a data buffer.*/
   uint8_t	length;        /**< Length of the data buffer.*/
}bus_data_t;

/**
  * @}
  */

/**
 * @brief This function initializes the VSM hardware. It can be used to power on the VSM.
 *        CAll #vsm_driver_set_signal() to apply the VSM operation.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 * @sa  #vsm_driver_deinit
 * @par       Example
 * Sample code please refer to @ref Driver_Usage_Chapter
 */
vsm_status_t vsm_driver_init(void);


/**
 * @brief This function deinitializes the VSM.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 * @sa  #vsm_driver_init
 * @par       Example
 * Sample code please refer to @ref Driver_Usage_Chapter
 */
vsm_status_t vsm_driver_deinit(void);


/**
 * @brief This function sets the signal setting of the VSM.
 * Before calling this function, #vsm_driver_init() must be called first.
 * @param[in] signal is the signal configuration parameter. Please always use #vsm_signal_t.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 * @par       Example
 * Sample code please refer to @ref Driver_Usage_Chapter
 */
vsm_status_t vsm_driver_set_signal(vsm_signal_t signal);

/**
 * @brief This function clear the signal setting of the VSM.
 * @param[in] signal is the signal configuration parameter. Please always use #vsm_signal_t.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 * @par       Example
 * Sample code please refer to @ref Driver_Usage_Chapter
 */
vsm_status_t vsm_driver_clear_signal(vsm_signal_t signal);

/**
 * @brief This function reads data from the VSM register using a bus protocol implemented by the user. 
 * @param[out] data is the bus register structure. Please always use #bus_data_t.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 * @sa  #vsm_driver_write_register
 */
vsm_status_t vsm_driver_read_register(bus_data_t *data);


/**
 * @brief This function write data to the VSM register using a bus protocol implemented by the user. 
 * @param[in] data is the bus register structure. Please always use #bus_data_t.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 * @sa  #vsm_driver_read_register
 */
vsm_status_t vsm_driver_write_register(bus_data_t *data);


/**
 * @brief This function reads the SRAM data by the SRAM type.
 * @param[in] sram_type is the type of the SRAM. Please always use #vsm_sram_type_t.
 * @param[out] data_buf is the data buffer to receive.
 * @param[out] len is the length of results.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 * @par       Example
 */
vsm_status_t vsm_driver_read_sram(vsm_sram_type_t sram_type, uint32_t *data_buf, int32_t *len);


/**
 * @brief This function updates the register status.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 */
vsm_status_t vsm_driver_update_register(void);

/**
 * @brief This function adjusts the LED current to increase the PPG amplitude.
 * @param[in] led_type is the type of led. Please always use #VSM_LED_1 or #VSM_LED_2.
 * @param[in] input is the LED current value and provided by algorithm.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 */
vsm_status_t vsm_driver_set_led_current(vsm_led_type_t led_type, uint32_t input);

/**
 * @brief This function sets the TIA gain that converts the input photodiode current into an appropriate voltage.
 * @param[in] ppg_type is the type of ppg. Please always use #VSM_SIGNAL_PPG1 or #VSM_SIGNAL_PPG2.
 * @param[in] input is the pd gain value. Please use #vsm_tia_gain_t.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 */
vsm_status_t vsm_driver_set_tia_gain(vsm_signal_t ppg_type, vsm_tia_gain_t input);

/**
 * @brief This function sets up the AMBDAC current that shifts the TIA output current.
 * @param[in] ambdac_type is the type of AMBDAC. Please refer to #vsm_ambdac_type_t.
 * @param[in] current is the AMBDAC current. Please use #vsm_ambdac_current_t.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 */
vsm_status_t vsm_driver_set_ambdac_current(vsm_ambdac_type_t ambdac_type, vsm_ambdac_current_t current);

/**
 * @brief This function sets up the PGA gain that amplifies the TIA gain. 
 * @param[in] input is the pga gain value. Please use #vsm_pga_gain_t.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 */
vsm_status_t vsm_driver_set_pga_gain(vsm_pga_gain_t input);

/**
 * @brief This function customizes the register map settings.
 * @param[in] signal is the signal type. Please use #vsm_signal_t.
 * @param[in] addr is the address of the register of mt2511. The first byte is the register offset. The second byte is the device address.
 * @param[in] reg_data will be written to the register map.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 */
vsm_status_t vsm_driver_set_register_map(vsm_signal_t signal, int32_t addr, uint32_t reg_data);

/**
 * @brief This function enables the selected biosignal timing module.
 * @param[in] signal is the signal type. Please use #vsm_signal_t.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 */
vsm_status_t vsm_driver_enable_signal(vsm_signal_t signal);

/**
 * @brief This function disables the selected biosignal timing module.
 * @param[in] signal is the signal type. Please use #vsm_signal_t.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 */
vsm_status_t vsm_driver_disable_signal(vsm_signal_t signal);

/**
 * @brief This function sets up the PPG duty .
 * @param[in] ppg_duty is the type of PPG duty. Please refer to #vsm_ppg_duty_t.
 * @return #VSM_STATUS_ERROR, if the operation failed. \n
 *         #VSM_STATUS_INVALID_PARAMETER, if an invalid parameter was given. \n
 *         #VSM_STATUS_UNINITIALIZED, if the VSM is not initialized. \n
 *         #VSM_STATUS_OK, if the operation completed successfully. \n
 */
vsm_status_t vsm_driver_set_ppg_duty(vsm_ppg_duty_t ppg_duty);

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/

#endif /* DEMOS_VSM_DRIVER_H_ */
