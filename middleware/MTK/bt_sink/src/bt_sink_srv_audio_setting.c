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

#include "bt_sink_srv_audio_setting.h"

#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__

#include "stdint.h"
#include "audio_nvdm.h"
#include "audio_coefficient.h"
#include "hal_audio.h"
#include "bt_hfp.h"
#include "bt_sink_srv_utils.h"
#include "audio_dsp_fd216_db_to_gain_value_mapping_table.h"
#include "FreeRTOS.h"
#include "hal_audio_enhancement.h"

#if PRODUCT_VERSION == 2533
/* MT2533 */
#define SPEECH_MODE_HEADSET_HEADSET_MIC             (0)
/* external dsp*/
#define SPEECH_MODE_HEADSET_DUAL_DIGITAL_MIC        (0)
#define SPEECH_MODE_HEADSET_SINGLE_DIGITAL_MIC      (0)
#else
/* MT2523 */
#define SPEECH_MODE_HEADSET_HEADSET_MIC             (0)
#define SPEECH_MODE_HANDSFREE_MAIN_MIC              (1)
#define SPEECH_MODE_HEADSET_DIGITAL_MIC             (2)
#endif


#define AUDIO_SETTING_ERR_SUCCESS_7TH            (7)
#define AUDIO_SETTING_ERR_SUCCESS_6TH            (6)
#define AUDIO_SETTING_ERR_SUCCESS_5TH            (5)
#define AUDIO_SETTING_ERR_SUCCESS_4TH            (4)
#define AUDIO_SETTING_ERR_SUCCESS_3RD            (3)
#define AUDIO_SETTING_ERR_SUCCESS_2ND            (2)
#define AUDIO_SETTING_ERR_SUCCESS_1ST            (1)
#define AUDIO_SETTING_ERR_SUCCESS_OK             (0)
#define AUDIO_SETTING_ERR_FAIL_1ST               (-1)
#define AUDIO_SETTING_ERR_FAIL_2ND               (-2)
#define AUDIO_SETTING_ERR_FAIL_3RD               (-3)
#define AUDIO_SETTING_ERR_FAIL_4TH               (-4)
#define AUDIO_SETTING_ERR_FAIL_5TH               (-5)
#define AUDIO_SETTING_ERR_FAIL_6TH               (-6)
#define AUDIO_SETTING_ERR_FAIL_7TH               (-7)

/* Audio vol gain table--A2DP, Local player(MP3 & PCM) */
static uint32_t audio_vol_out_table_handsfree[BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV][2] = {{0}};

static uint32_t audio_vol_out_table_headset[BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV][2] = {{0}};


/* voice vol gain table */

/* nb vol gain (CVSD) */
#if PRODUCT_VERSION == 2533
/* SPEECH_MODE_HEADSET_HEADSET_MIC */
static uint32_t voice_nb_vol_out_table_headset_headset_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV][2] = {{0}};

static uint32_t voice_nb_vol_in_table_headset_headset_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV][2] = {{0}};

/* SPEECH_MODE_HEADSET_DUAL_DIGITAL_MIC extern dsp for 2533 */

static uint32_t voice_nb_vol_out_table_headset_dual_digital_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV][2] = {{0}};

static uint32_t voice_nb_vol_in_table_headset_dual_digital_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV][2] = {{0}};
#else
/* SPEECH_MODE_HEADSET_HEADSET_MIC */
static uint32_t voice_nb_vol_out_table_headset_headset_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV][2] = {{0}};

static uint32_t voice_nb_vol_in_table_headset_headset_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV][2] = {{0}};

/* SPEECH_MODE_HANDSFREE_MAIN_MIC */
static uint32_t voice_nb_vol_out_table_handsfree_main_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV][2] = {{0}};

static uint32_t voice_nb_vol_in_table_handsfree_main_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV][2] = {{0}};

/* SPEECH_MODE_HEADSET_DIGITAL_MIC */
static uint32_t voice_nb_vol_out_table_headset_digital_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV][2] = {{0}};

static uint32_t voice_nb_vol_in_table_headset_digital_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV][2] = {{0}};
#endif


/* voice vol gain table */

/* nb vol gain (MSBC) */
#if PRODUCT_VERSION == 2533
/* SPEECH_MODE_HEADSET_HEADSET_MIC */
static uint32_t voice_wb_vol_out_table_headset_headset_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV][2] = {{0}};

static uint32_t voice_wb_vol_in_table_headset_headset_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV][2] = {{0}};

/* SPEECH_MODE_HEADSET_DUAL_DIGITAL_MIC extern dsp for 2533 */

static uint32_t voice_wb_vol_out_table_headset_dual_digital_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV][2] = {{0}};

static uint32_t voice_wb_vol_in_table_headset_dual_digital_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV][2] = {{0}};
#else
/* SPEECH_MODE_HEADSET_HEADSET_MIC */
static uint32_t voice_wb_vol_out_table_headset_headset_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV][2] = {{0}};

static uint32_t voice_wb_vol_in_table_headset_headset_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV][2] = {{0}};

/* SPEECH_MODE_HANDSFREE_MAIN_MIC */
static uint32_t voice_wb_vol_out_table_handsfree_main_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV][2] = {{0}};

static uint32_t voice_wb_vol_in_table_handsfree_main_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV][2] = {{0}};

/* SPEECH_MODE_HEADSET_DIGITAL_MIC */
static uint32_t voice_wb_vol_out_table_headset_digital_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV][2] = {{0}};

static uint32_t voice_wb_vol_in_table_headset_digital_mic[BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV][2] = {{0}};
#endif




/* static function */
static bt_sink_srv_audio_setting_context_t g_bt_sink_srv_audio_setting_ctx;

static void bt_sink_srv_audio_setting_update_callback(void *data);

static void bt_sink_srv_audio_setting_gen_vol_table(void);

static void bt_sink_srv_audio_setting_gen_audio_vol_table(audio_eaps_t *am_speech_eaps);

static void bt_sink_srv_audio_setting_gen_voice_vol_table(audio_eaps_t *am_speech_eaps);

