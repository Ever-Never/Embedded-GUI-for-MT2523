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

#ifndef __AUDIO_DB_TO_GAIN_VALUE_MAPPING_TABLE_H__
#define __AUDIO_DB_TO_GAIN_VALUE_MAPPING_TABLE_H__


#ifdef __cplusplus
extern "C" {
#endif


#define AUDIO_DOWNLINK_AMP_DB_MINIMUM -76
#define AUDIO_DOWNLINK_AMP_DB_MAXIMUM -2
#define ADUIO_DOWNLINK_AMP_DB_STEP 2
#define VOICE_DOWNLINK_AMP_DB_MINIMUM -23
#define VOICE_DOWNLINK_AMP_DB_MAXIMUM 7
#define VOICE_DOWNLINK_AMP_DB_STEP 2
#define VOICE_UPLINK_AMP_DB_MINIMUM 4
#define VOICE_UPLINK_AMP_DB_MAXIMUM 45
#define VOICE_UPLINK_AMP_DB_STEP 1
#define VOICE_UPLINK_DMIC_DB_MINIMUM 4
#define VOICE_UPLINK_DMIC_DB_MAXIMUM 34
#define VOICE_UPLINK_DMIC_DB_STEP 1
#define VOICE_SIDETONE_DB_MINIMUM 0
#define VOICE_SIDETONE_DB_MAXIMUM 31
#define VOICE_SIDETONE_DB_STEP 1


void audio_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(int32_t input_db, int32_t *analog_gain_in_db, int32_t *digital_gain_in_db);
void voice_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(int32_t input_db, int32_t *analog_gain_in_db, int32_t *digital_gain_in_db);
void voice_uplink_amp_db_transfer_to_analog_gain_and_digital_gain(int32_t input_db, int32_t *analog_gain_in_db, int32_t *digital_gain_in_db);
void voice_uplink_dmic_db_transfer_to_analog_gain_and_digital_gain(int32_t input_db, int32_t *analog_gain_in_db, int32_t *digital_gain_in_db);
void voice_sidetone_db_make_sure_in_range(int32_t *input_db);


#ifdef __cplusplus
}
#endif

#endif  /* __AUDIO_DB_TO_GAIN_VALUE_MAPPING_TABLE_H__ */

