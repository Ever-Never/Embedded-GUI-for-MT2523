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
#include "ppg_control_module.h"

/** 
 * @brief ppg_noise_level represents the noise amplitude of the PPG signal.
*/
const int ppg_noise_level = 40;

/** 
 * @brief ppg_control_max_tia defines the upper bound of MT2511 TIA setting for automatic adjustment. \n
 *        ppg_control_min_tia defines the lower bound of MT2511 TIA setting for automatic adjustment.
*/
const int ppg_control_max_tia = MT2511_PPG_TIA_GAIN_500K;
const int ppg_control_min_tia = MT2511_PPG_TIA_GAIN_010K; 

/** 
 * @brief ppg_init_ambdac_current defines the initial setting of MT2511 ambdac1 current (0~6uA). \n
 *        ppg_init_led_current defines the initial setting of MT2511 LED current (0~255 code). \n
 *        ppg_init_tia_gain defines the initial setting of MT2511 TIA gain (010K~001M). \n 
*/
const int ppg_init_ambdac_current = 3;
const int ppg_init_led_current = 192;
const int ppg_init_tia_gain = MT2511_PPG_TIA_GAIN_100K; 

/** 
 * @brief threshold for watch off to on. 
*/
const int ppg_amb_on_threshold = 10000;
const int ppg_amb_variation_on_threshold = 1000;
const int ppg_Ipd_led_on_threshold = 1370;

/** 
 * @brief threshold for watch on to off. 
*/
const int ppg_Ipd_led_off_threshold = 1370;
const int ppg_Ipd_amb_off_threshold = 102050;
const int ppg_amb_variation_off_threshold = 1000;
const int ppg_amb_variation_off_threshold_zero = 15000;
const int ppg_Ipd_amb_led_off_threshold = 130000;   



