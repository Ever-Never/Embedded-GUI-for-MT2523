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
// Author: chihming.fu@mediatek.com
#ifndef BP_ALG_INT_H
#define BP_ALG_INT_H

#include <stdint.h>
#include <stdio.h>

#define HRV_VERSION     7
#define BP_ALG_VERSION  10 
// 7 all data model
// 8 add android log
// 9 wcp SQC3
// 10 iot release

#define BP_ALG_PCMODEL 0
#define HRV_ALG_PCMODEL 0
#define ECG_ALG_PCMODEL 0
#define BP_ALG_DEBUG 0
#define BP_ALG_DEBUG_ANDROID 0

#define BP_ALG_PPG_FEA_MORE 1
#define BP_ALG_CHECK_RAW_OUT 0 // for 2511 cg data
#define BP_ALG_BEAT_ORG 1 // 2511 ecg beat
#define HRV_HR_EST 0

#define GET_ECG_LEVEL 0
#if GET_ECG_LEVEL
#define GET_ECG_QRS 1
#else
#define GET_ECG_QRS 0
#endif

#if BP_ALG_DEBUG_ANDROID
#define LOG_TAG "BP_ALG"
#define LOGV(...) __android_log_print( ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__ )
#define LOGD(...) __android_log_print( ANDROID_LOG_DEBUG  , LOG_TAG, __VA_ARGS__ )
#define LOGI(...) __android_log_print( ANDROID_LOG_INFO   , LOG_TAG, __VA_ARGS__ )
#define LOGW(...) __android_log_print( ANDROID_LOG_WARN   , LOG_TAG, __VA_ARGS__ )
#define LOGE(...) __android_log_print( ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__ )
#include <android/log.h>
#endif

#if BP_ALG_PCMODEL || HRV_ALG_PCMODEL
#define MAIN_EXIST 1
#else
#define MAIN_EXIST 0
#endif

#define USE_ORDER_64_FILTER 1

#define BPA_FS 512
#define USE_FILTER_60 0
#define USE_SMOOTH_RRI 0
#define MAX_VALUE_NUM 12 //af:8 org:4 // SEARCH_RANGE_LONG/SEARCH_RANGE_SHORT i4Max_value_num 
#define FRAME_SIZE             (512)  ///< FRAME_SIZE

#define BP_DATA_LEN 12800 // 23040 
#define BPA_BEAT_POS_BUFFER_SIZE 60
#if ECG_ALG_PCMODEL
#define BEAT_POS_BUFFER_SIZE          (60000)  ///< 120 * 5
#else
#define BEAT_POS_BUFFER_SIZE          (600) //600 ///< 120 * 5
#define BEAT_POS_BUFFER_SIZE_INTERP   (300) //300 ///< 60 * 5
#endif

#if ECG_ALG_PCMODEL
#define ECG_BEAT_SCALING_CLIP  (60000) ///< ECG_BEAT_SCALING
#else
#define ECG_BEAT_SCALING_CLIP  (18874) ///< ECG_BEAT_SCALING
#endif

#define DEV_HIST_SIZE 40//35//35 // f4DevHist  = malloc(sizeof(int32_t)*2*i4St_order2+1); 
#define SF 6//6 // (int32_t)myround((float)0.01*(float)i4SfreqDSP)
#define BP_DIFF_DIST 5 //5

#define MAX_INT         (2147483647)  ///< max. value of signed 32-bit integer
#define MIN_INT        (-2147483646)  ///< max. value of signed 32-bit integer

#if USE_ORDER_64_FILTER
#define FIR_ORDER_64 64
#define FIR_ORDER FIR_ORDER_64 
#else
#define FIR_ORDER_64 64
#define FIR_ORDER 32
#endif

#define ECG_IN 0
#define PPG_IN 1
#define BPA_FRAME_SIZE 512 /**< The buffer size for BP_ALG data. */

#define NUM_PWTT_FEA_MED 7

static int32_t f4Lpf40CoefInt[FIR_ORDER] = 
{
  6,8,9,6,1,-6,-13,-17,-16,-10,3,21,41,61,77,85,85,77,61,41,21,3,-10,-16,-17,-13,-6,1,6,9,8,6
};