static void bt_sink_srv_audio_setting_audio_downlink_amp_db_transfer(uint16_t gain, uint32_t *digital, uint32_t *analog);

static void bt_sink_srv_audio_setting_voice_uplink_amp_db_transfer(uint16_t gain, uint32_t *digital, uint32_t *analog);

static void bt_sink_srv_audio_setting_voice_uplink_dmic_db_transfer(uint16_t gain, uint32_t *digital, uint32_t *analog);

static void bt_sink_srv_audio_setting_voice_downlink_amp_db_transfer(uint16_t gain, uint32_t *digital, uint32_t *analog);

static bt_sink_srv_audio_setting_context_t *bt_sink_srv_audio_setting_get_context(void);

static void bt_sink_srv_audio_setting_get_a2dp_vol(a2dp_vol_info_t *info,
            a2dp_vol_t *vol);

static void bt_sink_srv_audio_setting_get_hfp_vol(hfp_vol_info_t *info,
            hfp_vol_t *vol);

static void bt_sink_srv_audio_setting_get_pcm_vol(pcm_vol_info_t *info,
            pcm_vol_t *vol);

static void bt_sink_srv_audio_setting_get_mp3_vol(mp3_vol_info_t *info,
            mp3_vol_t *vol);


static bt_sink_srv_audio_setting_context_t *bt_sink_srv_audio_setting_get_context(void)
{
    return &g_bt_sink_srv_audio_setting_ctx;
}


static void bt_sink_srv_audio_setting_update_callback(void *data)
{
    bt_sink_srv_report("[Sink][Setting]update_callback--data: 0x%x\n", data);

    /* re-gen vol table */
    bt_sink_srv_audio_setting_gen_vol_table();
}


static void bt_sink_srv_audio_setting_audio_downlink_amp_db_transfer(uint16_t gain, uint32_t *digital, uint32_t *analog)
{
    int32_t stream_out_db = 0;
    int32_t analog_gain_in_db = 0, digital_gain_in_db = 0;
    int16_t temp_int16 = 0;

    temp_int16 = (int16_t)gain;
    stream_out_db = (int32_t)temp_int16;
    audio_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(stream_out_db, &analog_gain_in_db, &digital_gain_in_db);
    digital_gain_in_db *= 100;
    analog_gain_in_db *= 100;
    *digital = (uint32_t)digital_gain_in_db;
    *analog = (uint32_t)analog_gain_in_db;
}


static void bt_sink_srv_audio_setting_voice_uplink_amp_db_transfer(uint16_t gain, uint32_t *digital, uint32_t *analog)
{
    int32_t stream_in_db = 0;
    int32_t analog_gain_in_db = 0, digital_gain_in_db = 0;
    int16_t temp_int16 = 0;

    temp_int16 = (int16_t)gain;
    stream_in_db = (int32_t)temp_int16;
    voice_uplink_amp_db_transfer_to_analog_gain_and_digital_gain(stream_in_db, &analog_gain_in_db, &digital_gain_in_db);
    digital_gain_in_db *= 100;
    analog_gain_in_db *= 100;
    *digital = (uint32_t)digital_gain_in_db;
    *analog = (uint32_t)analog_gain_in_db;
}


static void bt_sink_srv_audio_setting_voice_uplink_dmic_db_transfer(uint16_t gain, uint32_t *digital, uint32_t *analog)
{
    int32_t stream_in_db = 0;
    int32_t analog_gain_in_db = 0, digital_gain_in_db = 0;
    int16_t temp_int16 = 0;

    temp_int16 = (int16_t)gain;
    stream_in_db = (int32_t)temp_int16;
    voice_uplink_dmic_db_transfer_to_analog_gain_and_digital_gain(stream_in_db, &analog_gain_in_db, &digital_gain_in_db);
    digital_gain_in_db *= 100;
    analog_gain_in_db *= 100;
    *digital = (uint32_t)digital_gain_in_db;
    *analog = (uint32_t)analog_gain_in_db;
}


static void bt_sink_srv_audio_setting_voice_downlink_amp_db_transfer(uint16_t gain, uint32_t *digital, uint32_t *analog)
{
    int32_t stream_out_db = 0;
    int32_t analog_gain_in_db = 0, digital_gain_in_db = 0;
    int16_t temp_int16 = 0;

    temp_int16 = (int16_t)gain;
    stream_out_db = (int32_t)temp_int16;
    voice_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(stream_out_db, &analog_gain_in_db, &digital_gain_in_db);
    digital_gain_in_db *= 100;
    analog_gain_in_db *= 100;
    *digital = (uint32_t)digital_gain_in_db;
    *analog = (uint32_t)analog_gain_in_db;
}


