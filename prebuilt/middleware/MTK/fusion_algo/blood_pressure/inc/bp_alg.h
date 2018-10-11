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
#ifndef BP_ALG_H
#define BP_ALG_H 
/**
 * @file    bp_alg.h
 * @brief   Library of blood pressure algorithm
 * @author  Mediatek
 * @version 1.0.0.1
 * @date    2016.08.04
 */
/** 
 * @addtogroup BP_ALG
 * @{
 *
 * @section Terms_Chapter Terms and acronyms
 * 
 * |Terms                   |Details                                                                 |
 * |------------------------|------------------------------------------------------------------------|
 * |\b BP_ALG               | Blood pressure algorithm.                                              |
 * |\b PPG                  | Photoplethysmogram. For more information, please refer to the <a href="https://en.wikipedia.org/wiki/Photoplethysmogram"> PPG in Wikipedia </a>.   |
 * |\b ECG                  | Electrocardiography. For more information, please refer to the <a href="https://en.wikipedia.org/wiki/Electrocardiographym"> ECG in Wikipedia </a>.|
 * |\b PWTT                 | Pulse Wave Transit Time. It is the time difference between R-peak of ECG and wave-peak of PPG of the same heart beat. For more information, please refer to the <a href="http://www.nihonkohden.com/tech/pwtt/how.html"> How it works: PWTT </a>.|
 * |\b SBP                  | Systolic Blood Pressure. For more information, please refer to the <a href="https://en.wikipedia.org/wiki/Blood_pressure"> Systolic blood pressure in Wikipedia </a>.|
 * |\b DBP                  | Diastolic Blood Pressure. For more information, please refer to the <a href="https://en.wikipedia.org/wiki/Blood_pressure"> Diastolic blood pressure in Wikipedia </a>.|
 * |\b BPM                  | Beats Per Minute. The common unit for heart rate value representation.
 *
 *
 * @section BP_ALG_Usage_Chapter How to use this library
 * Step 1: Call #bp_alg_init() to initialize blood pressure algorithm library. \n 
 * Step 2: Call #bp_alg_set_user_info() to set the user information. \n 
 * Step 3: Call #bp_alg_process_data() when the PPG data or ECG data is received. \n 
 * Step 4: Call #bp_alg_get_bp() to get the blood pressure value, after the PPG/ECG data has been processed by #bp_alg_process_data(). \n
 *
 *  - Sample code:
 *    @code 
 *
 * #include "bp_alg.h" 
 * #define BP_DATA_LEN   12800  // The input data length 
 * bp_alg_out_t bp_output; // The output structure for #bp_alg_get_bp().
 * int32_t bp_height;      // The user's height as an input to #bp_alg_set_user_info().
 * int32_t bp_weight;      // The user's weight as an input to #bp_alg_set_user_info(). 
 * int32_t bp_gender;      // The user's gender as an input to #bp_alg_set_user_info(). 
 * int32_t bp_age;         // The user's age as an input to #bp_alg_set_user_info(). 
 * int32_t bp_arm_len;     // The user's arm length as an input to #bp_alg_set_user_info(). 
 * void example(void)
 * {
 *   int32_t i;
 *   // Initialization
 *   bp_alg_init();
 *   // Set the user specific settings.
 *   bp_alg_set_user_info(bp_age, bp_gender, bp_height, bp_weight, bp_arm_len);
 *   for (i=0;i<BP_DATA_LEN;i++)
 *   {
 *     bp_alg_process_data(ecg_data[i], 0); // Input the ECG data for processing. 
 *     bp_alg_process_data(ppg_data[i], 1); // Input the PPG data for processing.
 *   }
 *   // Output blood pressure.
 *   bp_alg_get_bp(&output_data);
 * }
 *    @endcode
 *
 */

/** @defgroup bp_alg_define Define
  * @{
  */

#define MAX_NUM_PWTT 40    /**< The maximum number of PWTT. */
/**
  * @}
  */

/** @defgroup ohrm_struct Struct
  * @{
  */

/** @struct bp_alg_out_t
* @brief This structure defines output of the blood pressure algorithm.
*/
typedef struct 
{	
  int32_t sbp;      /**< Systolic blood pressure. */
  int32_t dbp;      /**< Diastolic blood pressure. */
  int32_t bpm;	    /**< Beats per minute. */
  int32_t num_pwtt; /**< The number of PWTT. */
  int32_t pwtt[MAX_NUM_PWTT]; /**< An output array for the PWTT data. */
//   int32_t status;
}bp_alg_out_t;

/** 
 * @brief This function initializes the algorithm.
*/
void bp_alg_init(void);

/** 
 * @brief This function sets the user information for the blood pressure algorithm library. Call this function right after initializing the algorithm to include user's setting for all the other calculations.
 * @param[in] age is the user's age.
 * @param[in] gender is the user's gender. If the value is 1, the gender is male, otherwise it is female.
 * @param[in] height is the user's height. The unit of height is centimeters. 
 * @param[in] weight is the user's weight. The unit of weight is kilograms.
 * @param[in] arm_length is the user's arm length. The unit of length is centimeters.
*/
void bp_alg_set_user_info(int32_t age, int32_t gender, int32_t height, int32_t weight, int32_t arm_length);
/** 
 * @brief This function processes the input ECG/PPG data.
 * @param[in] data is the input data.
 * @param[in] type is the type of input data. If the value is 0, the input data is the ECG data point. If the value is 1, the input data is the PPG data point.
*/
void bp_alg_process_data(int32_t data, int32_t type);
/** 
 * @brief This function gets the blood pressure value from the blood pressure algorithm.
 * @param[out] output is the output structure of the blood pressure algorithm.
*/
void bp_alg_get_bp(bp_alg_out_t* output);
/**
* @}
* @}
*/
void bp_alg_set_calibration_data(int32_t* data_in,  int32_t data_in_len);
void bp_alg_get_calibration_data(int32_t* data_out, int32_t data_out_len);
int32_t get_pwtt_variation(bp_alg_out_t* bp_output);
int32_t bp_alg_get_ecg_error(int32_t data_curr);
int32_t bp_alg_get_version(void);

#endif //BP_ALG_H
