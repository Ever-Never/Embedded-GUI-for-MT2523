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

#ifndef __CMF_ECG_H__
#define __CMF_ECG_H__

#include "stdint.h"
#define DEBUG_MODE             (1)  ///< DEBUG_MODE

#if DEBUG_MODE
#include <stdio.h>
#define DEBUG_MEM               1
#else
#define DEBUG_MEM               0
#endif

#define BEAT_DETECTION_IN_ARR    (1)  ///< BEAT_DETECTION_IN_ARR 
#define HRV_DETECTION            (1)  ///< HRV_DETECTION
#define MEM_ORDER               (20)  ///< MEM_ORDER
#define EXTEND_BOUNDARY_BEAT     (0)  ///< EXTEND_BOUNDARY_BEAT
#define EXTEND_BEAT_NUM          (3)  ///< EXTEND_BEAT_NUM
#define ECG_FS                 (512)  ///< ECG_FS
#define ECG_FS_BITS              (9)  ///< ECG_FS

#define MEM_SAMPLE_SIZE        (300)  ///< MEM_SAMPLE_SIZE
#define SMALL_SPECTRUM           (1)
#if SMALL_SPECTRUM
#define SPECTRUM_SIZE          (50)  ///< SPECTRUM_SIZE
#else
#define SPECTRUM_SIZE          (500)  ///< SPECTRUM_SIZE
#endif

#define FRAME_SIZE             (512)  ///< FRAME_SIZE

#define ECG_SCALE                (1)  ///< ECG_SCALE
#define ECG_BEAT_SCALING     (12583) ///< ECG_BEAT_SCALING
#define ECG_BEAT_SCALING_15  (18874) ///< ECG_BEAT_SCALING
#define MAX_R_AVE_INI        (10000)
#define MAX_INT         (2147483647)  ///< max. value of signed 32-bit integer
#define MIN_INT        (-2147483646)  ///< max. value of signed 32-bit integer
#define RR_SIZE                (300)

/** 
 * @addtogroup API
 * @{
 * This document include ECG beat detection APIs.
 * @addtogroup ECG ECG
 * @{
 * This document mainly introduce all details about the ECG beat detection APIs including terms and acronym, how to use this driver and ECG function groups.
 
 // supported features,software achitechture,how to use this driver,ECG function groups,all enum,structure and functions.
 * @section Terms_Chapter Terms and acronyms
 * 
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b HRV                        | Heart Rate Variability. For more information, please check <a href="https://en.wikipedia.org/wiki/Heart_rate_variability"> Heart Rate in wiki </a>.|
 *
 * @section Function_Group_Chapter ECG function groups description
 * 
 * For easily using,we group the ECG function by its functionality.
 * You can refer to the details of each function by clicking the function name.
 *
 
 * @section Function_Block_Diagram Control Flow: HRV Estimation
 * Function flow
 * @image html flow.png
 
 * @section ECG_Usage_Chapter How to use heart_rate_variability.c function
 *  - sample code:
 *    @code 
 *#include <stdio.h>
 *#include "heart_rate_variability.h"
 *
 *void main(int32_t argc, char* argv[])
 *{
 *  int32_t i;
 *  int32_t sdnn, lf, hf, lf_hf;    // HRV output values
 *  HRV_initialization(ECG_FS);     // Initialization
 *  for (i=0;i<HRV_DATA_LENGTH;i++)
 *  {
 *    HRV_process_data(ecg_data[i]);//  Input the ECG data for processing.
 *  }
 *  HRV_estimate(&sdnn,&lf,&hf,&lf_hf); // Output HRV values
 *}

 * @endcode
 
 * - HRV software initialize functions. \n The #HRV_initialization function is a initialization stage.
 *   The functions is:
 *  - #HRV_initialization
 
 * - HRV software estimate functions. \n The #HRV_estimate function should be called for 300 second data.
 *   The functions is:
 *  - #HRV_estimate
 
 * - HRV software pre-processing functions. \n The #HRV_process_data function should be called for every ECG sample data.
 *   The functions is:
 *  - #HRV_process_data 
 */

int32_t myround(float x);

/** 
 * @brief This function initializes the algorithm.
 * @param[in] SfreqDSP is ECG sampling rate.
*/ 	
void HRV_initialization(int32_t SfreqDSP);

/** 
 * @brief This function should be called for a 5x60 secs EKG signal.
 * @param[in] *data is a ECG data point.
 * Sample code please refer to @ref ECG_Usage_Chapter   	
*/
void HRV_process_data(int32_t data);

/** 
 * @brief This function will calculate the heart rate variability parameters 
 * @param[out] SDNN is SDNN of HRV.
 * @param[out] LF is LF of HRV.
 * @param[out] HF is HF of HRV.
 * @param[out] LF_HF is LF/HF of HRV .
 * @sa  #version_info
 * @par Example
 * Sample code please refer to @ref ECG_Usage_Chapter   	
*/
int32_t HRV_estimate(int32_t* SDNN,int32_t* LF,int32_t* HF,int32_t* LF_HF);
int get_fatigue_index_from_hrv (float lf, float hf);
int get_pressure_index_from_sdnn(float sdnn, float age);


/** 
 * @brief This function remove noise from input signal.
 * @param[in] *dataIn is a input data buffer.
 * @param[out] *dataOut is a output data buffer.
 * @param[out] dataLength is the length of data buffer.
 * @sa  #version_info
 * @par Example
 * Sample code please refer to @ref ECG_Usage_Chapter   	
*/
void cmf_ECG_remove_noise(int32_t *dataIn, int32_t *dataOut, int32_t dataLength);

/** 
 * @brief This function get beat interval of input signal.
 * @param[in] *dataIn is a input data buffer.
 * @param[in] FrameSize is a the data length of input signal.
 * @param[in] shiftI is the shift value compare with orignal signal.
 * @param[out] beatPos is output of beat position.
 * @sa  #version_info
 * @par Example
 * Sample code please refer to @ref ECG_Usage_Chapter   	
*/
void cmf_ECG_detect_beats(int32_t *dataIn, int32_t FrameSize,  int32_t shiftI, int32_t *beatPos);

/** 
 * @brief This function get beat interval of input signal.
 * @param[in] *rr is a input rr interval.
 * @param[in] len is the data length of input signal.
 * @sa  #version_info
 * @par Example
 * Sample code please refer to @ref ECG_Usage_Chapter   	
*/
void cmf_ECG_get_MEM_coef(int32_t* rr, int32_t len);
void hrv_main(char* filename, int32_t age, int32_t file_format);
int32_t HRV_get_version();
#endif