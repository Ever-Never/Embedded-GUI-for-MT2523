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

#include <stdint.h> // uint32_t, int32_t..

#include "audio_dsp_fd216_db_to_gain_value_mapping_table.h"
#include "syslog.h"


//#define NOT_PRINT_MESSAGE
#ifdef NOT_PRINT_MESSAGE
#define LOGE(fmt,arg...)   {}
#define LOGW(fmt,arg...)   {}
#define LOGI(fmt,arg...)   {}
#else
log_create_module(gain_value_mapping, PRINT_LEVEL_INFO);
#define LOGE(fmt,arg...)   LOG_E(gain_value_mapping, "[gain_value_mapping]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(gain_value_mapping, "[gain_value_mapping]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(gain_value_mapping,"[gain_value_mapping]"fmt,##arg)
#endif


static void truncate_out_of_range_value(int32_t *truncate_value, int32_t minimum, int32_t maximum)
{
    *truncate_value = *truncate_value < minimum ? minimum : *truncate_value;
    *truncate_value = *truncate_value > maximum ? maximum : *truncate_value;
}

static uint32_t a_value_linear_transfer_to_b_value(int32_t input_a, int32_t maximum_a_value, int32_t minimum_a_value, int32_t a_step_value_in_absolute, int32_t maximum_b_value, int32_t minimum_b_value, int32_t minimum_a_to_minimum_b_value)
{
    int32_t total_dB_steps = (maximum_a_value - minimum_a_value) / a_step_value_in_absolute;
    int32_t b_value_step_in_absolute = (maximum_b_value - minimum_b_value) / total_dB_steps;    // positive
    int32_t intput_a_to_a_step = 0;
    int32_t b_value = 0;

    truncate_out_of_range_value(&input_a, minimum_a_value, maximum_a_value);

    intput_a_to_a_step = (input_a - minimum_a_value) / a_step_value_in_absolute;   // always positive; our equation is from a side

    if (minimum_a_to_minimum_b_value) {
        b_value = minimum_b_value + b_value_step_in_absolute * intput_a_to_a_step;  // our view of equation is from a side
    } else {
        b_value = maximum_b_value - b_value_step_in_absolute * intput_a_to_a_step;  // our view of equation is from a side
    }

    LOGI("intput_a=%d, total_dB_steps=%d, b_value_step_in_absolute=%d, intput_a_to_a_step=%d, b_value=%d\n", input_a, total_dB_steps, b_value_step_in_absolute, intput_a_to_a_step, b_value);
    truncate_out_of_range_value(&b_value, minimum_b_value, maximum_b_value);
    LOGI("b_value=%d\n", b_value);


    return b_value;
}


void voice_sidetone_db_make_sure_in_range(int32_t *input_db)
{
    truncate_out_of_range_value(input_db, VOICE_SIDETONE_DB_MINIMUM, VOICE_SIDETONE_DB_MAXIMUM);
}


void voice_uplink_dmic_db_transfer_to_analog_gain_and_digital_gain(int32_t input_db, int32_t *analog_gain_in_db, int32_t *digital_gain_in_db)
{
    int32_t db_max = 0;
    int32_t db_min = 0;
    int32_t db_step_in_absolute = VOICE_UPLINK_DMIC_DB_STEP;
    int32_t b_value_max = 0;   // b is the value we want to convert to from our view.  db value ->  b value
    int32_t b_value_min = 0;
    int32_t minimum_db_to_minimum_b_value = 1;

    truncate_out_of_range_value(&input_db, VOICE_UPLINK_DMIC_DB_MINIMUM, VOICE_UPLINK_DMIC_DB_MAXIMUM);

    if (input_db <= VOICE_UPLINK_DMIC_DB_MAXIMUM && input_db >= VOICE_UPLINK_DMIC_DB_MINIMUM) {
        *analog_gain_in_db = 0;

        db_max = VOICE_UPLINK_DMIC_DB_MAXIMUM;
        db_min = VOICE_UPLINK_DMIC_DB_MINIMUM;
        db_step_in_absolute = 1;
        b_value_max = 40;
        b_value_min = 0;
        minimum_db_to_minimum_b_value = 1;

        *digital_gain_in_db = a_value_linear_transfer_to_b_value(input_db, db_max, db_min, db_step_in_absolute, b_value_max, b_value_min, minimum_db_to_minimum_b_value);
    }

    LOGI("voice uplink DMIC input_db=%d, analog gain=%d, digital gain=%d\n", input_db, *analog_gain_in_db, *digital_gain_in_db);
}