// scaling 512
static int32_t low_pass_filter_ppg [FIR_ORDER_64] = 
{
-1 ,   -1  ,  -1   , -1  ,  -1 ,   -1   ,  0 ,    0    , 0    , 1   ,  2  ,   2  ,   3  ,   4   ,  5  ,   6  ,   7  ,   8  ,  10 ,   11 ,   12 ,   13 ,   14, 15 ,   16  ,  17 ,   18  ,  19  ,  19   , 20  ,  20  ,  20  ,  20  ,  20  ,  20  ,  19  ,  19  ,  18 ,   17  ,  16  ,  15 ,   14   , 13  ,  12  ,  11 ,   10, 8   ,  7  ,   6  ,   5 ,    4    , 3   ,  2  ,   2  ,   1   ,  0    , 0    , 0  ,  -1 ,   -1 ,   -1 ,   -1 ,   -1  ,  -1
};
// scaling 512
static int32_t band_pass_filter_ecg [FIR_ORDER_64] = 
{
-1,-1,-1,-1,0,0,0,1,1,1,1,0,-1,-2,-3,-5, 3,-1,-7,-14,-22,-29,-33,-33,-28,-19,-5,11,28,44,55,61,61,55,44,28,11,-5,-19,-28,-33,-33,-29,-22,-14, -7,-1,3,-5,-3,-2,-1,0,1,1,1,1,0,0,0,-1,-1,-1,-1
};


static int32_t pwtt_set_median_pre[8] = 
{
  89,	103,	123,	145,	188,	270,	276,	310
};

#define FILTER_TAP_NUM 239
#define FILTER_GAIN 50724.0


#if USE_FILTER_60
static int filterCoeff60Hz[FILTER_TAP_NUM] = {  6430,  -1810,  -1471,  -1180,  -988,  -907,  -903,  -911,  -869,  -745,  -555,  -357,  -218,  -181,  -245,  -353,  -432,  -418,  -299,  -117,  47,  116,  58,  -96,  -263,  -346,  -291,  -113,  105,  253,  250,  92,  -144,  -333,  -370,  -224,  39,  288,  391,  283,  10,  -288,  -452,  -393,  -121,  225,  467,  471,  222,  -160,  -477,  -557,  -347,  57,  448,  620,  468,  63,  -393,  -662,  -591,  -204,  305,  672,  698,  356,  -187,  -651,  -788,  -514,  40,  590,  847,  666,  129,  -491,  -871,  -803,  -311,  357,  855,  916,  498,  -191,  -796,  -998,  -681,  -1,  693,  1037,  845,  208,  -549,  -1032,  -982,  -419,  370,  983,  1077,  629,  -164,  -884,  -1146,  -812,  -54,  738,  1153,  972,  285,  -555,  -1111,  -1094,  -508,  344,  1022,  1171,  712,  -116,  -886,  64340,  -886,  -116,  712,  1171,  1022,  344,  -508,  -1094,  -1111,  -555,  285,  972,  1153,  738,  -54,  -812,  -1146,  -884,  -164,  629,  1077,  983,  370,  -419,  -982,  -1032,  -549,  208,  845,  1037,  693,  -1,  -681,  -998,  -796,  -191,  498,  916,  855,  357,  -311,  -803,  -871,  -491,  129,  666,  847,  590,  40,  -514,  -788,  -651,  -187,  356,  698,  672,  305,  -204,  -591,  -662,  -393,  63,  468,  620,  448,  57,  -347,  -557,  -477,  -160,  222,  471,  467,  225,  -121,  -393,  -452,  -288,  10,  283,  391,  288,  39,  -224,  -370,  -333,  -144,  92,  250,  253,  105,  -113,  -291,  -346,  -263,  -96,  58,  116,  47,  -117,  -299,  -418,  -432,  -353,  -245,  -181,  -218,  -357,  -555,  -745,  -869,  -911,  -903,  -907,  -988,  -1180,  -1471,  -1810,  6430};
#endif