static void bt_sink_srv_audio_setting_gen_audio_vol_table(audio_eaps_t *am_speech_eaps)
{
    int32_t i = 0, k = 0;
    uint16_t gain = 0;
    //uint8_t mode = 0;
    uint32_t digital = 0, analog = 0;

    bt_sink_srv_assert(am_speech_eaps);

#if PRODUCT_VERSION == 2533
    k = BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV;
    for (i = 0; i < k; ++i) {
        gain = am_speech_eaps->audio_parameter.handsfree_headset_stream_out_gain.headset_gain[i];
        bt_sink_srv_audio_setting_audio_downlink_amp_db_transfer(gain, &digital, &analog);
        audio_vol_out_table_headset[i][0] = digital;
        audio_vol_out_table_headset[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_audio_vol(headset)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    k = BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV;
    for (i = 0; i < k; ++i) {
        gain = am_speech_eaps->audio_parameter.handsfree_headset_stream_out_gain.handsfree_gain[i];
        bt_sink_srv_audio_setting_voice_downlink_amp_db_transfer(gain, &digital, &analog);
        audio_vol_out_table_handsfree[i][0] = digital;
        audio_vol_out_table_handsfree[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_audio_vol(handsfree)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }
#else
    /* 2523 */
    k = BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV;
    for (i = 0; i < k; ++i) {
        gain = am_speech_eaps->audio_parameter.audio_handsfree_headset_gain_parameter.headset_gain.stream_out_gain[i];
        bt_sink_srv_audio_setting_audio_downlink_amp_db_transfer(gain, &digital, &analog);
        audio_vol_out_table_headset[i][0] = digital;
        audio_vol_out_table_headset[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_audio_vol(headset)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    k = BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV;
    for (i = 0; i < k; ++i) {
        gain = am_speech_eaps->audio_parameter.audio_handsfree_headset_gain_parameter.handsfree_gain.stream_out_gain[i];
        bt_sink_srv_audio_setting_voice_downlink_amp_db_transfer(gain, &digital, &analog);
        audio_vol_out_table_handsfree[i][0] = digital;
        audio_vol_out_table_handsfree[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_audio_vol(handsfree)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }
#endif /* 2533 */
}


static void bt_sink_srv_audio_setting_gen_voice_vol_table(audio_eaps_t *am_speech_eaps)
{
    int32_t i = 0, k = 0, l = 0;
    uint16_t gain = 0;
    uint8_t mode = 0;
    uint32_t digital = 0, analog = 0;

    bt_sink_srv_assert(am_speech_eaps);

    /* CVSD */
    bt_sink_srv_report("[Sink][Setting]CVSD start gen_voice_vol\n");
#if PRODUCT_VERSION == 2533
    bt_sink_srv_report("[Sink][Setting]SPEECH_MODE_HEADSET_HEADSET_MIC gen\n");
    mode = SPEECH_MODE_HEADSET_HEADSET_MIC;
    l = BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV;
    for (i = 0; i < l; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_gain.stream_in_gain[i];
        bt_sink_srv_audio_setting_voice_uplink_amp_db_transfer(gain, &digital, &analog);
        voice_nb_vol_in_table_headset_headset_mic[i][0] = digital;
        voice_nb_vol_in_table_headset_headset_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(in)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    k = BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV;
    for (i = 0; i < k; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter[0].voice_nb_gain.stream_out_gain[i];
        bt_sink_srv_audio_setting_audio_downlink_amp_db_transfer(gain, &digital, &analog);
        voice_nb_vol_out_table_headset_headset_mic[i][0] = digital;
        voice_nb_vol_out_table_headset_headset_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(out)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    bt_sink_srv_report("[Sink][Setting]SPEECH_MODE_HEADSET_DUAL_DIGITAL_MIC gen\n");
    mode = SPEECH_MODE_HEADSET_DUAL_DIGITAL_MIC;
    l = BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV;
    for (i = 0; i < l; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[mode].voice_nb_gain.stream_in_gain[i];
        bt_sink_srv_audio_setting_voice_uplink_dmic_db_transfer(gain, &digital, &analog);
        voice_nb_vol_in_table_headset_dual_digital_mic[i][0] = digital;
        voice_nb_vol_in_table_headset_dual_digital_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(in)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    k = BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV;
    for (i = 0; i < k; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[mode].voice_nb_gain.stream_out_gain[i];
        bt_sink_srv_audio_setting_audio_downlink_amp_db_transfer(gain, &digital, &analog);
        voice_nb_vol_out_table_headset_dual_digital_mic[i][0] = digital;
        voice_nb_vol_out_table_headset_dual_digital_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(out)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }
#else
    bt_sink_srv_report("[Sink][Setting]SPEECH_MODE_HEADSET_HEADSET_MIC gen\n");
    mode = SPEECH_MODE_HEADSET_HEADSET_MIC;
    l = BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV;
    for (i = 0; i < l; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_nb_parameter[mode].voice_nb_gain.stream_in_gain[i];
        bt_sink_srv_audio_setting_voice_uplink_amp_db_transfer(gain, &digital, &analog);
        voice_nb_vol_in_table_headset_headset_mic[i][0] = digital;
        voice_nb_vol_in_table_headset_headset_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(in)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    k = BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV;
    for (i = 0; i < k; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_nb_parameter[mode].voice_nb_gain.stream_out_gain[i];
        bt_sink_srv_audio_setting_audio_downlink_amp_db_transfer(gain, &digital, &analog);
        voice_nb_vol_out_table_headset_headset_mic[i][0] = digital;
        voice_nb_vol_out_table_headset_headset_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(out)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    bt_sink_srv_report("[Sink][Setting]SPEECH_MODE_HANDSFREE_MAIN_MIC gen\n");
    mode = SPEECH_MODE_HANDSFREE_MAIN_MIC;
    l = BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV;
    for (i = 0; i < l; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_nb_parameter[mode].voice_nb_gain.stream_in_gain[i];
        bt_sink_srv_audio_setting_voice_uplink_amp_db_transfer(gain, &digital, &analog);
        voice_nb_vol_in_table_handsfree_main_mic[i][0] = digital;
        voice_nb_vol_in_table_handsfree_main_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(in)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    k = BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV;
    for (i = 0; i < k; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_nb_parameter[mode].voice_nb_gain.stream_out_gain[i];
        bt_sink_srv_audio_setting_voice_downlink_amp_db_transfer(gain, &digital, &analog);
        voice_nb_vol_out_table_handsfree_main_mic[i][0] = digital;
        voice_nb_vol_out_table_handsfree_main_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(out)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    bt_sink_srv_report("[Sink][Setting]SPEECH_MODE_HEADSET_DIGITAL_MIC gen\n");
    mode = SPEECH_MODE_HEADSET_DIGITAL_MIC;
    l = BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV;
    for (i = 0; i < l; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_nb_parameter[mode].voice_nb_gain.stream_in_gain[i];
        bt_sink_srv_audio_setting_voice_uplink_dmic_db_transfer(gain, &digital, &analog);
        voice_nb_vol_in_table_headset_digital_mic[i][0] = digital;
        voice_nb_vol_in_table_headset_digital_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(in)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    k = BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV;
    for (i = 0; i < k; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_nb_parameter[mode].voice_nb_gain.stream_out_gain[i];
        bt_sink_srv_audio_setting_audio_downlink_amp_db_transfer(gain, &digital, &analog);
        voice_nb_vol_out_table_headset_digital_mic[i][0] = digital;
        voice_nb_vol_out_table_headset_digital_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(out)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }
#endif /* 2533 */

    /* MSBC */
    bt_sink_srv_report("[Sink][Setting]MSBC start gen_voice_vol\n");
#if PRODUCT_VERSION == 2533
    bt_sink_srv_report("[Sink][Setting]SPEECH_MODE_HEADSET_HEADSET_MIC gen\n");
    mode = SPEECH_MODE_HEADSET_HEADSET_MIC;
    l = BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV;
    for (i = 0; i < l; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_gain.stream_in_gain[i];
        bt_sink_srv_audio_setting_voice_uplink_amp_db_transfer(gain, &digital, &analog);
        voice_wb_vol_in_table_headset_headset_mic[i][0] = digital;
        voice_wb_vol_in_table_headset_headset_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(in)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    k = BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV;
    for (i = 0; i < k; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter[0].voice_wb_gain.stream_out_gain[i];
        bt_sink_srv_audio_setting_audio_downlink_amp_db_transfer(gain, &digital, &analog);
        voice_wb_vol_out_table_headset_headset_mic[i][0] = digital;
        voice_wb_vol_out_table_headset_headset_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(out)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    bt_sink_srv_report("[Sink][Setting]SPEECH_MODE_HEADSET_DUAL_DIGITAL_MIC gen\n");
    mode = SPEECH_MODE_HEADSET_DUAL_DIGITAL_MIC;
    l = BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV;
    for (i = 0; i < l; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[mode].voice_wb_gain.stream_in_gain[i];
        bt_sink_srv_audio_setting_voice_uplink_dmic_db_transfer(gain, &digital, &analog);
        voice_wb_vol_in_table_headset_dual_digital_mic[i][0] = digital;
        voice_wb_vol_in_table_headset_dual_digital_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(in)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    k = BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV;
    for (i = 0; i < k; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[mode].voice_wb_gain.stream_out_gain[i];
        bt_sink_srv_audio_setting_audio_downlink_amp_db_transfer(gain, &digital, &analog);
        voice_wb_vol_out_table_headset_dual_digital_mic[i][0] = digital;
        voice_wb_vol_out_table_headset_dual_digital_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(out)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }
#else
    bt_sink_srv_report("[Sink][Setting]SPEECH_MODE_HEADSET_HEADSET_MIC gen\n");
    mode = SPEECH_MODE_HEADSET_HEADSET_MIC;
    l = BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV;
    for (i = 0; i < l; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_wb_parameter[mode].voice_wb_gain.stream_in_gain[i];
        bt_sink_srv_audio_setting_voice_uplink_amp_db_transfer(gain, &digital, &analog);
        voice_wb_vol_in_table_headset_headset_mic[i][0] = digital;
        voice_wb_vol_in_table_headset_headset_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(in)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    k = BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV;
    for (i = 0; i < k; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_wb_parameter[mode].voice_wb_gain.stream_out_gain[i];
        bt_sink_srv_audio_setting_audio_downlink_amp_db_transfer(gain, &digital, &analog);
        voice_wb_vol_out_table_headset_headset_mic[i][0] = digital;
        voice_wb_vol_out_table_headset_headset_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(out)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    bt_sink_srv_report("[Sink][Setting]SPEECH_MODE_HANDSFREE_MAIN_MIC gen\n");
    mode = SPEECH_MODE_HANDSFREE_MAIN_MIC;
    l = BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV;
    for (i = 0; i < l; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_wb_parameter[mode].voice_wb_gain.stream_in_gain[i];
        bt_sink_srv_audio_setting_voice_uplink_amp_db_transfer(gain, &digital, &analog);
        voice_wb_vol_in_table_handsfree_main_mic[i][0] = digital;
        voice_wb_vol_in_table_handsfree_main_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(in)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    k = BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV;
    for (i = 0; i < k; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_wb_parameter[mode].voice_wb_gain.stream_out_gain[i];
        bt_sink_srv_audio_setting_voice_downlink_amp_db_transfer(gain, &digital, &analog);
        voice_wb_vol_out_table_handsfree_main_mic[i][0] = digital;
        voice_wb_vol_out_table_handsfree_main_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(out)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    bt_sink_srv_report("[Sink][Setting]SPEECH_MODE_HEADSET_DIGITAL_MIC gen\n");
    mode = SPEECH_MODE_HEADSET_DIGITAL_MIC;
    l = BT_SINK_SRV_AUDIO_SETTING_STREAM_IN_GAIN_LEV;
    for (i = 0; i < l; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_wb_parameter[mode].voice_wb_gain.stream_in_gain[i];
        bt_sink_srv_audio_setting_voice_uplink_dmic_db_transfer(gain, &digital, &analog);
        voice_wb_vol_in_table_headset_digital_mic[i][0] = digital;
        voice_wb_vol_in_table_headset_digital_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(in)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }

    k = BT_SINK_SRV_AUDIO_SETTING_STREAM_OUT_GAIN_LEV;
    for (i = 0; i < k; ++i) {
        gain = am_speech_eaps->voice_parameter.voice_wb_parameter[mode].voice_wb_gain.stream_out_gain[i];
        bt_sink_srv_audio_setting_audio_downlink_amp_db_transfer(gain, &digital, &analog);
        voice_wb_vol_out_table_headset_digital_mic[i][0] = digital;
        voice_wb_vol_out_table_headset_digital_mic[i][1] = analog;
        bt_sink_srv_report("[Sink][Setting]gen_voice_vol(out)--i: %d, gain: 0x%x, digital: 0x%x, analog: 0x%x\n", i, gain, digital, analog);
    }
#endif /* 2533 */

}


static void bt_sink_srv_audio_setting_gen_vol_table(void)
{
    audio_eaps_t *am_speech_eaps = NULL;
    bt_sink_srv_audio_setting_context_t *setting_ctx = NULL;

    setting_ctx = bt_sink_srv_audio_setting_get_context();
    am_speech_eaps = (audio_eaps_t *)pvPortMalloc(sizeof(audio_eaps_t));

    if (am_speech_eaps) {
        audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);
    } else {
        bt_sink_srv_report("[Sink][Setting]eaps no memory\n");
        return;
    }

    setting_ctx->flag &= ~(BT_SINK_SRV_AUDIO_SETTING_FALG_GEN_VOL_READY);

    if (am_speech_eaps) {
        /* gen audio vol tab */
        bt_sink_srv_audio_setting_gen_audio_vol_table(am_speech_eaps);

        /* gen voice vol tab */
        bt_sink_srv_audio_setting_gen_voice_vol_table(am_speech_eaps);
    }

    if (am_speech_eaps != NULL) {
        vPortFree(am_speech_eaps);
    }

    setting_ctx->flag |= BT_SINK_SRV_AUDIO_SETTING_FALG_GEN_VOL_READY;
}


void bt_sink_srv_audio_setting_init(void)
{
    bt_sink_srv_report("[Sink][Setting]init--ver: %d\n", PRODUCT_VERSION);

    memset(&g_bt_sink_srv_audio_setting_ctx, 0x00, sizeof(bt_sink_srv_audio_setting_context_t));

    bt_sink_srv_audio_setting_gen_vol_table();

    audio_nvdm_register_eaps_is_changed_callback((audio_nvdm_callback_id *)&g_bt_sink_srv_audio_setting_ctx, bt_sink_srv_audio_setting_update_callback, &g_bt_sink_srv_audio_setting_ctx);
    bt_sink_srv_music_report("[Sink][Setting]eaps_reg--cid: 0x%08x", &g_bt_sink_srv_audio_setting_ctx);
}


static void bt_sink_srv_audio_setting_get_a2dp_vol(a2dp_vol_info_t *info,
            a2dp_vol_t *vol)
{
    device_t dev = info->dev;
    uint8_t lev = info->lev;
    int32_t ret = AUDIO_SETTING_ERR_SUCCESS_OK;
    uint32_t digital = 0;
    uint32_t analog = 0;

    if (HAL_AUDIO_DEVICE_HEADSET == dev) {
        digital = audio_vol_out_table_headset[lev][0];
        analog = audio_vol_out_table_headset[lev][1];
    } else if ((HAL_AUDIO_DEVICE_HANDS_FREE_STEREO == dev) ||
                (HAL_AUDIO_DEVICE_HANDSET == dev)) {
        digital = audio_vol_out_table_handsfree[lev][0];
        analog = audio_vol_out_table_handsfree[lev][1];
    } else {
        ret = AUDIO_SETTING_ERR_FAIL_1ST;
    }

    vol->vol.digital = digital;
    vol->vol.analog = analog;
    bt_sink_srv_report("[Sink][Setting]get_a2dp_vol--dev: 0x%x, lev: %d, ret: %d, d: 0x%x, a: 0x%x\n",
        dev, lev, ret, digital, analog);
}


static void bt_sink_srv_audio_setting_get_hfp_vol(hfp_vol_info_t *info,
            hfp_vol_t *vol)
{
    device_t dev_out = info->dev_out;
    device_t dev_in = info->dev_in;
    uint8_t lev_in = info->lev_in;
    uint8_t lev_out = info->lev_out;
    uint8_t codec = info->codec;
    int32_t ret = AUDIO_SETTING_ERR_SUCCESS_OK;
    uint32_t digital = 0;
    uint32_t analog = 0;
    uint8_t mode = 0;


    if (BT_HFP_CODEC_TYPE_CVSD == codec) {
#if PRODUCT_VERSION == 2533
    if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
        HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC == dev_in) {
        mode = SPEECH_MODE_HEADSET_DUAL_DIGITAL_MIC;
        /* vol in */
        digital = voice_nb_vol_in_table_headset_dual_digital_mic[lev_in][0];
        analog = voice_nb_vol_in_table_headset_dual_digital_mic[lev_in][1];
        vol->vol_in.digital = digital;
        vol->vol_in.analog = analog;
        /* vol out */
        digital = voice_nb_vol_out_table_headset_dual_digital_mic[lev_out][0];
        analog = voice_nb_vol_out_table_headset_dual_digital_mic[lev_out][1];
        vol->vol_out.digital = digital;
        vol->vol_out.analog = analog;
        ret = AUDIO_SETTING_ERR_SUCCESS_1ST;
    } else if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
        HAL_AUDIO_DEVICE_HEADSET_MIC == dev_in) {
        mode = SPEECH_MODE_HEADSET_HEADSET_MIC;
        /* vol in */
        digital = voice_nb_vol_in_table_headset_headset_mic[lev_in][0];
        analog = voice_nb_vol_in_table_headset_headset_mic[lev_in][1];
        vol->vol_in.digital = digital;
        vol->vol_in.analog = analog;
        /* vol out */
        digital = voice_nb_vol_out_table_headset_headset_mic[lev_out][0];
        analog = voice_nb_vol_out_table_headset_headset_mic[lev_out][1];
        vol->vol_out.digital = digital;
        vol->vol_out.analog = analog;
        ret = AUDIO_SETTING_ERR_SUCCESS_2ND;
    } else if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
        HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC == dev_in) {
        mode = SPEECH_MODE_HEADSET_SINGLE_DIGITAL_MIC;
        /* vol in */
        digital = voice_nb_vol_in_table_headset_dual_digital_mic[lev_in][0];
        analog = voice_nb_vol_in_table_headset_dual_digital_mic[lev_in][1];
        vol->vol_in.digital = digital;
        vol->vol_in.analog = analog;
        /* vol out */
        digital = voice_nb_vol_out_table_headset_dual_digital_mic[lev_out][0];
        analog = voice_nb_vol_out_table_headset_dual_digital_mic[lev_out][1];
        vol->vol_out.digital = digital;
        vol->vol_out.analog = analog;
        ret = AUDIO_SETTING_ERR_SUCCESS_3RD;
    } else {
        ret = AUDIO_SETTING_ERR_FAIL_1ST;
    }
#else
    if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
        ((HAL_AUDIO_DEVICE_HEADSET_MIC == dev_in) || (HAL_AUDIO_DEVICE_MAIN_MIC == dev_in))) {
        mode = SPEECH_MODE_HEADSET_HEADSET_MIC;
        /* vol in */
        digital = voice_nb_vol_in_table_headset_headset_mic[lev_in][0];
        analog = voice_nb_vol_in_table_headset_headset_mic[lev_in][1];
        vol->vol_in.digital = digital;
        vol->vol_in.analog = analog;
        /* vol out */
        digital = voice_nb_vol_out_table_headset_headset_mic[lev_out][0];
        analog = voice_nb_vol_out_table_headset_headset_mic[lev_out][1];
        vol->vol_out.digital = digital;
        vol->vol_out.analog = analog;
        ret = AUDIO_SETTING_ERR_SUCCESS_4TH;
    } else if (HAL_AUDIO_DEVICE_HANDS_FREE_STEREO == dev_out &&
               HAL_AUDIO_DEVICE_MAIN_MIC == dev_in) {
        mode = SPEECH_MODE_HANDSFREE_MAIN_MIC;
        /* vol in */
        digital = voice_nb_vol_in_table_handsfree_main_mic[lev_in][0];
        analog = voice_nb_vol_in_table_handsfree_main_mic[lev_in][1];
        vol->vol_in.digital = digital;
        vol->vol_in.analog = analog;
        /* vol out */
        digital = voice_nb_vol_out_table_handsfree_main_mic[lev_out][0];
        analog = voice_nb_vol_out_table_handsfree_main_mic[lev_out][1];
        vol->vol_out.digital = digital;
        vol->vol_out.analog = analog;
        ret = AUDIO_SETTING_ERR_SUCCESS_5TH;
    } else if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
               HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC == dev_in) {
        mode = SPEECH_MODE_HEADSET_DIGITAL_MIC;
        /* vol in */
        digital = voice_nb_vol_in_table_headset_digital_mic[lev_in][0];
        analog = voice_nb_vol_in_table_headset_digital_mic[lev_in][1];
        vol->vol_in.digital = digital;
        vol->vol_in.analog = analog;
        /* vol out */
        digital = voice_nb_vol_out_table_headset_digital_mic[lev_out][0];
        analog = voice_nb_vol_out_table_headset_digital_mic[lev_out][1];
        vol->vol_out.digital = digital;
        vol->vol_out.analog = analog;
        ret = AUDIO_SETTING_ERR_SUCCESS_6TH;
    } else {
        ret = AUDIO_SETTING_ERR_FAIL_2ND;
    }
#endif
    } else if (BT_HFP_CODEC_TYPE_MSBC == codec) {
#if PRODUCT_VERSION == 2533
    if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
        HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC == dev_in) {
        mode = SPEECH_MODE_HEADSET_DUAL_DIGITAL_MIC;
        /* vol in */
        digital = voice_wb_vol_in_table_headset_dual_digital_mic[lev_in][0];
        analog = voice_wb_vol_in_table_headset_dual_digital_mic[lev_in][1];
        vol->vol_in.digital = digital;
        vol->vol_in.analog = analog;
        /* vol out */
        digital = voice_wb_vol_out_table_headset_dual_digital_mic[lev_out][0];
        analog = voice_wb_vol_out_table_headset_dual_digital_mic[lev_out][1];
        vol->vol_out.digital = digital;
        vol->vol_out.analog = analog;
        ret = AUDIO_SETTING_ERR_SUCCESS_1ST;
    } else if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
        HAL_AUDIO_DEVICE_HEADSET_MIC == dev_in) {
        mode = SPEECH_MODE_HEADSET_HEADSET_MIC;
        /* vol in */
        digital = voice_wb_vol_in_table_headset_headset_mic[lev_in][0];
        analog = voice_wb_vol_in_table_headset_headset_mic[lev_in][1];
        vol->vol_in.digital = digital;
        vol->vol_in.analog = analog;
        /* vol out */
        digital = voice_wb_vol_out_table_headset_headset_mic[lev_out][0];
        analog = voice_wb_vol_out_table_headset_headset_mic[lev_out][1];
        vol->vol_out.digital = digital;
        vol->vol_out.analog = analog;
        ret = AUDIO_SETTING_ERR_SUCCESS_2ND;
    } else if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
        HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC == dev_in) {
        mode = SPEECH_MODE_HEADSET_SINGLE_DIGITAL_MIC;
        /* vol in */
        digital = voice_wb_vol_in_table_headset_dual_digital_mic[lev_in][0];
        analog = voice_wb_vol_in_table_headset_dual_digital_mic[lev_in][1];
        vol->vol_in.digital = digital;
        vol->vol_in.analog = analog;
        /* vol out */
        digital = voice_wb_vol_out_table_headset_dual_digital_mic[lev_out][0];
        analog = voice_wb_vol_out_table_headset_dual_digital_mic[lev_out][1];
        vol->vol_out.digital = digital;
        vol->vol_out.analog = analog;
        ret = AUDIO_SETTING_ERR_SUCCESS_3RD;
    } else {
        ret = AUDIO_SETTING_ERR_FAIL_1ST;
    }
#else
    if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
        ((HAL_AUDIO_DEVICE_HEADSET_MIC == dev_in) || (HAL_AUDIO_DEVICE_MAIN_MIC == dev_in))) {
        mode = SPEECH_MODE_HEADSET_HEADSET_MIC;
        /* vol in */
        digital = voice_wb_vol_in_table_headset_headset_mic[lev_in][0];
        analog = voice_wb_vol_in_table_headset_headset_mic[lev_in][1];
        vol->vol_in.digital = digital;
        vol->vol_in.analog = analog;
        /* vol out */
        digital = voice_wb_vol_out_table_headset_headset_mic[lev_out][0];
        analog = voice_wb_vol_out_table_headset_headset_mic[lev_out][1];
        vol->vol_out.digital = digital;
        vol->vol_out.analog = analog;
        ret = AUDIO_SETTING_ERR_SUCCESS_4TH;
    } else if (HAL_AUDIO_DEVICE_HANDS_FREE_STEREO == dev_out &&
               HAL_AUDIO_DEVICE_MAIN_MIC == dev_in) {
        mode = SPEECH_MODE_HANDSFREE_MAIN_MIC;
        /* vol in */
        digital = voice_wb_vol_in_table_handsfree_main_mic[lev_in][0];
        analog = voice_wb_vol_in_table_handsfree_main_mic[lev_in][1];
        vol->vol_in.digital = digital;
        vol->vol_in.analog = analog;
        /* vol out */
        digital = voice_wb_vol_out_table_handsfree_main_mic[lev_out][0];
        analog = voice_wb_vol_out_table_handsfree_main_mic[lev_out][1];
        vol->vol_out.digital = digital;
        vol->vol_out.analog = analog;
        ret = AUDIO_SETTING_ERR_SUCCESS_5TH;
    } else if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
               HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC == dev_in) {
        mode = SPEECH_MODE_HEADSET_DIGITAL_MIC;
        /* vol in */
        digital = voice_wb_vol_in_table_headset_digital_mic[lev_in][0];
        analog = voice_wb_vol_in_table_headset_digital_mic[lev_in][1];
        vol->vol_in.digital = digital;
        vol->vol_in.analog = analog;
        /* vol out */
        digital = voice_wb_vol_out_table_headset_digital_mic[lev_out][0];
        analog = voice_wb_vol_out_table_headset_digital_mic[lev_out][1];
        vol->vol_out.digital = digital;
        vol->vol_out.analog = analog;
        ret = AUDIO_SETTING_ERR_SUCCESS_6TH;
    } else {
        ret = AUDIO_SETTING_ERR_FAIL_2ND;
    }
#endif
    } else {
        ret = AUDIO_SETTING_ERR_FAIL_7TH;
    }

    bt_sink_srv_report("[Sink][Setting]get_hfp_vol--dev_in: 0x%x, dev_out: 0x%x, lev_in: %d, lev_out: %d, codec: %d",
        info->dev_in, info->dev_out, info->lev_in, info->lev_out, codec);
    bt_sink_srv_report("[Sink][Setting]get_hfp_vol--ret: %d, mode: %d, d-in: 0x%x, a-in: 0x%x, d-out: 0x%x, a-out: 0x%x",
        ret, mode, vol->vol_in.digital, vol->vol_in.analog, vol->vol_out.digital, vol->vol_out.analog);
    bt_sink_srv_music_report("[Sink][Setting]get_hfp_vol--dev_in: 0x%x, dev_out: 0x%x, lev_in: %d, lev_out: %d, codec: %d",
        info->dev_in, info->dev_out, info->lev_in, info->lev_out, codec);
    bt_sink_srv_music_report("[Sink][Setting]get_hfp_vol--ret: %d, mode: %d, d-in: 0x%x, a-in: 0x%x, d-out: 0x%x, a-out: 0x%x",
        ret, mode, vol->vol_in.digital, vol->vol_in.analog, vol->vol_out.digital, vol->vol_out.analog);
}


static void bt_sink_srv_audio_setting_get_pcm_vol(pcm_vol_info_t *info,
            pcm_vol_t *vol)
{
    device_t dev = info->dev;
    uint8_t lev = info->lev;
    int32_t ret = AUDIO_SETTING_ERR_SUCCESS_OK;
    uint32_t digital = 0;
    uint32_t analog = 0;

    if (HAL_AUDIO_DEVICE_HEADSET == dev) {
        digital = audio_vol_out_table_headset[lev][0];
        analog = audio_vol_out_table_headset[lev][1];
    } else if (HAL_AUDIO_DEVICE_HANDS_FREE_STEREO == dev) {
        digital = audio_vol_out_table_handsfree[lev][0];
        analog = audio_vol_out_table_handsfree[lev][1];
    } else {
        ret = AUDIO_SETTING_ERR_FAIL_1ST;
    }

    vol->vol.digital = digital;
    vol->vol.analog = analog;
    bt_sink_srv_report("[Sink][Setting]get_pcm_vol--dev: 0x%x, lev: %d, ret: %d, d: 0x%x, a: 0x%x\n",
        dev, lev, ret, digital, analog);
}


static void bt_sink_srv_audio_setting_get_mp3_vol(mp3_vol_info_t *info,
            mp3_vol_t *vol)
{
    device_t dev = info->dev;
    uint8_t lev = info->lev;
    int32_t ret = AUDIO_SETTING_ERR_SUCCESS_OK;
    uint32_t digital = 0;
    uint32_t analog = 0;

    if (HAL_AUDIO_DEVICE_HEADSET == dev) {
        digital = audio_vol_out_table_headset[lev][0];
        analog = audio_vol_out_table_headset[lev][1];
    } else if (HAL_AUDIO_DEVICE_HANDS_FREE_STEREO == dev) {
        digital = audio_vol_out_table_handsfree[lev][0];
        analog = audio_vol_out_table_handsfree[lev][1];
    } else {
        ret = AUDIO_SETTING_ERR_FAIL_1ST;
    }

    vol->vol.digital = digital;
    vol->vol.analog = analog;
    bt_sink_srv_report("[Sink][Setting]get_mp3_vol--dev: 0x%x, lev: %d, ret: %d, d: 0x%x, a: 0x%x\n",
        dev, lev, ret, digital, analog);
}


int32_t bt_sink_srv_audio_setting_get_vol(bt_sink_srv_audio_setting_vol_info_t *vol_info,
            bt_sink_srv_audio_setting_vol_t *vol)
{
    bt_sink_srv_audio_setting_context_t *setting_ctx = NULL;
    vol_type_t type;

    setting_ctx = bt_sink_srv_audio_setting_get_context();
    type = vol_info->type;
    if (setting_ctx->flag & BT_SINK_SRV_AUDIO_SETTING_FALG_GEN_VOL_READY) {
        switch (type) {
            case VOL_A2DP: {
                vol->type = VOL_A2DP;
                bt_sink_srv_audio_setting_get_a2dp_vol(&(vol_info->vol_info.a2dp_vol_info), &(vol->vol.a2dp_vol));
                break;
            }

            case VOL_HFP: {
                vol->type = VOL_HFP;
                bt_sink_srv_audio_setting_get_hfp_vol(&(vol_info->vol_info.hfp_vol_info), &(vol->vol.hfp_vol));
                break;
            }

            case VOL_PCM: {
                vol->type = VOL_PCM;
                bt_sink_srv_audio_setting_get_pcm_vol(&(vol_info->vol_info.pcm_vol_info), &(vol->vol.pcm_vol));
                break;
            }

            case VOL_MP3: {
                vol->type = VOL_MP3;
                bt_sink_srv_audio_setting_get_mp3_vol(&(vol_info->vol_info.mp3_vol_info), &(vol->vol.mp3_vol));
                break;
            }

            case VOL_DEF: {
                vol->type = VOL_DEF;
                bt_sink_srv_audio_setting_get_mp3_vol(&(vol_info->vol_info.mp3_vol_info), &(vol->vol.mp3_vol));
                break;
            }

            default:
                break;
        }
    }

    return 0;
}


void bt_sink_srv_audio_setting_update_voice_fillter_setting(bt_sink_srv_audio_setting_vol_info_t *vol_info,
        audio_eaps_t *am_speech_eaps)
{
    hfp_vol_info_t *vol_hfp = &(vol_info->vol_info.hfp_vol_info);
    device_t dev_out = vol_hfp->dev_out;
    device_t dev_in = vol_hfp->dev_in;
    //uint8_t lev_in = vol_hfp->lev_in;
    //uint8_t lev_out = vol_hfp->lev_out;
    uint8_t codec = vol_hfp->codec;
    int32_t ret = AUDIO_SETTING_ERR_SUCCESS_OK;
    uint8_t mode = 0;

#if PRODUCT_VERSION == 2533
    if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
        HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC == dev_in) {
        mode = SPEECH_MODE_HEADSET_DUAL_DIGITAL_MIC;
        speech_update_common(am_speech_eaps->speech_common_parameter.speech_common_parameter);
        speech_update_nb_fir(NULL,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[mode].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient);
        speech_update_nb_param(am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[mode].voice_nb_enhancement_parameter.nb_mode_parameter);
        speech_update_wb_fir(NULL,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[mode].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient);
        speech_update_wb_param(am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[mode].voice_wb_enhancement_parameter.wb_mode_parameter);
        ret = AUDIO_SETTING_ERR_SUCCESS_1ST;
    } else if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
        HAL_AUDIO_DEVICE_HEADSET_MIC == dev_in) {
        mode = SPEECH_MODE_HEADSET_HEADSET_MIC;
        speech_update_common(am_speech_eaps->speech_common_parameter.speech_common_parameter);
        speech_update_nb_fir((int16_t *)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter[mode].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter[mode].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient);
        speech_update_nb_param(am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter[mode].voice_nb_enhancement_parameter.nb_mode_parameter);
        speech_update_wb_fir((int16_t *)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter[mode].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter[mode].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient);
        speech_update_wb_param(am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter[mode].voice_wb_enhancement_parameter.wb_mode_parameter);
        ret = AUDIO_SETTING_ERR_SUCCESS_2ND;
    } else if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
        HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC == dev_in) {
        mode = SPEECH_MODE_HEADSET_SINGLE_DIGITAL_MIC;
        speech_update_common(am_speech_eaps->speech_common_parameter.speech_common_parameter);
        speech_update_nb_fir(NULL,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[mode].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient);
        speech_update_nb_param(am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[mode].voice_nb_enhancement_parameter.nb_mode_parameter);
        speech_update_wb_fir(NULL,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[mode].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient);
        speech_update_wb_param(am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[mode].voice_wb_enhancement_parameter.wb_mode_parameter);
        ret = AUDIO_SETTING_ERR_SUCCESS_3RD;
    } else {
        ret = AUDIO_SETTING_ERR_FAIL_1ST;
    }
#else
    if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
        ((HAL_AUDIO_DEVICE_HEADSET_MIC == dev_in) || (HAL_AUDIO_DEVICE_MAIN_MIC == dev_in))) {
        mode = SPEECH_MODE_HEADSET_HEADSET_MIC;
        ret = AUDIO_SETTING_ERR_SUCCESS_4TH;
    } else if (HAL_AUDIO_DEVICE_HANDS_FREE_STEREO == dev_out &&
               HAL_AUDIO_DEVICE_MAIN_MIC == dev_in) {
        mode = SPEECH_MODE_HANDSFREE_MAIN_MIC;
        ret = AUDIO_SETTING_ERR_SUCCESS_5TH;
    } else if (HAL_AUDIO_DEVICE_HEADSET == dev_out &&
               HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC == dev_in) {
        mode = SPEECH_MODE_HEADSET_DIGITAL_MIC;
        ret = AUDIO_SETTING_ERR_SUCCESS_6TH;
    } else {
        ret = AUDIO_SETTING_ERR_FAIL_2ND;
    }

    if (ret > AUDIO_SETTING_ERR_SUCCESS_OK) {
        speech_update_common(am_speech_eaps->speech_common_parameter.speech_common_parameter);
        speech_update_nb_fir((int16_t *)am_speech_eaps->voice_parameter.voice_nb_parameter[mode].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_nb_parameter[mode].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient);
        speech_update_nb_param(am_speech_eaps->voice_parameter.voice_nb_parameter[mode].voice_nb_enhancement_parameter.nb_mode_parameter);
        speech_update_wb_fir((int16_t *)am_speech_eaps->voice_parameter.voice_wb_parameter[mode].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_wb_parameter[mode].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient);
        speech_update_wb_param(am_speech_eaps->voice_parameter.voice_wb_parameter[mode].voice_wb_enhancement_parameter.wb_mode_parameter);
    }
#endif

    bt_sink_srv_report("[Sink][Setting]update_voice_fillter--dev_in: 0x%x, dev_out: 0x%x, lev_in: %d, lev_out: %d, codec: %d",
        vol_hfp->dev_in, vol_hfp->dev_out, vol_hfp->lev_in, vol_hfp->lev_out, codec);
    bt_sink_srv_report("[Sink][Setting]update_voice_fillter--ret: %d, mode: %d", ret, mode);
}

#endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */

