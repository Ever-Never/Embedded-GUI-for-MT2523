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
#ifndef OHRM_H
#define OHRM_H
/**
 * @file    ohrm.h
 * @brief   Library of optical heart rate monitor
 * @author  Mediatek
 * @version 1.0.0.30
 * @date    2016.04.17
 */

/** 
 * @addtogroup OHRM
 * @{
 *
 * @section Terms_Chapter Terms and acronyms
 * 
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b OHRM                       | Optical heart rate monitor. |
 * |\b HR                         | Heart Rate. For more information, please check <a href="https://en.wikipedia.org/wiki/Heart_rate"> Heart Rate in Wikipedia </a>.|
 * |\b BPM                        | Beats Per Minute. The common unit for heart rate value representation.
 * |\b PPG                        | Photoplethysmogram for optical HR sensing. For more information, please check <a href="https://en.wikipedia.org/wiki/Photoplethysmogram"> PPG in Wikipedia </a>.|
 * |\b motion                     | The signal acquired from accelerometer. |
 *
 * @section Function_Block_Diagram Control Flow: OHRM
 * Initialization
 * @image html Initial.png
 * Measurement
 * @image html Measurement.png
 *
 * @section OHRM_Usage_Chapter How to use OHRM APIs
 * Step 1: Call #ohrm_init() to initialize the OHRM library. The input value should have the correct representation of 1 g-force data. The dynamic range of accelerometer should be set to +/-8g in oder to prevent signal saturation under heavy motion condition. \n
 * Step 2: Call #ohrm_process() when PPG data or motion data is received. \n 
 * Step 3: Call #ohrm_get_bpm() to get the heart rate BPM value, after the PPG data has been processed by #ohrm_process(). \n
 *
 *  - sample code:
 *    @code 
 *
 * ohrm_data_t param_ppg1;   // The input PPG data structure for #ohrm_process().
 * ohrm_data_t param_motion; // The input motion data structure for #ohrm_process().
 * INT32 *data_input_ppg;       // The input data buffer of the PPG signal.
 * INT32 *data_input_motion;    // The input data buffer of the motion signal.
 * INT32 fs_input_ppg = 125;    // The input sample rate of the PPG signal (supports 125Hz only).
 * INT32 fs_input_motion = 125; // The input sample rate of the motion signal (support 125Hz only).
 * INT32 bit_width_ppg = 23;    // The PPG bit width (default value is 23 bits for MediaTek MT2511). Note: if the data is signed, set bit width to 32 bits.
 * INT32 bit_width_motion = 32; // The motion bit width. Note: if the data is signed, set bit width to 32 bits.
 * INT32 *ohrm_out;             // Output result for debugging purposes from #ohrm_process().
 * INT32 ohrm_fs_compare_ratio = 0x1000; // 0x1000 means 1X (motion fs = PPG fs).
 *
 * void example(int state)
 * {
 *      // Initialization
 *      ohrm_init(256); //For example: 1 g-force magnitude of the accelerometer data is represented by 256 codes in this case.
 *
 *      switch(state)
 *      {
 *      //Get accelerometer samples from motion driver.
 *      case MOTION_DATA_READY: 
 *          // motion processing
 *          param_motion.data = data_input_motion;
 *          param_motion.data_fs = fs_input_motion; 
 *          param_motion.data_length = fs_input_motion; //1-second as an example
 *          param_motion.data_bit_width = bit_width_motion; 
 *          param_motion.data_config = 0; // For motion, currently fixed to 0
 *          param_motion.data_source = 0; // 0: motion 
 *          ohrm_process(&param_motion, ohrm_fs_compare_ratio, ohrm_out);
 *			break;
 *
 *      //Get PPG samples from MT2511 vsm_driver.
 *      case PPG_DATA_READY: 
 *          // PPG processing
 *          param_ppg1.data = data_input_ppg;
 *          param_ppg1.data_fs = fs_input_ppg; 
 *          param_ppg1.data_length = fs_input_ppg*2; //Apply 1-second PPG input as an example (i.e. 1-sec PPG includes 250 samples).
 *          param_ppg1.data_bit_width = bit_width_ppg;
 *          param_ppg1.data_config = 2; // For PPG, currently fixed to 2.
 *          param_ppg1.data_source = 1; // 1: PPG
 *          ohrm_process(&param_ppg1, ohrm_fs_compare_ratio, ohrm_out);
 *
 *          // Output heart rate
 *          bpm = ohrm_get_bpm()>>16;
 *          break;
 *      }
 * }
 *    @endcode
 *
 */

/** @defgroup ohrm_define Define
  * @{
  */

  #include <stdint.h>
  #define INT32 int32_t
  #define INT16 int16_t
  #define UINT32 uint32_t
  #define UINT16 uint16_t

/**
  * @}
  */

/** @defgroup ohrm_struct Struct
  * @{
  */

/** @struct ohrm_data_t
* @brief This structure defines the data input structure for OHRM library.
*/
typedef struct
{	
    INT32 *data;            /**< A pointer to the input PPG and motion signals. */
    INT32 data_fs;          /**< The sampling frequency (fs) of the input data (supports 125Hz only). */
    INT32 data_length;	    /**< The number of input samples. The maximum value: input_fs for motion, input_fs*2 for PPG. */
    INT32 data_bit_width;   /**< The bit width of input data. If the data is already converted to signed representation, set bit width to 32. */
    INT32 data_config;      /**< Configuration of the input signal (default: 0 for motion, 2 for PPG). */
    INT32 data_source;      /**< 0: this is the motion data. 1: this is the PPG data. */
}ohrm_data_t;

/**
  * @}
  */

// Function definition
/** 
 * @brief Call this function to get the OHRM library version code.
 * @return The return value of 0xAABBCCDD corresponds to version AA.BB.CC.DD. The integer value should be converted to heximal representation to get the version number. For example, 0x01000000 is for verion 1.0.0.0.
*/
UINT32 ohrm_get_version(void);
const char* ohrm_get_version_string(void);

/** 
 * @brief  This function should be called when new data comes (PPG and motion signal are processed separatly).
 * @param[in] ohrm_signal_in is the motion & PPG input structure.
 * @param[in] fs_compare_ratio is defined as the ratio of the sampling rate of motion signal to the sampling rate of PPG. The value is left-shifted by 12 bits for fixed-point representation. For example, 4096 (0x1000) means the ratio equals to 1.0. The value of 3990 means that the ratio equals to 3990/4096.
 * @param[out] ohrm_out is for the debugging purposes of OHRM library.
*/
void ohrm_process(ohrm_data_t *ohrm_signal_in, INT32 fs_compare_ratio, INT32 *ohrm_out);

/** 
 * @brief This function should be called at the initialization stage.
 * @param[in] ohrm_motion_represent_1g is the representation of 1 g-force magnitude from accelerometer input. For example, if the least significant bit of accelerometer input represents 0.001 g-force, then ohrm_motion_represent_1g should be set to 1000. The maximum range of accelerometer should be set to +/- 8 g-force to prevent signal saturation under heavy motion condition.
*/
void ohrm_init(INT32 ohrm_motion_represent_1g);

/** 
 * @brief Call this function to get the HR BPM value from optical heart rate monitor.
 * @return Return the HR value in BPM. The return value is left-shifted 16 bits for fixed point representation. Right shift by 16 bits to get the integer value.
*/
INT32 ohrm_get_bpm(void);

/** 
 * @brief Call this function to get the OHRM timestamp (in seconds). The timestamp is reset to zero when #ohrm_init() is called.
 * @return Return timestamp value in seconds.
*/
INT32 ohrm_get_time(void);

/** 
 * @brief Call this function to get the OHRM confidence level. The confidence level is updated whenever HR value is updated.
 * @return Return the OHRM confidence level. \n
 *         The 8-bit LSB of the return value ranges between -1 (or 255 for unsigned value) to 3. \n
 *         -1 (255): no contact \n
 *         0: unreliable HR \n
 *         1: low accuracy \n
 *         2: medium accuracy \n
 *         3: high accuracy \n
 *         Suggested usage: maintain the last HR value or stop to display the HR value when the confidence level is -1, 0, or 1.
*/
UINT32 ohrm_get_confidence_level(void);

/*
  * @brief Call this function to get the OHRM heart rate(HR) tracking speed. The HR tracking speed is controlled by the customer.   
  *             1 : Increase the HR tracking speed (default).
  *             0 : Normal speed.
*/
void ohrm_set_tracking_speed(INT32 speed);
/**
* @}
* @}
*/


#endif //OHRM_H