void voice_uplink_amp_db_transfer_to_analog_gain_and_digital_gain(int32_t input_db, int32_t *analog_gain_in_db, int32_t *digital_gain_in_db)
{
    int32_t db_max = 0;
    int32_t db_min = 0;
    int32_t db_step_in_absolute = VOICE_UPLINK_AMP_DB_STEP;
    int32_t b_value_max = 0;   // b is the value we want to convert to from our view.  db value ->  b value
    int32_t b_value_min = 0;
    int32_t minimum_db_to_minimum_b_value = 1;

    truncate_out_of_range_value(&input_db, VOICE_UPLINK_AMP_DB_MINIMUM, VOICE_UPLINK_AMP_DB_MAXIMUM);

    if (input_db <= VOICE_UPLINK_AMP_DB_MAXIMUM && input_db >= 31) {
        *analog_gain_in_db = 18;

        db_max = 45;
        db_min = 31;
        db_step_in_absolute = 1;
        b_value_max = 23;
        b_value_min = 9;
        minimum_db_to_minimum_b_value = 1;

        *digital_gain_in_db = a_value_linear_transfer_to_b_value(input_db, db_max, db_min, db_step_in_absolute, b_value_max, b_value_min, minimum_db_to_minimum_b_value);
    } else if (input_db <= 30 && input_db >= 25) {
        *analog_gain_in_db = 12;

        db_max = 30;
        db_min = 25;
        db_step_in_absolute = 1;
        b_value_max = 14;
        b_value_min = 9;
        minimum_db_to_minimum_b_value = 1;

        *digital_gain_in_db = a_value_linear_transfer_to_b_value(input_db, db_max, db_min, db_step_in_absolute, b_value_max, b_value_min, minimum_db_to_minimum_b_value);
    } else if (input_db <= 24 && input_db >= 19) {
        *analog_gain_in_db = 6;

        db_max = 24;
        db_min = 19;
        db_step_in_absolute = 1;
        b_value_max = 14;
        b_value_min = 9;
        minimum_db_to_minimum_b_value = 1;

        *digital_gain_in_db = a_value_linear_transfer_to_b_value(input_db, db_max, db_min, db_step_in_absolute, b_value_max, b_value_min, minimum_db_to_minimum_b_value);
    } else if (input_db <= 18 && input_db >= VOICE_UPLINK_AMP_DB_MINIMUM) {
        *analog_gain_in_db = 0;

        db_max = 18;
        db_min = 0;
        db_step_in_absolute = 1;
        b_value_max = 14;
        b_value_min = -4;
        minimum_db_to_minimum_b_value = 1;

        *digital_gain_in_db = a_value_linear_transfer_to_b_value(input_db, db_max, db_min, db_step_in_absolute, b_value_max, b_value_min, minimum_db_to_minimum_b_value);
    } else {
        // never here
    }

    LOGI("voice uplink amp input_db=%d, analog gain=%d, digital gain=%d\n", input_db, *analog_gain_in_db, *digital_gain_in_db);
}


void voice_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(int32_t input_db, int32_t *analog_gain_in_db, int32_t *digital_gain_in_db)
{
    int32_t db_max = 0;
    int32_t db_min = 0;
    int32_t db_step_in_absolute = VOICE_DOWNLINK_AMP_DB_STEP;
    int32_t b_value_max = 0;   // b is the value we want to convert to from our view.  db value ->  b value
    int32_t b_value_min = 0;
    int32_t minimum_db_to_minimum_b_value = 1;

    truncate_out_of_range_value(&input_db, VOICE_DOWNLINK_AMP_DB_MINIMUM, VOICE_DOWNLINK_AMP_DB_MAXIMUM);

    if (input_db <= VOICE_DOWNLINK_AMP_DB_MAXIMUM && input_db >= VOICE_DOWNLINK_AMP_DB_MINIMUM) {
        *digital_gain_in_db = 0;

        db_max = 7;
        db_min = -23;
        db_step_in_absolute = 2;
        b_value_max = 8;
        b_value_min = -22;
        minimum_db_to_minimum_b_value = 1;

        *analog_gain_in_db = a_value_linear_transfer_to_b_value(input_db, db_max, db_min, db_step_in_absolute, b_value_max, b_value_min, minimum_db_to_minimum_b_value);
    } else {
        // never here
    }

    LOGI("voice downlink amp input_db=%d, analog gain=%d, digital gain=%d\n", input_db, *analog_gain_in_db, *digital_gain_in_db);
}


void audio_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(int32_t input_db, int32_t *analog_gain_in_db, int32_t *digital_gain_in_db)
{
    int32_t db_max = 0;
    int32_t db_min = 0;
    int32_t db_step_in_absolute = ADUIO_DOWNLINK_AMP_DB_STEP;
    int32_t b_value_max = 0;   // b is the value we want to convert to from our view.  db value ->  b value
    int32_t b_value_min = 0;
    int32_t minimum_db_to_minimum_b_value = 1;

    truncate_out_of_range_value(&input_db, AUDIO_DOWNLINK_AMP_DB_MINIMUM, AUDIO_DOWNLINK_AMP_DB_MAXIMUM);

    if (input_db <= AUDIO_DOWNLINK_AMP_DB_MAXIMUM && input_db >= -16) {
        *digital_gain_in_db = 0;

        db_max = -2;
        db_min = -16;
        db_step_in_absolute = 2;
        b_value_max = 11;
        b_value_min = -3;
        minimum_db_to_minimum_b_value = 1;

        *analog_gain_in_db = a_value_linear_transfer_to_b_value(input_db, db_max, db_min, db_step_in_absolute, b_value_max, b_value_min, minimum_db_to_minimum_b_value);
    } else if (input_db == -17) {
        *analog_gain_in_db = -3;
        *digital_gain_in_db = -2;
    } else if (input_db <= -18 && input_db >= AUDIO_DOWNLINK_AMP_DB_MINIMUM) {
        *analog_gain_in_db = -3;

        db_max = -18;
        db_min = -76;
        db_step_in_absolute = 2;
        b_value_max = -1;
        b_value_min = -60;
        minimum_db_to_minimum_b_value = 1;

        *digital_gain_in_db = a_value_linear_transfer_to_b_value(input_db, db_max, db_min, db_step_in_absolute, b_value_max, b_value_min, minimum_db_to_minimum_b_value);
    } else {
        // never here
    }

    LOGI("audio downlink amp input_db=%d, analog gain=%d, digital gain=%d\n", input_db, *analog_gain_in_db, *digital_gain_in_db);
}