typedef struct
{	
  int32_t *ppg_data;
  int32_t *ppg_data_2;
  int32_t *ecg_data;
  int32_t ppg_len;
  int32_t ppg_len_2;
  int32_t ecg_len;
  int32_t ecg_len_max;
  int32_t ppg_len_max;
  int32_t ppg_len_max_2;
} bp_alg_data_t;

typedef struct
{	
  int32_t is_LPF_first;            
  int32_t dc;
  int32_t LPF_buf[FIR_ORDER];
  int32_t type;
  int32_t fs;
  int32_t firOrder;
  int32_t firOrderRoundShift;
  int32_t firOrderBit;
  int32_t LPFCoefInt[FIR_ORDER];
} noise_reduction_t;

typedef struct 
{
  int32_t age;
  int32_t gender;
  int32_t height;
  int32_t weight;
  int32_t arm_length;
} user_info_t;


typedef struct
{
  int32_t buffer_data[BPA_FRAME_SIZE]; /**< the buffer data for BP_ALG library. */
  int32_t data_idx;                    /**< the data index of current input data. */
  int32_t buffer_data_count;           /**< the buffer data count. */
} bp_alg_buffer_t ;

int32_t bp_alg_detect_beats(int32_t *dataIn, int32_t dataECGLen, int32_t dataIdx, int32_t* beatLen, int32_t* beatPos);
int32_t bp_alg_preprocessing_ecg(int32_t *dataIn, int32_t dataECGLen, int32_t dataIdx, int32_t* beatLen, int32_t* beatPos, bp_alg_data_t *data_out, noise_reduction_t *nr_buffer);
int32_t bp_alg_preprocessing_ppg(int32_t *dataIn, int32_t dataECGLen, int32_t dataIdx, bp_alg_data_t *data_out, noise_reduction_t *nr_buffer);
void    bp_alg_initialization(int32_t SfreqDSP, noise_reduction_t *nr_ecg, noise_reduction_t *nr_ppg) ;

// cmf: common module function
void bp_remove_noise(int32_t *dataIn, int32_t *dataOut, int32_t dataLength, noise_reduction_t *data_nr);
void bp_remove_noise_init(noise_reduction_t *nr, int32_t type, int32_t fs);
void bp_ecg_detect_beats_init(int32_t SfreqDSP, int32_t beat_pos_buffer_size, int32_t rr_short_range, int32_t rr_long_range);
void bp_ecg_detect_beats(int32_t *InputData, int32_t FrameSize, int32_t shiftI, int32_t *beatPos);
int32_t  bp_get_bmi (int32_t height, int32_t weight);

// int32_t  bp_get_sbp (int32_t gender, int32_t age, int32_t height, int32_t weight, int32_t bmi, int32_t pwtt_min_inv, int32_t pwtt_max_inv, int32_t heart_rate, int32_t pwtt_diff,int32_t sbp_in,int32_t dbp_in);
// int32_t  bp_get_dbp (int32_t gender, int32_t age, int32_t height, int32_t weight, int32_t bmi, int32_t pwtt_min_inv, int32_t pwtt_max_inv, int32_t heart_rate, int32_t pwtt_diff,int32_t sbp_in,int32_t dbp_in);

int32_t  bp_clip (int32_t input_val, int32_t max_val,int32_t min_val);
int32_t  bp_clip_reset (int32_t input_val, int32_t max_val,int32_t min_val, int32_t reset_val);

void read_catch_log_data(bp_alg_data_t* bp_data_file, char* filename, int32_t format_idx);

int32_t get_median(int32_t *data, int iSize) ;
int cmf_abs_value (int x);

int bp_alg_get_ecg_beat_interval (int32_t* ecg_beat_interval_out, int32_t out_len);

#if USE_FILTER_60
void cmf_filtering60Hz (int32_t *dataIn, int32_t *dataOut, int32_t dataLen);
#endif

#if BP_ALG_CHECK_RAW_OUT
int32_t raw_data_log_cal[6];
#endif

#endif //BP_ALG_INT_H
