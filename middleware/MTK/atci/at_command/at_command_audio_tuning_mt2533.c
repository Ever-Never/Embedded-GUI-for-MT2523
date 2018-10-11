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

// For Register AT command handler
#include "at_command.h"
// System head file
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// EADP EAPS header file
#include "audio_coefficient.h"
#include "audio_nvdm.h"

// Audio db to gain value mapping table
#include "audio_dsp_fd216_db_to_gain_value_mapping_table.h"

// for audio tuning tool test (ATTTEST)
#include "hal_audio.h"

#include "memory_attribute.h"

// for audio_update_iir_design
#include "hal_audio_post_processing.h"

#include "hal_audio_enhancement.h"

#ifdef MTK_EXTERNAL_DSP_ENABLE
#include "external_dsp_application.h"
#endif
/*
 * audio tuning tool
*/

/*---  Define ---*/
#define ITEM_SIZE 2

#define EAPS_CONTENT_MAX_SIZE 127
#define EAPS_CONTENT_MIN_SIZE 4

#define ATTDET_CONTENT_MAX_SIZE 50  // temp size
#define ATTDET_CONTENT_MIN_SIZE 4

#define ATTTEST_CONTENT_MAX_SIZE 10  // temp size
#define ATTTEST_CONTENT_MIN_SIZE 2

#define USER_BUFFER_SIZE 2048*2

#define NOT_PRINT_MESSAGE
log_create_module(audio_tuning_tool, PRINT_LEVEL_INFO);
#ifdef NOT_PRINT_MESSAGE
#define LOGE(fmt,arg...)   {}
#define LOGW(fmt,arg...)   {}
#define LOGI(fmt,arg...)   {}
#else
#define LOGE(fmt,arg...)   LOG_E(audio_tuning_tool, "[audio_tuning_tool]"fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(audio_tuning_tool, "[audio_tuning_tool]"fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(audio_tuning_tool,"[audio_tuning_tool]"fmt,##arg)
#endif
#define LOGP(fmt,arg...)   LOG_I(audio_tuning_tool,"[audio_tuning_tool]"fmt,##arg)      // it will print event #define NOT_PRINT_MESSAGE




typedef enum {
    ATTDET_ERROR = -1,
    ATTDET_NO_ERROR = 0,
} attdet_error_check_t;



typedef enum {
    ATTDET_PARAMETER_INDEX_SPEECH_COMMON_PARAMETER = 0,
    ATTDET_PARAMETER_INDEX_AUDIO_GAIN_INFO_HEADSET = 0x10,
    ATTDET_PARAMETER_INDEX_AUDIO_GAIN_INFO_HANDSFREE = 0x11,
    ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_HEADSET = 0x12,
    ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_HANDSFREE = 0x13,
    ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_RESERVED = 0x14,
    ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_NORMAL = 0x15,
    ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_ASR = 0x16,
    ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_STEREO = 0x17,
    ATTDET_PARAMETER_INDEX_AUDIO_IIR_FILTER_INFO = 0x20,
    ATTDET_PARAMETER_INDEX_VOICE_NB_MODE_AND_FIR_PARAMETER_INFO = 0x21,
    ATTDET_PARAMETER_INDEX_VOICE_WB_MODE_AND_FIR_PARAMETER_INFO = 0x22,
    ATTDET_PARAMETER_INDEX_RECORD_WB_MODE_AND_FIR_PARAMETERS_INFO = 0x23,

    ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_HEADSET_A_MIC_MT2533 = 0x60,    // (for voice NB/WB mode1~3)
    ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_HEADSET_DUAL_DIGITAL_MIC_MT2523 = 0x61,    // (for voice NB/WB mode4,5)
    ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_HANDSFREE_DUAL_DIGITAL_MIC_MT2523 = 0x62,    // (for voice NB/WB mode ?)
    ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_A_MIC_MT2533 = 0x63,    // (for record wb mode 1)
    ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_DUAL_DIGITAL_MIC_MT2533 = 0x64,    // (for record wb mode 2~3)

    ATTDET_PARAMETER_INDEX_VOICE_NB_MODE_AND_FIR_INFO_DUAL_DIGITAL_MIC_MT2533 = 0x70, // (for voice NB mode4~6)
    ATTDET_PARAMETER_INDEX_VOICE_WB_ENHANCEMENT_INFO_DUAL_DIGITAL_MIC_MT2533 = 0x71, // (for voice WB mode4~6)

    ATTDET_PARAMETER_INDEX_RECORD_WB_ENHANCEMENT_INFO_DUAL_DIGITAL_MIC_MT2533 = 0x72, // (for record WB mode1)

    ATTDET_PARAMETER_INDEX_EXTERNAL_DSP_SENDING_PATH_INFO_MT2533 = 0x80, // (FOR VOICE/RECORD DUAL DIGITAL MIC MT2533)

    ATTDET_PARAMETER_INDEX_EXTERNAL_DSP_SENDING_PATH_POWER_ON_INDEX_VALUE_MT2533 = 0x90, // (FOR VOICE/RECORD DUAL DIGITAL MIC MT2533)
    ATTDET_PARAMETER_INDEX_EXTERNAL_DSP_SENDING_PATH_POWER_ON_REG_VALUE_MT2533 = 0x91, // (FOR VOICE/RECORD DUAL DIGITAL MIC MT2533)


    
    ATTDET_PARAMETER_INDEX_SUPPORT_IOT_PC_AUDIO_TUNING_TOOL = 0x400,
    ATTDET_PARAMETER_INDEX_SUPPORT_VOCIE_TEST = 0x401,
    ATTDET_PARAMETER_INDEX_SUPPORT_RECORD_TEST = 0x402,
    ATTDET_PARAMETER_INDEX_INVALID,
} attdet_parameter_index_t;

typedef enum {
    ATTDET_DEVICE_MODE_COMMON,
    ATTDET_DEVICE_MODE_GAIN_PARAMETER_INFORMATION = 0x10,
    ATTDET_DEVICE_MODE_ACF_OR_MODE_OR_FIR_INFORMATION = 0x20,
    ATTDET_DEVICE_MODE_SUPORT_FEATURE_SUPPORT_TOOL = 0x100,
    ATTDET_DEVICE_MODE_INVALID,
} attdet_device_mode_t;

typedef enum {
    ATTDET_SCENARIO_COMMON,
    ATTDET_SCENARIO_AUDIO,
    ATTDET_SCENARIO_VOICE,
    ATTDET_SCENARIO_RECORD,
    ATTDET_SUPPORT_FEATURE = 0x10,
    ATTDET_SCENARIO_INVALID,
} attdet_scenario_t;

typedef enum {
    ATTDET_OPERATION_GET_OLD_VALUE,
    ATTDET_OPERATION_SET_NEW_VALUE,
    ATTDET_OPERATION_INVALID,
} attdet_operation_t;




typedef enum {
    NB,
    WB,
} nb_or_wb_t;

typedef enum {
    ATTTEST_ERROR = -1,
    ATTTEST_NO_ERROR = 0,
} atttest_error_check_t;

typedef enum {
    ATTTEST_DEVICE_MODE_AUDIO_ACF_MODE_1 = 0x1,
    ATTTEST_DEVICE_MODE_AUDIO_ACF_MODE_2,
    ATTTEST_DEVICE_MODE_AUDIO_ACF_MODE_3,
    ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1,
    ATTTEST_DEVICE_MODE_VOICE_NB_MODE_2,
    ATTTEST_DEVICE_MODE_VOICE_NB_MODE_3,
    ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1,
    ATTTEST_DEVICE_MODE_VOICE_WB_MODE_2,
    ATTTEST_DEVICE_MODE_VOICE_WB_MODE_3,
    ATTTEST_DEVICE_MODE_RECORD_WB_MODE_1,
    ATTTEST_DEVICE_MODE_RECORD_WB_MODE_2,
    ATTTEST_DEVICE_MODE_RECORD_WB_MODE_3,
    ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1_MT2533 = 0x60,
    ATTTEST_DEVICE_MODE_VOICE_NB_MODE_2_MT2533 = 0x61,
    ATTTEST_DEVICE_MODE_VOICE_NB_MODE_3_MT2533 = 0x62,
    ATTTEST_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 = 0x63,
    ATTTEST_DEVICE_MODE_VOICE_NB_MODE_5_MT2533 = 0x64,
    ATTTEST_DEVICE_MODE_VOICE_NB_MODE_6_MT2533 = 0x65,
    ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1_MT2533 = 0x66,
    ATTTEST_DEVICE_MODE_VOICE_WB_MODE_2_MT2533 = 0x67,
    ATTTEST_DEVICE_MODE_VOICE_WB_MODE_3_MT2533 = 0x68,
    ATTTEST_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 = 0x69,
    ATTTEST_DEVICE_MODE_VOICE_WB_MODE_5_MT2533 = 0x6A,
    ATTTEST_DEVICE_MODE_VOICE_WB_MODE_6_MT2533 = 0x6B, 
    ATTTEST_DEVICE_MODE_STOP_STREAM_OUT = 0x100,
    ATTTEST_DEVICE_MODE_STOP_STREAM_IN = 0x101,
    ATTTEST_DEVICE_MODE_STOP_STREAM_OUT_AND_STREAM_IN = 0x102,
    ATTTEST_DEVICE_MODE_STOP_AUDIO = 0x103,
    ATTTEST_DEVICE_MODE_STOP_VOICE = 0x104,
    ATTTEST_DEVICE_MODE_STOP_RECORD = 0x105,
    ATTTEST_DEVICE_MODE_TEMP_DSP_SENDING_PATH_REGISTER = 0x1000,
    ATTTEST_DEVICE_MODE_TEMP_DSP_FD216_REGISTER_VALUE = 0x1010,
    ATTTEST_DEVICE_MODE_TEMP_DSP_SAVE_REGISTER_DATA_TO_NVDM = 0x1100,
    ATTTEST_DEVICE_MODE_TEMP_DSP_CONFIG_PATH_SET = 0x1200,
    ATTTEST_DEVICE_MODE_TEMP_DSP_CONFIG_PATH_GET = 0x1300,
    ATTTEST_DEVICE_MODE_INVALID,
} atttest_device_mode_t;

typedef enum {
    ATTTEST_SCENARIO_AUDIO = 0x1,
    ATTTEST_SCENARIO_VOICE,
    ATTTEST_SCENARIO_RECORD,
    ATTTEST_SCENARIO_STOP = 0x10,
    ATTTEST_SCENARIO_TEMP_DSP_GET_REGISTER_DATA = 0x100,
    ATTTEST_SCENARIO_TEMP_DSP_SET_REGISTER_DATA = 0x101,
    ATTTEST_SCENARIO_TEMP_DSP_SAVE_REGISTER_DATA_TO_NVDM = 0x110,
    ATTTEST_SCENARIO_TEMP_DSP_CONFIG_PATH = 0x111,
    ATTTEST_SCENARIO_INVALID,
} atttest_scenario_t;

typedef enum {
    ATTTEST_STRUCUTRE_CHECK_NO_ERROR,
    ATTTEST_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE,
} atttest_structure_check_t;


typedef enum {
    AT_COMMAND_NO_ERROR,
    AT_COMMAND_TOO_MANY_CONTENTS,
    AT_COMMAND_TOO_LESS_CONTENTS,
    AT_COMMAND_NOT_DEFINE_YET,
} AT_command_error_check_t;

typedef enum {
    EADP_OPERATION_GET_OLD_VALUE,
    EADP_OPERATION_SET_NEW_VALUE,
    EADP_OPERATION_INVALID,
} EADP_operation_t;

typedef enum {
    EADP_SCENARIO_AUDIO,
    EADP_SCENARIO_VOICE,
    EADP_SCENARIO_RECORD,
    EADP_SCENARIO_INVALID,
} EADP_scenario_t;

typedef enum {
    EADP_DEVICE_MODE_HANDSFREE,
    EADP_DEVICE_MODE_HEADSET,
    EADP_DEVICE_MODE_INVALID,
} EADP_device_mode_t;

typedef enum {
    EADP_STRUCUTRE_CHECK_NO_ERROR,
    EADP_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE,
} EADP_structure_check_t;


typedef enum {
    EAPS_OPERATION_GET_OLD_VALUE,
    EAPS_OPERATION_SET_NEW_VALUE,
    EAPS_OPERATION_INVALID,
} EAPS_operation_t;

typedef enum {
    EAPS_SCENARIO_COMMON,
    EAPS_SCENARIO_AUDIO,
    EAPS_SCENARIO_VOICE,
    EAPS_SCENARIO_RECORD,
    EAPS_SCENARIO_SAVE_DATA_TO_NVDM = 0x900,
    EAPS_SCENARIO_INVALID,
} EAPS_scenario_t;

typedef enum {
    EAPS_DEVICE_MODE_COMMON,
    EAPS_DEVICE_MODE_AUDIO_GAIN_MODE,
    EAPS_DEVICE_MODE_AUDIO_ACF_MODE_1,
    EAPS_DEVICE_MODE_AUDIO_ACF_MODE_2,
    EAPS_DEVICE_MODE_AUDIO_ACF_MODE_3,
    EAPS_DEVICE_MODE_VOICE_NB_MODE_1,
    EAPS_DEVICE_MODE_VOICE_NB_MODE_2,
    EAPS_DEVICE_MODE_VOICE_NB_MODE_3,
    EAPS_DEVICE_MODE_VOICE_WB_MODE_1,
    EAPS_DEVICE_MODE_VOICE_WB_MODE_2,
    EAPS_DEVICE_MODE_VOICE_WB_MODE_3,
    EAPS_DEVICE_MODE_RECORD_WB_MODE_1,
    EAPS_DEVICE_MODE_RECORD_WB_MODE_2,
    EAPS_DEVICE_MODE_RECORD_WB_MODE_3,
    EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533 = 0x60, // if chip is mt2533, then voice use this. (HEADSET + HEADSET MIC)
    EAPS_DEVICE_MODE_VOICE_NB_MODE_2_MT2533 = 0x61, // if chip is mt2533, then voice use this. (RESERVED FOR A-MIC)
    EAPS_DEVICE_MODE_VOICE_NB_MODE_3_MT2533 = 0x62, // if chip is mt2533, then voice use this. (RESERVED FOR A-MIC)
    EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 = 0x63, // if chip is mt2533, then voice use this. (HEADSET + DUAL Digital MIC)
    EAPS_DEVICE_MODE_VOICE_NB_MODE_5_MT2533 = 0x64, // if chip is mt2533, then voice use this. (RESERVED1, HEADSET + DUAL Digital MIC)
    EAPS_DEVICE_MODE_VOICE_NB_MODE_6_MT2533 = 0x65, // if chip is mt2533, then voice use this.  (RESERVED2)
    EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533 = 0x66, // if chip is mt2533, then voice use this.  (HEADSET + HEADSET MIC)
    EAPS_DEVICE_MODE_VOICE_WB_MODE_2_MT2533 = 0x67, // if chip is mt2533, then voice use this.  (RESERVED FOR A-MIC)
    EAPS_DEVICE_MODE_VOICE_WB_MODE_3_MT2533 = 0x68, // if chip is mt2533, then voice use this.  (RESERVED FOR A-MIC)
    EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 = 0x69, // if chip is mt2533, then voice use this.  (HEADSET + DUAL Digital MIC)
    EAPS_DEVICE_MODE_VOICE_WB_MODE_5_MT2533 = 0x6A, // if chip is mt2533, then voice use this.  (RESERVED1, HEADSET + DUAL Digital MIC)
    EAPS_DEVICE_MODE_VOICE_WB_MODE_6_MT2533 = 0x6B, // if chip is mt2533, then voice use this.  (RESERVED2)
    EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533 = 0x80, // if chip is mt2533, then record use this.  (A-MIC [HEADSET MIC or MAIN MIC])
    EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533 = 0x81, // if chip is mt2533, then record use this.  (Dual Digital MIC)
    EAPS_DEVICE_MODE_RECORD_WB_MODE_3_MT2533 = 0x82, // if chip is mt2533, then record use this.  (RESERVED)    

    
    EAPS_DEVICE_MODE_SAVE_DATA_TO_NVDM = 0x900,
    EAPS_DEVICE_MODE_INVALID,
} EAPS_device_mode_t;

typedef enum {
    EAPS_PARAMETER_INDEX_SPEECH_COMMON_PARAMETER,
    EAPS_PARAMETER_INDEX_GAIN_PARAMETER = 0x1,
    EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_1 = 0x100,
    EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_2 = 0x101,
    EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_3 = 0x102,
    EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_4 = 0x103,
    EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_5 = 0x104,
    EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_6 = 0x105,
    EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_7 = 0x106,
    EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_8 = 0x107,
    EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_9 = 0x108,
    EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_10 = 0x109,
    EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_11 = 0x10A,
    EAPS_PARAMETER_INDEX_VOICE_NB_MODE_PARAMETERS = 0x200,
    EAPS_PARAMETER_INDEX_VOICE_NB_INPUT_FIR_COEFFICIENTS = 0x201,
    EAPS_PARAMETER_INDEX_VOICE_NB_OUTPUT_FIR_COEFFICIENTS = 0x202,
    EAPS_PARAMETER_INDEX_VOICE_WB_MODE_PARAMETERS = 0x203,
    EAPS_PARAMETER_INDEX_VOICE_WB_INPUT_FIR_COEFFICIENTS = 0x204,
    EAPS_PARAMETER_INDEX_VOICE_WB_OUTPUT_FIR_COEFFICIENTS = 0x205,
    EAPS_PARAMETER_INDEX_RECORD_MODE_PARAMETERS = 0x300,
    EAPS_PARAMETER_INDEX_RECORD_CHANNEL_1_INPUT_FIR_COEFFICIENTS = 0x301,
    EAPS_PARAMETER_INDEX_RECORD_CHANNEL_2_INPUT_FIR_COEFFICIENTS = 0x302,
    EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_INDEX_VALUE = 0x400,
    EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_REGISTER_VALUE = 0x401,
    EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_NEED_TO_DOWNLOAD = 0x402,
    EAPS_PARAMETER_INDEX_INVALID,
    EAPS_PARAMETER_SAVE_DATA_TO_NVDM = 0x900,
} EAPS_parameter_index_t;

typedef enum {
    EAPS_GAIN_PARAMETER_CHOOSE_HEADSET = 0,
    EAPS_GAIN_PARAMETER_CHOOSE_HANDSFREE = 1,
} EAPS_gain_parameter_choose_which_device_t;

typedef enum {
    EAPS_STRUCUTRE_CHECK_NO_ERROR,
    EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE,
} EAPS_structure_check_t;


typedef enum {
    AUDIO_TEST_IS_RUNNING,
    AUDIO_TEST_IS_NOT_RUNNING,
} atttest_audio_test_running_state_t;

typedef enum {
    RECORD_TEST_IS_NOT_RUNNING,
    RECORD_TEST_IS_RUNNING,
} atttest_record_test_running_state_t;



typedef struct {
    uint8_t audio_test_is_running;
    hal_audio_device_t audio_device;
    hal_audio_channel_number_t mono_or_stereo;
    hal_audio_sampling_rate_t sampling_rate;
    uint32_t tone_hz;
    uint32_t selected_stream_out_volume_level;
    uint32_t selected_side_tone_volume_level;
    uint32_t selected_stream_in_volume_level;
} atttest_audio_struct_t;


typedef struct {
    uint8_t record_test_is_running;
    uint32_t start_record_time_in_ms;
    uint32_t wanted_record_time_in_ms;
} atttest_record_struct_t;


typedef struct {
    atttest_device_mode_t now_device_mode;
    uint32_t now_stream_out_level;
    uint32_t now_stream_in_level;    
} atttest_nvdm_callback_useful_info_t;


// default no record function, since we must have SD card & file system
// if want to enable, make sure you have SD card & have file system.
// this is just for internal used.
// 1. add fatfs file feature in GCC/Makefile
// 2. define RECORD_SDFATFS_ENABLE
//#define RECORD_SDFATFS_ENABLE
#if defined(RECORD_SDFATFS_ENABLE)
#include "ff.h"
FATFS fatfs;
FIL fdst;
#endif


ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t *audio_internal_ring_buffer; // for audio hal internal ring buffer used
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t *record_internal_ring_buffer; // for record hal internal ring buffer used

atttest_nvdm_callback_useful_info_t atttest_nvdm_callback_useful_info;


static uint16_t user_buffer[USER_BUFFER_SIZE];


static atttest_audio_struct_t atttest_audio_struct;
#if defined(RECORD_SDFATFS_ENABLE)
static atttest_record_struct_t atttest_record_struct = {0};
#endif
static atttest_audio_test_running_state_t atttest_audio_test_running_state = AUDIO_TEST_IS_NOT_RUNNING;
//static atttest_record_test_running_state_t atttest_record_test_running_state = RECORD_TEST_IS_NOT_RUNNING;

static int16_t voice_test_start = 0;
static uint16_t *stream_in_stream_out_buffer_ptr = NULL;


static void atttest_audio_stop_stream_out(void)
{
    hal_audio_stop_stream_out();
}

static void atttest_audio_stop_stream_in(void)
{
    hal_audio_stop_stream_in();
}


static void atttest_stop_audio(void)
{
    if (atttest_audio_test_running_state == AUDIO_TEST_IS_RUNNING) {
        atttest_audio_stop_stream_out();

        //Free the internal memory allocation
        if (audio_internal_ring_buffer != NULL) {
            vPortFree(audio_internal_ring_buffer);
            audio_internal_ring_buffer = NULL;
        }

        atttest_audio_test_running_state = AUDIO_TEST_IS_NOT_RUNNING;
    }
}

static void atttest_stop_voice(void)
{
    if (voice_test_start == 1) {
        speech_set_enhancement(false);    // for test side tone
        // in pcm2way the api of atttest_audio_stop_stream_out() has stop stream in function
        atttest_audio_stop_stream_out();

        //Free the internal memory allocation
        if (stream_in_stream_out_buffer_ptr != NULL) {
            vPortFree(stream_in_stream_out_buffer_ptr);
            stream_in_stream_out_buffer_ptr = NULL;
        }

        voice_test_start = 0;
    }
}

static void atttest_stop_record(void)
{
#if defined(RECORD_SDFATFS_ENABLE)
    FRESULT fatfs_res;

    if (atttest_record_struct.record_test_is_running = RECORD_TEST_IS_RUNNING) {

        fatfs_res = f_close(&fdst);
        atttest_audio_stop_stream_in();

        //Free the internal memory allocation
        if (record_internal_ring_buffer != NULL) {
            vPortFree(record_internal_ring_buffer);
            record_internal_ring_buffer = NULL;
        }

        atttest_record_struct.record_test_is_running = RECORD_TEST_IS_NOT_RUNNING;
        atttest_record_struct.wanted_record_time_in_ms = 0;
        atttest_record_struct.start_record_time_in_ms = 0;
    }
#endif
}


void truncate_out_of_range_value(uint32_t *truncate_value, uint32_t minimum, uint32_t maximum)
{
    *truncate_value = *truncate_value < minimum ? minimum : *truncate_value;
    *truncate_value = *truncate_value > maximum ? maximum : *truncate_value;
}

uint32_t defined_sampling_rate_enum_to_defined_sampling_rate_value(hal_audio_sampling_rate_t hal_audio_sampling_rate_enum)
{
    switch (hal_audio_sampling_rate_enum) {
        case HAL_AUDIO_SAMPLING_RATE_8KHZ:
            return 8000;
        case HAL_AUDIO_SAMPLING_RATE_11_025KHZ:
            return 11025;
        case HAL_AUDIO_SAMPLING_RATE_12KHZ:
            return 12000;
        case HAL_AUDIO_SAMPLING_RATE_16KHZ:
            return 16000;
        case HAL_AUDIO_SAMPLING_RATE_22_05KHZ:
            return 22050;
        case HAL_AUDIO_SAMPLING_RATE_24KHZ:
            return 24000;
        case HAL_AUDIO_SAMPLING_RATE_32KHZ:
            return 32000;
        case HAL_AUDIO_SAMPLING_RATE_44_1KHZ:
            return 44100;
        case HAL_AUDIO_SAMPLING_RATE_48KHZ:
            return 48000;
        case HAL_AUDIO_SAMPLING_RATE_96KHZ:
            return 96000;

        default:
            return 8000;
    }
}

hal_audio_sampling_rate_t modify_user_sampling_rate_to_near_floor_value_of_defined_sampling_rate_enum(uint32_t user_sampling_rate)
{
#define DEFINDED_SAMPLING_AMOUNT 10   // refer to hal_audio.h
    uint32_t defined_sampling_rate[DEFINDED_SAMPLING_AMOUNT] = {8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 96000}; // refer to hal_audio.h
    int i = DEFINDED_SAMPLING_AMOUNT - 1;
    hal_audio_sampling_rate_t ret_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_8KHZ;

    for (i = DEFINDED_SAMPLING_AMOUNT - 1; i >= 0; i--) {
        if (((int32_t)user_sampling_rate - (int32_t)defined_sampling_rate[i]) >= 0) {
            break;
        }
    }

    if (i < 0) {
        i = 0;
    }

    ret_audio_sampling_rate = (hal_audio_sampling_rate_t)i;

    return ret_audio_sampling_rate;
}


static void atttest_generate_tone_with_sinwave(uint32_t *amount_of_wanted_generating_tone_samples, uint16_t *output_tone_data_buffer, int output_tone_data_buffer_size, uint32_t wanted_tone_hz, uint32_t sampling_rate)
{
    static uint32_t tone_i = 0;
    int generated_samples = (*amount_of_wanted_generating_tone_samples > output_tone_data_buffer_size) ? output_tone_data_buffer_size : *amount_of_wanted_generating_tone_samples;
    int generated_sample_count = 0;
    float temp_tone = 0;

    uint16_t temp_uint16 = 0;
    int16_t temp_int16 = 0;


    float pi = 3.14159265358979;
    float temp_float_1 = 0;
    float temp_float_2 = 0;
    float temp_float_3 = 0;

    float temp_tone_2 = 0;
    int16_t temp_int16_2 = 0;

    //LOGI("QtoneHZ = %d, sampling = %d\n", wanted_tone_hz, sampling_rate);

    *amount_of_wanted_generating_tone_samples = generated_samples;
    LOGI("generated_samples = %d, wanted_tone_hz =%d, tone_i = %d, sampling_rate = %d\n", generated_samples, wanted_tone_hz, tone_i, sampling_rate);

    for (generated_sample_count = 0; generated_sample_count < generated_samples; generated_sample_count++) {
        //temp_tone = 32767 * sin((float)2 * (float)3.14159265358979 * (float)wanted_tone_hz * (float)tone_i / (float)sampling_rate);

        // vvvv mapping to -pi ~ pi vvvv
        temp_float_1 = (float)2 * pi * (float)wanted_tone_hz * (float)tone_i / (float)sampling_rate;

        temp_float_3 = temp_float_1 / (2 * pi);
        temp_int16_2 = (int)temp_float_3;


        temp_float_1 = temp_float_1 - temp_int16_2 * (2 * pi);

        temp_float_1 = (temp_float_1 > pi) ? (temp_float_1 - 2 * pi) : temp_float_1;
        // ^^^ mapping to -pi ~ pi ^^^


        // vvvv sine function using parabola equation to improve the speed vvvv
        if (temp_float_1 < 0) {
            temp_float_2 = 1.27323954473516f * temp_float_1 + 0.40528473456935f * temp_float_1 * temp_float_1;

            if (temp_float_2 < 0) {
                temp_float_2 = 0.225f * (temp_float_2 * (-temp_float_2) - temp_float_2) + temp_float_2;
            } else {
                temp_float_2 = 0.225f * (temp_float_2 * temp_float_2 - temp_float_2) + temp_float_2;
            }
        } else {
            temp_float_2 = 1.27323954473516f * temp_float_1 - 0.40528473456935f * temp_float_1 * temp_float_1;

            if (temp_float_2 < 0) {
                temp_float_2 = 0.225f * (temp_float_2 * (-temp_float_2) - temp_float_2) + temp_float_2;
            } else {
                temp_float_2 = 0.225f * (temp_float_2 * temp_float_2 - temp_float_2) + temp_float_2;
            }
        }

        temp_tone_2 = 32767 * temp_float_2;

        temp_tone = temp_tone_2;

        // ^^^ sine function using parabola equation to improve the speed ^^^


        //LOGI("generated_sample_count =%d, wanted_tone_hz = %d, tone_i =%d, sampling_rate =%d\n", generated_sample_count, wanted_tone_hz, tone_i, sampling_rate);


        temp_int16 = (int16_t)temp_tone;
        //LOGI("temp_int16 = %d\n", temp_int16);
        //LOGI("%d\n", temp_int16);

        temp_uint16 = (uint16_t)temp_int16;
        //LOGI("temp_uint16_t = %x\n", temp_uint16);


        *(output_tone_data_buffer + generated_sample_count) = temp_uint16;
        //LOGI("*(output_tone_data_buffer + generated_sample_count) = %x\n", *(output_tone_data_buffer + generated_sample_count));

        tone_i++;

        if (tone_i >= sampling_rate) {
            tone_i = 0;
        }
    }

    // debug use
    //for (generated_sample_count = 0; generated_sample_count < generated_samples; generated_sample_count++) {
    //    LOGI("%x\n", *(output_tone_data_buffer + generated_sample_count));
    //}
}

static void atttest_fill_stream_out_buffer(uint32_t *amount_of_wanted_fill_tone_samples, uint16_t *stream_out_buffer, int stream_out_buffer_size, uint32_t wanted_tone_hz, uint32_t sampling_rate)
{
    int i = 0;
    uint32_t request_sinwave_samples = 0;

    *amount_of_wanted_fill_tone_samples = (*amount_of_wanted_fill_tone_samples > stream_out_buffer_size) ? stream_out_buffer_size : *amount_of_wanted_fill_tone_samples;

    request_sinwave_samples = *amount_of_wanted_fill_tone_samples / 2; 	// since audio data array is: array = {data1, data1, data2, data2, data3, data3....}  // left channel, right channel ... even mono is the same type.

    // since audio data array is: array = {data1, data1, data2, data2, data3, data3....}  // left channel, right channel ... even mono is the same type.
    request_sinwave_samples = (request_sinwave_samples > (stream_out_buffer_size / 2)) ? (stream_out_buffer_size / 2) : request_sinwave_samples;

    //LOGI("1:*amount_of_wanted_generating_tone_samples=%d, request_sinwave_samples=%d\n", *amount_of_wanted_fill_tone_samples, request_sinwave_samples);


    atttest_generate_tone_with_sinwave(&request_sinwave_samples, stream_out_buffer, stream_out_buffer_size, wanted_tone_hz, sampling_rate);

    for (i = (request_sinwave_samples - 1); i >= 0; i--) {
        stream_out_buffer[i * 2] = stream_out_buffer[i];
        stream_out_buffer[i * 2 + 1] = stream_out_buffer[i];
    }

    *amount_of_wanted_fill_tone_samples = request_sinwave_samples * 2;

    //LOGI("2:*amount_of_wanted_generating_tone_samples=%d, request_sinwave_samples=%d\n", *amount_of_wanted_fill_tone_samples, request_sinwave_samples);

    /*  // debug use
        for (i = 0; i < *amount_of_wanted_fill_tone_samples; i++) {
            temp_int16 = (int16_t)stream_out_buffer[i];
            LOGI("%d\n", temp_int16);
        }
    */

}


static void user_audio_stream_out_callback(hal_audio_event_t event, void *user_data)
{
    uint32_t sample_count = 0;
    uint32_t fill_tone_samples = 0;

    atttest_audio_struct_t *atttest_audio_struct_ptr = (atttest_audio_struct_t *)user_data;

    switch (event) {
        case HAL_AUDIO_EVENT_UNDERFLOW:
        case HAL_AUDIO_EVENT_DATA_REQUEST:
            //LOGI("callback++\n");
            hal_audio_get_stream_out_sample_count(&sample_count);   // note: this sample_count is in byte type, but the audio signal is in word(2 bytes)

            fill_tone_samples = sample_count / 2;    // note: this sample_count is in byte type, but the audio signal is in word(2 bytes)
            //LOGI("sine++\n");
            LOGI("toneHZ = %d, sampling = %d\n", atttest_audio_struct_ptr->tone_hz, defined_sampling_rate_enum_to_defined_sampling_rate_value(atttest_audio_struct_ptr->sampling_rate));


            atttest_fill_stream_out_buffer(&fill_tone_samples, user_buffer, USER_BUFFER_SIZE, atttest_audio_struct_ptr->tone_hz, defined_sampling_rate_enum_to_defined_sampling_rate_value(atttest_audio_struct_ptr->sampling_rate));
            //LOGI("sine--\n");
            //LOGI("fill_tone_samples = %d, tonehz =%d, user_buffer[0]= %d, user_buffer[1]= %d, user_buffer[2]= %d, user_buffer[3]= %d\n", fill_tone_samples, atttest_audio_struct_ptr->tone_hz, user_buffer[0], user_buffer[1], user_buffer[2], user_buffer[3]);

            hal_audio_write_stream_out(user_buffer, fill_tone_samples * 2);

            //LOGI("callback--\n");
            break;
        case HAL_AUDIO_EVENT_ERROR:
            //Error handler.
            break;
    }
}


static atttest_error_check_t audio_play(hal_audio_device_t stream_out_device, uint32_t stream_out_volume_db, hal_audio_sampling_rate_t audio_sampling_rate, uint32_t tone_hz)
{
    uint32_t needed_memory_size = 0;
    uint32_t wanted_samples = USER_BUFFER_SIZE;
    int32_t temp_input_db = (int32_t)stream_out_volume_db;
    int32_t temp_a_gain = 0;
    int32_t temp_d_gain = 0;
    uint32_t a_gain_in_hex = 0;
    uint32_t d_gain_in_hex = 0;


    hal_audio_set_stream_out_sampling_rate(audio_sampling_rate);
    hal_audio_set_stream_out_channel_number(HAL_AUDIO_STEREO);
    hal_audio_set_stream_out_device(stream_out_device);


    if (stream_out_device == HAL_AUDIO_DEVICE_HEADSET) {
        audio_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(temp_input_db, &temp_a_gain, &temp_d_gain);
    } else if (stream_out_device == HAL_AUDIO_DEVICE_HANDS_FREE_STEREO) {
        voice_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(temp_input_db, &temp_a_gain, &temp_d_gain);
    } else {
        // audio play never here
        return ATTTEST_NO_ERROR;
    }


    a_gain_in_hex = (uint32_t)(temp_a_gain * 100);  // since hal_audio_set_stream_out_volume's unit is 0.01db
    d_gain_in_hex = (uint32_t)(temp_d_gain * 100);

    LOGP("temp_input_db=%d, temp_a_gain=%d, temp_d_gain=%d, a_gain_in_hex=%x, d_gain_in_hex=%x\n", temp_input_db, temp_a_gain, temp_d_gain, a_gain_in_hex, d_gain_in_hex);

    hal_audio_set_stream_out_volume(d_gain_in_hex, a_gain_in_hex);


    atttest_audio_struct.audio_device = stream_out_device;
    atttest_audio_struct.mono_or_stereo = HAL_AUDIO_STEREO;
    atttest_audio_struct.sampling_rate = audio_sampling_rate;
    atttest_audio_struct.tone_hz = tone_hz;


    if (atttest_audio_test_running_state == AUDIO_TEST_IS_NOT_RUNNING) {

        hal_audio_register_stream_out_callback(user_audio_stream_out_callback, &atttest_audio_struct);

        // Get the amount of internal memory usage.
        if (audio_internal_ring_buffer == NULL) {
            hal_audio_get_memory_size(&needed_memory_size);
            audio_internal_ring_buffer = pvPortMalloc(needed_memory_size * sizeof(uint8_t));

            if (audio_internal_ring_buffer == NULL) {
                LOGP("audio_play: audio_internal_ring_buffer=NULL, needed_memory_size=%d, remain heap size=%d\r\n", needed_memory_size, xPortGetFreeHeapSize());
                //vDumpHeapStatus();  // this can dump heap status. sometime alougth remain heap size is large than needed momory size, but it has no continue heap, it will also false. this feature need MTK_SUPPORT_HEAP_DEBUG = y
                return ATTTEST_ERROR;
            }
        }

        if (hal_audio_set_memory(audio_internal_ring_buffer) != HAL_AUDIO_STATUS_OK) {
            return ATTTEST_ERROR;
        }

        atttest_fill_stream_out_buffer((uint32_t *)&wanted_samples , user_buffer, USER_BUFFER_SIZE, atttest_audio_struct.tone_hz, defined_sampling_rate_enum_to_defined_sampling_rate_value(atttest_audio_struct.sampling_rate));
        hal_audio_write_stream_out(user_buffer, wanted_samples);
        hal_audio_start_stream_out(HAL_AUDIO_PLAYBACK_MUSIC);


        atttest_audio_test_running_state = AUDIO_TEST_IS_RUNNING;
    }
    return ATTTEST_NO_ERROR;
}



audio_nvdm_callback_id nvdm_callback_id;

void call_this_funtion_if_eaps_data_is_changed(void *data)
{
    atttest_nvdm_callback_useful_info_t *atttest_nvdm_callback_useful_info_ptr = (atttest_nvdm_callback_useful_info_t *)data;

    audio_eaps_t *eaps;
    uint32_t chose_stream_out_level = 0;
    uint32_t chose_stream_out_level_in_db = 0;
    uint32_t chose_side_tone_db = 0;
    uint32_t chose_stream_in_level = 0;
    uint32_t chose_stream_in_level_in_db = 0;
    int32_t temp_stream_out_db = 0;
    int32_t temp_side_tone_db = 0;
    int32_t temp_stream_in_db = 0;
    int16_t temp_int16 = 0;
    int32_t temp_a_gain = 0;
    int32_t temp_d_gain = 0;
    uint32_t a_gain_in_hex = 0;
    uint32_t d_gain_in_hex = 0;

    eaps = (audio_eaps_t *)pvPortMalloc(sizeof(audio_eaps_t));

    audio_nvdm_get_eaps_data_by_memcpy(eaps);

    LOGI("nvdm changed, now device_mode=%x, stream_out_level=%d\n", atttest_nvdm_callback_useful_info_ptr->now_device_mode, atttest_nvdm_callback_useful_info_ptr->now_stream_out_level);

    switch (atttest_nvdm_callback_useful_info_ptr->now_device_mode) {
        case ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1_MT2533:
        case ATTTEST_DEVICE_MODE_VOICE_NB_MODE_2_MT2533:
        case ATTTEST_DEVICE_MODE_VOICE_NB_MODE_3_MT2533:
            chose_stream_out_level = atttest_nvdm_callback_useful_info_ptr->now_stream_out_level;
            chose_stream_out_level = (chose_stream_out_level > (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1) : chose_stream_out_level;
            temp_int16 = (int16_t)eaps->voice_parameter.voice_nb_band.voice_nb_parameter[atttest_nvdm_callback_useful_info_ptr->now_device_mode - ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_gain.stream_out_gain[chose_stream_out_level];
            chose_stream_out_level_in_db = (uint32_t)temp_int16;

            temp_int16 = (int16_t)eaps->voice_parameter.voice_nb_band.voice_nb_parameter[atttest_nvdm_callback_useful_info_ptr->now_device_mode - ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_gain.sidetone_gain;
            chose_side_tone_db = (uint32_t)temp_int16;

            chose_stream_in_level = atttest_nvdm_callback_useful_info_ptr->now_stream_in_level;
            chose_stream_in_level = (chose_stream_in_level > (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1) : chose_stream_in_level;            
            temp_int16 = (int16_t)eaps->voice_parameter.voice_nb_band.voice_nb_parameter[atttest_nvdm_callback_useful_info_ptr->now_device_mode - ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_gain.stream_in_gain[chose_stream_in_level];
            chose_stream_in_level_in_db = (uint32_t)temp_int16;

            break;
        case ATTTEST_DEVICE_MODE_VOICE_NB_MODE_4_MT2533:
        case ATTTEST_DEVICE_MODE_VOICE_NB_MODE_5_MT2533:
        case ATTTEST_DEVICE_MODE_VOICE_NB_MODE_6_MT2533:
            chose_stream_out_level = atttest_nvdm_callback_useful_info_ptr->now_stream_out_level;
            chose_stream_out_level = (chose_stream_out_level > (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1) : chose_stream_out_level;
            temp_int16 = (int16_t)eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[atttest_nvdm_callback_useful_info_ptr->now_device_mode - ATTTEST_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_gain.stream_out_gain[chose_stream_out_level];
            chose_stream_out_level_in_db = (uint32_t)temp_int16;

            temp_int16 = (int16_t)eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[atttest_nvdm_callback_useful_info_ptr->now_device_mode - ATTTEST_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_gain.sidetone_gain;
            chose_side_tone_db = (uint32_t)temp_int16;

            chose_stream_in_level = atttest_nvdm_callback_useful_info_ptr->now_stream_in_level;
            chose_stream_in_level = (chose_stream_in_level > (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1) : chose_stream_in_level;            
            temp_int16 = (int16_t)eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[atttest_nvdm_callback_useful_info_ptr->now_device_mode - ATTTEST_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_gain.stream_in_gain[chose_stream_in_level];
            chose_stream_in_level_in_db = (uint32_t)temp_int16;

            break;

        case ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1_MT2533:
        case ATTTEST_DEVICE_MODE_VOICE_WB_MODE_2_MT2533:
        case ATTTEST_DEVICE_MODE_VOICE_WB_MODE_3_MT2533:            
            chose_stream_out_level = atttest_nvdm_callback_useful_info_ptr->now_stream_out_level;
            chose_stream_out_level = (chose_stream_out_level > (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1) : chose_stream_out_level;
            temp_int16 = (int16_t)eaps->voice_parameter.voice_wb_band.voice_wb_parameter[atttest_nvdm_callback_useful_info_ptr->now_device_mode - ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_gain.stream_out_gain[chose_stream_out_level];
            chose_stream_out_level_in_db = (uint32_t)temp_int16;

            temp_int16 = (int16_t)eaps->voice_parameter.voice_wb_band.voice_wb_parameter[atttest_nvdm_callback_useful_info_ptr->now_device_mode - ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_gain.sidetone_gain;
            chose_side_tone_db = (uint32_t)temp_int16;

            chose_stream_in_level = atttest_nvdm_callback_useful_info_ptr->now_stream_in_level;
            chose_stream_in_level = (chose_stream_in_level > (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1) : chose_stream_in_level;
            temp_int16 = (int16_t)eaps->voice_parameter.voice_wb_band.voice_wb_parameter[atttest_nvdm_callback_useful_info_ptr->now_device_mode - ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_gain.stream_in_gain[chose_stream_in_level];
            chose_stream_in_level_in_db = (uint32_t)temp_int16;

            break;

        case ATTTEST_DEVICE_MODE_VOICE_WB_MODE_4_MT2533:
        case ATTTEST_DEVICE_MODE_VOICE_WB_MODE_5_MT2533:
        case ATTTEST_DEVICE_MODE_VOICE_WB_MODE_6_MT2533:            
            chose_stream_out_level = atttest_nvdm_callback_useful_info_ptr->now_stream_out_level;
            chose_stream_out_level = (chose_stream_out_level > (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1) : chose_stream_out_level;
            temp_int16 = (int16_t)eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[atttest_nvdm_callback_useful_info_ptr->now_device_mode - ATTTEST_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_gain.stream_out_gain[chose_stream_out_level];
            chose_stream_out_level_in_db = (uint32_t)temp_int16;

            temp_int16 = (int16_t)eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[atttest_nvdm_callback_useful_info_ptr->now_device_mode - ATTTEST_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_gain.sidetone_gain;
            chose_side_tone_db = (uint32_t)temp_int16;

            chose_stream_in_level = atttest_nvdm_callback_useful_info_ptr->now_stream_in_level;
            chose_stream_in_level = (chose_stream_in_level > (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1) : chose_stream_in_level;
            temp_int16 = (int16_t)eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[atttest_nvdm_callback_useful_info_ptr->now_device_mode - ATTTEST_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_gain.stream_in_gain[chose_stream_in_level];
            chose_stream_in_level_in_db = (uint32_t)temp_int16;

            break;

        default:
            vPortFree(eaps);
            return;
    }

    temp_stream_out_db = (int32_t)chose_stream_out_level_in_db;
    temp_side_tone_db = (int32_t)chose_side_tone_db;
    temp_stream_in_db = (int32_t)chose_stream_in_level_in_db;

    {   // update stream out volume

        if (atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1_MT2533 ||
            atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_NB_MODE_2_MT2533 ||
            atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_NB_MODE_3_MT2533 ||
            atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 ||
            atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_NB_MODE_6_MT2533 ||
            atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1_MT2533 ||
            atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_WB_MODE_2_MT2533 ||
            atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_WB_MODE_3_MT2533 ||
            atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 ||
            atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_WB_MODE_6_MT2533) {
            audio_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(temp_stream_out_db, &temp_a_gain, &temp_d_gain);
        } else {    // ATTTEST_DEVICE_MODE_VOICE_NB_MODE_5_MT2533 or ATTTEST_DEVICE_MODE_VOICE_WB_MODE_5_MT2533
            voice_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(temp_stream_out_db, &temp_a_gain, &temp_d_gain);
        }


        a_gain_in_hex = (uint32_t)(temp_a_gain * 100);  // since hal_audio_set_stream_out_volume's unit is 0.01db
        d_gain_in_hex = (uint32_t)(temp_d_gain * 100);

        LOGI("temp_stream_out_db=%d, temp_a_gain=%d, temp_d_gain=%d, a_gain_in_hex=%x, d_gain_in_hex=%x\n", temp_stream_out_db, temp_a_gain, temp_d_gain, a_gain_in_hex, d_gain_in_hex);

        hal_audio_set_stream_out_volume(d_gain_in_hex, a_gain_in_hex);
    }

    {   // update stream in volume

        if (atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1_MT2533 ||
            atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_NB_MODE_2_MT2533 ||
            atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_NB_MODE_3_MT2533 ||
            atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1_MT2533 ||
            atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_WB_MODE_2_MT2533 ||
            atttest_nvdm_callback_useful_info_ptr->now_device_mode == ATTTEST_DEVICE_MODE_VOICE_WB_MODE_3_MT2533) {
            voice_uplink_amp_db_transfer_to_analog_gain_and_digital_gain(temp_stream_in_db, &temp_a_gain, &temp_d_gain);
        } else {    // d-mic cases
            voice_uplink_dmic_db_transfer_to_analog_gain_and_digital_gain(temp_stream_in_db, &temp_a_gain, &temp_d_gain);
        }



        a_gain_in_hex = (uint32_t)(temp_a_gain * 100);  // since hal_audio_set_stream_out_volume's unit is 0.01db
        d_gain_in_hex = (uint32_t)(temp_d_gain * 100);

        LOGI("temp_stream_in_db=%d, temp_a_gain=%d, temp_d_gain=%d, a_gain_in_hex=%x, d_gain_in_hex=%x\n", temp_stream_in_db, temp_a_gain, temp_d_gain, a_gain_in_hex, d_gain_in_hex);

        hal_audio_set_stream_in_volume(d_gain_in_hex, a_gain_in_hex);
    }


    // set side tone gain. // no side tone in mt2533
    voice_sidetone_db_make_sure_in_range(&temp_side_tone_db);
    //speech_set_sidetone_volume((uint32_t)temp_side_tone_db);


    vPortFree(eaps);
}


void voice_p2w_stream_out_callback(hal_audio_event_t event, void *user_data)
{
    nb_or_wb_t *nb_or_wb = (nb_or_wb_t *)user_data;

    switch (event) {
        case HAL_AUDIO_EVENT_UNDERFLOW:
        case HAL_AUDIO_EVENT_DATA_REQUEST:
            LOGI("voice p2w stream out callback\n");
            if (*nb_or_wb == NB) {
                hal_audio_write_stream_out(stream_in_stream_out_buffer_ptr, 160);
            } else {
                hal_audio_write_stream_out(stream_in_stream_out_buffer_ptr, 320);
            }

            break;
        default:
            break;
    }
}

void voice_p2w_stream_in_callback(hal_audio_event_t event, void *user_data)
{
    nb_or_wb_t *nb_or_wb = (nb_or_wb_t *)user_data;

    switch (event) {
        case HAL_AUDIO_EVENT_DATA_NOTIFICATION:
            LOGI("voice p2w stream in callback\n");
            if (*nb_or_wb == NB) {
                hal_audio_read_stream_in(stream_in_stream_out_buffer_ptr, 160);
            } else {
                hal_audio_read_stream_in(stream_in_stream_out_buffer_ptr, 320);
            }

            break;
        default:
            break;
    }

}


static atttest_error_check_t voice_play(hal_audio_device_t stream_out_device, hal_audio_device_t stream_in_device, nb_or_wb_t nb_or_wb, uint32_t stream_out_volume_db, uint32_t side_tone_volume_db, uint32_t stream_in_volume_db)
{
    int32_t temp_stream_out_db = (int32_t)stream_out_volume_db;
    int32_t temp_stream_in_db = (int32_t)stream_in_volume_db;
    int32_t temp_side_tone_in_db = (int32_t)side_tone_volume_db;
    int32_t temp_a_gain = 0;
    int32_t temp_d_gain = 0;
    uint32_t a_gain_in_hex = 0;
    uint32_t d_gain_in_hex = 0;

    uint16_t i = 0;


    {
        // stream out volume setting
        if (stream_out_device == HAL_AUDIO_DEVICE_HEADSET_MONO) {
            audio_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(temp_stream_out_db, &temp_a_gain, &temp_d_gain);
        } else if (stream_out_device == HAL_AUDIO_DEVICE_HANDS_FREE_MONO) {
            voice_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(temp_stream_out_db, &temp_a_gain, &temp_d_gain);
        } else {
            // audio play never here
            return ATTTEST_NO_ERROR;
        }


        a_gain_in_hex = (uint32_t)(temp_a_gain * 100);  // since hal_audio_set_stream_out_volume's unit is 0.01db
        d_gain_in_hex = (uint32_t)(temp_d_gain * 100);

        LOGI("temp_stream_out_db=%d, temp_a_gain=%d, temp_d_gain=%d, a_gain_in_hex=%x, d_gain_in_hex=%x\n", temp_stream_out_db, temp_a_gain, temp_d_gain, a_gain_in_hex, d_gain_in_hex);

        hal_audio_set_stream_out_volume(d_gain_in_hex, a_gain_in_hex);
    }

    {
        // stream in volume setting
        if (stream_in_device == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) {
            voice_uplink_dmic_db_transfer_to_analog_gain_and_digital_gain(temp_stream_in_db, &temp_a_gain, &temp_d_gain);
        } else {
            voice_uplink_amp_db_transfer_to_analog_gain_and_digital_gain(temp_stream_in_db, &temp_a_gain, &temp_d_gain);
        }

        a_gain_in_hex = (uint32_t)(temp_a_gain * 100);  // since hal_audio_set_stream_out_volume's unit is 0.01db
        d_gain_in_hex = (uint32_t)(temp_d_gain * 100);

        LOGI("temp_stream_in_db=%d, temp_a_gain=%d, temp_d_gain=%d, a_gain_in_hex=%x, d_gain_in_hex=%x\n", temp_stream_in_db, temp_a_gain, temp_d_gain, a_gain_in_hex, d_gain_in_hex);

        hal_audio_set_stream_in_volume(d_gain_in_hex, a_gain_in_hex);
    }


    {
        // set side tone gain
        voice_sidetone_db_make_sure_in_range(&temp_side_tone_in_db);
        speech_set_sidetone_volume((uint32_t)temp_side_tone_in_db);
    }


    if (voice_test_start == 0) {
        // setting voice test.

        // allocate stream_in_stream_out_buffer_ptr
        if (stream_in_stream_out_buffer_ptr == NULL) {
            if (nb_or_wb == NB) {
                stream_in_stream_out_buffer_ptr = pvPortMalloc(160 * sizeof(uint16_t));

                for (i = 0; i < 160; i++) {
                    *(stream_in_stream_out_buffer_ptr + i) = 0;
                }
            } else {
                stream_in_stream_out_buffer_ptr = pvPortMalloc(320 * sizeof(uint16_t));
                for (i = 0; i < 320; i++) {
                    *(stream_in_stream_out_buffer_ptr + i) = 0;
                }
            }
            if (stream_in_stream_out_buffer_ptr == NULL) {
                return ATTTEST_ERROR;
            }
        }



        // stream out setting
        if (nb_or_wb == NB) {
            hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
        } else {
            hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);
        }


        hal_audio_set_stream_out_channel_number(HAL_AUDIO_MONO);
        hal_audio_set_stream_out_device(stream_out_device);
        hal_audio_register_stream_out_callback(voice_p2w_stream_out_callback, &nb_or_wb);
        hal_audio_write_stream_out(stream_in_stream_out_buffer_ptr, 160);



        // stream in setting
        if (nb_or_wb == NB) {
            hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
        } else {
            hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);
        }

        if (stream_in_device == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) {
            hal_audio_set_stream_in_channel_number(HAL_AUDIO_STEREO);
        } else {
            hal_audio_set_stream_in_channel_number(HAL_AUDIO_MONO);
        }

        hal_audio_set_stream_in_device(stream_in_device);
        hal_audio_register_stream_in_callback(voice_p2w_stream_in_callback, NULL);


        if (stream_out_device == HAL_AUDIO_DEVICE_HEADSET || stream_out_device == HAL_AUDIO_DEVICE_HEADSET_MONO) {
            speech_set_mode(SPH_MODE_BT_EARPHONE);
        } else if (stream_out_device == HAL_AUDIO_DEVICE_HANDS_FREE_MONO || stream_out_device == HAL_AUDIO_DEVICE_HANDS_FREE_STEREO) {
            speech_set_mode(SPH_MODE_BT_LOUDSPEAKER);  // in handsfree mode default will turn off side tone
        } else {
            // never here
        }


        hal_audio_start_stream_out(HAL_AUDIO_TWO_WAY_VOICE);
        speech_set_enhancement(true);   // set enhancement to enable side tone. this function must later then hal_audio_start_stream_out(HAL_AUDIO_TWO_WAY_VOICE);

        voice_test_start = 1;
        return ATTTEST_NO_ERROR;
    }


    voice_test_start = 1;

    return ATTTEST_NO_ERROR;
}

#if defined(RECORD_SDFATFS_ENABLE)
static uint32_t get_current_time_in_ms(void)
{
    uint32_t count = 0;
    uint64_t count64 = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count);
    count64 = ((uint64_t)count) * 1000 / 32768;
    return (uint32_t)count64;
}
#endif

void record_stream_in_callback(hal_audio_event_t event, void *user_data)
{
#if defined(RECORD_SDFATFS_ENABLE)
    uint32_t stream_in_data_length;
    UINT length_written = 0;
    FRESULT fatfs_res;
    uint8_t *tempPtr;
    tempPtr = (uint8_t *)(user_buffer);
    switch (event) {
        case HAL_AUDIO_EVENT_DATA_NOTIFICATION:
            LOGI("record_stream_in_callback\n");
            hal_audio_get_stream_in_sample_count(&stream_in_data_length);
            hal_audio_read_stream_in(user_buffer, stream_in_data_length);

            while (stream_in_data_length > 0) {
                fatfs_res = f_write(&fdst, tempPtr, stream_in_data_length, &length_written);
                if (fatfs_res != FR_OK) {
                    return false;
                }

                tempPtr += length_written;
                stream_in_data_length = stream_in_data_length - length_written;
            }

            if ((get_current_time_in_ms() - atttest_record_struct.start_record_time_in_ms) > atttest_record_struct.wanted_record_time_in_ms) {
                LOGP("record time up, stop record\n");

                fatfs_res = f_close(&fdst);
                atttest_audio_stop_stream_in();

#if 0   // we can't free memory in isr, may think another method, like create a task
                //Free the internal memory allocation
                if (record_internal_ring_buffer != NULL) {
                    vPortFree(record_internal_ring_buffer);
                    record_internal_ring_buffer = NULL;
                }
#endif
                atttest_record_struct.record_test_is_running = RECORD_TEST_IS_NOT_RUNNING;
                atttest_record_struct.wanted_record_time_in_ms = 0;
                atttest_record_struct.start_record_time_in_ms = 0;

#if 0   // we can't print message by atci, may think another method, like create a task
                strcpy((char *)output.response_buf, "+ATTTEST:record time up\r\nOK\r\n");
                output.response_len = strlen((char *)output.response_buf);
                atci_send_response(&output);
#endif
            }
            break;

        default:
            break;
    }
#endif

}


static atttest_error_check_t record_play(hal_audio_device_t stream_in_device, uint32_t stream_in_volume_db, uint32_t wanted_record_time_in_ms)
{
#if defined(RECORD_SDFATFS_ENABLE)
    FRESULT fatfs_res;
    char record_folder_path[15] = "SD:/temp/";
    char record_path[18];
    char temp_string[5];
    static int8_t record_file_index = 0;
    int32_t temp_stream_in_db = (int32_t)stream_in_volume_db;
    int32_t temp_a_gain = 0;
    int32_t temp_d_gain = 0;
    uint32_t a_gain_in_hex = 0;
    uint32_t d_gain_in_hex = 0;

    if (atttest_record_struct.record_test_is_running == RECORD_TEST_IS_RUNNING) {
        LOGI("record playing, please stop first");
        return ATTTEST_NO_ERROR;
    }


    LOGI("record play \n");

    wanted_record_time_in_ms = (wanted_record_time_in_ms > 60000) ? 60000 : wanted_record_time_in_ms;   // maximum 60s
    atttest_record_struct.wanted_record_time_in_ms = wanted_record_time_in_ms;

    uint32_t needed_memory_size = 0;


    fatfs_res = f_mount(&fatfs, "0:", 1);
    if (!fatfs_res) {
        LOGI("fmount ok \n");

        do {
            sprintf((char *)temp_string, "%d", record_file_index);
            strcpy((char *)record_path, (char *)record_folder_path);
            strcat((char *)record_path, (char *)temp_string);
            strcat((char *)record_path, ".pcm");

            fatfs_res = f_open(&fdst, record_path, FA_CREATE_NEW | FA_WRITE | FA_READ);
            if (!fatfs_res) {
                LOGI("fopen ok \n");
            } else {
                LOGI("fopen error,fatfs_res = %d, record_file_index=%d\n", fatfs_res, record_file_index);
                record_file_index++;
            }
        } while (fatfs_res && record_file_index <= 100);

        if (fatfs_res) {
            LOGI("file index exceed 100\n");
        }
    } else {
        log_hal_info("fmount error \n");
        return ATTTEST_NO_ERROR;
    }


    hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);   // since there only 16kHz in record in audio tuning tool

    if (stream_in_device == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) {
        hal_audio_set_stream_in_channel_number(HAL_AUDIO_STEREO);
    } else {
        hal_audio_set_stream_in_channel_number(HAL_AUDIO_MONO);
    }
    hal_audio_set_stream_in_device(stream_in_device);

    // Get the amount of internal memory usage.
    if (record_internal_ring_buffer == NULL) {
        hal_audio_get_memory_size(&needed_memory_size);
        record_internal_ring_buffer = pvPortMalloc(needed_memory_size * sizeof(uint8_t));

        if (record_internal_ring_buffer == NULL) {
            return ATTTEST_ERROR;
        }
    }

    if (hal_audio_set_memory(record_internal_ring_buffer) != HAL_AUDIO_STATUS_OK) {
        return ATTTEST_ERROR;
    }


    hal_audio_register_stream_in_callback(record_stream_in_callback, NULL);
    *(volatile uint16_t *)(0x82255d5e) = 0x0000; //dsp clean data   // temp solution

    if (stream_in_device == HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC) {
        voice_uplink_dmic_db_transfer_to_analog_gain_and_digital_gain(temp_stream_in_db, &temp_a_gain, &temp_d_gain);
    } else {
        voice_uplink_amp_db_transfer_to_analog_gain_and_digital_gain(temp_stream_in_db, &temp_a_gain, &temp_d_gain);
    }

    a_gain_in_hex = (uint32_t)(temp_a_gain * 100);  // since hal_audio_set_stream_out_volume's unit is 0.01db
    d_gain_in_hex = (uint32_t)(temp_d_gain * 100);
    LOGI("temp_stream_in_db=%d, temp_a_gain=%d, temp_d_gain=%d, a_gain_in_hex=%x, d_gain_in_hex=%x\n", temp_stream_in_db, temp_a_gain, temp_d_gain, a_gain_in_hex, d_gain_in_hex);
    hal_audio_set_stream_in_volume(d_gain_in_hex, a_gain_in_hex);


    atttest_record_struct.start_record_time_in_ms = get_current_time_in_ms();

    speech_set_mode(SPH_MODE_RECORD);

    hal_audio_start_stream_in(HAL_AUDIO_RECORD_VOICE);

    speech_set_enhancement(true);

    atttest_record_struct.record_test_is_running = RECORD_TEST_IS_RUNNING;

#endif
    return ATTTEST_NO_ERROR;


}

atttest_structure_check_t atttest_tuning_outcome_test(uint32_t *out_data_buffer, int16_t *out_data_buffer_size, uint32_t AT_command_array[], int AT_command_array_size)
{
    atttest_structure_check_t atttest_structure_check =  ATTTEST_STRUCUTRE_CHECK_NO_ERROR;
    audio_eaps_t EAPS_structure = audio_nvdm_get_eaps_data();
    uint32_t chose_stream_out_level = 0;
    uint32_t chose_stream_out_level_in_db = 0;
    uint32_t chose_side_tone_db = 0;
    uint32_t chose_stream_in_level = 0;
    uint32_t chose_stream_in_level_in_db = 0;
    hal_audio_sampling_rate_t chose_audio_sampling_rate_enum = HAL_AUDIO_SAMPLING_RATE_8KHZ;
    uint32_t chose_audio_sampling_rate_value = 8000;
    uint32_t chose_tone_hz = 0;
    int16_t temp_int16 = 0;
    int16_t audio_iir_filter_index = 0;
    int16_t audio_iir_filter_element_index = 0;
    uint32_t audio_iir_filter_array[AMOUNT_OF_AUDIO_FILTERS * EAPS_AUDIO_IIR_FILTER_PARAMETER_SIZE];
    int16_t audio_iir_filter_array_index = 0;


    switch (AT_command_array[0]) {
        case ATTTEST_SCENARIO_AUDIO:
            switch (AT_command_array[1]) {
                case ATTTEST_DEVICE_MODE_AUDIO_ACF_MODE_1:
                case ATTTEST_DEVICE_MODE_AUDIO_ACF_MODE_2:
                case ATTTEST_DEVICE_MODE_AUDIO_ACF_MODE_3:
                    // AT_command_array[2]:device(device:0 ->headset , 1->handsfree), AT_command_array[3]:stream out level(0~6), AT_command_array[4]:sampling rate(8000, 11025, 16000, 22050, 24000, 32000, 44100, 48000), AT_command_array[5]:tone Hz(0~1/2* sampling rate)
                    LOGI("atttest AT command scenario:audio, device mode:%d, device:%d, stream out level:%d, sampling rate:%d, tone Hz:%d\n", AT_command_array[1], AT_command_array[2], AT_command_array[3], AT_command_array[4], AT_command_array[5]);


                    chose_audio_sampling_rate_enum = modify_user_sampling_rate_to_near_floor_value_of_defined_sampling_rate_enum(AT_command_array[4]);


                    chose_tone_hz = AT_command_array[5];
                    chose_audio_sampling_rate_value = defined_sampling_rate_enum_to_defined_sampling_rate_value(chose_audio_sampling_rate_enum);
                    truncate_out_of_range_value(&chose_tone_hz, 0, (chose_audio_sampling_rate_value / 2 - 1));  // -1: since Nyquist theorm said can't equal



                    if (atttest_audio_test_running_state == AUDIO_TEST_IS_NOT_RUNNING) {
                        for (audio_iir_filter_index = 0; audio_iir_filter_index < AMOUNT_OF_AUDIO_FILTERS; audio_iir_filter_index++) {
                            for (audio_iir_filter_element_index = 0; audio_iir_filter_element_index < EAPS_AUDIO_IIR_FILTER_PARAMETER_SIZE; audio_iir_filter_element_index++) {
                                audio_iir_filter_array[audio_iir_filter_array_index] = EAPS_structure.audio_parameter.audio_post_processing_parameter[AT_command_array[1] - ATTTEST_DEVICE_MODE_AUDIO_ACF_MODE_1].audio_post_processing_compensation_filter[audio_iir_filter_index].audio_iir_design_parameter[audio_iir_filter_element_index];
                                audio_iir_filter_array_index++;
                            }
                        }

                        audio_update_iir_design((const uint32_t *)audio_iir_filter_array);
                    }


                    if (AT_command_array[2] == 0) { // device chooses headset
                        chose_stream_out_level = AT_command_array[3];
                        chose_stream_out_level = (chose_stream_out_level > (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1) : chose_stream_out_level;
                        temp_int16 = (int16_t)EAPS_structure.audio_parameter.handsfree_headset_stream_out_gain.headset_gain[chose_stream_out_level];
                        chose_stream_out_level_in_db = (uint32_t)temp_int16;

                        LOGP("atttest: device=headset chose_stream_out_level=%d, chose_stream_out_level_in_db=%x, chose_audio_sampling_rate_enum=%d, chose_audio_sampling_rate_value=%d, chose_tone_hz=%d\n", chose_stream_out_level, chose_stream_out_level_in_db, chose_audio_sampling_rate_enum, chose_audio_sampling_rate_value, chose_tone_hz);
                        audio_play(HAL_AUDIO_DEVICE_HEADSET, chose_stream_out_level_in_db, chose_audio_sampling_rate_enum, chose_tone_hz);

                    } else if (AT_command_array[2] == 1) {   // device choose handsfree
                        chose_stream_out_level = AT_command_array[3];
                        chose_stream_out_level = (chose_stream_out_level > (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1) : chose_stream_out_level;
                        temp_int16 = (int16_t)EAPS_structure.audio_parameter.handsfree_headset_stream_out_gain.handsfree_gain[chose_stream_out_level];
                        chose_stream_out_level_in_db = (uint32_t)temp_int16;

                        LOGP("atttest: device=handsfree chose_stream_out_level=%d, chose_stream_out_level_in_db=%x, chose_audio_sampling_rate_enum=%d, chose_audio_sampling_rate_value=%d, chose_tone_hz=%d\n", chose_stream_out_level, chose_stream_out_level_in_db, chose_audio_sampling_rate_enum, chose_audio_sampling_rate_value, chose_tone_hz);
                        audio_play(HAL_AUDIO_DEVICE_HANDS_FREE_STEREO, chose_stream_out_level_in_db, chose_audio_sampling_rate_enum, chose_tone_hz);
                    } else {    // no this device
                        atttest_structure_check = ATTTEST_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                    }

                    break;

                default:
                    atttest_structure_check = ATTTEST_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                    break;
            }

            break;

        case ATTTEST_SCENARIO_VOICE:
            switch (AT_command_array[1]) {
                case ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1_MT2533:
                case ATTTEST_DEVICE_MODE_VOICE_NB_MODE_2_MT2533:
                case ATTTEST_DEVICE_MODE_VOICE_NB_MODE_3_MT2533:
                    LOGI("atttest scenario:voice, device mode:%d, stream out level:%d, stream in level:%d\n", (AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1_MT2533 + 1), AT_command_array[2], AT_command_array[3]);

                    chose_stream_out_level = AT_command_array[2];
                    chose_stream_out_level = (chose_stream_out_level > (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1) : chose_stream_out_level;
                    temp_int16 = (int16_t)EAPS_structure.voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_gain.stream_out_gain[chose_stream_out_level];
                    chose_stream_out_level_in_db = (uint32_t)temp_int16;

                    temp_int16 = (int16_t)EAPS_structure.voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_gain.sidetone_gain;
                    chose_side_tone_db = (uint32_t)temp_int16;

                    chose_stream_in_level = AT_command_array[3];
                    chose_stream_in_level = (chose_stream_in_level > (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1) : chose_stream_in_level;
                    temp_int16 = (int16_t)EAPS_structure.voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_gain.stream_in_gain[chose_stream_in_level];
                    chose_stream_in_level_in_db = (uint32_t)temp_int16;


                    atttest_nvdm_callback_useful_info.now_device_mode = (atttest_device_mode_t)AT_command_array[1];
                    atttest_nvdm_callback_useful_info.now_stream_out_level = AT_command_array[2];
                    atttest_nvdm_callback_useful_info.now_stream_in_level = AT_command_array[3];
                    audio_nvdm_register_eaps_is_changed_callback(&nvdm_callback_id, call_this_funtion_if_eaps_data_is_changed, &atttest_nvdm_callback_useful_info);

                    if (AT_command_array[1] == ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1) {
                        LOGI("atttest: stream_out:HAL_AUDIO_DEVICE_HEADSET_MONO, stream_in:HAL_AUDIO_DEVICE_HEADSET_MIC, NB, chose_stream_out_level_in_db=%x, chose_side_tone_db=%x, chose_stream_in_level_in_db=%x\n", chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                        voice_play(HAL_AUDIO_DEVICE_HEADSET_MONO, HAL_AUDIO_DEVICE_HEADSET_MIC, NB, chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                    } else if (AT_command_array[1] == ATTTEST_DEVICE_MODE_VOICE_NB_MODE_2) {
                        LOGI("atttest: stream_out:HAL_AUDIO_DEVICE_HANDS_FREE_MONO, stream_in:HAL_AUDIO_DEVICE_MAIN_MIC, NB, chose_stream_out_level_in_db=%x, chose_side_tone_db=%x, chose_stream_in_level_in_db=%x\n", chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                        voice_play(HAL_AUDIO_DEVICE_HEADSET_MONO, HAL_AUDIO_DEVICE_HEADSET_MIC, NB, chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                    } else {    // ATTTEST_DEVICE_MODE_VOICE_NB_MODE_3
                        LOGI("atttest: stream_out:HAL_AUDIO_DEVICE_HANDS_FREE_MONO, stream_in:HAL_AUDIO_DEVICE_MAIN_MIC, NB, chose_stream_out_level_in_db=%x, chose_side_tone_db=%x, chose_stream_in_level_in_db=%x\n", chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                        voice_play(HAL_AUDIO_DEVICE_HEADSET_MONO, HAL_AUDIO_DEVICE_HEADSET_MIC, NB, chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                    }

                    break;

                case ATTTEST_DEVICE_MODE_VOICE_NB_MODE_4_MT2533:
                case ATTTEST_DEVICE_MODE_VOICE_NB_MODE_5_MT2533:
                case ATTTEST_DEVICE_MODE_VOICE_NB_MODE_6_MT2533:
                    LOGI("atttest scenario:voice, device mode:%d, stream out level:%d, stream in level:%d\n", (AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_NB_MODE_1_MT2533 + 1), AT_command_array[2], AT_command_array[3]);

                    chose_stream_out_level = AT_command_array[2];
                    chose_stream_out_level = (chose_stream_out_level > (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1) : chose_stream_out_level;
                    temp_int16 = (int16_t)EAPS_structure.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_gain.stream_out_gain[chose_stream_out_level];
                    chose_stream_out_level_in_db = (uint32_t)temp_int16;

                    temp_int16 = (int16_t)EAPS_structure.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_gain.sidetone_gain;
                    chose_side_tone_db = (uint32_t)temp_int16;

                    chose_stream_in_level = AT_command_array[3];
                    chose_stream_in_level = (chose_stream_in_level > (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1) : chose_stream_in_level;
                    temp_int16 = (int16_t)EAPS_structure.voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_gain.stream_in_gain[chose_stream_in_level];
                    chose_stream_in_level_in_db = (uint32_t)temp_int16;


                    atttest_nvdm_callback_useful_info.now_device_mode = (atttest_device_mode_t)AT_command_array[1];
                    atttest_nvdm_callback_useful_info.now_stream_out_level = AT_command_array[2];
                    atttest_nvdm_callback_useful_info.now_stream_in_level = AT_command_array[3];
                    audio_nvdm_register_eaps_is_changed_callback(&nvdm_callback_id, call_this_funtion_if_eaps_data_is_changed, &atttest_nvdm_callback_useful_info);

                    if (AT_command_array[1] == ATTTEST_DEVICE_MODE_VOICE_NB_MODE_4_MT2533) {
                        LOGI("atttest: stream_out:HAL_AUDIO_DEVICE_HEADSET_MONO, stream_in:HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC, NB, chose_stream_out_level_in_db=%x, chose_side_tone_db=%x, chose_stream_in_level_in_db=%x\n", chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                        voice_play(HAL_AUDIO_DEVICE_HEADSET_MONO, HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC, NB, chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                    } else if (AT_command_array[1] == ATTTEST_DEVICE_MODE_VOICE_NB_MODE_5_MT2533) {
                        LOGI("atttest: stream_out:HAL_AUDIO_DEVICE_HANDS_FREE_MONO, stream_in:HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC, NB, chose_stream_out_level_in_db=%x, chose_side_tone_db=%x, chose_stream_in_level_in_db=%x\n", chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                        voice_play(HAL_AUDIO_DEVICE_HANDS_FREE_MONO, HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC, NB, chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                    } else {    // ATTTEST_DEVICE_MODE_VOICE_NB_MODE_6_MT2533
                        LOGI("atttest: stream_out:HAL_AUDIO_DEVICE_HEADSET_MONO, stream_in:HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC, NB, chose_stream_out_level_in_db=%x, chose_side_tone_db=%x, chose_stream_in_level_in_db=%x\n", chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                        voice_play(HAL_AUDIO_DEVICE_HEADSET_MONO, HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC, NB, chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                    }

                    break;


                case ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1_MT2533:
                case ATTTEST_DEVICE_MODE_VOICE_WB_MODE_2_MT2533:
                case ATTTEST_DEVICE_MODE_VOICE_WB_MODE_3_MT2533:
                    LOGI("atttest scenario:voice, device mode:%d, stream out level:%d, stream in level:%d\n", (AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1_MT2533), AT_command_array[2], AT_command_array[3]);

                    chose_stream_out_level = AT_command_array[2];
                    chose_stream_out_level = (chose_stream_out_level > (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1) : chose_stream_out_level;
                    temp_int16 = (int16_t)EAPS_structure.voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_gain.stream_out_gain[chose_stream_out_level];
                    chose_stream_out_level_in_db = (uint32_t)temp_int16;

                    temp_int16 = (int16_t)EAPS_structure.voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_gain.sidetone_gain;
                    chose_side_tone_db = (uint32_t)temp_int16;

                    chose_stream_in_level = AT_command_array[3];
                    chose_stream_in_level = (chose_stream_in_level > (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1) : chose_stream_in_level;
                    temp_int16 = (int16_t)EAPS_structure.voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_gain.stream_in_gain[chose_stream_in_level];
                    chose_stream_in_level_in_db = (uint32_t)temp_int16;


                    atttest_nvdm_callback_useful_info.now_device_mode = (atttest_device_mode_t)AT_command_array[1];
                    atttest_nvdm_callback_useful_info.now_stream_out_level = AT_command_array[2];
                    atttest_nvdm_callback_useful_info.now_stream_in_level = AT_command_array[3];
                    audio_nvdm_register_eaps_is_changed_callback(&nvdm_callback_id, call_this_funtion_if_eaps_data_is_changed, &atttest_nvdm_callback_useful_info);

                    if (AT_command_array[1] == ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1_MT2533) {
                        LOGI("atttest: stream_out:HAL_AUDIO_DEVICE_HEADSET_MONO, stream_in:HAL_AUDIO_DEVICE_HEADSET_MIC, WB, chose_stream_out_level_in_db=%x, chose_side_tone_db=%x, chose_stream_in_level_in_db=%x\n", chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                        voice_play(HAL_AUDIO_DEVICE_HEADSET_MONO, HAL_AUDIO_DEVICE_HEADSET_MIC, WB, chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                    } else if (ATTTEST_DEVICE_MODE_VOICE_WB_MODE_2_MT2533) {
                        LOGI("atttest: stream_out:HAL_AUDIO_DEVICE_HEADSET_MONO, stream_in:HAL_AUDIO_DEVICE_HEADSET_MIC, WB, chose_stream_out_level_in_db=%x, chose_side_tone_db=%x, chose_stream_in_level_in_db=%x\n", chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                        voice_play(HAL_AUDIO_DEVICE_HEADSET_MONO, HAL_AUDIO_DEVICE_HEADSET_MIC, WB, chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                    } else {    // ATTTEST_DEVICE_MODE_VOICE_WB_MODE_3_MT2533
                        LOGI("atttest: stream_out:HAL_AUDIO_DEVICE_HEADSET_MONO, stream_in:HAL_AUDIO_DEVICE_HEADSET_MIC, WB, chose_stream_out_level_in_db=%x, chose_side_tone_db=%x, chose_stream_in_level_in_db=%x\n", chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                        voice_play(HAL_AUDIO_DEVICE_HEADSET_MONO, HAL_AUDIO_DEVICE_HEADSET_MIC, WB, chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                    }

                    break;

                case ATTTEST_DEVICE_MODE_VOICE_WB_MODE_4_MT2533:
                case ATTTEST_DEVICE_MODE_VOICE_WB_MODE_5_MT2533:
                case ATTTEST_DEVICE_MODE_VOICE_WB_MODE_6_MT2533:
                    LOGI("atttest scenario:voice, device mode:%d, stream out level:%d, stream in level:%d\n", (AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_WB_MODE_1_MT2533), AT_command_array[2], AT_command_array[3]);

                    chose_stream_out_level = AT_command_array[2];
                    chose_stream_out_level = (chose_stream_out_level > (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE - 1) : chose_stream_out_level;
                    temp_int16 = (int16_t)EAPS_structure.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_gain.stream_out_gain[chose_stream_out_level];
                    chose_stream_out_level_in_db = (uint32_t)temp_int16;

                    temp_int16 = (int16_t)EAPS_structure.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_gain.sidetone_gain;
                    chose_side_tone_db = (uint32_t)temp_int16;

                    chose_stream_in_level = AT_command_array[3];
                    chose_stream_in_level = (chose_stream_in_level > (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1) : chose_stream_in_level;
                    temp_int16 = (int16_t)EAPS_structure.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[1] - ATTTEST_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_gain.stream_in_gain[chose_stream_in_level];
                    chose_stream_in_level_in_db = (uint32_t)temp_int16;


                    atttest_nvdm_callback_useful_info.now_device_mode = (atttest_device_mode_t)AT_command_array[1];
                    atttest_nvdm_callback_useful_info.now_stream_out_level = AT_command_array[2];
                    atttest_nvdm_callback_useful_info.now_stream_in_level = AT_command_array[3];
                    audio_nvdm_register_eaps_is_changed_callback(&nvdm_callback_id, call_this_funtion_if_eaps_data_is_changed, &atttest_nvdm_callback_useful_info);

                    if (AT_command_array[1] == ATTTEST_DEVICE_MODE_VOICE_WB_MODE_4_MT2533) {
                        LOGI("atttest: stream_out:HAL_AUDIO_DEVICE_HEADSET_MONO, stream_in:HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC, WB, chose_stream_out_level_in_db=%x, chose_side_tone_db=%x, chose_stream_in_level_in_db=%x\n", chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                        voice_play(HAL_AUDIO_DEVICE_HEADSET_MONO, HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC, WB, chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                    } else if (ATTTEST_DEVICE_MODE_VOICE_WB_MODE_5_MT2533) {
                        LOGI("atttest: stream_out:HAL_AUDIO_DEVICE_HANDS_FREE_MONO, stream_in:HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC, WB, chose_stream_out_level_in_db=%x, chose_side_tone_db=%x, chose_stream_in_level_in_db=%x\n", chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                        voice_play(HAL_AUDIO_DEVICE_HANDS_FREE_MONO, HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC, WB, chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                    } else {    // ATTTEST_DEVICE_MODE_VOICE_WB_MODE_6_MT2533
                        LOGI("atttest: stream_out:HAL_AUDIO_DEVICE_HEADSET_MONO, stream_in:HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC, WB, chose_stream_out_level_in_db=%x, chose_side_tone_db=%x, chose_stream_in_level_in_db=%x\n", chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                        voice_play(HAL_AUDIO_DEVICE_HEADSET_MONO, HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC, WB, chose_stream_out_level_in_db, chose_side_tone_db, chose_stream_in_level_in_db);
                    }

                    break;
                default:
                    atttest_structure_check = ATTTEST_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                    break;
            }

            break;

        case ATTTEST_SCENARIO_RECORD:
            switch (AT_command_array[1]) {
                case ATTTEST_DEVICE_MODE_RECORD_WB_MODE_1:
                    LOGI("atttest scenario:record, device mode:%d\n", AT_command_array[1]);

                    chose_stream_in_level = AT_command_array[3];
                    chose_stream_in_level = (chose_stream_in_level > (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1) : chose_stream_in_level;
                    temp_int16 = (int16_t)EAPS_structure.record_parameter.record_wb_parameter[AT_command_array[1] - ATTTEST_DEVICE_MODE_RECORD_WB_MODE_1].stream_in_gain[chose_stream_in_level];
                    chose_stream_in_level_in_db = (uint32_t)temp_int16;

                    record_play(HAL_AUDIO_DEVICE_HEADSET_MIC, chose_stream_in_level_in_db, AT_command_array[2]);

                    break;

                case ATTTEST_DEVICE_MODE_RECORD_WB_MODE_2:
                case ATTTEST_DEVICE_MODE_RECORD_WB_MODE_3:
                    LOGI("atttest scenario:record, device mode:%d\n", AT_command_array[1]);

                    chose_stream_in_level = AT_command_array[3];
                    chose_stream_in_level = (chose_stream_in_level > (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1)) ? (EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE - 1) : chose_stream_in_level;
                    temp_int16 = (int16_t)EAPS_structure.record_parameter.record_wb_parameter_with_external_dsp[AT_command_array[1] - ATTTEST_DEVICE_MODE_RECORD_WB_MODE_2].stream_in_gain[chose_stream_in_level];
                    chose_stream_in_level_in_db = (uint32_t)temp_int16;

                    record_play(HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC, chose_stream_in_level_in_db, AT_command_array[2]);
                    break;

                default:
                    atttest_structure_check = ATTTEST_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                    break;
            }

            break;

        case ATTTEST_SCENARIO_STOP:
            switch (AT_command_array[1]) {
                case ATTTEST_DEVICE_MODE_STOP_STREAM_OUT:
                    LOGI("atttest scenario:stop tream out\n");
                    atttest_audio_stop_stream_out();

                    break;

                case ATTTEST_DEVICE_MODE_STOP_STREAM_IN:
                    LOGI("atttest scenario:stop tream in\n");
                    atttest_audio_stop_stream_in();

                    break;

                case ATTTEST_DEVICE_MODE_STOP_STREAM_OUT_AND_STREAM_IN:
                    LOGI("atttest scenario:stop tream out and stream in\n");
                    atttest_audio_stop_stream_out();
                    atttest_audio_stop_stream_in();

                    break;

                case ATTTEST_DEVICE_MODE_STOP_AUDIO:
                    LOGI("atttest scenario:stop tream out and stream in\n");
                    atttest_stop_audio();

                    break;

                case ATTTEST_DEVICE_MODE_STOP_VOICE:
                    LOGI("atttest scenario:stop voice\n");
                    audio_nvdm_unregister_eaps_is_changed_callback(&nvdm_callback_id);
                    atttest_stop_voice();

                    break;

                case ATTTEST_DEVICE_MODE_STOP_RECORD:
                    LOGI("atttest scenario:stop record\n");
                    atttest_stop_record();

                    break;

                default:
                    atttest_structure_check = ATTTEST_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                    break;
            }

            break;

        case ATTTEST_SCENARIO_TEMP_DSP_GET_REGISTER_DATA:
            switch (AT_command_array[1]) {
                case ATTTEST_DEVICE_MODE_TEMP_DSP_SENDING_PATH_REGISTER:
                    LOGI("atttest scenario: get dsp sending path register\n");

                    uint16_t register_data = 0;

                    int16_t i = 0;
                    for (i = 0; i < 640; i++) {
                        external_dsp_read_parameter(i, &register_data);
                        printf("index=%3d, value=%x\r\n", i, register_data);
                    }

                    break;

                case ATTTEST_DEVICE_MODE_TEMP_DSP_FD216_REGISTER_VALUE:
                    {   // spe_dump_common_param
                        int16_t i;
                        volatile uint16_t *dsp_addptr = DSP_DM_ADDR(5, *DSP_SPH_EMP_ADDR);
                        for (i = 0; i < 12; i++) {
                            LOGP("common_para[%d]=%d\r\n",i,*dsp_addptr++);
                        }

                        LOGP("\r\n");
                    }


                    {   //void spe_dump_mode_param
                        int16_t i;
                        volatile uint16_t *dsp_addptr = DSP_DM_ADDR(SPH_EMP_PAGE, 0);
                        for (i = 0; i < 48; i++) {
                            LOGP("nb_mode_para[%d]=%d\r\n",i,*dsp_addptr++);
                        }

                        LOGP("\r\n");

                        dsp_addptr = DSP_DM_ADDR(SPH_EMP_PAGE, 0x30);
                        for (i = 0; i < 48; i++) {
                            LOGP("wb_mode_para[%d]=%d\r\n",i,*dsp_addptr++);
                        }
                    }

                    break;



                default:
                    atttest_structure_check = ATTTEST_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                    break;
            }

            break;
/*
        case ATTTEST_SCENARIO_TEMP_DSP_SET_REGISTER_DATA:
            switch (AT_command_array[1]) {
                case ATTTEST_DEVICE_MODE_TEMP_DSP_SENDING_PATH_REGISTER:
                    LOGI("atttest scenario: set dsp sending path register\n");

                    external_dsp_sending_path_register_value_t external_dsp_sending_path_register_value;

                    audio_nvdm_get_external_dsp_register_value_by_memcpy(DSP_REGISTER_PROFILE_TUNING, &external_dsp_sending_path_register_value);

                    uint32_t need_to_download = (uint32_t)AT_command_array[2];
                    int32_t register_index = (int32_t)AT_command_array[3];
                    uint16_t set_value = (uint16_t)AT_command_array[4];
                    if (register_index >= 0 && register_index <= (EXTERNAL_DSP_REGISTER_SENDING_PATH_INDEX_AMOUNT - 1)) {
                        printf("atttest_tuning_outcome_test: TEMP_DSP_SET_REGISTER_DATA:: register_index =%d, set_value=%x(hex)\r\n", (int)register_index, set_value);
                        external_dsp_sending_path_register_value.external_dsp_register_value[register_index] = set_value;
                        external_dsp_sending_path_register_value.need_to_download_index[register_index] = need_to_download ? 1 : 0;
                        audio_nvdm_set_external_dsp_register_value(external_dsp_sending_path_register_value);
                    } else {
                        atttest_structure_check = ATTTEST_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                        printf("atttest_tuning_outcome_test: TEMP_DSP_SET_REGISTER_DATA:: ATTTEST_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE:: register_index =%d, set_value=%x(hex)\r\n", (int)register_index, set_value);
                    }


                    break;

                default:
                    atttest_structure_check = ATTTEST_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                    break;
            }

            break;

        case ATTTEST_SCENARIO_TEMP_DSP_SAVE_REGISTER_DATA_TO_NVDM:
            switch (AT_command_array[1]) {
                case ATTTEST_DEVICE_MODE_TEMP_DSP_SAVE_REGISTER_DATA_TO_NVDM:
                    LOGI("atttest scenario: DSP_SAVE_REGISTER_DATA_TO_NVDM\n");

                    audio_nvdm_save_external_dsp_register_value_to_nvdm();

                    break;

                default:
                    atttest_structure_check = ATTTEST_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                    break;
            }

            break;
*/
		case ATTTEST_SCENARIO_TEMP_DSP_CONFIG_PATH:
            switch (AT_command_array[1]) {
                case ATTTEST_DEVICE_MODE_TEMP_DSP_CONFIG_PATH_SET:
                    LOGI("atttest scenario: DSP_SET_CONFIG_PATH\n");

                    external_dsp_set_output_source(AT_command_array[2]);
					external_dsp_configure_data_path(external_dsp_get_output_source());

                    break;
				case ATTTEST_DEVICE_MODE_TEMP_DSP_CONFIG_PATH_GET:
                    LOGI("atttest scenario: DSP_GET_CONFIG_PATH\n");

					*out_data_buffer =external_dsp_read_dsp_config_path();
					
					*out_data_buffer_size = 1;

                    break;

                default:
                    atttest_structure_check = ATTTEST_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                    break;
            }

            break;
		
        default:
            atttest_structure_check = ATTTEST_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
            break;
    }


    return atttest_structure_check;
}


static attdet_error_check_t query_useful_information_for_ui_tool(uint32_t *out_data_buffer, int16_t *out_data_buffer_size, uint32_t at_command_array[], int16_t at_command_array_size)
{
    attdet_error_check_t attdet_error_check = ATTDET_NO_ERROR;
    int32_t i = 0;
    audio_eaps_t eaps_structure;
    int32_t bits_per_byte = 8;

    // we omit to recognize at_command_array[1], at_command_array[2], since at_command_array[3] can cover every case now.

    switch (at_command_array[3]) {
        case ATTDET_PARAMETER_INDEX_SPEECH_COMMON_PARAMETER:
            // parameter array size, parameter value range(Amount of bits)
            LOGI("parameter index: speech common parameter\n");

            *out_data_buffer_size = 2;

            *out_data_buffer = EAPS_SPEECH_COMMON_PARAMETER_SIZE;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.speech_common_parameter.speech_common_parameter[0]) * bits_per_byte;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_AUDIO_GAIN_INFO_HEADSET:
            // stream out levels, stream out level min value, stream out level max value, stream out level step,
            // stream in levels, stream in level min value, stream in level max value, stream in level step,
            // sidetone levels,, sidetone level min value, sidetone level max value,, sidetone level step.
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_AUDIO_GAIN_INFO_HEADSET\n");

            *out_data_buffer_size = 12;

            *out_data_buffer = EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = (uint32_t)AUDIO_DOWNLINK_AMP_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = (uint32_t)AUDIO_DOWNLINK_AMP_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = ADUIO_DOWNLINK_AMP_DB_STEP;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_AUDIO_GAIN_INFO_HANDSFREE:
            // stream out levels, stream out level min value, stream out level max value, stream out level step,
            // stream in levels, stream in level min value, stream in level max value, stream in level step,
            // sidetone levels,, sidetone level min value, sidetone level max value,, sidetone level step.
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_AUDIO_GAIN_INFO_HANDSFREE\n");

            *out_data_buffer_size = 12;

            *out_data_buffer = EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = (uint32_t)VOICE_DOWNLINK_AMP_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_DOWNLINK_AMP_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_DOWNLINK_AMP_DB_STEP;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_HEADSET:
            // stream out levels, stream out level min value, stream out level max value, stream out level step,
            // stream in levels, stream in level min value, stream in level max value, stream in level step,
            // sidetone levels,, sidetone level min value, sidetone level max value,, sidetone level step.
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_HEADSET\n");

            *out_data_buffer_size = 12;

            *out_data_buffer = EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = (uint32_t)AUDIO_DOWNLINK_AMP_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = (uint32_t)AUDIO_DOWNLINK_AMP_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = ADUIO_DOWNLINK_AMP_DB_STEP;
            i++;
            *(out_data_buffer + i) = EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_STEP;
            i++;
            *(out_data_buffer + i) = EAPS_GAIN_PARAMETER_SIDETONE_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = VOICE_SIDETONE_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_SIDETONE_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_SIDETONE_DB_STEP;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_HANDSFREE:
            // stream out levels, stream out level min value, stream out level max value, stream out level step,
            // stream in levels, stream in level min value, stream in level max value, stream in level step,
            // sidetone levels,, sidetone level min value, sidetone level max value,, sidetone level step.
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_HANDSFREE\n");

            *out_data_buffer_size = 12;

            *out_data_buffer = EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = (uint32_t)VOICE_DOWNLINK_AMP_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_DOWNLINK_AMP_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_DOWNLINK_AMP_DB_STEP;
            i++;
            *(out_data_buffer + i) = EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_STEP;
            i++;
            *(out_data_buffer + i) = EAPS_GAIN_PARAMETER_SIDETONE_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = VOICE_SIDETONE_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_SIDETONE_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_SIDETONE_DB_STEP;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_RESERVED:    // stream_out: headset; stream_in: dual mic
            // stream out levels, stream out level min value, stream out level max value, stream out level step,
            // stream in levels, stream in level min value, stream in level max value, stream in level step,
            // sidetone levels,, sidetone level min value, sidetone level max value,, sidetone level step.
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_RESERVED\n");

            *out_data_buffer_size = 12;

            *out_data_buffer = EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = (uint32_t)AUDIO_DOWNLINK_AMP_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = (uint32_t)AUDIO_DOWNLINK_AMP_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = ADUIO_DOWNLINK_AMP_DB_STEP;
            i++;
            *(out_data_buffer + i) = EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_STEP;
            i++;
            *(out_data_buffer + i) = EAPS_GAIN_PARAMETER_SIDETONE_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = VOICE_SIDETONE_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_SIDETONE_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_SIDETONE_DB_STEP;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_NORMAL: // headset mic
            // stream out levels, stream out level min value, stream out level max value, stream out level step,
            // stream in levels, stream in level min value, stream in level max value, stream in level step,
            // sidetone levels,, sidetone level min value, sidetone level max value,, sidetone level step.
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_NORMAL\n");

            *out_data_buffer_size = 12;

            *out_data_buffer = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_STEP;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_ASR:    // A-mic
            // stream out levels, stream out level min value, stream out level max value, stream out level step,
            // stream in levels, stream in level min value, stream in level max value, stream in level step,
            // sidetone levels,, sidetone level min value, sidetone level max value,, sidetone level step.
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_ASR\n");

            *out_data_buffer_size = 12;

            *out_data_buffer = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_STEP;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_STEREO: // dual mic
            // stream out levels, stream out level min value, stream out level max value, stream out level step,
            // stream in levels, stream in level min value, stream in level max value, stream in level step,
            // sidetone levels,, sidetone level min value, sidetone level max value,, sidetone level step.

            LOGI("parameter index: ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_STEREO\n");

            *out_data_buffer_size = 12;

            *out_data_buffer = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_STEP;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_AUDIO_IIR_FILTER_INFO:
            // Amount of IIR filter, IIR filter array size, IIR parameter value range(Amount of bits)
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_AUDIO_IIR_FILTER_INFO\n");

            *out_data_buffer_size = 3;

            *out_data_buffer = AMOUNT_OF_AUDIO_FILTERS;
            i++;
            *(out_data_buffer + i) = EAPS_AUDIO_IIR_FILTER_PARAMETER_SIZE;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.audio_parameter.audio_post_processing_parameter[0].audio_post_processing_compensation_filter[0].audio_iir_design_parameter[0]) * bits_per_byte;;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_VOICE_NB_MODE_AND_FIR_PARAMETER_INFO:
            // mode parameter array size, mode parameter value range(Amount of bits),
            // input FIR coefficient array size, input FIR coefficient value range(Amount of bits), Amount of frquency, Amount of response,
            // output FIR coefficient array size, output FIR coefficient value range(Amount of bits), Amount of frquency, Amount of response,
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_VOICE_NB_MODE_AND_FIR_PARAMETER_INFO\n");

            *out_data_buffer_size = 10;

            *out_data_buffer = EAPS_VOICE_NB_MODE_PARAMETERS_PARAMETER_SIZE;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_mode_parameter[0]) * bits_per_byte;
            i++;

            *(out_data_buffer + i) = EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[0]) * bits_per_byte;
            i++;
            *(out_data_buffer + i) = EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;
            i++;
            *(out_data_buffer + i) = EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;
            i++;

            *(out_data_buffer + i) = EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient[0]) * bits_per_byte;
            i++;
            *(out_data_buffer + i) = EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;
            i++;
            *(out_data_buffer + i) = EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_VOICE_WB_MODE_AND_FIR_PARAMETER_INFO:
            // mode parameter array size, mode parameter value range(Amount of bits),
            // input FIR coefficient array size, input FIR coefficient value range(Amount of bits), Amount of frquency, Amount of response,
            // output FIR coefficient array size, output FIR coefficient value range(Amount of bits), Amount of frquency, Amount of response,"
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_VOICE_WB_MODE_AND_FIR_PARAMETER_INFO\n");

            *out_data_buffer_size = 10;

            *out_data_buffer = EAPS_VOICE_WB_MODE_PARAMETERS_PARAMETER_SIZE;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_mode_parameter[0]) * bits_per_byte;
            i++;

            *(out_data_buffer + i) = EAPS_VOICE_WB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient[0]) * bits_per_byte;
            i++;
            *(out_data_buffer + i) = EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;
            i++;
            *(out_data_buffer + i) = EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;
            i++;

            *(out_data_buffer + i) = EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient[0]) * bits_per_byte;
            i++;
            *(out_data_buffer + i) = EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;
            i++;
            *(out_data_buffer + i) = EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_RECORD_WB_MODE_AND_FIR_PARAMETERS_INFO:
            // mode parameter array size, mode parameter value range(Amount of bits),
            // input channel 1 FIR coefficient array size, input channel 1 coefficient value range(Amount of bits), Amount of frquency, Amount of response,
            // input channel 2  FIR coefficient array size, input channel 2 FIR coefficient value range(Amount of bits), Amount of frquency, Amount of response,
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_RECORD_WB_MODE_AND_FIR_PARAMETERS_INFO\n");

            *out_data_buffer_size = 10;

            *out_data_buffer = EAPS_RECORD_WB_MODE_PARAMETERS_PARAMETER_SIZE;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.record_parameter.record_wb_parameter[0].record_wb_enhancement_parameter.record_wb_mode_parameter[0]) * bits_per_byte;
            i++;

            *(out_data_buffer + i) = EAPS_RECORD_WB_CHANNEL_1_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.record_parameter.record_wb_parameter[0].record_wb_enhancement_parameter.record_wb_channel_1_fir_coefficient[0]) * bits_per_byte;
            i++;
            *(out_data_buffer + i) = EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;
            i++;
            *(out_data_buffer + i) = EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;
            i++;

            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            break;


        case ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_HEADSET_A_MIC_MT2533:  // (for voice NB/WB mode1~3)
            // stream out levels, stream out level min value, stream out level max value, stream out level step,
            // stream in levels, stream in level min value, stream in level max value, stream in level step,
            // sidetone levels,, sidetone level min value, sidetone level max value,, sidetone level step.
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_NB_WB_HEADSET_A_MIC_MT2533\n");

            *out_data_buffer_size = 12;

            *out_data_buffer = EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = (uint32_t)AUDIO_DOWNLINK_AMP_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = (uint32_t)AUDIO_DOWNLINK_AMP_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = ADUIO_DOWNLINK_AMP_DB_STEP;
            i++;
            *(out_data_buffer + i) = EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_STEP;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            break;


        case ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_HEADSET_DUAL_DIGITAL_MIC_MT2523:  // (for voice NB/WB mode1,4)
            // stream out levels, stream out level min value, stream out level max value, stream out level step,
            // stream in levels, stream in level min value, stream in level max value, stream in level step,
            // sidetone levels,, sidetone level min value, sidetone level max value,, sidetone level step.
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_NB_WB_HEADSET_DUAL_DIGITAL_MIC_MT2523\n");

            *out_data_buffer_size = 12;

            *out_data_buffer = EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = (uint32_t)AUDIO_DOWNLINK_AMP_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = (uint32_t)AUDIO_DOWNLINK_AMP_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = ADUIO_DOWNLINK_AMP_DB_STEP;
            i++;
            *(out_data_buffer + i) = EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_STEP;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            break;


        case ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_HANDSFREE_DUAL_DIGITAL_MIC_MT2523:  // (for voice NB/WB mode 3)
            // stream out levels, stream out level min value, stream out level max value, stream out level step,
            // stream in levels, stream in level min value, stream in level max value, stream in level step,
            // sidetone levels,, sidetone level min value, sidetone level max value,, sidetone level step.
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_VOICE_GAIN_INFO_NB_WB_HANDSFREE_DUAL_DIGITAL_MIC_MT2523\n");

            *out_data_buffer_size = 12;

            *out_data_buffer = EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = (uint32_t)VOICE_DOWNLINK_AMP_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_DOWNLINK_AMP_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_DOWNLINK_AMP_DB_STEP;
            i++;
            *(out_data_buffer + i) = EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_STEP;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            break;


        case ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_A_MIC_MT2533:  // (for record wb mode 1)
            // stream out levels, stream out level min value, stream out level max value, stream out level step,
            // stream in levels, stream in level min value, stream in level max value, stream in level step,
            // sidetone levels,, sidetone level min value, sidetone level max value,, sidetone level step.
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_WB_A_MIC_MT2533\n");

            *out_data_buffer_size = 12;

            *out_data_buffer = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_AMP_DB_STEP;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_DUAL_DIGITAL_MIC_MT2533:  // (for record mode2~3)
            // stream out levels, stream out level min value, stream out level max value, stream out level step,
            // stream in levels, stream in level min value, stream in level max value, stream in level step,
            // sidetone levels,, sidetone level min value, sidetone level max value,, sidetone level step.
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_RECORD_GAIN_INFO_WB_DUAL_MIC_MT2533\n");

            *out_data_buffer_size = 12;

            *out_data_buffer = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_MINIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_MAXIMUM;
            i++;
            *(out_data_buffer + i) = VOICE_UPLINK_DMIC_DB_STEP;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            break;




        case ATTDET_PARAMETER_INDEX_VOICE_NB_MODE_AND_FIR_INFO_DUAL_DIGITAL_MIC_MT2533:
            // mode parameter array size, mode parameter value range(Amount of bits),
            // input FIR coefficient array size, input FIR coefficient value range(Amount of bits), Amount of frquency, Amount of response,
            // output FIR coefficient array size, output FIR coefficient value range(Amount of bits), Amount of frquency, Amount of response,
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_VOICE_NB_ENHANCEMENT_INFO_DUAL_DIGITAL_MIC_MT2533\n");

            *out_data_buffer_size = 10;

            *out_data_buffer = EAPS_VOICE_NB_MODE_PARAMETERS_PARAMETER_SIZE;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_mode_parameter[0]) * bits_per_byte;
            i++;

            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            *(out_data_buffer + i) = EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient[0]) * bits_per_byte;
            i++;
            *(out_data_buffer + i) = EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;
            i++;
            *(out_data_buffer + i) = EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;
            i++;

            break;



        case ATTDET_PARAMETER_INDEX_VOICE_WB_ENHANCEMENT_INFO_DUAL_DIGITAL_MIC_MT2533:
            // mode parameter array size, mode parameter value range(Amount of bits),
            // input FIR coefficient array size, input FIR coefficient value range(Amount of bits), Amount of frquency, Amount of response,
            // output FIR coefficient array size, output FIR coefficient value range(Amount of bits), Amount of frquency, Amount of response,"
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_VOICE_WB_ENHANCEMENT_INFO_DUAL_DIGITAL_MIC_MT2533\n");

            *out_data_buffer_size = 10;

            *out_data_buffer = EAPS_VOICE_WB_MODE_PARAMETERS_PARAMETER_SIZE;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_mode_parameter[0]) * bits_per_byte;
            i++;

            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            *(out_data_buffer + i) = EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient[0]) * bits_per_byte;
            i++;
            *(out_data_buffer + i) = EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;
            i++;
            *(out_data_buffer + i) = EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;
            i++;

            break;


        case ATTDET_PARAMETER_INDEX_RECORD_WB_ENHANCEMENT_INFO_DUAL_DIGITAL_MIC_MT2533:  // (for record WB mode1)
            // mode parameter array size, mode parameter value range(Amount of bits),
            // input channel 1 FIR coefficient array size, input channel 1 coefficient value range(Amount of bits), Amount of frquency, Amount of response,
            // input channel 2  FIR coefficient array size, input channel 2 FIR coefficient value range(Amount of bits), Amount of frquency, Amount of response,
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_RECORD_WB_ENHANCEMENT_INFO_DUAL_DIGITAL_MIC_MT2533\n");

            *out_data_buffer_size = 10;

            *out_data_buffer = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;
            *(out_data_buffer + i) = 0;
            i++;

            break;


        case ATTDET_PARAMETER_INDEX_EXTERNAL_DSP_SENDING_PATH_INFO_MT2533: // (FOR VOICE/RECORD DUAL DIGITAL MIC MT2533)
            // customer register amount, register value range(Amount of bits)
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_EXTERNAL_DSP_SENDING_PATH_INFO_MT2533\n");

            *out_data_buffer_size = 2;

            *out_data_buffer = EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT;
            i++;
            *(out_data_buffer + i) = sizeof(eaps_structure.voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info[0].register_value) * bits_per_byte;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_EXTERNAL_DSP_SENDING_PATH_POWER_ON_INDEX_VALUE_MT2533:
            // customer register amount, register value range(Amount of bits)
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_EXTERNAL_DSP_SENDING_PATH_POWER_ON_INDEX_VALUE_MT2533\n");

            *out_data_buffer_size = EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT;

            external_dsp_power_on_default_value_t *external_dsp_power_on_default_value = NULL;
            external_dsp_power_on_default_value = (external_dsp_power_on_default_value_t *)pvPortMalloc(sizeof(external_dsp_power_on_default_value_t));

            audio_nvdm_get_external_dsp_power_on_default_value_by_memcpy(external_dsp_power_on_default_value);

            for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                *(out_data_buffer + i) = (uint32_t)external_dsp_power_on_default_value->register_info[i].index_value;
            }

            vPortFree(external_dsp_power_on_default_value);

            break;

        case ATTDET_PARAMETER_INDEX_EXTERNAL_DSP_SENDING_PATH_POWER_ON_REG_VALUE_MT2533:
            // customer register amount, register value range(Amount of bits)
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_EXTERNAL_DSP_SENDING_PATH_POWER_ON_REG_VALUE_MT2533\n");
            {   // if we don't use () it will compile error since external_dsp_power_on_default_value redifinition
                *out_data_buffer_size = EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT;

                external_dsp_power_on_default_value_t *external_dsp_power_on_default_value = NULL;
                external_dsp_power_on_default_value = (external_dsp_power_on_default_value_t *)pvPortMalloc(sizeof(external_dsp_power_on_default_value_t));            

                audio_nvdm_get_external_dsp_power_on_default_value_by_memcpy(external_dsp_power_on_default_value);

                for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                    *(out_data_buffer + i) = (uint32_t)external_dsp_power_on_default_value->register_info[i].register_value;
                }

                vPortFree(external_dsp_power_on_default_value);
            }
            break;

        case ATTDET_PARAMETER_INDEX_SUPPORT_IOT_PC_AUDIO_TUNING_TOOL:
            // (0: not support; 1: support), chip id
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_SUPPORT_IOT_PC_AUDIO_TUNING_TOOL\n");

            *out_data_buffer_size = 2;

            *out_data_buffer = 1;
            i++;
            *(out_data_buffer + i) = 2533;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_SUPPORT_VOCIE_TEST:
            // (0: not support; 1: support)
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_SUPPORT_VOCIE_TEST\n");

            *out_data_buffer_size = 1;

            *out_data_buffer = 1;
            i++;

            break;

        case ATTDET_PARAMETER_INDEX_SUPPORT_RECORD_TEST:
            // (0: not support; 1: support)
            LOGI("parameter index: ATTDET_PARAMETER_INDEX_SUPPORT_RECORD_TEST\n");

            *out_data_buffer_size = 1;

            *out_data_buffer = 1;
            i++;

            break;

        default:
            LOGP("query_useful_information_for_ui_tool: parameter index error\n");

            *out_data_buffer_size = 0;

            *out_data_buffer = 0;

            attdet_error_check = ATTDET_ERROR;
            break;
    }

    return attdet_error_check;
}


EAPS_structure_check_t EAPS_AT_command_in_array_to_EAPS_strucutre(EAPS_operation_t *EAPS_operation, uint32_t *EAPS_operation_is_get_out_data_buffer, int16_t *EAPS_operation_is_get_out_data_buffer_size, audio_eaps_t *EAPS_structure, uint32_t AT_command_array[], int AT_command_array_size)
{

    EAPS_structure_check_t EAPS_structure_check = EAPS_STRUCUTRE_CHECK_NO_ERROR;
    int i = 0;
    int i_temp = 0;


    switch (AT_command_array[1]) {
        case EAPS_SCENARIO_COMMON:
            switch (AT_command_array[2]) {
                case EAPS_DEVICE_MODE_COMMON:
                    switch (AT_command_array[3]) {
                        case EAPS_PARAMETER_INDEX_SPEECH_COMMON_PARAMETER:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EAPS_SPEECH_COMMON_PARAMETER_SIZE;

                                    //vvvv get data from NVDM vvvv
                                    for (i = 0; i < EAPS_SPEECH_COMMON_PARAMETER_SIZE; i++) {
                                        *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->speech_common_parameter.speech_common_parameter[i];
                                    }
                                    //^^^ get data from NVDM ^^^

                                    // debug
                                    LOGI("EAPS_SPEECH_COMMON_PARAMETER\n");
                                    //for (i = 0; i < EAPS_SPEECH_COMMON_PARAMETER_SIZE; i++) {
                                    //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                    //}

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    for (i = 0; i < EAPS_SPEECH_COMMON_PARAMETER_SIZE; i++) {
                                        EAPS_structure->speech_common_parameter.speech_common_parameter[i] = (uint16_t)(AT_command_array[4 + i]);
                                    }

                                    // debug
                                    //for (i = 0; i < EAPS_SPEECH_COMMON_PARAMETER_SIZE; i++) {
                                    //    LOGI("EAPS_structure->speech_common_parameter.speech_common_parameter[%d]=%d. \n", i, EAPS_structure->speech_common_parameter.speech_common_parameter[i]);
                                    //}

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;
                            }

                            break;

                        default:
                            EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                            break;
                    }

                    break;

                default:
                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                    break;
            }

            break;

        case EAPS_SCENARIO_AUDIO:
            switch (AT_command_array[2]) {
                case EAPS_DEVICE_MODE_AUDIO_GAIN_MODE:
                    switch (AT_command_array[3]) {
                        case EAPS_PARAMETER_INDEX_GAIN_PARAMETER:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE;

                                    //vvvv get data from NVDM vvvv
                                    if (AT_command_array[4] == EAPS_GAIN_PARAMETER_CHOOSE_HEADSET) {
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->audio_parameter.handsfree_headset_stream_out_gain.headset_gain[i];
                                        }
                                    } else if (AT_command_array[4] == EAPS_GAIN_PARAMETER_CHOOSE_HANDSFREE) {
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->audio_parameter.handsfree_headset_stream_out_gain.handsfree_gain[i];
                                        }
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    //^^^ get data from NVDM ^^^

                                    // debug
                                    LOGI("EAPS, audio gain mode, gain parameter\n");
                                    //for (i = 0; i < *EAPS_operation_is_get_out_data_buffer_size; i++) {
                                    //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                    //}

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if (AT_command_array[4] == EAPS_GAIN_PARAMETER_CHOOSE_HEADSET) {
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                            EAPS_structure->audio_parameter.handsfree_headset_stream_out_gain.headset_gain[i] = (uint16_t)(AT_command_array[5 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                        //    LOGI("EAPS_structure->audio_parameter.audio_handsfree_headset_gain_parameter.headset_gain.stream_out_gain[%d]=%d. \n", i, EAPS_structure->audio_parameter.audio_handsfree_headset_gain_parameter.headset_gain.stream_out_gain[i]);
                                        //}
                                        //LOGI("EAPS_structure->audio_parameter.audio_handsfree_headset_gain_parameter.headset_gain.stream_in_gain = %d. \n", EAPS_structure->audio_parameter.audio_handsfree_headset_gain_parameter.headset_gain.stream_in_gain[0]);
                                        //LOGI("EAPS_structure->audio_parameter.audio_handsfree_headset_gain_parameter.headset_gain.sidetone_gain = %d. \n", EAPS_structure->audio_parameter.audio_handsfree_headset_gain_parameter.headset_gain.sidetone_gain);
                                    } else if (AT_command_array[4] == EAPS_GAIN_PARAMETER_CHOOSE_HANDSFREE) {
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                            EAPS_structure->audio_parameter.handsfree_headset_stream_out_gain.handsfree_gain[i] = (uint16_t)(AT_command_array[5 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                        //    LOGI("EAPS_structure->audio_parameter.audio_handsfree_headset_gain_parameter.handsfree_gain.stream_out_gain[%d]=%d. \n", i, EAPS_structure->audio_parameter.audio_handsfree_headset_gain_parameter.handsfree_gain.stream_out_gain[i]);
                                        //}
                                        //LOGI("EAPS_structure->audio_parameter.audio_handsfree_headset_gain_parameter.handsfree_gain.stream_in_gain = %d. \n", EAPS_structure->audio_parameter.audio_handsfree_headset_gain_parameter.handsfree_gain.stream_in_gain[0]);
                                        //LOGI("EAPS_structure->audio_parameter.audio_handsfree_headset_gain_parameter.handsfree_gain.sidetone_gain = %d. \n", EAPS_structure->audio_parameter.audio_handsfree_headset_gain_parameter.handsfree_gain.sidetone_gain);
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }


                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;
                            }

                            break;

                        default:
                            EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                            break;
                    }

                    break;

                case EAPS_DEVICE_MODE_AUDIO_ACF_MODE_1:
                case EAPS_DEVICE_MODE_AUDIO_ACF_MODE_2:
                case EAPS_DEVICE_MODE_AUDIO_ACF_MODE_3:
                    switch (AT_command_array[3]) {
                        case EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_1:
                        case EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_2:
                        case EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_3:
                        case EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_4:
                        case EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_5:
                        case EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_6:
                        case EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_7:
                        case EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_8:
                        case EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_9:
                        case EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_10:
                        case EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_11:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EAPS_AUDIO_IIR_FILTER_PARAMETER_SIZE;

                                    //vvvv get data from NVDM vvvv
                                    for (i = 0; i < EAPS_AUDIO_IIR_FILTER_PARAMETER_SIZE; i++) {
                                        *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->audio_parameter.audio_post_processing_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_AUDIO_ACF_MODE_1].audio_post_processing_compensation_filter[AT_command_array[3] - EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_1].audio_iir_design_parameter[i];
                                    }
                                    //^^^ ToDo: get data from NVDM ^^^

                                    // debug
                                    LOGI("EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_%d, ACFDevice mode = %d\n", AT_command_array[3] - EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_1, AT_command_array[2] - EAPS_DEVICE_MODE_AUDIO_ACF_MODE_1);
                                    //for (i = 0; i < EAPS_AUDIO_IIR_FILTER_PARAMETER_SIZE; i++) {
                                    //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                    //}

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    for (i = 0; i < EAPS_AUDIO_IIR_FILTER_PARAMETER_SIZE; i++) {
                                        EAPS_structure->audio_parameter.audio_post_processing_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_AUDIO_ACF_MODE_1].audio_post_processing_compensation_filter[AT_command_array[3] - EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_1].audio_iir_design_parameter[i] = AT_command_array[4 + i];
                                    }

                                    // debug
                                    //for (i = 0; i < EAPS_AUDIO_IIR_FILTER_PARAMETER_SIZE; i++) {
                                    //    LOGI("EAPS_structure->audio_parameter.audio_post_processing_parameter[%d].audio_post_processing_compensation_filter[%d].iir_design_parameter[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_AUDIO_ACF_MODE_1, AT_command_array[3] - EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_1, i, EAPS_structure->audio_parameter.audio_post_processing_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_AUDIO_ACF_MODE_1].audio_post_processing_compensation_filter[AT_command_array[3] - EAPS_PARAMETER_INDEX_AUDIO_IIR_FILTER_1].audio_iir_design_parameter[i] = AT_command_array[4 + i]);
                                    //}

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;

                        default:
                            EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                            break;
                    }

                    break;

                default:
                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                    break;
            }

            break;

        case EAPS_SCENARIO_VOICE:
            switch (AT_command_array[2]) {
                case EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533:
                case EAPS_DEVICE_MODE_VOICE_NB_MODE_2_MT2533:
                case EAPS_DEVICE_MODE_VOICE_NB_MODE_3_MT2533:
                case EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533:
                case EAPS_DEVICE_MODE_VOICE_NB_MODE_5_MT2533:
                case EAPS_DEVICE_MODE_VOICE_NB_MODE_6_MT2533:                    
                    switch (AT_command_array[3]) {
                        case EAPS_PARAMETER_INDEX_GAIN_PARAMETER:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE + EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE + EAPS_GAIN_PARAMETER_SIDETONE_GAIN_SIZE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_3_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_gain.stream_out_gain[i];
                                        }

                                        i_temp = i;

                                        for (; i < i_temp + EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_gain.stream_in_gain[i - i_temp];
                                        }
                                        
                                        *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_gain.sidetone_gain;
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS, voice nb mode[%x], gain parameter\n", AT_command_array[2]);
                                        //for (i = 0; i < *EAPS_operation_is_get_out_data_buffer_size; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_6_MT2533) {

                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_gain.stream_out_gain[i];
                                        }
                                        
                                        i_temp = i;
                                        
                                        for (; i < i_temp + EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_gain.stream_in_gain[i - i_temp];
                                        }
                                        
                                        *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_gain.sidetone_gain;
                                        //^^^ get data from NVDM ^^^
                                        
                                        // debug
                                        LOGI("EAPS, voice nb with_external_dsp mode[%x], gain parameter\n", AT_command_array[2]);
                                        //for (i = 0; i < *EAPS_operation_is_get_out_data_buffer_size; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}


                                    
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }
                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_3_MT2533) {
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_gain.stream_out_gain[i] = (uint16_t)(AT_command_array[5 + i]);
                                        }
                                        
                                        i_temp = i;
                                        
                                        for (; i < i_temp + EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_gain.stream_in_gain[i - i_temp] = (uint16_t)(AT_command_array[5 + i]);
                                        }
                                        
                                        
                                        EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_gain.sidetone_gain = (uint16_t)(AT_command_array[5 + i]);

                                        // debug
                                        //for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_gain.stream_out_gain[%d]=%d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_gain.stream_out_gain[i]);
                                        //}
                                        //LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_gain.stream_in_gain = %d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_gain.stream_in_gain[0]);
                                        //LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_gain.sidetone_gain = %d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_gain.sidetone_gain);
                                    } else if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_6_MT2533) {
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_gain.stream_out_gain[i] = (uint16_t)(AT_command_array[5 + i]);
                                        }
                                        
                                        i_temp = i;
                                        
                                        for (; i < i_temp + EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_gain.stream_in_gain[i - i_temp] = (uint16_t)(AT_command_array[5 + i]);
                                        }
                                        
                                        
                                        EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_gain.sidetone_gain = (uint16_t)(AT_command_array[5 + i]);

                                        // debug
                                        //for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_gain.stream_out_gain[%d]=%d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_gain.stream_out_gain[i]);
                                        //}
                                        //LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_gain.stream_in_gain = %d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_gain.stream_in_gain[0]);
                                        //LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_gain.sidetone_gain = %d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_gain.sidetone_gain);


                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;
                            }

                            break;

                        case EAPS_PARAMETER_INDEX_VOICE_NB_MODE_PARAMETERS:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EAPS_VOICE_NB_MODE_PARAMETERS_PARAMETER_SIZE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_3_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EAPS_VOICE_NB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_enhancement_parameter.nb_mode_parameter[i];
                                        }
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_VOICE_NB_MODE_PARAMETERS, voice nb Device mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_NB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_6_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EAPS_VOICE_NB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_enhancement_parameter.nb_mode_parameter[i];
                                        }
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_VOICE_NB_MODE_PARAMETERS, voice nb with_external_dsp mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_NB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}                                    
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }
                                    
                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_3_MT2533) {
                                        for (i = 0; i < EAPS_VOICE_NB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_enhancement_parameter.nb_mode_parameter[i] = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_NB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_enhancement_parameter.nb_mode_parameter[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_enhancement_parameter.nb_mode_parameter[i]);
                                        //}
                                    } else if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_6_MT2533) {
                                        for (i = 0; i < EAPS_VOICE_NB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_enhancement_parameter.nb_mode_parameter[i] = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_NB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_enhancement_parameter.nb_mode_parameter[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_enhancement_parameter.nb_mode_parameter[i]);
                                        //}                                    
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;

                        case EAPS_PARAMETER_INDEX_VOICE_NB_INPUT_FIR_COEFFICIENTS:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_3_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[i];
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_enhancement_parameter.nb_stream_in_fir_frequency[i];
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_enhancement_parameter.nb_stream_in_fir_response[i];
                                        }

                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS, voice nb Device mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_3_MT2533) {
                                        for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[i] = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_enhancement_parameter.nb_stream_in_fir_frequency[i] = (uint16_t)(AT_command_array[4 + EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + i]);
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_enhancement_parameter.nb_stream_in_fir_response[i] = (uint16_t)(AT_command_array[4 + EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[i]);
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;

                        case EAPS_PARAMETER_INDEX_VOICE_NB_OUTPUT_FIR_COEFFICIENTS:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_3_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient[i];
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_enhancement_parameter.nb_stream_out_fir_frequency[i];
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_enhancement_parameter.nb_stream_out_fir_response[i];
                                        }

                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS, voice nb Device mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_6_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient[i];
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_enhancement_parameter.nb_stream_out_fir_frequency[i];
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + i) = EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_enhancement_parameter.nb_stream_out_fir_response[i];
                                        }

                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS, voice nb with_external_dsp mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}                                    
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_3_MT2533) {
                                        for (i = 0; i < EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient[i] = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_enhancement_parameter.nb_stream_out_fir_frequency[i] = (uint16_t)(AT_command_array[4 + EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + i]);
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1_MT2533].voice_nb_enhancement_parameter.nb_stream_out_fir_response[i] = (uint16_t)(AT_command_array[4 + EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + i]);
                                        }


                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient[i]);
                                        //}
                                    } else if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_6_MT2533) {
                                        for (i = 0; i < EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient[i] = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_enhancement_parameter.nb_stream_out_fir_frequency[i] = (uint16_t)(AT_command_array[4 + EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + i]);
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_enhancement_parameter.nb_stream_out_fir_response[i] = (uint16_t)(AT_command_array[4 + EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + i]);
                                        }


                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_NB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient[i]);
                                        //}                                    
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;

                        case EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_INDEX_VALUE:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_6_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = (uint32_t)EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info[i].index_value;
                                        }
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_INDEX_VALUE, voice nb with_external_dsp mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_6_MT2533) {
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info[i].index_value = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[i]);
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;


                        case EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_REGISTER_VALUE:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_6_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = (uint32_t)EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info[i].register_value;
                                        }
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_REGISTER_VALUE, voice nb with_external_dsp mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_6_MT2533) {
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT;i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info[i].register_value = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[i]);
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;


                        case EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_NEED_TO_DOWNLOAD:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_6_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = (uint32_t)EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info[i].need_to_download_flag;
                                        }
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_NEED_TO_DOWNLOAD, voice nb with_external_dsp mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_NB_MODE_6_MT2533) {
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            EAPS_structure->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_4_MT2533].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info[i].need_to_download_flag = (uint8_t)(AT_command_array[4 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[i]);
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;


                        default:
                            EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                            break;
                    }

                    break;

                case EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533:
                case EAPS_DEVICE_MODE_VOICE_WB_MODE_2_MT2533:
                case EAPS_DEVICE_MODE_VOICE_WB_MODE_3_MT2533:
                case EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533:
                case EAPS_DEVICE_MODE_VOICE_WB_MODE_5_MT2533:
                case EAPS_DEVICE_MODE_VOICE_WB_MODE_6_MT2533:                    
                    switch (AT_command_array[3]) {
                        case EAPS_PARAMETER_INDEX_GAIN_PARAMETER:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE + EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE + EAPS_GAIN_PARAMETER_SIDETONE_GAIN_SIZE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_3_MT2533) {
                                        //vvvv get data from NVDM vvvv 
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_gain.stream_out_gain[i];
                                        }


                                        i_temp = i;

                                        for (; i < i_temp + EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_gain.stream_in_gain[i - i_temp];
                                        }


                                        *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_gain.sidetone_gain;
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS, voice wb mode[%x], gain parameter\n", AT_command_array[2]);
                                        //for (i = 0; i < *EAPS_operation_is_get_out_data_buffer_size; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_6_MT2533) {
                                        //vvvv get data from NVDM vvvv 
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_gain.stream_out_gain[i];
                                        }


                                        i_temp = i;

                                        for (; i < i_temp + EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_gain.stream_in_gain[i - i_temp];
                                        }


                                        *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_gain.sidetone_gain;
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS, voice wb with_external_dsp mode[%x], gain parameter\n", AT_command_array[2]);
                                        //for (i = 0; i < *EAPS_operation_is_get_out_data_buffer_size; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_3_MT2533) {
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_gain.stream_out_gain[i] = (uint16_t)(AT_command_array[5 + i]);
                                        }

                                        i_temp = i;
                                        
                                        for (; i < i_temp + EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_gain.stream_in_gain[i - i_temp] = (uint16_t)(AT_command_array[5 + i]);
                                        }

                                        EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_gain.sidetone_gain = (uint16_t)(AT_command_array[5 + i]);

                                        // debug
                                        //for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_wb_parameter[%d].voice_wb_gain.stream_out_gain[%d]=%d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1, i, EAPS_structure->voice_parameter.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1].voice_wb_gain.stream_out_gain[i]);
                                        //}
                                        //LOGI("EAPS_structure->voice_parameter.voice_wb_parameter[%d].voice_wb_gain.stream_in_gain = %d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1, EAPS_structure->voice_parameter.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1].voice_wb_gain.stream_in_gain[0]);
                                        //LOGI("EAPS_structure->voice_parameter.voice_wb_parameter[%d].voice_wb_gain.sidetone_gain = %d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1, EAPS_structure->voice_parameter.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1].voice_wb_gain.sidetone_gain);
                                    } else if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_6_MT2533) {
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_gain.stream_out_gain[i] = (uint16_t)(AT_command_array[5 + i]);
                                        }

                                        i_temp = i;
                                        
                                        for (; i < i_temp + EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_gain.stream_in_gain[i - i_temp] = (uint16_t)(AT_command_array[5 + i]);
                                        }

                                        EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_gain.sidetone_gain = (uint16_t)(AT_command_array[5 + i]);

                                        // debug
                                        //for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_wb_parameter[%d].voice_wb_gain.stream_out_gain[%d]=%d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1, i, EAPS_structure->voice_parameter.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1].voice_wb_gain.stream_out_gain[i]);
                                        //}
                                        //LOGI("EAPS_structure->voice_parameter.voice_wb_parameter[%d].voice_wb_gain.stream_in_gain = %d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1, EAPS_structure->voice_parameter.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1].voice_wb_gain.stream_in_gain[0]);
                                        //LOGI("EAPS_structure->voice_parameter.voice_wb_parameter[%d].voice_wb_gain.sidetone_gain = %d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1, EAPS_structure->voice_parameter.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1].voice_wb_gain.sidetone_gain);                                    
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }
                                    
                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;
                            }

                            break;

                        case EAPS_PARAMETER_INDEX_VOICE_WB_MODE_PARAMETERS:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EAPS_VOICE_WB_MODE_PARAMETERS_PARAMETER_SIZE;
                                    
                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_3_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EAPS_VOICE_WB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_enhancement_parameter.wb_mode_parameter[i];
                                        }
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_VOICE_WB_MODE_PARAMETERS, voice wb Device mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_WB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_6_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EAPS_VOICE_WB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_enhancement_parameter.wb_mode_parameter[i];
                                        }
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_VOICE_WB_MODE_PARAMETERS, voice wb with_external_dsp mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_WB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}                                    
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;
                                    
                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_3_MT2533) {
                                        for (i = 0; i < EAPS_VOICE_WB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_enhancement_parameter.wb_mode_parameter[i] = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_WB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_wb_parameter[%d].voice_wb_enhancement_parameter.wb_mode_parameter[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1, i, EAPS_structure->voice_parameter.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1].voice_wb_enhancement_parameter.wb_mode_parameter[i]);
                                        //}
                                    } else if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_6_MT2533) {
                                        for (i = 0; i < EAPS_VOICE_WB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_enhancement_parameter.wb_mode_parameter[i] = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_WB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_wb_parameter[%d].voice_wb_enhancement_parameter.wb_mode_parameter[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1, i, EAPS_structure->voice_parameter.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1].voice_wb_enhancement_parameter.wb_mode_parameter[i]);
                                        //}                                    
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;
                        case EAPS_PARAMETER_INDEX_VOICE_WB_INPUT_FIR_COEFFICIENTS:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EAPS_VOICE_WB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_3_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EAPS_VOICE_WB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient[i];
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + EAPS_VOICE_WB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_enhancement_parameter.wb_stream_in_fir_frequency[i];
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + EAPS_VOICE_WB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_enhancement_parameter.wb_stream_in_fir_response[i];
                                        }

                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_VOICE_WB_INPUT_FIR_COEFFICIENTS, voice wb Device mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_WB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_3_MT2533) {
                                        for (i = 0; i < EAPS_VOICE_WB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient[i] = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_enhancement_parameter.wb_stream_in_fir_frequency[i] = (uint16_t)(AT_command_array[4 + EAPS_VOICE_WB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + i]);
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_enhancement_parameter.wb_stream_in_fir_response[i] = (uint16_t)(AT_command_array[4 + EAPS_VOICE_WB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_WB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_wb_parameter[%d].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1, i, EAPS_structure->voice_parameter.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient[i]);
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }


                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;

                        case EAPS_PARAMETER_INDEX_VOICE_WB_OUTPUT_FIR_COEFFICIENTS:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_3_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient[i];
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_enhancement_parameter.wb_stream_out_fir_frequency[i];
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_enhancement_parameter.wb_stream_out_fir_response[i];
                                        }

                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS, voice wb Device mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_6_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient[i];
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_enhancement_parameter.wb_stream_out_fir_frequency[i];
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + i) = EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_enhancement_parameter.wb_stream_out_fir_response[i];
                                        }

                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS, voice wb with_external_dsp mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}

                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_3_MT2533) {
                                        for (i = 0; i < EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient[i] = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_enhancement_parameter.wb_stream_out_fir_frequency[i] = (uint16_t)(AT_command_array[4 + EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + i]);
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1_MT2533].voice_wb_enhancement_parameter.wb_stream_out_fir_response[i] = (uint16_t)(AT_command_array[4 + EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_wb_parameter[%d].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1, i, EAPS_structure->voice_parameter.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient[i]);
                                        //}
                                    } else if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_6_MT2533) {                                        
                                        for (i = 0; i < EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient[i] = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_enhancement_parameter.wb_stream_out_fir_frequency[i] = (uint16_t)(AT_command_array[4 + EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + i]);
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_enhancement_parameter.wb_stream_out_fir_response[i] = (uint16_t)(AT_command_array[4 + EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_WB_OUTPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_wb_parameter[%d].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1, i, EAPS_structure->voice_parameter.voice_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_1].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient[i]);
                                        //}                                    
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;
                            }

                            break;

                        case EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_INDEX_VALUE:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_6_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = (uint32_t)EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].index_value;
                                        }
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_INDEX_VALUE, voice wb with_external_dsp mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_6_MT2533) {
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].index_value = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[i]);
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;                            


                        case EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_REGISTER_VALUE:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_6_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = (uint32_t)EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].register_value;
                                        }
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_REGISTER_VALUE, voice wb with_external_dsp mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_6_MT2533) {
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].register_value = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[i]);
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;                            


                        case EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_NEED_TO_DOWNLOAD:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_6_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = (uint32_t)EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].need_to_download_flag;
                                        }
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_NEED_TO_DOWNLOAD, voice wb with_external_dsp mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_VOICE_WB_MODE_6_MT2533) {
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            EAPS_structure->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_WB_MODE_4_MT2533].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].need_to_download_flag = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[i]);
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;                            

                        default:
                            EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                            break;


                    }

                    break;

                default:
                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                    break;
            }

            break;

        case EAPS_SCENARIO_RECORD:
            switch (AT_command_array[2]) {
                case EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533:
                case EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533:
                case EAPS_DEVICE_MODE_RECORD_WB_MODE_3_MT2533:
                    switch (AT_command_array[3]) {
                        case EAPS_PARAMETER_INDEX_GAIN_PARAMETER:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533) {
                                        //vvvv ToDo: get data from NVDM vvvv
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533].stream_in_gain[i];
                                        }
                                        //^^^ ToDo: get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS, record wb mode[%d], gain parameter\n", AT_command_array[2]);
                                        //for (i = 0; i < *EAPS_operation_is_get_out_data_buffer_size; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else if(AT_command_array[2] >= EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_RECORD_WB_MODE_3_MT2533) {
                                        //vvvv ToDo: get data from NVDM vvvv
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->record_parameter.record_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533].stream_in_gain[i];
                                        }
                                        //^^^ ToDo: get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS, record wb with_external_dsp mode[%d], gain parameter\n", AT_command_array[2]);
                                        //for (i = 0; i < *EAPS_operation_is_get_out_data_buffer_size; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}                                    
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }
                                        
                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533) {
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE; i++) {
                                            EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1].stream_in_gain[i] = (uint16_t)(AT_command_array[5 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                        //    LOGI("EAPS_structure->record_parameter.record_wb_parameter[%d].record_wb_gain.stream_out_gain[%d]=%d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1, i, EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1].record_wb_gain.stream_out_gain[i]);
                                        //}
                                        //LOGI("EAPS_structure->record_parameter.record_wb_parameter[%d].record_wb_gain.stream_in_gain = %d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1, EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1].record_wb_gain.stream_in_gain[0]);
                                        //LOGI("EAPS_structure->record_parameter.record_wb_parameter[%d].record_wb_gain.sidetone_gain = %d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1, EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1].record_wb_gain.sidetone_gain);
                                    } else if(AT_command_array[2] >= EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_RECORD_WB_MODE_3_MT2533) {
                                        for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_IN_GAIN_SIZE; i++) {
                                            EAPS_structure->record_parameter.record_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533].stream_in_gain[i] = (uint16_t)(AT_command_array[5 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_GAIN_PARAMETER_STREAM_OUT_GAIN_SIZE; i++) {
                                        //    LOGI("EAPS_structure->record_parameter.record_wb_parameter[%d].record_wb_gain.stream_out_gain[%d]=%d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1, i, EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1].record_wb_gain.stream_out_gain[i]);
                                        //}
                                        //LOGI("EAPS_structure->record_parameter.record_wb_parameter[%d].record_wb_gain.stream_in_gain = %d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1, EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1].record_wb_gain.stream_in_gain[0]);
                                        //LOGI("EAPS_structure->record_parameter.record_wb_parameter[%d].record_wb_gain.sidetone_gain = %d. \n", AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1, EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1].record_wb_gain.sidetone_gain);                                    
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                        
                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;
                            }

                            break;

                        case EAPS_PARAMETER_INDEX_RECORD_MODE_PARAMETERS:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EAPS_RECORD_WB_MODE_PARAMETERS_PARAMETER_SIZE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EAPS_RECORD_WB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533].record_wb_enhancement_parameter.record_wb_mode_parameter[i];
                                        }
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_PARAMETER_INDEX_RECORD_MODE_PARAMETERS, record wb device mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533);
                                        //for (i = 0; i < EAPS_RECORD_WB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533) {
                                        for (i = 0; i < EAPS_RECORD_WB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                            EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533].record_wb_enhancement_parameter.record_wb_mode_parameter[i] = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_RECORD_WB_MODE_PARAMETERS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->record_parameter.record_wb_parameter[%d].record_wb_enhancement_parameter.record_wb_mode_parameter[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1, i, EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1].record_wb_enhancement_parameter.record_wb_mode_parameter[i]);
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;
                        case EAPS_PARAMETER_INDEX_RECORD_CHANNEL_1_INPUT_FIR_COEFFICIENTS:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EAPS_RECORD_WB_CHANNEL_1_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533) {
                                        //vvvv ToDo: get data from NVDM vvvv
                                        for (i = 0; i < EAPS_RECORD_WB_CHANNEL_1_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533].record_wb_enhancement_parameter.record_wb_channel_1_fir_coefficient[i];
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + EAPS_RECORD_WB_CHANNEL_1_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + i) = EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533].record_wb_enhancement_parameter.record_wb_channel_1_fir_frequency[i];
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + EAPS_RECORD_WB_CHANNEL_1_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + i) = EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533].record_wb_enhancement_parameter.record_wb_channel_1_fir_response[i];
                                        }

                                        //^^^ ToDo: get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_RECORD_WB_CHANNEL_1_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE, record wb device mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533);
                                        //for (i = 0; i < EAPS_RECORD_WB_CHANNEL_1_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }


                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533) {
                                        for (i = 0; i < EAPS_RECORD_WB_CHANNEL_1_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                            EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533].record_wb_enhancement_parameter.record_wb_channel_1_fir_coefficient[i] = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533].record_wb_enhancement_parameter.record_wb_channel_1_fir_frequency[i] = (uint16_t)(AT_command_array[4 + EAPS_RECORD_WB_CHANNEL_1_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + i]);
                                        }

                                        for (i = 0; i < EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                            EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1_MT2533].record_wb_enhancement_parameter.record_wb_channel_1_fir_response[i] = (uint16_t)(AT_command_array[4 + EAPS_RECORD_WB_CHANNEL_1_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + i]);
                                        }


                                        // debug
                                        //for (i = 0; i < EAPS_RECORD_WB_CHANNEL_1_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->record_parameter.record_wb_parameter[%d].record_wb_enhancement_parameter.record_wb_channel_1_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1, i, EAPS_structure->record_parameter.record_wb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_1].record_wb_enhancement_parameter.record_wb_channel_1_fir_coefficient[i]);
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }


                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }
                            break;


                        case EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_INDEX_VALUE:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_RECORD_WB_MODE_3_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = (uint32_t)EAPS_structure->record_parameter.record_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533].record_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].index_value;
                                        }
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_INDEX_VALUE, record wb with_external_dsp mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_RECORD_WB_MODE_3_MT2533) {
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            EAPS_structure->record_parameter.record_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533].record_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].index_value= (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[i]);
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;                            


                        case EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_REGISTER_VALUE:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_RECORD_WB_MODE_3_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = (uint32_t)EAPS_structure->record_parameter.record_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533].record_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].register_value;
                                        }
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_REGISTER_VALUE, record wb with_external_dsp mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_RECORD_WB_MODE_3_MT2533) {
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            EAPS_structure->record_parameter.record_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533].record_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].register_value = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[i]);
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;                            

                        case EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_NEED_TO_DOWNLOAD:
                            switch (AT_command_array[0]) {
                                case EAPS_OPERATION_GET_OLD_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
                                    *EAPS_operation_is_get_out_data_buffer_size = EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_RECORD_WB_MODE_3_MT2533) {
                                        //vvvv get data from NVDM vvvv
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            *(EAPS_operation_is_get_out_data_buffer + i) = (uint32_t)EAPS_structure->record_parameter.record_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533].record_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].need_to_download_flag;
                                        }
                                        //^^^ get data from NVDM ^^^

                                        // debug
                                        LOGI("EAPS_PARAMETER_INDEX_VOICE_RECORD_EXTERNAL_DSP_SENDING_PATH_NEED_TO_DOWNLOAD, record wb with_external_dsp mode = %d\n", AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533);
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE + EAPS_FIR_FREQUENCY_AND_RESPONSE_SIZE; i++) {
                                        //    LOGI("*(EAPS_operation_is_get_out_data_buffer+%d)=%d", i, *(EAPS_operation_is_get_out_data_buffer + i));
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                case EAPS_OPERATION_SET_NEW_VALUE:
                                    *EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;

                                    if(AT_command_array[2] >= EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533 && AT_command_array[2] <= EAPS_DEVICE_MODE_RECORD_WB_MODE_3_MT2533) {
                                        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
                                            EAPS_structure->record_parameter.record_wb_parameter_with_external_dsp[AT_command_array[2] - EAPS_DEVICE_MODE_RECORD_WB_MODE_2_MT2533].record_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].need_to_download_flag = (uint16_t)(AT_command_array[4 + i]);
                                        }

                                        // debug
                                        //for (i = 0; i < EAPS_VOICE_NB_INPUT_FIR_COEFFICIENTS_PARAMETER_SIZE; i++) {
                                        //    LOGI("EAPS_structure->voice_parameter.voice_nb_parameter[%d].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[%d] = %d \n", AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1, i, EAPS_structure->voice_parameter.voice_nb_parameter[AT_command_array[2] - EAPS_DEVICE_MODE_VOICE_NB_MODE_1].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient[i]);
                                        //}
                                    } else {
                                        EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
                                    }

                                    break;

                                default:
                                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                                    break;

                            }

                            break;                            


                        default:
                            EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                            break;
                    }

                    break;

                default:
                    EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;

                    break;
            }

            break;

        default:
            EAPS_structure_check = EAPS_STRUCUTRE_CHECK_SOME_ELEMENT_OUT_OF_RANGE;
            break;
    }



    return EAPS_structure_check;
}

EAPS_operation_t EAPS_get_operation_type(int AT_command_operation_value)
{
    EAPS_operation_t EAPS_operation = EAPS_OPERATION_INVALID;
    switch (AT_command_operation_value) {
        case EAPS_OPERATION_GET_OLD_VALUE:
            EAPS_operation = EAPS_OPERATION_GET_OLD_VALUE;
            break;

        case EAPS_OPERATION_SET_NEW_VALUE:
            EAPS_operation = EAPS_OPERATION_SET_NEW_VALUE;
            break;

        default:
            EAPS_operation = EAPS_OPERATION_INVALID;
    }

    return EAPS_operation;
}


AT_command_error_check_t segment_AT_command_to_int_array(uint32_t output_array[], int maximum_array_size, int minimum_array_size, atci_parse_cmd_param_t *parse_cmd)
{
    AT_command_error_check_t AT_command_error_check = AT_COMMAND_NO_ERROR;
    char *command_content_for_strtok = NULL;
    int command_content_index = 0;
    uint32_t temp_uint32 = 0;

    command_content_for_strtok = strtok((parse_cmd->string_ptr + parse_cmd->parse_pos), ",\n\r");

    while (command_content_for_strtok != NULL) {
        if (command_content_index >= maximum_array_size) {
            LOGI("too many contents\n");
            AT_command_error_check = AT_COMMAND_TOO_MANY_CONTENTS;
            break;
        }

        temp_uint32 = (uint32_t)strtoul(command_content_for_strtok, NULL, 16);

        //temp_int = (temp_uint16 >= 32768) ? -((uint16_t)~temp_uint16 + 1) : (int)temp_uint16;   // e.g: 0xfff5 = -11

        output_array[command_content_index] = temp_uint32;
        command_content_for_strtok = strtok(NULL, ",\n\r");

        command_content_index++;
    }

    if (command_content_index < (minimum_array_size)) {
        LOGI("too less contents, command_content_index=%d\n", command_content_index);
        AT_command_error_check = AT_COMMAND_TOO_LESS_CONTENTS;
    }

    // vvvv debug vvvv
    LOGI("command_content_index = %d\n", command_content_index);
    //int i = 0;
    //for (i; i < maximum_array_size; i++) {
    //    LOGI("segment_AT_command_to_int_array: command_content_value[%d]=%d\n", i, output_array[i]);
    //}

    // ^^^^ debug ^^^^

    return AT_command_error_check;
}


// EAPS command handler
atci_status_t atci_cmd_hdlr_eaps(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};
    uint32_t command_content_value_array[EAPS_CONTENT_MAX_SIZE] = {0};
    AT_command_error_check_t AT_command_error_check = AT_COMMAND_NO_ERROR;
    audio_eaps_t EAPS_structure = audio_nvdm_get_eaps_data();
    EAPS_structure_check_t EAPS_structure_check = EAPS_STRUCUTRE_CHECK_NO_ERROR;
    EAPS_operation_t EAPS_operation = EAPS_OPERATION_INVALID;
    int ret = 0;
    uint32_t EAPS_operation_is_get_out_data_buffer[EAPS_MAX_PARAMETER_SIZE] = {0};
    int16_t EAPS_operation_is_get_out_data_buffer_size = 0;
    int i = 0;
    uint8_t temp_string[8]; // 0xffffffff  -> eight character


    LOGI("EAPS_command_handler_function\n");

    output.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+EAPS=?
            LOGI("user ask how to use EAPS command.\n");
            strcpy((char *)output.response_buf, "+EAPS:(0-1),(0-3),(0-20),(0;1;100-10A;200-205;300-302),parameter0,parameter1,....\r\nOK\r\n");
            output.response_len = strlen((char *)output.response_buf);
            atci_send_response(&output);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+EAPS=<p1>  the handler need to parse the parameters
            LOGI("EAPS Executing...\n");
            //parsing the parameter


            AT_command_error_check = segment_AT_command_to_int_array(command_content_value_array, EAPS_CONTENT_MAX_SIZE, EAPS_CONTENT_MIN_SIZE, parse_cmd);
            if (AT_command_error_check != AT_COMMAND_NO_ERROR) {
                LOGI("AT_command check error: %d.\n", AT_command_error_check);
                ret = -1;
            } else { // AT command correct
                if (command_content_value_array[0] == EAPS_OPERATION_SET_NEW_VALUE && command_content_value_array[1] == EAPS_SCENARIO_SAVE_DATA_TO_NVDM && command_content_value_array[2] == EAPS_DEVICE_MODE_SAVE_DATA_TO_NVDM && command_content_value_array[3] == EAPS_PARAMETER_SAVE_DATA_TO_NVDM) {
                    LOGI("save all data to NVDM\n");
                    if (audio_nvdm_save_eaps_data_to_nvdm(EAPS_structure) != 0) {
                        LOGW("error: save all data to NVDM fail\n");
                        ret = -1;
                    }
                } else { // not save data to NVDM, just save/get data to/from static eaps structure.
                    EAPS_structure_check = EAPS_AT_command_in_array_to_EAPS_strucutre(&EAPS_operation, EAPS_operation_is_get_out_data_buffer, &EAPS_operation_is_get_out_data_buffer_size, &EAPS_structure, command_content_value_array, (int)sizeof(command_content_value_array));
                    if (EAPS_structure_check != EAPS_STRUCUTRE_CHECK_NO_ERROR) {
                        LOGI("EAPS structure check error: %d.\n", EAPS_structure_check);
                        ret = -1;
                    } else {
                        LOGI("EAPS operation type is= %d.\n", EAPS_operation);

                        if (EAPS_operation == EAPS_OPERATION_GET_OLD_VALUE) {
                            for (i = 0; i < EAPS_operation_is_get_out_data_buffer_size; i++) {
                                LOGI("EAPS_operation_is_get_out_data_buffer[%d] = %d.\n", i, EAPS_operation_is_get_out_data_buffer[i]);
                            }
                        } else if (EAPS_operation == EAPS_OPERATION_SET_NEW_VALUE) {
                            audio_nvdm_set_eaps_data(EAPS_structure);
                        }
                    }
                }
            }

            strcpy((char *)output.response_buf, "+EAPS:");
            for (i = 0; i < EAPS_operation_is_get_out_data_buffer_size; i++) {
                if (i != 0) {
                    strcat((char *)output.response_buf, ",");
                }
                sprintf((char *)temp_string, "%lx", (uint32_t)EAPS_operation_is_get_out_data_buffer[i]);
                strcat((char *)output.response_buf, (char *)temp_string);
            }
            strcat((char *)output.response_buf, "\r\n");


            LOGI("%s\n", output.response_buf);


            LOGI("EAPS ret = %d\n", ret);

            if (ret >= 0) {
                // valid parameter, update the data and return "OK"
                if (EAPS_operation == EAPS_OPERATION_GET_OLD_VALUE) {
                    output.response_len = strlen((char *)output.response_buf);
                } else {
                    output.response_len = 0;
                }

                output.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append "OK" at the end of output buffer
            } else {
                // invalide parameter, return "ERROR"
                output.response_len = 0;
                output.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of output buffer
            };
            atci_send_response(&output);
            break;


        default :
            strcpy((char *)output.response_buf, "ERROR\r\n");
            output.response_len = strlen((char *)output.response_buf);
            atci_send_response(&output);
            break;
    }
    return ATCI_STATUS_OK;
}


// ATTDET command handler
atci_status_t atci_cmd_hdlr_attdet(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};
    uint32_t command_content_value_array[ATTDET_CONTENT_MAX_SIZE] = {0};
    AT_command_error_check_t AT_command_error_check = AT_COMMAND_NO_ERROR;
    int ret = 0;
    int i = 0;
    uint8_t temp_string[8]; // 0xffffffff  -> eight character
    uint32_t attdet_out_data_buffer[EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT] = {0};
    int16_t attdet_out_data_buffer_size = 0;

    LOGI("ATTDET_command_handler_function\n");

    output.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+ATTDET=?
            LOGI("user ask how to use ATTDET command.\n");
            strcpy((char *)output.response_buf, "+ATTDET:TO DO\r\nOK\r\n");
            output.response_len = strlen((char *)output.response_buf);
            atci_send_response(&output);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+ATTDET=<p1>  the handler need to parse the parameters
            LOGI("ATTDET Executing...\n");
            //parsing the parameter


            AT_command_error_check = segment_AT_command_to_int_array(command_content_value_array, ATTDET_CONTENT_MAX_SIZE, ATTDET_CONTENT_MIN_SIZE, parse_cmd);
            if (AT_command_error_check != AT_COMMAND_NO_ERROR) {
                LOGP("AT_command check error: %d.\n", AT_command_error_check);
                ret = -1;
            } else {
                if (query_useful_information_for_ui_tool(attdet_out_data_buffer, &attdet_out_data_buffer_size, command_content_value_array, (int16_t)sizeof(command_content_value_array)) == ATTDET_ERROR) {
                    LOGP("query_useful_information_for_ui_tool error\n");
                    ret = -1;
                }
            }


            strcpy((char *)output.response_buf, "+EAPS:");
            for (i = 0; i < attdet_out_data_buffer_size; i++) {
                if (i != 0) {
                    strcat((char *)output.response_buf, ",");
                }
                sprintf((char *)temp_string, "%lx", (uint32_t)attdet_out_data_buffer[i]);
                strcat((char *)output.response_buf, (char *)temp_string);
            }
            strcat((char *)output.response_buf, "\r\n");


            /*
                        if (command_content_value_array[0] == 0 && command_content_value_array[1] == 0x10 && command_content_value_array[2] == 0x100 && command_content_value_array[3] == 0x400) {
                            strcpy((char *)output.response_buf, "+ATTDET:1\r\n");
                        } else {
                            strcpy((char *)output.response_buf, "+ATTDET:0\r\n");
                        }
            */
            LOGI("%s\n", output.response_buf);


            LOGI("ATTDET ret = %d\n", ret);

            if (ret >= 0) {
                output.response_len = strlen((char *)output.response_buf);

                output.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append "OK" at the end of output buffer
            } else {
                // invalide parameter, return "ERROR"
                output.response_len = 0;
                output.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of output buffer
            };
            atci_send_response(&output);
            break;


        default :
            strcpy((char *)output.response_buf, "ERROR\r\n");
            output.response_len = strlen((char *)output.response_buf);
            atci_send_response(&output);
            break;
    }
    return ATCI_STATUS_OK;
}

// ATTTEST command handler
atci_status_t atci_cmd_hdlr_atttest(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};
    uint32_t command_content_value_array[ATTTEST_CONTENT_MAX_SIZE] = {0};
    AT_command_error_check_t AT_command_error_check = AT_COMMAND_NO_ERROR;
    atttest_structure_check_t atttest_structure_check = ATTTEST_STRUCUTRE_CHECK_NO_ERROR;
    int ret = 0;
    uint8_t temp_string[8]; // 0xffffffff  -> eight character
    uint32_t attdet_out_data_buffer[ATTDET_CONTENT_MAX_SIZE] = {0};
    int16_t attdet_out_data_buffer_size = 0;

    LOGI("ATTTEST_command_handler_function\n");

    output.response_flag = 0; // Command Execute Finish.

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+ATTTEST=?
            LOGI("user ask how to use ATTTEST command.\n");
            strcpy((char *)output.response_buf, "+ATTTEST:TO DO\r\nOK\r\n");
            output.response_len = strlen((char *)output.response_buf);
            atci_send_response(&output);
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+ATTTEST=<p1>  the handler need to parse the parameters
            LOGI("ATTDET Executing...\n");
            //parsing the parameter

            AT_command_error_check = segment_AT_command_to_int_array(command_content_value_array, ATTTEST_CONTENT_MAX_SIZE, ATTTEST_CONTENT_MIN_SIZE, parse_cmd);
            if (AT_command_error_check != AT_COMMAND_NO_ERROR) {
                LOGI("AT_command check error: %d.\n", AT_command_error_check);
                ret = -1;
            } else { // AT command correct
                atttest_structure_check = atttest_tuning_outcome_test(attdet_out_data_buffer, &attdet_out_data_buffer_size, command_content_value_array, (int)sizeof(command_content_value_array));
                if (atttest_structure_check != ATTTEST_STRUCUTRE_CHECK_NO_ERROR) {
                    LOGI("atttest structure check error: %d.\n", atttest_structure_check);
                    ret = -1;
                }
            }

            if (attdet_out_data_buffer_size != 0) {
                strcpy((char *)output.response_buf, "+ATTTEST:");
                int16_t i;
                for (i = 0; i < attdet_out_data_buffer_size; i++) {
                    if (i != 0) {
                        strcat((char *)output.response_buf, ",");
                    }
                    sprintf((char *)temp_string, "%lx", (uint32_t)attdet_out_data_buffer[i]);
                    strcat((char *)output.response_buf, (char *)temp_string);
                }
                strcat((char *)output.response_buf, "\r\n");

                LOGI("%s\n", output.response_buf);
            }

            LOGI("ATTTEST ret = %d\n", ret);


            if (ret >= 0) {
                //output.response_len = strlen(output.response_buf);
                //output.response_len = 0;
                output.response_len = strlen((char *)output.response_buf);
                output.response_flag |= ATCI_RESPONSE_FLAG_APPEND_OK;  // ATCI will help append "OK" at the end of output buffer
                //printf("ATTTEST OK output.response_len = %d\r\n", output.response_len);
            } else {
                // invalide parameter, return "ERROR"
                output.response_len = 0;
                output.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;  // ATCI will help append "ERROR" at the end of output buffer
                printf("ATTTEST ERROR output.response_len = %d\r\n", output.response_len);
            }

            atci_send_response(&output);
            break;


        default :
            strcpy((char *)output.response_buf, "ERROR\r\n");
            output.response_len = strlen((char *)output.response_buf);
            atci_send_response(&output);
            break;
    }
    return ATCI_STATUS_OK;
}

